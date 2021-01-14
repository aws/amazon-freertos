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
 *  ======== I2CCC32XX.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*
 * By default disable asserts for this module.
 * This must be done before DebugP.h is included.
 */
#ifndef DebugP_ASSERT_ENABLED
#define DebugP_ASSERT_ENABLED 0
#endif

#include <ti/drivers/i2c/I2CCC32XX.h>
#include <ti/drivers/i2c/I2CSupport.h>

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC32XX.h>

#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/inc/hw_common_reg.h>
#include <ti/devices/cc32xx/inc/hw_ocp_shared.h>
#include <ti/devices/cc32xx/driverlib/rom.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>
#include <ti/devices/cc32xx/driverlib/i2c.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>
#include <ti/devices/cc32xx/inc/hw_i2c.h>
#include <ti/devices/cc32xx/driverlib/pin.h>

/* Pad configuration defines */
#define PAD_CONFIG_BASE (OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_0)
#define PAD_DEFAULT_STATE 0xC60 /* pad reset, plus set GPIO mode to free I2C */

#define I2CCC32XX_ERROR_INTS (I2C_MASTER_INT_NACK | \
    I2C_MASTER_INT_ARB_LOST | I2C_MASTER_INT_TIMEOUT)

#define I2CCC32XX_TRANSFER_INTS (I2C_MASTER_INT_STOP | \
    I2CCC32XX_ERROR_INTS)

#define I2CCC32XX_FIFO_SIZE 8
#define I2CCC32XX_MAX_BURST 255

/* Map MCS read bits to write bits */
#define I2C_MCS_START (I2C_MCS_ERROR)
#define I2C_MCS_STOP (I2C_MCS_ADRACK)
#define I2C_MCS_BURST (I2C_MCS_BUSBSY)
#define I2C_MCS_DATACK (I2C_MCS_ACK)

/* Prototypes */
static void I2CCC32XX_blockingCallback(I2C_Handle handle, I2C_Transaction *msg,
    bool transferStatus);
static void I2CCC32XX_completeTransfer(I2C_Handle handle);
void I2CCC32XX_init(I2C_Handle handle);
static void I2CCC32XX_initHw(I2C_Handle handle);
static int I2CCC32XX_postNotify(unsigned int eventType, uintptr_t eventArg,
    uintptr_t clientArg);
static void I2CCC32XX_primeReadBurst(I2CCC32XX_Object *object,
    I2CCC32XX_HWAttrsV1 const *hwAttrs);
static void I2CCC32XX_primeWriteBurst(I2CCC32XX_Object *object,
    I2CCC32XX_HWAttrsV1 const *hwAttrs);
static void I2CCC32XX_readRecieveFifo(I2CCC32XX_Object *object,
    I2CCC32XX_HWAttrsV1 const *hwAttrs);
void I2CCC32XX_reset();
static void I2CCC32XX_updateReg(uint32_t reg, uint32_t mask, uint32_t val);

/* Default I2C parameters structure */
static const I2C_Params I2C_defaultParams = {
    I2C_MODE_BLOCKING,  /* transferMode */
    NULL,               /* transferCallbackFxn */
    I2C_100kHz,         /* bitRate */
    NULL                /* custom */
};

/*
 *  ======== I2CCC32XX_blockingCallback ========
 */
static void I2CCC32XX_blockingCallback(I2C_Handle handle,
    I2C_Transaction *msg, bool transferStatus)
{
    I2CCC32XX_Object  *object = handle->object;

    /* Indicate transfer complete */
    SemaphoreP_post(&(object->transferComplete));
}

/*
 *  ======== I2CCC32XX_fillTransmitFifo ========
 */
static inline void I2CCC32XX_fillTransmitFifo(I2CCC32XX_Object *object,
    I2CCC32XX_HWAttrsV1 const *hwAttrs)
{
    while(object->writeCount && object->burstCount &&
        I2CFIFODataPutNonBlocking(hwAttrs->baseAddr, *(object->writeBuf))) {
        object->writeBuf++;
        object->writeCount--;
        object->burstCount--;
    }

    I2CMasterIntClearEx(hwAttrs->baseAddr, I2C_MASTER_INT_TX_FIFO_EMPTY);
}

