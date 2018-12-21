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

/*
** $Log: hal_I2S.c $
**
** 09 30 2015 pierre.chang
** 1) Purpose:
** 	Change the i2s API for phase 2
** 	2) Changed function name:
** 	N/A
** 	3) Code change description brief:
** 	Adjust the header and API
** 	4) Unit Test Result:
** 	N/A
**
** 07 24 2015 pierre.chang
** Update UART, I2S, ADC.
** 02 04 2015 maker.wang
** 1. Add halI2sCfgInitialSetting, halI2sCfgSampleRate, halI2sCfgMonoStereo, halI2sCfgTdmChBitPerSample for simplify I2S intial flow
** 2. Rename I2S API
**
** 12 11 2014 maker.wang
** 1. Modify I2S and vdma drvier
**
** 11 07 2014 pierre.chang
** DMA change for peripherals and add I2S code.
**
**
*/


/*
I2S initial flow
Step 1 : Config I2S setting
I2sCfgInitialSetting
I2sCfgSampleRate
I2sCfgMonoStereo
I2sCfgTdmChBitPerSample(If I2S is TDM mode)

Step 2 : Set I2S config to register
I2sSetGblCfg(&I2SGblCfg)
I2sSetDlCfg(&I2SDLCfg)
I2sSetUlCfg(&I2SULCfg)

Step 3 : Enable I2S Clock, DL pipe, UL pipe
I2sClkFifoEn(I2S_EN)
I2sDlEn(I2S_EN)
I2sUlEn(I2S_EN)

I2S disable flow
I2sClkFifoEn(I2S_DIS)
I2sDlEn(I2S_DIS)
I2sUlEn(I2S_DIS)
I2sReset();
*/



/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "hal_i2s_internal.h"
#ifdef HAL_I2S_MODULE_ENABLED

//#include "mt7687_conf.h"
//#include "nvic.h"
#include "type_def.h"
#include "mt7687_cm4_hw_memmap.h"
#include "dma_hw.h"
#include "dma_sw.h"
#include "hal_i2s.h"
#include "_mtk_hal_debug.h"
#include "top.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/



/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/


/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*
halI2STxVDMAInit
u4TxBufAddr:I2S VDMA TX buffer
u4TxBufLen:I2S VDMA TX buffer length
u4TxCasPortAddr:I2S VDMA TX buffer cascade port address
bTxCasEn: I2S_DMA_CVFF_EN, I2S_DMA_CVFF_DIS
*/
BOOLEAN  I2sVdmaTxStart(UINT32 u4TxBufAddr, UINT32 u4TxBufLen, DMA_VFIFO_CHANNEL TxCasPortAddr, BOOLEAN bTxCasEn)
{
    if (bTxCasEn > I2S_DMA_CVFF_EN) {
        return KAL_FALSE;
    }
    DRV_WriteReg32(DMA_PGMADDR(VDMA_I2S_TX_CH), u4TxBufAddr);
    DRV_WriteReg32(DMA_FFSIZE(VDMA_I2S_TX_CH), u4TxBufLen);

    if (bTxCasEn == KAL_TRUE) {
        DRV_Reg32(DMA_CVFF(VDMA_I2S_TX_CH)) = VFIFO_base | ((TxCasPortAddr - DMA_VFIFO_CH_S) << 8) | DMA_CVFF_CVFF_EN;
    } else {
        DRV_Reg32(DMA_CVFF(VDMA_I2S_TX_CH)) &= ~(DMA_CVFF_CVFF_EN);
    }

    DMA_Vfifo_Flush(VDMA_I2S_TX_CH);

    return KAL_TRUE;
}

/*
halI2SRxVDMAInit
u4RxBufAddr:I2S VDMA RX buffer
u4RxBufLen:I2S VDMA RX buffer length
u4RxCasPortAddr:I2S VDMA RX buffer cascade port address
bRxCasEn: I2S_DMA_CVFF_EN, I2S_DMA_CVFF_DIS
*/
BOOLEAN  I2sVdmaRxStart(UINT32 u4RxBufAddr, UINT32 u4RxBufLen, DMA_VFIFO_CHANNEL RxCasPortAddr, BOOLEAN bRxCasEn)
{
    if (bRxCasEn > I2S_DMA_CVFF_EN) {
        return KAL_FALSE;
    }
    DRV_WriteReg32(DMA_PGMADDR(VDMA_I2S_RX_CH), u4RxBufAddr);
    DRV_WriteReg32(DMA_FFSIZE(VDMA_I2S_RX_CH), u4RxBufLen);

    if (bRxCasEn == KAL_TRUE) {
        DRV_Reg32(DMA_CVFF(VDMA_I2S_RX_CH)) = VFIFO_base | ((RxCasPortAddr - DMA_VFIFO_CH_S) << 8) | DMA_CVFF_CVFF_EN;
    } else {
        DRV_Reg32(DMA_CVFF(VDMA_I2S_RX_CH)) &= ~(DMA_CVFF_CVFF_EN);
    }

    DMA_Vfifo_Flush(VDMA_I2S_RX_CH);

    return KAL_TRUE;
}

