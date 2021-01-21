/*
 * FreeRTOS Serializer V1.1.2
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

/*
 * This library is to provide a consistent layer for serializing JSON-like format.
 * The implementations can be CBOR or JSON.
 */

#ifndef IOT_SERIALIZER_H_
#define IOT_SERIALIZER_H_

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#if IOT_SERIALIZER_ENABLE_ASSERTS == 1
    #ifndef IotSerializer_Assert
        #include <assert.h>
        #define IotSerializer_Assert( expression )    assert( expression )
    #endif
#else
    #define IotSerializer_Assert( expression )
#endif

/*
 * Provide default values for undefined memory allocation functions based on
 * the usage of dynamic memory allocation.
 */
#if IOT_STATIC_MEMORY_ONLY == 1
    #include "private/iot_static_memory.h"

/**
 * @brief Allocate an array of uint8_t. This function should have the same
 * signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    void * IotSerializer_MallocCborEncoder( size_t size );

/**
 * @brief Free an array of uint8_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    void IotSerializer_FreeCborEncoder( void * ptr );

/**
 * @brief Allocate an array of uint8_t. This function should have the same
 * signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    void * IotSerializer_MallocCborParser( size_t size );

/**
 * @brief Free an array of uint8_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    void IotSerializer_FreeCborParser( void * ptr );

/**
 * @brief Allocate an array of uint8_t. This function should have the same
 * signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    void * IotSerializer_MallocCborValue( size_t size );

/**
 * @brief Free an array of uint8_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    void IotSerializer_FreeCborValue( void * ptr );

/**
 * @brief Allocate an array of uint8_t. This function should have the same
 * signature as [malloc]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
 */
    void * IotSerializer_MallocDecoderObject( size_t size );

/**
 * @brief Free an array of uint8_t. This function should have the same
 * signature as [free]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
 */
    void IotSerializer_FreeDecoderObject( void * ptr );
#else /* if IOT_STATIC_MEMORY_ONLY */
    #include <stdlib.h>

    #ifndef IotSerializer_MallocCborEncoder
        #define IotSerializer_MallocCborEncoder      malloc
    #endif
    #ifndef IotSerializer_FreeCborEncoder
        #define IotSerializer_FreeCborEncoder        free
    #endif
    #ifndef IotSerializer_MallocCborParser
        #define IotSerializer_MallocCborParser       malloc
    #endif
    #ifndef IotSerializer_FreeCborParser
        #define IotSerializer_FreeCborParser         free
    #endif
    #ifndef IotSerializer_MallocCborValue
        #define IotSerializer_MallocCborValue        malloc
    #endif
    #ifndef IotSerializer_FreeCborValue
        #define IotSerializer_FreeCborValue          free
    #endif
    #ifndef IotSerializer_MallocDecoderObject
        #define IotSerializer_MallocDecoderObject    malloc
    #endif
    #ifndef IotSerializer_FreeDecoderObject
        #define IotSerializer_FreeDecoderObject      free
    #endif
#endif /* if IOT_STATIC_MEMORY_ONLY */

#define IOT_SERIALIZER_INDEFINITE_LENGTH                       0xffffffff

#define IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM    { .pHandle = NULL, .type = IOT_SERIALIZER_CONTAINER_STREAM }

#define IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP       { .pHandle = NULL, .type = IOT_SERIALIZER_CONTAINER_MAP }

#define IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY     { .pHandle = NULL, .type = IOT_SERIALIZER_CONTAINER_ARRAY }

#define IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER              { .type = IOT_SERIALIZER_UNDEFINED }

#define IOT_SERIALIZER_DECODER_ITERATOR_INITIALIZER            NULL

#define IOT_SERIALIZER_SCALAR_DATA_INITIALIZER                 { .type = IOT_SERIALIZER_UNDEFINED }

/* helper macro to create scalar data */
#define IotSerializer_ScalarSignedInt( signedIntValue )                                                                        \
    ( IotSerializerScalarData_t ) { .value = { .u.signedInt = ( signedIntValue ) }, .type = IOT_SERIALIZER_SCALAR_SIGNED_INT } \

#define IotSerializer_ScalarTextString( pTextStringValue )                                                                                                                                          \
    ( IotSerializerScalarData_t ) { .value = { .u.string.pString = ( ( uint8_t * ) pTextStringValue ), .u.string.length = strlen( pTextStringValue ) }, .type = IOT_SERIALIZER_SCALAR_TEXT_STRING } \

