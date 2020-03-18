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
 * Thread.c
 * by Kevin Gibbs <kgibbs@nlanr.net>
 *
 * Based on:
 * Thread.cpp
 * by Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * The thread subsystem is responsible for all thread functions. It
 * provides a thread implementation agnostic interface to Iperf. If 
 * threads are not available (HAVE_THREAD is undefined), thread_start
 * does not start a new thread but just launches the specified object 
 * in the current thread. Everything that defines a thread of 
 * execution in Iperf is contained in an thread_Settings structure. To
 * start a thread simply pass one such structure into thread_start.
 * -------------------------------------------------------------------
 * headers
 * uses
 *   <stdlib.h>
 *   <stdio.h>
 *   <assert.h>
 *   <errno.h>
 * Thread.h may include <pthread.h>
 * ------------------------------------------------------------------- */

#include "headers.h"

#include "iperf_thread.h"
#include "iperf_locale.h"
#include "iperf_util.h"
#include "Settings.hpp"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#if 0
/* -------------------------------------------------------------------
 * define static variables.
 * ------------------------------------------------------------------- */

// number of currently running threads
int thread_sNum = 0;
// number of non-terminating running threads (ie listener thread)
int nonterminating_num = 0;
// condition to protect updating the above and alerting on 
// changes to above
Condition thread_sNum_cond;


/* -------------------------------------------------------------------
 * Initialize the thread subsystems variables and set the concurrency
 * level in solaris.
 * ------------------------------------------------------------------- */
void iperf_thread_init( ) {
    Condition_Initialize( &thread_sNum_cond );
#ifdef sun
    /* Solaris apparently doesn't default to timeslicing threads,
     * as such we force it to play nice. This may not work perfectly
     * when _sending_ multiple _UDP_ streams.
     */
    pthread_setconcurrency (3);
#endif /* sun */
} // end thread_init

/* -------------------------------------------------------------------
 * Destroy the thread subsystems variables.
 * ------------------------------------------------------------------- */
void iperf_thread_destroy( ) {
    Condition_Destroy( &thread_sNum_cond );
} // end thread_destroy

/* -------------------------------------------------------------------
 * Start the specified object's thread execution. Increments thread
 * count, spawns new thread, and stores thread ID.
 * ------------------------------------------------------------------- */
void iperf_thread_start( struct thread_Settings* thread ) {
    // Make sure this object has not been started already
    if ( iperf_thread_equalid( thread->mTID, iperf_thread_zeroid() ) ) {
        // Check if we need to start another thread before this one
        if ( thread->runNow != NULL ) {
            iperf_thread_start( thread->runNow );
        }

        // increment thread count
        Condition_Lock( thread_sNum_cond );
        IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Incrementing thread count from %d to %d.\n", thread_sNum, (thread_sNum + 1) ) );
        thread_sNum++;
        Condition_Unlock( thread_sNum_cond );

        IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "Spawning %s thread.\n", thread_names[thread->mThreadMode] ) );
#if   defined( HAVE_POSIX_THREAD )
        // pthreads -- spawn new thread
        if ( pthread_create( &thread->mTID, NULL, iperf_thread_run_wrapper, thread ) != 0 ) {
            WARN( 1, ( "pthread_create failed!\n" ) );

            // decrement thread count
            Condition_Lock( thread_sNum_cond );
            IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Decrementing thread count from %d to %d.\n", thread_sNum, (thread_sNum - 1) ) );
            thread_sNum--;
            Condition_Unlock( thread_sNum_cond );
        }
#elif defined( HAVE_WIN32_THREAD )
        // Win32 threads -- spawn new thread
        // Win32 has a thread handle in addition to the thread ID
        thread->mHandle = CreateThread( NULL, 0, iperf_thread_run_wrapper, thread, 0, &thread->mTID );
        if ( thread->mHandle == NULL ) {
            WARN( 1, ( "CreateThread failed!\n" ) );

            // decrement thread count
            Condition_Lock( thread_sNum_cond );
            IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Decrementing thread count from %d to %d.\n", thread_sNum, (thread_sNum - 1) ) );
            thread_sNum--;
            Condition_Unlock( thread_sNum_cond );
        }
