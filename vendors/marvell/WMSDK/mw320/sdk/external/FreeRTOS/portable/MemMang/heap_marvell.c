/*
    FreeRTOS V7.0.0 - Copyright (C) 2011 Real Time Engineers Ltd.
	

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/*
   * (C) Copyright 2011
   * Marvell Semiconductor <www.marvell.com>
   *
   * This code is for internal use of Marvell was forked from
   * heap_2.c. The code to be upstreamed is patched in heap_2.c.
   * 
   * Changes:
   * - Added re-alloc support.
   * - Added free blocks merge support.
   */


#include <stdlib.h>
#include <string.h>
#include <wmstdio.h>
#include <compat_attribute.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"

/* #define DEBUG_HEAP */
/* #define HEAP_PUT_RANDOM_DATA */

#ifdef DEBUG_HEAP
/*
 * This adds allocation traces. All allocations, reallocations and free
 * will be printed.
 */
/* #define ALLOC_TRACE */
/*
 * This enables extra debugging features viz. guard bytes, information
 * about calling function and acutal block size is printed in
 * traces. Please note that enabling this will increase the metadata size
 * for each allocation.
 */
/* #define DEBUG_HEAP_EXTRA */
#endif /* DEBUG_HEAP */

#ifdef DEBUG_HEAP_EXTRA
#warning "Please note that the crypto operations using Big Numbers will fail as DEBUG_HEAP_EXTRA is enabled"
#endif

#ifdef ALLOC_TRACE
#define ATRACE wmprintf
#else /* ! ALLOC_TRACE */
#define ATRACE(...)
#endif /* ALLOC_TRACE */

#ifdef DEBUG_HEAP
#define DTRACE wmprintf
#else /* ! DEBUG_HEAP */
#define DTRACE(...)
#endif /* DEBUG_HEAP */

#undef ASSERT
#define ASSERT(_cond_) \
	if(!(_cond_)) {				\
		DTRACE("ASSERT: %s: %d : %s\n\r", __FILE__, __LINE__, #_cond_); \
		while(1){}						\
	}								\

#if 0

#include "memdebug.h"

#else

#define SET_CALLER_ADDR(pxBlock)
#define GET_CALLER_ADDR(pxBlock)

#define SET_ACTUAL_SIZE(...)
#define GET_ACTUAL_SIZE(...)

#define pre_alloc_hook(...)
#define post_alloc_hook(...)
#define pre_free_hook(...)
#define post_free_hook(...)

#endif

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/*
 * Some assumptions for this algo. Needs to be peer reviewed:
 * 1. Each block when is freed, will always have a xBlockLink at its end (except end block)
*/

#if portBYTE_ALIGNMENT < 2
#error We need the lower bit of xBlockLink->xBlockSize for remembering whether next block is free or allocated.
#endif
#define BLOCK_ALLOCATED 0x1

/* 
 * Below variables are linker script variables. In case you have to change
 * heap parameters like size or start address please do so in your linker
 * script.
 */
extern unsigned _heap_end, _heap_start;
static unsigned configTOTAL_HEAP_SIZE;
static portBASE_TYPE xHeapHasBeenInitialised = pdFALSE;

/* Allocate the memory for the heap.  The struct is used to force byte
alignment without using any non-portable code. */
static union xRTOS_HEAP
{
	#if portBYTE_ALIGNMENT == 8
		volatile portDOUBLE dDummy;
	#else
		volatile unsigned long ulDummy;
	#endif
	unsigned char *ucHeap;
} xHeap;

/*
 * Note 1:
 * The xBlockSize may be bigger than (xWantedSize + heapSTRUCT_SIZE +
 * Alignments). How?
 *
 * Normally a big free block is split into two whenever the
 * required size is less than the free block size. However, if we take into
 * consideration the overhead bytes needed for each allocation, after a
 * split of a block the remaining free block's size should be enough to
 * hold the overhead bytes plus some usable area. If the remaining block is
 * not large enough, then the allocated block is not split. Thus, we have
 * some bytes at the end of the block which will never be used by anybody
 * as long as this block is allocated. Note that this is not a functional
 * problem or a bug. It is just how this allocator is designed.
 * 
 * So that our memory corruption detection is effective,  the guard bytes
 * need to immediately after (and before) the "actual" allocation used by
 * the application. So, we do put them there. However, during free of the
 * block (when we verify the guard bytes integrity) we have no way to know
 * exactly at what offset  the "actual" allocation ends (where we have put
 * the guard bytes). xBlockLink :: xBlockSize is size of the entire block
 * including the unused bytes. So, we introduce a new member to store the
 * actual size of the block. If will be compiled in only when the guard byte
 * feature is enabled.
 */


/* Define the linked list structure.  This is used to link free blocks in order
of their size. */
typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	struct A_BLOCK_LINK *pxPrev; /*<< The prev free block in the list by address. */
	size_t xBlockSize;						/*<< The size of the free block. */
#ifdef DEBUG_HEAP_EXTRA
	/* Please see note 1 above for rationale */
	size_t xActualBlockSize;
	/* To know who allocated this block */
	void *xCallerAddr;
#endif /* DEBUG_HEAP_EXTRA */
} __MAY_ALIAS__ xBlockLink;


