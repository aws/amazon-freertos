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
#include <stdlib.h>
#include <string.h>

/** Minimum size (in bytes) that the CBOR buffer shall start with */
#define CBOR_MIN_BUFFER_SIZE (32)

cbor_handle_t CBOR_New(cbor_ssize_t xSize)
{
    cbor_handle_t pxNew_cbor_data = pxCBOR_malloc(sizeof(struct CborData_s));
    if (NULL == pxNew_cbor_data) {
        return NULL;
    }

    xSize = xSize < CBOR_MIN_BUFFER_SIZE ? CBOR_MIN_BUFFER_SIZE : xSize;
    cbor_byte_t *pxNew_buffer = pxCBOR_malloc(xSize);
    if (NULL == pxNew_buffer) {
        pxCBOR_free(pxNew_cbor_data);
        return NULL;
    }

    cbor_byte_t xEmpty_map[2] = {CBOR_MAP_OPEN, CBOR_BREAK};
    memcpy(pxNew_buffer, xEmpty_map, 2);
    pxNew_cbor_data->buffer_start = pxNew_buffer;
    pxNew_cbor_data->cursor       = pxNew_buffer;
    pxNew_cbor_data->buffer_end   = &pxNew_buffer[xSize - 1];
    pxNew_cbor_data->map_end      = &pxNew_buffer[1];

    return pxNew_cbor_data;
}

void CBOR_Delete(cbor_handle_t *ppxHandle)
{
    if (NULL == ppxHandle) {
        return;
    }
    if (NULL == *ppxHandle) {
        return;
    }
    pxCBOR_free((*ppxHandle)->buffer_start);
    pxCBOR_free(*ppxHandle);
    *ppxHandle = NULL;
}

cbor_byte_t const *const CBOR_GetRawBuffer(cbor_handle_t pxHandle)
{
    return pxHandle->buffer_start;
}

cbor_ssize_t const CBOR_GetBufferSize(cbor_handle_t pxHandle)
{
    return pxHandle->map_end - pxHandle->buffer_start + 1;
}

bool CBOR_FindKey(cbor_handle_t pxCbor_data, cbor_const_key_t pcKey)
{
    if (NULL == pxCbor_data) {
        return false;
    }
    if (NULL == pcKey) {
        pxCbor_data->err = eCBOR_ERR_NULL_KEY;
        return false;
    }

    (pxCbor_data->cursor) = (pxCbor_data->buffer_start);
    CBOR_OpenMap(pxCbor_data);
    CBOR_SearchForKey(pxCbor_data, pcKey);
    bool xFound = false;
    if (CBOR_KeyIsMatch(pxCbor_data, pcKey)) {
        xFound = true;
        CBOR_Next(pxCbor_data);
    }
    return xFound;
}

void CBOR_AssignKeyWithString(
    cbor_handle_t pxCbor_data, const char *pcKey, const char *pcValue)
{
    if (NULL == pxCbor_data) {
        return;
    }
    if (NULL == pcKey) {
        pxCbor_data->err = eCBOR_ERR_NULL_KEY;
        return;
    }
    if (NULL == pcValue) {
        pxCbor_data->err = eCBOR_ERR_NULL_VALUE;
        return;
    }

    CBOR_AssignKey(pxCbor_data, pcKey, CBOR_WriteString, pcValue);
}

void CBOR_AppendKeyWithString(
    cbor_handle_t pxCbor_data, const char *pcKey, const char *pcValue)
{
    if (NULL == pxCbor_data) {
        return;
    }
    if (NULL == pcKey) {
        pxCbor_data->err = eCBOR_ERR_NULL_KEY;
        return;
    }
    if (NULL == pcValue) {
        pxCbor_data->err = eCBOR_ERR_NULL_VALUE;
        return;
    }

    CBOR_AppendKey(pxCbor_data, pcKey, CBOR_WriteString, pcValue);
}

char *CBOR_FromKeyReadString(cbor_handle_t pxCbor_data, const char *pcKey)
{
    if (NULL == pxCbor_data) {
        return NULL;
    }
    if (NULL == pcKey) {
        pxCbor_data->err = eCBOR_ERR_NULL_KEY;
        return NULL;
    }

    CBOR_FindKey(pxCbor_data, pcKey);
    char *pcStr = CBOR_ReadString(pxCbor_data);
    return pcStr;
}

