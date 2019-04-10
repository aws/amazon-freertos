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
 * @brief User-facing functions of the Shadow library.
 */

#ifndef _AWS_IOT_SHADOW_H_
#define _AWS_IOT_SHADOW_H_

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Shadow types include. */
#include "types/aws_iot_shadow_types.h"

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
AwsIotShadowError_t AwsIotShadow_Init( uint32_t mqttTimeout );
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
                                         const char * pThingName,
                                         size_t thingNameLength,
                                         uint32_t flags,
                                         const AwsIotShadowCallbackInfo_t * pCallbackInfo,
                                         AwsIotShadowOperation_t * pDeleteOperation );
/* @[declare_shadow_delete] */

/**
 * @brief Delete a Thing Shadow with a timeout.
 */
/* @[declare_shadow_timeddelete] */
AwsIotShadowError_t AwsIotShadow_TimedDelete( IotMqttConnection_t mqttConnection,
                                              const char * pThingName,
                                              size_t thingNameLength,
                                              uint32_t flags,
                                              uint32_t timeoutMs );
/* @[declare_shadow_timeddelete] */

/**
 * @brief Retrieve a Thing Shadow and receive an asynchronous notification when
 * the Shadow document is received.
 */
/* @[declare_shadow_get] */
AwsIotShadowError_t AwsIotShadow_Get( IotMqttConnection_t mqttConnection,
                                      const AwsIotShadowDocumentInfo_t * pGetInfo,
                                      uint32_t flags,
                                      const AwsIotShadowCallbackInfo_t * pCallbackInfo,
                                      AwsIotShadowOperation_t * pGetOperation );
/* @[declare_shadow_get] */

/**
 * @brief Retrieve a Thing Shadow with a timeout.
 */
/* @[declare_shadow_timedget] */
AwsIotShadowError_t AwsIotShadow_TimedGet( IotMqttConnection_t mqttConnection,
                                           const AwsIotShadowDocumentInfo_t * pGetInfo,
                                           uint32_t flags,
                                           uint32_t timeoutMs,
                                           const char ** pShadowDocument,
                                           size_t * pShadowDocumentLength );
/* @[declare_shadow_timedget] */

/**
 * @brief Send a Thing Shadow update and receive an asynchronous notification when
 * the Shadow Update completes.
 */
/* @[declare_shadow_update] */
AwsIotShadowError_t AwsIotShadow_Update( IotMqttConnection_t mqttConnection,
                                         const AwsIotShadowDocumentInfo_t * pUpdateInfo,
                                         uint32_t flags,
                                         const AwsIotShadowCallbackInfo_t * pCallbackInfo,
                                         AwsIotShadowOperation_t * pUpdateOperation );
/* @[declare_shadow_update] */

/**
 * @brief Send a Thing Shadow update with a timeout.
 */
/* @[declare_shadow_timedupdate] */
AwsIotShadowError_t AwsIotShadow_TimedUpdate( IotMqttConnection_t mqttConnection,
                                              const AwsIotShadowDocumentInfo_t * pUpdateInfo,
                                              uint32_t flags,
                                              uint32_t timeoutMs );
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
 * by the waitable operation. This means `operation` is invalidated as soon as
 * this function returns, even if it returns #AWS_IOT_SHADOW_TIMEOUT or another
 * error.
 *
 * @param[in] operation Reference to the Shadow operation to wait for. The flag
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
 * AwsIotShadowOperation_t updateOperation = AWS_IOT_SHADOW_OPERATION_INITIALIZER;
 * uint32_t timeout = 5000; // 5 seconds
 *
 * // Shadow update operation.
 * result = AwsIotShadow_Update( mqttConnection,
 *                               &updateInfo,
 *                               AWS_IOT_SHADOW_FLAG_WAITABLE,
 *                               NULL,
 *                               &updateOperation );
 *
 * // Update should have returned AWS_IOT_SHADOW_STATUS_PENDING. The call to wait
 * // returns once the result of the update is available or the timeout expires.
 * if( result == AWS_IOT_SHADOW_STATUS_PENDING )
 * {
 *     // The last two parameters are ignored for a Shadow update.
 *     result = AwsIotShadow_Wait( updateOperation, timeout, NULL, NULL );
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
 * AwsIotShadowOperation_t getOperation = AWS_IOT_SHADOW_OPERATION_INITIALIZER;
 * uint32_t timeout = 5000; // 5 seconds
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
 *                            &getOperation );
 *
 * // Get should have returned AWS_IOT_SHADOW_STATUS_PENDING. The call to wait
 * // returns once the result of the get is available or the timeout expires.
 * if( result == AWS_IOT_SHADOW_STATUS_PENDING )
 * {
 *     // The last two parameters must be set for a Shadow get.
 *     result = AwsIotShadow_Wait( getOperation, timeout, &pShadowDocument, &documentLength );
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
AwsIotShadowError_t AwsIotShadow_Wait( AwsIotShadowOperation_t operation,
                                       uint32_t timeoutMs,
                                       const char ** pShadowDocument,
                                       size_t * pShadowDocumentLength );
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
                                                   const char * pThingName,
                                                   size_t thingNameLength,
                                                   uint32_t flags,
                                                   const AwsIotShadowCallbackInfo_t * pDeltaCallback );
/* @[declare_shadow_setdeltacallback] */

/**
 * @brief Set a callback to be invoked when a Thing Shadow changes.
 */
/* @[declare_shadow_setupdatedcallback] */
AwsIotShadowError_t AwsIotShadow_SetUpdatedCallback( IotMqttConnection_t mqttConnection,
                                                     const char * pThingName,
                                                     size_t thingNameLength,
                                                     uint32_t flags,
                                                     const AwsIotShadowCallbackInfo_t * pUpdatedCallback );
/* @[declare_shadow_setupdatedcallback] */

/**
 * @brief Remove persistent Thing Shadow operation topic subscriptions.
 *
 * Not safe to call with any in-progress operation. Does not affect callbacks.
 */
/* @[declare_shadow_removepersistentsubscriptions] */
AwsIotShadowError_t AwsIotShadow_RemovePersistentSubscriptions( IotMqttConnection_t mqttConnection,
                                                                const char * pThingName,
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
