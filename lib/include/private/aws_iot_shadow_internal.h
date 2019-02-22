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
 * @file aws_iot_shadow_internal.h
 * @brief Internal header of Shadow library. This header should not be included in
 * typical application code.
 */

#ifndef _AWS_IOT_SHADOW_INTERNAL_H_
#define _AWS_IOT_SHADOW_INTERNAL_H_

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Linear containers (lists and queues) include. */
#include "iot_linear_containers.h"

/* Platform layer types include. */
#include "types/iot_platform_types.h"

/* Shadow include. */
#include "aws_iot_shadow.h"

/**
 * @def AwsIotShadow_Assert( expression )
 * @brief Assertion macro for the Shadow library.
 *
 * Set @ref AWS_IOT_SHADOW_ENABLE_ASSERTS to `1` to enable assertions in the Shadow
 * library.
 *
 * @param[in] expression Expression to be evaluated.
 */
#if AWS_IOT_SHADOW_ENABLE_ASSERTS == 1
    #ifndef AwsIotShadow_Assert
        #include <assert.h>
        #define AwsIotShadow_Assert( expression )    assert( expression )
    #endif
#else
    #define AwsIotShadow_Assert( expression )
#endif

/* Configure logs for Shadow functions. */
#ifdef AWS_IOT_LOG_LEVEL_SHADOW
    #define _LIBRARY_LOG_LEVEL        AWS_IOT_LOG_LEVEL_SHADOW
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define _LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define _LIBRARY_LOG_LEVEL    IOT_LOG_NONE
    #endif
#endif

#define _LIBRARY_LOG_NAME    ( "Shadow" )
#include "iot_logging_setup.h"

/*
 * Provide default values for undefined memory allocation functions based on
 * the usage of dynamic memory allocation.
 */
#if IOT_STATIC_MEMORY_ONLY == 1
    #include "private/iot_static_memory.h"

/**
 * @brief Allocate a #_shadowOperation_t. This function should have the same
 * signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    #ifndef AwsIotShadow_MallocOperation
        #define AwsIotShadow_MallocOperation    AwsIot_MallocShadowOperation
    #endif

/**
 * @brief Free a #_shadowOperation_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    #ifndef AwsIotShadow_FreeOperation
        #define AwsIotShadow_FreeOperation    AwsIot_FreeShadowOperation
    #endif

/**
 * @brief Allocate a buffer for a short string, used for topic names or client
 * tokens. This function should have the same signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    #ifndef AwsIotShadow_MallocString
        #define AwsIotShadow_MallocString    Iot_MallocMessageBuffer
    #endif

/**
 * @brief Free a string. This function should have the same signature as
 * [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    #ifndef AwsIotShadow_FreeString
        #define AwsIotShadow_FreeString    Iot_FreeMessageBuffer
    #endif

/**
 * @brief Allocate a #_shadowSubscription_t. This function should have the
 * same signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    #ifndef AwsIotShadow_MallocSubscription
        #define AwsIotShadow_MallocSubscription    AwsIot_MallocShadowSubscription
    #endif

/**
 * @brief Free a #_shadowSubscription_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    #ifndef AwsIotShadow_FreeSubscription
        #define AwsIotShadow_FreeSubscription    AwsIot_FreeShadowSubscription
    #endif
#else /* if IOT_STATIC_MEMORY_ONLY == 1 */
    #include <stdlib.h>

    #ifndef AwsIotShadow_MallocOperation
        #define AwsIotShadow_MallocOperation    malloc
    #endif

    #ifndef AwsIotShadow_FreeOperation
        #define AwsIotShadow_FreeOperation    free
    #endif

    #ifndef AwsIotShadow_MallocString
        #define AwsIotShadow_MallocString    malloc
    #endif

    #ifndef AwsIotShadow_FreeString
        #define AwsIotShadow_FreeString    free
    #endif

    #ifndef AwsIotShadow_MallocSubscription
        #define AwsIotShadow_MallocSubscription    malloc
    #endif

    #ifndef AwsIotShadow_FreeSubscription
        #define AwsIotShadow_FreeSubscription    free
    #endif
