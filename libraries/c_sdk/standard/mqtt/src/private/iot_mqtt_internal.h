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
 * https://github.com/freertos
 * https://www.FreeRTOS.org
 */

/**
 * @file iot_mqtt_internal.h
 * @brief Internal header of MQTT library. This header should not be included in
 * typical application code.
 */

#ifndef IOT_MQTT_INTERNAL_H_
#define IOT_MQTT_INTERNAL_H_

/* The config header is always included first. */
#include "iot_config.h"

/* Linear containers (lists and queues) include. */
#include "iot_linear_containers.h"

/* MQTT include. */
#include "iot_mqtt.h"

/* Task pool include. */
#include "iot_taskpool.h"

/* MQTT LTS library includes. */
#include "core_mqtt_serializer.h"
#include "core_mqtt.h"
#include "core_mqtt_state.h"

/* FreeRTOS include. */
#include "semphr.h"

/* Transport interface include. */
#include "transport_interface.h"

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
    #ifndef LIBRARY_LOG_LEVEL
        #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_MQTT
    #endif
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_GLOBAL
    #else
        #ifndef LIBRARY_LOG_LEVEL
            #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
        #endif
    #endif
#endif /* ifdef IOT_LOG_LEVEL_MQTT */

#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    ( "MQTT" )
#endif

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
    void * IotMqtt_MallocConnection( size_t size );

/**
 * @brief Free an #_mqttConnection_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    void IotMqtt_FreeConnection( void * ptr );

/**
 * @brief Allocate memory for an MQTT packet. This function should have the
 * same signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    #define IotMqtt_MallocMessage    Iot_MallocMessageBuffer

/**
 * @brief Free an MQTT packet. This function should have the same signature
 * as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    #define IotMqtt_FreeMessage      Iot_FreeMessageBuffer

/**
 * @brief Allocate an #_mqttOperation_t. This function should have the same
 * signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    void * IotMqtt_MallocOperation( size_t size );

/**
 * @brief Free an #_mqttOperation_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    void IotMqtt_FreeOperation( void * ptr );

/**
 * @brief Allocate an #_mqttSubscription_t. This function should have the
 * same signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    void * IotMqtt_MallocSubscription( size_t size );

/**
 * @brief Free an #_mqttSubscription_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    void IotMqtt_FreeSubscription( void * ptr );
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
#ifndef EMPTY_ELSE_MARKER
    #define EMPTY_ELSE_MARKER
#endif

/*
 * Constants related to limits defined in AWS Service Limits.
 *
 * For details, see
 * https://docs.aws.amazon.com/general/latest/gr/aws_service_limits.html
 *
 * Used to validate parameters if when connecting to an AWS IoT MQTT server.
 */
#define AWS_IOT_MQTT_SERVER_MIN_KEEPALIVE                      ( 30 )   /**< @brief Minumum keep-alive interval accepted by AWS IoT. */
#define AWS_IOT_MQTT_SERVER_MAX_KEEPALIVE                      ( 1200 ) /**< @brief Maximum keep-alive interval accepted by AWS IoT. */
#define AWS_IOT_MQTT_SERVER_MAX_CLIENTID                       ( 128 )  /**< @brief Maximum length of client identifier accepted by AWS IoT. */
#define AWS_IOT_MQTT_SERVER_MAX_TOPIC_LENGTH                   ( 256 )  /**< @brief Maximum length of topic names or filters accepted by AWS IoT. */
#define AWS_IOT_MQTT_SERVER_MAX_TOPIC_FILTERS_PER_SUBSCRIBE    ( 8 )    /**< @brief Maximum number of topic filters in a single SUBSCRIBE packet. */

/*
 * MQTT control packet type and flags. Always the first byte of an MQTT
 * packet.
 *
 * For details, see
 * http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/csprd02/mqtt-v3.1.1-csprd02.html#_Toc385349757
 */


/**
 * @brief A value that represents an invalid remaining length.
 *
 * This value is greater than what is allowed by the MQTT specification.
 */
#define MQTT_REMAINING_LENGTH_INVALID    ( ( size_t ) 268435456 )

/**
 * @brief Default config for Maximum Number of MQTT CONNECTIONS.
 * This config can be specified by the application based on number of MQTT
 * connections needed.
 */
#ifndef MAX_NO_OF_MQTT_CONNECTIONS
    #define MAX_NO_OF_MQTT_CONNECTIONS    ( 2 )
#endif

/**
 * @brief Default config for Maximum Number of MQTT Subscriptions.
 * This config can be specified by the application based on number of MQTT
 * subscriptions. Maximum number of subscriptions that shim can hold is 128.
 */
#ifndef MAX_NO_OF_MQTT_SUBSCRIPTIONS
    #define MAX_NO_OF_MQTT_SUBSCRIPTIONS    ( 10 )
#endif

/**
 * @brief Static buffer size provided to MQTT LTS API.
 * This buffer will be used to send the packets on the network.
 *
 */
