/** @file usbc_reg.h
*
*  @brief Automatically generated register structure
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
 * @file     usbc_reg.h
 * @brief    Automatically generated register structure.
 * @version  V1.0.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 *
 *******************************************************************************/


#ifndef _USBC_REG_H
#define _USBC_REG_H

struct usbc_reg {
    /* 0x000: The ID register identifies the USB-HS 2.0 core and its revision. */
    union {
        struct {
            uint32_t ID                   :  6;  /* [5:0]   r/o */
            uint32_t RESERVED_7_6         :  2;  /* [7:6]   rsvd */
            uint32_t NID                  :  6;  /* [13:8]  r/o */
            uint32_t RESERVED_15_14       :  2;  /* [15:14] rsvd */
            uint32_t TAG                  :  5;  /* [20:16] r/o */
            uint32_t REVISION             :  4;  /* [24:21] r/o */
            uint32_t VERSION              :  4;  /* [28:25] r/o */
            uint32_t CIVERSION            :  3;  /* [31:29] r/o */
        } BF;
        uint32_t WORDVAL;
    } ID;

    /* 0x004: General hardware parameters as defined */
    union {
        struct {
            uint32_t RT             :  1;  /* [0]     r/o */
            uint32_t CLKC           :  2;  /* [2:1]   r/o */
            uint32_t BWT            :  1;  /* [3]     r/o */
            uint32_t PHYW           :  2;  /* [5:4]   r/o */
            uint32_t PHYM           :  4;  /* [9:6]   r/o */
            uint32_t SM             :  2;  /* [11:10] r/o */
            uint32_t RESERVED_31_12 : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } HWGENERAL;

    /* 0x008: Host hardware parameters as defined */
    union {
        struct {
            uint32_t HC             :  1;  /* [0]     r/o */
            uint32_t NPORT          :  3;  /* [3:1]   r/o */
            uint32_t RESERVED_15_4  : 12;  /* [15:4]  rsvd */
            uint32_t TTASY          :  8;  /* [23:16] r/o */
            uint32_t TTPER          :  8;  /* [31:24] r/o */
        } BF;
        uint32_t WORDVAL;
    } HWHOST;

    /* 0x00c: Device hardware parameters as defined */
    union {
        struct {
            uint32_t DC             :  1;  /* [0]     r/o */
            uint32_t DEVEP          :  5;  /* [5:1]   r/o */
            uint32_t RESERVED_31_6  : 26;  /* [31:6]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } HWDEVICE;

    /* 0x010: TX buffer hardware parameters */
    union {
        struct {
            uint32_t TXBURST        :  8;  /* [7:0]   r/o */
            uint32_t TXADD          :  8;  /* [15:8]  r/o */
            uint32_t TXCHANADD      :  8;  /* [23:16] r/o */
            uint32_t RESERVED_31_24 :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } HWTXBUF;

    /* 0x014: RX buffer hardware parameters */
    union {
        struct {
            uint32_t RXBURST        :  8;  /* [7:0]   r/o */
            uint32_t RXADD          :  8;  /* [15:8]  r/o */
            uint32_t RESERVED_31_16 : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } HWRXBUF;

    uint8_t zReserved0x018[104];  /* pad 0x018 - 0x07f */

    /* 0x080: */
    union {
        struct {
            uint32_t GPTLD          : 24;  /* [23:0]  r/w */
            uint32_t RESERVED_31_24 :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } GPTIMER0LD;

    /* 0x084: */
    union {
        struct {
            uint32_t GPTCNT         : 24;  /* [23:0]  r/o */
            uint32_t GPTMODE        :  1;  /* [24]    r/w */
            uint32_t RESERVED_29_25 :  5;  /* [29:25] rsvd */
            uint32_t GPTRST         :  1;  /* [30]    r/o */
            uint32_t GPTRUN         :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } GPTIMER0CTRL;

    /* 0x088: */
    union {
        struct {
            uint32_t GPTLD          : 24;  /* [23:0]  r/w */
            uint32_t RESERVED_31_24 :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } GPTTIMER1LD;

    /* 0x08c: */
    union {
        struct {
            uint32_t GPTCNT         : 24;  /* [23:0]  r/o */
            uint32_t GPTMODE        :  1;  /* [24]    r/w */
            uint32_t RESERVED_29_25 :  5;  /* [29:25] rsvd */
            uint32_t GPTRST         :  1;  /* [30]    r/o */
            uint32_t GPTRUN         :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } GPTIMER1CTRL;

    /* 0x090: This register contains the control for the system bus interface */
    union {
        struct {
            uint32_t AHBBRST        :  3;  /* [2:0]   r/w */
            uint32_t RESERVED_31_3  : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SBUSCFG;

    uint8_t zReserved0x094[108];  /* pad 0x094 - 0x0ff */

    /* 0x100: indicate which offset to add to the register base address */
    union {
        struct {
            uint32_t CAPLENGTH      :  8;  /* [7:0]   r/o */
            uint32_t RESERVED_15_8  :  8;  /* [15:8]  rsvd */
            uint32_t HCIVERSION     : 16;  /* [31:16]  r/o */          
        } BF;
        uint32_t WORDVAL;
    } CAPLENGTH_HCIVERSION;

