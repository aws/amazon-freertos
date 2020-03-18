/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software"), is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */


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
#include "Condition.h"
#include "Settings.hpp"
#include "iperf_locale.h"

/******************************************************
 *             Static Variables
 ******************************************************/
int         thread_sNum = 0;                /* Number of currently running threads */
int         nonterminating_num = 0;         /* Number of non-terminating running
                                               threads (ie listener thread) */
Condition   thread_sNum_cond;               /* Condition to protect updating the above
                                               and alerting on changes to above */


void iperf_thread_init( void )
{
    IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Initializing thread number condition.\n" ) );
    Condition_Initialize( &thread_sNum_cond );
} /* thread_init */


void iperf_thread_destroy( void )
{
    IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Destroying thread number condition.\n" ) );
    Condition_Destroy( &thread_sNum_cond );
} /* thread_destroy */


void iperf_thread_start( struct thread_Settings *thread )
{
    /* Make sure this object has not been started already */
    if ( iperf_thread_equalid( thread->mTID, iperf_thread_zeroid( ) ) )
    {
        /* Check if we need to start another thread before this one */
        if ( thread->runNow != NULL )
        {
            iperf_thread_start( thread->runNow );
        }

        /* Increment thread count */
        Condition_Lock( &thread_sNum_cond );
        IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Incrementing thread count from %d to %d.\n", thread_sNum, thread_sNum + 1 ) );
        thread_sNum++;
        Condition_Unlock( &thread_sNum_cond );

#ifdef HAVE_THREADS
        xTaskCreate( thread_run_wrapper, (signed char*) thread_names[thread->mThreadMode], IPERF_THREAD_STACKSIZE, (void*) thread, IPERF_THREAD_PRIORITY, &(thread->mTID) );

        if ( thread->mTID == NULL )
        {
            IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE | IPERF_DBG_LEVEL_SERIOUS, ( "xTaskCreate failed.\n" ) );

            /* Decrement thread count */
            Condition_Lock( &thread_sNum_cond );
            IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Decrementing thread count from %d to %d.\n", thread_sNum, thread_sNum - 1 ) )
            thread_sNum--;
            Condition_Unlock( &thread_sNum_cond );

        }
#else /* single threaded */
        /* Call Run_Wrapper in this thread */
        iperf_thread_run_wrapper( thread );
#endif
    }
} /* thread_start */


void iperf_thread_stop( struct thread_Settings *thread )
{
#ifdef HAVE_THREADS
    /* Make sure we have been started */
    if ( !iperf_thread_equalid( thread->mTID, iperf_thread_zeroid( ) ) )
    {
        /* Decrement thread count */
        Condition_Lock( &thread_sNum_cond );
        IPERF_DEBUGF( THREAD_DEBUG | IPERF_DBG_TRACE, ( "Decrementing thread count from %d to %d.\n", thread_sNum, thread_sNum - 1 ) );
        thread_sNum--;
        IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Signaling on thread number condition.\n" ) );
        Condition_Signal( &thread_sNum_cond );
        Condition_Unlock( &thread_sNum_cond );

        /* Use exit()   if called from within this thread */
        /* Use cancel() if called from a different thread */
        if ( iperf_thread_equalid( thread_getid( ), thread->mTID ) )
        {
            /* Destroy the object */
            Settings_Destroy( thread );
        }
        else
        {
            /* Destroy the object only after killing the thread */
            Settings_Destroy( thread );
        }
    }
#endif // HAVE_THREADS
} /* thread_stop */


void *iperf_thread_run_wrapper( void *parameter )
{
    struct thread_Settings *thread = (struct thread_Settings *) parameter;

    /* Which type of object are we? */
    switch ( thread->mThreadMode )
    {
        case kMode_Server:
            /* Spawn a Server thread with these settings */
            server_spawn( thread );
            break;

        case kMode_Client:
            /* Spawn a Client thread with these settings */
            client_spawn( thread );
            break;

        case kMode_Reporter:
            /* Spawn a Reporter thread with these settings */
            reporter_spawn( thread );
            break;

        case kMode_Listener:
            /* Increment the non-terminating thread count */
            iperf_thread_register_nonterm( );

            /* Spawn a Listener thread with these settings */
            listener_spawn( thread );

            /* Decrement the non-terminating thread count */
            iperf_thread_unregister_nonterm( );

            break;

        default:
            FAIL( 1, ( "Unknown thread type!\n" ), thread );
            break;
    }

    /* Decrement thread count and send condition signal */
    Condition_Lock( &thread_sNum_cond );
    IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE, ( "Decrementing thread count from %d to %d.\n", thread_sNum, thread_sNum - 1 ) );
    thread_sNum--;
    IPERF_DEBUGF( CONDITION_DEBUG | IPERF_DBG_TRACE,( "Signaling thread number condition.\n" ) );
    Condition_Signal( &thread_sNum_cond );
    Condition_Unlock( &thread_sNum_cond );

    /* Check if we need to start up a thread after executing this one */
    if ( thread->runNext != NULL )
    {
        iperf_thread_start( thread->runNext );
    }

    /* Destroy this thread object */
    Settings_Destroy( thread );

    return NULL;
} /* thread_run_wrapper */


void iperf_thread_joinall( void )
{
    Condition_Lock( &thread_sNum_cond );

    while ( thread_sNum > 0 )
    {
        Condition_Wait( &thread_sNum_cond );
    }

    Condition_Unlock( &thread_sNum_cond );
} /* thread_joinall */


int iperf_thread_equalid( nthread_t inLeft, nthread_t inRight )
{
    return ( memcmp( &inLeft, &inRight, sizeof( inLeft ) ) == 0 );
}


nthread_t iperf_thread_zeroid( void )
{
    nthread_t a;
    memset( &a, 0, sizeof( a ) );
    return a;
}


void iperf_thread_setignore( void )
{
    Condition_Lock( &thread_sNum_cond );
    thread_sNum--;
    Condition_Signal( &thread_sNum_cond );
    Condition_Unlock( &thread_sNum_cond );
}


void iperf_thread_unsetignore( void )
{
    Condition_Lock( &thread_sNum_cond );
    thread_sNum++;
    Condition_Signal( &thread_sNum_cond );
    Condition_Unlock( &thread_sNum_cond );
}


void iperf_thread_register_nonterm( void )
{
    Condition_Lock( &thread_sNum_cond );
    nonterminating_num++;
    Condition_Unlock( &thread_sNum_cond );
}


void iperf_thread_unregister_nonterm( void )
{
    Condition_Lock( &thread_sNum_cond );
    if ( nonterminating_num == 0 )
    {
        /*
         * Nonterminating has been released with release_nonterm. Add back to
         * the threads to wait on.
         */
        thread_sNum++;
    }
    else
    {
        nonterminating_num--;
    }
    Condition_Unlock( &thread_sNum_cond );
}


int iperf_thread_release_nonterm( int interrupt )
{
    Condition_Lock( &thread_sNum_cond );
    thread_sNum -= nonterminating_num;
    if ( thread_sNum > 1  &&  nonterminating_num > 0  &&  interrupt != 0 )
    {
        fprintf( stderr, "%s", wait_server_threads );
    }
    nonterminating_num = 0;
    Condition_Signal( &thread_sNum_cond );
    Condition_Unlock( &thread_sNum_cond );

    return thread_sNum;
}


int iperf_thread_numuserthreads( void )
{
    return thread_sNum;
}


void iperf_thread_rest ( void )
{
    /* Sleep for 500ms */
	usleep(500*1000);
}

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
