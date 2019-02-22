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
 * @file iot_static_memory.h
 * @brief Functions for managing static buffers. Only used when
 * @ref IOT_STATIC_MEMORY_ONLY is `1`.
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* The functions in this file should only exist in static memory only mode, hence
 * the check for IOT_STATIC_MEMORY_ONLY in the double inclusion guard. */
#if !defined( _IOT_STATIC_MEMORY_H_ ) && ( IOT_STATIC_MEMORY_ONLY == 1 )
#define _IOT_STATIC_MEMORY_H_

/* Standard includes. */
#include <stdbool.h>
#include <stddef.h>

/**
 * @functionspage{static_memory,static memory component}
 * - @functionname{static_memory_function_init}
 * - @functionname{static_memory_function_cleanup}
 * - @functionname{static_memory_function_messagebuffersize}
 * - @functionname{static_memory_function_mallocmessagebuffer}
 * - @functionname{static_memory_function_malloctaskpooljob}
 * - @functionname{static_memory_function_freetaskpooljob}
 * - @functionname{static_memory_function_malloctaskpooltimerevent}
 * - @functionname{static_memory_function_freetaskpooltimerevent}
 * - @functionname{static_memory_function_freemessagebuffer}
 * - @functionname{static_memory_function_mallocmqttconnection}
 * - @functionname{static_memory_function_freemqttconnection}
 * - @functionname{static_memory_function_mallocmqttoperation}
 * - @functionname{static_memory_function_freemqttoperation}
 * - @functionname{static_memory_function_mallocmqttsubscription}
 * - @functionname{static_memory_function_freemqttsubscription}
 * - @functionname{static_memory_function_mallocshadowoperation}
 * - @functionname{static_memory_function_freeshadowoperation}
 * - @functionname{static_memory_function_mallocshadowsubscription}
 * - @functionname{static_memory_function_freeshadowsubscription}
 */

/*----------------------- Initialization and cleanup ------------------------*/

/**
 * @functionpage{IotStaticMemory_Init,static_memory,init}
 * @functionpage{IotStaticMemory_Cleanup,static_memory,cleanup}
 */

/**
 * @brief One-time initialization function for static memory.
 *
 * This function performs internal setup of static memory. <b>It must be called
 * once (and only once) before calling any other static memory function.</b>
 * Calling this function more than once without first calling
 * @ref static_memory_function_cleanup may result in a crash.
 *
 * @return `true` if initialization succeeded; `false` otherwise.
 *
 * @attention This function is called by `IotCommon_Init` and does not need to be
 * called by itself.
 *
 * @warning No thread-safety guarantees are provided for this function.
 *
 * @see static_memory_function_cleanup
 */
/* @[declare_static_memory_init] */
bool IotStaticMemory_Init( void );
/* @[declare_static_memory_init] */

/**
 * @brief One-time deinitialization function for static memory.
 *
 * This function frees resources taken in @ref static_memory_function_init.
 * It should be called after to clean up static memory. After this function
 * returns, @ref static_memory_function_init must be called again before
 * calling any other static memory function.
 *
 * @attention This function is called by `IotCommon_Cleanup` and does not need
 * to be called by itself.
 *
 * @warning No thread-safety guarantees are provided for this function.
 *
 * @see static_memory_function_init
 */
/* @[declare_static_memory_cleanup] */
void IotStaticMemory_Cleanup( void );
/* @[declare_static_memory_cleanup] */

/*------------------------ Message buffer management ------------------------*/

/**
 * @functionpage{Iot_MessageBufferSize,static_memory,messagebuffersize}
 * @functionpage{Iot_MallocMessageBuffer,static_memory,mallocmessagebuffer}
 * @functionpage{Iot_FreeMessageBuffer,static_memory,freemessagebuffer}
 */

/**
 * @brief Get the fixed size of a [message buffer]
 * (@ref static_memory_types_messagebuffers).
 *
 * The size of the message buffers are known at compile time, but it is a [constant]
 * (@ref IOT_MESSAGE_BUFFER_SIZE) that may not be visible to all source files.
 * This function allows other source files to know the size of a message buffer.
 *
 * @return The size, in bytes, of a single message buffer.
 */
/* @[declare_static_memory_messagebuffersize] */
size_t Iot_MessageBufferSize( void );
/* @[declare_static_memory_messagebuffersize] */

/**
 * @brief Get an empty [message buffer]
 * (@ref static_memory_types_messagebuffers).
 *
 * This function is the analog of [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html)
 * for message buffers.
 *
 * @param[in] size Requested size for a message buffer.
 *
 * @return Pointer to the start of a message buffer. If the `size` argument is larger
 * than the [fixed size of a message buffer](@ref IOT_MESSAGE_BUFFER_SIZE)
 * or no message buffers are available, `NULL` is returned.
 */
