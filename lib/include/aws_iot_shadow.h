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
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* MQTT include. */
#include "aws_iot_mqtt.h"

/*--------------------------- Shadow handle types ---------------------------*/

/**
 * @handles{shadow,Shadow library}
 */

/**
 * @ingroup shadow_datatypes_handles
 * @brief Opaque handle that references an in-progress Shadow operation.
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
 * description when [logging is enabled for the Shadow library]
 * (@ref AWS_IOT_LOG_LEVEL_SHADOW).
 */
typedef enum AwsIotShadowError
{
    /**
     * @brief Shadow operation completed successfully.
     */
    AWS_IOT_SHADOW_SUCCESS,

    /**
     * @brief Shadow operation queued, awaiting result.
     */
    AWS_IOT_SHADOW_STATUS_PENDING,

    /**
     * @brief Initialization failed.
     */
    AWS_IOT_SHADOW_INIT_FAILED,

    /**
     * @brief At least one parameter is invalid.
     */
    AWS_IOT_SHADOW_BAD_PARAMETER,

    /**
     * @brief Shadow operation failed because of memory allocation failure.
     */
    AWS_IOT_SHADOW_NO_MEMORY,

    /**
     * @brief Shadow operation failed because of failure in MQTT library.
     */
    AWS_IOT_SHADOW_MQTT_ERROR,

    /**
     * @brief Reponse received from Shadow service not understood.
     */
    AWS_IOT_SHADOW_BAD_RESPONSE,

    /**
     * @brief A blocking Shadow operation timed out.
     */
    AWS_IOT_SHADOW_TIMEOUT,

    AWS_IOT_SHADOW_BAD_REQUEST = 400,
    AWS_IOT_SHADOW_UNAUTHORIZED = 401,
    AWS_IOT_SHADOW_FORBIDDEN = 403,
    AWS_IOT_SHADOW_NOT_FOUND = 404,
    AWS_IOT_SHADOW_CONFLICT = 409,
    AWS_IOT_SHADOW_TOO_LARGE = 413,
    AWS_IOT_SHADOW_UNSUPPORTED = 415,
    AWS_IOT_SHADOW_TOO_MANY_REQUESTS = 429,
    AWS_IOT_SHADOW_SERVER_ERROR = 500,
} AwsIotShadowError_t;

typedef enum AwsIotShadowCallbackType
{
    AWS_IOT_SHADOW_DELETE_COMPLETE,
    AWS_IOT_SHADOW_GET_COMPLETE,
    AWS_IOT_SHADOW_UPDATE_COMPLETE,
    AWS_IOT_SHADOW_DELTA_CALLBACK,
    AWS_IOT_SHADOW_UPDATED_CALLBACK
} AwsIotShadowCallbackType_t;

typedef struct AwsIotShadowCallbackParam
{
    AwsIotShadowCallbackType_t callbackType;

    const char * pThingName;
    size_t thingNameLength;

    union
    {
        struct
        {
            struct
            {
                const char * pDocument;
                size_t documentLength;
            } get;

            AwsIotShadowError_t result;
            AwsIotShadowReference_t reference;
        } operation;
        struct
        {
            const char * pDocument;
            size_t documentLength;
        } callback;
    };
} AwsIotShadowCallbackParam_t;

typedef struct AwsIotShadowCallbackInfo
{
    void * param1;
    void ( * function )( void *,
                         AwsIotShadowCallbackParam_t * const );
} AwsIotShadowCallbackInfo_t;

typedef struct AwsIotShadowDocumentInfo
{
    const char * pThingName;
    size_t thingNameLength;

    int QoS;
    int retryLimit;
    uint64_t retryMs;

    union
    {
        struct
        {
            void *( *mallocDocument )( size_t );
        } get;
        struct
        {
            const char * pUpdateDocument;
            size_t updateDocumentLength;
        } update;
    };
} AwsIotShadowDocumentInfo_t;

#define AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER           { 0 }
#define AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER           { 0 }
#define AWS_IOT_SHADOW_REFERENCE_INITIALIZER               NULL

