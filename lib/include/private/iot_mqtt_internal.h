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
 * @file iot_mqtt_internal.h
 * @brief Internal header of MQTT library. This header should not be included in
 * typical application code.
 */

#ifndef _IOT_MQTT_INTERNAL_H_
#define _IOT_MQTT_INTERNAL_H_

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Linear containers (lists and queues) include. */
#include "iot_linear_containers.h"

/* Platform layer types include. */
#include "types/iot_platform_types.h"

/* MQTT include. */
#include "iot_mqtt.h"

/* Task pool include. */
#include "iot_taskpool.h"

/**
 * @def IotMqtt_Assert( expression )
 * @brief Assertion macro for the MQTT library.
 *
 * Set @ref IOT_MQTT_ENABLE_ASSERTS to `1` to enable assertions in the MQTT
 * library.
 *
 * @param[in] expression Expression to be evaluated.
 */
#if IOT_MQTT_ENABLE_ASSERTS == 1
    #ifndef IotMqtt_Assert
        #include <assert.h>
        #define IotMqtt_Assert( expression )    assert( expression )
    #endif
#else
    #define IotMqtt_Assert( expression )
#endif

/* Configure logs for MQTT functions. */
#ifdef IOT_LOG_LEVEL_MQTT
    #define _LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_MQTT
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define _LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define _LIBRARY_LOG_LEVEL    IOT_LOG_NONE
    #endif
#endif

#define _LIBRARY_LOG_NAME    ( "MQTT" )
#include "iot_logging_setup.h"

/*
 * Provide default values for undefined memory allocation functions based on
 * the usage of dynamic memory allocation.
 */
#if IOT_STATIC_MEMORY_ONLY == 1
    #include "private/iot_static_memory.h"

/**
 * @brief Allocate an #_mqttConnection_t. This function should have the same
 * signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    #ifndef IotMqtt_MallocConnection
        #define IotMqtt_MallocConnection    Iot_MallocMqttConnection
    #endif

/**
 * @brief Free an #_mqttConnection_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    #ifndef IotMqtt_FreeConnection
        #define IotMqtt_FreeConnection    Iot_FreeMqttConnection
    #endif

/**
 * @brief Allocate memory for an MQTT packet. This function should have the
 * same signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    #ifndef IotMqtt_MallocMessage
        #define IotMqtt_MallocMessage    Iot_MallocMessageBuffer
    #endif

/**
 * @brief Free an MQTT packet. This function should have the same signature
 * as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    #ifndef IotMqtt_FreeMessage
        #define IotMqtt_FreeMessage    Iot_FreeMessageBuffer
    #endif

/**
 * @brief Allocate an #_mqttOperation_t. This function should have the same
 * signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    #ifndef IotMqtt_MallocOperation
        #define IotMqtt_MallocOperation    Iot_MallocMqttOperation
    #endif

/**
 * @brief Free an #_mqttOperation_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    #ifndef IotMqtt_FreeOperation
        #define IotMqtt_FreeOperation    Iot_FreeMqttOperation
    #endif

/**
 * @brief Allocate an #_mqttSubscription_t. This function should have the
 * same signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    #ifndef IotMqtt_MallocSubscription
        #define IotMqtt_MallocSubscription    Iot_MallocMqttSubscription
    #endif

/**
 * @brief Free an #_mqttSubscription_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    #ifndef IotMqtt_FreeSubscription
        #define IotMqtt_FreeSubscription    Iot_FreeMqttSubscription
    #endif
#else /* if IOT_STATIC_MEMORY_ONLY == 1 */
    #include <stdlib.h>

    #ifndef IotMqtt_MallocConnection
        #define IotMqtt_MallocConnection    malloc
    #endif

    #ifndef IotMqtt_FreeConnection
        #define IotMqtt_FreeConnection    free
    #endif

    #ifndef IotMqtt_MallocMessage
        #define IotMqtt_MallocMessage    malloc
    #endif

    #ifndef IotMqtt_FreeMessage
        #define IotMqtt_FreeMessage    free
    #endif

    #ifndef IotMqtt_MallocOperation
        #define IotMqtt_MallocOperation    malloc
    #endif

    #ifndef IotMqtt_FreeOperation
        #define IotMqtt_FreeOperation    free
    #endif

    #ifndef IotMqtt_MallocSubscription
        #define IotMqtt_MallocSubscription    malloc
    #endif

    #ifndef IotMqtt_FreeSubscription
        #define IotMqtt_FreeSubscription    free
    #endif
#endif /* if IOT_STATIC_MEMORY_ONLY == 1 */

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values for undefined configuration constants.
 */
#ifndef AWS_IOT_MQTT_ENABLE_METRICS
    #define AWS_IOT_MQTT_ENABLE_METRICS             ( 1 )
#endif
#ifndef IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES
    #define IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES    ( 0 )