/* @[declare_static_memory_mallocmessagebuffer] */
void * Iot_MallocMessageBuffer( size_t size );
/* @[declare_static_memory_mallocmessagebuffer] */

/**
 * @brief Free an in-use [message buffer]
 * (@ref static_memory_types_messagebuffers).
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for message buffers.
 *
 * @param[in] ptr Pointer to the message buffer to free.
 */
/* @[declare_static_memory_freemessagebuffer] */
void Iot_FreeMessageBuffer( void * ptr );
/* @[declare_static_memory_freemessagebuffer] */

/*------------------- Task pool job and event management --------------------*/

/**
 * @functionpage{Iot_MallocTaskPoolJob,static_memory,malloctaskpooljob}
 * @functionpage{Iot_FreeTaskPoolJob,static_memory,freetaskpooljob}
 * @functionpage{Iot_MallocTaskPoolTimerEvent,static_memory,malloctaskpooltimerevent}
 * @functionpage{Iot_FreeTaskPoolTimerEvent,static_memory,freetaskpooltimerevent}
 */

/**
 * @brief Allocates memory to hold data for a new task pool job.
 *
 * This function is the analog of [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html)
 * for task pool jobs.
 *
 * @param[in] size Must be equal to the size of a task pool job.
 *
 * @return Pointer to a task pool job.
 */
/* @[declare_static_memory_malloctaskpooljob] */
void * Iot_MallocTaskPoolJob( size_t size );
/* @[declare_static_memory_malloctaskpooljob] */

/**
 * @brief Frees an in-use task pool job.
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for task pool jobs.
 *
 * @param[in] ptr Pointer to an active task pool job to free.
 */
/* @[declare_static_memory_freetaskpooljob] */
void Iot_FreeTaskPoolJob( void * ptr );
/* @[declare_static_memory_freetaskpooljob] */

/**
 * @brief Allocates memory to hold data for a new task pool timer event.
 *
 * This function is the analog of [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html)
 * for task pool timer events.
 *
 * @param[in] size Must be equal to the size of a task pool timer event.
 *
 * @return Pointer to a task pool timer event.
 */
/* @[declare_static_memory_malloctaskpooltimerevent] */
void * Iot_MallocTaskPoolTimerEvent( size_t size );
/* @[declare_static_memory_malloctaskpooltimerevent] */

/**
 * @brief Frees an in-use task pool timer event.
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for task pool timer events.
 *
 * @param[in] ptr Pointer to an active task pool timer event to free.
 */
/* @[declare_static_memory_freetaskpooltimerevent] */
void Iot_FreeTaskPoolTimerEvent( void * ptr );
/* @[declare_static_memory_freetaskpooltimerevent] */

/*----------------------- MQTT connection management ------------------------*/

/**
 * @functionpage{Iot_MallocMqttConnection,static_memory,mallocmqttconnection}
 * @functionpage{Iot_FreeMqttConnection,static_memory,freemqttconnection}
 */

/**
 * @brief Allocates memory to hold data for a new [MQTT connection]
 * (@ref static_memory_types_mqttconnections).
 *
 * This function is the analog of [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html)
 * for MQTT connections.
 *
 * @param[in] size Must be equal to sizeof( #_mqttConnection_t ).
 *
 * @return Pointer to an MQTT connection.
 */
/* @[declare_static_memory_mallocmqttconnection] */
void * Iot_MallocMqttConnection( size_t size );
/* @[declare_static_memory_mallocmqttconnection] */

/**
 * @brief Frees an in-use [MQTT connection]
 * (@ref static_memory_types_mqttconnections).
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for MQTT connections.
 *
 * @param[in] ptr Pointer to an active MQTT connection to free.
 */
/* @[declare_static_memory_freemqttconnection] */
void Iot_FreeMqttConnection( void * ptr );
/* @[declare_static_memory_freemqttconnection] */

/*------------------------ MQTT operation management ------------------------*/

/**
 * @functionpage{Iot_MallocMqttOperation,static_memory,mallocmqttoperation}
 * @functionpage{Iot_FreeMqttOperation,static_memory,freemqttoperation}
 */

/**
 * @brief Allocates memory to hold data for a new [MQTT operation]
 * (@ref static_memory_types_mqttoperations).
 *
 * This function is the analog of [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html)
 * for MQTT operations.
 *
 * @param[in] size Must be equal to sizeof( #_mqttOperation_t ).
 *
 * @return Pointer to an MQTT operation.
 */
/* @[declare_static_memory_mallocmqttoperation] */
void * Iot_MallocMqttOperation( size_t size );
/* @[declare_static_memory_mallocmqttoperation] */

