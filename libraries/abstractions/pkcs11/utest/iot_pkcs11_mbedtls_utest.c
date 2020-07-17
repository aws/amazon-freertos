/*
 * FreeRTOS PKCS #11 V2.1.0
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
#include "mock_ecdsa.h"
#include "mock_rsa.h"
#include "mock_bignum.h"
#include "mock_portable.h"
#include "mock_iot_pki_utils.h"
#include "mock_iot_logging_task.h"
#include "mock_mbedtls_error.h"

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

#define EC_PRIV_KEY_INITIALIZER                                                            \
    {                                                                                      \
        { CKA_CLASS, &xPrivateKeyClass, sizeof( CK_OBJECT_CLASS ) },                       \
        { CKA_KEY_TYPE, &xPrivateKeyType, sizeof( CK_KEY_TYPE ) },                         \
        { CKA_LABEL, pucPrivLabel, ( CK_ULONG ) strlen( ( const char * ) pucPrivLabel ) }, \
        { CKA_TOKEN, &xTrue, sizeof( CK_BBOOL ) },                                         \
        { CKA_SIGN, &xTrue, sizeof( CK_BBOOL ) },                                          \
        { CKA_EC_PARAMS, pxEcPrivParams, EC_PARAMS_LENGTH },                               \
        { CKA_VALUE, pxD, EC_D_LENGTH }                                                    \
    }

#define EC_PUB_KEY_INITIALIZER                                             \
    {                                                                      \
        { CKA_CLASS, &xPublicKeyClass, sizeof( xPublicKeyClass ) },        \
        { CKA_KEY_TYPE, &xPublicKeyType, sizeof( xPublicKeyType ) },       \
        { CKA_TOKEN, &xTrue, sizeof( xTrue ) },                            \
        { CKA_VERIFY, &xTrue, sizeof( xTrue ) },                           \
        { CKA_EC_PARAMS, pxEcPubParams, sizeof( pxEcPubParams ) },         \
        { CKA_EC_POINT, pxEcPoint, xLength + 2 },                          \
        { CKA_LABEL, pucPubLabel, strlen( ( const char * ) pucPubLabel ) } \
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
        { CKA_PRIVATE_EXPONENT, xRsaParams.d + 1, D_LENGTH },                      \
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
    if( ptr != NULL )
    {
        usMallocFreeCalls--;
        free( ptr );
    }
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
    usMallocFreeCalls = 0;
}

/* called at the end of the whole suite */
int suiteTearDown( int numFailures )
{
    return numFailures;
}

/* ==========================  Helper functions  ============================ */

#define prvCommonInitStubs()               \
    xResult = prvInitializePkcs11();       \
    TEST_ASSERT_EQUAL( CKR_OK, xResult );  \
    xResult = prvOpenSession( &xSession ); \
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

#define prvCommonDeinitStubs()              \
    xResult = prvCloseSession( &xSession ); \
    TEST_ASSERT_EQUAL( CKR_OK, xResult );   \
    xResult = prvUninitializePkcs11();      \
    TEST_ASSERT_EQUAL( CKR_OK, xResult )

/*!
 * @brief Helper function to stub mbedtls_pk_free.
 *
 */
void vMbedPkFree( mbedtls_pk_context * pxCtx,
                  int lCallCount )
{
    ( void ) lCallCount;
    vPkcs11FreeCb( pxCtx->pk_ctx, 1 );
}

/*!
 * @brief Helper function to initialize PKCS #11.
 *
 */
static CK_RV prvInitializePkcs11()
{
    CK_RV xResult = CKR_OK;

    PKCS11_PAL_Initialize_IgnoreAndReturn( CKR_OK );
    xQueueCreateMutexStatic_IgnoreAndReturn( ( SemaphoreHandle_t ) 1 );
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
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    xQueueCreateMutex_IgnoreAndReturn( ( SemaphoreHandle_t ) 1 );
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
    mbedtls_pk_free_CMockIgnore();
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
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 3 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    xResult = C_CreateObject( *pxSession,
                              ( CK_ATTRIBUTE_PTR ) &xTemplate,
                              sizeof( xTemplate ) / sizeof( CK_ATTRIBUTE ),
                              pxObject );
    return xResult;
}

/*!
 * @brief Helper function to create an EC Private Key.
 *
 */