    /* 0x104: Port steering logic capabilities */
    union {
        struct {
            uint32_t N_PORTS        :  4;  /* [3:0]   r/o */
            uint32_t PPC            :  1;  /* [4]     r/o */
            uint32_t RESERVED_7_5   :  3;  /* [7:5]   rsvd */
            uint32_t N_PCC          :  4;  /* [11:8]  r/o */
            uint32_t N_CC           :  4;  /* [15:12] r/o */
            uint32_t PI             :  1;  /* [16]    r/o */
            uint32_t RESERVED_19_17 :  3;  /* [19:17] rsvd */
            uint32_t N_PTT          :  4;  /* [23:20] r/o */
            uint32_t N_TT           :  4;  /* [27:24] r/o */
            uint32_t RESERVED_31_28 :  4;  /* [31:28] rsvd */
        } BF;
        uint32_t WORDVAL;
    } HCSPARAMS;

    /* 0x108: identifies multiple mode control */
    union {
        struct {
            uint32_t ADC            :  1;  /* [0]     r/o */
            uint32_t PFL            :  1;  /* [1]     r/o */
            uint32_t ASP            :  1;  /* [2]     r/o */
            uint32_t RESERVED_3     :  1;  /* [3]     rsvd */
            uint32_t IST            :  4;  /* [7:4]   r/o */
            uint32_t EECP           :  8;  /* [15:8]  r/o */
            uint32_t RESERVED_31_16 : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } HCCPARAMS;

    uint8_t zReserved0x10c[20];  /* pad 0x10c - 0x11f */

    /* 0x120: */
    union {
        struct {
            uint32_t DCIVERSION     : 16;  /* [15:0]  r/o */
            uint32_t RESERVED_31_16 : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } DCIVERSION;

    /* 0x124: describe the overall host/device capability of the controller. */
    union {
        struct {
            uint32_t DEN            :  5;  /* [4:0]   r/o */
            uint32_t RESERVED_6_5   :  2;  /* [6:5]   rsvd */
            uint32_t DC             :  1;  /* [7]     r/o */
            uint32_t HC             :  1;  /* [8]     r/o */
            uint32_t RESERVED_31_9  : 23;  /* [31:9]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DCCPARAMS;

    uint8_t zReserved0x128[24];  /* pad 0x128 - 0x13f */

    /* 0x140: The serial bus host/device controller executes the command */
    /* indicated in this register. */
    union {
        struct {
            uint32_t RS             :  1;  /* [0]     r/w */
            uint32_t RST            :  1;  /* [1]     r/w */
            uint32_t FS0            :  1;  /* [2]     r/w */
            uint32_t FS1            :  1;  /* [3]     r/w */
            uint32_t PSE            :  1;  /* [4]     r/w */
            uint32_t ASE            :  1;  /* [5]     r/w */
            uint32_t IAA            :  1;  /* [6]     r/w */
            uint32_t LR             :  1;  /* [7]     r/o */
            uint32_t ASP0           :  1;  /* [8]     r/w */
            uint32_t ASP1           :  1;  /* [9]     r/w */
            uint32_t RESERVED_10    :  1;  /* [10]    rsvd */
            uint32_t ASPE           :  1;  /* [11]    r/w */
            uint32_t RESERVED_12    :  1;  /* [12]    rsvd */
            uint32_t SUTW           :  1;  /* [13]    r/w */
            uint32_t ATDTW          :  1;  /* [14]    r/w */
            uint32_t FS2            :  1;  /* [15]    r/w */
            uint32_t ITC            :  8;  /* [23:16] r/w */
            uint32_t RESERVED_31_24 :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USBCMD;

    /* 0x144: indicates various states of the Controller and any pending */
    /* interrupts */
    union {
        struct {
            uint32_t UI             :  1;  /* [0]     r/w */
            uint32_t UEI            :  1;  /* [1]     r/w */
            uint32_t PCI            :  1;  /* [2]     r/w */
            uint32_t FRI            :  1;  /* [3]     r/w */
            uint32_t SEI            :  1;  /* [4]     r/w */
            uint32_t AAI            :  1;  /* [5]     r/w */
            uint32_t URI            :  1;  /* [6]     r/w */
            uint32_t SRI            :  1;  /* [7]     r/w */
            uint32_t SLI            :  1;  /* [8]     r/w */
            uint32_t RESERVED_9     :  1;  /* [9]     rsvd */
            uint32_t ULPII          :  1;  /* [10]    r/w */
            uint32_t UALTI          :  1;  /* [11]    r/o */
            uint32_t HCH            :  1;  /* [12]    r/o */
            uint32_t RCL            :  1;  /* [13]    r/o */
            uint32_t PS             :  1;  /* [14]    r/o */
            uint32_t AS             :  1;  /* [15]    r/o */
            uint32_t NAKI           :  1;  /* [16]    r/o */
            uint32_t RESERVED_17    :  1;  /* [17]    rsvd */
            uint32_t UAI            :  1;  /* [18]    r/w */
            uint32_t UPI            :  1;  /* [19]    r/w */
            uint32_t RESERVED_23_20 :  4;  /* [23:20] rsvd */
            uint32_t TI0            :  1;  /* [24]    r/w */
            uint32_t TI1            :  1;  /* [25]    r/w */
            uint32_t RESERVED_31_26 :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USBSTS;

