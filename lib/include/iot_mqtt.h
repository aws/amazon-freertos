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
 * @file iot_mqtt.h
 * @brief User-facing functions and structs of the MQTT 3.1.1 library.
 */

#ifndef _IOT_MQTT_H_
#define _IOT_MQTT_H_

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* Platform network include. */
#include "platform/iot_network.h"

/*---------------------------- MQTT handle types ----------------------------*/

/**
 * @handles{mqtt,MQTT library}
 */

/**
 * @ingroup mqtt_datatypes_handles
 * @brief Opaque handle of an MQTT connection.
 *
 * This type identifies an MQTT connection, which is valid after a successful call
 * to @ref mqtt_function_connect. A variable of this type is passed as the first
 * argument to [MQTT library functions](@ref mqtt_functions) to identify which
 * connection that function acts on.
 *
 * A call to @ref mqtt_function_disconnect makes a connection handle invalid. Once
 * @ref mqtt_function_disconnect returns, the connection handle should no longer
 * be used.
 *
 * @initializer{IotMqttConnection_t,IOT_MQTT_CONNECTION_INITIALIZER}
 */
typedef void * IotMqttConnection_t;

/**
 * @ingroup mqtt_datatypes_handles
 * @brief Opaque handle that references an in-progress MQTT operation.
 *
 * Set as an output parameter of @ref mqtt_function_publish, @ref mqtt_function_subscribe,
 * and @ref mqtt_function_unsubscribe. These functions queue an MQTT operation; the result
 * of the operation is unknown until a response from the MQTT server is received. Therefore,
 * this handle serves as a reference to MQTT operations awaiting MQTT server response.
 *
 * This reference will be valid from the successful return of @ref mqtt_function_publish,
 * @ref mqtt_function_subscribe, or @ref mqtt_function_unsubscribe. The reference becomes
 * invalid once the [completion callback](@ref IotMqttCallbackInfo_t) is invoked, or
 * @ref mqtt_function_wait returns.
 *
 * @initializer{IotMqttReference_t,IOT_MQTT_REFERENCE_INITIALIZER}
 *
 * @see @ref mqtt_function_wait and #IOT_MQTT_FLAG_WAITABLE for waiting on a reference.
 * #IotMqttCallbackInfo_t and #IotMqttCallbackParam_t for an asynchronous notification
 * of completion.
 */
typedef void * IotMqttReference_t;

/*-------------------------- MQTT enumerated types --------------------------*/

/**
 * @enums{mqtt,MQTT library}
 */

/**
 * @ingroup mqtt_datatypes_enums
 * @brief Return codes of [MQTT functions](@ref mqtt_functions).
 *
 * The function @ref mqtt_function_strerror can be used to get a return code's
 * description.
 */
typedef enum IotMqttError
{
    /**
     * @brief MQTT operation completed successfully.
     *
     * Functions that may return this value:
     * - @ref mqtt_function_connect
     * - @ref mqtt_function_publish with QoS 0 parameter
     * - @ref mqtt_function_wait
     * - @ref mqtt_function_timedsubscribe
     * - @ref mqtt_function_timedunsubscribe
     * - @ref mqtt_function_timedpublish
     *
     * Will also be the value of an operation completion callback's
     * #IotMqttCallbackParam_t.result when successful.
     */
    IOT_MQTT_SUCCESS = 0,

    /**
     * @brief MQTT operation queued, awaiting result.
     *
     * Functions that may return this value:
     * - @ref mqtt_function_subscribe
     * - @ref mqtt_function_unsubscribe
     * - @ref mqtt_function_publish with QoS 1 parameter
     */
    IOT_MQTT_STATUS_PENDING,

    /**
     * @brief Initialization failed.
     *
     * Functions that may return this value:
     * - @ref mqtt_function_init
     */
    IOT_MQTT_INIT_FAILED,

    /**
     * @brief At least one parameter is invalid.
     *
     * Functions that may return this value:
     * - @ref mqtt_function_connect
     * - @ref mqtt_function_subscribe and @ref mqtt_function_timedsubscribe
     * - @ref mqtt_function_unsubscribe and @ref mqtt_function_timedunsubscribe
     * - @ref mqtt_function_publish and @ref mqtt_function_timedpublish
     * - @ref mqtt_function_wait
     */
    IOT_MQTT_BAD_PARAMETER,

    /**
     * @brief MQTT operation failed because of memory allocation failure.
     *
     * Functions that may return this value:
     * - @ref mqtt_function_connect
     * - @ref mqtt_function_subscribe and @ref mqtt_function_timedsubscribe
     * - @ref mqtt_function_unsubscribe and @ref mqtt_function_timedunsubscribe
     * - @ref mqtt_function_publish and @ref mqtt_function_timedpublish
     */
    IOT_MQTT_NO_MEMORY,

    /**
     * @brief MQTT operation failed because the network was unusable.
     *
     * This return value may indicate that the network is disconnected.
     *
     * Functions that may return this value:
     * - @ref mqtt_function_connect
     * - @ref mqtt_function_wait
     * - @ref mqtt_function_timedsubscribe
     * - @ref mqtt_function_timedunsubscribe
     * - @ref mqtt_function_timedpublish
     *
     * May also be the value of an operation completion callback's
     * #IotMqttCallbackParam_t.result.
     */
    IOT_MQTT_NETWORK_ERROR,

    /**
     * @brief MQTT operation could not be scheduled, i.e. enqueued for sending.
     *
     * Functions that may return this value:
     * - @ref mqtt_function_connect
     * - @ref mqtt_function_subscribe and @ref mqtt_function_timedsubscribe
     * - @ref mqtt_function_unsubscribe and @ref mqtt_function_timedunsubscribe
     * - @ref mqtt_function_publish and @ref mqtt_function_timedpublish
     */
    IOT_MQTT_SCHEDULING_ERROR,

    /**
     * @brief MQTT response packet received from the network is malformed.
     *
     * Functions that may return this value:
     * - @ref mqtt_function_connect
     * - @ref mqtt_function_wait
     * - @ref mqtt_function_timedsubscribe
     * - @ref mqtt_function_timedunsubscribe
     * - @ref mqtt_function_timedpublish
     *
     * May also be the value of an operation completion callback's
     * #IotMqttCallbackParam_t.result.
     *
     * @note If this value is received, the network connection has been closed
     * (unless a [disconnect function](@ref IotMqttNetIf_t.disconnect) was not
     * provided to @ref mqtt_function_connect).
     */
    IOT_MQTT_BAD_RESPONSE,

    /**
     * @brief A blocking MQTT operation timed out.
     *
     * Functions that may return this value:
     * - @ref mqtt_function_connect
     * - @ref mqtt_function_wait
     * - @ref mqtt_function_timedsubscribe
     * - @ref mqtt_function_timedunsubscribe
     * - @ref mqtt_function_timedpublish
     */
    IOT_MQTT_TIMEOUT,

    /**
     * @brief A CONNECT or at least one subscription was refused by the server.
     *
     * Functions that may return this value:
     * - @ref mqtt_function_connect
     * - @ref mqtt_function_wait, but only when its #IotMqttReference_t parameter
     * is associated with a SUBSCRIBE operation.
     * - @ref mqtt_function_timedsubscribe
     *
     * May also be the value of an operation completion callback's
     * #IotMqttCallbackParam_t.result for a SUBSCRIBE.
     *
     * @note If this value is returned and multiple subscriptions were passed to
     * @ref mqtt_function_subscribe (or @ref mqtt_function_timedsubscribe), it's
     * still possible that some of the subscriptions succeeded. This value only
     * signifies that AT LEAST ONE subscription was rejected. The function @ref
     * mqtt_function_issubscribed can be used to determine which subscriptions
     * were accepted or rejected.
     */
    IOT_MQTT_SERVER_REFUSED,

    /**
     * @brief A QoS 1 PUBLISH received no response and [the retry limit]
     * (#IotMqttPublishInfo_t.retryLimit) was reached.
     *
     * Functions that may return this value:
     * - @ref mqtt_function_wait, but only when its #IotMqttReference_t parameter
     * is associated with a QoS 1 PUBLISH operation
     * - @ref mqtt_function_timedpublish
     *
     * May also be the value of an operation completion callback's
     * #IotMqttCallbackParam_t.result for a QoS 1 PUBLISH.
     */
    IOT_MQTT_RETRY_NO_RESPONSE
} IotMqttError_t;

/**
 * @ingroup mqtt_datatypes_enums
 * @brief Types of MQTT operations.
 *
 * The function @ref mqtt_function_operationtype can be used to get an operation
 * type's description.
 */
typedef enum IotMqttOperationType
{
    IOT_MQTT_CONNECT,           /**< Client-to-server CONNECT. */
    IOT_MQTT_PUBLISH_TO_SERVER, /**< Client-to-server PUBLISH. */
    IOT_MQTT_PUBACK,            /**< Client-to-server PUBACK. */
    IOT_MQTT_SUBSCRIBE,         /**< Client-to-server SUBSCRIBE. */
    IOT_MQTT_UNSUBSCRIBE,       /**< Client-to-server UNSUBSCRIBE. */
    IOT_MQTT_PINGREQ,           /**< Client-to-server PINGREQ. */
    IOT_MQTT_DISCONNECT         /**< Client-to-server DISCONNECT. */
} IotMqttOperationType_t;

/**
 * @ingroup mqtt_datatypes_enums
 * @brief Quality of service levels for MQTT PUBLISH messages.
 *
 * All MQTT PUBLISH messages, including Last Will and Testament and messages
 * received on subscription filters, have an associated <i>Quality of Service</i>,
 * which defines any delivery guarantees for that message.
 * - QoS 0 messages will be delivered at most once. This is a "best effort"
 * transmission with no retransmissions.
 * - QoS 1 messages will be delivered at least once. See #IotMqttPublishInfo_t
 * for the retransmission strategy this library uses to redeliver messages
 * assumed to be lost.
 *
 * @attention QoS 2 is not supported by this library and should not be used.
 */
typedef enum IotMqttQos
{
    IOT_MQTT_QOS_0 = 0, /**< Delivery at most once. */
    IOT_MQTT_QOS_1 = 1, /**< Delivery at least once. See #IotMqttPublishInfo_t for client-side retry strategy. */
    IOT_MQTT_QOS_2 = 2  /**< Delivery exactly once. Unsupported, but enumerated for completeness. */
} IotMqttQos_t;

/*------------------------- MQTT parameter structs --------------------------*/

/**
 * @paramstructs{mqtt,MQTT}
 */

