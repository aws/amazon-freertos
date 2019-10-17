/*
 * Amazon FreeRTOS Common IO V1.0.0
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_i2c.h
 * @brief File for the APIs of I2C called by application layer.
 */
#ifndef _IOT_I2C_H_
#define _IOT_I2C_H_

/* Standard includes. */
#include <stdint.h>
#include <stddef.h>

/**
 * @constants_page{common_io_i2c}
 * @constants_brief{Common I/O I2C library}
 *
 * @section common_io_i2c_constants_initializers I2C Speeds
 * @brief The speeds supported by I2C bus.
 *
 * @snippet this define_common_io_i2c_speeds
 *
 *
 * @section common_io_i2c_constants_errors I2C Return Codes
 * @brief The return codes for the functions in I2C.
 *
 * @snippet this define_common_io_i2c_errors
 */

/* @[define_common_io_i2c_speeds] */
#define IOT_I2C_STANDARD_MODE_BPS     ( 100000 )      /*!< Standard mode bits per second. */
#define IOT_I2C_FAST_MODE_BPS         ( 400000 )      /*!< Fast mode bits per second. */
#define IOT_I2C_FAST_MODE_PLUS_BPS    ( 1000000 )     /*!< Fast plus mode bits per second. */
#define IOT_I2C_HIGH_SPEED_BPS        ( 3400000 )     /*!< High speed mode bits per second. */
/* @[define_common_io_i2c_speeds] */

/* @[define_common_io_i2c_errors] */
#define IOT_I2C_SUCCESS                   ( 0 )     /*!< I2C operation completed successfully. */
#define IOT_I2C_INVALID_VALUE             ( 1 )     /*!< At least one parameter is invalid. */
#define IOT_I2C_BUSY                      ( 2 )     /*!< I2C bus is busy at current time. */
#define IOT_I2C_WRITE_FAILED              ( 3 )     /*!< I2C driver returns error when performing write operation. */
#define IOT_I2C_READ_FAILED               ( 4 )     /*!< I2C driver returns error when performing read operation. */
#define IOT_I2C_NACK                      ( 5 )     /*!< Unexpected NACK is caught. */
#define IOT_I2C_BUS_TIMEOUT               ( 6 )     /*!< I2C operation not completed within specified timeout. */
#define IOT_I2C_NOTHING_TO_CANCEL         ( 7 )     /*!< No ongoing transaction when cancel operation is performed. */
#define IOT_I2C_FUNCTION_NOT_SUPPORTED    ( 8 )     /*!< I2C operation is not supported. */
#define IOT_I2C_SLAVE_ADDRESS_NOT_SET     ( 9 )     /*!< Slave address is not set before calling I2C read or write operation. */
/* @[define_common_io_i2c_errors] */

/**
 * @enums_group{common_io_i2c}
 * @enums_brief{Common I/O I2C library}
 */

/**
 * @ingroup common_io_i2c_datatypes_enums
 * @brief I2C Bus status
 */
typedef enum
{
    eI2CBusIdle = 0,            /*!< I2C bus is idle. */
    eI2cBusBusy = IOT_I2C_BUSY, /*!< I2C bus is busy. */
} IotI2CBusStatus_t;

/**
 * @ingroup common_io_i2c_datatypes_enums
 * @brief I2C operation status.
 */
typedef enum
{
    eI2CCompleted = IOT_I2C_SUCCESS,         /*!< I2C operation completed successfully. */
    eI2CDriverFailed,                        /*!< I2C driver returns error during last operation. */
    eI2CNackFromSlave = IOT_I2C_NACK,        /*!< Unexpected NACK is caught. */
    eI2CMasterTimeout = IOT_I2C_BUS_TIMEOUT, /*!< I2C operation not completed within specified timeout. */
} IotI2COperationStatus_t;

/**
 * @ingroup common_io_i2c_datatypes_enums
 * @brief Ioctl request types.
 */
