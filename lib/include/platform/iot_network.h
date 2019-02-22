/*
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_network.h
 * @brief Abstraction of network functions used by libraries in this SDK.
 */

#ifndef _IOT_NETWORK_H_
#define _IOT_NETWORK_H_

/* Standard includes. */
#include <stdint.h>

/**
 * @ingroup platform_datatypes_enums
 * @brief Return codes for [network functions](@ref platform_network_functions).
 */
typedef enum IotNetworkError
{
    IOT_NETWORK_SUCCESS = 0,   /**< Function successfully completed. */
    IOT_NETWORK_FAILURE,       /**< Generic failure not covered by other values. */
    IOT_NETWORK_BAD_PARAMETER, /**< At least one parameter was invalid. */
    IOT_NETWORK_NO_MEMORY,     /**< Memory allocation failed. */
    IOT_NETWORK_SYSTEM_ERROR   /**< An error occurred when calling a system API. */
} IotNetworkError_t;

/**
 * @page platform_network_functions Networking
 * @brief Functions of the network abstraction component.
 *
 * The network abstraction component does not declare functions, but uses function
 * pointers instead. This allows multiple network stacks to be used at the same time.
 * Libraries that require the network will request an #IotNetworkInterface_t
 * parameter. The members of the #IotNetworkInterface_t will be called whenever
 * the library interacts with the network.
 *
 * The following function pointers are associated with an #IotNetworkInterface_t.
 * Together, they represent a network stack.
 * - @functionname{platform_network_function_create}
 * - @functionname{platform_network_function_setreceivecallback}
 * - @functionname{platform_network_function_send}
 * - @functionname{platform_network_function_receive}
 * - @functionname{platform_network_function_close}
 * - @functionname{platform_network_function_destroy}
 * - @functionname{platform_network_function_receivecallback}
 */

/**
 * @functionpage{IotNetworkInterface_t::create,platform_network,create}
 * @functionpage{IotNetworkInterface_t::setReceiveCallback,platform_network,setreceivecallback}
 * @functionpage{IotNetworkInterface_t::send,platform_network,send}
 * @functionpage{IotNetworkInterface_t::receive,platform_network,receive}
 * @functionpage{IotNetworkInterface_t::close,platform_network,close}
 * @functionpage{IotNetworkInterface_t::destroy,platform_network,destroy}
 * @functionpage{IotNetworkReceiveCallback_t,platform_network,receivecallback}
 */

/**
 * @brief Provide an asynchronous notification of incoming network data.
 *
 * A function with this signature may be set with @ref platform_network_function_setreceivecallback
 * to be invoked when data is available on the network.
 *
 * An important concept associated with this function is <i>buffer ownership</i>.
 * Normally, once `pReceivedData` is passed to this function, it is considered
 * property of the library that set the receive callback. <b>The network stack
 *  must keep `pReceivedData` valid  and in-scope even after this function returns</b>,
 * and must not make any changes to `pReceivedData` until `freeReceivedData` is called
 * (which transfers ownership of `pReceivedData` back to the network stack).
 *
 * <b>If `freeReceivedData` is `NULL`, then ownership of `pReceivedData` will always
 * revert to the network stack as soon as the library's callback function returns.</b>
 * In this scenario, the library must copy any data it requires out of `pReceivedData`.
 * Therefore, passing `NULL` for `freeReceivedData` may increase memory usage.
 *
 * @param[in] pContext The third argument passed to @ref platform_network_function_setreceivecallback.
 * @param[in] pConnection The connection on which data was received, defined by
 * the network stack.
 * @param[in] pReceivedData Buffer holding data received from the network. This buffer
 * must remain valid and in-scope until `freeReceivedData` is called if this function
 * returns a value of `dataLength-offset`.
 * @param[in] receivedDataLength The length of the buffer `pReceivedData`.
 * @param[in] offset The offset (in bytes) into `pReceivedData` where the received data
 * begins. The received data ranges from `pReceivedData+offset` to `pReceivedData+dataLength`.
 * Pass `0` to start from the beginning of `pReceivedData`.
 * @param[in] freeReceivedData The function to call when finished with `pReceivedData`.
 * Its parameter must be `pReceivedData`. Pass `NULL` to ignore.
 *
 * @return
 * The network stack must respond to this function's return value as follows:
 * - This function returns `-1` to indicate a fatal, unrecoverable error. The network
 * connection should no longer be used. The receive callback will not call `freeReceivedData`
 * if it returns `-1`.
 * - This function returns a positive value less than `dataLength-offset` to indicate
 * that the received data was incomplete in some way. The network stack should keep
 * the data in `pReceivedBuffer` and wait more data to arrive. The receive callback should
 * be called again with this additional data. The receive callback will not call
 * `freeReceivedData` in this case.
 * - This function returns `dataLength-offset` to indicate all received data was
 * processed. The buffer `pReceivedData` must remain valid and in-scope until
 * `freeReceivedData` is called.
 *
 * @attention The function @ref platform_network_function_receive must not be
 * called if a receive callback is active!
 */
/* @[declare_platform_network_receivecallback] */
typedef int32_t ( * IotNetworkReceiveCallback_t )( void * pContext,
                                                   void * pConnection,
                                                   const uint8_t * pReceivedData,
                                                   size_t receivedDataLength,
                                                   size_t offset,
                                                   void ( * freeReceivedData )( void * ) );
/* @[declare_platform_network_receivecallback] */