BOOLEAN  I2sSetVdmaTxIntrEn(BOOLEAN bI2sVdmaTxIntrEn)
{
    UINT32 u4DmaCon;

    u4DmaCon = DRV_Reg32(DMA_CON(VDMA_I2S_TX_CH));

    if (bI2sVdmaTxIntrEn == I2S_DMA_INTR_EN) {
        u4DmaCon |= DMA_CON_ITEN;
    } else if (bI2sVdmaTxIntrEn == I2S_DMA_INTR_DIS) {
        u4DmaCon &= ~(DMA_CON_ITEN);
    } else {
        printf("Error parameter!!!\n");
    }

    DRV_WriteReg32(DMA_CON(VDMA_I2S_TX_CH), u4DmaCon);

    return KAL_TRUE;
}

BOOLEAN  I2sSetVdmaRxIntrEn(BOOLEAN bI2sVdmaRxIntrEn)
{
    UINT32 u4DmaCon;

    u4DmaCon = DRV_Reg32(DMA_CON(VDMA_I2S_RX_CH));

    if (bI2sVdmaRxIntrEn == I2S_DMA_INTR_EN) {
        u4DmaCon |= DMA_CON_ITEN;
    } else if (bI2sVdmaRxIntrEn == I2S_DMA_INTR_DIS) {
        u4DmaCon &= ~(DMA_CON_ITEN);
    } else {
        printf("Error parameter!!!\n");
    }

    DRV_WriteReg32(DMA_CON(VDMA_I2S_RX_CH), u4DmaCon);

    return KAL_TRUE;
}

BOOLEAN  I2sVdmaTxStop(void)
{
    DMA_Stop((UINT8)VDMA_I2S_TX_CH);

    return KAL_TRUE;
}

BOOLEAN  I2sVdmaRxStop(void)
{
    DMA_Stop((UINT8)VDMA_I2S_RX_CH);

    return KAL_TRUE;
}

BOOLEAN  I2sSetVdmaTxThreshold(UINT32 u4TxThreshold)
{
    DRV_Reg32(DMA_COUNT(VDMA_I2S_TX_CH)) = u4TxThreshold;

    return KAL_TRUE;
}

BOOLEAN  I2sSetVdmaRxThreshold(UINT32 u4RxThreshold)
{
    DRV_Reg32(DMA_COUNT(VDMA_I2S_RX_CH)) = u4RxThreshold;

    return KAL_TRUE;
}

BOOLEAN  I2sWriteVdmaTxFifo(UINT32 u4Txdata)
{
    DRV_Reg32(I2S_DMA_TX_FIFO) = u4Txdata;

    return KAL_TRUE;
}

BOOLEAN  I2sReadVdmaRxFifo(PUINT32 pu4Rxdata)
{
    *pu4Rxdata = DRV_Reg32(I2S_DMA_RX_FIFO);

    return KAL_TRUE;
}

UINT16 I2sGetVdmaTxFifoCnt()
{
    return DRV_Reg16(I2S_DMA_TX_FIFO_CNT);
}

UINT16 I2sGetVdmaRxFifoCnt()
{
    return DRV_Reg16(I2S_DMA_RX_FIFO_CNT);
}

