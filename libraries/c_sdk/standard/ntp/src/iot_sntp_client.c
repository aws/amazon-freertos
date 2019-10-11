/*
 * Amazon FreeRTOS SNTP Client V1.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/**
 * @file iot_sntp_client.c
 * @brief AFR SNTP Client.
 **/

/* iot config file. */
#include "iot_config.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

/* Internal includes. */
#include "iot_sntp_date_time.h"
#include "iot_sntp_client.h"
#include "iot_sntp_config.h"

/* Library specific includes. */
#include "iot_taskpool.h"
#include "platform/iot_network_udp.h"
/* Error handling include. */
#include "private/iot_error.h"

/*-----------------------------------------------------------*/
/* SNTP minimum Packet size */
#define SNTP_PACKET_SIZE    ( ( uint16_t ) 48 )
/* Kiss of Death statum value */
#define SNTP_STRATUM_KOD    ( ( uint8_t ) 0x00 )

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_NETWORK
    #define LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_NETWORK
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
    #endif
#endif

#define LIBRARY_LOG_NAME    ( "NTP" )
#include "iot_logging_setup.h"

/*-----------------------------------------------------------*/

/* 64-bit NTP timestamp. */
#include "pack_struct_start.h"
struct _iotSntpTimestamp
{
    /* Number of seconds passed since Jan 1 1900, in big-endian format. */
    uint32_t seconds;

    /* Fractional time part in 1\0xFFFFFFFF th  of a second. */
    uint32_t fraction;
}
#include "pack_struct_end.h"
typedef struct _iotSntpTimestamp _iotSntpTimestamp_t;

/* NTP packet without auth */
#include "pack_struct_start.h"
struct _iotSntpPacket
{
    /* Flags. */
    uint8_t flags;

    /* Stratum of the clock. */
    uint8_t stratum;

    /* Maximum interval between successive messages, in log2 seconds. Note that the value is signed. */
    int8_t poll;

    /* Precision of the clock, in log2 seconds. Note that the value is signed. */
    int8_t precision;

    /* Round trip time to the primary reference source, in NTP short format. */
    int32_t rootDelay;

    /* Nominal error relative to the primary reference source. */
    int32_t rootDispersion;

    /* Reference identifier (either a 4 character string or an IP address). */
    int8_t referenceID[ 4 ];

    /* The time at which the clock was last set or corrected. */
    _iotSntpTimestamp_t referenceTimestamp;

    /* The time at which the request departed the client for the server. */
    _iotSntpTimestamp_t originateTimestamp;

    /* The time at which the request arrived at the server. */
    _iotSntpTimestamp_t receiveTimestamp;

    /* The time at which the reply departed the server for client. */
    _iotSntpTimestamp_t transmitTimestamp;
}
#include "pack_struct_end.h"
typedef struct _iotSntpPacket _iotSntpPacket_t;

typedef struct _iotSntpContext
{
    /* Opaque connection to NTP server */
    IotNetworkConnectionUdp_t * pNtpConnection;

    /* Poiter to network interface  */
    IotNetworkInterface_t const * pNetworkInterface;

    /* Index of the server to be used from the server list */
    uint8_t severIndex;

    /* 32 bit IP address of server last used */
    uint32_t lastServerAddr;

    /* time in ticks when the last request was sent */
    TickType_t lastSendTicks;

    /* ticks when last timestamp was received from the server */
    TickType_t lastTimeStampUpdateTicks;

    /* UTC seconds and MS last received from the server */
    time_t lastSeconds;
    time_t lastMSeconds;

    /* Timestamp sent in SNTP request ( for verification) */
    _iotSntpTimestamp_t transmitTimestamp;

    /* current retry period when failure happens */
    uint16_t retryPeriod;

    /* current retry count */
    uint16_t currentRetryCount;

    /* clock skew */
    double clockSkewSeconds;

    /* Application callback */
    IotSntpStatusCallback appCallback;
} _iotSntpContext_t;


/*-----------------------------------------------------------*/

void _sntpSendRequest( IotTaskPool_t pTaskPool,
                       IotTaskPoolJob_t pJob,
                       void * pUserContext );
void _sntpSyncRecv( IotTaskPool_t pTaskPool,
                    IotTaskPoolJob_t pJob,
                    void * pUserContext );
