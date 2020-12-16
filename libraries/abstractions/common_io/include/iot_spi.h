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
 * @file iot_spi.h
 * @brief File for the APIs of SPI called by application layer.
 */
#ifndef _IOT_SPI_H_
#define _IOT_SPI_H_

/* Standard includes. */
#include <stdint.h>

/**
 * @defgroup iot_spi SPI Abstraction APIs.
 * @{
 */

/**
 * @brief The return codes for the methods in SPI.
 */
#define IOT_SPI_SUCCESS                   ( 0 ) /*!< SPI operation completed successfully. */
#define IOT_SPI_INVALID_VALUE             ( 1 ) /*!< At least one parameter is invalid. */
#define IOT_SPI_WRITE_FAILED              ( 2 ) /*!< SPI driver returns error when performing write operation. */
#define IOT_SPI_READ_FAILED               ( 3 ) /*!< SPI driver returns error when performing read operation. */
#define IOT_SPI_TRANSFER_ERROR            ( 4 ) /*!< SPI driver returns error when performing transfer. */
#define IOT_SPI_BUS_BUSY                  ( 5 ) /*!< SPI bus is busy at current time. */
#define IOT_SPI_NOTHING_TO_CANCEL         ( 6 ) /*!< No ongoing operation when cancel operation is performed. */
#define IOT_SPI_FUNCTION_NOT_SUPPORTED    ( 7 ) /*!< SPI operation is not supported. */


/**
 * @brief The SPI return status from Async operations.
 */
typedef enum
{
    eSPISuccess = IOT_SPI_SUCCESS,              /*!< SPI operation completed successfully. */
    eSPIWriteError = IOT_SPI_WRITE_FAILED,      /*!< SPI driver returns error when performing write operation. */
    eSPIReadError = IOT_SPI_READ_FAILED,        /*!< SPI driver returns error when performing read operation. */
    eSPITransferError = IOT_SPI_TRANSFER_ERROR, /*!< SPI driver returns error when performing transfer. */
} IotSPITransactionStatus_t;

/**
 * @brief The SPI Modes denoting the clock polarity
 * and clock phase.
 */
typedef enum
{
    eSPIMode0, /*!< CPOL = 0 and CPHA = 0 */
    eSPIMode1, /*!< CPOL = 0 and CPHA = 1 */
    eSPIMode2, /*!< CPOL = 1 and CPHA = 0 */
    eSPIMode3, /*!<CPOL = 1 and CPHA = 1 */
} IotSPIMode_t;

/**
 * @brief The bit order of the data transmission.
 */
typedef enum
{
    eSPIMSBFirst, /*!< The master sends the most-significant bit (MSB) first */
    eSPILSBFirst  /*!< The master sends the least-significant bit (LSB) first */
} IotSPIBitOrder_t;

/**
 * @brief Ioctl request for SPI HAL.
 */
typedef enum
{
    eSPISetMasterConfig, /*!< Sets the configuration of the SPI master and the data type is IotSPIMasterConfig_t. */
    eSPIGetMasterConfig, /*!< Gets the configuration of the SPI master and the data type is IotSPIMasterConfig_t. */
    eSPIGetTxNoOfbytes,  /*!< Get the number of bytes sent in write operation and the data type is uint16_t. */
    eSPIGetRxNoOfbytes,  /*!< Get the number of bytes received in read operation and the data type is uint16_t. */
} IotSPIIoctlRequest_t;

/**
 * @brief The configuration parameters for SPI Master.
 *
 * @details The application will set the SPI master interface using the Ioctl
 * eSPISetMasterConfig and send this structure.
 */
typedef struct IotSPIMasterConfig
{
    uint32_t ulFreq;               /*!< SPI frequency set for data transmission in Hz. */
    IotSPIMode_t eMode;            /*!< Mode selected as per enum IotSPIMode_t. */
    IotSPIBitOrder_t eSetBitOrder; /*!< Bit Order selected as per enum IotSPIBitOrder_t. */
    uint8_t ucDummyValue;          /*!< The dummy value sent by master for SPI Read. */
} IotSPIMasterConfig_t;

/**
 * @brief The SPI descriptor type defined in the source file.
 */
struct                      IotSPIDescriptor;

/**
 * @brief IotSPIHandle_t is the handle type returned by calling iot_spi_open().
 *        This is initialized in open and returned to caller. The caller must pass
 *        this pointer to the rest of APIs.
 */
typedef struct IotSPIDescriptor * IotSPIHandle_t;

/**
 * @brief The callback function for completion of SPI operation.
 */
typedef void (* IotSPICallback_t) ( IotSPITransactionStatus_t xStatus,
                                    void * pvSPIparam );