    /* 0x148: interrupt sources */
    union {
        struct {
            uint32_t UE             :  1;  /* [0]     r/w */
            uint32_t UEE            :  1;  /* [1]     r/w */
            uint32_t PCE            :  1;  /* [2]     r/w */
            uint32_t FRE            :  1;  /* [3]     r/w */
            uint32_t SEE            :  1;  /* [4]     r/w */
            uint32_t AAE            :  1;  /* [5]     r/w */
            uint32_t URE            :  1;  /* [6]     r/w */
            uint32_t SRE            :  1;  /* [7]     r/w */
            uint32_t SLE            :  1;  /* [8]     r/w */
            uint32_t RESERVED_9     :  1;  /* [9]     rsvd */
            uint32_t ULPIE          :  1;  /* [10]    r/w */
            uint32_t UALTIE         :  1;  /* [11]    r/w */
            uint32_t RESERVED_15_12 :  4;  /* [15:12] rsvd */
            uint32_t NAKE           :  1;  /* [16]    r/o */
            uint32_t RESERVED_17    :  1;  /* [17]    rsvd */
            uint32_t UAIE           :  1;  /* [18]    r/w */
            uint32_t UPIE           :  1;  /* [19]    r/w */
            uint32_t RESERVED_23_20 :  4;  /* [23:20] rsvd */
            uint32_t TIE0           :  1;  /* [24]    r/w */
            uint32_t TIE1           :  1;  /* [25]    r/w */
            uint32_t RESERVED_31_26 :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USBINTR;

    /* 0x14c: used by the host controller to index the periodic frame list */
    union {
        struct {
            uint32_t FRINDEX        : 14;  /* [13:0]  r/w */
            uint32_t RESERVED_31_14 : 18;  /* [31:14] rsvd */
        } BF;
        uint32_t WORDVAL;
    } FRINDEX;

    uint8_t zReserved0x150[4];  /* pad 0x150 - 0x153 */

    /* 0x154: host controller frame list base address */
    /* 0x154: device controller usb device address */
    union {
        struct {
            uint32_t RESERVED_11_0  : 12;  /* [11:0]  rsvd */
            uint32_t PERBASE        : 20;  /* [31:12] r/w */
        } BF_PERIODICLISTBASE_HOST;
        struct {
            uint32_t RESERVED_23_0  : 24;  /* [23:0]  rsvd */
            uint32_t USBADRA        :  1;  /* [24]    r/w */
            uint32_t USBADR         :  7;  /* [31:25] r/w */
        } BF_PERIODICLISTBASE_DEVICE;
        uint32_t WORDVAL;
    } HOST_PERIODICLISTBASE_DEVICE;

    /* 0x158: contains the address of the top of the endpoint list in system */
    /* memory */
    /* 0x158: contains the address of the top of the endpoint list in system */
    /* memory */
    union {
        struct {
            uint32_t RESERVED_4_0   :  5;  /* [4:0]   rsvd */
            uint32_t ASYBASE        : 27;  /* [31:5]  r/w */
        } BF_ASYNCLISTADDR_HOST;
        struct {
            uint32_t RESERVED_10_0  : 11;  /* [10:0]  rsvd */
            uint32_t EPBASE         : 21;  /* [31:11] r/w */
        } BF_ASYNCLISTADDR_DEVICE;
        uint32_t WORDVAL;
    } HOST_ASYNCLISTADDR_DEVICE;

    /* 0x15c: contains parameters needed for internal TT operations. */
    union {
        struct {
            uint32_t TTAS           :  1;  /* [0]     r/o */
            uint32_t TTAC           :  1;  /* [1]     r/w */
            uint32_t RESERVED_23_2  : 22;  /* [23:2]  rsvd */
            uint32_t TTHA           :  7;  /* [30:24] r/w */
            uint32_t RESERVED_31    :  1;  /* [31]    rsvd */
        } BF;
        uint32_t WORDVAL;
    } TTCTRL;

    /* 0x160: controls the burst size used during data movement */
    union {
        struct {
            uint32_t RXPBURST       :  8;  /* [7:0]   r/w */
            uint32_t TXPBURST       :  8;  /* [15:8]  r/w */
            uint32_t RESERVED_31_16 : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } BURSTSIZE;

    /* 0x164: */
    union {
        struct {
            uint32_t TXSCHOH        :  7;  /* [6:0]   r/w */
            uint32_t RESERVED_7     :  1;  /* [7]     rsvd */
            uint32_t TXSCHHEALTH    :  5;  /* [12:8]  r/w */
            uint32_t RESERVED_15_13 :  3;  /* [15:13] rsvd */
            uint32_t TXFIFOTHRES    :  6;  /* [21:16] r/w */
            uint32_t RESERVED_31_22 : 10;  /* [31:22] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TXFILLTUNING;

    /* 0x168: */
    union {
        struct {
            uint32_t TXTTSCHOH      :  5;  /* [4:0]   r/w */
            uint32_t RESERVED_7_5   :  3;  /* [7:5]   rsvd */
            uint32_t TXTTSCHHEALTJ  :  5;  /* [12:8]  r/w */
            uint32_t RESERVED_31_13 : 19;  /* [31:13] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TXTTFILLTUNING;

    /* 0x16c: enable and controls the IC_USB FS/LS transceiver. */
    union {
        struct {
            uint32_t IC_VDD1        :  3;  /* [2:0]   r/w */
            uint32_t IC1            :  1;  /* [3]     r/w */
            uint32_t IC_VDD2        :  3;  /* [6:4]   r/w */
            uint32_t IC2            :  1;  /* [7]     r/w */
            uint32_t IC_VDD3        :  3;  /* [10:8]  r/w */
            uint32_t IC3            :  1;  /* [11]    r/w */
            uint32_t IC_VDD4        :  3;  /* [14:12] r/w */
            uint32_t IC4            :  1;  /* [15]    r/w */
            uint32_t IC_VDD5        :  3;  /* [18:16] r/w */
            uint32_t IC5            :  1;  /* [19]    r/w */
            uint32_t IC_VDD6        :  3;  /* [22:20] r/w */
            uint32_t IC6            :  1;  /* [23]    r/w */
            uint32_t IC_VDD7        :  3;  /* [26:24] r/w */
            uint32_t IC7            :  1;  /* [27]    r/w */
            uint32_t IC_VDD8        :  3;  /* [30:28] r/w */
            uint32_t IC8            :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } IC_USB;