/*
Reference the comment of I2sCfgInitialSetting
*/
BOOLEAN  I2sSetGblCfg(P_I2S_GBL_CFG pI2sGblCfg)
{
    UINT32 u4RegCfg = 0;

    ASSERT(pI2sGblCfg);

    if (pI2sGblCfg->ucI2SExtCodecSel == I2S_EXT_EN) {
        u4RegCfg |= BIT(I2S_EXT_SHFT);
    }

    if (pI2sGblCfg->ucI2SExtSwapLR == I2S_EXT_LRSW_EN) {
        u4RegCfg |= BIT(I2S_EXT_LRSW_SHFT);
    }

    if (pI2sGblCfg->ucI2SDLSwapLR == I2S_DL_LRSW_EN) {
        u4RegCfg |= BIT(I2S_DL_LRSW_SHFT);
    }

    if (pI2sGblCfg->ucI2SDLMonoStereoSel == I2S_DL_MONO) {
        u4RegCfg |= BIT(I2S_DL_MONO_SHFT);
    }

    if (pI2sGblCfg->bI2SDLMonoDupEn == I2S_DL_MONO_DUP_EN) {
        u4RegCfg |= BIT(I2S_DL_MONO_DUP_SHFT);
    }

    if (pI2sGblCfg->bI2S_CODEC_26M_EN == I2S_CODEC_26M_EN) {
        u4RegCfg |= BIT(I2S_CODEC_26M_SHFT);
    }

    if (pI2sGblCfg->bI2S_26M_SEL == I2S_26M_SEL_XPLL) {
        u4RegCfg |= BIT(I2S_26M_SEL_SHFT);
    }

    if (pI2sGblCfg->bI2SClkInvEn == I2S_CK_INV_EN) {
        u4RegCfg |= BIT(I2S_CK_INV_SHFT);
    }

    if (pI2sGblCfg->bI2SNegCapEn == I2S_NEG_CAP_EN) {
        u4RegCfg |= BIT(I2S_NEG_CAP_SHFT);
    }

    //u4RegCfg |= (pI2sGblCfg->u4I2SInClkSel << I2S_IN_CLK_SEL_SHFT) & I2S_IN_CLK_SEL_MASK;
    //u4RegCfg |= (pI2sGblCfg->u4I2SOutClkSel << I2S_OUT_CLK_SEL_SHFT) & I2S_OUT_CLK_SEL_MASK;

    if (pI2sGblCfg->bI2SLoopBackEn  == I2S_LOOPBACK_EN) {

        u4RegCfg |= BIT(I2S_LOOPBACK_SHFT);

    }

    //set CLK_SEL_IN

    // clear bit 25:24
    u4RegCfg &= ~0x03000000;

    switch (pI2sGblCfg->u4I2SInClkSel) {
        case I2S_CLK_SEL_13M://0x0
            //u4RegCfg &= ~0x03000000;
            break;
        case I2S_CLK_SEL_26M:
            u4RegCfg |= 0x01000000;
            break;
        case I2S_CLK_SEL_XPLL_16M:
            u4RegCfg |= 0x02000000;
            break;
        case I2S_CLK_SEL_EXT_BCLK:
            u4RegCfg |= 0x03000000;
            break;
    }

    //set CLK_SEL_OUT

    // clear bit 27:26
    u4RegCfg &= ~0x0C000000;

    switch (pI2sGblCfg->u4I2SOutClkSel) {
        case I2S_CLK_SEL_13M:
            //u4RegCfg &= ~0x0C000000;
            break;
        case I2S_CLK_SEL_26M:
            u4RegCfg |= 0x04000000;
            break;
        case I2S_CLK_SEL_XPLL_16M:
            u4RegCfg |= 0x08000000;
            break;
        case I2S_CLK_SEL_EXT_BCLK:
            u4RegCfg |= 0x0C000000;
            break;
    }

    //printf("I2sSetGblCfg  u4RegCfg=%08x\n",u4RegCfg);

    DRV_Reg32(I2S_GLB_CONTROL_ADDR) = u4RegCfg;

    return KAL_TRUE;
}

/*
Reference the comment of I2sCfgInitialSetting
*/
BOOLEAN  I2sSetDlCfg(P_I2S_DL_CFG pI2sDlCfg)
{
    UINT32 u4RegCfg = 0;

    ASSERT(pI2sDlCfg);

    if (pI2sDlCfg->ucI2SSRC == I2S_DL_SRC_SLAVE) {
        u4RegCfg |= BIT(I2S_DL_SRC_SHFT);
    }

    if (pI2sDlCfg->ucI2SFormat == I2S_DL_FMT_I2S) {
        u4RegCfg |= BIT(I2S_DL_FMT_SHFT);
    }

    if (pI2sDlCfg->ucI2SWordSelInv == I2S_DL_WSINV_EN) {
        u4RegCfg |= BIT(I2S_DL_WSINV_SHFT);
    }

    if (pI2sDlCfg->ucI2Sdl_fifo_2deq == I2S_DL_FIFO_2DEQ_EN) {
        u4RegCfg |= BIT(I2S_DL_FIFO_2DEQ_EN_SHFT);
    }

    //u4RegCfg |= (pI2sDlCfg->u4I2SSampleRate << I2S_DL_SR_SHFT) & I2S_DL_SR_MASK;

    //u4RegCfg |= (pI2sDlCfg->u4I2SBitPerSample << I2S_DL_BIT_PER_S_SHFT) & I2S_DL_BIT_PER_S_MASK;

    u4RegCfg |= (pI2sDlCfg->u4I2SMsbOffset << I2S_DL_MSB_OFFSET_SHFT) & I2S_DL_MSB_OFFSET_MASK;

    //u4RegCfg |= (pI2sDlCfg->u4I2SChPerSample << I2S_DL_CH_PER_S_SHFT) & I2S_DL_CH_PER_S_MASK;

    //set TX sample rate
    // clear bit 11:8
    u4RegCfg &= ~0x00000F00;

    switch (pI2sDlCfg->u4I2SSampleRate) {
        case I2S_SAMPLE_RATE_8K:
            break;
        case I2S_SAMPLE_RATE_12K:
            u4RegCfg |= 0x00000200;
            break;
        case I2S_SAMPLE_RATE_16K:
            u4RegCfg |= 0x00000400;
            break;
        case I2S_SAMPLE_RATE_24K:
            u4RegCfg |= 0x00000600;
            break;
        case I2S_SAMPLE_RATE_32K:
            u4RegCfg |= 0x00000800;
            break;
        case I2S_SAMPLE_RATE_48K:
            u4RegCfg |= 0x00000A00;
            break;
    }

    //set TX bit per sample
    // clear bit 14:13
    u4RegCfg &= ~0x00006000;

    switch (pI2sDlCfg->u4I2SBitPerSample) {
        case I2S_BIT_PER_S_32BTIS:
            break;
        case I2S_BIT_PER_S_64BTIS:
            u4RegCfg |= 0x00002000;
            break;
        case I2S_BIT_PER_S_128BTIS:
            u4RegCfg |= 0x00004000;
            break;
    }

    //set TX channel per sample
    // clear bit 30:29
    u4RegCfg &= ~0x60000000;

    switch (pI2sDlCfg->u4I2SChPerSample) {
        case I2S_CH_PER_S_2_CH:
            break;
        case I2S_CH_PER_S_4_CH:
            u4RegCfg |= 0x20000000;
            break;
    }

    //printf("I2sSetDlCfg  u4RegCfg=%08x\n",u4RegCfg);

    DRV_Reg32(I2S_DL_CONTROL_ADDR) = u4RegCfg | BIT(I2S_DL_WS_RESYNC_SHFT);

    return KAL_TRUE;
}

