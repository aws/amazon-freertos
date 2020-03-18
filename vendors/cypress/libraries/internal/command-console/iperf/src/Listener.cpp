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
 * Listener.cpp
 * by Mark Gates <mgates@nlanr.net> 
 * &  Ajay Tirumala <tirumala@ncsa.uiuc.edu> 
 * ------------------------------------------------------------------- 
 * Listener sets up a socket listening on the server host. For each 
 * connected socket that accept() returns, this creates a Server 
 * socket and spawns a thread for it. 
 * 
 * Changes to the latest version. Listener will run as a daemon 
 * Multicast Server is now Multi-threaded 
 * ------------------------------------------------------------------- 
 * headers 
 * uses 
 *   <stdlib.h> 
 *   <stdio.h> 
 *   <string.h> 
 *   <errno.h> 
 * 
 *   <sys/types.h> 
 *   <unistd.h> 
 * 
 *   <netdb.h> 
 *   <netinet/in.h> 
 *   <sys/socket.h> 
 * ------------------------------------------------------------------- */ 

#define HEADERS() 

#include "headers.h" 
#include "Listener.hpp"
#include "SocketAddr.h"
#include "PerfSocket.hpp"
#include "Iperf_list.h"
#include "iperf_util.h"

/* ------------------------------------------------------------------- 
 * Stores local hostname and socket info. 
 * ------------------------------------------------------------------- */ 
