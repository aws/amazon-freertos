/*
 * FreeRTOS V202203.00
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

/**
 * @brief The separator between the "https" scheme and the host in a URL.
 */
#define SCHEME_SEPARATOR                         "://"

/**
 * @brief The length of the "://" separator.
 */
#define SCHEME_SEPARATOR_LEN                     ( sizeof( SCHEME_SEPARATOR ) - 1 )

/*-----------------------------------------------------------*/

/**
 * @brief Calculate and perform an exponential backoff with jitter delay for
 * the next retry attempt of a failed network operation with the server.
 *
 * The function generates a random number, calculates the next backoff period
 * with the generated random number, and performs the backoff delay operation if the
 * number of retries have not exhausted.
 *
 * @note The PKCS11 module is used to generate the random number as it allows access
 * to a True Random Number Generator (TRNG) if the vendor platform supports it.
 * It is recommended to seed the random number generator with a device-specific entropy
 * source so that probability of collisions from devices in connection retries is mitigated.
 *
 * @note The backoff period is calculated using the backoffAlgorithm library.
 *
 * @param[in, out] pxRetryAttempts The context to use for backoff period calculation
 * with the backoffAlgorithm library.
 *
 * @return pdPASS if calculating the backoff period was successful; otherwise pdFAIL
 * if there was failure in random number generation OR all retry attempts had exhausted.
 */
static BaseType_t prvBackoffForRetry( BackoffAlgorithmContext_t * pxRetryParams );

/*-----------------------------------------------------------*/

