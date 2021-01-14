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

#include <stdint.h>
#include <stdbool.h>
/*
 * By default disable both asserts and log for this module.
 * This must be done before DebugP.h is included.
 */
#ifndef DebugP_ASSERT_ENABLED
#define DebugP_ASSERT_ENABLED 0
#endif
#ifndef DebugP_LOG_ENABLED
#define DebugP_LOG_ENABLED 0
#endif

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC32XX.h>
#include <ti/drivers/sd/SDHostCC32XX.h>
#include <ti/drivers/dma/UDMACC32XX.h>

/* Driverlib header files */
#include <ti/devices/cc32xx/inc/hw_common_reg.h>
#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/inc/hw_ocp_shared.h>
#include <ti/devices/cc32xx/inc/hw_ints.h>
#include <ti/devices/cc32xx/inc/hw_mmchs.h>
#include <ti/devices/cc32xx/inc/hw_udma.h>
#include <ti/devices/cc32xx/inc/hw_apps_rcm.h>
#include <ti/devices/cc32xx/driverlib/rom.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>
#include <ti/devices/cc32xx/driverlib/pin.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>
#include <ti/devices/cc32xx/driverlib/sdhost.h>
#include <ti/devices/cc32xx/driverlib/udma.h>

#define PinConfigPinMode(config)       (((config) >> 8) & 0xF)
#define PinConfigPin(config)           (((config) >> 0) & 0x3F)

#define PAD_CONFIG_BASE (OCP_SHARED_BASE + OCP_SHARED_O_GPIO_PAD_CONFIG_0)
#define PAD_RESET_STATE 0xC61

/* Definitions for SDC driverlib commands  */
#define CMD_GO_IDLE_STATE    (SDHOST_CMD_0)
#define CMD_SEND_OP_COND     (SDHOST_CMD_1 | SDHOST_RESP_LEN_48)
#define CMD_ALL_SEND_CID     (SDHOST_CMD_2 | SDHOST_RESP_LEN_136)
#define CMD_SEND_REL_ADDR    (SDHOST_CMD_3 | SDHOST_RESP_LEN_48)
#define CMD_SELECT_CARD      (SDHOST_CMD_7 | SDHOST_RESP_LEN_48B)
#define CMD_DESELECT_CARD    (SDHOST_CMD_7)
#define CMD_SEND_IF_COND     (SDHOST_CMD_8 | SDHOST_RESP_LEN_48)
#define CMD_SEND_CSD         (SDHOST_CMD_9 | SDHOST_RESP_LEN_136)
#define CMD_STOP_TRANS       (SDHOST_CMD_12 | SDHOST_RESP_LEN_48B)
#define CMD_READ_SINGLE_BLK  (SDHOST_CMD_17 | SDHOST_RD_CMD | SDHOST_RESP_LEN_48)
#define CMD_READ_MULTI_BLK   (SDHOST_CMD_18 | SDHOST_RD_CMD | \
                              SDHOST_RESP_LEN_48 | SDHOST_MULTI_BLK)
#define CMD_SET_BLK_CNT      (SDHOST_CMD_23 | SDHOST_RESP_LEN_48)
#define CMD_WRITE_SINGLE_BLK (SDHOST_CMD_24 | SDHOST_WR_CMD | SDHOST_RESP_LEN_48)
#define CMD_WRITE_MULTI_BLK  (SDHOST_CMD_25 | SDHOST_WR_CMD | \
                              SDHOST_RESP_LEN_48 | SDHOST_MULTI_BLK)
#define CMD_SD_SEND_OP_COND  (SDHOST_CMD_41 | SDHOST_RESP_LEN_48)
#define CMD_APP_CMD          (SDHOST_CMD_55 | SDHOST_RESP_LEN_48)

/* Group of all possible SD command and data error flags */
#define CMDERROR             (SDHOST_INT_CTO | SDHOST_INT_CEB)

#define DATAERROR            (SDHOST_INT_DTO | SDHOST_INT_DCRC | \
                              SDHOST_INT_DEB | SDHOST_INT_CERR | \
                              SDHOST_INT_BADA)

#define SECTORSIZE           (512)

/* PLL CLK DIV value to get divider of 8 */
#define PLL_CLK_DIV8         (0x7)

/* SD initialization stream bit */
#define SD_INIT_STREAM       (0x2)

/* SD initialization dummy cmd/arg */
#define DUMMY                (0x00000000)

/* SD initialization frequency of 80KHz */
#define SD_INIT_FREQ_80KHZ   (80000)

/* SD identification frequency of 400KHz */
#define SD_ID_FREQ_400KHZ    (400000)

/* Voltage window (VDD) used on this device (3.3-3.6 V) */
#define VOLTAGEWINDOW        (0x00E00000)

/* Bit set to indicate the host controller has high capacity support */
#define HIGHCAPSUPPORT       (0x40000000)

/* Voltage supplied 2.7-3.6V */
#define SUPPLYVOLTAGE        (0x00000100)

/* Checksum to validate SD command responses */
#define CHECKSUM             (0xA5)

/* Setup the UDMA controller to either read or write to the SDHost buffer */
#define UDMAWRITE            (0x01)

#define UDMAREAD             (0x00)

/* Define used for commands that do not require an argument */
#define NULLARG              (0x00)

/* SDHostCC32XX configuration - initialized in the board file */
extern const SD_Config SD_config[];

/* SDHostCC32XX functions */
void SDHostCC32XX_close(SD_Handle handle);
int_fast16_t SDHostCC32XX_control(SD_Handle handle, uint_fast16_t cmd,
    void *arg);
