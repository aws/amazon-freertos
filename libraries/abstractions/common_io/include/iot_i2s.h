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
#define IOT_I2S_SUCCESS                 ( 0 )
#define IOT_I2S_INVALID_VALUE           ( 1 )
#define IOT_I2S_NOT_INITIALIZED         ( 2 )
#define IOT_I2S_BUSY                    ( 3 )
#define IOT_I2S_WRITE_FAILED            ( 4 )
#define IOT_I2S_READ_FAILED             ( 5 )
#define IOT_I2S_NOTHING_TO_CANCEL       ( 7 )
#define IOT_I2S_FUNCTION_NOT_SUPPORTED  ( 8 )

/**
 * @brief I2S Bus status
 */
typedef enum
{
    eI2SBusIdle = 0,
    eI2SBusBusy = IOT_I2S_BUSY,
} IotI2SBusStatus_t;

/**
 * @brief I2S operation status.
 */
typedef enum
{
    eI2SCompleted = IOT_I2S_SUCCESS,
    eI2SLastWriteFailed = IOT_I2S_WRITE_FAILED,
    eI2SLastReadFailed = IOT_I2S_READ_FAILED,

} IotI2SOperationStatus_t;

/**
 * @brief Ioctl request types.
 */
typedef enum
{
    eI2SSetConfig,      /* Set I2S configuration using the struct IotI2SIoctlConfig_t */
    eI2SGetConfig,      /* Get I2S configuration using the struct IotI2SIoctlConfig_t */
    eI2SGetBusState,    /* Get the State of the I2S Bus form of the error code i.e. Idle or Busy */
} IotI2SIoctlRequest_t;

/**
 * @brief I2S mode types.
 */
typedef enum
{
    eI2SNormalMode,
    eI2SLeftJustifiedMode,
    eI2SRightJustifiedMode,
    eI2SPcmMode,
    eI2SDspMode,
} IotI2SMode_t;

/**
 * @brief I2S channel types.
 */
typedef enum
{
    eI2SChannelStereo,
    eI2SChannelMono,
} IotI2SChannel_t;

/**
 * @brief I2S clock polarity for SCK and WS
 */
typedef enum
{
    eI2SFallingEdge,
    eI2SRisingEdge,
} IotI2SClkPolarity_t;

/**
 * @brief I2S configuration
 */
typedef struct
{
    IotI2SMode_t xI2SMode;                /*!< Set I2S mode using IotI2SMode_t */
    IotI2SChannel_t xI2SChannel;          /*!< Set I2S channel using IotI2SChannel_t */
    IotI2SClkPolarity_t xI2SSckPolarity;  /*!< Set I2S SCK polarity using IotI2SClkPolarity_t */
    IotI2SClkPolarity_t xI2SWsPolarity;   /*!< Set I2S WS polarity using IotI2SClkPolarity_t */
    uint32_t ulI2SFrequency;              /*!< Frequency to transfer/receive data*/
    uint32_t ulI2SDataLength;             /*!< Number of data bits per sample */
    uint32_t ulI2SFrameLength;            /*!< Number of data bits in the frames */
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
typedef void ( * IotI2SCallback_t )( IotI2SOperationStatus_t xOpStatus, void * pvUserContext );

/**
 * @brief Initiates the I2S bus as.
 *
 * @param[in]   lI2SInstance The index of I2S to initialize.
 *
 * @return  The handle to the I2S port if SUCCESS else NULL.
 */
IotI2SHandle_t iot_i2s_open( int32_t lI2SInstance );

/**
 * @brief Sets the application callback to be called on completion of an operation.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 * @param[in]   xCallback  The callback function to be called on completion of transaction.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
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
 * @return  SUCCESS=IOT_I2S_SUCCESS, FAILED=IOT_I2S_INVALID_VALUE, Driver_Error=IOT_I2S_READ_FAIL, Not initialized=IOT_I2S_NOT_INITIALIZED
 */
int32_t iot_i2s_read_async( IotI2SHandle_t const pxI2SPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes);

/**
 * @brief Starts the I2S write operation in non-blocking mode.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 * @param[in]   pvBuffer        The transmit buffer to write the data into.
 * @param[in]   xBytes          The number of bytes to write.
 *
 * @return  SUCCESS=IOT_I2S_SUCCESS, FAILED=IOT_I2S_INVALID_VALUE, Driver_Error=IOT_I2S_WRITE_FAILED, Not initialized=IOT_I2S_NOT_INITIALIZED
 */
int32_t iot_i2s_write_async( IotI2SHandle_t const pxI2SPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes);

/**
 * @brief Starts the I2S read operation in blocking mode.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 * @param[in]   pvBuffer        The receive buffer to read the data into.
 * @param[in]   xBytes          The number of bytes to read.
 *
 * @return  SUCCESS=IOT_I2S_SUCCESS, FAILED=IOT_I2S_INVALID_VALUE, Driver_Error=IOT_I2S_READ_FAILED, Not initialized=IOT_I2S_NOT_INITIALIZED
 */
int32_t iot_i2s_read_sync( IotI2SHandle_t const pxI2SPeripheral,
                           uint8_t * const pvBuffer,
                           size_t xBytes);

/**
 * @brief Starts the I2S write operation in blocking mode.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 * @param[in]   pvBuffer        The transmit buffer to write the data into.
 * @param[in]   xBytes          The number of bytes to write.
 *
 * @return  SUCCESS=IOT_I2S_SUCCESS, FAILED=IOT_I2S_INVALID_VALUE, Driver_Error=IOT_I2S_WRITE_FAIL, Not initialized=IOT_I2S_NOT_INITIALIZED
 */
int32_t iot_i2s_write_sync( IotI2SHandle_t const pxI2SPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes);

/**
 * @brief Stops the ongoing operation and deinitializes the I2S peripheral.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 *
 * @return  SUCCESS=IOT_I2S_SUCCESS, Not initialized=IOT_I2S_NOT_INITIALIZED
 */
int32_t iot_i2s_close( IotI2SHandle_t const pxI2SPeripheral);

/**
 * @brief Configures the I2S with user configuration.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 * @param[in]   lRequest    Should be one of I2S_Ioctl_Request_t.
 * @param[in]   pvBuffer    The configuration values for the IOCTL request.
 *
 * @return  SUCCESS=IOT_I2S_SUCCESS, or one of IOT_I2S_INVALID_VALUE, IOT_I2S_NOT_INITIALIZED,
 *          IOT_I2S_FUNCTION_NOT_SUPPORTED
 */
int32_t iot_i2s_ioctl( IotI2SHandle_t const pxI2SPeripheral,
                       IotI2SIoctlRequest_t xI2SRequest,
                       void * const pvBuffer );

/**
 * @brief This function is used to cancel the current operation in progress if possible.
 *
 * @param[in]   pxI2SPeripheral The I2S handle returned in open() call.
 *
 * @return  SUCCESS=IOT_I2S_SUCCESS, or one of IOT_I2S_INVALID_VALUE on error
 *          or IOT_I2S_NOTHING_TO_CANCEL, IOT_I2S_FUNCTION_NOT_SUPPORTED
 */
int32_t iot_i2s_cancel( IotI2SHandle_t const pxI2SPeripheral);

#endif /* ifndef _IOT_HAL_I2S_H_ */
