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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef DebugP_ASSERT_ENABLED
#define DebugP_ASSERT_ENABLED 0
#endif
#ifndef DebugP_LOG_ENABLED
#define DebugP_LOG_ENABLED 0
#endif

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>

#include <ti/drivers/i2s/I2SCC32XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC32XX.h>
#include <ti/drivers/dma/UDMACC32XX.h>
#include <ti/drivers/utils/List.h>

#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/inc/hw_ocp_shared.h>
#include <ti/devices/cc32xx/inc/hw_ints.h>
#include <ti/devices/cc32xx/inc/hw_mcasp.h>
#include <ti/devices/cc32xx/inc/hw_apps_config.h>
#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/devices/cc32xx/driverlib/rom.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>
#include <ti/devices/cc32xx/driverlib/interrupt.h>
#include <ti/devices/cc32xx/driverlib/i2s.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>
#include <ti/devices/cc32xx/driverlib/udma.h>

#define PinConfigPinMode(config)       (((config) >> 8) & 0xF)
#define PinConfigPin(config)           (((config) >> 0) & 0x3F)
#define PAD_CONFIG_BASE (OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_0)
#define PAD_RESET_STATE 0xC61
#define I2S_NB_CHANNELS_MAX                       8U
#define BYTE_LENGTH                               8U /* Length in bits of a byte*/
#define UDMA_MAX_TRANSFERLENGTH                   1023 /* This is the maximum transfer length for uDMA in MODE BASIC. Transfers lengths must be inferior or equal to this value. */
#define HWRESOLUTION                              4U /* All the settings are expressed in half-bytes (4 bits) */
#define PRCMI2SCLOCKFREQMIN                       234377 /* Minimum Frequency (in Hz) possible with only one divider */
#define HCLKDIVMAX                                0xFFF  /* Max division of ulHClkDiv (ulHClkDiv is a 12-bit clock divider) */

#define DATA_SHIFT_NONE                           0x00 /* Value to use for RXFMT.RDATDLY and TXFMT.XDATDLY if no data shift is expected */
#define DATA_SHIFT_1                              0x01 /* Value to use for RXFMT.RDATDLY and TXFMT.XDATDLY if data are expected to shift by one - Value to use for true I2S format */
#define DATA_SHIFT_2                              0x02 /* Value to use for RXFMT.RDATDLY and TXFMT.XDATDLY if data are expected to shift by two */

#define MCASP_RXFMCTL_FRWID_DEFAULT               0x10 /* This value is the default value to use for RXFMCTL.FRWID */
#define MCASP_TXFMCTL_FXWID_DEFAULT               0x10 /* This value is the default value to use for TXFMCTL.FXWID */
#define MCASP_RXFMCTL_RMOD_DEFAULT                0x100 /* This value is the default value to use for RXFMCTL.RMOD */
#define MCASP_TXFMCTL_XMOD_DEFAULT                0x100 /* This value is the default value to use for TXFMCTL.XMOD */
#define MCASP_AHCLKRCTL_HCLKRM_DEFAULT            0x8000 /* This value is the default value to use for AHCLKRCTL.HCLKRM */
#define MCASP_AHCLKXCTL_HCLKXM_DEFAULT            0x8000 /* This value is the default value to use for AHCLKXCTL.HCLKXM */

/*
 * This lookup table is used to configure the DMA channels for the appropriate
 * (8bit, 16bit or 32bit) transfer sizes.
 * Table for an I2S DMA RX channel.
 */
static const uint32_t dmaRxConfig[] = {
    UDMA_SIZE_8  | UDMA_SRC_INC_NONE | UDMA_DST_INC_8,
    UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16,
    UDMA_SIZE_32 | UDMA_SRC_INC_NONE | UDMA_DST_INC_32,
};

/*
 * This lookup table is used to configure the DMA channels for the appropriate
 * (8bit, 16bit or 32bit) transfer sizes.
 * Table for an I2S DMA TX channel.
 */
static const uint32_t dmaTxConfig[] = {
    UDMA_SIZE_8  | UDMA_SRC_INC_8  | UDMA_DST_INC_NONE,
    UDMA_SIZE_16 | UDMA_SRC_INC_16 | UDMA_DST_INC_NONE,
    UDMA_SIZE_32 | UDMA_SRC_INC_32 | UDMA_DST_INC_NONE,
};

/*
 * This lookup table is used to configure the DMA channels for the appropriate
 * (8bit, 16bit or 32bit) transfer sizes.
 * Table works for an both RX and TX I2S DMA channels.
 */
#define UDMA_ARBLENGTH_DEFAULT 4
static const uint32_t dmaArbConfig [] = {
    UDMA_ARB_1,
    UDMA_ARB_2,
    UDMA_ARB_4,
    UDMA_ARB_8,
    UDMA_ARB_16,
    UDMA_ARB_32,
    UDMA_ARB_64,
    UDMA_ARB_128,
};

/* Forward declarations */
static bool initObject(I2S_Handle handle, I2S_Params *params);
static void initIO(I2S_Handle handle);
static void initDMA(I2S_Handle handle);
static void initHw(I2S_Handle handle);
static int postNotify(unsigned int eventType, uintptr_t eventArg, uintptr_t clientArg);

static void resetPin(unsigned long pinI2S);
static void openPin(unsigned long pinI2S);
static void restorePinpads(I2SCC32XX_HWAttrs const *hwAttrs);

static void configSCK(I2S_Handle handle);
static void configWS(I2S_Handle handle);
static void configSerialFormat(I2S_Handle handle);
static uint8_t getNumberOfChannels(const uint8_t channelsList);

static void I2S_hwiIntFxn(uintptr_t arg);
static void updateDataReadDMA(uintptr_t arg);
static void updateDataWriteDMA(uintptr_t arg);
static void updateDataReadCPU(uintptr_t arg);
static void updateDataWriteCPU(uintptr_t arg);
static void handleDMATransaction(I2S_Handle handle, I2S_Transaction *transaction, uint32_t transLength, I2SCC32XX_Interface *interface);
static void handleCPUTransaction(I2S_Handle handle, I2S_Transaction **ptrTransaction, I2SCC32XX_Interface *interface);

/* Extern globals */
extern const I2S_Config I2S_config[];
extern const uint_least8_t I2S_count;

/* Static globals */
static bool isInitialized = (bool)false;

/*
 *  ======== I2S_init ========
 */
void I2S_init(void) {
    uint_least8_t i;

    if (!isInitialized) {
        /* Init each instances' driver */
        for (i = 0; i < I2S_count; i++) {
            I2S_Handle handle = (I2S_Handle)&(I2S_config[i]);
            I2SCC32XX_Object *object = (I2SCC32XX_Object *)handle->object;
            object->isOpen = (bool)false;
        }
        /* Init as well UDMA */
        UDMACC32XX_init();
        isInitialized = (bool)true;
    }
}

/*
 *  ======== I2S_open ========
 */
