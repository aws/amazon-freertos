/*
 * Amazon FreeRTOS Secure Socket V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_secure_sockets.c
 * @brief WiFi and Secure Socket interface implementation.
 */

/* Define _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE to prevent secure sockets functions
 * from redefining in iot_secure_sockets_wrapper_metrics.h */
#define _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE

/* FreeRTOS includes. */
#include <string.h>
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "list.h"
#include "iot_secure_sockets.h"
#include "iot_tls.h"
#include "task.h"
#include "iot_pkcs11.h"
#include "iot_crypto.h"
#include "platform.h"
#include "r_wifi_sx_ulpgn_if.h"

#undef _SECURE_SOCKETS_WRAPPER_NOT_REDEFINE


/* Internal context structure. */
typedef struct SSOCKETContext
{
    Socket_t xSocket;
    char * pcDestination;
    void * pvTLSContext;
    BaseType_t xRequireTLS;
    BaseType_t xSendFlags;
    BaseType_t xRecvFlags;
    uint32_t ulSendTimeout;
    uint32_t ulRecvTimeout;
    char * pcServerCertificate;
    uint32_t ulServerCertificateLength;
    uint32_t socket_no;
} SSOCKETContext_t, * SSOCKETContextPtr_t;

/**
 * @brief Maximum number of sockets.
 *
 * 16 total sockets
 */
#define MAX_NUM_SSOCKETS    (WIFI_CFG_CREATABLE_SOCKETS)

/**
 * @brief Number of secure sockets allocated.
 *
 * Keep a count of the number of open sockets.
 */
static uint8_t ssockets_num_allocated = 0;


static SemaphoreHandle_t xUcInUse = NULL;
static const TickType_t xMaxSemaphoreBlockTime = pdMS_TO_TICKS( 60000UL );

/* Generate a randomized TCP Initial Sequence Number per RFC. */
uint32_t ulApplicationGetNextSequenceNumber(
    uint32_t ulSourceAddress,
    uint16_t usSourcePort,
    uint32_t ulDestinationAddress,
    uint16_t usDestinationPort );

/*
 * @brief Network send callback.
 */
static BaseType_t prvNetworkSend( void * pvContext,
                                  const unsigned char * pucData,
                                  size_t xDataLength )
{
	BaseType_t send_byte;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) pvContext; /*lint !e9087 cast used for portability. */

    send_byte = R_WIFI_SX_ULPGN_SocketSend(pxContext->socket_no, (uint8_t *)pucData, xDataLength, pxContext->ulSendTimeout);
    return send_byte;
}
/*-----------------------------------------------------------*/

/*
 * @brief Network receive callback.
*/
static BaseType_t prvNetworkRecv( void * pvContext,
                                 unsigned char * pucReceiveBuffer,
                                 size_t xReceiveLength )
{
	BaseType_t receive_byte;
   SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) pvContext; /*lint !e9087 cast used for portability. */


   receive_byte = R_WIFI_SX_ULPGN_SocketRecv(pxContext->socket_no, pucReceiveBuffer, xReceiveLength, pxContext->ulRecvTimeout);

   return receive_byte;
}

/*-----------------------------------------------------------*/

/**
 * @brief Creates a TCP socket.
 *
 * This call allocates memory and claims a socket resource.
 *
 * @sa SOCKETS_Close()
 *
 * @param[in] lDomain Must be set to SOCKETS_AF_INET. See @ref SocketDomains.
 * @param[in] lType Set to SOCKETS_SOCK_STREAM to create a TCP socket.
 * No other value is valid.  See @ref SocketTypes.
 * @param[in] lProtocol Set to SOCKETS_IPPROTO_TCP to create a TCP socket.
 * No other value is valid. See @ref Protocols.
 *
 * @return
 * * If a socket is created successfully, then the socket handle is
 * returned
 * * @ref SOCKETS_INVALID_SOCKET is returned if an error occurred.
 */
