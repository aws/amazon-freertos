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
 * @file iot_tests_mqtt_subscription.c
 * @brief Tests for the functions in iot_mqtt_subscription.c
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* Common include. */
#include "iot_common.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"
#include "platform/iot_clock.h"

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/* Test framework includes. */
#include "unity_fixture.h"

/* MQTT test access include. */
#include "iot_test_access_mqtt.h"

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Including stdio.h also brings in unwanted (and conflicting) symbols on some
 * platforms. Therefore, any functions in stdio.h needed in this file have an
 * extern declaration here. */
extern int snprintf( char *,
                     size_t,
                     const char *,
                     ... );
/** @endcond */

/*-----------------------------------------------------------*/

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values of test configuration constants.
 */
#ifndef IOT_TEST_MQTT_TIMEOUT_MS
    #define IOT_TEST_MQTT_TIMEOUT_MS    ( 5000 )
#endif
/** @endcond */

/**
 * @brief Determine which MQTT server mode to test (AWS IoT or Mosquitto).
 */
#if !defined( IOT_TEST_MQTT_MOSQUITTO ) || IOT_TEST_MQTT_MOSQUITTO == 0
    #define _AWS_IOT_MQTT_SERVER    true
#else
    #define _AWS_IOT_MQTT_SERVER    false
#endif

/*
 * Constants relating to the test subscription list.
 */
#define _LIST_ITEM_COUNT             ( 10 )                                      /**< @brief Number of subscriptions. */
#define _TEST_TOPIC_FILTER_FORMAT    ( "/test%lu" )                              /**< @brief Format of each topic filter. */
#define _TEST_TOPIC_FILTER_LENGTH    ( sizeof( _TEST_TOPIC_FILTER_FORMAT ) + 1 ) /**< @brief Maximum length of each topic filter. */

/*
 * Constants relating to the multithreaded subscription test.
 */
#define _MT_THREAD_COUNT             ( 8 )         /**< @brief Number of threads. */
#define _MT_TOPIC_FILTER_FORMAT      ( "/%p-%lu" ) /**< @brief Format of each topic filter. */
#define _MT_TOPIC_FILTER_LENGTH      ( 16 )        /**< @brief Maximum length of each topic filter. */

/**
 * @brief A non-NULL function pointer to use for subscription callback. This
 * "function" should cause a crash if actually called.
 */
#define _CALLBACK_FUNCTION  \
    ( ( void ( * )( void *, \
                    IotMqttCallbackParam_t * ) ) 0x1 )

/**
 * @brief All test topic filters in the tests #TEST_MQTT_Unit_Subscription_TopicFilterMatchTrue_
 * and #TEST_MQTT_Unit_Subscription_TopicFilterMatchFalse_ should be shorter than this
 * length.
 */
#define _TOPIC_FILTER_MATCH_MAX_LENGTH    ( 32 )

/**
 * @brief Macro to check a single topic name against a topic filter.
 *
 * @param[in] topicNameString The topic name to check.
 * @param[in] topicFilterString The topic filter to check.
 * @param[in] exactMatch Whether an exact match is required. If this is false,
 * wildcard matching is allowed.
 * @param[in] expectedResult Whether the topic name and filter are expected to match.
 *
 * @note This macro may only be used when a #_mqttSubscription_t pointer named pTopicFilter
 * is in scope.
 */
#define _TEST_TOPIC_MATCH( topicNameString, topicFilterString, exactMatch, expectedResult )             \
    {                                                                                                   \
        _topicMatchParams_t _topicMatchParams = { 0 };                                                  \
        _topicMatchParams.pTopicName = topicNameString;                                                 \
        _topicMatchParams.topicNameLength = ( uint16_t ) strlen( _topicMatchParams.pTopicName );        \
        _topicMatchParams.exactMatchOnly = exactMatch;                                                  \
                                                                                                        \
        pTopicFilter->topicFilterLength = ( uint16_t ) snprintf( pTopicFilter->pTopicFilter,            \
                                                                 _TOPIC_FILTER_MATCH_MAX_LENGTH,        \
                                                                 topicFilterString );                   \
                                                                                                        \
        TEST_ASSERT_EQUAL_INT( expectedResult,                                                          \
                               IotTestMqtt_topicMatch( &( pTopicFilter->link ), &_topicMatchParams ) ); \
    }

/*-----------------------------------------------------------*/

/**
 * @brief Tracks whether the global MQTT connection has been created.
 */
static bool _connectionCreated = false;

/**
 * @brief The MQTT connection shared by all tests.
 */
static _mqttConnection_t * _pMqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

/**
 * @brief Synchronizes threads in the multithreaded test at start.
 */
static IotSemaphore_t _mtTestStart;

/**
 * @brief Synchronizes threads in the multithreaded test at exit.
 */
static IotSemaphore_t _mtTestExit;

/*-----------------------------------------------------------*/

/**
 * @brief Places dummy subscriptions in the subscription list of #_pMqttConnection.
 */