/**
 * @ingroup mqtt_datatypes_paramstructs
 * @brief Information on a PUBLISH message.
 *
 * @paramfor @ref mqtt_function_connect, @ref mqtt_function_publish
 *
 * Passed to @ref mqtt_function_publish as the message to publish and @ref
 * mqtt_function_connect as the Last Will and Testament (LWT) message.
 *
 * @initializer{IotMqttPublishInfo_t,IOT_MQTT_PUBLISH_INFO_INITIALIZER}
 *
 * #IotMqttPublishInfo_t.retryMs and #IotMqttPublishInfo_t.retryLimit are only
 * relevant to QoS 1 PUBLISH messages. They are ignored for QoS 0 PUBLISH
 * messages and LWT messages. These members control retransmissions of QoS 1
 * messages under the following rules:
 * - Retransmission is disabled when #IotMqttPublishInfo_t.retryLimit is 0.
 * After sending the PUBLISH, the library will wait indefinitely for a PUBACK.
 * - If #IotMqttPublishInfo_t.retryLimit is greater than 0, then QoS 1 publishes
 * that do not receive a PUBACK within #IotMqttPublishInfo_t.retryMs will be
 * retransmitted, up to #IotMqttPublishInfo_t.retryLimit times.
 *
 * Retransmission follows a truncated exponential backoff strategy. The constant
 * @ref IOT_MQTT_RETRY_MS_CEILING controls the maximum time between retransmissions.
 *
 * After #IotMqttPublishInfo_t.retryLimit retransmissions are sent, the MQTT
 * library will wait @ref IOT_MQTT_RESPONSE_WAIT_MS before a final check
 * for a PUBACK. If no PUBACK was received within this time, the QoS 1 PUBLISH
 * fails with the code #IOT_MQTT_RETRY_NO_RESPONSE.
 *
 * @note The lengths of the strings in this struct should not include the NULL
 * terminator. Strings in this struct do not need to be NULL-terminated.
 *
 * @note The AWS IoT MQTT server does not support the DUP bit. When
 * [using this library with the AWS IoT MQTT server](@ref IotMqttConnectInfo_t.awsIotMqttMode),
 * retransmissions will instead be sent with a new packet identifier in the PUBLISH
 * packet. This is a nonstandard workaround. Note that this workaround has some
 * flaws, including the following:
 * - The previous packet identifier is forgotten, so if a PUBACK arrives for that
 * packet identifier, it will be ignored. On an exceptionally busy network, this
 * may cause excessive retransmissions when too many PUBACKS arrive after the
 * PUBLISH packet identifier is changed. However, the exponential backoff
 * retransmission strategy should mitigate this problem.
 * - Log messages will be printed using the new packet identifier; the old packet
 * identifier is not saved.
 *
 * <b>Example</b>
 *
 * Consider a situation where
 * - @ref IOT_MQTT_RETRY_MS_CEILING is 60000
 * - #IotMqttPublishInfo_t.retryMs is 2000
 * - #IotMqttPublishInfo_t.retryLimit is 20
 *
 * A PUBLISH message will be retransmitted at the following times after the initial
 * transmission if no PUBACK is received:
 * - 2000 ms (2000 ms after previous transmission)
 * - 6000 ms (4000 ms after previous transmission)
 * - 14000 ms (8000 ms after previous transmission)
 * - 30000 ms (16000 ms after previous transmission)
 * - 62000 ms (32000 ms after previous transmission)
 * - 122000 ms, 182000 ms, 242000 ms... (every 60000 ms until 20 transmissions have been sent)
 *
 * After the 20th retransmission, the MQTT library will wait
 * @ref IOT_MQTT_RESPONSE_WAIT_MS before checking a final time for a PUBACK.
 */
typedef struct IotMqttPublishInfo
{
    IotMqttQos_t qos;          /**< @brief QoS of message. Must be 0 or 1. */
    bool retain;               /**< @brief MQTT message retain flag. */

    const char * pTopicName;   /**< @brief Topic name of PUBLISH. */
    uint16_t topicNameLength;  /**< @brief Length of #IotMqttPublishInfo_t.pTopicName. */

    const void * pPayload;     /**< @brief Payload of PUBLISH. */
    size_t payloadLength;      /**< @brief Length of #IotMqttPublishInfo_t.pPayload. For LWT messages, this is limited to 65535. */

    uint64_t retryMs;          /**< @brief If no response is received within this time, the message is retransmitted. */
    uint32_t retryLimit;       /**< @brief How many times to attempt retransmission. */
} IotMqttPublishInfo_t;

/**
 * @ingroup mqtt_datatypes_paramstructs
 * @brief Parameter to an MQTT callback function.
 *
 * @paramfor MQTT callback functions
 *
 * The MQTT library passes this struct to registered callback whenever an
 * operation completes or a message is received on a topic filter.
 *
 * The members of this struct are different based on the callback trigger. If the
 * callback function was triggered for completed operation, the `operation`
 * member is valid. Otherwise, if the callback was triggered because of a
 * server-to-client PUBLISH, the `message` member is valid.
 *
 * For an incoming PUBLISH, the `message.pTopicFilter` parameter provides the
 * subscription topic filter that matched the topic name in the PUBLISH. Because
 * topic filters may use MQTT wildcards, the topic filter may be different from the
 * topic name. This pointer must be treated as read-only; the topic filter must not
 * be modified. Additionally, the topic filter may go out of scope as soon as the
 * callback function returns, so it must be copied if it is needed at a later time.
 *
 * @attention Any pointers in this callback parameter may be freed as soon as
 * the [callback function](@ref IotMqttCallbackInfo_t.function) returns.
 * Therefore, data must be copied if it is needed after the callback function
 * returns.
 * @attention The MQTT library may set strings that are not NULL-terminated.
 *
 * @see #IotMqttCallbackInfo_t for the signature of a callback function.
 */
typedef struct IotMqttCallbackParam
{
    /**
     * @brief The MQTT connection associated with this completed operation or
     * incoming PUBLISH.
     *
     * [MQTT API functions](@ref mqtt_functions) are safe to call from a callback.
     * However, blocking function calls (including @ref mqtt_function_wait) are
     * not recommended (though still safe).
     */
    IotMqttConnection_t mqttConnection;

    union
    {
        /* Valid for completed operations. */
        struct
        {
            IotMqttOperationType_t type;      /**< @brief Type of operation that completed. */
            IotMqttReference_t reference;     /**< @brief Reference to the operation that completed. */
            IotMqttError_t result;            /**< @brief Result of operation, e.g. succeeded or failed. */
        } operation;

        /* Valid for incoming PUBLISH messages. */
        struct
        {
            const char * pTopicFilter;           /**< @brief Topic filter that matched the message. */
            uint16_t topicFilterLength;          /**< @brief Length of `pTopicFilter`. */
            IotMqttPublishInfo_t info;        /**< @brief PUBLISH message received from the server. */
        } message;
    };
} IotMqttCallbackParam_t;

/**
 * @ingroup mqtt_datatypes_paramstructs
 * @brief Information on a user-provided MQTT callback function.
 *
 * @paramfor @ref mqtt_function_subscribe, @ref mqtt_function_unsubscribe,
 * and @ref mqtt_function_publish. Cannot be used with #IOT_MQTT_FLAG_WAITABLE.
 *
 * Provides a function to be invoked when an operation completes or when a
 * server-to-client PUBLISH is received.
 *
 * @initializer{IotMqttCallbackInfo_t,IOT_MQTT_CALLBACK_INFO_INITIALIZER}
 *
 * Below is an example for receiving an asynchronous notification on operation
 * completion. See @ref mqtt_function_subscribe for an example of using this struct
 * with for incoming PUBLISH messages.
 *
 * @code{c}
 * // Operation completion callback.
 * void operationComplete( void * pArgument, IotMqttCallbackParam_t * pOperation );
 *
 * // Callback information.
 * IotMqttCallbackInfo_t callbackInfo = IOT_MQTT_CALLBACK_INFO_INITIALIZER;
 * callbackInfo.function = operationComplete;
 *
 * // Operation to wait for.
 * IotMqttError_t result = IotMqtt_Publish( mqttConnection,
 *                                          &publishInfo,
 *                                          0,
 *                                          &callbackInfo,
 *                                          &reference );
 *
 * // Publish should have returned IOT_MQTT_STATUS_PENDING. Once a response
 * // is received, operationComplete is executed with the actual status passed
 * // in pOperation.
 * @endcode
 */
typedef struct IotMqttCallbackInfo
{
    void * param1; /**< @brief The first parameter to pass to the callback function. */

    /**
     * @brief User-provided callback function signature.
     *
     * @param[in] void * #IotMqttCallbackInfo_t.param1
     * @param[in] IotMqttCallbackParam_t * Details on the outcome of the MQTT operation
     * or an incoming MQTT PUBLISH.
     *
     * @see #IotMqttCallbackParam_t for more information on the second parameter.
     */
    void ( * function )( void *,
                         IotMqttCallbackParam_t * );
} IotMqttCallbackInfo_t;

/**
 * @ingroup mqtt_datatypes_paramstructs
 * @brief Information on an MQTT subscription.
 *
 * @paramfor @ref mqtt_function_subscribe, @ref mqtt_function_unsubscribe
 *
 * An array of these is passed to @ref mqtt_function_subscribe and @ref
 * mqtt_function_unsubscribe. However, #IotMqttSubscription_t.callback and
 * and #IotMqttSubscription_t.qos are ignored by @ref mqtt_function_unsubscribe.
 *
 * @initializer{IotMqttSubscription_t,IOT_MQTT_SUBSCRIPTION_INITIALIZER}
 *
 * @note The lengths of the strings in this struct should not include the NULL
 * terminator. Strings in this struct do not need to be NULL-terminated.
 * @see #IotMqttCallbackInfo_t for details on setting a callback function.
 */
typedef struct IotMqttSubscription
{
    /**
     * @brief QoS of messages delivered on subscription.
     *
     * Must be `0` or `1`. Ignored by @ref mqtt_function_unsubscribe.
     */
    IotMqttQos_t qos;

    const char * pTopicFilter;         /**< @brief Topic filter of subscription. */
    uint16_t topicFilterLength;        /**< @brief Length of #IotMqttSubscription_t.pTopicFilter. */

    /**
     * @brief Callback to invoke when a message is received.
     *
     * See #IotMqttCallbackInfo_t. Ignored by @ref mqtt_function_unsubscribe.
     */
    IotMqttCallbackInfo_t callback;
} IotMqttSubscription_t;