static const unsigned short  heapSTRUCT_SIZE	= ( sizeof( xBlockLink )) + portBYTE_ALIGNMENT -
	( sizeof( xBlockLink ) % portBYTE_ALIGNMENT );
#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( heapSTRUCT_SIZE * 2 ) )

/* Create a couple of list links to mark the start and end of the list. */
static xBlockLink xStart, xEnd;

/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
static size_t xFreeBytesRemaining;

/* STATIC FUNCTIONS ARE DEFINED AS MACROS TO MINIMIZE THE FUNCTION CALL DEPTH. */


static size_t lastHeapAddress, startHeapAddress;


#define BLOCK_SIZE(_x_) (_x_->xBlockSize & ~BLOCK_ALLOCATED)
#define NEXT_BLOCK(_x_) ((xBlockLink *)((size_t)_x_ + BLOCK_SIZE(_x_)))
#define PREV_BLOCK(_x_) (_x_->pxPrev)
#define IS_LAST_BLOCK(_x_)   ((size_t)NEXT_BLOCK(_x_) == lastHeapAddress)
#define IS_FIRST_BLOCK(_x_)   ((size_t)_x_ == startHeapAddress)
#define IS_ALLOCATED_BLOCK(_x_) (_x_->xBlockSize & BLOCK_ALLOCATED)
#define IS_FREE_BLOCK(_x_) (!IS_ALLOCATED_BLOCK(_x_))
#define SET_ALLOCATED(_x_) ( _x_->xBlockSize |= BLOCK_ALLOCATED )
#define SET_FREE(_x_) ( _x_->xBlockSize &= ~BLOCK_ALLOCATED )

#ifdef FREERTOS_ENABLE_MALLOC_STATS
static heapAllocatorInfo_t hI;
#endif // FREERTOS_ENABLE_MALLOC_STATS

static inline void randomizeAreaData(unsigned char *addr, int size)
{
#ifdef HEAP_PUT_RANDOM_DATA
	int i = 0;
	for(i = 0; i < size; i++)
		addr[i] = (unsigned char)rand();
#endif /* HEAP_PUT_RANDOM_DATA */
}

/* 
 * Returns the information of amount of memory lost in overhead for
* managing the allocated blocks. Note that the bytes used for storing
* metadata for free blocks is not accounted for. 
*
* This API returns the total bytes allocated and total bytes used for metadata.
* 
* totalBytesAllocated = ApplicationUsableBytes + totalBytesOverhead
*/
#ifdef FREERTOS_ENABLE_MALLOC_STATS
size_t vPortBiggestFreeBlockSize()
{
	xBlockLink *pxBlock = &xStart;
	while( pxBlock->pxNextFreeBlock != &xEnd )
		pxBlock = pxBlock->pxNextFreeBlock;

	return BLOCK_SIZE(pxBlock);
}

const heapAllocatorInfo_t *getheapAllocInfo()
{
	/* Fill up remaining members */
	hI.freeSize = xFreeBytesRemaining;
	hI.biggestFreeBlockAvailable = vPortBiggestFreeBlockSize();
	return &hI;
}
#endif // FREERTOS_ENABLE_MALLOC_STATS

/*
 * Insert a block into the list of free blocks - which is ordered by size of
 * the block.  Small blocks at the start of the list and large blocks at the end
 * of the list.
 */
