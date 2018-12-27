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

#include "aws_iot_defender_metrics_socket.h"

/* Standard includes. */
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

#include "list.h"

/* Global variable defined in FreeRTOS TCP/IP stack. */
extern List_t xBoundTCPSocketsList;

/*-----------------------------------------------------------*/

uint8_t _getSocketCount( uint8_t tcpState )
{
    ListItem_t * pListItem = listGET_HEAD_ENTRY( &xBoundTCPSocketsList );
    FreeRTOS_Socket_t * pSocket = NULL;

    uint8_t total = 0;

    /* count the number of established tcp sockets */
    while( pListItem != listGET_END_MARKER( &xBoundTCPSocketsList ) )
    {
        pSocket = listGET_LIST_ITEM_OWNER( pListItem );

        if( ( pSocket != NULL ) && ( pSocket->u.xTCP.ucTCPState == tcpState ) )
        {
            total++;
        }

        pListItem = listGET_NEXT( pListItem );
    }

    return total;
}

/*-----------------------------------------------------------*/

uint8_t AwsIotDefenderInternal_GetEstablishedConnectionsCount()
{
    return _getSocketCount( eESTABLISHED );
}

/*-----------------------------------------------------------*/

uint8_t AwsIotDefenderInternal_GetMetricsEstablishedConnections( _defenderMetricsConnection_t * pConnections,
                                                              uint8_t connectionsLength )
{
    ListItem_t * pListItem = listGET_HEAD_ENTRY( &xBoundTCPSocketsList );
    FreeRTOS_Socket_t * pSocket = NULL;

	uint8_t result = 0;
    uint8_t i = 0;

    pListItem = listGET_HEAD_ENTRY( &xBoundTCPSocketsList );

    while( pListItem != listGET_END_MARKER( &xBoundTCPSocketsList ) )
    {
        pSocket = listGET_LIST_ITEM_OWNER( pListItem );

        if( ( pSocket != NULL ) && ( pSocket->u.xTCP.ucTCPState == eESTABLISHED ) )
        {
            if( i >= connectionsLength )
            {
                /* something wrong */
				result = 1;
				break;
            }

            pConnections[ i ].localPort = pSocket->usLocalPort;
            pConnections[ i ].remoteIP = pSocket->u.xTCP.ulRemoteIP;
            pConnections[ i ].remotePort = pSocket->u.xTCP.usRemotePort;

            i++;
        }

        pListItem = listGET_NEXT( pListItem );
    }

	return result;
}


/*-----------------------------------------------------------*/

uint8_t AwsIotDefenderInternal_GetListeningTcpPortsCount()
{
    return _getSocketCount( eTCP_LISTEN );
}

/*-----------------------------------------------------------*/

uint8_t AwsIotDefenderInternal_GetMetricsListeningTcpPorts( _defenderMetricsPort_t * pPorts,
                                                         uint8_t portsLength )
{
    ListItem_t * pListItem = listGET_HEAD_ENTRY( &xBoundTCPSocketsList );
    FreeRTOS_Socket_t * pSocket = NULL;

	uint8_t result = 0;
    uint8_t i = 0;

    pListItem = listGET_HEAD_ENTRY( &xBoundTCPSocketsList );

    while( pListItem != listGET_END_MARKER( &xBoundTCPSocketsList ) )
    {
        pSocket = listGET_LIST_ITEM_OWNER( pListItem );

        if( ( pSocket != NULL ) && ( pSocket->u.xTCP.ucTCPState == eTCP_LISTEN ) )
        {
            if( i >= portsLength )
            {
                /* something wrong */
				result = 1;
				break;
            }

            pPorts[ i ].port = pSocket->usLocalPort;

            i++;
        }

        pListItem = listGET_NEXT( pListItem );
    }

	return result;
}