#endif /* if IOT_STATIC_MEMORY_ONLY == 1 */

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values for undefined configuration constants.
 */
#ifndef AWS_IOT_SHADOW_DEFAULT_MQTT_TIMEOUT_MS
    #define AWS_IOT_SHADOW_DEFAULT_MQTT_TIMEOUT_MS    ( 5000 )
#endif
/** @endcond */

/**
 * @brief The longest Thing Name accepted by the Shadow service, per the [AWS IoT
 * Service Limits](https://docs.aws.amazon.com/general/latest/gr/aws_service_limits.html#limits_iot).
 */
#define _MAX_THING_NAME_LENGTH                    ( 128 )

/**
 * @brief The number of currently available Shadow operations.
 *
 * The 3 Shadow operations are DELETE, GET, and UPDATE.
 */
#define _SHADOW_OPERATION_COUNT                   ( 3 )

/**
 * @brief The number of currently available Shadow callbacks.
 *
 * The 2 Shadow callbacks are `update/delta` (AKA "Delta") and `/update/documents`
 * (AKA "Updated").
 */
#define _SHADOW_CALLBACK_COUNT                    ( 2 )

/**
 * @brief The common prefix of all Shadow MQTT topics, per the [AWS IoT Shadow
 * spec](https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html).
 */
#define _SHADOW_TOPIC_PREFIX                      "$aws/things/"

/**
 * @brief The length of #_SHADOW_TOPIC_PREFIX.
 */
#define _SHADOW_TOPIC_PREFIX_LENGTH               ( ( uint16_t ) ( sizeof( _SHADOW_TOPIC_PREFIX ) - 1 ) )

/**
 * @brief The string representing a Shadow DELETE operation in a Shadow MQTT topic.
 */
#define _SHADOW_DELETE_OPERATION_STRING           "/shadow/delete"

/**
 * @brief The length of #_SHADOW_DELETE_OPERATION_STRING.
 */
#define _SHADOW_DELETE_OPERATION_STRING_LENGTH    ( ( uint16_t ) ( sizeof( _SHADOW_DELETE_OPERATION_STRING ) - 1 ) )

/**
 * @brief The string representing a Shadow GET operation in a Shadow MQTT topic.
 */
#define _SHADOW_GET_OPERATION_STRING              "/shadow/get"

/**
 * @brief The length of #_SHADOW_GET_OPERATION_STRING.
 */
#define _SHADOW_GET_OPERATION_STRING_LENGTH       ( ( uint16_t ) ( sizeof( _SHADOW_GET_OPERATION_STRING ) - 1 ) )

/**
 * @brief The string representing a Shadow UPDATE operation in a Shadow MQTT topic.
 */
#define _SHADOW_UPDATE_OPERATION_STRING           "/shadow/update"

/**
 * @brief The length of #_SHADOW_UPDATE_OPERATION_STRING.
 */
#define _SHADOW_UPDATE_OPERATION_STRING_LENGTH    ( ( uint16_t ) ( sizeof( _SHADOW_UPDATE_OPERATION_STRING ) - 1 ) )

/**
 * @brief The suffix for a Shadow operation "accepted" topic.
 */
#define _SHADOW_ACCEPTED_SUFFIX                   "/accepted"

/**
 * @brief The length of #_SHADOW_ACCEPTED_SUFFIX.
 */
#define _SHADOW_ACCEPTED_SUFFIX_LENGTH            ( ( uint16_t ) ( sizeof( _SHADOW_ACCEPTED_SUFFIX ) - 1 ) )

/**
 * @brief The suffix for a Shadow operation "rejected" topic.
 */
#define _SHADOW_REJECTED_SUFFIX                   "/rejected"

/**
 * @brief The length of #_SHADOW_REJECTED_SUFFIX.
 */
#define _SHADOW_REJECTED_SUFFIX_LENGTH            ( ( uint16_t ) ( sizeof( _SHADOW_REJECTED_SUFFIX ) - 1 ) )

/**
 * @brief The suffix for a Shadow delta topic.
 */
#define _SHADOW_DELTA_SUFFIX                      "/delta"

/**
 * @brief The length of #_SHADOW_DELTA_SUFFIX.
 */