#define AWS_IOT_SHADOW_FLAG_WAITABLE                       ( 0x00000001 )
#define AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS             ( 0x00000002 )

#define AWS_IOT_SHADOW_FLAG_REMOVE_DELETE_SUBSCRIPTIONS    ( 0x00000001 )
#define AWS_IOT_SHADOW_FLAG_REMOVE_GET_SUBSCRIPTIONS       ( 0x00000002 )
#define AWS_IOT_SHADOW_FLAG_REMOVE_UPDATE_SUBSCRIPTIONS    ( 0x00000004 )

AwsIotShadowError_t AwsIotShadow_Init( uint64_t mqttTimeout );

void AwsIotShadow_Cleanup( void );

AwsIotShadowError_t AwsIotShadow_Delete( AwsIotMqttConnection_t mqttConnection,
                                         const char * const pThingName,
                                         size_t thingNameLength,
                                         uint32_t flags,
                                         const AwsIotShadowCallbackInfo_t * const pCallbackInfo,
                                         AwsIotShadowReference_t * const pDeleteRef );

AwsIotShadowError_t AwsIotShadow_TimedDelete( AwsIotMqttConnection_t mqttConnection,
                                              const char * const pThingName,
                                              size_t thingNameLength,
                                              uint32_t flags,
                                              uint64_t timeoutMs );

AwsIotShadowError_t AwsIotShadow_Get( AwsIotMqttConnection_t mqttConnection,
                                      const AwsIotShadowDocumentInfo_t * const pGetInfo,
                                      uint32_t flags,
                                      const AwsIotShadowCallbackInfo_t * const pCallbackInfo,
                                      AwsIotShadowReference_t * const pGetRef );

AwsIotShadowError_t AwsIotShadow_TimedGet( AwsIotMqttConnection_t mqttConnection,
                                           const AwsIotShadowDocumentInfo_t * const pGetInfo,
                                           uint32_t flags,
                                           uint64_t timeoutMs,
                                           const char ** const pShadowDocument,
                                           size_t * const pShadowDocumentLength );

AwsIotShadowError_t AwsIotShadow_Update( AwsIotMqttConnection_t mqttConnection,
                                         const AwsIotShadowDocumentInfo_t * const pUpdateInfo,
                                         uint32_t flags,
                                         const AwsIotShadowCallbackInfo_t * const pCallbackInfo,
                                         AwsIotShadowReference_t * const pUpdateRef );

AwsIotShadowError_t AwsIotShadow_TimedUpdate( AwsIotMqttConnection_t mqttConnection,
                                              const AwsIotShadowDocumentInfo_t * const pUpdateInfo,
                                              uint32_t flags,
                                              uint64_t timeoutMs );

AwsIotShadowError_t AwsIotShadow_Wait( AwsIotShadowReference_t reference,
                                       uint64_t timeoutMs,
                                       const char ** const pShadowDocument,
                                       size_t * const pShadowDocumentLength );

AwsIotShadowError_t AwsIotShadow_SetDeltaCallback( AwsIotMqttConnection_t mqttConnection,
                                                   const char * const pThingName,
                                                   size_t thingNameLength,
                                                   uint32_t flags,
                                                   const AwsIotShadowCallbackInfo_t * const pDeltaCallback );

AwsIotShadowError_t AwsIotShadow_SetUpdatedCallback( AwsIotMqttConnection_t mqttConnection,
                                                     const char * const pThingName,
                                                     size_t thingNameLength,
                                                     uint32_t flags,
                                                     const AwsIotShadowCallbackInfo_t * const pUpdatedCallback );

/* Not safe to call with any in-progress operation. Does not affect callbacks. */
AwsIotShadowError_t AwsIotShadow_RemovePersistentSubscriptions( AwsIotMqttConnection_t mqttConnection,
                                                                const char * const pThingName,
                                                                size_t thingNameLength,
                                                                uint32_t flags );

#endif /* ifndef _AWS_IOT_SHADOW_H_ */
