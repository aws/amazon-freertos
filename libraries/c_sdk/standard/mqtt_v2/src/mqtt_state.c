/*
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <assert.h>
#include "mqtt_state.h"

#define MQTT_PACKET_ID_INVALID    ( uint16_t ) 0U

/**
 * @brief Test if a transition to new state is possible, when dealing with PUBLISHes.
 *
 * @param[in] currentState The current state.
 * @param[in] newState State to transition to.
 * @param[in] opType Reserve, Send, or Receive.
 * @param[in] qos 0, 1, or 2.
 *
 * @note This function does not validate the current state, or the new state
 * based on either the operation type or QoS. It assumes the new state is valid
 * given the opType and QoS, which will be the case if calculated by
 * MQTT_CalculateStatePublish().
 *
 * @return `true` if transition is possible, else `false`
 */
static bool validateTransitionPublish( MQTTPublishState_t currentState,
                                       MQTTPublishState_t newState,
                                       MQTTStateOperation_t opType,
                                       MQTTQoS_t qos );

/**
 * @brief Test if a transition to a new state is possible, when dealing with acks.
 *
 * @param[in] currentState The current state.
 * @param[in] newState State to transition to.
 *
 * @return `true` if transition is possible, else `false`.
 */
static bool validateTransitionAck( MQTTPublishState_t currentState,
                                   MQTTPublishState_t newState );

/**
 * @brief Test if the publish corresponding to an ack is outgoing or incoming.
 *
 * @param[in] packetType PUBACK, PUBREC, PUBREL, or PUBCOMP.
 * @param[in] opType Send, or Receive.
 *
 * @return `true` if corresponds to outgoing publish, else `false`.
 */
static bool isPublishOutgoing( MQTTPubAckType_t packetType,
                               MQTTStateOperation_t opType );

/**
 * @brief Find a packet ID in the state record.
 *
 * @param[in] records State record array.
 * @param[in] recordCount Length of record array.
 * @param[in] packetId packet ID to search for.
 * @param[out] pQos QoS retrieved from record.
 * @param[out] pCurrentState state retrieved from record.
 *
 * @return index of the packet id in the record if it exists, else the record length.
 */
static size_t findInRecord( const MQTTPubAckInfo_t * records,
                            size_t recordCount,
                            uint16_t packetId,
                            MQTTQoS_t * pQos,
                            MQTTPublishState_t * pCurrentState );

/**
 * @brief Store a new entry in the state record.
 *
 * @param[in] records State record array.
 * @param[in] recordCount Length of record array.
 * @param[in] packetId, packet ID of new entry.
 * @param[in] qos QoS of new entry.
 * @param[in] publishState state of new entry.
 *
 * @return MQTTSuccess, MQTTNoMemory, MQTTStateCollision.
 */
static MQTTStatus_t addRecord( MQTTPubAckInfo_t * records,
                               size_t recordCount,
                               uint16_t packetId,
                               MQTTQoS_t qos,
                               MQTTPublishState_t publishState );

/**
 * @brief Update and possibly delete an entry in the state record.
 *
 * @param[in] records State record array.
 * @param[in] recordIndex index of record to update.
 * @param[in] newState New state to update.
 * @param[in] shouldDelete Whether an existing entry should be deleted.
 */
static void updateRecord( MQTTPubAckInfo_t * records,
                          size_t recordIndex,
                          MQTTPublishState_t newState,
                          bool shouldDelete );


static bool validateTransitionPublish( MQTTPublishState_t currentState,
                                       MQTTPublishState_t newState,
                                       MQTTStateOperation_t opType,
                                       MQTTQoS_t qos )
{
    bool isValid = false;

    switch( currentState )
    {
        case MQTTStateNull:

            /* Transitions from null occur when storing a new entry into the record. */
            if( opType == MQTT_RECEIVE )
            {
                isValid = ( ( newState == MQTTPubAckSend ) || ( newState == MQTTPubRecSend ) ) ? true : false;
            }

            break;

        case MQTTPublishSend:

            /* Outgoing publish. All such publishes start in this state due to
             * the reserve operation. */
            switch( qos )
            {
                case MQTTQoS1:
                    isValid = ( newState == MQTTPubAckPending ) ? true : false;
                    break;

                case MQTTQoS2:
                    isValid = ( newState == MQTTPubRecPending ) ? true : false;
                    break;

                default:
                    /* QoS 0 is checked before calling this function. */
                    break;
            }

            break;

        default:
            /* For a PUBLISH, we should not start from any other state. */
            break;
    }

    return isValid;
}