#ifndef NETWORK_BUFFER_SIZE
    #define NETWORK_BUFFER_SIZE    ( 1024U )
#endif
/*---------------------- MQTT internal data structures ----------------------*/

/**
 * @brief Defining the structure for network context used for sending the packets on the network.
 * The declaration of the structure is mentioned in the transport_interface.h file.
 */
typedef struct MqttTransportParams
{
    void * pNetworkConnection;                       /**< @brief The network connection used for sending packets on the network. */
    const IotNetworkInterface_t * pNetworkInterface; /**< @brief The network interface used to send packets on the network using the above network connection. */
} MqttTransportParams_t;

/**
 * @brief Represents an MQTT connection.
 */
typedef struct _mqttConnection
{
    bool awsIotMqttMode;                             /**< @brief Specifies if this connection is to an AWS IoT MQTT server. */
    bool ownNetworkConnection;                       /**< @brief Whether this MQTT connection owns its network connection. */
    void * pNetworkConnection;                       /**< @brief References the transport-layer network connection. */
    const IotNetworkInterface_t * pNetworkInterface; /**< @brief Network interface provided to @ref mqtt_function_connect. */
    IotMqttCallbackInfo_t disconnectCallback;        /**< @brief A function to invoke when this connection is disconnected. */

    #if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
        const IotMqttSerializer_t * pSerializer; /**< @brief MQTT packet serializer overrides. */
    #endif

    bool disconnected;                           /**< @brief Tracks if this connection has been disconnected. */
    IotMutex_t referencesMutex;                  /**< @brief Recursive mutex. Grants access to connection state and operation lists. */
    int32_t references;                          /**< @brief Counts callbacks and operations using this connection. */
    IotListDouble_t pendingProcessing;           /**< @brief List of operations waiting to be processed by a task pool routine. */
    IotListDouble_t pendingResponse;             /**< @brief List of processed operations awaiting a server response. */

    uint64_t lastMessageTime;                    /**< @brief When the most recent message was transmitted. */
    bool keepAliveFailure;                       /**< @brief Failure flag for keep-alive operation. */
    uint32_t keepAliveMs;                        /**< @brief Keep-alive interval in milliseconds. Its max value (per spec) is 65,535,000. */
    uint32_t nextKeepAliveMs;                    /**< @brief Relative delay for next keep-alive job. */
    IotTaskPoolJobStorage_t keepAliveJobStorage; /**< @brief Task pool job for processing this connection's keep-alive. */
    IotTaskPoolJob_t keepAliveJob;               /**< @brief Task pool job for processing this connection's keep-alive. */
    uint8_t * pPingreqPacket;                    /**< @brief An MQTT PINGREQ packet, allocated if keep-alive is active. */
    size_t pingreqPacketSize;                    /**< @brief The size of an allocated PINGREQ packet. */
} _mqttConnection_t;

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
    char * pTopicFilter;            /**< @brief The subscription topic filter. */
} _mqttSubscription_t;

/**
 * @brief Internal structure representing a single MQTT operation, such as
 * CONNECT, SUBSCRIBE, PUBLISH, etc.
 *
 * Queues of these structures keeps track of all in-progress MQTT operations.
 */
typedef struct _mqttOperation
{
    /* Pointers to neighboring queue elements. */
    IotLink_t link;                      /**< @brief List link member. */

    bool incomingPublish;                /**< @brief Set to true if this operation an incoming PUBLISH. */
    _mqttConnection_t * pMqttConnection; /**< @brief MQTT connection associated with this operation. */

    IotTaskPoolJobStorage_t jobStorage;  /**< @brief Task pool job storage associated with this operation. */
    IotTaskPoolJob_t job;                /**< @brief Task pool job associated with this operation. */

    union
    {
        /* If incomingPublish is false, this struct is valid. */
        struct
        {
            /* Basic operation information. */
            int32_t jobReference;        /**< @brief Tracks if a job is using this operation. Must always be 0, 1, or 2. */
            IotMqttOperationType_t type; /**< @brief What operation this structure represents. */
            uint32_t flags;              /**< @brief Flags passed to the function that created this operation. */
            uint16_t packetIdentifier;   /**< @brief The packet identifier used with this operation. */

            /* Serialized packet and size. */
            uint8_t * pMqttPacket;           /**< @brief The MQTT packet to send over the network. */
            uint8_t * pPacketIdentifierHigh; /**< @brief The location of the high byte of the packet identifier in the MQTT packet. */
            size_t packetSize;               /**< @brief Size of `pMqttPacket`. */

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
                uint32_t nextPeriod;
            } retry;
        } operation;

        /* If incomingPublish is true, this struct is valid. */
        struct
        {
            IotMqttPublishInfo_t publishInfo; /**< @brief Deserialized PUBLISH. */
            const void * pReceivedData;       /**< @brief Any buffer associated with this PUBLISH that should be freed. */
        } publish;
    } u;                                      /**< @brief Valid member depends on _mqttOperation_t.incomingPublish. */
} _mqttOperation_t;

