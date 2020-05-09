/*
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright (c) 2019-2020 Arm Limited. All Rights Reserved.
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
 */

/**
 * @file iot_pkcs11_psa.c
 * @brief PSA based PKCS#11 implementation.
 */

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* PKCS#11 includes. */
#include "iot_pkcs11_config.h"
#include "iot_pkcs11.h"
#include "iot_pkcs11_psa_input_format.h"
#include "iot_pkcs11_psa_object_management.h"

/* Crypto include. */
#include "iot_crypto.h"

/* mbedTLS includes. */
#include "mbedtls/pk.h"
#include "mbedtls/pk_internal.h"

#if pkcs11configVENDOR_DEVICE_CERTIFICATE_SUPPORTED
#include "iot_pkcs11_psa_jitp_status.h"
#endif

#define PKCS11_PRINT( X )            vLoggingPrintf X
#define PKCS11_WARNING_PRINT( X )    /* vLoggingPrintf X */
#define pkcs11NO_OPERATION           ( ( CK_MECHANISM_TYPE ) 0xFFFFFFFFUL )

/*
 * The length of the content of an ECPoint n uncompressed
 * representation format defined by SEC1 §2.3.3.
 */
#define pkcs11KEY_ECPOINT_LENGTH     ( 65 )

/**
 * @brief Cryptoki module attribute definitions.
 */
#define pkcs11SLOT_ID                ( 1 )

/* Private defines for checking that attribute templates are complete. */
#define LABEL_IN_TEMPLATE            ( 1U )
#define PRIVATE_IN_TEMPLATE          ( 1U << 1 )
#define SIGN_IN_TEMPLATE             ( 1U << 2 )
#define EC_PARAMS_IN_TEMPLATE        ( 1U << 3 )
#define VERIFY_IN_TEMPLATE           ( 1U << 4 )

/**
 * @brief Helper definitions.
 */
#define PKCS11_MODULE_IS_INITIALIZED    ( ( xP11Context.xIsInitialized == CK_TRUE ) ? CK_TRUE : CK_FALSE )
#define PKCS11_SESSION_IS_OPEN( xSessionHandle )    ( ( ( ( P11SessionPtr_t ) xSessionHandle )->xOpened ) == CK_TRUE ? CKR_OK : CKR_SESSION_CLOSED )
#define PKCS11_SESSION_IS_VALID( xSessionHandle )    ( ( ( P11SessionPtr_t ) xSessionHandle != NULL ) ? PKCS11_SESSION_IS_OPEN( xSessionHandle ) : CKR_SESSION_HANDLE_INVALID )
#define PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSessionHandle )    ( PKCS11_MODULE_IS_INITIALIZED ? PKCS11_SESSION_IS_VALID( xSessionHandle ) : CKR_CRYPTOKI_NOT_INITIALIZED )

typedef struct P11Object_t
{
    CK_OBJECT_HANDLE xHandle;                            /* The "PSA Handle". */
    CK_BYTE xLabel[ pkcs11configMAX_LABEL_LENGTH + 1 ]; /* Plus 1 for the null terminator. */
} P11Object_t;

/* This structure helps the aws_pkcs11_psa.c maintain a mapping of all objects in one place.
 * The ObjectList maintains a list of what object handles are available.
 */
typedef struct P11ObjectList_t
{
    SemaphoreHandle_t xMutex; /* Mutex that protects write operations to the xObjects array. */
    P11Object_t xObjects[ pkcs11configMAX_NUM_OBJECTS ];
} P11ObjectList_t;

/* PKCS #11 Module Object */
typedef struct P11Struct_t
{
    CK_BBOOL xIsInitialized;                     /* Indicates whether PKCS #11 module has been initialized with a call to C_Initialize. */
    P11ObjectList_t xObjectList;                 /* List of PKCS #11 objects that have been found/created since module initialization.
                                                  * The array position indicates the "App Handle"  */
} P11Struct_t, * P11Context_t;

/* The global PKCS #11 module object.
 * Entropy/randomness and object lists are shared across PKCS #11 sessions. */
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
    CK_BYTE * pxFindObjectLabel;
    uint8_t xFindObjectLabelLength;
    SemaphoreHandle_t xVerifyMutex; /* Protects the verification key from being modified while in use. */
    psa_key_handle_t uxVerifyKey;
    CK_MECHANISM_TYPE xVerifyMechanism;    /* The mechanism of verify operation in progress. Set during C_VerifyInit. */
    psa_algorithm_t xVerifyAlgorithm; /* Verify algorithm that is compatible with the type of key. */
    SemaphoreHandle_t xSignMutex;   /* Protects the signing key from being modified while in use. */
    psa_key_handle_t uxSignKey;
    CK_MECHANISM_TYPE xSignMechanism;      /* Mechanism of the sign operation in progress. Set during C_SignInit. */
    psa_algorithm_t xSignAlgorithm; /* Signature algorithm that is compatible with the type of key. */
    psa_hash_operation_t *pxHashOperationHandle;   /* hash operation handle in PSA crypto service. */
} P11Session_t, * P11SessionPtr_t;

extern P11KeyConfig_t P11KeyConfig;
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
    C_GetTokenInfo,
    NULL, /*C_GetMechanismList*/
    C_GetMechanismInfo,
    C_InitToken,
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
/**
 * @brief Maps an opaque caller session handle into its internal state structure.
 */
P11SessionPtr_t prvSessionPointerFromHandle( CK_SESSION_HANDLE xSession )
{
    return ( P11SessionPtr_t ) xSession; /*lint !e923 Allow casting integer type to pointer for handle. */
}

/**
 * @brief Add an object that exists in NVM to the application object array.
 *
 * @param[in[ xPalHandle         The handle used by the PKCS #11 PAL for object.
 * @param[out] pxAppHandle       Updated to contain the application handle corresponding to xPalHandle.
 * @param[in]  pcLabel           Pointer to object label.
 * @param[in] xLabelLength       Length of the PKCS #11 label.
 *
 */
CK_RV prvAddObjectToList( CK_OBJECT_HANDLE xPalHandle,
                          CK_OBJECT_HANDLE_PTR pxAppHandle,
                          uint8_t * pcLabel,
                          size_t xLabelLength )
{
    CK_RV xResult = CKR_OK;
    BaseType_t xGotSemaphore;

    CK_BBOOL xObjectFound = CK_FALSE;
    int lInsertIndex = -1;
    int lSearchIndex = pkcs11configMAX_NUM_OBJECTS - 1;

    xGotSemaphore = xSemaphoreTake( xP11Context.xObjectList.xMutex, portMAX_DELAY );

    if( xGotSemaphore == pdTRUE )
    {
        for( lSearchIndex = pkcs11configMAX_NUM_OBJECTS - 1; lSearchIndex >= 0; lSearchIndex-- )
        {
            if( xP11Context.xObjectList.xObjects[ lSearchIndex ].xHandle == xPalHandle )
            {
                /* Object already exists in list. */
                xObjectFound = CK_TRUE;
                break;
            }
            else if( xP11Context.xObjectList.xObjects[ lSearchIndex ].xHandle == CK_INVALID_HANDLE )
            {
                lInsertIndex = lSearchIndex;
            }
        }

        if( xObjectFound == CK_FALSE )
        {
            if( lInsertIndex != -1 )
            {
                if( xLabelLength < pkcs11configMAX_LABEL_LENGTH )
                {
                    xP11Context.xObjectList.xObjects[ lInsertIndex ].xHandle = xPalHandle;
                    memcpy( xP11Context.xObjectList.xObjects[ lInsertIndex ].xLabel, pcLabel, xLabelLength );
                    *pxAppHandle = lInsertIndex + 1;
                }
                else
                {
                    xResult = CKR_DATA_LEN_RANGE;
                }
            }
            else
            {
                xResult = CKR_BUFFER_TOO_SMALL;
            }
        }

        xSemaphoreGive( xP11Context.xObjectList.xMutex );
    }
    else
    {
        xResult = CKR_CANT_LOCK;
    }

    return xResult;
}

/**
 * @brief Looks up a PKCS #11 object's label and PAL handle given an application handle.
 *
 * @param[in] xAppHandle         The handle of the object being lookedup for, used by the application.
 * @param[out] xPalHandle        Pointer to the handle corresponding to xPalHandle being used by the PAL.
 * @param[out] ppcLabel          Pointer to an array containing label.  NULL if object not found.
 * @param[out] pxLabelLength     Pointer to label length (includes a string null terminator).
 *                               0 if no object found.
 */
void prvFindObjectInListByHandle( CK_OBJECT_HANDLE xAppHandle,
                                  CK_OBJECT_HANDLE_PTR pxPalHandle,
                                  uint8_t ** ppcLabel,
                                  size_t * pxLabelLength )
{
    int lIndex = xAppHandle - 1;

    *ppcLabel = NULL;
    *pxLabelLength = 0;
    *pxPalHandle = CK_INVALID_HANDLE;

    if( lIndex < pkcs11configMAX_NUM_OBJECTS ) /* Check that handle is in bounds. */
    {
        if( xP11Context.xObjectList.xObjects[ lIndex ].xHandle != CK_INVALID_HANDLE )
        {
            *ppcLabel = xP11Context.xObjectList.xObjects[ lIndex ].xLabel;
            *pxLabelLength = strlen( ( const char * ) xP11Context.xObjectList.xObjects[ lIndex ].xLabel ) + 1;
            *pxPalHandle = xP11Context.xObjectList.xObjects[ lIndex ].xHandle;
        }
    }
}

/**
 * @brief Searches the PKCS #11 module's object list for label and provides handle.
 *
 * @param[in] pcLabel            Array containing label.
 * @param[in] xLableLength       Length of the label, in bytes.
 * @param[out] pxPalHandle       Pointer to the PAL handle to be provided.
 *                               CK_INVALID_HANDLE if no object found.
 * @param[out] pxAppHandle       Pointer to the application handle to be provided.
 *                               CK_INVALID_HANDLE if no object found.
 */
void prvFindObjectInListByLabel( uint8_t * pcLabel,
                                 size_t xLabelLength,
                                 CK_OBJECT_HANDLE_PTR pxPalHandle,
                                 CK_OBJECT_HANDLE_PTR pxAppHandle )
{
    uint8_t ucIndex;

    *pxPalHandle = CK_INVALID_HANDLE;
    *pxAppHandle = CK_INVALID_HANDLE;

    for( ucIndex = 0; ucIndex < pkcs11configMAX_NUM_OBJECTS; ucIndex++ )
    {
        if( 0 == memcmp( pcLabel, xP11Context.xObjectList.xObjects[ ucIndex ].xLabel, xLabelLength ) )
        {
            *pxPalHandle = xP11Context.xObjectList.xObjects[ ucIndex ].xHandle;
            *pxAppHandle = ucIndex + 1; /* Zero is not a valid handle, so let's offset by 1. */
            break;
        }
    }
}

/**
 * @brief This function is not implemented for this port.
 *
 * C_Login() is only implemented for compatibility with other ports.
 * All inputs to this function are ignored, and calling this
 * function on this port does not add any security.
 *
 * @return CKR_OK.
 */
CK_DECLARE_FUNCTION( CK_RV, C_Login )( CK_SESSION_HANDLE hSession,
                                       CK_USER_TYPE userType,
                                       CK_UTF8CHAR_PTR pPin,
                                       CK_ULONG ulPinLen )
{
    /* Avoid warnings about unused parameters. */
    ( void ) hSession;
    ( void ) userType;
    ( void ) pPin;
    ( void ) ulPinLen;

    return CKR_OK;
}

/* Helper function for parsing the templates of device certificates for
 * C_CreateObject. */
CK_RV prvCreateCertificate( CK_ATTRIBUTE_PTR pxTemplate,
                            CK_ULONG ulCount,
                            CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_RV xResult = CKR_OK;
    CK_BYTE_PTR pxCertificateValue = NULL;
    CK_ULONG xCertificateLength = 0;
    CK_ATTRIBUTE_PTR pxLabel = NULL;
    CK_ATTRIBUTE_PTR pxClass = NULL;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    CK_CERTIFICATE_TYPE xCertificateType = 0; /* = CKC_X_509; */
    uint32_t ulIndex = 0;
    CK_BBOOL xBool = CK_FALSE;
    CK_ATTRIBUTE xAttribute;

    /* Search for the pointer to the certificate VALUE. */
    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        switch( xAttribute.type )
        {
            case ( CKA_VALUE ):
                pxCertificateValue = xAttribute.pValue;
                xCertificateLength = xAttribute.ulValueLen;
                break;

            case ( CKA_LABEL ):

                if( xAttribute.ulValueLen <= pkcs11configMAX_LABEL_LENGTH )
                {
                    pxLabel = &pxTemplate[ ulIndex ];
                }
                else
                {
                    xResult = CKR_DATA_LEN_RANGE;
                }

                break;

            case ( CKA_CERTIFICATE_TYPE ):
                memcpy( &xCertificateType, xAttribute.pValue, sizeof( CK_CERTIFICATE_TYPE ) );

                if( xCertificateType != CKC_X_509 )
                {
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            case ( CKA_TOKEN ):
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Only token key object is supported. \r\n" ) );
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            case ( CKA_CLASS ):
                pxClass = &pxTemplate[ ulIndex ];
                break;

            case ( CKA_SUBJECT ):

                /* Do nothing.  This was already parsed out of the template previously. */
                break;

            default:
                xResult = CKR_TEMPLATE_INCONSISTENT;
                break;
        }
    }

    if( ( pxCertificateValue == NULL ) || ( pxLabel == NULL ) )
    {
        xResult = CKR_TEMPLATE_INCOMPLETE;
    }

    if( xResult == CKR_OK )
    {
        xPalHandle = PKCS11_PSA_SaveObject( pxClass, pxLabel, pxCertificateValue, xCertificateLength, NULL );
        if( xPalHandle == 0 ) /*Invalid handle. */
        {
            xResult = CKR_DEVICE_MEMORY;
        }
    }

    if( xResult == CKR_OK )
    {
        /* If adding the object to the list fails, an error will be returned. In this case, remain the
         * saved object in PSA as the object list as well as the PSA secure storage will be searched when
         * finding an object in C_findobjects.
         */
        xResult = prvAddObjectToList( xPalHandle, pxObject, pxLabel->pValue, pxLabel->ulValueLen );
    }

    return xResult;
}

