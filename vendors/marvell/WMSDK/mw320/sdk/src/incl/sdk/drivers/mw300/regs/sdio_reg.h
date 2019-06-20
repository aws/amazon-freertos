/** @file
*
*  @brief
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
 * @file     sdio_reg.h
 * @brief    Automatically generated register structure.
 * @version  V1.0.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 *
 *******************************************************************************/

#ifndef _SDIO_REG_H
#define _SDIO_REG_H

struct sdio_reg {
    /* 0x00: SDIO System Address Register */
    union {
        struct {
            uint32_t SYSADDR        : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } SYSADDR;

    /* 0x04: SDIO Block Control Register */
    union {
        struct {
            uint32_t XFR_BLKSZ      : 12;  /* [11:0]  r/w */
            uint32_t DMA_BUFSZ      :  3;  /* [14:12] r/w */
            uint32_t RESERVED_15    :  1;  /* [15]    rsvd */
            uint32_t BLK_CNT        : 16;  /* [31:16] r/w */
        } BF;
        uint32_t WORDVAL;
    } BLK_CNTL;

    /* 0x08: SDIO Command Argument Register */
    union {
        struct {
            uint32_t ARG            : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } ARG;

    /* 0x0c: SDIO Command and Transfer Mode Register */
    union {
        struct {
            uint32_t DMA_EN         :  1;  /* [0]     r/w */
            uint32_t BLKCNTEN       :  1;  /* [1]     r/w */
            uint32_t RESERVED_3_2   :  2;  /* [3:2]   rsvd */
            uint32_t DXFRDIR        :  1;  /* [4]     r/w */
            uint32_t MS_BLKSEL      :  1;  /* [5]     r/w */
            uint32_t RESERVED_15_6  : 10;  /* [15:6]  rsvd */
            uint32_t RES_TYPE       :  2;  /* [17:16] r/w */
            uint32_t RESERVED_18    :  1;  /* [18]    rsvd */
            uint32_t CRCCHKEN       :  1;  /* [19]    r/w */
            uint32_t IDXCHKEN       :  1;  /* [20]    r/w */
            uint32_t DPSEL          :  1;  /* [21]    r/w */
            uint32_t CMD_TYPE       :  2;  /* [23:22] r/w */
            uint32_t CMD_IDX        :  6;  /* [29:24] r/w */
            uint32_t RESERVED_31_30 :  2;  /* [31:30] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CMD_XFRMD;

    /* 0x10-0x1c: SDIO Command Response Register0 */
    union {
        struct {
            uint32_t RESP_31_0      : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } RESP[4];

    /* 0x20: SDIO Buffer Data Port Register */
    union {
        struct {
            uint32_t BFR_DATA       : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } DP;

    /* 0x24: MMC4 Present State Register */
    union {
        struct {
            uint32_t CCMDINHBT      :  1;  /* [0]     r/o */
            uint32_t DCMDINHBT      :  1;  /* [1]     r/o */
            uint32_t DATACTV        :  1;  /* [2]     r/o */
            uint32_t RESERVED_7_3   :  5;  /* [7:3]   rsvd */
            uint32_t WRACTV         :  1;  /* [8]     r/o */
            uint32_t RDACTV         :  1;  /* [9]     r/o */
            uint32_t BUFWREN        :  1;  /* [10]    r/o */
            uint32_t BUFRDEN        :  1;  /* [11]    r/o */
            uint32_t RESERVED_15_12 :  4;  /* [15:12] rsvd */
            uint32_t CDINSTD        :  1;  /* [16]    r/o */
            uint32_t CDSTBL         :  1;  /* [17]    r/o */
            uint32_t CDDETLVL       :  1;  /* [18]    r/o */
            uint32_t WPSWLVL        :  1;  /* [19]    r/o */
            uint32_t LWRDATLVL      :  4;  /* [23:20] r/o */
            uint32_t CMDLVL         :  1;  /* [24]    r/o */
            uint32_t UPRDATLVL      :  4;  /* [28:25] r/o */
            uint32_t RESERVED_31_29 :  3;  /* [31:29] rsvd */
        } BF;
        uint32_t WORDVAL;
    } STATE;

