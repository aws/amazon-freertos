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
 * Server.cpp
 * by Mark Gates <mgates@nlanr.net>
 *     Ajay Tirumala (tirumala@ncsa.uiuc.edu>.
 * -------------------------------------------------------------------
 * A server thread is initiated for each connection accept() returns.
 * Handles sending and receiving data, and then closes socket.
 * Changes to this version : The server can be run as a daemon
 * ------------------------------------------------------------------- */

#define HEADERS()

#include "headers.h"
#include "Server.hpp"
#include "Iperf_list.h"
#include "Extractor.h"
#include "Reporter.h"
#include "iperf_locale.h"

/* -------------------------------------------------------------------
 * Stores connected socket and socket info.
 * ------------------------------------------------------------------- */
Server::Server( thread_Settings *inSettings ) {
    IPERF_DEBUGF( SERVER_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Creating a new Server.\n" ) );

    mSettings = inSettings;
    mBuf = NULL;

    // initialize buffer
    mBuf = (char*) malloc( mSettings->mBufLen );
    FAIL_errno( mBuf == NULL, ( "No memory for buffer Server::mBuf.\n" ), mSettings );
	IPERF_DEBUGF( MEMALLOC_DEBUG | IPERF_DBG_TRACE, IPERF_MEMALLOC_MSG( mBuf, mSettings->mBufLen ) );
}

/* -------------------------------------------------------------------
 * Destructor close socket.
 * ------------------------------------------------------------------- */
Server::~Server() {
    IPERF_DEBUGF( SERVER_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Destroying Server.\n" ) );

    if ( mSettings->mSock != (signed) INVALID_SOCKET ) {
        IPERF_DEBUGF( SERVER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Closing Server socket %d.\n", mSettings->mSock ) );

        int rc = iperf_close( mSettings->mSock );
        WARN_errno( rc == SOCKET_ERROR, ( "Socket close failed.\n" ) );
        mSettings->mSock = INVALID_SOCKET;
    }
    
	IPERF_DEBUGF( MEMFREE_DEBUG | IPERF_DBG_TRACE, IPERF_MEMFREE_MSG( mBuf ) );
    FREE_PTR( mBuf );
} // end ~Server

#ifndef NO_INTERRUPTS
void Server::Sig_Int( int inSigno ) {
}
#endif /* NO_INTERRUPTS */

/* ------------------------------------------------------------------- 
 * Receive data from the (connected) socket.
 * Sends termination flag several times at the end. 
 * Does not close the socket. 
 * ------------------------------------------------------------------- */ 
void Server::Run( void ) {
    IPERF_DEBUGF( SERVER_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Running the Server.\n" ) );

    long currLen; 
    max_size_t totLen = 0;
    struct UDP_datagram* mBuf_UDP  = (struct UDP_datagram*) mBuf; 

    ReportStruct *reportstruct = NULL;
    reportstruct = (ReportStruct*) malloc( sizeof( ReportStruct ) );
    if ( reportstruct != NULL ) {
		IPERF_DEBUGF( MEMALLOC_DEBUG | IPERF_DBG_TRACE, IPERF_MEMALLOC_MSG( reportstruct, sizeof( ReportStruct ) ) );
	
        reportstruct->packetID = 0;
        mSettings->reporthdr = InitReport( mSettings );
        IPERF_DEBUGF( SERVER_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Server is ready to receive.\n" ) );

        do {
            // perform read 
            currLen = recv( mSettings->mSock, mBuf, mSettings->mBufLen, 0 ); 

            if ( isUDP( mSettings ) ) {
                // read the datagram ID and sentTime out of the buffer 
                reportstruct->packetID = ntohl( mBuf_UDP->id ); 
                reportstruct->sentTime.tv_sec = ntohl( mBuf_UDP->tv_sec  );
                reportstruct->sentTime.tv_usec = ntohl( mBuf_UDP->tv_usec ); 
                reportstruct->packetLen = currLen;
                gettimeofday( &(reportstruct->packetTime), NULL );
                IPERF_DEBUGF_COUNTER( SERVER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Server read UDP packet from socket %d. [Packet ID=%d] [Packet length=%lu]\n", mSettings->mSock, reportstruct->packetID, currLen ) );
            } else {
		        totLen += currLen;
                IPERF_DEBUGF_COUNTER( SERVER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Server read TCP packet from socket %d. [Packet ID=%d] [Packet length=%lu] [Total length=%lu]\n", mSettings->mSock, reportstruct->packetID, currLen, (long unsigned) totLen ) );
	        }
        
            // terminate when datagram begins with negative index 
            // the datagram ID should be correct, just negated 
            if ( reportstruct->packetID < 0 ) {
                reportstruct->packetID = -reportstruct->packetID;
                currLen = -1; 
            }

            if ( isUDP (mSettings)) {
                IPERF_DEBUGF_COUNTER( CLIENT_DEBUG | IPERF_DBG_TRACE, ( "Server is reporting packet %d.\n", reportstruct->packetID ) );
                ReportPacket( mSettings->reporthdr, reportstruct );
            } else if ( !isUDP (mSettings) && mSettings->mInterval > 0) {
                reportstruct->packetLen = currLen;
                gettimeofday( &(reportstruct->packetTime), NULL );
                IPERF_DEBUGF_COUNTER( TIME_DEBUG | IPERF_DBG_TRACE, ( "Packet time for packet %d is %ld sec %ld usec.\n", reportstruct->packetID, (long)reportstruct->packetTime.tv_sec, (long)reportstruct->packetTime.tv_usec ) );
                IPERF_DEBUGF_COUNTER( CLIENT_DEBUG | IPERF_DBG_TRACE, ( "Server is reporting packet %d.\n", reportstruct->packetID ) );
                ReportPacket( mSettings->reporthdr, reportstruct );
            }
            IPERF_DEBUGF_COUNTER_INCREMENT();
        } while ( currLen > 0 ); 
        
        // stop timing 
        IPERF_DEBUGF( SERVER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Server has finished receiving packets.\n" ) );
        gettimeofday( &(reportstruct->packetTime), NULL );
        IPERF_DEBUGF( SERVER_DEBUG | TIME_DEBUG | IPERF_DBG_TRACE, ( "Server has stopped timing. Stop time is %ld sec %ld us.\n", (long)reportstruct->packetTime.tv_sec, (long)reportstruct->packetTime.tv_usec ) );

        if ( !isUDP (mSettings)) {
            if(0.0 == mSettings->mInterval) {
                reportstruct->packetLen = totLen;
            }
            IPERF_DEBUGF( SERVER_DEBUG | IPERF_DBG_TRACE, ( "Server is reporting packet %d.\n", reportstruct->packetID ) );
            ReportPacket( mSettings->reporthdr, reportstruct );
	    }
        IPERF_DEBUGF( SERVER_DEBUG | IPERF_DBG_TRACE, ( "Server is closing report. [reporterindex=%d] [agentindex=%d].\n", mSettings->reporthdr->reporterindex, mSettings->reporthdr->agentindex ) );
        CloseReport( mSettings->reporthdr, reportstruct );
        
        // send an acknowledgment back only if we're NOT receiving multicast
        if ( isUDP( mSettings ) && !isMulticast( mSettings ) ) {
            // send back an acknowledgment of the terminating datagram
            IPERF_DEBUGF( SERVER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Server is sending a UDP acknowledgement to the client.\n" ) );
            write_UDP_AckFIN( ); 
        }
    } else {
        printf( "Out of memory! Closing server thread.\n");
        FAIL(1, ( "Out of memory! Closing server thread.\n" ), mSettings);
    }

    IPERF_DEBUGF( SERVER_DEBUG | MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Server is locking clients mutex.\n" ) );
    Mutex_Lock( &clients_mutex );
    Iperf_delete( &(mSettings->peer), &clients );
    IPERF_DEBUGF( SERVER_DEBUG | MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Server is unlocking clients mutex.\n" ) );
    Mutex_Unlock( &clients_mutex );

    IPERF_DEBUGF( MEMFREE_DEBUG | IPERF_DBG_TRACE, IPERF_MEMFREE_MSG( reportstruct ) );
    FREE_PTR( reportstruct );
    EndReport( mSettings->reporthdr );
} // end Server::Run

/* ------------------------------------------------------------------- 
 * Send an AckFIN (a datagram acknowledging a FIN) on the socket, 
 * then select on the socket for some time. If additional datagrams 
 * come in, probably our AckFIN was lost and they are re-transmitted 
 * termination datagrams, so re-transmit our AckFIN. 
 * ------------------------------------------------------------------- */ 
void Server::write_UDP_AckFIN( ) {
    int rc; 

    fd_set readSet; 
    FD_ZERO( &readSet ); 

    struct timeval timeout; 

    int count = 0; 
    while ( count < 10 ) {
        count++; 

        UDP_datagram *UDP_Hdr;
        server_hdr *hdr;

        UDP_Hdr = (UDP_datagram*) mBuf;

        if ( mSettings->mBufLen > (int) ( sizeof( UDP_datagram )
                                          + sizeof( server_hdr ) ) ) {
            Transfer_Info *stats = GetReport( mSettings->reporthdr );
            hdr = (server_hdr*) (UDP_Hdr+1);

            hdr->flags        = htonl( HEADER_VERSION );
            hdr->total_len1   = htonl( (long) (stats->TotalLen >> 32) );
            hdr->total_len2   = htonl( (long) (stats->TotalLen & 0xFFFFFFFF) );
            hdr->stop_sec     = htonl( (long) stats->endTime );
            hdr->stop_usec    = htonl( (long)((stats->endTime - (long)stats->endTime)
                                              * rMillion));
            hdr->error_cnt    = htonl( stats->cntError );
            hdr->outorder_cnt = htonl( stats->cntOutofOrder );
            hdr->datagrams    = htonl( stats->cntDatagrams );
            hdr->jitter1      = htonl( (long) stats->jitter );
            hdr->jitter2      = htonl( (long) ((stats->jitter - (long)stats->jitter) 
                                               * rMillion) );
        }

        // write data 
        rc = iperf_write( mSettings->mSock, mBuf, mSettings->mBufLen );

        // wait until the socket is readable, or our timeout expires 
        FD_SET( mSettings->mSock, &readSet ); 
        timeout.tv_sec  = 1; 
        timeout.tv_usec = 0; 

        rc = iperf_select( mSettings->mSock+1, &readSet, NULL, NULL, &timeout );
        FAIL_errno( rc == SOCKET_ERROR, ( "Socket select failed.\n" ), mSettings );

        if ( rc == 0 ) {
            // select timed out 
            return; 
        } else {
            // socket ready to read 
            rc = iperf_read( mSettings->mSock, mBuf, mSettings->mBufLen );
//            WARN_errno( rc < 0, ( "Unexpected value returned from socket read operation: %d.\n", rc ) );
            if ( rc <= 0 ) {
                // Connection closed or errored
                // Stop using it.
                return;
            }
        } 
    } 

    fprintf( stderr, warn_ack_failed, mSettings->mSock, count ); 
} // end Server::write_UDP_AckFIN 
