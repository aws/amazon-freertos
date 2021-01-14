/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== ClockP_freertos.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <timers.h>

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/HwiP.h>

#define FREERTOS_TICKPERIOD_US (1000000 / configTICK_RATE_HZ)

static TickType_t ticksToWait = portMAX_DELAY;

void ClockP_callbackFxn(uintptr_t arg);

typedef struct ClockP_FreeRTOSObj {
    TimerHandle_t    timer;
    ClockP_Fxn       fxn;
    uintptr_t        arg;
    uint32_t         timeout;      /* Timeout set by API */
    uint32_t         curTimeout;   /* Either timeout or period */
    uint32_t         period;
} ClockP_FreeRTOSObj;

#if (configSUPPORT_STATIC_ALLOCATION == 1)
typedef struct ClockP_StaticFreeRTOSObj {
    ClockP_FreeRTOSObj clockObj;
    StaticTimer_t      staticTimer;
} ClockP_StaticFreeRTOSObj;
#endif

static bool setClockObjTimeout(ClockP_Handle pObj);

/*
 *  ======== ClockP_callbackFxn ========
 */
void ClockP_callbackFxn(uintptr_t arg)
{
    TimerHandle_t       handle = (TimerHandle_t)arg;
    ClockP_FreeRTOSObj *obj;

    obj = (ClockP_FreeRTOSObj *)pvTimerGetTimerID(handle);

    /*
     *  If the period is non-zero and different from the initial timeout
     *  passed to ClockP_create() we need to change the FreeRTOS timer's
     *  period to the ClockP object's period.
     */
    if ((obj->period != 0) && (obj->curTimeout != obj->period)) {
        obj->curTimeout = obj->period;

        /* Set the xTimer timeout to the new value */
        setClockObjTimeout(obj);
    }

    (obj->fxn)(obj->arg);
}

/*
 *  ======== ClockP_construct ========
 */
ClockP_Handle ClockP_construct(ClockP_Struct *clockP, ClockP_Fxn clockFxn,
     uint32_t timeout, ClockP_Params *params)
{
    ClockP_FreeRTOSObj *pObj = NULL;

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    ClockP_StaticFreeRTOSObj * statObj = (ClockP_StaticFreeRTOSObj *)clockP;
    StaticTimer_t *    staticTimer = (StaticTimer_t *)&statObj->staticTimer;
    TickType_t         initialTimeout = timeout;
    ClockP_Params      defaultParams;
    TimerHandle_t      tHandle;
    UBaseType_t        autoReload;

    pObj = (ClockP_FreeRTOSObj *)&statObj->clockObj;

    if (params == NULL) {
        params = &defaultParams;
        ClockP_Params_init(&defaultParams);
    }

    autoReload = (params->period == 0) ? 0 : 1;

    /*
     *  FreeRTOS does not allow you to create a timer with a timeout
     *  of 0.  If timeout is 0, assume that ClockP_setTimeout() will be
     *  called to change it, and create the timer with a non-zero timeout.
     */
    if (timeout == 0) {
        initialTimeout = (TickType_t)0xFFFFFFFF;
    }

    tHandle = xTimerCreateStatic(NULL, initialTimeout, autoReload,
            (void *)pObj, (TimerCallbackFunction_t)ClockP_callbackFxn,
            staticTimer);

    if (tHandle == NULL) {
        pObj = NULL;
    }
    else {
        pObj->timer = tHandle;
        pObj->fxn = clockFxn;
        pObj->arg = params->arg;
        pObj->timeout = timeout;
        pObj->period = params->period;

        if (params->startFlag) {
            /* Just returns if timeout is 0 */
            ClockP_start((ClockP_Handle)pObj);
        }
    }
#endif

    return ((ClockP_Handle)pObj);
}

/*
 *  ======== ClockP_create ========
 */
ClockP_Handle ClockP_create(ClockP_Fxn clockFxn, uint32_t timeout,
        ClockP_Params *params)
{
    ClockP_Params       defaultParams;
    ClockP_FreeRTOSObj *pObj;
    UBaseType_t         autoReload;
    TimerHandle_t       handle = NULL;
    TickType_t          initialTimeout = timeout;

    if (params == NULL) {
        params = &defaultParams;
        ClockP_Params_init(&defaultParams);
    }

    if ((pObj = pvPortMalloc(sizeof(ClockP_FreeRTOSObj))) == NULL) {
        return (NULL);
    }

    autoReload = (params->period == 0) ? 0 : 1;

    /*
     *  FreeRTOS does not allow you to create a timer with a timeout
     *  of 0.  If timeout is 0, assume that ClockP_setTimeout() will be
     *  called to change it, and create the timer with a non-zero timeout.
     */
    if (timeout == 0) {
        initialTimeout = (TickType_t)0xFFFFFFFF;
    }

    handle = xTimerCreate(NULL, initialTimeout, autoReload,
            (void *)pObj, (TimerCallbackFunction_t)ClockP_callbackFxn);

    if (handle == NULL) {
        vPortFree(pObj);
        return (NULL);
    }

    pObj->timer = handle;
    pObj->fxn = clockFxn;
    pObj->arg = params->arg;
    pObj->timeout = timeout;
    pObj->period = params->period;

    if (params->startFlag) {
        /* Just returns if timeout is 0 */
        ClockP_start((ClockP_Handle)pObj);
    }

    return ((ClockP_Handle)pObj);
}

/*
 *  ======== ClockP_delete ========
 */
void ClockP_delete(ClockP_Handle handle)
{
    ClockP_FreeRTOSObj *pObj = (ClockP_FreeRTOSObj *)handle;
    BaseType_t status;

    status = xTimerDelete((TimerHandle_t)pObj->timer, ticksToWait);

    configASSERT(status == pdPASS);

    if (status == pdPASS) {
        vPortFree(pObj);
    }
}

