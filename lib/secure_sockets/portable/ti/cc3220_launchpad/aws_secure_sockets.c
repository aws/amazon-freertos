/*
 * Amazon FreeRTOS Secure Sockets for CC3220SF-LAUNCHXL V1.0.5
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

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Secure socket interface. */
#include "aws_secure_sockets.h"

/* TI driver includes. */
#include <ti/drivers/net/wifi/simplelink.h>

/* Credentials includes. */
#include "aws_clientcredential.h"
#include "aws_default_root_certificates.h"



#define SOCKETS_PRINT( X )               vLoggingPrintf X


/**
 * @brief Maximum number of sockets.
 *
 * 16 Sockets including up to 6 connected secure sockets.
 */
#define securesocketsMAX_NUM_SOCKETS                    16

/**
 * @brief A Flag to indicate whether or not a socket is
 * secure i.e. it uses TLS or not.
 */
#define securesocketsSOCKET_SECURE_FLAG                 ( 1UL << 0 )

/**
 * @brief A flag to indicate whether or not a socket is closed
 * for receive.
 */
#define securesocketsSOCKET_READ_CLOSED_FLAG            ( 1UL << 1 )

/**
 * @brief A flag to indicate whether or not a socket is closed
 * for send.
 */
#define securesocketsSOCKET_WRITE_CLOSED_FLAG           ( 1UL << 2 )

/**
 * @brief A flag to indicate whether or not a non-default server
 * certificate has been bound to the socket.
 */
#define securesocketsSOCKET_TRUSTED_SERVER_CERT_FLAG    ( 1UL << 3 )

/**
 * @brief A flag to indicate whether or not the socket is connected.
 *
 */
#define securesocketsSOCKET_IS_CONNECTED                ( 1UL << 4 )

/**
 * @defgroup TLSHandshakeResults TLS handshake results.
 */
/* @{ */
#define securesocketsTLS_HANDSHAKE_SUCCESS    0             /**< TLS handshake successful. */
#define securesocketsTLS_HANDSHAKE_FAILED     1             /**< TLS handshake failed. */
#define securesocketsTLS_HANDSHAKE_INVALID    2             /**< TLS handshake invalid result. */
/* @} */

/*-----------------------------------------------------------*/

/*
 * @brief Internal context structure.
 *
 * Each socket created has a socket context structure associated with it.
 */
typedef struct SSocketContext
{
    uint8_t ucInUse;                     /**< Tracks whether the socket is in use or not. The xUcInUse mutex must be secured before modifying. */
    _i16 sSocketDescriptor;              /**< Socket descriptor as returned by the Simple Link. */
    uint32_t ulFlags;                    /**< Various properties of the socket (secured, non-blocking etc.). */
    TickType_t xSendTimeoutTicks;        /**< Send timeout in ticks. Currently unused because Simple Link socket layer does not provide socket send timeout.
                                          *   Currently we just store the user supplied timeout in the socket context and do not use it. In future, we may
                                          *   use it to simulate send timeout in our layer. */
    BaseType_t xSSLHandshakeResult;      /**< Result of the SSL handshake as reported in the socket event handler callback. */
    SemaphoreHandle_t xSSLSyncSemaphore; /**< Semaphore used to synchronize SSL operations with the socket event handler callback from Simple Link. */
} SSocketContext_t, * SSocketContextPtr_t;

/*-----------------------------------------------------------*/

/**
 * @brief The global mutex to ensure that only one operation is accessing the
 * SSocketContext.ucInUse flag at one time.
 */
static SemaphoreHandle_t xUcInUse = NULL;

/**
 * @brief Controls global access to the sl_NetAppDnsGetHostByName, which
 * should only be called by one thread at a time.
 */
static SemaphoreHandle_t xGetHostByName = NULL;

/**
 * @brief Maximum time in ticks to wait for obtaining a semaphore.
 */
static const TickType_t xMaxSemaphoreBlockTime = pdMS_TO_TICKS( 60000UL );

/**
 * @brief Secure socket contexts.
 *
 * An index in this array is returned to the user from SOCKETS_Socket
 * function.
 */
static SSocketContext_t xSockets[ securesocketsMAX_NUM_SOCKETS ] = { 0 };

/*-----------------------------------------------------------*/

/**
 * @brief Gets a free socket from the free socket pool.
 *
 * Iterates over the xSockets array to see if it can find
 * a free socket. A free or unused socket is indicated by
 * the zero value of the ucInUse member of SSocketContext_t.
 *
 * @return Index of the socket in the xSockets array, if it is
 * able to find a free socket, SOCKETS_INVALID_SOCKET otherwise.
 */
static uint32_t prvGetFreeSocket( void );

/**
 * @brief Finds the socket in the xSockets array matching the given
 * socket descriptor.
 *
 * @param[in] sSocketDescriptor The given socket descriptor to match.
 *
 * @return Index of the socket in the xSockets array, if a matching
 * socket was found, SOCKETS_INVALID_SOCKET otherwise.
 */
static uint32_t prvGetSocketForDescriptor( _i16 sSocketDescriptor );

/**
 * @brief Returns the socket back to the free socket pool.
 *
 * Marks the socket as free by setting ucInUse member of the
 * SSocketContext_t structure as zero.
 *
 * @param[in] ulSocketNumber Index of the socket in the xSockets
 * array to be marked as free.
 *
 * @return pdTRUE if socket was successfully returned, pdFALSE otherwise.
 */
static BaseType_t prvReturnSocket( uint32_t ulSocketNumber );

/**
 * @brief Checks whether or not the provided socket number is valid.
 *
 * Ensures that the provided number is less than securesocketsMAX_NUM_SOCKETS
 * and the socket is "in-use" i.e. ucInUse is set to non-zero in the socket
 * context structure.
 *
 * @param[in] ulSocketNumber The provided socket number to check.
 *
 * @return pdTRUE if the socket is valid, pdFALSE otherwise.
 */
static BaseType_t prvIsValidSocket( uint32_t ulSocketNumber );



/**
 * @brief Configures the required security settings for the TI socket corresponding
 * to the given socket context.
 *
 * It sets the security protocols, root CA, client certificate and keys
 * and ALPN list on the corresponding TI socket.
 *
 * @param[in] pxSocketContext The given socket context for which to configure the
 * security settings.
 *
 * @return SOCKETS_ERROR_NONE if everything succeeds, SOCKETS_SOCKET_ERROR otherwise.
 */
static int32_t prvSetupSecurity( const SSocketContextPtr_t pxSocketContext );

