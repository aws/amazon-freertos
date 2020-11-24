/*
 * FreeRTOS V202011.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* Standard includes. */
#include <assert.h>

#include "http_demo_utils.h"

/* Retry utilities. */
#include "backoff_algorithm.h"

/* Include PKCS11 helpers header. */
#include "pkcs11_helpers.h"

/* Parser utilities. */
#include "http_parser.h"

/*-----------------------------------------------------------*/

/**
 * @brief The maximum number of retries for connecting to server.
 */
#define CONNECTION_RETRY_MAX_ATTEMPTS            ( 5U )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying connection to server.
 */
#define CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS    ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for connection retry attempts.
 */
#define CONNECTION_RETRY_BACKOFF_BASE_MS         ( 500U )

/*-----------------------------------------------------------*/

/**
 * @brief The random number generator to use for exponential backoff with
 * jitter retry logic.
 * This function is an implementation the #BackoffAlgorithm_RNG_t interface type
 * of the backoff algorithm library API.
 *
 * The PKCS11 module is used to generate the random number as it allows access
 * to a True Random Number Generator (TRNG) if the vendor platform supports it.
 * It is recommended to use a device-specific unique random number generator so
 * that probability of collisions from devices in connection retries is mitigated.
 *
 * @return A positive value if generating the random number was successful; otherwise
 * -1 to indicate failure.
 */
static int32_t prvGenerateRandomNumber();

/*-----------------------------------------------------------*/

static int32_t prvGenerateRandomNumber()
{
    uint32_t ulRandomNum;

    /* Use the PKCS11 module to generate a random number. */
    if( xPkcs11GenerateRandomNumber( ( uint8_t * ) &ulRandomNum,
                                     ( sizeof( ulRandomNum ) == pdPASS ) ) )
    {
        ulRandomNum = ( ulRandomNum & INT32_MAX );
    }
    else
    {
        /* Set the return value as negative to indicate failure. */
        ulRandomNum = -1;
    }

    return ( int32_t ) ulRandomNum;
}

/*-----------------------------------------------------------*/

BaseType_t connectToServerWithBackoffRetries( TransportConnect_t connectFunction,
                                              NetworkContext_t * pxNetworkContext )
{
    BaseType_t xReturn = pdFAIL;
    /* Status returned by the retry utilities. */
    BackoffAlgorithmStatus_t xBackoffAlgStatus = BackoffAlgorithmSuccess;
    /* Struct containing the next backoff time. */
    BackoffAlgorithmContext_t xReconnectParams;
    uint16_t usNextRetryBackOff = 0U;

    configASSERT( connectFunction != NULL );
    configASSERT( pxNetworkContext != NULL );

    /* Initialize reconnect attempts and interval. */
    BackoffAlgorithm_InitializeParams( &xReconnectParams,
                                       CONNECTION_RETRY_BACKOFF_BASE_MS,
                                       CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS,
                                       CONNECTION_RETRY_MAX_ATTEMPTS,
                                       prvGenerateRandomNumber );

    /* Attempt to connect to the HTTP server. If connection fails, retry after a
     * timeout. The timeout value will exponentially increase until either the
     * maximum timeout value is reached or the set number of attempts are
     * exhausted.*/
    do
    {
        xReturn = connectFunction( pxNetworkContext );

        if( xReturn != pdPASS )
        {
            /* Get back-off value (in milliseconds) for the next connection retry. */
            xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff( &xReconnectParams, &usNextRetryBackOff );
            configASSERT( xBackoffAlgStatus != BackoffAlgorithmRngFailure );

            if( xBackoffAlgStatus == BackoffAlgorithmRetriesExhausted )
            {
                LogError( ( "Connection to the server failed, all attempts exhausted." ) );
            }
            else if( xBackoffAlgStatus == BackoffAlgorithmSuccess )
            {
                LogWarn( ( "Connection to the HTTP server failed. "
                           "Retrying connection with backoff and jitter." ) );
                vTaskDelay( pdMS_TO_TICKS( usNextRetryBackOff ) );
                LogInfo( ( "Retry attempt %lu out of maximum retry attempts %lu.",
                           ( xReconnectParams.attemptsDone + 1 ),
                           MAX_RETRY_ATTEMPTS ) );
            }
        }
    } while( ( xReturn == pdFAIL ) && ( xBackoffAlgStatus == BackoffAlgorithmSuccess ) );

    return xReturn;
}

