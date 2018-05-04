/*
 * Amazon FreeRTOS TLS V1.1.1
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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "aws_tls.h"
#include "aws_crypto.h"
#include "aws_pkcs11.h"
#include "task.h"
#include "aws_clientcredential.h"
#include "aws_default_root_certificates.h"

/* mbedTLS includes. */
#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/sha256.h"
#include "mbedtls/pk.h"
#include "mbedtls/debug.h"
#ifdef MBEDTLS_DEBUG_C
    #define tlsDEBUG_VERBOSE    4
#endif

/* C runtime includes. */
#include <string.h>
#include <time.h>
#include <stdio.h>

/**
 * @brief Internal context structure.
 *
 * @param[in] pcDestination Server location, can be a DNS name or IP address.
 * @param[in] pcServerCertificate Server X.509 certificate in PEM format to trust.
 * @param[in] ulServerCertificateLength Length in bytes of the server certificate.
 * @param[in] pxNetworkRecv Callback for receiving data on an open TCP socket.
 * @param[in] pxNetworkSend Callback for sending data on an open TCP socket.
 * @param[in] pvCallerContext Opaque pointer provided by caller for above callbacks.
 * @param[out] mbedSslCtx Connection context for mbedTLS.
 * @param[out] mbedSslConfig Configuration context for mbedTLS.
 * @param[out] mbedX509CA Server certificate context for mbedTLS.
 * @param[out] mbedX509Cli Client certificate context for mbedTLS.
 * @param[out] mbedPkAltCtx RSA crypto implementation context for mbedTLS.
 * @param[out] pxP11FunctionList PKCS#11 function list structure.
 * @param[out] xP11Session PKCS#11 session context.
 * @param[out] xP11PrivateKey PKCS#11 private key context.
 * @param[out] ulP11ModulusBytes Number of bytes in the client private key modulus.
 */
typedef struct TLSContext
{
    const char * pcDestination;
    const char * pcServerCertificate;
    uint32_t ulServerCertificateLength;
    const char ** ppcAlpnProtocols;
    uint32_t ulAlpnProtocolsCount;

    NetworkRecv_t pxNetworkRecv;
    NetworkSend_t pxNetworkSend;
    void * pvCallerContext;

    /* mbedTLS. */
    mbedtls_ssl_context mbedSslCtx;
    mbedtls_ssl_config mbedSslConfig;
    mbedtls_x509_crt mbedX509CA;
    mbedtls_x509_crt mbedX509Cli;
    mbedtls_pk_context mbedPkCtx;

    /* PKCS#11. */
    CK_FUNCTION_LIST_PTR pxP11FunctionList;
    CK_SESSION_HANDLE xP11Session;
    CK_OBJECT_HANDLE xP11PrivateKey;
    CK_ULONG ulP11ModulusBytes;
} TLSContext_t;

/*
 * Helper routines.
 */

/**
 * @brief Network send callback shim.
 *
 * @param[in] pvContext Caller context.
 * @param[in] pucData Byte buffer to send.
 * @param[in] xDataLength Length of byte buffer to send.
 *
 * @return Number of bytes sent, or a negative value on error.
 */
static int prvNetworkSend( void * pvContext,
                           const unsigned char * pucData,
                           size_t xDataLength )
{
    TLSContext_t * pCtx = ( TLSContext_t * ) pvContext; /*lint !e9087 !e9079 Allow casting void* to other types. */

    return ( int ) pCtx->pxNetworkSend( pCtx->pvCallerContext, pucData, xDataLength );
}

/**
 * @brief Network receive callback shim.
 *
 * @param[in] pvContext Caller context.
 * @param[out] pucReceiveBuffer Byte buffer to receive into.
 * @param[in] xReceiveLength Length of byte buffer for receive.
 *
 * @return Number of bytes received, or a negative value on error.
 */