#endif
#ifndef IOT_MQTT_TEST
    #define IOT_MQTT_TEST                           ( 0 )
#endif
#ifndef IOT_MQTT_RESPONSE_WAIT_MS
    #define IOT_MQTT_RESPONSE_WAIT_MS               ( 1000 )
#endif
#ifndef IOT_MQTT_RETRY_MS_CEILING
    #define IOT_MQTT_RETRY_MS_CEILING               ( 60000 )
#endif
/** @endcond */

/**
 * @brief Marks the empty statement of an `else` branch.
 *
 * Does nothing, but allows test coverage to detect branches not taken. By default,
 * this is defined to nothing. When running code coverage testing, this is defined
 * to an assembly NOP.
 */
#ifndef _EMPTY_ELSE_MARKER
    #define _EMPTY_ELSE_MARKER
#endif

/*
 * Constants related to limits defined in AWS Service Limits.
 *
 * For details, see
 * https://docs.aws.amazon.com/general/latest/gr/aws_service_limits.html
 *
 * Used to validate parameters if when connecting to an AWS IoT MQTT server.
 */
#define _AWS_IOT_MQTT_SERVER_MIN_KEEPALIVE                      ( 30 )   /**< @brief Minumum keep-alive interval accepted by AWS IoT. */
#define _AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE                      ( 1200 ) /**< @brief Maximum keep-alive interval accepted by AWS IoT. */
#define _AWS_IOT_MQTT_SERVER_MAX_CLIENTID                       ( 128 )  /**< @brief Maximum length of client identifier accepted by AWS IoT. */
#define _AWS_IOT_MQTT_SERVER_MAX_TOPIC_LENGTH                   ( 256 )  /**< @brief Maximum length of topic names or filters accepted by AWS IoT. */
#define _AWS_IOT_MQTT_SERVER_MAX_TOPIC_FILTERS_PER_SUBSCRIBE    ( 8 )    /**< @brief Maximum number of topic filters in a single SUBSCRIBE packet. */

/*
 * MQTT control packet type and flags. Always the first byte of an MQTT
 * packet.
 *
 * For details, see
 * http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/csprd02/mqtt-v3.1.1-csprd02.html#_Toc385349757
 */
#define _MQTT_PACKET_TYPE_CONNECT                               ( ( uint8_t ) 0x10U ) /**< @brief CONNECT (client-to-server). */
#define _MQTT_PACKET_TYPE_CONNACK                               ( ( uint8_t ) 0x20U ) /**< @brief CONNACK (server-to-client). */
#define _MQTT_PACKET_TYPE_PUBLISH                               ( ( uint8_t ) 0x30U ) /**< @brief PUBLISH (bi-directional). */
#define _MQTT_PACKET_TYPE_PUBACK                                ( ( uint8_t ) 0x40U ) /**< @brief PUBACK (server-to-client). */
#define _MQTT_PACKET_TYPE_SUBSCRIBE                             ( ( uint8_t ) 0x82U ) /**< @brief SUBSCRIBE (client-to-server). */
#define _MQTT_PACKET_TYPE_SUBACK                                ( ( uint8_t ) 0x90U ) /**< @brief SUBACK (server-to-client). */
#define _MQTT_PACKET_TYPE_UNSUBSCRIBE                           ( ( uint8_t ) 0xa2U ) /**< @brief UNSUBSCRIBE (client-to-server). */
#define _MQTT_PACKET_TYPE_UNSUBACK                              ( ( uint8_t ) 0xb0U ) /**< @brief UNSUBACK (server-to-client). */
#define _MQTT_PACKET_TYPE_PINGREQ                               ( ( uint8_t ) 0xc0U ) /**< @brief PINGREQ (client-to-server). */
#define _MQTT_PACKET_TYPE_PINGRESP                              ( ( uint8_t ) 0xd0U ) /**< @brief PINGRESP (server-to-client). */
#define _MQTT_PACKET_TYPE_DISCONNECT                            ( ( uint8_t ) 0xe0U ) /**< @brief DISCONNECT (client-to-server). */

/*---------------------- MQTT internal data structures ----------------------*/

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Forward declarations.
 */
struct _mqttSubscription;
struct _mqttConnection;
struct _mqttOperation;
struct _mqttTimerEvent;
/** @endcond */

/**
 * @brief Represents a subscription stored in an MQTT connection.
 */
typedef struct _mqttSubscription
{
    IotLink_t link;     /**< @brief List link member. */

    int32_t references; /**< @brief How many subscription callbacks are using this subscription. */

    /**
     * @brief Tracks whether @ref mqtt_function_unsubscribe has been called for
     * this subscription.
     *
     * If there are active subscription callbacks, @ref mqtt_function_unsubscribe
     * cannot remove this subscription. Instead, it will set this flag, which
     * schedules the removal of this subscription once all subscription callbacks
     * terminate.
     */
    bool unsubscribed;

    struct
    {
        uint16_t identifier;        /**< @brief Packet identifier. */
        size_t order;               /**< @brief Order in the packet's list of subscriptions. */
    } packetInfo;                   /**< @brief Information about the SUBSCRIBE packet that registered this subscription. */

    IotMqttCallbackInfo_t callback; /**< @brief Callback information for this subscription. */

    uint16_t topicFilterLength;     /**< @brief Length of #_mqttSubscription_t.pTopicFilter. */
    char pTopicFilter[];            /**< @brief The subscription topic filter. */
} _mqttSubscription_t;