    /* 0x170: provides indirect access to the ULPI PHY register set. */
    union {
        struct {
            uint32_t ULPIDATWR      :  8;  /* [7:0]   r/w */
            uint32_t ULPIDATRD      :  8;  /* [15:8]  r/o */
            uint32_t ULPIADDR       :  8;  /* [23:16] r/w */
            uint32_t ULPIPORT       :  3;  /* [26:24] r/w */
            uint32_t ULPISS         :  1;  /* [27]    r/w */
            uint32_t RESERVED_28    :  1;  /* [28]    rsvd */
            uint32_t ULPIRW         :  1;  /* [29]    r/w */
            uint32_t ULPIRUN        :  1;  /* [30]    r/w */
            uint32_t ULPIWU         :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } ULPI_VIEWPORT;

    uint8_t zReserved0x174[4];  /* pad 0x174 - 0x177 */

    /* 0x178: */
    union {
        struct {
            uint32_t EPRN           : 16;  /* [15:0]  r/w */
            uint32_t EPTN           : 16;  /* [31:16] r/w */
        } BF;
        uint32_t WORDVAL;
    } ENDPTNAK;

    /* 0x17c: */
    union {
        struct {
            uint32_t EPRNE                : 16;  /* [15:0]  r/w */
            uint32_t EPTNE                : 16;  /* [31:16] r/w */
        } BF;
        uint32_t WORDVAL;
    } ENDPTNAKEN;

    uint8_t zReserved0x180[4];  /* pad 0x180 - 0x183 */

    /* 0x184: */
    union {
        struct {
            uint32_t CCS            :  1;  /* [0]     r/o */
            uint32_t CSC            :  1;  /* [1]     r/w */
            uint32_t PE             :  1;  /* [2]     r/w */
            uint32_t PEC            :  1;  /* [3]     r/w */
            uint32_t OCA            :  1;  /* [4]     r/o */
            uint32_t OCC            :  1;  /* [5]     r/w */
            uint32_t FPR            :  1;  /* [6]     r/w */
            uint32_t SUSP           :  1;  /* [7]     r/w */
            uint32_t PR             :  1;  /* [8]     r/w */
            uint32_t HSP            :  1;  /* [9]     r/o */
            uint32_t LS             :  2;  /* [11:10] r/o */
            uint32_t PP             :  1;  /* [12]    r/w */
            uint32_t PO             :  1;  /* [13]    r/o */
            uint32_t PIC            :  2;  /* [15:14] r/w */
            uint32_t PTC            :  4;  /* [19:16] r/w */
            uint32_t WKCN           :  1;  /* [20]    r/w */
            uint32_t WKDS           :  1;  /* [21]    r/w */
            uint32_t WKOC           :  1;  /* [22]    r/w */
            uint32_t PHCD           :  1;  /* [23]    r/w */
            uint32_t PFSC           :  1;  /* [24]    r/w */
            uint32_t PTS2           :  1;  /* [25]    r/w */
            uint32_t PSPD           :  2;  /* [27:26] r/o */
            uint32_t PTW            :  1;  /* [28]    r/w */
            uint32_t STS            :  1;  /* [29]    r/w */
            uint32_t PTS            :  2;  /* [31:30] r/w */
        } BF;
        uint32_t WORDVAL;
    } PORTSC1;

    uint8_t zReserved0x188[28];  /* pad 0x188 - 0x1a3 */

    /* 0x1a4: This register only exists in a OTG implementation. */
    union {
        struct {
            uint32_t VD             :  1;  /* [0]     r/w */
            uint32_t VC             :  1;  /* [1]     r/w */
            uint32_t HAAR           :  1;  /* [2]     r/w */
            uint32_t OT             :  1;  /* [3]     r/w */
            uint32_t DP             :  1;  /* [4]     r/w */
            uint32_t IDPU           :  1;  /* [5]     r/w */
            uint32_t HADP           :  1;  /* [6]     r/w */
            uint32_t HABA           :  1;  /* [7]     r/w */
            uint32_t ID             :  1;  /* [8]     r/o */
            uint32_t AVV            :  1;  /* [9]     r/o */
            uint32_t ASV            :  1;  /* [10]    r/o */
            uint32_t BSV            :  1;  /* [11]    r/o */
            uint32_t BSE            :  1;  /* [12]    r/o */
            uint32_t _1MST          :  1;  /* [13]    r/o */
            uint32_t DPS            :  1;  /* [14]    r/o */
            uint32_t RESERVED_15    :  1;  /* [15]    rsvd */
            uint32_t IDIS           :  1;  /* [16]    r/w */
            uint32_t AVVIS          :  1;  /* [17]    r/w */
            uint32_t ASVIS          :  1;  /* [18]    r/w */
            uint32_t BSVIS          :  1;  /* [19]    r/w */
            uint32_t BSEIS          :  1;  /* [20]    r/w */
            uint32_t _1MSS          :  1;  /* [21]    r/w */
            uint32_t DPIS           :  1;  /* [22]    r/w */
            uint32_t RESERVED_23    :  1;  /* [23]    rsvd */
            uint32_t IDIE           :  1;  /* [24]    r/w */
            uint32_t AVVIE          :  1;  /* [25]    r/w */
            uint32_t ASVIE          :  1;  /* [26]    r/w */
            uint32_t BSVIE          :  1;  /* [27]    r/w */
            uint32_t BSEIE          :  1;  /* [28]    r/w */
            uint32_t _1MSE          :  1;  /* [29]    r/w */
            uint32_t DPIE           :  1;  /* [30]    r/w */
            uint32_t RESERVED_31    :  1;  /* [31]    rsvd */
        } BF;
        uint32_t WORDVAL;
    } OTGSC;

