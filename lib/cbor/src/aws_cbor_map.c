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

void CBOR_OpenMap(cbor_handle_t pxCbor_data)
{
    assert(NULL != pxCbor_data);

    CBOR_AssignAndIncrementCursor(pxCbor_data, CBOR_MAP_OPEN);
}

void CBOR_CloseMap(cbor_handle_t pxCbor_data)
{
    assert(NULL != pxCbor_data);

    CBOR_AssignAndIncrementCursor(pxCbor_data, CBOR_BREAK);
    pxCbor_data->map_end = pxCbor_data->cursor - 1;
}

bool CBOR_KeyIsMatch(cbor_handle_t pxCbor_data, const char *pcKey)
{
    assert(NULL != pxCbor_data);
    assert(NULL != pcKey);

    bool xIs_match = false;
    if (0 == CBOR_StringCompare(pxCbor_data, pcKey)) {
        xIs_match = true;
    }
    return xIs_match;
}
void CBOR_SearchForKey(cbor_handle_t pxCbor_data, const char *pcKey)
{
    assert(NULL != pxCbor_data);
    assert(NULL != pcKey);
    assert(*(pxCbor_data->cursor) != 0);
    assert(*(pxCbor_data->cursor) != CBOR_MAP_OPEN);

    while (!((CBOR_BREAK == *(pxCbor_data->cursor))
             || true == CBOR_KeyIsMatch(pxCbor_data, pcKey))) {
        CBOR_NextKey(pxCbor_data);
    }
}
void CBOR_AppendKey(cbor_handle_t pxCbor_data, const char *pcKey,
    write_function_t pxWrite_function, const void *pxValue)
{
    assert(NULL != pxCbor_data);
    assert(NULL != pcKey);
    assert(NULL != pxWrite_function);
    assert(NULL != pxValue);

    pxCbor_data->cursor = pxCbor_data->map_end;
    /* Key not found, at end of map */
    assert(CBOR_BREAK == *(pxCbor_data->cursor));
    /* This overwrites map close... */
    CBOR_WriteString(pxCbor_data, pcKey);
    CBOR_CloseMap(pxCbor_data);
    pxCbor_data->cursor--;

    pxWrite_function(pxCbor_data, pxValue);

    /* ...So need to reclose it here. */
    CBOR_CloseMap(pxCbor_data);
}

void CBOR_AssignKey(cbor_handle_t pxCbor_data, const char *pcKey,
    write_function_t pxWrite_function, const void *pxValue)
{
    assert(NULL != pxCbor_data);
    assert(NULL != pcKey);
    assert(NULL != pxWrite_function);
    assert(NULL != pxValue);

    (pxCbor_data->err)    = eCBOR_ERR_DEFAULT_ERROR;
    (pxCbor_data->cursor) = (pxCbor_data->buffer_start);

    if (CBOR_MAP_OPEN == *(pxCbor_data->cursor)) {
        CBOR_OpenMap(pxCbor_data);
        CBOR_SearchForKey(pxCbor_data, pcKey);

        if (CBOR_KeyIsMatch(pxCbor_data, pcKey)) {
            /* Key was found */
            CBOR_Next(pxCbor_data);
            pxWrite_function(pxCbor_data, pxValue);
        } else {
            CBOR_AppendKey(pxCbor_data, pcKey, pxWrite_function, pxValue);
        }
    } else {
        (pxCbor_data->err) = eCBOR_ERR_UNSUPPORTED_READ_OPERATION;
    }
}

cbor_ssize_t CBOR_MapSize(const cbor_byte_t *pxPtr)
{
    assert(NULL != pxPtr);

    const cbor_byte_t *pxCursor_start = pxPtr;
    while (CBOR_BREAK != *(pxPtr)) {
        pxPtr = CBOR_NextKeyPtr(pxPtr);
    }
    cbor_ssize_t xSize = pxPtr - pxCursor_start + 1;
    return xSize;
}

void CBOR_WriteMap(cbor_handle_t pxCbor_data, const void *pxInput)
{
    assert(NULL != pxCbor_data);
    assert(NULL != pxInput);

    const struct CborData_s *pxMap       = pxInput;
    const cbor_byte_t *      pxMap_start = pxMap->buffer_start;

    cbor_ssize_t xMap_size = CBOR_MapSize(pxMap_start);

    CBOR_ValueResize(pxCbor_data, xMap_size);

    CBOR_MemCopy(pxCbor_data, pxMap_start, xMap_size);
}

cbor_handle_t CBOR_ReadMap(cbor_handle_t pxCbor_data)
{
    assert(NULL != pxCbor_data);

    cbor_byte_t xData_head  = *(pxCbor_data->cursor);
    cbor_byte_t xMajor_type = xData_head & CBOR_MAJOR_TYPE_MASK;
    if (CBOR_MAP != xMajor_type) {
        pxCbor_data->err = eCBOR_ERR_READ_TYPE_MISMATCH;
        return NULL;
    }
    cbor_handle_t pxMap = CBOR_New(0);
    if (NULL == pxMap) {
        pxCbor_data->err = eCBOR_ERR_INSUFFICENT_SPACE;
        return NULL;
    };
    cbor_ssize_t xMap_size = CBOR_MapSize(pxCbor_data->cursor);
    CBOR_MemCopy(pxMap, pxCbor_data->cursor, xMap_size);
    pxMap->map_end = pxMap->cursor - 1;
    return pxMap;
}
