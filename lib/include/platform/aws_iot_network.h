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
 * @file aws_iot_network.h
 * @brief Network-related functions used by the AWS IoT libraries.
 */

#ifndef _AWS_IOT_NETWORK_H_
#define _AWS_IOT_NETWORK_H_

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stddef.h>
#include <stdint.h>

/* MQTT include. */
#include "aws_iot_mqtt.h"

/**
 * @ingroup platform_datatypes_handles
 * @brief Opaque handle of a network connection.
 *
 * This type identifies a network connection, which is valid after a successful call
 * to @ref platform_network_function_createconnection. A variable of this type is
 * passed as the first argument to [network layer functions](@ref platform_network_functions)
 * to identify which connection that function acts on.
 *
 * A call to @ref platform_network_function_closeconnection shuts down the network
 * connection, but keeps its resources allocated. A call to
 * @ref platform_network_function_destroyconnection makes a connection handle
 * invalid. Once @ref platform_network_function_destroyconnection is called, the
 * connection handle should no longer be used.
 *
 * @initializer{AwsIotNetworkConnection_t,AWS_IOT_NETWORK_CONNECTION_INITIALIZER}
 */
typedef void * AwsIotNetworkConnection_t;

/**
 * @ingroup platform_datatypes_handles
 * @brief Function pointer for the [MQTT receive callback]
 * (@ref mqtt_function_receivecallback).
 *
 * This function pointer is used to identify the function @ref
 * mqtt_function_receivecallback.
 */
typedef int32_t ( * AwsIotMqttReceiveCallback_t )( AwsIotMqttConnection_t *,
                                                   const void *,
                                                   size_t,
                                                   size_t,
                                                   void ( * )( void * ) );

/**
 * @ingroup platform_datatypes_enums
 * @brief Return codes for [network functions](@ref platform_network_functions).
 */
typedef enum AwsIotNetworkError
{
    AWS_IOT_NETWORK_SUCCESS = 0,           /**< Function successfully completed. */
    AWS_IOT_NETWORK_INIT_FAILED,           /**< Initialization failed. Only returned by @ref platform_network_function_init. */
    AWS_IOT_NETWORK_BAD_PARAMETER,         /**< At least one parameter was invalid. */
    AWS_IOT_NETWORK_NO_MEMORY,             /**< Memory allocation failed. */
    AWS_IOT_NETWORK_DNS_LOOKUP_ERROR,      /**< Host name could not be translated.*/
    AWS_IOT_NETWORK_CREDENTIAL_READ_ERROR, /**< Credentials could not be read. */
    AWS_IOT_NETWORK_TLS_LIBRARY_ERROR,     /**< An error occurred in the system's TLS library. */
    AWS_IOT_NETWORK_SYSTEM_ERROR           /**< An error occurred when calling a system function. */
} AwsIotNetworkError_t;

/**
 * @ingroup platform_datatypes_paramstructs
 * @brief Parameters to @ref platform_network_function_createconnection for setting
 * up TLS.
 *
 * This structure provides @ref platform_network_function_createconnection with the
 * necessary information to set up TLS on a TCP connection.
 *
 * @paramfor @ref platform_network_function_createconnection
 *
 * @initializer{AwsIotNetworkTlsInfo_t,AWS_IOT_NETWORK_TLS_INFO_INITIALIZER}
 */
