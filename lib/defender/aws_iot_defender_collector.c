/*
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
 */

/* Defender internal include. */
#include "private/aws_iot_defender_internal.h"

#include "platform/aws_iot_clock.h"

#define  _HEADER_TAG      AwsIotDefenderInternal_SelectTag( "header", "hed" )
#define  _REPORTID_TAG    AwsIotDefenderInternal_SelectTag( "report_id", "rid" )
#define  _VERSION_TAG     AwsIotDefenderInternal_SelectTag( "version", "v" )
/* This version is used by defender service to indicate the schema change of report, e.g. adding new field. */
#define  _VERSION_1_0     "1.0"
#define  _METRICS_TAG     AwsIotDefenderInternal_SelectTag( "metrics", "met" )

/**
 * Attempt to serialize metrics report with given data buffer.
 */
static bool _serialize( AwsIotSerializerEncoderObject_t * pEncoderObject,
                        uint8_t * pDataBuffer,
                        size_t dataSize );

/*-----------------------------------------------------------*/

bool AwsIotDefenderInternal_CreateReport( _defenderReport_t * pReport,
                                          AwsIotDefenderEventType_t * pEventType )
{
    AwsIotSerializerEncoderObject_t * pEncoderObject = &( pReport->object );
    size_t dataSize = 0;
    bool result = false;
    uint8_t * pReportBuffer = NULL;

    /* first-round serialization to calculate the size */
    if( _serialize( pEncoderObject, NULL, 0 ) )
    {
        /* get the calculated required size. */
        dataSize = _AwsIotDefenderEncoder.getExtraBufferSizeNeeded( pEncoderObject );

        /* clean the object handle. */
        _AwsIotDefenderEncoder.destroy( pEncoderObject );

        /* allocated memory only one time. */
        pReportBuffer = pvPortMalloc( dataSize * sizeof( uint8_t ) );

        if( pReportBuffer != NULL )
        {
            /* second-round serialization to do the actual encoding */
            if( _serialize( pEncoderObject, pReportBuffer, dataSize ) )
            {
                pReport->pDataBuffer = pReportBuffer;
                pReport->size = dataSize;
                result = true;
            }
            else
            {
                *pEventType = AWS_IOT_DEFENDER_METRICS_SERIALIZATION_FAILED;
            }
        }
        else
        {
            *pEventType = AWS_IOT_DEFENDER_EVENT_NO_MEMORY;
        }
    }
    else
    {
        *pEventType = AWS_IOT_DEFENDER_METRICS_SERIALIZATION_FAILED;
    }

    return result;
}

/*-----------------------------------------------------------*/

void AwsIotDefenderInternal_DeleteReport( _defenderReport_t * report )
{
    _AwsIotDefenderEncoder.destroy( &( report->object ) );
    vPortFree( report->pDataBuffer );
    report->pDataBuffer = NULL;
    report->size = 0;
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
static bool _serialize( AwsIotSerializerEncoderObject_t * pEncoderObject,
                        uint8_t * pDataBuffer,
                        size_t dataSize )
{
    /* Declare and initialize sub-objects */
    AwsIotSerializerEncoderObject_t reportMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t headerMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t metricsMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;

    AwsIotSerializerError_t serializerError = AWS_IOT_SERIALIZER_SUCCESS;

    bool ignoreTooSmallBuffer = pDataBuffer == NULL;

    serializerError = _AwsIotDefenderEncoder.init( pEncoderObject, pDataBuffer, dataSize );

    if( _defenderSerializeSuccess( serializerError, ignoreTooSmallBuffer ) )
    {
        /* 2 sub-objects: header, metrics */
        serializerError = _AwsIotDefenderEncoder.openContainer( pEncoderObject,
                                                                &reportMap,
                                                                2 );
    }

    if( _defenderSerializeSuccess( serializerError, ignoreTooSmallBuffer ) )
    {
        /* 2 keys: report_id, version */
        serializerError = _AwsIotDefenderEncoder.openContainerWithKey( &reportMap,
                                                                       _HEADER_TAG,
                                                                       &headerMap,
                                                                       2 );
    }

    if( _defenderSerializeSuccess( serializerError, ignoreTooSmallBuffer ) )
    {
        serializerError = _AwsIotDefenderEncoder.appendKeyValue( &headerMap,
                                                                 _REPORTID_TAG,
                                                                 AwsIotSerializer_ScalarSignedInt( AwsIotClock_GetTimeMs() ) );
    }

    if( _defenderSerializeSuccess( serializerError, ignoreTooSmallBuffer ) )
    {
        serializerError = _AwsIotDefenderEncoder.appendKeyValue( &headerMap,
                                                                 _VERSION_TAG,
                                                                 AwsIotSerializer_ScalarTextString( _VERSION_1_0 ) );
    }

    if( _defenderSerializeSuccess( serializerError, ignoreTooSmallBuffer ) )
    {
        serializerError = _AwsIotDefenderEncoder.closeContainer( &reportMap, &headerMap );
    }

    if( _defenderSerializeSuccess( serializerError, ignoreTooSmallBuffer ) )
    {
        /* length is unknown */
        serializerError = _AwsIotDefenderEncoder.openContainerWithKey( &reportMap,
                                                                       _METRICS_TAG,
                                                                       &metricsMap,
                                                                       AWS_IOT_SERIALIZER_INDEFINITE_LENGTH );
    }

    if( _defenderSerializeSuccess( serializerError, ignoreTooSmallBuffer ) )
    {
        AwsIotMutex_Lock( &_AwsIotDefenderMetrics.mutex );

        for( uint8_t i = 0; i < _DEFENDER_METRICS_GROUP_COUNT; i++ )
        {
            /* if no specified metircs in this group, simply skip. */
            if( _AwsIotDefenderMetrics.metricsFlag[ i ] )
            {
                switch( i )
                {
                    case AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS:
                        serializerError = AwsIotDefenderInternal_GetMetricsTcpConnections( &metricsMap, ignoreTooSmallBuffer );
                        break;

                    case AWS_IOT_DEFENDER_METRICS_LISTENING_TCP:
                        serializerError = AwsIotDefenderInternal_GetMetricsListeningTcpPorts( &metricsMap, ignoreTooSmallBuffer );
                        break;

                    default:
                        ;
                }
            }

            if( !_defenderSerializeSuccess( serializerError, ignoreTooSmallBuffer ) )
            {
                break;
            }
        }

        AwsIotMutex_Unlock( &_AwsIotDefenderMetrics.mutex );
    }

    if( _defenderSerializeSuccess( serializerError, ignoreTooSmallBuffer ) )
    {
        serializerError = _AwsIotDefenderEncoder.closeContainer( &reportMap, &metricsMap );
    }

    if( _defenderSerializeSuccess( serializerError, ignoreTooSmallBuffer ) )
    {
        serializerError = _AwsIotDefenderEncoder.closeContainer( pEncoderObject, &reportMap );
    }

    return _defenderSerializeSuccess( serializerError, ignoreTooSmallBuffer );
}
