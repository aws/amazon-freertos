/*
 * FreeRTOS Shadow V2.2.3
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
 * @file aws_iot_tests_shadow_api.c
 * @brief Tests for the user-facing API functions (declared in aws_iot_shadow.h).
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdint.h>
#include <string.h>

/* SDK initialization include. */
#include "iot_init.h"

/* Shadow internal include. */
#include "private/aws_iot_shadow_internal.h"

/* Error handling include. */
#include "private/iot_error.h"

/* Undefine logging configuration set in Shadow internal header. */
#undef LIBRARY_LOG_NAME
#undef LIBRARY_LOG_LEVEL

/* MQTT types include. */
#include "types/iot_mqtt_types.h"

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/* Undefine logging configuration set in MQTT internal header. */
#undef LIBRARY_LOG_NAME
#undef LIBRARY_LOG_LEVEL

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/* Test framework includes. */
#include "unity_fixture.h"

/* MQTT test access include. */
#include "iot_test_access_mqtt.h"

/* Require Shadow library asserts to be enabled for these tests. The Shadow
 * assert function is used to abort the tests on failure from the MQTT send
 * or receive threads. */
#if AWS_IOT_SHADOW_ENABLE_ASSERTS == 0
    #error "Shadow API unit tests require AWS_IOT_SHADOW_ENABLE_ASSERTS to be 1."
#endif

/**
 * @brief Whether to check the number of MQTT library errors in the malloc
 * failure tests.
 *
 * Should only be checked if malloc overrides are available and not testing for
 * code coverage. In static memory mode, there should be no MQTT library errors.
 */
#if ( IOT_TEST_COVERAGE == 1 ) || ( IOT_TEST_NO_MALLOC_OVERRIDES == 1 )
    #define CHECK_MQTT_ERROR_COUNT( expected, actual )
#else
    #if ( IOT_STATIC_MEMORY_ONLY == 1 )
        #define CHECK_MQTT_ERROR_COUNT( expected, actual )    TEST_ASSERT_EQUAL( 0, actual )
    #else
        #define CHECK_MQTT_ERROR_COUNT( expected, actual )    TEST_ASSERT_EQUAL( expected, actual )
    #endif
#endif

/*-----------------------------------------------------------*/

/**
 * @brief The Thing Name shared among all the tests.
 */
#define TEST_THING_NAME                         "TestThingName"

/**
 * @brief The length of #TEST_THING_NAME.
 */
#define TEST_THING_NAME_LENGTH                  ( sizeof( TEST_THING_NAME ) - 1 )

/**
 * @brief A delay that simulates the time required for an MQTT packet to be sent
 * to the server and for the server to send a response.
 */
#define NETWORK_ROUND_TRIP_TIME_MS              ( 25 )

/**
 * @brief The maximum size of any MQTT acknowledgement packet (e.g. SUBACK,
 * PUBACK, UNSUBACK) used in these tests.
 */
#define ACKNOWLEDGEMENT_PACKET_SIZE             ( 5 )

/**
 * @brief Test packet type for MQTT PUBLISH header.
 */
#define TEST_MQTT_PACKET_TYPE_PUBLISH_HEADER    ( MQTT_PACKET_TYPE_PUBLISH + 1 )

/*-----------------------------------------------------------*/

/**
 * @brief Context for calls to the network receive function.
 */
typedef struct _receiveContext
{
    const uint8_t * pData; /**< @brief The data to receive. */
    size_t dataLength;     /**< @brief Length of data. */
    size_t dataIndex;      /**< @brief Next byte of data to read. */
} _receiveContext_t;

/*-----------------------------------------------------------*/

/**
 * @brief The MQTT connection object shared among all the tests.
 */
static _mqttConnection_t * _pMqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

/**
 * @brief The #IotNetworkInterface_t to share among the tests.
 */
static IotNetworkInterface_t _networkInterface = { 0 };

/**
 * @brief Timer used to simulate a response from the network.
 */
static IotTimer_t _receiveTimer;

