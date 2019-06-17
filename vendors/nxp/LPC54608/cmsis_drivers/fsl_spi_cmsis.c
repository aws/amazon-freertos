/*
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "fsl_spi_cmsis.h"

#define ARM_SPI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0) /* driver version */

/*! @brief IDs of clock for each FLEXCOMM module */
static const clock_ip_name_t s_flexcommClocks[] = FLEXCOMM_CLOCKS;
/* Array of SPI reset number. */
static const reset_ip_name_t s_spiResetInstance[] = FLEXCOMM_RSTS;
/*
 * ARMCC does not support split the data section automatically, so the driver
 * needs to split the data to separate sections explicitly, to reduce codesize.
 */
#if defined(__CC_ARM)
#define ARMCC_SECTION(section_name) __attribute__((section(section_name)))
#endif

typedef struct _cmsis_spi_status
{
    bool isInitialized;
    bool isPowerOn;
    bool isConfigured;
    bool isMasterMode;
} cmsis_spi_status_t;

typedef const struct _cmsis_spi_resource
{
    SPI_Type *base;
    uint32_t instance;
    uint32_t (*GetFreq)(void);
} cmsis_spi_resource_t;

typedef union _cmsis_spi_handle
{
    spi_master_handle_t masterHandle;
    spi_slave_handle_t slaveHandle;
} cmsis_spi_handle_t;

typedef struct _cmsis_spi_interrupt_driver_state
{
    cmsis_spi_resource_t *resource;
    cmsis_spi_handle_t *handle;
    ARM_SPI_SignalEvent_t cb_event;
    uint32_t baudRate_Bps;
    cmsis_spi_status_t status;
} cmsis_spi_interrupt_driver_state_t;

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

typedef const struct _cmsis_spi_dma_resource
{
    DMA_Type *txdmaBase;
    uint32_t txdmaChannel;

    DMA_Type *rxdmaBase;
    uint32_t rxdmaChannel;
} cmsis_spi_dma_resource_t;

typedef union _cmsis_spi_dma_handle
{
    spi_dma_handle_t masterHandle;
    spi_dma_handle_t slaveHandle;
} cmsis_spi_dma_handle_t;

typedef struct _cmsis_spi_dma_driver_state
{
    cmsis_spi_resource_t *resource;
    cmsis_spi_dma_resource_t *dmaResource;
    cmsis_spi_dma_handle_t *handle;
    dma_handle_t *dmaRxDataHandle;
    dma_handle_t *dmaTxDataHandle;

    uint32_t baudRate_Bps;
    ARM_SPI_SignalEvent_t cb_event;
    cmsis_spi_status_t status;
} cmsis_spi_dma_driver_state_t;
#endif

/* Driver Version */
static const ARM_DRIVER_VERSION s_SPIDriverVersion = {ARM_SPI_API_VERSION, ARM_SPI_DRV_VERSION};

/* Driver Capabilities */
static const ARM_SPI_CAPABILITIES s_SPIDriverCapabilities = {
    1, /* Simplex Mode (Master and Slave) */
    0, /* TI Synchronous Serial Interface */
    0, /* Microwire Interface  */
    0  /* Signal Mode Fault event: \ref ARM_SPI_EVENT_MODE_FAULT */
};

/*******************************************************************************
 * Code
 ******************************************************************************/

void KSDK_SPI_MasterInterruptCallback(SPI_Type *base, spi_master_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0;

    if ((kStatus_Success == status) || (kStatus_SPI_Idle == status))
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_SPI_Error == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }

    /* User data is actually CMSIS driver callback. */
    if (userData)
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}
void KSDK_SPI_SlaveInterruptCallback(SPI_Type *base, spi_slave_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0;

    if ((kStatus_Success == status) || (kStatus_SPI_Idle == status))
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_SPI_Error == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }

    /* User data is actually CMSIS driver callback. */
    if (userData)
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}

