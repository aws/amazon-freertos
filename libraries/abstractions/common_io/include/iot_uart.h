/*
 * Amazon FreeRTOS V1.2.3
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_uart.h
 * @brief File for the HAL APIs of UART called by application layer.
 */
#ifndef _IOT_UART_H_
#define _IOT_UART_H_

/* Standard includes. */
#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup iot_uart UART Abstraction APIs.
 * @{
 */

/**
 * @brief The default baud rate for a given UART port.
 */
#define IOT_UART_BAUD_RATE_DEFAULT         ( 115200 )

/**
 * @brief Error codes returned by every function module in UART HAL.
 */
#define IOT_UART_SUCCESS                   ( 0 )
#define IOT_UART_INVALID_VALUE             ( 1 )
#define IOT_UART_WRITE_FAILED              ( 2 )
#define IOT_UART_READ_FAILED               ( 3 )
#define IOT_UART_BUSY                      ( 4 )
#define IOT_UART_NOTHING_TO_CANCEL         ( 5 )
#define IOT_UART_FUNCTION_NOT_SUPPORTED    ( 6 )

/**
 * @brief UART read/write operation status values
 */
typedef enum
{
    eUartCompleted = IOT_UART_SUCCESS,
    eUartLastWriteFailed = IOT_UART_WRITE_FAILED,
    eUartLastReadFailed = IOT_UART_READ_FAILED,
} IotUARTOperationStatus_t;

/**
 * @brief UART callback.
 *
 * @param[out] xStatus      UART asynchronous operation status.
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used or modified by the driver. The context
 *                          is provided by the caller when setting the callback, and is
 *                          passed back to the caller in the callback.
 */
typedef void ( * IotUARTCallback_t )( IotUARTOperationStatus_t xStatus ,  void * pvUserContext );


/**
 * @brief The UART descriptor type defined in the source file.
 */
struct                       IotUARTDescriptor_t;

/**
 * @brief IotUARTHandle_t is the handle type returned by calling iot_uart_open().
 *        This is initialized in open and returned to caller. The caller must pass
 *        this pointer to the rest of APIs.
 */
typedef struct IotUARTDescriptor_t   * IotUARTHandle_t;

/**
 * @brief Ioctl requests for UART HAL.
 */
typedef enum
{
    eUartSetConfig,  /** Sets the UART configuration according to @IotUARTConfig_t. */
    eUartGetConfig,  /** Gets the UART configuration according to @IotUARTConfig_t. */
    eGetTxNoOfbytes, /** Get the number of bytes sent in write operation. */
    eGetRxNoOfbytes  /** Get the number of bytes received in read operation. */
} IotUARTIoctlRequest_t;

/**
 * @brief Configuration parameters for the UART.
 *
 * The application will send the user configuration in the form of the
 * following structure in ioctl.
 */
typedef struct
{
    uint32_t ulBaudrate;    /**< The baud rate to be set for the UART port. */
    uint32_t ulParity;      /**< The parity to be set for the UART port. */
    uint32_t ulWordlength;  /**< The word length to be set for the UART port. */
    uint32_t ulStopbits;    /**< The stop bits to be set for the UART port. */
    uint32_t ulFlowControl; /**< The flow control to be set for the UART port. */
} IotUARTConfig_t;

/**
 * @brief Initializes the UART peripheral of the board with default configuration.
 *
 * The application should call this function to initialize the desired UART port.
 *
 * @param[in] lUartInstance The instance of the UART port to initialize.
 *
 * @return handle to the UART if everything succeeds, otherwise NULL.
 */
IotUARTHandle_t iot_uart_open( int32_t lUartInstance );

/**
 * @brief Sets the application callback to be called on completion of an operation.
 *
 * On completion of receive or transmit on UART, the application is notified with a
 * function callback with the state of the transaction. The callback function and the parameters
 * for the state of the callback are set using iot_uart_set_callback.
 * The application should call this function with the callback function and its parameters.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[in] xCallback The callback function to be called on completion of transaction.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_uart_set_callback( IotUARTHandle_t const pxUartPeripheral,
                            IotUARTCallback_t xCallback,
                            void * pvUserContext );

/**
 * @brief Starts receiving the data from UART synchronously i.e. this is a
 * blocking call.
 *
 * The application should call this function to start synchronous read.
 * The application should also give the address of the buffer to read the data
 * into with number of bytes to read. If the number of bytes is not known,
 * the application reads one byte at a time.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[out] pvBuffer The buffer to store the received data.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return IOT_UART_SUCCESS on successful completion of synchronous read,
 *         else one of the IOT_UART_INVALID_VALUE, IOT_UART_BUSY, IOT_UART_READ_FAILED  on error.
 */
int32_t iot_uart_read_sync( IotUARTHandle_t const pxUartPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes );

/**
 * @brief Starts the transmission of data from UART synchronously i.e. this is a
 * blocking call.
 *
 * The application should call this function to start synchronous write.
 * The application should also give the address of the buffer to send the data
 * from with number of bytes to write.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[in] pvBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to send.
 *
 * @return IOT_UART_SUCCESS on successful completion of synchronous write,
 *         else one of the IOT_UART_INVALID_VALUE, IOT_UART_BUSY, IOT_UART_WRITE_FAILED on error.
 */
int32_t iot_uart_write_sync( IotUARTHandle_t const pxUartPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes );

/**
 * @brief Starts receiving the data from UART asynchronously.
 *
 * The application should call this function to start asynchronous read.
 * The application should also give the address of the buffer to read into with
 * number of bytes to read. If the number of bytes is not known, the application
 * reads one byte at a time.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[out] pvBuffer The buffer to store the received data.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return IOT_UART_SUCCESS on successful start of asynchronous read,
 *         else one of the IOT_UART_INVALID_VALUE, IOT_UART_BUSY on error.
 */
int32_t iot_uart_read_async( IotUARTHandle_t const pxUartPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes );

/**
 * @brief Starts the transmission of data from UART asynchronously.
 *
 * The application should call this function to start asynchronous write.
 * The application should also give the address of the buffer to send the data
 * from; with number of bytes to write.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[in] pvBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to send.
 *
 * @return IOT_UART_SUCCESS on successful start of asynchronous write,
 *         else one of the IOT_UART_INVALID_VALUE, IOT_UART_BUSY on error.
 */
int32_t iot_uart_write_async( IotUARTHandle_t const pxUartPeripheral,
                              uint8_t * const pvBuffer,
                              size_t xBytes );

/**
 * @brief Configures the UART port with user configuration.
 *
 * The application should call this function to configure the UART port
 * with values in the structure IotUARTConfig_t. The application also
 * needs to specify the request which is as per the enum IotUARTIoctlRequest_t.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[in] xUartRequest The configuration request. Should be one of the values
 * from IotUARTIoctlRequest_t.
 * @param[in,out] pvBuffer The configuration values for the UART port.
 *
 * @return IOT_UART_SUCCESS on successful configuartion of UART port,
 *         else one of the IOT_UART_INVALID_VALUE, IOT_UART_BUSY,
 *         IOT_UART_FUNCTION_NOT_SUPPORTED on error.
 */
int32_t iot_uart_ioctl( IotUARTHandle_t const pxUartPeripheral,
                        IotUARTIoctlRequest_t xUartRequest,
                        void * const pvBuffer );

/**
 * @brief Aborts the transaction on the UART port if any underlying driver allows
 * cancellation of the transaction.
 *
 * The application should call this function to stop the ongoing transaction.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 *
 * @return IOT_UART_SUCCESS on successful cancellation of the operation,
 *         else one of IOT_UART_INVALID_VALUE or
 *         or IOT_UART_FUNCTION_NOT_SUPPORTED, IOT_UART_NOTHING_TO_CANCEL
 */
int32_t iot_uart_cancel( IotUARTHandle_t const pxUartPeripheral );

/**
 * @brief Stops the transmission and de-initializes the UART peripheral.
 *
 * The application should call this function to stop the ongoing transaction
 * and de-initialize the UART port.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 *
 * @return IOT_UART_SUCCESS on stop and deinitialization,
 *         IOT_UART_INVALID_VALUE on error.
 */
int32_t iot_uart_close( IotUARTHandle_t const pxUartPeripheral );


/**
 * @}
 */
/* end of group iot_uart */
#endif /* _IOT_UART_H_ */