#define IotSerializer_ScalarByteString( pByteStringValue, pByteStringLength )                                                                                                    \
    ( IotSerializerScalarData_t ) { .value = { .u.string.pString = ( pByteStringValue ), .u.string.length = ( pByteStringLength ) }, .type = IOT_SERIALIZER_SCALAR_BYTE_STRING } \

/* Determine if an object is a container. */
#define IotSerializer_IsContainer( object )                      \
    ( ( object )                                                 \
      && ( ( object )->type == IOT_SERIALIZER_CONTAINER_STREAM   \
           || ( object )->type == IOT_SERIALIZER_CONTAINER_ARRAY \
           || ( object )->type == IOT_SERIALIZER_CONTAINER_MAP ) )

/* error code returned by serializer function interface */
typedef enum
{
    IOT_SERIALIZER_SUCCESS = 0,
    IOT_SERIALIZER_BUFFER_TOO_SMALL,
    IOT_SERIALIZER_OUT_OF_MEMORY,
    IOT_SERIALIZER_INVALID_INPUT,
    IOT_SERIALIZER_UNDEFINED_TYPE,
    IOT_SERIALIZER_NOT_SUPPORTED,
    IOT_SERIALIZER_NOT_FOUND,
    IOT_SERIALIZER_INTERNAL_FAILURE
} IotSerializerError_t;

/* two categories:
 * 1. scalar: int, string, byte, ...
 * 2. container: array, map, none
 */
typedef enum
{
    IOT_SERIALIZER_UNDEFINED = 0,
    IOT_SERIALIZER_SCALAR_NULL,
    IOT_SERIALIZER_SCALAR_BOOL,
    IOT_SERIALIZER_SCALAR_SIGNED_INT,
    IOT_SERIALIZER_SCALAR_TEXT_STRING,
    IOT_SERIALIZER_SCALAR_BYTE_STRING,
    /* below is container */
    IOT_SERIALIZER_CONTAINER_STREAM,
    IOT_SERIALIZER_CONTAINER_ARRAY,
    IOT_SERIALIZER_CONTAINER_MAP,
} IotSerializerDataType_t;

/* encapsulate data value of different types */
typedef struct IotSerializerScalarValue
{
    union
    {
        int64_t signedInt;
        struct
        {
            uint8_t * pString;
            size_t length;
        } string;
        bool booleanValue;
    } u;
} IotSerializerScalarValue_t;

/* scalar data handle used in encoder */
typedef struct IotSerializerScalarData
{
    IotSerializerDataType_t type;
    IotSerializerScalarValue_t value;
} IotSerializerScalarData_t;

/* container data handle used in encoder */
typedef struct IotSerializerEncoderObject
{
    IotSerializerDataType_t type;
    void * pHandle;
} IotSerializerEncoderObject_t;


/* data handle used in decoder: either container or scalar */
typedef struct IotSerializerDecoderObject
{
    IotSerializerDataType_t type;
    union
    {
        /* useful when this is a container */
        void * pHandle;
        /* if the type is a container, the scalarValue is unuseful */
        IotSerializerScalarValue_t value;
    } u;
} IotSerializerDecoderObject_t;

typedef void * IotSerializerDecoderIterator_t;

/**
 * @brief Table containing function pointers for encoder APIs.
 */
