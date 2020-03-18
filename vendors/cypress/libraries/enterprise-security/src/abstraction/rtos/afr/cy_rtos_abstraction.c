/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *  Implements functions for calling AFR RTOS APIs
 *
 *  This file provides specific RTOS APIs
 *
 */
#include "cy_rtos_abstraction.h"
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"

void* cy_rtos_malloc( uint32_t size )
{
    return pvPortMalloc( size );
}

void* cy_rtos_calloc( uint32_t num, uint32_t size )
{
    char *p;
    
    p = pvPortMalloc( size * num );
    if (!p)
    {
        return NULL;
    }

    memset( p, 0, size * num);
    return p;
}

void cy_rtos_free( void* p )
{
    vPortFree( p );
}
