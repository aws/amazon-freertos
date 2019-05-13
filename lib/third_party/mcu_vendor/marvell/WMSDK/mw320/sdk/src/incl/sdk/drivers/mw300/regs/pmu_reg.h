/** @file pmu_reg.h
*
*  @briefA Automatically generated register structure
*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/

/****************************************************************************//**
 * @file     pmu_reg.h
 * @brief    Automatically generated register structure.
 * @version  V1.0.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 *
 *******************************************************************************/


#ifndef _PMU_REG_H
#define _PMU_REG_H

struct pmu_reg {
    /* 0x000: Power mode control register */
    union {
        struct {
            uint32_t PWR_MODE                :  2;  /* [1:0]   w/o */
            uint32_t RESERVED_31_2           : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PWR_MODE;

    /* 0x004: BOOT_JTAG register */
    union {
        struct {
            uint32_t JTAG_EN                 :  1;  /* [0]     r/w */
            uint32_t BOOT_MODE_REG           :  2;  /* [2:1]   r/o */
            uint32_t FLASH_OPT               :  1;  /* [3]     r/o */
            uint32_t RESERVED_31_4           : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } BOOT_JTAG;

    /* 0x008: Last Reset Cause  Register */
    union {
        struct {
            uint32_t BROWNOUT_VBAT           :  1;  /* [0]     r/o */
            uint32_t BROWNOUT_V12            :  1;  /* [1]     r/o */
            uint32_t BROWNOUT_AV18           :  1;  /* [2]     r/o */
            uint32_t CM3_SYSRESETREQ         :  1;  /* [3]     r/o */
            uint32_t CM3_LOCKUP              :  1;  /* [4]     r/o */
            uint32_t WDT_RST                 :  1;  /* [5]     r/o */
            uint32_t RESERVED_31_6           : 26;  /* [31:6]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } LAST_RST_CAUSE;

    /* 0x00c: Last Reset Cause Clear Register */
    union {
        struct {
            uint32_t BROWNOUT_VBAT_CLR       :  1;  /* [0]     r/w */
            uint32_t BROWNOUT_V12_CLR        :  1;  /* [1]     r/w */
            uint32_t BROWNOUT_AV18_CLR       :  1;  /* [2]     r/w */
            uint32_t CM3_SYSRESETREQ_CLR     :  1;  /* [3]     r/w */
            uint32_t CM3_LOCKUP_CLR          :  1;  /* [4]     r/w */
            uint32_t WDT_RST_CLR             :  1;  /* [5]     r/w */
            uint32_t RESERVED_31_6           : 26;  /* [31:6]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } LAST_RST_CLR;

    /* 0x010: Wake up source clear register */
    union {
        struct {
            uint32_t CLR_PIN0_INT            :  1;  /* [0]     r/w */
            uint32_t CLR_PIN1_INT            :  1;  /* [1]     r/w */
            uint32_t CLR_WL_INT              :  1;  /* [2]     r/w */
            uint32_t CLR_RTC_INT             :  1;  /* [3]     r/w */
            uint32_t CLR_COMP_INT            :  1;  /* [4]     r/w */
            uint32_t RESERVED_31_5           : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } WAKE_SRC_CLR;

    /* 0x014: Power mode status register */
    union {
        struct {
            uint32_t PWR_MODE_STATUS         :  2;  /* [1:0]   r/o */
            uint32_t RESERVED_31_2           : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PWR_MODE_STATUS;

    /* 0x018: Clock source selection register */
    union {
        struct {
            uint32_t SYS_CLK_SEL             :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_31_2           : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK_SRC;

    /* 0x01c: Wakeup status register */
    union {
        struct {
            uint32_t PIN0_WAKEUP_STATUS      :  1;  /* [0]     r/o */
            uint32_t PIN1_WAKEUP_STATUS      :  1;  /* [1]     r/o */
            uint32_t WLINT_WAKEUP_STATUS     :  1;  /* [2]     r/o */
            uint32_t RTC_WAKEUP_STATUS       :  1;  /* [3]     r/o */
            uint32_t PMIP_COMP_WAKEUP_STATUS :  1;  /* [4]     r/o */
            uint32_t RESERVED_31_5           : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } WAKEUP_STATUS;

    /* 0x020: PMIP Brown Interupt Select */
    union {
        struct {
            uint32_t PMIP_BRN_INT_SEL        :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1           : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_BRN_INT_SEL;

    uint8_t zReserved0x024[4];  /* pad 0x024 - 0x027 */

    /* 0x028: Clock ready  register */
    union {
        struct {
            uint32_t PLL_CLK_RDY             :  1;  /* [0]     r/o */
            uint32_t RESERVED_1              :  1;  /* [1]     rsvd */
            uint32_t RC32M_RDY               :  1;  /* [2]     r/o */
            uint32_t X32K_RDY                :  1;  /* [3]     r/o */
            uint32_t PLL_AUDIO_RDY           :  1;  /* [4]     r/o */
            uint32_t RESERVED_5              :  1;  /* [5]     rsvd */
            uint32_t XTAL32M_CLK_RDY         :  1;  /* [6]     r/o */
            uint32_t RESERVED_31_7           : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK_RDY;

