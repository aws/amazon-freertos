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
 * @file iot_spi.h
 * @brief File for the APIs of SPI called by application layer.
 */
#ifndef _IOT_SPI_H_
#define _IOT_SPI_H_

/**
 * @defgroup iot_spi SPI Abstraction APIs.
 * @{
 */

/**
 * @brief The return codes for the methods in SPI.
 */
#define IOT_SPI_SUCCESS                   ( 0 )
#define IOT_SPI_INVALID_VALUE             ( 1 )
#define IOT_SPI_MASTER_NOT_INITIALIZED    ( 2 )
#define IOT_SPI_BUS_BUSY                  ( 3 )
#define IOT_SPI_SLAVE_NOT_SELECTED        ( 4 )
#define IOT_SPI_NOTHING_TO_CANCEL         ( 6 )
#define IOT_SPI_FUNCTION_NOT_SUPPORTED    ( 7 )
#define IOT_SPI_TRANSFER_ERROR            ( 8 )

/**
 * @brief The SPI return status from Async transactions
 */
typedef enum
{
    eSPISuccess               = IOT_SPI_SUCCESS,
    eSPIInvalidValue          = IOT_SPI_INVALID_VALUE,
    eSPIMasterNotInitialized  = IOT_SPI_MASTER_NOT_INITIALIZED,
    eSPIBusBusy               = IOT_SPI_BUS_BUSY,
    eSPISlaveNotSelected      = IOT_SPI_SLAVE_NOT_SELECTED,
    eSPITransferError         = IOT_SPI_TRANSFER_ERROR,
} IotSPITransactionStatus_t;

/**
 * @brief The SPI Modes denoting the clock polarity
 * and clock phase.
 */
typedef enum
{
    eSPIMode0, /**< CPOL = 0 and CPHA = 0 */
    eSPIMode1, /**< CPOL = 0 and CPHA = 1 */
    eSPIMode2, /**< CPOL = 1 and CPHA = 0 */
    eSPIMode3, /**< CPOL = 1 and CPHA = 1 */
} IotSPIMode_t;

/**
 * @brief The bit order of the data transmission.
 */
typedef enum
{
    eSPIMSBFirst, /**< The master sends the most-significant bit (MSB) first */
    eSPILSBFirst  /**< The master sends the least-significant bit (LSB) first */
} IotSPIBitOrder_t;

/**
 * @brief Ioctl request for SPI HAL.
 */
typedef enum
{
    eSPISetMasterConfig,  /** Sets the configuration of the SPI master. */
    eSPIGetMasterConfig,  /** Gets the configuration of the SPI master. */
    eSPIGetTxNoOfbytes,   /** Get the number of bytes sent in write operation. */
    eSPIGetRxNoOfbytes,   /** Get the number of bytes received in read operation. */
} IotSPIIoctlRequest_t;

/**
 * @brief The configuration parameters for SPI Master.
 *
 * @details The application will set the SPI master interface using the Ioctl
 * eSPISetMasterConfig and send this structure.
 */