uint_fast32_t SDHostCC32XX_getNumSectors(SD_Handle handle);
uint_fast32_t SDHostCC32XX_getSectorSize(SD_Handle handle);
int_fast16_t SDHostCC32XX_initialize(SD_Handle handle);
void SDHostCC32XX_init(SD_Handle handle);
SD_Handle SDHostCC32XX_open(SD_Handle handle, SD_Params *params);
int_fast16_t SDHostCC32XX_read(SD_Handle handle, void *buf,
    int_fast32_t sector, uint_fast32_t secCount);
int_fast16_t SDHostCC32XX_write(SD_Handle handle, const void *buf,
    int_fast32_t sector, uint_fast32_t secCount);

/* Local Functions */
static inline void configDMAChannel(SD_Handle handle, uint_fast32_t channelSel,
    uint_fast32_t operation);
static int_fast32_t deSelectCard(SD_Handle handle);
static uint_fast32_t getPowerMgrId(uint_fast32_t baseAddr);
static void hwiIntFxn(uintptr_t handle);
static void initHw(SD_Handle handle);
static int postNotifyFxn(unsigned int eventType, uintptr_t eventArg,
    uintptr_t clientArg);
static int_fast32_t send_cmd(SD_Handle handle, uint_fast32_t cmd,
    uint_fast32_t arg);
static int_fast32_t selectCard(SD_Handle handle);

/* SDHostCC32XX function table for SDSPICC32XX implementation */
const SD_FxnTable sdHostCC32XX_fxnTable = {
    SDHostCC32XX_close,
    SDHostCC32XX_control,
    SDHostCC32XX_getNumSectors,
    SDHostCC32XX_getSectorSize,
    SDHostCC32XX_init,
    SDHostCC32XX_initialize,
    SDHostCC32XX_open,
    SDHostCC32XX_read,
    SDHostCC32XX_write
};

/*
 *  ======== SDHostCC32XX_close ========
 */
void SDHostCC32XX_close(SD_Handle handle)
{
    SDHostCC32XX_Object          *object = handle->object;
    SDHostCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;
    uint32_t                      padRegister;

    if (object->cardType != SD_NOCARD) {
        /* De-select the SD Card, move back to standby state */
        if (deSelectCard(handle) != SD_STATUS_SUCCESS) {
            DebugP_log1("SDHost:(%p) Failed to de-select SD Card",
                hwAttrs->baseAddr);
        }
    }

    /* Disable SD Host interrupts */
    MAP_SDHostIntDisable(hwAttrs->baseAddr, DATAERROR | CMDERROR);

    if (object->dmaHandle) {
        UDMACC32XX_close(object->dmaHandle);
    }
    if (object->cmdSem) {
        SemaphoreP_delete(object->cmdSem);
    }
    if (object->hwiHandle) {
        HwiP_delete(object->hwiHandle);
    }

    /* Remove Power driver settings */
    if (object->clkPin != (uint16_t)-1) {
        PowerCC32XX_restoreParkState((PowerCC32XX_Pin)object->clkPin,
            object->prevParkCLK);
        object->clkPin = (uint16_t)-1;
    }
    Power_unregisterNotify(&object->postNotify);
    Power_releaseDependency(object->powerMgrId);
    Power_releaseConstraint(PowerCC32XX_DISALLOW_LPDS);

    /* Restore pin pads to their reset states */
    padRegister = (PinToPadGet((hwAttrs->dataPin) & 0x3f)<<2) + PAD_CONFIG_BASE;
    HWREG(padRegister) = PAD_RESET_STATE;
    padRegister = (PinToPadGet((hwAttrs->cmdPin) & 0x3f)<<2) + PAD_CONFIG_BASE;
    HWREG(padRegister) = PAD_RESET_STATE;
    padRegister = (PinToPadGet((hwAttrs->clkPin) & 0x3f)<<2) + PAD_CONFIG_BASE;
    HWREG(padRegister) = PAD_RESET_STATE;

    object->isOpen = false;

    DebugP_log1("SDHost:(%p) closed and released power dependency",
        hwAttrs->baseAddr);
}

/*
 *  ======== SDHostCC32XX_control ========
 *  @pre    Function assumes that the handle is not NULL.
 */
int_fast16_t SDHostCC32XX_control(SD_Handle handle, uint_fast32_t cmd,
    void *arg)
{
    /* No implementation yet */
    return (SD_STATUS_UNDEFINEDCMD);
}

/*
 *  ======== SDHostCC32XX_getNumSectors ========
 *  Function to read the CSD register of the SD card on the drive specified
 *  by the SD_Handle and calculate the total card capacity in sectors.
 */
uint_fast32_t SDHostCC32XX_getNumSectors(SD_Handle handle)
{
    uint_fast32_t                 sectors = 0;
    uint_fast32_t                 cSize; /* Device size */
    uint_fast32_t                 blockSize; /* Read block length */
    uint_fast32_t                 sizeMult;
    uint_fast32_t                 resp[4];
    SDHostCC32XX_Object          *object = handle->object;
    SDHostCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    /* De-Select the card on the input drive(Stand-by state) */
    if (deSelectCard(handle) != SD_STATUS_SUCCESS) {
        return (0);
    }

    if (send_cmd(handle, CMD_SEND_CSD, (object->rca << 16)) ==
        SD_STATUS_SUCCESS) {

        MAP_SDHostRespGet(hwAttrs->baseAddr, (unsigned long *)resp);

       /*
        * 136 bit CSD register is read into an array of 4 words
        * Note: Does not include 8 bit CRC
        * resp[0] = CSD[31:0]
        * resp[1] = CSD[63:32]
        * resp[2] = CSD[95:64]
        * resp[3] = CSD[127:96]
        */
        if(((resp[3] >> 30) & 0x01) == 1) {
            sectors = (resp[1] >> 16 | ((resp[2] & 0x3F) << 16)) + 1;
            sectors *= 1024;
        }
        else {
            blockSize = 1 << ((resp[2] >> 16) & 0x0F);
            sizeMult = ((resp[1] >> 15) & 0x07);
            cSize = ((resp[1] >> 30) | (resp[2] & 0x3FF) << 2);
            sectors = (cSize + 1) * (1 << (sizeMult + 2));
            sectors = (sectors * blockSize) / SECTORSIZE;

        }
    }

    /* Select the card on the input drive(Transfer state) */
    if (selectCard(handle) != SD_STATUS_SUCCESS) {
        sectors = 0;
    }

    return (sectors);
}