I2S_Handle I2S_open(uint_least8_t index, I2S_Params *params) {
    I2S_Handle        handle;
    I2SCC32XX_Object *object;
    I2SCC32XX_HWAttrs const *hwAttrs;

    handle = (I2S_Handle)&(I2S_config[index]);
    object = handle->object;

    /* Get the pointer to the object and hwAttrs */
    hwAttrs = handle->hwAttrs;

    DebugP_assert(index < I2S_count);

    /* Check if module is initialized. */
    if (!isInitialized ||  object->isOpen) {
        handle =  NULL;
    }
    /* Initialization of the I2S-object and verification of the parameters. */
    else if (!initObject(handle, params)) {
        /* The parameters provided are not correct. */
        handle = NULL;
    }
    /* Set Power, set HW and register interrupts */
    else {
        /* Register HW interrupt */
        HwiP_Params hwiParams;
        HwiP_clearInterrupt(INT_I2S);
        HwiP_Params_init(&hwiParams);
        hwiParams.arg = (uintptr_t)handle;
        hwiParams.priority = hwAttrs->intPriority;
        object->hwi = HwiP_create(INT_I2S, (HwiP_Fxn)I2S_hwiIntFxn, &hwiParams);

        if (object->hwi == NULL) {
            /* HwiP creation failed */
            handle = NULL;
            restorePinpads(hwAttrs);
        }
        else {
            object->isOpen = (bool)true;

            /* Set Power */
            Power_setDependency(PowerCC32XX_PERIPH_I2S);
            MAP_PRCMPeripheralReset(PRCM_I2S);

            /* Set DMA */
            initDMA(handle);

            /* Set HardWare */
            initHw(handle);

            Power_registerNotify(&(object->notifyObj), PowerCC32XX_AWAKE_LPDS, postNotify, (uint32_t)handle);

            MAP_I2SIntDisable(I2S_BASE, 0xFFFFFFFF);

            uintptr_t key = HwiP_disable();
            /* Keep activatedFlags to be modified from somewhere else */
            object->activatedFlag = 0;
            HwiP_restore(key);
        }
    }

    return handle;
}

/*
 *  ======== I2S_close ========
 */
void I2S_close(I2S_Handle handle)
{
    /* Get the pointer to the object and hwAttrs */
    I2SCC32XX_Object *object = handle->object;
    I2SCC32XX_HWAttrs const *hwAttrs = handle->hwAttrs;

    /* Disable interrupts. */
    MAP_I2SIntDisable(I2S_BASE, 0xFFFFFFFF);

    uintptr_t key = HwiP_disable();
    /* Keep activatedFlags to be modified from somewhere else */
    object->activatedFlag = 0;
    HwiP_restore(key);

    /* Close the uDMA */
    if(object->dmaHandle != NULL) {
        UDMACC32XX_close(object->dmaHandle);
        object->dmaHandle = NULL;
    }

    /* Delete the I2S HWI */
    if(object->hwi != NULL) {
        HwiP_delete(object->hwi);
        object->hwi = NULL;
    }

    Power_unregisterNotify(&(object->notifyObj));
    /* Release power dependency - i.e. potentially power down serial domain. */
    Power_releaseDependency(PowerCC32XX_PERIPH_I2S);

    /* Restore pin pads to their reset states */
    restorePinpads(hwAttrs);

    /* Mark the module as available */
    object->isOpen = (bool)false;
}

/*
 *  ======== I2S_setReadQueueHead ========
 */
void I2S_setReadQueueHead(I2S_Handle handle, I2S_Transaction *transaction){

    DebugP_assert(transaction != 0x0);

    I2SCC32XX_Object    *object = handle->object;
    I2SCC32XX_Interface *interface = &object->read;

    interface->activeTransfer = transaction;
}

/*
 *  ======== I2S_setWriteQueueHead ========
 */
void I2S_setWriteQueueHead(I2S_Handle handle, I2S_Transaction *transaction){

    DebugP_assert(transaction != 0x0);

    I2SCC32XX_Object    *object = handle->object;
    I2SCC32XX_Interface *interface = &object->write;

    interface->activeTransfer = transaction;
}

/*
 *  ======== I2S_startClocks ========
 */
void I2S_startClocks(I2S_Handle handle) {

    I2SCC32XX_Object *object = handle->object;

    Power_setConstraint(PowerCC32XX_DISALLOW_LPDS);

    uint8_t mode = ((object->noOfInputs  != 0)? 1:0) << 1 |
                   ((object->noOfOutputs != 0)? 1:0) << 0;

    MAP_I2SIntEnable(I2S_BASE,   I2S_INT_RSYNCERR | I2S_INT_XSYNCERR);

    uintptr_t key = HwiP_disable();
    /* Keep activatedFlags to be modified from somewhere else */
    object->activatedFlag |= I2S_STS_RSYNCERR | I2S_STS_XSYNCERR;
    HwiP_restore(key);

    MAP_I2SEnable(I2S_BASE, (unsigned long)mode);
}

/*
 *  ======== I2S_stopClocks ========
 */
void I2S_stopClocks(I2S_Handle handle) {

    I2SCC32XX_Object *object = handle->object;

    /* Wait for the transfers to be properly terminated */
    while(object->isLastReadTransfer || object->isLastWriteTransfer);

    MAP_I2SIntDisable(I2S_BASE,    I2S_INT_RSYNCERR | I2S_INT_XSYNCERR);

    uintptr_t key = HwiP_disable();
    /* Keep activatedFlags to be modified from somewhere else */
    object->activatedFlag &= ~(I2S_STS_RSYNCERR | I2S_STS_XSYNCERR);
    HwiP_restore(key);

    MAP_I2SDisable(I2S_BASE);
    Power_releaseConstraint(PowerCC32XX_DISALLOW_LPDS);
}

/*
 *  ======== I2S_startRead ========
 */
void I2S_startRead(I2S_Handle handle) {

    I2SCC32XX_Object *object = handle->object;
    I2SCC32XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    I2SCC32XX_DataInterface   *SD0;
    I2SCC32XX_DataInterface   *SD1;

    /* Get the pointer to the SD0 and SD1 interfaces */
    SD0 = &object->dataInterfaceSD0;
    SD1 = &object->dataInterfaceSD1;

    if(object->noOfInputs != 0) {

        uintptr_t key = HwiP_disable();
        if(object->isDMAUnused) {
            /* Enable I2S_INT_RDATA | I2S_INT_ROVRN */
            HWREG(I2S_BASE + MCASP_O_EVTCTLR) |= (((I2S_INT_RDATA | I2S_INT_ROVRN) >> 16) & 0xFF);
            object->activatedFlag |= I2S_STS_ROVERN | I2S_STS_RDATA;
            object->updateDataReadFxn((uintptr_t)handle);
        }
        else {
            /* Turn on the DMA channel of the I2S Read interface */
            MAP_uDMAChannelAttributeEnable(hwAttrs->rxChannelIndex,UDMA_ATTR_USEBURST);
            MAP_uDMAChannelAssign(hwAttrs->rxChannelIndex);
            object->updateDataReadFxn((uintptr_t)handle);
            MAP_I2SRxFIFOEnable(I2S_BASE, object->udmaArbLength, object->noOfInputs);

            /* Enable I2S_INT_RDMA | I2S_INT_ROVRN | I2S_INT_RLAST */
            /* Note: we do not activate the I2S_STS_RDMAERR flag (uDMA's responsibility) */
            HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_MASK_CLR ) |= (((I2S_INT_RDMA | I2S_INT_ROVRN | I2S_INT_RLAST) &0xC0000000) >> 20);
            HWREG(I2S_BASE + MCASP_O_EVTCTLR) |= (((I2S_INT_RDMA | I2S_INT_ROVRN | I2S_INT_RLAST) >> 16) & 0xFF);
            object->activatedFlag |= I2S_STS_RLAST | I2S_STS_ROVERN | I2S_STS_RDMA;
        }
        HwiP_restore(key);

        if(SD0->interfaceConfig == I2S_SD0_INPUT) {
            MAP_I2SSerializerConfig(I2S_BASE, SD0->dataLine, I2S_SER_MODE_RX, I2S_INACT_LOW_LEVEL);
        }
        if(SD1->interfaceConfig == I2S_SD1_INPUT) {
            MAP_I2SSerializerConfig(I2S_BASE, SD1->dataLine, I2S_SER_MODE_RX, I2S_INACT_LOW_LEVEL);
        }
    }
}

