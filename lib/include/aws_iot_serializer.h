/*
 * Amazon FreeRTOS System Initialization
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

/*
 * This library is to provide a consistent layer for serializing JSON-like format.
 * The implementations can be CBOR or JSON.
 */

#ifndef AWS_IOT_SERIALIZER_H
#define AWS_IOT_SERIALIZER_H

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

#if AWS_IOT_SERIALIZER_ENABLE_ASSERTS == 1
    #ifndef AwsIotSerializer_Assert
        #include <assert.h>
        #define AwsIotSerializer_Assert( expression )    assert( expression )
    #endif
#else
    #define AwsIotSerializer_Assert( expression )
#endif

#define AWS_IOT_SERIALIZER_INDEFINITE_LENGTH                       0xffffffff

#define AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM    { .pHandle = NULL, .type = AWS_IOT_SERIALIZER_CONTAINER_STREAM }

#define AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP       { .pHandle = NULL, .type = AWS_IOT_SERIALIZER_CONTAINER_MAP }

#define AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY     { .pHandle = NULL, .type = AWS_IOT_SERIALIZER_CONTAINER_ARRAY }

#define AWS_IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER              { .type = AWS_IOT_SERIALIZER_UNDEFINED, .pHandle = NULL, .value = 0 }

#define AWS_IOT_SERIALIZER_DECODER_ITERATOR_INITIALIZER            NULL

/* helper macro to create scalar data */
#define AwsIotSerializer_ScalarSignedInt( signedIntValue )                                                                          \
    ( AwsIotSerializerScalarData_t ) { .value = { .signedInt = ( signedIntValue ) }, .type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT } \

#define AwsIotSerializer_ScalarTextString( pTextStringValue )                                                                                                                                  \
    ( AwsIotSerializerScalarData_t ) { .value = { .pString = ( ( uint8_t * ) pTextStringValue ), .stringLength = strlen( pTextStringValue ) }, .type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING } \

#define AwsIotSerializer_ScalarByteString( pByteStringValue, length )                                                                                            \
    ( AwsIotSerializerScalarData_t ) { .value = { .pString = ( pByteStringValue ), .stringLength = ( length ) }, .type = AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING } \

/* Determine if an object is a container. */
#define AwsIotSerializer_IsContainer( object )                       \
    ( ( object )                                                     \
      && ( ( object )->type == AWS_IOT_SERIALIZER_CONTAINER_STREAM   \
           || ( object )->type == AWS_IOT_SERIALIZER_CONTAINER_ARRAY \
           || ( object )->type == AWS_IOT_SERIALIZER_CONTAINER_MAP ) )

/* error code returned by serializer function interface */
typedef enum
{
    AWS_IOT_SERIALIZER_SUCCESS = 0,
    AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL,
    AWS_IOT_SERIALIZER_OUT_OF_MEMORY,
    AWS_IOT_SERIALIZER_INVALID_INPUT,
    AWS_IOT_SERIALIZER_UNDEFINED_TYPE,
    AWS_IOT_SERIALIZER_NOT_SUPPORTED,
    AWS_IOT_SERIALIZER_NOT_FOUND,
    AWS_IOT_SERIALIZER_INTERNAL_FAILURE
} AwsIotSerializerError_t;

/* two categories:
 * 1. scalar: int, string, byte, ...
 * 2. container: array, map, none
 */
typedef enum
{
    AWS_IOT_SERIALIZER_UNDEFINED = 0,
    AWS_IOT_SERIALIZER_SCALAR_NULL,
    AWS_IOT_SERIALIZER_SCALAR_BOOL,
    AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT,
    AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING,
    AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING,
    /* below is container */
    AWS_IOT_SERIALIZER_CONTAINER_STREAM,
    AWS_IOT_SERIALIZER_CONTAINER_ARRAY,
    AWS_IOT_SERIALIZER_CONTAINER_MAP,
} AwsIotSerializerDataType_t;

/* encapsulate data value of different types */
typedef struct AwsIotSerializerScalarValue
{
    union
    {
        int64_t signedInt;
        struct
        {
            uint8_t * pString;
            size_t stringLength;
        };
        bool booleanValue;
    };
} AwsIotSerializerScalarValue_t;

/* scalar data handle used in encoder */
typedef struct AwsIotSerializerScalarData
{
    AwsIotSerializerDataType_t type;
    AwsIotSerializerScalarValue_t value;
} AwsIotSerializerScalarData_t;

