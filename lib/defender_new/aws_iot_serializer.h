#ifndef AWS_IOT_SERIALIZER_H
#define AWS_IOT_SERIALIZER_H

/*
 * This library is to provide a consistent layer for serializing JSON-like format.
 * The implementations can be CBOR or JSON.
 */

#include <stdint.h>

/* Constant: indicate the length is not pre-known when creating the container object(map, array). */
#define AWS_IOT_SERIALIZER_INDEFINITE_LENGTH                       0xffffffff

/* Constant: initializer for creating encoder container. */
#define AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM    { .pHandle = NULL, .type = AWS_IOT_SERIALIZER_CONTAINER_STREAM }

#define AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP       { .pHandle = NULL, .type = AWS_IOT_SERIALIZER_CONTAINER_MAP }

#define AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY     { .pHandle = NULL, .type = AWS_IOT_SERIALIZER_CONTAINER_ARRAY }

/* Helper macros to create scalar data */
#define AwsIotSerializer_ScalarSignedInt( signedIntValue )                                                                          \
    ( AwsIotSerializerScalarData_t ) { .value = { .signedInt = ( signedIntValue ) }, .type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT } \

#define AwsIotSerializer_ScalarTextString( pTextStringValue )                                                                                           \
    ( AwsIotSerializerScalarData_t ) { .value = { .pString = ( pTextStringValue ), .stringLength = 0 }, .type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING } \

#define AwsIotSerializer_ScalarByteString( pByteStringValue, length )                                                                                            \
    ( AwsIotSerializerScalarData_t ) { .value = { .pString = ( pByteStringValue ), .stringLength = ( length ) }, .type = AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING } \

/* error code returned by serializer function interface */
typedef enum
{
    AWS_IOT_SERIALIZER_SUCCESS = 0,
    AWS_IOT_SERIALIZER_OUT_OF_MEMORY,
    AWS_IOT_SERIALIZER_INVALID_INPUT,
    AWS_IOT_SERIALIZER_UNDEFINED_TYPE,
    AWS_IOT_SERIALIZER_NOT_FOUND,
    AWS_IOT_SERIALIZER_INTERNAL_FAILURE
} AwsIotSerializerError_t;

/* data type in two categories:
 * 1. scalar: int, string, byte, ...
 * 2. container: array, map, stream
 */
typedef enum
{
    AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT,
    AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING,
    AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING,
    /* below is container */
    AWS_IOT_SERIALIZER_CONTAINER_MAP,
    AWS_IOT_SERIALIZER_CONTAINER_ARRAY,
    AWS_IOT_SERIALIZER_CONTAINER_STREAM,
    AWS_IOT_SERIALIZER_UNDEFINED
} AwsIotSerializerDataType_t;

/* encapsulate data value of different types:
 * - signed int (64 bits)
 * - byte string
 */
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
    };
} AwsIotSerializerScalarValue_t;

/* scalar data handle: type + value */
typedef struct AwsIotSerializerScalarData
{
    AwsIotSerializerDataType_t type;
    AwsIotSerializerScalarValue_t value;
} AwsIotSerializerScalarData_t;

/* container data handle used in encoder: type + handle */
typedef struct AwsIotSerializerEncoderObject
{
    AwsIotSerializerDataType_t type;
    void * pHandle;
} AwsIotSerializerEncoderObject_t;

/* data handle used in decoder */
typedef struct AwsIotSerializerDecoderObject
{
    AwsIotSerializerDataType_t type;
    union
    {
        /* useful when this is a container */
        void * pHandle;
        /* useful when this is a scalar */
        AwsIotSerializerScalarValue_t value;
    };
} AwsIotSerializerDecoderObject_t;

typedef struct AwsIotSerializerDecoderIterator
{
    AwsIotSerializerDataType_t type;
    void * pHandle;
} AwsIotSerializerDecoderIterator_t;

