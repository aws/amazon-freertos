/*
 * FreeRTOS V202011.00
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
 *
 * https://www.FreeRTOS.org
 * https://aws.amazon.com/freertos
 *
 */

/**
 * @file mqtt_agent.h
 * @brief Functions for running a coreMQTT client in a dedicated thread.
 */
#ifndef MQTT_AGENT_H
#define MQTT_AGENT_H

/* MQTT library includes. */
#include "core_mqtt.h"
#include "core_mqtt_state.h"

/* Queue include. */
#include "agent_message.h"

/**
 * @brief The maximum number of pending acknowledgments to track for a single
 * connection.
 *
 * @note The MQTT agent tracks MQTT commands (such as PUBLISH and SUBSCRIBE) th
 * at are still waiting to be acknowledged.  MQTT_AGENT_MAX_OUTSTANDING_ACKS set
 * the maximum number of acknowledgments that can be outstanding at any one time.
 * The higher this number is the greater the agent's RAM consumption will be.
 */
#ifndef MQTT_AGENT_MAX_OUTSTANDING_ACKS
    #define MQTT_AGENT_MAX_OUTSTANDING_ACKS    ( 20 )
#endif

/**
 * @brief Time in MS that the MQTT agent task will wait in the Blocked state (so
 * not using any CPU time) for a command to arrive in its command queue before
 * exiting the blocked state so it can call MQTT_ProcessLoop().
 *
 * @note It is important MQTT_ProcessLoop() is called often if there is known
 * MQTT traffic, but calling it too often can take processing time away from
 * lower priority tasks and waste CPU time and power.
 */
#ifndef MQTT_AGENT_MAX_EVENT_QUEUE_WAIT_TIME
    #define MQTT_AGENT_MAX_EVENT_QUEUE_WAIT_TIME    ( 1000 )
#endif

/*-----------------------------------------------------------*/

/**
 * @brief A type of command for interacting with the MQTT API.
 */
typedef enum CommandType
{
    NONE = 0,    /**< @brief No command received.  Must be zero (its memset() value). */
    PROCESSLOOP, /**< @brief Call MQTT_ProcessLoop(). */
    PUBLISH,     /**< @brief Call MQTT_Publish(). */
    SUBSCRIBE,   /**< @brief Call MQTT_Subscribe(). */
    UNSUBSCRIBE, /**< @brief Call MQTT_Unsubscribe(). */
    PING,        /**< @brief Call MQTT_Ping(). */
    CONNECT,     /**< @brief Call MQTT_Connect(). */
    DISCONNECT,  /**< @brief Call MQTT_Disconnect(). */
    TERMINATE    /**< @brief Exit the command loop and stop processing commands. */
} CommandType_t;

struct MQTTAgentContext;
typedef struct MQTTAgentContext   MQTTAgentContext_t;

struct Command;
typedef struct Command            Command_t;

/**
 * @brief Struct holding return codes and outputs from a command
 */
typedef struct MQTTAgentReturnInfo
{
    MQTTStatus_t returnCode; /**< Return code of the MQTT command. */
    uint8_t * pSubackCodes;  /**< Array of SUBACK statuses, for a SUBSCRIBE command. */
} MQTTAgentReturnInfo_t;

/**
 * @brief Information for a pending MQTT ack packet expected by the agent.
 */
typedef struct ackInfo
{
    uint16_t packetId;            /**< Packet ID of the pending acknowledgment. */
    Command_t * pOriginalCommand; /**< Command expecting acknowledgment. */
} AckInfo_t;

/**
 * @brief Struct containing context for a specific command.
 *
 * @note An instance of this struct and any variables it points to MUST stay
 * in scope until the associated command is processed, and its callback called.
 */
struct CommandContext;
typedef struct CommandContext CommandContext_t;

/**
 * @brief Callback function called when a command completes.
 *
 * @param[in] pCmdCallbackContext The callback context passed to the original command.
 * @param[in] pReturnInfo A struct of status codes and outputs from the command.
 *
 * @note A command should not be considered complete until this callback is
 * called, and arguments the command needs MUST stay in scope until such happens.
 */
