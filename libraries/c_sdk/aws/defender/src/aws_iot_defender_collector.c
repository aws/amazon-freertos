/*
 * FreeRTOS Defender V3.0.2
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
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* Standard includes */
#include <stdio.h>

/* Defender internal include. */
#include "private/aws_iot_defender_internal.h"

#include "platform/iot_metrics.h"

#include "platform/iot_clock.h"

/* Used in debugging. It will decode the report with cbor format and print to stdout. */
#define DEBUG_CBOR_PRINT    0

#define  HEADER_TAG         AwsIotDefenderInternal_SelectTag( "header", "hed" )
#define  REPORTID_TAG       AwsIotDefenderInternal_SelectTag( "report_id", "rid" )
#define  VERSION_TAG        AwsIotDefenderInternal_SelectTag( "version", "v" )
#define  VERSION_1_0        "1.0"   /* Used by defender service to indicate the schema change of report, e.g. adding new field. */
#define  METRICS_TAG        AwsIotDefenderInternal_SelectTag( "metrics", "met" )

#define TCP_CONN_TAG        AwsIotDefenderInternal_SelectTag( "tcp_connections", "tc" )
#define EST_CONN_TAG        AwsIotDefenderInternal_SelectTag( "established_connections", "ec" )
#define TOTAL_TAG           AwsIotDefenderInternal_SelectTag( "total", "t" )
#define CONN_TAG            AwsIotDefenderInternal_SelectTag( "connections", "cs" )
#define REMOTE_ADDR_TAG     AwsIotDefenderInternal_SelectTag( "remote_addr", "rad" )

/**
 * Structure to hold a metrics report.
 */
typedef struct _metricsReport
{
    IotSerializerEncoderObject_t object; /* Encoder object handle. */
    uint8_t * pDataBuffer;               /* Raw data buffer to be published with MQTT. */
    size_t size;                         /* Raw data size. */
} _metricsReport_t;

/* Initialize metrics report. */
static _metricsReport_t _report =
{
    .object      = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM,
    .pDataBuffer = NULL,
    .size        = 0
};

/* Define a "snapshot" global array of metrics flag. */
static uint32_t _metricsFlagSnapshot[ DEFENDER_METRICS_GROUP_COUNT ];

/* Report id integer. */
static uint64_t _AwsIotDefenderReportId = 0;

const IotSerializerEncodeInterface_t * _pAwsIotDefenderEncoder = NULL;
const IotSerializerDecodeInterface_t * _pAwsIotDefenderDecoder = NULL;

/*---------------------- Helper Functions -------------------------*/

static void _assertSuccess( IotSerializerError_t error );

static void _assertSuccessOrBufferToSmall( IotSerializerError_t error );

static void _copyMetricsFlag( void );

static void _serialize( void );

static void _serializeTcpConnections( void * param1,
                                      const IotListDouble_t * pTcpConnectionsMetricsList );

#if DEBUG_CBOR_PRINT == 1
    static void _printReport();
#endif

/*-----------------------------------------------------------*/

void _assertSuccess( IotSerializerError_t error )
{
    ( void ) error;
    AwsIotDefender_Assert( error == IOT_SERIALIZER_SUCCESS );
}

/*-----------------------------------------------------------*/

void _assertSuccessOrBufferToSmall( IotSerializerError_t error )
{
    ( void ) error;
    AwsIotDefender_Assert( error == IOT_SERIALIZER_SUCCESS || error == IOT_SERIALIZER_BUFFER_TOO_SMALL );
}

/*-----------------------------------------------------------*/

uint8_t * AwsIotDefenderInternal_GetReportBuffer( void )
{
    return _report.pDataBuffer;
}

/*-----------------------------------------------------------*/

size_t AwsIotDefenderInternal_GetReportBufferSize( void )
{
    /* Encoder might over-calculate the needed size. Therefor encoded size might be smaller than buffer size: _report.size. */
    return _report.pDataBuffer == NULL ? 0
           : _pAwsIotDefenderEncoder->getEncodedSize( &_report.object, _report.pDataBuffer );
}

/*-----------------------------------------------------------*/

bool AwsIotDefenderInternal_CreateReport( void )
{
    /* Assert report buffer is not allocated. */
    AwsIotDefender_Assert( _report.pDataBuffer == NULL && _report.size == 0 );

    bool result = true;

    IotSerializerEncoderObject_t * pEncoderObject = &( _report.object );

    size_t dataSize = 0;
    uint8_t * pReportBuffer = NULL;

    /* Copy the metrics flag user specified. */
    _copyMetricsFlag();

    /* Generate report id based on current time. */
    _AwsIotDefenderReportId = IotClock_GetTimeMs();

    /* Dry-run serialization to calculate the required size. */
    _serialize();

    /* Get the calculated required size. */
    dataSize = _pAwsIotDefenderEncoder->getExtraBufferSizeNeeded( pEncoderObject );

    /* Clean the encoder object handle. */
    _pAwsIotDefenderEncoder->destroy( pEncoderObject );

    /* Allocate memory once. */
    pReportBuffer = AwsIotDefender_MallocReport( dataSize * sizeof( uint8_t ) );

    if( pReportBuffer != NULL )
    {
        _report.pDataBuffer = pReportBuffer;
        _report.size = dataSize;

        /* Actual serialization. */
        _serialize();

        /* Ouput the report to stdout if debugging mode is enabled. */
        #if DEBUG_CBOR_PRINT == 1
            _printReport();
        #endif
    }
    else
    {
        result = false;
    }

    return result;
}

