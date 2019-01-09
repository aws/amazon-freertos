/*
 * Amazon FreeRTOS PKCS11 AFQP V1.1.4
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
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "event_groups.h"
#include "aws_pki_utils.h"

/* Crypto includes. */
#include "aws_crypto.h"
#include "aws_clientcredential.h"
#include "aws_pkcs11.h"
#include "aws_dev_mode_key_provisioning.h"
#include "aws_test_pkcs11_config.h"

#if ( pkcs11testRSA_KEY_SUPPORT == 0 ) && ( pkcs11testEC_KEY_SUPPORT == 0 )
    #error "RSA or Elliptic curve keys (or both) must be supported."
#endif

#if ( pkcs11testCREATE_OBJECT_SUPPORT == 0 ) && ( pkcs11testGENERATE_KEYPAIR_SUPPORT == 0 )
    #error "Either key pair import or key pair generation must be supported."
#endif


#include "aws_pkcs11_config.h"

/* Test includes. */
#include "unity_fixture.h"
#include "unity.h"

/* mbedTLS includes. */
#include "mbedtls/sha256.h"
#include "mbedtls/pk.h"
#include "mbedtls/oid.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"


typedef enum
{
    eNone,                /* Device is not provisioned.  All credentials have been destroyed. */
    eRsaTest,             /* Provisioned using the RSA test credentials located in this file. */
    eEllipticCurveTest,   /* Provisioned using EC test credentials located in this file. */
    eClientCredential,    /* Provisioned using the credentials in aws_clientcredential_keys. */
    eGeneratedEc,         /* Provisioned using elliptic curve generated on device.  Private key unknown.  No corresponding certificate. */
    eGeneratedRsa,
    eDeliberatelyInvalid, /* Provisioned using credentials that are meant to trigger an error condition. */
    eStateUnknown         /* State of the credentials is unknown. */
} CredentialsProvisioned_t;

/* PKCS #11 Globals.
 * These are used to reduce setup and tear down calls, and to
 * prevent memory leaks in the case of TEST_PROTECT() actions being triggered. */
CK_SESSION_HANDLE xGlobalSession;
CK_FUNCTION_LIST_PTR pxGlobalFunctionList;
CredentialsProvisioned_t xCurrentCredentials = eStateUnknown;

/* Function Prototypes. */
CK_RV prvBeforeRunningTests( void );
void prvAfterRunningTests( void );

/* Test buffer size definitions. */
#define SHA256_DIGEST_SIZE      32
#define ECDSA_SIGNATURE_SIZE    64
#define RSA_SIGNATURE_SIZE      256

/* The StartFinish test group is for General Purpose,
 * Session, Slot, and Token management functions.
 * These tests do not require provisioning. */
TEST_GROUP( Full_PKCS11_StartFinish );

/* The NoKey test group is for test of cryptographic functionality
 * that do not require keys.  Covers digesting and randomness.
 * These tests do not require provisioning. */
TEST_GROUP( Full_PKCS11_NoObject );
/* The RSA test group is for tests that require RSA keys. */
TEST_GROUP( Full_PKCS11_RSA );
/* The EC test group is for tests that require elliptic curve keys. */
TEST_GROUP( Full_PKCS11_EC );

/* #define PKCS11_TEST_MEMORY_LEAK */
#ifdef PKCS11_TEST_MEMORY_LEAK
    BaseType_t xHeapBefore;
    BaseType_t xHeapAfter;
#endif

TEST_SETUP( Full_PKCS11_StartFinish )
{
    #ifdef PKCS11_TEST_MEMORY_LEAK
        /* Give the print buffer time to empty */
        vTaskDelay( 500 );
        xHeapBefore = xPortGetFreeHeapSize();
    #endif
}

TEST_TEAR_DOWN( Full_PKCS11_StartFinish )
{
    #ifdef PKCS11_TEST_MEMORY_LEAK
        /* Give the print buffer time to empty */
        vTaskDelay( 500 );
        xHeapAfter = xPortGetFreeHeapSize();
        configPRINTF( ( "Heap before %d, Heap After %d, Difference %d \r\n", xHeapBefore, xHeapAfter, ( xHeapAfter - xHeapBefore ) ) );
    #endif
}

TEST_GROUP_RUNNER( Full_PKCS11_StartFinish )
{
    RUN_TEST_CASE( Full_PKCS11_StartFinish, AFQP_StartFinish_FirstTest );
    RUN_TEST_CASE( Full_PKCS11_StartFinish, AFQP_GetFunctionList );
    RUN_TEST_CASE( Full_PKCS11_StartFinish, AFQP_InitializeFinalize );
    RUN_TEST_CASE( Full_PKCS11_StartFinish, AFQP_GetSlotList );
    RUN_TEST_CASE( Full_PKCS11_StartFinish, AFQP_OpenSessionCloseSession );
    RUN_TEST_CASE( Full_PKCS11_StartFinish, AFQP_InitializeMultiThread );

    prvAfterRunningTests();
}

TEST_SETUP( Full_PKCS11_NoObject )
{
    #ifdef PKCS11_TEST_MEMORY_LEAK
        /* Give the print buffer time to empty */
        vTaskDelay( 500 );
        xHeapBefore = xPortGetFreeHeapSize();
    #endif
    CK_RV xResult;

    xResult = xInitializePkcs11Session( &xGlobalSession ); /*TODO: update to take a slot*/
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to initialize PKCS #11 session." );
}

TEST_TEAR_DOWN( Full_PKCS11_NoObject )
{
    CK_RV xResult;

    xResult = pxGlobalFunctionList->C_CloseSession( xGlobalSession );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to close session." );
    xResult = pxGlobalFunctionList->C_Finalize( NULL );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to finalize session." );

    #ifdef PKCS11_TEST_MEMORY_LEAK
        /* Give the print buffer time to empty */
        vTaskDelay( 500 );
        xHeapAfter = xPortGetFreeHeapSize();
        configPRINTF( ( "Heap before %d, Heap After %d, Difference %d \r\n", xHeapBefore, xHeapAfter, ( xHeapAfter - xHeapBefore ) ) );
    #endif
}


TEST_GROUP_RUNNER( Full_PKCS11_NoObject )
{
    prvBeforeRunningTests();

    RUN_TEST_CASE( Full_PKCS11_NoObject, AFQP_Digest );
    RUN_TEST_CASE( Full_PKCS11_NoObject, AFQP_Digest_ErrorConditions );
    RUN_TEST_CASE( Full_PKCS11_NoObject, AFQP_GenerateRandom );
    RUN_TEST_CASE( Full_PKCS11_NoObject, AFQP_GenerateRandomMultiThread );


    prvAfterRunningTests();
}

TEST_SETUP( Full_PKCS11_RSA )
{
    #ifdef PKCS11_TEST_MEMORY_LEAK
        /* Give the print buffer time to empty */
        vTaskDelay( 500 );
        xHeapBefore = xPortGetFreeHeapSize();
    #endif

    CK_RV xResult;
    xResult = xInitializePkcs11Session( &xGlobalSession );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to initialize PKCS #11 session." );
}

TEST_TEAR_DOWN( Full_PKCS11_RSA )
{
    CK_RV xResult;

    xResult = pxGlobalFunctionList->C_CloseSession( xGlobalSession );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to close session." );

    xResult = pxGlobalFunctionList->C_Finalize( NULL );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to finalize session." );

    #ifdef PKCS11_TEST_MEMORY_LEAK
        /* Give the print buffer time to empty */
        vTaskDelay( 500 );
        xHeapAfter = xPortGetFreeHeapSize();
        configPRINTF( ( "Heap before %d, Heap After %d, Difference %d \r\n", xHeapBefore, xHeapAfter, ( xHeapAfter - xHeapBefore ) ) );
    #endif
}

TEST_GROUP_RUNNER( Full_PKCS11_RSA )
{
    prvBeforeRunningTests();
    ( void ) xDestroyCredentials( xGlobalSession );
    xCurrentCredentials = eNone;

    RUN_TEST_CASE( Full_PKCS11_RSA, AFQP_GenerateKeyPair );
    RUN_TEST_CASE( Full_PKCS11_RSA, AFQP_CreateObjectFindObject );
    RUN_TEST_CASE( Full_PKCS11_RSA, AFQP_CreateObjectGetAttributeValue );
    RUN_TEST_CASE( Full_PKCS11_RSA, AFQP_Sign );

    prvAfterRunningTests();
}

TEST_SETUP( Full_PKCS11_EC )
{
    #ifdef PKCS11_TEST_MEMORY_LEAK
        /* Give the print buffer time to empty */
        vTaskDelay( 500 );
        xHeapBefore = xPortGetFreeHeapSize();
    #endif

    CK_RV xResult;

    xResult = xInitializePkcs11Session( &xGlobalSession );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to initialize PKCS #11 session." );
}

TEST_TEAR_DOWN( Full_PKCS11_EC )
{
    CK_RV xResult;

    xResult = pxGlobalFunctionList->C_CloseSession( xGlobalSession );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to close session." );

    xResult = pxGlobalFunctionList->C_Finalize( NULL );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to finalize session." );

    #ifdef PKCS11_TEST_MEMORY_LEAK
        /* Give the print buffer time to empty */
        vTaskDelay( 500 );
        xHeapAfter = xPortGetFreeHeapSize();
        configPRINTF( ( "Heap before %d, Heap After %d, Difference %d \r\n", xHeapBefore, xHeapAfter, ( xHeapAfter - xHeapBefore ) ) );
    #endif
}

TEST_GROUP_RUNNER( Full_PKCS11_EC )
{
    prvBeforeRunningTests();

    RUN_TEST_CASE( Full_PKCS11_EC, AFQP_FindObject );
    RUN_TEST_CASE( Full_PKCS11_EC, AFQP_GetAttributeValue );
    RUN_TEST_CASE( Full_PKCS11_EC, AFQP_Sign );
    RUN_TEST_CASE( Full_PKCS11_EC, AFQP_Verify );
    RUN_TEST_CASE( Full_PKCS11_EC, AFQP_GenerateKeyPair );


    prvAfterRunningTests();
}

/* Data structure to store results of multi-thread tests. */
typedef struct MultithreadTaskParams
{
    BaseType_t xTaskNumber;
    CK_RV xTestResult;
    void * pvTaskData;
} MultithreadTaskParams_t;

/* Event group used to synchronize tasks. */
static EventGroupHandle_t xSyncEventGroup;
static MultithreadTaskParams_t xGlobalTaskParams[ pkcs11testMULTI_THREAD_TASK_COUNT ];

