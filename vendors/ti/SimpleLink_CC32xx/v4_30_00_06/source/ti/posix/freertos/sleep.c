/*
 * Copyright (c) 2016-2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== sleep.c ========
 */

#include <errno.h>
#include <time.h>
#include <unistd.h>

#include <FreeRTOS.h>
#include <task.h>

#if defined( __IAR_SYSTEMS_ICC__ )
    #include "../iar/errno.h"
#endif

/* number of microseconds per tick */
#define TICK_PERIOD_USECS (1000000L / configTICK_RATE_HZ)

/*  The maximum number of ticks before the tick count rolls over. We use
 *  0xFFFFFFFF instead of 0x100000000 to avoid 64-bit math.
 */
#define MAX_TICKS 0xFFFFFFFFL
#define TICKS_PER_SEC (1000000L / TICK_PERIOD_USECS)

/* integral number of seconds in a period of MAX_TICKS */
#define MAX_SECONDS (MAX_TICKS / TICKS_PER_SEC)


/*
 *  ======== nanosleep ========
 */
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
    TickType_t ticks;

    /* max interval to avoid tick count overflow */
    if (rqtp->tv_sec >= MAX_SECONDS) {
        errno = EINVAL;
        return (-1);
    }
    if ((rqtp->tv_nsec < 0) || (rqtp->tv_nsec >= 1000000000)) {
        errno = EINVAL;
        return (-1);
    }
    if ((rqtp->tv_sec == 0) && (rqtp->tv_nsec == 0)) {
        return (0);
    }

    ticks = rqtp->tv_sec * (1000000 / TICK_PERIOD_USECS);

    /* compute ceiling value */
    ticks += (rqtp->tv_nsec + TICK_PERIOD_USECS * 1000 - 1) /
            (TICK_PERIOD_USECS * 1000);

    /*  Add one tick to ensure the timeout is not less than the
     *  amount of time requested. The clock may be about to tick,
     *  and that counts as one tick even though the amount of time
     *  until this tick is less than a full tick period.
     */
    ticks++;

    /* suspend for the requested time interval */
    vTaskDelay(ticks);

    /*  If the rmtp argument is non-NULL, the structure referenced
     *  by it should contain the amount of time remaining in the
     *  interval. Signals are not supported, therefore, this will
     *  always be zero. Caution: the rqtp and rmtp arguments may
     *  point to the same object.
     */
    if (rmtp != NULL) {
        rmtp->tv_sec = 0;
        rmtp->tv_nsec = 0;
    }

    return (0);
}

/*
 *  ======== sleep ========
 */
unsigned sleep(unsigned seconds)
{
    TickType_t xDelay;
    unsigned long secs;         /* at least 32-bit */
    unsigned max_secs, rval;    /* native size, might be 16-bit */

    max_secs = MAX_SECONDS;

    if (seconds < max_secs) {
        secs = seconds;
        rval = 0;
    }
    else {
        secs = max_secs;
        rval = seconds - max_secs;
    }

    /* compute requested ticks */
    xDelay = secs * configTICK_RATE_HZ;

    /* must add one tick to ensure a full duration of requested ticks */
    vTaskDelay(xDelay + 1);

    return (rval);
}

/*
 *  ======== usleep ========
 */
int usleep(useconds_t usec)
{
    TickType_t xDelay;

    /* usec must be less than 1000000 */
    if (usec >= 1000000) {
        errno = EINVAL;
        return (-1);
    }

    /* take the ceiling */
    xDelay = (usec + TICK_PERIOD_USECS - 1) / TICK_PERIOD_USECS;

    /* must add one tick to ensure a full duration of xDelay ticks */
    vTaskDelay(xDelay + 1);

    return (0);
}
