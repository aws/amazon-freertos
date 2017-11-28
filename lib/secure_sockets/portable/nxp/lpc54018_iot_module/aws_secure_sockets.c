/*
 * Amazon FreeRTOS Secure Sockets for NXP54018_IoT_Module V1.0.0 beta 1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
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

/*
 * Copyright (C) NXP 20017.
 */


/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "aws_secure_sockets.h"
#include "aws_tls.h"
#include "task.h"

/* Third-party wifi driver include. */
#include "qcom_api.h"
#include "aws_wifi.h"
#include "custom_stack_offload.h"
#include "atheros_stack_offload.h"


/* Internal context structure. */
typedef struct SSOCKETContext
{
    Socket_t xSocket;
    char * pcDestination;
    void * pvTLSContext;
    BaseType_t xRequireTLS;
    BaseType_t xSendFlags;
    BaseType_t xRecvFlags;
    char * pcServerCertificate;
    uint32_t ulServerCertificateLength;
} SSOCKETContext_t, * SSOCKETContextPtr_t;

/*
 * Helper routines.
 */

/*
 * @brief Network send callback.
 */
static BaseType_t prvNetworkSend( void * pvContext,
                                  const unsigned char * pucData,
                                  size_t xDataLength )
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) pvContext;

    char * sendBuf = custom_alloc( xDataLength );

    if( sendBuf == NULL )
    {
        return -1;
    }

    memcpy( sendBuf, pucData, xDataLength );
    int ret = qcom_send( ( int ) pxContext->xSocket,
                         sendBuf,
                         xDataLength,
                         pxContext->xSendFlags );
    custom_free( sendBuf );

    return ret;
}
/*-----------------------------------------------------------*/


QCA_CONTEXT_STRUCT * wlan_get_context( void );

/*
 * @brief Network receive callback.
 */
static BaseType_t prvNetworkRecv( void * pvContext,
                                  unsigned char * pucReceiveBuffer,
                                  size_t xReceiveLength )
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) pvContext;

    QCA_CONTEXT_STRUCT * enetCtx = wlan_get_context();
    A_STATUS status = ( A_STATUS ) t_select( enetCtx,
                                             ( uint32_t ) pxContext->xSocket,
                                             1000 );

    if( status == A_ERROR )
    {
        /* Timeout. */
        return 0;
    }

    char * buffLoc = NULL;
    int ret = 0;
    ret = qcom_recv( ( int ) pxContext->xSocket, &buffLoc, xReceiveLength, 0 );

    if( ret > 0 )
    {
        memcpy( pucReceiveBuffer, buffLoc, ret );
    }

    if( buffLoc != NULL )
    {
        zero_copy_free( buffLoc );
    }

    return ret;
}
/*-----------------------------------------------------------*/

/*
 * Interface routines.
 */

int32_t SOCKETS_Close( Socket_t xSocket )
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket;

    if( NULL != pxContext )
    {
        if( NULL != pxContext->pcDestination )
        {
            vPortFree( pxContext->pcDestination );
        }

        if( NULL != pxContext->pcServerCertificate )
        {
            vPortFree( pxContext->pcServerCertificate );
        }

        if( pdTRUE == pxContext->xRequireTLS )
        {
            TLS_Cleanup( pxContext->pvTLSContext );
        }

        qcom_socket_close( ( int ) pxContext->xSocket );
        vPortFree( pxContext );
    }

    return pdFREERTOS_ERRNO_NONE;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Connect( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t xAddressLength )
{
    int32_t xStatus = pdFREERTOS_ERRNO_NONE;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket;
    TLSParams_t xTLSParams = { 0 };
    SOCKADDR_T xTempAddress = { 0 };

    if( ( pxContext != SOCKETS_INVALID_SOCKET ) && ( pxAddress != NULL ) )
    {
        /* Connect the wrapped socket. */

        /* The driver code expects the endianess of the address and port in host order and then swaps before
         * sending the connection data to the firmware. */
        xTempAddress.sin_addr.s_addr = SOCKETS_ntohl( pxAddress->ulAddress );
        xTempAddress.sin_family = pxAddress->ucSocketDomain;
        xTempAddress.sin_port = SOCKETS_ntohs( pxAddress->usPort );
        xStatus = qcom_connect( ( int ) pxContext->xSocket,
                                ( struct sockaddr * ) &xTempAddress,
                                xAddressLength );

        /* Negotiate TLS if requested. */
        if( ( pdFREERTOS_ERRNO_NONE == xStatus ) && ( pdTRUE == pxContext->xRequireTLS ) )
        {
            xTLSParams.ulSize = sizeof( xTLSParams );
            xTLSParams.pcDestination = pxContext->pcDestination;
            xTLSParams.pcServerCertificate = pxContext->pcServerCertificate;
            xTLSParams.ulServerCertificateLength = pxContext->ulServerCertificateLength;
            xTLSParams.pvCallerContext = pxContext;
            xTLSParams.pxNetworkRecv = prvNetworkRecv;
            xTLSParams.pxNetworkSend = prvNetworkSend;
            xStatus = TLS_Init( &pxContext->pvTLSContext, &xTLSParams );

            if( pdFREERTOS_ERRNO_NONE == xStatus )
            {
                xStatus = TLS_Connect( pxContext->pvTLSContext );
            }
        }
    }
    else
    {
        xStatus = SOCKETS_SOCKET_ERROR;
    }

    return xStatus;
}
/*-----------------------------------------------------------*/


