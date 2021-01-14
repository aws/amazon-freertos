/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated
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
 *  ======== UART2.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef DebugP_ASSERT_ENABLED
#define DebugP_ASSERT_ENABLED 0
#endif

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/UART2.h>
#include <ti/drivers/uart2/UART2Support.h>

extern const UART2_Config UART2_config[];
extern const uint_least8_t UART2_count;

/* Default UART parameters structure */
const UART2_Params UART2_defaultParams = {
    UART2_Mode_BLOCKING,          /* readMode */
    UART2_Mode_BLOCKING,          /* writeMode */
    NULL,                         /* readCallback */
    NULL,                         /* writeCallback */
    NULL,                         /* eventCallback */
    0,                            /* eventMask */
    UART2_ReadReturnMode_PARTIAL, /* readReturnMode */
    115200,                       /* baudRate */
    UART2_DataLen_8,              /* dataLength */
    UART2_StopBits_1,             /* stopBits */
    UART2_Parity_NONE,            /* parityType */
    NULL                          /* userArg */
};

/*
 *  ======== UART2_getRxCount ========
 */
size_t __attribute__((weak)) UART2_getRxCount(UART2_Handle handle)
{
    UART2_Object *object = handle->object;
    uintptr_t     key;
    int           count = 0;

    key = HwiP_disable();
    if (object->state.opened) {
        UART2Support_dmaRefreshRx(handle);
        count = RingBuf_getCount(&object->rxBuffer);
    }
    HwiP_restore(key);

    return (count);
}

/*
 *  ======== UART2_Params_init ========
 */
void UART2_Params_init(UART2_Params *params)
{
    *params = UART2_defaultParams;
}

/*
 *  ======== UART2_read ========
 */
int_fast16_t __attribute__((weak)) UART2_read(UART2_Handle handle, void *buffer,
        size_t size, size_t *bytesRead)
{
    UART2_Object        *object = handle->object;
    int                  status = UART2_STATUS_SUCCESS;

    if ((object->state.readMode == UART2_Mode_BLOCKING) &&
            (object->state.readReturnMode == UART2_ReadReturnMode_FULL)) {
        status = UART2_readFull(handle, buffer, size, bytesRead);
    }
    else {
        status = UART2_readTimeout(handle, buffer, size, bytesRead,
                UART2_WAIT_FOREVER);
    }

    return (status);
}

/*
 *  ======== UART2_readFull ========
 */
int_fast16_t __attribute__((weak)) UART2_readFull(UART2_Handle handle,
        void *buffer, size_t size, size_t *bytesRead)
{
    unsigned char       *buf = (unsigned char *)buffer;
    int                  status = UART2_STATUS_SUCCESS;
    size_t               totalRead = 0;
    size_t               nRead;

    while (size > 0) {
        status = UART2_readTimeout(handle, buf + totalRead, size - totalRead,
                &nRead, UART2_WAIT_FOREVER);

        totalRead += nRead;
        size -= nRead;

        if (status != UART2_STATUS_SUCCESS) {
            break;
        }
    }

    if (bytesRead) {
        *bytesRead = totalRead;
    }

    return (status);
}

/*
 *  ======== UART2_readTimeout ========
 */
