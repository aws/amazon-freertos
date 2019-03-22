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

#ifndef __MT7687_HW_MEMMAP_H__
#define __MT7687_HW_MEMMAP_H__
#include "type_def.h"
#include "memory_attribute.h"
/* AON */
#define TOP_CFG_AON_BASE		0x81021000

/* CM4 AHB */
#define CM4_TCMROM_BASE			0x00000000
#define CM4_TCMRAM_BASE			0x00100000	/* TCM Start */
#define CM4_TCMRAM_END			0x00118000	/* TCM End -- include cache space */
#define CM4_CACHE_CTRL_BASE		0x01530000
#define CM4_CACHE_MPU_BASE		0x01540000
#define CM4_EXEC_IN_PLACE_BASE		0x10000000	/* XIP */
#define CM4_EXEC_IN_PLACE_END		0x11000000	/* XIP End -- 16MB */
#define CM4_SYSRAM_BASE			0x20000000
#define CM4_SYSRAM_END			0x20040000
#define CM4_SPISLAVE_BASE		0x21000000
#define CM4_AUDIO_BASE			0x22000000
#define CM4_HSPI_BASE			0x23000000
#define CM4_SPIMASTER_BASE		0x24000000
#define CM4_SPIFLASH_BASE		0x30000000	/* remap to CM4_EXEC_IN_PLACE_BASE */

/* CM4 APB2 peripheral */
#define CM4_CONFIG_BASE			0x83000000
#define CM4_TOPCFGAON_BASE      0x83008000
#define CM4_TOPRGU_BASE         0x83009000
#define IOT_PWM_BASE            (0x8300A600)
#define CM4_CONFIG_AON_BASE     (0x8300C000)
#define CM4_DMA_BASE			0x83010000
#define CM4_UART_DSN_BASE		0x83020000
#define CM4_UART1_BASE			0x83030000
#define CM4_UART2_BASE			0x83040000
#define CM4_GPT_BASE                    0x83050000
#define CM4_IRDA_TX_BASE		0x83060000
#define CM4_IRDA_RX_BASE		0x83068000
#define CM4_SFC_BASE			0x83070000	/* Serial Flash Controller */
#define CM4_WDT_BASE			0x83080000
#define CM4_I2C1_BASE			0x83090000
#define CM4_I2C2_BASE			0x830A0000
#define CM4_I2S_BASE			0x830B0000
#define CM4_RTC_BASE			0x830C0000
#define CM4_ADC_BASE			0x830D0000
#define CM4_BTIF_BASE			0x830E0000
#define CM4_MTK_CRYPTO_BASE		0x830F0000

#define IOT_GPIO_PINMUX_AON_BASE    (0x81023000)
#define IOT_GPIO_PINMUX_OFF_BASE    (0x80025100)

#define MCU_CFG_NVIC_BASE           (0xE000E000)
#define MCU_CFG_DEBOUNCE_BASE       (0x8300c000)
#define MCU_CFG_NVIC_SENSE_BASE     (0x8300c000)

#define IOT_GPIO_OFF_BASE           (0x80025000)
#define IOT_GPIO_AON_BASE           (0x8300B000)

/* N9 APB1 peripheral */
#define EFUSE_base               (0x81070000)

/* USB */
#define SSUSB_MAC_base           (0x50301000)
#define SSUSB_DEV_base           (0x50302000)
#define SSUSB_EPCTL_base         (0X50302800)
#define SSUSB_USB3_MAC_CSR_base  (0x50303400)
#define SSUSB_USB3_SYS_CSR_base  (0x50303400)
#define USB_USB2_CSR_base        (0x50304400)
#define SSUSB_SIFSLV_IPPC_base   (0x50305700)
#define SSUSB_USB20_PHY_BASE     (0x50305800)
#define SSUSB_SIFSLV_U3PHYA_BASE (0x50305b00)
#define USB3_UDMA_base           (0x50309000)


/* TOP SYS module registers */

#define CONFG_HVR_ADDR    (0x00000000 + CM4_CONFIG_BASE)
#define CONFG_HVR_MASK    0x0000FFFF
#define CONFG_HVR_SHFT    0

#define CONFG_FVR_ADDR    (0x00000004 + CM4_CONFIG_BASE)
#define CONFG_FVR_MASK    0x0000FFFF
#define CONFG_FVR_SHFT    0

#define CONFG_HCR_ADDR    (0x00000008 + CM4_CONFIG_BASE)
#define CONFG_HCR_MASK    0x0000FFFF
#define CONFG_HCR_SHFT    0


#define CONFG_TEST_ADDR    (0x00000010 + CM4_CONFIG_BASE)
#define CONFG_TEST_MASK    0xFFFFFFFF
#define CONFG_TEST_SHFT    0

#define CONFG_MCCR_ADDR         (0x00000010 + CM4_CONFIG_BASE)
#define CONFG_MCCR_MASK         0x03FFFFFF
#define CONFG_MCCR_SHFT         0

#define CONFG_MCCR_SET_ADDR     (0x00000014 + CM4_CONFIG_BASE)
#define CONFG_MCCR_SET_MASK     0x01FFFFFF
#define CONFG_MCCR_SET_SHFT     0

#define CONFG_MCCR_CLR_ADDR     (0x00000018 + CM4_CONFIG_BASE)
#define CONFG_MCCR_CLR_MASK     0x01FFFFFF
#define CONFG_MCCR_CLR_SHFT     0

#define TOP_CFG_AON_N9_MESSAGE_ADDR                         (0x00000050 + TOP_CFG_AON_BASE)
#define TOP_CFG_AON_N9_MESSAGE_MASK                         0xFFFFFFFF
#define TOP_CFG_AON_N9_MESSAGE_SHFT                         0

#define TOP_CFG_AON_N9_MESSAGE_N9_WIFI_ADDR                 (0x00000050 + TOP_CFG_AON_BASE)
#define TOP_CFG_AON_N9_MESSAGE_N9_WIFI_MASK                 0x00000001
#define TOP_CFG_AON_N9_MESSAGE_N9_WIFI_SHFT                 0

#define TOP_CFG_AON_N9_CM4_MESSAGE_ADDR                     (0x00000054 + TOP_CFG_AON_BASE)
#define TOP_CFG_AON_N9_CM4_MESSAGE_MASK                     0xFFFFFFFF
#define TOP_CFG_AON_N9_CM4_MESSAGE_SHFT                     0

#define TOP_CFG_AON_N9_CM4_MESSAGE_CM4_ADC_ADDR             (0x00000054 + TOP_CFG_AON_BASE)
#define TOP_CFG_AON_N9_CM4_MESSAGE_CM4_ADC_MASK             0x00000002
#define TOP_CFG_AON_N9_CM4_MESSAGE_CM4_ADC_SHFT             1

#define TOP_CFG_AON_N9_CM4_MESSAGE_CM4_AUDIO_ADDR           (0x00000054 + TOP_CFG_AON_BASE)
#define TOP_CFG_AON_N9_CM4_MESSAGE_CM4_AUDIO_MASK           0x00000004
#define TOP_CFG_AON_N9_CM4_MESSAGE_CM4_AUDIO_SHFT           2

#define TOP_CFG_AON_PMU_RG_BUCK_FORCE_PWM_ADDR              (0x0000040C + TOP_CFG_AON_BASE)
#define TOP_CFG_AON_PMU_RG_BUCK_FORCE_PWM_MASK              0x00404000
#define TOP_CFG_AON_PMU_RG_BUCK_FORCE_PWM_SHFT              14

/******************************************************************************/
/*                Device Specific Peripheral registers structures             */
/******************************************************************************/

typedef enum _XTAL_FREQ_T {
    XTAL_FREQ_20M = 0,
    XTAL_FREQ_40M = 1,
    XTAL_FREQ_26M = 2,
    XTAL_FREQ_52M = 3,
} XTAL_FREQ_T, *P_XTAL_FREQ_T;


// IOT
typedef union _PWM_GLO_CTRL_T {
    struct {
        UINT32    global_kick             : 1;    // All PWM modules with "pwm_global_kick_enable" would be kicked by this bit at the same time
        UINT32    PWM_tick_clock_sel      : 2;    // 0: 32KHz 1: 2MHz 2: XTAL clock
        UINT32    reserve3_31             : 29;   // Reserved
    } field;
    UINT32 word;
} PWM_GLO_CTRL_T, *P_PWM_GLO_CTRL_T;

typedef struct {
    volatile UINT32 PWM_CTRL;
    volatile UINT32 PWM_PARAM_S0;
    volatile UINT32 PWM_PARAM_S1;
} IOT_PWM_TypeDef, *P_IOT_PWM_TypeDef;

typedef union _PWM_CTRL_T {
    struct {
        UINT32    kick                    : 1;    // Module load PWM parameter setting and generate waveform
        UINT32    replay_mode             : 1;    // replay_mode	Replay mode indication (Only available when S1 exists)
        UINT32    polarity                : 1;    // PWM polarity setting, 0: active high 1:active low
        UINT32    pwm_io_ctrl             : 1;    // 0: PIO (as output) 1: open drain (as output when active low)
        UINT32    pwm_clock_gated         : 1;    // 1: Gating tick clock for PWM
        UINT32    pwm_global_kick_enable  : 1;    // PWM would be kicked by global kick if this bit is set
        UINT32    reserve6_7              : 2;    // Reserved
        UINT32    S0_stay_cycle           : 12;   // The stay cycles of S0
        UINT32    S1_stay_cycle           : 12;   // The stay cycles of S1 (If this field is 0, S1 does not exist)
    } field;
    UINT32 word;
} PWM_CTRL_T, *P_PWM_CTRL_T;

typedef union _PWM_PARAM_S_T {
    struct {
        UINT32    S_pwm_on_time          : 16;   // State PWM_ON duration     (unit: tick clock period)
        UINT32    S_pwm_off_time         : 16;   // State PWM_OFF duration     (unit: tick clock period)
    } field;
    UINT32 word;
} PWM_PARAM_S_T, *P_PWM_PARAM_S_T;