Listener::Listener( thread_Settings *inSettings ) {
    IPERF_DEBUGF( LISTENER_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Creating new Listener.\n" ) );

    mClients = inSettings->mThreads;
    server = NULL;
    mBuf = NULL;
    mSettings = inSettings;

    // Force listener to run in single client mode
    setSingleClient(mSettings);

    // initialize buffer
    mBuf = (char*) malloc( mSettings->mBufLen );
    FAIL( mBuf == NULL, ( "No memory for buffer Listener::mBuf.\n" ), inSettings );
    IPERF_DEBUGF( MEMALLOC_DEBUG | IPERF_DBG_TRACE, IPERF_MEMALLOC_MSG( mBuf, mSettings->mBufLen ) );

    // open listening socket 
    Listen( ); 

    IPERF_DEBUGF( LISTENER_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Listener is reporting settings.\n" ) );
    ReportSettings( inSettings );
} // end Listener 

/* ------------------------------------------------------------------- 
 * Delete memory (buffer). 
 * ------------------------------------------------------------------- */ 
Listener::~Listener() {
    IPERF_DEBUGF( LISTENER_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Destroying Listener.\n" ) );

    if ( mSettings->mSock != (signed) INVALID_SOCKET ) {
        IPERF_DEBUGF( LISTENER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Closing Listener socket %d.\n", mSettings->mSock ) );
        int rc = iperf_close( mSettings->mSock );
        WARN_errno( rc == SOCKET_ERROR, ( "Socket close failed.\n" ) );
        mSettings->mSock = (signed) INVALID_SOCKET;
    }
    IPERF_DEBUGF( MEMFREE_DEBUG | IPERF_DBG_TRACE, IPERF_MEMFREE_MSG( mBuf ) );
    FREE_PTR( mBuf );
} // end ~Listener 

/* ------------------------------------------------------------------- 
 * Listens for connections and starts Servers to handle data. 
 * For TCP, each accepted connection spawns a Server thread. 
 * For UDP, handle all data in this thread for Win32 Only, otherwise
 *          spawn a new Server thread. 
 * ------------------------------------------------------------------- */ 
void Listener::Run( void ) {
    IPERF_DEBUGF( LISTENER_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Running Listener.\n" ) );

#ifdef WIN32
    if ( isUDP( mSettings ) && !isSingleUDP( mSettings ) ) {
        UDPSingleServer();
    } else
#else
#ifdef sun
    if ( ( isUDP( mSettings ) && 
           isMulticast( mSettings ) && 
           !isSingleUDP( mSettings ) ) ||
         isSingleUDP( mSettings ) ) {
        UDPSingleServer();
    } else
#else
    if ( isSingleUDP( mSettings ) ) {
        UDPSingleServer();
    } else
#endif /* sun */
#endif /* WIN32 */
    {
        bool client = false, UDP = isUDP( mSettings ), mCount = (mSettings->mThreads != 0);
        thread_Settings *tempSettings = NULL;
        Iperf_ListEntry *exist, *listtemp;
        client_hdr* hdr = ( UDP ? (client_hdr*) (((UDP_datagram*)mBuf) + 1) : 
                                  (client_hdr*) mBuf);
        
        if ( mSettings->mHost != NULL ) {
            client = true;
            SockAddr_remoteAddr( mSettings );
        }
        IPERF_DEBUGF( LISTENER_DEBUG | IPERF_DBG_TRACE, ( "Listener is copying settings into server.\n" ) );
        Settings_Copy( mSettings, &server );
        server->mThreadMode = kMode_Server;
    
    
        // Accept each packet, 
        // If there is no existing client, then start  
        // a new thread to service the new client 
        // The listener runs in a single thread 
        // Thread per client model is followed 
        do {
            // Get a new socket
            IPERF_DEBUGF( LISTENER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Listener is blocking until a new connection arrives.\n" ) );
            Accept( server );
            if ( server->mSock == (signed) INVALID_SOCKET ) {
				WARN_errno( 1, ( "Invalid socket!\n" ) );
                break;
            }
#ifndef NO_INTERRUPTS
            if ( sInterupted != 0 )
            {
                iperf_close( server->mSock );
                break;
            }
#endif /* NO_INTERRUPTS */
            // Reset Single Client Stuff
            if ( isSingleClient( mSettings ) && clients != NULL ) {
                mSettings->peer = server->peer;
                mClients--;
                client = true;
                // Once all the server threads exit then quit
                // Must keep going in case this client sends
                // more streams
                if ( mClients == 0 ) {
                    iperf_thread_release_nonterm( 0 );
                    mClients = 1;
                }
            }
            // Verify that it is allowed
            if ( client ) {
                if ( !SockAddr_Hostare_Equal( (sockaddr*) &mSettings->peer, 
                                              (sockaddr*) &server->peer ) ) {
                    // Not allowed try again
                    iperf_close( server->mSock );
                    if ( isUDP( mSettings ) ) {
                        mSettings->mSock = (signed) INVALID_SOCKET;
                        Listen();
                    }
                    continue;
                }
            }
    
            // Create an entry for the connection list
            listtemp = (Iperf_ListEntry*) malloc( sizeof( Iperf_ListEntry ) );
            FAIL( listtemp == NULL, ( "No memory for Iperf_ListEntry listtemp.\n" ), mSettings );
            IPERF_DEBUGF( MEMFREE_DEBUG | IPERF_DBG_TRACE, IPERF_MEMALLOC_MSG( listtemp, sizeof( Iperf_ListEntry ) ) );
            memcpy(listtemp, &server->peer, sizeof(iperf_sockaddr));
            listtemp->next = NULL;
    
            // See if we need to do summing
            Mutex_Lock( &clients_mutex );
            exist = Iperf_hostpresent( &server->peer, clients); 
    
            if ( exist != NULL ) {
                // Copy group ID
                listtemp->holder = exist->holder;
                server->multihdr = exist->holder;
                //(exist->holder->count)++;
            } else {
                server->mThreads = 0;
                Mutex_Lock( &groupCond );
                groupID--;
                listtemp->holder = InitMulti( server, groupID );
                server->multihdr = listtemp->holder;
                Mutex_Unlock( &groupCond );
            }
    
            // Store entry in connection list
            Iperf_pushback( listtemp, &clients );
            Mutex_Unlock( &clients_mutex ); 
    
            tempSettings = NULL;
            if ( !isCompat( mSettings ) && !isMulticast( mSettings ) ) {
                if ( !UDP ) {
                    // TCP does not have the info yet
                    if ( recv( server->mSock, (char*)hdr, sizeof(client_hdr), 0) > 0 ) {
                        Settings_GenerateClientSettings( server, &tempSettings, 
                                                          hdr );
                    }
                } else {
                    Settings_GenerateClientSettings( server, &tempSettings, 
                                                      hdr );
                }
            }
    
            if ( tempSettings != NULL ) {
                client_init( tempSettings );
                if ( tempSettings->mMode == kTest_DualTest ) {
#ifdef HAVE_THREAD
                    server->runNow =  tempSettings;
#else
                    server->runNext = tempSettings;
#endif /* HAVE_THREAD */
                } else {
                    server->runNext =  tempSettings;
                }
            }
    
            // Start the server
            IPERF_DEBUGF( LISTENER_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Listener is starting the server.\n" ) );
#if defined(WIN32) && defined(HAVE_THREAD)
            if ( UDP ) {
                // WIN32 does bad UDP handling so run single threaded
                if ( server->runNow != NULL ) {
                    iperf_thread_start( server->runNow );
                }
                server_spawn( server );
                if ( server->runNext != NULL ) {
                    iperf_thread_start( server->runNext );
                }
            } else
#endif /* defined(WIN32) && defined(HAVE_THREAD) */
            iperf_thread_start( server );
    
            // create a new socket
            if ( UDP ) {
                mSettings->mSock = -1; 
                IPERF_DEBUGF( LISTENER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Listener is listening.\n" ) );
                Listen( );
            }
    
            // Prep for next connection
            IPERF_DEBUGF( LISTENER_DEBUG | IPERF_DBG_TRACE, ( "Listener is preparing for next connection.\n" ) );
            if ( !isSingleClient( mSettings ) ) {
                mClients--;
            }
            Settings_Copy( mSettings, &server );
            server->mThreadMode = kMode_Server;
        } while (
#ifndef NO_INTERRUPTS
	             !sInterupted && 
#endif /* NO_INTERRUPTS */
				 (!isSingleClient( mSettings ) && (!mCount || ( mCount && mClients > 0 )))
                );
    
        IPERF_DEBUGF( LISTENER_DEBUG | IPERF_DBG_TRACE, ( "Listener is destroying server settings.\n" ) );
        Settings_Destroy( server );
    }
} // end Listener::Run 

/* -------------------------------------------------------------------
 * Setup a socket listening on a port.
 * For TCP, this calls bind() and listen().
 * For UDP, this just calls bind().
 * If inLocalhost is not null, bind to that address rather than the
 * wildcard server address, specifying what incoming interface to
 * accept connections on.
 * ------------------------------------------------------------------- */
void Listener::Listen( ) {
    int rc;

    SockAddr_localAddr( mSettings );

    // create an internet TCP socket
    int type = (isUDP( mSettings )  ?  SOCK_DGRAM  :  SOCK_STREAM);
    int domain = (
#ifdef HAVE_IPV6
                  SockAddr_isIPv6( &mSettings->local ) ? AF_INET6 :
#endif /* HAVE_IPV6 */
                  AF_INET);

#ifdef WIN32
    if ( SockAddr_isMulticast( &mSettings->local ) ) {
        // Multicast on Win32 requires special handling
        mSettings->mSock = WSASocket( domain, type, 0, 0, 0, WSA_FLAG_MULTIPOINT_C_LEAF | WSA_FLAG_MULTIPOINT_D_LEAF );
        WARN_errno( mSettings->mSock == (signed) INVALID_SOCKET, ( "WSASocket failed.\n" ) );

    } else
#endif /* WIN32 */
    {
        mSettings->mSock = socket( domain, type, 0 );
        WARN_errno( mSettings->mSock == (signed) INVALID_SOCKET, ( "Invalid socket descriptor.\n" ) );
    } 

    IPERF_DEBUGF( LISTENER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Listener is setting socket options.\n" ) );
    SetSocketOptions( mSettings );

    // Don't reuse the socket. So that, the server can be spawned again with the same port. Also socket is cleanly un-binded and deleted during exit
//    Disabled since the option_name argument had been modified to zero (was SO_REUSEADDR) - Also should have return check
//    int boolean = 1;
//    Socklen_t len = sizeof(boolean);
//    setsockopt( mSettings->mSock, SOL_SOCKET, 0, (char*) &boolean, len );

    // bind socket to server address
#ifdef WIN32
    if ( SockAddr_isMulticast( &mSettings->local ) ) {
        // Multicast on Win32 requires special handling
        rc = WSAJoinLeaf( mSettings->mSock, (sockaddr*) &mSettings->local, mSettings->size_local,0,0,0,0,JL_BOTH);
        WARN_errno( rc == SOCKET_ERROR, ( "WSAJoinLeaf (aka bind) failed.\n" ) );
    } else
#endif
    {
        IPERF_DEBUGF( LISTENER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Listener is binding socket to server address.\n" ) );
        rc = bind( mSettings->mSock, (sockaddr*) &mSettings->local, mSettings->size_local );
        /**
         * Description: iperf die on bind fail
         * When iperf encounters a fatal error when binding to a port, such as
         * the port being already in use or lacking permission, it fails to give
         * fatal error, but instead pretends to continue to listen on the port.
         *
         * Author: Deny IP Any Any <denyipanyany@gmail.com>
         * Bug-Debian: http://bugs.debian.org/517239
         * Forwarded: https://sourceforge.net/tracker/?func=detail&aid=3140400&group_id=128336&atid=711371
         */
        FAIL_errno( rc == SOCKET_ERROR, ( "Failed to bind socket.\n" ), mSettings );
    }
    // listen for connections (TCP only).
    // default backlog traditionally 5
    if ( !isUDP( mSettings ) ) {
        IPERF_DEBUGF( LISTENER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Listener is listening.\n" ) );
        rc = listen( mSettings->mSock, 5 );
        WARN_errno( rc == SOCKET_ERROR, ( "Failed to listen on socket.\n" ) );
    }

#ifndef WIN32
    // if multicast, join the group
    if ( SockAddr_isMulticast( &mSettings->local ) ) {
        IPERF_DEBUGF( LISTENER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Listener is joining the multicast group.\n" ) );

        McastJoin( );
    }
#endif /* WIN32 */
} // end Listener::Listen

/* -------------------------------------------------------------------
 * Joins the multicast group, with the default interface.
 * ------------------------------------------------------------------- */
void Listener::McastJoin( ) {
#ifdef HAVE_MULTICAST
    if ( !SockAddr_isIPv6( &mSettings->local ) ) {
        struct ip_mreq mreq;

        memcpy( &mreq.imr_multiaddr, SockAddr_get_in_addr( &mSettings->local ), 
                sizeof(mreq.imr_multiaddr));

        mreq.imr_interface.s_addr = htonl( INADDR_ANY );

        int rc = setsockopt( mSettings->mSock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                             (char*) &mreq, sizeof(mreq));
        WARN_errno( rc == SOCKET_ERROR, ( "Failed to set multicast join.\n" ) );
    }
#ifdef HAVE_IPV6_MULTICAST
      else {
        struct ipv6_mreq mreq;

        memcpy( &mreq.ipv6mr_multiaddr, SockAddr_get_in6_addr( &mSettings->local ), 
                sizeof(mreq.ipv6mr_multiaddr));

        mreq.ipv6mr_interface = 0;

        int rc = setsockopt( mSettings->mSock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP,
                             (char*) &mreq, sizeof(mreq));
        WARN_errno( rc == SOCKET_ERROR, ( "Failed to set multicast join.\n" ) );
    }
#endif
#endif
} // end Listener::McastJoin

/* -------------------------------------------------------------------
 * Sets the Multicast TTL for outgoing packets.
 * ------------------------------------------------------------------- */
void Listener::McastSetTTL( int val ) {
#ifdef HAVE_MULTICAST
    if ( !SockAddr_isIPv6( &mSettings->local ) ) {
        int rc = setsockopt( mSettings->mSock, IPPROTO_IP, IP_MULTICAST_TTL,
                             (char*) &val, sizeof(val));
        WARN_errno( rc == SOCKET_ERROR, ( "Failed to set multicast TTL.\n )" ) );
    }
#ifdef HAVE_IPV6_MULTICAST
      else {
        int rc = setsockopt( mSettings->mSock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
                             (char*) &val, sizeof(val));
        WARN_errno( rc == SOCKET_ERROR, ( "Failed to set multicast TTL.\n" ) );
    }
#endif
#endif
} // end Listener::McastSetTTL

/* -------------------------------------------------------------------
 * After Listen() has setup mSock, this will block
 * until a new connection arrives.
 * ------------------------------------------------------------------- */
void Listener::Accept( thread_Settings *server ) {
    int rc;
    server->size_peer = sizeof(iperf_sockaddr); 
    if ( isUDP( server ) ) {
        /* ------------------------------------------------------------------- 
         * Do the equivalent of an accept() call for UDP sockets. This waits 
         * on a listening UDP socket until we get a datagram. 
         * ------------------------------------------------------------------- */
        Iperf_ListEntry *exist;
        int32_t datagramID;
        server->mSock = INVALID_SOCKET;
        IPERF_DEBUGF( LISTENER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Listener is waiting while listening on a UDP socket.\n") );
        while ( server->mSock == (signed) INVALID_SOCKET ) {
            rc = recvfrom( mSettings->mSock, mBuf, mSettings->mBufLen, 0, 
                           (struct sockaddr*) &server->peer, &server->size_peer );
            FAIL_errno( rc == SOCKET_ERROR, ( "recvfrom failed.\n" ), mSettings );

            IPERF_DEBUGF_COUNTER( LISTENER_DEBUG | MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Listener is unlocking the clients mutex.\n" ) );
            Mutex_Lock( &clients_mutex );
    
            // Handle connection for UDP sockets.
            exist = Iperf_present( &server->peer, clients);

            datagramID = ntohl( ((UDP_datagram*) mBuf)->id );

            if ( exist == NULL && datagramID >= 0 ) {
                int rc;
                IPERF_DEBUGF( LISTENER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Listener received a UDP datagram, connecting to server.\n" ) );
                server->mSock = mSettings->mSock;
                rc = connect( server->mSock, (struct sockaddr*) &server->peer,
                                  server->size_peer );
                FAIL_errno( rc != 0, ( "connect UDP failed.\n" ), mSettings );
            } else {
                server->mSock = INVALID_SOCKET;
            }

            IPERF_DEBUGF_COUNTER( LISTENER_DEBUG | MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Listener is unlocking the clients mutex.\n" ) );
            Mutex_Unlock( &clients_mutex );
        }
    } else {
        // Handles interupted accepts. Returns the newly connected socket.
        server->mSock = INVALID_SOCKET;
    
        IPERF_DEBUGF( LISTENER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Listener is waiting while listening on a TCP socket.\n") );
        while ( server->mSock == (signed) INVALID_SOCKET ) {
            // accept a connection
            server->mSock = accept( mSettings->mSock, 
                                    (sockaddr*) &server->peer, &server->size_peer );
            if ( server->mSock == (signed) INVALID_SOCKET &&  errno == EINTR ) {
                IPERF_DEBUGF( LISTENER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Listener accepted a connection.\n" ) );
                continue;
            }
        }
    }
    server->size_local = sizeof(iperf_sockaddr); 
    rc = getsockname( server->mSock, (sockaddr*) &server->local, &server->size_local );
    FAIL_errno( rc != 0, ( "getsockname failed.\n" ), mSettings );

} // end Listener::Accept

void Listener::UDPSingleServer( ) {
    IPERF_DEBUGF( LISTENER_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Listener is running UDP Single Server.\n" ) );
    
    bool client = false, UDP = isUDP( mSettings ), mCount = (mSettings->mThreads != 0);
    thread_Settings *tempSettings = NULL;
    Iperf_ListEntry *exist, *listtemp;
    int rc;
    int32_t datagramID;
    client_hdr* hdr = ( UDP ? (client_hdr*) (((UDP_datagram*)mBuf) + 1) : 
                              (client_hdr*) mBuf);
    ReportStruct *reportstruct = (ReportStruct*) malloc( sizeof( ReportStruct ) );
    FAIL( reportstruct == NULL, ( "No memory for ReportStruct reportstruct.\n" ), NULL );
    IPERF_DEBUGF( MEMALLOC_DEBUG | IPERF_DBG_TRACE, IPERF_MEMALLOC_MSG( reportstruct, sizeof( ReportStruct ) ) );
    
    if ( mSettings->mHost != NULL ) {
        client = true;
        SockAddr_remoteAddr( mSettings );
    }

    IPERF_DEBUGF( LISTENER_DEBUG | IPERF_DBG_TRACE, ( "Listener copying settings into server.\n" ) );
    Settings_Copy( mSettings, &server );
    server->mThreadMode = kMode_Server;


    // Accept each packet, 
    // If there is no existing client, then start  
    // a new report to service the new client 
    // The listener runs in a single thread 
    IPERF_DEBUGF( LISTENER_DEBUG | MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Listener is locking the clients mutex.\n" ) );
    Mutex_Lock( &clients_mutex );
    do {
        // Get next packet
#ifndef NO_INTERRUPTS
        while ( sInterupted == 0) {
#else
		while ( 1 ) {
#endif /* NO_INTERRUPTS */
            IPERF_DEBUGF( LISTENER_DEBUG | SOCKET_DEBUG | IPERF_DBG_TRACE, ( "Listener received the next packet.\n" ) );
            IPERF_DEBUGF_COUNTER_INCREMENT();

            server->size_peer = sizeof( iperf_sockaddr );
            rc = recvfrom( mSettings->mSock, mBuf, mSettings->mBufLen, 0, 
                           (struct sockaddr*) &server->peer, &server->size_peer );
            WARN_errno( rc == SOCKET_ERROR, ( "Failed to recvfrom through socket.\n" ) );
            if ( rc == SOCKET_ERROR ) {
                FREE_PTR( reportstruct );
                return;
            }
        
            // Handle connection for UDP sockets.
            exist = Iperf_present( &server->peer, clients);

            datagramID = ntohl( ((UDP_datagram*) mBuf)->id );

            if ( datagramID >= 0 ) {
                if ( exist != NULL ) {
                    // read the datagram ID and sentTime out of the buffer 
                    reportstruct->packetID = datagramID; 

                    reportstruct->sentTime.tv_sec = ntohl( ((UDP_datagram*) mBuf)->tv_sec  );
                    reportstruct->sentTime.tv_usec = ntohl( ((UDP_datagram*) mBuf)->tv_usec ); 

                    reportstruct->packetLen = rc;
                    gettimeofday( &(reportstruct->packetTime), NULL );
        
                    ReportPacket( exist->server->reporthdr, reportstruct );
                } else {
                    IPERF_DEBUGF( LISTENER_DEBUG | MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Listener is locking the group condition mutex.\n" ) );
                    Mutex_Lock( &groupCond );
                    groupID--;
                    server->mSock = -groupID;
                    IPERF_DEBUGF( LISTENER_DEBUG | MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Listener is unlocking the group condition mutex.\n" ) );
                    Mutex_Unlock( &groupCond );
                    if ( server->mSock >= 0 ) {
                        server->size_local = sizeof(iperf_sockaddr);
                        rc = getsockname( server->mSock, (sockaddr*) &server->local, &server->size_local );
                        WARN_errno( rc != 0, ( "getsockname failed.\n" ) );
                        if ( rc != 0 ) {
                            FREE_PTR( reportstruct );
                            return;
                        }
                    } else {
                        server->mSock = INVALID_SOCKET;
                    }

                    break;
                }
            } else {
                if ( exist != NULL ) {
                    // read the datagram ID and sentTime out of the buffer 
                    reportstruct->packetID = -datagramID; 

                    reportstruct->sentTime.tv_sec = ntohl( ((UDP_datagram*) mBuf)->tv_sec  );
                    reportstruct->sentTime.tv_usec = ntohl( ((UDP_datagram*) mBuf)->tv_usec ); 

                    reportstruct->packetLen = rc;
                    gettimeofday( &(reportstruct->packetTime), NULL );
        
                    ReportPacket( exist->server->reporthdr, reportstruct );
                    // stop timing 
                    gettimeofday( &(reportstruct->packetTime), NULL );
                    CloseReport( exist->server->reporthdr, reportstruct );
        
                    if ( rc > (int) ( sizeof( UDP_datagram )
                                                      + sizeof( server_hdr ) ) ) {
                        UDP_datagram *UDP_Hdr;
                        server_hdr *hdr;
        
                        UDP_Hdr = (UDP_datagram*) mBuf;
                        Transfer_Info *stats = GetReport( exist->server->reporthdr );
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
                    EndReport( exist->server->reporthdr );
                    exist->server->reporthdr = NULL;
                    Iperf_delete( &(exist->server->peer), &clients );
                } else if ( rc > (int) ( sizeof( UDP_datagram )
                                                  + sizeof( server_hdr ) ) ) {
                    UDP_datagram *UDP_Hdr;
                    server_hdr *hdr;
        
                    UDP_Hdr = (UDP_datagram*) mBuf;
                    hdr = (server_hdr*) (UDP_Hdr+1);
                    hdr->flags = htonl( 0 );
                }
                sendto( mSettings->mSock, mBuf, mSettings->mBufLen, 0,
                        (struct sockaddr*) &server->peer, server->size_peer);
            }
        }
        if ( server->mSock == (signed) INVALID_SOCKET ) {
            break;
        }
#ifndef NO_INTERRUPTS
        if (sInterupted != 0) {
            iperf_close( server->mSock );
            break;
        }
#endif /* NO_INTERRUPTS */
        // Reset Single Client Stuff
        if ( isSingleClient( mSettings ) && clients == NULL ) {
            mSettings->peer = server->peer;
            mClients--;
            client = true;
            // Once all the server threads exit then quit
            // Must keep going in case this client sends
            // more streams
            if ( mClients == 0 ) {
                iperf_thread_release_nonterm( 0 );
                mClients = 1;
            }
        }
        // Verify that it is allowed
        if ( client ) {
            if ( !SockAddr_Hostare_Equal( (sockaddr*) &mSettings->peer, 
                                          (sockaddr*) &server->peer ) ) {
                iperf_close( mSettings->mSock );
                mSettings->mSock = (signed) INVALID_SOCKET;
                Listen( );
                continue;
            }
        }

        // Create an entry for the connection list
        listtemp = (Iperf_ListEntry*) malloc( sizeof( Iperf_ListEntry ) );
        FAIL( listtemp == NULL, ( "No memory for Iperf_ListEntry listtemp.\n" ), NULL );
        IPERF_DEBUGF( MEMALLOC_DEBUG | IPERF_DBG_TRACE, IPERF_MEMALLOC_MSG( listtemp, sizeof( Iperf_ListEntry ) ) );
        memcpy(listtemp, &server->peer, sizeof(iperf_sockaddr));
        listtemp->server = server;
        listtemp->next = NULL;

        // See if we need to do summing
        exist = Iperf_hostpresent( &server->peer, clients); 

        if ( exist != NULL ) {
            // Copy group ID
            listtemp->holder = exist->holder;
            server->multihdr = exist->holder;
        } else {
            server->mThreads = 0;
            IPERF_DEBUGF( LISTENER_DEBUG | MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Listener is locking the group condition mutex.\n" ) );
            Mutex_Lock( &groupCond );
            groupID--;
            listtemp->holder = InitMulti( server, groupID );
            server->multihdr = listtemp->holder;
            IPERF_DEBUGF( LISTENER_DEBUG | MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Listener is unlocking the group condition mutex.\n" ) );
            Mutex_Unlock( &groupCond );
        }

        // Store entry in connection list
        Iperf_pushback( listtemp, &clients ); 

        tempSettings = NULL;
        if ( !isCompat( mSettings ) && !isMulticast( mSettings ) ) {
            Settings_GenerateClientSettings( server, &tempSettings, 
                                              hdr );
        }


        if ( tempSettings != NULL ) {
            client_init( tempSettings );
            if ( tempSettings->mMode == kTest_DualTest ) {
#ifdef HAVE_THREAD
                iperf_thread_start( tempSettings );
#else
                server->runNext = tempSettings;
#endif /* HAVE_THREAD */
            } else {
                server->runNext =  tempSettings;
            }
        }
        server->reporthdr = InitReport( server );

        // Prep for next connection
        if ( !isSingleClient( mSettings ) ) {
            mClients--;
        }
        Settings_Copy( mSettings, &server );
        server->mThreadMode = kMode_Server;
    } while ( 
#ifndef NO_INTERRUPTS
	          !sInterupted && 
#endif /* NO_INTERRUPTS */
			  (!mCount || ( mCount && mClients > 0 )) );
    IPERF_DEBUGF( LISTENER_DEBUG | MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Listener is unlocking the clients mutex.\n" ) );
    Mutex_Unlock( &clients_mutex );

    IPERF_DEBUGF( LISTENER_DEBUG | IPERF_DBG_TRACE, ( "Listener is destroying server settings.\n" ) );
    Settings_Destroy( server );
    FREE_PTR( reportstruct );
} // end Listener::UDPSingleServer

#ifndef NO_DAEMON
/* -------------------------------------------------------------------- 
 * Run the server as a daemon  
 * --------------------------------------------------------------------*/ 
void Listener::runAsDaemon(const char *pname, int facility) {
#ifndef WIN32 
    pid_t pid; 
    int rc;

    /* Create a child process & if successful, exit from the parent process */ 
    if ( (pid = fork()) == -1 ) {
        fprintf( stderr, "error in first child create\n");     
        exit(0); 
    } else if ( pid != 0 ) {
        exit(0); 
    }

    /* Try becoming the session leader, once the parent exits */
    if ( setsid() == -1 ) {           /* Become the session leader */ 
        fprintf( stderr, "Cannot change the session group leader\n"); 
    } else {
    } 
    signal(SIGHUP,SIG_IGN); 

    /* Now fork() and get released from the terminal */  
    if ( (pid = fork()) == -1 ) {
        fprintf( stderr, "error\n");   
        exit(0); 
    } else if ( pid != 0 ) {
        exit(0); 
    }

    rc = chdir("."); 
    UNUSED_PARAMETER(rc);
    fprintf( stderr, "Running Iperf Server as a daemon\n"); 
    fprintf( stderr, "The Iperf daemon process ID : %d\n",((int)getpid())); 
    fflush(stderr); 

    fclose(stdin); 
#else 
    fprintf( stderr, "Use the precompiled windows version for service (daemon) option\n"); 
#endif  /* WIN32 */
} // end Listener::runAsDaemon
#endif /* NO_DAEMON */