/*-----------------------------------------------------------*/
/*           Multitask loop configuration.                   */
/*-----------------------------------------------------------*/
/* Stack size of each task. This can be configured in aws_test_pkcs11_config.h. */
#ifndef pkcs11testMULTI_TASK_STACK_SIZE
    #define pkcs11testMULTI_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 4 )
#endif

/* Priority of each task. This can be configured in aws_test_pkcs11_config.h. */
#ifndef pkcs11testMULTI_TASK_PRIORITY
    #define pkcs11testMULTI_TASK_PRIORITY    ( tskIDLE_PRIORITY )
#endif

/* Specifies bits for all tasks to the event group. */
#define pkcs11testALL_BITS    ( ( 1 << pkcs11testMULTI_THREAD_TASK_COUNT ) - 1 )


CK_RV prvBeforeRunningTests( void )
{
    CK_RV xResult;

    /* Initialize the function list */
    xResult = C_GetFunctionList( &pxGlobalFunctionList );

    if( xResult == CKR_OK )
    {
        /* Close the last session if it was not closed already. */
        pxGlobalFunctionList->C_Finalize( NULL );
    }

    return xResult;
}

void prvAfterRunningTests( void )
{
    /* Re-provision the device with default RSA certs so that subsequent tests are not changed. */
    vDevModeKeyProvisioning();
    xCurrentCredentials = eClientCredential;
}

/*void prvProvisionTestCredentials( CK_OBJECT_HANDLE_PTR pxPrivateKeyHandle, */
/*    CK_OBJECT_HANDLE_PTR pxCertificateHandle, */
/*    CK_KEY_TYPE xKeyType) */
/*{ */
/*    CK_RV xResult; */
/*    CredentialsProvisioned_t xDesiredCredentials; */
/*    int lReprovision = 1; */
/* */
/*    if ( xKeyType == CKK_RSA ) */
/*    { */
/*        xDesiredCredentials = eRsaTest; */
/*    } */
/*    else if ( xKeyType == CKK_EC ) */
/*    { */
/*        xDesiredCredentials = eEllipticCurveTest; */
/*    } */
/*    else */
/*    { */
/*        TEST_FAIL_MESSAGE( "Invalid credential provisioning request." ); */
/*    } */
/* */
/*    if ( xCurrentCredentials != xDesiredCredentials ) */
/*    { */
/*        xResult = xDestroyCredentials( xGlobalSession ); */
/*        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to destroy credentials in test setup." ); */
/*        xCurrentCredentials = eNone; */
/* */
/*        / * Create a private key. * / */
/*        xResult = xProvisionPrivateKey( xGlobalSession, */
/*            ( uint8_t * ) cValidRSAPrivateKey, */
/*            sizeof( cValidRSAPrivateKey ), */
/*            xKeyType, */
/*            pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, */
/*            pxPrivateKeyHandle ); */
/* */
/*        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to create RSA private key." ); */
/*        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, *pxPrivateKeyHandle, "Invalid object handle returned for RSA private key." ); */
/* */
/*        / * Create a certificate. * / */
/*        xResult = xProvisionCertificate( xGlobalSession, */
/*            ( uint8_t * ) cValidRSACertificate, */
/*            sizeof( cValidRSACertificate ), */
/*            pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, */
/*            pxCertificateHandle ); */
/* */
/*        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to create test certificate." ); */
/*        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, *pxCertificateHandle, "Invalid object handle returned for test certificate." ); */
/* */
/*        xCurrentCredentials = xDesiredCredentials; */
/*    } */
/*    else */
/*    { */
/*        xResult = xFindObjectWithLabel( xGlobalSession, pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, pxPrivateKeyHandle ); */
/*        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find test private key." ); */
/*        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, pxPrivateKeyHandle, "Invalid object handle found for test private key." ); */
/* */
/*        xResult = xFindObjectWithLabel( xGlobalSession, pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, pxCertificateHandle ); */
/*        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find test certificate." ); */
/*        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, pxCertificateHandle, "Invalid object handle found for test certificate." ); */
/*    } */
/*} */


static void prvMultiThreadHelper( void * pvTaskFxnPtr )
{
    BaseType_t xTaskNumber;

    /* Create the event group used to synchronize tasks. */
    xSyncEventGroup = xEventGroupCreate();

    if( xSyncEventGroup != NULL )
    {
        /* Create the sign verify loop tasks. */
        for( xTaskNumber = 0; xTaskNumber < pkcs11testMULTI_THREAD_TASK_COUNT; xTaskNumber++ )
        {
            xGlobalTaskParams[ xTaskNumber ].xTaskNumber = xTaskNumber;
            xGlobalTaskParams[ xTaskNumber ].xTestResult = 0xFFFFFFFF; /* Initialize to a non-zero value. */

            xTaskCreate( pvTaskFxnPtr,                                 /* Task code. */
                         "Multithread",                                /* All tasks have same name, but are distinguished by task number. */
                         pkcs11testMULTI_TASK_STACK_SIZE,              /* Task stack size. */
                         &( xGlobalTaskParams[ xTaskNumber ] ),        /* Where the task writes its result. */
                         pkcs11testMULTI_TASK_PRIORITY,                /* Task priority. */
                         NULL );                                       /* Task handle (not used). */
        }

        /* Wait for all tasks to finish. */
        if( xEventGroupWaitBits( xSyncEventGroup,
                                 pkcs11testALL_BITS,
                                 pdFALSE,
                                 pdTRUE,
                                 pkcs11testEVENT_GROUP_TIMEOUT_MS ) != pkcs11testALL_BITS )
        {
            configPRINTF( ( "Timed out waiting for tasks to finish in multi thread test.\r\n" ) );
        }

        vEventGroupDelete( xSyncEventGroup );
    }

    /* Check the tasks' results. */
    if( TEST_PROTECT() )
    {
        for( xTaskNumber = 0; xTaskNumber < pkcs11testMULTI_THREAD_TASK_COUNT; xTaskNumber++ )
        {
            if( xGlobalTaskParams[ xTaskNumber ].xTestResult != 0 )
            {
                configPRINTF( ( "Multi thread task %d returned failure.\r\n",
                                xGlobalTaskParams[ xTaskNumber ].xTaskNumber ) );
                TEST_FAIL();
            }
        }
    }
}


/* Assumes that device is already provisioned at time of calling. */
void prvFindObjectTest( void )
{
    CK_RV xResult;
    CK_OBJECT_HANDLE xPrivateKeyHandle;
    CK_OBJECT_HANDLE xPublicKeyHandle;
    CK_OBJECT_HANDLE xCertificateHandle;
    CK_OBJECT_HANDLE xTestObjectHandle;

    /* Happy Path - Find a previously created object. */
    xResult = xFindObjectWithLabel( xGlobalSession,
                                    pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                    &xPrivateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find private key after closing and reopening a session." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, xPrivateKeyHandle, "Invalid object handle found for  private key." );

    /*         TODO: Add the code sign key and root ca. */
    xResult = xFindObjectWithLabel( xGlobalSession,
                                    pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                                    &xPublicKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find public key after closing and reopening a session." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, xPublicKeyHandle, "Invalid object handle found for public key key." );


    xResult = xFindObjectWithLabel( xGlobalSession,
                                    pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                    &xCertificateHandle );

    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find certificate after closing and reopening a session." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, xCertificateHandle, "Invalid object handle found for certificate." );

    /* Try to find an object that has never been created. */
    xResult = xFindObjectWithLabel( xGlobalSession,
                                    "This label doesn't exist",
                                    &xTestObjectHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Incorrect error code finding object that doesn't exist" );
    TEST_ASSERT_EQUAL_MESSAGE( pkcs11INVALID_OBJECT_HANDLE, xTestObjectHandle, "Incorrect error code finding object that doesn't exist" );

    /* Destroy the private key and try to find it. */
    xCurrentCredentials = eStateUnknown;
    xResult = pxGlobalFunctionList->C_DestroyObject( xGlobalSession, xPrivateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Error destroying private key" );
    xResult = xFindObjectWithLabel( xGlobalSession,
                                    pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                    &xPrivateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failure searching for destroyed object." );
    TEST_ASSERT_EQUAL_MESSAGE( 0, xPrivateKeyHandle, "Object found after it was destroyed." );

    /* Make sure the certificate can still be found. */
    xResult = xFindObjectWithLabel( xGlobalSession,
                                    pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                    &xCertificateHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find certificate after destroying private key." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, xCertificateHandle, "Invalid object handle found for certificate." );

    xResult = pxGlobalFunctionList->C_DestroyObject( xGlobalSession, xCertificateHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Error destroying public key" );
    xResult = xFindObjectWithLabel( xGlobalSession,
                                    pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                    &xPrivateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failure searching for destroyed object." );
    TEST_ASSERT_EQUAL_MESSAGE( 0, xPrivateKeyHandle, "Object found after it was destroyed." );
}


TEST( Full_PKCS11_StartFinish, AFQP_StartFinish_FirstTest )
{
    CK_RV xResult;

    /* Finalize the PKCS #11 module to get it in a known state.
     * Set up the PKCS #11 function list pointer. */
    xResult = prvBeforeRunningTests();

    /* prvBeforeRunningTests finalizes the PKCS #11 modules so that tests will start
     * in a known state.  It is OK if the module was not previously initialized. */
    if( xResult == CKR_CRYPTOKI_NOT_INITIALIZED )
    {
        xResult = CKR_OK;
    }

    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Setup for the PKCS #11 routine failed.  Test module will start in an unknown state." );
}

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_StartFinish, AFQP_GetFunctionList )
{
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, C_GetFunctionList( NULL ) );

    TEST_ASSERT_EQUAL( CKR_OK, C_GetFunctionList( &pxFunctionList ) );

    /* Ensure that pxFunctionList was changed by C_GetFunctionList. */
    TEST_ASSERT_NOT_EQUAL( NULL, pxFunctionList );
}

TEST( Full_PKCS11_StartFinish, AFQP_InitializeFinalize )
{
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;

    CK_RV xResult;

    xResult = C_GetFunctionList( &pxFunctionList );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to get function list." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, pxFunctionList, "Invalid function list pointer." );

    /* Ask Bryan --> is the NULL ok here?
     * Used in his version of aws_dev_mode_key_provisioning but maybe both NULL
     * and multithread option supported. */
    xResult = pxFunctionList->C_Initialize( NULL );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to initialize PKCS #11 module." );

    if( TEST_PROTECT() )
    {
        /* Call initialize a second time.  Since this call may be made many times,
         * it is important that PKCS #11 implementations be tolerant of multiple calls. */
        xResult = pxFunctionList->C_Initialize( NULL );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_CRYPTOKI_ALREADY_INITIALIZED, xResult, "Second PKCS #11 module initialization. " );

        /* C_Finalize should fail if pReserved isn't NULL. */
        xResult = pxFunctionList->C_Finalize( ( CK_VOID_PTR ) 0x1234 );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_ARGUMENTS_BAD, xResult, "Negative Test: Finalize with invalid argument" );
    }

    xResult = pxFunctionList->C_Finalize( NULL );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Finalize failed" );

    /* Call Finalize a second time.  Since finalize may be called multiple times,
     * it is important that the PKCS #11 module is tolerant of multiple calls. */
    xResult = pxFunctionList->C_Finalize( NULL );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_CRYPTOKI_NOT_INITIALIZED, xResult, "Second PKCS #11 finalization failed" );
}

TEST( Full_PKCS11_StartFinish, AFQP_GetSlotList )
{
    CK_RV xResult;
    CK_SLOT_ID * pxSlotId = NULL;
    CK_ULONG xSlotCount = 0;
    CK_ULONG xExtraSlotCount = 0;

    xResult = pxGlobalFunctionList->C_Initialize( NULL );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to initialize PKCS #11 module" );

    if( TEST_PROTECT() )
    {
        /* The Happy Path. */

        /* When a NULL slot pointer is passed in,
         *  the number of slots should be updated. */
        xResult = pxGlobalFunctionList->C_GetSlotList( CK_TRUE, NULL, &xSlotCount );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to get slot count" );
        TEST_ASSERT_GREATER_THAN_MESSAGE( 0, xSlotCount, "Slot count incorrectly updated" );

        /* Allocate memory to receive the list of slots. */
        pxSlotId = pvPortMalloc( sizeof( CK_SLOT_ID ) * xSlotCount );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, pxSlotId, "Failed malloc memory for slot list" );

        /* Call C_GetSlotList again to receive all slots with tokens present. */
        xResult = pxGlobalFunctionList->C_GetSlotList( CK_TRUE, pxSlotId, &xSlotCount );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to get slot count" );

        /* Note: Being able to use the slot to open a session will be  tested
         * in the C_OpenSession tests. */

        /* Off the happy path. */

        vPortFree( pxSlotId );
        xExtraSlotCount = xSlotCount + 1;
        pvPortMalloc( sizeof( CK_SLOT_ID ) * xExtraSlotCount );

        /* Make sure that number of slots returned is updated when extra buffer room exists. */
        xResult = pxGlobalFunctionList->C_GetSlotList( CK_TRUE, pxSlotId, &xExtraSlotCount );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to get slot list" );
        TEST_ASSERT_EQUAL_MESSAGE( xSlotCount, xExtraSlotCount, "Failed to update the number of slots" );

        /* Claim that the buffer to receive slots is too small. */
        xSlotCount = 0;
        xResult = pxGlobalFunctionList->C_GetSlotList( CK_TRUE, pxSlotId, &xSlotCount );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_BUFFER_TOO_SMALL, xResult, "Negative Test: Improper handling of too-small slot buffer" );

        /* Passing NULL as pulCount isn't valid. */
        TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, pxGlobalFunctionList->C_GetSlotList( CK_TRUE, NULL, NULL ) );
    }

    if( pxSlotId != NULL )
    {
        vPortFree( pxSlotId );
    }

    xResult = pxGlobalFunctionList->C_Finalize( NULL );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Finalize failed" );
}

