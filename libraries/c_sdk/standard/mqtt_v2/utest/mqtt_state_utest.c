#include "unity.h"

#include "mqtt_state.h"

#define MQTT_PACKET_ID_INVALID    ( ( uint16_t ) 0U )

/* ============================   UNITY FIXTURES ============================ */
void setUp( void )
{
}

/* called before each testcase */
void tearDown( void )
{
}

/* called at the beginning of the whole suite */
void suiteSetUp()
{
}

/* called at the end of the whole suite */
int suiteTearDown( int numFailures )
{
    return numFailures;
}

/* ========================================================================== */

static void resetPublishRecords( MQTTContext_t * pMqttContext )
{
    int i = 0;

    for( ; i < MQTT_STATE_ARRAY_MAX_COUNT; i++ )
    {
        pMqttContext->outgoingPublishRecords[ i ].packetId = MQTT_PACKET_ID_INVALID;
        pMqttContext->outgoingPublishRecords[ i ].qos = MQTTQoS0;
        pMqttContext->outgoingPublishRecords[ i ].publishState = MQTTStateNull;
        pMqttContext->incomingPublishRecords[ i ].packetId = MQTT_PACKET_ID_INVALID;
        pMqttContext->incomingPublishRecords[ i ].qos = MQTTQoS0;
        pMqttContext->incomingPublishRecords[ i ].publishState = MQTTStateNull;
    }
}

static void addToRecord( MQTTPubAckInfo_t * records,
                         size_t index,
                         uint16_t packetId,
                         MQTTQoS_t qos,
                         MQTTPublishState_t state )
{
    records[ index ].packetId = packetId;
    records[ index ].qos = qos;
    records[ index ].publishState = state;
}

static void fillRecord( MQTTPubAckInfo_t * records,
                        uint16_t startingId,
                        MQTTQoS_t qos,
                        MQTTPublishState_t state )
{
    int i;

    for( i = 0; i < MQTT_STATE_ARRAY_MAX_COUNT; i++ )
    {
        records[ i ].packetId = startingId + i;
        records[ i ].qos = qos;
        records[ i ].publishState = state;
    }
}

/* ========================================================================== */

void test_MQTT_ReserveState( void )
{
    MQTTContext_t mqttContext = { 0 };
    MQTTStatus_t status;

    /* QoS 0 returns success. */
    TEST_ASSERT_EQUAL( MQTTSuccess, MQTT_ReserveState( NULL, MQTT_PACKET_ID_INVALID, MQTTQoS0 ) );

    /* Test for bad parameters */
    status = MQTT_ReserveState( &mqttContext, MQTT_PACKET_ID_INVALID, MQTTQoS1 );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );
    status = MQTT_ReserveState( NULL, 1, MQTTQoS1 );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    /* Test for collisions. */
    mqttContext.outgoingPublishRecords[ 1 ].packetId = 1;
    mqttContext.outgoingPublishRecords[ 1 ].qos = MQTTQoS1;
    mqttContext.outgoingPublishRecords[ 1 ].publishState = MQTTPublishSend;

    status = MQTT_ReserveState( &mqttContext, 1, MQTTQoS1 );
    TEST_ASSERT_EQUAL( MQTTStateCollision, status );

    /* Test for no memory. */
    fillRecord( mqttContext.outgoingPublishRecords, 2, MQTTQoS1, MQTTPublishSend );
    status = MQTT_ReserveState( &mqttContext, 1, MQTTQoS1 );
    TEST_ASSERT_EQUAL( MQTTNoMemory, status );

    /* Success. */
    resetPublishRecords( &mqttContext );
    status = MQTT_ReserveState( &mqttContext, 1, MQTTQoS1 );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    /* Reserve uses first available entry. */
    TEST_ASSERT_EQUAL( 1, mqttContext.outgoingPublishRecords[ 0 ].packetId );
    TEST_ASSERT_EQUAL( MQTTQoS1, mqttContext.outgoingPublishRecords[ 0 ].qos );
    TEST_ASSERT_EQUAL( MQTTPublishSend, mqttContext.outgoingPublishRecords[ 0 ].publishState );
}

