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
** $Log: hal_i2s_internal.h $
**
** 10 27 2015 austin cheng
** align api for  mt2523
**
** 08 13 2015 pierre.chang
** update i2s and adc sample code
**
** 08 12 2015 pierre.chang
** Update UART, I2S and ADC code.
**
** 07 24 2015 pierre.chang
** Update UART, I2S, ADC.
** 02 04 2015 maker.wang
** 1. Add halI2sInitial, halI2sCfgSampleRate, halI2sCfgMonoStereo, halI2sCfgTdmChBitPerSample for simplify I2S intial flow
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

#ifndef __HAL_I2S_INTERNAL_H__
#define __HAL_I2S_INTERNAL_H__

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "hal_i2s.h"
#if defined(HAL_I2S_MODULE_ENABLED)

//#include "mt7687_cm4_hw_memmap.h"
#include "dma_hw.h"
#include "dma_sw.h"

#include <stdio.h>


/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

#define I2S_STATE_IDLE          0
#define I2S_STATE_INIT          1
#define I2S_STATE_RX_RUNNING    2
#define I2S_STATE_TX_RUNNING    3
//#define I2S_STATE_AUDIO_TOP_RUNNING    3
/*I2S EOF threshold*/
#define I2S_EOF_THRESHOLD               4//Send EOF event until less than 4 samples in tx vfifo


#define I2S_GLB_CONTROL_ADDR        (CM4_I2S_BASE + 0x0000)
#define I2S_DL_CONTROL_ADDR         (CM4_I2S_BASE + 0x0004)
#define I2S_UL_CONTROL_ADDR         (CM4_I2S_BASE + 0x0008)
#define I2S_SW_RESET_ADDR           (CM4_I2S_BASE + 0x000C)

#define I2S_BIT(n)                      ((uint32_t) 1 << (n))

//------------I2S_GLB_CONTROL------------------------

#define I2S_CODEC_26M_EN            1
#define I2S_CODEC_26M_DIS           0
#define I2S_CODEC_26M_MASK          0x00020000
#define I2S_CODEC_26M_SHFT          17

#define I2S_26M_SEL_XPLL            1
#define I2S_26M_SEL_XTAL            0
#define I2S_26M_SEL_MASK            0x00040000
#define I2S_26M_SEL_SHFT            18

#define I2S_EN_MASK                 0x00000001
#define I2S_EN_SHFT                 0

#define I2S_DLFIFO_EN_MASK          0x00000002
#define I2S_DLFIFO_EN_SHFT          1

#define I2S_ULFIFO_EN_MASK          0x00000004
#define I2S_ULFIFO_EN_SHFT          2

#define I2S_ENGEN_EN_MASK           0x00000008
#define I2S_ENGEN_EN_SHFT           3

#define I2S_EXT_MASK                0x00000020
#define I2S_EXT_SHFT                5

#define I2S_EXT_LRSW_MASK           0x00000040
#define I2S_EXT_LRSW_SHFT           6

#define I2S_DL_LRSW_MASK            0x00000080
#define I2S_DL_LRSW_SHFT            7

#define I2S_DL_MONO_MASK            0x00000100
#define I2S_DL_MONO_SHFT            8

#define I2S_DL_MONO_DUP_MASK        0x00000200
#define I2S_DL_MONO_DUP_SHFT        9

#define I2S_26M_SEL_MASK            0x00040000
#define I2S_26M_SEL_SHFT            18

#define I2S_CK_INV_MASK             0x00080000
#define I2S_CK_INV_SHFT             19

#define I2S_NEG_CAP_MASK            0x00100000
#define I2S_NEG_CAP_SHFT            20

#define I2S_IN_CLK_SEL_MASK         0x03000000
#define I2S_IN_CLK_SEL_SHFT         24
#define I2S_OUT_CLK_SEL_MASK        0x0C000000
#define I2S_OUT_CLK_SEL_SHFT        26

#define I2S_LOOPBACK_MASK           0x80000000
#define I2S_LOOPBACK_SHFT           31

//------------I2S_DL_CONTROL_ADDR------------------------

#define I2S_DL_EN_MASK              0x00000001
#define I2S_DL_EN_SHFT              0

#define I2S_DL_WLEN_MASK            0x00000002
#define I2S_DL_WLEN_SHFT            1