static int prvNetworkRecv( void * pvContext,
                           unsigned char * pucReceiveBuffer,
                           size_t xReceiveLength )
{
    TLSContext_t * pCtx = ( TLSContext_t * ) pvContext; /*lint !e9087 !e9079 Allow casting void* to other types. */

    return ( int ) pCtx->pxNetworkRecv( pCtx->pvCallerContext, pucReceiveBuffer, xReceiveLength );
}

/**
 * @brief Callback that wraps PKCS#11 for pseudo-random number generation.
 *
 * @param[in] pvCtx Caller context.
 * @param[in] pucRandom Byte array to fill with random data.
 * @param[in] xRandomLength Length of byte array.
 *
 * @return Zero on success.
 */
static int prvGenerateRandomBytes( void * pvCtx,
                                   unsigned char * pucRandom,
                                   size_t xRandomLength )
{
    TLSContext_t * pCtx = ( TLSContext_t * ) pvCtx; /*lint !e9087 !e9079 Allow casting void* to other types. */

    return ( int ) pCtx->pxP11FunctionList->C_GenerateRandom( pCtx->xP11Session, pucRandom, xRandomLength );
}

/**
 * @brief Callback that enforces a worst-case expiration check on TLS server
 * certificates.
 *
 * @param[in] pvCtx Caller context.
 * @param[in] pxCertificate Certificate to check.
 * @param[in] lPathCount Location of this certificate in the chain.
 * @param[in] pulFlags Verification status flags.
 *
 * @return Zero on success.
 */