typedef enum
{
    eI2CSendNoStopFlag,  /**<! Set flag to not send stop after transaction */
                         /**<! Default is always stop for every transaction */
                         /**<! Flag will auto reset to stop after one transaction if you set no stop */
    eI2CSetSlaveAddr,    /**<! This can be either 7-bit address or 10-bit address. All the operations use this slave address after it is set. */
    eI2CSetMasterConfig, /**<! Sets the I2C bus frequency and timeout using the struct IotI2CConfig_t, default speed is Standard mode. */
    eI2CGetMasterConfig, /**<! Gets the I2C bus frequency and timeout set for the I2C master. */
    eI2CGetBusState,     /**<! Get the current I2C bus status. Returns eI2CBusIdle or eI2CBusy */
    eI2CBusReset,        /**<! Master resets the bus. */
    eI2CGetTxNoOfbytes,  /**<! Get the number of bytes sent in write operation. */
    eI2CGetRxNoOfbytes,  /**<! Get the number of bytes received in read operation. */
} IotI2CIoctlRequest_t;

/**
 * @paramstructs_group{common_io_i2c}
 * @paramstructs_brief{common_io_i2c, Common I/O I2C}
 */

/**
 * @ingroup common_io_i2c_datatypes_paramstructs
 * @brief I2C bus configuration
 */
typedef struct IotI2CConfig
{
    uint32_t ulMasterTimeout; /**<! Master timeout value in msec, to relinquish the bus if slave does not respond. This is an extended feature of SMBus which is not part of I2C protocol. */
    uint32_t ulBusFreq;       /**<! Bus frequency/baud rate */
} IotI2CConfig_t;

/**
 * @brief The I2C descriptor type defined in the source file.
 * This is an anonymous struct that is vendor/driver specific.
 */
struct                    IotI2CDescriptor;

/**
 * @handles_group{common_io_i2c}
 * @handles_brief{Common I/O I2C library}
 */

/**
 * @ingroup common_io_i2c_datatypes_handles
 * @brief IotI2CHandle_t is the handle type returned by calling iot_i2c_open().
 *        This is initialized in open and returned to caller. The caller must pass
 *        this pointer to the rest of APIs.
 */
typedef struct IotI2CDescriptor * IotI2CHandle_t;

/**
 * @brief The callback function for completion of I2C operation.
 *
 * @param[out] xOpStatus    I2C asynchronous operation status.
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used or modified by the driver. The context
 *                          is provided by the caller when setting the callback, and is
 *                          passed back to the caller in the callback.
 *
 */
typedef void (* IotI2CCallback_t) ( IotI2COperationStatus_t xOpStatus,
                                    void * pvUserContext );

/**
 * @functions_page{common_io_i2c, Common I/O I2C}
 * @functions_brief{common_io_i2c}
 * - @function_name{common_io_i2c_function_open}
 * @function_brief{common_io_i2c_function_open}
 * - @function_name{common_io_i2c_function_set_callback}
 * @function_brief{common_io_i2c_function_set_callback}
 * - @function_name{common_io_i2c_function_read_sync}
 * @function_brief{common_io_i2c_function_read_sync}
 * - @function_name{common_io_i2c_function_write_sync}
 * @function_brief{common_io_i2c_function_write_sync}
 * - @function_name{common_io_i2c_function_read_async}
 * @function_brief{common_io_i2c_function_read_async}
 * - @function_name{common_io_i2c_function_write_async}
 * @function_brief{common_io_i2c_function_write_async}
 * - @function_name{common_io_i2c_function_ioctl}
 * @function_brief{common_io_i2c_function_ioctl}
 * - @function_name{common_io_i2c_function_close}
 * @function_brief{common_io_i2c_function_close}
 * - @function_name{common_io_i2c_function_cancel}
 * @function_brief{common_io_i2c_function_cancel}
 */