#else
        // single-threaded -- call Run_Wrapper in this thread
        iperf_thread_run_wrapper( thread );
#endif /* HAVE_POSIX_THREAD */
    }
} // end thread_start

/* -------------------------------------------------------------------
 * Stop the specified object's thread execution (if any) immediately.
 * Decrements thread count and resets the thread ID.
 * ------------------------------------------------------------------- */
void iperf_thread_stop( struct thread_Settings* thread ) {
#ifdef HAVE_THREAD
    // Make sure we have been started
    if ( ! iperf_thread_equalid( thread->mTID, iperf_thread_zeroid() ) ) {
        // decrement thread count
        Condition_Lock( thread_sNum_cond );
        IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Decrementing thread count from %d to %d.\n", thread_sNum, (thread_sNum - 1) ) );
        thread_sNum--;
        IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Signaling thread_sNum_cond condition.\n" ) );
        Condition_Signal( &thread_sNum_cond );
        Condition_Unlock( thread_sNum_cond );

        // use exit()   if called from within this thread
        // use cancel() if called from a different thread
        if ( iperf_thread_equalid( thread_getid(), thread->mTID ) ) {
            // Destroy the object
            Settings_Destroy( thread );

            // Exit
#if   defined( HAVE_POSIX_THREAD )
            IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Exiting %s thread.\n", thread_names[thread->mThreadMode] ) );
            pthread_exit( NULL );
#elif defined( HAVE_WIN32_THREAD )
            IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Exiting %s thread.\n", thread_names[thread->mThreadMode] ) );
            CloseHandle( thread->mHandle );
            ExitThread( 0 );
#endif /* HAVE_POSIX_THREAD */
        } else {
            // Cancel
#if   defined( HAVE_POSIX_THREAD )
            // Cray J90 doesn't have pthread_cancel; Iperf works okay without
#ifdef HAVE_PTHREAD_CANCEL
            IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Canceling %s thread.\n", thread_names[thread->mThreadMode] ) );
            pthread_cancel( thread->mTID );
#endif /* HAVE_PTHREAD_CANCEL */
#elif defined(HAVE_WIN32_THREAD)
            // this is a somewhat dangerous function; it's not
            // suggested to Stop() threads a lot.
            IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Terminating %s thread.\n", thread_names[thread->mThreadMode] ) );
            TerminateThread( thread->mHandle, 0 );
#endif /* HAVE_POSIX_THREAD */

            // Destroy the object only after killing the thread
            Settings_Destroy( thread );
        }
    }
#endif /* HAVE_THREAD */
} // end thread_stop

/* -------------------------------------------------------------------
 * This function is the entry point for new threads created in 
 * thread_start. 
 * ------------------------------------------------------------------- */
#if   defined( HAVE_WIN32_THREAD )
DWORD WINAPI
#else
void*
#endif /* HAVE_WIN32_THREAD */
iperf_thread_run_wrapper( void* paramPtr ) {
    struct thread_Settings* thread = (struct thread_Settings*) paramPtr;
    IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_STATE, ( "%s thread is now running. ID is %lu.\n", thread_names[thread->mThreadMode], (long unsigned) thread_getid() ) );

    // which type of object are we
    switch ( thread->mThreadMode ) {
        case kMode_Server:
            {
                /* Spawn a Server thread with these settings */
                server_spawn( thread );
            } break;
        case kMode_Client:
            {
                /* Spawn a Client thread with these settings */
                client_spawn( thread );
            } break;
        case kMode_Reporter:
            {
                /* Spawn a Reporter thread with these settings */
                reporter_spawn( thread );
            } break;
        case kMode_Listener:
            {
                // Increment the non-terminating thread count
                iperf_thread_register_nonterm();
                /* Spawn a Listener thread with these settings */
                listener_spawn( thread );
                // Decrement the non-terminating thread count
                iperf_thread_unregister_nonterm();
            } break;
        default:
            {
                FAIL(1, ( "Unknown Thread Type!\n" ), thread);
            } break;
    }

