#include <string.h>

#include "unity.h"

/* Include paths for public enums, structures, and macros. */
#include "mqtt_lightweight.h"

#define MQTT_MAX_REMAINING_LENGTH                   ( 268435455UL )
#define MQTT_PACKET_CONNACK_REMAINING_LENGTH        ( ( uint8_t ) 2U )    /**< @brief A CONNACK packet always has a "Remaining length" of 2. */
#define MQTT_PACKET_CONNACK_SESSION_PRESENT_MASK    ( ( uint8_t ) 0x01U ) /**< @brief The "Session Present" bit is always the lowest bit. */
#define MQTT_PACKET_SIMPLE_ACK_REMAINING_LENGTH     ( ( uint8_t ) 2 )     /**< @brief PUBACK, PUBREC, PUBREl, PUBCOMP, UNSUBACK Remaining length. */
#define MQTT_PACKET_PINGRESP_REMAINING_LENGTH       ( 0U )                /**< @brief A PINGRESP packet always has a "Remaining length" of 0. */
#define MQTT_PACKET_PUBACK_REMAINING_LENGTH         ( 2U )
#define MQTT_PACKET_UNSUBACK_REMAINING_LENGTH       ( 2U )

/*
 * Client identifier and length to use for the MQTT API tests.
 */
#define CLIENT_IDENTIFIER                           ( "test" )                                           /**< @brief Client identifier. */
#define CLIENT_IDENTIFIER_LENGTH                    ( ( uint16_t ) ( sizeof( CLIENT_IDENTIFIER ) - 1 ) ) /**< @brief Length of client identifier. */

/*
 * Will topic name and length to use for the MQTT API tests.
 */
#define TEST_TOPIC_NAME                             ( "/test/topic" )                                  /**< @brief An arbitrary topic name. */
#define TEST_TOPIC_NAME_LENGTH                      ( ( uint16_t ) ( sizeof( TEST_TOPIC_NAME ) - 1 ) ) /**< @brief Length of topic name. */


/**
 * @brief MQTT protocol version 3.1.1.
 */
#define MQTT_VERSION_3_1_1                  ( ( uint8_t ) 4U )

/**
 * @brief Test-defined macro for MQTT username.
 */
#define MQTT_TEST_USERNAME                  "username"
#define MQTT_TEST_USERNAME_LEN              ( sizeof( MQTT_TEST_USERNAME ) - 1 )

/**
 * @brief Test-defined macro for MQTT password.
 */
#define MQTT_TEST_PASSWORD                  "password"
#define MQTT_TEST_PASSWORD_LEN              ( sizeof( MQTT_TEST_PASSWORD ) - 1 )

/**
 * @brief Test-defined macro for MQTT topic.
 */
#define MQTT_TEST_TOPIC                     "topic"
#define MQTT_TEST_TOPIC_LEN                 ( sizeof( MQTT_TEST_TOPIC ) - 1 )

/**
 * @brief Length of the client identifier.
 */
#define MQTT_CLIENT_IDENTIFIER_LEN          ( sizeof( MQTT_CLIENT_IDENTIFIER ) - 1 )

/**
 * @brief Payload for will info.
 */
#define MQTT_SAMPLE_PAYLOAD                 "payload"
#define MQTT_SAMPLE_PAYLOAD_LEN             ( sizeof( MQTT_SAMPLE_PAYLOAD ) - 1 )

/* MQTT CONNECT flags. */
#define MQTT_CONNECT_FLAG_CLEAN             ( 1 )         /**< @brief Clean session. */
#define MQTT_CONNECT_FLAG_WILL              ( 2 )         /**< @brief Will present. */
#define MQTT_CONNECT_FLAG_WILL_QOS1         ( 3 )         /**< @brief Will QoS 1. */
#define MQTT_CONNECT_FLAG_WILL_QOS2         ( 4 )         /**< @brief Will QoS 2. */
#define MQTT_CONNECT_FLAG_WILL_RETAIN       ( 5 )         /**< @brief Will retain. */
#define MQTT_CONNECT_FLAG_PASSWORD          ( 6 )         /**< @brief Password present. */
#define MQTT_CONNECT_FLAG_USERNAME          ( 7 )         /**< @brief User name present. */

/**
 * @brief The Remaining Length field of MQTT disconnect packets, per MQTT spec.
 */
#define MQTT_DISCONNECT_REMAINING_LENGTH    ( ( uint8_t ) 0 )

/**
 * @brief Set a bit in an 8-bit unsigned integer.
 */
#define UINT8_SET_BIT( x, position )      ( ( x ) = ( uint8_t ) ( ( x ) | ( 0x01U << ( position ) ) ) )

/**
 * @brief Macro for checking if a bit is set in a 1-byte unsigned int.
 *
 * @param[in] x The unsigned int to check.
 * @param[in] position Which bit to check.
 */
#define UINT8_CHECK_BIT( x, position )    ( ( ( x ) & ( 0x01U << ( position ) ) ) == ( 0x01U << ( position ) ) )

/**
 * @brief Get the high byte of a 16-bit unsigned integer.
 */
#define UINT16_HIGH_BYTE( x )             ( ( uint8_t ) ( ( x ) >> 8 ) )

/**
 * @brief Get the low byte of a 16-bit unsigned integer.
 */
#define UINT16_LOW_BYTE( x )              ( ( uint8_t ) ( ( x ) & 0x00ffU ) )

/**
 * @brief Maximum number of bytes in the Remaining Length field is four according
 * to MQTT 3.1.1 spec.
 */
#define MQTT_REMAINING_BUFFER_MAX_LENGTH    ( 4 )

/**
 * @brief Length of buffer padding to use in under/overflow checks.
 */
#define BUFFER_PADDING_LENGTH               ( 4 )

/**
 * @brief Byte to use for buffer padding in under/overflow checks.
 */
#define BUFFER_PADDING_BYTE                 ( 0xA5 )

/**
 * @brief Length of the MQTT network buffer.
 */
#define MQTT_TEST_BUFFER_LENGTH             ( 1024 )

static uint8_t remainingLengthBuffer[ MQTT_REMAINING_BUFFER_MAX_LENGTH ] = { 0 };

static uint8_t encodedStringBuffer[ MQTT_TEST_BUFFER_LENGTH ] = { 0 };

static uint8_t mqttBuffer[ MQTT_TEST_BUFFER_LENGTH ] = { 0 };

/* ============================   UNITY FIXTURES ============================ */

/* Called before each test method. */
void setUp( void )
{
}

/* Called after each test method. */
void tearDown( void )
{
}

/* Called at the beginning of the whole suite. */
void suiteSetUp()
{
}

/* Called at the end of the whole suite. */
int suiteTearDown( int numFailures )
{
    return numFailures;
}

/* ========================================================================== */

/**
 * @brief Mock successful transport receive by reading data from a buffer.
 */
static int32_t mockReceive( NetworkContext_t context,
                            void * pBuffer,
                            size_t bytesToRecv )
{
    uint8_t * returnBuffer = ( uint8_t * ) pBuffer;
    uint8_t * mockNetwork;
    size_t bytesRead = 0;

    /* Treat network context as pointer to buffer for mocking  */
    mockNetwork = ( *( uint8_t ** ) context );

    while( bytesRead++ < bytesToRecv )
    {
        /* Read single byte and advance buffer. */
        *returnBuffer++ = *mockNetwork++;
    }

    /* Move stream by bytes read. */
    ( *( uint8_t ** ) context ) = mockNetwork;

    return bytesToRecv;
}

/**
 * @brief Mock transport receive with no data available.
 */
static int32_t mockReceiveNoData( NetworkContext_t context,
                                  void * pBuffer,
                                  size_t bytesToRecv )
{
    return 0;
}

/**
 * @brief Mock transport receive failure.
 */
static int32_t mockReceiveFailure( NetworkContext_t context,
                                   void * pBuffer,
                                   size_t bytesToRecv )
{
    return -1;
}

/**
 * @brief Mock transport receive that succeeds once, then fails.
 */
static int32_t mockReceiveSucceedThenFail( NetworkContext_t context,
                                           void * pBuffer,
                                           size_t bytesToRecv )
{
    int32_t retVal = 0;
    static int counter = 0;

    if( counter++ )
    {
        retVal = mockReceiveFailure( context, pBuffer, bytesToRecv );
        counter = 0;
    }
    else
    {
        retVal = mockReceive( context, pBuffer, bytesToRecv );
    }

    return retVal;
}

/* ========================================================================== */

/**
 * @brief Initialize pNetworkBuffer using static buffer.
 *
 * @param[in] pNetworkBuffer Network buffer provided for the context.
 */
static void setupNetworkBuffer( MQTTFixedBuffer_t * const pNetworkBuffer )
{
    pNetworkBuffer->pBuffer = mqttBuffer;
    pNetworkBuffer->size = MQTT_TEST_BUFFER_LENGTH;
}

/**
 * @brief Initialize pConnectInfo using test-defined macros.
 *
 * @param[in] pConnectInfo MQTT CONNECT packet parameters.
 */
static void setupConnectInfo( MQTTConnectInfo_t * const pConnectInfo )
{
    pConnectInfo->cleanSession = true;
    pConnectInfo->pClientIdentifier = MQTT_CLIENT_IDENTIFIER;
    pConnectInfo->clientIdentifierLength = MQTT_CLIENT_IDENTIFIER_LEN;
    pConnectInfo->keepAliveSeconds = 0;
    pConnectInfo->pUserName = MQTT_TEST_USERNAME;
    pConnectInfo->userNameLength = MQTT_TEST_USERNAME_LEN;
    pConnectInfo->pPassword = MQTT_TEST_PASSWORD;
    pConnectInfo->passwordLength = MQTT_TEST_PASSWORD_LEN;
}

/**
 * @brief Initialize pWillInfo using test-defined macros.
 *
 * @param[in] pWillInfo Last Will and Testament.
 */
static void setupWillInfo( MQTTPublishInfo_t * const pWillInfo )
{
    pWillInfo->pPayload = MQTT_SAMPLE_PAYLOAD;
    pWillInfo->payloadLength = MQTT_SAMPLE_PAYLOAD_LEN;
    pWillInfo->pTopicName = MQTT_CLIENT_IDENTIFIER;
    pWillInfo->topicNameLength = MQTT_CLIENT_IDENTIFIER_LEN;
    pWillInfo->dup = true;
    pWillInfo->qos = MQTTQoS0;
    pWillInfo->retain = true;
}

/**
 * @brief Encode remaining length into pDestination for packet serialization
 * using MQTT v3.1.1 spec.
 *
 * @param[in] pDestination Buffer to write encoded remaining length.
 * @param[in] length Actual remaining length.
 */
