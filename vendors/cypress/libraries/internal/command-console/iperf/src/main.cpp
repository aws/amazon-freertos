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
 * main.cpp
 * by Mark Gates <mgates@nlanr.net>
 * &  Ajay Tirumala <tirumala@ncsa.uiuc.edu>
 * -------------------------------------------------------------------
 * main does initialization and creates the various objects that will
 * actually run the iperf program, then waits in the Joinall().
 * -------------------------------------------------------------------
 * headers
 * uses
 *   <stdlib.h>
 *   <string.h>
 *
 *   <signal.h>
 * ------------------------------------------------------------------- */

#define HEADERS()

#include "headers.h"

#include "Settings.hpp"
#include "PerfSocket.hpp"
#include "iperf_locale.h"
#include "Condition.h"
#include "Timestamp.hpp"
#include "Listener.hpp"
#include "Iperf_list.h"
#include "iperf_util.h"

#ifdef WIN32
#include "service.h"
#endif /* WIN32 */

#ifndef NO_EXIT
/* -------------------------------------------------------------------
 * prototypes
 * ------------------------------------------------------------------- */
// Function called at exit to clean up as much as possible
void cleanup( void );
#endif /* NO_EXIT */

#ifdef WIN32
 /* -------------------------------------------------------------------
 * helper to call the sigalarm function
 * ------------------------------------------------------------------- */
void call_sigalrm() {
    call_signal(SIGALRM);
} // end call_sigalrm
#endif /* WIN32 */

/* -------------------------------------------------------------------
 * global variables
 * ------------------------------------------------------------------- */
extern "C" {
#ifndef NO_INTERRUPTS
    // Global flag to signal a user interrupt
    int sInterupted = 0;
#endif /* NO_INTERRUPTS */
    // Global ID that we increment to be used
    // as identifier for SUM reports
    int groupID = 0;
    // Mutex to protect access to the above ID
    Mutex groupCond;
    // Condition used to signify advances of the current
    // records being accessed in a report and also to
    // serialize modification of the report list
    Condition ReportCond;
    Condition ReportDoneCond;

    IPERF_DEBUGF_COUNTER_INITIALISE();

#ifdef NO_EXIT
    /* To allow iperf to exit cleanly in some instances, without making a call to exit() */
    int should_exit = 0;
#endif /* NO_EXIT */
} // end extern "C"

// global variables only accessed within this file

// Thread that received the SIGTERM or SIGINT signal
// Used to ensure that if multiple threads receive the
// signal we do not prematurely exit
nthread_t sThread;
// The main thread uses this function to wait
// for all other threads to complete
void waitUntilQuit( void );

/* -------------------------------------------------------------------
 * main()
 *      Entry point into Iperf
 *
 * sets up signal handlers
 * initialize global locks and conditions
 * parses settings from environment and command line
 * starts up server or client thread
 * waits for all threads to complete
 * ------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
#endif /* __cplusplus */
int IPERF_MAIN( int argc, char **argv ) {
    thread_Settings *ext_gSettings;
#ifdef NO_EXIT
    should_exit = 0;
#endif /* NO_EXIT */
#ifdef IPERF_DEBUG
    debug_init();
#endif /* IPERF_DEBUG */

#ifndef NO_INTERRUPTS
#ifdef WIN32
    setsigalrmfunc(call_sigalrm);
#endif /* WIN32 */

    // Set SIGTERM and SIGINT to call our user interrupt function
    my_signal( SIGTERM, Sig_Interupt );
    my_signal( SIGINT,  Sig_Interupt );
    my_signal( SIGALRM,  Sig_Interupt );

#ifndef WIN32
    // Ignore broken pipes
    signal(SIGPIPE,SIG_IGN);
#else
    // Start winsock
    WORD wVersionRequested;
    WSADATA wsaData;

    // Using MAKEWORD macro, Winsock version request 2.2
    wVersionRequested = MAKEWORD(2, 2);

    int rc = WSAStartup( wVersionRequested, &wsaData );
    WARN_errno( rc == SOCKET_ERROR, ( "WSAStartup failed.\n" ) );
	if (rc != 0) {
	    fprintf(stderr, "The Winsock DLL was not found!\n");
		return 1;
	}

    /*
     * Confirm that the WinSock DLL supports 2.2. Note that if the DLL supports
	 * versions greater than 2.2 in addition to 2.2, it will still return 2.2 in
	 * wVersion since that is the version we requested.
     */
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2 ) {
        /* Tell the user that we could not find a usable WinSock DLL. */
        fprintf(stderr, "The DLL does not support the Winsock version %u.%u!\n", LOBYTE(wsaData.wVersion),HIBYTE(wsaData.wVersion));
        WSACleanup();
        return 1;
    }

    // Tell windows we want to handle our own signals
    SetConsoleCtrlHandler( sig_dispatcher, true );