/*
 *  ======== I2S_startWrite ========
 */
void I2S_startWrite(I2S_Handle handle) {

    I2SCC32XX_Object *object = handle->object;
    I2SCC32XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    I2SCC32XX_DataInterface *SD0;
    I2SCC32XX_DataInterface *SD1;

    /* Get the pointer to the SD0 and SD1 interfaces */
    SD0 = &object->dataInterfaceSD0;
    SD1 = &object->dataInterfaceSD1;

    if(object->noOfOutputs != 0) {

        uintptr_t key = HwiP_disable();
        if(object->isDMAUnused) {
            /* Enable I2S_INT_XDATA | I2S_INT_XUNDRN */
            HWREG(I2S_BASE + MCASP_O_EVTCTLX) |= ((I2S_INT_XDATA | I2S_INT_XUNDRN) & 0xFF);
            object->activatedFlag |= I2S_STS_XDATA | I2S_STS_XUNDRN;
            object->updateDataWriteFxn((uintptr_t)handle);
        }
        else {
            /* Turn on the DMA channel of the I2S Write interface */
            MAP_uDMAChannelAttributeEnable(hwAttrs->txChannelIndex,UDMA_ATTR_USEBURST);
            MAP_uDMAChannelAssign(hwAttrs->txChannelIndex);
            object->updateDataWriteFxn((uintptr_t)handle);
            MAP_I2STxFIFOEnable(I2S_BASE, object->udmaArbLength, object->noOfOutputs);

            /* Enable I2S_INT_XDMA | I2S_INT_XLAST | I2S_INT_XUNDRN */
            /* Note: we do not activate the I2S_STS_XDMAERR flag (uDMA's responsibility) */
            HWREG(APPS_CONFIG_BASE + APPS_CONFIG_O_DMA_DONE_INT_MASK_CLR ) |= (((I2S_INT_XDMA | I2S_INT_XLAST | I2S_INT_XUNDRN) &0xC0000000) >> 20);
            HWREG(I2S_BASE + MCASP_O_EVTCTLX) |= ((I2S_INT_XDMA | I2S_INT_XLAST | I2S_INT_XUNDRN) & 0xFF);
            object->activatedFlag |= I2S_STS_XLAST | I2S_STS_XUNDRN | I2S_STS_XDMA;
        }
        HwiP_restore(key);

        if(SD0->interfaceConfig == I2S_SD0_OUTPUT) {
            MAP_I2SSerializerConfig(I2S_BASE, SD0->dataLine, I2S_SER_MODE_TX, I2S_INACT_LOW_LEVEL);
        }
        if(SD1->interfaceConfig == I2S_SD1_OUTPUT) {
            MAP_I2SSerializerConfig(I2S_BASE, SD1->dataLine, I2S_SER_MODE_TX, I2S_INACT_LOW_LEVEL);
        }
    }
}

/*
 *  ======== I2S_stopRead ========
 */
void I2S_stopRead(I2S_Handle handle) {

    I2SCC32XX_Object *object = handle->object;
    I2SCC32XX_DataInterface *SD0;
    I2SCC32XX_DataInterface *SD1;

    /* Get the pointer to the SD0 and SD1 interfaces*/
    SD0 = &object->dataInterfaceSD0;
    SD1 = &object->dataInterfaceSD1;

    if(object->noOfInputs != 0) {

        if(!object->isDMAUnused) {
            /* To stop the Read interface, we must wait for the end of the current DMA transaction*/
            object->isLastReadTransfer = (bool)true;
        }
        else {

            MAP_I2SIntDisable(I2S_BASE, I2S_INT_RDMA | I2S_INT_ROVRN );

            uintptr_t key = HwiP_disable();
            /* Keep activatedFlag to be modified from somewhere else */
            object->activatedFlag &= ~(I2S_STS_RDMA  | I2S_STS_ROVERN);
            HwiP_restore(key);

            if(SD0->interfaceConfig == I2S_SD0_INPUT) {
                MAP_I2SSerializerConfig(I2S_BASE, SD0->dataLine, I2S_SER_MODE_DISABLE, I2S_INACT_LOW_LEVEL);
            }
            if(SD1->interfaceConfig == I2S_SD1_INPUT) {
                MAP_I2SSerializerConfig(I2S_BASE, SD1->dataLine, I2S_SER_MODE_DISABLE, I2S_INACT_LOW_LEVEL);
            }
        }
    }
}

/*
 *  ======== I2S_stopWrite ========
 */
void I2S_stopWrite(I2S_Handle handle) {

    I2SCC32XX_Object *object = handle->object;
    I2SCC32XX_DataInterface *SD0;
    I2SCC32XX_DataInterface *SD1;

    /* Get the pointer to the SD0 and SD1 interfaces*/
    SD0 = &object->dataInterfaceSD0;
    SD1 = &object->dataInterfaceSD1;

    if(object->noOfOutputs != 0) {

        if(!object->isDMAUnused) {
            /* To stop the Write interface, we must wait for the end of the current DMA transaction*/
            object->isLastWriteTransfer = (bool)true;
        }
        else {

            MAP_I2SIntDisable(I2S_BASE, I2S_INT_XDMA | I2S_INT_XUNDRN);

            uintptr_t key = HwiP_disable();
            /* Keep activatedFlag to be modified from somewhere else */
            object->activatedFlag &= ~(I2S_STS_XDMA | I2S_STS_XUNDRN);
            HwiP_restore(key);

            if(SD0->interfaceConfig == I2S_SD0_OUTPUT) {
                MAP_I2SSerializerConfig(I2S_BASE, SD0->dataLine, I2S_SER_MODE_DISABLE, I2S_INACT_LOW_LEVEL);
            }
            if(SD1->interfaceConfig == I2S_SD1_OUTPUT) {
                MAP_I2SSerializerConfig(I2S_BASE, SD1->dataLine, I2S_SER_MODE_DISABLE, I2S_INACT_LOW_LEVEL);
            }
        }
    }
}

/*
*  ======== initHw ========
*  This functions initializes the I2S hardware module.
*
*  @pre    Function assumes that the I2S handle is pointing to a hardware
*          module which has already been opened.
*/
static void initHw(I2S_Handle handle) {

    /* Configure IOs */
    initIO(handle);
    /* Configure SCK clock (including sampling edge) */
    configSCK(handle);
    /* Configure WS clock (including WS inversion) */
    configWS(handle);
    /* Configure serial format (data-length, DMA use, MSB/LSB first, data padding, channels activated) */
    configSerialFormat(handle);
}