static CK_RV prvCreateEcPriv( CK_SESSION_HANDLE_PTR pxSession,
                              CK_OBJECT_HANDLE_PTR pxObject )
{
    CK_RV xResult = CKR_OK;
    CK_KEY_TYPE xPrivateKeyType = CKK_EC;
    CK_OBJECT_CLASS xPrivateKeyClass = CKO_PRIVATE_KEY;
    CK_BBOOL xTrue = CK_TRUE;
    char * pucPrivLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;

    /* DER-encoding of an ANSI X9.62 Parameters value */
    CK_BYTE * pxEcPrivParams = ( CK_BYTE * ) ( "\x06\x08" MBEDTLS_OID_EC_GRP_SECP256R1 );

    /* Private value D. */
    CK_BYTE pxD[ EC_D_LENGTH ] = { 0 };

    CK_ATTRIBUTE xPrivateKeyTemplate[] = EC_PRIV_KEY_INITIALIZER;

    mbedtls_pk_init_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    PKCS11_PAL_FindObject_IgnoreAndReturn( 2 );
    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_ecp_keypair_init_CMockIgnore();
    mbedtls_ecp_group_init_CMockIgnore();
    mbedtls_ecp_group_load_IgnoreAndReturn( 0 );
    mbedtls_mpi_read_binary_IgnoreAndReturn( 0 );
    mbedtls_pk_write_key_der_IgnoreAndReturn( 1 );
    mbedtls_pk_free_CMockIgnore();
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 2 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_CreateObject( *pxSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
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
    char * pucPubLabel = pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
    size_t xLength = 256;
    /* DER-encoding of an ANSI X9.62 Parameters value */
    CK_BYTE pxEcPubParams[] = pkcs11DER_ENCODED_OID_P256;
    CK_BYTE pxEcPoint[ 256 ] = { 0 };

    CK_ATTRIBUTE xTemplate[] = EC_PUB_KEY_INITIALIZER;

    /* Create  EC based public key. */
    mbedtls_pk_init_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    PKCS11_PAL_FindObject_IgnoreAndReturn( 1 );
    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 0 );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_ecp_keypair_init_CMockIgnore();
    mbedtls_ecp_group_init_CMockIgnore();
    mbedtls_ecp_group_load_IgnoreAndReturn( 0 );
    mbedtls_ecp_point_read_binary_IgnoreAndReturn( 0 );
    mbedtls_mpi_read_binary_IgnoreAndReturn( 0 );
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

    PKCS11_PAL_Initialize_IgnoreAndReturn( CKR_OK );
    xQueueCreateMutexStatic_IgnoreAndReturn( ( SemaphoreHandle_t ) 1 );
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

    PKCS11_PAL_Initialize_IgnoreAndReturn( CKR_OK );
    xQueueCreateMutexStatic_IgnoreAndReturn( NULL );
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

    PKCS11_PAL_Initialize_IgnoreAndReturn( CKR_OK );
    xQueueCreateMutexStatic_IgnoreAndReturn( ( SemaphoreHandle_t ) 1 );
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
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    xQueueCreateMutex_IgnoreAndReturn( ( SemaphoreHandle_t ) 1 );
    xResult = C_OpenSession( 0, xFlags, NULL, 0, &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief C_OpenSession mutex allocation failures.
 *
 */
void test_pkcs11_C_OpenSessionMutexMem( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_FLAGS xFlags = CKF_SERIAL_SESSION | CKF_RW_SESSION;

    xResult = prvInitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    pvPortMalloc_Stub( pvPkcs11MallocCb );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    xQueueCreateMutex_ExpectAnyArgsAndReturn( NULL );
    xQueueCreateMutex_ExpectAnyArgsAndReturn( ( SemaphoreHandle_t ) 1 );
    vLoggingPrintf_Ignore();
    vQueueDelete_Ignore();
    xResult = C_OpenSession( 0, xFlags, NULL, 0, &xSession );
    TEST_ASSERT_EQUAL( CKR_HOST_MEMORY, xResult );

    xQueueCreateMutex_ExpectAnyArgsAndReturn( ( SemaphoreHandle_t ) 1 );
    xQueueCreateMutex_ExpectAnyArgsAndReturn( NULL );
    xResult = C_OpenSession( 0, xFlags, NULL, 0, &xSession );
    TEST_ASSERT_EQUAL( CKR_HOST_MEMORY, xResult );

    xQueueCreateMutex_ExpectAnyArgsAndReturn( NULL );
    xQueueCreateMutex_ExpectAnyArgsAndReturn( NULL );
    xResult = C_OpenSession( 0, xFlags, NULL, 0, &xSession );
    TEST_ASSERT_EQUAL( CKR_HOST_MEMORY, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
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

    vLoggingPrintf_Ignore();
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

    vLoggingPrintf_Ignore();
    xResult = C_OpenSession( 0, xFlags, NULL, 0, NULL );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    xResult = C_OpenSession( 0, CKF_RW_SESSION, NULL, 0, &xSession );
    TEST_ASSERT_EQUAL( CKR_SESSION_PARALLEL_NOT_SUPPORTED, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/* ======================  TESTING C_CloseSession  ============================ */

/*!
 * @brief C_CloseSession happy path.
 *
 */
void test_pkcs11_C_CloseSession( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;

    prvCommonInitStubs();

    xResult = prvCloseSession( &xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvUninitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
}

/*!
 * @brief C_CloseSession PKCS #11 not initialized.
 *
 */
void test_pkcs11_C_CloseSessionUninit( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;

    xResult = C_CloseSession( xSession );
    TEST_ASSERT_EQUAL( CKR_CRYPTOKI_NOT_INITIALIZED, xResult );
}

/*!
 * @brief C_CloseSession PKCS #11 invalid session.
 *
 */
void test_pkcs11_C_CloseSessionBadSession( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;

    xResult = prvInitializePkcs11();
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = C_CloseSession( xSession );
    TEST_ASSERT_EQUAL( CKR_SESSION_HANDLE_INVALID, xResult );

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
    mbedtls_ecp_keypair xKeyContext = { 0 };
    char * pucPrivLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
    /* DER-encoding of an ANSI X9.62 Parameters value */
    CK_BYTE * pxEcPrivParams = ( CK_BYTE * ) ( "\x06\x08" MBEDTLS_OID_EC_GRP_SECP256R1 );
    CK_OBJECT_HANDLE xObject = 0;

    /* Private value D. */
    CK_BYTE pxD[ EC_D_LENGTH ] = { 0 };

    CK_ATTRIBUTE xPrivateKeyTemplate[] = EC_PRIV_KEY_INITIALIZER;

    prvCommonInitStubs();

    mbedtls_pk_init_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    PKCS11_PAL_FindObject_IgnoreAndReturn( 1 );
    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    pvPortMalloc_IgnoreAndReturn( &xKeyContext );
    mbedtls_ecp_keypair_init_CMockIgnore();
    mbedtls_ecp_group_init_CMockIgnore();
    mbedtls_ecp_group_load_IgnoreAndReturn( 0 );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    mbedtls_mpi_read_binary_IgnoreAndReturn( 0 );
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

    prvCommonDeinitStubs();
}

/*
 *!
 * @brief C_CreateObject Creating an EC private key attribute failures.
 *
 */
void test_pkcs11_C_CreateObjectECPrivKeyBadAtt( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_KEY_TYPE xPrivateKeyType = CKK_EC;
    CK_OBJECT_CLASS xPrivateKeyClass = CKO_PRIVATE_KEY;
    CK_BBOOL xTrue = CK_TRUE;
    CK_BBOOL xFalse = CK_FALSE;
    char * pucPrivLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
    /* DER-encoding of an ANSI X9.62 Parameters value */
    CK_BYTE * pxEcPrivParams = ( CK_BYTE * ) ( "\x06\x08" MBEDTLS_OID_EC_GRP_SECP256R1 );
    CK_OBJECT_HANDLE xObject = 0;

    /* Private value D. */
    CK_BYTE pxD[ EC_D_LENGTH ] = { 0 };

    CK_ATTRIBUTE xPrivateKeyTemplate[] = EC_PRIV_KEY_INITIALIZER;

    xPrivateKeyTemplate[ 5 ].type = CKA_MODULUS;

    prvCommonInitStubs();

    mbedtls_pk_init_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    PKCS11_PAL_FindObject_IgnoreAndReturn( 1 );
    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    vLoggingPrintf_CMockIgnore();
    mbedtls_pk_free_Stub( vMbedPkFree );
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_ATTRIBUTE_TYPE_INVALID, xResult );

    xPrivateKeyTemplate[ 4 ].pValue = &xFalse;
    xPrivateKeyTemplate[ 5 ].type = CKA_EC_PARAMS;
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_ATTRIBUTE_VALUE_INVALID, xResult );

    xPrivateKeyTemplate[ 4 ].pValue = &xTrue;
    mbedtls_mpi_read_binary_IgnoreAndReturn( -1 );
    mbedtls_strerror_highlevel_IgnoreAndReturn( NULL );
    mbedtls_strerror_lowlevel_IgnoreAndReturn( NULL );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    xPrivateKeyTemplate[ 3 ].pValue = &xFalse;
    mbedtls_mpi_read_binary_IgnoreAndReturn( 0 );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    xPrivateKeyTemplate[ 3 ].pValue = &xTrue;
    xPrivateKeyTemplate[ 5 ].pValue = &pucPrivLabel;
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_TEMPLATE_INCONSISTENT, xResult );


    mbedtls_pk_parse_key_IgnoreAndReturn( -1 );
    mbedtls_ecp_keypair_init_CMockIgnore();
    mbedtls_ecp_group_init_CMockIgnore();
    mbedtls_ecp_group_load_IgnoreAndReturn( -1 );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );
    TEST_ASSERT_EQUAL( CKR_FUNCTION_FAILED, xResult );

    prvCommonDeinitStubs();
}

/*
 *!
 * @brief C_CreateObject Creating an EC private key fails when saving to DER.
 *
 */
void test_pkcs11_C_CreateObjectECPrivKeyDerFail( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_KEY_TYPE xPrivateKeyType = CKK_EC;
    CK_OBJECT_CLASS xPrivateKeyClass = CKO_PRIVATE_KEY;
    CK_BBOOL xTrue = CK_TRUE;
    mbedtls_ecp_keypair xKeyContext = { 0 };
    char * pucPrivLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
    /* DER-encoding of an ANSI X9.62 Parameters value */
    CK_BYTE * pxEcPrivParams = ( CK_BYTE * ) ( "\x06\x08" MBEDTLS_OID_EC_GRP_SECP256R1 );
    CK_OBJECT_HANDLE xObject = 0;

    /* Private value D. */
    CK_BYTE pxD[ EC_D_LENGTH ] = { 0 };

    CK_ATTRIBUTE xPrivateKeyTemplate[] = EC_PRIV_KEY_INITIALIZER;

    prvCommonInitStubs();

    mbedtls_pk_init_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    PKCS11_PAL_FindObject_IgnoreAndReturn( 1 );
    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    pvPortMalloc_IgnoreAndReturn( &xKeyContext );
    mbedtls_ecp_keypair_init_CMockIgnore();
    mbedtls_ecp_group_init_CMockIgnore();
    mbedtls_ecp_group_load_IgnoreAndReturn( 0 );
    mbedtls_mpi_read_binary_IgnoreAndReturn( 0 );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    mbedtls_pk_write_key_der_IgnoreAndReturn( -1 );
    vLoggingPrintf_CMockIgnore();
    mbedtls_strerror_lowlevel_IgnoreAndReturn( NULL );
    mbedtls_strerror_highlevel_IgnoreAndReturn( NULL );
    mbedtls_pk_free_CMockIgnore();
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_FUNCTION_FAILED, xResult );

    /* Malloc fails in prvSaveDerKeyToPal */
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    pvPortMalloc_IgnoreAndReturn( NULL );
    mbedtls_pk_write_key_der_IgnoreAndReturn( 1 );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_HOST_MEMORY, xResult );

    prvCommonDeinitStubs();
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
    char * pucPubLabel = pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
    size_t xLength = 256;
    /* DER-encoding of an ANSI X9.62 Parameters value */
    CK_BYTE pxEcPubParams[] = pkcs11DER_ENCODED_OID_P256;
    CK_OBJECT_HANDLE xObject = 0;

    CK_BYTE pxEcPoint[ 256 ] = { 0 };

    CK_ATTRIBUTE xPublicKeyTemplate[] = EC_PUB_KEY_INITIALIZER;

    prvCommonInitStubs();

    mbedtls_pk_init_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    PKCS11_PAL_FindObject_IgnoreAndReturn( CK_INVALID_HANDLE );
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 0 );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_ecp_keypair_init_CMockIgnore();
    mbedtls_ecp_group_init_CMockIgnore();
    mbedtls_ecp_group_load_IgnoreAndReturn( 0 );
    mbedtls_ecp_point_read_binary_IgnoreAndReturn( 0 );
    mbedtls_mpi_read_binary_IgnoreAndReturn( 0 );
    mbedtls_pk_write_pubkey_der_IgnoreAndReturn( 1 );
    mbedtls_pk_free_Stub( vMbedPkFree );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                              sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*
 *!
 * @brief C_CreateObject Creating an EC public key attribute failures.
 *
 */
void test_pkcs11_C_CreateObjectECPubKeyBadAtt( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_KEY_TYPE xPublicKeyType = CKK_EC;
    CK_KEY_TYPE xFaultyPublicKeyType = CKK_AES;
    CK_OBJECT_CLASS xPublicKeyClass = CKO_PUBLIC_KEY;
    CK_BBOOL xTrue = CK_TRUE;
    CK_BBOOL xFalse = CK_FALSE;
    char * pucPubLabel = pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
    size_t xLength = 256;
    /* DER-encoding of an ANSI X9.62 Parameters value */
    CK_BYTE pxEcPubParams[] = pkcs11DER_ENCODED_OID_P256;
    CK_OBJECT_HANDLE xObject = 0;
    CK_BYTE pxEcPoint[ 256 ] = { 0 };

    CK_ATTRIBUTE xPublicKeyTemplate[] = EC_PUB_KEY_INITIALIZER;

    prvCommonInitStubs();

    xPublicKeyTemplate[ 5 ].type = CKA_MODULUS;

    mbedtls_pk_init_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    PKCS11_PAL_FindObject_IgnoreAndReturn( 1 );
    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 0 );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    vLoggingPrintf_CMockIgnore();
    mbedtls_pk_free_CMockIgnore();
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                              sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_ATTRIBUTE_TYPE_INVALID, xResult );

    xPublicKeyTemplate[ 1 ].pValue = &xFaultyPublicKeyType;
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                              sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_MECHANISM_INVALID, xResult );

    /* Reuse the variable. The goal being to pass a bad attribute class to
     * C_CreateObject */
    xPublicKeyTemplate[ 0 ].pValue = &xFaultyPublicKeyType;
    xPublicKeyTemplate[ 1 ].pValue = &xPublicKeyType;
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                              sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_ATTRIBUTE_VALUE_INVALID, xResult );

    xPublicKeyTemplate[ 0 ].type = CKA_KEY_TYPE;
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                              sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_TEMPLATE_INCOMPLETE, xResult );

    xPublicKeyTemplate[ 0 ].type = CKA_CLASS;
    xPublicKeyTemplate[ 0 ].pValue = &xPublicKeyClass;
    xPublicKeyTemplate[ 3 ].pValue = &xFalse;
    xPublicKeyTemplate[ 5 ].type = CKA_EC_POINT;
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                              sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_ATTRIBUTE_VALUE_INVALID, xResult );

    xPublicKeyTemplate[ 3 ].pValue = &xTrue;
    mbedtls_ecp_point_read_binary_IgnoreAndReturn( -1 );
    mbedtls_strerror_highlevel_IgnoreAndReturn( NULL );
    mbedtls_strerror_lowlevel_IgnoreAndReturn( NULL );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                              sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_FUNCTION_FAILED, xResult );

    prvCommonDeinitStubs();
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

    prvCommonInitStubs();

    CK_ATTRIBUTE xPrivateKeyTemplate[] = RSA_PRIV_KEY_INITIALIZER;

    mbedtls_pk_init_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    mbedtls_rsa_init_CMockIgnore();
    mbedtls_rsa_import_raw_IgnoreAndReturn( 0 );
    mbedtls_mpi_read_binary_IgnoreAndReturn( 0 );
    mbedtls_pk_write_key_der_IgnoreAndReturn( 1 );
    mbedtls_pk_free_Stub( vMbedPkFree );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*
 *!
 * @brief C_CreateObject Creating an RSA private key attribute failures.
 *
 */
