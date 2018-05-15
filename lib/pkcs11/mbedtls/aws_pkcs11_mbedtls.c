/*
 * Amazon FreeRTOS mbedTLS-based PKCS#11 V1.0.3
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
 * @brief mbedTLS PKCS#11 implementation for software keys. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "aws_pkcs11_config.h"
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

typedef int ( * pfnMbedTlsSign )( void * ctx,
                                  mbedtls_md_type_t md_alg,
                                  const unsigned char * hash,
                                  size_t hash_len,
                                  unsigned char * sig,
                                  size_t * sig_len,
                                  int ( *f_rng )( void *, unsigned char *, size_t ),
                                  void * p_rng );

#define pkcs11NO_OPERATION    ( ( CK_MECHANISM_TYPE ) 0xFFFFFFFFF )

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
    CK_MECHANISM_TYPE xOperationInProgress;
    CK_BBOOL xFindObjectInit;
    CK_BBOOL xFindObjectComplete;
    CK_OBJECT_CLASS xFindObjectClass;
    mbedtls_ctr_drbg_context xMbedDrbgCtx;
    mbedtls_entropy_context xMbedEntropyContext;
    mbedtls_pk_context xPublicKey;
    mbedtls_sha256_context xSHA256Context;
} P11Session_t, * P11SessionPtr_t;

/**
 * @brief Cryptoki module attribute definitions.
 */
#define pkcs11SLOT_ID                      1
#define pkcs11OBJECT_HANDLE_PUBLIC_KEY     1
#define pkcs11OBJECT_HANDLE_PRIVATE_KEY    2
#define pkcs11OBJECT_HANDLE_CERTIFICATE    3

#define pkcs11SUPPORTED_KEY_BITS           2048


/**
 * @brief Helper definitions.
 */
#define pkcs11CREATE_OBJECT_MIN_ATTRIBUTE_COUNT                  3
#define pkcs11CERTIFICATE_ATTRIBUTE_COUNT                        3
#define pkcs11PUBLIC_KEY_ATTRIBUTE_COUNT                         3
#define pkcs11PRIVATE_KEY_ATTRIBUTE_COUNT                        4

#define pkcs11CREATE_OBJECT_CLASS_ATTRIBUTE_INDEX                0
#define pkcs11CREATE_OBJECT_CERTIFICATE_VALUE_ATTRIBUTE_INDEX    1
#define pkcs11CREATE_OBJECT_CERTIFICATE_TYPE_ATTRIBUTE_INDEX     2
#define pkcs11CREATE_OBJECT_PUBLIC_KEY_VALUE_ATTRIBUTE_INDEX     2
#define pkcs11CREATE_OBJECT_PRIVATE_KEY_VALUE_ATTRIBUTE_INDEX    3

#define pkcs11GENERATE_KEY_PAIR_KEYTYPE_ATTRIBUTE_INDEX          0
#define pkcs11GENERATE_KEY_PAIR_ECPARAMS_ATTRIBUTE_INDEX         1

/**
 * @brief Write file to filesystem (see PAL).
 */
extern BaseType_t PKCS11_PAL_SaveFile( char * pcFileName,
                                       uint8_t * pucData,
                                       uint32_t ulDataSize );

/**
 * @brief Read file from filesystem (see PAL).
 */
extern BaseType_t PKCS11_PAL_ReadFile( char * pcFileName,
                                       uint8_t ** ppucData,
                                       uint32_t * pulDataSize );

/**
 * @brief Free the buffer allocated in PKCS11_PAL_ReadFile (see PAL).
 */
extern void PKCS11_PAL_ReleaseFileData( uint8_t * pucBuffer,
                                        uint32_t ulBufferSize );
/*-----------------------------------------------------------*/

/**
 * @brief Maps an opaque caller session handle into its internal state structure.
 */
P11SessionPtr_t prvSessionPointerFromHandle( CK_SESSION_HANDLE xSession )
{
    return ( P11SessionPtr_t ) xSession; /*lint !e923 Allow casting integer type to pointer for handle. */
}

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
 * @brief Allow mbedTLS client authentication to use PKCS#11 without a separate
 * shim library. 
 */
