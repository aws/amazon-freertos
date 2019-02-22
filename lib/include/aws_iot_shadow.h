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
 * @file aws_iot_shadow.h
 * @brief User-facing functions and structs of the Shadow library.
 */

#ifndef _AWS_IOT_SHADOW_H_
#define _AWS_IOT_SHADOW_H_

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* MQTT include. */
#include "iot_mqtt.h"

/*--------------------------- Shadow handle types ---------------------------*/

/**
 * @handles{shadow,Shadow library}
 */

/**
 * @ingroup shadow_datatypes_handles
 * @brief Opaque handle that references an in-progress Shadow operation.
 *
 * Set as an output parameter of @ref shadow_function_delete, @ref shadow_function_get,
 * and @ref shadow_function_update. These functions send a message to the Shadow
 * service requesting a Shadow operation; the result of this operation is unknown
 * until the Shadow service sends a response. Therefore, this handle serves as a
 * reference to Shadow operations awaiting a response from the Shadow service.
 *
 * This reference will be valid from the successful return of @ref shadow_function_delete,
 * @ref shadow_function_get, or @ref shadow_function_update. The reference becomes
 * invalid once the [completion callback](@ref AwsIotShadowCallbackInfo_t) is invoked,
 * or @ref shadow_function_wait returns.
 *
 * @initializer{AwsIotShadowReference_t,AWS_IOT_SHADOW_REFERENCE_INITIALIZER}
 *
 * @see @ref shadow_function_wait and #AWS_IOT_SHADOW_FLAG_WAITABLE for waiting on
 * a reference. #AwsIotShadowCallbackInfo_t and #AwsIotShadowCallbackParam_t for an
 * asynchronous notification of completion.
 */
typedef void * AwsIotShadowReference_t;

/*------------------------- Shadow enumerated types -------------------------*/

/**
 * @enums{shadow,Shadow library}
 */

/**
 * @ingroup shadow_datatypes_enums
 * @brief Return codes of [Shadow functions](@ref shadow_functions).
 *
 * The function @ref shadow_function_strerror can be used to get a return code's
 * description.
 *
 * The values between 400 (#AWS_IOT_SHADOW_BAD_REQUEST) and 500
 * (#AWS_IOT_SHADOW_SERVER_ERROR) may be returned by the Shadow service when it
 * rejects a Shadow operation. See [this page]
 * (https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-error-messages.html)
 * for more information.
 */