/* functions table of encoding */
typedef struct AwsIotSerializerEncodeInterface
{
    /*
     *  Return the actual total size after encoding finished.
     *
     *  pEncoderObject: the outermost object pointer; behavior is undefined for any other object
     *  pDataBuffer: the buffer pointer passed into init; behavior is undefined for any other buffer pointer
     */
    size_t ( * getEncodedSize )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                 uint8_t * pDataBuffer );

    /*
     * Return the extra size needed when the data to encode exceeds the maximum length of underlying buffer.
     * When no exceeding, this should return 0.
     *
     * pEncoderObject: the outermost object pointer; behavior is undefined for any other object
     */
    size_t ( * getExtraBufferSizeNeeded )( AwsIotSerializerEncoderObject_t * pEncoderObject );

    /*
     * Initialize the object's handle with specified buffer and max size.
     * The type of pEncoderObject will be intialized to AWS_IOT_SERIALIZER_CONTAINER_STREAM
     *
     * pDataBuffer: the pointer to allocated buffer by user;
     *              NULL pDataBuffer is valid, used to calculate needed size by calling getExtraBufferSizeNeeded.
     * maxSize: allocated buffer size
     */
    AwsIotSerializerError_t ( * init )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                        uint8_t * pDataBuffer,
                                        size_t maxSize );

    /*
     * Destroy the object's handle. Supposed to be used only for outermost object
     *
     * pEncoderObject: the outermost object pointer; behavior is undefined for any other object
     */
    AwsIotSerializerError_t ( * destroy )( AwsIotSerializerEncoderObject_t * pEncoderObject );

    /*
     * Open a child container object(pNewEncoderObject).
     * (must be invoke closeContainer after encoding)
     *
     * pEncoderObject: the parent object. It must be a container object
     * pNewEncoderObject: the child object to create. It must be a container object
     * length: pre-known length of the container or AWS_IOT_SERIALIZER_INDEFINITE_LENGTH
     *         map: the length is number of key-value pairs
     *         array: the length is number of elements
     *         stream: the length is unuseful
     */
    AwsIotSerializerError_t ( * openContainer )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                 AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                                 size_t length );

    /*
     * Open a child container object(pNewEncoderObject) with string key.
     * (must be invoke closeContainer after encoding)
     *
     * pEncoderObject: the parent object. It must be a container object
     * pNewEncoderObject: the child object to create. It must be a container object
     * containerType: must be a container type
     * length: pre-known length of the container or AWS_IOT_SERIALIZER_INDEFINITE_LENGTH
     *         map: the length is number of key-value pairs
     *         array: the length is number of elements
     *         stream: the length is unuseful
     */
    AwsIotSerializerError_t ( * openContainerWithKey )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                        const char * pKey,
                                                        AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                                        size_t length );

    /*
     * Close a child container object. (invoked after encoding)
     * pEncoderObject and pNewEncoderObject must be parent-child relationship and are in open state
     *
     * pEncoderObject: the parent object. It must be a container object
     * pNewEncoderObject: the child object to create. It must be a container object
     */
    AwsIotSerializerError_t ( * closeContainer )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                  AwsIotSerializerEncoderObject_t * pNewEncoderObject );

    /*
     * Append a scalar data to a container, with type array or stream. Map container is invalid
     *
     * pEncoderObject: the parent object. It must be a container object, with type array or stream.
     * scalarData: the scalar data to encode
     */
    AwsIotSerializerError_t ( * append )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                          AwsIotSerializerScalarData_t scalarData );

    /*
     * Append a key-value pair to a map container. The key is string type and value is a scalar.
     *
     * pEncoderObject: the parent object. It must be a map container object.
     * pKey: text string representing key
     * scalarData: the scalar data to encode
     */
    AwsIotSerializerError_t ( * appendKeyValue )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                  const char * pKey,
                                                  AwsIotSerializerScalarData_t scalarData );
} AwsIotSerializerEncodeInterface_t;

/* functions table of decoding
 */
