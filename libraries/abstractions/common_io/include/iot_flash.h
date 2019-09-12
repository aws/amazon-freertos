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
 * @file    iot_flash.h
 * @brief   This file contains all the Flash HAL API definitions
 */

#ifndef _IOT_FLASH_H_
#define _IOT_FLASH_H_

/**
 * @defgroup iot_flash Flash HAL APIs
 * @{
 */

/**
 * @brief Return values used by this driver
 */
#define IOT_FLASH_SUCCESS                   ( 0 )
#define IOT_FLASH_INVALID_VALUE             ( 1 )
#define IOT_FLASH_WRITE_FAILED              ( 2 )
#define IOT_FLASH_READ_FAILED               ( 3 )
#define IOT_FLASH_ERASE_FAILED              ( 4 )
#define IOT_FLASH_DEVICE_BUSY               ( 5 )
#define IOT_FLASH_CTRL_OP_FAILED            ( 6 )
#define IOT_FLASH_FUNCTION_NOT_SUPPORTED    ( 7 )

/**
 * @brief Flash current status
 */
typedef enum
{
    eFlashIdle,
    eFlashCmdInProgress,
    eFlashEraseFailed,
    eFlashProgramSuspended,
    eFlashEraseSuspended
} IotFlashStatus_t;

/**
 * @brief Flash driver last operation status
 */
typedef enum
{
    eFlashCompleted = IOT_FLASH_SUCCESS,
    eFlashLastEraseFailed = IOT_FLASH_ERASE_FAILED,
    eFlashLastReadFailed = IOT_FLASH_READ_FAILED,
    eFlashLastWriteFailed = IOT_FLASH_WRITE_FAILED,
} IotFlashOperationStatus_t;

/**
 * @brief Flash sector protection type
 */
typedef enum
{
    eFlashReadWrite,
    eFlashReadOnly
} IotFlashWriteProtect_t;

/**
 * @brief flash information
 */
typedef struct IotFlashInfo
{
    uint32_t ulFlashSize;       /*!< flash total size */
    uint32_t ulBlockSize;       /*!< flash device's larger block size in bytes.*/
    uint32_t ulSectorSize;      /*!< flash sector size */
    uint32_t ulPageSize;        /*!< page size in bytes */
    uint32_t ulLockSupportSize; /*!< Size of the physical block, for which write protection can be applied.
                                 *   for example, if only 64k sectors can be write protected this will be set to 64K,
                                 *   and if sub-sectors of 4k can be write protected then this may be set to 4K*/
    uint8_t ucAsyncSupported;   /*!< Provides a flag to indicate if background erase/program
                                 * is supported. User must check this before they can setup a callback and wait.*/
} IotFlashInfo_t;

/**
 * Flash protection configuration.
 */
typedef struct IotFlashWriteProtectConfig
{
    uint32_t ulAddress;                      /*!< flash offset address, must be aligned to ulLockSupportSize.*/
    uint32_t ulSize;                         /*!< size in bytes. Must be multiple of ulLockSupportSize.*/
    IotFlashWriteProtect_t xProtectionLevel; /*!< Protection level. Either readonly or readwrite*/
} IotFlashWriteProtectConfig_t;

/**
 * @brief Ioctl request types.
 */
typedef enum IotFlashIoctlRequest
{
    eSetFlashBlockProtection,  /*!< Set block protection, takes the input of IotFlashWriteProtectConfig_t type*/
    eGetFlashBlockProtection,  /*!< Get block protection. takes the input of IotFlashWriteProtectConfig_t type
                                * and updates the xProtectionLevel for the address passed. ulSize in the input is ignored*/
    eSuspendFlashProgramErase, /*!< Suspends any program or erase operations. This may be useful
                                * if there is an high priority read that needs to happen
                                * when program or erase may be in progress. Ex: XiP access*/
    eResumeFlashProgramErase,  /*!< Resumes any program or erase operation that was previously
                                * Suspended using eSuspendFlashProgramErase IOCTL */
    eGetFlashStatus,           /*!< Get flash status. Return IotFlashStatus_t type */
    eGetFlashTxNoOfbytes,      /*!< Get the number of bytes sent in write operation. */
    eGetFlashRxNoOfbytes,      /*!< Get the number of bytes received in read operation. */
} IotFlashIoctlRequest_t;

/**
 * @brief   flash descriptor type defined in the source file.
 */
struct                        IotFlashDescriptor;

/**
 * @brief   IotFlashHandle_t type is the flash handle returned by calling iot_flash_open()
 *          this is initialized in open and returned to caller. Caller must pass this pointer
 *          to the rest of the APIs.
 */
typedef struct IotFlashDescriptor   * IotFlashHandle_t;

