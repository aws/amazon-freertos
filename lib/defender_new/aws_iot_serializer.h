#ifndef AWS_IOT_SERIALIZER_H
#define AWS_IOT_SERIALIZER_H

#include "cbor.h"
#include "aws_cbor_types.h"
#include "aws_cbor.h"

//#define SERIALIZER_WITH_TINYCBOR
#define SERIALIZER_WITH_AFRCBOR

#ifdef SERIALIZER_WITH_TINYCBOR

    #define SerializerErrorCode    CborError;

    static const CborEncoder EMPTY_CONTAINER;

    typedef struct SerializerObject_s
    {
        uint8_t * pBuffer;
        CborEncoder objectEncoder;
        CborEncoder outerMapEncoder;
        size_t maxSize;
    } SerializerObject_t;

    typedef CborEncoder    SerializerContainer_t;

#elif defined SERIALIZER_WITH_AFRCBOR

    #define SerializerErrorCode    cborError_t;

	typedef struct SerializerContainer_s
	{
		CBORHandle_t cborHandle;
		char * key;
	} SerializerContainer_t;

	typedef SerializerContainer_t  SerializerObject_t;

#elif defined SERIALIZER_WITH_JSON

#endif /* ifdef SERIALIZER_WITH_TINYCBOR */

SerializerErrorCode Serializer_New( SerializerObject_t * object,
                                    size_t maxSize );
SerializerErrorCode Serializer_Delete( SerializerObject_t * object );

SerializerContainer_t * Serializer_Start( SerializerObject_t * object );
void Serializer_End( SerializerObject_t * object );

uint8_t * Serializer_GetDataBuffer( SerializerObject_t * object );
size_t Serializer_GetDataSize( SerializerObject_t * object );

void Serializer_OpenMap( SerializerContainer_t * outerContainer,
                         SerializerContainer_t * interContainer,
                         const char * key );
void Serializer_CloseMap( SerializerContainer_t * outerContainer,
                          SerializerContainer_t * interContainer );

void Serializer_OpenArray( SerializerContainer_t * outerContainer,
                           SerializerContainer_t * interContainer,
                           const char * key );
void Serializer_CloseArray( SerializerContainer_t * outerContainer,
                            SerializerContainer_t * interContainer );

SerializerErrorCode Serializer_PutString( SerializerContainer_t * container,
                                          const char * key,
                                          const char * value );

SerializerErrorCode Serializer_PutInt( SerializerContainer_t * container,
                                       const char * key,
                                       uint32_t value );

#endif /* ifndef AWS_IOT_SERIALIZER_H */