Socket_t SOCKETS_Socket( int32_t lDomain,
                         int32_t lType,
                         int32_t lProtocol )
{
	int32_t lStatus = SOCKETS_ERROR_NONE;
    int32_t ret;
    SSOCKETContextPtr_t pxContext = NULL;

    /* Ensure that only supported values are supplied. */
    configASSERT( lDomain == SOCKETS_AF_INET );
    configASSERT( lType == SOCKETS_SOCK_STREAM );
    configASSERT( lProtocol == SOCKETS_IPPROTO_TCP );
	/* Ensure that only supported values are supplied. */
    if((lDomain != SOCKETS_AF_INET) || ( lType != SOCKETS_SOCK_STREAM ) || ( lProtocol != SOCKETS_IPPROTO_TCP ))
	{
		return SOCKETS_INVALID_SOCKET;
	}
    if (xSemaphoreTake( xUcInUse, xMaxSemaphoreBlockTime) == pdTRUE)
    {
		 if( SOCKETS_ERROR_NONE == lStatus )
		 {
			/* Allocate the internal context structure. */
			if( NULL == ( pxContext = pvPortMalloc( sizeof( SSOCKETContext_t ) ) ) )
			{
				configPRINTF(("create malloc error: %d\r\n",ret));
				lStatus = SOCKETS_ENOMEM;
			}
		}

		if( SOCKETS_ERROR_NONE == lStatus )
		{
			memset( pxContext, 0, sizeof( SSOCKETContext_t ) );
			/* Create the wrapped socket. */
			ret = R_WIFI_SX_ULPGN_SocketCreate(WIFI_SOCKET_IP_PROTOCOL_TCP, WIFI_SOCKET_IP_VERSION_4);

			if(0 > ret)
			{
				configPRINTF(("create error: %d\r\n",ret));
				lStatus = SOCKETS_SOCKET_ERROR;
			}
			else
			{
				pxContext->socket_no = ret;
				pxContext->xSocket = pxContext;
				pxContext->ulRecvTimeout = socketsconfigDEFAULT_RECV_TIMEOUT;
				pxContext->ulSendTimeout = socketsconfigDEFAULT_SEND_TIMEOUT;
			}
		}

		if( SOCKETS_ERROR_NONE != lStatus )
		{
			if(NULL != pxContext)
			{
				vPortFree( pxContext );
			}
			/* Give back the socketInUse mutex. */
		    lStatus = SOCKETS_INVALID_SOCKET;
		}
	    xSemaphoreGive(xUcInUse);
    }
    if( SOCKETS_ERROR_NONE != lStatus )
    {
        return SOCKETS_INVALID_SOCKET;
    }
    else
    {
    	return pxContext;
    }
}
/*-----------------------------------------------------------*/

Socket_t SOCKETS_Accept( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t * pxAddressLength )
{
    /* FIX ME. */
    return SOCKETS_INVALID_SOCKET;
}
/*-----------------------------------------------------------*/