typedef enum AwsIotShadowError
{
    /**
     * @brief Shadow operation completed successfully.
     *
     * Functions that may return this value:
     * - @ref shadow_function_init
     * - @ref shadow_function_wait
     * - @ref shadow_function_timeddelete
     * - @ref shadow_function_timedget
     * - @ref shadow_function_timedupdate
     * - @ref shadow_function_setdeltacallback
     * - @ref shadow_function_setupdatedcallback
     * - @ref shadow_function_removepersistentsubscriptions
     *
     * Will also be the value of a Shadow operation completion callback's<br>
     * [AwsIotShadowCallbackParam_t.operation.result](@ref AwsIotShadowCallbackParam_t.result)
     * when successful.
     */
    AWS_IOT_SHADOW_SUCCESS = 0,

    /**
     * @brief Shadow operation queued, awaiting result.
     *
     * Functions that may return this value:
     * - @ref shadow_function_delete
     * - @ref shadow_function_get
     * - @ref shadow_function_update
     */
    AWS_IOT_SHADOW_STATUS_PENDING,

    /**
     * @brief Initialization failed.
     *
     * Functions that may return this value:
     * - @ref shadow_function_init
     */
    AWS_IOT_SHADOW_INIT_FAILED,

    /**
     * @brief At least one parameter is invalid.
     *
     * Functions that may return this value:
     * - @ref shadow_function_delete and @ref shadow_function_timeddelete
     * - @ref shadow_function_get and @ref shadow_function_timedget
     * - @ref shadow_function_update and @ref shadow_function_timedupdate
     * - @ref shadow_function_wait
     * - @ref shadow_function_setdeltacallback
     * - @ref shadow_function_setupdatedcallback
     */
    AWS_IOT_SHADOW_BAD_PARAMETER,

    /**
     * @brief Shadow operation failed because of memory allocation failure.
     *
     * Functions that may return this value:
     * - @ref shadow_function_delete and @ref shadow_function_timeddelete
     * - @ref shadow_function_get and @ref shadow_function_timedget
     * - @ref shadow_function_update and @ref shadow_function_timedupdate
     * - @ref shadow_function_setdeltacallback
     * - @ref shadow_function_setupdatedcallback
     */
    AWS_IOT_SHADOW_NO_MEMORY,

    /**
     * @brief Shadow operation failed because of failure in MQTT library.
     *
     * Check the Shadow library logs for the error code returned by the MQTT
     * library.
     *
     * Functions that may return this value:
     * - @ref shadow_function_delete and @ref shadow_function_timeddelete
     * - @ref shadow_function_get and @ref shadow_function_timedget
     * - @ref shadow_function_update and @ref shadow_function_timedupdate
     * - @ref shadow_function_setdeltacallback
     * - @ref shadow_function_setupdatedcallback
     * - @ref shadow_function_removepersistentsubscriptions
     */
    AWS_IOT_SHADOW_MQTT_ERROR,

    /**
     * @brief Reponse received from Shadow service not understood.
     *
     * Functions that may return this value:
     * - @ref shadow_function_timeddelete
     * - @ref shadow_function_timedget
     * - @ref shadow_function_timedupdate
     * - @ref shadow_function_wait
     *
     * May also be the value of a Shadow operation completion callback's<br>
     * [AwsIotShadowCallbackParam_t.operation.result](@ref AwsIotShadowCallbackParam_t.result)
     */
    AWS_IOT_SHADOW_BAD_RESPONSE,

    /**
     * @brief A blocking Shadow operation timed out.
     *
     * Functions that may return this value:
     * - @ref shadow_function_timeddelete
     * - @ref shadow_function_timedget
     * - @ref shadow_function_timedupdate
     * - @ref shadow_function_wait
     * - @ref shadow_function_setdeltacallback
     * - @ref shadow_function_setupdatedcallback
     */
    AWS_IOT_SHADOW_TIMEOUT,

    /**
     * @brief Shadow operation rejected: Bad request.
     *
     * Functions that may return this value:
     * - @ref shadow_function_timeddelete
     * - @ref shadow_function_timedget
     * - @ref shadow_function_timedupdate
     * - @ref shadow_function_wait
     *
     * May also be the value of a Shadow operation completion callback's<br>
     * [AwsIotShadowCallbackParam_t.operation.result](@ref AwsIotShadowCallbackParam_t.result)
     */
    AWS_IOT_SHADOW_BAD_REQUEST = 400,

    /**
     * @brief Shadow operation rejected: Unauthorized.
     *
     * Functions that may return this value:
     * - @ref shadow_function_timeddelete
     * - @ref shadow_function_timedget
     * - @ref shadow_function_timedupdate
     * - @ref shadow_function_wait
     *
     * May also be the value of a Shadow operation completion callback's<br>
     * [AwsIotShadowCallbackParam_t.operation.result](@ref AwsIotShadowCallbackParam_t.result)
     */
    AWS_IOT_SHADOW_UNAUTHORIZED = 401,

    /**
     * @brief Shadow operation rejected: Forbidden.
     *
     * Functions that may return this value:
     * - @ref shadow_function_timeddelete
     * - @ref shadow_function_timedget
     * - @ref shadow_function_timedupdate
     * - @ref shadow_function_wait
     *
     * May also be the value of a Shadow operation completion callback's<br>
     * [AwsIotShadowCallbackParam_t.operation.result](@ref AwsIotShadowCallbackParam_t.result)
     */
    AWS_IOT_SHADOW_FORBIDDEN = 403,

    /**
     * @brief Shadow operation rejected: Thing not found.
     *
     * Functions that may return this value:
     * - @ref shadow_function_timeddelete
     * - @ref shadow_function_timedget
     * - @ref shadow_function_timedupdate
     * - @ref shadow_function_wait
     *
     * May also be the value of a Shadow operation completion callback's<br>
     * [AwsIotShadowCallbackParam_t.operation.result](@ref AwsIotShadowCallbackParam_t.result)
     */
    AWS_IOT_SHADOW_NOT_FOUND = 404,

    /**
     * @brief Shadow operation rejected: Version conflict.
     *
     * Functions that may return this value:
     * - @ref shadow_function_timeddelete
     * - @ref shadow_function_timedget
     * - @ref shadow_function_timedupdate
     * - @ref shadow_function_wait
     *
     * May also be the value of a Shadow operation completion callback's<br>
     * [AwsIotShadowCallbackParam_t.operation.result](@ref AwsIotShadowCallbackParam_t.result)
     */
    AWS_IOT_SHADOW_CONFLICT = 409,

    /**
     * @brief Shadow operation rejected: The payload exceeds the maximum size
     * allowed.
     *
     * Functions that may return this value:
     * - @ref shadow_function_timeddelete
     * - @ref shadow_function_timedget
     * - @ref shadow_function_timedupdate
     * - @ref shadow_function_wait
     *
     * May also be the value of a Shadow operation completion callback's<br>
     * [AwsIotShadowCallbackParam_t.operation.result](@ref AwsIotShadowCallbackParam_t.result)
     */
    AWS_IOT_SHADOW_TOO_LARGE = 413,

    /**
     * @brief Shadow operation rejected: Unsupported document encoding; supported
     * encoding is UTF-8.
     *
     * Functions that may return this value:
     * - @ref shadow_function_timeddelete
     * - @ref shadow_function_timedget
     * - @ref shadow_function_timedupdate
     * - @ref shadow_function_wait
     *
     * May also be the value of a Shadow operation completion callback's<br>
     * [AwsIotShadowCallbackParam_t.operation.result](@ref AwsIotShadowCallbackParam_t.result)
     */
    AWS_IOT_SHADOW_UNSUPPORTED = 415,

    /**
     * @brief Shadow operation rejected: The Device Shadow service will generate
     * this error message when there are more than 10 in-flight requests.
     *
     * Functions that may return this value:
     * - @ref shadow_function_timeddelete
     * - @ref shadow_function_timedget
     * - @ref shadow_function_timedupdate
     * - @ref shadow_function_wait
     *
     * May also be the value of a Shadow operation completion callback's<br>
     * [AwsIotShadowCallbackParam_t.operation.result](@ref AwsIotShadowCallbackParam_t.result)
     */
    AWS_IOT_SHADOW_TOO_MANY_REQUESTS = 429,

    /**
     * @brief Shadow operation rejected: Internal service failure.
     *
     * Functions that may return this value:
     * - @ref shadow_function_timeddelete
     * - @ref shadow_function_timedget
     * - @ref shadow_function_timedupdate
     * - @ref shadow_function_wait
     *
     * May also be the value of a Shadow operation completion callback's<br>
     * [AwsIotShadowCallbackParam_t.operation.result](@ref AwsIotShadowCallbackParam_t.result)
     */
    AWS_IOT_SHADOW_SERVER_ERROR = 500,
} AwsIotShadowError_t;