/**
 * @ingroup mqtt_datatypes_paramstructs
 * @brief Information on a new MQTT connection.
 *
 * @paramfor @ref mqtt_function_connect
 *
 * Passed as an argument to @ref mqtt_function_connect. Most members of this struct
 * correspond to the content of an [MQTT CONNECT packet.]
 * (http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/csprd02/mqtt-v3.1.1-csprd02.html#_Toc385349764)
 *
 * @initializer{IotMqttConnectInfo_t,IOT_MQTT_CONNECT_INFO_INITIALIZER}
 *
 * @note The lengths of the strings in this struct should not include the NULL
 * terminator. Strings in this struct do not need to be NULL-terminated.
 */
typedef struct IotMqttConnectInfo
{
    /**
     * @brief Specifies if this MQTT connection is to an AWS IoT MQTT server.
     *
     * The AWS IoT MQTT broker [differs somewhat from the MQTT specification.]
     * (https://docs.aws.amazon.com/iot/latest/developerguide/mqtt.html)
     * When this member is `true`, the MQTT library will accommodate these
     * differences. This setting should be `false` when communicating with a
     * fully-compliant MQTT broker.
     *
     * @attention This setting <b>MUST</b> be `true` when using the AWS IoT MQTT
     * server; it <b>MUST</b> be `false` otherwise.
     * @note Currently, @ref IOT_MQTT_CONNECT_INFO_INITIALIZER sets this
     * this member to `true`.
     */
    bool awsIotMqttMode;

    /**
     * @brief Whether this connection is a clean session.
     *
     * MQTT servers can maintain and topic filter subscriptions and unacknowledged
     * PUBLISH messages. These form part of an <i>MQTT session</i>, which is identified by
     * the [client identifier](@ref IotMqttConnectInfo_t.pClientIdentifier).
     *
     * Setting this value to `true` establishes a <i>clean session</i>, which causes
     * the MQTT server to discard any previous session data for a client identifier.
     * When the client disconnects, the server discards all session data. If this
     * value is `true`, #IotMqttConnectInfo_t.pPreviousSubscriptions and
     * #IotMqttConnectInfo_t.previousSubscriptionCount are ignored.
     *
     * Setting this value to `false` does one of the following:
     * - If no previous session exists, the MQTT server will create a new
     * <i>persistent session</i>. The server may maintain subscriptions and
     * unacknowledged PUBLISH messages after a client disconnects, to be restored
     * once the same client identifier reconnects.
     * - If a previous session exists, the MQTT server restores all of the session's
     * subscriptions for the client identifier and may immediately transmit any
     * unacknowledged PUBLISH packets to the client.
     *
     * When a client with a persistent session disconnects, the MQTT server
     * continues to maintain all subscriptions and unacknowledged PUBLISH messages.
     * The client must also remember the session subscriptions to restore them
     * upon reconnecting. #IotMqttConnectInfo_t.pPreviousSubscriptions and
     * #IotMqttConnectInfo_t.previousSubscriptionCount are used to restore a
     * previous session's subscriptions client-side.
     */
    bool cleanSession;

    /**
     * @brief An array of MQTT subscriptions present in a previous session, if any.
     *
     * Pointer to the start of an array of subscriptions present a previous session,
     * if any. These subscriptions will be immediately restored upon reconnecting.
     *
     * This member is ignored if it is `NULL` or #IotMqttConnectInfo_t.cleanSession
     * is `true`. If this member is not `NULL`, #IotMqttConnectInfo_t.previousSubscriptionCount
     * must be nonzero.
     */
    const IotMqttSubscription_t * pPreviousSubscriptions;

    /**
     * @brief The number of MQTT subscriptions present in a previous session, if any.
     *
     * Number of subscriptions contained in the array
     * #IotMqttConnectInfo_t.pPreviousSubscriptions.
     *
     * This value is ignored if #IotMqttConnectInfo_t.pPreviousSubscriptions
     * is `NULL` or #IotMqttConnectInfo_t.cleanSession is `true`. If
     * #IotMqttConnectInfo_t.pPreviousSubscriptions is not `NULL`, this value
     * must be nonzero.
     */
    size_t previousSubscriptionCount;

    /**
     * @brief A message to publish if the new MQTT connection is unexpectedly closed.
     *
     * A Last Will and Testament (LWT) message may be published if this connection is
     * closed without sending an MQTT DISCONNECT packet. This pointer should be set to
     * an #IotMqttPublishInfo_t representing any LWT message to publish. If an LWT
     * is not needed, this member must be set to `NULL`.
     *
     * Unlike other PUBLISH messages, an LWT message is limited to 65535 bytes in
     * length. Additionally, [pWillInfo->retryMs](@ref IotMqttPublishInfo_t.retryMs)
     * and [pWillInfo->retryLimit](@ref IotMqttPublishInfo_t.retryLimit) will
     * be ignored.
     */
    const IotMqttPublishInfo_t * pWillInfo;

    uint16_t keepAliveSeconds;       /**< @brief Period of keep-alive messages. Set to 0 to disable keep-alive. */

    const char * pClientIdentifier;  /**< @brief MQTT client identifier. */
    uint16_t clientIdentifierLength; /**< @brief Length of #IotMqttConnectInfo_t.pClientIdentifier. */

    /* These credentials are not used by AWS IoT and may be ignored if
     * awsIotMqttMode is true. */
    const char * pUserName;          /**< @brief Username for MQTT connection. */
    uint16_t userNameLength;         /**< @brief Length of #IotMqttConnectInfo_t.pUserName. */
    const char * pPassword;          /**< @brief Password for MQTT connection. */
    uint16_t passwordLength;         /**< @brief Length of #IotMqttConnectInfo_t.pPassword. */
} IotMqttConnectInfo_t;

/**
 * @ingroup mqtt_datatypes_paramstructs
 * @brief Provides the network functions for sending data and closing connections.
 *
 * @paramfor @ref mqtt_function_connect
 *
 * The MQTT library needs to be able to send and receive data over a network.
 * This struct provides the functions (as function pointers) for sending data
 * and closing the network connection. In addition to providing this struct to
 * @ref mqtt_function_connect, the function @ref mqtt_function_receivecallback
 * should be called to process data received from the network.
 *
 * @initializer{IotMqttNetIf_t,IOT_MQTT_NETIF_INITIALIZER}
 */