typedef struct AwsIotNetworkTlsInfo
{
    /**
     * @brief Set this to a non-NULL value to use ALPN.
     *
     * This string must be NULL-terminated.
     *
     * See [this link]
     * (https://aws.amazon.com/blogs/iot/mqtt-with-tls-client-authentication-on-port-443-why-it-is-useful-and-how-it-works/)
     * for more information.
     */
    const char * pAlpnProtos;

    /**
     * @brief Set this to a non-zero value to use TLS max fragment length
     * negotiation (TLS MFLN).
     *
     * @note The system's TLS library may have a minimum value for this parameter.
     * @ref platform_network_function_createconnection may return an error if
     * this parameter is too small.
     */
    size_t maxFragmentLength;

    /**
     * @brief Disable server name indication (SNI) for a TLS session.
     */
    bool disableSni;

    /* Credentials. The exact meaning of these parameters depends on the system.
     * For example, on Linux these may be used as the paths to credentials, while
     * on FreeRTOS they may be the PEM-encoded credentials themselves. Note that
     * some systems may ignore the "length" parameters. */
    const char * pRootCa;     /**< @brief Trusted server root certificate. */
    size_t rootCaLength;      /**< @brief Length of #AwsIotNetworkTlsInfo_t.pRootCa. */
    const char * pClientCert; /**< @brief Client certificate. */
    size_t clientCertLength;  /**< @brief Length of #AwsIotNetworkTlsInfo_t.pClientCert. */
    const char * pPrivateKey; /**< @brief Client certificate private key. */
    size_t privateKeyLength;  /**< @brief Length of #AwsIotNetworkTlsInfo_t.pPrivateKey. */
} AwsIotNetworkTlsInfo_t;

/**
 * @brief Provides a default value for an #AwsIotNetworkConnection_t.
 *
 * All instances of #AwsIotNetworkConnection_t should be initialized with this
 * constant.
 *
 * @code{c}
 * AwsIotNetworkConnection_t networkConnection = AWS_IOT_NETWORK_CONNECTION_INITIALIZER;
 * @endcode
 *
 * @warning Failing to initialize a network connection with this initializer
 * may result in undefined behavior!
 * @note This initializer may change at any time in future versions, but its
 * names will remain the same.
 */
#define AWS_IOT_NETWORK_CONNECTION_INITIALIZER    NULL

/**
 * @brief Provides default values for an #AwsIotNetworkTlsInfo_t.
 *
 * All instances of #AwsIotNetworkTlsInfo_t should be initialized with this
 * constant. In most use cases, the default values should not be changed.
 *
 * @code{c}
 * AwsIotNetworkTlsInfo_t tlsInfo = AWS_IOT_NETWORK_TLS_INFO_INITIALIZER;
 * @endcode
 *
 * @warning Failing to initialize an #AwsIotNetworkTlsInfo_t with this initializer
 * may result in undefined behavior!
 * @note This initializer may change at any time in future versions, but its
 * name will remain the same.
 */
#define AWS_IOT_NETWORK_TLS_INFO_INITIALIZER      { .pAlpnProtos = "\x0ex-amzn-mqtt-ca" }

/**
 * @functionspage{platform_network,platform network component,Networking}
 * - @functionname{platform_network_function_init}
 * - @functionname{platform_network_function_cleanup}
 * - @functionname{platform_network_function_createconnection}
 * - @functionname{platform_network_function_closeconnection}
 * - @functionname{platform_network_function_destroyconnection}
 * - @functionname{platform_network_function_setmqttreceivecallback}
 * - @functionname{platform_network_function_send}
 */

/**
 * @functionpage{AwsIotNetwork_Init,platform_network,init}
 * @functionpage{AwsIotNetwork_Cleanup,platform_network,cleanup}
 * @functionpage{AwsIotNetwork_CreateConnection,platform_network,createconnection}
 * @functionpage{AwsIotNetwork_CloseConnection,platform_network,closeconnection}
 * @functionpage{AwsIotNetwork_DestroyConnection,platform_network,destroyconnection}
 * @functionpage{AwsIotNetwork_SetMqttReceiveCallback,platform_network,setmqttreceivecallback}
 * @functionpage{AwsIotNetwork_Send,platform_network,send}
 */

/**
 * @brief One-time initialization function for the platform networking component.
 *
 * This function performs internal setup of the platform networking component.
 * <b>It must be called once (and only once) before calling any other platform
 * networking function</b>. Calling this function more than once without first calling
 * @ref platform_network_function_cleanup may result in a crash.
 *
 * @return #AWS_IOT_NETWORK_SUCCESS or #AWS_IOT_NETWORK_INIT_FAILED.
 *
 * @warning No thread-safety guarantees are provided for this function.
 *
 * @see @ref platform_network_function_cleanup
 *
 * @code{c}
 * // Before calling any other network function.
 * if( AwsIotNetwork_Init() == AWS_IOT_NETWORK_SUCCESS )
 * {
 *     // Do something with the network library...
 *
 *     // After the network is no longer needed.
 *     AwsIotNetwork_Cleanup();
 * }
 * else
 * {
 *     // Initialization failed.
 * }
 * @endcode
 */