/*-----------------------------------------------------------*/

void AwsIotDefenderInternal_DeleteReport( void )
{
    /* Destroy the encoder object. */
    _pAwsIotDefenderEncoder->destroy( &( _report.object ) );

    /* Free the memory of data buffer. */
    AwsIotDefender_FreeReport( _report.pDataBuffer );

    /* Reset report members. */
    _report.pDataBuffer = NULL;
    _report.size = 0;
    _report.object = ( IotSerializerEncoderObject_t ) IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
}

/*
 * report:
 * {
 *  "header": {
 *      "report_id": 1530304554,
 *      "version": "1.0"
 *  },
 *  "metrics": {
 *      ...
 *  }
 * }
 */
static void _serialize( void )
{
    IotSerializerError_t serializerError = IOT_SERIALIZER_SUCCESS;

    IotSerializerEncoderObject_t * pEncoderObject = &( _report.object );

    IotSerializerEncoderObject_t reportMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerEncoderObject_t headerMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerEncoderObject_t metricsMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;

    /* Define an assert function for serialization returned error. */
    void (* assertNoError)( IotSerializerError_t ) = _report.pDataBuffer == NULL ? _assertSuccessOrBufferToSmall
                                                     : _assertSuccess;

    uint8_t metricsGroupCount = 0;
    uint32_t i = 0;

    serializerError = _pAwsIotDefenderEncoder->init( pEncoderObject, _report.pDataBuffer, _report.size );
    assertNoError( serializerError );

    /* Create the outermost map with 2 keys: "header", "metrics". */
    serializerError = _pAwsIotDefenderEncoder->openContainer( pEncoderObject, &reportMap, 2 );
    assertNoError( serializerError );

    /* Create the "header" map with 2 keys: "report_id", "version". */
    serializerError = _pAwsIotDefenderEncoder->openContainerWithKey( &reportMap,
                                                                     HEADER_TAG,
                                                                     &headerMap,
                                                                     2 );
    assertNoError( serializerError );

    /* Append key-value pair of "report_Id" which uses clock time. */
    serializerError = _pAwsIotDefenderEncoder->appendKeyValue( &headerMap,
                                                               REPORTID_TAG,
                                                               IotSerializer_ScalarSignedInt( ( int64_t ) _AwsIotDefenderReportId ) );
    assertNoError( serializerError );

    /* Append key-value pair of "version". */
    serializerError = _pAwsIotDefenderEncoder->appendKeyValue( &headerMap,
                                                               VERSION_TAG,
                                                               IotSerializer_ScalarTextString( VERSION_1_0 ) );
    assertNoError( serializerError );

    /* Close the "header" map. */
    serializerError = _pAwsIotDefenderEncoder->closeContainer( &reportMap, &headerMap );
    assertNoError( serializerError );

    /* Count how many metrics groups user specified. */
    for( i = 0; i < DEFENDER_METRICS_GROUP_COUNT; i++ )
    {
        metricsGroupCount += _metricsFlagSnapshot[ i ] > 0;
    }

    /* Create the "metrics" map with number of keys as the number of metrics groups. */
    serializerError = _pAwsIotDefenderEncoder->openContainerWithKey( &reportMap,
                                                                     METRICS_TAG,
                                                                     &metricsMap,
                                                                     metricsGroupCount );
    assertNoError( serializerError );

    for( i = 0; i < DEFENDER_METRICS_GROUP_COUNT; i++ )
    {
        /* Skip if this metrics group has 0 metrics flag. */
        if( _metricsFlagSnapshot[ i ] )
        {
            switch( i )
            {
                case AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS:
                    IotMetrics_GetTcpConnections( ( void * ) &metricsMap, _serializeTcpConnections );
                    break;

                default:
                    /* The index of metricsFlagSnapshot must be one of the metrics group. */
                    AwsIotDefender_Assert( 0 );
            }
        }
    }

    /* Close the "metrics" map. */
    serializerError = _pAwsIotDefenderEncoder->closeContainer( &reportMap, &metricsMap );
    assertNoError( serializerError );

    /* Close the "report" map. */
    serializerError = _pAwsIotDefenderEncoder->closeContainer( pEncoderObject, &reportMap );
    assertNoError( serializerError );
}

/*-----------------------------------------------------------*/

