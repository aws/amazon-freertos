/*
 * FreeRTOS PKCS #11 V2.0.3
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

/* C runtime includes. */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "semphr.h"

/* Mock includes. */
#include "mock_iot_crypto.h"
#include "mock_queue.h"
#include "mock_ctr_drbg.h"
#include "mock_entropy.h"
#include "mock_sha256.h"
#include "mock_pk.h"
#include "mock_ecp.h"
#include "mock_rsa.h"
#include "mock_bignum.h"
#include "mock_portable.h"
#include "mock_iot_pki_utils.h"

/* PKCS #11 includes. */
#include "iot_pkcs11_config.h"
#include "iot_pkcs11.h"
/* This mock must be included after pkcs11.h */
#include "mock_iot_pkcs11_pal.h"

/* mbedtls includes. */
#include "oid.h"

/* Test includes. */
#include "unity.h"


/* ============================  GLOBAL VARIABLES =========================== */
/* Length parameters for importing EC private keys. */
#define EC_PARAMS_LENGTH    10
#define EC_D_LENGTH         32

#define EC_PRIV_KEY_INITIALIZER                                                    \
    {                                                                              \
        { CKA_CLASS, &xPrivateKeyClass, sizeof( CK_OBJECT_CLASS ) },               \
        { CKA_KEY_TYPE, &xPrivateKeyType, sizeof( CK_KEY_TYPE ) },                 \
        { CKA_LABEL, pucLabel, ( CK_ULONG ) strlen( ( const char * ) pucLabel ) }, \
        { CKA_TOKEN, &xTrue, sizeof( CK_BBOOL ) },                                 \
        { CKA_SIGN, &xTrue, sizeof( CK_BBOOL ) },                                  \
        { CKA_EC_PARAMS, pxEcParams, EC_PARAMS_LENGTH },                           \
        { CKA_VALUE, pxD, EC_D_LENGTH }                                            \
    }

#define EC_PUB_KEY_INITIALIZER                                       \
    {                                                                \
        { CKA_CLASS, &xPublicKeyClass, sizeof( xPublicKeyClass ) },  \
        { CKA_KEY_TYPE, &xPublicKeyType, sizeof( xPublicKeyType ) }, \
        { CKA_TOKEN, &xTrue, sizeof( xTrue ) },                      \
        { CKA_VERIFY, &xTrue, sizeof( xTrue ) },                     \
        { CKA_EC_PARAMS, pxEcParams, sizeof( pxEcParams ) },         \
        { CKA_EC_POINT, pxEcPoint, xLength + 2 },                    \
        { CKA_LABEL, pucLabel, strlen( ( const char * ) pucLabel ) } \
    }

/* Length parameters for importing RSA-2048 private keys. */
#define MODULUS_LENGTH        pkcs11RSA_2048_MODULUS_BITS / 8
#define E_LENGTH              3
#define D_LENGTH              pkcs11RSA_2048_MODULUS_BITS / 8
#define PRIME_1_LENGTH        128
#define PRIME_2_LENGTH        128
#define EXPONENT_1_LENGTH     128
#define EXPONENT_2_LENGTH     128
#define COEFFICIENT_LENGTH    128

#define RSA_PRIV_KEY_INITIALIZER                                                   \
    {                                                                              \
        { CKA_CLASS, &xPrivateKeyClass, sizeof( CK_OBJECT_CLASS ) },               \
        { CKA_KEY_TYPE, &xPrivateKeyType, sizeof( CK_KEY_TYPE ) },                 \
        { CKA_LABEL, pucLabel, ( CK_ULONG ) strlen( ( const char * ) pucLabel ) }, \
        { CKA_TOKEN, &xTrue, sizeof( CK_BBOOL ) },                                 \
        { CKA_SIGN, &xTrue, sizeof( CK_BBOOL ) },                                  \
        { CKA_MODULUS, xRsaParams.modulus + 1, MODULUS_LENGTH },                   \
        { CKA_PUBLIC_EXPONENT, xRsaParams.e + 1, E_LENGTH },                       \
        { CKA_PRIME_1, xRsaParams.prime1 + 1, PRIME_1_LENGTH },                    \
        { CKA_PRIME_2, xRsaParams.prime2 + 1, PRIME_2_LENGTH },                    \
        { CKA_EXPONENT_1, xRsaParams.exponent1 + 1, EXPONENT_1_LENGTH },           \
        { CKA_EXPONENT_2, xRsaParams.exponent2 + 1, EXPONENT_2_LENGTH },           \
        { CKA_COEFFICIENT, xRsaParams.coefficient + 1, COEFFICIENT_LENGTH }        \
    }

#define CERT_INITIALIZER                                                              \
    {                                                                                 \
        { CKA_CLASS, &xCertificateClass, sizeof( xCertificateClass ) },               \
        { CKA_SUBJECT, xSubject, strlen( ( const char * ) xSubject ) },               \
        { CKA_VALUE, ( CK_VOID_PTR ) xCert, ( CK_ULONG ) sizeof( xCert ) },           \
        { CKA_LABEL, ( CK_VOID_PTR ) pucLabel, strlen( ( const char * ) pucLabel ) }, \
        { CKA_CERTIFICATE_TYPE, &xCertificateType, sizeof( CK_CERTIFICATE_TYPE ) },   \
        { CKA_TOKEN, &xTokenStorage, sizeof( xTokenStorage ) }                        \
    }

