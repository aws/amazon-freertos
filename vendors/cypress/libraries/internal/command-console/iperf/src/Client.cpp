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
 * Client.cpp
 * by Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * A client thread initiates a connect to the server and handles
 * sending and receiving data, then closes the socket.
 * ------------------------------------------------------------------- */

#include "headers.h"
#include "Client.hpp"
#include "iperf_thread.h"
#include "SocketAddr.h"
#include "PerfSocket.hpp"
#include "Extractor.h"
#include "delay.hpp"
#include "iperf_util.h"
#include "iperf_locale.h"
#include "Reporter.h"

/* -------------------------------------------------------------------
 * Store server hostname, optionally local hostname, and socket info.
 * ------------------------------------------------------------------- */
Client::Client( thread_Settings *inSettings ) {
    IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Creating new client.\n" ) );

    mSettings = inSettings;
    mBuf = NULL;

    // initialize buffer
    mBuf = (char*) malloc( mSettings->mBufLen );
	FAIL_errno( mBuf == NULL, ( "No memory for Client::mBuf.\n" ), inSettings );
    IPERF_DEBUGF( MEMALLOC_DEBUG | IPERF_DBG_TRACE, IPERF_MEMALLOC_MSG( mBuf, mSettings->mBufLen ) );
	
    IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Filling client buffer with data.\n" ) );
    pattern( mBuf, mSettings->mBufLen );
#ifndef NO_FILE_IO
    if ( isFileInput( mSettings ) ) {
        if ( !isSTDIN( mSettings ) )
            Extractor_Initialize( mSettings->mFileName, mSettings->mBufLen, mSettings );
        else
            Extractor_InitializeFile( stdin, mSettings->mBufLen, mSettings );

        if ( !Extractor_canRead( mSettings ) ) {
            fprintf(stderr, "Cannot read extractor... unsetting file input.\n");
            unsetFileInput( mSettings );
        }
    }
#endif /* NO_FILE_IO */

    // connect
    Connect( );

    if ( isReport( inSettings ) ) {
        IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Client is reporting settings.\n" ) );
        ReportSettings( inSettings );
        if ( mSettings->multihdr && isMultipleReport( inSettings ) ) {
            mSettings->multihdr->report->connection.peer = mSettings->peer;
            mSettings->multihdr->report->connection.size_peer = mSettings->size_peer;
            mSettings->multihdr->report->connection.local = mSettings->local;
            SockAddr_setPortAny( &mSettings->multihdr->report->connection.local );
            mSettings->multihdr->report->connection.size_local = mSettings->size_local;
        }
    }
} // end Client

/* -------------------------------------------------------------------
 * Delete memory (hostname strings).
 * ------------------------------------------------------------------- */
Client::~Client() {
    IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Destroying Client.\n" ) );

    if ( mSettings->mSock != (signed) INVALID_SOCKET ) {
        IPERF_DEBUGF( CLIENT_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Closing Client socket %d.\n", mSettings->mSock ) );
        int rc = iperf_close( mSettings->mSock );
        WARN_errno( rc == SOCKET_ERROR, ( "Socket close failed.\n" ) );
        mSettings->mSock = INVALID_SOCKET;
    }
    IPERF_DEBUGF( MEMFREE_DEBUG | IPERF_DBG_TRACE, IPERF_MEMFREE_MSG( mBuf ) );
    FREE_PTR( mBuf );
} // end ~Client

const double kSecs_to_usecs = 1e6; 
const int    kBytes_to_Bits = 8; 

void Client::RunTCP( void ) {
    IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "TCP client running.\n" ) );

	/**
	 * BUGFIX: This should not be declared unsigned.
	 * @see http://sourceforge.net/tracker/?func=detail&aid=3454612&group_id=128336&atid=711373
	 */
    long currLen = 0; 
#ifndef NO_ITIMER
    struct itimerval it;
#endif /* NO_ITIMER */
    max_size_t totLen = 0;

#ifndef NO_ITIMER
    int err;
#endif /* NO_ITIMER */

#ifndef NO_FILE_IO
    char* readAt = mBuf;
#endif /* NO_FILE_IO */

    // Indicates if the stream is readable 
#ifndef NO_FILE_IO
    bool canRead = true;