/* @[declare_platform_network_init] */
AwsIotNetworkError_t AwsIotNetwork_Init( void );
/* @[declare_platform_network_init] */

/**
 * @brief One-time deinitialization function for the platform networking component.
 *
 * This function frees resources taken in @ref platform_network_function_init.
 * It should be called after [destroying all network connections]
 * (@ref platform_network_function_destroyconnection) to clean up the platform
 * networking component. After this function returns, @ref platform_network_function_init
 * must be called again before calling any other platform networking function.
 *
 * @warning No thread-safety guarantees are provided for this function. Do not
 * call this function if any network connections exist!
 *
 * @see @ref platform_network_function_init
 */
/* @[declare_platform_network_cleanup] */
void AwsIotNetwork_Cleanup( void );
/* @[declare_platform_network_cleanup] */

/**
 * @brief Open a TCP connection to a remote server with an option for TLS.
 *
 * This function opens a TCP connection to a server on a given port. If security
 * is needed, parameter `pTlsInfo` may be passed to set up TLS on the TCP connection.
 *
 * @param[out] pNetworkConnection The handle by which this new connection will
 * be referenced.
 * @param[in] pHostName Host name for connection. Must be null-terminated.
 * @param[in] port Port (in host-order) for connection.
 * @param[in] pTlsInfo TLS setup parameters. Optional; pass `NULL` for an unsecured
 * connection.
 *
 * @return Any #AwsIotNetworkError_t except #AWS_IOT_NETWORK_INIT_FAILED or
 * #AWS_IOT_NETWORK_SYSTEM_ERROR.
 *
 * @see @ref platform_network_function_closeconnection,
 * @ref platform_network_function_destroyconnection
 *
 * @code{c}
 * // Network connection handle and TLS setup parameters.
 * AwsIotNetworkConnection_t networkConnection = AWS_IOT_NETWORK_CONNECTION_INITIALIZER;
 * AwsIotNetworkTlsInfo_t tlsInfo = AWS_IOT_NETWORK_TLS_INFO_INITIALIZER;
 *
 * // Example server.
 * const char * const pHostName = "test.mosquitto.org";
 * uint16_t port = 8883;
 *
 * // Set the TLS setup credentials. The exact format of these credentials depends
 * // on the system.
 * tlsInfo.pRootCa = ROOT_CA;
 * tlsInfo.rootCaLength = ROOT_CA_LENGTH;
 * tlsInfo.pClientCert = CLIENT_CERT;
 * tlsInfo.clientCertLength = CLIENT_CERT_LENGTH;
 * tlsInfo.pPrivateKey = PRIVATE_KEY;
 * tlsInfo.privateKeyLength = PRIVATE_KEY_LENGTH;
 *
 * // Returns AWS_IOT_NETWORK_SUCCESS and sets networkConnection when the
 * // connection is established.
 * AwsIotNetworkError_t result = AwsIotNetwork_CreateConnection( &networkConnection,
 *                                                               pHostName,
 *                                                               port,
 *                                                               &tlsInfo );
 *
 * // Do something with the connection...
 *
 * // Close the connection.
 * AwsIotNetwork_CloseConnection( networkConnection );
 *
 * // Destroy the network connection. This should only be called once the
 * // connection is guaranteed to be unused.
 * AwsIotNetwork_DestroyConnection( networkConnection );
 * @endcode
 */
/* @[declare_platform_network_createconnection] */
AwsIotNetworkError_t AwsIotNetwork_CreateConnection( AwsIotNetworkConnection_t * const pNetworkConnection,
                                                     const char * const pHostName,
                                                     uint16_t port,
                                                     const AwsIotNetworkTlsInfo_t * const pTlsInfo );
/* @[declare_platform_network_createconnection] */