/**
 * @brief Synchronizes the MQTT send and receive threads in these tests.
 */
static IotMutex_t _lastPacketMutex;

/**
 * @brief The type of the last packet sent by the send thread.
 *
 * Must be one of: PUBLISH, SUBSCRIBE, UNSUBSCRIBE.
 */
static uint8_t _lastPacketType = 0;

/**
 * @brief The packet identifier of the last packet send by the send thread.
 */
static uint16_t _lastPacketIdentifier = 0;

/**
 * @brief Receive context for the MQTT PUBLISH packet.
 *
 * MQTT library sends MQTT PUBLISH packet using 2 different transport calls.
 * First one is for sending the header and then the payload. This static
 * context variable stores the header and adds payload to it to form the
 * complete PUBLISH packet.
 */
static _receiveContext_t _publishContext = { 0 };

/*-----------------------------------------------------------*/

/* Using initialized connToContext variable. */
extern _connContext_t connToContext[ MAX_NO_OF_MQTT_CONNECTIONS ];

/*-----------------------------------------------------------*/

/**
 * @brief Invokes the MQTT receive callback to simulate a response received from
 * the network.
 */
static void _receiveThread( void * pArgument )
{
    uint8_t pReceivedData[ ACKNOWLEDGEMENT_PACKET_SIZE ] = { 0 };
    _receiveContext_t receiveContext = { 0 };

    receiveContext.pData = pReceivedData;
    receiveContext.dataLength = ACKNOWLEDGEMENT_PACKET_SIZE;

    /* Silence warnings about unused parameters. */
    ( void ) pArgument;

    /* Lock mutex to read and process the last packet sent. */
    IotMutex_Lock( &_lastPacketMutex );

    /* Ensure that the last packet type and identifier were set. */
    AwsIotShadow_Assert( _lastPacketType != 0 );
    AwsIotShadow_Assert( _lastPacketIdentifier != 0 );

    /* Set the packet identifier in the ACK packet. */
    pReceivedData[ 2 ] = UINT16_HIGH_BYTE( _lastPacketIdentifier );
    pReceivedData[ 3 ] = UINT16_LOW_BYTE( _lastPacketIdentifier );

    /* Create the corresponding ACK packet based on the last packet type. */
    switch( _lastPacketType )
    {
        case MQTT_PACKET_TYPE_PUBLISH:

            pReceivedData[ 0 ] = MQTT_PACKET_TYPE_PUBACK;
            pReceivedData[ 1 ] = 2;
            receiveContext.dataLength = 4;
            break;

        case MQTT_PACKET_TYPE_SUBSCRIBE:

            pReceivedData[ 0 ] = MQTT_PACKET_TYPE_SUBACK;
            pReceivedData[ 1 ] = 3;
            pReceivedData[ 4 ] = 1;
            receiveContext.dataLength = 5;
            break;

        case MQTT_PACKET_TYPE_UNSUBSCRIBE:

            pReceivedData[ 0 ] = MQTT_PACKET_TYPE_UNSUBACK;
            pReceivedData[ 1 ] = 2;
            receiveContext.dataLength = 4;
            break;

        default:

            /* The only valid outgoing packets are PUBLISH, SUBSCRIBE, and
             * UNSUBSCRIBE. Abort if any other packet is found. */
            AwsIotShadow_Assert( 0 );
    }

    /* Call the MQTT receive callback to process the ACK packet. */
    IotMqtt_ReceiveCallback( &receiveContext,
                             _pMqttConnection );

    IotMutex_Unlock( &_lastPacketMutex );
}

/*-----------------------------------------------------------*/

/**
 * @brief A send function that always "succeeds". It also sets the receive
 * timer to respond with an ACK when necessary.
 */
