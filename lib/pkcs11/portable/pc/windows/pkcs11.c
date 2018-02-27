/*
 * Amazon FreeRTOS PKCS#11 for Windows Simulator V1.0.1
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


/**
 * @file pkcs11.c
 * @brief Windows simulator PKCS#11 implementation for software keys. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "aws_crypto.h"
#include "aws_pkcs11.h"

/* mbedTLS includes. */
#include "mbedtls/pk.h"
#include "mbedtls/pk_internal.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/sha256.h"
#include "mbedtls/base64.h"
#include "aws_clientcredential.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/**
 * @brief File storage location definitions.
 */
#define pkcs11FILE_NAME_CLIENT_CERTIFICATE    "FreeRTOS_P11_Certificate.dat"
#define pkcs11FILE_NAME_KEY                   "FreeRTOS_P11_Key.dat"

/**
 * @brief Cryptoki module attribute definitions.
 */
#define pkcs11SLOT_ID                         1
#define pkcs11OBJECT_HANDLE_PUBLIC_KEY        1
#define pkcs11OBJECT_HANDLE_PRIVATE_KEY       2
#define pkcs11OBJECT_HANDLE_CERTIFICATE       3

#define pkcs11SUPPORTED_KEY_BITS              2048

typedef int ( * pfnMbedTlsSign )( void * ctx,
                                  mbedtls_md_type_t md_alg,
                                  const unsigned char * hash,
                                  size_t hash_len,
                                  unsigned char * sig,
                                  size_t * sig_len,
                                  int ( *f_rng )( void *, unsigned char *, size_t ),
                                  void * p_rng );

/**
 * @brief Key structure.
 */
typedef struct P11Key
{
    mbedtls_pk_context xMbedPkCtx;
    mbedtls_x509_crt xMbedX509Cli;
    mbedtls_pk_info_t xMbedPkInfo;
    pfnMbedTlsSign pfnSavedMbedSign;
    void * pvSavedMbedPkCtx;
} P11Key_t, * P11KeyPtr_t;

/**
 * @brief Session structure.
 */
typedef struct P11Session
{
    P11KeyPtr_t pxCurrentKey;
    CK_ULONG ulState;
    CK_BBOOL xOpened;
    CK_BBOOL xFindObjectInit;
    CK_BBOOL xFindObjectComplete;
    CK_OBJECT_CLASS xFindObjectClass;
    mbedtls_ctr_drbg_context xMbedDrbgCtx;
    mbedtls_entropy_context xMbedEntropyContext;
} P11Session_t, * P11SessionPtr_t;

/**
 * @brief Helper definitions.
 */
#define pkcs11CREATE_OBJECT_MIN_ATTRIBUTE_COUNT 3
#define pkcs11CERTIFICATE_ATTRIBUTE_COUNT 3
#define pkcs11PRIVATE_KEY_ATTRIBUTE_COUNT 4

/*-----------------------------------------------------------*/

/**
 * @brief Maps an opaque caller session handle into its internal state structure.
 */
static P11SessionPtr_t prvSessionPointerFromHandle( CK_SESSION_HANDLE xSession )
{
    return ( P11SessionPtr_t ) xSession; /*lint !e923 Allow casting integer type to pointer for handle. */
}

/*-----------------------------------------------------------*/

/**
 * @brief Writes a file to local storage.
 */
static BaseType_t prvSaveFile( char * pcFileName,
                               uint8_t * pucData,
                               uint32_t ulDataSize )
{
    uint32_t ulStatus = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD lpNumberOfBytesWritten;

    /* Open the file. */
    hFile = CreateFileA( pcFileName,
                         GENERIC_WRITE,
                         0,
                         NULL,
                         CREATE_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL );

    if( INVALID_HANDLE_VALUE == hFile )
    {
        ulStatus = GetLastError();
    }

    /* Write the data. */
    if( ERROR_SUCCESS == ulStatus )
    {
        if( FALSE == WriteFile( hFile, pucData, ulDataSize, &lpNumberOfBytesWritten, NULL ) )
        {
            ulStatus = GetLastError();
        }
    }

    /* Clean up. */
    if( INVALID_HANDLE_VALUE != hFile )
    {
        CloseHandle( hFile );
    }

    return 0 == ulStatus;
}