/* TBD: Make this function force inline */
static inline void prvInsertBlockIntoFreeList( xBlockLink * pxBlockToInsert )
{
	xBlockLink *pxIterator;
	xBlockLink *xBlockToMerge;
	size_t xBlockSize;

	ASSERT(IS_FREE_BLOCK(pxBlockToInsert));

	/* We have a block which we are about to declare as a free block. 
	  *  Lets find out if there is a free block in front of us and back
	  *  of us  
	  */ 

	/*
	TRACE("pxBlockToInsert->pxNextFreeBlock: %x ->pxPrev = %x\n\r",
	      pxBlockToInsert->pxNextFreeBlock, pxBlockToInsert->pxPrev);
	*/
	
        /* Check for front merge */
	if ( !IS_LAST_BLOCK( pxBlockToInsert ) ) { 
		if ( IS_FREE_BLOCK(NEXT_BLOCK( pxBlockToInsert ) ) ) {
			xBlockToMerge = NEXT_BLOCK( pxBlockToInsert );

			/*  Find out xBlockToMerge's location on the free list.*/
			for( pxIterator = &xStart;
			     pxIterator->pxNextFreeBlock != xBlockToMerge &&
				     pxIterator->pxNextFreeBlock != NULL;
			     pxIterator = pxIterator->pxNextFreeBlock ) {}

#ifdef DEBUG_HEAP
			if(! ( pxIterator->pxNextFreeBlock == xBlockToMerge ) ) {
				/*
				 * This is not a good situation. The
				 * problem is that data structures here are
				 * showing that the next block is free. But
				 * ths free block could not be found in the
				 * free list.
				 */
				ATRACE("Target block dump :\n\r");
				ATRACE("pxNextFreeBlock : 0x%x\n\r", 
				      xBlockToMerge->pxNextFreeBlock);
				ATRACE("pxPrev          : 0x%x\n\r", 
				      xBlockToMerge->pxPrev);
				ATRACE("xBlockSize      : %d (0x%x)\n\r", 
				      xBlockToMerge->xBlockSize, xBlockToMerge->xBlockSize);
#ifdef DEBUG_HEAP_EXTRA
				ATRACE("xActualBlockSize: %d\n\r", 
				      xBlockToMerge->xActualBlockSize);
#endif /* DEBUG_HEAP_EXTRA */
				ATRACE("Panic\n\r");
				while(1) {}
			}
#endif /* DEBUG_HEAP */
			ASSERT( pxIterator->pxNextFreeBlock == xBlockToMerge );
			
			//TRACE("Merge: F\n\r");
			/* Delete node from Free list */
			pxIterator->pxNextFreeBlock = xBlockToMerge->pxNextFreeBlock;
			
			/* Delete xBlockToMerge node from Serial List */
			if( ! IS_LAST_BLOCK( xBlockToMerge ) )
				NEXT_BLOCK( xBlockToMerge )->pxPrev = pxBlockToInsert;

			/* Update node size */
			pxBlockToInsert->xBlockSize += BLOCK_SIZE( xBlockToMerge );
			/* Now forget about xBlockToMerge */
		}
	}

	/* Check for back merge */
	if ( ! IS_FIRST_BLOCK(pxBlockToInsert) ) {
		if ( IS_FREE_BLOCK( PREV_BLOCK(pxBlockToInsert ))) {

			xBlockToMerge = PREV_BLOCK(pxBlockToInsert);

			/* Find out xBlockToMerge's location on the free list  */
			for( pxIterator = &xStart; 
			     pxIterator->pxNextFreeBlock != xBlockToMerge &&
				     pxIterator->pxNextFreeBlock != NULL;
			     pxIterator = pxIterator->pxNextFreeBlock ) {}
			
			ASSERT( pxIterator->pxNextFreeBlock == xBlockToMerge );
			//TRACE("Merge: R\n\r");
			/* Delete xBlockToMerge node from Free list */
			pxIterator->pxNextFreeBlock = xBlockToMerge->pxNextFreeBlock;
				
			/* Delete _ pxBlockToInsert _ node from Serial List */
			if( ! IS_LAST_BLOCK( pxBlockToInsert ) )
				NEXT_BLOCK( pxBlockToInsert )->pxPrev = xBlockToMerge;

			/* Update node size */
			xBlockToMerge->xBlockSize += BLOCK_SIZE( pxBlockToInsert );
				
			/* Now forget about pxBlockToInsert */
			pxBlockToInsert = xBlockToMerge;
		}
	}       

	xBlockSize = pxBlockToInsert->xBlockSize;

	
	
	/* Iterate through the list until a block is found that has a larger size */
	/* than the block we are inserting. */
	for( pxIterator = &xStart; pxIterator->pxNextFreeBlock->xBlockSize < xBlockSize;
	     pxIterator = pxIterator->pxNextFreeBlock )
	{
		/* There is nothing to do here - just iterate to the correct position. */
	}

	/* Update the list to include the block being inserted in the correct */
	/* position. */
	pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
	pxIterator->pxNextFreeBlock = pxBlockToInsert;
}
/*-----------------------------------------------------------*/

