/** @file gpt_reg.h
*
*  @brief This file contains automatically generated register structure
*
*  (C) Copyright 2012-2018 Marvell International Ltd. All Rights Reserved
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

/****************************************************************************//*
 * @version  V1.0.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 ******************************************************************************/

#ifndef _GPT_REG_H
#define _GPT_REG_H

struct gpt_reg {
    /* 0x000: Counter Enable Register */
    union {
        struct {
            uint32_t CNT_START      :  1;  /* [0]     w/o */
            uint32_t CNT_STOP       :  1;  /* [1]     w/o */
            uint32_t CNT_RESET      :  1;  /* [2]     w/o */
            uint32_t RESERVED_15_3  : 13;  /* [15:3]  rsvd */
            uint32_t CNT_RUN        :  1;  /* [16]    r/o */
            uint32_t CNT_RST_DONE   :  1;  /* [17]    r/o */
            uint32_t STS_RESETN     :  1;  /* [18]    r/o */
            uint32_t RESERVED_31_19 : 13;  /* [31:19] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CNT_EN_REG;

    uint8_t zReserved0x004[28];  /* pad 0x004 - 0x01f */

    /* 0x020: Status Register */
    union {
        struct {
            uint32_t CH0_STS        :  1;  /* [0]     r/w1clr */
            uint32_t CH1_STS        :  1;  /* [1]     r/w1clr */
            uint32_t CH2_STS        :  1;  /* [2]     r/w1clr */
            uint32_t CH3_STS        :  1;  /* [3]     r/w1clr */
            uint32_t CH4_STS        :  1;  /* [4]     r/w1clr */
            uint32_t CH5_STS        :  1;  /* [5]     r/w1clr */
            uint32_t RESERVED_7_6   :  2;  /* [7:6]   rsvd */
            uint32_t CH0_ERR_STS    :  1;  /* [8]     r/w1clr */
            uint32_t CH1_ERR_STS    :  1;  /* [9]     r/w1clr */
            uint32_t CH2_ERR_STS    :  1;  /* [10]    r/w1clr */
            uint32_t CH3_ERR_STS    :  1;  /* [11]    r/w1clr */
            uint32_t CH4_ERR_STS    :  1;  /* [12]    r/w1clr */
            uint32_t CH5_ERR_STS    :  1;  /* [13]    r/w1clr */
            uint32_t RESERVED_15_14 :  2;  /* [15:14] rsvd */
            uint32_t CNT_UPP_STS    :  1;  /* [16]    r/w1clr */
            uint32_t RESERVED_23_17 :  7;  /* [23:17] rsvd */
            uint32_t DMA0_OF_STS    :  1;  /* [24]    r/w1clr */
            uint32_t DMA1_OF_STS    :  1;  /* [25]    r/w1clr */
            uint32_t RESERVED_31_26 :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } STS_REG;

    /* 0x024: Interrupt Register */
    union {
        struct {
            uint32_t CH0_INTR       :  1;  /* [0]     r/o */
            uint32_t CH1_INTR       :  1;  /* [1]     r/o */
            uint32_t CH2_INTR       :  1;  /* [2]     r/o */
            uint32_t CH3_INTR       :  1;  /* [3]     r/o */
            uint32_t CH4_INTR       :  1;  /* [4]     r/o */
            uint32_t CH5_INTR       :  1;  /* [5]     r/o */
            uint32_t RESERVED_7_6   :  2;  /* [7:6]   rsvd */
            uint32_t CH0_ERR_INTR   :  1;  /* [8]     r/o */
            uint32_t CH1_ERR_INTR   :  1;  /* [9]     r/o */
            uint32_t CH2_ERR_INTR   :  1;  /* [10]    r/o */
            uint32_t CH3_ERR_INTR   :  1;  /* [11]    r/o */
            uint32_t CH4_ERR_INTR   :  1;  /* [12]    r/o */
            uint32_t CH5_ERR_INTR   :  1;  /* [13]    r/o */
            uint32_t RESERVED_15_14 :  2;  /* [15:14] rsvd */
            uint32_t CNT_UPP_INTR   :  1;  /* [16]    r/o */
            uint32_t RESERVED_23_17 :  7;  /* [23:17] rsvd */
            uint32_t DMA0_OF_INTR   :  1;  /* [24]    r/o */
            uint32_t DMA1_OF_INTR   :  1;  /* [25]    r/o */
            uint32_t RESERVED_31_26 :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_REG;

