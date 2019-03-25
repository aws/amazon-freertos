#include "platform/iot_metrics.h"
#include "platform/iot_threads.h"

static IotListDouble_t _connectionsList = IOT_LIST_DOUBLE_INITIALIZER;
static IotMutex_t _mutex;

/* Compare function to identify the TCP connection data handle. */
static bool _tcpConnectionMatch( const IotLink_t * pLink,
                                 void * pHandle );

/*-----------------------------------------------------------*/

static bool _tcpConnectionMatch( const IotLink_t * pLink,
                                 void * pHandle )
{
    ( void ) pLink; // Remove this line
    ( void ) pHandle; // Remove this line
    #if 0
    IotMetrics_Assert( pLink != NULL );
    IotMetrics_Assert( pHandle != NULL );

    return pHandle == IotLink_Container( IotMetricsTcpConnection_t, pLink, link )->pHandle;
    #endif
    return false;
}

/*-----------------------------------------------------------*/

bool IotMetrics_Init( void )
{
    bool result = false;

    if( IotMutex_Create( &_mutex, false ) )
    {
        IotListDouble_Create( &_connectionsList );
        result = true;
    }

    return result;
}

/*-----------------------------------------------------------*/

void IotMetrics_AddTcpConnection( IotMetricsTcpConnection_t * pTcpConnection )
{
    ( void ) pTcpConnection; // Remove this line
    #if 0
    IotMetrics_Assert( pTcpConnection != NULL );
    IotMetrics_Assert( pTcpConnection->pHandle != NULL );

    IotMutex_Lock( &_mutex );

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

    IotMutex_Unlock( &_mutex );
    #endif
}

/*-----------------------------------------------------------*/

void IotMetrics_RemoveTcpConnection( IotMetricsConnectionId_t pTcpConnectionHandle )
{
    ( void ) pTcpConnectionHandle; // Remove this line
    #if 0
    IotMetrics_Assert( pTcpConnectionHandle != NULL );

    IotMutex_Lock( &_mutex );

    IotLink_t * pFoundConnectionLink = IotListDouble_RemoveFirstMatch( &_connectionsList,
                                                                       NULL,
                                                                       _tcpConnectionMatch,
                                                                       pTcpConnectionHandle );

    if( pFoundConnectionLink != NULL )
    {
        AwsIotMetrics_FreeTcpConnection( IotLink_Container( IotMetricsTcpConnection_t, pFoundConnectionLink, link ) );
    }

    IotMutex_Unlock( &_mutex );
    #endif
}

/*-----------------------------------------------------------*/

void IotMetrics_ProcessTcpConnections( IotMetricsListCallback_t tcpConnectionsCallback )
{
    /* If no callback function is provided, simply return. */
    if( tcpConnectionsCallback.function == NULL )
    {
        return;
    }

    IotMutex_Lock( &_mutex );

    /* Execute the callback function. */
    tcpConnectionsCallback.function( tcpConnectionsCallback.param1, &_connectionsList );

    IotMutex_Unlock( &_mutex );
}