/*
 *  ======== I2S_hwiIntFxn ========
 *  Hwi function that processes I2S interrupts.
 */
static void I2S_hwiIntFxn(uintptr_t arg) {

    I2S_Handle handle = (I2S_Handle)arg;
    I2SCC32XX_Object const *object = handle->object;
    uint16_t errStatus = 0U;

    /* Read and clear all the interrupt flags */
    /* Note: in CPU mode, RDATA and XDATA flags must be cleared AFTER data update */
    uint32_t rawInterruptStatus = (uint32_t)MAP_I2SIntStatus(I2S_BASE);
    if(object->isDMAUnused) {
        MAP_I2SIntClear(I2S_BASE,  rawInterruptStatus & ~I2S_STS_RDATA & ~I2S_STS_XDATA);
    }
    else {
        MAP_I2SIntClear(I2S_BASE, rawInterruptStatus);
    }

    /* Filter the interrupt-status to only consider the activated flags */
    uint32_t interruptStatus = (rawInterruptStatus & object->activatedFlag);


    /* Normal work */
    if((interruptStatus & (uint32_t)(I2S_STS_RLAST | I2S_STS_RDATA | I2S_STS_RDMA)) != 0U)
    {
        object->updateDataReadFxn(arg);
    }

    if((interruptStatus & (uint32_t)(I2S_STS_XLAST | I2S_STS_XDATA | I2S_STS_XDMA)) != 0U)
    {
        object->updateDataWriteFxn(arg);
    }

    /* Read errors */
    if((interruptStatus & (uint32_t)I2S_STS_ROVERN  ) != 0U)
    {
        errStatus |= I2S_PTR_READ_ERROR;
    }

    if((interruptStatus & (uint32_t)I2S_STS_RSYNCERR) != 0U)
    {
        errStatus |= I2S_TIMEOUT_ERROR;
    }

    if((interruptStatus & (uint32_t)I2S_STS_RDMAERR ) != 0U)
    {
        errStatus |= I2S_BUS_ERROR;
    }

    /* Write errors */
    if((interruptStatus & (uint32_t)I2S_STS_XUNDRN  ) != 0U)
    {
        errStatus |= I2S_PTR_WRITE_ERROR;
    }

    if((interruptStatus & (uint32_t)I2S_STS_XSYNCERR) != 0U)
    {
        errStatus |= I2S_WS_ERROR;
    }

    if((interruptStatus & (uint32_t)I2S_STS_XDMAERR ) != 0U)
    {
        errStatus |= I2S_BUS_ERROR;
    }

    /* Triggering of the error-Callback (only one call, with potentially several status bits activated) */
    if(errStatus != 0U) {
        object->errorCallback(handle, errStatus, NULL);
    }
}

/*
 *  ======== updateDataReadDMA ========
 */
static void updateDataReadDMA(uintptr_t arg) {
    I2S_Handle handle = (I2S_Handle)arg;
    I2SCC32XX_Object *object = handle->object;
    I2SCC32XX_HWAttrs const *hwAttrs = handle->hwAttrs;

    /* Wait for the mode to be changed to "stopped" */
    if(MAP_uDMAChannelModeGet(hwAttrs->rxChannelIndex) == UDMA_MODE_STOP){

        if(!object->isLastReadTransfer) {

            I2S_Transaction *transaction = (I2S_Transaction *)object->read.activeTransfer;
            if(transaction != NULL) {

                uint32_t transLength = transaction->bufSize - transaction->bytesTransferred;                   /* transfer length expressed in bytes */
                transLength = transLength / (object->memorySlotLength / BYTE_LENGTH);                          /* expressed in number of samples (number of atomic DMA transfers) */
                transLength = (transLength > UDMA_MAX_TRANSFERLENGTH)? UDMA_MAX_TRANSFERLENGTH : transLength;  /* the transfer length of the uDMA is limited */
                transLength = transLength - (transLength % object->udmaArbLength);                             /* taking in consideration the limitation of the DMA (DMA copies each time UDMA_ARB_x samples) */

                MAP_uDMAChannelControlSet(hwAttrs->rxChannelIndex,
                                      object->read.udmaConfig);
                MAP_uDMAChannelTransferSet(hwAttrs->rxChannelIndex,
                                       UDMA_MODE_BASIC,
                                       (void *)I2S_RX_DMA_PORT,
                                       (void *)((uint32_t)transaction->bufPtr + transaction->bytesTransferred),
                                       transLength);

                MAP_uDMAChannelEnable(hwAttrs->rxChannelIndex);

                transLength = transLength * (object->memorySlotLength / BYTE_LENGTH);                         /* handleDMATransaction() expects transLength in bytes (not in samples): re-convert transLength in bytes */
                handleDMATransaction(handle, transaction, transLength, &object->read);
            }
        }
        else {
            I2SCC32XX_DataInterface *SD0;
            I2SCC32XX_DataInterface *SD1;

            /* Get the pointer to the SD0 and SD1 interfaces*/
            SD0 = &object->dataInterfaceSD0;
            SD1 = &object->dataInterfaceSD1;

            MAP_I2SIntDisable(I2S_BASE, I2S_INT_RLAST | I2S_INT_RDMA | I2S_INT_ROVRN);
            object->activatedFlag &=  ~(I2S_STS_RLAST | I2S_STS_RDMA | I2S_STS_ROVERN);

            MAP_I2SRxFIFODisable(I2S_BASE);

            if(SD0->interfaceConfig == I2S_SD0_INPUT) {
                MAP_I2SSerializerConfig(I2S_BASE, SD0->dataLine, I2S_SER_MODE_DISABLE, I2S_INACT_LOW_LEVEL);
            }
            if(SD1->interfaceConfig == I2S_SD1_INPUT) {
                MAP_I2SSerializerConfig(I2S_BASE, SD1->dataLine, I2S_SER_MODE_DISABLE, I2S_INACT_LOW_LEVEL);
            }

            object->isLastReadTransfer = (bool)false;
        }
    }
}

/*
 *  ======== updateDataWriteDMA ========
 */