/**
 * @brief Initializes SPI peripheral with default configuration.
 *
 * @warning Once opened, the same SPI instance must be closed before calling open again.
 *
 * @param[in] lSPIInstance The instance of the SPI driver to initialize.
 *
 * @return
 * - 'the handle to the SPI port (not NULL)', on success.
 * - 'NULL', if
 *     - invalid instance number
 *     - open same instance more than once before closing it
 */
IotSPIHandle_t iot_spi_open( int32_t lSPIInstance );

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
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in the open() call.
 * @param[in] xCallback The callback function to be called on completion of operation.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_spi_set_callback( IotSPIHandle_t const pxSPIPeripheral,
                           IotSPICallback_t xCallback,
                           void * pvUserContext );

/**
 * @brief Configures the SPI port with user configuration.
 *
 *
 * @note eSPISetMasterConfig sets the configurations for master.
 * This request expects the buffer with size of IotSPIMasterConfig_t.
 *
 * @note eSPIGetMasterConfig gets the current configuration for SPI master.
 * This request expects the buffer with size of IotSPIMasterConfig_t.
 *
 * @note eSPIGetTxNoOfbytes returns the number of written bytes in last operation.
 * This is supposed to be called in the caller task or application callback, right after last operation completes.
 * This request expects 2 bytes buffer (uint16_t).
 *
 * - If the last operation only did write, this returns the actual number of written bytes which might be smaller than the requested number (partial write).
 * - If the last operation only did read, this returns 0.
 * - If the last operation did both write and read, this returns the number of written bytes.
 *
 * @note eSPIGetRxNoOfbytes returns the number of read bytes in last operation.
 * This is supposed to be called in the caller task or application callback, right after last operation completes.
 * This request expects 2 bytes buffer (uint16_t).
 *
 * - If the last operation only did read, this returns the actual number of read bytes which might be smaller than the requested number (partial read).
 * - If the last operation only did write, this returns 0.
 * - If the last operation did both write and read, this returns the number of read bytes.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] xSPIRequest The configuration request from one of the
 * IotSPIIoctlRequest_t.
 * @param[in,out] pvBuffer The configuration values for the SPI port.
 *
 *
 * @return
 * - IOT_SPI_SUCCESS, on success
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL with requests which needs buffer
 * - IOT_SPI_BUS_BUSY, if the bus is busy for only following requests:
 *     - eSPISetMasterConfig
 */
int32_t iot_spi_ioctl( IotSPIHandle_t const pxSPIPeripheral,
                       IotSPIIoctlRequest_t xSPIRequest,
                       void * const pvBuffer );

/**
 * @brief The SPI master starts reading from the slave synchronously.
 *
 * This function attempts to read certain number of bytes from slave device to a pre-allocated buffer, in synchronous way.
 * This function does not return on paritial read, unless there is an error.
 * And the number of bytes that have been actually read can be obtained by calling iot_spi_ioctl.
 *
 * @note Dummy data will be written to slave while reading. The dummy data value can be configured with iot_spi_ioctl.
 *
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[out] pvBuffer The buffer to store the received data.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success (all the requested bytes have been read)
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_SPI_READ_ERROR, if there is some unknown driver error.
 * - IOT_SPI_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_spi_read_sync( IotSPIHandle_t const pxSPIPeripheral,
                           uint8_t * const pvBuffer,
                           size_t xBytes );

/**
 * @brief The SPI master starts reading from the slave asynchronously.
 *
 * This function attempts to read certain number of bytes from a pre-allocated buffer, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_spi_ioctl.
 *
 * Once the operation completes successfully, the user callback will be invoked.
 * If the operation encounters an error, the user callback will be invoked.
 * The callback is not invoked on paritial read, unless there is an error.
 * And the number of bytes that have been actually read can be obtained by calling iot_spi_ioctl.
 *
 * @note Dummy data will be written to slave while reading. The dummy data value can be configured with iot_spi_ioctl.
 * @note In order to get notification when the asynchronous call is completed, iot_spi_set_callback must be called prior to this.
 *
 * @warning pvBuffer must be valid before callback is invoked.
 * @warning None of other read or write functions shall be called during this function or before user callback.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[out] pvBuffer The buffer to store the received data.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success (all the requested bytes have been read)
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_SPI_READ_ERROR, if there is some unknown driver error.
 * - IOT_SPI_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_spi_read_async( IotSPIHandle_t const pxSPIPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes );

/**
 * @brief The SPI master starts transmission of data to the slave synchronously.
 *
 * This function attempts to write certain number of bytes from a pre-allocated buffer to a slave device, in synchronous way.
 * This function does not return on paritial write, unless there is an error.
 * And the number of bytes that have been actually written can be obtained by calling iot_spi_ioctl.
 *
 *
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] pvBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success (all the requested bytes have been read)
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_SPI_WRITE_ERROR, if there is some unknown driver error.
 * - IOT_SPI_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_spi_write_sync( IotSPIHandle_t const pxSPIPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes );

/**
 * @brief The SPI master starts transmission of data to the slave asynchronously.
 *
 * This function attempts to read certain number of bytes from a pre-allocated buffer, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_spi_ioctl.
 *
 * Once the operation completes successfully, the user callback will be invoked.
 * If the operation encounters an error, the user callback will be invoked.
 * The callback is not invoked on paritial write, unless there is an error.
 * And the number of bytes that have been actually written can be obtained by calling iot_spi_ioctl.
 *
 * @note In order to get notification when the asynchronous call is completed, iot_spi_set_callback must be called prior to this.
 *
 * @warning pvBuffer must be valid before callback is invoked.
 * @warning None of other read or write functions shall be called during this function or before user callback.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] pvBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success (all the requested bytes have been read)
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_SPI_WRITE_ERROR, if there is some unknown driver error.
 * - IOT_SPI_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_spi_write_async( IotSPIHandle_t const pxSPIPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes );

/**
 * @brief The SPI master starts a synchronous transfer between master and the slave.
 *
 * This function attempts to read/write certain number of bytes from/to two pre-allocated buffers at the same time, in synchronous way.
 * This function does not return on paritial read/write, unless there is an error.
 * And the number of bytes that have been actually read or written can be obtained by calling iot_spi_ioctl.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] pvTxBuffer The buffer to store the received data.
 * @param[out] pvRxBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to transfer.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success (all the requested bytes have been read/written)
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_SPI_TRANSFER_ERROR, if there is some unknown driver error.
 * - IOT_SPI_BUSY, if the bus is busy which means there is an ongoing operation.
 */
