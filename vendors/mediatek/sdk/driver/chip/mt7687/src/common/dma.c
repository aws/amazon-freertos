/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "hal_platform.h"
#ifdef HAL_GDMA_MODULE_ENABLED

#include "mt7687.h"
#include "nvic.h"
#include "type_def.h"
#include "dma_hw.h"
#include "dma_sw.h"
#include "_mtk_hal_debug.h"
#include "hal_nvic.h"



/*
 * Function prototype.
 */
void DMA_ERROR_LISR(void);

/*
 * Global variable.
 */
VOID_FUNC DMA_LISR_FUNC[DMA_MAX_CHANNEL] = {
    DMA_ERROR_LISR
};

VOID_FUNC DMA_VFIFO_LISR_FUNC[DMA_VFIFO_CH_E - DMA_VFIFO_CH_S + 1] = {
    DMA_ERROR_LISR
};

VOID_FUNC DMA_VFIFO_TO_LISR_FUNC[DMA_VFIFO_CH_E - DMA_VFIFO_CH_S + 1] = {
    DMA_ERROR_LISR
};

static kal_uint32 DMA_Channel_Status = 0xffff;
static bool DMA_used_channel[DMA_MAX_CHANNEL] = {false};
static kal_uint32 vfifo_init_count = 0;

DMA_Master DMA_Owner[DMA_MAX_CHANNEL];

void DMA_Stop(uint8_t channel)
{
    DMA_Stop_Now(channel);
    DMA_ACKI(channel);
}

void DMA_Run(uint8_t channel)
{
    DMA_Stop(channel);
    DMA_Start(channel);
}


kal_uint8 DMA_GetChannel(DMA_Master DMA_CODE)
{
    kal_uint8 index;
    kal_uint8 handle = 0xff;
    unsigned long flags;


    local_irq_save(flags);

    for (index = DMA_MAX_FULL_CHANNEL + 1; index <= DMA_MAX_CHANNEL; index++) {
        if ((DMA_Channel_Status & (0x01 << index)) != 0) {

            handle = index;

            DMA_Channel_Status &= ~(0x01 << index);

            break;
        }
    }
    local_irq_restore(flags);



    DMA_Owner[index - 1] = DMA_CODE;

    return handle;
}

void DMA_FreeChannel(uint8_t handle)
{
    unsigned long flags;

    local_irq_save(flags);


    DMA_Channel_Status |= (0x01 << handle);

    DMA_Stop(handle);

    DMA_Owner[handle - 1] = DMA_IDLE;

    local_irq_restore(flags);

}

uint8_t DMA_FullSize_CheckIdleChannel(DMA_Master DMA_CODE, uint8_t channel)
{
    kal_uint8 index;
    kal_uint8 handle = 0xff;
    unsigned long flags;

    local_irq_save(flags);

    index = channel + 1 ;
    if (index > DMA_MAX_FULL_CHANNEL) {
        return handle;
    }

    if ((DMA_Channel_Status & (0x01 << index)) != 0) {

        handle = index;

        DMA_Channel_Status &= ~(0x01 << index);

    }


    local_irq_restore(flags);


    DMA_Owner[index - 1] = DMA_CODE;
    return handle;
}

void DMA_FullSize_FreeChannel(uint8_t handle)
{
    unsigned long flags;

    local_irq_save(flags);
    DMA_Channel_Status |= (0x01 << handle);

    DMA_Stop(handle);

    DMA_Owner[handle - 1] = DMA_IDLE;

    local_irq_restore(flags);

}

