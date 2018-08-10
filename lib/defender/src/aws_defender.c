/*
 * Amazon FreeRTOS Device Defender Agent V1.0.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#include "aws_cbor.h"
#include "aws_cbor_alloc.h"
#include "aws_cbor_print.h"
#include "aws_defender_internals.h"

#include "FreeRTOS.h"
#include "aws_clientcredential.h"
#include "aws_mqtt_agent.h"
#include "task.h"

static TickType_t xDEFENDER_mqtt_timeout_period = pdMS_TO_TICKS( 10U * 1000U );
static int32_t xDEFENDER_delay_period_sec = 300;

static DefenderReportStatus_t DEFENDER_report_status;
static defender_state_t DEFENDER_state;

static void DEFENDER_AgentLoop();
static void DEFENDER_AgentSleep( int /*sleep_seconds*/ );

static TaskHandle_t pxDEFENDER_task_handle = NULL;
static bool xDEFENDER_kill;

static char const * const pcDEFENDER_mqtt_endpoint =
    clientcredentialMQTT_BROKER_ENDPOINT;

static defender_state_t (* DEFENDER_States[ eDEFENDER_STATE_COUNT ])( void ) = { 0 };

static defender_state_t DEFENDER_StateInit( void );
static defender_state_t DEFENDER_StateNewMQTT( void );
static defender_state_t DEFENDER_StateConnectMqtt( void );
static defender_state_t DEFENDER_StateDisconnectMqtt( void );
static defender_state_t DEFENDER_StateSubscribe( void );
static defender_state_t DEFENDER_StateCreateReport( void );
static defender_state_t DEFENDER_StateDeleteMqtt( void );
static defender_state_t DEFENDER_StateSleep( void );

defender_state_t DEFENDER_StateGet( void )
{
    return DEFENDER_state;
}

static void DEFENDER_StateFunctionSet( void )
{
    /* clang-format off */
    DEFENDER_States[ eDEFENDER_STATE_INIT ] = DEFENDER_StateInit;
    DEFENDER_States[ eDEFENDER_STATE_STARTED ] = DEFENDER_StateNewMQTT;
    DEFENDER_States[ eDEFENDER_STATE_NEW_MQTT_FAILED ] = DEFENDER_StateSleep;
    DEFENDER_States[ eDEFENDER_STATE_NEW_MQTT_SUCCESS ] = DEFENDER_StateConnectMqtt;
    DEFENDER_States[ eDEFENDER_STATE_CONNECT_MQTT_FAILED ] = DEFENDER_StateDeleteMqtt;
    DEFENDER_States[ eDEFENDER_STATE_CONNECT_MQTT_SUCCESS ] = DEFENDER_StateSubscribe;
    DEFENDER_States[ eDEFENDER_STATE_SUBSCRIBE_MQTT_FAILED ] = DEFENDER_StateDisconnectMqtt;
    DEFENDER_States[ eDEFENDER_STATE_SUBSCRIBE_MQTT_SUCCESS ] = DEFENDER_StateCreateReport;
    DEFENDER_States[ eDEFENDER_STATE_SUBMIT_REPORT_FAILED ] = DEFENDER_StateDisconnectMqtt;
    DEFENDER_States[ eDEFENDER_STATE_SUBMIT_REPORT_SUCCESS ] = DEFENDER_StateDisconnectMqtt;
    DEFENDER_States[ eDEFENDER_STATE_DISCONNECT_FAILED ] = DEFENDER_StateDisconnectMqtt;
    DEFENDER_States[ eDEFENDER_STATE_DISCONNECTED ] = DEFENDER_StateDeleteMqtt;
    DEFENDER_States[ eDEFENDER_STATE_DELETE_FAILED ] = DEFENDER_StateDeleteMqtt;
    DEFENDER_States[ eDEFENDER_STATE_SLEEP ] = DEFENDER_StateSleep;
    /* clang-format on */
}

static defender_state_t DEFENDER_StateInit( void )
{
    return eDEFENDER_STATE_STARTED;
}

static MQTTAgentHandle_t pxDEFENDER_mqtt_agent;

static defender_state_t DEFENDER_StateNewMQTT( void )
{
    MQTTAgentReturnCode_t create_result =
        MQTT_AGENT_Create( &pxDEFENDER_mqtt_agent );

    if( eMQTTAgentSuccess != create_result )
    {
        return eDEFENDER_STATE_NEW_MQTT_FAILED;
    }

    return eDEFENDER_STATE_NEW_MQTT_SUCCESS;
}

