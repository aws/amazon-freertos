/*
 * FreeRTOS MQTT V2.3.1
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

/**
 * @file iot_tests_mqtt_api.c
 * @brief Tests for the user-facing API functions (declared in iot_mqtt.h).
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* SDK initialization include. */
#include "iot_init.h"

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/* Test framework includes. */
#include "unity_fixture.h"

/* MQTT test access include. */
#include "iot_test_access_mqtt.h"

/* MQTT mock include. */
#include "iot_tests_mqtt_mock.h"

/* Error handling include. */
#include "private/iot_error.h"

/*-----------------------------------------------------------*/

/**
 * @brief Determine which MQTT server mode to test (AWS IoT or Mosquitto).
 */
#if !defined( IOT_TEST_MQTT_MOSQUITTO ) || IOT_TEST_MQTT_MOSQUITTO == 0
    #define AWS_IOT_MQTT_SERVER    true
#else
    #define AWS_IOT_MQTT_SERVER    false
#endif

/**
 * @brief Timeout to use for the tests. This can be short, but should allow time
 * for other threads to run.
 */
#define TIMEOUT_MS                                    ( 400 )

/**
 * @brief A short keep-alive interval to use for the keep-alive tests. It may be
 * shorter than the minimum 1 second specified by the MQTT spec.
 */
#define SHORT_KEEP_ALIVE_MS                           ( IOT_MQTT_RESPONSE_WAIT_MS + 100 )

/**
 * @brief The number of times the periodic keep-alive should run.
 */
#define KEEP_ALIVE_COUNT                              ( 10 )

/*
 * Client identifier and length to use for the MQTT API tests.
 */
#define CLIENT_IDENTIFIER                             ( "test" )                                           /**< @brief Client identifier. */
#define CLIENT_IDENTIFIER_LENGTH                      ( ( uint16_t ) ( sizeof( CLIENT_IDENTIFIER ) - 1 ) ) /**< @brief Length of client identifier. */

/*
 * Will topic name and length to use for the MQTT API tests.
 */
#define TEST_TOPIC_NAME                               ( "/test/topic" )                                  /**< @brief An arbitrary topic name. */
#define TEST_TOPIC_NAME_LENGTH                        ( ( uint16_t ) ( sizeof( TEST_TOPIC_NAME ) - 1 ) ) /**< @brief Length of topic name. */

/*
 * Strings with format specifiers for printing the periodic keep-alive status.
 * This is needed in the thread simulating incoming PINGRESP messages.
 */
#define KEEP_ALIVE_PERIODIC_STATUS_MAIN_STRING        "KeepAlivePeriodic %d of %d DONE at %lu ms"
#define KEEP_ALIVE_PERIODIC_STATUS_INTERVAL_STRING    " (+ %lu ).\r\n"
#define KEEP_ALIVE_PERIODIC_STATUS_ENDING_STRING      ".\r\n"

/**
 * @brief The length of the string use to print the keep-alive periodic status.
 * This is needed in the thread simulating incoming PINGRESP messages.
 */
#define KEEP_ALIVE_PERIODIC_STATUS_LENGTH             128


/**
 * @brief A non-NULL function pointer to use for subscription callback. This
 * "function" should cause a crash if actually called.
 */
#define SUBSCRIPTION_CALLBACK \
    ( ( void ( * )( void *,   \
                    IotMqttCallbackParam_t * ) ) 0x01 )

/**
 * @brief How many times #TEST_MQTT_Unit_API_DisconnectMallocFail_ will test
 * malloc failures.
 *
 * The DISCONNECT function provides no mechanism to wait on its successful
 * completion. Therefore, this function simply uses the value below as an estimate
 * for the maximum number of times DISCONNECT will use malloc.
 */
#define DISCONNECT_MALLOC_LIMIT    ( 20 )

/*
 * Constants that affect the behavior of #TEST_MQTT_Unit_API_PublishDuplicates.
 */
#define DUP_CHECK_RETRY_MS         ( 100 )  /**< @brief When to start sending duplicate packets. */
#define DUP_CHECK_RETRY_LIMIT      ( 3 )    /**< @brief How many duplicate packets to send. */
#define DUP_CHECK_TIMEOUT          ( 3000 ) /**< @brief Total time allowed to send all duplicate packets.
                                             * Duplicates are sent using an exponential backoff strategy. */

/**
 * @brief The minimum amount of time the #TEST_MQTT_Unit_API_PublishDuplicate
 * test can take.
 */
#define DUP_CHECK_MINIMUM_WAIT \
    ( DUP_CHECK_RETRY_MS +     \
      2 * DUP_CHECK_RETRY_MS + \
      4 * DUP_CHECK_RETRY_MS + \
      IOT_MQTT_RESPONSE_WAIT_MS )

/*-----------------------------------------------------------*/

/**
 * @brief Each compilation unit that consumes the NetworkContext must define it.
 * It should contain a single pointer to the type of your desired transport.
 * When using multiple transports in the same compilation unit, define this pointer as void *.
 *
 * @note Transport stacks are defined in amazon-freertos/libraries/abstractions/transport/secure_sockets/transport_secure_sockets.h.
 */
struct NetworkContext
{
    MqttTransportParams_t * pParams;
};

/*-----------------------------------------------------------*/

/**
 * @brief Tracks whether #_publishSetDup has been called.
 */
static bool _publishSetDupCalled = false;

/**
 * @brief Counts how many time #_sendPingreq has been called.
 */
static int32_t _pingreqSendCount = 0;

/**
 * @brief Counts how many times #_close has been called.
 */
static int32_t _closeCount = 0;

/**
 * @brief Counts how many times #_disconnectCallback has been called.
 */
static int32_t _disconnectCallbackCount = 0;

/**
 * @brief An MQTT connection to share among the tests.
 */
static _mqttConnection_t * _pMqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

/**
 * @brief An #IotMqttNetworkInfo_t to share among the tests.
 */
static IotMqttNetworkInfo_t _networkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;

/**
 * @brief An #IotNetworkInterface_t to share among the tests.
 */
static IotNetworkInterface_t _networkInterface = { 0 };

/**
 * @brief Buffer holding the keep-alive periodic status string.
 * This needed in the thread simulating incoming PINGRESP messages.
 */
static char pKeepAliveStatus[ KEEP_ALIVE_PERIODIC_STATUS_LENGTH ] = { 0 };

/**
 * @brief Represents the network context used for the TLS session with the
 * server.
 */
static NetworkContext_t networkContext = { 0 };

/*-----------------------------------------------------------*/

/* Using initialized connToContext variable. */
extern _connContext_t connToContext[ MAX_NO_OF_MQTT_CONNECTIONS ];

/*-----------------------------------------------------------*/

