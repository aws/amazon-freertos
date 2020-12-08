#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOSConfig.h"

#define WICED_THREAD_PRIORITY_HIGHEST  (configMAX_PRIORITIES-1)
#define WICED_THREAD_PRIORITY_LOWEST   (0)
#define WICED_THREAD_PRIORITY_DEFAULT  (WICED_THREAD_PRIORITY_HIGHEST - 4)
#define WICED_THREAD_PRIORITY_WORKER   (WICED_THREAD_PRIORITY_HIGHEST - 3)
/* for backward compatibility */
#define WICED_NETWORK_WORKER_PRIORITY  (WICED_THREAD_PRIORITY_WORKER)

#if defined INCLUDE_xTaskIsTaskFinished && (INCLUDE_xTaskIsTaskFinished == 1)
signed portBASE_TYPE xTaskIsTaskFinished( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;
#endif

#ifdef __cplusplus
} /*extern "C" */
#endif