#ifdef HAVE_POSIX_THREAD
    // detach Thread. If someone already joined it will not do anything
    // If noone has then it will free resources upon return from this
    // function (Run_Wrapper)
    IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Detaching %s thread.\n", thread_names[thread->mThreadMode] ) );
    pthread_detach(thread->mTID);
#endif /* HAVE_POSIX_THREAD */

    // decrement thread count and send condition signal
    Condition_Lock( thread_sNum_cond );
    IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Decrementing thread count from %d to %d.\n", thread_sNum, (thread_sNum - 1) ) );
    thread_sNum--;

    IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Signaling thread_sNum_cond condition.\n" ) );
    Condition_Signal( &thread_sNum_cond );
    Condition_Unlock( thread_sNum_cond );

    // Check if we need to start up a thread after executing this one
    if ( thread->runNext != NULL ) {
        iperf_thread_start( thread->runNext );
    }

    // Destroy this thread object
    Settings_Destroy( thread );

    return 0;
} // end thread_run_wrapper

/* -------------------------------------------------------------------
 * Wait for all thread object's execution to complete. Depends on the
 * thread count being accurate and the threads sending a condition
 * signal when they terminate.
 * ------------------------------------------------------------------- */
void iperf_thread_joinall( void ) {
    Condition_Lock( thread_sNum_cond );
#if IPERF_DEBUG
    int waiting_on = -1;
#endif /* IPERF_DEBUG */
    IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Waiting on thread_sNum_cond condition.\n" ) );
    while ( thread_sNum > 0 ) {
#if IPERF_DEBUG
        if ( thread_sNum != waiting_on )
        {
            waiting_on = thread_sNum;
            IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Waiting on %d threads.\n", waiting_on ) );
        }
#endif /* IPERF_DEBUG */
        Condition_Wait( &thread_sNum_cond );
    }
    Condition_Unlock( thread_sNum_cond );
} // end Joinall

/* -------------------------------------------------------------------
 * Compare the thread ID's (inLeft == inRight); return true if they
 * are equal. On some OS's nthread_t is a struct so == will not work.
 * TODO use pthread_equal. Any Win32 equivalent??
 * ------------------------------------------------------------------- */
int iperf_thread_equalid( nthread_t inLeft, nthread_t inRight ) {
    return(memcmp( &inLeft, &inRight, sizeof(inLeft)) == 0);
} // end thread_equalid

/* -------------------------------------------------------------------
 * Return a zero'd out thread ID. On some OS's nthread_t is a struct
 * so == 0 will not work.
 * [static]
 * ------------------------------------------------------------------- */
nthread_t iperf_thread_zeroid( void ) {
    nthread_t a;
    memset( &a, 0, sizeof(a));
    return a;
} // end thread_zeroid

/* -------------------------------------------------------------------
 * set a thread to be ignorable, so joinall won't wait on it
 * this simply decrements the thread count that joinall uses.
 * This is utilized by the reporter thread which knows when it
 * is ok to quit (aka no pending reports).
 * ------------------------------------------------------------------- */
void iperf_thread_setignore( ) {
    Condition_Lock( thread_sNum_cond );
    IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Setting ignorable thread.\n" ) );
    thread_sNum--;
    IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Signaling thread_sNum_cond condition.\n" ) );
    Condition_Signal( &thread_sNum_cond );
    Condition_Unlock( thread_sNum_cond );
} // end thread_setignore

