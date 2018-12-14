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

#ifndef __CONNSYS_UTIL_H__
#define __CONNSYS_UTIL_H__

#include <stdint.h>

#include "flash_map.h"

#if defined(__CC_ARM) || defined(__ICCARM__)
#include "type_def.h"
#endif
#define GDMA_CH2_BASE          (DMA_base+0x200)
#define GDMA_CH2_SRC           (GDMA_CH2_BASE)
#define GDMA_CH2_DST           (GDMA_CH2_BASE+0x04)

#define GDMA_CH2_COUNT         (GDMA_CH2_BASE+0x10)
#define GDMA_CH2_CON           (GDMA_CH2_BASE+0x14)
#define GDMA_CH2_START         (GDMA_CH2_BASE+0x18)
#define GDMA_CH2_RLCT          (GDMA_CH2_BASE+0x24)

#define MAX_BUF_SIZE (0x400)
#define SDIO_MAX_RW_SIZE    (128) // 32KB is very huge for CM4 SYSRAM,,,,,(128*1024)

//=======================  FW Download  ==================================//
#define WIFI_PROFILE_LEN                (0x800) // 2K
#define WIFI_PROFILE_ADDR               (0x020B2000)
#define WIFI_PROFILE_DATA_MODE          (0x80000000) // plain (non-encrypted)
#define WIFI_PROFILE_KEY_INDEX          (0x0)

/*  Security */
#define WIFI_SEC_BASE           (WIFI_MAC_BASE + 0x00050000)
#define WIFI_MAC_BASE           0x60100000

#define  SEC_WPCR           (WIFI_SEC_BASE + 0x0000)
#define  SEC_SCR            (WIFI_SEC_BASE + 0x0004)
#define  SEC_SSR            (WIFI_SEC_BASE + 0x0010)

#define SEC_SFR1            (WIFI_SEC_BASE + 0x0018)
#define SEC_SMACR           (WIFI_SEC_BASE + 0x0070)
#define SEC_AKR0            (WIFI_SEC_BASE + 0x0080)
#define SEC_AKR1            (WIFI_SEC_BASE + 0x0084)
#define SEC_AKR2            (WIFI_SEC_BASE + 0x0088)
#define SEC_AKR3            (WIFI_SEC_BASE + 0x008C)
#define SEC_AIDR0           (WIFI_SEC_BASE + 0x0090)
#define SEC_AIDR1           (WIFI_SEC_BASE + 0x0094)
#define SEC_AIDR2           (WIFI_SEC_BASE + 0x0098)
#define SEC_AIDR3           (WIFI_SEC_BASE + 0x009c)
#define SEC_AODR0           (WIFI_SEC_BASE + 0x00a0)
#define SEC_AODR1           (WIFI_SEC_BASE + 0x00a4)
#define SEC_AODR2           (WIFI_SEC_BASE + 0x00a8)
#define SEC_AODR3           (WIFI_SEC_BASE + 0x00ac)

/* SEC SMACR */
#define SEC_SMACR_SW_MODE           BIT(0)
#define SEC_SMACR_KE_SKIP           BIT(1)
#define SEC_SMACR_AES_START         BIT(2)
#define SEC_SMACR_AES_DONE          BIT(3)
#define SEC_SMACR_SW_SECRET_KEY_SEL BITS(12, 13)
#define SEC_SMACR_SW_AES2_EN        BIT(15)

#define AES_BLOCK_SIZE 16
#define AES_KEY_NUM         4
#define AES_KEY_SIZE    16
/* PDA - Patch Decryption Accelerator */
#define PDA_DWLD_STATE      (PDA_BASE + 0x0008)
#define PDA_CONFG           (PDA_BASE + 0x000c)

#define PDA_TARGET_LENGTH                   BITS(0, 17)

#define PDA_DOWNLOAD_SOFT_RESET             BIT(31)
#define PDA_DOWNLOAD_BUSY                   BIT(1)
#define PDA_DOWNLOAD_FINISH                 BIT(0)
#define PDA_CONFG_DISABLE_DECRYPT           BIT(0)

#define DOWNLOAD_CONFIG_ENCRYPTION_MODE     BIT(0)
#define DOWNLOAD_CONFIG_ENCRYPT_IDX_MASK    BITS(1,2)
#define DOWNLOAD_CONFIG_ENCRYPT_IDX_OFFSET  1
#define DOWNLOAD_CONFIG_RESET_IV_OPTION     BIT(3)
#define DOWNLOAD_CONFIG_ACK_OPTION

#define PDA_BUSY_WAIT_ITERATION             (100000)
#define PDA_BUSY_WAIT_INTERVAL              (33) // 1ms
#define PDA_SOURCE_CON          (0x80000110)
#define PDA_BASE                (0x81090000)
#define PDA_TAR_ADDR         (PDA_BASE)
#define PDA_TAR_LEN          (PDA_BASE + 0x4)