/*------------- Inter-Integrated Circuit (I2C) -------------------------------*/
typedef struct _IOT_I2C_TypeDef {
    volatile UINT32 MM_PAD_CON0;
    volatile UINT32 MM_CNT_VAL_PHL;
    volatile UINT32 MM_CNT_VAL_PHH;
    volatile UINT32 MM_CNT_VAL_HS_PHL;
    volatile UINT32 MM_CNT_VAL_HS_PHH;
    volatile UINT32 MM_CNT_BYTE_VAL_PK0;
    volatile UINT32 MM_CNT_BYTE_VAL_PK1;
    volatile UINT32 MM_CNT_BYTE_VAL_PK2;
    volatile UINT32 MM_ID_CON0;
    volatile UINT32 MM_ID_CON1;
    volatile UINT32 MM_PACK_CON0;
    volatile UINT32 MM_ACK_VAL;
    volatile UINT32 MM_CON0;
    volatile UINT32 MM_STATUS;
    volatile UINT32 MM_FIFO_CON0;
    volatile UINT32 MM_FIFO_DATA;
    volatile UINT32 MM_FIFO_STATUS;
    volatile UINT32 MM_FIFO_PTR;
} IOT_I2C_TypeDef, *P_IOT_I2C_TypeDef;




// IOT PWM
#define IOT_PWM_GLO_CTRL_ADDR   ((PUINT8)(IOT_PWM_BASE + 0x0000))
#define IOT_PWM0_ADDR           ((IOT_PWM_TypeDef *)(IOT_PWM_BASE + 0x0100))
#define IOT_PWM_OFFSET          (0x10)
#define IOT_PWM_MAX_NUM         (40)

// PWM_GLO_CTRL
#define PWM_GLO_CTRL_PWM_GLOBAL_RESET_OFFSET        (3)
#define PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_OFFSET      (1)
#define PWM_GLO_CTRL_PWM_TICK_CLOCK_SEL_MASK        (BITS(1, 2))
#define PWM_GLO_CTRL_GLOBAL_KICK_OFFSET             (0)

// PWM_CTRL
#define PWM_CTRL_S1_STAY_CYCLE_OFFSET               (20)
#define PWM_CTRL_S1_STAY_CYCLE_MASK                 (BITS(20, 31))
#define PWM_CTRL_S0_STAY_CYCLE_OFFSET               (8)
#define PWM_CTRL_S0_STAY_CYCLE_MASK                 (BITS(8, 19))
#define PWM_CTRL_PWM_GLOBAL_KICK_ENABLE_OFFSET      (5)
#define PWM_CTRL_PWM_CLOCK_EN_OFFSET                (4)
#define PWM_CTRL_PWM_IO_CTRL_OFFSET                 (3)
#define PWM_CTRL_POLARITY_OFFSET                    (2)
#define PWM_CTRL_REPLAY_MODE_OFFSET                 (1)
#define PWM_CTRL_KICK_OFFSET                        (0)

// PWM_PARAM_S0
#define PWM_PARAM_S0_PWM_OFF_TIME_OFFSET            (16)
#define PWM_PARAM_S0_PWM_OFF_TIME_MASK              (BITS(16, 31))
#define PWM_PARAM_S0_PWM_ON_TIME_OFFSET             (0)
#define PWM_PARAM_S0_PWM_ON_TIME_MASK               (BITS(0, 15))

// PWM_PARAM_S1
#define PWM_PARAM_S1_PWM_OFF_TIME_OFFSET            (16)
#define PWM_PARAM_S1_PWM_OFF_TIME_MASK              (BITS(16, 31))
#define PWM_PARAM_S1_PWM_ON_TIME_OFFSET             (0)
#define PWM_PARAM_S1_PWM_ON_TIME_MASK               (BITS(0, 15))


// IOT I2C
#define IOT_I2C0_MM_PAD_CON0            ((IOT_I2C_TypeDef   *)(CM4_I2C1_BASE + 0x0240))
/* defined in IOT_I2C_TypeDef
#define IOT_I2C0_MM_CNT_VAL_PHL         ((CM4_I2C1_BASE + 0x0244))
#define IOT_I2C0_MM_CNT_VAL_PHH         ((CM4_I2C1_BASE + 0x0248))
#define IOT_I2C0_MM_CNT_VAL_HS_PHL      ((CM4_I2C1_BASE + 0x024C))
#define IOT_I2C0_MM_CNT_VAL_HS_PHH      ((CM4_I2C1_BASE + 0x0250))
#define IOT_I2C0_MM_CNT_BYTE_VAL_PK0    ((CM4_I2C1_BASE + 0x0254))
#define IOT_I2C0_MM_CNT_BYTE_VAL_PK1    ((CM4_I2C1_BASE + 0x0258))
#define IOT_I2C0_MM_CNT_BYTE_VAL_PK2    ((CM4_I2C1_BASE + 0x025C))
#define IOT_I2C0_MM_ID_CON0             ((CM4_I2C1_BASE + 0x0260))
#define IOT_I2C0_MM_ID_CON1             ((CM4_I2C1_BASE + 0x0264))
#define IOT_I2C0_MM_PACK_CON0           ((CM4_I2C1_BASE + 0x0268))
#define IOT_I2C0_MM_ACK_VAL             ((CM4_I2C1_BASE + 0x026C))
#define IOT_I2C0_MM_CON0                ((CM4_I2C1_BASE + 0x0270))
#define IOT_I2C0_MM_STATUS              ((CM4_I2C1_BASE + 0x0274))
#define IOT_I2C0_MM_FIFO_CON0           ((CM4_I2C1_BASE + 0x0278))
#define IOT_I2C0_MM_FIFO_DATA           ((CM4_I2C1_BASE + 0x027C))
#define IOT_I2C0_MM_FIFO_STATUS         ((CM4_I2C1_BASE + 0x0280))
#define IOT_I2C0_MM_FIFO_PTR            ((CM4_I2C1_BASE + 0x0284))
*/
#define IOT_I2C0_DMA_CON0               ((CM4_I2C1_BASE + 0x02C0)) // NOT continuous address
#define IOT_I2C0_RESERVED0              ((CM4_I2C1_BASE + 0x02FC)) // NOT continuous address

#define IOT_I2C1_MM_PAD_CON0            ((IOT_I2C_TypeDef   *)(CM4_I2C2_BASE + 0x0240))
#define IOT_I2C1_DMA_CON0               ((CM4_I2C2_BASE + 0x02C0)) // NOT continuous address
#define IOT_I2C1_RESERVED0              ((CM4_I2C2_BASE + 0x02FC)) // NOT continuous address

#define I2C_FIFO_MAX_LEN        (8)
#define I2C_PKT_MAX_NUM         (3)

// 0x240 MM_PAD_CON0
#define I2C_DE_CNT_OFFSET       (0)
#define I2C_DE_CNT_MASK         (BITS(0, 4))
#define I2C_SCL_DRVH_EN_OFFSET  (5)
#define I2C_SDA_DRVH_EN_OFFSET  (6)
#define I2C_CLK_SYNC_EN_OFFSET  (7)

// 0x244 MM_CNT_VAL_PHL
#define I2C_MM_CNTPHASE_VAL1_OFFSET (8)
#define I2C_MM_CNTPHASE_VAL0_OFFSET (0)

// 0x248 MM_CNT_VAL_PHH
#define I2C_MM_CNTPHASE_VAL3_OFFSET (8)
#define I2C_MM_CNTPHASE_VAL2_OFFSET (0)

// 0x260 MM_ID_CON0
#define I2C_MM_SLAVE_ID_OFFSET      (0)
#define I2C_MM_SLAVE_ID_MASK        (BITS(0, 6))

// 0x268 MM_PACK_CON0
#define I2C_MM_PACK_RW_OFFSET       (0)
#define I2C_MM_PACK_RW_MASK         (BITS(0, 3))
#define I2C_MM_PACK_VAL_OFFSET      (4)
#define I2C_MM_PACK_VAL_MASK        (BITS(4, 5))

// 0x26C MM_ACK_VAL
#define I2C_ACK_PKT0_OFFSET         (8)
#define I2C_ACK_PKT1_OFFSET         (9)
#define I2C_ACK_PKT2_OFFSET         (10)
#define I2C_MM_ACK_DATA_MASK        (BITS(0, 7))

// 0x270 MM_CON0
#define I2C_MM_START_EN_OFFSET  (0)
#define I2C_MCU_SEL_OFFSET      (11)
#define I2C_MM_TB_EN_OFFSET     (12)
#define I2C_MM_HS_EN_OFFSET     (13)
#define I2C_MM_GMODE_OFFSET     (14)
#define I2C_MASTER_EN_OFFSET    (15)

// 0x274 MM_STATUS
#define I2C_BUS_BUSY_OFFSET         (0)
#define I2C_MM_ARB_HAD_LOSE_OFFSET  (1)
#define I2C_MM_START_READY_OFFSET   (2)

// 0x278 MM_FIFO_CON0
#define I2C_MM_TX_FIFO_CLR_OFFSET   (1)
#define I2C_MM_RX_FIFO_CLR_OFFSET   (0)

// 0x284 MM_FIFO_PTR
#define I2C_MM_RX_FIFO_RPTR_OFFSET  (0)
#define I2C_MM_RX_FIFO_RPTR_MASK    (BITS(0, 3))
#define I2C_MM_RX_FIFO_WPTR_OFFSET  (4)
#define I2C_MM_RX_FIFO_WPTR_MASK    (BITS(4, 7))
#define I2C_MM_TX_FIFO_RPTR_OFFSET  (8)
#define I2C_MM_TX_FIFO_RPTR_MASK    (BITS(8, 11))
#define I2C_MM_TX_FIFO_WPTR_OFFSET  (12)
#define I2C_MM_TX_FIFO_WPTR_MASK    (BITS(12, 15))

// IOT HAL common
typedef enum _ENUM_HAL_RET_T {
    HAL_RET_SUCCESS = 0,
    HAL_RET_FAIL
} ATTR_PACKED ENUM_HAL_RET_T, *P_ENUM_HAL_RET_T;

// IOT PWM
typedef enum _ENUM_PWM_CLK_T {
    PWM_CLK_32K = 0,
    PWM_CLK_2M,
    PWM_CLK_XTAL,
    PWM_CLK_NUM
} ATTR_PACKED ENUM_PWM_CLK_T, *P_ENUM_PWM_CLK_T;

typedef enum _ENUM_PWM_STATE_T {
    PWM_S0 = 0,
    PWM_S1,
    PWM_STATE_NUM
} ATTR_PACKED ENUM_PWM_STATE_T, *P_ENUM_PWM_STATE_T;

