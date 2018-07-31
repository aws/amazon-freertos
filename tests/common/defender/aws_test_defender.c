#include "FreeRTOS.h"

#include "projdefs.h"

#include "aws_cbor.h"
#include "aws_cbor_types.h"
#include "aws_cbor_print.h"
#include "aws_defender_internals.h"
#include "aws_clientcredential.h"
#include "aws_mqtt_agent.h"
#include "unity_fixture.h"
#include <assert.h>

#define open_extra_socket ( 1 )

static bool report_accepted;
static bool report_rejected;

TEST_GROUP( Full_DEFENDER );

TEST_SETUP( Full_DEFENDER )
{
    /* Delay to allow the MQTT logs to flush */
    vTaskDelay( pdMS_TO_TICKS( 500 ) );
    report_accepted = false;
    report_rejected = false;
}

TEST_TEAR_DOWN( Full_DEFENDER )
{
    /* Delay to allow the MQTT logs to flush */
    ( void ) DEFENDER_Stop();
    vTaskDelay( pdMS_TO_TICKS( 500 ) );
}

TEST_GROUP_RUNNER( Full_DEFENDER )
{
    /* These test should not require any network connectivity */
    RUN_TEST_CASE( Full_DEFENDER, Start_should_return_err_if_already_started );
    RUN_TEST_CASE( Full_DEFENDER, Start_should_return_success );
    RUN_TEST_CASE( Full_DEFENDER, Stop_should_return_success_when_started );
    RUN_TEST_CASE( Full_DEFENDER, Stop_should_return_err_when_not_started );

    /* These tests check the connectivity and responses from the service */
    RUN_TEST_CASE( Full_DEFENDER, report_to_echo_server );
    RUN_TEST_CASE( Full_DEFENDER, endpoint_accepts_cbor_example_report );
    RUN_TEST_CASE( Full_DEFENDER, endpoint_accepts_json_example_report );

    /* This tests that the agent successfully reports to the service */
    RUN_TEST_CASE( Full_DEFENDER, agent_happy_states );
    RUN_TEST_CASE( Full_DEFENDER, endpoint_accepts_report_from_agent );
}

/*----------------------------------------------------------------------------*/

TEST( Full_DEFENDER, Start_should_return_err_if_already_started )
{
    ( void ) DEFENDER_Start();
    DefenderErr_t err = DEFENDER_Start();
    TEST_ASSERT_EQUAL( eDefenderErrAlreadyStarted, err );
}

TEST( Full_DEFENDER, Start_should_return_success )
{
    DefenderErr_t err = DEFENDER_Start();
    TEST_ASSERT_EQUAL( eDefenderErrSuccess, err );
}

TEST( Full_DEFENDER, Stop_should_return_success_when_started )
{
    ( void ) DEFENDER_Start();
    DefenderErr_t err = DEFENDER_Stop();
    TEST_ASSERT_EQUAL( eDefenderErrSuccess, err );
}

TEST( Full_DEFENDER, Stop_should_return_err_when_not_started )
{
    DefenderErr_t err = DEFENDER_Stop();
    TEST_ASSERT_EQUAL( eDefenderErrNotStarted, err );
}

/*----------------------------------------------------------------------------*/

static bool echo_triggered;

static cbor_handle_t     CreateDummyReport( void );
static MQTTAgentHandle_t MqttAgentNew( void );
static void              MqttAgentConnectToEcho( MQTTAgentHandle_t );
static void              SubscribeToEcho( MQTTAgentHandle_t );
static MQTTBool_t        echo_callback( void * pvPublishCallbackContext,
                                        MQTTPublishData_t const * pxPublishData );
static void ReportPublishToEcho( MQTTAgentHandle_t, cbor_handle_t );

TEST( Full_DEFENDER, report_to_echo_server )
{
    cbor_handle_t     defender_report     = CreateDummyReport();
    MQTTAgentHandle_t defender_mqtt_agent = MqttAgentNew();
    MqttAgentConnectToEcho( defender_mqtt_agent );
    SubscribeToEcho( defender_mqtt_agent );
    ReportPublishToEcho( defender_mqtt_agent, defender_report );

    /* Wait for 1 second while we  */
    vTaskDelay( pdMS_TO_TICKS( 10000 ) );
    TEST_ASSERT_TRUE_MESSAGE( echo_triggered,
                              "Expected the echo to trigger the callback" )

    TickType_t const timeout = pdMS_TO_TICKS( 10000 );
    ( void ) MQTT_AGENT_Disconnect( defender_mqtt_agent, timeout );
    ( void ) MQTT_AGENT_Delete( defender_mqtt_agent );
    CBOR_Delete( &defender_report );
}