typedef struct IotSerializerEncodeInterface
{
    /**
     * @brief Return the actual total size after encoding finished.
     *
     * @param[in] pEncoderObject: the outermost object pointer; behavior is undefined for any other object
     * @param[in] pDataBuffer: the buffer pointer passed into init; behavior is undefined for any other buffer pointer
     */
    size_t ( * getEncodedSize )( IotSerializerEncoderObject_t * pEncoderObject,
                                 uint8_t * pDataBuffer );

    /**
     * @brief Return the extra size needed when the data to encode exceeds the maximum length of underlying buffer.
     * When no exceeding, this should return 0.
     *
     * @param[in] pEncoderObject: the outermost object pointer; behavior is undefined for any other object
     */
    size_t ( * getExtraBufferSizeNeeded )( IotSerializerEncoderObject_t * pEncoderObject );

    /**
     * @brief Initialize the object's handle with specified buffer and max size.
     *
     * @param[in] pEncoderObject Pointer of Encoder Object. After init, its type will be set to IOT_SERIALIZER_CONTAINER_STREAM.
     * @param[in] pDataBuffer Pointer to allocated buffer by user;
     *            NULL pDataBuffer is valid, used to calculate needed size by calling getExtraBufferSizeNeeded.
     * @param[in] maxSize Allocated buffer size
     */
    IotSerializerError_t ( * init )( IotSerializerEncoderObject_t * pEncoderObject,
                                     uint8_t * pDataBuffer,
                                     size_t maxSize );

    /**
     * @brief Clean the object's handle
     *
     * @param[in] pEncoderObject The outermost object pointer; behavior is undefined for any other object
     */
    void ( * destroy )( IotSerializerEncoderObject_t * pEncoderObject );

    /**
     * @brief Open a child container object.
     *
     * @param[in] pEncoderObject: the parent object. It must be a container object
     * @param[in] pNewEncoderObject: the child object to create. It must be a container object
     * @param[in] length: pre-known length of the container or IOT_SERIALIZER_INDEFINITE_LENGTH
     *         map: the length is number of key-value pairs
     *         array: the length is number of elements
     *         none: the length is unuseful
     */
    IotSerializerError_t ( * openContainer )( IotSerializerEncoderObject_t * pEncoderObject,
                                              IotSerializerEncoderObject_t * pNewEncoderObject,
                                              size_t length );

    /**
     * @brief Open a child container object with string key.
     *
     * @param[in] pEncoderObject the parent object. It must be a container object
     * @param[in] pKey Key for the child container
     * @param[in] pNewEncoderObject the child object to create. It must be a container object
     * @param[in] length: pre-known length of the container or IOT_SERIALIZER_INDEFINITE_LENGTH
     *         map: the length is number of key-value pairs
     *         array: the length is number of elements
     *         none: the length is unuseful
     */
    IotSerializerError_t ( * openContainerWithKey )( IotSerializerEncoderObject_t * pEncoderObject,
                                                     const char * pKey,
                                                     IotSerializerEncoderObject_t * pNewEncoderObject,
                                                     size_t length );

    /**
     * @brief Close a child container object.
     * pEncoderObject and pNewEncoderObject must be parent-child relationship and are in open state
     *
     * @param[in] pEncoderObject: the parent object. It must be a container object
     * @param[in] pNewEncoderObject: the child object to create. It must be a container object
     */
    IotSerializerError_t ( * closeContainer )( IotSerializerEncoderObject_t * pEncoderObject,
                                               IotSerializerEncoderObject_t * pNewEncoderObject );

    /**
     * @brief Append a scalar data to a container, with type array or none. Map container is invalid
     *
     * @param[in] pEncoderObject: the parent object. It must be a container object, with type array or none.
     * @param[in] scalarData: the scalar data to encode
     */
    IotSerializerError_t ( * append )( IotSerializerEncoderObject_t * pEncoderObject,
                                       IotSerializerScalarData_t scalarData );

    /**
     * @brief Append a key-value pair to a map container. The key is string type and value is a scalar.
     *
     * @param[in] pEncoderObject: the parent object. It must be a map container object.
     * @param[in] pKey: text string representing key
     * @param[in] scalarData: the scalar data to encode
     */
    IotSerializerError_t ( * appendKeyValue )( IotSerializerEncoderObject_t * pEncoderObject,
                                               const char * pKey,
                                               IotSerializerScalarData_t scalarData );
} IotSerializerEncodeInterface_t;

/**
 * @brief Table containing function pointers for decoder APIs.
 */