int32_t iot_spi_transfer_sync( IotSPIHandle_t const pxSPIPeripheral,
                               uint8_t * const pvTxBuffer,
                               uint8_t * const pvRxBuffer,
                               size_t xBytes );

/**
 * @brief The SPI master starts a asynchronous transfer between master and the slave.
 *
 * This function attempts to read/write certain number of bytes from/to two pre-allocated buffers at the same time, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_spi_ioctl.
 *
 * Once the operation completes successfully, the user callback will be invoked.
 * If the operation encounters an error, the user callback will be invoked.
 * The callback is not invoked on paritial read/write, unless there is an error.
 * And the number of bytes that have been actually read/write can be obtained by calling iot_spi_ioctl.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] pvTxBuffer The buffer to store the received data.
 * @param[out] pvRxBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to transfer.
 *
 * - IOT_SPI_SUCCESS, on success (all the requested bytes have been read/written)
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_SPI_TRANSFER_ERROR, if there is some unknown driver error.
 * - IOT_SPI_BUSY, if the bus is busy which means there is an ongoing opeartion.
 */
int32_t iot_spi_transfer_async( IotSPIHandle_t const pxSPIPeripheral,
                                uint8_t * const pvTxBuffer,
                                uint8_t * const pvRxBuffer,
                                size_t xBytes );

/**
 * @brief Stops the ongoing operation on SPI bus and de-initializes the SPI peripheral.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 */
int32_t iot_spi_close( IotSPIHandle_t const pxSPIPeripheral );

/**
 * @brief This function is used to cancel the current operation in progress, if the underlying driver
 * allows the cancellation.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success
 * - IOT_SPI_INVALID_VALUE, if
 *     - pxSPIPeripheral is NULL
 *     - pxSPIPeripheral is not opened yet
 * - IOT_SPI_NOTHING_TO_CANCEL, if there is no on-going operation.
 * - IOT_SPI_FUNCTION_NOT_SUPPORTED, if this board doesn't support this operation.
 */
int32_t iot_spi_cancel( IotSPIHandle_t const pxSPIPeripheral );

/**
 * @brief This function is used to select spi slave.
 *
 * @param[in] lSPIInstance The instance of the SPI driver to initialize.
 * @param[in] lSPISlave Slave select number.
 *
 * @return
 * - IOT_SPI_SUCCESS, on success
 * - IOT_SPI_INVALID_VALUE, if
 *     - lSPISlave is invalid
 */
int32_t iot_spi_select_slave( int32_t lSPIInstance,
                              int32_t lSPISlave );

/**
 * @}
 */
/* end of group iot_spi */

#endif /* _IOT_SPI_H_ */