static size_t _sendSuccess( void * pSendContext,
                            const uint8_t * pMessage,
                            size_t messageLength )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    _mqttOperation_t deserializedPublish = { .link = { 0 } };
    _mqttPacket_t mqttPacket = { .u.pMqttConnection = NULL };
    _receiveContext_t receiveContext = { 0 };

    /* Ignore the send context. */
    ( void ) pSendContext;

    /* Lock the mutex to modify the information on the last packet sent. */
    IotMutex_Lock( &_lastPacketMutex );

    if( _lastPacketType != TEST_MQTT_PACKET_TYPE_PUBLISH_HEADER )
    {
        /* Read the packet type, which is the first byte in the message. */
        mqttPacket.type = *pMessage;

        /* Set the members of the receive context. */
        receiveContext.pData = pMessage + 1;
        receiveContext.dataLength = messageLength;

        /* Read the remaining length. */
        mqttPacket.remainingLength = _IotMqtt_GetRemainingLength( &receiveContext,
                                                                  &_networkInterface );
        AwsIotShadow_Assert( mqttPacket.remainingLength != MQTT_REMAINING_LENGTH_INVALID );

        /* Set the last packet type based on the outgoing message. */
        switch( mqttPacket.type & 0xf0 )
        {
            case MQTT_PACKET_TYPE_PUBLISH:

                /* Only set the last packet type to PUBLISH for QoS 1. */
                if( ( ( *pMessage & 0x06 ) >> 1 ) == 1 )
                {
                    /* Check if only MQTT PUBLISH header is received. */
                    if( messageLength < mqttPacket.remainingLength )
                    {
                        /* PUBLISH header is received. */
                        _lastPacketType = TEST_MQTT_PACKET_TYPE_PUBLISH_HEADER;

                        /* Save the received data. */
                        memset( &_publishContext, 0x00, sizeof( _receiveContext_t ) );
                        /* Allocate a size of remaining length + first 2 bytes. */
                        _publishContext.pData = IotTest_Malloc( mqttPacket.remainingLength + sizeof( uint16_t ) );
                        AwsIotShadow_Assert( _publishContext.pData != NULL );

                        /* Copy the data received. */
                        memcpy( ( void * ) _publishContext.pData, pMessage, messageLength );
                        _publishContext.dataLength = messageLength;
                    }
                    else
                    {
                        _lastPacketType = MQTT_PACKET_TYPE_PUBLISH;
                    }
                }
                else
                {
                    _lastPacketType = 0;
                    _lastPacketIdentifier = 0;
                }

                break;

            case ( MQTT_PACKET_TYPE_SUBSCRIBE & 0xf0 ):
                _lastPacketType = MQTT_PACKET_TYPE_SUBSCRIBE;
                break;

            case ( MQTT_PACKET_TYPE_UNSUBSCRIBE & 0xf0 ):
                _lastPacketType = MQTT_PACKET_TYPE_UNSUBSCRIBE;
                break;

            default:

                /* The only valid outgoing packets are PUBLISH, SUBSCRIBE, and
                 * UNSUBSCRIBE. Abort if any other packet is found. */
                AwsIotShadow_Assert( 0 );
        }
    }
    else
    {
        /* MQTT PUBLISH payload is received. Mark the completion of PUBLISH. */
        _lastPacketType = MQTT_PACKET_TYPE_PUBLISH;
    }

    /* Check if a network response is needed. */
    if( ( _lastPacketType != 0 ) && ( _lastPacketType != TEST_MQTT_PACKET_TYPE_PUBLISH_HEADER ) )
    {
        /* Save the packet identifier as the last packet identifier. */
        if( _lastPacketType != MQTT_PACKET_TYPE_PUBLISH )
        {
            _lastPacketIdentifier = UINT16_DECODE( receiveContext.pData + receiveContext.dataIndex );
            status = IOT_MQTT_SUCCESS;
        }
        else
        {
            mqttPacket.u.pIncomingPublish = &deserializedPublish;

            /* Check if MQTT PUBLISH packet is fully received. */
            if( _publishContext.dataLength == 0 )
            {
                mqttPacket.pRemainingData = ( uint8_t * ) pMessage + ( messageLength - mqttPacket.remainingLength );
            }
            else
            {
                AwsIotShadow_Assert( _publishContext.pData != NULL );
                /* Copy the remaining part of the MQTT PUBLISH. */
                memcpy( ( void * ) ( _publishContext.pData + _publishContext.dataLength ), pMessage, messageLength );
                _publishContext.dataLength += messageLength;

                /* Read the packet type, which is the first byte in the message. */
                mqttPacket.type = _publishContext.pData[ 0 ];

                /* Set the members of the receive context. */
                receiveContext.pData = _publishContext.pData + 1;
                receiveContext.dataLength = _publishContext.dataLength;

                /* Read the remaining length. */
                mqttPacket.remainingLength = _IotMqtt_GetRemainingLength( &receiveContext,
                                                                          &_networkInterface );

                mqttPacket.pRemainingData = ( uint8_t * ) _publishContext.pData +
                                            ( _publishContext.dataLength - mqttPacket.remainingLength );
            }

            status = _IotMqtt_deserializePublishWrapper( &mqttPacket );
            _lastPacketIdentifier = mqttPacket.packetIdentifier;

            /* Clear the publish data stored. */
            if( _publishContext.dataLength != 0 )
            {
                IotTest_Free( ( void * ) _publishContext.pData );
                memset( &_publishContext, 0x00, sizeof( _receiveContext_t ) );
            }
        }

        AwsIotShadow_Assert( status == IOT_MQTT_SUCCESS );
        AwsIotShadow_Assert( _lastPacketIdentifier != 0 );

        /* Set the receive thread to run after a "network round-trip". */
        AwsIotShadow_Assert( IotClock_TimerArm( &_receiveTimer,
                                                NETWORK_ROUND_TRIP_TIME_MS,
                                                0 ) == true );
    }

    IotMutex_Unlock( &_lastPacketMutex );

    /* Return the message length to simulate a successful send. */
    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief Simulates a network receive function.
 */