/**
 * @brief Represents an MQTT packet received from the network.
 *
 * This struct is used to hold parameters for the deserializers so that all
 * deserializers have the same function signature.
 */
typedef struct _mqttPacket
{
    union
    {
        /**
         * @brief (Input) MQTT connection associated with this packet. Only used
         * when deserializing SUBACKs.
         */
        _mqttConnection_t * pMqttConnection;

        /**
         * @brief (Output) Operation representing an incoming PUBLISH. Only used
         * when deserializing PUBLISHes.
         */
        _mqttOperation_t * pIncomingPublish;
    } u;                       /**< @brief Valid member depends on packet being decoded. */

    uint8_t * pRemainingData;  /**< @brief (Input) The remaining data in MQTT packet. */
    size_t remainingLength;    /**< @brief (Input) Length of the remaining data in the MQTT packet. */
    uint16_t packetIdentifier; /**< @brief (Output) MQTT packet identifier. */
    uint8_t type;              /**< @brief (Input) A value identifying the packet type. */
} _mqttPacket_t;

/**
 * @brief Represents a mapping of MQTT Connection in MQTT 201906.00 library to the corresponding MQTT context
 * used in MQTT LTS library. MQTT Context is used to call the MQTT LTS API from the shim to serialize
 * and send packets on the network.
 */
typedef struct connContextMapping
{
    IotMqttConnection_t mqttConnection;                                    /**< @brief MQTT connection used in MQTT 201906.00 library. */
    MQTTContext_t context;                                                 /**< @brief MQTT Context used for calling MQTT LTS API from the shim. */
    StaticSemaphore_t contextMutexStorage;                                 /**< @brief Static storage for Mutex for synchronization of MQTT Context. */
    SemaphoreHandle_t contextMutex;                                        /**< @brief Mutex for synchronization of network buffer as the same buffer can be used my multiple applications. */
    uint8_t buffer[ NETWORK_BUFFER_SIZE ];                                 /**< @brief Network Buffer used to send packets on the network. This will be used by MQTT context defined above. */
    _mqttSubscription_t subscriptionArray[ MAX_NO_OF_MQTT_SUBSCRIPTIONS ]; /**< @brief Holds subscriptions associated with this connection. */
    StaticSemaphore_t subscriptionMutexStorage;                            /**< @brief Static storage for Mutex for synchronization of subscription list. */
    SemaphoreHandle_t subscriptionMutex;                                   /**< @brief Grants exclusive access to the subscription list. */
    MqttTransportParams_t mqttTransportParams;                             /**< @brief MQTT Transport Params holds the network interface and network connection. */
} _connContext_t;

/**
 * @brief First parameter to #_topicMatch.
 */
typedef struct _topicMatchParams
{
    const char * pTopicName;  /**< @brief The topic name to parse. */
    uint16_t topicNameLength; /**< @brief Length of #_topicMatchParams_t.pTopicName. */
    bool exactMatchOnly;      /**< @brief Whether to allow wildcards or require exact matches. */
} _topicMatchParams_t;

/**
 * @brief First parameter to #_packetMatch.
 */
typedef struct _packetMatchParams
{
    uint16_t packetIdentifier; /**< Packet identifier to match. */
    int32_t order;             /**< Order to match. Set to `-1` to ignore. */
} _packetMatchParams_t;

/*-------------------- MQTT struct validation functions ---------------------*/

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
 * @brief Check that an #IotMqttOperation_t is valid and waitable.
 *
 * @param[in] operation The #IotMqttOperation_t to validate.
 *
 * @return `true` if `operation` is valid; `false` otherwise.
 */
bool _IotMqtt_ValidateOperation( IotMqttOperation_t operation );

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
 * @brief Get the MQTT packet type from a stream of bytes off the network.
 *
 * @param[in] pNetworkConnection Reference to the network connection.
 * @param[in] pNetworkInterface Function pointers used to interact with the
 * network.
 *
 * @return One of the server-to-client MQTT packet types.
 *
 * @note This function is only used for incoming packets, and may not work
 * correctly for outgoing packets.
 */
uint8_t _IotMqtt_GetPacketType( void * pNetworkConnection,
                                const IotNetworkInterface_t * pNetworkInterface );

/**
 * @brief Get the remaining length from a stream of bytes off the network.
 *
 * @param[in] pNetworkConnection Reference to the network connection.
 * @param[in] pNetworkInterface Function pointers used to interact with the
 * network.
 *
 * @return The remaining length; #MQTT_REMAINING_LENGTH_INVALID on error.
 */
size_t _IotMqtt_GetRemainingLength( void * pNetworkConnection,
                                    const IotNetworkInterface_t * pNetworkInterface );



