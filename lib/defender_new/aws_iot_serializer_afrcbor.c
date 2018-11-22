#include "aws_iot_serializer.h"

#ifdef SERIALIZER_WITH_AFRCBOR

/*-----------------------------------------------------------*/

    uint8_t * Serializer_GetDataBuffer( SerializerObject_t * object )
    {
		return CBOR_GetRawBuffer(object->cborHandle);
    }

/*-----------------------------------------------------------*/

    size_t Serializer_GetDataSize( SerializerObject_t * object )
    {
        return CBOR_GetBufferSize(object->cborHandle);
    }

/*-----------------------------------------------------------*/

    SerializerErrorCode Serializer_New( SerializerObject_t * object,
                                        size_t maxSize )
    {
		object->cborHandle = CBOR_New(maxSize);
		object->key = NULL;

        return eCborErrNoError;
    }

/*-----------------------------------------------------------*/

    SerializerContainer_t * Serializer_Start( SerializerObject_t * object )
    {
		return object;
    }

/*-----------------------------------------------------------*/

    void Serializer_End( SerializerObject_t * object )
    {
        
    }

/*-----------------------------------------------------------*/

    SerializerErrorCode Serializer_Delete( SerializerObject_t * object )
    {
		CBOR_Delete(&object->cborHandle);
		return eCborErrNoError;
    }

/*-----------------------------------------------------------*/

    void Serializer_OpenMap( SerializerContainer_t * outerContainer,
                             SerializerContainer_t * interContainer,
                             const char * key )
    {
		interContainer->cborHandle = CBOR_New(0);
		interContainer->key = key;
    }

/*-----------------------------------------------------------*/

    void Serializer_CloseMap( SerializerContainer_t * outerContainer,
                              SerializerContainer_t * interContainer )
    {
		CBOR_AssignKeyWithMap(outerContainer->cborHandle, interContainer->key, interContainer->cborHandle);
		CBOR_Delete(&interContainer->cborHandle);
    }

/*-----------------------------------------------------------*/

    void Serializer_OpenArray( SerializerContainer_t * outerContainer,
                               SerializerContainer_t * interContainer,
                               const char * key )
    {

    }

/*-----------------------------------------------------------*/

    void Serializer_CloseArray( SerializerContainer_t * outerContainer,
                                SerializerContainer_t * interContainer )
    {

    }

/*-----------------------------------------------------------*/

    SerializerErrorCode Serializer_PutInt( SerializerContainer_t * container,
                                           const char * key,
                                           uint32_t value )
    {
		CBOR_AssignKeyWithInt(container->cborHandle, key, value);
    }

#endif /* ifdef SERIALIZER_WITH_AFRCBOR */