/* Malloc calls */
static uint16_t usMallocFreeCalls = 0;


/* Internal struct to facilitate passing RSA params to mbedTLS. */

/* Adding one to all of the lengths because ASN1 may pad a leading 0 byte
 * to numbers that could be interpreted as negative */
typedef struct RsaParams_t
{
    CK_BYTE modulus[ MODULUS_LENGTH + 1 ];
    CK_BYTE e[ E_LENGTH + 1 ];
    CK_BYTE d[ D_LENGTH + 1 ];
    CK_BYTE prime1[ PRIME_1_LENGTH + 1 ];
    CK_BYTE prime2[ PRIME_2_LENGTH + 1 ];
    CK_BYTE exponent1[ EXPONENT_1_LENGTH + 1 ];
    CK_BYTE exponent2[ EXPONENT_2_LENGTH + 1 ];
    CK_BYTE coefficient[ COEFFICIENT_LENGTH + 1 ];
} RsaParams_t;

/* ==========================  CALLBACK FUNCTIONS =========================== */

/*!
 * @brief Wrapper stub for malloc.
 */
void * pvPkcs11MallocCb( size_t size,
                         int numCalls )
{
    usMallocFreeCalls++;
    return ( void * ) malloc( size );
}

/*!
 * @brief Wrapper stub for free.
 *
 */
void vPkcs11FreeCb( void * ptr,
                    int numCalls )
{
    usMallocFreeCalls--;
    free( ptr );
}

/* ============================   UNITY FIXTURES ============================ */
void setUp( void )
{
}

/* called before each testcase */
void tearDown( void )
{
    TEST_ASSERT_EQUAL_INT_MESSAGE( 0, usMallocFreeCalls,
                                   "free is not called the same number of times as malloc, \
        you might have a memory leak!!" );
    usMallocFreeCalls = 0;
}

/* called at the beginning of the whole suite */
void suiteSetUp()
{
}

/* called at the end of the whole suite */
int suiteTearDown( int numFailures )
{
    return numFailures;
}

/* ==========================  Helper functions  ============================ */

/*!
 * @brief Helper function to initialize PKCS #11.
 *
 */
static CK_RV prvInitializePkcs11()
{
    CK_RV xResult = CKR_OK;

    xQueueCreateMutex_IgnoreAndReturn( ( SemaphoreHandle_t ) 1 );
    CRYPTO_Init_Ignore();
    mbedtls_entropy_init_Ignore();
    mbedtls_ctr_drbg_init_Ignore();
    mbedtls_ctr_drbg_seed_IgnoreAndReturn( 0 );
    xResult = C_Initialize( NULL );

    return xResult;
}

/*!
 * @brief Helper function to initialize PKCS #11.
 *
 */
static CK_RV prvUninitializePkcs11()
{
    CK_RV xResult = CKR_OK;

    mbedtls_entropy_free_CMockIgnore();
    mbedtls_ctr_drbg_free_CMockIgnore();
    vQueueDelete_CMockIgnore();
    xResult = C_Finalize( NULL );

    return xResult;
}

/*!
 * @brief Helper function to create a PKCS #11 session.
 *
 */
static CK_RV prvOpenSession( CK_SESSION_HANDLE_PTR pxSession )
{
    CK_RV xResult = CKR_OK;
    CK_FLAGS xFlags = CKF_SERIAL_SESSION | CKF_RW_SESSION;

    pvPortMalloc_Stub( pvPkcs11MallocCb );
    xQueueCreateMutex_IgnoreAndReturn( ( SemaphoreHandle_t ) &xResult );
    xQueueCreateMutex_IgnoreAndReturn( ( SemaphoreHandle_t ) &xResult );
    xResult = C_OpenSession( 0, xFlags, NULL, 0, pxSession );

    return xResult;
}

/*!
 * @brief Helper function to close a PKCS #11 session.
 *
 */
static CK_RV prvCloseSession( CK_SESSION_HANDLE_PTR pxSession )
{
    CK_RV xResult = CKR_OK;

    vQueueDelete_Ignore();
    vQueueDelete_Ignore();
    vPortFree_Stub( vPkcs11FreeCb );
    mbedtls_sha256_free_CMockIgnore();
    xResult = C_CloseSession( *pxSession );

    return xResult;
}

/*!
 * @brief Helper function to create a x509 certificate.
 *
 */
