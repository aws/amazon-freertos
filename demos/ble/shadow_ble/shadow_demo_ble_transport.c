/*
 * FreeRTOS V202203.00
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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


/* Standard includes. */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* FreeRTOS includes for task syncrhonization and delay. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Defines configs used by BLE library and transport interface demo. */
#include "iot_ble_config.h"
#include "shadow_ble_demo_config.h"

#include "aws_clientcredential.h"

/* BLE transport interface headers */
#include "iot_ble_mqtt_transport.h"
#include "iot_ble_data_transfer.h"

/* MQTT API header. */
#include "core_mqtt.h"

/* SHADOW API header. */
#include "shadow.h"

/* JSON parser header to parse shadow documents */
#include "core_json.h"


/**
 * @file shadow_demo_ble_transport.c
 *
 * @brief Demo for showing how to use the Device Shadow library's API with BLE transport
 * interface implementation to perform shadow updates assisted by a companion mobile device.
 * This example assumes there is a powerOn state in the device shadow. It does the
 * following operations:
 * 1. Establishes a BLE data transfer channel with the companion device over BLE GATT connection.
 * 2. Establishes a MQTT connection to the AWS IOT through the companion device.
 * 3. Assemble strings for the MQTT topics of device shadow, by using macros defined by the Device Shadow library.
 * 4. Subscribe to those MQTT topics.
 * 5. Publish a desired state of powerOn by using helper functions.  That will cause a delta message to be sent to device.
 * 6. Handle incoming MQTT messages in prvEventCallback, determine whether the message is related to the device
 * shadow by using a function defined by the Device Shadow library (Shadow_MatchTopic). If the message is a
 * device shadow delta message, set a flag for the main function to know, then the main function will publish
 * a second message to update the reported state of powerOn.
 * 7. Handle incoming message again in prvEventCallback. If the message is from update/accepted, verify that it
 * has the same clientToken as previously published in the update message. That will mark the end of the demo.
 *
 * A smartphone application using the FreeRTOS companion mobile SDK, in BLE central role should scan and
 * connect to this device successfully, before starting the demo.
 * Broker endpoint and the thing name should be configured in aws_clientcredential.h.
 */

/**
 * @brief Timeout for MQTT_ProcessLoop in milliseconds.
 */
#define mqttexamplePROCESS_LOOP_TIMEOUT_MS    ( 5000U )


/**
 * @brief Keep alive time reported to the broker while establishing an MQTT connection.
 *
 * It is the responsibility of the Client to ensure that the interval between
 * Control Packets being sent does not exceed the this Keep Alive value. In the
 * absence of sending any other Control Packets, the Client MUST send a
 * PINGREQ Packet.
 */
#define mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS    ( 60U )


/**
 * @brief Timeout for receiving CONNACK packet in milliseconds.
 */
#define mqttexampleCONNACK_RECV_TIMEOUT_MS    ( 5000U )

/**
 * @brief Milliseconds per second.
 */
#define MILLISECONDS_PER_SECOND               ( 1000U )

/**
 * @brief Milliseconds per FreeRTOS tick.
 */
#define MILLISECONDS_PER_TICK                 ( MILLISECONDS_PER_SECOND / configTICK_RATE_HZ )


/**
 * @brief Format string representing a Shadow document with a "desired" state.
 *
 * The real json document will look like this:
 * {
 *   "state": {
 *     "desired": {
 *       "powerOn": 1
 *     }
 *   },
 *   "clientToken": "021909"
 * }
 *
 * Note the client token, which is optional for all Shadow updates. The client
 * token must be unique at any given time, but may be reused once the update is
 * completed. For this demo, a timestamp is used for a client token.
 */
#define SHADOW_DESIRED_JSON     \
    "{"                         \
    "\"state\":{"               \
    "\"desired\":{"             \
    "\"powerOn\":%01d"          \
    "}"                         \
    "},"                        \
    "\"clientToken\":\"%06lu\"" \
    "}"

/**
 * @brief The expected size of #SHADOW_DESIRED_JSON.
 *
 * Because all the format specifiers in #SHADOW_DESIRED_JSON include a length,
 * its full actual size is known by pre-calculation, here's the formula why
 * the length need to minus 3:
 * 1. The length of "%01d" is 4.
 * 2. The length of %06lu is 5.
 * 3. The actual length we will use in case 1. is 1 ( for the state of powerOn ).
 * 4. The actual length we will use in case 2. is 6 ( for the clientToken length ).
 * 5. Thus the additional size 3 = 4 + 5 - 1 - 6 + 1 (termination character).
 *
 * In your own application, you could calculate the size of the json doc in this way.
 */
#define SHADOW_DESIRED_JSON_LENGTH    ( sizeof( SHADOW_DESIRED_JSON ) - 3 )

/**
 * @brief Format string representing a Shadow document with a "reported" state.
 *
 * The real json document will look like this:
 * {
 *   "state": {
 *     "reported": {
 *       "powerOn": 1
 *     }
 *   },
 *   "clientToken": "021909"
 * }
 *
 * Note the client token, which is required for all Shadow updates. The client
 * token must be unique at any given time, but may be reused once the update is
 * completed. For this demo, a timestamp is used for a client token.
 */
#define SHADOW_REPORTED_JSON    \
    "{"                         \
    "\"state\":{"               \
    "\"reported\":{"            \
    "\"powerOn\":%01d"          \
    "}"                         \
    "},"                        \
    "\"clientToken\":\"%06lu\"" \
    "}"

/**
 * @brief The expected size of #SHADOW_REPORTED_JSON.
 *
 * Because all the format specifiers in #SHADOW_REPORTED_JSON include a length,
 * its full size is known at compile-time by pre-calculation. Users could refer to
 * the way how to calculate the actual length in #SHADOW_DESIRED_JSON_LENGTH.
 */
#define SHADOW_REPORTED_JSON_LENGTH    ( sizeof( SHADOW_REPORTED_JSON ) - 3 )

#ifndef THING_NAME

/**
 * @brief Predefined thing name.
 *
 * This is the example predefine thing name and could be compiled in ROM code.
 */
    #define THING_NAME    clientcredentialIOT_THING_NAME
#endif


/**
 * @brief The length of #THING_NAME.
 */
#define THING_NAME_LENGTH    ( ( uint16_t ) ( sizeof( THING_NAME ) - 1 ) )

/*-----------------------------------------------------------*/

/**
 * @brief Each compilation unit that consumes the NetworkContext must define it.
 * It should contain a single pointer to the type of your desired transport.
 * When using multiple transports in the same compilation unit, define this pointer as void *.
 *
 * @note Transport stacks are defined in amazon-freertos/libraries/ble/include/iot_ble_mqtt_transport.h.
 */