IotSntpError_t _sntpProcessResponse( _iotSntpPacket_t * pSntpPacket );
time_t _sntpGetCurrentTime( time_t * currentUTCMS );
size_t _sntpReplyRecv( _iotSntpContext_t * pContext );
void _sntpAsyncReceive( void * pConnection,
                        void * pContext );
/*-----------------------------------------------------------*/

static _iotSntpPacket_t _iotSntpPacket = { 0 };
static _iotSntpContext_t _iotSntpContext = { 0 };
static IotSntpConfig_t _iotSntpConfig = { 0 };
static IotTaskPoolJob_t _iotSntpSendJob = IOT_TASKPOOL_JOB_INITIALIZER;
static IotTaskPoolJob_t _iotSntpRecvJob = IOT_TASKPOOL_JOB_INITIALIZER;
static IotTaskPoolJobStorage_t _iotSntpSendJobStorage = { 0 };
static IotTaskPoolJobStorage_t _iotSntpRecvJobStorage = { 0 };
static bool _iotSntpInitialized = false;

/*-----------------------------------------------------------*/

static void _sntpPacketSwapFields( _iotSntpPacket_t * pSntpPacket )
{
    /* NTP messages are big-endian */
    pSntpPacket->rootDelay = SOCKETS_htonl( pSntpPacket->rootDelay );
    pSntpPacket->rootDispersion = SOCKETS_htonl( pSntpPacket->rootDispersion );

    pSntpPacket->referenceTimestamp.seconds = SOCKETS_htonl( pSntpPacket->referenceTimestamp.seconds );
    pSntpPacket->referenceTimestamp.fraction = SOCKETS_htonl( pSntpPacket->referenceTimestamp.fraction );

    pSntpPacket->originateTimestamp.seconds = SOCKETS_htonl( pSntpPacket->originateTimestamp.seconds );
    pSntpPacket->originateTimestamp.fraction = SOCKETS_htonl( pSntpPacket->originateTimestamp.fraction );

    pSntpPacket->receiveTimestamp.seconds = SOCKETS_htonl( pSntpPacket->receiveTimestamp.seconds );
    pSntpPacket->receiveTimestamp.fraction = SOCKETS_htonl( pSntpPacket->receiveTimestamp.fraction );

    pSntpPacket->transmitTimestamp.seconds = SOCKETS_htonl( pSntpPacket->transmitTimestamp.seconds );
    pSntpPacket->transmitTimestamp.fraction = SOCKETS_htonl( pSntpPacket->transmitTimestamp.fraction );
}
/*-----------------------------------------------------------*/

/*
 * @brief get the next NTP server from the list
 */
static const char * _sntpGetServerAddress()
{
    const char * server = NULL;
    uint8_t count = 0;

    /*
     * Get the next server if the address is not NULL
     * server address will be NULL  if is has been removed due to error
     * or connrection failures.
     */
    do
    {
        server = _iotSntpConfig.pActiveNTPServers[ _iotSntpContext.severIndex ];

        if( ++_iotSntpContext.severIndex == _iotSntpConfig.sntpServerCount )
        {
            _iotSntpContext.severIndex = 0;
        }

        count++;
    } while( ( server == NULL ) && ( count < _iotSntpConfig.sntpServerCount ) );

    return server;
}
/*-----------------------------------------------------------*/

static void _sntpPacketInit()
{
    time_t currentUTCMS = 0;
    time_t currentUTCSec = 0;

    memset( &_iotSntpPacket, '\0', sizeof( _iotSntpPacket ) );

    _iotSntpPacket.flags = 0xDB; /* value 0xDB : mode 3 (client), version 3, leap indicator unknown 3 */
    currentUTCSec = _sntpGetCurrentTime( &currentUTCMS );

    if( currentUTCSec == 0 )
    {
        /* If this is the first request after boot send ticks instead of timestamp. */
        _iotSntpPacket.transmitTimestamp.seconds = xTaskGetTickCount();
        _iotSntpPacket.transmitTimestamp.fraction = 0;
    }
    else
    {
        _iotSntpPacket.transmitTimestamp.seconds = ( uint32_t ) _sntpGetCurrentTime( &currentUTCMS ) + IOT_SNTP1900_TO_UNIX1970;
        /* For simplicity set franction to Milliseconds. */
        _iotSntpPacket.transmitTimestamp.fraction = ( uint32_t ) currentUTCMS;
    }

    /* Save sent timestamp. */
    _iotSntpContext.transmitTimestamp = _iotSntpPacket.transmitTimestamp;
    /* Transform the contents of the fields from native to big endian. */
    _sntpPacketSwapFields( &_iotSntpPacket );
}
/*-----------------------------------------------------------*/

