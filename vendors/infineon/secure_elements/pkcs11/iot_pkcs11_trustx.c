/**
 * MIT License
 *
 * Copyright (c) 2018 Infineon Technologies AG
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE
 *
 * OPTIGA(TM) Trust X - based PKCS#11 V1.0.0
 */


/**
 * @file aws_pkcs11_trustx.c
 * @brief OPTIGA(TM) Trust X -based PKCS#11 implementation for software keys. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS #11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "iot_pkcs11_config.h"
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "iot_test_pkcs11_config.h"
#endif
#include "iot_crypto.h"
#include "iot_pkcs11.h"
#include "iot_pkcs11_pal.h"

/* OPTIGA(TM) Trust X Includes */
#include "optiga/optiga_crypt.h"
#include "optiga/optiga_util.h"
#include "optiga/pal/pal_os_event.h"
#include "optiga/ifx_i2c/ifx_i2c_config.h"
#include "ecdsa_utils.h"

#if ( pkcs11configOTA_SUPPORTED == 1 )
    #include "aws_ota_codesigner_certificate.h"
#endif

/* C runtime includes. */
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define PKCS11_PRINT( X )            configPRINTF( X )
#define PKCS11_WARNING_PRINT( X )    /* configPRINTF( X ) */

#define pkcs11NO_OPERATION            ( ( CK_MECHANISM_TYPE ) 0xFFFFFFFFF )

/* The size of the buffer malloc'ed for the exported public key in C_GenerateKeyPair */
#define pkcs11KEY_GEN_MAX_DER_SIZE    200

#define MAX_PUBLIC_KEY_SIZE           100

typedef struct P11Object_t
{
    CK_OBJECT_HANDLE xHandle;
    /* If Label is associated with a private key slot. */
    /* This flag is used to mark it active/non-active, as we can't remove the private key */
    CK_BYTE xLabel[ pkcs11configMAX_LABEL_LENGTH + 1 ]; /* Plus 1 for the null terminator. */
} P11Object_t;

typedef struct P11ObjectList_t
{
    SemaphoreHandle_t xMutex; /* Mutex that protects write operations to the xObjects array. */
    P11Object_t xObjects[ pkcs11configMAX_NUM_OBJECTS ];
} P11ObjectList_t;

/* PKCS #11 Object */
typedef struct P11Struct_t
{
    CK_BBOOL xIsInitialized;
    P11ObjectList_t xObjectList;
    uint16_t xCertOid;
    uint16_t xPrivKeyOid;
} P11Struct_t, * P11Context_t;

static P11Struct_t xP11Context;

typedef struct OptigaSha256Ctx_t
{
    uint8_t pubHashCtxBuf[ 130 ];
    optiga_hash_context_t xHashCtx;
} OptigaSha256Ctx_t;

/**
 * @brief Session structure.
 */
typedef struct P11Session
{
    CK_ULONG ulState;
    CK_BBOOL xOpened;
    CK_MECHANISM_TYPE xOperationInProgress;
    CK_BYTE * pxFindObjectLabel; /* Pointer to the label for the search in progress. Should be NULL if no search in progress. */
    uint8_t xFindObjectLabelLength;
    CK_MECHANISM_TYPE xVerifyMechanism;
    uint16_t usVerifyKeyOid;
    CK_MECHANISM_TYPE xSignMechanism;
    uint16_t usSignKeyOid;
    OptigaSha256Ctx_t xSHA256Context;
} P11Session_t, * P11SessionPtr_t;


/**
 * @brief Cryptoki module attribute definitions.
 */
#define pkcs11SLOT_ID    1


/**
 * @brief Helper definitions.
 */
#define pkcs11CREATE_OBJECT_MIN_ATTRIBUTE_COUNT             3

#define pkcs11GENERATE_KEY_PAIR_KEYTYPE_ATTRIBUTE_INDEX     0
#define pkcs11GENERATE_KEY_PAIR_ECPARAMS_ATTRIBUTE_INDEX    1
#define PKCS11_MODULE_IS_INITIALIZED                        ( ( xP11Context.xIsInitialized == CK_TRUE ) ? 1 : 0 )
#define PKCS11_SESSION_IS_OPEN( xSessionHandle )                         ( ( ( ( P11SessionPtr_t ) xSessionHandle )->xOpened ) == CK_TRUE ? CKR_OK : CKR_SESSION_CLOSED )
#define PKCS11_SESSION_IS_VALID( xSessionHandle )                        ( ( ( P11SessionPtr_t ) xSessionHandle != NULL ) ? PKCS11_SESSION_IS_OPEN( xSessionHandle ) : CKR_SESSION_HANDLE_INVALID )
#define PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSessionHandle )    ( PKCS11_MODULE_IS_INITIALIZED ? PKCS11_SESSION_IS_VALID( xSessionHandle ) : CKR_CRYPTOKI_NOT_INITIALIZED )
/*-----------------------------------------------------------*/

#define pkcs11OBJECT_CERTIFICATE_MAX_SIZE    1728

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsTestPrivateKey,
    eAwsDevicePublicKey,
    eAwsTestPublicKey,
    eAwsDeviceCertificate,
    eAwsTestCertificate,
    eAwsCodeSigningKey
};


/* The communication context `ifx_i2c_context_0` is declared in the header file ifx_i2c_config.h */
optiga_comms_t optiga_comms = { ( void * ) &ifx_i2c_context_0, NULL, NULL, OPTIGA_COMMS_SUCCESS };

/*-----------------------------------------------------------*/

static uint8_t prvAppendOptigaCertTags( uint16_t xCertWithoutTagsLength,
                                        uint8_t * pxCertTags,
                                        uint16_t xCertTagsLength )
{
    char t1[ 3 ], t2[ 3 ], t3[ 3 ];

    int xCalc = xCertWithoutTagsLength,
        xCalc1 = 0,
        xCalc2 = 0;

    uint8_t ret = 0;

    do
    {
        if( ( pxCertTags == NULL ) || ( xCertWithoutTagsLength == 0 ) ||
            ( xCertTagsLength != 9 ) )
        {
            break;
        }

        if( xCalc > 0xFF )
        {
            xCalc1 = xCalc >> 8;
            xCalc = xCalc % 0x100;

            if( xCalc1 > 0xFF )
            {
                xCalc2 = xCalc1 >> 8;
                xCalc1 = xCalc1 % 0x100;
            }
        }

        t3[ 0 ] = xCalc2;
        t3[ 1 ] = xCalc1;
        t3[ 2 ] = xCalc;
        xCalc = xCertWithoutTagsLength + 3;

        if( xCalc > 0xFF )
        {
            xCalc1 = xCalc >> 8;
            xCalc = xCalc % 0x100;

            if( xCalc1 > 0xFF )
            {
                xCalc2 = xCalc1 >> 8;
                xCalc1 = xCalc1 % 0x100;
            }
        }

        t2[ 0 ] = xCalc2;
        t2[ 1 ] = xCalc1;
        t2[ 2 ] = xCalc;
        xCalc = xCertWithoutTagsLength + 6;

        if( xCalc > 0xFF )
        {
            xCalc1 = xCalc >> 8;
            xCalc = xCalc % 0x100;

            if( xCalc1 > 0xFF )
            {
                xCalc2 = xCalc1 >> 8;
                xCalc1 = xCalc1 % 0x100;
            }
        }

        t1[ 0 ] = 0xC0;
        t1[ 1 ] = xCalc1;
        t1[ 2 ] = xCalc;

        for( int i = 0; i < 3; i++ )
        {
            pxCertTags[ i ] = t1[ i ];
        }

        for( int i = 0; i < 3; i++ )
        {
            pxCertTags[ i + 3 ] = t2[ i ];
        }

        for( int i = 0; i < 3; i++ )
        {
            pxCertTags[ i + 6 ] = t3[ i ];
        }

        ret = 1;
    } while( 0 );

    return ret;
}