/* Convert IP address in uint32_t to comma separated bytes. */
#define UINT32_IPADDR_TO_CSV_BYTES( a )                     \
    ( ( ( a ) >> 24 ) & 0xFF ), ( ( ( a ) >> 16 ) & 0xFF ), \
    ( ( ( a ) >> 8 ) & 0xFF ), ( ( a ) & 0xFF )

/*-----------------------------------------------------------*/

uint32_t SOCKETS_GetHostByName( const char * pcHostName )
{
    uint32_t addr = 0;

    if( strlen( pcHostName ) <= ( size_t ) securesocketsMAX_DNS_NAME_LENGTH )
    {
        WIFI_GetHostIP( ( char * ) pcHostName, ( uint8_t * ) &addr );
        configPRINTF( ( "Looked up %s as %d.%d.%d.%d\r\n",
                        pcHostName,
                        UINT32_IPADDR_TO_CSV_BYTES( addr ) ) );
    }
    else
    {
        configPRINTF( ( "Malformed URL, Host name: %s is too long.", pcHostName ) );
    }

    /* This api is to return the address in network order. WIFI_GetHostIP returns the host IP
     * in host order.
     */
    addr = SOCKETS_htonl( addr );

    return addr;
}

/*-----------------------------------------------------------*/

int32_t SOCKETS_Recv( Socket_t xSocket,
                      void * pvBuffer,
                      size_t xBufferLength,
                      uint32_t ulFlags )
{
    int32_t xStatus = pdFREERTOS_ERRNO_NONE;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket;

    pxContext->xRecvFlags = ( BaseType_t ) ulFlags;

    if( ( xSocket != SOCKETS_INVALID_SOCKET ) &&
        ( pvBuffer != NULL ) )
    {
        if( pdTRUE == pxContext->xRequireTLS )
        {
            /* Receive through TLS pipe, if negotiated. */
            xStatus = TLS_Recv( pxContext->pvTLSContext, pvBuffer, xBufferLength );
        }
        else
        {
            /* Receive unencrypted. */
            xStatus = prvNetworkRecv( pxContext, pvBuffer, xBufferLength );
        }
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Send( Socket_t xSocket,
                      const void * pvBuffer,
                      size_t xDataLength,
                      uint32_t ulFlags )
{
    /* The WiFi module refuses to send data if it exceeds this threshold defined in
     * atheros_stack_offload.h. */
    const uint32_t ulSendMaxLength = IPV4_FRAGMENTATION_THRESHOLD;
    int32_t lWritten = 0, lWrittenPerLoop = 0;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket;

    if( ( xSocket != SOCKETS_INVALID_SOCKET ) &&
        ( pvBuffer != NULL ) )
    {
        pxContext->xSendFlags = ( BaseType_t ) ulFlags;

        if( pdTRUE == pxContext->xRequireTLS )
        {
            for(
                uint32_t ulToRemain = xDataLength, ulBufferPos = 0, ulToSend = 0 ;
                ulToRemain ;
                ulBufferPos += ulToSend, ulToRemain -= ulToSend
                )
            {
                ulToSend = ulToRemain > ulSendMaxLength ? ulSendMaxLength : ulToRemain;
                /* Send through TLS pipe, if negotiated. */
                lWrittenPerLoop = TLS_Send( pxContext->pvTLSContext,
                                            &( ( unsigned char const * ) pvBuffer )[ ulBufferPos ],
                                            ulToSend );

                /* Error code < 0. */
                if( lWrittenPerLoop < 0 )
                {
                    /* Set the error code to be returned */
                    lWritten = lWrittenPerLoop;
                    break;
                }

                /* Number of lWritten bytes. */
                lWritten += lWrittenPerLoop;

                if( lWrittenPerLoop != ulToSend )
                {
                    break;
                }
            }
        }
        else
        {
            for(
                uint32_t ulToRemain = xDataLength, ulBufferPos = 0, ulToSend = 0 ;
                ulToRemain ;
                ulBufferPos += ulToSend, ulToRemain -= ulToSend
                )
            {
                ulToSend = ulToRemain > ulSendMaxLength ? ulSendMaxLength : ulToRemain;
                lWrittenPerLoop = prvNetworkSend( pxContext,
                                                  &( ( unsigned char const * ) pvBuffer )[ ulBufferPos ],
                                                  ulToSend );

                /* Error code < 0. */
                if( lWrittenPerLoop < 0 )
                {
                    /* Set the error code to be returned */
                    lWritten = lWrittenPerLoop;
                    break;
                }

                /* Number of lWritten bytes. */
                lWritten += lWrittenPerLoop;

                if( lWrittenPerLoop != ulToSend )
                {
                    break;
                }
            }
        }
    }

    return lWritten;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_SetSockOpt( Socket_t xSocket,
                            int32_t lLevel,
                            int32_t lOptionName,
                            const void * pvOptionValue,
                            size_t xOptionLength )
{
    int32_t xStatus = pdFREERTOS_ERRNO_NONE;
    TickType_t xTimeout;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket;

    switch( lOptionName )
    {
        case SOCKETS_SO_SERVER_NAME_INDICATION:

            /* Non-NULL destination string indicates that SNI extension should
             * be used during TLS negotiation. */
            if( NULL == ( pxContext->pcDestination =
                              ( char * ) pvPortMalloc( 1 + xOptionLength ) ) )
            {
                xStatus = pdFREERTOS_ERRNO_ENOMEM;
            }
            else
            {
                memcpy( pxContext->pcDestination, pvOptionValue, xOptionLength );
                pxContext->pcDestination[ xOptionLength ] = '\0';
            }

            break;

        case SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE:

            /* Non-NULL server certificate field indicates that the default trust
             * list should not be used. */
            if( NULL == ( pxContext->pcServerCertificate =
                              ( char * ) pvPortMalloc( xOptionLength ) ) )
            {
                xStatus = pdFREERTOS_ERRNO_ENOMEM;
            }
            else
            {
                memcpy( pxContext->pcServerCertificate, pvOptionValue, xOptionLength );
                pxContext->ulServerCertificateLength = xOptionLength;
            }

            break;

        case SOCKETS_SO_REQUIRE_TLS:
            pxContext->xRequireTLS = pdTRUE;
            break;

        case SOCKETS_SO_NONBLOCK:
            xTimeout = 0;
            xStatus = qcom_setsockopt( ( int ) pxContext->xSocket,
                                       lLevel,
                                       SOCKETS_SO_RCVTIMEO,
                                       ( void * ) &xTimeout,
                                       sizeof( xTimeout ) );

            if( xStatus == SOCKETS_ERROR_NONE )
            {
                xStatus = qcom_setsockopt( ( int ) pxContext->xSocket,
                                           lLevel,
                                           SOCKETS_SO_SNDTIMEO,
                                           ( void * ) &xTimeout,
                                           sizeof( xTimeout ) );
            }

            break;

        case SOCKETS_SO_RCVTIMEO:
        case SOCKETS_SO_SNDTIMEO:
            /* Comply with Berkeley standard - a 0 timeout is wait forever. */
            xTimeout = *( ( const TickType_t * ) pvOptionValue ); /*lint !e9087 pvOptionValue passed should be of TickType_t. */

            if( xTimeout == 0U )
            {
                xTimeout = portMAX_DELAY;
            }

            xStatus = qcom_setsockopt( ( int ) pxContext->xSocket,
                                       lLevel,
                                       lOptionName,
                                       ( void * ) &xTimeout,
                                       xOptionLength );
            break;

        default:
            xStatus = qcom_setsockopt( ( int ) pxContext->xSocket,
                                       lLevel,
                                       lOptionName,
                                       ( void * ) pvOptionValue,
                                       xOptionLength );
            break;
    }

    return xStatus;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Shutdown( Socket_t xSocket,
                          uint32_t ulHow )
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket;

    ( void ) pxContext;

    /* Not yet implemented. */
    return 0;
}
/*-----------------------------------------------------------*/

Socket_t SOCKETS_Socket( int32_t xDomain,
                         int32_t xType,
                         int32_t xProtocol )
{
    int32_t xStatus = pdFREERTOS_ERRNO_NONE;
    SSOCKETContextPtr_t pxContext = NULL;

    /* Allocate the internal context structure. */
    if( NULL == ( pxContext = pvPortMalloc( sizeof( SSOCKETContext_t ) ) ) )
    {
        xStatus = pdFREERTOS_ERRNO_ENOMEM;
    }

    if( pdFREERTOS_ERRNO_NONE == xStatus )
    {
        memset( pxContext, 0, sizeof( SSOCKETContext_t ) );

        /* Create the wrapped socket. */
        int socket = 0;
        socket = qcom_socket( xDomain, xType, /*xProtocol*/ 0 );

        if( socket == A_ERROR )
        {
            pxContext->xSocket = SOCKETS_INVALID_SOCKET;
        }
        else
        {
            pxContext->xSocket = ( Socket_t ) socket;
        }

/*        if( NULL == ( pxContext->xSocket ) ) */
/*        { */
/*            xStatus = pdFREERTOS_ERRNO_ENOMEM; */
/*        } */
    }

    if( pdFREERTOS_ERRNO_NONE != xStatus )
    {
        vPortFree( pxContext );
    }

    return pxContext;
}
/*-----------------------------------------------------------*/

BaseType_t SOCKETS_Init( void )
{
    /* Empty initialization for NXP board. */
    return pdPASS;
}
