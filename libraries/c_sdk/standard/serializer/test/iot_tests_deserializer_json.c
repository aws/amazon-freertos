/* Unity framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/* Serializer includes. */
#include "iot_serializer.h"

#define _encoder    _IotSerializerJsonEncoder
#define _decoder    _IotSerializerJsonDecoder

static IotSerializerDecoderObject_t rootObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
static IotSerializerDecoderObject_t childObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

static const uint8_t test_data[] =                                                                                                                                     \
    "{"                                                                                                                                                                \
    "    \"name\" : \"xQueueSend\","                                                                                                                                   \
    "    \"number\" : 3,"                                                                                                                                              \
    "    \"returns\" : { \"type\" :  \"BaseType_t\" },"                                                                                                                \
    "    \"parameters\": [{\"name\" : \"xQueue\", \"index\": 1 }, { \"name\" : \"pvItemToQueue\", \"index\" : 2 }, { \"name\" : \"xTicksToWait\" , \"index\" : 3 }], " \
    "    \"related\" : {"                                                                                                                                              \
    "       \"id\" : \"ABC123\","                                                                                                                                      \
    "       \"types\" : [ {\"type\" : \"QueueHandle_t\" }, {\"type\": \"TickType_t\"}  ]"                                                                              \
    "    }"                                                                                                                                                            \
    "}";

static const uint16_t test_data_length = sizeof( test_data ) / sizeof( test_data[ 0 ] );

TEST_GROUP( Full_Serializer_JSON_deserialize );

TEST_SETUP( Full_Serializer_JSON_deserialize )
{
    /* Init decoder object with buffer. */
    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _decoder.init( &rootObject, test_data, test_data_length ) );
}

TEST_TEAR_DOWN( Full_Serializer_JSON_deserialize )
{
    /* Destroy encoder object. */
    _decoder.destroy( &rootObject );
    _decoder.destroy( &childObject );
    TEST_ASSERT_NULL( rootObject.u.pHandle );
    TEST_ASSERT_TRUE( ( childObject.type != IOT_SERIALIZER_CONTAINER_ARRAY &&
                        childObject.type != IOT_SERIALIZER_CONTAINER_MAP ) ||
                      childObject.u.pHandle == NULL );
}

TEST_GROUP_RUNNER( Full_Serializer_JSON_deserialize )
{
    RUN_TEST_CASE( Full_Serializer_JSON_deserialize, find_key_string_value );
    RUN_TEST_CASE( Full_Serializer_JSON_deserialize, find_key_object_value );
    RUN_TEST_CASE( Full_Serializer_JSON_deserialize, find_key_array_of_objects_value );
    RUN_TEST_CASE( Full_Serializer_JSON_deserialize, find_nested_key_array_of_objects_value );
}

TEST( Full_Serializer_JSON_deserialize, find_key_string_value )
{
    const char name[] = "xQueueSend";
    IotSerializerError_t json_error = _decoder.find( &rootObject, "name", &childObject );

    TEST_ASSERT_EQUAL( json_error, IOT_SERIALIZER_SUCCESS );
    TEST_ASSERT_EQUAL( childObject.type, IOT_SERIALIZER_SCALAR_TEXT_STRING );
    TEST_ASSERT_EQUAL( childObject.u.value.u.string.length, strlen( name ) );
    TEST_ASSERT_EQUAL( 0, strncmp( ( const char * ) childObject.u.value.u.string.pString, name, strlen( name ) ) );
}

TEST( Full_Serializer_JSON_deserialize, find_key_object_value )
{
    IotSerializerError_t json_error = _decoder.find( &rootObject, "returns", &childObject );

    TEST_ASSERT_EQUAL( json_error, IOT_SERIALIZER_SUCCESS );
    TEST_ASSERT_EQUAL( childObject.type, IOT_SERIALIZER_CONTAINER_MAP );
}

TEST( Full_Serializer_JSON_deserialize, find_key_array_of_objects_value )
{
    IotSerializerError_t json_error = _decoder.find( &rootObject, "parameters", &childObject );

    TEST_ASSERT_EQUAL( json_error, IOT_SERIALIZER_SUCCESS );
    TEST_ASSERT_EQUAL( childObject.type, IOT_SERIALIZER_CONTAINER_ARRAY );
}

TEST( Full_Serializer_JSON_deserialize, find_nested_key_array_of_objects_value )
{
    IotSerializerDecoderObject_t nestedObject;


    IotSerializerError_t json_error = _decoder.find( &rootObject, "related", &nestedObject );

    TEST_ASSERT_EQUAL( json_error, IOT_SERIALIZER_SUCCESS );
    TEST_ASSERT_EQUAL( nestedObject.type, IOT_SERIALIZER_CONTAINER_MAP );

    json_error = _decoder.find( &nestedObject, "types", &childObject );
    TEST_ASSERT_EQUAL( json_error, IOT_SERIALIZER_SUCCESS );
    TEST_ASSERT_EQUAL( childObject.type, IOT_SERIALIZER_CONTAINER_ARRAY );

    _decoder.destroy( &nestedObject );
}