static CK_RV prvCreateCert( CK_SESSION_HANDLE_PTR pxSession,
                            CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_RV xResult = CKR_OK;

    CK_OBJECT_CLASS xCertificateClass = CKO_CERTIFICATE;
    CK_CERTIFICATE_TYPE xCertificateType = CKC_X_509;
    CK_BBOOL xTokenStorage = CK_TRUE;
    CK_BYTE xSubject[] = "TestSubject";
    CK_BYTE xCert[] = "Empty Cert";
    char * pucLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
    PKCS11_CertificateTemplate_t xTemplate = CERT_INITIALIZER;

    /* Create Certificate. */
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    xResult = C_CreateObject( *pxSession,
                              ( CK_ATTRIBUTE_PTR ) &xTemplate,
                              sizeof( xTemplate ) / sizeof( CK_ATTRIBUTE ),
                              pxObject );
    return xResult;
}

/*!
 * @brief Helper function to create a EC Public Key.
 *
 */
static CK_RV prvCreateEcPub( CK_SESSION_HANDLE_PTR pxSession,
                             CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_RV xResult = CKR_OK;

    CK_KEY_TYPE xPublicKeyType = CKK_EC;
    CK_OBJECT_CLASS xPublicKeyClass = CKO_PUBLIC_KEY;
    CK_BBOOL xTrue = CK_TRUE;
    char * pucLabel = pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
    size_t xLength = 256;
    /* DER-encoding of an ANSI X9.62 Parameters value */
    CK_BYTE pxEcParams[] = pkcs11DER_ENCODED_OID_P256;
    CK_BYTE pxEcPoint[ 256 ] = { 0 };

    CK_ATTRIBUTE xTemplate[] = EC_PUB_KEY_INITIALIZER;

    /* Create  EC based public key. */
    mbedtls_pk_init_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    PKCS11_PAL_FindObject_IgnoreAndReturn( 1 );
    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 0 );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    mbedtls_ecp_keypair_init_CMockIgnore();
    mbedtls_ecp_group_init_CMockIgnore();
    mbedtls_ecp_group_load_IgnoreAndReturn( 0 );
    mbedtls_ecp_point_read_binary_IgnoreAndReturn( 0 );
    mbedtls_mpi_read_binary_IgnoreAndReturn( 0 );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    mbedtls_pk_write_pubkey_der_IgnoreAndReturn( 1 );
    mbedtls_pk_free_CMockIgnore();
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_CreateObject( *pxSession,
                              ( CK_ATTRIBUTE_PTR ) &xTemplate,
                              sizeof( xTemplate ) / sizeof( CK_ATTRIBUTE ),
                              pxObject );


    return xResult;
}

/* ======================  TESTING C_Initialize  ============================ */

/*!
 * @brief C_Initialize happy path.
 *
 */