    /* 0x1a8: */
    union {
        struct {
            uint32_t CM             :  2;  /* [1:0]   r/w */
            uint32_t ES             :  1;  /* [2]     r/w */
            uint32_t SLOM           :  1;  /* [3]     r/w */
            uint32_t SDIS           :  1;  /* [4]     r/w */
            uint32_t VBPS           :  1;  /* [5]     r/w */
            uint32_t RESERVED_14_6  :  9;  /* [14:6]  rsvd */
            uint32_t SRT            :  1;  /* [15]    r/w */
            uint32_t RESERVED_31_16 : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USBMODE;

    /* 0x1ac: Setup Endpoint Status */
    union {
        struct {
            uint32_t ENDPTSETUPSTAT : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16 : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ENDPTSETUPSTAT;

    /* 0x1b0: */
    union {
        struct {
            uint32_t PERB           : 16;  /* [15:0]  r/w */
            uint32_t PETB           : 16;  /* [31:16] r/w */
        } BF;
        uint32_t WORDVAL;
    } ENDPTPRIME;

    /* 0x1b4: */
    union {
        struct {
            uint32_t FERB           : 16;  /* [15:0]  r/w */
            uint32_t FETB           : 16;  /* [31:16] r/w */
        } BF;
        uint32_t WORDVAL;
    } ENDPTFLUSH;

    /* 0x1b8: */
    union {
        struct {
            uint32_t ERBR           : 16;  /* [15:0]  r/o */
            uint32_t ETBR           : 16;  /* [31:16] r/o */
        } BF;
        uint32_t WORDVAL;
    } ENDPTSTAT;

    /* 0x1bc: */
    union {
        struct {
            uint32_t ERCE           : 16;  /* [15:0]  r/w */
            uint32_t ETCE           : 16;  /* [31:16] r/w */
        } BF;
        uint32_t WORDVAL;
    } ENDPTCOMPLETE;

    /* 0x1c0: Every device will implement Endpoint0 as a control endpoint. */
    union {
        struct {
            uint32_t RXS            :  1;  /* [0]     r/w */
            uint32_t RESERVED_1     :  1;  /* [1]     rsvd */
            uint32_t RXT            :  2;  /* [3:2]   r/o */
            uint32_t RESERVED_6_4   :  3;  /* [6:4]   rsvd */
            uint32_t RXE            :  1;  /* [7]     r/o */
            uint32_t RESERVED_15_8  :  8;  /* [15:8]  rsvd */
            uint32_t TXS            :  1;  /* [16]    r/w */
            uint32_t RESERVED_17    :  1;  /* [17]    rsvd */
            uint32_t TXT            :  2;  /* [19:18] r/o */
            uint32_t RESERVED_22_20 :  3;  /* [22:20] rsvd */
            uint32_t TXE            :  1;  /* [23]    r/o */
            uint32_t RESERVED_31_24 :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ENDPTCTRL0;

    /* 0x1c4--0x1e0: */
    union {
        struct {
            uint32_t RXS            :  1;  /* [0]     r/w */
            uint32_t RXD            :  1;  /* [1]     r/w */
            uint32_t RXT            :  2;  /* [3:2]   r/w */
            uint32_t RESERVED_4     :  1;  /* [4]     rsvd */
            uint32_t RXI            :  1;  /* [5]     r/w */
            uint32_t RXR            :  1;  /* [6]     r/w */
            uint32_t RXE            :  1;  /* [7]     r/w */
            uint32_t RESERVED_15_8  :  8;  /* [15:8]  rsvd */
            uint32_t TXS            :  1;  /* [16]    r/w */
            uint32_t TXD            :  1;  /* [17]    r/w */
            uint32_t TXT            :  2;  /* [19:18] r/w */
            uint32_t RESERVED_20    :  1;  /* [20]    rsvd */
            uint32_t TXI            :  1;  /* [21]    r/w */
            uint32_t TXR            :  1;  /* [22]    r/w */
            uint32_t TXE            :  1;  /* [23]    r/w */
            uint32_t RESERVED_31_24 :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ENDPTCTRL[7];

    uint8_t zReserved0x1e0[32];  /* pad 0x1e0 - 0x1ff */

    /* 0x200: */
    union {
        struct {
            uint32_t CID0                 :  8;  /* [7:0]   r/o */
            uint32_t CID1                 :  8;  /* [15:8]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PHY_ID;

    /* 0x204: */
    union {
        struct {
            uint32_t FBDIV                :  9;  /* [8:0]   r/w */
            uint32_t REFDIV               :  5;  /* [13:9]  r/w */
            uint32_t PLLVDD18             :  2;  /* [15:14] r/w */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PLL_CONTROL_0;