/* ========================================================================== */

void test_MQTT_CalculateStatePublish( void )
{
    /* QoS 0. */
    TEST_ASSERT_EQUAL( MQTTPublishDone, MQTT_CalculateStatePublish( MQTT_SEND, MQTTQoS0 ) );
    TEST_ASSERT_EQUAL( MQTTPublishDone, MQTT_CalculateStatePublish( MQTT_RECEIVE, MQTTQoS0 ) );

    /* QoS 1. */
    TEST_ASSERT_EQUAL( MQTTPubAckPending, MQTT_CalculateStatePublish( MQTT_SEND, MQTTQoS1 ) );
    TEST_ASSERT_EQUAL( MQTTPubAckSend, MQTT_CalculateStatePublish( MQTT_RECEIVE, MQTTQoS1 ) );

    /* QoS 2. */
    TEST_ASSERT_EQUAL( MQTTPubRecPending, MQTT_CalculateStatePublish( MQTT_SEND, MQTTQoS2 ) );
    TEST_ASSERT_EQUAL( MQTTPubRecSend, MQTT_CalculateStatePublish( MQTT_RECEIVE, MQTTQoS2 ) );

    /* Invalid QoS. */
    TEST_ASSERT_EQUAL( MQTTStateNull, MQTT_CalculateStatePublish( MQTT_SEND, 3 ) );
}

/* ========================================================================== */

void test_MQTT_UpdateStatePublish( void )
{
    MQTTContext_t mqttContext = { 0 };
    const uint16_t PACKET_ID = 1;
    MQTTStateOperation_t operation = MQTT_SEND;
    MQTTQoS_t qos = MQTTQoS0;
    MQTTPublishState_t state;
    MQTTStatus_t status;

    /* QoS 0. */
    status = MQTT_UpdateStatePublish( &mqttContext, 0, operation, qos, &state );
    TEST_ASSERT_EQUAL( MQTTPublishDone, state );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );

    /* Invalid parameters. */
    /* Invalid ID. */
    qos = MQTTQoS1;
    status = MQTT_UpdateStatePublish( &mqttContext, 0, operation, qos, &state );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );
    /* NULL context. */
    status = MQTT_UpdateStatePublish( NULL, PACKET_ID, operation, qos, &state );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );
    /* NULL new state. */
    status = MQTT_UpdateStatePublish( &mqttContext, PACKET_ID, operation, qos, NULL );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );
    /* No record found. */
    status = MQTT_UpdateStatePublish( &mqttContext, PACKET_ID, operation, qos, &state );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );
    /* QoS mismatch. */
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, MQTTQoS2, MQTTPublishSend );
    status = MQTT_UpdateStatePublish( &mqttContext, PACKET_ID, operation, qos, &state );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    /* Invalid transition. */
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, MQTTQoS1, MQTTPubAckPending );
    status = MQTT_UpdateStatePublish( &mqttContext, PACKET_ID, operation, qos, &state );
    TEST_ASSERT_EQUAL( MQTTIllegalState, status );

    /* Invalid QoS. */
    operation = MQTT_SEND;
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, 3, MQTTPublishSend );
    status = MQTT_UpdateStatePublish( &mqttContext, PACKET_ID, operation, 3, &state );
    TEST_ASSERT_EQUAL( MQTTIllegalState, status );
    operation = MQTT_RECEIVE;
    status = MQTT_UpdateStatePublish( &mqttContext, PACKET_ID, operation, 3, &state );
    TEST_ASSERT_EQUAL( MQTTIllegalState, status );

    /* Invalid current state. */
    operation = MQTT_SEND;
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, qos, MQTTStateNull );
    status = MQTT_UpdateStatePublish( &mqttContext, PACKET_ID, operation, qos, &state );
    TEST_ASSERT_EQUAL( MQTTIllegalState, status );

    /* Collision. */
    operation = MQTT_RECEIVE;
    addToRecord( mqttContext.incomingPublishRecords, 0, PACKET_ID, MQTTQoS1, MQTTPubAckSend );
    status = MQTT_UpdateStatePublish( &mqttContext, PACKET_ID, operation, qos, &state );
    TEST_ASSERT_EQUAL( MQTTStateCollision, status );

    /* No memory. */
    operation = MQTT_RECEIVE;
    fillRecord( mqttContext.incomingPublishRecords, 2, MQTTQoS1, MQTTPublishSend );
    status = MQTT_UpdateStatePublish( &mqttContext, PACKET_ID, operation, qos, &state );
    TEST_ASSERT_EQUAL( MQTTNoMemory, status );

    resetPublishRecords( &mqttContext );

    /* QoS 1. */
    qos = MQTTQoS1;
    /* Send. */
    operation = MQTT_SEND;
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, MQTTQoS1, MQTTPublishSend );
    status = MQTT_UpdateStatePublish( &mqttContext, PACKET_ID, operation, qos, &state );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_EQUAL( MQTTPubAckPending, state );
    TEST_ASSERT_EQUAL( MQTTPubAckPending, mqttContext.outgoingPublishRecords[ 0 ].publishState );
    /* Receive. */
    operation = MQTT_RECEIVE;
    status = MQTT_UpdateStatePublish( &mqttContext, PACKET_ID, operation, qos, &state );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_EQUAL( MQTTPubAckSend, state );
    TEST_ASSERT_EQUAL( MQTTPubAckSend, mqttContext.incomingPublishRecords[ 0 ].publishState );

    resetPublishRecords( &mqttContext );

    /* QoS 2. */
    qos = MQTTQoS2;
    /* Send. */
    operation = MQTT_SEND;
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, MQTTQoS2, MQTTPublishSend );
    status = MQTT_UpdateStatePublish( &mqttContext, PACKET_ID, operation, qos, &state );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_EQUAL( MQTTPubRecPending, state );
    TEST_ASSERT_EQUAL( MQTTPubRecPending, mqttContext.outgoingPublishRecords[ 0 ].publishState );
    /* Receive. */
    operation = MQTT_RECEIVE;
    status = MQTT_UpdateStatePublish( &mqttContext, PACKET_ID, operation, qos, &state );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_EQUAL( MQTTPubRecSend, state );
    TEST_ASSERT_EQUAL( MQTTPubRecSend, mqttContext.incomingPublishRecords[ 0 ].publishState );
}