void test_pkcs11_C_CreateObjectRSAPrivKeyBadAtt( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_KEY_TYPE xPrivateKeyType = CKK_RSA;
    CK_OBJECT_CLASS xPrivateKeyClass = CKO_PRIVATE_KEY;
    CK_BBOOL xTrue = CK_TRUE;
    char * pucLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
    CK_OBJECT_HANDLE xObject = 0;

    RsaParams_t xRsaParams = { 0 };

    prvCommonInitStubs();

    CK_ATTRIBUTE xPrivateKeyTemplate[] = RSA_PRIV_KEY_INITIALIZER;
    xPrivateKeyTemplate[ 4 ].type = CKA_EC_POINT;

    mbedtls_pk_init_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    mbedtls_rsa_init_CMockIgnore();
    mbedtls_pk_free_Stub( vMbedPkFree );
    vPortFree_Stub( vPkcs11FreeCb );
    vLoggingPrintf_CMockIgnore();
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_ATTRIBUTE_TYPE_INVALID, xResult );

    xTrue = CK_FALSE;
    xPrivateKeyTemplate[ 4 ].type = CKA_SIGN;
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_ATTRIBUTE_VALUE_INVALID, xResult );

    xTrue = CK_FALSE;
    xPrivateKeyTemplate[ 2 ].type = CKA_EC_POINT;
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    xPrivateKeyTemplate[ 2 ].type = CKA_LABEL;
    xTrue = CK_TRUE;
    mbedtls_rsa_import_raw_IgnoreAndReturn( 1 );
    mbedtls_strerror_highlevel_IgnoreAndReturn( NULL );
    mbedtls_strerror_lowlevel_IgnoreAndReturn( NULL );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );

    TEST_ASSERT_EQUAL( CKR_FUNCTION_FAILED, xResult );

    prvCommonDeinitStubs();
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

    prvCommonInitStubs();

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

    prvCommonDeinitStubs();
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

    prvCommonInitStubs();

    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xCertificateTemplate,
                              sizeof( xCertificateTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*
 *!
 * @brief C_CreateObject Bad Certificate type.
 *
 */
void test_pkcs11_C_CreateObjectCertificateBadType( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    CK_OBJECT_CLASS xCertificateClass = CKO_CERTIFICATE;
    CK_CERTIFICATE_TYPE xCertificateType = -1;
    CK_BBOOL xTokenStorage = CK_TRUE;
    CK_BYTE xSubject[] = "TestSubject";
    CK_BYTE xCert[] = "Empty Cert";
    char * pucLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;

    PKCS11_CertificateTemplate_t xCertificateTemplate = CERT_INITIALIZER;

    prvCommonInitStubs();

    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_Ignore();
    vLoggingPrintf_CMockIgnore();

    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xCertificateTemplate,
                              sizeof( xCertificateTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );
    TEST_ASSERT_EQUAL( CKR_ATTRIBUTE_VALUE_INVALID, xResult );

    prvCommonDeinitStubs();
}

/*
 *!
 * @brief C_CreateObject Bad Certificate token value.
 *
 */
void test_pkcs11_C_CreateObjectCertificateBadToken( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    CK_OBJECT_CLASS xCertificateClass = CKO_CERTIFICATE;
    CK_CERTIFICATE_TYPE xCertificateType = CKC_X_509;
    CK_BBOOL xTokenStorage = CK_FALSE;
    CK_BYTE xSubject[] = "TestSubject";
    CK_BYTE xCert[] = "Empty Cert";
    char * pucLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;

    PKCS11_CertificateTemplate_t xCertificateTemplate = CERT_INITIALIZER;

    prvCommonInitStubs();

    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_Ignore();
    vLoggingPrintf_CMockIgnore();
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xCertificateTemplate,
                              sizeof( xCertificateTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );
    TEST_ASSERT_EQUAL( CKR_ATTRIBUTE_VALUE_INVALID, xResult );

    prvCommonDeinitStubs();
}

/*
 *!
 * @brief C_CreateObject Unknown Certificate Attribute.
 *
 */
void test_pkcs11_C_CreateObjectCertificateUnkownAtt( void )
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

    xCertificateTemplate.xSubject.type = CKA_MODULUS;

    prvCommonInitStubs();

    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_Ignore();
    vLoggingPrintf_CMockIgnore();
    xResult = C_CreateObject( xSession,
                              ( CK_ATTRIBUTE_PTR ) &xCertificateTemplate,
                              sizeof( xCertificateTemplate ) / sizeof( CK_ATTRIBUTE ),
                              &xObject );
    TEST_ASSERT_EQUAL( CKR_ATTRIBUTE_VALUE_INVALID, xResult );

    prvCommonDeinitStubs();
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

    prvCommonInitStubs();

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

    prvCommonDeinitStubs();
}

/*!
 * @brief C_GetAttributeValue test attribute parsing of PKCS #11 templates.
 *
 */