    /* 0x208: */
    union {
        struct {
            uint32_t PLLCAL12             :  2;  /* [1:0]   r/w */
            uint32_t VCOCAL_START         :  1;  /* [2]     r/w */
            uint32_t CLK_BLK_EN           :  1;  /* [3]     r/w */
            uint32_t KVCO                 :  3;  /* [6:4]   r/w */
            uint32_t KVCO_EXT             :  1;  /* [7]     r/w */
            uint32_t ICP                  :  3;  /* [10:8]  r/w */
            uint32_t DLL_RESET_BLK        :  1;  /* [11]    r/w */
            uint32_t PLL_LOCK_BYPASS      :  1;  /* [12]    r/w */
            uint32_t PU_PLL               :  1;  /* [13]    r/w */
            uint32_t PLL_CONTRL_BY_PIN    :  1;  /* [14]    r/w */
            uint32_t PLL_READY            :  1;  /* [15]    r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PLL_CONTROL_1;

    /* 0x20c: */
    union {
        struct {
            uint32_t RESERVED_BIT_15_0    : 16;  /* [15:0]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RESERVED_ADDR3;

    /* 0x210: */
    union {
        struct {
            uint32_t EXT_FS_RCAL          :  4;  /* [3:0]   r/w */
            uint32_t EXT_HS_RCAL          :  4;  /* [7:4]   r/w */
            uint32_t IMPCAL_VTH           :  3;  /* [10:8]  r/w */
            uint32_t EXT_FS_RCAL_EN       :  1;  /* [11]    r/w */
            uint32_t EXT_HS_RCAL_EN       :  1;  /* [12]    r/w */
            uint32_t RCAL_START           :  1;  /* [13]    r/w */
            uint32_t TXDATA_BLOCK_EN      :  1;  /* [14]    r/w */
            uint32_t ND                   :  1;  /* [15]    r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TX_CHANNEL_CONTRL_0;

    /* 0x214: */
    union {
        struct {
            uint32_t CK60_PHSEL           :  4;  /* [3:0]   r/w */
            uint32_t AMP                  :  3;  /* [6:4]   r/w */
            uint32_t LOWVDD_EN            :  1;  /* [7]     r/w */
            uint32_t TXVDD12              :  2;  /* [9:8]   r/w */
            uint32_t TXVDD15              :  2;  /* [11:10] r/w */
            uint32_t ND                   :  4;  /* [15:12] r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TX_CHANNEL_CONTRL_1;

    /* 0x218: */
    union {
        struct {
            uint32_t HSDRV_EN             :  4;  /* [3:0]   r/w */
            uint32_t FSDRV_EN             :  4;  /* [7:4]   r/w */
            uint32_t IMP_CAL_DLY          :  2;  /* [9:8]   r/w */
            uint32_t DRV_SLEWRATE         :  2;  /* [11:10] r/w */
            uint32_t ND                   :  4;  /* [15:12] r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TX_CHANNEL_CONTRL_2;

    /* 0x21c: */
    union {
        struct {
            uint32_t RESERVED_BIT_15_0    : 16;  /* [15:0]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RESERVED_ADDR7;

    /* 0x220: */
    union {
        struct {
            uint32_t INTPI                :  2;  /* [1:0]   r/w */
            uint32_t LPF_COEF             :  2;  /* [3:2]   r/w */
            uint32_t SQ_THRESH            :  4;  /* [7:4]   r/w */
            uint32_t DISCON_THRESH        :  2;  /* [9:8]   r/w */
            uint32_t SQ_LENGTH            :  2;  /* [11:10] r/w */
            uint32_t ACQ_LENGTH           :  2;  /* [13:12] r/w */
            uint32_t USQ_LENGTH           :  1;  /* [14]    r/w */
            uint32_t PHASE_FREEZE_DLY     :  1;  /* [15]    r/w */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RX_CHANNEL_CONTRL_0;

    /* 0x224: */
    union {
        struct {
            uint32_t S2TO3_DLY_SEL        :  2;  /* [1:0]   r/w */
            uint32_t CDR_FASTLOCK_EN      :  1;  /* [2]     r/w */
            uint32_t CDR_COEF_SEL         :  1;  /* [3]     r/w */
            uint32_t EDGE_DET_SEL         :  2;  /* [5:4]   r/w */
            uint32_t RXDATA_BLOCK_LENGTH  :  2;  /* [7:6]   r/w */
            uint32_t CAP_SEL              :  3;  /* [10:8]  r/w */
            uint32_t EDGE_DET_EN          :  1;  /* [11]    r/w */
            uint32_t RXDATA_BLOCK_EN      :  1;  /* [12]    r/w */
            uint32_t EARLY_VOS_ON_EN      :  1;  /* [13]    r/w */
            uint32_t ND                   :  2;  /* [15:14] r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RX_CHANNEL_CONTRL_1;

    /* 0x228: */
    union {
        struct {
            uint32_t RXVDD12              :  2;  /* [1:0]   r/w */
            uint32_t RXVDD18              :  2;  /* [3:2]   r/w */
            uint32_t SQ_ALWAYS_ON         :  1;  /* [4]     r/w */
            uint32_t SQ_BUFFER_EN         :  1;  /* [5]     r/w */
            uint32_t SAMPLER_CTRL         :  1;  /* [6]     r/w */
            uint32_t SQ_CM_SEL            :  1;  /* [7]     r/w */
            uint32_t USQ_FILTER           :  1;  /* [8]     r/w */
            uint32_t ND                   :  7;  /* [15:9]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RX_CHANNEL_CONTRL_2;

    uint8_t zReserved0x22c[4];  /* pad 0x22c - 0x22f */

