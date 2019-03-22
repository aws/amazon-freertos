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
** $Log: hal_ADC.h $
**
** 08 24 2015 pierre.chang
** Add adc sequence released by DE for IoT.
**
** 06 03 2015 morris.yeh
** [ADC]
** refine API(if on-off period < 200ms, not turn off ALDO)
**
** 05 27 2015 morris.yeh
** [ADC]
** HEC verify
** 1. UART print will impact readwrite ptr & IIR info to check if the interrupt source is time out or trigger level
** 2. if use DMA mode, need to disable "RX full and time-out interrupt"->mSetHWEntry(ADC_RXFEN, 0);
**
** 04 30 2015 morris.yeh
** [ADC]
** Apply BUCK patch
**
** 04 28 2015 morris.yeh
** [ADC]
** refine code for 8ms test
** [CMM]
** query radio stats by index & size
**
** 04 16 2015 morris.yeh
** [ADC]
** refine code for ADC DVT
**
** 04 09 2015 morris.yeh
** [ADC]
** Add test code for HP ch10 & ch9
**
** 04 09 2015 morris.yeh
** [ADC]
** Add test code for CPU clock change
**
** 04 01 2015 morris.yeh
** [ADC]
** Turn on GDMA clock for ADC DMA mode work,HAL_REG_32(0x83000018) = 0x03FFFFFF;
**
** 03 30 2015 morris.yeh
** [ADC]
** Refine ADC code for DVT test
**
** 01 29 2015 morris.yeh
** [ADC]
** fix bug of ADC test code for always zero in the last element of ring buffer
**
** 01 12 2015 morris.yeh
** [ADC]
** Use 32bytes ADC sample instead of 16bytes
**
** 01 09 2015 morris.yeh
** [ADC]
** Re-Org HAL ADC driver
**
** 12 24 2014 morris.yeh
** [ADC]
** add DMA ADC test code
**
** 12 23 2014 morris.yeh
** [ADC]
** Add IoT ADC comparator test code
**
** 12 11 2014 morris.yeh
** [ADC][GPIO]
** 1. Add ADC test code
** 2. Add pull-up/pull-down code for GPIO
**
** 11 28 2014 morris.yeh
** [ADC]
** 1. Modify HAL ADC API prototype to meet MTK coding style
** 2. Integrate MSFT Indium ADC API with HAL ADC API
**
** 11 27 2014 pierre.chang
** Update ADC API.
**
** 11 24 2014 pierre.chang
** Update ADC definitions.
**
** 11 21 2014 pierre.chang
** Update ADC definition.
**
** 11 21 2014 pierre.chang
** Modify DMA API.
**
**
*/

#ifndef __LOW_HAL_ADC_H__
#define __LOW_HAL_ADC_H__

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "hal_adc.h"

#ifdef HAL_ADC_MODULE_ENABLED

//#include "mt7687_conf.h"
//#include "IOT7687.h"
#include "mt7687_cm4_hw_memmap.h"
#include "type_def.h"

#define ASIC_DVT    1
#define FPGA_DVT    !(ASIC_DVT)
#define CPU_CLK_DVT 0
//#define HP_DVT 1
#define AUDIO_DVT 0
#define DBG_8MS_DVT 0
#define BUCK_DVT 0  // 1 //taylor set to 0 for LP DVT
#define TIME_OUT_DVT 0
#define CHK_TIME_OUT_INT_TRIGER_LVL_INT_DVT 0

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*
 * TOP register
 */
#define IOT_GPIO_PAD_CTRL1                          (TOP_CFG_AON_BASE + 0x00000080)
#define IOT_GPIO_PAD_CTRL2                          (TOP_CFG_AON_BASE + 0x00000084)
#define IOT_GPIO_PAD_CTRL3                          (TOP_CFG_AON_BASE + 0x00000088)
#define TOP_GFG_AON_RG_PMU_03                       (TOP_CFG_AON_BASE + 0x0000040C)
#define TOP_GFG_AON_RG_PMU_04                       (TOP_CFG_AON_BASE + 0x00000410)

#define ADC_ALDO_EN_ADDR                            (0x81021438)
#define ADC_ALDO_EN_MASK                            BIT(1)
#define ADC_ALDO_EN_SHFT                            1

#define HCLK_2M_DIV_SEL_SW_ADDR                     (0x830081B4)
#define HCLK_2M_DIV_SEL_SW_MASK                     BIT(1)
#define HCLK_2M_DIV_SEL_SW_SHFT                     1

