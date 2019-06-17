/** @file adc_reg.h
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


#ifndef _ADC_REG_H
#define _ADC_REG_H

struct adc_reg {
    /* 0x00: */
    union {
        struct {
            uint32_t CONV_START                       :  1;  /* [0]     r/w */
            uint32_t SOFT_RST                         :  1;  /* [1]     r/w */
            uint32_t SOFT_CLK_RST                     :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3                    : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CMD;

    /* 0x04: */
    union {
        struct {
            uint32_t GPADC_MASTER                     :  1;  /* [0]     r/w */
            uint32_t GLOBAL_EN                        :  1;  /* [1]     r/w */
            uint32_t FORCE_CLK_ON                     :  1;  /* [2]     r/w */
            uint32_t CLK_ANA64M_INV                   :  1;  /* [3]     r/w */
            uint32_t CLK_ANA2M_INV                    :  1;  /* [4]     r/w */
            uint32_t ADC_CAL_EN                       :  1;  /* [5]     r/w */
            uint32_t RESERVED_7_6                     :  2;  /* [7:6]   rsvd */
            uint32_t CLK_DIV_RATIO                    :  6;  /* [13:8]  r/w */
            uint32_t RESERVED_31_14                   : 18;  /* [31:14] rsvd */
        } BF;
        uint32_t WORDVAL;
    } GENERAL;

    /* 0x08: */
    union {
        struct {
            uint32_t TRIGGER_SEL                      :  4;  /* [3:0]   r/w */
            uint32_t TRIGGER_EN                       :  1;  /* [4]     r/w */
            uint32_t DUAL_MODE                        :  1;  /* [5]     r/w */
            uint32_t SINGLE_DUAL                      :  1;  /* [6]     r/w */
            uint32_t RESERVED_7                       :  1;  /* [7]     rsvd */
            uint32_t CONT_CONV_EN                     :  1;  /* [8]     r/w */
            uint32_t DATA_FORMAT_SEL                  :  1;  /* [9]     r/w */
            uint32_t CAL_VREF_SEL                     :  1;  /* [10]    r/w */
            uint32_t CAL_DATA_RST                     :  1;  /* [11]    r/w */
            uint32_t CAL_DATA_SEL                     :  1;  /* [12]    r/w */
            uint32_t AVG_SEL                          :  3;  /* [15:13] r/w */
            uint32_t SCAN_LENGTH                      :  4;  /* [19:16] r/w */
            uint32_t PWR_MODE                         :  1;  /* [20]    r/w */
            uint32_t RESERVED_31_21                   : 11;  /* [31:21] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CONFIG;

    /* 0x0c: */
    union {
        struct {
            uint32_t WARMUP_TIME                      :  5;  /* [4:0]   r/w */
            uint32_t BYPASS_WARMUP                    :  1;  /* [5]     r/w */
            uint32_t RESERVED_31_6                    : 26;  /* [31:6]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } INTERVAL;

    /* 0x10: */
    union {
        struct {
            uint32_t TSEXT_SEL                        :  1;  /* [0]     r/w */
            uint32_t TS_EN                            :  1;  /* [1]     r/w */
            uint32_t VREF_SCF_BYPASS                  :  1;  /* [2]     r/w */
            uint32_t VREF_CHOP_EN                     :  1;  /* [3]     r/w */
            uint32_t VREF_SEL                         :  2;  /* [5:4]   r/w */
            uint32_t RESERVED_9_6                     :  4;  /* [9:6]   rsvd */
            uint32_t SINGLEDIFF                       :  1;  /* [10]    r/w */
            uint32_t INBUF_GAIN                       :  2;  /* [12:11] r/w */
            uint32_t INBUF_CHOP_EN                    :  1;  /* [13]    r/w */
            uint32_t INBUF_EN                         :  1;  /* [14]    r/w */
            uint32_t CHOP_EN                          :  1;  /* [15]    r/w */
            uint32_t BIAS_SEL                         :  1;  /* [16]    r/w */
            uint32_t RES_SEL                          :  2;  /* [18:17] r/w */
            uint32_t RESERVED_31_19                   : 13;  /* [31:19] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ANA;

    uint8_t zReserved0x14[4];  /* pad 0x14 - 0x17 */

    /* 0x18: adc converation sequence 1 */
    union {
        struct {
            uint32_t SCAN_CH_0                        :  4;  /* [3:0]   r/w */
            uint32_t SCAN_CH_1                        :  4;  /* [7:4]   r/w */
            uint32_t SCAN_CH_2                        :  4;  /* [11:8]  r/w */
            uint32_t SCAN_CH_3                        :  4;  /* [15:12] r/w */
            uint32_t SCAN_CH_4                        :  4;  /* [19:16] r/w */
            uint32_t SCAN_CH_5                        :  4;  /* [23:20] r/w */
            uint32_t SCAN_CH_6                        :  4;  /* [27:24] r/w */
            uint32_t SCAN_CH_7                        :  4;  /* [31:28] r/w */
        } BF;
        uint32_t WORDVAL;
    } SCN1;
    
    /* 0x1c: adc converation sequence 2 */
    union {
        struct {
            uint32_t SCAN_CH_8                        :  4;  /* [3:0]   r/w */
            uint32_t SCAN_CH_9                        :  4;  /* [7:4]   r/w */
            uint32_t SCAN_CH_10                       :  4;  /* [11:8]  r/w */
            uint32_t SCAN_CH_11                       :  4;  /* [15:12] r/w */
            uint32_t SCAN_CH_12                       :  4;  /* [19:16] r/w */
            uint32_t SCAN_CH_13                       :  4;  /* [23:20] r/w */
            uint32_t SCAN_CH_14                       :  4;  /* [27:24] r/w */
            uint32_t SCAN_CH_15                       :  4;  /* [31:28] r/w */
        } BF;
        uint32_t WORDVAL;
    } SCN2;
    
    /* 0x20: */
    union {
        struct {
            uint32_t WIDTH_SEL                        :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1                    : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RESULT_BUF;

    uint8_t zReserved0x24[4];  /* pad 0x24 - 0x27 */

    /* 0x28: */
    union {
        struct {
            uint32_t DMA_EN                           :  1;  /* [0]     r/w */
            uint32_t FIFO_THL                         :  2;  /* [2:1]   r/w */
            uint32_t RESERVED_31_3                    : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DMAR;

    /* 0x2c: */
    union {
        struct {
            uint32_t ACT                              :  1;  /* [0]     r/o */
            uint32_t FIFO_NE                          :  1;  /* [1]     r/o */
            uint32_t FIFO_FULL                        :  1;  /* [2]     r/o */
            uint32_t FIFO_DATA_COUNT                  :  6;  /* [8:3]   r/o */
            uint32_t RESERVED_31_9                    : 23;  /* [31:9]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } STATUS;

    /* 0x30: */
    union {
        struct {
            uint32_t RDY                              :  1;  /* [0]     r/o */
            uint32_t GAINSAT                          :  1;  /* [1]     r/o */
            uint32_t OFFSAT                           :  1;  /* [2]     r/o */
            uint32_t DATASAT_NEG                      :  1;  /* [3]     r/o */
            uint32_t DATASAT_POS                      :  1;  /* [4]     r/o */
            uint32_t FIFO_OVERRUN                     :  1;  /* [5]     r/o */
            uint32_t FIFO_UNDERRUN                    :  1;  /* [6]     r/o */
            uint32_t RESERVED_31_7                    : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISR;

    /* 0x34: */
    union {
        struct {
            uint32_t RDY_MASK                         :  1;  /* [0]     r/w */
            uint32_t GAINSAT_MASK                     :  1;  /* [1]     r/w */
            uint32_t OFFSAT_MASK                      :  1;  /* [2]     r/w */
            uint32_t DATASAT_NEG_MASK                 :  1;  /* [3]     r/w */
            uint32_t DATASAT_POS_MASK                 :  1;  /* [4]     r/w */
            uint32_t FIFO_OVERRUN_MASK                :  1;  /* [5]     r/w */
            uint32_t FIFO_UNDERRUN_MASK               :  1;  /* [6]     r/w */
            uint32_t RESERVED_31_7                    : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IMR;

    /* 0x38: */
    union {
        struct {
            uint32_t RDY_RAW                          :  1;  /* [0]     r/o */
            uint32_t GAINSAT_RAW                      :  1;  /* [1]     r/o */
            uint32_t OFFSAT_RAW                       :  1;  /* [2]     r/o */
            uint32_t DATASAT_NEG_RAW                  :  1;  /* [3]     r/o */
            uint32_t DATASAT_POS_RAW                  :  1;  /* [4]     r/o */
            uint32_t FIFO_OVERRUN_RAW                 :  1;  /* [5]     r/o */
            uint32_t FIFO_UNDERRUN_RAW                :  1;  /* [6]     r/o */
            uint32_t RESERVED_31_7                    : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IRSR;

    /* 0x3c: */
    union {
        struct {
            uint32_t RDY_CLR                          :  1;  /* [0]     r/w */
            uint32_t GAINSAT_CLR                      :  1;  /* [1]     r/w */
            uint32_t OFFSAT_CLR                       :  1;  /* [2]     r/w */
            uint32_t DATASAT_NEG_CLR                  :  1;  /* [3]     r/w */
            uint32_t DATASAT_POS_CLR                  :  1;  /* [4]     r/w */
            uint32_t FIFO_OVERRUN_CLR                 :  1;  /* [5]     r/w */
            uint32_t FIFO_UNDERRUN_CLR                :  1;  /* [6]     r/w */
            uint32_t RESERVED_31_7                    : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ICR;

    uint8_t zReserved0x40[4];  /* pad 0x40 - 0x43 */

    /* 0x44: */
    union {
        struct {
            uint32_t DATA                             : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } RESULT;

    /* 0x48: */
    union {
        struct {
            uint32_t RAW_DATA                         : 22;  /* [21:0]  r/o */
            uint32_t RESERVED_31_22                   : 10;  /* [31:22] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RAW_RESULT;

    /* 0x4c: */
    union {
        struct {
            uint32_t OFFSET_CAL                       : 16;  /* [15:0]  r/o */
            uint32_t OFFSET_CAL_USR                   : 16;  /* [31:16] r/w */
        } BF;
        uint32_t WORDVAL;
    } OFFSET_CAL;

    /* 0x50: */
    union {
        struct {
            uint32_t GAIN_CAL                         : 16;  /* [15:0]  r/o */
            uint32_t GAIN_CAL_USR                     : 16;  /* [31:16] r/w */
        } BF;
        uint32_t WORDVAL;
    } GAIN_CAL;

    /* 0x54: */
    union {
        struct {
            uint32_t TEST_EN                          :  1;  /* [0]     r/w */
            uint32_t TEST_SEL                         :  3;  /* [3:1]   r/w */
            uint32_t RESERVED_31_4                    : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TEST;

    /* 0x58: */
    union {
        struct {
            uint32_t EN                               :  1;  /* [0]     r/w */
            uint32_t RESERVED_2_1                     :  2;  /* [2:1]   rsvd */
            uint32_t PGA_GAIN                         :  3;  /* [5:3]   r/w */
            uint32_t PGA_CM                           :  3;  /* [8:6]   r/w */
            uint32_t PGA_CHOP_EN                      :  1;  /* [9]     r/w */
            uint32_t RESERVED_31_10                   : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } AUDIO;

    /* 0x5c: */
    union {
        struct {
            uint32_t DET_EN                           :  1;  /* [0]     r/w */
            uint32_t LEVEL_SEL                        :  3;  /* [3:1]   r/w */
            uint32_t RESERVED_31_4                    : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } VOICE_DET;

    /* 0x60: */
    union {
        struct {
            uint32_t UNUSED_RESERVED_ADC_CONTROL_BITS : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16                   : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RSVD;

};

typedef volatile struct adc_reg adc_reg_t;

#ifdef ADC_IMPL
BEGIN_REG_SECTION(adc_registers)
adc_reg_t ADCREG;
END_REG_SECTION(adc_registers)
#else
extern adc_reg_t ADCREG;
#endif

#endif /* _ADC_REG_H */