    /* 0x028: Interrupt Mask Register */
    union {
        struct {
            uint32_t CH0_MSK        :  1;  /* [0]     r/w */
            uint32_t CH1_MSK        :  1;  /* [1]     r/w */
            uint32_t CH2_MSK        :  1;  /* [2]     r/w */
            uint32_t CH3_MSK        :  1;  /* [3]     r/w */
            uint32_t CH4_MSK        :  1;  /* [4]     r/w */
            uint32_t CH5_MSK        :  1;  /* [5]     r/w */
            uint32_t RESERVED_7_6   :  2;  /* [7:6]   rsvd */
            uint32_t CH0_ERR_MSK    :  1;  /* [8]     r/w */
            uint32_t CH1_ERR_MSK    :  1;  /* [9]     r/w */
            uint32_t CH2_ERR_MSK    :  1;  /* [10]    r/w */
            uint32_t CH3_ERR_MSK    :  1;  /* [11]    r/w */
            uint32_t CH4_ERR_MSK    :  1;  /* [12]    r/w */
            uint32_t CH5_ERR_MSK    :  1;  /* [13]    r/w */
            uint32_t RESERVED_15_14 :  2;  /* [15:14] rsvd */
            uint32_t CNT_UPP_MSK    :  1;  /* [16]    r/w */
            uint32_t RESERVED_23_17 :  7;  /* [23:17] rsvd */
            uint32_t DMA0_OF_MSK    :  1;  /* [24]    r/w */
            uint32_t DMA1_OF_MSK    :  1;  /* [25]    r/w */
            uint32_t RESERVED_31_26 :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_MSK_REG;

    uint8_t zReserved0x02c[20];  /* pad 0x02c - 0x03f */

    /* 0x040: Counter Control Register */
    union {
        struct {
            uint32_t RESERVED_3_0   :  4;  /* [3:0]   rsvd */
            uint32_t CNT_DBG_ACT    :  1;  /* [4]     r/w */
            uint32_t RESERVED_7_5   :  3;  /* [7:5]   rsvd */
            uint32_t CNT_UPDT_MOD   :  2;  /* [9:8]   r/w */
            uint32_t RESERVED_31_10 : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CNT_CNTL_REG;

    uint8_t zReserved0x044[12];  /* pad 0x044 - 0x04f */

    /* 0x050: Counter Value Register */
    union {
        struct {
            uint32_t CNT_VAL        : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } CNT_VAL_REG;

    uint8_t zReserved0x054[12];  /* pad 0x054 - 0x05f */

    /* 0x060: Counter Upper Value Register */
    union {
        struct {
            uint32_t UPP_VAL        : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } CNT_UPP_VAL_REG;

    uint8_t zReserved0x064[28];  /* pad 0x064 - 0x07f */

    /* 0x080: Clock Control Register */
    union {
        struct {
            uint32_t CLK_SRC        :  1;  /* [0]     r/w */
            uint32_t RESERVED_7_1   :  7;  /* [7:1]   rsvd */
            uint32_t CLK_DIV        :  4;  /* [11:8]  r/w */
            uint32_t RESERVED_15_12 :  4;  /* [15:12] rsvd */
            uint32_t CLK_PRE        :  8;  /* [23:16] r/w */
            uint32_t RESERVED_31_24 :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK_CNTL_REG;

    uint8_t zReserved0x084[4];  /* pad 0x084 - 0x087 */

    /* 0x088: Input Capture Control Register */
    union {
        struct {
            uint32_t IC_WIDTH       :  3;  /* [2:0]   r/w */
            uint32_t RESERVED_3     :  1;  /* [3]     rsvd */
            uint32_t IC_DIV         :  3;  /* [6:4]   r/w */
            uint32_t RESERVED_31_7  : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IC_CNTL_REG;

    uint8_t zReserved0x08c[20];  /* pad 0x08c - 0x09f */

    /* 0x0a0: DMA Control Enable Register */
    union {
        struct {
            uint32_t DMA0_EN        :  1;  /* [0]     r/w */
            uint32_t DMA1_EN        :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DMA_CNTL_EN_REG;

    /* 0x0a4: DMA Control Channel Register */
    union {
        struct {
            uint32_t DMA0_CH        :  3;  /* [2:0]   r/w */
            uint32_t RESERVED_3     :  1;  /* [3]     rsvd */
            uint32_t DMA1_CH        :  3;  /* [6:4]   r/w */
            uint32_t RESERVED_31_7  : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DMA_CNTL_CH_REG;

    uint8_t zReserved0x0a8[40];  /* pad 0x0a8 - 0x0cf */

