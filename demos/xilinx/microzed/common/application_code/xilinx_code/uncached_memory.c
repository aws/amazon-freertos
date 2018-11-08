/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 * uncached_memory.c
 *
 * This module will declare 1 MB of memory and switch off the caching for it.
 *
 * pucGetUncachedMemory( ulSize ) returns a trunc of this memory with a length
 * rounded up to a multiple of 4 KB.
 *
 * ucIsCachedMemory( pucBuffer ) returns non-zero if a given pointer is NOT
 * within the range of the 1 MB non-cached memory.
 *
 */

/*
 * After "_end", 1 MB of uncached memory will be allocated for DMA transfers.
 * Both the DMA descriptors as well as all EMAC TX-buffers will be allocated in
 * uncached memory.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Xilinx includes. */
#include "Zynq/x_emacpsif.h"
#include "Zynq/x_topology.h"
#include "xstatus.h"
#include "xparameters.h"
#include "xparameters_ps.h"
#include "xil_exception.h"
#include "xil_mmu.h"

/* Dem includes. */
#include "uncached_memory.h"
//#include "UDPLoggingPrintf.h"

#define UNCACHED_MEMORY_SIZE	0x100000ul
#define DDR_MEMORY_END	(XPAR_PS7_DDR_0_S_AXI_HIGHADDR+1)

static void vInitialiseUncachedMemory( void );

static uint8_t *pucHeadOfMemory;
static uint32_t ulMemorySize;
static uint8_t *pucStartOfMemory = NULL;
/*-----------------------------------------------------------*/

uint8_t ucIsCachedMemory( const uint8_t *pucBuffer )
{
uint8_t ucReturn;

	if( ( pucStartOfMemory != NULL ) &&
		( pucBuffer >= pucStartOfMemory ) &&
		( pucBuffer < ( pucStartOfMemory + UNCACHED_MEMORY_SIZE ) ) )
	{
		ucReturn = pdFALSE;
	}
	else
	{
		ucReturn = pdTRUE;
	}

	return ucReturn;
}
/*-----------------------------------------------------------*/

uint8_t *pucGetUncachedMemory( uint32_t ulSize )
{
uint8_t *pucReturn;
uint32_t ulSkipSize;

	if( pucStartOfMemory == NULL )
	{
		vInitialiseUncachedMemory( );
	}
	if( ( pucStartOfMemory == NULL ) || ( ulSize > ulMemorySize ) )
	{
		pucReturn = NULL;
	}
	else
	{
		pucReturn = pucHeadOfMemory;
		ulSkipSize = ( ulSize + 0x1000ul ) & ~0xffful;
		pucHeadOfMemory += ulSkipSize;
		ulMemorySize -= ulSkipSize;
	}

	return pucReturn;
}
/*-----------------------------------------------------------*/

static void vInitialiseUncachedMemory( )
{
extern u8 _end;

	/* At the end of program's space... */
	pucStartOfMemory = (uint8_t *) &_end;

	/* Align the start address to 1 MB boundary. */
	pucStartOfMemory = (uint8_t *)( ( ( uint32_t )pucStartOfMemory + UNCACHED_MEMORY_SIZE ) & ( ~( UNCACHED_MEMORY_SIZE - 1 ) ) );

	if( ( ( u32 )pucStartOfMemory ) + UNCACHED_MEMORY_SIZE > DDR_MEMORY_END )
	{
		vLoggingPrintf( "vInitialiseUncachedMemory: Can not allocate uncached memory\n" );
	}
	else
	{
		/* Some objects want to be stored in uncached memory. Hence the 1 MB
		address range that starts after "_end" is made uncached by setting
		appropriate attributes in the translation table. */
		Xil_SetTlbAttributes( ( uint32_t )pucStartOfMemory, 0xc02 ); /* addr, attr */

		/* For experiments in the SDIO driver, make the remaining uncached memory
		public */
		pucHeadOfMemory = pucStartOfMemory;
		ulMemorySize = UNCACHED_MEMORY_SIZE;
		memset( pucStartOfMemory, '\0', UNCACHED_MEMORY_SIZE );
	}
}