/*
 *  ======== I2CCC32XX_initHw ========
 */
static void I2CCC32XX_initHw(I2C_Handle handle)
{
    ClockP_FreqHz              freq;
    I2CCC32XX_Object          *object = handle->object;
    I2CCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;
    uint32_t                   ulRegVal;

    /*
     *  Take I2C hardware semaphore to prevent NWP from accessing I2C.
     *
     *  This is done in initHw() instead of postNotify(), so the
     *  hardware sempahore is also taken during open().
     */
    ulRegVal = HWREG(COMMON_REG_BASE + COMMON_REG_O_I2C_Properties_Register);
    ulRegVal = (ulRegVal & ~0x3) | 0x1;
    HWREG(0x400F7000) = ulRegVal;

    /* Disable all interrupts */
    MAP_I2CMasterIntDisableEx(hwAttrs->baseAddr, 0xFFFFFFFF);

    /* Get CPU Frequency */
    ClockP_getCpuFreq(&freq);

    /* I2CMasterEnable() invoked in this call */
    MAP_I2CMasterInitExpClk(hwAttrs->baseAddr, freq.lo,
        object->bitRate > I2C_100kHz);

    MAP_I2CMasterTimeoutSet(hwAttrs->baseAddr, hwAttrs->sclTimeout);

    /* Flush the FIFOs. They must be empty before re-assignment */
    MAP_I2CTxFIFOFlush(hwAttrs->baseAddr);
    MAP_I2CRxFIFOFlush(hwAttrs->baseAddr);

    /* Set TX and RX FIFOs to master mode */
    MAP_I2CTxFIFOConfigSet(hwAttrs->baseAddr, I2C_FIFO_CFG_TX_MASTER);
    MAP_I2CRxFIFOConfigSet(hwAttrs->baseAddr, I2C_FIFO_CFG_RX_MASTER);

    /* Clear any pending interrupts */
    MAP_I2CMasterIntClearEx(hwAttrs->baseAddr, 0xFFFFFFFF);
}

/*
 *  ======== I2CCC32XX_postNotify ========
 *  This functions is called to notify the I2C driver of an ongoing transition
 *  out of LPDS mode.
 *  clientArg should be pointing to a hardware module which has already
 *  been opened.
 */
static int I2CCC32XX_postNotify(unsigned int eventType,
    uintptr_t eventArg, uintptr_t clientArg)
{
    /* Reconfigure the hardware when returning from LPDS */
    I2CCC32XX_initHw((I2C_Handle) clientArg);

    return (Power_NOTIFYDONE);
}


/*
 *  ======== I2CCC32XX_completeTransfer ========
 */