static defender_state_t DEFENDER_StateConnectMqtt( void )
{
    MQTTAgentConnectParams_t xDefender_con_params =
    {
        .pcURL              = pcDEFENDER_mqtt_endpoint,
        .xFlags             = mqttagentREQUIRE_TLS,
        .xURLIsIPAddress    = pdFALSE,
        .usPort             = clientcredentialMQTT_BROKER_PORT,
        .pucClientId        = ( const uint8_t * ) clientcredentialIOT_THING_NAME,
        .usClientIdLength   = strlen( clientcredentialIOT_THING_NAME ),
        .xSecuredConnection = pdTRUE,
        .pvUserData         = NULL,
        .pxCallback         = NULL,
        .pcCertificate      = NULL,
        .ulCertificateSize  = 0,
    };
    MQTTAgentReturnCode_t connect_result =
        MQTT_AGENT_Connect( pxDEFENDER_mqtt_agent, &xDefender_con_params,
                            xDEFENDER_mqtt_timeout_period );

    if( eMQTTAgentSuccess != connect_result )
    {
        return eDEFENDER_STATE_CONNECT_MQTT_FAILED;
    }

    return eDEFENDER_STATE_CONNECT_MQTT_SUCCESS;
}

static bool SubscribeToAcceptCbor( void );
static bool SubscribeToRejectCbor( void );

static defender_state_t DEFENDER_StateSubscribe( void )
{
    bool xErr = false;

    xErr |= SubscribeToAcceptCbor();
    xErr |= SubscribeToRejectCbor();

    if( xErr )
    {
        return eDEFENDER_STATE_SUBSCRIBE_MQTT_FAILED;
    }

    return eDEFENDER_STATE_SUBSCRIBE_MQTT_SUCCESS;
}

static MQTTBool_t accept_callback( void * pxPvPublishCallbackContext,
                                   MQTTPublishData_t const * pxPublishData );

static bool SubscribeToAcceptCbor( void )
{
    uint8_t * pucTopic = "$aws/things/" clientcredentialIOT_THING_NAME
                         "/defender/metrics/cbor/accepted";
    MQTTAgentSubscribeParams_t xSub_params =
    {
        .pucTopic                 = pucTopic,
        .usTopicLength            = strlen( pucTopic ),
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = accept_callback,
    };
    MQTTAgentReturnCode_t sub_result = MQTT_AGENT_Subscribe(
        pxDEFENDER_mqtt_agent, &xSub_params, xDEFENDER_mqtt_timeout_period );

    bool xErr = eMQTTAgentSuccess != sub_result;

    return xErr;
}

static MQTTBool_t accept_callback( void * pxPvPublishCallbackContext,
                                   MQTTPublishData_t const * pxPublishData )
{
    DEFENDER_report_status = eDefenderRepSuccess;

    return eMQTTFalse;
}

static MQTTBool_t reject_callback( void * pxPvPublishCallbackContext,
                                   MQTTPublishData_t const * pxPublishData );

static bool SubscribeToRejectCbor( void )
{
    uint8_t * pucTopic = "$aws/things/" clientcredentialIOT_THING_NAME
                         "/defender/metrics/cbor/rejected";
    MQTTAgentSubscribeParams_t xSub_params =
    {
        .pucTopic                 = pucTopic,
        .usTopicLength            = strlen( pucTopic ),
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = reject_callback,
    };
    MQTTAgentReturnCode_t sub_result = MQTT_AGENT_Subscribe(
        pxDEFENDER_mqtt_agent, &xSub_params, xDEFENDER_mqtt_timeout_period );

    bool xErr = eMQTTAgentSuccess != sub_result;

    return xErr;
}

static MQTTBool_t reject_callback( void * pxPvPublishCallbackContext,
                                   MQTTPublishData_t const * pxPublishData )
{
    DEFENDER_report_status = eDefenderRepRejected;

    return eMQTTFalse;
}

static bool PublishCborToDevDef( cbor_handle_t );

static defender_state_t DEFENDER_StateCreateReport( void )
{
    cbor_handle_t pxReport;

    pxReport = DEFENDER_CreateReport();

    if( NULL == pxReport )
    {
        return eDEFENDER_STATE_SUBMIT_REPORT_FAILED;
    }

    bool xErr = PublishCborToDevDef( pxReport );
    CBOR_Delete( &pxReport );

    /* Wait for ack from service */
    vTaskDelay( pdMS_TO_TICKS( 10000 ) );

    if( true == xErr )
    {
        return eDEFENDER_STATE_SUBMIT_REPORT_FAILED;
    }

    return eDEFENDER_STATE_SUBMIT_REPORT_SUCCESS;
}

