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

/*******************************************************************************
 * @file iot_hal_i2s.h
 * @brief File for the APIs of I2S called by application layer.
 *******************************************************************************
 */

#ifndef _IOT_I2S_H_
#define _IOT_I2S_H_

/**
 * The return codes for the functions in I2S.
 */
#define IOT_I2S_SUCCESS                   ( 0 )  /*!< I2S operation completed successfully. */
#define IOT_I2S_INVALID_VALUE             ( 1 )  /*!< At least one parameter is invalid. */
#define IOT_I2S_NOT_INITIALIZED           ( 2 )  /*!< I2S interface not initialized. */
#define IOT_I2S_BUSY                      ( 3 )  /*!< I2S operation can't be completed because another operation is happening. */
#define IOT_I2S_WRITE_FAILED              ( 4 )  /*!< I2S write operation failed. */
#define IOT_I2S_READ_FAILED               ( 5 )  /*!< I2S read operation failed. */
#define IOT_I2S_NOTHING_TO_CANCEL         ( 7 )  /*!< No active I2S operations to cancel. */
#define IOT_I2S_FUNCTION_NOT_SUPPORTED    ( 8 )  /*!< I2S operation not supported. */

/**
 * @brief I2S Bus status
 */
typedef enum
{
    eI2SBusIdle = 0,            /*!< I2S Bus status is idle. */
    eI2SBusBusy = IOT_I2S_BUSY, /*!< I2S Bus status is busy. */
} IotI2SBusStatus_t;

/**
 * @brief I2S operation status.
 */
typedef enum
{
    eI2SCompleted = IOT_I2S_SUCCESS,            /*!< I2S operation completed successfully. */
    eI2SLastWriteFailed = IOT_I2S_WRITE_FAILED, /*!< I2S write operation failed. */
    eI2SLastReadFailed = IOT_I2S_READ_FAILED,   /*!< I2S read operation failed. */
} IotI2SOperationStatus_t;

/**
 * @brief Ioctl request types.
 */
typedef enum
{
    eI2SSetConfig,      /*!< Set I2S configuration taking the struct IotI2SIoctlConfig_t */
    eI2SGetConfig,      /*!< Get I2S configuration returning the struct IotI2SIoctlConfig_t */
    eI2SGetBusState,    /*!< Get the State of the I2S Bus returning the struct IotI2SBusStatus_t */
    eI2SGetTxNoOfbytes, /*!< Get the number of bytes sent in write operation. Returns uint32_t type. */
    eI2SGetRxNoOfbytes, /*!< Get the number of bytes received in read operation. Returns uint32_t type. */
} IotI2SIoctlRequest_t;

/**
 * @brief I2S mode types.
 */
typedef enum
{
    eI2SNormalMode,         /*!< HW uses normal mode for signal detection. */
    eI2SLeftJustifiedMode,  /*!< HW uses left justified mode for signal detection. */
    eI2SRightJustifiedMode, /*!< HW uses right justified mode for signal detection. */
    eI2SPcmMode,            /*!< HW uses PCM mode for signal detection. */
    eI2SDspMode,            /*!< HW uses DSP mode for signal detection. */
} IotI2SMode_t;

/**
 * @brief I2S channel types.
 */
typedef enum
{
    eI2SChannelStereo, /*!< Channel is in stereo. */
    eI2SChannelMono,   /*!< Channel is in mono. */
} IotI2SChannel_t;

/**
 * @brief I2S clock polarity for SCK and WS
 */
typedef enum
{
    eI2SFallingEdge, /*!< Clock polarity detected on falling edge of signal. */
    eI2SRisingEdge,  /*!< Clock polarity detected on rising edge of signal. */
} IotI2SClkPolarity_t;

/**
 * @brief I2S configuration
 */