/**
 * @brief Frees an in-use [MQTT operation]
 * (@ref static_memory_types_mqttoperations).
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for MQTT operations.
 *
 * @param[in] ptr Pointer to an active MQTT operation to free.
 */
/* @[declare_static_memory_freemqttoperation] */
void Iot_FreeMqttOperation( void * ptr );
/* @[declare_static_memory_freemqttoperation] */

/*---------------------- MQTT subscription management -----------------------*/

/**
 * @functionpage{Iot_MallocMqttSubscription,static_memory,mallocmqttsubscription}
 * @functionpage{Iot_FreeMqttSubscription,static_memory,freemqttsubscription}
 */

/**
 * @brief Allocates memory to hold data for a new [MQTT subscription]
 * (@ref static_memory_types_mqttsubscriptions).
 *
 * This function is the analog of [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html)
 * for subscriptions.
 *
 * @param[in] size Requested size for the subscription. Because each subscription
 * contains a different topic, subscriptions will be different sizes. This value
 * should be checked to make sure that the statically-allocated subscription
 * object is large enough to accommodate the new topic filter.
 *
 * @return Pointer to an MQTT subscription. If the size argument is larger than
 * the fixed size of an MQTT subscription object or no free MQTT subscriptions
 * are available, `NULL` is returned.
 */
/* @[declare_static_memory_mallocmqttsubscription] */
void * Iot_MallocMqttSubscription( size_t size );
/* @[declare_static_memory_mallocmqttsubscription] */

/**
 * @brief Frees an in-use [MQTT subscription]
 * (@ref static_memory_types_mqttsubscriptions).
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for MQTT subscriptions.
 *
 * @param[in] ptr Pointer to an active MQTT subscription to free.
 */
/* @[declare_static_memory_freemqttsubscription] */
void Iot_FreeMqttSubscription( void * ptr );
/* @[declare_static_memory_freemqttsubscription] */

/*---------------------- Shadow operation management ------------------------*/

/**
 * @functionpage{AwsIot_MallocShadowOperation,static_memory,mallocshadowoperation}
 * @functionpage{AwsIot_FreeShadowOperation,static_memory,freeshadowoperation}
 */

/**
 * @brief Allocates memory to hold data for a new [Shadow operation]
 * (@ref static_memory_types_shadowoperations).
 *
 * This function is the analog of [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html)
 * for Shadow operations.
 *
 * @param[in] size Must be equal to sizeof( #_shadowOperation_t ).
 *
 * @return Pointer to a Shadow operation.
 */
/* @[declare_static_memory_mallocshadowoperation] */
void * AwsIot_MallocShadowOperation( size_t size );
/* @[declare_static_memory_mallocshadowoperation] */

/**
 * @brief Frees an in-use [Shadow operation]
 * (@ref static_memory_types_shadowoperations).
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for Shadow operations.
 *
 * @param[in] ptr Pointer to an active Shadow operation to free.
 */
/* @[declare_static_memory_freeshadowoperation] */
void AwsIot_FreeShadowOperation( void * ptr );
/* @[declare_static_memory_freeshadowoperation] */

/*--------------------- Shadow subscription management ----------------------*/

/**
 * @functionpage{AwsIot_MallocShadowSubscription,static_memory,mallocshadowsubscription}
 * @functionpage{AwsIot_FreeShadowSubscription,static_memory,freeshadowsubscription}
 */

/**
 * @brief Allocates memory to hold data for a new [Shadow subscription]
 * (@ref static_memory_types_shadowsubscriptions).
 *
 * This function is the analog of [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html)
 * for Shadow subscriptions.
 *
 * @param[in] size Requested size for the subscription. Because each subscription
 * contains a different Thing Name, subscriptions will be different sizes. This
 * value should be checked to make sure that the statically-allocated subscription
 * object is large enough to accommodate the new Thing Name.
 *
 * @return Pointer to a Shadow subscription. If the size argument is larger than
 * the fixed size of a Shadow subscription object or no free Shadow subscriptions
 * are available, `NULL` is returned.
 */
/* @[declare_static_memory_mallocshadowsubscription] */
void * AwsIot_MallocShadowSubscription( size_t size );
/* @[declare_static_memory_mallocshadowsubscription] */

/**
 * @brief Frees an in-use [Shadow subscription]
 * (@ref static_memory_types_shadowsubscriptions).
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for Shadow subscriptions.
 *
 * @param[in] ptr Pointer to an active Shadow subscription to free.
 */
/* @[declare_static_memory_freeshadowsubscription] */
void AwsIot_FreeShadowSubscription( void * ptr );
/* @[declare_static_memory_freeshadowsubscription] */

#endif /* if !defined( _IOT_STATIC_MEMORY_H_ ) && ( IOT_STATIC_MEMORY_ONLY == 1 ) */
