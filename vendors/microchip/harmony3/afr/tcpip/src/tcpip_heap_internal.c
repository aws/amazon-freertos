/*******************************************************************************
  TCPIP Heap Allocation Manager

  Summary:
    
  Description:
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/









#include <string.h>
#include <stdlib.h>

#if !defined(__PIC32C__) && !defined(__SAMA5D2__)
#include <sys/kmem.h>
#endif


#include "tcpip/src/tcpip_private.h"
// definitions


// min heap alignment
// always power of 2
#if defined(__PIC32MZ__) || defined(__PIC32WK__)
typedef struct __attribute__((aligned(16)))
{
    uint64_t     pad[2];
}_heap_Align;
#elif defined(__PIC32C__) || defined(__SAMA5D2__)
typedef struct __attribute__((aligned(32)))
{
    uint32_t     pad[8];
}_heap_Align;
#else   // PIC32MX, PIC32MK
typedef uint32_t _heap_Align;
#endif  // defined(__PIC32MZ__) || defined(__PIC32WK__)


typedef union __attribute__((aligned(16))) _tag_headNode
{
    _heap_Align x;
    struct
    {
        union _tag_headNode*	next;
        size_t			        units;
    };
}_headNode;


#define	_TCPIP_HEAP_MIN_BLK_USIZE_  	2	// avoid tiny blocks having units <=  value. 

#define	_TCPIP_HEAP_MIN_BLKS_           64	// efficiency reasons, the minimum heap size that can be handled. 



typedef struct
{
    _headNode*      _heapHead;
    _headNode*      _heapTail;                  // head and tail pointers
    size_t          _heapUnits;                 // size of the heap, units
    size_t          _heapAllocatedUnits;        // how many units allocated out there
    size_t          _heapWatermark;             // max allocated units
    TCPIP_STACK_HEAP_RES  _lastHeapErr;         // last error encountered
    TCPIP_STACK_HEAP_FLAGS _heapFlags;          // heap flags
    void*           allocatedBuffer;            // buffer initially allocated for this heap
    void            (*free_fnc)(void* ptr);     // free function needed to delete the heap
    
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    _headNode*   _heapStart;                    // debug checkpoint     
    _headNode*   _heapEnd;                      // debug checkpoint     
#endif

    OSAL_SEM_HANDLE_TYPE _heapSemaphore;
    
    // alignment padding
    // _headNode _heap[0];           // the heap itself, dynamically allocated

}TCPIP_HEAP_DCPT; // descriptor of a heap


// local data
//

static TCPIP_STACK_HEAP_RES   _TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH);
static void*            _TCPIP_HEAP_Malloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes);
static void*            _TCPIP_HEAP_Calloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize);
static size_t           _TCPIP_HEAP_Free(TCPIP_STACK_HEAP_HANDLE heapH, const void* pBuff);

static size_t           _TCPIP_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           _TCPIP_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           _TCPIP_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH);
static size_t           _TCPIP_HEAP_HighWatermark(TCPIP_STACK_HEAP_HANDLE heapH);
static TCPIP_STACK_HEAP_RES   _TCPIP_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH);
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
static size_t           _TCPIP_HEAP_AllocSize(TCPIP_STACK_HEAP_HANDLE heapH, const void* ptr);
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

// maps a buffer to non cached memory
const void* _TCPIP_HEAP_BufferMapNonCached(const void* buffer, size_t buffSize);




// the heap object
static const TCPIP_HEAP_OBJECT      _tcpip_heap_object = 
{
    .TCPIP_HEAP_Delete = _TCPIP_HEAP_Delete,
    .TCPIP_HEAP_Malloc = _TCPIP_HEAP_Malloc,
    .TCPIP_HEAP_Calloc = _TCPIP_HEAP_Calloc,
    .TCPIP_HEAP_Free = _TCPIP_HEAP_Free,
    .TCPIP_HEAP_Size = _TCPIP_HEAP_Size,
    .TCPIP_HEAP_MaxSize = _TCPIP_HEAP_MaxSize,
    .TCPIP_HEAP_FreeSize = _TCPIP_HEAP_FreeSize,
    .TCPIP_HEAP_HighWatermark = _TCPIP_HEAP_HighWatermark,
    .TCPIP_HEAP_LastError = _TCPIP_HEAP_LastError,
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    .TCPIP_HEAP_AllocSize = _TCPIP_HEAP_AllocSize,
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
};

typedef struct
{
    TCPIP_HEAP_OBJECT   heapObj;    // heap object API
    TCPIP_HEAP_DCPT     heapDcpt;   // private heap object data
}TCPIP_HEAP_OBJ_INSTANCE;



// local prototypes
//

// returns the TCPIP_HEAP_OBJ_INSTANCE associated with a heap handle
// null if invalid
static __inline__ TCPIP_HEAP_OBJ_INSTANCE* __attribute__((always_inline)) _TCPIP_HEAP_ObjInstance(TCPIP_STACK_HEAP_HANDLE heapH)
{
#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
    if(heapH)
    {
        TCPIP_HEAP_OBJ_INSTANCE* pInst = (TCPIP_HEAP_OBJ_INSTANCE*)heapH;
        if(pInst->heapObj.TCPIP_HEAP_Delete == _TCPIP_HEAP_Delete)
        {
            return pInst;
        }
    }
    return 0;
#else
    return (heapH == 0) ? 0 : (TCPIP_HEAP_OBJ_INSTANCE*)heapH;
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 

}

// returns the TCPIP_HEAP_DCPT associated with a heap handle
// null if invalid
static __inline__ TCPIP_HEAP_DCPT* __attribute__((always_inline)) _TCPIP_HEAP_ObjDcpt(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_OBJ_INSTANCE* hInst = _TCPIP_HEAP_ObjInstance(heapH);

    return (hInst == 0) ? 0 : &hInst->heapDcpt;
}

// API

// do the CacheInvalidate before calling specific pool/heap creation routines
// actually, for the pool, only the pool itself needs to be k1, not the pool entries!
TCPIP_STACK_HEAP_HANDLE TCPIP_HEAP_CreateInternal(const TCPIP_STACK_HEAP_INTERNAL_CONFIG* pHeapConfig, TCPIP_STACK_HEAP_RES* pRes)
{
    TCPIP_HEAP_DCPT* hDcpt;
    TCPIP_HEAP_OBJ_INSTANCE* hInst;
    size_t          heapSize, heapUnits, headerSize;
    uint8_t*        allocatedHeapBuffer;
    uint8_t*        alignHeapBuffer;
    size_t          heapBufferSize;
    uintptr_t       alignBuffer;
    uint8_t*        heapStart;
    TCPIP_STACK_HEAP_RES  res;
    

    while(true)
    {
        hDcpt =0;
        hInst = 0;
        
        if( pHeapConfig == 0)
        {
            res = TCPIP_STACK_HEAP_RES_INIT_ERR;
            break;
        }
        

        heapBufferSize = pHeapConfig->heapSize;
        allocatedHeapBuffer = (uint8_t*)(*pHeapConfig->malloc_fnc)(heapBufferSize);

        if(allocatedHeapBuffer == 0)
        {
            res = TCPIP_STACK_HEAP_RES_CREATE_ERR;
            break;
        }


        // align properly: round up and truncate
        alignBuffer = ((uintptr_t)allocatedHeapBuffer + sizeof(_heap_Align)-1 ) & ~(sizeof(_heap_Align)-1);
        heapBufferSize -= (uint8_t*)alignBuffer - allocatedHeapBuffer ;
        heapBufferSize &= ~(sizeof(_heap_Align)-1) ;
        alignHeapBuffer = (uint8_t*)alignBuffer;

        headerSize = ((sizeof(TCPIP_HEAP_OBJ_INSTANCE) + sizeof(_headNode) - 1) / sizeof(_headNode)) * sizeof(_headNode);

        heapSize = heapBufferSize - headerSize;

        heapUnits = heapSize / sizeof(_headNode);           // adjust to multiple of heads

        if(heapUnits < _TCPIP_HEAP_MIN_BLKS_)
        {
            res = TCPIP_STACK_HEAP_RES_BUFF_SIZE_ERR;
            break;
        }


        // check if mapping needed; always alloc uncached!
        // if((pHeapConfig->heapFlags & TCPIP_STACK_HEAP_FLAG_ALLOC_UNCACHED) != 0) 
        {
            alignHeapBuffer = (uint8_t*)_TCPIP_HEAP_BufferMapNonCached(alignHeapBuffer, heapBufferSize);
        }
        heapStart = alignHeapBuffer + headerSize; 
        hInst = (TCPIP_HEAP_OBJ_INSTANCE*)alignHeapBuffer;
        hInst->heapObj = _tcpip_heap_object;
        hDcpt = &hInst->heapDcpt;
        hDcpt->_heapHead = (_headNode*)heapStart;
        hDcpt->_heapHead->units = heapUnits;
        hDcpt->_heapHead->next = 0;
        hDcpt->_heapTail = hDcpt->_heapHead;
        hDcpt->_heapUnits = heapUnits;
        hDcpt->_heapAllocatedUnits = 0;
        hDcpt->_heapWatermark = 0;
        hDcpt->_lastHeapErr = TCPIP_STACK_HEAP_RES_OK;
        hDcpt->_heapFlags = pHeapConfig->heapFlags;
        hDcpt->allocatedBuffer = allocatedHeapBuffer;
        hDcpt->free_fnc = pHeapConfig->free_fnc;
#ifdef TCPIP_STACK_DRAM_DEBUG_ENABLE
        hDcpt->_heapStart = hDcpt->_heapHead;
        hDcpt->_heapEnd = hDcpt->_heapHead + heapUnits;
#endif
        if(OSAL_SEM_Create(&hDcpt->_heapSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1) != OSAL_RESULT_TRUE)
        {
            (*pHeapConfig->free_fnc)(allocatedHeapBuffer);
            res = TCPIP_STACK_HEAP_RES_SYNCH_ERR;
            break;
        }

        res = TCPIP_STACK_HEAP_RES_OK;
        break;
    }

    if(pRes)
    {
        *pRes = res;
    }

    return hInst;
    
}

// internal functions
//
// deallocates the heap
// NOTE: check is done if some blocks are still in use!
static TCPIP_STACK_HEAP_RES _TCPIP_HEAP_Delete(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_OBJ_INSTANCE* hInst;
    TCPIP_HEAP_DCPT*   hDcpt;

    hInst = _TCPIP_HEAP_ObjInstance(heapH);

    if(hInst == 0)
    {
        return TCPIP_STACK_HEAP_RES_NO_HEAP; 
    }

    hDcpt = &hInst->heapDcpt;
    
    if(hDcpt->_heapHead)
    {
    #ifdef TCPIP_STACK_DRAM_DEBUG_ENABLE
        if(hDcpt->_heapAllocatedUnits != 0 || hDcpt->_heapHead != hDcpt->_heapStart || hDcpt->_heapHead->units != hDcpt->_heapUnits)
    #else
        if(hDcpt->_heapAllocatedUnits != 0)
    #endif
        {
            //  deallocating a heap not completely de-allocated or corrupted
            return (hDcpt->_lastHeapErr = TCPIP_STACK_HEAP_RES_IN_USE); 
        }

        OSAL_SEM_Delete(&hDcpt->_heapSemaphore);
        // invalidate it
        memset(&hInst->heapObj, 0, sizeof(hInst->heapObj));
        (*hDcpt->free_fnc)(hDcpt->allocatedBuffer);

        return TCPIP_STACK_HEAP_RES_OK;
    }
    
    return (hDcpt->_lastHeapErr = TCPIP_STACK_HEAP_RES_IN_USE); 
}


static void* _TCPIP_HEAP_Malloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nBytes)
{
	_headNode	*ptr,*prev;
	size_t      nunits;
    TCPIP_HEAP_DCPT*  hDcpt;


    hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);

	if(hDcpt == 0 || nBytes == 0)
	{
		return 0;
	}
	
	nunits=(nBytes+sizeof(_headNode)-1)/sizeof(_headNode)+1;	// allocate units   
	prev=0;

    OSAL_SEM_Pend(&hDcpt->_heapSemaphore, OSAL_WAIT_FOREVER);

	for(ptr = hDcpt->_heapHead; ptr != 0; prev = ptr, ptr = ptr->next)
	{
		if(ptr->units >= nunits)
		{   // found block
			if(ptr->units-nunits <= _TCPIP_HEAP_MIN_BLK_USIZE_)
			{
				nunits=ptr->units;	// get the whole block
			}

            if(ptr->units == nunits)
			{   // exact match
				if(prev)
				{
					prev->next = ptr->next;
				}
				else
				{
					hDcpt->_heapHead = ptr->next;
                    prev = hDcpt->_heapHead;
				}

                if(hDcpt->_heapTail == ptr)
                {
                    hDcpt->_heapTail = prev;
                }
			}
			else
			{   // larger than we need
				ptr->units -= nunits;
				ptr += ptr->units;
				ptr->units = nunits;
			}

            if((hDcpt->_heapAllocatedUnits += nunits) > hDcpt->_heapWatermark)
            {
                hDcpt->_heapWatermark = hDcpt->_heapAllocatedUnits;
            }
            OSAL_SEM_Post(&hDcpt->_heapSemaphore);
            return ptr + 1;
		}
	}

    hDcpt->_lastHeapErr = TCPIP_STACK_HEAP_RES_NO_MEM;
    OSAL_SEM_Post(&hDcpt->_heapSemaphore);
    return 0;
}

static void* _TCPIP_HEAP_Calloc(TCPIP_STACK_HEAP_HANDLE heapH, size_t nElems, size_t elemSize)
{
    void* pBuff = _TCPIP_HEAP_Malloc(heapH, nElems * elemSize);
    if(pBuff)
    {
        memset(pBuff, 0, nElems * elemSize);
    }

    return pBuff;

}

static size_t _TCPIP_HEAP_Free(TCPIP_STACK_HEAP_HANDLE heapH, const void* pBuff)
{  
    TCPIP_HEAP_DCPT*  hDcpt;
	_headNode	*hdr,*ptr;
    int         fail;
    size_t      freedUnits;

    hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt == 0 || pBuff == 0)
	{
        return 0;
    }

    ptr = (_headNode*)pBuff-1;

    OSAL_SEM_Pend(&hDcpt->_heapSemaphore, OSAL_WAIT_FOREVER);

#ifdef TCPIP_STACK_DRAM_DEBUG_ENABLE
    if(ptr < hDcpt->_heapStart || ptr+ptr->units > hDcpt->_heapEnd)
    {
        hDcpt->_lastHeapErr = TCPIP_STACK_HEAP_RES_PTR_ERR;   // not one of our pointers!!!
        OSAL_SEM_Post(&hDcpt->_heapSemaphore);
        return 0;
    }
#endif
    
    freedUnits = ptr->units;
    
    fail = 0;
    
    if(!hDcpt->_heapHead)
    {
        ptr->next=0;
        hDcpt->_heapHead = hDcpt->_heapTail = ptr;
    }
    else if(ptr < hDcpt->_heapHead)
    {   // put it in front
        if(ptr+ptr->units == hDcpt->_heapHead)
        {   // compact
            ptr->units += hDcpt->_heapHead->units;
            ptr->next = hDcpt->_heapHead->next;
            if(hDcpt->_heapTail == hDcpt->_heapHead)
            {
                hDcpt->_heapTail = ptr;
            }
        }
        else
        {
            ptr->next = hDcpt->_heapHead;
        }
        hDcpt->_heapHead = ptr;    // new head
    }
    else if(ptr > hDcpt->_heapTail)
    {   // append tail
        if(hDcpt->_heapTail + hDcpt->_heapTail->units == ptr)
        {   // compact
            hDcpt->_heapTail->units += ptr->units;
        }
        else
        {
            hDcpt->_heapTail->next = ptr;
            ptr->next = 0;
            hDcpt->_heapTail = ptr;
        }
    }
    else
    {   // somewhere in the middle
        fail = 1;
        for(hdr = hDcpt->_heapHead; hdr != 0; hdr = hdr->next)
        {
            if(hdr<ptr && ptr<hdr->next)
            {   // found a place
                if(ptr+ptr->units == hdr->next)
                {   // compact right
                    ptr->units += hdr->next->units;
                    ptr->next = hdr->next->next;
                    if(hDcpt->_heapTail == hdr->next)
                    {
                        hDcpt->_heapTail = ptr;
                    }
                }
                else
                {
                #ifdef TCPIP_STACK_DRAM_DEBUG_ENABLE
                    if(ptr+ptr->units > hdr->next)
                    {
                        break;  // corrupted pointer!!!
                    }
                #endif
                    ptr->next = hdr->next;
                }
                
                // compact left
                if(hdr+hdr->units == ptr)
                {
                    hdr->units += ptr->units;
                    hdr->next = ptr->next;
                    if(hDcpt->_heapTail == ptr)
                    {
                        hDcpt->_heapTail = hdr;
                    }
                }
                else
                {
                #ifdef TCPIP_STACK_DRAM_DEBUG_ENABLE
                    if(hdr+hdr->units > ptr)
                    {
                        break;      // corrupted pointer!!!
                    }
                #endif
                    hdr->next = ptr;
                }

                fail = 0;   // everything OK
                break;                
            }
        }
        
    }

    if(fail)
    {
        hDcpt->_lastHeapErr = TCPIP_STACK_HEAP_RES_PTR_ERR;   // corrupted pointer!!!
        OSAL_SEM_Post(&hDcpt->_heapSemaphore);
        return 0;
    }
    
    hDcpt->_heapAllocatedUnits -= freedUnits;
    OSAL_SEM_Post(&hDcpt->_heapSemaphore);
    return freedUnits * sizeof(_headNode);
}


static size_t _TCPIP_HEAP_Size(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_DCPT*      hDcpt;

    hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt)
    {
        return hDcpt->_heapUnits * sizeof(_headNode);   
    }

    return 0;
}

static size_t _TCPIP_HEAP_FreeSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_DCPT*      hDcpt;

    hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);


    if(hDcpt)
    {
        return (hDcpt->_heapUnits - hDcpt->_heapAllocatedUnits) * sizeof(_headNode);   
    }
    return 0;
}

static size_t _TCPIP_HEAP_HighWatermark(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_DCPT*      hDcpt;

    hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt)
    {
        return hDcpt->_heapWatermark * sizeof(_headNode);
    }
    return 0;
}

static size_t _TCPIP_HEAP_MaxSize(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_DCPT   *hDcpt;
    _headNode	*ptr;
    size_t      max_nunits;

    max_nunits = 0;

    hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);
    if(hDcpt)
    {
        OSAL_SEM_Pend(&hDcpt->_heapSemaphore, OSAL_WAIT_FOREVER);

        for(ptr = hDcpt->_heapHead; ptr != 0; ptr = ptr->next)
        {
            if(ptr->units >= max_nunits)
            {   // found block
                max_nunits = ptr->units;
            }
        }
        OSAL_SEM_Post(&hDcpt->_heapSemaphore);
    }

    return max_nunits * sizeof(_headNode);   

}


static TCPIP_STACK_HEAP_RES _TCPIP_HEAP_LastError(TCPIP_STACK_HEAP_HANDLE heapH)
{
    TCPIP_HEAP_DCPT*      hDcpt;
    TCPIP_STACK_HEAP_RES  res;

    hDcpt = _TCPIP_HEAP_ObjDcpt(heapH);

    if(hDcpt)
    {
        res = hDcpt->_lastHeapErr;
        hDcpt->_lastHeapErr = TCPIP_STACK_HEAP_RES_OK;
        return res;
    }

    return TCPIP_STACK_HEAP_RES_NO_HEAP;

}

#if defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 
static size_t _TCPIP_HEAP_AllocSize(TCPIP_STACK_HEAP_HANDLE heapH, const void* ptr)
{
    if(ptr)
    {
        _headNode* hPtr = (_headNode*)ptr -1;
        return hPtr->units * sizeof(_headNode);
    }

    return 0;
}
#endif  // defined(TCPIP_STACK_DRAM_DEBUG_ENABLE) 