typedef struct
{
    IotI2SMode_t xI2SMode;               /*!< Set I2S mode using IotI2SMode_t */
    IotI2SChannel_t xI2SChannel;         /*!< Set I2S channel using IotI2SChannel_t */
    IotI2SClkPolarity_t xI2SSckPolarity; /*!< Set I2S SCK polarity using IotI2SClkPolarity_t */
    IotI2SClkPolarity_t xI2SWsPolarity;  /*!< Set I2S WS polarity using IotI2SClkPolarity_t */
    uint32_t ulI2SFrequency;             /*!< Frequency to transfer/receive data*/
    uint32_t ulI2SDataLength;            /*!< Number of data bits per sample */
    uint32_t ulI2SFrameLength;           /*!< Number of data bits in the frames */
} IotI2sIoctlConfig_t;

/**
 * @brief Opaque descriptor of I2C
 */
struct IotI2SDescriptor;

/**
 * @brief The handle for a I2S peripheral.
 */
typedef struct IotI2SDescriptor * IotI2SHandle_t;

/**
 * @brief The callback function for completion of I2C operation.
 *
 * @param[out] xOpStatus    I2S asynchronous operation status.
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used or modified by the driver. The context
 *                          is provided by the caller when setting the callback, and is
 *                          passed back to the caller in the callback.
 */
typedef void ( * IotI2SCallback_t )( IotI2SOperationStatus_t xOpStatus,
                                     void * pvUserContext );

/**
 * @brief Initiates the I2S bus as.
 *
 * @param[in]   lI2SInstance The index of I2S to initialize.
 *
 * @return
 *   - The handle to the I2S port if SUCCESS
 *   - NULL if
 *      - invalid lI2SInstance
 *      - lI2SInstance already open
 */
IotI2SHandle_t iot_i2s_open( int32_t lI2SInstance );

/**
 * @brief Sets the application callback to be called on completion of an operation.
 *
 * @note Single callback is used per instance for iot_i2s_read_async() and iot_i2s_write_async() calls only.
 * @note Newly set callback overrides the one previously set
 *
 * @warning If input handle or if callback function is NULL, this function silently takes no action.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 * @param[in]   xCallback  The callback function to be called on completion of transaction.
 * @param[in]   pvUserContext The user context to be passed back when callback is called.
 */
void iot_i2s_set_callback( IotI2SHandle_t const pxI2SPeripheral,
                           IotI2SCallback_t xCallback,
                           void * pvUserContext );

/**
 * @brief Starts the I2S read operation in non-blocking mode.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 * @param[in]   pvBuffer        The receive buffer to read the data into.
 * @param[in]   xBytes          The number of bytes to read.
 *
 * @return
 *   - IOT_I2S_SUCCESS on success
 *   - IOT_I2S_INVALID_VALUE if
 *      - pxI2SPeriperal or pvBuffer are NULL
 *      - xBytes == 0
 *   - IOT_I2S_READ_FAIL if there was an error doing the read
 *   - IOT_I2S_NOT_INITIALIZED if I2S not first initialized with ioctl call eI2SSetConfig.
 *   - IOT_I2S_BUSY if another read (sync or async) is in process
 */
int32_t iot_i2s_read_async( IotI2SHandle_t const pxI2SPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes );

/**
 * @brief Starts the I2S write operation in non-blocking mode.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 * @param[in]   pvBuffer        The transmit buffer to write the data into.
 * @param[in]   xBytes          The number of bytes to write.
 *
 * @return
 *   - IOT_I2S_SUCCESS on success
 *   - IOT_I2S_INVALID_VALUE if
 *      - pxI2SPeriperal or pvBuffer are NULL
 *      - xBytes == 0
 *   - IOT_I2S_WRITE_FAIL if there was an error doing the write
 *   - IOT_I2S_NOT_INITIALIZED if I2S not first initialized with ioctl call eI2SSetConfig.
 *   - IOT_I2S_BUSY if another write (sync or async) is in process
 */
int32_t iot_i2s_write_async( IotI2SHandle_t const pxI2SPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes );

/**
 * @brief Starts the I2S read operation in blocking mode.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 * @param[in]   pvBuffer        The receive buffer to read the data into.
 * @param[in]   xBytes          The number of bytes to read.
 *
 * @return
 *   - IOT_I2S_SUCCESS on success
 *   - IOT_I2S_INVALID_VALUE if
 *      - pxI2SPeriperal or pvBuffer are NULL
 *      - xBytes == 0
 *   - IOT_I2S_READ_FAIL if there was an error doing the read
 *   - IOT_I2S_NOT_INITIALIZED if I2S not first initialized with ioctl call eI2SSetConfig.
 *   - IOT_I2S_BUSY if another read (sync or async) is in process
 */
int32_t iot_i2s_read_sync( IotI2SHandle_t const pxI2SPeripheral,
                           uint8_t * const pvBuffer,
                           size_t xBytes );

/**
 * @brief Starts the I2S write operation in blocking mode.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 * @param[in]   pvBuffer        The transmit buffer to write the data into.
 * @param[in]   xBytes          The number of bytes to write.
 *
 * @return
 *   - IOT_I2S_SUCCESS on success
 *   - IOT_I2S_INVALID_VALUE if
 *      - pxI2SPeriperal or pvBuffer are NULL
 *      - xBytes == 0
 *   - IOT_I2S_WRITE_FAIL if there was an error doing the write
 *   - IOT_I2S_NOT_INITIALIZED if I2S not first initialized with ioctl call eI2SSetConfig.
 *   - IOT_I2S_BUSY if another write (sync or async) is in process
 */
int32_t iot_i2s_write_sync( IotI2SHandle_t const pxI2SPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes );

/**
 * @brief Stops the ongoing operation and deinitializes the I2S peripheral.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 *
 * @return
 *   - IOT_I2S_SUCCESS on success
 *   - IOT_I2S_INVALID_VALUE if
 *      - pxI2SPeriperal is NULL
 *      - instance not open (previously closed)
 */
int32_t iot_i2s_close( IotI2SHandle_t const pxI2SPeripheral );

/**
 * @brief Configures the I2S with user configuration.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 * @param[in]   lRequest    Should be one of I2S_Ioctl_Request_t.
 * @param[in]   pvBuffer    The configuration values for the IOCTL request.
 *
 * @note eI2SGetTxNoOfbytes returns the number of written bytes in last operation.
 * This is supposed to be called in the caller task or application callback, right after last operation completes.
 * This request expects 4 bytes buffer (uint32_t).
 *
 * - If the last operation only did read, this returns 0.
 *
 * @note eI2SGetRxNoOfbytes returns the number of read bytes in last operation.
 * This is supposed to be called in the caller task or application callback, right after last operation completes.
 * This request expects 4 bytes buffer (uint32_t).
 *
 * - If the last operation was a read, this returns the actual number of read bytes which might be smaller than the requested number (partial read).
 * - If the last operation was a write, this returns 0.
 *
 * @return
 *   - IOT_I2S_SUCCESS on success
 *   - IOT_I2S_INVALID_VALUE if
 *      - pxI2SPeriperal is NULL
 *      - xI2SRequest is invalid
 *      - pvBuffer is NULL
 *   - IOT_I2S_FUNCTION_NOT_SUPPORTED if designated IotI2SMode_t in eI2SSetConfig is not supported.
 */
int32_t iot_i2s_ioctl( IotI2SHandle_t const pxI2SPeripheral,
                       IotI2SIoctlRequest_t xI2SRequest,
                       void * const pvBuffer );

/**
 * @brief This function is used to cancel the current operation in progress if possible.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 *
 * @return
 *   - IOT_I2S_SUCCESS on success
 *   - IOT_I2S_INVALID_VALUE if pxI2SPeriperal is NULL
 *   - IOT_I2S_NOTHING_TO_CANCEL if nothing to cancel.
 *   - IOT_I2S_FUNCTION_NOT_SUPPORTED if cancel operation not supported.
 */
int32_t iot_i2s_cancel( IotI2SHandle_t const pxI2SPeripheral );

#endif /* ifndef _IOT_HAL_I2S_H_ */