// IOT I2C
typedef enum _ENUM_I2C_CLK_T {
    I2C_CLK_50K = 0,
    I2C_CLK_100K,
    I2C_CLK_200K,
    I2C_CLK_300K,
    I2C_CLK_400K,
    I2C_CLK_NUM
} ATTR_PACKED ENUM_I2C_CLK_T, *P_ENUM_I2C_CLK_T;

typedef enum _ENUM_I2C_TRANS_STATUS_T {
    I2C_TRANS_STATUS_SLAVE_NOT_EXIST = 0,
    I2C_TRANS_STATUS_NACK,
    I2C_TRANS_STATUS_ARBITRATE,
    I2C_TRANS_STATUS_FIFO_FLOW,
    I2C_TRANS_STATUS_FAIL,
    I2C_TRANS_STATUS_OK,
} ATTR_PACKED ENUM_I2C_TRANS_STATUS_T, *P_ENUM_I2C_TRANS_STATUS_T;

// IOT GDMA
// TODO: ONLY for I2C+DMA DVT. Should follow GDMA definition.
/*------------- Half channel GDMA -------------------------------*/
typedef struct _IOT_I2C_HALF_GDMA_TypeDef {
    // n is from 3 to 12 for half channel
    volatile UINT32    DMAn_WPPT;      // DMA + 0n08h
    volatile UINT32    DMAn_WPTO;      // DMA + 0n0Ch
    volatile UINT32    DMAn_COUNT;     // DMA + 0n10h
    volatile UINT32    DMAn_CON;       // DMA + 0n14h
    volatile UINT32    DMAn_START;     // DMA + 0n18h
    volatile UINT32    DMAn_INTSTA;    // DMA + 0n1Ch
    volatile UINT32    DMAn_ACKINT;    // DMA + 0n20h
    volatile UINT32    DMAn_RLCT;      // DMA + 0n24h
    volatile UINT32    DMAn_LIMITER;   // DMA + 0n28h
    volatile UINT32    DMAn_PGMADDR;   // DMA + 0n2Ch
} IOT_I2C_HALF_GDMA_TypeDef, *P_IOT_I2C_HALF_GDMA_TypeDef;

#define I2C_GDMA_IDX_OFFSET             (0x100)
#define I2C_GDMA_DMAN_WPPT_OFFSET       (0x08)

// IOT IrTx
typedef struct _IOT_IRTX_TypeDef {
    volatile UINT32   IRTXCFG;        // 0x00
    volatile UINT32   IRTXD0;         // 0x04
    volatile UINT32   IRTXD1;         // 0x08
    volatile UINT32   IRTXD2;         // 0x0C
    volatile UINT32   IRTX_L0H;       // 0x10
    volatile UINT32   IRTX_L0L;       // 0x14
    volatile UINT32   IRTX_L1H;       // 0x18
    volatile UINT32   IRTX_L1L;       // 0x1C
    volatile UINT32   IRTXSYNCH;      // 0x20
    volatile UINT32   IRTXSYNCL;      // 0x24
    volatile UINT32   IRTXMT;         // 0x28
    volatile UINT32   IRTX_INT_CLR;   // 0x2C
    volatile UINT32   IRTX_SWM_BP;    // 0x30
    volatile UINT32   IRTX_SWM_PW0;   // 0x34
    volatile UINT32   IRTX_SWM_PW1;   // 0x38
    volatile UINT32   IRTX_SWM_PW2;   // 0x3C
    volatile UINT32   IRTX_SWM_PW3;   // 0x40
    volatile UINT32   IRTX_SWM_PW4;   // 0x44
    volatile UINT32   IRTX_SWM_PW5;   // 0x48
    volatile UINT32   IRTX_SWM_PW6;   // 0x4C
    volatile UINT32   IRTX_SWM_PW7;   // 0x50
    volatile UINT32   IRTX_SWM_PW8;   // 0x54
    volatile UINT32   IRTX_SWM_PW9;   // 0x58
    volatile UINT32   IRTX_SWM_PW10;  // 0x5C
    volatile UINT32   IRTX_SWM_PW11;  // 0x60
    volatile UINT32   IRTX_SWM_PW12;  // 0x64
    volatile UINT32   IRTX_SWM_PW13;  // 0x68
    volatile UINT32   IRTX_SWM_PW14;  // 0x6C
    volatile UINT32   IRTX_SWM_PW15;  // 0x70
    volatile UINT32   IRTX_SWM_PW16;  // 0x74
} IOT_IRTX_TypeDef, *P_IOT_IRTX_TypeDef;


// IRTXCFG 0x00
#define IRTX_SWO_OFFSET         (17)
#define IRTX_DATA_INV_OFFSET    (15)
#define IRTX_BITNUM_OFFSET      (8)
#define IRTX_BITNUM_MASK        (BITS(8, 14))
#define IRTX_IRINV_OFFSET       (7)
#define IRTX_IROS_OFFSET        (6)
#define IRTX_RODR_OFFSET        (5)
#define IRTX_BODR_OFFSET        (4)
#define IRTX_MODE_OFFSET        (1)
#define IRTX_MODE_MASK          (BITS(1, 3))
#define IRTX_STRT_OFFSET        (0)


// IRTXMT 0x28
#define IRTX_CDT_OFFSET         (16)

// IRTX_INT_CLR;   // 0x2C
#define IRTX_INT_CLR_OFFSET     (0)

//IRTX_SWM_BP;    // 0x30
#define IRTX_SWM_BP_OFFSET      (0)
#define IRTX_SWM_BP_MASK        BITS(0, 7)

// IOT IRRX
typedef struct _IOT_IRRX_TypeDef {
    volatile UINT32 IRH       ;    // 0x000
    volatile UINT32 IRM       ;    // 0x004
    volatile UINT32 IRL       ;    // 0x008
    volatile UINT32 IRCFGH    ;    // 0x00C
    volatile UINT32 IRCFGL    ;    // 0x010
    volatile UINT32 IRTHD     ;    // 0x014
    volatile UINT32 IRCLR     ;    // 0x018
    volatile UINT32 RESV0     ;    // 0x01C
    volatile UINT32 IR_INTCLR ;    // 0x020
    volatile UINT32 RESV1     ;    // 0x024
    volatile UINT32 RESV2     ;    // 0x028
    volatile UINT32 RESV3     ;    // 0x02C
    volatile UINT32 CHK_DATA0 ;    // 0x030
    volatile UINT32 CHK_DATA1 ;    // 0x034
    volatile UINT32 CHK_DATA2 ;    // 0x038
    volatile UINT32 CHK_DATA3 ;    // 0x03C
    volatile UINT32 CHK_DATA4 ;    // 0x040
    volatile UINT32 CHK_DATA5 ;    // 0x044
    volatile UINT32 CHK_DATA6 ;    // 0x048
    volatile UINT32 CHK_DATA7 ;    // 0x04C
    volatile UINT32 CHK_DATA8 ;    // 0x050
    volatile UINT32 CHK_DATA9 ;    // 0x054
    volatile UINT32 CHK_DATA10;    // 0x058
    volatile UINT32 CHK_DATA11;    // 0x05C
    volatile UINT32 CHK_DATA12;    // 0x060
    volatile UINT32 CHK_DATA13;    // 0x064
    volatile UINT32 CHK_DATA14;    // 0x068
    volatile UINT32 CHK_DATA15;    // 0x06C
    volatile UINT32 CHK_DATA16;    // 0x070
} IOT_IRRX_TypeDef, *P_IOT_IRRX_TypeDef;

// IRH 0x000
#define IRH_BIT_CNT_OFFSET          (0)
#define IRH_BIT_CNT_MASK            BITS(0, 5)

//IRM 0X004
#define IRM_DATA_MASK               (1)


// IRCFGH 0x00C
#define IRCFGH_IREN_OFFSET          (0)
#define IRCFGH_IRINV_OFFSET         (1)
#define IRCFGH_ORDINV_OFFSET        (4)
#define IRCFGH_ORDINV_MASK          (BIT(4))
#define IRCFGH_DISL_OFFSET          (6)
#define IRCFGH_DISH_OFFSET          (7)
#define IRCFGH_CHK_EN_OFFSET        (13)
#define IRCFGH_IGB0_OFFSET          (14)
#define IRCFGH_OK_PERIOD_OFFSET     (16)
#define IRCFGH_OK_PERIOD_MASK       (BITS(16, 23))
#define IRCFGH_IR_FSM_OFFSET        (24)
#define IRCFGH_IR_FSM_MASK          (BITS(24, 26))

// IRCFGL 0x010
#define IRCFGL_SAPERIOD_OFFSET      (0)
#define IRCFGL_SAPERIOD_MASK        BITS(0, 11)
#define IRCFGL_CHK_PERIOD_OFFSET    (16)
#define IRCFGL_CHK_PERIOD_MASK      BITS(16, 31)

// IRTHD 0x014
#define IRTHD_INTCLR_IRCLR_OFFSET   (7)
#define IRTHD_DG_SEL_OFFSET         (8)
#define IRTHD_DG_SEL_MASK           (BITS(8, 12))

// IRCLR 0x018
#define IRCLR_OFFSET                (0)

// IR_INTCLR 0x020
#define IR_INTCLR_OFFSET            (0)


// MTK CRYPTO
typedef struct _IOT_CRYPTO_CTRL_TypeDef {
    volatile UINT32 ENGINE_CTRL;  //0x0004
    volatile UINT32 ENGINE_STA;   //0x0008
    volatile UINT32 TOTAL_LEN;    //0x000C
    volatile UINT32 SOUR_ADR;     //0x0010
    volatile UINT32 RESV0;        //0x0014
    volatile UINT32 RESV1;        //0x0018
    volatile UINT32 RESV2;        //0x001C
    volatile UINT32 DEST_ADR;     //0x0020
} IOT_CRYPTO_CTRL_TypeDef, *P_IOT_CRYPTO_CTRL_TypeDef;

// ENGINE_CTRL;  //0x0004
#define ENGINE_CTRL_KEY_BANK_OFFSET     (8)
#define ENGINE_CTRL_KEY_BANK_MASK       BITS(8, 9)
#define ENGINE_CTRL_START_OFFSET        (4)
#define ENGINE_CTRL_KEY_MODE_OFFSET     (2)
#define ENGINE_CTRL_ES_OFFSET           (0)
#define ENGINE_CTRL_ES_MASK             BITS(0, 1)