static bool validateTransitionAck( MQTTPublishState_t currentState,
                                   MQTTPublishState_t newState )
{
    bool isValid = false;

    switch( currentState )
    {
        case MQTTPubAckSend:
        /* Incoming publish, QoS 1. */
        case MQTTPubAckPending:
            /* Outgoing publish, QoS 1. */
            isValid = ( newState == MQTTPublishDone ) ? true : false;
            break;

        case MQTTPubRecSend:
            /* Incoming publish, QoS 2. */
            isValid = ( newState == MQTTPubRelPending ) ? true : false;
            break;

        case MQTTPubRelPending:
            /* Incoming publish, QoS 2. */
            isValid = ( newState == MQTTPubCompSend ) ? true : false;
            break;

        case MQTTPubCompSend:
            /* Incoming publish, QoS 2. */
            isValid = ( newState == MQTTPublishDone ) ? true : false;
            break;

        case MQTTPubRecPending:
            /* Outgoing publish, Qos 2. */
            isValid = ( newState == MQTTPubRelSend ) ? true : false;
            break;

        case MQTTPubRelSend:
            /* Outgoing publish, Qos 2. */
            isValid = ( newState == MQTTPubCompPending ) ? true : false;
            break;

        case MQTTPubCompPending:
            /* Outgoing publish, Qos 2. */
            isValid = ( newState == MQTTPublishDone ) ? true : false;
            break;

        case MQTTPublishDone:
        /* Done state should transition to invalid since it will be removed from the record. */
        case MQTTPublishSend:
        /* If an ack was sent/received we shouldn't have been in this state. */
        case MQTTStateNull:
        /* If an ack was sent/received the record should exist. */
        default:
            /* Invalid. */
            break;
    }

    return isValid;
}

static bool isPublishOutgoing( MQTTPubAckType_t packetType,
                               MQTTStateOperation_t opType )
{
    bool isOutgoing = false;

    switch( packetType )
    {
        case MQTTPuback:
        case MQTTPubrec:
        case MQTTPubcomp:
            isOutgoing = ( opType == MQTT_RECEIVE ) ? true : false;
            break;

        case MQTTPubrel:
            isOutgoing = ( opType == MQTT_SEND ) ? true : false;
            break;

        default:
            /* No other ack type. */
            break;
    }

    return isOutgoing;
}

static size_t findInRecord( const MQTTPubAckInfo_t * records,
                            size_t recordCount,
                            uint16_t packetId,
                            MQTTQoS_t * pQos,
                            MQTTPublishState_t * pCurrentState )
{
    size_t index = 0;

    *pCurrentState = MQTTStateNull;

    if( packetId == MQTT_PACKET_ID_INVALID )
    {
        index = recordCount;
    }
    else
    {
        for( index = 0; index < recordCount; index++ )
        {
            if( records[ index ].packetId == packetId )
            {
                *pQos = records[ index ].qos;
                *pCurrentState = records[ index ].publishState;
                break;
            }
        }
    }

    return index;
}