/**
 * @brief Internal structure to hold data on an MQTT connection.
 */
typedef struct _mqttConnection
{
    bool awsIotMqttMode;               /**< @brief Specifies if this connection is to an AWS IoT MQTT server. */
    IotMqttNetIf_t network;            /**< @brief Network interface provided to @ref mqtt_function_connect. */

    bool disconnected;                 /**< @brief Tracks if this connection has been disconnected. */
    IotMutex_t referencesMutex;        /**< @brief Recursive mutex. Grants access to connection state and operation lists. */
    int32_t references;                /**< @brief Counts callbacks and operations using this connection. */
    IotListDouble_t pendingProcessing; /**< @brief List of operations waiting to be processed by a task pool routine. */
    IotListDouble_t pendingResponse;   /**< @brief List of processed operations awaiting a server response. */

    IotListDouble_t subscriptionList;  /**< @brief Holds subscriptions associated with this connection. */
    IotMutex_t subscriptionMutex;      /**< @brief Grants exclusive access to #_mqttConnection_t.subscriptionList. */

    bool keepAliveFailure;             /**< @brief Failure flag for keep-alive operation. */
    uint32_t keepAliveMs;              /**< @brief Keep-alive interval in milliseconds. Its max value (per spec) is 65,535,000. */
    uint64_t nextKeepAliveMs;          /**< @brief Relative delay for next keep-alive job. */
    IotTaskPoolJob_t keepAliveJob;     /**< @brief Task pool job for processing this connection's keep-alive. */
    uint8_t * pPingreqPacket;          /**< @brief An MQTT PINGREQ packet, allocated if keep-alive is active. */
    size_t pingreqPacketSize;          /**< @brief The size of an allocated PINGREQ packet. */
} _mqttConnection_t;

/**
 * @brief Internal structure representing a single MQTT operation, such as
 * CONNECT, SUBSCRIBE, PUBLISH, etc.
 *
 * Queues of these structures keeps track of all in-progress MQTT operations.
 */
typedef struct _mqttOperation
{
    /* Pointers to neighboring queue elements. */
    IotLink_t link;                           /**< @brief List link member. */

    bool incomingPublish;                     /**< @brief Set to true if this operation an incoming PUBLISH. */
    struct _mqttConnection * pMqttConnection; /**< @brief MQTT connection associated with this operation. */

    IotTaskPoolJob_t job;                     /**< @brief Task pool job associated with this operation. */

    union
    {
        /* If incomingPublish is false, this struct is valid. */
        struct
        {
            /* Basic operation information. */
            int32_t jobReference;             /**< @brief Tracks if a job is using this operation. Must always be 0, 1, or 2. */
            IotMqttOperationType_t operation; /**< @brief What operation this structure represents. */
            uint32_t flags;                   /**< @brief Flags passed to the function that created this operation. */
            uint16_t packetIdentifier;        /**< @brief The packet identifier used with this operation. */

            /* Serialized packet and size. */
            uint8_t * pMqttPacket; /**< @brief The MQTT packet to send over the network. */
            size_t packetSize;     /**< @brief Size of `pMqttPacket`. */

            /* How to notify of an operation's completion. */
            union
            {
                IotSemaphore_t waitSemaphore;   /**< @brief Semaphore to be used with @ref mqtt_function_wait. */
                IotMqttCallbackInfo_t callback; /**< @brief User-provided callback function and parameter. */
            } notify;                           /**< @brief How to notify of this operation's completion. */
            IotMqttError_t status;              /**< @brief Result of this operation. This is reported once a response is received. */

            struct
            {
                uint32_t count;
                uint32_t limit;
                uint64_t nextPeriod;
            } retry;
        };

        /* If incomingPublish is true, this struct is valid. */
        struct
        {
            IotMqttPublishInfo_t publishInfo;      /**< @brief Deserialized PUBLISH. */
            struct _mqttOperation * pNextPublish;  /**< @brief Pointer to the next PUBLISH in the data stream. */
            const void * pReceivedData;            /**< @brief Any buffer associated with this PUBLISH that should be freed. */
            void ( * freeReceivedData )( void * ); /**< @brief Function called to free `pReceivedData`. */
        };
    };
} _mqttOperation_t;

/* Declaration of the MQTT task pool for internal files. */
extern IotTaskPool_t _IotMqttTaskPool;

/*-------------------- MQTT struct validation functions ---------------------*/