/* ========================================================================== */

void test_MQTT_CalculateStateAck( void )
{
    MQTTPubAckType_t ack;
    MQTTQoS_t qos;
    MQTTStateOperation_t opType;

    /* Invalid qos. */
    qos = MQTTQoS0;
    ack = MQTTPuback;
    opType = MQTT_SEND;
    TEST_ASSERT_EQUAL( MQTTStateNull, MQTT_CalculateStateAck( ack, opType, qos ) );
    qos = MQTTQoS2;
    TEST_ASSERT_EQUAL( MQTTStateNull, MQTT_CalculateStateAck( ack, opType, qos ) );
    qos = MQTTQoS0;
    ack = MQTTPubrec;
    TEST_ASSERT_EQUAL( MQTTStateNull, MQTT_CalculateStateAck( ack, opType, qos ) );
    qos = MQTTQoS1;
    TEST_ASSERT_EQUAL( MQTTStateNull, MQTT_CalculateStateAck( ack, opType, qos ) );

    /* Invalid ack type. */
    ack = MQTTPubcomp + 1;
    TEST_ASSERT_EQUAL( MQTTStateNull, MQTT_CalculateStateAck( ack, opType, qos ) );

    /* PUBACK */
    ack = MQTTPuback;
    qos = MQTTQoS1;
    opType = MQTT_SEND;
    TEST_ASSERT_EQUAL( MQTTPublishDone, MQTT_CalculateStateAck( ack, opType, qos ) );
    opType = MQTT_RECEIVE;
    TEST_ASSERT_EQUAL( MQTTPublishDone, MQTT_CalculateStateAck( ack, opType, qos ) );

    /* QoS 2 tests. */
    qos = MQTTQoS2;

    /* PUBREC */
    ack = MQTTPubrec;
    /* Send */
    opType = MQTT_SEND;
    TEST_ASSERT_EQUAL( MQTTPubRelPending, MQTT_CalculateStateAck( ack, opType, qos ) );
    /* Receive */
    opType = MQTT_RECEIVE;
    TEST_ASSERT_EQUAL( MQTTPubRelSend, MQTT_CalculateStateAck( ack, opType, qos ) );

    /* PUBREL */
    ack = MQTTPubrel;
    /* Send */
    opType = MQTT_SEND;
    TEST_ASSERT_EQUAL( MQTTPubCompPending, MQTT_CalculateStateAck( ack, opType, qos ) );
    /* Receive */
    opType = MQTT_RECEIVE;
    TEST_ASSERT_EQUAL( MQTTPubCompSend, MQTT_CalculateStateAck( ack, opType, qos ) );

    /* PUBCOMP */
    ack = MQTTPubcomp;
    TEST_ASSERT_EQUAL( MQTTPublishDone, MQTT_CalculateStateAck( ack, opType, qos ) );
    opType = MQTT_SEND;
    TEST_ASSERT_EQUAL( MQTTPublishDone, MQTT_CalculateStateAck( ack, opType, qos ) );
}

