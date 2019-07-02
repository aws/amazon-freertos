#include "FreeRTOS.h"
#include "queue.h"
#include "queue_datastructure.h"

#include "cbmc.h"

void harness() {
  uint8_t ucQueueType;
  QueueHandle_t xMutex =
      xQueueCreateMutex( ucQueueType);
  if (xMutex) {
    xMutex->uxQueueType = ucQueueType;
    UBaseType_t uxCounter;
    /* This assumption is explained in the queue.c file inside the method body
       xQueueGiveMutexRecursive and guards against an underflow error. */
    __CPROVER_assume(uxCounter > 0);
    xMutex->u.xSemaphore.uxRecursiveCallCount = uxCounter;
    xQueueGiveMutexRecursive(xMutex);
  }
}