void test_pkcs11_C_Initialize( void )
{
    CK_RV xResult = CKR_OK;

    xQueueCreateMutex_IgnoreAndReturn( ( SemaphoreHandle_t ) 1 );
    CRYPTO_Init_Ignore();
    mbedtls_entropy_init_Ignore();
    mbedtls_ctr_drbg_init_Ignore();
    mbedtls_ctr_drbg_seed_IgnoreAndReturn( 0 );
    xResult = C_Initialize( NULL );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief C_Initialize memory failure when creating a mutex.
 *
 */
void test_pkcs11_C_InitializeMemFail( void )
{
    CK_RV xResult = CKR_OK;

    xQueueCreateMutex_IgnoreAndReturn( NULL );
    CRYPTO_Init_Ignore();
    mbedtls_entropy_init_Ignore();
    mbedtls_ctr_drbg_init_Ignore();
    mbedtls_ctr_drbg_seed_IgnoreAndReturn( 0 );
    xResult = C_Initialize( NULL );

    TEST_ASSERT_EQUAL( CKR_HOST_MEMORY, xResult );
}

/*!
 * @brief C_Initialize failed to seed DRBG.
 *
 */
void test_pkcs11_C_InitializeSeedFail( void )
{
    CK_RV xResult = CKR_OK;

    xQueueCreateMutex_IgnoreAndReturn( ( SemaphoreHandle_t ) 1 );
    CRYPTO_Init_Ignore();
    mbedtls_entropy_init_Ignore();
    mbedtls_ctr_drbg_init_Ignore();
    mbedtls_ctr_drbg_seed_IgnoreAndReturn( 1 );
    xResult = C_Initialize( NULL );

    TEST_ASSERT_EQUAL( CKR_FUNCTION_FAILED, xResult );
}

/*!
 * @brief C_Initialize already initialized.
 *
 */
void test_pkcs11_C_InitializeInitTwice( void )
{
    CK_RV xResult = CKR_OK;

    xResult = prvInitializePkcs11();

    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = C_Initialize( NULL );
    TEST_ASSERT_EQUAL( CKR_CRYPTOKI_ALREADY_INITIALIZED, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/* ======================  TESTING C_Finalize  ============================ */

/*!
 * @brief C_Finalize initialize was not already called.
 *
 */
void test_pkcs11_C_FinalizeUninitialized( void )
{
    CK_RV xResult = CKR_OK;

    mbedtls_entropy_free_CMockIgnore();
    mbedtls_ctr_drbg_free_CMockIgnore();
    vQueueDelete_CMockIgnore();
    xResult = C_Finalize( NULL );
    TEST_ASSERT_EQUAL( CKR_CRYPTOKI_NOT_INITIALIZED, xResult );
}

/*!
 * @brief C_Finalize was called with a non-null pointer.
 *
 */
void test_pkcs11_C_FinalizeBadArgs( void )
{
    CK_RV xResult = CKR_OK;

    mbedtls_entropy_free_CMockIgnore();
    mbedtls_ctr_drbg_free_CMockIgnore();
    vQueueDelete_CMockIgnore();
    xResult = C_Finalize( ( CK_VOID_PTR ) &xResult );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/* ======================  TESTING C_GetFunctionList  ============================ */

/*!
 * @brief C_GetFunctionList happy path.
 *
 */
void test_pkcs11_C_GetFunctionList( void )
{
    CK_RV xResult = CKR_OK;
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;

    xResult = C_GetFunctionList( &pxFunctionList );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief C_GetFunctionList bad args.
 *
 */
void test_pkcs11_C_GetFunctionListBadArgs( void )
{
    CK_RV xResult = CKR_OK;

    xResult = C_GetFunctionList( NULL );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/* ======================  TESTING C_GetSlotList  ============================ */

/*!
 * @brief C_GetSlotList happy path.
 *
 */
void test_pkcs11_C_GetSlotList( void )
{
    CK_RV xResult = CKR_OK;
    CK_SLOT_ID xSlotId = 0;
    CK_ULONG xSlotCount = 1;
    CK_BBOOL xTokenPresent = CK_TRUE;

    xResult = prvInitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = C_GetSlotList( xTokenPresent, &xSlotId, &xSlotCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( 1, xSlotCount );

    xResult = C_GetSlotList( xTokenPresent, &xSlotId, &xSlotCount );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( 1, xSlotCount );
    TEST_ASSERT_EQUAL( 1, xSlotId );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief C_GetSlotList NULL SlotList.
 *
 */
void test_pkcs11_C_GetSlotListNullSlot( void )
{
    CK_RV xResult = CKR_OK;
    CK_ULONG xSlotCount = 0;
    CK_BBOOL xTokenPresent = CK_TRUE;

    xResult = prvInitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = C_GetSlotList( xTokenPresent, NULL, &xSlotCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( 1, xSlotCount );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief C_GetSlotList NULL SlotCount.
 *
 */
void test_pkcs11_C_GetSlotListNullCount( void )
{
    CK_RV xResult = CKR_OK;
    CK_BBOOL xTokenPresent = CK_TRUE;

    xResult = prvInitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = C_GetSlotList( xTokenPresent, NULL, NULL );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief C_GetSlotList 0 object count, valid slotlist.
 *
 */
void test_pkcs11_C_GetSlotListBadObjCount( void )
{
    CK_RV xResult = CKR_OK;
    CK_SLOT_ID xSlotId = 1;
    CK_ULONG xSlotCount = 0;
    CK_BBOOL xTokenPresent = CK_TRUE;

    xResult = prvInitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = C_GetSlotList( xTokenPresent, &xSlotId, &xSlotCount );

    TEST_ASSERT_EQUAL( CKR_BUFFER_TOO_SMALL, xResult );
    TEST_ASSERT_EQUAL( 0, xSlotCount );
    TEST_ASSERT_EQUAL( 1, xSlotId );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief C_GetSlotList PKCS #11 Not initialized.
 *
 */
void test_pkcs11_C_GetSlotListUninit( void )
{
    CK_RV xResult = CKR_OK;
    CK_SLOT_ID xSlotId = 1;
    CK_ULONG xSlotCount = 1;
    CK_BBOOL xTokenPresent = CK_TRUE;

    xResult = C_GetSlotList( xTokenPresent, &xSlotId, &xSlotCount );

    TEST_ASSERT_EQUAL( CKR_CRYPTOKI_NOT_INITIALIZED, xResult );
    TEST_ASSERT_EQUAL( 1, xSlotCount );
    TEST_ASSERT_EQUAL( 1, xSlotId );
}

/* ======================  TESTING C_GetTokenInfo  ============================ */

/*!
 * @brief C_GetTokenInfo Happy path.
 *
 * @note this test will need to be updated if this port needs to start returning
 * token information.
 *
 */
void test_pkcs11_C_GetTokenInfo( void )
{
    CK_RV xResult = CKR_OK;

    xResult = C_GetTokenInfo( 0, NULL );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}
/* ======================  TESTING C_GetMechanismInfo  ============================ */

/*!
 * @brief C_GetMechanismInfo happy path.
 *
 */
void test_pkcs11_C_GetMechanismInfo( void )
{
    CK_RV xResult = CKR_OK;
    CK_MECHANISM_TYPE xType = CKM_RSA_PKCS;
    CK_MECHANISM_INFO xInfo = { 0 };

    xResult = C_GetMechanismInfo( 0, xType, &xInfo );

    TEST_ASSERT_EQUAL( 2048, xInfo.ulMinKeySize );
    TEST_ASSERT_EQUAL( 2048, xInfo.ulMaxKeySize );
    TEST_ASSERT_EQUAL( CKF_SIGN, xInfo.flags );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xType = CKM_RSA_X_509;
    memset( &xInfo, 0, sizeof( CK_MECHANISM_INFO ) );
    xResult = C_GetMechanismInfo( 0, xType, &xInfo );

    TEST_ASSERT_EQUAL( 2048, xInfo.ulMinKeySize );
    TEST_ASSERT_EQUAL( 2048, xInfo.ulMaxKeySize );
    TEST_ASSERT_EQUAL( CKF_VERIFY, xInfo.flags );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    #if ( pkcs11configSUPPRESS_ECDSA_MECHANISM != 1 )
        xType = CKM_ECDSA;
        memset( &xInfo, 0, sizeof( CK_MECHANISM_INFO ) );
        xResult = C_GetMechanismInfo( 0, xType, &xInfo );

        TEST_ASSERT_EQUAL( 256, xInfo.ulMinKeySize );
        TEST_ASSERT_EQUAL( 256, xInfo.ulMaxKeySize );
        TEST_ASSERT_EQUAL( CKF_SIGN | CKF_VERIFY, xInfo.flags );
        TEST_ASSERT_EQUAL( CKR_OK, xResult );

        xType = CKM_EC_KEY_PAIR_GEN;
        memset( &xInfo, 0, sizeof( CK_MECHANISM_INFO ) );
        xResult = C_GetMechanismInfo( 0, xType, &xInfo );

        TEST_ASSERT_EQUAL( 256, xInfo.ulMinKeySize );
        TEST_ASSERT_EQUAL( 256, xInfo.ulMaxKeySize );
        TEST_ASSERT_EQUAL( CKF_GENERATE_KEY_PAIR, xInfo.flags );
        TEST_ASSERT_EQUAL( CKR_OK, xResult );
    #endif /* if ( pkcs11configSUPPRESS_ECDSA_MECHANISM != 1 ) */

    xType = CKM_SHA256;
    memset( &xInfo, 0, sizeof( CK_MECHANISM_INFO ) );
    xResult = C_GetMechanismInfo( 0, xType, &xInfo );

    TEST_ASSERT_EQUAL( 0, xInfo.ulMinKeySize );
    TEST_ASSERT_EQUAL( 0, xInfo.ulMaxKeySize );
    TEST_ASSERT_EQUAL( CKF_DIGEST, xInfo.flags );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief C_GetMechanismInfo NULL info pointer.
 *
 */
void test_pkcs11_C_GetMechanismInfoNullInfoPtr( void )
{
    CK_RV xResult = CKR_OK;
    CK_MECHANISM_TYPE xType = CKM_RSA_PKCS;

    xResult = C_GetMechanismInfo( 0, xType, NULL );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );
}

/*!
 * @brief C_GetMechanismInfo Bad Mechanism.
 *
 */
void test_pkcs11_C_GetMechanismInfoBadMech( void )
{
    CK_RV xResult = CKR_OK;
    CK_MECHANISM_TYPE xType = -1;
    CK_MECHANISM_INFO xInfo = { 0 };

    xResult = C_GetMechanismInfo( 0, xType, &xInfo );
    TEST_ASSERT_EQUAL( CKR_MECHANISM_INVALID, xResult );
}

/* ======================  TESTING C_InitToken  ============================ */

/*!
 * @brief C_InitToken Happy path.
 *
 * @note currently the port behaves like a fixed token, and doesn't do anything
 * when this function is called.
 *
 */
void test_pkcs11_C_InitToken( void )
{
    CK_RV xResult = CKR_OK;

    xResult = C_InitToken( 0, NULL, 0, NULL );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}
/* ======================  TESTING C_OpenSession  ============================ */

/*!
 * @brief C_OpenSession happy path.
 *
 */
void test_pkcs11_C_OpenSession( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_FLAGS xFlags = CKF_SERIAL_SESSION | CKF_RW_SESSION;

    xResult = prvInitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    pvPortMalloc_Stub( pvPkcs11MallocCb );
    xQueueCreateMutex_IgnoreAndReturn( ( SemaphoreHandle_t ) &xResult );
    xQueueCreateMutex_IgnoreAndReturn( ( SemaphoreHandle_t ) &xResult );
    xResult = C_OpenSession( 0, xFlags, NULL, 0, &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    vPkcs11FreeCb( ( void * ) xSession, 1 );
}

/*!
 * @brief C_OpenSession PKCS #11 Uninitialized.
 *
 */
void test_pkcs11_C_OpenSessionUninit( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_FLAGS xFlags = CKF_SERIAL_SESSION | CKF_RW_SESSION;

    xResult = C_OpenSession( 0, xFlags, NULL, 0, &xSession );
    TEST_ASSERT_EQUAL( CKR_CRYPTOKI_NOT_INITIALIZED, xResult );
}

/*!
 * @brief C_OpenSession bad args.
 *
 */
void test_pkcs11_C_OpenSessionBadArgs( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_FLAGS xFlags = CKF_SERIAL_SESSION | CKF_RW_SESSION;

    xResult = prvInitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = C_OpenSession( 0, xFlags, NULL, 0, NULL );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    xResult = C_OpenSession( 0, CKF_RW_SESSION, NULL, 0, &xSession );
    TEST_ASSERT_EQUAL( CKR_SESSION_PARALLEL_NOT_SUPPORTED, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/* ======================  TESTING C_OpenSession  ============================ */

/*!
 * @brief C_CloseSession happy path.
 *
 */
void test_pkcs11_C_CloseSession( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;

    xResult = prvInitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}
/* ======================  TESTING C_Login  ============================ */

/*!
 * @brief C_Login happy path.
 *
 * @note This test will need to be updated if support is added for C_Login.
 *
 */
void test_pkcs11_C_Login( void )
{
    CK_RV xResult = CKR_OK;

    xResult = C_Login( 0, 0, NULL, 0 );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/* ======================  TESTING C_CreateObject  ============================ */

/*!
 * @brief C_CreateObject Creating an EC private key happy path.
 *
 */
void test_pkcs11_C_CreateObjectECPrivKey( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_KEY_TYPE xPrivateKeyType = CKK_EC;
    CK_OBJECT_CLASS xPrivateKeyClass = CKO_PRIVATE_KEY;
    CK_BBOOL xTrue = CK_TRUE;
    char * pucLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
    /* DER-encoding of an ANSI X9.62 Parameters value */
    CK_BYTE * pxEcParams = ( CK_BYTE * ) ( "\x06\x08" MBEDTLS_OID_EC_GRP_SECP256R1 );
    CK_OBJECT_HANDLE xObject = 0;

    /* Private value D. */
    CK_BYTE pxD[ EC_D_LENGTH ] = { 0 };

    CK_ATTRIBUTE xPrivateKeyTemplate[] = EC_PRIV_KEY_INITIALIZER;

    xResult = prvInitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_pk_init_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    PKCS11_PAL_FindObject_IgnoreAndReturn( 1 );
    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    mbedtls_ecp_keypair_init_CMockIgnore();
    mbedtls_ecp_group_init_CMockIgnore();
    mbedtls_ecp_group_load_IgnoreAndReturn( 0 );
    mbedtls_mpi_read_binary_IgnoreAndReturn( 0 );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    mbedtls_pk_write_key_der_IgnoreAndReturn( 1 );
    mbedtls_pk_free_CMockIgnore();
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief C_CreateObject Creating an EC public key happy path.
 *
 */
void test_pkcs11_C_CreateObjectECPubKey( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_KEY_TYPE xPublicKeyType = CKK_EC;
    CK_OBJECT_CLASS xPublicKeyClass = CKO_PUBLIC_KEY;
    CK_BBOOL xTrue = CK_TRUE;
    char * pucLabel = pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
    size_t xLength = 256;
    /* DER-encoding of an ANSI X9.62 Parameters value */
    CK_BYTE pxEcParams[] = pkcs11DER_ENCODED_OID_P256;
    CK_OBJECT_HANDLE xObject = 0;

    CK_BYTE pxEcPoint[ 256 ] = { 0 };

    CK_ATTRIBUTE xPublicKeyTemplate[] = EC_PUB_KEY_INITIALIZER;

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_pk_init_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    PKCS11_PAL_FindObject_IgnoreAndReturn( 1 );
    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 0 );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    mbedtls_ecp_keypair_init_CMockIgnore();
    mbedtls_ecp_group_init_CMockIgnore();
    mbedtls_ecp_group_load_IgnoreAndReturn( 0 );
    mbedtls_ecp_point_read_binary_IgnoreAndReturn( 0 );
    mbedtls_mpi_read_binary_IgnoreAndReturn( 0 );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    mbedtls_pk_write_pubkey_der_IgnoreAndReturn( 1 );
    mbedtls_pk_free_CMockIgnore();
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                              sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*
 *!
 * @brief C_CreateObject Creating an RSA Private key happy path.
 *
 */
void test_pkcs11_C_CreateObjectRSAPrivKey( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_KEY_TYPE xPrivateKeyType = CKK_RSA;
    CK_OBJECT_CLASS xPrivateKeyClass = CKO_PRIVATE_KEY;
    CK_BBOOL xTrue = CK_TRUE;
    char * pucLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
    CK_OBJECT_HANDLE xObject = 0;

    RsaParams_t xRsaParams = { 0 };

    xResult = prvInitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    CK_ATTRIBUTE xPrivateKeyTemplate[] = RSA_PRIV_KEY_INITIALIZER;

    mbedtls_pk_init_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    mbedtls_rsa_init_CMockIgnore();
    mbedtls_rsa_import_raw_IgnoreAndReturn( 0 );
    mbedtls_mpi_read_binary_IgnoreAndReturn( 0 );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    mbedtls_pk_write_key_der_IgnoreAndReturn( 1 );
    mbedtls_pk_free_CMockIgnore();
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*
 *!
 * @brief C_CreateObject Creating an RSA Public key happy path.
 *
 * Note: This test will need to be updated should this port support RSA public keys.
 *
 */
void test_pkcs11_C_CreateObjectRSAPubKey( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_KEY_TYPE xPublicKeyType = CKK_RSA;
    CK_OBJECT_CLASS xPublicKeyClass = CKO_PUBLIC_KEY;
    CK_OBJECT_HANDLE xObject = 0;

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    CK_ATTRIBUTE xPublicKeyTemplate[] =
    {
        { CKA_CLASS,    &xPublicKeyClass, sizeof( CK_OBJECT_CLASS ) },
        { CKA_KEY_TYPE, &xPublicKeyType,  sizeof( CK_KEY_TYPE )     },
    };

    mbedtls_pk_init_CMockIgnore();
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                              sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_ATTRIBUTE_TYPE_INVALID, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*
 *!
 * @brief C_CreateObject Creating a Certificate happy path.
 *
 */
void test_pkcs11_C_CreateObjectCertificate( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    CK_OBJECT_CLASS xCertificateClass = CKO_CERTIFICATE;
    CK_CERTIFICATE_TYPE xCertificateType = CKC_X_509;
    CK_BBOOL xTokenStorage = CK_TRUE;
    CK_BYTE xSubject[] = "TestSubject";
    CK_BYTE xCert[] = "Empty Cert";
    char * pucLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;

    PKCS11_CertificateTemplate_t xCertificateTemplate = CERT_INITIALIZER;

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xCertificateTemplate,
                              sizeof( xCertificateTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}
/* ======================  TESTING C_GetAttributeValue  ============================ */

/*!
 * @brief C_GetAttributeValue get value of a certificate happy path.
 *
 */
void test_pkcs11_C_GetAttributeValueCert( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    CK_ULONG ulCount = 1;
    PKCS11_CertificateTemplate_t xCertificateTemplate = { 0 };

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCreateCert( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    /* Get Certificate value. */
    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    mbedtls_pk_init_CMockIgnore();
    mbedtls_pk_parse_key_IgnoreAndReturn( 1 );
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 1 );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_pk_free_CMockIgnore();
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xCertificateTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );


    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/* ======================  TESTING C_FindObjectsInit  ============================ */

/*!
 * @brief C_FindObjectsInit happy path.
 *
 */
void test_pkcs11_C_FindObjectsInit( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_ULONG ulCount = 1;
    CK_OBJECT_HANDLE xObject = 0;
    char * pucLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;

    CK_ATTRIBUTE xFindTemplate = { CKA_LABEL, pucLabel, strlen( ( const char * ) pucLabel ) };

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCreateCert( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    pvPortMalloc_Stub( pvPkcs11MallocCb );
    xResult = C_FindObjectsInit( xSession, ( CK_ATTRIBUTE_PTR ) &xFindTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    vPortFree_Stub( vPkcs11FreeCb );
    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/* ======================  TESTING C_FindObjects  ============================ */

/*!
 * @brief C_FindObjects happy path.
 *
 */
void test_pkcs11_C_FindObjects( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_ULONG ulCount = 1;
    CK_ULONG ulFoundCount = 0;
    CK_OBJECT_HANDLE xObject = 0;
    char * pucLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;

    CK_ATTRIBUTE xFindTemplate = { CKA_LABEL, pucLabel, strlen( ( const char * ) pucLabel ) };

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCreateCert( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    pvPortMalloc_Stub( pvPkcs11MallocCb );
    xResult = C_FindObjectsInit( xSession, ( CK_ATTRIBUTE_PTR ) &xFindTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_FindObject_IgnoreAndReturn( 1 );
    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    xResult = C_FindObjects( xSession, ( CK_OBJECT_HANDLE_PTR ) &xObject, 1, &ulFoundCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );


    vPortFree_Stub( vPkcs11FreeCb );
    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/* ======================  TESTING C_FindObjectsFinal  ============================ */

/*!
 * @brief C_FindObjectsFinal happy path.
 *
 */
void test_pkcs11_C_FindObjectsFinal( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_ULONG ulCount = 1;
    CK_OBJECT_HANDLE xObject = 0;
    char * pucLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;

    PKCS11_CertificateTemplate_t xCertificateTemplate = { { CKA_LABEL,
                                                            pucLabel,
                                                            strlen( ( const char * ) pucLabel ) } };

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCreateCert( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    pvPortMalloc_Stub( pvPkcs11MallocCb );
    xResult = C_FindObjectsInit( xSession, ( CK_ATTRIBUTE_PTR ) &xCertificateTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_FindObjectsFinal( xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}
/* ======================  TESTING C_DigestInit  ============================ */

/*!
 * @brief C_DigestInit happy path.
 *
 */
void test_pkcs11_C_DigestInit( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_MECHANISM xMechanism = { 0 };

    xMechanism.mechanism = CKM_SHA256;

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_sha256_init_CMockIgnore();
    mbedtls_sha256_starts_ret_IgnoreAndReturn( 0 );
    xResult = C_DigestInit( xSession, &xMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}
/* ======================  TESTING C_DigestUpdate  ============================ */

/*!
 * @brief C_DigestUpdate happy path.
 *
 */
void test_pkcs11_C_DigestUpdate( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_MECHANISM xMechanism = { 0 };

    xMechanism.mechanism = CKM_SHA256;
    CK_BYTE pxDummyData[] = "Dummy data";

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_sha256_init_CMockIgnore();
    mbedtls_sha256_starts_ret_IgnoreAndReturn( 0 );
    xResult = C_DigestInit( xSession, &xMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_sha256_update_ret_IgnoreAndReturn( 0 );
    xResult = C_DigestUpdate( xSession, pxDummyData, sizeof( pxDummyData ) );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}
/* ======================  TESTING C_DigestFinal  ============================ */

/*!
 * @brief C_DigestFinal happy path.
 *
 */
void test_pkcs11_C_DigestFinal( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_MECHANISM xMechanism = { 0 };

    xMechanism.mechanism = CKM_SHA256;
    CK_BYTE pxDummyData[] = "Dummy data";
    CK_ULONG ulDigestLen = pkcs11SHA256_DIGEST_LENGTH;


    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_sha256_init_CMockIgnore();
    mbedtls_sha256_starts_ret_IgnoreAndReturn( 0 );
    xResult = C_DigestInit( xSession, &xMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_sha256_finish_ret_IgnoreAndReturn( 0 );
    xResult = C_DigestFinal( xSession, pxDummyData, &ulDigestLen );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( pkcs11SHA256_DIGEST_LENGTH, ulDigestLen );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}
/* ======================  TESTING PKCS11_PAL_DestroyObject  ============================ */

/*!
 * @brief PKCS11_PAL_DestroyObject happy path.
 *
 */
void test_pkcs11_PKCS11_PAL_DestroyObject( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCreateCert( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( xObject );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    xResult = PKCS11_PAL_DestroyObject( xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}
/* ======================  TESTING C_SignInit  ============================ */

/*!
 * @brief C_SignInit ECDSA happy path.
 *
 */
void test_pkcs11_C_SignInitECDSA( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_MECHANISM xMechanism = { 0 };

    xMechanism.mechanism = CKM_ECDSA;
    CK_OBJECT_HANDLE xKey = 0;
    CK_OBJECT_HANDLE xObject = 0;

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCreateEcPub( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_pk_free_CMockIgnore();
    mbedtls_pk_init_CMockIgnore();
    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_ECDSA );
    xResult = C_SignInit( xSession, &xMechanism, xKey );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}
/* ======================  TESTING C_Sign  ============================ */

/*!
 * @brief C_Sign ECDSA happy path.
 *
 */
void test_pkcs11_C_SignECDSA( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_MECHANISM xMechanism = { 0 };

    xMechanism.mechanism = CKM_ECDSA;
    CK_OBJECT_HANDLE xKey = 0;
    CK_BYTE pxDummyData[ pkcs11SHA256_DIGEST_LENGTH ] = { 0xAA };
    CK_ULONG ulDummyDataLen = sizeof( pxDummyData );
    CK_BYTE pxDummySignature[ pkcs11ECDSA_P256_SIGNATURE_LENGTH ] = { 0xAA };
    CK_ULONG ulDummySignatureLen = sizeof( pxDummySignature );

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_pk_free_CMockIgnore();
    mbedtls_pk_init_CMockIgnore();
    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_ECDSA );
    xResult = C_SignInit( xSession, &xMechanism, xKey );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_pk_sign_IgnoreAndReturn( 0 );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKI_mbedTLSSignatureToPkcs11Signature_IgnoreAndReturn( 0 );
    xResult = C_Sign( xSession, pxDummyData, ulDummyDataLen, pxDummySignature, &ulDummySignatureLen );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}
/* ======================  TESTING C_VerifyInit  ============================ */

/*!
 * @brief C_VerifyInit ECDSA happy path.
 *
 */
void test_pkcs11_C_VerifyInitECDSA( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    CK_MECHANISM xMechanism = { 0 };

    xMechanism.mechanism = CKM_ECDSA;
    CK_BBOOL xIsPrivate = CK_FALSE;

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCreateEcPub( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_pk_free_CMockIgnore();
    mbedtls_pk_init_CMockIgnore();
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 0 );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_ECDSA );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}
/* ======================  TESTING C_GenerateRandom  ============================ */

/*!
 * @brief C_GenerateRandom happy path.
 *
 */
void test_pkcs11_C_GenerateRandom( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_BYTE ucRandData[ 3 ] = { 0 };
    CK_ULONG ulRandLen = sizeof( ucRandData );

    xResult = prvInitializePkcs11( ( SemaphoreHandle_t ) &xResult );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvOpenSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_ctr_drbg_random_IgnoreAndReturn( 0 );
    xResult = C_GenerateRandom( xSession, ucRandData, ulRandLen );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}