static int prvCheckCertificate( void * pvCtx,
                                mbedtls_x509_crt * pxCertificate,
                                int lPathCount,
                                uint32_t * pulFlags )
{
    int lCompilationYear = 0;

#define tlsCOMPILER_DATE_STRING_MONTH_LENGTH    4
    char pcCompilationMonth[ tlsCOMPILER_DATE_STRING_MONTH_LENGTH ];
    int lCompilationMonth = 0;
    int lCompilationDay = 0;
    const char pcMonths[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    /* Unreferenced parameters. */
    ( void ) ( pvCtx );
    ( void ) ( lPathCount );

    /* Parse the date string fields. */
    sscanf( __DATE__,
            "%3s %d %d",
            pcCompilationMonth,
            &lCompilationDay,
            &lCompilationYear );
    pcCompilationMonth[ tlsCOMPILER_DATE_STRING_MONTH_LENGTH - 1 ] = '\0';

    /* Check for server expiration. First check the year. */
    if( pxCertificate->valid_to.year < lCompilationYear )
    {
        *pulFlags |= MBEDTLS_X509_BADCERT_EXPIRED;
    }
    else if( pxCertificate->valid_to.year == lCompilationYear )
    {
        /* Convert the month. */
        lCompilationMonth =
            ( ( strstr( pcMonths, pcCompilationMonth ) - pcMonths ) /
              ( tlsCOMPILER_DATE_STRING_MONTH_LENGTH - 1 ) ) + 1;

        /* Check the month. */
        if( pxCertificate->valid_to.mon < lCompilationMonth )
        {
            *pulFlags |= MBEDTLS_X509_BADCERT_EXPIRED;
        }
        else if( pxCertificate->valid_to.mon == lCompilationMonth )
        {
            /* Check the day. */
            if( pxCertificate->valid_to.day < lCompilationDay )
            {
                *pulFlags |= MBEDTLS_X509_BADCERT_EXPIRED;
            }
        }
    }

    return 0;
}

/**
 * @brief Helper for setting up potentially hardware-based cryptographic context
 * for the client TLS certificate and private key.
 *
 * @param Caller context.
 *
 * @return Zero on success.
 */
static int prvInitializeClientCredential( TLSContext_t * pCtx )
{
    BaseType_t xResult = 0;
    CK_C_GetFunctionList pxCkGetFunctionList = NULL;
    CK_SLOT_ID xSlotId = 0;
    CK_ULONG ulCount = 1;
    CK_ATTRIBUTE xTemplate = { 0 };
    CK_OBJECT_CLASS xObjClass = 0;
    CK_OBJECT_HANDLE xCertObj = 0;
    CK_BYTE * pucCertificate = NULL;

    /* Initialize the mbed contexts. */
    mbedtls_x509_crt_init( &pCtx->mbedX509Cli );

    /* Ensure that the PKCS#11 module is initialized. */
    if( 0 == xResult )
    {
        pxCkGetFunctionList = C_GetFunctionList;
        xResult = ( BaseType_t ) pxCkGetFunctionList( &pCtx->pxP11FunctionList );
    }

    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) pCtx->pxP11FunctionList->C_Initialize( NULL );
    }

    /* Get the default private key storage ID. */
    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) pCtx->pxP11FunctionList->C_GetSlotList( CK_TRUE, &xSlotId, &ulCount );
    }

    /* Start a private session with the P#11 module. */
    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) pCtx->pxP11FunctionList->C_OpenSession( xSlotId,
                                                                         CKF_SERIAL_SESSION,
                                                                         NULL,
                                                                         NULL,
                                                                         &pCtx->xP11Session );
    }

    /* Enumerate the first private key. */
    if( 0 == xResult )
    {
        xTemplate.type = CKA_CLASS;
        xTemplate.ulValueLen = sizeof( CKA_CLASS );
        xTemplate.pValue = &xObjClass;
        xObjClass = CKO_PRIVATE_KEY;
        xResult = ( BaseType_t ) pCtx->pxP11FunctionList->C_FindObjectsInit( pCtx->xP11Session, &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) pCtx->pxP11FunctionList->C_FindObjects( pCtx->xP11Session, &pCtx->xP11PrivateKey, 1, &ulCount );
    }

    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) pCtx->pxP11FunctionList->C_FindObjectsFinal( pCtx->xP11Session );
    }

    /* Get the internal key context. */
    if( 0 == xResult )
    {
        xTemplate.type = CKA_VENDOR_DEFINED;
        xTemplate.ulValueLen = sizeof( pCtx->mbedPkCtx );
        xTemplate.pValue = &pCtx->mbedPkCtx;
        xResult = ( BaseType_t ) pCtx->pxP11FunctionList->C_GetAttributeValue(
            pCtx->xP11Session, pCtx->xP11PrivateKey, &xTemplate, 1 );
    }

    /* Get the key size. */
    if( 0 == xResult )
    {
        xTemplate.type = CKA_MODULUS_BITS;
        xTemplate.ulValueLen = sizeof( pCtx->ulP11ModulusBytes );
        xTemplate.pValue = &pCtx->ulP11ModulusBytes;
        xResult = ( BaseType_t ) pCtx->pxP11FunctionList->C_GetAttributeValue(
            pCtx->xP11Session, pCtx->xP11PrivateKey, &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        pCtx->ulP11ModulusBytes /= 8;

        /* Enumerate the first client certificate. */
        xTemplate.type = CKA_CLASS;
        xTemplate.ulValueLen = sizeof( CKA_CLASS );
        xTemplate.pValue = &xObjClass;
        xObjClass = CKO_CERTIFICATE;
        xResult = ( BaseType_t ) pCtx->pxP11FunctionList->C_FindObjectsInit( pCtx->xP11Session, &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) pCtx->pxP11FunctionList->C_FindObjects( pCtx->xP11Session, &xCertObj, 1, &ulCount );
    }

    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) pCtx->pxP11FunctionList->C_FindObjectsFinal( pCtx->xP11Session );
    }

    if( 0 == xResult )
    {
        /* Query the certificate size. */
        xTemplate.type = CKA_VALUE;
        xTemplate.ulValueLen = 0;
        xTemplate.pValue = NULL;
        xResult = ( BaseType_t ) pCtx->pxP11FunctionList->C_GetAttributeValue( pCtx->xP11Session, xCertObj, &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        /* Create a buffer for the certificate. */
        pucCertificate = ( CK_BYTE_PTR ) pvPortMalloc( xTemplate.ulValueLen ); /*lint !e9079 Allow casting void* to other types. */

        if( NULL == pucCertificate )
        {
            xResult = ( BaseType_t ) CKR_HOST_MEMORY;
        }
    }

    if( 0 == xResult )
    {
        /* Export the certificate. */
        xTemplate.pValue = pucCertificate;
        xResult = ( BaseType_t ) pCtx->pxP11FunctionList->C_GetAttributeValue(
            pCtx->xP11Session, xCertObj, &xTemplate, 1 );
    }

    /* Decode the client certificate. */
    if( 0 == xResult )
    {
        xResult = mbedtls_x509_crt_parse( &pCtx->mbedX509Cli,
                                          ( const unsigned char * ) pucCertificate,
                                          xTemplate.ulValueLen );
    }

    /*
     * Add a JITR device issuer certificate, if present.
     */
    if( ( 0 == xResult ) &&
        ( NULL != clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM ) )
    {
        /* Decode the JITR issuer. The device client certificate will get
         * inserted as the first certificate in this chain below. */
        xResult = mbedtls_x509_crt_parse(
            &pCtx->mbedX509Cli,
            ( const unsigned char * ) clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM,
            1 + strlen( clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM ) );
    }

    /*
     * Attach the client certificate and private key to the TLS configuration.
     */
    if( 0 == xResult )
    {
        xResult = mbedtls_ssl_conf_own_cert(
            &pCtx->mbedSslConfig, &pCtx->mbedX509Cli, &pCtx->mbedPkCtx );
    }

    if( NULL != pucCertificate )
    {
        vPortFree( pucCertificate );
    }

    return xResult;
}

