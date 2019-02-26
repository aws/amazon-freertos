/*
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 */

/**
 * @file iot_tests_mqtt_api.c
 * @brief Tests for the user-facing API functions (declared in iot_mqtt.h).
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* POSIX includes. */
#ifdef POSIX_TIME_HEADER
    #include POSIX_TIME_HEADER
#else
    #include <time.h>
#endif
#ifdef POSIX_UNISTD_HEADER
    #include POSIX_UNISTD_HEADER
#else
    #include <unistd.h>
#endif

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/* Test framework includes. */
#include "unity_fixture.h"

/* MQTT test access include. */
#include "iot_test_access_mqtt.h"

/*-----------------------------------------------------------*/

/**
 * @brief Determine which MQTT server mode to test (AWS IoT or Mosquitto).
 */
#if !defined( IOT_TEST_MQTT_MOSQUITTO ) || IOT_TEST_MQTT_MOSQUITTO == 0
    #define _AWS_IOT_MQTT_SERVER    true
#else
    #define _AWS_IOT_MQTT_SERVER    false

/* Redefine the connect info initializer if not using an AWS IoT MQTT server. */
    #undef IOT_MQTT_CONNECT_INFO_INITIALIZER
    #define IOT_MQTT_CONNECT_INFO_INITIALIZER    { 0 }
#endif

/**
 * @brief Timeout to use for the tests. This can be short, but should allow time
 * for other threads to run.
 */
#define _TIMEOUT_MS                  ( 400 )

/**
 * @brief A short keep-alive interval to use for the keep-alive tests. It may be
 * shorter than the minimum 1 second specified by the MQTT spec.
 */
#define _SHORT_KEEP_ALIVE_MS         ( 100 )

/**
 * @brief The number of times the periodic keep-alive should run.
 */
#define _KEEP_ALIVE_COUNT            ( 10 )

/*
 * Client identifier and length to use for the MQTT API tests.
 */
#define _CLIENT_IDENTIFIER           ( "test" )                                            /**< @brief Client identifier. */
#define _CLIENT_IDENTIFIER_LENGTH    ( ( uint16_t ) ( sizeof( _CLIENT_IDENTIFIER ) - 1 ) ) /**< @brief Length of client identifier. */

/*
 * Will topic name and length to use for the MQTT API tests.
 */
#define _TEST_TOPIC_NAME             ( "/test/topic" )                                   /**< @brief An arbitrary topic name. */
#define _TEST_TOPIC_NAME_LENGTH      ( ( uint16_t ) ( sizeof( _TEST_TOPIC_NAME ) - 1 ) ) /**< @brief Length of topic name. */

/**
 * @brief A non-NULL function pointer to use for subscription callback. This
 * "function" should cause a crash if actually called.
 */
#define _SUBSCRIPTION_CALLBACK \
    ( ( void ( * )( void *,    \
                    IotMqttCallbackParam_t * ) ) 0x01 )

/**
 * @brief How many times #TEST_MQTT_Unit_API_DisconnectMallocFail_ will test
 * malloc failures.
 *
 * The DISCONNECT function provides no mechanism to wait on its successful
 * completion. Therefore, this function simply uses the value below as an estimate
 * for the maximum number of times DISCONNECT will use malloc.
 */
#define _DISCONNECT_MALLOC_LIMIT    ( 20 )

/*
 * Constants that affect the behavior of #TEST_MQTT_Unit_API_PublishDuplicates.
 */
#define _DUP_CHECK_RETRY_MS         ( 100 )  /**< @brief When to start sending duplicate packets. */
#define _DUP_CHECK_RETRY_LIMIT      ( 3 )    /**< @brief How many duplicate packets to send. */
#define _DUP_CHECK_TIMEOUT          ( 3000 ) /**< @brief Total time allowed to send all duplicate packets.
                                              * Duplicates are sent using an exponential backoff strategy. */
/** @brief The minimum amount of time the test can take. */
#define _DUP_CHECK_MINIMUM_WAIT \
    ( _DUP_CHECK_RETRY_MS +     \
      2 * _DUP_CHECK_RETRY_MS + \
      4 * _DUP_CHECK_RETRY_MS + \
      IOT_MQTT_RESPONSE_WAIT_MS )

/*-----------------------------------------------------------*/

/**
 * @brief Tracks whether #_publishSetDup has been called.
 */
static bool _publishSetDupCalled = false;

/**
 * @brief Counts how many time #_sendPingreq has been called.
 */
static int32_t _pingreqSendCount = 0;

/*-----------------------------------------------------------*/

/**
 * @brief A thread routine that simulates an incoming PINGRESP.
 */