static void _sntpSendRequest( IotTaskPool_t pTaskPool,
                              IotTaskPoolJob_t pJob,
                              void * pUserContext )
{
    size_t bytesSent = 0;
    IotTaskPoolError_t taskPoolError = IOT_TASKPOOL_SUCCESS;
    IotNetworkServerInfoUdp_t ntpServerInfo = IOT_NETWORK_SERVER_INFO_UDP_INITIALIZER;
    IotNetworkError_t networkError = IOT_NETWORK_SUCCESS;

    ( void ) pTaskPool;
    ( void ) pJob;
    ( void ) pUserContext;

    /* Get next SNTP server */
    ntpServerInfo.pHostName = _sntpGetServerAddress();

    /* Without SNTP server there is no point of having SNTP running */
    if( ntpServerInfo.pHostName == NULL )
    {
        IotLogError( " Please configure NTP server\n " );
    }
    else
    {
        ntpServerInfo.port = IOT_SNTP_PORT;
        ntpServerInfo.socketTimeout = _iotSntpConfig.sntpSocketTimeout;

        /* make sure earlier connection does not exist */
        if( _iotSntpContext.pNtpConnection != IOT_NETWORK_INVALID_CONNECTION )
        {
            _iotSntpContext.pNetworkInterface->destroy( _iotSntpContext.pNtpConnection );
            _iotSntpContext.pNtpConnection = IOT_NETWORK_INVALID_CONNECTION;
        }

        /* Create a new connection for every server request */
        networkError = _iotSntpContext.pNetworkInterface->create( ( void * ) &ntpServerInfo,
                                                                  NULL,
                                                                  ( void ** ) &_iotSntpContext.pNtpConnection );

        if( networkError == IOT_NETWORK_SUCCESS )
        {
            _sntpPacketInit();
            _iotSntpContext.lastSendTicks = xTaskGetTickCount();
            bytesSent = _iotSntpContext.pNetworkInterface->send( _iotSntpContext.pNtpConnection,
                                                                 ( void * ) &_iotSntpPacket,
                                                                 sizeof( _iotSntpPacket ) );

            if( bytesSent < sizeof( _iotSntpPacket ) )
            {
                IotLogError( "SNTP SEND Failure" );
                networkError = IOT_NETWORK_FAILURE;
            }
            else
            {
                /* reset retry count and period */
                _iotSntpContext.currentRetryCount = 0;
                _iotSntpContext.retryPeriod = IOT_SNTP_RETRY_PERIOD;

                /* If async receive is requested, setup async callback *
                 * Note: Since bind is not used, callback is setup after send. callback loop will fail
                 * if setup before send. send() will bind the socket if it is not bound.
                 */

                if( ( networkError == IOT_NETWORK_SUCCESS ) && ( _iotSntpConfig.sntpUseRecvCallback ) )
                {
                    /* set async callback */
                    networkError = _iotSntpContext.pNetworkInterface->setReceiveCallback( _iotSntpContext.pNtpConnection, _sntpAsyncReceive, ( void * ) &_iotSntpContext );

                    if( networkError != IOT_NETWORK_SUCCESS )
                    {
                        IotLogError( "Set Receive Callback Failed" );
                        networkError = IOT_NETWORK_FAILURE;
                    }
                }
                else
                {
                    /* Instantly Schedule Receive if not async callback set */
                    memset( &_iotSntpRecvJobStorage, 0, sizeof( _iotSntpRecvJobStorage ) );
                    IotTaskPool_CreateJob( _sntpSyncRecv, ( void * ) &_iotSntpContext, &_iotSntpRecvJobStorage, &_iotSntpRecvJob );
                    taskPoolError = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, _iotSntpRecvJob, IOT_TASKPOOL_JOB_HIGH_PRIORITY );

                    if( taskPoolError != IOT_TASKPOOL_SUCCESS )
                    {
                        IotLogError( "SNTP taskpool failed to schedule NTP recv job %d", taskPoolError );
                        networkError = IOT_NETWORK_FAILURE;
                    }
                }
            }
        }

        /* Note: this loop also handles failures after interface create success. */
        if( networkError != IOT_NETWORK_SUCCESS )
        {
            /* Destroy network connection */
            _iotSntpContext.pNetworkInterface->destroy( _iotSntpContext.pNtpConnection );
            _iotSntpContext.pNtpConnection = IOT_NETWORK_INVALID_CONNECTION;

            /* Check if more retries are remaining when polling is disabled */
            if( ( _iotSntpConfig.sntpPollingPeriod == 0 ) && ( _iotSntpContext.currentRetryCount >= IOT_SNTP_MAX_RETRIES ) )
            {
                IotLogWarn( "SNTP all retries are exhausted and polling is disabled" );

                /* Set SNTP initialzed to false so SNTP can be initialized again */
                _iotSntpInitialized = false;

                if( _iotSntpContext.appCallback != NULL )
                {
                    /* notify application of the failure as there are no more retries */
                    _iotSntpContext.appCallback( false );
                }
            }
            else
            {
                /* reschedule send job if it failed */
                memset( &_iotSntpSendJobStorage, 0, sizeof( _iotSntpSendJobStorage ) );
                IotTaskPool_CreateJob( _sntpSendRequest, ( void * ) &_iotSntpContext, &_iotSntpSendJobStorage, &_iotSntpSendJob );
                taskPoolError = IotTaskPool_ScheduleDeferred( IOT_SYSTEM_TASKPOOL, _iotSntpSendJob, ( _iotSntpContext.retryPeriod * 1000 ) );

                if( taskPoolError != IOT_TASKPOOL_SUCCESS )
                {
                    IotLogError( "SNTP taskpool failed to schedule NTP send job %d", taskPoolError );
                }

                /* Update retry period and count for the next iteration */
                _iotSntpContext.currentRetryCount++;

                if( ( _iotSntpConfig.sntpPollingPeriod != 0 ) && ( _iotSntpContext.retryPeriod < _iotSntpConfig.sntpPollingPeriod ) )
                {
                    _iotSntpContext.retryPeriod *= _iotSntpContext.currentRetryCount;
                }

                if( _iotSntpContext.retryPeriod > _iotSntpConfig.sntpPollingPeriod )
                {
                    _iotSntpContext.retryPeriod = _iotSntpConfig.sntpPollingPeriod;
                }
            }
        }
    }
}
/*-----------------------------------------------------------*/