static void _populateList( void )
{
    size_t i = 0;
    _mqttSubscription_t * pSubscription = NULL;

    for( i = 0; i < _LIST_ITEM_COUNT; i++ )
    {
        pSubscription = IotMqtt_MallocSubscription( sizeof( _mqttSubscription_t ) + _TEST_TOPIC_FILTER_LENGTH );
        TEST_ASSERT_NOT_NULL( pSubscription );

        ( void ) memset( pSubscription, 0x00, sizeof( _mqttSubscription_t ) + _TEST_TOPIC_FILTER_LENGTH );
        pSubscription->packetInfo.identifier = 1;
        pSubscription->packetInfo.order = i;
        pSubscription->callback.function = _CALLBACK_FUNCTION;
        pSubscription->topicFilterLength = ( uint16_t ) snprintf( pSubscription->pTopicFilter,
                                                                  _TEST_TOPIC_FILTER_LENGTH,
                                                                  _TEST_TOPIC_FILTER_FORMAT,
                                                                  ( unsigned long ) i );

        IotListDouble_InsertHead( &( _pMqttConnection->subscriptionList ),
                                  &( pSubscription->link ) );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Wait for a reference count to reach a target value, subject to a timeout.
 */
static bool _waitForCount( IotMutex_t * pMutex,
                           const int32_t * pReferenceCount,
                           int32_t target )
{
    bool status = false;
    int32_t referenceCount = 0;
    uint32_t sleepCount = 0;

    /* Calculate limit on the number of times to sleep for 100 ms. */
    const uint32_t sleepLimit = ( IOT_TEST_MQTT_TIMEOUT_MS / 100 ) +
                                ( ( IOT_TEST_MQTT_TIMEOUT_MS % 100 ) != 0 );

    /* Wait for the reference count to reach the target value. */
    for( sleepCount = 0; sleepCount < sleepLimit; sleepCount++ )
    {
        /* Read reference count. */
        IotMutex_Lock( pMutex );
        referenceCount = *pReferenceCount;
        IotMutex_Unlock( pMutex );

        /* Exit if target value is reached. Otherwise, sleep. */
        if( referenceCount == target )
        {
            status = true;
            break;
        }
        else
        {
            IotClock_SleepMs( 100 );
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

/**
 * @brief A subscription callback function that only reports whether it was invoked.
 */
static void _publishCallback( void * pArgument,
                              IotMqttCallbackParam_t * pPublish )
{
    uint16_t i = 0;
    bool * pCallbackInvoked = ( bool * ) pArgument;

    /* Notify the caller that this callback was invoked. */
    *pCallbackInvoked = true;

    /* If the topic filter doesn't match the topic name, ensure that the topic
     * filter contains a wildcard. */
    if( pPublish->message.topicFilterLength != pPublish->message.info.topicNameLength )
    {
        for( i = 0; i < pPublish->message.topicFilterLength; i++ )
        {
            if( ( pPublish->message.pTopicFilter[ i ] == '+' ) ||
                ( pPublish->message.pTopicFilter[ i ] == '#' ) )
            {
                break;
            }
        }

        TEST_ASSERT_LESS_THAN_UINT16( pPublish->message.topicFilterLength, i );
    }

    /* Ensure that the MQTT connection was set correctly. */
    TEST_ASSERT_EQUAL_PTR( pPublish->mqttConnection, _pMqttConnection );

    /* Ensure that publish info is valid. */
    TEST_ASSERT_EQUAL_INT( true,
                           _IotMqtt_ValidatePublish( _AWS_IOT_MQTT_SERVER,
                                                     &( pPublish->message.info ) ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief A subscription callback function that blocks on a semaphore until signaled.
 */
static void _blockingCallback( void * pArgument,
                               IotMqttCallbackParam_t * pPublish )
{
    IotSemaphore_t * pSemaphore = ( IotSemaphore_t * ) pArgument;

    /* Silence warnings about unused parameters. */
    ( void ) pPublish;

    /* Wait until signaled. */
    IotSemaphore_Wait( pSemaphore );
}

/*-----------------------------------------------------------*/

/**
 * @brief Thread routing of the multithreaded test.
 */
static void _multithreadTestThread( void * pArgument )
{
    size_t i = 0;
    int barrierResult = 0;
    bool * pThreadResult = ( bool * ) pArgument;
    char pTopicFilters[ _LIST_ITEM_COUNT ][ _MT_TOPIC_FILTER_LENGTH ] = { { 0 } };
    IotMqttSubscription_t subscription[ _LIST_ITEM_COUNT ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    /* Synchronize with the other threads before starting the test. */
    IotSemaphore_Wait( &( _mtTestStart ) );

    /* Add items to the subscription list. */
    for( i = 0; i < _LIST_ITEM_COUNT; i++ )
    {
        subscription[ i ].callback.function = _CALLBACK_FUNCTION;
        subscription[ i ].pTopicFilter = pTopicFilters[ i ];
        subscription[ i ].topicFilterLength = ( uint16_t ) snprintf( pTopicFilters[ i ],
                                                                     _MT_TOPIC_FILTER_LENGTH,
                                                                     _MT_TOPIC_FILTER_FORMAT,
                                                                     &i,
                                                                     ( unsigned long ) i );

        if( _IotMqtt_AddSubscriptions( _pMqttConnection,
                                       1,
                                       &( subscription[ i ] ),
                                       1 ) != IOT_MQTT_SUCCESS )
        {
            *pThreadResult = false;

            return;
        }
    }

    /* Remove the previously added items from the list. */
    for( i = 0; i < _LIST_ITEM_COUNT; i++ )
    {
        _IotMqtt_RemoveSubscriptionByTopicFilter( _pMqttConnection,
                                                  &( subscription[ i ] ),
                                                  1 );
    }

    *pThreadResult = true;

    /* Synchronize with the other threads before exiting the test. */
    IotSemaphore_Post( &( _mtTestExit ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group for MQTT subscription tests.
 */
TEST_GROUP( MQTT_Unit_Subscription );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for MQTT subscription tests.
 */
TEST_SETUP( MQTT_Unit_Subscription )
{
    static IotNetworkInterface_t networkInterface = { 0 };
    IotMqttNetworkInfo_t networkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;

    /* Initialize common components. */
    TEST_ASSERT_EQUAL_INT( true, IotCommon_Init() );

    networkInfo.pNetworkInterface = &networkInterface;

    /* Initialize the MQTT library. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, IotMqtt_Init() );

    /* Create an MQTT connection with empty network info. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                         &networkInfo,
                                                         0 );
    TEST_ASSERT_NOT_NULL( _pMqttConnection );

    _connectionCreated = true;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for MQTT subscription tests.
 */
TEST_TEAR_DOWN( MQTT_Unit_Subscription )
{
    /* Destroy the MQTT connection used for the tests. */
    if( _connectionCreated == true )
    {
        IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );
        _connectionCreated = false;
    }

    /* Clean up libraries. */
    IotCommon_Cleanup();
    IotMqtt_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for MQTT subscription tests.
 */
TEST_GROUP_RUNNER( MQTT_Unit_Subscription )
{
    RUN_TEST_CASE( MQTT_Unit_Subscription, ListInsertRemove );
    RUN_TEST_CASE( MQTT_Unit_Subscription, ListFindByTopicFilter );
    RUN_TEST_CASE( MQTT_Unit_Subscription, ListFindByPacket );
    RUN_TEST_CASE( MQTT_Unit_Subscription, SubscriptionRemoveByPacket );
    RUN_TEST_CASE( MQTT_Unit_Subscription, SubscriptionRemoveByTopicFilter );
    RUN_TEST_CASE( MQTT_Unit_Subscription, SubscriptionAddDuplicate );
    RUN_TEST_CASE( MQTT_Unit_Subscription, SubscriptionAddMallocFail );
    RUN_TEST_CASE( MQTT_Unit_Subscription, SubscriptionMultithreaded );
    RUN_TEST_CASE( MQTT_Unit_Subscription, ProcessPublish );
    RUN_TEST_CASE( MQTT_Unit_Subscription, ProcessPublishMultiple );
    RUN_TEST_CASE( MQTT_Unit_Subscription, SubscriptionReferences );
    RUN_TEST_CASE( MQTT_Unit_Subscription, TopicFilterMatchTrue );
    RUN_TEST_CASE( MQTT_Unit_Subscription, TopicFilterMatchFalse );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests simple insertion and removal of elements from the subscription list.
 */
TEST( MQTT_Unit_Subscription, ListInsertRemove )
{
    _mqttSubscription_t node1;
    _mqttSubscription_t node2;
    _mqttSubscription_t node3;

    ( void ) memset( &node1, 0x00, sizeof( _mqttSubscription_t ) );
    ( void ) memset( &node2, 0x00, sizeof( _mqttSubscription_t ) );
    ( void ) memset( &node3, 0x00, sizeof( _mqttSubscription_t ) );

    IotListDouble_InsertHead( &( _pMqttConnection->subscriptionList ),
                              &( node1.link ) );
    IotListDouble_InsertHead( &( _pMqttConnection->subscriptionList ),
                              &( node2.link ) );
    IotListDouble_InsertHead( &( _pMqttConnection->subscriptionList ),
                              &( node3.link ) );

    IotListDouble_Remove( &( node1.link ) );
    IotListDouble_Remove( &( node2.link ) );
    IotListDouble_Remove( &( node3.link ) );

    TEST_ASSERT_EQUAL_INT( true, IotListDouble_IsEmpty( &( _pMqttConnection->subscriptionList ) ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests searching the subscription list using a topic filter.
 */
TEST( MQTT_Unit_Subscription, ListFindByTopicFilter )
{
    _mqttSubscription_t * pSubscription = NULL;
    IotLink_t * pSubscriptionLink = NULL;
    _topicMatchParams_t topicMatchParams = { 0 };

    topicMatchParams.pTopicName = "/test0";
    topicMatchParams.topicNameLength = 6;

    /* On empty list. */
    pSubscriptionLink = IotListDouble_FindFirstMatch( &( _pMqttConnection->subscriptionList ),
                                                      NULL,
                                                      IotTestMqtt_topicMatch,
                                                      &topicMatchParams );
    TEST_ASSERT_EQUAL_PTR( NULL, pSubscriptionLink );

    _populateList();

    /* Topic filter present. */
    pSubscriptionLink = IotListDouble_FindFirstMatch( &( _pMqttConnection->subscriptionList ),
                                                      NULL,
                                                      IotTestMqtt_topicMatch,
                                                      &topicMatchParams );
    TEST_ASSERT_NOT_EQUAL( NULL, pSubscriptionLink );
    pSubscription = IotLink_Container( _mqttSubscription_t, pSubscriptionLink, link );
    TEST_ASSERT_NOT_EQUAL( NULL, pSubscription );

    /* Topic filter not present. */
    topicMatchParams.pTopicName = "/notpresent";
    topicMatchParams.topicNameLength = 11;
    pSubscriptionLink = IotListDouble_FindFirstMatch( &( _pMqttConnection->subscriptionList ),
                                                      NULL,
                                                      IotTestMqtt_topicMatch,
                                                      &topicMatchParams );
    TEST_ASSERT_EQUAL_PTR( NULL, pSubscriptionLink );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests searching the subscription list using a packet identifier.
 */
TEST( MQTT_Unit_Subscription, ListFindByPacket )
{
    _mqttSubscription_t * pSubscription = NULL;
    IotLink_t * pSubscriptionLink = NULL;
    _packetMatchParams_t packetMatchParams = { 0 };

    packetMatchParams.packetIdentifier = 1;
    packetMatchParams.order = 0;

    /* On empty list. */
    pSubscriptionLink = IotListDouble_FindFirstMatch( &( _pMqttConnection->subscriptionList ),
                                                      NULL,
                                                      IotTestMqtt_packetMatch,
                                                      &packetMatchParams );
    TEST_ASSERT_EQUAL_PTR( NULL, pSubscriptionLink );

    _populateList();

    /* Packet and order present. */
    pSubscriptionLink = IotListDouble_FindFirstMatch( &( _pMqttConnection->subscriptionList ),
                                                      NULL,
                                                      IotTestMqtt_packetMatch,
                                                      &packetMatchParams );
    TEST_ASSERT_NOT_EQUAL( NULL, pSubscriptionLink );
    pSubscription = IotLink_Container( _mqttSubscription_t, pSubscriptionLink, link );
    TEST_ASSERT_NOT_EQUAL( NULL, pSubscription );

    /* Packet present, order not present. */
    packetMatchParams.order = _LIST_ITEM_COUNT;
    pSubscriptionLink = IotListDouble_FindFirstMatch( &( _pMqttConnection->subscriptionList ),
                                                      NULL,
                                                      IotTestMqtt_packetMatch,
                                                      &packetMatchParams );
    TEST_ASSERT_EQUAL_PTR( NULL, pSubscriptionLink );

    /* Packet not present, order present. */
    packetMatchParams.packetIdentifier = 0;
    packetMatchParams.order = 0;
    pSubscriptionLink = IotListDouble_FindFirstMatch( &( _pMqttConnection->subscriptionList ),
                                                      NULL,
                                                      IotTestMqtt_packetMatch,
                                                      &packetMatchParams );
    TEST_ASSERT_EQUAL_PTR( NULL, pSubscriptionLink );

    /* Packet and order not present. */
    packetMatchParams.packetIdentifier = 0;
    packetMatchParams.order = _LIST_ITEM_COUNT;
    pSubscriptionLink = IotListDouble_FindFirstMatch( &( _pMqttConnection->subscriptionList ),
                                                      NULL,
                                                      IotTestMqtt_packetMatch,
                                                      &packetMatchParams );
    TEST_ASSERT_EQUAL_PTR( NULL, pSubscriptionLink );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests removing subscriptions by packet identifier.
 */
TEST( MQTT_Unit_Subscription, SubscriptionRemoveByPacket )
{
    long i = 0;

    /* On empty list (should not crash). */
    _IotMqtt_RemoveSubscriptionByPacket( _pMqttConnection,
                                         1,
                                         0 );

    _populateList();

    /* Remove all subscriptions by packet one-by-one. */
    for( i = 0; i < _LIST_ITEM_COUNT; i++ )
    {
        _IotMqtt_RemoveSubscriptionByPacket( _pMqttConnection,
                                             1,
                                             i );
    }

    /* List should be empty. */
    TEST_ASSERT_EQUAL_INT( true, IotListDouble_IsEmpty( &( _pMqttConnection->subscriptionList ) ) );

    /* Remove all subscriptions for a packet one-shot. */
    _populateList();
    _IotMqtt_RemoveSubscriptionByPacket( _pMqttConnection,
                                         1,
                                         -1 );
    TEST_ASSERT_EQUAL_INT( true, IotListDouble_IsEmpty( &( _pMqttConnection->subscriptionList ) ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests removing subscriptions by a topic filter.
 */
TEST( MQTT_Unit_Subscription, SubscriptionRemoveByTopicFilter )
{
    size_t i = 0;
    char pTopicFilters[ _LIST_ITEM_COUNT ][ _TEST_TOPIC_FILTER_LENGTH ] = { { 0 } };
    IotMqttSubscription_t subscription[ _LIST_ITEM_COUNT ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    /* On empty list (should not crash). */
    subscription[ 0 ].pTopicFilter = "/topic";
    subscription[ 0 ].topicFilterLength = 6;
    _IotMqtt_RemoveSubscriptionByTopicFilter( _pMqttConnection,
                                              &( subscription[ 0 ] ),
                                              1 );

    _populateList();
    subscription[ 0 ].pTopicFilter = pTopicFilters[ 0 ];

    /* Removal one-by-one. */
    for( i = 0; i < _LIST_ITEM_COUNT; i++ )
    {
        subscription[ 0 ].topicFilterLength = ( uint16_t ) snprintf( pTopicFilters[ 0 ],
                                                                     _TEST_TOPIC_FILTER_LENGTH,
                                                                     _TEST_TOPIC_FILTER_FORMAT,
                                                                     ( unsigned long ) i );

        _IotMqtt_RemoveSubscriptionByTopicFilter( _pMqttConnection,
                                                  &( subscription[ 0 ] ),
                                                  1 );
    }

    /* List should be empty. */
    TEST_ASSERT_EQUAL_INT( true, IotListDouble_IsEmpty( &( _pMqttConnection->subscriptionList ) ) );

    /* Refill the list. */
    _populateList();
    TEST_ASSERT_EQUAL_INT( false, IotListDouble_IsEmpty( &( _pMqttConnection->subscriptionList ) ) );

    /* Removal all at once. */
    for( i = 0; i < _LIST_ITEM_COUNT; i++ )
    {
        subscription[ i ].pTopicFilter = pTopicFilters[ i ];
        subscription[ i ].topicFilterLength = ( uint16_t ) snprintf( pTopicFilters[ i ],
                                                                     _TEST_TOPIC_FILTER_LENGTH,
                                                                     _TEST_TOPIC_FILTER_FORMAT,
                                                                     ( unsigned long ) i );
    }

    _IotMqtt_RemoveSubscriptionByTopicFilter( _pMqttConnection,
                                              subscription,
                                              _LIST_ITEM_COUNT );

    /* List should be empty. */
    TEST_ASSERT_EQUAL_INT( true, IotListDouble_IsEmpty( &( _pMqttConnection->subscriptionList ) ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests adding duplicate subscriptions.
 */
TEST( MQTT_Unit_Subscription, SubscriptionAddDuplicate )
{
    size_t i = 0;
    _mqttSubscription_t * pSubscription = NULL;
    IotLink_t * pSubscriptionLink = NULL;
    _topicMatchParams_t topicMatchParams = { 0 };
    char pTopicFilters[ _LIST_ITEM_COUNT ][ _TEST_TOPIC_FILTER_LENGTH ] = { { 0 } };
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttSubscription_t subscription[ _LIST_ITEM_COUNT ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    /* Set valid values in the subscription list. */
    for( i = 0; i < _LIST_ITEM_COUNT; i++ )
    {
        subscription[ i ].callback.function = _CALLBACK_FUNCTION;
        subscription[ i ].pTopicFilter = pTopicFilters[ i ];
        subscription[ i ].topicFilterLength = ( uint16_t ) snprintf( pTopicFilters[ i ],
                                                                     _TEST_TOPIC_FILTER_LENGTH,
                                                                     _TEST_TOPIC_FILTER_FORMAT,
                                                                     ( unsigned long ) i );
    }

    /* Add all subscriptions to the list. */
    status = _IotMqtt_AddSubscriptions( _pMqttConnection,
                                        1,
                                        subscription,
                                        _LIST_ITEM_COUNT );
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

    /* Change the callback information, but not the topic filter. */
    subscription[ 1 ].callback.function = _publishCallback;
    subscription[ 1 ].callback.pCallbackContext = _pMqttConnection;

    /* Add the duplicate subscription. */
    status = _IotMqtt_AddSubscriptions( _pMqttConnection,
                                        3,
                                        &( subscription[ 1 ] ),
                                        1 );
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

    /* Find the subscription that was just modified. */
    topicMatchParams.pTopicName = "/test1";
    topicMatchParams.topicNameLength = 6;
    topicMatchParams.exactMatchOnly = true;
    pSubscriptionLink = IotListDouble_FindFirstMatch( &( _pMqttConnection->subscriptionList ),
                                                      NULL,
                                                      IotTestMqtt_topicMatch,
                                                      &topicMatchParams );
    TEST_ASSERT_NOT_EQUAL( NULL, pSubscriptionLink );
    pSubscription = IotLink_Container( _mqttSubscription_t, pSubscriptionLink, link );
    TEST_ASSERT_NOT_EQUAL( NULL, pSubscription );

    /* Check that the information was changed. */
    TEST_ASSERT_EQUAL_UINT16( 3, pSubscription->packetInfo.identifier );
    TEST_ASSERT_EQUAL( 0, pSubscription->packetInfo.order );
    TEST_ASSERT_EQUAL_PTR( _publishCallback, pSubscription->callback.function );
    TEST_ASSERT_EQUAL_PTR( _pMqttConnection, pSubscription->callback.pCallbackContext );

    /* Check that a duplicate entry wasn't created. */
    IotListDouble_Remove( &( pSubscription->link ) );
    IotMqtt_FreeSubscription( pSubscription );
    pSubscriptionLink = IotListDouble_FindFirstMatch( &( _pMqttConnection->subscriptionList ),
                                                      NULL,
                                                      IotTestMqtt_topicMatch,
                                                      &topicMatchParams );
    TEST_ASSERT_EQUAL_PTR( NULL, pSubscriptionLink );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests adding subscriptions when memory allocation fails at various points.
 */
TEST( MQTT_Unit_Subscription, SubscriptionAddMallocFail )
{
    size_t i = 0;
    char pTopicFilters[ _LIST_ITEM_COUNT ][ _TEST_TOPIC_FILTER_LENGTH ] = { { 0 } };
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttSubscription_t subscription[ _LIST_ITEM_COUNT ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    /* Set valid values in the subscription list. */
    for( i = 0; i < _LIST_ITEM_COUNT; i++ )
    {
        subscription[ i ].callback.function = _CALLBACK_FUNCTION;
        subscription[ i ].pTopicFilter = pTopicFilters[ i ];
        subscription[ i ].topicFilterLength = ( uint16_t ) snprintf( pTopicFilters[ i ],
                                                                     _TEST_TOPIC_FILTER_LENGTH,
                                                                     _TEST_TOPIC_FILTER_FORMAT,
                                                                     ( unsigned long ) i );
    }

    /* Set malloc to fail at various points. */
    for( i = 0; i < _LIST_ITEM_COUNT; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( ( int ) i );

        status = _IotMqtt_AddSubscriptions( _pMqttConnection,
                                            1,
                                            subscription,
                                            _LIST_ITEM_COUNT );

        if( status == IOT_MQTT_SUCCESS )
        {
            break;
        }

        TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        TEST_ASSERT_EQUAL_INT( true, IotListDouble_IsEmpty( &( _pMqttConnection->subscriptionList ) ) );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests adding and removing subscriptions in a multithreaded environment.
 */
TEST( MQTT_Unit_Subscription, SubscriptionMultithreaded )
{
    int32_t i = 0, threadsCreated = 0, threadsExited = 0;
    bool threadResults[ _MT_THREAD_COUNT ] = { 0 };

    /* Create the synchronization semaphores. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( _mtTestStart ), 0, _MT_THREAD_COUNT ) );
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( _mtTestExit ), 0, _MT_THREAD_COUNT ) );

    /* Spawn threads for the test. */
    for( i = 0; i < _MT_THREAD_COUNT; i++ )
    {
        if( Iot_CreateDetachedThread( _multithreadTestThread,
                                      &( threadResults[ i ] ),
                                      IOT_THREAD_DEFAULT_PRIORITY,
                                      IOT_THREAD_DEFAULT_STACK_SIZE ) == false )
        {
            break;
        }
    }

    /* Record how many threads were created. */
    threadsCreated = i;

    /* Signal all created threads to start. */
    for( i = 0; i < threadsCreated; i++ )
    {
        IotSemaphore_Post( &_mtTestStart );
    }

    /* Wait for all created threads to finish. */
    for( i = 0; i < threadsCreated; i++ )
    {
        if( IotSemaphore_TimedWait( &_mtTestExit,
                                    IOT_TEST_MQTT_TIMEOUT_MS ) == false )
        {
            break;
        }

        threadsExited++;
    }

    if( TEST_PROTECT() )
    {
        /* Check how many threads ran. */
        TEST_ASSERT_EQUAL_INT( threadsCreated, threadsExited );
        TEST_ASSERT_EQUAL_INT( threadsCreated, _MT_THREAD_COUNT );

        /* Check the results of the test threads. */
        for( i = 0; i < _MT_THREAD_COUNT; i++ )
        {
            TEST_ASSERT_EQUAL_INT( true, threadResults[ i ] );
        }

        /* The subscription list should be empty. */
        TEST_ASSERT_EQUAL_INT( true, IotListDouble_IsEmpty( &( _pMqttConnection->subscriptionList ) ) );
    }

    /* Destroy the synchronization semaphores. */
    IotSemaphore_Destroy( &_mtTestStart );
    IotSemaphore_Destroy( &_mtTestExit );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests invoking subscription callbacks with PUBLISH messages.
 */
TEST( MQTT_Unit_Subscription, ProcessPublish )
{
    bool callbackInvoked = false;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttCallbackParam_t callbackParam = { .message = { 0 } };

    /* Set the subscription and corresponding publish info. */
    subscription.pTopicFilter = "/test";
    subscription.topicFilterLength = 5;
    subscription.callback.function = _publishCallback;
    subscription.callback.pCallbackContext = &callbackInvoked;

    callbackParam.message.info.pTopicName = "/test";
    callbackParam.message.info.topicNameLength = 5;
    callbackParam.message.info.pPayload = "";
    callbackParam.message.info.payloadLength = 0;

    /* Add the subscription. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS,
                       _IotMqtt_AddSubscriptions( _pMqttConnection,
                                                  1,
                                                  &subscription,
                                                  1 ) );

    /* Increment connection reference count for processing subscription callbacks. */
    TEST_ASSERT_EQUAL_INT( true, _IotMqtt_IncrementConnectionReferences( _pMqttConnection ) );

    /* Find the subscription and invoke its callback. */
    _IotMqtt_InvokeSubscriptionCallback( _pMqttConnection,
                                         &callbackParam );

    /* Check that the callback was invoked. */
    TEST_ASSERT_EQUAL_INT( true, callbackInvoked );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests that all matching subscription callbacks are invoked for a
 * PUBLISH.
 */
TEST( MQTT_Unit_Subscription, ProcessPublishMultiple )
{
    bool callbackInvoked[ 3 ] = { false };
    IotMqttSubscription_t subscription[ 3 ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER };
    IotMqttCallbackParam_t callbackParam = { .message = { 0 } };

    /* Set the subscription info. */
    subscription[ 0 ].pTopicFilter = "/test";
    subscription[ 0 ].topicFilterLength = 5;
    subscription[ 0 ].callback.function = _publishCallback;
    subscription[ 0 ].callback.pCallbackContext = &( callbackInvoked[ 0 ] );

    subscription[ 1 ].pTopicFilter = "/+";
    subscription[ 1 ].topicFilterLength = 2;
    subscription[ 1 ].callback.function = _publishCallback;
    subscription[ 1 ].callback.pCallbackContext = &( callbackInvoked[ 1 ] );

    subscription[ 2 ].pTopicFilter = "/#";
    subscription[ 2 ].topicFilterLength = 2;
    subscription[ 2 ].callback.function = _publishCallback;
    subscription[ 2 ].callback.pCallbackContext = &( callbackInvoked[ 2 ] );

    /* Create a PUBLISH that matches all 3 subscriptions. */
    callbackParam.message.info.pTopicName = "/test";
    callbackParam.message.info.topicNameLength = 5;
    callbackParam.message.info.pPayload = "";
    callbackParam.message.info.payloadLength = 0;

    /* Add the subscriptions. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS,
                       _IotMqtt_AddSubscriptions( _pMqttConnection,
                                                  1,
                                                  &( subscription[ 0 ] ),
                                                  3 ) );

    /* Increment connection reference count for processing subscription callbacks. */
    TEST_ASSERT_EQUAL_INT( true, _IotMqtt_IncrementConnectionReferences( _pMqttConnection ) );

    /* Invoke subscription callbacks. */
    _IotMqtt_InvokeSubscriptionCallback( _pMqttConnection,
                                         &callbackParam );

    /* Check that all 3 callbacks were invoked. */
    TEST_ASSERT_EQUAL_INT( true, callbackInvoked[ 0 ] );
    TEST_ASSERT_EQUAL_INT( true, callbackInvoked[ 1 ] );
    TEST_ASSERT_EQUAL_INT( true, callbackInvoked[ 2 ] );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests that subscriptions are properly reference counted.
 */
TEST( MQTT_Unit_Subscription, SubscriptionReferences )
{
    int32_t i = 0;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    _mqttOperation_t * pIncomingPublish[ 3 ] = { NULL };
    _mqttSubscription_t * pSubscription = NULL;
    IotLink_t * pSubscriptionLink;
    IotSemaphore_t waitSem;

    /* Adjustment to reference count based on keep-alive status. */
    const int32_t keepAliveReference = 1 + ( ( _pMqttConnection->keepAliveMs != 0 ) ? 1 : 0 );

    #if ( IOT_STATIC_MEMORY_ONLY == 1 ) && ( IOT_MQTT_MAX_IN_PROGRESS_OPERATIONS < 3 )
    #error "IOT_MQTT_MAX_IN_PROGRESS_OPERATIONS must be at least 3 for SubscriptionReferences test."
    #endif

    /* The MQTT task pool must support at least 3 threads for this test to run successfully. */
    TEST_ASSERT_EQUAL( IOT_TASKPOOL_SUCCESS, IotTaskPool_SetMaxThreads( IOT_SYSTEM_TASKPOOL, 4 ) );

    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &waitSem, 0, 3 ) );

    /* Set the subscription info. */
    subscription.pTopicFilter = "/test";
    subscription.topicFilterLength = 5;
    subscription.callback.function = _blockingCallback;
    subscription.callback.pCallbackContext = &waitSem;

    /* Add the subscriptions. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _IotMqtt_AddSubscriptions( _pMqttConnection,
                                                                    1,
                                                                    &subscription,
                                                                    1 ) );

    /* Get the pointer to the subscription in the MQTT connection. */
    pSubscriptionLink = IotListDouble_PeekHead( &( _pMqttConnection->subscriptionList ) );
    TEST_ASSERT_NOT_NULL( pSubscriptionLink );
    pSubscription = IotLink_Container( _mqttSubscription_t, pSubscriptionLink, link );
    TEST_ASSERT_NOT_NULL( pSubscription );

    /* Create 3 incoming PUBLISH messages that match the subscription. */
    for( i = 0; i < 3; i++ )
    {
        pIncomingPublish[ i ] = IotMqtt_MallocOperation( sizeof( _mqttOperation_t ) );
        TEST_ASSERT_NOT_NULL( pIncomingPublish );

        ( void ) memset( pIncomingPublish[ i ], 0x00, sizeof( _mqttOperation_t ) );
        pIncomingPublish[ i ]->incomingPublish = true;
        pIncomingPublish[ i ]->pMqttConnection = _pMqttConnection;
        pIncomingPublish[ i ]->publishInfo.pTopicName = "/test";
        pIncomingPublish[ i ]->publishInfo.topicNameLength = 5;
        pIncomingPublish[ i ]->publishInfo.pPayload = "";

        IotListDouble_InsertHead( &( _pMqttConnection->pendingProcessing ),
                                  &( pIncomingPublish[ i ]->link ) );
    }

    if( TEST_PROTECT() )
    {
        /* Schedule 3 callback invocations for the incoming PUBLISH. */
        for( i = 0; i < 3; i++ )
        {
            TEST_ASSERT_EQUAL_INT( true, _IotMqtt_IncrementConnectionReferences( _pMqttConnection ) );
            TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _IotMqtt_ScheduleOperation( pIncomingPublish[ i ],
                                                                             _IotMqtt_ProcessIncomingPublish,
                                                                             0 ) );
        }

        /* Wait for the connection reference count to reach 3 (adjusted for possible keep-alive). */
        TEST_ASSERT_EQUAL_INT( true, _waitForCount( &( _pMqttConnection->referencesMutex ),
                                                    &( _pMqttConnection->references ),
                                                    3 + keepAliveReference ) );

        /* Check that the subscription also has a reference count of 3. */
        TEST_ASSERT_EQUAL_INT32( true, _waitForCount( &( _pMqttConnection->subscriptionMutex ),
                                                      &( pSubscription->references ),
                                                      3 ) );

        /* Post to the wait semaphore, which unblocks one subscription callback. */
        IotSemaphore_Post( &waitSem );

        /* Wait for the connection reference count to decrease to 2 (adjusted for
         * possible keep-alive). Check that the subscription reference count also
         * decreases to 2. */
        TEST_ASSERT_EQUAL_INT( true, _waitForCount( &( _pMqttConnection->referencesMutex ),
                                                    &( _pMqttConnection->references ),
                                                    2 + keepAliveReference ) );
        TEST_ASSERT_EQUAL_INT32( true, _waitForCount( &( _pMqttConnection->subscriptionMutex ),
                                                      &( pSubscription->references ),
                                                      2 ) );

        /* Shut down the MQTT connection. */
        IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );

        /* Post twice to the wait semaphore, which unblocks the remaining blocking
         * callbacks. */
        IotSemaphore_Post( &waitSem );
        IotSemaphore_Post( &waitSem );

        /* Wait for the callbacks to exit. */
        while( IotSemaphore_GetCount( &waitSem ) > 0 )
        {
            IotClock_SleepMs( 100 );
        }

        /* Clear the MQTT connection flag so test cleanup does not double-free it. */
        _connectionCreated = false;
    }

    IotSemaphore_Destroy( &waitSem );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests result of matching topic filters and topic names.
 */
TEST( MQTT_Unit_Subscription, TopicFilterMatchTrue )
{
    _mqttSubscription_t * pTopicFilter =
        IotMqtt_MallocSubscription( sizeof( _mqttSubscription_t ) + _TOPIC_FILTER_MATCH_MAX_LENGTH );

    TEST_ASSERT_NOT_EQUAL( NULL, pTopicFilter );

    if( TEST_PROTECT() )
    {
        /* Exact matching. */
        _TEST_TOPIC_MATCH( "/exact", "/exact", true, true );
        _TEST_TOPIC_MATCH( "/exact", "/exact", false, true );

        /* Topic level wildcard matching. */
        _TEST_TOPIC_MATCH( "/aws", "/+", false, true );
        _TEST_TOPIC_MATCH( "/aws/iot", "/aws/+", false, true );
        _TEST_TOPIC_MATCH( "/aws/iot/shadow", "/aws/+/shadow", false, true );
        _TEST_TOPIC_MATCH( "/aws/iot/shadow", "/aws/+/+", false, true );
        _TEST_TOPIC_MATCH( "aws/", "aws/+", false, true );
        _TEST_TOPIC_MATCH( "/aws", "+/+", false, true );
        _TEST_TOPIC_MATCH( "aws//iot", "aws/+/iot", false, true );
        _TEST_TOPIC_MATCH( "aws//iot", "aws//+", false, true );
        _TEST_TOPIC_MATCH( "aws///iot", "aws/+/+/iot", false, true );

        /* Multi level wildcard matching. */
        _TEST_TOPIC_MATCH( "/aws/iot/shadow", "#", false, true );
        _TEST_TOPIC_MATCH( "aws/iot/shadow", "#", false, true );
        _TEST_TOPIC_MATCH( "/aws/iot/shadow", "/#", false, true );
        _TEST_TOPIC_MATCH( "aws/iot/shadow", "aws/iot/#", false, true );
        _TEST_TOPIC_MATCH( "aws/iot/shadow/thing", "aws/iot/#", false, true );
        _TEST_TOPIC_MATCH( "aws", "aws/#", false, true );

        /* Both topic level and multi level wildcard. */
        _TEST_TOPIC_MATCH( "aws/iot/shadow/thing/temp", "aws/+/shadow/#", false, true );
    }

    IotMqtt_FreeSubscription( pTopicFilter );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests result of matching topic filters and topic names that don't
 * match.
 */
TEST( MQTT_Unit_Subscription, TopicFilterMatchFalse )
{
    _mqttSubscription_t * pTopicFilter =
        IotMqtt_MallocSubscription( sizeof( _mqttSubscription_t ) + _TOPIC_FILTER_MATCH_MAX_LENGTH );

    TEST_ASSERT_NOT_EQUAL( NULL, pTopicFilter );

    if( TEST_PROTECT() )
    {
        /* Topic filter longer than filter name. */
        _TEST_TOPIC_MATCH( "/short", "/toolong", true, false );
        _TEST_TOPIC_MATCH( "/short", "/toolong", false, false );

        /* Case mismatch. */
        _TEST_TOPIC_MATCH( "/exact", "/eXaCt", true, false );
        _TEST_TOPIC_MATCH( "/exact", "/ExAcT", false, false );

        /* Substrings should not match. */
        _TEST_TOPIC_MATCH( "aws/", "aws/iot", true, false );
        _TEST_TOPIC_MATCH( "aws/", "aws/iot", false, false );

        /* Topic level wildcard matching. */
        _TEST_TOPIC_MATCH( "aws", "aws/", false, false );
        _TEST_TOPIC_MATCH( "aws/iot/shadow", "aws/+", false, false );
        _TEST_TOPIC_MATCH( "aws/iot/shadow", "aws/+/thing", false, false );
        _TEST_TOPIC_MATCH( "/aws", "+", false, false );

        /* Multi level wildcard matching. */
        _TEST_TOPIC_MATCH( "aws/iot/shadow", "iot/#", false, false );
        _TEST_TOPIC_MATCH( "aws/iot", "/#", false, false );

        /* Both topic level and multi level wildcard. */
        _TEST_TOPIC_MATCH( "aws/iot/shadow", "iot/+/#", false, false );
    }

    IotMqtt_FreeSubscription( pTopicFilter );
}

/*-----------------------------------------------------------*/