/**
 * @page common_io_i2c_function_open iot_i2c_open
 * @snippet this declare_common_io_i2c_open
 * @copydoc iot_i2c_open
 * @page common_io_i2c_function_set_callback iot_i2c_set_callback
 * @snippet this declare_common_io_i2c_set_callback
 * @copydoc iot_i2c_set_callback
 * @page common_io_i2c_function_open iot_i2c_open
 * @snippet this declare_common_io_i2c_open
 * @copydoc iot_i2c_open
 * @page common_io_i2c_function_read_sync iot_i2c_read_sync
 * @snippet this declare_common_io_i2c_read_sync
 * @copydoc iot_i2c_read_sync
 * @page common_io_i2c_function_write_sync iot_i2c_write_sync
 * @snippet this declare_common_io_i2c_write_sync
 * @copydoc iot_i2c_write_sync
 * @page common_io_i2c_function_read_async iot_i2c_read_async
 * @snippet this declare_common_io_i2c_read_async
 * @copydoc iot_i2c_read_async
 * @page common_io_i2c_function_write_async iot_i2c_write_async
 * @snippet this declare_common_io_i2c_write_async
 * @copydoc iot_i2c_write_async
 * @page common_io_i2c_function_ioctl iot_i2c_ioctl
 * @snippet this declare_common_io_i2c_ioctl
 * @copydoc iot_i2c_ioctl
 * @page common_io_i2c_function_close iot_i2c_close
 * @snippet this declare_common_io_i2c_close
 * @copydoc iot_i2c_close
 * @page common_io_i2c_function_cancel iot_i2c_cancel
 * @snippet this declare_common_io_i2c_cancel
 * @copydoc iot_i2c_cancel
 */

/**
 * @brief Initiates and reserves an I2C instance as master.
 *
 * One instance can communicate with one or more slave devices.
 * Slave addresses need to be changed between actions to different slave devices.
 *
 * @warning Once opened, the same I2C instance must be closed before calling open again.
 *
 * @param[in] lI2CInstance The instance of I2C to initialize. This is between 0 and the number of I2C instances on board - 1.
 *
 * @return
 * - 'the handle to the I2C port (not NULL)', on success.
 * - 'NULL', if
 *     - invalid instance number
 *     - open same instance more than once before closing it
 */
/* @[declare_common_io_i2c_open] */
IotI2CHandle_t iot_i2c_open( int32_t lI2CInstance );
/* @[declare_common_io_i2c_open] */


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
 * @param[in] pxI2CPeripheral The I2C peripheral handle returned in the open() call.
 * @param[in] xCallback The callback function to be called on completion of transaction.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
/* @[declare_common_io_i2c_set_callback] */
void iot_i2c_set_callback( IotI2CHandle_t const pxI2CPeripheral,
                           IotI2CCallback_t xCallback,
                           void * pvUserContext );
/* @[declare_common_io_i2c_set_callback] */