    /* 0x230: */
    union {
        struct {
            uint32_t IPTAT_SEL            :  3;  /* [2:0]   r/w */
            uint32_t VDD_USB2_DIG_TOP_SEL :  1;  /* [3]     r/w */
            uint32_t TOPVDD18             :  2;  /* [5:4]   r/w */
            uint32_t DIG_SEL              :  2;  /* [7:6]   r/w */
            uint32_t BG_VSEL              :  2;  /* [9:8]   r/w */
            uint32_t ND                   :  6;  /* [15:10] r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ANA_CONTRL_0;

    /* 0x234: */
    union {
        struct {
            uint32_t TESTMON_ANA          :  6;  /* [5:0]   r/w */
            uint32_t STRESS_TEST_MODE     :  1;  /* [6]     r/w */
            uint32_t R_ROTATE_SEL         :  1;  /* [7]     r/w */
            uint32_t V2I                  :  3;  /* [10:8]  r/w */
            uint32_t V2I_EXT              :  1;  /* [11]    r/w */
            uint32_t SEL_LPFR             :  1;  /* [12]    r/w */
            uint32_t ANA_CONTRL_BY_PIN    :  1;  /* [13]    r/w */
            uint32_t PU_ANA               :  1;  /* [14]    r/w */
            uint32_t ND                   :  1;  /* [15]    r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ANA_CONTRL_1;

    /* 0x238: */
    union {
        struct {
            uint32_t RESERVED_BIT_15_0    : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RESERVED_ADDR_C;

    /* 0x23c: */
    union {
        struct {
            uint32_t FIFO_FILL_NUM        :  4;  /* [3:0]   r/w */
            uint32_t SYNC_NUM             :  2;  /* [5:4]   r/w */
            uint32_t HS_DRIBBLE_EN        :  1;  /* [6]     r/w */
            uint32_t CLK_SUSPEND_EN       :  1;  /* [7]     r/w */
            uint32_t SYNCDET_WINDOW_EN    :  1;  /* [8]     r/w */
            uint32_t EARLY_TX_EN          :  1;  /* [9]     r/w */
            uint32_t FORCE_END_EN         :  1;  /* [10]    r/w */
            uint32_t HOST_DISCON_SEL0     :  1;  /* [11]    r/w */
            uint32_t HOST_DISCON_SEL1     :  1;  /* [12]    r/w */
            uint32_t FS_EOP_MODE          :  1;  /* [13]    r/w */
            uint32_t FIFO_OV              :  1;  /* [14]    r/o */
            uint32_t FIFO_UF              :  1;  /* [15]    r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } DIGITAL_CONTROL_0;

    /* 0x240: */
    union {
        struct {
            uint32_t MON_SEL              :  6;  /* [5:0]   r/w */
            uint32_t SQ_RST_RX            :  1;  /* [6]     r/w */
            uint32_t SYNC_IGNORE_SQ       :  1;  /* [7]     r/w */
            uint32_t DM_PULLDOWN          :  1;  /* [8]     r/w */
            uint32_t DP_PULLDOWN          :  1;  /* [9]     r/w */
            uint32_t ARC_DPDM_MODE        :  1;  /* [10]    r/w */
            uint32_t EXT_TX_CLK_SEL       :  1;  /* [11]    r/w */
            uint32_t CLK_OUT_SEL          :  1;  /* [12]    r/w */
            uint32_t FS_RX_ERROR_MODE     :  1;  /* [13]    r/w */
            uint32_t FS_RX_ERROR_MODE1    :  1;  /* [14]    r/w */
            uint32_t FS_RX_ERROR_MODE2    :  1;  /* [15]    r/w */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } DIGITAL_CONTROL_1;

    /* 0x244: */
    union {
        struct {
            uint32_t FS_HDL_OPMD          :  1;  /* [0]     r/w */
            uint32_t HS_HDL_SYNC          :  1;  /* [1]     r/w */
            uint32_t ALIGN_FS_OUTEN       :  1;  /* [2]     r/w */
            uint32_t DISABLE_EL16         :  1;  /* [3]     r/w */
            uint32_t NOVBUS_DPDM00        :  1;  /* [4]     r/w */
            uint32_t LONG_EOP             :  1;  /* [5]     r/w */
            uint32_t ND                   :  2;  /* [7:6]   r/o */
            uint32_t PAD_STRENGTH         :  5;  /* [12:8]  r/w */
            uint32_t ND_15_13             :  3;  /* [15:13] r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } DIGITAL_CONTROL_2;

    /* 0x248: */
    union {
        struct {
            uint32_t RESERVED_BIT_15_0    : 16;  /* [15:0]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RESERVED_ADDR_12H;

    /* 0x24c: */
    union {
        struct {
            uint32_t TEST_TX_PATTERN      :  8;  /* [7:0]   r/w */
            uint32_t TEST_MODE_2_0        :  3;  /* [10:8]  r/w */
            uint32_t TEST_BYPASS          :  1;  /* [11]    r/w */
            uint32_t TEST_LENGTH_1_0      :  2;  /* [13:12] r/w */
            uint32_t TEST_ANA_LPBK        :  1;  /* [14]    r/w */
            uint32_t TEST_DIG_LPBK        :  1;  /* [15]    r/w */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TEST_CONTRL_AND_STATUS_0;