/**
 * @ingroup shadow_datatypes_enums
 * @brief Types of Shadow library callbacks.
 *
 * One of these values will be placed in #AwsIotShadowCallbackParam_t.callbackType
 * to identify the reason for invoking a callback function.
 */
typedef enum AwsIotShadowCallbackType
{
    AWS_IOT_SHADOW_DELETE_COMPLETE, /**< Callback invoked because a [Shadow delete](@ref shadow_function_delete) completed. */
    AWS_IOT_SHADOW_GET_COMPLETE,    /**< Callback invoked because a [Shadow get](@ref shadow_function_get) completed. */
    AWS_IOT_SHADOW_UPDATE_COMPLETE, /**< Callback invoked because a [Shadow update](@ref shadow_function_update) completed. */
    AWS_IOT_SHADOW_DELTA_CALLBACK,  /**< Callback invoked for an incoming message on a [Shadow delta](@ref shadow_function_setdeltacallback) topic. */
    AWS_IOT_SHADOW_UPDATED_CALLBACK /**< Callback invoked for an incoming message on a [Shadow updated](@ref shadow_function_setupdatedcallback) topic. */
} AwsIotShadowCallbackType_t;

/*------------------------- Shadow parameter structs ------------------------*/

/**
 * @paramstructs{shadow,Shadow}
 */

/**
 * @ingroup shadow_datatypes_paramstructs
 * @brief Parameter to a Shadow callback function.
 *
 * @paramfor Shadow callback functions
 *
 * The Shadow library passes this struct to a callback function whenever a
 * Shadow operation completes or a message is received on a Shadow delta or
 * updated topic.
 *
 * The valid members of this struct are different based on
 * #AwsIotShadowCallbackParam_t.callbackType. If the callback type is
 * #AWS_IOT_SHADOW_DELETE_COMPLETE, #AWS_IOT_SHADOW_GET_COMPLETE, or
 * #AWS_IOT_SHADOW_UPDATE_COMPLETE, then #AwsIotShadowCallbackParam_t.operation
 * is valid. Otherwise, if the callback type is #AWS_IOT_SHADOW_DELTA_CALLBACK
 * or #AWS_IOT_SHADOW_UPDATED_CALLBACK, then #AwsIotShadowCallbackParam_t.callback
 * is valid.
 *
 * @attention Any pointers in this callback parameter may be freed as soon as the
 * [callback function](@ref AwsIotShadowCallbackInfo_t.function) returns. Therefore,
 * data must be copied if it is needed after the callback function returns.
 * @attention The Shadow library may set strings that are not NULL-terminated.
 *
 * @see #AwsIotShadowCallbackInfo_t for the signature of a callback function.
 */
typedef struct AwsIotShadowCallbackParam
{
    AwsIotShadowCallbackType_t callbackType; /**< @brief Reason for invoking the Shadow callback function. */

    const char * pThingName;                 /**< @brief The Thing Name associated with this Shadow callback. */
    size_t thingNameLength;                  /**< @brief Length of #AwsIotShadowCallbackParam_t.pThingName. */

    union
    {
        /* Valid for completed Shadow operations. */
        struct
        {
            /* Valid for a completed Shadow GET operation. */
            struct
            {
                const char * pDocument;        /**< @brief Retrieved Shadow document. */
                size_t documentLength;         /**< @brief Length of retrieved Shadow document. */
            } get;                             /**< @brief Retrieved Shadow document, valid only for a completed [Shadow Get](@ref shadow_function_get). */

            AwsIotShadowError_t result;        /**< @brief Result of Shadow operation, e.g. succeeded or failed. */
            AwsIotShadowReference_t reference; /**< @brief Reference to the Shadow operation that completed. */
        } operation;                           /**< @brief Information on a completed Shadow operation. */

        /* Valid for a message on a Shadow delta or updated topic. */
        struct
        {
            const char * pDocument; /**< @brief Shadow delta or updated document. */
            size_t documentLength;  /**< @brief Length of Shadow delta or updated document. */
        } callback;                 /**< @brief Shadow document from an incoming delta or updated topic. */
    };
} AwsIotShadowCallbackParam_t;

/**
 * @ingroup shadow_datatypes_paramstructs
 * @brief Information on a user-provided Shadow callback function.
 *
 * @paramfor @ref shadow_function_delete, @ref shadow_function_get, @ref
 * shadow_function_update, @ref shadow_function_setdeltacallback, @ref
 * shadow_function_setupdatedcallback
 *
 * Provides a function to be invoked when a Shadow operation completes or when a
 * Shadow document is received on a callback topic (delta or updated).
 *
 * @initializer{AwsIotShadowCallbackInfo_t,AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER}
 */
typedef struct AwsIotShadowCallbackInfo
{
    void * param1; /**< @brief The first parameter to pass to the callback function. */

    /**
     * @brief User-provided callback function signature.
     *
     * @param[in] void* #AwsIotShadowCallbackInfo_t.param1
     * @param[in] AwsIotShadowCallbackParam_t* Details on the outcome of the Shadow
     * operation or an incoming Shadow document.
     *
     * @see #AwsIotShadowCallbackParam_t for more information on the second parameter.
     */
    void ( * function )( void *,
                         AwsIotShadowCallbackParam_t * const );
} AwsIotShadowCallbackInfo_t;

/**
 * @ingroup shadow_datatypes_paramstructs
 * @brief Information on a Shadow document for @ref shadow_function_get or @ref
 * shadow_function_update.
 *
 * @paramfor @ref shadow_function_get, @ref shadow_function_update
 *
 * The valid members of this struct are different based on whether this struct
 * is passed to @ref shadow_function_get or @ref shadow_function_update. When
 * passed to @ref shadow_function_get, the `get` member is valid. When passed to
 * @ref shadow_function_update, the `update` member is valid. All other members
 * must always be set.
 *
 * @initializer{AwsIotShadowDocumentInfo_t,AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER}
 */
