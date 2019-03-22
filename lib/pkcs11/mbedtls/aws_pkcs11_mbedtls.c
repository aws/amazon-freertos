/*
 * Amazon FreeRTOS mbedTLS-based PKCS#11 V1.0.7
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
#include "aws_pkcs11_config.h"
#include "aws_crypto.h"
#include "aws_pkcs11.h"
#include "aws_pkcs11_mbedtls.h"
#include "aws_pkcs11_pal.h"

/* mbedTLS includes. */
#include "mbedtls/pk.h"
#include "mbedtls/pk_internal.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/sha256.h"
#include "mbedtls/base64.h"
#include "threading_alt.h"

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

#define PKCS11_PRINT( X )    vLoggingPrintf X

#define pkcs11NO_OPERATION            ( ( CK_MECHANISM_TYPE ) 0xFFFFFFFFF )

/* The size of the buffer malloc'ed for the exported public key in C_GenerateKeyPair */
#define pkcs11KEY_GEN_MAX_DER_SIZE    200

/* PKCS#11 Object */
typedef struct P11Struct_t
{
    CK_BBOOL xIsInitialized;
    mbedtls_ctr_drbg_context xMbedDrbgCtx;
    mbedtls_entropy_context xMbedEntropyContext;
} P11Struct_t, * P11Context_t;

static P11Struct_t xP11Context;



/**
 * @brief Session structure.
 */
typedef struct P11Session
{
    CK_ULONG ulState;
    CK_BBOOL xOpened;
    CK_MECHANISM_TYPE xOperationInProgress;
    CK_BBOOL xFindObjectInit;
    CK_BBOOL xFindObjectComplete;
    SearchableAttributes_t * xFindObjectTemplate; /* Pointer to the template of the search in progress. Should be NULL if no search in progres. */
    uint8_t xFindObjectLabelLength;
    SemaphoreHandle_t xVerifyMutex;               /* Protects the verification key from being modified while in use. */
    mbedtls_pk_context xVerifyKey;
    SemaphoreHandle_t xSignMutex;                 /* Protects the signing key from being modified while in use. */
    mbedtls_pk_context xSignKey;
    mbedtls_sha256_context xSHA256Context;
} P11Session_t, * P11SessionPtr_t;


/**
 * @brief Cryptoki module attribute definitions.
 */
#define pkcs11SLOT_ID               1

#define pkcs11SUPPORTED_KEY_BITS    2048


/**
 * @brief Helper definitions.
 */
#define pkcs11CREATE_OBJECT_MIN_ATTRIBUTE_COUNT             3

#define pkcs11GENERATE_KEY_PAIR_KEYTYPE_ATTRIBUTE_INDEX     0
#define pkcs11GENERATE_KEY_PAIR_ECPARAMS_ATTRIBUTE_INDEX    1



/*-----------------------------------------------------------*/



/**
 * @brief Maps an opaque caller session handle into its internal state structure.
 */