/**
 * @brief Write the given certificate to the file system.
 *
 * @param[in] pcDeviceFileName The filename to write the certificate to.
 * @param[in] pcCertificate The certificate to write.
 * @param[in] ulCertificateSize The size of the above certificate.
 *
 * @return SOCKETS_ERROR_NONE if everything succeeds, SOCKETS_SOCKET_ERROR otherwise.
 */
static int32_t prvWriteCertificate( const char * pcDeviceFileName,
                                    const char * pcCertificate,
                                    uint32_t ulCertificateSize );

/*-----------------------------------------------------------*/

static uint32_t prvGetFreeSocket( void )
{
    uint32_t ulSocketNumber;

    /* Obtain the socketInUse mutex. */
    if( xSemaphoreTake( xUcInUse, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
        /* Iterate over xSockets array to see if any free socket is available. */
        for( ulSocketNumber = 0; ulSocketNumber < ( uint32_t ) securesocketsMAX_NUM_SOCKETS; ulSocketNumber++ )
        {
            if( xSockets[ ulSocketNumber ].ucInUse == 0U )
            {
                /* Mark the socket as "in-use". */
                xSockets[ ulSocketNumber ].ucInUse = 1;

                /* We have found a free socket, so stop. */
                break;
            }
        }

        /* Give back the socketInUse mutex. */
        ( void ) xSemaphoreGive( xUcInUse );
    }
    else
    {
        /* Failed to obtain socketInUse mutex.
         * Set socket number to an invalid value. */
        ulSocketNumber = securesocketsMAX_NUM_SOCKETS;
    }

    /* Did we find a free socket? */
    if( ulSocketNumber == ( uint32_t ) securesocketsMAX_NUM_SOCKETS )
    {
        /* Return SOCKETS_INVALID_SOCKET if we fail to
         * find a free socket. */
        ulSocketNumber = ( uint32_t ) SOCKETS_INVALID_SOCKET;
    }

    return ulSocketNumber;
}
/*-----------------------------------------------------------*/

static uint32_t prvGetSocketForDescriptor( _i16 sSocketDescriptor )
{
    uint32_t ulSocketNumber;

    /* Obtain the socketInUse mutex. */
    if( xSemaphoreTake( xUcInUse, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
        /* Iterate over xSockets array to see if any socket context which
         * is "in-use", matches the given socket descriptor. */
        for( ulSocketNumber = 0; ulSocketNumber < ( uint32_t ) securesocketsMAX_NUM_SOCKETS; ulSocketNumber++ )
        {
            if( ( xSockets[ ulSocketNumber ].ucInUse == 1U ) &&
                ( xSockets[ ulSocketNumber ].sSocketDescriptor == sSocketDescriptor ) )
            {
                break;
            }
        }

        /* Give back the socketInUse mutex. */
        ( void ) xSemaphoreGive( xUcInUse );
    }
    else
    {
        ulSocketNumber = securesocketsMAX_NUM_SOCKETS;
    }

    /* Did we find the socket we were looking for? */
    if( ulSocketNumber == ( uint32_t ) securesocketsMAX_NUM_SOCKETS )
    {
        /* Return SOCKETS_INVALID_SOCKET if we fail to
         * find the socket. */
        ulSocketNumber = ( uint32_t ) SOCKETS_INVALID_SOCKET;
    }

    return ulSocketNumber;
}
/*-----------------------------------------------------------*/

static BaseType_t prvReturnSocket( uint32_t ulSocketNumber )
{
    BaseType_t xResult = pdFAIL;

    /* Obtain the socketInUse mutex. */
    if( xSemaphoreTake( xUcInUse, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
        /* Mark the socket as free. */
        xSockets[ ulSocketNumber ].ucInUse = 0;

        xResult = pdPASS;

        /* Give back the socketInUse mutex. */
        ( void ) xSemaphoreGive( xUcInUse );
    }

    return xResult;
}
/*-----------------------------------------------------------*/

static BaseType_t prvIsValidSocket( uint32_t ulSocketNumber )
{
    BaseType_t xValid = pdFALSE;

    /* Obtain the socketInUse mutex. */
    if( xSemaphoreTake( xUcInUse, xMaxSemaphoreBlockTime ) == pdTRUE )
    {
        /* Check that the provided socket number is within the valid
         * index range. */
        if( ulSocketNumber < ( uint32_t ) securesocketsMAX_NUM_SOCKETS )
        {
            /* Check that this socket is in use. */
            if( xSockets[ ulSocketNumber ].ucInUse == 1U )
            {
                /* This is a valid socket number. */
                xValid = pdTRUE;
            }
        }

        /* Give back the socketInUse mutex. */
        ( void ) xSemaphoreGive( xUcInUse );
    }

    return xValid;
}

/*-----------------------------------------------------------*/

static int32_t prvSetupSecurity( const SSocketContextPtr_t pxSocketContext )
{
    SlSockSecureMethod_t xSockSecureMethod;
    SlSockSecureMask_t xSockSecureMask;
    SlSockSecureALPN_t xSockSecureAlpn;
    _i16 sTIRetCode;
    int32_t lRetCode = SOCKETS_ERROR_NONE;
    uint32_t ulDummyOptVal = 0;

    /* Select the correct TLS method. */
    xSockSecureMethod.SecureMethod = SL_SO_SEC_METHOD_TLSV1_2;
    sTIRetCode = sl_SetSockOpt( pxSocketContext->sSocketDescriptor,
                                SL_SOL_SOCKET,
                                SL_SO_SECMETHOD,
                                &( xSockSecureMethod ),
                                ( SlSocklen_t ) sizeof( xSockSecureMethod ) );

    if( sTIRetCode < 0 )
    {
        /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
        SOCKETS_PRINT( ( "ERROR: %d Failed to select TLS method.\r\n", sTIRetCode ) );
        lRetCode = SOCKETS_SOCKET_ERROR;
    }

    /* Select the security protocols. */
    if( lRetCode == SOCKETS_ERROR_NONE )
    {
        xSockSecureMask.SecureMask = SL_SEC_MASK_TLS_RSA_WITH_AES_128_GCM_SHA256; /*lint !e9027 constants defined as shifted int instead of shifted uint in TI file.
                                                                                   * MISRA does not permit << or | on signed integers. This can be waived since values
                                                                                   * are used as a mask, not for their numerical value. */
        sTIRetCode = sl_SetSockOpt( pxSocketContext->sSocketDescriptor,
                                    SL_SOL_SOCKET,
                                    SL_SO_SECURE_MASK,
                                    &( xSockSecureMask ),
                                    ( SlSocklen_t ) sizeof( xSockSecureMask ) );

        if( sTIRetCode < 0 )
        {
            /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
            SOCKETS_PRINT( ( "ERROR: %d Failed to set cipher suites.\r\n", sTIRetCode ) );
            lRetCode = SOCKETS_SOCKET_ERROR;
        }
    }

    /* Disable the root certificate catalog. */
    if( lRetCode == SOCKETS_ERROR_NONE )
    {
        sTIRetCode = sl_SetSockOpt( pxSocketContext->sSocketDescriptor,
                                    SL_SOL_SOCKET,
                                    SL_SO_SECURE_DISABLE_CERTIFICATE_STORE,
                                    &( ulDummyOptVal ),
                                    ( SlSocklen_t ) sizeof( ulDummyOptVal ) );

        if( sTIRetCode < 0 )
        {
            /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
            SOCKETS_PRINT( ( "ERROR: %d Failed to disable root certificate catalog.\r\n", sTIRetCode ) );
            lRetCode = SOCKETS_SOCKET_ERROR;
        }
    }

    /* Set the root certificate if a custom server cert was not already designated. */
    if( lRetCode == SOCKETS_ERROR_NONE )
    {
        if( 0 == ( pxSocketContext->ulFlags & securesocketsSOCKET_TRUSTED_SERVER_CERT_FLAG ) )
        {
            sTIRetCode = sl_SetSockOpt( pxSocketContext->sSocketDescriptor,
                                        SL_SOL_SOCKET,
                                        SL_SO_SECURE_FILES_CA_FILE_NAME,
                                        socketsconfigSECURE_FILE_NAME_ROOTCA,
                                        ( SlSocklen_t ) strlen( socketsconfigSECURE_FILE_NAME_ROOTCA ) );

            if( sTIRetCode < 0 )
            {
                /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
                SOCKETS_PRINT( ( "ERROR: %d Failed to set the root certificate.\r\n", sTIRetCode ) );
                lRetCode = SOCKETS_SOCKET_ERROR;
            }
        }
    }

    /* Set the client certificate. */
    if( lRetCode == SOCKETS_ERROR_NONE )
    {
        sTIRetCode = sl_SetSockOpt( pxSocketContext->sSocketDescriptor,
                                    SL_SOL_SOCKET,
                                    SL_SO_SECURE_FILES_CERTIFICATE_FILE_NAME,
                                    socketsconfigSECURE_FILE_NAME_CLIENTCERT,
                                    ( SlSocklen_t ) strlen( socketsconfigSECURE_FILE_NAME_CLIENTCERT ) );

        if( sTIRetCode < 0 )
        {
            /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
            SOCKETS_PRINT( ( "ERROR: %d Failed to set the client certificate.\r\n", sTIRetCode ) );
            lRetCode = SOCKETS_SOCKET_ERROR;
        }
    }

    /* Set the private keys. */
    if( lRetCode == SOCKETS_ERROR_NONE )
    {
        sTIRetCode = sl_SetSockOpt( pxSocketContext->sSocketDescriptor,
                                    SL_SOL_SOCKET,
                                    SL_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME,
                                    socketsconfigSECURE_FILE_NAME_PRIVATEKEY,
                                    ( SlSocklen_t ) strlen( socketsconfigSECURE_FILE_NAME_PRIVATEKEY ) );

        if( sTIRetCode < 0 )
        {
            /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
            SOCKETS_PRINT( ( "ERROR: %d Failed to set the private keys.\r\n", sTIRetCode ) );
            lRetCode = SOCKETS_SOCKET_ERROR;
        }
    }

    /* Set the Application Layer Protocol Negotiation (ALPN) list. */
    if( lRetCode == SOCKETS_ERROR_NONE )
    {
        xSockSecureAlpn.SecureALPN = SL_SECURE_ALPN_FULL_LIST; /*lint !e9027 shift operator used in define in TI file.
                                                                * Can be ignored since number used as mask and sign does not
                                                                * affect outcome. */
        sTIRetCode = sl_SetSockOpt( pxSocketContext->sSocketDescriptor,
                                    SL_SOL_SOCKET,
                                    SL_SO_SECURE_ALPN,
                                    &( xSockSecureAlpn ),
                                    ( SlSocklen_t ) sizeof( SlSockSecureALPN_t ) );

        if( sTIRetCode < 0 )
        {
            /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
            SOCKETS_PRINT( ( "ERROR: %d Failed to set the ALPN list.\r\n", sTIRetCode ) );
            lRetCode = SOCKETS_SOCKET_ERROR;
        }
    }

    return lRetCode;
}
/*-----------------------------------------------------------*/

static int32_t prvWriteCertificate( const char * pcDeviceFileName,
                                    const char * pcCertificate,
                                    uint32_t ulCertificateSize )
{
    _i32 lFileHandle, lTIRetCode;
    const uint16_t usMaxSize = 3000;
    int32_t lRetCode = SOCKETS_ERROR_NONE;
    unsigned long ulMasterToken = 0;

    /* Open the file to which to write the certificate. */
    lFileHandle = sl_FsOpen( ( const unsigned char * ) pcDeviceFileName,
                             SL_FS_CREATE | SL_FS_CREATE_NOSIGNATURE
                             | SL_FS_OVERWRITE | SL_FS_CREATE_MAX_SIZE( usMaxSize ), /*lint !e9029 !e9027  offending macros in a TI file. */
                             &( ulMasterToken ) );

    if( lFileHandle < 0 )
    {
        /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
        SOCKETS_PRINT( ( "ERROR: %d Failed to open file %s.\r\n", lFileHandle, pcDeviceFileName ) );
        lRetCode = SOCKETS_SOCKET_ERROR;
    }

    /* Write the certificate to the file. */
    if( lRetCode == SOCKETS_ERROR_NONE )
    {
        lTIRetCode = sl_FsWrite( lFileHandle,
                                 0U,                                      /* Offset zero means write from the beginning. */
                                 ( unsigned char * ) pcCertificate, /*lint !e605 cert bytes must be cast as unsigned for compliance with TI file. */
                                 ulCertificateSize );

        if( lTIRetCode < 0 )
        {
            /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
            SOCKETS_PRINT( ( "ERROR: %d Failed to write certificate to file %s.\r\n", lTIRetCode, pcDeviceFileName ) );
            lRetCode = SOCKETS_SOCKET_ERROR;
        }

        /* Close the file. */
        ( void ) sl_FsClose( lFileHandle, NULL, NULL, 0 );
    }

    return lRetCode;
}
/*-----------------------------------------------------------*/

BaseType_t SOCKETS_Init( void )
{
    BaseType_t xResult = pdFAIL;

    /* Create the global mutex which is used to ensure
     * that only one socket is accessing the ucInUse bits in
     * the socket array. */
    xUcInUse = xSemaphoreCreateMutex();

    if( xUcInUse != NULL )
    {
        xGetHostByName = xSemaphoreCreateMutex();

        if( xGetHostByName != NULL )
        {
            /* Success. */
            xResult = pdPASS;
        }
        else
        {
            SOCKETS_PRINT( ( "ERROR: SOCKETS_Init - Failed to create the GetHostByName mutex. \r\n" ) );
        }
    }
    else
    {
        SOCKETS_PRINT( ( "ERROR: SOCKETS_Init - Failed to create the SocketInUse mutex.\r\n" ) );
        xResult = pdFAIL;
    }

    return xResult;
}
/*-----------------------------------------------------------*/

Socket_t SOCKETS_Socket( int32_t lDomain,
                         int32_t lType,
                         int32_t lProtocol )
{
    Socket_t xCreatedSocket = SOCKETS_INVALID_SOCKET;
    SSocketContextPtr_t pxSocketContext;
    TickType_t xTimeoutTicks;
    int32_t lRetCode = SOCKETS_ERROR_NONE;
    uint32_t ulSocketNumber;

    /* Ensure that only supported values are supplied. */
    configASSERT( lDomain == SOCKETS_AF_INET );
    configASSERT( lType == SOCKETS_SOCK_STREAM );
    configASSERT( lProtocol == SOCKETS_IPPROTO_TCP );

    /* Get a free socket from the free socket pool. */
    ulSocketNumber = prvGetFreeSocket();

    /* Check that we indeed got a free socket. */
    if( ulSocketNumber != ( uint32_t ) SOCKETS_INVALID_SOCKET )
    {
        /* Socket to be returned to the user. */
        xCreatedSocket = ( Socket_t ) ulSocketNumber;

        /* Shortcut for easy access. */
        pxSocketContext = &( xSockets[ ulSocketNumber ] );

        /* Initialize socket context. */
        pxSocketContext->ulFlags = 0;
        pxSocketContext->xSSLSyncSemaphore = NULL;
        pxSocketContext->xSSLHandshakeResult = 0;

        /* Create the socket on the TI network processor. */
        pxSocketContext->sSocketDescriptor = sl_Socket( SL_AF_INET, SL_SOCK_STREAM, 0 );

        /* If the TI socket was created successfully, set the
         * default timeouts. */
        if( pxSocketContext->sSocketDescriptor >= 0 )
        {
            /* Set the receive timeout to the default value. */
            xTimeoutTicks = ( TickType_t ) socketsconfigDEFAULT_RECV_TIMEOUT;

            /* Since we already have the global synchronization mutex,
             * call the internal version of SetSockOpt. */
            lRetCode = SOCKETS_SetSockOpt( xCreatedSocket,
                                           0, /* lLevel - Unused. */
                                           SOCKETS_SO_RCVTIMEO,
                                           &( xTimeoutTicks ),
                                           sizeof( xTimeoutTicks ) );

            if( lRetCode == SOCKETS_ERROR_NONE )
            {
                /* Set the send timeout to the default value. */
                xTimeoutTicks = ( TickType_t ) socketsconfigDEFAULT_SEND_TIMEOUT;

                /* Since we already have the global synchronization mutex,
                 * call the internal version of SetSockOpt. */
                lRetCode = SOCKETS_SetSockOpt( xCreatedSocket,
                                               0, /* lLevel - Unused. */
                                               SOCKETS_SO_SNDTIMEO,
                                               &( xTimeoutTicks ),
                                               sizeof( xTimeoutTicks ) );
            }

            /* If we failed to set send or receive timeout, close
             * the socket. */
            if( lRetCode != SOCKETS_ERROR_NONE )
            {
                ( void ) SOCKETS_Close( xCreatedSocket );

                /* Inform the user about the failure by returning
                 * invalid socket. */
                xCreatedSocket = SOCKETS_INVALID_SOCKET;
            }
        }
        else
        {
            /* If the TI Socket could not be created, return the socket
             * context back to the free socket pool. */
            SOCKETS_PRINT( ( "ERROR: %d Failed to create TI socket.\r\n", pxSocketContext->sSocketDescriptor ) );
            prvReturnSocket( ulSocketNumber );

            /* Inform the user about the failure by returning
             * invalid socket. */
            xCreatedSocket = SOCKETS_INVALID_SOCKET;
        }
    }
    else
    {
        SOCKETS_PRINT( ( "ERROR: No more free sockets available.\r\n" ) );
    }

    return xCreatedSocket;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Connect( Socket_t xSocket,
                         SocketsSockaddr_t * pxAddress,
                         Socklen_t xAddressLength )
{
    SlSockAddrIn_t xAddress;
    SSocketContextPtr_t pxSocketContext;
    _i16 sTIRetCode;
    int32_t lRetCode = SOCKETS_ERROR_NONE;
    uint32_t ulDummyOptVal = 1;
    uint32_t ulSocketNumber = ( uint32_t ) xSocket;

    /* Remove compiler warnings about unused parameters. */
    ( void ) xAddressLength;

    configASSERT( pxAddress != NULL );

    /* Ensure that the socket is valid. */
    if( prvIsValidSocket( ulSocketNumber ) == pdTRUE )
    {
        /* Shortcut for easy access. */
        pxSocketContext = &( xSockets[ ulSocketNumber ] );

        /* Check that the socket is not already connected. */
        if( ( pxSocketContext->ulFlags & securesocketsSOCKET_IS_CONNECTED ) != 0UL )
        {
            lRetCode = SOCKETS_SOCKET_ERROR;
        }

        /* If the socket is a secure socket, setup security for it. */
        if( ( lRetCode == SOCKETS_ERROR_NONE ) &&
            ( ( pxSocketContext->ulFlags & securesocketsSOCKET_SECURE_FLAG ) != 0UL ) )
        {
            lRetCode = prvSetupSecurity( pxSocketContext );
        }

        /* Connect. */
        if( lRetCode == SOCKETS_ERROR_NONE )
        {
            /* Set up the address to connect to. */
            xAddress.sin_family = SL_AF_INET;
            xAddress.sin_port = pxAddress->usPort;
            xAddress.sin_addr.s_addr = pxAddress->ulAddress;

            /* Initiate connect. */
            sTIRetCode = sl_Connect( pxSocketContext->sSocketDescriptor,
                                     ( const SlSockAddr_t * ) &( xAddress ), /*lint !e9087 !e740 SlSockAddr_t behaves like a union of SlSockAddrIn_t.
                                                                             * SlSockAddrIn_t is the appropriate interpretation of a IPv4 address. */
                                     ( _i16 ) sizeof( SlSockAddrIn_t ) );

            /* If connection is successful, mark the socket as connected. */
            if( sTIRetCode == 0 )
            {
                pxSocketContext->ulFlags |= securesocketsSOCKET_IS_CONNECTED;
            }
            else
            {
                /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
                SOCKETS_PRINT( ( "ERROR: %d Socket failed to connect.\r\n", sTIRetCode ) );
                lRetCode = SOCKETS_SOCKET_ERROR;
            }

            /* If connection is successful and the socket is a secure
             * socket, start TLS. */
            if( ( lRetCode == SOCKETS_ERROR_NONE ) && ( ( pxSocketContext->ulFlags & securesocketsSOCKET_SECURE_FLAG ) != 0UL ) )
            {
                /* Clear the TLS handshake result before starting the TLS handshake. */
                pxSocketContext->xSSLHandshakeResult = securesocketsTLS_HANDSHAKE_INVALID;

                /* Initiate the TLS handshake. */
                sTIRetCode = sl_SetSockOpt( pxSocketContext->sSocketDescriptor, /*lint !e613 pxSocketContext is checked for null at top. */
                                            SL_SOL_SOCKET,
                                            SL_SO_STARTTLS,
                                            &( ulDummyOptVal ),
                                            ( SlSocklen_t ) sizeof( ulDummyOptVal ) );

                if( sTIRetCode >= 0 )
                {
                    /* When the TLS handshake completes, SimpleLinkSockEventHandler is called
                     * to inform about the success or failure. Block the calling task here
                     * and wait for the SimpleLinkSockEventHandler to unblock us. This semaphore
                     * is given in the SimpleLinkSockEventHandler to unblock this task. */
                    if( xSemaphoreTake( pxSocketContext->xSSLSyncSemaphore, portMAX_DELAY ) == pdTRUE )
                    {
                        /* Check whether or not the handshake was successful and inform the user. */
                        if( pxSocketContext->xSSLHandshakeResult != securesocketsTLS_HANDSHAKE_SUCCESS )
                        {
                            lRetCode = SOCKETS_TLS_HANDSHAKE_ERROR;
                        }
                    }
                    else
                    {
                        /* We should never get here. */
                        lRetCode = SOCKETS_TLS_HANDSHAKE_ERROR;
                    }
                }
                else
                {
                    /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
                    SOCKETS_PRINT( ( "ERROR: %d Failed to start TLS Handshake.\r\n", sTIRetCode ) );
                    lRetCode = SOCKETS_TLS_INIT_ERROR;
                }
            }
        }
    }
    else
    {
        SOCKETS_PRINT( ( "ERROR: Invalid socket number in SOCKETS_Connect.\r\n" ) );
        lRetCode = SOCKETS_EINVAL;
    }

    return lRetCode;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Recv( Socket_t xSocket,
                      void * pvBuffer,
                      size_t xBufferLength,
                      uint32_t ulFlags )
{
    SSocketContextPtr_t pxSocketContext;
    _i16 sTIRetCode;
    int32_t lRetCode = SOCKETS_SOCKET_ERROR;
    uint32_t ulSocketNumber = ( uint32_t ) xSocket;


    /* Ensure that the socket is valid and the passed buffer is not NULL. */
    if( ( prvIsValidSocket( ulSocketNumber ) == pdTRUE ) && ( pvBuffer != NULL ) )
    {
        /* Shortcut for easy access. */
        pxSocketContext = &( xSockets[ ulSocketNumber ] );

        /* Check that receive is allowed on the socket. */
        if( ( pxSocketContext->ulFlags & securesocketsSOCKET_READ_CLOSED_FLAG ) == 0UL )
        {
            /* Receive on the TI socket. */
            sTIRetCode = sl_Recv( pxSocketContext->sSocketDescriptor, pvBuffer, ( _i16 ) xBufferLength, ( _i16 ) 0 );

            if( sTIRetCode < 0 )
            {
                if( sTIRetCode == SL_ERROR_BSD_EWOULDBLOCK )
                {
                    /* If sl_Recv returned SL_ERROR_BSD_EWOULDBLOCK, return SOCKETS_EWOULDBLOCK. */
                    lRetCode = SOCKETS_EWOULDBLOCK;
                }
                else
                {
                    /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
                    SOCKETS_PRINT( ( "ERROR: %d Socket receive failed.\r\n", sTIRetCode ) );
                    lRetCode = SOCKETS_SOCKET_ERROR;
                }
            }
            else
            {
                /* In case of success, return the number of bytes
                 * read to the user. */
                lRetCode = sTIRetCode;
            }
        }
        else
        {
            /* The socket has been closed for read. */
            lRetCode = SOCKETS_ECLOSED;
        }
    }
    else
    {
        SOCKETS_PRINT( ( "ERROR: Invalid Socket number or NULL pvBuffer in SOCKETS_Recv.\r\n" ) );
        lRetCode = SOCKETS_EINVAL;
    }

    return lRetCode;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Send( Socket_t xSocket,
                      const void * pvBuffer,
                      size_t xDataLength,
                      uint32_t ulFlags )
{
    SSocketContextPtr_t pxSocketContext;
    _i16 sTIRetCode;
    int32_t lRetCode = SOCKETS_SOCKET_ERROR;
    uint32_t ulSocketNumber = ( uint32_t ) xSocket;


    /* Ensure that the socket is valid and the passed buffer is not NULL. */
    if( ( prvIsValidSocket( ulSocketNumber ) == pdTRUE ) && ( pvBuffer != NULL ) )
    {
        /* Shortcut for easy access. */
        pxSocketContext = &( xSockets[ ulSocketNumber ] );

        /* Check that send is allowed on the socket. */
        if( ( pxSocketContext->ulFlags & securesocketsSOCKET_WRITE_CLOSED_FLAG ) == 0UL )
        {
            /* Send on the TI socket. */
            sTIRetCode = sl_Send( pxSocketContext->sSocketDescriptor, pvBuffer, ( _i16 ) xDataLength, ( _i16 ) 0 );

            if( sTIRetCode < 0 )
            {
                if( sTIRetCode == SL_ERROR_BSD_EWOULDBLOCK )
                {
                    /* If sl_Recv returned SL_ERROR_BSD_EWOULDBLOCK, return SOCKETS_EWOULDBLOCK. */
                    lRetCode = SOCKETS_EWOULDBLOCK;
                }
                else
                {
                    /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
                    SOCKETS_PRINT( ( "ERROR: %d Socket send failed.\r\n", sTIRetCode ) );
                    /* For any other error, return SOCKETS_SOCKET_ERROR. */
                    lRetCode = SOCKETS_SOCKET_ERROR;
                }
            }
            else
            {
                /* In case of success, return the number of bytes
                 * sent. */
                lRetCode = sTIRetCode;
            }
        }
        else
        {
            /* The socket has been closed for write. */
            lRetCode = SOCKETS_ECLOSED;
        }
    }
    else
    {
        SOCKETS_PRINT( ( "ERROR: Invalid Socket number or NULL pvBuffer in SOCKETS_Send.\r\n" ) );
        lRetCode = SOCKETS_EINVAL;
    }

    return lRetCode;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Shutdown( Socket_t xSocket,
                          uint32_t ulHow )
{
    SSocketContextPtr_t pxSocketContext;
    int32_t lRetCode = SOCKETS_SOCKET_ERROR;
    uint32_t ulSocketNumber = ( uint32_t ) xSocket;


    /* Ensure that the socket is valid. */
    if( prvIsValidSocket( ulSocketNumber ) == pdTRUE )
    {
        /* Shortcut for easy access. */
        pxSocketContext = &( xSockets[ ulSocketNumber ] );

        switch( ulHow )
        {
            case SOCKETS_SHUT_RD:

                /* Further receive calls on this socket should return error. */
                pxSocketContext->ulFlags |= securesocketsSOCKET_READ_CLOSED_FLAG;

                /* Return success to the user. */
                lRetCode = SOCKETS_ERROR_NONE;

                break;

            case SOCKETS_SHUT_WR:

                /* Further send calls on this socket should return error. */
                pxSocketContext->ulFlags |= securesocketsSOCKET_WRITE_CLOSED_FLAG;

                /* Return success to the user. */
                lRetCode = SOCKETS_ERROR_NONE;

                break;

            case SOCKETS_SHUT_RDWR:

                /* Further send or receive calls on this socket should return error. */
                pxSocketContext->ulFlags |= securesocketsSOCKET_READ_CLOSED_FLAG;
                pxSocketContext->ulFlags |= securesocketsSOCKET_WRITE_CLOSED_FLAG;

                /* Return success to the user. */
                lRetCode = SOCKETS_ERROR_NONE;

                break;

            default:

                /* An invalid value was passed for ulHow. */
                lRetCode = SOCKETS_EINVAL;

                break;
        }
    }
    else
    {
        SOCKETS_PRINT( ( "ERROR: Invalid socket number in SOCKETS_Shutdown.\r\n" ) );
        lRetCode = SOCKETS_EINVAL;
    }

    return lRetCode;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_Close( Socket_t xSocket )
{
    SSocketContextPtr_t pxSocketContext;
    _i16 sTIRetCode;
    int32_t lRetCode = SOCKETS_SOCKET_ERROR;
    uint32_t ulSocketNumber = ( uint32_t ) xSocket;

    /* Ensure that the socket is valid. */
    if( prvIsValidSocket( ulSocketNumber ) == pdTRUE )
    {
        /* Shortcut for easy access. */
        pxSocketContext = &( xSockets[ ulSocketNumber ] );

        /* Mark the socket as closed. */
        pxSocketContext->ulFlags |= securesocketsSOCKET_READ_CLOSED_FLAG;
        pxSocketContext->ulFlags |= securesocketsSOCKET_WRITE_CLOSED_FLAG;

        /* Close the TI socket. */
        sTIRetCode = sl_Close( pxSocketContext->sSocketDescriptor );

        /* If TI socket was closed successfully, free up the internal
         * resources. */
        if( sTIRetCode >= 0 )
        {
            /* Free up TLS handshake synchronization semaphore. */
            if( pxSocketContext->xSSLSyncSemaphore != NULL )
            {
                vSemaphoreDelete( pxSocketContext->xSSLSyncSemaphore );
            }

            /* Return the socket back to the free socket pool. */
            prvReturnSocket( ulSocketNumber );

            /* Success. */
            lRetCode = SOCKETS_ERROR_NONE;
        }
        else
        {
            /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
            SOCKETS_PRINT( ( "ERROR: %d Socket close failed.\r\n", sTIRetCode ) );
        }
    }
    else
    {
        SOCKETS_PRINT( ( "ERROR: Invalid socket number in SOCKETS_Close.\r\n" ) );
        lRetCode = SOCKETS_EINVAL;
    }

    return lRetCode;
}
/*-----------------------------------------------------------*/

int32_t SOCKETS_SetSockOpt( Socket_t xSocket,
                            int32_t lLevel,
                            int32_t lOptionName,
                            const void * pvOptionValue,
                            size_t xOptionLength )
{
    SSocketContextPtr_t pxSocketContext;
    SlSockNonblocking_t xSockNonblocking;
    struct SlTimeval_t xTimeVal;
    _i16 sTIRetCode;
    int32_t lRetCode = SOCKETS_SOCKET_ERROR;
    uint32_t ulTimeoutInMilliseconds;
    uint32_t ulSocketNumber = ( uint32_t ) xSocket;

    /* Remove compiler warnings about unused parameters. */
    ( void ) lLevel;

    /* Ensure that the socket is valid. */
    if( prvIsValidSocket( ulSocketNumber ) == pdTRUE )
    {
        /* Shortcut for easy access. */
        pxSocketContext = &( xSockets[ ulSocketNumber ] );

        switch( lOptionName )
        {
            case SOCKETS_SO_SERVER_NAME_INDICATION:

                if( ( pxSocketContext->ulFlags & securesocketsSOCKET_IS_CONNECTED ) == 0 )
                {
                    /* Set Server Name Indication (SNI) on the TI socket. */
                    sTIRetCode = sl_SetSockOpt( pxSocketContext->sSocketDescriptor,
                                                SL_SOL_SOCKET,
                                                SL_SO_SECURE_DOMAIN_NAME_VERIFICATION,
                                                pvOptionValue,
                                                ( SlSocklen_t ) xOptionLength );

                    /* If the above sl_SetSockOpt succeeded, return success to the user. */
                    if( sTIRetCode >= 0 )
                    {
                        lRetCode = SOCKETS_ERROR_NONE;
                    }
                    else
                    {
                        /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
                        SOCKETS_PRINT( ( "ERROR: %d SockOpt SNI failed.\r\n", sTIRetCode ) );
                    }
                }
                else
                {
                    lRetCode = SOCKETS_SOCKET_ERROR;
                    SOCKETS_PRINT( ( "ERROR: SNI must be set before connection is established.\r\n" ) );
                }

                break;

            case SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE:

                if( ( pxSocketContext->ulFlags & securesocketsSOCKET_IS_CONNECTED ) == 0 )
                {
                    /* Write the certificate to the file system. */
                    lRetCode = prvWriteCertificate( socketsconfigSECURE_FILE_NAME_CUSTOMROOTCA,
                                                    pvOptionValue,
                                                    xOptionLength - 1U );

                    /* If the certificate was successfully written to the
                     * file system, use it for the socket. */
                    if( lRetCode == SOCKETS_ERROR_NONE )
                    {
                        sTIRetCode = sl_SetSockOpt( pxSocketContext->sSocketDescriptor,
                                                    SL_SOL_SOCKET,
                                                    SL_SO_SECURE_FILES_CA_FILE_NAME,
                                                    socketsconfigSECURE_FILE_NAME_CUSTOMROOTCA,
                                                    ( SlSocklen_t ) strlen( socketsconfigSECURE_FILE_NAME_CUSTOMROOTCA ) );

                        if( sTIRetCode >= 0 )
                        {
                            /* Mark the socket as having a custom trusted root CA. */
                            pxSocketContext->ulFlags |= securesocketsSOCKET_TRUSTED_SERVER_CERT_FLAG;
                        }
                        else
                        {
                            /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
                            SOCKETS_PRINT( ( "ERROR: %d Failed to set the custom root certificate.\r\n", sTIRetCode ) );
                            lRetCode = SOCKETS_SOCKET_ERROR;
                        }
                    }
                }
                else
                {
                    lRetCode = SOCKETS_SOCKET_ERROR;
                    SOCKETS_PRINT( ( "ERROR: Secure trusted cert must be set before connection is established.\r\n" ) );
                }

                break;

            case SOCKETS_SO_REQUIRE_TLS:

                if( ( pxSocketContext->ulFlags & securesocketsSOCKET_IS_CONNECTED ) == 0 )
                {
                    /* Create the semaphore required for synchronizing TLS handshake
                     * results returned in the SimpleLink socket event handler callback. */
                    pxSocketContext->xSSLSyncSemaphore = xSemaphoreCreateBinary();

                    if( pxSocketContext->xSSLSyncSemaphore != NULL )
                    {
                        /* Store that we need to use TLS on this socket. */
                        pxSocketContext->ulFlags |= securesocketsSOCKET_SECURE_FLAG;

                        /* Success. */
                        lRetCode = SOCKETS_ERROR_NONE;
                    }
                    else
                    {
                        /* Since we failed to create the TLS handshake synchronization
                         * semaphore, there is no point of turning the TLS flag on,
                         * as we will not be able to setup TLS connection. Let the user
                         * know that we failed to enable TLS. */
                        lRetCode = SOCKETS_SOCKET_ERROR;
                    }
                }
                else
                {
                    lRetCode = SOCKETS_SOCKET_ERROR;
                    SOCKETS_PRINT( ( "ERROR: Require TLS must be set before connection is established.\r\n" ) );
                }

                break;

            case SOCKETS_SO_RCVTIMEO:

                /* Ensure that uint32_t was passed as value. */
                configASSERT( ( xOptionLength == sizeof( uint32_t ) ) );

                /* Read the passed value. */
                ulTimeoutInMilliseconds = *( ( const uint32_t * ) pvOptionValue ); /*lint !e9079 !e9087 uint32_t type is expected. This function will hard-fault if the wrong type is passed. */

                /* Convert the milliseconds to the struct SlTimeval_t which
                 * contains seconds and microseconds. */
                xTimeVal.tv_sec = ( SlTime_t ) ( ulTimeoutInMilliseconds / 1000 );
                ulTimeoutInMilliseconds = ulTimeoutInMilliseconds - ( uint32_t ) xTimeVal.tv_sec * 1000U;
                xTimeVal.tv_usec = ( SlTime_t ) ( ulTimeoutInMilliseconds * 1000 );

                /* Set receive timeout on the TI socket. */
                sTIRetCode = sl_SetSockOpt( pxSocketContext->sSocketDescriptor,
                                            SL_SOL_SOCKET,
                                            SL_SO_RCVTIMEO,
                                            &( xTimeVal ),
                                            ( SlSocklen_t ) sizeof( xTimeVal ) );

                /* If the above sl_SetSockOpt succeeded, return success to the user. */
                if( sTIRetCode >= 0 )
                {
                    lRetCode = SOCKETS_ERROR_NONE;
                }
                else
                {
                    /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
                    SOCKETS_PRINT( ( "ERROR: %d SockOpt RCVTIMEO failed.\r\n", sTIRetCode ) );
                }

                break;

            case SOCKETS_SO_SNDTIMEO:

                /* Ensure that uint32_t was passed as value. */
                configASSERT( xOptionLength == sizeof( uint32_t ) );

                /* Simple Link socket layer does not provide socket send
                 * timeout. So currently we just store the user supplied
                 * timeout in the socket context and do not use it. In future,
                 * we may use it to simulate send timeout in secure sockets
                 * layer. */
                pxSocketContext->xSendTimeoutTicks = pdMS_TO_TICKS( *( ( const uint32_t * ) pvOptionValue ) ); /*lint !e9079 !e9087 uint32_t type is expected. This function will hard-fault if the wrong type is passed. */

                /* Success. */
                lRetCode = SOCKETS_ERROR_NONE;

                break;

            case SOCKETS_SO_NONBLOCK:

                if( ( pxSocketContext->ulFlags & securesocketsSOCKET_IS_CONNECTED ) != 0 )
                {
                    xSockNonblocking.NonBlockingEnabled = 1;

                    /* Mark the TI socket as non-blocking. */
                    sTIRetCode = sl_SetSockOpt( pxSocketContext->sSocketDescriptor,
                                                SL_SOL_SOCKET,
                                                SL_SO_NONBLOCKING,
                                                &( xSockNonblocking ),
                                                ( SlSocklen_t ) sizeof( xSockNonblocking ) );

                    /* If the above sl_SetSockOpt succeeded, return success to the user. */
                    if( sTIRetCode >= 0 )
                    {
                        lRetCode = SOCKETS_ERROR_NONE;
                    }
                    else
                    {
                        /* See \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h */
                        SOCKETS_PRINT( ( "ERROR: %d SockOpt NONBLOCK failed.\r\n", sTIRetCode ) );
                    }
                }
                else
                {
                    lRetCode = SOCKETS_SOCKET_ERROR;
                    SOCKETS_PRINT( ( "ERROR: NONBLOCKING socket option must be called after connect. Nonblocking connect is not supported.\r\n" ) );
                }

                break;

            default:
                /* Invalid option. */
                lRetCode = SOCKETS_EINVAL;
                break;
        }
    }
    else
    {
        SOCKETS_PRINT( ( "ERROR: SOCKETS_Connect - Invalid Socket number.\r\n" ) );
        lRetCode = SOCKETS_EINVAL;
    }

    return lRetCode;
}
/*-----------------------------------------------------------*/

uint32_t SOCKETS_GetHostByName( const char * pcHostName )
{
    _i16 sTIRetCode = -1;
    _u32 ulResolvedIPAddress;
    uint32_t ulReturnedIPAddress;

    if( xSemaphoreTake( xGetHostByName, xMaxSemaphoreBlockTime ) == pdPASS )
    {
        /* Call the TI GetHostByName. */
        sTIRetCode = sl_NetAppDnsGetHostByName( ( _i8 * ) pcHostName, /*lint !e605 pcHostName will be interpreted as characters. */
                                                ( _u16 ) strlen( pcHostName ),
                                                &( ulResolvedIPAddress ),
                                                ( _u8 ) SL_AF_INET );
        ( void ) xSemaphoreGive( xGetHostByName );
    }

    if( sTIRetCode == 0 )
    {
        /* Convert to network byte order. */
        ulReturnedIPAddress = ( uint32_t ) sl_Htonl( ulResolvedIPAddress );
    }
    else
    {
        /* Return 0 in case of an error. */
        ulReturnedIPAddress = 0;
    }

    return ulReturnedIPAddress;
}
/*-----------------------------------------------------------*/

/**
 * @brief This function handles socket events indication.
 *
 * @note This is executed asynchronously in the context of SimpleLink
 * task.
 *
 * @note SimpleLink error codes can be found in
 * \lib\third_party\mcu_vendor\ti\SimpleLink_CC32xx\v2_10_00_04\source\ti\drivers\net\wifi\errors.h
 *
 * @param[in] pSlSockEvent Pointer to Socket Event Info.
 */
void SimpleLinkSockEventHandler( SlSockEvent_t * pSlSockEvent )
{
    uint32_t ulSocketNumber;
    SSocketContextPtr_t pxSocketContext;

    /* TODO: Finish checking error conditions for this function. */
    switch( pSlSockEvent->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:

            switch( pSlSockEvent->SocketAsyncEvent.SockTxFailData.Status )
            {
                case SL_ERROR_BSD_ECLOSE: /*lint !e9034 switch on a _i16, but SL_ERROR* defines are _i32.  Neither are defined in files controlled by Amazon FreeRTOS. */

                    /* TODO: Should this call sockets close? */
                    SOCKETS_PRINT( ( "[SimpleLinkSockEventHandler ERROR]: Close socket (%d) operation failed to transmit all queued packets.\r\n",
                                    pSlSockEvent->SocketAsyncEvent.SockTxFailData.Sd ) );
                    break;

                default:
                    SOCKETS_PRINT( ( "[SimpleLinkSockEventHandler ERROR]: TX Failed - socket %d, reason (%d).\r\n",
                                    pSlSockEvent->SocketAsyncEvent.SockTxFailData.Sd,
                                    pSlSockEvent->SocketAsyncEvent.SockTxFailData.Status ) );
                    break;
            }

            break;

        case SL_SOCKET_ASYNC_EVENT:

            switch( pSlSockEvent->SocketAsyncEvent.SockAsyncData.Type )
            {
                case SL_SSL_NOTIFICATION_CONNECTED_SECURED:

                    /* Get the socket context this event is for. */
                    ulSocketNumber = prvGetSocketForDescriptor( pSlSockEvent->SocketAsyncEvent.SockAsyncData.Sd );

                    if( ulSocketNumber != ( uint32_t ) SOCKETS_INVALID_SOCKET )
                    {
                        /* Shortcut for easy access. */
                        pxSocketContext = &( xSockets[ ulSocketNumber ] );

                        /* Set the TLS handshake result. */
                        pxSocketContext->xSSLHandshakeResult = securesocketsTLS_HANDSHAKE_SUCCESS;

                        /* Unblock the waiting task. */
                        ( void ) xSemaphoreGive( pxSocketContext->xSSLSyncSemaphore );
                    }
                    else
                    {
                        SOCKETS_PRINT( ( "[SimpleLinkSockEventHandler ERROR]: Unexpected TLS handshake event.\r\n" ) );
                    }

                    break;

                case SL_SSL_NOTIFICATION_HANDSHAKE_FAILED:

                    SOCKETS_PRINT( ( "[SimpleLinkSockEventHandler ERROR]: Handshake Failed - socket %d, reason: %d.\r\n",
                                    pSlSockEvent->SocketAsyncEvent.SockAsyncData.Sd,
                                    pSlSockEvent->SocketAsyncEvent.SockAsyncData.Val ) );

                    /* Get the socket context this event is for. */
                    ulSocketNumber = prvGetSocketForDescriptor( pSlSockEvent->SocketAsyncEvent.SockAsyncData.Sd );

                    if( ulSocketNumber != ( uint32_t ) SOCKETS_INVALID_SOCKET )
                    {
                        /* Shortcut for easy access. */
                        pxSocketContext = &( xSockets[ ulSocketNumber ] );

                        /* Set the TLS handshake result. */
                        pxSocketContext->xSSLHandshakeResult = securesocketsTLS_HANDSHAKE_FAILED;

                        /* Unblock the waiting task. */
                        ( void ) xSemaphoreGive( pxSocketContext->xSSLSyncSemaphore );
                    }
                    else
                    {
                        SOCKETS_PRINT( ( "[SimpleLinkSockEventHandler EVENT]: Unexpected TLS handshake event.\r\n" ) );
                    }

                    break;

                case SL_SSL_NOTIFICATION_WRONG_ROOT_CA:

                    /* We do not unblock the waiting task upon getting this event because
                     * SL_SSL_NOTIFICATION_HANDSHAKE_FAILED is also received whenever
                     * SL_SSL_NOTIFICATION_WRONG_ROOT_CA is received and we unblock the waiting
                     * task upon receiving SL_SSL_NOTIFICATION_WRONG_ROOT_CA. */
                    SOCKETS_PRINT( ( "[SimpleLinkSockEventHandler ERROR]: Root CA in file system did not sign the chain.\r\n" ) );

                    break;

                default:

                    SOCKETS_PRINT( ( "[SimpleLinkSockEventHandler EVENT]: Unexpected Event: %d, Socket: %d Reason: %d.\r\n",
                                    pSlSockEvent->SocketAsyncEvent.SockAsyncData.Type,
                                    pSlSockEvent->SocketAsyncEvent.SockAsyncData.Sd,
                                    pSlSockEvent->SocketAsyncEvent.SockAsyncData.Val ) );

                    break;
            }

            break;

        default:

            SOCKETS_PRINT( ( "[SimpleLinkSockEventHandler EVENT] - Unexpected Event [%x0x].\r\n", pSlSockEvent->Event ) );

            break;
    }
}
/*-----------------------------------------------------------*/