static void updateDataWriteDMA(uintptr_t arg) {
    I2S_Handle handle = (I2S_Handle)arg;
    I2SCC32XX_Object *object = handle->object;
    I2SCC32XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    I2SCC32XX_DataInterface *SD0;
    I2SCC32XX_DataInterface *SD1;

    /* Wait for the mode to be changed to "stopped" */
    if(MAP_uDMAChannelModeGet(hwAttrs->txChannelIndex) == UDMA_MODE_STOP){

        if(!object->isLastWriteTransfer) {
            I2S_Transaction *transaction = (I2S_Transaction *)object->write.activeTransfer;
            if(transaction != NULL) {

                uint32_t transLength = transaction->bufSize - transaction->bytesTransferred;                   /* transfer length expressed in bits */
                transLength = transLength / (object->memorySlotLength / BYTE_LENGTH);                          /* expressed in number of samples (number of atomic DMA transfers) */
                transLength = (transLength > UDMA_MAX_TRANSFERLENGTH)? UDMA_MAX_TRANSFERLENGTH : transLength;  /* the transfer length of the uDMA is limited */
                transLength = transLength - (transLength % object->udmaArbLength);                             /* taking in consideration the limitation of the DMA (DMA copies each time UDMA_ARB_x samples) */

                MAP_uDMAChannelControlSet(hwAttrs->txChannelIndex,
                                      object->write.udmaConfig);
                MAP_uDMAChannelTransferSet(hwAttrs->txChannelIndex,
                                       UDMA_MODE_BASIC,
                                       (void *)((uint32_t)transaction->bufPtr + transaction->bytesTransferred),
                                       (void *)I2S_TX_DMA_PORT,
                                       transLength);

                MAP_uDMAChannelEnable(hwAttrs->txChannelIndex);

                transLength = transLength * (object->memorySlotLength / BYTE_LENGTH);                         /* handleDMATransaction() expects transLength in bytes (not in samples): re-convert transLength in bytes */
                handleDMATransaction(handle, transaction, transLength, &object->write);
            }
        }

        else {

            /* Get the pointer to the SD0 and SD1 interfaces*/
            SD0 = &object->dataInterfaceSD0;
            SD1 = &object->dataInterfaceSD1;

            MAP_I2SIntDisable(I2S_BASE, I2S_INT_XDMA | I2S_INT_XUNDRN | I2S_INT_XLAST);
            object->activatedFlag &=  ~(I2S_STS_XDMA | I2S_STS_XUNDRN | I2S_STS_XLAST);

            MAP_I2STxFIFODisable(I2S_BASE);

            if(SD0->interfaceConfig == I2S_SD0_OUTPUT) {
                MAP_I2SSerializerConfig(I2S_BASE, SD0->dataLine, I2S_SER_MODE_DISABLE, I2S_INACT_LOW_LEVEL);
            }
            if(SD1->interfaceConfig == I2S_SD1_OUTPUT) {
                MAP_I2SSerializerConfig(I2S_BASE, SD1->dataLine, I2S_SER_MODE_DISABLE, I2S_INACT_LOW_LEVEL);
            }

            object->isLastWriteTransfer = (bool)false;
        }
    }
}

/*
 *  ======== handleDMATransaction ========
 *  This function avoid to duplicate the code dedicated to take care of the transactions in DMA mode.
 *  This function remove the transaction from the list and trigger the user callback.
 *
 */
static void handleDMATransaction(I2S_Handle      handle,
                                 I2S_Transaction *transactionFinished,
                                 uint32_t        transLength,
                                 I2SCC32XX_Interface   *interface){

    I2SCC32XX_Object *object = handle->object;

    /* Is the transaction finished? (i.e. all the element are sent or we cannot perform the next DMA) */
    uint16_t dmaMinimumStep = object->udmaArbLength * object->memorySlotLength / BYTE_LENGTH;
    if(((transactionFinished->bytesTransferred + transLength + dmaMinimumStep) > transactionFinished->bufSize)) {

        /* This transaction is finished */
        interface->activeTransfer = (I2S_Transaction*)List_next(&transactionFinished->queueElement);

        transactionFinished->untransferredBytes = transactionFinished->bufSize - transLength;
        transactionFinished->numberOfCompletions ++;
        transactionFinished->bytesTransferred = 0;

        if(interface->activeTransfer == NULL) {
            interface->callback(handle, I2S_ALL_TRANSACTIONS_SUCCESS, transactionFinished);

            /* If needed the interface must be stopped */
            if(interface->activeTransfer != NULL){
                /* Application called I2S_setXxxxQueueHead()
                 *     -> new transfers can be executed next time */
            }
            else if((I2S_Transaction*)List_next(&transactionFinished->queueElement) != NULL){
                /* Application queued transactions after the transaction finished
                 *     -> activeTransfer must be modified and transfers can be executed next time */
                interface->activeTransfer = (I2S_Transaction*)List_next(&transactionFinished->queueElement);
            }
            else {
                /* Application did nothing, we need to stop the interface to avoid errors */
                interface->stopInterface(handle);
            }
        }
        else {
            interface->callback(handle, I2S_TRANSACTION_SUCCESS, transactionFinished);
        }
    }
    else{
        transactionFinished->bytesTransferred += transLength;
    }
}

/*
 *  ======== updateDataReadCPU ========
 */
static void updateDataReadCPU(uintptr_t arg) {
    I2S_Handle handle = (I2S_Handle)arg;
    I2SCC32XX_Object *object = handle->object;
    I2SCC32XX_DataInterface   *SD0;
    I2SCC32XX_DataInterface   *SD1;

    /* Get the pointer to the SD0 and SD1 interfaces*/
    SD0 = &object->dataInterfaceSD0;
    SD1 = &object->dataInterfaceSD1;

    I2S_Transaction *transaction = (I2S_Transaction *)object->read.activeTransfer;
    if(transaction != NULL) {

        bool dataKO = (bool)false;
        uint32_t *ptrValue;
        ptrValue = (uint32_t*)((uint32_t)transaction->bufPtr + (uint32_t)transaction->bytesTransferred);

        while((!dataKO) && (transaction != NULL)) {

            if((SD0->interfaceConfig == I2S_SD0_INPUT) && (transaction != NULL) && (!dataKO)) {

                dataKO |= MAP_I2SDataGetNonBlocking(I2S_BASE, SD0->dataLine, (unsigned long*)ptrValue);

                if(!dataKO) {

                    handleCPUTransaction(handle, &transaction, &object->read);

                    if(transaction != NULL){
                        ptrValue = (uint32_t*)((uint32_t)transaction->bufPtr + (uint32_t)transaction->bytesTransferred);
                    }
                }
            }

            if((SD1->interfaceConfig == I2S_SD1_INPUT) && (transaction != NULL) && (!dataKO)) {

                dataKO |= MAP_I2SDataGetNonBlocking(I2S_BASE, SD1->dataLine, (unsigned long*)ptrValue);

                if(!dataKO) {

                    handleCPUTransaction(handle, &transaction, &object->read);

                    if(transaction != NULL){
                        ptrValue = (uint32_t*)((uint32_t)transaction->bufPtr + (uint32_t)transaction->bytesTransferred);
                    }
                }
            }
        }
    }

    /* In CPU mode interrupt clearing must be done AFTER data update */
    MAP_I2SIntClear(I2S_BASE,I2S_STS_RDATA);
}

/*
 *  ======== updateDataWriteCPU ========
 */
static void updateDataWriteCPU(uintptr_t arg) {
    I2S_Handle handle = (I2S_Handle)arg;
    I2SCC32XX_Object *object = handle->object;
    I2SCC32XX_DataInterface   *SD0;
    I2SCC32XX_DataInterface   *SD1;

    /* Get the pointer to the SD0 and SD1 interfaces*/
    SD0 = &object->dataInterfaceSD0;
    SD1 = &object->dataInterfaceSD1;

    I2S_Transaction *transaction = (I2S_Transaction *)object->write.activeTransfer;
    if(transaction != NULL) {

        bool dataKO = (bool)false;
        uint32_t Value;
        uint32_t *ptrValue = &Value;
        ptrValue = (uint32_t*)((uint32_t)transaction->bufPtr + (uint32_t)transaction->bytesTransferred);
        Value = *ptrValue;

        while((!dataKO) && (transaction != NULL)) {

            if((SD0->interfaceConfig == I2S_SD0_OUTPUT) && (transaction != NULL) && (!dataKO)) {

                dataKO |= MAP_I2SDataPutNonBlocking(I2S_BASE, SD0->dataLine, Value);

                if(!dataKO) {

                    handleCPUTransaction(handle, &transaction, &object->write);

                    if(transaction != NULL){
                        ptrValue = (uint32_t*)((uint32_t)transaction->bufPtr + (uint32_t)transaction->bytesTransferred);
                        Value = *ptrValue;
                    }
                }
            }

            if((SD1->interfaceConfig == I2S_SD1_OUTPUT) && (transaction != NULL) && (!dataKO)) {

                dataKO |= MAP_I2SDataPutNonBlocking(I2S_BASE, SD1->dataLine, Value);

                if(!dataKO) {

                    handleCPUTransaction(handle, &transaction, &object->write);

                    if(transaction != NULL){
                        ptrValue = (uint32_t*)((uint32_t)transaction->bufPtr + (uint32_t)transaction->bytesTransferred);
                        Value = *ptrValue;
                    }
                }
            }
        }
    }

    /* In CPU mode interrupt clearing must be done AFTER data update */
    MAP_I2SIntClear(I2S_BASE,I2S_STS_XDATA);
}