#define _SHADOW_DELTA_SUFFIX_LENGTH               ( ( uint16_t ) ( sizeof( _SHADOW_DELTA_SUFFIX ) - 1 ) )

/**
 * @brief The suffix for a Shadow updated topic.
 */
#define _SHADOW_UPDATED_SUFFIX                    "/documents"

/**
 * @brief The length of #_SHADOW_UPDATED_SUFFIX.
 */
#define _SHADOW_UPDATED_SUFFIX_LENGTH             ( ( uint16_t ) ( sizeof( _SHADOW_UPDATED_SUFFIX ) - 1 ) )

/**
 * @brief The length of the longest Shadow suffix.
 */
#define _SHADOW_LONGEST_SUFFIX_LENGTH             _SHADOW_UPDATED_SUFFIX_LENGTH

/**
 * @brief The JSON key used to represent client tokens in a Shadow update document.
 */
#define _CLIENT_TOKEN_KEY                         "clientToken"

/**
 * @brief The length of #_CLIENT_TOKEN_KEY.
 */
#define _CLIENT_TOKEN_KEY_LENGTH                  ( sizeof( _CLIENT_TOKEN_KEY ) - 1 )

/**
 * @brief The longest client token accepted by the Shadow service, per AWS IoT
 * service limits.
 */
#define _MAX_CLIENT_TOKEN_LENGTH                  ( 64 )

/**
 * @brief A flag to represent persistent subscriptions in a Shadow subscriptions
 * object.
 *
 * Its value is negative to distinguish it from valid subscription counts, which
 * are 0 or positive.
 */
#define _PERSISTENT_SUBSCRIPTION                  ( -1 )

/*----------------------- Shadow internal data types ------------------------*/

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Forward declarations.
 */
struct _shadowOperation;
struct _shadowSubscription;
/** @endcond */

/**
 * @brief Function pointer representing an MQTT timed operation.
 *
 * Currently, this is used to represent @ref mqtt_function_timedsubscribe or
 * @ref mqtt_function_timedunsubscribe.
 */
typedef IotMqttError_t ( * _mqttOperationFunction_t )( IotMqttConnection_t,
                                                       const IotMqttSubscription_t * const,
                                                       size_t,
                                                       uint32_t,
                                                       uint64_t );

/**
 * @brief Function pointer representing an MQTT library callback function.
 */
typedef void ( * _mqttCallbackFunction_t )( void *,
                                            IotMqttCallbackParam_t * const );

/**
 * @brief Enumerations representing each of the Shadow library's API functions.
 */
typedef enum _shadowOperationType
{
    /* Shadow operations. */
    _SHADOW_DELETE = 0, /**< @ref shadow_function_delete */
    _SHADOW_GET = 1,    /**< @ref shadow_function_get */
    _SHADOW_UPDATE = 2, /**< @ref shadow_function_update */

    /* Shadow callbacks. */
    _SET_DELTA_CALLBACK = 3,  /**< @ref shadow_function_setdeltacallback */
    _SET_UPDATED_CALLBACK = 4 /**< @ref shadow_function_setupdatedcallback */
} _shadowOperationType_t;

/**
 * @brief Enumerations representing each of the Shadow callback functions.
 */
typedef enum _shadowCallbackType
{
    _DELTA_CALLBACK = 0,  /**< Delta callback. */
    _UPDATED_CALLBACK = 1 /**< Updated callback. */
} _shadowCallbackType_t;

/**
 * @brief Enumerations representing each of the statuses that may be parsed
 * from a Shadow status topic.
 */
typedef enum _shadowOperationStatus
{
    _SHADOW_ACCEPTED = 0, /**< Shadow operation accepted. */
    _SHADOW_REJECTED = 1, /**< Shadow operation rejected. */
    _UNKNOWN_STATUS = 2   /**< Parsed value matched neither accepted nor rejected. */
} _shadowOperationStatus_t;

/**
 * @brief Internal structure representing a single Shadow operation (DELETE,
 * GET, or UPDATE).
 *
 * A list of these structures keeps track of all in-progress Shadow operations.
 */