typedef struct AwsIotSerializerDecodeInterface
{
    /*
     * Init decoder object with specified buffer. It updates correct data type to the object
     *
     * pDecoderObject: the object handle for initialization
     */
    AwsIotSerializerError_t ( * init )( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                        uint8_t * pDataBuffer,
                                        size_t maxSize );

    /*
     * Destroy the object handle.
     * This should be invoked on objects created by init and find
     *
     * pDecoderObject: the object handle for destroy
     */
    AwsIotSerializerError_t ( * destroy )( AwsIotSerializerDecoderObject_t * pDecoderObject );

    /*
     * Get object value from iterator: either scalar or container
     *
     * pIterator: a valid input iterator
     * pValueObject: result object with correct type
     */
    AwsIotSerializerError_t ( * get )( AwsIotSerializerDecoderIterator_t * pIterator,
                                       AwsIotSerializerDecoderObject_t * pValueObject );

    /*
     * Find the object value paired to input key.
     * If not found, pValueObject will be untouched and return AWS_IOT_SERIALIZER_NOT_FOUND
     * pValueObject must be destroyed after usage
	 *
	 * If found object is with scalar type:
	 *
	 * if it is fixed-size type (int, ...), the value will be saved in pValueObject directly
	 * if not(string...), user needs to pass allocated buffer and length to pValueObject->value.pString, and pValueObject->value.stringLength.
	 * passing NULL to pValueObject->value.pString will result in calculated length will be saved in pValueObject->value.stringLength
     *
     * pDecoderObject: a container object with map type
     * pKey: text string with null terminator
     * pValueObject: found value saving to it. It must be destroyed after usage
     */
    AwsIotSerializerError_t ( * find )( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                        const char * pKey,
                                        AwsIotSerializerDecoderObject_t * pValueObject );

    /*
     * Advance pIterator to next iterator in same level. If it is the end of container, it will return error.
     * For array, it will simply go to next element.
     * For map, it will go to next key.
     * For stream, it will go to next value.
     *
     * Array example: pIterator points to 1 currently. After invoking next, it will point to 2
     * [(1), 2, 3, 4, 5] -> [1, (2), 3, 4, 5]
     * Array of Map example: pIterator points to first map currently. After invoking next, it will point to second map
     * [{"a":"b"}, {"c":"d"}] -> [{"a":"b"}, {"c":"d"}]
     *  ^                                    ^
     * Map Example: pIterator points to first key "a". After invoking next, it will point to second key "c"
     * {"a":"b", "c":"d"} -> {"a":"b", "c":"d"}
     *   ^                              ^
     */
    AwsIotSerializerError_t ( * next )( AwsIotSerializerDecoderIterator_t * pIterator );

    /*
     * Check if this is the end of container.
     */
    uint8_t ( * isEndOfContainer )( AwsIotSerializerDecoderIterator_t * pIterator );

    /*
     * enter the container object and save the iterator to pIterator, which represents the first element.
     * the type of pIterator will be the type of the container.
     * user can invoke get to obtain the decoder object that pIterator points to
     */
    AwsIotSerializerError_t ( * stepIn )( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                          AwsIotSerializerDecoderIterator_t * pIterator );

    /*
     * leave the current container and go to the next container which will be save into pDecoderObject
     *
     * [ [1, 2, 3, 4, 5] , [1, 2, 3, 4, 5] ]  ->  [ [1, 2, 3, 4, 5] , [1, 2, 3, 4, 5] ]
     *          ^                                                     ^
     */
    AwsIotSerializerError_t ( * stepOut )( AwsIotSerializerDecoderIterator_t * pIterator,
                                           AwsIotSerializerDecoderObject_t * pDecoderObject );

    /*
     * print a beautiful string to stdout
     */
    void ( * print )( uint8_t * pDataBuffer,
                      size_t dataSize );
} AwsIotSerializerDecodeInterface_t;

/* Global reference of CBOR/JSON encoder and decoder. */
extern AwsIotSerializerEncodeInterface_t _AwsIotSerializerCborEncoder;

extern AwsIotSerializerDecodeInterface_t _AwsIotSerializerCborDecoder;

extern AwsIotSerializerEncodeInterface_t _AwsIotSerializerJsonEncoder;

extern AwsIotSerializerDecodeInterface_t _AwsIotSerializerJsonDecoder;

#endif /* ifndef AWS_IOT_SERIALIZER_H */
