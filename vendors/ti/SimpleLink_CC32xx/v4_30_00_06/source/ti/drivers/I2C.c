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
 *  ======== I2C.c ========
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CSupport.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

/* Default I2C parameters structure */
const I2C_Params I2C_defaultParams = {
    I2C_MODE_BLOCKING,  /* transferMode */
    NULL,               /* transferCallbackFxn */
    I2C_100kHz,         /* bitRate */
    NULL                /* custom */
};

/*
 *  ======== I2C_cancel ========
 */
void I2C_cancel(I2C_Handle handle)
{
    I2C_Object        *object = handle->object;
    I2C_HWAttrs const *hwAttrs = handle->hwAttrs;
    uintptr_t          key;

    key = HwiP_disable();

    /* Return if no transfer in progress */
    if (!object->headPtr) {
        HwiP_restore(key);
        return;
    }

    I2CSupport_masterFinish(hwAttrs);

    /* Set current transaction as canceled */
    object->currentTransaction->status = I2C_STATUS_CANCEL;

    HwiP_restore(key);
}

/*
 *  ======== I2C_init ========
 */
void I2C_init(void)
{
    /* Do nothing */
}

/*
 *  ======== I2C_Params_init =======
 */
void I2C_Params_init(I2C_Params *params)
{
    *params = I2C_defaultParams;
}

/*
 *  ======== I2C_transfer ========
 */
bool I2C_transfer(I2C_Handle handle, I2C_Transaction *transaction)
{
    int_fast16_t result = (I2C_transferTimeout(handle, transaction, I2C_WAIT_FOREVER));

    if (result == I2C_STATUS_SUCCESS) {
        return (true);
    }
    else {
        return (false);
    }
}

/*
 *  ======== I2C_transferTimeout ========
 */
int_fast16_t I2C_transferTimeout(I2C_Handle handle, I2C_Transaction *transaction, uint32_t timeout)
{
    I2C_Object        *object = handle->object;
    I2C_HWAttrs const *hwAttrs = handle->hwAttrs;
    uintptr_t          key;
    int_fast16_t       ret;

    /* Check if anything needs to be written or read */
    if ((!transaction->writeCount) && (!transaction->readCount)) {
        transaction->status = I2C_STATUS_INVALID_TRANS;
        /* Nothing to write or read */
        return (transaction->status);
    }

    key = HwiP_disable();

    /* If callback mode, determine if transfer in progress */
    if (object->transferMode == I2C_MODE_CALLBACK && object->headPtr) {
        /*
         * Queued transactions are being canceled. Can't allow additional
         * transactions to be queued.
         */
        if (object->headPtr->status == I2C_STATUS_CANCEL) {
            transaction->status = I2C_STATUS_INVALID_TRANS;
            ret = transaction->status;
        }
        /* Transfer in progress */
        else {
            /*
             * Update the message pointed by the tailPtr to point to the next
             * message in the queue
             */
            object->tailPtr->nextPtr = transaction;

            /* Update the tailPtr to point to the last message */
            object->tailPtr = transaction;

            /* Set queued status */
            transaction->status = I2C_STATUS_QUEUED;

            ret = I2C_STATUS_SUCCESS;
        }

        HwiP_restore(key);
        return (ret);
    }

    /* Store the headPtr indicating I2C is in use */
    object->headPtr = transaction;
    object->tailPtr = transaction;

    /* In blocking mode, transactions can queue on the I2C mutex */
    transaction->status = I2C_STATUS_QUEUED;

    HwiP_restore(key);

    /* Get the lock for this I2C handle */
    if (SemaphoreP_pend(&(object->mutex), SemaphoreP_NO_WAIT)
        == SemaphoreP_TIMEOUT) {

        /* We were unable to get the mutex in CALLBACK mode */
        if (object->transferMode == I2C_MODE_CALLBACK) {
            /*
             * Recursively call transfer() and attempt to place transaction
             * on the queue. This may only occur if a thread is preempted
             * after restoring interrupts and attempting to grab this mutex.
             */
            return (I2C_transferTimeout(handle, transaction, timeout));
        }

        /* Wait for the I2C lock. If it times-out before we retrieve it, then
         * return false to cancel the transaction. */
        if(SemaphoreP_pend(&(object->mutex), timeout) == SemaphoreP_TIMEOUT) {
            transaction->status = I2C_STATUS_TIMEOUT;
            return (I2C_STATUS_TIMEOUT);
        }
    }

    if (object->transferMode == I2C_MODE_BLOCKING) {
       /*
        * In the case of a timeout, It is possible for the timed-out transaction
        * to call the hwi function and post the object->transferComplete Semaphore
        * To clear this, we simply do a NO_WAIT pend on (binary)
        * object->transferComplete, so that it resets the Semaphore count.
        */
        SemaphoreP_pend(&(object->transferComplete), SemaphoreP_NO_WAIT);
    }

    I2CSupport_powerSetConstraint();

    /*
     * I2CSupport_primeTransfer is a longer process and
     * protection is needed from the I2C interrupt
     */
    HwiP_disableInterrupt(hwAttrs->intNum);
    ret = I2CSupport_primeTransfer(handle, transaction);
    HwiP_enableInterrupt(hwAttrs->intNum);

    if (ret != I2C_STATUS_SUCCESS) {
        /* Release disallow constraint. */
        I2CSupport_powerRelConstraint();
    }
    else if (object->transferMode == I2C_MODE_BLOCKING) {
        /* Wait for the primed transfer to complete */
        if (SemaphoreP_pend(&(object->transferComplete), timeout)
            == SemaphoreP_TIMEOUT) {

            key = HwiP_disable();

            /*
             * Protect against a race condition in which the transfer may
             * finish immediately after the timeout. If this occurs, we
             * will preemptively consume the semaphore on the next initiated
             * transfer.
             */
            if (object->headPtr) {
                /*
                 * It's okay to call cancel here since this is blocking mode.
                 * Cancel will generate a STOP condition and immediately
                 * return.
                 */
                I2C_cancel(handle);

                HwiP_restore(key);

                /*
                 *  We must wait until the STOP interrupt occurs. When this
                 *  occurs, the semaphore will be posted. Since the STOP
                 *  condition will finish the current burst, we can't
                 *  unblock the object->mutex until this occurs.
                 *
                 *  This may block forever if the slave holds the clock line--
                 *  rendering the I2C bus un-usable.
                 */
                SemaphoreP_pend(&(object->transferComplete),
                    SemaphoreP_WAIT_FOREVER);

                transaction->status = I2C_STATUS_TIMEOUT;
            }
            else {
                HwiP_restore(key);
            }
        }

        ret = transaction->status;
    }

    /* Release the lock for this particular I2C handle */
    SemaphoreP_post(&(object->mutex));

    /* Return status */
    return (ret);
}
