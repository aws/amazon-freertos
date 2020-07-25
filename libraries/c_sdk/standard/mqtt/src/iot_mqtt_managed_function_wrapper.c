/*
 * FreeRTOS MQTT SHIM V1.1.1
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
 * @file iot_mqtt_managed_function_wrapper.c
 * @brief Wrapper to use the MQTT LTS transmit side API for
 * sending the packets on the network .
 */
/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* MQTT internal includes. */
#include "private/iot_mqtt_internal.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"

/* Using initialized connToContext variable. */
extern _connContext_t connToContext[ MAX_NO_OF_MQTT_CONNECTIONS ];

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_managedDisconnect( IotMqttConnection_t mqttConnection )
{
    int8_t contextIndex = -1;
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    /* Initializing MQTT Status. */
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;

    IotMqtt_Assert( mqttConnection != NULL );

    /* Getting MQTT Context for the specified MQTT Connection. */
    contextIndex = _IotMqtt_getContextIndexFromConnection( mqttConnection );

    if( contextIndex >= 0 )
    {
        IotMutex_Lock( &( connToContext[ contextIndex ].contextMutex ) );

        /* Calling MQTT LTS API for sending the DISCONNECT packet on the network. */
        managedMqttStatus = MQTT_Disconnect( &( connToContext[ contextIndex ].context ) );

        IotMutex_Unlock( &( connToContext[ contextIndex ].contextMutex ) );

        /* Converting the status code. */
        status = convertReturnCode( managedMqttStatus );
    }

    return status;
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_managedSubscribe( IotMqttConnection_t mqttConnection,
                                          _mqttOperation_t * pSubscriptionOperation,
                                          const IotMqttSubscription_t * pSubscriptionList,
                                          size_t subscriptionCount )
{
    int8_t contextIndex = -1;
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    /* Initializing MQTT Status. */
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    uint16_t packetId = 0;
    MQTTSubscribeInfo_t * subscriptionList = IotMqtt_MallocMessage( sizeof( MQTTSubscribeInfo_t ) * subscriptionCount );

    IotMqtt_Assert( mqttConnection != NULL );
    IotMqtt_Assert( pSubscriptionOperation != NULL );
    IotMqtt_Assert( pSubscriptionList != NULL );

    /* Getting MQTT Context for the specified MQTT Connection. */
    contextIndex = _IotMqtt_getContextIndexFromConnection( mqttConnection );

    if( contextIndex >= 0 )
    {
        /* Generating the packet id for SUBSCRIBE packet. */
        packetId = _nextPacketIdentifier();

        pSubscriptionOperation->u.operation.packetIdentifier = packetId;

        /* Populating the subscription list to be used by MQTT LTS API. */
        for( size_t i = 0; i < subscriptionCount; i++ )
        {
            subscriptionList[ i ].qos = ( MQTTQoS_t ) ( pSubscriptionList + i )->qos;
            subscriptionList[ i ].pTopicFilter = ( pSubscriptionList + i )->pTopicFilter;
            subscriptionList[ i ].topicFilterLength = ( pSubscriptionList + i )->topicFilterLength;
        }

        IotMutex_Lock( &( connToContext[ contextIndex ].contextMutex ) );

        /* Calling MQTT LTS API for sending the SUBSCRIBE packet on the network. */
        managedMqttStatus = MQTT_Subscribe( &( connToContext[ contextIndex ].context ), subscriptionList, subscriptionCount, packetId );

        IotMutex_Unlock( &( connToContext[ contextIndex ].contextMutex ) );

        /* Converting the status code. */
        status = convertReturnCode( managedMqttStatus );
    }

    return status;
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_managedUnsubscribe( IotMqttConnection_t mqttConnection,
                                            _mqttOperation_t * pSubscriptionOperation,
                                            const IotMqttSubscription_t * pSubscriptionList,
                                            size_t subscriptionCount )
{
    int8_t contextIndex = -1;
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    /* Initializing MQTT Status. */
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    uint16_t packetId = 0;
    MQTTSubscribeInfo_t * subscriptionList = IotMqtt_MallocMessage( sizeof( MQTTSubscribeInfo_t ) * subscriptionCount );

    IotMqtt_Assert( mqttConnection != NULL );
    IotMqtt_Assert( pSubscriptionOperation != NULL );
    IotMqtt_Assert( pSubscriptionList != NULL );

    /* Getting MQTT Context for the specified MQTT Connection. */
    contextIndex = _IotMqtt_getContextIndexFromConnection( mqttConnection );

    if( contextIndex >= 0 )
    {
        /* Generating the packet id for UNSUBSCRIBE packet. */
        packetId = _nextPacketIdentifier();

        pSubscriptionOperation->u.operation.packetIdentifier = packetId;

        /* Populating the subscription list to be used by MQTT LTS API. */
        for( size_t i = 0; i < subscriptionCount; i++ )
        {
            subscriptionList[ i ].qos = ( MQTTQoS_t ) ( pSubscriptionList + i )->qos;
            subscriptionList[ i ].pTopicFilter = ( pSubscriptionList + i )->pTopicFilter;
            subscriptionList[ i ].topicFilterLength = ( pSubscriptionList + i )->topicFilterLength;
        }

        IotMutex_Lock( &( connToContext[ contextIndex ].contextMutex ) );

        /* Calling MQTT LTS API for sending the UNSUBSCRIBE packet on the network. */
        managedMqttStatus = MQTT_Unsubscribe( &( connToContext[ contextIndex ].context ), subscriptionList, subscriptionCount, packetId );

        IotMutex_Unlock( &( connToContext[ contextIndex ].contextMutex ) );

        /* Converting the status code. */
        status = convertReturnCode( managedMqttStatus );
    }

    return status;
}

/*-----------------------------------------------------------*/


IotMqttError_t _IotMqtt_managedPublish( IotMqttConnection_t mqttConnection,
                                        _mqttOperation_t * pOperation,
                                        const IotMqttPublishInfo_t * pPublishInfo )
{
    int8_t contextIndex = -1;
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    /* Initializing MQTT Status. */
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    uint16_t packetId = 0;
    MQTTPublishInfo_t publishInfo;

    IotMqtt_Assert( mqttConnection != NULL );
    IotMqtt_Assert( pOperation != NULL );
    IotMqtt_Assert( pPublishInfo != NULL );

    /* Getting MQTT Context for the specified MQTT Connection. */
    contextIndex = _IotMqtt_getContextIndexFromConnection( mqttConnection );

    if( contextIndex >= 0 )
    {
        /* Generating the packet id for PUBLISH packet. */
        packetId = _nextPacketIdentifier();
        pOperation->u.operation.packetIdentifier = packetId;

        /* Populating the publish info to be used by MQTT LTS PUBLISH API. */
        publishInfo.retain = pPublishInfo->retain;
        publishInfo.pTopicName = pPublishInfo->pTopicName;
        publishInfo.topicNameLength = pPublishInfo->topicNameLength;
        publishInfo.pPayload = pPublishInfo->pPayload;
        publishInfo.payloadLength = pPublishInfo->payloadLength;
        publishInfo.qos = ( MQTTQoS_t ) pPublishInfo->qos;

        if( pPublishInfo->qos == IOT_MQTT_QOS_1 )
        {
            publishInfo.dup = true;
        }
        else
        {
            publishInfo.dup = false;
        }

        IotMutex_Lock( &( connToContext[ contextIndex ].contextMutex ) );
        /* Calling MQTT LTS API for sending the PUBLISH packet on the network. */
        managedMqttStatus = MQTT_Publish( &( connToContext[ contextIndex ].context ), &publishInfo, packetId );
        IotMutex_Unlock( &( connToContext[ contextIndex ].contextMutex ) );

        /* Converting the status code. */
        status = convertReturnCode( managedMqttStatus );
    }

    return status;
}

/*-----------------------------------------------------------*/
