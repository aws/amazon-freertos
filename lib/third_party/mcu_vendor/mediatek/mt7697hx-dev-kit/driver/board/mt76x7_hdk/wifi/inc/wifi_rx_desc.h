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

#ifndef __WIFI_RX_DESC__
#define __WIFI_RX_DESC__

typedef enum _ENUM_MAC_RX_GROUP_VLD_T {
    RX_GROUP_VLD_1 = 0,
    RX_GROUP_VLD_2,
    RX_GROUP_VLD_3,
    RX_GROUP_VLD_4,
    RX_GROUP_VLD_NUM
} ENUM_MAC_RX_GROUP_VLD_T;

/* Word 1 */
#define RX_STATUS_GROUP_VLD_MASK        BITS(9,12)
#define RX_STATUS_GROUP_VLD_OFFSET      9
#define RX_STATUS_PKT_TYPE_MASK         BITS(13,15)
#define RX_STATUS_PKT_TYPE_OFFSET       13

/* Byte 2 */
#define RX_STATUS_HEADER_LEN_MASK           BITS(0,5)
#define RX_STATUS_HEADER_OFFSET             BIT(6)
#define RX_STATUS_HEADER_TRAN               BIT(7)

/* RX Vector, 3rd Cycle */
#define RX_VT_RCPI0_MASK       BITS(8,15)
#define RX_VT_RCPI0_OFFSET     8

/* DW 0 */
#define HAL_RX_STATUS_GET_RX_BYTE_CNT(_prHwMacRxDesc) ((_prHwMacRxDesc)->u2RxByteCount)
#define HAL_RX_STATUS_GET_GROUP_VLD(_prHwMacRxDesc) (((_prHwMacRxDesc)->u2PktTYpe & RX_STATUS_GROUP_VLD_MASK) >> RX_STATUS_GROUP_VLD_OFFSET)
#define HAL_RX_STATUS_GET_PKT_TYPE(_prHwMacRxDesc) (((_prHwMacRxDesc)->u2PktTYpe & RX_STATUS_PKT_TYPE_MASK) >> RX_STATUS_PKT_TYPE_OFFSET)

/* DW 1 */
#define HAL_RX_STATUS_GET_HEADER_LEN(_prHwMacRxDesc) ((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_LEN_MASK)
#define HAL_RX_STATUS_IS_HEADER_OFFSET(_prHwMacRxDesc) (((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_OFFSET)?TRUE:FALSE)
#define HAL_RX_STATUS_GET_HEADER_OFFSET(_prHwMacRxDesc) (((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_OFFSET)? 2 : 0)

/* DW 14~19 */
#define HAL_RX_STATUS_GET_RCPI(_prHwMacRxStsGroup3) (((_prHwMacRxStsGroup3)->u4RxVector[2] & RX_VT_RCPI0_MASK) >> RX_VT_RCPI0_OFFSET)

typedef struct _HW_MAC_RX_DESC_T {
    uint16_t    u2RxByteCount;       /* DW 0 */
    uint16_t    u2PktTYpe;
    uint8_t     ucMatchPacket;       /* DW 1 */
    uint8_t     ucChanFreq;
    uint8_t     ucHeaderLen;
    uint8_t     ucBssid;
    uint8_t     ucWlanIdx;           /* DW 2 */
    uint8_t     ucTidSecMode;
    uint16_t    u2StatusFlag;
    uint32_t    u4PatternFilterInfo; /* DW 3 */
}  HW_MAC_RX_DESC_T, *P_HW_MAC_RX_DESC_T;
typedef struct _HW_MAC_RX_STS_GROUP_1_T {
    uint8_t     aucPN[16];
} HW_MAC_RX_STS_GROUP_1_T, *P_HW_MAC_RX_STS_GROUP_1_T;
typedef struct _HW_MAC_RX_STS_GROUP_2_T {
    uint32_t    u4Timestamp;      /* DW 12 */
    uint32_t    u4CRC;            /* DW 13 */
} HW_MAC_RX_STS_GROUP_2_T, *P_HW_MAC_RX_STS_GROUP_2_T;
typedef struct _HW_MAC_RX_STS_GROUP_4_T {
    /* For HDR_TRAN */
    uint16_t    u2FrameCtl;          /* DW 4 */
    uint8_t     aucTA[6];            /* DW 4~5 */
    uint16_t    u2SeqFrag;           /* DW 6 */
    uint16_t    u2Qos;               /* DW 6 */
    uint32_t    u4HTC;               /* DW 7 */
} HW_MAC_RX_STS_GROUP_4_T, *P_HW_MAC_RX_STS_GROUP_4_T;
typedef struct _HW_MAC_RX_STS_GROUP_3_T {
    /*!  RX Vector Info*/
    uint32_t    u4RxVector[6];       /* DW 14~19 */
} HW_MAC_RX_STS_GROUP_3_T, *P_HW_MAC_RX_STS_GROUP_3_T;
/* Used for MAC RX */
typedef enum _ENUM_MAC_RX_PKT_TYPE_T {
    RX_PKT_TYPE_TX_STATUS = 0,
    RX_PKT_TYPE_RX_VECTOR,
    RX_PKT_TYPE_RX_DATA,
    RX_PKT_TYPE_DUP_RFB,
    RX_PKT_TYPE_TM_REPORT,
    RX_PKT_TYPE_SW_DEFINED = 7
} ENUM_MAC_RX_PKT_TYPE_T;

#endif

