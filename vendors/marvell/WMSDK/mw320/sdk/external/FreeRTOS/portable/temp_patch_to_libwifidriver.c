#include "FreeRTOS.h"
#include "task.h"

/* ATTENTION!!!:
   This file is a patch for the libwifidriver.a to make it compatiable with the latest kernel release.
   The libwifidriver.a should be updated as soon as possiable after the kernel is released.
   Once the libwifidriver.a is re-compiled and updated, then this file can be deleted.
*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )

#pragma push_macro("ulTaskNotifyTake")
#undef ulTaskNotifyTake
    uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait )
    {
        ulTaskGenericNotifyTake( ( tskDEFAULT_INDEX_TO_NOTIFY ), ( xClearCountOnExit ), ( xTicksToWait ) );
    }
#pragma pop_macro("ulTaskNotifyTake")

#pragma push_macro("vTaskNotifyGiveFromISR")
#undef vTaskNotifyGiveFromISR
    uint32_t vTaskNotifyGiveFromISR( TaskHandle_t xTaskToNotify, BaseType_t *pxHigherPriorityTaskWoken )
    {
        vTaskGenericNotifyGiveFromISR( ( xTaskToNotify ), ( tskDEFAULT_INDEX_TO_NOTIFY ), ( pxHigherPriorityTaskWoken ) );
    }
#pragma pop_macro("vTaskNotifyGiveFromISR")

#endif