/**
 * @brief Connects the socket to the specified IP address and port.
 *
 * The socket must first have been successfully created by a call to SOCKETS_Socket().
 *
 * @param[in] xSocket The handle of the socket to be connected.
 * @param[in] pxAddress A pointer to a SocketsSockaddr_t structure that contains the
 * the address to connect the socket to.
 * @param[in] xAddressLength Should be set to sizeof( @ref SocketsSockaddr_t ).
 *
 * @return
 * * @ref SOCKETS_ERROR_NONE if a connection is established.
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Connect( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t xAddressLength )
{
    int32_t lStatus = SOCKETS_ERROR_NONE;
    wifi_err_t ret;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */
    TLSParams_t xTLSParams = { 0 };

    if( ( pxContext != SOCKETS_INVALID_SOCKET ) && ( pxAddress != NULL ) )
    {
		ret = R_WIFI_SX_ULPGN_SocketConnect(pxContext->socket_no, SOCKETS_ntohl(pxAddress->ulAddress), SOCKETS_ntohs(pxAddress->usPort));
		if( WIFI_SUCCESS != ret )
		{
			lStatus = SOCKETS_SOCKET_ERROR;
		}

        /* Negotiate TLS if requested. */
        if( ( SOCKETS_ERROR_NONE == lStatus ) && ( pdTRUE == pxContext->xRequireTLS ) )
        {
            xTLSParams.ulSize = sizeof( xTLSParams );
            xTLSParams.pcDestination = pxContext->pcDestination;
            xTLSParams.pcServerCertificate = pxContext->pcServerCertificate;
            xTLSParams.ulServerCertificateLength = pxContext->ulServerCertificateLength;
            xTLSParams.pvCallerContext = pxContext;
            xTLSParams.pxNetworkRecv = prvNetworkRecv;
            xTLSParams.pxNetworkSend = prvNetworkSend;
            lStatus = TLS_Init( &pxContext->pvTLSContext, &xTLSParams );

            if( SOCKETS_ERROR_NONE == lStatus )
            {
                lStatus = TLS_Connect( pxContext->pvTLSContext );
                if( lStatus < 0 )
                {
                    lStatus = SOCKETS_TLS_HANDSHAKE_ERROR;
                }
            }
        }
    }
    else
    {
        lStatus = SOCKETS_SOCKET_ERROR;
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Receive data from a TCP socket.
 *
 * The socket must have already been created using a call to SOCKETS_Socket()
 * and connected to a remote socket using SOCKETS_Connect().
 *
 * @param[in] xSocket The handle of the socket from which data is being received.
 * @param[out] pvBuffer The buffer into which the received data will be placed.
 * @param[in] xBufferLength The maximum number of bytes which can be received.
 * pvBuffer must be at least xBufferLength bytes long.
 * @param[in] ulFlags Not currently used. Should be set to 0.
 *
 * @return
 * * If the receive was successful then the number of bytes received (placed in the
 *   buffer pointed to by pvBuffer) is returned.
 * * If a timeout occurred before data could be received then 0 is returned (timeout
 *   is set using @ref SOCKETS_SO_RCVTIMEO).
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Recv( Socket_t xSocket,
                      void * pvBuffer,
                      size_t xBufferLength,
                      uint32_t ulFlags )
{
    int32_t lStatus = SOCKETS_SOCKET_ERROR;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */

    pxContext->xRecvFlags = ( BaseType_t ) ulFlags;

    if( ( xSocket != SOCKETS_INVALID_SOCKET ) &&
        ( pvBuffer != NULL ) )
    {
        if( pdTRUE == pxContext->xRequireTLS )
        {
            /* Receive through TLS pipe, if negotiated. */
            lStatus = TLS_Recv( pxContext->pvTLSContext, pvBuffer, xBufferLength );
        }
        else
        {
            /* Receive unencrypted. */
            lStatus = prvNetworkRecv( pxContext, pvBuffer, xBufferLength );
        }
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Transmit data to the remote socket.
 *
 * The socket must have already been created using a call to SOCKETS_Socket() and
 * connected to a remote socket using SOCKETS_Connect().
 *
 * @param[in] xSocket The handle of the sending socket.
 * @param[in] pvBuffer The buffer containing the data to be sent.
 * @param[in] xDataLength The length of the data to be sent.
 * @param[in] ulFlags Not currently used. Should be set to 0.
 *
 * @return
 * * On success, the number of bytes actually sent is returned.
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Send( Socket_t xSocket,
                      const void * pvBuffer,
                      size_t xDataLength,
                      uint32_t ulFlags )
{
    int32_t lStatus = SOCKETS_SOCKET_ERROR;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */

    if( ( xSocket != SOCKETS_INVALID_SOCKET ) &&
        ( pvBuffer != NULL ) )
    {
        pxContext->xSendFlags = ( BaseType_t ) ulFlags;

        if( pdTRUE == pxContext->xRequireTLS )
        {
            /* Send through TLS pipe, if negotiated. */
            lStatus = TLS_Send( pxContext->pvTLSContext, pvBuffer, xDataLength );
        }
        else
        {
            /* Send unencrypted. */
            lStatus = prvNetworkSend( pxContext, pvBuffer, xDataLength );
        }
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Closes all or part of a full-duplex connection on the socket.
 *
 * @param[in] xSocket The handle of the socket to shutdown.
 * @param[in] ulHow SOCKETS_SHUT_RD, SOCKETS_SHUT_WR or SOCKETS_SHUT_RDWR.
 * @ref ShutdownFlags
 *
 * @return
 * * If the operation was successful, 0 is returned.
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Shutdown( Socket_t xSocket,
                          uint32_t ulHow )
{
	int32_t lStatus = SOCKETS_SOCKET_ERROR;
	wifi_err_t wifi_ret;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */

    if(( NULL == pxContext ) || (pxContext == SOCKETS_INVALID_SOCKET) || (pxContext->socket_no >= MAX_NUM_SSOCKETS))
    {
    	return SOCKETS_EINVAL;
    }
    else if((ulHow != SOCKETS_SHUT_RD) && (ulHow != SOCKETS_SHUT_WR) && (ulHow != SOCKETS_SHUT_RDWR))
    {
    	return SOCKETS_EINVAL;
    }

    wifi_ret = R_WIFI_SX_ULPGN_SocketShutdown(pxContext->socket_no);
    if(WIFI_SUCCESS == wifi_ret)
    {
    	lStatus = SOCKETS_ERROR_NONE;
    }
	return lStatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Closes the socket and frees the related resources.
 *
 * @param[in] xSocket The handle of the socket to close.
 *
 * @return
 * * On success, 0 is returned.
 * * If an error occurred, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_Close( Socket_t xSocket )
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */

    if( ( NULL != pxContext ) && ( SOCKETS_INVALID_SOCKET != pxContext ) && (pxContext->socket_no < MAX_NUM_SSOCKETS))
    {
        if (xSemaphoreTake( xUcInUse, xMaxSemaphoreBlockTime) != pdTRUE)
        {
        	return SOCKETS_SOCKET_ERROR;
        }
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
		R_WIFI_SX_ULPGN_SocketClose(pxContext->socket_no);
		vPortFree( pxContext );
		xSemaphoreGive(xUcInUse);
    }
    else
    {
    	return SOCKETS_EINVAL;
    }

    return pdFREERTOS_ERRNO_NONE;
}
/*-----------------------------------------------------------*/

/**
 * @brief Manipulates the options for the socket.
 *
 * @param[in] xSocket The handle of the socket to set the option for.
 * @param[in] lLevel Not currently used. Should be set to 0.
 * @param[in] lOptionName See @ref SetSockOptOptions.
 * @param[in] pvOptionValue A buffer containing the value of the option to set.
 * @param[in] xOptionLength The length of the buffer pointed to by pvOptionValue.
 *
 * @note Socket option support and possible values vary by port. Please see
 * PORT_SPECIFIC_LINK to check the valid options and limitations of your device.
 *
 *  - Berkeley Socket Options
 *    - @ref SOCKETS_SO_RCVTIMEO
 *      - Sets the receive timeout
 *      - pvOptionValue (TickType_t) is the number of milliseconds that the
 *      receive function should wait before timing out.
 *      - Setting pvOptionValue = 0 causes receive to wait forever.
 *      - See PORT_SPECIFIC_LINK for device limitations.
 *    - @ref SOCKETS_SO_SNDTIMEO
 *      - Sets the send timeout
 *      - pvOptionValue (TickType_t) is the number of milliseconds that the
 *      send function should wait before timing out.
 *      - Setting pvOptionValue = 0 causes send to wait forever.
 *      - See PORT_SPECIFIC_LINK for device limitations.
 *  - Non-Standard Options
 *    - @ref SOCKETS_SO_NONBLOCK
 *      - Makes a socket non-blocking.
 *      - pvOptionValue is ignored for this option.
 *  - Security Sockets Options
 *    - @ref SOCKETS_SO_REQUIRE_TLS
 *      - Use TLS for all connect, send, and receive on this socket.
 *      - This socket options MUST be set for TLS to be used, even
 *        if other secure socket options are set.
 *      - pvOptionValue is ignored for this option.
 *    - @ref SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE
 *      - Set the root of trust server certificiate for the socket.
 *      - This socket option only takes effect if @ref SOCKETS_SO_REQUIRE_TLS
 *        is also set.  If @ref SOCKETS_SO_REQUIRE_TLS is not set,
 *        this option will be ignored.
 *      - pvOptionValue is a pointer to the formatted server certificate.
 *        TODO: Link to description of how to format certificates with \n
 *      - xOptionLength (BaseType_t) is the length of the certificate
 *        in bytes.
 *    - @ref SOCKETS_SO_SERVER_NAME_INDICATION
 *      - Use Server Name Indication (SNI)
 *      - This socket option only takes effect if @ref SOCKETS_SO_REQUIRE_TLS
 *        is also set.  If @ref SOCKETS_SO_REQUIRE_TLS is not set,
 *        this option will be ignored.
 *      - pvOptionValue is a pointer to a string containing the hostname
 *      - xOptionLength is the length of the hostname string in bytes.
 *
 * @return
 * * On success, 0 is returned.
 * * If an error occured, a negative value is returned. @ref SocketsErrors
 */
int32_t SOCKETS_SetSockOpt( Socket_t xSocket,
                            int32_t lLevel,
                            int32_t lOptionName,
                            const void * pvOptionValue,
                            size_t xOptionLength )
{
    int32_t lStatus = SOCKETS_ERROR_NONE;
    TickType_t xTimeout;
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) xSocket; /*lint !e9087 cast used for portability. */

    switch( lOptionName )
    {
        case SOCKETS_SO_SERVER_NAME_INDICATION:

            /* Non-NULL destination string indicates that SNI extension should
             * be used during TLS negotiation. */
            if( NULL == ( pxContext->pcDestination =
                              ( char * ) pvPortMalloc( 1U + xOptionLength ) ) )
            {
                lStatus = SOCKETS_ENOMEM;
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
                lStatus = SOCKETS_ENOMEM;
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
            pxContext->ulSendTimeout = 1000;
            pxContext->ulRecvTimeout = 2;
            break;

        case SOCKETS_SO_RCVTIMEO:
            /* Comply with Berkeley standard - a 0 timeout is wait forever. */
            xTimeout = *( ( const TickType_t * ) pvOptionValue ); /*lint !e9087 pvOptionValue passed should be of TickType_t */

            if( xTimeout == 0U )
            {
                xTimeout = portMAX_DELAY;
            }
        	pxContext->ulRecvTimeout = xTimeout;
            break;
        case SOCKETS_SO_SNDTIMEO:
            /* Comply with Berkeley standard - a 0 timeout is wait forever. */
            xTimeout = *( ( const TickType_t * ) pvOptionValue ); /*lint !e9087 pvOptionValue passed should be of TickType_t */

            if( xTimeout == 0U )
            {
                xTimeout = portMAX_DELAY;
            }
        	pxContext->ulSendTimeout = xTimeout;
            break;

        default:
            lStatus = SOCKETS_ENOPROTOOPT;
//            FreeRTOS_setsockopt( pxContext->xSocket,
//                                           lLevel,
//                                           lOptionName,
//                                           pvOptionValue,
//                                           xOptionLength );
            break;
    }

    return lStatus;
}
/*-----------------------------------------------------------*/

/**
 * @brief Resolve a host name using Domain Name Service.
 *
 * @param[in] pcHostName The host name to resolve.
 * @return
 * * The IPv4 address of the specified host.
 * * If an error has occured, 0 is returned.
 */
uint32_t SOCKETS_GetHostByName( const char * pcHostName )
{
	uint32_t ulAddr = 0;
	uint32_t ret;

	ret = R_WIFI_SX_ULPGN_DnsQuery((uint8_t *)pcHostName, &ulAddr);
	if(0 == ret)
	{
		ulAddr = SOCKETS_htonl( ulAddr );
	}
	return ulAddr;
}
/*-----------------------------------------------------------*/

/**
 * @brief Secure Sockets library initialization function.
 *
 * This function does general initialization and setup. It must be called once
 * and only once before calling any other function.
 *
 * @return
 * * @ref pdPASS if everything succeeds
 * * @ref pdFAIL otherwise.
 */
BaseType_t SOCKETS_Init( void )
{
    /* FIX ME. */
    BaseType_t xResult = pdFAIL;

    /* Create the global mutex which is used to ensure
     * that only one socket is accessing the ucInUse bits in
     * the socket array. */
	xUcInUse = xSemaphoreCreateMutex();
	if (xUcInUse == NULL)
	{
		return pdFAIL;
	}
	xResult = pdPASS;
    /* Success. */

    return xResult;
}
/*-----------------------------------------------------------*/

static CK_RV prvSocketsGetCryptoSession( CK_SESSION_HANDLE * pxSession,
                                         CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{
    CK_RV xResult = 0;
    CK_C_GetFunctionList pxCkGetFunctionList = NULL;
    static CK_SESSION_HANDLE xPkcs11Session = 0;
    static CK_FUNCTION_LIST_PTR pxPkcs11FunctionList = NULL;
    CK_ULONG ulCount = 1;
    CK_SLOT_ID xSlotId = 0;

    portENTER_CRITICAL();

    if( 0 == xPkcs11Session )
    {
        /* One-time initialization. */

        /* Ensure that the PKCS#11 module is initialized. */
        if( 0 == xResult )
        {
            pxCkGetFunctionList = C_GetFunctionList;
            xResult = pxCkGetFunctionList( &pxPkcs11FunctionList );
        }

        if( 0 == xResult )
        {
            xResult = pxPkcs11FunctionList->C_Initialize( NULL );
        }

        /* Get the default slot ID. */
        if( ( 0 == xResult ) || ( CKR_CRYPTOKI_ALREADY_INITIALIZED == xResult ) )
        {
            xResult = pxPkcs11FunctionList->C_GetSlotList( CK_TRUE,
                                                           &xSlotId,
                                                           &ulCount );
        }

        /* Start a session with the PKCS#11 module. */
        if( 0 == xResult )
        {
            xResult = pxPkcs11FunctionList->C_OpenSession( xSlotId,
                                                           CKF_SERIAL_SESSION,
                                                           NULL,
                                                           NULL,
                                                           &xPkcs11Session );
        }
    }

    portEXIT_CRITICAL();

    /* Output the shared function pointers and session handle. */
    *ppxFunctionList = pxPkcs11FunctionList;
    *pxSession = xPkcs11Session;

    return xResult;
}
/*-----------------------------------------------------------*/

/**
 * @brief Generate a TCP Initial Sequence Number that is reasonably difficult
 * to predict, per https://tools.ietf.org/html/rfc6528.
 */
uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                             uint16_t usSourcePort,
                                             uint32_t ulDestinationAddress,
                                             uint16_t usDestinationPort )
{
    CK_RV xResult = 0;
    CK_SESSION_HANDLE xPkcs11Session = 0;
    CK_FUNCTION_LIST_PTR pxPkcs11FunctionList = NULL;
    CK_MECHANISM xMechSha256 = { 0 };
    uint8_t ucSha256Result[ cryptoSHA256_DIGEST_BYTES ];
    CK_ULONG ulLength = sizeof( ucSha256Result );
    uint32_t ulNextSequenceNumber = 0;
    static uint64_t ullKey = 0;

    /* Acquire a crypto session handle. */
    xResult = prvSocketsGetCryptoSession( &xPkcs11Session,
                                          &pxPkcs11FunctionList );

    if( 0 == xResult )
    {
        if( 0 == ullKey )
        {
            /* One-time initialization, per boot, of the random seed. */
            xResult = pxPkcs11FunctionList->C_GenerateRandom( xPkcs11Session,
                                                              ( CK_BYTE_PTR ) &ullKey,
                                                              sizeof( ullKey ) );
        }
    }

    /* Lock the shared crypto session. */
    portENTER_CRITICAL();

    /* Start a hash. */
    if( 0 == xResult )
    {
        xMechSha256.mechanism = CKM_SHA256;
        xResult = pxPkcs11FunctionList->C_DigestInit( xPkcs11Session, &xMechSha256 );
    }

    /* Hash the seed. */
    if( 0 == xResult )
    {
        xResult = pxPkcs11FunctionList->C_DigestUpdate( xPkcs11Session,
                                                        ( CK_BYTE_PTR ) &ullKey,
                                                        sizeof( ullKey ) );
    }

    /* Hash the source address. */
    if( 0 == xResult )
    {
        xResult = pxPkcs11FunctionList->C_DigestUpdate( xPkcs11Session,
                                                        ( CK_BYTE_PTR ) &ulSourceAddress,
                                                        sizeof( ulSourceAddress ) );
    }

    /* Hash the source port. */
    if( 0 == xResult )
    {
        xResult = pxPkcs11FunctionList->C_DigestUpdate( xPkcs11Session,
                                                        ( CK_BYTE_PTR ) &usSourcePort,
                                                        sizeof( usSourcePort ) );
    }

    /* Hash the destination address. */
    if( 0 == xResult )
    {
        xResult = pxPkcs11FunctionList->C_DigestUpdate( xPkcs11Session,
                                                        ( CK_BYTE_PTR ) &ulDestinationAddress,
                                                        sizeof( ulDestinationAddress ) );
    }

    /* Hash the destination port. */
    if( 0 == xResult )
    {
        xResult = pxPkcs11FunctionList->C_DigestUpdate( xPkcs11Session,
                                                        ( CK_BYTE_PTR ) &usDestinationPort,
                                                        sizeof( usDestinationPort ) );
    }

    /* Get the hash. */
    if( 0 == xResult )
    {
        xResult = pxPkcs11FunctionList->C_DigestFinal( xPkcs11Session,
                                                       ucSha256Result,
                                                       &ulLength );
    }

    portEXIT_CRITICAL();

    /* Use the first four bytes of the hash result as the starting point for
     * all initial sequence numbers for connections based on the input 4-tuple. */
    if( 0 == xResult )
    {
        memcpy( &ulNextSequenceNumber,
                ucSha256Result,
                sizeof( ulNextSequenceNumber ) );

        /* Add the tick count of four-tick intervals. In theory, per the RFC
         * (see above), this approach still allows server equipment to optimize
         * handling of connections from the same device that haven't fully timed out. */
        ulNextSequenceNumber += xTaskGetTickCount() / 4;
    }

    return ulNextSequenceNumber;
}
uint8_t get_socket_no(void *pvContext)
{
    SSOCKETContextPtr_t pxContext = ( SSOCKETContextPtr_t ) pvContext; /*lint !e9087 cast used for portability. */

	return pxContext->socket_no;
}
/*-----------------------------------------------------------*/