int_fast16_t __attribute__((weak)) UART2_readTimeout(UART2_Handle handle,
        void *buffer, size_t size, size_t *bytesRead, uint32_t timeout)
{
    UART2_Object        *object = handle->object;
    int                  status = UART2_STATUS_SUCCESS;
    unsigned char       *buf = (unsigned char *)buffer;
    uintptr_t            key;
    size_t               nBytesRead;
    size_t              *pNBytesRead;
    int                  available;
    size_t               count = 0;
    unsigned char       *srcAddr;

    pNBytesRead = (bytesRead == NULL) ? &nBytesRead : bytesRead;
    *pNBytesRead = 0;

    key = HwiP_disable();

    if (!object->state.opened) {
        /* device instance not yet opened */
        HwiP_restore(key);
        return (UART2_STATUS_ENOTOPEN);
    }

    if (object->readInUse) {
        /* Another read is ongoing */
        HwiP_restore(key);
        return (UART2_STATUS_EINUSE);
    }

    object->readInUse = true;

    HwiP_restore(key);

    /* Save the data to be read and restore interrupts. */
    object->readBuf = (unsigned char *)buffer;
    object->readSize = size;
    object->readCount = size; /* Number remaining to be read */
    object->bytesRead = 0;    /* Number of bytes read */
    object->rxStatus = 0;     /* Clear receive errors */
    object->state.rxCancelled = false;
    object->state.readTimedOut = false;

    UART2_rxEnable(handle);

    // TODO: case for !HwiP_interruptsEnabled()?

    if ((object->state.readMode == UART2_Mode_BLOCKING) &&
            (timeout != UART2_WAIT_FOREVER) && (timeout != 0)) {
        ClockP_setTimeout(&(object->readTimeoutClk), timeout);
        ClockP_start(&(object->readTimeoutClk));
    }

    key = HwiP_disable();

    while (size > 0) {
        if (object->state.rxCancelled) {
            object->state.rxCancelled = false;
            status = UART2_STATUS_ECANCELLED;
            break;
        }

        if (object->state.readTimedOut) {
            status = UART2_STATUS_ETIMEOUT;
            break;
        }

        /* Update RX ring buffer state with latest number of bytes dma'd */
        UART2Support_dmaRefreshRx(handle);

        /* Flush semaphore before re-enabling HWI */
        SemaphoreP_pend(&object->readSem, SemaphoreP_NO_WAIT);
        HwiP_restore(key);

        /* Read as much data as is available from ring buffer */
        do {
            available = RingBuf_getPointer(&object->rxBuffer, &srcAddr);

            /*
             *  This logic does not need a HWI lock because it is already
             *  mutually exclusive above by allowing only one active read()
             *  call before reporting to the caller that the driver is busy.
             *  The reason we want to perform the copy without HWI locked is
             *  that the size of the copy is not necessarily deterministic,
             *  and we don't want preemption disabled for an indeterminate
             *  amount of time.
             */
            if (available > size) {
                available = size;
            }
            memcpy(buf, srcAddr, available);

            RingBuf_getConsume(&object->rxBuffer, available);

            size -= available;
            object->readCount -= available;
            object->bytesRead += available;
            buf += available;
            count += available;
        } while ((available > 0) && (size > 0));

        key = HwiP_disable();

        /* Non-blocking mode returns */
        if (object->state.readMode == UART2_Mode_NONBLOCKING) {
            if (count == 0) {
                status = UART2_STATUS_EAGAIN;
            }
            break;
        }

        if (count > 0) {
            /*
             *  Because we pulled some data out, we made space in the RX
             *  buffer.  Check if the RX DMA is stopped, and restart if
             *  appropriate.
             */
            UART2Support_dmaStartRx(handle);
            if (object->state.readMode != UART2_Mode_CALLBACK) {
                if ((object->state.readReturnMode != UART2_ReadReturnMode_FULL)
                        || (size == 0)) {

                    /* got 'some' data, break out of 'while (size > 0)' */
                    break;
                }
            }
            else {
                /* Callback mode */
                if ((object->state.readReturnMode != UART2_ReadReturnMode_FULL)
                        || (size == 0)) {

                    object->readCount = 0;
                    object->readInUse = false;

                    HwiP_restore(key);

                    /*
                     *  Safely call the callback function in case UART2_read()
                     *  is called from within the callback, to avoid recursion.
                     */
                    object->state.readCallbackPending = true;

                    /* If we're not inside the read callback function... */
                    if (object->state.inReadCallback == false) {
                        while (object->state.readCallbackPending) {
                            object->state.readCallbackPending = false;
                            object->state.inReadCallback = true;

                            object->readCallback(handle,
                                    (void *)object->readBuf,
                                    object->bytesRead, object->userArg,
                                    UART2_STATUS_SUCCESS);

                            object->state.inReadCallback = false;
                        }
                    }

                    key = HwiP_disable();
                }
                /* Return immediately for callback mode */
                break;
            } /* callback mode */
        } /* if (count > 0) */
        HwiP_restore(key);

        if (object->state.readMode != UART2_Mode_CALLBACK) {
            /* Wait for more data to be transferred to the ring buffer */
            SemaphoreP_pend(&object->readSem, SemaphoreP_WAIT_FOREVER);
        }
        else {
            /* Callback mode, all available bytes read */
            key = HwiP_disable();
            break;
        }

        key = HwiP_disable();
    }  /* while (size > 0) */

    /* Don't set readInUse for callback mode in case not done */
    if (object->state.readMode != UART2_Mode_CALLBACK) {
        object->readInUse = false;
        ClockP_stop(&(object->readTimeoutClk));
    }

    /* If readInUse is true, readCount must not be 0 */
    DebugP_assert(!(object->readInUse && (object->readCount == 0)));

    HwiP_restore(key);

    *pNBytesRead = count;

    return (status);
}

/*
 *  ======== UART2_readCancel ========
 */