/**
 * @brief Starts the I2C master read operation in synchronous mode.
 *
 * This function attempts to read certain number of bytes from slave device to a pre-allocated buffer, in synchronous way.
 * Partial read might happen, e.g. no more data is available.
 * And the number of bytes that have been actually read can be obtained by calling iot_i2c_ioctl.
 *
 * @note Usually, the address of register needs to be written before calling this function.
 * @note If eI2CSendNoStopFlag is set and this function returns, whether the actual transaction has been started is undefined. This is board-specific behavior.
 *
 * @warning Prior to this function, slave address must be already configured.
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[out] pucBuffer The receive buffer to read the data into. It must stay allocated before this function returns.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 * - IOT_I2C_SUCCESS, on success (all the requested bytes have been read)
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_I2C_SLAVE_ADDRESS_NOT_SET, if slave address is not set yet
 * - IOT_I2C_NACK or IOT_I2C_READ_FAILED, if
 *     - no device correspond to the slave address
 *     - the slave is unable to receive or transmit
 *     - the slave gets data or commands that it does not understand
 *     - there is some unknown driver error
 * - IOT_I2C_BUS_TIMEOUT, if timeout is supported and slave device does not respond within configured timeout.
 * - IOT_I2C_BUSY, if the bus is busy which means there is an ongoing transaction.
 *
 * <b>Example</b>
 * @code{c}
 *  // Declare an I2C handle.
 *  IotI2CHandle_t xI2CHandle;
 *
 *  // Return value of I2C functions.
 *  int32_t lRetVal = IOT_I2C_SUCCESS;
 *
 *  // Register address on I2C slave device.
 *  uint8_t xDeviceRegisterAddress = 0x73;
 *
 *  // Number of read/write bytes.
 *  uint16_t usReadBytes = 0;
 *  uint16_t usWriteBytes = 0;
 *
 *  uint8_t ucReadBuffer[2] = {0};
 *
 *  // Configurations of I2C master device.
 *  IotI2CConfig_t xI2CConfig =
 *  {
 *      .ulBusFreq       = IOT_I2C_FAST_MODE_BPS,
 *      .ulMasterTimeout = 500
 *  };
 *
 *  // Open one of the I2C instance and get a handle.
 *  xI2CHandle = iot_i2c_open( 1 );
 *
 *  if ( xI2CHandle != NULL )
 *  {
 *      // Set I2C configuration.
 *      lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetMasterConfig, &xI2CConfig );
 *      // assert(lRetVal == IOT_I2C_SUCCESS);
 *
 *      // Set slave address.
 *      lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CSetSlaveAddr, &uctestIotI2CSlaveAddr );
 *      // assert(lRetVal == IOT_I2C_SUCCESS);
 *
 *      // Write the register address as single byte, in a transaction.
 *      lRetVal = iot_i2c_write_sync( xI2CHandle, &xDeviceRegisterAddress, sizeof( xDeviceRegisterAddress ) );
 *
 *      if ( lRetVal == IOT_I2C_SUCCESS )
 *      {
 *          // Get the number of written bytes in last transaction.
 *          lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetTxNoOfbytes, &usWriteBytes );
 *          // assert(lRetVal == IOT_I2C_SUCCESS);
 *          // assert(usWriteBytes == 1);
 *
 *          // Read two bytes of data to allocated buffer, in a transaction.
 *          lRetVal = iot_i2c_read_sync( xI2CHandle, &ucReadBuffer, sizeof( ucReadBuffer ) );
 *
 *          if ( lRetVal == IOT_I2C_SUCCESS )
 *          {
 *              // Get the number of read bytes in last transaction.
 *              lRetVal = iot_i2c_ioctl( xI2CHandle, eI2CGetRxNoOfbytes, &usReadBytes );
 *              // assert(lRetVal == IOT_I2C_SUCCESS);
 *              // assert(usReadBytes == 2);
 *          }
 *      }
 *
 *      lRetVal = iot_i2c_close( xI2CHandle );
 *      // assert(lRetVal == IOT_I2C_SUCCESS);
 *  }
 * @endcode
 */
/* @[declare_common_io_i2c_read_sync] */
int32_t iot_i2c_read_sync( IotI2CHandle_t const pxI2CPeripheral,
                           uint8_t * const pucBuffer,
                           size_t xBytes );
/* @[declare_common_io_i2c_read_sync] */

/**
 * @brief Starts the I2C master write operation in synchronous mode.
 *
 * This function attempts to write certain number of bytes from a pre-allocated buffer to a slave device, in synchronous way.
 * Partial write might happen, e.g. slave device unable to receive more data.
 * And the number of bytes that have been actually written can be obtained by calling iot_i2c_ioctl.
 *
 * @note Usually, the first byte is treated as the register address and the following bytes are treated as data to be written.
 * @note If eI2CSendNoStopFlag is set and this function returns, whether the actual transaction has been started is undefined. This is board-specific behavior.
 *
 * @warning Prior to this function, slave address must be already configured.
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[in] pucBuffer The transmit buffer containing the data to be written. It must stay allocated before this function returns.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return
 * - IOT_I2C_SUCCESS, on success (all the requested bytes have been written)
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_I2C_SLAVE_ADDRESS_NOT_SET, if slave address is not set yet
 * - IOT_I2C_NACK or IOT_I2C_WRITE_FAILED, if
 *     - no device correspond to the slave address
 *     - the slave is unable to receive or transmit
 *     - the slave gets data or commands that it does not understand
 *     - there is some unknown driver error
 * - IOT_I2C_BUS_TIMEOUT, if timeout is supported and slave device does not respond within configured timeout.
 * - IOT_I2C_BUSY, if the bus is busy which means there is an ongoing transaction.
 */