#define I2S_DL_SRC_MASK             0x00000004
#define I2S_DL_SRC_SHFT             2

#define I2S_DL_FMT_MASK             0x00000008
#define I2S_DL_FMT_SHFT             3

#define I2S_DL_DIR_MASK             0x00000010
#define I2S_DL_DIR_SHFT             4

#define I2S_DL_WSINV_MASK           0x00000020
#define I2S_DL_WSINV_SHFT           5

#define I2S_DL_FIFO_2DEQ_EN_MASK    0x00000080
#define I2S_DL_FIFO_2DEQ_EN_SHFT    7

#define I2S_DL_SR_MASK              0x00000F00
#define I2S_DL_SR_SHFT              8

#define I2S_DL_HDEN_MASK            0x00001000
#define I2S_DL_HDEN_SHFT            12

#define I2S_DL_BIT_PER_S_MASK       0x00006000
#define I2S_DL_BIT_PER_S_SHFT       13

#define I2S_DL_WS_RESYNC_MASK       0x00008000
#define I2S_DL_WS_RESYNC_SHFT       15

#define I2S_DL_MSB_OFFSET_MASK      0x00FE0000
#define I2S_DL_MSB_OFFSET_SHFT      17

#define I2S_DL_CH_PER_S_MASK        0x60000000
#define I2S_DL_CH_PER_S_SHFT        29

//------------I2S_UL_CONTROL_ADDR------------------------

#define I2S_UL_EN_MASK              0x00000001
#define I2S_UL_EN_SHFT              0

#define I2S_UL_WLEN_MASK            0x00000002
#define I2S_UL_WLEN_SHFT            1

#define I2S_UL_SRC_MASK             0x00000004
#define I2S_UL_SRC_SHFT             2

#define I2S_UL_FMT_MASK             0x00000008
#define I2S_UL_FMT_SHFT             3

#define I2S_UL_DIR_MASK             0x00000010
#define I2S_UL_DIR_SHFT             4

#define I2S_UL_WSINV_MASK           0x00000020
#define I2S_UL_WSINV_SHFT           5

#define I2S_UL_SR_MASK              0x00000F00
#define I2S_UL_SR_SHFT              8

#define I2S_UL_HDEN_MASK            0x00001000
#define I2S_UL_HDEN_SHFT            12

#define I2S_UL_BIT_PER_S_MASK       0x00006000
#define I2S_UL_BIT_PER_S_SHFT       13

#define I2S_UL_WS_RESYNC_MASK       0x00008000
#define I2S_UL_WS_RESYNC_SHFT       15

#define I2S_UL_DOWN_RATE_MASK       0x00010000
#define I2S_UL_DOWN_RATE_SHFT       16

#define I2S_UL_MSB_OFFSET_MASK      0x00FE0000
#define I2S_UL_MSB_OFFSET_SHFT      17

#define I2S_UL_UPDATE_WORD_MASK     0x1F000000
#define I2S_UL_UPDATE_WORD_SHFT     24

#define I2S_UL_CH_PER_S_MASK        0x60000000
#define I2S_UL_CH_PER_S_SHFT        29

#define I2S_UL_LR_SWAP_MASK         0x80000000
#define I2S_UL_LR_SWAP_SHFT         31

//------------I2S_SW_RESET_ADDR------------------------
#define I2S_SW_RST_EN_MASK          0x00000001
#define I2S_SW_RST_EN_SHFT          0

//------------I2S_DMA_ADDR------------------------
#define I2S_DMA_TX_FIFO             0x79000000 //DMA12
#define I2S_DMA_RX_FIFO             0x79000100 //DMA13

#define I2S_DMA_TX_FIFO_CNT         0x83010C38
#define I2S_DMA_RX_FIFO_CNT         0x83010D38



//------------I2S Variable------------------------
#define I2S_TRUE                        1
#define I2S_FALSE                       0

#define I2S_EN                          1
#define I2S_DIS                         0
#define I2S_DLFIFO_EN                   1
#define I2S_DLFIFO_DIS                  0

#define I2S_ULFIFO_EN                   1
#define I2S_ULFIFO_DIS                  0

#define I2S_ENGEN_EN                    1
#define I2S_ENGEN_DIS                   0