#define PDA_TOP_WRAPPER_PORT    (0x70000000)
#define PDA_PKT_HDR_SIZE        (12) // 3 DW
#define PDA_CONFG_HEADER_3DW    BIT(1)
#define MTK_FW_DW_BY_CM4_ADDR (0x10000000)

#ifdef MTK_FW_DW_BY_CM4
#define WIFI_FW_ADDR_IN_FLASH   (0x10000000 + N9_RAMCODE_BASE) //  0x10000000 is used for cache (XIP)
#else
#define WIFI_FW_ADDR_IN_FLASH   (0x30000000 + N9_RAMCODE_BASE) // only this address can be access by SYS BUS (DMA), 0x10000000 can only used for cache (XIP)
#endif

#define FEATURE_MASK_ENCRYPT_MODE    (0x01 << 0)    //bit0
#define FEATURE_MASK_KEY_INDEX       (0x03 << 1)    //bit[2:1]

#define LEN_4_BYTE_CRC  4
#define PDA_SOURCE_SEL_BIT              BIT(28)

typedef struct _fw_dl_data_t {
    uint32_t ilm_addr;
    uint32_t ilm_len;
    uint8_t  ilm_encrypt;
    uint8_t  ilm_encrypt_key_index;
    uint32_t dlm_addr;
    uint32_t dlm_len;
    uint8_t  dlm_encrypt;
    uint8_t  dlm_encrypt_key_index;
    uint8_t  *image;
} fw_dl_data_t;

typedef struct _tailer_format_tag {
    uint32_t addr;
    uint8_t  chip_info;
    uint8_t  feature_set;
    uint8_t  ram_version[10];
    uint8_t  ram_built_date[16];
    uint32_t len;

} tailer_format_t;

typedef struct _fw_image_tailer_tag {
    tailer_format_t ilm_info;
    tailer_format_t dlm_info;
} fw_image_tailer_t;


typedef struct _INIT_CMD_WIFI_START {
    uint32_t     u4Override;
    uint32_t     u4Address;
} INIT_CMD_WIFI_START, *P_INIT_CMD_WIFI_START;

typedef struct _INIT_EVENT_PATCH_SEMA_CTRL_T {
    uint8_t      ucStatus;           /* refer to ENUM_PATCH_SEMA_CTRL_EVENT */
    uint8_t      aucReserved[3];
} INIT_EVENT_PATCH_SEMA_CTRL_T, *P_INIT_EVENT_PATCH_SEMA_CTRL_T;

#define DATA_MODE_BIT_SHFT_ENCRYPT_MODE       (0)    //bit0
#define DATA_MODE_MASK_ENCRYPT_MODE           (0x01 << DATA_MODE_BIT_SHFT_ENCRYPT_MODE)    //bit0


#define DATA_MODE_BIT_SHFT_KEY_INDEX          (1)    //bit[2:1]
#define DATA_MODE_MASK_KEY_INDEX              (0x03 << DATA_MODE_BIT_SHFT_KEY_INDEX)    //bit[2:1]

#define DATA_MODE_BIT_SHFT_RESET_IV            (3)   //bit3
#define DATA_MODE_MASK_RESET_IV                (0x1 << DATA_MODE_BIT_SHFT_RESET_IV)

#define DATA_MODE_BIT_SHFT_NEED_ACK           (31)    //bit31
#define DATA_MODE_MASK_NEED_ACK               ((uint32_t)0x01 << DATA_MODE_BIT_SHFT_NEED_ACK)    //bit31

#define LEN_FW_SCATTER                      MAX_BUF_SIZE //(1024*4)
#define PQ_TO_PDA                           0xC000
#define LEN_FW_DOWNLOAD_EVENT               (sizeof(INIT_WIFI_EVENT_T) + sizeof(INIT_EVENT_CMD_RESULT))
//========================   End  =====================================./

//================== Patch download Test ===============================//

typedef struct _patch_header_t {
    /* total 30 bytes */
    uint8_t built_date[16];
    uint8_t platform[4];
    uint8_t hw_ver[2];
    uint8_t sw_ver[2];
    uint8_t reserved[4];
    uint16_t CRC;
} patch_header_t;

typedef struct _patch_dl_data_t {
    uint32_t ilm_addr;
    uint32_t ilm_len;
    uint8_t  *image;
} patch_dl_data_t;

#define MT7687_PATCH_START_ADDR        0xD0000 //0x0008C000
#define LEN_GET_PATCH_SEMAPHORE_EVENT   (sizeof(INIT_WIFI_EVENT_T) + sizeof(INIT_EVENT_PATCH_SEMA_CTRL_T))
#define LEN_PATCH_FILE_HEADER           30 //out patch binary is constructed by (30 bytes header + patch payload)

