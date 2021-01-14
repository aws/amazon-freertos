/*
 * Copyright (c) 2016-2020, Texas Instruments Incorporated
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

/* ----------------------------------------------------------------------------
 *  Includes
 * ----------------------------------------------------------------------------
 */

#include <ti/display/Display.h>
#include <ti/display/DisplayHost.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/SwiP.h>
#include <ti/drivers/dpl/SystemP.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

/* ----------------------------------------------------------------------------
 *  Constants and macros
 * ----------------------------------------------------------------------------
 */
#if !defined(__IAR_SYSTEMS_ICC__)
#define FWRITE(BUF, SIZE, HANDLE, STREAM) fwrite(BUF, SIZE, HANDLE, STREAM)
#else
#include <yfuns.h>
#define FWRITE(BUF, SIZE, HANDLE, STREAM) __write(HANDLE, (unsigned char*) BUF,SIZE)
#endif

/* ----------------------------------------------------------------------------
 *   Type definitions
 * ----------------------------------------------------------------------------
 */

/* ----------------------------------------------------------------------------
 *                           Local variables
 * ----------------------------------------------------------------------------
 */
/* Display function table for DisplayHost implementation */
const Display_FxnTable DisplayHost_fxnTable = {
    DisplayHost_init,
    DisplayHost_open,
    DisplayHost_clear,
    DisplayHost_clearLines,
    DisplayHost_vprintf,
    DisplayHost_close,
    DisplayHost_control,
    DisplayHost_getType,
};

/* Semaphore to synchronize writes to display buffer */
static SemaphoreP_Handle  writeSem;

/* ----------------------------------------------------------------------------
 *                                          Functions
 * ----------------------------------------------------------------------------
 */

/*
 *  ======== DisplayHost_clear ========
 */
void DisplayHost_clear(Display_Handle handle)
{
}

/*
 *  ======== DisplayHost_clearLines ========
 */
void DisplayHost_clearLines(Display_Handle handle, uint8_t fromLine,
                            uint8_t toLine)
{
}

/*
 *  ======== DisplayHost_close ========
 */
void DisplayHost_close(Display_Handle handle)
{
}

/*
 *  ======== DisplayHost_control ========
 */
int DisplayHost_control(Display_Handle handle, unsigned int cmd, void *arg)
{
    return (DISPLAY_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== DisplayHost_getType ========
 */
unsigned int DisplayHost_getType(void)
{
    return (Display_Type_HOST);
}

/*
 *  ======== DisplayHost_init ========
 */
void DisplayHost_init(Display_Handle handle)
{
    SemaphoreP_Handle sem;
    unsigned int      key;

    /* Speculatively create a binary semaphore for thread safety */
    sem = SemaphoreP_createBinary(1);

    key = HwiP_disable();

    if (writeSem == NULL) {
        /* use the binary sem created above */
        writeSem = sem;
        HwiP_restore(key);
    }
    else {
        /* open already called */
        HwiP_restore(key);
        /* delete unused Semaphore */
        if (sem) {
            SemaphoreP_delete(sem);
        }
    }
}

/*
 *  ======== DisplayHost_open ========
 */
Display_Handle DisplayHost_open(Display_Handle handle, Display_Params *params)
{
    if (writeSem == NULL) {
        DisplayHost_init(handle);
        if (writeSem == NULL) {
            return (NULL);
        }
    }

    return (handle);
}

/*
 *  ======== DisplayHost_vprintf ========
 */
void DisplayHost_vprintf(Display_Handle handle, uint8_t line,
                         uint8_t column, const char *fmt, va_list va)
{
    DisplayHost_HWAttrs *hwAttrs = (DisplayHost_HWAttrs *)handle->hwAttrs;
    int                  strSize;
    char                *buf = hwAttrs->strBuf;
    uint32_t             timeout = SemaphoreP_WAIT_FOREVER;

    /* Set timeout to 0 if we're in a Hwi, Swi, or main(). */
    if (HwiP_inISR() || SwiP_inISR() || (HwiP_interruptsEnabled() == false)) {
        timeout = 0;
    }

    /*
     *  Don't bother checking result of SemaphoreP_pend() for timeout 0.
     *  If the semaphore is not available, just go ahead and write.
     */
    SemaphoreP_pend(writeSem, timeout);

    SystemP_vsnprintf(buf, hwAttrs->strBufLen - 1, fmt, va);

    strSize = strlen(hwAttrs->strBuf);
    hwAttrs->strBuf[strSize++] = '\n';

    FWRITE(buf, strSize, 1, stdout);

    SemaphoreP_post(writeSem);
}