/* ========================================================================== */

void test_MQTT_UpdateStateAck( void )
{
    MQTTContext_t mqttContext = { 0 };
    MQTTPubAckType_t ack = MQTTPuback;
    MQTTStateOperation_t operation = MQTT_RECEIVE;
    MQTTPublishState_t state = MQTTStateNull;
    MQTTStatus_t status;

    const uint16_t PACKET_ID = 1;

    /* NULL parameters. */
    status = MQTT_UpdateStateAck( NULL, PACKET_ID, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );
    status = MQTT_UpdateStateAck( &mqttContext, PACKET_ID, ack, operation, NULL );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );
    /* No matching record found. */
    status = MQTT_UpdateStateAck( &mqttContext, PACKET_ID, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );
    /* Invalid packet ID. */
    status = MQTT_UpdateStateAck( &mqttContext, 0, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    /* Invalid transition. */
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, MQTTQoS2, MQTTPubRecPending );
    ack = MQTTPubcomp;
    status = MQTT_UpdateStateAck( &mqttContext, 1, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTIllegalState, status );

    /* Invalid ack type. */
    status = MQTT_UpdateStateAck( &mqttContext, 1, MQTTPubcomp + 1, operation, &state );
    TEST_ASSERT_EQUAL( MQTTBadParameter, status );

    /* Invalid current state. */
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, MQTTQoS2, MQTTPublishDone );
    status = MQTT_UpdateStateAck( &mqttContext, PACKET_ID, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTIllegalState, status );
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, MQTTQoS2, MQTTPublishSend );
    status = MQTT_UpdateStateAck( &mqttContext, PACKET_ID, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTIllegalState, status );
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, MQTTQoS2, MQTTStateNull );
    status = MQTT_UpdateStateAck( &mqttContext, PACKET_ID, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTIllegalState, status );

    resetPublishRecords( &mqttContext );

    /* QoS 1, outgoing publish. */
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, MQTTQoS1, MQTTPubAckPending );
    operation = MQTT_RECEIVE;
    ack = MQTTPuback;
    status = MQTT_UpdateStateAck( &mqttContext, PACKET_ID, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_EQUAL( MQTTPublishDone, state );
    /* Test for deletion. */
    TEST_ASSERT_EQUAL( MQTTStateNull, mqttContext.outgoingPublishRecords[ 0 ].publishState );
    TEST_ASSERT_EQUAL( MQTT_PACKET_ID_INVALID, mqttContext.outgoingPublishRecords[ 0 ].packetId );
    TEST_ASSERT_EQUAL( MQTTQoS0, mqttContext.outgoingPublishRecords[ 0 ].qos );
    /* Incoming publish. */
    operation = MQTT_SEND;
    addToRecord( mqttContext.incomingPublishRecords, 0, PACKET_ID, MQTTQoS1, MQTTPubAckSend );
    status = MQTT_UpdateStateAck( &mqttContext, PACKET_ID, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_EQUAL( MQTTPublishDone, state );

    resetPublishRecords( &mqttContext );

    /* QoS 2, PUBREL. */
    /* Outgoing. */
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, MQTTQoS2, MQTTPubRelSend );
    operation = MQTT_SEND;
    ack = MQTTPubrel;
    status = MQTT_UpdateStateAck( &mqttContext, PACKET_ID, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_EQUAL( MQTTPubCompPending, state );
    /* Incoming . */
    addToRecord( mqttContext.incomingPublishRecords, 0, PACKET_ID, MQTTQoS2, MQTTPubRelPending );
    operation = MQTT_RECEIVE;
    status = MQTT_UpdateStateAck( &mqttContext, PACKET_ID, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_EQUAL( MQTTPubCompSend, state );
    /* Test for update. */
    TEST_ASSERT_EQUAL( MQTTPubCompSend, mqttContext.incomingPublishRecords[ 0 ].publishState );

    /* QoS 2, PUBREC. */
    /* Outgoing. */
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, MQTTQoS2, MQTTPubRecPending );
    operation = MQTT_RECEIVE;
    ack = MQTTPubrec;
    status = MQTT_UpdateStateAck( &mqttContext, PACKET_ID, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_EQUAL( MQTTPubRelSend, state );
    /* Incoming. */
    addToRecord( mqttContext.incomingPublishRecords, 0, PACKET_ID, MQTTQoS2, MQTTPubRecSend );
    operation = MQTT_SEND;
    status = MQTT_UpdateStateAck( &mqttContext, PACKET_ID, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_EQUAL( MQTTPubRelPending, state );

    /* QoS 2, PUBCOMP. */
    /* Outgoing. */
    addToRecord( mqttContext.outgoingPublishRecords, 0, PACKET_ID, MQTTQoS2, MQTTPubCompPending );
    operation = MQTT_RECEIVE;
    ack = MQTTPubcomp;
    status = MQTT_UpdateStateAck( &mqttContext, PACKET_ID, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_EQUAL( MQTTPublishDone, state );
    /* Incoming. */
    addToRecord( mqttContext.incomingPublishRecords, 0, PACKET_ID, MQTTQoS2, MQTTPubCompSend );
    operation = MQTT_SEND;
    status = MQTT_UpdateStateAck( &mqttContext, PACKET_ID, ack, operation, &state );
    TEST_ASSERT_EQUAL( MQTTSuccess, status );
    TEST_ASSERT_EQUAL( MQTTPublishDone, state );
}

/* ========================================================================== */

void test_MQTT_StateSelect( void )
{
    MQTTContext_t mqttContext = { 0 };
    MQTTStateCursor_t outgoingCursor = MQTT_STATE_CURSOR_INITIALIZER;
    MQTTStateCursor_t incomingCursor = MQTT_STATE_CURSOR_INITIALIZER;
    MQTTPublishState_t search = MQTTPublishSend;
    uint16_t packetId;
    const uint16_t PACKET_ID = 1;
    const uint16_t PACKET_ID2 = 2;
    const size_t index = MQTT_STATE_ARRAY_MAX_COUNT / 2;
    const size_t secondIndex = index + 2;

    /* Invalid parameters. */
    packetId = MQTT_StateSelect( NULL, MQTTPublishSend, &outgoingCursor );
    TEST_ASSERT_EQUAL( MQTT_PACKET_ID_INVALID, packetId );
    packetId = MQTT_StateSelect( NULL, MQTTPubAckSend, &incomingCursor );
    TEST_ASSERT_EQUAL( MQTT_PACKET_ID_INVALID, packetId );
    packetId = MQTT_StateSelect( &mqttContext, search, NULL );
    TEST_ASSERT_EQUAL( MQTT_PACKET_ID_INVALID, packetId );
    packetId = MQTT_StateSelect( &mqttContext, MQTTStateNull, &outgoingCursor );
    TEST_ASSERT_EQUAL( MQTT_PACKET_ID_INVALID, packetId );

    /* Incoming. */
    search = MQTTPubAckSend;
    addToRecord( mqttContext.incomingPublishRecords, index, PACKET_ID, MQTTQoS1, search );
    packetId = MQTT_StateSelect( &mqttContext, search, &incomingCursor );
    TEST_ASSERT_EQUAL( PACKET_ID, packetId );
    TEST_ASSERT_EQUAL( index + 1, incomingCursor );

    /* Outgoing. */
    search = MQTTPublishSend;
    addToRecord( mqttContext.outgoingPublishRecords, index, PACKET_ID, MQTTQoS1, search );
    packetId = MQTT_StateSelect( &mqttContext, search, &outgoingCursor );
    TEST_ASSERT_EQUAL( PACKET_ID, packetId );
    TEST_ASSERT_EQUAL( index + 1, outgoingCursor );

    /* Test if second one can be found. */
    addToRecord( mqttContext.outgoingPublishRecords, secondIndex, PACKET_ID2, MQTTQoS2, search );
    packetId = MQTT_StateSelect( &mqttContext, search, &outgoingCursor );
    TEST_ASSERT_EQUAL( PACKET_ID2, packetId );
    TEST_ASSERT_EQUAL( secondIndex + 1, outgoingCursor );

    /* Test if end of loop reached. */
    packetId = MQTT_StateSelect( &mqttContext, search, &outgoingCursor );
    TEST_ASSERT_EQUAL( MQTT_PACKET_ID_INVALID, packetId );
    TEST_ASSERT_EQUAL( MQTT_STATE_ARRAY_MAX_COUNT, outgoingCursor );
}

void test_MQTT_State_strerror( void )
{
    MQTTPublishState_t state;
    const char * str = NULL;

    state = MQTTStateNull;
    str = MQTT_State_strerror( state );
    TEST_ASSERT_EQUAL_STRING( "MQTTStateNull", str );

    state = MQTTPublishSend;
    str = MQTT_State_strerror( state );
    TEST_ASSERT_EQUAL_STRING( "MQTTPublishSend", str );

    state = MQTTPubAckSend;
    str = MQTT_State_strerror( state );
    TEST_ASSERT_EQUAL_STRING( "MQTTPubAckSend", str );

    state = MQTTPubRecSend;
    str = MQTT_State_strerror( state );
    TEST_ASSERT_EQUAL_STRING( "MQTTPubRecSend", str );

    state = MQTTPubRelSend;
    str = MQTT_State_strerror( state );
    TEST_ASSERT_EQUAL_STRING( "MQTTPubRelSend", str );

    state = MQTTPubCompSend;
    str = MQTT_State_strerror( state );
    TEST_ASSERT_EQUAL_STRING( "MQTTPubCompSend", str );

    state = MQTTPubAckPending;
    str = MQTT_State_strerror( state );
    TEST_ASSERT_EQUAL_STRING( "MQTTPubAckPending", str );

    state = MQTTPubRecPending;
    str = MQTT_State_strerror( state );
    TEST_ASSERT_EQUAL_STRING( "MQTTPubRecPending", str );

    state = MQTTPubRelPending;
    str = MQTT_State_strerror( state );
    TEST_ASSERT_EQUAL_STRING( "MQTTPubRelPending", str );

    state = MQTTPubCompPending;
    str = MQTT_State_strerror( state );
    TEST_ASSERT_EQUAL_STRING( "MQTTPubCompPending", str );

    state = MQTTPublishDone;
    str = MQTT_State_strerror( state );
    TEST_ASSERT_EQUAL_STRING( "MQTTPublishDone", str );

    state = MQTTPublishDone + 1;
    str = MQTT_State_strerror( state );
    TEST_ASSERT_EQUAL_STRING( "Invalid MQTT State", str );
}
