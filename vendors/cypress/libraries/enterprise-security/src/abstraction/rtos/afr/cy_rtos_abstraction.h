/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *  Implements functions for calling AFR RTOS APIs
 *
 *  This file provides specific RTOS APIs
 *
 */
#include "cy_wifi_abstraction.h"

void* cy_rtos_malloc( uint32_t size );
void* cy_rtos_calloc( uint32_t num, uint32_t size );
void cy_rtos_free( void* p );