static cbor_handle_t CreateDummyReport( void )
{
    /* Create example report */
    cbor_handle_t defender_report = CBOR_New( 0 );
    TEST_ASSERT_NOT_NULL_MESSAGE(defender_report, "Failed to create new report");
    cbor_handle_t header          = CBOR_New( 0 );
    if(NULL == header){
        CBOR_Delete(&defender_report);
        TEST_FAIL_MESSAGE("Failed to create new header");
    }
    else
    {
        CBOR_AppendKeyWithInt( header, "report_id", 13 );
        CBOR_AppendKeyWithString( header, "version", "1.0" );
        CBOR_AppendKeyWithMap( defender_report, "header", header );
    }
    CBOR_Delete( &header );

    cbor_handle_t metrics = CBOR_New( 0 );
    {
        cbor_handle_t tcp_ports = CBOR_New( 0 );
        {
            CBOR_AppendKeyWithInt( tcp_ports, "total", 0 );
            CBOR_AppendKeyWithMap( metrics, "listening_tcp_ports", tcp_ports );
        }
        CBOR_Delete( &tcp_ports );

        cbor_handle_t udp_ports = CBOR_New( 0 );
        {
            CBOR_AppendKeyWithInt( udp_ports, "total", 0 );
            CBOR_AppendKeyWithMap( metrics, "listening_udp_ports", udp_ports );
        }
        CBOR_Delete( &udp_ports );

        cbor_handle_t tcp_connections = CBOR_New( 0 );
        {
            cbor_handle_t est_connections = CBOR_New( 0 );
            {
                CBOR_AppendKeyWithInt( est_connections, "total", 2 );
                CBOR_AppendKeyWithMap( tcp_connections,
                                       "established_connections",
                                       est_connections );
            }
            CBOR_AppendKeyWithMap(
                metrics, "tcp_connections", tcp_connections );
        }
        CBOR_Delete( &tcp_connections );

        CBOR_AppendKeyWithMap( defender_report, "metrics", metrics );
    }
    CBOR_Delete( &metrics );
    cbor_err_t err = CBOR_CheckError( defender_report );
    if( err )
    {
        CBOR_Delete( &defender_report );
        TEST_ASSERT_EQUAL_MESSAGE(
            eCBOR_ERR_NO_ERROR, err, "Failed to create report" );
    }
    return defender_report;
}

static MQTTAgentHandle_t MqttAgentNew( void )
{
    MQTTAgentHandle_t     new_mqtt_agent = 0;
    MQTTAgentReturnCode_t create_result  = MQTT_AGENT_Create( &new_mqtt_agent );
    TEST_ASSERT_EQUAL_MESSAGE(
        eMQTTAgentSuccess, create_result, "Failed to create agent" );
    return new_mqtt_agent;
}

static void MqttAgentConnectToEcho( MQTTAgentHandle_t mqtt_agent )
{
    MQTTAgentConnectParams_t defender_con_params = {
        .pcURL            = clientcredentialMQTT_BROKER_ENDPOINT,
        .xFlags           = mqttagentREQUIRE_TLS,
        .xURLIsIPAddress  = pdFALSE,
        .usPort           = clientcredentialMQTT_BROKER_PORT,
        .pucClientId      = ( const uint8_t * ) clientcredentialIOT_THING_NAME,
        .usClientIdLength = strlen( clientcredentialIOT_THING_NAME ),
        .xSecuredConnection = pdTRUE,
        .pvUserData         = NULL,
        .pxCallback         = NULL,
        .pcCertificate      = NULL,
        .ulCertificateSize  = 0,
    };
    TickType_t const      timeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t connect_result =
        MQTT_AGENT_Connect( mqtt_agent, &defender_con_params, timeout );
    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == connect_result,
                         "Failed to connect agent to echo broker" );
}

static void SubscribeToEcho( MQTTAgentHandle_t mqtt_agent )
{
    uint8_t *                  topic      = "freertos/tests/echo";
    MQTTAgentSubscribeParams_t sub_params = {
        .pucTopic                 = topic,
        .usTopicLength            = strlen( topic ),
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = echo_callback,
    };
    TickType_t const      timeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t sub_result =
        MQTT_AGENT_Subscribe( mqtt_agent, &sub_params, timeout );
    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == sub_result,
                         "Failed to subscribe to topic" );
}

