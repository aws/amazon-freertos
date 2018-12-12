#include "aws_iot_serializer.h"
#include "cbor.h"

#define _noCborError( error )                                ( ( ( error ) == CborNoError ) || ( ( error ) == CborErrorOutOfMemory ) )

#define _castEncoderObjectToCborEncoder( pEncoderObject )    ( pEncoderObject )->pHandle

static size_t _getEncodedSize( AwsIotSerializerEncoderObject_t * pEncoderObject,
                               uint8_t * pDataBuffer );
static size_t _getExtraBufferSizeNeeded( AwsIotSerializerEncoderObject_t * pEncoderObject );
static AwsIotSerializerError_t _init( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                      uint8_t * pDataBuffer,
                                      size_t maxSize );
static AwsIotSerializerError_t _destroy( AwsIotSerializerEncoderObject_t * pEncoderObject );
static AwsIotSerializerError_t _openContainer( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                               AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                               size_t length );

static AwsIotSerializerError_t _openContainerWithKey( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                      const char * pKey,
                                                      AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                                      size_t length );
static AwsIotSerializerError_t _closeContainer( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                AwsIotSerializerEncoderObject_t * pNewEncoderObject );
static AwsIotSerializerError_t _append( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                        AwsIotSerializerScalarData_t scalarData);
static AwsIotSerializerError_t _appendKeyValue( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                const char * pKey,
                                                AwsIotSerializerScalarData_t scalarData);


AwsIotSerializerEncodeInterface_t _AwsIotSerializerCborEncoder =
{
    .getEncodedSize           = _getEncodedSize,
    .getExtraBufferSizeNeeded = _getExtraBufferSizeNeeded,
    .init                     = _init,
    .destroy                  = _destroy,
    .openContainer            = _openContainer,
    .openContainerWithKey     = _openContainerWithKey,
    .closeContainer           = _closeContainer,
    .append                   = _append,
    .appendKeyValue           = _appendKeyValue,
};


/*-----------------------------------------------------------*/

static size_t _getEncodedSize( AwsIotSerializerEncoderObject_t * pEncoderObject,
                               uint8_t * pDataBuffer )
{
    CborEncoder * pCborEncoder = _castEncoderObjectToCborEncoder( pEncoderObject );

    return cbor_encoder_get_buffer_size( pCborEncoder, pDataBuffer );
}

/*-----------------------------------------------------------*/

