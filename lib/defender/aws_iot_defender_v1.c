/*
 * Amazon FreeRTOS Device Defender Agent V1.0.2
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

/* Old and new defender includes. */
#include "aws_iot_defender.h"
#include "aws_defender.h"

#include "aws_clientcredential.h"

extern uint64_t _AwsIotDefenderReportId;

/* Defender agent's status, initialized with eDefenderRepInit. */
static DefenderReportStatus_t _status = eDefenderRepInit;

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

    /* Use compile-time network information. */
    AwsIotDefenderStartInfo_t startInfo =
    {
        .tlsInfo         = IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER,
        .pAwsIotEndpoint = clientcredentialMQTT_BROKER_ENDPOINT,
        .port            = clientcredentialMQTT_BROKER_PORT,
        .pThingName      = clientcredentialIOT_THING_NAME,
        .thingNameLength = ( uint16_t ) strlen( clientcredentialIOT_THING_NAME ),
        .callback        = callback
    };

    /* Use the default root CA provided with Amazon FreeRTOS. */
    startInfo.tlsInfo.pRootCa = NULL;
    startInfo.tlsInfo.rootCaLength = 0;

    /* Set client credentials. */
    startInfo.tlsInfo.pClientCert = clientcredentialCLIENT_CERTIFICATE_PEM;
    startInfo.tlsInfo.clientCertSize = ( size_t ) clientcredentialCLIENT_CERTIFICATE_LENGTH;
    startInfo.tlsInfo.pPrivateKey = clientcredentialCLIENT_PRIVATE_KEY_PEM;
    startInfo.tlsInfo.privateKeySize = ( size_t ) clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

    /* If not connecting over port 443, disable ALPN. */
    if( clientcredentialMQTT_BROKER_PORT != 443 )
    {
        startInfo.tlsInfo.pAlpnProtos = NULL;
    }

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
    return ( int32_t ) _AwsIotDefenderReportId;
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