/*
 *  ======== ClockP_destruct ========
 */
void ClockP_destruct(ClockP_Struct * clockP)
{
}

/*
 *  ======== ClockP_getCpuFreq ========
 */
void ClockP_getCpuFreq(ClockP_FreqHz *freq)
{
    /* return configCPU_CLOCK_HZ, as defined in FreeRTOSConfig.h */
    freq->lo = (uint32_t) configCPU_CLOCK_HZ;
    freq->hi = 0;
}

/*
 *  ======== ClockP_getSystemTickPeriod ========
 */
uint32_t ClockP_getSystemTickPeriod()
{
    return (FREERTOS_TICKPERIOD_US);
}

/*
 *  ======== ClockP_getSystemTicks ========
 */
uint32_t ClockP_getSystemTicks()
{
    uint32_t ticks;

    if (!HwiP_inISR()) {
        ticks = (uint32_t)xTaskGetTickCount();
    }
    else {
        ticks = (uint32_t)xTaskGetTickCountFromISR();
    }

    return (ticks);
}

/*
 *  ======== ClockP_getTimeout ========
 */
uint32_t ClockP_getTimeout(ClockP_Handle handle)
{
    ClockP_FreeRTOSObj *pObj = (ClockP_FreeRTOSObj *)handle;
    TickType_t          remainingTime;

    /*
     *  Calculate the time that remains before the timer expires.
     *  TickType_t is an unsigned type, so the subtraction will result in
     *  the correct answer even if the timer will not expire until after
     *  the tick count has overflowed.
     */
    remainingTime = xTimerGetExpiryTime(pObj->timer) - xTaskGetTickCount();

    return ((uint32_t)remainingTime);
}

/*
 *  ======== ClockP_isActive ========
 */
bool ClockP_isActive(ClockP_Handle handle)
{
    ClockP_FreeRTOSObj *pObj = (ClockP_FreeRTOSObj *)handle;

    if (xTimerIsTimerActive(pObj->timer) != pdFALSE) {
        return (true);
    }

    return (false);
}

/*
 *  ======== ClockP_Params_init ========
 */
void ClockP_Params_init(ClockP_Params *params)
{
    params->arg = (uintptr_t)0;
    params->startFlag = false;
    params->period = 0;
}

/*
 *  ======== ClockP_setTimeout ========
 */
void ClockP_setTimeout(ClockP_Handle handle, uint32_t timeout)
{
    ClockP_FreeRTOSObj *pObj = (ClockP_FreeRTOSObj *)handle;

    /*
     *  We are not allowed to change the timeout once the ClockP objecct has
     *  been started.  The next call to ClockP_start() will set the FreeRTOS
     *  timer's timeout to the new value.
     */
    pObj->timeout = timeout;
}

/*
 *  ======== ClockP_start ========
 */
void ClockP_start(ClockP_Handle handle)
{
    ClockP_FreeRTOSObj *pObj = (ClockP_FreeRTOSObj *)handle;

    if (pObj->timeout == 0) {
        return;
    }

    /*
     *  Set the timeout value in case ClockP_setTimeout() was called.
     *  According to the FreeRTOS API documentation, xTimerChangePeriod()
     *  actually starts a dormant timer, so calling xTimerStart() is
     *  not necessary.
     */
    pObj->curTimeout = pObj->timeout;
    setClockObjTimeout(handle);
}

#if (configSUPPORT_STATIC_ALLOCATION == 1)
/*
 *  ======== ClockP_staticObjectSize ========
 */
size_t ClockP_staticObjectSize(void)
{
    return (sizeof(ClockP_StaticFreeRTOSObj));
}
#endif

/*
 *  ======== ClockP_stop ========
 */
void ClockP_stop(ClockP_Handle handle)
{
    ClockP_FreeRTOSObj *pObj = (ClockP_FreeRTOSObj *)handle;
    BaseType_t status;

    if (!HwiP_inISR()) {
        status = xTimerStop(pObj->timer, ticksToWait);
    }
    else {
        status = xTimerStopFromISR(pObj->timer, NULL);
    }
    configASSERT(status == pdPASS);
}

/*
 *  ======== ClockP_sleep ========
 */
void ClockP_sleep(uint32_t sec)
{
    TickType_t xDelay;

    if (sec > 0xFFFFFFFF / configTICK_RATE_HZ) {
        xDelay = 0xFFFFFFFF;
    }
    else {
        xDelay = sec * configTICK_RATE_HZ;
    }

    vTaskDelay(xDelay);
}

/*
 *  ======== ClockP_usleep ========
 */
void ClockP_usleep(uint32_t usec)
{
    TickType_t xDelay;

    /* Take the ceiling */
    xDelay = (usec + FREERTOS_TICKPERIOD_US - 1) / FREERTOS_TICKPERIOD_US;

    vTaskDelay(xDelay);
}

/*
 *  ======== setClockObjTimeout ========
 */
static bool setClockObjTimeout(ClockP_Handle handle)
{
    ClockP_FreeRTOSObj *pObj = (ClockP_FreeRTOSObj *)handle;
    BaseType_t          status;

    if (!HwiP_inISR()) {
        status = xTimerChangePeriod(pObj->timer, (TickType_t)pObj->curTimeout,
                ticksToWait);

        configASSERT(status == pdPASS);
    }
    else {
        status = xTimerChangePeriodFromISR(pObj->timer,
                (TickType_t)pObj->curTimeout, NULL);
    }

    if (status == pdPASS) {
        return (true);
    }

    return (false);
}