static void I2CCC32XX_completeTransfer(I2C_Handle handle)
{
    I2CCC32XX_Object          *object = handle->object;
    I2CCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    /* Disable and clear all interrupts */
    I2CMasterIntDisableEx(hwAttrs->baseAddr, 0xFFFFFFFF);
    I2CMasterIntClearEx(hwAttrs->baseAddr, 0xFFFFFFFF);

    /*
     *  If this current transaction was canceled, we need to cancel
     *  any queued transactions
     */
    if (object->currentTransaction->status == I2C_STATUS_CANCEL) {
        /*
         * Allow callback to run. If in CALLBACK mode, the application
         * may initiate a transfer in the callback which will call
         * primeTransfer().
         */
        object->transferCallbackFxn(handle, object->currentTransaction, false);
        object->currentTransaction->status = I2C_STATUS_CANCEL;

        Power_releaseConstraint(PowerCC32XX_DISALLOW_LPDS);

        /* also dequeue and call the transfer callbacks for any queued transfers */
        while (object->headPtr != object->tailPtr) {
            object->headPtr = object->headPtr->nextPtr;
            object->headPtr->status = I2C_STATUS_CANCEL;
            object->transferCallbackFxn(handle, object->headPtr, false);
            object->headPtr->status = I2C_STATUS_CANCEL;
        }

        /* clean up object */
        object->currentTransaction = NULL;
        object->headPtr = NULL;
        object->tailPtr = NULL;
    }
    /* See if we need to process any other transactions */
    else if (object->headPtr == object->tailPtr) {
        /* No other transactions need to occur */
        object->headPtr = NULL;
        object->tailPtr = NULL;

        /*
         * Allow callback to run. If in CALLBACK mode, the application
         * may initiate a transfer in the callback which will call
         * primeTransfer().
         */
        object->transferCallbackFxn(handle, object->currentTransaction,
            (object->currentTransaction->status == I2C_STATUS_SUCCESS));

        /* Release constraint since transaction is done */
        Power_releaseConstraint(PowerCC32XX_DISALLOW_LPDS);
    }
    else {
        /* Another transfer needs to take place */
        object->headPtr = object->headPtr->nextPtr;

        /*
         * Allow application callback to run. The application may
         * initiate a transfer in the callback which will add an
         * additional transfer to the queue.
         */
        object->transferCallbackFxn(handle, object->currentTransaction,
            (object->currentTransaction->status == I2C_STATUS_SUCCESS));

        I2CSupport_primeTransfer(handle, object->headPtr);
    }
}

/*
 *  ======== I2C_control ========
 *  @brief  Function for setting control parameters of the I2C driver
 *          after it has been opened.
 *
 *  @pre    Function assumes that the handle is not NULL
 *
 *  @note   Currently not in use.
 */