extern CK_RV prvGetSlotList( CK_SLOT_ID ** ppxSlotId,
                             CK_ULONG * pxSlotCount );
TEST( Full_PKCS11_StartFinish, AFQP_OpenSessionCloseSession )
{
    CK_SLOT_ID_PTR pxSlotId = NULL;
    CK_SLOT_ID xSlotId = 0;
    CK_ULONG xSlotCount = 0;
    CK_SESSION_HANDLE xSession = 0;
    CK_BBOOL xSessionOpen = CK_FALSE;
    CK_RV xResult = CKR_OK;

    /* Happy path. */

    xResult = pxGlobalFunctionList->C_Initialize( NULL );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to initialize PKCS #11 module" );

    if( TEST_PROTECT() )
    {
        xResult = prvGetSlotList( &pxSlotId,
                                  &xSlotCount );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to get slot list" );
        xSlotId = pxSlotId[ pkcs11testSLOT_NUMBER ];
        vPortFree( pxSlotId ); /* prvGetSlotList allocates memory. */
        TEST_ASSERT_GREATER_THAN( 0, xSlotCount );


        xResult = pxGlobalFunctionList->C_OpenSession( xSlotId,
                                                       CKF_SERIAL_SESSION, /* This flag is mandatory for PKCS #11 legacy reasons. */
                                                       NULL,               /* Application defined pointer. */
                                                       NULL,               /* Callback function. */
                                                       &xSession );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to open session" );
        xSessionOpen = CK_TRUE;
    }

    if( xSessionOpen )
    {
        xResult = pxGlobalFunctionList->C_CloseSession( xSession );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to close session." );
    }

    pxGlobalFunctionList->C_Finalize( NULL );


    /* Negative tests */

    /* Try to open a session without having initialize the module. */
    xResult = pxGlobalFunctionList->C_OpenSession( xSlotId,
                                                   CKF_SERIAL_SESSION, /* This flag is mandatory for PKCS #11 legacy reasons. */
                                                   NULL,               /* Application defined pointer. */
                                                   NULL,               /* Callback function. */
                                                   &xSession );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_CRYPTOKI_NOT_INITIALIZED, xResult, "Negative Test: Opened a session before initializing module." );
}


static void prvInitializeMultiThreadTask( void * pvParameters )
{
    MultithreadTaskParams_t * pxTaskParams;
    BaseType_t xCount;
    CK_RV xResult;

    pxTaskParams = ( MultithreadTaskParams_t * ) pvParameters;

    for( xCount = 0; xCount < pkcs11testMULTI_THREAD_LOOP_COUNT; xCount++ )
    {
        /* Code here. */
        xResult = pxGlobalFunctionList->C_Initialize( NULL );

        if( ( xResult != CKR_OK ) && ( xResult != CKR_CRYPTOKI_ALREADY_INITIALIZED ) )
        {
            configPRINTF( ( "C_Initialize multi-thread task failed.  Error: %d \r\n", xResult ) );
            break;
        }
    }

    /* If module already intialized, this is thread should report a successful outcome. */
    if( xResult == CKR_CRYPTOKI_ALREADY_INITIALIZED )
    {
        xResult = CKR_OK;
    }

    /* Report the result of the loop. */
    pxTaskParams->xTestResult = xResult;

    /* Report that task is finished, then delete task. */
    ( void ) xEventGroupSetBits( xSyncEventGroup,
                                 ( 1 << pxTaskParams->xTaskNumber ) );
    vTaskDelete( NULL );
}

TEST( Full_PKCS11_StartFinish, AFQP_InitializeMultiThread )
{
    CK_RV xResult;

    prvMultiThreadHelper( prvInitializeMultiThreadTask );

    xResult = pxGlobalFunctionList->C_Finalize( NULL );
    /* TODO: Does there need to be one call for each call to initialize? */
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Finalize failed in InitializeMultiThread test" );
}


/*--------------------------------------------------------*/
/*-------------- No Object Tests ------------------------ */
/*--------------------------------------------------------*/

static CK_BYTE x896BitInput[] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

static CK_BYTE xSha256HashOf896BitInput[] =
{
    0xcf, 0x5b, 0x16, 0xa7, 0x78, 0xaf, 0x83, 0x80, 0x03, 0x6c, 0xe5, 0x9e, 0x7b, 0x04, 0x92, 0x37,
    0x0b, 0x24, 0x9b, 0x11, 0xe8, 0xf0, 0x7a, 0x51, 0xaf, 0xac, 0x45, 0x03, 0x7a, 0xfe, 0xe9, 0xd1
};