    /* 0x02c: RC 32M control */
    union {
        struct {
            uint32_t CAL_IN_PROGRESS         :  1;  /* [0]     r/o */
            uint32_t CAL_ALLOW               :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2           : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RC32M_CTRL;

    uint8_t zReserved0x030[4];  /* pad 0x030 - 0x033 */

    /* 0x034: SFLL control register 1 */
    union {
        struct {
            uint32_t SFLL_REFDIV             :  9;  /* [8:0]   r/w */
            uint32_t SFLL_TEST_ANA           :  3;  /* [11:9]  r/w */
            uint32_t RESERVED_18_12          :  7;  /* [18:12] rsvd */
            uint32_t SFLL_DIV_SEL            :  2;  /* [20:19] r/w */
            uint32_t RESERVED_22_21          :  2;  /* [22:21] rsvd */
            uint32_t SFLL_RESERVE_IN         :  8;  /* [30:23] r/w */
            uint32_t REG_PLL_PU_INT          :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } SFLL_CTRL1;

    /* 0x038: Xtal32M clock request register */
    union {
        struct {
            uint32_t PU_OSC                  :  1;  /* [0]     r/w */
            uint32_t PU_XTAL                 :  1;  /* [1]     r/w */
            uint32_t PU                      :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3           : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ANA_GRP_CTRL0;

    /* 0x03c: SFLL control register 2 */
    union {
        struct {
            uint32_t SFLL_PU                 :  1;  /* [0]     r/w */
            uint32_t RESERVED_6_1            :  6;  /* [6:1]   rsvd */
            uint32_t SFLL_FBDIV              :  9;  /* [15:7]  r/w */
            uint32_t RESERVED_19_16          :  4;  /* [19:16] rsvd */
            uint32_t SFLL_KVCO               :  2;  /* [21:20] r/w */
            uint32_t RESERVED_24_22          :  3;  /* [24:22] rsvd */
            uint32_t SFLL_REFCLK_SEL         :  1;  /* [25]    r/w */
            uint32_t SFLL_LOCK               :  1;  /* [26]    r/o */
            uint32_t RESERVED_31_27          :  5;  /* [31:27] rsvd */
        } BF;
        uint32_t WORDVAL;
    } SFLL_CTRL0;

    /* 0x040: Power config register */
    union {
        struct {
            uint32_t RESERVED_3_0            :  4;  /* [3:0]   rsvd */
            uint32_t PM3_RET_MEM_CFG         :  7;  /* [10:4]  r/w */
            uint32_t RESERVED_31_11          : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PWR_CFG;

    /* 0x044: Power status register */
    union {
        struct {
            uint32_t RESERVED_0              :  1;  /* [0]     rsvd */
            uint32_t V12_LDO_RDY             :  1;  /* [1]     r/o */
            uint32_t RESERVED_6_2            :  5;  /* [6:2]   rsvd */
            uint32_t AV18_RDY                :  1;  /* [7]     r/o */
            uint32_t RESERVED_31_8           : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PWR_STAT;

    /* 0x048: WF OPT power-saving register 0 */
    union {
        struct {
            uint32_t RESERVED_0              :  1;  /* [0]     rsvd */
            uint32_t MAX_FREQ_CTRL           :  1;  /* [1]     r/w */
            uint32_t MEM_CTRL                :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3           : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } WF_OPT0;

    /* 0x04c: WF OPT power-saving register 0 */
    union {
        struct {
            uint32_t RESERVED_1_0            :  2;  /* [1:0]   rsvd */
            uint32_t SPARE                   :  2;  /* [3:2]   r/w */
            uint32_t RESERVED_31_4           : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } WF_OPT1;

    uint8_t zReserved0x050[4];  /* pad 0x050 - 0x053 */

    /* 0x054: Brownout config register */
    union {
        struct {
            uint32_t BRNDET_V12_RST_EN       :  1;  /* [0]     r/w */
            uint32_t BRNDET_VBAT_RST_EN      :  1;  /* [1]     r/w */
            uint32_t RESERVED_2              :  1;  /* [2]     rsvd */
            uint32_t BRNDET_AV18_RST_EN      :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4           : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_BRN_CFG;

    /* 0x058: aupll lock status */
    union {
        struct {
            uint32_t RESERVED_OUT_0          :  2;  /* [1:0]   r/w */
            uint32_t AUPLL_LOCK              :  1;  /* [2]     r/o */
            uint32_t RESERVED_OUT_1          : 29;  /* [31:3]  r/w */
        } BF;
        uint32_t WORDVAL;
    } AUPLL_LOCK;

    /* 0x05c: BG control register */
    union {
        struct {
            uint32_t BG_CTRL                 :  3;  /* [2:0]   r/w */
            uint32_t GAINX2                  :  1;  /* [3]     r/w */
            uint32_t R_ORIEN_SEL             :  1;  /* [4]     r/w */
            uint32_t BG_SEL                  :  2;  /* [6:5]   r/w */
            uint32_t TEST                    :  3;  /* [9:7]   r/w */
            uint32_t BYPASS                  :  1;  /* [10]    r/w */
            uint32_t RESERVED_31_11          : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ANA_GRP_CTRL1;

    /* 0x060: Power Configure register */
    union {
        struct {
            uint32_t STATUS_DEL_SEL          :  2;  /* [1:0]   r/w */
            uint32_t AV18_EXT                :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3           : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_PWR_CONFIG;