/*-----------------------------------------------------------*/

/**
 * @brief Reads a file from local storage.
 */
static BaseType_t prvReadFile( char * pcFileName,
                               uint8_t ** ppucData,
                               uint32_t * pulDataSize )
{
    uint32_t ulStatus = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    uint32_t ulSize = 0;

    /* Open the file. */
    hFile = CreateFileA( pcFileName,
                         GENERIC_READ,
                         0,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL );

    if( INVALID_HANDLE_VALUE == hFile )
    {
        ulStatus = GetLastError();
    }

    if( 0 == ulStatus )
    {
        /* Get the file size. */
        *pulDataSize = GetFileSize( hFile, ( LPDWORD ) ( &ulSize ) );

        /* Create a buffer. */
        *ppucData = pvPortMalloc( *pulDataSize );

        if( NULL == *ppucData )
        {
            ulStatus = ERROR_NOT_ENOUGH_MEMORY;
        }
    }

    /* Read the file. */
    if( 0 == ulStatus )
    {
        if( FALSE == ReadFile( hFile,
                               *ppucData,
                               *pulDataSize,
                               ( LPDWORD ) ( &ulSize ),
                               NULL ) )
        {
            ulStatus = GetLastError();
        }
    }

    /* Confirm the amount of data read. */
    if( 0 == ulStatus )
    {
        if( ulSize != *pulDataSize )
        {
            ulStatus = ERROR_INVALID_DATA;
        }
    }

    /* Clean up. */
    if( INVALID_HANDLE_VALUE != hFile )
    {
        CloseHandle( hFile );
    }

    return 0 == ulStatus;
}

/*-----------------------------------------------------------*/

/**
 * @brief Sign a cryptographic hash with the private key.
 *
 * @param[in] pvContext Crypto context.
 * @param[in] xMdAlg Unused.
 * @param[in] pucHash Length in bytes of hash to be signed.
 * @param[in] uiHashLen Byte array of hash to be signed.
 * @param[out] pucSig RSA signature bytes.
 * @param[in] pxSigLen Length in bytes of signature buffer.
 * @param[in] piRng Unused.
 * @param[in] pvRng Unused.
 *
 * @return Zero on success.
 */