#define HCLK_2M_SW_DIV_SEL_ADDR                     (0x830081B4)
#define HCLK_2M_SW_DIV_SEL_MASK                     BITS(8,12)
#define HCLK_2M_SW_DIV_SEL_SHFT                     8

/*
 * ADC register
 */
#define TOP_CFG_ADC_AON_BASE                        0x83008180

#define ADC_FSM_EN_ADDR                             (TOP_CFG_ADC_AON_BASE + 0x0000)
#define ADC_FSM_EN_MASK                             BIT(0)
#define ADC_FSM_EN_SHFT                             0

#define ADC_REG_AVG_MODE_ADDR                       (TOP_CFG_ADC_AON_BASE + 0x0000)
#define ADC_REG_AVG_MODE_MASK                       BITS(1,3)
#define ADC_REG_AVG_MODE_SHFT                       1

#define ADC_REG_T_CH_ADDR                           (TOP_CFG_ADC_AON_BASE + 0x0000)
#define ADC_REG_T_CH_MASK                           BITS(4,7)
#define ADC_REG_T_CH_SHFT                           4

#define ADC_PMODE_EN_ADDR                           (TOP_CFG_ADC_AON_BASE + 0x0000)
#define ADC_PMODE_EN_MASK                           BIT(8)
#define ADC_PMODE_EN_SHFT                           8

#define ADC_REG_T_INIT_ADDR                         (TOP_CFG_ADC_AON_BASE + 0x0000)
#define ADC_REG_T_INIT_MASK                         BITS(9,15)
#define ADC_REG_T_INIT_SHFT                         9

#define ADC_REG_CH_MAP_ADDR                         (TOP_CFG_ADC_AON_BASE + 0x0000)
#define ADC_REG_CH_MAP_MASK                         BITS(16,31)
#define ADC_REG_CH_MAP_SHFT                         16

#define ADC_REG_PERIOD_ADDR                         (TOP_CFG_ADC_AON_BASE + 0x0004)
#define ADC_REG_PERIOD_MASK                         BITS(0,31)
#define ADC_REG_PERIOD_SHFT                         0

#define ADC_REG_COMP_IRQ_EN_ADDR                    (TOP_CFG_ADC_AON_BASE + 0x0008)
#define ADC_REG_COMP_IRQ_EN_MASK                    BITS(0,3)
#define ADC_REG_COMP_IRQ_EN_SHFT                    0

#define ADC_REG_COMP_THRESHOLD_ADDR                 (TOP_CFG_ADC_AON_BASE + 0x0008)
#define ADC_REG_COMP_THRESHOLD_MASK                 BITS(4,15)
#define ADC_REG_COMP_THRESHOLD_SHFT                 4

#define ADC_RO_ADC_COMP_FLAG_ADDR                   (TOP_CFG_ADC_AON_BASE + 0x0008)
#define ADC_RO_ADC_COMP_FLAG_MASK                   BITS(16,19)
#define ADC_RO_ADC_COMP_FLAG_SHFT                   16

#define ADC_REG_ADC_DATA_SYNC_MODE_ADDR             (TOP_CFG_ADC_AON_BASE + 0x0008)
#define ADC_REG_ADC_DATA_SYNC_MODE_MASK             BIT(20)
#define ADC_REG_ADC_DATA_SYNC_MODE_SHFT             20

#define ADC_REG_ADC_TIMESTAMP_EN_ADDR               (TOP_CFG_ADC_AON_BASE + 0x0008)
#define ADC_REG_ADC_TIMESTAMP_EN_MASK               BIT(21)
#define ADC_REG_ADC_TIMESTAMP_EN_SHFT               21

#define ADC_REG_INTERNAL_CLK_SAMPLE_DATA_EN_ADDR    (TOP_CFG_ADC_AON_BASE + 0x0008)
#define ADC_REG_INTERNAL_CLK_SAMPLE_DATA_EN_MASK    BIT(22)
#define ADC_REG_INTERNAL_CLK_SAMPLE_DATA_EN_SHFT    22

#define ADC_REG_AUXADC_COMP_DELAY_TIME_ADDR         (TOP_CFG_ADC_AON_BASE + 0x000C)
#define ADC_REG_AUXADC_COMP_DELAY_TIME_MASK         BITS(0,1)
#define ADC_REG_AUXADC_COMP_DELAY_TIME_SHFT         0

#define ADC_REG_AUXADC_COMP_PREAMP_CURRENT_ADDR     (TOP_CFG_ADC_AON_BASE + 0x000C)
#define ADC_REG_AUXADC_COMP_PREAMP_CURRENT_MASK     BITS(2,3)
#define ADC_REG_AUXADC_COMP_PREAMP_CURRENT_SHFT     2