/* @[declare_common_io_i2c_write_sync] */
int32_t iot_i2c_write_sync( IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pucBuffer,
                            size_t xBytes );
/* @[declare_common_io_i2c_write_sync] */

/**
 * @brief Starts the I2C master read operation in asynchronous mode.
 *
 * This function attempts to read certain number of bytes from a pre-allocated buffer, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_i2c_ioctl.
 * Once the operation completes, successful or not, the user callback will be invoked.
 *
 * Partial read might happen, e.g. slave device unable to receive more data.
 * And the number of bytes that have been actually read can be obtained by calling iot_i2c_ioctl.
 *
 * @note Usually, the address of register needs to be written before calling this function.
 * @note If eI2CSendNoStopFlag is set and this function returns, whether the actual transaction has been started is undefined. This is board-specific behavior.
 * @note In order to get notification when the asynchronous call is completed, iot_i2c_set_callback must be called prior to this.
 *
 * @warning Prior to this function, slave address must be already configured.
 * @warning pucBuffer must be valid before callback is invoked.
 * @warning None of other read or write functions shall be called during this function or before user callback.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[out] pucBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 * - IOT_I2C_SUCCESS, on success
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_I2C_SLAVE_ADDRESS_NOT_SET, if slave address is not set yet
 * - IOT_I2C_NACK or IOT_I2C_READ_FAILED, if
 *     - no device correspond to the slave address
 *     - the slave is unable to receive or transmit
 *     - the slave gets data or commands that it does not understand
 *     - there is some unknown driver error
 */
/* @[declare_common_io_i2c_read_async] */
int32_t iot_i2c_read_async( IotI2CHandle_t const pxI2CPeripheral,
                            uint8_t * const pucBuffer,
                            size_t xBytes );
/* @[declare_common_io_i2c_read_async] */

/**
 * @brief Starts the I2C master write operation in asynchronous mode.
 *
 * This function attempts to write certain number of bytes from a pre-allocated buffer to a slave device, in asynchronous way.
 * It returns immediately when the operation is started and the status can be check by calling iot_i2c_ioctl.
 * Once the operation completes, successful or not, the user callback will be invoked.
 *
 * Partial write might happen, e.g. slave device unable to receive more data.
 * And the number of bytes that have been actually written can be obtained by calling iot_i2c_ioctl.
 *
 * @note Usually, the first byte is treated as the register address and the following bytes are treated as data to be written.
 * @note If eI2CSendNoStopFlag is set and this function returns, whether the actual transaction has been started is undefined. This is board-specific behavior.
 * @note In order to get notification when the asynchronous call is completed, iot_i2c_set_callback must be called prior to this.
 *
 * @warning Prior to this function, slave address must be already configured.
 * @warning None of other read or write functions shall be called during this function.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[in] pucBuffer The transmit buffer containing the data to be written. It must stay allocated before this function returns.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return
 * - IOT_I2C_SUCCESS, on success
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 *     - pucBuffer is NULL
 *     - xBytes is 0
 * - IOT_I2C_SLAVE_ADDRESS_NOT_SET, if slave address is not set yet
 * - IOT_I2C_NACK or IOT_I2C_WRITE_FAILED, if
 *     - no device correspond to the slave address
 *     - the slave is unable to receive or transmit
 *     - the slave gets data or commands that it does not understand
 *     - there is some unknown driver error
 * - IOT_I2C_BUS_TIMEOUT, if timeout is supported and slave device does not respond within configured timeout.
 * - IOT_I2C_BUSY, if the bus is busy which means there is an ongoing transaction.
 */