/**
 * @brief Set the DUP bit in a QoS 1 PUBLISH packet.
 *
 * @param[in] pPublishPacket Pointer to the PUBLISH packet to modify.
 * @param[in] pPacketIdentifierHigh The high byte of any packet identifier to modify.
 * @param[out] pNewPacketIdentifier Since AWS IoT does not support the DUP flag,
 * a new packet identifier is generated and should be written here. This parameter
 * is only used when connected to an AWS IoT MQTT server.
 *
 * @note See #IotMqttPublishInfo_t for caveats with retransmission to the
 * AWS IoT MQTT server.
 */
void _IotMqtt_PublishSetDup( uint8_t * pPublishPacket,
                             uint8_t * pPacketIdentifierHigh,
                             uint16_t * pNewPacketIdentifier );

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
 * @param[in] pTaskPool Pointer to the system task pool.
 * @param[in] pKeepAliveJob Pointer the an MQTT connection's keep-alive job.
 * @param[in] pContext Pointer to an MQTT connection, passed as an opaque context.
 */
void _IotMqtt_ProcessKeepAlive( IotTaskPool_t pTaskPool,
                                IotTaskPoolJob_t pKeepAliveJob,
                                void * pContext );

/**
 * @brief Task pool routine for processing an incoming PUBLISH message.
 *
 * @param[in] pTaskPool Pointer to the system task pool.
 * @param[in] pPublishJob Pointer to the incoming PUBLISH operation's job.
 * @param[in] pContext Pointer to the incoming PUBLISH operation, passed as an
 * opaque context.
 */
void _IotMqtt_ProcessIncomingPublish( IotTaskPool_t pTaskPool,
                                      IotTaskPoolJob_t pPublishJob,
                                      void * pContext );

/**
 * @brief Task pool routine for processing an MQTT operation to send.
 *
 * @param[in] pTaskPool Pointer to the system task pool.
 * @param[in] pSendJob Pointer to an operation's job.
 * @param[in] pContext Pointer to the operation to send, passed as an opaque
 * context.
 */
void _IotMqtt_ProcessSend( IotTaskPool_t pTaskPool,
                           IotTaskPoolJob_t pSendJob,
                           void * pContext );

/**
 * @brief Task pool routine for processing a completed MQTT operation.
 *
 * @param[in] pTaskPool Pointer to the system task pool.
 * @param[in] pOperationJob Pointer to the completed operation's job.
 * @param[in] pContext Pointer to the completed operation, passed as an opaque
 * context.
 */
void _IotMqtt_ProcessCompletedOperation( IotTaskPool_t pTaskPool,
                                         IotTaskPoolJob_t pOperationJob,
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
                                           uint32_t delay );

/**
 * @brief Search a list of MQTT operations pending responses using an operation
 * name and packet identifier. Removes a matching operation from the list if found.
 *
 * @param[in] pMqttConnection The connection associated with the operation.
 * @param[in] type The operation type to look for.
 * @param[in] pPacketIdentifier A packet identifier to match. Pass `NULL` to ignore.
 *
 * @return Pointer to any matching operation; `NULL` if no match was found.
 */