static MQTTBool_t echo_callback( void * pvPublishCallbackContext,
                                 MQTTPublishData_t const * pxPublishData )
{
    echo_triggered = true;
    return eMQTTFalse;
}

static void ReportPublishToEcho( MQTTAgentHandle_t mqtt_agent,
                                 cbor_handle_t     report )
{
    uint8_t * topic = "freertos/tests/echo";

    MQTTAgentPublishParams_t pub_rec_params = {
        .pucTopic      = topic,
        .usTopicLength = ( uint16_t ) strlen( topic ),
        .xQoS          = eMQTTQoS0,
        .pvData        = report->buffer_start,
        .ulDataLength  = report->map_end - report->buffer_start,
    };
    TickType_t const      timeout = pdMS_TO_TICKS( 1000 );
    MQTTAgentReturnCode_t publish_result =
        MQTT_AGENT_Publish( mqtt_agent, &pub_rec_params, timeout );
    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == publish_result,
                         "Failed to publish to topic" );
}

/*----------------------------------------------------------------------------*/

static char const * const defender_test_endpoint =
    clientcredentialMQTT_BROKER_ENDPOINT;
// "data.gamma.us-east-1.iot.amazonaws.com";

static MQTTBool_t
test_reject_callback( void *                    pvPublishCallbackContext,
                      MQTTPublishData_t const * pxPublishData );

static MQTTBool_t
test_accept_callback( void *                    pvPublishCallbackContext,
                      MQTTPublishData_t const * pxPublishData );

static void MqttAgentConnectToDevDef( MQTTAgentHandle_t );
static void SubscribeToAcceptCbor( MQTTAgentHandle_t );
static void SubscribeToRejectCbor( MQTTAgentHandle_t );
static void PublishCborToDevDef( MQTTAgentHandle_t, cbor_handle_t );

TEST( Full_DEFENDER, endpoint_accepts_cbor_example_report )
{
    cbor_handle_t     defender_report     = CreateDummyReport();
    MQTTAgentHandle_t defender_mqtt_agent = MqttAgentNew();
    MqttAgentConnectToDevDef( defender_mqtt_agent );
    SubscribeToAcceptCbor( defender_mqtt_agent );
    SubscribeToRejectCbor( defender_mqtt_agent );
    PublishCborToDevDef( defender_mqtt_agent, defender_report );

    /* Wait for 1 second for the response to come back from the service */
    TickType_t const timeout = pdMS_TO_TICKS( 10000 );
    vTaskDelay( pdMS_TO_TICKS( timeout ) );

    /* Clean up resources before checking test assertions */
    ( void ) MQTT_AGENT_Disconnect( defender_mqtt_agent, timeout );
    ( void ) MQTT_AGENT_Delete( defender_mqtt_agent );
    CBOR_Delete( &defender_report );

    TEST_ASSERT_FALSE_MESSAGE( report_rejected, "Metrics report was rejected." )
    TEST_ASSERT_TRUE_MESSAGE( report_accepted,
                              "Expected the report to be accepted." )
}

static void MqttAgentConnectToDevDef( MQTTAgentHandle_t mqtt_agent )
{
    MQTTAgentConnectParams_t defender_con_params = {
        .pcURL            = defender_test_endpoint,
        .xFlags           = mqttagentREQUIRE_TLS,
        .xURLIsIPAddress  = pdFALSE,
        .usPort           = clientcredentialMQTT_BROKER_PORT,
        .pucClientId      = ( const uint8_t * ) clientcredentialIOT_THING_NAME,
        .usClientIdLength = strlen( clientcredentialIOT_THING_NAME ),
        .xSecuredConnection = pdTRUE,
        .pvUserData         = NULL,
        .pxCallback         = NULL,
        .pcCertificate      = NULL,
        .ulCertificateSize  = 0,
    };
    TickType_t const      timeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t connect_result =
        MQTT_AGENT_Connect( mqtt_agent, &defender_con_params, timeout );
    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == connect_result,
                         "Failed to connect agent to broker" );
}

static void SubscribeToAcceptCbor( MQTTAgentHandle_t mqtt_agent )
{
    uint8_t * topic = "$aws/things/" clientcredentialIOT_THING_NAME
                      "/defender/metrics/cbor/accepted";
    MQTTAgentSubscribeParams_t sub_params = {
        .pucTopic                 = topic,
        .usTopicLength            = strlen( topic ),
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = test_accept_callback,
    };
    TickType_t const      timeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t sub_result =
        MQTT_AGENT_Subscribe( mqtt_agent, &sub_params, timeout );
    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == sub_result,
                         "Failed to subscribe to topic" );
}