typedef struct IotMqttNetIf
{
    void * pSendContext;       /**< Passed as the first argument to #IotMqttNetIf_t.send. */
    void * pDisconnectContext; /**< Passed as the first argument to #IotMqttNetIf_t.disconnect. */

    /**
     * @brief Function that sends data on the network.
     *
     * @param[in] void * #IotMqttNetIf_t.pSendContext
     * @param[in] const void * Pointer to the data to send.
     * @param[in] size_t Size of the data to send.
     *
     * @return Number of bytes successfully sent, 0 on failure.
     */
    size_t ( * send )( void *,
                       const uint8_t *,
                       size_t );

    /**
     * @brief Function that closes the network connection.
     *
     * If this function is not provided, the network connection will not be closed
     * by the MQTT library.
     *
     * @param[in] void * #IotMqttNetIf_t.pDisconnectContext
     *
     * @note Optional; set to `NULL` to ignore. The MQTT spec states that connections
     * must be closed in certain conditions; if this function is not provided, the
     * MQTT library is noncompliant.
     */
    IotNetworkError_t ( * disconnect )( void * );

    /*
     * In addition to providing the network send and disconnect functions, this
     * struct also allows the MQTT serialization and deserialization functions
     * to be overridden for an MQTT connection. The compile-time setting
     * IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES must be 1 to enable this
     * functionality. See the IotMqttNetIf_t.serialize and
     * IotMqttNetIf_t.deserialize members for a list of functions that can be
     * overridden. In addition, the functions for freeing packets and determining
     * the packet type can also be overridden. If
     * IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES is 1, the serializer initialization
     * and cleanup functions may be extended. See documentation of
     * IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES for more information.
     *
     * If any function pointers that are NULL (the default value set by
     * IOT_MQTT_NETIF_INITIALIZER), then the default implementation of that
     * function will be used.
     */
    #if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1

        struct
        {
            /**
             * @brief CONNECT packet serializer function.
             * @param[in] IotMqttConnectInfo_t* User-provided CONNECT information.
             * @param[out] uint8_t** Where the CONNECT packet is written.
             * @param[out] size_t* Size of the CONNECT packet.
             *
             * <b>Default implementation:</b> #_IotMqtt_SerializeConnect
             */
            IotMqttError_t ( * connect )( const IotMqttConnectInfo_t * /* pConnectInfo */,
                                          uint8_t ** /* pConnectPacket */,
                                          size_t * /* pPacketSize */ );

            /**
             * @brief PUBLISH packet serializer function.
             * @param[in] IotMqttPublishInfo_t* User-provided PUBLISH information.
             * @param[out] uint8_t** Where the PUBLISH packet is written.
             * @param[out] size_t* Size of the PUBLISH packet.
             * @param[out] uint16_t* The packet identifier generated for this PUBLISH.
             *
             * <b>Default implementation:</b> #_IotMqtt_SerializePublish
             */
            IotMqttError_t ( * publish )( const IotMqttPublishInfo_t * /* pPublishInfo */,
                                          uint8_t ** /* pPublishPacket */,
                                          size_t * /* pPacketSize */,
                                          uint16_t * /* pPacketIdentifier */ );

            /**
             * @brief Set the `DUP` bit in a QoS `1` PUBLISH packet.
             * @param[in] bool Specifies if this PUBLISH packet is being sent to
             * an AWS IoT MQTT server.
             * @param[in] uint8_t* Pointer to the PUBLISH packet to modify.
             * @param[out] uint16_t* New packet identifier (AWS IoT MQTT mode only).
             *
             * <b>Default implementation:</b> #_IotMqtt_PublishSetDup
             */
            void ( * publishSetDup )( bool /* awsIotMqttMode */,
                                      uint8_t * /* pPublishPacket */,
                                      uint16_t * /* pNewPacketIdentifier */ );

            /**
             * @brief PUBACK packet serializer function.
             * @param[in] uint16_t The packet identifier to place in PUBACK.
             * @param[out] uint8_t** Where the PUBACK packet is written.
             * @param[out] size_t* Size of the PUBACK packet.
             *
             * <b>Default implementation:</b> #_IotMqtt_SerializePuback
             */
            IotMqttError_t ( * puback )( uint16_t /* packetIdentifier */,
                                         uint8_t ** /* pPubackPacket */,
                                         size_t * /* pPacketSize */ );

            /**
             * @brief SUBSCRIBE packet serializer function.
             * @param[in] IotMqttSubscription_t* User-provided array of subscriptions.
             * @param[in] size_t Number of elements in the subscription array.
             * @param[out] uint8_t** Where the SUBSCRIBE packet is written.
             * @param[out] size_t* Size of the SUBSCRIBE packet.
             * @param[out] uint16_t* The packet identifier generated for this SUBSCRIBE.
             *
             * <b>Default implementation:</b> #_IotMqtt_SerializeSubscribe
             */
            IotMqttError_t ( * subscribe )( const IotMqttSubscription_t * /* pSubscriptionList */,
                                            size_t /* subscriptionCount */,
                                            uint8_t ** /* pSubscribePacket */,
                                            size_t * /* pPacketSize */,
                                            uint16_t * /* pPacketIdentifier */ );

            /**
             * @brief UNSUBSCRIBE packet serializer function.
             * @param[in] IotMqttSubscription_t* User-provided array of subscriptions to remove.
             * @param[in] size_t Number of elements in the subscription array.
             * @param[out] uint8_t** Where the UNSUBSCRIBE packet is written.
             * @param[out] size_t* Size of the UNSUBSCRIBE packet.
             * @param[out] uint16_t* The packet identifier generated for this UNSUBSCRIBE.
             *
             * <b>Default implementation:</b> #_IotMqtt_SerializeUnsubscribe
             */
            IotMqttError_t ( * unsubscribe )( const IotMqttSubscription_t * /* pSubscriptionList */,
                                              size_t /* subscriptionCount */,
                                              uint8_t ** /* pUnsubscribePacket */,
                                              size_t * /* pPacketSize */,
                                              uint16_t * /* pPacketIdentifier */ );

            /**
             * @brief PINGREQ packet serializer function.
             * @param[out] uint8_t** Where the PINGREQ packet is written.
             * @param[out] size_t* Size of the PINGREQ packet.
             *
             * <b>Default implementation:</b> #_IotMqtt_SerializePingreq
             */
            IotMqttError_t ( * pingreq )( uint8_t ** /* pPingreqPacket */,
                                          size_t * /* pPacketSize */ );

            /**
             * @brief DISCONNECT packet serializer function.
             * @param[out] uint8_t** Where the DISCONNECT packet is written.
             * @param[out] size_t* Size of the DISCONNECT packet.
             *
             * <b>Default implementation:</b> #_IotMqtt_SerializeDisconnect
             */
            IotMqttError_t ( * disconnect )( uint8_t ** /* pDisconnectPacket */,
                                             size_t * /* pPacketSize */ );
        } serialize; /**< @brief Overrides the packet serialization functions for a single connection. */

        struct
        {
            /**
             * @brief CONNACK packet deserializer function.
             * @param[in] uint8_t* Pointer to the start of a CONNACK packet.
             * @param[in] size_t Length of the data stream.
             * @param[out] size_t* The number of bytes in the data stream processed
             * by this function.
             *
             * <b>Default implementation:</b> #_IotMqtt_DeserializeConnack
             */
            IotMqttError_t ( * connack )( const uint8_t * /* pConnackStart */,
                                          size_t /* dataLength */,
                                          size_t * /* pBytesProcessed */ );

            /**
             * @brief PUBLISH packet deserializer function.
             * @param[in] uint8_t* Pointer to the start of a PUBLISH packet.
             * @param[in] size_t Length of the data stream.
             * @param[out] IotMqttPublishInfo_t* Where the deserialized PUBLISH will be written.
             * @param[out] uint16_t* The packet identifier in the PUBLISH.
             * @param[out] size_t* The number of bytes in the data stream processed
             * by this function.
             *
             * <b>Default implementation:</b> #_IotMqtt_DeserializePublish
             */
            IotMqttError_t ( * publish )( const uint8_t * /* pPublishStart */,
                                          size_t /* dataLength */,
                                          IotMqttPublishInfo_t * /* pOutput */,
                                          uint16_t * /* pPacketIdentifier */,
                                          size_t * /* pBytesProcessed */ );

            /**
             * @brief PUBACK packet deserializer function.
             * @param[in] uint8_t* Pointer to the start of a PUBACK packet.
             * @param[in] size_t Length of the data stream.
             * @param[out] uint16_t* The packet identifier in the PUBACK.
             * @param[out] size_t* The number of bytes in the data stream processed
             * by this function.
             *
             * <b>Default implementation:</b> #_IotMqtt_DeserializePuback
             */
            IotMqttError_t ( * puback )( const uint8_t * /* pPubackStart */,
                                         size_t /* dataLength */,
                                         uint16_t * /* pPacketIdentifier */,
                                         size_t * /* pBytesProcessed */ );

            /**
             * @brief SUBACK packet deserializer function.
             * @param[in] IotMqttConnection_t The MQTT connection associated with
             * the subscription. Rejected topic filters should be removed from this
             * connection.
             * @param[in] uint8_t* Pointer to the start of a SUBACK packet.
             * @param[in] size_t Length of the data stream.
             * @param[out] uint16_t* The packet identifier in the SUBACK.
             * @param[out] size_t* The number of bytes in the data stream processed
             * by this function.
             *
             * <b>Default implementation:</b> #_IotMqtt_DeserializeSuback
             */
            IotMqttError_t ( * suback )( IotMqttConnection_t /* mqttConnection */,
                                         const uint8_t * /* pSubackStart */,
                                         size_t /* dataLength */,
                                         uint16_t * /* pPacketIdentifier */,
                                         size_t * /* pBytesProcessed */ );

            /**
             * @brief UNSUBACK packet deserializer function.
             * @param[in] uint8_t* Pointer to the start of a UNSUBACK packet.
             * @param[in] size_t Length of the data stream.
             * @param[out] uint16_t* The packet identifier in the UNSUBACK.
             * @param[out] size_t* The number of bytes in the data stream processed
             * by this function.
             *
            * <b>Default implementation:</b> #_IotMqtt_DeserializeUnsuback
             */
            IotMqttError_t ( * unsuback )( const uint8_t * /* pUnsubackStart */,
                                           size_t /* dataLength */,
                                           uint16_t * /* pPacketIdentifier */,
                                           size_t * /* pBytesProcessed */ );

            /**
             * @brief PINGRESP packet deserializer function.
             * @param[in] uint8_t* Pointer to the start of a PINGRESP packet.
             * @param[in] size_t Length of the data stream.
             * @param[out] size_t* The number of bytes in the data stream processed
             * by this function.
             *
             * <b>Default implementation:</b> #_IotMqtt_DeserializePingresp
             */
            IotMqttError_t ( * pingresp )( const uint8_t * /* pPingrespStart */,
                                           size_t /* dataLength */,
                                           size_t * /* pBytesProcessed */ );
        } deserialize; /**< @brief Overrides the packet deserialization functions for a single connection. */

        /**
         * @brief Get the MQTT packet type from a stream of bytes.
         *
         * @param[in] uint8_t* pPacket Pointer to the beginning of the byte stream.
         * @param[in] size_t Size of the byte stream.
         *
         * <b>Default implementation:</b> #_IotMqtt_GetPacketType
         */
        uint8_t ( * getPacketType )( const uint8_t * /* pPacket */,
                                     size_t /* packetSize */ );

        /**
         * @brief Free a packet generated by the serializer.
         *
         * This function pointer must be set if any other serializer override is set.
         * @param[in] uint8_t* The packet to free.
         *
         * <b>Default implementation:</b> #_IotMqtt_FreePacket
         */
        void ( * freePacket )( uint8_t * /* pPacket */ );
    #endif /* if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1 */
} IotMqttNetIf_t;

/*------------------------- MQTT defined constants --------------------------*/

/**
 * @constantspage{mqtt,MQTT library}
 *
 * @section mqtt_constants_initializers MQTT Initializers
 * @brief Provides default values for the data types of the MQTT library.
 *
 * @snippet this define_mqtt_initializers
 *
 * All user-facing data types of the MQTT library should be initialized using
 * one of the following.
 *
 * @warning Failing to initialize an MQTT data type with the appropriate initializer
 * may result in undefined behavior!
 * @note The initializers may change at any time in future versions, but their
 * names will remain the same.
 *
 * <b>Example</b>
 * @code{c}
 * IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
 * IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
 * IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
 * IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
 * IotMqttCallbackInfo_t callbackInfo = IOT_MQTT_CALLBACK_INFO_INITIALIZER;
 * IotMqttConnection_t connection = IOT_MQTT_CONNECTION_INITIALIZER;
 * IotMqttReference_t reference = IOT_MQTT_REFERENCE_INITIALIZER;
 * @endcode
 *
 * @section mqtt_constants_flags MQTT Function Flags
 * @brief Flags that modify the behavior of MQTT library functions.
 * - #IOT_MQTT_FLAG_WAITABLE <br>
 *   @copybrief IOT_MQTT_FLAG_WAITABLE
 *
 * Flags should be bitwise-ORed with each other to change the behavior of
 * @ref mqtt_function_subscribe, @ref mqtt_function_unsubscribe, or
 * @ref mqtt_function_publish.
 *
 * @note The values of the flags may change at any time in future versions, but
 * their names will remain the same. Additionally, flags will be bitwise-exclusive
 * of each other.
 */

/* @[define_mqtt_initializers] */
/** @brief Initializer for #IotMqttNetIf_t. */
#define IOT_MQTT_NETIF_INITIALIZER            { 0 }
/** @brief Initializer for #IotMqttConnectInfo_t. */
#define IOT_MQTT_CONNECT_INFO_INITIALIZER     { .awsIotMqttMode = true, \
                                                .cleanSession = true }
/** @brief Initializer for #IotMqttPublishInfo_t. */
#define IOT_MQTT_PUBLISH_INFO_INITIALIZER     { 0 }
/** @brief Initializer for #IotMqttSubscription_t. */
#define IOT_MQTT_SUBSCRIPTION_INITIALIZER     { 0 }
/** @brief Initializer for #IotMqttCallbackInfo_t. */
#define IOT_MQTT_CALLBACK_INFO_INITIALIZER    { 0 }
/** @brief Initializer for #IotMqttConnection_t. */
#define IOT_MQTT_CONNECTION_INITIALIZER       NULL
/** @brief Initializer for #IotMqttReference_t. */
#define IOT_MQTT_REFERENCE_INITIALIZER        NULL
/* @[define_mqtt_initializers] */

