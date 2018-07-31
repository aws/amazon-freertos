/*
 * Amazon FreeRTOS CBOR Library V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
#include "aws_cbor_internals.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/**
 * @brief reads the specified byte from the number
 *
 * Reads the byte specified by the index from the number, with 1 being the most
 * significant and n being the least significant (where n is the number of
 * bytes in the integer).
 *
 * @param  number Number to extract byte from
 * @param  index  Byte to extract (1 being most significant)
 * @return        The extracted byte
 */
#define CBOR_ReadByte(number, index)                                           \
    number >> (CBOR_BYTE_WIDTH * (sizeof(number) - (index)))

void CBOR_WriteInt(cbor_handle_t pxCbor_data, const void *pxInput)
{
    assert(NULL != pxCbor_data);
    assert(NULL != pxInput);

    cbor_int_t xNum = *(cbor_int_t *)pxInput;

    if (CBOR_IsSmallInt(xNum)) {
        CBOR_ValueResize(pxCbor_data, CBOR_SMALL_INT_SIZE);
        CBOR_WriteSmallInt(pxCbor_data, xNum);
    } else if (CBOR_Is8BitInt(xNum)) {
        CBOR_ValueResize(pxCbor_data, CBOR_INT8_SIZE);
        CBOR_WriteInt8(pxCbor_data, (cbor_byte_t)xNum);
    } else if (CBOR_Is16BitInt(xNum)) {
        CBOR_ValueResize(pxCbor_data, CBOR_INT16_SIZE);
        CBOR_WriteInt16(pxCbor_data, (uint16_t)xNum);
    } else if (CBOR_Is32BitInt(xNum)) {
        CBOR_ValueResize(pxCbor_data, CBOR_INT32_SIZE);
        CBOR_WriteInt32(pxCbor_data, (uint32_t)xNum);
    }
}

cbor_ssize_t CBOR_IntSize(const cbor_byte_t *pxPtr)
{
    assert(NULL != pxPtr);

    cbor_byte_t xData_head  = *(pxPtr);
    cbor_byte_t xMajor_type = xData_head & CBOR_MAJOR_TYPE_MASK;
    assert(xMajor_type == CBOR_POS_INT || xMajor_type == CBOR_NEG_INT);
    cbor_byte_t xAdditional_detail = xData_head & CBOR_ADDITIONAL_DATA_MASK;

    cbor_ssize_t xSize = 0;
    if (CBOR_IsSmallInt(xAdditional_detail)) {
        xSize = CBOR_SMALL_INT_SIZE;
    } else if (CBOR_INT8_FOLLOWS == xAdditional_detail) {
        xSize = CBOR_INT8_SIZE;
    } else if (CBOR_INT16_FOLLOWS == xAdditional_detail) {
        xSize = CBOR_INT16_SIZE;
    } else if (CBOR_INT32_FOLLOWS == xAdditional_detail) {
        xSize = CBOR_INT32_SIZE;
    }
    return xSize;
}

void CBOR_WriteInt32(cbor_handle_t pxCbor_data, uint32_t ulNum)
{
    assert(NULL != pxCbor_data);

    CBOR_AssignAndIncrementCursor(pxCbor_data, CBOR_INT32_FOLLOWS);

    for (cbor_int_t xI = 1; xI <= sizeof(ulNum); xI++) {
        cbor_byte_t xByte = CBOR_ReadByte(ulNum, xI);
        CBOR_AssignAndIncrementCursor(pxCbor_data, xByte);
    }
}

void CBOR_WriteInt16(cbor_handle_t pxCbor_data, uint16_t usNum)
{
    assert(NULL != pxCbor_data);

    CBOR_AssignAndIncrementCursor(pxCbor_data, CBOR_INT16_FOLLOWS);

    for (cbor_int_t xI = 1; xI <= sizeof(usNum); xI++) {
        cbor_byte_t xByte = CBOR_ReadByte(usNum, xI);
        CBOR_AssignAndIncrementCursor(pxCbor_data, xByte);
    }
}

void CBOR_WriteInt8(cbor_handle_t pxCbor_data, cbor_byte_t xNum)
{
    assert(NULL != pxCbor_data);

    CBOR_AssignAndIncrementCursor(pxCbor_data, CBOR_INT8_FOLLOWS);
    CBOR_AssignAndIncrementCursor(pxCbor_data, xNum);
}

void CBOR_WriteSmallInt(cbor_handle_t pxCbor_data, cbor_byte_t xNum)
{
    assert(NULL != pxCbor_data);

    CBOR_AssignAndIncrementCursor(pxCbor_data, xNum);
}

cbor_int_t CBOR_ReadInt(cbor_handle_t pxCbor_data)
{
    assert(NULL != pxCbor_data);

    cbor_byte_t xData_head  = *(pxCbor_data->cursor);
    cbor_byte_t xMajor_type = xData_head & CBOR_MAJOR_TYPE_MASK;

    if (CBOR_POS_INT != xMajor_type) {
        pxCbor_data->err = eCBOR_ERR_READ_TYPE_MISMATCH;
    }

    cbor_int_t  xNum               = 0;
    cbor_byte_t xAdditional_detail = xData_head & CBOR_ADDITIONAL_DATA_MASK;
    switch (xAdditional_detail) {
    case CBOR_INT32_FOLLOWS:
        xNum = CBOR_ReadPositiveInt32(pxCbor_data);
        break;

    case CBOR_INT16_FOLLOWS:
        xNum = CBOR_ReadPositiveInt16(pxCbor_data);
        break;

    case CBOR_INT8_FOLLOWS:
        xNum = CBOR_ReadPositiveInt8(pxCbor_data);
        break;

    default:
        xNum = xAdditional_detail;
        break;
    }

    return xNum;
}

uint32_t CBOR_ReadPositiveInt32(cbor_handle_t pxCbor_data)
{
    assert(NULL != pxCbor_data);

    uint32_t     ulNum = 0;
    cbor_byte_t *pxPtr = pxCbor_data->cursor + 1;

    // Scan byte by byte and reassemble the integer
    for (cbor_int_t xI = 0; xI < sizeof(ulNum); xI++) {
        cbor_byte_t xByte = *(pxPtr)++;
        ulNum <<= CBOR_BYTE_WIDTH;
        ulNum += xByte;
    }

    return ulNum;
}

uint16_t CBOR_ReadPositiveInt16(cbor_handle_t pxCbor_data)
{
    assert(NULL != pxCbor_data);

    uint16_t     usNum = 0;
    cbor_byte_t *pxPtr = pxCbor_data->cursor + 1;

    // Scan byte by byte and reassemble the integer
    for (cbor_int_t xI = 0; xI < sizeof(usNum); xI++) {
        cbor_byte_t xByte = *(pxPtr)++;
        usNum <<= CBOR_BYTE_WIDTH;
        usNum += xByte;
    }

    return usNum;
}

cbor_byte_t CBOR_ReadPositiveInt8(cbor_handle_t pxCbor_data)
{
    assert(NULL != pxCbor_data);

    return pxCbor_data->cursor[1];
}

cbor_byte_t CBOR_ReadSmallPositiveInt(cbor_handle_t pxCbor_data)
{
    assert(NULL != pxCbor_data);

    return *pxCbor_data->cursor;
}
