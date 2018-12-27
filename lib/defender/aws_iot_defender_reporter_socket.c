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

/* Specific metrics include. */
#include "aws_iot_defender_metrics_socket.h"

/* Secure sockets include. */
#include "aws_secure_sockets.h"

#define _TCP_CONN_TAG         AwsIotDefenderInternal_SelectTag( "tcp_connections", "tc" )
#define _EST_CONN_TAG         AwsIotDefenderInternal_SelectTag( "established_connections", "ec" )
#define _TOTAL_TAG            AwsIotDefenderInternal_SelectTag( "total", "t" )
#define _CONN_TAG             AwsIotDefenderInternal_SelectTag( "connections", "cs" )
#define _LOCAL_PORT_TAG       AwsIotDefenderInternal_SelectTag( "local_port", "lp" )
#define _REMOTE_ADDR_TAG      AwsIotDefenderInternal_SelectTag( "remote_addr", "rad" )

#define _LIS_TCP_PORTS_TAG    AwsIotDefenderInternal_SelectTag( "listening_tcp_ports", "tp" )
#define _PORTS_TAG            AwsIotDefenderInternal_SelectTag( "ports", "pts" )
#define _PORT_TAG             AwsIotDefenderInternal_SelectTag( "port", "pt" )

/*-----------------------------------------------------------*/

AwsIotSerializerError_t AwsIotDefenderInternal_GetTcpConnMetrics( AwsIotSerializerEncoderObject_t * pEncoderObject )
{
    AwsIotSerializerError_t error;

    AwsIotSerializerEncoderObject_t tcpConnectionMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t establishedMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t connectionsArray = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;

    uint32_t tcpConnFlag = _AwsIotDefenderMetricsFlag[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ];

    uint8_t hasEstablishedConnections = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED ) > 0;
    uint8_t hasConnections = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_CONNECTIONS ) > 0;
    uint8_t hasTotal = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL ) > 0;
    uint8_t hasLocalPort = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_LOCAL_PORT ) > 0;
    uint8_t hasRemoteAddr = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR ) > 0;

    uint8_t total = AwsIotDefenderInternal_GetEstablishedConnectionsCount();

    _defenderMetricsConnection_t * pConnections = NULL;

    /* 15 for IP + 1 for ":" + 5 for port + 1 terminator */
    char remoteAddr[ 15 + 1 + 5 + 1 ] = "";
    char * pRemoteAddr = remoteAddr;

    /* "tcp_connections" has only 1 child "established_connections" */
    error = _AwsIotDefenderEncoder.openContainerWithKey( pEncoderObject,
                                                         _TCP_CONN_TAG,
                                                         &tcpConnectionMap,
                                                         1 );

    /* if user specify any metrics under "established_connections" */
    if( hasEstablishedConnections && !error )
    {
        /* create map "established_connections" under "tcp_connections". */
        error = _AwsIotDefenderEncoder.openContainerWithKey( &tcpConnectionMap,
                                                             _EST_CONN_TAG,
                                                             &establishedMap,
                                                             hasConnections + hasTotal );

        /* if user specify any metrics under "connections" and there are at least one connection */
        if( hasConnections && ( total > 0 ) && !error )
        {
            pConnections = pvPortMalloc( total * sizeof( _defenderMetricsConnection_t ) );

            /* allocated memory for connections metrics */
            if( pConnections != NULL )
            {
                AwsIotDefenderInternal_GetMetricsEstablishedConnections( pConnections, total );

                /* create array "connections" under "established_connections" */
                error = _AwsIotDefenderEncoder.openContainerWithKey( &establishedMap,
                                                                     _CONN_TAG,
                                                                     &connectionsArray,
                                                                     total );

                if( !error )
                {
                    for( uint8_t i = 0; i < total; i++ )
                    {
                        AwsIotSerializerEncoderObject_t connectionMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;

                        /* open a map under "connections" */
                        error = _AwsIotDefenderEncoder.openContainer( &connectionsArray,
                                                                      &connectionMap,
                                                                      hasLocalPort + hasRemoteAddr );

                        /* add local port */
                        if( hasLocalPort && !error )
                        {
                            error = _AwsIotDefenderEncoder.appendKeyValue( &connectionMap, _LOCAL_PORT_TAG,
                                                                           AwsIotSerializer_ScalarSignedInt( pConnections[ i ].localPort ) );
                        }

                        /* add remote address */
                        if( hasRemoteAddr && !error )
                        {
                            SOCKETS_inet_ntoa( pConnections[ i ].remoteIP, remoteAddr );
                            sprintf( remoteAddr, "%s:%d", remoteAddr, pConnections[ i ].remotePort );

                            error = _AwsIotDefenderEncoder.appendKeyValue( &connectionMap, _REMOTE_ADDR_TAG,
                                                                           AwsIotSerializer_ScalarTextString( pRemoteAddr ) );
                        }

                        if( !error )
                        {
                            error = _AwsIotDefenderEncoder.closeContainer( &connectionsArray, &connectionMap );
                        }
                    }
                }

                if( !error )
                {
                    error = _AwsIotDefenderEncoder.closeContainer( &establishedMap, &connectionsArray );
                }

                vPortFree( pConnections );
            }
            else
            {
                error = AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
            }
        }

        if( hasTotal && !error )
        {
            error = _AwsIotDefenderEncoder.appendKeyValue( &establishedMap,
                                                           _TOTAL_TAG,
                                                           AwsIotSerializer_ScalarSignedInt( total ) );
        }

        if( !error )
        {
            error = _AwsIotDefenderEncoder.closeContainer( &tcpConnectionMap, &establishedMap );
        }
    }

    if( !error )
    {
        error = _AwsIotDefenderEncoder.closeContainer( pEncoderObject, &tcpConnectionMap );
    }

    return error;
}

