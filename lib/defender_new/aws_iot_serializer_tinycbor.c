#include "aws_iot_serializer.h"

#ifdef SERIALIZER_WITH_TINYCBOR

/*-----------------------------------------------------------*/

    uint8_t * Serializer_GetDataBuffer( SerializerObject_t * object )
    {
        return object->pBuffer;
    }

/*-----------------------------------------------------------*/

    size_t Serializer_GetDataSize( SerializerObject_t * object )
    {
        return cbor_encoder_get_buffer_size( &( object->objectEncoder ), object->pBuffer );
    }

/*-----------------------------------------------------------*/

    SerializerErrorCode Serializer_New( SerializerObject_t * object,
                                        size_t maxSize )
    {
        uint8_t * pBuffer = pvPortMalloc( maxSize * sizeof( uint8_t ) );

        object->pBuffer = pBuffer;
        object->maxSize = maxSize;

        cbor_encoder_init( &( object->objectEncoder ), object->pBuffer, object->maxSize, 0 );

        return CborNoError;
    }

/*-----------------------------------------------------------*/

    SerializerContainer_t * Serializer_Start( SerializerObject_t * object )
    {
        cbor_encoder_create_map( &( object->objectEncoder ), &( object->outerMapEncoder ), CborIndefiniteLength );

        return &( object->outerMapEncoder );
    }

/*-----------------------------------------------------------*/

    void Serializer_End( SerializerObject_t * object )
    {
        cbor_encoder_close_container( &( object->objectEncoder ), &( object->outerMapEncoder ) );
    }

/*-----------------------------------------------------------*/

    SerializerErrorCode Serializer_Delete( SerializerObject_t * object )
    {
        vPortFree( object->pBuffer );
        object->outerMapEncoder = EMPTY_CONTAINER;
        object->objectEncoder = EMPTY_CONTAINER;

        return CborNoError;
    }

/*-----------------------------------------------------------*/

    void Serializer_OpenMap( SerializerContainer_t * outerContainer,
                             SerializerContainer_t * interContainer,
                             const char * key )
    {
        cbor_encode_text_stringz( outerContainer, key );
        cbor_encoder_create_map( outerContainer, interContainer, CborIndefiniteLength );
    }

/*-----------------------------------------------------------*/

    void Serializer_CloseMap( SerializerContainer_t * outerContainer,
                              SerializerContainer_t * interContainer )
    {
        cbor_encoder_close_container( outerContainer, interContainer );
        *interContainer = EMPTY_CONTAINER;
    }

/*-----------------------------------------------------------*/

    void Serializer_OpenArray( SerializerContainer_t * outerContainer,
                               SerializerContainer_t * interContainer,
                               const char * key )
    {
        cbor_encode_text_stringz( outerContainer, key );
        cbor_encoder_create_array( outerContainer, interContainer, CborIndefiniteLength );
    }

/*-----------------------------------------------------------*/

    void Serializer_CloseArray( SerializerContainer_t * outerContainer,
                                SerializerContainer_t * interContainer )
    {
        cbor_encoder_close_container( outerContainer, interContainer );
        *interContainer = EMPTY_CONTAINER;
    }

/*-----------------------------------------------------------*/

    SerializerErrorCode Serializer_PutInt( SerializerContainer_t * container,
                                           const char * key,
                                           uint32_t value )
    {
        cbor_encode_text_stringz( container, key );
        cbor_encode_uint( container, value );
    }

#endif /* ifdef SERIALIZER_WITH_TINYCBOR */