/**
 * @brief Check that an #IotMqttNetIf_t is valid.
 *
 * @param[in] pNetworkInterface The #IotMqttNetIf_t to validate.
 *
 * @return `true` if `pNetworkInterface` is valid; `false` otherwise.
 */
bool _IotMqtt_ValidateNetIf( const IotMqttNetIf_t * pNetworkInterface );

/**
 * @brief Check that an #IotMqttConnectInfo_t is valid.
 *
 * @param[in] pConnectInfo The #IotMqttConnectInfo_t to validate.
 *
 * @return `true` if `pConnectInfo` is valid; `false` otherwise.
 */
bool _IotMqtt_ValidateConnect( const IotMqttConnectInfo_t * pConnectInfo );

/**
 * @brief Check that an #IotMqttPublishInfo_t is valid.
 *
 * @param[in] awsIotMqttMode Specifies if this PUBLISH packet is being sent to
 * an AWS IoT MQTT server.
 * @param[in] pPublishInfo The #IotMqttPublishInfo_t to validate.
 *
 * @return `true` if `pPublishInfo` is valid; `false` otherwise.
 */
bool _IotMqtt_ValidatePublish( bool awsIotMqttMode,
                               const IotMqttPublishInfo_t * pPublishInfo );

/**
 * @brief Check that an #IotMqttReference_t is valid and waitable.
 *
 * @param[in] reference The #IotMqttReference_t to validate.
 *
 * @return `true` if `reference` is valid; `false` otherwise.
 */
bool _IotMqtt_ValidateReference( IotMqttReference_t reference );

/**
 * @brief Check that a list of #IotMqttSubscription_t is valid.
 *
 * @param[in] operation Either #IOT_MQTT_SUBSCRIBE or #IOT_MQTT_UNSUBSCRIBE.
 * Some parameters are not validated for #IOT_MQTT_UNSUBSCRIBE.
 * @param[in] awsIotMqttMode Specifies if this SUBSCRIBE packet is being sent to
 * an AWS IoT MQTT server.
 * @param[in] pListStart First element of the list to validate.
 * @param[in] listSize Number of elements in the subscription list.
 *
 * @return `true` if every element in the list is valid; `false` otherwise.
 */
bool _IotMqtt_ValidateSubscriptionList( IotMqttOperationType_t operation,
                                        bool awsIotMqttMode,
                                        const IotMqttSubscription_t * pListStart,
                                        size_t listSize );

/*-------------------- MQTT packet serializer functions ---------------------*/

/**
 * @brief Initialize the MQTT packet serializer. Called by @ref mqtt_function_init
 * when initializing the MQTT library.
 *
 * @return #IOT_MQTT_SUCCESS or #IOT_MQTT_INIT_FAILED.
 */
IotMqttError_t _IotMqtt_InitSerialize( void );

/**
 * @brief Free resources taken by #_IotMqtt_InitSerialize.
 *
 * No parameters, no return values.
 */
void _IotMqtt_CleanupSerialize( void );

/**
 * @brief Get the MQTT packet type from a stream of bytes.
 *
 * @param[in] pPacket Pointer to the beginning of the byte stream.
 * @param[in] packetSize Size of the byte stream.
 *
 * @return One of the server-to-client MQTT packet types.
 *
 * @note This function is only used for incoming packets, and may not work
 * correctly for outgoing packets.
 */
uint8_t _IotMqtt_GetPacketType( const uint8_t * pPacket,
                                size_t packetSize );

/**
 * @brief Generate a CONNECT packet from the given parameters.
 *
 * @param[in] pConnectInfo User-provided CONNECT information.
 * @param[out] pConnectPacket Where the CONNECT packet is written.
 * @param[out] pPacketSize Size of the packet written to `pConnectPacket`.
 *
 * @return #IOT_MQTT_SUCCESS or #IOT_MQTT_NO_MEMORY.
 */
IotMqttError_t _IotMqtt_SerializeConnect( const IotMqttConnectInfo_t * pConnectInfo,
                                          uint8_t ** pConnectPacket,
                                          size_t * pPacketSize );

/**
 * @brief Deserialize a CONNACK packet.
 *
 * Converts the packet from a stream of bytes to an #IotMqttError_t. Also
 * prints out debug log messages about the packet.
 * @param[in] pConnackStart Pointer to the start of a CONNACK packet.
 * @param[in] dataLength Length of the data stream after `pConnackStart`.
 * @param[out] pBytesProcessed The number of bytes in the data stream processed
 * by this function. For CONNACK, this is always 4.
 *
 * @return #IOT_MQTT_SUCCESS if CONNACK specifies that CONNECT was accepted;
 * #IOT_MQTT_SERVER_REFUSED if CONNACK specifies that CONNECT was rejected;
 * #IOT_MQTT_BAD_RESPONSE if the CONNACK packet doesn't follow MQTT spec.
 */