void __attribute__((weak)) UART2_readCancel(UART2_Handle handle)
{
    UART2_Object *object = handle->object;
    uintptr_t     key;

    if ((object->state.readMode != UART2_Mode_BLOCKING) &&
            (object->state.readMode != UART2_Mode_CALLBACK)) {
        return;
    }

    key = HwiP_disable();

    if (object->readInUse == false) {
        HwiP_restore(key);
        return;
    }

    object->readInUse = false;
    object->readCount = 0;

    if (object->state.rxCancelled == false) {
        object->state.rxCancelled = true;
        SemaphoreP_post(&object->readSem);

        if (object->state.readMode == UART2_Mode_CALLBACK) {
            object->readCallback(handle, object->readBuf, object->bytesRead,
                    object->userArg, UART2_STATUS_ECANCELLED);
        }
    }

    HwiP_restore(key);
}

/*
 *  ======== UART2_rxDisable ========
 */
void __attribute__((weak)) UART2_rxDisable(UART2_Handle handle)
{
    UART2_Object        *object = handle->object;
    UART2_HWAttrs const *hwAttrs = handle->hwAttrs;

    uintptr_t key = HwiP_disable();
    if (object->state.rxEnabled) {
        UART2Support_dmaStopRx(handle);
        UART2Support_disableRx(hwAttrs);

        UART2Support_powerRelConstraint();

        object->state.rxEnabled = false;
    }
    HwiP_restore(key);
}

/*
 *  ======== UART2_rxEnable ========
 */
void __attribute__((weak)) UART2_rxEnable(UART2_Handle handle)
{
    UART2_Object        *object = handle->object;

    uintptr_t key = HwiP_disable();
    if (!object->state.rxEnabled) {
        object->state.rxEnabled = true;

        /* enable registered event callbacks/interrupts and receive */
        UART2Support_enableInts(handle);
        UART2Support_enableRx(handle->hwAttrs);

        UART2Support_dmaStartRx(handle);

        /* Set constraints to guarantee transaction */
        UART2Support_powerSetConstraint();
    }
    HwiP_restore(key);
}

/*
 *  ======== UART2_write ========
 */
int_fast16_t __attribute__((weak)) UART2_write(UART2_Handle handle,
        const void *buffer, size_t size, size_t *bytesWritten)
{
    return (UART2_writeTimeout(handle, buffer, size, bytesWritten,
            UART2_WAIT_FOREVER));
}

/*
 *  ======== UART2_writeTimeout ========
 */