static CK_RV prvSetupPkcs11SigningForMbedTls( P11SessionPtr_t pxSessionObj, 
                                              P11KeyPtr_t pxKeyObj )
{
    /* Swap out the signing function pointer. */
    memcpy(
        &pxKeyObj->xMbedPkInfo,
        pxKeyObj->xMbedPkCtx.pk_info,
        sizeof( pxKeyObj->xMbedPkInfo ) );
    pxKeyObj->pfnSavedMbedSign = pxKeyObj->xMbedPkInfo.sign_func;
    pxKeyObj->xMbedPkInfo.sign_func = prvPrivateKeySigningCallback;
    pxKeyObj->xMbedPkCtx.pk_info = &pxKeyObj->xMbedPkInfo;

    /* Swap out the underlying internal key context. */
    pxKeyObj->pvSavedMbedPkCtx = pxKeyObj->xMbedPkCtx.pk_ctx;
    pxKeyObj->xMbedPkCtx.pk_ctx = pxSessionObj;

    return CKR_OK;
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
            xResult = prvSetupPkcs11SigningForMbedTls( pxSessionObj, 
                                                       pxSessionObj->pxCurrentKey );
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
    if( pdFALSE == PKCS11_PAL_ReadFile( pkcs11configFILE_NAME_CLIENT_CERTIFICATE,
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
    if( pdFALSE == PKCS11_PAL_ReadFile( pkcs11configFILE_NAME_KEY,
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
        PKCS11_PAL_ReleaseFileData( pucCertificateData, ulCertificateDataLength );
    }

    if( ( NULL != pucKeyData ) && ( pdTRUE == xFreeKey ) )
    {
        PKCS11_PAL_ReleaseFileData( pucKeyData, ulKeyDataLength );
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
    NULL, /*C_GetInfo */
    C_GetFunctionList,
    C_GetSlotList,
    NULL, /*C_GetSlotInfo*/
    NULL, /*C_GetTokenInfo*/
    NULL, /*C_GetMechanismList*/
    NULL, /*C_GetMechansimInfo */
    NULL, /*C_InitToken*/
    NULL, /*C_InitPIN*/
    NULL, /*C_SetPIN*/
    C_OpenSession,
    C_CloseSession,
    NULL, /*C_CloseAllSessions*/
    NULL, /*C_GetSessionInfo*/
    NULL, /*C_GetOperationState*/
    NULL, /*C_SetOperationState*/
    NULL, /*C_Login*/
    NULL, /*C_Logout*/
    C_CreateObject,
    NULL, /*C_CopyObject*/
    C_DestroyObject,
    NULL, /*C_GetObjectSize*/
    C_GetAttributeValue,
    NULL, /*C_SetAttributeValue*/
    C_FindObjectsInit,
    C_FindObjects,
    C_FindObjectsFinal,
    NULL, /*C_EncryptInit*/
    NULL, /*C_Encrypt*/
    NULL, /*C_EncryptUpdate*/
    NULL, /*C_EncryptFinal*/
    NULL, /*C_DecryptInit*/
    NULL, /*C_Decrypt*/
    NULL, /*C_DecryptUpdate*/
    NULL, /*C_DecryptFinal*/
    C_DigestInit,
    NULL, /*C_Digest*/
    C_DigestUpdate,
    NULL, /* C_DigestKey*/
    C_DigestFinal,
    C_SignInit,
    C_Sign,
    NULL, /*C_SignUpdate*/
    NULL, /*C_SignFinal*/
    NULL, /*C_SignRecoverInit*/
    NULL, /*C_SignRecover*/
    C_VerifyInit,
    C_Verify,
    NULL, /*C_VerifyUpdate*/
    NULL, /*C_VerifyFinal*/
    NULL, /*C_VerifyRecoverInit*/
    NULL, /*C_VerifyRecover*/
    NULL, /*C_DigestEncryptUpdate*/
    NULL, /*C_DecryptDigestUpdate*/
    NULL, /*C_SignEncryptUpdate*/
    NULL, /*C_DecryptVerifyUpdate*/
    NULL, /*C_GenerateKey*/
    C_GenerateKeyPair,
    NULL, /*C_WrapKey*/
    NULL, /*C_UnwrapKey*/
    NULL, /*C_DeriveKey*/
    NULL, /*C_SeedRandom*/
    C_GenerateRandom,
    NULL, /*C_GetFunctionStatus*/
    NULL, /*C_CancelFunction*/
    NULL  /*C_WaitForSlotEvent*/
};

/**
 * @brief Initialize the Cryptoki module for use.
 */
#if !defined( pkcs11configC_INITIALIZE_ALT )
    CK_DEFINE_FUNCTION( CK_RV, C_Initialize )( CK_VOID_PTR pvInitArgs )
    { /*lint !e9072 It's OK to have different parameter name. */
        ( void ) ( pvInitArgs );

        /* Ensure that the FreeRTOS heap is used. */
        CRYPTO_ConfigureHeap();

        return CKR_OK;
    }
#endif

/**
 * @brief Un-initialize the Cryptoki module.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Finalize )( CK_VOID_PTR pvReserved )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    if( NULL != pvReserved )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    return xResult;
}

/**
 * @brief Query the list of interface function pointers.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetFunctionList )( CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    if( NULL == ppxFunctionList )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        *ppxFunctionList = &prvP11FunctionList;
    }

    return xResult;
}

/**
 * @brief Query the list of slots. A single default slot is implemented.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetSlotList )( CK_BBOOL xTokenPresent,
                                            CK_SLOT_ID_PTR pxSlotList,
                                            CK_ULONG_PTR pulCount )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    ( void ) ( xTokenPresent );

    if( NULL == pulCount )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else if( NULL == pxSlotList )
    {
        *pulCount = 1;
    }
    else
    {
        if( 0u == *pulCount )
        {
            xResult = CKR_BUFFER_TOO_SMALL;
        }
        else
        {
            pxSlotList[ 0 ] = pkcs11SLOT_ID;
            *pulCount = 1;
        }
    }

    return xResult;
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

    /* Check arguments. */
    if( NULL == pxSession )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    /* For legacy reasons, the CKF_SERIAL_SESSION bit MUST always be set. */
    if( 0 == ( CKF_SERIAL_SESSION & xFlags ) )
    {
        xResult = CKR_SESSION_PARALLEL_NOT_SUPPORTED;
    }

    /*
     * Make space for the context.
     */
    if( CKR_OK == xResult )
    {
        pxSessionObj = ( P11SessionPtr_t ) pvPortMalloc( sizeof( P11Session_t ) ); /*lint !e9087 Allow casting void* to other types. */

        if( NULL == pxSessionObj )
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    /*
     * Initialize RNG.
     */

    if( CKR_OK == xResult )
    {
        memset( pxSessionObj, 0, sizeof( P11Session_t ) );

        mbedtls_entropy_init( &pxSessionObj->xMbedEntropyContext );

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

    /*
     *   Initialize the operation in progress.
     */
    if( CKR_OK == xResult )
    {
        pxSessionObj->xOperationInProgress = pkcs11NO_OPERATION;
    }

    if( ( NULL != pxSessionObj ) && ( CKR_OK != xResult ) )
    {
        vPortFree( pxSessionObj );
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

        /* Free the public key context if it exists. */
        if( NULL != pxSession->xPublicKey.pk_ctx )
        {
            mbedtls_pk_free( &pxSession->xPublicKey );
        }

        mbedtls_ctr_drbg_free( &pxSession->xMbedDrbgCtx );
        vPortFree( pxSession );
    }
    else
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
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
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );
    void * pvContext = NULL;
    int32_t lMbedTLSParseResult = ~0;

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
        if( ( CKA_CLASS != pxTemplate[ pkcs11CREATE_OBJECT_CLASS_ATTRIBUTE_INDEX ].type ) ||
            ( sizeof( CK_OBJECT_CLASS ) != pxTemplate[ pkcs11CREATE_OBJECT_CLASS_ATTRIBUTE_INDEX ].ulValueLen ) )
        {
            xResult = CKR_ARGUMENTS_BAD;
        }
    }

    /*
     * Handle the object by class.
     */

    if( CKR_OK == xResult )
    {
        switch( *( ( uint32_t * ) pxTemplate[ pkcs11CREATE_OBJECT_CLASS_ATTRIBUTE_INDEX ].pValue ) )
        {
            case CKO_CERTIFICATE:

                /* Validate the attribute count for this object class. */
                if( pkcs11CERTIFICATE_ATTRIBUTE_COUNT != ulCount )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Validate the next attribute type. */
                if( CKA_VALUE != pxTemplate[ pkcs11CREATE_OBJECT_CERTIFICATE_VALUE_ATTRIBUTE_INDEX ].type )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Verify that the given certificate can be parsed. */
                pvContext = pvPortMalloc( sizeof( mbedtls_x509_crt ) );

                if( NULL != pvContext )
                {
                    mbedtls_x509_crt_init( ( mbedtls_x509_crt * ) pvContext );
                    lMbedTLSParseResult = mbedtls_x509_crt_parse( ( mbedtls_x509_crt * ) pvContext,
                                                                  pxTemplate[ pkcs11CREATE_OBJECT_CERTIFICATE_VALUE_ATTRIBUTE_INDEX ].pValue,
                                                                  pxTemplate[ pkcs11CREATE_OBJECT_CERTIFICATE_VALUE_ATTRIBUTE_INDEX ].ulValueLen );
                    mbedtls_x509_crt_free( ( mbedtls_x509_crt * ) pvContext );
                    vPortFree( pvContext );
                }
                else
                {
                    xResult = CKR_HOST_MEMORY;
                    break;
                }

                if( 0 != lMbedTLSParseResult )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                if( *( ( uint32_t * ) pxTemplate[ pkcs11CREATE_OBJECT_CERTIFICATE_TYPE_ATTRIBUTE_INDEX ].pValue )
                    == pkcs11CERTIFICATE_TYPE_USER )
                {
                    /* Write out the client certificate. */
                    if( pdFALSE == PKCS11_PAL_SaveFile( pkcs11configFILE_NAME_CLIENT_CERTIFICATE,
                                                        pxTemplate[ pkcs11CREATE_OBJECT_CERTIFICATE_VALUE_ATTRIBUTE_INDEX ].pValue,
                                                        pxTemplate[ pkcs11CREATE_OBJECT_CERTIFICATE_VALUE_ATTRIBUTE_INDEX ].ulValueLen ) )
                    {
                        xResult = CKR_DEVICE_ERROR;
                        break;
                    }
                    else
                    {
                        /* If successful, set object handle to certificate. */
                        *pxObject = pkcs11OBJECT_HANDLE_CERTIFICATE;
                    }
                }
                else if( *( ( uint32_t * ) pxTemplate[ pkcs11CREATE_OBJECT_CERTIFICATE_TYPE_ATTRIBUTE_INDEX ].pValue )
                         == pkcs11CERTIFICATE_TYPE_ROOT )
                {
                    /* Ignore writing the default root certificate. */
                }

                break;

            case CKO_PUBLIC_KEY:

                /* Validate the attribute count for this object class. */
                if( pkcs11PUBLIC_KEY_ATTRIBUTE_COUNT != ulCount )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Validate the next attribute type. */
                if( CKA_VALUE != pxTemplate[ pkcs11CREATE_OBJECT_PUBLIC_KEY_VALUE_ATTRIBUTE_INDEX ].type )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* If there's an existing public key, free it before importing a new one. */
                if( NULL != pxSession->xPublicKey.pk_ctx )
                {
                    mbedtls_pk_free( &pxSession->xPublicKey );
                }

                /* Parse the public key. */
                if( 0 != mbedtls_pk_parse_public_key( &( pxSession->xPublicKey ),
                                                      pxTemplate[ pkcs11CREATE_OBJECT_PUBLIC_KEY_VALUE_ATTRIBUTE_INDEX ].pValue,
                                                      pxTemplate[ pkcs11CREATE_OBJECT_PUBLIC_KEY_VALUE_ATTRIBUTE_INDEX ].ulValueLen ) )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                }
                else
                {
                    /* If successful, set object handle to public key. */
                    *pxObject = pkcs11OBJECT_HANDLE_PUBLIC_KEY;
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
                if( CKA_VALUE != pxTemplate[ pkcs11CREATE_OBJECT_PRIVATE_KEY_VALUE_ATTRIBUTE_INDEX ].type )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Verify that the given private key can be parsed. */
                pvContext = pvPortMalloc( sizeof( mbedtls_pk_context ) );

                if( NULL != pvContext )
                {
                    mbedtls_pk_init( ( mbedtls_pk_context * ) pvContext );
                    lMbedTLSParseResult = mbedtls_pk_parse_key( ( mbedtls_pk_context * ) pvContext,
                                                                pxTemplate[ pkcs11CREATE_OBJECT_PRIVATE_KEY_VALUE_ATTRIBUTE_INDEX ].pValue,
                                                                pxTemplate[ pkcs11CREATE_OBJECT_PRIVATE_KEY_VALUE_ATTRIBUTE_INDEX ].ulValueLen,
                                                                NULL,
                                                                0 );
                    mbedtls_pk_free( ( mbedtls_pk_context * ) pvContext );
                    vPortFree( pvContext );
                }
                else
                {
                    xResult = CKR_HOST_MEMORY;
                    break;
                }

                if( 0 != lMbedTLSParseResult )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Write out the key. */
                if( pdFALSE == PKCS11_PAL_SaveFile( pkcs11configFILE_NAME_KEY,
                                                    pxTemplate[ pkcs11CREATE_OBJECT_PRIVATE_KEY_VALUE_ATTRIBUTE_INDEX ].pValue,
                                                    pxTemplate[ pkcs11CREATE_OBJECT_PRIVATE_KEY_VALUE_ATTRIBUTE_INDEX ].ulValueLen ) )
                {
                    xResult = CKR_DEVICE_ERROR;
                    break;
                }
                else
                {
                    /* If successful, set object handle to private key. */
                    *pxObject = pkcs11OBJECT_HANDLE_PRIVATE_KEY;
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
    P11KeyPtr_t pxKey = ( P11KeyPtr_t )xObject;

    ( void ) ( xSession );

    if( NULL != pxKey &&
        pkcs11OBJECT_HANDLE_PRIVATE_KEY != xObject &&
        pkcs11OBJECT_HANDLE_PUBLIC_KEY != xObject &&
        pkcs11OBJECT_HANDLE_CERTIFICATE != xObject )
    {
        prvFreeKey( pxKey );
    }
    else
    {
        /*
         * This implementation uses virtual handles, and the certificate and
         * private key data are attached to the session, so nothing to do here.
         */
    }

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


    if( NULL == pxTemplate )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
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

                    switch( xObject )
                    {
                        case pkcs11OBJECT_HANDLE_CERTIFICATE:
                            pvAttr = ( CK_VOID_PTR ) pxSession->pxCurrentKey->xMbedX509Cli.raw.p; /*lint !e9005 !e9087 Allow casting other types to void*. */
                            ulAttrLength = pxSession->pxCurrentKey->xMbedX509Cli.raw.len;
                            break;

                        case pkcs11OBJECT_HANDLE_PUBLIC_KEY:
                            pvAttr = ( CK_VOID_PTR ) ( &pxSession->xPublicKey );
                            ulAttrLength = sizeof( mbedtls_pk_context );
                            break;

                        default:
                            xResult = CKR_ARGUMENTS_BAD;
                            break;
                    }

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
    CK_RV xResult = CKR_OK;

    ( void ) ( ulCount );

    /*
     * Check parameters.
     */
    if( NULL == pxTemplate )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        /*
         * Allow filtering on a single object class attribute.
         */

        pxSession->xFindObjectInit = CK_TRUE;
        pxSession->xFindObjectComplete = CK_FALSE;
        memcpy( &pxSession->xFindObjectClass,
                pxTemplate[ 0 ].pValue,
                sizeof( CK_OBJECT_CLASS ) );
    }

    return xResult;
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
    if( ( NULL == pxObject ) ||
        ( NULL == pulObjectCount ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
        xDone = pdTRUE;
    }

    if( ( pdFALSE == xDone ) &&
        ( ( CK_BBOOL ) CK_FALSE == pxSession->xFindObjectInit ) )
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

CK_DEFINE_FUNCTION( CK_RV, C_DigestInit )( CK_SESSION_HANDLE xSession,
                                           CK_MECHANISM_PTR pMechanism )
{
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( pxSession == NULL )
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }

    if( pMechanism->mechanism != CKM_SHA256 )
    {
        xResult = CKR_MECHANISM_INVALID;
    }

    /*
     * Initialize the requested hash type
     */
    if( xResult == CKR_OK )
    {
        mbedtls_sha256_init( &pxSession->xSHA256Context );

        if( 0 != mbedtls_sha256_starts_ret( &pxSession->xSHA256Context, 0 ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
        else
        {
            pxSession->xOperationInProgress = pMechanism->mechanism;
        }
    }

    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_DigestUpdate )( CK_SESSION_HANDLE xSession,
                                             CK_BYTE_PTR pPart,
                                             CK_ULONG ulPartLen )
{
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( pxSession == NULL )
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }
    else if( pxSession->xOperationInProgress != CKM_SHA256 )
    {
        xResult = CKR_OPERATION_NOT_INITIALIZED;
    }

    if( xResult == CKR_OK )
    {
        if( 0 != mbedtls_sha256_update_ret( &pxSession->xSHA256Context, pPart, ulPartLen ) )
        {
            xResult = CKR_FUNCTION_FAILED;
            pxSession->xOperationInProgress = pkcs11NO_OPERATION;
        }
    }

    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_DigestFinal )( CK_SESSION_HANDLE xSession,
                                            CK_BYTE_PTR pDigest,
                                            CK_ULONG_PTR pulDigestLen )
{
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( pxSession == NULL )
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }
    else if( pxSession->xOperationInProgress != CKM_SHA256 )
    {
        xResult = CKR_OPERATION_NOT_INITIALIZED;
        pxSession->xOperationInProgress = pkcs11NO_OPERATION;
    }

    if( xResult == CKR_OK )
    {
        if( pDigest == NULL )
        {
            /* Supply the required buffer size. */
            *pulDigestLen = pcks11SHA256_DIGEST_LENGTH;
        }
        else
        {
            if( *pulDigestLen < pcks11SHA256_DIGEST_LENGTH )
            {
                xResult = CKR_BUFFER_TOO_SMALL;
            }
            else
            {
                if( 0 != mbedtls_sha256_finish_ret( &pxSession->xSHA256Context, pDigest ) )
                {
                    xResult = CKR_FUNCTION_FAILED;
                }

                pxSession->xOperationInProgress = pkcs11NO_OPERATION;
            }
        }
    }

    return xResult;
}

/**
 * @brief Begin a digital signature generation session.
 */
CK_DEFINE_FUNCTION( CK_RV, C_SignInit )( CK_SESSION_HANDLE xSession,
                                         CK_MECHANISM_PTR pxMechanism,
                                         CK_OBJECT_HANDLE xKey )
{
    CK_RV xResult = CKR_OK;

    /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( xSession );
    ( void ) ( xKey );

    if( NULL == pxMechanism )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    return xResult;
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

    if( NULL == pulSignatureLen )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( CKR_OK == xResult )
    {
        if( NULL == pucSignature )
        {
            *pulSignatureLen = pkcs11SUPPORTED_KEY_BITS / 8;
        }
        else
        {
            /*
             * Check algorithm support.
             */
            if( CKR_OK == xResult )
            {
                if( ( CK_ULONG ) cryptoSHA256_DIGEST_BYTES != ulDataLen )
                {
                    xResult = CKR_DATA_LEN_RANGE;
                }
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
    }

    return xResult;
}

/**
 * @brief Begin a digital signature verification session.
 */
CK_DEFINE_FUNCTION( CK_RV, C_VerifyInit )( CK_SESSION_HANDLE xSession,
                                           CK_MECHANISM_PTR pxMechanism,
                                           CK_OBJECT_HANDLE xKey )
{
    CK_RV xResult = CKR_OK;

    /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( xSession );
    ( void ) ( xKey );

    if( NULL == pxMechanism )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    return xResult;
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
{
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSessionObj;

    /*
     * Check parameters.
     */
    if( ( NULL == pucData ) ||
        ( NULL == pucSignature ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        pxSessionObj = prvSessionPointerFromHandle( xSession ); /*lint !e9072 It's OK to have different parameter name. */

        /* Verify the signature. If a public key is present, use it. */
        if( NULL != pxSessionObj->xPublicKey.pk_ctx )
        {
            if( 0 != mbedtls_pk_verify( &pxSessionObj->xPublicKey,
                                        MBEDTLS_MD_SHA256,
                                        pucData,
                                        ulDataLen,
                                        pucSignature,
                                        ulSignatureLen ) )
            {
                xResult = CKR_SIGNATURE_INVALID;
            }
        }
        else
        {
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
        }
    }

    /* Return the signature verification result. */
    return xResult;
}

/**
 * @brief Generate a new assymetric keyset.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GenerateKeyPair )( CK_SESSION_HANDLE xSession,
                                                CK_MECHANISM_PTR pxMechanism,
                                                CK_ATTRIBUTE_PTR pxPublicKeyTemplate,
                                                CK_ULONG ulPublicKeyAttributeCount,
                                                CK_ATTRIBUTE_PTR pxPrivateKeyTemplate,
                                                CK_ULONG ulPrivateKeyAttributeCount,
                                                CK_OBJECT_HANDLE_PTR pxPublicKey,
                                                CK_OBJECT_HANDLE_PTR pxPrivateKey )
{
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSessionObj = prvSessionPointerFromHandle( xSession );
    P11KeyPtr_t pxNewKey = NULL;

    /* Check supported options. */
    if( CKM_EC_KEY_PAIR_GEN != pxMechanism->mechanism ||
        CKA_KEY_TYPE != pxPrivateKeyTemplate[ 
            pkcs11GENERATE_KEY_PAIR_KEYTYPE_ATTRIBUTE_INDEX ].type ||
        CKK_EC != *( ( CK_ULONG *)pxPrivateKeyTemplate[ 
            pkcs11GENERATE_KEY_PAIR_KEYTYPE_ATTRIBUTE_INDEX ].pValue ) ||
        CKA_KEY_TYPE != pxPublicKeyTemplate[ 
            pkcs11GENERATE_KEY_PAIR_KEYTYPE_ATTRIBUTE_INDEX ].type ||
        CKK_EC != *( ( CK_ULONG * )pxPrivateKeyTemplate[ 
            pkcs11GENERATE_KEY_PAIR_KEYTYPE_ATTRIBUTE_INDEX ].pValue ) ||
        CKA_EC_PARAMS != pxPublicKeyTemplate[ 
            pkcs11GENERATE_KEY_PAIR_ECPARAMS_ATTRIBUTE_INDEX ].type ||
        0 != strcmp( pkcs11ELLIPTIC_CURVE_NISTP256, pxPublicKeyTemplate[ 
            pkcs11GENERATE_KEY_PAIR_ECPARAMS_ATTRIBUTE_INDEX ].pValue ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    /* Allocate the wrapper key context. */
    if( 0 == xResult )
    {
        pxNewKey = pvPortMalloc( sizeof( P11Key_t ) );
        if( NULL == pxNewKey )
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    /* Initialize the mbed key fields. */
    if( 0 == xResult )
    {
        memset( pxNewKey, 0, sizeof( P11Key_t ) );
        xResult = mbedtls_pk_setup( &pxNewKey->xMbedPkCtx,
                                    mbedtls_pk_info_from_type( 
                                        MBEDTLS_PK_ECDSA ) );
    }

    /* Do the actual crypto aspect of key generation. */
    if( 0 == xResult )
    {
        mbedtls_ecdsa_init( pxNewKey->xMbedPkCtx.pk_ctx );
        if( 0 != mbedtls_ecdsa_genkey( pxNewKey->xMbedPkCtx.pk_ctx,
                                       MBEDTLS_ECP_DP_SECP256R1,
                                       mbedtls_ctr_drbg_random,
                                       &pxSessionObj->xMbedDrbgCtx ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    /* Complete the wrapped key context. */
    if( 0 == xResult )
    {
        xResult = prvSetupPkcs11SigningForMbedTls( pxSessionObj, pxNewKey );
    }

    /* Return the new private key. */
    if( 0 == xResult )
    {
        if( NULL != pxSessionObj->pxCurrentKey )
        {
            prvFreeKey( pxSessionObj->pxCurrentKey );
        }

        pxSessionObj->pxCurrentKey = pxNewKey;
        *pxPrivateKey = ( CK_OBJECT_HANDLE )pxNewKey;
        pxNewKey = NULL;
    }

    /* Clean up. */
    if( NULL != pxNewKey )
    {
        mbedtls_pk_free( &pxNewKey->xMbedPkCtx );
        vPortFree( pxNewKey );
    }

    return xResult;
}

/**
 * @brief Generate cryptographically random bytes.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GenerateRandom )( CK_SESSION_HANDLE xSession,
                                               CK_BYTE_PTR pucRandomData,
                                               CK_ULONG ulRandomLen )
{
    CK_RV xResult = CKR_OK;
    /*lint !e9072 It's OK to have different parameter name. */
    P11SessionPtr_t pxSessionObj = prvSessionPointerFromHandle( xSession );

    if( ( NULL == pucRandomData ) ||
        ( ulRandomLen == 0 ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        if( 0 != mbedtls_ctr_drbg_random( &pxSessionObj->xMbedDrbgCtx, pucRandomData, ulRandomLen ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    return xResult;
}