typedef struct _shadowOperation
{
    IotLink_t link; /**< @brief List link member. */

    /* Basic operation information. */
    _shadowOperationType_t type;                /**< @brief Operation type. */
    uint32_t flags;                             /**< @brief Flags passed to operation API function. */
    AwsIotShadowError_t status;                 /**< @brief Status of operation. */

    IotMqttConnection_t mqttConnection;         /**< @brief MQTT connection associated with this operation. */
    struct _shadowSubscription * pSubscription; /**< @brief Shadow subscriptions object associated with this operation. */

    union
    {
        /* Members valid only for a GET operation. */
        struct
        {
            /**
             * @brief Function to allocate memory for an incoming Shadow document.
             *
             * Only used when the flag #AWS_IOT_SHADOW_FLAG_WAITABLE is set.
             */
            void *( *mallocDocument )( size_t );

            const char * pDocument; /**< @brief Retrieved Shadow document. */
            size_t documentLength;  /**< @brief Length of retrieved Shadow document. */
        } get;

        /* Members valid only for an UPDATE operation. */
        struct
        {
            const char * pClientToken; /**< @brief Client token in update document. */
            size_t clientTokenLength;  /**< @brief Length of client token. */
        } update;
    };

    /* How to notify of an operation's completion. */
    union
    {
        IotSemaphore_t waitSemaphore;        /**< @brief Semaphore to be used with @ref shadow_function_wait. */
        AwsIotShadowCallbackInfo_t callback; /**< @brief User-provided callback function and parameter. */
    } notify;                                /**< @brief How to notify of an operation's completion. */
} _shadowOperation_t;

/**
 * @brief Represents a Shadow subscriptions object.
 *
 * These structures are stored in a list.
 */
typedef struct _shadowSubscription
{
    IotLink_t link;                                                 /**< @brief List link member. */

    int32_t references[ _SHADOW_OPERATION_COUNT ];                  /**< @brief Reference counter for Shadow operation topics. */
    AwsIotShadowCallbackInfo_t callbacks[ _SHADOW_CALLBACK_COUNT ]; /**< @brief Shadow callbacks for this Thing. */

    /**
     * @brief Buffer allocated for removing Shadow topics.
     *
     * This buffer is pre-allocated to ensure that memory is available when
     * unsubscribing.
     */
    char * pTopicBuffer;

    size_t thingNameLength; /**< @brief Length of Thing Name. */
    char pThingName[];      /**< @brief Thing Name associated with this subscriptions object. */
} _shadowSubscription_t;

/* Declarations of names printed in logs. */
#if _LIBRARY_LOG_LEVEL > IOT_LOG_NONE
    extern const char * const _pAwsIotShadowOperationNames[];
    extern const char * const _pAwsIotShadowCallbackNames[];
#endif

/* Declarations of variables for internal Shadow files. */
extern uint64_t _AwsIotShadowMqttTimeoutMs;
extern IotListDouble_t _AwsIotShadowPendingOperations;
extern IotListDouble_t _AwsIotShadowSubscriptions;
extern IotMutex_t _AwsIotShadowPendingOperationsMutex;
extern IotMutex_t _AwsIotShadowSubscriptionsMutex;

/*----------------------- Shadow operation functions ------------------------*/

/**
 * @brief Create a record for a new in-progress Shadow operation.
 *
 * @param[out] pNewOperation Set to point to the new operation on success.
 * @param[in] operation The type of Shadow operation.
 * @param[in] flags Flags variables passed to a user-facing Shadow function.
 * @param[in] pCallbackInfo User-provided callback function and parameter.
 *
 * @return #AWS_IOT_SHADOW_SUCCESS or #AWS_IOT_SHADOW_NO_MEMORY
 */
AwsIotShadowError_t _AwsIotShadow_CreateOperation( _shadowOperation_t ** const pNewOperation,
                                                   _shadowOperationType_t operation,
                                                   uint32_t flags,
                                                   const AwsIotShadowCallbackInfo_t * const pCallbackInfo );