/*
 *  ======== SDHostCC32XX_getSectorSize ========
 *  Function to perform a disk read from the SD Card on the specifed drive.
 */
uint_fast32_t SDHostCC32XX_getSectorSize(SD_Handle handle)
{
    return (SECTORSIZE);
}

/*
 *  ======== SDHostCC32XX_initialize ========
 *  Function to initialize the SD Card.
 */
int_fast16_t SDHostCC32XX_initialize(SD_Handle handle)
{
    int_fast32_t                  result;
    uint_fast32_t                 resp[4];
    uint8_t                       i;
    SDHostCC32XX_Object          *object = handle->object;
    SDHostCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    /*
     * Send go to IDLE command. This may take multiple tries depending
     * on the previous state of the SD card. Try up to 255 attempts.
     */
    for(i = 0; i < 255; i++)
    {
        result = send_cmd(handle, CMD_GO_IDLE_STATE, NULLARG);
        if (result == SD_STATUS_SUCCESS)
        {
            break;
        }
    }

    if (result == SD_STATUS_SUCCESS) {
        /* Get interface operating condition for the card */
        result = send_cmd(handle, CMD_SEND_IF_COND,
            SUPPLYVOLTAGE | CHECKSUM);
        /* Verify response will be valid */
        if (result == SD_STATUS_SUCCESS) {
            MAP_SDHostRespGet(hwAttrs->baseAddr, (unsigned long *)resp);
        }
    }

    /* SD ver 2.0 or higher card */
    if ((result == SD_STATUS_SUCCESS) && ((resp[0] & 0xFF) ==
        CHECKSUM)) {
        object->cardType = SD_SDSC;

        /* Wait for card to be ready */
        do {
            /* Send ACMD41 */
            result = send_cmd(handle, CMD_APP_CMD, NULLARG);

            if (result == SD_STATUS_SUCCESS) {
                result = send_cmd(handle, CMD_SD_SEND_OP_COND,
                    HIGHCAPSUPPORT | VOLTAGEWINDOW);
                /* Response contains 32-bit OCR register */
                MAP_SDHostRespGet(hwAttrs->baseAddr, (unsigned long *)resp);
            }

        /* Wait until card is ready, spool on busy bit */
        } while((result == SD_STATUS_SUCCESS) && ((resp[0] >> 31) == 0));

        /* Card capacity status bit */
        if ((result == SD_STATUS_SUCCESS) && (resp[0] & (1UL << 30))) {
            object->cardType = SD_SDHC;
        }
    }
    /* It's a MMC or SD 1.x card */
    else {
        /* Wait for card to be ready */
        do {
            /* Send ACMD41 */
            result = send_cmd(handle, CMD_APP_CMD, NULLARG);

            if (result == SD_STATUS_SUCCESS) {
                result = send_cmd(handle, CMD_SD_SEND_OP_COND, VOLTAGEWINDOW);
            }

            if (result == SD_STATUS_SUCCESS) {
                /* Response contains 32-bit OCR register */
                MAP_SDHostRespGet(hwAttrs->baseAddr, (unsigned long *)resp);
            }

        /* Wait until card is ready, spool on busy bit */
        } while((result == SD_STATUS_SUCCESS) && ((resp[0] >> 31) == 0));

        if (result == SD_STATUS_SUCCESS) {
            object->cardType = SD_SDSC;
        }
        else if (send_cmd(handle, CMD_SEND_OP_COND, NULLARG) ==
            SD_STATUS_SUCCESS) {
            object->cardType = SD_MMC;
        }
        /* No command responses */
        else {
            object->cardType = SD_NOCARD;
        }
    }

    /* Get the relative card address (RCA) of the attached card */
    if (object->cardType != SD_NOCARD) {
        result = send_cmd(handle, CMD_ALL_SEND_CID, NULLARG);

        if (result == SD_STATUS_SUCCESS) {
            result = send_cmd(handle, CMD_SEND_REL_ADDR, NULLARG);
        }

        if (result == SD_STATUS_SUCCESS) {
            /* Fill in the RCA */
            MAP_SDHostRespGet(hwAttrs->baseAddr, (unsigned long *)resp);

            object->rca = resp[0] >> 16;

            /* Select the card on the input drive(Transfer state) */
            result = selectCard(handle);
            if (result == SD_STATUS_SUCCESS) {
                /* Set card read/write block length */
                MAP_SDHostBlockSizeSet(hwAttrs->baseAddr, SECTORSIZE);

                /* Initialization succeeded */
                result = SD_STATUS_SUCCESS;
            }
        }
    }
    else {
        DebugP_log1("SDHost:(%p) Could not select card",
            hwAttrs->baseAddr);
        result = SD_STATUS_ERROR;
    }

    /*
     * Reconfigure clock frequency after card initialization and
     * identification process has been completed successfully at 400KHz.
     */
    if (result == SD_STATUS_SUCCESS)
    {
        /* Configure card clock */
        MAP_SDHostSetExpClk(hwAttrs->baseAddr,
            MAP_PRCMPeripheralClockGet(PRCM_SDHOST), hwAttrs->clkRate);
    }

    return (result);
}