typedef struct AwsIotShadowDocumentInfo
{
    const char * pThingName; /**< @brief The Thing Name associated with this Shadow document. */
    size_t thingNameLength;  /**< @brief Length of #AwsIotShadowDocumentInfo_t.pThingName. */

    IotMqttQos_t qos;        /**< @brief QoS when sending a Shadow get or update message. See #IotMqttPublishInfo_t.qos. */
    uint32_t retryLimit;     /**< @brief Maximum number of retries for a Shadow get or update message. See #IotMqttPublishInfo_t.retryLimit. */
    uint64_t retryMs;        /**< @brief First retry time for a Shadow get or update message. See IotMqttPublishInfo_t.retryMs. */

    union
    {
        /* Valid for Shadow get. */
        struct
        {
            /**
             * @brief Function to allocate memory for an incoming Shadow document.
             *
             * This only needs to be set if #AWS_IOT_SHADOW_FLAG_WAITABLE is passed to
             * @ref shadow_function_get.
             */
            void *( *mallocDocument )( size_t );
        } get; /**< @brief Valid members for @ref shadow_function_get. */

        /* Valid for Shadow update. */
        struct
        {
            const char * pUpdateDocument; /**< @brief The Shadow document to send in the update. */
            size_t updateDocumentLength;  /**< @brief Length of Shadow update document. */
        } update;                         /**< @brief Valid members for @ref shadow_function_update. */
    };
} AwsIotShadowDocumentInfo_t;

/*------------------------ Shadow defined constants -------------------------*/

/**
 * @constantspage{shadow,Shadow library}
 *
 * @section shadow_constants_initializers Shadow Initializers
 * @brief Provides default values for the data types of the Shadow library.
 *
 * @snippet this define_shadow_initializers
 *
 * All user-facing data types of the Shadow library should be initialized
 * using one of the following.
 *
 * @warning Failing to initialize a Shadow data type with the appropriate
 * initializer may result in undefined behavior!
 * @note The initializers may change at any time in future versions, but their
 * names will remain the same.
 *
 * <b>Example</b>
 * @code{c}
 * AwsIotShadowCallbackInfo_t callbackInfo = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER;
 * AwsIotShadowDocumentInfo_t documentInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
 * AwsIotShadowReference_t reference = AWS_IOT_SHADOW_REFERENCE_INITIALIZER;
 * @endcode
 *
 * @section shadow_constants_flags Shadow Function Flags
 * @brief Flags that modify the behavior of Shadow library functions.
 *
 * Flags should be bitwise-ORed with each other to change the behavior of
 * Shadow library functions.
 *
 * The following flags are valid for the Shadow operation functions:
 * @ref shadow_function_delete, @ref shadow_function_get, @ref shadow_function_update,
 * and their <i>Timed</i> variants.
 * - #AWS_IOT_SHADOW_FLAG_WAITABLE <br>
 *   @copybrief AWS_IOT_SHADOW_FLAG_WAITABLE
 * - #AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS <br>
 *   @copybrief AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS
 *
 * The following flags are valid for @ref shadow_function_removepersistentsubscriptions.
 * These flags are not valid for the Shadow operation functions.
 * - #AWS_IOT_SHADOW_FLAG_REMOVE_DELETE_SUBSCRIPTIONS <br>
 *   @copybrief AWS_IOT_SHADOW_FLAG_REMOVE_DELETE_SUBSCRIPTIONS
 * - #AWS_IOT_SHADOW_FLAG_REMOVE_GET_SUBSCRIPTIONS <br>
 *   @copybrief AWS_IOT_SHADOW_FLAG_REMOVE_GET_SUBSCRIPTIONS
 * - #AWS_IOT_SHADOW_FLAG_REMOVE_UPDATE_SUBSCRIPTIONS <br>
 *   @copybrief AWS_IOT_SHADOW_FLAG_REMOVE_UPDATE_SUBSCRIPTIONS
 *
 * @note The values of the flags may change at any time in future versions, but
 * their names will remain the same. Additionally, flags which may be used at
 * the same time will be bitwise-exclusive of each other.
 */

/* @[define_shadow_initializers] */
#define AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER           { 0 } /**< @brief Initializer for #AwsIotShadowCallbackInfo_t. */
#define AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER           { 0 } /**< @brief Initializer for #AwsIotShadowDocumentInfo_t. */
#define AWS_IOT_SHADOW_REFERENCE_INITIALIZER               NULL  /**< @brief Initializer for #AwsIotShadowReference_t. */
/* @[define_shadow_initializers] */

/**
 * @brief Allows the use of @ref shadow_function_wait for blocking until completion.
 *
 * This flag is only valid if passed to the functions @ref shadow_function_delete,
 * @ref shadow_function_get, or @ref shadow_function_update.
 *
 * An #AwsIotShadowReference_t <b>MUST</b> be provided if this flag is set.
 * Additionally, an #AwsIotShadowCallbackInfo_t <b>MUST NOT</b> be provided.
 *
 * @note If this flag is set, @ref shadow_function_wait <b>MUST</b> be called to
 * clean up resources.
 */
#define AWS_IOT_SHADOW_FLAG_WAITABLE                       ( 0x00000001 )

