/*
 * FreeRTOS Secure Sockets for STM32L4 Discovery kit IoT node V1.0.0 Beta 4
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/**
 * @file iot_secure_sockets.c
 * @brief WiFi and Secure Socket interface implementation for ST board.
 */

/* Define _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE to prevent secure sockets functions
 * from redefining in iot_secure_sockets_wrapper_metrics.h */
#define _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* TLS includes. */
#include "iot_tls.h"

/* WiFi driver includes. */
#include "es_wifi.h"
#include "es_wifi_io.h"

/* Socket and WiFi interface includes. */
#include "iot_secure_sockets.h"
#include "iot_wifi.h"

/* WiFi configuration includes. */
#include "aws_wifi_config.h"

/* Credentials includes. */
#include "aws_clientcredential.h"
#include "iot_default_root_certificates.h"

#undef _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE

/**
 * @brief A Flag to indicate whether or not a socket is
 * secure i.e. it uses TLS or not.
 */
#define stsecuresocketsSOCKET_SECURE_FLAG          ( 1UL << 0 )

/**
 * @brief A flag to indicate whether or not a socket is closed
 * for receive.
 */
#define stsecuresocketsSOCKET_READ_CLOSED_FLAG     ( 1UL << 1 )

/**
 * @brief A flag to indicate whether or not a socket is closed
 * for send.
 */
#define stsecuresocketsSOCKET_WRITE_CLOSED_FLAG    ( 1UL << 2 )

/**
 * @brief A flag to indicate whether or not the socket is connected.
 */
#define stsecuresocketsSOCKET_IS_CONNECTED_FLAG    ( 1UL << 3 )

/**
 * @brief The maximum timeout accepted by the Inventek module.
 *
 * This value is dictated by the hardware and should not be
 * modified.
 */
#define stsecuresocketsMAX_TIMEOUT                 ( 30000 )

/**
 * @brief Delay used between network read attempts when effecting a receive timeout.
 *
 * If receive timeouts are implemented by the Inventek module then
 * the SPI driver will poll for extended periods, preventing lower
 * priority tasks from executing.  Therefore timeouts are mocked in
 * the secure sockets layer, and this constant sets the sleep time
 * between each read attempt during the receive timeout period.
 */
#define stsecuresocketsFIVE_MILLISECONDS           ( pdMS_TO_TICKS( 5 ) )

/**
 * @brief The timeout supplied to the Inventek module in receive operation.
 *
 * Receive timeout are emulated in secure sockets layer and therefore we
 * do not want the Inventek module to block. Setting to zero means
 * no timeout, so one is the smallest value we can set it to.
 */
#define stsecuresocketsONE_MILLISECOND             ( 1 )

/**
 * @brief The credential set to use for TLS on the Inventek module.
 *
 * @note This is hard-coded to 3 because we are using re-writable
 * credential slot.
 */
#define stsecuresocketsOFFLOAD_SSL_CREDS_SLOT      ( 3 )
/*-----------------------------------------------------------*/

/**
 * @brief Represents the WiFi module.
 *
 * Since there is only one WiFi module on the ST board, only
 * one instance of this type is needed. All the operations on
 * the WiFi module must be serialized because a single operation
 * (like socket connect, send etc) consists of multiple AT Commands
 * sent over the same SPI bus. A semaphore is therefore used to
 * serialize all the operations.
 */
typedef struct STWiFiModule
{
    ES_WIFIObject_t xWifiObject;        /**< Internal WiFi object. */
    SemaphoreHandle_t xSemaphoreHandle; /**< Semaphore used to serialize all the operations on the WiFi module. */
} STWiFiModule_t;

/**
 * @brief Represents a secure socket.
 */
typedef struct STSecureSocket
{
    uint8_t ucInUse;                    /**< Tracks whether the socket is in use or not. */
    ES_WIFI_ConnType_t xSocketType;     /**< Type of the socket. @see ES_WIFI_ConnType_t. */
    uint32_t ulFlags;                   /**< Various properties of the socket (secured etc.). */
    uint32_t ulSendTimeout;             /**< Send timeout. */
    uint32_t ulReceiveTimeout;          /**< Receive timeout. */
    char * pcDestination;               /**< Destination URL. Set using SOCKETS_SO_SERVER_NAME_INDICATION option in SOCKETS_SetSockOpt function. */
    void * pvTLSContext;                /**< The TLS Context. */
    char * pcServerCertificate;         /**< Server certificate. Set using SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE option in SOCKETS_SetSockOpt function. */
    uint32_t ulServerCertificateLength; /**< Length of the server certificate. */
} STSecureSocket_t;
/*-----------------------------------------------------------*/

/**
 * @brief Secure socket objects.
 *
 * An index in this array is returned to the user from SOCKETS_Socket
 * function.
 */
static STSecureSocket_t xSockets[ wificonfigMAX_SOCKETS ];

/**
 * @brief WiFi module object.
 *
 * Since the ST board contains only one WiFi module, only one instance
 * is needed and there is no need to pass this to the user.
 */
extern STWiFiModule_t xWiFiModule;

/**
 * @brief Maximum time to wait in ticks for obtaining the WiFi semaphore
 * before failing the operation.
 */
static const TickType_t xSemaphoreWaitTicks = pdMS_TO_TICKS( wificonfigMAX_SEMAPHORE_WAIT_TIME_MS );
/*-----------------------------------------------------------*/

