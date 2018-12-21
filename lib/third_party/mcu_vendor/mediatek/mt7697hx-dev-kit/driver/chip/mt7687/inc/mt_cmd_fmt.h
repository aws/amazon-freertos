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

#ifndef __MT_CMD_FMT_H__
#define __MT_CMD_FMT_H__

#define PQ_TO_PDA               0xC000
#define P1_Q0                   0x8000
#define P1_Q1                   0x8800
#define PKT_ID_CMD              0xA0
#define PKT_ID_EVENT            0xE000

/*
 * Command type table
 */
enum MT_CMD_TYPE {
    MT_TARGET_ADDRESS_LEN_REQ = 0x01,
    MT_FW_START_REQ = 0x02,
    INIT_CMD_ACCESS_REG = 0x3,
    MT_PATCH_START_REQ = 0x05,
    MT_PATCH_FINISH_REQ = 0x07,
    MT_PATCH_SEM_CONTROL = 0x10,
    MT_HIF_LOOPBACK = 0x20,
    CMD_CH_PRIVILEGE = 0x20,
    CMD_ACCESS_REG = 0xC2,
    EXT_CID = 0xED,
    MT_FW_SCATTER = 0xEE,
    MT_RESTART_DL_REQ = 0xEF,
};

#define EXT_EVENT_ID_ASSERT_DUMP                    0x23


//================== Inband Command ====================================================//
typedef struct _INIT_WIFI_CMD_T {
    UINT8      ucCID;
    UINT8      ucPktTypeID;    /* Must be 0xA0 (CMD Packet) */
    UINT8      ucReserved;
    UINT8      ucSeqNum;
//    UINT32     u4Reserved;     /* add one DW to compatible with normal TXD format. */
// ----------Ext Cmd ----------------
    UINT8      ucPacketOffset;     /* for N9 tx zero copy */
    UINT8      ucExtenCID;         /* Extend CID */
    UINT8      ucD2B2Rev;          /* padding fields, hw may auto modify this field */
    UINT8      ucExtCmdOption;     /* Extend CID option */
// ----------Ext Cmd ----------------
    UINT8      aucBuffer[];        /* for KEIL porting, don't use zero-length [0] */
} INIT_WIFI_CMD_T, *P_INIT_WIFI_CMD_T;

typedef struct _INIT_HIF_TX_HEADER_T { /* size=12 bytes  */
    UINT16     u2TxByteCount;
    UINT16     u2PQ_ID;        /* Must be 0x8000 (Port1, Queue 0) */
    INIT_WIFI_CMD_T rInitWifiCmd;
} INIT_HIF_TX_HEADER_T, *P_INIT_HIF_TX_HEADER_T;

#define LEN_INBAND_CMD_HDR_ROM      sizeof(INIT_HIF_TX_HEADER_T)


typedef struct _HIF_TX_HEADER_PORT1_T { /* size=12 bytes  */
    UINT16     u2TxByteCount;
    UINT16     u2PQ_ID;        /* Must be 0x8000 (Port1, Queue 0) */
    UINT8      ucCID;
    UINT8      ucPktTypeID;    /* Must be 0xA0 (CMD Packet) */
    UINT8      ucReserved;
    UINT8      ucSeqNum;
    UINT8      ucRsv0;
    UINT8      ucPacketOffset;     /* for N9 tx zero copy */
    UINT8      ucRsv2;
    UINT8      ucRsv3;
    UINT8      aucBuffer[];        /* for KEIL porting, don't use zero-length [0] */
} HIF_TX_HEADER_PORT1_T, *P_HIF_TX_HEADER_PORT1_T;



typedef struct _INIT_CMD_DOWNLOAD_CONFIG {
    UINT32     u4Address;
    UINT32     u4Length;
    UINT32     u4DataMode;
} INIT_CMD_DOWNLOAD_CONFIG, *P_INIT_CMD_DOWNLOAD_CONFIG;


//================== Inband Event ====================================================//
typedef enum _ENUM_INIT_EVENT_ID {
    INIT_EVENT_ID_CMD_RESULT = 1,
    INIT_EVENT_ID_ACCESS_REG,
    INIT_EVENT_ID_PENDING_ERROR,
    INIT_EVENT_ID_PATCH_SEMA_CTRL
} ENUM_INIT_EVENT_ID, *P_ENUM_INIT_EVENT_ID;

typedef struct _INIT_WIFI_EVENT_T {

    /* DW#0 */
    UINT16     u2RxByteCount;
    UINT16     u2PacketType;   /* Must be filled with 0xE000 (EVENT Packet) */
    /* DW#1 */
    UINT8      ucEID;
    UINT8      ucSeqNum;
    UINT8      aucReserved[2];
    /* DW#2 */
    UINT8      aucBuffer[];   /* for KEIL porting, don't use zero-length [0] */
} INIT_WIFI_EVENT_T, *P_INIT_WIFI_EVENT_T;


typedef struct _WIFI_EVENT_T {
    /* DW#0 */
    UINT16     u2Length;
    UINT16     u2PktTypeID;        /* Must be filled with 0xE000 (EVENT Packet) */

    /* DW#1 */
    UINT8      ucEID;
    UINT8      ucSeqNum;
    UINT8      aucReserved[2];

    /* DW#2 */
    UINT8      ucExtenEID;         /* Extend EID for MT7603 */
    UINT8      aucReserved2[3];    /* padding fields */
    UINT8      aucBuffer[];        /* for KEIL porting, don't use zero-length [0] */
} WIFI_EVENT_T, *P_WIFI_EVENT_T;

typedef struct _WIFI_EVENT_DATA_PORT_T {
    /* DW#0 */
    UINT16     u2Length;
    UINT16     u2PktTypeID;        /* Must be filled with 0xE000 (EVENT Packet) */

    /* DW#1 */
    UINT8      ucEID;
    UINT8      ucSeqNum;
    UINT8      aucReserved[2];

    /* DW#2 */
    UINT8      ucPacketOffset;
    UINT8      aucReserved2[3];    /* padding fields */
    UINT8      aucBuffer[];        /* for KEIL porting, don't use zero-length [0] */
} WIFI_EVENT_DATA_PORT_T, *P_WIFI_EVENT_DATA_PORT_T;





typedef struct _INIT_EVENT_CMD_RESULT {
    UINT8      ucStatus;           /* 0: success, others: failure */
    UINT8      aucReserved[3];
} INIT_EVENT_CMD_RESULT, *P_INIT_EVENT_CMD_RESULT, INIT_EVENT_PENDING_ERROR, *P_INIT_EVENT_PENDING_ERROR;
#define WIFI_EVENT_HDR_LEN (sizeof (INIT_WIFI_EVENT_T) + sizeof(INIT_EVENT_CMD_RESULT))

#define LEN_INBAND_EVENT_HDR_ROM      sizeof(INIT_WIFI_EVENT_T)
#define LEN_LOOPBACK_EVENT            (LEN_INBAND_EVENT_HDR_ROM + sizeof(INIT_EVENT_CMD_RESULT))

#endif /* __MT_CMD_FMT_H__ */

