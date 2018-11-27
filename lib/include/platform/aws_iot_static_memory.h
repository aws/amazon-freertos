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
 * @file aws_iot_static_memory.h
 * @brief Functions for managing static buffers. Only used when
 * @ref AWS_IOT_STATIC_MEMORY_ONLY is `1`.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* The functions in this file should only exist in static memory only mode, hence
 * the check for AWS_IOT_STATIC_MEMORY_ONLY in the double inclusion guard. */
#if !defined( _AWS_IOT_STATIC_MEMORY_H_ ) && ( AWS_IOT_STATIC_MEMORY_ONLY == 1 )
#define _AWS_IOT_STATIC_MEMORY_H_

/* Standard includes. */
#include <stdbool.h>
#include <stddef.h>

/**
 * @functionspage{platform_static_memory,platform static memory component,Static Memory}
 * - @functionname{platform_static_memory_function_messagebuffersize}
 * - @functionname{platform_static_memory_function_mallocmessagebuffer}
 * - @functionname{platform_static_memory_function_freemessagebuffer}
 * - @functionname{platform_static_memory_function_mallocmqttconnection}
 * - @functionname{platform_static_memory_function_freemqttconnection}
 * - @functionname{platform_static_memory_function_mallocmqttoperation}
 * - @functionname{platform_static_memory_function_freemqttoperation}
 * - @functionname{platform_static_memory_function_mallocmqtttimerevent}
 * - @functionname{platform_static_memory_function_freemqtttimerevent}
 * - @functionname{platform_static_memory_function_mallocmqttsubscription}
 * - @functionname{platform_static_memory_function_freemqttsubscription}
 * - @functionname{platform_static_memory_function_mallocshadowoperation}
 * - @functionname{platform_static_memory_function_freeshadowoperation}
 * - @functionname{platform_static_memory_function_mallocshadowsubscription}
 * - @functionname{platform_static_memory_function_freeshadowsubscription}
 */

/*------------------------ Message buffer management ------------------------*/

/**
 * @functionpage{AwsIot_MessageBufferSize,platform_static_memory,messagebuffersize}
 * @functionpage{AwsIot_MallocMessageBuffer,platform_static_memory,mallocmessagebuffer}
 * @functionpage{AwsIot_FreeMessageBuffer,platform_static_memory,freemessagebuffer}
 */

/**
 * @brief Get the fixed size of a [message buffer]
 * (@ref platform_static_memory_types_messagebuffers).
 *
 * The size of the message buffers are known at compile time, but it is a [constant]
 * (@ref AWS_IOT_MESSAGE_BUFFER_SIZE) that may not be visible to all source files.
 * This function allows other source files to know the size of a message buffer.
 *
 * @return The size, in bytes, of a single message buffer.
 */
/* @[declare_platform_static_memory_messagebuffersize] */
size_t AwsIot_MessageBufferSize( void );
/* @[declare_platform_static_memory_messagebuffersize] */

/**
 * @brief Get an empty [message buffer]
 * (@ref platform_static_memory_types_messagebuffers).
 *
 * This function is the analog of [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html)
 * for message buffers.
 *
 * @param[in] size Requested size for a message buffer.
 *
 * @return Pointer to the start of a message buffer. If the `size` argument is larger
 * than the [fixed size of a message buffer](@ref AWS_IOT_MESSAGE_BUFFER_SIZE)
 * or no message buffers are available, `NULL` is returned.
 */
/* @[declare_platform_static_memory_mallocmessagebuffer] */
void * AwsIot_MallocMessageBuffer( size_t size );
/* @[declare_platform_static_memory_mallocmessagebuffer] */

/**
 * @brief Free an in-use [message buffer]
 * (@ref platform_static_memory_types_messagebuffers).
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for message buffers.
 *
 * @param[in] ptr Pointer to the message buffer to free.
 */
/* @[declare_platform_static_memory_freemessagebuffer] */
void AwsIot_FreeMessageBuffer( void * ptr );
/* @[declare_platform_static_memory_freemessagebuffer] */

/*----------------------- MQTT connection management ------------------------*/

/**
 * @functionpage{AwsIot_MallocMqttConnection,platform_static_memory,mallocmqttconnection}
 * @functionpage{AwsIot_FreeMqttConnection,platform_static_memory,freemqttconnection}
 */

/**
 * @brief Allocates memory to hold data for a new [MQTT connection]
 * (@ref platform_static_memory_types_mqttconnections).
 *
 * This function is the analog of [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html)
 * for MQTT connections.
 *
 * @param[in] size Must be equal to sizeof( #_mqttConnection_t ).
 *
 * @return Pointer to an MQTT connection.
 */
/* @[declare_platform_static_memory_mallocmqttconnection] */
void * AwsIot_MallocMqttConnection( size_t size );
/* @[declare_platform_static_memory_mallocmqttconnection] */

/**
 * @brief Frees an in-use [MQTT connection]
 * (@ref platform_static_memory_types_mqttconnections).
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for MQTT connections.
 *
 * @param[in] ptr Pointer to an active MQTT connection to free.
 */
/* @[declare_platform_static_memory_freemqttconnection] */
void AwsIot_FreeMqttConnection( void * ptr );
/* @[declare_platform_static_memory_freemqttconnection] */

/*------------------------ MQTT operation management ------------------------*/

/**
 * @functionpage{AwsIot_MallocMqttOperation,platform_static_memory,mallocmqttoperation}
 * @functionpage{AwsIot_FreeMqttOperation,platform_static_memory,freemqttoperation}
 */

/**
 * @brief Allocates memory to hold data for a new [MQTT operation]
 * (@ref platform_static_memory_types_mqttoperations).
 *
 * This function is the analog of [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html)
 * for MQTT operations.
 *
 * @param[in] size Must be equal to sizeof( #_mqttOperation_t ).
 *
 * @return Pointer to an MQTT operation.
 */