    uint8_t zReserved0x064[8];  /* pad 0x064 - 0x06b */

    /* 0x06c: PMIP test register */
    union {
        struct {
            uint32_t PMIP_TEST               :  4;  /* [3:0]   r/w */
            uint32_t PMIP_TEST_EN            :  1;  /* [4]     r/w */
            uint32_t PMU_PMIP_TEST           :  4;  /* [8:5]   r/w */
            uint32_t PMU_PMIP_TEST_EN        :  1;  /* [9]     r/w */
            uint32_t RESERVED_31_10          : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_TEST;

    uint8_t zReserved0x070[8];  /* pad 0x070 - 0x077 */

    /* 0x078: Audio pll control register */
    union {
        struct {
            uint32_t FRACT                   : 20;  /* [19:0]  r/w */
            uint32_t PU                      :  1;  /* [20]    r/w */
            uint32_t RESERVED_31_21          : 11;  /* [31:21] rsvd */
        } BF;
        uint32_t WORDVAL;
    } AUPLL_CTRL0;

    /* 0x07c: Peripheral clock enable register */
    union {
        struct {
            uint32_t RESERVED_0              :  1;  /* [0]     r/w */
            uint32_t QSPI0_CLK_EN            :  1;  /* [1]     r/w */
            uint32_t RESERVED_2              :  1;  /* [2]     r/w */
            uint32_t PDM_CLK_EN              :  1;  /* [3]     r/w */
            uint32_t GPIO_CLK_EN             :  1;  /* [4]     r/w */
            uint32_t UART0_CLK_EN            :  1;  /* [5]     r/w */
            uint32_t UART1_CLK_EN            :  1;  /* [6]     r/w */
            uint32_t I2C0_CLK_EN             :  1;  /* [7]     r/w */
            uint32_t SSP0_CLK_EN             :  1;  /* [8]     r/w */
            uint32_t SSP1_CLK_EN             :  1;  /* [9]     r/w */
            uint32_t GPT0_CLK_EN             :  1;  /* [10]    r/w */
            uint32_t GPT1_CLK_EN             :  1;  /* [11]    r/w */
            uint32_t RESERVED_14_12          :  3;  /* [14:12] rsvd */
            uint32_t UART2_CLK_EN            :  1;  /* [15]    r/w */
            uint32_t RESERVED_16             :  1;  /* [16]    r/w */
            uint32_t SSP2_CLK_EN             :  1;  /* [17]    r/w */
            uint32_t RESERVED_18             :  1;  /* [18]    rsvd */
            uint32_t I2C1_CLK_EN             :  1;  /* [19]    r/w */
            uint32_t I2C2_CLK_EN             :  1;  /* [20]    r/w */
            uint32_t GPT2_CLK_EN             :  1;  /* [21]    r/w */
            uint32_t GPT3_CLK_EN             :  1;  /* [22]    r/w */
            uint32_t WDT_CLK_EN              :  1;  /* [23]    r/w */
            uint32_t RESERVED_24             :  1;  /* [24]    r/w */
            uint32_t SDIO_CLK_EN             :  1;  /* [25]    r/w */
            uint32_t ADC_CLK_EN              :  1;  /* [26]    r/w */
            uint32_t USBC_CLK_EN             :  1;  /* [27]    r/w */
            uint32_t RESERVED_29_28          :  2;  /* [29:28] rsvd */
            uint32_t USBC_AHB_CLK_EN         :  1;  /* [30]    r/w */
            uint32_t SDIO_AHB_CLK_EN         :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } PERI_CLK_EN;

    /* 0x080: UART fast clock div register */
    union {
        struct {
            uint32_t DENOMINATOR             : 11;  /* [10:0]  r/w */
            uint32_t NOMINATOR               : 13;  /* [23:11] r/w */
            uint32_t RESERVED_31_24          :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } UART_FAST_CLK_DIV;

    /* 0x084: UART slow clock div register */
    union {
        struct {
            uint32_t DENOMINATOR             : 11;  /* [10:0]  r/w */
            uint32_t NOMINATOR               : 13;  /* [23:11] r/w */
            uint32_t RESERVED_31_24          :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } UART_SLOW_CLK_DIV;

    /* 0x088: UART clock select register */
    union {
        struct {
            uint32_t UART0_CLK_SEL           :  1;  /* [0]     r/w */
            uint32_t UART1_CLK_SEL           :  1;  /* [1]     r/w */
            uint32_t UART2_CLK_SEL           :  1;  /* [2]     r/w */
            uint32_t RESERVED_3              :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4           : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } UART_CLK_SEL;

    /* 0x08c: MCU CORE clock divider ratio register */
    union {
        struct {
            uint32_t FCLK_DIV                :  6;  /* [5:0]   r/w */
            uint32_t RESERVED_31_6           : 26;  /* [31:6]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } MCU_CORE_CLK_DIV;

    /* 0x090: Peripheral0 clock divider ratio register */
    union {
        struct {
            uint32_t SSP0_CLK_DIV            :  5;  /* [4:0]   r/w */
            uint32_t SSP1_CLK_DIV            :  5;  /* [9:5]   r/w */
            uint32_t SSP2_CLK_DIV            :  5;  /* [14:10] r/w */
            uint32_t RESERVED_15             :  1;  /* [15]    rsvd */
            uint32_t SDIO_CLK_DIV            :  4;  /* [19:16] r/w */
            uint32_t RESERVED_25_20          :  6;  /* [25:20] r/w */
            uint32_t PDM_CLK_DIV             :  5;  /* [30:26] r/w */
            uint32_t RESERVED_31             :  1;  /* [31]    rsvd */
        } BF;
        uint32_t WORDVAL;
    } PERI0_CLK_DIV;