    /* 0x250: */
    union {
        struct {
            uint32_t TEST_XCVR_SELECT     :  2;  /* [1:0]   r/w */
            uint32_t TEST_OP_MODE         :  2;  /* [3:2]   r/w */
            uint32_t TEST_TERM_SELECT     :  1;  /* [4]     r/w */
            uint32_t TEST_TX_BITSTUFF_EN  :  1;  /* [5]     r/w */
            uint32_t TEST_SUSPENDM        :  1;  /* [6]     r/w */
            uint32_t TEST_UTMI_SEL        :  1;  /* [7]     r/w */
            uint32_t TEST_SKIP_2_0        :  3;  /* [10:8]  r/w */
            uint32_t ND                   :  1;  /* [11]    r/w */
            uint32_t TEST_RESET           :  1;  /* [12]    r/w */
            uint32_t TEST_EN              :  1;  /* [13]    r/w */
            uint32_t TEST_FLAG            :  1;  /* [14]    r/o */
            uint32_t TEST_DONE            :  1;  /* [15]    r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TEST_CONTRL_AND_STATUS_1;

    /* 0x254: */
    union {
        struct {
            uint32_t RESERVED_BIT_15_0    : 16;  /* [15:0]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RESERVED_ADDR_15H;

    /* 0x258: */
    union {
        struct {
            uint32_t TESTMON_CHRGDTC      :  2;  /* [1:0]   r/w */
            uint32_t PU_CHRG_DTC          :  1;  /* [2]     r/w */
            uint32_t ENABLE_SWITCH        :  1;  /* [3]     r/w */
            uint32_t ND                   : 12;  /* [15:4]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PHY_REG_CHGDTC_CONTRL;

    /* 0x25c: */
    union {
        struct {
            uint32_t TESTMON_OTG_2_0      :  3;  /* [2:0]   r/w */
            uint32_t PU_OTG               :  1;  /* [3]     r/w */
            uint32_t OTG_CONTROL_BY_PIN   :  1;  /* [4]     r/w */
            uint32_t ND                   : 11;  /* [15:5]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PHY_REG_OTG_CONTROL;

    /* 0x260: */
    union {
        struct {
            uint32_t PHY_MON              : 16;  /* [15:0]  r/w */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USB2_PHY_MON0;

    /* 0x264: */
    union {
        struct {
            uint32_t RESERVED_0           :  2;  /* [1:0]   r/o */
            uint32_t CDP_EN               :  1;  /* [2]     r/w */
            uint32_t DCP_EN               :  1;  /* [3]     r/w */
            uint32_t PD_EN                :  1;  /* [4]     r/w */
            uint32_t CDP_DM_AUTO_SWITCH   :  1;  /* [5]     r/w */
            uint32_t ENABLE_SWITCH_DM     :  1;  /* [6]     r/w */
            uint32_t ENABLE_SWITCH_DP     :  1;  /* [7]     r/w */
            uint32_t VDAT_CHARGE          :  2;  /* [9:8]   r/w */
            uint32_t VSRC_CHARGE          :  2;  /* [11:10] r/w */
            uint32_t PLLVDD12             :  2;  /* [13:12] r/w */
            uint32_t RESERVED_14          :  1;  /* [14]    r/o */
            uint32_t DP_DM_SWAP_CTRL      :  1;  /* [15]    r/w */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PHY_REG_CHGDTC_CONTRL_1;

    /* 0x268: */
    union {
        struct {
            uint32_t RESERVED_BIT_15_0    : 16;  /* [15:0]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RESERVED_ADDR_1AH;

    /* 0x26c: */
    union {
        struct {
            uint32_t RESERVED_BIT_15_0    : 16;  /* [15:0]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RESERVED_ADDR_1BH;

    /* 0x270: */
    union {
        struct {
            uint32_t RESERVED_BIT_15_0    : 16;  /* [15:0]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RESERVED_ADDR_1CH;

    /* 0x274: */
    union {
        struct {
            uint32_t RESERVED_BIT_15_0    : 16;  /* [15:0]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RESERVED_ADDR_1DH;

    /* 0x278: */
    union {
        struct {
            uint32_t ICID1                :  8;  /* [7:0]   r/o */
            uint32_t ICID0                :  8;  /* [15:8]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INTERNAL_CID;

    /* 0x27c: */
    union {
        struct {
            uint32_t PHY_MULTIPORT        :  1;  /* [0]     r/o */
            uint32_t ND                   :  2;  /* [2:1]   r/o */
            uint32_t DIG_REGULATOR        :  1;  /* [3]     r/o */
            uint32_t PHY_ULPI             :  1;  /* [4]     r/o */
            uint32_t PHY_HSIC             :  1;  /* [5]     r/o */
            uint32_t PHY_CHG_DTC          :  1;  /* [6]     r/o */
            uint32_t PHY_OTG              :  1;  /* [7]     r/o */
            uint32_t ICID2                :  8;  /* [15:8]  r/o */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USB2_ICID_REG1;

};

typedef volatile struct usbc_reg usbc_reg_t;

#ifdef USBC_IMPL
BEGIN_REG_SECTION(usbc_registers)
usbc_reg_t USBCREG;
END_REG_SECTION(usbc_registers)
#else
extern usbc_reg_t USBCREG;
#endif

#endif /* _USBC_REG_H */