#define I2S_EXT_EN                      1
#define I2S_EXT_DIS                     0

#define I2S_EXT_LRSW_EN                 1
#define I2S_EXT_LRSW_DIS                0

#define I2S_DL_LRSW_EN                  1
#define I2S_DL_LRSW_DIS                 0

#define I2S_DL_MONO                     1
#define I2S_DL_STEREO                   0

#define I2S_DL_MONO_DUP_EN              1
#define I2S_DL_MONO_DUP_DIS             0

#define I2S_CK_INV_EN                   1
#define I2S_CK_INV_DIS                  0

#define I2S_NEG_CAP_EN                  1
#define I2S_NEG_CAP_DIS                 0

#define I2S_LOOPBACK_EN                 1
#define I2S_LOOPBACK_DIS                0

#define I2S_DL_EN                       1
#define I2S_DL_DIS                      0

#define I2S_DL_SRC_SLAVE                1
#define I2S_DL_SRC_MASTER               0

#define I2S_DL_FMT_I2S                  1
#define I2S_DL_FMT_TDM                  0

#define I2S_DL_WSINV_EN                 1
#define I2S_DL_WSINV_DIS                0

#define I2S_DL_FIFO_2DEQ_EN             1
#define I2S_DL_FIFO_2DEQ_DIS            0

#define I2S_CLK_SEL_13M                 0x0
#define I2S_CLK_SEL_26M                 0x1
#define I2S_CLK_SEL_XPLL_16M            0x2
#define I2S_CLK_SEL_EXT_BCLK            0x3

#define I2S_SR_8K                    0x0
#define I2S_SR_12K                   0x2
#define I2S_SR_16K                   0x4
#define I2S_SR_24K                   0x6
#define I2S_SR_32K                   0x8
#define I2S_SR_48K                   0xA

#define I2S_BIT_PER_S_32BTIS         0x0
#define I2S_BIT_PER_S_64BTIS         0x1
#define I2S_BIT_PER_S_128BTIS        0x2

#define I2S_DL_WS_RESYNC_EN             1
#define I2S_DL_WS_RESYNC_DIS            0

#define I2S_CH_PER_S_2_CH            0x0
#define I2S_CH_PER_S_4_CH            0x1

#define I2S_UL_EN                       1
#define I2S_UL_DIS                      0

#define I2S_UL_SRC_SLAVE                1
#define I2S_UL_SRC_MASTER               0

#define I2S_UL_FMT_I2S                  1
#define I2S_UL_FMT_TDM                  0

#define I2S_UL_WSINV_EN                 1
#define I2S_UL_WSINV_DIS                0

#define I2S_UL_WS_RESYNC_EN             1
#define I2S_UL_WS_RESYNC_DIS            0

#define I2S_UL_DOWN_RATE_EN             1
#define I2S_UL_DOWN_RATE_DIS            0

#define I2S_UL_LR_SWAP_EN               1
#define I2S_UL_LR_SWAP_DIS              0

#define I2S_SW_RST_EN                   0x1
#define I2S_SW_RST_DIS                  0x0

#define I2S_DMA_CVFF_EN                 1
#define I2S_DMA_CVFF_DIS                0

#define I2S_DMA_INTR_EN                 1
#define I2S_DMA_INTR_DIS                0


// ==============================================================================
// xpll settings

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              R E G I S T E R    D E F I N E
********************************************************************************
*/

#define XPLL_CTL0               (TOP_CFG_AON_BASE + 0x0280)
#define XPLL_CTL1               (TOP_CFG_AON_BASE + 0x0284)
#define XPLL_CTL2               (TOP_CFG_AON_BASE + 0x0288)
#define XPLL_CTL3               (TOP_CFG_AON_BASE + 0x028C)
#define XPLL_CTL4               (TOP_CFG_AON_BASE + 0x0290)
#define XPLL_CTL5               (TOP_CFG_AON_BASE + 0x0294)
#define XPLL_CTL6               (TOP_CFG_AON_BASE + 0x0298)
#define XPLL_CTL7               (TOP_CFG_AON_BASE + 0x029C)
#define XPLL_CTL8               (TOP_CFG_AON_BASE + 0x02A0)