static int prvPrivateKeySigningCallback( void * pvContext,
                                         mbedtls_md_type_t xMdAlg,
                                         const unsigned char * pucHash,
                                         unsigned int uiHashLen,
                                         unsigned char * pucSig,
                                         size_t * pxSigLen,
                                         int ( *piRng )( void *, unsigned char *, size_t ), /*lint !e955 This parameter is unused. */
                                         void * pvRng )
{
    BaseType_t xResult = 0;
    P11SessionPtr_t pxSession = ( P11SessionPtr_t ) pvContext;
    CK_MECHANISM xMech = { 0 };

    /* Unreferenced parameters. */
    ( void ) ( piRng );
    ( void ) ( pvRng );
    ( void ) ( xMdAlg );

    /* Use the PKCS#11 module to sign. */
    xMech.mechanism = CKM_SHA256;

    xResult = ( BaseType_t ) C_SignInit(
        ( CK_SESSION_HANDLE ) pxSession,
        &xMech,
        ( CK_OBJECT_HANDLE ) pxSession->pxCurrentKey );

    if( 0 == xResult )
    {
        xResult = ( BaseType_t ) C_Sign(
            ( CK_SESSION_HANDLE ) pxSession,
            ( CK_BYTE_PTR ) pucHash, /*lint !e9005 The interfaces are from 3rdparty libraries, we are not suppose to change them. */
            uiHashLen,
            pucSig,
            ( CK_ULONG_PTR ) pxSigLen );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

/**
 * @brief Initializes a key structure.
 */
static CK_RV prvInitializeKey( P11SessionPtr_t pxSessionObj,
                               const char * pcEncodedKey,
                               const uint32_t ulEncodedKeyLength,
                               const char * pcEncodedCertificate,
                               const uint32_t ulEncodedCertificateLength )
{
    CK_RV xResult = 0;

    /*
     * Create the key structure, but allow an existing one to be used.
     */

    if( NULL == pxSessionObj->pxCurrentKey )
    {
        if( NULL == ( pxSessionObj->pxCurrentKey = ( P11KeyPtr_t ) pvPortMalloc(
                          sizeof( P11Key_t ) ) ) ) /*lint !e9087 Allow casting void* to other types. */
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    /*
     * Initialize the key field, if requested.
     */

    if( ( CKR_OK == xResult ) && ( NULL != pcEncodedKey ) )
    {
        memset( pxSessionObj->pxCurrentKey, 0, sizeof( P11Key_t ) );
        mbedtls_pk_init( &pxSessionObj->pxCurrentKey->xMbedPkCtx );

        if( 0 != mbedtls_pk_parse_key(
                &pxSessionObj->pxCurrentKey->xMbedPkCtx,
                ( const unsigned char * ) pcEncodedKey,
                ulEncodedKeyLength,
                NULL,
                0 ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }

        if( CKR_OK == xResult )
        {
            /* Swap out the signing function pointer. */
            memcpy(
                &pxSessionObj->pxCurrentKey->xMbedPkInfo,
                pxSessionObj->pxCurrentKey->xMbedPkCtx.pk_info,
                sizeof( pxSessionObj->pxCurrentKey->xMbedPkInfo ) );
            pxSessionObj->pxCurrentKey->pfnSavedMbedSign = pxSessionObj->pxCurrentKey->xMbedPkInfo.sign_func;
            pxSessionObj->pxCurrentKey->xMbedPkInfo.sign_func = prvPrivateKeySigningCallback;
            pxSessionObj->pxCurrentKey->xMbedPkCtx.pk_info = &pxSessionObj->pxCurrentKey->xMbedPkInfo;

            /* Swap out the underlying internal key context. */
            pxSessionObj->pxCurrentKey->pvSavedMbedPkCtx = pxSessionObj->pxCurrentKey->xMbedPkCtx.pk_ctx;
            pxSessionObj->pxCurrentKey->xMbedPkCtx.pk_ctx = pxSessionObj;
        }
    }

    /*
     * Initialize the certificate field, if requested.
     */

    if( ( CKR_OK == xResult ) && ( NULL != pcEncodedCertificate ) )
    {
        mbedtls_x509_crt_init( &pxSessionObj->pxCurrentKey->xMbedX509Cli );

        if( 0 != mbedtls_x509_crt_parse(
                &pxSessionObj->pxCurrentKey->xMbedX509Cli,
                ( const unsigned char * ) pcEncodedCertificate,
                ulEncodedCertificateLength ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    return xResult;
}

/*-----------------------------------------------------------*/

/**
 * @brief Load the default key and certificate from storage.
 */
static CK_RV prvLoadAndInitializeDefaultCertificateAndKey( P11SessionPtr_t pxSession )
{
    CK_RV xResult = 0;
    uint8_t * pucCertificateData = NULL;
    uint32_t ulCertificateDataLength = 0;
    BaseType_t xFreeCertificate = pdFALSE;
    uint8_t * pucKeyData = NULL;
    uint32_t ulKeyDataLength = 0;
    BaseType_t xFreeKey = pdFALSE;

    /* Read the certificate from storage. */
    if( pdFALSE == prvReadFile( pkcs11FILE_NAME_CLIENT_CERTIFICATE,
                                &pucCertificateData,
                                &ulCertificateDataLength ) )
    {
        pucCertificateData = ( uint8_t * ) clientcredentialCLIENT_CERTIFICATE_PEM;
        ulCertificateDataLength = clientcredentialCLIENT_CERTIFICATE_LENGTH;
    }
    else
    {
        xFreeCertificate = pdTRUE;
    }

    /* Read the private key from storage. */
    if( pdFALSE == prvReadFile( pkcs11FILE_NAME_KEY,
                                &pucKeyData,
                                &ulKeyDataLength ) )
    {
        pucKeyData = ( uint8_t * ) clientcredentialCLIENT_PRIVATE_KEY_PEM;
        ulKeyDataLength = clientcredentialCLIENT_PRIVATE_KEY_LENGTH;
    }
    else
    {
        xFreeKey = pdTRUE;
    }

    /* Attach the certificate and key to the session. */
    xResult = prvInitializeKey( pxSession,
                                ( const char * ) pucKeyData,
                                ulKeyDataLength,
                                ( const char * ) pucCertificateData,
                                ulCertificateDataLength );

    /* Stir the random pot. */
    mbedtls_entropy_update_manual( &pxSession->xMbedEntropyContext,
                                   pucKeyData,
                                   ulKeyDataLength );
    mbedtls_entropy_update_manual( &pxSession->xMbedEntropyContext,
                                   pucCertificateData,
                                   ulCertificateDataLength );

    /* Clean-up. */
    if( ( NULL != pucCertificateData ) && ( pdTRUE == xFreeCertificate ) )
    {
        vPortFree( pucCertificateData );
    }

    if( ( NULL != pucKeyData ) && ( pdTRUE == xFreeKey ) )
    {
        vPortFree( pucKeyData );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

/**
 * @brief Cleans up a key structure.
 */
static void prvFreeKey( P11KeyPtr_t pxKey )
{
    if( NULL != pxKey )
    {
        /* Restore the internal key context. */
        pxKey->xMbedPkCtx.pk_ctx = pxKey->pvSavedMbedPkCtx;

        /* Clean-up. */
        mbedtls_pk_free( &pxKey->xMbedPkCtx );
        mbedtls_x509_crt_free( &pxKey->xMbedX509Cli );
        vPortFree( pxKey );
    }
}

/*-----------------------------------------------------------*/

/*
 * PKCS#11 module implementation.
 */

/**
 * @brief PKCS#11 interface functions implemented by this Cryptoki module.
 */
static CK_FUNCTION_LIST prvP11FunctionList =
{
    { CRYPTOKI_VERSION_MAJOR, CRYPTOKI_VERSION_MINOR },
    C_Initialize,
    C_Finalize,
    NULL,
    C_GetFunctionList,
    C_GetSlotList,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_OpenSession,
    C_CloseSession,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_CreateObject,
    NULL,
    C_DestroyObject,
    NULL,
    C_GetAttributeValue,
    NULL,
    C_FindObjectsInit,
    C_FindObjects,
    C_FindObjectsFinal,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_SignInit,
    C_Sign,
    NULL,
    NULL,
    NULL,
    NULL,
    C_VerifyInit,
    C_Verify,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_GenerateRandom,
    NULL,
    NULL,
    NULL
};

/**
 * @brief Initialize the Cryptoki module for use.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Initialize )( CK_VOID_PTR pvInitArgs )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( pvInitArgs );

    return CKR_OK;
}

/**
 * @brief Un-initialize the Cryptoki module.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Finalize )( CK_VOID_PTR pvReserved )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( pvReserved );

    return CKR_OK;
}

/**
 * @brief Query the list of interface function pointers.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetFunctionList )( CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{   /*lint !e9072 It's OK to have different parameter name. */
    *ppxFunctionList = &prvP11FunctionList;

    return CKR_OK;
}

/**
 * @brief Query the list of slots. A single default slot is implemented.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetSlotList )( CK_BBOOL xTokenPresent,
                                            CK_SLOT_ID_PTR pxSlotList,
                                            CK_ULONG_PTR pulCount )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( xTokenPresent );

    if( NULL == pxSlotList )
    {
        *pulCount = 1;
    }
    else
    {
        if( 0u == *pulCount )
        {
            return CKR_BUFFER_TOO_SMALL;
        }

        pxSlotList[ 0 ] = pkcs11SLOT_ID;
        *pulCount = 1;
    }

    return CKR_OK;
}

/**
 * @brief Start a session for a cryptographic command sequence.
 */
CK_DEFINE_FUNCTION( CK_RV, C_OpenSession )( CK_SLOT_ID xSlotID,
                                            CK_FLAGS xFlags,
                                            CK_VOID_PTR pvApplication,
                                            CK_NOTIFY xNotify,
                                            CK_SESSION_HANDLE_PTR pxSession )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSessionObj = NULL;

    ( void ) ( xSlotID );
    ( void ) ( pvApplication );
    ( void ) ( xNotify );

    /*
     * Make space for the context.
     */
    if( NULL == ( pxSessionObj = ( P11SessionPtr_t ) pvPortMalloc( sizeof( P11Session_t ) ) ) ) /*lint !e9087 Allow casting void* to other types. */
    {
        xResult = CKR_HOST_MEMORY;
    }

    /*
     * Assume that there's no performance tradeoff in loading the default key
     * now, since that's the principal use case for opening a session in this
     * provider anyway. This way, the private key can be used for seeding the RNG,
     * especially if there's no hardware-based alternative.
     */

    if( CKR_OK == xResult )
    {
        memset( pxSessionObj, 0, sizeof( P11Session_t ) );
        xResult = prvLoadAndInitializeDefaultCertificateAndKey( pxSessionObj );
    }

    /*
     * Initialize RNG.
     */

    if( CKR_OK == xResult )
    {
        memset( &pxSessionObj->xMbedEntropyContext,
                0,
                sizeof( pxSessionObj->xMbedEntropyContext ) );

        mbedtls_entropy_init( &pxSessionObj->xMbedEntropyContext );
    }

    if( CKR_OK == xResult )
    {
        mbedtls_ctr_drbg_init( &pxSessionObj->xMbedDrbgCtx );

        if( 0 != mbedtls_ctr_drbg_seed( &pxSessionObj->xMbedDrbgCtx,
                                        mbedtls_entropy_func,
                                        &pxSessionObj->xMbedEntropyContext,
                                        NULL,
                                        0 ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    if( CKR_OK == xResult )
    {
        /*
         * Assign the session.
         */

        pxSessionObj->ulState =
            0u != ( xFlags & CKF_RW_SESSION ) ? CKS_RW_PUBLIC_SESSION : CKS_RO_PUBLIC_SESSION;
        pxSessionObj->xOpened = CK_TRUE;

        /*
         * Return the session.
         */

        *pxSession = ( CK_SESSION_HANDLE ) pxSessionObj; /*lint !e923 Allow casting pointer to integer type for handle. */
    }

    return xResult;
}

/**
 * @brief Terminate a session and release resources.
 */
CK_DEFINE_FUNCTION( CK_RV, C_CloseSession )( CK_SESSION_HANDLE xSession )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( NULL != pxSession )
    {
        /*
         * Tear down the session.
         */

        if( NULL != pxSession->pxCurrentKey )
        {
            prvFreeKey( pxSession->pxCurrentKey );
        }

        mbedtls_ctr_drbg_free( &pxSession->xMbedDrbgCtx );
        vPortFree( pxSession );
    }

    return xResult;
}

/**
 * @brief Provides import and storage of a single client certificate and
 * associated private key.
 */
CK_DEFINE_FUNCTION( CK_RV, C_CreateObject )( CK_SESSION_HANDLE xSession,
                                             CK_ATTRIBUTE_PTR pxTemplate,
                                             CK_ULONG ulCount,
                                             CK_OBJECT_HANDLE_PTR pxObject )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    ( void )( xSession );

    /*
     * Check parameters.
     */
    if( ( pkcs11CREATE_OBJECT_MIN_ATTRIBUTE_COUNT > ulCount ) ||
        ( NULL == pxTemplate ) ||
        ( NULL == pxObject ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( CKR_OK == xResult )
    {
        if( ( CKA_CLASS != pxTemplate[ 0 ].type ) ||
            ( sizeof( CK_OBJECT_CLASS ) != pxTemplate[ 0 ].ulValueLen ) )
        {
            xResult = CKR_ARGUMENTS_BAD;
        }
    }

    /*
     * Handle the object by class.
     */

    if( CKR_OK == xResult )
    {
        switch( *( ( uint32_t * ) pxTemplate[ 0 ].pValue ) )
        {
            case CKO_CERTIFICATE:

                /* Validate the attribute count for this object class. */
                if( pkcs11CERTIFICATE_ATTRIBUTE_COUNT != ulCount )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Validate the next attribute type. */
                if( CKA_VALUE )
                {
                    if( CKA_VALUE != pxTemplate[ 1 ].type )
                    {
                        xResult = CKR_ARGUMENTS_BAD;
                        break;
                    }
                }

                if( *( ( uint32_t * )pxTemplate[ 2 ].pValue ) == pkcs11CERTIFICATE_TYPE_USER )
                {
                    /* Write out the client certificate. */
                    if( pdFALSE == prvSaveFile( pkcs11FILE_NAME_CLIENT_CERTIFICATE,
                        pxTemplate[ 1 ].pValue,
                        pxTemplate[ 1 ].ulValueLen ) )
                    {
                        xResult = CKR_DEVICE_ERROR;
                        break;
                    }
                }
                else if( *( ( uint32_t * )pxTemplate[ 2 ].pValue ) == pkcs11CERTIFICATE_TYPE_ROOT )
                {
                    /* Ignore writing the default root certificate. */
                }
                break;

            case CKO_PRIVATE_KEY:

                /* Validate the attribute count for this object class. */
                if( pkcs11PRIVATE_KEY_ATTRIBUTE_COUNT != ulCount )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Find the key bytes. */
                if( CKA_VALUE )
                {
                    if( CKA_VALUE != pxTemplate[ 3 ].type )
                    {
                        xResult = CKR_ARGUMENTS_BAD;
                        break;
                    }
                }

                /* Write out the key. */
                if( pdFALSE == prvSaveFile( pkcs11FILE_NAME_KEY,
                                            pxTemplate[ 3 ].pValue,
                                            pxTemplate[ 3 ].ulValueLen ) )
                {
                    xResult = CKR_DEVICE_ERROR;
                    break;
                }
                break;

            default:
                xResult = CKR_ARGUMENTS_BAD;
        }
    }

    return xResult;
}

/**
 * @brief Free resources attached to an object handle.
 */
CK_DEFINE_FUNCTION( CK_RV, C_DestroyObject )( CK_SESSION_HANDLE xSession,
                                              CK_OBJECT_HANDLE xObject )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( xSession );
    ( void ) ( xObject );

    /*
     * This implementation uses virtual handles, and the certificate and
     * private key data are attached to the session, so nothing to do here.
     */
    return CKR_OK;
}

/**
 * @brief Query the value of the specified cryptographic object attribute.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetAttributeValue )( CK_SESSION_HANDLE xSession,
                                                  CK_OBJECT_HANDLE xObject,
                                                  CK_ATTRIBUTE_PTR pxTemplate,
                                                  CK_ULONG ulCount )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );
    CK_VOID_PTR pvAttr = NULL;
    CK_ULONG ulAttrLength = 0;
    mbedtls_pk_type_t xMbedPkType;
    CK_ULONG xP11KeyType, iAttrib, xKeyBitLen;

    ( void ) ( xObject );

    /*
     * Enumerate the requested attributes.
     */

    for( iAttrib = 0; iAttrib < ulCount && CKR_OK == xResult; iAttrib++ )
    {
        /*
         * Get the attribute data and size.
         */

        switch( pxTemplate[ iAttrib ].type )
        {
            case CKA_KEY_TYPE:

                /*
                 * Map the private key type between APIs.
                 */
                xMbedPkType = mbedtls_pk_get_type( &pxSession->pxCurrentKey->xMbedPkCtx );

                switch( xMbedPkType )
                {
                    case MBEDTLS_PK_RSA:
                    case MBEDTLS_PK_RSA_ALT:
                    case MBEDTLS_PK_RSASSA_PSS:
                        xP11KeyType = CKK_RSA;
                        break;

                    case MBEDTLS_PK_ECKEY:
                    case MBEDTLS_PK_ECKEY_DH:
                        xP11KeyType = CKK_EC;
                        break;

                    case MBEDTLS_PK_ECDSA:
                        xP11KeyType = CKK_ECDSA;
                        break;

                    default:
                        xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                        break;
                }

                ulAttrLength = sizeof( xP11KeyType );
                pvAttr = &xP11KeyType;
                break;

            case CKA_VALUE:

                /*
                 * Assume that the query is for the encoded client certificate.
                 */
                pvAttr = ( CK_VOID_PTR ) pxSession->pxCurrentKey->xMbedX509Cli.raw.p; /*lint !e9005 !e9087 Allow casting other types to void*. */
                ulAttrLength = pxSession->pxCurrentKey->xMbedX509Cli.raw.len;
                break;

            case CKA_MODULUS_BITS:
            case CKA_PRIME_BITS:

                /*
                 * Key strength size query, handled the same for RSA or ECDSA
                 * in this port.
                 */
                xKeyBitLen = mbedtls_pk_get_bitlen(
                    &pxSession->pxCurrentKey->xMbedPkCtx );
                ulAttrLength = sizeof( xKeyBitLen );
                pvAttr = &xKeyBitLen;
                break;

            case CKA_VENDOR_DEFINED:

                /*
                 * Return the key context for application-layer use.
                 */
                ulAttrLength = sizeof( pxSession->pxCurrentKey->xMbedPkCtx );
                pvAttr = &pxSession->pxCurrentKey->xMbedPkCtx;
                break;

            default:
                xResult = CKR_ATTRIBUTE_TYPE_INVALID;
                break;
        }

        if( CKR_OK == xResult )
        {
            /*
             * Copy out the data and size.
             */

            if( NULL != pxTemplate[ iAttrib ].pValue )
            {
                if( pxTemplate[ iAttrib ].ulValueLen < ulAttrLength )
                {
                    xResult = CKR_BUFFER_TOO_SMALL;
                }
                else
                {
                    memcpy( pxTemplate[ iAttrib ].pValue, pvAttr, ulAttrLength );
                }
            }

            pxTemplate[ iAttrib ].ulValueLen = ulAttrLength;
        }
    }

    return xResult;
}

/**
 * @brief Begin an enumeration sequence for the objects of the specified type.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsInit )( CK_SESSION_HANDLE xSession,
                                                CK_ATTRIBUTE_PTR pxTemplate,
                                                CK_ULONG ulCount )
{   /*lint !e9072 It's OK to have different parameter name. */
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    ( void ) ( ulCount );

    /*
     * Allow filtering on a single object class attribute.
     */

    pxSession->xFindObjectInit = CK_TRUE;
    pxSession->xFindObjectComplete = CK_FALSE;
    memcpy( &pxSession->xFindObjectClass,
            pxTemplate[ 0 ].pValue,
            sizeof( CK_OBJECT_CLASS ) );

    return CKR_OK;
}

/**
 * @brief Query the objects of the requested type.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjects )( CK_SESSION_HANDLE xSession,
                                            CK_OBJECT_HANDLE_PTR pxObject,
                                            CK_ULONG ulMaxObjectCount,
                                            CK_ULONG_PTR pulObjectCount )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    BaseType_t xDone = pdFALSE;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    /*
     * Check parameters.
     */

    if( ( CK_BBOOL ) CK_FALSE == pxSession->xFindObjectInit )
    {
        xResult = CKR_OPERATION_NOT_INITIALIZED;
        xDone = pdTRUE;
    }

    if( ( pdFALSE == xDone ) && ( 0u == ulMaxObjectCount ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
        xDone = pdTRUE;
    }

    if( ( pdFALSE == xDone ) && ( ( CK_BBOOL ) CK_TRUE == pxSession->xFindObjectComplete ) )
    {
        *pulObjectCount = 0;
        xResult = CKR_OK;
        xDone = pdTRUE;
    }

    /*
     * Load the default private key and certificate.
     */

    if( ( pdFALSE == xDone ) && ( NULL == pxSession->pxCurrentKey ) )
    {
        if( CKR_OK != ( xResult = prvLoadAndInitializeDefaultCertificateAndKey( pxSession ) ) )
        {
            xDone = pdTRUE;
        }
    }

    if( pdFALSE == xDone )
    {
        /*
         * Return object handles based on find type.
         */

        switch( pxSession->xFindObjectClass )
        {
            case CKO_PRIVATE_KEY:
                *pxObject = pkcs11OBJECT_HANDLE_PRIVATE_KEY;
                *pulObjectCount = 1;
                break;

            case CKO_PUBLIC_KEY:
                *pxObject = pkcs11OBJECT_HANDLE_PUBLIC_KEY;
                *pulObjectCount = 1;
                break;

            case CKO_CERTIFICATE:
                *pxObject = pkcs11OBJECT_HANDLE_CERTIFICATE;
                *pulObjectCount = 1;
                break;

            default:
                *pxObject = 0;
                *pulObjectCount = 0;
                break;
        }

        pxSession->xFindObjectComplete = CK_TRUE;
    }

    return xResult;
}

/**
 * @brief Terminate object enumeration.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsFinal )( CK_SESSION_HANDLE xSession )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    /*
     * Check parameters.
     */

    if( ( CK_BBOOL ) CK_FALSE == pxSession->xFindObjectInit )
    {
        xResult = CKR_OPERATION_NOT_INITIALIZED;
    }
    else
    {
        /*
         * Clean-up find objects state.
         */

        pxSession->xFindObjectInit = CK_FALSE;
        pxSession->xFindObjectComplete = CK_FALSE;
        pxSession->xFindObjectClass = 0;
    }

    return xResult;
}

/**
 * @brief Begin a digital signature generation session.
 */
CK_DEFINE_FUNCTION( CK_RV, C_SignInit )( CK_SESSION_HANDLE xSession,
                                         CK_MECHANISM_PTR pxMechanism,
                                         CK_OBJECT_HANDLE xKey )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( xSession );
    ( void ) ( pxMechanism );
    ( void ) ( xKey );

    return CKR_OK;
}

/**
 * @brief Digitally sign the indicated cryptographic hash bytes.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Sign )( CK_SESSION_HANDLE xSession,
                                     CK_BYTE_PTR pucData,
                                     CK_ULONG ulDataLen,
                                     CK_BYTE_PTR pucSignature,
                                     CK_ULONG_PTR pulSignatureLen )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSessionObj = prvSessionPointerFromHandle( xSession );

    /*
     * Support length check.
     */

    if( NULL == pucSignature )
    {
        *pulSignatureLen = pkcs11SUPPORTED_KEY_BITS / 8;
    }
    else
    {
        /*
         * Check algorithm support.
         */

        if( ( CK_ULONG ) cryptoSHA256_DIGEST_BYTES != ulDataLen )
        {
            xResult = CKR_DATA_LEN_RANGE;
        }

        /*
         * Sign the data.
         */

        if( CKR_OK == xResult )
        {
            if( 0 != pxSessionObj->pxCurrentKey->pfnSavedMbedSign(
                    pxSessionObj->pxCurrentKey->pvSavedMbedPkCtx,
                    MBEDTLS_MD_SHA256,
                    pucData,
                    ulDataLen,
                    pucSignature,
                    ( size_t * ) pulSignatureLen,
                    mbedtls_ctr_drbg_random,
                    &pxSessionObj->xMbedDrbgCtx ) )
            {
                xResult = CKR_FUNCTION_FAILED;
            }
        }
    }

    return xResult;
}

/**
 * @brief Begin a digital signature verification session.
 */
CK_DEFINE_FUNCTION( CK_RV, C_VerifyInit )( CK_SESSION_HANDLE xSession,
                                           CK_MECHANISM_PTR pxMechanism,
                                           CK_OBJECT_HANDLE xKey )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( xSession );
    ( void ) ( pxMechanism );
    ( void ) ( xKey );

    return CKR_OK;
}

/**
 * @brief Verify the digital signature of the specified data using the public
 * key attached to this session.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Verify )( CK_SESSION_HANDLE xSession,
                                       CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataLen,
                                       CK_BYTE_PTR pucSignature,
                                       CK_ULONG ulSignatureLen )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSessionObj = prvSessionPointerFromHandle( xSession );

    /* Verify the signature. */
    if( 0 != pxSessionObj->pxCurrentKey->xMbedPkInfo.verify_func(
        pxSessionObj->pxCurrentKey->pvSavedMbedPkCtx,
        MBEDTLS_MD_SHA256,
        pucData,
        ulDataLen,
        pucSignature,
        ulSignatureLen ) )
    {
        xResult = CKR_SIGNATURE_INVALID;
    }

    /* Return the signature verification result. */
    return xResult;
}

/**
 * @brief Generate cryptographically random bytes.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GenerateRandom )( CK_SESSION_HANDLE xSession,
                                               CK_BYTE_PTR pucRandomData,
                                               CK_ULONG ulRandomLen )
{   /*lint !e9072 It's OK to have different parameter name. */
    P11SessionPtr_t pxSessionObj = prvSessionPointerFromHandle( xSession );

    if( 0 != mbedtls_ctr_drbg_random( &pxSessionObj->xMbedDrbgCtx, pucRandomData, ulRandomLen ) )
    {
        return CKR_FUNCTION_FAILED;
    }

    return CKR_OK;
}