/**
 * @brief Maintain the subscriptions for the Shadow operation topics, even after
 * this function returns.
 *
 * This flag is only valid if passed to the functions @ref shadow_function_delete,
 * @ref shadow_function_get, or @ref shadow_function_update.
 *
 * The Shadow service reports results of Shadow operations by publishing
 * messages to MQTT topics. By default, the functions @ref shadow_function_delete,
 * @ref shadow_function_get, and @ref shadow_function_update subscribe to the
 * necessary topics, wait for the Shadow service to publish the result of the
 * Shadow operation, then unsubscribe from those topics. This workflow is suitable
 * for infrequent Shadow operations, but is inefficient for frequent, periodic
 * Shadow operations (where subscriptions for the Shadow operation topics would be
 * constantly added and removed).
 *
 * This flag causes @ref shadow_function_delete, @ref shadow_function_get, or
 * @ref shadow_function_update to maintain Shadow operation topic subscriptions,
 * even after the function returns. These subscriptions may then be used by a
 * future call to the same function.
 *
 * This flags only needs to be set once, after which subscriptions are maintained
 * and reused for a specific Thing Name and Shadow function. The function @ref
 * shadow_function_removepersistentsubscriptions may be used to remove
 * subscriptions maintained by this flag.
 */
#define AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS             ( 0x00000002 )

/**
 * @brief Remove the persistent subscriptions from a Shadow delete operation.
 *
 * This flag is only valid if passed to the function @ref
 * shadow_function_removepersistentsubscriptions.
 *
 * This flag may be passed to @ref shadow_function_removepersistentsubscriptions
 * to remove any subscriptions for a specific Thing Name maintained by a previous
 * call to @ref shadow_function_delete.
 *
 * @warning Do not call @ref shadow_function_removepersistentsubscriptions with
 * this flag for Thing Names with any in-progress Shadow delete operations.
 */
#define AWS_IOT_SHADOW_FLAG_REMOVE_DELETE_SUBSCRIPTIONS    ( 0x00000001 )

/**
 * @brief Remove the persistent subscriptions from a Shadow get operation.
 *
 * This flag is only valid if passed to the function @ref
 * shadow_function_removepersistentsubscriptions.
 *
 * This flag may be passed to @ref shadow_function_removepersistentsubscriptions
 * to remove any subscriptions for a specific Thing Name maintained by a previous
 * call to @ref shadow_function_get.
 *
 * @warning Do not call @ref shadow_function_removepersistentsubscriptions with
 * this flag for Thing Names with any in-progress Shadow get operations.
 */
#define AWS_IOT_SHADOW_FLAG_REMOVE_GET_SUBSCRIPTIONS       ( 0x00000002 )

/**
 * @brief Remove the persistent subscriptions from a Shadow update operation.
 *
 * This flag is only valid if passed to the function @ref
 * shadow_function_removepersistentsubscriptions.
 *
 * This flag may be passed to @ref shadow_function_removepersistentsubscriptions
 * to remove any subscriptions for a specific Thing Name maintained by a previous
 * call to @ref shadow_function_update.
 *
 * @warning Do not call @ref shadow_function_removepersistentsubscriptions with
 * this flag for Thing Names with any in-progress Shadow update operations.
 */
#define AWS_IOT_SHADOW_FLAG_REMOVE_UPDATE_SUBSCRIPTIONS    ( 0x00000004 )

/*------------------------ Shadow library functions -------------------------*/

/**
 * @functionspage{shadow,Shadow library}
 * - @functionname{shadow_function_init}
 * - @functionname{shadow_function_cleanup}
 * - @functionname{shadow_function_delete}
 * - @functionname{shadow_function_timeddelete}
 * - @functionname{shadow_function_get}
 * - @functionname{shadow_function_timedget}
 * - @functionname{shadow_function_update}
 * - @functionname{shadow_function_timedupdate}
 * - @functionname{shadow_function_wait}
 * - @functionname{shadow_function_setdeltacallback}
 * - @functionname{shadow_function_setupdatedcallback}
 * - @functionname{shadow_function_removepersistentsubscriptions}
 * - @functionname{shadow_function_strerror}
 */

/**
 * @functionpage{AwsIotShadow_Init,shadow,init}
 * @functionpage{AwsIotShadow_Cleanup,shadow,cleanup}
 * @functionpage{AwsIotShadow_Delete,shadow,delete}
 * @functionpage{AwsIotShadow_TimedDelete,shadow,timeddelete}
 * @functionpage{AwsIotShadow_Get,shadow,get}
 * @functionpage{AwsIotShadow_TimedGet,shadow,timedget}
 * @functionpage{AwsIotShadow_Update,shadow,update}
 * @functionpage{AwsIotShadow_TimedUpdate,shadow,timedupdate}
 * @functionpage{AwsIotShadow_Wait,shadow,wait}
 * @functionpage{AwsIotShadow_SetDeltaCallback,shadow,setdeltacallback}
 * @functionpage{AwsIotShadow_SetUpdatedCallback,shadow,setupdatedcallback}
 * @functionpage{AwsIotShadow_RemovePersistentSubscriptions,shadow,removepersistentsubscriptions}
 * @functionpage{AwsIotShadow_strerror,shadow,strerror}
 */

/**
 * @brief One-time initialization function for the Shadow library.
 *
 * This function performs internal setup of the Shadow library. <b>It must be
 * called once (and only once) before calling any other Shadow function.</b>
 * Calling this function more than once without first calling @ref
 * shadow_function_cleanup may result in a crash.
 *
 * @return One of the following:
 * - #AWS_IOT_SHADOW_SUCCESS
 * - #AWS_IOT_SHADOW_INIT_FAILED
 *
 * @warning No thread-safety guarantees are provided for this function.
 *
 * @see @ref shadow_function_cleanup
 */
/* @[declare_shadow_init] */
AwsIotShadowError_t AwsIotShadow_Init( uint64_t mqttTimeout );
/* @[declare_shadow_init] */