static int32_t prvUploadCertificate( char * pucLabel,
                                     uint8_t * pucData,
                                     uint32_t ulDataSize )
{
    long lOptigaOid = 0;
    const uint8_t xTagsLength = 9;
    uint8_t pxCertTags[ xTagsLength ];
    optiga_lib_status_t xReturn;
    char * xEnd = NULL;

    /**
     * Write a certificate to a given cert object (e.g. E0E8)
     * using optiga_util_write_data.
     *
     * Use Erase and Write (OPTIGA_UTIL_ERASE_AND_WRITE) option,
     * to clear the remaining data in the object
     */

    lOptigaOid = strtol( pucLabel, &xEnd, 16 );

    if( ( 0 != lOptigaOid ) && ( USHRT_MAX > lOptigaOid ) && ( USHRT_MAX > ulDataSize ) )
    {
        /* Certificates on OPTIGA Trust SE are stored with certitficate identifiers -> tags, */
        /* which are 9 bytes long */
        if( prvAppendOptigaCertTags( ulDataSize, pxCertTags, xTagsLength ) )
        {
            xReturn = optiga_util_write_data( ( uint16_t ) lOptigaOid,
                                              OPTIGA_UTIL_ERASE_AND_WRITE,
                                              0,
                                              pxCertTags,
                                              9 );

            if( OPTIGA_LIB_SUCCESS == xReturn )
            {
                xReturn = optiga_util_write_data( ( uint16_t ) lOptigaOid,
                                                  OPTIGA_UTIL_WRITE_ONLY,
                                                  xTagsLength,
                                                  pucData,
                                                  ulDataSize );
            }
        }
    }

    return xReturn;
}

static int32_t prvUploadPublicKey( char * pucLabel,
                                   uint8_t * pucData,
                                   uint32_t ulDataSize )
{
    long lOptigaOid = 0;
    optiga_lib_status_t xReturn;
    char * xEnd = NULL;

    /**
     * Write a public key to an arbitrary data object
     * Note: You might need to lock the data object here. see optiga_util_write_metadata()
     *
     * Use Erase and Write (OPTIGA_UTIL_ERASE_AND_WRITE) option,
     * to clear the remaining data in the object
     */
    lOptigaOid = strtol( pucLabel, &xEnd, 16 );

    if( ( 0 != lOptigaOid ) && ( USHRT_MAX >= lOptigaOid ) && ( USHRT_MAX >= ulDataSize ) )
    {
        xReturn = optiga_util_write_data( ( uint16_t ) lOptigaOid,
                                          OPTIGA_UTIL_ERASE_AND_WRITE,
                                          0,
                                          pucData,
                                          ulDataSize );
    }

    return xReturn;
}