typedef struct IotSPIMasterConfig
{
    uint32_t ulFreq;                /**< SPI frequency set for data transmission. */
    IotSPIMode_t eMode;             /**< Mode selected as per enum IotSPIMode_t. */
    IotSPIBitOrder_t eSetBitOrder;  /**< Bit Order selected as per enum IotSPIBitOrder_t. */
    uint8_t ucDummyValue;           /**< The dummy value sent by master for SPI Read. */
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
typedef struct IotSPIDescriptor   * IotSPIHandle_t;

/**
 * @brief The callback function for completion of SPI operation.
 */
typedef void (* IotSPICallback_t) ( IotSPITransactionStatus_t xStatus, void * pvSPIparam );

/**
 * @brief Initializes SPI peripheral with default configuration.
 *
 * The application should call this function to initialize the desired SPI port.
 *
 * @param[in] lSPIInstance The instance of the SPI driver to initialize.
 *
 * @return handle to the SPI if everything succeeds else NULL.
 */
IotSPIHandle_t iot_spi_open( int32_t lSPIInstance );

/**
 * @brief Sets the application callback to be called on completion of an operation.
 *
 * On completion of asynchronous SPI transfer, the application is notified with
 * a function callback. The callback function and the parameters are set using
 * iot_spi_set_callback.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in the open() call.
 * @param[in] xCallback The callback function to be called on completion of transaction.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_spi_set_callback( IotSPIHandle_t const pxSPIPeripheral,
                           IotSPICallback_t xCallback,
                           void * pvUserContext );

/**
 * @brief Configures the SPI port with user configuration.
 *
 * The application should call this function to configure the SPI driver
 * with values in the structure IotSPIMasterConfig_t. The application also needs to
 * specify the request which is as per enum IotSPIIoctlRequest_t.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] xSPIRequest The configuration request from one of the
 * IotSPIIoctlRequest_t.
 * @param[in,out] pvBuffer The configuration values for the SPI port.
 *
 *
 * @return SUCCESS = IOT_SPI_SUCESS,
 *         FAILED = IOT_SPI_FUNCTION_NOT_SUPPORTED,
 *         SPI_BUSY = IOT_SPI_BUS_BUSY,
 *          Master_UNINITIALIZED = IOT_SPI_MASTER_NOT_INITIALIZED
 */
int32_t iot_spi_ioctl( IotSPIHandle_t const pxSPIPeripheral,
                       IotSPIIoctlRequest_t xSPIRequest,
                       void * const pvBuffer );

/**
 * @brief The SPI master starts reading the number of bytes from the slave
 * synchronously.
 *
 * The application should call this function to start synchronous read.
 * The application should also give the address of the buffer to read the data
 * into; with number of bytes to read.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[out] pvBuffer The buffer to store the received data.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return SUCCESS = IOT_SPI_SUCCESS,
 *          FAILED = IOT_SPI_INVALID_VALUE,
 *          SPI_BUSY = IOT_SPI_BUS_BUSY,
 *          Master_UNINITIALIZED = IOT_SPI_MASTER_NOT_INITIALIZED
 *          Slave not selected = IOT_SPI_SLAVE_NOT_SELECTED
 */
int32_t iot_spi_read_sync( IotSPIHandle_t const pxSPIPeripheral,
                           uint8_t * const pvBuffer,
                           size_t xBytes );

/**
 * @brief The SPI master starts reading the number of bytes from the slave
 * asynchronously.
 *
 * The application should call this function to start synchronous read.
 * The application should also give the address of the buffer to read the data
 * into; with number of bytes to read.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[out] pvBuffer The buffer to store the received data.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return SUCCESS = IOT_SPI_SUCCESS,
 *          FAILED = IOT_SPI_INVALID_VALUE,
 *          SPI_BUSY = IOT_SPI_BUS_BUSY,
 *          Master_UNINITIALIZED = IOT_SPI_MASTER_NOT_INITIALIZED
 *          Slave not selected = IOT_SPI_SLAVE_NOT_SELECTED
 */
int32_t iot_spi_read_async( IotSPIHandle_t const pxSPIPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes );

/**
 * @brief The SPI master starts transmission of data to the slave
 * synchronously.
 *
 * The application should call this function to start synchronous write.
 * The application should also give the address of the buffer to send the data
 * from; with number of bytes to write.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] pvBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return SUCCESS = IOT_SPI_SUCCESS,
 *          FAILED = IOT_SPI_INVALID_VALUE,
 *          SPI_BUSY = IOT_SPI_BUS_BUSY,
 *          Master_UNINITIALIZED = IOT_SPI_MASTER_NOT_INITIALIZED
 *          Slave not selected = IOT_SPI_SLAVE_NOT_SELECTED
 */
int32_t iot_spi_write_sync( IotSPIHandle_t const pxSPIPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes );

/**
 * @brief The SPI master starts transmission of data to the slave
 * asynchronously.
 *
 * The application should call this function to start asynchronous write.
 * The application should also give the address of the buffer to send the data
 * from; with number of bytes to write.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] pvBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return SUCCESS = IOT_SPI_SUCCESS,
 *          FAILED = IOT_SPI_INVALID_VALUE,
 *          SPI_BUSY = IOT_SPI_BUS_BUSY,
 *          Master_UNINITIALIZED = IOT_SPI_MASTER_NOT_INITIALIZED
 *          Slave not selected = IOT_SPI_SLAVE_NOT_SELECTED
 */
int32_t iot_spi_write_async( IotSPIHandle_t const pxSPIPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes );

/**
 * @brief The SPI master starts a synchronous transfer between master and the
 * slave.
 *
 * The application should call this function to start the transfer with number
 * of bytes specified. The SPI driver fills in xBytes in RxBuffer and reads
 * xBytes in TxBuffer.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] pvTxBuffer The buffer to store the received data.
 * @param[out] pvRxBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to transfer.
 *
 * @return SUCCESS = IOT_SPI_SUCCESS,
 *          FAILED = IOT_SPI_INVALID_VALUE,
 *          SPI_BUSY = IOT_SPI_BUS_BUSY,
 *          MASTER_UNINITALIZED = IOT_SPI_MASTER_NOT_INITIALIZED,
 *          Slave not Selected = IOT_SPI_SLAVE_NOT_SELECTED
 */
int32_t iot_spi_transfer_sync( IotSPIHandle_t const pxSPIPeripheral,
                               uint8_t * const pvTxBuffer,
                               uint8_t * const pvRxBuffer,
                               size_t xBytes );

/**
 * @brief The SPI master starts a asynchronous transfer between master and the
 * slave.
 *
 * The application should call this function to start the asynchronous transfer
 * with number of bytes specified. The SPI driver fills in xBytes in RxBuffer
 * and reads xBytes in TxBuffer when the SPI bus is free.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 * @param[in] pvTxBuffer The buffer to store the received data.
 * @param[out] pvRxBuffer The buffer with data to transmit.
 * @param[in] xBytes The number of bytes to transfer.
 *
 * @return SUCCESS = IOT_SPI_SUCCESS,
 *          FAILED = IOT_SPI_INVALID_VALUE,
 *          SPI_BUSY = IOT_SPI_BUS_BUSY,
 *          MASTER_UNINITALIZED = IOT_SPI_MASTER_NOT_INITIALIZED,
 *          Slave not Selected = IOT_SPI_SLAVE_NOT_SELECTED
 */
int32_t iot_spi_transfer_async( IotSPIHandle_t const pxSPIPeripheral,
                                uint8_t * const pvTxBuffer,
                                uint8_t * const pvRxBuffer,
                                size_t xBytes );

/**
 * @brief Aborts the ongoing operation on SPI bus if the underlying driver
 * allows the cancellation.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 *
 * @return SUCCESS = IOT_SPI_SUCCESS,
 *         FAILED = IOT_SPI_INVALID_VALUE,
 *         MASTER_UNINITALIZED = IOT_SPI_MASTER_NOT_INITIALIZED
 */
int32_t iot_spi_close( IotSPIHandle_t const pxSPIPeripheral );

/**
 * @brief This function is used to cancel the current operation in progress.
 *
 * @param[in] pxSPIPeripheral The SPI peripheral handle returned in open() call.
 *
 * @return SUCCESS = IOT_SPI_SUCCESS,
 *          FAILED = IOT_SPI_INVALID_VALUE
 *          MASTER_UNINITALIZED = IOT_SPI_MASTER_NOT_INITIALIZED,
 *          NOT_SUPPORTED = IOT_SPI_FUNCTION_NOT_SUPPORTED,
 *          NOTHING_TO_CANCEL = IOT_SPI_NOTHING_TO_CANCEL
 */
int32_t iot_spi_cancel( IotSPIHandle_t const pxSPIPeripheral );

/**
 * @}
 */
/* end of group iot_spi */

#endif /* _IOT_SPI_H_ */
