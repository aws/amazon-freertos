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
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Shadow include. */
#include "aws_iot_shadow.h"

/* Queue include. */
#include "private/aws_iot_queue.h"

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
    #ifdef AWS_IOT_LOG_LEVEL_GLOBAL
        #define _LIBRARY_LOG_LEVEL    AWS_IOT_LOG_LEVEL_GLOBAL
    #else
        #define _LIBRARY_LOG_LEVEL    AWS_IOT_LOG_NONE
    #endif
#endif

#define _LIBRARY_LOG_NAME    ( "Shadow" )
#include "private/aws_iot_logging_setup.h"

/*
 * Provide default values for undefined memory allocation functions based on
 * the usage of dynamic memory allocation.
 */
#if AWS_IOT_STATIC_MEMORY_ONLY == 1
    #include "platform/aws_iot_static_memory.h"

    #ifndef AwsIotShadow_MallocOperation
        #define AwsIotShadow_MallocOperation    AwsIot_MallocShadowOperation
    #endif

    #ifndef AwsIotShadow_FreeOperation
        #define AwsIotShadow_FreeOperation    AwsIot_FreeShadowOperation
    #endif

    #ifndef AwsIotShadow_MallocString
        #define AwsIotShadow_MallocString    AwsIot_MallocMessageBuffer
    #endif

    #ifndef AwsIotShadow_FreeString
        #define AwsIotShadow_FreeString    AwsIot_FreeMessageBuffer
    #endif

    #ifndef AwsIotShadow_MallocSubscription
        #define AwsIotShadow_MallocSubscription    AwsIot_MallocShadowSubscription
    #endif

    #ifndef AwsIotShadow_FreeSubscription
        #define AwsIotShadow_FreeSubscription    AwsIot_FreeShadowSubscription
    #endif
#else /* if AWS_IOT_STATIC_MEMORY_ONLY == 1 */
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
#endif /* if AWS_IOT_STATIC_MEMORY_ONLY == 1 */

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

#define _MAX_THING_NAME_LENGTH                    ( 128 )
#define _SHADOW_OPERATION_COUNT                   ( 3 )
#define _SHADOW_CALLBACK_COUNT                    ( 2 )

#define _SHADOW_TOPIC_PREFIX                      "$aws/things/"
#define _SHADOW_TOPIC_PREFIX_LENGTH               ( ( uint16_t ) ( sizeof( _SHADOW_TOPIC_PREFIX ) - 1 ) )

#define _SHADOW_DELETE_OPERATION_STRING           "/shadow/delete"
#define _SHADOW_DELETE_OPERATION_STRING_LENGTH    ( ( uint16_t ) ( sizeof( _SHADOW_DELETE_OPERATION_STRING ) - 1 ) )
#define _SHADOW_GET_OPERATION_STRING              "/shadow/get"
#define _SHADOW_GET_OPERATION_STRING_LENGTH       ( ( uint16_t ) ( sizeof( _SHADOW_GET_OPERATION_STRING ) - 1 ) )
#define _SHADOW_UPDATE_OPERATION_STRING           "/shadow/update"
#define _SHADOW_UPDATE_OPERATION_STRING_LENGTH    ( ( uint16_t ) ( sizeof( _SHADOW_UPDATE_OPERATION_STRING ) - 1 ) )

#define _SHADOW_ACCEPTED_SUFFIX                   "/accepted"
#define _SHADOW_ACCEPTED_SUFFIX_LENGTH            ( ( uint16_t ) ( sizeof( _SHADOW_ACCEPTED_SUFFIX ) - 1 ) )
#define _SHADOW_REJECTED_SUFFIX                   "/rejected"
#define _SHADOW_REJECTED_SUFFIX_LENGTH            ( ( uint16_t ) ( sizeof( _SHADOW_REJECTED_SUFFIX ) - 1 ) )

#define _SHADOW_DELTA_SUFFIX                      "/delta"
#define _SHADOW_DELTA_SUFFIX_LENGTH               ( ( uint16_t ) ( sizeof( _SHADOW_DELTA_SUFFIX ) - 1 ) )
#define _SHADOW_UPDATED_SUFFIX                    "/documents"
#define _SHADOW_UPDATED_SUFFIX_LENGTH             ( ( uint16_t ) ( sizeof( _SHADOW_UPDATED_SUFFIX ) - 1 ) )

#define _SHADOW_LONGEST_SUFFIX_LENGTH             _SHADOW_UPDATED_SUFFIX_LENGTH

#define _CLIENT_TOKEN_KEY                         "clientToken"
#define _CLIENT_TOKEN_KEY_LENGTH                  ( sizeof( _CLIENT_TOKEN_KEY ) - 1 )

#define _MAX_CLIENT_TOKEN_LENGTH                  ( 64 )

#define _PERSISTENT_SUBSCRIPTION                  ( -1 )

/*----------------------- Shadow internal data types ------------------------*/

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Forward declarations.
 */
typedef struct _shadowOperation      _shadowOperation_t;
typedef struct _shadowSubscription   _shadowSubscription_t;
/** @endcond */

typedef AwsIotMqttError_t ( * _mqttOperationFunction_t )( AwsIotMqttConnection_t,
                                                          const AwsIotMqttSubscription_t * const,
                                                          size_t,
                                                          uint32_t,
                                                          uint64_t );