IotMqttError_t _IotMqtt_DeserializeConnack( const uint8_t * pConnackStart,
                                            size_t dataLength,
                                            size_t * pBytesProcessed );

/**
 * @brief Generate a PUBLISH packet from the given parameters.
 *
 * @param[in] pPublishInfo User-provided PUBLISH information.
 * @param[out] pPublishPacket Where the PUBLISH packet is written.
 * @param[out] pPacketSize Size of the packet written to `pPublishPacket`.
 * @param[out] pPacketIdentifier The packet identifier generated for this PUBLISH.
 *
 * @return #IOT_MQTT_SUCCESS or #IOT_MQTT_NO_MEMORY.
 */
IotMqttError_t _IotMqtt_SerializePublish( const IotMqttPublishInfo_t * pPublishInfo,
                                          uint8_t ** pPublishPacket,
                                          size_t * pPacketSize,
                                          uint16_t * pPacketIdentifier );

/**
 * @brief Set the DUP bit in a QoS 1 PUBLISH packet.
 *
 * @param[in] awsIotMqttMode Specifies if this PUBLISH packet is being sent to
 * an AWS IoT MQTT server.
 * @param[in] pPublishPacket Pointer to the PUBLISH packet to modify.
 * @param[out] pNewPacketIdentifier Since AWS IoT does not support the DUP flag,
 * a new packet identifier is generated and should be written here. This parameter
 * is only used when connected to an AWS IoT MQTT server.
 *
 * @note See #IotMqttPublishInfo_t for caveats with retransmission to the
 * AWS IoT MQTT server.
 */
void _IotMqtt_PublishSetDup( bool awsIotMqttMode,
                             uint8_t * pPublishPacket,
                             uint16_t * pNewPacketIdentifier );

/**
 * @brief Deserialize a PUBLISH packet received from the server.
 *
 * Converts the packet from a stream of bytes to an #IotMqttPublishInfo_t and
 * extracts the packet identifier. Also prints out debug log messages about the
 * packet.
 *
 * @param[in] pPublishStart Pointer to the start of a PUBLISH packet.
 * @param[in] dataLength Length of the data stream after `pPublishStart`.
 * @param[out] pOutput Where the deserialized PUBLISH will be written.
 * @param[out] pPacketIdentifier The packet identifier in the PUBLISH.
 * @param[out] pBytesProcessed The number of bytes in the data stream processed
 * by this function.
 *
 * @return #IOT_MQTT_SUCCESS if PUBLISH is valid; #IOT_MQTT_BAD_RESPONSE
 * if the PUBLISH packet doesn't follow MQTT spec.
 */
IotMqttError_t _IotMqtt_DeserializePublish( const uint8_t * pPublishStart,
                                            size_t dataLength,
                                            IotMqttPublishInfo_t * pOutput,
                                            uint16_t * pPacketIdentifier,
                                            size_t * pBytesProcessed );

/**
 * @brief Generate a PUBACK packet for the given packet identifier.
 *
 * @param[in] packetIdentifier The packet identifier to place in PUBACK.
 * @param[out] pPubackPacket Where the PUBACK packet is written.
 * @param[out] pPacketSize Size of the packet written to `pPubackPacket`.
 *
 * @return #IOT_MQTT_SUCCESS or #IOT_MQTT_NO_MEMORY.
 */
IotMqttError_t _IotMqtt_SerializePuback( uint16_t packetIdentifier,
                                         uint8_t ** pPubackPacket,
                                         size_t * pPacketSize );

/**
 * @brief Deserialize a PUBACK packet.
 *
 * Converts the packet from a stream of bytes to an #IotMqttError_t and extracts
 * the packet identifier. Also prints out debug log messages about the packet.
 *
 * @param[in] pPubackStart Pointer to the start of a PUBACK packet.
 * @param[in] dataLength Length of the data stream after `pPubackStart`.
 * @param[out] pPacketIdentifier The packet identifier in the PUBACK.
 * @param[out] pBytesProcessed The number of bytes in the data stream processed
 * by this function. For PUBACK, this is always 4.
 *
 * @return #IOT_MQTT_SUCCESS if PUBACK is valid; #IOT_MQTT_BAD_RESPONSE
 * if the PUBACK packet doesn't follow MQTT spec.
 */
IotMqttError_t _IotMqtt_DeserializePuback( const uint8_t * pPubackStart,
                                           size_t dataLength,
                                           uint16_t * pPacketIdentifier,
                                           size_t * pBytesProcessed );

/**
 * @brief Generate a SUBSCRIBE packet from the given parameters.
 *
 * @param[in] pSubscriptionList User-provided array of subscriptions.
 * @param[in] subscriptionCount Size of `pSubscriptionList`.
 * @param[out] pSubscribePacket Where the SUBSCRIBE packet is written.
 * @param[out] pPacketSize Size of the packet written to `pSubscribePacket`.
 * @param[out] pPacketIdentifier The packet identifier generated for this SUBSCRIBE.
 *
 * @return #IOT_MQTT_SUCCESS or #IOT_MQTT_NO_MEMORY.
 */