static MQTTStatus_t addRecord( MQTTPubAckInfo_t * records,
                               size_t recordCount,
                               uint16_t packetId,
                               MQTTQoS_t qos,
                               MQTTPublishState_t publishState )
{
    MQTTStatus_t status = MQTTNoMemory;
    int32_t index = 0;
    size_t availableIndex = recordCount;

    assert( packetId != MQTT_PACKET_ID_INVALID );
    assert( qos != MQTTQoS0 );

    /* Start from end so first available index will be populated. */
    for( index = ( ( int32_t ) recordCount - 1 ); index >= 0; index-- )
    {
        if( records[ index ].packetId == MQTT_PACKET_ID_INVALID )
        {
            availableIndex = ( size_t ) index;
        }
        else if( records[ index ].packetId == packetId )
        {
            /* Collision. */
            LogError( ( "Collision when adding PacketID=%u at index=%u",
                        packetId,
                        index ) );

            status = MQTTStateCollision;
            availableIndex = recordCount;
            break;
        }
        else
        {
            /* Empty else clause. */
        }
    }

    if( availableIndex < recordCount )
    {
        records[ availableIndex ].packetId = packetId;
        records[ availableIndex ].qos = qos;
        records[ availableIndex ].publishState = publishState;
        status = MQTTSuccess;
    }

    return status;
}

static void updateRecord( MQTTPubAckInfo_t * records,
                          size_t recordIndex,
                          MQTTPublishState_t newState,
                          bool shouldDelete )
{
    if( shouldDelete == true )
    {
        records[ recordIndex ].packetId = MQTT_PACKET_ID_INVALID;
        records[ recordIndex ].qos = MQTTQoS0;
        records[ recordIndex ].publishState = MQTTStateNull;
    }
    else
    {
        records[ recordIndex ].publishState = newState;
    }
}

MQTTStatus_t MQTT_ReserveState( MQTTContext_t * pMqttContext,
                                uint16_t packetId,
                                MQTTQoS_t qos )
{
    MQTTStatus_t status = MQTTSuccess;

    if( qos == MQTTQoS0 )
    {
        status = MQTTSuccess;
    }
    else if( ( packetId == MQTT_PACKET_ID_INVALID ) || ( pMqttContext == NULL ) )
    {
        status = MQTTBadParameter;
    }
    else
    {
        /* Collisions are detected when adding the record. */
        status = addRecord( pMqttContext->outgoingPublishRecords,
                            MQTT_STATE_ARRAY_MAX_COUNT,
                            packetId,
                            qos,
                            MQTTPublishSend );
    }

    return status;
}

MQTTPublishState_t MQTT_CalculateStatePublish( MQTTStateOperation_t opType,
                                               MQTTQoS_t qos )
{
    MQTTPublishState_t calculatedState = MQTTStateNull;

    switch( qos )
    {
        case MQTTQoS0:
            calculatedState = MQTTPublishDone;
            break;

        case MQTTQoS1:
            calculatedState = ( opType == MQTT_SEND ) ? MQTTPubAckPending : MQTTPubAckSend;
            break;

        case MQTTQoS2:
            calculatedState = ( opType == MQTT_SEND ) ? MQTTPubRecPending : MQTTPubRecSend;
            break;

        default:
            /* No other QoS values. */
            break;
    }

    return calculatedState;
}