/*
 *  ======== handleCPUTransaction ========
 *  This function avoid to duplicate the code dedicated to take care of the transactions in CPU mode.
 *  If needed, this function remove the transaction from the list and trigger the user callback.
 */
static void handleCPUTransaction(I2S_Handle handle,
                                 I2S_Transaction **ptrTransaction,
                                 I2SCC32XX_Interface *interface){

    I2SCC32XX_Object const *object = handle->object;
    I2S_Transaction *transaction = *ptrTransaction;

    transaction->bytesTransferred += object->memorySlotLength / BYTE_LENGTH;

    if(transaction->bytesTransferred >= transaction->bufSize){

        /* We note the transaction as finished */
        transaction->numberOfCompletions ++;
        transaction->untransferredBytes = transaction->bufSize - transaction->bytesTransferred;
        transaction->bytesTransferred = 0;

        /* We now take in consideration the next transaction */
        interface->activeTransfer = (I2S_Transaction*)List_next(&transaction->queueElement);
        *ptrTransaction = (I2S_Transaction*)interface->activeTransfer; /* transaction pointer must be updated because
                                                                        * it is potentially reused by the calling function */

        I2S_Transaction *newTransaction = *ptrTransaction;
        if(newTransaction == NULL){
            /* It's too late for the user to queue new transactions: the hardware is already running out of data */
            interface->stopInterface(handle);
        }
        else if(List_next(&newTransaction->queueElement) == NULL) {
            interface->callback(handle, I2S_ALL_TRANSACTIONS_SUCCESS, transaction);
        }
        else {
            interface->callback(handle, I2S_TRANSACTION_SUCCESS, transaction);
        }
    }
}

/*
 *  ======== initObject ========
 */
static bool initObject(I2S_Handle handle, I2S_Params *params) {

    I2SCC32XX_Object          *object = handle->object;
    I2SCC32XX_DataInterface   *SD0;
    I2SCC32XX_DataInterface   *SD1;

    uint8_t indexDmaConfig = 0;
    uint8_t indexDmaArbConfig = 0;

    bool retVal = (bool)true;

    /* Get the pointer to the SD0 and SD1 interfaces*/
    SD0 = &object->dataInterfaceSD0;
    SD1 = &object->dataInterfaceSD1;

    object->moduleRole           = params->moduleRole;
    object->invertWS             = params->invertWS;
    object->samplingEdge         = params->samplingEdge;
    object->isMSBFirst           = params->isMSBFirst;
    object->samplingFrequency    = params->samplingFrequency;
    object->dataShift            = (params->trueI2sFormat)? DATA_SHIFT_1 : DATA_SHIFT_NONE;
    object->isLastReadTransfer   = (bool)false;
    object->isLastWriteTransfer  = (bool)false;

    DebugP_assert(params->memorySlotLength == I2S_MEMORY_LENGTH_8BITS  ||
                  params->memorySlotLength == I2S_MEMORY_LENGTH_16BITS ||
                  params->memorySlotLength == I2S_MEMORY_LENGTH_24BITS ||
                  params->memorySlotLength == I2S_MEMORY_LENGTH_32BITS   );

    if(!params->isDMAUnused){
        if(params->memorySlotLength <= I2S_MEMORY_LENGTH_8BITS){
            indexDmaConfig = 0;
            object->memorySlotLength = I2S_MEMORY_LENGTH_8BITS;
        }
        else if(params->memorySlotLength <= I2S_MEMORY_LENGTH_16BITS){
            indexDmaConfig = 1;
            object->memorySlotLength = I2S_MEMORY_LENGTH_16BITS;
        }
        else if(params->memorySlotLength <= I2S_MEMORY_LENGTH_32BITS){
            indexDmaConfig = 2;
            object->memorySlotLength = I2S_MEMORY_LENGTH_32BITS;
        }
        else {
            retVal = (bool)false;
        }
    }
    else {
        indexDmaConfig = 0;
        object->memorySlotLength = params->memorySlotLength;
    }

    /* Computation of the sampleMask and the sampleRotation (to achieve before and after word padding) */
    if(params->beforeWordPadding + params->bitsPerWord + params->afterWordPadding <= 32) {
        object->dataLength      = params->beforeWordPadding + params->bitsPerWord + params->afterWordPadding;

        /* This is the default setting. Data are right aligned. */
        object->sampleMask      = 0xFFFFFFFF >> (32-object->memorySlotLength);
        object->sampleRotation  = (object->dataLength / HWRESOLUTION) % BYTE_LENGTH;

        /* Data truncation is made by masking the LSB of the samples. Data are still right aligned. */
        if(params->bitsPerWord < object->memorySlotLength) {
            object->sampleMask &= (0xFFFFFFFF << (object->memorySlotLength - params->bitsPerWord));
            object->sampleRotation += ((object->memorySlotLength - params->bitsPerWord) / HWRESOLUTION) % BYTE_LENGTH;
        }

        /* Data padding is made by increasing the left rotation (i.e. decreasing right rotation) */
        if(params->bitsPerWord > object->memorySlotLength) {
            object->sampleRotation -= ((params->bitsPerWord - object->memorySlotLength) / HWRESOLUTION) % BYTE_LENGTH;
        }

        /* After word padding is made by increasing the left rotation (i.e. decreasing right rotation) */
        if(params->afterWordPadding) {
            object->sampleRotation -= (params->afterWordPadding / HWRESOLUTION) % BYTE_LENGTH;
        }
    }
    else {
        retVal = (bool)false;
    }

    object->isDMAUnused          = params->isDMAUnused;
    if(object->isDMAUnused) {
        object->updateDataReadFxn  = updateDataReadCPU;
        object->updateDataWriteFxn = updateDataWriteCPU;
    }
    else {
        object->updateDataReadFxn  = updateDataReadDMA;
        object->updateDataWriteFxn = updateDataWriteDMA;
    }

    SD0->interfaceConfig      = params->SD0Use;
    SD0->channelsUsed         = params->SD0Channels;
    SD0->numberOfChannelsUsed = getNumberOfChannels((uint8_t)SD0->channelsUsed);
    SD0->dataLine             = I2S_DATA_LINE_0;
    SD1->interfaceConfig      = params->SD1Use;
    SD1->channelsUsed         = params->SD1Channels;
    SD1->numberOfChannelsUsed = getNumberOfChannels((uint8_t)SD1->channelsUsed);
    SD1->dataLine             = I2S_DATA_LINE_1;

    object->noOfInputs          =   ((SD0->interfaceConfig == I2S_SD0_INPUT)? 1: 0)
                                  + ((SD1->interfaceConfig == I2S_SD1_INPUT)? 1: 0);
    object->noOfOutputs         =   ((SD0->interfaceConfig == I2S_SD0_OUTPUT)? 1: 0)
                                  + ((SD1->interfaceConfig == I2S_SD1_OUTPUT)? 1: 0);

    /* Here we are looking for the biggest possible  uDMA arbitration length */
    if(params->fixedBufferLength == 0) {
        retVal = (bool)false;
    }
    else {
        if(params->fixedBufferLength != 1) {
            uint16_t const fixedBufferLength = (params->fixedBufferLength /(object->memorySlotLength/BYTE_LENGTH));
            int i;

            /* The udmaArbLength minimum value is 2 (= 2^1)*/
            object->udmaArbLength = 2;
            /* The udmaArbLength maximum value is 16 (= 2^4)*/
            for(i=2; i<=4; i++) {
                if(fixedBufferLength % (1 << i) == 0) {
                    object->udmaArbLength = 1 << i;
                }
            }
        }
        else {
            object->udmaArbLength = UDMA_ARBLENGTH_DEFAULT;
        }
    }

    /* Authorized Arbitration values are limited to 2, 4, 8, 16 */
    DebugP_assert(object->udmaArbLength == 2  ||
                  object->udmaArbLength == 4  ||
                  object->udmaArbLength == 8  ||
                  object->udmaArbLength == 16   );

    int i;
    for(i=0; i<BYTE_LENGTH; i++) {
        if(object->udmaArbLength == (1 << i)) {
            indexDmaArbConfig = i;
        }
    }

    object->read.udmaConfig = dmaRxConfig[indexDmaConfig] | dmaArbConfig[indexDmaArbConfig];
    object->read.stopInterface = I2S_stopRead;
    object->read.callback = params->readCallback;
    if((object->read.callback == NULL) && (object->noOfInputs != 0U)) {
        retVal = (bool)false;
    }

    object->write.udmaConfig = dmaTxConfig[indexDmaConfig] | dmaArbConfig[indexDmaArbConfig];
    object->write.stopInterface = I2S_stopWrite;
    object->write.callback = params->writeCallback;
    if((object->write.callback == NULL) && (object->noOfOutputs != 0U)) {
        retVal = (bool)false;
    }

    object->errorCallback = params->errorCallback;
    if(object->errorCallback == NULL) {
        retVal = (bool)false;
    }

    object->read.activeTransfer   = NULL;
    object->write.activeTransfer  = NULL;

    return retVal;
}

