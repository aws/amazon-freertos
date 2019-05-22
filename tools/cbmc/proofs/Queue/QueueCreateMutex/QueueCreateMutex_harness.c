#include "FreeRTOS.h"
#include "queue.h"

#include "cbmc.h"

void harness() {
  uint8_t ucQueueType;

  xQueueCreateMutex(ucQueueType);
}
