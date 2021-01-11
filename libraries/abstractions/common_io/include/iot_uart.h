/*
 * FreeRTOS Common IO V0.1.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @defgroup IOT_UART UART Abstraction APIs.
 * @{
 */

/**
 * @brief The default baud rate for a given UART port.
 */
#define IOT_UART_BAUD_RATE_DEFAULT         ( 115200 )

/**
 * @brief Error codes returned by every function module in UART HAL.
 */
#define IOT_UART_SUCCESS                   ( 0 ) /*!< UART operation completed successfully. */
#define IOT_UART_INVALID_VALUE             ( 1 ) /*!< At least one parameter is invalid. */
#define IOT_UART_WRITE_FAILED              ( 2 ) /*!< UART driver returns error when performing write operation. */
#define IOT_UART_READ_FAILED               ( 3 ) /*!< UART driver returns error when performing read operation. */
#define IOT_UART_BUSY                      ( 4 ) /*!< UART bus is busy at current time. */
#define IOT_UART_NOTHING_TO_CANCEL         ( 5 ) /*!< No ongoing operation when cancel is performed. */
#define IOT_UART_FUNCTION_NOT_SUPPORTED    ( 6 ) /*!< UART operation is not supported. */

/**
 * @brief UART read/write operation status values
 */
typedef enum
{
    eUartWriteCompleted,  /*!< UART operation write completed successfully. */
    eUartReadCompleted,   /*!< UART operation read completed successfully. */
    eUartLastWriteFailed, /*!< UART driver returns error when performing write operation. */
    eUartLastReadFailed,  /*!< UART driver returns error when performing read operation. */
} IotUARTOperationStatus_t;

/**
 * @brief UART parity mode
 */
typedef enum
{
    eUartParityNone, /*!< UART parity as none. */
    eUartParityOdd,  /*!< UART parity as odd. */
    eUartParityEven, /*!< UART parity as even. */
} IotUARTParity_t;

/**
 * @brief UART stop bits
 */
typedef enum
{
    eUartStopBitsOne, /*!< UART character stop is one bit. */
    eUartStopBitsTwo, /*!< UART character stop is two bits. */
} IotUARTStopBits_t;

/**
 * @brief The callback function for completion of UART operation.
 *
 * @param[out] xStatus      UART asynchronous operation status.
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used or modified by the driver. The context
 *                          is provided by the caller when setting the callback, and is
 *                          passed back to the caller in the callback.
 */
typedef void ( * IotUARTCallback_t )( IotUARTOperationStatus_t xStatus,
                                      void * pvUserContext );


/**
 * @brief The UART descriptor type defined in the source file.
 */
struct                       IotUARTDescriptor;

/**
 * @brief IotUARTHandle_t is the handle type returned by calling iot_uart_open().
 *        This is initialized in open and returned to caller. The caller must pass
 *        this pointer to the rest of APIs.
 */
typedef struct IotUARTDescriptor * IotUARTHandle_t;

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
    uint32_t ulBaudrate;         /**< The baud rate to be set for the UART port. */
    IotUARTParity_t xParity;     /**< The parity to be set for the UART port, defined in IotUARTParity_t. */
    IotUARTStopBits_t xStopbits; /**< The stop bits to be set for the UART port, defined in IotUARTStopBits_t. */
    uint8_t ucWordlength;        /**< The word length to be set for the UART port. */
    uint8_t ucFlowControl;       /**< The flow control to be set for the UART port: 0 is disabled and 1 is enabled. */
} IotUARTConfig_t;