int_fast16_t I2C_control(I2C_Handle handle, uint_fast16_t cmd, void *arg)
{
    /* No implementation yet */
    return (I2C_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== I2CCC32XX_hwiFxn ========
 *  Hwi interrupt handler to service the I2C peripheral
 *
 *  The handler is a generic handler for a I2C object.
 */
static void I2CCC32XX_hwiFxn(uintptr_t arg)
{
    /* Get the pointer to the object and hwAttrs */
    I2CCC32XX_Object          *object = ((I2C_Handle)arg)->object;
    I2CCC32XX_HWAttrsV1 const *hwAttrs = ((I2C_Handle)arg)->hwAttrs;
    uint32_t                   intStatus;

    /* Read and clear masked interrupts */
    intStatus = I2CMasterIntStatusEx(hwAttrs->baseAddr, true);
    I2CMasterIntClearEx(hwAttrs->baseAddr, intStatus);

    if (intStatus & I2CCC32XX_ERROR_INTS) {

        /* The MSR contains all error bits */
        uint32_t status = HWREG(hwAttrs->baseAddr + I2C_O_MCS);

        /* Decode interrupt status */
        if (status & I2C_MCS_ARBLST) {
            object->currentTransaction->status = I2C_STATUS_ARB_LOST;
        }
        else if (status & I2C_MCS_DATACK) {
            object->currentTransaction->status = I2C_STATUS_DATA_NACK;
        }
        else if (status & I2C_MCS_ADRACK) {
            object->currentTransaction->status = I2C_STATUS_ADDR_NACK;
        }
        else if (status & I2C_MCS_CLKTO) {
            object->currentTransaction->status = I2C_STATUS_CLOCK_TIMEOUT;
        }
        else {
            object->currentTransaction->status = I2C_STATUS_ERROR;
        }

        /* If arbitration is lost, another I2C master owns the bus */
        if (status & I2C_MCS_ARBLST) {
            I2CCC32XX_completeTransfer((I2C_Handle) arg);
            return;
        }

        /* If the stop interrupt has not occurred, force a STOP condition */
        if (!(intStatus & I2C_MASTER_INT_STOP)) {

            HWREG(hwAttrs->baseAddr + I2C_O_MCS) = I2C_MCS_STOP;
            return;
        }
    }
    else if (intStatus & I2C_MASTER_INT_TX_FIFO_EMPTY) {
        /* If there is more data to transmit */
        if (object->writeCount) {

            /* If we need to configure a new burst */
            if (0 == object->burstCount) {
                I2CCC32XX_primeWriteBurst(object, hwAttrs);
            }
            else {
                I2CCC32XX_fillTransmitFifo(object, hwAttrs);
            }
        }
        /* Finished writing, is there data to receive? */
        else if (object->readCount) {

            /* Disable TX interrupt */
            I2CMasterIntDisableEx(hwAttrs->baseAddr,
                I2C_MASTER_INT_TX_FIFO_EMPTY);

            object->burstStarted = false;
            I2CCC32XX_primeReadBurst(object, hwAttrs);
        }
    }
    else if (intStatus & I2C_MASTER_INT_RX_FIFO_REQ ||
       !(HWREG(hwAttrs->baseAddr + I2C_O_FIFOSTATUS) & I2C_FIFOSTATUS_RXFE)) {

        I2CCC32XX_readRecieveFifo(object, hwAttrs);

        /* If we need to configure a new burst */
        if (object->readCount && (0 == object->burstCount)) {
            I2CCC32XX_primeReadBurst(object, hwAttrs);
        }
    }

    /* If the STOP condition was set, complete the transfer */
    if (intStatus & I2C_MASTER_INT_STOP) {
        /* If the transaction completed, update the transaction status */
        if (object->currentTransaction->status == I2C_STATUS_INCOMPLETE &&
            (!(object->readCount || object->writeCount))) {

            uint32_t fifoStatus = I2CFIFOStatus(hwAttrs->baseAddr) &
                (I2C_FIFO_TX_EMPTY | I2C_FIFO_RX_EMPTY);

            /* If both FIFOs are empty */
            if (fifoStatus == (I2C_FIFO_RX_EMPTY | I2C_FIFO_TX_EMPTY)) {
                object->currentTransaction->status = I2C_STATUS_SUCCESS;
            }
        }

        I2CCC32XX_completeTransfer((I2C_Handle) arg);
    }
}

/*
 *  ======== I2CCC32XX_primeReadBurst =======
 */
static void I2CCC32XX_primeReadBurst(I2CCC32XX_Object *object,
    I2CCC32XX_HWAttrsV1 const *hwAttrs)
{
    uint32_t command;

    /* Specify the I2C slave address in receive mode */
    I2CMasterSlaveAddrSet(hwAttrs->baseAddr,
        object->currentTransaction->slaveAddress, true);

    /* Determine the size of this burst */
    if(object->readCount > I2CCC32XX_MAX_BURST) {
        object->burstCount = I2CCC32XX_MAX_BURST;
    }
    else {
        object->burstCount = object->readCount;
    }

    /*
     * If we are reading less than FIFO_SIZE bytes, set the RX FIFO
     * REQ interrupt to trigger when we finish burstCount bytes.
     * Else we are reading more than 8 bytes. set the RX FIFO REQ
     * interrupt to trigger when full. If we are reading less than RXTRIG
     * bytes during the final byte reads, the RXDONE interrupt will allow
     * us to complete the read transaction.
     */
    if(object->burstCount < I2CCC32XX_FIFO_SIZE) {
        I2CCC32XX_updateReg(hwAttrs->baseAddr + I2C_O_FIFOCTL,
            I2C_FIFOCTL_RXTRIG_M, object->burstCount << I2C_FIFOCTL_RXTRIG_S);
    }
    else {
        I2CCC32XX_updateReg(hwAttrs->baseAddr + I2C_O_FIFOCTL,
            I2C_FIFOCTL_RXTRIG_M, I2C_FIFOCTL_RXTRIG_M);
    }

    I2CMasterBurstLengthSet(hwAttrs->baseAddr, object->burstCount);

    /* If we will be sending multiple bursts */
    if(object->readCount > I2CCC32XX_MAX_BURST) {
         /* Setting the ACK enable ensures we ACK the last byte of a burst */
         command = (I2C_MCS_BURST | I2C_MCS_ACK);
    }
    else {
         command = (I2C_MCS_BURST | I2C_MCS_STOP);
    }

    /* Only generate a start condition if the burst hasn't started */
    if (!object->burstStarted) {
        command |= I2C_MCS_START;
        object->burstStarted = true;
    }

    I2CMasterIntEnableEx(hwAttrs->baseAddr, I2C_MASTER_INT_RX_FIFO_REQ |
        I2CCC32XX_TRANSFER_INTS);

    HWREG(hwAttrs->baseAddr + I2C_O_MCS) = command;
}

/*
 *  ======== I2CCC32XX_primeWriteBurst =======
 */
static void I2CCC32XX_primeWriteBurst(I2CCC32XX_Object  *object,
    I2CCC32XX_HWAttrsV1 const *hwAttrs)
{
    uint32_t command;

    /* Write I2C Slave Address and transmit direction */
    I2CMasterSlaveAddrSet(hwAttrs->baseAddr,
        object->currentTransaction->slaveAddress, false);

    /* Determine the size of this burst */
    if(object->writeCount > I2CCC32XX_MAX_BURST) {
        object->burstCount = I2CCC32XX_MAX_BURST;
    }
    else {
        object->burstCount = object->writeCount;
    }

    /* Write burst length */
    I2CMasterBurstLengthSet(hwAttrs->baseAddr,
        object->burstCount);

    /* If we will be sending multiple bursts */
    if (object->readCount || object->writeCount > I2CCC32XX_MAX_BURST) {
        command = I2C_MCS_BURST;
    }
    else {
        command = I2C_MCS_BURST | I2C_MCS_STOP;
    }

    /* Only generate a start condition if the burst hasn't started */
    if (!object->burstStarted) {
        command |= I2C_MCS_START;
        object->burstStarted = true;
    }

    /* Fill transmit FIFO. This will modify the object counts */
    I2CCC32XX_fillTransmitFifo(object, hwAttrs);

    /* Enable TXFIFOEMPTY interrupt and other standard transfer interrupts */
    I2CMasterIntEnableEx(hwAttrs->baseAddr, I2C_MASTER_INT_TX_FIFO_EMPTY
        | I2CCC32XX_TRANSFER_INTS);

    /* Write the Master Control & Status register */
    HWREG(hwAttrs->baseAddr + I2C_O_MCS) = command;
}

/*
 *  ======== I2CSupport_primeTransfer =======
 */
int_fast16_t I2CSupport_primeTransfer(I2C_Handle handle,
    I2C_Transaction *transaction)
{
    I2CCC32XX_Object          *object = handle->object;
    I2CCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;
    int_fast16_t               status = I2C_STATUS_SUCCESS;

    /* Store the new internal counters and pointers */
    object->currentTransaction = transaction;

    object->writeBuf = transaction->writeBuf;
    object->writeCount = transaction->writeCount;

    object->readBuf = transaction->readBuf;
    object->readCount = transaction->readCount;

    object->burstCount = 0;
    object->burstStarted = false;

    /*
     * Transaction is incomplete unless the stop condition occurs AND
     * all bytes have been sent and received. This condition is checked
     * in the hardware interrupt when the STOP condition occurs.
     */
    transaction->status = I2C_STATUS_INCOMPLETE;

    /* Flush the FIFOs */
    I2CTxFIFOFlush(hwAttrs->baseAddr);
    I2CRxFIFOFlush(hwAttrs->baseAddr);

    /* Determine if the bus is in use by another I2C Master */
    if (I2CMasterBusBusy(hwAttrs->baseAddr))
    {
        transaction->status = I2C_STATUS_BUS_BUSY;
        status = I2C_STATUS_BUS_BUSY;
    }
    /* Start transfer in Transmit mode */
    else if (object->writeCount) {
        I2CCC32XX_primeWriteBurst(object, hwAttrs);
    }
    /* Start transfer in Receive mode */
    else {
        I2CCC32XX_primeReadBurst(object, hwAttrs);
    }

    return (status);
}

/*
 *  ======== I2CSupport_masterFinish ========
 */
void I2CSupport_masterFinish(I2C_HWAttrs const *hwAttrs)
{
    /*
     * Writing the STOP command may put the I2C peripheral's FSM in a
     * bad state, so we write RUN | STOP such that the current burst
     * is transferred or received.
     */
    MAP_I2CMasterControl(hwAttrs->baseAddr, I2C_MASTER_CMD_BURST_SEND_FINISH);
}

/*
 *  ======== I2CSupport_powerRelConstraint =======
 */
void I2CSupport_powerRelConstraint(void)
{
    /* Release disallow constraint. */
    Power_releaseConstraint(PowerCC32XX_DISALLOW_LPDS);
}

/*
 *  ======== I2CSupport_powerSetConstraint =======
 */
void I2CSupport_powerSetConstraint(void)
{
    /* Set disallow constraint. */
    Power_setConstraint(PowerCC32XX_DISALLOW_LPDS);
}

/*
 *  ======== I2CCC32XX_readRecieveFifo ========
 */
static void I2CCC32XX_readRecieveFifo(I2CCC32XX_Object *object,
    I2CCC32XX_HWAttrsV1 const *hwAttrs)
{
    while(object->readCount && object->burstCount &&
        I2CFIFODataGetNonBlocking(hwAttrs->baseAddr, object->readBuf)) {
        object->readBuf++;
        object->readCount--;
        object->burstCount--;
    }

    I2CMasterIntClearEx(hwAttrs->baseAddr, I2C_MASTER_INT_RX_FIFO_REQ);
}

/*
 *  ======== I2CCC32XX_updateReg ========
 */
static void I2CCC32XX_updateReg(uint32_t reg, uint32_t mask, uint32_t val)
{
    uint32_t regL = HWREG(reg) & ~mask;
    HWREG(reg) = (regL | (val & mask));
}

/*
 *  ======== I2C_close ========
 */
void I2C_close(I2C_Handle handle)
{
    I2CCC32XX_Object          *object = handle->object;
    I2CCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;
    uint32_t                   padRegister;

    /* Check to see if a I2C transaction is in progress */
    DebugP_assert(object->headPtr == NULL);

    /* Mask I2C interrupts */
    MAP_I2CMasterIntDisableEx(hwAttrs->baseAddr, I2CCC32XX_TRANSFER_INTS);

    /* Disable the I2C Master */
    MAP_I2CMasterDisable(hwAttrs->baseAddr);

    /* Disable I2C module clocks */
    Power_releaseDependency(PowerCC32XX_PERIPH_I2CA0);

    Power_unregisterNotify(&(object->notifyObj));

    /* Restore pin pads to their reset states */
    padRegister = (PinToPadGet((hwAttrs->clkPin) & 0xff)<<2) + PAD_CONFIG_BASE;
    HWREG(padRegister) = PAD_DEFAULT_STATE;
    padRegister = (PinToPadGet((hwAttrs->dataPin) & 0xff)<<2) + PAD_CONFIG_BASE;
    HWREG(padRegister) = PAD_DEFAULT_STATE;

    /* Destruct modules used in driver */
    HwiP_destruct(&(object->hwi));
    SemaphoreP_destruct(&(object->mutex));
    if (object->transferMode == I2C_MODE_BLOCKING) {
        SemaphoreP_destruct(&(object->transferComplete));
    }

    object->isOpen = false;

    return;
}

/*
 *  ======== I2C_open ========
 */
I2C_Handle I2C_open(uint_least8_t index, I2C_Params *params)
{
    I2C_Handle handle = NULL;
    uintptr_t                  key;
    I2CCC32XX_Object          *object;
    I2CCC32XX_HWAttrsV1 const *hwAttrs;
    HwiP_Params                hwiParams;
    uint16_t                   pin;
    uint16_t                   mode;

    if (index < I2C_count) {
        if (params == NULL) {
            params = (I2C_Params *) &I2C_defaultParams;
        }

        handle = (I2C_Handle)&(I2C_config[index]);
        hwAttrs = handle->hwAttrs;
        object = handle->object;
    }
    else {
        return (NULL);
    }

    /* Check for valid bit rate */
    if (params->bitRate > I2C_400kHz) {
        return (NULL);
    }

    /* Determine if the device index was already opened */
    key = HwiP_disable();
    if(object->isOpen == true){
        HwiP_restore(key);
        return (NULL);
    }

    /* Mark the handle as being used */
    object->isOpen = true;
    HwiP_restore(key);

    /* Save parameters */
    object->transferMode = params->transferMode;
    object->transferCallbackFxn = params->transferCallbackFxn;
    object->bitRate = params->bitRate;

    /* Enable the I2C module clocks */
    Power_setDependency(PowerCC32XX_PERIPH_I2CA0);

    /* In case of app restart: disable I2C module, clear interrupt at NVIC */
    MAP_I2CMasterDisable(hwAttrs->baseAddr);
    HwiP_clearInterrupt(hwAttrs->intNum);

    pin = hwAttrs->clkPin & 0xff;
    mode = (hwAttrs->clkPin >> 8) & 0xff;
    MAP_PinTypeI2C((unsigned long)pin, (unsigned long)mode);

    pin = hwAttrs->dataPin & 0xff;
    mode = (hwAttrs->dataPin >> 8) & 0xff;
    MAP_PinTypeI2C((unsigned long)pin, (unsigned long)mode);

    Power_registerNotify(&(object->notifyObj), PowerCC32XX_AWAKE_LPDS,
        I2CCC32XX_postNotify, (uint32_t)handle);

    HwiP_Params_init(&hwiParams);
    hwiParams.arg = (uintptr_t)handle;
    hwiParams.priority = hwAttrs->intPriority;
    HwiP_construct(&(object->hwi), hwAttrs->intNum, I2CCC32XX_hwiFxn,
        &hwiParams);

    /*
     * Create threadsafe handles for this I2C peripheral
     * Semaphore to provide exclusive access to the I2C peripheral
     */
    SemaphoreP_constructBinary(&(object->mutex), 1);

    /*
     * Store a callback function that posts the transfer complete
     * semaphore for synchronous mode
     */
    if (object->transferMode == I2C_MODE_BLOCKING) {
        /* Semaphore to cause the waiting task to block for the I2C to finish */
        SemaphoreP_constructBinary(&(object->transferComplete), 0);

        /* Store internal callback function */
        object->transferCallbackFxn = I2CCC32XX_blockingCallback;
    }
    else {
        /* Check to see if a callback function was defined for async mode */
        DebugP_assert(object->transferCallbackFxn != NULL);
    }

    /* Clear the head pointer */
    object->headPtr = NULL;
    object->tailPtr = NULL;
    object->currentTransaction = NULL;

    /* Set the I2C configuration */
    I2CCC32XX_initHw(handle);

    /* Return the address of the handle */
    return (handle);
}

/*
 *  ======== I2CC32XX_reset ========
 */
void I2CCC32XX_reset()
{
    PRCMPeripheralReset(PRCM_I2CA0);
    while(!PRCMPeripheralStatusGet(PRCM_I2CA0)) {}
}

/*
 *  ======== I2C_setClockTimeout ========
 */
int_fast16_t I2C_setClockTimeout(I2C_Handle handle, uint32_t timeout)
{
    I2CCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;
    int_fast16_t status = I2C_STATUS_ERROR;

    if (timeout > 0x1 && timeout <= 0xFF) {
        MAP_I2CMasterTimeoutSet(hwAttrs->baseAddr, timeout);
        status = I2C_STATUS_SUCCESS;
    }

    return (status);
}