#define IOT_CRYPTO_AES_OFFSET   (0x1000)
#define IOT_CRYPTO_AES_DATA1_ADDR       (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0000)
#define IOT_CRYPTO_AES_DATA2_ADDR       (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0010)
#define IOT_CRYPTO_AES_DATA3_ADDR       (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0020)
#define IOT_CRYPTO_AES_DATA4_ADDR       (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0030)
#define IOT_CRYPTO_AES_KEY1_ADDR        (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0040)
#define IOT_CRYPTO_AES_KEY2_ADDR        (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0050)
#define IOT_CRYPTO_AES_KEY3_ADDR        (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0060)
#define IOT_CRYPTO_AES_KEY4_ADDR        (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0070)
#define IOT_CRYPTO_AES_KEY5_ADDR        (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0080)
#define IOT_CRYPTO_AES_KEY6_ADDR        (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0090)
#define IOT_CRYPTO_AES_KEY7_ADDR        (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x00A0)
#define IOT_CRYPTO_AES_KEY8_ADDR        (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x00B0)
#define IOT_CRYPTO_AES_EOD1_ADDR        (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x00C0)
#define IOT_CRYPTO_AES_EOD2_ADDR        (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x00D0)
#define IOT_CRYPTO_AES_EOD3_ADDR        (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x00E0)
#define IOT_CRYPTO_AES_EOD4_ADDR        (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x00F0)
#define IOT_CRYPTO_AES_CRY_DATA1_ADDR   (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0100)
#define IOT_CRYPTO_AES_CRY_DATA2_ADDR   (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0110)
#define IOT_CRYPTO_AES_CRY_DATA3_ADDR   (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0120)
#define IOT_CRYPTO_AES_CRY_DATA4_ADDR   (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0130)
#define IOT_CRYPTO_AES_MODE_ADDR        (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_AES_OFFSET + 0x0200)

#define AES_MODE_XOREOD_OFFSET  (8)
#define AES_MODE_CTR_OFFSET     (7)
#define AES_MODE_UK_OFFSET      (6)
#define AES_MODE_XORDAT_OFFSET  (5)
#define AES_MODE_XORO_OFFSET    (4)
#define AES_MODE_XORI_OFFSET    (3)
#define AES_MODE_ENC_OFFSET     (2)
#define AES_MODE_KEY_LEN_OFFSET (0)
#define AES_MODE_KEY_LEN_MASK   BITS(0, 1)

#define IOT_CRYPTO_DES_OFFSET   (0x2000)
#define IOT_CRYPTO_DES_KEY1_ADDR     (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DES_OFFSET + 0x0040)
#define IOT_CRYPTO_DES_KEY2_ADDR     (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DES_OFFSET + 0x0050)
#define IOT_CRYPTO_DES_KEY3_ADDR     (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DES_OFFSET + 0x0060)
#define IOT_CRYPTO_DES_KEY4_ADDR     (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DES_OFFSET + 0x0070)
#define IOT_CRYPTO_DES_KEY5_ADDR     (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DES_OFFSET + 0x0080)
#define IOT_CRYPTO_DES_KEY6_ADDR     (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DES_OFFSET + 0x0090)
#define IOT_CRYPTO_DES_IV1_ADDR      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DES_OFFSET + 0x00C0)
#define IOT_CRYPTO_DES_IV2_ADDR      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DES_OFFSET + 0x00D0)
#define IOT_CRYPTO_DES_MODE_ADDR     (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DES_OFFSET + 0x0200)
#define DES_MODE_CBC_OFFSET     (3)
#define DES_MODE_ENC_OFFSET     (2)
#define DES_MODE_KEY_LEN_OFFSET (0)
#define DES_MODE_KEY_LEN_MASK   BITS(0, 1)


#define IOT_CRYPTO_SHA256_OFFSET   (0x3000)
#define IOT_CRYPTO_SHA256_IV1      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA256_OFFSET + 0x0000)
#define IOT_CRYPTO_SHA256_IV2      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA256_OFFSET + 0x0010)
#define IOT_CRYPTO_SHA256_IV3      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA256_OFFSET + 0x0020)
#define IOT_CRYPTO_SHA256_IV4      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA256_OFFSET + 0x0030)
#define IOT_CRYPTO_SHA256_IV5      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA256_OFFSET + 0x0040)
#define IOT_CRYPTO_SHA256_IV6      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA256_OFFSET + 0x0050)
#define IOT_CRYPTO_SHA256_IV7      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA256_OFFSET + 0x0060)
#define IOT_CRYPTO_SHA256_IV8      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA256_OFFSET + 0x0070)
#define IOT_CRYPTO_SHA256_MODE_ADDR (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA256_OFFSET + 0x0200)
#define IOT_CRYPTO_SHA256_MODE_MASK 0x00000010
#define IOT_CRYPTO_SHA256_MODE_SHFT 4


#define IOT_CRYPTO_SHA512_OFFSET   (0x4000)
#define IOT_CRYPTO_SHA512_IV1      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA512_OFFSET + 0x0000)
#define IOT_CRYPTO_SHA512_IV2      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA512_OFFSET + 0x0010)
#define IOT_CRYPTO_SHA512_IV3      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA512_OFFSET + 0x0020)
#define IOT_CRYPTO_SHA512_IV4      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA512_OFFSET + 0x0030)
#define IOT_CRYPTO_SHA512_IV5      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA512_OFFSET + 0x0040)
#define IOT_CRYPTO_SHA512_IV6      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA512_OFFSET + 0x0050)
#define IOT_CRYPTO_SHA512_IV7      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA512_OFFSET + 0x0060)
#define IOT_CRYPTO_SHA512_IV8      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA512_OFFSET + 0x0070)
#define IOT_CRYPTO_SHA512_MODE_ADDR (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_SHA512_OFFSET + 0x0200)
#define IOT_CRYPTO_SHA512_MODE_MASK 0x00000010
#define IOT_CRYPTO_SHA512_MODE_SHFT 4

#define IOT_CRYPTO_DMA1_OFFSET   (0x8000)
#define IOT_CRYPTO_DMA1_SRC_ADDR    (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DMA1_OFFSET + 0x0000)
#define IOT_CRYPTO_DMA1_WPPT_ADDR   (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DMA1_OFFSET + 0x0008)
#define IOT_CRYPTO_DMA1_WPTO_ADDR   (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DMA1_OFFSET + 0x000C)
#define IOT_CRYPTO_DMA1_CON_ADDR    (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DMA1_OFFSET + 0x0014)
#define IOT_CRYPTO_DMA1_RLCT_ADDR   (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DMA1_OFFSET + 0x0024)

#define CRYPTO_DMA1_CON_WPSD_OFFSET     (16)
#define CRYPTO_DMA1_CON_WPEN_OFFSET     (17)

#define IOT_CRYPTO_DMA2_OFFSET   (0x9000)
#define IOT_CRYPTO_DMA2_DST_ADDR    (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DMA2_OFFSET + 0x0000)
#define IOT_CRYPTO_DMA2_WPPT_ADDR   (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DMA2_OFFSET + 0x0008)
#define IOT_CRYPTO_DMA2_WPTO_ADDR   (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DMA2_OFFSET + 0x000C)
#define IOT_CRYPTO_DMA2_CON_ADDR    (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DMA2_OFFSET + 0x0014)
#define IOT_CRYPTO_DMA2_RLCT_ADDR   (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_DMA2_OFFSET + 0x0024)

#define CRYPTO_DMA2_CON_WPSD_OFFSET     (16)
#define CRYPTO_DMA2_CON_WPEN_OFFSET     (17)

#define IOT_CRYPTO_TRNG_OFFSET   (0xF000)
#define IOT_CRYPTO_TRNG_CTRL_ENABLE_ADDR  (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_TRNG_OFFSET + 0x0000)
#define IOT_CRYPTO_TRNG_CTRL_ENABLE_MASK  0x00000001
#define IOT_CRYPTO_TRNG_CTRL_ENABLE_SHFT  0
#define IOT_CRYPTO_TRNG_TIME_ADDR         (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_TRNG_OFFSET + 0x0004)
#define IOT_CRYPTO_TRNG_TIME_MASK         0xFFFFFFFF
#define IOT_CRYPTO_TRNG_TIME_SHFT         0
#define IOT_CRYPTO_TRNG_DATA_ADDR         (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_TRNG_OFFSET + 0x0008)
#define IOT_CRYPTO_TRNG_DATA_MASK         0xFFFFFFFF
#define IOT_CRYPTO_TRNG_DATA_SHFT         0
#define IOT_CRYPTO_TRNG_CONF_ADDR         (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_TRNG_OFFSET + 0x000C)
#define IOT_CRYPTO_TRNG_CONF_MASK         0xFFFFFFFF
#define IOT_CRYPTO_TRNG_CONF_SHFT         0
#define IOT_CRYPTO_TRNG_INT_SET_ADDR      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_TRNG_OFFSET + 0x0010)
#define IOT_CRYPTO_TRNG_INT_SET_MASK      0x00000001
#define IOT_CRYPTO_TRNG_INT_SET_SHFT      0
#define IOT_CRYPTO_TRNG_INT_CLR_ADDR      (CM4_MTK_CRYPTO_BASE + IOT_CRYPTO_TRNG_OFFSET + 0x0014)
#define IOT_CRYPTO_TRNG_INT_CLR_MASK      0xFFFFFFFF
#define IOT_CRYPTO_TRNG_INT_CLR_SHFT      0