typedef void (* CommandCallback_t )( void * pCmdCallbackContext,
                                     MQTTAgentReturnInfo_t * pReturnInfo );

/**
 * @brief Callback function called when receiving a publish.
 *
 * @param[in] pMqttAgentContext The context of the MQTT agent.
 * @param[in] packetId The packet ID of the received publish.
 * @param[in] pPublishInfo Deserialized publish information.
 */
typedef void (* IncomingPublishCallback_t )( MQTTAgentContext_t * pMqttAgentContext,
                                             uint16_t packetId,
                                             MQTTPublishInfo_t * pPublishInfo );

/**
 * @brief Information used by each MQTT agent. A context will be initialized by
 * MQTTAgent_Init(), and every API function will accept a pointer to the
 * initalized struct.
 */
struct MQTTAgentContext
{
    MQTTContext_t mqttContext;
    AgentMessageContext_t * pMessageCtx;
    AckInfo_t pPendingAcks[ MQTT_AGENT_MAX_OUTSTANDING_ACKS ];
    IncomingPublishCallback_t pIncomingCallback;
    void * pIncomingCallbackContext;
    bool packetReceivedInLoop;
};

/**
 * @brief Struct holding arguments for a SUBSCRIBE or UNSUBSCRIBE call.
 */
typedef struct MQTTAgentSubscribeArgs
{
    MQTTSubscribeInfo_t * pSubscribeInfo;
    size_t numSubscriptions;
} MQTTAgentSubscribeArgs_t;

/**
 * @brief Struct holding arguments for a CONNECT call.
 */
typedef struct MQTTAgentConnectArgs
{
    MQTTConnectInfo_t * pConnectInfo;
    MQTTPublishInfo_t * pWillInfo;
    uint32_t timeoutMs;
    bool sessionPresent;
} MQTTAgentConnectArgs_t;

/**
 * @brief Struct holding arguments that are common to every command.
 */
typedef struct CommandInfo
{
    CommandCallback_t cmdCompleteCallback;
    CommandContext_t * pCmdCompleteCallbackContext;
    uint32_t blockTimeMs;
} CommandInfo_t;

/**
 * @brief The commands sent from the publish API to the MQTT agent.
 *
 * @note The structure used to pass information from the public facing API into the
 * agent task. */
struct Command
{
    CommandType_t commandType;
    void * pArgs;
    CommandCallback_t pCommandCompleteCallback;
    CommandContext_t * pCmdContext;
};

/*-----------------------------------------------------------*/

/**
 * @brief Perform any initialisation the MQTT agent requires before it can
 * be used. Must be called before any other function.
 *
 * @param[in] pMqttAgentContext Pointer to struct to initialize.
 * @param[in] pMsgCtx Message context handle to use in the command loop.
 * @param[in] pNetworkBuffer Pointer to network buffer to use.
 * @param[in] pTransportInterface Transport interface to use with the MQTT
 * library.  See https://www.freertos.org/network-interface.html
 * @param[in] getCurrentTimeMs Pointer to a function that returns a count value
 * that increments every millisecond.
 * @param[in] incomingCallback The callback to execute when receiving publishes.
 * @param[in] pIncomingPacketContext A pointer to a context structure defined by
 * the application writer.
 *
 * @return Appropriate status code from MQTT_Init().
 */
MQTTStatus_t MQTTAgent_Init( MQTTAgentContext_t * pMqttAgentContext,
                             AgentMessageContext_t * pMsgCtx,
                             MQTTFixedBuffer_t * pNetworkBuffer,
                             TransportInterface_t * pTransportInterface,
                             MQTTGetCurrentTimeFunc_t getCurrentTimeMs,
                             IncomingPublishCallback_t incomingCallback,
                             void * pIncomingPacketContext );

/**
 * @brief Process commands from the command queue in a loop.
 *
 * @param[in] pMqttAgentContext The MQTT agent to use.
 *
 * @return appropriate error code, or `MQTTSuccess` from a successful disconnect
 * or termination.
 */
MQTTStatus_t MQTTAgent_CommandLoop( MQTTAgentContext_t * pMqttAgentContext );