/**
 * @ingroup platform_datatypes_paramstructs
 * @brief Represents the functions of a network stack.
 *
 * Functions that match these signatures should be implemented against a system's
 * network stack. See the `platform` directory for existing implementations.
 */
typedef struct IotNetworkInterface
{
    /**
     * @brief Create a new network connection.
     *
     * This function allocates resources and establishes a new network connection.
     * @param[in] pConnectionInfo Represents information needed to set up the
     * new connection, defined by the network stack.
     * @param[in] pCredentialInfo Represents information needed to secure the
     * new connection, defined by the network stack.
     * @param[out] pConnection Set to represent a new connection, defined by the
     * network stack.
     *
     * @return Any #IotNetworkError_t, as defined by the network stack.
     */
    /* @[declare_platform_network_create] */
    IotNetworkError_t ( * create )( void * pConnectionInfo,
                                    void * pCredentialInfo,
                                    void * const pConnection );
    /* @[declare_platform_network_create] */

    /**
     * @brief Register an @ref platform_network_function_receivecallback.
     *
     * Sets an @ref platform_network_function_receivecallback to be called
     * asynchronously when data arrives on the network. The network stack
     * should invoke this function "as if" it were the thread routine of a
     * detached thread.
     *
     * Each network connection may only have one receive callback at any time.
     * If this function is called for a connection that already has a receive
     * callback, the existing callback should be replaced. If the `receiveCallback`
     * parameter is `NULL`, any existing receive callback should be removed. In
     * addition, @ref platform_network_function_close is expected to remove any
     * active receive callbacks.
     *
     * @param[in] pConnection The connection to associate with the receive callback.
     * @param[in] receiveCallback The function to invoke for incoming network data.
     * @param[in] pContext A value to pass as the first parameter to the receive callback.
     *
     * @return Any #IotNetworkError_t, as defined by the network stack.
     *
     * @attention @ref platform_network_function_receive must not be called if
     * a receive callback is set!
     *
     * @see platform_network_function_receivecallback
     */
    /* @[declare_platform_network_setreceivecallback] */
    IotNetworkError_t ( * setReceiveCallback )( void * pConnection,
                                                IotNetworkReceiveCallback_t receiveCallback,
                                                void * pContext );
    /* @[declare_platform_network_setreceivecallback] */

    /**
     * @brief Send data over a return connection.
     *
     * Attempts to transmit `messageLength` bytes of `pMessage` across the
     * connection represented by `pConnection`. Returns the number of bytes
     * actually sent, `0` on failure.
     *
     * @param[in] pConnection The connection used to send data, defined by the
     * network stack.
     * @param[in] pMessage The message to send.
     * @param[in] messageLength The length of `pMessage`.
     *
     * @return The number of bytes successfully sent, `0` on failure.
     */
    /* @[declare_platform_network_send] */
    size_t ( * send )( void * pConnection,
                       const uint8_t * pMessage,
                       size_t messageLength );
    /* @[declare_platform_network_send] */

    /**
     * @brief Block and wait for incoming network data.
     *
     * Wait for a message of size `bytesRequested` to arrive on the network and
     * place it in `pBuffer`.
     *
     * @param[in] pConnection The connection to wait on, defined by the network
     * stack.
     * @param[out] pBuffer Where to place the incoming network data. This buffer
     * must be at least `bytesRequested` in size.
     * @param[in] bytesRequested How many bytes to wait for. `pBuffer` must be at
     * least this size.
     *
     * @return The number of bytes successfully received. This should be
     * `bytesRequested` when successful. Any other value may indicate an error.
     *
     * @attention This function must not be called if a
     * [receive callback](@ref platform_network_function_receivecallback) is active!
     */
    /* @[declare_platform_network_receive] */
    size_t ( * receive )( void * pConnection,
                          uint8_t * const pBuffer,
                          size_t bytesRequested );
    /* @[declare_platform_network_receive] */

    /**
     * @brief Close a network connection.
     *
     * This function closes the connection, but does not release the resources
     * used by the connection. This allows calls to other networking functions
     * to return an error and handle a closed connection without the risk of
     * crashing. Once it can be guaranteed that `pConnection` will no longer be
     * used, the connection can be destroyed with @ref platform_network_function_destroy.
     *
     * In addition to closing the connection, this function should also remove
     * any active [receive callback](@ref platform_network_function_receivecallback).
     *
     * @param[in] pConnection The network connection to close, defined by the
     * network stack.
     *
     * @return Any #IotNetworkError_t, as defined by the network stack.
     *
     * @note It must be safe to call this function on an already-closed connection.
     */
    /* @[declare_platform_network_close] */
    IotNetworkError_t ( * close )( void * pConnection );
    /* @[declare_platform_network_close] */

    /**
     * @brief Free resources used by a network connection.
     *
     * This function releases the resources of a closed connection. It should be
     * called after @ref platform_network_function_close.
     *
     * @param[in] pConnection The network connection to destroy, defined by
     * the network stack.
     *
     * @return Any #IotNetworkError_t, as defined by the network stack.
     *
     * @attention No function should be called on the network connection after
     * calling this function.
     */
    /* @[declare_platform_network_destroy] */
    IotNetworkError_t ( * destroy )( void * pConnection );
    /* @[declare_platform_network_destroy] */
} IotNetworkInterface_t;

#endif /* ifndef _IOT_NETWORK_H_ */