int_fast16_t __attribute__((weak)) UART2_writeTimeout(UART2_Handle handle,
        const void *buffer, size_t size, size_t *bytesWritten,
        uint32_t timeout)
{
    UART2_Object        *object = handle->object;
    UART2_HWAttrs const *hwAttrs = handle->hwAttrs;
    int                  status = UART2_STATUS_SUCCESS;
    uintptr_t            key;
    uint32_t             writeCount;
    unsigned char       *buf = (unsigned char *)buffer;
    size_t               nBytesWritten;
    size_t              *pNBytesWritten;
    unsigned char       *dstAddr;
    int                  space;
    size_t               count = 0;

    if (size == 0) {
        return (UART2_STATUS_EINVALID);
    }

    pNBytesWritten = (bytesWritten == NULL) ? &nBytesWritten : bytesWritten;

    key = HwiP_disable();

    if (!object->state.opened) {
        /* device instance not yet opened */
        HwiP_restore(key);
        return (UART2_STATUS_ENOTOPEN);
    }

    if (object->writeInUse) {
        /* an active write call is already in progress */
        HwiP_restore(key);
        return (UART2_STATUS_EINUSE);
    }

    object->writeInUse = true;

    /* Save the data to be written and restore interrupts. */
    object->writeBuf = buffer;
    object->writeSize = size;
    object->writeCount = size;
    object->bytesWritten = 0;
    object->state.writeTimedOut = false;

    object->txStatus = UART2_STATUS_SUCCESS;

    /* Enable TX - interrupts must be disabled */
    UART2Support_enableTx(hwAttrs);

    HwiP_restore(key);

    if (!HwiP_interruptsEnabled()) {
        /* RTOS not started yet, called from main(), use polling mode */
        if (object->eventMask & UART2_EVENT_TX_BEGIN) {
            object->eventCallback(handle, UART2_EVENT_TX_BEGIN, 0,
                    object->userArg);
        }
        nBytesWritten = 0;
        while (size) {
            writeCount = UART2Support_sendData(hwAttrs, size,
                    buf + nBytesWritten);
            nBytesWritten += writeCount;
            size -= writeCount;
        }
        /* Wait for data to be fully shifted out of TX shift register. This is
         * required so that the device does not transition to low power mode
         * prematurely upon starting of the RTOS.
         */
        while (!UART2Support_txDone(hwAttrs)) {
            ;
        }
        UART2Support_disableTx(hwAttrs);
        if (object->eventMask & UART2_EVENT_TX_FINISHED) {
            object->eventCallback(handle, UART2_EVENT_TX_FINISHED, 0,
                    object->userArg);
        }

        return (status);
    }

    if ((object->state.writeMode == UART2_Mode_BLOCKING) &&
            (timeout != UART2_WAIT_FOREVER) && (timeout != 0)) {
        ClockP_setTimeout(&(object->writeTimeoutClk), timeout);
        ClockP_start(&(object->writeTimeoutClk));
    }

    key = HwiP_disable();

    while (size) {
        if (object->state.txCancelled) {
            object->state.txCancelled = false;
            status = UART2_STATUS_ECANCELLED;
            break;
        }
        if (object->state.writeTimedOut) {
            status = UART2_STATUS_ETIMEOUT;
            break;
        }

        /* Flush semaphore before re-enabling HWI */
        SemaphoreP_pend(&object->writeSem, SemaphoreP_NO_WAIT);
        HwiP_restore(key);

        /* Copy as much data as we can to the ring buffer */
        do {
            /*
             *  Get the number of bytes we can copy to the ring buffer
             *  and the location where we can start the copy into the
             *  ring buffer.
             */
            space = RingBuf_putPointer(&object->txBuffer, &dstAddr);
            if (space > size) {
                space = size;
            }
            memcpy(dstAddr, buf, space);

            /* Update the ring buffer state with the number of bytes copied */
            RingBuf_putAdvance(&object->txBuffer, space);

            size -= space;
            buf += space;
            count += space;
        } while ((space > 0) && (size > 0));

        key = HwiP_disable();
        if (count > 0) {
            /* Try and start the dma, it may already be started. */
            UART2Support_enableTx(hwAttrs);
            UART2Support_dmaStartTx(handle);
        }
        if (object->state.writeMode == UART2_Mode_NONBLOCKING) {
            if (count == 0) {
                /* There is currently no space in the ring buffer */
                status = UART2_STATUS_EAGAIN;
            }
            break;
        }

        if (object->state.writeMode == UART2_Mode_CALLBACK) {
            object->bytesWritten = count;
            object->writeCount = size;  /* Number of bytes left to write */
            if (size == 0) {
                /*
                 *  All data written to ring buffer, call callback function.
                 *  Since the write callback function may call UART2_write(),
                 *  take care to prevent recursion causing a stack overflow.
                 */
                object->writeInUse = false;
                object->writeCount = 0;
                HwiP_restore(key);

                object->state.writeCallbackPending = true;
                /* If we're not inside the write callback function... */
                if (object->state.inWriteCallback == false) {
                    while (object->state.writeCallbackPending) {
                        object->state.writeCallbackPending = false;
                        object->state.inWriteCallback = true;

                        object->writeCallback(handle, (void *)buffer,
                                count, object->userArg,
                                UART2_STATUS_SUCCESS);

                        object->state.inWriteCallback = false;
                    }
                }
                key = HwiP_disable();
            }
            break;
        }

        if (size == 0) {
            /* All data has been written to the TX buffer */
            break;
        }

        HwiP_restore(key);

        /* Wait for space to be available in ring buffer */
        SemaphoreP_pend(&object->writeSem, SemaphoreP_WAIT_FOREVER);
        key = HwiP_disable();
    } // while (size)

    /* Don't set writeInUse to false for callback mode if not done. */
    if (object->state.writeMode != UART2_Mode_CALLBACK) {
        object->writeInUse = false;
        ClockP_stop(&(object->writeTimeoutClk));
    }

    DebugP_assert(!(object->writeInUse && (object->writeCount == 0)));

    HwiP_restore(key);

    *pNBytesWritten = count;

    return (status);
}

/*
 *  ======== UART2_writeCancel ========
 */
void __attribute__((weak)) UART2_writeCancel(UART2_Handle handle)
{
    UART2_Object        *object = handle->object;
    uintptr_t            key;

    key = HwiP_disable();

    if (!object->state.txCancelled) {
        object->state.txCancelled = true;
        SemaphoreP_post(&object->writeSem);

        if (object->state.writeMode == UART2_Mode_CALLBACK) {
            object->writeInUse = false;
            object->writeCount = 0;
            object->writeCallback(handle, (void *)object->writeBuf,
                    object->bytesWritten, object->userArg,
                    UART2_STATUS_ECANCELLED);
        }
    }

    HwiP_restore(key);
}