    /* 0x094: Peripheral1 clock divider ratio register */
    union {
        struct {
            uint32_t RESERVED_3_0            :  4;  /* [3:0]   rsvd */
            uint32_t FLASH_CLK_DIV           :  3;  /* [6:4]   r/w */
            uint32_t RESERVED_7              :  1;  /* [7]     rsvd */
            uint32_t QSPI0_CLK_DIV           :  3;  /* [10:8]  r/w */
            uint32_t RESERVED_11             :  1;  /* [11]    rsvd */
            uint32_t RESERVED_14_12          :  3;  /* [14:12] r/w */
            uint32_t RESERVED_19_15          :  5;  /* [19:15] rsvd */
            uint32_t RESERVED_24_20          :  5;  /* [24:20] r/w */
            uint32_t RESERVED_31_25          :  7;  /* [31:25] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PERI1_CLK_DIV;

    /* 0x098: Peripheral2 Clock Divider Ratio Register */
    union {
        struct {
            uint32_t GPT_SAMPLE_CLK_DIV      :  3;  /* [2:0]   r/w */
            uint32_t RESERVED_3              :  1;  /* [3]     rsvd */
            uint32_t WDT_CLK_DIV_5_3         :  3;  /* [6:4]   r/w */
            uint32_t RESERVED_7              :  1;  /* [7]     rsvd */
            uint32_t GPT3_CLK_DIV_2_0        :  3;  /* [10:8]  r/w */
            uint32_t RESERVED_11             :  1;  /* [11]    rsvd */
            uint32_t GPT3_CLK_DIV_5_3        :  3;  /* [14:12] r/w */
            uint32_t RESERVED_19_15          :  5;  /* [19:15] rsvd */
            uint32_t I2C_CLK_DIV             :  2;  /* [21:20] r/w */
            uint32_t RESERVED_23_22          :  2;  /* [23:22] rsvd */
            uint32_t WDT_CLK_DIV_1_0         :  2;  /* [25:24] r/w */
            uint32_t RESERVED_27_26          :  2;  /* [27:26] rsvd */
            uint32_t WDT_CLK_DIV_2_2         :  1;  /* [28]    r/w */
            uint32_t RESERVED_31_29          :  3;  /* [31:29] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PERI2_CLK_DIV;

    /* 0x09c: select signal for cau mclk */
    union {
        struct {
            uint32_t CAU_CLK_SEL             :  2;  /* [1:0]   r/w */
            uint32_t CAU_SW_GATE             :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3           : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CAU_CLK_SEL;

    /* 0x0a0: low power control registers in PM3/PM4 mode */
    union {
        struct {
            uint32_t RESERVED_0              :  1;  /* [0]     rsvd */
            uint32_t CACHE_LINE_FLUSH        :  1;  /* [1]     r/w */
            uint32_t RESERVED_2              :  1;  /* [2]     rsvd */
            uint32_t SLP_CTRL                :  1;  /* [3]     r/w */
            uint32_t RC_OSC_SEL              :  1;  /* [4]     r/w */
            uint32_t RESERVED_31_5           : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } LOW_PWR_CTRL;

    /* 0x0a4: Power config resigter */
    union {
        struct {
            uint32_t GPIO0_PDB               :  1;  /* [0]     r/w */
            uint32_t GPIO1_PDB               :  1;  /* [1]     r/w */
            uint32_t GPIO2_PDB               :  1;  /* [2]     r/w */
            uint32_t GPIO3_PDB               :  1;  /* [3]     r/w */
            uint32_t GPIO0_V25               :  1;  /* [4]     r/w */
            uint32_t GPIO1_V25               :  1;  /* [5]     r/w */
            uint32_t GPIO2_V25               :  1;  /* [6]     r/w */
            uint32_t GPIO3_V25               :  1;  /* [7]     r/w */
            uint32_t GPIO0_V18               :  1;  /* [8]     r/w */
            uint32_t GPIO1_V18               :  1;  /* [9]     r/w */
            uint32_t GPIO2_V18               :  1;  /* [10]    r/w */
            uint32_t GPIO3_V18               :  1;  /* [11]    r/w */
            uint32_t GPIO0_LOW_VDDB          :  1;  /* [12]    r/w */
            uint32_t GPIO1_LOW_VDDB          :  1;  /* [13]    r/w */
            uint32_t GPIO2_LOW_VDDB          :  1;  /* [14]    r/w */
            uint32_t GPIO3_LOW_VDDB          :  1;  /* [15]    r/w */
            uint32_t GPIO_AON_LOW_VDDB       :  1;  /* [16]    r/w */
            uint32_t GPIO_AON_V25            :  1;  /* [17]    r/w */
            uint32_t GPIO_AON_V18            :  1;  /* [18]    r/w */
            uint32_t GPIO_AON_PDB            :  1;  /* [19]    r/w */
            uint32_t RESERVED_31_20          : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } IO_PAD_PWR_CFG;