/**
 * @brief Initializes the UART peripheral of the board.
 *
 * The application should call this function to initialize the desired UART port.
 *
 * @warning Once opened, the same UART instance must be closed before calling open again.
 *
 * @param[in] lUartInstance The instance of the UART port to initialize.
 *
 * @return
 * - 'the handle to the UART port (not NULL)', on success.
 * - 'NULL', if
 *     - invalid instance number
 *     - open same instance more than once before closing it
 *
 * <b>Example</b>
 * @code{c}
 *  // These two buffers can contain 32 bytes for reading and writing.
 *  uint8_t cpBuffer[ 32 ] = { 0 };
 *  uint8_t cpBufferRead[ 32 ] = { 0 };
 *
 *  IotUARTHandle_t xOpen;
 *  int32_t lRead, lWrite, lClose;
 *  BaseType_t xCallbackReturn;
 *  uint8_t ucPort = 1; // Each UART peripheral will be assigned an integer.
 *
 *  xOpen = iot_uart_open( ucPort );
 *  if( xOpen != NULL )
 *  {
 *      iot_uart_set_callback( xOpen, prvReadWriteCallback, NULL );
 *
 *      lWrite = iot_uart_write_async( xOpen, cpBuffer, testIotUART_READ_BUFFER_LENGTH );
 *      // assert(IOT_UART_SUCCESS, lWrite)
 *      // Wait for asynchronous write to complete
 *      xCallbackReturn = xSemaphoreTake( ( SemaphoreHandle_t ) &xUartSemaphore, IotUART_DEFAULT_SEMPAHORE_DELAY );
 *      // assert(xCallbackReturn, pdTrue)
 *
 *      lRead = iot_uart_read_async( xOpen, cpBufferRead, testIotUART_READ_BUFFER_LENGTH );
 *      // assert(IOT_UART_SUCCESS, lRead)
 *
 *      // Wait for asynchronous read to complete
 *      xCallbackReturn = xSemaphoreTake( ( SemaphoreHandle_t ) &xUartSemaphore, IotUART_DEFAULT_SEMPAHORE_DELAY );
 *      // assert(xCallbackReturn, pdTrue)
 *  }
 *
 *  lClose = iot_uart_close( xOpen );
 *  // assert(IOT_UART_SUCCESS, lClose)
 *  @endcode
 */
IotUARTHandle_t iot_uart_open( int32_t lUartInstance );

/**
 * @brief Sets the application callback to be called on completion of an operation.
 *
 * The callback is guaranteed to be invoked when the current asynchronous operation completes, either successful or failed.
 * This simply provides a notification mechanism to user's application. It has no impact if the callback is not set.
 *
 * @note This callback will not be invoked when synchronous operation completes.
 * @note This callback is per handle. Each instance has its own callback.
 * @note Single callback is used for both read_async and write_async. Newly set callback overrides the one previously set.
 * @warning If the input handle is invalid, this function silently takes no action.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[in] xCallback The callback function to be called on completion of transaction (This can be NULL).
 * @param[in] pvUserContext The user context to be passed back when callback is called (This can be NULL).
 */
void iot_uart_set_callback( IotUARTHandle_t const pxUartPeripheral,
                            IotUARTCallback_t xCallback,
                            void * pvUserContext );

/**
 * @brief Starts receiving the data from UART synchronously.
 *
 * This function attempts to read certain number of bytes from transmitter device to a pre-allocated buffer, in synchronous way.
 * Partial read might happen, e.g. no more data is available.
 * And the number of bytes that have been actually read can be obtained by calling iot_uart_ioctl.
 *
 * @note If the number of bytes is not known, it is recommended that the application reads one byte at a time.
 *
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[out] pvBuffer The buffer to store the received data.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return IOT_UART_SUCCESS on successful completion of synchronous read,
 *         else one of the IOT_UART_INVALID_VALUE, IOT_UART_BUSY, IOT_UART_READ_FAILED  on error.
 * @return
 * - IOT_UART_SUCCESS, on success (all the requested bytes have been read)
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_UART_READ_FAILED, if there is unknown driver error
 * - IOT_UART_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_uart_read_sync( IotUARTHandle_t const pxUartPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes );

/**
 * @brief Starts the transmission of data from UART synchronously.
 *
 * This function attempts to write certain number of bytes from a pre-allocated buffer to a receiver device, in synchronous way.
 * Partial write might happen, e.g. receiver device unable to receive more data.
 * And the number of bytes that have been actually written can be obtained by calling iot_uart_ioctl.
 *
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[in] pvBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to send.
 *
 * @return
 * - IOT_UART_SUCCESS, on success (all the requested bytes have been write)
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_UART_WRITE_FAILED, if there is unknown driver error
 * - IOT_UART_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_uart_write_sync( IotUARTHandle_t const pxUartPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes );

/**
 * @brief Starts receiving the data from UART asynchronously.
 *
 * This function attempts to read certain number of bytes from a pre-allocated buffer, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_uart_ioctl.
 * Once the operation completes, successful or not, the user callback will be invoked.
 *
 * Partial read might happen.
 * And the number of bytes that have been actually read can be obtained by calling iot_uart_ioctl.
 *
 * @note In order to get notification when the asynchronous call is completed, iot_uart_set_callback must be called prior to this.
 * @warning pucBuffer must be valid before callback is invoked.
 * @warning None of other read or write functions shall be called during this function or before user callback.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[out] pvBuffer The buffer to store the received data.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 * - IOT_UART_SUCCESS, on success
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_UART_READ_FAILED, if there is unknown driver error
 */