/**
 * @brief Free resources used to record a Shadow operation. This is called when
 * the operation completes.
 *
 * @param[in] pData The operation which completed. This parameter is of type
 * `void*` to match the signature of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
void _AwsIotShadow_DestroyOperation( void * pData );

/**
 * @brief Fill a buffer with a Shadow topic.
 *
 * @param[in] type One of: DELETE, GET, UPDATE.
 * @param[in] pThingName Thing Name to place in the topic.
 * @param[in] thingNameLength Length of `pThingName`.
 * @param[out] pTopicBuffer Address of the buffer for the Shadow topic. If the
 * pointer at this address is `NULL`, this function will allocate a new buffer;
 * otherwise, it will use the provided buffer.
 * @param[out] pOperationTopicLength Length of the Shadow operation topic (excluding
 * any suffix) placed in `pTopicBuffer`.
 *
 * @warning This function does not check the length of `pTopicBuffer`! Any provided
 * buffer must be large enough to accommodate the full Shadow topic, plus
 * #_SHADOW_LONGEST_SUFFIX_LENGTH.
 *
 * @return #AWS_IOT_SHADOW_SUCCESS or #AWS_IOT_SHADOW_NO_MEMORY. This function
 * will not return #AWS_IOT_SHADOW_NO_MEMORY when a buffer is provided.
 */
AwsIotShadowError_t _AwsIotShadow_GenerateShadowTopic( _shadowOperationType_t type,
                                                       const char * const pThingName,
                                                       size_t thingNameLength,
                                                       char ** const pTopicBuffer,
                                                       uint16_t * const pOperationTopicLength );

/**
 * @brief Process a Shadow operation by sending the necessary MQTT packets.
 *
 * @param[in] mqttConnection The MQTT connection to use.
 * @param[in] pThingName Thing Name for the Shadow operation.
 * @param[in] thingNameLength Length of `pThingName`.
 * @param[in] pOperation Operation data to process.
 * @param[in] pDocumentInfo Information on the Shadow document for GET or UPDATE
 * operations.
 *
 * @return #AWS_IOT_SHADOW_STATUS_PENDING on success. On error, one of
 * #AWS_IOT_SHADOW_NO_MEMORY or #AWS_IOT_SHADOW_MQTT_ERROR.
 */
AwsIotShadowError_t _AwsIotShadow_ProcessOperation( IotMqttConnection_t mqttConnection,
                                                    const char * const pThingName,
                                                    size_t thingNameLength,
                                                    _shadowOperation_t * const pOperation,
                                                    const AwsIotShadowDocumentInfo_t * const pDocumentInfo );

/**
 * @brief Notify of a completed Shadow operation.
 *
 * @param[in] pOperation The operation which completed.
 *
 * Depending on the parameters passed to a user-facing Shadow function, the
 * notification will cause @ref shadow_function_wait to return or invoke a
 * user-provided callback.
 */
void _AwsIotShadow_Notify( _shadowOperation_t * const pOperation );

/*---------------------- Shadow subscription functions ----------------------*/

/**
 * @brief Find a Shadow subscription object. Creates a new subscription object
 * and adds it to the subscription list if not found.
 *
 * @param[in] pThingName Thing Name in the subscription object.
 * @param[in] thingNameLength Length of `pThingName`.
 *
 * @return Pointer to a Shadow subscription object, either found or newly
 * allocated. Returns `NULL` if no subscription object is found and a new
 * subscription object could not be allocated.
 *
 * @note This function should be called with the subscription list mutex locked.
 */
_shadowSubscription_t * _AwsIotShadow_FindSubscription( const char * const pThingName,
                                                        size_t thingNameLength );

/**
 * @brief Remove a Shadow subscription object from the subscription list if
 * unreferenced.
 *
 * @param[in] pSubscription Subscription object to check. If this object has no
 * active references, it is removed from the subscription list.
 * @param[out] pRemovedSubscription Removed subscription object, if any. Optional;
 * pass `NULL` to ignore. If not `NULL`, this parameter will be set to the removed
 * subscription and that subscription will not be destroyed.
 *
 * @note This function should be called with the subscription list mutex locked.
 */
void _AwsIotShadow_RemoveSubscription( _shadowSubscription_t * const pSubscription,
                                       _shadowSubscription_t ** const pRemovedSubscription );