    /* 0x0a8: extra interrupt select register0 */
    union {
        struct {
            uint32_t SEL_34                  :  1;  /* [0]     r/w */
            uint32_t SEL_35                  :  1;  /* [1]     r/w */
            uint32_t SEL_36                  :  1;  /* [2]     r/w */
            uint32_t SEL_37                  :  1;  /* [3]     r/w */
            uint32_t SEL_38                  :  1;  /* [4]     r/w */
            uint32_t SEL_39                  :  1;  /* [5]     r/w */
            uint32_t SEL_40                  :  1;  /* [6]     r/w */
            uint32_t SEL_41                  :  1;  /* [7]     r/w */
            uint32_t SEL_42                  :  1;  /* [8]     r/w */
            uint32_t SEL_43                  :  1;  /* [9]     r/w */
            uint32_t SEL_44                  :  1;  /* [10]    r/w */
            uint32_t SEL_45                  :  1;  /* [11]    r/w */
            uint32_t SEL_46                  :  1;  /* [12]    r/w */
            uint32_t SEL_47                  :  1;  /* [13]    r/w */
            uint32_t SEL_48                  :  1;  /* [14]    r/w */
            uint32_t SEL_49                  :  1;  /* [15]    r/w */
            uint32_t SEL_50                  :  1;  /* [16]    r/w */
            uint32_t SEL_51                  :  1;  /* [17]    r/w */
            uint32_t SEL_52                  :  1;  /* [18]    r/w */
            uint32_t SEL_53                  :  1;  /* [19]    r/w */
            uint32_t SEL_54                  :  1;  /* [20]    r/w */
            uint32_t SEL_55                  :  1;  /* [21]    r/w */
            uint32_t SEL_56                  :  1;  /* [22]    r/w */
            uint32_t SEL_57                  :  1;  /* [23]    r/w */
            uint32_t SEL_58                  :  1;  /* [24]    r/w */
            uint32_t RESERVED_31_25          :  7;  /* [31:25] rsvd */
        } BF;
        uint32_t WORDVAL;
    } EXT_SEL_REG0;

    uint8_t zReserved0x0ac[4];  /* pad 0x0ac - 0x0af */

    /* 0x0b0: USB and audio pll control register */
    union {
        struct {
            uint32_t PD_OVPROT               :  1;  /* [0]     r/w */
            uint32_t REFCLK_SEL              :  1;  /* [1]     r/w */
            uint32_t ICP                     :  2;  /* [3:2]   r/w */
            uint32_t ENA_DITHER              :  1;  /* [4]     r/w */
            uint32_t DIV_OCLK_PATTERN        :  2;  /* [6:5]   r/w */
            uint32_t DIV_OCLK_MODULO         :  3;  /* [9:7]   r/w */
            uint32_t DIV_MCLK                :  4;  /* [13:10] r/w */
            uint32_t DIV_FBCCLK              :  6;  /* [19:14] r/w */
            uint32_t ANA_TSTPNT              :  2;  /* [21:20] r/w */
            uint32_t DIG_TSTPNT              :  2;  /* [23:22] r/w */
            uint32_t RESERVE                 :  5;  /* [28:24] r/w */
            uint32_t EN_VCOX2                :  1;  /* [29]    r/w */
            uint32_t RESERVED_31_30          :  2;  /* [31:30] rsvd */
        } BF;
        uint32_t WORDVAL;
    } AUPLL_CTRL1;

    /* 0x0b4: */
    union {
        struct {
            uint32_t CAU_ACOMP_MCLK_EN       :  1;  /* [0]     r/w */
            uint32_t CAU_GPDAC_MCLK_EN       :  1;  /* [1]     r/w */
            uint32_t RESERVED_2              :  1;  /* [2]     r/w */
            uint32_t CAU_GPADC0_MCLK_EN      :  1;  /* [3]     r/w */
            uint32_t CAU_BG_MCLK_EN          :  1;  /* [4]     r/w */
            uint32_t RESERVED_31_5           : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CAU_CTRL;

    /* 0x0b8: */
    union {
        struct {
            uint32_t RC32K_CODE_FR_EXT       : 14;  /* [13:0]  r/w */
            uint32_t RC32K_CAL_EN            :  1;  /* [14]    r/w */
            uint32_t RC32K_PD                :  1;  /* [15]    r/w */
            uint32_t RESERVED_31_16          : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RC32K_CTRL0;

    /* 0x0bc: */
    union {
        struct {
            uint32_t RC32K_CAL_DONE          :  1;  /* [0]     r/o */
            uint32_t RC32K_RDY               :  1;  /* [1]     r/o */
            uint32_t RC32K                   :  1;  /* [2]     r/o */
            uint32_t RC32K_CAL_INPROGRESS    :  1;  /* [3]     r/o */
            uint32_t RC32K_CODE_FR_CAL       : 14;  /* [17:4]  r/o */
            uint32_t RC32K_ALLOW_CAL         :  1;  /* [18]    r/w */
            uint32_t RC32K_CAL_DIV           :  3;  /* [21:19] r/w */
            uint32_t RC32K_REFCLK32K         :  1;  /* [22]    r/w */
            uint32_t RC32K_EXT_CODE_EN       :  1;  /* [23]    r/w */
            uint32_t RESERVED_31_24          :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RC32K_CTRL1;