int32_t iot_uart_read_async( IotUARTHandle_t const pxUartPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes );

/**
 * @brief Starts the transmission of data from UART asynchronously.
 *
 * This function attempts to write certain number of bytes from a pre-allocated buffer to a receiver device, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_uart_ioctl.
 * Once the operation completes, successful or not, the user callback will be invoked.
 *
 * Partial write might happen.
 * And the number of bytes that have been actually written can be obtained by calling iot_uart_ioctl.
 *
 * @note In order to get notification when the asynchronous call is completed, iot_uart_set_callback must be called prior to this.
 *
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[in] pvBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to send.
 *
 * @return
 * - IOT_UART_SUCCESS, on success
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_UART_WRITE_FAILED, if there is unknown driver error
 */
int32_t iot_uart_write_async( IotUARTHandle_t const pxUartPeripheral,
                              uint8_t * const pvBuffer,
                              size_t xBytes );

/**
 * @brief Configures the UART port with user configuration.
 *
 *
 * @note eUartSetConfig sets the UART configuration.
 * This request expects the buffer with size of IotUARTConfig_t.
 *
 * @note eUartGetConfig gets the current UART configuration.
 * This request expects the buffer with size of IotUARTConfig_t.
 *
 * @note eGetTxNoOfbytes returns the number of written bytes in last operation.
 * This is supposed to be called in the caller task or application callback, right after last operation completes.
 * This request expects 2 bytes buffer (uint16_t).
 *
 * - If the last operation was write, this returns the actual number of written bytes which might be smaller than the requested number (partial write).
 * - If the last operation was read, this returns 0.
 *
 * @note eGetRxNoOfbytes returns the number of read bytes in last operation.
 * This is supposed to be called in the caller task or application callback, right after last operation completes.
 * This request expects 2 bytes buffer (uint16_t).
 *
 * - If the last operation was read, this returns the actual number of read bytes which might be smaller than the requested number (partial read).
 * - If the last operation was write, this returns 0.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 * @param[in] xUartRequest The configuration request. Should be one of the values
 * from IotUARTIoctlRequest_t.
 * @param[in,out] pvBuffer The configuration values for the UART port.
 *
 * @return IOT_UART_SUCCESS on successful configuartion of UART port,
 *         else one of the IOT_UART_INVALID_VALUE, IOT_UART_BUSY,
 *         IOT_UART_FUNCTION_NOT_SUPPORTED on error.
 * @return
 * - IOT_UART_SUCCESS, on success
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 *     - pucBuffer is NULL with requests which needs buffer
 * - IOT_UART_FUNCTION_NOT_SUPPORTED, if this board doesn't support this feature.
 *     - eUartSetConfig: specific configuration is not supported
 */
int32_t iot_uart_ioctl( IotUARTHandle_t const pxUartPeripheral,
                        IotUARTIoctlRequest_t xUartRequest,
                        void * const pvBuffer );

/**
 * @brief Aborts the operation on the UART port if any underlying driver allows
 * cancellation of the operation.
 *
 * The application should call this function to stop the ongoing operation.
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 *
 * @return
 * - IOT_UART_SUCCESS, on success
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 * - IOT_UART_NOTHING_TO_CANCEL, if there is no on-going transaction.
 * - IOT_UART_FUNCTION_NOT_SUPPORTED, if this board doesn't support this operation.
 */
int32_t iot_uart_cancel( IotUARTHandle_t const pxUartPeripheral );

/**
 * @brief Stops the operation and de-initializes the UART peripheral.
 *
 *
 * @param[in] pxUartPeripheral The peripheral handle returned in the open() call.
 *
 * @return
 * - IOT_UART_SUCCESS, on success
 * - IOT_UART_INVALID_VALUE, if
 *     - pxUartPeripheral is NULL
 *     - pxUartPeripheral is not opened yet
 */
int32_t iot_uart_close( IotUARTHandle_t const pxUartPeripheral );


/**
 * @}
 */
/* end of group IOT_UART */
#endif /* _IOT_UART_H_ */