// IOT RTC
typedef struct _IOT_RTC_CTRL_TypeDef {
    volatile UINT32 RTC_PWRCHK1;        // 0x0004
    volatile UINT32 RTC_PWRCHK2;        // 0x0008
    volatile UINT32 RTC_KEY;            // 0x000C
    volatile UINT32 RTC_PROT1;          // 0x0010
    volatile UINT32 RTC_PROT2;          // 0x0014
    volatile UINT32 RTC_PROT3;          // 0x0018
    volatile UINT32 RTC_PROT4;          // 0x001C
    volatile UINT32 RTC_CTL;            // 0x0020
    volatile UINT32 RTC_LPD_CTL;        // 0x0024
    volatile UINT32 RTC_XOSC_CFG;       // 0x0028
    volatile UINT32 RTC_DEBNCE;         // 0x002C
    volatile UINT32 RTC_PMU_EN;         // 0x0030
    volatile UINT32 RTC_PAD_CTL;        // 0x0034
    volatile UINT32 RTC_RESV0;          // 0x0038
    volatile UINT32 RTC_WAVEOUT;        // 0x003C
    volatile UINT32 RTC_TC_YEA;         // 0x0040
    volatile UINT32 RTC_TC_MON;         // 0x0044
    volatile UINT32 RTC_TC_DOM;         // 0x0048
    volatile UINT32 RTC_TC_DOW;         // 0x004C
    volatile UINT32 RTC_TC_HOU;         // 0x0050
    volatile UINT32 RTC_TC_MIN;         // 0x0054
    volatile UINT32 RTC_TC_SEC;         // 0x0058
    volatile UINT32 RTC_RESV1;          // 0x005C
    volatile UINT32 RTC_AL_YEAR;        // 0x0060
    volatile UINT32 RTC_AL_MON;         // 0x0064
    volatile UINT32 RTC_AL_DOM;         // 0x0068
    volatile UINT32 RTC_AL_DOW;         // 0x006C
    volatile UINT32 RTC_AL_HOUR;        // 0x0070
    volatile UINT32 RTC_AL_MIN;         // 0x0074
    volatile UINT32 RTC_AL_SEC;         // 0x0078
    volatile UINT32 RTC_AL_CTL;         // 0x007C
    volatile UINT32 RTC_RIP_CTL;        // 0x0080
    volatile UINT32 RTC_RIP_CNTH;       // 0x0084
    volatile UINT32 RTC_RIP_CNTL;       // 0x0088
    volatile UINT32 RTC_RESV2;          // 0x008C
    volatile UINT32 RTC_TIMER_CTL;      // 0x0090
    volatile UINT32 RTC_TIMER_CNTH;     // 0x0094
    volatile UINT32 RTC_TIMER_CNTL;     // 0x0098
} IOT_RTC_CTRL_TypeDef, *P_IOT_RTC_CTRL_TypeDef;

typedef struct _IOT_RTC_SPARE_TypeDef {
    volatile UINT32 RTC_SPARE0 ; //0x00C0
    volatile UINT32 RTC_SPARE1 ; //0x00C4
    volatile UINT32 RTC_SPARE2 ; //0x00C8
    volatile UINT32 RTC_SPARE3 ; //0x00CC
    volatile UINT32 RTC_SPARE4 ; //0x00D0
    volatile UINT32 RTC_SPARE5 ; //0x00D4
    volatile UINT32 RTC_SPARE6 ; //0x00D8
    volatile UINT32 RTC_SPARE7 ; //0x00DC
    volatile UINT32 RTC_SPARE8 ; //0x00E0
    volatile UINT32 RTC_SPARE9 ; //0x00E4
    volatile UINT32 RTC_SPARE10; //0x00E8
    volatile UINT32 RTC_SPARE11; //0x00EC
    volatile UINT32 RTC_SPARE12; //0x00F0
    volatile UINT32 RTC_SPARE13; //0x00F4
    volatile UINT32 RTC_SPARE14; //0x00F8
    volatile UINT32 RTC_SPARE15; //0x00FC
} IOT_RTC_SPARE_TypeDef, *P_IOT_RTC_SPARE_TypeDef;
#define RTC_SPARE0_OFFSET   (0xC0)

typedef struct _IOT_RTC_BACKUP_TypeDef {
    volatile UINT32 RTC_BACKUP0 ; //0x0140
    volatile UINT32 RTC_BACKUP1 ; //0x0144
    volatile UINT32 RTC_BACKUP2 ; //0x0148
    volatile UINT32 RTC_BACKUP3 ; //0x014C
    volatile UINT32 RTC_BACKUP4 ; //0x0150
    volatile UINT32 RTC_BACKUP5 ; //0x0154
    volatile UINT32 RTC_BACKUP6 ; //0x0158
    volatile UINT32 RTC_BACKUP7 ; //0x015C
    volatile UINT32 RTC_BACKUP8 ; //0x0160
    volatile UINT32 RTC_BACKUP9 ; //0x0164
    volatile UINT32 RTC_BACKUP10; //0x0168
    volatile UINT32 RTC_BACKUP11; //0x016C
    volatile UINT32 RTC_BACKUP12; //0x0170
    volatile UINT32 RTC_BACKUP13; //0x0174
    volatile UINT32 RTC_BACKUP14; //0x0178
    volatile UINT32 RTC_BACKUP15; //0x017C
    volatile UINT32 RTC_BACKUP16; //0x0180
    volatile UINT32 RTC_BACKUP17; //0x0184
    volatile UINT32 RTC_BACKUP18; //0x0188
    volatile UINT32 RTC_BACKUP19; //0x018C
    volatile UINT32 RTC_BACKUP20; //0x0190
    volatile UINT32 RTC_BACKUP21; //0x0194
    volatile UINT32 RTC_BACKUP22; //0x0198
    volatile UINT32 RTC_BACKUP23; //0x019C
    volatile UINT32 RTC_BACKUP24; //0x01A0
    volatile UINT32 RTC_BACKUP25; //0x01A4
    volatile UINT32 RTC_BACKUP26; //0x01A8
    volatile UINT32 RTC_BACKUP27; //0x01AC
    volatile UINT32 RTC_BACKUP28; //0x01B0
    volatile UINT32 RTC_BACKUP29; //0x01B4
    volatile UINT32 RTC_BACKUP30; //0x01B8
    volatile UINT32 RTC_BACKUP31; //0x01BC
    volatile UINT32 RTC_BACKUP32; //0x01C0
    volatile UINT32 RTC_BACKUP33; //0x01C4
    volatile UINT32 RTC_BACKUP34; //0x01C8
    volatile UINT32 RTC_BACKUP35; //0x01CC
} IOT_RTC_BACKUP_TypeDef, *P_IOT_RTC_BACKUP_TypeDef;
#define RTC_BACKUP0_OFFSET   (0x140)


#define IOT_RTC_COREPDN_ADDR    (CM4_RTC_BASE + 0x0100)
#define RTC_COREPDN_G_ENABLE_OFFSET	        (1)
#define RTC_COREPDN_CORE_SHUTDOWN_OFFSET    (0)

#define IOT_RTC_RTC_PWRCHK1     (0xC6)
#define IOT_RTC_RTC_PWRCHK2     (0x9A)
#define IOT_RTC_RTC_KEY         (0x59)
#define IOT_RTC_RTC_PROT1       (0xA3)
#define IOT_RTC_RTC_PROT2       (0x57)
#define IOT_RTC_RTC_PROT3       (0x67)
#define IOT_RTC_RTC_PROT4       (0xD2)

// RTC_CTL 0x0020
#define RTC_CTL_DEBNCE_OK_OFFSET   (7)
#define RTC_CTL_INHIBIT_OFFSET     (6)
#define RTC_CTL_PROT_PASS_OFFSET   (4)
#define RTC_CTL_KEY_PASS_OFFSET    (3)
#define RTC_CTL_PWR_PASS_OFFSET    (2)
#define RTC_CTL_SIM_RTC_OFFSET     (1)
#define RTC_CTL_RC_STOP_OFFSET     (0)

// RTC_XOSC_CFG 0x0028
#define RTC_XOSC_CFG_OSCPDN_OFFSET      (7)
#define RTC_XOSC_CFG_AMPCTL_EN_OFFSET   (5)
#define RTC_XOSC_CFG_AMP_GSEL_OFFSET    (4)
#define RTC_XOSC_CFG_OSCCALI_MASK       (BITS(0, 3))
#define RTC_XOSC_CFG_OSCCALI_OFFSET     (0)

// RTC_DEBNCE 0x002C
#define RTC_DEBNCE_DEBOUNCE_OFFSET  (0)
#define RTC_DEBNCE_DEBOUNCE_MASK    BITS(0, 2)

// RTC_TIMER_CTL 0x0090
#define RTC_TIMER_CTL_TR_INTEN_OFFSET   (1)
#define RTC_TIMER_CTL_TR_EXTEN_OFFSET   (0)

// RTC_PMU_EN 0x0030
#define RTC_TIMER_STA_OFFSET        (5)
#define RTC_ALARM_STA_OFFSET        (4)
#define RTC_PMU_EN_STATE_OFFSET     (2)
#define RTC_PMU_EN_STATE_MASK       BITS(2, 3)
#define RTC_PMU_EN_EXT_OFFSET       (1)
#define RTC_PMU_EN_OFFSET           (0)


// WDT
typedef struct _IOT_WDT_TypeDef {
    volatile UINT32 WDT_MODE;       //0x0030
    volatile UINT32 WDT_LENGTH;     //0x0034
    volatile UINT32 WDT_RESTART;    //0x0038
    volatile UINT32 WDT_STA;        //0x003C
    volatile UINT32 WDT_INTERVAL;   //0x0040
    volatile UINT32 WDT_SWRST;      //0x0044
} IOT_WDT_TypeDef, *P_IOT_WDT_TypeDef;
#define IOT_WDT_MODE_OFFSET     (0x30)
#define IOT_WDT_DUAL_RST_ADDR   (CM4_WDT_BASE + 0x80)

// WDT_MODE;       //0x0030
#define WDT_MODE_KEY_OFFSET     (8)
#define WDT_MODE_KEY_MASK       BITS(8, 15)
#define WDT_MODE_IRQ_OFFSET     (3)
#define WDT_MODE_EXTEN_OFFSET   (2)
#define WDT_MODE_EXTPOL_OFFSET  (1)
#define WDT_MODE_ENABLE_OFFSET  (0)

// WDT_LENGTH;     //0x0034
#define WDT_LENGTH_TIMEOUT_OFFSET   (5)
#define WDT_LENGTH_TIMEOUT_MASK     BITS(5, 15)
#define WDT_LENGTH_KEY_OFFSET       (0)
#define WDT_LENGTH_KEY_MASK         BITS(0, 4)

// WDT_STA;        //0x003C
#define WDT_STA_WDT_OFFSET              (15)
#define WDT_STA_SW_WDT_OFFSET           (14)
#define WDT_STA_DUAL_CORE_WDT_OFFSET    (13)

// WDT_INTERVAL;   //0x0040
#define WDT_INTERVAL_LENGTH_OFFSET      (0)
#define WDT_INTERVAL_LENGTH_MASK        BITS(0, 11)

// WDT_DUAL_RST 0x80
#define WDT_DUAL_RST_SW_INT_OFFSET      (31)
#define WDT_DUAL_RST_SW_INT_CLR_OFFSET  (30)
#define WDT_DUAL_RST_RST_OFFSET         (0)