#ifdef __ICCARM__

extern unsigned RAM0_END;
#pragma section = "HEAP_1"

#define WMSDK_HEAP_START_ADDR (__section_begin("HEAP_1"))
#define WMSDK_HEAP_SIZE \
	((unsigned) &RAM0_END - (unsigned) WMSDK_HEAP_START_ADDR)

#else

#define WMSDK_HEAP_START_ADDR (&_heap_start)
#define WMSDK_HEAP_SIZE ((unsigned) &_heap_end - (unsigned) &_heap_start)

#endif /* __ICCARM__ */

void prvHeapInit()
{
	xBlockLink *pxFirstFreeBlock;

	xHeap.ucHeap = (unsigned char *) WMSDK_HEAP_START_ADDR;
	configTOTAL_HEAP_SIZE = WMSDK_HEAP_SIZE;
	xFreeBytesRemaining = configTOTAL_HEAP_SIZE;
	
#ifdef HEAP_PUT_RANDOM_DATA
	randomizeAreaData((unsigned char*)xHeap.ucHeap,
			  configTOTAL_HEAP_SIZE);
#else
	memset(xHeap.ucHeap, 0x00, configTOTAL_HEAP_SIZE);
#endif /* HEAP_PUT_RANDOM_DATA */

	/* xStart is used to hold a pointer to the first item in the list of free */
	/* blocks.  The void cast is used to prevent compiler warnings. */
	xStart.pxNextFreeBlock = ( void * ) xHeap.ucHeap;
	xStart.pxPrev = NULL;
	xStart.xBlockSize = ( size_t ) 0;

	/* xEnd is used to mark the end of the list of free blocks. */
	xEnd.xBlockSize = configTOTAL_HEAP_SIZE;
	xEnd.pxPrev = NULL;
	xEnd.pxNextFreeBlock = NULL;
							
	/* To start with there is a single free block that is sized to take up the		\
	entire heap space. */
	pxFirstFreeBlock = ( xBlockLink * ) xHeap.ucHeap;
	pxFirstFreeBlock->xBlockSize = configTOTAL_HEAP_SIZE;
	pxFirstFreeBlock->pxNextFreeBlock = &xEnd;
	pxFirstFreeBlock->pxPrev = NULL;
	
	lastHeapAddress = ( size_t )&xHeap.ucHeap[ configTOTAL_HEAP_SIZE ];
	startHeapAddress = ( size_t )&xHeap.ucHeap[ 0 ];

#ifdef FREERTOS_ENABLE_MALLOC_STATS
	memset(&hI, 0x00, sizeof(heapAllocatorInfo_t));
	hI.heapSize = pxFirstFreeBlock->xBlockSize;
	hI.freeSize = pxFirstFreeBlock->xBlockSize;
	hI.minOverheadPerAllocation = heapSTRUCT_SIZE;
#endif /* FREERTOS_ENABLE_MALLOC_STATS */
}