/*
Reference the comment of I2sCfgInitialSetting
*/
BOOLEAN  I2sSetUlCfg(P_I2S_UL_CFG pI2sUlCfg)
{
    UINT32 u4RegCfg = 0;

    ASSERT(pI2sUlCfg);

    if (pI2sUlCfg->ucI2SSRC == I2S_UL_SRC_SLAVE) {
        u4RegCfg |= BIT(I2S_UL_SRC_SHFT);
    }

    if (pI2sUlCfg->ucI2SFormat == I2S_UL_FMT_I2S) {
        u4RegCfg |= BIT(I2S_UL_FMT_SHFT);
    }

    if (pI2sUlCfg->ucI2SWordSelInv == I2S_UL_WSINV_EN) {
        u4RegCfg |= BIT(I2S_UL_WSINV_SHFT);
    }

    //u4RegCfg |= (pI2sUlCfg->u4I2SSampleRate << I2S_UL_SR_SHFT) & I2S_UL_SR_MASK;

    //u4RegCfg |= (pI2sUlCfg->u4I2SBitPerSample << I2S_UL_BIT_PER_S_SHFT) & I2S_UL_BIT_PER_S_MASK;

    if (pI2sUlCfg->bI2SDownRateEn == I2S_UL_DOWN_RATE_EN) {
        u4RegCfg |= BIT(I2S_UL_DOWN_RATE_SHFT);
    }

    if (pI2sUlCfg->u4I2SLRSwap == I2S_UL_LR_SWAP_EN) {
        u4RegCfg |= BIT(I2S_UL_LR_SWAP_SHFT);
    }

    u4RegCfg |= (pI2sUlCfg->u4I2SMsbOffset << I2S_UL_MSB_OFFSET_SHFT) & I2S_UL_MSB_OFFSET_MASK;

    u4RegCfg |= (pI2sUlCfg->u4I2SUpdateWord << I2S_UL_UPDATE_WORD_SHFT) & I2S_UL_UPDATE_WORD_MASK;

    //u4RegCfg |= (pI2sUlCfg->u4I2SChPerSample << I2S_UL_CH_PER_S_SHFT) & I2S_UL_CH_PER_S_MASK;

    //set RX sample rate
    // clear bit 11:8
    u4RegCfg &= ~0x00000F00;

    switch (pI2sUlCfg->u4I2SSampleRate) {
        case I2S_SAMPLE_RATE_8K:
            break;
        case I2S_SAMPLE_RATE_12K:
            u4RegCfg |= 0x00000200;
            break;
        case I2S_SAMPLE_RATE_16K:
            u4RegCfg |= 0x00000400;
            break;
        case I2S_SAMPLE_RATE_24K:
            u4RegCfg |= 0x00000600;
            break;
        case I2S_SAMPLE_RATE_32K:
            u4RegCfg |= 0x00000800;
            break;
        case I2S_SAMPLE_RATE_48K:
            u4RegCfg |= 0x00000A00;
            break;
    }

    //set RX bit per sample
    // clear bit 14:13
    u4RegCfg &= ~0x00006000;

    switch (pI2sUlCfg->u4I2SBitPerSample) {
        case I2S_BIT_PER_S_32BTIS:
            break;
        case I2S_BIT_PER_S_64BTIS:
            u4RegCfg |= 0x00002000;
            break;
        case I2S_BIT_PER_S_128BTIS:
            u4RegCfg |= 0x00004000;
            break;
    }

    //set RX channel per sample
    // clear bit 30:29
    u4RegCfg &= ~0x60000000;

    switch (pI2sUlCfg->u4I2SChPerSample) {
        case I2S_CH_PER_S_2_CH:
            break;
        case I2S_CH_PER_S_4_CH:
            u4RegCfg |= 0x20000000;
            break;
    }

    //printf("I2sSetUlCfg  u4RegCfg=%08x\n",u4RegCfg);

    DRV_Reg32(I2S_UL_CONTROL_ADDR) = u4RegCfg | BIT(I2S_UL_WS_RESYNC_SHFT);

    //dump register
    /*
    unsigned int temp;
    temp = DRV_Reg32(I2S_GLB_CONTROL_ADDR);
    printf("I2S_GLB_CONTROL_ADDR=%08x\n",temp);

    temp = DRV_Reg32(I2S_DL_CONTROL_ADDR);
    printf("I2S_DL_CONTROL_ADDR=%08x\n",temp);

    temp = DRV_Reg32(I2S_UL_CONTROL_ADDR);
    printf("I2S_UL_CONTROL_ADDR=%08x\n",temp);
    */
    return KAL_TRUE;
}