P11SessionPtr_t prvSessionPointerFromHandle( CK_SESSION_HANDLE xSession )
{
    return ( P11SessionPtr_t ) xSession; /*lint !e923 Allow casting integer type to pointer for handle. */
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
    NULL,    /*C_CloseAllSessions*/
    NULL,    /*C_GetSessionInfo*/
    NULL,    /*C_GetOperationState*/
    NULL,    /*C_SetOperationState*/
    C_Login, /*C_Login*/
    NULL,    /*C_Logout*/
    C_CreateObject,
    NULL,    /*C_CopyObject*/
    C_DestroyObject,
    NULL,    /*C_GetObjectSize*/
    C_GetAttributeValue,
    NULL,    /*C_SetAttributeValue*/
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

/*-----------------------------------------------------------*/

/* Note: Before prvMbedTLS_Initialize can be called, CRYPTO_Init()
 * must be called to initialize the mbedTLS mutex & heap management functions. */
CK_RV prvMbedTLS_Initialize( void )
{
    CK_RV xResult = CKR_OK;

    if( xP11Context.xIsInitialized == CK_TRUE )
    {
        xResult = CKR_CRYPTOKI_ALREADY_INITIALIZED;
    }

    if( xResult == CKR_OK )
    {
        /* Initialze the entropy source and DRBG for the PKCS#11 module */
        mbedtls_entropy_init( &xP11Context.xMbedEntropyContext );
        mbedtls_ctr_drbg_init( &xP11Context.xMbedDrbgCtx );

        if( 0 != mbedtls_ctr_drbg_seed( &xP11Context.xMbedDrbgCtx,
                                        mbedtls_entropy_func,
                                        &xP11Context.xMbedEntropyContext,
                                        NULL,
                                        0 ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
        else
        {
            xP11Context.xIsInitialized = CK_TRUE;
        }
    }

    return xResult;
}



CK_RV xCreateSearchableAttributeTemplate( SearchableAttributes_t * pxFindObjectInfo,
                                          CK_ATTRIBUTE_PTR pxTemplate,
                                          CK_ULONG ulCount )
{
    CK_RV xResult = CKR_OK;

    /* Unpack provided template and store information serially. */
    if( xResult == CKR_OK )
    {
        for( int i = 0; i < ulCount; i++ )
        {
            CK_ATTRIBUTE xAttribute = pxTemplate[ i ];

            switch( xAttribute.type ) /* TODO: Are we concerned about a template with the same attribute in it twice? */
            {
                case ( CKA_LABEL ):

                    if( xAttribute.ulValueLen < pkcs11MAX_LABEL_LENGTH )
                    {
                        memcpy( pxFindObjectInfo->cLabel, xAttribute.pValue, xAttribute.ulValueLen );
                        pxFindObjectInfo->xLabelIsValid = CK_TRUE;
                    }
                    else
                    {
                        configPRINTF( ( "Label exceeds maximum object label length (%d) \r\n", pkcs11MAX_LABEL_LENGTH ) );
                        xResult = CKR_TEMPLATE_INCONSISTENT;
                    }

                    break;

                case ( CKA_CLASS ):
                    memcpy( &pxFindObjectInfo->xClass, xAttribute.pValue, sizeof( CK_OBJECT_CLASS ) );
                    pxFindObjectInfo->xClassIsValid = CK_TRUE;
                    break;

                default:
                    /* Do nothing.  It is one of the other attributes. */
                    /*xResult = CKR_TEMPLATE_INCONSISTENT; */
                    /*configPRINTF( ("Find objects only supported by label and/or class") ); */
                    break;
            }
        }
    }

    return xResult;
}

/**
 * @brief Initialize the Cryptoki module for use.
 */
#if !defined( pkcs11configC_INITIALIZE_ALT )
    CK_DEFINE_FUNCTION( CK_RV, C_Initialize )( CK_VOID_PTR pvInitArgs )
    { /*lint !e9072 It's OK to have different parameter name. */
        ( void ) ( pvInitArgs );

        return prvMbedTLS_Initialize();
    }
#endif /* if !defined( pkcs11configC_INITIALIZE_ALT ) */

/**
 * @brief Un-initialize the Cryptoki module.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Finalize )( CK_VOID_PTR pvReserved )
{
    /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    if( NULL != pvReserved )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( xP11Context.xIsInitialized == CK_FALSE )
    {
        xResult = CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    if( xResult == CKR_OK )
    {
        if( NULL != &xP11Context.xMbedEntropyContext )
        {
            mbedtls_entropy_free( &xP11Context.xMbedEntropyContext );
        }

        if( NULL != &xP11Context.xMbedDrbgCtx )
        {
            mbedtls_ctr_drbg_free( &xP11Context.xMbedDrbgCtx );
        }

        xP11Context.xIsInitialized = CK_FALSE;
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

    /* Since the mbedTLS implementation of PKCS#11 does not depend
     * on a physical token, this parameter is ignored. */
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

    /* Check that the PKCS #11 module is initialized. */
    if( xP11Context.xIsInitialized != CK_TRUE )
    {
        xResult = CKR_CRYPTOKI_NOT_INITIALIZED;
    }

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

        /*
         * Zero out the session structure.
         */
        if( CKR_OK == xResult )
        {
            memset( pxSessionObj, 0, sizeof( P11Session_t ) );
        }

        pxSessionObj->xSignMutex = xSemaphoreCreateMutex();

        if( NULL == pxSessionObj->xSignMutex )
        {
            xResult = CKR_HOST_MEMORY;
        }

        pxSessionObj->xVerifyMutex = xSemaphoreCreateMutex();

        if( NULL == pxSessionObj->xVerifyMutex )
        {
            xResult = CKR_HOST_MEMORY;
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

        if( NULL != pxSession->xSignKey.pk_ctx )
        {
            mbedtls_pk_free( &pxSession->xSignKey );
        }

        if( NULL != pxSession->xSignMutex )
        {
            vSemaphoreDelete( pxSession->xSignMutex );
        }

        /* Free the public key context if it exists. */
        if( NULL != pxSession->xVerifyKey.pk_ctx )
        {
            mbedtls_pk_free( &pxSession->xVerifyKey );
        }

        if( NULL != pxSession->xVerifyMutex )
        {
            vSemaphoreDelete( pxSession->xVerifyMutex );
        }

        if( NULL != &pxSession->xSHA256Context )
        {
            mbedtls_sha256_free( &pxSession->xSHA256Context );
        }

        vPortFree( pxSession );
    }
    else
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }

    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_Login )( CK_SESSION_HANDLE hSession,
                                      CK_USER_TYPE userType,
                                      CK_UTF8CHAR_PTR pPin,
                                      CK_ULONG ulPinLen )
{
    /* THIS FUNCTION IS NOT IMPLEMENTED FOR MBEDTLS-BASED PORTS.
     * If login capability is required, implement it here.
     * Defined for compatibility with other PKCS #11 ports. */
    return CKR_OK;
}


CK_RV prvCreateCertificate( SearchableAttributes_t * pxSearchable,
                            CK_ATTRIBUTE_PTR pxTemplate,
                            CK_ULONG ulCount,
                            CK_OBJECT_HANDLE_PTR pxObject )
{
    SearchableAttributes_t xAttributes;
    CK_RV xResult = CKR_OK;
    CK_BYTE_PTR pxCertificateValue = NULL;
    CK_ULONG xCertificateLength = 0;

    /* Search for the pointer to the certificate VALUE. */
    for( int i = 0; i < ulCount; i++ )
    {
        CK_ATTRIBUTE xAttribute = pxTemplate[ i ];

        if( xAttribute.type == CKA_VALUE )
        {
            pxCertificateValue = xAttribute.pValue;
            xCertificateLength = xAttribute.ulValueLen;
        }
    }

    if( pxCertificateValue == NULL )
    {
        xResult = CKR_TEMPLATE_INCOMPLETE;
    }

    if( xResult == CKR_OK )
    {
        *pxObject = PKCS11_PAL_SaveObject( pxSearchable, pxCertificateValue, xCertificateLength );

        if( *pxObject == 0 ) /*Invalid handle. */
        {
            xResult = CKR_DEVICE_MEMORY;
        }
    }

    return xResult;
}

CK_RV prvWriteKey()
{
}

CK_RV prvCreateEcPrivateKey( mbedtls_pk_context * pxMbedContext,
                             SearchableAttributes_t * pxSearchable,
                             CK_ATTRIBUTE_PTR pxTemplate,
                             CK_ULONG ulCount,
                             CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_RV xResult = CKR_OK;
    int lMbedReturn;
    /* Key will be assembled in the mbedTLS key context and then exported to DER for storage. */
    mbedtls_ecp_keypair * pxKeyPair;

    pxKeyPair = ( mbedtls_ecp_keypair * ) pxMbedContext->pk_ctx;
    mbedtls_ecp_keypair_init( pxKeyPair );
    mbedtls_ecp_group_init( &pxKeyPair->grp );
    /*mbedtls_ecp_point_init( &pxKeyPair->d ); */
    /* At this time, only P-256 curves are supported. */
    mbedtls_ecp_group_load( &pxKeyPair->grp, MBEDTLS_ECP_DP_SECP256R1 );

    for( int i = 0; i < ulCount; i++ )
    {
        CK_ATTRIBUTE xAttribute = pxTemplate[ i ];

        switch( xAttribute.type )
        {
            case ( CKA_CLASS ):
            case ( CKA_KEY_TYPE ):
            case ( CKA_LABEL ):
            case ( CKA_TOKEN ):

                /* Do nothing.
                 * At this time there is only token object support.
                 * Key type was checked previously.
                 * CLASS and LABEL have already been parsed into the Searchable Attributes. */
                break;

            case ( CKA_EC_PARAMS ):

                if( memcmp( ( CK_BYTE[] ) pkcs11DER_ENCODED_OID_P256, xAttribute.pValue, xAttribute.ulValueLen ) )
                {
                    PKCS11_PRINT( ( "ERROR: Only elliptic curve P-256 is supported.\r\n" ) );
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            case ( CKA_VALUE ):
                lMbedReturn = mbedtls_mpi_read_binary( &pxKeyPair->d,
                                                       xAttribute.pValue,
                                                       xAttribute.ulValueLen );

                if( lMbedReturn != 0 )
                {
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            default:
                xResult = CKR_TEMPLATE_INCONSISTENT;
                break;
        }
    }

    return xResult;
}


CK_RV prvCreateRsaPrivateKey( mbedtls_pk_context * pxMbedContext,
                              SearchableAttributes_t * pxSearchable,
                              CK_ATTRIBUTE_PTR pxTemplate,
                              CK_ULONG ulCount,
                              CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_RV xResult = CKR_OK;
    mbedtls_rsa_context * pxRsaContext;
    int lMbedReturn = 0;
    CK_BBOOL xBool;

    pxRsaContext = pxMbedContext->pk_ctx;
    mbedtls_rsa_init( pxRsaContext, MBEDTLS_RSA_PKCS_V15, 0 /*ignored.*/ );

    /* Get the memory management of this context right in the morning. */

    /* Parse template and collect the relevant parts. */
    for( int i = 0; i < ulCount; i++ )
    {
        CK_ATTRIBUTE xAttribute = pxTemplate[ i ];

        switch( xAttribute.type )
        {
            case ( CKA_TOKEN ):
            case ( CKA_CLASS ):
            case ( CKA_LABEL ):
            case ( CKA_KEY_TYPE ):

                /* Do nothing.
                 * At this time there is only token object support.
                 * Key type was checked previously.
                 * CLASS and LABEL have already been parsed into the Searchable Attributes. */
                break;

            case ( CKA_SIGN ):
                memcpy( &xBool, xAttribute.pValue, xAttribute.ulValueLen );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "Only RSA private keys with signing permissions supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
                }

                break;

            case ( CKA_MODULUS ):
                lMbedReturn |= mbedtls_rsa_import_raw( pxRsaContext,
                                                       xAttribute.pValue, xAttribute.ulValueLen, /* N */
                                                       NULL, 0,                                  /* P */
                                                       NULL, 0,                                  /* Q */
                                                       NULL, 0,                                  /* D */
                                                       NULL, 0 );                                /* E */
                break;

            case ( CKA_PUBLIC_EXPONENT ):
                lMbedReturn |= mbedtls_rsa_import_raw( pxRsaContext,
                                                       NULL, 0,                                    /* N */
                                                       NULL, 0,                                    /* P */
                                                       NULL, 0,                                    /* Q */
                                                       NULL, 0,                                    /* D */
                                                       xAttribute.pValue, xAttribute.ulValueLen ); /* E */
                break;

            case ( CKA_PRIME_1 ):
                lMbedReturn |= mbedtls_rsa_import_raw( pxRsaContext,
                                                       NULL, 0,                                  /* N */
                                                       xAttribute.pValue, xAttribute.ulValueLen, /* P */
                                                       NULL, 0,                                  /* Q */
                                                       NULL, 0,                                  /* D */
                                                       NULL, 0 );                                /* E */
                break;

            case ( CKA_PRIME_2 ):
                lMbedReturn |= mbedtls_rsa_import_raw( pxRsaContext,
                                                       NULL, 0,                                  /* N */
                                                       NULL, 0,                                  /* P */
                                                       xAttribute.pValue, xAttribute.ulValueLen, /* Q */
                                                       NULL, 0,                                  /* D */
                                                       NULL, 0 );                                /* E */
                break;

            case ( CKA_PRIVATE_EXPONENT ):
                lMbedReturn |= mbedtls_rsa_import_raw( pxRsaContext,
                                                       NULL, 0,                                  /* N */
                                                       NULL, 0,                                  /* P */
                                                       NULL, 0,                                  /* Q */
                                                       xAttribute.pValue, xAttribute.ulValueLen, /* D */
                                                       NULL, 0 );                                /* E */
                break;

            default:
                PKCS11_PRINT( ( "Unknown attribute found for RSA private key. %d \r\n", xAttribute.type ) );
                xResult = CKR_TEMPLATE_INCONSISTENT;
                break;
        }
    }

    if( lMbedReturn != 0 )
    {
        xResult = CKR_ATTRIBUTE_VALUE_INVALID;
    }

    return xResult;
}

CK_RV prvCreatePrivateKey( SearchableAttributes_t * pxSearchable,
                           CK_ATTRIBUTE_PTR pxTemplate,
                           CK_ULONG ulCount,
                           CK_OBJECT_HANDLE_PTR pxObject )
{
    /* TODO: How long is a typical RSA key anyhow?  */
#define MAX_LENGTH_KEY    3000
    mbedtls_pk_context xMbedContext;
    int lDerKeyLength;
    CK_BYTE_PTR pxDerKey = NULL;
    CK_RV xResult = CKR_OK;

    mbedtls_pk_init( &xMbedContext );

    /* RSA or Elliptic Curve? */
    /* TODO: I could actually check the attributes.*/
    if( ulCount > 6 )
    {
        mbedtls_rsa_context xRsaCtx;
        xMbedContext.pk_ctx = &xRsaCtx;
        xMbedContext.pk_info = &mbedtls_rsa_info;
        xResult = prvCreateRsaPrivateKey( &xMbedContext,
                                          pxSearchable,
                                          pxTemplate,
                                          ulCount,
                                          pxObject );
    }
    else
    {
        /* Key will be assembled in the mbedTLS key context and then exported to DER for storage. */
        mbedtls_ecp_keypair xKeyPair;
        xMbedContext.pk_ctx = &xKeyPair;
        xMbedContext.pk_info = &mbedtls_eckey_info; /* TODO: deprecated ecdsa vs eckey?*/
        xResult = prvCreateEcPrivateKey( &xMbedContext,
                                         pxSearchable,
                                         pxTemplate,
                                         ulCount,
                                         pxObject );
    }

    /* Convert back to DER and save to memory. */
    if( xResult == CKR_OK )
    {
        pxDerKey = pvPortMalloc( MAX_LENGTH_KEY );

        if( pxDerKey == NULL )
        {
            xResult = CKR_DEVICE_MEMORY;
        }
    }

    if( xResult == CKR_OK )
    {
        lDerKeyLength = mbedtls_pk_write_key_der( &xMbedContext, pxDerKey, MAX_LENGTH_KEY );

        if( lDerKeyLength < 0 )
        {
            xResult = CKR_ATTRIBUTE_VALUE_INVALID;
        }
    }

    if( xResult == CKR_OK )
    {
        *pxObject = PKCS11_PAL_SaveObject( pxSearchable,
                                           pxDerKey + ( MAX_LENGTH_KEY - lDerKeyLength ),
                                           lDerKeyLength );

        if( *pxObject == 0 )
        {
            xResult = CKR_DEVICE_MEMORY;
        }
    }

    if( pxDerKey != NULL )
    {
        vPortFree( pxDerKey );
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
    PKCS11_KeyTemplatePtr_t pxKeyTemplate = NULL;
    SearchableAttributes_t xSearchable;
    PKCS11_CertificateTemplatePtr_t pxCertificateTemplate = NULL;
    CK_ATTRIBUTE_PTR pxObjectClassAttribute = pxTemplate;
    CK_OBJECT_CLASS xClass;

    /* Avoid warnings about unused parameters. */
    ( void ) xSession;

    /*
     * Check parameters.
     */
    if( pxSession->xOpened != CK_TRUE )
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }

    if( ( NULL == pxTemplate ) ||
        ( NULL == pxObject ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( xResult == CKR_OK )
    {
        xResult = xCreateSearchableAttributeTemplate( &xSearchable, pxTemplate, ulCount );

        if( xSearchable.xClassIsValid != CK_TRUE )
        {
            xResult = CKR_TEMPLATE_INCOMPLETE;
        }
    }

    if( xResult == CKR_OK )
    {
        switch( xSearchable.xClass )
        {
            case CKO_CERTIFICATE:
                xResult = prvCreateCertificate( &xSearchable, pxTemplate, ulCount, pxObject );
                break;

            case CKO_PRIVATE_KEY:
                xResult = prvCreatePrivateKey( &xSearchable, pxTemplate, ulCount, pxObject );
                break;

            case CKO_PUBLIC_KEY:
                break;

            default:
                xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                break;
        }
    }

    return xResult;
}

/**
 * @brief Free resources attached to an object handle.
 */
CK_DEFINE_FUNCTION( CK_RV, C_DestroyObject )( CK_SESSION_HANDLE xSession,
                                              CK_OBJECT_HANDLE xObject )
{
    /* TODO: Delete objects from NVM. */
    ( void ) xSession;
    ( void ) xObject;

    PKCS11_PAL_DestroyObject( xObject );
    return CKR_OK;
}

/**
 * @brief Query the value of the specified cryptographic object attribute.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetAttributeValue )( CK_SESSION_HANDLE xSession,
                                                  CK_OBJECT_HANDLE xObject,
                                                  CK_ATTRIBUTE_PTR pxTemplate,
                                                  CK_ULONG ulCount )
{
    /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    CK_BBOOL xIsPrivate = CK_TRUE;
    CK_ULONG iAttrib;
    mbedtls_pk_context privateKeyContext;
    mbedtls_pk_type_t xKeyType;
    CK_KEY_TYPE xPkcsKeyType = ( CK_KEY_TYPE ) ~0;

    uint8_t * pxObjectValue = NULL;
    uint32_t ulLength = 0;

    /* Avoid warnings about unused parameters. */
    ( void ) xSession;

    if( NULL == pxTemplate )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        /*
         * Copy the object into a buffer.
         */
        xResult = PKCS11_PAL_GetObjectValue( xObject, &pxObjectValue, &ulLength, &xIsPrivate );
    }

    if( xResult == CKR_OK )
    {
        for( iAttrib = 0; iAttrib < ulCount && CKR_OK == xResult; iAttrib++ )
        {
            switch( pxTemplate[ iAttrib ].type )
            {
                case CKA_VALUE:

                    if( xIsPrivate == CK_TRUE )
                    {
                        pxTemplate[ iAttrib ].ulValueLen = CK_UNAVAILABLE_INFORMATION;
                        xResult = CKR_ATTRIBUTE_SENSITIVE;
                    }
                    else
                    {
                        if( pxTemplate[ iAttrib ].pValue == NULL )
                        {
                            pxTemplate[ iAttrib ].ulValueLen = ulLength;
                        }
                        else if( pxTemplate[ iAttrib ].ulValueLen < ulLength )
                        {
                            xResult = CKR_BUFFER_TOO_SMALL;
                        }
                        else
                        {
                            memcpy( pxTemplate[ iAttrib ].pValue, pxObjectValue, ulLength );
                        }
                    }

                    break;

                case CKA_KEY_TYPE:

                    if( pxTemplate[ iAttrib ].pValue == NULL )
                    {
                        pxTemplate[ iAttrib ].ulValueLen = sizeof( mbedtls_pk_type_t );
                    }
                    else if( pxTemplate[ iAttrib ].ulValueLen < sizeof( mbedtls_pk_type_t ) )
                    {
                        xResult = CKR_BUFFER_TOO_SMALL;
                    }
                    else
                    {
                        mbedtls_pk_init( &privateKeyContext );
                        xResult = mbedtls_pk_parse_key( &privateKeyContext, pxObjectValue, ulLength, NULL, 0 );

                        if( 0 != xResult )
                        {
                            xResult = CKR_FUNCTION_FAILED;
                        }
                        else
                        {
                            xKeyType = mbedtls_pk_get_type( &privateKeyContext );

                            switch( xKeyType )
                            {
                                case MBEDTLS_PK_RSA:
                                case MBEDTLS_PK_RSA_ALT:
                                case MBEDTLS_PK_RSASSA_PSS:
                                    xPkcsKeyType = CKK_RSA;
                                    break;

                                case MBEDTLS_PK_ECKEY:
                                case MBEDTLS_PK_ECKEY_DH:
                                    xPkcsKeyType = CKK_EC;
                                    break;

                                case MBEDTLS_PK_ECDSA:
                                    xPkcsKeyType = CKK_ECDSA;
                                    break;

                                default:
                                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                                    break;
                            }

                            memcpy( pxTemplate[ iAttrib ].pValue, &xPkcsKeyType, sizeof( CK_KEY_TYPE ) );
                        }

                        /* Free the mbedTLS structure used to parse the key. */
                        mbedtls_pk_free( &privateKeyContext );
                    }

                    break;

                default:
                    xResult = CKR_ATTRIBUTE_TYPE_INVALID;
            }
        }

        /* Free the buffer where object was stored. */
        PKCS11_PAL_GetObjectValueCleanup( pxObjectValue, ulLength );
    }

    return xResult;
}



/**
 * @brief Begin an enumeration sequence for the objects of the specified type.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsInit )( CK_SESSION_HANDLE xSession,
                                                CK_ATTRIBUTE_PTR pxTemplate,
                                                CK_ULONG ulCount )
{
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );
    CK_RV xResult = CKR_OK;
    SearchableAttributes_t * pxFindObjectInfo = NULL;

    /* Check inputs. */
    if( ( pxSession == NULL ) || ( pxSession->xOpened != CK_TRUE ) )
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
        configPRINTF( ( "Invalid session. \r\n" ) );
    }
    else if( pxSession->xFindObjectTemplate != NULL )
    {
        xResult = CKR_OPERATION_ACTIVE;
        configPRINTF( ( "Find object operation already in progress. \r\n" ) );
    }
    else if( NULL == pxTemplate )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else if( ( ulCount != 1 ) && ( ulCount != 2 ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
        configPRINTF( ( "Find objects can only filter by one attribute. \r\n" ) );
    }

    /* Malloc space to save template information. */
    if( xResult == CKR_OK )
    {
        pxFindObjectInfo = pvPortMalloc( sizeof( SearchableAttributes_t ) );
        pxSession->xFindObjectTemplate = pxFindObjectInfo;

        if( pxFindObjectInfo != NULL )
        {
            memset( pxFindObjectInfo, 0, sizeof( SearchableAttributes_t ) );
        }
        else
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    /* Unpack provided template and store information serially. */
    if( xResult == CKR_OK )
    {
        xResult = xCreateSearchableAttributeTemplate( pxFindObjectInfo, pxTemplate, ulCount );
    }

    /* Clean up memory if there was an error parsing the template. */
    if( xResult != CKR_OK )
    {
        if( pxFindObjectInfo != NULL )
        {
            vPortFree( pxFindObjectInfo );
            pxSession->xFindObjectTemplate = NULL;
        }
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

    if( pxSession->xFindObjectTemplate == NULL )
    {
        xResult = CKR_OPERATION_NOT_INITIALIZED;
        xDone = pdTRUE;
    }

    if( 0u == ulMaxObjectCount )
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

    if( ( pdFALSE == xDone ) )
    {
        *pxObject = PKCS11_PAL_FindObject( pxSession->xFindObjectTemplate );

        if( *pxObject != 0 ) /* 0 is always an invalid handle. */
        {
            *pulObjectCount = 1;
            xResult = CKR_OK;
        }
        else
        {
            PKCS11_PRINT( ( "ERROR: Object with label '%s' not found. \r\n", ( char * ) pxSession->xFindObjectTemplate ) );
            xResult = CKR_FUNCTION_FAILED;
        }
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
    if( pxSession->xOpened != CK_TRUE )
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }

    if( pxSession->xFindObjectTemplate == NULL )
    {
        xResult = CKR_OPERATION_NOT_INITIALIZED;
    }

    if( xResult == CKR_OK )
    {
        /*
         * Clean-up find objects state.
         */
        vPortFree( pxSession->xFindObjectTemplate );
        pxSession->xFindObjectTemplate = NULL;
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
    CK_BBOOL xIsPrivate;

    /*lint !e9072 It's OK to have different parameter name. */
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );
    uint8_t * keyData = NULL;
    uint32_t ulKeyDataLength = 0;

    if( NULL == pxMechanism )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        xResult = PKCS11_PAL_GetObjectValue( xKey, &keyData, &ulKeyDataLength, &xIsPrivate );

        if( xIsPrivate != CK_TRUE )
        {
            xResult = CKR_KEY_TYPE_INCONSISTENT;
        }

        if( xResult == CKR_OK )
        {
            if( pdTRUE == xSemaphoreTake( pxSession->xSignMutex, portMAX_DELAY ) )
            {
                /* Free the private key context if it exists.
                 * TODO: Check if the key is the same as was used previously. */
                if( NULL != pxSession->xSignKey.pk_ctx )
                {
                    mbedtls_pk_free( &pxSession->xSignKey );
                }

                mbedtls_pk_init( &pxSession->xSignKey );

                /* TODO: Remove this hack.
                 * mbedTLS call appends empty public key data when saving EC private key.
                 * a1 04        -> Application identifier of length 4
                 * 03 02     -> Bit string of length 2
                 *    00 00  -> "Public key"
                 * This is not handled by the parser.
                 * This has not been tested very carefully either.*/
                
                if( pxMechanism->mechanism == CKM_ECDSA )
                {
                    keyData[ 1 ] = keyData[ 1 ] - 6;
                    ulKeyDataLength -= 6;
                }

                if( 0 == mbedtls_pk_parse_key( &pxSession->xSignKey, keyData, ulKeyDataLength, NULL, 0 ) )
                {
                    /* TODO: Check the mechanism.  Note: Currently, mechanism is being set to CKM_SHA256, rather than
                     * CKM_RSA_PKCS
                     * CKM_SHA256_RSA_PKCS
                     * CKM_ECDSA
                     * Calling function does not know whether key is RSA or ECDSA.
                     * xKeyType = mbedtls_pk_get_type( &pxSession->xSignKey );
                     */
                }
                else
                {
                    xResult = CKR_KEY_HANDLE_INVALID;
                }

                xSemaphoreGive( pxSession->xSignMutex );
            }
            else
            {
                xResult = CKR_CANT_LOCK;
            }
        }

        PKCS11_PAL_GetObjectValueCleanup( keyData, ulKeyDataLength );
    }

    return xResult;
}


xMbedTLSSignatureToPkcs11Signature( CK_BYTE * pxSignaturePKCS, uint8_t * pxMbedSignature )
{

    /* Reconstruct the signature in PKCS #11 format. */
    CK_BYTE * pxNextLength;
    uint8_t ucSigComponentLength = pxMbedSignature[ 3 ];

    /* R Component. */
    if ( ucSigComponentLength == 33 )
    {
        memcpy( pxSignaturePKCS, &pxMbedSignature[ 5 ], 32 ); /* Chop off the leading zero. */
        pxNextLength = pxMbedSignature + 5 /* Sequence, length, integer, length, leading zero */ + 32 /*(R) */ + 1 /*(S's integer tag) */;
    }
    else
    {
        memcpy( &pxSignaturePKCS[ 32 - ucSigComponentLength ], &pxMbedSignature[ 4 ], ucSigComponentLength );
        pxNextLength = pxMbedSignature + 4 + 32 + 1;
    }

    /* S Component. */
    ucSigComponentLength = pxNextLength[ 0 ];

    if ( ucSigComponentLength == 33 )
    {
        memcpy( &pxSignaturePKCS[ 32 ], &pxNextLength[ 2 ], 32 ); /* Skip leading zero. */
    }
    else
    {
        memcpy( &pxSignaturePKCS[ 64 - ucSigComponentLength ], &pxNextLength[ 1 ], ucSigComponentLength );
    }
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

    size_t x = 100;
    uint8_t ecSignature[ 100 ];

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
                if( pdTRUE == xSemaphoreTake( pxSessionObj->xSignMutex, portMAX_DELAY ) )
                {
                    BaseType_t x = mbedtls_pk_sign( &pxSessionObj->xSignKey,
                                                    MBEDTLS_MD_SHA256,
                                                    pucData,
                                                    ulDataLen,
                                                    ecSignature,
                                                    ( size_t * ) &x,
                                                    mbedtls_ctr_drbg_random,
                                                    &xP11Context.xMbedDrbgCtx );

                    if( x != CKR_OK )
                    {
                        xResult = CKR_FUNCTION_FAILED;
                    }

                    xSemaphoreGive( pxSessionObj->xSignMutex );
                }
                else
                {
                    xResult = CKR_CANT_LOCK;
                }
            }
        }
    }

    if( xResult == CKR_OK )
    {
        mbedtls_pk_type_t xType = mbedtls_pk_get_type( &pxSessionObj->xSignKey );

        /* Reformat the signature */
        if( ( xType == MBEDTLS_PK_ECKEY ) ||
            ( xType == MBEDTLS_PK_ECDSA ) )
        {
            xMbedTLSSignatureToPkcs11Signature( pucSignature, &ecSignature );
            *pulSignatureLen = 64;
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
    CK_BBOOL xIsPrivate = CK_TRUE;
    P11SessionPtr_t pxSession;
    uint8_t * keyData = NULL;
    uint32_t ulKeyDataLength = 0;

    /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( xSession );

    pxSession = prvSessionPointerFromHandle( xSession );

    if( NULL == pxMechanism )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( xResult == CKR_OK )
    {
        xResult = PKCS11_PAL_GetObjectValue( xKey, &keyData, &ulKeyDataLength, &xIsPrivate );
    }

    if( ( xResult == CKR_OK ) && ( xIsPrivate != CK_FALSE ) )
    {
        xResult = CKR_KEY_TYPE_INCONSISTENT;
        PKCS11_PAL_GetObjectValueCleanup( keyData, ulKeyDataLength );
    }

    if( xResult == CKR_OK )
    {
        if( pdTRUE == xSemaphoreTake( pxSession->xVerifyMutex, portMAX_DELAY ) )
        {
            /* Free the public key context if it exists.
             * TODO: Check if the key is the same as used by last verify operation. */
            if( NULL != pxSession->xVerifyKey.pk_ctx )
            {
                mbedtls_pk_free( &pxSession->xVerifyKey );
            }

            mbedtls_pk_init( &pxSession->xVerifyKey );

            if( 0 != mbedtls_pk_parse_public_key( &pxSession->xVerifyKey, keyData, ulKeyDataLength ) )
            {
                if( 0 != mbedtls_pk_parse_key( &pxSession->xVerifyKey, keyData, ulKeyDataLength, NULL, 0 ) )
                {
                    xResult = CKR_KEY_HANDLE_INVALID;
                }
            }

            xSemaphoreGive( pxSession->xVerifyMutex );
        }
        else
        {
            xResult = CKR_CANT_LOCK;
        }

        PKCS11_PAL_GetObjectValueCleanup( keyData, ulKeyDataLength );
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

        if( pdTRUE == xSemaphoreTake( pxSessionObj->xVerifyMutex, portMAX_DELAY ) )
        {
            /* Verify the signature. If a public key is present, use it. */
            if( NULL != pxSessionObj->xVerifyKey.pk_ctx )
            {
                if( 0 != mbedtls_pk_verify( &pxSessionObj->xVerifyKey,
                                            MBEDTLS_MD_SHA256,
                                            pucData,
                                            ulDataLen,
                                            pucSignature,
                                            ulSignatureLen ) )
                {
                    xResult = CKR_SIGNATURE_INVALID;
                }
            }

            xSemaphoreGive( pxSessionObj->xVerifyMutex );
        }
        else
        {
            xResult = CKR_CANT_LOCK;
        }

        /* TODO: Deleted else. */
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
    /* Avoid warnings about unused parameters. */
    ( void ) ( pxPublicKey );
    ( void ) ( ulPrivateKeyAttributeCount );
    ( void ) ( ulPublicKeyAttributeCount );
    ( void ) ( xSession );

    PKCS11_GenerateKeyPrivateTemplatePtr_t pxPrivateTemplate = ( PKCS11_GenerateKeyPrivateTemplatePtr_t ) pxPrivateKeyTemplate;
    PKCS11_GenerateKeyPublicTemplatePtr_t pxPublicTemplate = ( PKCS11_GenerateKeyPublicTemplatePtr_t ) pxPublicKeyTemplate;

    CK_RV xResult = CKR_OK;
    uint8_t * pucDerFile = pvPortMalloc( pkcs11KEY_GEN_MAX_DER_SIZE );

    if( pucDerFile == NULL )
    {
        xResult = CKR_HOST_MEMORY;
    }

    if( CKM_EC_KEY_PAIR_GEN != pxMechanism->mechanism )
    {
        xResult = CKR_MECHANISM_PARAM_INVALID;
    }

    if( ( pxPrivateTemplate->xLabel.type != CKA_LABEL ) ||
        ( pxPublicTemplate->xLabel.type != CKA_LABEL ) ||
        ( pxPublicTemplate->xEcParams.type != CKA_EC_PARAMS ) )
    {
        xResult = CKR_TEMPLATE_INCOMPLETE;
    }

    if( 0 != strcmp( pkcs11ELLIPTIC_CURVE_NISTP256, pxPublicTemplate->xEcParams.pValue ) )
    {
        xResult = CKR_CURVE_NOT_SUPPORTED;
    }

    mbedtls_pk_context xCtx;
    mbedtls_pk_init( &xCtx );
    xResult = mbedtls_pk_setup( &xCtx, mbedtls_pk_info_from_type( MBEDTLS_PK_ECKEY ) );

    if( xResult == 0 )
    {
        /*mbedtls_ecdsa_init( xCtx.pk_ctx ); */

        if( 0 != mbedtls_ecp_gen_key( MBEDTLS_ECP_DP_SECP256R1,
                                      mbedtls_pk_ec( xCtx ),
                                      mbedtls_ctr_drbg_random,
                                      &xP11Context.xMbedDrbgCtx ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    if( xResult == CKR_OK )
    {
        xResult = mbedtls_pk_write_key_der( &xCtx, pucDerFile, pkcs11KEY_GEN_MAX_DER_SIZE );
    }

    /* TODO: Write back public key value if CKA_VALUE is provided in template. */
    /* TODO: Think on how to give 2 different labels to private public key for non mbed implementations. */

    if( xResult > 0 )
    {
        *pxPrivateKey = PKCS11_PAL_SaveObject( &pxPrivateTemplate->xLabel, pucDerFile + pkcs11KEY_GEN_MAX_DER_SIZE - xResult, xResult );
        /* FIXME: This is a hack.*/
        *pxPublicKey = *pxPrivateKey + 1;
        xResult = CKR_OK;
    }
    else
    {
        xResult = CKR_GENERAL_ERROR;
    }

    /* Clean up. */
    if( NULL != pucDerFile )
    {
        vPortFree( pucDerFile );
    }

    mbedtls_pk_free( &xCtx );

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

    /* Avoid warnings about unused parameters. */
    ( void ) xSession;

    if( ( NULL == pucRandomData ) ||
        ( ulRandomLen == 0 ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        if( 0 != mbedtls_ctr_drbg_random( &xP11Context.xMbedDrbgCtx, pucRandomData, ulRandomLen ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    return xResult;
}