#define ADC_REG_AUXADC_COMP_PREAMP_EN_ADDR          (TOP_CFG_ADC_AON_BASE + 0x000C)
#define ADC_REG_AUXADC_COMP_PREAMP_EN_MASK          BIT(4)
#define ADC_REG_AUXADC_COMP_PREAMP_EN_SHFT          4

#define ADC_REG_AUXADC_DITHERING_EN_ADDR            (TOP_CFG_ADC_AON_BASE + 0x000C)
#define ADC_REG_AUXADC_DITHERING_EN_MASK            BIT(6)
#define ADC_REG_AUXADC_DITHERING_EN_SHFT            6

#define ADC_REG_AUXADC_INVERT_PMU_CLK_ADDR          (TOP_CFG_ADC_AON_BASE + 0x000C)
#define ADC_REG_AUXADC_INVERT_PMU_CLK_MASK          BIT(16)
#define ADC_REG_AUXADC_INVERT_PMU_CLK_SHFT          16

#define ADC_REG_AUXADC_ADC_CLK_SRC_ADDR             (TOP_CFG_ADC_AON_BASE + 0x000C)
#define ADC_REG_AUXADC_ADC_CLK_SRC_MASK             BIT(17)
#define ADC_REG_AUXADC_ADC_CLK_SRC_SHFT             17

#define ADC_REG_AUXADC_ADC_CLK_CTRL_MUX_ADDR        (TOP_CFG_ADC_AON_BASE + 0x0010)
#define ADC_REG_AUXADC_ADC_CLK_CTRL_MUX_MASK        BITS(28, 29)
#define ADC_REG_AUXADC_ADC_CLK_CTRL_MUX_SHFT        28

#define ADC_REG_AUXADC_ADC_CLK_CTRL_INV_EN_ADDR     (TOP_CFG_ADC_AON_BASE + 0x0010)
#define ADC_REG_AUXADC_ADC_CLK_CTRL_INV_EN_MASK     BIT(31)
#define ADC_REG_AUXADC_ADC_CLK_CTRL_INV_EN_SHFT     31

#define ADC_RBR_ADDR                                (CM4_ADC_BASE + 0x0000)
#define ADC_RBR_MASK                                BITS(0,31)
#define ADC_RBR_SHFT                                0

#define ADC_RXFEN_ADDR                              (CM4_ADC_BASE + 0x0004)
#define ADC_RXFEN_MASK                              BIT(0)
#define ADC_RXFEN_SHFT                              0

#define ADC_IIR_ADDR                                (CM4_ADC_BASE + 0x0008)
#define ADC_IIR_MASK                                BITS(0,3)
#define ADC_IIR_SHFT                                0

#define ADC_CLRR_ADDR                               (CM4_ADC_BASE + 0x0008)
#define ADC_CLRR_MASK                               BIT(2)
#define ADC_CLRR_SHFT                               2

#define ADC_FAKELCR_ADDR                            (CM4_ADC_BASE + 0x000C)
#define ADC_FAKELCR_MASK                            BITS(0,7)
#define ADC_FAKELCR_SHFT                            0

#define ADC_LSR_DR_ADDR                             (CM4_ADC_BASE + 0x0014)
#define ADC_LSR_DR_MASK                             BIT(0)
#define ADC_LSR_DR_SHFT                             0

#define ADC_SLEEP_EN_ADDR                           (CM4_ADC_BASE + 0x0048)
#define ADC_SLEEP_EN_MASK                           BIT(0)
#define ADC_SLEEP_EN_SHFT                           0

#define ADC_RX_DMA_EN_ADDR                          (CM4_ADC_BASE + 0x004C)
#define ADC_RX_DMA_EN_MASK                          BIT(0)
#define ADC_RX_DMA_EN_SHFT                          0

#define ADC_TO_CNT_AUTORST_ADDR                     (CM4_ADC_BASE + 0x004C)
#define ADC_TO_CNT_AUTORST_MASK                     BIT(2)
#define ADC_TO_CNT_AUTORST_SHFT                     2

#define ADC_RTOCNT_ADDR                             (CM4_ADC_BASE + 0x0054)
#define ADC_RTOCNT_MASK                             BITS(0,7)
#define ADC_RTOCNT_SHFT                             0

#define ADC_DEBUG_RX_FIFO_6_ADDR                    (CM4_ADC_BASE + 0x0058)
#define ADC_DEBUG_RX_FIFO_6_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_6_SHFT                    0

