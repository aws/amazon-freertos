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

/**
 * The implementation of Condition Variables using semaphores.
 *
 * Adapted from http://research.microsoft.com/pubs/64242/ImplementingCVs.pdf
 *
 */

#if defined(AFR) && defined(HAVE_THREADS)

#include "headers.h"
#include "Condition.h"

#include <FreeRTOS.h>
#include <semphr.r>
#include <portmacro.h>

void Lock_Initialize( Lock *m )
{
    /*
     * sm = new Semaphore( );
     * sm.count = 1;
     * sm.limit = 1;
     */
    m->sm = xSemaphoreCreateCounting( 1, 1 );
}


void Lock_Destroy( Lock *m )
{
    if ( m->sm != NULL)
    {
        vQueueDelete( m->sm );
        m->sm = NULL;
    }
}


void Lock_Acquire( Lock *m )
{
    /*
     * sm.P( );
     */
    xSemaphoreTake( m->sm, portMAX_DELAY );
}


void Lock_Release( Lock *m )
{
    /*
     * sm.V( );
     */
    xSemaphoreGive( m->sm );
}


void Condition_Initialize( Condition *c )
{
    if ( c->m == NULL )
    {
        /* this.m = m; */
        Lock *m = (Lock*) malloc ( sizeof( Lock ) );
        FAIL( m == NULL, ( "Unable to allocate memory for Lock m.\n" ), NULL );
        IPERF_MEMALLOC_DEBUG_MSG( m, sizeof( Lock ) );
        Lock_Initialize( m );
        c->m = m;

        /* int waiters = 0; */
        c->waiters = 0;

        /*
         * s = new Semaphore( );
         * s.count = 0;
         * s.limit = 999999;
         */
        c->s = xSemaphoreCreateCounting( portMAX_DELAY, 0 );

        /*
         * x = new Semaphore( );
         * x.count = 1;
         * x.limit = 1;
         */
        c->x = xSemaphoreCreateCounting( 1, 1 );

        /*
         * h = new Semaphore( );
         * h.count = 0;
         * h.limit = 999999;
         */
        c->h = xSemaphoreCreateCounting( portMAX_DELAY, 0 );
    }
    else
    {
        fprintf( stderr, "Condition already has a lock. Unable to initialize a new lock.\n" );
    }
}


void Condition_Destroy( Condition *c )
{
    Lock_Destroy( c->m );
    c->m = NULL;

    host_rtos_deinit_semaphore( c->s );
    c->s = NULL;

    host_rtos_deinit_semaphore( c->x );
    c->x = NULL;

    host_rtos_deinit_semaphore( c->h );
    c->h = NULL;
}


void Condition_Wait( Condition *c )
{
     /* x.P( ); */
    xSemaphoreTake( c->x, portMAX_DELAY );

    /* waiters++; */
    {
        c->waiters++;
    }

    /* x.V( ); */
    xSemaphoreGive( c->x );

    /* m.Release() */
    Lock_Release( c->m );

    /* s.P( ); */
    xSemaphoreTake( c->s, portMAX_DELAY );

    /* h.V( ); */
    xSemaphoreGive( c->h );

    /* m.Acquire( ); */
     Lock_Acquire( c->m );
}


void Condition_Signal( Condition *c )
{
    /* x.P( ); */
    xSemaphoreTake( c->x, portMAX_DELAY );

    {
        /* if ( waiters > 0 ) */
        if ( c->waiters > 0 )
        {
            /* waiters--; */
            c->waiters--;

            /* s.V( ); */
            xSemaphoreGive( c->s );

            /* h.P( ); */
            xSemaphoreTake( c->s, portMAX_DELAY );
        }
    }

    /* x.V( ); */
    xSemaphoreGive( c->x );
}


void Condition_Broadcast( Condition *c )
{
    /* x.P( ); */
    xSemaphoreTake( c->x, portMAX_DELAY );

    {
        /* for ( int i = 0; i < waiters; i++ ) */
        unsigned int i;
        for ( i = 0; c->waiters >= 0  &&  i < (unsigned) c->waiters; i++ )
        {
            /* s.V( ); */
            xSemaphoreGive( c->s );
        }

        /* while ( waiters > 0 ) */
        while ( c->waiters > 0 )
        {
            /* waiters--; */
            c->waiters--;

            /* h.P( ); */
            xSemaphoreTake( c->h, portMAX_DELAY );
        }
    }

    /* x.V( ); */
    xSemaphoreGive( c->x );
}


void Condition_Lock( Condition *c )
{
    Lock_Acquire( c->m );
}


void Condition_Unlock( Condition *c )
{
    Lock_Release( c-> m );
}

#endif