/**
 * @brief Get a free socket from the free socket pool.
 *
 * Iterates over the xSockets array to see if it can find
 * a free socket. A free or unused socket is indicated by
 * the zero value of the ucInUse member of STSecureSocket_t.
 *
 * @return Index of the socket in the xSockets array, if it is
 * able to find a free socket, SOCKETS_INVALID_SOCKET otherwise.
 */
static uint32_t prvGetFreeSocket( void );

/**
 * @brief Returns the socket back to the free socket pool.
 *
 * Marks the socket as free by setting ucInUse member of the
 * STSecureSocket_t structure as zero.
 */
static void prvReturnSocket( uint32_t ulSocketNumber );

/**
 * @brief Checks whether or not the provided socket number is valid.
 *
 * Ensures that the provided number is less than wificonfigMAX_SOCKETS
 * and the socket is "in-use" i.e. ucInUse is set to non-zero in the
 * socket structure.
 *
 * @param[in] ulSocketNumber The provided socket number to check.
 *
 * @return pdTRUE if the socket is valid, pdFALSE otherwise.
 */
static BaseType_t prvIsValidSocket( uint32_t ulSocketNumber );

/**
 * @brief Sends the provided data over WiFi.
 *
 * @param[in] pvContext The caller context. Socket number in our case.
 * @param[in] pucData The data to send.
 * @param[in] xDataLength Length of the data.
 *
 * @return Number of bytes actually sent if successful, SOCKETS_SOCKET_ERROR
 * otherwise.
 */
static BaseType_t prvNetworkSend( void * pvContext,
                                  const unsigned char * pucData,
                                  size_t xDataLength );

/**
 * @brief Receives the data over WiFi.
 *
 * @param[in] pvContext The caller context. Socket number in our case.
 * @param[out] pucReceiveBuffer The buffer to receive the data in.
 * @param[in] xReceiveBufferLength The length of the provided buffer.
 *
 * @return The number of bytes actually received if successful, SOCKETS_SOCKET_ERROR
 * otherwise.
 */
static BaseType_t prvNetworkRecv( void * pvContext,
                                  unsigned char * pucReceiveBuffer,
                                  size_t xReceiveBufferLength );
/*-----------------------------------------------------------*/

static uint32_t prvGetFreeSocket( void )
{
    uint32_t ulIndex;

    /* Iterate over xSockets array to see if any free socket
     * is available. */
    for( ulIndex = 0; ulIndex < ( uint32_t ) wificonfigMAX_SOCKETS; ulIndex++ )
    {
        /* Since multiple tasks can be accessing this simultaneously,
         * this has to be in critical section. */
        taskENTER_CRITICAL();

        if( xSockets[ ulIndex ].ucInUse == 0U )
        {
            /* Mark the socket as "in-use". */
            xSockets[ ulIndex ].ucInUse = 1;
            taskEXIT_CRITICAL();

            /* We have found a free socket, so stop. */
            break;
        }
        else
        {
            taskEXIT_CRITICAL();
        }
    }

    /* Did we find a free socket? */
    if( ulIndex == ( uint32_t ) wificonfigMAX_SOCKETS )
    {
        /* Return SOCKETS_INVALID_SOCKET if we fail to
         * find a free socket. */
        ulIndex = ( uint32_t ) SOCKETS_INVALID_SOCKET;
    }

    return ulIndex;
}
/*-----------------------------------------------------------*/