#define PKCS11_INVALID_KEY_TYPE    ( ( CK_KEY_TYPE ) 0xFFFFFFFF )

/* Helper to search an attribute for the key type attribute. */
void prvGetKeyType( CK_KEY_TYPE * pxKeyType,
                    CK_ATTRIBUTE_PTR pxTemplate,
                    CK_ULONG ulCount )
{
    uint32_t ulIndex;
    CK_ATTRIBUTE xAttribute;

    *pxKeyType = PKCS11_INVALID_KEY_TYPE;

    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        if( xAttribute.type == CKA_KEY_TYPE )
        {
            memcpy( pxKeyType, xAttribute.pValue, sizeof( CK_KEY_TYPE ) );
            break;
        }
    }
}

/* Helper to search a template for the label attribute. */
void prvGetLabel( CK_ATTRIBUTE_PTR * ppxLabel,
                  CK_ATTRIBUTE_PTR pxTemplate,
                  CK_ULONG ulCount )
{
    CK_ATTRIBUTE xAttribute;
    uint32_t ulIndex;

    *ppxLabel = NULL;

    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        if( xAttribute.type == CKA_LABEL )
        {
            *ppxLabel = &pxTemplate[ ulIndex ];
            break;
        }
    }
}

/* Helper function for checking attribute templates of elliptic curve
 * private keys before import with C_CreateObject. */
