#include "FreeRTOS.h"
#include "queue.h"
#include "cbmc.h"


void harness(){
    UBaseType_t uxMaxCount;
    UBaseType_t uxInitialCount;

    //xStaticQueue is required to be not null
    StaticQueue_t xStaticQueue;

    //Checked invariant
    __CPROVER_assume(uxMaxCount != 0);
    __CPROVER_assume(uxInitialCount <= uxMaxCount);
    xQueueCreateCountingSemaphoreStatic( uxMaxCount, uxInitialCount, &xStaticQueue );
}