//XPLL_CTL0
#define RG_XPLL_FBSEL_DIV_MASK   0x0000C000
#define RG_XPLL_FBSEL_DIV_4      0x00008000
#define RG_XPLL_FBSEL_DIV_2      0x00004000
#define RG_XPLL_FBSEL_DIV_1      0x00000000
#define RG_XPLL_CKCTRL_MODE_MASK 0x00003000
#define RG_XPLL_CKCTRL_MODE_3    0x00003000    // (2^6 * Tref)
#define RG_XPLL_CKCTRL_MODE_2    0x00002000    // (2^7 * Tref)
#define RG_XPLL_CKCTRL_MODE_1    0x00001000    // (2^8 * Tref)
#define RG_XPLL_CKCTRL_MODE_0    0x00000000    // (2^9 * Tref)
#define RG_XPLL_POSDIV_MASK      0x00000C00
#define RG_XPLL_POSDIV_4         0x00000800
#define RG_XPLL_POSDIV_2         0x00000400
#define RG_XPLL_POSDIV_1         0x00000000
#define RG_XPLL_PREDIV_MASK      0x00000300
#define RG_XPLL_PREDIV_4         0x00000200
#define RG_XPLL_PREDIV_2         0x00000100
#define RG_XPLL_PREDIV_1         0x00000000
#define RG_XPLL_FBDIV_MASK       0x000000FE
#define RG_XPLL_FBDIV(value)    ((((UINT32) value) << 1) & RG_XPLL_FBDIV_MASK)
#define RG_XPLL_PWD_MASK         0x00000001
#define RG_XPLL_PWD              0x00000001

//XPLL_CTL1
#define RG_XPLL_BIR_MASK         0x0000F000
#define RG_XPLL_BIR(value)       ((((UINT32) value) << 12) & RG_XPLL_BIR_MASK)
#define RG_XPLL_BR_MASK          0x00000E00
#define RG_XPLL_BR_20_ohm        0x00000800
#define RG_XPLL_BR_40_ohm        0x00000400
#define RG_XPLL_BR_60_ohm        0x00000200
#define RG_XPLL_BR_80_ohm        0x00000000
#define RG_XPLL_MONEN_MASK       0x00000100
#define RG_XPLL_MONEN            0x00000100
#define RG_XPLL_BP_MASK          0x000000F0
#define RG_XPLL_BP(value)        ((((UINT32) value) << 4) & RG_XPLL_BP_MASK)
#define RG_XPLL_DIVEN_MASK       0x0000000E
#define RG_XPLL_DIVEN(value)     ((((UINT32) value) << 1) & RG_XPLL_DIVEN_MASK)
#define RG_XPLL_FPEN_MASK        0x00000001
#define RG_XPLL_FPEN_4_PH        0x00000001
#define RG_XPLL_FPEN_2_PH        0x00000000

//XPLL_CTL2
#define RG_XPLL_DDSEN_MASK       0x00008000
#define RG_XPLL_DDSEN            0x00008000
#define RG_XPLL_LOAD_RSTB_MASK   0x00004000
#define RG_XPLL_LOAD_RSTB        0x00004000
#define RG_XPLL_AUTOK_LOAD_MASK  0x00002000
#define RG_XPLL_AUTOK_LOAD       0x00002000
#define RG_XPLL_AUTOK_VCO_MASK   0x00001000
#define RG_XPLL_AUTOK_VCO        0x00001000
#define RG_XPLL_BIC_MASK         0x00000E00
#define RG_XPLL_BIC(value)      ((((UINT32) value) << 9) & RG_XPLL_BIC_MASK)
#define RG_XPLL_VODEN_MASK       0x00000100
#define RG_XPLL_VODEN            0x00000100
#define RG_XPLL_BAND_MASK        0x000000FC
#define RG_XPLL_BAND(value)      ((((UINT32) value) << 2) & RG_XPLL_BAND_MASK)
#define RG_XPLL_BC_MASK          0x00000003
#define RG_XPLL_BC_1_POINT_5_PF  0x00000002
#define RG_XPLL_BC_1_PF          0x00000001
#define RG_XPLL_BC_0_POINT_5_PF  0x00000000

