#ifndef SRC_IF_H
#define SRC_IF_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
/*
Basic, somewhat quick and dirty, interface for an audio source. The main code uses this to grab samples 
from whatever input source it instantiates. Here's how to use it:

- Create a src_cfg_t variable
- Set item_size to the amount of samples per queue read.
- Set queue to a queue with an item size of (item_size * sizeof(int16_t))
- Create a task for the worker function of the input method of your choice (e.g. wavsrcTask). Pass the
  src_cfg_t variable address as the argument.
- The worker task should now start filling the queue. Receive from it and do with the samples as 
  you please.

Note that at the moment all source interfaces are expected to return signed 16-bit samples at an 16KHz 
sample rate.
*/


typedef struct {
    QueueHandle_t *queue;
    int item_size; //in bytes
} src_cfg_t;

void recsrcTask(void *arg);
void app_speech_wakeup_init();
#endif