    /* 0x0c0: */
    union {
        struct {
            uint32_t X32K_RDY                :  1;  /* [0]     r/o */
            uint32_t XCLK32K                 :  1;  /* [1]     r/o */
            uint32_t X32K_STUP_ASSIST        :  2;  /* [3:2]   r/w */
            uint32_t X32K_TEST_EN            :  1;  /* [4]     r/w */
            uint32_t X32K_TMODE              :  2;  /* [6:5]   r/w */
            uint32_t RESERVED_8_7            :  2;  /* [8:7]   rsvd */
            uint32_t X32K_VDDXO_CNTL         :  2;  /* [10:9]  r/w */
            uint32_t X32K_EXT_OSC_EN         :  1;  /* [11]    r/w */
            uint32_t X32K_EN                 :  1;  /* [12]    r/w */
            uint32_t X32K_DLY_SEL            :  2;  /* [14:13] r/w */
            uint32_t RESERVED_31_15          : 17;  /* [31:15] rsvd */
        } BF;
        uint32_t WORDVAL;
    } XTAL32K_CTRL;

    /* 0x0c4: */
    union {
        struct {
            uint32_t COMP_OUT                :  1;  /* [0]     r/o */
            uint32_t COMP_RDY                :  1;  /* [1]     r/o */
            uint32_t COMP_REF_SEL            :  3;  /* [4:2]   r/w */
            uint32_t COMP_DIFF_EN            :  1;  /* [5]     r/w */
            uint32_t COMP_EN                 :  1;  /* [6]     r/w */
            uint32_t COMP_HYST               :  2;  /* [8:7]   r/w */
            uint32_t CAU_REF_EN              :  1;  /* [9]     r/w */
            uint32_t RESERVED_31_10          : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_CMP_CTRL;

    /* 0x0c8: */
    union {
        struct {
            uint32_t DEL_AV18_SEL            :  2;  /* [1:0]   r/w */
            uint32_t LDO_AON_AV18_HYST       :  2;  /* [3:2]   r/w */
            uint32_t LDO_AON_AV18_SEL        :  3;  /* [6:4]   r/w */
            uint32_t LDO_AON_AV18_EN         :  1;  /* [7]     r/w */
            uint32_t BRNDET_AV18_OUT         :  1;  /* [8]     r/o */
            uint32_t BRNDET_AV18_RDY         :  1;  /* [9]     r/o */
            uint32_t BRNDET_AV18_FILT        :  2;  /* [11:10] r/w */
            uint32_t BRNHYST_AV18_CNTL       :  2;  /* [13:12] r/w */
            uint32_t BRNTRIG_AV18_CNTL       :  3;  /* [16:14] r/w */
            uint32_t BRNDET_AV18_EN          :  1;  /* [17]    r/w */
            uint32_t DEL_AV18_HYST           :  2;  /* [19:18] r/w */
            uint32_t RESERVED_31_20          : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_BRNDET_AV18;

    uint8_t zReserved0x0cc[4];  /* pad 0x0cc - 0x0cf */

    /* 0x0d0: */
    union {
        struct {
            uint32_t MON_VBAT_OUT            :  1;  /* [0]     r/o */
            uint32_t MON_VBAT_RDY            :  1;  /* [1]     r/o */
            uint32_t MON_VBAT_FILT           :  2;  /* [3:2]   r/w */
            uint32_t MONHYST_VBAT_CNTL       :  2;  /* [5:4]   r/w */
            uint32_t MONTRIG_VBAT_CNTL       :  3;  /* [8:6]   r/w */
            uint32_t MON_VBAT_EN             :  1;  /* [9]     r/w */
            uint32_t BRNDET_VBAT_OUT         :  1;  /* [10]    r/o */
            uint32_t BRNDET_VBAT_RDY         :  1;  /* [11]    r/o */
            uint32_t BRNDET_VBAT_FILT        :  2;  /* [13:12] r/w */
            uint32_t BRNHYST_VBAT_CNTL       :  2;  /* [15:14] r/w */
            uint32_t BRNTRIG_VBAT_CNTL       :  3;  /* [18:16] r/w */
            uint32_t BRNDET_VBAT_EN          :  1;  /* [19]    r/w */
            uint32_t RESERVED_31_20          : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_BRNDET_VBAT;

