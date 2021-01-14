/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQueueNTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== PowerCC26X2_freertos.c ========
 */

#include <stdbool.h>
#include <ti/drivers/ITM.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>
#include <ti/drivers/dpl/ClockP.h>

#include <FreeRTOS.h>
#include <task.h>
#include <portmacro.h>

/* driverlib header files */
#include <ti/devices/cc13x2_cc26x2/driverlib/prcm.h>
#include <ti/devices/cc13x2_cc26x2/driverlib/cpu.h>
#include <ti/devices/cc13x2_cc26x2/driverlib/sys_ctrl.h>
#include <ti/devices/cc13x2_cc26x2/driverlib/vims.h>
#include <ti/devices/cc13x2_cc26x2/driverlib/systick.h>

#include "ClockPCC26X2.h"

#define FREERTOS_TICKPERIOD_US (1000000 / configTICK_RATE_HZ)

extern PowerCC26X2_ModuleState PowerCC26X2_module;

static volatile uint32_t PowerCC26X2_idleTimeOS = 0;

/*
 *  ======== PowerCC26XX_standbyPolicy ========
 */
void PowerCC26XX_standbyPolicy(void)
{
#if (configUSE_TICKLESS_IDLE != 0)
    eSleepModeStatus eSleep;
    uint32_t ticksBefore;
    uint32_t ticksAfter;
    uint32_t sleptTicks;
    uint32_t constraints;
    uint32_t soonest;
    uint32_t timeout;
    uint32_t ticksOS;
    uint32_t timeOS;
    uint32_t ticks;
    uint32_t time;

    /* disable interrupts */
    CPUcpsid();

    /* final check with FreeRTOS to make sure still OK to go to sleep... */
    eSleep = eTaskConfirmSleepModeStatus();
    if (eSleep == eAbortSleep ) {
        CPUcpsie();
        return;
    }

    /* check operating conditions, optimally choose DCDC versus GLDO */
    SysCtrl_DCDC_VoltageConditionalControl();

    /* query the declared constraints */
    constraints = Power_getConstraintMask();

    /* do quick check to see if only WFI allowed; if yes, do it now */
    if ((constraints & (1 << PowerCC26XX_DISALLOW_STANDBY)) ==
        (1 << PowerCC26XX_DISALLOW_STANDBY)) {

        /* Flush any remaining log messages in the ITM */
        ITM_flush();
        PRCMSleep();
        /* Restore ITM settings */
        ITM_restore();
    }

    /*
     *  check if any sleep modes are allowed for automatic activation
     */
    else {
        /* check if allowed to go to standby */
        if ((constraints & (1 << PowerCC26XX_DISALLOW_STANDBY)) == 0) {

            /* check how many ClockP ticks until next scheduled wakeup */
            ticks = ClockP_getTicksUntilInterrupt();
            time = ticks * ClockP_tickPeriod;          /* convert to usec */

            /* check soonest wake time pending for FreeRTOS */
            ticksOS = PowerCC26X2_idleTimeOS;
            timeOS = ticksOS * FREERTOS_TICKPERIOD_US; /* convert to usec */

            /* get soonest wake time and corresponding ClockP timeout */
            if (time < timeOS) {
                soonest = time;   /* soonest is ClockP timeout */
                timeout = ticks;
            }
            else {
                soonest = timeOS; /* soonest is OS timeout */
                timeout = timeOS / ClockP_tickPeriod; /* calc ClockP ticks */
            }

            /* check soonest time vs STANDBY latency */
            if (soonest > Power_getTransitionLatency(PowerCC26XX_STANDBY,
                Power_TOTAL)) {

                /* stop FreeRTOS ticks */
                SysTickDisable();

                /* schedule the early wakeup event */
                timeout -= PowerCC26X2_WAKEDELAYSTANDBY / ClockP_tickPeriod;
                ClockP_setTimeout(ClockP_handle((ClockP_Struct *)
                    &PowerCC26X2_module.clockObj), timeout);
                ClockP_start(ClockP_handle((ClockP_Struct *)
                    &PowerCC26X2_module.clockObj));

                /* save ClockP tick count before sleep */
                ticksBefore = ClockP_getSystemTicks();

                /* Flush any remaining log messages in the ITM */
                ITM_flush();

                /* go to standby mode */
                Power_sleep(PowerCC26XX_STANDBY);

                /* Restore ITM settings */
                ITM_restore();

                ClockP_stop(ClockP_handle((ClockP_Struct *)
                    &PowerCC26X2_module.clockObj));

                /* get ClockP tick count after sleep */
                ticksAfter = ClockP_getSystemTicks();

                /* calculate elapsed FreeRTOS tick periods in STANDBY */
                sleptTicks = (ticksAfter - ticksBefore) * ClockP_tickPeriod;
                sleptTicks /= FREERTOS_TICKPERIOD_US;

                /* update FreeRTOS tick count for time spent in STANDBY */
                vTaskStepTick(sleptTicks);

                /* restart FreeRTOS ticks */
                SysTickEnable();
            }
        }
    }

    /* re-enable interrupts */
    CPUcpsie();
#endif
}

/*
 *  ======== PowerCC26XX_schedulerDisable ========
 */
void PowerCC26XX_schedulerDisable()
{
    /* TODO: don't need this if keep using vPortSuppressTicksAndSleep */
    vTaskSuspendAll();
}

/*
 *  ======== PowerCC26XX_schedulerRestore ========
 */
void PowerCC26XX_schedulerRestore()
{
    /* TODO: don't need this if keep using vPortSuppressTicksAndSleep */
    xTaskResumeAll();
}

/*
 *  ======== vPortSuppressTicksAndSleep ========
 *  FreeRTOS hook function invoked when Idle when configUSE_TICKLESS_IDLE
 *
 *  TODO: see if can use vApplicationIdleHook instead so don't need minimum
 *  config of 2 ticks before this hook gets called. This requires mimicing
 *  what is done in static function prvGetExpectedIdleTime() to get expected
 *  idle time.
 */
void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    /* stash FreeRTOS' expected idle time */
    PowerCC26X2_idleTimeOS = xExpectedIdleTime;

    /*
     * call Power-driver-specified idle function, to conditionally invoke the
     * Power policy
     */
    Power_idleFunc();
}