static size_t _receive( void * pConnection,
                        uint8_t * pBuffer,
                        size_t bytesRequested )
{
    size_t bytesReceived = 0;
    _receiveContext_t * pReceiveContext = pConnection;

    AwsIotShadow_Assert( bytesRequested != 0 );
    AwsIotShadow_Assert( pReceiveContext->dataIndex < pReceiveContext->dataLength );

    /* Calculate how much data to copy. */
    const size_t dataAvailable = pReceiveContext->dataLength - pReceiveContext->dataIndex;

    if( bytesRequested > dataAvailable )
    {
        bytesReceived = dataAvailable;
    }
    else
    {
        bytesReceived = bytesRequested;
    }

    /* Copy data into given buffer. */
    if( bytesReceived > 0 )
    {
        ( void ) memcpy( pBuffer,
                         pReceiveContext->pData + pReceiveContext->dataIndex,
                         bytesReceived );

        pReceiveContext->dataIndex += bytesReceived;
    }

    return bytesReceived;
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
    bytesSent = pNetworkContext->pNetworkInterface->send( pNetworkContext->pNetworkConnection, ( const uint8_t * ) pMessage, bytesToSend );

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

static IotMqttError_t _setContext( IotMqttConnection_t pMqttConnection )
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
        connToContext[ contextIndex ].networkContext.pNetworkConnection = pMqttConnection->pNetworkConnection;
        connToContext[ contextIndex ].networkContext.pNetworkInterface = pMqttConnection->pNetworkInterface;

        /* Fill in TransportInterface send function pointer. We will not be implementing the
         * TransportInterface receive function pointer as receiving of packets is handled in shim by network
         * receive task. Only using MQTT LTS APIs for transmit path.*/
        transport.pNetworkContext = &( connToContext[ contextIndex ].networkContext );
        transport.send = transportSend;
        transport.recv = transportRecv;

        /* Fill the values for network buffer. */
        networkBuffer.pBuffer = &( connToContext[ contextIndex ].buffer[ 0 ] );
        networkBuffer.size = NETWORK_BUFFER_SIZE;
        subscriptionMutexCreated = IotMutex_CreateNonRecursiveMutex( &( connToContext[ contextIndex ].subscriptionMutex ),
                                                                     &( connToContext[ contextIndex ].subscriptionMutexStorage ) );

        if( subscriptionMutexCreated == false )
        {
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
            IOT_GOTO_CLEANUP();
        }
    }
    else
    {
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

/**
 * @brief Test group for Shadow API tests.
 */
TEST_GROUP( Shadow_Unit_API );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for Shadow API tests.
 */
TEST_SETUP( Shadow_Unit_API )
{
    IotMqttNetworkInfo_t networkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;

    /* Initialize SDK. */
    TEST_ASSERT_EQUAL_INT( true, IotSdk_Init() );

    /* Initialize the MQTT library. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, IotMqtt_Init() );

    /* Initialize the Shadow library. */
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, AwsIotShadow_Init( 0 ) );

    /* Clear the last packet type and identifier. */
    _lastPacketType = 0;
    _lastPacketIdentifier = 0;

    /* Create the mutex that synchronizes the receive callback and send thread. */
    TEST_ASSERT_EQUAL_INT( true, IotMutex_Create( &_lastPacketMutex, false ) );

    /* Create the receive thread timer. */
    IotClock_TimerCreate( &_receiveTimer,
                          _receiveThread,
                          NULL );

    /* Set the network interface send function. */
    ( void ) memset( &_networkInterface, 0x00, sizeof( IotNetworkInterface_t ) );
    _networkInterface.send = _sendSuccess;
    _networkInterface.receive = _receive;
    networkInfo.pNetworkInterface = &_networkInterface;

    /* Initialize the MQTT connection object to use for the Shadow tests. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( false,
                                                         &networkInfo,
                                                         0 );

    TEST_ASSERT_NOT_NULL( _pMqttConnection );

    /* Set the MQTT Context for the new MQTT Connection*/
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection ) );

    TEST_ASSERT_NOT_NULL( _pMqttConnection );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for Shadow API tests.
 */