CK_RV prvCreateEcPrivateKey( mbedtls_pk_context * pxMbedContext,
                             CK_ATTRIBUTE_PTR * ppxLabel,
                             CK_ATTRIBUTE_PTR * ppxClass,
                             CK_ATTRIBUTE_PTR pxTemplate,
                             CK_ULONG ulCount,
                             CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_RV xResult = CKR_OK;
    int lMbedReturn;
    CK_BBOOL xBool;
    uint32_t ulIndex;
    CK_ATTRIBUTE xAttribute;
    *ppxLabel = NULL;
    *ppxClass = NULL;

    /* Key will be assembled in the mbedTLS key context and then exported to DER for storage. */
    mbedtls_ecp_keypair * pxKeyPair = ( mbedtls_ecp_keypair * ) pxMbedContext->pk_ctx;

    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        switch( xAttribute.type )
        {
            case ( CKA_CLASS ):
                *ppxClass = &pxTemplate[ ulIndex ];
                break;

            case ( CKA_KEY_TYPE ):

                /* Do nothing.
                 * Key type and object type were checked previously. */
                break;

            case ( CKA_TOKEN ):
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Only token key creation is supported. \r\n" ) );
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            case ( CKA_SIGN ):
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Only keys with signing priveledges are supported. \r\n" ) );
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            case ( CKA_LABEL ):

                if( xAttribute.ulValueLen <= pkcs11configMAX_LABEL_LENGTH )
                {
                    *ppxLabel = &pxTemplate[ ulIndex ];
                }
                else
                {
                    xResult = CKR_DATA_LEN_RANGE;
                }

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

/* Helper function for parsing RSA Private Key attribute templates
 * for C_CreateObject. */
CK_RV prvCreateRsaPrivateKey( mbedtls_pk_context * pxMbedContext,
                              CK_ATTRIBUTE_PTR * ppxLabel,
                              CK_ATTRIBUTE_PTR * ppxClass,
                              CK_ATTRIBUTE_PTR pxTemplate,
                              CK_ULONG ulCount,
                              CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_RV xResult = CKR_OK;
    mbedtls_rsa_context * pxRsaContext;
    int lMbedReturn = 0;
    CK_BBOOL xBool;
    uint32_t ulIndex;
    CK_ATTRIBUTE xAttribute;

    *ppxLabel = NULL;
    *ppxClass = NULL;
    pxRsaContext = pxMbedContext->pk_ctx;
    mbedtls_rsa_init( pxRsaContext, MBEDTLS_RSA_PKCS_V15, 0 /*ignored.*/ );

    /* Parse template and collect the relevant parts. */
    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        switch( xAttribute.type )
        {
            case ( CKA_CLASS ):
                *ppxClass = &pxTemplate[ ulIndex ];
                break;

            case ( CKA_KEY_TYPE ):

                /* Do nothing.
                 * Key type & object type were checked previously.
                 */
                break;

            case ( CKA_TOKEN ):
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Only token key creation is supported. \r\n" ) );
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            case ( CKA_LABEL ):

                if( xAttribute.ulValueLen <= pkcs11configMAX_LABEL_LENGTH )
                {
                    *ppxLabel = &pxTemplate[ ulIndex ];
                }
                else
                {
                    xResult = CKR_DATA_LEN_RANGE;
                }

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
                lMbedReturn = mbedtls_rsa_import_raw( pxRsaContext,
                                                       xAttribute.pValue, xAttribute.ulValueLen, /* N */
                                                       NULL, 0,                                  /* P */
                                                       NULL, 0,                                  /* Q */
                                                       NULL, 0,                                  /* D */
                                                       NULL, 0 );                                /* E */
                break;

            case ( CKA_PUBLIC_EXPONENT ):
                lMbedReturn = mbedtls_rsa_import_raw( pxRsaContext,
                                                       NULL, 0,                                    /* N */
                                                       NULL, 0,                                    /* P */
                                                       NULL, 0,                                    /* Q */
                                                       NULL, 0,                                    /* D */
                                                       xAttribute.pValue, xAttribute.ulValueLen ); /* E */
                break;

            case ( CKA_PRIME_1 ):
                lMbedReturn = mbedtls_rsa_import_raw( pxRsaContext,
                                                       NULL, 0,                                  /* N */
                                                       xAttribute.pValue, xAttribute.ulValueLen, /* P */
                                                       NULL, 0,                                  /* Q */
                                                       NULL, 0,                                  /* D */
                                                       NULL, 0 );                                /* E */
                break;

            case ( CKA_PRIME_2 ):
                lMbedReturn = mbedtls_rsa_import_raw( pxRsaContext,
                                                       NULL, 0,                                  /* N */
                                                       NULL, 0,                                  /* P */
                                                       xAttribute.pValue, xAttribute.ulValueLen, /* Q */
                                                       NULL, 0,                                  /* D */
                                                       NULL, 0 );                                /* E */
                break;

            case ( CKA_PRIVATE_EXPONENT ):
                lMbedReturn = mbedtls_rsa_import_raw( pxRsaContext,
                                                       NULL, 0,                                  /* N */
                                                       NULL, 0,                                  /* P */
                                                       NULL, 0,                                  /* Q */
                                                       xAttribute.pValue, xAttribute.ulValueLen, /* D */
                                                       NULL, 0 );                                /* E */
                break;

            case ( CKA_EXPONENT_1 ):
                lMbedReturn = mbedtls_mpi_read_binary( &pxRsaContext->DP, xAttribute.pValue, xAttribute.ulValueLen );
                break;

            case ( CKA_EXPONENT_2 ):
                lMbedReturn = mbedtls_mpi_read_binary( &pxRsaContext->DQ, xAttribute.pValue, xAttribute.ulValueLen );
                break;

            case ( CKA_COEFFICIENT ):
                lMbedReturn = mbedtls_mpi_read_binary( &pxRsaContext->QP, xAttribute.pValue, xAttribute.ulValueLen );
                break;

            default:
                PKCS11_PRINT( ( "Unknown attribute found for RSA private key. %d \r\n", xAttribute.type ) );
                xResult = CKR_TEMPLATE_INCONSISTENT;
                break;
        }

        if( lMbedReturn != 0 )
        {
            xResult = CKR_ATTRIBUTE_VALUE_INVALID;
            break;
        }
    }

    return xResult;
}

/* Helper function for importing private keys using template
 * C_CreateObject. */
CK_RV prvCreatePrivateKey( CK_ATTRIBUTE_PTR pxTemplate,
                           CK_ULONG ulCount,
                           CK_OBJECT_HANDLE_PTR pxObject )
{
/*
 * RSAPrivateKey ::= SEQUENCE {
 * version           Version,
 * modulus           INTEGER,  -- n   (256 bytes for RSA-2048)
 * publicExponent    INTEGER,  -- e  (3 or 4 bytes for RSA-2048)
 * privateExponent   INTEGER,  -- d  (about 256 bytes for RSA-2048)
 * prime1            INTEGER,  -- p  (128 bytes for RSA-2048)
 * prime2            INTEGER,  -- q  (128 bytes for RSA-2048)
 * exponent1         INTEGER,  -- d mod (p-1)  (128 bytes for RSA-2048)
 * exponent2         INTEGER,  -- d mod (q-1)  (128 bytes for RSA-2048)
 * coefficient       INTEGER,  -- (inverse of q) mod p  (128 bytes for RSA-2048)
 * otherPrimeInfos   OtherPrimeInfos OPTIONAL
 * }
 * Besides, when encode the key, some ASN tags are also introduced.
 * For RSA-2048, the encoded length should be around 1232  bytes.
 */
#define MAX_PRIVATE_KEY_SIZE    1300
    mbedtls_pk_context xMbedContext;
    int lDerKeyLength = 0;
    CK_BYTE_PTR pxDerKey = NULL;
    CK_RV xResult = CKR_OK;
    CK_KEY_TYPE xKeyType;
    CK_ATTRIBUTE_PTR pxLabel = NULL;
    CK_ATTRIBUTE_PTR pxClass = NULL;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    mbedtls_rsa_context * pxRsaCtx = NULL;
    mbedtls_ecp_keypair * pxKeyPair = NULL;
    prvGetKeyType( &xKeyType, pxTemplate, ulCount );

    mbedtls_pk_init( &xMbedContext );
    if( xKeyType == CKK_RSA )
    {
        /* mbedtls_rsa_context must be malloc'ed to use with mbedtls_pk_free function. */
        pxRsaCtx = pvPortMalloc( sizeof( mbedtls_rsa_context ) );

        if( pxRsaCtx != NULL )
        {
            xMbedContext.pk_ctx = pxRsaCtx;
            xMbedContext.pk_info = &mbedtls_rsa_info;
            xResult = prvCreateRsaPrivateKey( &xMbedContext,
                                              &pxLabel,
                                              &pxClass,
                                              pxTemplate,
                                              ulCount,
                                              pxObject );
        }
        else
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    #if ( pkcs11configSUPPRESS_ECDSA_MECHANISM != 1 )
        else if( xKeyType == CKK_EC ) /* CKK_EC = CKK_ECDSA. */
        {
            /* Key will be assembled in the mbedTLS key context and then exported to DER for storage. */
            prvGetLabel( &pxLabel, pxTemplate, ulCount );

            /* An mbedTLS key is comprised of 2 pieces of data- an "info" and a "context".
                * Since a valid key was not found by prvGetExistingKeyComponent, we are going to initialize
                * the structure so that the mbedTLS structures will look the same as they would if a key
                * had been found, minus the public key component. */

            /* If a key had been found by prvGetExistingKeyComponent, the keypair context
                * would have been malloc'ed. */
            pxKeyPair = pvPortMalloc( sizeof( mbedtls_ecp_keypair ) );

            if( pxKeyPair != NULL )
            {
                /* Initialize the info. */
                xMbedContext.pk_info = &mbedtls_eckey_info;

                /* Initialize the context. */
                xMbedContext.pk_ctx = pxKeyPair;
                mbedtls_ecp_keypair_init( pxKeyPair );
                mbedtls_ecp_group_init( &pxKeyPair->grp );
                /*/ * At this time, only P-256 curves are supported. * / */
                mbedtls_ecp_group_load( &pxKeyPair->grp, MBEDTLS_ECP_DP_SECP256R1 );
            }
            else
            {
                xResult = CKR_HOST_MEMORY;
            }

            xResult = prvCreateEcPrivateKey( &xMbedContext,
                                             &pxLabel,
                                             &pxClass,
                                             pxTemplate,
                                             ulCount,
                                             pxObject );
        }
    #endif /* if ( pkcs11configSUPPRESS_ECDSA_MECHANISM != 1 ) */
    else
    {
        xResult = CKR_MECHANISM_INVALID;
    }

    /* Convert back to DER and save to memory. */
    if( xResult == CKR_OK )
    {
        pxDerKey = pvPortMalloc( MAX_PRIVATE_KEY_SIZE );

        if( pxDerKey == NULL )
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    if( xResult == CKR_OK )
    {
        lDerKeyLength = mbedtls_pk_write_key_der( &xMbedContext, pxDerKey, MAX_PRIVATE_KEY_SIZE );

        if( lDerKeyLength < 0 )
        {
            xResult = CKR_ATTRIBUTE_VALUE_INVALID;
        }

        /* Do not clean up the mbedTLS key context here as it will be used in psa_save_object. */
    }

    /* Save the object to device NVM. */
    if( xResult == CKR_OK )
    {
        xPalHandle = PKCS11_PSA_SaveObject( pxClass,
                                            pxLabel,
                                            pxDerKey + ( MAX_PRIVATE_KEY_SIZE - lDerKeyLength ),
                                            lDerKeyLength,
                                            &xMbedContext );

        if( xPalHandle == 0 )
        {
            xResult = CKR_DEVICE_MEMORY;
        }

        mbedtls_pk_free( &xMbedContext );

    }

    /* Store the PAL handle/label/application handle in lookup. */
    if( xResult == CKR_OK )
    {
        xResult = prvAddObjectToList( xPalHandle, pxObject, pxLabel->pValue, pxLabel->ulValueLen );
    }

    if( pxDerKey != NULL )
    {
        vPortFree( pxDerKey );
    }

    return xResult;
}

/* Helper function for importing elliptic curve public keys from
 * template using C_CreateObject. */
CK_RV prvCreateECPublicKey( mbedtls_pk_context * pxMbedContext,
                            CK_ATTRIBUTE_PTR * ppxLabel,
                            CK_ATTRIBUTE_PTR *ppxClass,
                            CK_ATTRIBUTE_PTR pxTemplate,
                            CK_ULONG ulCount,
                            CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_RV xResult = CKR_OK;
    int lMbedReturn;
    CK_BBOOL xBool;
    uint32_t ulIndex;
    CK_ATTRIBUTE xAttribute;
    *ppxLabel = NULL;
    *ppxClass = NULL;

    /* Key will be assembled in the mbedTLS key context and then exported to DER for storage. */
    mbedtls_ecp_keypair * pxKeyPair = ( mbedtls_ecp_keypair * ) pxMbedContext->pk_ctx;

    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        switch( xAttribute.type )
        {
            case ( CKA_CLASS ):
                *ppxClass = &pxTemplate[ ulIndex ];
            case ( CKA_KEY_TYPE ):

                /* Do nothing.
                 * Key type and class were checked previously. */
                break;

            case ( CKA_TOKEN ):
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Only token key creation is supported. \r\n" ) );
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            case ( CKA_LABEL ):

                if( xAttribute.ulValueLen < pkcs11configMAX_LABEL_LENGTH )
                {
                    *ppxLabel = &pxTemplate[ ulIndex ];
                }
                else
                {
                    xResult = CKR_DATA_LEN_RANGE;
                }

                break;

            case ( CKA_VERIFY ):
                memcpy( &xBool, xAttribute.pValue, xAttribute.ulValueLen );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "Only EC Public Keys with verify permissions supported. \r\n" ) );
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            case ( CKA_EC_PARAMS ):

                if( memcmp( ( CK_BYTE[] ) pkcs11DER_ENCODED_OID_P256, xAttribute.pValue, xAttribute.ulValueLen ) )
                {
                    PKCS11_PRINT( ( "ERROR: Only elliptic curve P-256 is supported.\r\n" ) );
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            case ( CKA_EC_POINT ):
                /* The first 2 bytes are for ASN1 type/length encoding. */
                lMbedReturn = mbedtls_ecp_point_read_binary( &pxKeyPair->grp, &pxKeyPair->Q, ( ( uint8_t * ) ( xAttribute.pValue ) + 2 ), ( xAttribute.ulValueLen - 2 ) );

                if( lMbedReturn != 0 )
                {
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            default:
                PKCS11_PRINT( ( "Unsupported attribute found for EC public key. %d \r\n", xAttribute.type ) );
                xResult = CKR_ATTRIBUTE_TYPE_INVALID;
                break;
        }
    }

    return xResult;
}

/* Helper function for importing public keys using
 * C_CreateObject. */
CK_RV prvCreatePublicKey( CK_ATTRIBUTE_PTR pxTemplate,
                          CK_ULONG ulCount,
                          CK_OBJECT_HANDLE_PTR pxObject )
{
/*
 * RSAPublicKey ::= SEQUENCE {
 *  modulus           INTEGER,  -- n   (256 bytes for RSA-2048)
 *  publicExponent    INTEGER   -- e (3 or 4 bytes for RSA-2048)
 * }

 * Besides, when encoding the key, some ASN tags are also introduced.
 */
#define MAX_PUBLIC_KEY_SIZE    310
    mbedtls_pk_context xMbedContext;
    int lDerKeyLength = 0;
    CK_BYTE_PTR pxDerKey = NULL;
    CK_KEY_TYPE xKeyType;
    CK_RV xResult = CKR_OK;
    CK_ATTRIBUTE_PTR pxLabel = NULL;
    CK_ATTRIBUTE_PTR pxClass = NULL;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    mbedtls_pk_init( &xMbedContext );
    mbedtls_ecp_keypair * pxKeyPair;

    prvGetKeyType( &xKeyType, pxTemplate, ulCount );
    if( xKeyType == CKK_RSA )
    {
        xResult = CKR_ATTRIBUTE_TYPE_INVALID;
    }

    #if ( pkcs11configSUPPRESS_ECDSA_MECHANISM != 1 )
        else if( xKeyType == CKK_EC ) /* CKK_EC = CKK_ECDSA. */
        {
            prvGetLabel( &pxLabel, pxTemplate, ulCount );

            /* An mbedTLS key is comprised of 2 pieces of data- an "info" and a "context".
                * Since a valid key was not found by prvGetExistingKeyComponent, we are going to initialize
                * the structure so that the mbedTLS structures will look the same as they would if a key
                * had been found, minus the private key component. */

            /* If a key had been found by prvGetExistingKeyComponent, the keypair context
                * would have been malloc'ed. */
            pxKeyPair = pvPortMalloc( sizeof( mbedtls_ecp_keypair ) );

            if( pxKeyPair != NULL )
            {
                /* Initialize the info. */
                xMbedContext.pk_info = &mbedtls_eckey_info;

                /* Initialize the context. */
                xMbedContext.pk_ctx = pxKeyPair;
                mbedtls_ecp_keypair_init( pxKeyPair );
                mbedtls_ecp_group_init( &pxKeyPair->grp );
                /*/ * At this time, only P-256 curves are supported. * / */
                mbedtls_ecp_group_load( &pxKeyPair->grp, MBEDTLS_ECP_DP_SECP256R1 );
            }
            else
            {
                xResult = CKR_HOST_MEMORY;
            }

            xResult = prvCreateECPublicKey( &xMbedContext, &pxLabel, &pxClass, pxTemplate, ulCount, pxObject );
        }
    #endif /* if ( pkcs11configSUPPRESS_ECDSA_MECHANISM != 1 ) */
    else
    {
        PKCS11_PRINT( ( "Invalid key type %d \r\n", xKeyType ) );
        xResult = CKR_MECHANISM_INVALID;
    }

    if( xResult == CKR_OK )
    {
        /* Store the key.*/
        pxDerKey = pvPortMalloc( MAX_PUBLIC_KEY_SIZE );

        if( pxDerKey == NULL )
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    if( xResult == CKR_OK )
    {
        lDerKeyLength = mbedtls_pk_write_pubkey_der( &xMbedContext, pxDerKey, MAX_PUBLIC_KEY_SIZE );

        /* Do not clean up the mbedTLS key context here as it will be used in psa_save_object. */
    }

    if( xResult == CKR_OK )
    {
        xPalHandle = PKCS11_PSA_SaveObject( pxClass,
                                            pxLabel,
                                            pxDerKey + ( MAX_PUBLIC_KEY_SIZE - lDerKeyLength ),
                                            lDerKeyLength,
                                            &xMbedContext );

        if( xPalHandle == CK_INVALID_HANDLE )
        {
            xResult = CKR_DEVICE_MEMORY;
        }

        mbedtls_pk_free( &xMbedContext );
    }

    if( xResult == CKR_OK )
    {
        xResult = prvAddObjectToList( xPalHandle, pxObject, pxLabel->pValue, pxLabel->ulValueLen );
    }

    if( pxDerKey != NULL )
    {
        vPortFree( pxDerKey );
    }

    return xResult;
}

CK_RV prvMbedTLS_Initialize( void )
{
    CK_RV xResult = CKR_OK;
    SemaphoreHandle_t xMutex = NULL;
#if pkcs11configVENDOR_DEVICE_CERTIFICATE_SUPPORTED
    psa_status_t status;
    psa_key_handle_t key_handle;
    struct psa_storage_info_t ps_info;
#endif

    if( xP11Context.xIsInitialized == CK_TRUE )
    {
        xResult = CKR_CRYPTOKI_ALREADY_INITIALIZED;
    }

    if( xResult == CKR_OK )
    {
        memset( &xP11Context, 0, sizeof( xP11Context ) );
        xMutex = xSemaphoreCreateMutex();
        if( xMutex != NULL )
        {
            xP11Context.xObjectList.xMutex = xMutex;
        }
        else
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    if( xResult == CKR_OK )
    {
        CRYPTO_Init();

#if pkcs11configVENDOR_DEVICE_CERTIFICATE_SUPPORTED
        status = psa_open_key(pkcs11configVENDOR_DEVICE_KEY_ID, &key_handle);
        if (status == PSA_SUCCESS) {
            P11KeyConfig.uxDevicePrivateKey = key_handle;
            P11KeyConfig.ulDevicePrivateKeyMark = pkcs11OBJECT_PRESENT_MAGIC + sizeof(key_handle);
        }
        else 
        {
            xResult = CKR_ARGUMENTS_BAD;
            PKCS11_PRINT( ( "ERROR: Failed to open vendor key(%x) err=0x%x. \r\n", pkcs11configVENDOR_DEVICE_KEY_ID, status ) );
        }

        if( xResult == CKR_OK )
        {
            /* Retrieve device certificate info */
            status = psa_ps_get_info( P11KeyConfig.uxDeviceCertificate, &ps_info);
            if (status == PSA_SUCCESS) {
                P11KeyConfig.ulDeviceCertificateMark = pkcs11OBJECT_PRESENT_MAGIC | ps_info.size;

                /* PSA Crypto library should haven been initialised successfully in secure world. */
                xP11Context.xIsInitialized = CK_TRUE;
            }
            else
            {
               xResult = CKR_ARGUMENTS_BAD;
               PKCS11_PRINT( ( "ERROR: Failed to retrieve device certificate information  err=0x%x. \r\n",  status ) );
            }
        }

        if( xResult == CKR_OK )
        {
            if (GetAwsIoTJITPStatus() == 0)
            {
                PKCS11_PRINT( ( "INFO: Get JITP certificate info.\r\n" ) );
                /* Retrieve JITP certificate info */
                status = psa_ps_get_info( P11KeyConfig.uxJitpCertificate, &ps_info);
                if (status == PSA_SUCCESS) {
                    P11KeyConfig.ulJitpCertificateMark = pkcs11OBJECT_PRESENT_MAGIC | ps_info.size;
                }
                else
                {
                    /* Optional, Not treat as error if doesn't exist */
                    if (status != PSA_ERROR_INVALID_HANDLE) {
                        xResult = CKR_ARGUMENTS_BAD;
                    }
                    PKCS11_PRINT( ( "WARN: Failed to retrieve JITP certificate information err=0x%x. \r\n",  status ) );
                }
            }
        }
#else
        /* PSA Crypto library should haven been initialised successfully in secure world. */
        xP11Context.xIsInitialized = CK_TRUE;
#endif /* pkcs11configVENDOR_DEVICE_CERTIFICATE_SUPPORTED */
    }
    return xResult;
}

/**
 * @brief Initialize the PKCS #11 module for use.
 *
 * @note C_Initialize is not thread-safe.
 *
 * C_Initialize should be called (and allowed to return) before
 * any additional PKCS #11 operations are invoked.
 *
 * In this implementation, all arguments are ignored.
 * Thread protection for the rest of PKCS #11 functions
 * default to FreeRTOS primitives.
 *
 * @param[in] pvInitArgs    This parameter is ignored.
 *
 * @return CKR_OK if successful.
 * CKR_CRYPTOKI_ALREADY_INITIALIZED if C_Initialize was previously called.
 * All other errors indicate that the PKCS #11 module is not ready to be used.
 * See <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
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
    else
    {
        if( xP11Context.xIsInitialized == CK_FALSE )
        {
            xResult = CKR_CRYPTOKI_NOT_INITIALIZED;
        }
        else
        {
            vSemaphoreDelete( xP11Context.xObjectList.xMutex );
            memset( &xP11Context, 0, sizeof( xP11Context ) );
        }
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

    /* Since the PSA implementation of PKCS#11 does not depend
     * on a physical token, this parameter is ignored. */
    ( void ) ( xTokenPresent );

    if( PKCS11_MODULE_IS_INITIALIZED != CK_TRUE )
    {
        xResult = CKR_CRYPTOKI_NOT_INITIALIZED;
    }

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
 * @brief This function is not implemented for this port.
 *
 * C_GetTokenInfo() is only implemented for compatibility with other ports.
 * All inputs to this function are ignored, and calling this
 * function on this port does provide any information about
 * the PKCS #11 token.
 *
 * @return CKR_OK.
 */
CK_DECLARE_FUNCTION( CK_RV, C_GetTokenInfo )( CK_SLOT_ID slotID,
                                              CK_TOKEN_INFO_PTR pInfo )
{
    /* Avoid compiler warnings about unused variables. */
    ( void ) slotID;
    ( void ) pInfo;

    return CKR_OK;
}

/**
 * @brief This function obtains information about a particular
 * mechanism possibly supported by a token.
 *
 *  \param[in]  xSlotID         This parameter is unused in this port.
 *  \param[in]  type            The cryptographic capability for which support
 *                              information is being queried.
 *  \param[out] pInfo           Algorithm sizes and flags for the requested
 *                              mechanism, if supported.
 *
 * @return CKR_OK if the mechanism is supported. Otherwise, CKR_MECHANISM_INVALID.
 */
CK_DECLARE_FUNCTION( CK_RV, C_GetMechanismInfo )( CK_SLOT_ID slotID,
                                                  CK_MECHANISM_TYPE type,
                                                  CK_MECHANISM_INFO_PTR pInfo )
{
    CK_RV xResult = CKR_MECHANISM_INVALID;
    struct CryptoMechanisms
    {
        CK_MECHANISM_TYPE xType;
        CK_MECHANISM_INFO xInfo;
    }
    pxSupportedMechanisms[] =
    {
        { CKM_RSA_PKCS,        { 2048, 2048, CKF_SIGN              } },
        { CKM_RSA_X_509,       { 2048, 2048, CKF_VERIFY            } },
        #if ( pkcs11configSUPPRESS_ECDSA_MECHANISM != 1 )
            { CKM_ECDSA,           { 256,  256,  CKF_SIGN | CKF_VERIFY } },
            { CKM_EC_KEY_PAIR_GEN, { 256,  256,  CKF_GENERATE_KEY_PAIR } },
        #endif
        { CKM_SHA256,          { 0,    0,    CKF_DIGEST            } }
    };
    uint32_t ulMech = 0;

    /* Look for the requested mechanism in the above table. */
    for( ; ulMech < sizeof( pxSupportedMechanisms ) / sizeof( pxSupportedMechanisms[ 0 ] ); ulMech++ )
    {
        if( pxSupportedMechanisms[ ulMech ].xType == type )
        {
            /* The mechanism is supported. Copy out the details and break
             * out of the loop. */
            memcpy( pInfo, &( pxSupportedMechanisms[ ulMech ].xInfo ), sizeof( CK_MECHANISM_INFO ) );
            xResult = CKR_OK;
            break;
        }
    }

    return xResult;
}

/**
 * @brief This function is not implemented for this port.
 *
 * C_InitToken() is only implemented for compatibility with other ports.
 * All inputs to this function are ignored, and calling this
 * function on this port does not add any security.
 *
 * @return CKR_OK.
 */
CK_DECLARE_FUNCTION( CK_RV, C_InitToken )( CK_SLOT_ID slotID,
                                           CK_UTF8CHAR_PTR pPin,
                                           CK_ULONG ulPinLen,
                                           CK_UTF8CHAR_PTR pLabel )
{
    /* Avoid compiler warnings about unused variables. */
    ( void ) slotID;
    ( void ) pPin;
    ( void ) ulPinLen;
    ( void ) pLabel;

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
    CK_BBOOL xSessionMemAllocated = CK_FALSE;
    CK_BBOOL xSignMutexCreated = CK_FALSE;
    CK_BBOOL xVerifyMutexCreated = CK_FALSE;

    ( void ) ( xSlotID );
    ( void ) ( pvApplication );
    ( void ) ( xNotify );

    /* Check that the PKCS #11 module is initialized. */
    if( PKCS11_MODULE_IS_INITIALIZED != CK_TRUE )
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
        else
        {
            xSessionMemAllocated = CK_TRUE;
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
        else
        {
            xSignMutexCreated = CK_TRUE;
        }

        pxSessionObj->xVerifyMutex = xSemaphoreCreateMutex();

        if( NULL == pxSessionObj->xVerifyMutex )
        {
            xResult = CKR_HOST_MEMORY;
        }
        else
        {
            xVerifyMutexCreated = CK_TRUE;
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

    if( CKR_OK != xResult )
    {
        if( xSessionMemAllocated == CK_TRUE )
        {
            if( xSignMutexCreated == CK_TRUE )
            {
                vSemaphoreDelete( pxSessionObj->xSignMutex );
            }

            if( xVerifyMutexCreated == CK_TRUE )
            {
                vSemaphoreDelete( pxSessionObj->xVerifyMutex );
            }

            vPortFree( pxSessionObj );
        }
    }

    return xResult;
}

/**
 * @brief Terminate a session and release resources.
 */
CK_DEFINE_FUNCTION( CK_RV, C_CloseSession )( CK_SESSION_HANDLE xSession )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( xResult == CKR_OK )
    {
        /*
         * Tear down the session.
         */
        pxSession->uxSignKey = PSA_NULL_HANDLE;
        if( NULL != pxSession->xSignMutex )
        {
            vSemaphoreDelete( pxSession->xSignMutex );
        }
        pxSession->uxVerifyKey = PSA_NULL_HANDLE;
        if( NULL != pxSession->xVerifyMutex )
        {
            vSemaphoreDelete( pxSession->xVerifyMutex );
        }
        if( pxSession->pxHashOperationHandle != NULL )
        {
            vPortFree( pxSession->pxHashOperationHandle );
        }
        vPortFree( pxSession );
    }
    else
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }

    return xResult;
}

/* Searches a template for the CKA_CLASS attribute. */
CK_RV prvGetObjectClass( CK_ATTRIBUTE_PTR pxTemplate,
                         CK_ULONG ulCount,
                         CK_OBJECT_CLASS * pxClass )
{
    CK_RV xResult = CKR_TEMPLATE_INCOMPLETE;
    uint32_t ulIndex = 0;

    /* Search template for class attribute. */
    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        CK_ATTRIBUTE xAttribute = pxTemplate[ ulIndex ];

        if( xAttribute.type == CKA_CLASS )
        {
            memcpy( pxClass, xAttribute.pValue, sizeof( CK_OBJECT_CLASS ) );
            xResult = CKR_OK;
            break;
        }
    }

    return xResult;
}

/**
 * @brief Create a PKCS #11 certificate, public key, or private key object
 * by importing it into device storage.
 *
 * @param[in] xSession                   Handle of a valid PKCS #11 session.
 * @param[in] pxTemplate                 List of attributes of the object to
 *                                       be created.
 * @param[in] ulCount                    Number of attributes in pxTemplate.
 * @param[out] pxObject                  Pointer to the location where the created
 *                                       object's handle will be placed.
 *
 * <table>
 * <tr><th> Object Type             <th> Template Attributes
 * <tr><td rowspan="6">Certificate<td>CKA_CLASS
 * <tr>                           <td>CKA_VALUE
 * <tr>                              <td>CKA_TOKEN
 * <tr>                              <td>CKA_LABEL
 * <tr>                              <td>CKA_CERTIFICATE_TYPE
 * <tr>                              <td>CKA_VALUE
 * <tr><td rowspan="7">EC Private Key<td>CKA_CLASS
 * <tr>                              <td>CKA_KEY_TYPE
 * <tr>                              <td>CKA_TOKEN
 * <tr>                              <td>CKA_LABEL
 * <tr>                              <td>CKA_SIGN
 * <tr>                              <td>CKA_EC_PARAMS
 * <tr>                              <td>CKA_VALUE
 * <tr><td rowspan="7">EC Public Key<td>CKA_CLASS
 * <tr>                              <td>CKA_KEY_TYPE
 * <tr>                              <td>CKA_TOKEN
 * <tr>                              <td>CKA_VERIFY
 * <tr>                              <td>CKA_LABEL
 * <tr>                              <td>CKA_EC_PARAMS
 * <tr>                              <td>CKA_EC_POINT
 * <tr><td rowspan="13">RSA Private Key<td>CKA_CLASS
 * <tr>                               <td>CKA_KEY_TYPE
 * <tr>                               <td>CKA_TOKEN
 * <tr>                               <td>CKA_LABEL
 * <tr>                               <td>CKA_SIGN
 * <tr>                               <td>CKA_MODULUS
 * <tr>                               <td>CKA_PUBLIC_EXPONENT
 * <tr>                               <td>CKA_PRIME_1
 * <tr>                               <td>CKA_PRIME_2
 * <tr>                               <td>CKA_PRIVATE_EXPONENT
 * <tr>                               <td>CKA_EXPONENT_1
 * <tr>                               <td>CKA_EXPONENT_2
 * <tr>                               <td>CKA_COEFFICIENT
 * </table>
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DECLARE_FUNCTION( CK_RV, C_CreateObject )( CK_SESSION_HANDLE xSession,
                                              CK_ATTRIBUTE_PTR pxTemplate,
                                              CK_ULONG ulCount,
                                              CK_OBJECT_HANDLE_PTR pxObject )
{ /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    CK_OBJECT_CLASS xClass;

    if( ( NULL == pxTemplate ) ||
        ( NULL == pxObject ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( xResult == CKR_OK )
    {
        xResult = prvGetObjectClass( pxTemplate, ulCount, &xClass );
    }

    if( xResult == CKR_OK )
    {
        switch( xClass )
        {
            case CKO_CERTIFICATE:
                xResult = prvCreateCertificate( pxTemplate, ulCount, pxObject );
                break;

            case CKO_PRIVATE_KEY:
                xResult = prvCreatePrivateKey( pxTemplate, ulCount, pxObject );
                break;

            case CKO_PUBLIC_KEY:
                xResult = prvCreatePublicKey( pxTemplate, ulCount, pxObject );
                break;

            default:
                xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                break;
        }
    }

    return xResult;
}

/**
 * @brief Removes an object from the module object list (xP11Context.xObjectList)
 *
 * \warn This does not delete the object from NVM.
 *
 * @param[in] xAppHandle     Application handle of the object to be deleted.
 *
 */
CK_RV prvDeleteObjectFromList( CK_OBJECT_HANDLE xAppHandle )
{
    CK_RV xResult = CKR_OK;
    BaseType_t xGotSemaphore = pdFALSE;
    int lIndex = xAppHandle - 1;

    if( lIndex >= pkcs11configMAX_NUM_OBJECTS )
    {
        xResult = CKR_OBJECT_HANDLE_INVALID;
    }

    if( xResult == CKR_OK )
    {
        xGotSemaphore = xSemaphoreTake( xP11Context.xObjectList.xMutex, portMAX_DELAY );
    }

    if( ( xGotSemaphore == pdTRUE ) && ( xResult == CKR_OK ) )
    {
        if( xP11Context.xObjectList.xObjects[ lIndex ].xHandle != CK_INVALID_HANDLE )
        {
            memset( &xP11Context.xObjectList.xObjects[ lIndex ], 0, sizeof( P11Object_t ) );
        }
        else
        {
            xResult = CKR_OBJECT_HANDLE_INVALID;
        }

        xSemaphoreGive( xP11Context.xObjectList.xMutex );
    }
    else
    {
        xResult = CKR_CANT_LOCK;
    }

    return xResult;
}

/**
 * @brief Free resources attached to an object handle.
 */
CK_DEFINE_FUNCTION( CK_RV, C_DestroyObject )( CK_SESSION_HANDLE xSession,
                                              CK_OBJECT_HANDLE xObject )
{
    psa_status_t uxStatus = 0;
    uint8_t * pcLabel = NULL;
    size_t xLabelLength = 0;
    CK_OBJECT_HANDLE xPalHandle;

    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );

    if( xResult == CKR_OK )
    {
        prvFindObjectInListByHandle( xObject, &xPalHandle, &pcLabel, &xLabelLength );
        if( xPalHandle != CK_INVALID_HANDLE )
        {
            if( memcmp( pcLabel, pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, xLabelLength ) == 0 )
            {
                if( ( P11KeyConfig.ulDevicePrivateKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
                {
                    uxStatus = psa_destroy_key( P11KeyConfig.uxDevicePrivateKey );
                    if( uxStatus != PSA_SUCCESS )
                    {
                        xResult = CKR_FUNCTION_FAILED;
                    }
                    else
                    {
                        P11KeyConfig.ulDevicePrivateKeyMark = 0;
                        xResult = prvDeleteObjectFromList( xObject );
                    }
                }
            }
            else if( memcmp( pcLabel, pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, xLabelLength ) == 0 )
            {
                if( ( P11KeyConfig.ulDevicePublicKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
                {
                    uxStatus = psa_destroy_key( P11KeyConfig.uxDevicePublicKey );
                    if( uxStatus != PSA_SUCCESS )
                    {
                        xResult = CKR_FUNCTION_FAILED;
                    }
                    else
                    {
                        P11KeyConfig.ulDevicePublicKeyMark = 0;
                        xResult = prvDeleteObjectFromList( xObject );
                    }
                }
            }
            else if( memcmp( pcLabel, pkcs11configLABEL_CODE_VERIFICATION_KEY, xLabelLength ) == 0 )
            {
                if( ( P11KeyConfig.ulCodeVerifyKeyMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
                {
                    uxStatus = psa_destroy_key( P11KeyConfig.uxCodeVerifyKey );
                    if( uxStatus != PSA_SUCCESS )
                    {
                        xResult = CKR_FUNCTION_FAILED;
                    }
                    else
                    {
                        P11KeyConfig.ulCodeVerifyKeyMark = 0;
                        xResult = prvDeleteObjectFromList( xObject );
                    }
                }
            }
            else if( memcmp( pcLabel, pkcs11configLABEL_ROOT_CERTIFICATE, xLabelLength ) == 0 )
            {
                if( ( P11KeyConfig.ulRootCertificateMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
                {
                    uxStatus = psa_ps_remove( P11KeyConfig.uxRootCertificate );
                    if( uxStatus != PSA_SUCCESS )
                    {
                        xResult = CKR_FUNCTION_FAILED;
                    }
                    else
                    {
                        P11KeyConfig.ulRootCertificateMark = 0;
                        xResult = prvDeleteObjectFromList( xObject );
                    }
                }
            }
            else if( memcmp( pcLabel, pkcs11configLABEL_JITP_CERTIFICATE, xLabelLength ) == 0 )
            {
                if( ( P11KeyConfig.ulJitpCertificateMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
                {
                    uxStatus = psa_ps_remove( P11KeyConfig.uxJitpCertificate );
                    if( uxStatus != PSA_SUCCESS )
                    {
                        xResult = CKR_FUNCTION_FAILED;
                    }
                    else
                    {
                        P11KeyConfig.ulJitpCertificateMark = 0;
                        xResult = prvDeleteObjectFromList( xObject );
                    }
                }
            }
            else if( memcmp( pcLabel, pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, xLabelLength ) == 0 )
            {
                if( ( P11KeyConfig.ulDeviceCertificateMark & pkcs11OBJECT_PRESENT_MASK ) == pkcs11OBJECT_PRESENT_MAGIC )
                {
                    uxStatus = psa_ps_remove( P11KeyConfig.uxDeviceCertificate );
                    if( uxStatus != PSA_SUCCESS )
                    {
                        xResult = CKR_FUNCTION_FAILED;
                    }
                    else
                    {
                        P11KeyConfig.ulDeviceCertificateMark = 0;
                        xResult = prvDeleteObjectFromList( xObject );
                    }
                }
            }
            else
            {
                xResult = CKR_ATTRIBUTE_VALUE_INVALID;
            }
        }
    }

    return xResult;
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
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    CK_BBOOL xIsPrivate = CK_TRUE;
    CK_ULONG iAttrib;
    mbedtls_pk_context xKeyContext = { 0 };
    CK_KEY_TYPE xPkcsKeyType = ( CK_KEY_TYPE ) ~0;
    CK_OBJECT_CLASS xClass;
    uint8_t * pxObjectValue = NULL;
    uint8_t ucP256Oid[] = pkcs11DER_ENCODED_OID_P256;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    size_t xSize;
    uint8_t * pcLabel = NULL;
    psa_status_t uxStatus;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_ecp_point ecp_point;
    uint8_t ucGetData[ pkcs11OBJECT_MAX_SIZE ];
    size_t ulLength = pkcs11OBJECT_MAX_SIZE;

    if( xResult == CKR_OK )
    {
        if( ( NULL == pxTemplate ) || ( 0 == ulCount ) )
        {
            xResult = CKR_ARGUMENTS_BAD;
        }
    }
    if( xResult == CKR_OK )
    {
        /*
         * Find the object in P11context firstly.
         */
        prvFindObjectInListByHandle( xObject, &xPalHandle, &pcLabel, &xSize ); /*pcLabel and xSize are ignored. */

        if( xPalHandle != CK_INVALID_HANDLE )
        {
            xResult = PKCS11_PSA_GetObjectValue( xPalHandle, ucGetData, &ulLength, &xIsPrivate );
            if( xResult == CKR_OK )
            {
                pxObjectValue = ucGetData;
            }
        }
        else
        {
            xResult = CKR_OBJECT_HANDLE_INVALID;
        }
    }

    if( xResult == CKR_OK )
    {
        for( iAttrib = 0; iAttrib < ulCount && CKR_OK == xResult; iAttrib++ )
        {
            switch( pxTemplate[ iAttrib ].type )
            {
                case CKA_CLASS:

                    if( pxTemplate[ iAttrib ].pValue == NULL )
                    {
                        pxTemplate[ iAttrib ].ulValueLen = sizeof( CK_OBJECT_CLASS );
                    }
                    else
                    {
                        if( pxTemplate[ iAttrib ].ulValueLen >= sizeof( CK_OBJECT_CLASS ) )
                        {
                            switch (( P11ObjectHandles_t )xPalHandle )
                            {
                                case eAwsDevicePrivateKey:
                                    xClass = CKO_PRIVATE_KEY;
                                    break;

                                case eAwsDevicePublicKey:
                                case eAwsCodeSigningKey:
                                    xClass = CKO_PUBLIC_KEY;
                                    break;

                                case eAwsJitpCertificate:
                                case eAwsRootCertificate:
                                case eAwsDeviceCertificate:
                                    xClass = CKO_CERTIFICATE;
                                    break;

                                default:
                                    xResult = CKR_OBJECT_HANDLE_INVALID;
                                    xClass = CKO_PUBLIC_KEY;
                                    break;
                            }
                            memcpy( pxTemplate[ iAttrib ].pValue, &xClass, sizeof( CK_OBJECT_CLASS ) );
                        }
                        else
                        {
                            xResult = CKR_BUFFER_TOO_SMALL;
                        }
                    }

                    break;

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
                        pxTemplate[ iAttrib ].ulValueLen = sizeof( CK_KEY_TYPE );
                    }
                    else if( pxTemplate[ iAttrib ].ulValueLen < sizeof( CK_KEY_TYPE ) )
                    {
                        xResult = CKR_BUFFER_TOO_SMALL;
                    }
                    else
                    {
                        if( 0 != xResult )
                        {
                            xResult = CKR_FUNCTION_FAILED;
                        }
                        else
                        {
                            psa_key_handle_t ulKeyHandle = 0;
                            if( xPalHandle == eAwsDevicePrivateKey )
                            {
                                ulKeyHandle = P11KeyConfig.uxDevicePrivateKey;
                            }
                            else if( xPalHandle == eAwsDevicePublicKey )
                            {
                                ulKeyHandle = P11KeyConfig.uxDevicePublicKey;
                            }
                            else if( xPalHandle == eAwsCodeSigningKey )
                            {
                                ulKeyHandle = P11KeyConfig.uxCodeVerifyKey;
                            }
                            else
                            {
                                xResult = CKR_OBJECT_HANDLE_INVALID;
                            }

                            /* Get the key policy from which the key type can be derived. */
                            if( xResult == CKR_OK )
                            {
                                uxStatus = psa_get_key_attributes( ulKeyHandle, &attributes );
                                if ( uxStatus != PSA_SUCCESS )
                                {
                                    xResult = CKR_FUNCTION_FAILED;
                                }
                            }
                            if( xResult == CKR_OK )
                            {
                                psa_algorithm_t alg = psa_get_key_algorithm(&attributes);
                                if( PSA_ALG_IS_ECDSA( alg ) )
                                {
                                    xPkcsKeyType = CKK_EC;
                                }
                                else if( PSA_ALG_IS_RSA_PKCS1V15_SIGN( alg ) ||
                                        PSA_ALG_IS_RSA_PSS( alg ) )
                                {
                                    xPkcsKeyType = CKK_RSA;
                                }
                                else
                                {
                                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                                }
                                psa_reset_key_attributes( &attributes );
                            }
                            if( xResult == CKR_OK )
                            {
                                memcpy( pxTemplate[ iAttrib ].pValue, &xPkcsKeyType, sizeof( CK_KEY_TYPE ) );
                            }
                        }
                    }

                    break;

                case CKA_PRIVATE_EXPONENT:

                    xResult = CKR_ATTRIBUTE_SENSITIVE;

                    break;

                case CKA_EC_PARAMS:

                    /* TODO: Add check that is key, is ec key. */

                    pxTemplate[ iAttrib ].ulValueLen = sizeof( ucP256Oid );

                    if( pxTemplate[ iAttrib ].pValue != NULL )
                    {
                        if( pxTemplate[ iAttrib ].ulValueLen < sizeof( ucP256Oid ) )
                        {
                            xResult = CKR_BUFFER_TOO_SMALL;
                        }
                        else
                        {
                            memcpy( pxTemplate[ iAttrib ].pValue, ucP256Oid, sizeof( ucP256Oid ) );
                        }
                    }

                    break;

                case CKA_EC_POINT:

                    if( pxTemplate[ iAttrib ].pValue == NULL )
                    {
                        pxTemplate[ iAttrib ].ulValueLen = 67;
                    }
                    else
                    {
                        if( pxTemplate[ iAttrib ].ulValueLen < sizeof( ecp_point ) )
                        {
                            xResult = CKR_BUFFER_TOO_SMALL;
                        }
                        else
                        {
                            pxTemplate[ iAttrib ].ulValueLen = ulLength + 2;
                            *(( CK_BYTE *)(pxTemplate[ iAttrib ].pValue)) = 0x04;
                            *(( CK_BYTE *)(pxTemplate[ iAttrib ].pValue) + 1) = ulLength;
                            memcpy(( CK_BYTE *)(pxTemplate[ iAttrib ].pValue) + 2, pxObjectValue, ulLength );
                        }
                    }

                    break;

                default:
                    xResult = CKR_ATTRIBUTE_TYPE_INVALID;
            }
        }

        /* Free the buffer where object was stored. */
        PKCS11_PSA_GetObjectValueCleanup( pxObjectValue, ulLength );

        /* Free the mbedTLS structure used to parse the key. */
        mbedtls_pk_free( &xKeyContext );
    }

    return xResult;
}

/**
 * @brief Initializes a search for an object by its label.
 *
 * \sa C_FindObjects() and C_FindObjectsFinal() which must be called
 * after C_FindObjectsInit().
 *
 * \note FindObjects parameters are shared by a session.  Calling
 * C_FindObjectsInit(), C_FindObjects(), and C_FindObjectsFinal() with the
 * same session across different tasks may lead to unexpected results.
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pxTemplate                    Pointer to a template which specifies
 *                                          the object attributes to match.
 *                                          In this port, the only searchable attribute
 *                                          is object label.  All other attributes will
 *                                          be ignored.
 * @param[in] ulCount                       The number of attributes in pxTemplate.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DECLARE_FUNCTION( CK_RV, C_FindObjectsInit )( CK_SESSION_HANDLE xSession,
                                                 CK_ATTRIBUTE_PTR pxTemplate,
                                                 CK_ULONG ulCount )
{
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );
    CK_BYTE * pxFindObjectLabel = NULL;
    uint32_t ulIndex;
    CK_ATTRIBUTE xAttribute;

    if( NULL == pxTemplate )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( ( ulCount != 1 ) && ( ulCount != 2 ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
        PKCS11_PRINT( ( "ERROR: Find objects does not support searching by %d attributes. \r\n", ulCount ) );
    }

    if( xResult == CKR_OK )
    {
        if( pxSession->pxFindObjectLabel != NULL )
        {
            xResult = CKR_OPERATION_ACTIVE;
            PKCS11_PRINT( ( "ERROR: Find object operation already in progress. \r\n" ) );
        }
    }

    /* Malloc space to save template information. */
    if( xResult == CKR_OK )
    {
        pxFindObjectLabel = pvPortMalloc( pxTemplate->ulValueLen + 1 ); /* Add 1 to guarantee null termination for PAL. */

        if( pxFindObjectLabel != NULL )
        {
            pxSession->pxFindObjectLabel = pxFindObjectLabel;
            memset( pxFindObjectLabel, 0, pxTemplate->ulValueLen + 1 );
        }
        else
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    /* Search template for label.
     * NOTE: This port only supports looking up objects by CKA_LABEL and all
     * other search attributes are ignored. */
    if( xResult == CKR_OK )
    {
        xResult = CKR_TEMPLATE_INCOMPLETE;

        for( ulIndex = 0; ulIndex < ulCount; ulIndex++ ) /* TODO: Re-evaluate the need for this for loop... we are making bad assumptions if 2 objects have the same label anyhow! */
        {
            xAttribute = pxTemplate[ ulIndex ];

            if( xAttribute.type == CKA_LABEL )
            {
                memcpy( pxSession->pxFindObjectLabel, xAttribute.pValue, xAttribute.ulValueLen );
                xResult = CKR_OK;
            }
            else
            {
                PKCS11_WARNING_PRINT( ( "WARNING: Search parameters other than label are ignored.\r\n" ) );
            }
        }
    }

    /* Clean up memory if there was an error parsing the template. */
    if( xResult != CKR_OK )
    {
        if( pxFindObjectLabel != NULL )
        {
            vPortFree( pxFindObjectLabel );
            pxSession->pxFindObjectLabel = NULL;
        }
    }

    return xResult;
}

/**
 * @brief Find an object.
 *
 * \sa C_FindObjectsInit() which must be called before calling C_FindObjects()
 * and C_FindObjectsFinal(), which must be called after.
 *
 * \note FindObjects parameters are shared by a session.  Calling
 * C_FindObjectsInit(), C_FindObjects(), and C_FindObjectsFinal() with the
 * same session across different tasks may lead to unexpected results.
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[out] pxObject                     Points to the handle of the object to
 *                                          be found.
 * @param[in] ulMaxObjectCount              The size of the pxObject object handle
 *                                          array. In this port, this value should
 *                                          always be set to 1, as searching for
 *                                          multiple objects is not supported.
 * @param[out] pulObjectCount               The actual number of objects that are
 *                                          found. In this port, if an object is found
 *                                          this value will be 1, otherwise if the
 *                                          object is not found, it will be set to 0.
 *
 * \note In the event that an object does not exist, CKR_OK will be returned, but
 * pulObjectCount will be set to 0.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DECLARE_FUNCTION( CK_RV, C_FindObjects )( CK_SESSION_HANDLE xSession,
                                             CK_OBJECT_HANDLE_PTR pxObject,
                                             CK_ULONG ulMaxObjectCount,
                                             CK_ULONG_PTR pulObjectCount )
{ /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );

    BaseType_t xDone = pdFALSE;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;

    /*
     * Check parameters.
     */
    if( ( NULL == pxObject ) ||
        ( NULL == pulObjectCount ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
        xDone = pdTRUE;
    }

    if( xResult == CKR_OK )
    {
        if( pxSession->pxFindObjectLabel == NULL )
        {
            xResult = CKR_OPERATION_NOT_INITIALIZED;
            xDone = pdTRUE;
        }

        if( 0u == ulMaxObjectCount )
        {
            xResult = CKR_ARGUMENTS_BAD;
            xDone = pdTRUE;
        }

        if( 1u != ulMaxObjectCount )
        {
            PKCS11_WARNING_PRINT( ( "WARN: Searching for more than 1 object not supported. \r\n" ) );
        }

        if( ( pdFALSE == xDone ) && ( ( CK_BBOOL ) CK_TRUE == pxSession->xFindObjectComplete ) )
        {
            *pulObjectCount = 0;
            xResult = CKR_OK;
            xDone = pdTRUE;
        }
    }

    if( ( pdFALSE == xDone ) )
    {
        /* Try to find the object in module's list first. */
        prvFindObjectInListByLabel( pxSession->pxFindObjectLabel, strlen( ( const char * ) pxSession->pxFindObjectLabel ), &xPalHandle, pxObject );

        /* Check with the PAL if the object was previously stored. */
        if( *pxObject != CK_INVALID_HANDLE )
        {
            *pulObjectCount = 1;
        }
        else
        {

            /* Check with the PSA if the object was previously stored. */
            xPalHandle = PKCS11_PSA_FindObject( pxSession->pxFindObjectLabel, ( uint8_t ) strlen( ( const char * ) pxSession->pxFindObjectLabel ) );
            if( xPalHandle != CK_INVALID_HANDLE )
            {
                xResult = prvAddObjectToList( xPalHandle, pxObject, pxSession->pxFindObjectLabel, strlen( ( const char * ) pxSession->pxFindObjectLabel ) );
                *pulObjectCount = 1;
            }
            else
            {
                /* According to the PKCS #11 standard, not finding an object results in a CKR_OK return value with an object count of 0. */
                *pulObjectCount = 0;
                PKCS11_WARNING_PRINT( ( "WARN: Object with label '%s' not found. \r\n", ( char * ) pxSession->pxFindObjectLabel ) );
            }
        }
    }

    return xResult;
}

/**
 * @brief Completes an object search operation.
 *
 * \sa C_FindObjectsInit(), C_FindObjects() which must be called before
 * calling C_FindObjectsFinal().
 *
 * \note FindObjects parameters are shared by a session.  Calling
 * C_FindObjectsInit(), C_FindObjects(), and C_FindObjectsFinal() with the
 * same session across different tasks may lead to unexpected results.
 *
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DECLARE_FUNCTION( CK_RV, C_FindObjectsFinal )( CK_SESSION_HANDLE xSession )
{ /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );

    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    /*
     * Check parameters.
     */
    if( xResult == CKR_OK )
    {
        if( pxSession->pxFindObjectLabel == NULL )
        {
            xResult = CKR_OPERATION_NOT_INITIALIZED;
        }
    }

    if( xResult == CKR_OK )
    {
        /*
         * Clean-up find objects state.
         */
        vPortFree( pxSession->pxFindObjectLabel );
        pxSession->pxFindObjectLabel = NULL;
    }

    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_DigestInit )( CK_SESSION_HANDLE xSession,
                                           CK_MECHANISM_PTR pMechanism )
{
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    psa_hash_operation_t* pxHandle = NULL;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );
    psa_status_t uxStatus;

    if( xResult == CKR_OK )
    {
        if( pMechanism == NULL )
        {
            xResult = CKR_ARGUMENTS_BAD;
        }
    }

    if(( xResult == CKR_OK ) && ( pMechanism->mechanism != CKM_SHA256 ))
    {
        xResult = CKR_MECHANISM_INVALID;
    }

    if( xResult == CKR_OK )
    {
        pxHandle = ( psa_hash_operation_t * )pvPortMalloc( sizeof( psa_hash_operation_t ) );
        if( pxHandle == NULL )
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    /*
     * Initialize the requested hash type
     */
    if ( xResult == CKR_OK )
    {
        *pxHandle = psa_hash_operation_init();

        /*
         * Setup the hash object for the desired hash.
         * Currently only the SHA_256 algorithm is supported.
         */
        uxStatus = psa_hash_setup( pxHandle, PSA_ALG_SHA_256 );
        if ( uxStatus != PSA_SUCCESS )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
        else
        {
            pxSession->xOperationInProgress = pMechanism->mechanism;
            pxSession->pxHashOperationHandle = pxHandle;
        }
    }

    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_DigestUpdate )( CK_SESSION_HANDLE xSession,
                                             CK_BYTE_PTR pPart,
                                             CK_ULONG ulPartLen )
{
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );
    psa_status_t uxStatus;

    if( xResult == CKR_OK )
    {
        if( pPart == NULL )
        {
            PKCS11_PRINT( ( "ERROR: Null digest data provided. \r\n" ) );
            xResult = CKR_ARGUMENTS_BAD;
        }
    }

    if( xResult == CKR_OK )
    {
        if( ( pxSession->xOperationInProgress != CKM_SHA256 ) ||
            ( pxSession->pxHashOperationHandle == NULL ) )
        {
            xResult = CKR_OPERATION_NOT_INITIALIZED;
        }
    }

    if( xResult == CKR_OK )
    {
        uxStatus = psa_hash_update( pxSession->pxHashOperationHandle, pPart, ulPartLen );
        if( uxStatus != PSA_SUCCESS )
        {
            uxStatus = psa_hash_abort( pxSession->pxHashOperationHandle );
            if( uxStatus != PSA_SUCCESS )
            {
                PKCS11_PRINT( ( "ERROR: psa_hash_abort error. \r\n" ) );
            }
            vPortFree( pxSession->pxHashOperationHandle );
            pxSession->pxHashOperationHandle = NULL;
            pxSession->xOperationInProgress = pkcs11NO_OPERATION;
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    return xResult;
}

CK_DEFINE_FUNCTION( CK_RV, C_DigestFinal )( CK_SESSION_HANDLE xSession,
                                            CK_BYTE_PTR pDigest,
                                            CK_ULONG_PTR pulDigestLen )
{
    psa_status_t uxStatus;
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( xResult == CKR_OK )
    {
        if( pulDigestLen == NULL )
        {
            xResult = CKR_ARGUMENTS_BAD;
        }
    }

    if( xResult == CKR_OK )
    {
        if( pxSession->xOperationInProgress != CKM_SHA256 )
        {
            xResult = CKR_OPERATION_NOT_INITIALIZED;
            pxSession->xOperationInProgress = pkcs11NO_OPERATION;
        }
    }

    if( xResult == CKR_OK )
    {
        if( pDigest == NULL )
        {
            /* Supply the required buffer size. */
            *pulDigestLen = pkcs11SHA256_DIGEST_LENGTH;
        }
        else
        {
            if( *pulDigestLen < pkcs11SHA256_DIGEST_LENGTH )
            {
                xResult = CKR_BUFFER_TOO_SMALL;
            }
            else
            {
                uxStatus = psa_hash_finish( pxSession->pxHashOperationHandle, pDigest, *pulDigestLen, ( size_t * )pulDigestLen );
                if( uxStatus != PSA_SUCCESS )
                {
                    xResult = CKR_FUNCTION_FAILED;
                }
                pxSession->xOperationInProgress = pkcs11NO_OPERATION;
                vPortFree( pxSession->pxHashOperationHandle );
                pxSession->pxHashOperationHandle = NULL;
            }
        }
    }

    return xResult;
}

/**
 * @brief Begin creating a digital signature.
 *
 * \sa C_Sign() completes signatures initiated by C_SignInit().
 *
 * \note C_Sign() parameters are shared by a session.  Calling
 * C_SignInit() & C_Sign() with the same session across different
 * tasks may lead to unexpected results.
 *
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pxMechanism                   Mechanism used to sign.
 *                                          This port supports the following mechanisms:
 *                                          - CKM_RSA_PKCS for RSA signatures
 *                                          - CKM_ECDSA for elliptic curve signatures
 *                                          Note that neither of these mechanisms perform
 *                                          hash operations.
 * @param[in] xKey                          The handle of the private key to be used for
 *                                          signature. Key must be compatible with the
 *                                          mechanism chosen by pxMechanism.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_SignInit )( CK_SESSION_HANDLE xSession,
                                         CK_MECHANISM_PTR pxMechanism,
                                         CK_OBJECT_HANDLE xKey )
{
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    CK_OBJECT_HANDLE xPalHandle;
    uint8_t * pxLabel = NULL;
    size_t xLabelLength = 0;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t uxStatus;

    /*lint !e9072 It's OK to have different parameter name. */
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( xResult == CKR_OK )
    {
        if( NULL == pxMechanism )
        {
            xResult = CKR_ARGUMENTS_BAD;
        }
    }

    /* Retrieve key value from storage. */
    if( xResult == CKR_OK )
    {
        prvFindObjectInListByHandle( xKey,
                                     &xPalHandle,
                                     &pxLabel,
                                     &xLabelLength );
    }

    if( xResult == CKR_OK )
    {
        if( xPalHandle == CK_INVALID_HANDLE )
        {
            xResult = CKR_KEY_HANDLE_INVALID;
        }
    }

    if( xResult == CKR_OK )
    {
        if( pdTRUE == xSemaphoreTake( pxSession->xSignMutex, portMAX_DELAY ) )
        {

            /*
             * Only the device private key and code sign key can be used
             * to make a signature.
             */
            if ( xPalHandle == eAwsDevicePrivateKey )
            {
                pxSession->uxSignKey = P11KeyConfig.uxDevicePrivateKey;

                /* Get the key's algorithm by getting the key attributes. */
                uxStatus = psa_get_key_attributes( P11KeyConfig.uxDevicePrivateKey, &attributes );
                if ( uxStatus != PSA_SUCCESS )
                {
                    xResult = CKR_FUNCTION_FAILED;
                }
                else
                {
                    pxSession->xSignAlgorithm = psa_get_key_algorithm( &attributes );
                    psa_reset_key_attributes(&attributes);
                }
            }
            else
            {
                pxSession->uxSignKey = PSA_NULL_HANDLE;
                xResult = CKR_KEY_HANDLE_INVALID;
            }
            xSemaphoreGive( pxSession->xSignMutex );
        }
        else
        {
            xResult = CKR_CANT_LOCK;
        }
    }

    /* Check that the mechanism and key type are compatible, supported. */
    if( xResult == CKR_OK )
    {
        if( pxMechanism->mechanism == CKM_RSA_PKCS )
        {
            if( ( !( PSA_ALG_IS_RSA_PKCS1V15_SIGN( pxSession->xSignAlgorithm ) ) ) &&
                ( !( PSA_ALG_IS_RSA_PSS( pxSession->xSignAlgorithm ) ) ) )
            {
                PKCS11_PRINT( ( "ERROR: Signing key type (%d) does not match RSA mechanism \r\n", pxSession->xSignAlgorithm ) );
                xResult = CKR_KEY_TYPE_INCONSISTENT;
            }
        }
        else if( pxMechanism->mechanism == CKM_ECDSA )
        {
            if( !PSA_ALG_IS_ECDSA( pxSession->xSignAlgorithm ) )
            {
                PKCS11_PRINT( ( "ERROR: Signing key type (%d) does not match ECDSA mechanism \r\n", pxSession->xSignAlgorithm ) );
                xResult = CKR_KEY_TYPE_INCONSISTENT;
            }
        }
        else
        {
            PKCS11_PRINT( ( "ERROR: Unsupported mechanism type %d \r\n", pxMechanism->mechanism ) );
            xResult = CKR_MECHANISM_INVALID;
        }
    }

    if( xResult == CKR_OK )
    {
        pxSession->xSignMechanism = pxMechanism->mechanism;
    }

    return xResult;
}

/**
 * @brief Performs a digital signature operation.
 *
 * \sa C_SignInit() initiates signatures signature creation.
 *
 * \note C_Sign() parameters are shared by a session.  Calling
 * C_SignInit() & C_Sign() with the same session across different
 * tasks may lead to unexpected results.
 *
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pucData                       Data to be signed.
 *                                          Note: Some applications may require this data to
 *                                          be hashed before passing to C_Sign().
 * @param[in] ulDataLen                     Length of pucData, in bytes.
 * @param[out] pucSignature                 Buffer where signature will be placed.
 *                                          Caller is responsible for allocating memory.
 *                                          Providing NULL for this input will cause
 *                                          pulSignatureLen to be updated for length of
 *                                          buffer required.
 * @param[in,out] pulSignatureLen           Length of pucSignature buffer.
 *                                          If pucSignature is non-NULL, pulSignatureLen is
 *                                          updated to contain the actual signature length.
 *                                          If pucSignature is NULL, pulSignatureLen is
 *                                          updated to the buffer length required for signature
 *                                          data.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Sign )( CK_SESSION_HANDLE xSession,
                                     CK_BYTE_PTR pucData,
                                     CK_ULONG ulDataLen,
                                     CK_BYTE_PTR pucSignature,
                                     CK_ULONG_PTR pulSignatureLen )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    P11SessionPtr_t pxSessionObj = prvSessionPointerFromHandle( xSession );
    CK_ULONG xSignatureLength = 0;
    CK_ULONG xExpectedInputLength = 0;
    psa_status_t uxStatus;
    CK_BYTE_PTR pucDataToBeSigned = pucData;
    CK_LONG ulDataLengthToBeSigned = ulDataLen;

    if( CKR_OK == xResult )
    {
        if( ( NULL == pulSignatureLen ) || ( NULL == pucData ) )
        {
            xResult = CKR_ARGUMENTS_BAD;
        }
    }

    if( CKR_OK == xResult )
    {
        /* Update the signature length. */

        if( pxSessionObj->xSignMechanism == CKM_RSA_PKCS )
        {
            xSignatureLength = pkcs11RSA_2048_SIGNATURE_LENGTH;
            xExpectedInputLength = pkcs11RSA_SIGNATURE_INPUT_LENGTH;

            /* Remove the padding of SHA256 Algorithm Identifier Sequence. */
            pucDataToBeSigned = pucData + pkcs11DER_ENCODED_OID_P256_LEGNTH;
            ulDataLengthToBeSigned = ulDataLengthToBeSigned - pkcs11DER_ENCODED_OID_P256_LEGNTH;

        }
        else if( pxSessionObj->xSignMechanism == CKM_ECDSA )
        {
            xSignatureLength = pkcs11ECDSA_P256_SIGNATURE_LENGTH;
            xExpectedInputLength = pkcs11SHA256_DIGEST_LENGTH;
        }
        else
        {
            xResult = CKR_OPERATION_NOT_INITIALIZED;
        }
    }

    if( xResult == CKR_OK )
    {
        /* Calling application is trying to determine length needed for signature buffer. */
        if( NULL != pucSignature )
        {
            /* Check that the signature buffer is long enough. */
            if( *pulSignatureLen < xSignatureLength )
            {
                xResult = CKR_BUFFER_TOO_SMALL;
            }

            /* Check that input data to be signed is the expected length. */
            if( CKR_OK == xResult )
            {
                if( xExpectedInputLength != ulDataLen )
                {
                    xResult = CKR_DATA_LEN_RANGE;
                }
            }

            /* Sign the data.*/
            if( CKR_OK == xResult )
            {
                if( pdTRUE == xSemaphoreTake( pxSessionObj->xSignMutex, portMAX_DELAY ) )
                {
                    if ( pxSessionObj->uxSignKey == PSA_NULL_HANDLE )
                    {
                        xResult = CKR_KEY_HANDLE_INVALID;
                    }
                    else
                    {
                        uxStatus = psa_sign_hash( pxSessionObj->uxSignKey,
                                                  pxSessionObj->xSignAlgorithm,
                                                  ( const uint8_t * )pucDataToBeSigned,
                                                  ( size_t )ulDataLengthToBeSigned,
                                                  ( uint8_t * )pucSignature,
                                                  ( size_t )*pulSignatureLen,
                                                  ( size_t * )pulSignatureLen );

                        if( uxStatus != PSA_SUCCESS )
                        {
                            PKCS11_PRINT( ( "mbedTLS sign failed with error %d \r\n", uxStatus ) );
                            xResult = CKR_FUNCTION_FAILED;
                        }
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

    /* Complete the operation in the context. */
    if( xResult != CKR_BUFFER_TOO_SMALL )
    {
        pxSessionObj->xSignMechanism = pkcs11NO_OPERATION;
    }

    return xResult;
}

/**
 * @brief Begin a digital signature verification.
 *
 * \sa C_Verify() completes verifications initiated by C_VerifyInit().
 *
 * \note C_Verify() parameters are shared by a session.  Calling
 * C_VerifyInit() & C_Verify() with the same session across different
 * tasks may lead to unexpected results.
 *
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pxMechanism                   Mechanism used to verify signature.
 *                                          This port supports the following mechanisms:
 *                                          - CKM_RSA_X_509 for RSA verifications
 *                                          - CKM_ECDSA for elliptic curve verifications
 * @param[in] xKey                          The handle of the public key to be used for
 *                                          verification. Key must be compatible with the
 *                                          mechanism chosen by pxMechanism.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_VerifyInit )( CK_SESSION_HANDLE xSession,
                                           CK_MECHANISM_PTR pxMechanism,
                                           CK_OBJECT_HANDLE xKey )
{
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    P11SessionPtr_t pxSession;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    uint8_t * pxLabel = NULL;
    size_t xLabelLength = 0;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t uxStatus;

    pxSession = prvSessionPointerFromHandle( xSession );

    if( xResult == CKR_OK )
    {
        if( NULL == pxMechanism )
        {
            PKCS11_PRINT( ( "ERROR: Null verification mechanism provided. \r\n" ) );
            xResult = CKR_ARGUMENTS_BAD;
        }
    }

    /* Retrieve key value from storage. */
    if( xResult == CKR_OK )
    {
        prvFindObjectInListByHandle( xKey,
                                     &xPalHandle,
                                     &pxLabel,
                                     &xLabelLength );

        if( xPalHandle == CK_INVALID_HANDLE )
        {
            pxSession->uxSignKey = PSA_NULL_HANDLE;
            xResult = CKR_KEY_HANDLE_INVALID;
        }
    }

    if( xResult == CKR_OK )
    {
        if( pdTRUE == xSemaphoreTake( pxSession->xVerifyMutex, portMAX_DELAY ) )
        {
            if ( xPalHandle == eAwsDevicePublicKey )
            {
                pxSession->uxVerifyKey = P11KeyConfig.uxDevicePublicKey;
                uxStatus = psa_get_key_attributes( P11KeyConfig.uxDevicePublicKey, &attributes );
                if ( uxStatus != PSA_SUCCESS )
                {
                    xResult = CKR_FUNCTION_FAILED;
                }
                else
                {
                    pxSession->xVerifyAlgorithm = psa_get_key_algorithm( &attributes );
                    psa_reset_key_attributes( &attributes );
                }
            }
            else if( xPalHandle == eAwsCodeSigningKey )
            {
                pxSession->uxVerifyKey = P11KeyConfig.uxCodeVerifyKey;
                uxStatus = psa_get_key_attributes( P11KeyConfig.uxCodeVerifyKey, &attributes );
                if ( uxStatus != PSA_SUCCESS )
                {
                    xResult = CKR_FUNCTION_FAILED;
                }
                else
                {
                    pxSession->xVerifyAlgorithm = psa_get_key_algorithm( &attributes );
                    psa_reset_key_attributes( &attributes );
                }
            }
            else
            {
                pxSession->uxVerifyKey = PSA_NULL_HANDLE;
                xResult = CKR_KEY_HANDLE_INVALID;
            }

            xSemaphoreGive( pxSession->xVerifyMutex );
        }
        else
        {
            xResult = CKR_CANT_LOCK;
        }
    }

    /* Check that the mechanism and key type are compatible, supported. */
    if( xResult == CKR_OK )
    {
        if( pxMechanism->mechanism == CKM_RSA_X_509 )
        {
            if( ( !( PSA_ALG_IS_RSA_PKCS1V15_SIGN( pxSession->xSignAlgorithm ) ) ) &&
                ( !( PSA_ALG_IS_RSA_PSS( pxSession->xSignAlgorithm ) ) ) )
            {
                PKCS11_PRINT( ( "ERROR: Verification key type (%d) does not match RSA mechanism \r\n", pxSession->xSignAlgorithm ) );
                xResult = CKR_KEY_TYPE_INCONSISTENT;
            }
        }
        else if( pxMechanism->mechanism == CKM_ECDSA )
        {
            if( !PSA_ALG_IS_ECDSA( pxSession->xSignAlgorithm ) )
            {
                PKCS11_PRINT( ( "ERROR: Verification key type (%d) does not match ECDSA mechanism \r\n", pxSession->xSignAlgorithm ) );
                xResult = CKR_KEY_TYPE_INCONSISTENT;
            }
        }
        else
        {
            PKCS11_PRINT( ( "ERROR: Unsupported mechanism type %d \r\n", pxMechanism->mechanism ) );
            xResult = CKR_MECHANISM_INVALID;
        }
    }

    if( xResult == CKR_OK )
    {
        pxSession->xVerifyMechanism = pxMechanism->mechanism;
    }

    return xResult;
}

/**
 * @brief Verifies a digital signature.
 *
 * \note C_VerifyInit() must have been called previously.
 *
 * \note C_Verify() parameters are shared by a session.  Calling
 * C_VerifyInit() & C_Verify() with the same session across different
 * tasks may lead to unexpected results.
 *
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pucData                       Data who's signature is to be verified.
 *                                          Note: In this implementation, this is generally
 *                                          expected to be the hash of the data.
 * @param[in] ulDataLen                     Length of pucData.
 * @param[in] pucSignature                  The signature to be verified.
 * @param[in] ulSignatureLength             Length of pucSignature in bytes.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Verify )( CK_SESSION_HANDLE xSession,
                                       CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataLen,
                                       CK_BYTE_PTR pucSignature,
                                       CK_ULONG ulSignatureLen )
{
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    P11SessionPtr_t pxSessionObj;
    psa_status_t uxStatus;

    pxSessionObj = prvSessionPointerFromHandle( xSession ); /*lint !e9072 It's OK to have different parameter name. */

    if( xResult == CKR_OK )
    {
        /* Check parameters. */
        if( ( NULL == pucData ) ||
            ( NULL == pucSignature ) )
        {
            xResult = CKR_ARGUMENTS_BAD;
        }
    }

    /* Check that the signature and data are the expected length.
     * These PKCS #11 mechanism expect data to be pre-hashed/formatted. */
    if( xResult == CKR_OK )
    {
        if( pxSessionObj->xVerifyMechanism == CKM_RSA_X_509 )
        {
            if( ulDataLen != pkcs11RSA_2048_SIGNATURE_LENGTH )
            {
                xResult = CKR_DATA_LEN_RANGE;
            }

            if( ulSignatureLen != pkcs11RSA_2048_SIGNATURE_LENGTH )
            {
                xResult = CKR_SIGNATURE_LEN_RANGE;
            }
        }
        else if( pxSessionObj->xVerifyMechanism == CKM_ECDSA )
        {
            if( ulDataLen != pkcs11SHA256_DIGEST_LENGTH )
            {
                xResult = CKR_DATA_LEN_RANGE;
            }

            if( ulSignatureLen != pkcs11ECDSA_P256_SIGNATURE_LENGTH )
            {
                xResult = CKR_SIGNATURE_LEN_RANGE;
            }
        }
        else
        {
            xResult = CKR_OPERATION_NOT_INITIALIZED;
        }
    }

    if( xResult == CKR_OK )
    {
        if( pdTRUE == xSemaphoreTake( pxSessionObj->xVerifyMutex, portMAX_DELAY ) )
        {
            /* Verify the signature. If a public key is present, use it. */
            if ( pxSessionObj->uxVerifyKey == PSA_NULL_HANDLE )
            {
                xResult = CKR_KEY_HANDLE_INVALID;
            }
            else
            {
                uxStatus = psa_verify_hash( pxSessionObj->uxVerifyKey,
                                            pxSessionObj->xVerifyAlgorithm,
                                            ( const uint8_t * )pucData,
                                            ( size_t )ulDataLen,
                                            ( const uint8_t * )pucSignature,
                                            ( size_t )ulSignatureLen );

                if( uxStatus == PSA_SUCCESS )
                {
                    xResult = CKR_OK;
                }

                /*
                    * In PSA_ERROR_BUFFER_TOO_SMALL and PSA_ERROR_INVALID_SIGNATURE case,
                    * return invalid signature.
                    */
                else if( ( uxStatus == PSA_ERROR_INVALID_SIGNATURE ) ||
                         ( uxStatus == PSA_ERROR_BUFFER_TOO_SMALL ) )
                {
                    xResult = CKR_SIGNATURE_INVALID;
                }
                else
                {
                    xResult = CKR_FUNCTION_FAILED;
                }
            }
            xSemaphoreGive( pxSessionObj->xVerifyMutex );
        }
        else
        {
            xResult = CKR_CANT_LOCK;
        }
    }

    /* Return the signature verification result. */
    return xResult;
}

/* Checks that the private key template provided for C_GenerateKeyPair
 * contains all necessary attributes, and does not contain any invalid
 * attributes. */
CK_RV prvCheckGenerateKeyPairPrivateTemplate( CK_ATTRIBUTE_PTR * ppxLabel,
                                              CK_ATTRIBUTE_PTR pxTemplate,
                                              CK_ULONG ulTemplateLength )
{
    CK_ATTRIBUTE xAttribute;
    CK_RV xResult = CKR_OK;
    CK_BBOOL xBool;
    CK_ULONG xTemp;
    CK_ULONG xIndex;
    uint32_t xAttributeMap = 0;
    uint32_t xRequiredAttributeMap = ( LABEL_IN_TEMPLATE | PRIVATE_IN_TEMPLATE | SIGN_IN_TEMPLATE );

    for( xIndex = 0; xIndex < ulTemplateLength; xIndex++ )
    {
        xAttribute = pxTemplate[ xIndex ];

        switch( xAttribute.type )
        {
            case ( CKA_LABEL ):
                *ppxLabel = &pxTemplate[ xIndex ];
                xAttributeMap |= LABEL_IN_TEMPLATE;
                break;

            case ( CKA_TOKEN ):
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Only token key generation is supported. \r\n" ) );
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            case ( CKA_KEY_TYPE ):
                memcpy( &xTemp, xAttribute.pValue, sizeof( CK_ULONG ) );

                if( xTemp != CKK_EC )
                {
                    PKCS11_PRINT( ( "ERROR: Only EC key pair generation is supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
                }

                break;

            case ( CKA_PRIVATE ):
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Generating private keys that are not marked private is not supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
                }

                xAttributeMap |= PRIVATE_IN_TEMPLATE;
                break;

            case ( CKA_SIGN ):
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Generating private keys that cannot sign is not supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
                }

                xAttributeMap |= SIGN_IN_TEMPLATE;
                break;

            default:
                xResult = CKR_TEMPLATE_INCONSISTENT;
                break;
        }
    }

    if( ( xAttributeMap & xRequiredAttributeMap ) != xRequiredAttributeMap )
    {
        xResult = CKR_TEMPLATE_INCOMPLETE;
    }

    return xResult;
}

/* Checks that the public key template provided for C_GenerateKeyPair
 * contains all necessary attributes, and does not contain any invalid
 * attributes. */
CK_RV prvCheckGenerateKeyPairPublicTemplate( CK_ATTRIBUTE_PTR * ppxLabel,
                                             CK_ATTRIBUTE_PTR pxTemplate,
                                             CK_ULONG ulTemplateLength )
{
    CK_ATTRIBUTE xAttribute;
    CK_RV xResult = CKR_OK;
    CK_BBOOL xBool;
    CK_KEY_TYPE xKeyType;
    CK_BYTE xEcParams[] = pkcs11DER_ENCODED_OID_P256;
    int lCompare;
    CK_ULONG ulIndex;
    uint32_t xAttributeMap = 0;
    uint32_t xRequiredAttributeMap = ( LABEL_IN_TEMPLATE | EC_PARAMS_IN_TEMPLATE | VERIFY_IN_TEMPLATE );

    for( ulIndex = 0; ulIndex < ulTemplateLength; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        switch( xAttribute.type )
        {
            case ( CKA_LABEL ):

                *ppxLabel = &pxTemplate[ ulIndex ];
                xAttributeMap |= LABEL_IN_TEMPLATE;
                break;

            case ( CKA_KEY_TYPE ):
                memcpy( &xKeyType, xAttribute.pValue, sizeof( CK_KEY_TYPE ) );

                if( xKeyType != CKK_EC )
                {
                    PKCS11_PRINT( ( "ERROR: Only EC key pair generation is supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
                }

                break;

            case ( CKA_EC_PARAMS ):
                lCompare = memcmp( xEcParams, xAttribute.pValue, sizeof( xEcParams ) );

                if( lCompare != 0 )
                {
                    PKCS11_PRINT( ( "ERROR: Only P-256 key generation is supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
                }

                xAttributeMap |= EC_PARAMS_IN_TEMPLATE;
                break;

            case ( CKA_VERIFY ):
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Generating public keys that cannot verify is not supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
                }

                xAttributeMap |= VERIFY_IN_TEMPLATE;
                break;

            case ( CKA_TOKEN ):
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Only token key generation is supported. \r\n" ) );
                    xResult = CKR_ATTRIBUTE_VALUE_INVALID;
                }

                break;

            default:
                xResult = CKR_TEMPLATE_INCONSISTENT;
                break;
        }
    }

    if( ( xAttributeMap & xRequiredAttributeMap ) != xRequiredAttributeMap )
    {
        xResult = CKR_TEMPLATE_INCOMPLETE;
    }

    return xResult;
}

/**
 * @brief Generate a new public-private key pair.
 *
 * This port only supports generating elliptic curve P-256
 * key pairs.
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pxMechanism                   Pointer to a mechanism. At this time,
 *                                          CKM_EC_KEY_PAIR_GEN is the only supported mechanism.
 * @param[in] pxPublicKeyTemplate           Pointer to a list of attributes that the generated
 *                                          public key should possess.
 *                                          Public key template must have the following attributes:
 *                                          - CKA_LABEL
 *                                              - Label should be no longer than #pkcs11configMAX_LABEL_LENGTH
 *                                              and must be supported by port's PKCS #11 PAL.
 *                                          - CKA_EC_PARAMS
 *                                              - Must equal pkcs11DER_ENCODED_OID_P256.
 *                                              Only P-256 keys are supported.
 *                                          - CKA_VERIFY
 *                                              - Must be set to true.  Only public keys used
 *                                              for verification are supported.
 *                                          Public key templates may have the following attributes:
 *                                          - CKA_KEY_TYPE
 *                                              - Must be set to CKK_EC. Only elliptic curve key
 *                                              generation is supported.
 *                                          - CKA_TOKEN
 *                                              - Must be set to CK_TRUE.
 * @param[in] ulPublicKeyAttributeCount     Number of attributes in pxPublicKeyTemplate.
 * @param[in] pxPrivateKeyTemplate          Pointer to a list of attributes that the generated
 *                                          private key should possess.
 *                                          Private key template must have the following attributes:
 *                                          - CKA_LABEL
 *                                              - Label should be no longer than #pkcs11configMAX_LABEL_LENGTH
 *                                              and must be supported by port's PKCS #11 PAL.
 *                                          - CKA_PRIVATE
 *                                              - Must be set to true.
 *                                          - CKA_SIGN
 *                                              - Must be set to true.  Only private keys used
 *                                              for signing are supported.
 *                                          Private key template may have the following attributes:
 *                                          - CKA_KEY_TYPE
 *                                              - Must be set to CKK_EC. Only elliptic curve key
 *                                              generation is supported.
 *                                          - CKA_TOKEN
 *                                              - Must be set to CK_TRUE.
 *
 * @param[in] ulPrivateKeyAttributeCount    Number of attributes in pxPrivateKeyTemplate.
 * @param[out] pxPublicKey                  Pointer to the handle of the public key to be created.
 * @param[out] pxPrivateKey                 Pointer to the handle of the private key to be created.
 *
 * \note Not all attributes specified by the PKCS #11 standard are supported.
 * \note CKA_LOCAL attribute is not supported.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
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
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    psa_status_t uxStatus;
    psa_key_handle_t key_handle_private;
    psa_key_handle_t key_handle_public;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    CK_ATTRIBUTE_PTR pxPrivateLabel = NULL;
    CK_ATTRIBUTE_PTR pxPublicLabel = NULL;
    uint8_t * pUncompressedECPoint = pvPortMalloc( pkcs11KEY_ECPOINT_LENGTH );
    size_t uxPublicKeyLength;

    if( pUncompressedECPoint == NULL )
    {
        xResult = CKR_HOST_MEMORY;
    }

    #if ( pkcs11configSUPPRESS_ECDSA_MECHANISM == 1 )
        if( xResult == CKR_OK )
        {
            xResult = CKR_MECHANISM_INVALID;
        }
    #endif

    if( xResult == CKR_OK )
    {
        if( ( pxPublicKeyTemplate == NULL ) ||
            ( pxPrivateKeyTemplate == NULL ) ||
            ( pxPublicKey == NULL ) ||
            ( pxPrivateKey == NULL ) ||
            ( pxMechanism == NULL ) )
        {
            xResult = CKR_ARGUMENTS_BAD;
        }
    }

    if( xResult == CKR_OK )
    {
        if( CKM_EC_KEY_PAIR_GEN != pxMechanism->mechanism )
        {
            xResult = CKR_MECHANISM_PARAM_INVALID;
        }
    }

    if( xResult == CKR_OK )
    {
        xResult = prvCheckGenerateKeyPairPrivateTemplate( &pxPrivateLabel,
                                                          pxPrivateKeyTemplate,
                                                          ulPrivateKeyAttributeCount );
    }

    if( xResult == CKR_OK )
    {
        xResult = prvCheckGenerateKeyPairPublicTemplate( &pxPublicLabel,
                                                         pxPublicKeyTemplate,
                                                         ulPublicKeyAttributeCount );
    }

    if( xResult == CKR_OK )
    {
        attributes = psa_key_attributes_init();

        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
        psa_set_key_algorithm(&attributes, PSA_ALG_ECDSA( PSA_ALG_SHA_256 ));
        /*
         * Please refer https://tools.ietf.org/html/rfc8422
         * NISTP256 is equivalent to SECP256r1.
         * To keep consistent with mbedtls based implementation, set the key
         * type as ECC_KEYPAIR(SECP256R1).
         * Generate key pair with the private key handle.
         */
        psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_KEY_PAIR( PSA_ECC_CURVE_SECP256R1));
        psa_set_key_bits(&attributes, 256);

        uxStatus = psa_generate_key( &attributes, &key_handle_private);

        if ( uxStatus == PSA_SUCCESS )
        {
            /*
             * Export the public key from the generated key pair.
             * Policy is not checked when exporting a public key.
             */
            uxStatus = psa_export_public_key( key_handle_private,
                                              pUncompressedECPoint,
                                              pkcs11KEY_ECPOINT_LENGTH,
                                              &uxPublicKeyLength );

        }
        if ( uxStatus == PSA_SUCCESS )
        {
            attributes = psa_key_attributes_init();

            psa_set_key_usage_flags( &attributes, PSA_KEY_USAGE_VERIFY_HASH);
            psa_set_key_algorithm( &attributes, PSA_ALG_ECDSA( PSA_ALG_SHA_256 ));
            psa_set_key_type( &attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_CURVE_SECP256R1));

            /* Import the public key with data exported by psa_export_public_key. */
            uxStatus = psa_import_key( &attributes,
                                       pUncompressedECPoint,
                                       uxPublicKeyLength,
                                       &key_handle_public );
        }
        if ( uxStatus == PSA_SUCCESS )
        {
            if( strcmp( pxPrivateLabel->pValue,
                    pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) == 0 )
            {
                P11KeyConfig.uxDevicePrivateKey = key_handle_private;

                /*
                * The length of a PSA_ECC_CURVE_SECP256R1 private key is 32 bytes.
                */
                P11KeyConfig.ulDevicePrivateKeyMark = ( pkcs11OBJECT_PRESENT_MAGIC | ( size_t )32 );
                xResult = prvAddObjectToList( eAwsDevicePrivateKey, pxPrivateKey, pxPrivateLabel->pValue, pxPrivateLabel->ulValueLen );
            }
            else
            {
                xResult = CKR_ATTRIBUTE_VALUE_INVALID;
            }
            if( strcmp( pxPublicLabel->pValue,
                    pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) == 0 )
            {
                P11KeyConfig.uxDevicePublicKey = key_handle_public;
                P11KeyConfig.ulDevicePublicKeyMark = ( pkcs11OBJECT_PRESENT_MAGIC | uxPublicKeyLength );
                if( xResult == CKR_OK )
                {
                    xResult = prvAddObjectToList( eAwsDevicePublicKey, pxPublicKey, pxPublicLabel->pValue, pxPublicLabel->ulValueLen );
                }
            }
            else
            {
                xResult = CKR_ATTRIBUTE_VALUE_INVALID;
            }
        }
        else
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }
    return xResult;
}

/**
 * @brief Generate cryptographically random bytes.
 *
 * @param xSession[in]          Handle of a valid PKCS #11 session.
 * @param pucRandomData[out]    Pointer to location that random data will be placed.
 *                              It is the responsiblity of the application to allocate
 *                              this memory.
 * @param ulRandomLength[in]    Length of data (in bytes) to be generated.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GenerateRandom )( CK_SESSION_HANDLE xSession,
                                               CK_BYTE_PTR pucRandomData,
                                               CK_ULONG ulRandomLen )
{
    CK_RV xResult = CKR_OK;
    psa_status_t uxStatus;

    /* Avoid warnings about unused parameters. */
    ( void ) xSession;

    if( ( NULL == pucRandomData ) ||
        ( ulRandomLen == 0 ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        uxStatus = psa_generate_random( pucRandomData, ( size_t )ulRandomLen );
        if ( uxStatus != PSA_SUCCESS )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    return xResult;
}