void CBOR_AssignKeyWithInt(
    cbor_handle_t pxCbor_data, const char *pcKey, cbor_int_t xValue)
{
    if (NULL == pxCbor_data) {
        return;
    }
    if (NULL == pcKey) {
        pxCbor_data->err = eCBOR_ERR_NULL_KEY;
        return;
    }

    CBOR_AssignKey(pxCbor_data, pcKey, CBOR_WriteInt, &xValue);
}

void CBOR_AppendKeyWithInt(
    cbor_handle_t pxCbor_data, const char *pcKey, cbor_int_t xValue)
{
    if (NULL == pxCbor_data) {
        return;
    }
    if (NULL == pcKey) {
        pxCbor_data->err = eCBOR_ERR_NULL_KEY;
        return;
    }

    CBOR_AppendKey(pxCbor_data, pcKey, CBOR_WriteInt, &xValue);
}

cbor_int_t CBOR_FromKeyReadInt(cbor_handle_t pxCbor_data, const char *pcKey)
{
    if (NULL == pxCbor_data) {
        return 0;
    }
    if (NULL == pcKey) {
        pxCbor_data->err = eCBOR_ERR_NULL_KEY;
        return 0;
    }

    CBOR_FindKey(pxCbor_data, pcKey);
    cbor_int_t xValue = CBOR_ReadInt(pxCbor_data);
    return xValue;
}

void CBOR_AssignKeyWithMap(
    cbor_handle_t pxCbor_data, const char *pcKey, cbor_handle_t pxValue)
{
    if (NULL == pxCbor_data) {
        return;
    }
    if (NULL == pcKey) {
        pxCbor_data->err = eCBOR_ERR_NULL_KEY;
        return;
    }
    if (NULL == pxValue) {
        pxCbor_data->err = eCBOR_ERR_NULL_VALUE;
        return;
    }

    CBOR_AssignKey(pxCbor_data, pcKey, CBOR_WriteMap, pxValue);
}

void CBOR_AppendKeyWithMap(
    cbor_handle_t pxCbor_data, const char *pcKey, cbor_handle_t pxValue)
{
    if (NULL == pxCbor_data) {
        return;
    }
    if (NULL == pcKey) {
        pxCbor_data->err = eCBOR_ERR_NULL_KEY;
        return;
    }
    if (NULL == pxValue) {
        pxCbor_data->err = eCBOR_ERR_NULL_VALUE;
        return;
    }

    CBOR_AppendKey(pxCbor_data, pcKey, CBOR_WriteMap, pxValue);
}

cbor_handle_t CBOR_FromKeyReadMap(cbor_handle_t pxCbor_data, const char *pcKey)
{
    if (NULL == pxCbor_data) {
        return NULL;
    }
    if (NULL == pcKey) {
        pxCbor_data->err = eCBOR_ERR_NULL_KEY;
        return NULL;
    }

    CBOR_FindKey(pxCbor_data, pcKey);
    cbor_handle_t pxMap = CBOR_ReadMap(pxCbor_data);
    return pxMap;
}

void CBOR_AppendMap(cbor_handle_t pxDest, cbor_handle_t pxSrc)
{
    pxSrc->cursor         = pxSrc->buffer_start + 1;
    cbor_ssize_t xLength = pxSrc->buffer_end - pxSrc->cursor;
    pxDest->cursor        = pxDest->map_end;
    CBOR_MemCopy(pxDest, pxSrc->cursor, xLength);
    pxDest->map_end = pxDest->cursor - 1;
}

cbor_err_t CBOR_CheckError(cbor_handle_t pxCbor_data)
{
    if (NULL == pxCbor_data) {
        return eCBOR_ERR_NULL_HANDLE;
    }
    return pxCbor_data->err;
}

void CBOR_ClearError(cbor_handle_t pxCbor_data)
{
    if (NULL == pxCbor_data) {
        return;
    }
    pxCbor_data->err = eCBOR_ERR_NO_ERROR;
}

/*
 * End of File
 */
