/*
Amazon FreeRTOS OTA Agent V0.9.4
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/

/**
 * @file aws_ota_cbor.c
 * @brief CBOR encode/decode routines for AWS IoT Over-the-Air updates.
 */

#include "FreeRTOS.h"
#include "cbor.h"
#include "aws_ota_cbor.h"
#include "aws_ota_cbor_internal.h"

/**
 * @brief Message field definitions, per the server specification.
 */

#if INCLUDE_DESCRIBE_STREAM
#define OTA_CBOR_DESCRIBESTREAMREQUEST_ITEM_COUNT    1
#endif
#define OTA_CBOR_GETSTREAMREQUEST_ITEM_COUNT         5

/**
 * @brief Internal context structure for decoding CBOR arrays.
 */
typedef struct OTAMessageDecodeContext
{
    CborParser xCborParser;
    CborValue xCborRecursedItem;
} OTAMessageDecodeContext_t, * OTAMessageDecodeContextPtr_t;

/**
 * @brief Decode a Get Stream response message from AWS IoT OTA.
 */
BaseType_t OTA_CBOR_Decode_GetStreamResponseMessage( uint8_t * pucMessageBuffer,
                                                     size_t xMessageSize,
                                                     int * plFileId,
                                                     int * plBlockId,
                                                     int * plBlockSize,
                                                     uint8_t ** ppucPayload,
                                                     size_t * pxPayloadSize )
{
    CborError xCborResult = CborNoError;
    CborParser xCborParser;
    CborValue xCborValue, xCborMap;

    /* Initialize the parser. */
    xCborResult = cbor_parser_init(
        pucMessageBuffer,
        xMessageSize,
        0,
        &xCborParser,
        &xCborMap );

    /* Get the outer element and confirm that it's a "map," i.e., a set of
     * CBOR key/value pairs. */
    if( CborNoError == xCborResult )
    {
        if( false == cbor_value_is_map( &xCborMap ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    /* Find the file ID. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_map_find_value(
            &xCborMap,
            OTA_CBOR_FILEID_KEY,
            &xCborValue );
    }

    if( CborNoError == xCborResult )
    {
        if( CborIntegerType != cbor_value_get_type(
                                     &xCborValue ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_get_int(
            &xCborValue,
            plFileId );
    }

    /* Find the block ID. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_map_find_value(
            &xCborMap,
            OTA_CBOR_BLOCKID_KEY,
            &xCborValue );
    }

    if( CborNoError == xCborResult )
    {
        if( CborIntegerType != cbor_value_get_type(
                                     &xCborValue ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_get_int(
            &xCborValue,
            plBlockId );
    }

    /* Find the block size. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_map_find_value(
            &xCborMap,
            OTA_CBOR_BLOCKSIZE_KEY,
            &xCborValue );
    }

    if( CborNoError == xCborResult )
    {
        if( CborIntegerType != cbor_value_get_type(
                                     &xCborValue ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_get_int(
            &xCborValue,
            plBlockSize );
    }

    /* Find the payload bytes. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_map_find_value(
            &xCborMap,
            OTA_CBOR_BLOCKPAYLOAD_KEY,
            &xCborValue );
    }

    if( CborNoError == xCborResult )
    {
        if( CborByteStringType != cbor_value_get_type(
                                        &xCborValue ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_calculate_string_length(
            &xCborValue,
            pxPayloadSize );
    }

    if( CborNoError == xCborResult )
    {
        *ppucPayload = pvPortMalloc( *pxPayloadSize );

        if( NULL == *ppucPayload )
        {
            xCborResult = CborErrorOutOfMemory;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_copy_byte_string(
            &xCborValue,
            *ppucPayload,
            pxPayloadSize,
            NULL );
    }

    return CborNoError == xCborResult;
}



/**
 * @brief Create an encoded Get Stream Request message for the AWS IoT OTA
 * service. The service allows block count or block bitmap to be requested,
 * but not both.
 */
BaseType_t OTA_CBOR_Encode_GetStreamRequestMessage( uint8_t * pucMessageBuffer,
                                                    size_t xMessageBufferSize,
                                                    size_t * pxEncodedMessageSize,
                                                    char * pcClientToken,
                                                    int lFileId,
                                                    int lBlockSize,
                                                    int lBlockOffset,
                                                    uint8_t * pucBlockBitmap,
                                                    size_t xBlockBitmapSize )
{
    CborError xCborResult = CborNoError;
    CborEncoder xCborEncoder, xCborMapEncoder;

    /* Initialize the CBOR encoder. */
    cbor_encoder_init(
        &xCborEncoder,
        pucMessageBuffer,
        xMessageBufferSize,
        0 );
    xCborResult = cbor_encoder_create_map(
        &xCborEncoder,
        &xCborMapEncoder,
        OTA_CBOR_GETSTREAMREQUEST_ITEM_COUNT );

    /* Encode the client token key and value. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborMapEncoder,
            OTA_CBOR_CLIENTTOKEN_KEY );
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborMapEncoder,
            pcClientToken );
    }

#ifdef INCLUDE_STREAM_VERSION_PARAM
    /* Encode the stream version key and value. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborMapEncoder,
            OTA_CBOR_STREAMVERSION_KEY );
    }
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_int(
            &xCborMapEncoder,
            lStreamVersion );
    }
#endif

    /* Encode the file ID key and value. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborMapEncoder,
            OTA_CBOR_FILEID_KEY );
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_int(
            &xCborMapEncoder,
            lFileId );
    }

    /* Encode the block size key and value. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborMapEncoder,
            OTA_CBOR_BLOCKSIZE_KEY );
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_int(
            &xCborMapEncoder,
            lBlockSize );
    }

    /* Encode the block offset key and value. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborMapEncoder,
            OTA_CBOR_BLOCKOFFSET_KEY );
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_int(
            &xCborMapEncoder,
            lBlockOffset );
    }
#ifdef INCLUDE_BLOCK_COUNT_PARAM
    if( NULL == pucBlockBitmap )
    {
        /* Encode the block count key and value. */
        if( CborNoError == xCborResult )
        {
            xCborResult = cbor_encode_text_stringz(
                &xCborMapEncoder,
                OTA_CBOR_BLOCKCOUNT_KEY );
        }

        if( CborNoError == xCborResult )
        {
            xCborResult = cbor_encode_int(
                &xCborMapEncoder,
                lBlockCount );
        }
    }
    else
#endif
    	{
        /* Encode the block bitmap key and value. */
        if( CborNoError == xCborResult )
        {
            xCborResult = cbor_encode_text_stringz(
                &xCborMapEncoder,
                OTA_CBOR_BLOCKBITMAP_KEY );
        }

        if( CborNoError == xCborResult )
        {
            xCborResult = cbor_encode_byte_string(
                &xCborMapEncoder,
                pucBlockBitmap,
                xBlockBitmapSize );
        }
    }

    /* Done with the encoder. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encoder_close_container_checked(
            &xCborEncoder,
            &xCborMapEncoder );
    }

    /* Get the encoded size. */
    if( CborNoError == xCborResult )
    {
        *pxEncodedMessageSize = cbor_encoder_get_buffer_size(
            &xCborEncoder,
            pucMessageBuffer );
    }

    return CborNoError == xCborResult;
}


#if INCLUDE_DESCRIBE_STREAM

/**
 * @brief Free resources consumed by message decoding.
 */
void OTA_CBOR_Decode_DescribeStreamResponseMessage_Finish( void * pvDecodeContext )
{
    if( NULL != pvDecodeContext )
    {
        vPortFree( pvDecodeContext );
    }
}

/**
 * @brief Decode the description of the first (or only) file in an OTA stream.
 */
BaseType_t OTA_CBOR_Decode_DescribeStreamResponseMessage_Next( void * pvDecodeContext,
                                                               int * plNextFileId,
                                                               int * plNextFileSize )
{
    OTAMessageDecodeContextPtr_t pxOTADecoder =
        ( OTAMessageDecodeContextPtr_t ) pvDecodeContext;
    CborError xCborResult = CborNoError;
    CborValue xCborValue;

    /* Get the next stream file element. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_advance(
            &pxOTADecoder->xCborRecursedItem );
    }

    /* Determine if it actually is another file. */
    if( true == cbor_value_at_end( &pxOTADecoder->xCborRecursedItem ) )
    {
        xCborResult = CborErrorAdvancePastEOF;
    }

    /* Confirm the expected element type. */
    if( CborNoError == xCborResult )
    {
        if( false == cbor_value_is_map( &pxOTADecoder->xCborRecursedItem ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    /* Find the stream file ID. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_map_find_value(
            &pxOTADecoder->xCborRecursedItem,
            OTA_CBOR_FILEID_KEY,
            &xCborValue );
    }

    if( CborNoError == xCborResult )
    {
        if( CborIntegerType != cbor_value_get_type(
                                     &xCborValue ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_get_int(
            &xCborValue,
            plNextFileId );
    }

    /* Find the stream file size. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_map_find_value(
            &pxOTADecoder->xCborRecursedItem,
            OTA_CBOR_FILESIZE_KEY,
            &xCborValue );
    }

    if( CborNoError == xCborResult )
    {
        if( CborIntegerType != cbor_value_get_type(
                                     &xCborValue ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_get_int(
            &xCborValue,
            plNextFileSize );
    }

    /* Handle no more items, if applicable. */
    if( CborErrorAdvancePastEOF == xCborResult )
    {
        *plNextFileId = 0;
        *plNextFileSize = 0;
        xCborResult = CborNoError;
    }

    return CborNoError == xCborResult;
}

/**
 * @brief Decode the description of the first (or only) file in an OTA stream.
 */
BaseType_t OTA_CBOR_Decode_DescribeStreamResponseMessage_Start( void ** ppvDecodeContext,
                                                                uint8_t * pucMessageBuffer,
                                                                size_t xMessageSize,
                                                                char ** ppcClientToken,
                                                                int * plStreamVersion,
                                                                char ** ppcStreamDescription,
                                                                int * plFirstFileId,
                                                                int * plFirstFileSize )
{
    OTAMessageDecodeContextPtr_t pxOTADecoder = NULL;
    CborError xCborResult = CborNoError;
    CborValue xCborValue, xCborOuterMap, xCborFilesArray;
    size_t xSize = 0;

    /* Create a context. */
    pxOTADecoder = pvPortMalloc( sizeof( OTAMessageDecodeContext_t ) );

    if( NULL == pxOTADecoder )
    {
        xCborResult = CborErrorOutOfMemory;
    }

    /* Initialize the parser. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_parser_init(
            pucMessageBuffer,
            xMessageSize,
            0,
            &pxOTADecoder->xCborParser,
            &xCborOuterMap );
    }

    /* Get the outer element and confirm that it's a "map," i.e., a set of
     * CBOR key/value pairs. */
    if( CborNoError == xCborResult )
    {
        if( false == cbor_value_is_map( &xCborOuterMap ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    /* Find the client token. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_map_find_value(
            &xCborOuterMap,
            OTA_CBOR_CLIENTTOKEN_KEY,
            &xCborValue );
    }

    if( CborNoError == xCborResult )
    {
        if( CborTextStringType != cbor_value_get_type(
                                        &xCborValue ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_calculate_string_length(
            &xCborValue,
            &xSize );
    }

    if( CborNoError == xCborResult )
    {
        *ppcClientToken = pvPortMalloc( xSize );

        if( NULL == *ppcClientToken )
        {
            xCborResult = CborErrorOutOfMemory;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_copy_text_string(
            &xCborValue,
            *ppcClientToken,
            &xSize,
            NULL );
    }

    /* Find the stream version. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_map_find_value(
            &xCborOuterMap,
            OTA_CBOR_STREAMVERSION_KEY,
            &xCborValue );
    }

    if( CborNoError == xCborResult )
    {
        if( CborIntegerType != cbor_value_get_type(
                                     &xCborValue ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_get_int(
            &xCborValue,
            plStreamVersion );
    }

    /* Find the stream description. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_map_find_value(
            &xCborOuterMap,
            OTA_CBOR_STREAMDESCRIPTION_KEY,
            &xCborValue );
    }

    if( CborNoError == xCborResult )
    {
        if( CborTextStringType != cbor_value_get_type(
                                        &xCborValue ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_calculate_string_length(
            &xCborValue,
            &xSize );
    }

    if( CborNoError == xCborResult )
    {
        *ppcStreamDescription = pvPortMalloc( xSize );

        if( NULL == *ppcStreamDescription )
        {
            xCborResult = CborErrorOutOfMemory;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_copy_text_string(
            &xCborValue,
            *ppcStreamDescription,
            &xSize,
            NULL );
    }

    /* Find the stream files and confirm that it's an array. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_map_find_value(
            &xCborOuterMap,
            OTA_CBOR_STREAMFILES_KEY,
            &xCborFilesArray );
    }

    if( CborNoError == xCborResult )
    {
        if( false == cbor_value_is_array( &xCborFilesArray ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    /* Get the first stream file element. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_enter_container(
            &xCborFilesArray,
            &pxOTADecoder->xCborRecursedItem );
    }

    if( CborNoError == xCborResult )
    {
        if( false == cbor_value_is_map( &pxOTADecoder->xCborRecursedItem ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    /* Find the stream file ID. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_map_find_value(
            &pxOTADecoder->xCborRecursedItem,
            OTA_CBOR_FILEID_KEY,
            &xCborValue );
    }

    if( CborNoError == xCborResult )
    {
        if( CborIntegerType != cbor_value_get_type(
                                     &xCborValue ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_get_int(
            &xCborValue,
            plFirstFileId );
    }

    /* Find the stream file size. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_map_find_value(
            &pxOTADecoder->xCborRecursedItem,
            OTA_CBOR_FILESIZE_KEY,
            &xCborValue );
    }

    if( CborNoError == xCborResult )
    {
        if( CborIntegerType != cbor_value_get_type(
                                     &xCborValue ) )
        {
            xCborResult = CborErrorIllegalType;
        }
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_value_get_int(
            &xCborValue,
            plFirstFileSize );
    }

    /* Clean-up and return. */
    if( CborNoError == xCborResult )
    {
        *ppvDecodeContext = pxOTADecoder;
        pxOTADecoder = NULL;
    }

    if( NULL != pxOTADecoder )
    {
        vPortFree( pxOTADecoder );
        *ppvDecodeContext = NULL;
    }

    return CborNoError == xCborResult;
}


/**
 * @brief Create an encoded Describe Stream Request message for the AWS IoT
 * OTA service.
 */
BaseType_t OTA_CBOR_Encode_DescribeStreamRequestMessage( uint8_t * pucMessageBuffer,
                                                         size_t xMessageBufferSize,
                                                         size_t * pxEncodedMessageSize,
                                                         char * pcClientToken )
{
    CborError xCborResult = CborNoError;
    CborEncoder xCborEncoder, xCborMapEncoder;

    /* Initialize the CBOR encoder. */
    cbor_encoder_init(
        &xCborEncoder,
        pucMessageBuffer,
        xMessageBufferSize,
        0 );
    xCborResult = cbor_encoder_create_map(
        &xCborEncoder,
        &xCborMapEncoder,
        OTA_CBOR_DESCRIBESTREAMREQUEST_ITEM_COUNT );

    /* Encode the client token key and value. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborMapEncoder,
            OTA_CBOR_CLIENTTOKEN_KEY );
    }

    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encode_text_stringz(
            &xCborMapEncoder,
            pcClientToken );
    }

    /* Done with the encoder. */
    if( CborNoError == xCborResult )
    {
        xCborResult = cbor_encoder_close_container_checked(
            &xCborEncoder,
            &xCborMapEncoder );
    }

    /* Get the encoded size. */
    if( CborNoError == xCborResult )
    {
        *pxEncodedMessageSize = cbor_encoder_get_buffer_size(
            &xCborEncoder,
            pucMessageBuffer );
    }

    return CborNoError == xCborResult;
}
#endif /* INCLUDE_DESCRIBE_STREAM */