/*
 *  ======== SDHostCC32XX_init ========
 *  Function to initialize the SDHost module
 */
void SDHostCC32XX_init(SD_Handle handle)
{
    SDHostCC32XX_Object *object = handle->object;

    object->buffer = NULL;
    object->sectorCount = 0;
    object->cardType = SD_NOCARD;
    object->isOpen = false;

    UDMACC32XX_init();
}

/*
 *  ======== SDHostCC32XX_open ========
 */
SD_Handle SDHostCC32XX_open(SD_Handle handle, SD_Params *params)
{
    uintptr_t                     key;
    SemaphoreP_Params             semParams;
    HwiP_Params                   hwiParams;
    SDHostCC32XX_Object          *object = handle->object;
    SDHostCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;
    uint16_t pin;

    key = HwiP_disable();

    if (object->isOpen) {
        HwiP_restore(key);

        DebugP_log1("SDHost:(%p) already in use.", hwAttrs->baseAddr);
        return (NULL);
    }
    object->isOpen = true;

    HwiP_restore(key);

    /* Initialize the SDCARD_CLK pin ID to undefined */
    object->clkPin = (uint16_t)-1;

    /* Get the Power resource Id from the base address */
    object->powerMgrId = getPowerMgrId(hwAttrs->baseAddr);
    if (object->powerMgrId == (unsigned int)-1) {
        DebugP_log1("SDHost:(%p) Failed to determine Power resource id",
            hwAttrs->baseAddr);
        return (NULL);
    }

    /*
     *  Register power dependency. Keeps the clock running in SLP
     *  and DSLP modes.
     */
    Power_setDependency(object->powerMgrId);

    Power_registerNotify(&object->postNotify, PowerCC32XX_AWAKE_LPDS,
        postNotifyFxn, (uintptr_t)handle);

    Power_setConstraint(PowerCC32XX_DISALLOW_LPDS);

    object->dmaHandle = UDMACC32XX_open();
    if (object->dmaHandle == NULL) {
        DebugP_log1("SDHost:(%p) UDMACC32XX_open() failed.",
            hwAttrs->baseAddr);
        SDHostCC32XX_close(handle);
        return (NULL);
    }

    SemaphoreP_Params_init(&semParams);
    semParams.mode = SemaphoreP_Mode_COUNTING;
    object->cmdSem = SemaphoreP_create(0, &semParams);

    if (object->cmdSem == NULL) {
        DebugP_log1("SDHost:(%p) SemaphoreP_create() failed.",
            hwAttrs->baseAddr);
        SDHostCC32XX_close(handle);
        return (NULL);
    }

    HwiP_Params_init(&hwiParams);
    hwiParams.arg = (uintptr_t)handle;
    hwiParams.priority = hwAttrs->intPriority;
    object->hwiHandle = HwiP_create(INT_MMCHS, hwiIntFxn,
        &hwiParams);
    if (object->hwiHandle == NULL) {
        DebugP_log1("SDHostT:(%p) HwiP_create() failed", hwAttrs->baseAddr);
        SDHostCC32XX_close(handle);
        return (NULL);
    }

    /* Initialize the hardware */
    initHw(handle);

    /* Save clkPin park state; set to logic '0' during LPDS */
    pin = PinConfigPin(hwAttrs->clkPin);
    object->prevParkCLK =
        (PowerCC32XX_ParkState) PowerCC32XX_getParkState((PowerCC32XX_Pin)pin);
    PowerCC32XX_setParkState((PowerCC32XX_Pin)pin, 0);
    object->clkPin = pin;

    DebugP_log1("SDHost:(%p) opened", hwAttrs->baseAddr);

    return (handle);
}

/*
 *  ======== SDHostCC32XX_read ========
 */