int prvHeapAddMemBank(char *chunk_start, size_t size)
{
	xBlockLink *pxIterator;
	xBlockLink *pxNewBlock;
	xBlockLink *pxAllocBlock;
	xBlockLink *p;

	/* Ensure that blocks are always aligned to the required number of bytes. */
	DTRACE("AddMemBank: Received size: %u\r\n", size);

	/* Make sure chunk_start is on portBYTE_ALIGNMENT */
	if( (unsigned long) chunk_start & portBYTE_ALIGNMENT_MASK )
	{
		chunk_start += ( portBYTE_ALIGNMENT -
				 ( (unsigned long) chunk_start & portBYTE_ALIGNMENT_MASK ) );
		size -=  ( portBYTE_ALIGNMENT -
				 ( (unsigned long) chunk_start & portBYTE_ALIGNMENT_MASK ) );
	}
	if( size & portBYTE_ALIGNMENT_MASK )
	{
		/* Let go of the last few bytes */
		size -=  ( size & portBYTE_ALIGNMENT_MASK ) ;
	}
	DTRACE("AddMemBank: Fixed size: %u\r\n", size);

	vTaskSuspendAll();
	/* Initialize heap if not already */
	if( xHeapHasBeenInitialised == pdFALSE ) {
		prvHeapInit();
		xHeapHasBeenInitialised = pdTRUE;
	}


	for( pxIterator = ( xBlockLink * )xHeap.ucHeap;
	     NEXT_BLOCK(pxIterator) != (void *)lastHeapAddress;
	     pxIterator = NEXT_BLOCK(pxIterator) ) { }

	if (!IS_FREE_BLOCK(pxIterator)) {
		/* For adding a new chunk it is important that the last part of the
		 * previous chunk is empty (ie end of SRAM0 should be available)
		 */
		DTRACE("End of the first bank is not free. Cannot add new memory bank");
		return pdFAIL;
	}

	if ((chunk_start - (char *)lastHeapAddress) < heapMINIMUM_BLOCK_SIZE) {
		/* Too small hole in between */
		DTRACE("Too Small Hole in between %p %p %d\r\n", chunk_start, lastHeapAddress, heapMINIMUM_BLOCK_SIZE);
		return pdFAIL;
	}

	if (chunk_start < (char *)startHeapAddress) {
		DTRACE("Adding of a memory bank BEFORE the default heap is not supported");
		return pdFAIL;
	}

	/* pxIterator should now point to the free block that contains the end
	 * of the previous chunk.
	 * We have to split this now, such that end of the previous chunk
	 * contains the heapSTRUCT. This heapSTRUCT should say that the entire
	 * hole from end of SRAM0 to start of the free space in SRAM1 is a huge
	 * allocation.
	 */

	/* Fix the size of the last free block */
	/* XXX Check if we are creating a block lesser than the minimum allowed
	 * block size */
	DTRACE("AddMemBank: Last Block: %p size %u\r\n", pxIterator, pxIterator->xBlockSize);
	pxIterator->xBlockSize -= heapSTRUCT_SIZE;
	unsigned unalignment = (pxIterator->xBlockSize & portBYTE_ALIGNMENT_MASK);
	if (unalignment) {
		pxIterator->xBlockSize -= unalignment;
	}
	DTRACE("AddMemBank: Fixed Last Block size %u\r\n", pxIterator->xBlockSize);

	/* Create a new block that marks the hole as allocated */
	pxAllocBlock = NEXT_BLOCK(pxIterator);

	DTRACE("AddMemBank: Alloc Block: %p \r\n", pxAllocBlock);
	pxAllocBlock->xBlockSize = chunk_start - (char *)lastHeapAddress +
		heapSTRUCT_SIZE +
		unalignment;

	DTRACE("AddMemBank: Alloc Block: %p size %u\r\n", pxAllocBlock, pxAllocBlock->xBlockSize);
	/* These are never used for an allocated block*/
	pxAllocBlock->pxNextFreeBlock = NULL;
	pxAllocBlock->pxPrev = pxIterator;
	SET_ALLOCATED(pxAllocBlock);

	/* Manage all accounting variables */
	lastHeapAddress = (size_t)(chunk_start + size);
	xFreeBytesRemaining += size;
	configTOTAL_HEAP_SIZE += size;
#ifdef FREERTOS_ENABLE_MALLOC_STATS
	hI.heapSize += size;
#endif // FREERTOS_ENABLE_MALLOC_STATS
	xEnd.xBlockSize = configTOTAL_HEAP_SIZE;

	/* Create a new block at the start of the chunk_start */
	/* Ensure the allocation flags aren't part of the block size */
	pxNewBlock = NEXT_BLOCK(pxAllocBlock);
	DTRACE("AddMemBank: New Block: %p \r\n", pxNewBlock);
	pxNewBlock->xBlockSize = size;
	pxNewBlock->pxNextFreeBlock = &xEnd;
	pxNewBlock->pxPrev = pxAllocBlock;

	/* Iterate through the list until a block is found that has a larger size */
	/* than the block we are inserting. */
	for( p = &xStart; (p->pxNextFreeBlock != &xEnd) && (p->pxNextFreeBlock->xBlockSize < size);
	     p = p->pxNextFreeBlock )
	{
		/* There is nothing to do here - just iterate to the correct position. */
	}

	if (p->pxNextFreeBlock == &xEnd) {
		/* Ours is the largest block */
		p->pxNextFreeBlock = pxNewBlock;
	} else {
		/* Update the list to include the block being inserted in the correct */
		/* position. */
		pxNewBlock->pxNextFreeBlock = p->pxNextFreeBlock;
		p->pxNextFreeBlock = pxNewBlock;
	}

	DTRACE("AddMemBank: New Block: %p size %u\r\n", pxNewBlock, pxNewBlock->xBlockSize);

	xTaskResumeAll();
	return pdPASS;
}
/*-----------------------------------------------------------*/