typedef struct IotSerializerDecodeInterface
{
    /**
     * @brief Initialize decoder object with specified buffer.
     *
     * @param pDecoderObject Pointer to the decoder object allocated by user.
     * @param pDataBuffer Pointer to the buffer containing data to be decoded.
     * @param maxSize Maximum length of the buffer containing data to be decoded.
     * @return IOT_SERIALIZER_SUCCESS if successful
     */
    IotSerializerError_t ( * init )( IotSerializerDecoderObject_t * pDecoderObject,
                                     const uint8_t * pDataBuffer,
                                     size_t maxSize );

    /**
     * @brief Destroy the decoder object handle
     * @param pDecoderObject Pointer to the decoder object
     */
    void ( * destroy )( IotSerializerDecoderObject_t * pDecoderObject );


    /**
     * @brief Steps into a container and creates an iterator to traverse through the container.
     * Container can be of type array, map or stream.
     *
     * @param[in] pDecoderObject Pointer to the decoder object representing the container
     * @param[out] pIterator Pointer to iterator which can be used for the traversing the container by calling next()
     * @return IOT_SERIALIZER_SUCCESS if successful
     */
    IotSerializerError_t ( * stepIn )( IotSerializerDecoderObject_t * pDecoderObject,
                                       IotSerializerDecoderIterator_t * pIterator );


    /**
     * @brief Gets the object value currently pointed to by an iterator.
     * @param iterator The iterator handle
     * @param pValueObject Value of the object pointed to by the iterator.
     * @return IOT_SERIALIZER_SUCCESS if successful
     */
    IotSerializerError_t ( * get )( IotSerializerDecoderIterator_t iterator,
                                    IotSerializerDecoderObject_t * pValueObject );

    /**
     *
     * @brief Find an object by key within a container.
     * Container should always be of type MAP.
     *
     * @param[in] pDecoderObject Pointer to the decoder object representing  container
     * @param[in] pKey Pointer to the key for which value needs to be found.
     * @param[out] pValueObject Pointer to the value object for the key.
     * @return IOT_SERIALIZER_SUCCESS if successful
     */
    IotSerializerError_t ( * find )( IotSerializerDecoderObject_t * pDecoderObject,
                                     const char * pKey,
                                     IotSerializerDecoderObject_t * pValueObject );


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
     * @return IOT_SERIALIZER_SUCCESS if successful
     */
    IotSerializerError_t ( * next )( IotSerializerDecoderIterator_t iterator );

    /**
     * @brief Function to check if the iterator reached end of container
     * @param[in] iterator Pointer to iterator for the container
     * @return IOT_SERIALIZER_SUCCESS if successful
     */
    bool ( * isEndOfContainer )( IotSerializerDecoderIterator_t iterator );

    /**
     * @brief Function to obtain the starting buffer address of the raw encoded data (scalar or container type)
     * represented by the passed decoder object.
     * Container SHOULD be of type array or map.
     *
     * @param[in] pDecoderObject The decoder object whose underlying data's starting location in the buffer
     * is to be returned.
     * @param[out] pEncodedDataStartAddr This will be populated with the starting location of the encoded object
     * in the data buffer.
     *
     * @return #IOT_SERIALIZER_SUCCESS if successful; otherwise #IOT_SERIALIZER_NOT_SUPPORTED
     * for a non-container type iterator.
     */
    IotSerializerError_t ( * getBufferAddress )( const IotSerializerDecoderObject_t *
                                                 pDecoderObject,
                                                 const uint8_t **
                                                 pEncodedDataStartAddr );


    /**
     * @brief Function to get the size of the raw encoded data in the buffer (ONLY of container type object)
     * that is represented by the passed decoder object.
     * Container SHOULD be of type array or map.
     *
     * @param[in] pDecoderObject The decoder objects whose underlying buffer data's length needs to be
     * calculated.
     * @param[out] The length of the underlying data in the buffer represented by the decoder object.
     *
     * @return #IOT_SERIALIZER_SUCCESS if successful; otherwise #IOT_SERIALIZER_NOT_SUPPORTED
     * for a non-container type decoder object.
     */
    IotSerializerError_t ( * getSizeOfEncodedData )( const IotSerializerDecoderObject_t * pDecoderObject,
                                                     size_t * pEncodedDataLength );

    /**
     * @brief Function to calculate the size of a container.
     * It returns in constant time, O(1), for fixed-length containers, and incurs O(N) time for indefinite-length
     * containers.
     *
     * @param[in] pDecoderObject The decoder object representing a container whose size will be calculated.
     * @param[out] pLength The calculated length of the container will be stored here, if successful.
     *
     * @return #IOT_SERIALIZER_SUCCESS if successful, #IOT_SERIALIZER_NOT_SUPPORTED for a non-container type object
     * or the appropriate error code.
     */
    IotSerializerError_t ( * getSizeOf )( const IotSerializerDecoderObject_t * pDecoderObject,
                                          size_t * pLength );

    /**
     * @brief Steps out of the container by updating the decoder object to next byte position
     * after the container.
     * The iterator **should** point to the end of the container when calling this function.
     *
     * @param[in] iterator Pointer to iterator for the container.
     * @param[in] The outer decoder object to the same container.
     * @return IOT_SERIALIZER_SUCCESS if successful
     */
    IotSerializerError_t ( * stepOut )( IotSerializerDecoderIterator_t iterator,
                                        IotSerializerDecoderObject_t * pDecoderObject );
} IotSerializerDecodeInterface_t;

/* Global reference of CBOR/JSON encoder and decoder. */
extern IotSerializerEncodeInterface_t _IotSerializerCborEncoder;

extern IotSerializerDecodeInterface_t _IotSerializerCborDecoder;

extern IotSerializerEncodeInterface_t _IotSerializerJsonEncoder;

extern IotSerializerDecodeInterface_t _IotSerializerJsonDecoder;

#endif /* ifndef IOT_SERIALIZER_H_ */