IotMqttError_t _IotMqtt_SerializeSubscribe( const IotMqttSubscription_t * pSubscriptionList,
                                            size_t subscriptionCount,
                                            uint8_t ** pSubscribePacket,
                                            size_t * pPacketSize,
                                            uint16_t * pPacketIdentifier );

/**
 * @brief Deserialize a SUBACK packet.
 *
 * Converts the packet from a stream of bytes to an #IotMqttError_t and extracts
 * the packet identifier. Also prints out debug log messages about the packet.
 *
 * @param[in] mqttConnection The MQTT connection associated with the subscription.
 * Rejected topic filters are removed from this connection.
 * @param[in] pSubackStart Pointer to the start of a SUBACK packet.
 * @param[in] dataLength Length of the data stream after `pSubackStart`.
 * @param[out] pPacketIdentifier The packet identifier in the SUBACK.
 * @param[out] pBytesProcessed The number of bytes in the data stream processed by
 * this function.
 *
 * @return #IOT_MQTT_SUCCESS if SUBACK is valid; #IOT_MQTT_BAD_RESPONSE
 * if the SUBACK packet doesn't follow MQTT spec.
 */
IotMqttError_t _IotMqtt_DeserializeSuback( IotMqttConnection_t mqttConnection,
                                           const uint8_t * pSubackStart,
                                           size_t dataLength,
                                           uint16_t * pPacketIdentifier,
                                           size_t * pBytesProcessed );

/**
 * @brief Generate an UNSUBSCRIBE packet from the given parameters.
 *
 * @param[in] pSubscriptionList User-provided array of subscriptions to remove.
 * @param[in] subscriptionCount Size of `pSubscriptionList`.
 * @param[out] pUnsubscribePacket Where the UNSUBSCRIBE packet is written.
 * @param[out] pPacketSize Size of the packet written to `pUnsubscribePacket`.
 * @param[out] pPacketIdentifier The packet identifier generated for this UNSUBSCRIBE.
 *
 * @return #IOT_MQTT_SUCCESS or #IOT_MQTT_NO_MEMORY.
 */
IotMqttError_t _IotMqtt_SerializeUnsubscribe( const IotMqttSubscription_t * pSubscriptionList,
                                              size_t subscriptionCount,
                                              uint8_t ** pUnsubscribePacket,
                                              size_t * pPacketSize,
                                              uint16_t * pPacketIdentifier );

/**
 * @brief Deserialize a UNSUBACK packet.
 *
 * Converts the packet from a stream of bytes to an #IotMqttError_t and extracts
 * the packet identifier. Also prints out debug log messages about the packet.
 *
 * @param[in] pUnsubackStart Pointer to the start of an UNSUBACK packet.
 * @param[in] dataLength Length of the data stream after `pUnsubackStart`.
 * @param[out] pPacketIdentifier The packet identifier in the UNSUBACK.
 * @param[out] pBytesProcessed The number of bytes in the data stream processed by
 * this function. For UNSUBACK, this is always 4.
 *
 * @return #IOT_MQTT_SUCCESS if UNSUBACK is valid; #IOT_MQTT_BAD_RESPONSE
 * if the UNSUBACK packet doesn't follow MQTT spec.
 */
IotMqttError_t _IotMqtt_DeserializeUnsuback( const uint8_t * pUnsubackStart,
                                             size_t dataLength,
                                             uint16_t * pPacketIdentifier,
                                             size_t * pBytesProcessed );

/**
 * @brief Generate a PINGREQ packet.
 *
 * @param[out] pPingreqPacket Where the PINGREQ packet is written.
 * @param[out] pPacketSize Size of the packet written to `pPingreqPacket`.
 *
 * @return Always returns #IOT_MQTT_SUCCESS.
 */
IotMqttError_t _IotMqtt_SerializePingreq( uint8_t ** pPingreqPacket,
                                          size_t * pPacketSize );

/**
 * @brief Deserialize a PINGRESP packet.
 *
 * Converts the packet from a stream of bytes to an #IotMqttError_t. Also
 * prints out debug log messages about the packet.
 * @param[in] pPingrespStart Pointer to the start of a PINGRESP packet.
 * @param[in] dataLength Length of the data stream after `pPingrespStart`.
 * @param[out] pBytesProcessed The number of bytes in the data stream processed by
 * this function. For PINGRESP, this is always 2.
 *
 * @return #IOT_MQTT_SUCCESS if PINGRESP is valid; #IOT_MQTT_BAD_RESPONSE
 * if the PINGRESP packet doesn't follow MQTT spec.
 */
IotMqttError_t _IotMqtt_DeserializePingresp( const uint8_t * pPingrespStart,
                                             size_t dataLength,
                                             size_t * pBytesProcessed );

