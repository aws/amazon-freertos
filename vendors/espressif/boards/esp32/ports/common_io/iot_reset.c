/*
 * Copyright 2020 Espressif Systems (Shanghai) Co., Ltd.
 * All rights reserved.
 *
 * You may not use this file except in compliance with the terms and
 * conditions set forth in the accompanying LICENSE.TXT file.
 *
 * THESE MATERIALS ARE PROVIDED ON AN "AS IS" BASIS. ESPRESSIF/AMAZON SPECIFICALLY
 * DISCLAIMS, WITH RESPECT TO THESE MATERIALS, ALL WARRANTIES, EXPRESS,
 * IMPLIED, OR STATUTORY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
 */

#include <iot_reset.h>
#include <esp_system.h>
#include <rom/rtc.h>

void iot_reset_reboot(IotResetBootFlag_t coldBootFlag)
{
    (void) coldBootFlag;
    esp_restart();
}

int32_t iot_reset_shutdown()
{
    return IOT_RESET_FUNCTION_NOT_SUPPORTED;
}

int32_t iot_get_reset_reason(IotResetReason_t *xResetReason)
{
    if (xResetReason != NULL) {
        RESET_REASON res = rtc_get_reset_reason(0);
        if (res == POWERON_RESET) {
            *xResetReason = eResetPowerOnBoot;
        } else if (res == SW_CPU_RESET) {
            *xResetReason = eResetWarmBoot;
        } else if (res == TG0WDT_SYS_RESET || res == TG1WDT_SYS_RESET || res == RTCWDT_SYS_RESET || res == RTCWDT_CPU_RESET || res == RTCWDT_RTC_RESET) {
            *xResetReason = eResetWatchdog;
        } else if (res == RTCWDT_BROWN_OUT_RESET) {
            *xResetReason = eResetBrownOut;
        } else {
            *xResetReason = eResetOther;
        }
    }
    return IOT_RESET_SUCCESS;
}
