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

#ifndef CORE_MQTT_AGENT_CONFIG_H_
#define CORE_MQTT_AGENT_CONFIG_H_

#define MQTT_AGENT_COMMAND_QUEUE_LENGTH     ( 25 )
#define MQTT_COMMAND_CONTEXTS_POOL_SIZE     ( 10 )

/**
 * @brief The maximum number of subscriptions to track for a single connection.
 *
 * @note The MQTT agent keeps a record of all existing MQTT subscriptions.
 * MQTT_AGENT_MAX_SIMULTANEOUS_SUBSCRIPTIONS sets the maximum number of
 * subscriptions records that can be maintained at one time.  The higher this
 * number is the greater the agent's RAM consumption will be.
 */
#define MQTT_AGENT_MAX_SIMULTANEOUS_SUBSCRIPTIONS    ( 10 )
/**
 * @brief Size of statically allocated buffers for holding subscription filters.
 *
 * @note Subscription filters are strings such as "/my/topicname/#".  These
 * strings are limited to a maximum of MQTT_AGENT_MAX_SUBSCRIPTION_FILTER_LENGTH
 * characters. The higher this number is the greater the agent's RAM consumption
 * will be.
 */
#define MQTT_AGENT_MAX_SUBSCRIPTION_FILTER_LENGTH ( 100 )
/**
 * @brief Dimensions the buffer used to serialise and deserialise MQTT packets.
 * @note Specified in bytes.  Must be large enough to hold the maximum
 * anticipated MQTT payload.
 */
#define MQTT_AGENT_NETWORK_BUFFER_SIZE    ( 5000 )
/**
 * @breif Timeout for which MQTT library keeps polling the transport interface,
 * when no byte is received.
 * The timeout is honoured only after the first byte is read and while remaining bytes
 * are read from network interface. Keeping this timeout to a sufficiently large value so
 * as to account for delay of receipt of a large block of message.
 */
#define MQTT_RECV_POLLING_TIMEOUT_MS  ( 1000U )
 /**
  * @breif Maximum time MQTT agent waits in the queue for any pending MQTT operations.
  * The wait time is kept smallest possible to increase the responsiveness of MQTT agent
  * while processing  pending MQTT operations as well as receive packets from network.
  */
#define MQTT_AGENT_MAX_EVENT_QUEUE_WAIT_TIME ( 1U )

#endif /* ifndef CORE_MQTT_AGENT_CONFIG_H */