MQTTStatus_t MQTT_UpdateStatePublish( MQTTContext_t * pMqttContext,
                                      uint16_t packetId,
                                      MQTTStateOperation_t opType,
                                      MQTTQoS_t qos,
                                      MQTTPublishState_t * pNewState )
{
    MQTTPublishState_t newState = MQTTStateNull;
    MQTTPublishState_t currentState = MQTTStateNull;
    MQTTStatus_t mqttStatus = MQTTSuccess;
    size_t recordIndex = MQTT_STATE_ARRAY_MAX_COUNT;
    MQTTQoS_t foundQoS = MQTTQoS0;
    bool isTransitionValid = false;

    if( ( pMqttContext == NULL ) || ( pNewState == NULL ) )
    {
        LogError( ( "Argument cannot be NULL: pMqttContext=%p, pNewState=%p",
                    pMqttContext,
                    pNewState ) );

        mqttStatus = MQTTBadParameter;
    }
    else if( qos == MQTTQoS0 )
    {
        /* QoS 0 publish. Do nothing. */
        *pNewState = MQTTPublishDone;
    }
    else if( packetId == MQTT_PACKET_ID_INVALID )
    {
        /* Publishes > QoS 0 need a valid packet ID. */
        mqttStatus = MQTTBadParameter;
    }
    else if( opType == MQTT_SEND )
    {
        /* Search record for entry so we can check QoS. */
        recordIndex = findInRecord( pMqttContext->outgoingPublishRecords,
                                    MQTT_STATE_ARRAY_MAX_COUNT,
                                    packetId,
                                    &foundQoS,
                                    &currentState );

        if( ( recordIndex == MQTT_STATE_ARRAY_MAX_COUNT ) || ( foundQoS != qos ) )
        {
            /* Entry should match with supplied QoS. */
            mqttStatus = MQTTBadParameter;
        }
    }
    else
    {
        /* QoS 1 or 2 receive. Nothing to be done. */
    }

    if( ( qos != MQTTQoS0 ) && ( mqttStatus == MQTTSuccess ) )
    {
        newState = MQTT_CalculateStatePublish( opType, qos );
        isTransitionValid = validateTransitionPublish( currentState, newState, opType, qos );

        if( isTransitionValid == true )
        {
            /* addRecord will check for collisions. */
            if( opType == MQTT_RECEIVE )
            {
                mqttStatus = addRecord( pMqttContext->incomingPublishRecords,
                                        MQTT_STATE_ARRAY_MAX_COUNT,
                                        packetId,
                                        qos,
                                        newState );
            }
            /* Send operation. */
            else
            {
                updateRecord( pMqttContext->outgoingPublishRecords, recordIndex, newState, false );
            }
        }
        else
        {
            mqttStatus = MQTTIllegalState;
            LogError( ( "Invalid transition from state %s to state %s.",
                        MQTT_State_strerror( currentState ),
                        MQTT_State_strerror( newState ) ) );
        }

        *pNewState = newState;
    }

    return mqttStatus;
}

MQTTPublishState_t MQTT_CalculateStateAck( MQTTPubAckType_t packetType,
                                           MQTTStateOperation_t opType,
                                           MQTTQoS_t qos )
{
    MQTTPublishState_t calculatedState = MQTTStateNull;
    /* There are more QoS2 cases than QoS1, so initialize to that. */
    bool qosValid = ( qos == MQTTQoS2 ) ? true : false;

    switch( packetType )
    {
        case MQTTPuback:
            qosValid = ( qos == MQTTQoS1 ) ? true : false;
            calculatedState = MQTTPublishDone;
            break;

        case MQTTPubrec:

            /* Incoming publish: send PUBREC, PUBREL pending.
             * Outgoing publish: receive PUBREC, send PUBREL. */
            calculatedState = ( opType == MQTT_SEND ) ? MQTTPubRelPending : MQTTPubRelSend;
            break;

        case MQTTPubrel:

            /* Incoming publish: receive PUBREL, send PUBCOMP.
             * Outgoing publish: send PUBREL, PUBCOMP pending. */
            calculatedState = ( opType == MQTT_SEND ) ? MQTTPubCompPending : MQTTPubCompSend;
            break;

        case MQTTPubcomp:
            calculatedState = MQTTPublishDone;
            break;

        default:
            /* No other ack type. */
            break;
    }

    /* Sanity check, make sure ack and QoS agree. */
    if( qosValid == false )
    {
        calculatedState = MQTTStateNull;
    }

    return calculatedState;
}