int_fast16_t SDHostCC32XX_read(SD_Handle handle, void *buf,
    int_fast32_t sector, uint_fast32_t secCount)
{
    int_fast32_t                  result;
    uint_fast32_t                 ret;
    uint_fast32_t                 size;
    SDHostCC32XX_Object          *object  = handle->object;
    SDHostCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    object->stat = SD_STATUS_SUCCESS;

    /* Check for valid sector count */
    if (secCount == 0) {
        return (SD_STATUS_ERROR);
    }

    /* SDSC uses linear address, SDHC uses block address */
    if (object->cardType == SD_SDSC) {
        sector = sector * SECTORSIZE;
    }

    /* Set the block count */
    MAP_SDHostBlockCountSet(hwAttrs->baseAddr, secCount);

    /* If input buffer is word aligned use DMA */
    if (!(((uint32_t)buf) & 0x03)) {
        object->buffer = (uint32_t *)buf;
        object->sectorCount = secCount;

        /* Configure ping */
        configDMAChannel(handle, UDMA_PRI_SELECT , UDMAREAD);

        /* Add offset to input buffer */
        object->buffer = object->buffer + (SECTORSIZE / 4);

        /* Configure pong */
        if (secCount > 1) {
            configDMAChannel(handle, UDMA_ALT_SELECT , UDMAREAD);
        }

        /*
         * Enable DMA and transmit complete interrupts, reset ping and
         * dmaPosted flags.
         */
        object->dmaPosted = false;
        object->ping = false;
        MAP_SDHostIntEnable(hwAttrs->baseAddr, SDHOST_INT_DMARD | SDHOST_INT_TC);

        /* Send multi block read command to the SD card */
        result = send_cmd(handle, CMD_READ_MULTI_BLK | SDHOST_DMA_EN, sector);

        /* Wait for DMA read(s) to complete */
        if (result == SD_STATUS_SUCCESS) {
            /* Any remaining DMA transfers are completed in the hwiFxn */
            SemaphoreP_pend(object->cmdSem, SemaphoreP_WAIT_FOREVER);
        }
    }
    /* Poll buffer for new data */
    else {
        /* Compute the number of words to read */
        size = (SECTORSIZE * secCount) / 4;

        /* Send multi block read command */
        result = send_cmd(handle, CMD_READ_MULTI_BLK, sector);

        if (result == SD_STATUS_SUCCESS) {
            /* Read single word of data */
            while (size > 0) {
                ret = MAP_SDHostDataNonBlockingRead(hwAttrs->baseAddr,
                    (unsigned long *)buf);
                /* Block until buffer is ready */
                if (ret == 0) {
                    MAP_SDHostIntEnable(hwAttrs->baseAddr, SDHOST_INT_BRR);

                    SemaphoreP_pend(object->cmdSem, SemaphoreP_WAIT_FOREVER);

                    MAP_SDHostIntDisable(hwAttrs->baseAddr, SDHOST_INT_BRR);
                }
                else {
                    buf = (uint_least8_t *)buf + 4;
                    size--;
                }
            }
        }
    }

    /* Verify host controller errors didn't occur */
    if (object->stat == SD_STATUS_SUCCESS) {

        MAP_SDHostIntEnable(hwAttrs->baseAddr, SDHOST_INT_TC);

        /* Wait for full data transfer to complete */
        SemaphoreP_pend(object->cmdSem, SemaphoreP_WAIT_FOREVER);

        MAP_SDHostIntDisable(hwAttrs->baseAddr, SDHOST_INT_TC);
    }

    /* Verify host controller errors didn't occur */
    if (object->stat == SD_STATUS_SUCCESS) {

        /* Wait for command transfer stop acknowledgement */
        result = send_cmd(handle, CMD_STOP_TRANS, NULLARG);

        if (result ==  SD_STATUS_SUCCESS) {
            MAP_SDHostIntEnable(hwAttrs->baseAddr, SDHOST_INT_TC);

            SemaphoreP_pend(object->cmdSem, SemaphoreP_WAIT_FOREVER);

            MAP_SDHostIntDisable(hwAttrs->baseAddr, SDHOST_INT_TC);
        }
    }

    if (object->stat != SD_STATUS_SUCCESS) {
        result = SD_STATUS_ERROR;
    }

    return (result);
}

/*
 *  ======== SDHostCC32XX_write ========
 */
int_fast16_t SDHostCC32XX_write(SD_Handle handle, const void *buf,
    int_fast32_t sector, uint_fast32_t secCount)
{
    int_fast32_t                  result;
    uint_fast32_t                 ret;
    uint_fast32_t                 size;
    SDHostCC32XX_Object          *object  = handle->object;
    SDHostCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    object->stat = SD_STATUS_SUCCESS;

    /* Check for valid sector count */
    if (secCount == 0) {
        return (SD_STATUS_ERROR);
    }

    /* SDSC uses linear address, SDHC uses block address */
    if(object->cardType == SD_SDSC) {
        sector = sector * SECTORSIZE;
    }

    /* Set the block count */
    MAP_SDHostBlockCountSet(hwAttrs->baseAddr, secCount);

    /* Set card write block erase count */
    result = send_cmd(handle, CMD_APP_CMD,  object->rca << 16);
    if (result != SD_STATUS_SUCCESS) {
        return (SD_STATUS_ERROR);
    }
    result = send_cmd(handle, CMD_SET_BLK_CNT, secCount);
    if (result != SD_STATUS_SUCCESS) {
        return (SD_STATUS_ERROR);
    }

    /* If the write buffer is word aligned use the DMA */
    if (!(((uint_fast32_t)buf) & 0x03)) {
        object->buffer = (uint32_t *)buf;
        object->sectorCount = secCount;

        /* Configure ping */
        configDMAChannel(handle, UDMA_PRI_SELECT, UDMAWRITE);

        /* Add offset to input buffer */
        object->buffer = object->buffer + (SECTORSIZE / 4);

        /* Configure pong */
        if (secCount > 1) {
            configDMAChannel(handle, UDMA_ALT_SELECT, UDMAWRITE);
        }

        /*
         * Enable DMA and transmit complete interrupts, reset ping and
         * dmaPosted flags.
         */
        object->dmaPosted = false;
        object->ping = false;
        MAP_SDHostIntEnable(hwAttrs->baseAddr, SDHOST_INT_DMAWR | SDHOST_INT_TC);

        /* Send write multi block command to the SD card */
        result = send_cmd(handle, CMD_WRITE_MULTI_BLK | SDHOST_DMA_EN, sector);

        /* Wait for the DMA to finish */
        if (result == SD_STATUS_SUCCESS) {
            /* Any remaining DMA transfers are completed in the hwiFxn */
            SemaphoreP_pend(object->cmdSem, SemaphoreP_WAIT_FOREVER);
        }
    }
    else {
        /* Compute the number of words to write */
        size = (SECTORSIZE * secCount) / 4;

        /* Verify that the block count had been previous set */
        if (result == SD_STATUS_SUCCESS) {
            result = send_cmd(handle, CMD_WRITE_MULTI_BLK, sector);
        }

        if (result == SD_STATUS_SUCCESS) {
            /* Write single word of data */
            while (size > 0) {
                ret = MAP_SDHostDataNonBlockingWrite(hwAttrs->baseAddr,
                    (*(unsigned long *)buf));

                /* Block until buffer is ready */
                if (ret == 0) {
                    MAP_SDHostIntEnable(hwAttrs->baseAddr, SDHOST_INT_BWR);

                    SemaphoreP_pend(object->cmdSem, SemaphoreP_WAIT_FOREVER);

                    MAP_SDHostIntDisable(hwAttrs->baseAddr, SDHOST_INT_BWR);

                }
                else {
                    buf = (uint_least8_t *)buf + 4;
                    size--;
                }
            }
        }
    }

    /* Verify host controller errors didn't occur */
    if (object->stat == SD_STATUS_SUCCESS) {
        /* Wait for the full data transfer to complete */
        MAP_SDHostIntEnable(hwAttrs->baseAddr, SDHOST_INT_TC);

        SemaphoreP_pend(object->cmdSem, SemaphoreP_WAIT_FOREVER);

        MAP_SDHostIntDisable(hwAttrs->baseAddr, SDHOST_INT_TC);
    }

    /* Verify host controller errors didn't occur */
    if (object->stat == SD_STATUS_SUCCESS) {
        /* Wait for command transfer stop acknowledgment */
        result = send_cmd(handle, CMD_STOP_TRANS, NULLARG);

        if (result ==  SD_STATUS_SUCCESS) {
            MAP_SDHostIntEnable(hwAttrs->baseAddr, SDHOST_INT_TC);

            SemaphoreP_pend(object->cmdSem, SemaphoreP_WAIT_FOREVER);

            MAP_SDHostIntDisable(hwAttrs->baseAddr, SDHOST_INT_TC);
        }
    }

    if (object->stat != SD_STATUS_SUCCESS) {
        result = SD_STATUS_ERROR;
    }

    return (result);
}