/**
 * @brief Allows the use of @ref mqtt_function_wait for blocking until completion.
 *
 * This flag is always valid for @ref mqtt_function_subscribe and
 * @ref mqtt_function_unsubscribe. If passed to @ref mqtt_function_publish,
 * the parameter [pPublishInfo->qos](@ref IotMqttPublishInfo_t.qos) must not be `0`.
 *
 * An #IotMqttReference_t <b>MUST</b> be provided if this flag is set. Additionally, an
 * #IotMqttCallbackInfo_t <b>MUST NOT</b> be provided.
 *
 * @note If this flag is set, @ref mqtt_function_wait <b>MUST</b> be called to clean up
 * resources.
 */
#define IOT_MQTT_FLAG_WAITABLE    ( 0x00000001 )

/*------------------------- MQTT library functions --------------------------*/

/**
 * @functionspage{mqtt,MQTT library}
 * - @functionname{mqtt_function_init}
 * - @functionname{mqtt_function_cleanup}
 * - @functionname{mqtt_function_receivecallback}
 * - @functionname{mqtt_function_connect}
 * - @functionname{mqtt_function_disconnect}
 * - @functionname{mqtt_function_subscribe}
 * - @functionname{mqtt_function_timedsubscribe}
 * - @functionname{mqtt_function_unsubscribe}
 * - @functionname{mqtt_function_timedunsubscribe}
 * - @functionname{mqtt_function_publish}
 * - @functionname{mqtt_function_timedpublish}
 * - @functionname{mqtt_function_wait}
 * - @functionname{mqtt_function_strerror}
 * - @functionname{mqtt_function_operationtype}
 * - @functionname{mqtt_function_issubscribed}
 */

/**
 * @functionpage{IotMqtt_Init,mqtt,init}
 * @functionpage{IotMqtt_Cleanup,mqtt,cleanup}
 * @functionpage{IotMqtt_ReceiveCallback,mqtt,receivecallback}
 *
 * @anchor mqtt_function_receivecallback_nopartial
 * <b>Sequence Diagram: Processing a network buffer without partial packets</b>
 * @image html mqtt_function_receivecallback_nopartial.png width=80%
 *
 * @anchor mqtt_function_receivecallback_partial
 * <b>Sequence Diagram: Processing a network buffer with partial packets</b>
 * @image html mqtt_function_receivecallback_partial.png width=80%
 *
 * @functionpage{IotMqtt_Connect,mqtt,connect}
 * @functionpage{IotMqtt_Disconnect,mqtt,disconnect}
 * @functionpage{IotMqtt_Subscribe,mqtt,subscribe}
 * @functionpage{IotMqtt_TimedSubscribe,mqtt,timedsubscribe}
 * @functionpage{IotMqtt_Unsubscribe,mqtt,unsubscribe}
 * @functionpage{IotMqtt_TimedUnsubscribe,mqtt,timedunsubscribe}
 * @functionpage{IotMqtt_Publish,mqtt,publish}
 * @functionpage{IotMqtt_TimedPublish,mqtt,timedpublish}
 * @functionpage{IotMqtt_Wait,mqtt,wait}
 * @functionpage{IotMqtt_strerror,mqtt,strerror}
 * @functionpage{IotMqtt_OperationType,mqtt,operationtype}
 * @functionpage{IotMqtt_IsSubscribed,mqtt,issubscribed}
 */

/**
 * @brief One-time initialization function for the MQTT library.
 *
 * This function performs setup of the MQTT library. <b>It must be called
 * once (and only once) before calling any other MQTT function.</b> Calling this
 * function more than once without first calling @ref mqtt_function_cleanup
 * may result in a crash.
 *
 * @return One of the following:
 * - #IOT_MQTT_SUCCESS
 * - #IOT_MQTT_INIT_FAILED
 *
 * @warning No thread-safety guarantees are provided for this function.
 *
 * @see @ref mqtt_function_cleanup
 */
/* @[declare_mqtt_init] */
IotMqttError_t IotMqtt_Init( void );
/* @[declare_mqtt_init] */

/**
 * @brief One-time deinitialization function for the MQTT library.
 *
 * This function frees resources taken in @ref mqtt_function_init. It should be
 * called after [closing all MQTT connections](@ref mqtt_function_disconnect) to
 * clean up the MQTT library. After this function returns, @ref mqtt_function_init
 * must be called again before calling any other MQTT function.
 *
 * @warning No thread-safety guarantees are provided for this function. Do not
 * call this function if any MQTT connections are open!
 *
 * @see @ref mqtt_function_init
 */
/* @[declare_mqtt_cleanup] */
void IotMqtt_Cleanup( void );
/* @[declare_mqtt_cleanup] */

/**
 * @brief Network receive callback for the MQTT library.
 *
 * This function should be called by the system whenever a stream of MQTT data
 * is received from the network. It processes the data stream and decodes any
 * MQTT packets it finds. The MQTT library uses #IotMqttNetIf_t for sending
 * data and closing network connections.
 *
 * @attention Remember that this function's input `pReceivedData` is a data
 * stream! A data stream may contain any number of different MQTT packets,
 * including incomplete MQTT packets.
 *
 * The input parameter `pReceivedData` must be allocated by the system. The system
 * should read data from the network and place the data in `pReceivedData`. Then,
 * the system should call this function.
 *
 * An important concept associated with this function is *buffer ownership*.
 * Normally, once `pReceivedData` is passed to this function, it is considered
 * property of the MQTT library. <b>The system must keep `pReceivedData` valid
 * and in-scope even after this function returns</b>, and must not make any changes
 * to `pReceivedData` until the MQTT library calls `freeReceivedData` (which
 * transfers ownership of `pReceivedData` back to the system).
 * [This sequence diagram](@ref mqtt_function_receivecallback_nopartial) illustrates
 * the flow where `pReceivedData` is completely processed.
 *
 * If `pReceivedData` ends with a partial MQTT packet, the MQTT library will not
 * call `freeReceivedData`. In this case, this function will return the actual
 * number of bytes processed (from this point onwards called `bytesProcessed`).
 * The buffer `pReceivedData` will be returned to the system. The system should
 * wait for the rest of the MQTT packet to be received and place the remainder
 * of the MQTT packet immediately after the last processed byte in `pReceivedData`.
 * The bytes in `pReceivedData` in the range of `[offset, offset+bytesProcessed]`
 * should be considered "already processed" and should be discarded.
 * [This sequence diagram](@ref mqtt_function_receivecallback_partial) illustrates
 * the case where `pReceivedData` contains a partial packet.
 *
 * <b>If `freeReceivedData` is `NULL`, then ownership of `pReceivedData` will always
 * revert to the system as soon as this function returns.</b> In this scenario, the
 * MQTT library will copy any data it requires out of `pReceivedData`. Therefore,
 * passing `NULL` for `freeReceivedData` may increase memory usage.
 *
 * @param[in] pMqttConnection A pointer to the MQTT connection handle for which
 * the packet was received.
 * @param[in] pConnection The network connection associated with the MQTT connection,
 * passed by the network stack.
 * @param[in] pReceivedData Pointer to the beginning of the data stream. This buffer
 * must remain valid and in-scope until the MQTT library calls `freeReceivedData`.
 * @param[in] dataLength The length of `pReceivedData` in bytes.
 * @param[in] offset The offset (in bytes) into `pReceivedData` where the MQTT library
 * begins processing. All bytes from `pReceivedData+offset` to `pReceivedData+dataLength`
 * will be processed. Pass `0` to start from the beginning of `pReceivedData`.
 * @param[in] freeReceivedData The function that the MQTT library calls when it is
 * finished with `pReceivedData`. This function will only be called when all data is
 * successfully processed, i.e. when this function returns a value of `dataLength-offset`.
 * Pass `NULL` to ignore.
 *
 * @return
 * - `-1` if a protocol violation is encountered. If this function returns `-1`, then
 * the network connection is closed (unless a [disconnect function]
 * (@ref IotMqttNetIf_t.disconnect) was not provided to @ref mqtt_function_connect).
 * `freeReceivedData` is not called if this function returns `-1`.
 * - Number of bytes processed otherwise. If the return value is less than `dataLength`
 * (but not `-1`), the data stream probably contained a partial MQTT packet. The function
 * `freeReceivedData` will only be called if all bytes in the range `pReceivedData+offset`
 * to `pReceivedData+dataLength` were successfully processed.
 */
/* @[declare_mqtt_receivecallback] */
int32_t IotMqtt_ReceiveCallback( void * pMqttConnection,
                                 void * pConnection,
                                 const uint8_t * pReceivedData,
                                 size_t dataLength,
                                 size_t offset,
                                 void ( * freeReceivedData )( void * ) );
/* @[declare_mqtt_receivecallback] */