/*
Reference the comment of I2sCfgInitialSetting
Parameter
bI2sEn:I2S_EN, I2S_DIS
*/
BOOLEAN  I2sClkFifoEn(BOOLEAN bI2sEn)
{
    if (bI2sEn == I2S_EN) {
        DRV_Reg32(I2S_GLB_CONTROL_ADDR) |= BIT(I2S_EN_SHFT);

    } else {
        DRV_Reg32(I2S_GLB_CONTROL_ADDR) &= ~(I2S_EN_MASK);
        DRV_Reg32(I2S_GLB_CONTROL_ADDR) &= ~(I2S_ULFIFO_EN_MASK);
        DRV_Reg32(I2S_GLB_CONTROL_ADDR) &= ~(I2S_DLFIFO_EN_MASK);
        DRV_Reg32(I2S_GLB_CONTROL_ADDR) &= ~(I2S_ENGEN_EN_MASK);
    }

    return KAL_TRUE;
}

/*
Reference the comment of I2sCfgInitialSetting
Parameter
bI2sEn:I2S_EN, I2S_DIS
*/
BOOLEAN  I2sDlEn(BOOLEAN bI2sEn)
{
    if (bI2sEn == I2S_EN) {

        DRV_Reg32(I2S_DL_CONTROL_ADDR) |= BIT(I2S_DL_EN_SHFT);
    } else {

        DRV_Reg32(I2S_DL_CONTROL_ADDR) &= ~(I2S_DL_EN_MASK);
    }

    return KAL_TRUE;
}

/*
Reference the comment of I2sCfgInitialSetting
Parameter
bI2sEn:I2S_EN, I2S_DIS
*/
BOOLEAN  I2sUlEn(BOOLEAN bI2sEn)
{
    if (bI2sEn == I2S_EN) {

        DRV_Reg32(I2S_UL_CONTROL_ADDR) |= BIT(I2S_UL_EN_SHFT);
    } else {

        DRV_Reg32(I2S_UL_CONTROL_ADDR) &= ~(I2S_UL_EN_MASK);
    }

    return KAL_TRUE;
}

/*
Reference the comment of I2sCfgInitialSetting
*/
BOOLEAN  I2sReset()
{
    DRV_Reg32(I2S_SW_RESET_ADDR) |= BIT(I2S_SW_RST_EN_SHFT);
    DRV_Reg32(I2S_SW_RESET_ADDR) &= ~(I2S_SW_RST_EN_MASK);

    return KAL_TRUE;
}


