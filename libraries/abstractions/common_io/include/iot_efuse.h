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
 * @file iot_efuse.h
 * @brief File for the APIs of efuse called by application layer.
 */
#ifndef _IOT_EFUSE_H_
#define _IOT_EFUSE_H_

/**
 * @defgroup iot_efuse EFUSE Abstraction APIs.
 * @{
 */

/**
 * The return codes for the functions in EFUSE.
 */
#define IOT_EFUSE_SUCCESS                   ( 0 )      /*!< Efuse operation completed successfully. */
#define IOT_EFUSE_INVALID_VALUE             ( 1 )      /*!< At least one parameter is invalid. */
#define IOT_EFUSE_READ_FAIL                 ( 2 )      /*!< Error reading Efuse value. */
#define IOT_EFUSE_WRITE_FAIL                ( 3 )      /*!< Error writing Efuse value. */
#define IOT_EFUSE_CLOSE_FAIL                ( 4 )      /*!< Error closing Efuse instance. */
#define IOT_EFUSE_FUNCTION_NOT_SUPPORTED    ( 5 )      /*!< Efuse operation not supported. */
#define IOT_EFUSE_ERROR                     ( 6 )      /*!< Error performing Efuse operation. */

/**
 * @brief The Efuse descriptor type defined in the source file.
 */
struct                      IotEfuseDescriptor;

/**
 * @brief IotEfuseHandle_t is the handle type returned by calling iot_efuse_open().
 *        This is initialized in open and returned to caller. The caller must pass
 *        this pointer to the rest of APIs.
 */
typedef struct IotEfuseDescriptor * IotEfuseHandle_t;

/**
 * @brief iot_efuse_open is used to Initialize things needed to access efuse.
 *
 * @return
 *   - Handle to IotEfuseHandle_t on success
 *   -  NULL if the handle is already opened. handle must be closed before calling open again
 */
IotEfuseHandle_t iot_efuse_open( void );

/**
 * @brief iot_efuse_close is used to de Initialize things needed to disable efuse access.
 *
 * @param[in] pxEfuseHandle handle to efuse interface returned in iot_efuse_open()
 *
 * @return
 *   - IOT_EFUSE_SUCCESS if succeeded,
 *   - IOT_EFUSE_INVALID_VALUE on NULL pxEfuseHandle
 *   - IOT_EFUSE_INVALID_VALUE if instance not previously opened.
 *   - IOT_EFUSE_CLOSE_FAIL if the underneath HW deinit api returns fail.
 */
int32_t iot_efuse_close( IotEfuseHandle_t const pxEfuseHandle );

/**
 * @brief Read 32-bit efuse word from specified index.
 *
 * @param[in] pxEfuseHandle handle to efuse interface returned in iot_efuse_open()
 * @param[in] ulIndex index of efuse word to read. Caller must know the underlying
 *            efuse mechanism and make sure index is a valid one.
 * @param[out] ulValue The receive buffer to read the data into
 *
 * @return
 *   - IOT_EFUSE_SUCCESS if read succeeded,
 *   - IOT_EFUSE_READ_FAIL if read failed,
 *   - IOT_EFUSE_INVALID_VALUE if pxEfuseHandle is NULL,  index is invalid, or ulValue is NULL.
 *   - IOT_EFUSE_FUNCTION_NOT_SUPPORTED if 32-bit efuse word is not supported
 */
int32_t iot_efuse_read_32bit_word( IotEfuseHandle_t const pxEfuseHandle,
                                   uint32_t ulIndex,
                                   uint32_t * ulValue );

/**
 * @brief Write 32-bit value to the 32-bit efuse word at specified index.
 *
 * @param[in] pxEfuseHandle handle to efuse interface returned in iot_efuse_open()
 * @param[in] ulIndex index of efuse word to write to. Caller must know the underlying
 *            efuse mechanism and make sure index is a valid one.
 * @param[in] ulValue The 32-bit value to write.
 *
 * @return
 *   - IOT_EFUSE_SUCCESS if write succeeded,
 *   - IOT_EFUSE_WRITE_FAIL if write failed
 *   - IOT_EFUSE_INVALID_VALUE if pxEfuseHandle is NULL, or index is invalid.
 *   - IOT_EFUSE_FUNCTION_NOT_SUPPORTED if 32-bit efuse word is not supported
 */
int32_t iot_efuse_write_32bit_word( IotEfuseHandle_t const pxEfuseHandle,
                                    uint32_t ulIndex,
                                    uint32_t ulValue );

/**
 * @brief Read 16-bit efuse word from specified index.
 *
 * @param[in] pxEfuseHandle handle to efuse interface returned in iot_efuse_open()
 * @param[in] ulIndex index of efuse word to read. Caller must know the underlying
 *            efuse mechanism and make sure index is a valid one.
 * @param[out] ulValue The receive buffer to read the data into
 *
 * @return
 *   - IOT_EFUSE_SUCCESS if read succeeded,
 *   - IOT_EFUSE_READ_FAIL if read failed
 *   - IOT_EFUSE_INVALID_VALUE if pxEfuseHandle or ulValue is NULL, or index is invalid
 *   - IOT_EFUSE_FUNCTION_NOT_SUPPORTED if 16-bit efuse word is not supported
 */
int32_t iot_efuse_read_16bit_word( IotEfuseHandle_t const pxEfuseHandle,
                                   uint32_t ulIndex,
                                   uint16_t * ulValue );

/**
 * @brief Write 16-bit value to the 16-bit efuse word at specified index.
 *
 * @param[in] pxEfuseHandle handle to efuse interface returned in iot_efuse_open()
 * @param[in] ulIndex index of efuse word to write to. Caller must know the underlying
 *            efuse mechanism and make sure index is a valid one.
 * @param[in] ulValue The 16-bit value to write.
 *
 * @return
 *   - IOT_EFUSE_SUCCESS if write succeeded,
 *   - IOT_EFUSE_WRITE_FAIL if write failed
 *   - IOT_EFUSE_INVALID_VALUE if index is invalid, or pxEfuseHandle is NULL.
 *   - IOT_EFUSE_FUNCTION_NOT_SUPPORTED if 16-bit efuse word is not supported
 */
int32_t iot_efuse_write_16bit_word( IotEfuseHandle_t const pxEfuseHandle,
                                    uint32_t ulIndex,
                                    uint16_t value );

/**
 * @}
 */
/* end of group iot_efuse */

#endif /* ifndef _IOT_EFUSE_H_ */