TEST_TEAR_DOWN( Shadow_Unit_API )
{
    /* Clean up the MQTT connection object. */
    IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );

    /* Clean up the Shadow library. */
    AwsIotShadow_Cleanup();

    /* Clean up the MQTT library. */
    IotMqtt_Cleanup();

    /* Clean up SDK. */
    IotSdk_Cleanup();

    /* Destroy the receive thread timer. */
    IotClock_TimerDestroy( &_receiveTimer );

    /* Wait for the receive thread to finish and release the last packet mutex. */
    IotMutex_Lock( &_lastPacketMutex );

    /* Destroy the last packet mutex. */
    IotMutex_Unlock( &_lastPacketMutex );
    IotMutex_Destroy( &_lastPacketMutex );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for Shadow API tests.
 */
TEST_GROUP_RUNNER( Shadow_Unit_API )
{
    RUN_TEST_CASE( Shadow_Unit_API, Init );
    RUN_TEST_CASE( Shadow_Unit_API, OperationInvalidParameters );
    RUN_TEST_CASE( Shadow_Unit_API, DocumentInvalidParameters );
    RUN_TEST_CASE( Shadow_Unit_API, WaitInvalidParameters );
    RUN_TEST_CASE( Shadow_Unit_API, DeleteMallocFail );
    RUN_TEST_CASE( Shadow_Unit_API, GetMallocFail );
    RUN_TEST_CASE( Shadow_Unit_API, UpdateMallocFail );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the function @ref shadow_function_init.
 */
TEST( Shadow_Unit_API, Init )
{
    /* Check that test set up set the default value. */
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_DEFAULT_MQTT_TIMEOUT_MS, _AwsIotShadowMqttTimeoutMs );

    /* The Shadow library was already initialized by test set up. Clean it up
     * before running this test. */
    AwsIotShadow_Cleanup();

    /* Set a MQTT timeout. */
    AwsIotShadow_Init( AWS_IOT_SHADOW_DEFAULT_MQTT_TIMEOUT_MS + 1 );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_DEFAULT_MQTT_TIMEOUT_MS + 1, _AwsIotShadowMqttTimeoutMs );

    /* Cleanup should restore the default MQTT timeout. */
    AwsIotShadow_Cleanup();
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_DEFAULT_MQTT_TIMEOUT_MS, _AwsIotShadowMqttTimeoutMs );

    /* Initialize the Shadow library for test clean up. */
    AwsIotShadow_Init( 0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of Shadow operation functions with various
 * invalid parameters.
 */
TEST( Shadow_Unit_API, OperationInvalidParameters )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowDocumentInfo_t documentInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    AwsIotShadowOperation_t operation = AWS_IOT_SHADOW_OPERATION_INITIALIZER;
    AwsIotShadowCallbackInfo_t callbackInfo = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER;

    /* Missing Thing Name. */
    status = AwsIotShadow_Delete( _pMqttConnection,
                                  NULL,
                                  0,
                                  0,
                                  NULL,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    status = AwsIotShadow_Update( _pMqttConnection,
                                  &documentInfo,
                                  0,
                                  0,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* Thing Name too long. */
    status = AwsIotShadow_Delete( _pMqttConnection,
                                  TEST_THING_NAME,
                                  MAX_THING_NAME_LENGTH + 1,
                                  0,
                                  NULL,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* No reference with waitable operation. */
    status = AwsIotShadow_Delete( _pMqttConnection,
                                  TEST_THING_NAME,
                                  TEST_THING_NAME_LENGTH,
                                  AWS_IOT_SHADOW_FLAG_WAITABLE,
                                  NULL,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* Both callback and waitable flag set. */
    status = AwsIotShadow_Delete( _pMqttConnection,
                                  TEST_THING_NAME,
                                  TEST_THING_NAME_LENGTH,
                                  AWS_IOT_SHADOW_FLAG_WAITABLE,
                                  &callbackInfo,
                                  &operation );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* No callback for non-waitable GET. */
    documentInfo.pThingName = TEST_THING_NAME;
    documentInfo.thingNameLength = TEST_THING_NAME_LENGTH;
    status = AwsIotShadow_Get( _pMqttConnection,
                               &documentInfo,
                               0,
                               NULL,
                               NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* Callback function not set. */
    status = AwsIotShadow_Delete( _pMqttConnection,
                                  TEST_THING_NAME,
                                  TEST_THING_NAME_LENGTH,
                                  0,
                                  &callbackInfo,
                                  &operation );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of Shadow operation functions with an invalid
 * document info parameter.
 */
TEST( Shadow_Unit_API, DocumentInvalidParameters )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowDocumentInfo_t documentInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    AwsIotShadowOperation_t operation = AWS_IOT_SHADOW_OPERATION_INITIALIZER;

    /* Missing Thing Name. */
    status = AwsIotShadow_Get( _pMqttConnection,
                               &documentInfo,
                               AWS_IOT_SHADOW_FLAG_WAITABLE,
                               NULL,
                               &operation );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );
    documentInfo.pThingName = TEST_THING_NAME;
    documentInfo.thingNameLength = TEST_THING_NAME_LENGTH;

    /* Invalid QoS. */
    documentInfo.qos = ( IotMqttQos_t ) 3;
    status = AwsIotShadow_Get( _pMqttConnection,
                               &documentInfo,
                               AWS_IOT_SHADOW_FLAG_WAITABLE,
                               NULL,
                               &operation );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );
    documentInfo.qos = IOT_MQTT_QOS_0;

    /* Invalid retry parameters. */
    documentInfo.retryLimit = 1;
    status = AwsIotShadow_Get( _pMqttConnection,
                               &documentInfo,
                               AWS_IOT_SHADOW_FLAG_WAITABLE,
                               NULL,
                               &operation );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );
    documentInfo.retryLimit = 0;

    /* Waitable Shadow get with no memory allocation function. */
    status = AwsIotShadow_Get( _pMqttConnection,
                               &documentInfo,
                               AWS_IOT_SHADOW_FLAG_WAITABLE,
                               NULL,
                               &operation );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* Update with no document. */
    status = AwsIotShadow_Update( _pMqttConnection,
                                  &documentInfo,
                                  0,
                                  0,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* Update with no client token. */
    documentInfo.u.update.pUpdateDocument = "{\"state\":{\"reported\":{null}}}";
    documentInfo.u.update.updateDocumentLength = 29;
    status = AwsIotShadow_Update( _pMqttConnection,
                                  &documentInfo,
                                  0,
                                  0,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* Client token too long. */
    documentInfo.u.update.pUpdateDocument = "{\"state\":{\"reported\":{null}}},\"clientToken\": "
                                            "\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"";
    documentInfo.u.update.updateDocumentLength = 146;
    status = AwsIotShadow_Update( _pMqttConnection,
                                  &documentInfo,
                                  0,
                                  0,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref shadow_function_wait with various
 * invalid parameters.
 */
TEST( Shadow_Unit_API, WaitInvalidParameters )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    _shadowOperation_t operation;

    ( void ) memset( &operation, 0x00, sizeof( _shadowOperation_t ) );

    /* NULL reference. */
    status = AwsIotShadow_Wait( NULL, 0, NULL, NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* No waitable flag set. */
    status = AwsIotShadow_Wait( &operation, 0, NULL, NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* NULL output parameters for Shadow GET. */
    operation.flags = AWS_IOT_SHADOW_FLAG_WAITABLE;
    operation.type = _SHADOW_GET;
    status = AwsIotShadow_Wait( &operation, 0, NULL, NULL );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref shadow_function_delete when memory
 * allocation fails at various points.
 */
TEST( Shadow_Unit_API, DeleteMallocFail )
{
    int32_t i = 0, mqttErrorCount = 0;
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowOperation_t deleteOperation = AWS_IOT_SHADOW_OPERATION_INITIALIZER;

    /* Set a short timeout so this test runs faster. */
    _AwsIotShadowMqttTimeoutMs = 75;

    for( i = 0; ; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call Shadow DELETE. Memory allocation will fail at various times
         * during this call. */
        status = AwsIotShadow_Delete( _pMqttConnection,
                                      TEST_THING_NAME,
                                      TEST_THING_NAME_LENGTH,
                                      AWS_IOT_SHADOW_FLAG_WAITABLE,
                                      NULL,
                                      &deleteOperation );

        /* Once the Shadow DELETE call succeeds, wait for it to complete. */
        if( status == AWS_IOT_SHADOW_STATUS_PENDING )
        {
            /* No response will be received from the network, so the Shadow DELETE
             * is expected to time out. */
            TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_TIMEOUT,
                               AwsIotShadow_Wait( deleteOperation, 0, NULL, NULL ) );
            break;
        }

        /* Count the number of MQTT library errors. Otherwise, check that the error
         * is a "No memory" error. */
        if( status == AWS_IOT_SHADOW_MQTT_ERROR )
        {
            mqttErrorCount++;
        }
        else
        {
            TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_NO_MEMORY, status );
        }
    }

    /* Allow 2 MQTT library errors, which are caused by failure to allocate memory
     * for incoming packets (SUBSCRIBE, UNSUBSCRIBE). */
    CHECK_MQTT_ERROR_COUNT( 2, mqttErrorCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref shadow_function_get when memory
 * allocation fails at various points.
 */
TEST( Shadow_Unit_API, GetMallocFail )
{
    int32_t i = 0, mqttErrorCount = 0;
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowDocumentInfo_t documentInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    AwsIotShadowOperation_t getOperation = AWS_IOT_SHADOW_OPERATION_INITIALIZER;
    const char * pRetrievedDocument = NULL;
    size_t retrievedDocumentSize = 0;

    /* Set a short timeout so this test runs faster. */
    _AwsIotShadowMqttTimeoutMs = 75;

    /* Set the members of the document info. */
    documentInfo.pThingName = TEST_THING_NAME;
    documentInfo.thingNameLength = TEST_THING_NAME_LENGTH;
    documentInfo.qos = IOT_MQTT_QOS_1;
    documentInfo.u.get.mallocDocument = IotTest_Malloc;

    for( i = 0; ; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call Shadow GET. Memory allocation will fail at various times
         * during this call. */
        status = AwsIotShadow_Get( _pMqttConnection,
                                   &documentInfo,
                                   AWS_IOT_SHADOW_FLAG_WAITABLE,
                                   NULL,
                                   &getOperation );

        /* Once the Shadow GET call succeeds, wait for it to complete. */
        if( status == AWS_IOT_SHADOW_STATUS_PENDING )
        {
            /* No response will be received from the network, so the Shadow GET
             * is expected to time out. */
            TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_TIMEOUT,
                               AwsIotShadow_Wait( getOperation,
                                                  0,
                                                  &pRetrievedDocument,
                                                  &retrievedDocumentSize ) );
            break;
        }

        /* Count the number of MQTT library errors. Otherwise, check that the error
         * is a "No memory" error. */
        if( status == AWS_IOT_SHADOW_MQTT_ERROR )
        {
            mqttErrorCount++;
        }
        else
        {
            TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_NO_MEMORY, status );
        }
    }

    /* Allow 3 MQTT library errors, which are caused by failure to allocate memory
     * for incoming packets (SUBSCRIBE, PUBLISH, UNSUBSCRIBE). */
    CHECK_MQTT_ERROR_COUNT( 3, mqttErrorCount );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref shadow_function_update when memory
 * allocation fails at various points.
 */
TEST( Shadow_Unit_API, UpdateMallocFail )
{
    int32_t i = 0, mqttErrorCount = 0;
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowDocumentInfo_t documentInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    AwsIotShadowOperation_t updateOperation = AWS_IOT_SHADOW_OPERATION_INITIALIZER;

    /* Set a short timeout so this test runs faster. */
    _AwsIotShadowMqttTimeoutMs = 75;

    /* Set the members of the document info. */
    documentInfo.pThingName = TEST_THING_NAME;
    documentInfo.thingNameLength = TEST_THING_NAME_LENGTH;
    documentInfo.qos = IOT_MQTT_QOS_1;
    documentInfo.u.update.pUpdateDocument = "{\"state\":{\"reported\":{null}},\"clientToken\":\"TEST\"}";
    documentInfo.u.update.updateDocumentLength = 50;

    for( i = 0; ; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call Shadow UPDATE. Memory allocation will fail at various times
         * during this call. */
        status = AwsIotShadow_Update( _pMqttConnection,
                                      &documentInfo,
                                      AWS_IOT_SHADOW_FLAG_WAITABLE,
                                      NULL,
                                      &updateOperation );

        /* Once the Shadow UPDATE call succeeds, wait for it to complete. */
        if( status == AWS_IOT_SHADOW_STATUS_PENDING )
        {
            /* No response will be received from the network, so the Shadow UPDATE
             * is expected to time out. */
            TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_TIMEOUT,
                               AwsIotShadow_Wait( updateOperation,
                                                  0,
                                                  NULL,
                                                  NULL ) );
            break;
        }

        /* Count the number of MQTT library errors. Otherwise, check that the error
         * is a "No memory" error. */
        if( status == AWS_IOT_SHADOW_MQTT_ERROR )
        {
            mqttErrorCount++;
        }
        else
        {
            TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_NO_MEMORY, status );
        }
    }

    /* Allow 3 MQTT library errors, which are caused by failure to allocate memory
     * for incoming packets (SUBSCRIBE, PUBLISH, UNSUBSCRIBE). */
    CHECK_MQTT_ERROR_COUNT( 3, mqttErrorCount );
}

/*-----------------------------------------------------------*/