/*
 *  ======== initIO ========
 */
static void initIO(I2S_Handle handle) {

    I2SCC32XX_HWAttrs const   *hwAttrs;

    /* Get the pointer to the object and hwAttrs */
    hwAttrs = handle->hwAttrs;

    DebugP_assert(hwAttrs->pinSD1 == I2SCC32XX_PIN_50_SD1 ||
                  hwAttrs->pinSD1 == I2SCC32XX_PIN_60_SD1 ||
                  hwAttrs->pinSD1 == I2SCC32XX_PIN_UNUSED);

    DebugP_assert(hwAttrs->pinSD0 == I2SCC32XX_PIN_45_SD0 ||
                  hwAttrs->pinSD0 == I2SCC32XX_PIN_50_SD0 ||
                  hwAttrs->pinSD0 == I2SCC32XX_PIN_52_SD0 ||
                  hwAttrs->pinSD0 == I2SCC32XX_PIN_64_SD0 ||
                  hwAttrs->pinSD0 == I2SCC32XX_PIN_UNUSED);

    DebugP_assert(hwAttrs->pinSCK == I2SCC32XX_PIN_03_SCK ||
                  hwAttrs->pinSCK == I2SCC32XX_PIN_52_SCK ||
                  hwAttrs->pinSCK == I2SCC32XX_PIN_53_SCK ||
                  hwAttrs->pinSCK == I2SCC32XX_PIN_UNUSED);

    DebugP_assert(hwAttrs->pinWS  == I2SCC32XX_PIN_02_WS ||
                  hwAttrs->pinWS  == I2SCC32XX_PIN_15_WS ||
                  hwAttrs->pinWS  == I2SCC32XX_PIN_17_WS ||
                  hwAttrs->pinWS  == I2SCC32XX_PIN_21_WS ||
                  hwAttrs->pinWS  == I2SCC32XX_PIN_45_WS ||
                  hwAttrs->pinWS  == I2SCC32XX_PIN_53_WS ||
                  hwAttrs->pinWS  == I2SCC32XX_PIN_63_WS);

    DebugP_assert(hwAttrs->pinSD1 == I2SCC32XX_PIN_62_SCKX ||
                  hwAttrs->pinSD1 == I2SCC32XX_PIN_UNUSED);

    openPin(hwAttrs->pinSD1);
    openPin(hwAttrs->pinSD0);
    openPin(hwAttrs->pinSCK);
    openPin(hwAttrs->pinSCKX);
    openPin(hwAttrs->pinWS);
}

/*
 *  ======== openPin ========
 */
static void openPin(unsigned long pinI2S) {

    if(pinI2S != I2SCC32XX_PIN_UNUSED) {
        MAP_PinTypeI2S((unsigned long)PinConfigPin(pinI2S),
                   (unsigned long)PinConfigPinMode(pinI2S));
    }
}

/*
 *  ======== resetPin ========
 */
static void resetPin(unsigned long pinI2S) {

    if(pinI2S != I2SCC32XX_PIN_UNUSED) {
        uint32_t padRegister = (PinToPadGet((pinI2S) & 0x3f)<<2) + PAD_CONFIG_BASE;
        HWREG(padRegister) = PAD_RESET_STATE;
    }
}

/*
 *  ======== restorePinpads ========
 */
static void restorePinpads(I2SCC32XX_HWAttrs const *hwAttrs){
    resetPin(hwAttrs->pinSD1);
    resetPin(hwAttrs->pinSD0);
    resetPin(hwAttrs->pinSCK);
    resetPin(hwAttrs->pinSCKX);
    resetPin(hwAttrs->pinWS);
}

/*
 *  ======== initDMA ========
 */
static void initDMA(I2S_Handle handle) {

    I2SCC32XX_Object *object;
    object  = handle->object;

    if(!object->isDMAUnused) {

        object->dmaHandle = UDMACC32XX_open();
    }
}

/*
 *  ======== getNumberOfChannels ========
 */
static uint8_t getNumberOfChannels(const uint8_t channelsList) {

    uint8_t i = 0;
    uint8_t nbChannelsUsed = 0;
    for(i=0; i<(I2S_NB_CHANNELS_MAX); i++) {
        if(((channelsList >> i) & 1U) == 1U) {
            nbChannelsUsed ++;
        }
    }
    return nbChannelsUsed;
}

/*
 *  ======== configSCK ========
 */