/*
 *  ======== configDMAChannel ========
 *  Configures either the primary or alternate DMA control structures in
 *  ping-pong mode.
 *  channelSel is the PRI or ALT channel select flag
 */
static inline void configDMAChannel(SD_Handle handle, uint_fast32_t channelSel,
    uint_fast32_t operation)
{
    unsigned long                 channelControlOptions;
    SDHostCC32XX_Object          *object = handle->object;
    SDHostCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    if (operation == UDMAWRITE) {
        channelControlOptions = UDMA_SIZE_32 | UDMA_SRC_INC_32 |
            UDMA_DST_INC_NONE | UDMA_ARB_512;

        /* Primary control structure set-up */
        MAP_uDMAChannelControlSet(hwAttrs->txChIdx |
            channelSel, channelControlOptions);

        /* Transfer size is the sector size in words */
        MAP_uDMAChannelTransferSet(hwAttrs->txChIdx | channelSel,
            UDMA_MODE_PINGPONG, (void *)object->buffer,
            (void *)(hwAttrs->baseAddr + MMCHS_O_DATA), SECTORSIZE / 4);
    }
    else {
        channelControlOptions = UDMA_SIZE_32 | UDMA_SRC_INC_NONE |
                UDMA_DST_INC_32 | UDMA_ARB_512;

        /* Primary control structure set-up */
        MAP_uDMAChannelControlSet(hwAttrs->rxChIdx |
            channelSel, channelControlOptions);

        /* Transfer size is the sector size in words */
        MAP_uDMAChannelTransferSet(hwAttrs->rxChIdx | channelSel,
            UDMA_MODE_PINGPONG, (void *)(hwAttrs->baseAddr + MMCHS_O_DATA),
            (void *)object->buffer, SECTORSIZE / 4);
    }
}

/*
 *  ======== deSelectCard ========
 *  Function to de-select a card on the drive specified by the handle.
 */
static int_fast32_t deSelectCard(SD_Handle handle)
{
    int_fast32_t result;

    /* De-select the card */
    result = send_cmd(handle, CMD_DESELECT_CARD, NULLARG);

    return (result);
}

/*
 *  ======== getPowerMgrId ========
 */
static uint_fast32_t getPowerMgrId(uint_fast32_t baseAddr)
{
    if (baseAddr == SDHOST_BASE) {
        return (PowerCC32XX_PERIPH_SDHOST);
    }
    else {
        return ((uint_fast32_t)-1);
    }
}

/*
 *  ======== hwiIntFxn ========
 *  ISR to service pending SD or DMA commands.
 */