struct NetworkContext
{
    BleTransportParams_t * pParams;
};

/*-----------------------------------------------------------*/

/**
 * @brief BLE transport interface context passed in by the MQTT library.
 */
static NetworkContext_t xBLETransportCtxt = { 0 };

/**
 * @brief Ble Transport Parameters structure to store the data channel.
 */
static BleTransportParams_t xBleTransportParams;

/**
 * @brief Semaphore used to synchronize BLE data transfer channel callback.
 */
static SemaphoreHandle_t xChannelSemaphore;

/**
 * @brief Global flag to represent channel closure.
 */
static bool channelActive = false;

/**
 * @brief Static buffer used to queue packets on BLE transport interface.
 */
static uint8_t ucTransportBuffer[ democonfigBLE_TRANSPORT_BUFFER_SIZE ] = { 0 };

/**
 * @breif MQTT context passed by the application to the MQTT library.
 */
static MQTTContext_t xMQTTContext = { 0 };

/**
 * @brief Static buffer used to hold MQTT messages being sent and received.
 */
static uint8_t ucSharedBuffer[ democonfigNETWORK_BUFFER_SIZE ] = { 0 };

/**
 * @brief Static buffer used to hold MQTT messages being sent and received.
 */
static MQTTFixedBuffer_t xBuffer =
{
    .pBuffer = ucSharedBuffer,
    .size    = democonfigNETWORK_BUFFER_SIZE
};


/**
 * @brief Packet Identifier generated when Subscribe request was sent to the broker;
 * it is used to match received Subscribe ACK to the transmitted subscribe.
 */
static uint16_t globalSubscribePacketIdentifier = 0U;

/**
 * @brief Packet Identifier generated when Unsubscribe request was sent to the broker;
 * it is used to match received Unsubscribe ACK to the transmitted unsubscribe
 * request.
 */
static uint16_t globalUnsubscribePacketIdentifier = 0U;

/**
 * @brief Packet Identifier generated when Unsubscribe request was sent to the broker;
 * it is used to match received Unsubscribe ACK to the transmitted unsubscribe
 * request.
 */
static uint16_t globalPublishPacketIdentifier = 0U;

/**
 * @brief The flag to indicate pubAck packet is received.
 */
static volatile bool pubAckReceived = false;

/**
 * @brief The flag to indicate the mqtt session changed.
 */
static bool mqttSessionEstablished = false;

/**
 * @brief Global entry time into the application to use as a reference timestamp
 * in the #prvGetTimeMs function. #prvGetTimeMs will always return the difference
 * between the current time and the global entry time. This will reduce the chances
 * of overflow for the 32 bit unsigned integer used for holding the timestamp.
 */
static uint32_t ulGlobalEntryTimeMs;

/**
 * @brief The simulated device current power on state.
 */
static uint32_t ulCurrentPowerOnState = 0U;

/**
 * @brief The flag to indicate the device current power on state changed.
 */
static bool stateChanged = false;

/**
 * @brief When we send an update to the device shadow, and if we care about
 * the response from cloud (accepted/rejected), remember the clientToken and
 * use it to match with the response.
 */
static uint32_t ulClientToken = 0U;


/**
 * @brief The return status of prvUpdateDeltaHandler callback function.
 */
static int32_t lUpdateDeltaReturn = EXIT_SUCCESS;

/**
 * @brief The return status of prvUpdateAcceptedHandler callback function.
 */
static int32_t lUpdateAcceptedReturn = EXIT_SUCCESS;


/**
 * @brief The timer query function provided to the MQTT context.
 *
 * @return Time in milliseconds.
 */
static uint32_t prvGetTimeMs( void );

/**
 * @brief This example uses the MQTT library of the AWS IoT Device SDK for
 * Embedded C. This is the prototype of the callback function defined by
 * that library. It will be invoked whenever the MQTT library receives an
 * incoming message.
 *
 * @param[in] pxMqttContext MQTT context pointer.
 * @param[in] pxPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] pxDeserializedInfo Deserialized information from the incoming packet.
 */
static void prvEventCallback( MQTTContext_t * pxMqttContext,
                              MQTTPacketInfo_t * pxPacketInfo,
                              MQTTDeserializedInfo_t * pxDeserializedInfo );

/**
 * @brief BLE data transfer channel callback.
 *
 * @param[in] event Event received from the data transfer channel.
 * @param[in] pChannel Pointer to the data transfer channel.
 * @param[in] context User context owned by the application.
 */
static void bleChannelCallback( IotBleDataTransferChannelEvent_t event,
                                IotBleDataTransferChannel_t * pChannel,
                                void * context );

/**
 * @brief Sets up BLE transport interface.
 *
 * @param[in] pTransportCtxt Context passed to transport interface from MQTT library.
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int32_t prvBLETransportInterfaceConnect( NetworkContext_t * pTransportCtxt );


/**
 * @brief Disconnect BLE transport interface.
 *
 * @param[in] pTransportCtxt Context passed to transport interface from MQTT library.
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int32_t prvBLETransportInterfaceDisconnect( NetworkContext_t * pTransportCtxt );

/**
 * @brief Callback used to handle MQTT control packets.
 *
 * @param[in] pxPacketInfo Pointer to the MQTT packet information.
 * @param[in] usPacketIdentifier Packet identifier for the packet.
 */
static void prvHandleMQTTControlPacket( MQTTPacketInfo_t * pxPacketInfo,
                                        uint16_t usPacketIdentifier );

/**
 * @brief Callback generated for delta information in shadow document.
 *
 * @param[in] pxPublishInfo Publish information for the shadow delta.
 */
static void prvUpdateDeltaHandler( MQTTPublishInfo_t * pxPublishInfo );

/**
 * @brief Callback generated for shadow document update acknowledgement.
 *
 * @param[in] pxPublishInfo Publish information for the acknowledgement.
 */
static void prvUpdateAcceptedHandler( MQTTPublishInfo_t * pxPublishInfo );