/**
 * @brief Generate a DISCONNECT packet.
 *
 * @param[out] pDisconnectPacket Where the DISCONNECT packet is written.
 * @param[out] pPacketSize Size of the packet written to `pDisconnectPacket`.
 *
 * @return Always returns #IOT_MQTT_SUCCESS.
 */
IotMqttError_t _IotMqtt_SerializeDisconnect( uint8_t ** pDisconnectPacket,
                                             size_t * pPacketSize );

/**
 * @brief Free a packet generated by the serializer.
 *
 * @param[in] pPacket The packet to free.
 */
void _IotMqtt_FreePacket( uint8_t * pPacket );

/*-------------------- MQTT operation record functions ----------------------*/

/**
 * @brief Create a record for a new in-progress MQTT operation.
 *
 * @param[in] pMqttConnection The MQTT connection to associate with the operation.
 * @param[in] flags Flags variable passed to a user-facing MQTT function.
 * @param[in] pCallbackInfo User-provided callback function and parameter.
 * @param[out] pNewOperation Set to point to the new operation on success.
 *
 * @return #IOT_MQTT_SUCCESS, #IOT_MQTT_BAD_PARAMETER, or #IOT_MQTT_NO_MEMORY.
 */
IotMqttError_t _IotMqtt_CreateOperation( _mqttConnection_t * pMqttConnection,
                                         uint32_t flags,
                                         const IotMqttCallbackInfo_t * pCallbackInfo,
                                         _mqttOperation_t ** pNewOperation );

/**
 * @brief Decrement the job reference count of an MQTT operation and optionally
 * cancel its job.
 *
 * Checks if the operation may be destroyed afterwards.
 *
 * @param[in] pOperation The MQTT operation with the job to cancel.
 * @param[in] cancelJob Whether to attempt cancellation of the operation's job.
 *
 * @return `true` if the the operation may be safely destroyed; `false` otherwise.
 */
bool _IotMqtt_DecrementOperationReferences( _mqttOperation_t * pOperation,
                                            bool cancelJob );

/**
 * @brief Free resources used to record an MQTT operation. This is called when
 * the operation completes.
 *
 * @param[in] pOperation The operation which completed.
 */
void _IotMqtt_DestroyOperation( _mqttOperation_t * pOperation );

/**
 * @brief Task pool routine for processing an MQTT connection's keep-alive.
 *
 * @param[in] pTaskPool Pointer to #_IotMqttTaskPool.
 * @param[in] pKeepAliveJob Pointer the an MQTT connection's keep-alive job.
 * @param[in] pContext Pointer to an MQTT connection, passed as an opaque context.
 */
void _IotMqtt_ProcessKeepAlive( IotTaskPool_t * pTaskPool,
                                IotTaskPoolJob_t * pKeepAliveJob,
                                void * pContext );

/**
 * @brief Task pool routine for processing an incoming PUBLISH message.
 *
 * @param[in] pTaskPool Pointer to #_IotMqttTaskPool.
 * @param[in] pPublishJob Pointer to the incoming PUBLISH operation's job.
 * @param[in] pContext Pointer to the incoming PUBLISH operation, passed as an
 * opaque context.
 */
void _IotMqtt_ProcessIncomingPublish( IotTaskPool_t * pTaskPool,
                                      IotTaskPoolJob_t * pPublishJob,
                                      void * pContext );

/**
 * @brief Task pool routine for processing an MQTT operation to send.
 *
 * @param[in] pTaskPool Pointer to #_IotMqttTaskPool.
 * @param[in] pSendJob Pointer to an operation's job.
 * @param[in] pContext Pointer to the operation to send, passed as an opaque
 * context.
 */
void _IotMqtt_ProcessSend( IotTaskPool_t * pTaskPool,
                           IotTaskPoolJob_t * pSendJob,
                           void * pContext );

/**
 * @brief Task pool routine for processing a completed MQTT operation.
 *
 * @param[in] pTaskPool Pointer to #_IotMqttTaskPool.
 * @param[in] pOperationJob Pointer to the completed operation's job.
 * @param[in] pContext Pointer to the completed operation, passed as an opaque
 * context.
 */
void _IotMqtt_ProcessCompletedOperation( IotTaskPool_t * pTaskPool,
                                         IotTaskPoolJob_t * pOperationJob,
                                         void * pContext );

/**
 * @brief Schedule an operation for immediate processing.
 *
 * @param[in] pOperation The operation to schedule.
 * @param[in] jobRoutine The routine to run for the job. Must be either
 * #_IotMqtt_ProcessSend, #_IotMqtt_ProcessCompletedOperation, or
 * #_IotMqtt_ProcessIncomingPublish.
 * @param[in] delay A delay before the operation job should be executed. Pass
 * `0` to execute ASAP.
 *
 * @return #IOT_MQTT_SUCCESS or #IOT_MQTT_SCHEDULING_ERROR.
 */