static MQTTBool_t
test_accept_callback( void *                    pvPublishCallbackContext,
                      MQTTPublishData_t const * pxPublishData )
{
    report_accepted = true;
    return eMQTTFalse;
}

static void SubscribeToRejectCbor( MQTTAgentHandle_t mqtt_agent )
{
    uint8_t * topic = "$aws/things/" clientcredentialIOT_THING_NAME
                      "/defender/metrics/cbor/rejected";
    MQTTAgentSubscribeParams_t sub_params = {
        .pucTopic                 = topic,
        .usTopicLength            = strlen( topic ),
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = test_reject_callback,
    };
    TickType_t const      timeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t sub_result =
        MQTT_AGENT_Subscribe( mqtt_agent, &sub_params, timeout );
    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == sub_result,
                         "Failed to subscribe to topic" );
}

static MQTTBool_t
test_reject_callback( void *                    pvPublishCallbackContext,
                      MQTTPublishData_t const * pxPublishData )
{
    report_rejected = true;
    return eMQTTFalse;
}

static void PublishCborToDevDef( MQTTAgentHandle_t mqtt_agent,
                                 cbor_handle_t     report )
{
    uint8_t * topic =
        "$aws/things/" clientcredentialIOT_THING_NAME "/defender/metrics/cbor";
    uint8_t const * buffer  = report->buffer_start;
    int             buf_len = report->map_end - report->buffer_start + 1;

    MQTTAgentPublishParams_t pub_rec_params = {
        .pucTopic      = topic,
        .usTopicLength = ( uint16_t ) strlen( topic ),
        .xQoS          = eMQTTQoS0,
        .pvData        = buffer,
        .ulDataLength  = buf_len,
    };
    TickType_t const timeout = pdMS_TO_TICKS( 10000 );

    MQTTAgentReturnCode_t publish_result =
        MQTT_AGENT_Publish( mqtt_agent, &pub_rec_params, timeout );
    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == publish_result,
                         "Failed to publish to topic" );
}

/*----------------------------------------------------------------------------*/

static void SubscribeToAcceptJson( MQTTAgentHandle_t );
static void SubscribeToRejectJson( MQTTAgentHandle_t );
static void PublishJsonToDevDef( MQTTAgentHandle_t, char const * );

TEST( Full_DEFENDER, endpoint_accepts_json_example_report )
{
    cbor_handle_t      defender_report = CreateDummyReport();
    char const * const json_report     = CBOR_AsString( defender_report );
    CBOR_Delete( &defender_report );

    MQTTAgentHandle_t defender_mqtt_agent = MqttAgentNew();
    MqttAgentConnectToDevDef( defender_mqtt_agent );
    SubscribeToAcceptJson( defender_mqtt_agent );
    SubscribeToRejectJson( defender_mqtt_agent );
    PublishJsonToDevDef( defender_mqtt_agent, json_report );
    vPortFree( json_report );

    /* Wait for 1 second whule we  */
    vTaskDelay( pdMS_TO_TICKS( 10000 ) );
    TickType_t const timeout = pdMS_TO_TICKS( 10000 );

    ( void ) MQTT_AGENT_Disconnect( defender_mqtt_agent, timeout );
    ( void ) MQTT_AGENT_Delete( defender_mqtt_agent );

    TEST_ASSERT_FALSE_MESSAGE( report_rejected,
                               "JSON Metrics report was rejected." )
    TEST_ASSERT_TRUE_MESSAGE( report_accepted,
                              "Expected the JSON report to be accepted." )
}

static void SubscribeToAcceptJson( MQTTAgentHandle_t mqtt_agent )
{
    uint8_t * topic = "$aws/things/" clientcredentialIOT_THING_NAME
                      "/defender/metrics/json/accepted";
    MQTTAgentSubscribeParams_t sub_params = {
        .pucTopic                 = topic,
        .usTopicLength            = strlen( topic ),
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = test_accept_callback,
    };
    TickType_t const      timeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t sub_result =
        MQTT_AGENT_Subscribe( mqtt_agent, &sub_params, timeout );
    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == sub_result,
                         "Failed to subscribe to topic" );
}