/**
 * @brief Establish a new MQTT connection.
 *
 * This function opens a connection between a new MQTT client and an MQTT server
 * (also called a <i>broker</i>). MQTT connections are established on top of transport
 * layer protocols (such as TCP/IP), and optionally, application layer security
 * protocols (such as TLS). The MQTT packet that establishes a connection is called
 * the MQTT CONNECT packet. After @ref mqtt_function_init, this function must be
 * called before any other MQTT library function.
 *
 * If [pConnectInfo->cleanSession](@ref IotMqttConnectInfo_t.cleanSession) is `true`,
 * this function establishes a clean MQTT session. Subscriptions and unacknowledged
 * PUBLISH messages will be discarded when the connection is closed.
 *
 * If [pConnectInfo->cleanSession](@ref IotMqttConnectInfo_t.cleanSession) is `false`,
 * this function establishes (or re-establishes) a persistent MQTT session. The parameters
 * [pConnectInfo->pPreviousSubscriptions](@ref IotMqttConnectInfo_t.pPreviousSubscriptions)
 * and [pConnectInfo->previousSubscriptionCount](@ref IotMqttConnectInfo_t.previousSubscriptionCount)
 * may be used to restore subscriptions present in a re-established persistent session.
 * Any restored subscriptions <b>MUST</b> have been present in the persistent session;
 * <b>this function does not send an MQTT SUBSCRIBE packet!</b>
 *
 * This MQTT library is network agnostic, meaning it has no knowledge of the
 * underlying network protocol carrying the MQTT packets. It interacts with the
 * network through a network abstraction layer, allowing it to be used with many
 * different network stacks. The network abstraction layer is established
 * per-connection, allowing every #IotMqttConnection_t to use a different network
 * stack. The parameter `pNetworkInterface` sets up the network abstraction layer
 * for an MQTT connection; see the documentation on #IotMqttNetIf_t for details
 * on its members.
 *
 * The `pConnectInfo` parameter provides the contents of the MQTT CONNECT packet.
 * Most members [are defined by the MQTT spec.](@ref IotMqttConnectInfo_t). The
 * [pConnectInfo->pWillInfo](@ref IotMqttConnectInfo_t.pWillInfo) member provides
 * information on a Last Will and Testament (LWT) message to be published if the
 * MQTT connection is closed without [sending a DISCONNECT packet]
 * (@ref mqtt_function_disconnect). Unlike other PUBLISH
 * messages, a LWT message payload is limited to 65535 bytes in length. Additionally,
 * the retry [interval](@ref IotMqttPublishInfo_t.retryMs) and [limit]
 * (@ref IotMqttPublishInfo_t.retryLimit) members of #IotMqttPublishInfo_t
 * are ignored for LWT messages. The LWT message is optional; `pWillInfo` may be NULL.
 *
 * Unlike @ref mqtt_function_publish, @ref mqtt_function_subscribe, and
 * @ref mqtt_function_unsubscribe, this function is always blocking. Additionally,
 * because the MQTT connection acknowledgement packet (CONNACK packet) does not
 * contain any information on <i>which</i> CONNECT packet it acknowledges, only one
 * CONNECT operation may be in progress at any time. This means that parallel
 * threads making calls to @ref mqtt_function_connect will be serialized to send
 * their CONNECT packets one-by-one.
 *
 * @param[out] pMqttConnection Set to a newly-initialized MQTT connection handle
 * if this function succeeds.
 * @param[in] pNetworkInterface The network `send` and `disconnect` functions that
 * this MQTT connection will use.
 * @param[in] pConnectInfo MQTT connection setup parameters.
 * @param[in] timeoutMs If the MQTT server does not accept the connection within
 * this timeout, this function returns #IOT_MQTT_TIMEOUT.
 *
 * @return One of the following:
 * - #IOT_MQTT_SUCCESS
 * - #IOT_MQTT_BAD_PARAMETER
 * - #IOT_MQTT_NO_MEMORY
 * - #IOT_MQTT_NETWORK_ERROR
 * - #IOT_MQTT_SCHEDULING_ERROR
 * - #IOT_MQTT_BAD_RESPONSE
 * - #IOT_MQTT_TIMEOUT
 * - #IOT_MQTT_SERVER_REFUSED
 *
 * <b>Example</b>
 * @code{c}
 * // An initialized and connected network connection.
 * IotNetworkConnection_t pNetworkConnection;
 *
 * // Parameters to MQTT connect.
 * IotMqttConnection_t mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
 * IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
 * IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
 * IotMqttPublishInfo_t willInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
 *
 * // Example using a generic network implementation.
 * networkInterface.pSendContext = pNetworkConnection;
 * networkInterface.pDisconnectContext = pNetworkConnection;
 * networkInterface.send = IotNetwork_Send;
 * networkInterface.disconnect = IotNetwork_CloseConnection;
 *
 * // Set the members of the connection info (password and username not used).
 * connectInfo.cleanSession = true;
 * connectInfo.keepAliveSeconds = 30;
 * connectInfo.pClientIdentifier = "uniqueclientidentifier";
 * connectInfo.clientIdentifierLength = 22;
 *
 * // Set the members of the will info (retain and retry not used).
 * willInfo.qos = IOT_MQTT_QOS_1;
 * willInfo.pTopicName = "will/topic/name";
 * willInfo.topicNameLength = 15;
 * willInfo.pPayload = "MQTT client unexpectedly disconnected.";
 * willInfo.payloadLength = 38;
 *
 * // Set the pointer to the will info.
 * connectInfo.pWillInfo = &willInfo;
 *
 * // Call CONNECT with a 5 second block time. Should return
 * // IOT_MQTT_SUCCESS when successful.
 * IotMqttError_t result = IotMqtt_Connect( &mqttConnection,
 *                                          &networkInterface,
 *                                          &connectInfo,
 *                                          5000 );
 *
 * if( result == IOT_MQTT_SUCCESS )
 * {
 *     // Do something with the MQTT connection...
 *
 *     // Clean up and close the MQTT connection once it's no longer needed.
 *     IotMqtt_Disconnect( mqttConnection, false );
 * }
 * @endcode
 */
/* @[declare_mqtt_connect] */
IotMqttError_t IotMqtt_Connect( IotMqttConnection_t * pMqttConnection,
                                const IotMqttNetIf_t * pNetworkInterface,
                                const IotMqttConnectInfo_t * pConnectInfo,
                                uint64_t timeoutMs );
/* @[declare_mqtt_connect] */

/**
 * @brief Closes an MQTT connection and frees resources.
 *
 * This function closes an MQTT connection and should only be called once
 * the MQTT connection is no longer needed. Its exact behavior depends on the
 * `cleanupOnly` parameter.
 *
 * Normally, `cleanupOnly` should be `false`. This gracefully shuts down an MQTT
 * connection by sending an MQTT DISCONNECT packet. Any [disconnect function]
 * (@ref IotMqttNetIf_t.disconnect) provided [when the connection was established]
 * (@ref mqtt_function_connect) will also be called. Note that because the MQTT server
 * will not acknowledge a DISCONNECT packet, the client has no way of knowing if
 * the server received the DISCONNECT packet. In the case where the DISCONNECT
 * packet is lost in transport, any Last Will and Testament (LWT) message established
 * with the connection may be published. However, if the DISCONNECT reaches the
 * MQTT server, the LWT message will be discarded and not published.
 *
 * Should the underlying network connection become unusable, this function should
 * be called with `cleanupOnly` set to `true`. In this case, no DISCONNECT packet
 * nor [disconnect function](@ref IotMqttNetIf_t.disconnect) will be called.
 * This function will only free the resources used by the MQTT connection; it still
 * must be called even if the network is offline to avoid leaking resources.
 *
 * Once this function is called, its parameter `mqttConnection` should no longer
 * be used.
 *
 * @param[in] mqttConnection The MQTT connection to close and clean up.
 * @param[in] cleanupOnly Passing `true` will cause this function to only perform
 * cleanup of the MQTT connection and not send a DISCONNECT packet. This parameter
 * should be `true` if the network goes offline or is otherwise unusable. Otherwise,
 * it should be `false`.
 */
/* @[declare_mqtt_disconnect] */
void IotMqtt_Disconnect( IotMqttConnection_t mqttConnection,
                         bool cleanupOnly );
/* @[declare_mqtt_disconnect] */

/**
 * @brief Subscribes to the given array of topic filters and receive an asynchronous
 * notification when the subscribe completes.
 *
 * This function transmits an MQTT SUBSCRIBE packet to the server. A SUBSCRIBE
 * packet notifies the server to send any matching PUBLISH messages to this client.
 * A single SUBSCRIBE packet may carry more than one topic filter, hence the
 * parameters to this function include an array of [subscriptions]
 * (@ref IotMqttSubscription_t).
 *
 * An MQTT subscription has two pieces:
 * 1. The subscription topic filter registered with the MQTT server. The MQTT
 * SUBSCRIBE packet sent from this client to server notifies the server to send
 * messages matching the given topic filters to this client.
 * 2. The [callback function](@ref IotMqttCallbackInfo_t.function) that this
 * client will invoke when an incoming message is received. The callback function
 * notifies applications of an incoming PUBLISH message.
 *
 * The helper function @ref mqtt_function_issubscribed can be used to check if a
 * [callback function](@ref IotMqttCallbackInfo_t.function) is registered for
 * a particular topic filter.
 *
 * To modify an already-registered subscription callback, call this function with
 * a new `pSubscriptionList`. Any topic filters in `pSubscriptionList` that already
 * have a registered callback will be replaced with the new values in `pSubscriptionList`.
 *
 * @attention QoS 2 subscriptions are currently unsupported. Only 0 or 1 are valid
 * for subscription QoS.
 *
 * @param[in] mqttConnection The MQTT connection to use for the subscription.
 * @param[in] pSubscriptionList Pointer to the first element in the array of
 * subscriptions.
 * @param[in] subscriptionCount The number of elements in pSubscriptionList.
 * @param[in] flags Flags which modify the behavior of this function. See @ref mqtt_constants_flags.
 * @param[in] pCallbackInfo Asynchronous notification of this function's completion.
 * @param[out] pSubscribeRef Set to a handle by which this operation may be
 * referenced after this function returns. This reference is invalidated once
 * the subscription operation completes.
 *
 * @return This function will return #IOT_MQTT_STATUS_PENDING upon success.
 * @return Upon completion of the subscription (either through an
 * #IotMqttCallbackInfo_t or @ref mqtt_function_wait), the status will be one of:
 * - #IOT_MQTT_SUCCESS
 * - #IOT_MQTT_NETWORK_ERROR
 * - #IOT_MQTT_SCHEDULING_ERROR
 * - #IOT_MQTT_BAD_RESPONSE
 * - #IOT_MQTT_SERVER_REFUSED
 * @return If this function fails before queuing a subscribe operation, it will return
 * one of:
 * - #IOT_MQTT_BAD_PARAMETER
 * - #IOT_MQTT_NO_MEMORY
 *
 * @see @ref mqtt_function_timedsubscribe for a blocking variant of this function.
 * @see @ref mqtt_function_unsubscribe for the function that removes subscriptions.
 *
 * <b>Example</b>
 * @code{c}
 * #define NUMBER_OF_SUBSCRIPTIONS ...
 *
 * // Subscription callback function.
 * void subscriptionCallback( void * pArgument, IotMqttCallbackParam_t * pPublish );
 *
 * // An initialized and connected MQTT connection.
 * IotMqttConnection_t mqttConnection;
 *
 * // Subscription information.
 * pSubscriptions[ NUMBER_OF_SUBSCRIPTIONS ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER };
 * IotMqttReference_t lastOperation = IOT_MQTT_REFERENCE_INITIALIZER;
 *
 * // Set the subscription information.
 * for( int i = 0; i < NUMBER_OF_SUBSCRIPTIONS; i++ )
 * {
 *     pSubscriptions[ i ].qos = IOT_MQTT_QOS_1;
 *     pSubscriptions[ i ].pTopicFilter = "some/topic/filter";
 *     pSubscriptions[ i ].topicLength = ( uint16_t ) strlen( pSubscriptions[ i ].pTopicFilter );
 *     pSubscriptions[ i ].callback.function = subscriptionCallback;
 * }
 *
 * IotMqttError_t result = IotMqtt_Subscribe( mqttConnection,
 *                                            pSubscriptions,
 *                                            NUMBER_OF_SUBSCRIPTIONS,
 *                                            IOT_MQTT_FLAG_WAITABLE,
 *                                            NULL,
 *                                            &lastOperation );
 *
 * // Subscribe returns IOT_MQTT_STATUS_PENDING when successful. Wait up to
 * // 5 seconds for the operation to complete.
 * if( result == IOT_MQTT_STATUS_PENDING )
 * {
 *     result = IotMqtt_Wait( subscriptionRef, 5000 );
 * }
 *
 * // Check that the subscriptions were successful.
 * if( result == IOT_MQTT_SUCCESS )
 * {
 *     // Wait for messages on the subscription topic filters...
 *
 *     // Unsubscribe once the subscriptions are no longer needed.
 *     result = IotMqtt_Unsubscribe( mqttConnection,
 *                                   pSubscriptions,
 *                                   NUMBER_OF_SUBSCRIPTIONS,
 *                                   IOT_MQTT_FLAG_WAITABLE,
 *                                   NULL,
 *                                   &lastOperation );
 *
 *     // UNSUBSCRIBE returns IOT_MQTT_STATUS_PENDING when successful.
 *     // Wait up to 5 seconds for the operation to complete.
 *     if( result == IOT_MQTT_STATUS_PENDING )
 *     {
 *         result = IotMqtt_Wait( lastOperation, 5000 );
 *     }
 * }
 * // Check which subscriptions were rejected by the server.
 * else if( result == IOT_MQTT_SERVER_REFUSED )
 * {
 *     for( int i = 0; i < NUMBER_OF_SUBSCRIPTIONS; i++ )
 *     {
 *         if( IotMqtt_IsSubscribed( mqttConnection,
 *                                   pSubscriptions[ i ].pTopicFilter,
 *                                   pSubscriptions[ i ].topicFilterLength,
 *                                   NULL ) == false )
 *         {
 *             // This subscription was rejected.
 *         }
 *     }
 * }
 * @endcode
 */