void *pvPortMalloc( size_t xWantedSize )
{
	xBlockLink *pxBlock = NULL, *pxPreviousBlock, *pxNewBlockLink;
	void *pvReturn = NULL;

	if(!xWantedSize)
		return  NULL;

	pre_alloc_hook( xWantedSize );

	vTaskSuspendAll();
	{
		/* If this is the first call to malloc then the heap will require
		initialisation to setup the list of free blocks. */
		if( xHeapHasBeenInitialised == pdFALSE )
		{
			prvHeapInit();
			xHeapHasBeenInitialised = pdTRUE;
		}

		/* The wanted size is increased so it can contain a xBlockLink
		structure in addition to the requested amount of bytes. */
		if( xWantedSize > 0 )
		{
			xWantedSize += heapSTRUCT_SIZE;

			/* Ensure that blocks are always aligned to the required number of bytes. */
			if( xWantedSize & portBYTE_ALIGNMENT_MASK )
			{
				/* Byte alignment required. */
				xWantedSize += ( portBYTE_ALIGNMENT -
						 ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
			}
		}

		if( ( xWantedSize > 0 ) && ( xWantedSize < configTOTAL_HEAP_SIZE ) )
		{
			/* Blocks are stored in byte order - traverse the list from the start
			(smallest) block until one of adequate size is found. */
			pxPreviousBlock = &xStart;
			pxBlock = xStart.pxNextFreeBlock;
			while( ( pxBlock->xBlockSize < xWantedSize ) && ( pxBlock->pxNextFreeBlock ) )
			{
				pxPreviousBlock = pxBlock;
				pxBlock = pxBlock->pxNextFreeBlock;
			}

			/* If we found the end marker then a block of adequate size was not found. */
			if( pxBlock != &xEnd )
			{
				/* Return the memory space - jumping over the xBlockLink structure
				at its start. */
				pvReturn = ( void * ) ( ( ( unsigned char * ) pxPreviousBlock->pxNextFreeBlock )
							+ heapSTRUCT_SIZE );

#ifdef FREERTOS_ENABLE_MALLOC_STATS
				hI.totalAllocations++;
#endif // FREERTOS_ENABLE_MALLOC_STATS


				/* This block is being returned for use so must be taken off the
				list of free blocks. */
				pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;
				pxBlock->pxNextFreeBlock = NULL;

				/* If the block is larger than required it can be split into two. */
				if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
				{
					/* This block is to be split into two.  Create a new block
					following the number of bytes requested. The void cast is
					used to prevent byte alignment warnings from the compiler. */
					pxNewBlockLink = ( void * ) ( ( ( unsigned char * ) pxBlock ) + xWantedSize );

					/* Calculate the sizes of two blocks split from the single
					block. */
					pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
					/* Assume bit 0 is 0 i.e. BLOCK_ALLOCATED flag is clear */
					pxBlock->xBlockSize = xWantedSize; 

					/* Add the new block to the serial list */
					pxNewBlockLink->pxPrev = pxBlock;
					if( ! IS_LAST_BLOCK(pxNewBlockLink) )
						NEXT_BLOCK( pxNewBlockLink )->pxPrev = 
							pxNewBlockLink;

					SET_ALLOCATED(pxBlock);

					/* insert the new block into the list of free blocks. */
					prvInsertBlockIntoFreeList( pxNewBlockLink );
				}
				else {
					SET_ALLOCATED(pxBlock);
				}
				xFreeBytesRemaining -= BLOCK_SIZE(pxBlock);
			}
		}
	}
	xTaskResumeAll();

#if( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if( pvReturn == NULL )
		{
			DTRACE("Heap allocation failed.\n\r"
				       "Requested: %d\n\r"
				       "Available : %d\n\r", xWantedSize, xFreeBytesRemaining);
			extern void vApplicationMallocFailedHook( void );
			vApplicationMallocFailedHook();
		}
	}
#else
	if( pvReturn == NULL ) {
		DTRACE("Heap allocation failed.\n\r"
		      "Requested: %d\n\r"
		      "Available : %d\n\r", xWantedSize, xFreeBytesRemaining);
#ifdef FREERTOS_ENABLE_MALLOC_STATS
		hI.failedAllocations++;
#endif /* FREERTOS_ENABLE_MALLOC_STATS */
	}
#endif

	if(pvReturn) {
		SET_ACTUAL_SIZE( pxBlock );
		SET_CALLER_ADDR( pxBlock );
		ATRACE("MDC A %10x %6d %10d R: %x\r\n", pvReturn ,
		       BLOCK_SIZE( pxBlock ),
		       xFreeBytesRemaining, __builtin_return_address(0));
		randomizeAreaData((unsigned char*)pvReturn, 
				  BLOCK_SIZE( pxBlock ) - heapSTRUCT_SIZE);
		post_alloc_hook( pvReturn );

#ifdef FREERTOS_ENABLE_MALLOC_STATS
		if ((configTOTAL_HEAP_SIZE - xFreeBytesRemaining) > hI.peakHeapUsage) {
			hI.peakHeapUsage =
				(configTOTAL_HEAP_SIZE - xFreeBytesRemaining);
		}
#endif
	}
	
	return pvReturn;
}
/*-----------------------------------------------------------*/
void vPortFree( void *pv )
{
pre_free_hook(pv);
unsigned char *puc = ( unsigned char * ) pv;
xBlockLink *pxLink;

 ASSERT( ( pv >= (void*)WMSDK_HEAP_START_ADDR ) ||
	 ( pv == NULL ) );
 ASSERT( ( pv <= (void*)lastHeapAddress ) ||
	 ( pv == NULL ) );

	if( pv )
	{
		/* The memory being freed will have an xBlockLink structure immediately
		before it. */
		puc -= heapSTRUCT_SIZE;

		/* This casting is to keep the compiler from issuing warnings. */
		pxLink = ( void * ) puc;

		ATRACE("MDC F %10x %6d %10d R: %x\r\n", 
		      puc + heapSTRUCT_SIZE, BLOCK_SIZE( pxLink ), 
		      xFreeBytesRemaining + BLOCK_SIZE(pxLink),
		      __builtin_return_address(0));

		post_free_hook(((unsigned)puc + heapSTRUCT_SIZE), 
				GET_ACTUAL_SIZE( pxLink ));

		randomizeAreaData((unsigned char*)((unsigned)puc + heapSTRUCT_SIZE),
				  BLOCK_SIZE( pxLink ) - heapSTRUCT_SIZE);

		vTaskSuspendAll();
		{
			/* Add this block to the list of free blocks. */
			SET_FREE( pxLink );
			xFreeBytesRemaining += BLOCK_SIZE(pxLink);
			prvInsertBlockIntoFreeList( ( ( xBlockLink * ) pxLink ) );
#ifdef FREERTOS_ENABLE_MALLOC_STATS
			hI.totalAllocations--;
#endif // FREERTOS_ENABLE_MALLOC_STATS
		}
		xTaskResumeAll();
	}
}
/*-----------------------------------------------------------*/
void* pvPortReAlloc( void *pv,  size_t xWantedSize )
{
	ASSERT( ( pv >= (void*)WMSDK_HEAP_START_ADDR ) ||
	 ( pv == NULL ) );
        ASSERT( ( pv <= (void*)lastHeapAddress ) ||
	 ( pv == NULL )  );


	pre_free_hook(pv);
	unsigned char *puc = ( unsigned char * ) pv;
#ifdef ALLOC_TRACE
	unsigned char *old_ptr= puc;
#endif /* ALLOC_TRACE */
	xBlockLink *pxLink;

	if( pv )
	{
		if( !xWantedSize )  
			{
				vPortFree( pv );
				return NULL;
			}
		
		void *newArea = pvPortMalloc( xWantedSize );
		if( newArea )  
			{
				/* The memory being freed will have an xBlockLink structure immediately
				   before it. */
				puc -= heapSTRUCT_SIZE;
				
				/* This casting is to keep the compiler from issuing warnings. */
				pxLink = ( void * ) puc;

				ATRACE("MDC F %10x %10d %10d\r\n", 
				      puc + heapSTRUCT_SIZE, BLOCK_SIZE( pxLink ), 
				      xFreeBytesRemaining + BLOCK_SIZE(pxLink));

				post_free_hook( ( ( unsigned )puc + heapSTRUCT_SIZE ), 
						GET_ACTUAL_SIZE( pxLink ) );

				int oldSize = BLOCK_SIZE( pxLink ) - heapSTRUCT_SIZE;
				int copySize = ( oldSize < xWantedSize ) ?
					oldSize : xWantedSize;
				memcpy( newArea, pv, copySize );

				randomizeAreaData((unsigned char*)
						  ((unsigned)pxLink + heapSTRUCT_SIZE),
						  BLOCK_SIZE( pxLink ) - heapSTRUCT_SIZE);
				
				vTaskSuspendAll();
				{
					/* Add this block to the list of free blocks. */
					SET_FREE( pxLink );
					xFreeBytesRemaining += BLOCK_SIZE(pxLink);

					prvInsertBlockIntoFreeList( ( ( xBlockLink * ) pxLink ) );
#ifdef FREERTOS_ENABLE_MALLOC_STATS
					hI.totalAllocations--;
#endif // FREERTOS_ENABLE_MALLOC_STATS
				}
				xTaskResumeAll();
				return newArea;
			}
	}
	else if( xWantedSize )
		return pvPortMalloc( xWantedSize );
	else
		return NULL;
	
	return NULL;
}
/*-----------------------------------------------------------*/

