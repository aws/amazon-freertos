#include "iot_metrics.h"
#include "platform/aws_iot_threads.h"

static IotListDouble_t _connectionsList = IOT_LIST_DOUBLE_INITIALIZER;
static AwsIotMutex_t _mutex;

/* Compare function to identify the TCP connection data handle. */
static bool _tcpConnectionMatch( const IotLink_t * pLink,
                                 void * pHandle );

/*-----------------------------------------------------------*/

static bool _tcpConnectionMatch( const IotLink_t * pLink,
                                 void * pHandle )
{
    IotMetrics_Assert( pLink != NULL );
    IotMetrics_Assert( pHandle != NULL );

    return pHandle == IotLink_Container( IotMetricsTcpConnection_t, pLink, link )->pHandle;
}

/*-----------------------------------------------------------*/

BaseType_t IotMetrics_Init()
{
    BaseType_t result = pdFAIL;

    if( AwsIotMutex_Create( &_mutex ) )
    {
        IotListDouble_Create( &_connectionsList );
        result = pdPASS;
    }

    return result;
}

/*-----------------------------------------------------------*/

void IotMetrics_AddTcpConnection( IotMetricsTcpConnection_t * pTcpConnection )
{
    IotMetrics_Assert( pTcpConnection != NULL );
    IotMetrics_Assert( pTcpConnection->pHandle != NULL );

    AwsIotMutex_Lock( &_mutex );

    /* Only add if it doesn't exist in the connectionsList. */
    if( IotListDouble_FindFirstMatch( &_connectionsList,
                                      NULL,
                                      _tcpConnectionMatch,
                                      pTcpConnection->pHandle ) == NULL )
    {
        IotMetricsTcpConnection_t * pNewTcpConnection = AwsIotMetrics_MallocTcpConnection( sizeof( IotMetricsTcpConnection_t ) );

        if( pNewTcpConnection != NULL )
        {
            /* Copy TCP connection to the new one. */
            *pNewTcpConnection = *pTcpConnection;

            /* Insert to the list. */
            IotListDouble_InsertTail( &_connectionsList, &( pNewTcpConnection->link ) );
        }
    }

    AwsIotMutex_Unlock( &_mutex );
}

/*-----------------------------------------------------------*/

void IotMetrics_RemoveTcpConnection( void * pTcpConnectionHandle )
{
    IotMetrics_Assert( pTcpConnectionHandle != NULL );

    AwsIotMutex_Lock( &_mutex );

    IotLink_t * pFoundConnectionLink = IotListDouble_RemoveFirstMatch( &_connectionsList,
                                                                       NULL,
                                                                       _tcpConnectionMatch,
                                                                       pTcpConnectionHandle );

    if( pFoundConnectionLink != NULL )
    {
        AwsIotMetrics_FreeTcpConnection( IotLink_Container( IotMetricsTcpConnection_t, pFoundConnectionLink, link ) );
    }

    AwsIotMutex_Unlock( &_mutex );
}

/*-----------------------------------------------------------*/

void IotMetrics_ProcessTcpConnections( IotMetricsListCallback_t tcpConnectionsCallback )
{
    /* If no callback function is provided, simply return. */
    if( tcpConnectionsCallback.function == NULL )
    {
        return;
    }

    AwsIotMutex_Lock( &_mutex );

    /* Execute the callback function. */
    tcpConnectionsCallback.function( tcpConnectionsCallback.param1, &_connectionsList );

    AwsIotMutex_Unlock( &_mutex );
}