//XPLL_CTL3
#define RG_XPLL_PCW_NCPO_MASK        0xFFFFFFFE
#define RG_XPLL_PCW_NCPO(value)      ((((UINT32) value) << 1) & RG_XPLL_PCW_NCPO_MASK)
#define RG_XPLL_PCW_NCPO_CHG_MASK    0x00000001
#define RG_XPLL_PCW_NCPO_CHG_HIGH    0x00000001

//XPLL_CTL4
#define AD_RGS_PLL_VCO_STATE_MASK    0x000000FC
#define AD_RGS_PLL_VCOCAL_CPLT_MASK  0x00000002
#define AD_RGS_PLL_VCOCAL_CPLT       0x00000002
#define AD_RGS_PLL_VCOCAL_FAIL_MASK  0x00000001
#define AD_RGS_PLL_VCOCAL_FAIL       0x00000001

//XPLL_CTL5
#define RG_XPLL_SSC_DELTA_MASK       0x0000FFFF
#define RG_XPLL_SSC_DELTA(value)    ((((UINT32) value) << 0) & RG_XPLL_SSC_DELTA_MASK)

//XPLL_CTL6
#define RG_XPLL_SSC_DELTA_1_MASK     0x0000FFFF
#define RG_XPLL_SSC_DELTA_1(value)   ((((UINT32) value) << 0) & RG_XPLL_SSC_DELTA_1_MASK)

//XPLL_CTL7
#define RG_XPLL_SSC_PRD_MASK         0x0000FFFF
#define RG_XPLL_SSC_PRD(value)       ((((UINT32) value) << 0) & RG_XPLL_SSC_PRD_MASK)

//XPLL_CTL8.
#define RG_XPLL_BYPASS_26M_EN_MASK   0x00040000
#define RG_XPLL_BYPASS_26M_EN        0x00040000
#define RG_XPLL_BIAS_RST_MASK        0x00020000
#define RG_XPLL_BIAS_RST             0x00020000
#define RG_XPLL_BIAS_PWD_MASK        0x00010000
#define RG_XPLL_BIAS_PWD             0x00010000
#define RG_XPLL_RESERVE_MASK         0x0000FF00
#define RG_XPLL_RESERVE(value)      ((((UINT32) value) << 8) & RG_XPLL_RESERVE_MASK)
#define RG_XPLL_DDS_HF_EN_MASK       0x00000080
#define RG_XPLL_DDS_HF_EN            0x00000080
#define RG_XPLL_SSC_EN_MASK          0x00000040
#define RG_XPLL_SSC_EN               0x00000040
#define RG_XPLL_FIFO_START_MAN_MASK  0x00000020
#define RG_XPLL_FIFO_START_MAN       0x00000020
#define RG_XPLL_DDS_RSTB_MASK        0x00000010
#define RG_XPLL_DDS_RSTB             0x00000010
#define RG_XPLL_DDS_PWDB_MASK        0x00000008
#define RG_XPLL_DDS_PWDB             0x00000008
#define RG_SSUSB_PLL_SSC_PHASE_INI_MASK    0x00000001
#define RG_SSUSB_PLL_SSC_PHASE_INI         0x00000001

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
#define XPLL_NCPO_VALUE    251658240    //416Mhz

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef struct _XPLL_INFO {
    UINT32    u4NcpoValue;
} XPLL_INFO, *P_XPLL_INFO;


//---for internal driver use----
typedef struct I2S_GBL_CFG {
    uint8_t             bI2SLoopBackEn;
    uint8_t             u4I2SInClkSel;
    uint8_t             u4I2SOutClkSel;
    uint8_t             bI2SClkInvEn;
    uint8_t             bI2SNegCapEn;
    uint8_t             bI2SDLMonoDupEn;
    uint8_t             bI2S_CODEC_26M_EN;
    uint8_t             bI2S_26M_SEL;
    uint8_t             ucI2SDLMonoStereoSel;
    uint8_t             ucI2SDLSwapLR;
    uint8_t             ucI2SExtSwapLR;
    uint8_t             ucI2SExtCodecSel;
} I2S_GBL_CFG, *P_I2S_GBL_CFG;

typedef struct I2S_DL_CFG {
    uint8_t             u4I2SChPerSample;
    uint32_t            u4I2SMsbOffset;
    uint8_t             u4I2SBitPerSample;
    uint8_t             u4I2SSampleRate;
    uint8_t             ucI2SWordSelInv;
    uint8_t             ucI2SFormat;
    uint8_t             ucI2SSRC;
    uint8_t             ucI2Sdl_fifo_2deq;//i2s hw fifo not dma fifo
} I2S_DL_CFG, *P_I2S_DL_CFG;