/**
 * @brief A thread routine that simulates an incoming PINGRESP.
 */
static void _incomingPingresp( void * pArgument )
{
    /* Silence warnings about unused parameters. */
    ( void ) pArgument;

    static int32_t invokeCount = 0;
    static uint64_t lastInvokeTime = 0;
    uint64_t currentTime = IotClock_GetTimeMs();

    /* The number of characters written in snprintf. */
    int numWritten = 0;

    /* Increment invoke count for this function. */
    invokeCount++;

    /* Sleep to simulate the network round-trip time. Should be less than
     * the response wait time. */
    IotClock_SleepMs( IOT_MQTT_RESPONSE_WAIT_MS / 2 );

    /* Respond with a PINGRESP. */
    if( invokeCount <= KEEP_ALIVE_COUNT )
    {
        /* Log a status with Unity, as this test may take a while. */
        numWritten = snprintf( pKeepAliveStatus,
                               KEEP_ALIVE_PERIODIC_STATUS_LENGTH,
                               KEEP_ALIVE_PERIODIC_STATUS_MAIN_STRING,
                               invokeCount,
                               KEEP_ALIVE_COUNT,
                               ( unsigned long ) IotClock_GetTimeMs() );
        /* Assert there are no unlikely encoding errors. */
        TEST_ASSERT( numWritten > 0 );

        if( invokeCount > 1 )
        {
            /* Write the ms interval between the PINGRESP's. */
            int prevNumWritten = numWritten;
            numWritten += snprintf( &( pKeepAliveStatus[ numWritten ] ),
                                    KEEP_ALIVE_PERIODIC_STATUS_LENGTH - numWritten,
                                    KEEP_ALIVE_PERIODIC_STATUS_INTERVAL_STRING,
                                    ( unsigned long ) ( currentTime - lastInvokeTime ) );
            /* Assert there are no unlikely encoding errors. */
            TEST_ASSERT( numWritten > prevNumWritten );
        }
        else
        {
            /* Write a period with some line endings for pretty printing. The
             * NULL termintor in the C string is also written. */
            memcpy( &( pKeepAliveStatus[ numWritten ] ),
                    KEEP_ALIVE_PERIODIC_STATUS_ENDING_STRING,
                    sizeof( KEEP_ALIVE_PERIODIC_STATUS_ENDING_STRING ) );
        }

        /* Print the status. */
        UnityPrint( pKeepAliveStatus );

        lastInvokeTime = currentTime;

        IotMqtt_ReceiveCallback( NULL,
                                 _pMqttConnection );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief PUBLISH set DUP function override.
 */
static void _publishSetDup( uint8_t * pPublishPacket,
                            uint8_t * pPacketIdentifierHigh,
                            uint16_t * pNewPacketIdentifier )
{
    _publishSetDupCalled = true;

    _IotMqtt_PublishSetDup( pPublishPacket,
                            pPacketIdentifierHigh,
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
    /* Silence warnings about unused parameters. */
    ( void ) pSendContext;
    ( void ) pMessage;

    /* Create a thread that responds with PINGRESP, then increment the PINGREQ
     * send counter if successful. */
    if( Iot_CreateDetachedThread( _incomingPingresp,
                                  NULL,
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
    IotClock_SleepMs( 2000 );

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
    #if AWS_IOT_MQTT_SERVER == true
        static uint16_t lastPacketIdentifier = 0;
        _mqttPacket_t publishPacket = { .u.pMqttConnection = NULL };
        _mqttOperation_t publishOperation = { .link = { 0 } };

        publishPacket.type = publishFlags;
        publishPacket.u.pIncomingPublish = &publishOperation;
        publishPacket.remainingLength = 8 + TEST_TOPIC_NAME_LENGTH;
        publishPacket.pRemainingData = ( uint8_t * ) pMessage + ( messageLength - publishPacket.remainingLength );
    #endif

    /* Ignore any MQTT packet that's not a PUBLISH. */
    if( ( publishFlags & 0xf0 ) != MQTT_PACKET_TYPE_PUBLISH )
    {
        return messageLength;
    }

    runCount++;

    /* Check how many times this function has been called. */
    if( runCount == 1 )
    {
        #if AWS_IOT_MQTT_SERVER == true
            /* Deserialize the PUBLISH to read the packet identifier. */
            if( _IotMqtt_deserializePublishWrapper( &publishPacket ) != IOT_MQTT_SUCCESS )
            {
                status = false;
            }
            else
            {
                lastPacketIdentifier = publishPacket.packetIdentifier;
            }
        #else /* if AWS_IOT_MQTT_SERVER == true */
            /* DUP flag should not be set on this function's first run. */
            if( ( publishFlags & 0x08 ) == 0x08 )
            {
                status = false;
            }
        #endif /* if AWS_IOT_MQTT_SERVER == true */
    }
    else
    {
        /* Only check the packet again if the previous run checks passed. */
        if( status == true )
        {
            #if AWS_IOT_MQTT_SERVER == true
                /* Deserialize the PUBLISH to read the packet identifier. */
                if( _IotMqtt_deserializePublishWrapper( &publishPacket ) != IOT_MQTT_SUCCESS )
                {
                    status = false;
                }
                else
                {
                    /* Check that the packet identifier is different. */
                    status = ( publishPacket.packetIdentifier != lastPacketIdentifier );
                    lastPacketIdentifier = publishPacket.packetIdentifier;
                }
            #else /* if AWS_IOT_MQTT_SERVER == true */
                /* DUP flag should be set when this function runs again. */
                if( ( publishFlags & 0x08 ) != 0x08 )
                {
                    status = false;
                }
            #endif /* if AWS_IOT_MQTT_SERVER == true */
        }

        /* Write the check result on the last expected run of this function. */
        if( runCount == DUP_CHECK_RETRY_LIMIT )
        {
            *pDupCheckResult = status;
        }
    }

    /* Return the message length to simulate a successful send. */
    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief A network receive function that simulates receiving a PINGRESP.
 */
static size_t _receivePingresp( void * pReceiveContext,
                                uint8_t * pBuffer,
                                size_t bytesRequested )
{
    size_t bytesReceived = 0;
    static size_t receiveIndex = 0;
    const uint8_t pPingresp[ 2 ] = { MQTT_PACKET_TYPE_PINGRESP, 0x00 };

    /* Silence warnings about unused parameters. */
    ( void ) pReceiveContext;

    /* Receive of PINGRESP should only ever request 1 byte. */
    if( bytesRequested == 1 )
    {
        /* Write a byte of PINGRESP. */
        *pBuffer = pPingresp[ receiveIndex ];
        bytesReceived = 1;

        /* Alternate the byte of PINGRESP to write. */
        receiveIndex = ( receiveIndex + 1 ) % 2;
    }

    return bytesReceived;
}

/*-----------------------------------------------------------*/

/**
 * @brief A function for setting the receive callback that just returns success.
 */
static IotNetworkError_t _setReceiveCallback( void * pConnection,
                                              IotNetworkReceiveCallback_t receiveCallback,
                                              void * pReceiveContext )
{
    /* Silence warnings about unused parameters. */
    ( void ) pConnection;
    ( void ) receiveCallback;
    ( void ) pReceiveContext;

    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

/**
 * @brief A network close function that counts how many times it was invoked.
 */
static IotNetworkError_t _close( void * pCloseContext )
{
    /* Silence warnings about unused parameters. */
    ( void ) pCloseContext;

    _closeCount++;

    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

/**
 * @brief An MQTT disconnect callback that counts how many times it was invoked.
 */
static void _disconnectCallback( void * pCallbackContext,
                                 IotMqttCallbackParam_t * pCallbackParam )
{
    IotMqttDisconnectReason_t * pExpectedReason = ( IotMqttDisconnectReason_t * ) pCallbackContext;

    /* Only increment counter if the reasons match. */
    if( pCallbackParam->u.disconnectReason == *pExpectedReason )
    {
        _disconnectCallbackCount++;
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief A task pool job routine that decrements an MQTT operation's job
 * reference count.
 */
static void _decrementReferencesJob( IotTaskPool_t pTaskPool,
                                     IotTaskPoolJob_t pJob,
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
        IotSemaphore_Post( &( pOperation->u.operation.notify.waitSemaphore ) );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Transport send interface provided to the MQTT context used in calling MQTT LTS APIs.
 */
static int32_t transportSend( NetworkContext_t * pNetworkContext,
                              const void * pMessage,
                              size_t bytesToSend )
{
    int32_t bytesSent = 0;

    IotMqtt_Assert( pNetworkContext != NULL );
    IotMqtt_Assert( pMessage != NULL );

    /* Sending the bytes on the network using Network Interface. */
    bytesSent = pNetworkContext->pParams->pNetworkInterface->send( pNetworkContext->pParams->pNetworkConnection, ( const uint8_t * ) pMessage, bytesToSend );

    if( bytesSent < 0 )
    {
        /* Network Send Interface return negative value in case of any socket error,
         * unifying the error codes here for socket error and timeout to comply with the MQTT LTS Library.
         */
        bytesSent = -1;
    }

    return bytesSent;
}

/*-----------------------------------------------------------*/

/**
 * @brief A transport send function that delays.
 */
static int32_t transportSendDelay( NetworkContext_t * pSendContext,
                                   const void * pMessage,
                                   size_t messageLength )
{
    IotSemaphore_t * pWaitSem = ( IotSemaphore_t * ) pSendContext;

    /* Silence warnings about unused parameters. */
    ( void ) pMessage;

    /* Post to the wait semaphore. */
    IotSemaphore_Post( pWaitSem );

    /* Delay for 2 seconds. */
    IotClock_SleepMs( 2000 );

    /* This function returns the message length to simulate a successful send. */
    return ( int32_t ) messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief A dummy function for transport interface receive.
 *
 * MQTT shim handles the receive from the network and hence transport
 * implementation for receive is not used by the coreMQTT library. This
 * dummy implementation is used for passing a non-NULL parameter to
 * `MQTT_Init()`.
 *
 * @param[in] pNetworkContext Implementation-defined network context.
 * @param[in] pBuffer Buffer to receive the data into.
 * @param[in] bytesToRecv Number of bytes requested from the network.
 *
 * @return The number of bytes received or a negative error code.
 */
static int32_t transportRecv( NetworkContext_t * pNetworkContext,
                              void * pBuffer,
                              size_t bytesToRecv )
{
    /* MQTT shim handles the receive from the network and hence transport
     * implementation for receive is not used by the coreMQTT library. This
     * dummy implementation is used for passing a non-NULL parameter to
     * `MQTT_Init()`. */
    ( void ) pNetworkContext;
    ( void ) pBuffer;
    ( void ) bytesToRecv;

    /* Always return an error. */
    return -1;
}

/*-----------------------------------------------------------*/

/**
 * @brief The time interface provided to the MQTT context used in calling MQTT LTS APIs.
 */
static uint32_t getTimeMs( void )
{
    return 0U;
}

/*-----------------------------------------------------------*/

/**
 * @brief The dummy application callback function.
 *
 * This function doesn't need to have any implementation as
 * the receive from network is not handled by the coreMQTT library,
 * but the MQTT shim itself. This function is just a dummy function
 * to be passed as a parameter to `MQTT_Init()`.
 *
 * @param[in] pMqttContext MQTT context pointer.
 * @param[in] pPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] pDeserializedInfo Deserialized information from incoming packet.
 */
static void eventCallback( MQTTContext_t * pContext,
                           MQTTPacketInfo_t * pPacketInfo,
                           MQTTDeserializedInfo_t * pDeserializedInfo )
{
    /* This function doesn't need to have any implementation as
     * the receive from network is not handled by the coreMQTT library,
     * but the MQTT shim itself. This function is just a dummy function
     * to be passed as a parameter to `MQTT_Init()`. */
    ( void ) pContext;
    ( void ) pPacketInfo;
    ( void ) pDeserializedInfo;
}

/*-----------------------------------------------------------*/

/**
 * @brief Setting the MQTT Context for the given MQTT Connection.
 */
static IotMqttError_t _setContext( IotMqttConnection_t pMqttConnection,
                                   TransportSend_t transportSend )
{
    IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_BAD_PARAMETER );
    int8_t contextIndex = -1;
    bool subscriptionMutexCreated = false;
    bool contextMutex = false;
    TransportInterface_t transport;
    MQTTFixedBuffer_t networkBuffer;
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;

    /* Clear the MQTT connection to context array. */
    memset( connToContext, 0x00, sizeof( connToContext ) );

    /* Getting the free index from the MQTT connection to MQTT context mapping array. */
    contextIndex = _IotMqtt_getFreeIndexFromContextConnectionArray();
    TEST_ASSERT_NOT_EQUAL( -1, contextIndex );

    /* Clear the array at the index obtained. */
    memset( &( connToContext[ contextIndex ] ), 0x00, sizeof( _connContext_t ) );

    /* Creating Mutex for the synchronization of MQTT Context used for sending the packets
     * on the network using MQTT LTS API. */
    contextMutex = IotMutex_CreateRecursiveMutex( &( connToContext[ contextIndex ].contextMutex ),
                                                  &( connToContext[ contextIndex ].contextMutexStorage ) );

    /* Create the subscription mutex for a new connection. */
    if( contextMutex == true )
    {
        /* Assigning the MQTT Connection. */
        connToContext[ contextIndex ].mqttConnection = pMqttConnection;

        /* Assigning the Network Context to be used by this MQTT Context. */
        networkContext.pParams = &connToContext[ contextIndex ].mqttTransportParams;
        connToContext[ contextIndex ].mqttTransportParams.pNetworkConnection = pMqttConnection->pNetworkConnection;
        connToContext[ contextIndex ].mqttTransportParams.pNetworkInterface = pMqttConnection->pNetworkInterface;

        /* Fill in TransportInterface send function pointer. We will not be implementing the
         * TransportInterface receive function pointer as receiving of packets is handled in shim by network
         * receive task. Only using MQTT LTS APIs for transmit path.*/
        transport.pNetworkContext = &networkContext;
        transport.send = transportSend;
        transport.recv = transportRecv;

        /* Fill the values for network buffer. */
        networkBuffer.pBuffer = &( connToContext[ contextIndex ].buffer[ 0 ] );
        networkBuffer.size = NETWORK_BUFFER_SIZE;
        subscriptionMutexCreated = IotMutex_CreateNonRecursiveMutex( &( connToContext[ contextIndex ].subscriptionMutex ),
                                                                     &( connToContext[ contextIndex ].subscriptionMutexStorage ) );

        if( subscriptionMutexCreated == false )
        {
            IotLogError( "Failed to create subscription mutex for new connection." );
            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
        }
        else
        {
            /* Initializing the MQTT context used in calling MQTT LTS API. */
            managedMqttStatus = MQTT_Init( &( connToContext[ contextIndex ].context ), &transport, getTimeMs, eventCallback, &networkBuffer );
            status = convertReturnCode( managedMqttStatus );
        }

        if( status != IOT_MQTT_SUCCESS )
        {
            IotLogError( "(MQTT connection %p) Failed to initialize context for "
                         "the MQTT connection.",
                         pMqttConnection );
            IOT_GOTO_CLEANUP();
        }
    }
    else
    {
        IotLogError( "(MQTT connection %p) Failed to create mutex for "
                     "the MQTT context.",
                     pMqttConnection );
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
    }

    IOT_FUNCTION_CLEANUP_BEGIN();

    /* Clean up the context on error. */
    if( status != IOT_MQTT_SUCCESS )
    {
        _IotMqtt_removeContext( pMqttConnection );
    }

    IOT_FUNCTION_CLEANUP_END();
}


/*-----------------------------------------------------------*/

static bool _isEmpty( _mqttSubscription_t * pSubscriptionArray )
{
    bool status = true;
    size_t index = 0;

    while( index < MAX_NO_OF_MQTT_SUBSCRIPTIONS )
    {
        if( pSubscriptionArray[ index ].topicFilterLength != 0 )
        {
            status = false;
            break;
        }

        index++;
    }

    return status;
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

    /* Reset the network info and interface. */
    ( void ) memset( &_networkInfo, 0x00, sizeof( IotMqttNetworkInfo_t ) );
    ( void ) memset( &_networkInterface, 0x00, sizeof( IotNetworkInterface_t ) );
    _networkInterface.setReceiveCallback = _setReceiveCallback;
    _networkInfo.pNetworkInterface = &_networkInterface;

    /* Reset the counters. */
    _pingreqSendCount = 0;
    _closeCount = 0;
    _disconnectCallbackCount = 0;

    /* Initialize libraries. */
    TEST_ASSERT_EQUAL_INT( true, IotSdk_Init() );
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, IotMqtt_Init() );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for MQTT API tests.
 */
TEST_TEAR_DOWN( MQTT_Unit_API )
{
    IotMqtt_Cleanup();
    IotSdk_Cleanup();
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
    RUN_TEST_CASE( MQTT_Unit_API, SubscribeUnsubscribeParameters );
    RUN_TEST_CASE( MQTT_Unit_API, SubscribeMallocFail );
    RUN_TEST_CASE( MQTT_Unit_API, UnsubscribeMallocFail );
    RUN_TEST_CASE( MQTT_Unit_API, KeepAlivePeriodic );
    RUN_TEST_CASE( MQTT_Unit_API, KeepAliveJobCleanup );
    RUN_TEST_CASE( MQTT_Unit_API, WaitAfterDisconnect );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test reference counts as MQTT operations are created and destroyed.
 */
TEST( MQTT_Unit_API, OperationCreateDestroy )
{
    _mqttOperation_t * pOperation = NULL;
    bool subscriptionMutexCreated = false;

    /* Create a new MQTT connection. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( AWS_IOT_MQTT_SERVER,
                                                         &_networkInfo,
                                                         0 );
    TEST_ASSERT_NOT_NULL( _pMqttConnection );

    /* Set the MQTT Context for the new MQTT Connection*/
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection, transportSend ) );

    /* Adjustment to reference count based on keep-alive status. */
    const int32_t keepAliveReference = 1 + ( ( _pMqttConnection->keepAliveMs != 0 ) ? 1 : 0 );

    /* A new MQTT connection should only have a possible reference for keep-alive. */
    TEST_ASSERT_EQUAL_INT32( keepAliveReference, _pMqttConnection->references );

    /* Create a new operation referencing the MQTT connection. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _IotMqtt_CreateOperation( _pMqttConnection,
                                                                   IOT_MQTT_FLAG_WAITABLE,
                                                                   NULL,
                                                                   &pOperation ) );

    /* Check reference counts and list placement. */
    TEST_ASSERT_EQUAL_INT32( 1 + keepAliveReference, _pMqttConnection->references );
    TEST_ASSERT_EQUAL_INT32( 2, pOperation->u.operation.jobReference );
    TEST_ASSERT_EQUAL_PTR( &( pOperation->link ), IotListDouble_FindFirstMatch( &( _pMqttConnection->pendingProcessing ),
                                                                                NULL,
                                                                                NULL,
                                                                                &( pOperation->link ) ) );

    /* Schedule a job that destroys the operation. */
    TEST_ASSERT_EQUAL( IOT_TASKPOOL_SUCCESS, IotTaskPool_CreateJob( _decrementReferencesJob,
                                                                    pOperation,
                                                                    &( pOperation->jobStorage ),
                                                                    &( pOperation->job ) ) );
    TEST_ASSERT_EQUAL( IOT_TASKPOOL_SUCCESS, IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL,
                                                                   pOperation->job,
                                                                   0 ) );

    /* Wait for the job to complete. */
    IotSemaphore_Wait( &( pOperation->u.operation.notify.waitSemaphore ) );

    /* Check reference counts after job completion. */
    TEST_ASSERT_EQUAL_INT32( 1 + keepAliveReference, _pMqttConnection->references );
    TEST_ASSERT_EQUAL_INT32( 1, pOperation->u.operation.jobReference );
    TEST_ASSERT_EQUAL_PTR( &( pOperation->link ), IotListDouble_FindFirstMatch( &( _pMqttConnection->pendingProcessing ),
                                                                                NULL,
                                                                                NULL,
                                                                                &( pOperation->link ) ) );

    /* Disconnect the MQTT connection, then call Wait to clean up the operation. */
    IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );
    IotMqtt_Wait( pOperation, 0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that an operation is correctly cleaned up if @ref mqtt_function_wait
 * times out while its job is executing.
 */
TEST( MQTT_Unit_API, OperationWaitTimeout )
{
    _mqttOperation_t * pOperation = NULL;
    IotSemaphore_t waitSem;

    /* An arbitrary MQTT packet for this test. All packets are malloc'ed. */
    uint8_t * pPacket = IotMqtt_MallocMessage( 2 );

    pPacket[ 0 ] = MQTT_PACKET_TYPE_PINGREQ;
    pPacket[ 1 ] = 0x00;

    /* Create the wait semaphore. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &waitSem, 0, 1 ) );

    if( TEST_PROTECT() )
    {
        /* Set the network interface send function. */
        _networkInterface.send = _sendDelay;

        /* Create a new MQTT connection. */
        _pMqttConnection = IotTestMqtt_createMqttConnection( AWS_IOT_MQTT_SERVER,
                                                             &_networkInfo,
                                                             0 );
        TEST_ASSERT_NOT_NULL( _pMqttConnection );

        /* Set the MQTT Context for the new MQTT Connection*/
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection, transportSendDelay ) );

        /* Set parameter to network send function. */
        _pMqttConnection->pNetworkConnection = &waitSem;

        /* Create a new operation referencing the MQTT connection. */
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _IotMqtt_CreateOperation( _pMqttConnection,
                                                                       IOT_MQTT_FLAG_WAITABLE,
                                                                       NULL,
                                                                       &pOperation ) );

        /* Set an arbitrary MQTT packet for the operation. */
        pOperation->u.operation.type = IOT_MQTT_PINGREQ;
        pOperation->u.operation.pMqttPacket = pPacket;
        pOperation->u.operation.packetSize = 2;

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
        IotMutex_Lock( &( _pMqttConnection->referencesMutex ) );
        TEST_ASSERT_EQUAL_INT32( 1, pOperation->u.operation.jobReference );
        IotMutex_Unlock( &( _pMqttConnection->referencesMutex ) );

        /* Disconnect the MQTT connection. */
        IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );

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
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttPublishInfo_t willInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;

    _networkInterface.send = _sendSuccess;
    _networkInterface.close = _close;

    /* Check that the network interface is validated. */
    status = IotMqtt_Connect( &_networkInfo,
                              &connectInfo,
                              TIMEOUT_MS,
                              &_pMqttConnection );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

    /* Check that the connection info is validated. */
    status = IotMqtt_Connect( &_networkInfo,
                              &connectInfo,
                              TIMEOUT_MS,
                              &_pMqttConnection );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
    connectInfo.pClientIdentifier = CLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = CLIENT_IDENTIFIER_LENGTH;

    /* Connect with bad previous session subscription. */
    connectInfo.cleanSession = false;
    connectInfo.pPreviousSubscriptions = &subscription;
    connectInfo.previousSubscriptionCount = 1;
    status = IotMqtt_Connect( &_networkInfo,
                              &connectInfo,
                              TIMEOUT_MS,
                              &_pMqttConnection );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

    /* Connect with bad subscription count. */
    connectInfo.previousSubscriptionCount = 0;
    subscription.pTopicFilter = TEST_TOPIC_NAME;
    subscription.topicFilterLength = TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = SUBSCRIPTION_CALLBACK;
    status = IotMqtt_Connect( &_networkInfo,
                              &connectInfo,
                              TIMEOUT_MS,
                              &_pMqttConnection );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
    connectInfo.previousSubscriptionCount = 1;

    /* Check that the will info is validated when it's provided. */
    connectInfo.pWillInfo = &willInfo;
    status = IotMqtt_Connect( &_networkInfo,
                              &connectInfo,
                              TIMEOUT_MS,
                              &_pMqttConnection );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
    willInfo.pTopicName = TEST_TOPIC_NAME;
    willInfo.topicNameLength = TEST_TOPIC_NAME_LENGTH;

    /* Check that a will message longer than 65535 is not allowed. */
    willInfo.pPayload = "";
    willInfo.payloadLength = 65536;
    status = IotMqtt_Connect( &_networkInfo,
                              &connectInfo,
                              TIMEOUT_MS,
                              &_pMqttConnection );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
    willInfo.payloadLength = 0;

    /* Check that passing a wait time of 0 returns immediately. */
    status = IotMqtt_Connect( &_networkInfo,
                              &connectInfo,
                              0,
                              &_pMqttConnection );
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
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;

    /* Initialize parameters. */
    _networkInterface.send = _sendSuccess;
    _networkInterface.close = _close;
    connectInfo.keepAliveSeconds = 100;
    connectInfo.cleanSession = true;
    connectInfo.pClientIdentifier = CLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = CLIENT_IDENTIFIER_LENGTH;

    for( i = 0; ; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call CONNECT. Memory allocation will fail at various times during
         * this call. */
        status = IotMqtt_Connect( &_networkInfo,
                                  &connectInfo,
                                  TIMEOUT_MS,
                                  &_pMqttConnection );

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
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;

    /* Initialize parameters. */
    _networkInterface.send = _sendSuccess;
    _networkInterface.close = _close;
    connectInfo.cleanSession = false;
    connectInfo.keepAliveSeconds = 100;
    connectInfo.pClientIdentifier = CLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = CLIENT_IDENTIFIER_LENGTH;
    subscription.pTopicFilter = TEST_TOPIC_NAME;
    subscription.topicFilterLength = TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = SUBSCRIPTION_CALLBACK;

    connectInfo.pPreviousSubscriptions = &subscription;
    connectInfo.previousSubscriptionCount = 1;

    for( i = 0; ; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call CONNECT with a previous session. Memory allocation will fail at
         * various times during this call. */
        status = IotMqtt_Connect( &_networkInfo,
                                  &connectInfo,
                                  TIMEOUT_MS,
                                  &_pMqttConnection );

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
    IotMqttDisconnectReason_t expectedReason = IOT_MQTT_DISCONNECT_CALLED;

    /* Set the members of the network interface. */
    _networkInterface.send = _sendSuccess;
    _networkInterface.close = _close;
    _networkInfo.createNetworkConnection = false;
    _networkInfo.disconnectCallback.pCallbackContext = &expectedReason;
    _networkInfo.disconnectCallback.function = _disconnectCallback;

    for( i = 0; i < DISCONNECT_MALLOC_LIMIT; i++ )
    {
        /* Allow unlimited use of malloc during connection initialization. */
        UnityMalloc_MakeMallocFailAfterCount( -1 );

        /* Create a new MQTT connection. */
        _pMqttConnection = IotTestMqtt_createMqttConnection( AWS_IOT_MQTT_SERVER,
                                                             &_networkInfo,
                                                             0 );

        TEST_ASSERT_NOT_NULL( _pMqttConnection );

        /* Set the MQTT Context for the new MQTT Connection*/
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection, transportSend ) );

        /* Set malloc to eventually fail. */
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call DISCONNECT; this function should always perform cleanup regardless
         * of memory allocation errors. */
        IotMqtt_Disconnect( _pMqttConnection, 0 );
        TEST_ASSERT_EQUAL_INT( 1, _closeCount );
        TEST_ASSERT_EQUAL_INT( 1, _disconnectCallbackCount );
        _closeCount = 0;
        _disconnectCallbackCount = 0;
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
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttOperation_t publishOperation = IOT_MQTT_OPERATION_INITIALIZER;
    IotMqttCallbackInfo_t callbackInfo = IOT_MQTT_CALLBACK_INFO_INITIALIZER;

    /* Initialize parameters. */
    _networkInterface.send = _sendSuccess;

    /* Create a new MQTT connection. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( AWS_IOT_MQTT_SERVER,
                                                         &_networkInfo,
                                                         0 );
    TEST_ASSERT_NOT_NULL( _pMqttConnection );

    /* Set the MQTT Context for the new MQTT Connection*/
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection, transportSend ) );

    if( TEST_PROTECT() )
    {
        /* Check that the publish info is validated. */
        status = IotMqtt_Publish( _pMqttConnection, &publishInfo, 0, NULL, NULL );
        TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
        publishInfo.pTopicName = TEST_TOPIC_NAME;
        publishInfo.topicNameLength = TEST_TOPIC_NAME_LENGTH;

        /* Check that a QoS 0 publish is refused if a notification is requested. */
        status = IotMqtt_Publish( _pMqttConnection, &publishInfo, IOT_MQTT_FLAG_WAITABLE, NULL, &publishOperation );
        TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );
        status = IotMqtt_Publish( _pMqttConnection, &publishInfo, 0, &callbackInfo, NULL );
        TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

        /* If valid parameters are passed, QoS 0 publish should always return success. */
        status = IotMqtt_Publish( _pMqttConnection, &publishInfo, 0, 0, &publishOperation );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
    }

    IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );
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
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;

    /* Initialize parameters. */
    _networkInterface.send = _sendSuccess;

    /* Create a new MQTT connection. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( AWS_IOT_MQTT_SERVER,
                                                         &_networkInfo,
                                                         0 );
    TEST_ASSERT_NOT_NULL( _pMqttConnection );

    /* Set the MQTT Context for the new MQTT Connection*/
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection, transportSend ) );

    /* Set the necessary members of publish info. */
    publishInfo.pTopicName = TEST_TOPIC_NAME;
    publishInfo.topicNameLength = TEST_TOPIC_NAME_LENGTH;

    if( TEST_PROTECT() )
    {
        for( i = 0; ; i++ )
        {
            UnityMalloc_MakeMallocFailAfterCount( i );

            /* Call PUBLISH. Memory allocation will fail at various times during
             * this call. */
            status = IotMqtt_Publish( _pMqttConnection, &publishInfo, 0, NULL, NULL );

            /* Once PUBLISH succeeds, the loop can exit. */
            if( status == IOT_MQTT_SUCCESS )
            {
                break;
            }

            /* If the return value isn't success, check that it is memory allocation
             * failure. */
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }
    }

    IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );
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
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttOperation_t publishOperation = IOT_MQTT_OPERATION_INITIALIZER;
    IotMqttCallbackInfo_t callbackInfo = IOT_MQTT_CALLBACK_INFO_INITIALIZER;

    /* Initialize parameters. */
    _networkInterface.send = _sendSuccess;

    /* Create a new MQTT connection. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( AWS_IOT_MQTT_SERVER,
                                                         &_networkInfo,
                                                         0 );
    TEST_ASSERT_NOT_NULL( _pMqttConnection );

    /* Set the MQTT Context for the new MQTT Connection*/
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection, transportSend ) );

    /* Set the publish info. */
    publishInfo.qos = IOT_MQTT_QOS_1;
    publishInfo.pTopicName = TEST_TOPIC_NAME;
    publishInfo.topicNameLength = TEST_TOPIC_NAME_LENGTH;

    if( TEST_PROTECT() )
    {
        /* Setting the waitable flag with no reference is not allowed. */
        status = IotMqtt_Publish( _pMqttConnection,
                                  &publishInfo,
                                  IOT_MQTT_FLAG_WAITABLE,
                                  NULL,
                                  NULL );
        TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

        /* Setting both the waitable flag and callback info is not allowed. */
        status = IotMqtt_Publish( _pMqttConnection,
                                  &publishInfo,
                                  IOT_MQTT_FLAG_WAITABLE,
                                  &callbackInfo,
                                  &publishOperation );
        TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

        /* Check QoS 1 PUBLISH behavior with malloc failures. */
        for( i = 0; ; i++ )
        {
            UnityMalloc_MakeMallocFailAfterCount( i );

            /* Call PUBLISH. Memory allocation will fail at various times during
             * this call. */
            status = IotMqtt_Publish( _pMqttConnection,
                                      &publishInfo,
                                      IOT_MQTT_FLAG_WAITABLE,
                                      NULL,
                                      &publishOperation );

            /* If the PUBLISH succeeded, the loop can exit after waiting for the QoS
             * 1 PUBLISH to be cleaned up. */
            if( status == IOT_MQTT_STATUS_PENDING )
            {
                TEST_ASSERT_EQUAL( IOT_MQTT_TIMEOUT, IotMqtt_Wait( publishOperation, TIMEOUT_MS ) );
                break;
            }

            /* If the return value isn't success, check that it is memory allocation
             * failure. */
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }
    }

    /* Clean up MQTT connection. */
    IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );
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
    static IotMqttSerializer_t serializer = IOT_MQTT_SERIALIZER_INITIALIZER;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttOperation_t publishOperation = IOT_MQTT_OPERATION_INITIALIZER;
    bool dupCheckResult = false;
    uint64_t startTime = 0;

    /* Initializer parameters. */
    serializer.serialize.publishSetDup = _publishSetDup;
    _networkInterface.send = _dupChecker;

    /* Create a new MQTT connection. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( AWS_IOT_MQTT_SERVER,
                                                         &_networkInfo,
                                                         0 );
    TEST_ASSERT_NOT_NULL( _pMqttConnection );
    /* Set the MQTT Context for the new MQTT Connection*/
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection, transportSend ) );

    /* Set the serializers and parameter to the send function. */
    _pMqttConnection->pNetworkConnection = &dupCheckResult;
    _pMqttConnection->pSerializer = &serializer;

    /* Set the publish info. */
    publishInfo.qos = IOT_MQTT_QOS_1;
    publishInfo.pTopicName = TEST_TOPIC_NAME;
    publishInfo.topicNameLength = TEST_TOPIC_NAME_LENGTH;
    publishInfo.pPayload = "test";
    publishInfo.payloadLength = 4;
    publishInfo.retryMs = DUP_CHECK_RETRY_MS;
    publishInfo.retryLimit = DUP_CHECK_RETRY_LIMIT;

    startTime = IotClock_GetTimeMs();

    if( TEST_PROTECT() )
    {
        /* Send a PUBLISH with retransmissions enabled. */
        TEST_ASSERT_EQUAL( IOT_MQTT_STATUS_PENDING,
                           IotMqtt_Publish( _pMqttConnection,
                                            &publishInfo,
                                            IOT_MQTT_FLAG_WAITABLE,
                                            NULL,
                                            &publishOperation ) );

        /* Since _dupChecker doesn't actually transmit a PUBLISH, no PUBACK is
         * expected. */
        TEST_ASSERT_EQUAL( IOT_MQTT_RETRY_NO_RESPONSE,
                           IotMqtt_Wait( publishOperation, DUP_CHECK_TIMEOUT ) );

        /* Check the result of the DUP check. */
        TEST_ASSERT_EQUAL_INT( true, dupCheckResult );

        /* Check that at least the minimum wait time elapsed. */
        TEST_ASSERT_TRUE( startTime + DUP_CHECK_MINIMUM_WAIT <= IotClock_GetTimeMs() );
    }

    /* Clean up MQTT connection. */
    IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );

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
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttOperation_t subscribeOperation = IOT_MQTT_OPERATION_INITIALIZER;

    /* Create a new MQTT connection. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( AWS_IOT_MQTT_SERVER,
                                                         &_networkInfo,
                                                         0 );
    TEST_ASSERT_NOT_NULL( _pMqttConnection );

    /* Set the MQTT Context for the new MQTT Connection*/
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection, transportSend ) );

    /* Check that subscription info is validated. */
    status = IotMqtt_Subscribe( _pMqttConnection,
                                &subscription,
                                1,
                                IOT_MQTT_FLAG_WAITABLE,
                                NULL,
                                &subscribeOperation );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

    status = IotMqtt_Unsubscribe( _pMqttConnection,
                                  &subscription,
                                  1,
                                  IOT_MQTT_FLAG_WAITABLE,
                                  NULL,
                                  &subscribeOperation );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

    subscription.pTopicFilter = TEST_TOPIC_NAME;
    subscription.topicFilterLength = TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = SUBSCRIPTION_CALLBACK;

    /* A reference must be provided for a waitable SUBSCRIBE. */
    status = IotMqtt_Subscribe( _pMqttConnection,
                                &subscription,
                                1,
                                IOT_MQTT_FLAG_WAITABLE,
                                NULL,
                                NULL );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

    status = IotMqtt_Unsubscribe( _pMqttConnection,
                                  &subscription,
                                  1,
                                  IOT_MQTT_FLAG_WAITABLE,
                                  NULL,
                                  NULL );
    TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER, status );

    IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );
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
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttOperation_t subscribeOperation = IOT_MQTT_OPERATION_INITIALIZER;

    /* Initializer parameters. */
    _networkInterface.send = _sendSuccess;

    /* Create a new MQTT connection. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( AWS_IOT_MQTT_SERVER,
                                                         &_networkInfo,
                                                         0 );
    TEST_ASSERT_NOT_NULL( _pMqttConnection );

    /* Set the MQTT Context for the new MQTT Connection*/
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection, transportSend ) );

    /* Set the necessary members of the subscription. */
    subscription.pTopicFilter = TEST_TOPIC_NAME;
    subscription.topicFilterLength = TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = SUBSCRIPTION_CALLBACK;

    if( TEST_PROTECT() )
    {
        for( i = 0; ; i++ )
        {
            UnityMalloc_MakeMallocFailAfterCount( i );

            /* Call SUBSCRIBE. Memory allocation will fail at various times during
             * this call. */
            status = IotMqtt_Subscribe( _pMqttConnection,
                                        &subscription,
                                        1,
                                        IOT_MQTT_FLAG_WAITABLE,
                                        NULL,
                                        &subscribeOperation );

            /* If the SUBSCRIBE succeeded, the loop can exit after waiting for
             * the SUBSCRIBE to be cleaned up. */
            if( status == IOT_MQTT_STATUS_PENDING )
            {
                TEST_ASSERT_EQUAL( IOT_MQTT_TIMEOUT, IotMqtt_Wait( subscribeOperation, TIMEOUT_MS ) );
                break;
            }

            /* If the return value isn't success, check that it is memory allocation
             * failure. */
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }
    }

    IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );
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
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttOperation_t unsubscribeOperation = IOT_MQTT_OPERATION_INITIALIZER;
    int8_t contextIndex = -1;

    /* Initialize parameters. */
    _networkInterface.send = _sendSuccess;

    /* Create a new MQTT connection. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( AWS_IOT_MQTT_SERVER,
                                                         &_networkInfo,
                                                         0 );
    TEST_ASSERT_NOT_NULL( _pMqttConnection );

    /* Set the MQTT Context for the new MQTT Connection*/
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection, transportSend ) );

    /* Set the necessary members of the subscription. */
    subscription.pTopicFilter = TEST_TOPIC_NAME;
    subscription.topicFilterLength = TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = SUBSCRIPTION_CALLBACK;

    if( TEST_PROTECT() )
    {
        for( i = 0; ; i++ )
        {
            UnityMalloc_MakeMallocFailAfterCount( i );

            /* Call UNSUBSCRIBE. Memory allocation will fail at various times during
             * this call. */
            status = IotMqtt_Unsubscribe( _pMqttConnection,
                                          &subscription,
                                          1,
                                          IOT_MQTT_FLAG_WAITABLE,
                                          NULL,
                                          &unsubscribeOperation );

            /* If the UNSUBSCRIBE succeeded, the loop can exit after waiting for
             * the UNSUBSCRIBE to be cleaned up. */
            if( status == IOT_MQTT_STATUS_PENDING )
            {
                TEST_ASSERT_EQUAL( IOT_MQTT_TIMEOUT, IotMqtt_Wait( unsubscribeOperation, TIMEOUT_MS ) );
                break;
            }

            /* If the return value isn't success, check that it is memory allocation
             * failure. */
            TEST_ASSERT_EQUAL( IOT_MQTT_NO_MEMORY, status );
        }

        contextIndex = _IotMqtt_getContextIndexFromConnection( _pMqttConnection );
        /* No lingering subscriptions should be in the MQTT connection. */
        TEST_ASSERT_TRUE( _isEmpty( connToContext[ contextIndex ].subscriptionArray ) );
    }

    IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests keep-alive handling and ensures that it is periodic.
 */