/**
 * @brief Resume a session by resending publishes if a session is present in
 * the broker, or clear state information if not.
 *
 * @param[in] pMqttAgentContext The MQTT agent to use.
 * @param[in] sessionPresent The session present flag from the broker.
 *
 * @return `MQTTSuccess` if it succeeds in resending publishes, else an
 * appropriate error code from `MQTT_Publish()`
 */
MQTTStatus_t MQTTAgent_ResumeSession( MQTTAgentContext_t * pMqttAgentContext,
                                      bool sessionPresent );

/**
 * @brief Add a command to call MQTT_Subscribe() for an MQTT connection.
 *
 * @param[in] pMqttAgentContext The MQTT agent to use.
 * @param[in] pSubscriptionInfo Struct describing topic to subscribe to.
 * @param[in] pCommandInfo The information pertaining to the command, including:
 *  - cmdCompleteCallback Optional callback to invoke when the command completes.
 *  - pCmdCompleteCallbackContext Optional completion callback context.
 *  - blockTimeMs The maximum amount of time in milliseconds to wait for the
 *    command to be posted to the MQTT agent, should the agent's event queue
 *    be full. Tasks wait in the Blocked state so don't use any CPU time.
 *
 * @return `MQTTSuccess` if the command was posted to the MQTT agents event queue.
 * Otherwise an enumerated error code.
 */
MQTTStatus_t MQTTAgent_Subscribe( MQTTAgentContext_t * pMqttAgentContext,
                                  MQTTAgentSubscribeArgs_t * pSubscriptionArgs,
                                  CommandInfo_t * pCommandInfo );

/**
 * @brief Add a command to call MQTT_Unsubscribe() for an MQTT connection.
 *
 * @param[in] pMqttAgentContext The MQTT agent to use.
 * @param[in] pSubscriptionList List of topics to unsubscribe from.
 * @param[in] pCommandInfo The information pertaining to the command, including:
 *  - cmdCompleteCallback Optional callback to invoke when the command completes.
 *  - pCmdCompleteCallbackContext Optional completion callback context.
 *  - blockTimeMs The maximum amount of time in milliseconds to wait for the
 *    command to be posted to the MQTT agent, should the agent's event queue
 *    be full. Tasks wait in the Blocked state so don't use any CPU time.
 *
 * @return `MQTTSuccess` if the command was posted to the MQTT agents event queue.
 * Otherwise an enumerated error code.
 */
MQTTStatus_t MQTTAgent_Unsubscribe( MQTTAgentContext_t * pMqttAgentContext,
                                    MQTTAgentSubscribeArgs_t * pSubscriptionArgs,
                                    CommandInfo_t * pCommandInfo );

/**
 * @brief Add a command to call MQTT_Publish() for an MQTT connection.
 *
 * @param[in] pMqttAgentContext The MQTT agent to use.
 * @param[in] pPublishInfo MQTT PUBLISH information.
 * @param[in] pCommandInfo The information pertaining to the command, including:
 *  - cmdCompleteCallback Optional callback to invoke when the command completes.
 *  - pCmdCompleteCallbackContext Optional completion callback context.
 *  - blockTimeMs The maximum amount of time in milliseconds to wait for the
 *    command to be posted to the MQTT agent, should the agent's event queue
 *    be full. Tasks wait in the Blocked state so don't use any CPU time.
 *
 * @return `MQTTSuccess` if the command was posted to the MQTT agents event queue.
 * Otherwise an enumerated error code.
 */
MQTTStatus_t MQTTAgent_Publish( MQTTAgentContext_t * pMqttAgentContext,
                                MQTTPublishInfo_t * pPublishInfo,
                                CommandInfo_t * pCommandInfo );

/**
 * @brief Send a message to the MQTT agent purely to trigger an iteration of its loop,
 * which will result in a call to MQTT_ProcessLoop().  This function can be used to
 * wake the MQTT agent task when it is known data may be available on the connected
 * socket.
 *
 * @param[in] pMqttAgentContext The MQTT agent to use.
 * @param[in] blockTimeMs The maximum amount of time in milliseconds to wait for the
 * command to be posted to the MQTT agent, should the agent's event queue be
 * full.  Tasks wait in the Blocked state so don't use any CPU time.
 *
 * @return `MQTTSuccess` if the command was posted to the MQTT agents event queue.
 * Otherwise an enumerated error code.
 */