static bool PublishCborToDevDef( cbor_handle_t pxReport )
{
    uint8_t * pucTopic =
        "$aws/things/" clientcredentialIOT_THING_NAME "/defender/metrics/cbor";
    uint8_t const * pucBuffer = CBOR_GetRawBuffer( pxReport );
    int xBuf_len = CBOR_GetBufferSize( pxReport );

    MQTTAgentPublishParams_t xPub_rec_params =
    {
        .pucTopic      = pucTopic,
        .usTopicLength = strlen( pucTopic ),
        .xQoS          = eMQTTQoS0,
        .pvData        = pucBuffer,
        .ulDataLength  = xBuf_len,
    };
    MQTTAgentReturnCode_t publish_result = MQTT_AGENT_Publish(
        pxDEFENDER_mqtt_agent, &xPub_rec_params, xDEFENDER_mqtt_timeout_period );

    if( eMQTTAgentSuccess != publish_result )
    {
        DEFENDER_report_status = eDefenderRepNotSent;

        return true;
    }

    DEFENDER_report_status = eDefenderRepNoAck;

    return false;
}

static defender_state_t DEFENDER_StateDisconnectMqtt( void )
{
    if( eMQTTAgentSuccess
        != MQTT_AGENT_Disconnect(
            pxDEFENDER_mqtt_agent, xDEFENDER_mqtt_timeout_period ) )
    {
        return eDEFENDER_STATE_DISCONNECT_FAILED;
    }

    return eDEFENDER_STATE_DISCONNECTED;
}

static defender_state_t DEFENDER_StateDeleteMqtt( void )
{
    if( eMQTTAgentSuccess != MQTT_AGENT_Delete( pxDEFENDER_mqtt_agent ) )
    {
        return eDEFENDER_STATE_DELETE_FAILED;
    }

    return eDEFENDER_STATE_SLEEP;
}

static defender_state_t DEFENDER_StateSleep( void )
{
    static TickType_t xWake_tick;
    TickType_t const current_tick = xTaskGetTickCount();
    TickType_t const active_period = ( current_tick - xWake_tick );
    int const xPeriod_ms = ( xDEFENDER_delay_period_sec * 1000 );
    TickType_t const period_ticks = pdMS_TO_TICKS( xPeriod_ms );
    TickType_t sleep_period = ( period_ticks - active_period );

    if( sleep_period > xDEFENDER_delay_period_sec )
    {
        sleep_period = 1;
    }

    vTaskDelay( sleep_period );

    xWake_tick = xTaskGetTickCount();

    return eDEFENDER_STATE_STARTED;
}

static defender_state_t DEFENDER_StateFunction( defender_state_t current_state )
{
    if( current_state == eDEFENDER_STATE_INIT )
    {
        DEFENDER_StateFunctionSet();
    }

    defender_state_t next_state = DEFENDER_States[ current_state ]();

    return next_state;
}

DefenderErr_t DEFENDER_ReportPeriodSet( int32_t xPeriod_sec )
{
    xDEFENDER_delay_period_sec = xPeriod_sec;

    return eDefenderErrSuccess;
}

DefenderReportStatus_t DEFENDER_ReportStatusGet( void )
{
    DefenderReportStatus_t report_status;

    do
    {
        report_status = DEFENDER_report_status;
    } while( report_status != DEFENDER_report_status );

    return report_status;
}

DefenderErr_t DEFENDER_Start( void )
{
    if( NULL != pxDEFENDER_task_handle )
    {
        return eDefenderErrAlreadyStarted;
    }

    xDEFENDER_kill = false;

    /* Returns pdTrue (1) on success. */
    BaseType_t success =
        xTaskCreate( DEFENDER_AgentLoop, "DD_Agent", configMINIMAL_STACK_SIZE,
                     NULL, tskIDLE_PRIORITY, &pxDEFENDER_task_handle );

    if( 1 != success )
    {
        return eDefenderErrFailedToCreateTask;
    }

    return eDefenderErrSuccess;
}

DefenderErr_t DEFENDER_Stop( void )
{
    if( NULL == pxDEFENDER_task_handle )
    {
        return eDefenderErrNotStarted;
    }

    xDEFENDER_kill = true;

    return eDefenderErrSuccess;
}

static void DEFENDER_AgentLoop()
{
    for( ; ; )
    {
        /* Check if a kill was requested before the report was started */
        if( xDEFENDER_kill )
        {
            break;
            /* TODO: Appropriately clean up resources on kill request */
        }

        DEFENDER_state = DEFENDER_StateFunction( DEFENDER_state );

        int xState_period_ms = 5;
        vTaskDelay( pdMS_TO_TICKS( xState_period_ms ) );
    }

    TaskHandle_t pxTask_handle = pxDEFENDER_task_handle;
    pxDEFENDER_task_handle = NULL;
    vTaskDelete( pxTask_handle );
}