static void _copyMetricsFlag( void )
{
    /* Copy the metrics flags to snapshot so that it is unlocked quicker. */
    IotMutex_Lock( &_AwsIotDefenderMetrics.mutex );

    /* Memory copy from the metricsFlag array to metricsFlagSnapshot array. */
    memcpy( _metricsFlagSnapshot, _AwsIotDefenderMetrics.metricsFlag, sizeof( _metricsFlagSnapshot ) );

    IotMutex_Unlock( &_AwsIotDefenderMetrics.mutex );
}

/*-----------------------------------------------------------*/

static void _serializeTcpConnections( void * param1,
                                      const IotListDouble_t * pTcpConnectionsMetricsList )
{
    IotSerializerEncoderObject_t * pMetricsObject = ( IotSerializerEncoderObject_t * ) param1;

    AwsIotDefender_Assert( pMetricsObject != NULL );

    IotSerializerError_t serializerError = IOT_SERIALIZER_SUCCESS;

    IotSerializerEncoderObject_t tcpConnectionMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerEncoderObject_t establishedMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerEncoderObject_t connectionsArray = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;

    IotLink_t * pListIterator = NULL;
    IotMetricsTcpConnection_t * pMetricsTcpConnection = NULL;

    size_t total = IotListDouble_Count( pTcpConnectionsMetricsList );

    uint32_t tcpConnFlag = _metricsFlagSnapshot[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ];

    uint8_t hasEstablishedConnections = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED ) > 0;
    /* Whether "connections" should show up is not only determined by user input, but also if there is at least 1 connection. */
    uint8_t hasConnections = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_CONNECTIONS ) > 0 &&
                             ( total > 0 );
    uint8_t hasTotal = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL ) > 0;
    uint8_t hasRemoteAddr = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR ) > 0;

    void (* assertNoError)( IotSerializerError_t ) = _report.pDataBuffer == NULL ? _assertSuccessOrBufferToSmall
                                                     : _assertSuccess;

    /* Create the "tcp_connections" map with 1 key "established_connections" */
    serializerError = _pAwsIotDefenderEncoder->openContainerWithKey( pMetricsObject,
                                                                     TCP_CONN_TAG,
                                                                     &tcpConnectionMap,
                                                                     1 );
    assertNoError( serializerError );

    /* if user specify any metrics under "established_connections" */
    if( hasEstablishedConnections )
    {
        /* Create the "established_connections" map with "total" and/or "connections". */
        serializerError = _pAwsIotDefenderEncoder->openContainerWithKey( &tcpConnectionMap,
                                                                         EST_CONN_TAG,
                                                                         &establishedMap,
                                                                         hasConnections + hasTotal );
        assertNoError( serializerError );

        /* if user specify any metrics under "connections" and there are at least one connection */
        if( hasConnections )
        {
            /* create array "connections" under "established_connections" */
            serializerError = _pAwsIotDefenderEncoder->openContainerWithKey( &establishedMap,
                                                                             CONN_TAG,
                                                                             &connectionsArray,
                                                                             total );
            assertNoError( serializerError );

            IotContainers_ForEach( pTcpConnectionsMetricsList, pListIterator )
            {
                IotSerializerEncoderObject_t connectionMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;

                /* open a map under "connections" */
                serializerError = _pAwsIotDefenderEncoder->openContainer( &connectionsArray,
                                                                          &connectionMap,
                                                                          hasRemoteAddr );
                assertNoError( serializerError );

                /* add remote address */
                if( hasRemoteAddr )
                {
                    pMetricsTcpConnection = IotLink_Container( IotMetricsTcpConnection_t, pListIterator, link );

                    serializerError = _pAwsIotDefenderEncoder->appendKeyValue( &connectionMap, REMOTE_ADDR_TAG,
                                                                               IotSerializer_ScalarTextString( pMetricsTcpConnection->pRemoteAddress ) );
                    assertNoError( serializerError );
                }

                serializerError = _pAwsIotDefenderEncoder->closeContainer( &connectionsArray, &connectionMap );
                assertNoError( serializerError );
            }

            serializerError = _pAwsIotDefenderEncoder->closeContainer( &establishedMap, &connectionsArray );
            assertNoError( serializerError );
        }

        if( hasTotal )
        {
            serializerError = _pAwsIotDefenderEncoder->appendKeyValue( &establishedMap,
                                                                       TOTAL_TAG,
                                                                       IotSerializer_ScalarSignedInt( total ) );
            assertNoError( serializerError );
        }

        serializerError = _pAwsIotDefenderEncoder->closeContainer( &tcpConnectionMap, &establishedMap );
        assertNoError( serializerError );
    }

    serializerError = _pAwsIotDefenderEncoder->closeContainer( pMetricsObject, &tcpConnectionMap );
    assertNoError( serializerError );
}

#if DEBUG_CBOR_PRINT == 1
    #include "cbor.h"
    /*-----------------------------------------------------------*/

    static void _printReport()
    {
        CborParser cborParser;
        CborValue cborValue;

        cbor_parser_init(
            _report.pDataBuffer,
            _report.size,
            0,
            &cborParser,
            &cborValue );
        cbor_value_to_pretty( stdout, &cborValue );
    }
#endif /* if DEBUG_CBOR_PRINT == 1 */