/* @[declare_platform_static_memory_mallocmqttoperation] */
void * AwsIot_MallocMqttOperation( size_t size );
/* @[declare_platform_static_memory_mallocmqttoperation] */

/**
 * @brief Frees an in-use [MQTT operation]
 * (@ref platform_static_memory_types_mqttoperations).
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for MQTT operations.
 *
 * @param[in] ptr Pointer to an active MQTT operation to free.
 */
/* @[declare_platform_static_memory_freemqttoperation] */
void AwsIot_FreeMqttOperation( void * ptr );
/* @[declare_platform_static_memory_freemqttoperation] */

/*----------------------- MQTT timer event management -----------------------*/

/**
 * @functionpage{AwsIot_MallocMqttTimerEvent,platform_static_memory,mallocmqtttimerevent}
 * @functionpage{AwsIot_FreeMqttTimerEvent,platform_static_memory,freemqtttimerevent}
 */

/**
 * @brief Allocates memory to hold data for a new [MQTT timer event]
 * (@ref platform_static_memory_types_mqtttimerevents).
 *
 * This function is the analog of [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html) for MQTT timer events.
 *
 * @param[in] size Must be equal to sizeof( #_mqttTimerEvent_t ).
 *
 * @return Pointer to an MQTT timer event.
 */
/* @[declare_platform_static_memory_mallocmqtttimerevent] */
void * AwsIot_MallocMqttTimerEvent( size_t size );
/* @[declare_platform_static_memory_mallocmqtttimerevent] */

/**
 * @brief Frees an in-use [MQTT timer event]
 * (@ref platform_static_memory_types_mqtttimerevents).
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for MQTT timer events.
 *
 * @param[in] ptr Pointer to an active MQTT timer event to free.
 */
/* @[declare_platform_static_memory_freemqtttimerevent] */
void AwsIot_FreeMqttTimerEvent( void * ptr );
/* @[declare_platform_static_memory_freemqtttimerevent] */

/*---------------------- MQTT subscription management -----------------------*/

/**
 * @functionpage{AwsIot_MallocMqttSubscription,platform_static_memory,mallocmqttsubscription}
 * @functionpage{AwsIot_FreeMqttSubscription,platform_static_memory,freemqttsubscription}
 */

/**
 * @brief Allocates memory to hold data for a new [MQTT subscription]
 * (@ref platform_static_memory_types_mqttsubscriptions).
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
/* @[declare_platform_static_memory_mallocmqttsubscription] */
void * AwsIot_MallocMqttSubscription( size_t size );
/* @[declare_platform_static_memory_mallocmqttsubscription] */

/**
 * @brief Frees an in-use [MQTT subscription]
 * (@ref platform_static_memory_types_mqttsubscriptions).
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for MQTT subscriptions.
 *
 * @param[in] ptr Pointer to an active MQTT subscription to free.
 */
/* @[declare_platform_static_memory_freemqttsubscription] */
void AwsIot_FreeMqttSubscription( void * ptr );
/* @[declare_platform_static_memory_freemqttsubscription] */

/*---------------------- Shadow operation management ------------------------*/

/**
 * @functionpage{AwsIot_MallocShadowOperation,platform_static_memory,mallocshadowoperation}
 * @functionpage{AwsIot_FreeShadowOperation,platform_static_memory,freeshadowoperation}
 */

/**
 * @brief Allocates memory to hold data for a new [Shadow operation]
 * (@ref platform_static_memory_types_shadowoperations).
 *
 * This function is the analog of [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html)
 * for Shadow operations.
 *
 * @param[in] size Must be equal to sizeof( #_shadowOperation_t ).
 *
 * @return Pointer to a Shadow operation.
 */
/* @[declare_platform_static_memory_mallocshadowoperation] */
void * AwsIot_MallocShadowOperation( size_t size );
/* @[declare_platform_static_memory_mallocshadowoperation] */

/**
 * @brief Frees an in-use [Shadow operation]
 * (@ref platform_static_memory_types_shadowoperations).
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for Shadow operations.
 *
 * @param[in] ptr Pointer to an active Shadow operation to free.
 */
/* @[declare_platform_static_memory_freeshadowoperation] */
void AwsIot_FreeShadowOperation( void * ptr );
/* @[declare_platform_static_memory_freeshadowoperation] */

/*--------------------- Shadow subscription management ----------------------*/

/**
 * @functionpage{AwsIot_MallocShadowSubscription,platform_static_memory,mallocshadowsubscription}
 * @functionpage{AwsIot_FreeShadowSubscription,platform_static_memory,freeshadowsubscription}
 */

/**
 * @brief Allocates memory to hold data for a new [Shadow subscription]
 * (@ref platform_static_memory_types_shadowsubscriptions).
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
/* @[declare_platform_static_memory_mallocshadowsubscription] */
void * AwsIot_MallocShadowSubscription( size_t size );
/* @[declare_platform_static_memory_mallocshadowsubscription] */

/**
 * @brief Frees an in-use [Shadow subscription]
 * (@ref platform_static_memory_types_shadowsubscriptions).
 *
 * This function is the analog of [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html)
 * for Shadow subscriptions.
 *
 * @param[in] ptr Pointer to an active Shadow subscription to free.
 */
/* @[declare_platform_static_memory_freeshadowsubscription] */
void AwsIot_FreeShadowSubscription( void * ptr );
/* @[declare_platform_static_memory_freeshadowsubscription] */

#endif /* if !defined( _AWS_IOT_STATIC_MEMORY_H_ ) && ( AWS_IOT_STATIC_MEMORY_ONLY == 1 ) */
