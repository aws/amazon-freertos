#include "FreeRTOS.h"
#include "queue.h"

#include "cbmc.h"


void harness(){
    UBaseType_t uxMaxCount;
    UBaseType_t uxInitialCount;

    __CPROVER_assume(uxMaxCount != 0);
    __CPROVER_assume(uxInitialCount <= uxMaxCount);

    xQueueCreateCountingSemaphore( uxMaxCount, uxInitialCount );
}