static void hwiIntFxn(uintptr_t handle)
{
    uint_fast32_t                 ret;
    SDHostCC32XX_Object          *object = ((SD_Handle)handle)->object;
    SDHostCC32XX_HWAttrsV1 const *hwAttrs = ((SD_Handle)handle)->hwAttrs;

    /* Get interrupt and clear all interrupt flags */
    ret = MAP_SDHostIntStatus(hwAttrs->baseAddr);
    MAP_SDHostIntClear(hwAttrs->baseAddr, ret);

    /* Error or unsupported interrupt occurred */
    if (ret & SDHOST_INT_ERRI) {
        object->stat = SD_STATUS_ERROR;
        SemaphoreP_post(object->cmdSem);
        return;
    }

    /* Command complete flag */
    if (ret & SDHOST_INT_CC) {
        if (object->stat != SD_STATUS_ERROR) {
            object->stat = SD_STATUS_SUCCESS;
        }
        SemaphoreP_post(object->cmdSem);
    }

    /*
     * DMA read and write interrupt flags. This interrupt is
     * generated when a DMA channel finishes a transfer AND when
     * the SD peripheral empties it's TX or RX buffer. Both events
     * use the DMA WRITE or DMA READ interrupt flags respectively.
     */
    if (ret & (SDHOST_INT_DMARD | SDHOST_INT_DMAWR)) {
        uint_fast32_t priChannelMode;
        uint_fast32_t altChannelMode;
        uint32_t channel = hwAttrs->txChIdx;
        uint32_t operation = UDMAWRITE;

        if (ret & SDHOST_INT_DMARD) {
            channel = hwAttrs->rxChIdx;
            operation = UDMAREAD;
        }

        priChannelMode = MAP_uDMAChannelModeGet(channel | UDMA_PRI_SELECT);
        altChannelMode = MAP_uDMAChannelModeGet(channel | UDMA_ALT_SELECT);

        /*
         * Differentiate between the SD peripheral emptying a buffer
         * and a DMA channel completing a transfer. If neither DMA
         * channel is stopped, then we wait for the next interrupt.
         */
        if (priChannelMode == UDMA_MODE_STOP ||
            altChannelMode == UDMA_MODE_STOP) {

            object->ping = !object->ping;
            if (object->sectorCount != 0) {
                object->sectorCount--;
            }

            /*
             * Check DMA transfers are complete,
             * otherwise wait for the next interrupt
             */
            if (priChannelMode == UDMA_MODE_STOP &&
                altChannelMode == UDMA_MODE_STOP &&
                object->sectorCount != 0) {

                /* Determine if multiple DMA transfers completed */
                if (object->ping) {
                    object->sectorCount--;
                    object->ping = false;
                }

                /*
                 * If sectorCount is 0, then we do not need to configure
                 * any more DMA transfers.
                 */
                if(object->sectorCount != 0) {
                    /* Configure the primary DMA channel */
                    object->buffer = object->buffer + (SECTORSIZE / 4);
                    configDMAChannel((SD_Handle) handle, UDMA_PRI_SELECT, operation);

                    /* If more than 1 sector is left, configure pong */
                    if (object->sectorCount > 1) {
                        /* Configure the alternate DMA channel */
                        object->buffer = object->buffer + (SECTORSIZE / 4);
                        configDMAChannel((SD_Handle) handle, UDMA_ALT_SELECT, operation);
                    }
                }
            }

            /* If the DMA has transfered all sectors to the SD peripheral */
            if (object->sectorCount == 0 && object->dmaPosted == false &&
                priChannelMode == UDMA_MODE_STOP &&
                altChannelMode == UDMA_MODE_STOP) {

                if (object->stat != SD_STATUS_ERROR) {
                    object->stat = SD_STATUS_SUCCESS;
                }

                /* Reset to primary channel and disable all DMA interrupts */
                MAP_uDMAChannelAttributeDisable(channel, UDMA_ATTR_ALTSELECT);
                MAP_SDHostIntDisable(hwAttrs->baseAddr, SDHOST_INT_DMAWR | SDHOST_INT_DMARD);
                MAP_SDHostIntClear(hwAttrs->baseAddr, SDHOST_INT_DMAWR | SDHOST_INT_DMARD);
                object->dmaPosted = true;
                SemaphoreP_post(object->cmdSem);
            }
        }
    }

    /* Transfer complete flag */
    if (ret & SDHOST_INT_TC) {
        if (object->stat != SD_STATUS_ERROR) {
            object->stat = SD_STATUS_SUCCESS;
        }
        SemaphoreP_post(object->cmdSem);
    }

    /* Data buffer read ready flag */
    if (ret & SDHOST_INT_BRR) {
        if (object->stat != SD_STATUS_ERROR) {
            object->stat = SD_STATUS_SUCCESS;
        }
        SemaphoreP_post(object->cmdSem);
    }

    /* Data buffer write ready flag */
    if (ret & SDHOST_INT_BWR) {
        if (object->stat != SD_STATUS_ERROR) {
            object->stat = SD_STATUS_SUCCESS;
        }
        SemaphoreP_post(object->cmdSem);
    }
}

/*
 *  ======== initHw ========
 */
