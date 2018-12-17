#include "iot_metrics.h"

static AwsIotList_t _connectionsList;

/* Compare function to identify the TCP connection data handle. */
static bool _tcpConnectionMatch( void * pArgument,
                                 void * pData );

/*-----------------------------------------------------------*/

static bool _tcpConnectionMatch( void * pArgument,
                                 void * pData )
{
    IotMetrics_Assert( pArgument != NULL );
    IotMetrics_Assert( pData != NULL );

    /* Passed argument is a handle of IotMetricsTcpConnection_t. */
    void * pTcpConnectionHandle = pArgument;

    /* Passed data is a pointer of IotMetricsTcpConnection_t. */
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
    IotMetrics_Assert( pTcpConnection != NULL );
    IotMetrics_Assert( pTcpConnection->pHandle != NULL );

    AwsIotMutex_Lock( &_connectionsList.mutex );

    /* Check if it exists in the connectionsList. */
    IotMetricsTcpConnection_t * pFoundConnection = AwsIotList_FindFirstMatch( _connectionsList.pHead,
                                                                              IOT_METRICS_TCP_CONNECTION_OFFSET,
                                                                              pTcpConnection->pHandle,
                                                                              _tcpConnectionMatch );

    /* Only add if it doesn't exist. */
    if( pFoundConnection == NULL )
    {
        IotMetricsTcpConnection_t * pNewTcpConnection = AwsIotMetrics_MallocTcpConnection( sizeof( IotMetricsTcpConnection_t ) );

        if( pNewTcpConnection != NULL )
        {
            /* Copy TCP connection to the new one. */
            *pNewTcpConnection = *pTcpConnection;

            /* Insert to the list. */
            AwsIotList_InsertHead( &_connectionsList,
                                   &( pNewTcpConnection->link ),
                                   IOT_METRICS_TCP_CONNECTION_OFFSET );
        }
    }

    AwsIotMutex_Unlock( &_connectionsList.mutex );
}

/*-----------------------------------------------------------*/

void IotMetrics_RemoveTcpConnection( void * pTcpConnectionHandle )
{
    IotMetrics_Assert( pTcpConnectionHandle != NULL );

    AwsIotMutex_Lock( &_connectionsList.mutex );

    IotMetricsTcpConnection_t * pFoundConnection = AwsIotList_FindFirstMatch( _connectionsList.pHead,
                                                                              IOT_METRICS_TCP_CONNECTION_OFFSET,
                                                                              pTcpConnectionHandle,
                                                                              _tcpConnectionMatch );

    if( pFoundConnection != NULL )
    {
        AwsIotList_Remove( &_connectionsList, &( pFoundConnection->link ), IOT_METRICS_TCP_CONNECTION_OFFSET );
        AwsIotMetrics_FreeTcpConnection( pFoundConnection );
    }

    AwsIotMutex_Unlock( &_connectionsList.mutex );
}

/*-----------------------------------------------------------*/

void IotMetrics_ProcessTcpConnections( IotMetricsListCallback_t tcpConnectionsCallback )
{
    /* If no callback function is provided, simply return. */
    if( tcpConnectionsCallback.function == NULL )
    {
        return;
    }

    AwsIotMutex_Lock( &_connectionsList.mutex );

    /* Execute the callback function. */
    tcpConnectionsCallback.function( tcpConnectionsCallback.param1, &_connectionsList );

    AwsIotMutex_Unlock( &_connectionsList.mutex );
}