#define ADC_RX_TRI_LVL_ADDR                         (CM4_ADC_BASE + 0x0060)
#define ADC_RX_TRI_LVL_MASK                         BITS(3,6)
#define ADC_RX_TRI_LVL_SHFT                         3

#define ADC_LOOP_ADDR                               (CM4_ADC_BASE + 0x0060)
#define ADC_LOOP_MASK                               BIT(7)
#define ADC_LOOP_SHFT                               7

#define ADC_WAK_ADDR                                (CM4_ADC_BASE + 0x0064)
#define ADC_WAK_MASK                                BIT(0)
#define ADC_WAK_SHFT                                0

#define ADC_WAT_TIME_1_ADDR                         (CM4_ADC_BASE + 0x0068)
#define ADC_WAT_TIME_1_MASK                         BITS(0,2)
#define ADC_WAT_TIME_1_SHFT                         0

#define ADC_WAT_TIME_2_ADDR                         (CM4_ADC_BASE + 0x0068)
#define ADC_WAT_TIME_2_MASK                         BITS(3,5)
#define ADC_WAT_TIME_2_SHFT                         3

#define ADC_HANDSHAKE_EN_ADDR                       (CM4_ADC_BASE + 0x006C)
#define ADC_HANDSHAKE_EN_MASK                       BIT(0)
#define ADC_HANDSHAKE_EN_SHFT                       0

#define ADC_HIGH_SPEED_EN_ADDR                      (CM4_ADC_BASE + 0x006C)
#define ADC_HIGH_SPEED_EN_MASK                      BIT(1)
#define ADC_HIGH_SPEED_EN_SHFT                      1

#define ADC_RTO_EXT_ADDR                            (CM4_ADC_BASE + 0x006C)
#define ADC_RTO_EXT_MASK                            BIT(2)
#define ADC_RTO_EXT_SHFT                         2

#define ADC_DEBUG_RX_FIFO_0_ADDR                    (CM4_ADC_BASE + 0x0070)
#define ADC_DEBUG_RX_FIFO_0_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_0_SHFT                    0

#define ADC_DEBUG_RX_FIFO_1_ADDR                    (CM4_ADC_BASE + 0x0074)
#define ADC_DEBUG_RX_FIFO_1_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_1_SHFT                    0

#define ADC_DEBUG_RX_FIFO_2_ADDR                    (CM4_ADC_BASE + 0x0078)
#define ADC_DEBUG_RX_FIFO_2_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_2_SHFT                    0

#define ADC_DEBUG_RX_FIFO_3_ADDR                    (CM4_ADC_BASE + 0x007C)
#define ADC_DEBUG_RX_FIFO_3_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_3_SHFT                    0

#define ADC_DEBUG_RX_FIFO_4_ADDR                    (CM4_ADC_BASE + 0x0080)
#define ADC_DEBUG_RX_FIFO_4_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_4_SHFT                    0

#define ADC_DEBUG_RX_FIFO_5_ADDR                    (CM4_ADC_BASE + 0x0084)
#define ADC_DEBUG_RX_FIFO_5_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_5_SHFT                    0

#define ADC_DEBUG_RX_FIFO_7_ADDR                    (CM4_ADC_BASE + 0x008C)
#define ADC_DEBUG_RX_FIFO_7_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_7_SHFT                    0

#define ADC_DEBUG_RX_FIFO_8_ADDR                    (CM4_ADC_BASE + 0x0090)
#define ADC_DEBUG_RX_FIFO_8_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_8_SHFT                    0

#define ADC_DEBUG_RX_FIFO_9_ADDR                    (CM4_ADC_BASE + 0x0094)
#define ADC_DEBUG_RX_FIFO_9_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_9_SHFT                    0

#define ADC_DEBUG_RX_FIFO_A_ADDR                    (CM4_ADC_BASE + 0x0098)
#define ADC_DEBUG_RX_FIFO_A_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_A_SHFT                    0

#define ADC_DEBUG_RX_FIFO_B_ADDR                    (CM4_ADC_BASE + 0x009C)
#define ADC_DEBUG_RX_FIFO_B_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_B_SHFT                    0

#define ADC_DEBUG_RX_FIFO_C_ADDR                    (CM4_ADC_BASE + 0x00A0)
#define ADC_DEBUG_RX_FIFO_C_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_C_SHFT                    0

#define ADC_DEBUG_RX_FIFO_D_ADDR                    (CM4_ADC_BASE + 0x00A4)
#define ADC_DEBUG_RX_FIFO_D_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_D_SHFT                    0