// Low Power related CR
#define TOP_AON_CM4_CKGEN0          (CM4_TOPCFGAON_BASE + 0x1B0)
#define CM4_CKGEN0_CM4_WBTAC_MCU_CK_SEL_OFFSET  (14)
#define CM4_CKGEN0_CM4_WBTAC_MCU_CK_SEL_MASK    BITS(14, 15)
#define CM4_CKGEN0_CM4_HCLK_SEL_OFFSET  (0)
#define CM4_CKGEN0_CM4_HCLK_SEL_MASK    BITS(0, 2)

#define TOP_AON_HCLK_2M_CKEGN       (CM4_TOPCFGAON_BASE + 0x1B4)
#define TOP_AON_CM4_PWRCTLCR        (CM4_TOPCFGAON_BASE + 0x1B8)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_OFFSET       (0)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_MASK         BITS(0, 10)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_20M_OFFSET   (4)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_40M_OFFSET   (9)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_26M_OFFSET   (6)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_52M_OFFSET   (10)
#define CM4_PWRCTLCR_CM4_MCU_PWR_STAT_OFFSET    (11)
#define CM4_PWRCTLCR_CM4_MCU_PWR_STAT_MASK      BITS(11, 12)
#define CM4_PWRCTLCR_CM4_XO_NO_OFF_OFFSET       (15)
#define CM4_PWRCTLCR_CM4_FORCE_MCU_STOP_OFFSET  (16)
#define CM4_PWRCTLCR_CM4_MCU_960_EN_OFFSET      (18)
#define CM4_PWRCTLCR_CM4_HCLK_IRQ_B_OFFSET      (19)
#define CM4_PWRCTLCR_CM4_HCLK_IRQ_EN_OFFSET     (20)
#define CM4_PWRCTLCR_CM4_DSLP_IRQ_B_OFFSET      (21)
#define CM4_PWRCTLCR_CM4_DSLP_IRQ_EN_OFFSET     (22)
#define CM4_PWRCTLCR_CM4_DSLP_IRQ_EN_MASK       BITS(22, 23)
#define CM4_PWRCTLCR_CM4_NEED_RESTORE_OFFSET    (24)
#define CM4_PWRCTLCR_CM4_DSLP_MODE_OFFSET       (25)
#define CM4_PWRCTLCR_WF_PLL_RDY_OFFSET          (26)
#define CM4_PWRCTLCR_BT_PLL_RDY_OFFSET          (27)
#define CM4_PWRCTLCR_CM4_MPLL_EN_OFFSET         (28)
#define CM4_PWRCTLCR_CM4_MPLL_EN_MASK           BITS(28, 29)
#define CM4_PWRCTLCR_CM4_MPLL_BT_EN_OFFSET      (29)
#define CM4_PWRCTLCR_CM4_HW_CONTROL_OFFSET      (30)
#define CM4_PWRCTLCR_CM4_HWCTL_PWR_STAT_OFFSET  (31)

#define TOP_AON_CM4_MISC                        (CM4_TOPCFGAON_BASE + 0x1BC)

#define TOP_AON_CM4_STRAP_STA		            (CM4_TOPCFGAON_BASE + 0x1C0)
#define CM4_STRAP_STA_XTAL_FREQ_OFFSET          (13)
#define CM4_STRAP_STA_XTAL_FREQ_MASK            BITS(13, 15)


typedef struct _IOT_CM4_RGU_MEM_CTRL_TypeDef {
    volatile UINT32 CM4_FPGA_RGU_DUMMY_CR;  //0x8300917C
    volatile UINT32 CM4_POS_S_EN;           //0x83009180
    volatile UINT32 CM4_PWR_ACK_ST;         //0x83009184
    volatile UINT32 CM4_MEM_DLY_CTL;        //0x83009188
    volatile UINT32 CM4_RESV0;              //0x8300918C
    volatile UINT32 CM4_MEM0_PDN_EN;        //0x83009190
    volatile UINT32 CM4_MEM1_PDN_EN;        //0x83009194
    volatile UINT32 CM4_MEM0_SLP_EN;        //0x83009198
    volatile UINT32 CM4_MEM1_SLP_EN;        //0x8300919C
    volatile UINT32 CM4_MEM0_PDN;           //0x830091A0
    volatile UINT32 CM4_MEM1_PDN;           //0x830091A4
    volatile UINT32 CM4_MEM0_SLP;           //0x830091A8
    volatile UINT32 CM4_MEM1_SLP;           //0x830091AC
    volatile UINT32 CM4_MEM0_ISO;           //0x830091B0
    volatile UINT32 CM4_MEM1_ISO;           //0x830091B4
    volatile UINT32 CM4_RESV1;              //0x830091B8
    volatile UINT32 CM4_RESV2;              //0x830091BC
    volatile UINT32 CM4_ROM_PD_EN;          //0x830091C0
    volatile UINT32 CM4_ROM_PD;             //0x830091C4
    volatile UINT32 CM4_ROM_RSTB;           //0x830091C8
    volatile UINT32 CM4_RESV3;              //0x830091CC
    volatile UINT32 CM4_MEM0_PDN_STAT;      //0x830091D0
    volatile UINT32 CM4_MEM1_PDN_STAT;      //0x830091D4
    volatile UINT32 CM4_MEM0_SLP_STAT;      //0x830091D8
    volatile UINT32 CM4_MEM1_SLP_STAT;      //0x830091DC
    volatile UINT32 CM4_MEM0_ISO_STAT;      //0x830091E0
    volatile UINT32 CM4_MEM1_ISO_STAT;      //0x830091E0
} IOT_CM4_RGU_MEM_CTRL_TypeDef, *P_IOT_CM4_RGU_MEM_CTRL_TypeDef;
#define IOT_CM4_RGU_MEM_CTRL_OFFSET (0x17C)

/* WIC */
#define CM4_WIC_SW_CLR_ADDR     (CM4_CONFIG_AON_BASE + 0x32C)
#define CM4_WIC_PEND_STA0_ADDR  (CM4_CONFIG_AON_BASE + 0x334)


/* *************************dma hardware definition start line**********************************
*/

#define __I volatile const
#define __O volatile
#define __IO volatile



/*gdma base address definition
*/
#define GDMA1_base  ((uint32_t)0x83010100)

#define GDMA2_base  ((uint32_t)0x83010200)



/*gdma global status base address definition
*/
#define GDMA_GLOAL1_base  ((uint32_t)0x83010000)



/*pdma base address definition
*/

#define PDMA3_base   ((uint32_t)0x83010308)
#define PDMA4_base   ((uint32_t)0x83010408)
#define PDMA5_base   ((uint32_t)0x83010508)
#define PDMA6_base   ((uint32_t)0x83010608)
#define PDMA7_base   ((uint32_t)0x83010708)
#define PDMA8_base   ((uint32_t)0x83010808)
#define PDMA9_base   ((uint32_t)0x83010908)
#define PDMA10_base  ((uint32_t)0x83010A08)
#define PDMA11_base  ((uint32_t)0x83010B08)



/*pdma global status base address definition
*/

#define PDMA_GLOAL1_base  ((uint32_t)0x83010000)




/*virtual fifo dma base address definition
*/
#define VDMA12_base   ((uint32_t)0x83010C10)
#define VDMA13_base   ((uint32_t)0x83010D10)
#define VDMA14_base   ((uint32_t)0x83010E10)
#define VDMA15_base  ((uint32_t)0x83010F10)
#define VDMA16_base  ((uint32_t)0x83011010)
#define VDMA17_base  ((uint32_t)0x83011110)
#define VDMA18_base  ((uint32_t)0x83011210)
#define VDMA19_base  ((uint32_t)0x83011310)
#define VDMA20_base  ((uint32_t)0x83011410)
#define VDMA21_base  ((uint32_t)0x83011510)
#define VDMA22_base  ((uint32_t)0x83011610)
#define VDMA23_base  ((uint32_t)0x83011710)
#define VDMA24_base  ((uint32_t)0x83011810)
#define VDMA25_base  ((uint32_t)0x83011910)




/*virtual dma port address definition
*/
#define VDMA12_port    ((uint32_t)0x79000000)
#define VDMA13_port    ((uint32_t)0x79000100)
#define VDMA14_port    ((uint32_t)0x79000200)
#define VDMA15_port    ((uint32_t)0x79000300)
#define VDMA16_port    ((uint32_t)0x79000400)
#define VDMA17_port    ((uint32_t)0x79000500)
#define VDMA18_port    ((uint32_t)0x79000600)
#define VDMA19_port    ((uint32_t)0x79000700)
#define VDMA20_port    ((uint32_t)0x79000800)
#define VDMA21_port    ((uint32_t)0x79000900)
#define VDMA22_port    ((uint32_t)0x79000A00)
#define VDMA23_port    ((uint32_t)0x79000B00)
#define VDMA24_port    ((uint32_t)0x79000C00)
#define VDMA25_port    ((uint32_t)0x79000D00)



/*pdma global status base address definition
*/

#define VDMA_GLOAL1_base  ((uint32_t)0x83010000)
#define VDMA_GLOAL2_base  ((uint32_t)0x83010004)


/*general dma register definition
*/
typedef struct {
    __IO uint32_t GDMA_SRC;		   /*!<  general dma source address register */
    __IO uint32_t GDMA_DST;		   /*!<  general dma destination address register */
    __IO uint32_t GDMA_WPPT;         /*!<  general dma wrap point address register */
    __IO uint32_t GDMA_WPTO;         /*!<  general dma wrap to address register */
    __IO uint32_t GDMA_COUNT;        /*!<  general dma transfer counter  register */
    __IO uint32_t GDMA_CON;          /*!<  general dma control register */
    __IO uint32_t GDMA_START;        /*!<  general dma start register */
    __IO uint32_t GDMA_INTSTA;      /*!<  general dma interrupt status register*/
    __O  uint32_t GDMA_ACKINT;       /*!<  generall dma interrupt acknowledge register*/
    __I  uint32_t GDMA_RLCT;         /*!<  general dma remaining length of current transfer register*/
    __IO  uint32_t GDMA_LIMITER;      /*!<  general dma bandwidth limiter*/
} GDMA_REGISTER_T;

/*general dma  global  status register definition
*/
typedef struct {
    __IO uint32_t GDMA_GLBSTA;         /*!<  general dma global status register */
} GDMA_REGISTER_GLOBAL_T;