void test_pkcs11_C_GetAttributeValueAttParsing( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    CK_OBJECT_HANDLE xObjectPub = 0;
    CK_ULONG ulCount = 1;
    CK_ULONG ulLength = 1;
    CK_BYTE pulKnownBuf[] = pkcs11DER_ENCODED_OID_P256;
    CK_BYTE pulBuf[ sizeof( pulKnownBuf ) ] = { 0 };
    CK_BYTE ulPoint = 0;
    CK_BYTE ulKnownPoint = 0x04;
    CK_BBOOL xIsPrivate = CK_FALSE;
    CK_OBJECT_CLASS xPrivateKeyClass = { 0 };
    CK_OBJECT_CLASS xKnownPrivateKeyClass = CKO_PRIVATE_KEY;
    CK_ATTRIBUTE xTemplate = { CKA_EC_PARAMS, pulBuf, sizeof( pulBuf ) };

    prvCommonInitStubs();

    xResult = prvCreateEcPriv( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCreateEcPub( &xSession, &xObjectPub );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );


    /* EC Params Case */
    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    mbedtls_pk_init_CMockIgnore();
    mbedtls_pk_parse_key_IgnoreAndReturn( 1 );
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 1 );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_pk_free_CMockIgnore();
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL_MEMORY( pulKnownBuf, xTemplate.pValue, sizeof( pulKnownBuf ) );

    /* EC Point Case */
    mbedtls_ecp_tls_write_point_IgnoreAndReturn( 1 );
    xTemplate.type = CKA_EC_POINT;
    xTemplate.pValue = NULL;
    xTemplate.ulValueLen = 0;

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    /* This is hard coded in the library. We need to update this test case and the
     * library code to either have this in the config or use a macro instead of a magic number. */
    TEST_ASSERT_EQUAL( 67, xTemplate.ulValueLen );

    xTemplate.pValue = &ulPoint;
    xTemplate.ulValueLen = sizeof( ulPoint );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( ulKnownPoint, ulPoint );

    xTemplate.pValue = &ulPoint;
    xTemplate.ulValueLen = sizeof( ulPoint );

    mbedtls_ecp_tls_write_point_IgnoreAndReturn( MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL );
    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_BUFFER_TOO_SMALL, xResult );

    mbedtls_ecp_tls_write_point_IgnoreAndReturn( -1 );
    xTemplate.pValue = &ulPoint;
    xTemplate.ulValueLen = sizeof( ulPoint );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_FUNCTION_FAILED, xResult );

    mbedtls_ecp_tls_write_point_IgnoreAndReturn( 1 );

    /* Unknown attribute. */
    xTemplate.type = CKA_MODULUS;

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_ATTRIBUTE_TYPE_INVALID, xResult );

    /* CKA Class Case */
    xTemplate.type = CKA_CLASS;
    xTemplate.pValue = NULL;
    xTemplate.ulValueLen = 0;

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( NULL, xTemplate.pValue );
    TEST_ASSERT_EQUAL( sizeof( xPrivateKeyClass ), xTemplate.ulValueLen );

    xTemplate.pValue = &xPrivateKeyClass;

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( sizeof( xPrivateKeyClass ), xTemplate.ulValueLen );
    TEST_ASSERT_EQUAL_MEMORY( &xKnownPrivateKeyClass, xTemplate.pValue, sizeof( xPrivateKeyClass ) );

    /* CKA Value Case */
    xTemplate.type = CKA_VALUE;
    xTemplate.pValue = NULL;
    xTemplate.ulValueLen = 0;

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pulDataSize( &ulLength );
    mbedtls_pk_parse_key_IgnoreAndReturn( 1 );
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 0 );
    xResult = C_GetAttributeValue( xSession, xObjectPub, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( 1, xTemplate.ulValueLen );

    xTemplate.type = CKA_VALUE;
    xTemplate.pValue = &ulPoint;
    xTemplate.ulValueLen = 0;

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pulDataSize( &ulLength );
    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    xResult = C_GetAttributeValue( xSession, xObjectPub, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_BUFFER_TOO_SMALL, xResult );
    TEST_ASSERT_EQUAL( 0, xTemplate.ulValueLen );

    xTemplate.type = CKA_VALUE;
    xTemplate.pValue = &ulLength;
    xTemplate.ulValueLen = 1;

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    xResult = C_GetAttributeValue( xSession, xObjectPub, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( 1, xTemplate.ulValueLen );
    TEST_ASSERT_EQUAL( 1, *( uint32_t * ) xTemplate.pValue );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_GetAttributeValue paths.
 *
 */
void test_pkcs11_C_GetAttributeValuePrivKey( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    CK_ULONG ulCount = 1;
    CK_ATTRIBUTE xTemplate = { CKA_VALUE, NULL, 0 };
    CK_KEY_TYPE xKeyType = { 0 };
    CK_KEY_TYPE xKnownKeyType = CKK_EC;

    prvCommonInitStubs();

    xResult = prvCreateEcPriv( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    mbedtls_pk_init_CMockIgnore();
    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_pk_free_CMockIgnore();
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_ATTRIBUTE_SENSITIVE, xResult );
    TEST_ASSERT_EQUAL( CK_UNAVAILABLE_INFORMATION, xTemplate.ulValueLen );

    xTemplate.type = CKA_KEY_TYPE;
    xTemplate.pValue = NULL;
    xTemplate.ulValueLen = 0;

    mbedtls_pk_get_type_ExpectAnyArgsAndReturn( CKK_EC );
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( sizeof( CK_KEY_TYPE ), xTemplate.ulValueLen );


    xTemplate.type = CKA_KEY_TYPE;
    xTemplate.pValue = &xKeyType;
    xTemplate.ulValueLen = sizeof( CK_KEY_TYPE );

    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( sizeof( CK_KEY_TYPE ), xTemplate.ulValueLen );
    TEST_ASSERT_EQUAL_MEMORY( &xKnownKeyType, xTemplate.pValue, sizeof( CK_KEY_TYPE ) );

    xTemplate.type = CKA_KEY_TYPE;
    xTemplate.pValue = NULL;
    xTemplate.ulValueLen = 0;

    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( sizeof( CK_KEY_TYPE ), xTemplate.ulValueLen );


    xTemplate.type = CKA_KEY_TYPE;
    xTemplate.pValue = &xKeyType;
    xTemplate.ulValueLen = sizeof( CK_KEY_TYPE );
    xKnownKeyType = CKK_RSA;

    mbedtls_pk_get_type_ExpectAnyArgsAndReturn( MBEDTLS_PK_RSA );
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( sizeof( CK_KEY_TYPE ), xTemplate.ulValueLen );
    TEST_ASSERT_EQUAL_MEMORY( &xKnownKeyType, xTemplate.pValue, sizeof( CK_KEY_TYPE ) );

    xTemplate.type = CKA_KEY_TYPE;
    xTemplate.pValue = NULL;
    xTemplate.ulValueLen = 0;
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( sizeof( CK_KEY_TYPE ), xTemplate.ulValueLen );


    xTemplate.type = CKA_KEY_TYPE;
    xTemplate.pValue = &xKeyType;
    xTemplate.ulValueLen = sizeof( CK_KEY_TYPE );
    xKnownKeyType = CKK_ECDSA;

    mbedtls_pk_get_type_ExpectAnyArgsAndReturn( MBEDTLS_PK_ECDSA );
    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( sizeof( CK_KEY_TYPE ), xTemplate.ulValueLen );
    TEST_ASSERT_EQUAL_MEMORY( &xKnownKeyType, xTemplate.pValue, sizeof( CK_KEY_TYPE ) );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_GetAttributeValue bad args.
 *
 */
void test_pkcs11_C_GetAttributeBadArgs( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    CK_ULONG ulCount = 1;

    prvCommonInitStubs();

    xResult = C_GetAttributeValue( xSession, xObject, NULL, ulCount );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    xResult = C_GetAttributeValue( xSession, xObject, ( CK_ATTRIBUTE_PTR ) &xResult, 0 );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    xResult = C_GetAttributeValue( -1, xObject, ( CK_ATTRIBUTE_PTR ) &xResult, ulCount );
    TEST_ASSERT_EQUAL( CKR_SESSION_HANDLE_INVALID, xResult );

    prvCommonDeinitStubs();
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

    prvCommonInitStubs();

    xResult = prvCreateCert( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    pvPortMalloc_Stub( pvPkcs11MallocCb );
    xResult = C_FindObjectsInit( xSession, ( CK_ATTRIBUTE_PTR ) &xFindTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    /* Clean up after C_FindObjectsInit. */
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_FindObjectsFinal( xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_FindObjectsInit Active Operation.
 *
 */
void test_pkcs11_C_FindObjectsInitActiveOp( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_ULONG ulCount = 1;
    CK_OBJECT_HANDLE xObject = 0;
    char * pucLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;

    CK_ATTRIBUTE xFindTemplate = { CKA_LABEL, pucLabel, strlen( ( const char * ) pucLabel ) };

    prvCommonInitStubs();

    xResult = prvCreateCert( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    pvPortMalloc_Stub( pvPkcs11MallocCb );
    xResult = C_FindObjectsInit( xSession, ( CK_ATTRIBUTE_PTR ) &xFindTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    vLoggingPrintf_CMockIgnore();
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_FindObjectsInit( xSession, ( CK_ATTRIBUTE_PTR ) &xFindTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OPERATION_ACTIVE, xResult );

    /* Clean up after the successful C_FindObjectsInit. */
    xResult = C_FindObjectsFinal( xSession );
    TEST_ASSERT_EQUAL( CKR_OPERATION_NOT_INITIALIZED, xResult );

    /* The memory that was initialized in the first call gets free'd in the second
     * call because they share a session. */
    usMallocFreeCalls--;

    prvCommonDeinitStubs();
}

/*!
 * @brief C_FindObjectsInit Bad args.
 *
 */
void test_pkcs11_C_FindObjectsInitBadArgs( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_ULONG ulCount = 1;
    CK_OBJECT_HANDLE xObject = 0;
    char * pucLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;

    CK_ATTRIBUTE xFindTemplate = { CKA_LABEL, pucLabel, strlen( ( const char * ) pucLabel ) };

    prvCommonInitStubs();

    xResult = prvCreateCert( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    vLoggingPrintf_CMockIgnore();
    xResult = C_FindObjectsInit( xSession, NULL, ulCount );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    xResult = C_FindObjectsInit( xSession, ( CK_ATTRIBUTE_PTR ) &xFindTemplate, -1 );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    /* Clean up after C_FindObjectsInit. */
    xResult = C_FindObjectsFinal( xSession );
    TEST_ASSERT_EQUAL( CKR_OPERATION_NOT_INITIALIZED, xResult );


    prvCommonDeinitStubs();
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
    CK_BYTE pucBuf[] = { 1, 1, 1, 1 };
    CK_BYTE_PTR * ppucBufPtr = ( CK_BYTE_PTR * ) &pucBuf;
    CK_ULONG ulObjectLength = sizeof( pucBuf );
    char * pucLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;

    CK_ATTRIBUTE xFindTemplate = { CKA_LABEL, pucLabel, strlen( ( const char * ) pucLabel ) };

    prvCommonInitStubs();

    xResult = prvCreateCert( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = C_FindObjectsInit( xSession, ( CK_ATTRIBUTE_PTR ) &xFindTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_FindObject_IgnoreAndReturn( 1 );
    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_ppucData( ( CK_BYTE_PTR * ) &ppucBufPtr );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pulDataSize( &ulObjectLength );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    xResult = C_FindObjects( xSession, ( CK_OBJECT_HANDLE_PTR ) &xObject, 1, &ulFoundCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( 1, ulFoundCount );

    /* Clean up after C_FindObjectsInit. */
    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_FindObjectsFinal( xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_FindObjects bad args.
 *
 */
void test_pkcs11_C_FindObjectsBadArgs( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_ULONG ulCount = 1;
    CK_ULONG ulFoundCount = 0;
    CK_OBJECT_HANDLE xObject = 0;
    char * pucLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
    CK_BYTE pucBuf[] = { 0, 0, 0, 0 };
    CK_BYTE ** ppucBufPtr = ( CK_BYTE ** ) &pucBuf;
    CK_ULONG ulObjectLength = sizeof( pucBuf );

    CK_ATTRIBUTE xFindTemplate = { CKA_LABEL, pucLabel, strlen( ( const char * ) pucLabel ) };

    prvCommonInitStubs();

    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_FindObjects( xSession, ( CK_OBJECT_HANDLE_PTR ) &xObject, 1, &ulFoundCount );
    TEST_ASSERT_EQUAL( CKR_OPERATION_NOT_INITIALIZED, xResult );

    xResult = C_FindObjectsInit( xSession, ( CK_ATTRIBUTE_PTR ) &xFindTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_FindObject_IgnoreAndReturn( CK_INVALID_HANDLE );
    xResult = C_FindObjects( xSession, ( CK_OBJECT_HANDLE_PTR ) &xObject, 1, &ulFoundCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( 0, ulFoundCount );

    PKCS11_PAL_FindObject_IgnoreAndReturn( 1 );
    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_ppucData( ( CK_BYTE_PTR * ) &ppucBufPtr );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pulDataSize( &ulObjectLength );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();

    xResult = C_FindObjects( xSession, ( CK_OBJECT_HANDLE_PTR ) &xObject, 1, &ulFoundCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( CK_INVALID_HANDLE, xObject );
    TEST_ASSERT_EQUAL( 0, ulFoundCount );

    xResult = C_FindObjectsFinal( xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
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

    prvCommonInitStubs();

    xResult = prvCreateCert( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    pvPortMalloc_Stub( pvPkcs11MallocCb );
    xResult = C_FindObjectsInit( xSession, ( CK_ATTRIBUTE_PTR ) &xCertificateTemplate, ulCount );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    vPortFree_Stub( vPkcs11FreeCb );
    xResult = C_FindObjectsFinal( xSession );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
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

    prvCommonInitStubs();

    mbedtls_sha256_init_CMockIgnore();
    mbedtls_sha256_starts_ret_IgnoreAndReturn( 0 );
    xResult = C_DigestInit( xSession, &xMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_DigestInit session was closed.
 *
 */
void test_pkcs11_C_DigestInitClosedSession( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_MECHANISM xMechanism = { 0 };

    xMechanism.mechanism = CKM_SHA256;

    prvCommonInitStubs();
    prvCloseSession( &xSession );

    mbedtls_sha256_init_CMockIgnore();
    mbedtls_sha256_starts_ret_IgnoreAndReturn( 0 );
    xResult = C_DigestInit( xSession, &xMechanism );
    TEST_ASSERT_EQUAL( CKR_SESSION_HANDLE_INVALID, xResult );

    prvUninitializePkcs11();
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

    prvCommonInitStubs();

    mbedtls_sha256_init_CMockIgnore();
    mbedtls_sha256_starts_ret_IgnoreAndReturn( 0 );
    xResult = C_DigestInit( xSession, &xMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_sha256_update_ret_IgnoreAndReturn( 0 );
    xResult = C_DigestUpdate( xSession, pxDummyData, sizeof( pxDummyData ) );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_DigestUpdate bad args.
 *
 */
void test_pkcs11_C_DigestUpdateBadArgs( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_MECHANISM xMechanism = { 0 };

    xMechanism.mechanism = CKM_SHA256;
    CK_BYTE pxDummyData[] = "Dummy data";

    prvCommonInitStubs();

    mbedtls_sha256_init_CMockIgnore();
    mbedtls_sha256_starts_ret_IgnoreAndReturn( 0 );
    xResult = C_DigestInit( xSession, &xMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_sha256_update_ret_ExpectAnyArgsAndReturn( -1 );
    mbedtls_sha256_free_CMockIgnore();
    vLoggingPrintf_CMockIgnore();
    xResult = C_DigestUpdate( xSession, pxDummyData, sizeof( pxDummyData ) );
    TEST_ASSERT_EQUAL( CKR_FUNCTION_FAILED, xResult );

    xResult = C_DigestUpdate( xSession, NULL, sizeof( pxDummyData ) );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    xResult = C_DigestUpdate( xSession, pxDummyData, sizeof( pxDummyData ) );
    TEST_ASSERT_EQUAL( CKR_OPERATION_NOT_INITIALIZED, xResult );

    xResult = C_DigestInit( xSession, &xMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );


    prvCommonDeinitStubs();
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


    prvCommonInitStubs();

    mbedtls_sha256_init_CMockIgnore();
    mbedtls_sha256_starts_ret_IgnoreAndReturn( 0 );
    xResult = C_DigestInit( xSession, &xMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = C_DigestFinal( xSession, NULL, &ulDigestLen );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( pkcs11SHA256_DIGEST_LENGTH, ulDigestLen );

    mbedtls_sha256_finish_ret_IgnoreAndReturn( 0 );
    xResult = C_DigestFinal( xSession, pxDummyData, &ulDigestLen );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( pkcs11SHA256_DIGEST_LENGTH, ulDigestLen );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_DigestFinal bad args.
 *
 */
void test_pkcs11_C_DigestFinalBadArgs( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_MECHANISM xMechanism = { 0 };

    xMechanism.mechanism = CKM_SHA256;
    CK_BYTE pxDummyData[] = "Dummy data";
    CK_ULONG ulDigestLen = pkcs11SHA256_DIGEST_LENGTH;


    prvCommonInitStubs();

    mbedtls_sha256_free_CMockIgnore();
    xResult = C_DigestFinal( xSession, pxDummyData, &ulDigestLen );
    TEST_ASSERT_EQUAL( CKR_OPERATION_NOT_INITIALIZED, xResult );

    mbedtls_sha256_init_CMockIgnore();
    mbedtls_sha256_starts_ret_IgnoreAndReturn( 0 );
    xResult = C_DigestInit( xSession, &xMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    vLoggingPrintf_CMockIgnore();
    mbedtls_sha256_finish_ret_IgnoreAndReturn( -1 );
    vLoggingPrintf_CMockIgnore();
    xResult = C_DigestFinal( xSession, pxDummyData, &ulDigestLen );
    TEST_ASSERT_EQUAL( CKR_FUNCTION_FAILED, xResult );
    TEST_ASSERT_EQUAL( pkcs11SHA256_DIGEST_LENGTH, ulDigestLen );



    prvCommonDeinitStubs();
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

    prvCommonInitStubs();

    xResult = prvCreateEcPriv( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 2 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    xResult = PKCS11_PAL_DestroyObject( xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief PKCS11_PAL_DestroyObject PKCS11_PAL_SAVE returns an invalid handle.
 *
 */
void test_pkcs11_PKCS11_PAL_DestroyObjectBadSave( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;

    prvCommonInitStubs();

    xResult = prvCreateEcPub( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( xObject + 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    xResult = PKCS11_PAL_DestroyObject( xObject );
    TEST_ASSERT_EQUAL( CKR_GENERAL_ERROR, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief PKCS11_PAL_DestroyObject PKCS11_PAL_SAVE malloc fail.
 *
 */
void test_pkcs11_PKCS11_PAL_DestroyObjectMemFail( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;

    prvCommonInitStubs();

    xResult = prvCreateEcPub( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    pvPortMalloc_IgnoreAndReturn( NULL );
    vPortFree_CMockIgnore();
    xResult = PKCS11_PAL_DestroyObject( xObject );
    TEST_ASSERT_EQUAL( CKR_HOST_MEMORY, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief PKCS11_PAL_DestroyObject PAL Destroy on a public key.
 *
 */
void test_pkcs11_PKCS11_PAL_DestroyObjectPubKey( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;

    prvCommonInitStubs();

    xResult = prvCreateEcPriv( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCreateEcPub( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    xResult = PKCS11_PAL_DestroyObject( xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief PKCS11_PAL_DestroyObject PAL Destroy on a private key.
 *
 */
void test_pkcs11_PKCS11_PAL_DestroyObjectPrivKey( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;

    prvCommonInitStubs();

    xResult = prvCreateEcPub( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCreateEcPriv( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 2 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    xResult = PKCS11_PAL_DestroyObject( xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
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
    CK_OBJECT_HANDLE xObject = 0;

    prvCommonInitStubs();

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
    xResult = C_SignInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_SignInit ECDSA bad args.
 *
 */
void test_pkcs11_C_SignInitECDSABadArgs( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_MECHANISM xMechanism = { 0 };
    CK_OBJECT_HANDLE xObject = 0;
    CK_BBOOL xIsPrivate = CK_FALSE;

    xMechanism.mechanism = CKM_ECDSA;
    CK_OBJECT_HANDLE xKey = pkcs11configMAX_NUM_OBJECTS + 1;

    prvCommonInitStubs();

    vLoggingPrintf_CMockIgnore();
    xResult = C_SignInit( xSession, NULL, xKey );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_pk_free_CMockIgnore();
    mbedtls_pk_init_CMockIgnore();
    mbedtls_pk_parse_key_IgnoreAndReturn( -1 );
    mbedtls_strerror_highlevel_IgnoreAndReturn( NULL );
    mbedtls_strerror_lowlevel_IgnoreAndReturn( NULL );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    xResult = C_SignInit( xSession, &xMechanism, xKey );
    TEST_ASSERT_EQUAL( CKR_KEY_HANDLE_INVALID, xResult );

    xResult = C_SignInit( xSession, &xMechanism, xKey );
    TEST_ASSERT_EQUAL( CKR_KEY_HANDLE_INVALID, xResult );

    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_RSA );
    xResult = C_SignInit( xSession, &xMechanism, xKey );
    TEST_ASSERT_EQUAL( CKR_KEY_TYPE_INCONSISTENT, xResult );

    xMechanism.mechanism = CKM_RSA_PKCS;

    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_ECDSA );
    xResult = C_SignInit( xSession, &xMechanism, xKey );
    TEST_ASSERT_EQUAL( CKR_KEY_TYPE_INCONSISTENT, xResult );

    xMechanism.mechanism = CKM_RSA_X9_31;

    xResult = C_SignInit( xSession, &xMechanism, xKey );
    TEST_ASSERT_EQUAL( CKR_MECHANISM_INVALID, xResult );

    xMechanism.mechanism = CKM_ECDSA;

    xResult = prvCreateEcPub( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_KEY_HANDLE_INVALID );
    xResult = C_SignInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_KEY_HANDLE_INVALID, xResult );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    xResult = C_SignInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_KEY_TYPE_INCONSISTENT, xResult );


    prvCommonDeinitStubs();
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
    mbedtls_pk_context xSignAndVerifyKey;

    xSignAndVerifyKey.pk_ctx = &xResult;

    xMechanism.mechanism = CKM_ECDSA;
    CK_OBJECT_HANDLE xKey = 0;
    CK_BYTE pxDummyData[ pkcs11SHA256_DIGEST_LENGTH ] = { 0xAA };
    CK_ULONG ulDummyDataLen = sizeof( pxDummyData );
    CK_BYTE pxDummySignature[ pkcs11ECDSA_P256_SIGNATURE_LENGTH ] = { 0xAA };
    CK_ULONG ulDummySignatureLen = sizeof( pxDummySignature );

    prvCommonInitStubs();

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
    mbedtls_pk_sign_ReturnThruPtr_ctx( &xSignAndVerifyKey );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKI_mbedTLSSignatureToPkcs11Signature_IgnoreAndReturn( 0 );
    xResult = C_Sign( xSession, pxDummyData, ulDummyDataLen, pxDummySignature, &ulDummySignatureLen );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_Sign RSA happy path.
 *
 */
void test_pkcs11_C_SignRSA( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_MECHANISM xMechanism = { 0 };
    mbedtls_pk_context xSignAndVerifyKey;

    xSignAndVerifyKey.pk_ctx = &xResult;

    xMechanism.mechanism = CKM_RSA_PKCS;
    CK_OBJECT_HANDLE xKey = 0;
    CK_BYTE pxDummyData[ pkcs11RSA_SIGNATURE_INPUT_LENGTH ] = { 0xAA };
    CK_ULONG ulDummyDataLen = sizeof( pxDummyData );
    CK_BYTE pxDummySignature[ pkcs11RSA_2048_SIGNATURE_LENGTH ] = { 0xAA };
    CK_ULONG ulDummySignatureLen = sizeof( pxDummySignature );

    prvCommonInitStubs();

    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_pk_free_CMockIgnore();
    mbedtls_pk_init_CMockIgnore();
    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_RSA );
    xResult = C_SignInit( xSession, &xMechanism, xKey );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_pk_sign_IgnoreAndReturn( 0 );
    mbedtls_pk_sign_ReturnThruPtr_ctx( &xSignAndVerifyKey );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKI_mbedTLSSignatureToPkcs11Signature_IgnoreAndReturn( 0 );
    xResult = C_Sign( xSession, pxDummyData, ulDummyDataLen, pxDummySignature, &ulDummySignatureLen );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_Sign ECDSA no preceding C_SignInit call.
 *
 */
void test_pkcs11_C_SignNoInit( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;

    CK_BYTE pxDummyData[ pkcs11SHA256_DIGEST_LENGTH ] = { 0xAA };
    CK_ULONG ulDummyDataLen = sizeof( pxDummyData );
    CK_BYTE pxDummySignature[ pkcs11ECDSA_P256_SIGNATURE_LENGTH ] = { 0xAA };
    CK_ULONG ulDummySignatureLen = sizeof( pxDummySignature );

    prvCommonInitStubs();

    xResult = C_Sign( xSession, pxDummyData, ulDummyDataLen, pxDummySignature, &ulDummySignatureLen );
    TEST_ASSERT_EQUAL( CKR_OPERATION_NOT_INITIALIZED, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_Sign Bad args.
 *
 */
void test_pkcs11_C_SignBadArgs( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_MECHANISM xMechanism = { 0 };
    mbedtls_pk_context xSignAndVerifyKey;

    xSignAndVerifyKey.pk_ctx = &xResult;

    xMechanism.mechanism = CKM_ECDSA;
    CK_OBJECT_HANDLE xKey = 0;
    CK_BYTE pxDummyData[ pkcs11SHA256_DIGEST_LENGTH ] = { 0xAA };
    CK_ULONG ulDummyDataLen = sizeof( pxDummyData );
    CK_BYTE pxDummySignature[ pkcs11ECDSA_P256_SIGNATURE_LENGTH ] = { 0xAA };
    CK_ULONG ulDummySignatureLen = sizeof( pxDummySignature );

    prvCommonInitStubs();

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

    mbedtls_pk_sign_IgnoreAndReturn( -1 );
    vLoggingPrintf_CMockIgnore();
    mbedtls_strerror_highlevel_IgnoreAndReturn( NULL );
    mbedtls_strerror_lowlevel_IgnoreAndReturn( NULL );
    xResult = C_Sign( xSession, pxDummyData, ulDummyDataLen, pxDummySignature, &ulDummySignatureLen );
    TEST_ASSERT_EQUAL( CKR_FUNCTION_FAILED, xResult );

    xResult = C_SignInit( xSession, &xMechanism, xKey );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_pk_parse_key_IgnoreAndReturn( 0 );
    mbedtls_pk_sign_IgnoreAndReturn( 0 );
    mbedtls_pk_sign_ReturnThruPtr_ctx( &xSignAndVerifyKey );
    PKI_mbedTLSSignatureToPkcs11Signature_IgnoreAndReturn( -1 );
    xResult = C_Sign( xSession, pxDummyData, ulDummyDataLen, pxDummySignature, &ulDummySignatureLen );
    TEST_ASSERT_EQUAL( CKR_FUNCTION_FAILED, xResult );

    prvCommonDeinitStubs();
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
    mbedtls_pk_context xVerifyKey = { NULL, &xResult };

    xMechanism.mechanism = CKM_ECDSA;
    CK_BBOOL xIsPrivate = CK_FALSE;

    prvCommonInitStubs();

    xResult = prvCreateEcPub( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_pk_free_CMockIgnore();
    mbedtls_pk_init_ExpectAnyArgs();
    mbedtls_pk_init_ReturnThruPtr_ctx( &xVerifyKey );
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 0 );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_ECDSA );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_VerifyInit ECDSA bad args.
 *
 */
void test_pkcs11_C_VerifyInitBadArgs( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    CK_MECHANISM xMechanism = { 0 };

    xMechanism.mechanism = CKM_ECDSA;
    CK_BBOOL xIsPrivate = CK_TRUE;

    prvCommonInitStubs();

    xResult = prvCreateEcPub( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    vLoggingPrintf_CMockIgnore();
    xResult = C_VerifyInit( xSession, NULL, xObject );
    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, xResult );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_KEY_HANDLE_INVALID );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_KEY_HANDLE_INVALID, xResult );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_KEY_TYPE_INCONSISTENT, xResult );

    xIsPrivate = CK_FALSE;

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_pk_free_CMockIgnore();
    mbedtls_pk_init_CMockIgnore();
    mbedtls_pk_parse_public_key_IgnoreAndReturn( -1 );
    mbedtls_pk_parse_key_IgnoreAndReturn( -1 );
    mbedtls_strerror_highlevel_IgnoreAndReturn( NULL );
    mbedtls_strerror_lowlevel_IgnoreAndReturn( NULL );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_KEY_HANDLE_INVALID, xResult );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 0 );
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_RSA );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_KEY_TYPE_INCONSISTENT, xResult );

    xMechanism.mechanism = CKM_RSA_X_509;

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_ECDSA );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_KEY_TYPE_INCONSISTENT, xResult );

    xMechanism.mechanism = CKM_RSA_X9_31;

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_MECHANISM_INVALID, xResult );

    prvCommonDeinitStubs();
}
/* ======================  TESTING C_Verify  ============================ */

/*!
 * @brief C_Verify ECDSA happy path.
 *
 */
void test_pkcs11_C_VerifyECDSA( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    mbedtls_pk_context xVerifyKey = { NULL, &xResult };
    CK_MECHANISM xMechanism = { 0 };
    CK_BYTE pxDummyData[ pkcs11SHA256_DIGEST_LENGTH ] = { 0xAA };
    CK_ULONG ulDummyDataLen = sizeof( pxDummyData );
    CK_BYTE pxDummySignature[ pkcs11ECDSA_P256_SIGNATURE_LENGTH ] = { 0xAA };
    CK_ULONG ulDummySignatureLen = sizeof( pxDummySignature );

    xMechanism.mechanism = CKM_ECDSA;
    CK_BBOOL xIsPrivate = CK_FALSE;

    prvCommonInitStubs();

    xResult = prvCreateEcPub( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_pk_free_CMockIgnore();
    mbedtls_pk_init_ExpectAnyArgs();
    mbedtls_pk_init_ReturnThruPtr_ctx( &xVerifyKey );
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 0 );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_ECDSA );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_pk_verify_IgnoreAndReturn( 0 );
    mbedtls_mpi_init_CMockIgnore();
    mbedtls_mpi_read_binary_IgnoreAndReturn( 0 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_ecdsa_verify_IgnoreAndReturn( 0 );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    mbedtls_mpi_free_CMockIgnore();
    xResult = C_Verify( xSession, pxDummyData, ulDummyDataLen, pxDummySignature, ulDummySignatureLen );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_Verify ECDSA verify failure due to an invalid signature.
 *
 */
void test_pkcs11_C_VerifyECDSAInvalidSig( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    mbedtls_pk_context xVerifyKey = { NULL, &xResult };
    CK_MECHANISM xMechanism = { 0 };
    CK_BYTE pxDummyData[ pkcs11SHA256_DIGEST_LENGTH ] = { 0xAA };
    CK_ULONG ulDummyDataLen = sizeof( pxDummyData );
    CK_BYTE pxDummySignature[ pkcs11ECDSA_P256_SIGNATURE_LENGTH ] = { 0xAA };
    CK_ULONG ulDummySignatureLen = sizeof( pxDummySignature );

    xMechanism.mechanism = CKM_ECDSA;
    CK_BBOOL xIsPrivate = CK_FALSE;

    prvCommonInitStubs();

    xResult = prvCreateEcPub( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_pk_free_CMockIgnore();
    mbedtls_pk_init_ExpectAnyArgs();
    mbedtls_pk_init_ReturnThruPtr_ctx( &xVerifyKey );
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 0 );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_ECDSA );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_pk_verify_IgnoreAndReturn( 0 );
    mbedtls_mpi_init_CMockIgnore();
    mbedtls_mpi_read_binary_IgnoreAndReturn( 0 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_ecdsa_verify_IgnoreAndReturn( -1 );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    mbedtls_mpi_free_CMockIgnore();
    xResult = C_Verify( xSession, pxDummyData, ulDummyDataLen, pxDummySignature, ulDummySignatureLen );
    TEST_ASSERT_EQUAL( CKR_SIGNATURE_INVALID, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_Verify RSA happy path.
 *
 */
void test_pkcs11_C_VerifyRSA( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    CK_MECHANISM xMechanism = { 0 };
    CK_BYTE pxDummyData[ pkcs11RSA_2048_SIGNATURE_LENGTH ] = { 0xAA };
    CK_ULONG ulDummyDataLen = sizeof( pxDummyData );
    CK_BYTE pxDummySignature[ pkcs11RSA_2048_SIGNATURE_LENGTH ] = { 0xAA };
    CK_ULONG ulDummySignatureLen = sizeof( pxDummySignature );

    xMechanism.mechanism = CKM_RSA_X_509;
    CK_BBOOL xIsPrivate = CK_FALSE;

    prvCommonInitStubs();

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
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_RSA );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_pk_verify_IgnoreAndReturn( 0 );
    xResult = C_Verify( xSession, pxDummyData, ulDummyDataLen, pxDummySignature, ulDummySignatureLen );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_Verify Bad args.
 *
 */
void test_pkcs11_C_VerifyBadArgs( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    CK_MECHANISM xMechanism = { 0 };
    mbedtls_pk_context xVerifyKey = { NULL, &xResult };
    CK_BYTE pxDummyData[ pkcs11SHA256_DIGEST_LENGTH ] = { 0xAA };
    CK_ULONG ulDummyDataLen = sizeof( pxDummyData );
    CK_BYTE pxDummySignature[ pkcs11ECDSA_P256_SIGNATURE_LENGTH ] = { 0xAA };
    CK_ULONG ulDummySignatureLen = sizeof( pxDummySignature );

    xMechanism.mechanism = CKM_ECDSA;
    CK_BBOOL xIsPrivate = CK_FALSE;

    prvCommonInitStubs();

    xResult = prvCreateEcPub( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    mbedtls_pk_free_CMockIgnore();
    mbedtls_pk_init_ExpectAnyArgs();
    mbedtls_pk_init_ReturnThruPtr_ctx( &xVerifyKey );
    mbedtls_pk_parse_public_key_IgnoreAndReturn( 0 );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_ECDSA );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    vLoggingPrintf_CMockIgnore();
    xResult = C_Verify( xSession, pxDummyData, 0, pxDummySignature, ulDummySignatureLen );
    TEST_ASSERT_EQUAL( CKR_DATA_LEN_RANGE, xResult );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    mbedtls_pk_init_ExpectAnyArgs();
    mbedtls_pk_init_ReturnThruPtr_ctx( &xVerifyKey );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = C_Verify( xSession, pxDummyData, ulDummyDataLen, pxDummySignature, 0 );
    TEST_ASSERT_EQUAL( CKR_SIGNATURE_LEN_RANGE, xResult );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    mbedtls_pk_init_ExpectAnyArgs();
    mbedtls_pk_init_ReturnThruPtr_ctx( &xVerifyKey );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_mpi_init_CMockIgnore();
    mbedtls_mpi_read_binary_IgnoreAndReturn( -1 );
    mbedtls_strerror_highlevel_IgnoreAndReturn( NULL );
    mbedtls_strerror_lowlevel_IgnoreAndReturn( NULL );
    mbedtls_ecdsa_verify_IgnoreAndReturn( 0 );
    mbedtls_mpi_free_CMockIgnore();
    xResult = C_Verify( xSession, pxDummyData, ulDummyDataLen, pxDummySignature, ulDummySignatureLen );
    TEST_ASSERT_EQUAL( CKR_SIGNATURE_INVALID, xResult );

    xMechanism.mechanism = CKM_RSA_X_509;
    mbedtls_pk_get_type_IgnoreAndReturn( MBEDTLS_PK_RSA );
    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    mbedtls_pk_init_ExpectAnyArgs();
    mbedtls_pk_init_ReturnThruPtr_ctx( &xVerifyKey );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = C_Verify( xSession, pxDummyData, pkcs11RSA_2048_SIGNATURE_LENGTH, pxDummySignature, 0 );
    TEST_ASSERT_EQUAL( CKR_SIGNATURE_LEN_RANGE, xResult );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    mbedtls_pk_init_ExpectAnyArgs();
    mbedtls_pk_init_ReturnThruPtr_ctx( &xVerifyKey );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = C_Verify( xSession, pxDummyData, 0, pxDummySignature, pkcs11RSA_2048_SIGNATURE_LENGTH );
    TEST_ASSERT_EQUAL( CKR_DATA_LEN_RANGE, xResult );

    PKCS11_PAL_GetObjectValue_ExpectAnyArgsAndReturn( CKR_OK );
    PKCS11_PAL_GetObjectValue_ReturnThruPtr_pIsPrivate( &xIsPrivate );
    mbedtls_pk_init_ExpectAnyArgs();
    mbedtls_pk_init_ReturnThruPtr_ctx( &xVerifyKey );
    xResult = C_VerifyInit( xSession, &xMechanism, xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    mbedtls_pk_verify_IgnoreAndReturn( -1 );
    xResult = C_Verify( xSession, pxDummyData, pkcs11RSA_2048_SIGNATURE_LENGTH, pxDummySignature, pkcs11RSA_2048_SIGNATURE_LENGTH );
    TEST_ASSERT_EQUAL( CKR_SIGNATURE_INVALID, xResult );

    prvCommonDeinitStubs();
}
/* ======================  TESTING C_GenerateKeyPair  ============================ */

/*!
 * @brief C_GenerateKeyPair ECDSA happy path.
 *
 */
void test_pkcs11_C_GenerateKeyPairECDSA( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xPrivKeyHandle = 0;
    CK_OBJECT_HANDLE xPubKeyHandle = 0;

    prvCommonInitStubs();

    char * pucPublicKeyLabel = pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
    char * pucPrivateKeyLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;

    CK_MECHANISM xMechanism =
    {
        CKM_EC_KEY_PAIR_GEN, NULL_PTR, 0
    };
    CK_BYTE xEcParams[] = pkcs11DER_ENCODED_OID_P256; /* prime256v1 */
    CK_KEY_TYPE xKeyType = CKK_EC;

    CK_BBOOL xTrue = CK_TRUE;
    CK_ATTRIBUTE xPublicKeyTemplate[] =
    {
        { CKA_KEY_TYPE,  &xKeyType,         sizeof( xKeyType )                           },
        { CKA_VERIFY,    &xTrue,            sizeof( xTrue )                              },
        { CKA_EC_PARAMS, xEcParams,         sizeof( xEcParams )                          },
        { CKA_LABEL,     pucPublicKeyLabel, strlen( ( const char * ) pucPublicKeyLabel ) }
    };

    CK_ATTRIBUTE xPrivateKeyTemplate[] =
    {
        { CKA_KEY_TYPE, &xKeyType,          sizeof( xKeyType )                            },
        { CKA_TOKEN,    &xTrue,             sizeof( xTrue )                               },
        { CKA_PRIVATE,  &xTrue,             sizeof( xTrue )                               },
        { CKA_SIGN,     &xTrue,             sizeof( xTrue )                               },
        { CKA_LABEL,    pucPrivateKeyLabel, strlen( ( const char * ) pucPrivateKeyLabel ) }
    };

    mbedtls_pk_init_CMockIgnore();
    mbedtls_pk_info_from_type_IgnoreAndReturn( 0 );
    mbedtls_pk_setup_IgnoreAndReturn( 0 );
    mbedtls_ecp_gen_key_IgnoreAndReturn( 0 );
    mbedtls_pk_write_pubkey_der_IgnoreAndReturn( 1 );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    mbedtls_pk_write_key_der_IgnoreAndReturn( 1 );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 2 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    vPortFree_Stub( vPkcs11FreeCb );
    mbedtls_pk_free_CMockIgnore();
    xResult = C_GenerateKeyPair( xSession, &xMechanism, xPublicKeyTemplate,
                                 sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 xPrivateKeyTemplate, sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 &xPubKeyHandle, &xPrivKeyHandle );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_GenerateKeyPair ECDSA can't lock semaphore
 *
 */
void test_pkcs11_C_GenerateKeyPairECDSALockFail( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xPrivKeyHandle = 0;
    CK_OBJECT_HANDLE xPubKeyHandle = 0;

    prvCommonInitStubs();

    char * pucPublicKeyLabel = pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
    char * pucPrivateKeyLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;

    CK_MECHANISM xMechanism =
    {
        CKM_EC_KEY_PAIR_GEN, NULL_PTR, 0
    };
    CK_BYTE xEcParams[] = pkcs11DER_ENCODED_OID_P256; /* prime256v1 */
    CK_KEY_TYPE xKeyType = CKK_EC;

    CK_BBOOL xTrue = CK_TRUE;
    CK_ATTRIBUTE xPublicKeyTemplate[] =
    {
        { CKA_KEY_TYPE,  &xKeyType,         sizeof( xKeyType )                           },
        { CKA_VERIFY,    &xTrue,            sizeof( xTrue )                              },
        { CKA_EC_PARAMS, xEcParams,         sizeof( xEcParams )                          },
        { CKA_LABEL,     pucPublicKeyLabel, strlen( ( const char * ) pucPublicKeyLabel ) }
    };

    CK_ATTRIBUTE xPrivateKeyTemplate[] =
    {
        { CKA_KEY_TYPE, &xKeyType,          sizeof( xKeyType )                            },
        { CKA_TOKEN,    &xTrue,             sizeof( xTrue )                               },
        { CKA_PRIVATE,  &xTrue,             sizeof( xTrue )                               },
        { CKA_SIGN,     &xTrue,             sizeof( xTrue )                               },
        { CKA_LABEL,    pucPrivateKeyLabel, strlen( ( const char * ) pucPrivateKeyLabel ) }
    };

    mbedtls_pk_init_CMockIgnore();
    mbedtls_pk_info_from_type_IgnoreAndReturn( 0 );
    mbedtls_pk_setup_IgnoreAndReturn( 0 );
    mbedtls_ecp_gen_key_IgnoreAndReturn( 0 );
    mbedtls_pk_write_pubkey_der_IgnoreAndReturn( 1 );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    mbedtls_pk_write_key_der_IgnoreAndReturn( 1 );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 2 );
    xQueueSemaphoreTake_ExpectAnyArgsAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    xQueueSemaphoreTake_ExpectAnyArgsAndReturn( pdFALSE );
    vPortFree_Stub( vPkcs11FreeCb );
    mbedtls_pk_free_CMockIgnore();
    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 2 );
    xQueueSemaphoreTake_ExpectAnyArgsAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    xResult = C_GenerateKeyPair( xSession, &xMechanism, xPublicKeyTemplate,
                                 sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 xPrivateKeyTemplate, sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 &xPubKeyHandle, &xPrivKeyHandle );
    TEST_ASSERT_EQUAL( CKR_CANT_LOCK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_GenerateKeyPair ECDSA private key attribute not set to private.
 *
 */
void test_pkcs11_C_GenerateKeyPairECDSABadPrivateKeyParam( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xPrivKeyHandle = 0;
    CK_OBJECT_HANDLE xPubKeyHandle = 0;

    prvCommonInitStubs();

    char * pucPublicKeyLabel = pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
    char * pucPrivateKeyLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;

    CK_MECHANISM xMechanism =
    {
        CKM_EC_KEY_PAIR_GEN, NULL_PTR, 0
    };
    CK_BYTE xEcParams[] = pkcs11DER_ENCODED_OID_P256; /* prime256v1 */
    CK_KEY_TYPE xKeyType = CKK_EC;

    CK_BBOOL xTrue = CK_TRUE;
    CK_BBOOL xFalse = CK_FALSE;
    CK_ATTRIBUTE xPublicKeyTemplate[] =
    {
        { CKA_KEY_TYPE,  &xKeyType,         sizeof( xKeyType )                           },
        { CKA_VERIFY,    &xTrue,            sizeof( xTrue )                              },
        { CKA_EC_PARAMS, xEcParams,         sizeof( xEcParams )                          },
        { CKA_LABEL,     pucPublicKeyLabel, strlen( ( const char * ) pucPublicKeyLabel ) }
    };

    CK_ATTRIBUTE xPrivateKeyTemplate[] =
    {
        { CKA_KEY_TYPE, &xKeyType,          sizeof( xKeyType )                            },
        { CKA_TOKEN,    &xTrue,             sizeof( xTrue )                               },
        { CKA_PRIVATE,  &xFalse,            sizeof( xFalse )                              },
        { CKA_SIGN,     &xTrue,             sizeof( xTrue )                               },
        { CKA_LABEL,    pucPrivateKeyLabel, strlen( ( const char * ) pucPrivateKeyLabel ) }
    };

    mbedtls_pk_init_CMockIgnore();
    mbedtls_pk_info_from_type_IgnoreAndReturn( 0 );
    mbedtls_pk_setup_IgnoreAndReturn( 0 );
    mbedtls_ecp_gen_key_IgnoreAndReturn( 0 );
    mbedtls_pk_write_pubkey_der_IgnoreAndReturn( 1 );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 1 );
    mbedtls_pk_write_key_der_IgnoreAndReturn( 1 );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 2 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    vPortFree_Stub( vPkcs11FreeCb );
    mbedtls_pk_free_CMockIgnore();
    xResult = C_GenerateKeyPair( xSession, &xMechanism, xPublicKeyTemplate,
                                 sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 xPrivateKeyTemplate, sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 &xPubKeyHandle, &xPrivKeyHandle );
    TEST_ASSERT_EQUAL( CKR_TEMPLATE_INCONSISTENT, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_GenerateKeyPair Bad Args.
 *
 */
void test_pkcs11_C_GenerateKeyPairBadArgs( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xPrivKeyHandle = 0;
    CK_OBJECT_HANDLE xPubKeyHandle = 0;

    prvCommonInitStubs();

    char * pucPublicKeyLabel = pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
    char * pucPrivateKeyLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;

    CK_MECHANISM xMechanism =
    {
        CKM_EC_KEY_PAIR_GEN, NULL_PTR, 0
    };
    CK_BYTE xEcParams[] = pkcs11DER_ENCODED_OID_P256;
    CK_BYTE xEcGarbageParams[ sizeof( xEcParams ) ] = { 0xAA };
    CK_KEY_TYPE xKeyType = CKK_EC;
    CK_KEY_TYPE xBadKeyType = CKK_RSA;

    CK_BBOOL xTrue = CK_TRUE;
    CK_BBOOL xFalse = CK_FALSE;
    CK_ATTRIBUTE xPublicKeyTemplate[] =
    {
        { CKA_KEY_TYPE,  &xKeyType,         sizeof( xKeyType )                           },
        { CKA_VERIFY,    &xTrue,            sizeof( xTrue )                              },
        { CKA_EC_PARAMS, xEcParams,         sizeof( xEcParams )                          },
        { CKA_LABEL,     pucPublicKeyLabel, strlen( ( const char * ) pucPublicKeyLabel ) }
    };

    CK_ATTRIBUTE xPrivateKeyTemplate[] =
    {
        { CKA_KEY_TYPE, &xKeyType,          sizeof( xKeyType )                            },
        { CKA_TOKEN,    &xTrue,             sizeof( xTrue )                               },
        { CKA_PRIVATE,  &xTrue,             sizeof( xTrue )                               },
        { CKA_SIGN,     &xTrue,             sizeof( xTrue )                               },
        { CKA_LABEL,    pucPrivateKeyLabel, strlen( ( const char * ) pucPrivateKeyLabel ) }
    };

    vLoggingPrintf_CMockIgnore();
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    mbedtls_pk_free_CMockIgnore();
    xPrivateKeyTemplate[ 0 ].pValue = &xBadKeyType;
    xResult = C_GenerateKeyPair( xSession, &xMechanism, xPublicKeyTemplate,
                                 sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 xPrivateKeyTemplate, sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 &xPubKeyHandle, &xPrivKeyHandle );
    TEST_ASSERT_EQUAL( CKR_TEMPLATE_INCONSISTENT, xResult );
    xPrivateKeyTemplate[ 0 ].pValue = &xKeyType;

    xPublicKeyTemplate[ 2 ].pValue = &xEcGarbageParams;
    xResult = C_GenerateKeyPair( xSession, &xMechanism, xPublicKeyTemplate,
                                 sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 xPrivateKeyTemplate, sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 &xPubKeyHandle, &xPrivKeyHandle );
    TEST_ASSERT_EQUAL( CKR_TEMPLATE_INCONSISTENT, xResult );
    xPublicKeyTemplate[ 2 ].pValue = &xEcParams;

    xResult = C_GenerateKeyPair( xSession, &xMechanism, xPublicKeyTemplate,
                                 sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 xPrivateKeyTemplate,
                                 ( sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ) ) - 1,
                                 &xPubKeyHandle, &xPrivKeyHandle );
    TEST_ASSERT_EQUAL( CKR_TEMPLATE_INCOMPLETE, xResult );

    xPrivateKeyTemplate[ 3 ].pValue = &xFalse;
    xResult = C_GenerateKeyPair( xSession, &xMechanism, xPublicKeyTemplate,
                                 sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 xPrivateKeyTemplate, sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 &xPubKeyHandle, &xPrivKeyHandle );
    TEST_ASSERT_EQUAL( CKR_TEMPLATE_INCONSISTENT, xResult );
    xPrivateKeyTemplate[ 3 ].pValue = &xTrue;

    xPrivateKeyTemplate[ 1 ].pValue = &xFalse;
    xResult = C_GenerateKeyPair( xSession, &xMechanism, xPublicKeyTemplate,
                                 sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 xPrivateKeyTemplate, sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 &xPubKeyHandle, &xPrivKeyHandle );
    TEST_ASSERT_EQUAL( CKR_TEMPLATE_INCONSISTENT, xResult );
    xPrivateKeyTemplate[ 1 ].pValue = &xTrue;

    xPublicKeyTemplate[ 1 ].pValue = &xFalse;
    xResult = C_GenerateKeyPair( xSession, &xMechanism, xPublicKeyTemplate,
                                 sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 xPrivateKeyTemplate, sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 &xPubKeyHandle, &xPrivKeyHandle );
    TEST_ASSERT_EQUAL( CKR_TEMPLATE_INCONSISTENT, xResult );
    xPublicKeyTemplate[ 1 ].pValue = &xTrue;

    xPublicKeyTemplate[ 0 ].pValue = &xBadKeyType;
    xResult = C_GenerateKeyPair( xSession, &xMechanism, xPublicKeyTemplate,
                                 sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 xPrivateKeyTemplate, sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                 &xPubKeyHandle, &xPrivKeyHandle );
    TEST_ASSERT_EQUAL( CKR_TEMPLATE_INCONSISTENT, xResult );
    xPublicKeyTemplate[ 0 ].pValue = &xKeyType;

    prvCommonDeinitStubs();
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

    prvCommonInitStubs();

    mbedtls_ctr_drbg_random_IgnoreAndReturn( 0 );
    xResult = C_GenerateRandom( xSession, ucRandData, ulRandLen );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}

/*!
 * @brief C_GenerateRandom drbg failed.
 *
 */
void test_pkcs11_C_GenerateRandomDrbgFail( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_BYTE ucRandData[ 3 ] = { 0 };
    CK_ULONG ulRandLen = sizeof( ucRandData );

    prvCommonInitStubs();

    mbedtls_ctr_drbg_random_IgnoreAndReturn( -1 );
    vLoggingPrintf_CMockIgnore();
    xResult = C_GenerateRandom( xSession, ucRandData, ulRandLen );
    TEST_ASSERT_EQUAL( CKR_FUNCTION_FAILED, xResult );

    prvCommonDeinitStubs();
}

/* ======================  TESTING C_DestroyObject  ============================ */

/*!
 * @brief C_GenerateRandom happy path.
 *
 */
void test_pkcs11_C_DestroyObject( void )
{
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;

    prvCommonInitStubs();

    xResult = prvCreateEcPub( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    xResult = prvCreateEcPriv( &xSession, &xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    PKCS11_PAL_GetObjectValue_IgnoreAndReturn( CKR_OK );
    pvPortMalloc_Stub( pvPkcs11MallocCb );
    vPortFree_Stub( vPkcs11FreeCb );
    PKCS11_PAL_SaveObject_IgnoreAndReturn( 2 );
    xQueueSemaphoreTake_IgnoreAndReturn( pdTRUE );
    xQueueGenericSend_IgnoreAndReturn( pdTRUE );
    PKCS11_PAL_GetObjectValueCleanup_CMockIgnore();
    xResult = C_DestroyObject( xSession, xObject );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    prvCommonDeinitStubs();
}
