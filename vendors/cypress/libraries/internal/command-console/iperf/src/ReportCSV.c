/*--------------------------------------------------------------- 
 * Copyright (c) 1999,2000,2001,2002,2003                              
 * The Board of Trustees of the University of Illinois            
 * All Rights Reserved.                                           
 *--------------------------------------------------------------- 
 * Permission is hereby granted, free of charge, to any person    
 * obtaining a copy of this software (Iperf) and associated       
 * documentation files (the "Software"), to deal in the Software  
 * without restriction, including without limitation the          
 * rights to use, copy, modify, merge, publish, distribute,        
 * sublicense, and/or sell copies of the Software, and to permit     
 * persons to whom the Software is furnished to do
 * so, subject to the following conditions: 
 *
 *     
 * Redistributions of source code must retain the above 
 * copyright notice, this list of conditions and 
 * the following disclaimers. 
 *
 *     
 * Redistributions in binary form must reproduce the above 
 * copyright notice, this list of conditions and the following 
 * disclaimers in the documentation and/or other materials 
 * provided with the distribution. 
 * 
 *     
 * Neither the names of the University of Illinois, NCSA, 
 * nor the names of its contributors may be used to endorse 
 * or promote products derived from this Software without
 * specific prior written permission. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTIBUTORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 * ________________________________________________________________
 * National Laboratory for Applied Network Research 
 * National Center for Supercomputing Applications 
 * University of Illinois at Urbana-Champaign 
 * http://www.ncsa.uiuc.edu
 * ________________________________________________________________ 
 *
 * ReportCSV.c
 * by Kevin Gibbs <kgibbs@nlanr.net>
 *
 * ________________________________________________________________ */

#include "headers.h"
#include "Settings.hpp"
#include "iperf_util.h"
#include "Reporter.h"
#include "report_CSV.h"
#include "iperf_locale.h"

void CSV_timestamp( char *timestamp, int length );
 
void CSV_stats( Transfer_Info *stats ) {
    // $TIMESTAMP,$ID,$INTERVAL,$BYTE,$SPEED,$JITTER,$LOSS,$PACKET,$%LOSS
    max_size_t speed = (max_size_t)(((double)stats->TotalLen * 8.0) / (stats->endTime - stats->startTime));
    char timestamp[16];
    CSV_timestamp( timestamp, sizeof(timestamp) );
    if ( stats->mUDP != (char)kMode_Server ) {
        // TCP Reporting
        printf( reportCSV_bw_format, 
                timestamp, 
                (stats->reserved_delay == NULL ? ",,," : stats->reserved_delay),
                stats->transferID, 
                stats->startTime, 
                stats->endTime, 
                stats->TotalLen, 
                speed);
    } else {
        // UDP Reporting
        printf( reportCSV_bw_jitter_loss_format, 
                timestamp, 
                (stats->reserved_delay == NULL ? ",,," : stats->reserved_delay),
                stats->transferID, 
                stats->startTime, 
                stats->endTime, 
                stats->TotalLen, 
                speed,
                stats->jitter*1000.0, 
                stats->cntError, 
                stats->cntDatagrams,
                (100.0 * stats->cntError) / stats->cntDatagrams, stats->cntOutofOrder );
    }
    if ( stats->free == 1 && stats->reserved_delay != NULL ) {
        free( stats->reserved_delay );
    }
} // end CSV_stats