void DMA_Config_Internal(uint8_t    dma_no,
                         DMA_INPUT  *dma_menu,
                         bool   fullsize,
                         bool   b2w, uint8_t limit,
                         bool   start)
{
    kal_uint32 dma_con = 0;
    kal_uint32 temp;

    DMA_Clock_Enable(dma_no);

    if (DMA_CheckRunStat(dma_no)) {
        ASSERT(0);
    }

    if (DMA_CheckITStat(dma_no)) {
        ASSERT(0);
    }

    ASSERT(DMA_Owner[dma_no - 1] != DMA_IDLE);

    if (fullsize == KAL_TRUE) {
        ASSERT(b2w == KAL_FALSE);
    }

    ASSERT(dma_menu->count <= 0xffff);

    DMA_Stop(dma_no);

    switch (dma_menu->type) {

        case  DMA_HWTX:

//    #if (MTK_DEV_DMA_FIX_CHANNEL == 0) /*Comply with MT6260's HW design to fix DMA channel, Fangming, 20120809*/
            if (fullsize == KAL_TRUE)

            {
                temp = (kal_uint32)(((DMA_FULLSIZE_HWMENU *)dma_menu->menu)->master);
            } else

            {
                temp = (kal_uint32)(((DMA_HWMENU *)dma_menu->menu)->master);
            }

            dma_con |= (kal_uint32)(temp << 20);
//    #endif

            dma_con |= DMA_CON_TXCONTRL;

            dma_con |= DMA_CON_DRQ;

            if (fullsize == KAL_TRUE) {

                DRV_WriteReg32(DMA_SRC(dma_no), ((DMA_FULLSIZE_HWMENU *)dma_menu->menu)->source);

                DRV_WriteReg32(DMA_DST(dma_no), ((DMA_FULLSIZE_HWMENU *)dma_menu->menu)->destination);
            } else {

//        #if (MTK_DEV_DMA_SHORT_SIZE == 1) /*Comply with MT6260's HW design to decrease size, Fangming, 20120724*/
//            DRV_WriteReg32(DMA_PGMADDR(dma_no), (((DMA_HWMENU *)dma_menu->menu)->addr) & DMA_SIZE_24BIT_MASK);
//        #else
                DRV_WriteReg32(DMA_PGMADDR(dma_no), ((DMA_HWMENU *)dma_menu->menu)->addr);
//        #endif

                if (b2w == KAL_TRUE) {

                    dma_con |= DMA_CON_B2W;
                }
            }

            break;

        case  DMA_HWRX:

//    #if (MTK_DEV_DMA_FIX_CHANNEL == 0) /*Comply with MT6260's HW design to fix DMA channel, Fangming, 20120809*/
            if (fullsize == KAL_TRUE)

            {
                dma_con |= (kal_uint32)(((DMA_FULLSIZE_HWMENU *)dma_menu->menu)->master) << 20;
            } else

            {
                dma_con |= (kal_uint32)(((DMA_HWMENU *)dma_menu->menu)->master) << 20;
            }
//    #endif

            dma_con |= DMA_CON_RXCONTRL;

            dma_con |= DMA_CON_DRQ;

            if (fullsize == KAL_TRUE) {

                DRV_WriteReg32(DMA_SRC(dma_no), ((DMA_FULLSIZE_HWMENU *)dma_menu->menu)->source);

                DRV_WriteReg32(DMA_DST(dma_no), ((DMA_FULLSIZE_HWMENU *)dma_menu->menu)->destination);
            } else {

//        #if (MTK_DEV_DMA_SHORT_SIZE == 1) /*Comply with MT6260's HW design to decrease size, Fangming, 20120724*/
//            DRV_WriteReg32(DMA_PGMADDR(dma_no), (((DMA_HWMENU *)dma_menu->menu)->addr) & DMA_SIZE_24BIT_MASK);
//        #else
                DRV_WriteReg32(DMA_PGMADDR(dma_no), ((DMA_HWMENU *)dma_menu->menu)->addr);
//        #endif

                if (b2w == KAL_TRUE) {

                    dma_con |= DMA_CON_B2W;
                }
            }

            break;

        case DMA_SWCOPY:

            DRV_WriteReg32(DMA_SRC(dma_no), ((DMA_SWCOPYMENU *)dma_menu->menu)->srcaddr);

            DRV_WriteReg32(DMA_DST(dma_no), ((DMA_SWCOPYMENU *)dma_menu->menu)->dstaddr);

            dma_con = DMA_CON_SWCOPY;

            break;

        case DMA_HWTX_RINGBUFF:

//    #if (MTK_DEV_DMA_FIX_CHANNEL == 0) /*Comply with MT6260's HW design to fix DMA channel, Fangming, 20120809*/
            if (fullsize == KAL_TRUE)

            {
                dma_con |= ((kal_uint32)((DMA_FULLSIZE_HWRINGBUFF_MENU *)dma_menu->menu)->master) << 20;
            } else

            {
                dma_con |= ((kal_uint32)((DMA_HWRINGBUFF_MENU *)dma_menu->menu)->master) << 20;
            }
//    #endif

            dma_con |= DMA_CON_TXCONTRL;

            dma_con |= DMA_CON_DRQ;

            dma_con |= DMA_CON_WPEN;

            if (fullsize == KAL_TRUE) {

                DRV_WriteReg32(DMA_WPPT(dma_no), ((DMA_FULLSIZE_HWRINGBUFF_MENU *)dma_menu->menu)->WPPT);

//        #if (MTK_DEV_DMA_SHORT_SIZE == 1) /*Comply with MT6260's HW design to decrease size, Fangming, 20120724*/
//            DRV_WriteReg32(DMA_WPTO(dma_no), (kal_uint32)(((DMA_FULLSIZE_HWRINGBUFF_MENU *)dma_menu->menu)->WPTO)& DMA_SIZE_24BIT_MASK);
//        #else
                DRV_WriteReg32(DMA_WPTO(dma_no), ((DMA_FULLSIZE_HWRINGBUFF_MENU *)dma_menu->menu)->WPTO);
//        #endif

                DRV_WriteReg32(DMA_SRC(dma_no), ((DMA_FULLSIZE_HWRINGBUFF_MENU *)dma_menu->menu)->source);

                DRV_WriteReg32(DMA_DST(dma_no), ((DMA_FULLSIZE_HWRINGBUFF_MENU *)dma_menu->menu)->destination);
            } else {

                DRV_WriteReg32(DMA_WPPT(dma_no), ((DMA_HWRINGBUFF_MENU *)dma_menu->menu)->WPPT);

//        #if (MTK_DEV_DMA_SHORT_SIZE == 1)/*Comply with MT6260's HW design to decrease size, Fangming, 20120724*/
//            DRV_WriteReg32(DMA_WPTO(dma_no), (kal_uint32)(((DMA_HWRINGBUFF_MENU *)dma_menu->menu)->WPTO) & DMA_SIZE_24BIT_MASK);

//            DRV_WriteReg32(DMA_PGMADDR(dma_no), (((DMA_HWRINGBUFF_MENU *)dma_menu->menu)->addr) & DMA_SIZE_24BIT_MASK);
//        #else
                DRV_WriteReg32(DMA_WPTO(dma_no), ((DMA_HWRINGBUFF_MENU *)dma_menu->menu)->WPTO);

                DRV_WriteReg32(DMA_PGMADDR(dma_no), ((DMA_HWRINGBUFF_MENU *)dma_menu->menu)->addr);
//        #endif

                if (b2w == KAL_TRUE) {

                    dma_con |= DMA_CON_B2W;
                }
            }

            break;

        case DMA_HWRX_RINGBUFF:

//    #if (MTK_DEV_DMA_FIX_CHANNEL == 0) /*Comply with MT6260's HW design to fix DMA channel, Fangming, 20120809*/
            if (fullsize == KAL_TRUE)

            {
                dma_con |= ((kal_uint32)((DMA_FULLSIZE_HWRINGBUFF_MENU *)dma_menu->menu)->master) << 20;
            } else

            {
                dma_con |= ((kal_uint32)((DMA_HWRINGBUFF_MENU *)dma_menu->menu)->master) << 20;
            }
//    #endif

            dma_con |= DMA_CON_RXCONTRL;

            dma_con |= DMA_CON_DRQ;

            dma_con |= DMA_CON_WPSD;

            dma_con |= DMA_CON_WPEN;

            if (fullsize == KAL_TRUE) {

                DRV_WriteReg32(DMA_WPPT(dma_no), ((DMA_FULLSIZE_HWRINGBUFF_MENU *)dma_menu->menu)->WPPT);

//        #if (MTK_DEV_DMA_SHORT_SIZE == 1)/*Comply with MT6260's HW design to decrease size, Fangming, 20120724*/
//            DRV_WriteReg32(DMA_WPTO(dma_no), (kal_uint32)(((DMA_FULLSIZE_HWRINGBUFF_MENU *)dma_menu->menu)->WPTO) & DMA_SIZE_24BIT_MASK);
//        #else
                DRV_WriteReg32(DMA_WPTO(dma_no), ((DMA_FULLSIZE_HWRINGBUFF_MENU *)dma_menu->menu)->WPTO);
//        #endif

                DRV_WriteReg32(DMA_SRC(dma_no), ((DMA_FULLSIZE_HWRINGBUFF_MENU *)dma_menu->menu)->source);

                DRV_WriteReg32(DMA_DST(dma_no), ((DMA_FULLSIZE_HWRINGBUFF_MENU *)dma_menu->menu)->destination);
            } else {

                DRV_WriteReg32(DMA_WPPT(dma_no), ((DMA_HWRINGBUFF_MENU *)dma_menu->menu)->WPPT);

//        #if (MTK_DEV_DMA_SHORT_SIZE == 1)/*Comply with MT6260's HW design to decrease size, Fangming, 20120724*/
//            DRV_WriteReg32(DMA_WPTO(dma_no), (kal_uint32)(((DMA_HWRINGBUFF_MENU *)dma_menu->menu)->WPTO) & DMA_SIZE_24BIT_MASK);

//            DRV_WriteReg32(DMA_PGMADDR(dma_no), (((DMA_HWRINGBUFF_MENU *)dma_menu->menu)->addr) & DMA_SIZE_24BIT_MASK);
//        #else
                DRV_WriteReg32(DMA_WPTO(dma_no), ((DMA_HWRINGBUFF_MENU *)dma_menu->menu)->WPTO);

                DRV_WriteReg32(DMA_PGMADDR(dma_no), ((DMA_HWRINGBUFF_MENU *)dma_menu->menu)->addr);
//        #endif

                if (b2w == KAL_TRUE) {

                    dma_con |= DMA_CON_B2W;
                }
            }

            break;

        default:

            ASSERT(0);


    }

    switch (dma_menu->size) {

        case DMA_BYTE:

            if (dma_menu->type != DMA_SWCOPY) {

                if ((((DMA_TMODE *)(dma_menu->menu))->burst_mode == KAL_TRUE) &&
                        (((DMA_TMODE *)(dma_menu->menu))->cycle & 0xf0))

                {
                    dma_con |= DMA_CON_BURST_16BEAT;
                }

                else if ((((DMA_TMODE *)(dma_menu->menu))->burst_mode == KAL_TRUE) &&
                         (((DMA_TMODE *)(dma_menu->menu))->cycle & 0x08))

                {
                    dma_con |= DMA_CON_BURST_8BEAT;
                }

                else if ((((DMA_TMODE *)(dma_menu->menu))->burst_mode == KAL_TRUE) &&
                         (((DMA_TMODE *)(dma_menu->menu))->cycle & 0x04))

                {
                    dma_con |= DMA_CON_BURST_4BEAT;
                }
            }

            break;

        case DMA_SHORT:

            if (dma_menu->type != DMA_SWCOPY) {

                if ((((DMA_TMODE *)(dma_menu->menu))->burst_mode == KAL_TRUE) &&
                        (((DMA_TMODE *)(dma_menu->menu))->cycle & 0xf8))

                {
                    dma_con |= DMA_CON_BURST_8BEAT;
                }

                else if ((((DMA_TMODE *)(dma_menu->menu))->burst_mode == KAL_TRUE) &&
                         (((DMA_TMODE *)(dma_menu->menu))->cycle & 0x04))

                {
                    dma_con |= DMA_CON_BURST_4BEAT;
                }
            }

            dma_con |= DMA_CON_SIZE_SHORT;

            break;

        case DMA_LONG:

            if (dma_menu->type != DMA_SWCOPY) {

                if ((((DMA_TMODE *)(dma_menu->menu))->burst_mode == KAL_TRUE) &&
                        (((DMA_TMODE *)(dma_menu->menu))->cycle & 0xfc))

                {
                    dma_con |= DMA_CON_BURST_4BEAT;
                }
            }

            dma_con |= DMA_CON_SIZE_LONG;

            break;

        default:

            ASSERT(0);


    }

    if (dma_menu->callback != NULL) {

        dma_con |= DMA_CON_ITEN;

        DMA_LISR_FUNC[dma_no - 1] = dma_menu->callback;
    }

    DRV_WriteReg32(DMA_CON(dma_no), dma_con);

    DRV_WriteReg32(DMA_COUNT(dma_no), (kal_uint32)dma_menu->count);

    if (start) {

        DMA_Start(dma_no);
    }
}