/* -------------------------------------------------------------------
 * unset a thread from being ignorable, so joinall will wait on it
 * this simply increments the thread count that joinall uses.
 * This is utilized by the reporter thread which knows when it
 * is ok to quit (aka no pending reports).
 * ------------------------------------------------------------------- */
void iperf_thread_unsetignore( void ) {
    Condition_Lock( thread_sNum_cond );
    IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Unsetting ignorable thread.\n" ) );
    thread_sNum++;
    IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Signaling thread_sNum_cond condition.\n" ) );
    Condition_Signal( &thread_sNum_cond );
    Condition_Unlock( thread_sNum_cond );
} // end thread_unsetignore

/* -------------------------------------------------------------------
 * set a thread to be non-terminating, so if you cancel through
 * Ctrl-C they can be ignored by the joinall.
 * ------------------------------------------------------------------- */
void iperf_thread_register_nonterm( void ) {
    Condition_Lock( thread_sNum_cond );
    IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Registering non-terminating thread.\n" ) );
    nonterminating_num++; 
    Condition_Unlock( thread_sNum_cond );
} // end thread_register_nonterm

/* -------------------------------------------------------------------
 * unset a thread from being non-terminating, so if you cancel through
 * Ctrl-C they can be ignored by the joinall.
 * ------------------------------------------------------------------- */
void iperf_thread_unregister_nonterm( void ) {
    Condition_Lock( thread_sNum_cond );
    IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Unregistering non-terminating thread.\n" ) );
    if ( nonterminating_num == 0 ) {
        // nonterminating has been released with release_nonterm
        // Add back to the threads to wait on
        thread_sNum++;
    } else {
        nonterminating_num--; 
    }
    Condition_Unlock( thread_sNum_cond );
} // end thread_unregister_nonterm

/* -------------------------------------------------------------------
 * this function releases all non-terminating threads from the list
 * of active threads, so that when all terminating threads quit
 * the joinall will complete. This is called on a Ctrl-C input. It is
 * also used by the -P usage on the server side
 * ------------------------------------------------------------------- */
int iperf_thread_release_nonterm( int interrupt ) {
    Condition_Lock( thread_sNum_cond );
    IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Releasing all non-terminating threads.\n" ) );
    thread_sNum -= nonterminating_num;
    if ( thread_sNum > 1 && nonterminating_num > 0 && interrupt != 0 ) {
        fprintf( stderr, "%s", wait_server_threads );
    }
    nonterminating_num = 0;
    IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Signaling thread_sNum_cond condition.\n" ) );
    Condition_Signal( &thread_sNum_cond );
    Condition_Unlock( thread_sNum_cond );
    return thread_sNum;
} // end thread_release_nonterm

/* -------------------------------------------------------------------
 * Return the number of threads currently running (doesn't include
 * active threads that have called setdaemon (aka reporter thread))
 * ------------------------------------------------------------------- */
int iperf_thread_numuserthreads( void ) {
    return thread_sNum;
} // end thread_numuserthreads

/*
 * -------------------------------------------------------------------
 * Allow another thread to execute. If no other threads are runable this
 * is not guarenteed to actually rest.
 * ------------------------------------------------------------------- */
void iperf_thread_rest ( void ) {
#if defined( HAVE_THREAD )
#if defined( HAVE_WIN32_THREAD )
    SwitchToThread( );
#endif /* HAVE_WIN32_THREAD */
#endif /* HAVE_THREAD */
} // end thread_rest


/* TODO: This doesn't work on Windows... causes early exit. */
int iperf_thread_reporterdone( void ) {
    int val;

    Condition_Lock( thread_sNum_cond );
    if ( thread_sNum <= 1 )
        val = 1;
    else
        val = 0;
    Condition_Unlock( thread_sNum_cond );

    return val;
} // end thread_reporterdone
#endif
#ifdef __cplusplus
} /* end extern "C" */
#endif /*__cplusplus */
