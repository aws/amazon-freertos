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
 * Thread.h
 * by Kevin Gibbs <kgibbs@nlanr.net>
 *
 * Based on:
 * Thread.hpp
 * by Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * The thread subsystem is responsible for all thread functions. It
 * provides a thread implementation agnostic interface to Iperf. If 
 * threads are not available (HAVE_THREAD is undefined), thread_start
 * does not start a new thread but just launches the specified object 
 * in the current thread. Everything that defines a thread of 
 * execution in Iperf is contained in an thread_Settings structure. To
 * start a thread simply pass one such structure into thread_start.
 * ------------------------------------------------------------------- */

#ifndef THREAD_H
#define THREAD_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if   defined(HAVE_POSIX_THREAD) && !defined(NO_THREADS)
	/* Definitions for Posix Threads (pthreads) */
    #include <pthread.h>
	typedef pthread_t nthread_t;
    #define HAVE_THREAD 1
#elif defined(HAVE_WIN32_THREAD) && !defined(NO_THREADS)
	/* Definitions for Win32 NT Threads */
	typedef DWORD nthread_t;
    #define HAVE_THREAD 1
#elif defined (HAVE_FREERTOS_THREAD) && !defined(NO_THREADS)
	typedef xTaskHandle nthread_t;
	#define HAVE_THREAD 1
#elif defined(HAVE_THREADX_THREAD) && !defined(NO_THREADS)
	typedef TX_THREAD nthread_t;
	#define HAVE_THREAD 1
#else
	/* Definitions for no threads */
	typedef int nthread_t;
    #undef HAVE_THREAD
#endif /* defined(HAVE_POSIX_THREAD) && !defined(NO_THREADS) */

// Forward declaration
struct thread_Settings;

#if IPERF_DEBUG
/* This must match ThreadMode */
static const char * const thread_names[] =
{
    (char*) "iperf_unknown",
    (char*) "iperf_server",
    (char*) "iperf_client",
    (char*) "iperf_reporter",
    (char*) "iperf_listener"
};
#endif /* IPERF_DEBUG */

#include "Condition.h"

// initialize or destroy the thread subsystem
void iperf_thread_init( );
void iperf_thread_destroy( );

// start or stop a thread executing
void iperf_thread_start( struct thread_Settings* thread );
void iperf_thread_stop( struct thread_Settings* thread );

/* wait for this or all threads to complete */
void iperf_thread_joinall( void );

int iperf_thread_numuserthreads( void );

// set a thread to be ignorable, so joinall won't wait on it
void iperf_thread_setignore( void );
void iperf_thread_unsetignore( void );

// Used for threads that may never terminate (ie Listener Thread)
void iperf_thread_register_nonterm( void );
void iperf_thread_unregister_nonterm( void );
int iperf_thread_release_nonterm( int force );

int iperf_thread_reporterdone( void );

/* -------------------------------------------------------------------
 * Return the current thread's ID.
 * ------------------------------------------------------------------- */
#if   defined(HAVE_POSIX_THREAD) && !defined(NO_THREADS)
    #define iperf_thread_getid() pthread_self()
#elif defined(HAVE_WIN32_THREAD) && !defined(NO_THREADS)
    #define iperf_thread_getid() GetCurrentThreadId()
#elif defined(HAVE_FREERTOS_THREAD) && !defined(NO_THREADS)
	#define thread_getid() xTaskGetCurrentTaskHandle()
#elif defined(HAVE_THREADX_THREAD) && !defined(NO_THREADS)
	#define iperf_thread_getid() *(tx_thread_identify())
#else
    #define iperf_thread_getid() 0
#endif /* defined(HAVE_POSIX_THREAD) && !defined(NO_THREADS) */

int iperf_thread_equalid( nthread_t inLeft, nthread_t inRight );

nthread_t iperf_thread_zeroid( void );
    
#if   defined(HAVE_WIN32_THREAD)
    DWORD WINAPI iperf_thread_run_wrapper( void* paramPtr );
#else
    void*       iperf_thread_run_wrapper( void* paramPtr );
#endif /* HAVE_WIN32_THREAD */

void iperf_thread_rest ( void );

// defined in launch.cpp
void server_spawn( struct thread_Settings* thread );
void client_spawn( struct thread_Settings* thread );
void client_init( struct thread_Settings* clients );
void listener_spawn( struct thread_Settings* thread );

// defined in reporter.c
void reporter_spawn( struct thread_Settings* thread );

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* THREAD_H */
