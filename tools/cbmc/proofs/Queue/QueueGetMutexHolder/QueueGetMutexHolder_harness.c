#include "FreeRTOS.h"
#include "queue_init.h"
#include "queue.h"

#include "cbmc.h"

void harness() {
  QueueHandle_t xSemaphore =
      xUnconstrainedQueue();
  if (xSemaphore) {
    xSemaphore->uxQueueType = nondet();
    xQueueGetMutexHolder(xSemaphore);
  }
}