/*-----------------------------------------------------------*/

AwsIotSerializerError_t AwsIotDefenderInternal_GetLisTcpMetrics( AwsIotSerializerEncoderObject_t * pEncoderObject )
{
    AwsIotSerializerError_t error;

    AwsIotSerializerEncoderObject_t lisTcpPortsMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t portsArray = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;

    uint32_t lisTcpPortsFlag = _AwsIotDefenderMetricsFlag[ AWS_IOT_DEFENDER_METRICS_LISTENING_TCP ];

    uint8_t hasPorts = ( lisTcpPortsFlag & AWS_IOT_DEFENDER_METRICS_LISTENING_TCP_PORTS ) > 0;
    uint8_t hasPortNumber = ( lisTcpPortsFlag & AWS_IOT_DEFENDER_METRICS_LISTENING_TCP_PORT ) > 0;
    uint8_t hasTotal = ( lisTcpPortsFlag & AWS_IOT_DEFENDER_METRICS_LISTENING_TCP_TOTAL ) > 0;

    uint8_t total = AwsIotDefenderInternal_GetListeningTcpPortsCount();

    _defenderMetricsPort_t * pPorts = NULL;

    /* create "listening_udp_ports" map. */
    error = _AwsIotDefenderEncoder.openContainerWithKey( pEncoderObject,
                                                         _LIS_TCP_PORTS_TAG,
                                                         &lisTcpPortsMap,
                                                         hasPorts + hasTotal );

    /* if user specify any metrics under "established_connections" */
    if( hasPorts && ( total > 0 ) && !error )
    {
        pPorts = pvPortMalloc( total * sizeof( _defenderMetricsPort_t ) );

        if( pPorts != NULL )
        {
            AwsIotDefenderInternal_GetMetricsListeningTcpPorts( pPorts, total );

            /* create array "ports" under "listening_tcp_ports". */
            error = _AwsIotDefenderEncoder.openContainerWithKey( &lisTcpPortsMap,
                                                                 _PORTS_TAG,
                                                                 &portsArray,
                                                                 total );

            if( !error )
            {
                for( uint8_t i = 0; i < total; i++ )
                {
                    AwsIotSerializerEncoderObject_t portMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;

                    /* open a map under "ports" */
                    error = _AwsIotDefenderEncoder.openContainer( &portsArray,
                                                                  &portMap,
                                                                  1 );

                    if( hasPortNumber && !error )
                    {
                        error = _AwsIotDefenderEncoder.appendKeyValue( &portMap, _PORT_TAG,
                                                                       AwsIotSerializer_ScalarSignedInt( pPorts[ i ].port ) );
                    }

                    if( !error )
                    {
                        error = _AwsIotDefenderEncoder.closeContainer( &portsArray, &portMap );
                    }
                }
            }

            if( !error )
            {
                error = _AwsIotDefenderEncoder.closeContainer( &lisTcpPortsMap, &portsArray );
            }

            vPortFree( pPorts );
        }
        else
        {
            error = AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
        }

        if( hasTotal && !error )
        {
            error = _AwsIotDefenderEncoder.appendKeyValue( &lisTcpPortsMap,
                                                           _TOTAL_TAG,
                                                           AwsIotSerializer_ScalarSignedInt( total ) );
        }

        error = _AwsIotDefenderEncoder.closeContainer( pEncoderObject, &lisTcpPortsMap );
    }

    return error;
}