BOOLEAN I2sCfgInitialSetting(ENUM_I2S_INITIAL_TYPE_T I2sInitialType, P_I2S_GBL_CFG pI2sGblCfg, P_I2S_DL_CFG pI2sDlCfg, P_I2S_UL_CFG pI2sUlCfg)
{
    ASSERT(pI2sGblCfg);
    ASSERT(pI2sDlCfg);
    ASSERT(pI2sUlCfg);

    switch (I2sInitialType) {
        case I2S_INTERNAL_LOOPBACK_MODE://just for test case
            pI2sGblCfg->bI2SLoopBackEn = I2S_LOOPBACK_EN;
            pI2sGblCfg->u4I2SInClkSel = I2S_CLK_SEL_13M;
            pI2sGblCfg->u4I2SOutClkSel = I2S_CLK_SEL_13M;
            pI2sGblCfg->bI2SClkInvEn = I2S_CK_INV_DIS;
            pI2sGblCfg->bI2SNegCapEn = I2S_NEG_CAP_DIS;
            pI2sGblCfg->bI2S_CODEC_26M_EN = I2S_CODEC_26M_DIS;
            pI2sGblCfg->bI2S_26M_SEL = I2S_26M_SEL_XTAL;
            pI2sGblCfg->ucI2SDLSwapLR = I2S_DL_LRSW_DIS;
            pI2sGblCfg->ucI2SExtSwapLR = I2S_EXT_LRSW_DIS;
            pI2sGblCfg->ucI2SExtCodecSel = I2S_EXT_DIS;
            pI2sDlCfg->u4I2SChPerSample = I2S_CH_PER_S_2_CH;
            pI2sDlCfg->u4I2SMsbOffset = 0;
            pI2sDlCfg->u4I2SBitPerSample = I2S_BIT_PER_S_32BTIS;
            pI2sDlCfg->ucI2SWordSelInv = I2S_DL_WSINV_DIS;
            pI2sDlCfg->ucI2SFormat = I2S_DL_FMT_I2S;
            pI2sDlCfg->ucI2SSRC = I2S_DL_SRC_MASTER;
            pI2sUlCfg->u4I2SLRSwap = I2S_UL_LR_SWAP_DIS;
            pI2sUlCfg->u4I2SChPerSample = I2S_CH_PER_S_2_CH;
            pI2sUlCfg->u4I2SUpdateWord = 8;
            pI2sUlCfg->u4I2SMsbOffset = 0;
            pI2sUlCfg->u4I2SBitPerSample = I2S_BIT_PER_S_32BTIS;
            pI2sUlCfg->ucI2SWordSelInv = I2S_UL_WSINV_DIS;
            pI2sUlCfg->ucI2SFormat = I2S_UL_FMT_I2S;
            pI2sUlCfg->ucI2SSRC = I2S_UL_SRC_MASTER;
            break;
        case I2S_EXTERNAL_MODE://external codec, i2s slave mode
            pI2sGblCfg->bI2SLoopBackEn = I2S_LOOPBACK_DIS;
            pI2sGblCfg->u4I2SInClkSel = I2S_CLK_SEL_EXT_BCLK;
            pI2sGblCfg->u4I2SOutClkSel = I2S_CLK_SEL_EXT_BCLK;
            pI2sGblCfg->bI2SNegCapEn = I2S_NEG_CAP_EN;
            pI2sGblCfg->bI2SClkInvEn = I2S_CK_INV_EN;
            pI2sGblCfg->bI2S_CODEC_26M_EN = I2S_CODEC_26M_DIS;
            pI2sGblCfg->bI2S_26M_SEL = I2S_26M_SEL_XTAL;
            pI2sGblCfg->ucI2SDLSwapLR = I2S_DL_LRSW_DIS;
            pI2sGblCfg->ucI2SExtSwapLR = I2S_EXT_LRSW_DIS;
            pI2sGblCfg->ucI2SExtCodecSel = I2S_EXT_EN;
            pI2sDlCfg->u4I2SChPerSample = I2S_CH_PER_S_2_CH;
            pI2sDlCfg->u4I2SMsbOffset = 0;
            pI2sDlCfg->u4I2SBitPerSample = I2S_BIT_PER_S_32BTIS;
            pI2sDlCfg->ucI2SWordSelInv = I2S_DL_WSINV_DIS;
            pI2sDlCfg->ucI2SFormat = I2S_DL_FMT_I2S;
            pI2sDlCfg->ucI2SSRC = I2S_DL_SRC_SLAVE;
            pI2sUlCfg->u4I2SLRSwap = I2S_UL_LR_SWAP_DIS;
            pI2sUlCfg->u4I2SChPerSample = I2S_CH_PER_S_2_CH;
            pI2sUlCfg->u4I2SUpdateWord = 8;
            pI2sUlCfg->u4I2SMsbOffset = 0;
            pI2sUlCfg->u4I2SBitPerSample = I2S_BIT_PER_S_32BTIS;
            pI2sUlCfg->ucI2SWordSelInv = I2S_UL_WSINV_DIS;
            pI2sUlCfg->ucI2SFormat = I2S_UL_FMT_I2S;
            pI2sUlCfg->ucI2SSRC = I2S_DL_SRC_SLAVE;
            break;
        default:
            return KAL_FALSE;
    }

    return KAL_TRUE;
}


// =======================================================================================
// xpll setting

#define TOP_AON_BASE  0x81021000
#define RG_PMU_14                   (TOP_AON_BASE + 0x0438)

//RG_PMU_14
#define RG_ALDO_EN    0x80000000
#define RG_ALDO_VSEL_MASK    0x70000000
#define RG_ALDO_VSEL_2_POINT_65    7
#define RG_ALDO_VSEL_2_POINT_6    6
#define RG_ALDO_VSEL_2_POINT_55    5
#define RG_ALDO_VSEL_2_POINT_5    4
#define RG_ALDO_VSEL_2_POINT_45    3
#define RG_ALDO_VSEL_2_POINT_4    2
#define RG_ALDO_VSEL_2_POINT_35    1
#define RG_ALDO_VSEL_2_POINT_3    0
#define RG_ALDO_VSEL(value)    ((((UINT32) value) << 28) & RG_ALDO_VSEL_MASK)
#define RG_ALDO_VCALL_MASK    0x07000000
#define RG_ALDO_VSEL_POSITIVE_30_MINI_V    7
#define RG_ALDO_VSEL_POSITIVE_20_MINI_V    6
#define RG_ALDO_VSEL_POSITIVE_10_MINI_V    5
#define RG_ALDO_VSEL_POSITIVE_0_MINI_V    4
#define RG_ALDO_VSEL_NAGATIVE_10_MINI_V    3
#define RG_ALDO_VSEL_NAGATIVE_20_MINI_V    2
#define RG_ALDO_VSEL_NAGATIVE_30_MINI_V    1
#define RG_ALDO_VSEL_NAGATIVE_40_MINI_V    0
#define RG_ALDO_VCAL(value)    ((((UINT32) value) << 24) & RG_ALDO_VCALL_MASK)
#define RG_ALDO_REMOTE_SEL    0x00800000

#define AUD_ANA_BASE  0x830B2000