static size_t encodeRemainingLength( uint8_t * pDestination,
                                     size_t length )
{
    uint8_t lengthByte;
    uint8_t * pLengthEnd = NULL;
    size_t remainingLength = length;

    TEST_ASSERT_NOT_NULL( pDestination );

    pLengthEnd = pDestination;

    /* This algorithm is copied from the MQTT v3.1.1 spec. */
    do
    {
        lengthByte = ( uint8_t ) ( remainingLength % 128U );
        remainingLength = remainingLength / 128U;

        /* Set the high bit of this byte, indicating that there's more data. */
        if( remainingLength > 0U )
        {
            UINT8_SET_BIT( lengthByte, 7 );
        }

        /* Output a single encoded byte. */
        *pLengthEnd = lengthByte;
        pLengthEnd++;
    } while( remainingLength > 0U );

    return ( size_t ) ( pLengthEnd - pDestination );
}

/**
 * @brief Encode UTF-8 string and its length into pDestination for
 * packet serialization.
 *
 * @param[in] pDestination Buffer to write encoded string.
 * @param[in] source String to encode.
 * @param[in] sourceLength Length of the string to encode.
 */
static size_t encodeString( uint8_t * pDestination,
                            const char * source,
                            uint16_t sourceLength )
{
    uint8_t * pBuffer = NULL;

    /* Typecast const char * typed source buffer to const uint8_t *.
     * This is to use same type buffers in memcpy. */
    const uint8_t * pSourceBuffer = ( const uint8_t * ) source;

    TEST_ASSERT_NOT_NULL( pSourceBuffer );
    TEST_ASSERT_NOT_NULL( pDestination );

    pBuffer = pDestination;

    /* The first byte of a UTF-8 string is the high byte of the string length. */
    *pBuffer = UINT16_HIGH_BYTE( sourceLength );
    pBuffer++;

    /* The second byte of a UTF-8 string is the low byte of the string length. */
    *pBuffer = UINT16_LOW_BYTE( sourceLength );
    pBuffer++;

    /* Copy the string into pBuffer. */
    ( void ) memcpy( pBuffer, pSourceBuffer, sourceLength );

    /* Return the pointer to the end of the encoded string. */
    pBuffer += sourceLength;

    return ( size_t ) ( pBuffer - pDestination );
}

/**
 * @brief Pad beginning and end of buffer with non-zero bytes to be used in
 * checking for under/overflow after serialization.
 *
 * @param[in] pBuffer Buffer to pad.
 * @param[in] bufferLength Total length of buffer.
 */
static void padAndResetBuffer( uint8_t * pBuffer,
                               size_t bufferLength )
{
    int i = 0;

    for( i = 0; i < BUFFER_PADDING_LENGTH; i++ )
    {
        pBuffer[ i ] = BUFFER_PADDING_BYTE;
        pBuffer[ bufferLength - 1 - i ] = BUFFER_PADDING_BYTE;
    }

    /* Zero out rest of buffer. */
    memset( ( void * ) &pBuffer[ BUFFER_PADDING_LENGTH ], 0x0, bufferLength - 2 * BUFFER_PADDING_LENGTH );
}

/**
 * @brief Test buffer for under/overflow.
 *
 * @param[in] pBuffer Buffer to check.
 * @param[in] bufferLength Total length of buffer.
 */
static void checkBufferOverflow( uint8_t * pBuffer,
                                 size_t bufferLength )
{
    /* Check beginning of buffer. */
    TEST_ASSERT_EACH_EQUAL_UINT8( BUFFER_PADDING_BYTE,
                                  pBuffer,
                                  BUFFER_PADDING_LENGTH );
    /* Check end. */
    TEST_ASSERT_EACH_EQUAL_UINT8( BUFFER_PADDING_BYTE,
                                  pBuffer + bufferLength - BUFFER_PADDING_LENGTH,
                                  BUFFER_PADDING_LENGTH );
}

/* ========================================================================== */

/**
 * @brief Tests that MQTT_GetConnectPacketSize works as intended.
 */