    /* 0x0d4: */
    union {
        struct {
            uint32_t LDO_AON_V12_HYST        :  2;  /* [1:0]   r/w */
            uint32_t LDO_AON_V12_SEL         :  3;  /* [4:2]   r/w */
            uint32_t BRNDET_V12_OUT          :  1;  /* [5]     r/o */
            uint32_t BRNDET_V12_RDY          :  1;  /* [6]     r/o */
            uint32_t BRNDET_V12_FILT         :  2;  /* [8:7]   r/w */
            uint32_t BRNHYST_V12_CNTL        :  2;  /* [10:9]  r/w */
            uint32_t BRNTRIG_V12_CNTL        :  3;  /* [13:11] r/w */
            uint32_t BRNDET_V12_EN           :  1;  /* [14]    r/w */
            uint32_t RESERVED_31_15          : 17;  /* [31:15] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_BRNDET_V12;

    /* 0x0d8: */
    union {
        struct {
            uint32_t LDO_V12_REF_SEL         :  1;  /* [0]     r/w */
            uint32_t LDO_V12_SOFTST_EN       :  1;  /* [1]     r/w */
            uint32_t LDO_V12_VOUT_SEL        :  3;  /* [4:2]   r/w */
            uint32_t LDO_V12_EN              :  1;  /* [5]     r/w */
            uint32_t LDO_AV18_PWRSW_EN       :  1;  /* [6]     r/w */
            uint32_t LDO_AV18_SOFTST_EN      :  1;  /* [7]     r/w */
            uint32_t LDO_AV18_VOUT_SEL       :  3;  /* [10:8]  r/w */
            uint32_t LDO_AV18_EN             :  1;  /* [11]    r/w */
            uint32_t LDO_VFL_PWRSW_EN        :  1;  /* [12]    r/w */
            uint32_t RESERVED_31_13          : 19;  /* [31:13] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_LDO_CTRL;

    /* 0x0dc: */
    union {
        struct {
            uint32_t SSP0_AUDIO_SEL          :  1;  /* [0]     r/w */
            uint32_t SSP1_AUDIO_SEL          :  1;  /* [1]     r/w */
            uint32_t SSP2_AUDIO_SEL          :  1;  /* [2]     r/w */
            uint32_t RESERVED_3              :  1;  /* [3]     rsvd */
            uint32_t GPT_INT_SEL0            :  1;  /* [4]     r/w */
            uint32_t GPT_INT_SEL1            :  1;  /* [5]     r/w */
            uint32_t GPT_INT_SEL2            :  1;  /* [6]     r/w */
            uint32_t GPT_INT_SEL3            :  1;  /* [7]     r/w */
            uint32_t GPT0_INT_SEL            :  1;  /* [8]     r/w */
            uint32_t GPT1_INT_SEL            :  1;  /* [9]     r/w */
            uint32_t GPT2_INT_SEL            :  1;  /* [10]    r/w */
            uint32_t GPT3_INT_SEL            :  1;  /* [11]    r/w */
            uint32_t PDM_AUDIO_SEL           :  1;  /* [12]    r/w */
            uint32_t RESERVED_13             :  1;  /* [13]    r/w */
            uint32_t RESERVED_31_14          : 18;  /* [31:14] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PERI_CLK_SRC;

    /* 0x0e0: */
    union {
        struct {
            uint32_t RESERVE_IN              : 10;  /* [9:0]   r/w */
            uint32_t RESERVE_OUT             :  6;  /* [15:10] r/o */
            uint32_t RESERVED_31_16          : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_RSVD;

    /* 0x0e4: */
    union {
        struct {
            uint32_t GPT0_CLK_DIV            :  6;  /* [5:0]   r/w */
            uint32_t GPT0_FREQ_CHANGE        :  1;  /* [6]     r/w */
            uint32_t GPT0_CLK_SEL1           :  2;  /* [8:7]   r/w */
            uint32_t GPT0_CLK_SEL0           :  2;  /* [10:9]  r/w */
            uint32_t RESERVED_31_11          : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } GPT0_CTRL;

    /* 0x0e8: */
    union {
        struct {
            uint32_t GPT1_CLK_DIV            :  6;  /* [5:0]   r/w */
            uint32_t GPT1_FREQ_CHANGE        :  1;  /* [6]     r/w */
            uint32_t GPT1_CLK_SEL1           :  2;  /* [8:7]   r/w */
            uint32_t GPT1_CLK_SEL0           :  2;  /* [10:9]  r/w */
            uint32_t RESERVED_31_11          : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } GPT1_CTRL;

    /* 0x0ec: */
    union {
        struct {
            uint32_t GPT2_CLK_DIV            :  6;  /* [5:0]   r/w */
            uint32_t GPT2_FREQ_CHANGE        :  1;  /* [6]     r/w */
            uint32_t GPT2_CLK_SEL1           :  2;  /* [8:7]   r/w */
            uint32_t GPT2_CLK_SEL0           :  2;  /* [10:9]  r/w */
            uint32_t RESERVED_31_11          : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } GPT2_CTRL;

    /* 0x0f0: */
    union {
        struct {
            uint32_t GPT3_CLK_DIV            :  6;  /* [5:0]   r/w */
            uint32_t GPT3_FREQ_CHANGE        :  1;  /* [6]     r/w */
            uint32_t GPT3_CLK_SEL1           :  2;  /* [8:7]   r/w */
            uint32_t GPT3_CLK_SEL0           :  2;  /* [10:9]  r/w */
            uint32_t RESERVED_31_11          : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } GPT3_CTRL;

    /* 0x0f4: */
    union {
        struct {
            uint32_t WAKEUP0                 :  1;  /* [0]     r/w */
            uint32_t WAKEUP1                 :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2           : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } WAKEUP_EDGE_DETECT;