/* @[declare_mqtt_subscribe] */
IotMqttError_t IotMqtt_Subscribe( IotMqttConnection_t mqttConnection,
                                  const IotMqttSubscription_t * pSubscriptionList,
                                  size_t subscriptionCount,
                                  uint32_t flags,
                                  const IotMqttCallbackInfo_t * pCallbackInfo,
                                  IotMqttReference_t * pSubscribeRef );
/* @[declare_mqtt_subscribe] */

/**
 * @brief Subscribes to the given array of topic filters with a timeout.
 *
 * This function transmits an MQTT SUBSCRIBE packet to the server, then waits for
 * a server response to the packet. Internally, this function is a call to @ref
 * mqtt_function_subscribe followed by @ref mqtt_function_wait. See @ref
 * mqtt_function_subscribe for more information about the MQTT SUBSCRIBE operation.
 *
 * @attention QoS 2 subscriptions are currently unsupported. Only 0 or 1 are valid
 * for subscription QoS.
 *
 * @param[in] mqttConnection The MQTT connection to use for the subscription.
 * @param[in] pSubscriptionList Pointer to the first element in the array of
 * subscriptions.
 * @param[in] subscriptionCount The number of elements in pSubscriptionList.
 * @param[in] flags Flags which modify the behavior of this function. See @ref mqtt_constants_flags.
 * Currently, flags are ignored by this function; this parameter is for
 * future-compatibility.
 * @param[in] timeoutMs If the MQTT server does not acknowledge the subscriptions within
 * this timeout, this function returns #IOT_MQTT_TIMEOUT.
 *
 * @return One of the following:
 * - #IOT_MQTT_SUCCESS
 * - #IOT_MQTT_BAD_PARAMETER
 * - #IOT_MQTT_NO_MEMORY
 * - #IOT_MQTT_NETWORK_ERROR
 * - #IOT_MQTT_SCHEDULING_ERROR
 * - #IOT_MQTT_BAD_RESPONSE
 * - #IOT_MQTT_TIMEOUT
 * - #IOT_MQTT_SERVER_REFUSED
 */
/* @[declare_mqtt_timedsubscribe] */
IotMqttError_t IotMqtt_TimedSubscribe( IotMqttConnection_t mqttConnection,
                                       const IotMqttSubscription_t * pSubscriptionList,
                                       size_t subscriptionCount,
                                       uint32_t flags,
                                       uint64_t timeoutMs );
/* @[declare_mqtt_timedsubscribe] */

/**
 * @brief Unsubscribes from the given array of topic filters and receive an asynchronous
 * notification when the unsubscribe completes.
 *
 * This function transmits an MQTT UNSUBSCRIBE packet to the server. An UNSUBSCRIBE
 * packet removes registered topic filters from the server. After unsubscribing,
 * the server will no longer send messages on these topic filters to the client.
 *
 * Corresponding [subscription callback functions](@ref IotMqttCallbackInfo_t.function)
 * are also removed from the MQTT connection. These subscription callback functions
 * will be removed even if the MQTT UNSUBSCRIBE packet fails to send.
 *
 * @param[in] mqttConnection The MQTT connection used for the subscription.
 * @param[in] pSubscriptionList Pointer to the first element in the array of
 * subscriptions.
 * @param[in] subscriptionCount The number of elements in pSubscriptionList.
 * @param[in] flags Flags which modify the behavior of this function. See @ref mqtt_constants_flags.
 * @param[in] pCallbackInfo Asynchronous notification of this function's completion.
 * @param[out] pUnsubscribeRef Set to a handle by which this operation may be
 * referenced after this function returns. This reference is invalidated once
 * the unsubscribe operation completes.
 *
 * @return This function will return #IOT_MQTT_STATUS_PENDING upon success.
 * @return Upon completion of the unsubscribe (either through an
 * #IotMqttCallbackInfo_t or @ref mqtt_function_wait), the status will be one of:
 * - #IOT_MQTT_SUCCESS
 * - #IOT_MQTT_NETWORK_ERROR
 * - #IOT_MQTT_SCHEDULING_ERROR
 * - #IOT_MQTT_BAD_RESPONSE
 * @return If this function fails before queuing an unsubscribe operation, it will return
 * one of:
 * - #IOT_MQTT_BAD_PARAMETER
 * - #IOT_MQTT_NO_MEMORY
 *
 * @see @ref mqtt_function_timedsubscribe for a blocking variant of this function.
 * @see @ref mqtt_function_subscribe for the function that adds subscriptions.
 */
/* @[declare_mqtt_unsubscribe] */
IotMqttError_t IotMqtt_Unsubscribe( IotMqttConnection_t mqttConnection,
                                    const IotMqttSubscription_t * pSubscriptionList,
                                    size_t subscriptionCount,
                                    uint32_t flags,
                                    const IotMqttCallbackInfo_t * pCallbackInfo,
                                    IotMqttReference_t * pUnsubscribeRef );
/* @[declare_mqtt_unsubscribe] */

/**
 * @brief Unsubscribes from a given array of topic filters with a timeout.
 *
 * This function transmits an MQTT UNSUBSCRIBE packet to the server, then waits
 * for a server response to the packet. Internally, this function is a call to
 * @ref mqtt_function_unsubscribe followed by @ref mqtt_function_wait. See @ref
 * mqtt_function_unsubscribe for more information about the MQTT UNSUBSCRIBE
 * operation.
 *
 * @param[in] mqttConnection The MQTT connection used for the subscription.
 * @param[in] pSubscriptionList Pointer to the first element in the array of
 * subscriptions.
 * @param[in] subscriptionCount The number of elements in pSubscriptionList.
 * @param[in] flags Flags which modify the behavior of this function. See @ref mqtt_constants_flags.
 * Currently, flags are ignored by this function; this parameter is for
 * future-compatibility.
 * @param[in] timeoutMs If the MQTT server does not acknowledge the UNSUBSCRIBE within
 * this timeout, this function returns #IOT_MQTT_TIMEOUT.
 *
 * @return One of the following:
 * - #IOT_MQTT_SUCCESS
 * - #IOT_MQTT_BAD_PARAMETER
 * - #IOT_MQTT_NO_MEMORY
 * - #IOT_MQTT_NETWORK_ERROR
 * - #IOT_MQTT_SCHEDULING_ERROR
 * - #IOT_MQTT_BAD_RESPONSE
 */
/* @[declare_mqtt_timedunsubscribe] */
IotMqttError_t IotMqtt_TimedUnsubscribe( IotMqttConnection_t mqttConnection,
                                         const IotMqttSubscription_t * pSubscriptionList,
                                         size_t subscriptionCount,
                                         uint32_t flags,
                                         uint64_t timeoutMs );
/* @[declare_mqtt_timedunsubscribe] */