void SPI_MasterCommonControl(uint32_t control,
                             cmsis_spi_resource_t *resource,
                             cmsis_spi_status_t *status,
                             spi_master_config_t *masterConfig)
{
    switch (control & ARM_SPI_FRAME_FORMAT_Msk)
    {
        case ARM_SPI_CPOL0_CPHA1:
            masterConfig->polarity = kSPI_ClockPolarityActiveLow;
            masterConfig->phase = kSPI_ClockPhaseSecondEdge;
            break;

        case ARM_SPI_CPOL1_CPHA0:
            masterConfig->polarity = kSPI_ClockPolarityActiveHigh;
            masterConfig->phase = kSPI_ClockPhaseFirstEdge;
            break;

        case ARM_SPI_CPOL1_CPHA1:
            masterConfig->polarity = kSPI_ClockPolarityActiveHigh;
            masterConfig->phase = kSPI_ClockPhaseSecondEdge;
            break;
        case ARM_SPI_CPOL0_CPHA0:
            masterConfig->polarity = kSPI_ClockPolarityActiveLow;
            masterConfig->phase = kSPI_ClockPhaseFirstEdge;
            break;

        default:
            break;
    }

    if (control & ARM_SPI_DATA_BITS_Msk) /* setting Number of Data bits */
    {
        if ((((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos) >= 4) &&
            (((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos) <= 16))
        {
            masterConfig->dataWidth =
                (spi_data_width_t)(((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos) - 1);
        }
    }

    switch (control & ARM_SPI_BIT_ORDER_Msk)
    {
        case ARM_SPI_LSB_MSB:
            masterConfig->direction = kSPI_LsbFirst;
            break;
        case ARM_SPI_MSB_LSB:
            masterConfig->direction = kSPI_MsbFirst;
            break;

        default:
            break;
    }
}

void SPI_SlaveCommonControl(uint32_t control,
                            cmsis_spi_resource_t *resource,
                            cmsis_spi_status_t *status,
                            spi_slave_config_t *slaveConfig)
{
    switch (control & ARM_SPI_FRAME_FORMAT_Msk)
    {
        case ARM_SPI_CPOL0_CPHA1:
            slaveConfig->polarity = kSPI_ClockPolarityActiveLow;
            slaveConfig->phase = kSPI_ClockPhaseSecondEdge;
            break;

        case ARM_SPI_CPOL1_CPHA0:
            slaveConfig->polarity = kSPI_ClockPolarityActiveHigh;
            slaveConfig->phase = kSPI_ClockPhaseFirstEdge;
            break;

        case ARM_SPI_CPOL1_CPHA1:
            slaveConfig->polarity = kSPI_ClockPolarityActiveHigh;
            slaveConfig->phase = kSPI_ClockPhaseSecondEdge;
            break;
        case ARM_SPI_CPOL0_CPHA0:
            slaveConfig->polarity = kSPI_ClockPolarityActiveLow;
            slaveConfig->phase = kSPI_ClockPhaseFirstEdge;
            break;

        default:
            break;
    }
    if (control & ARM_SPI_DATA_BITS_Msk) /* setting Number of Data bits */
    {
        if ((((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos) >= 4) &&
            (((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos) <= 16))
        {
            slaveConfig->dataWidth =
                (spi_data_width_t)(((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos) - 1);
        }
    }
    switch (control & ARM_SPI_BIT_ORDER_Msk)
    {
        case ARM_SPI_LSB_MSB:
            slaveConfig->direction = kSPI_LsbFirst;
            break;
        case ARM_SPI_MSB_LSB:
            slaveConfig->direction = kSPI_MsbFirst;
            break;

        default:
            break;
    }
}

static ARM_DRIVER_VERSION SPIx_GetVersion(void)
{
    return s_SPIDriverVersion;
}

static ARM_SPI_CAPABILITIES SPIx_GetCapabilities(void)
{
    return s_SPIDriverCapabilities;
}

static int32_t SPI_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event, cmsis_spi_interrupt_driver_state_t *spi)
{
    if (spi->status.isInitialized)
    {
        return ARM_DRIVER_OK;
    }
    spi->cb_event = cb_event;
    spi->status.isInitialized = true;
    return ARM_DRIVER_OK;
}

static int32_t SPI_InterruptUninitialize(cmsis_spi_interrupt_driver_state_t *spi)
{
    spi->status.isInitialized = false;
    return ARM_DRIVER_OK;
}

static int32_t SPI_InterruptPowerControl(ARM_POWER_STATE state, cmsis_spi_interrupt_driver_state_t *spi)
{
    switch (state)
    {
        case ARM_POWER_OFF:
            if (spi->status.isPowerOn)
            {
                SPI_Deinit(spi->resource->base);
                /* Reset Periphera instance, and disable the clock gate */
                RESET_PeripheralReset(s_spiResetInstance[spi->resource->instance]);
                spi->status.isPowerOn = false;
                spi->status.isConfigured = false;
                spi->status.isMasterMode = false;
            }
            break;

        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_POWER_FULL:
            if (!spi->status.isInitialized)
            {
                return ARM_DRIVER_ERROR;
            }

            if (!spi->status.isPowerOn)
            {
                /* Enable flexcomm clock gate */
                CLOCK_EnableClock(s_flexcommClocks[spi->resource->instance]);
                spi->status.isPowerOn = true;
            }
            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t SPI_InterruptSend(const void *data, uint32_t num, cmsis_spi_interrupt_driver_state_t *spi)
{
    int32_t ret;
    status_t status;
    spi_transfer_t xfer;

    xfer.rxData = NULL;
    xfer.txData = (uint8_t *)data;
    xfer.dataSize = num;

    if (spi->status.isMasterMode)
    {
        status = SPI_MasterTransferNonBlocking(spi->resource->base, &spi->handle->masterHandle, &xfer);
    }
    else
    {
        status = SPI_SlaveTransferNonBlocking(spi->resource->base, &spi->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_SPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
        default:
            ret = ARM_DRIVER_ERROR;
    }

    return ret;
}

static int32_t SPI_InterruptReceive(void *data, uint32_t num, cmsis_spi_interrupt_driver_state_t *spi)
{
    int32_t ret;
    status_t status;
    spi_transfer_t xfer;

    xfer.txData = NULL;
    xfer.rxData = (uint8_t *)data;
    xfer.dataSize = num;

    if (spi->status.isMasterMode)
    {
        status = SPI_MasterTransferNonBlocking(spi->resource->base, &spi->handle->masterHandle, &xfer);
    }
    else
    {
        status = SPI_SlaveTransferNonBlocking(spi->resource->base, &spi->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_SPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
        default:
            ret = ARM_DRIVER_ERROR;
    }

    return ret;
}

static int32_t SPI_InterruptTransfer(const void *data_out,
                                     void *data_in,
                                     uint32_t num,
                                     cmsis_spi_interrupt_driver_state_t *spi)
{
    int32_t ret;
    status_t status;
    spi_transfer_t xfer;

    xfer.txData = (uint8_t *)data_out;
    xfer.rxData = (uint8_t *)data_in;
    xfer.dataSize = num;
    if (spi->status.isMasterMode)
    {
        xfer.configFlags |= kSPI_FrameAssert;
    }

    if (spi->status.isMasterMode)
    {
        status = SPI_MasterTransferNonBlocking(spi->resource->base, &spi->handle->masterHandle, &xfer);
    }
    else
    {
        status = SPI_SlaveTransferNonBlocking(spi->resource->base, &spi->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_SPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
        default:
            ret = ARM_DRIVER_ERROR;
    }

    return ret;
}
static uint32_t SPI_InterruptGetCount(cmsis_spi_interrupt_driver_state_t *spi)
{
    if (spi->status.isMasterMode)
    {
        return spi->handle->masterHandle.totalByteCount - spi->handle->masterHandle.rxRemainingBytes;
    }
    else
    {
        return spi->handle->slaveHandle.toReceiveCount - spi->handle->slaveHandle.rxRemainingBytes;
    }
}

static int32_t SPI_InterruptControl(uint32_t control, uint32_t arg, cmsis_spi_interrupt_driver_state_t *spi)
{
    if (!spi->status.isPowerOn)
    {
        return ARM_DRIVER_ERROR;
    }

    switch (control & ARM_SPI_CONTROL_Msk)
    {
        case ARM_SPI_MODE_INACTIVE: /* SPI mode Inactive */
            FLEXCOMM_Init(spi->resource->base, FLEXCOMM_PERIPH_NONE);
            break;

        case ARM_SPI_MODE_MASTER: /* SPI Master (Output on MOSI, Input on MISO); arg = Bus Speed in bps */
            spi->baudRate_Bps = arg;
            spi->status.isMasterMode = true;
            break;

        case ARM_SPI_MODE_SLAVE: /* SPI Slave  (Output on MISO, Input on MOSI) */
            spi->status.isMasterMode = false;
            break;

        case ARM_SPI_GET_BUS_SPEED: /* Get Bus Speed in bps */
            if (!spi->status.isMasterMode)
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            return spi->baudRate_Bps;

        case ARM_SPI_SET_BUS_SPEED: /* Set Bus Speed in bps; */
            if (!spi->status.isMasterMode)
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }
            SPI_MasterSetBaud(spi->resource->base, arg, spi->resource->GetFreq());
            spi->baudRate_Bps = arg;

            return ARM_DRIVER_OK;

        case ARM_SPI_CONTROL_SS:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_SPI_ABORT_TRANSFER: /* Abort current data transfer */

            if (spi->status.isMasterMode)
            {
                SPI_MasterTransferAbort(spi->resource->base, &spi->handle->masterHandle);
            }
            else
            {
                SPI_SlaveTransferAbort(spi->resource->base, &spi->handle->slaveHandle);
            }
            return ARM_DRIVER_OK;

        default:
            break;
    }

    if (spi->status.isMasterMode)
    {
        switch (control & ARM_SPI_SS_MASTER_MODE_Msk)
        {
            /*
             * Note:
             * ARM_SPI_SS_MASTER_HW_OUTPUT is default configuration in driver, if ARM_SPI_SS_MASTER_UNUSED or
             * ARM_SPI_SS_MASTER_SW is wanted, please disable pin function in SPIx_InitPins() which is configured
             * by user in extern file. Besides ARM_SPI_SS_MASTER_HW_INPUT is not supported in this driver.
             */
            case ARM_SPI_SS_MASTER_UNUSED: /*!< SPI Slave Select when Master: Not used */
                break;
            case ARM_SPI_SS_MASTER_SW: /*!< SPI Slave Select when Master: Software controlled. */
                break;
            case ARM_SPI_SS_MASTER_HW_OUTPUT: /*!< SPI Slave Select when Master: Hardware controlled Output */
                break;
            case ARM_SPI_SS_MASTER_HW_INPUT: /*!< SPI Slave Select when Master: Hardware monitored Input */
                return ARM_SPI_ERROR_SS_MODE;
            default:
                break;
        }

        spi_master_config_t masterConfig;
        SPI_MasterGetDefaultConfig(&masterConfig);
        masterConfig.baudRate_Bps = spi->baudRate_Bps;

        SPI_MasterCommonControl(control, spi->resource, &spi->status, &masterConfig);

        if (spi->status.isConfigured)
        {
            SPI_Deinit(spi->resource->base);
            RESET_PeripheralReset(s_spiResetInstance[spi->resource->instance]);
        }
        SPI_MasterInit(spi->resource->base, &masterConfig, spi->resource->GetFreq());
        SPI_MasterTransferCreateHandle(spi->resource->base, &spi->handle->masterHandle,
                                       KSDK_SPI_MasterInterruptCallback, (void *)spi->cb_event);
        spi->status.isConfigured = true;
    }
    else
    {
        spi_slave_config_t slaveConfig;
        SPI_SlaveGetDefaultConfig(&slaveConfig);

        SPI_SlaveCommonControl(control, spi->resource, &spi->status, &slaveConfig);

        if (spi->status.isConfigured)
        {
            SPI_Deinit(spi->resource->base);
            RESET_PeripheralReset(s_spiResetInstance[spi->resource->instance]);
        }
        SPI_SlaveInit(spi->resource->base, &slaveConfig);
        SPI_SlaveTransferCreateHandle(spi->resource->base, &spi->handle->slaveHandle, KSDK_SPI_SlaveInterruptCallback,
                                      (void *)spi->cb_event);
        spi->status.isConfigured = true;
    }

    return ARM_DRIVER_OK;
}

ARM_SPI_STATUS SPI_InterruptGetStatus(cmsis_spi_interrupt_driver_state_t *spi)
{
    ARM_SPI_STATUS stat;

    if (spi->status.isMasterMode)
    {
        stat.busy =
            ((spi->handle->masterHandle.txRemainingBytes > 0) || (spi->handle->masterHandle.rxRemainingBytes > 0)) ?
                (0U) :
                (1U);
        stat.data_lost = (kStatus_SPI_Error == spi->handle->masterHandle.state) ? (1U) : (0U);
    }
    else
    {
        stat.busy =
            ((spi->handle->slaveHandle.txRemainingBytes > 0) || (spi->handle->slaveHandle.rxRemainingBytes > 0)) ?
                (0U) :
                (1U);
        stat.data_lost = (kStatus_SPI_Error == spi->handle->slaveHandle.state) ? (1U) : (0U);
    }

    stat.mode_fault = 0U;
    return stat;
}

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

void KSDK_SPI_MasterDMACallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_SPI_Error == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }

    if (userData)
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}
void KSDK_SPI_SlaveDMACallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_SPI_Error == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }
    /* User data is actually CMSIS driver callback. */
    if (userData)
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}

static int32_t SPI_DMAInitialize(ARM_SPI_SignalEvent_t cb_event, cmsis_spi_dma_driver_state_t *spi)
{
    if (spi->status.isInitialized)
    {
        return ARM_DRIVER_OK;
    }

    spi->status.isInitialized = true;
    spi->cb_event = cb_event;
    return ARM_DRIVER_OK;
}

static int32_t SPI_DMAUninitialize(cmsis_spi_dma_driver_state_t *spi)
{
    spi->status.isInitialized = false;
    return ARM_DRIVER_OK;
}

static int32_t SPI_DMAPowerControl(ARM_POWER_STATE state, cmsis_spi_dma_driver_state_t *spi)
{
    switch (state)
    {
        case ARM_POWER_OFF:
            if (spi->status.isPowerOn)
            {
                SPI_Deinit(spi->resource->base);
                RESET_PeripheralReset(s_spiResetInstance[spi->resource->instance]);

                DMA_DisableChannel(spi->dmaResource->txdmaBase, spi->dmaResource->txdmaChannel);
                DMA_DisableChannel(spi->dmaResource->rxdmaBase, spi->dmaResource->rxdmaChannel);
                DMA_Deinit(spi->dmaResource->txdmaBase);
                DMA_Deinit(spi->dmaResource->rxdmaBase);

                spi->status.isPowerOn = false;
                spi->status.isConfigured = false;
                spi->status.isMasterMode = false;
            }
            break;
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_POWER_FULL:
            if (!spi->status.isInitialized)
            {
                return ARM_DRIVER_ERROR;
            }

            if (!spi->status.isPowerOn)
            {
                /* Enable flexcomm clock gate */
                CLOCK_EnableClock(s_flexcommClocks[spi->resource->instance]);
                /* Init DMA */
                DMA_Init(spi->dmaResource->rxdmaBase);
                DMA_Init(spi->dmaResource->txdmaBase);
                spi->status.isPowerOn = true;
            }
            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ARM_DRIVER_OK;
}

static int32_t SPI_DMASend(const void *data, uint32_t num, cmsis_spi_dma_driver_state_t *spi)
{
    int32_t ret;
    status_t status;
    spi_transfer_t xfer;

    xfer.rxData = NULL;
    xfer.txData = (uint8_t *)data;
    xfer.dataSize = num;

    if (spi->status.isMasterMode)
    {
        status = SPI_MasterTransferDMA(spi->resource->base, &spi->handle->masterHandle, &xfer);
    }
    else
    {
        status = SPI_SlaveTransferDMA(spi->resource->base, &spi->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_SPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
    }

    return ret;
}

static int32_t SPI_DMAReceive(void *data, uint32_t num, cmsis_spi_dma_driver_state_t *spi)
{
    int32_t ret;
    status_t status;
    spi_transfer_t xfer;

    xfer.txData = NULL;
    xfer.rxData = (uint8_t *)data;
    xfer.dataSize = num;

    if (spi->status.isMasterMode)
    {
        status = SPI_MasterTransferDMA(spi->resource->base, &spi->handle->masterHandle, &xfer);
    }
    else
    {
        status = SPI_SlaveTransferDMA(spi->resource->base, &spi->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_SPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
    }

    return ret;
}

static int32_t SPI_DMATransfer(const void *data_out, void *data_in, uint32_t num, cmsis_spi_dma_driver_state_t *spi)
{
    int32_t ret;
    status_t status;
    spi_transfer_t xfer;

    xfer.txData = (uint8_t *)data_out;
    xfer.rxData = (uint8_t *)data_in;
    xfer.dataSize = num;

    if (spi->status.isMasterMode)
    {
        status = SPI_MasterTransferDMA(spi->resource->base, &spi->handle->masterHandle, &xfer);
    }
    else
    {
        status = SPI_SlaveTransferDMA(spi->resource->base, &spi->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_SPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
    }

    return ret;
}
static uint32_t SPI_DMAGetCount(cmsis_spi_dma_driver_state_t *spi)
{
    uint32_t cnt;
    size_t bytes;

    bytes = DMA_GetRemainingBytes(spi->dmaResource->rxdmaBase, spi->dmaResource->rxdmaChannel);

    if (spi->status.isMasterMode)
    {
        cnt = spi->handle->masterHandle.transferSize - bytes;
    }
    else
    {
        cnt = spi->handle->slaveHandle.transferSize - bytes;
    }

    return cnt;
}

static int32_t SPI_DMAControl(uint32_t control, uint32_t arg, cmsis_spi_dma_driver_state_t *spi)
{
    if (!spi->status.isPowerOn)
    {
        return ARM_DRIVER_ERROR;
    }

    switch (control & ARM_SPI_CONTROL_Msk)
    {
        case ARM_SPI_MODE_INACTIVE:
            SPI_Enable(spi->resource->base, false);
            break;
        case ARM_SPI_MODE_MASTER:
            spi->baudRate_Bps = arg;
            spi->status.isMasterMode = true;
            break;

        case ARM_SPI_MODE_SLAVE:
        {
            spi->status.isMasterMode = false;
        }
        break;

        case ARM_SPI_SET_BUS_SPEED:

            if (!spi->status.isMasterMode)
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            SPI_MasterSetBaud(spi->resource->base, arg, spi->resource->GetFreq());

            spi->baudRate_Bps = arg;
            return ARM_DRIVER_OK;

        case ARM_SPI_GET_BUS_SPEED: /* Set Bus Speed in bps; arg = value */
            if (!spi->status.isMasterMode)
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            return spi->baudRate_Bps;

        case ARM_SPI_CONTROL_SS:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_SPI_ABORT_TRANSFER:

            if (spi->status.isMasterMode)
            {
                SPI_MasterTransferAbortDMA(spi->resource->base, &spi->handle->masterHandle);
            }
            else
            {
                SPI_SlaveTransferAbortDMA(spi->resource->base, &spi->handle->slaveHandle);
            }
            return ARM_DRIVER_OK;

        default:
            break;
    }

    if (spi->status.isMasterMode)
    {
        switch (control & ARM_SPI_SS_MASTER_MODE_Msk)
        {
            /*
             * Note:
             * ARM_SPI_SS_MASTER_HW_OUTPUT is default configuration in driver, if ARM_SPI_SS_MASTER_UNUSED or
             * ARM_SPI_SS_MASTER_SW is wanted, please disable pin function in SPIx_InitPins() which is configured
             * by user in extern file. Besides ARM_SPI_SS_MASTER_HW_INPUT is not supported in this driver.
             */
            case ARM_SPI_SS_MASTER_UNUSED: /*!< SPI Slave Select when Master: Not used */
                break;
            case ARM_SPI_SS_MASTER_SW: /*!< SPI Slave Select when Master: Software controlled. */
                break;
            case ARM_SPI_SS_MASTER_HW_OUTPUT: /*!< SPI Slave Select when Master: Hardware controlled Output */
                break;
            case ARM_SPI_SS_MASTER_HW_INPUT: /*!< SPI Slave Select when Master: Hardware monitored Input */
                return ARM_SPI_ERROR_SS_MODE;
            default:
                break;
        }
        spi_master_config_t masterConfig;
        SPI_MasterGetDefaultConfig(&masterConfig);
        masterConfig.baudRate_Bps = spi->baudRate_Bps;
        SPI_MasterCommonControl(control, spi->resource, &spi->status, &masterConfig);

        if (spi->status.isConfigured)
        {
            SPI_Deinit(spi->resource->base);
            RESET_PeripheralReset(s_spiResetInstance[spi->resource->instance]);
        }
        SPI_MasterInit(spi->resource->base, &masterConfig, spi->resource->GetFreq());

        DMA_EnableChannel(spi->dmaResource->txdmaBase, spi->dmaResource->txdmaChannel);
        DMA_EnableChannel(spi->dmaResource->rxdmaBase, spi->dmaResource->rxdmaChannel);
        DMA_SetChannelPriority(spi->dmaResource->txdmaBase, spi->dmaResource->txdmaChannel, kDMA_ChannelPriority3);
        DMA_SetChannelPriority(spi->dmaResource->rxdmaBase, spi->dmaResource->rxdmaChannel, kDMA_ChannelPriority2);
        DMA_CreateHandle(spi->dmaTxDataHandle, spi->dmaResource->txdmaBase, spi->dmaResource->txdmaChannel);
        DMA_CreateHandle(spi->dmaRxDataHandle, spi->dmaResource->rxdmaBase, spi->dmaResource->rxdmaChannel);

        SPI_MasterTransferCreateHandleDMA(spi->resource->base, &(spi->handle->masterHandle), KSDK_SPI_MasterDMACallback,
                                          (void *)spi->cb_event, spi->dmaTxDataHandle, spi->dmaRxDataHandle);
        spi->status.isConfigured = true;
    }
    else
    {
        spi_slave_config_t slaveConfig;
        SPI_SlaveGetDefaultConfig(&slaveConfig);
        SPI_SlaveCommonControl(control, spi->resource, &spi->status, &slaveConfig);

        if (spi->status.isConfigured)
        {
            SPI_Deinit(spi->resource->base);
            RESET_PeripheralReset(s_spiResetInstance[spi->resource->instance]);
        }
        SPI_SlaveInit(spi->resource->base, &slaveConfig);

        DMA_EnableChannel(spi->dmaResource->txdmaBase, spi->dmaResource->txdmaChannel);
        DMA_EnableChannel(spi->dmaResource->rxdmaBase, spi->dmaResource->rxdmaChannel);
        DMA_SetChannelPriority(spi->dmaResource->txdmaBase, spi->dmaResource->txdmaChannel, kDMA_ChannelPriority0);
        DMA_SetChannelPriority(spi->dmaResource->rxdmaBase, spi->dmaResource->rxdmaChannel, kDMA_ChannelPriority1);
        DMA_CreateHandle(spi->dmaTxDataHandle, spi->dmaResource->txdmaBase, spi->dmaResource->txdmaChannel);
        DMA_CreateHandle(spi->dmaRxDataHandle, spi->dmaResource->rxdmaBase, spi->dmaResource->rxdmaChannel);

        SPI_SlaveTransferCreateHandleDMA(spi->resource->base, &(spi->handle->slaveHandle), KSDK_SPI_SlaveDMACallback,
                                         (void *)spi->cb_event, spi->dmaTxDataHandle, spi->dmaRxDataHandle);

        spi->status.isConfigured = true;
    }

    return ARM_DRIVER_OK;
}

ARM_SPI_STATUS SPI_DMAGetStatus(cmsis_spi_dma_driver_state_t *spi)
{
    ARM_SPI_STATUS stat;

    if (spi->status.isMasterMode)
    {
        stat.busy =
            ((spi->handle->masterHandle.txInProgress == true) || (spi->handle->masterHandle.rxInProgress == true)) ?
                (0U) :
                (1U);
        stat.data_lost = (kStatus_SPI_Error == spi->handle->masterHandle.state) ? (1U) : (0U);
    }
    else
    {
        stat.busy =
            ((spi->handle->slaveHandle.txInProgress == true) || (spi->handle->slaveHandle.rxInProgress == true)) ?
                (0U) :
                (1U);
        stat.data_lost = (kStatus_SPI_Error == spi->handle->slaveHandle.state) ? (1U) : (0U);
    }

    stat.mode_fault = 0U;
    return stat;
}
#endif /* defined(FSL_FEATURE_SOC_DMA_COUNT) */

#if defined(SPI0)

extern uint32_t SPI0_GetFreq(void);
extern void SPI0_InitPins(void);
extern void SPI0_DeinitPins(void);

cmsis_spi_resource_t SPI0_Resource = {SPI0, 0, SPI0_GetFreq};

static cmsis_spi_handle_t SPI0_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi0_interrupt_driver_state")
static cmsis_spi_interrupt_driver_state_t SPI0_InterruptDriverState = {
#else
static cmsis_spi_interrupt_driver_state_t SPI0_InterruptDriverState = {
#endif
    &SPI0_Resource, &SPI0_Handle,
};

static int32_t SPI0_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI0_InitPins();
    return SPI_InterruptInitialize(cb_event, &SPI0_InterruptDriverState);
}

static int32_t SPI0_InterruptUninitialize(void)
{
    SPI0_DeinitPins();
    return SPI_InterruptUninitialize(&SPI0_InterruptDriverState);
}

static int32_t SPI0_InterruptPowerControl(ARM_POWER_STATE state)
{
    return SPI_InterruptPowerControl(state, &SPI0_InterruptDriverState);
}

static int32_t SPI0_InterruptSend(const void *data, uint32_t num)
{
    return SPI_InterruptSend(data, num, &SPI0_InterruptDriverState);
}

static int32_t SPI0_InterruptReceive(void *data, uint32_t num)
{
    return SPI_InterruptReceive(data, num, &SPI0_InterruptDriverState);
}

static int32_t SPI0_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_InterruptTransfer(data_out, data_in, num, &SPI0_InterruptDriverState);
}

static uint32_t SPI0_InterruptGetCount(void)
{
    return SPI_InterruptGetCount(&SPI0_InterruptDriverState);
}

static int32_t SPI0_InterruptControl(uint32_t control, uint32_t arg)
{
    return SPI_InterruptControl(control, arg, &SPI0_InterruptDriverState);
}

static ARM_SPI_STATUS SPI0_InterruptGetStatus(void)
{
    return SPI_InterruptGetStatus(&SPI0_InterruptDriverState);
}

ARM_DRIVER_SPI SPI0_Interrupt_Driver = {SPIx_GetVersion,
                                        SPIx_GetCapabilities,
                                        SPI0_InterruptInitialize,
                                        SPI0_InterruptUninitialize,
                                        SPI0_InterruptPowerControl,
                                        SPI0_InterruptSend,
                                        SPI0_InterruptReceive,
                                        SPI0_InterruptTransfer,
                                        SPI0_InterruptGetCount,
                                        SPI0_InterruptControl,
                                        SPI0_InterruptGetStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
cmsis_spi_dma_resource_t SPI0_DMAResource = {RTE_SPI0_DMA_TX_DMA_BASE, RTE_SPI0_DMA_TX_CH, RTE_SPI0_DMA_RX_DMA_BASE,
                                             RTE_SPI0_DMA_RX_CH};

static cmsis_spi_dma_handle_t SPI0_DmaHandle;
static dma_handle_t SPI0_DmaTxDataHandle;
static dma_handle_t SPI0_DmaRxDataHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi0_dma_driver_state")
static cmsis_spi_dma_driver_state_t SPI0_DMADriverState = {
#else
static cmsis_spi_dma_driver_state_t SPI0_DMADriverState = {
#endif
    &SPI0_Resource, &SPI0_DMAResource, &SPI0_DmaHandle, &SPI0_DmaTxDataHandle, &SPI0_DmaRxDataHandle,

};

static int32_t SPI0_DMAtInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI0_InitPins();
    return SPI_DMAInitialize(cb_event, &SPI0_DMADriverState);
}

static int32_t SPI0_DMAUninitialize(void)
{
    SPI0_DeinitPins();
    return SPI_DMAUninitialize(&SPI0_DMADriverState);
}

static int32_t SPI0_DMAPowerControl(ARM_POWER_STATE state)
{
    return SPI_DMAPowerControl(state, &SPI0_DMADriverState);
}

static int32_t SPI0_DMASend(const void *data, uint32_t num)
{
    return SPI_DMASend(data, num, &SPI0_DMADriverState);
}

static int32_t SPI0_DMAReceive(void *data, uint32_t num)
{
    return SPI_DMAReceive(data, num, &SPI0_DMADriverState);
}

static int32_t SPI0_DMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_DMATransfer(data_out, data_in, num, &SPI0_DMADriverState);
}

static uint32_t SPI0_DMAGetCount(void)
{
    return SPI_DMAGetCount(&SPI0_DMADriverState);
}

static int32_t SPI0_DMAControl(uint32_t control, uint32_t arg)
{
    return SPI_DMAControl(control, arg, &SPI0_DMADriverState);
}

static ARM_SPI_STATUS SPI0_DMAGetStatus(void)
{
    return SPI_DMAGetStatus(&SPI0_DMADriverState);
}

ARM_DRIVER_SPI SPI0_Dma_Driver = {SPIx_GetVersion,      SPIx_GetCapabilities, SPI0_DMAtInitialize, SPI0_DMAUninitialize,
                                  SPI0_DMAPowerControl, SPI0_DMASend,         SPI0_DMAReceive,     SPI0_DMATransfer,
                                  SPI0_DMAGetCount,     SPI0_DMAControl,      SPI0_DMAGetStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /*  SPI0  */

#if defined(SPI1)
extern uint32_t SPI1_GetFreq(void);
extern void SPI1_InitPins(void);
extern void SPI1_DeinitPins(void);
cmsis_spi_resource_t SPI1_Resource = {SPI1, 1, SPI1_GetFreq};

static cmsis_spi_handle_t SPI1_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi1_interrupt_driver_state")
static cmsis_spi_interrupt_driver_state_t SPI1_InterruptDriverState = {
#else
static cmsis_spi_interrupt_driver_state_t SPI1_InterruptDriverState = {
#endif
    &SPI1_Resource, &SPI1_Handle,
};

static int32_t SPI1_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI1_InitPins();
    return SPI_InterruptInitialize(cb_event, &SPI1_InterruptDriverState);
}

static int32_t SPI1_InterruptUninitialize(void)
{
    SPI1_DeinitPins();
    return SPI_InterruptUninitialize(&SPI1_InterruptDriverState);
}

static int32_t SPI1_InterruptPowerControl(ARM_POWER_STATE state)
{
    return SPI_InterruptPowerControl(state, &SPI1_InterruptDriverState);
}

static int32_t SPI1_InterruptSend(const void *data, uint32_t num)
{
    return SPI_InterruptSend(data, num, &SPI1_InterruptDriverState);
}

static int32_t SPI1_InterruptReceive(void *data, uint32_t num)
{
    return SPI_InterruptReceive(data, num, &SPI1_InterruptDriverState);
}

static int32_t SPI1_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_InterruptTransfer(data_out, data_in, num, &SPI1_InterruptDriverState);
}

static uint32_t SPI1_InterruptGetCount(void)
{
    return SPI_InterruptGetCount(&SPI1_InterruptDriverState);
}

static int32_t SPI1_InterruptControl(uint32_t control, uint32_t arg)
{
    return SPI_InterruptControl(control, arg, &SPI1_InterruptDriverState);
}

static ARM_SPI_STATUS SPI1_InterruptGetStatus(void)
{
    return SPI_InterruptGetStatus(&SPI1_InterruptDriverState);
}

ARM_DRIVER_SPI SPI1_Interrupt_Driver = {SPIx_GetVersion,
                                        SPIx_GetCapabilities,
                                        SPI1_InterruptInitialize,
                                        SPI1_InterruptUninitialize,
                                        SPI1_InterruptPowerControl,
                                        SPI1_InterruptSend,
                                        SPI1_InterruptReceive,
                                        SPI1_InterruptTransfer,
                                        SPI1_InterruptGetCount,
                                        SPI1_InterruptControl,
                                        SPI1_InterruptGetStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
cmsis_spi_dma_resource_t SPI1_DMAResource = {RTE_SPI1_DMA_TX_DMA_BASE, RTE_SPI1_DMA_TX_CH, RTE_SPI1_DMA_RX_DMA_BASE,
                                             RTE_SPI1_DMA_RX_CH};

static cmsis_spi_dma_handle_t SPI1_DmaHandle;
static dma_handle_t SPI1_DmaTxDataHandle;
static dma_handle_t SPI1_DmaRxDataHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi1_dma_driver_state")
static cmsis_spi_dma_driver_state_t SPI1_DMADriverState = {
#else
static cmsis_spi_dma_driver_state_t SPI1_DMADriverState = {
#endif
    &SPI1_Resource, &SPI1_DMAResource, &SPI1_DmaHandle, &SPI1_DmaRxDataHandle, &SPI1_DmaTxDataHandle,
};

static int32_t SPI1_DMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI1_InitPins();
    return SPI_DMAInitialize(cb_event, &SPI1_DMADriverState);
}

static int32_t SPI1_DMAUninitialize(void)
{
    SPI1_DeinitPins();
    return SPI_DMAUninitialize(&SPI1_DMADriverState);
}

static int32_t SPI1_DMAPowerControl(ARM_POWER_STATE state)
{
    return SPI_DMAPowerControl(state, &SPI1_DMADriverState);
}

static int32_t SPI1_DMASend(const void *data, uint32_t num)
{
    return SPI_DMASend(data, num, &SPI1_DMADriverState);
}

static int32_t SPI1_DMAReceive(void *data, uint32_t num)
{
    return SPI_DMAReceive(data, num, &SPI1_DMADriverState);
}

static int32_t SPI1_DMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_DMATransfer(data_out, data_in, num, &SPI1_DMADriverState);
}

static uint32_t SPI1_DMAGetCount(void)
{
    return SPI_DMAGetCount(&SPI1_DMADriverState);
}

static int32_t SPI1_DMAControl(uint32_t control, uint32_t arg)
{
    return SPI_DMAControl(control, arg, &SPI1_DMADriverState);
}

static ARM_SPI_STATUS SPI1_DMAGetStatus(void)
{
    return SPI_DMAGetStatus(&SPI1_DMADriverState);
}

ARM_DRIVER_SPI SPI1_Dma_Driver = {SPIx_GetVersion,      SPIx_GetCapabilities, SPI1_DMAInitialize, SPI1_DMAUninitialize,
                                  SPI1_DMAPowerControl, SPI1_DMASend,         SPI1_DMAReceive,    SPI1_DMATransfer,
                                  SPI1_DMAGetCount,     SPI1_DMAControl,      SPI1_DMAGetStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /*  SPI1  */

#if defined(SPI2)

extern uint32_t SPI2_GetFreq(void);
extern void SPI2_InitPins(void);
extern void SPI2_DeinitPins(void);

cmsis_spi_resource_t SPI2_Resource = {SPI2, 2, SPI2_GetFreq};

static cmsis_spi_handle_t SPI2_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi2_interrupt_driver_state")
static cmsis_spi_interrupt_driver_state_t SPI2_InterruptDriverState = {
#else
static cmsis_spi_interrupt_driver_state_t SPI2_InterruptDriverState = {
#endif
    &SPI2_Resource, &SPI2_Handle,
};

static int32_t SPI2_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI2_InitPins();
    return SPI_InterruptInitialize(cb_event, &SPI2_InterruptDriverState);
}

static int32_t SPI2_InterruptUninitialize(void)
{
    SPI2_DeinitPins();
    return SPI_InterruptUninitialize(&SPI2_InterruptDriverState);
}

static int32_t SPI2_InterruptPowerControl(ARM_POWER_STATE state)
{
    return SPI_InterruptPowerControl(state, &SPI2_InterruptDriverState);
}

static int32_t SPI2_InterruptSend(const void *data, uint32_t num)
{
    return SPI_InterruptSend(data, num, &SPI2_InterruptDriverState);
}

static int32_t SPI2_InterruptReceive(void *data, uint32_t num)
{
    return SPI_InterruptReceive(data, num, &SPI2_InterruptDriverState);
}

static int32_t SPI2_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_InterruptTransfer(data_out, data_in, num, &SPI2_InterruptDriverState);
}

static uint32_t SPI2_InterruptGetCount(void)
{
    return SPI_InterruptGetCount(&SPI2_InterruptDriverState);
}

static int32_t SPI2_InterruptControl(uint32_t control, uint32_t arg)
{
    return SPI_InterruptControl(control, arg, &SPI2_InterruptDriverState);
}

static ARM_SPI_STATUS SPI2_InterruptGetStatus(void)
{
    return SPI_InterruptGetStatus(&SPI2_InterruptDriverState);
}

ARM_DRIVER_SPI SPI2_Interrupt_Driver = {SPIx_GetVersion,
                                        SPIx_GetCapabilities,
                                        SPI2_InterruptInitialize,
                                        SPI2_InterruptUninitialize,
                                        SPI2_InterruptPowerControl,
                                        SPI2_InterruptSend,
                                        SPI2_InterruptReceive,
                                        SPI2_InterruptTransfer,
                                        SPI2_InterruptGetCount,
                                        SPI2_InterruptControl,
                                        SPI2_InterruptGetStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
cmsis_spi_dma_resource_t SPI2_DMAResource = {RTE_SPI2_DMA_TX_DMA_BASE, RTE_SPI2_DMA_TX_CH, RTE_SPI2_DMA_RX_DMA_BASE,
                                             RTE_SPI2_DMA_RX_CH};

static cmsis_spi_dma_handle_t SPI2_DmaHandle;
static dma_handle_t SPI2_DmaTxDataHandle;
static dma_handle_t SPI2_DmaRxDataHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi2_dma_driver_state")
static cmsis_spi_dma_driver_state_t SPI2_DMADriverState = {
#else
static cmsis_spi_dma_driver_state_t SPI2_DMADriverState = {
#endif
    &SPI2_Resource, &SPI2_DMAResource, &SPI2_DmaHandle, &SPI2_DmaRxDataHandle, &SPI2_DmaTxDataHandle,
};

static int32_t SPI2_DMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI2_InitPins();
    return SPI_DMAInitialize(cb_event, &SPI2_DMADriverState);
}

static int32_t SPI2_DMAUninitialize(void)
{
    SPI2_DeinitPins();
    return SPI_DMAUninitialize(&SPI2_DMADriverState);
}

static int32_t SPI2_DMAPowerControl(ARM_POWER_STATE state)
{
    return SPI_DMAPowerControl(state, &SPI2_DMADriverState);
}

static int32_t SPI2_DMASend(const void *data, uint32_t num)
{
    return SPI_DMASend(data, num, &SPI2_DMADriverState);
}

static int32_t SPI2_DMAReceive(void *data, uint32_t num)
{
    return SPI_DMAReceive(data, num, &SPI2_DMADriverState);
}

static int32_t SPI2_DMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_DMATransfer(data_out, data_in, num, &SPI2_DMADriverState);
}

static uint32_t SPI2_DMAGetCount(void)
{
    return SPI_DMAGetCount(&SPI2_DMADriverState);
}

static int32_t SPI2_DMAControl(uint32_t control, uint32_t arg)
{
    return SPI_DMAControl(control, arg, &SPI2_DMADriverState);
}

static ARM_SPI_STATUS SPI2_DMAGetStatus(void)
{
    return SPI_DMAGetStatus(&SPI2_DMADriverState);
}

ARM_DRIVER_SPI SPI2_Dma_Driver = {SPIx_GetVersion,      SPIx_GetCapabilities, SPI2_DMAInitialize, SPI2_DMAUninitialize,
                                  SPI2_DMAPowerControl, SPI2_DMASend,         SPI2_DMAReceive,    SPI2_DMATransfer,
                                  SPI2_DMAGetCount,     SPI2_DMAControl,      SPI2_DMAGetStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /*  SPI2  */

#if defined(SPI3)

extern uint32_t SPI3_GetFreq(void);
extern void SPI3_InitPins(void);
extern void SPI3_DeinitPins(void);

cmsis_spi_resource_t SPI3_Resource = {SPI3, 3, SPI3_GetFreq};

static cmsis_spi_handle_t SPI3_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi3_interrupt_driver_state")
static cmsis_spi_interrupt_driver_state_t SPI3_InterruptDriverState = {
#else
static cmsis_spi_interrupt_driver_state_t SPI3_InterruptDriverState = {
#endif
    &SPI3_Resource, &SPI3_Handle,
};

static int32_t SPI3_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI3_InitPins();
    return SPI_InterruptInitialize(cb_event, &SPI3_InterruptDriverState);
}

static int32_t SPI3_InterruptUninitialize(void)
{
    SPI3_DeinitPins();
    return SPI_InterruptUninitialize(&SPI3_InterruptDriverState);
}

static int32_t SPI3_InterruptPowerControl(ARM_POWER_STATE state)
{
    return SPI_InterruptPowerControl(state, &SPI3_InterruptDriverState);
}

static int32_t SPI3_InterruptSend(const void *data, uint32_t num)
{
    return SPI_InterruptSend(data, num, &SPI3_InterruptDriverState);
}

static int32_t SPI3_InterruptReceive(void *data, uint32_t num)
{
    return SPI_InterruptReceive(data, num, &SPI3_InterruptDriverState);
}

static int32_t SPI3_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_InterruptTransfer(data_out, data_in, num, &SPI3_InterruptDriverState);
}

static uint32_t SPI3_InterruptGetCount(void)
{
    return SPI_InterruptGetCount(&SPI3_InterruptDriverState);
}

static int32_t SPI3_InterruptControl(uint32_t control, uint32_t arg)
{
    return SPI_InterruptControl(control, arg, &SPI3_InterruptDriverState);
}

static ARM_SPI_STATUS SPI3_InterruptGetStatus(void)
{
    return SPI_InterruptGetStatus(&SPI3_InterruptDriverState);
}

ARM_DRIVER_SPI SPI3_Interrupt_Driver = {SPIx_GetVersion,
                                        SPIx_GetCapabilities,
                                        SPI3_InterruptInitialize,
                                        SPI3_InterruptUninitialize,
                                        SPI3_InterruptPowerControl,
                                        SPI3_InterruptSend,
                                        SPI3_InterruptReceive,
                                        SPI3_InterruptTransfer,
                                        SPI3_InterruptGetCount,
                                        SPI3_InterruptControl,
                                        SPI3_InterruptGetStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
cmsis_spi_dma_resource_t SPI3_DMAResource = {RTE_SPI3_DMA_TX_DMA_BASE, RTE_SPI3_DMA_TX_CH, RTE_SPI3_DMA_RX_DMA_BASE,
                                             RTE_SPI3_DMA_RX_CH};

static cmsis_spi_dma_handle_t SPI3_DmaHandle;
static dma_handle_t SPI3_DmaTxDataHandle;
static dma_handle_t SPI3_DmaRxDataHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi3_dma_driver_state")
static cmsis_spi_dma_driver_state_t SPI3_DMADriverState = {
#else
static cmsis_spi_dma_driver_state_t SPI3_DMADriverState = {
#endif
    &SPI3_Resource, &SPI3_DMAResource, &SPI3_DmaHandle, &SPI3_DmaRxDataHandle, &SPI3_DmaTxDataHandle,
};

static int32_t SPI3_DMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI3_InitPins();
    return SPI_DMAInitialize(cb_event, &SPI3_DMADriverState);
}

static int32_t SPI3_DMAUninitialize(void)
{
    SPI3_DeinitPins();
    return SPI_DMAUninitialize(&SPI3_DMADriverState);
}

static int32_t SPI3_DMAPowerControl(ARM_POWER_STATE state)
{
    return SPI_DMAPowerControl(state, &SPI3_DMADriverState);
}

static int32_t SPI3_DMASend(const void *data, uint32_t num)
{
    return SPI_DMASend(data, num, &SPI3_DMADriverState);
}

static int32_t SPI3_DMAReceive(void *data, uint32_t num)
{
    return SPI_DMAReceive(data, num, &SPI3_DMADriverState);
}

static int32_t SPI3_DMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_DMATransfer(data_out, data_in, num, &SPI3_DMADriverState);
}

static uint32_t SPI3_DMAGetCount(void)
{
    return SPI_DMAGetCount(&SPI3_DMADriverState);
}

static int32_t SPI3_DMAControl(uint32_t control, uint32_t arg)
{
    return SPI_DMAControl(control, arg, &SPI3_DMADriverState);
}

static ARM_SPI_STATUS SPI3_DMAGetStatus(void)
{
    return SPI_DMAGetStatus(&SPI3_DMADriverState);
}

ARM_DRIVER_SPI SPI3_Dma_Driver = {SPIx_GetVersion,      SPIx_GetCapabilities, SPI3_DMAInitialize, SPI3_DMAUninitialize,
                                  SPI3_DMAPowerControl, SPI3_DMASend,         SPI3_DMAReceive,    SPI3_DMATransfer,
                                  SPI3_DMAGetCount,     SPI3_DMAControl,      SPI3_DMAGetStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /*  SPI3  */

#if defined(SPI4)

extern uint32_t SPI4_GetFreq(void);
extern void SPI4_InitPins(void);
extern void SPI4_DeinitPins(void);

cmsis_spi_resource_t SPI4_Resource = {SPI4, 4, SPI4_GetFreq};

static cmsis_spi_handle_t SPI4_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi4_interrupt_driver_state")
static cmsis_spi_interrupt_driver_state_t SPI4_InterruptDriverState = {
#else
static cmsis_spi_interrupt_driver_state_t SPI4_InterruptDriverState = {
#endif
    &SPI4_Resource, &SPI4_Handle,
};

static int32_t SPI4_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI4_InitPins();
    return SPI_InterruptInitialize(cb_event, &SPI4_InterruptDriverState);
}

static int32_t SPI4_InterruptUninitialize(void)
{
    SPI4_DeinitPins();
    return SPI_InterruptUninitialize(&SPI4_InterruptDriverState);
}

static int32_t SPI4_InterruptPowerControl(ARM_POWER_STATE state)
{
    return SPI_InterruptPowerControl(state, &SPI4_InterruptDriverState);
}

static int32_t SPI4_InterruptSend(const void *data, uint32_t num)
{
    return SPI_InterruptSend(data, num, &SPI4_InterruptDriverState);
}

static int32_t SPI4_InterruptReceive(void *data, uint32_t num)
{
    return SPI_InterruptReceive(data, num, &SPI4_InterruptDriverState);
}

static int32_t SPI4_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_InterruptTransfer(data_out, data_in, num, &SPI4_InterruptDriverState);
}

static uint32_t SPI4_InterruptGetCount(void)
{
    return SPI_InterruptGetCount(&SPI4_InterruptDriverState);
}

static int32_t SPI4_InterruptControl(uint32_t control, uint32_t arg)
{
    return SPI_InterruptControl(control, arg, &SPI4_InterruptDriverState);
}

static ARM_SPI_STATUS SPI4_InterruptGetStatus(void)
{
    return SPI_InterruptGetStatus(&SPI4_InterruptDriverState);
}

ARM_DRIVER_SPI SPI4_Interrupt_Driver = {SPIx_GetVersion,
                                        SPIx_GetCapabilities,
                                        SPI4_InterruptInitialize,
                                        SPI4_InterruptUninitialize,
                                        SPI4_InterruptPowerControl,
                                        SPI4_InterruptSend,
                                        SPI4_InterruptReceive,
                                        SPI4_InterruptTransfer,
                                        SPI4_InterruptGetCount,
                                        SPI4_InterruptControl,
                                        SPI4_InterruptGetStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
cmsis_spi_dma_resource_t SPI4_DMAResource = {RTE_SPI4_DMA_TX_DMA_BASE, RTE_SPI4_DMA_TX_CH, RTE_SPI4_DMA_RX_DMA_BASE,
                                             RTE_SPI4_DMA_RX_CH};

static cmsis_spi_dma_handle_t SPI4_DmaHandle;
static dma_handle_t SPI4_DmaTxDataHandle;
static dma_handle_t SPI4_DmaRxDataHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi4_dma_driver_state")
static cmsis_spi_dma_driver_state_t SPI4_DMADriverState = {
#else
static cmsis_spi_dma_driver_state_t SPI4_DMADriverState = {
#endif
    &SPI4_Resource, &SPI4_DMAResource, &SPI4_DmaHandle, &SPI4_DmaRxDataHandle, &SPI4_DmaTxDataHandle,
};

static int32_t SPI4_DMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI4_InitPins();
    return SPI_DMAInitialize(cb_event, &SPI4_DMADriverState);
}

static int32_t SPI4_DMAUninitialize(void)
{
    SPI4_DeinitPins();
    return SPI_DMAUninitialize(&SPI4_DMADriverState);
}

static int32_t SPI4_DMAPowerControl(ARM_POWER_STATE state)
{
    return SPI_DMAPowerControl(state, &SPI4_DMADriverState);
}

static int32_t SPI4_DMASend(const void *data, uint32_t num)
{
    return SPI_DMASend(data, num, &SPI4_DMADriverState);
}

static int32_t SPI4_DMAReceive(void *data, uint32_t num)
{
    return SPI_DMAReceive(data, num, &SPI4_DMADriverState);
}

static int32_t SPI4_DMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_DMATransfer(data_out, data_in, num, &SPI4_DMADriverState);
}

static uint32_t SPI4_DMAGetCount(void)
{
    return SPI_DMAGetCount(&SPI4_DMADriverState);
}

static int32_t SPI4_DMAControl(uint32_t control, uint32_t arg)
{
    return SPI_DMAControl(control, arg, &SPI4_DMADriverState);
}

static ARM_SPI_STATUS SPI4_DMAGetStatus(void)
{
    return SPI_DMAGetStatus(&SPI4_DMADriverState);
}

ARM_DRIVER_SPI SPI4_Dma_Driver = {SPIx_GetVersion,      SPIx_GetCapabilities, SPI4_DMAInitialize, SPI4_DMAUninitialize,
                                  SPI4_DMAPowerControl, SPI4_DMASend,         SPI4_DMAReceive,    SPI4_DMATransfer,
                                  SPI4_DMAGetCount,     SPI4_DMAControl,      SPI4_DMAGetStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /*  SPI4  */

#if defined(SPI5)

extern uint32_t SPI5_GetFreq(void);
extern void SPI5_InitPins(void);
extern void SPI5_DeinitPins(void);

cmsis_spi_resource_t SPI5_Resource = {SPI5, 5, SPI5_GetFreq};

static cmsis_spi_handle_t SPI5_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi5_interrupt_driver_state")
static cmsis_spi_interrupt_driver_state_t SPI5_InterruptDriverState = {
#else
static cmsis_spi_interrupt_driver_state_t SPI5_InterruptDriverState = {
#endif
    &SPI5_Resource, &SPI5_Handle,
};

static int32_t SPI5_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI5_InitPins();
    return SPI_InterruptInitialize(cb_event, &SPI5_InterruptDriverState);
}

static int32_t SPI5_InterruptUninitialize(void)
{
    SPI5_DeinitPins();
    return SPI_InterruptUninitialize(&SPI5_InterruptDriverState);
}

static int32_t SPI5_InterruptPowerControl(ARM_POWER_STATE state)
{
    return SPI_InterruptPowerControl(state, &SPI5_InterruptDriverState);
}

static int32_t SPI5_InterruptSend(const void *data, uint32_t num)
{
    return SPI_InterruptSend(data, num, &SPI5_InterruptDriverState);
}

static int32_t SPI5_InterruptReceive(void *data, uint32_t num)
{
    return SPI_InterruptReceive(data, num, &SPI5_InterruptDriverState);
}

static int32_t SPI5_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_InterruptTransfer(data_out, data_in, num, &SPI5_InterruptDriverState);
}

static uint32_t SPI5_InterruptGetCount(void)
{
    return SPI_InterruptGetCount(&SPI5_InterruptDriverState);
}

static int32_t SPI5_InterruptControl(uint32_t control, uint32_t arg)
{
    return SPI_InterruptControl(control, arg, &SPI5_InterruptDriverState);
}

static ARM_SPI_STATUS SPI5_InterruptGetStatus(void)
{
    return SPI_InterruptGetStatus(&SPI5_InterruptDriverState);
}

ARM_DRIVER_SPI SPI5_Interrupt_Driver = {SPIx_GetVersion,
                                        SPIx_GetCapabilities,
                                        SPI5_InterruptInitialize,
                                        SPI5_InterruptUninitialize,
                                        SPI5_InterruptPowerControl,
                                        SPI5_InterruptSend,
                                        SPI5_InterruptReceive,
                                        SPI5_InterruptTransfer,
                                        SPI5_InterruptGetCount,
                                        SPI5_InterruptControl,
                                        SPI5_InterruptGetStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
cmsis_spi_dma_resource_t SPI5_DMAResource = {RTE_SPI5_DMA_TX_DMA_BASE, RTE_SPI5_DMA_TX_CH, RTE_SPI5_DMA_RX_DMA_BASE,
                                             RTE_SPI5_DMA_RX_CH};

static cmsis_spi_dma_handle_t SPI5_DmaHandle;
static dma_handle_t SPI5_DmaTxDataHandle;
static dma_handle_t SPI5_DmaRxDataHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi5_dma_driver_state")
static cmsis_spi_dma_driver_state_t SPI5_DMADriverState = {
#else
static cmsis_spi_dma_driver_state_t SPI5_DMADriverState = {
#endif
    &SPI5_Resource, &SPI5_DMAResource, &SPI5_DmaHandle, &SPI5_DmaRxDataHandle, &SPI5_DmaTxDataHandle,
};

static int32_t SPI5_DMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI5_InitPins();
    return SPI_DMAInitialize(cb_event, &SPI5_DMADriverState);
}

static int32_t SPI5_DMAUninitialize(void)
{
    SPI5_DeinitPins();
    return SPI_DMAUninitialize(&SPI5_DMADriverState);
}

static int32_t SPI5_DMAPowerControl(ARM_POWER_STATE state)
{
    return SPI_DMAPowerControl(state, &SPI5_DMADriverState);
}

static int32_t SPI5_DMASend(const void *data, uint32_t num)
{
    return SPI_DMASend(data, num, &SPI5_DMADriverState);
}

static int32_t SPI5_DMAReceive(void *data, uint32_t num)
{
    return SPI_DMAReceive(data, num, &SPI5_DMADriverState);
}

static int32_t SPI5_DMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_DMATransfer(data_out, data_in, num, &SPI5_DMADriverState);
}

static uint32_t SPI5_DMAGetCount(void)
{
    return SPI_DMAGetCount(&SPI5_DMADriverState);
}

static int32_t SPI5_DMAControl(uint32_t control, uint32_t arg)
{
    return SPI_DMAControl(control, arg, &SPI5_DMADriverState);
}

static ARM_SPI_STATUS SPI5_DMAGetStatus(void)
{
    return SPI_DMAGetStatus(&SPI5_DMADriverState);
}

ARM_DRIVER_SPI SPI5_Dma_Driver = {SPIx_GetVersion,      SPIx_GetCapabilities, SPI5_DMAInitialize, SPI5_DMAUninitialize,
                                  SPI5_DMAPowerControl, SPI5_DMASend,         SPI5_DMAReceive,    SPI5_DMATransfer,
                                  SPI5_DMAGetCount,     SPI5_DMAControl,      SPI5_DMAGetStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /*  SPI5  */

#if defined(SPI6)

extern uint32_t SPI6_GetFreq(void);
extern void SPI6_InitPins(void);
extern void SPI6_DeinitPins(void);

cmsis_spi_resource_t SPI6_Resource = {SPI6, 6, SPI6_GetFreq};

static cmsis_spi_handle_t SPI6_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi6_interrupt_driver_state")
static cmsis_spi_interrupt_driver_state_t SPI6_InterruptDriverState = {
#else
static cmsis_spi_interrupt_driver_state_t SPI6_InterruptDriverState = {
#endif
    &SPI6_Resource, &SPI6_Handle,
};

static int32_t SPI6_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI6_InitPins();
    return SPI_InterruptInitialize(cb_event, &SPI6_InterruptDriverState);
}

static int32_t SPI6_InterruptUninitialize(void)
{
    SPI6_DeinitPins();
    return SPI_InterruptUninitialize(&SPI6_InterruptDriverState);
}

static int32_t SPI6_InterruptPowerControl(ARM_POWER_STATE state)
{
    return SPI_InterruptPowerControl(state, &SPI6_InterruptDriverState);
}

static int32_t SPI6_InterruptSend(const void *data, uint32_t num)
{
    return SPI_InterruptSend(data, num, &SPI6_InterruptDriverState);
}

static int32_t SPI6_InterruptReceive(void *data, uint32_t num)
{
    return SPI_InterruptReceive(data, num, &SPI6_InterruptDriverState);
}

static int32_t SPI6_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_InterruptTransfer(data_out, data_in, num, &SPI6_InterruptDriverState);
}

static uint32_t SPI6_InterruptGetCount(void)
{
    return SPI_InterruptGetCount(&SPI6_InterruptDriverState);
}

static int32_t SPI6_InterruptControl(uint32_t control, uint32_t arg)
{
    return SPI_InterruptControl(control, arg, &SPI6_InterruptDriverState);
}

static ARM_SPI_STATUS SPI6_InterruptGetStatus(void)
{
    return SPI_InterruptGetStatus(&SPI6_InterruptDriverState);
}

ARM_DRIVER_SPI SPI6_Interrupt_Driver = {SPIx_GetVersion,
                                        SPIx_GetCapabilities,
                                        SPI6_InterruptInitialize,
                                        SPI6_InterruptUninitialize,
                                        SPI6_InterruptPowerControl,
                                        SPI6_InterruptSend,
                                        SPI6_InterruptReceive,
                                        SPI6_InterruptTransfer,
                                        SPI6_InterruptGetCount,
                                        SPI6_InterruptControl,
                                        SPI6_InterruptGetStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
cmsis_spi_dma_resource_t SPI6_DMAResource = {RTE_SPI6_DMA_TX_DMA_BASE, RTE_SPI6_DMA_TX_CH, RTE_SPI6_DMA_RX_DMA_BASE,
                                             RTE_SPI6_DMA_RX_CH};

static cmsis_spi_dma_handle_t SPI6_DmaHandle;
static dma_handle_t SPI6_DmaTxDataHandle;
static dma_handle_t SPI6_DmaRxDataHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi6_dma_driver_state")
static cmsis_spi_dma_driver_state_t SPI6_DMADriverState = {
#else
static cmsis_spi_dma_driver_state_t SPI6_DMADriverState = {
#endif
    &SPI6_Resource, &SPI6_DMAResource, &SPI6_DmaHandle, &SPI6_DmaRxDataHandle, &SPI6_DmaTxDataHandle,
};

static int32_t SPI6_DMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI6_InitPins();
    return SPI_DMAInitialize(cb_event, &SPI6_DMADriverState);
}

static int32_t SPI6_DMAUninitialize(void)
{
    SPI6_DeinitPins();
    return SPI_DMAUninitialize(&SPI6_DMADriverState);
}

static int32_t SPI6_DMAPowerControl(ARM_POWER_STATE state)
{
    return SPI_DMAPowerControl(state, &SPI6_DMADriverState);
}

static int32_t SPI6_DMASend(const void *data, uint32_t num)
{
    return SPI_DMASend(data, num, &SPI6_DMADriverState);
}

static int32_t SPI6_DMAReceive(void *data, uint32_t num)
{
    return SPI_DMAReceive(data, num, &SPI6_DMADriverState);
}

static int32_t SPI6_DMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_DMATransfer(data_out, data_in, num, &SPI6_DMADriverState);
}

static uint32_t SPI6_DMAGetCount(void)
{
    return SPI_DMAGetCount(&SPI6_DMADriverState);
}

static int32_t SPI6_DMAControl(uint32_t control, uint32_t arg)
{
    return SPI_DMAControl(control, arg, &SPI6_DMADriverState);
}

static ARM_SPI_STATUS SPI6_DMAGetStatus(void)
{
    return SPI_DMAGetStatus(&SPI6_DMADriverState);
}

ARM_DRIVER_SPI SPI6_Dma_Driver = {SPIx_GetVersion,      SPIx_GetCapabilities, SPI6_DMAInitialize, SPI6_DMAUninitialize,
                                  SPI6_DMAPowerControl, SPI6_DMASend,         SPI6_DMAReceive,    SPI6_DMATransfer,
                                  SPI6_DMAGetCount,     SPI6_DMAControl,      SPI6_DMAGetStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /*  SPI6  */

#if defined(SPI7)

extern uint32_t SPI7_GetFreq(void);
extern void SPI7_InitPins(void);
extern void SPI7_DeinitPins(void);

cmsis_spi_resource_t SPI7_Resource = {SPI7, 7, SPI7_GetFreq};

static cmsis_spi_handle_t SPI7_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi7_interrupt_driver_state")
static cmsis_spi_interrupt_driver_state_t SPI7_InterruptDriverState = {
#else
static cmsis_spi_interrupt_driver_state_t SPI7_InterruptDriverState = {
#endif
    &SPI7_Resource, &SPI7_Handle,
};

static int32_t SPI7_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI7_InitPins();
    return SPI_InterruptInitialize(cb_event, &SPI7_InterruptDriverState);
}

static int32_t SPI7_InterruptUninitialize(void)
{
    SPI7_DeinitPins();
    return SPI_InterruptUninitialize(&SPI7_InterruptDriverState);
}

static int32_t SPI7_InterruptPowerControl(ARM_POWER_STATE state)
{
    return SPI_InterruptPowerControl(state, &SPI7_InterruptDriverState);
}

static int32_t SPI7_InterruptSend(const void *data, uint32_t num)
{
    return SPI_InterruptSend(data, num, &SPI7_InterruptDriverState);
}

static int32_t SPI7_InterruptReceive(void *data, uint32_t num)
{
    return SPI_InterruptReceive(data, num, &SPI7_InterruptDriverState);
}

static int32_t SPI7_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_InterruptTransfer(data_out, data_in, num, &SPI7_InterruptDriverState);
}

static uint32_t SPI7_InterruptGetCount(void)
{
    return SPI_InterruptGetCount(&SPI7_InterruptDriverState);
}

static int32_t SPI7_InterruptControl(uint32_t control, uint32_t arg)
{
    return SPI_InterruptControl(control, arg, &SPI7_InterruptDriverState);
}

static ARM_SPI_STATUS SPI7_InterruptGetStatus(void)
{
    return SPI_InterruptGetStatus(&SPI7_InterruptDriverState);
}

ARM_DRIVER_SPI SPI7_Interrupt_Driver = {SPIx_GetVersion,
                                        SPIx_GetCapabilities,
                                        SPI7_InterruptInitialize,
                                        SPI7_InterruptUninitialize,
                                        SPI7_InterruptPowerControl,
                                        SPI7_InterruptSend,
                                        SPI7_InterruptReceive,
                                        SPI7_InterruptTransfer,
                                        SPI7_InterruptGetCount,
                                        SPI7_InterruptControl,
                                        SPI7_InterruptGetStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
cmsis_spi_dma_resource_t SPI7_DMAResource = {RTE_SPI7_DMA_TX_DMA_BASE, RTE_SPI7_DMA_TX_CH, RTE_SPI7_DMA_RX_DMA_BASE,
                                             RTE_SPI7_DMA_RX_CH};

static cmsis_spi_dma_handle_t SPI7_DmaHandle;
static dma_handle_t SPI7_DmaTxDataHandle;
static dma_handle_t SPI7_DmaRxDataHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi7_dma_driver_state")
static cmsis_spi_dma_driver_state_t SPI7_DMADriverState = {
#else
static cmsis_spi_dma_driver_state_t SPI7_DMADriverState = {
#endif
    &SPI7_Resource, &SPI7_DMAResource, &SPI7_DmaHandle, &SPI7_DmaRxDataHandle, &SPI7_DmaTxDataHandle,
};

static int32_t SPI7_DMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI7_InitPins();
    return SPI_DMAInitialize(cb_event, &SPI7_DMADriverState);
}

static int32_t SPI7_DMAUninitialize(void)
{
    SPI7_DeinitPins();
    return SPI_DMAUninitialize(&SPI7_DMADriverState);
}

static int32_t SPI7_DMAPowerControl(ARM_POWER_STATE state)
{
    return SPI_DMAPowerControl(state, &SPI7_DMADriverState);
}

static int32_t SPI7_DMASend(const void *data, uint32_t num)
{
    return SPI_DMASend(data, num, &SPI7_DMADriverState);
}

static int32_t SPI7_DMAReceive(void *data, uint32_t num)
{
    return SPI_DMAReceive(data, num, &SPI7_DMADriverState);
}

static int32_t SPI7_DMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_DMATransfer(data_out, data_in, num, &SPI7_DMADriverState);
}

static uint32_t SPI7_DMAGetCount(void)
{
    return SPI_DMAGetCount(&SPI7_DMADriverState);
}

static int32_t SPI7_DMAControl(uint32_t control, uint32_t arg)
{
    return SPI_DMAControl(control, arg, &SPI7_DMADriverState);
}

static ARM_SPI_STATUS SPI7_DMAGetStatus(void)
{
    return SPI_DMAGetStatus(&SPI7_DMADriverState);
}

ARM_DRIVER_SPI SPI7_Dma_Driver = {SPIx_GetVersion,      SPIx_GetCapabilities, SPI7_DMAInitialize, SPI7_DMAUninitialize,
                                  SPI7_DMAPowerControl, SPI7_DMASend,         SPI7_DMAReceive,    SPI7_DMATransfer,
                                  SPI7_DMAGetCount,     SPI7_DMAControl,      SPI7_DMAGetStatus};

#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /*  SPI7  */

#if defined(SPI8)

extern uint32_t SPI8_GetFreq(void);
extern void SPI8_InitPins(void);
extern void SPI8_DeinitPins(void);

cmsis_spi_resource_t SPI8_Resource = {SPI8, 8, SPI8_GetFreq};

static cmsis_spi_handle_t SPI8_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi8_interrupt_driver_state")
static cmsis_spi_interrupt_driver_state_t SPI8_InterruptDriverState = {
#else
static cmsis_spi_interrupt_driver_state_t SPI8_InterruptDriverState = {
#endif
    &SPI8_Resource, &SPI8_Handle,
};

static int32_t SPI8_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI8_InitPins();
    return SPI_InterruptInitialize(cb_event, &SPI8_InterruptDriverState);
}

static int32_t SPI8_InterruptUninitialize(void)
{
    SPI8_DeinitPins();
    return SPI_InterruptUninitialize(&SPI8_InterruptDriverState);
}

static int32_t SPI8_InterruptPowerControl(ARM_POWER_STATE state)
{
    return SPI_InterruptPowerControl(state, &SPI8_InterruptDriverState);
}

static int32_t SPI8_InterruptSend(const void *data, uint32_t num)
{
    return SPI_InterruptSend(data, num, &SPI8_InterruptDriverState);
}

static int32_t SPI8_InterruptReceive(void *data, uint32_t num)
{
    return SPI_InterruptReceive(data, num, &SPI8_InterruptDriverState);
}

static int32_t SPI8_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_InterruptTransfer(data_out, data_in, num, &SPI8_InterruptDriverState);
}

static uint32_t SPI8_InterruptGetCount(void)
{
    return SPI_InterruptGetCount(&SPI8_InterruptDriverState);
}

static int32_t SPI8_InterruptControl(uint32_t control, uint32_t arg)
{
    return SPI_InterruptControl(control, arg, &SPI8_InterruptDriverState);
}

static ARM_SPI_STATUS SPI8_InterruptGetStatus(void)
{
    return SPI_InterruptGetStatus(&SPI8_InterruptDriverState);
}

ARM_DRIVER_SPI SPI8_Interrupt_Driver = {SPIx_GetVersion,
                                        SPIx_GetCapabilities,
                                        SPI8_InterruptInitialize,
                                        SPI8_InterruptUninitialize,
                                        SPI8_InterruptPowerControl,
                                        SPI8_InterruptSend,
                                        SPI8_InterruptReceive,
                                        SPI8_InterruptTransfer,
                                        SPI8_InterruptGetCount,
                                        SPI8_InterruptControl,
                                        SPI8_InterruptGetStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
cmsis_spi_dma_resource_t SPI8_DMAResource = {RTE_SPI8_DMA_TX_DMA_BASE, RTE_SPI8_DMA_TX_CH, RTE_SPI8_DMA_RX_DMA_BASE,
                                             RTE_SPI8_DMA_RX_CH};

static cmsis_spi_dma_handle_t SPI8_DmaHandle;
static dma_handle_t SPI8_DmaTxDataHandle;
static dma_handle_t SPI8_DmaRxDataHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi8_dma_driver_state")
static cmsis_spi_dma_driver_state_t SPI8_DMADriverState = {
#else
static cmsis_spi_dma_driver_state_t SPI8_DMADriverState = {
#endif
    &SPI8_Resource, &SPI8_DMAResource, &SPI8_DmaHandle, &SPI8_DmaRxDataHandle, &SPI8_DmaTxDataHandle,
};

static int32_t SPI8_DMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI8_InitPins();
    return SPI_DMAInitialize(cb_event, &SPI8_DMADriverState);
}

static int32_t SPI8_DMAUninitialize(void)
{
    SPI8_DeinitPins();
    return SPI_DMAUninitialize(&SPI8_DMADriverState);
}

static int32_t SPI8_DMAPowerControl(ARM_POWER_STATE state)
{
    return SPI_DMAPowerControl(state, &SPI8_DMADriverState);
}

static int32_t SPI8_DMASend(const void *data, uint32_t num)
{
    return SPI_DMASend(data, num, &SPI8_DMADriverState);
}

static int32_t SPI8_DMAReceive(void *data, uint32_t num)
{
    return SPI_DMAReceive(data, num, &SPI8_DMADriverState);
}

static int32_t SPI8_DMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_DMATransfer(data_out, data_in, num, &SPI8_DMADriverState);
}

static uint32_t SPI8_DMAGetCount(void)
{
    return SPI_DMAGetCount(&SPI8_DMADriverState);
}

static int32_t SPI8_DMAControl(uint32_t control, uint32_t arg)
{
    return SPI_DMAControl(control, arg, &SPI8_DMADriverState);
}

static ARM_SPI_STATUS SPI8_DMAGetStatus(void)
{
    return SPI_DMAGetStatus(&SPI8_DMADriverState);
}

ARM_DRIVER_SPI SPI8_Dma_Driver = {SPIx_GetVersion,      SPIx_GetCapabilities, SPI8_DMAInitialize, SPI8_DMAUninitialize,
                                  SPI8_DMAPowerControl, SPI8_DMASend,         SPI8_DMAReceive,    SPI8_DMATransfer,
                                  SPI8_DMAGetCount,     SPI8_DMAControl,      SPI8_DMAGetStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /*  SPI8  */

#if defined(SPI9)

extern uint32_t SPI9_GetFreq(void);
extern void SPI9_InitPins(void);
extern void SPI9_DeinitPins(void);

cmsis_spi_resource_t SPI9_Resource = {SPI9, 9, SPI9_GetFreq};

static cmsis_spi_handle_t SPI9_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi9_interrupt_driver_state")
static cmsis_spi_interrupt_driver_state_t SPI9_InterruptDriverState = {
#else
static cmsis_spi_interrupt_driver_state_t SPI9_InterruptDriverState = {
#endif
    &SPI9_Resource, &SPI9_Handle,
};

static int32_t SPI9_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI9_InitPins();
    return SPI_InterruptInitialize(cb_event, &SPI9_InterruptDriverState);
}

static int32_t SPI9_InterruptUninitialize(void)
{
    SPI9_DeinitPins();
    return SPI_InterruptUninitialize(&SPI9_InterruptDriverState);
}

static int32_t SPI9_InterruptPowerControl(ARM_POWER_STATE state)
{
    return SPI_InterruptPowerControl(state, &SPI9_InterruptDriverState);
}

static int32_t SPI9_InterruptSend(const void *data, uint32_t num)
{
    return SPI_InterruptSend(data, num, &SPI9_InterruptDriverState);
}

static int32_t SPI9_InterruptReceive(void *data, uint32_t num)
{
    return SPI_InterruptReceive(data, num, &SPI9_InterruptDriverState);
}

static int32_t SPI9_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_InterruptTransfer(data_out, data_in, num, &SPI9_InterruptDriverState);
}

static uint32_t SPI9_InterruptGetCount(void)
{
    return SPI_InterruptGetCount(&SPI9_InterruptDriverState);
}

static int32_t SPI9_InterruptControl(uint32_t control, uint32_t arg)
{
    return SPI_InterruptControl(control, arg, &SPI9_InterruptDriverState);
}

static ARM_SPI_STATUS SPI9_InterruptGetStatus(void)
{
    return SPI_InterruptGetStatus(&SPI9_InterruptDriverState);
}

ARM_DRIVER_SPI SPI9_Interrupt_Driver = {SPIx_GetVersion,
                                        SPIx_GetCapabilities,
                                        SPI9_InterruptInitialize,
                                        SPI9_InterruptUninitialize,
                                        SPI9_InterruptPowerControl,
                                        SPI9_InterruptSend,
                                        SPI9_InterruptReceive,
                                        SPI9_InterruptTransfer,
                                        SPI9_InterruptGetCount,
                                        SPI9_InterruptControl,
                                        SPI9_InterruptGetStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
cmsis_spi_dma_resource_t SPI9_DMAResource = {RTE_SPI9_DMA_TX_DMA_BASE, RTE_SPI9_DMA_TX_CH, RTE_SPI9_DMA_RX_DMA_BASE,
                                             RTE_SPI9_DMA_RX_CH};

static cmsis_spi_dma_handle_t SPI9_DmaHandle;
static dma_handle_t SPI9_DmaTxDataHandle;
static dma_handle_t SPI9_DmaRxDataHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("spi9_dma_driver_state")
static cmsis_spi_dma_driver_state_t SPI9_DMADriverState = {
#else
static cmsis_spi_dma_driver_state_t SPI9_DMADriverState = {
#endif
    &SPI9_Resource, &SPI9_DMAResource, &SPI9_DmaHandle, &SPI9_DmaRxDataHandle, &SPI9_DmaTxDataHandle,
};

static int32_t SPI9_DMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI9_InitPins();
    return SPI_DMAInitialize(cb_event, &SPI9_DMADriverState);
}

static int32_t SPI9_DMAUninitialize(void)
{
    SPI9_DeinitPins();
    return SPI_DMAUninitialize(&SPI9_DMADriverState);
}

static int32_t SPI9_DMAPowerControl(ARM_POWER_STATE state)
{
    return SPI_DMAPowerControl(state, &SPI9_DMADriverState);
}

static int32_t SPI9_DMASend(const void *data, uint32_t num)
{
    return SPI_DMASend(data, num, &SPI9_DMADriverState);
}

static int32_t SPI9_DMAReceive(void *data, uint32_t num)
{
    return SPI_DMAReceive(data, num, &SPI9_DMADriverState);
}

static int32_t SPI9_DMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_DMATransfer(data_out, data_in, num, &SPI9_DMADriverState);
}

static uint32_t SPI9_DMAGetCount(void)
{
    return SPI_DMAGetCount(&SPI9_DMADriverState);
}

static int32_t SPI9_DMAControl(uint32_t control, uint32_t arg)
{
    return SPI_DMAControl(control, arg, &SPI9_DMADriverState);
}

static ARM_SPI_STATUS SPI9_DMAGetStatus(void)
{
    return SPI_DMAGetStatus(&SPI9_DMADriverState);
}

ARM_DRIVER_SPI SPI9_Dma_Driver = {SPIx_GetVersion,      SPIx_GetCapabilities, SPI9_DMAInitialize, SPI9_DMAUninitialize,
                                  SPI9_DMAPowerControl, SPI9_DMASend,         SPI9_DMAReceive,    SPI9_DMATransfer,
                                  SPI9_DMAGetCount,     SPI9_DMAControl,      SPI9_DMAGetStatus};

#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /*  SPI9  */