/*peripheral dma register definition
*/
typedef struct {
    __IO uint32_t PDMA_WPPT;         /*!<  peripheral dma wrap point address register */
    __IO uint32_t PDMA_WPTO;         /*!<  peripheral dma wrap to address register */
    __IO uint32_t PDMA_COUNT;        /*!<  peripheral dma transfer counter  register */
    __IO uint32_t PDMA_CON;          /*!<  peripheral dma control register */
    __IO uint32_t PDMA_START;        /*!<  peripheral dma start register */
    __IO uint32_t PDMA_INTSTA;       /*!<  peripheral dma interrupt status register*/
    __O  uint32_t PDMA_ACKINT;       /*!<  peripheral dma interrupt acknowledge register*/
    __I  uint32_t PDMA_RLCT;         /*!<  peripheral dma remaining length of current transfer register*/
    __IO uint32_t PDMA_LIMITER;      /*!< peripheral  dma bandwidth register*/
    __IO uint32_t PDMA_PGMADDR;      /*!<  peripheral dma programmable address register*/
} PDMA_REGISTER_T;

/*peripheral dma  global  status register definition
*/
typedef struct {
    __IO uint32_t PDMA_GLBSTA;         /*!<  peripheral dma wrap point address register */
} PDMA_REGISTER_GLOBAL_T;


/*virtual fifo dma register definition
*/
typedef struct {
    __IO uint32_t VDMA_COUNT;        /*!<  virtual fifo dma transfer counter  register */
    __IO uint32_t VDMA_CON;		   /*!<  peripheral dma control register */
    __IO uint32_t VDMA_START;        /*!<  virtual fifo dma start register */
    __IO uint32_t VDMA_INTSTA;       /*!<  virtual fifol dma interrupt status register*/
    __O  uint32_t VDMA_ACKINT;       /*!<  virtual fifo dma interrupt acknowledge register*/
    __IO uint32_t DUMMY_OFFSET[1];      /*!< virtual fifo dma dummy offser register*/
    __O  uint32_t VDMA_LIMITER;     /*!< virtual fifo dma bandwidth register*/
    __IO uint32_t VDMA_PGMADDR;      /*!<  virtual fifo dma programmable address register*/
    __I  uint32_t VDMA_WRPTR;        /*!<  virtual fifo dma write pointer register */
    __I  uint32_t VDMA_RDPTR;        /*!<  virtual fifo dma read  pointer register */
    __I uint32_t  VDMA_FFCNT;        /*!<  virtual fifo dma fifo count register */
    __I uint32_t  VDMA_FFSTA;        /*!<  virtual fifo dma fifo status  register */
    __IO uint32_t VDMA_ALTLEN;        /*!<  virtual fifo dma fifo alert lentgh register */
    __IO uint32_t VDMA_FFSIZE;        /*!<  virtual fifo dma fifo size  register */
	__IO uint32_t VDMA_CVFF;          /*!<  virtual fifo  dma cascade virtual FIFO control register */
	__IO uint32_t DUMMY_OFFSET2[1];      /*!< virtual fifo dma dummy offser register*/
    __IO uint32_t VDMA_TO;            /*!<  virtual fifo dma timeout value  register */

} VDMA_REGISTER_T;

/*virtual fifo dma  port address definition
*/
typedef struct {
    __IO uint32_t VDMA_PORT;         /*!<  virtual fifo dma port address register */
} VDMA_REGISTER_PORT_T;


/*virtual fifo dma  global  status register definition
*/
typedef struct {
    __IO uint32_t VDMA_GLBSTA;         /*!<  virtual fifo dma global status  register */
} VDMA_REGISTER_GLOBAL_T;


/*general dma base address definition
*/
#define GDMA1                   ((GDMA_REGISTER_T *) (GDMA1_base))

#define GDMA2                   ((GDMA_REGISTER_T *) (GDMA2_base))



/*geripheral dma global status base address definition
*/
#define GDMA_GLOAL1                   ((GDMA_REGISTER_GLOBAL_T *) (GDMA_GLOAL1_base))


/*peripheral dma base address definition
*/
#define PDMA3                   ((PDMA_REGISTER_T *) (PDMA3_base))
#define PDMA4                   ((PDMA_REGISTER_T *) (PDMA4_base))
#define PDMA5                  ((PDMA_REGISTER_T *) (PDMA5_base))
#define PDMA6                  ((PDMA_REGISTER_T *) (PDMA6_base))
#define PDMA7                   ((PDMA_REGISTER_T *) (PDMA7_base))
#define PDMA8                   ((PDMA_REGISTER_T *) (PDMA8_base))
#define PDMA9                  ((PDMA_REGISTER_T *) (PDMA9_base))
#define PDMA10                  ((PDMA_REGISTER_T *) (PDMA10_base))
#define PDMA11                   ((PDMA_REGISTER_T *) (PDMA11_base))

/*peripheral dma global status base address definition
*/
#define PDMA_GLOAL1                   ((PDMA_REGISTER_GLOBAL_T *) (PDMA_GLOAL1_base))



/*virtual fifo dma base address definition
*/
#define VDMA12                  ((VDMA_REGISTER_T *) (VDMA12_base))
#define VDMA13                   ((VDMA_REGISTER_T *) (VDMA13_base))
#define VDMA14                  ((VDMA_REGISTER_T *) (VDMA14_base))
#define VDMA15                   ((VDMA_REGISTER_T *) (VDMA15_base))
#define VDMA16                   ((VDMA_REGISTER_T *) (VDMA16_base))
#define VDMA17                   ((VDMA_REGISTER_T *) (VDMA17_base))
#define VDMA18                   ((VDMA_REGISTER_T *) (VDMA18_base))
#define VDMA19                    ((VDMA_REGISTER_T *) (VDMA19_base))
#define VDMA20                    ((VDMA_REGISTER_T *) (VDMA20_base))
#define VDMA21                    ((VDMA_REGISTER_T *) (VDMA21_base))
#define VDMA22                   ((VDMA_REGISTER_T *) (VDMA22_base))
#define VDMA23                   ((VDMA_REGISTER_T *) (VDMA23_base))
#define VDMA24                   ((VDMA_REGISTER_T *) (VDMA24_base))
#define VDMA25                   ((VDMA_REGISTER_T *) (VDMA25_base))




/*virtual fifo dma  port address definition
*/
#define VDMA12_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA12_port))
#define VDMA13_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA13_port))
#define VDMA14_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA14_port))
#define VDMA15_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA15_port))
#define VDMA16_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA16_port))
#define VDMA17_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA17_port))
#define VDMA18_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA18_port))
#define VDMA19_PORT                    ((VDMA_REGISTER_PORT_T *) (VDMA19_port))
#define VDMA20_PORT                    ((VDMA_REGISTER_PORT_T *) (VDMA20_port))
#define VDMA21_PORT                    ((VDMA_REGISTER_PORT_T *) (VDMA21_port))
#define VDMA22_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA22_port))
#define VDMA23_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA23_port))
#define VDMA24_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA24_port))
#define VDMA25_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA25_port))




/*virtual fifo  dma global status base address definition
*/
#define VDMA_GLOAL1                   ((VDMA_REGISTER_GLOBAL_T *) (VDMA_GLOAL1_base))
#define VDMA_GLOAL2                   ((VDMA_REGISTER_GLOBAL_T *) (VDMA_GLOAL2_base))


/*the bit value in gdma control  register
*/
#define GDMA_CON_SIZE_BYTE_OFFSET           (0)
#define GDMA_CON_SIZE_BYTE_MASK             (0x0<< GDMA_CON_SIZE_BYTE_OFFSET)

#define GDMA_CON_SIZE_HALF_WORD_OFFSET       (0)
#define GDMA_CON_SIZE_HALF_WORD_MASK         (0x1<< GDMA_CON_SIZE_HALF_WORD_OFFSET)

#define GDMA_CON_SIZE_WORD_OFFSET            (1)
#define GDMA_CON_SIZE_WORD_MASK              (0x1<< GDMA_CON_SIZE_WORD_OFFSET)

#define GDMA_CON_SIZE_OFFSET                (0)
#define GDMA_CON_SIZE_MASK                  (0x3<< GDMA_CON_SIZE_OFFSET)


#define GDMA_CON_SINC_OFFSET                  (2)
#define GDMA_CON_SINC_MASK                    (0x1<< GDMA_CON_SINC_OFFSET)

#define GDMA_CON_DINC_OFFSET                  (3)
#define GDMA_CON_DINC_MASK                     (0x1<< GDMA_CON_DINC_OFFSET)

#define GDMA_CON_BURST_4BEAT_OFFSET           (9)
#define GDMA_CON_BURST_4BEAT_MASK             (0x1<< GDMA_CON_BURST_4BEAT_OFFSET)

#define GDMA_CON_TOEN_OFFSET                   (14)
#define GDMA_CON_TOEN_MASK                     (0x1<< GDMA_CON_TOEN_OFFSET)

#define GDMA_CON_ITEN_OFFSET                   (15)
#define GDMA_CON_ITEN_MASK                     (0x1<< GDMA_CON_ITEN_OFFSET)




#define GDMA_CON_WPSD_OFFSET                   (16)
#define GDMA_CON_WPSD_MASK                      (0x1<< GDMA_CON_WPSD_OFFSET)

#define GDMA_CON_WPEN_OFFSET                   (17)
#define GDMA_CON_WPEN_MASK                      (0x1<< GDMA_CON_WPEN_OFFSET)

/*the bit mask definition in gdma control  register
*/
#define GDMA_CON_BURST_OFFSET            (8)
#define GDMA_CON_BURST_MASK              (0x3<< GDMA_CON_BURST_OFFSET)

#define GDMA_RUNNING_STATUS_OFFSET              (0)
#define GDMA_RUNNING_STATUS_MASK                (0x1<< GDMA_RUNNING_STATUS_OFFSET)

#define GDMA_INTERRUPT_STATUS_OFFSET            (1)
#define GDMA_INTERRUPT_STATUS_MASK              (0x1<< GDMA_INTERRUPT_STATUS_OFFSET)


/*the bit value in gdma start  register
*/
#define GDMA_START_BIT_OFFSET            (15)
#define GDMA_START_BIT_MASK              (0x1<< GDMA_START_BIT_OFFSET)

#define GDMA_STOP_BIT_OFFSET            (15)
#define GDMA_STOP_BIT_MASK              (0x0<< GDMA_STOP_BIT_OFFSET)


/*the bit value in gdma interrput status  register
*/
#define GDMA_INTSTA_BIT_OFFSET            (15)
#define GDMA_INTSTA_BIT_MASK              (0x1<< GDMA_INTSTA_BIT_OFFSET)