/**
 * @brief   Flash notification callback type. This callback can be passed
 *          to the driver by using iot_flash_set_callback API (the callback is used
 *          only if flash supports asynchronous operations, which can be checked from
 *          flashInfo.ucAsyncSupported value.
 *
 * @param[out]  xStatus    Flash asynchronous operation status.
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used or modified by the driver. The context
 *                          is provided by the caller when setting the callback, and is
 *                          passed back to the caller in the callback.
 */
typedef void ( * IotFlashCallback_t)( IotFlashOperationStatus_t xStatus, void * pvUserContext );

/*!
 * @brief   iot_flash_open is used to initialize the flash device.
 *          This must be called before using any other flash APIs.
 *          This function Initializes the peripheral, configures buses etc.
 *
 * @param[in]   lFlashInstance  The instance of the flash to initialize.
 *                              If there are more than one flash per device, the
 *                              first flash is 0, the second flash is 1 etc.
 *
 * @return  returns the handle IotFlashHandle_t on success, or NULL on failure.
 */
IotFlashHandle_t iot_flash_open( int32_t lFlashInstance );

/*!
 * @brief   iot_flash_getinfo is used to get the information
 *          about the physical flash device.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 *
 * @return  returns the pointer to flash information structure IotFlashInfo_t
 *          on success, or NULL on failure.
 */
IotFlashInfo_t * iot_flash_getinfo( IotFlashHandle_t const pxFlashHandle );

/*!
 * @brief   iot_flash_set_callback is used to set the callback to be called upon completion
 *          of erase/program/read. The callback is used only if the underlying HW supports
 *          asynchronous operations. Caller can check if asynchronous operations are
 *          supported by checking the "ucAsyncSupported" flag in IotFlashInfo_t structure.
 *          If asynchronous operations are not supported, then erase/write/read operations
 *          are blocking operations, and this API has no affect, i.e even if a callback is set,
 *          it will never be called back.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   xCallback       callback function to be called.
 * @param[in]   pvUserContext   user context to be passed when callback is called.
 *
 * @return      None
 */
void iot_flash_set_callback( IotFlashHandle_t const pxFlashHandle,
                             IotFlashCallback_t xCallback,
                             void * pvUserContext );

/*!
 * @brief   iot_flash_ioctl is used to configure the flash parameters and setup
 *          certain flash operations and also used to read flash info and configuration.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   xRequest        configuration request of type IotFlashIoctlRequest_t
 * @param[in,out] pvBuffer      configuration values to be written to flash or to be read from flash.
 *
 * @return  returns IOT_FLASH_SUCCESS on success or returns
 *          one of IOT_FLASH_INVALID_VALUE, IOT_FLASH_DEVICE_BUSY,IOT_FLASH_CTRL_OP_FAILED,
 *          IOT_FLASH_FUNCTION_NOT_SUPPORTED on error.
 */
int32_t iot_flash_ioctl( IotFlashHandle_t const pxFlashHandle,
                         IotFlashIoctlRequest_t xRequest,
                         void * const pvBuffer );

/*!
 * @brief   iot_flash_erase_sectors is used to erase data in flash from the start
 *          of the address specified until the startAddress plus size passed.
 *          The address passed in 'ulAddress' must be aligned to ulSectorSize,
 *          and size must be a multiple of ulSectorSize. If there is another flash operation
 *          is in progress, the erase_sectors API will return an error and this usually happens if
 *          flash supports asynchronous erase/write/read operations.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   ulStartAddress  starting address(offset) in flash, from where erase starts. Aligned to ulSectorSize
 * @param[in]   xSize           size of the flash range to be erased. multiple of ulSectorSize
 *
 * @return  returns IOT_FLASH_SUCCESS on success or returns
 *          one of IOT_FLASH_INVALID_VALUE, IOT_FLASH_DEVICE_BUSY, IOT_FLASH_ERASE_FAILED on error.
 */
int32_t iot_flash_erase_sectors( IotFlashHandle_t const pxFlashHandle,
                                 uint32_t ulStartAddress,
                                 size_t xSize );

/*!
 * @brief   iot_erase_chip is used to erase the entire flash chip.
 *          If there is another flash operation is in progress, the erase_chip API will return an error.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 *
 * @return  returns IOT_FLASH_SUCCESS on success or returns
 *          one of IOT_FLASH_INVALID_VALUE, IOT_FLASH_DEVICE_BUSY, IOT_FLASH_ERASE_FAILED on error.
 */
int32_t iot_flash_erase_chip( IotFlashHandle_t const pxFlashHandle );