/**
 * @brief Saves an object in non-volatile storage.
 *
 * Port-specific file write for cryptographic information.
 *
 * @param[in] pxLabel       The label of the object to be stored.
 * @param[in] pucData       The object data to be saved
 * @param[in] pulDataSize   Size (in bytes) of object data.
 *
 * @return The object handle if successful.
 * eInvalidHandle = 0 if unsuccessful.
 */
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                        CK_BYTE_PTR pucData,
                                        CK_ULONG ulDataSize )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;

    long lOptigaOid = 0;
    uint8_t bOffset = 0;
    char * xEnd = NULL;

    optiga_lib_status_t xReturn;


    if( ulDataSize <= pkcs11OBJECT_CERTIFICATE_MAX_SIZE )
    {
        /* Translate from the PKCS#11 label to local storage file name. */
        if( 0 == memcmp( pxLabel->pValue,
                         &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                         sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
        {
            if( prvUploadCertificate( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, pucData, ulDataSize ) == OPTIGA_LIB_SUCCESS )
            {
                xHandle = eAwsDeviceCertificate;
            }
        }

        #if AMAZON_FREERTOS_ENABLE_UNIT_TESTS
            else if( 0 == memcmp( pxLabel->pValue,
                                  &pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                  sizeof( pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
            {
                if( prvUploadCertificate( pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS, pucData, ulDataSize ) == OPTIGA_LIB_SUCCESS )
                {
                    xHandle = eAwsTestCertificate;
                }
            }
        #endif
        else if( 0 == memcmp( pxLabel->pValue,
                              &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
        {
            /* This operation isn't supported for the OPTIGA(TM) Trust X due to a security considerations
             * You can only generate a keypair and export a private component if you like */
            /* We do assign a handle though, as the AWS can#t handle the lables without having a handle*/
            xHandle = eAwsDevicePrivateKey;
        }
        #if AMAZON_FREERTOS_ENABLE_UNIT_TESTS
            else if( 0 == memcmp( pxLabel->pValue,
                                  &pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                  sizeof( pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
            {
                xHandle = eAwsTestPrivateKey;
            }
        #endif
        else if( 0 == memcmp( pxLabel->pValue,
                              &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
        {
            if( prvUploadPublicKey( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, pucData, ulDataSize ) == OPTIGA_LIB_SUCCESS )
            {
                xHandle = eAwsDevicePublicKey;
            }
        }
        #if AMAZON_FREERTOS_ENABLE_UNIT_TESTS
            else if( 0 == memcmp( pxLabel->pValue,
                                  &pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                                  sizeof( pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
            {
                if( prvUploadPublicKey( pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, pucData, ulDataSize ) == OPTIGA_LIB_SUCCESS )
                {
                    xHandle = eAwsTestPublicKey;
                }
            }
        #endif
        else if( 0 == memcmp( pxLabel->pValue,
                              &pkcs11configLABEL_CODE_VERIFICATION_KEY,
                              sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
        {
            /**
             * Write a Code Verification Key/Certificate to an Trust Anchor data object
             * Note: You might need to lock the data object here. see optiga_util_write_metadata()
             *
             * Use Erase and Write (OPTIGA_UTIL_ERASE_AND_WRITE) option,
             * to clear the remaining data in the object
             */
            lOptigaOid = strtol( pkcs11configLABEL_CODE_VERIFICATION_KEY, &xEnd, 16 );

            if( ( 0 != lOptigaOid ) && ( USHRT_MAX > lOptigaOid ) && ( USHRT_MAX > ulDataSize ) )
            {
                xReturn = optiga_util_write_data( ( uint16_t ) lOptigaOid,
                                                  OPTIGA_UTIL_ERASE_AND_WRITE,
                                                  bOffset,
                                                  pucData,
                                                  ulDataSize );

                if( OPTIGA_LIB_SUCCESS == xReturn )
                {
                    xHandle = eAwsCodeSigningKey;
                }
            }
        }
    }

    return xHandle;
}


/*-----------------------------------------------------------*/

/**
 * @brief Translates a PKCS #11 label into an object handle.
 *
 * Port-specific object handle retrieval.
 *
 *
 * @param[in] pxLabel         Pointer to the label of the object
 *                           who's handle should be found.
 * @param[in] usLength       The length of the label, in bytes.
 *
 * @return The object handle if operation was successful.
 * Returns eInvalidHandle if unsuccessful.
 */
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( CK_BYTE_PTR pLabel,
                                        CK_ULONG usLength )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;

    /* Translate from the PKCS#11 label to local storage file name. */
    if( 0 == memcmp( pLabel,
                     &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                     sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
    {
        xHandle = eAwsDeviceCertificate;
    }

    #if AMAZON_FREERTOS_ENABLE_UNIT_TESTS
        else if( 0 == memcmp( pLabel,
                              &pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                              sizeof( pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
        {
            xHandle = eAwsTestCertificate;
        }
    #endif
    else if( 0 == memcmp( pLabel,
                          &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                          sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
    {
        /* This operation isn't supported for the OPTIGA(TM) Trust X due to a security considerations
         * You can only generate a keypair and export a private component if you like */
        /* We do assign a handle though, as the AWS can#t handle the lables without having a handle*/
        xHandle = eAwsDevicePrivateKey;
    }
    #if AMAZON_FREERTOS_ENABLE_UNIT_TESTS
        else if( 0 == memcmp( pLabel,
                              &pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                              sizeof( pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
        {
            xHandle = eAwsTestPrivateKey;
        }
    #endif
    else if( 0 == memcmp( pLabel,
                          &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                          sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
    {
        xHandle = eAwsDevicePublicKey;
    }
    #if AMAZON_FREERTOS_ENABLE_UNIT_TESTS
        else if( 0 == memcmp( pLabel,
                              &pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                              sizeof( pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
        {
            xHandle = eAwsTestPublicKey;
        }
    #endif
    else if( 0 == memcmp( pLabel,
                          &pkcs11configLABEL_CODE_VERIFICATION_KEY,
                          sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
    {
        xHandle = eAwsCodeSigningKey;
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

/**
 * @brief Gets the value of an object in storage, by handle.
 *
 * Port-specific file access for cryptographic information.
 *
 * This call dynamically allocates the buffer which object value
 * data is copied into.  PKCS11_PAL_GetObjectValueCleanup()
 * should be called after each use to free the dynamically allocated
 * buffer.
 *
 * @sa PKCS11_PAL_GetObjectValueCleanup
 *
 * @param[in] pcFileName    The name of the file to be read.
 * @param[out] ppucData     Pointer to buffer for file data.
 * @param[out] pulDataSize  Size (in bytes) of data located in file.
 * @param[out] pIsPrivate   Boolean indicating if value is private (CK_TRUE)
 *                          or exportable (CK_FALSE)
 *
 * @return CKR_OK if operation was successful.  CKR_KEY_HANDLE_INVALID if
 * no such object handle was found, CKR_DEVICE_MEMORY if memory for
 * buffer could not be allocated, CKR_FUNCTION_FAILED for device driver
 * error.
 */
CK_RV PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                      CK_BYTE_PTR * ppucData,
                                      CK_ULONG_PTR pulDataSize,
                                      CK_BBOOL * pIsPrivate )
{
    BaseType_t ulReturn = CKR_OK;
    optiga_lib_status_t xReturn;
    long lOptigaOid = 0;
    char * xEnd = NULL;
    uint8_t xOffset = 0;

    *pIsPrivate = CK_FALSE;

    switch( xHandle )
    {
        case eAwsDeviceCertificate:
            lOptigaOid = strtol( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, &xEnd, 16 );
            xOffset = 9;
            break;

            #if AMAZON_FREERTOS_ENABLE_UNIT_TESTS
                case eAwsTestCertificate:
                    lOptigaOid = strtol( pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS, &xEnd, 16 );
                    xOffset = 9;
                    break;
            #endif
        case eAwsDevicePublicKey:
            lOptigaOid = strtol( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, &xEnd, 16 );
            break;

            #if AMAZON_FREERTOS_ENABLE_UNIT_TESTS
                case eAwsTestPublicKey:
                    lOptigaOid = strtol( pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, &xEnd, 16 );
                    break;
            #endif
        case eAwsCodeSigningKey:
            lOptigaOid = strtol( pkcs11configLABEL_CODE_VERIFICATION_KEY, &xEnd, 16 );
            break;

        case eAwsDevicePrivateKey:
        case eAwsTestPrivateKey:

        /*
         * This operation isn't supported for the OPTIGA(TM) Trust X due to a security considerations
         * You can only generate a keypair and export a private component if you like
         */
        default:
            ulReturn = CKR_KEY_HANDLE_INVALID;
            break;
    }

    /* We need to allocate a buffer for a certificate/certificate chain */
    /* This data is later should be freed with PKCS11_PAL_GetObjectValueCleanup */
    *ppucData = pvPortMalloc( pkcs11OBJECT_CERTIFICATE_MAX_SIZE );

    if( NULL != *ppucData )
    {
        *pulDataSize = pkcs11OBJECT_CERTIFICATE_MAX_SIZE;

        if( ( 0 != lOptigaOid ) && ( USHRT_MAX > lOptigaOid ) && ( NULL != pulDataSize ) )
        {
            xReturn = optiga_util_read_data( lOptigaOid, xOffset, *ppucData, ( uint16_t * ) pulDataSize );

            if( OPTIGA_LIB_SUCCESS != xReturn )
            {
                *ppucData = NULL;
                *pulDataSize = 0;
                ulReturn = CKR_KEY_HANDLE_INVALID;
            }
        }
    }

    return ulReturn;
}



/*-----------------------------------------------------------*/

/**
 * @brief Cleanup after PKCS11_GetObjectValue().
 *
 * @param[in] pucData       The buffer to free.
 *                          (*ppucData from PKCS11_PAL_GetObjectValue())
 * @param[in] ulDataSize    The length of the buffer to free.
 *                          (*pulDataSize from PKCS11_PAL_GetObjectValue())
 */
void PKCS11_PAL_GetObjectValueCleanup( CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataSize )
{
    /* Unused parameters. */
    vPortFree( pucData );

    /* Since no buffer was allocated on heap, there is no cleanup
     * to be done. */
}


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
    NULL,               /*C_GetSlotInfo*/
    C_GetTokenInfo,
    NULL,               /*C_GetMechanismList*/
    C_GetMechanismInfo, /*C_GetMechanismInfo */
    C_InitToken,
    NULL,               /*C_InitPIN*/
    NULL,               /*C_SetPIN*/
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

/*-----------------------------------------------------------*/

CK_RV prvOPTIGATrustX_Initialize( void )
{
    CK_RV xResult = CKR_OK;
    uint16_t optiga_oid;

    if( xP11Context.xIsInitialized == CK_TRUE )
    {
        xResult = CKR_CRYPTOKI_ALREADY_INITIALIZED;
    }

    if( xResult == CKR_OK )
    {
        memset( &xP11Context, 0, sizeof( xP11Context ) );
        xP11Context.xObjectList.xMutex = xSemaphoreCreateMutex();

/*        CRYPTO_Init(); */

        pal_os_event_init();

        xResult = optiga_util_open_application( &optiga_comms );

        /*status = example_authenticate_chip(); */

        if( OPTIGA_LIB_SUCCESS == xResult )
        {
            xP11Context.xIsInitialized = CK_TRUE;
            xResult = CKR_OK;

            optiga_oid = eCURRENT_LIMITATION;
            /* Maximum Power, Minimum Current limitation */
            uint8_t current_limit = 15;
            optiga_util_write_data( optiga_oid, OPTIGA_UTIL_WRITE_ONLY,
                                    0, /*offset */
                                    &current_limit,
                                    1 );
        }
        else
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    return xResult;
}


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
/*-----------------------------------------------------------------------*/
/* Functions for maintaining the PKCS #11 module's label-handle lookups. */
/*-----------------------------------------------------------------------*/


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
 * @brief Searches the PKCS #11 module's object list for handle and provides label info.
 *
 * @param[in] xAppHandle         The handle of the object being searched for, used by the application.
 * @param[out] xPalHandle        The handle of the object being used by the PAL.
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

    if( pkcs11configMAX_NUM_OBJECTS >= lIndex )
    {
        if( xP11Context.xObjectList.xObjects[ lIndex ].xHandle != CK_INVALID_HANDLE )
        {
            *ppcLabel = xP11Context.xObjectList.xObjects[ lIndex ].xLabel;
            *pxLabelLength = strlen( ( const char * ) xP11Context.xObjectList.xObjects[ lIndex ].xLabel ) + 1;
            *pxPalHandle = xP11Context.xObjectList.xObjects[ lIndex ].xHandle;
        }
    }
    else
    {
        *ppcLabel = NULL;
        *pxLabelLength = 0;
        pxPalHandle = NULL;
    }
}

CK_RV prvDeleteObjectFromList( CK_OBJECT_HANDLE xAppHandle )
{
    CK_RV xResult = CKR_OK;
    BaseType_t xGotSemaphore;
    int lIndex = xAppHandle - 1;

    xGotSemaphore = xSemaphoreTake( xP11Context.xObjectList.xMutex, portMAX_DELAY );

    if( xGotSemaphore == pdTRUE )
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
        }

        xSemaphoreGive( xP11Context.xObjectList.xMutex );
    }
    else
    {
        xResult = CKR_CANT_LOCK;
    }

    return xResult;
}

#if ( pkcs11configPAL_DESTROY_SUPPORTED != 1 )

    CK_RV PKCS11_PAL_DestroyObject( CK_OBJECT_HANDLE xAppHandle )
    {
        uint8_t * pcLabel = NULL;
        char * pcTempLabel = NULL;
        size_t xLabelLength = 0;
        uint32_t ulObjectLength = 0;
        CK_RV xResult = CKR_OK;
        CK_BBOOL xFreeMemory = CK_FALSE;
        CK_BYTE_PTR pxObject = NULL;
        CK_OBJECT_HANDLE xPalHandle;
        CK_OBJECT_HANDLE xAppHandle2;
        CK_LONG lOptigaOid = 0;
        char * xEnd = NULL;

        prvFindObjectInListByHandle( xAppHandle, &xPalHandle, &pcLabel, &xLabelLength );

        if( pcLabel != NULL )
        {
            if( ( 0 == memcmp( pcLabel, pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, xLabelLength ) )
                #if AMAZON_FREERTOS_ENABLE_UNIT_TESTS
                    ||
                    ( 0 == memcmp( pcLabel, pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, xLabelLength ) )
                #endif
                )
            {
                prvFindObjectInListByLabel( ( uint8_t * ) pcLabel, strlen( ( char * ) pcLabel ), &xPalHandle, &xAppHandle2 );

                if( xPalHandle != CK_INVALID_HANDLE )
                {
                    xResult = prvDeleteObjectFromList( xAppHandle2 );
                }

                lOptigaOid = strtol( ( char * ) pcLabel, &xEnd, 16 );

                CK_BYTE pucDumbData[ 68 ];
                uint16_t ucDumbDataLength = 68;

                if( OPTIGA_LIB_SUCCESS != optiga_crypt_ecc_generate_keypair( OPTIGA_ECC_NIST_P_256,
                                                                             ( uint8_t ) OPTIGA_KEY_USAGE_SIGN,
                                                                             FALSE,
                                                                             &lOptigaOid,
                                                                             pucDumbData,
                                                                             &ucDumbDataLength ) )
                {
                    PKCS11_PRINT( ( "ERROR: Failed to invalidate a keypair \r\n" ) );
                    xResult = CKR_FUNCTION_FAILED;
                }
            }
            else
            {
                if( 0 == memcmp( pcLabel, pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, xLabelLength ) )
                {
                    pcTempLabel = pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
                }

                #if AMAZON_FREERTOS_ENABLE_UNIT_TESTS
                    else if( 0 == memcmp( pcLabel, pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, xLabelLength ) )
                    {
                        pcTempLabel = pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
                    }
                #endif
                else if( 0 == memcmp( pcLabel, pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, xLabelLength ) )
                {
                    pcTempLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
                }
                #if AMAZON_FREERTOS_ENABLE_UNIT_TESTS
                    else if( 0 == memcmp( pcLabel, pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS, xLabelLength ) )
                    {
                        pcTempLabel = pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS;
                    }
                #endif
                else if( 0 == memcmp( pcLabel, pkcs11configLABEL_CODE_VERIFICATION_KEY, xLabelLength ) )
                {
                    pcTempLabel = pkcs11configLABEL_CODE_VERIFICATION_KEY;
                }

                if( pcTempLabel != NULL )
                {
                    lOptigaOid = strtol( pcTempLabel, &xEnd, 16 );

                    if( ( 0 != lOptigaOid ) && ( USHRT_MAX >= lOptigaOid ) )
                    {
                        /* Erase the object */
                        CK_BYTE pucData[] = { 0 };
                        xResult = optiga_util_write_data( ( uint16_t ) lOptigaOid,
                                                          OPTIGA_UTIL_ERASE_AND_WRITE,
                                                          0, /* Offset */
                                                          pucData,
                                                          1 );

                        if( OPTIGA_LIB_SUCCESS == xResult )
                        {
                            prvFindObjectInListByLabel( ( uint8_t * ) pcTempLabel, strlen( ( char * ) pcTempLabel ), &xPalHandle, &xAppHandle2 );

                            if( xPalHandle != CK_INVALID_HANDLE )
                            {
                                xResult = prvDeleteObjectFromList( xAppHandle2 );
                            }
                        }
                    }
                }
                else
                {
                    xResult = CKR_KEY_HANDLE_INVALID;
                }
            }

            if( xAppHandle2 != xAppHandle )
            {
                xResult = prvDeleteObjectFromList( xAppHandle );
            }
        }
        else
        {
            xResult = CKR_KEY_HANDLE_INVALID;
        }

        if( xFreeMemory == CK_TRUE )
        {
            PKCS11_PAL_GetObjectValueCleanup( pxObject, ulObjectLength );
        }

        return xResult;
    }

#endif /* if ( pkcs11configPAL_DESTROY_SUPPORTED != 1 ) */

/*-------------------------------------------------------------*/

/**
 * @brief Initialize the Cryptoki module for use.
 *
 */
CK_DEFINE_FUNCTION( CK_RV, C_Initialize )( CK_VOID_PTR pvInitArgs )
{ /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( pvInitArgs );

    CK_RV xResult = CKR_OK;

    /* Ensure that the FreeRTOS heap is used. */
    if( xP11Context.xIsInitialized != CK_TRUE )
    {
        /*
         *   Reset OPTIGA(TM) Trust X and open an application on it
         */
        xResult = prvOPTIGATrustX_Initialize();


        CK_OBJECT_HANDLE xObject;
        CK_OBJECT_HANDLE xPalHandle = PKCS11_PAL_FindObject( ( uint8_t * ) pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, strlen( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) );
        xResult = prvAddObjectToList( xPalHandle, &xObject, ( uint8_t * ) pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, strlen( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) );
    }
    else
    {
        xResult = CKR_CRYPTOKI_ALREADY_INITIALIZED;
    }

    return xResult;
}

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
        vSemaphoreDelete( xP11Context.xObjectList.xMutex );

        xP11Context.xIsInitialized = CK_FALSE;
    }

    return xResult;
}

/**
 * @brief Query the list of interface function pointers.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetFunctionList )( CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{ /*lint !e9072 It's OK to have different parameter name. */
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
{ /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    /* Since the implementation of PKCS#11 does not depend
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
 * @brief Returns firmware, hardware, manufacturer, and model information for
 * the crypto token.
 *
 * @return CKR_OK.
 */
CK_DECLARE_FUNCTION( CK_RV, C_GetTokenInfo )( CK_SLOT_ID slotID,
                                              CK_TOKEN_INFO_PTR pInfo )
{
    /* Avoid compiler warnings about unused variables. */
    ( void ) slotID;

    if( pInfo != NULL )
    {
        pInfo->firmwareVersion.major = 0x11;
        pInfo->firmwareVersion.minor = 0x18;

        pInfo->hardwareVersion.major = 1;
        pInfo->hardwareVersion.minor = 0;

        sprintf( ( char * ) pInfo->manufacturerID, "Infineon Technologies AG" );
        sprintf( ( char * ) pInfo->model, "OPTIGA Trust X" );
        pInfo->ulMaxSessionCount = 4;
    }

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
        { CKM_ECDSA,           { 256, 256, CKF_SIGN | CKF_VERIFY } },
        { CKM_EC_KEY_PAIR_GEN, { 256, 256, CKF_GENERATE_KEY_PAIR } },
        { CKM_SHA256,          { 0,   0,   CKF_DIGEST            } }
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
{ /*lint !e9072 It's OK to have different parameter name. */
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
        pxSessionObj = ( P11SessionPtr_t ) pvPortMalloc( sizeof( struct P11Session ) ); /*lint !e9087 Allow casting void* to other types. */

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
{ /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( ( xResult == CKR_OK ) && ( NULL != pxSession ) )
    {
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
    /* THIS FUNCTION IS NOT IMPLEMENTED
     * If login capability is required, implement it here.
     * Defined for compatibility with other PKCS #11 ports. */
    return CKR_OK;
}


CK_RV prvCreateCertificate( CK_ATTRIBUTE_PTR pxTemplate,
                            CK_ULONG ulCount,
                            CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_RV xResult = CKR_OK;
    CK_BYTE_PTR pxCertificateValue = NULL;
    CK_ULONG xCertificateLength = 0;
    CK_ATTRIBUTE_PTR pxLabel = NULL;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    CK_CERTIFICATE_TYPE xCertificateType = 0; /* = CKC_X_509; */
    uint32_t ulIndex = 0;
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

                if( xAttribute.ulValueLen < pkcs11configMAX_LABEL_LENGTH )
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

            case ( CKA_CLASS ):
            case ( CKA_SUBJECT ):
            case ( CKA_TOKEN ):

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
        xPalHandle = PKCS11_PAL_SaveObject( pxLabel, pxCertificateValue, xCertificateLength );

        if( xPalHandle == 0 ) /*Invalid handle. */
        {
            xResult = CKR_DEVICE_MEMORY;
        }
    }

    if( xResult == CKR_OK )
    {
        xResult = prvAddObjectToList( xPalHandle, pxObject, pxLabel->pValue, pxLabel->ulValueLen );
        /* TODO: If this fails, should the object be wiped back out of flash?  But what if that fails?!?!? */
    }

    return xResult;
}

#define PKCS11_INVALID_KEY_TYPE    ( ( CK_KEY_TYPE ) 0xFFFFFFFF )

CK_KEY_TYPE prvGetKeyType( CK_ATTRIBUTE_PTR pxTemplate,
                           CK_ULONG ulCount )
{
    CK_KEY_TYPE xKeyType = PKCS11_INVALID_KEY_TYPE;
    uint32_t ulIndex;
    CK_ATTRIBUTE xAttribute;

    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        if( xAttribute.type == CKA_KEY_TYPE )
        {
            memcpy( &xKeyType, xAttribute.pValue, sizeof( CK_KEY_TYPE ) );
            break;
        }
    }

    return xKeyType;
}

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


CK_RV prvGetExistingKeyComponent( CK_ATTRIBUTE_PTR pxLabel,
                                  uint8_t * pxPublicKey,
                                  uint32_t ulKeySize )
{
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    uint8_t * pucData = NULL;
    size_t xDataLength = 0;
    CK_BBOOL xIsPrivate = CK_TRUE;
    BaseType_t xResult = CKR_KEY_HANDLE_INVALID;
    CK_BBOOL xNeedToFreeMem = CK_FALSE;

    if( 0 == memcmp( pxLabel->pValue, pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, pxLabel->ulValueLen ) )
    {
        xPalHandle = PKCS11_PAL_FindObject( ( uint8_t * ) pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, ( uint8_t ) pxLabel->ulValueLen );
    }
    else if( 0 == memcmp( pxLabel->pValue, pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, pxLabel->ulValueLen ) )
    {
        xPalHandle = PKCS11_PAL_FindObject( ( uint8_t * ) pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, ( uint8_t ) pxLabel->ulValueLen );
    }

    if( xPalHandle != CK_INVALID_HANDLE )
    {
        xResult = PKCS11_PAL_GetObjectValue( xPalHandle, &pucData, ( uint32_t * ) &xDataLength, &xIsPrivate );

        if( xResult == CKR_OK )
        {
            xNeedToFreeMem = CK_TRUE;
        }
    }

    if( ( xResult == CKR_OK ) && ( NULL != pxPublicKey ) )
    {
        memcpy( pxPublicKey, pucData, xDataLength );
    }

    if( xNeedToFreeMem == CK_TRUE )
    {
        PKCS11_PAL_GetObjectValueCleanup( pucData, xDataLength );
    }

    return xResult;
}


CK_RV prvCreateECPublicKey( uint8_t * pxPublicKey,
                            uint32_t * pulKeySize,
                            CK_ATTRIBUTE_PTR * ppxLabel,
                            CK_ATTRIBUTE_PTR pxTemplate,
                            CK_ULONG ulCount,
                            CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_RV xResult = CKR_OK;
    CK_BBOOL xBool;
    uint32_t ulIndex;
    CK_ATTRIBUTE xAttribute;

    for( ulIndex = 0; ulIndex < ulCount; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        switch( xAttribute.type )
        {
            case ( CKA_CLASS ):
            case ( CKA_KEY_TYPE ):
            case ( CKA_TOKEN ):

                /* Do nothing.
                 * At this time there is only token object support.
                 * Key type and class were checked previously. */
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

                if( *pulKeySize > ( xAttribute.ulValueLen - 2 ) )
                {
                    /* The first 2 bytes are for ASN1 type/length encoding. */
                    memcpy( pxPublicKey, ( ( uint8_t * ) ( xAttribute.pValue ) + 2 ), ( xAttribute.ulValueLen - 2 ) );
                    *pulKeySize = xAttribute.ulValueLen - 2;
                }
                else
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

CK_RV prvCreatePublicKey( CK_ATTRIBUTE_PTR pxTemplate,
                          CK_ULONG ulCount,
                          CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_KEY_TYPE xKeyType;
    CK_RV xResult = CKR_OK;
    CK_ATTRIBUTE_PTR pxLabel = NULL;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;

    uint8_t pxPublicKey[ MAX_PUBLIC_KEY_SIZE ];
    uint32_t ulKeySize = MAX_PUBLIC_KEY_SIZE;

    xKeyType = prvGetKeyType( pxTemplate, ulCount );

    if( ( xKeyType == CKK_EC ) || ( xKeyType == CKK_ECDSA ) )
    {
        prvGetLabel( &pxLabel, pxTemplate, ulCount );

        xResult = prvCreateECPublicKey( pxPublicKey, &ulKeySize, &pxLabel, pxTemplate, ulCount, pxObject );
    }
    else
    {
        PKCS11_PRINT( ( "Invalid key type %d \r\n", xKeyType ) );
        xResult = CKR_TEMPLATE_INCONSISTENT;
    }

    if( xResult == CKR_OK )
    {
        xPalHandle = PKCS11_PAL_SaveObject( pxLabel, pxPublicKey, ulKeySize );

        if( xPalHandle == CK_INVALID_HANDLE )
        {
            xResult = CKR_DEVICE_MEMORY;
        }
    }

    if( xResult == CKR_OK )
    {
        xResult = prvAddObjectToList( xPalHandle, pxObject, pxLabel->pValue, pxLabel->ulValueLen );
    }

    return xResult;
}

CK_RV prvActivatePrivateKey( CK_ATTRIBUTE_PTR pxTemplate,
                             CK_ULONG ulCount,
                             CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_KEY_TYPE xKeyType;
    CK_RV xResult = CKR_OK;
    CK_ATTRIBUTE_PTR pxLabel = NULL;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;

    uint8_t pxPublicKey[ MAX_PUBLIC_KEY_SIZE ];
    uint32_t ulKeySize = MAX_PUBLIC_KEY_SIZE;

    xKeyType = prvGetKeyType( pxTemplate, ulCount );

    if( ( xKeyType == CKK_EC ) || ( xKeyType == CKK_ECDSA ) )
    {
        prvGetLabel( &pxLabel, pxTemplate, ulCount );

        xResult = prvCreateECPublicKey( pxPublicKey, &ulKeySize, &pxLabel, pxTemplate, ulCount, pxObject );
    }
    else
    {
        PKCS11_PRINT( ( "Invalid key type %d \r\n", xKeyType ) );
        xResult = CKR_TEMPLATE_INCONSISTENT;
    }

    if( xResult == CKR_OK )
    {
        xPalHandle = PKCS11_PAL_SaveObject( pxLabel, pxPublicKey, ulKeySize );

        if( xPalHandle == CK_INVALID_HANDLE )
        {
            xResult = CKR_DEVICE_MEMORY;
        }
    }

    if( xResult == CKR_OK )
    {
        xResult = prvAddObjectToList( xPalHandle, pxObject, pxLabel->pValue, pxLabel->ulValueLen );
    }

    return xResult;
}


/**
 * @brief Provides import and storage of a single client certificate.
 */
CK_DEFINE_FUNCTION( CK_RV, C_CreateObject )( CK_SESSION_HANDLE xSession,
                                             CK_ATTRIBUTE_PTR pxTemplate,
                                             CK_ULONG ulCount,
                                             CK_OBJECT_HANDLE_PTR pxObject )
{ /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    CK_OBJECT_CLASS xClass;

    /* Avoid warnings about unused parameters. */
    ( void ) xSession;

    /*
     * Check parameters.
     */
    if( ( NULL == pxTemplate ) ||
        ( NULL == pxObject ) ||
        ( CKR_OK != xResult ) )
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
                xResult = CKR_ATTRIBUTE_VALUE_INVALID;
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
 * @brief Free resources attached to an object handle.
 */
CK_DEFINE_FUNCTION( CK_RV, C_DestroyObject )( CK_SESSION_HANDLE xSession,
                                              CK_OBJECT_HANDLE xObject )
{
    CK_RV xResult;

    ( void ) xSession;
    ( void ) xObject;



    xResult = PKCS11_PAL_DestroyObject( xObject );

    return xResult;
}

/**
 * @brief Query the value of the specified cryptographic object attribute.
 * Regarding keys, only ECDSA P256 is supported by this implementation.
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
    CK_KEY_TYPE xPkcsKeyType = ( CK_KEY_TYPE ) ~0;
    CK_OBJECT_CLASS xClass;
    uint8_t * pxObjectValue = NULL;
    uint32_t ulLength = 0;
    uint8_t ucP256Oid[] = pkcs11DER_ENCODED_OID_P256;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    size_t xSize;
    uint8_t * pcLabel = NULL;

    /* Avoid warnings about unused parameters. */
    ( void ) xSession;

    if( ( CKR_OK != xResult ) || ( NULL == pxTemplate ) || ( 0 == ulCount ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        /*
         * Copy the object into a buffer.
         */

        prvFindObjectInListByHandle( xObject, &xPalHandle, &pcLabel, &xSize ); /*pcLabel and xSize are ignored. */

        if( ( xPalHandle != CK_INVALID_HANDLE ) && ( xPalHandle != eAwsDevicePrivateKey ) && ( xPalHandle != eAwsTestPrivateKey ) )
        {
            xResult = PKCS11_PAL_GetObjectValue( xPalHandle, &pxObjectValue, &ulLength, &xIsPrivate );
        }
        else if( xPalHandle == CK_INVALID_HANDLE )
        {
            xResult = CKR_DATA_INVALID;
        }
    }

    if( CKR_OK != xResult )
    {
        xResult = CKR_DATA_INVALID;
    }
    else
    {
        switch( xPalHandle )
        {
            case eAwsDevicePrivateKey:
            case eAwsTestPrivateKey:
                xClass = CKO_PRIVATE_KEY;
                break;

            case eAwsDevicePublicKey:
            case eAwsTestPublicKey:
                xClass = CKO_PUBLIC_KEY;
                break;

            case eAwsDeviceCertificate:
            case eAwsTestCertificate:
                xClass = CKO_CERTIFICATE;
                break;

            default:
                xResult = CKR_DATA_INVALID;
                break;
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
                            pxTemplate[ iAttrib ].ulValueLen = ulLength;
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
                            /* Elliptic-curve is the only asymmetric cryptosystem
                             * supported by this implementation. */
                            xPkcsKeyType = CKK_EC;

                            memcpy( pxTemplate[ iAttrib ].pValue, &xPkcsKeyType, sizeof( CK_KEY_TYPE ) );
                        }
                    }

                    break;

                case CKA_EC_PARAMS:

                    /* P256 is the only elliptic-curve supported by this
                     * implementation. */
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
                        pxTemplate[ iAttrib ].ulValueLen = 67; /* TODO: Is this large enough?*/
                    }
                    else
                    {
                        if( pxTemplate[ iAttrib ].ulValueLen < ulLength )
                        {
                            xResult = CKR_BUFFER_TOO_SMALL;
                        }
                        else
                        {
                            memcpy( ( uint8_t * ) pxTemplate[ iAttrib ].pValue, ( uint8_t * ) pxObjectValue, ulLength );
                            pxTemplate[ iAttrib ].ulValueLen = ulLength;
                        }
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
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    CK_BYTE * pxFindObjectLabel = NULL;
    uint32_t ulIndex;
    CK_ATTRIBUTE xAttribute;

    /* Check inputs. */
    if( ( pxSession == NULL ) || ( pxSession->xOpened != CK_TRUE ) )
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
        PKCS11_PRINT( ( "ERROR: Invalid session. \r\n" ) );
    }
    else if( pxSession->pxFindObjectLabel != NULL )
    {
        xResult = CKR_OPERATION_ACTIVE;
        PKCS11_PRINT( ( "ERROR: Find object operation already in progress. \r\n" ) );
    }
    else if( NULL == pxTemplate )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else if( ( ulCount != 1 ) && ( ulCount != 2 ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
        PKCS11_PRINT( ( "ERROR: Find objects does not support searching by %d attributes. \r\n", ulCount ) );
    }

    /* Malloc space to save template information. */
    if( xResult == CKR_OK )
    {
        pxFindObjectLabel = pvPortMalloc( pxTemplate->ulValueLen + 1 ); /* Add 1 to guarantee null termination for PAL. */
        pxSession->pxFindObjectLabel = pxFindObjectLabel;

        if( pxFindObjectLabel != NULL )
        {
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
 * @brief Query the objects of the requested type.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjects )( CK_SESSION_HANDLE xSession,
                                            CK_OBJECT_HANDLE_PTR pxObject,
                                            CK_ULONG ulMaxObjectCount,
                                            CK_ULONG_PTR pulObjectCount )
{ /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    BaseType_t xDone = pdFALSE;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    CK_BYTE_PTR pcObjectValue = NULL;
    uint32_t xObjectLength = 0;
    CK_BBOOL xIsPrivate = CK_TRUE;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    uint32_t ulIndex;

    /*
     * Check parameters.
     */
    if( ( CKR_OK != xResult ) ||
        ( NULL == pxObject ) ||
        ( NULL == pulObjectCount ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
        xDone = pdTRUE;
    }

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

    if( ( pdFALSE == xDone ) )
    {
        /* Try to find the object in module's list first. */
        prvFindObjectInListByLabel( pxSession->pxFindObjectLabel, strlen( ( const char * ) pxSession->pxFindObjectLabel ), &xPalHandle, pxObject );

        /* Check with the PAL if the object was previously stored. */
        if( *pxObject == CK_INVALID_HANDLE )
        {
            xPalHandle = PKCS11_PAL_FindObject( pxSession->pxFindObjectLabel, ( uint8_t ) strlen( ( const char * ) pxSession->pxFindObjectLabel ) );
        }

        /* Query for the object contents, but only if it's not a private key. */
        if( ( xPalHandle != CK_INVALID_HANDLE ) &&
            ( xPalHandle != eAwsDevicePrivateKey ) &&
            ( xPalHandle != eAwsTestPrivateKey ) )
        {
            xResult = PKCS11_PAL_GetObjectValue( xPalHandle, &pcObjectValue, &xObjectLength, &xIsPrivate );

            if( xResult == CKR_OK )
            {
                /* Deleted objects are overwritten with zero. Assume that a
                 * non-zero byte implies a valid object. */
                for( ulIndex = 0; ulIndex < xObjectLength; ulIndex++ )
                {
                    if( 0 != pcObjectValue[ ulIndex ] )
                    {
                        break;
                    }
                }

                if( xObjectLength == ulIndex )
                {
                    *pxObject = CK_INVALID_HANDLE;
                }
                else
                {
                    xResult = prvAddObjectToList( xPalHandle, pxObject, pxSession->pxFindObjectLabel, strlen( ( const char * ) pxSession->pxFindObjectLabel ) );
                    *pulObjectCount = 1;
                }

                PKCS11_PAL_GetObjectValueCleanup( pcObjectValue, xObjectLength );
            }
            else
            {
                /* The object handle is recognized but its contents don't exist
                 * in storage. */
                *pxObject = CK_INVALID_HANDLE;
                xResult = CKR_OK;
            }
        }
        else if( ( xPalHandle == eAwsDevicePrivateKey ) || ( xPalHandle == eAwsTestPrivateKey ) )
        {
            *pulObjectCount = 1;
            xResult = CKR_OK;
        }
        else
        {
            /* If FindObject doesn't see an object, return CKR_OK with an invalid handle and an object count of 0. */
            PKCS11_WARNING_PRINT( ( "ERROR: Object with label '%s' not found. \r\n", ( char * ) pxSession->pxFindObjectLabel ) );
            *pxObject = CK_INVALID_HANDLE;
        }
    }

    return xResult;
}

/**
 * @brief Terminate object enumeration.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsFinal )( CK_SESSION_HANDLE xSession )
{ /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    /*
     * Check parameters.
     */
    if( pxSession->xOpened != CK_TRUE )
    {
        xResult = CKR_SESSION_HANDLE_INVALID;
    }

    if( pxSession->pxFindObjectLabel == NULL )
    {
        xResult = CKR_OPERATION_NOT_INITIALIZED;
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
        pxSession->xSHA256Context.xHashCtx.context_buffer = pxSession->xSHA256Context.pubHashCtxBuf;
        pxSession->xSHA256Context.xHashCtx.context_buffer_length = sizeof( pxSession->xSHA256Context.pubHashCtxBuf );
        pxSession->xSHA256Context.xHashCtx.hash_algo = OPTIGA_HASH_TYPE_SHA_256;

        /*Hash start */
        if( OPTIGA_LIB_SUCCESS != optiga_crypt_hash_start( &pxSession->xSHA256Context.xHashCtx ) )
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
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );
    hash_data_from_host_t xHashDataHost;

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
        xHashDataHost.buffer = pPart;
        xHashDataHost.length = ulPartLen;

        if( OPTIGA_LIB_SUCCESS != optiga_crypt_hash_update( &pxSession->xSHA256Context.xHashCtx,
                                                            OPTIGA_CRYPT_HOST_DATA,
                                                            &xHashDataHost ) )
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
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
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
                /* hash finalize */
                if( OPTIGA_LIB_SUCCESS != optiga_crypt_hash_finalize(
                        &pxSession->xSHA256Context.xHashCtx,
                        pDigest ) )
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

    /*lint !e9072 It's OK to have different parameter name. */
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );
    long lOptigaOid = 0;
    char * xEnd = NULL;

    if( NULL == pxMechanism )
    {
        PKCS11_PRINT( ( "ERROR: Null signing mechanism provided. \r\n" ) );
        xResult = CKR_ARGUMENTS_BAD;
    }

    /* Retrieve key value from storage. */
    if( xResult == CKR_OK )
    {
        prvFindObjectInListByHandle( xKey,
                                     &xPalHandle,
                                     &pxLabel,
                                     &xLabelLength );

        if( xPalHandle != CK_INVALID_HANDLE )
        {
            lOptigaOid = strtol( ( char * ) pxLabel, &xEnd, 16 );

            if( ( 0 != lOptigaOid ) && ( USHRT_MAX >= lOptigaOid ) )
            {
                pxSession->usSignKeyOid = ( uint16_t ) lOptigaOid;
            }
            else
            {
                PKCS11_PRINT( ( "ERROR: Unable to retrieve value of private key for signing %d. \r\n", xResult ) );
            }
        }
        else
        {
            xResult = CKR_KEY_HANDLE_INVALID;
        }
    }

    /* Check that the mechanism and key type are compatible, supported. */
    if( xResult == CKR_OK )
    {
        if( pxMechanism->mechanism != CKM_ECDSA )
        {
            PKCS11_PRINT( ( "ERROR: Unsupported mechanism type %d \r\n", pxMechanism->mechanism ) );
            xResult = CKR_MECHANISM_INVALID;
        }
        else
        {
            pxSession->xSignMechanism = pxMechanism->mechanism;
        }
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
{ /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );
    CK_ULONG xSignatureLength = 0;
    CK_ULONG xExpectedInputLength = 0;
    /* Signature Length + 3x2 bytes reserved for DER tags */
    uint8_t ecSignature[ pkcs11ECDSA_P256_SIGNATURE_LENGTH + 3 + 3 ];
    uint16_t ecSignatureLength = sizeof( ecSignature );

    if( NULL == pulSignatureLen )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( CKR_OK == xResult )
    {
        /* Update the signature length. */
        xSignatureLength = pkcs11ECDSA_P256_SIGNATURE_LENGTH;
        xExpectedInputLength = pkcs11SHA256_DIGEST_LENGTH;
    }

    if( xResult == CKR_OK )
    {
        /* Calling application is trying to determine length needed for signature buffer. */
        if( NULL == pucSignature )
        {
            *pulSignatureLen = xSignatureLength;
        }
        else
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
                if( 0 != pxSession->usSignKeyOid )
                {
                    /*
                     * An example of a returned signature
                     * 0x000000: 02 20 38 0f 56 c8 90 53 18 9d 8f 58 b4 46 35 a0 . 8.V..S...X.F5.
                     * 0x000010: d7 07 63 ef 9f a2 30 64 93 e4 3d bf 7b db 57 a1 ..c...0d..=.{.W.
                     * 0x000020: b6 d7 02 20 4f 5e 3a db 6b 1a eb ac 66 9a 15 69 ... O^:.k...f..i
                     * 0x000030: 0d 7d 46 5b 44 72 40 06 a5 7b 06 84 0f d7 6e 0f .}F[Dr@..{....n.
                     * 0x000040: 4b 45 7f 50                                     KE.P
                     */
                    if( OPTIGA_LIB_SUCCESS != optiga_crypt_ecdsa_sign( pucData,
                                                                       xExpectedInputLength,
                                                                       pxSession->usSignKeyOid,
                                                                       ecSignature,
                                                                       &ecSignatureLength ) )
                    {
                        xResult = CKR_FUNCTION_FAILED;
                    }
                }
            }
        }
    }

    if( xResult == CKR_OK )
    {
        /* Reformat from DER encoded to 64-byte R & S components */
        asn1_to_ecdsa_rs( ecSignature, ecSignatureLength, pucSignature, 64 );
    }

    if( ( xResult == CKR_OK ) || ( xResult == CKR_BUFFER_TOO_SMALL ) )
    {
        *pulSignatureLen = 64;
    }

    /* Complete the operation in the context. */
    if( xResult != CKR_BUFFER_TOO_SMALL )
    {
        pxSession->xSignMechanism = pkcs11NO_OPERATION;
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
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession;
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    uint8_t * pxLabel = NULL;
    size_t xLabelLength = 0;
    CK_LONG lOptigaOid = 0;
    char * xEnd = NULL;

    /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( xSession );

    pxSession = prvSessionPointerFromHandle( xSession );

    if( NULL == pxMechanism )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    /* Retrieve key value from storage. */
    if( xResult == CKR_OK )
    {
        prvFindObjectInListByHandle( xKey, &xPalHandle, &pxLabel, &xLabelLength );

        if( xPalHandle != CK_INVALID_HANDLE )
        {
            lOptigaOid = strtol( ( char * ) pxLabel, &xEnd, 16 );

            if( ( 0 != lOptigaOid ) && ( USHRT_MAX >= lOptigaOid ) )
            {
                pxSession->usVerifyKeyOid = ( uint16_t ) lOptigaOid;
            }
            else
            {
                PKCS11_PRINT( ( "ERROR: Unable to retrieve value of private key for signing %d. \r\n", xResult ) );
            }
        }
        else
        {
            xResult = CKR_KEY_HANDLE_INVALID;
        }
    }

    /* Check for a supported crypto algorithm. */
    if( pxMechanism->mechanism == CKM_ECDSA )
    {
        pxSession->xVerifyMechanism = pxMechanism->mechanism;
    }
    else
    {
        xResult = CKR_MECHANISM_INVALID;
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
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession;
    /* (R component 32 bytes) + (S component 32 bytes) + DER tags 3 bytes max each*/
    CK_BYTE pubASN1Signature[ 0x40 + 0x03 + 0x03 ];
    CK_ULONG pubASN1SignatureLength = sizeof( pubASN1Signature );

    pxSession = prvSessionPointerFromHandle( xSession ); /*lint !e9072 It's OK to have different parameter name. */

    /* Check parameters. */
    if( ( NULL == pucData ) ||
        ( NULL == pucSignature ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    /* Check that the signature and data are the expected length.
     * These PKCS #11 mechanism expect data to be pre-hashed/formatted. */
    if( xResult == CKR_OK )
    {
        if( pxSession->xVerifyMechanism == CKM_ECDSA )
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

    /* Verification step. */
    if( xResult == CKR_OK )
    {
        /* Perform an ECDSA verification. */
        if( !ecdsa_rs_to_asn1_integers( &pucSignature[ 0 ], &pucSignature[ 32 ], 32,
                                        pubASN1Signature, ( size_t * ) &pubASN1SignatureLength ) )
        {
            xResult = CKR_SIGNATURE_INVALID;
            PKCS11_PRINT( ( "Failed to parse EC signature \r\n" ) );
        }

        if( xResult == CKR_OK )
        {
            uint8_t temp[ 100 ];
            uint16_t tempLen = 100;

            if( OPTIGA_LIB_SUCCESS != optiga_util_read_data( pxSession->usVerifyKeyOid, 2, &temp[ 3 ], &tempLen ) )
            {
                xResult = CKR_SIGNATURE_INVALID;
                PKCS11_PRINT( ( "Failed to extract the Public Key from the SE\r\n" ) );
            }

            /* Return tags (e.g. 0x03, 0x42, 0x00) to the public key buffer */
            temp[ 0 ] = 0x03;
            temp[ 1 ] = 0x42;
            temp[ 2 ] = 0x00;
            tempLen += 3;

            public_key_from_host_t xPublicKeyDetails =
            {
                temp,
                tempLen,
                OPTIGA_ECC_NIST_P_256
            };

            if( OPTIGA_LIB_SUCCESS != optiga_crypt_ecdsa_verify( pucData, ulDataLen,
                                                                 pubASN1Signature, pubASN1SignatureLength,
                                                                 OPTIGA_CRYPT_HOST_DATA, &xPublicKeyDetails ) )
            {
                xResult = CKR_SIGNATURE_INVALID;
                PKCS11_PRINT( ( "Failed to verify the EC signature\r\n" ) );
            }
        }
    }

    /* Return the signature verification result. */
    return xResult;
}



CK_RV prvCheckGenerateKeyPairPrivateTemplate( CK_ATTRIBUTE_PTR * ppxLabel,
                                              CK_ATTRIBUTE_PTR pxTemplate,
                                              CK_ULONG ulTemplateLength )
{
#define LABEL      ( 1U )
#define PRIVATE    ( 1U << 1 )
#define SIGN       ( 1U << 2 )

    CK_ATTRIBUTE xAttribute;
    CK_RV xResult = CKR_OK;
    CK_BBOOL xBool;
    CK_ULONG xTemp;
    CK_ULONG xIndex;
    uint32_t xAttributeMap = 0;
    uint32_t xRequiredAttributeMap = ( LABEL | PRIVATE | SIGN );

    for( xIndex = 0; xIndex < ulTemplateLength; xIndex++ )
    {
        xAttribute = pxTemplate[ xIndex ];

        switch( xAttribute.type )
        {
            case ( CKA_LABEL ):
                *ppxLabel = &pxTemplate[ xIndex ];
                xAttributeMap |= LABEL;
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

                xAttributeMap |= PRIVATE;
                break;

            case ( CKA_SIGN ):
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Generating private keys that cannot sign is not supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
                }

                xAttributeMap |= SIGN;
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

CK_RV prvCheckGenerateKeyPairPublicTemplate( CK_ATTRIBUTE_PTR * ppxLabel,
                                             CK_ATTRIBUTE_PTR pxTemplate,
                                             CK_ULONG ulTemplateLength )
{
#define LABEL        ( 1U )
#define EC_PARAMS    ( 1U << 1 )
#define VERIFY       ( 1U << 2 )

    CK_ATTRIBUTE xAttribute;
    CK_RV xResult = CKR_OK;
    CK_BBOOL xBool;
    CK_KEY_TYPE xKeyType;
    CK_BYTE xEcParams[] = pkcs11DER_ENCODED_OID_P256;
    int lCompare;
    CK_ULONG ulIndex;
    uint32_t xAttributeMap = 0;
    uint32_t xRequiredAttributeMap = ( LABEL | EC_PARAMS | VERIFY );

    for( ulIndex = 0; ulIndex < ulTemplateLength; ulIndex++ )
    {
        xAttribute = pxTemplate[ ulIndex ];

        switch( xAttribute.type )
        {
            case ( CKA_LABEL ):

                *ppxLabel = &pxTemplate[ ulIndex ];
                xAttributeMap |= LABEL;
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

                xAttributeMap |= EC_PARAMS;
                break;

            case ( CKA_VERIFY ):
                memcpy( &xBool, xAttribute.pValue, sizeof( CK_BBOOL ) );

                if( xBool != CK_TRUE )
                {
                    PKCS11_PRINT( ( "ERROR: Generating public keys that cannot verify is not supported. \r\n" ) );
                    xResult = CKR_TEMPLATE_INCONSISTENT;
                }

                xAttributeMap |= VERIFY;
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
    uint8_t * pucPublicKeyDer = NULL;
    uint16_t ucPublicKeyDerLength = 0;
    CK_ATTRIBUTE_PTR pxPrivateLabel = NULL;
    CK_ATTRIBUTE_PTR pxPublicLabel = NULL;
    CK_OBJECT_HANDLE xPalPublic = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE xPalPrivate = CK_INVALID_HANDLE;
    char * xEnd = NULL;
    long lOptigaOid = 0;

#define ECDSA_P256_UNCOMPRESSED_POINTS_LENGTH    65
    uint8_t pucEcPointPrefix[] = { 0x04, 0x41 };
    uint8_t pucUnusedKeyTag[] = { 0x03, 0x42, 0x00 };

    if( CKM_EC_KEY_PAIR_GEN != pxMechanism->mechanism )
    {
        xResult = CKR_MECHANISM_PARAM_INVALID;
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
        ucPublicKeyDerLength = ECDSA_P256_UNCOMPRESSED_POINTS_LENGTH + sizeof( pucUnusedKeyTag );
        pucPublicKeyDer = pvPortMalloc( ucPublicKeyDerLength );

        if( pucPublicKeyDer == NULL )
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    if( ( xResult == CKR_OK ) )
    {
        lOptigaOid = strtol( ( char * ) pxPrivateLabel->pValue, &xEnd, 16 );

        if( ( 0 != lOptigaOid ) && ( USHRT_MAX >= lOptigaOid ) )
        {
            /* For the public key, the OPTIGA library will return the standard 65
             * bytes of uncompressed curve points plus a 3-byte tag. The latter will
             * be intentionally overwritten below. */
            if( OPTIGA_LIB_SUCCESS != optiga_crypt_ecc_generate_keypair( OPTIGA_ECC_NIST_P_256,
                                                                         ( uint8_t ) OPTIGA_KEY_USAGE_SIGN,
                                                                         FALSE,
                                                                         &lOptigaOid,
                                                                         pucPublicKeyDer,
                                                                         &ucPublicKeyDerLength ) )
            {
                PKCS11_PRINT( ( "ERROR: Failed to generate a keypair \r\n" ) );
                xResult = CKR_FUNCTION_FAILED;
            }
        }
        else
        {
            xResult = CKR_FUNCTION_FAILED;
        }
    }

    if( xResult == CKR_OK )
    {
        /* Complete the encoding of the public key. */
        memcpy( pucPublicKeyDer + 1, pucEcPointPrefix, sizeof( pucEcPointPrefix ) );
        xPalPublic = PKCS11_PAL_SaveObject( pxPublicLabel,
                                            ( unsigned char * ) pucPublicKeyDer + 1,
                                            ucPublicKeyDerLength - 1 );
    }
    else
    {
        xResult = CKR_GENERAL_ERROR;
    }

    if( xResult == CKR_OK )
    {
        /* This is a trick to have a handle to store */
        xPalPrivate = PKCS11_PAL_SaveObject( pxPrivateLabel, NULL, 0 );
    }
    else
    {
        xResult = CKR_GENERAL_ERROR;
    }

    if( ( xPalPublic != CK_INVALID_HANDLE ) )
    {
        xResult = prvAddObjectToList( xPalPrivate, pxPrivateKey, pxPrivateLabel->pValue, pxPrivateLabel->ulValueLen );

        if( xResult == CKR_OK )
        {
            if( xResult == CKR_OK )
            {
                xResult = prvAddObjectToList( xPalPublic, pxPublicKey, pxPublicLabel->pValue, pxPublicLabel->ulValueLen );

                if( xResult != CKR_OK )
                {
                    PKCS11_PAL_DestroyObject( *pxPrivateKey );
                }
            }
        }
    }

    /* Clean up. */
    if( NULL != pucPublicKeyDer )
    {
        vPortFree( pucPublicKeyDer );
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
    /* this is to truncate random numbers to the required length, as OPTIGA(TM) Trust can generate */
    /* values starting from 8 bytes */
    CK_BYTE xRandomBuf4SmallLengths[ 8 ];
    CK_ULONG xBuferSwitcherLength = ulRandomLen;
    CK_BYTE_PTR pxBufferSwitcher = pucRandomData;

    xResult = PKCS11_SESSION_VALID_AND_MODULE_INITIALIZED( xSession );

    if( xResult == CKR_OK )
    {
        if( ( NULL == pucRandomData ) ||
            ( ulRandomLen == 0 ) )
        {
            xResult = CKR_ARGUMENTS_BAD;
        }
        else
        {
            if( xBuferSwitcherLength < 8 )
            {
                pxBufferSwitcher = xRandomBuf4SmallLengths;
                xBuferSwitcherLength = 8;
            }

            if( OPTIGA_LIB_SUCCESS != optiga_crypt_random( OPTIGA_RNG_TYPE_TRNG,
                                                           pxBufferSwitcher,
                                                           xBuferSwitcherLength ) )
            {
                PKCS11_PRINT( ( "ERROR: Failed to generate a random value \r\n" ) );
                xResult = CKR_FUNCTION_FAILED;
            }

            if( pxBufferSwitcher == xRandomBuf4SmallLengths )
            {
                memcpy( pucRandomData, xRandomBuf4SmallLengths, ulRandomLen );
            }
        }
    }

    return xResult;
}
