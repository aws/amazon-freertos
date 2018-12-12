#include "aws_iot_serializer.h"
#include "cbor.h"
#include "FreeRTOSConfig.h"

#define _noCborError( error )    ( ( error  == CborNoError  ) )

#define _undefinedType( type )    ( ( type  == AWS_IOT_SERIALIZER_UNDEFINED  ) )

#define _castDecoderObjectToCborValue( pDecoderObject )    ( pDecoderObject )->pHandle

#define _castDecoderIteratorToCborValue( pIterator )    ( *pIterator )->pHandle

static AwsIotSerializerError_t _init(AwsIotSerializerDecoderObject_t * pDecoderObject,
	uint8_t * pDataBuffer,
	size_t maxSize);
static AwsIotSerializerError_t _get(AwsIotSerializerDecoderIterator_t * pIterator,
	AwsIotSerializerDecoderObject_t * pValueObject);

static AwsIotSerializerError_t _find(AwsIotSerializerDecoderObject_t * pDecoderObject,
	const char * pKey,
	AwsIotSerializerDecoderObject_t * pValueObject);
static AwsIotSerializerError_t _stepIn(AwsIotSerializerDecoderObject_t * pDecoderObject,
	AwsIotSerializerDecoderIterator_t * pIterator);
static AwsIotSerializerError_t _stepOut(AwsIotSerializerDecoderIterator_t * pIterator,
	AwsIotSerializerDecoderObject_t * pDecoderObject);
static AwsIotSerializerError_t _next(AwsIotSerializerDecoderIterator_t * pIterator);
static uint8_t _isEndOfContainer(AwsIotSerializerDecoderIterator_t * pIterator);

static void _print( uint8_t * pDataBuffer,
                    size_t dataSize );

AwsIotSerializerDecodeInterface_t _AwsIotSerializerCborDecoder =
{
    .print = _print,
	.init = _init,
	.get = _get,
	.find = _find,
	//.stepIn = _stepIn,
	//.stepOut = _stepOut,
	//.next = _next,
	//.isEndOfContainer = _isEndOfContainer
};

static AwsIotSerializerDataType_t _toSerializerType(CborType type)
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

static AwsIotSerializerError_t _createDecoderObject(CborValue * pCborValue, AwsIotSerializerDecoderObject_t * pValueObject) 
{
	AwsIotSerializerError_t returnError = AWS_IOT_SERIALIZER_SUCCESS;
	
	pValueObject->type = _toSerializerType(cbor_value_get_type(pCborValue));

	/*dataType = _toSerializerType(cbor_value_get_type(pCborValue));

	if (_undefinedType(pDecoderObject->type))
	{
		returnError = AWS_IOT_SERIALIZER_UNDEFINED_TYPE;
	}
	else {
		pDecoderObject->pHandle = pCborValue;
		pDecoderObject->type = dataType;
	}*/

	switch (pValueObject->type) {
	case  AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT: {
		int64_t i = 0;
		cbor_value_get_int(pCborValue, &i);
		pValueObject->value = (AwsIotSerializerScalarValue_t) { .signedInt = i };
		break;
	}
	case AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING: {
		size_t length;
		cbor_value_calculate_string_length(pCborValue, &length);
		if (pValueObject->value.pByteString == NULL)
		{
			cbor_value_calculate_string_length(pCborValue, &length);
			pValueObject->value.byteStringLength = length;
			returnError = AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
		}
		else
		{
			cbor_value_copy_text_string(pCborValue, pValueObject->value.pByteString, &length, NULL);
			pValueObject->value.byteStringLength = length;
		}
		break;
	}
	case AWS_IOT_SERIALIZER_CONTAINER_MAP: {
		pValueObject->pHandle = pCborValue;
		break;
	}
	default: returnError = AWS_IOT_SERIALIZER_UNDEFINED_TYPE;
	}

	return returnError;
}

static AwsIotSerializerError_t _init(AwsIotSerializerDecoderObject_t * pDecoderObject,
	uint8_t * pDataBuffer,
	size_t maxSize)
{
	CborParser * pCborParser = pvPortMalloc(sizeof(CborParser));
	CborValue * pCborValue = pvPortMalloc(sizeof(CborValue));
	AwsIotSerializerError_t returnError = AWS_IOT_SERIALIZER_SUCCESS;
	CborError cborError;
	AwsIotSerializerDataType_t dataType;

	cborError = cbor_parser_init(
		pDataBuffer,
		maxSize,
		0,
		pCborParser,
		pCborValue);

	if (_noCborError(cborError))
	{
		_createDecoderObject(pCborValue, pDecoderObject);
	}
	else 
	{
		returnError = AWS_IOT_SERIALIZER_INTERNAL_FAILURE;
	}

	return returnError;
}

static AwsIotSerializerError_t _get(AwsIotSerializerDecoderIterator_t * pIterator,
	AwsIotSerializerDecoderObject_t * pValueObject)
{
	CborValue * pCborValue = _castDecoderIteratorToCborValue(pIterator);
	return _createDecoderObject(pCborValue, pValueObject);
}

static AwsIotSerializerError_t _find(AwsIotSerializerDecoderObject_t * pDecoderObject,
	const char * pKey,
	AwsIotSerializerDecoderObject_t * pValueObject)
{
	CborValue * pCborValue = _castDecoderObjectToCborValue(pDecoderObject);
	CborValue * pNewCborValue = pvPortMalloc(sizeof(CborValue));
	
	cbor_value_map_find_value(
		pCborValue,
		pKey,
		pNewCborValue);

	_createDecoderObject(pNewCborValue, pValueObject);
}

/*-----------------------------------------------------------*/

static void _print(uint8_t * pDataBuffer,
	size_t dataSize)
{
	CborParser cborParser;
	CborValue cborValue;

	cbor_parser_init(
		pDataBuffer,
		dataSize,
		0,
		&cborParser,
		&cborValue);

	/* output to standard out */
	cbor_value_to_pretty(stdout, &cborValue);
}