size_t xPortGetFreeHeapSize( void )
{
	return xFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

void vPortInitialiseBlocks( void )
{
	/* This just exists to keep the linker quiet. */
}

int vHeapSelfTest( int trace )
{
	{ // find out total size of heap
		xBlockLink *pxBlock = ( xBlockLink * )xHeap.ucHeap;
		int totalSize = 0;
		if( trace ) {
#ifdef DEBUG_HEAP_EXTRA
			DTRACE( "%s%11s%11s%11s%11s%11s%11s%11s\n\r",
					"TAG", "My Addr", "Next Free", "Prev Block", "Block Size", 
					"Act. Size", "Caller", "Block Type");
#else /* ! DEBUG_HEAP_EXTRA */
			DTRACE( "%s%11s%11s%11s%11s%11s\n\r",
					"TAG", "My Addr", "Next Free", "Prev Block", "Block Size", "Block Type");
#endif /* DEBUG_HEAP_EXTRA */
			DTRACE( "HST%11x%11x%11x%11d%11c\n\r",
				&xStart, xStart.pxNextFreeBlock, xStart.pxPrev,  
					xStart.xBlockSize, 'X' );
		}

		/* A counter to prevent infinite loop */
		int max_sane_blocks = 10000;
	        while( max_sane_blocks-- ) {
			if( trace ) {
#ifdef DEBUG_HEAP_EXTRA
				DTRACE( "HST%11x%11x%11x%11d%11d%11x ",
						pxBlock, pxBlock->pxNextFreeBlock, pxBlock->pxPrev, 
						BLOCK_SIZE( pxBlock ), GET_ACTUAL_SIZE( pxBlock ),
						GET_CALLER_ADDR( pxBlock ) );
#else /* ! DEBUG_HEAP_EXTRA */
				DTRACE( "HST%11x%11x%11x%11d ",
					pxBlock, pxBlock->pxNextFreeBlock, pxBlock->pxPrev, 
						BLOCK_SIZE( pxBlock ) );
#endif /* DEBUG_HEAP_EXTRA */
				if (BLOCK_SIZE( pxBlock ) == 0) {
					DTRACE("Unknown fault: Cannot find next block\n\r");
					break;
				}

				if( IS_ALLOCATED_BLOCK(pxBlock) ) {
					DTRACE( "A\n\r" );
				} else {
					DTRACE( "F\n\r" );
				}
			}
			totalSize += BLOCK_SIZE( pxBlock );
			if( IS_LAST_BLOCK( pxBlock ) )
				break;
			pxBlock = NEXT_BLOCK( pxBlock );
		}
		if( totalSize != configTOTAL_HEAP_SIZE )
			return 1;
	}
	return 0;
}
