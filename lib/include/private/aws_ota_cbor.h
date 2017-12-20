/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#ifndef __AWS_OTACBOR__H__
#define __AWS_OTACBOR__H__


/**
 * @brief Decode a Get Stream response message from AWS IoT OTA.
 */
BaseType_t OTA_CBOR_Decode_GetStreamResponseMessage(
    uint8_t *pucMessageBuffer,
    size_t xMessageSize,
    int *plFileId,
    int *plBlockId,
    int *plBlockSize,
    uint8_t **ppucPayload,
    size_t *pxPayloadSize );

/**
 * @brief Create an encoded Get Stream Request message for the AWS IoT OTA
 * service.
 */
BaseType_t OTA_CBOR_Encode_GetStreamRequestMessage(
    uint8_t *pucMessageBuffer,
    size_t xMessageBufferSize,
    size_t *pxEncodedMessageSize,
    char *pcClientToken,
    int lFileId,
    int lBlockSize,
    int lBlockOffset,
    uint8_t *pucBlockBitmap,
    size_t xBlockBitmapSize );


#if INCLUDE_DESCRIBE_STREAM
/**
 * @brief Free resources consumed by message decoding.
 */
void OTA_CBOR_Decode_DescribeStreamResponseMessage_Finish(
    void *pvDecodeContext );

/**
 * @brief Decode the description of the first (or only) file in an OTA stream.
 */
BaseType_t OTA_CBOR_Decode_DescribeStreamResponseMessage_Next(
    void *pvDecodeContext,
    int *plNextFileId,
    int *plNextFileSize );

/**
 * @brief Decode the description of the first (or only) file in an OTA stream.
 */
BaseType_t OTA_CBOR_Decode_DescribeStreamResponseMessage_Start(
    void **ppvDecodeContext,
    uint8_t *pucMessageBuffer,
    size_t xMessageSize,
    char **ppcClientToken,
    int *plStreamVersion,
    char **ppcStreamDescription,
    int *plFirstFileId,
    int *plFirstFileSize );

/**
 * @brief Create an encoded Describe Stream Request message for the AWS IoT
 * OTA service.
 */
BaseType_t OTA_CBOR_Encode_DescribeStreamRequestMessage(
    uint8_t *pucMessageBuffer,
    size_t xMessageBufferSize,
    size_t *pxEncodedMessageSize,
    char *pcClientToken );

#endif /* INCLUDE_DESCRIBE_STREAM */
#endif 