//Analog reg
#define AUDDEC_CON0                 (AUD_ANA_BASE + 0x0500)
#define AUDDEC_CON1                 (AUD_ANA_BASE + 0x0504)
#define AUDDEC_CON2                 (AUD_ANA_BASE + 0x0508)
#define AUDDEC_CON3                 (AUD_ANA_BASE + 0x050C)
#define AUDDEC_CON4                 (AUD_ANA_BASE + 0x0510)
#define AUDDEC_CON5                 (AUD_ANA_BASE + 0x0514)
#define AUDDEC_CON6                 (AUD_ANA_BASE + 0x0518)
#define AUDDEC_CON7                 (AUD_ANA_BASE + 0x051C)
#define AUDDEC_CON8                 (AUD_ANA_BASE + 0x0520)
#define AUDDEC_CON9                 (AUD_ANA_BASE + 0x0524)
#define AUDDEC_CON10                (AUD_ANA_BASE + 0x0528)
#define AUDDEC_CON11                (AUD_ANA_BASE + 0x052C)

//AUD_DEC_CON11
#define RG_BGR_RSV_VA25_MASK   (0xFFUL << 0)
#define RG_BGR_RSV_VA25(value)    ((((UINT32) value) << 0) & RG_BGR_RSV_VA25_MASK)


#define XPLL_CM4_FREQ_MHZ    26
#define XPLL_INSTRACTION_PRE_WHILE    5
#define XPLL_ONE_MICRO_SEC_WHILE_COUNT    (XPLL_CM4_FREQ_MHZ/XPLL_INSTRACTION_PRE_WHILE)


XPLL_INFO XpllInfo;

void xpllMicroDelay(UINT32 u4DelayUS)
{
    volatile UINT32 u4DealyCount;

    u4DealyCount = u4DelayUS * XPLL_ONE_MICRO_SEC_WHILE_COUNT;

    while (u4DealyCount--);
}

void xpllOpen()
{
    UINT32 u4Reg;
    uint32_t xtal_freq;

    XpllInfo.u4NcpoValue = XPLL_NCPO_VALUE;//416Mhz

    //HAL_REG_32(0x830b2000) = 00000200;//SH setting

    //Open band gap for XPLL use
    //HAL_REG_32(0x83008040) |= 0x10;
    //HAL_REG_32(0x50305808) &= ~(0x20000);
    //HAL_REG_32(0x50305800) |= (0x8001);
    //HAL_REG_32(0x81021028) &= ~(0x600000);
    HAL_REG_32(0x81021028) |= 0x600000;

    xtal_freq = top_xtal_freq_get();
    if (xtal_freq == 26000000) {
        //Set XPLL to default value start
        HAL_REG_32(XPLL_CTL0) = RG_XPLL_FBSEL_DIV_2 | RG_XPLL_CKCTRL_MODE_3 | RG_XPLL_POSDIV_2 |
                                RG_XPLL_PREDIV_1 | RG_XPLL_FBDIV(0xF) | RG_XPLL_PWD;//0x0000741F
        HAL_REG_32(XPLL_CTL1) = RG_XPLL_BIR(0xD) | RG_XPLL_BR_20_ohm | RG_XPLL_BP(0x6) |
                                RG_XPLL_FPEN_4_PH;//0x0000D861
        HAL_REG_32(XPLL_CTL2) = RG_XPLL_LOAD_RSTB | RG_XPLL_AUTOK_LOAD | RG_XPLL_AUTOK_VCO |
                                RG_XPLL_BIC(0x1)  | RG_XPLL_VODEN | RG_XPLL_BC_1_POINT_5_PF;//0x00007302
        HAL_REG_32(XPLL_CTL3) = RG_XPLL_PCW_NCPO(XPLL_NCPO_VALUE); //0x1E000000
        HAL_REG_32(XPLL_CTL8) = RG_XPLL_BIAS_RST | RG_XPLL_BIAS_PWD | RG_XPLL_RESERVE(0xF0); //0x0003F000
        //Set XPLL to default value end

        xpllMicroDelay(10);//Delay 10us

        HAL_REG_32(XPLL_CTL8) &= ~(RG_XPLL_BIAS_PWD);
        HAL_REG_32(XPLL_CTL0) &= ~(RG_XPLL_PWD);
        HAL_REG_32(XPLL_CTL8) |= RG_XPLL_DDS_PWDB;

        while ((HAL_REG_32(XPLL_CTL4) & AD_RGS_PLL_VCOCAL_CPLT_MASK) != AD_RGS_PLL_VCOCAL_CPLT);

        xpllMicroDelay(20);//Delay 20us
        HAL_REG_32(XPLL_CTL8) |= RG_XPLL_DDS_RSTB;
        HAL_REG_32(XPLL_CTL3) |=  RG_XPLL_PCW_NCPO_CHG_HIGH;
        HAL_REG_32(XPLL_CTL8) |= RG_XPLL_FIFO_START_MAN;//Change RG_XPLL_NCPO_EN to RG_XPLL_FIFO_START_MAN
        HAL_REG_32(XPLL_CTL2) |= RG_XPLL_DDSEN;
        HAL_REG_32(XPLL_CTL8) &= ~(RG_XPLL_BIAS_RST);
    } else if (xtal_freq == 40000000) {
        HAL_REG_32(XPLL_CTL0) = 0x7413;
        HAL_REG_32(XPLL_CTL1) = 0x38C5;
        HAL_REG_32(XPLL_CTL2) = 0x7303;
        HAL_REG_32(XPLL_CTL3) = 0x12CC0000;
        HAL_REG_32(XPLL_CTL8) = 0x3FC00;

        xpllMicroDelay(10);//at least delay 1us

        HAL_REG_32(XPLL_CTL8) = 0x2FC00;

        xpllMicroDelay(10);//at least delay 1us

        HAL_REG_32(XPLL_CTL0) = 0x7412;
        HAL_REG_32(XPLL_CTL8) = 0x2FC08;

        xpllMicroDelay(300);//at least delay 200us

        HAL_REG_32(XPLL_CTL8) = 0x2FC18;

        xpllMicroDelay(10);//at least delay 1us

        HAL_REG_32(XPLL_CTL3) = 0x12CC0001;

        xpllMicroDelay(10);//at least delay 1us

        HAL_REG_32(XPLL_CTL8) = 0x2FC38;

        xpllMicroDelay(10);//at least delay 1us

        HAL_REG_32(XPLL_CTL2) = 0xF303;
        HAL_REG_32(XPLL_CTL8) = 0x0FC38;
        HAL_REG_32(XPLL_CTL8) = 0x0FCB8;

        xpllMicroDelay(200);//at least delay 20us

        //halI2sClkFifoEn(1);
    }

    xpllMicroDelay(20);//Delay 20us
    u4Reg = HAL_REG_32(RG_PMU_14);
    u4Reg &= ~(RG_ALDO_EN | RG_ALDO_VSEL_MASK | RG_ALDO_VCALL_MASK);
    u4Reg |= RG_ALDO_EN | RG_ALDO_VSEL(RG_ALDO_VSEL_2_POINT_5) | RG_ALDO_VCAL(RG_ALDO_VSEL_POSITIVE_0_MINI_V);
    HAL_REG_32(RG_PMU_14) = u4Reg;

    xpllMicroDelay(300);//Delay 300us
    HAL_REG_32(AUDDEC_CON9) = 0;
    HAL_REG_32(AUDDEC_CON10) = 0;
    HAL_REG_32(AUDDEC_CON11) = 0;

    xpllMicroDelay(30000);//Delay 30ms
}