/**
 * @brief One-time deinitialization function for the Shadow library.
 *
 * This function frees resources taken in @ref shadow_function_init. It should
 * be called to clean up the Shadow library. After this function returns, @ref
 * shadow_function_init must be called again before calling any other Shadow
 * function.
 *
 * @warning No thread-safety guarantees are provided for this function.
 *
 * @see @ref shadow_function_init
 */
/* @[declare_shadow_cleanup] */
void AwsIotShadow_Cleanup( void );
/* @[declare_shadow_cleanup] */

/**
 * @brief Delete a Thing Shadow and receive an asynchronous notification when
 * the Delete completes.
 */
/* @[declare_shadow_delete] */
AwsIotShadowError_t AwsIotShadow_Delete( IotMqttConnection_t mqttConnection,
                                         const char * const pThingName,
                                         size_t thingNameLength,
                                         uint32_t flags,
                                         const AwsIotShadowCallbackInfo_t * const pCallbackInfo,
                                         AwsIotShadowReference_t * const pDeleteRef );
/* @[declare_shadow_delete] */

/**
 * @brief Delete a Thing Shadow with a timeout.
 */
/* @[declare_shadow_timeddelete] */
AwsIotShadowError_t AwsIotShadow_TimedDelete( IotMqttConnection_t mqttConnection,
                                              const char * const pThingName,
                                              size_t thingNameLength,
                                              uint32_t flags,
                                              uint64_t timeoutMs );
/* @[declare_shadow_timeddelete] */

/**
 * @brief Retrieve a Thing Shadow and receive an asynchronous notification when
 * the Shadow document is received.
 */
/* @[declare_shadow_get] */
AwsIotShadowError_t AwsIotShadow_Get( IotMqttConnection_t mqttConnection,
                                      const AwsIotShadowDocumentInfo_t * const pGetInfo,
                                      uint32_t flags,
                                      const AwsIotShadowCallbackInfo_t * const pCallbackInfo,
                                      AwsIotShadowReference_t * const pGetRef );
/* @[declare_shadow_get] */

/**
 * @brief Retrieve a Thing Shadow with a timeout.
 */
/* @[declare_shadow_timedget] */
AwsIotShadowError_t AwsIotShadow_TimedGet( IotMqttConnection_t mqttConnection,
                                           const AwsIotShadowDocumentInfo_t * const pGetInfo,
                                           uint32_t flags,
                                           uint64_t timeoutMs,
                                           const char ** const pShadowDocument,
                                           size_t * const pShadowDocumentLength );
/* @[declare_shadow_timedget] */

/**
 * @brief Send a Thing Shadow update and receive an asynchronous notification when
 * the Shadow Update completes.
 */
/* @[declare_shadow_update] */
AwsIotShadowError_t AwsIotShadow_Update( IotMqttConnection_t mqttConnection,
                                         const AwsIotShadowDocumentInfo_t * const pUpdateInfo,
                                         uint32_t flags,
                                         const AwsIotShadowCallbackInfo_t * const pCallbackInfo,
                                         AwsIotShadowReference_t * const pUpdateRef );
/* @[declare_shadow_update] */

/**
 * @brief Send a Thing Shadow update with a timeout.
 */
/* @[declare_shadow_timedupdate] */
AwsIotShadowError_t AwsIotShadow_TimedUpdate( IotMqttConnection_t mqttConnection,
                                              const AwsIotShadowDocumentInfo_t * const pUpdateInfo,
                                              uint32_t flags,
                                              uint64_t timeoutMs );
/* @[declare_shadow_timedupdate] */