typedef struct I2S_UL_CFG {
    uint8_t             u4I2SLRSwap;
    uint8_t             u4I2SChPerSample;
    uint32_t            u4I2SUpdateWord;
    uint32_t            u4I2SMsbOffset;
    uint8_t             bI2SDownRateEn;
    uint8_t             u4I2SBitPerSample;
    uint8_t             u4I2SSampleRate;
    uint8_t             ucI2SWordSelInv;
    uint8_t             ucI2SFormat;
    uint8_t             ucI2SSRC;
} I2S_UL_CFG, *P_I2S_UL_CFG;



typedef enum _ENUM_I2S_INITIAL_TYPE_T {
    I2S_INTERNAL_LOOPBACK_MODE          = 0,
    I2S_INTERNAL_MODE,
    I2S_EXTERNAL_MODE,
    I2S_EXTERNAL_TDM_MODE,
    I2S_INITIAL_TYPE_NUM
} ENUM_I2S_INITIAL_TYPE_T;


typedef enum _ENUM_I2S_DL_SAMPLE_RATE_T {
    I2S_SAMPLE_RATE_8K               = I2S_SR_8K,
    I2S_SAMPLE_RATE_12K              = I2S_SR_12K,
    I2S_SAMPLE_RATE_16K              = I2S_SR_16K,
    I2S_SAMPLE_RATE_24K              = I2S_SR_24K,
    I2S_SAMPLE_RATE_32K              = I2S_SR_32K,
    I2S_SAMPLE_RATE_48K              = I2S_SR_48K
} ENUM_I2S_DL_SAMPLE_RATE_T;

typedef enum _ENUM_I2S_DL_MONO_STEREO_MODE_T {
    I2S_DL_MONO_MODE                    = I2S_DL_MONO,
    I2S_DL_STEREO_MODE                  = I2S_DL_STEREO
} ENUM_I2S_DL_MONO_STEREO_MODE_T;

typedef enum _ENUM_I2S_BIT_PER_SAMPLE_T {
    I2S_32BITS_PER_SAMPLE            = I2S_BIT_PER_S_32BTIS,
    I2S_64BITS_PER_SAMPLE            = I2S_BIT_PER_S_64BTIS,
    I2S_128BITS_PER_SAMPLE           = I2S_BIT_PER_S_128BTIS
} ENUM_I2S_DL_BIT_PER_SAMPLE_RATE_T;

typedef enum _ENUM_I2S_CH_PER_SAMPLE_T {
    I2S_2_CH_PER_SAMPLE              = I2S_CH_PER_S_2_CH,
    I2S_4_CH_PER_SAMPLE              = I2S_CH_PER_S_4_CH
} ENUM_I2S_UL_CH_PER_SAMPLE_T;


typedef struct {
    uint32_t           *tx_vfifo_base;  //< Tx buffer- internal use
    uint32_t            tx_vfifo_length;
    uint32_t            tx_vfifo_threshold;
    bool                tx_dma_interrupt;
    bool                tx_dma_configured;

    uint32_t           *rx_vfifo_base;  //< Rx buffer- internal use
    uint32_t            rx_vfifo_length;
    uint32_t            rx_vfifo_threshold;
    bool                rx_dma_interrupt;
    bool                rx_dma_configured;
} i2s_vfifo_t;


typedef struct {

    //---for recording value from setting of user
    ENUM_I2S_INITIAL_TYPE_T             I2S_INITIAL_TYPE;

    /*for original driver use*/
    I2S_GBL_CFG                         I2SGblCfg;
    I2S_DL_CFG                          I2SDLCfg;
    I2S_UL_CFG                          I2SULCfg;

    hal_i2s_initial_type_t              i2s_initial_type;
    /* structure for user use */
    hal_i2s_config_t                    i2s_user_config;

    /* state control */
    volatile uint8_t                   i2s_state;
    bool                               i2s_configured;
    bool                               i2s_audiotop_enabled;
    bool                               i2s_tx_eof;

    i2s_vfifo_t                        i2s_vfifo;
    /* user defined callback functions */
    hal_i2s_tx_callback_t               user_tx_callback_func;
    hal_i2s_rx_callback_t               user_rx_callback_func;
    void                                *user_tx_data;
    void                                *user_rx_data;
} i2s_internal_t;