static void SubscribeToRejectJson( MQTTAgentHandle_t mqtt_agent )
{
    uint8_t * topic = "$aws/things/" clientcredentialIOT_THING_NAME
                      "/defender/metrics/json/rejected";
    MQTTAgentSubscribeParams_t sub_params = {
        .pucTopic                 = topic,
        .usTopicLength            = strlen( topic ),
        .xQoS                     = eMQTTQoS0,
        .pvPublishCallbackContext = NULL,
        .pxPublishCallback        = test_reject_callback,
    };
    TickType_t const      timeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t sub_result =
        MQTT_AGENT_Subscribe( mqtt_agent, &sub_params, timeout );
    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == sub_result,
                         "Failed to subscribe to topic" );
}

static void PublishJsonToDevDef( MQTTAgentHandle_t mqtt_agent,
                                 char const *      report )
{
    uint8_t * topic =
        "$aws/things/" clientcredentialIOT_THING_NAME "/defender/metrics/json";

    MQTTAgentPublishParams_t pub_rec_params = {
        .pucTopic      = topic,
        .usTopicLength = ( uint16_t ) strlen( topic ),
        .xQoS          = eMQTTQoS0,
        .pvData        = report,
        .ulDataLength  = strlen( report ),
    };
    TickType_t const      timeout = pdMS_TO_TICKS( 10000 );
    MQTTAgentReturnCode_t publish_result =
        MQTT_AGENT_Publish( mqtt_agent, &pub_rec_params, timeout );
    TEST_ASSERT_MESSAGE( eMQTTAgentSuccess == publish_result,
                         "Failed to publish JSON report to topic" );
}

/*----------------------------------------------------------------------------*/
#define DEFENDER_AssertStateAndWait( state )                                   \
    do                                                                         \
    {                                                                          \
        TEST_ASSERT_EQUAL_MESSAGE(                                             \
            state, DEFENDER_StateGet(), "Expected " #state );                  \
        while( state == DEFENDER_StateGet() );                                 \
    } while( 0 )

#define DEFENDER_AssertState( state )                                          \
    TEST_ASSERT_EQUAL_MESSAGE( state, DEFENDER_StateGet(), "Expected " #state )

TEST( Full_DEFENDER, agent_happy_states )
{
    ( void ) DEFENDER_MetricsInitFunc( NULL, 0 );

    int report_period = 5;
    ( void ) DEFENDER_ReportPeriodSet( report_period );

    DEFENDER_Start();
    while( eDEFENDER_STATE_INIT == DEFENDER_StateGet() );

    /* The following asserts the flow of the device defender agent's state
     * machine, assuming no errors occur */
    DEFENDER_AssertStateAndWait( eDEFENDER_STATE_STARTED );
    DEFENDER_AssertStateAndWait( eDEFENDER_STATE_NEW_MQTT_SUCCESS );
    DEFENDER_AssertStateAndWait( eDEFENDER_STATE_CONNECT_MQTT_SUCCESS );
    DEFENDER_AssertStateAndWait( eDEFENDER_STATE_SUBSCRIBE_MQTT_SUCCESS );
    DEFENDER_AssertStateAndWait( eDEFENDER_STATE_SUBMIT_REPORT_SUCCESS );
    DEFENDER_AssertStateAndWait( eDEFENDER_STATE_DISCONNECTED );
    DEFENDER_AssertStateAndWait( eDEFENDER_STATE_SLEEP );
    DEFENDER_AssertState( eDEFENDER_STATE_STARTED );

    DEFENDER_Stop();
}

TEST( Full_DEFENDER, endpoint_accepts_report_from_agent )
{
    DefenderMetric_t metrics_list[] = {
        pxDEFENDER_tcp_connections,
    };
    ( void ) DEFENDER_MetricsInit( metrics_list );

    int report_period_sec = 20;
    ( void ) DEFENDER_ReportPeriodSet( report_period_sec );

    DEFENDER_Start();
    while( eDEFENDER_STATE_SLEEP != DEFENDER_StateGet() );
    DEFENDER_Stop();

    DefenderReportStatus_t report_status = DEFENDER_ReportStatusGet();

    TEST_ASSERT_NOT_EQUAL_MESSAGE(
        eDefenderRepRejected, report_status, "Metrics report was rejected" );

    TEST_ASSERT_NOT_EQUAL_MESSAGE( eDefenderRepNoAck,
                                   report_status,
                                   "Metrics report was not acknowledge" );

    TEST_ASSERT_EQUAL( eDefenderRepSuccess, report_status );
}
