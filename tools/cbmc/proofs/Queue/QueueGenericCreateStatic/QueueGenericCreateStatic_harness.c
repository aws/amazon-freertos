#include "FreeRTOS.h"
#include "queue.h"
#include "queue_datastructure.h"
#include "cbmc.h"


void harness(){
    UBaseType_t uxQueueLength;
    UBaseType_t uxItemSize;
    uint8_t *ucQueueStorage = (uint8_t *) pvPortMalloc(sizeof(uint8_t));

    //The queue storage space is explicitly assumed to be not null.
    StaticQueue_t xStaticQueue;
    uint8_t ucQueueType;

    /* The QueueGenericReset method called during static queue creation
       implicitly that there are no overflows in the computation
       and the inputs are safe. There is no check for this in the code base. */
    UBaseType_t upper_bound = portMAX_DELAY - sizeof(Queue_t);
    __CPROVER_assume(uxItemSize < (upper_bound)/uxQueueLength);
    __CPROVER_assume(uxQueueLength > ( UBaseType_t ) 0);
    if(!ucQueueStorage){
        uxItemSize = 0;
    }else{
        __CPROVER_assume(uxItemSize > 0);
    }

    xQueueGenericCreateStatic( uxQueueLength, uxItemSize, ucQueueStorage, &xStaticQueue, ucQueueType );
}