static void initHw(SD_Handle handle)
{
    SDHostCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;
    uint32_t pin;
    uint32_t mode;
    uint32_t clockcfg;

    /* Configure for SDHost Data */
    pin = PinConfigPin(hwAttrs->dataPin);
    mode = PinConfigPinMode(hwAttrs->dataPin);
    MAP_PinTypeSDHost(pin, mode);
    MAP_PinConfigSet(pin, PIN_STRENGTH_4MA, PIN_TYPE_STD_PU);

    /* Configure for SDHost Command */
    pin = PinConfigPin(hwAttrs->cmdPin);
    mode = PinConfigPinMode(hwAttrs->cmdPin);
    MAP_PinTypeSDHost(pin, mode);
    MAP_PinConfigSet(pin, PIN_STRENGTH_4MA, PIN_TYPE_STD_PU);

    /* Configure for SDHost clock output */
    pin = PinConfigPin(hwAttrs->clkPin);
    mode = PinConfigPinMode(hwAttrs->clkPin);
    MAP_PinTypeSDHost(pin, mode);
    MAP_PinDirModeSet(pin, PIN_DIR_MODE_OUT);

    MAP_PRCMPeripheralReset(PRCM_SDHOST);
    MAP_SDHostInit(hwAttrs->baseAddr);

    MAP_SDHostIntDisable(hwAttrs->baseAddr, SDHOST_INT_CC | SDHOST_INT_TC |
        SDHOST_INT_DMAWR | SDHOST_INT_DMARD | SDHOST_INT_BRR | SDHOST_INT_BWR);

    /* Store current clock configuration */
    clockcfg = HWREG(ARCM_BASE + APPS_RCM_O_MMCHS_CLK_GEN);
    /*
     * Set PLL CLK DIV to 8 in order to get 80KHz clock freuqnecy. The
     * requirement to wake up the card with the initialization stream
     * is 80 clock cycles in 1ms.
     */
    HWREG(ARCM_BASE + APPS_RCM_O_MMCHS_CLK_GEN) |= PLL_CLK_DIV8;
    /* Set clock frequency to 80KHz for initialization stream */
    MAP_SDHostSetExpClk(hwAttrs->baseAddr,
            MAP_PRCMPeripheralClockGet(PRCM_SDHOST), SD_INIT_FREQ_80KHZ);
    /* Enable SD initialization stream */
    HWREG(hwAttrs->baseAddr + MMCHS_O_CON) |= SD_INIT_STREAM;
    /* Dummy command to send out the 80 clock cycles */
    send_cmd(handle, DUMMY, DUMMY);
    /* End SD initialization stream */
    HWREG(hwAttrs->baseAddr + MMCHS_O_CON) &= ~SD_INIT_STREAM;
     /* Reset clock cfg to initial cfg */
    HWREG(ARCM_BASE + APPS_RCM_O_MMCHS_CLK_GEN) = clockcfg;

    /*
     * Configure the card clock to 400KHz for the card initialization and
     * idenitification process. This is done to ensure compatibility with
     * older SD cards. Once this is complete, the card clock will be
     * reconfigured to the set operating value.
     */
    MAP_SDHostSetExpClk(hwAttrs->baseAddr,
        MAP_PRCMPeripheralClockGet(PRCM_SDHOST), SD_ID_FREQ_400KHZ);

    MAP_SDHostIntClear(hwAttrs->baseAddr, SDHOST_INT_CC | SDHOST_INT_TC |
        DATAERROR | CMDERROR | SDHOST_INT_DMAWR | SDHOST_INT_DMARD |
        SDHOST_INT_BRR | SDHOST_INT_BWR);

    /*
     * DMA channels 23 and 24 are connected to the SD peripheral by default.
     * If someone hasn't remapped them to something else already, we remap
     * them to SW.
     */
    if (!(HWREG(UDMA_BASE + UDMA_O_CHMAP2) & UDMA_CHMAP2_CH23SEL_M)) {
        MAP_uDMAChannelAssign(UDMA_CH23_SW);
    }
    if (!(HWREG(UDMA_BASE + UDMA_O_CHMAP3) & UDMA_CHMAP3_CH24SEL_M)) {
        MAP_uDMAChannelAssign(UDMA_CH24_SW);
    }

    /* Configure DMA for TX and RX */
    MAP_uDMAChannelAssign(hwAttrs->txChIdx);
    MAP_uDMAChannelAssign(hwAttrs->rxChIdx);

    MAP_uDMAChannelEnable(hwAttrs->txChIdx);
    MAP_uDMAChannelEnable(hwAttrs->rxChIdx);

    /* Enable SDHost Error Interrupts */
    MAP_SDHostIntEnable(hwAttrs->baseAddr, DATAERROR | CMDERROR);
}


/*
 *  ======== postNotifyFxn ========
 *  Called by Power module when waking up from LPDS.
 */
static int postNotifyFxn(unsigned int eventType, uintptr_t eventArg,
        uintptr_t clientArg)
{
    initHw((SD_Handle)clientArg);

    return (Power_NOTIFYDONE);
}

/*
 *  ======== send_cmd ========
 *  Function to send a command to the SD Card
 */
static int_fast32_t send_cmd(SD_Handle handle, uint_fast32_t cmd,
    uint_fast32_t arg)
{
    int_fast32_t                  result  = SD_STATUS_SUCCESS;
    SDHostCC32XX_Object          *object  = handle->object;
    SDHostCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    /* Enable command complete interrupts */
    MAP_SDHostIntEnable(hwAttrs->baseAddr, SDHOST_INT_CC);

    /* Send the command */
    MAP_SDHostCmdSend(hwAttrs->baseAddr, cmd, arg);

    SemaphoreP_pend(object->cmdSem, SemaphoreP_WAIT_FOREVER);

    MAP_SDHostIntDisable(hwAttrs->baseAddr, SDHOST_INT_CC);

    /* SD Card Error, reset the command line */
    if (object->stat == SD_STATUS_ERROR) {
        MAP_SDHostCmdReset(hwAttrs->baseAddr);
        result = SD_STATUS_ERROR;
    }

    return (result);
}

/*
 *  ======== selectCard ========
 *  Function to select a card on the specified drive.
 */
static int_fast32_t selectCard(SD_Handle handle)
{
    int_fast32_t                  result;
    SDHostCC32XX_Object          *object  = handle->object;
    SDHostCC32XX_HWAttrsV1 const *hwAttrs = handle->hwAttrs;

    /* Select the card */
    result = send_cmd(handle, CMD_SELECT_CARD, object->rca << 16);

    if (result == SD_STATUS_SUCCESS) {
        /* Wait for transfer compelte interrupt */
        MAP_SDHostIntEnable(hwAttrs->baseAddr, SDHOST_INT_TC);

        SemaphoreP_pend(object->cmdSem, SemaphoreP_WAIT_FOREVER);

        MAP_SDHostIntDisable(hwAttrs->baseAddr, SDHOST_INT_TC);

        /* Host controller error occurred */
        if (object->stat != SD_STATUS_SUCCESS) {
            result = SD_STATUS_ERROR;
        }
    }
    return (result);
}