    /* 0x28: SDIO Host Control Register 1 */
    union {
        struct {
            uint32_t LEDCNTL        :  1;  /* [0]     r/w */
            uint32_t _4BITMD        :  1;  /* [1]     r/w */
            uint32_t HISPEED        :  1;  /* [2]     r/w */
            uint32_t RESERVED_7_3   :  5;  /* [7:3]   rsvd */
            uint32_t BUSPWR         :  1;  /* [8]     r/w */
            uint32_t VLTGSEL        :  3;  /* [11:9]  r/w */
            uint32_t RESERVED_15_12 :  4;  /* [15:12] rsvd */
            uint32_t BGREQSTP       :  1;  /* [16]    r/w */
            uint32_t CONTREQ        :  1;  /* [17]    r/w */
            uint32_t RDWTCNTL       :  1;  /* [18]    r/w */
            uint32_t BGIRQEN        :  1;  /* [19]    r/w */
            uint32_t RESERVED_31_20 : 12;  /* [31:20] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CNTL1;

    /* 0x2c: SDIO Host Control Register 2 */
    union {
        struct {
            uint32_t INTCLKEN       :  1;  /* [0]     r/w */
            uint32_t RESERVED_1     :  1;  /* [1]     rsvd */
            uint32_t CLKEN          :  1;  /* [2]     r/w */
            uint32_t RESERVED_15_3  : 13;  /* [15:3]  rsvd */
            uint32_t DTOCNTR        :  4;  /* [19:16] r/w */
            uint32_t RESERVED_23_20 :  4;  /* [23:20] rsvd */
            uint32_t MSWRST         :  1;  /* [24]    r/w */
            uint32_t CMDSWRST       :  1;  /* [25]    r/w */
            uint32_t DATSWRST       :  1;  /* [26]    r/w */
            uint32_t RESERVED_31_27 :  5;  /* [31:27] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CNTL2;

    /* 0x30: SDIO Interrupt Status Register */
    union {
        struct I_STAT {
            uint32_t CMDCOMP        :  1;  /* [0]     r/w */
            uint32_t XFRCOMP        :  1;  /* [1]     r/w */
            uint32_t BGEVNT         :  1;  /* [2]     r/w */
            uint32_t DMAINT         :  1;  /* [3]     r/w */
            uint32_t BUFWRRDY       :  1;  /* [4]     r/w */
            uint32_t BUFRDRDY       :  1;  /* [5]     r/w */
            uint32_t CDINS          :  1;  /* [6]     r/w */
            uint32_t CDREM          :  1;  /* [7]     r/w */
            uint32_t CDINT          :  1;  /* [8]     r/o */
            uint32_t RESERVED_14_9  :  6;  /* [14:9]  rsvd */
            uint32_t ERRINT         :  1;  /* [15]    r/o */
            uint32_t CTOERR         :  1;  /* [16]    r/w */
            uint32_t CCRCERR        :  1;  /* [17]    r/w */
            uint32_t CENDERR        :  1;  /* [18]    r/w */
            uint32_t CIDXERR        :  1;  /* [19]    r/w */
            uint32_t DTOERR         :  1;  /* [20]    r/w */
            uint32_t DCRCERR        :  1;  /* [21]    r/w */
            uint32_t DENDERR        :  1;  /* [22]    r/w */
            uint32_t ILMTERR        :  1;  /* [23]    r/w */
            uint32_t RESERVED_27_24 :  4;  /* [27:24] rsvd */
            uint32_t AHBTERR        :  1;  /* [28]    r/w */
            uint32_t RESERVED_31_29 :  3;  /* [31:29] rsvd */
        } BF;
        uint32_t WORDVAL;
    } I_STAT;

    /* 0x34: SDIO Interrupt Status Enable Register */
    union {
        struct {
            uint32_t CMDCOMPSTEN    :  1;  /* [0]     r/w */
            uint32_t XFRCOMPSTEN    :  1;  /* [1]     r/w */
            uint32_t BGEVNTSTEN     :  1;  /* [2]     r/w */
            uint32_t DMAINTSTEN     :  1;  /* [3]     r/w */
            uint32_t BUFWRRDYSTEN   :  1;  /* [4]     r/w */
            uint32_t BUFRDRDYSTEN   :  1;  /* [5]     r/w */
            uint32_t CDINSSTEN      :  1;  /* [6]     r/w */
            uint32_t CDREMSTEN      :  1;  /* [7]     r/w */
            uint32_t CDINTSTEN      :  1;  /* [8]     r/w */
            uint32_t RESERVED_15_9  :  7;  /* [15:9]  rsvd */
            uint32_t CTOSTEN        :  1;  /* [16]    r/w */
            uint32_t CCRCSTEN       :  1;  /* [17]    r/w */
            uint32_t CENDSTEN       :  1;  /* [18]    r/w */
            uint32_t CIDXSTEN       :  1;  /* [19]    r/w */
            uint32_t DTOSTEN        :  1;  /* [20]    r/w */
            uint32_t DCRCSTEN       :  1;  /* [21]    r/w */
            uint32_t DENDSTEN       :  1;  /* [22]    r/w */
            uint32_t ILMTSTEN       :  1;  /* [23]    r/w */
            uint32_t RESERVED_27_24 :  4;  /* [27:24] rsvd */
            uint32_t ATERRSTEN      :  1;  /* [28]    r/w */
            uint32_t RESERVED_31_29 :  3;  /* [31:29] rsvd */
        } BF;
        uint32_t WORDVAL;
    } I_STAT_EN;