//-----Austin Cheng-----end

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/
void xpllOpen(void);
void xpllClose(void);
//BOOLEAN xpllSetFineTuneValue(UINT32 u4FineTuneValue);
//XPLL_INFO xpllGetInfo(void);

/*
========================================================================
Routine Description:

Note:
========================================================================
*/
BOOLEAN  I2sVdmaTxStart(UINT32 u4TxBufAddr, UINT32 u4TxBufLen, DMA_VFIFO_CHANNEL TxCasPortAddr, BOOLEAN bTxCasEn);
BOOLEAN  I2sVdmaRxStart(UINT32 u4RxBufAddr, UINT32 u4RxBufLen, DMA_VFIFO_CHANNEL RxCasPortAddr, BOOLEAN bRxCasEn);
BOOLEAN  I2sSetVdmaTxIntrEn(BOOLEAN bI2sVdmaTxIntrEn);
BOOLEAN  I2sSetVdmaRxIntrEn(BOOLEAN bI2sVdmaRxIntrEn);
BOOLEAN  I2sVdmaTxStop(void);
BOOLEAN  I2sVdmaRxStop(void);
BOOLEAN  I2sSetVdmaTxThreshold(UINT32 tx_threshold);
BOOLEAN  I2sSetVdmaRxThreshold(UINT32 rx_threshold);
BOOLEAN  I2sWriteVdmaTxFifo(UINT32 u4Txdata);
BOOLEAN  I2sReadVdmaRxFifo(PUINT32 pu4Rxdata);
UINT16   I2sGetVdmaTxFifoCnt(void);
UINT16   I2sGetVdmaRxFifoCnt(void);

BOOLEAN  I2sSetGblCfg(P_I2S_GBL_CFG pI2sGblCfg);
BOOLEAN  I2sSetDlCfg(P_I2S_DL_CFG pI2sDlCfg);
BOOLEAN  I2sSetUlCfg(P_I2S_UL_CFG pI2sUlCfg);
BOOLEAN  I2sClkFifoEn(BOOLEAN bI2sEn);
BOOLEAN  I2sReset(void);
BOOLEAN  I2sDlEn(BOOLEAN bI2sEn);
BOOLEAN  I2sUlEn(BOOLEAN bI2sEn);

/*
BOOLEAN  I2sCfgTdmChBitPerSample(ENUM_I2S_DL_CH_PER_SAMPLE_T I2sDlChPerSample,
                                                                         ENUM_I2S_UL_CH_PER_SAMPLE_T I2sUlChPerSample,
                                                                         ENUM_I2S_DL_BIT_PER_SAMPLE_RATE_T I2sDlBitPerSample,
                                                                         ENUM_I2S_UL_BIT_PER_SAMPLE_RATE_T I2sUlBitPerSample,
                                                                         P_I2S_DL_CFG pI2sDlCfg, P_I2S_UL_CFG pI2sUlCfg);
BOOLEAN  I2sCfgMonoStereo(ENUM_I2S_DL_MONO_STEREO_MODE_T I2sDlMonoStereo, BOOLEAN bI2sDlMonoDupEn, P_I2S_GBL_CFG pI2sGblCfg);
BOOLEAN  I2sCfgSampleRate(ENUM_I2S_DL_SAMPLE_RATE_T I2sDlSampleRate, ENUM_I2S_UL_SAMPLE_RATE_T I2sUlSampleRate, BOOLEAN bI2sUlDownRateEn, P_I2S_DL_CFG pI2sDlCfg, P_I2S_UL_CFG pI2sUlCfg);
*/
BOOLEAN  I2sCfgInitialSetting(ENUM_I2S_INITIAL_TYPE_T I2sInitialType, P_I2S_GBL_CFG pI2sGblCfg, P_I2S_DL_CFG pI2sDlCfg, P_I2S_UL_CFG pI2sUlCfg);

#endif  /* defined(HAL_I2S_MODULE_ENABLED)*/

#endif //#ifndef __HAL_I2S_INTERNAL_H__
