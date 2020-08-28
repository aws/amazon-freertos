/*
 * Amazon FreeRTOS CELLULAR Preview Release
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

#ifndef __IOT_FIFO_H__
#define __IOT_FIFO_H__

/* C standard library includes. */
#include <stdbool.h>
#include <stdint.h>

#include "iot_config.h"

/**
 * @brief Circular FIFO structure definition.
 */
typedef struct
{
    uint32_t ulHead;        /**< Heading index */
    uint32_t ulTail;        /**< Tailing index */
    uint32_t ulLength;      /**< Number of items */
    uint32_t ulItemSize;    /**< Size of item */
    void * pBuffer;         /**< Internal buffer */
} IotFifo_t;

/**
 * @brief Initialize the FIFO.
 *
 * @param[in] pFifo The FIFO to be initialized.
 * @param[in] pBuffer The FIFO buffer to be used.
 * @param[in] ulLength The number of items to be allocated.
 * @param[in] ulItemSize The size of single item.
 */
void IotFifo_Init( IotFifo_t * pFifo, void * pBuffer, uint32_t ulLength, uint32_t ulItemSize );

/**
 * @brief De-initialize the FIFO.
 *
 * @param[in] pFifo The FIFO to be de-initialized.
 *
 */
void IotFifo_DeInit( IotFifo_t * pFifo );

/**
 * @brief Puts an item at the head of the FIFO.
 *
 * @param[in] pFifo The FIFO.
 * @param[in] pData The item to put in the FIFO.
 *
 * @return 1 if the operation is successful, 0 if the FIFO is full
 */
bool IotFifo_Put( IotFifo_t * pFifo, const void * pData );

/**
 * @brief Gets an item at the tail of the FIFO.
 *
 * @param[in] pFifo The FIFO.
 * @param[in] pData The item to get out the FIFO.
 *
 * @return 1 if the operation is successful, 0 if the FIFO is empty
 */
bool IotFifo_Get( IotFifo_t * pFifo, void * pData );

#endif /* __IOT_FIFO_H__ */