IotMqttError_t _IotMqtt_ScheduleOperation( _mqttOperation_t * pOperation,
                                           IotTaskPoolRoutine_t jobRoutine,
                                           uint64_t delay );

/**
 * @brief Search a list of MQTT operations pending responses using an operation
 * name and packet identifier. Removes a matching operation from the list if found.
 *
 * @param[in] pMqttConnection The connection associated with the operation.
 * @param[in] operation The operation type to look for.
 * @param[in] pPacketIdentifier A packet identifier to match. Pass `NULL` to ignore.
 *
 * @return Pointer to any matching operation; `NULL` if no match was found.
 */
_mqttOperation_t * _IotMqtt_FindOperation( _mqttConnection_t * pMqttConnection,
                                           IotMqttOperationType_t operation,
                                           const uint16_t * pPacketIdentifier );

/**
 * @brief Notify of a completed MQTT operation.
 *
 * @param[in] pOperation The MQTT operation which completed.
 *
 * Depending on the parameters passed to a user-facing MQTT function, the
 * notification will cause @ref mqtt_function_wait to return or invoke a
 * user-provided callback.
 */
void _IotMqtt_Notify( _mqttOperation_t * pOperation );

/*----------------- MQTT subscription management functions ------------------*/

/**
 * @brief Add an array of subscriptions to the subscription manager.
 *
 * @param[in] pMqttConnection The MQTT connection associated with the subscriptions.
 * @param[in] subscribePacketIdentifier Packet identifier for the subscriptions'
 * SUBSCRIBE packet.
 * @param[in] pSubscriptionList The first element in the array.
 * @param[in] subscriptionCount Number of elements in `pSubscriptionList`.
 *
 * @return #IOT_MQTT_SUCCESS or #IOT_MQTT_NO_MEMORY.
 */
IotMqttError_t _IotMqtt_AddSubscriptions( _mqttConnection_t * pMqttConnection,
                                          uint16_t subscribePacketIdentifier,
                                          const IotMqttSubscription_t * pSubscriptionList,
                                          size_t subscriptionCount );

/**
 * @brief Process a received PUBLISH from the server, invoking any subscription
 * callbacks that have a matching topic filter.
 *
 * @param[in] pMqttConnection The MQTT connection associated with the received
 * PUBLISH.
 * @param[in] pCallbackParam The parameter to pass to a PUBLISH callback.
 */
void _IotMqtt_InvokeSubscriptionCallback( _mqttConnection_t * pMqttConnection,
                                          IotMqttCallbackParam_t * pCallbackParam );

/**
 * @brief Remove a single subscription from the subscription manager by
 * packetIdentifier and order.
 *
 * @param[in] pMqttConnection The MQTT connection associated with the subscriptions.
 * @param[in] packetIdentifier The packet identifier associated with the subscription's
 * SUBSCRIBE packet.
 * @param[in] order The order of the subscription in the SUBSCRIBE packet.
 * Pass `-1` to ignore order and remove all subscriptions for `packetIdentifier`.
 */
void _IotMqtt_RemoveSubscriptionByPacket( _mqttConnection_t * pMqttConnection,
                                          uint16_t packetIdentifier,
                                          int32_t order );

/**
 * @brief Remove an array of subscriptions from the subscription manager by
 * topic filter.
 *
 * @param[in] pMqttConnection The MQTT connection associated with the subscriptions.
 * @param[in] pSubscriptionList The first element in the array.
 * @param[in] subscriptionCount Number of elements in `pSubscriptionList`.
 */
void _IotMqtt_RemoveSubscriptionByTopicFilter( _mqttConnection_t * pMqttConnection,
                                               const IotMqttSubscription_t * pSubscriptionList,
                                               size_t subscriptionCount );

/*------------------ MQTT connection management functions -------------------*/

/**
 * @brief Attempt to increment the reference count of an MQTT connection.
 *
 * @param[in] pMqttConnection The referenced MQTT connection.
 *
 * @return `true` if the reference count was incremented; `false` otherwise. The
 * reference count will not be incremented for a disconnected connection.
 */
bool _IotMqtt_IncrementConnectionReferences( _mqttConnection_t * pMqttConnection );

/**
 * @brief Decrement the reference count of an MQTT connection.
 *
 * Also destroys an unreferenced MQTT connection.
 *
 * @param[in] pMqttConnection The referenced MQTT connection.
 */
void _IotMqtt_DecrementConnectionReferences( _mqttConnection_t * pMqttConnection );

/**
 * @brief Closes the network connection associated with an MQTT connection.
 *
 * A network disconnect function must be set in the network interface for the
 * network connection to be closed.
 *
 * @param[in] pMqttConnection The MQTT connection with the network connection
 * to close.
 */
void _IotMqtt_CloseNetworkConnection( _mqttConnection_t * pMqttConnection );

#endif /* ifndef _IOT_MQTT_INTERNAL_H_ */