MQTTStatus_t MQTTAgent_TriggerProcessLoop( MQTTAgentContext_t * pMqttAgentContext,
                                           uint32_t blockTimeMs );

/**
 * @brief Add a command to call MQTT_Ping() for an MQTT connection.
 *
 * @param[in] pMqttAgentContext The MQTT agent to use.
 * @param[in] pCommandInfo The information pertaining to the command, including:
 *  - cmdCompleteCallback Optional callback to invoke when the command completes.
 *  - pCmdCompleteCallbackContext Optional completion callback context.
 *  - blockTimeMs The maximum amount of time in milliseconds to wait for the
 *    command to be posted to the MQTT agent, should the agent's event queue
 *    be full. Tasks wait in the Blocked state so don't use any CPU time.
 *
 * @return `MQTTSuccess` if the command was posted to the MQTT agents event queue.
 * Otherwise an enumerated error code.
 */
MQTTStatus_t MQTTAgent_Ping( MQTTAgentContext_t * pMqttAgentContext,
                             CommandInfo_t * pCommandInfo );

/**
 * @brief Add a command to call MQTT_Connect() for an MQTT connection. If a session
 * is resumed with the broker, it will also resend the necessary QoS1/2 publishes.
 *
 * @param[in] pMqttAgentContext The MQTT agent to use.
 * @param[in] pConnectArgs Struct holding args for MQTT_Connect().
 * @param[in] pCommandInfo The information pertaining to the command, including:
 *  - cmdCompleteCallback Optional callback to invoke when the command completes.
 *  - pCmdCompleteCallbackContext Optional completion callback context.
 *  - blockTimeMs The maximum amount of time in milliseconds to wait for the
 *    command to be posted to the MQTT agent, should the agent's event queue
 *    be full. Tasks wait in the Blocked state so don't use any CPU time.
 *
 * @return `MQTTSuccess` if the command was posted to the MQTT agents event queue.
 * Otherwise an enumerated error code.
 */
MQTTStatus_t MQTTAgent_Connect( MQTTAgentContext_t * pMqttAgentContext,
                                MQTTAgentConnectArgs_t * pConnectArgs,
                                CommandInfo_t * pCommandInfo );

/**
 * @brief Add a command to disconnect an MQTT connection.
 *
 * @param[in] pMqttAgentContext The MQTT agent to use.
 * @param[in] pCommandInfo The information pertaining to the command, including:
 *  - cmdCompleteCallback Optional callback to invoke when the command completes.
 *  - pCmdCompleteCallbackContext Optional completion callback context.
 *  - blockTimeMs The maximum amount of time in milliseconds to wait for the
 *    command to be posted to the MQTT agent, should the agent's event queue
 *    be full. Tasks wait in the Blocked state so don't use any CPU time.
 *
 * @return `MQTTSuccess` if the command was posted to the MQTT agents event queue.
 * Otherwise an enumerated error code.
 */
MQTTStatus_t MQTTAgent_Disconnect( MQTTAgentContext_t * pMqttAgentContext,
                                   CommandInfo_t * pCommandInfo );

/**
 * @brief Add a termination command to the command queue.
 *
 * @param[in] pMqttAgentContext The MQTT agent to use.
 * @param[in] pCommandInfo The information pertaining to the command, including:
 *  - cmdCompleteCallback Optional callback to invoke when the command completes.
 *  - pCmdCompleteCallbackContext Optional completion callback context.
 *  - blockTimeMs The maximum amount of time in milliseconds to wait for the
 *    command to be posted to the MQTT agent, should the agent's event queue
 *    be full. Tasks wait in the Blocked state so don't use any CPU time.
 *
 * @return `MQTTSuccess` if the command was posted to the MQTT agents event queue.
 * Otherwise an enumerated error code.
 */
MQTTStatus_t MQTTAgent_Terminate( MQTTAgentContext_t * pMqttAgentContext,
                                  CommandInfo_t * pCommandInfo );

#endif /* MQTT_AGENT_H */