static void _incomingPingresp( void * pArgument )
{
    /* The sleep time calculation below does not work if the short keep-alive
     * interval is 1 second or greater. */
    #if _SHORT_KEEP_ALIVE_MS >= 1000
    #error "_SHORT_KEEP_ALIVE_MS must be less than 1000."
    #endif

    /* This test will not work if the response wait time is too small. */
    #if IOT_MQTT_RESPONSE_WAIT_MS < ( 2 * _SHORT_KEEP_ALIVE_MS + 100 )
    #error "IOT_MQTT_RESPONSE_WAIT_MS too small for keep-alive tests."
    #endif

    static int32_t invokeCount = 0;
    static uint64_t lastInvokeTime = 0;
    uint64_t currentTime = IotClock_GetTimeMs();

    /* A PINGRESP packet. */
    const uint8_t pPingresp[ 2 ] = { _MQTT_PACKET_TYPE_PINGRESP, 0x00 };

    /* Sleep time of twice the keep-alive interval. */
    const struct timespec sleepTime = { .tv_sec = 0, .tv_nsec = 2 * _SHORT_KEEP_ALIVE_MS * 1000000 };

    /* Increment invoke count for this function. */
    invokeCount++;

    /* Sleep to simulate the network round-trip time. */
    if( nanosleep( &sleepTime, NULL ) == 0 )
    {
        /* Respond with a PINGRESP. */
        if( invokeCount <= _KEEP_ALIVE_COUNT )
        {
            /* Log a status with Unity, as this test may take a while. */
            UnityPrint( "KeepAlivePeriodic " );
            UnityPrintNumber( ( UNITY_INT ) invokeCount );
            UnityPrint( " of " );
            UnityPrintNumber( ( UNITY_INT ) _KEEP_ALIVE_COUNT );
            UnityPrint( " DONE at " );
            UnityPrintNumber( ( UNITY_INT ) IotClock_GetTimeMs() );
            UnityPrint( " ms" );

            if( invokeCount > 1 )
            {
                UnityPrint( " (+" );
                UnityPrintNumber( ( UNITY_INT ) ( currentTime - lastInvokeTime ) );
                UnityPrint( " ms)." );
            }
            else
            {
                UnityPrint( "." );
            }

            UNITY_PRINT_EOL();
            lastInvokeTime = currentTime;

            ( void ) IotMqtt_ReceiveCallback( pArgument,
                                              NULL,
                                              pPingresp,
                                              2,
                                              0,
                                              NULL );
        }
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief PUBLISH set DUP function override.
 */
static void _publishSetDup( bool awsIotMqttMode,
                            uint8_t * pPublishPacket,
                            uint16_t * pNewPacketIdentifier )
{
    _publishSetDupCalled = true;

    _IotMqtt_PublishSetDup( awsIotMqttMode,
                            pPublishPacket,
                            pNewPacketIdentifier );
}

/*-----------------------------------------------------------*/

/**
 * @brief A send function that always "succeeds". May report that it was invoked
 * through a semaphore.
 */
static size_t _sendSuccess( void * pSendContext,
                            const uint8_t * pMessage,
                            size_t messageLength )
{
    IotSemaphore_t * pWaitSem = ( IotSemaphore_t * ) pSendContext;

    /* Silence warnings about unused parameters. */
    ( void ) pMessage;

    /* Post to the wait semaphore if given. */
    if( pWaitSem != NULL )
    {
        IotSemaphore_Post( pWaitSem );
    }

    /* This function returns the message length to simulate a successful send. */
    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief A send function for PINGREQ that responds with a PINGRESP.
 */
static size_t _sendPingreq( void * pSendContext,
                            const uint8_t * pMessage,
                            size_t messageLength )
{
    _mqttConnection_t * pMqttConnection = ( _mqttConnection_t * ) pSendContext;

    /* Silence warnings about unused parameters. */
    ( void ) pMessage;

    /* Create a thread that responds with PINGRESP, then increment the PINGREQ
     * send counter if successful. */
    if( Iot_CreateDetachedThread( _incomingPingresp,
                                  &( pMqttConnection->network.pSendContext ),
                                  IOT_THREAD_DEFAULT_PRIORITY,
                                  IOT_THREAD_DEFAULT_STACK_SIZE ) == true )
    {
        _pingreqSendCount++;
    }

    /* This function returns the message length to simulate a successful send. */
    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief A send function that delays.
 */
static size_t _sendDelay( void * pSendContext,
                          const uint8_t * pMessage,
                          size_t messageLength )
{
    IotSemaphore_t * pWaitSem = ( IotSemaphore_t * ) pSendContext;

    /* Silence warnings about unused parameters. */
    ( void ) pMessage;

    /* Post to the wait semaphore. */
    IotSemaphore_Post( pWaitSem );

    /* Delay for 2 seconds. */
    sleep( 2 );

    /* This function returns the message length to simulate a successful send. */
    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief This send function checks that a duplicate outgoing message differs from
 * the original.
 */
static size_t _dupChecker( void * pSendContext,
                           const uint8_t * pMessage,
                           size_t messageLength )
{
    static int32_t runCount = 0;
    static bool status = true;
    bool * pDupCheckResult = ( bool * ) pSendContext;
    uint8_t publishFlags = *pMessage;

    /* Declare the remaining variables required to check packet identifier
     * for the AWS IoT MQTT server. */
    #if _AWS_IOT_MQTT_SERVER == true
        static uint16_t lastPacketIdentifier = 0;
        uint16_t currentPacketIdentifier = 0;
        size_t bytesProcessed = 0;
        IotMqttPublishInfo_t publishInfo = { 0 };
    #endif

    /* Ignore any MQTT packet that's not a PUBLISH. */
    if( ( publishFlags & 0xf0 ) != _MQTT_PACKET_TYPE_PUBLISH )
    {
        return messageLength;
    }

    runCount++;

    /* Check how many times this function has been called. */
    if( runCount == 1 )
    {
        #if _AWS_IOT_MQTT_SERVER == true
            /* Deserialize the PUBLISH to read the packet identifier. */
            if( _IotMqtt_DeserializePublish( pMessage,
                                             messageLength,
                                             &publishInfo,
                                             &lastPacketIdentifier,
                                             &bytesProcessed ) != IOT_MQTT_SUCCESS )
            {
                status = false;
            }
            else
            {
                /* Ensure that all bytes of the PUBLISH were processed. */
                status = ( bytesProcessed == messageLength );
            }
        #else /* if _AWS_IOT_MQTT_SERVER == true */
            /* DUP flag should not be set on this function's first run. */
            if( ( publishFlags & 0x08 ) == 0x08 )
            {
                status = false;
            }
        #endif /* if _AWS_IOT_MQTT_SERVER == true */
    }
    else
    {
        /* Only check the packet again if the previous run checks passed. */
        if( status == true )
        {
            #if _AWS_IOT_MQTT_SERVER == true
                /* Deserialize the PUBLISH to read the packet identifier. */
                if( _IotMqtt_DeserializePublish( pMessage,
                                                 messageLength,
                                                 &publishInfo,
                                                 &currentPacketIdentifier,
                                                 &bytesProcessed ) != IOT_MQTT_SUCCESS )
                {
                    status = false;
                }
                else
                {
                    /* Ensure that all bytes of the PUBLISH were processed. */
                    status = ( bytesProcessed == messageLength );

                    /* Check that the packet identifier is different. */
                    if( status == true )
                    {
                        status = ( currentPacketIdentifier != lastPacketIdentifier );
                        lastPacketIdentifier = currentPacketIdentifier;
                    }
                }
            #else /* if _AWS_IOT_MQTT_SERVER == true */
                /* DUP flag should be set when this function runs again. */
                if( ( publishFlags & 0x08 ) != 0x08 )
                {
                    status = false;
                }
            #endif /* if _AWS_IOT_MQTT_SERVER == true */
        }

        /* Write the check result on the last expected run of this function. */
        if( runCount == _DUP_CHECK_RETRY_LIMIT )
        {
            *pDupCheckResult = status;
        }
    }

    /* Return the message length to simulate a successful send. */
    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief A disconnect function that counts how many times it was invoked.
 */
static IotNetworkError_t _disconnect( void * pDisconnectContext )
{
    int32_t * pDisconnectCount = ( int32_t * ) pDisconnectContext;

    /* Increment the counter for how many times this function was invoked. */
    if( pDisconnectCount != NULL )
    {
        ( *pDisconnectCount )++;
    }

    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

/**
 * @brief A task pool job routine that decrements an MQTT operation's job
 * reference count.
 */
static void _decrementReferencesJob( IotTaskPool_t * pTaskPool,
                                     IotTaskPoolJob_t * pJob,
                                     void * pContext )
{
    _mqttOperation_t * pOperation = ( _mqttOperation_t * ) pContext;

    /* Silence warnings about unused parameters. */
    ( void ) pTaskPool;
    ( void ) pJob;

    /* Decrement an operation's reference count. */
    if( _IotMqtt_DecrementOperationReferences( pOperation, false ) == false )
    {
        /* Unblock the main test thread. */
        IotSemaphore_Post( &( pOperation->notify.waitSemaphore ) );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group for MQTT API tests.
 */
TEST_GROUP( MQTT_Unit_API );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for MQTT API tests.
 */
TEST_SETUP( MQTT_Unit_API )
{
    _publishSetDupCalled = false;
    _pingreqSendCount = 0;

    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, IotMqtt_Init() );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for MQTT API tests.
 */
TEST_TEAR_DOWN( MQTT_Unit_API )
{
    IotMqtt_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for MQTT API tests.
 */
TEST_GROUP_RUNNER( MQTT_Unit_API )
{
    RUN_TEST_CASE( MQTT_Unit_API, OperationCreateDestroy );
    RUN_TEST_CASE( MQTT_Unit_API, OperationWaitTimeout );
    RUN_TEST_CASE( MQTT_Unit_API, ConnectParameters );
    RUN_TEST_CASE( MQTT_Unit_API, ConnectMallocFail );
    RUN_TEST_CASE( MQTT_Unit_API, ConnectRestoreSessionMallocFail );
    RUN_TEST_CASE( MQTT_Unit_API, DisconnectMallocFail );
    RUN_TEST_CASE( MQTT_Unit_API, PublishQoS0Parameters );
    RUN_TEST_CASE( MQTT_Unit_API, PublishQoS0MallocFail );
    RUN_TEST_CASE( MQTT_Unit_API, PublishQoS1 );
    RUN_TEST_CASE( MQTT_Unit_API, PublishDuplicates );
    RUN_TEST_CASE( MQTT_Unit_API, SubscribeUnsubscribeParameters );
    RUN_TEST_CASE( MQTT_Unit_API, SubscribeMallocFail );
    RUN_TEST_CASE( MQTT_Unit_API, UnsubscribeMallocFail );
    RUN_TEST_CASE( MQTT_Unit_API, KeepAlivePeriodic );
    RUN_TEST_CASE( MQTT_Unit_API, KeepAliveJobCleanup );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test reference counts as MQTT operations are created and destroyed.
 */
TEST( MQTT_Unit_API, OperationCreateDestroy )
{
    _mqttConnection_t * pMqttConnection = NULL;
    _mqttOperation_t * pOperation = NULL;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;

    /* Create a new MQTT connection with an empty network interface. */
    pMqttConnection = IotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                        &networkInterface,
                                                        0 );
    TEST_ASSERT_NOT_NULL( pMqttConnection );

    /* Adjustment to reference count based on keep-alive status. */
    const int32_t keepAliveReference = 1 + ( ( pMqttConnection->keepAliveMs != 0 ) ? 1 : 0 );

    /* A new MQTT connection should only have a possible reference for keep-alive. */
    TEST_ASSERT_EQUAL_INT32( keepAliveReference, pMqttConnection->references );

    /* Create a new operation referencing the MQTT connection. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _IotMqtt_CreateOperation( pMqttConnection,
                                                                   IOT_MQTT_FLAG_WAITABLE,
                                                                   NULL,
                                                                   &pOperation ) );

    /* Check reference counts and list placement. */
    TEST_ASSERT_EQUAL_INT32( 1 + keepAliveReference, pMqttConnection->references );
    TEST_ASSERT_EQUAL_INT32( 2, pOperation->jobReference );
    TEST_ASSERT_EQUAL_PTR( &( pOperation->link ), IotListDouble_FindFirstMatch( &( pMqttConnection->pendingProcessing ),
                                                                                NULL,
                                                                                NULL,
                                                                                &( pOperation->link ) ) );

    /* Schedule a job that destroys the operation. */
    TEST_ASSERT_EQUAL( IOT_TASKPOOL_SUCCESS, IotTaskPool_CreateJob( _decrementReferencesJob,
                                                                    pOperation,
                                                                    &( pOperation->job ) ) );
    TEST_ASSERT_EQUAL( IOT_TASKPOOL_SUCCESS, IotTaskPool_Schedule( &( _IotMqttTaskPool ),
                                                                   &( pOperation->job ) ) );

    /* Wait for the job to complete. */
    IotSemaphore_Wait( &( pOperation->notify.waitSemaphore ) );

    /* Check reference counts after job completion. */
    TEST_ASSERT_EQUAL_INT32( 1 + keepAliveReference, pMqttConnection->references );
    TEST_ASSERT_EQUAL_INT32( 1, pOperation->jobReference );
    TEST_ASSERT_EQUAL_PTR( &( pOperation->link ), IotListDouble_FindFirstMatch( &( pMqttConnection->pendingProcessing ),
                                                                                NULL,
                                                                                NULL,
                                                                                &( pOperation->link ) ) );

    /* Disconnect the MQTT connection, then call Wait to clean up the operation. */
    IotMqtt_Disconnect( pMqttConnection, true );
    IotMqtt_Wait( pOperation, 0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that an operation is correctly cleaned up if @ref mqtt_function_wait
 * times out while its job is executing.
 */
TEST( MQTT_Unit_API, OperationWaitTimeout )
{
    _mqttConnection_t * pMqttConnection = NULL;
    _mqttOperation_t * pOperation = NULL;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
    IotSemaphore_t waitSem;

    /* An arbitrary MQTT packet for this test. */
    uint8_t pPacket[ 2 ] = { _MQTT_PACKET_TYPE_DISCONNECT, 0x00 };

    /* Create the wait semaphore. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &waitSem, 0, 1 ) );

    if( TEST_PROTECT() )
    {
        /* Set the members of the network interface and create a new MQTT connection. */
        networkInterface.pSendContext = &waitSem;
        networkInterface.send = _sendDelay;
        pMqttConnection = IotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                            &networkInterface,
                                                            0 );
        TEST_ASSERT_NOT_NULL( pMqttConnection );

        /* Create a new operation referencing the MQTT connection. */
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _IotMqtt_CreateOperation( pMqttConnection,
                                                                       IOT_MQTT_FLAG_WAITABLE,
                                                                       NULL,
                                                                       &pOperation ) );

        /* Set an arbitrary MQTT packet for the operation. */
        pOperation->operation = IOT_MQTT_DISCONNECT;
        pOperation->pMqttPacket = pPacket;
        pOperation->packetSize = 2;

        /* Schedule the send job. */
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _IotMqtt_ScheduleOperation( pOperation,
                                                                         _IotMqtt_ProcessSend,
                                                                         0 ) );

        /* Wait for the send job to begin. */
        IotSemaphore_Wait( &waitSem );

        /* Wait on the MQTT operation with a short timeout. This should cause a
         * timeout while the send job is still executing. */
        TEST_ASSERT_EQUAL( IOT_MQTT_TIMEOUT, IotMqtt_Wait( pOperation, 10 ) );

        /* Check reference count after a timed out wait. */
        IotMutex_Lock( &( pMqttConnection->referencesMutex ) );
        TEST_ASSERT_EQUAL_INT32( 1, pOperation->jobReference );
        IotMutex_Unlock( &( pMqttConnection->referencesMutex ) );

        /* Disconnect the MQTT connection. */
        IotMqtt_Disconnect( pMqttConnection, true );

        /* Clean up the MQTT library, which waits for the send job to finish. The
         * library must be re-initialized so that test tear down does not crash. */
        IotMqtt_Cleanup();
        IotMqtt_Init();
    }

    IotSemaphore_Destroy( &waitSem );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_connect with various
 * invalid parameters.
 */
TEST( MQTT_Unit_API, ConnectParameters )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttConnection_t mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttPublishInfo_t willInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;

    /* Check that the network interface is validated. */
    status = IotMqtt_Connect( &mqttConnection,
                              &networkInterface,
                              &connectInfo,
                              _TIMEOUT_MS );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
    networkInterface.send = _sendSuccess;

    /* Check that the connection info is validated. */
    status = IotMqtt_Connect( &mqttConnection,
                              &networkInterface,
                              &connectInfo,
                              _TIMEOUT_MS );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
    connectInfo.pClientIdentifier = _CLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = _CLIENT_IDENTIFIER_LENGTH;

    /* Connect with bad previous session subscription. */
    connectInfo.cleanSession = false;
    connectInfo.pPreviousSubscriptions = &subscription;
    connectInfo.previousSubscriptionCount = 1;
    status = IotMqtt_Connect( &mqttConnection,
                              &networkInterface,
                              &connectInfo,
                              _TIMEOUT_MS );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

    /* Connect with bad subscription count. */
    connectInfo.previousSubscriptionCount = 0;
    subscription.pTopicFilter = _TEST_TOPIC_NAME;
    subscription.topicFilterLength = _TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = _SUBSCRIPTION_CALLBACK;
    status = IotMqtt_Connect( &mqttConnection,
                              &networkInterface,
                              &connectInfo,
                              _TIMEOUT_MS );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
    connectInfo.previousSubscriptionCount = 1;

    /* Check that the will info is validated when it's provided. */
    connectInfo.pWillInfo = &willInfo;
    status = IotMqtt_Connect( &mqttConnection,
                              &networkInterface,
                              &connectInfo,
                              _TIMEOUT_MS );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
    willInfo.pTopicName = _TEST_TOPIC_NAME;
    willInfo.topicNameLength = _TEST_TOPIC_NAME_LENGTH;

    /* Check that a will message longer than 65535 is not allowed. */
    willInfo.pPayload = "";
    willInfo.payloadLength = 65536;
    status = IotMqtt_Connect( &mqttConnection,
                              &networkInterface,
                              &connectInfo,
                              _TIMEOUT_MS );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
    willInfo.payloadLength = 0;

    /* Check that passing a wait time of 0 returns immediately. */
    status = IotMqtt_Connect( &mqttConnection,
                              &networkInterface,
                              &connectInfo,
                              0 );
    TEST_ASSERT_EQUAL( IOT_MQTT_TIMEOUT, status );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_connect when memory
 * allocation fails at various points.
 */
TEST( MQTT_Unit_API, ConnectMallocFail )
{
    int32_t i = 0;
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttConnection_t mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;

    /* Initialize parameters. */
    networkInterface.send = _sendSuccess;
    connectInfo.keepAliveSeconds = 100;
    connectInfo.cleanSession = true;
    connectInfo.pClientIdentifier = _CLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = _CLIENT_IDENTIFIER_LENGTH;

    for( i = 0; ; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call CONNECT. Memory allocation will fail at various times during
         * this call. */
        status = IotMqtt_Connect( &mqttConnection,
                                  &networkInterface,
                                  &connectInfo,
                                  _TIMEOUT_MS );

        /* If the return value is timeout, then all memory allocation succeeded
         * and the loop can exit. The expected return value is timeout (and not
         * success) because the receive callback is never invoked. */
        if( status == IOT_MQTT_TIMEOUT )
        {
            break;
        }

        /* If the return value isn't timeout, check that it is memory allocation
         * failure. */
        TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_connect when memory
 * allocation fails at various points for a persistent session.
 */
TEST( MQTT_Unit_API, ConnectRestoreSessionMallocFail )
{
    int32_t i = 0;
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttConnection_t mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;

    /* Initialize parameters. */
    networkInterface.send = _sendSuccess;
    connectInfo.cleanSession = false;
    connectInfo.keepAliveSeconds = 100;
    connectInfo.pClientIdentifier = _CLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = _CLIENT_IDENTIFIER_LENGTH;
    subscription.pTopicFilter = _TEST_TOPIC_NAME;
    subscription.topicFilterLength = _TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = _SUBSCRIPTION_CALLBACK;

    connectInfo.pPreviousSubscriptions = &subscription;
    connectInfo.previousSubscriptionCount = 1;

    for( i = 0; ; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call CONNECT with a previous session. Memory allocation will fail at
         * various times during this call. */
        status = IotMqtt_Connect( &mqttConnection,
                                  &networkInterface,
                                  &connectInfo,
                                  _TIMEOUT_MS );

        /* If the return value is timeout, then all memory allocation succeeded
         * and the loop can exit. The expected return value is timeout (and not
         * success) because the receive callback is never invoked. */
        if( status == IOT_MQTT_TIMEOUT )
        {
            break;
        }

        /* If the return value isn't timeout, check that it is memory allocation
         * failure. */
        TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_disconnect when memory
 * allocation fails at various points.
 */
TEST( MQTT_Unit_API, DisconnectMallocFail )
{
    int32_t i = 0;
    int32_t disconnectCount = 0;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
    _mqttConnection_t * pMqttConnection = NULL;

    /* Set the members of the network interface. */
    networkInterface.pDisconnectContext = &disconnectCount;
    networkInterface.send = _sendSuccess;
    networkInterface.disconnect = _disconnect;

    for( i = 0; i < _DISCONNECT_MALLOC_LIMIT; i++ )
    {
        /* Allow unlimited use of malloc during connection initialization. */
        UnityMalloc_MakeMallocFailAfterCount( -1 );

        /* Create a new MQTT connection. */
        pMqttConnection = IotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                            &networkInterface,
                                                            0 );
        TEST_ASSERT_NOT_NULL( pMqttConnection );

        /* Set malloc to eventually fail. */
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call DISCONNECT; this function should always perform cleanup regardless
         * of memory allocation errors. */
        IotMqtt_Disconnect( pMqttConnection, false );
        TEST_ASSERT_EQUAL_INT( 1, disconnectCount );
        disconnectCount = 0;
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_publish (QoS 0) with various
 * valid and invalid parameters.
 */
TEST( MQTT_Unit_API, PublishQoS0Parameters )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    _mqttConnection_t * pMqttConnection = NULL;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttReference_t publishReference = IOT_MQTT_REFERENCE_INITIALIZER;
    IotMqttCallbackInfo_t callbackInfo = IOT_MQTT_CALLBACK_INFO_INITIALIZER;

    /* Set the members of the network interface and create a new MQTT connection. */
    networkInterface.send = _sendSuccess;
    pMqttConnection = IotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                        &networkInterface,
                                                        0 );
    TEST_ASSERT_NOT_NULL( pMqttConnection );

    if( TEST_PROTECT() )
    {
        /* Check that the publish info is validated. */
        status = IotMqtt_Publish( pMqttConnection, &publishInfo, 0, NULL, NULL );
        TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
        publishInfo.pTopicName = _TEST_TOPIC_NAME;
        publishInfo.topicNameLength = _TEST_TOPIC_NAME_LENGTH;

        /* Check that a QoS 0 publish is refused if a notification is requested. */
        status = IotMqtt_Publish( pMqttConnection, &publishInfo, IOT_MQTT_FLAG_WAITABLE, NULL, &publishReference );
        TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
        status = IotMqtt_Publish( pMqttConnection, &publishInfo, 0, &callbackInfo, NULL );
        TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

        /* If valid parameters are passed, QoS 0 publish should always return success. */
        status = IotMqtt_Publish( pMqttConnection, &publishInfo, 0, 0, &publishReference );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

        /* Allow time for the send job to run and clean up the PUBLISH. QoS 0
         * PUBLISH provides no mechanism to wait on completion, so sleep is used. */
        sleep( 1 );
    }

    IotMqtt_Disconnect( pMqttConnection, true );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_publish (QoS 0) when memory
 * allocation fails at various points.
 */
TEST( MQTT_Unit_API, PublishQoS0MallocFail )
{
    int32_t i = 0;
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    _mqttConnection_t * pMqttConnection = NULL;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;

    /* Set the members of the network interface and create a new MQTT connection. */
    networkInterface.send = _sendSuccess;
    pMqttConnection = IotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                        &networkInterface,
                                                        0 );
    TEST_ASSERT_NOT_NULL( pMqttConnection );

    /* Set the necessary members of publish info. */
    publishInfo.pTopicName = _TEST_TOPIC_NAME;
    publishInfo.topicNameLength = _TEST_TOPIC_NAME_LENGTH;

    if( TEST_PROTECT() )
    {
        for( i = 0; ; i++ )
        {
            UnityMalloc_MakeMallocFailAfterCount( i );

            /* Call PUBLISH. Memory allocation will fail at various times during
             * this call. */
            status = IotMqtt_Publish( pMqttConnection, &publishInfo, 0, NULL, NULL );

            /* Once PUBLISH succeeds, the loop can exit. */
            if( status == IOT_MQTT_SUCCESS )
            {
                break;
            }

            /* If the return value isn't success, check that it is memory allocation
             * failure. */
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }

        /* Wait for any pending QoS 0 publishes to clean up. */
        sleep( 1 );
    }

    IotMqtt_Disconnect( pMqttConnection, true );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_publish (QoS 1) with various
 * invalid parameters. Also tests the behavior of @ref mqtt_function_publish
 * (QoS 1) when memory allocation fails at various points.
 */
TEST( MQTT_Unit_API, PublishQoS1 )
{
    int32_t i = 0;
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    _mqttConnection_t * pMqttConnection = NULL;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttReference_t publishRef = IOT_MQTT_REFERENCE_INITIALIZER;
    IotMqttCallbackInfo_t callbackInfo = IOT_MQTT_CALLBACK_INFO_INITIALIZER;

    /* Set the members of the network interface and create a new MQTT connection. */
    networkInterface.send = _sendSuccess;
    pMqttConnection = IotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                        &networkInterface,
                                                        0 );
    TEST_ASSERT_NOT_NULL( pMqttConnection );

    /* Set the publish info. */
    publishInfo.qos = IOT_MQTT_QOS_1;
    publishInfo.pTopicName = _TEST_TOPIC_NAME;
    publishInfo.topicNameLength = _TEST_TOPIC_NAME_LENGTH;

    if( TEST_PROTECT() )
    {
        /* Setting the waitable flag with no reference is not allowed. */
        status = IotMqtt_Publish( pMqttConnection,
                                  &publishInfo,
                                  IOT_MQTT_FLAG_WAITABLE,
                                  NULL,
                                  NULL );
        TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

        /* Setting both the waitable flag and callback info is not allowed. */
        status = IotMqtt_Publish( pMqttConnection,
                                  &publishInfo,
                                  IOT_MQTT_FLAG_WAITABLE,
                                  &callbackInfo,
                                  &publishRef );
        TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

        /* Check QoS 1 PUBLISH behavior with malloc failures. */
        for( i = 0; ; i++ )
        {
            UnityMalloc_MakeMallocFailAfterCount( i );

            /* Call PUBLISH. Memory allocation will fail at various times during
             * this call. */
            status = IotMqtt_Publish( pMqttConnection,
                                      &publishInfo,
                                      IOT_MQTT_FLAG_WAITABLE,
                                      NULL,
                                      &publishRef );

            /* If the PUBLISH succeeded, the loop can exit after waiting for the QoS
             * 1 PUBLISH to be cleaned up. */
            if( status == IOT_MQTT_STATUS_PENDING )
            {
                TEST_ASSERT_EQUAL( IOT_MQTT_TIMEOUT, IotMqtt_Wait( publishRef, _TIMEOUT_MS ) );
                break;
            }

            /* If the return value isn't success, check that it is memory allocation
             * failure. */
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }
    }

    /* Clean up MQTT connection. */
    IotMqtt_Disconnect( pMqttConnection, true );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests that duplicate QoS 1 PUBLISH packets are different from the
 * original.
 *
 * For non-AWS IoT MQTT servers, checks that the DUP flag is set. For
 * AWS IoT MQTT servers, checks that the packet identifier is different.
 */
TEST( MQTT_Unit_API, PublishDuplicates )
{
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
    _mqttConnection_t * pMqttConnection = NULL;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttReference_t publishRef = IOT_MQTT_REFERENCE_INITIALIZER;
    volatile bool dupCheckResult = false;
    uint64_t startTime = 0;

    /* Set the members of the network interface and create a new MQTT connection. */
    networkInterface.pSendContext = ( void * ) &dupCheckResult;
    networkInterface.send = _dupChecker;
    networkInterface.serialize.publishSetDup = _publishSetDup;
    pMqttConnection = IotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                        &networkInterface,
                                                        0 );
    TEST_ASSERT_NOT_NULL( pMqttConnection );

    /* Set the publish info. */
    publishInfo.qos = IOT_MQTT_QOS_1;
    publishInfo.pTopicName = _TEST_TOPIC_NAME;
    publishInfo.topicNameLength = _TEST_TOPIC_NAME_LENGTH;
    publishInfo.pPayload = "test";
    publishInfo.payloadLength = 4;
    publishInfo.retryMs = _DUP_CHECK_RETRY_MS;
    publishInfo.retryLimit = _DUP_CHECK_RETRY_LIMIT;

    startTime = IotClock_GetTimeMs();

    if( TEST_PROTECT() )
    {
        /* Send a PUBLISH with retransmissions enabled. */
        TEST_ASSERT_EQUAL( IOT_MQTT_STATUS_PENDING,
                           IotMqtt_Publish( pMqttConnection,
                                            &publishInfo,
                                            IOT_MQTT_FLAG_WAITABLE,
                                            NULL,
                                            &publishRef ) );

        /* Since _dupChecker doesn't actually transmit a PUBLISH, no PUBACK is
         * expected. */
        TEST_ASSERT_EQUAL( IOT_MQTT_RETRY_NO_RESPONSE,
                           IotMqtt_Wait( publishRef, _DUP_CHECK_TIMEOUT ) );

        /* Check the result of the DUP check. */
        TEST_ASSERT_EQUAL_INT( true, dupCheckResult );

        /* Check that at least the minimum wait time elapsed. */
        TEST_ASSERT_TRUE( startTime + _DUP_CHECK_MINIMUM_WAIT <= IotClock_GetTimeMs() );
    }

    /* Clean up MQTT connection. */
    IotMqtt_Disconnect( pMqttConnection, true );

    /* Check that the set DUP override was called. */
    if( TEST_PROTECT() )
    {
        TEST_ASSERT_EQUAL_INT( true, _publishSetDupCalled );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_subscribe and
 * @ref mqtt_function_unsubscribe with various invalid parameters.
 */
TEST( MQTT_Unit_API, SubscribeUnsubscribeParameters )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    _mqttConnection_t mqttConnection = { 0 };
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttReference_t reference = IOT_MQTT_REFERENCE_INITIALIZER;

    /* Check that subscription info is validated. */
    status = IotMqtt_Subscribe( &mqttConnection,
                                &subscription,
                                1,
                                IOT_MQTT_FLAG_WAITABLE,
                                NULL,
                                &reference );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

    status = IotMqtt_Unsubscribe( &mqttConnection,
                                  &subscription,
                                  1,
                                  IOT_MQTT_FLAG_WAITABLE,
                                  NULL,
                                  &reference );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

    subscription.pTopicFilter = _TEST_TOPIC_NAME;
    subscription.topicFilterLength = _TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = _SUBSCRIPTION_CALLBACK;

    /* A reference must be provided for a waitable SUBSCRIBE. */
    status = IotMqtt_Subscribe( &mqttConnection,
                                &subscription,
                                1,
                                IOT_MQTT_FLAG_WAITABLE,
                                NULL,
                                NULL );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

    status = IotMqtt_Unsubscribe( &mqttConnection,
                                  &subscription,
                                  1,
                                  IOT_MQTT_FLAG_WAITABLE,
                                  NULL,
                                  NULL );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_subscribe when memory allocation
 * fails at various points.
 */
TEST( MQTT_Unit_API, SubscribeMallocFail )
{
    int32_t i = 0;
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
    _mqttConnection_t * pMqttConnection = NULL;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttReference_t subscribeRef = IOT_MQTT_REFERENCE_INITIALIZER;

    /* Set the members of the network interface and create a new MQTT connection. */
    networkInterface.send = _sendSuccess;
    pMqttConnection = IotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                        &networkInterface,
                                                        0 );
    TEST_ASSERT_NOT_NULL( pMqttConnection );

    /* Set the necessary members of the subscription. */
    subscription.pTopicFilter = _TEST_TOPIC_NAME;
    subscription.topicFilterLength = _TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = _SUBSCRIPTION_CALLBACK;

    if( TEST_PROTECT() )
    {
        for( i = 0; ; i++ )
        {
            UnityMalloc_MakeMallocFailAfterCount( i );

            /* Call SUBSCRIBE. Memory allocation will fail at various times during
             * this call. */
            status = IotMqtt_Subscribe( pMqttConnection,
                                        &subscription,
                                        1,
                                        IOT_MQTT_FLAG_WAITABLE,
                                        NULL,
                                        &subscribeRef );

            /* If the SUBSCRIBE succeeded, the loop can exit after waiting for
             * the SUBSCRIBE to be cleaned up. */
            if( status == IOT_MQTT_STATUS_PENDING )
            {
                TEST_ASSERT_EQUAL( IOT_MQTT_TIMEOUT, IotMqtt_Wait( subscribeRef, _TIMEOUT_MS ) );
                break;
            }

            /* If the return value isn't success, check that it is memory allocation
             * failure. */
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }

        /* No lingering subscriptions should be in the MQTT connection. */
        TEST_ASSERT_EQUAL_INT( true, IotListDouble_IsEmpty( &( pMqttConnection->subscriptionList ) ) );
    }

    IotMqtt_Disconnect( pMqttConnection, true );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_unsubscribe when memory
 * allocation fails at various points.
 */
TEST( MQTT_Unit_API, UnsubscribeMallocFail )
{
    int32_t i = 0;
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
    _mqttConnection_t * pMqttConnection = NULL;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttReference_t unsubscribeRef = IOT_MQTT_REFERENCE_INITIALIZER;

    /* Set the members of the network interface and create a new MQTT connection. */
    networkInterface.send = _sendSuccess;
    pMqttConnection = IotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                        &networkInterface,
                                                        0 );
    TEST_ASSERT_NOT_NULL( pMqttConnection );

    /* Set the necessary members of the subscription. */
    subscription.pTopicFilter = _TEST_TOPIC_NAME;
    subscription.topicFilterLength = _TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = _SUBSCRIPTION_CALLBACK;

    if( TEST_PROTECT() )
    {
        for( i = 0; ; i++ )
        {
            UnityMalloc_MakeMallocFailAfterCount( i );

            /* Call UNSUBSCRIBE. Memory allocation will fail at various times during
             * this call. */
            status = IotMqtt_Unsubscribe( pMqttConnection,
                                          &subscription,
                                          1,
                                          IOT_MQTT_FLAG_WAITABLE,
                                          NULL,
                                          &unsubscribeRef );

            /* If the UNSUBSCRIBE succeeded, the loop can exit after waiting for
             * the UNSUBSCRIBE to be cleaned up. */
            if( status == IOT_MQTT_STATUS_PENDING )
            {
                TEST_ASSERT_EQUAL( IOT_MQTT_TIMEOUT, IotMqtt_Wait( unsubscribeRef, _TIMEOUT_MS ) );
                break;
            }

            /* If the return value isn't success, check that it is memory allocation
             * failure. */
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }

        /* No lingering subscriptions should be in the MQTT connection. */
        TEST_ASSERT_EQUAL_INT( true, IotListDouble_IsEmpty( &( pMqttConnection->subscriptionList ) ) );
    }

    IotMqtt_Disconnect( pMqttConnection, true );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests keep-alive handling and ensures that it is periodic.
 */
TEST( MQTT_Unit_API, KeepAlivePeriodic )
{
    int32_t disconnectCount = 0;
    _mqttConnection_t * pMqttConnection = NULL;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;

    /* An estimate for the amount of time this test requires. */
    const unsigned sleepTime = ( ( _KEEP_ALIVE_COUNT * _SHORT_KEEP_ALIVE_MS ) / 1000 ) +
                               ( ( ( _KEEP_ALIVE_COUNT * _SHORT_KEEP_ALIVE_MS ) % 1000 ) != 0 ) +
                               ( IOT_MQTT_RESPONSE_WAIT_MS * _KEEP_ALIVE_COUNT ) / 1000 + 2;

    /* Print a newline so this test may log its status. */
    UNITY_PRINT_EOL();

    /* Set the members of the network interface and create a new MQTT connection
     * with keep-alive. */
    networkInterface.send = _sendPingreq;
    networkInterface.pDisconnectContext = &disconnectCount;
    networkInterface.disconnect = _disconnect;
    pMqttConnection = IotTestMqtt_createMqttConnection( false,
                                                        &networkInterface,
                                                        1 );
    TEST_ASSERT_NOT_NULL( pMqttConnection );

    /* Connection send context can only be set after connection is created. */
    pMqttConnection->network.pSendContext = pMqttConnection;

    /* Set a short keep-alive interval so this test runs faster. */
    pMqttConnection->keepAliveMs = _SHORT_KEEP_ALIVE_MS;
    pMqttConnection->nextKeepAliveMs = _SHORT_KEEP_ALIVE_MS;

    /* Schedule the initial PINGREQ. */
    TEST_ASSERT_EQUAL( IOT_TASKPOOL_SUCCESS,
                       IotTaskPool_ScheduleDeferred( &( _IotMqttTaskPool ),
                                                     &( pMqttConnection->keepAliveJob ),
                                                     pMqttConnection->nextKeepAliveMs ) );

    /* Sleep to allow ample time for periodic PINGREQ sends and PINGRESP responses. */
    sleep( sleepTime );

    /* Disconnect the connection. */
    IotMqtt_Disconnect( pMqttConnection, true );

    /* Check the counters for PINGREQ send and disconnect. */
    TEST_ASSERT_EQUAL_INT32( _KEEP_ALIVE_COUNT + 1, _pingreqSendCount );
    TEST_ASSERT_EQUAL_INT32( 2, disconnectCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests that the keep-alive job cleans up the MQTT connection after a call
 * to @ref mqtt_function_disconnect.
 */
TEST( MQTT_Unit_API, KeepAliveJobCleanup )
{
    _mqttConnection_t * pMqttConnection = NULL;
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
    IotSemaphore_t waitSem;

    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &waitSem, 0, 1 ) );

    if( TEST_PROTECT() )
    {
        /* Set the members of the network interface and create a new MQTT connection
         * with keep-alive. */
        networkInterface.pSendContext = &waitSem;
        networkInterface.send = _sendSuccess;
        pMqttConnection = IotTestMqtt_createMqttConnection( false,
                                                            &networkInterface,
                                                            1 );
        TEST_ASSERT_NOT_NULL( pMqttConnection );

        /* Set a short keep-alive interval so this test runs faster. */
        pMqttConnection->keepAliveMs = _SHORT_KEEP_ALIVE_MS;

        /* Schedule the initial PINGREQ. */
        TEST_ASSERT_EQUAL( IOT_TASKPOOL_SUCCESS,
                           IotTaskPool_ScheduleDeferred( &( _IotMqttTaskPool ),
                                                         &( pMqttConnection->keepAliveJob ),
                                                         _SHORT_KEEP_ALIVE_MS ) );

        /* Wait for the keep-alive job to send a PINGREQ. */
        IotSemaphore_Wait( &waitSem );

        /* Immediately disconnect the connection. */
        IotMqtt_Disconnect( pMqttConnection, true );
    }

    IotSemaphore_Destroy( &waitSem );
}

/*-----------------------------------------------------------*/
