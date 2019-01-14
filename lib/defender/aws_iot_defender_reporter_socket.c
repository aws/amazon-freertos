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

/* Secure sockets include. */
#include "aws_secure_sockets.h"

/* Metrics include. */
#include "aws_iot_metrics.h"

/* 15 for IP + 1 for ":" + 5 for port + 1 terminator
 * For example: "192.168.0.1:8000\0"
 */
#define _REMOTE_ADDR_LENGTH    22

#define _TCP_CONN_TAG          AwsIotDefenderInternal_SelectTag( "tcp_connections", "tc" )
#define _EST_CONN_TAG          AwsIotDefenderInternal_SelectTag( "established_connections", "ec" )
#define _TOTAL_TAG             AwsIotDefenderInternal_SelectTag( "total", "t" )
#define _CONN_TAG              AwsIotDefenderInternal_SelectTag( "connections", "cs" )
#define _REMOTE_ADDR_TAG       AwsIotDefenderInternal_SelectTag( "remote_addr", "rad" )


/*-----------------------------------------------------------*/

void * AwsIotDefenderInternal_TcpConnectionsCallback( void * param1,
                                                      AwsIotList_t * pTcpConnectionsMetricsList )
{
    _defenderMetricsCallbackInfo_t * pCallbackInfo = ( _defenderMetricsCallbackInfo_t * ) param1;

    AwsIotSerializerEncoderObject_t * pEncoderObject = pCallbackInfo->pEncoderObject;
    bool ignoreTooSmallBuffer = pCallbackInfo->ignoreTooSmallBuffer;

    AwsIotSerializerError_t error;

    AwsIotSerializerEncoderObject_t tcpConnectionMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t establishedMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t connectionsArray = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;

    uint32_t tcpConnFlag = _AwsIotDefenderMetrics.metricsFlag[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ];

    uint8_t hasEstablishedConnections = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED ) > 0;
    uint8_t hasConnections = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_CONNECTIONS ) > 0;
    uint8_t hasTotal = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL ) > 0;
    uint8_t hasRemoteAddr = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR ) > 0;

    char remoteAddr[ _REMOTE_ADDR_LENGTH ] = "";
    char * pRemoteAddr = remoteAddr;

    /* "tcp_connections" has only 1 child "established_connections" */
    error = _AwsIotDefenderEncoder.openContainerWithKey( pEncoderObject,
                                                         _TCP_CONN_TAG,
                                                         &tcpConnectionMap,
                                                         1 );

    /* if user specify any metrics under "established_connections" */
    if( hasEstablishedConnections && _defenderSerializeSuccess( error, ignoreTooSmallBuffer ) )
    {
        /* create map "established_connections" under "tcp_connections". */
        error = _AwsIotDefenderEncoder.openContainerWithKey( &tcpConnectionMap,
                                                             _EST_CONN_TAG,
                                                             &establishedMap,
                                                             hasConnections + hasTotal );

        uint8_t total = 0;
        AwsIotLink_t * pConnectionLink = pTcpConnectionsMetricsList->pHead;

        /* Count total connections. */
        for( ; pConnectionLink != NULL; pConnectionLink = pConnectionLink->pNext )
        {
            total++;
        }

        /* if user specify any metrics under "connections" and there are at least one connection */
        if( hasConnections && ( total > 0 ) && _defenderSerializeSuccess( error, ignoreTooSmallBuffer ) )
        {
            /* create array "connections" under "established_connections" */
            error = _AwsIotDefenderEncoder.openContainerWithKey( &establishedMap,
                                                                 _CONN_TAG,
                                                                 &connectionsArray,
                                                                 total );

            if( _defenderSerializeSuccess( error, ignoreTooSmallBuffer ) )
            {
                for( pConnectionLink = pTcpConnectionsMetricsList->pHead; pConnectionLink != NULL; pConnectionLink = pConnectionLink->pNext )
                {
                    IotMetricsTcpConnection_t * pConnection = AwsIotLink_Container( pConnectionLink, AwsIotLink_Offset( IotMetricsTcpConnection_t, link ) );
                    AwsIotSerializerEncoderObject_t connectionMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;

                    /* open a map under "connections" */
                    error = _AwsIotDefenderEncoder.openContainer( &connectionsArray,
                                                                  &connectionMap,
                                                                  hasRemoteAddr );

                    /* add remote address */
                    if( hasRemoteAddr && _defenderSerializeSuccess( error, ignoreTooSmallBuffer ) )
                    {
                        SOCKETS_inet_ntoa( pConnection->remoteIP, remoteAddr );
                        sprintf( remoteAddr, "%s:%d", remoteAddr, pConnection->remotePort );

                        error = _AwsIotDefenderEncoder.appendKeyValue( &connectionMap, _REMOTE_ADDR_TAG,
                                                                       AwsIotSerializer_ScalarTextString( pRemoteAddr ) );
                    }

                    if( _defenderSerializeSuccess( error, ignoreTooSmallBuffer ) )
                    {
                        error = _AwsIotDefenderEncoder.closeContainer( &connectionsArray, &connectionMap );
                    }
                }
            }

            if( _defenderSerializeSuccess( error, ignoreTooSmallBuffer ) )
            {
                error = _AwsIotDefenderEncoder.closeContainer( &establishedMap, &connectionsArray );
            }
        }

        if( hasTotal && _defenderSerializeSuccess( error, ignoreTooSmallBuffer ) )
        {
            error = _AwsIotDefenderEncoder.appendKeyValue( &establishedMap,
                                                           _TOTAL_TAG,
                                                           AwsIotSerializer_ScalarSignedInt( total ) );
        }

        if( _defenderSerializeSuccess( error, ignoreTooSmallBuffer ) )
        {
            error = _AwsIotDefenderEncoder.closeContainer( &tcpConnectionMap, &establishedMap );
        }
    }

    if( _defenderSerializeSuccess( error, ignoreTooSmallBuffer ) )
    {
        error = _AwsIotDefenderEncoder.closeContainer( pEncoderObject, &tcpConnectionMap );
    }

    /* Cast AwsIotSerializerError_t to void pointer explicitly. */
    return ( void * ) error;
}