static BaseType_t prvBackoffForRetry( BackoffAlgorithmContext_t * pxRetryParams )
{
    BaseType_t xReturnStatus = pdFAIL;
    uint16_t usNextRetryBackOff = 0U;
    BackoffAlgorithmStatus_t xBackoffAlgStatus = BackoffAlgorithmSuccess;

    /**
     * To calculate the backoff period for the next retry attempt, we will
     * generate a random number to provide to the backoffAlgorithm library.
     *
     * Note: The PKCS11 module is used to generate the random number as it allows access
     * to a True Random Number Generator (TRNG) if the vendor platform supports it.
     * It is recommended to use a random number generator seeded with a device-specific
     * entropy source so that probability of collisions from devices in connection retries
     * is mitigated.
     */
    uint32_t ulRandomNum = 0;

    if( xPkcs11GenerateRandomNumber( ( uint8_t * ) &ulRandomNum,
                                     sizeof( ulRandomNum ) ) == pdPASS )
    {
        /* Get back-off value (in milliseconds) for the next retry attempt. */
        xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff( pxRetryParams, ulRandomNum, &usNextRetryBackOff );

        if( xBackoffAlgStatus == BackoffAlgorithmRetriesExhausted )
        {
            LogError( ( "All retry attempts have exhausted. Operation will not be retried" ) );
        }
        else if( xBackoffAlgStatus == BackoffAlgorithmSuccess )
        {
            /* Perform the backoff delay. */
            vTaskDelay( pdMS_TO_TICKS( usNextRetryBackOff ) );

            xReturnStatus = pdPASS;

            LogInfo( ( "Retry attempt %lu out of maximum retry attempts %lu.",
                       ( pxRetryParams->attemptsDone + 1 ),
                       pxRetryParams->maxRetryAttempts ) );
        }
    }
    else
    {
        LogError( ( "Unable to retry operation with broker: Random number generation failed" ) );
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

BaseType_t connectToServerWithBackoffRetries( TransportConnect_t connectFunction,
                                              NetworkContext_t * pxNetworkContext )
{
    BaseType_t xReturn = pdFAIL;
    /* Struct containing the next backoff time. */
    BackoffAlgorithmContext_t xReconnectParams;
    BaseType_t xBackoffStatus = 0U;

    configASSERT( connectFunction != NULL );
    configASSERT( pxNetworkContext != NULL );

    /* Initialize reconnect attempts and interval. */
    BackoffAlgorithm_InitializeParams( &xReconnectParams,
                                       CONNECTION_RETRY_BACKOFF_BASE_MS,
                                       CONNECTION_RETRY_MAX_BACKOFF_DELAY_MS,
                                       CONNECTION_RETRY_MAX_ATTEMPTS );

    /* Attempt to connect to the HTTP server. If connection fails, retry after a
     * timeout. The timeout value will exponentially increase until either the
     * maximum timeout value is reached or the set number of attempts are
     * exhausted.*/
    do
    {
        xReturn = connectFunction( pxNetworkContext );

        if( xReturn != pdPASS )
        {
            LogWarn( ( "Connection to the HTTP server failed. "
                       "Retrying connection with backoff and jitter." ) );

            /* As the connection attempt failed, we will retry the connection after an
             * exponential backoff with jitter delay. */

            /* Calculate the backoff period for the next retry attempt and perform the wait operation. */
            xBackoffStatus = prvBackoffForRetry( &xReconnectParams );
        }
    } while( ( xReturn == pdFAIL ) && ( xBackoffStatus == pdPASS ) );

    return xReturn;
}

/*-----------------------------------------------------------*/

HTTPStatus_t getUrlPath( const char * pcUrl,
                         size_t xUrlLen,
                         const char ** pcPath,
                         size_t * pxPathLen )
{
    HTTPStatus_t xHttpStatus = HTTPSuccess;
    const char * pcHostStart = NULL;
    const char * pcPathStart = NULL;
    size_t xHostLen = 0, i = 0, xPathStartIndex = 0, xPathLen = 0;

    if( ( pcUrl == NULL ) || ( pcPath == NULL ) || ( pxPathLen == NULL ) )
    {
        LogError( ( "NULL parameter passed to getUrlPath()." ) );
        xHttpStatus = HTTPInvalidParameter;
    }

    if( xHttpStatus == HTTPSuccess )
    {
        xHttpStatus = getUrlAddress( pcUrl, xUrlLen, &pcHostStart, &xHostLen );
    }

    if( xHttpStatus == HTTPSuccess )
    {
        /* Search for the start of the path. */
        for( i = ( pcHostStart - pcUrl ) + xHostLen; i < xUrlLen; i++ )
        {
            if( pcUrl[ i ] == '/' )
            {
                pcPathStart = &pcUrl[ i ];
                xPathStartIndex = i;
                break;
            }
        }

        if( pcPathStart != NULL )
        {
            /* The end of the path will be either the start of the query,
             * start of the fragment, or end of the URL. If this is an S3
             * presigned URL, then there must be a query. */
            for( i = xPathStartIndex; i < xUrlLen; i++ )
            {
                if( pcUrl[ i ] == '?' )
                {
                    break;
                }
            }

            xPathLen = i - xPathStartIndex;
        }

        if( xPathLen == 0 )
        {
            LogError( ( "Could not parse path from input URL %.*s",
                        ( int32_t ) xUrlLen,
                        pcUrl ) );
            xHttpStatus = HTTPNoResponse;
        }
    }

    if( xHttpStatus == HTTPSuccess )
    {
        *pxPathLen = xPathLen;
        *pcPath = pcPathStart;
    }

    if( xHttpStatus != HTTPSuccess )
    {
        LogError( ( "Error parsing the path from URL %s. Error code: %d",
                    pcUrl,
                    xHttpStatus ) );
    }

    return xHttpStatus;
}

/*-----------------------------------------------------------*/

HTTPStatus_t getUrlAddress( const char * pcUrl,
                            size_t xUrlLen,
                            const char ** pcAddress,
                            size_t * pxAddressLen )
{
    HTTPStatus_t xHttpStatus = HTTPSuccess;
    const char * pcHostStart = NULL;
    const char * pcHostEnd = NULL;
    size_t i = 0, xHostLen = 0;

    if( ( pcUrl == NULL ) || ( pcAddress == NULL ) || ( pxAddressLen == NULL ) )
    {
        LogError( ( "NULL parameter passed to getUrlAddress()." ) );
        xHttpStatus = HTTPInvalidParameter;
    }

    if( xHttpStatus == HTTPSuccess )
    {
        /* Search for the start of the hostname using the "://" separator. */
        for( i = 0; i < ( xUrlLen - SCHEME_SEPARATOR_LEN ); i++ )
        {
            if( strncmp( &( pcUrl[ i ] ), SCHEME_SEPARATOR, SCHEME_SEPARATOR_LEN ) == 0 )
            {
                pcHostStart = pcUrl + i + SCHEME_SEPARATOR_LEN;
                break;
            }
        }

        if( pcHostStart == NULL )
        {
            LogError( ( "Could not find \"://\" scheme separator in input URL %.*s",
                        ( int32_t ) xUrlLen,
                        pcUrl ) );
            xHttpStatus = HTTPParserInternalError;
        }
        else
        {
            /* Search for the end of the hostname assuming that the object path
             * is next. Assume that there is no port number as this is used for
             * S3 presigned URLs. */
            for( pcHostEnd = pcHostStart; pcHostEnd < ( pcUrl + xUrlLen ); pcHostEnd++ )
            {
                if( *pcHostEnd == '/' )
                {
                    xHostLen = ( size_t ) ( pcHostEnd - pcHostStart );
                    break;
                }
            }
        }
    }

    if( xHttpStatus == HTTPSuccess )
    {
        *pxAddressLen = xHostLen;

        if( xHostLen == 0 )
        {
            LogError( ( "Could not find end of host in input URL %.*s",
                        ( int32_t ) xUrlLen,
                        pcUrl ) );
            xHttpStatus = HTTPNoResponse;
            *pcAddress = NULL;
        }
        else
        {
            *pcAddress = pcHostStart;
        }
    }

    if( xHttpStatus != HTTPSuccess )
    {
        LogError( ( "Error parsing the address from URL %s. Error code %d",
                    pcUrl,
                    xHttpStatus ) );
    }

    return xHttpStatus;
}