static void prvReturnSocket( uint32_t ulSocketNumber )
{
    /* Since multiple tasks can be accessing this simultaneously,
     * this has to be in critical section. */
    taskENTER_CRITICAL();
    {
        /* Mark the socket as free. */
        xSockets[ ulSocketNumber ].ucInUse = 0;
    }
    taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

static BaseType_t prvIsValidSocket( uint32_t ulSocketNumber )
{
    BaseType_t xValid = pdFALSE;

    /* Check that the provided socket number is within the valid
     * index range. */
    if( ulSocketNumber < ( uint32_t ) wificonfigMAX_SOCKETS )
    {
        /* Since multiple tasks can be accessing this simultaneously,
         * this has to be in critical section. */
        taskENTER_CRITICAL();
        {
            /* Check that this socket is in use. */
            if( xSockets[ ulSocketNumber ].ucInUse == 1U )
            {
                /* This is a valid socket number. */
                xValid = pdTRUE;
            }
        }
        taskEXIT_CRITICAL();
    }

    return xValid;
}
/*-----------------------------------------------------------*/

static BaseType_t prvNetworkSend( void * pvContext,
                                  const unsigned char * pucData,
                                  size_t xDataLength )
{
    uint32_t ulSocketNumber = ( uint32_t ) pvContext; /*lint !e923 cast is necessary for port. */
    STSecureSocket_t * pxSecureSocket;
    uint16_t usSentBytes = 0;
    BaseType_t xRetVal = SOCKETS_SOCKET_ERROR;
    ES_WIFI_Status_t xWiFiResult;

    /* Shortcut for easy access. */
    pxSecureSocket = &( xSockets[ ulSocketNumber ] );

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        /* Since WiFi module has only one timeout, this needs
         * to be set per send and receive operation to the
         * respective send or receive timeout. Also, this
         * must be done after acquiring the semaphore as the
         * xWiFiModule is a shared object.*/
        if( pxSecureSocket->ulSendTimeout == 0 )
        {
            /* Set the SPI timeout to the maximum uint32_t value.
             * This is a little over 49 days. */
            xWiFiModule.xWifiObject.Timeout = 0xFFFFFFFF;
        }
        else
        {
            /* The maximum timeout for Inventek module is 30 seconds.
             * This timeout is about 65 seconds, so the module should
             * timeout before the SPI. */
            xWiFiModule.xWifiObject.Timeout = ES_WIFI_TIMEOUT;
        }

        /* Send the data. */
        xWiFiResult = ES_WIFI_SendData( &( xWiFiModule.xWifiObject ),
                                        ( uint8_t ) ulSocketNumber,
                                        ( uint8_t * ) pucData, /*lint !e9005 STM function does not use const. */
                                        ( uint16_t ) xDataLength,
                                        &( usSentBytes ),
                                        pxSecureSocket->ulSendTimeout );

        if( xWiFiResult == ES_WIFI_STATUS_OK )
        {
            /* If the data was successfully sent, return the actual
             * number of bytes sent. Otherwise return SOCKETS_SOCKET_ERROR. */
            xRetVal = ( BaseType_t ) usSentBytes;
        }

        /* Return the semaphore. */
        ( void ) xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
    }

    /* The following code attempts to revive the Inventek WiFi module
     * from its unusable state.*/
    if( xWiFiResult == ES_WIFI_STATUS_IO_ERROR )
    {
        /* Reset the WiFi Module. Since the WIFI_Reset function
         * acquires the same semaphore, we must not acquire
         * it. */
        if( WIFI_Reset() == eWiFiSuccess )
        {
            /* Try to acquire the semaphore. */
            if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, portMAX_DELAY ) == pdTRUE )
            {
                /* Reinitialize the socket structures which
                 * marks all sockets as closed and free. */
                SOCKETS_Init();

                /* Return the semaphore. */
                ( void ) xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
            }

            /* Set the error code to indicate that
             * WiFi needs to be reconnected to network. */
            xRetVal = SOCKETS_PERIPHERAL_RESET;
        }
    }

    /* To allow other tasks of equal priority that are using this API to run as
     * a switch to an equal priority task that is waiting for the mutex will
     * only otherwise occur in the tick interrupt - at which point the mutex
     * might have been taken again by the currently running task.
     */
    taskYIELD();

    return xRetVal;
}
/*-----------------------------------------------------------*/

static BaseType_t prvNetworkRecv( void * pvContext,
                                  unsigned char * pucReceiveBuffer,
                                  size_t xReceiveBufferLength )
{
    uint32_t ulSocketNumber = ( uint32_t ) pvContext; /*lint !e923 cast is needed for portability. */
    STSecureSocket_t * pxSecureSocket;
    uint16_t usReceivedBytes = 0;
    BaseType_t xRetVal;
    ES_WIFI_Status_t xWiFiResult;
    TickType_t xTimeOnEntering = xTaskGetTickCount(), xSemaphoreWait;

    /* Shortcut for easy access. */
    pxSecureSocket = &( xSockets[ ulSocketNumber ] );

    /* WiFi module does not support receiving more than ES_WIFI_PAYLOAD_SIZE
     * bytes at a time. */
    if( xReceiveBufferLength > ( uint32_t ) ES_WIFI_PAYLOAD_SIZE )
    {
        xReceiveBufferLength = ( uint32_t ) ES_WIFI_PAYLOAD_SIZE;
    }

    xSemaphoreWait = pxSecureSocket->ulReceiveTimeout + stsecuresocketsFIVE_MILLISECONDS;

    for( ; ; )
    {
        /* Try to acquire the semaphore. */
        if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWait ) == pdTRUE )
        {
            /* Since WiFi module has only one timeout, this needs
             * to be set per send and receive operation to the
             * respective send or receive timeout. Also, this
             * must be done after acquiring the semaphore as the
             * xWiFiModule is a shared object.*/
            if( pxSecureSocket->ulReceiveTimeout == 0 )
            {
                /* Set the SPI timeout to the maximum uint32_t value.
                 * This is a little over 49 days. */
                xWiFiModule.xWifiObject.Timeout = 0xFFFFFFFF;
            }
            else
            {
                /* The maximum timeout for Inventek module is 30 seconds.
                 * This timeout is about 65 seconds, so the module should
                 * timeout before the SPI. */
                xWiFiModule.xWifiObject.Timeout = ES_WIFI_TIMEOUT;
            }

            /* Receive the data. */
            xWiFiResult = ES_WIFI_ReceiveData( &( xWiFiModule.xWifiObject ),
                                               ( uint8_t ) ulSocketNumber,
                                               ( uint8_t * ) pucReceiveBuffer,
                                               ( uint16_t ) xReceiveBufferLength,
                                               &( usReceivedBytes ),
                                               stsecuresocketsONE_MILLISECOND );

            /* Return the semaphore. */
            ( void ) xSemaphoreGive( xWiFiModule.xSemaphoreHandle );

            if( ( xWiFiResult == ES_WIFI_STATUS_OK ) && ( usReceivedBytes != 0 ) )
            {
                /* Success, return the number of bytes received. */
                xRetVal = ( BaseType_t ) usReceivedBytes;
                break;
            }
            else if( ( xWiFiResult == ES_WIFI_STATUS_TIMEOUT ) || ( ( xWiFiResult == ES_WIFI_STATUS_OK ) && ( usReceivedBytes == 0 ) ) )
            {
                /* The WiFi poll timed out, but has the socket timeout expired
                 * too? */
                if( ( xTaskGetTickCount() - xTimeOnEntering ) < pxSecureSocket->ulReceiveTimeout )
                {
                    /* The socket has not timed out, but the driver supplied
                     * with the board is polling, which would block other tasks, so
                     * block for a short while to allow other tasks to run before
                     * trying again. */
                    vTaskDelay( stsecuresocketsFIVE_MILLISECONDS );
                }
                else
                {
                    /* The socket read has timed out too. Returning
                     * SOCKETS_EWOULDBLOCK will cause mBedTLS to fail
                     * and so we must return zero. */
                    xRetVal = 0;
                    break;
                }
            }
            else
            {
                /* xWiFiResult contains an error status. */
                xRetVal = SOCKETS_SOCKET_ERROR;
                break;
            }
        }
        else
        {
            /* Semaphore wait time was longer than the receive timeout so this
             * is also a socket timeout. Returning SOCKETS_EWOULDBLOCK will
             * cause mBedTLS to fail and so we must return zero.*/
            xRetVal = 0;
            break;
        }
    }

    /* The following code attempts to revive the Inventek WiFi module
     * from its unusable state.*/
    if( xWiFiResult == ES_WIFI_STATUS_IO_ERROR )
    {
        /* Reset the WiFi Module. Since the WIFI_Reset function
         * acquires the same semaphore, we must not acquire
         * it. */
        if( WIFI_Reset() == eWiFiSuccess )
        {
            /* Try to acquire the semaphore. */
            if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, portMAX_DELAY ) == pdTRUE )
            {
                /* Reinitialize the socket structures which
                 * marks all sockets as closed and free. */
                SOCKETS_Init();

                /* Return the semaphore. */
                ( void ) xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
            }

            /* Set the error code to indicate that
             * WiFi needs to be reconnected to network. */
            xRetVal = SOCKETS_PERIPHERAL_RESET;
        }
    }

    return xRetVal;
}
/*-----------------------------------------------------------*/