static size_t _getExtraBufferSizeNeeded( AwsIotSerializerEncoderObject_t * pEncoderObject )
{
    CborEncoder * pCborEncoder = _castEncoderObjectToCborEncoder( pEncoderObject );

    return cbor_encoder_get_extra_bytes_needed( pCborEncoder );
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _init( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                      uint8_t * pDataBuffer,
                                      size_t maxSize )
{
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;
    /* unuseful flags for tinycbor init */
    int unusedCborFlags = 0;

    CborEncoder * pCborEncoder = pvPortMalloc( sizeof( CborEncoder ) );

    /* malloc succeeded */
    if( pCborEncoder != NULL )
    {
        /* store the CborEncoder pointer to handle */
        pEncoderObject->pHandle = pCborEncoder;

        cbor_encoder_init( pCborEncoder, pDataBuffer, maxSize, unusedCborFlags );
    }
    else /* malloc failed */
    {
        /* pEncoderObject is untouched. */

        error = AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
    }

    return error;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _destroy( AwsIotSerializerEncoderObject_t * pEncoderObject )
{
    CborEncoder * pCborEncoder = _castEncoderObjectToCborEncoder( pEncoderObject );

    vPortFree( pCborEncoder );

    /* No other error is returned currently. */
    return AWS_IOT_SERIALIZER_SUCCESS;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _openContainer( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                               AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                               size_t length )
{
    AwsIotSerializerError_t returnError = AWS_IOT_SERIALIZER_INTERNAL_FAILURE;
    CborError cborError = CborNoError;

    CborEncoder * pOuterEncoder = _castEncoderObjectToCborEncoder( pEncoderObject );
    CborEncoder * pInnerEncoder = pvPortMalloc( sizeof( CborEncoder ) );

    /* malloc succeeded */
    if( pInnerEncoder != NULL )
    {
        /* store the CborEncoder pointer to handle */
        pNewEncoderObject->pHandle = pInnerEncoder;

        /* using CborIndefiniteLength so that numbers of items are not pre-known */
        switch( pNewEncoderObject->type )
        {
            case AWS_IOT_SERIALIZER_CONTAINER_MAP:
                cborError = cbor_encoder_create_map( pOuterEncoder, pInnerEncoder, length );
                break;

            case AWS_IOT_SERIALIZER_CONTAINER_ARRAY:
                cborError = cbor_encoder_create_array( pOuterEncoder, pInnerEncoder, length );
                break;

            case AWS_IOT_SERIALIZER_CONTAINER_STREAM:
                /* no action is required. */
                break;

            default:
                returnError = AWS_IOT_SERIALIZER_UNDEFINED_TYPE;
        }

        if( _noCborError( cborError ) )
        {
            returnError = AWS_IOT_SERIALIZER_SUCCESS;
        }
    }
    else
    {
        /* pInnerMapObject is untouched. */
        returnError = AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
    }

    return returnError;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _openContainerWithKey( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                      const char * pKey,
                                                      AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                                      size_t length )
{
	AwsIotSerializerScalarData_t keyScalarData = AwsIotSerializer_ScalarTextString(pKey);

    AwsIotSerializerError_t returnError = _append( pEncoderObject, keyScalarData );

    if( returnError == AWS_IOT_SERIALIZER_SUCCESS )
    {
        returnError = _openContainer( pEncoderObject, pNewEncoderObject, length );
    }

    return returnError;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _closeContainer( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                AwsIotSerializerEncoderObject_t * pNewEncoderObject )
{
    AwsIotSerializerError_t returnError = AWS_IOT_SERIALIZER_INTERNAL_FAILURE;
    CborError cborError;

    CborEncoder * pOuterEncoder = _castEncoderObjectToCborEncoder( pEncoderObject );
    CborEncoder * pInnerEncoder = _castEncoderObjectToCborEncoder( pNewEncoderObject );

    cborError = cbor_encoder_close_container( pOuterEncoder, pInnerEncoder );

    /* free inner encoder's memory regardless the result of "close container". */
    vPortFree( pInnerEncoder );

    if( _noCborError( cborError ) )
    {
        returnError = AWS_IOT_SERIALIZER_SUCCESS;
    }

    return returnError;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _append( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                        AwsIotSerializerScalarData_t scalarData)
{
    
	
	AwsIotSerializerError_t returnError = AWS_IOT_SERIALIZER_INTERNAL_FAILURE;
    CborError cborError;

    CborEncoder * pCborEncoder = _castEncoderObjectToCborEncoder( pEncoderObject );

    switch( scalarData.type )
    {
        case AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT:
            cborError = cbor_encode_int( pCborEncoder, scalarData.value.signedInt );
            break;

        case AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING:
            cborError = cbor_encode_text_stringz( pCborEncoder, scalarData.value.pByteString );
            break;

        case AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING:
            cborError = cbor_encode_byte_string( pCborEncoder, scalarData.value.pByteString, scalarData.value.byteStringLength );
            break;

        default:
            returnError = AWS_IOT_SERIALIZER_UNDEFINED_TYPE;
    }

    if( _noCborError( cborError ) )
    {
        returnError = AWS_IOT_SERIALIZER_SUCCESS;
    }

    return returnError;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _appendKeyValue( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                const char * pKey,
                                                AwsIotSerializerScalarData_t scalarData)
{
	AwsIotSerializerScalarData_t keyScalarData = AwsIotSerializer_ScalarTextString(pKey);
    AwsIotSerializerError_t returnError = _append( pEncoderObject, keyScalarData );

    if( returnError == AWS_IOT_SERIALIZER_SUCCESS )
    {
        returnError = _append( pEncoderObject, scalarData);
    }

    return returnError;
}



static AwsIotSerializerDataType_t _decodeToSerializerType(CborType type)
{
	switch (type) {
	case CborIntegerType: return AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
	case CborByteStringType: return AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING;
	case CborTextStringType: return AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
		case CborArrayType : return AWS_IOT_SERIALIZER_CONTAINER_ARRAY;
		case	CborMapType: return AWS_IOT_SERIALIZER_CONTAINER_MAP;
		default: return AWS_IOT_SERIALIZER_UNDEFINED;
	}
}