TEST( MQTT_Unit_API, KeepAlivePeriodic )
{
    /* The expected disconnect reason for this test's disconnect callback. */
    IotMqttDisconnectReason_t expectedReason = IOT_MQTT_KEEP_ALIVE_TIMEOUT;

    /* An estimate for the amount of time this test requires. */
    const uint32_t sleepTimeMs = ( ( 2 + KEEP_ALIVE_COUNT ) * SHORT_KEEP_ALIVE_MS ) + 1500;

    /* Print a newline so this test may log its status. */
    UNITY_PRINT_EOL();

    /* Initialize parameters. */
    _networkInterface.send = _sendPingreq;
    _networkInterface.receive = _receivePingresp;
    _networkInterface.close = _close;
    _networkInfo.disconnectCallback.pCallbackContext = &expectedReason;
    _networkInfo.disconnectCallback.function = _disconnectCallback;

    /* Create a new MQTT connection. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( AWS_IOT_MQTT_SERVER,
                                                         &_networkInfo,
                                                         1 );
    TEST_ASSERT_NOT_NULL( _pMqttConnection );
    /* Set the MQTT Context for the new MQTT Connection*/
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection, transportSend ) );

    /* Set a short keep-alive interval so this test runs faster. */
    _pMqttConnection->keepAliveMs = SHORT_KEEP_ALIVE_MS;
    _pMqttConnection->nextKeepAliveMs = SHORT_KEEP_ALIVE_MS;

    /* Schedule the initial PINGREQ. */
    TEST_ASSERT_EQUAL( IOT_TASKPOOL_SUCCESS,
                       IotTaskPool_ScheduleDeferred( IOT_SYSTEM_TASKPOOL,
                                                     _pMqttConnection->keepAliveJob,
                                                     _pMqttConnection->nextKeepAliveMs ) );

    /* Sleep to allow ample time for periodic PINGREQ sends and PINGRESP responses. */
    IotClock_SleepMs( sleepTimeMs );

    /* Check the counters for PINGREQ send and close. */
    TEST_ASSERT_EQUAL_INT32( KEEP_ALIVE_COUNT + 1, _pingreqSendCount );
    TEST_ASSERT_EQUAL_INT32( 1, _closeCount );

    /* Check that the disconnect callback was invoked once (with reason
     * "keep-alive timeout"). */
    TEST_ASSERT_EQUAL_INT32( 1, _disconnectCallbackCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests that the keep-alive job cleans up the MQTT connection after a call
 * to @ref mqtt_function_disconnect.
 */
TEST( MQTT_Unit_API, KeepAliveJobCleanup )
{
    IotSemaphore_t waitSem;

    /* Initialize parameters. */
    _networkInterface.send = _sendSuccess;

    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &waitSem, 0, 1 ) );

    if( TEST_PROTECT() )
    {
        /* Create a new MQTT connection. */
        _pMqttConnection = IotTestMqtt_createMqttConnection( AWS_IOT_MQTT_SERVER,
                                                             &_networkInfo,
                                                             1 );
        TEST_ASSERT_NOT_NULL( _pMqttConnection );
        /* Set the parameter to the send function. */
        _pMqttConnection->pNetworkConnection = &waitSem;

        /* Set a short keep-alive interval so this test runs faster. */
        _pMqttConnection->keepAliveMs = SHORT_KEEP_ALIVE_MS;
        _pMqttConnection->nextKeepAliveMs = SHORT_KEEP_ALIVE_MS;

        /* Set the MQTT Context for the new MQTT Connection*/
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection, transportSend ) );



        /* Schedule the initial PINGREQ. */
        TEST_ASSERT_EQUAL( IOT_TASKPOOL_SUCCESS,
                           IotTaskPool_ScheduleDeferred( IOT_SYSTEM_TASKPOOL,
                                                         _pMqttConnection->keepAliveJob,
                                                         _pMqttConnection->nextKeepAliveMs ) );

        /* Wait for the keep-alive job to send a PINGREQ. */
        IotSemaphore_Wait( &waitSem );

        /* Immediately disconnect the connection. */
        IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );
    }

    IotSemaphore_Destroy( &waitSem );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that Wait can be safely invoked after Disconnect.
 */