MQTTStatus_t MQTT_UpdateStateAck( MQTTContext_t * pMqttContext,
                                  uint16_t packetId,
                                  MQTTPubAckType_t packetType,
                                  MQTTStateOperation_t opType,
                                  MQTTPublishState_t * pNewState )
{
    MQTTPublishState_t newState = MQTTStateNull;
    MQTTPublishState_t currentState = MQTTStateNull;
    bool isOutgoingPublish = isPublishOutgoing( packetType, opType );
    bool shouldDeleteRecord = false;
    bool isTransitionValid = false;
    MQTTQoS_t qos = MQTTQoS0;
    size_t recordIndex = MQTT_STATE_ARRAY_MAX_COUNT;
    MQTTPubAckInfo_t * records = NULL;
    MQTTStatus_t status = MQTTBadParameter;

    if( ( pMqttContext == NULL ) || ( pNewState == NULL ) )
    {
        LogError( ( "Argument cannot be NULL: pMqttContext=%p, pNewState=%p.",
                    pMqttContext,
                    pNewState ) );
    }
    else
    {
        if( isOutgoingPublish == true )
        {
            records = pMqttContext->outgoingPublishRecords;
        }
        else
        {
            records = pMqttContext->incomingPublishRecords;
        }

        recordIndex = findInRecord( records,
                                    MQTT_STATE_ARRAY_MAX_COUNT,
                                    packetId,
                                    &qos,
                                    &currentState );
    }

    if( recordIndex < MQTT_STATE_ARRAY_MAX_COUNT )
    {
        newState = MQTT_CalculateStateAck( packetType, opType, qos );
        shouldDeleteRecord = ( newState == MQTTPublishDone ) ? true : false;
        isTransitionValid = validateTransitionAck( currentState, newState );

        if( isTransitionValid == true )
        {
            updateRecord( records,
                          recordIndex,
                          newState,
                          shouldDeleteRecord );

            status = MQTTSuccess;
            *pNewState = newState;
        }
        else
        {
            status = MQTTIllegalState;
            LogError( ( "Invalid transition from state %s to state %s.",
                        MQTT_State_strerror( currentState ),
                        MQTT_State_strerror( newState ) ) );
        }
    }
    else
    {
        LogError( ( "No matching record found for publish %u.", packetId ) );
    }

    return status;
}

uint16_t MQTT_StateSelect( const MQTTContext_t * pMqttContext,
                           MQTTPublishState_t searchState,
                           MQTTStateCursor_t * pCursor )
{
    uint16_t packetId = MQTT_PACKET_ID_INVALID;
    const MQTTPubAckInfo_t * records = NULL;

    /* Classify state into incoming or outgoing so we don't search both. */
    switch( searchState )
    {
        case MQTTPubAckSend:
        case MQTTPubRecSend:
        case MQTTPubRelPending:
        case MQTTPubCompSend:

            if( pMqttContext != NULL )
            {
                records = pMqttContext->incomingPublishRecords;
            }

            break;

        case MQTTPublishSend:
        case MQTTPubAckPending:
        case MQTTPubRecPending:
        case MQTTPubRelSend:
        case MQTTPubCompPending:

            if( pMqttContext != NULL )
            {
                records = pMqttContext->outgoingPublishRecords;
            }

            break;

        default:
            /* NULL or done aren't valid entries to search for. */
            break;
    }

    if( ( records != NULL ) && ( pCursor != NULL ) )
    {
        while( *pCursor < MQTT_STATE_ARRAY_MAX_COUNT )
        {
            if( records[ *pCursor ].publishState == searchState )
            {
                packetId = records[ *pCursor ].packetId;
                ( *pCursor )++;
                break;
            }

            ( *pCursor )++;
        }
    }

    return packetId;
}

const char * MQTT_State_strerror( MQTTPublishState_t state )
{
    const char * str = NULL;

    switch( state )
    {
        case MQTTStateNull:
            str = "MQTTStateNull";
            break;

        case MQTTPublishSend:
            str = "MQTTPublishSend";
            break;

        case MQTTPubAckSend:
            str = "MQTTPubAckSend";
            break;

        case MQTTPubRecSend:
            str = "MQTTPubRecSend";
            break;

        case MQTTPubRelSend:
            str = "MQTTPubRelSend";
            break;

        case MQTTPubCompSend:
            str = "MQTTPubCompSend";
            break;

        case MQTTPubAckPending:
            str = "MQTTPubAckPending";
            break;

        case MQTTPubRecPending:
            str = "MQTTPubRecPending";
            break;

        case MQTTPubRelPending:
            str = "MQTTPubRelPending";
            break;

        case MQTTPubCompPending:
            str = "MQTTPubCompPending";
            break;

        case MQTTPublishDone:
            str = "MQTTPublishDone";
            break;

        default:
            /* Invalid state received. */
            str = "Invalid MQTT State";
            break;
    }

    return str;
}