void *CSV_peer( Connection_Info *stats, int ID ) {
    // copy the inet_ntop into temp buffers, to avoid overwriting
    char local_addr[ REPORT_ADDRLEN ];
    char remote_addr[ REPORT_ADDRLEN ];
    char *buf = (char*) malloc( REPORT_ADDRLEN * 2 + 10 );
    FAIL( buf == NULL, ( "No memory for buffer buf.\n" ), NULL );
    IPERF_DEBUGF( MEMALLOC_DEBUG | IPERF_DBG_TRACE, IPERF_MEMALLOC_MSG( buf, REPORT_ADDRLEN * 2 + 10 ) );
    struct sockaddr *local = ((struct sockaddr*)&stats->local);
    struct sockaddr *peer = ((struct sockaddr*)&stats->peer);

    if ( local->sa_family == AF_INET ) {
        inet_ntop( AF_INET, &((struct sockaddr_in*)local)->sin_addr, 
                   local_addr, REPORT_ADDRLEN);
    }
#ifdef HAVE_IPV6
      else {
        inet_ntop( AF_INET6, &((struct sockaddr_in6*)local)->sin6_addr, 
                   local_addr, REPORT_ADDRLEN);
    }
#endif /* HAVE_IPV6 */

    if ( peer->sa_family == AF_INET ) {
        inet_ntop( AF_INET, &((struct sockaddr_in*)peer)->sin_addr, 
                   remote_addr, REPORT_ADDRLEN);
    }
#ifdef HAVE_IPV6
      else {
        inet_ntop( AF_INET6, &((struct sockaddr_in6*)peer)->sin6_addr, 
                   remote_addr, REPORT_ADDRLEN);
    }
#endif /* HAVE_IPV6 */

    snprintf(buf, REPORT_ADDRLEN*2+10, reportCSV_peer, 
             local_addr, ( local->sa_family == AF_INET ?
                          ntohs(((struct sockaddr_in*)local)->sin_port) :
#ifdef HAVE_IPV6
                          ntohs(((struct sockaddr_in6*)local)->sin6_port)),
#else
                          0),
#endif /* HAVE_IPV6 */
            remote_addr, ( peer->sa_family == AF_INET ?
                          ntohs(((struct sockaddr_in*)peer)->sin_port) :
#ifdef HAVE_IPV6
                          ntohs(((struct sockaddr_in6*)peer)->sin6_port)));
#else
                          0));
#endif /* HAVE_IPV6 */
    return buf;
} // end CSV_peer

void CSV_serverstats( Connection_Info *conn, Transfer_Info *stats ) {
    stats->reserved_delay = CSV_peer( conn, stats->transferID );
    stats->free = 1;
    CSV_stats( stats );
} // end CSV_serverstats

void CSV_timestamp( char *timestamp, int length ) {
	#if 0
#if !defined(WICED) || defined(RTOS_NuttX)
    time_t times;
    struct tm *timest;
    times = time( NULL );
    timest = localtime( &times );
    strftime( timestamp, length,"%Y%m%d%H%M%S", timest );
#else
    #define MS_PER_SECOND (1000)
    #define SECONDS_PER_MINUTE (60)
    #define MINUTES_PER_HOUR (60)
    #define HOURS_PER_DAY (24)
    #define DAYS_PER_WEEK (7)
    #define DAYS_PER_MONTH (31)

    static const uint32_t ms_per_second = MS_PER_SECOND;
    static const uint32_t ms_per_minute = ( MS_PER_SECOND * SECONDS_PER_MINUTE );
    static const uint32_t ms_per_hour = ( MS_PER_SECOND * SECONDS_PER_MINUTE * MINUTES_PER_HOUR );
    static const uint32_t ms_per_day = ( MS_PER_SECOND * SECONDS_PER_MINUTE * MINUTES_PER_HOUR * HOURS_PER_DAY );
    static const uint32_t sec_per_month = ( SECONDS_PER_MINUTE * MINUTES_PER_HOUR * HOURS_PER_DAY * DAYS_PER_MONTH );

    struct tm timest;
    wwd_time_t time;

    time = host_rtos_get_time( );

    timest.tm_isdst = -1; /* daylight saving information not available */
    timest.tm_year = ( 2012 - 1900 ); /* 2012 */

    /* Note that wiced_time_t is only 32 bits */
    /* This should be set to a real valid date, relative to January 1, 2012 */
    timest.tm_yday = ( time / ms_per_day );
    timest.tm_wday = ( ( time / ms_per_day ) % DAYS_PER_WEEK ); /* Sunday */
    timest.tm_mon = 0 + ( ( time / 1000 ) / sec_per_month );
    timest.tm_mday = 1 + ( time / ms_per_day );
    timest.tm_hour = ( time / ms_per_hour ) % HOURS_PER_DAY;
    timest.tm_min = ( time / ms_per_minute ) % MINUTES_PER_HOUR;
    timest.tm_sec = ( time / ms_per_second ) % SECONDS_PER_MINUTE;

    sprintf( timestamp, "%4d%2d%2d%2d%2d%2d",
             timest.tm_year,
             timest.tm_mon,
             timest.tm_mday,
             timest.tm_hour,
             timest.tm_min,
             timest.tm_sec );
#endif /* WICED */
#endif
} // end CSV_timestamp