static void configSCK(I2S_Handle handle) {

    I2SCC32XX_Object const *object;

    /* Get the pointer to the object*/
    object = handle->object;

    /* There are three freq dividers (one in the PRCM module, two in the I2S module).
     * Two of the three freq dividers are used to keep the possibility to activate
     * only the input (or only the output).
     * Smallest SCK frequency possible is 57 Hz. */
    unsigned long ulI2SClk  = object->samplingFrequency * object->dataLength * 2;
    unsigned long ulBitClk  = (ulI2SClk < PRCMI2SCLOCKFREQMIN)? PRCMI2SCLOCKFREQMIN : ulI2SClk;
    MAP_PRCMI2SClockFreqSet(ulBitClk);

    /* Calculate remaining divider */
    unsigned long remainingDivider = ((ulBitClk + ulI2SClk/2) / ulI2SClk); /* We want to round the integer division: ROUND(a/b)=(a+b/2)/b */;
    unsigned long ulHClkDiv = (remainingDivider == 0)? 0 :(remainingDivider-1);
    ulHClkDiv = (ulHClkDiv > HCLKDIVMAX)? HCLKDIVMAX : ulHClkDiv;
    unsigned long ulClkDiv  = 0; /* ulClkDiv is not activated */

    HWREG(I2S_BASE + MCASP_O_AHCLKXCTL) = (MCASP_AHCLKXCTL_HCLKXM_DEFAULT|ulHClkDiv);
    if(object->noOfOutputs != 0) {

        HWREG(I2S_BASE + MCASP_O_ACLKXCTL)  = (((object->samplingEdge << 7)   & MCASP_ACLKXCTL_CLKXP) |
                                               ((object->moduleRole   << 5)   & MCASP_ACLKXCTL_CLKXM) |
                                               ((ulClkDiv             << 0)   & MCASP_ACLKXCTL_CLKXDIV_M));
    }

    HWREG(I2S_BASE + MCASP_O_AHCLKRCTL) = (MCASP_AHCLKRCTL_HCLKRM_DEFAULT|ulHClkDiv);
    if(object->noOfInputs != 0) {

        HWREG(I2S_BASE + MCASP_O_ACLKRCTL)  = (((object->samplingEdge << 7)   & MCASP_ACLKRCTL_CLKRP) |
                                               ((object->moduleRole   << 5)   & MCASP_ACLKRCTL_CLKRM) |
                                               ((ulClkDiv             << 0)   & MCASP_ACLKRCTL_CLKRDIV_M));
    }

}

/*
 *  ======== configWS ========
 */
static void configWS(I2S_Handle handle) {

    I2SCC32XX_Object const *object;

    /* Get the pointer to the object*/
    object = handle->object;

    if(object->noOfInputs != 0) {
        HWREG(I2S_BASE + MCASP_O_RXFMCTL) =  (  MCASP_RXFMCTL_RMOD_DEFAULT                       |
                                                MCASP_RXFMCTL_FRWID_DEFAULT                      |
                                              ((object->moduleRole << 1)   & MCASP_RXFMCTL_FSRM) |
                                              ((object->invertWS   << 0)   & MCASP_RXFMCTL_FSRP));
    }

    /* Must be executed including in read-only mode */
    HWREG(I2S_BASE + MCASP_O_TXFMCTL) =  (  MCASP_TXFMCTL_XMOD_DEFAULT                       |
                                            MCASP_TXFMCTL_FXWID_DEFAULT                      |
                                          ((object->moduleRole << 1)   & MCASP_TXFMCTL_FSXM) |
                                          ((object->invertWS   << 0)   & MCASP_TXFMCTL_FSXP));
}

/*
 *  ======== configSerialFormat ========
 */
static void configSerialFormat(I2S_Handle handle) {

    I2SCC32XX_Object          const *object;
    I2SCC32XX_DataInterface   const *SD0;
    I2SCC32XX_DataInterface   const *SD1;

    uint8_t channelsToActivate;

    /* Get the pointer to the SD0 and SD1 interfaces*/
    object  = handle->object;
    SD0     = &object->dataInterfaceSD0;
    SD1     = &object->dataInterfaceSD1;

    /* Expected values (intermediate values are also possible):
     * dataLength = 8     >>   dataLengthCC32XX = 0x3
     * dataLength = 16    >>   dataLengthCC32XX = 0x7
     * dataLength = 24    >>   dataLengthCC32XX = 0xB
     * dataLength = 32    >>   dataLengthCC32XX = 0xF
     * */
    uint8_t dataLengthCC32XX = 1;
    int i;
    for(i=HWRESOLUTION;i<=32;i+=HWRESOLUTION){
        if(object->dataLength > i) {
            dataLengthCC32XX += HWRESOLUTION/2;
        }
    }

    if(object->noOfInputs != 0) {

        channelsToActivate = ((SD0->interfaceConfig == I2S_SD0_INPUT)? SD0->channelsUsed: 0x00) |
                             ((SD1->interfaceConfig == I2S_SD1_INPUT)? SD1->channelsUsed: 0x00);
        HWREG(I2S_BASE + MCASP_O_RXFMT) = (((object->dataShift        << MCASP_RXFMT_RDATDLY_S) & MCASP_RXFMT_RDATDLY_M)|
                                           ((object->isMSBFirst       << 15                   ) & MCASP_RXFMT_RRVRS )   |
                                           ((dataLengthCC32XX         << MCASP_RXFMT_RSSZ_S   ) & MCASP_RXFMT_RSSZ_M)   |
                                           ((object->isDMAUnused      << 3                    ) & MCASP_RXFMT_RBUSEL)   |
                                           ((object->sampleRotation   << MCASP_RXFMT_RROT_S   ) & MCASP_RXFMT_RROT_M));
        HWREG(I2S_BASE + MCASP_O_RXMASK) = object->sampleMask;
        HWREG(I2S_BASE + MCASP_O_RXTDM) = channelsToActivate;
    }

    /* Must be executed including in read-only mode */
    channelsToActivate = ((SD0->interfaceConfig == I2S_SD0_OUTPUT)? SD0->channelsUsed: 0x00) |
                         ((SD1->interfaceConfig == I2S_SD1_OUTPUT)? SD1->channelsUsed: 0x00);
    HWREG(I2S_BASE + MCASP_O_TXFMT) = (((object->dataShift        << MCASP_TXFMT_XDATDLY_S) & MCASP_TXFMT_XDATDLY_M)|
                                       ((object->isMSBFirst       << 15                   ) & MCASP_TXFMT_XRVRS )   |
                                       ((dataLengthCC32XX         << MCASP_TXFMT_XSSZ_S   ) & MCASP_TXFMT_XSSZ_M)   |
                                       ((object->isDMAUnused      << 3                    ) & MCASP_TXFMT_XBUSEL)   |
                                       ((object->sampleRotation   << MCASP_TXFMT_XROT_S   ) & MCASP_TXFMT_XROT_M));
    HWREG(I2S_BASE + MCASP_O_TXMASK) = object->sampleMask;
    HWREG(I2S_BASE + MCASP_O_TXTDM) = channelsToActivate;
}

/*
 *  ======== postNotify ========
 *  This functions is called to notify the I2S driver of an ongoing transition
 *  out of LPDS mode.
 *  clientArg should be pointing to a hardware module which has already
 *  been opened.
 */
static int postNotify(unsigned int eventType, uintptr_t eventArg, uintptr_t clientArg) {

    /* Reconfigure the hardware when returning from LPDS */
    initHw((I2S_Handle)clientArg);

    return (Power_NOTIFYDONE);
}
