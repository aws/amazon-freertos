/*
 * This library is to provide a consistent layer for serializing JSON-like format.
 * The implementations can be CBOR or JSON.
 */

#include <stdint.h>

#ifndef AWS_IOT_SERIALIZER_H
#define AWS_IOT_SERIALIZER_H

#define AWS_IOT_SERIALIZER_INDEFINITE_LENGTH            0xffffffff

#define AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_NONE { .pHandle = NULL, .type = AWS_IOT_SERIALIZER_OBJECT_NONE }

#define AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP { .pHandle = NULL, .type = AWS_IOT_SERIALIZER_OBJECT_MAP }

#define AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY { .pHandle = NULL, .type = AWS_IOT_SERIALIZER_OBJECT_ARRAY }

/* helper macro to create scalar data */
#define AwsIotSerializer_ScalarSignedInt( signedIntValue )      \
( AwsIotSerializerScalarData_t ) { .value = { .signedInt = ( signedIntValue ) }, .type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT } \

#define AwsIotSerializer_ScalarTextString( pTextStringValue )    \
( AwsIotSerializerScalarData_t ) { .value = { .pTextString = ( pTextStringValue )}, .type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING } \

#define AwsIotSerializer_ScalarByteString( pByteStringValue, length )    \
( AwsIotSerializerScalarData_t ) { .value = { .pByteString = ( pByteStringValue ), .byteStringLength = ( length ) }, .type = AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING } \

/* error code returned by serializer function interface */
typedef enum
{
    AWS_IOT_SERIALIZER_SUCCESS = 0,
    AWS_IOT_SERIALIZER_OUT_OF_MEMORY,
    AWS_IOT_SERIALIZER_NOT_SUPPORTED,
    AWS_IOT_SERIALIZER_INTERNAL_FAILURE
} AwsIotSerializerError_t;

/* two categories:
 * 1. scalar: int, string, byte, ...
 * 2. container: array, map, none
 */
typedef enum
{
    AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT,
    AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING,
    AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING,
    /* below is container */
    AWS_IOT_SERIALIZER_OBJECT_MAP,
    AWS_IOT_SERIALIZER_OBJECT_ARRAY,
    AWS_IOT_SERIALIZER_OBJECT_NONE
} AwsIotSerializerDataType_t;

/* encapsulate data value of different types */
typedef struct AwsIotSerializerScalarValue
{
	union
	{
		int64_t signedInt;
		struct
		{
			uint8_t * pByteString;
			size_t byteStringLength;
		};
		char * pTextString;
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
    void * pHandle;
    /* if the type is a container, the scalarValue is unuseful */
	AwsIotSerializerScalarValue_t value;
} AwsIotSerializerDecoderObject_t;

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
     *
     * pDataBuffer: the pointer to allocated buffer by user; 
	 *              NULL pDataBuffer is valid, used to calculate needed size by calling getExtraBufferSizeNeeded.
     * maxSize: allocated buffer size
     */
    AwsIotSerializerError_t ( * init )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                        uint8_t * pDataBuffer,
                                        size_t maxSize );

    /*
     * Clean the object's handle
	 * 
	 * pEncoderObject: the outermost object pointer; behavior is undefined for any other object
     */
    AwsIotSerializerError_t ( * destroy )( AwsIotSerializerEncoderObject_t * pEncoderObject );

	/*
	 * Open a child container object.
	 *
	 * pEncoderObject: the parent object. It must be a container object
	 * pNewEncoderObject: the child object to create. It must be a container object
	 * length: pre-known length of the container or AWS_IOT_SERIALIZER_INDEFINITE_LENGTH
	 *         map: the length is number of key-value pairs
	 *         array: the length is number of elements
	 *         none: the length is unuseful
	 */
    AwsIotSerializerError_t ( * openContainer )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                 AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                                 size_t length );

	/*
	 * Open a child container object with string key.
	 *
	 * pEncoderObject: the parent object. It must be a container object
	 * pNewEncoderObject: the child object to create. It must be a container object
	 * containerType: must be a container type
	 * length: pre-known length of the container or AWS_IOT_SERIALIZER_INDEFINITE_LENGTH
	 *         map: the length is number of key-value pairs
	 *         array: the length is number of elements
	 *         none: the length is unuseful
	 */
    AwsIotSerializerError_t ( * openContainerWithKey )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                        const char * pKey,
                                                        AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                                        size_t length );

	/*
	 * Close a child container object.
	 * pEncoderObject and pNewEncoderObject must be parent-child relationship and are in open state
	 *
	 * pEncoderObject: the parent object. It must be a container object
	 * pNewEncoderObject: the child object to create. It must be a container object
	 */
    AwsIotSerializerError_t ( * closeContainer )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                  AwsIotSerializerEncoderObject_t * pNewEncoderObject );

	/*
	 * Append a scalar data to a container, with type array or none. Map container is invalid
	 *
	 * pEncoderObject: the parent object. It must be a container object, with type array or none.
	 * scalarData: the scalar data to encode
	 */
    AwsIotSerializerError_t ( * append )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                          AwsIotSerializerScalarData_t scalarData);

	/*
	 * Append a key-value pair to a map container. The key is string type and value is a scalar.
	 *
	 * pEncoderObject: the parent object. It must be a map container object.
	 * pKey: text string representing key
	 * scalarData: the scalar data to encode
	 */
    AwsIotSerializerError_t ( * appendKeyValue )( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                  const char * pKey,
                                                  AwsIotSerializerScalarData_t scalarData);
} AwsIotSerializerEncodeInterface_t;

/* functions table of decoding 
*  more APIs can be added later.
*/
typedef struct AwsIotSerializerDecodeInterface
{
	/*
	 * Init decoder object with specified buffer
	 *
	 * pDecoderObject: the object handle for initialization
	 */
    AwsIotSerializerError_t ( * init )( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                        uint8_t * pDataBuffer,
                                        size_t maxSize );

	/*
	 * Destroy the object handle
	 *
	 * pDecoderObject: the object handle for destroy
	 */
    AwsIotSerializerError_t ( * destroy )( AwsIotSerializerDecoderObject_t * pDecoderObject );

	/*
	 * Find an child object by string key and save it to pValueObject
	 *
	 * pDecoderObject: the parent object for search
	 * pKey: string key
	 * pValueObject: result object
	 */
    AwsIotSerializerError_t ( * get )( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                       const char * pKey,
                                       AwsIotSerializerDecoderObject_t * pValueObject );

	/*
	 * Find the next object in the same value and save it to pNewDecoderObject
	 *
	 * pDecoderObject: the current object
	 * pNewDecoderObject: result object
	 */
    AwsIotSerializerError_t ( * next )( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                        AwsIotSerializerDecoderObject_t * pNewDecoderObject );

	/*
	 * Check if this is the end of container.
	 */
    uint8_t ( * isEndOfContainer )( AwsIotSerializerDecoderObject_t * pDecoderObject );

	/*
	 * enter the container object and save the first child object to pNewDecoderObject
	 *
	 * pDecoderObject: the current object. it must be a container object
	 * pNewDecoderObject: result object
	 */
    AwsIotSerializerError_t ( * stepIn )( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                          AwsIotSerializerDecoderObject_t * pNewDecoderObject );
	/*
	 * leave the container wrapping pDecoderObject. skip all the same-level objects till leaving.
	 *
	 * pDecoderObject: the current object. 
	 * pNewDecoderObject: result object
	 */
    AwsIotSerializerError_t ( * stepOut )( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                           AwsIotSerializerDecoderObject_t * pNewDecoderObject );
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