    /* 0x0d0: ADC/DAC Trigger Control Register */
    union {
        struct {
            uint32_t TRIG_CHSEL     :  3;  /* [2:0]   r/w */
            uint32_t RESERVED_7_3   :  5;  /* [7:3]   rsvd */
            uint32_t TRIG_EN        :  1;  /* [8]     r/w */
            uint32_t RESERVED_31_9  : 23;  /* [31:9]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TCR_REG;

    uint8_t zReserved0x0d4[4];  /* pad 0x0d4 - 0x0d7 */

    /* 0x0d8: ADC/DAC Trigger Delay Register */
    union {
        struct {
            uint32_t TRIG_DLY       : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } TDR_REG;

    uint8_t zReserved0x0dc[20];  /* pad 0x0dc - 0x0ef */

    /* 0x0f0: User Request Register */
    union {
        struct {
            uint32_t CH0_USER_ITRIG :  1;  /* [0]     w/o */
            uint32_t CH1_USER_ITRIG :  1;  /* [1]     w/o */
            uint32_t CH2_USER_ITRIG :  1;  /* [2]     w/o */
            uint32_t CH3_USER_ITRIG :  1;  /* [3]     w/o */
            uint32_t CH4_USER_ITRIG :  1;  /* [4]     w/o */
            uint32_t CH5_USER_ITRIG :  1;  /* [5]     w/o */
            uint32_t RESERVED_7_6   :  2;  /* [7:6]   rsvd */
            uint32_t CH0_RST        :  1;  /* [8]     w/o */
            uint32_t CH1_RST        :  1;  /* [9]     w/o */
            uint32_t CH2_RST        :  1;  /* [10]    w/o */
            uint32_t CH3_RST        :  1;  /* [11]    w/o */
            uint32_t CH4_RST        :  1;  /* [12]    w/o */
            uint32_t CH5_RST        :  1;  /* [13]    w/o */
            uint32_t RESERVED_15_14 :  2;  /* [15:14] rsvd */
            uint32_t CH0_CMR_UPDT   :  1;  /* [16]    w/o */
            uint32_t CH1_CMR_UPDT   :  1;  /* [17]    w/o */
            uint32_t CH2_CMR_UPDT   :  1;  /* [18]    w/o */
            uint32_t CH3_CMR_UPDT   :  1;  /* [19]    w/o */
            uint32_t CH4_CMR_UPDT   :  1;  /* [20]    w/o */
            uint32_t CH5_CMR_UPDT   :  1;  /* [21]    w/o */
            uint32_t RESERVED_31_22 : 10;  /* [31:22] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USER_REQ_REG;

    uint8_t zReserved0x0f4[268];  /* pad 0x0f4 - 0x1ff */

    struct {
        /* 0x200: Channel 0 Control Register */
        union {
            struct {
                uint32_t CH_IO          :  3;  /* [2:0]   r/w */
                uint32_t RESERVED_11_3  :  9;  /* [11:3]  rsvd */
                uint32_t IC_EDGE        :  3;  /* [14:12] r/w */
                uint32_t RESERVED_15    :  1;  /* [15]    rsvd */
                uint32_t POL            :  1;  /* [16]    r/w */
                uint32_t RESERVED_31_17 : 15;  /* [31:17] rsvd */
            } BF;
            uint32_t WORDVAL;
        } CHX_CNTL_REG;

        uint8_t zReserved0x204[12];  /* pad 0x204 - 0x20f */

        /* 0x210: Channel 0 Counter Match Register 0 */
        union {
            struct {
                uint32_t CMR0           : 32;  /* [31:0]  r/w */
            } BF;
            uint32_t WORDVAL;
        } CHX_CMR0_REG;

        /* 0x214: Channel 0 Status Register 0 */
        union {
            struct {
                uint32_t OUT_ST         :  1;  /* [0]     r/o */
                uint32_t RESERVED_31_1  : 31;  /* [31:1]  rsvd */
            } BF;
            uint32_t WORDVAL;
        } CHX_STS_REG;

        uint8_t zReserved0x218[8];  /* pad 0x218 - 0x21f */

        /* 0x220: Channel 0 Counter Match Register 1 */
        union {
            struct {
                uint32_t CMR1           : 32;  /* [31:0]  r/w */
            } BF;
            uint32_t WORDVAL;
        } CHX_CMR1_REG;

        uint8_t zReserved0x224[28];  /* pad 0x224 - 0x23f */
    } CH[6];

};

typedef volatile struct gpt_reg gpt_reg_t;

#ifdef GPT_IMPL
BEGIN_REG_SECTION(gpt_registers)
gpt_reg_t GPTREG;
END_REG_SECTION(gpt_registers)
#else
extern gpt_reg_t GPTREG;
#endif

#endif /* _GPT_REG_H */