/*-----------------------------------------------------------*/

HTTPStatus_t getUrlPath( const char * pcUrl,
                         size_t xUrlLen,
                         const char ** pcPath,
                         size_t * pxPathLen )
{
    /* http-parser status. Initialized to 1 to signify failure. */
    int parserStatus = 1;
    struct http_parser_url urlParser;
    HTTPStatus_t xHTTPStatus = HTTPSuccess;

    /* Sets all members in urlParser to 0. */
    http_parser_url_init( &urlParser );

    if( ( pcUrl == NULL ) || ( pcPath == NULL ) || ( pxPathLen == NULL ) )
    {
        LogError( ( "NULL parameter passed to getUrlPath()." ) );
        xHTTPStatus = HTTPInvalidParameter;
    }

    if( xHTTPStatus == HTTPSuccess )
    {
        parserStatus = http_parser_parse_url( pcUrl, xUrlLen, 0, &urlParser );

        if( parserStatus != 0 )
        {
            LogError( ( "Error parsing the input URL %.*s. Error code: %d.",
                        ( int32_t ) xUrlLen,
                        pcUrl,
                        parserStatus ) );
            xHTTPStatus = HTTPParserInternalError;
        }
    }

    if( xHTTPStatus == HTTPSuccess )
    {
        *pxPathLen = ( size_t ) ( urlParser.field_data[ UF_PATH ].len );

        if( *pxPathLen == 0 )
        {
            xHTTPStatus = HTTPNoResponse;
            *pcPath = NULL;
        }
        else
        {
            *pcPath = &pcUrl[ urlParser.field_data[ UF_PATH ].off ];
        }
    }

    if( xHTTPStatus != HTTPSuccess )
    {
        LogError( ( "Error parsing the path from URL %s. Error code: %d",
                    pcUrl,
                    xHTTPStatus ) );
    }

    return xHTTPStatus;
}

/*-----------------------------------------------------------*/

HTTPStatus_t getUrlAddress( const char * pcUrl,
                            size_t xUrlLen,
                            const char ** pcAddress,
                            size_t * pxAddressLen )
{
    /* http-parser status. Initialized to 1 to signify failure. */
    int parserStatus = 1;
    struct http_parser_url urlParser;
    HTTPStatus_t xHTTPStatus = HTTPSuccess;

    /* Sets all members in urlParser to 0. */
    http_parser_url_init( &urlParser );

    if( ( pcUrl == NULL ) || ( pcAddress == NULL ) || ( pxAddressLen == NULL ) )
    {
        LogError( ( "NULL parameter passed to getUrlAddress()." ) );
        xHTTPStatus = HTTPInvalidParameter;
    }

    if( xHTTPStatus == HTTPSuccess )
    {
        parserStatus = http_parser_parse_url( pcUrl, xUrlLen, 0, &urlParser );

        if( parserStatus != 0 )
        {
            LogError( ( "Error parsing the input URL %.*s. Error code: %d.",
                        ( int32_t ) xUrlLen,
                        pcUrl,
                        parserStatus ) );
            xHTTPStatus = HTTPParserInternalError;
        }
    }

    if( xHTTPStatus == HTTPSuccess )
    {
        *pxAddressLen = ( size_t ) ( urlParser.field_data[ UF_HOST ].len );

        if( *pxAddressLen == 0 )
        {
            xHTTPStatus = HTTPNoResponse;
            *pcAddress = NULL;
        }
        else
        {
            *pcAddress = &pcUrl[ urlParser.field_data[ UF_HOST ].off ];
        }
    }

    if( xHTTPStatus != HTTPSuccess )
    {
        LogError( ( "Error parsing the address from URL %s. Error code %d",
                    pcUrl,
                    xHTTPStatus ) );
    }

    return xHTTPStatus;
}