/*the bit value in gdma acknowlege  register
*/
#define GDMA_ACKINT_BIT_OFFSET            (15)
#define GDMA_ACKINT_BIT_MASK              (0x1<< GDMA_ACKINT_BIT_OFFSET)

#define GDMA_ACKTO_BIT_OFFSET            (15)
#define GDMA_ACKTO_BIT_MASK              (0x1<< GDMA_ACKTO_BIT_OFFSET)


#define GDMA_CHANNEL0_OFFSET     (0)

#define GDMA_CHANNEL1_OFFSET    (2)




/*the bit value in pdma control  register
*/
#define PDMA_CON_SIZE_BYTE_OFFSET           (0)
#define PDMA_CON_SIZE_BYTE_MASK             (0x0<< PDMA_CON_SIZE_BYTE_OFFSET)

#define PDMA_CON_SIZE_HALF_WORD_OFFSET       (0)
#define PDMA_CON_SIZE_HALF_WORD_MASK         (0x1<< PDMA_CON_SIZE_HALF_WORD_OFFSET)

#define PDMA_CON_SIZE_WORD_OFFSET            (1)
#define PDMA_CON_SIZE_WORD_MASK              (0x1<< PDMA_CON_SIZE_WORD_OFFSET)

#define PDMA_CON_SINC_OFFSET                 (2)
#define PDMA_CON_SINC_MASK                   (0x1<< PDMA_CON_SINC_OFFSET)

#define PDMA_CON_DINC_OFFSET                 (3)
#define PDMA_CON_DINC_MASK                   (0x1<< PDMA_CON_DINC_OFFSET)

#define PDMA_CON_DREQ_OFFSET                 (4)
#define PDMA_CON_DREQ_MASK                   (0x1<< PDMA_CON_DREQ_OFFSET) /*peripheral device should enable this bit for handshake*/

#define PDMA_CON_B2W_OFFSET                  (5)
#define PDMA_CON_B2W_MASK                    (0x1<< PDMA_CON_B2W_OFFSET)


#define PDMA_CON_BURST_4BEAT_OFFSET           (9)
#define PDMA_CON_BURST_4BEAT_MASK             (0x1<< PDMA_CON_BURST_4BEAT_OFFSET)


#define PDMA_CON_TOEN_OFFSET                   (14)
#define PDMA_CON_TOEN_MASK                     (0x1<< PDMA_CON_TOEN_OFFSET)

#define PDMA_CON_ITEN_OFFSET                   (15)
#define PDMA_CON_ITEN_MASK                     (0x1<< PDMA_CON_ITEN_OFFSET)


#define PDMA_CON_WPSD_OFFSET                   (16)
#define PDMA_CON_WPSD_MASK                     (0x1<< PDMA_CON_WPSD_OFFSET)

#define PDMA_CON_WPEN_OFFSET                   (17)
#define PDMA_CON_WPEN_MASK                     (0x1<< PDMA_CON_WPEN_OFFSET)

#define PDMA_CON_DIR_OFFSET                    (18)
#define PDMA_CON_DIR_MASK                      (0x1<< PDMA_CON_DIR_OFFSET)



/*the bit value in pdma start  register
*/
#define PDMA_START_BIT_OFFSET            (15)
#define PDMA_START_BIT_MASK              (0x1<< PDMA_START_BIT_OFFSET)

#define PDMA_STOP_BIT_OFFSET            (15)
#define PDMA_STOP_BIT_MASK              (0x0<< PDMA_STOP_BIT_OFFSET)




/*the bit value in pdma acknowlege  register
*/
#define PDMA_ACKINT_BIT_OFFSET            (15)
#define PDMA_ACKINT_BIT_MASK              (0x1<< PDMA_ACKINT_BIT_OFFSET)

#define PDMA_ACKTO_BIT_OFFSET            (16)
#define PDMA_ACKTO_BIT_MASK              (0x1<< PDMA_ACKTO_BIT_OFFSET)


/*the bit mask definition in pdma control  register
*/
#define PDMA_CON_BURST_OFFSET            (8)
#define PDMA_CON_BURST_MASK              (0x3<< PDMA_CON_BURST_OFFSET)

/*pdma channel running offset definition
*/

#define PDMA_CHANNEL3_OFFSET    (4)

#define PDMA_CHANNEL4_OFFSET    (6)

#define PDMA_CHANNEL5_OFFSET    (8)

#define PDMA_CHANNEL6_OFFSET    (10)

#define PDMA_CHANNEL7_OFFSET    (12)

#define PDMA_CHANNEL8_OFFSET    (14)

#define PDMA_CHANNEL9_OFFSET    (16)

#define PDMA_CHANNEL10_OFFSET    (18)

#define PDMA_CHANNEL11_OFFSET    (20)



/*the bit value in virtual fifo dma control  register
*/
#define VDMA_CON_SIZE_BYTE_OFFSET           (0)
#define VDMA_CON_SIZE_BYTE_MASK                  (0x0<< VDMA_CON_SIZE_BYTE_OFFSET)

#define VDMA_CON_SIZE_HALF_WORD_OFFSET      (0)
#define VDMA_CON_SIZE_HALF_WORD_MASK        (0x1<< VDMA_CON_SIZE_HALF_WORD_OFFSET)

#define VDMA_CON_SIZE_WORD_OFFSET           (1)
#define VDMA_CON_SIZE_WORD_MASK             (0x1<< VDMA_CON_SIZE_WORD_OFFSET)


#define VDMA_CON_DREQ_OFFSET                (4)
#define VDMA_CON_DREQ_MASK                  (0x1<< VDMA_CON_DREQ_OFFSET) /*peripheral device should enable this bit for handshake*/

#define VDMA_CON_TOEN_OFFSET                 (14)
#define VDMA_CON_TOEN_MASK                   (0x1<< VDMA_CON_TOEN_OFFSET)

#define VDMA_CON_ITEN_OFFSET                (15)
#define VDMA_CON_ITEN_MASK                  (0x1<< VDMA_CON_ITEN_OFFSET)

#define VDMA_CON_DIR_OFFSET                  (18)
#define VDMA_CON_DIR_MASK                    (0x1<< VDMA_CON_DIR_OFFSET)


/*the bit value in vdma start  register
*/
#define VDMA_START_BIT_OFFSET            (15)
#define VDMA_START_BIT_MASK              (0x1<< VDMA_START_BIT_OFFSET)

#define VDMA_STOP_BIT_OFFSET            (15)
#define VDMA_STOP_BIT_MASK               (0x0<< VDMA_STOP_BIT_OFFSET)


/*the bit value in virtual fifo dma acknowlege  register
*/

#define VDMA_ACKTO_BIT_OFFSET                 (16)
#define VDMA_ACKTO_BIT_MASK                   (0x1<< VDMA_ACKTO_BIT_OFFSET)

#define VDMA_ACKINT_BIT_OFFSET                (15)
#define VDMA_ACKINT_BIT_MASK                   (0x1<< VDMA_ACKINT_BIT_OFFSET)


/*the bit value in virtual fifo dma  fifo count register
*/
#define VDMA_FFSTA_FULL_OFFSET            (0)
#define VDMA_FFSTA_FULL_MASK              (0x1<< VDMA_FFSTA_FULL_OFFSET)

#define VDMA_FFSTA_EMPTY_OFFSET            (1)
#define VDMA_FFSTA_EMPTY_MASK	           (0x1<< VDMA_FFSTA_EMPTY_OFFSET)

#define VDMA_FFSTA_ALERT_OFFSET            (2)
#define VDMA_FFSTA_ALERT_MASK	           (0x1<< VDMA_FFSTA_ALERT_OFFSET)


#define VDMA_CHANNEL12_OFFSET    (22)

#define VDMA_CHANNEL13_OFFSET    (24)

#define VDMA_CHANNEL14_OFFSET    (26)

#define VDMA_CHANNEL15_OFFSET    (28)

#define VDMA_CHANNEL16_OFFSET     (30)

#define VDMA_CHANNEL17_OFFSET    (0)

#define VDMA_CHANNEL18_OFFSET     (2)

#define VDMA_CHANNEL19_OFFSET    (4)

#define VDMA_CHANNEL20_OFFSET    (6)

#define VDMA_CHANNEL21_OFFSET    (8)

#define VDMA_CHANNEL22_OFFSET    (10)

#define VDMA_CHANNEL23_OFFSET    (12)

#define VDMA_CHANNEL24_OFFSET    (14)

#define VDMA_CHANNEL25_OFFSET    (16)

#define DMA_MASTER_OFFSET       (20)

#define PDMA_CON_MASTER_I2C1_TX_MASK  (0x02<< DMA_MASTER_OFFSET)
#define PDMA_CON_MASTER_I2C1_RX_MASK  (0x03<< DMA_MASTER_OFFSET)
#define PDMA_CON_MASTER_I2C2_TX_MASK  (0x04<< DMA_MASTER_OFFSET)
#define PDMA_CON_MASTER_I2C2_RX_MASK  (0x05<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_I2S_TX_MASK   (0x06<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_I2S_RX_MASK   (0x07<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_UART0TX_MASK  (0x08<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_UART0RX_MASK  (0x09<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_UART1TX_MASK  (0x0A<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_UART1RX_MASK  (0x0B<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_BTIF_TX_MASK  (0x0C<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_BTIF_RX_MASK  (0x0D<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_EP2I_TX_MASK  (0x0E<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_EP2O_RX_MASK  (0x0F<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_EP3I_TX_MASK  (0x10<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_EP3O_RX_MASK  (0x11<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_EP4I_TX_MASK  (0x12<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_EP4O_RX_MASK  (0x13<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_ADC_RX_MASK   (0x14<< DMA_MASTER_OFFSET)
#define VDMA_CON_MASTER_HIF_TRX_MASK  (0x15<< DMA_MASTER_OFFSET)
#define PDMA_CON_MASTER_SPIM_TX_MASK  (0x16<< DMA_MASTER_OFFSET)
#define PDMA_CON_MASTER_SPIM_RX_MASK  (0x17<< DMA_MASTER_OFFSET)
#define VDMA_CON_BURST_16BEAT         (0x6<< 8)
#define VDMA_CON_BURST_8BEAT          (0x4<< 8)
#define VDMA_CON_BURST_4BEAT          (0x2<< 8)


/************************ dma end register definition end line  *******************************
 */



#endif