/**
 * @brief Setups underlying BLE transport interface connection.
 * Sends an MQTT connect to the broker.
 *
 * @param[in] pxNetworkContext Context for the underlying network connection.
 * @param[in] pxMqttContext Context used by the MQTT library.
 * @param[in] Eventcallback Callback to be invoked for MQTT packets.
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int32_t prvEstablishMqttSession( NetworkContext_t * pxNetworkContext,
                                        MQTTContext_t * pxMqttContext,
                                        MQTTEventCallback_t eventCallback );


/**
 * @brief Sends disconnect packet to the broker if already connected. Tearsdown the
 * underlying network connection.
 *
 * @param[in] pxMqttContext Context used by the MQTT library.
 * @param[in] pxNetworkContext Context for the underlying network connection.
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int32_t prvDisconnectMqttSession( MQTTContext_t * pxMqttContext,
                                         NetworkContext_t * pxNetworkContext );

/**
 * @brief Subscribes to a topic filter with QOS 1 value.
 *
 * @param[in] pxMqttContext Context used by the MQTT library.
 * @param[in] pcTopicFilter Topic filter string.
 * @param[in] usTopicFilterLength topic filter string length.
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int32_t prvSubscribeToTopic( MQTTContext_t * pxMqttContext,
                                    const char * pcTopicFilter,
                                    uint16_t usTopicFilterLength );

/**
 * @brief Unsubscribes from a topic filter.
 *
 * @param[in] pxMqttContext Context used by the MQTT library.
 * @param[in] pcTopicFilter Topic filter string.
 * @param[in] usTopicFilterLength topic filter string length.
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int32_t prvUnsubscribeFromTopic( MQTTContext_t * pxMqttContext,
                                        const char * pcTopicFilter,
                                        uint16_t usTopicFilterLength );

/**
 * @brief Publishes given payload to an MQTT topic
 *
 * @param[in] pxMqttContext Context used by the MQTT library.
 * @param[in] pcTopicFilter Topic string.
 * @param[in] topicFilterLength topic string length.
 * @param[in] pcPayload Payload value.
 * @param[in] payloadLength length of the payload.
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int32_t prvPublishToTopic( MQTTContext_t * pxMqttContext,
                                  const char * pcTopicFilter,
                                  int32_t topicFilterLength,
                                  const char * pcPayload,
                                  size_t payloadLength );

/*--------------------------------------------------------------*/
static void bleChannelCallback( IotBleDataTransferChannelEvent_t event,
                                IotBleDataTransferChannel_t * pChannel,
                                void * context )
{
    /* Unused parameters. */
    ( void ) pChannel;
    ( void ) context;

    /* Event to see when the data channel is ready to receive data. */
    if( event == IOT_BLE_DATA_TRANSFER_CHANNEL_OPENED )
    {
        ( void ) xSemaphoreGive( xChannelSemaphore );
        channelActive = true;
    }

    /* Event for when data is received over the channel. */
    else if( event == IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED )
    {
        ( void ) IotBleMqttTransportAcceptData( &xBLETransportCtxt );
    }

    /* Event for when channel is closed. */
    else if( event == IOT_BLE_DATA_TRANSFER_CHANNEL_CLOSED )
    {
        channelActive = false;
    }

    else
    {
        /* Empty else, MISRA 2012 15.7 */
    }
}

/*-----------------------------------------------------------*/