static void _sntpAsyncReceive( void * pConnection,
                               void * pContext )
{
    IotTaskPool_t pTaskPool = IOT_TASKPOOL_INITIALIZER;
    IotTaskPoolJob_t pJob = IOT_TASKPOOL_JOB_INITIALIZER;

    ( void ) pConnection;

    /* call synch receive to process the reply and schedule next send */
    _sntpSyncRecv( pTaskPool, pJob, pContext );
}
/*-----------------------------------------------------------*/

static void _sntpSyncRecv( IotTaskPool_t pTaskPool,
                           IotTaskPoolJob_t pJob,
                           void * pUserContext )
{
    IotSntpError_t status = IOT_SNTP_ERROR_NONE;
    IotTaskPoolError_t taskPoolError = IOT_TASKPOOL_SUCCESS;

    /* Unused parameters. If used in future make sure to check for NULL */
    ( void ) pTaskPool;
    ( void ) pJob;

    /* Handle SNTP reply receive */
    status = _sntpReplyRecv( ( _iotSntpContext_t * ) pUserContext );

    /* Reset Task pool Request job */
    memset( &_iotSntpSendJobStorage, 0, sizeof( _iotSntpSendJobStorage ) );
    IotTaskPool_CreateJob( _sntpSendRequest, ( void * ) &_iotSntpContext, &_iotSntpSendJobStorage, &_iotSntpSendJob );

    /* if receive was not successfull schedule retry. */
    if( status != IOT_SNTP_ERROR_NONE )
    {
        taskPoolError = IotTaskPool_ScheduleDeferred( IOT_SYSTEM_TASKPOOL, _iotSntpSendJob, ( _iotSntpContext.retryPeriod * 1000 ) );

        /* Update retry period and count for the next iteration */
        _iotSntpContext.currentRetryCount++;

        if( ( _iotSntpConfig.sntpPollingPeriod != 0 ) && ( _iotSntpContext.retryPeriod < _iotSntpConfig.sntpPollingPeriod ) )
        {
            _iotSntpContext.retryPeriod *= _iotSntpContext.currentRetryCount;

            if( _iotSntpContext.retryPeriod > _iotSntpConfig.sntpPollingPeriod )
            {
                _iotSntpContext.retryPeriod = _iotSntpConfig.sntpPollingPeriod;
            }
        }
    }
    else
    {
        /* Schedule next refresh if polling is enabled. */
        if( _iotSntpConfig.sntpPollingPeriod != 0 )
        {
            taskPoolError = IotTaskPool_ScheduleDeferred( IOT_SYSTEM_TASKPOOL, _iotSntpSendJob, ( _iotSntpConfig.sntpPollingPeriod * 1000 ) );
        }
        else
        {
            _iotSntpInitialized = false;
        }

        /* indicate application that system has valid time */
        if( _iotSntpContext.appCallback != NULL )
        {
            _iotSntpContext.appCallback( true );
            /* reset appCallback to NULL */
            _iotSntpContext.appCallback = NULL;
        }
    }

    if( taskPoolError != IOT_TASKPOOL_SUCCESS )
    {
        IotLogError( "SNTP taskpool failed to schedule time fetch (%d)", taskPoolError );
    }
}
/*-----------------------------------------------------------*/