/*!
 * @brief   iot_flash_write_sync is used to write data to flash starting at the address provided.
 *          The sector(s) being written to, must be erased first before any write can take place.
 *          This is a blocking operation and waits until the number of bytes are written before returning.
 *          If there is another flash operation is in progress, write will return an error.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   ulAddress       starting address(offset) in flash to write.
 * @param[in]   xBytes          number of bytes to write.
 * @param[in]   pvBuffer        data buffer to write to flash
 *
 * @return  returns IOT_FLASH_SUCCESS on success or returns
 *          one of IOT_FLASH_INVALID_VALUE, IOT_FLASH_DEVICE_BUSY, IOT_FLASH_WRITE_FAILED on error.
 */
int32_t iot_flash_write_sync( IotFlashHandle_t const pxFlashHandle,
                              uint32_t ulAddress,
                              uint8_t * const pvBuffer,
                              size_t xBytes );

/*!
 * @brief   iot_flash_read_sync is used to read data from flash. This is a blocking operation
 *          and waits until the number of bytes are read before returning.
 *          If there is another flash operation is in progress, this will return an error.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   ulAddress       starting address(offset) in flash to read.
 * @param[in]   xBytes          number of bytes to be read.
 * @param[out]  pvBuffer        data buffer to hold the data read from flash
 *
 * @return  returns IOT_FLASH_SUCCESS on success or returns
 *          one of IOT_FLASH_INVALID_VALUE, IOT_FLASH_DEVICE_BUSY, IOT_FLASH_READ_FAILED on error.
 */
int32_t iot_flash_read_sync( IotFlashHandle_t const pxFlashHandle,
                             uint32_t ulAddress,
                             uint8_t * const pvBuffer,
                             size_t xBytes );

/*!
 * @brief   iot_flash_write_async is used to write data to flash starting at the address provided.
 *          The sector(s) being written to, must be erased first before any write can take place.
 *          This is an asynchronous (non-blocking) operation and returns as soon as the write
 *          operation is started. When the write is completed, user callback is called to
 *          notify that the write is complete. The caller can check the status of the operation
 *          by using eGetStatus IOCTL. User must register for a callback when using the non-blocking
 *          operations to know when they are complete.
 *          If there is another flash operation is in progress, write will return an error.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   ulAddress       starting address(offset) in flash to write.
 * @param[in]   xBytes          number of bytes to write.
 * @param[in]   pvBuffer        data buffer to write to flash
 *
 * @return  returns IOT_FLASH_SUCCESS on success or returns
 *          one of IOT_FLASH_INVALID_VALUE, IOT_FLASH_DEVICE_BUSY, IOT_FLASH_WRITE_FAILED on error
 *          IOT_FLASH_FUNCTION_NOT_SUPPORTED is returned if asynchronous is not supported (i,e ucAsyncSupported is set to false)
 */
int32_t iot_flash_write_async( IotFlashHandle_t const pxFlashHandle,
                               uint32_t ulAddress,
                               uint8_t * const pvBuffer,
                               size_t xBytes );

/*!
 * @brief   iot_flash_read_async is used to read data from flash.
 *          This is an asynchronous (non-blocking) operation and returns as soon as the read
 *          operation is started. When the read is completed, user callback is called to
 *          notify that the read is complete. The caller can check the status of the operation
 *          by using eGetStatus IOCTL and use the buffer. User must register for a callback when
 *          using the non-blocking operations to know when they are complete.
 *          If there is another flash operation is in progress, this will return an error.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 * @param[in]   ulAddress       starting address(offset) in flash to read.
 * @param[in]   xBytes          number of bytes to be read.
 * @param[out]  pvBuffer        data buffer to hold the data read from flash
 *
 * @return  returns IOT_FLASH_SUCCESS on success or returns
 *          one of IOT_FLASH_INVALID_VALUE, IOT_FLASH_DEVICE_BUSY, IOT_FLASH_READ_FAILED on error
 *          IOT_FLASH_FUNCTION_NOT_SUPPORTED is returned if asynchronous is not supported (i,e ucAsyncSupported is set to false)
 */
int32_t iot_flash_read_async( IotFlashHandle_t const pxFlashHandle,
                              uint32_t ulAddress,
                              uint8_t * const pvBuffer,
                              size_t xBytes );

/*!
 * @brief   iot_flash_close is used to close the flash device.
 *          If any operations are in progress when close is called,
 *          flash driver aborts those if possible.
 *
 * @param[in]   pxFlashHandle    handle to flash driver returned in
 *                              iot_flash_open()
 *
 * @return  returns IOT_FLASH_SUCCESS on success close or returns
 *          one of IOT_FLASH_INVALID_VALUE on error.
 */
int32_t iot_flash_close( IotFlashHandle_t const pxFlashHandle );

/**
 * @}
 */

#endif /* ifndef _IOT_FLASH_H_ */
