#include "aws_iot_metrics.h"

static AwsIotList_t _connectionsList;

/* Compare function to identify the TCP connection data handle. */
static bool _tcpConnectionMatch( void * pArgument,
                                 void * pData );

/*-----------------------------------------------------------*/

static bool _tcpConnectionMatch( void * pArgument,
                                 void * pData )
{
    void * pTcpConnectionHandle = pArgument;
    IotMetricsTcpConnection_t * pTcpConnection = ( IotMetricsTcpConnection_t * ) pData;

    return pTcpConnectionHandle == pTcpConnection->pHandle;
}

/*-----------------------------------------------------------*/

BaseType_t IotMetrics_Init()
{
    return AwsIotList_Create( &_connectionsList ) ? pdPASS : pdFAIL;
}

/*-----------------------------------------------------------*/

void IotMetrics_AddTcpConnection( IotMetricsTcpConnection_t * pTcpConnection )
{
    if( pTcpConnection == NULL )
    {
        return;
    }

    IotMetricsTcpConnection_t * pNewTcpConnection = pvPortMalloc( sizeof( IotMetricsTcpConnection_t ) );

    if( pNewTcpConnection == NULL )
    {
        return;
    }

    /* Copy TCP connection to the new one. */
    *pNewTcpConnection = *pTcpConnection;

    AwsIotMutex_Lock( &_connectionsList.mutex );

    AwsIotList_InsertHead( &_connectionsList,
                           &( pNewTcpConnection->link ),
                           IOT_METRICS_TCP_CONNECTION_OFFSET );

    AwsIotMutex_Unlock( &_connectionsList.mutex );
}

/*-----------------------------------------------------------*/

void IotMetrics_RemoveTcpConnection( void * pTcpConnectionHandle )
{
    if( pTcpConnectionHandle == NULL )
    {
        return;
    }

    AwsIotMutex_Lock( &_connectionsList.mutex );

    IotMetricsTcpConnection_t * pFoundConnection = AwsIotList_FindFirstMatch( _connectionsList.pHead,
                                                                              IOT_METRICS_TCP_CONNECTION_OFFSET,
                                                                              pTcpConnectionHandle,
                                                                              _tcpConnectionMatch );

    if( pFoundConnection != NULL )
    {
        AwsIotList_Remove( &_connectionsList, &( pFoundConnection->link ), IOT_METRICS_TCP_CONNECTION_OFFSET );
        vPortFree( pFoundConnection );
    }

    AwsIotMutex_Unlock( &_connectionsList.mutex );
}

/*-----------------------------------------------------------*/

void * IotMetrics_ProcessTcpConnections( IotMetricsListCallback_t tcpConnectionsCallback )
{
    if( tcpConnectionsCallback.function == NULL )
    {
        return NULL;
    }

    AwsIotMutex_Lock( &_connectionsList.mutex );

    void * pResult = tcpConnectionsCallback.function( tcpConnectionsCallback.param1, &_connectionsList );

    AwsIotMutex_Unlock( &_connectionsList.mutex );

    return pResult;
}