typedef void ( * _mqttCallbackFunction_t )( void *,
                                            AwsIotMqttCallbackParam_t * const );

typedef enum _shadowOperationType
{
    _SHADOW_DELETE,
    _SHADOW_GET,
    _SHADOW_UPDATE,
    _SET_DELTA_CALLBACK,
    _SET_UPDATED_CALLBACK
} _shadowOperationType_t;

typedef enum _shadowCallbackType
{
    _DELTA_CALLBACK,
    _UPDATED_CALLBACK
} _shadowCallbackType_t;

typedef enum _shadowOperationStatus
{
    _SHADOW_ACCEPTED,
    _SHADOW_REJECTED,
    _UNKNOWN_STATUS
} _shadowOperationStatus_t;

typedef struct _shadowOperation
{
    _shadowOperation_t * pNext;
    _shadowOperation_t * pPrevious;

    _shadowOperationType_t type;
    uint32_t flags;
    AwsIotShadowError_t status;

    AwsIotMqttConnection_t mqttConnection;
    _shadowSubscription_t * pSubscription;

    union
    {
        struct
        {
            void *( *mallocDocument )( size_t );

            const char * pDocument;
            size_t documentLength;
        } get;

        struct
        {
            const char * pClientToken;
            size_t clientTokenLength;
        } update;
    };

    union
    {
        AwsIotSemaphore_t waitSemaphore;
        AwsIotShadowCallbackInfo_t callback;
    } notify;
} _shadowOperation_t;

typedef struct _shadowSubscription
{
    _shadowSubscription_t * pNext;
    _shadowSubscription_t * pPrevious;

    int references[ _SHADOW_OPERATION_COUNT ];
    AwsIotShadowCallbackInfo_t callbacks[ _SHADOW_CALLBACK_COUNT ];

    char * pTopicBuffer;

    size_t thingNameLength;
    char pThingName[];
} _shadowSubscription_t;

#if _LIBRARY_LOG_LEVEL > AWS_IOT_LOG_NONE
    extern const char * const _pAwsIotShadowOperationNames[];
    extern const char * const _pAwsIotShadowCallbackNames[];
#endif

extern uint64_t _AwsIotShadowMqttTimeout;

extern AwsIotList_t _AwsIotShadowPendingOperations;
extern AwsIotList_t _AwsIotShadowSubscriptions;

/*----------------------- Shadow operation functions ------------------------*/

AwsIotShadowError_t AwsIotShadowInternal_CreatePendingOperationList( void );

AwsIotShadowError_t AwsIotShadowInternal_CreateOperation( _shadowOperation_t ** const pNewOperation,
                                                          _shadowOperationType_t operation,
                                                          uint32_t flags,
                                                          const AwsIotShadowCallbackInfo_t * const pCallbackInfo );

void AwsIotShadowInternal_DestroyOperation( void * pData );

AwsIotShadowError_t AwsIotShadowInternal_GenerateShadowTopic( _shadowOperationType_t type,
                                                              const char * const pThingName,
                                                              size_t thingNameLength,
                                                              char ** const pTopicBuffer,
                                                              uint16_t * const pOperationTopicLength );

AwsIotShadowError_t AwsIotShadowInternal_ProcessOperation( AwsIotMqttConnection_t mqttConnection,
                                                           const char * const pThingName,
                                                           size_t thingNameLength,
                                                           _shadowOperation_t * const pOperation,
                                                           const AwsIotShadowDocumentInfo_t * const pDocumentInfo );

void AwsIotShadowInternal_Notify( _shadowOperation_t * const pOperation );

/*---------------------- Shadow subscription functions ----------------------*/

AwsIotShadowError_t AwsIotShadowInternal_CreateSubscriptionList( void );

_shadowSubscription_t * AwsIotShadowInternal_FindSubscription( const char * const pThingName,
                                                               size_t thingNameLength );

void AwsIotShadowInternal_RemoveSubscription( _shadowSubscription_t * const pSubscription,
                                              _shadowSubscription_t ** const pRemovedSubscription );

void AwsIotShadowInternal_DestroySubscription( void * pData );

AwsIotShadowError_t AwsIotShadowInternal_IncrementReferences( _shadowOperation_t * const pOperation,
                                                              char * const pTopicBuffer,
                                                              uint16_t operationTopicLength,
                                                              _mqttCallbackFunction_t callback );

void AwsIotShadowInternal_DecrementReferences( _shadowOperation_t * const pOperation,
                                               char * const pTopicBuffer,
                                               _shadowSubscription_t ** const pRemovedSubscription );

/*------------------------- Shadow parser functions -------------------------*/

_shadowOperationStatus_t AwsIotShadowInternal_ParseShadowStatus( const char * const pTopicName,
                                                                 size_t topicNameLength );

AwsIotShadowError_t AwsIotShadowInternal_ParseTopicName( const char * const pTopicName,
                                                         uint16_t topicNameLength,
                                                         const char ** const pThingName,
                                                         size_t * const pThingNameLength );

AwsIotShadowError_t AwsIotShadowInternal_ParseErrorDocument( const char * const pErrorDocument,
                                                             size_t errorDocumentLength );

#endif /* ifndef _AWS_IOT_SHADOW_INTERNAL_H_ */