    /* 0x38: SDIO Interrupt Signal Enable Register */
    union {
        struct {
            uint32_t CMDCOMPSGEN    :  1;  /* [0]     r/w */
            uint32_t XFRCOMPSGEN    :  1;  /* [1]     r/w */
            uint32_t BGEVNTSGEN     :  1;  /* [2]     r/w */
            uint32_t DMAINTSGEN     :  1;  /* [3]     r/w */
            uint32_t BUFWRRDYSGEN   :  1;  /* [4]     r/w */
            uint32_t BUFRDRDYSGEN   :  1;  /* [5]     r/w */
            uint32_t CDINSSGEN      :  1;  /* [6]     r/w */
            uint32_t CDREMSGEN      :  1;  /* [7]     r/w */
            uint32_t CDINTSGEN      :  1;  /* [8]     r/w */
            uint32_t RESERVED_15_9  :  7;  /* [15:9]  rsvd */
            uint32_t CTOSGEN        :  1;  /* [16]    r/w */
            uint32_t CCRCSGEN       :  1;  /* [17]    r/w */
            uint32_t CENDSGEN       :  1;  /* [18]    r/w */
            uint32_t CIDXSGEN       :  1;  /* [19]    r/w */
            uint32_t DTOSGEN        :  1;  /* [20]    r/w */
            uint32_t DCRCSGEN       :  1;  /* [21]    r/w */
            uint32_t DENDSGEN       :  1;  /* [22]    r/w */
            uint32_t ILMTSGEN       :  1;  /* [23]    r/w */
            uint32_t RESERVED_27_24 :  4;  /* [27:24] rsvd */
            uint32_t ATERRSGEN      :  1;  /* [28]    r/w */
            uint32_t RESERVED_31_29 :  3;  /* [31:29] rsvd */
        } BF;
        uint32_t WORDVAL;
    } I_SIG_EN;

    uint8_t zReserved0x3c[4];  /* pad 0x3c - 0x3f */

    /* 0x40: SDIO Capabilities Register 0 */
    union {
        struct {
            uint32_t TOCLKFREQ      :  6;  /* [5:0]   r/o */
            uint32_t RESERVED_6     :  1;  /* [6]     rsvd */
            uint32_t TOCLKUNIT      :  1;  /* [7]     r/o */
            uint32_t BSCLKFREQ      :  6;  /* [13:8]  r/o */
            uint32_t RESERVED_15_14 :  2;  /* [15:14] rsvd */
            uint32_t MAXBLEN        :  2;  /* [17:16] r/o */
            uint32_t RESERVED_20_18 :  3;  /* [20:18] rsvd */
            uint32_t HISPDSPRT      :  1;  /* [21]    r/o */
            uint32_t DMASPRT        :  1;  /* [22]    r/o */
            uint32_t SUSP_RES       :  1;  /* [23]    r/o */
            uint32_t _3_3VSPRT      :  1;  /* [24]    r/o */
            uint32_t _3_0VSPRT      :  1;  /* [25]    r/o */
            uint32_t _1_8VSPRT      :  1;  /* [26]    r/o */
            uint32_t IRQMODE        :  1;  /* [27]    r/o */
            uint32_t RESERVED_31_28 :  4;  /* [31:28] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CAP0;

    uint8_t zReserved0x44[184];  /* pad 0x44 - 0xfb */

    /* 0xfc: SDIO Controller Version Status */
    union {
        struct {
            uint32_t RESERVED_15_0  : 16;  /* [15:0]  rsvd */
            uint32_t SPECVER        :  8;  /* [23:16] r/o */
            uint32_t VNDRVER        :  8;  /* [31:24] r/o */
        } BF;
        uint32_t WORDVAL;
    } VER;

};

typedef volatile struct sdio_reg sdio_reg_t;

#ifdef SDIO_IMPL
BEGIN_REG_SECTION(sdio_registers)
sdio_reg_t SDIOREG;
END_REG_SECTION(sdio_registers)
#else
extern sdio_reg_t SDIOREG;
#endif

#endif /* _SDIO_REG_H */