/* @[declare_common_io_i2c_write_async] */
int32_t iot_i2c_write_async( IotI2CHandle_t const pxI2CPeripheral,
                             uint8_t * const pucBuffer,
                             size_t xBytes );
/* @[declare_common_io_i2c_write_async] */

/**
 * @brief Configures the I2C master with user configuration.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 * @param[in] xI2CRequest Should be one of I2C_Ioctl_Request_t.
 * @param[in,out] pvBuffer The configuration values for the IOCTL request.
 *
 * @note SetMasterConfig is expected only called once at beginning.
 * This request expects the buffer with size of IotI2CConfig_t.
 *
 * @note eI2CGetMasterConfig gets the current configuration for I2C master.
 * This request expects the buffer with size of IotI2CConfig_t.
 *
 * @note eI2CGetBusState gets the current bus state.
 * This request expects buffer with size of IotI2CBusStatus_t.
 *
 * @note eI2CSendNoStopFlag is called at every operation you want to not send stop condition.
 *
 * @note eI2CSetSlaveAddr sets either 7-bit address or 10-bit address, according to hardware's capability.
 * This request expects 2 bytes buffer (uint16_t)
 *
 * @note eI2CGetTxNoOfbytes returns the number of written bytes in last transaction.
 * This is supposed to be called in the caller task or application callback, right after last transaction completes.
 * This request expects 2 bytes buffer (uint16_t).
 *
 * - If the last transaction only did write, this returns the actual number of written bytes which might be smaller than the requested number (partial write).
 * - If the last transaction only did read, this returns 0.
 * - If the last transaction did both write and read, this returns the number of written bytes.
 *
 * @note eI2CGetRxNoOfbytes returns the number of read bytes in last transaction.
 * This is supposed to be called in the caller task or application callback, right after last transaction completes.
 * This request expects 2 bytes buffer (uint16_t).
 *
 * - If the last transaction only did read, this returns the actual number of read bytes which might be smaller than the requested number (partial read).
 * - If the last transaction only did write, this returns 0.
 * - If the last transaction did both write and read, this returns the number of read bytes.
 *
 *
 * @return
 * - IOT_I2C_SUCCESS, on success
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 *     - pucBuffer is NULL with requests which needs buffer
 * - IOT_I2C_FUNCTION_NOT_SUPPORTED, if this board doesn't support this feature.
 *     - eI2CSetSlaveAddr: 10-bit address is not supported
 *     - eI2CSendNoStopFlag: explicitly not sending stop condition is not supported
 *     - eI2CBusReset: reset bus is not supported
 */
/* @[declare_common_io_i2c_ioctl] */
int32_t iot_i2c_ioctl( IotI2CHandle_t const pxI2CPeripheral,
                       IotI2CIoctlRequest_t xI2CRequest,
                       void * const pvBuffer );
/* @[declare_common_io_i2c_ioctl] */

/**
 * @brief Stops the ongoing operation and de-initializes the I2C peripheral.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 *
 * @return
 * - IOT_I2C_SUCCESS, on success
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 */
/* @[declare_common_io_i2c_close] */
int32_t iot_i2c_close( IotI2CHandle_t const pxI2CPeripheral );
/* @[declare_common_io_i2c_close] */

/**
 * @brief This function is used to cancel the current operation in progress, if possible.
 *
 * @param[in] pxI2CPeripheral The I2C handle returned in open() call.
 *
 * @return
 * - IOT_I2C_SUCCESS, on success
 * - IOT_I2C_INVALID_VALUE, if
 *     - pxI2CPeripheral is NULL
 *     - pxI2CPeripheral is not opened yet
 * - IOT_I2C_NOTHING_TO_CANCEL, if there is no on-going transaction.
 * - IOT_I2C_FUNCTION_NOT_SUPPORTED, if this board doesn't support this operation.
 */
/* @[declare_common_io_i2c_cancel] */
int32_t iot_i2c_cancel( IotI2CHandle_t const pxI2CPeripheral );
/* @[declare_common_io_i2c_cancel] */

#endif /* ifndef _IOT_I2C_H_ */