TEST( Full_PKCS11_NoObject, AFQP_Digest )
{
    CK_RV xResult = 0;

    CK_MECHANISM xDigestMechanism;

    CK_BYTE xDigestResult[ pcks11SHA256_DIGEST_LENGTH ] = { 0 };
    CK_ULONG xDigestLength = 0;


    /* Hash with SHA256 */
    xDigestMechanism.mechanism = CKM_SHA256;

    xResult = pxGlobalFunctionList->C_DigestInit( xGlobalSession, &xDigestMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    /* Subtract one because this hash was performed on the characters without the null terminator. */
    xResult = pxGlobalFunctionList->C_DigestUpdate( xGlobalSession, x896BitInput, sizeof( x896BitInput ) - 1 );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    /* Call C_DigestFinal on a NULL buffer to get the buffer length required. */
    xResult = pxGlobalFunctionList->C_DigestFinal( xGlobalSession, NULL, &xDigestLength );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( pcks11SHA256_DIGEST_LENGTH, xDigestLength );

    xResult = pxGlobalFunctionList->C_DigestFinal( xGlobalSession, xDigestResult, &xDigestLength );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL_INT8_ARRAY( xSha256HashOf896BitInput, xDigestResult, pcks11SHA256_DIGEST_LENGTH );
}

TEST( Full_PKCS11_NoObject, AFQP_Digest_ErrorConditions )
{
    CK_RV xResult = 0;
    CK_MECHANISM xDigestMechanism;
    CK_BYTE xDigestResult[ pcks11SHA256_DIGEST_LENGTH ] = { 0 };
    CK_ULONG xDigestLength = 0;

    /* Make sure that no NULL pointers in functions to be called in this test. */
    TEST_ASSERT_NOT_EQUAL( NULL, pxGlobalFunctionList->C_DigestInit );
    TEST_ASSERT_NOT_EQUAL( NULL, pxGlobalFunctionList->C_DigestUpdate );
    TEST_ASSERT_NOT_EQUAL( NULL, pxGlobalFunctionList->C_DigestFinal );

    /* Invalid hash mechanism. */
    xDigestMechanism.mechanism = 0x253; /*253 doesn't correspond to anything. */ /*CKM_MD5; */

    xResult = pxGlobalFunctionList->C_DigestInit( xGlobalSession, &xDigestMechanism );
    TEST_ASSERT_EQUAL( CKR_MECHANISM_INVALID, xResult );

    /* Null Session. */
    xDigestMechanism.mechanism = CKM_SHA256;
    xResult = pxGlobalFunctionList->C_DigestInit( ( CK_SESSION_HANDLE ) NULL, &xDigestMechanism );
    TEST_ASSERT_EQUAL( CKR_SESSION_HANDLE_INVALID, xResult );

    /* Make sure that digest update fails if DigestInit did not succeed. */
    xResult = pxGlobalFunctionList->C_DigestUpdate( xGlobalSession, x896BitInput, sizeof( x896BitInput ) - 1 );
    TEST_ASSERT_EQUAL( CKR_OPERATION_NOT_INITIALIZED, xResult );

    /* Initialize the session properly. */
    xResult = pxGlobalFunctionList->C_DigestInit( xGlobalSession, &xDigestMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    /* Try to update digest with a NULL session handle. */
    xResult = pxGlobalFunctionList->C_DigestUpdate( ( CK_SESSION_HANDLE ) NULL, x896BitInput, sizeof( x896BitInput ) - 1 );
    TEST_ASSERT_EQUAL( CKR_SESSION_HANDLE_INVALID, xResult );

    /* DigestUpdate correctly.  Note that digest is not terminated because we didn't tell the session handle last time. */
    xResult = pxGlobalFunctionList->C_DigestUpdate( xGlobalSession, x896BitInput, sizeof( x896BitInput ) - 1 );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    /* Call C_DigestFinal on a buffer that is too small. */
    xDigestLength = pcks11SHA256_DIGEST_LENGTH - 1;
    xResult = pxGlobalFunctionList->C_DigestFinal( xGlobalSession, xDigestResult, &xDigestLength );
    TEST_ASSERT_EQUAL( CKR_BUFFER_TOO_SMALL, xResult );

    /* Call C_DigestFinal on a NULL session handle. */
    xDigestLength = pcks11SHA256_DIGEST_LENGTH;
    xResult = pxGlobalFunctionList->C_DigestFinal( ( CK_SESSION_HANDLE ) NULL, xDigestResult, &xDigestLength );
    TEST_ASSERT_EQUAL( CKR_SESSION_HANDLE_INVALID, xResult );

    /* Call C_DigestFinal on a proper buffer size. Note that Digest is not terminated if error is "buffer too small" or if session handle wasn't present. */
    xDigestLength = pcks11SHA256_DIGEST_LENGTH;
    xResult = pxGlobalFunctionList->C_DigestFinal( xGlobalSession, xDigestResult, &xDigestLength );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL_INT8_ARRAY( xSha256HashOf896BitInput, xDigestResult, pcks11SHA256_DIGEST_LENGTH );

    /* Call C_DigestUpdate after the digest operation has been completed. */
    xResult = pxGlobalFunctionList->C_DigestUpdate( xGlobalSession, x896BitInput, sizeof( x896BitInput ) - 1 );
    TEST_ASSERT_EQUAL( CKR_OPERATION_NOT_INITIALIZED, xResult );
}


TEST( Full_PKCS11_NoObject, AFQP_GenerateRandom )
{
    CK_RV xResult = 0;
    CK_SLOT_ID xSlotId = 0;
    BaseType_t xSameSession = 0;
    BaseType_t xDifferentSessions = 0;

#define pkcstestRAND_BUFFER_SIZE    10 /* This number is not actually flexible anymore because of the print formatting. */
    CK_BYTE xBuf1[ pkcstestRAND_BUFFER_SIZE ];
    CK_BYTE xBuf2[ pkcstestRAND_BUFFER_SIZE ];
    CK_BYTE xBuf3[ pkcstestRAND_BUFFER_SIZE ];

    /* Generate random bytes twice. */
    if( CKR_OK == xResult )
    {
        xResult = pxGlobalFunctionList->C_GenerateRandom( xGlobalSession, xBuf1, pkcstestRAND_BUFFER_SIZE );
    }

    if( CKR_OK == xResult )
    {
        xResult = pxGlobalFunctionList->C_GenerateRandom( xGlobalSession, xBuf2, pkcstestRAND_BUFFER_SIZE );
    }

    if( CKR_OK == xResult )
    {
        /* Close the session and PKCS #11 module */
        if( NULL != pxGlobalFunctionList )
        {
            ( void ) pxGlobalFunctionList->C_CloseSession( xGlobalSession );
            ( void ) pxGlobalFunctionList->C_Finalize( NULL );
        }
    }

    /* Re-initialize PKCS #11. */
    xResult = xInitializePkcs11Session( &xGlobalSession );

    if( CKR_OK == xResult )
    {
        xResult = pxGlobalFunctionList->C_GenerateRandom( xGlobalSession, xBuf3, pkcstestRAND_BUFFER_SIZE );
    }

    /* Check that the result is good. */
    TEST_ASSERT_EQUAL_INT32( CKR_OK, xResult );

    /* Check that the random bytes generated within session
     * and between initializations of PKCS module are not the same. */
    for( int i = 0; i < pkcstestRAND_BUFFER_SIZE; i++ )
    {
        if( xBuf1[ i ] == xBuf2[ i ] )
        {
            xSameSession++;
        }

        if( xBuf1[ i ] == xBuf3[ i ] )
        {
            xDifferentSessions++;
        }
    }

    if( ( xSameSession > 1 ) || ( xDifferentSessions > 1 ) )
    {
        configPRINTF( ( "First Random Bytes: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                        xBuf1[ 0 ], xBuf1[ 1 ], xBuf1[ 2 ], xBuf1[ 3 ], xBuf1[ 4 ],
                        xBuf1[ 5 ], xBuf1[ 6 ], xBuf1[ 7 ], xBuf1[ 8 ], xBuf1[ 9 ] ) );

        configPRINTF( ( "Second Set of Random Bytes: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                        xBuf2[ 0 ], xBuf2[ 1 ], xBuf2[ 2 ], xBuf2[ 3 ], xBuf2[ 4 ],
                        xBuf2[ 5 ], xBuf2[ 6 ], xBuf2[ 7 ], xBuf2[ 8 ], xBuf2[ 9 ] ) );

        configPRINTF( ( "Third Set of Random Bytes:  %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                        xBuf3[ 0 ], xBuf3[ 1 ], xBuf3[ 2 ], xBuf3[ 3 ], xBuf3[ 4 ],
                        xBuf3[ 5 ], xBuf3[ 6 ], xBuf3[ 7 ], xBuf3[ 8 ], xBuf3[ 9 ] ) );
    }

    TEST_ASSERT_LESS_THAN( 2, xSameSession );
    TEST_ASSERT_LESS_THAN( 2, xDifferentSessions );
}

#define pkcs11testRANDOM_DATA_LENGTH    10
static void prvGenerateRandomMultiThreadTask( void * pvParameters )
{
    MultithreadTaskParams_t * pxMultiTaskParam = pvParameters;
    BaseType_t xCount;
    CK_RV xResult;
    CK_BYTE xRandomData[ pkcs11testRANDOM_DATA_LENGTH ];
    CK_SESSION_HANDLE xSession;

    memcpy( &xSession, pxMultiTaskParam->pvTaskData, sizeof( CK_SESSION_HANDLE ) );

    for( xCount = 0; xCount < pkcs11testMULTI_THREAD_LOOP_COUNT; xCount++ )
    {
        xResult = pxGlobalFunctionList->C_GenerateRandom( xSession,
                                                          xRandomData,
                                                          sizeof( xRandomData ) );

        if( xResult != CKR_OK )
        {
            configPRINTF( ( "GenerateRandom multi-thread task failed.  Error: %d \r\n", xResult ) );
            break;
        }
    }

    /* Report the result of the loop. */
    pxMultiTaskParam->xTestResult = xResult;

    /* Report that task is finished, then delete task. */
    ( void ) xEventGroupSetBits( xSyncEventGroup,
                                 ( 1 << pxMultiTaskParam->xTaskNumber ) );
    vTaskDelete( NULL );
}


TEST( Full_PKCS11_NoObject, AFQP_GenerateRandomMultiThread )
{
    CK_RV xResult;
    BaseType_t xTaskNumber;
    CK_SESSION_HANDLE xSessionHandle[ pkcs11testMULTI_THREAD_TASK_COUNT ];

    for( xTaskNumber = 0; xTaskNumber < pkcs11testMULTI_THREAD_TASK_COUNT; xTaskNumber++ )
    {
        xResult = xInitializePkcs11Session( &xSessionHandle[ xTaskNumber ] );
        xGlobalTaskParams[ xTaskNumber ].pvTaskData = &xSessionHandle[ xTaskNumber ];
    }

    prvMultiThreadHelper( prvGenerateRandomMultiThreadTask );

    for( xTaskNumber = 0; xTaskNumber < pkcs11testMULTI_THREAD_TASK_COUNT; xTaskNumber++ )
    {
        xResult = pxGlobalFunctionList->C_CloseSession( xSessionHandle[ xTaskNumber ] );
    }
}


/* Valid RSA private key. */
static const char cValidRSAPrivateKey[] =
    "-----BEGIN RSA PRIVATE KEY-----\n"
    "MIIEpAIBAAKCAQEAsIqRecRxLz3PZXzZOHF7jMlB25tfv2LDGR7nGTJiey5zxd7o\n"
    "swihe7+26yx8medpNvX1ym9jphty+9IR053k1WGnQQ4aaDeJonqn7V50Vesw6zFx\n"
    "/x8LMdXFoBAkRXIL8WS5YKafC87KPnye8A0piVWUFy7+IEEaK3hQEJTzB6LC/N10\n"
    "0XL5ykLCa4xJBOqlIvbDvJ+bKty1EBA3sStlTNuXi3nBWZbXwCB2A+ddjijFf5+g\n"
    "Ujinr7h6e2uQeipWyiIw9NKWbvq8AG1Mj4XBoFL9wP2YTf2SQAgAzx0ySPNrIYOz\n"
    "BNl1YZ4lIW5sJLATES9+Z8nHi7yRDLw6x/kcVQIDAQABAoIBADd+h3ZIeu/HtT8I\n"
    "vNuSSK0bwpj+wV1O9VcbMLfp760bEAd+J5XHu8NDo4NPi6dxZ9CABpBo7WEUtdNU\n"
    "2Ie11W4B8WpwvXpPIvOxLMJf85/ie5EjDNuObZ1vvlyvVkeCLyDlcaRhHBPBIC/+\n"
    "SpPY/1qNTSzwd6+55zkM69YajD60tFv8WuCsgkAteCoDjcqwDcowyAy4pILhOYaW\n"
    "1x+0ZPMUqwtld+06ct/yqBPB8C9IH7ZIeJr5e58R9SxytbuTwTN4jceOoeD5MBbG\n"
    "A+A0WxGdQ8N+kwWkz77qDbZfP4G8wNxeUXobnfqfDGqb0O5zeEaU7EI+mlEQH58Z\n"
    "B1edj6ECgYEA3rldciCQc4t2qYKiZSffnl7Pg7L+ojzH/Eam4Dlk27+DqOo70MnX\n"
    "LVWUWkLOMQ27dRSBQsUDUaqVRZLkcFKc6C8k2cIpPBMpA9WdZVd9kFawZ8lJ7jko\n"
    "qTbJxnDxvhdHrZRvLRjEenbdNXdAGy2EuqvThUJgPEybLAWg6sE3LB0CgYEAyurT\n"
    "14h4BGEGBpl2KevoPQ4PPS+IoDXMS29mtfcascVkYcxxW1v8wOQVyD4VrsRMErck\n"
    "ZMpu2evd+aQSPSrAod/sQ20C+wCCA7ipBlhAUeuS/FpqFIZWkHzZnVccp8O3nOFO\n"
    "KNeAmw4udq8PyjVVouey/6F386itJdxWt/d8i5kCgYA3Aru045wqHck6RvzLVVTj\n"
    "LfG9Sqmf8rlGc0DmYuapbB0dzHTnteLC3L9ep997uDOT0HO4xSZztllWLNjlcVI1\n"
    "+ub0LgO3Rdg8jTdp/3kQ/IhnqgzrnQyQ9upRbDYZSHC4y8/F6LcmtFMg0Ipx7AU7\n"
    "ghMld+aDHjy5W86KDR0OdQKBgQCAZoPSONqo+rQTbPwmns6AA+uErhVoO2KgwUdf\n"
    "EZPktaFFeVapltWjQTC/WvnhcvkoRpdS5/2pC+WUWEvqRKlMRSN9rvdZ2QJsVGcw\n"
    "Spu4urZx1MyXXEJef4I8W6kYR3JiZPdORL9uXlTsaO425/Tednr/4y7CEhQuhvSg\n"
    "yIwY0QKBgQC2NtKDOwcgFykKRYqtHuo6VpSeLmgm1DjlcAuaGJsblX7C07ZH8Tjm\n"
    "IHQb01oThNEa4tC0vO3518PkQwvyi/TWGHm9SLYdXvpVnBwkk5yRioKPgPmrs4Xi\n"
    "ERIYrvveGGtQ3vSknLWUJ/0BgmuYj5U6aJBZPv8COM2eKIbTQbtQaQ==\n"
    "-----END RSA PRIVATE KEY-----\n";

/* Valid RSA certificate. */
static const char cValidRSACertificate[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDsTCCApmgAwIBAgIJALg4YJlPspxyMA0GCSqGSIb3DQEBCwUAMG8xCzAJBgNV\n"
    "BAYTAlVTMQswCQYDVQQIDAJXQTEQMA4GA1UEBwwHU2VhdHRsZTENMAsGA1UECgwE\n"
    "QW16bjEMMAoGA1UECwwDSW9UMQ0wCwYDVQQDDARUZXN0MRUwEwYJKoZIhvcNAQkB\n"
    "FgZub2JvZHkwHhcNMTgwNjExMTk0NjM2WhcNMjEwMzMxMTk0NjM2WjBvMQswCQYD\n"
    "VQQGEwJVUzELMAkGA1UECAwCV0ExEDAOBgNVBAcMB1NlYXR0bGUxDTALBgNVBAoM\n"
    "BEFtem4xDDAKBgNVBAsMA0lvVDENMAsGA1UEAwwEVGVzdDEVMBMGCSqGSIb3DQEJ\n"
    "ARYGbm9ib2R5MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsIqRecRx\n"
    "Lz3PZXzZOHF7jMlB25tfv2LDGR7nGTJiey5zxd7oswihe7+26yx8medpNvX1ym9j\n"
    "phty+9IR053k1WGnQQ4aaDeJonqn7V50Vesw6zFx/x8LMdXFoBAkRXIL8WS5YKaf\n"
    "C87KPnye8A0piVWUFy7+IEEaK3hQEJTzB6LC/N100XL5ykLCa4xJBOqlIvbDvJ+b\n"
    "Kty1EBA3sStlTNuXi3nBWZbXwCB2A+ddjijFf5+gUjinr7h6e2uQeipWyiIw9NKW\n"
    "bvq8AG1Mj4XBoFL9wP2YTf2SQAgAzx0ySPNrIYOzBNl1YZ4lIW5sJLATES9+Z8nH\n"
    "i7yRDLw6x/kcVQIDAQABo1AwTjAdBgNVHQ4EFgQUHc4PjEL0CaxZ+1D/4VdeDjxt\n"
    "JO8wHwYDVR0jBBgwFoAUHc4PjEL0CaxZ+1D/4VdeDjxtJO8wDAYDVR0TBAUwAwEB\n"
    "/zANBgkqhkiG9w0BAQsFAAOCAQEAi1/okTpQuPcaQEBgepccZ/Lt/gEQNdGcbsYQ\n"
    "3aEABNVYL8dYOW9r/8l074zD+vi9iSli/yYmwRFD0baN1FRWUqkVEIQ+3yfivOW9\n"
    "R282NuQvEULgERC2KN7vm0vO+DF7ay58qm4PaAGHdQco1LaHKkljMPLHF841facG\n"
    "M9KVtzFveOQKkWvb4VgOyfn7aCnEogGlWt1S0d12pBRwYjJgKrVQaGs6IiGFVtm8\n"
    "JRLZrLL3sfgsN7L1xu//JUoTOkgxdKuYRmPuUdV2hw/VYDzcnKj7/DMXNDvgl3s7\n"
    "5GC4F+8LFLzRrZJWs18FMLaCE+zJChw/oeSt+RS0JZDFn+uX9Q==\n"
    "-----END CERTIFICATE-----\n";

void prvProvisionRsaTestCredentials( CK_OBJECT_HANDLE_PTR pxPrivateKeyHandle,
                                     CK_OBJECT_HANDLE_PTR pxCertificateHandle )
{
    CK_RV xResult;

    if( xCurrentCredentials != eRsaTest )
    {
        xResult = xDestroyCredentials( xGlobalSession );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to destroy credentials in test setup." );
        xCurrentCredentials = eNone;

        /* Create a private key. */
        xResult = xProvisionPrivateKey( xGlobalSession,
                                        ( uint8_t * ) cValidRSAPrivateKey,
                                        sizeof( cValidRSAPrivateKey ),
                                        CKK_RSA,
                                        pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                        pxPrivateKeyHandle );

        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to create RSA private key." );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, *pxPrivateKeyHandle, "Invalid object handle returned for RSA private key." );

        /* Create a certificate. */
        xResult = xProvisionCertificate( xGlobalSession,
                                         ( uint8_t * ) cValidRSACertificate,
                                         sizeof( cValidRSACertificate ),
                                         pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                         pxCertificateHandle );

        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to create RSA certificate." );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, *pxCertificateHandle, "Invalid object handle returned for RSA certificate." );
        xCurrentCredentials = eRsaTest;
    }
    else
    {
        xResult = xFindObjectWithLabel( xGlobalSession, pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, pxPrivateKeyHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find RSA private key." );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, pxPrivateKeyHandle, "Invalid object handle found for RSA private key." );

        xResult = xFindObjectWithLabel( xGlobalSession, pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, pxCertificateHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find RSA certificate." );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, pxCertificateHandle, "Invalid object handle found for RSA certificate." );
    }
}

/* Note: This tests that objects can be created and found successfully.
 * It does not check the correctness or usability of objects stored. */
TEST( Full_PKCS11_RSA, AFQP_CreateObjectFindObject )
{
    CK_RV xResult;
    CK_OBJECT_HANDLE xPrivateKeyHandle;
    CK_OBJECT_HANDLE xCertificateHandle;
    CK_OBJECT_HANDLE xFoundPrivateKeyHandle;
    CK_OBJECT_HANDLE xFoundCertificateHandle;

    if( xCurrentCredentials != eNone )
    {
        xResult = xDestroyCredentials( xGlobalSession );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to destroy credentials in test setup." );
        xCurrentCredentials = eNone;
    }

    prvProvisionRsaTestCredentials( &xPrivateKeyHandle, &xCertificateHandle );

    /* Find the newly created private key. */
    xResult = xFindObjectWithLabel( xGlobalSession,
                                    pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                    &xFoundPrivateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find RSA private key." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, xFoundPrivateKeyHandle, "Invalid object handle found for RSA private key." );
    TEST_ASSERT_EQUAL_MESSAGE( xPrivateKeyHandle, xFoundPrivateKeyHandle, "Private key handle found does not match private key handle created." );

    /* Find the newly created certificate. */
    xResult = xFindObjectWithLabel( xGlobalSession,
                                    pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                    &xFoundCertificateHandle );

    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find RSA certificate." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, xCertificateHandle, "Invalid object handle found for RSA certificate." );
    TEST_ASSERT_EQUAL_MESSAGE( xCertificateHandle, xFoundCertificateHandle, "Certificate handle found does not match certificate handle created." );

    /* Close and reopen a new session.  Make sure that the private key and certificate can still be found. */
    xResult = pxGlobalFunctionList->C_CloseSession( xGlobalSession );
    xResult = xInitializePkcs11Session( &xGlobalSession );

    xResult = xFindObjectWithLabel( xGlobalSession,
                                    pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                    &xFoundPrivateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find RSA private key after closing and reopening a session." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, xFoundPrivateKeyHandle, "Invalid object handle found for RSA private key." );

    xResult = xFindObjectWithLabel( xGlobalSession,
                                    pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                    &xFoundCertificateHandle );

    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find RSA certificate after closing and reopening a session." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, xCertificateHandle, "Invalid object handle found for RSA certificate." );
}



TEST( Full_PKCS11_RSA, AFQP_FindObject )
{
    CK_OBJECT_HANDLE xPrivateKey;
    CK_OBJECT_HANDLE xCertificate;

    prvProvisionRsaTestCredentials( &xPrivateKey, &xCertificate );
    prvFindObjectTest();
}

TEST( Full_PKCS11_RSA, AFQP_CreateObjectGetAttributeValue )
{
#define MODULUS_LENGTH              256
#define PUB_EXP_LENGTH              3
#define CERTIFICATE_VALUE_LENGTH    949
    CK_RV xResult;
    CK_OBJECT_HANDLE xPrivateKeyHandle;
    CK_OBJECT_HANDLE xCertificateHandle;
    CK_ATTRIBUTE xTemplate;
    CK_BYTE xModulus[ MODULUS_LENGTH ];
    CK_BYTE xPubExponent[ PUB_EXP_LENGTH ];
    CK_BYTE xCertificateValue[ CERTIFICATE_VALUE_LENGTH ];

    prvProvisionRsaTestCredentials( &xPrivateKeyHandle, &xCertificateHandle );

    /* Get public key components from private key. */

    /* Get the modulus length. */
    xTemplate.type = CKA_MODULUS;
    xTemplate.pValue = NULL;
    xTemplate.ulValueLen = 0;
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKeyHandle, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to query attribute length" );
    TEST_ASSERT_EQUAL_MESSAGE( MODULUS_LENGTH, xTemplate.ulValueLen, "GetAttributeValue returned incorrect length of RSA modulus" );

    /* Get the modulus value. */
    xTemplate.pValue = xModulus;
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKeyHandle, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to get RSA modulus" );
    TEST_ASSERT_EQUAL_MESSAGE( MODULUS_LENGTH, xTemplate.ulValueLen, "GetAttributeValue returned incorrect length of RSA modulus" );
    /* TODO: Check value of modulus. */

    /* Get the public exponent length. */
    xTemplate.type = CKA_PUBLIC_EXPONENT;
    xTemplate.pValue = NULL;
    xTemplate.ulValueLen = 0;
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKeyHandle, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to query attribute length" );
    TEST_ASSERT_EQUAL_MESSAGE( PUB_EXP_LENGTH, xTemplate.ulValueLen, "GetAttributeValue returned incorrect length of RSA public exponent" );

    /* Get the public exponent value. */
    xTemplate.pValue = xPubExponent;
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKeyHandle, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to get RSA public exponent" );
    TEST_ASSERT_EQUAL_MESSAGE( PUB_EXP_LENGTH, xTemplate.ulValueLen, "GetAttributeValue returned incorrect length for RSA public exponent" );
    /* TODO: Check value of public exponent. */

    /* Get the certificate value. */
    xTemplate.type = CKA_VALUE;
    xTemplate.pValue = NULL;
    xTemplate.ulValueLen = 0;
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xCertificateHandle, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CERTIFICATE_VALUE_LENGTH, xTemplate.ulValueLen, "GetAttributeValue returned incorrect length of RSA certificate value" );

    xTemplate.pValue = xCertificateValue;
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xCertificateHandle, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to get RSA certificate value" );
    TEST_ASSERT_EQUAL_MESSAGE( CERTIFICATE_VALUE_LENGTH, xTemplate.ulValueLen, "GetAttributeValue returned incorrect length of RSA certificate value" );
    /* TODO: Check byte array */
}


TEST( Full_PKCS11_RSA, AFQP_Sign )
{
    CK_RV xResult;
    CK_OBJECT_HANDLE xPrivateKeyHandle;
    CK_OBJECT_HANDLE xCertificateHandle;
    CK_MECHANISM xMechanism;
    CK_BYTE xHashedMessage[ SHA256_DIGEST_SIZE ] = { 0 };
    CK_BYTE xSignature[ RSA_SIGNATURE_SIZE ] = { 0 };
    CK_ULONG xSignatureLength;
    CK_BYTE xPaddedHash[ RSA_SIGNATURE_SIZE ] = { 0 };
    CK_BYTE xModulus[ RSA_SIGNATURE_SIZE ] = { 0 };
    unsigned int ulModulusLength = 0;
    CK_BYTE xExponent[ 4 ] = { 0 };
    unsigned int ulExponentLength = 0;

    prvProvisionRsaTestCredentials( &xPrivateKeyHandle, &xCertificateHandle );

    CK_ATTRIBUTE xTemplate;
    CK_BBOOL xTrue = CK_TRUE;
    xTemplate.type = CKA_SIGN;
    xTemplate.pValue = &xTrue;
    xTemplate.ulValueLen = sizeof( CK_BBOOL );
    xResult = pxGlobalFunctionList->C_SetAttributeValue( xGlobalSession, xPrivateKeyHandle, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to create RSA private key." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, xPrivateKeyHandle, "Invalid object handle returned for RSA private key." );

    xResult = PKI_RSA_RSASSA_PKCS1_v15_Encode( xHashedMessage, RSA_SIGNATURE_SIZE, xPaddedHash );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    /* The RSA X.509 mechanism assumes a pre-hashed input. */
    xMechanism.mechanism = CKM_RSA_X_509;
    xMechanism.pParameter = NULL;
    xMechanism.ulParameterLen = 0;
    xResult = pxGlobalFunctionList->C_SignInit( xGlobalSession, &xMechanism, xPrivateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to SignInit RSA." );

    xSignatureLength = sizeof( xSignature );
    xResult = pxGlobalFunctionList->C_Sign( xGlobalSession, xPaddedHash, RSA_SIGNATURE_SIZE, xSignature, &xSignatureLength );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to RSA Sign." );

    /* Verify the signature with mbedTLS */
    mbedtls_pk_context xMbedPkContext;
    int lMbedTLSResult;

    mbedtls_pk_init( &xMbedPkContext );

    if( TEST_PROTECT() )
    {
        lMbedTLSResult = mbedtls_pk_parse_key( ( mbedtls_pk_context * ) &xMbedPkContext,
                                               cValidRSAPrivateKey,
                                               sizeof( cValidRSAPrivateKey ),
                                               NULL,
                                               0 );

        lMbedTLSResult = mbedtls_rsa_pkcs1_verify( xMbedPkContext.pk_ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, 32, xHashedMessage, xSignature );
        TEST_ASSERT_EQUAL_MESSAGE( 0, xResult, "mbedTLS failed to parse valid RSA key (verification)" );
    }

    mbedtls_pk_free( &xMbedPkContext );
}

TEST( Full_PKCS11_RSA, AFQP_GenerateKeyPair )
{
    CK_RV xResult;
    CK_OBJECT_HANDLE xPrivateKeyHandle;
    CK_OBJECT_HANDLE xPublicKeyHandle;
    CK_MECHANISM xMechanism;
    CK_BYTE xHashedMessage[ SHA256_DIGEST_SIZE ] = { 0 };
    CK_BYTE xSignature[ RSA_SIGNATURE_SIZE ] = { 0 };
    CK_ULONG xSignatureLength;
    CK_BYTE xModulus[ RSA_SIGNATURE_SIZE ] = { 0 };
    unsigned int ulModulusLength = 0;
    CK_BYTE xExponent[ 4 ] = { 0 };
    unsigned int ulExponentLength = 0;
    CK_BYTE xPaddedHash[ RSA_SIGNATURE_SIZE ] = { 0 };
    mbedtls_rsa_context xRsaContext;



    xResult = xProvisionGenerateKeyPairRSA( xGlobalSession,
                                            "Amazon FreeRTOS test RSA private",
                                            "Amazon FreeRTOS test RSA public",
                                            &xPrivateKeyHandle,
                                            &xPublicKeyHandle );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, xPrivateKeyHandle, "Invalid object handle returned for RSA private key." );

    CK_ATTRIBUTE xTemplate;
    xTemplate.type = CKA_MODULUS;
    xTemplate.pValue = xModulus;
    xTemplate.ulValueLen = sizeof( xModulus );
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKeyHandle, &xTemplate, 1 );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    ulModulusLength = xTemplate.ulValueLen;

    xTemplate.type = CKA_PUBLIC_EXPONENT;
    xTemplate.pValue = xExponent;
    xTemplate.ulValueLen = sizeof( xExponent );
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKeyHandle, &xTemplate, 1 );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    ulExponentLength = xTemplate.ulValueLen;

    xResult = PKI_RSA_RSASSA_PKCS1_v15_Encode( xHashedMessage, RSA_SIGNATURE_SIZE, xPaddedHash );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    /* The RSA X.509 mechanism assumes a pre-hashed input. */
    xMechanism.mechanism = CKM_RSA_X_509;
    xMechanism.pParameter = NULL;
    xMechanism.ulParameterLen = 0;
    xResult = pxGlobalFunctionList->C_SignInit( xGlobalSession, &xMechanism, xPrivateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to SignInit RSA." );

    xSignatureLength = sizeof( xSignature );
    xResult = pxGlobalFunctionList->C_Sign( xGlobalSession, xPaddedHash, RSA_SIGNATURE_SIZE, xSignature, &xSignatureLength );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to RSA Sign." );

    /* Verify the signature with mbedTLS */

    /* Set up the RSA public key. */
    mbedtls_rsa_init( &xRsaContext, MBEDTLS_RSA_PKCS_V15, 0 );

    if( TEST_PROTECT() )
    {
        xResult = mbedtls_mpi_read_binary( &xRsaContext.N, xModulus, ulModulusLength );
        TEST_ASSERT_EQUAL( 0, xResult );
        xResult = mbedtls_mpi_read_binary( &xRsaContext.E, xExponent, ulExponentLength );
        TEST_ASSERT_EQUAL( 0, xResult );
        xRsaContext.len = RSA_SIGNATURE_SIZE;
        xResult = mbedtls_rsa_check_pubkey( &xRsaContext );
        TEST_ASSERT_EQUAL( 0, xResult );
        xResult = mbedtls_rsa_pkcs1_verify( &xRsaContext, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, 32, xHashedMessage, xSignature );
        TEST_ASSERT_EQUAL_MESSAGE( 0, xResult, "mbedTLS failed to parse valid RSA key (verification)" );
        /* Verify the signature with the generated public key. */
        xResult = pxGlobalFunctionList->C_VerifyInit( xGlobalSession, &xMechanism, xPublicKeyHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to VerifyInit RSA." );
        xResult = pxGlobalFunctionList->C_Verify( xGlobalSession, xPaddedHash, RSA_SIGNATURE_SIZE, xSignature, xSignatureLength );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to Verify RSA." );
    }

    mbedtls_rsa_free( &xRsaContext );
}


/* Valid ECDSA private key. */
static const char cValidECDSAPrivateKey[] =
    "-----BEGIN PRIVATE KEY-----\n"
    "MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQg8gHhd5ELAooWRQls\n"
    "PfpcQREiLrvEb3oLioicMYdUmrmhRANCAATFgks3zNgbMWJ7IXg43GTYEJjwjh9B\n"
    "Ol84nJZJgKpq3zfG00Qqg7EHfcU/bYwlefuuPbhggu7W5EusWQfalxGQ\n"
    "-----END PRIVATE KEY-----";

/* Valid ECDSA certificate. */
static const char cValidECDSACertificate[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIBVDCB+6ADAgECAgkAoJ9fIf9ayYswCgYIKoZIzj0EAwIwHTEbMBkGA1UEAwwS\n"
    "bm9ib2R5QG5vd2hlcmUuY29tMB4XDTE4MDMwODIyNDIzNFoXDTE5MDMwODIyNDIz\n"
    "NFowHTEbMBkGA1UEAwwSbm9ib2R5QG5vd2hlcmUuY29tMFkwEwYHKoZIzj0CAQYI\n"
    "KoZIzj0DAQcDQgAExYJLN8zYGzFieyF4ONxk2BCY8I4fQTpfOJyWSYCqat83xtNE\n"
    "KoOxB33FP22MJXn7rj24YILu1uRLrFkH2pcRkKMkMCIwCwYDVR0PBAQDAgeAMBMG\n"
    "A1UdJQQMMAoGCCsGAQUFBwMDMAoGCCqGSM49BAMCA0gAMEUCIQDhXUT02TsIlzBe\n"
    "Aw9pLCowZ+6dPY1igspplUqZcuDAKQIgN6j5s7x5AudklULRuFyBQBlkVR35IdWs\n"
    "zu/xp2COg9g=\n"
    "-----END CERTIFICATE-----";

void prvProvisionEcTestCredentials( CK_OBJECT_HANDLE_PTR pxPrivateKeyHandle,
                                    CK_OBJECT_HANDLE_PTR pxCertificateHandle,
                                    CK_OBJECT_HANDLE_PTR pxPublicKeyHandle )
{
    CK_RV xResult;


    if( xCurrentCredentials != eEllipticCurveTest )
    {
        xResult = xDestroyCredentials( xGlobalSession );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to destroy credentials in test setup." );
        xCurrentCredentials = eNone;

        xResult = xProvisionPrivateKey( xGlobalSession,
                                        ( uint8_t * ) cValidECDSAPrivateKey,
                                        sizeof( cValidECDSAPrivateKey ),
                                        CKK_EC,
                                        pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                        pxPrivateKeyHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to create EC private key." );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, *pxPrivateKeyHandle, "Invalid object handle returned for EC private key." );

        xResult = xProvisionPublicKey( xGlobalSession,
                                       ( uint8_t * ) cValidECDSAPrivateKey,
                                       sizeof( cValidECDSAPrivateKey ),
                                       CKK_EC,
                                       pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                                       pxPublicKeyHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to create EC public key." );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, *pxPrivateKeyHandle, "Invalid object handle returned for EC public key." );

        xResult = xProvisionCertificate( xGlobalSession,
                                         ( uint8_t * ) cValidECDSACertificate,
                                         sizeof( cValidECDSACertificate ),
                                         pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                         pxCertificateHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to create EC certificate." );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, *pxPrivateKeyHandle, "Invalid object handle returned for EC certificate." );

        xCurrentCredentials = eEllipticCurveTest;
    }
    else
    {
        xResult = xFindObjectWithLabel( xGlobalSession, pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, pxPrivateKeyHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find EC private key." );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, *pxPrivateKeyHandle, "Invalid object handle found for EC private key." );

        xResult = xFindObjectWithLabel( xGlobalSession, pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, pxCertificateHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find EC certificate." );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, *pxCertificateHandle, "Invalid object handle found for EC certificate." );

        xResult = xFindObjectWithLabel( xGlobalSession, pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, pxPublicKeyHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to find EC public key." );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, *pxPublicKeyHandle, "Invalid object handle found for EC public key." );
    }
}

TEST( Full_PKCS11_EC, AFQP_Sign )
{
    CK_RV xResult;
    CK_OBJECT_HANDLE xPrivateKeyHandle;
    CK_OBJECT_HANDLE xPublicKeyHandle;
    CK_OBJECT_HANDLE xCertificateHandle;
    /* Note that mbedTLS does not permit a signature on all 0's. */
    CK_BYTE xHashedMessage[ SHA256_DIGEST_SIZE ] = { 0xab };
    CK_MECHANISM xMechanism;
    CK_BYTE xSignature[ RSA_SIGNATURE_SIZE ] = { 0 };
    CK_BYTE xEcPoint[ 256 ] = { 0 };
    CK_BYTE xEcParams[ 11 ] = { 0 };
    CK_BYTE xWhatDoesThisBufferDo[ 256 ] = { 0 };
    CK_ULONG xSignatureLength;

    prvProvisionEcTestCredentials( &xPrivateKeyHandle, &xCertificateHandle, &xPublicKeyHandle );

    xMechanism.mechanism = CKM_ECDSA;
    xMechanism.pParameter = NULL;
    xMechanism.ulParameterLen = 0;
    xResult = pxGlobalFunctionList->C_SignInit( xGlobalSession, &xMechanism, xPrivateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to SignInit ECDSA." );

    xSignatureLength = sizeof( xSignature );
    xResult = pxGlobalFunctionList->C_Sign( xGlobalSession, xHashedMessage, SHA256_DIGEST_SIZE, xSignature, &xSignatureLength );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to ECDSA Sign." );

    /* Verify the signature with mbedTLS */
    int lMbedTLSResult;

    mbedtls_pk_context xEcdsaContext;
    mbedtls_pk_init( &xEcdsaContext );

    if( TEST_PROTECT() )
    {
        lMbedTLSResult = mbedtls_pk_parse_key( &xEcdsaContext, cValidECDSAPrivateKey, sizeof( cValidECDSAPrivateKey ), NULL, 0 );
        TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedTLSResult, "mbedTLS failed to parse the imported ECDSA private key." );

        mbedtls_ecp_keypair * pxEcdsaContext = ( mbedtls_ecp_keypair * ) xEcdsaContext.pk_ctx;
        /* An ECDSA signature is comprised of 2 components - R & S. */
        mbedtls_mpi xR;
        mbedtls_mpi xS;
        mbedtls_mpi_init( &xR );
        mbedtls_mpi_init( &xS );
        lMbedTLSResult = mbedtls_mpi_read_binary( &xR, &xSignature[ 0 ], 32 );
        lMbedTLSResult = mbedtls_mpi_read_binary( &xS, &xSignature[ 32 ], 32 );
        lMbedTLSResult = mbedtls_ecdsa_verify( &pxEcdsaContext->grp, xHashedMessage, sizeof( xHashedMessage ), &pxEcdsaContext->Q, &xR, &xS );
        mbedtls_mpi_free( &xR );
        mbedtls_mpi_free( &xS );
        TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedTLSResult, "mbedTLS failed to verify signature." );
    }

    mbedtls_pk_free( &xEcdsaContext );
}

/*
 * 1. Generates an Elliptic Curve P256 key pair
 * 2. Calls GetAttributeValue to check generated key attirbutes
 * 3. Constructs the public key using values from GetAttributeValue calls
 * 4. Uses private key to perform a sign operation
 * 5. Verifies the signature using mbedTLS library and reconstructed public key
 * 6. Verifies the signature using the public key just created.
 */
TEST( Full_PKCS11_EC, AFQP_GenerateKeyPair )
{
    CK_RV xResult;
    CK_OBJECT_HANDLE xPrivateKeyHandle;
    CK_OBJECT_HANDLE xPublicKeyHandle;
    /* Note that mbedTLS does not permit a signature on all 0's. */
    CK_BYTE xHashedMessage[ SHA256_DIGEST_SIZE ] = { 0xab };
    CK_MECHANISM xMechanism;
    CK_BYTE xSignature[ RSA_SIGNATURE_SIZE ] = { 0 };
    CK_BYTE xEcPoint[ 256 ] = { 0 };
    CK_BYTE xEcParams[ 11 ] = { 0 };
    CK_KEY_TYPE xKeyType;
    CK_BYTE xWhatDoesThisBufferDo[ 256 ] = { 0 };
    CK_ULONG xSignatureLength;
    CK_ATTRIBUTE xTemplate;
    /* mbedTLS structures for verification. */
    int lMbedTLSResult;
    mbedtls_ecdsa_context xEcdsaContext;
    uint8_t ucSecp256r1Oid[] = pkcs11DER_ENCODED_OID_P256; /*"\x06\x08" MBEDTLS_OID_EC_GRP_SECP256R1; */

    xResult = xProvisionGenerateKeyPairEC( xGlobalSession,
                                           "Amazon FreeRTOS test ECDSA private",
                                           "Amazon FreeRTOS test ECDSA public",
                                           &xPrivateKeyHandle,
                                           &xPublicKeyHandle );

    /* Call GetAttributeValue to retrieve information about the keypair stored. */

    /* Check that both keys are stored as EC Keys. */
    xTemplate.type = CKA_KEY_TYPE;
    xTemplate.pValue = &xKeyType;
    xTemplate.ulValueLen = sizeof( CK_KEY_TYPE );
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKeyHandle, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( CK_KEY_TYPE ), xTemplate.ulValueLen, "Length of key type incorrect in GetAttributeValue" );
    TEST_ASSERT_EQUAL_MESSAGE( CKK_EC, xKeyType, "Incorrect key type for private key" );

    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPublicKeyHandle, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( CK_KEY_TYPE ), xTemplate.ulValueLen, "Length of key type incorrect in GetAttributeValue" );
    TEST_ASSERT_EQUAL_MESSAGE( CKK_EC, xKeyType, "Incorrect key type for public key" );

    xTemplate.type = CKA_EC_PARAMS;
    xTemplate.pValue = xEcParams;
    xTemplate.ulValueLen = sizeof( xEcParams );
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKeyHandle, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( 10, xTemplate.ulValueLen, "Length of ECParameters identifier incorrect in GetAttributeValue" );
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE( ucSecp256r1Oid, xEcParams, xTemplate.ulValueLen, "EcParameters did not match P256 OID." );

    xTemplate.type = CKA_EC_POINT;
    xTemplate.pValue = xEcPoint;
    xTemplate.ulValueLen = sizeof( xEcPoint );
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPublicKeyHandle, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to retrieve EC Point." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, xPrivateKeyHandle, "Invalid object handle returned for EC private key." );

    /* Perform a sign with the generated private key. */
    xMechanism.mechanism = CKM_ECDSA;
    xMechanism.pParameter = NULL;
    xMechanism.ulParameterLen = 0;
    xResult = pxGlobalFunctionList->C_SignInit( xGlobalSession, &xMechanism, xPrivateKeyHandle );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to SignInit ECDSA." );

    xSignatureLength = sizeof( xSignature );
    xResult = pxGlobalFunctionList->C_Sign( xGlobalSession, xHashedMessage, SHA256_DIGEST_SIZE, xSignature, &xSignatureLength );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to ECDSA Sign." );

    /* Verify the signature with mbedTLS */
    mbedtls_ecdsa_init( &xEcdsaContext );
    mbedtls_ecp_group_init( &xEcdsaContext.grp );

    if( TEST_PROTECT() )
    {
        lMbedTLSResult = mbedtls_ecp_group_load( &xEcdsaContext.grp, MBEDTLS_ECP_DP_SECP256R1 );
        TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedTLSResult, "mbedTLS failed in setup for signature verification." );
        /* The first 2 bytes are for ASN1 type/length encoding. */
        lMbedTLSResult = mbedtls_ecp_point_read_binary( &xEcdsaContext.grp, &xEcdsaContext.Q, &xEcPoint[ 2 ], xTemplate.ulValueLen - 2 );
        TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedTLSResult, "mbedTLS failed in setup for signature verification." );

        /* An ECDSA signature is comprised of 2 components - R & S.  C_Sign returns them one after another. */
        mbedtls_mpi xR;
        mbedtls_mpi xS;
        mbedtls_mpi_init( &xR );
        mbedtls_mpi_init( &xS );
        lMbedTLSResult = mbedtls_mpi_read_binary( &xR, &xSignature[ 0 ], 32 );
        TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedTLSResult, "mbedTLS failed in setup for signature verification." );
        lMbedTLSResult = mbedtls_mpi_read_binary( &xS, &xSignature[ 32 ], 32 );
        TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedTLSResult, "mbedTLS failed in setup for signature verification." );
        lMbedTLSResult = mbedtls_ecdsa_verify( &xEcdsaContext.grp, xHashedMessage, sizeof( xHashedMessage ), &xEcdsaContext.Q, &xR, &xS );
        TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedTLSResult, "mbedTLS failed to verify signature." );
        mbedtls_mpi_free( &xR );
        mbedtls_mpi_free( &xS ); /* TODO: Won't get cleared by test protect. */

        /* Verify the signature with the generated public key. */
        xResult = pxGlobalFunctionList->C_VerifyInit( xGlobalSession, &xMechanism, xPublicKeyHandle );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to VerifyInit ECDSA." );
        xResult = pxGlobalFunctionList->C_Verify( xGlobalSession, xHashedMessage, SHA256_DIGEST_SIZE, xSignature, xSignatureLength );
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to Verify ECDSA." );
    }

    mbedtls_ecp_group_free( &xEcdsaContext.grp );
    mbedtls_ecdsa_free( &xEcdsaContext );
}

#include "mbedtls/entropy_poll.h"
TEST( Full_PKCS11_EC, AFQP_Verify )
{
    CK_RV xResult;
    CK_OBJECT_HANDLE xPrivateKey;
    CK_OBJECT_HANDLE xPublicKey;
    CK_OBJECT_HANDLE xCertificate;
    CK_MECHANISM xMechanism;
    CK_BYTE xHashedMessage[ SHA256_DIGEST_SIZE ] = { 0xbe };
    CK_BYTE xSignature[ ECDSA_SIGNATURE_SIZE + 10 ] = { 0 };
    CK_BYTE xSignaturePKCS[ 64 ] = { 0 };
    size_t xSignatureLength = ECDSA_SIGNATURE_SIZE;
    mbedtls_pk_context xPkContext;
    /* TODO: Consider switching this out for a C_GenerateRandom dependent function for ports not implementing mbedTLS. */
    mbedtls_entropy_context xEntropyContext;
    mbedtls_ctr_drbg_context xDrbgContext;
    int lMbedResult;

    prvProvisionEcTestCredentials( &xPrivateKey, &xCertificate, &xPublicKey );

    /* Sign data w/ PKCS. */
    xMechanism.mechanism = CKM_ECDSA;
    xMechanism.pParameter = NULL;
    xMechanism.ulParameterLen = 0;
    xResult = pxGlobalFunctionList->C_SignInit( xGlobalSession, &xMechanism, xPrivateKey );
    xResult = pxGlobalFunctionList->C_Sign( xGlobalSession, xHashedMessage, sizeof( xHashedMessage ), xSignature, &xSignatureLength );

    xResult = pxGlobalFunctionList->C_VerifyInit( xGlobalSession, &xMechanism, xPublicKey );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "VerifyInit failed." );

    xResult = pxGlobalFunctionList->C_Verify( xGlobalSession, xHashedMessage, SHA256_DIGEST_SIZE, xSignature, sizeof( xSignaturePKCS ) );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Verify failed." );
    /* Sign data with mbedTLS. */

    /* Initialize the private key. */
    mbedtls_pk_init( &xPkContext );
    lMbedResult = mbedtls_pk_parse_key( &xPkContext, cValidECDSAPrivateKey, sizeof( cValidECDSAPrivateKey ), NULL, 0 );
    TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedResult, "Failed to parse valid ECDSA key." );
    /* Initialize the RNG. */
    mbedtls_entropy_init( &xEntropyContext );
    mbedtls_ctr_drbg_init( &xDrbgContext );
    lMbedResult = mbedtls_ctr_drbg_seed( &xDrbgContext, mbedtls_entropy_func, &xEntropyContext, NULL, 0 );
    TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedResult, "Failed to initialize DRBG" );

    lMbedResult = mbedtls_pk_sign( &xPkContext, MBEDTLS_MD_SHA256, xHashedMessage, sizeof( xHashedMessage ), xSignature, &xSignatureLength, mbedtls_ctr_drbg_random, &xDrbgContext );
    TEST_ASSERT_EQUAL_MESSAGE( 0, lMbedResult, "Failed to perform ECDSA signature." );

    mbedtls_pk_free( &xPkContext );
    mbedtls_ctr_drbg_free( &xDrbgContext );
    mbedtls_entropy_free( &xEntropyContext );

    /* Reconstruct the signature in PKCS #11 format. */
    CK_BYTE * pxNextLength;
    uint8_t ucSigComponentLength = xSignature[ 3 ];

    /* R Component. */
    if( ucSigComponentLength == 33 )
    {
        memcpy( xSignaturePKCS, &xSignature[ 5 ], 32 ); /* Chop off the leading zero. */
        pxNextLength = xSignature + 5 /* Sequence, length, integer, length, leading zero */ + 32 /*(R) */ + 1 /*(S's integer tag) */;
    }
    else
    {
        memcpy( &xSignaturePKCS[ 32 - ucSigComponentLength ], &xSignature[ 4 ], ucSigComponentLength );
        pxNextLength = xSignature + 4 + 32 + 1;
    }

    /* S Component. */
    ucSigComponentLength = pxNextLength[ 0 ];

    if( ucSigComponentLength == 33 )
    {
        memcpy( &xSignaturePKCS[ 32 ], &pxNextLength[ 2 ], 32 ); /* Skip leading zero. */
    }
    else
    {
        memcpy( &xSignaturePKCS[ 64 - ucSigComponentLength ], &pxNextLength[ 1 ], ucSigComponentLength );
    }

    /* Verify with PKCS #11. */
    xMechanism.mechanism = CKM_ECDSA;
    xMechanism.pParameter = NULL;
    xMechanism.ulParameterLen = 0;
    xResult = pxGlobalFunctionList->C_VerifyInit( xGlobalSession, &xMechanism, xPublicKey );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "VerifyInit failed." );

    xResult = pxGlobalFunctionList->C_Verify( xGlobalSession, xHashedMessage, SHA256_DIGEST_SIZE, xSignaturePKCS, sizeof( xSignaturePKCS ) );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Verify failed." );



    /* Modify signature value and make sure verification fails. */
}

TEST( Full_PKCS11_EC, AFQP_FindObject )
{
    CK_OBJECT_HANDLE xPrivateKey;
    CK_OBJECT_HANDLE xPublicKey;
    CK_OBJECT_HANDLE xCertificate;

    prvProvisionEcTestCredentials( &xPrivateKey, &xCertificate, &xPublicKey );

    /* Provision a device public key as well. */
    prvFindObjectTest();
}


TEST( Full_PKCS11_EC, AFQP_GetAttributeValue )
{
    CK_RV xResult;
    CK_OBJECT_HANDLE xPrivateKey;
    CK_OBJECT_HANDLE xPublicKey;
    CK_OBJECT_HANDLE xCertificate;
    CK_ATTRIBUTE xTemplate;
    CK_KEY_TYPE xKeyType = 0;
    uint8_t ucP256Oid[] = pkcs11DER_ENCODED_OID_P256;
    CK_BYTE xEcParams[ 10 ] = { 0 };

    prvProvisionEcTestCredentials( &xPrivateKey, &xCertificate, &xPublicKey );

    /* Check Key Type. */

    /* Check that NULL pointer provides correct length. */
    xTemplate.type = CKA_KEY_TYPE;
    xTemplate.pValue = NULL;
    xTemplate.ulValueLen = 0;
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKey, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "GetAttributeValue for length of EC key type failed." );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( CK_KEY_TYPE ), xTemplate.ulValueLen, "Incorrect key type length provided." );
    /* Check key type value. */
    xTemplate.pValue = &xKeyType;
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKey, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "GetAttributeValue for EC key type failed." );
    TEST_ASSERT_EQUAL_MESSAGE( CKK_EC, xKeyType, "Incorrect key type returned." );

    /* Check Ec Params. */
    xTemplate.type = CKA_EC_PARAMS;
    xTemplate.pValue = NULL;
    xTemplate.ulValueLen = 0;
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKey, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "GetAttributeValue for length of EC key type failed." );
    TEST_ASSERT_EQUAL_MESSAGE( sizeof( ucP256Oid ), xTemplate.ulValueLen, "Incorrect key type length provided." );
    /* Check ECParameters value. */
    xTemplate.pValue = xEcParams;
    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, xPrivateKey, &xTemplate, 1 );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "GetAttributeValue for EC key type failed." );
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE( ucP256Oid, xEcParams, sizeof( ucP256Oid ), "Incorrect ECParameters returned from GetAttributeValue" );
}

/* TODO: Power cycle tests for key persistance. */
/* TODO: Test for Root CA & OTA Code Signing Certificate. */