_mqttOperation_t * _IotMqtt_FindOperation( _mqttConnection_t * pMqttConnection,
                                           IotMqttOperationType_t type,
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
 * @brief Read the next available byte on a network connection.
 *
 * @param[in] pNetworkConnection Reference to the network connection.
 * @param[in] pNetworkInterface Function pointers used to interact with the
 * network.
 * @param[out] pIncomingByte The byte read from the network.
 *
 * @return `true` if a byte was successfully received from the network; `false`
 * otherwise.
 */
bool _IotMqtt_GetNextByte( void * pNetworkConnection,
                           const IotNetworkInterface_t * pNetworkInterface,
                           uint8_t * pIncomingByte );

/**
 * @brief Closes the network connection associated with an MQTT connection.
 *
 * A network disconnect function must be set in the network interface for the
 * network connection to be closed.
 *
 * @param[in] disconnectReason A reason to pass to the connection's disconnect
 * callback.
 * @param[in] pMqttConnection The MQTT connection with the network connection
 * to close.
 */
void _IotMqtt_CloseNetworkConnection( IotMqttDisconnectReason_t disconnectReason,
                                      _mqttConnection_t * pMqttConnection );

/*----------------- MQTT Serialization /Deserialization Wrapper functions for Shim------------------*/

/**
 * @brief Generate a CONNECT packet from the given parameters by using MQTT v4 beta 2 serializer.
 *
 * @param[in] pConnectInfo User-provided CONNECT information.
 * @param[out] pConnectPacket Where the CONNECT packet is written.
 * @param[out] pPacketSize Size of the packet written to `pConnectPacket`.
 *
 * @return #IOT_MQTT_SUCCESS or #IOT_MQTT_NO_MEMORY.
 */
IotMqttError_t _IotMqtt_connectSerializeWrapper( const IotMqttConnectInfo_t * pConnectInfo,
                                                 uint8_t ** pConnectPacket,
                                                 size_t * pPacketSize );

/**
 * @brief Generate a DISCONNECT packet by using MQTT v4 beta 2 serializer.
 *
 * @param[out] pDisconnectPacket Where the DISCONNECT packet is written.
 * @param[out] pPacketSize Size of the packet written to `pDisconnectPacket`.
 *
 * @return Always returns #IOT_MQTT_SUCCESS.
 */
IotMqttError_t _IotMqtt_disconnectSerializeWrapper( uint8_t ** pDisconnectPacket,
                                                    size_t * pPacketSize );

/**
 * @brief Generate a SUBSCRIBE packet from the given parameters by using MQTT v4 beta 2 serializer.
 *
 * @param[in] pSubscriptionList User-provided array of subscriptions.
 * @param[in] subscriptionCount Size of `pSubscriptionList`.
 * @param[out] pSubscribePacket Where the SUBSCRIBE packet is written.
 * @param[out] pPacketSize Size of the packet written to `pSubscribePacket`.
 * @param[out] pPacketIdentifier The packet identifier generated for this SUBSCRIBE.
 *
 * @return #IOT_MQTT_SUCCESS or #IOT_MQTT_NO_MEMORY.
 */
IotMqttError_t _IotMqtt_subscribeSerializeWrapper( const IotMqttSubscription_t * pSubscriptionList,
                                                   size_t subscriptionCount,
                                                   uint8_t ** pSubscribePacket,
                                                   size_t * pPacketSize,
                                                   uint16_t * pPacketIdentifier );

/**
 * @brief Generate an UNSUBSCRIBE packet from the given parameters by using MQTT v4 beta 2 serializer.
 *
 * @param[in] pUnsubscriptionList User-provided array of subscriptions to remove.
 * @param[in] unsubscriptionCount Size of `pSubscriptionList`.
 * @param[out] pUnsubscribePacket Where the UNSUBSCRIBE packet is written.
 * @param[out] pPacketSize Size of the packet written to `pUnsubscribePacket`.
 * @param[out] pPacketIdentifier The packet identifier generated for this UNSUBSCRIBE.
 *
 * @return #IOT_MQTT_SUCCESS or #IOT_MQTT_NO_MEMORY.
 */

IotMqttError_t _IotMqtt_unsubscribeSerializeWrapper( const IotMqttSubscription_t * pUnsubscriptionList,
                                                     size_t unsubscriptionCount,
                                                     uint8_t ** pUnsubscribePacket,
                                                     size_t * pPacketSize,
                                                     uint16_t * pPacketIdentifier );

/**
 * @brief Generate a PUBLISH packet from the given parameters by using MQTT v4 beta 2 serializer.
 *
 * @param[in] pPublishInfo User-provided PUBLISH information.
 * @param[out] pPublishPacket Where the PUBLISH packet is written.
 * @param[out] pPacketSize Size of the packet written to `pPublishPacket`.
 * @param[out] pPacketIdentifier The packet identifier generated for this PUBLISH.
 * @param[out] pPacketIdentifierHigh Where the high byte of the packet identifier
 * is written.
 *
 * @return #IOT_MQTT_SUCCESS or #IOT_MQTT_NO_MEMORY.
 */
IotMqttError_t _IotMqtt_publishSerializeWrapper( const IotMqttPublishInfo_t * pPublishInfo,
                                                 uint8_t ** pPublishPacket,
                                                 size_t * pPacketSize,
                                                 uint16_t * pPacketIdentifier,
                                                 uint8_t ** pPacketIdentifierHigh );

/**
 * @brief Generate a PINGREQ packet by using MQTT v4 beta 2 serializer.
 *
 * @param[out] pPingreqPacket Where the PINGREQ packet is written.
 * @param[out] pPacketSize Size of the packet written to `pPingreqPacket`.
 *
 * @return Always returns #IOT_MQTT_SUCCESS.
 */
IotMqttError_t _IotMqtt_pingreqSerializeWrapper( uint8_t ** pPingreqPacket,
                                                 size_t * pPacketSize );

/**
 * @brief Deserialize a connack packet received from the network by using MQTT v4 beta 2 deserializer.
 *
 * @param[in,out] pConnack Pointer to an MQTT packet struct representing a CONNACK.
 *
 * @return #IOT_MQTT_SUCCESS if CONNACK specifies that CONNECT was accepted;
 * #IOT_MQTT_SERVER_REFUSED if CONNACK specifies that CONNECT was rejected;
 * #IOT_MQTT_BAD_RESPONSE if the CONNACK packet doesn't follow MQTT spec.
 */
IotMqttError_t _IotMqtt_deserializeConnackWrapper( _mqttPacket_t * pConnack );


/**
 * @brief Deserialize a suback packet received from the network by using MQTT v4 beta 2 deserializer.
 * Converts the packet from a stream of bytes to an #IotMqttError_t and extracts
 * the packet identifier.
 *
 * @param[in,out] pSuback Pointer to an MQTT packet struct representing a SUBACK.
 *
 * @return #IOT_MQTT_SUCCESS if SUBACK is valid; #IOT_MQTT_BAD_RESPONSE
 * if the SUBACK packet doesn't follow MQTT spec.
 */
IotMqttError_t _IotMqtt_deserializeSubackWrapper( _mqttPacket_t * pSuback );


/**
 * @brief Deserialize a unsuback packet received from the network by using MQTT v4 beta 2 deserializer.
 * Converts the packet from a stream of bytes to an #IotMqttError_t and extracts
 * the packet identifier.
 *
 * @param[in,out] pUnsuback Pointer to an MQTT packet struct representing an UNSUBACK.
 *
 * @return #IOT_MQTT_SUCCESS if UNSUBACK is valid; #IOT_MQTT_BAD_RESPONSE
 * if the UNSUBACK packet doesn't follow MQTT spec.
 */
IotMqttError_t _IotMqtt_deserializeUnsubackWrapper( _mqttPacket_t * pUnsuback );

/**
 * @brief Deserialize a puback packet received from the network by using MQTT v4 beta 2 deserializer.
 *
 *
 * @param[in,out] pPuback Pointer to an MQTT packet struct representing a PUBACK.
 *
 * @return #IOT_MQTT_SUCCESS if PUBACK is valid; #IOT_MQTT_BAD_RESPONSE
 * if the PUBACK packet doesn't follow MQTT spec.
 */
IotMqttError_t _IotMqtt_deserializePubackWrapper( _mqttPacket_t * pPuback );

/**
 * @brief Deserialize a pingresp packet received from the network by using MQTT v4 beta 2 deserializer.
 * Converts the packet from a stream of bytes to an #IotMqttError_t.
 *
 * @param[in,out] pPingresp Pointer to an MQTT packet struct representing a PINGRESP.
 *
 * @return #IOT_MQTT_SUCCESS if PINGRESP is valid; #IOT_MQTT_BAD_RESPONSE
 * if the PINGRESP packet doesn't follow MQTT spec.
 */
IotMqttError_t _IotMqtt_deserializePingrespWrapper( _mqttPacket_t * pPingresp );

/**
 * @brief Deserialize a publish packet received from the network by using MQTT v4 beta 2 deserializer.
 * Converts the packet from a stream of bytes to an #IotMqttPublishInfo_t and
 * extracts the packet identifier.
 *
 * @param[in,out] pPublish Pointer to an MQTT packet struct representing a PUBLISH.
 *
 * @return #IOT_MQTT_SUCCESS if PUBLISH is valid; #IOT_MQTT_BAD_RESPONSE
 * if the PUBLISH packet doesn't follow MQTT spec.
 */
IotMqttError_t _IotMqtt_deserializePublishWrapper( _mqttPacket_t * pPublish );

/**
 * @brief Serialize a puback packet to send  on the network by using MQTT v4 beta 2 serializer.
 *
 * @param[in] packetIdentifier The packet id of the packet to be sent on the network.
 * @param[out] pPubackPacket The puback packet serialized to be sent on the network.
 * @param[out] pPacketSize The size of the puback packet.
 *
 * @return #IOT_MQTT_SUCCESS or #IOT_MQTT_NO_MEMORY.
 */
IotMqttError_t _IotMqtt_pubackSerializeWrapper( uint16_t packetIdentifier,
                                                uint8_t ** pPubackPacket,
                                                size_t * pPacketSize );

/*----------- Processing Operation after Sending the Packet Using MQTT LTS API for Shim-------*/

/**
 * @brief Process the MQTT operation after sending it on the network using MQTT LTS API.
 * Processing includes:
 * 1. Setting the status of operation based on the type of packet.
 * 2. Decrementing Operation references for waitable operation.
 * 3. Destroying Operation if job refernce count is 0.
 * 4. For non-waitable opeartion, transferring the operation from pending processing list to pending response list.
 * @param[in] pOperation The operation which needs to be processed after sending it on the network.
 *
 */
void _IotMqtt_ProcessOperation( _mqttOperation_t * pOperation );

/*----------------- MQTT Context and MQTT Connection Mapping Functions for Shim------------------*/

/**
 * @brief Get the MQTT Context from the given MQTT Connection.
 *
 * @param[in] mqttConnection The MQTT connection for which the context is needed.
 *
 * @return Index of the context from the mapping data structure used to store mapping of context and connection.
 */
int8_t _IotMqtt_getContextIndexFromConnection( IotMqttConnection_t mqttConnection );

/**
 * @brief Get the free index from the mapping data structure used to store mapping of MQTT Connection
 * used in the MQTT 201906.00 library and MQTT Context used to call MQTT LTS API to send packets
 * on the network.
 *
 * @return Free Index from the mapping data structure used to store mapping of context and connection.
 */
int8_t _IotMqtt_getFreeIndexFromContextConnectionArray( void );

/**
 * @brief Remove the MQTT Context from the given MQTT Connection.
 *
 * @param[in] mqttConnection The MQTT connection for which the context needs to be removed.
 */
void _IotMqtt_removeContext( IotMqttConnection_t mqttConnection );

/*----------------- MQTT 201906.00 wrapper functions using MQTT LTS API -----------------------*/

/**
 * @brief Disconnect the MQTT connection using MQTT LTS DISCONNECT API.
 *
 * @param[in] mqttConnection The MQTT connection which needs to be disconnected.
 *
 * @return #IOT_MQTT_NO_MEMORY if the #networkBuffer is too small to
 * hold the MQTT packet;
 * #IOT_MQTT_BAD_PARAMETER if invalid parameters are passed;
 * #IOT_MQTT_NETWORK_ERROR if transport send failed;
 * #IOT_MQTT_SUCCESS otherwise.
 */
IotMqttError_t _IotMqtt_managedDisconnect( IotMqttConnection_t mqttConnection );

/**
 * @brief Send the PUBLISH packet using MQTT LTS PUBLISH API.
 *
 * @param[in] mqttConnection The MQTT connection to be used.
 * @param[in] pOperation The MQTT operation having information for the PUBLISH operation.
 * @param[in] pPublishInfo Contains the information to be published.
 *
 * @return #IOT_MQTT_NO_MEMORY if the #networkBuffer is too small to
 * hold the MQTT packet;
 * #IOT_MQTT_BAD_PARAMETER if invalid parameters are passed;
 * #IOT_MQTT_NETWORK_ERROR if transport send failed;
 * #IOT_MQTT_SUCCESS otherwise.
 */
IotMqttError_t _IotMqtt_managedPublish( IotMqttConnection_t mqttConnection,
                                        _mqttOperation_t * pOperation,
                                        const IotMqttPublishInfo_t * pPublishInfo );

/**
 * @brief Send the SUBSCRIBE packet using MQTT LTS SUBSCRIBE API.
 *
 * @param[in] mqttConnection The MQTT connection to be used.
 * @param[in] pSubscriptionOperation The MQTT operation having information for the SUBSCRIBE operation.
 * @param[in] pSubscriptionList User-provided array of subscriptions.
 * @param[in] subscriptionCount Size of `pSubscriptionList`.
 *
 * @return #IOT_MQTT_NO_MEMORY if the #networkBuffer is too small to
 * hold the MQTT packet;
 * #IOT_MQTT_BAD_PARAMETER if invalid parameters are passed;
 * #IOT_MQTT_NETWORK_ERROR if transport send failed;
 * #IOT_MQTT_SUCCESS otherwise.
 */
IotMqttError_t _IotMqtt_managedSubscribe( IotMqttConnection_t mqttConnection,
                                          _mqttOperation_t * pSubscriptionOperation,
                                          const IotMqttSubscription_t * pSubscriptionList,
                                          size_t subscriptionCount );

/**
 * @brief Send the UNSUBSCRIBE packet using MQTT LTS UNSUBSCRIBE API.
 *
 * @param[in] mqttConnection The MQTT connection to be used.
 * @param[in] pUnsubscriptionOperation The MQTT operation having information for the UNSUBSCRIBE operation.
 * @param[in] pUnsubscriptionList User-provided array of subscriptions.
 * @param[in] UnsubscriptionCount Size of `pSubscriptionList`.
 *
 * @return #IOT_MQTT_NO_MEMORY if the #networkBuffer is too small to
 * hold the MQTT packet;
 * #IOT_MQTT_BAD_PARAMETER if invalid parameters are passed;
 * #IOT_MQTT_NETWORK_ERROR if transport send failed;
 * #IOT_MQTT_SUCCESS otherwise.
 */
IotMqttError_t _IotMqtt_managedUnsubscribe( IotMqttConnection_t mqttConnection,
                                            _mqttOperation_t * pUnsubscriptionOperation,
                                            const IotMqttSubscription_t * pUnsubscriptionList,
                                            size_t unsubscriptionCount );

/**
 * @brief Send the PINGREQ packet using MQTT LTS PINGREQ API.
 *
 * @param[in] mqttConnection The MQTT connection to be used.
 *
 * @return #IOT_MQTT_NO_MEMORY if the #networkBuffer is too small to
 * hold the MQTT packet;
 * #IOT_MQTT_BAD_PARAMETER if invalid parameters are passed;
 * #IOT_MQTT_NETWORK_ERROR if transport send failed;
 * #IOT_MQTT_SUCCESS otherwise.
 */
IotMqttError_t _IotMqtt_managedPing( IotMqttConnection_t mqttConnection );

/*-----------------------------------------------------------*/

/**
 *  @brief Convert the MQTT LTS library status to MQTT 201906.00 status Code.
 *
 *  @param[in] managedMqttStatus The status code in MQTT LTS library status which needs to be converted to IOT MQTT status code.
 *
 *  @return #IOT_MQTT_SUCCESS, #IOT_MQTT_NETWORK_ERROR, #IOT_MQTT_NO_MEMORY, #IOT_MQTT_STATUS_PENDING, #IOT_MQTT_INIT_FAILED
 *  #IOT_MQTT_SCHEDULING_ERROR, #IOT_MQTT_BAD_RESPONSE, #IOT_MQTT_TIMEOUT, #IOT_MQTT_SERVER_REFUSED, #IOT_MQTT_RETRY_NO_RESPONSE.
 */
IotMqttError_t convertReturnCode( MQTTStatus_t managedMqttStatus );


/*-----------------------------------Subscription and Operation array container functions----------------------*/

/**
 * @brief Get free index from the subscription array to insert the new subscription.
 *
 * @param[in] pSubscriptionArray Subscription array in which the new subscription to be inserted.
 *
 * @return The index where the subscription to be inserted.
 */
int8_t IotMqtt_GetFreeIndexInSubscriptionArray( _mqttSubscription_t * pSubscriptionArray );

/**
 * @brief Remove the subscription in the subscription array.
 *
 * @param[in] pSubscriptionArray Subscription array from which the subscription needs to be removed.
 * @param[in] deleteIndex The index position from where the subscription to be removed.
 *
 * @return 'true' if subscription is removed else 'false'.
 */
bool IotMqtt_RemoveSubscription( _mqttSubscription_t * pSubscriptionArray,
                                 int8_t deleteIndex );

/**
 * @brief Remove all the matching subscriptions in the given subscription array.
 *
 * @param[in] pSubscriptionArray Subscription array from which the subscriptions to be removed.
 * @param[in] pMatch If `pMatch` is `NULL`, all the subscriptions will be removed.
 * Otherwise, it is used to find the matching subscription.
 *
 */
void IotMqtt_RemoveAllMatches( _mqttSubscription_t * pSubscriptionArray,
                               const _packetMatchParams_t * pMatch );

/**
 * @brief Find the first matching subscription in the given subscription array, starting at the given starting point.
 *
 * @param[in] pSubscriptionArray Subscription array from which the subscription needs to be matched.
 * @param[in] startIndex An element in `pSubscriptionArray`. Only elements starting from this one and
 * the end of the array are checked. Pass 0 to search from the beginning of the array.
 * @param[in] pMatch Contains the parameters used for matching the subscription.
 *
 * @return The first matching subscription from the subscription array.
 */
int8_t IotMqtt_FindFirstMatch( _mqttSubscription_t * pSubscriptionArray,
                               int8_t startIndex,
                               _topicMatchParams_t * pMatch );

/*-----------------------------------Mutexes Wrappers--------------------------------------------*/

/**
 * @brief Create a non recursive mutex.
 *
 * @param[out] pMutex Handle of the mutex created.
 * @param[in] pxMutexBuffer Static storage for the semaphore.
 *
 * @return true if mutex is created else false
 */
bool IotMutex_CreateNonRecursiveMutex( SemaphoreHandle_t * pMutex,
                                       StaticSemaphore_t * pxMutexBuffer );

/**
 * @brief Create a recursive mutex.
 *
 * @param[out] pMutex Mutex handle of the created mutex.
 * @param[in] pxMutexBuffer Static storage for the semaphore.
 *
 * @return true if mutex is created else false
 */
bool IotMutex_CreateRecursiveMutex( SemaphoreHandle_t * pMutex,
                                    StaticSemaphore_t * pxMutexBuffer );

/**
 * @brief Take a non recursive mutex.
 *
 * @param[in] pMutex Mutex handle.
 *
 * @return true if mutex is created else false
 */
bool IotMutex_Take( SemaphoreHandle_t * pMutex );

/**
 * @brief Give a non recursive mutex.
 *
 * @param[in] pMutex Mutex handle.
 *
 * @return true if mutex is created else false
 */
bool IotMutex_Give( SemaphoreHandle_t * pMutex );

/**
 * @brief Take a non recursive mutex.
 *
 * @param[in] pMutex Mutex handle.
 *
 * @return true if mutex is created else false
 */
bool IotMutex_TakeRecursive( SemaphoreHandle_t * pMutex );

/**
 * @brief Give a non recursive mutex.
 *
 * @param[in] pMutex Mutex handle.
 *
 * @return true if mutex is created else false
 */
bool IotMutex_GiveRecursive( SemaphoreHandle_t * pMutex );

/**
 * @brief Delete the mutex.
 *
 * @param[in] pMutex Mutex handle.
 *
 * @return true if mutex is created else false
 */
void IotMutex_Delete( SemaphoreHandle_t * pMutex );

#endif /* ifndef IOT_MQTT_INTERNAL_H_ */