#endif /* WIN32 */
#endif /* NO_INTERRUPTS */
    do {
        // Initialize global mutexes and conditions
        IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Initializing report condition.\n" ) );
        Condition_Initialize ( &ReportCond );
        IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Initializing report done condition.\n" ) );
        Condition_Initialize ( &ReportDoneCond );
        IPERF_DEBUGF( MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Initializing group condition mutex.\n" ) );
        Mutex_Initialize( &groupCond );
        IPERF_DEBUGF( MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Initializing clients mutex.\n" ) );
        Mutex_Initialize( &clients_mutex );

        // Initialize the thread subsystem
        IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Initializing the thread subsystem.\n" ) );
        iperf_thread_init( );

        // Initialize the interrupt handling thread to 0
        sThread = iperf_thread_zeroid();

#ifndef NO_EXIT
        // perform any cleanup when quitting Iperf
        atexit( cleanup );
#endif /* NO_EXIT */

        // Allocate the "global" settings
        ext_gSettings = (thread_Settings*) malloc( sizeof( thread_Settings ) );
        FAIL( ext_gSettings == NULL, ( "Unable to allocate memory for thread_Settings ext_gSettings.\n" ), NULL );
        IPERF_DEBUGF( MEMALLOC_DEBUG, IPERF_MEMALLOC_MSG( ext_gSettings, sizeof( thread_Settings ) ) );

        // Initialize settings to defaults
        Settings_Initialize( ext_gSettings );
#ifndef NO_ENVIRONMENT
        // read settings from environment variables
        Settings_ParseEnvironment( ext_gSettings );
#endif /* NO_ENVIORNMENT */
        // read settings from command-line parameters
        Settings_ParseCommandLine( argc, argv, ext_gSettings );

#ifdef NO_EXIT
        if (should_exit) {
            IPERF_DEBUGF( MEMFREE_DEBUG | IPERF_DBG_TRACE, IPERF_MEMFREE_MSG( ext_gSettings ) );
            FREE_PTR( ext_gSettings );

            IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Destroying report condition.\n" ) );
            Condition_Destroy( &ReportCond );
            IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Destroying report done condition.\n" ) );
            Condition_Destroy( &ReportDoneCond );
            IPERF_DEBUGF( MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Destroying group condition mutex.\n" ) );
            Mutex_Destroy( &groupCond );
            IPERF_DEBUGF( MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Destroying clients mutex.\n" ) );
            Mutex_Destroy( &clients_mutex );

            return 0;
    }
#endif /* NO_EXIT */

        // Check for either having specified client or server
        if ( ext_gSettings->mThreadMode == kMode_Client ||
             ext_gSettings->mThreadMode == kMode_Listener ) {
#ifdef WIN32
#ifndef NO_DAEMON
            // Start the server as a daemon
            // Daemon mode for non-windows in handled
            // in the listener_spawn function
            if ( isDaemon( ext_gSettings ) ) {
                CmdInstallService(argc, argv);
                return 0;
            }
#endif /* NO_DAEMON */

#ifndef NO_SERVICE
            // Remove the Windows service if requested
            if ( isRemoveService( ext_gSettings ) ) {
                // remove the service
                if ( CmdRemoveService() ) {
                    fprintf(stderr, "IPerf Service is removed.\n");
                    return 0;
                }
            }
#endif /* NO_SERVICE */
#endif /* WIN32 */
            // initialize client(s)
            if ( ext_gSettings->mThreadMode == kMode_Client ) {
                IPERF_DEBUGF( CLIENT_DEBUG | LISTENER_DEBUG | IPERF_DBG_TRACE, ( "Initializing client(s)...\n" ) );
                client_init( ext_gSettings );
            }

#ifdef HAVE_THREAD
            // start up the reporter and client(s) or listener
            thread_Settings *into = NULL;
            // Create the settings structure for the reporter thread
            IPERF_DEBUGF( CLIENT_DEBUG | LISTENER_DEBUG | REPORTER_DEBUG | IPERF_DBG_TRACE, ( "Creating the settings structure for the reporter thread.\n" ) );
            Settings_Copy( ext_gSettings, &into );
            into->mThreadMode = kMode_Reporter;

            // Have the reporter launch the client or listener
            IPERF_DEBUGF( CLIENT_DEBUG | LISTENER_DEBUG | IPERF_DBG_TRACE, ( "Setting the reporter to launch the client or listener before launching itself.\n" ) );
            into->runNow = ext_gSettings;

            // Start all the threads that are ready to go
            IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Starting all the threads...\n" ) );
            thread_start( into );
#else
            // No need to make a reporter thread because we don't have threads
            IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Starting iperf in a single thread...\n" ) );
            iperf_thread_start( ext_gSettings );
#endif /* HAVE_THREAD */
        } else {
            // neither server nor client mode was specified
            // print usage and exit

#ifdef WIN32
            // In Win32 we also attempt to start a previously defined service
            // Starting in 2.0 to restart a previously defined service
            // you must call iperf with "iperf -D" or using the environment variable
            SERVICE_TABLE_ENTRY dispatchTable[] =
            {
                { TEXT((char *) SZSERVICENAME), (LPSERVICE_MAIN_FUNCTION)service_main},
                { NULL, NULL}
            };

#ifndef NO_DAEMON
            // Only attempt to start the service if "-D" was specified
            if ( !isDaemon(ext_gSettings) ||
                // starting the service by SCM, there is no arguments will be passed in.
                // the arguments will pass into Service_Main entry.
                !StartServiceCtrlDispatcher(dispatchTable) )
                // If the service failed to start then print usage
#endif /* NO_DAEMON */
#endif /* WIN32 */
            {
                fprintf( stderr, usage_short, argv[0], argv[0] );
                IPERF_DEBUGF( MEMFREE_DEBUG | IPERF_DBG_TRACE, IPERF_MEMFREE_MSG( ext_gSettings ) );
                FREE_PTR( ext_gSettings );
                ext_gSettings = NULL;
                break;
            }
        }

        // wait for other (client, server) threads to complete
        //    IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Waiting for other (client, server) threads to complete...\n" ) );
        //    thread_joinall();

#ifdef NO_EXIT
        /* We can't run the atexit function */
#ifdef WIN32
        // Shutdown Winsock
        WSACleanup();
#endif /* WIN32 */
        // clean up the list of clients
        Iperf_destroy ( &clients );

        // shutdown the thread subsystem
        IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Deinitializing the thread subsystem.\n" ) );

        IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Destroying report condition.\n" ) );
        Condition_Destroy( &ReportCond );
        IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Destroying report done condition.\n" ) );
        Condition_Destroy( &ReportDoneCond );
        IPERF_DEBUGF( MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Destroying group condition mutex.\n" ) );
        Mutex_Destroy( &groupCond );
        IPERF_DEBUGF( MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Destroying clients mutex.\n" ) );
        Mutex_Destroy( &clients_mutex );

#ifdef IPERF_DEBUG
        debug_init();
#endif /* IPERF_DEBUG */
#endif /* NO_EXIT */
    } while ((ext_gSettings->flags & FLAG_DAEMON) &&
              (ext_gSettings->mThreadMode != kMode_Client));

    // all done!
    IPERF_DEBUGF( IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Done!\n" ) );
    return 0;
} // end main

#ifndef NO_INTERRUPTS
/* -------------------------------------------------------------------
 * Signal handler sets the sInterupted flag, so the object can
 * respond appropriately.. [static]
 * ------------------------------------------------------------------- */
void Sig_Interupt( int inSigno ) {
#ifdef HAVE_THREAD
    // We try to not allow a single interrupt handled by multiple threads
    // to completely kill the app so we save off the first thread ID
    // then that is the only thread that can supply the next interrupt
    if ( thread_equalid( sThread, thread_zeroid() ) ) {
        sThread = thread_getid();
    } else if ( thread_equalid( sThread, thread_getid() ) ) {
        sig_exit( inSigno );
    }

    // global variable used by threads to see if they were interrupted
    sInterupted = 1;

    // with threads, stop waiting for non-terminating threads
    // (ie Listener Thread)
    thread_release_nonterm( 1 );
#else
    // without threads, just exit quietly, same as sig_exit()
    sig_exit( inSigno );
#endif /* HAVE_THREAD */
} // end Sig_Interupt
#endif /* NO_INTERRUPTS */

#ifndef NO_EXIT
/* -------------------------------------------------------------------
 * Any necesary cleanup before Iperf quits. Called at program exit,
 * either by exit() or terminating main().
 * ------------------------------------------------------------------- */
void cleanup( void ) {
#ifdef WIN32
    // Shutdown Winsock
    WSACleanup();
#endif /* WIN32 */
    // clean up the list of clients
    Iperf_destroy ( &clients );

    // shutdown the thread subsystem
    IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Deinitializing the thread subsystem.\n" ) );
    thread_destroy( );

    IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Destroying report condition.\n" ) );
    Condition_Destroy( &ReportCond );
    IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Destroying report done condition.\n" ) );
    Condition_Destroy( &ReportDoneCond );
    IPERF_DEBUGF( MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Destroying group condition mutex.\n" ) );
    Mutex_Destroy( &groupCond );
    IPERF_DEBUGF( MUTEX_DEBUG | IPERF_DBG_TRACE, ( "Destroying clients mutex.\n" ) );
    Mutex_Destroy( &clients_mutex );

#ifdef IPERF_DEBUG
    debug_init();
#endif /* IPERF_DEBUG */
} // end cleanup
#endif /* NO_EXIT */
