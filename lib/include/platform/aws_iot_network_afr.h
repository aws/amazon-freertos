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
 * @file aws_iot_network_afr.h
 * @brief Network-related functions used by the AWS IoT libraries implemented on
 * Amazon FreeRTOS secure sockets.
 */

#ifndef _AWS_IOT_NETWORK_AFR_H_
#define _AWS_IOT_NETWORK_AFR_H_

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdbool.h>

/* Amazon FreeRTOS includes. */
#include "FreeRTOS.h"
#include "aws_secure_sockets.h"

/* MQTT include. */
#include "aws_iot_mqtt.h"

/**
 * @brief Opaque handle of a network connection.
 *
 * This type identifies a network connection, which is valid after a successful call
 * to #AwsIotNetwork_CreateConnection. A variable of this type is passed as the first
 * argument to network layer functions to identify which connection that function acts on.
 *
 * A call to #AwsIotNetwork_CloseConnection shuts down the network connection, but keeps
 * its resources allocated. A call to #AwsIotNetwork_DestroyConnection makes a connection
 * handle invalid. Once #AwsIotNetwork_DestroyConnection returns, the connection handle
 * should no longer be used.
 */
typedef void * AwsIotNetworkConnection_t;

/**
 * @brief Provides a default value for an #AwsIotNetworkConnection_t.
 *
 * All instances of #AwsIotNetworkConnection_t should be initialized with this
 * constant.
 *
 * @warning Failing to initialize a network connection with this initializer
 * may result in undefined behavior!
 * @note This initializer may change at any time in future versions, but its
 * names will remain the same.
 */
#define AWS_IOT_NETWORK_AFR_CONNECTION_INITIALIZER    NULL

/**
 * @defgroup NetworkConnectFlags Flags in #AwsIotNetworkConnectParams_t.flags
 * @brief Flags that modify the behavior of #AwsIotNetwork_CreateConnection,
 * passed in #AwsIotNetworkConnectParams_t.flags.
 *
 * Flags should be bitwise-ORed with each other.
 * @note The values of the flags may change at any time in future versions, but
 * their names will remain the same. Additionally, flags will be bitwise-exclusive
 * of each other.
 */
/** @{ */
#define AWS_IOT_NETWORK_AFR_FLAG_ALPN       ( ( uint32_t ) 0x00000001 ) /**< @brief Enable ALPN for a connection. */
#define AWS_IOT_NETWORK_AFR_FLAG_SECURED    ( ( uint32_t ) 0x00000002 ) /**< @brief Enable TLS security for a connection. */
#define AWS_IOT_NETWORK_AFR_FLAG_SNI        ( ( uint32_t ) 0x00000004 ) /**< @brief Enable SNI for a connection. */
/** @} */

/**
 * @brief Parameter to #AwsIotNetwork_CreateConnection.
 */
typedef struct AwsIotNetworkConnectParams
{
    const char * pcURL;            /**< The remote URL of a connection. */
    uint16_t usPort;               /**< The port of a connection. */
    uint32_t flags;                /**< Flags for a connection. See @ref NetworkConnectFlags. */
    char * pcServerCertificate;    /**< A custom trusted server certificate for a connection. Set to NULL to use the default. */
    size_t xServerCertificateSize; /**< The length (in bytes) of pcServerCertificate. */
} AwsIotNetworkConnectParams_t;

/**
 * @brief Provides default values for an #AwsIotNetworkConnectParams_t.
 *
 * All instances of #AwsIotNetworkConnectParams_t should be initialized
 * with this constant.
 *
 * @warning Failing to initialize an #AwsIotNetworkConnectParams_t with this
 * initializer may result in undefined behavior!
 * @note This initializer may change at any time in future versions, but its
 * names will remain the same.
 */
#define AWS_IOT_NETWORK_AFR_CONNECT_PARAMS_INITIALIZER    { 0 }

/**
 * @brief Open a new network connection.
 *
 * This function should be called before any other network function.
 * @param[in] pxConnectParams The parameters for the new connection.
 * @param[out] pxNewConnection The handle by which this new connection will be referenced.
 *
 * @return true if the new network connection was successfully created; false otherwise.
 */
bool AwsIotNetwork_CreateConnection( const AwsIotNetworkConnectParams_t * const pxConnectParams,
                                     AwsIotNetworkConnection_t * pxNewConnection );

/**
 * @brief Create the task that will process incoming MQTT packets.
 *
 * This function should be called after #AwsIotNetwork_CreateConnection if the MQTT
 * library is being used.
 * @param[in] xConnection The connection that MQTT packets will be received on.
 * @param[in] pxMqttConnection Pointer to pass to the MQTT callback.
 * @param[in] uxPriority Priority of the MQTT receive task. This task should run at a
 * high priority.
 * @param[in] xInitialReceiveBufferSize The size of the initial receive buffer to
 * allocate for the receive task. The receive task will reallocate a larger buffer
 * when necessary.
 */
bool AwsIotNetwork_CreateMqttReceiveTask( AwsIotNetworkConnection_t xConnection,
                                          AwsIotMqttConnection_t * pxMqttConnection,
                                          UBaseType_t uxPriority,
                                          size_t xInitialReceiveBufferSize );

/**
 * @brief Close a network connection.
 *
 * This function closes the given network connection. However, the resources for
 * the connection are not released by this function. This allows calls to
 * #AwsIotNetwork_Send (or similar functions) to receive an error and handle a
 * closed connection without the risk of crashing. Once it can be guaranteed that
 * the network connection won't be used further, it can be destroyed with a call to
 * #AwsIotNetwork_DestroyConnection.
 *
 * @param[in] xConnection The handle of the connection to close.
 */
void AwsIotNetwork_CloseConnection( AwsIotNetworkConnection_t xConnection );

/**
 * @brief Free resources in use by a connection.
 *
 * This function releases the resources used by a closed connection. It should be
 * called after #AwsIotNetwork_CloseConnection.
 * @param[in] xConnection The connection to destroy.
 *
 * @warning Using the network connection after calling this function will likely
 * result in a crash!
 */
void AwsIotNetwork_DestroyConnection( AwsIotNetworkConnection_t xConnection );

/**
 * @brief Sends data over a connection.
 *
 * The MQTT library requires this function signature.
 * @param[in] xConnection The handle of the connection used to send data.
 * For compatibility with other network implementations, this is a void*.
 * @param[in] pvMessage The message to send.
 * @param[in] xMessageLength The length of pvMessage.
 *
 * @return The number of bytes successfully sent, 0 on failure.
 */
size_t AwsIotNetwork_Send( void * xConnection,
                           const void * const pvMessage,
                           size_t xMessageLength );

#endif /* ifndef _AWS_IOT_NETWORK_AFR_H_ */