Socket_t SOCKETS_Socket( int32_t lDomain,
                         int32_t lType,
                         int32_t lProtocol )
{
    uint32_t ulSocketNumber;

    /* Ensure that only supported values are supplied. */
    configASSERT( lDomain == SOCKETS_AF_INET );
    configASSERT( ( lType == SOCKETS_SOCK_STREAM && lProtocol == SOCKETS_IPPROTO_TCP ) );

    /* Try to get a free socket. */
    ulSocketNumber = prvGetFreeSocket();

    /* If we get a free socket, set its attributes. */
    if( ulSocketNumber != ( uint32_t ) SOCKETS_INVALID_SOCKET )
    {
        /* Store the socket type. */
        xSockets[ ulSocketNumber ].xSocketType = ES_WIFI_TCP_CONNECTION;

        /* Initialize all the members to sane values. */
        xSockets[ ulSocketNumber ].ulFlags = 0;
        xSockets[ ulSocketNumber ].ulSendTimeout = socketsconfigDEFAULT_SEND_TIMEOUT;
        xSockets[ ulSocketNumber ].ulReceiveTimeout = socketsconfigDEFAULT_RECV_TIMEOUT;
        xSockets[ ulSocketNumber ].pcDestination = NULL;
        xSockets[ ulSocketNumber ].pvTLSContext = NULL;
        xSockets[ ulSocketNumber ].pcServerCertificate = NULL;
        xSockets[ ulSocketNumber ].ulServerCertificateLength = 0;
    }

    /* If we fail to get a free socket, we return SOCKETS_INVALID_SOCKET. */
    return ( Socket_t ) ulSocketNumber; /*lint !e923 cast required for portability. */
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Connect( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t xAddressLength )
{
    uint32_t ulSocketNumber = ( uint32_t ) xSocket; /*lint !e923 cast required for portability. */
    STSecureSocket_t * pxSecureSocket;
    ES_WIFI_Conn_t xWiFiConnection;
    int32_t lRetVal = SOCKETS_ERROR_NONE;

    #ifndef USE_OFFLOAD_SSL
        TLSParams_t xTLSParams = { 0 };
    #endif /* USE_OFFLOAD_SSL */

    /* Ensure that a valid socket was passed. */
    if( prvIsValidSocket( ulSocketNumber ) == pdTRUE )
    {
        /* Shortcut for easy access. */
        pxSecureSocket = &( xSockets[ ulSocketNumber ] );

        /* Check that the socket is not already connected. */
        if( ( pxSecureSocket->ulFlags & stsecuresocketsSOCKET_IS_CONNECTED_FLAG ) != 0UL )
        {
            /* Connect attempted on an already connected socket. */
            lRetVal = SOCKETS_SOCKET_ERROR;
        }

        /* Try to acquire the semaphore. */
        if( ( lRetVal == SOCKETS_ERROR_NONE ) &&
            ( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE ) )
        {
            /* Store server certificate if we are using offload SSL
             * and the socket is a secure socket. */
            #ifdef USE_OFFLOAD_SSL
                if( ( pxSecureSocket->ulFlags & stsecuresocketsSOCKET_SECURE_FLAG ) != 0UL )
                {
                    /* Store the custom certificate if needed. */
                    if( pxSecureSocket->pcServerCertificate != NULL )
                    {
                        if( ES_WIFI_StoreCA( &( xWiFiModule.xWifiObject ),
                                             ES_WIFI_FUNCTION_TLS,
                                             stsecuresocketsOFFLOAD_SSL_CREDS_SLOT,
                                             ( uint8_t * ) pxSecureSocket->pcServerCertificate,
                                             ( uint16_t ) pxSecureSocket->ulServerCertificateLength ) == ES_WIFI_STATUS_OK )
                        {
                            /* Certificate stored successfully. */
                            lRetVal = SOCKETS_ERROR_NONE;
                        }
                        else
                        {
                            /* Failed to store certificate. */
                            lRetVal = SOCKETS_SOCKET_ERROR;
                        }
                    }
                    else
                    {
                        /* If we are not using the ATS endpoint, use the VeriSign root CA. Otherwise
                         * use the Starfield root CA. */
                        if( strstr( clientcredentialMQTT_BROKER_ENDPOINT, "-ats.iot" ) == NULL )
                        {
                            /* Store the default certificate. */
                            if( ES_WIFI_StoreCA( &( xWiFiModule.xWifiObject ),
                                                 ES_WIFI_FUNCTION_TLS,
                                                 stsecuresocketsOFFLOAD_SSL_CREDS_SLOT,
                                                 ( uint8_t * ) tlsVERISIGN_ROOT_CERTIFICATE_PEM,
                                                 ( uint16_t ) tlsVERISIGN_ROOT_CERTIFICATE_LENGTH ) == ES_WIFI_STATUS_OK )
                            {
                                /* Certificate stored successfully. */
                                lRetVal = SOCKETS_ERROR_NONE;
                            }
                            else
                            {
                                /* Failed to store certificate. */
                                lRetVal = SOCKETS_SOCKET_ERROR;
                            }
                        }
                        else
                        {
                            /* Store the default certificate. */
                            if( ES_WIFI_StoreCA( &( xWiFiModule.xWifiObject ),
                                                 ES_WIFI_FUNCTION_TLS,
                                                 stsecuresocketsOFFLOAD_SSL_CREDS_SLOT,
                                                 ( uint8_t * ) tlsSTARFIELD_ROOT_CERTIFICATE_PEM,
                                                 ( uint16_t ) tlsSTARFIELD_ROOT_CERTIFICATE_LENGTH ) == ES_WIFI_STATUS_OK )
                            {
                                /* Certificate stored successfully. */
                                lRetVal = SOCKETS_ERROR_NONE;
                            }
                            else
                            {
                                /* Failed to store certificate. */
                                lRetVal = SOCKETS_SOCKET_ERROR;
                            }
                        }
                    }
                }
            #endif /* USE_OFFLOAD_SSL */

            if( lRetVal == SOCKETS_ERROR_NONE )
            {
                /* Setup connection parameters. */
                xWiFiConnection.Number = ( uint8_t ) ulSocketNumber;
                xWiFiConnection.Type = pxSecureSocket->xSocketType;
                xWiFiConnection.RemotePort = SOCKETS_ntohs( pxAddress->usPort ); /* WiFi Module expects the port number in host byte order. */
                memcpy( &( xWiFiConnection.RemoteIP ),
                        &( pxAddress->ulAddress ),
                        sizeof( xWiFiConnection.RemoteIP ) );
                xWiFiConnection.LocalPort = 0;
                xWiFiConnection.Name = NULL;

                /* Start the client connection. */
                if( ES_WIFI_StartClientConnection( &( xWiFiModule.xWifiObject ), &( xWiFiConnection ) ) == ES_WIFI_STATUS_OK )
                {
                    /* Successful connection is established. */
                    lRetVal = SOCKETS_ERROR_NONE;

                    /* Mark that the socket is connected. */
                    pxSecureSocket->ulFlags |= stsecuresocketsSOCKET_IS_CONNECTED_FLAG;
                }
                else
                {
                    /* Connection failed. */
                    lRetVal = SOCKETS_SOCKET_ERROR;
                }
            }

            /* Return the semaphore. */
            ( void ) xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
        }
        else
        {
            /* Could not acquire semaphore. */
            lRetVal = SOCKETS_SOCKET_ERROR;
        }
    }
    else
    {
        /* Invalid socket handle was passed. */
        lRetVal = SOCKETS_EINVAL;
    }

    /* TLS initialization is needed only if we are not using offload SSL. */
    #ifndef USE_OFFLOAD_SSL
        /* Initialize TLS only if the connection is successful. */
        if( ( lRetVal == SOCKETS_ERROR_NONE ) &&
            ( ( pxSecureSocket->ulFlags & stsecuresocketsSOCKET_SECURE_FLAG ) != 0UL ) )
        {
            /* Setup TLS parameters. */
            xTLSParams.ulSize = sizeof( xTLSParams );
            xTLSParams.pcDestination = pxSecureSocket->pcDestination;
            xTLSParams.pcServerCertificate = pxSecureSocket->pcServerCertificate;
            xTLSParams.ulServerCertificateLength = pxSecureSocket->ulServerCertificateLength;
            xTLSParams.pvCallerContext = ( void * ) xSocket;
            xTLSParams.pxNetworkRecv = &( prvNetworkRecv );
            xTLSParams.pxNetworkSend = &( prvNetworkSend );

            /* Initialize TLS. */
            if( TLS_Init( &( pxSecureSocket->pvTLSContext ), &( xTLSParams ) ) == pdFREERTOS_ERRNO_NONE )
            {
                /* Initiate TLS handshake. */
                if( TLS_Connect( pxSecureSocket->pvTLSContext ) != pdFREERTOS_ERRNO_NONE )
                {
                    /* TLS handshake failed. */
                    lRetVal = SOCKETS_TLS_HANDSHAKE_ERROR;
                }
            }
            else
            {
                /* TLS Initialization failed. */
                lRetVal = SOCKETS_TLS_INIT_ERROR;
            }
        }
    #endif /* USE_OFFLOAD_SSL*/

    return lRetVal;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Recv( Socket_t xSocket,
                      void * pvBuffer,
                      size_t xBufferLength,
                      uint32_t ulFlags )
{
    uint32_t ulSocketNumber = ( uint32_t ) xSocket; /*lint !e923 cast required for portability. */
    STSecureSocket_t * pxSecureSocket;
    int32_t lReceivedBytes = SOCKETS_SOCKET_ERROR;

    /* Remove warning about unused parameters. */
    ( void ) ulFlags;

    /* Ensure that a valid socket was passed and the
     * passed buffer is not NULL. */
    if( ( prvIsValidSocket( ulSocketNumber ) == pdTRUE ) &&
        ( pvBuffer != NULL ) )
    {
        /* Shortcut for easy access. */
        pxSecureSocket = &( xSockets[ ulSocketNumber ] );

        /* Check that receive is allowed on the socket. */
        if( ( pxSecureSocket->ulFlags & stsecuresocketsSOCKET_READ_CLOSED_FLAG ) == 0UL )
        {
            #ifndef USE_OFFLOAD_SSL
                if( ( pxSecureSocket->ulFlags & stsecuresocketsSOCKET_SECURE_FLAG ) != 0UL )
                {
                    /* Receive through TLS pipe, if negotiated. */
                    lReceivedBytes = TLS_Recv( pxSecureSocket->pvTLSContext, pvBuffer, xBufferLength );

                    /* Convert the error code. */
                    if( lReceivedBytes < 0 )
                    {
                        /* TLS_Recv failed. */
                        lReceivedBytes = SOCKETS_TLS_RECV_ERROR;
                    }
                }
                else
                {
                    /* Receive un-encrypted. */
                    lReceivedBytes = prvNetworkRecv( xSocket, pvBuffer, xBufferLength );
                }
            #else /* USE_OFFLOAD_SSL */
                /* Always receive using prvNetworkRecv if using offload SSL. */
                lReceivedBytes = prvNetworkRecv( xSocket, pvBuffer, xBufferLength );
            #endif /* USE_OFFLOAD_SSL */
        }
        else
        {
            /* The socket has been closed for read. */
            lReceivedBytes = SOCKETS_ECLOSED;
        }
    }

    return lReceivedBytes;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Send( Socket_t xSocket,
                      const void * pvBuffer,
                      size_t xDataLength,
                      uint32_t ulFlags )
{
    uint32_t ulSocketNumber = ( uint32_t ) xSocket; /*lint !e923 cast required for portability. */
    STSecureSocket_t * pxSecureSocket;
    int32_t lSentBytes = SOCKETS_SOCKET_ERROR;

    /* Remove warning about unused parameters. */
    ( void ) ulFlags;

    /* Ensure that a valid socket was passed and the passed buffer
     * is not NULL. */
    if( ( prvIsValidSocket( ulSocketNumber ) == pdTRUE ) &&
        ( pvBuffer != NULL ) )
    {
        /* Shortcut for easy access. */
        pxSecureSocket = &( xSockets[ ulSocketNumber ] );

        /* Check that send is allowed on the socket. */
        if( ( pxSecureSocket->ulFlags & stsecuresocketsSOCKET_WRITE_CLOSED_FLAG ) == 0UL )
        {
            #ifndef USE_OFFLOAD_SSL
                if( ( pxSecureSocket->ulFlags & stsecuresocketsSOCKET_SECURE_FLAG ) != 0UL )
                {
                    /* Send through TLS pipe, if negotiated. */
                    lSentBytes = TLS_Send( pxSecureSocket->pvTLSContext, pvBuffer, xDataLength );

                    /* Convert the error code. */
                    if( lSentBytes < 0 )
                    {
                        /* TLS_Send failed. */
                        lSentBytes = SOCKETS_TLS_SEND_ERROR;
                    }
                }
                else
                {
                    /* Send un-encrypted. */
                    lSentBytes = prvNetworkSend( xSocket, pvBuffer, xDataLength );
                }
            #else /* USE_OFFLOAD_SSL */
                /* Always send using prvNetworkSend if using offload SSL. */
                lSentBytes = prvNetworkSend( xSocket, pvBuffer, xDataLength );
            #endif /* USE_OFFLOAD_SSL */
        }
        else
        {
            /* The socket has been closed for write. */
            lSentBytes = SOCKETS_ECLOSED;
        }
    }

    return lSentBytes;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Shutdown( Socket_t xSocket,
                          uint32_t ulHow )
{
    uint32_t ulSocketNumber = ( uint32_t ) xSocket; /*lint !e923 cast required for portability. */
    STSecureSocket_t * pxSecureSocket;
    int32_t lRetVal = SOCKETS_SOCKET_ERROR;

    /* Ensure that a valid socket was passed. */
    if( prvIsValidSocket( ulSocketNumber ) == pdTRUE )
    {
        /* Shortcut for easy access. */
        pxSecureSocket = &( xSockets[ ulSocketNumber ] );

        switch( ulHow )
        {
            case SOCKETS_SHUT_RD:
                /* Further receive calls on this socket should return error. */
                pxSecureSocket->ulFlags |= stsecuresocketsSOCKET_READ_CLOSED_FLAG;

                /* Return success to the user. */
                lRetVal = SOCKETS_ERROR_NONE;
                break;

            case SOCKETS_SHUT_WR:
                /* Further send calls on this socket should return error. */
                pxSecureSocket->ulFlags |= stsecuresocketsSOCKET_WRITE_CLOSED_FLAG;

                /* Return success to the user. */
                lRetVal = SOCKETS_ERROR_NONE;
                break;

            case SOCKETS_SHUT_RDWR:
                /* Further send or receive calls on this socket should return error. */
                pxSecureSocket->ulFlags |= stsecuresocketsSOCKET_READ_CLOSED_FLAG;
                pxSecureSocket->ulFlags |= stsecuresocketsSOCKET_WRITE_CLOSED_FLAG;

                /* Return success to the user. */
                lRetVal = SOCKETS_ERROR_NONE;
                break;

            default:
                /* An invalid value was passed for ulHow. */
                lRetVal = SOCKETS_EINVAL;
                break;
        }
    }
    else
    {
        /* Invalid socket was passed. */
        lRetVal = SOCKETS_EINVAL;
    }

    return lRetVal;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Close( Socket_t xSocket )
{
    uint32_t ulSocketNumber = ( uint32_t ) xSocket; /*lint !e923 cast required for portability. */
    STSecureSocket_t * pxSecureSocket;
    ES_WIFI_Conn_t xWiFiConnection;
    int32_t lRetVal;

    /* Ensure that a valid socket was passed. */
    if( prvIsValidSocket( ulSocketNumber ) == pdTRUE )
    {
        /* Shortcut for easy access. */
        pxSecureSocket = &( xSockets[ ulSocketNumber ] );

        /* Mark the socket as closed. */
        pxSecureSocket->ulFlags |= stsecuresocketsSOCKET_READ_CLOSED_FLAG;
        pxSecureSocket->ulFlags |= stsecuresocketsSOCKET_WRITE_CLOSED_FLAG;

        /* Free the space allocated for pcDestination. */
        if( pxSecureSocket->pcDestination != NULL )
        {
            vPortFree( pxSecureSocket->pcDestination );
        }

        /* Free the space allocated for pcServerCertificate. */
        if( pxSecureSocket->pcServerCertificate != NULL )
        {
            vPortFree( pxSecureSocket->pcServerCertificate );
        }

        #ifndef USE_OFFLOAD_SSL
            /* Cleanup TLS. */
            if( ( pxSecureSocket->ulFlags & stsecuresocketsSOCKET_SECURE_FLAG ) != 0UL )
            {
                TLS_Cleanup( pxSecureSocket->pvTLSContext );
            }
        #endif /* USE_OFFLOAD_SSL */

        /* Initialize the members used by the ES_WIFI_StopClientConnection call. */
        xWiFiConnection.Number = ( uint8_t ) ulSocketNumber;

        /* Try to acquire the semaphore. */
        if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
        {
            /* Stop the client connection. */
            if( ES_WIFI_StopClientConnection( &( xWiFiModule.xWifiObject ), &( xWiFiConnection ) )
                == ES_WIFI_STATUS_OK )
            {
                /* Connection close successful. */
                lRetVal = SOCKETS_ERROR_NONE;
            }
            else
            {
                /* Couldn't stop WiFi client connection. */
                lRetVal = SOCKETS_SOCKET_ERROR;
            }

            /* Return the semaphore. */
            ( void ) xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
        }
        else
        {
            /* Couldn't get semaphore. */
            lRetVal = SOCKETS_SOCKET_ERROR;
        }

        /* Return the socket back to the free socket pool. */
        prvReturnSocket( ulSocketNumber );
    }
    else
    {
        /* Bad argument. */
        lRetVal = SOCKETS_EINVAL;
    }

    return lRetVal;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_SetSockOpt( Socket_t xSocket,
                            int32_t lLevel,
                            int32_t lOptionName,
                            const void * pvOptionValue,
                            size_t xOptionLength )
{
    uint32_t ulSocketNumber = ( uint32_t ) xSocket; /*lint !e923 cast required for portability. */
    STSecureSocket_t * pxSecureSocket;
    int32_t lRetVal = SOCKETS_ERROR_NONE;
    uint32_t lTimeout;

    /* Ensure that a valid socket was passed. */
    if( prvIsValidSocket( ulSocketNumber ) == pdTRUE )
    {
        /* Shortcut for easy access. */
        pxSecureSocket = &( xSockets[ ulSocketNumber ] );

        switch( lOptionName )
        {
            case SOCKETS_SO_SERVER_NAME_INDICATION:

                if( ( pxSecureSocket->ulFlags & stsecuresocketsSOCKET_IS_CONNECTED_FLAG ) == 0 )
                {
                    /* Non-NULL destination string indicates that SNI extension should
                     * be used during TLS negotiation. */
                    pxSecureSocket->pcDestination = ( char * ) pvPortMalloc( 1U + xOptionLength );

                    if( pxSecureSocket->pcDestination == NULL )
                    {
                        lRetVal = SOCKETS_ENOMEM;
                    }
                    else
                    {
                        memcpy( pxSecureSocket->pcDestination, pvOptionValue, xOptionLength );
                        pxSecureSocket->pcDestination[ xOptionLength ] = '\0';
                    }
                }
                else
                {
                    /* SNI must be set before connection is established. */
                    lRetVal = SOCKETS_SOCKET_ERROR;
                }

                break;

            case SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE:

                if( ( pxSecureSocket->ulFlags & stsecuresocketsSOCKET_IS_CONNECTED_FLAG ) == 0 )
                {
                    /* Non-NULL server certificate field indicates that the default trust
                     * list should not be used. */
                    pxSecureSocket->pcServerCertificate = ( char * ) pvPortMalloc( xOptionLength );

                    if( pxSecureSocket->pcServerCertificate == NULL )
                    {
                        lRetVal = SOCKETS_ENOMEM;
                    }
                    else
                    {
                        memcpy( pxSecureSocket->pcServerCertificate, pvOptionValue, xOptionLength );
                        pxSecureSocket->ulServerCertificateLength = xOptionLength;
                    }
                }
                else
                {
                    /* Trusted server certificate must be set before the connection is established. */
                    lRetVal = SOCKETS_SOCKET_ERROR;
                }

                break;

            case SOCKETS_SO_REQUIRE_TLS:

                if( ( pxSecureSocket->ulFlags & stsecuresocketsSOCKET_IS_CONNECTED_FLAG ) == 0 )
                {
                    /* Mark that it is a secure socket. */
                    pxSecureSocket->ulFlags |= stsecuresocketsSOCKET_SECURE_FLAG;

                    #ifdef USE_OFFLOAD_SSL
                        /* Set the socket type to SSL to use offload SSL. */
                        pxSecureSocket->xSocketType = ES_WIFI_TCP_SSL_CONNECTION;
                    #endif /* USE_OFFLOAD_SSL */
                }
                else
                {
                    /* Require TLS must be set before the connection is established. */
                    lRetVal = SOCKETS_SOCKET_ERROR;
                }

                break;

            case SOCKETS_SO_SNDTIMEO:

                lTimeout = *( ( const uint32_t * ) pvOptionValue ); /*lint !e9087 pvOptionValue is passed in as an opaque value, and must be casted for setsockopt. */

                /* Valid timeouts are 0 (no timeout) or 1-30000ms. */
                if( lTimeout < stsecuresocketsMAX_TIMEOUT )
                {
                    /* Store send timeout. */
                    pxSecureSocket->ulSendTimeout = lTimeout;
                }
                else
                {
                    lRetVal = SOCKETS_EINVAL;
                }

                break;

            case SOCKETS_SO_RCVTIMEO:

                lTimeout = *( ( const uint32_t * ) pvOptionValue ); /*lint !e9087 pvOptionValue is passed in as an opaque value, and must be casted for setsockopt. */

                /* Valid timeouts are 0 (no timeout) or 1-30000ms. */
                if( lTimeout < stsecuresocketsMAX_TIMEOUT )
                {
                    /* Store receive timeout. */
                    pxSecureSocket->ulReceiveTimeout = lTimeout;
                }
                else
                {
                    lRetVal = SOCKETS_EINVAL;
                }

                break;

            case SOCKETS_SO_NONBLOCK:

                if( ( pxSecureSocket->ulFlags & stsecuresocketsSOCKET_IS_CONNECTED_FLAG ) != 0 )
                {
                    /* Set the timeouts to the smallest value possible.
                     * This isn't true nonblocking, but as close as we can get. */
                    pxSecureSocket->ulReceiveTimeout = 1;
                    pxSecureSocket->ulSendTimeout = 1;
                }
                else
                {
                    /* Non blocking option must be set after the connection is
                     * established. Non blocking connect is not supported. */
                    lRetVal = SOCKETS_SOCKET_ERROR;
                }

                break;

            default:

                lRetVal = SOCKETS_ENOPROTOOPT;
                break;
        }
    }
    else
    {
        lRetVal = SOCKETS_SOCKET_ERROR;
    }

    return lRetVal;
}
/*-----------------------------------------------------------*/

uint32_t SOCKETS_GetHostByName( const char * pcHostName )
{
    uint32_t ulIPAddres = 0;

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        /* Do a DNS Lookup. */
        if( ES_WIFI_DNS_LookUp( &( xWiFiModule.xWifiObject ), pcHostName, ( uint8_t * ) &( ulIPAddres ) ) != ES_WIFI_STATUS_OK )
        {
            /* Return 0 if the DNS lookup fails. */
            ulIPAddres = 0;
        }

        /* Return the semaphore. */
        ( void ) xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
    }

    return ulIPAddres;
}
/*-----------------------------------------------------------*/

BaseType_t SOCKETS_Init( void )
{
    uint32_t ulIndex;

    /* Mark all the sockets as free and closed. */
    for( ulIndex = 0; ulIndex < ( uint32_t ) wificonfigMAX_SOCKETS; ulIndex++ )
    {
        xSockets[ ulIndex ].ucInUse = 0;
        xSockets[ ulIndex ].ulFlags = 0;

        xSockets[ ulIndex ].ulFlags |= stsecuresocketsSOCKET_READ_CLOSED_FLAG;
        xSockets[ ulIndex ].ulFlags |= stsecuresocketsSOCKET_WRITE_CLOSED_FLAG;
    }

    /* Empty initialization for ST board. */
    return pdPASS;
}
/*-----------------------------------------------------------*/