/**
 * @brief Wait for a Shadow operation to complete.
 *
 * This function blocks to wait for a [delete](@ref shadow_function_delete),
 * [get](@ref shadow_function_get), or [update](@ref shadow_function_update) to
 * complete. These operations are by default asynchronous; the function calls
 * queue an operation for processing, and a callback is invoked once the operation
 * is complete.
 *
 * To use this function, the flag #AWS_IOT_SHADOW_FLAG_WAITABLE must have been
 * set in the operation's function call. Additionally, this function must always
 * be called with any waitable operation to clean up resources.
 *
 * Regardless of its return value, this function always clean up resources used
 * by the waitable operation. This means `reference` is invalidated as soon as
 * this function returns, even if it returns #AWS_IOT_SHADOW_TIMEOUT or another
 * error.
 *
 * @param[in] reference Reference to the Shadow operation to wait for. The flag
 * #AWS_IOT_SHADOW_FLAG_WAITABLE must have been set for this operation.
 * @param[in] timeoutMs How long to wait before returning #AWS_IOT_SHADOW_TIMEOUT.
 * @param[out] pShadowDocument A pointer to a buffer containing the Shadow document
 * retrieved by a [Shadow get](@ref shadow_function_get) is placed here. The buffer
 * was allocated with the function #AwsIotShadowDocumentInfo_t.mallocDocument passed
 * to @ref shadow_function_get. This parameter is only valid for a [Shadow get]
 * (@ref shadow_function_get) and ignored for other Shadow operations. This output
 * parameter is only valid if this function returns #AWS_IOT_SHADOW_SUCCESS.
 * @param[out] pShadowDocumentLength The length of the Shadow document in
 * `pShadowDocument` is placed here. This parameter is only valid for a [Shadow get]
 * (@ref shadow_function_get) and ignored for other Shadow operations. This output
 * parameter is only valid if this function returns #AWS_IOT_SHADOW_SUCCESS.
 *
 * @return One of the following:
 * - #AWS_IOT_SHADOW_SUCCESS
 * - #AWS_IOT_SHADOW_BAD_PARAMETER
 * - #AWS_IOT_SHADOW_BAD_RESPONSE
 * - #AWS_IOT_SHADOW_TIMEOUT
 * - A Shadow service rejected reason between 400 (#AWS_IOT_SHADOW_BAD_REQUEST)
 * and 500 (#AWS_IOT_SHADOW_SERVER_ERROR)
 *
 * <b>Example 1 (Shadow Update)</b>
 * @code{c}
 * AwsIotShadowError_t result = AWS_IOT_SHADOW_STATUS_PENDING;
 * AwsIotShadowDocumentInfo_t updateInfo = { ... };
 *
 * // Reference and timeout.
 * AwsIotShadowReference_t reference = AWS_IOT_SHADOW_REFERENCE_INITIALIZER;
 * uint64_t timeout = 5000; // 5 seconds
 *
 * // Shadow update operation.
 * result = AwsIotShadow_Update( mqttConnection,
 *                               &updateInfo,
 *                               AWS_IOT_SHADOW_FLAG_WAITABLE,
 *                               NULL,
 *                               &reference );
 *
 * // Update should have returned AWS_IOT_SHADOW_STATUS_PENDING. The call to wait
 * // returns once the result of the update is available or the timeout expires.
 * if( result == AWS_IOT_SHADOW_STATUS_PENDING )
 * {
 *     // The last two parameters are ignored for a Shadow update.
 *     result = AwsIotShadow_Wait( reference, timeout, NULL, NULL );
 *
 *     // After the call to wait, the result of the update is known
 *     // (not AWS_IOT_SHADOW_STATUS_PENDING).
 *     assert( result != AWS_IOT_SHADOW_STATUS_PENDING );
 * }
 * @endcode
 *
 * <b>Example 2 (Shadow Get)</b>
 * @code{c}
 * AwsIotShadowError_t result = AWS_IOT_SHADOW_STATUS_PENDING;
 * AwsIotShadowDocumentInfo_t getInfo = { ... };
 *
 * // Reference and timeout.
 * AwsIotShadowReference_t reference = AWS_IOT_SHADOW_REFERENCE_INITIALIZER;
 * uint64_t timeout = 5000; // 5 seconds
 *
 * // Buffer pointer and size for retrieved Shadow document.
 * const char * pShadowDocument = NULL;
 * size_t documentLength = 0;
 *
 * // Buffer allocation function must be set for a waitable Shadow get.
 * getInfo.get.mallocDocument = malloc;
 *
 * // Shadow get operation.
 * result = AwsIotShadow_Get( mqttConnection,
 *                            &getInfo,
 *                            AWS_IOT_SHADOW_FLAG_WAITABLE,
 *                            NULL,
 *                            &reference );
 *
 * // Get should have returned AWS_IOT_SHADOW_STATUS_PENDING. The call to wait
 * // returns once the result of the get is available or the timeout expires.
 * if( result == AWS_IOT_SHADOW_STATUS_PENDING )
 * {
 *     // The last two parameters must be set for a Shadow get.
 *     result = AwsIotShadow_Wait( reference, timeout, &pShadowDocument, &documentLength );
 *
 *     // After the call to wait, the result of the get is known
 *     // (not AWS_IOT_SHADOW_STATUS_PENDING).
 *     assert( result != AWS_IOT_SHADOW_STATUS_PENDING );
 *
 *     // The retrieved Shadow document is only valid for a successful Shadow get.
 *     if( result == AWS_IOT_SHADOW_SUCCESS )
 *     {
 *         // Do something with the Shadow document...
 *
 *         // Free the Shadow document when finished.
 *         free( pShadowDocument );
 *     }
 * }
 * @endcode
 */
/* @[declare_shadow_wait] */
AwsIotShadowError_t AwsIotShadow_Wait( AwsIotShadowReference_t reference,
                                       uint64_t timeoutMs,
                                       const char ** const pShadowDocument,
                                       size_t * const pShadowDocumentLength );
/* @[declare_shadow_wait] */