#endif /* NO_FILE_IO */
	bool mMode_Time = isModeTime( mSettings ); 

    ReportStruct *reportstruct = NULL;

    // InitReport handles Barrier for multiple Streams
    IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE, ( "Client is initializing report.\n" ) );
    mSettings->reporthdr = InitReport( mSettings );
    reportstruct = (ReportStruct*) malloc( sizeof( ReportStruct ) );
    FAIL_errno( reportstruct == NULL, ( "No memory for ReportStruct reportstruct.\n" ), mSettings );
    IPERF_DEBUGF( MEMALLOC_DEBUG | IPERF_DBG_TRACE, IPERF_MEMALLOC_MSG( reportstruct, sizeof( ReportStruct ) ) );
    reportstruct->packetID = 0;

    lastPacketTime.setnow();
#ifndef NO_ITIMER
    if ( mMode_Time ) {
        memset (&it, 0, sizeof (it));
        it.it_value.tv_sec = (int) (mSettings->mAmount / 100.0);
        it.it_value.tv_usec = ( (int) 10000 * (mSettings->mAmount -
            it.it_value.tv_sec * 100.0) );
        err = setitimer( ITIMER_REAL, &it, NULL );
        if ( err != 0 ) {
            perror("setitimer failed");
            exit(1);
        }
    }
#endif /* NO_ITIMER */

    IPERF_DEBUGF( CLIENT_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Client is beginning to transmit TCP packets.\n" ) );
    do {
#ifndef NO_FILE_IO
        // Read the next data block from 
        // the file if it's file input 
        if ( isFileInput( mSettings ) ) {
            Extractor_getNextDataBlock( readAt, mSettings ); 
            canRead = Extractor_canRead( mSettings ) != 0; 
        } else
            canRead = true; 
#endif /* NO_FILE_IO */

        // perform write
#if HAVE_QUAD_SUPPORT
        IPERF_DEBUGF_COUNTER( SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Client is writing %d bytes to socket %d [total length=%llu].\n", mSettings->mBufLen, mSettings->mSock, (long long unsigned) totLen ) );
#else
        IPERF_DEBUGF_COUNTER( SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Client is writing %d bytes to socket %d [total length=%lu].\n", mSettings->mBufLen, mSettings->mSock, (long unsigned) totLen ) );
#endif /* HAVE_QUAD_SUPPORT */
        currLen = iperf_write( mSettings->mSock, mBuf, mSettings->mBufLen );
        if ( currLen < 0 
#if !defined(WIN32) && !defined(NO_ITIMER)
                         && errno != ENOBUFS
#endif /* !defined(WIN32) && !defined(NO_ITIMER) */
            ) {
            WARN_errno( currLen < 0, ( "Unexpected return value from socket write operation: %ld.\n", currLen ) );
            break; 
        }
		totLen += currLen;

		if(mSettings->mInterval > 0) {
        	gettimeofday( &(reportstruct->packetTime), NULL );
        	reportstruct->packetLen = currLen;
            IPERF_DEBUGF_COUNTER( TIME_DEBUG | IPERF_DBG_TRACE, ( "Packet time for packet %d is %ld sec %ld usec.\n", reportstruct->packetID, (long)reportstruct->packetTime.tv_sec, (long)reportstruct->packetTime.tv_usec ) );
				
            IPERF_DEBUGF_COUNTER( CLIENT_DEBUG | IPERF_DBG_TRACE, ( "Client is reporting packet %d.\n", reportstruct->packetID ) );
            ReportPacket( mSettings->reporthdr, reportstruct );
        }	

        if ( !mMode_Time ) {
            /* mAmount may be unsigned, so don't let it underflow! */
            if( currLen > 0 && mSettings->mAmount >= (unsigned) currLen ) {
                mSettings->mAmount -= currLen;
            } else {
                mSettings->mAmount = 0;
            }
        }
        IPERF_DEBUGF_COUNTER_INCREMENT();
    } while ( ! (
#ifndef NO_INTERRUPTS
					sInterupted  || 
#endif /* NO_INTERRUPTS */
                   (!mMode_Time  &&  0 >= mSettings->mAmount)) 
#ifndef NO_FILE_IO
				   && canRead 
#endif /* NO_FILE_IO */
				   ); 

    // stop timing
    IPERF_DEBUGF( CLIENT_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Client has finished transmitting TCP packets.\n" ) );
    gettimeofday( &(reportstruct->packetTime), NULL );
    IPERF_DEBUGF( CLIENT_DEBUG | TIME_DEBUG | IPERF_DBG_TRACE, ( "Client has stopped timing. Stop time is %ld sec %ld us.\n", (long)reportstruct->packetTime.tv_sec, (long)reportstruct->packetTime.tv_usec ) );

    // if we're not doing interval reporting, report the entire transfer as one big packet
    if(0.0 == mSettings->mInterval) {
        reportstruct->packetLen = totLen;
        IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE, ( "Client is reporting packet %d.\n", reportstruct->packetID ) );
        ReportPacket( mSettings->reporthdr, reportstruct );
    }
    IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE, ( "Client is closing report with index %d.\n", mSettings->reporthdr->reporterindex ) );
    CloseReport( mSettings->reporthdr, reportstruct );

    IPERF_DEBUGF( MEMFREE_DEBUG | IPERF_DBG_TRACE, IPERF_MEMFREE_MSG( reportstruct ) );
    FREE_PTR( reportstruct );
	
    IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE, ( "Client is ending report with index %d.\n", mSettings->reporthdr->reporterindex ) );
    EndReport( mSettings->reporthdr );
} // end Client::RunTCP