static IotSntpError_t _sntpReplyRecv( _iotSntpContext_t * pContext )
{
    uint8_t ntpRecvBuffer[ SNTP_PACKET_SIZE ];
    size_t recvBytes = 0;

    IOT_FUNCTION_ENTRY( IotSntpError_t, IOT_SNTP_ERROR_NONE );

    if( pContext == NULL )
    {
        IotLogError( " NTP Client: NULL context in reply" );
        IOT_SET_AND_GOTO_CLEANUP( IOT_SNTP_RESPONSE_ERROR );
    }

    if( pContext->pNtpConnection == IOT_NETWORK_INVALID_CONNECTION )
    {
        IotLogError( " NTP Client: Invalid connection inforamtion in reply" );
        IOT_SET_AND_GOTO_CLEANUP( IOT_SNTP_RESPONSE_ERROR );
    }

    recvBytes = pContext->pNetworkInterface->receive( _iotSntpContext.pNtpConnection,
                                                      ( void * ) &ntpRecvBuffer,
                                                      sizeof( ntpRecvBuffer ) );

    if( recvBytes == sizeof( ntpRecvBuffer ) )
    {
        IotLogDebug( "SNTP Received bytes: %u", recvBytes );
        status = _sntpProcessResponse( ( _iotSntpPacket_t * ) ntpRecvBuffer );
    }
    else
    {
        IotLogError( "SNTP Received failed: ( %u )", recvBytes );
        status = IOT_SNTP_RESPONSE_ERROR;
    }

    pContext->pNetworkInterface->destroy( _iotSntpContext.pNtpConnection );
    pContext->pNtpConnection = IOT_NETWORK_INVALID_CONNECTION;

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

void _sntpSetClockSkewSeconds( time_t currentSeconds,
                               time_t currentTicks )
{
    /* Time in Seconds since last update */
    time_t timeDiffSec = ( currentTicks - _iotSntpContext.lastTimeStampUpdateTicks ) / ( 1000 * pdMS_TO_TICKS( 1 ) );
    /* Expected current time */
    time_t expectedCurrentSec = _iotSntpContext.lastSeconds + timeDiffSec;

    if( _iotSntpConfig.sntpPollingPeriod != 0 )
    {
        /* Difference between expected current seconds and actual current seconds is clock skew */
        _iotSntpContext.clockSkewSeconds = ( ( double ) ( expectedCurrentSec - currentSeconds ) ) / _iotSntpConfig.sntpPollingPeriod;
        IotLogInfo( "Clock Error per second:%f seconds", _iotSntpContext.clockSkewSeconds );
    }
}
/*-----------------------------------------------------------*/

IotSntpError_t _sntpProcessResponse( _iotSntpPacket_t * pSntpPacket )
{
    IOT_FUNCTION_ENTRY( IotSntpError_t, IOT_SNTP_ERROR_NONE );
    time_t currentSeconds = 0;
    time_t currentMS = 0;
    time_t roundTripTime = 0;
    TickType_t currentTicks = xTaskGetTickCount();

    /* Transform the contents of the fields from big to native endian. */
    _sntpPacketSwapFields( pSntpPacket );

    if( pSntpPacket->stratum == SNTP_STRATUM_KOD )
    {
        /* TODO log ignore timestamp and restart polling */
        IotLogError( " Kiss of death received from the server, putting server on ignore list." );
        _iotSntpConfig.pActiveNTPServers[ _iotSntpContext.severIndex ] = NULL;
        IOT_SET_AND_GOTO_CLEANUP( IOT_SNTP_RESPONSE_ERROR );
    }

    /* make sure the reply has correct timestamp from the request */
    if( ( _iotSntpContext.transmitTimestamp.seconds != pSntpPacket->originateTimestamp.seconds ) ||
        ( _iotSntpContext.transmitTimestamp.fraction != pSntpPacket->originateTimestamp.fraction ) )
    {
        /* TODO log ignore timestamp and restart polling */
        IotLogError( " Incorrect Originate timestamp sent by the server" );
        IOT_SET_AND_GOTO_CLEANUP( IOT_SNTP_RESPONSE_ERROR );
    }

    /* convert ticks to MS */
    roundTripTime = ( ( currentTicks - _iotSntpContext.lastSendTicks ) )
                    / ( ( configTICK_RATE_HZ ) / 1000 );

    IotLogInfo( "SNTP Round Trip Time:%lld ms", roundTripTime );

    currentSeconds = pSntpPacket->transmitTimestamp.seconds - IOT_SNTP1900_TO_UNIX1970;
    /* fraction is 1/(2^32 -1) seconds. To find milliseconds last three digits are rounded off */
    currentMS = pSntpPacket->transmitTimestamp.fraction / 4294967;

    /* Add RTT delta */
    /* half of RTT is delay from packet travelling from server to client */
    roundTripTime /= 2;
    currentMS += roundTripTime;
    currentSeconds += currentMS / 1000;
    currentMS = currentMS % 1000;

    /* This is not the first time is fetched, find clock skew */
    if( _iotSntpContext.lastSeconds != 0 )
    {
        _sntpSetClockSkewSeconds( currentSeconds, currentTicks );
    }

    /* Save ticks and seconds for time calculation */
    _iotSntpContext.lastTimeStampUpdateTicks = currentTicks;
    _iotSntpContext.lastSeconds = currentSeconds;
    _iotSntpContext.lastMSeconds = currentMS;

    Iot_SntpPrintDateTime( "NTP time", currentSeconds, currentMS );

    IOT_FUNCTION_CLEANUP_BEGIN();
    IOT_FUNCTION_CLEANUP_END();
}
/*-----------------------------------------------------------*/


/**
 * @brief return current UTC seconds and milliseconds based on the last timestamp
 */
time_t _sntpGetCurrentTime( time_t * currentUTCMS )
{
    time_t elapsedTimeMS = 0;
    time_t currentUTCSec = 0;
    time_t elapsedTimeSec = 0;
    int32_t skew = 0;

    if( _iotSntpContext.lastSeconds != 0 )
    {
        /* Find Time Difference */
        elapsedTimeMS = xTaskGetTickCount() - _iotSntpContext.lastTimeStampUpdateTicks;
        elapsedTimeMS = elapsedTimeMS / pdMS_TO_TICKS( 1 );
        elapsedTimeSec = elapsedTimeMS / 1000;

        *currentUTCMS = _iotSntpContext.lastMSeconds + ( elapsedTimeMS % 1000 );
        currentUTCSec = _iotSntpContext.lastSeconds
                        + ( elapsedTimeMS / 1000 )
                        + ( *currentUTCMS / 1000 );
        *currentUTCMS = *currentUTCMS % 1000;

        /* clock skew correction */
        skew = ( int32_t ) ( ( double ) _iotSntpContext.clockSkewSeconds * elapsedTimeSec );
        currentUTCSec = currentUTCSec - skew;
    }

    return currentUTCSec;
}
/*-----------------------------------------------------------*/

/*
 * @brief setup SNTP context, must be called from init functions
 */
static void _sntpSetupContext()
{
    /* start from server 0 */
    _iotSntpContext.severIndex = 0;

    /* set connection to invalid */
    _iotSntpContext.pNtpConnection = IOT_NETWORK_INVALID_CONNECTION;
    /* set interface to UDP */
    _iotSntpContext.pNetworkInterface = IOT_NETWORK_INTERFACE_UDP;

    /* initialize retry timeout count */
    _iotSntpContext.retryPeriod = IOT_SNTP_RETRY_PERIOD;
    _iotSntpContext.currentRetryCount = 0;

    /* initialize skew to 0 */
    _iotSntpContext.clockSkewSeconds = 0;
}
/*-----------------------------------------------------------*/

/*
 * @brief setup SNTP config from configuration in header file
 */
static IotSntpError_t _sntpInitStatic()
{
    IOT_FUNCTION_ENTRY( IotSntpError_t, IOT_SNTP_ERROR_NONE );
    uint8_t validNtpServers = 0;

    _iotSntpConfig.sntpPollingPeriod = IOT_SNTP_POLLING_PERIOD;
    _iotSntpConfig.sntpUseRecvCallback = SNTP_USE_RECEIVE_CALLBACK;

    /* Polling period has to be more than the minumum. Zero period  means polling is disabled. */
    if( ( _iotSntpConfig.sntpPollingPeriod < IOT_SNTP_MIN_POLLING_PERIOD ) && ( _iotSntpConfig.sntpPollingPeriod != 0 ) )
    {
        IotLogError( "SNTP Polling period set below minimum allowed" );
        IOT_SET_AND_GOTO_CLEANUP( IOT_SNTP_INIT_FAILED );
    }

    _iotSntpConfig.sntpServerCount = sizeof( pNTPServers ) / sizeof( pNTPServers[ 0 ] );

    /* Copy references to the server addresses */
    for( int i = 0; i < _iotSntpConfig.sntpServerCount; i++ )
    {
        if( pNTPServers[ i ][ 0 ] != 0 )
        {
            _iotSntpConfig.pActiveNTPServers[ i ] = pNTPServers[ i ];
            validNtpServers++;
        }
        else
        {
            IotLogWarn( "Empty NTP server address found in config" );
            _iotSntpConfig.pActiveNTPServers[ i ] = NULL;
        }
    }

    /* Without the server SNTP will not start. */
    if( validNtpServers == 0 )
    {
        IotLogError( "SNTP server not configured" );
        IOT_SET_AND_GOTO_CLEANUP( IOT_SNTP_INIT_FAILED );
    }

    _iotSntpConfig.sntpSocketTimeout = IOT_SNTP_SOCKET_TIMEOUT;
    _sntpSetupContext();

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}
/*-----------------------------------------------------------*/

/*
 * @brief setup SNTP config from configuration in header file
 */
static IotSntpError_t _sntpInit( IotSntpConfig_t * config )
{
    IOT_FUNCTION_ENTRY( IotSntpError_t, IOT_SNTP_ERROR_NONE );

    _iotSntpConfig.sntpPollingPeriod = config->sntpPollingPeriod;
    _iotSntpConfig.sntpUseRecvCallback = config->sntpUseRecvCallback;

    /* Polling period has to be more than the minumum. Zero period  means polling is disabled. */
    if( ( _iotSntpConfig.sntpPollingPeriod < IOT_SNTP_MIN_POLLING_PERIOD ) && ( _iotSntpConfig.sntpPollingPeriod != 0 ) )
    {
        IotLogError( "SNTP Polling period set below minimum allowed" );
        IOT_SET_AND_GOTO_CLEANUP( IOT_SNTP_INIT_FAILED );
    }

    _iotSntpConfig.sntpServerCount = config->sntpServerCount;

    /* Sever count check */
    if( _iotSntpConfig.sntpServerCount > IOT_MAX_SNTP_SERVERS )
    {
        IotLogError( ( "SNTP Server count must be no more than: %d", IOT_MAX_SNTP_SERVERS ) );
        IOT_SET_AND_GOTO_CLEANUP( IOT_SNTP_INIT_FAILED );
    }

    /* Without the server SNTP will not start. */
    if( _iotSntpConfig.sntpServerCount <= 0 )
    {
        IotLogError( "SNTP Server not configured" );
        IOT_SET_AND_GOTO_CLEANUP( IOT_SNTP_INIT_FAILED );
    }

    /* Copy references to the server addresses */
    for( int i = 0; i < _iotSntpConfig.sntpServerCount; i++ )
    {
        _iotSntpConfig.pActiveNTPServers[ i ] = config->pActiveNTPServers[ i ];
    }

    _iotSntpConfig.sntpSocketTimeout = config->sntpSocketTimeout;

    _sntpSetupContext();

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*
 * @brief Create SNTP taskpool send and receve jobs
 */
IotSntpError_t _iotSntpCreateTaskpoolJobs()
{
    IotTaskPoolError_t taskError = IOT_TASKPOOL_SUCCESS;

    IOT_FUNCTION_ENTRY( IotSntpError_t, IOT_SNTP_ERROR_NONE );

    /* Initialize all taskpool jobs */
    taskError = IotTaskPool_CreateJob( _sntpSendRequest, ( void * ) &_iotSntpContext, &_iotSntpSendJobStorage, &_iotSntpSendJob );

    if( taskError != IOT_TASKPOOL_SUCCESS )
    {
        IotLogError( ( "TaskPool Create Failed for SNTP Send" ) );
        IOT_SET_AND_GOTO_CLEANUP( IOT_SNTP_INIT_FAILED );
    }

    IotTaskPool_CreateJob( _sntpSyncRecv, ( void * ) &_iotSntpContext, &_iotSntpRecvJobStorage, &_iotSntpRecvJob );

    if( taskError != IOT_TASKPOOL_SUCCESS )
    {
        IotLogError( ( "TaskPool Create Failed for SNTP Recv" ) );
        IOT_SET_AND_GOTO_CLEANUP( IOT_SNTP_INIT_FAILED );
    }

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}
/*-----------------------------------------------------------*/

IotSntpError_t Iot_SntpGetTimeOfDay( IotSntpTimeval_t * ptv )
{
    time_t currentUTCMS;
    IotSntpError_t status = IOT_SNTP_ERROR_NONE;

    ptv->tv_sec = _sntpGetCurrentTime( &currentUTCMS );
    ptv->tv_usec = currentUTCMS * 1000;

    if( ptv->tv_sec == 0 )
    {
        /* time is not yet obtained from NTP server */
        status = IOT_SNTP_INVALID_TIME;
    }

    return status;
}
/*-----------------------------------------------------------*/


IotSntpError_t Iot_SntpStart( IotSntpStatusCallback sntpStatusCallback,
                              IotSntpConfig_t * pSntpConfig )
{
    IotTaskPoolError_t taskPoolError = IOT_TASKPOOL_SUCCESS;

    IOT_FUNCTION_ENTRY( IotSntpError_t, IOT_SNTP_ERROR_NONE );

    /* SNTP module should be initialized only once */
    if( _iotSntpInitialized == true )
    {
        IotLogWarn( "SNTP already initialized" );
        IOT_SET_AND_GOTO_CLEANUP( IOT_SNTP_ERROR_NONE );
    }
    else
    {
        _iotSntpInitialized = true;
    }

    /* If config is not passed use static config */
    if( pSntpConfig == NULL )
    {
        status = _sntpInitStatic();
    }
    else
    {
        /* Use application provided config */
        status = _sntpInit( pSntpConfig );
    }

    if( status != IOT_SNTP_ERROR_NONE )
    {
        IOT_SET_AND_GOTO_CLEANUP( status );
    }

    status = _iotSntpCreateTaskpoolJobs();

    if( status != IOT_SNTP_ERROR_NONE )
    {
        IOT_SET_AND_GOTO_CLEANUP( status );
    }

    taskPoolError = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, _iotSntpSendJob, IOT_TASKPOOL_JOB_HIGH_PRIORITY );

    if( taskPoolError != IOT_TASKPOOL_SUCCESS )
    {
        IotLogError( ( "TaskPool Failed" ) );
        IOT_SET_AND_GOTO_CLEANUP( IOT_SNTP_TASKPOOL_FAILED );
    }

    /* setup callback function  to report */
    _iotSntpContext.appCallback = sntpStatusCallback;

    IOT_FUNCTION_CLEANUP_BEGIN();

    if( status != IOT_SNTP_ERROR_NONE )
    {
        _iotSntpInitialized = false;
    }

    IOT_FUNCTION_CLEANUP_END();
}
/*-----------------------------------------------------------*/
