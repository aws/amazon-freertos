/*
 * Amazon FreeRTOS Defender V2.0.1
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

#include <string.h>

/* Old and new defender includes. */
#include "aws_iot_defender.h"
#include "aws_defender.h"

#include "aws_clientcredential.h"

#include "platform/iot_network_freertos.h"

/* Defender agent's status, initialized with eDefenderRepInit. */
static DefenderReportStatus_t _status = eDefenderRepInit;

static IotNetworkServerInfo_t _AWS_IOT_SERVER_INFO = AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER;
static IotNetworkCredentials_t _AWS_IOT_CREDENTIALS = AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER;

/*-----------------------------------------------------------*/

void _defenderCallback( void * param1,
                        AwsIotDefenderCallbackInfo_t * const pCallbackInfo )
{
    ( void ) param1;

    switch( pCallbackInfo->eventType )
    {
        /* Metrics is accepted. */
        case AWS_IOT_DEFENDER_METRICS_ACCEPTED:
            _status = eDefenderRepSuccess;
            break;

        /* Metrics is rejected. */
        case AWS_IOT_DEFENDER_METRICS_REJECTED:
            _status = eDefenderRepRejected;
            break;

        /* Metrics failed to be sent. */
        default:
            _status = eDefenderRepNotSent;
    }
}

/*-----------------------------------------------------------*/

static DefenderErr_t _toDefenderErr( AwsIotDefenderError_t error )
{
    DefenderErr_t oldError = eDefenderErrSuccess;

    switch( error )
    {
        case AWS_IOT_DEFENDER_SUCCESS:
            oldError = eDefenderErrSuccess;
            break;

        case AWS_IOT_DEFENDER_ALREADY_STARTED:
            oldError = eDefenderErrAlreadyStarted;
            break;

        case AWS_IOT_DEFENDER_PERIOD_TOO_SHORT:
            oldError = eDefenderErrPeriodTooShort;
            break;

        default:
            oldError = eDefenderErrOther;
    }

    return oldError;
}

/*-----------------------------------------------------------*/

DefenderErr_t DEFENDER_MetricsInitFunc( DefenderMetric_t * pxMetricsList,
                                        int32_t lMetricsLength )
{
    uint32_t flag = 0;

    size_t i = 0, metricsLength = ( size_t ) lMetricsLength;

    for( ; i < metricsLength; i++ )
    {
        switch( pxMetricsList[ i ] )
        {
            case xDefenderTCPConnections:
                flag |= AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL;
                break;

            default:
                ;
        }
    }

    return _toDefenderErr( AwsIotDefender_SetMetrics( AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS, flag ) );
}


/*-----------------------------------------------------------*/

DefenderErr_t DEFENDER_ReportPeriodSet( uint32_t ulPeriodSec )
{
    return _toDefenderErr( AwsIotDefender_SetPeriod( ( uint64_t ) ulPeriodSec ) );
}


/*-----------------------------------------------------------*/

DefenderErr_t DEFENDER_ConnectionTimeoutSet( uint32_t ulTimeoutMs )
{
    ( void ) ulTimeoutMs;

    /* Not set anthing. This function is just for backward compatibility. */
    return eDefenderErrSuccess;
}


/*-----------------------------------------------------------*/

DefenderErr_t DEFENDER_Start( void )
{
    /* Register an internal callback. */
    const AwsIotDefenderCallback_t callback = { .function = _defenderCallback, .param1 = NULL };

    AwsIotDefenderStartInfo_t startInfo = AWS_IOT_DEFENDER_START_INFO_INITIALIZER;

    /* Set network information. */
    startInfo.mqttNetworkInfo = ( IotMqttNetworkInfo_t ) IOT_MQTT_NETWORK_INFO_INITIALIZER;
    startInfo.mqttNetworkInfo.createNetworkConnection = true;
    startInfo.mqttNetworkInfo.u.setup.pNetworkServerInfo = &_AWS_IOT_SERVER_INFO;
    startInfo.mqttNetworkInfo.u.setup.pNetworkCredentialInfo = &_AWS_IOT_CREDENTIALS;

    /* Only set ALPN protocol if the connected port is 443. */
    if( ( ( IotNetworkServerInfo_t * ) ( startInfo.mqttNetworkInfo.u.setup.pNetworkServerInfo ) )->port != 443 )
    {
        ( ( IotNetworkCredentials_t * ) ( startInfo.mqttNetworkInfo.u.setup.pNetworkCredentialInfo ) )->pAlpnProtos = NULL;
    }

    /* Set network interface. */
    startInfo.mqttNetworkInfo.pNetworkInterface = IOT_NETWORK_INTERFACE_AFR;

    /* Set MQTT connection information. */
    startInfo.mqttConnectionInfo = ( IotMqttConnectInfo_t ) IOT_MQTT_CONNECT_INFO_INITIALIZER;
    startInfo.mqttConnectionInfo.pClientIdentifier = clientcredentialIOT_THING_NAME;
    startInfo.mqttConnectionInfo.clientIdentifierLength = ( uint16_t ) strlen( clientcredentialIOT_THING_NAME );

    /* Set Defender callback. */
    startInfo.callback = callback;

    /* Invoke defender start API. */
    return _toDefenderErr( AwsIotDefender_Start( &startInfo ) );
}

/*-----------------------------------------------------------*/

DefenderErr_t DEFENDER_Stop( void )
{
    AwsIotDefender_Stop();

    /* No failure cases. */
    return eDefenderErrSuccess;
}

/*-----------------------------------------------------------*/

DefenderReportStatus_t DEFENDER_ReportStatusGet( void )
{
    return _status;
}

/*-----------------------------------------------------------*/

int32_t GetLastReportId( void )
{
    /* This function is not supported anymore. */
    return 0;
}

/*-----------------------------------------------------------*/

char const * DEFENDER_ErrAsString( DefenderErr_t eErrNum )
{
    switch( eErrNum )
    {
        case eDefenderErrSuccess:

            return "eDefenderErrSuccess";

        case eDefenderErrTooManyMetrics:

            return "eDefenderErrTooManyMetrics";

        case eDefenderErrPeriodTooShort:

            return "eDefenderErrPeriodTooShort";

        case eDefenderErrFailedToCreateTask:

            return "eDefenderErrFailedToCreateTask";

        case eDefenderErrAlreadyStarted:

            return "eDefenderErrAlreadyStarted";

        case eDefenderErrNotStarted:

            return "eDefenderErrNotStarted";

        case eDefenderErrOther:

            return "eDefenderErrOther";
    }

    return "Invalid value";
}

/*-----------------------------------------------------------*/

char const * DEFENDER_ReportStatusAsString( DefenderReportStatus_t eStatusNum )
{
    switch( eStatusNum )
    {
        case eDefenderRepSuccess:

            return "eDefenderRepSuccess";

        case eDefenderRepInit:

            return "eDefenderRepInit";

        case eDefenderRepRejected:

            return "eDefenderRepRejected";

        case eDefenderRepNoAck:

            return "eDefenderRepNoAck";

        case eDefenderRepNotSent:

            return "eDefenderRepNotSent";
    }

    return "Invalid value";
}