void test_MQTT_GetConnectPacketSize( void )
{
    MQTTConnectInfo_t connectInfo;
    size_t remainingLength = 0;
    size_t packetSize = 0;
    MQTTStatus_t status = MQTTSuccess;
    MQTTPublishInfo_t willInfo = { 0 };

    /* Call MQTT_GetConnectPacketSize() with various combinations of
     * incorrect paramters */

    status = MQTT_GetConnectPacketSize( NULL, NULL, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_GetConnectPacketSize( &connectInfo, NULL, NULL, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_GetConnectPacketSize( &connectInfo, NULL, &remainingLength, NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Verify empty connect info fails. */
    memset( ( void * ) &connectInfo, 0x0, sizeof( connectInfo ) );
    status = MQTT_GetConnectPacketSize( &connectInfo, NULL, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Verify empty client identifier fails. */
    connectInfo.pClientIdentifier = CLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = 0;
    status = MQTT_GetConnectPacketSize( &connectInfo, NULL, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    connectInfo.pClientIdentifier = NULL;
    connectInfo.clientIdentifierLength = CLIENT_IDENTIFIER_LENGTH;
    status = MQTT_GetConnectPacketSize( &connectInfo, NULL, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Connect packet too large. */
    memset( ( void * ) &connectInfo, 0x0, sizeof( connectInfo ) );
    connectInfo.pClientIdentifier = CLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = UINT16_MAX;
    connectInfo.pPassword = "";
    connectInfo.passwordLength = UINT16_MAX;
    connectInfo.pUserName = "";
    connectInfo.userNameLength = UINT16_MAX;
    willInfo.pTopicName = TEST_TOPIC_NAME;
    willInfo.topicNameLength = UINT16_MAX;
    willInfo.payloadLength = UINT16_MAX + 2;
    status = MQTT_GetConnectPacketSize( &connectInfo, &willInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    /* Verify good case */
    memset( ( void * ) &connectInfo, 0x0, sizeof( connectInfo ) );
    connectInfo.cleanSession = true;
    connectInfo.pClientIdentifier = "TEST";
    connectInfo.clientIdentifierLength = 4;
    status = MQTT_GetConnectPacketSize( &connectInfo, NULL, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    /* Make sure remaining size returned is 16. */
    TEST_ASSERT_EQUAL_INT( 16, remainingLength );
    /* Make sure packet size is 18. */
    TEST_ASSERT_EQUAL_INT( 18, packetSize );

    /* With will. These parameters will cause the packet to be
     * 4 + 2 + 8 + 2 = 16 bytes larger. */
    memset( ( void * ) &willInfo, 0x0, sizeof( willInfo ) );
    willInfo.pTopicName = "test";
    willInfo.topicNameLength = 4;
    willInfo.pPayload = "testload";
    willInfo.payloadLength = 8;
    status = MQTT_GetConnectPacketSize( &connectInfo, &willInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    /* Make sure remaining size returned is 32 = 16 + 16. */
    TEST_ASSERT_EQUAL_INT( 32, remainingLength );
    /* Make sure packet size is 34 = 18 + 16. */
    TEST_ASSERT_EQUAL_INT( 34, packetSize );

    /* With username and password. This will add 4 + 2 + 4 + 2 = 12 bytes. */
    connectInfo.cleanSession = true;
    connectInfo.pUserName = "USER";
    connectInfo.userNameLength = 4;
    connectInfo.pPassword = "PASS";
    connectInfo.passwordLength = 4;
    status = MQTT_GetConnectPacketSize( &connectInfo, NULL, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    /* Make sure remaining size returned is 28 = 16 + 12. */
    TEST_ASSERT_EQUAL_INT( 28, remainingLength );
    /* Make sure packet size is 30 = 18 + 12. */
    TEST_ASSERT_EQUAL_INT( 30, packetSize );
}

/**
 * @brief Tests that MQTT_SerializeConnect works as intended.
 */
void test_MQTT_SerializeConnect( void )
{
    MQTTConnectInfo_t connectInfo;
    MQTTPublishInfo_t willInfo;
    size_t remainingLength = 0;
    uint8_t buffer[ 70 + 2 * BUFFER_PADDING_LENGTH ];
    size_t bufferSize = sizeof( buffer ) - 2 * BUFFER_PADDING_LENGTH;
    size_t packetSize = bufferSize;
    MQTTStatus_t status = MQTTSuccess;
    MQTTFixedBuffer_t fixedBuffer = { .pBuffer = &buffer[ BUFFER_PADDING_LENGTH ], .size = bufferSize };

    /* Verify bad parameter errors. */
    status = MQTT_SerializeConnect( NULL, &willInfo, remainingLength, &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );
    status = MQTT_SerializeConnect( &connectInfo, &willInfo, remainingLength, NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    memset( ( void * ) &connectInfo, 0x0, sizeof( connectInfo ) );
    status = MQTT_SerializeConnect( &connectInfo, NULL, 120, &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTNoMemory, status );

    /* Good case succeeds */
    /* Calculate packet size. */
    memset( ( void * ) &connectInfo, 0x0, sizeof( connectInfo ) );
    connectInfo.pClientIdentifier = "TEST";
    connectInfo.clientIdentifierLength = 4;
    status = MQTT_GetConnectPacketSize( &connectInfo, NULL, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    /* Make sure buffer has enough space */
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, bufferSize );
    /* Make sure test succeeds. */
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializeConnect( &connectInfo, NULL, remainingLength, &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    checkBufferOverflow( buffer, sizeof( buffer ) );

    /* Encode user name. Also try clean session. */
    connectInfo.cleanSession = true;
    connectInfo.pUserName = "USER";
    connectInfo.userNameLength = 4;
    connectInfo.pPassword = "PASS";
    connectInfo.passwordLength = 4;
    status = MQTT_GetConnectPacketSize( &connectInfo, NULL, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, bufferSize );
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializeConnect( &connectInfo, NULL, remainingLength, &fixedBuffer );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    checkBufferOverflow( buffer, sizeof( buffer ) );

    /* Serialize connect with LWT. */
    /* Test for NULL topic name. */
    ( void ) memset( &willInfo, 0x00, sizeof( MQTTPublishInfo_t ) );
    willInfo.retain = true;
    willInfo.qos = MQTTQoS1;
    willInfo.pPayload = "test";
    willInfo.payloadLength = ( uint16_t ) strlen( willInfo.pPayload );
    status = MQTT_GetConnectPacketSize( &connectInfo, &willInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, bufferSize );
    status = MQTT_SerializeConnect( &connectInfo, &willInfo, remainingLength, &fixedBuffer );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    /* Success. */
    ( void ) memset( &willInfo, 0x00, sizeof( MQTTPublishInfo_t ) );
    willInfo.retain = true;
    willInfo.qos = MQTTQoS1;
    willInfo.pTopicName = "test";
    willInfo.topicNameLength = ( uint16_t ) strlen( willInfo.pTopicName );
    willInfo.pPayload = "test";
    willInfo.payloadLength = ( uint16_t ) strlen( willInfo.pPayload );
    status = MQTT_GetConnectPacketSize( &connectInfo, &willInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, bufferSize );
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializeConnect( &connectInfo, &willInfo, remainingLength, &fixedBuffer );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    checkBufferOverflow( buffer, sizeof( buffer ) );

    /* Again with QoS 2 and 0. */

    willInfo.qos = MQTTQoS2;
    status = MQTT_GetConnectPacketSize( &connectInfo, &willInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, bufferSize );
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializeConnect( &connectInfo, &willInfo, remainingLength, &fixedBuffer );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    checkBufferOverflow( buffer, sizeof( buffer ) );

    willInfo.qos = MQTTQoS0;
    willInfo.retain = false;
    /* NULL payload is acceptable. */
    willInfo.pPayload = NULL;
    willInfo.payloadLength = 0;
    status = MQTT_GetConnectPacketSize( &connectInfo, &willInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, bufferSize );
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializeConnect( &connectInfo, &willInfo, remainingLength, &fixedBuffer );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    checkBufferOverflow( buffer, sizeof( buffer ) );
}

/* ========================================================================== */

/**
 * @brief Tests that MQTT_GetSubscribePacketSize works as intended.
 */
void test_MQTT_GetSubscribePacketSize( void )
{
    MQTTSubscribeInfo_t subscriptionList;
    size_t subscriptionCount = 0;
    size_t remainingLength = 0;
    size_t packetSize = 0;
    MQTTStatus_t status = MQTTSuccess;
    MQTTSubscribeInfo_t fourThousandSubscriptions[ 4096 ] = { 0 };
    int i;

    /* Verify parameters. */

    status = MQTT_GetSubscribePacketSize( NULL,
                                          subscriptionCount,
                                          &remainingLength,
                                          &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_GetSubscribePacketSize( &subscriptionList,
                                          subscriptionCount,
                                          NULL,
                                          &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_GetSubscribePacketSize( &subscriptionList,
                                          subscriptionCount,
                                          &remainingLength,
                                          NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );


    /* Verify empty subscription list fails.  */
    memset( ( void * ) &subscriptionList, 0x0, sizeof( subscriptionList ) );
    subscriptionCount = 0;
    status = MQTT_GetSubscribePacketSize( &subscriptionList,
                                          subscriptionCount,
                                          &remainingLength,
                                          &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Verify packet size cannot exceed limit. Note the max remaining length of
     * an MQTT packet is 2^28-1 = 268435455, or 256MiB. Since the only way to increase
     * the subscribe packet size is with the topic filters of the subscriptions
     * (the lengths of which are only 2 bytes), we need at least
     * 2^28 / 2^16 = 2^12 = 4096 of them. */
    for( i = 0; i < 4096; i++ )
    {
        fourThousandSubscriptions[ i ].topicFilterLength = UINT16_MAX;
    }

    subscriptionCount = sizeof( fourThousandSubscriptions ) / sizeof( fourThousandSubscriptions[ 0 ] );
    status = MQTT_GetSubscribePacketSize( fourThousandSubscriptions,
                                          subscriptionCount,
                                          &remainingLength,
                                          &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Verify good case. */
    memset( ( void * ) &subscriptionList, 0x0, sizeof( subscriptionList ) );
    subscriptionList.qos = MQTTQoS0;
    subscriptionList.pTopicFilter = "/example/topic";
    subscriptionList.topicFilterLength = sizeof( "/example/topic" );
    subscriptionCount = sizeof( subscriptionList ) / sizeof( MQTTSubscribeInfo_t );
    status = MQTT_GetSubscribePacketSize( &subscriptionList,
                                          subscriptionCount,
                                          &remainingLength,
                                          &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_GREATER_THAN( remainingLength, packetSize );
}

/**
 * @brief Tests that MQTT_GetUnsubscribePacketSize works as intended.
 */
void test_MQTT_GetUnsubscribePacketSize( void )
{
    MQTTSubscribeInfo_t subscriptionList;
    size_t subscriptionCount = 0;
    size_t remainingLength = 0;
    size_t packetSize = 0;
    MQTTStatus_t status = MQTTSuccess;

    /* Verify parameters. */

    status = MQTT_GetUnsubscribePacketSize( NULL,
                                            subscriptionCount,
                                            &remainingLength,
                                            &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_GetUnsubscribePacketSize( &subscriptionList,
                                            subscriptionCount,
                                            NULL,
                                            &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_GetUnsubscribePacketSize( &subscriptionList,
                                            subscriptionCount,
                                            &remainingLength,
                                            NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );


    /* Verify empty subscription list fails.  */
    memset( ( void * ) &subscriptionList, 0x0, sizeof( subscriptionList ) );
    subscriptionCount = 0;
    status = MQTT_GetUnsubscribePacketSize( &subscriptionList,
                                            subscriptionCount,
                                            &remainingLength,
                                            &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Verify good case. */
    memset( ( void * ) &subscriptionList, 0x0, sizeof( subscriptionList ) );
    subscriptionList.qos = MQTTQoS0;
    subscriptionList.pTopicFilter = "/example/topic";
    subscriptionList.topicFilterLength = sizeof( "/example/topic" );
    subscriptionCount = sizeof( subscriptionList ) / sizeof( MQTTSubscribeInfo_t );
    status = MQTT_GetUnsubscribePacketSize( &subscriptionList,
                                            subscriptionCount,
                                            &remainingLength,
                                            &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_GREATER_THAN( remainingLength, packetSize );
}

/**
 * @brief Tests that MQTT_SerializeSubscribe works as intended.
 */
void test_MQTT_SerializeSubscribe( void )
{
    MQTTSubscribeInfo_t subscriptionList;
    size_t subscriptionCount = 1;
    size_t remainingLength = 0;
    uint16_t packetIdentifier;
    uint8_t buffer[ 25 + 2 * BUFFER_PADDING_LENGTH ];
    size_t bufferSize = sizeof( buffer ) - 2 * BUFFER_PADDING_LENGTH;
    size_t packetSize = bufferSize;
    MQTTStatus_t status = MQTTSuccess;
    MQTTFixedBuffer_t fixedBuffer = { .pBuffer = &buffer[ BUFFER_PADDING_LENGTH ], .size = bufferSize };
    uint8_t expectedPacket[ 100 ];
    uint8_t * pIterator = expectedPacket;

    const uint16_t PACKET_ID = 1;

    /* Verify bad parameters fail. */
    status = MQTT_SerializeSubscribe( NULL,
                                      subscriptionCount,
                                      PACKET_ID,
                                      remainingLength,
                                      &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_SerializeSubscribe( &subscriptionList,
                                      subscriptionCount,
                                      0,
                                      remainingLength,
                                      &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_SerializeSubscribe( &subscriptionList,
                                      subscriptionCount,
                                      PACKET_ID,
                                      remainingLength,
                                      NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Get correct values of packet size and remaining length. */
    memset( ( void * ) &subscriptionList, 0x0, sizeof( subscriptionList ) );
    subscriptionList.qos = MQTTQoS0;
    subscriptionList.pTopicFilter = "/example/topic";
    subscriptionList.topicFilterLength = sizeof( "/example/topic" );
    subscriptionCount = sizeof( subscriptionList ) / sizeof( MQTTSubscribeInfo_t );
    status = MQTT_GetSubscribePacketSize( &subscriptionList,
                                          subscriptionCount,
                                          &remainingLength,
                                          &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    /* Make sure buffer has enough space */
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, bufferSize );

    /* Make sure subscription count of zero fails. */
    status = MQTT_SerializeSubscribe( &subscriptionList,
                                      0,
                                      PACKET_ID,
                                      remainingLength,
                                      &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Test if buffer is too small. */
    fixedBuffer.size = 1;
    status = MQTT_SerializeSubscribe( &subscriptionList,
                                      subscriptionCount,
                                      PACKET_ID,
                                      remainingLength,
                                      &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTNoMemory, status );
    fixedBuffer.size = bufferSize;

    /* Make sure success is returned for good case. */
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializeSubscribe( &subscriptionList,
                                      subscriptionCount,
                                      PACKET_ID,
                                      remainingLength,
                                      &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    checkBufferOverflow( buffer, sizeof( buffer ) );

    /* MQTT SUBSCRIBE packet format:
     * 0x82 (1 byte)
     * Remaining length (1-4 bytes)
     * Packet ID (2 bytes)
     * Topic filters (series of 2 byte lengths followed by filter, then QoS) (variable) */
    *pIterator++ = MQTT_PACKET_TYPE_SUBSCRIBE;
    pIterator += encodeRemainingLength( pIterator, remainingLength );
    *pIterator++ = UINT16_HIGH_BYTE( PACKET_ID );
    *pIterator++ = UINT16_LOW_BYTE( PACKET_ID );
    pIterator += encodeString( pIterator, subscriptionList.pTopicFilter, subscriptionList.topicFilterLength );
    *pIterator++ = subscriptionList.qos;
    TEST_ASSERT_EQUAL_MEMORY( expectedPacket, &buffer[ BUFFER_PADDING_LENGTH ], packetSize );
}

/**
 * @brief Tests that MQTT_SerializeUnsubscribe works as intended.
 */
void test_MQTT_SerializeUnsubscribe( void )
{
    MQTTSubscribeInfo_t subscriptionList;
    size_t subscriptionCount = 1;
    size_t remainingLength = 0;
    uint16_t packetIdentifier;
    uint8_t buffer[ 25 + 2 * BUFFER_PADDING_LENGTH ];
    size_t bufferSize = sizeof( buffer ) - 2 * BUFFER_PADDING_LENGTH;
    size_t packetSize = bufferSize;
    MQTTStatus_t status = MQTTSuccess;
    MQTTFixedBuffer_t fixedBuffer = { .pBuffer = &buffer[ BUFFER_PADDING_LENGTH ], .size = bufferSize };
    uint8_t expectedPacket[ 100 ];
    uint8_t * pIterator = expectedPacket;

    const uint16_t PACKET_ID = 1;

    status = MQTT_SerializeUnsubscribe( NULL,
                                        subscriptionCount,
                                        PACKET_ID,
                                        remainingLength,
                                        &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_SerializeUnsubscribe( &subscriptionList,
                                        subscriptionCount,
                                        0,
                                        remainingLength,
                                        &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_SerializeUnsubscribe( &subscriptionList,
                                        subscriptionCount,
                                        PACKET_ID,
                                        remainingLength,
                                        NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Get correct values of packetsize and remaining length. */
    memset( ( void * ) &subscriptionList, 0x0, sizeof( subscriptionList ) );
    subscriptionList.qos = MQTTQoS0;
    subscriptionList.pTopicFilter = "/example/topic";
    subscriptionList.topicFilterLength = sizeof( "/example/topic" );
    subscriptionCount = sizeof( subscriptionList ) / sizeof( MQTTSubscribeInfo_t );
    status = MQTT_GetUnsubscribePacketSize( &subscriptionList,
                                            subscriptionCount,
                                            &remainingLength,
                                            &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    /* Make sure buffer has enough space */
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, bufferSize );

    /* Make sure subscription count of zero fails. */
    status = MQTT_SerializeUnsubscribe( &subscriptionList,
                                        0,
                                        PACKET_ID,
                                        remainingLength,
                                        &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Test if buffer is too small. */
    fixedBuffer.size = 1;
    status = MQTT_SerializeUnsubscribe( &subscriptionList,
                                        subscriptionCount,
                                        PACKET_ID,
                                        remainingLength,
                                        &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTNoMemory, status );
    fixedBuffer.size = bufferSize;

    /* Make sure success it returned for good case. */
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializeUnsubscribe( &subscriptionList,
                                        subscriptionCount,
                                        PACKET_ID,
                                        remainingLength,
                                        &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    checkBufferOverflow( buffer, sizeof( buffer ) );

    /* MQTT UNSUBSCRIBE packet format:
     * 0xA2 (1 byte)
     * Remaining length (1-4 bytes)
     * Packet ID (2 bytes)
     * Topic filters (series of 2 byte lengths followed by filter) (variable) */
    *pIterator++ = MQTT_PACKET_TYPE_UNSUBSCRIBE;
    pIterator += encodeRemainingLength( pIterator, remainingLength );
    *pIterator++ = UINT16_HIGH_BYTE( PACKET_ID );
    *pIterator++ = UINT16_LOW_BYTE( PACKET_ID );
    pIterator += encodeString( pIterator, subscriptionList.pTopicFilter, subscriptionList.topicFilterLength );
    TEST_ASSERT_EQUAL_MEMORY( expectedPacket, &buffer[ BUFFER_PADDING_LENGTH ], packetSize );
}

/* ========================================================================== */

/**
 * @brief Tests that MQTT_GetPublishPacketSize works as intended.
 */
void test_MQTT_GetPublishPacketSize( void )
{
    MQTTPublishInfo_t publishInfo;
    size_t remainingLength = 0;
    size_t packetSize;
    MQTTStatus_t status = MQTTSuccess;

    /* Verify bad paramameters fail. */
    status = MQTT_GetPublishPacketSize( NULL, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    status = MQTT_GetPublishPacketSize( &publishInfo, NULL, &packetSize );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    status = MQTT_GetPublishPacketSize( &publishInfo, &remainingLength, NULL );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    /* Empty topic must fail. */
    memset( ( void * ) &publishInfo, 0x00, sizeof( publishInfo ) );
    publishInfo.pTopicName = NULL;
    publishInfo.topicNameLength = TEST_TOPIC_NAME_LENGTH;
    status = MQTT_GetPublishPacketSize( &publishInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    publishInfo.pTopicName = TEST_TOPIC_NAME;
    publishInfo.topicNameLength = 0;
    status = MQTT_GetPublishPacketSize( &publishInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    /* Packet too large. */
    memset( ( void * ) &publishInfo, 0x00, sizeof( publishInfo ) );
    publishInfo.pTopicName = "/test/topic";
    publishInfo.topicNameLength = sizeof( "/test/topic" );
    publishInfo.payloadLength = MQTT_MAX_REMAINING_LENGTH;
    status = MQTT_GetPublishPacketSize( &publishInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    publishInfo.payloadLength = MQTT_MAX_REMAINING_LENGTH - publishInfo.topicNameLength - sizeof( uint16_t ) - 1;
    status = MQTT_GetPublishPacketSize( &publishInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    /* Good case succeeds. */
    publishInfo.pTopicName = "/test/topic";
    publishInfo.topicNameLength = sizeof( "/test/topic" );
    publishInfo.pPayload = "";
    publishInfo.payloadLength = 0;
    status = MQTT_GetPublishPacketSize( &publishInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );

    /* Again with QoS 2. */
    publishInfo.qos = MQTTQoS2;
    status = MQTT_GetPublishPacketSize( &publishInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
}

/**
 * @brief Tests that MQTT_SerializePublish works as intended.
 */
void test_MQTT_SerializePublish( void )
{
    MQTTPublishInfo_t publishInfo;
    size_t remainingLength = 98;
    uint16_t packetIdentifier;
    uint8_t * pPacketIdentifierHigh;
    uint8_t buffer[ 200 + 2 * BUFFER_PADDING_LENGTH ];
    size_t bufferSize = sizeof( buffer ) - 2 * BUFFER_PADDING_LENGTH;
    size_t packetSize = bufferSize;
    MQTTStatus_t status = MQTTSuccess;
    MQTTFixedBuffer_t fixedBuffer = { .pBuffer = &buffer[ BUFFER_PADDING_LENGTH ], .size = bufferSize };
    uint8_t expectedPacket[ 200 ];
    uint8_t * pIterator;

    const uint16_t PACKET_ID = 1;
    const char * longTopic = "/test/topic/name/longer/than/one/hundred/twenty/eight/characters" \
                             "/test/topic/name/longer/than/one/hundred/twenty/eight/characters";

    /* Verify bad parameters fail. */
    memset( ( void * ) &publishInfo, 0x00, sizeof( publishInfo ) );
    publishInfo.pTopicName = "/test/topic";
    publishInfo.topicNameLength = sizeof( "/test/topic" );

    status = MQTT_SerializePublish( NULL,
                                    PACKET_ID,
                                    remainingLength,
                                    &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_SerializePublish( &publishInfo,
                                    PACKET_ID,
                                    remainingLength,
                                    NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* 0 packet ID for QoS > 0. */
    publishInfo.qos = MQTTQoS1;
    status = MQTT_SerializePublish( &publishInfo,
                                    0,
                                    remainingLength,
                                    &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Empty topic fails. */
    publishInfo.pTopicName = NULL;
    publishInfo.topicNameLength = TEST_TOPIC_NAME_LENGTH;
    status = MQTT_SerializePublish( &publishInfo,
                                    PACKET_ID,
                                    remainingLength,
                                    &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    publishInfo.pTopicName = TEST_TOPIC_NAME;
    publishInfo.topicNameLength = 0;
    status = MQTT_SerializePublish( &publishInfo,
                                    PACKET_ID,
                                    remainingLength,
                                    &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Remaining length larger than buffer size. */
    publishInfo.pTopicName = TEST_TOPIC_NAME;
    publishInfo.topicNameLength = TEST_TOPIC_NAME_LENGTH;
    fixedBuffer.size = 5;
    status = MQTT_SerializePublish( &publishInfo,
                                    PACKET_ID,
                                    10,
                                    &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTNoMemory, status );

    /* Good case succeeds */
    publishInfo.qos = MQTTQoS0;
    publishInfo.pTopicName = "/test/topic";
    publishInfo.topicNameLength = sizeof( "/test/topic" );
    fixedBuffer.size = bufferSize;
    /* Calculate exact packet size and remaining length. */
    status = MQTT_GetPublishPacketSize( &publishInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    /* Make sure buffer has enough space */
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, bufferSize );

    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializePublish( &publishInfo,
                                    PACKET_ID,
                                    remainingLength,
                                    &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    checkBufferOverflow( buffer, sizeof( buffer ) );

    /* MQTT PUBLISH packet format:
     * 0x30 | publish flags (dup, qos, retain) (1 byte)
     * Remaining length (1-4 bytes)
     * Topic name length (2 bytes)
     * Topic name (variable)
     * Packet ID (if QoS > 0) (1 byte)
     * Payload (>= 0 bytes) */
    expectedPacket[ 0 ] = MQTT_PACKET_TYPE_PUBLISH;
    expectedPacket[ 1 ] = remainingLength;
    ( void ) encodeString( &expectedPacket[ 2 ], publishInfo.pTopicName, publishInfo.topicNameLength );
    TEST_ASSERT_EQUAL_MEMORY( expectedPacket, &buffer[ BUFFER_PADDING_LENGTH ], packetSize );

    /* Again with QoS2, dup, and retain. Also encode remaining length > 2 bytes. */
    publishInfo.qos = MQTTQoS2;
    publishInfo.retain = true;
    publishInfo.dup = true;
    publishInfo.pTopicName = longTopic;
    publishInfo.topicNameLength = strlen( longTopic );
    publishInfo.pPayload = MQTT_SAMPLE_PAYLOAD;
    publishInfo.payloadLength = MQTT_SAMPLE_PAYLOAD_LEN;
    memset( ( void * ) buffer, 0x00, bufferSize );
    /* Calculate exact packet size and remaining length. */
    status = MQTT_GetPublishPacketSize( &publishInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    /* Make sure buffer has enough space */
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, bufferSize );
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializePublish( &publishInfo,
                                    PACKET_ID,
                                    remainingLength,
                                    &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    checkBufferOverflow( buffer, sizeof( buffer ) );
    memset( ( void * ) expectedPacket, 0x00, sizeof( expectedPacket ) );
    pIterator = expectedPacket;
    /* Dup = 0x8, QoS2 = 0x4, Retain = 0x1. 8 + 4 + 1 = 0xD. */
    *pIterator++ = MQTT_PACKET_TYPE_PUBLISH | 0xD;
    pIterator += encodeRemainingLength( pIterator, remainingLength );
    pIterator += encodeString( pIterator, publishInfo.pTopicName, publishInfo.topicNameLength );
    *pIterator++ = UINT16_HIGH_BYTE( PACKET_ID );
    *pIterator++ = UINT16_LOW_BYTE( PACKET_ID );
    ( void ) memcpy( pIterator, publishInfo.pPayload, publishInfo.payloadLength );
    TEST_ASSERT_EQUAL_MEMORY( expectedPacket, &buffer[ BUFFER_PADDING_LENGTH ], packetSize );
}

/* ========================================================================== */

/**
 * @brief Tests that MQTT_GetDisconnectPacketSize works as intended.
 */
void test_MQTT_GetDisconnectPacketSize( void )
{
    MQTTStatus_t status;
    size_t packetSize;

    /* Verify parameters. */
    status = MQTT_GetDisconnectPacketSize( NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Good case succeeds. A DISCONNECT is 2 bytes. */
    status = MQTT_GetDisconnectPacketSize( &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_EQUAL_INT( 2, packetSize );
}

/**
 * @brief Tests that MQTT_SerializeDisconnect works as intended.
 */
void test_MQTT_SerializeDisconnect( void )
{
    uint8_t buffer[ 10 + 2 * BUFFER_PADDING_LENGTH ];
    MQTTFixedBuffer_t fixedBuffer = { .pBuffer = &buffer[ BUFFER_PADDING_LENGTH ] };
    uint8_t expectedPacket[ 2 ] = { MQTT_PACKET_TYPE_DISCONNECT, 0 };
    MQTTStatus_t status = MQTTSuccess;

    /* Buffer size less than disconnect request fails. */
    fixedBuffer.size = 1;
    status = MQTT_SerializeDisconnect( &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTNoMemory, status );

    /* NULL buffer fails. */
    status = MQTT_SerializeDisconnect( NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Good case succeeds. */
    fixedBuffer.size = 2;
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializeDisconnect( &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    checkBufferOverflow( buffer, sizeof( buffer ) );
    TEST_ASSERT_EQUAL_MEMORY( expectedPacket, &buffer[ BUFFER_PADDING_LENGTH ], 2 );
}

/**
 * @brief Tests that MQTT_GetPingreqPacketSize works as intended.
 */
void test_MQTT_GetPingreqPacketSize( void )
{
    MQTTStatus_t status;
    size_t packetSize;

    /* Verify parameters. */
    status = MQTT_GetPingreqPacketSize( NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Good case succeeds. A PINGREQ is 2 bytes. */
    status = MQTT_GetPingreqPacketSize( &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_EQUAL_INT( 2, packetSize );
}

/**
 * @brief Tests that MQTT_SerializePingreq works as intended.
 */
void test_MQTT_SerializePingreq( void )
{
    uint8_t buffer[ 10 + 2 * BUFFER_PADDING_LENGTH ];
    MQTTFixedBuffer_t fixedBuffer = { .pBuffer = &buffer[ BUFFER_PADDING_LENGTH ] };
    uint8_t expectedPacket[ 2 ] = { MQTT_PACKET_TYPE_PINGREQ, 0 };
    MQTTStatus_t status = MQTTSuccess;

    /* Buffer size less than ping request fails. */
    fixedBuffer.size = 1;
    status = MQTT_SerializePingreq( &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTNoMemory, status );

    /* NULL buffer fails. */
    status = MQTT_SerializePingreq( NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Good case succeeds. */
    fixedBuffer.size = 2;
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializePingreq( &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    checkBufferOverflow( buffer, sizeof( buffer ) );
    TEST_ASSERT_EQUAL_MEMORY( expectedPacket, &buffer[ BUFFER_PADDING_LENGTH ], 2 );
}

/* ========================================================================== */

/**
 * @brief Tests that MQTT_DeserializeAck works as intended with a CONNACK.
 */
void test_MQTT_DeserializeAck_connack( void )
{
    MQTTPacketInfo_t mqttPacketInfo;
    uint16_t packetIdentifier;
    bool sessionPresent;
    MQTTStatus_t status = MQTTSuccess;
    uint8_t buffer[ 10 ];

    /* Verify parameters */
    memset( ( void * ) &mqttPacketInfo, 0x00, sizeof( mqttPacketInfo ) );
    mqttPacketInfo.type = MQTT_PACKET_TYPE_CONNACK;
    status = MQTT_DeserializeAck( NULL, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );
    /* Packet ID can be NULL for CONNACK, don't need to check that. */
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    memset( ( void * ) &mqttPacketInfo, 0x00, sizeof( mqttPacketInfo ) );
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    /* Bad packet type. */
    mqttPacketInfo.type = 0x01;
    mqttPacketInfo.pRemainingData = buffer;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* Bad remaining length. */
    mqttPacketInfo.type = MQTT_PACKET_TYPE_CONNACK;
    mqttPacketInfo.remainingLength = MQTT_PACKET_CONNACK_REMAINING_LENGTH - 1;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* Incorrect reserved bits. */
    mqttPacketInfo.remainingLength = MQTT_PACKET_CONNACK_REMAINING_LENGTH;
    buffer[ 0 ] = 0xf;
    buffer[ 1 ] = 0;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* Session present but nonzero return code. */
    buffer[ 0 ] = MQTT_PACKET_CONNACK_SESSION_PRESENT_MASK;
    buffer[ 1 ] = 1;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* Invalid response code. */
    buffer[ 0 ] = 0;
    buffer[ 1 ] = 6;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* Valid packet with rejected code. */
    buffer[ 1 ] = 1;
    status = MQTT_DeserializeAck( &mqttPacketInfo, NULL, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTServerRefused, status );

    /* Valid packet with success code. */
    buffer[ 0 ] = 1;
    buffer[ 1 ] = 0;
    status = MQTT_DeserializeAck( &mqttPacketInfo, NULL, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
}

/**
 * @brief Tests that MQTT_DeserializeAck works as intended with a SUBACK.
 */
void test_MQTT_DeserializeAck_suback( void )
{
    MQTTPacketInfo_t mqttPacketInfo;
    uint16_t packetIdentifier;
    bool sessionPresent;
    MQTTStatus_t status = MQTTSuccess;
    uint8_t buffer[ 10 ] = { 0 };

    /* Bad remaining length. */
    mqttPacketInfo.type = MQTT_PACKET_TYPE_SUBACK;
    mqttPacketInfo.pRemainingData = buffer;
    mqttPacketInfo.remainingLength = 2;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* Set packet identifier. */
    buffer[ 0 ] = 0;
    buffer[ 1 ] = 1;

    /* Bad response code. */
    mqttPacketInfo.remainingLength = 3;
    buffer[ 2 ] = 5;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* Process a valid SUBACK with server refused response code. */
    mqttPacketInfo.remainingLength = 3;
    buffer[ 2 ] = 0x80;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTServerRefused, status );

    /* Process a valid SUBACK with various server acceptance codes. */
    mqttPacketInfo.remainingLength = 5;
    buffer[ 2 ] = 0x00;
    buffer[ 3 ] = 0x01;
    buffer[ 4 ] = 0x02;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
}

/**
 * @brief Tests that MQTT_DeserializeAck works as intended with an UNSUBACK.
 */
void test_MQTT_DeserializeAck_unsuback( void )
{
    MQTTPacketInfo_t mqttPacketInfo;
    uint16_t packetIdentifier;
    bool sessionPresent;
    MQTTStatus_t status = MQTTSuccess;
    uint8_t buffer[ 10 ] = { 0 };

    /* Bad remaining length. */
    mqttPacketInfo.type = MQTT_PACKET_TYPE_UNSUBACK;
    mqttPacketInfo.pRemainingData = buffer;
    mqttPacketInfo.remainingLength = MQTT_PACKET_UNSUBACK_REMAINING_LENGTH - 1;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* Packet identifier 0 is not valid (per spec). */
    buffer[ 0 ] = 0;
    buffer[ 1 ] = 0;
    mqttPacketInfo.remainingLength = MQTT_PACKET_UNSUBACK_REMAINING_LENGTH;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* Process a valid UNSUBACK. */
    buffer[ 1 ] = 1;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
}

/**
 * @brief Tests that MQTT_DeserializeAck works as intended with a PINGRESP.
 */
void test_MQTT_DeserializeAck_pingresp( void )
{
    MQTTPacketInfo_t mqttPacketInfo;
    uint16_t packetIdentifier;
    bool sessionPresent;
    MQTTStatus_t status = MQTTSuccess;
    uint8_t buffer[ 10 ] = { 0 };

    /* Bad remaining length. */
    mqttPacketInfo.type = MQTT_PACKET_TYPE_PINGRESP;
    mqttPacketInfo.pRemainingData = buffer;
    mqttPacketInfo.remainingLength = MQTT_PACKET_PINGRESP_REMAINING_LENGTH + 1;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* Process a valid PINGRESP. */
    mqttPacketInfo.remainingLength = MQTT_PACKET_PINGRESP_REMAINING_LENGTH;
    status = MQTT_DeserializeAck( &mqttPacketInfo, NULL, NULL );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
}

/**
 * @brief Tests that MQTT_DeserializeAck works as intended with a PUBACK,
 * PUBREC, PUBREL, and PUBCOMP.
 */
void test_MQTT_DeserializeAck_puback( void )
{
    MQTTPacketInfo_t mqttPacketInfo;
    uint16_t packetIdentifier;
    bool sessionPresent;
    MQTTStatus_t status = MQTTSuccess;
    uint8_t buffer[ 10 ] = { 0 };

    /* Verify parameters */
    memset( ( void * ) &mqttPacketInfo, 0x00, sizeof( mqttPacketInfo ) );
    mqttPacketInfo.type = MQTT_PACKET_TYPE_PUBACK;
    status = MQTT_DeserializeAck( NULL, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );
    status = MQTT_DeserializeAck( &mqttPacketInfo, NULL, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );
    /* mqttPacketInfo.pRemainingData not set. */
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Bad remaining length. */
    mqttPacketInfo.type = MQTT_PACKET_TYPE_PUBACK;
    mqttPacketInfo.pRemainingData = buffer;
    mqttPacketInfo.remainingLength = MQTT_PACKET_SIMPLE_ACK_REMAINING_LENGTH - 1;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* Packet identifier 0 is not valid (per spec). */
    buffer[ 0 ] = 0;
    buffer[ 1 ] = 0;
    mqttPacketInfo.remainingLength = MQTT_PACKET_SIMPLE_ACK_REMAINING_LENGTH;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* Process a valid PUBACK. */
    buffer[ 1 ] = 1;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_EQUAL_INT( 1, packetIdentifier );

    /* PUBREC. */
    mqttPacketInfo.type = MQTT_PACKET_TYPE_PUBREC;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_EQUAL_INT( 1, packetIdentifier );

    /* PUBREL. */
    mqttPacketInfo.type = MQTT_PACKET_TYPE_PUBREL;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_EQUAL_INT( 1, packetIdentifier );

    /* PUBCOMP. */
    mqttPacketInfo.type = MQTT_PACKET_TYPE_PUBCOMP;
    status = MQTT_DeserializeAck( &mqttPacketInfo, &packetIdentifier, &sessionPresent );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_EQUAL_INT( 1, packetIdentifier );
}

/* ========================================================================== */

/**
 * @brief Tests that MQTT_DeserializePublish works as intended.
 */
void test_MQTT_DeserializePublish( void )
{
    MQTTPacketInfo_t mqttPacketInfo;
    MQTTPublishInfo_t publishInfo;
    MQTTStatus_t status = MQTTSuccess;
    uint8_t buffer[ 100 ];
    size_t bufferSize = sizeof( buffer );
    MQTTFixedBuffer_t fixedBuffer = { 0 };
    size_t packetSize = bufferSize;

    size_t remainingLength = 0;
    uint16_t packetIdentifier;
    uint8_t * pPacketIdentifierHigh;

    fixedBuffer.pBuffer = buffer;
    fixedBuffer.size = bufferSize;

    const uint16_t PACKET_ID = 1;

    /* Verify parameters. */
    status = MQTT_DeserializePublish( NULL, &packetIdentifier, &publishInfo );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );
    status = MQTT_DeserializePublish( &mqttPacketInfo, NULL, &publishInfo );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );
    status = MQTT_DeserializePublish( &mqttPacketInfo, &packetIdentifier, NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    memset( ( void * ) &mqttPacketInfo, 0x00, sizeof( mqttPacketInfo ) );

    /* Bad Packet Type. */
    mqttPacketInfo.type = 0x01;
    mqttPacketInfo.pRemainingData = buffer;
    status = MQTT_DeserializePublish( &mqttPacketInfo, &packetIdentifier, &publishInfo );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Incorrect flags. */
    mqttPacketInfo.type = MQTT_PACKET_TYPE_PUBLISH | 0xf;
    status = MQTT_DeserializePublish( &mqttPacketInfo, &packetIdentifier, &publishInfo );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* QoS 0 bad remaining length. */
    mqttPacketInfo.type = MQTT_PACKET_TYPE_PUBLISH;
    mqttPacketInfo.remainingLength = 0;
    status = MQTT_DeserializePublish( &mqttPacketInfo, &packetIdentifier, &publishInfo );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* QoS 1 bad remaining length. */
    mqttPacketInfo.type = MQTT_PACKET_TYPE_PUBLISH | 0x2;
    mqttPacketInfo.remainingLength = 0;
    status = MQTT_DeserializePublish( &mqttPacketInfo, &packetIdentifier, &publishInfo );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* QoS 1 invalid packet identifier. */
    mqttPacketInfo.remainingLength = 5;
    buffer[ 0 ] = 0;
    buffer[ 1 ] = 1;
    buffer[ 2 ] = ( uint8_t ) 'a';
    buffer[ 3 ] = 0;
    buffer[ 4 ] = 0;
    status = MQTT_DeserializePublish( &mqttPacketInfo, &packetIdentifier, &publishInfo );
    TEST_ASSERT_EQUAL_INT( MQTTBadResponse, status );

    /* Create a PUBLISH packet to test. */
    memset( &publishInfo, 0x00, sizeof( publishInfo ) );
    publishInfo.pTopicName = "/test/topic";
    publishInfo.topicNameLength = ( uint16_t ) strlen( publishInfo.pTopicName );
    publishInfo.pPayload = "Hello World";
    publishInfo.payloadLength = ( uint16_t ) strlen( publishInfo.pPayload );

    /* Test serialization and deserialization of a QoS 0 PUBLISH. */
    publishInfo.qos = MQTTQoS0;

    /* Generate QoS 0 packet. */
    status = MQTT_GetPublishPacketSize( &publishInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, bufferSize );

    status = MQTT_SerializePublish( &publishInfo,
                                    0,
                                    remainingLength,
                                    &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );

    /* Deserialize QoS 0 packet. */
    mqttPacketInfo.type = buffer[ 0 ];

    /* We don't need to go through the trouble of calling MQTT_GetIncomingPacketTypeAndLength.
     * We know the remaining length is < 128. */
    mqttPacketInfo.remainingLength = ( size_t ) buffer[ 1 ];
    mqttPacketInfo.pRemainingData = &buffer[ 2 ];
    status = MQTT_DeserializePublish( &mqttPacketInfo, &packetIdentifier, &publishInfo );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );

    memset( ( void * ) &mqttPacketInfo, 0x00, sizeof( mqttPacketInfo ) );

    /* Test serialization and deserialization of a QoS 1 PUBLISH. */
    publishInfo.qos = MQTTQoS1;

    status = MQTT_GetPublishPacketSize( &publishInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, bufferSize );

    status = MQTT_SerializePublish( &publishInfo,
                                    PACKET_ID,
                                    remainingLength,
                                    &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );

    mqttPacketInfo.type = buffer[ 0 ];
    mqttPacketInfo.remainingLength = ( size_t ) buffer[ 1 ];
    mqttPacketInfo.pRemainingData = &buffer[ 2 ];
    status = MQTT_DeserializePublish( &mqttPacketInfo, &packetIdentifier, &publishInfo );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );

    /* QoS 2 PUBLISH. */
    publishInfo.qos = MQTTQoS2;
    /* Remaining length and packet size should be same as before. */
    status = MQTT_SerializePublish( &publishInfo,
                                    PACKET_ID,
                                    remainingLength,
                                    &fixedBuffer );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    mqttPacketInfo.type = buffer[ 0 ];
    mqttPacketInfo.remainingLength = ( size_t ) buffer[ 1 ];
    mqttPacketInfo.pRemainingData = &buffer[ 2 ];
    status = MQTT_DeserializePublish( &mqttPacketInfo, &packetIdentifier, &publishInfo );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
}

/* ========================================================================== */

/**
 * @brief Tests that MQTT_GetIncomingPacketTypeAndLength works as intended.
 */
void test_MQTT_GetIncomingPacketTypeAndLength( void )
{
    MQTTStatus_t status = MQTTSuccess;
    MQTTPacketInfo_t mqttPacket;
    uint8_t buffer[ 10 ];
    uint8_t * bufPtr = buffer;

    /* Dummy network context - pointer to pointer to a buffer. */
    NetworkContext_t networkContext = ( NetworkContext_t ) &bufPtr;

    buffer[ 0 ] = 0x20; /* CONN ACK */
    buffer[ 1 ] = 0x02; /* Remaining length. */

    status = MQTT_GetIncomingPacketTypeAndLength( mockReceive, networkContext, &mqttPacket );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_EQUAL_INT( 0x20, mqttPacket.type );
    TEST_ASSERT_EQUAL_INT( 0x02, mqttPacket.remainingLength );

    /* Remaining length of 128 needs 2 bytes. */
    bufPtr = buffer;
    buffer[ 0 ] = MQTT_PACKET_TYPE_PUBLISH;
    buffer[ 1 ] = 0x80;
    buffer[ 2 ] = 0x01;
    status = MQTT_GetIncomingPacketTypeAndLength( mockReceive, networkContext, &mqttPacket );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_EQUAL_INT( MQTT_PACKET_TYPE_PUBLISH, mqttPacket.type );
    TEST_ASSERT_EQUAL_INT( 128, mqttPacket.remainingLength );

    /* Test with incorrect packet type. */
    bufPtr = buffer;
    buffer[ 0 ] = 0x10; /* INVALID */
    status = MQTT_GetIncomingPacketTypeAndLength( mockReceive, networkContext, &mqttPacket );
    TEST_ASSERT_EQUAL( MQTTBadResponse, status );

    /* Test with invalid remaining length. */
    bufPtr = buffer;
    buffer[ 0 ] = 0x20; /* CONN ACK */

    /* To generate invalid remaining length response,
     * four bytes need to have MSB (or continuation bit, 0x80) set */
    buffer[ 1 ] = 0xFF;
    buffer[ 2 ] = 0xFF;
    buffer[ 3 ] = 0xFF;
    buffer[ 4 ] = 0xFF;
    status = MQTT_GetIncomingPacketTypeAndLength( mockReceive, networkContext, &mqttPacket );
    TEST_ASSERT_EQUAL( MQTTBadResponse, status );

    /* Check with an encoding that does not conform to the MQTT spec. */
    bufPtr = buffer;
    buffer[ 1 ] = 0x80;
    buffer[ 2 ] = 0x80;
    buffer[ 3 ] = 0x80;
    buffer[ 4 ] = 0x00;
    status = MQTT_GetIncomingPacketTypeAndLength( mockReceive, networkContext, &mqttPacket );
    TEST_ASSERT_EQUAL( MQTTBadResponse, status );

    /* Check when network receive fails. */
    memset( buffer, 0x00, 10 );
    bufPtr = buffer;
    status = MQTT_GetIncomingPacketTypeAndLength( mockReceiveFailure, networkContext, &mqttPacket );
    TEST_ASSERT_EQUAL( MQTTRecvFailed, status );

    /* Test if no data is available. */
    bufPtr = buffer;
    status = MQTT_GetIncomingPacketTypeAndLength( mockReceiveNoData, networkContext, &mqttPacket );
    TEST_ASSERT_EQUAL( MQTTNoDataAvailable, status );

    /* Branch coverage for PUBREL. */
    bufPtr = buffer;
    buffer[ 0 ] = MQTT_PACKET_TYPE_PUBREL & 0xF0U;
    status = MQTT_GetIncomingPacketTypeAndLength( mockReceive, networkContext, &mqttPacket );
    TEST_ASSERT_EQUAL( MQTTBadResponse, status );

    /* Receive type then fail. */
    bufPtr = buffer;
    buffer[ 0 ] = MQTT_PACKET_TYPE_PUBREL;
    status = MQTT_GetIncomingPacketTypeAndLength( mockReceiveSucceedThenFail, networkContext, &mqttPacket );
    TEST_ASSERT_EQUAL( MQTTBadResponse, status );
}

/* ========================================================================== */

/**
 * @brief Tests that MQTT_SerializePublishHeader works as intended.
 */
void test_MQTT_SerializePublishHeader( void )
{
    MQTTPublishInfo_t publishInfo;
    size_t remainingLength = 0;
    uint16_t packetIdentifier;
    uint8_t * pPacketIdentifierHigh;
    uint8_t buffer[ 200 + 2 * BUFFER_PADDING_LENGTH ];
    uint8_t expectedPacket[ 200 ];
    uint8_t * pIterator;
    size_t bufferSize = sizeof( buffer ) - 2 * BUFFER_PADDING_LENGTH;
    size_t packetSize = bufferSize;
    MQTTStatus_t status = MQTTSuccess;
    MQTTFixedBuffer_t fixedBuffer = { .pBuffer = &buffer[ BUFFER_PADDING_LENGTH ], .size = bufferSize };
    size_t headerSize = 0;

    const uint16_t PACKET_ID = 1;

    /* Verify bad parameters fail. */
    memset( ( void * ) &publishInfo, 0x00, sizeof( publishInfo ) );
    publishInfo.pTopicName = TEST_TOPIC_NAME;
    publishInfo.topicNameLength = TEST_TOPIC_NAME_LENGTH;
    status = MQTT_SerializePublishHeader( NULL,
                                          PACKET_ID,
                                          remainingLength,
                                          &fixedBuffer,
                                          &headerSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_SerializePublishHeader( &publishInfo,
                                          PACKET_ID,
                                          remainingLength,
                                          NULL,
                                          &headerSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_SerializePublishHeader( &publishInfo,
                                          PACKET_ID,
                                          remainingLength,
                                          &fixedBuffer,
                                          NULL );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Empty topic fails. */
    publishInfo.pTopicName = NULL;
    publishInfo.topicNameLength = TEST_TOPIC_NAME_LENGTH;
    status = MQTT_SerializePublishHeader( &publishInfo,
                                          PACKET_ID,
                                          remainingLength,
                                          &fixedBuffer,
                                          &headerSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    publishInfo.pTopicName = TEST_TOPIC_NAME;
    publishInfo.topicNameLength = 0;
    status = MQTT_SerializePublishHeader( &publishInfo,
                                          PACKET_ID,
                                          remainingLength,
                                          &fixedBuffer,
                                          &headerSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );
    publishInfo.topicNameLength = TEST_TOPIC_NAME_LENGTH;

    /* 0 packet ID for QoS > 0. */
    publishInfo.qos = MQTTQoS1;
    status = MQTT_SerializePublishHeader( &publishInfo,
                                          0,
                                          remainingLength,
                                          &fixedBuffer,
                                          &headerSize );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Buffer too small. */
    fixedBuffer.size = 1;
    status = MQTT_SerializePublishHeader( &publishInfo,
                                          PACKET_ID,
                                          remainingLength,
                                          &fixedBuffer,
                                          &headerSize );
    TEST_ASSERT_EQUAL_INT( MQTTNoMemory, status );
    fixedBuffer.size = bufferSize;

    /* Success case. */
    status = MQTT_GetPublishPacketSize( &publishInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializePublishHeader( &publishInfo,
                                          PACKET_ID,
                                          remainingLength,
                                          &fixedBuffer,
                                          &headerSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );

    /* MQTT PUBLISH packet format:
     * 0x30 | publish flags (dup, qos, retain) (1 byte)
     * Remaining length (1-4 bytes)
     * Topic name length (2 bytes)
     * Topic name (variable)
     * Packet ID (if QoS > 0) (1 byte)
     * Payload (>= 0 bytes) */
    memset( ( void * ) expectedPacket, 0x00, sizeof( expectedPacket ) );
    pIterator = expectedPacket;
    *pIterator++ = MQTT_PACKET_TYPE_PUBLISH | ( publishInfo.qos << 1 );
    pIterator += encodeRemainingLength( pIterator, remainingLength );
    pIterator += encodeString( pIterator, publishInfo.pTopicName, publishInfo.topicNameLength );
    *pIterator++ = UINT16_HIGH_BYTE( PACKET_ID );
    *pIterator++ = UINT16_LOW_BYTE( PACKET_ID );
    TEST_ASSERT_EQUAL_MEMORY( expectedPacket, &buffer[ BUFFER_PADDING_LENGTH ], packetSize );
    checkBufferOverflow( buffer, sizeof( buffer ) );

    publishInfo.qos = MQTTQoS0;
    publishInfo.pPayload = "test";
    publishInfo.payloadLength = 4;
    status = MQTT_GetPublishPacketSize( &publishInfo, &remainingLength, &packetSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializePublishHeader( &publishInfo,
                                          0,
                                          remainingLength,
                                          &fixedBuffer,
                                          &headerSize );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    memset( ( void * ) expectedPacket, 0x00, sizeof( expectedPacket ) );
    pIterator = expectedPacket;
    *pIterator++ = MQTT_PACKET_TYPE_PUBLISH;
    pIterator += encodeRemainingLength( pIterator, remainingLength );
    pIterator += encodeString( pIterator, publishInfo.pTopicName, publishInfo.topicNameLength );
    /* Payload should not be serialized. */
    TEST_ASSERT_EQUAL_MEMORY( expectedPacket, &buffer[ BUFFER_PADDING_LENGTH ], packetSize );
    checkBufferOverflow( buffer, sizeof( buffer ) );
}

/* ========================================================================== */

/**
 * @brief Tests that MQTT_SerializeAck works as intended.
 */
void test_MQTT_SerializeAck( void )
{
    uint8_t buffer[ 10 + 2 * BUFFER_PADDING_LENGTH ];
    uint8_t expectedPacket[ MQTT_PUBLISH_ACK_PACKET_SIZE ];
    size_t bufferSize = sizeof( buffer ) - 2 * BUFFER_PADDING_LENGTH;
    MQTTStatus_t status = MQTTSuccess;
    MQTTFixedBuffer_t fixedBuffer = { .pBuffer = &buffer[ BUFFER_PADDING_LENGTH ], .size = bufferSize };
    uint8_t packetType = MQTT_PACKET_TYPE_PUBACK;

    const uint16_t PACKET_ID = 1;

    expectedPacket[ 0 ] = packetType;
    expectedPacket[ 1 ] = 2U;
    expectedPacket[ 2 ] = UINT16_HIGH_BYTE( PACKET_ID );
    expectedPacket[ 3 ] = UINT16_LOW_BYTE( PACKET_ID );

    /* Verify parameters. */
    status = MQTT_SerializeAck( NULL, packetType, PACKET_ID );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    status = MQTT_SerializeAck( &fixedBuffer, packetType, 0 );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* Not a PUBACK, PUBREC, PUBREL, or PUBCOMP. */
    status = MQTT_SerializeAck( &fixedBuffer, MQTT_PACKET_TYPE_CONNACK, PACKET_ID );
    TEST_ASSERT_EQUAL_INT( MQTTBadParameter, status );

    /* An ack is 4 bytes. */
    fixedBuffer.size = 3;
    status = MQTT_SerializeAck( &fixedBuffer, packetType, PACKET_ID );
    TEST_ASSERT_EQUAL_INT( MQTTNoMemory, status );
    fixedBuffer.size = bufferSize;

    /* Good case succeeds. */
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializeAck( &fixedBuffer, packetType, PACKET_ID );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_EQUAL_MEMORY( expectedPacket, &buffer[ BUFFER_PADDING_LENGTH ], MQTT_PUBLISH_ACK_PACKET_SIZE );
    checkBufferOverflow( buffer, sizeof( buffer ) );

    /* QoS 2 acks. */
    packetType = MQTT_PACKET_TYPE_PUBREC;
    expectedPacket[ 0 ] = packetType;
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializeAck( &fixedBuffer, packetType, PACKET_ID );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_EQUAL_MEMORY( expectedPacket, &buffer[ BUFFER_PADDING_LENGTH ], MQTT_PUBLISH_ACK_PACKET_SIZE );
    checkBufferOverflow( buffer, sizeof( buffer ) );

    packetType = MQTT_PACKET_TYPE_PUBREL;
    expectedPacket[ 0 ] = packetType;
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializeAck( &fixedBuffer, packetType, PACKET_ID );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_EQUAL_MEMORY( expectedPacket, &buffer[ BUFFER_PADDING_LENGTH ], MQTT_PUBLISH_ACK_PACKET_SIZE );
    checkBufferOverflow( buffer, sizeof( buffer ) );

    packetType = MQTT_PACKET_TYPE_PUBCOMP;
    expectedPacket[ 0 ] = packetType;
    padAndResetBuffer( buffer, sizeof( buffer ) );
    status = MQTT_SerializeAck( &fixedBuffer, packetType, PACKET_ID );
    TEST_ASSERT_EQUAL_INT( MQTTSuccess, status );
    TEST_ASSERT_EQUAL_MEMORY( expectedPacket, &buffer[ BUFFER_PADDING_LENGTH ], MQTT_PUBLISH_ACK_PACKET_SIZE );
    checkBufferOverflow( buffer, sizeof( buffer ) );
}

/* =====================  Testing MQTT_SerializeConnect ===================== */

/**
 * @brief Check the serialization of an MQTT CONNECT packet in the given buffer,
 * following the same order in serializeConnectPacket.
 *
 * @param[in] pConnectInfo MQTT CONNECT packet parameters.
 * @param[in] pWillInfo Last Will and Testament. Pass NULL if not used.
 * @param[in] remainingLength Remaining Length of MQTT CONNECT packet.
 * @param[in] pBuffer Buffer to check packet serialization.
 *
 */
static void verifySerializedConnectPacket( const MQTTConnectInfo_t * const pConnectInfo,
                                           const MQTTPublishInfo_t * const pWillInfo,
                                           size_t remainingLength,
                                           const MQTTFixedBuffer_t * const pBuffer )
{
    uint8_t connectFlags = 0U;
    uint8_t encodedRemainingLength = 0U;
    uint8_t encodedStringLength = 0U;
    uint8_t * pIndex = NULL;

    pIndex = pBuffer->pBuffer;
    /* The first byte in the CONNECT packet is the control packet type. */
    TEST_ASSERT_EQUAL( MQTT_PACKET_TYPE_CONNECT, *pIndex );
    pIndex++;

    /* The remaining length of the CONNECT packet is encoded starting from the
     * second byte. The remaining length does not include the length of the fixed
     * header or the encoding of the remaining length. */
    encodedRemainingLength = encodeRemainingLength( remainingLengthBuffer, remainingLength );
    TEST_ASSERT_EQUAL_MEMORY( remainingLengthBuffer, pIndex, encodedRemainingLength );
    pIndex += encodedRemainingLength;

    /* The string "MQTT" is placed at the beginning of the CONNECT packet's variable
     * header. This string is 4 bytes long. */
    encodedStringLength = encodeString( encodedStringBuffer, "MQTT", 4 );
    TEST_ASSERT_EQUAL_MEMORY( encodedStringBuffer, pIndex, encodedStringLength );
    pIndex += encodedStringLength;

    /* The MQTT protocol version is the second field of the variable header. */
    TEST_ASSERT_EQUAL( MQTT_VERSION_3_1_1, *pIndex );
    pIndex++;

    /* Set the clean session flag if needed. */
    if( pConnectInfo->cleanSession == true )
    {
        UINT8_SET_BIT( connectFlags, MQTT_CONNECT_FLAG_CLEAN );
    }

    /* Set the flags for username and password if provided. */
    if( pConnectInfo->pUserName != NULL )
    {
        UINT8_SET_BIT( connectFlags, MQTT_CONNECT_FLAG_USERNAME );
    }

    if( pConnectInfo->pPassword != NULL )
    {
        UINT8_SET_BIT( connectFlags, MQTT_CONNECT_FLAG_PASSWORD );
    }

    /* Set will flag if a Last Will and Testament is provided. */
    if( pWillInfo != NULL )
    {
        UINT8_SET_BIT( connectFlags, MQTT_CONNECT_FLAG_WILL );

        /* Flags only need to be changed for Will QoS 1 or 2. */
        if( pWillInfo->qos == MQTTQoS1 )
        {
            UINT8_SET_BIT( connectFlags, MQTT_CONNECT_FLAG_WILL_QOS1 );
        }
        else if( pWillInfo->qos == MQTTQoS2 )
        {
            UINT8_SET_BIT( connectFlags, MQTT_CONNECT_FLAG_WILL_QOS2 );
        }
        else
        {
            /* Empty else MISRA 15.7 */
        }

        if( pWillInfo->retain == true )
        {
            UINT8_SET_BIT( connectFlags, MQTT_CONNECT_FLAG_WILL_RETAIN );
        }
    }

    TEST_ASSERT_EQUAL( connectFlags, *pIndex );
    pIndex++;

    /* Verify the 2 bytes of the keep alive interval into the CONNECT packet. */
    TEST_ASSERT_EQUAL( UINT16_HIGH_BYTE( pConnectInfo->keepAliveSeconds ),
                       *pIndex );
    pIndex++;
    TEST_ASSERT_EQUAL( UINT16_LOW_BYTE( pConnectInfo->keepAliveSeconds ),
                       *pIndex );
    pIndex++;

    /* Verify the client identifier into the CONNECT packet. */
    encodedStringLength = encodeString( encodedStringBuffer,
                                        pConnectInfo->pClientIdentifier,
                                        pConnectInfo->clientIdentifierLength );
    TEST_ASSERT_EQUAL_MEMORY( encodedStringBuffer, pIndex, encodedStringLength );
    pIndex += encodedStringLength;

    /* Verify the will topic name and message into the CONNECT packet if provided. */
    if( pWillInfo != NULL )
    {
        encodedStringLength = encodeString( encodedStringBuffer,
                                            pWillInfo->pTopicName,
                                            pWillInfo->topicNameLength );
        TEST_ASSERT_EQUAL_MEMORY( encodedStringBuffer, pIndex, encodedStringLength );
        pIndex += encodedStringLength;
        encodedStringLength = encodeString( encodedStringBuffer,
                                            pWillInfo->pPayload,
                                            ( uint16_t ) pWillInfo->payloadLength );
        TEST_ASSERT_EQUAL_MEMORY( encodedStringBuffer, pIndex, encodedStringLength );
        pIndex += encodedStringLength;
    }

    /* Verify the user name if provided. */
    if( pConnectInfo->pUserName != NULL )
    {
        encodedStringLength = encodeString( encodedStringBuffer,
                                            pConnectInfo->pUserName,
                                            pConnectInfo->userNameLength );
        TEST_ASSERT_EQUAL_MEMORY( encodedStringBuffer, pIndex, encodedStringLength );
        pIndex += encodedStringLength;
    }

    /* Verify the password if provided. */
    if( pConnectInfo->pPassword != NULL )
    {
        encodedStringLength = encodeString( encodedStringBuffer,
                                            pConnectInfo->pPassword,
                                            pConnectInfo->passwordLength );
        TEST_ASSERT_EQUAL_MEMORY( encodedStringBuffer, pIndex, encodedStringLength );
        pIndex += encodedStringLength;
    }
}

/**
 * @brief Call Mqtt_SerializeConnect using NULL parameters and insufficient buffer
 * size until we receive all possible MQTTBadParameter and MQTTNoMemory errors.
 */
void test_MQTT_SerializeConnect_Invalid_Params()
{
    MQTTStatus_t mqttStatus = MQTTSuccess;
    size_t remainingLength = 0UL, packetSize = 0UL;
    MQTTFixedBuffer_t networkBuffer;
    MQTTConnectInfo_t connectInfo;

    /* Test NULL pConnectInfo. */
    mqttStatus = MQTT_SerializeConnect( NULL, NULL,
                                        remainingLength, &networkBuffer );
    TEST_ASSERT_EQUAL( MQTTBadParameter, mqttStatus );

    /* Test NULL pBuffer. */
    mqttStatus = MQTT_SerializeConnect( &connectInfo, NULL,
                                        remainingLength, NULL );
    TEST_ASSERT_EQUAL( MQTTBadParameter, mqttStatus );

    /* Test connectPacketSize > pBuffer->size. */
    /* Get MQTT connect packet size and remaining length. */
    setupConnectInfo( &connectInfo );
    mqttStatus = MQTT_GetConnectPacketSize( &connectInfo,
                                            NULL,
                                            &remainingLength,
                                            &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, mqttStatus );
    networkBuffer.pBuffer = mqttBuffer;
    networkBuffer.size = packetSize - 1;
    mqttStatus = MQTT_SerializeConnect( &connectInfo, NULL,
                                        remainingLength, &networkBuffer );
    TEST_ASSERT_EQUAL( MQTTNoMemory, mqttStatus );
}

/**
 * @brief This method calls MQTT_SerializeConnect successfully using different parameters
 * until we have full coverage on the private method, serializeConnectPacket(...).
 */
void test_MQTT_SerializeConnect_Happy_Paths()
{
    MQTTStatus_t mqttStatus = MQTTSuccess;
    size_t remainingLength = 0, packetSize = 0;
    MQTTFixedBuffer_t networkBuffer;
    MQTTConnectInfo_t connectInfo;
    MQTTPublishInfo_t willInfo;

    /* Fill structs to pass into methods to be tested. */
    setupNetworkBuffer( &networkBuffer );
    setupConnectInfo( &connectInfo );
    setupWillInfo( &willInfo );

    /* Get MQTT connect packet size and remaining length. */
    mqttStatus = MQTT_GetConnectPacketSize( &connectInfo,
                                            &willInfo,
                                            &remainingLength,
                                            &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, mqttStatus );
    /* Make sure buffer has enough space. */
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, networkBuffer.size );
    mqttStatus = MQTT_SerializeConnect( &connectInfo, &willInfo,
                                        remainingLength, &networkBuffer );
    TEST_ASSERT_EQUAL( MQTTSuccess, mqttStatus );
    verifySerializedConnectPacket( &connectInfo, &willInfo,
                                   remainingLength, &networkBuffer );


    /* Repeat with MQTTQoS1. */
    willInfo.qos = MQTTQoS1;
    mqttStatus = MQTT_GetConnectPacketSize( &connectInfo,
                                            &willInfo,
                                            &remainingLength,
                                            &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, mqttStatus );
    /* Make sure buffer has enough space. */
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, networkBuffer.size );
    mqttStatus = MQTT_SerializeConnect( &connectInfo, &willInfo,
                                        remainingLength, &networkBuffer );
    TEST_ASSERT_EQUAL( MQTTSuccess, mqttStatus );
    verifySerializedConnectPacket( &connectInfo, &willInfo,
                                   remainingLength, &networkBuffer );


    /* Re-initialize objects for branch coverage. */
    willInfo.pPayload = MQTT_SAMPLE_PAYLOAD;
    willInfo.payloadLength = MQTT_SAMPLE_PAYLOAD_LEN;
    willInfo.pTopicName = MQTT_CLIENT_IDENTIFIER;
    willInfo.topicNameLength = MQTT_CLIENT_IDENTIFIER_LEN;
    willInfo.dup = true;
    willInfo.qos = MQTTQoS2;
    willInfo.retain = false;
    connectInfo.cleanSession = false;
    connectInfo.pClientIdentifier = MQTT_CLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = MQTT_CLIENT_IDENTIFIER_LEN;
    connectInfo.pUserName = NULL;
    connectInfo.userNameLength = 0;
    connectInfo.pPassword = NULL;
    connectInfo.passwordLength = 0;

    mqttStatus = MQTT_GetConnectPacketSize( &connectInfo,
                                            NULL,
                                            &remainingLength,
                                            &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, mqttStatus );
    /* Make sure buffer has enough space. */
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, networkBuffer.size );
    mqttStatus = MQTT_SerializeConnect( &connectInfo, &willInfo,
                                        remainingLength, &networkBuffer );
    TEST_ASSERT_EQUAL( MQTTSuccess, mqttStatus );
    verifySerializedConnectPacket( &connectInfo, &willInfo,
                                   remainingLength, &networkBuffer );


    /* Repeat with NULL pWillInfo. */
    mqttStatus = MQTT_GetConnectPacketSize( &connectInfo,
                                            NULL,
                                            &remainingLength,
                                            &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, mqttStatus );
    /* Make sure buffer has enough space. */
    TEST_ASSERT_GREATER_OR_EQUAL( packetSize, networkBuffer.size );
    mqttStatus = MQTT_SerializeConnect( &connectInfo, NULL,
                                        remainingLength, &networkBuffer );
    TEST_ASSERT_EQUAL( MQTTSuccess, mqttStatus );
    verifySerializedConnectPacket( &connectInfo, NULL,
                                   remainingLength, &networkBuffer );
}

/* ==================  Testing MQTT_SerializeDisconnect ===================== */

/**
 * @brief Call Mqtt_SerializeDisconnect using NULL pBuffer and insufficient buffer
 * size in order to receive MQTTBadParameter and MQTTNoMemory errors.
 */
void test_MQTT_SerializeDisconnect_Invalid_Params()
{
    MQTTStatus_t mqttStatus = MQTTSuccess;
    size_t packetSize = 0;
    MQTTFixedBuffer_t networkBuffer;

    /* Test NULL pBuffer. */
    mqttStatus = MQTT_SerializeDisconnect( NULL );
    TEST_ASSERT_EQUAL( MQTTBadParameter, mqttStatus );

    /* Test disconnectPacketSize > pBuffer->size. */
    /* Get MQTT disconnect packet size and remaining length. */
    mqttStatus = MQTT_GetDisconnectPacketSize( &packetSize );
    TEST_ASSERT_EQUAL( MQTTSuccess, mqttStatus );
    networkBuffer.pBuffer = mqttBuffer;
    networkBuffer.size = packetSize - 1;
    mqttStatus = MQTT_SerializeDisconnect( &networkBuffer );
    TEST_ASSERT_EQUAL( MQTTNoMemory, mqttStatus );
}

/**
 * @brief This method calls MQTT_SerializeDisconnect successfully in order to
 * get full coverage on the method.
 */
void test_MQTT_SerializeDisconnect_Happy_Path()
{
    MQTTStatus_t mqttStatus = MQTTSuccess;
    size_t packetSize = 0;
    MQTTFixedBuffer_t networkBuffer;

    /* Fill structs to pass into methods to be tested. */
    setupNetworkBuffer( &networkBuffer );

    /* Make sure buffer has enough space. */
    mqttStatus = MQTT_SerializeDisconnect( &networkBuffer );
    TEST_ASSERT_EQUAL( MQTT_PACKET_TYPE_DISCONNECT, networkBuffer.pBuffer[ 0 ] );
    TEST_ASSERT_EQUAL( MQTT_DISCONNECT_REMAINING_LENGTH, networkBuffer.pBuffer[ 1 ] );
    TEST_ASSERT_EQUAL( MQTTSuccess, mqttStatus );
}

/* ========================================================================== */