/**
 * @brief Close a network connection.
 *
 * This function closes the TCP socket and cleans up TLS. However, the resources
 * for the connection are not released by this function. This allows calls to
 * @ref platform_network_function_send (or similar functions) to receive an error
 * and handle a closed connection without the risk of crashing. Once it can be
 * guaranteed that the network connection won't be used further, it can be destroyed
 * with @ref platform_network_function_destroyconnection.
 *
 * Calling this function also removes any registered receive callback.
 *
 * @param[in] networkConnection The handle of the connection to close.
 *
 * @note It must be safe to call this function on an already-closed connection.
 *
 * @see @ref platform_network_function_createconnection,
 * @ref platform_network_function_destroyconnection
 */
/* @[declare_platform_network_closeconnection] */
void AwsIotNetwork_CloseConnection( AwsIotNetworkConnection_t networkConnection );
/* @[declare_platform_network_closeconnection] */

/**
 * @brief Free resources in use by a connection.
 *
 * This function releases the resources used by a closed connection. It should
 * be called after @ref platform_network_function_closeconnection.
 *
 * @param[in] networkConnection The connection to destroy.
 *
 * @see @ref platform_network_function_createconnection,
 * @ref platform_network_function_closeconnection
 *
 * @warning Using the network connection after calling this function will likely
 * result in a crash!
 */
/* @[declare_platform_network_destroyconnection] */
void AwsIotNetwork_DestroyConnection( AwsIotNetworkConnection_t networkConnection );
/* @[declare_platform_network_destroyconnection] */

/**
 * @brief Register the [MQTT callback function](@ref mqtt_function_receivecallback)
 * to be called when MQTT data is read from the network.
 *
 * The [MQTT library](@ref mqtt) expects to be asynchronously notified of incoming
 * network data. This function creates a thread that will notify the MQTT library
 * of incoming data.
 *
 * @param[in] networkConnection The handle of the connection to receive from.
 * @param[in] pMqttConnection Pointer to the MQTT connection to pass to the MQTT
 * callback.
 * @param[in] receiveCallback @ref mqtt_function_receivecallback
 *
 * @return #AWS_IOT_NETWORK_SUCCESS or #AWS_IOT_NETWORK_SYSTEM_ERROR.
 *
 * @see @ref mqtt_function_receivecallback
 *
 * @code{c}
 * // MQTT network receive callback.
 * int32_t AwsIotMqtt_ReceiveCallback( AwsIotMqttConnection_t * pMqttConnection,
 *                                     const void * pReceivedData,
 *                                     size_t offset,
 *                                     size_t dataLength,
 *                                     void ( * freeReceivedData )( void * ) );
 *
 * void setCallback( AwsIotNetworkConnection_t networkConnection, AwsIotMqttConnection_t * pMqttConnection )
 * {
 *     // Returns AWS_IOT_NETWORK_SUCCESS when the callback is successfully set.
 *     AwsIotNetworkError_t result = AwsIotNetwork_SetMqttReceiveCallback( networkConnection,
 *                                                                         pMqttConnection,
 *                                                                         AwsIotMqtt_ReceiveCallback );
 * }
 * @endcode
 */
/* @[declare_platform_network_setmqttreceivecallback] */
AwsIotNetworkError_t AwsIotNetwork_SetMqttReceiveCallback( AwsIotNetworkConnection_t networkConnection,
                                                           AwsIotMqttConnection_t * pMqttConnection,
                                                           AwsIotMqttReceiveCallback_t receiveCallback );
/* @[declare_platform_network_setmqttreceivecallback] */

/**
 * @brief Sends data over a connection.
 *
 * Attempts to transmit `messageLength` bytes of `pMessage` across
 * `networkConnection`. Returns the number of bytes actually sent, `0` on
 * failure.
 *
 * @param[in] networkConnection The handle of the connection used to send data.
 * For compatibility with other network implementations, this is a void*.
 * @param[in] pMessage The message to send.
 * @param[in] messageLength The length of pMessage.
 *
 * @return The number of bytes successfully sent, `0` on failure.
 */
/* @[declare_platform_network_send] */
size_t AwsIotNetwork_Send( void * networkConnection,
                           const void * const pMessage,
                           size_t messageLength );
/* @[declare_platform_network_send] */

#endif /* ifndef _AWS_IOT_NETWORK_H_ */