/* ------------------------------------------------------------------- 
 * Send data using the connected UDP/TCP socket, 
 * until a termination flag is reached. 
 * Does not close the socket. 
 * ------------------------------------------------------------------- */ 
void Client::Run( void ) {
    IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Running Client.\n" ) );

    struct UDP_datagram* mBuf_UDP = (struct UDP_datagram*) mBuf;
    long currLen = 0;

    int delay_target = 0;
    int delay = 0; 
    int adjust = 0; 

#ifndef NO_FILE_IO
    char* readAt = mBuf;
#endif /* NO_FILE_IO */

#if HAVE_THREAD
#if 0
    /**
     * BUGFIX: This causes iperf server to terminate unexpectedly.
     * @see http://sourceforge.net/tracker/index.php?func=detail&aid=1983829&group_id=128336&atid=711371
     */
    if ( !isUDP( mSettings ) ) {
		RunTCP();
		return;
    }
#endif /* 0 */
#endif /* HAVE_THREAD */
    
    // Indicates if the stream is readable 
#ifndef NO_FILE_IO
    bool canRead = true;
#endif /* NO_FILE_IO */
	bool mMode_Time = isModeTime( mSettings ); 

    // setup termination variables
    if ( mMode_Time ) {
        mEndTime.setnow();
        mEndTime.add( mSettings->mAmount / 100.0 );
        IPERF_DEBUGF( CLIENT_DEBUG | TIME_DEBUG | IPERF_DBG_TRACE, ( "Client set end time to %ld sec %ld usec.\n", mEndTime.getSecs( ), mEndTime.getUsecs( ) ) );
    }

    if ( isUDP( mSettings ) ) {
        // Due to the UDP timestamps etc, included 
        // reduce the read size by an amount 
        // equal to the header size
    
        // compute delay for bandwidth restriction, constrained to [0,1] seconds 
        delay_target = (int) ( mSettings->mBufLen * ((kSecs_to_usecs * kBytes_to_Bits) 
                                                     / mSettings->mUDPRate) ); 
        IPERF_DEBUGF( CLIENT_DEBUG | TIME_DEBUG | IPERF_DBG_TRACE, ( "Client calculated time delay for bandwidth restriction as %d usec.\n", delay_target ) );
        if ( delay_target < 0  || 
             delay_target > (int) 1 * kSecs_to_usecs ) {
            fprintf( stderr, warn_delay_large, delay_target / kSecs_to_usecs );
            delay_target = (int) kSecs_to_usecs * 1; 
        }
#ifndef NO_FILE_IO
        if ( isFileInput( mSettings ) ) {
            if ( isCompat( mSettings ) ) {
                Extractor_reduceReadSize( sizeof(struct UDP_datagram), mSettings );
                readAt += sizeof(struct UDP_datagram);
            } else {
                Extractor_reduceReadSize( sizeof(struct UDP_datagram) +
                                          sizeof(struct client_hdr), mSettings );
                readAt += sizeof(struct UDP_datagram) +
                          sizeof(struct client_hdr);
            }
        }
#endif /* NO_FILE_IO */
    }

    ReportStruct *reportstruct = NULL;

    // InitReport handles Barrier for multiple Streams
    IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE, ( "Client is initializing a report.\n" ) );
    mSettings->reporthdr = InitReport( mSettings );
    reportstruct = (ReportStruct*) malloc( sizeof( ReportStruct ) );
    FAIL_errno( reportstruct == NULL, ( "No memory for ReportStruct reportstruct.\n" ), mSettings );
    IPERF_DEBUGF( MEMALLOC_DEBUG | IPERF_DBG_TRACE, IPERF_MEMALLOC_MSG( reportstruct, sizeof( ReportStruct ) ) );
    reportstruct->packetID = 0;

    lastPacketTime.setnow();
    IPERF_DEBUGF( CLIENT_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Client is beginning to transmit packets.\n" ) );
    
    do {
        // Test case: drop 17 packets and send 2 out-of-order: 
        // sequence 51, 52, 70, 53, 54, 71, 72 
        //switch( datagramID ) { 
        //  case 53: datagramID = 70; break; 
        //  case 71: datagramID = 53; break; 
        //  case 55: datagramID = 71; break; 
        //  default: break; 
        //} 
        gettimeofday( &(reportstruct->packetTime), NULL );

        if ( isUDP( mSettings ) ) {
            // store datagram ID into buffer 

            mBuf_UDP->id      = htonl( (reportstruct->packetID) );
            reportstruct->packetID++;
            mBuf_UDP->tv_sec  = htonl( reportstruct->packetTime.tv_sec ); 
            mBuf_UDP->tv_usec = htonl( reportstruct->packetTime.tv_usec );

            // delay between writes 
            // make an adjustment for how long the last loop iteration took 
            // TODO this doesn't work well in certain cases, like 2 parallel streams 
            adjust = delay_target + lastPacketTime.subUsec( reportstruct->packetTime ); 
            lastPacketTime.set( reportstruct->packetTime.tv_sec, 
                                reportstruct->packetTime.tv_usec ); 

            if ( adjust > 0  ||  delay > 0 ) {
                delay += adjust; 
                IPERF_DEBUGF_COUNTER( TIME_DEBUG | IPERF_DBG_TRACE, ( "Client adjusted delay time by %d usec. Delay time is now %d usec.\n", adjust, delay ) );
            }
        }

#ifndef NO_FILE_IO
        // Read the next data block from 
        // the file if it's file input 
        if ( isFileInput( mSettings ) ) {
            Extractor_getNextDataBlock( readAt, mSettings ); 
            canRead = Extractor_canRead( mSettings ) != 0; 
        } else
            canRead = true; 
#endif /* NO_FILE_IO */
			
        // perform write 
        IPERF_DEBUGF_COUNTER( SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Client is writing %d bytes to host %s through socket %d.\n", mSettings->mBufLen, mSettings->mHost, mSettings->mSock ) );

        currLen = iperf_write( mSettings->mSock, mBuf, mSettings->mBufLen );


        if ( currLen < 0

#if !defined(WIN32) && !defined(NO_ITIMER)
		     && errno != ENOBUFS
#endif /* !defined(WIN32) && !defined(NO_ITIMER) */
			 ) {
            WARN_errno( currLen < 0, ( "Unexpected return value from socket write operation: %ld.\n", currLen ) );
            break; 
        }

        IPERF_DEBUGF_COUNTER( TIME_DEBUG | IPERF_DBG_TRACE, ( "Packet time for packet %d is %ld sec %ld usec.\n", reportstruct->packetID, (long)reportstruct->packetTime.tv_sec, (long)reportstruct->packetTime.tv_usec ) );

        // report packets 
        IPERF_DEBUGF_COUNTER( CLIENT_DEBUG | IPERF_DBG_TRACE, ( "Client is reporting packet %d.\n", reportstruct->packetID ) );
        reportstruct->packetLen = currLen;
        ReportPacket( mSettings->reporthdr, reportstruct );
        
        if ( delay > IPERF_UDP_DELAY ) {
#if HAVE_QUAD_SUPPORT
            IPERF_DEBUGF_COUNTER( CLIENT_DEBUG | TIME_DEBUG | IPERF_DBG_TRACE, ( "Client is delaying for %dms to constrain bandwidth to %llu bits/sec.\n", delay, (long long unsigned) mSettings->mUDPRate ) );
#else
            IPERF_DEBUGF_COUNTER( CLIENT_DEBUG | TIME_DEBUG | IPERF_DBG_TRACE, ( "Client is delaying for %dms to constrain bandwidth to %lu bits/sec.\n", delay, (long unsigned) mSettings->mUDPRate ) );
#endif /* HAVE_QUAD_SUPPORT */
            delay_loop( delay ); 
        }
        if ( !mMode_Time ) {
            /* mAmount may be unsigned, so don't let it underflow! */
            if( mSettings->mAmount >= (unsigned long)currLen ) {
                mSettings->mAmount -= currLen;
            } else {
                mSettings->mAmount = 0;
            }
        }
        IPERF_DEBUGF_COUNTER_INCREMENT();
    } while ( ! (
#ifndef NO_INTERRUPTS
				 sInterupted  || 
#endif /* NO_INTERRUPTS */
                 (mMode_Time   &&  mEndTime.before( reportstruct->packetTime ))  || 
                 (!mMode_Time  &&  0 >= mSettings->mAmount))
#ifndef NO_FILE_IO
				 && canRead 
#endif /* NO_FILE_IO */
				 ); 

    // stop timing
    IPERF_DEBUGF( CLIENT_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Client has finished transmitting packets.\n" ) );
    gettimeofday( &(reportstruct->packetTime), NULL );

    IPERF_DEBUGF( CLIENT_DEBUG | TIME_DEBUG | IPERF_DBG_TRACE, ( "Client has stopped timing. Stop time is %ld sec %ld us.\n", (long)reportstruct->packetTime.tv_sec, (long)reportstruct->packetTime.tv_usec ) );

    IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE, ( "Client is closing report with index %d.\n", mSettings->reporthdr->reporterindex ) );
    CloseReport( mSettings->reporthdr, reportstruct );

    if ( isUDP( mSettings ) ) {
        // send a final terminating datagram 
        // Don't count in the mTotalLen. The server counts this one, 
        // but didn't count our first datagram, so we're even now. 
        // The negative datagram ID signifies termination to the server. 

        IPERF_DEBUGF( CLIENT_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Client is sending the final terminating datagram.\n" ) );
    
        // store datagram ID into buffer 
        mBuf_UDP->id      = htonl( -(reportstruct->packetID)  ); 
        mBuf_UDP->tv_sec  = htonl( reportstruct->packetTime.tv_sec ); 
        mBuf_UDP->tv_usec = htonl( reportstruct->packetTime.tv_usec ); 

        if ( isMulticast( mSettings ) ) {
            int rc;

            rc = iperf_write( mSettings->mSock, mBuf, mSettings->mBufLen );

            WARN_errno( rc < 0, (  "Multicast socket write error: %d.\n", rc ) );
        } else {
            write_UDP_FIN( ); 
        }
    }
    IPERF_DEBUGF( MEMFREE_DEBUG | IPERF_DBG_TRACE, IPERF_MEMFREE_MSG( reportstruct ) );
    FREE_PTR( reportstruct );
	
    IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE, ( "Client is ending report with index %d.\n", mSettings->reporthdr->reporterindex ) );
    EndReport( mSettings->reporthdr ); 
} // end Client::Run

void Client::InitiateServer() {
    IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Client is initiating with the server.\n" ) );

    if ( !isCompat( mSettings ) ) {
        int currLen;
        client_hdr* temp_hdr;
        if ( isUDP( mSettings ) ) {
            UDP_datagram *UDPhdr = (UDP_datagram *)mBuf;
            temp_hdr = (client_hdr*)(UDPhdr + 1);
        } else {
            temp_hdr = (client_hdr*)mBuf;
        }
        IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE, ( "Client is generating the client header.\n" ) );
        Settings_GenerateClientHdr( mSettings, temp_hdr );
        if ( !isUDP( mSettings ) ) {
            IPERF_DEBUGF( SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Client is sending its header to the server using socket %d.\n", mSettings->mSock ) );
            currLen = send( mSettings->mSock, mBuf, sizeof(client_hdr), 0 );
            WARN_errno( currLen < 0, ( "Unexpected return value from socket write operation: %d.\n", currLen ) );
        }
    }
} // end Client::InitiateServer

/* -------------------------------------------------------------------
 * Setup a socket connected to a server.
 * If inLocalhost is not null, bind to that address, specifying
 * which outgoing interface to use.
 * ------------------------------------------------------------------- */
void Client::Connect( ) {
    int rc;
    int domain;
    SockAddr_remoteAddr( mSettings );

    assert( mSettings->mHost != NULL );

    IPERF_DEBUGF( CLIENT_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Client is setting up a socket connected to server %s.\n", mSettings->mHost ) );

    // create an internet socket
    int type = ( isUDP( mSettings )  ?  SOCK_DGRAM : SOCK_STREAM);

#ifdef HAVE_IPV6
    domain = (SockAddr_isIPv6( &mSettings->peer ) ? AF_INET6 : AF_INET;
#else
    domain = AF_INET;
#endif /* HAVE_IPV6 */

    mSettings->mSock = socket( domain, type, 0 );
    WARN_errno( mSettings->mSock == (signed) INVALID_SOCKET, ( "Invalid socket descriptor.\n" ) );

    IPERF_DEBUGF( SOCKET_DEBUG | IPERF_DBG_TRACE, ("Client is setting socket options for socket %d: {\n"
        "\tTCP window = %d\n"
        "\tCongestion control = %s\n"
        "\tMulticast = %s\n"
        "\tMulticast TTL = %d\n"
        "\tIP TOS = %d\n"
        "\tTCP MSS = %d\n"
		"\tTCP no delay = %s\n}\n",
		mSettings->mSock,
		mSettings->mTCPWin,
		isCongestionControl( mSettings ) ? "yes" : "no",
		isMulticast( mSettings ) ? "yes" : "no",
		mSettings->mTTL,
		mSettings->mTOS,
		mSettings->mMSS,
		isNoDelay( mSettings ) ? "yes" : "no" ) );
    SetSocketOptions( mSettings );

    SockAddr_localAddr( mSettings );
    if ( mSettings->mLocalhost != NULL ) {
        // bind socket to local address
        IPERF_DEBUGF( CLIENT_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Client is binding socket %d to localhost %s.\n", mSettings->mSock, mSettings->mLocalhost ) );
        rc = bind( mSettings->mSock, (sockaddr*) &mSettings->local,
                   SockAddr_get_sizeof_sockaddr( &mSettings->local ) );
        WARN_errno( rc == SOCKET_ERROR, ( "Socket bind failed.\n" ) );
    }

    // connect socket
    IPERF_DEBUGF( CLIENT_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Client is connecting to %s using socket %d.\n", mSettings->mHost, mSettings->mSock ) );

    rc = connect( mSettings->mSock, (sockaddr*) &mSettings->peer,
                  SockAddr_get_sizeof_sockaddr( &mSettings->peer ));
    FAIL_errno( rc == SOCKET_ERROR, ( "Socket connect failed.\n" ), mSettings );
    rc = getsockname( mSettings->mSock, (sockaddr*) &mSettings->local,
                 &mSettings->size_local );
    rc = getpeername( mSettings->mSock, (sockaddr*) &mSettings->peer,
                 &mSettings->size_peer );
} // end Client::Connect

/* ------------------------------------------------------------------- 
 * Send a datagram on the socket. The datagram's contents should signify 
 * a FIN to the application. Keep re-transmitting until an 
 * acknowledgement datagram is received. 
 * ------------------------------------------------------------------- */ 
void Client::write_UDP_FIN( ) {
    int rc; 
    fd_set readSet; 
    struct timeval timeout; 

    int count = 0; 
    while ( count < 10 ) {
        count++; 

        // write data 
        rc = iperf_write( mSettings->mSock, mBuf, mSettings->mBufLen );

        // wait until the socket is readable, or our timeout expires 
        // Note: Ignore CID# 33265(unused value 'readset') for FD_ZERO() operation in case of NetX*.
        // Other N/w stacks(for example - LwIP) may have different implementation of FD_ZERO compared to NetX*
        FD_ZERO( &readSet );
        FD_SET( mSettings->mSock, &readSet ); 
        timeout.tv_sec  = 0; 
        timeout.tv_usec = 250000; // quarter second, 250 ms 

        rc = iperf_select( mSettings->mSock+1, &readSet, NULL, NULL, &timeout );
        FAIL_errno( rc == SOCKET_ERROR, ( "Socket select failed.\n" ), mSettings );

        if ( rc == 0 ) {
            // select timed out 
            continue; 
        } else {
            // socket ready to read 
            rc = iperf_read( mSettings->mSock, mBuf, mSettings->mBufLen );
//            WARN_errno( rc < 0, ( "Unexpected value from socket read operation: %d.\n", rc ) );
    	    if ( rc < 0 ) {
                break;
            } else if ( rc >= (int) (sizeof(UDP_datagram) + sizeof(server_hdr)) ) {
                IPERF_DEBUGF( CLIENT_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Client will generate a report of the UDP statistics as reported by the server.\n" ) );
                ReportServerUDP( mSettings, (server_hdr*) ((UDP_datagram*)mBuf + 1) );
            }

            return; 
        } 
    } 

    fprintf( stderr, warn_no_ack, mSettings->mSock, count ); 
} // end Client::write_UDP_FIN 