#define ADC_DEBUG_RX_FIFO_E_ADDR                    (CM4_ADC_BASE + 0x00A8)
#define ADC_DEBUG_RX_FIFO_E_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_E_SHFT                    0

#define ADC_DEBUG_RX_FIFO_F_ADDR                    (CM4_ADC_BASE + 0x00AC)
#define ADC_DEBUG_RX_FIFO_F_MASK                    BITS(0,31)
#define ADC_DEBUG_RX_FIFO_F_SHFT                    0

#define ADC_RX_PTR_ADDR                             (CM4_ADC_BASE + 0x00D4)
#define ADC_RX_PTR_MASK                             BITS(0,7)
#define ADC_RX_PTR_SHFT                             0

#define ADC_RX_PTR_READ_ADDR                        (CM4_ADC_BASE + 0x00D4)
#define ADC_RX_PTR_READ_MASK                        BITS(0,3)
#define ADC_RX_PTR_READ_SHFT                        0

#define ADC_RX_PTR_WRITE_ADDR                       (CM4_ADC_BASE + 0x00D4)
#define ADC_RX_PTR_WRITE_MASK                       BITS(4,7)
#define ADC_RX_PTR_WRITE_SHFT                       4

#define MAX_CH_NUM   16
#define ADC_FIFO_SIZE   16
#define ADC_DMA_BUF_WORD_SIZE  64
#define COUNT_8MS   1000
/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef enum {
    ADC_PMODE_ONE_TIME = 0,
    ADC_PMODE_PERIODIC
} t_Adc_Pmode;

typedef enum {
    ADC_AVG_1_SAMPLE = 0,
    ADC_AVG_2_SAMPLE,
    ADC_AVG_4_SAMPLE,
    ADC_AVG_8_SAMPLE,
    ADC_AVG_16_SAMPLE,
    ADC_AVG_32_SAMPLE,
    ADC_AVG_64_SAMPLE,
} t_Adc_Avg_Mode;

typedef enum {
    ADC_FIFO_DIRECT = 0,
    ADC_FIFO_DMA
} t_Adc_Fifo_Mode;

typedef struct _ADC_FSM_PARAM_T {
    t_Adc_Pmode     ePmode;
    t_Adc_Avg_Mode  eAvg_Mode;
    UINT16          u2Channel_Map;
    UINT32          u4Period;
    t_Adc_Fifo_Mode eFifo_Mode;
    UINT32         *pru4DmaVfifoAddr;
    UINT32          u4DmaVfifoLen;
    UINT32          u4DmaVfifoTriggerLevel;
} ADC_FSM_PARAM_T, *P_ADC_FSM_PARAM_T;

typedef struct _ADC_INFO_T {
    ADC_FSM_PARAM_T rFsmParam;
    UINT8           u1ChnlNum;
    //UINT32          aru4AdcDmaBuf[ADC_DMA_BUF_WORD_SIZE];
    //UINT32          u48msCnt;
    //UINT32          u48msCntFiFo;
} ADC_INFO_T, *P_ADC_INFO_T;

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/


/*
========================================================================
Routine Description:

Note:
========================================================================
*/


void halADC_VDMA_Callback(void);
ENUM_HAL_RET_T halADC_Fsm_Enable(void);
ENUM_HAL_RET_T halADC_Fsm_Disable(BOOLEAN fgOffAldo);
ENUM_HAL_RET_T halADC_Fsm_Param_Set(P_ADC_FSM_PARAM_T prAdcFsmParam);
ENUM_HAL_RET_T halADC_Fsm_Param_Get(P_ADC_FSM_PARAM_T prAdcFsmParam);
ENUM_HAL_RET_T halADC_Fifo_Get_Length(UINT32 *pru4Len);
ENUM_HAL_RET_T halADC_Fifo_Read_Data(UINT32 u4Len, UINT32 *pru4Samples);


extern UINT32 CHIP_ADC_GetAdcData(UINT8 ucChId);
extern void CHIP_ADC_Periodic_DMA_MultiCh_Test(void);
extern void CHIP_ADC_Periodic_MultiCh_Test(void);
extern void CHIP_ADC_Periodic_DMA_MultiCh8Ch15_Test(void);
extern void CHIP_ADC_Periodic_MultiCh8Ch15_Test(void);
extern void cmnCpuClkConfigureToXtal(void);
extern void cmnCpuClkConfigureTo192M(void);
extern void cmnCpuClkConfigureTo160M(void);
extern void cmnCpuClkConfigureTo64M(void);

#endif //#ifdef HAL_ADC_MODULE_ENABLED

#endif //#ifndef __LOW_HAL_ADC_H__