    /* 0x0f8: */
    union {
        struct {
            uint32_t PMU_CLK_DIV             :  4;  /* [3:0]   r/w */
            uint32_t RTC_CLK_EN              :  1;  /* [4]     r/w */
            uint32_t RTC_INT_SEL             :  1;  /* [5]     r/w */
            uint32_t DMA_CLK_GATE_EN         :  1;  /* [6]     r/w */
            uint32_t APB0_CLK_DIV            :  2;  /* [8:7]   r/w */
            uint32_t APB1_CLK_DIV            :  2;  /* [10:9]  r/w */
            uint32_t RESERVED_31_11          : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } AON_CLK_CTRL;

    /* 0x0fc: */
    union {
        struct {
            uint32_t RESERVED_2_0            :  3;  /* [2:0]   r/w */
            uint32_t RESERVED_3              :  1;  /* [3]     r/w */
            uint32_t RESERVED_4              :  1;  /* [4]     r/w */
            uint32_t RESERVED_5              :  1;  /* [5]     r/w */
            uint32_t RESERVED_6              :  1;  /* [6]     r/w */
            uint32_t RESERVED_7              :  1;  /* [7]     r/w */
            uint32_t GAU_DIV                 :  5;  /* [12:8]  r/w */
            uint32_t RC32M_DIV               :  5;  /* [17:13] r/w */
            uint32_t RC32M_GATE              :  1;  /* [18]    r/w */
            uint32_t RESERVED_31_19          : 13;  /* [31:19] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PERI3_CTRL;

    /* 0x100: */
    union {
        struct {
	   uint32_t RESERVED_6_0	     :  6;   /* [5:0] */
	   uint32_t WAKE0_PULLUP_EXT	     :  1;   /* [6] */
	   uint32_t WAKE0_PULLUP_ENABLE	     :  1;   /* [7] */
	   uint32_t WAKE0_PULLDOWN_EXT	     :  1;   /* [8] */
	   uint32_t WAKE0_PULLDOWN_ENABLE    :  1;   /* [9] */
	   uint32_t RESERVED_15_10	     :  6;   /* [10:15] */
	   uint32_t WAKE1_PULLUP_EXT	     :  1;   /* [16] */
	   uint32_t WAKE1_PULLUP_ENABLE	     :  1;   /* [17] */
	   uint32_t WAKE1_PULLDOWN_EXT	     :  1;   /* [18] */
	   uint32_t WAKE1_PULLDOWN_ENABLE    :  1;   /* [19] */
	   uint32_t RESERVED_31_20	     : 12;   /* [21:20] */
        } BF;
        uint32_t WORDVAL;
    } IO_LPM_G03_;

    /* 0x104: */
    union {
        struct {
            uint32_t IO_LPM_G47              : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } IO_LPM_G47_;

    /* 0x108: */
    union {
        struct {
            uint32_t IO_LPM_G811             : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } IO_LPM_G811_;

    /* 0x10c: */
    union {
        struct {
            uint32_t IO_LPM_G1215            : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } IO_LPM_G1215_;

    /* 0x110: */
    union {
        struct {
            uint32_t IO_LPM_G1617            : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } IO_LPM_G1617_;

    /* 0x114: */
    union {
        struct {
            uint32_t RESERVED_2_0            :  3;  /* [2:0]   rsvd */
            uint32_t PIN0_WAKEUP_MASK        :  1;  /* [3]     r/w */
            uint32_t PIN1_WAKEUP_MASK        :  1;  /* [4]     r/w */
            uint32_t RTC_WAKEUP_MASK         :  1;  /* [5]     r/w */
            uint32_t PMIP_COMP_WAKEUP_MASK   :  1;  /* [6]     r/w */
            uint32_t WL_WAKEUP_MASK          :  1;  /* [7]     r/w */
            uint32_t RESERVED_31_8           : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } WAKEUP_MASK;

    /* 0x118: */
    union {
        struct {
            uint32_t PD                      :  1;  /* [0]     r/w */
            uint32_t REFCLK_SYS_REQ          :  1;  /* [1]     r/w */
            uint32_t REFCLK_AUD_REQ          :  1;  /* [2]     r/w */
            uint32_t REFCLK_USB_REQ          :  1;  /* [3]     r/w */
            uint32_t REFCLK_SYS_RDY          :  1;  /* [4]     r/o */
            uint32_t REFCLK_AUD_RDY          :  1;  /* [5]     r/o */
            uint32_t REFCLK_USB_RDY          :  1;  /* [6]     r/o */
            uint32_t WL_PD_DEL_CFG           : 11;  /* [17:7]  r/w */
            uint32_t RESERVED_31_18          : 14;  /* [31:18] rsvd */
        } BF;
        uint32_t WORDVAL;
    } WLAN_CTRL;

    /* 0x11c: */
    union {
        struct {
            uint32_t RESERVED_10_0           : 11;  /* [10:0]  rsvd */
            uint32_t MCI_WL_WAKEUP           :  1;  /* [11]    r/w */
            uint32_t RESERVED_31_12          : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } WLAN_CTRL1;

};

typedef volatile struct pmu_reg pmu_reg_t;

#ifdef PMU_IMPL
BEGIN_REG_SECTION(pmu_registers)
pmu_reg_t PMUREG;
END_REG_SECTION(pmu_registers)
#else
extern pmu_reg_t PMUREG;
#endif

#endif /* _PMU_REG_H */