static int32_t prvBLETransportInterfaceConnect( NetworkContext_t * pTransportCtxt )
{
    int32_t status = EXIT_SUCCESS;

    pTransportCtxt->pParams->pChannel = IotBleDataTransfer_Open( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT );

    if( pTransportCtxt->pParams->pChannel != NULL )
    {
        xChannelSemaphore = xSemaphoreCreateBinary();

        if( xChannelSemaphore != NULL )
        {
            ( void ) IotBleDataTransfer_SetCallback( pTransportCtxt->pParams->pChannel, bleChannelCallback, NULL );

            /* Open is a handshake proceture, so we need to wait until it is ready to use. */
            if( xSemaphoreTake( xChannelSemaphore, pdMS_TO_TICKS( IOT_BLE_MQTT_CREATE_CONNECTION_WAIT_MS ) ) == pdTRUE )
            {
                LogInfo( ( "The channel was initialized successfully" ) );
                status = EXIT_SUCCESS;
            }
            else
            {
                LogError( ( "The BLE Connection timed out" ) );
                status = EXIT_FAILURE;
            }
        }
        else
        {
            LogError( ( "Could not create BLE channel semaphore." ) );
            status = EXIT_FAILURE;
        }
    }

    if( status == EXIT_SUCCESS )
    {
        if( IotBleMqttTransportInit( ucTransportBuffer, democonfigBLE_TRANSPORT_BUFFER_SIZE, pTransportCtxt ) != true )
        {
            LogError( ( "Failed to create BLE transport interface." ) );
            status = EXIT_FAILURE;
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

static int32_t prvBLETransportInterfaceDisconnect( NetworkContext_t * pTransportCtxt )
{
    int32_t status = EXIT_SUCCESS;

    ( void ) IotBleMqttTransportCleanup( pTransportCtxt );
    ( void ) IotBleDataTransfer_Close( pTransportCtxt->pParams->pChannel );
    ( void ) IotBleDataTransfer_Reset( pTransportCtxt->pParams->pChannel );
    ( void ) vSemaphoreDelete( xChannelSemaphore );

    return status;
}

/*-----------------------------------------------------------*/

static void prvHandleMQTTControlPacket( MQTTPacketInfo_t * pxPacketInfo,
                                        uint16_t usPacketIdentifier )
{
    /* Handle other packets. */
    switch( pxPacketInfo->type )
    {
        case MQTT_PACKET_TYPE_SUBACK:
            LogInfo( ( "MQTT_PACKET_TYPE_SUBACK.\n\n" ) );
            /* Make sure ACK packet identifier matches with Request packet identifier. */
            assert( globalSubscribePacketIdentifier == usPacketIdentifier );
            break;

        case MQTT_PACKET_TYPE_UNSUBACK:
            LogInfo( ( "MQTT_PACKET_TYPE_UNSUBACK.\n\n" ) );
            /* Make sure ACK packet identifier matches with Request packet identifier. */
            assert( globalUnsubscribePacketIdentifier == usPacketIdentifier );
            break;

        case MQTT_PACKET_TYPE_PINGRESP:

            /* Nothing to be done from application as library handles
             * PINGRESP. */
            LogWarn( ( "PINGRESP should not be handled by the application "
                       "callback when using MQTT_ProcessLoop.\n\n" ) );
            break;

        case MQTT_PACKET_TYPE_PUBACK:
            LogInfo( ( "PUBACK received for packet id %u.\n\n",
                       usPacketIdentifier ) );
            assert( globalPublishPacketIdentifier == usPacketIdentifier );
            pubAckReceived = true;
            break;

        /* Any other packet type is invalid. */
        default:
            LogError( ( "Unknown packet type received:(%02x).\n\n",
                        pxPacketInfo->type ) );
    }
}

static void prvUpdateDeltaHandler( MQTTPublishInfo_t * pxPublishInfo )
{
    static uint32_t ulCurrentVersion = 0; /* Remember the latestVersion # we've ever received */
    uint32_t ulVersion = 0U;
    uint32_t ulNewState = 0U;
    char * pcOutValue = NULL;
    uint32_t ulOutValueLength = 0U;
    JSONStatus_t result = JSONSuccess;

    assert( pxPublishInfo != NULL );
    assert( pxPublishInfo->pPayload != NULL );

    LogInfo( ( "/update/delta json payload:%s.", ( const char * ) pxPublishInfo->pPayload ) );

    /* The payload will look similar to this:
     * {
     *      "version": 12,
     *      "timestamp": 1595437367,
     *      "state": {
     *          "powerOn": 1
     *      },
     *      "metadata": {
     *          "powerOn": {
     *          "timestamp": 1595437367
     *          }
     *      },
     *      "clientToken": "388062"
     *  }
     */

    /* Make sure the payload is a valid json document. */
    result = JSON_Validate( pxPublishInfo->pPayload,
                            pxPublishInfo->payloadLength );

    if( result == JSONSuccess )
    {
        /* Then we start to get the version value by JSON keyword "version". */
        result = JSON_Search( ( char * ) pxPublishInfo->pPayload,
                              pxPublishInfo->payloadLength,
                              "version",
                              sizeof( "version" ) - 1,
                              &pcOutValue,
                              ( size_t * ) &ulOutValueLength );
    }
    else
    {
        LogError( ( "The json document is invalid!!" ) );
    }

    if( result == JSONSuccess )
    {
        LogInfo( ( "version: %.*s",
                   ulOutValueLength,
                   pcOutValue ) );

        /* Convert the extracted value to an unsigned integer value. */
        ulVersion = ( uint32_t ) strtoul( pcOutValue, NULL, 10 );
    }
    else
    {
        LogError( ( "No version in json document!!" ) );
    }

    LogInfo( ( "version:%d, ulCurrentVersion:%d \r\n", ulVersion, ulCurrentVersion ) );

    /* When the version is much newer than the on we retained, that means the powerOn
     * state is valid for us. */
    if( ulVersion > ulCurrentVersion )
    {
        /* Set to received version as the current version. */
        ulCurrentVersion = ulVersion;

        /* Get powerOn state from json documents. */
        result = JSON_Search( ( char * ) pxPublishInfo->pPayload,
                              pxPublishInfo->payloadLength,
                              "state.powerOn",
                              sizeof( "state.powerOn" ) - 1,
                              &pcOutValue,
                              ( size_t * ) &ulOutValueLength );
    }
    else
    {
        /* In this demo, we discard the incoming message
         * if the version number is not newer than the latest
         * that we've received before. Your application may use a
         * different approach.
         */
        LogWarn( ( "The received version is smaller than current one!!" ) );
    }

    if( result == JSONSuccess )
    {
        /* Convert the powerOn state value to an unsigned integer value. */
        ulNewState = ( uint32_t ) strtoul( pcOutValue, NULL, 10 );

        LogInfo( ( "The new power on state newState:%d, ulCurrentPowerOnState:%d \r\n",
                   ulNewState, ulCurrentPowerOnState ) );

        if( ulNewState != ulCurrentPowerOnState )
        {
            /* The received powerOn state is different from the one we retained before, so we switch them
             * and set the flag. */
            ulCurrentPowerOnState = ulNewState;

            /* State change will be handled in main(), where we will publish a "reported"
             * state to the device shadow. We do not do it here because we are inside of
             * a callback from the MQTT library, so that we don't re-enter
             * the MQTT library. */
            stateChanged = true;
        }
    }
    else
    {
        LogError( ( "No powerOn in json document!!" ) );
        lUpdateDeltaReturn = EXIT_FAILURE;
    }
}

/*-----------------------------------------------------------*/

static void prvUpdateAcceptedHandler( MQTTPublishInfo_t * pxPublishInfo )
{
    char * pcOutValue = NULL;
    uint32_t ulOutValueLength = 0U;
    uint32_t ulReceivedToken = 0U;
    JSONStatus_t result = JSONSuccess;

    assert( pxPublishInfo != NULL );
    assert( pxPublishInfo->pPayload != NULL );

    LogInfo( ( "/update/accepted json payload:%s.", ( const char * ) pxPublishInfo->pPayload ) );

    /* Handle the reported state with state change in /update/accepted topic.
     * Thus we will retrieve the client token from the json document to see if
     * it's the same one we sent with reported state on the /update topic.
     * The payload will look similar to this:
     *  {
     *      "state": {
     *          "reported": {
     *          "powerOn": 1
     *          }
     *      },
     *      "metadata": {
     *          "reported": {
     *          "powerOn": {
     *              "timestamp": 1596573647
     *          }
     *          }
     *      },
     *      "version": 14698,
     *      "timestamp": 1596573647,
     *      "clientToken": "022485"
     *  }
     */

    /* Make sure the payload is a valid json document. */
    result = JSON_Validate( pxPublishInfo->pPayload,
                            pxPublishInfo->payloadLength );

    if( result == JSONSuccess )
    {
        /* Get clientToken from json documents. */
        result = JSON_Search( ( char * ) pxPublishInfo->pPayload,
                              pxPublishInfo->payloadLength,
                              "clientToken",
                              sizeof( "clientToken" ) - 1,
                              &pcOutValue,
                              ( size_t * ) &ulOutValueLength );
    }
    else
    {
        LogError( ( "Invalid json documents !!" ) );
    }

    if( result == JSONSuccess )
    {
        LogInfo( ( "clientToken: %.*s", ulOutValueLength,
                   pcOutValue ) );

        /* Convert the code to an unsigned integer value. */
        ulReceivedToken = ( uint32_t ) strtoul( pcOutValue, NULL, 10 );

        LogInfo( ( "receivedToken:%d, clientToken:%u \r\n", ulReceivedToken, ulClientToken ) );

        /* If the clientToken in this update/accepted message matches the one we
         * published before, it means the device shadow has accepted our latest
         * reported state. We are done. */
        if( ulReceivedToken == ulClientToken )
        {
            LogInfo( ( "Received response from the device shadow. Previously published "
                       "update with clientToken=%u has been accepted. ", ulClientToken ) );
        }
        else
        {
            LogWarn( ( "The received clientToken=%u is not identical with the one=%u we sent ",
                       ulReceivedToken, ulClientToken ) );
        }
    }
    else
    {
        LogError( ( "No clientToken in json document!!" ) );
        lUpdateAcceptedReturn = EXIT_FAILURE;
    }
}


/*-----------------------------------------------------------*/

/*
 * This is the callback function invoked by the MQTT stack when it receives
 * incoming messages. This function demonstrates how to use the Shadow_MatchTopic
 * function to determine whether the incoming message is a device shadow message
 * or not. If it is, it handles the message depending on the message type.
 */
static void prvEventCallback( MQTTContext_t * pxMqttContext,
                              MQTTPacketInfo_t * pxPacketInfo,
                              MQTTDeserializedInfo_t * pxDeserializedInfo )
{
    ShadowMessageType_t messageType = ShadowMessageTypeMaxNum;
    const char * pcThingName = NULL;
    uint16_t usThingNameLength = 0U;
    uint16_t usPacketIdentifier;

    ( void ) pxMqttContext;

    assert( pxDeserializedInfo != NULL );
    assert( pxMqttContext != NULL );
    assert( pxPacketInfo != NULL );

    usPacketIdentifier = pxDeserializedInfo->packetIdentifier;

    /* Handle incoming publish. The lower 4 bits of the publish packet
     * type is used for the dup, QoS, and retain flags. Hence masking
     * out the lower bits to check if the packet is publish. */
    if( ( pxPacketInfo->type & 0xF0U ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        assert( pxDeserializedInfo->pPublishInfo != NULL );
        LogInfo( ( "pPublishInfo->pTopicName:%s.", pxDeserializedInfo->pPublishInfo->pTopicName ) );

        /* Let the Device Shadow library tell us whether this is a device shadow message. */
        if( SHADOW_SUCCESS == Shadow_MatchTopic( pxDeserializedInfo->pPublishInfo->pTopicName,
                                                 pxDeserializedInfo->pPublishInfo->topicNameLength,
                                                 &messageType,
                                                 &pcThingName,
                                                 &usThingNameLength ) )
        {
            /* Upon successful return, the messageType has been filled in. */
            if( messageType == ShadowMessageTypeUpdateDelta )
            {
                /* Handler function to process payload. */
                prvUpdateDeltaHandler( pxDeserializedInfo->pPublishInfo );
            }
            else if( messageType == ShadowMessageTypeUpdateAccepted )
            {
                /* Handler function to process payload. */
                prvUpdateAcceptedHandler( pxDeserializedInfo->pPublishInfo );
            }
            else if( messageType == ShadowMessageTypeUpdateDocuments )
            {
                LogInfo( ( "/update/documents json payload:%s.", ( const char * ) pxDeserializedInfo->pPublishInfo->pPayload ) );
            }
            else if( messageType == ShadowMessageTypeUpdateRejected )
            {
                LogInfo( ( "/update/rejected json payload:%s.", ( const char * ) pxDeserializedInfo->pPublishInfo->pPayload ) );
            }
            else
            {
                LogInfo( ( "Other message type:%d !!", messageType ) );
            }
        }
        else
        {
            LogError( ( "Shadow_MatchTopic parse failed:%s !!", ( const char * ) pxDeserializedInfo->pPublishInfo->pTopicName ) );
        }
    }
    else
    {
        prvHandleMQTTControlPacket( pxPacketInfo, usPacketIdentifier );
    }
}

/*-----------------------------------------------------------*/

static int32_t prvEstablishMqttSession( NetworkContext_t * pxNetworkContext,
                                        MQTTContext_t * pxMqttContext,
                                        MQTTEventCallback_t eventCallback )
{
    MQTTStatus_t eMqttStatus;
    MQTTConnectInfo_t xConnectInfo;
    TransportInterface_t xTransport;
    bool sessionPresent = false;
    int32_t returnStatus;

    assert( pxMqttContext != NULL );
    assert( pxNetworkContext != NULL );

    /* Initialize the mqtt context and network context. */
    ( void ) memset( pxMqttContext, 0U, sizeof( MQTTContext_t ) );
    ( void ) memset( pxNetworkContext, 0U, sizeof( NetworkContext_t ) );

    xBLETransportCtxt.pParams = &xBleTransportParams;
    returnStatus = prvBLETransportInterfaceConnect( pxNetworkContext );

    if( returnStatus != EXIT_SUCCESS )
    {
        /* Log error to indicate connection failure after all
         * reconnect attempts are over. */
        LogError( ( "Failed to establish BLE transport interface connection." ) );
    }
    else
    {
        /* Fill in Transport Interface send and receive function pointers. */
        xTransport.pNetworkContext = pxNetworkContext;
        xTransport.send = IotBleMqttTransportSend;
        xTransport.recv = IotBleMqttTransportReceive;

        /* Initialize MQTT library. */
        eMqttStatus = MQTT_Init( pxMqttContext,
                                 &xTransport,
                                 prvGetTimeMs,
                                 eventCallback,
                                 &xBuffer );

        if( eMqttStatus != MQTTSuccess )
        {
            returnStatus = EXIT_FAILURE;
            LogError( ( "MQTT init failed with status %u.", eMqttStatus ) );
        }
        else
        {
            /* Establish MQTT session by sending a CONNECT packet. */

            /* Many fields not used in this demo so start with everything at 0. */
            ( void ) memset( ( void * ) &xConnectInfo, 0x00, sizeof( xConnectInfo ) );

            /* Start with a clean session i.e. direct the MQTT broker to discard any
             * previous session data. Also, establishing a connection with clean session
             * will ensure that the broker does not store any data when this client
             * gets disconnected. */
            xConnectInfo.cleanSession = true;

            /* The client identifier is used to uniquely identify this MQTT client to
             * the MQTT broker. In a production device the identifier can be something
             * unique, such as a device serial number. */
            xConnectInfo.pClientIdentifier = clientcredentialIOT_THING_NAME;
            xConnectInfo.clientIdentifierLength = ( uint16_t ) strlen( clientcredentialIOT_THING_NAME );

            /* The maximum time interval in seconds which is allowed to elapse
             * between two Control Packets.
             * It is the responsibility of the Client to ensure that the interval between
             * Control Packets being sent does not exceed the this Keep Alive value. In the
             * absence of sending any other Control Packets, the Client MUST send a
             * PINGREQ Packet. */
            xConnectInfo.keepAliveSeconds = mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS;

            /* Send MQTT CONNECT packet to broker. */
            eMqttStatus = MQTT_Connect( pxMqttContext,
                                        &xConnectInfo,
                                        NULL,
                                        mqttexampleCONNACK_RECV_TIMEOUT_MS,
                                        &sessionPresent );

            if( eMqttStatus != MQTTSuccess )
            {
                returnStatus = EXIT_FAILURE;
                LogError( ( "Connection with MQTT broker failed with status %u.", eMqttStatus ) );
            }
            else
            {
                LogInfo( ( "MQTT connection successfully established with broker.\n\n" ) );
            }
        }

        if( returnStatus == EXIT_SUCCESS )
        {
            /* Keep a flag for indicating if MQTT session is established. This
             * flag will mark that an MQTT DISCONNECT has to be sent at the end
             * of the demo even if there are intermediate failures. */
            mqttSessionEstablished = true;
        }
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/

static int32_t prvDisconnectMqttSession( MQTTContext_t * pxMqttContext,
                                         NetworkContext_t * pxNetworkContext )
{
    MQTTStatus_t eMqttStatus = MQTTSuccess;
    int returnStatus = EXIT_SUCCESS;

    assert( pxMqttContext != NULL );
    assert( pxNetworkContext != NULL );

    if( mqttSessionEstablished == true )
    {
        /* Send DISCONNECT. */
        eMqttStatus = MQTT_Disconnect( pxMqttContext );

        if( eMqttStatus != MQTTSuccess )
        {
            LogError( ( "Sending MQTT DISCONNECT failed with status=%u.",
                        eMqttStatus ) );
            returnStatus = EXIT_FAILURE;
        }
    }

    /* Close the BLE network connection.  */
    returnStatus = prvBLETransportInterfaceDisconnect( pxNetworkContext );

    return returnStatus;
}

/*-----------------------------------------------------------*/

static int32_t prvSubscribeToTopic( MQTTContext_t * pxMqttContext,
                                    const char * pcTopicFilter,
                                    uint16_t usTopicFilterLength )
{
    int returnStatus = EXIT_SUCCESS;
    MQTTStatus_t eMqttStatus;
    MQTTSubscribeInfo_t pSubscriptionList[ 1 ];

    assert( pcTopicFilter != NULL );
    assert( usTopicFilterLength > 0 );

    /* Start with everything at 0. */
    ( void ) memset( ( void * ) pSubscriptionList, 0x00, sizeof( pSubscriptionList ) );

    /* This example subscribes to only one topic and uses QOS1. */
    pSubscriptionList[ 0 ].qos = MQTTQoS1;
    pSubscriptionList[ 0 ].pTopicFilter = pcTopicFilter;
    pSubscriptionList[ 0 ].topicFilterLength = usTopicFilterLength;

    /* Generate packet identifier for the SUBSCRIBE packet. */
    globalSubscribePacketIdentifier = MQTT_GetPacketId( pxMqttContext );

    /* Send SUBSCRIBE packet. */
    eMqttStatus = MQTT_Subscribe( pxMqttContext,
                                  pSubscriptionList,
                                  sizeof( pSubscriptionList ) / sizeof( MQTTSubscribeInfo_t ),
                                  globalSubscribePacketIdentifier );

    if( eMqttStatus != MQTTSuccess )
    {
        LogError( ( "Failed to send SUBSCRIBE packet to broker with error = %u.",
                    eMqttStatus ) );
        returnStatus = EXIT_FAILURE;
    }
    else
    {
        LogInfo( ( "SUBSCRIBE topic %.*s to broker.\n\n",
                   usTopicFilterLength,
                   pcTopicFilter ) );

        /* Process incoming packet from the broker. Acknowledgment for subscription
         * ( SUBACK ) will be received here. However after sending the subscribe, the
         * client may receive a publish before it receives a subscribe ack. Since this
         * demo is subscribing to the topic to which no one is publishing, probability
         * of receiving publish message before subscribe ack is zero; but application
         * must be ready to receive any packet. This demo uses MQTT_ProcessLoop to
         * receive packet from network. */
        eMqttStatus = MQTT_ProcessLoop( pxMqttContext, mqttexamplePROCESS_LOOP_TIMEOUT_MS );

        if( eMqttStatus != MQTTSuccess )
        {
            returnStatus = EXIT_FAILURE;
            LogError( ( "MQTT_ProcessLoop returned with status = %u.",
                        eMqttStatus ) );
        }
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/

static int32_t prvUnsubscribeFromTopic( MQTTContext_t * pxMqttContext,
                                        const char * pcTopicFilter,
                                        uint16_t usTopicFilterLength )
{
    int returnStatus = EXIT_SUCCESS;
    MQTTStatus_t eMqttStatus;
    MQTTSubscribeInfo_t pSubscriptionList[ 1 ];

    assert( pcTopicFilter != NULL );
    assert( usTopicFilterLength > 0 );

    /* Start with everything at 0. */
    ( void ) memset( ( void * ) pSubscriptionList, 0x00, sizeof( pSubscriptionList ) );

    /* This example subscribes to only one topic and uses QOS1. */
    pSubscriptionList[ 0 ].qos = MQTTQoS1;
    pSubscriptionList[ 0 ].pTopicFilter = pcTopicFilter;
    pSubscriptionList[ 0 ].topicFilterLength = usTopicFilterLength;

    /* Generate packet identifier for the UNSUBSCRIBE packet. */
    globalUnsubscribePacketIdentifier = MQTT_GetPacketId( pxMqttContext );

    /* Send UNSUBSCRIBE packet. */
    eMqttStatus = MQTT_Unsubscribe( pxMqttContext,
                                    pSubscriptionList,
                                    sizeof( pSubscriptionList ) / sizeof( MQTTSubscribeInfo_t ),
                                    globalUnsubscribePacketIdentifier );

    if( eMqttStatus != MQTTSuccess )
    {
        LogError( ( "Failed to send UNSUBSCRIBE packet to broker with error = %u.",
                    eMqttStatus ) );
        returnStatus = EXIT_FAILURE;
    }
    else
    {
        LogInfo( ( "UNSUBSCRIBE sent topic %.*s to broker.\n\n",
                   usTopicFilterLength,
                   pcTopicFilter ) );

        /* Process incoming packet from the broker. Acknowledgment for unsubscribe operation
         * ( UNSUBACK ) will be received here. However after sending the subscribe, the
         * client may receive a publish before it receives a subscribe ack. Since this
         * demo is subscribing to the topic to which no one is publishing, probability
         * of receiving publish message before subscribe ack is zero; but application
         * must be ready to receive any packet. This demo uses MQTT_ProcessLoop to
         * receive packet from network. */
        eMqttStatus = MQTT_ProcessLoop( pxMqttContext, mqttexamplePROCESS_LOOP_TIMEOUT_MS );

        if( eMqttStatus != MQTTSuccess )
        {
            returnStatus = EXIT_FAILURE;
            LogError( ( "MQTT_ProcessLoop returned with status = %u.",
                        eMqttStatus ) );
        }
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/

static int32_t prvPublishToTopic( MQTTContext_t * pxMqttContext,
                                  const char * pcTopicFilter,
                                  int32_t topicFilterLength,
                                  const char * pcPayload,
                                  size_t payloadLength )
{
    int returnStatus = EXIT_SUCCESS;
    MQTTStatus_t eMqttStatus = MQTTSuccess;
    MQTTPublishInfo_t xPublishInfo = { 0 };

    assert( pxMqttContext != NULL );
    assert( pcTopicFilter != NULL );
    assert( topicFilterLength > 0 );

    if( returnStatus == EXIT_FAILURE )
    {
        LogError( ( "Unable to find a free spot for outgoing PUBLISH message.\n\n" ) );
    }
    else
    {
        LogInfo( ( "the published payload:%s \r\n ", pcPayload ) );
        /* This example publishes to only one topic and uses QOS1. */
        xPublishInfo.qos = MQTTQoS1;
        xPublishInfo.pTopicName = pcTopicFilter;
        xPublishInfo.topicNameLength = topicFilterLength;
        xPublishInfo.pPayload = pcPayload;
        xPublishInfo.payloadLength = payloadLength;

        /* Get a new packet id. */
        globalPublishPacketIdentifier = MQTT_GetPacketId( pxMqttContext );

        pubAckReceived = false;

        /* Send PUBLISH packet. */
        eMqttStatus = MQTT_Publish( pxMqttContext,
                                    &xPublishInfo,
                                    globalPublishPacketIdentifier );

        if( eMqttStatus != MQTTSuccess )
        {
            LogError( ( "Failed to send PUBLISH packet to broker with error = %u.",
                        eMqttStatus ) );
            returnStatus = EXIT_FAILURE;
        }
        else
        {
            LogInfo( ( "PUBLISH sent for topic %.*s to broker with packet ID %u.\n\n",
                       topicFilterLength,
                       pcTopicFilter,
                       globalPublishPacketIdentifier ) );

            /* Calling MQTT_ProcessLoop to process incoming publishes or a puback.
             * This function also sends ping request to broker if MQTT_KEEP_ALIVE_INTERVAL_SECONDS
             * has expired since the last MQTT packet sent and receive
             * ping responses. */
            eMqttStatus = MQTT_ProcessLoop( &xMQTTContext, mqttexamplePROCESS_LOOP_TIMEOUT_MS );

            if( eMqttStatus != MQTTSuccess )
            {
                LogWarn( ( "MQTT_ProcessLoop returned with status = %u.",
                           eMqttStatus ) );
            }
            else
            {
                if( pubAckReceived == false )
                {
                    LogError( ( " MQTT PUBACK not received after %d milliseconds", mqttexamplePROCESS_LOOP_TIMEOUT_MS ) );
                    returnStatus = EXIT_FAILURE;
                }
            }
        }
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/

static uint32_t prvGetTimeMs( void )
{
    TickType_t xTickCount = 0;
    uint32_t ulTimeMs = 0UL;

    /* Get the current tick count. */
    xTickCount = xTaskGetTickCount();

    /* Convert the ticks to milliseconds. */
    ulTimeMs = ( uint32_t ) xTickCount * MILLISECONDS_PER_TICK;

    /* Reduce ulGlobalEntryTimeMs from obtained time so as to always return the
     * elapsed time in the application. */
    ulTimeMs = ( uint32_t ) ( ulTimeMs - ulGlobalEntryTimeMs );

    return ulTimeMs;
}

/*-----------------------------------------------------------*/

/**
 * @brief Entry point of shadow demo over BLE transport interface.
 *
 * This main function demonstrates how to use the macros provided by the
 * Device Shadow library to assemble strings for the MQTT topics defined
 * by AWS IoT Device Shadow. It uses these macros for topics to subscribe
 * to:
 * - SHADOW_TOPIC_STRING_UPDATE_DELTA for "$aws/things/thingName/shadow/update/delta"
 * - SHADOW_TOPIC_STRING_UPDATE_ACCEPTED for "$aws/things/thingName/shadow/update/accepted"
 * - SHADOW_TOPIC_STRING_UPDATE_REJECTED for "$aws/things/thingName/shadow/update/rejected"
 *
 * It also uses these macros for topics to publish to:
 * - SHADOW_TOPIC_STIRNG_DELETE for "$aws/things/thingName/shadow/delete"
 * - SHADOW_TOPIC_STRING_UPDATE for "$aws/things/thingName/shadow/update"
 *
 * The helper functions this demo uses for MQTT operations have internal
 * loops to process incoming messages.
 */

int RunShadowBLETransportDemo( bool awsIotMqttMode,
                               const char * pIdentifier,
                               void * pNetworkServerInfo,
                               void * pNetworkCredentialInfo,
                               const void * pNetworkInterface )
{
    int returnStatus = EXIT_SUCCESS;

    /* A buffer containing the update document. It has static duration to prevent
     * it from being placed on the call stack. */
    static char pcUpdateDocument[ SHADOW_REPORTED_JSON_LENGTH + 1 ] = { 0 };

    /* Remove compiler warnings about unused parameters. */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;

    returnStatus = prvEstablishMqttSession( &xBLETransportCtxt, &xMQTTContext, &prvEventCallback );

    if( returnStatus == EXIT_FAILURE )
    {
        /* Log error to indicate connection failure. */
        LogError( ( "Failed to connect to MQTT broker." ) );
    }
    else
    {
        /* First of all, try to delete any Shadow document in the cloud. */
        returnStatus = prvPublishToTopic( &xMQTTContext,
                                          SHADOW_TOPIC_STRING_DELETE( THING_NAME ),
                                          SHADOW_TOPIC_LENGTH_DELETE( THING_NAME_LENGTH ),
                                          pcUpdateDocument,
                                          0U );

        /* Then try to subscribe shadow topics. */
        if( returnStatus == EXIT_SUCCESS )
        {
            returnStatus = prvSubscribeToTopic( &xMQTTContext,
                                                SHADOW_TOPIC_STRING_UPDATE_DELTA( THING_NAME ),
                                                SHADOW_TOPIC_LENGTH_UPDATE_DELTA( THING_NAME_LENGTH ) );
        }

        if( returnStatus == EXIT_SUCCESS )
        {
            returnStatus = prvSubscribeToTopic( &xMQTTContext,
                                                SHADOW_TOPIC_STRING_UPDATE_ACCEPTED( THING_NAME ),
                                                SHADOW_TOPIC_LENGTH_UPDATE_ACCEPTED( THING_NAME_LENGTH ) );
        }

        if( returnStatus == EXIT_SUCCESS )
        {
            returnStatus = prvSubscribeToTopic( &xMQTTContext,
                                                SHADOW_TOPIC_STRING_UPDATE_REJECTED( THING_NAME ),
                                                SHADOW_TOPIC_LENGTH_UPDATE_REJECTED( THING_NAME_LENGTH ) );
        }

        /* This demo uses a constant #THING_NAME known at compile time therefore we can use macros to
         * assemble shadow topic strings.
         * If the thing name is known at run time, then we could use the API #Shadow_GetTopicString to
         * assemble shadow topic strings, here is the example for /update/delta:
         *
         * For /update/delta:
         *
         * #define SHADOW_TOPIC_MAX_LENGTH  (256U)
         *
         * ShadowStatus_t shadowStatus = SHADOW_STATUS_SUCCESS;
         * char cTopicBuffer[ SHADOW_TOPIC_MAX_LENGTH ] = { 0 };
         * uint16_t usBufferSize = SHADOW_TOPIC_MAX_LENGTH;
         * uint16_t usOutLength = 0;
         * const char * pcThingName = "TestThingName";
         * uint16_t usThingNameLength  = ( sizeof( pcThingName ) - 1U );
         *
         * shadowStatus = Shadow_GetTopicString( SHADOW_TOPIC_STRING_TYPE_UPDATE_DELTA,
         *                                       pcThingName,
         *                                       usThingNameLength,
         *                                       & ( cTopicBuffer[ 0 ] ),
         *                                       usBufferSize,
         *                                       & usOutLength );
         */

        /* Then we publish a desired state to the /update topic. Since we've deleted
         * the device shadow at the beginning of the demo, this will cause a delta message
         * to be published, which we have subscribed to.
         * In many real applications, the desired state is not published by
         * the device itself. But for the purpose of making this demo self-contained,
         * we publish one here so that we can receive a delta message later.
         */
        if( returnStatus == EXIT_SUCCESS )
        {
            /* Desired power on state . */
            LogInfo( ( "Send desired power state with 1." ) );

            ( void ) memset( pcUpdateDocument,
                             0x00,
                             sizeof( pcUpdateDocument ) );

            snprintf( pcUpdateDocument,
                      SHADOW_DESIRED_JSON_LENGTH + 1,
                      SHADOW_DESIRED_JSON,
                      ( int ) 1,
                      ( long unsigned ) ( xTaskGetTickCount() % 1000000 ) );

            returnStatus = prvPublishToTopic( &xMQTTContext,
                                              SHADOW_TOPIC_STRING_UPDATE( THING_NAME ),
                                              SHADOW_TOPIC_LENGTH_UPDATE( THING_NAME_LENGTH ),
                                              pcUpdateDocument,
                                              ( SHADOW_DESIRED_JSON_LENGTH + 1 ) );
        }

        if( returnStatus == EXIT_SUCCESS )
        {
            /* Note that PublishToTopic already called MQTT_ProcessLoop,
             * therefore responses may have been received and the prvEventCallback
             * may have been called, which may have changed the stateChanged flag.
             * Check if the state change flag has been modified or not. If it's modified,
             * then we publish reported state to update topic.
             */
            if( stateChanged == true )
            {
                /* Report the latest power state back to device shadow. */
                LogInfo( ( "Report to the state change: %d", ulCurrentPowerOnState ) );
                ( void ) memset( pcUpdateDocument,
                                 0x00,
                                 sizeof( pcUpdateDocument ) );

                /* Keep the client token in global variable used to compare if
                 * the same token in /update/accepted. */
                ulClientToken = ( xTaskGetTickCount() % 1000000 );

                snprintf( pcUpdateDocument,
                          SHADOW_REPORTED_JSON_LENGTH + 1,
                          SHADOW_REPORTED_JSON,
                          ( int ) ulCurrentPowerOnState,
                          ( long unsigned ) ulClientToken );

                returnStatus = prvPublishToTopic( &xMQTTContext,
                                                  SHADOW_TOPIC_STRING_UPDATE( THING_NAME ),
                                                  SHADOW_TOPIC_LENGTH_UPDATE( THING_NAME_LENGTH ),
                                                  pcUpdateDocument,
                                                  ( SHADOW_DESIRED_JSON_LENGTH + 1 ) );
            }
            else
            {
                LogInfo( ( "No change from /update/delta, unsubscribe all shadow topics and disconnect from MQTT.\r\n" ) );
            }
        }

        if( returnStatus == EXIT_SUCCESS )
        {
            LogInfo( ( "Start to unsubscribe shadow topics and disconnect from MQTT. \r\n" ) );

            returnStatus = prvUnsubscribeFromTopic( &xMQTTContext,
                                                    SHADOW_TOPIC_STRING_UPDATE_DELTA( THING_NAME ),
                                                    SHADOW_TOPIC_LENGTH_UPDATE_DELTA( THING_NAME_LENGTH ) );

            if( returnStatus != EXIT_SUCCESS )
            {
                LogError( ( "Failed to unsubscribe the topic %s",
                            SHADOW_TOPIC_STRING_UPDATE_DELTA( THING_NAME ) ) );
            }
        }

        if( returnStatus == EXIT_SUCCESS )
        {
            returnStatus = prvUnsubscribeFromTopic( &xMQTTContext,
                                                    SHADOW_TOPIC_STRING_UPDATE_ACCEPTED( THING_NAME ),
                                                    SHADOW_TOPIC_LENGTH_UPDATE_ACCEPTED( THING_NAME_LENGTH ) );

            if( returnStatus != EXIT_SUCCESS )
            {
                LogError( ( "Failed to unsubscribe the topic %s",
                            SHADOW_TOPIC_STRING_UPDATE_ACCEPTED( THING_NAME ) ) );
            }
        }

        if( returnStatus == EXIT_SUCCESS )
        {
            returnStatus = prvUnsubscribeFromTopic( &xMQTTContext,
                                                    SHADOW_TOPIC_STRING_UPDATE_REJECTED( THING_NAME ),
                                                    SHADOW_TOPIC_LENGTH_UPDATE_REJECTED( THING_NAME_LENGTH ) );

            if( returnStatus != EXIT_SUCCESS )
            {
                LogError( ( "Failed to unsubscribe the topic %s",
                            SHADOW_TOPIC_STRING_UPDATE_REJECTED( THING_NAME ) ) );
            }
        }

        /* The MQTT session is always disconnected, even there were prior failures. */
        returnStatus = prvDisconnectMqttSession( &xMQTTContext, &xBLETransportCtxt );

        /* This demo performs only Device Shadow operations. If matching the Shadow
         * MQTT topic fails or there are failure in parsing the received JSON document,
         * then this demo was not successful. */
        if( ( lUpdateAcceptedReturn != EXIT_SUCCESS ) || ( lUpdateDeltaReturn != EXIT_SUCCESS ) )
        {
            LogError( ( "Callback function failed." ) );
            returnStatus = EXIT_FAILURE;
        }
    }

    return returnStatus;
}

/*-----------------------------------------------------------*/