void xpllClose()
{
    //XPLL power down
    HAL_REG_32(AUDDEC_CON11) = RG_BGR_RSV_VA25(1);//0x00000001;
    HAL_REG_32(RG_PMU_14) &= ~(RG_ALDO_EN);//0x44000000;
    HAL_REG_32(XPLL_CTL8) = RG_XPLL_RESERVE(0xF0) | RG_XPLL_FIFO_START_MAN | RG_XPLL_DDS_RSTB_MASK;//0x0000F030;
    HAL_REG_32(XPLL_CTL0) = RG_XPLL_FBSEL_DIV_2 | RG_XPLL_CKCTRL_MODE_3 | RG_XPLL_POSDIV_2 |
                            RG_XPLL_PREDIV_1 | RG_XPLL_FBDIV(0xF) | RG_XPLL_PWD;//0x0000741F
    HAL_REG_32(XPLL_CTL8) = RG_XPLL_BIAS_PWD | RG_XPLL_RESERVE(0xF0) | RG_XPLL_FIFO_START_MAN | RG_XPLL_DDS_RSTB_MASK;//0x0001F030;

    //Close band gap for XPLL use
    HAL_REG_32(0x81021028) &= ~(0x600000);
    HAL_REG_32(0x81021028) |= 0x400000;

}
/*
void xpllClose()
{
    //XPLL power down
    HAL_REG_32(AUDDEC_CON11) = RG_BGR_RSV_VA25(1);//0x00000001;
    HAL_REG_32(RG_PMU_14) = RG_ALDO_EN | RG_ALDO_VSEL(RG_ALDO_VSEL_2_POINT_5) |
                            RG_ALDO_VCAL(RG_ALDO_VSEL_POSITIVE_0_MINI_V);//0xC4000000;
    HAL_REG_32(XPLL_CTL8) = RG_XPLL_RESERVE(0xF0) | RG_XPLL_FIFO_START_MAN | RG_XPLL_DDS_RSTB_MASK;//0x0000F030;
    HAL_REG_32(XPLL_CTL0) = RG_XPLL_FBSEL_DIV_2 | RG_XPLL_CKCTRL_MODE_3 | RG_XPLL_POSDIV_2 |
                            RG_XPLL_PREDIV_1 | RG_XPLL_FBDIV(0xF) | RG_XPLL_PWD;//0x0000741F
    HAL_REG_32(XPLL_CTL8) = RG_XPLL_BIAS_PWD | RG_XPLL_RESERVE(0xF0) | RG_XPLL_FIFO_START_MAN | RG_XPLL_DDS_RSTB_MASK;//0x0001F030;

    //Close band gap for XPLL use
    HAL_REG_32(0x81021028) &= ~(0x600000);
    HAL_REG_32(0x81021028) |= 0x400000;
}
*/
#endif//#ifdef HAL_I2S_MODULE_ENABLED