typedef struct _INIT_CMD_PATCH_SEMA_CTRL {
    uint8_t ucGetSemaphore;/*1: get semaphore, 0: release semaphore*/
    uint8_t aucReserved[3];
} INIT_CMD_PATCH_SEMA_CTRL, *P_INIT_CMD_PATCH_SEMA_CTRL;


typedef enum _ENUM_PATCH_SEMA_CTRL_EVENT_T {
    WIFI_PATCH_SEMA_NEED_PATCH_SEMA_FAIL = 0,
    WIFI_PATCH_SEMA_PATCH_DONE_SEMA_IGNORE = 1,
    WIFI_PATCH_SEMA_NEED_PATCH_SEMA_SUCCESS = 2,
    WIFI_PATCH_SEMA_RELEASE_SUCCESS = 3
} ENUM_PATCH_SEMA_CTRL_EVENT, *P_ENUM_PATCH_SEMA_CTRL_EVENT;


#define MT7636_PATCH_START_ADDR        0x0008C000
#define LEN_GET_PATCH_SEMAPHORE_EVENT   (sizeof(INIT_WIFI_EVENT_T) + sizeof(INIT_EVENT_PATCH_SEMA_CTRL_T))
#define LEN_PATCH_FILE_HEADER           30 //out patch binary is constructed by (30 bytes header + patch payload)
#define SDIO_TX_OUTPUT_BUF_SIZE    1600
#define WIFI_HIF_TX_BYTE_CNT_LEN    4

//========================   End  =====================================./

// ========== Dual Interface =================
#define INIT_EVENT_PACKET_TYPE              (0xE000) /* bit[31:29] = 0x7 */

#define IOT_RX_DATA_PACKET_TYPE             (0xE001) /* don't mess up w/ wlan RAW packet  bit[31:29] of DW0 , 3'b2 (RX), 3'b3 (duplicated RFB) */

#define IOT_PACKET_TYPE_MASK       (0x1 << 1)         /* 802.3 or 802.11 */
#define     IOT_PACKET_TYPE_802_3       (0x0 << 1)
#define     IOT_PACKET_TYPE_802_11      (0x1 << 1)
#define IOT_INF_MASK               (0xF << 4)           /* INF 0~3 */
#define     IOT_PACKET_TYPE_INF_0       (0x0 << 4)
#define     IOT_PACKET_TYPE_INF_1       (0x1 << 4)
#define     IOT_PACKET_TYPE_INF_2       (0x2 << 4)
#define     IOT_PACKET_TYPE_INF_3       (0x3 << 4)

#define IOT_PACKET_TYPE_INF_0_IDX    0
#define IOT_PACKET_TYPE_INF_1_IDX    1

// ========== End =================

#define print_content(__len, __buf, __msg) hex_dump(__msg, __buf, __len)


extern uint8_t g_tx_buf[]; //SDIO_MAX_RW_SIZE

enum MT_CMD_TYPE;

void    connsys_util_fw_download(void);
void    connsys_util_patch_download(void);
int32_t connsys_util_fw_direct_download(void);
uint8_t connsys_util_fw_download_CmdAddressLenReq_event_check(uint8_t *buffer);
int32_t connsys_util_firmware_download(void);
void    connsys_util_fw_download_done(void);
int32_t connsys_util_pda_download(uint32_t dest_addr, uint32_t data_len, uint32_t data_mode, uint32_t data_offset, uint8_t *image);
uint8_t connsys_util_patch_download_get_semaphore_event_check(uint8_t *buffer, uint8_t *status);

uint32_t connsys_util_create_inband_cmd_txd_rom(uint32_t cmd_len, uint8_t *buffer, enum MT_CMD_TYPE cmd);
int32_t connsys_util_low_level_output(uint8_t *buf, ssize_t len, int32_t inf_num);
int32_t connsys_util_low_level_rx_data(uint8_t *buf, ssize_t *len, int32_t port);
int32_t connsys_util_tx_data(uint8_t *buf, ssize_t len);
int32_t connsys_util_rx_data(uint8_t *buf, ssize_t len, int32_t port);

int32_t connsys_util_intr_enhance_mode_receive_data(void);
void    connsys_util_intr_enhance_mode_dump_last_struct(void);

void    connsys_util_get_ncp_patch_ver(char *ver);
void    connsys_util_get_n9_fw_ver(char *ver);
uint32_t connsys_util_pse_get_p0_min_resv(void);

uint8_t connsys_util_cli_handler(uint8_t len, char *param[]);

#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
void connnsys_balance_init(void);
#endif


uint8_t raw_packet_handler_enabled(void);

void wifi_set_raw_rssi_threshold(uint8_t enable, int8_t rssi);

void wifi_get_raw_rssi_threshold(uint8_t *enable, int8_t *rssi);

void connsys_tx_query_whisr(void);

int32_t wifi_util_get_inf_number(uint8_t *buf);

#endif