TEST( MQTT_Unit_API, WaitAfterDisconnect )
{
    int32_t i = 0;
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttOperation_t pPublishOperation[ 3 ] = { IOT_MQTT_OPERATION_INITIALIZER };

    /* Set the members of the publish info. */
    publishInfo.qos = IOT_MQTT_QOS_1;
    publishInfo.pTopicName = TEST_TOPIC_NAME;
    publishInfo.topicNameLength = TEST_TOPIC_NAME_LENGTH;
    publishInfo.pPayload = "";
    publishInfo.payloadLength = 0;
    publishInfo.retryLimit = 3;
    publishInfo.retryMs = 5000;

    /* Establish the MQTT connection. */
    TEST_ASSERT_EQUAL_INT( true, IotTest_MqttMockInit( &_pMqttConnection ) );


    /* Set the MQTT Context for the new MQTT Connection*/
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection, transportSend ) );

    if( TEST_PROTECT() )
    {
        /* Publish a sequence of messages. */
        for( i = 0; i < 3; i++ )
        {
            status = IotMqtt_Publish( _pMqttConnection,
                                      &publishInfo,
                                      IOT_MQTT_FLAG_WAITABLE,
                                      NULL,
                                      &( pPublishOperation[ i ] ) );
            TEST_ASSERT_EQUAL( IOT_MQTT_STATUS_PENDING, status );
        }
    }

    /* Disconnect the MQTT connection. */
    IotTest_MqttMockCleanup();

    if( TEST_PROTECT() )
    {
        /* Waiting on operations after a connection is disconnected should not crash.
         * The actual statuses of the PUBLISH operations may vary depending on the
         * timing of publish versus disconnect, so the statuses are not checked. */
        for( i = 0; i < 3; i++ )
        {
            status = IotMqtt_Wait( pPublishOperation[ i ], 100 );
        }
    }
}

/*-----------------------------------------------------------*/