/* container data handle used in encoder */
typedef struct AwsIotSerializerEncoderObject
{
    AwsIotSerializerDataType_t type;
    void * pHandle;
} AwsIotSerializerEncoderObject_t;


/* data handle used in decoder: either container or scalar */
typedef struct AwsIotSerializerDecoderObject
{
    AwsIotSerializerDataType_t type;
    union
    {
        /* useful when this is a container */
        void * pHandle;
        /* if the type is a container, the scalarValue is unuseful */
        AwsIotSerializerScalarValue_t value;
    };
} AwsIotSerializerDecoderObject_t;

typedef void * AwsIotSerializerDecoderIterator_t;

/**
 * @brief Table containing function pointers for encoder APIs.
 */
typedef struct AwsIotSerializerEncodeInterface
{
    /**
     * @brief Return the actual total size after encoding finished.
     *
     * @param[in] pEncoderObject: the outermost object pointer; behavior is undefined for any other object
     * @param[in] pDataBuffer: the buffer pointer passed into init; behavior is undefined for any other buffer pointer
     */
    size_t ( * getEncodedSize )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                 uint8_t * pDataBuffer );

    /**
     * @brief Return the extra size needed when the data to encode exceeds the maximum length of underlying buffer.
     * When no exceeding, this should return 0.
     *
     * @param[in] pEncoderObject: the outermost object pointer; behavior is undefined for any other object
     */
    size_t ( * getExtraBufferSizeNeeded )( AwsIotSerializerEncoderObject_t * pEncoderObject );

    /**
     * @brief Initialize the object's handle with specified buffer and max size.
     *
     * @param[in] pEncoderObject Pointer of Encoder Object. After init, its type will be set to AWS_IOT_SERIALIZER_CONTAINER_STREAM.
     * @param[in] pDataBuffer Pointer to allocated buffer by user;
     *            NULL pDataBuffer is valid, used to calculate needed size by calling getExtraBufferSizeNeeded.
     * @param[in] maxSize Allocated buffer size
     */
    AwsIotSerializerError_t ( * init )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                        uint8_t * pDataBuffer,
                                        size_t maxSize );

    /**
     * @brief Clean the object's handle
     *
     * @param[in] pEncoderObject The outermost object pointer; behavior is undefined for any other object
     */
    void ( * destroy )( AwsIotSerializerEncoderObject_t * pEncoderObject );

    /**
     * @brief Open a child container object.
     *
     * @param[in] pEncoderObject: the parent object. It must be a container object
     * @param[in] pNewEncoderObject: the child object to create. It must be a container object
     * @param[in] length: pre-known length of the container or AWS_IOT_SERIALIZER_INDEFINITE_LENGTH
     *         map: the length is number of key-value pairs
     *         array: the length is number of elements
     *         none: the length is unuseful
     */
    AwsIotSerializerError_t ( * openContainer )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                 AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                                 size_t length );

    /**
     * @brief Open a child container object with string key.
     *
     * @param[in] pEncoderObject the parent object. It must be a container object
     * @param[in] pKey Key for the child container
     * @param[in] pNewEncoderObject the child object to create. It must be a container object
     * @param[in] length: pre-known length of the container or AWS_IOT_SERIALIZER_INDEFINITE_LENGTH
     *         map: the length is number of key-value pairs
     *         array: the length is number of elements
     *         none: the length is unuseful
     */
    AwsIotSerializerError_t ( * openContainerWithKey )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                        const char * pKey,
                                                        AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                                        size_t length );

    /**
     * @brief Close a child container object.
     * pEncoderObject and pNewEncoderObject must be parent-child relationship and are in open state
     *
     * @param[in] pEncoderObject: the parent object. It must be a container object
     * @param[in] pNewEncoderObject: the child object to create. It must be a container object
     */
    AwsIotSerializerError_t ( * closeContainer )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                  AwsIotSerializerEncoderObject_t * pNewEncoderObject );

    /**
     * @brief Append a scalar data to a container, with type array or none. Map container is invalid
     *
     * @param[in] pEncoderObject: the parent object. It must be a container object, with type array or none.
     * @param[in] scalarData: the scalar data to encode
     */
    AwsIotSerializerError_t ( * append )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                          AwsIotSerializerScalarData_t scalarData );

    /**
     * @brief Append a key-value pair to a map container. The key is string type and value is a scalar.
     *
     * @param[in] pEncoderObject: the parent object. It must be a map container object.
     * @param[in] pKey: text string representing key
     * @param[in] scalarData: the scalar data to encode
     */
    AwsIotSerializerError_t ( * appendKeyValue )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                  const char * pKey,
                                                  AwsIotSerializerScalarData_t scalarData );
} AwsIotSerializerEncodeInterface_t;