/**
 * @brief Free resources used for a Shadow subscription object.
 *
 * @param[in] pData The subscription object to destroy. This parameter is of type
 * `void*` to match the signature of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
void _AwsIotShadow_DestroySubscription( void * pData );

/**
 * @brief Increment the reference count of a Shadow subscriptions object.
 *
 * Also adds MQTT subscriptions if necessary.
 *
 * @param[in] pOperation The operation for which the reference count should be
 * incremented.
 * @param[in] pTopicBuffer Topic buffer containing the operation topic, used if
 * subscriptions need to be added.
 * @param[in] operationTopicLength The length of the operation topic in `pTopicBuffer`.
 * @param[in] callback MQTT callback function for when this operation completes.
 *
 * @warning This function does not check the length of `pTopicBuffer`! Any provided
 * buffer must already contain the Shadow operation topic, plus enough space for the
 * status suffix.
 *
 * @return #AWS_IOT_SHADOW_STATUS_PENDING on success. On error, one of
 * #AWS_IOT_SHADOW_NO_MEMORY or #AWS_IOT_SHADOW_MQTT_ERROR.
 *
 * @note This function should be called with the subscription list mutex locked.
 */
AwsIotShadowError_t _AwsIotShadow_IncrementReferences( _shadowOperation_t * const pOperation,
                                                       char * const pTopicBuffer,
                                                       uint16_t operationTopicLength,
                                                       _mqttCallbackFunction_t callback );

/**
 * @brief Decrement the reference count of a Shadow subscriptions object.
 *
 * Also removed MQTT subscriptions and deletes the subscription object if necessary.
 *
 * @param[in] pOperation The operation for which the reference count should be
 * decremented.
 * @param[in] pTopicBuffer Topic buffer containing the operation topic, used if
 * subscriptions need to be removed.
 * @param[out] pRemovedSubscription Set to point to a removed subscription.
 * Optional; pass `NULL` to ignore. If not `NULL`, this function will not destroy
 * a removed subscription.
 *
 * @warning This function does not check the length of `pTopicBuffer`! Any provided
 * buffer must be large enough to accommodate the full Shadow topic, plus
 * #_SHADOW_LONGEST_SUFFIX_LENGTH.
 *
 * @note This function should be called with the subscription list mutex locked.
 */
void _AwsIotShadow_DecrementReferences( _shadowOperation_t * const pOperation,
                                        char * const pTopicBuffer,
                                        _shadowSubscription_t ** const pRemovedSubscription );

/*------------------------- Shadow parser functions -------------------------*/

/**
 * @brief Parse the operation status (accepted or rejected) from a Shadow topic.
 *
 * @param[in] pTopicName The topic to parse.
 * @param[in] topicNameLength The length of `pTopicName`.
 *
 * @return Any #_shadowOperationStatus_t.
 */
_shadowOperationStatus_t _AwsIotShadow_ParseShadowStatus( const char * const pTopicName,
                                                          size_t topicNameLength );

/**
 * @brief Parse the Thing Name from a Shadow topic.
 *
 * @param[in] pTopicName The topic to parse.
 * @param[in] topicNameLength The length of `pTopicName`.
 * @param[out] pThingName Set to point to the Thing Name.
 * @param[out] pThingNameLength Set to the length of the Thing Name.
 *
 * @return #AWS_IOT_SHADOW_SUCCESS or #AWS_IOT_SHADOW_BAD_RESPONSE.
 */
AwsIotShadowError_t _AwsIotShadow_ParseThingName( const char * const pTopicName,
                                                  uint16_t topicNameLength,
                                                  const char ** const pThingName,
                                                  size_t * const pThingNameLength );

/**
 * @brief Parse a Shadow error document.
 *
 * @param[in] pErrorDocument The error document to parse.
 * @param[in] errorDocumentLength The length of `pErrorDocument`.
 *
 * @return One of the #AwsIotShadowError_t ranging from 400 to 500 on success.
 * #AWS_IOT_SHADOW_BAD_RESPONSE on error.
 */
AwsIotShadowError_t _AwsIotShadow_ParseErrorDocument( const char * const pErrorDocument,
                                                      size_t errorDocumentLength );

#endif /* ifndef _AWS_IOT_SHADOW_INTERNAL_H_ */
