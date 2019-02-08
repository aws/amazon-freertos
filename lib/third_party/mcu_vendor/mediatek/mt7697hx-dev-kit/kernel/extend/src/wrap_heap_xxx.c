
#include "FreeRTOS.h"

#include <string.h>

#include "os.h"

/*-----------------------------------------------------------*/

//lib/third_party/mcu_vendor/mtk/mt76x7_combo/kernel/extend/src/wrap_heap.c
#include "../../../../../../lib/FreeRTOS/portable/MemMang/heap_4.c"

/*-----------------------------------------------------------*/

void *pvPortCalloc( size_t nmemb, size_t size )
{
    void *pvReturn;

    pvReturn = os_malloc( nmemb * size );
    if( pvReturn != NULL )
    {
      os_memset( pvReturn, 0, nmemb * size );
    }

    return pvReturn;
}

/*-----------------------------------------------------------*/


void *pvPortRealloc( void *pv, size_t size )
{
    void        *pvReturn   = NULL;

#if 0
    assert(0);
#else
    size_t       xBlockSize = 0;
    uint8_t     *puc        = ( uint8_t * ) pv;
    BlockLink_t *pxLink     = NULL;

    pvReturn = os_malloc( size );

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
            os_memcpy(pvReturn, pv, xBlockSize);
        else
            os_memcpy(pvReturn, pv, size);

        // Free Original Ptr
        os_free(pv);
    }
#endif

    return pvReturn;
}

/*-----------------------------------------------------------*/

/* Wrap c stand library malloc family, include malloc/calloc/realloc/free to FreeRTOS heap service */
#if defined(__GNUC__)

/*-----------------------------------------------------------*/

void *__wrap_malloc(size_t size)
{
    return os_malloc(size);
}

/*-----------------------------------------------------------*/

void *__wrap_calloc(size_t nmemb, size_t size )
{
    return os_malloc(nmemb * size);
}

/*-----------------------------------------------------------*/

void *__wrap_realloc(void *pv, size_t size )
{
    return os_realloc(pv,size);
}

/*-----------------------------------------------------------*/

void __wrap_free(void *pv)
{
     os_free(pv);
}

/*-----------------------------------------------------------*/

#elif defined(__CC_ARM)

/*-----------------------------------------------------------*/

void *$Sub$$malloc(size_t size)
{
    return os_malloc(size);
}

/*-----------------------------------------------------------*/

void *$Sub$$calloc(size_t nmemb, size_t size )
{
    return os_calloc(nmemb,size);
}

/*-----------------------------------------------------------*/

void *$Sub$$realloc(void *pv, size_t size )
{
    return os_realloc(pv,size);
}

/*-----------------------------------------------------------*/

void $Sub$$free(void *pv)
{
     os_free(pv);
}
#endif /* __GNUC__ */

