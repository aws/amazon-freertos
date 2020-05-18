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
 * Condition.h
 * by Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * An abstract class for waiting on a condition variable. If
 * threads are not available, this does nothing.
 * ------------------------------------------------------------------- */

#ifndef CONDITION_H
#define CONDITION_H

#include "iperf_mutex.h"

#if   defined(HAVE_POSIX_THREAD) && !defined(NO_THREADS)
typedef struct Condition {
    pthread_cond_t mCondition;
    pthread_mutex_t mMutex;
} Condition;
#elif defined(HAVE_WIN32_THREAD) && !defined(NO_THREADS)
typedef struct Condition {
    HANDLE mCondition;
    HANDLE mMutex;
} Condition;
#elif (defined(HAVE_FREERTOS_THREAD) || defined(HAVE_THREADX_THREAD)) && !defined(NO_THREADS)
typedef struct Lock {
    host_semaphore_type_t sm;
} Lock;

void Lock_Initialize(Lock *m);
void Lock_Destroy(Lock *m);
void Lock_Acquire(Lock *m);
void Lock_Release(Lock *m);

typedef struct Condition {
    host_semaphore_type_t s;
    host_semaphore_type_t x;
    Lock *m;
    int waiters;
    host_semaphore_type_t h;
} Condition;

void Condition_Initialize(Condition *c);
void Condition_Destroy(Condition *c);
void Condition_Wait(Condition *c);
void Condition_Signal(Condition *c);
void Condition_Broadcast(Condition *c);
void Condition_Lock(Condition *c);
void Condition_Unlock(Condition *c);
#else
/* Generic dummy implementation */
typedef struct Condition {
    int mCondition;
    int mMutex;
} Condition;
#endif /* defined(HAVE_POSIX_THREAD) && !defined(NO_THREADS) */

#define Condition_Lock(Cond) \
    Mutex_Lock(&Cond.mMutex)

#define Condition_Unlock(Cond) \
    Mutex_Unlock(&Cond.mMutex)

    // initialize condition
#if   defined(HAVE_POSIX_THREAD) && !defined(NO_THREADS)
    #define Condition_Initialize(Cond)                    \
        do {                                              \
            Mutex_Initialize(&(Cond)->mMutex);            \
            pthread_cond_init(&(Cond)->mCondition, NULL); \
        } while (0)
#elif defined(HAVE_WIN32_THREAD) && !defined(NO_THREADS)
    // set all conditions to be broadcast
    // unfortunately in Win32 you have to know at creation
    // whether the signal is broadcast or not.
    #define Condition_Initialize( Cond )                               \
        do {                                                           \
            Mutex_Initialize(&(Cond)->mMutex);                         \
            (Cond)->mCondition = CreateEvent(NULL, true, false, NULL); \
        } while (0)
#else
    #define Condition_Initialize(Cond)
#endif /* HAVE_POSIX_THREAD */

    // destroy condition
#if   defined(HAVE_POSIX_THREAD) && !defined(NO_THREADS)
    #define Condition_Destroy(Cond)                    \
        do {                                           \
            pthread_cond_destroy(&(Cond)->mCondition); \
            Mutex_Destroy(&(Cond)->mMutex);            \
        } while (0)
#elif defined(HAVE_WIN32_THREAD) && !defined(NO_THREADS)
    #define Condition_Destroy(Cond)          \
        do {                                 \
            CloseHandle((Cond)->mCondition); \
            Mutex_Destroy(&(Cond)->mMutex);  \
        } while (0)
#else
    #define Condition_Destroy(Cond)
#endif /* HAVE_POSIX_THREAD */

    // sleep this thread, waiting for condition signal
#if   defined(HAVE_POSIX_THREAD) && !defined(NO_THREADS)
    #define Condition_Wait(Cond) \
        pthread_cond_wait(&(Cond)->mCondition, &(Cond)->mMutex)
    #define Condition_Wait_Event(Cond)          \
        do {                                    \
            Mutex_Lock(&(Cond)->mMutex);        \
            pthread_cond_wait(&(Cond)->mCondition, &(Cond)->mMutex); \
            Mutex_Unlock(&(Cond)->mMutex);      \
        } while (0)
#elif defined(HAVE_WIN32_THREAD) && !defined(NO_THREADS)
    // atomically release mutex and wait on condition,                      
    // then re-acquire the mutex
    #define Condition_Wait(Cond)         \
        do {                             \
            SignalObjectAndWait((Cond)->mMutex, (Cond)->mCondition, INFINITE, false); \
            Mutex_Lock(&(Cond)->mMutex); \
        } while (0)
    #define Condition_Wait_Event(Cond)          \
        do {                                    \
            Mutex_Lock(&(Cond)->mMutex);        \
            SignalObjectAndWait((Cond)->mMutex, (Cond)->mCondition, INFINITE, false); \
        } while (0)
#else
    #define Condition_Wait(Cond)
    #define Condition_Wait_Event(Cond)
#endif /* HAVE_POSIX_THREAD */

    // sleep this thread, waiting for condition signal,
    // but bound sleep time by the relative time inSeconds.
#if   defined(HAVE_POSIX_THREAD) && !defined(NO_THREADS)
    #define Condition_TimedWait(Cond, inSeconds)         \
        do {                                             \
            struct timespec absTimeout;                  \
            absTimeout.tv_sec  = time(NULL) + inSeconds; \
            absTimeout.tv_nsec = 0;                      \
           pthread_cond_timedwait(&(Cond)->mCondition, &(Cond)->mMutex, &absTimeout); \
        } while (0)
#elif defined(HAVE_WIN32_THREAD) && !defined(NO_THREADS)
    // atomically release mutex and wait on condition,
    // then re-acquire the mutex
    #define Condition_TimedWait(Cond, inSeconds) \
        do {                                     \
            SignalObjectAndWait((Cond)->mMutex, (Cond)->mCondition, inSeconds*1000, false); \
            Mutex_Lock(&(Cond)->mMutex);         \
        } while (0)
#else
    #define Condition_TimedWait(Cond, inSeconds)
#endif /* HAVE_POSIX_THREAD */

    // send a condition signal to wake one thread waiting on condition
    // in Win32, this actually wakes up all threads, same as Broadcast
    // use PulseEvent to auto-reset the signal after waking all threads
#if   defined(HAVE_POSIX_THREAD) && !defined(NO_THREADS)
    #define Condition_Signal(Cond) \
        pthread_cond_signal(&(Cond)->mCondition)
#elif defined(HAVE_WIN32_THREAD) && !defined(NO_THREADS)
    #define Condition_Signal(Cond) \
        PulseEvent((Cond)->mCondition)
#else
    #define Condition_Signal(Cond)
#endif /* HAVE_POSIX_THREAD */

    // send a condition signal to wake all threads waiting on condition
#if   defined(HAVE_POSIX_THREAD) && !defined(NO_THREADS)
    #define Condition_Broadcast(Cond) \
        pthread_cond_broadcast(&(Cond)->mCondition)
#elif defined(HAVE_WIN32_THREAD) && !defined(NO_THREADS)
    #define Condition_Broadcast(Cond) \
        PulseEvent((Cond)->mCondition)
#else
    #define Condition_Broadcast(Cond)
#endif /* HAVE_POSIX_THREAD */

#endif /* CONDITION_H */