void DMA_ERROR_LISR(void)
{
    while (1);
}

int DMA_Register(uint8_t dma_ch, VOID_FUNC handler)
{
    if (dma_ch < 1 || dma_ch > DMA_MAX_CHANNEL) {
        printf("%s: ERROR! Register invalid DMA CH(%d)\n", __FUNCTION__, dma_ch);
        return -1;
    }

    DMA_LISR_FUNC[dma_ch - 1] = handler;

    return 0;
}

int DMA_UnRegister(uint8_t dma_ch)
{
    if (dma_ch < 1 || dma_ch > DMA_MAX_CHANNEL) {
        printf("%s: ERROR! Unregister invalid DMA CH(%d)\n", __FUNCTION__, dma_ch);
        return -1;
    }
    DMA_LISR_FUNC[dma_ch - 1] = DMA_ERROR_LISR;
    return 0;
}

void DMA_LISR(hal_nvic_irq_t irq_number)
{
    kal_uint8 index;
    kal_uint32 con;
    /* mask DMA intr. */

    NVIC_DisableIRQ((IRQn_Type)CM4_DMA_IRQ);

    /* find the channel of interrupt and acknowledge it */
    for (index = 0; index < DMA_MAX_CHANNEL; index++) {

        if (DRV_Reg32(DMA_INTSTA(index + 1)) & DMA_INTSTA_BIT) {

            DMA_Stop((kal_uint8)(index + 1));

            DMA_ACKI(index + 1);
            // James modify for new config clock control
            while (DMA_CheckRunStat(index + 1));
            DMA_Clock_Disable(index + 1);
            DMA_LISR_FUNC[index]();

        }
    }

    for (index = DMA_VFIFO_CH_S; index <= DMA_VFIFO_CH_E; index++) {

        con = DRV_Reg32(DMA_CON(index));

        if (DRV_Reg32(DMA_INTSTA(index)) & DMA_INTSTA_BIT) {

            switch (con >> 20) {

                case DMA_CON_MASTER_I2S_TX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_I2S_TX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_I2S_TX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_I2S_RX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_I2S_RX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_I2S_RX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_UART1TX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_UART1TX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_UART1TX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_UART1RX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_UART1RX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_UART1RX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_UART2TX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_UART2TX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_UART2TX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_UART2RX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_UART2RX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_UART2RX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_BTIF_TX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_BTIF_TX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_BTIF_TX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_BTIF_RX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_BTIF_RX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_BTIF_RX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_EP2O_RX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_USB_EP2O_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_USB_EP2O_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_EP3I_TX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_USB_EP3I_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_USB_EP3I_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_EP3O_RX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_USB_EP3O_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_USB_EP3O_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_EP4I_TX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_USB_EP4I_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_USB_EP4I_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_EP4O_RX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_USB_EP4O_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_USB_EP4O_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_ADC_RX:
                    if (DMA_VFIFO_LISR_FUNC[VDMA_ADC_RX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_LISR_FUNC[VDMA_ADC_RX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;

            }
            DMA_ACKI(index);
        }
    }

    for (index = DMA_VFIFO_CH_S; index <= DMA_VFIFO_CH_E; index++) {

        con = DRV_Reg32(DMA_CON(index));

        if (DRV_Reg32(DMA_INTSTA(index)) & DMA_TOINTSTA_BIT) {

            switch (con >> 20) {

                case DMA_CON_MASTER_I2S_TX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_I2S_TX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_I2S_TX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_I2S_RX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_I2S_RX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_I2S_RX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_UART1TX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_UART1TX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_UART1TX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_UART1RX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_UART1RX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_UART1RX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_UART2TX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_UART2TX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_UART2TX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_UART2RX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_UART2RX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_UART2RX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_BTIF_TX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_BTIF_TX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_BTIF_TX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_BTIF_RX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_BTIF_RX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_BTIF_RX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_EP2O_RX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_USB_EP2O_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_USB_EP2O_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_EP3I_TX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_USB_EP3I_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_USB_EP3I_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_EP3O_RX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_USB_EP3O_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_USB_EP3O_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_EP4I_TX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_USB_EP4I_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_USB_EP4I_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_EP4O_RX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_USB_EP4O_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_USB_EP4O_CH - DMA_VFIFO_CH_S]();
                    }
                    break;
                case DMA_CON_MASTER_ADC_RX:
                    if (DMA_VFIFO_TO_LISR_FUNC[VDMA_ADC_RX_CH - DMA_VFIFO_CH_S] != NULL) {
                        DMA_VFIFO_TO_LISR_FUNC[VDMA_ADC_RX_CH - DMA_VFIFO_CH_S]();
                    }
                    break;

            }
            DMA_ACKTOI(index);
        }
    }

    NVIC_EnableIRQ((IRQn_Type)CM4_DMA_IRQ);
}


void DMA_Init(void)
{
    kal_uint8 index;

    /* Allen 20060306 , Reset DMA_Channel_Status for HCI_RESET */
    //DMA_Channel_Status = 0xffff;

    for (index = 0; index < DMA_MAX_CHANNEL; index++) {
        if ((DMA_Channel_Status & (0x01 << index)) != 0) {
            DMA_Owner[index] = DMA_IDLE;
        }
    }

    // James modify for new config clock control
    for (index = 0; index < DMA_MAX_CHANNEL; index++) {

        DMA_Clock_Enable(index + 1);
    }

    //NVIC_Register(CM4_DMA_IRQ, DMA_LISR);
    hal_nvic_register_isr_handler(CM4_DMA_IRQ, DMA_LISR);
    NVIC_SetPriority((IRQn_Type)CM4_DMA_IRQ, CM4_DMA_PRI);
    NVIC_EnableIRQ((IRQn_Type)CM4_DMA_IRQ);
}

uint8_t  dma_set_channel_busy(uint8_t channel)
{
    uint8_t channel_index = 0xff;
    unsigned long flags;



    uint8_t index = channel + 1;

    local_irq_save(flags);

    if (DMA_used_channel[index] == false) {
        DMA_used_channel[index] = true;
        DMA_Clock_Enable(index);
    } else {
        return channel_index;
    }
    local_irq_restore(flags);

    hal_nvic_register_isr_handler(CM4_DMA_IRQ, DMA_LISR);
    NVIC_SetPriority((IRQn_Type)CM4_DMA_IRQ, CM4_DMA_PRI);
    NVIC_EnableIRQ((IRQn_Type)CM4_DMA_IRQ);
    return channel;

}

uint8_t  dma_set_channel_idle(uint8_t channel)
{
    unsigned long flags;
    uint8_t index = channel + 1;
    DMA_Stop(index);
    local_irq_save(flags);
    DMA_used_channel[index] = false;
    DMA_Channel_Status |= (0x01 << index);
    DMA_Owner[index - 1] = DMA_IDLE;
    local_irq_restore(flags);

    return channel;

}

void DMA_Vfifo_restore(void)
{
    vfifo_init_count = 0;
}

void DMA_Vfifo_init(void)
{
    kal_uint32 dma_con = 0;

	/*just init once is enough, avoid user A  call dma_vfifo_init which would modify user B' setting*/
  if(vfifo_init_count == 0) {
    DMA_Clock_Enable(VDMA_I2S_TX_CH);
    dma_con = (DMA_CON_MASTER_I2S_TX << 20) | DMA_CON_BURST_16BEAT | DMA_CON_DRQ | DMA_CON_SIZE_LONG;
    DRV_WriteReg32(DMA_CON(VDMA_I2S_TX_CH), dma_con);

    DMA_Clock_Enable(VDMA_I2S_RX_CH);
    dma_con = (DMA_CON_MASTER_I2S_RX << 20) | DMA_CON_DIR | DMA_CON_BURST_16BEAT | DMA_CON_DRQ | DMA_CON_SIZE_LONG;
    DRV_WriteReg32(DMA_CON(VDMA_I2S_RX_CH), dma_con);

    DMA_Clock_Enable(VDMA_UART1TX_CH);
    dma_con = (DMA_CON_MASTER_UART1TX << 20) | DMA_CON_BURST_SINGLE | DMA_CON_DRQ | DMA_CON_SIZE_BYTE;
    DRV_WriteReg32(DMA_CON(VDMA_UART1TX_CH), dma_con);

    DMA_Clock_Enable(VDMA_UART1RX_CH);
    dma_con = (DMA_CON_MASTER_UART1RX << 20) | DMA_CON_DIR | DMA_CON_ITEN | DMA_CON_TOEN | DMA_CON_BURST_SINGLE | DMA_CON_DRQ | DMA_CON_SIZE_BYTE;
    DRV_WriteReg32(DMA_CON(VDMA_UART1RX_CH), dma_con);

    DMA_Clock_Enable(VDMA_UART2TX_CH);
    dma_con = (DMA_CON_MASTER_UART2TX << 20) | DMA_CON_BURST_SINGLE | DMA_CON_DRQ | DMA_CON_SIZE_BYTE;
    DRV_WriteReg32(DMA_CON(VDMA_UART2TX_CH), dma_con);

    DMA_Clock_Enable(VDMA_UART2RX_CH);
    dma_con = (DMA_CON_MASTER_UART2RX << 20) | DMA_CON_DIR | DMA_CON_ITEN | DMA_CON_TOEN | DMA_CON_BURST_SINGLE | DMA_CON_DRQ | DMA_CON_SIZE_BYTE;
    DRV_WriteReg32(DMA_CON(VDMA_UART2RX_CH), dma_con);

    DMA_Clock_Enable(VDMA_BTIF_TX_CH);
    dma_con = (DMA_CON_MASTER_BTIF_TX << 20) | DMA_CON_DRQ;
    DRV_WriteReg32(DMA_CON(VDMA_BTIF_TX_CH), dma_con);

    DMA_Clock_Enable(VDMA_BTIF_RX_CH);
    dma_con = (DMA_CON_MASTER_BTIF_RX << 20) | DMA_CON_DIR | DMA_CON_DRQ | DMA_CON_ITEN | DMA_CON_TOEN;
    DRV_WriteReg32(DMA_CON(VDMA_BTIF_RX_CH), dma_con);

    DMA_Clock_Enable(VDMA_USB_EP2O_CH);
    dma_con = (DMA_CON_MASTER_EP2O_RX << 20) | DMA_CON_DIR | DMA_CON_DRQ | DMA_CON_ITEN;
    DRV_WriteReg32(DMA_CON(VDMA_USB_EP2O_CH), dma_con);

    DMA_Clock_Enable(VDMA_USB_EP3I_CH);
    dma_con = (DMA_CON_MASTER_EP3I_TX << 20) | DMA_CON_DIR | DMA_CON_DRQ | DMA_CON_ITEN;
    DRV_WriteReg32(DMA_CON(VDMA_USB_EP3I_CH), dma_con);

    DMA_Clock_Enable(VDMA_USB_EP3O_CH);
    dma_con = (DMA_CON_MASTER_EP3O_RX << 20) | DMA_CON_DIR | DMA_CON_DRQ | DMA_CON_ITEN;
    DRV_WriteReg32(DMA_CON(VDMA_USB_EP3O_CH), dma_con);

    DMA_Clock_Enable(VDMA_USB_EP4I_CH);
    dma_con = (DMA_CON_MASTER_EP4I_TX << 20) | DMA_CON_DIR | DMA_CON_DRQ | DMA_CON_ITEN;
    DRV_WriteReg32(DMA_CON(VDMA_USB_EP4I_CH), dma_con);

    DMA_Clock_Enable(VDMA_USB_EP4O_CH);
    dma_con = (DMA_CON_MASTER_EP4O_RX << 20) | DMA_CON_DIR | DMA_CON_DRQ | DMA_CON_ITEN;
    DRV_WriteReg32(DMA_CON(VDMA_USB_EP4O_CH), dma_con);

    DMA_Clock_Enable(VDMA_ADC_RX_CH);
    dma_con = (DMA_CON_MASTER_ADC_RX << 20) | DMA_CON_DIR | DMA_CON_DRQ | DMA_CON_BURST_SINGLE | DMA_CON_DRQ | DMA_CON_SIZE_LONG | DMA_CON_ITEN | DMA_CON_TOEN;
    DRV_WriteReg32(DMA_CON(VDMA_ADC_RX_CH), dma_con);
  	}

	vfifo_init_count++;
}


void DMA_Vfifo_SetAdrs(uint32_t adrs,
                       uint32_t                      len, DMA_VFIFO_CHANNEL ch,
                       uint32_t                      alt_len,
                       uint32_t                      dma_count,
                       uint32_t                      timeout_counter)
{
    DRV_WriteReg32(DMA_PGMADDR(ch), adrs);
    DRV_WriteReg32(DMA_FFSIZE(ch), len);
    DRV_WriteReg32(DMA_ALTLEN(ch), alt_len);
    DRV_Reg32(DMA_COUNT(ch)) = dma_count;
    DRV_Reg32(DMA_TO(ch)) = timeout_counter;
}

void DMA_Vfifo_Set_timeout(DMA_VFIFO_CHANNEL ch, uint32_t timeout_counter)

{
    DRV_Reg32(DMA_TO(ch)) = timeout_counter;
}

void DMA_Vfifo_enable_interrupt(DMA_VFIFO_CHANNEL ch)

{
    DMA_EnableINT(ch);
}

void DMA_Vfifo_disable_interrupt(DMA_VFIFO_CHANNEL ch)

{
    DMA_DisableINT(ch);
}


void DMA_Vfifo_Flush(DMA_VFIFO_CHANNEL ch)
{
    DMA_Stop((kal_uint8)ch);

    DMA_Start(ch);
}

void DMA_Vfifo_Register_Callback(DMA_VFIFO_CHANNEL  ch,
                                 VOID_FUNC          callback_func)
{
    DMA_VFIFO_LISR_FUNC[ch - DMA_VFIFO_CH_S] = callback_func;
}

void DMA_Vfifo_Register_TO_Callback(DMA_VFIFO_CHANNEL ch,
                                    VOID_FUNC         callback_func)
{
    DMA_VFIFO_TO_LISR_FUNC[ch - DMA_VFIFO_CH_S] = callback_func;
}

void DMA_Clock_Enable(uint8_t channel)
{
    *(volatile kal_uint32 *)(CONFG_MCCR_CLR_ADDR) = 1 << (channel - 1);
}

void DMA_Clock_Disable(uint8_t channel)
{
    *(volatile kal_uint32 *)(CONFG_MCCR_SET_ADDR) = 1 << (channel - 1);
}

#endif

