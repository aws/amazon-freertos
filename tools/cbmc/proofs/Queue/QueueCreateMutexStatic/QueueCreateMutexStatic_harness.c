#include "FreeRTOS.h"
#include "queue.h"
#include "cbmc.h"


void harness(){
    uint8_t ucQueueType;

    //The mutex storage is assumed to be not null.
    StaticQueue_t xStaticQueue;

    xQueueCreateMutexStatic( ucQueueType, &xStaticQueue );
}
