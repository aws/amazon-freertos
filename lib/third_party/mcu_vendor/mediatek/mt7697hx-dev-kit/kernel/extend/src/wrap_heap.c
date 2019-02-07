
#include "FreeRTOS.h"

#include <string.h>

/*-----------------------------------------------------------*/

//lib/third_party/mcu_vendor/mtk/mt76x7_combo/kernel/extend/src/wrap_heap.c
#include "../../../../../../../lib/FreeRTOS/portable/MemMang/heap_4.c"

/*-----------------------------------------------------------*/

void *pvPortCalloc( size_t nmemb, size_t size )
{
    void *pvReturn;

    pvReturn = pvPortMalloc( nmemb * size );
    if( pvReturn != NULL )
    {
      memset( pvReturn, 0, nmemb * size );
    }

    return pvReturn;
}

/*-----------------------------------------------------------*/


void *pvPortRealloc( void *pv, size_t size )
{
    void        *pvReturn   = NULL;

    size_t       xBlockSize = 0;
    uint8_t     *puc        = ( uint8_t * ) pv;
    BlockLink_t *pxLink     = NULL;

    pvReturn = pvPortCalloc( size, 1 );

    if( pv != NULL )
    {
        // The memory being freed will have an BlockLink_t structure immediately before it.
        puc -= xHeapStructSize;

        // This casting is to keep the compiler from issuing warnings.
        pxLink = ( void * ) puc;

        // Check the block is actually allocated
        configASSERT( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 );
        configASSERT( pxLink->pxNextFreeBlock == NULL );

        // Get Original Block Size
        xBlockSize = (pxLink->xBlockSize & ~xBlockAllocatedBit);

        // Get Original data length
        xBlockSize = (xBlockSize - xHeapStructSize);

        if(xBlockSize < size)
            memcpy(pvReturn, pv, xBlockSize);
        else
            memcpy(pvReturn, pv, size);

        // Free Original Ptr
        vPortFree(pv);
    }

    return pvReturn;
}

/*-----------------------------------------------------------*/

/* Wrap c stand library malloc family, include malloc/calloc/realloc/free to FreeRTOS heap service */
#if defined(__GNUC__)

/*-----------------------------------------------------------*/

void *__wrap_malloc(size_t size)
{
    return pvPortMalloc(size);
}

/*-----------------------------------------------------------*/

void *__wrap_calloc(size_t nmemb, size_t size )
{
    return pvPortCalloc(nmemb,size);
}

/*-----------------------------------------------------------*/

void *__wrap_realloc(void *pv, size_t size )
{
    return pvPortRealloc(pv,size);
}

/*-----------------------------------------------------------*/

void __wrap_free(void *pv)
{
     vPortFree(pv);
}

/*-----------------------------------------------------------*/

#elif defined(__CC_ARM)

/*-----------------------------------------------------------*/

void *$Sub$$malloc(size_t size)
{
    return pvPortMalloc(size);
}

/*-----------------------------------------------------------*/

void *$Sub$$calloc(size_t nmemb, size_t size )
{
    return pvPortCalloc(nmemb,size);
}

/*-----------------------------------------------------------*/

void *$Sub$$realloc(void *pv, size_t size )
{
    return pvPortRealloc(pv,size);
}

/*-----------------------------------------------------------*/

void $Sub$$free(void *pv)
{
     vPortFree(pv);
}
#endif /* __GNUC__ */