/**
 * @brief Set a callback to be invoked when the Thing Shadow `desired` and `reported`
 * states differ.
 *
 * A Thing Shadow contains `reported` and `desired` states, meant to represent
 * the current device status and some desired status, respectively. When the
 * `reported` and `desired` states differ, the Thing Shadow service generates a
 * <i>delta document</i> and publishes it to the topic `update/delta`. Devices
 * with a subscription for this topic will receive the delta document and may act
 * based on the different `reported` and `desired` states. See [this page]
 * (https://docs.aws.amazon.com/iot/latest/developerguide/using-device-shadows.html#delta-state)
 * for more information about using delta documents.
 *
 * A <i>delta callback</i> may be invoked whenever a delta document is generated.
 * Each Thing may have a single delta callback set. This function modifies the delta
 * callback for a specific Thing depending on the `pDeltaCallback` parameter and
 * the presence of any existing delta callback:
 * - When no existing delta callback exists for a specific Thing, a new delta
 * callback is added.
 * - If there is an existing delta callback and `pDeltaCallback` is not `NULL`, then
 * the existing callback function and parameter are replaced with `pDeltaCallback`.
 * - If there is an existing subscription and `pDeltaCallback` is `NULL`, then the
 * delta callback is removed.
 *
 * This function is always blocking; it may block for up to the default MQTT
 * timeout. This timeout is set as a parameter to @ref shadow_function_init,
 * and defaults to @ref AWS_IOT_SHADOW_DEFAULT_MQTT_TIMEOUT_MS if not set. If
 * this function's underlying MQTT operations fail to complete within this
 * timeout, then this function returns #AWS_IOT_SHADOW_TIMEOUT.
 *
 * @param[in] mqttConnection The MQTT connection to use for the subscription to
 * `update/delta`.
 * @param[in] pThingName The subscription to `update/delta` will be added for
 * this Thing Name.
 * @param[in] thingNameLength The length of `pThingName`.
 * @param[in] flags This parameter is for future-compatibility. Currently, flags
 * are not supported for this function and this parameter is ignored.
 * @param[in] pDeltaCallback Callback function and to invoke for incoming delta
 * documents.
 *
 * @return One of the following:
 * - #AWS_IOT_SHADOW_SUCCESS
 * - #AWS_IOT_SHADOW_BAD_PARAMETER
 * - #AWS_IOT_SHADOW_NO_MEMORY
 * - #AWS_IOT_SHADOW_MQTT_ERROR
 * - #AWS_IOT_SHADOW_TIMEOUT
 *
 * @return This function always returns #AWS_IOT_SHADOW_SUCCESS when replacing or
 * removing existing delta callbacks.
 *
 * @see @ref shadow_function_setupdatedcallback for the function to register
 * callbacks for all Shadow updates.
 *
 * <b>Example</b>
 * @code{c}
 * #define _THING_NAME "Test_device"
 * #define _THING_NAME_LENGTH ( sizeof( _THING_NAME ) - 1 )
 *
 * AwsIotShadowError_t result = AWS_IOT_SHADOW_STATUS_PENDING;
 * AwsIotShadowCallbackInfo_t deltaCallback = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER;
 *
 * // _deltaCallbackFunction will be invoked when a delta document is received.
 * deltaCallback.function = _deltaCallbackFunction;
 *
 * // Set the delta callback for the Thing "Test_device".
 * result = AwsIotShadow_SetDeltaCallback( mqttConnection,
 *                                         _THING_NAME,
 *                                         _THING_NAME_LENGTH,
 *                                         0,
 *                                         &deltaCallback );
 *
 * // Check if callback was successfully set.
 * if( result == AWS_IOT_SHADOW_SUCCESS )
 * {
 *     AwsIotShadowDocumentInfo_t updateInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
 *
 *     // Set the Thing Name for Shadow update.
 *     updateInfo.pThingName = _THING_NAME;
 *     updateInfo.thingNameLength = _THING_NAME_LENGTH;
 *
 *     // Set the Shadow document to send. This document has different "reported"
 *     // and "desired" states. It represents a scenario where a device is currently
 *     // off, but is being ordered to turn on.
 *     updateInfo.update.pUpdateDocument =
 *     "{"
 *          "\"state\": {"
 *              "\"reported\": { \"deviceOn\": false },"
 *              "\"desired\": { \"deviceOn\": true }"
 *          "}"
 *     "}";
 *     updateInfo.update.updateDocumentLength = strlen( updateInfo.update.pUpdateDocument );
 *
 *     // Send the Shadow document with different "reported" and desired states.
 *     result = AwsIotShadow_TimedUpdate( mqttConnection,
 *                                        &updateInfo,
 *                                        0,
 *                                        0,
 *                                        5000 );
 *
 *     // After the update is successfully sent, the function _deltaCallbackFunction
 *     // will be invoked once the Shadow service generates and sends a delta document.
 *     // The delta document will contain the different "deviceOn" states, as well as
 *     // metadata.
 *
 *     // Once the delta callback is no longer needed, it may be removed by passing
 *     // NULL as pDeltaCallback.
 *     result = AwsIotShadow_SetDeltaCallback( mqttConnection,
 *                                             _THING_NAME,
 *                                             _THING_NAME_LENGTH,
 *                                             0,
 *                                             NULL );
 *
 *     // The return value from removing a delta callback should always be success.
 *     assert( result == AWS_IOT_SHADOW_SUCCESS );
 * }
 * @endcode
 */
/* @[declare_shadow_setdeltacallback] */
AwsIotShadowError_t AwsIotShadow_SetDeltaCallback( IotMqttConnection_t mqttConnection,
                                                   const char * const pThingName,
                                                   size_t thingNameLength,
                                                   uint32_t flags,
                                                   const AwsIotShadowCallbackInfo_t * const pDeltaCallback );
/* @[declare_shadow_setdeltacallback] */

/**
 * @brief Set a callback to be invoked when a Thing Shadow changes.
 */
/* @[declare_shadow_setupdatedcallback] */
AwsIotShadowError_t AwsIotShadow_SetUpdatedCallback( IotMqttConnection_t mqttConnection,
                                                     const char * const pThingName,
                                                     size_t thingNameLength,
                                                     uint32_t flags,
                                                     const AwsIotShadowCallbackInfo_t * const pUpdatedCallback );
/* @[declare_shadow_setupdatedcallback] */

/**
 * @brief Remove persistent Thing Shadow operation topic subscriptions.
 *
 * Not safe to call with any in-progress operation. Does not affect callbacks.
 */
/* @[declare_shadow_removepersistentsubscriptions] */
AwsIotShadowError_t AwsIotShadow_RemovePersistentSubscriptions( IotMqttConnection_t mqttConnection,
                                                                const char * const pThingName,
                                                                size_t thingNameLength,
                                                                uint32_t flags );
/* @[declare_shadow_removepersistentsubscriptions] */

/*------------------------- Shadow helper functions -------------------------*/

/**
 * @brief Returns a string that describes an #AwsIotShadowError_t.
 *
 * Like POSIX's `strerror`, this function returns a string describing a return
 * code. In this case, the return code is a Shadow library error code, `status`.
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
/* @[declare_shadow_strerror] */
const char * AwsIotShadow_strerror( AwsIotShadowError_t status );
/* @[declare_shadow_strerror] */

#endif /* ifndef _AWS_IOT_SHADOW_H_ */