/*
 * Interface routines.
 */

BaseType_t TLS_Init( void ** ppvContext,
                     TLSParams_t * pxParams )
{
    BaseType_t xResult = 0;
    TLSContext_t * pCtx = NULL;

    /* Allocate an internal context. */
    pCtx = ( TLSContext_t * ) pvPortMalloc( sizeof( TLSContext_t ) ); /*lint !e9087 !e9079 Allow casting void* to other types. */

    if( NULL != pCtx )
    {
        memset( pCtx, 0, sizeof( TLSContext_t ) );
        *ppvContext = pCtx;

        /* Initialize the context. */
        pCtx->pcDestination = pxParams->pcDestination;
        pCtx->pcServerCertificate = pxParams->pcServerCertificate;
        pCtx->ulServerCertificateLength = pxParams->ulServerCertificateLength;
        pCtx->ppcAlpnProtocols = pxParams->ppcAlpnProtocols;
        pCtx->ulAlpnProtocolsCount = pxParams->ulAlpnProtocolsCount;
        pCtx->pxNetworkRecv = pxParams->pxNetworkRecv;
        pCtx->pxNetworkSend = pxParams->pxNetworkSend;
        pCtx->pvCallerContext = pxParams->pvCallerContext;
    }
    else
    {
        xResult = ( BaseType_t ) CKR_HOST_MEMORY;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

#ifdef MBEDTLS_DEBUG_C
    static void prvTlsDebugPrint( void * ctx,
                                  int level,
                                  const char * file,
                                  int line,
                                  const char * str )
    {
        /* Unused parameters. */
        ( void ) ctx;
        ( void ) file;
        ( void ) line;

        /* Send the debug string to the portable logger. */
        vLoggingPrintf( "mbedTLS: |%d| %s", level, str );
    }
#endif /* ifdef MBEDTLS_DEBUG_C */

/*-----------------------------------------------------------*/

BaseType_t TLS_Connect( void * pvContext )
{
    BaseType_t xResult = 0;
    TLSContext_t * pCtx = ( TLSContext_t * ) pvContext; /*lint !e9087 !e9079 Allow casting void* to other types. */

    /* Ensure that the FreeRTOS heap is used. */
    CRYPTO_ConfigureHeap();

    /* Initialize mbedTLS structures. */
    mbedtls_ssl_init( &pCtx->mbedSslCtx );
    mbedtls_ssl_config_init( &pCtx->mbedSslConfig );
    mbedtls_x509_crt_init( &pCtx->mbedX509CA );

    /* Decode the root certificate: either the default or the override. */
    if( NULL != pCtx->pcServerCertificate )
    {
        xResult = mbedtls_x509_crt_parse( &pCtx->mbedX509CA,
                                          ( const unsigned char * ) pCtx->pcServerCertificate,
                                          pCtx->ulServerCertificateLength );
    }
    else
    {
        xResult = mbedtls_x509_crt_parse( &pCtx->mbedX509CA,
                                          ( const unsigned char * ) tlsVERISIGN_ROOT_CERTIFICATE_PEM,
                                          tlsVERISIGN_ROOT_CERTIFICATE_LENGTH );

        if( 0 == xResult )
        {
            xResult = mbedtls_x509_crt_parse( &pCtx->mbedX509CA,
                                              ( const unsigned char * ) tlsATS1_ROOT_CERTIFICATE_PEM,
                                              tlsATS1_ROOT_CERTIFICATE_LENGTH );
        }
    }

    /* Start with protocol defaults. */
    if( 0 == xResult )
    {
        xResult = mbedtls_ssl_config_defaults( &pCtx->mbedSslConfig,
                                               MBEDTLS_SSL_IS_CLIENT,
                                               MBEDTLS_SSL_TRANSPORT_STREAM,
                                               MBEDTLS_SSL_PRESET_DEFAULT );
    }

    if( 0 == xResult )
    {
        /* Use a callback for additional server certificate validation. */
        mbedtls_ssl_conf_verify( &pCtx->mbedSslConfig,
                                 &prvCheckCertificate,
                                 pCtx );

        /* Server certificate validation is mandatory. */
        mbedtls_ssl_conf_authmode( &pCtx->mbedSslConfig, MBEDTLS_SSL_VERIFY_REQUIRED );

        /* Set the RNG callback. */
        mbedtls_ssl_conf_rng( &pCtx->mbedSslConfig, &prvGenerateRandomBytes, pCtx ); /*lint !e546 Nothing wrong here. */

        /* Set issuer certificate. */
        mbedtls_ssl_conf_ca_chain( &pCtx->mbedSslConfig, &pCtx->mbedX509CA, NULL );

        /* Setup the client credential. */
        xResult = prvInitializeClientCredential( pCtx );
    }

    if( ( 0 == xResult ) && ( NULL != pCtx->ppcAlpnProtocols ) )
    {
        /* Include an application protocol list in the TLS ClientHello
         * message. */
        xResult = mbedtls_ssl_conf_alpn_protocols(
            &pCtx->mbedSslConfig,
            pCtx->ppcAlpnProtocols );
    }

    #ifdef MBEDTLS_DEBUG_C

        /* If mbedTLS is being compiled with debug support, assume that the
         * runtime configuration should use verbose output. */
        mbedtls_ssl_conf_dbg( &pCtx->mbedSslConfig, prvTlsDebugPrint, NULL );
        mbedtls_debug_set_threshold( tlsDEBUG_VERBOSE );
    #endif

    if( 0 == xResult )
    {
        /* Set the resulting protocol configuration. */
        xResult = mbedtls_ssl_setup( &pCtx->mbedSslCtx, &pCtx->mbedSslConfig );
    }

    /* Set the hostname, if requested. */
    if( ( 0 == xResult ) && ( NULL != pCtx->pcDestination ) )
    {
        xResult = mbedtls_ssl_set_hostname( &pCtx->mbedSslCtx, pCtx->pcDestination );
    }

    /* Set the socket callbacks. */
    if( 0 == xResult )
    {
        mbedtls_ssl_set_bio( &pCtx->mbedSslCtx,
                             pCtx,
                             prvNetworkSend,
                             prvNetworkRecv,
                             NULL );

        /* Negotiate. */
        while( 0 != ( xResult = mbedtls_ssl_handshake( &pCtx->mbedSslCtx ) ) )
        {
            if( ( MBEDTLS_ERR_SSL_WANT_READ != xResult ) &&
                ( MBEDTLS_ERR_SSL_WANT_WRITE != xResult ) )
            {
                break;
            }
        }
    }

    /* Free up allocated memory. */
    mbedtls_x509_crt_free( &pCtx->mbedX509CA );
    mbedtls_x509_crt_free( &pCtx->mbedX509Cli );

    return xResult;
}

/*-----------------------------------------------------------*/

BaseType_t TLS_Recv( void * pvContext,
                     unsigned char * pucReadBuffer,
                     size_t xReadLength )
{
    BaseType_t xResult = 0;
    TLSContext_t * pCtx = ( TLSContext_t * ) pvContext; /*lint !e9087 !e9079 Allow casting void* to other types. */
    size_t xRead = 0;

    if( NULL != pCtx )
    {
        while( xRead < xReadLength )
        {
            xResult = mbedtls_ssl_read( &pCtx->mbedSslCtx,
                                        pucReadBuffer + xRead,
                                        xReadLength - xRead );

            if( 0 < xResult )
            {
                /* Got data, so update the tally and keep looping. */
                xRead += ( size_t ) xResult;
            }
            else
            {
                if( ( 0 == xResult ) || ( MBEDTLS_ERR_SSL_WANT_READ != xResult ) )
                {
                    /* No data and no error or call read again, if indicated, otherwise return error. */
                    break;
                }
            }
        }
    }

    if( 0 <= xResult )
    {
        xResult = ( BaseType_t ) xRead;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

BaseType_t TLS_Send( void * pvContext,
                     const unsigned char * pucMsg,
                     size_t xMsgLength )
{
    BaseType_t xResult = 0;
    TLSContext_t * pCtx = ( TLSContext_t * ) pvContext; /*lint !e9087 !e9079 Allow casting void* to other types. */
    size_t xWritten = 0;

    if( NULL != pCtx )
    {
        while( xWritten < xMsgLength )
        {
            xResult = mbedtls_ssl_write( &pCtx->mbedSslCtx,
                                         pucMsg + xWritten,
                                         xMsgLength - xWritten );

            if( 0 < xResult )
            {
                /* Sent data, so update the tally and keep looping. */
                xWritten += ( size_t ) xResult;
            }
            else
            {
                if( ( 0 == xResult ) || ( MBEDTLS_ERR_SSL_WANT_WRITE != xResult ) )
                {
                    /* No data and no error or call read again, if indicated, otherwise return error. */
                    break;
                }
            }
        }
    }

    if( 0 <= xResult )
    {
        xResult = ( BaseType_t ) xWritten;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

void TLS_Cleanup( void * pvContext )
{
    TLSContext_t * pCtx = ( TLSContext_t * ) pvContext; /*lint !e9087 !e9079 Allow casting void* to other types. */

    if( NULL != pCtx )
    {
        /* Cleanup mbedTLS. */
        mbedtls_ssl_close_notify( &pCtx->mbedSslCtx ); /*lint !e534 The error is already taken care of inside mbedtls_ssl_close_notify*/
        mbedtls_ssl_free( &pCtx->mbedSslCtx );
        mbedtls_ssl_config_free( &pCtx->mbedSslConfig );

        /* Cleanup PKCS#11. */
        if( ( NULL != pCtx->pxP11FunctionList ) &&
            ( NULL != pCtx->pxP11FunctionList->C_CloseSession ) )
        {
            pCtx->pxP11FunctionList->C_CloseSession( pCtx->xP11Session ); /*lint !e534 This function always return CKR_OK. */
            pCtx->pxP11FunctionList->C_Finalize( NULL );                  /*lint !e534 This function always return CKR_OK. */
        }

        /* Free memory. */
        vPortFree( pCtx );
    }
}