/**
 * @brief Publishes a message to the given topic name and receive an asynchronous
 * notification when the publish completes.
 *
 * This function transmits an MQTT PUBLISH packet to the server. A PUBLISH packet
 * contains a payload and a topic name. Any clients with a subscription on a
 * topic filter matching the PUBLISH topic name will receive a copy of the
 * PUBLISH packet from the server.
 *
 * If a PUBLISH packet fails to reach the server and it is not a QoS 0 message,
 * it will be retransmitted. See #IotMqttPublishInfo_t for a description
 * of the retransmission strategy.
 *
 * @attention QoS 2 messages are currently unsupported. Only 0 or 1 are valid
 * for message QoS.
 *
 * @param[in] mqttConnection The MQTT connection to use for the publish.
 * @param[in] pPublishInfo MQTT publish parameters.
 * @param[in] flags Flags which modify the behavior of this function. See @ref mqtt_constants_flags.
 * @param[in] pCallbackInfo Asynchronous notification of this function's completion.
 * @param[out] pPublishRef Set to a handle by which this operation may be
 * referenced after this function returns. This reference is invalidated once
 * the publish operation completes.
 *
 * @return This function will return #IOT_MQTT_STATUS_PENDING upon success for
 * QoS 1 publishes. For a QoS 0 publish it returns #IOT_MQTT_SUCCESS upon
 * success.
 * @return Upon completion of a QoS 1 publish (either through an
 * #IotMqttCallbackInfo_t or @ref mqtt_function_wait), the status will be one of:
 * - #IOT_MQTT_SUCCESS
 * - #IOT_MQTT_NETWORK_ERROR
 * - #IOT_MQTT_SCHEDULING_ERROR
 * - #IOT_MQTT_BAD_RESPONSE
 * - #IOT_MQTT_RETRY_NO_RESPONSE (if [pPublishInfo->retryMs](@ref IotMqttPublishInfo_t.retryMs)
 * and [pPublishInfo->retryLimit](@ref IotMqttPublishInfo_t.retryLimit) were set).
 * @return If this function fails before queuing an publish operation (regardless
 * of QoS), it will return one of:
 * - #IOT_MQTT_BAD_PARAMETER
 * - #IOT_MQTT_NO_MEMORY
 *
 * @note The parameters `pCallbackInfo` and `pPublishRef` should only be used for QoS
 * 1 publishes. For QoS 0, they should both be `NULL`.
 *
 * @see @ref mqtt_function_timedpublish for a blocking variant of this function.
 *
 * <b>Example</b>
 * @code{c}
 * // An initialized and connected MQTT connection.
 * IotMqttConnection_t mqttConnection;
 *
 * // Publish information.
 * IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
 *
 * // Set the publish information. QoS 0 example (retain not used):
 * publishInfo.qos = IOT_MQTT_QOS_0;
 * publishInfo.pTopicName = "some/topic/name";
 * publishInfo.topicNameLength = 15;
 * publishInfo.pPayload = "payload";
 * publishInfo.payloadLength = 8;
 *
 * // QoS 0 publish should return IOT_MQTT_SUCCESS upon success.
 * IotMqttError_t qos0Result = IotMqtt_Publish( mqttConnection,
 *                                              &publishInfo,
 *                                              0,
 *                                              NULL,
 *                                              NULL );
 *
 * // QoS 1 with retry example (using same topic name and payload as QoS 0 example):
 * IotMqttReference_t qos1Reference = IOT_MQTT_REFERENCE_INITIALIZER;
 * publishInfo.qos = IOT_MQTT_QOS_1;
 * publishInfo.retryMs = 1000; // Retry if no response is received in 1 second.
 * publishInfo.retryLimit = 5; // Retry up to 5 times.
 *
 * // QoS 1 publish should return IOT_MQTT_STATUS_PENDING upon success.
 * IotMqttError_t qos1Result = IotMqtt_Publish( mqttConnection,
 *                                              &publishInfo,
 *                                              IOT_MQTT_FLAG_WAITABLE,
 *                                              NULL,
 *                                              &qos1Reference );
 *
 * // Wait up to 5 seconds for the publish to complete.
 * if( qos1Result == IOT_MQTT_STATUS_PENDING )
 * {
 *     qos1Result = IotMqtt_Wait( qos1Reference, 5000 );
 * }
 * @endcode
 */
/* @[declare_mqtt_publish] */
IotMqttError_t IotMqtt_Publish( IotMqttConnection_t mqttConnection,
                                const IotMqttPublishInfo_t * pPublishInfo,
                                uint32_t flags,
                                const IotMqttCallbackInfo_t * pCallbackInfo,
                                IotMqttReference_t * pPublishRef );
/* @[declare_mqtt_publish] */

/**
 * @brief Publish a message to the given topic name with a timeout.
 *
 * This function transmits an MQTT PUBLISH packet to the server, then waits for
 * a server response to the packet. Internally, this function is a call to @ref
 * mqtt_function_publish followed by @ref mqtt_function_wait. See @ref
 * mqtt_function_publish for more information about the MQTT PUBLISH operation.
 *
 * @attention QoS 2 messages are currently unsupported. Only 0 or 1 are valid
 * for message QoS.
 *
 * @param[in] mqttConnection The MQTT connection to use for the publish.
 * @param[in] pPublishInfo MQTT publish parameters.
 * @param[in] flags Flags which modify the behavior of this function. See @ref mqtt_constants_flags.
 * Currently, flags are ignored by this function; this parameter is for
 * future-compatibility.
 * @param[in] timeoutMs If the MQTT server does not acknowledge a QoS 1 PUBLISH
 * within this timeout, this function returns #IOT_MQTT_TIMEOUT. This parameter
 * is ignored for QoS 0 PUBLISH messages.
 *
 * @return One of the following:
 * - #IOT_MQTT_SUCCESS
 * - #IOT_MQTT_BAD_PARAMETER
 * - #IOT_MQTT_NO_MEMORY
 * - #IOT_MQTT_NETWORK_ERROR
 * - #IOT_MQTT_SCHEDULING_ERROR
 * - #IOT_MQTT_BAD_RESPONSE
 * - #IOT_MQTT_RETRY_NO_RESPONSE (if [pPublishInfo->retryMs](@ref IotMqttPublishInfo_t.retryMs)
 * and [pPublishInfo->retryLimit](@ref IotMqttPublishInfo_t.retryLimit) were set).
 */
/* @[declare_mqtt_timedpublish] */
IotMqttError_t IotMqtt_TimedPublish( IotMqttConnection_t mqttConnection,
                                     const IotMqttPublishInfo_t * pPublishInfo,
                                     uint32_t flags,
                                     uint64_t timeoutMs );
/* @[declare_mqtt_timedpublish] */

/**
 * @brief Waits for an operation to complete.
 *
 * This function blocks to wait for a [subscribe](@ref mqtt_function_subscribe),
 * [unsubscribe](@ref mqtt_function_unsubscribe), or [publish]
 * (@ref mqtt_function_publish) to complete. These operations are by default
 * asynchronous; the function calls queue an operation for processing, and a
 * callback is invoked once the operation is complete.
 *
 * To use this function, the flag #IOT_MQTT_FLAG_WAITABLE must have been
 * set in the operation's function call. Additionally, this function must always
 * be called with any waitable operation to clean up resources.
 *
 * Regardless of its return value, this function always clean up resources used
 * by the waitable operation. This means `reference` is invalidated as soon as
 * this function returns, even if it returns #IOT_MQTT_TIMEOUT or another error.
 *
 * @param[in] reference Reference to the operation to wait for. The flag
 * #IOT_MQTT_FLAG_WAITABLE must have been set for this operation.
 * @param[in] timeoutMs How long to wait before returning #IOT_MQTT_TIMEOUT.
 *
 * @return The return value of this function depends on the MQTT operation associated
 * with `reference`. See #IotMqttError_t for possible return values.
 *
 * <b>Example</b>
 * @code{c}
 * // Reference and timeout.
 * IotMqttReference_t reference = IOT_MQTT_REFERENCE_INITIALIZER;
 * uint64_t timeoutMs = 5000; // 5 seconds
 *
 * // MQTT operation to wait for.
 * IotMqttError_t result = IotMqtt_Publish( mqttConnection,
 *                                          &publishInfo,
 *                                          IOT_MQTT_FLAG_WAITABLE,
 *                                          NULL,
 *                                          &reference );
 *
 * // Publish should have returned IOT_MQTT_STATUS_PENDING. The call to wait
 * // returns once the result of the publish is available or the timeout expires.
 * if( result == IOT_MQTT_STATUS_PENDING )
 * {
 *     result = IotMqtt_Wait( reference, timeoutMs );
 *
 *     // After the call to wait, the result of the publish is known
 *     // (not IOT_MQTT_STATUS_PENDING).
 *     assert( result != IOT_MQTT_STATUS_PENDING );
 * }
 * @endcode
 */
/* @[declare_mqtt_wait] */
IotMqttError_t IotMqtt_Wait( IotMqttReference_t reference,
                             uint64_t timeoutMs );
/* @[declare_mqtt_wait] */

/*-------------------------- MQTT helper functions --------------------------*/

/**
 * @brief Returns a string that describes an #IotMqttError_t.
 *
 * Like the POSIX's `strerror`, this function returns a string describing a
 * return code. In this case, the return code is an MQTT library error code,
 * `status`.
 *
 * The string returned by this function <b>MUST</b> be treated as read-only: any
 * attempt to modify its contents may result in a crash. Therefore, this function
 * is limited to usage in logging.
 *
 * @param[in] status The status to describe.
 *
 * @return A read-only string that describes `status`.
 *
 * @warning The string returned by this function must never be modified.
 */
/* @[declare_mqtt_strerror] */
const char * IotMqtt_strerror( IotMqttError_t status );
/* @[declare_mqtt_strerror] */

/**
 * @brief Returns a string that describes an #IotMqttOperationType_t.
 *
 * This function returns a string describing an MQTT operation type, `operation`.
 *
 * The string returned by this function <b>MUST</b> be treated as read-only: any
 * attempt to modify its contents may result in a crash. Therefore, this function
 * is limited to usage in logging.
 *
 * @param[in] operation The operation to describe.
 *
 * @return A read-only string that describes `operation`.
 *
 * @warning The string returned by this function must never be modified.
 */
/* @[declare_mqtt_operationtype] */
const char * IotMqtt_OperationType( IotMqttOperationType_t operation );
/* @[declare_mqtt_operationtype] */

/**
 * @brief Check if an MQTT connection has a subscription for a topic filter.
 *
 * This function checks whether an MQTT connection `mqttConnection` has a
 * subscription callback registered for a topic filter `pTopicFilter`. If a
 * subscription callback is found, its details are copied into the output parameter
 * `pCurrentSubscription`. This subscription callback will be invoked for incoming
 * PUBLISH messages on `pTopicFilter`.
 *
 * <b>The check for a matching subscription is only performed client-side</b>;
 * therefore, this function should not be relied upon for perfect accuracy. For
 * example, this function may return an incorrect result if the MQTT server
 * crashes and drops subscriptions without informing the client.
 *
 * Note that an MQTT connection's subscriptions might change between the time this
 * function checks the subscription list and its caller tests the return value.
 * This function certainly should not be used concurrently with any pending SUBSCRIBE
 * or UNSUBSCRIBE operations.
 *
 * One suitable use of this function is to check <i>which</i> subscriptions were rejected
 * if @ref mqtt_function_subscribe returns #IOT_MQTT_SERVER_REFUSED; that return
 * code only means that <i>at least one</i> subscription was rejected.
 *
 * @param[in] mqttConnection The MQTT connection to check.
 * @param[in] pTopicFilter The topic filter to check.
 * @param[in] topicFilterLength Length of `pTopicFilter`.
 * @param[out] pCurrentSubscription If a subscription is found, its details are
 * copied here. This output parameter is only valid if this function returns `true`.
 * Pass `NULL` to ignore.
 *
 * @return `true` if a subscription was found; `false` otherwise.
 *
 * @note The subscription QoS is not stored by the MQTT library; therefore,
 * `pCurrentSubscription->qos` will always be set to #IOT_MQTT_QOS_0.
 */
/* @[declare_mqtt_issubscribed] */
bool IotMqtt_IsSubscribed( IotMqttConnection_t mqttConnection,
                           const char * pTopicFilter,
                           uint16_t topicFilterLength,
                           IotMqttSubscription_t * pCurrentSubscription );
/* @[declare_mqtt_issubscribed] */

#endif /* ifndef _IOT_MQTT_H_ */