/**
 * @brief Table containing function pointers for decoder APIs.
 */
typedef struct AwsIotSerializerDecodeInterface
{
    /**
     * @brief Initialize decoder object with specified buffer.
     *
     * @param pDecoderObject Pointer to the decoder object allocated by user.
     * @param pDataBuffer Pointer to the buffer containing data to be decoded.
     * @param maxSize Maximum length of the buffer containing data to be decoded.
     * @return AWS_IOT_SERIALIZER_SUCCESS if successful
     */
    AwsIotSerializerError_t ( * init )( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                        const uint8_t * pDataBuffer,
                                        size_t maxSize );

    /**
     * @brief Destroy the decoder object handle
     * @param pDecoderObject Pointer to the decoder object
     */
    void ( * destroy )( AwsIotSerializerDecoderObject_t * pDecoderObject );


    /**
     * @brief Steps into a container and creates an iterator to traverse through the container.
     * Container can be of type array, map or stream.
     *
     * @param[in] pDecoderObject Pointer to the decoder object representing the container
     * @param[out] pIterator Pointer to iterator which can be used for the traversing the container by calling next()
     * @return AWS_IOT_SERIALIZER_SUCCESS if successful
     */
    AwsIotSerializerError_t ( * stepIn )( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                          AwsIotSerializerDecoderIterator_t * pIterator );


    /**
     * @brief Gets the object value currently pointed to by an iterator.
     * @param iterator The iterator handle
     * @param pValueObject Value of the object pointed to by the iterator.
     * @return AWS_IOT_SERIALIZER_SUCCESS if successful
     */
    AwsIotSerializerError_t ( * get )( AwsIotSerializerDecoderIterator_t iterator,
                                       AwsIotSerializerDecoderObject_t * pValueObject );

    /**
     *
     * @brief Find an object by key within a container.
     * Container should always be of type MAP.
     *
     * @param[in] pDecoderObject Pointer to the decoder object representing  container
     * @param[in] pKey Pointer to the key for which value needs to be found.
     * @param[out] pValueObject Pointer to the value object for the key.
     * @return AWS_IOT_SERIALIZER_SUCCESS if successful
     */
    AwsIotSerializerError_t ( * find )( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                        const char * pKey,
                                        AwsIotSerializerDecoderObject_t * pValueObject );


    /*
     * Find the next object in the same value and save it to pNewDecoderObject
     *
     */

    /**
     * @brief Moves the iterator to next object within the container
     * If the container is a map, it skips either a key or the value at a time.
     * If the container is an array, it skips by one array element.
     *
     * @param[in] iterator Pointer to iterator
     * @return AWS_IOT_SERIALIZER_SUCCESS if successful
     */
    AwsIotSerializerError_t ( * next )( AwsIotSerializerDecoderIterator_t iterator );

    /**
     * @brief Function to check if the iterator reached end of container
     * @param[in] iterator Pointer to iterator for the container
     * @return AWS_IOT_SERIALIZER_SUCCESS if successful
     */
    bool ( * isEndOfContainer )( AwsIotSerializerDecoderIterator_t iterator );



    /**
     * @brief Steps out of the container by updating the decoder object to next byte position
     * after the container.
     * The iterator **should** point to the end of the container when calling this function.
     *
     * @param[in] iterator Pointer to iterator for the container.
     * @param[in] The outer decoder object to the same container.
     * @return AWS_IOT_SERIALIZER_SUCCESS if successful
     */
    AwsIotSerializerError_t ( * stepOut )( AwsIotSerializerDecoderIterator_t iterator,
                                           AwsIotSerializerDecoderObject_t * pDecoderObject );
} AwsIotSerializerDecodeInterface_t;

/* Global reference of CBOR/JSON encoder and decoder. */
extern AwsIotSerializerEncodeInterface_t _AwsIotSerializerCborEncoder;

extern AwsIotSerializerDecodeInterface_t _AwsIotSerializerCborDecoder;

extern AwsIotSerializerEncodeInterface_t _AwsIotSerializerJsonEncoder;

extern AwsIotSerializerDecodeInterface_t _AwsIotSerializerJsonDecoder;

#endif /* ifndef AWS_IOT_SERIALIZER_H */
