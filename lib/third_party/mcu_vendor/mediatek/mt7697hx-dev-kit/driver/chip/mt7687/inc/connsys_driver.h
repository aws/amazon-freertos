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

#ifndef __CONNSYS_DRIVER_H__
#define __CONNSYS_DRIVER_H__

#include <stdint.h>
#include <stdio.h>
#include "connsys_profile.h"

/* CONNSYS_MAX_RX_PKT_SIZE is set by PBUF_POOL_BUFSIZE. */
#define CONNSYS_MAX_RX_PKT_SIZE 1664 //(0x640) // The max RX pkt size by upper layer (lwip)
#define MY_CONNSYS_BLOCK_SIZE  128

#ifdef MTK_HIF_GDMA_ENABLE
#define CFG_WIFI_HIF_GDMA_EN                 1
#else
#define CFG_WIFI_HIF_GDMA_EN                 0
#endif
#define CFG_CONNSYS_POLLING_IRQ_EN              0
#define CFG_CONNSYS_DVT_RX_ENHANCE_MODE_EN      0
#define CFG_CONNSYS_IOT_RX_ZERO_COPY_EN         1
#define CFG_CONNSYS_IOT_TX_ZERO_COPY_EN         1
#define CFG_CONNSYS_TRX_BALANCE_EN              0

#define CFG_CONNSYS_MEASURE_INT_TIME_EN         0
#define CFG_CONNSYS_MEASURE_TIME_NUM            4
#define CFG_CONNSYS_CONTINOUS_ALLOCATE_FAIL_PRINT_CNT_VAL (16)

#define CFG_CONNSYS_BALANCE_DEFAULT_TX_NUM      10
#define CFG_CONNSYS_BALANCE_DEFAULT_RX_NUM      10
#define CFG_CONNSYS_BALANCE_DEFAULT_EN_VAL      1


#define CONNSYS_POLLING_DELAY_TIME 10
#define CFG_MAX_HIF_RX_LEN_NUM                  0
#define CFG_RPT_OWN_RX_PACKET_LEN_EN            0
#define CFG_RX0_RPT_PKT_LEN                     4 /* 0:16, 1~15 */
#define CFG_RX1_RPT_PKT_LEN                     10
#define DEFAULT_N9_PSE_PAGE_SIZE                128
#define DEFAULT_N9_PSE_PAGE_QUOTA               910

#define NUM_OF_WIFI_HIF_RX_PORT                 2
#define NUM_OF_WIFI_HIF_TX_PORT                 2
#define WIFI_HIF_RX_PORT0_IDX                   0   /* in-band command port */
#define WIFI_HIF_RX_PORT1_IDX                   1   /* data port */
#define WIFI_HIF_TX_PORT_IDX                    0

#define WIFI_HIF_RX_CS_OFFLOAD_STATUS_LEN       4
#define WIFI_HIF_HEADER_LEN                 12
#define NUM_OF_WIFI_TXQ                     16
#define WIFI_HIF_RX_FIFO_MAX_LEN            16

#define CONNSYS_STATUS_SUCCESS  (0)
#define CONNSYS_STATUS_FAIL     (-1)
#define CONNSYS_DEBUG_MODE_EN          1


/* TXCFFA: the real amount of free pages that HIF source port can use for this time.
 * Other TXQ_CNT: the amount of finished packet for a certain queue.
 */
#define WIFI_TXQ_CNT_IDX_0_TXCAC0       0
#define WIFI_TXQ_CNT_IDX_1_TXCAC1       1
#define WIFI_TXQ_CNT_IDX_2_TXCAC2       2
#define WIFI_TXQ_CNT_IDX_3_TXCAC3       3
#define WIFI_TXQ_CNT_IDX_4_TXCAC4       4
#define WIFI_TXQ_CNT_IDX_5_TXCAC5       5
#define WIFI_TXQ_CNT_IDX_6_TXCAC6       6
#define WIFI_TXQ_CNT_IDX_7_TXCBMC       7
#define WIFI_TXQ_CNT_IDX_8_TXCBCN       8
#define WIFI_TXQ_CNT_IDX_9_TXCAC10      9
#define WIFI_TXQ_CNT_IDX_10_TXCAC11     10
#define WIFI_TXQ_CNT_IDX_11_TXCAC12     11
#define WIFI_TXQ_CNT_IDX_12_TXCAC13     12
#define WIFI_TXQ_CNT_IDX_13_TXCAC14     13
#define WIFI_TXQ_CNT_IDX_14_TXCFFA      14
#define WIFI_TXQ_CNT_IDX_15_TXCCPU      15

#define LEN_INT_ENHANCE_MODE    (sizeof(enhance_mode_data_struct_t))
#define LEN_RX_ENHANCE_MODE     (4 + sizeof(enhance_mode_data_struct_t)) // HW design spec


#define WF_PSE_TOP_BASE 0xA0000000

#define PSE_BC          (WF_PSE_TOP_BASE + 0x14)
#define PSE_BC_TOTAL_PAGE_NUM_OFFSET  (0)
#define PSE_BC_TOTAL_PAGE_NUM_MASK  ((0xFFF)<<PSE_BC_TOTAL_PAGE_NUM_OFFSET)



#define PSE_GQC         (WF_PSE_TOP_BASE + 0x118)

#define PSE_GQC_QLEN_IN_PAGE_OFFSET 23
#define PSE_GQC_QLEN_IN_PAGE_PAGES (1<<23)
#define PSE_GQC_QLEN_IN_PAGE_FRAMES (0<<23)


#define PSE_GQC_GET_QLEN_PID_OFFSET 21
#define PSE_GQC_GET_QLEN_PID_MASK   ((0x3)<<21)
#define PSE_GQC_GET_QLEN_QID_OFFSET 16
#define PSE_GQC_GET_QLEN_QID_MASK   ((0x1F)<<16)
#define PSE_GQC_QLEN_RETURN_VALUE_OFFSET    0
#define PSE_GQC_QLEN_RETURN_VALUE_MASK      (0xFFF)


#define PSE_FC_P0           (WF_PSE_TOP_BASE + 0x120)
#define PSE_FC_P1           (WF_PSE_TOP_BASE + 0x124)
#define PSE_FC_P2_Q0        (WF_PSE_TOP_BASE + 0x128)
#define PSE_FC_P2_Q1        (WF_PSE_TOP_BASE + 0x12C)
#define PSE_FC_P2_Q2        (WF_PSE_TOP_BASE + 0x130)

#define PSE_FC_MAX_QUOTA_OFFSET     (16)
#define PSE_FC_MAX_QUOTA_MASK     ((0xFFF)<<PSE_FC_MAX_QUOTA_OFFSET)
#define PSE_FC_MIN_RSRV_OFFSET     (0)
#define PSE_FC_MIN_RSRV_MASK     ((0xFFF)<<PSE_FC_MIN_RSRV_OFFSET)


#define PSE_FC_FFC          (WF_PSE_TOP_BASE + 0x134)
#define PSE_FC_FFC_FFA_CNT_OFFSET (16)
#define PSE_FC_FFC_FFA_CNT_MASK ((0xFFF)<<PSE_FC_FFC_FFA_CNT_OFFSET)
#define PSE_FC_FFC_FREE_PAGE_CNTT_OFFSET (0)
#define PSE_FC_FFC_FREE_PAGE_CNTT_MASK ((0xFFF)<<PSE_FC_FFC_FREE_PAGE_CNTT_OFFSET)






#define DBG_CONNSYS(FEATURE, _Fmt) \
        {                                       \
            if (g_connsys_debug_feature & FEATURE)   \
            {               \
                printf _Fmt; \
            }   \
        }


#define CONNSYS_ASSIGN_DEBUG(_CAP)       ( (g_connsys_debug_feature) = (_CAP))
#define CONNSYS_SET_DEBUG(_F)       ( (g_connsys_debug_feature) |= (_F))
#define CONNSYS_CLEAR_DEBUG(_F)     ((g_connsys_debug_feature) &= ~(_F))
#define CONNSYS_TEST_DEBUG(_F)      (((g_connsys_debug_feature) & (_F)) != 0)
#define CONNSYS_TEST_DEBUGS(_F)     (((g_connsys_debug_feature) & (_F)) == (_F))
#define CONNSYS_TEST_DEBUG_ENABLE(_F)      ((((g_connsys_debug_feature) & (_F)) != 0)?1:0)




#define CONNSYS_DBG_RX   (1<<0)
#define CONNSYS_DBG_TX_Q0   (1<<1)
#define CONNSYS_DBG_TX_Q1   (1<<2)
#define CONNSYS_DBG_PORT    (1<<3)
#define CONNSYS_DBG_MAX_NUM 3

#define SHARE_MEM_START_ADDR   (0x2003EC00)
#define SHARE_MEM_LENGTH       (0x1000)


typedef struct {
    uint32_t             WHISR_reg_val;
    union {
        struct {
            uint16_t             free_page_num[NUM_OF_WIFI_TXQ];
        } u;
        uint32_t                 WTSR_val[8];
    } tx_info;
    union {
        struct {
            uint16_t             valid_len_num[NUM_OF_WIFI_HIF_RX_PORT];
            uint16_t             each_rx_len[NUM_OF_WIFI_HIF_RX_PORT][WIFI_HIF_RX_FIFO_MAX_LEN];
        } u;
        uint32_t                 rx_status_raw[17];
    } rx_info;
    uint32_t                     receive_mail_box_0;
    uint32_t                     receive_mail_box_1;
} enhance_mode_data_struct_t;

#define MAX_TX_BUF 5

typedef struct {
    uint8_t *buf_ptr;
    uint32_t buf_len;
} connsys_tx_buf_t;

typedef struct {
    uint8_t txd[12];
    uint32_t reserved_headroom_len;
    uint32_t tx_len;
    uint32_t real_tx_len;
    uint32_t buf_num;
    connsys_tx_buf_t buff[MAX_TX_BUF];
    int8_t fg_wmm;
} connsys_tx_scatter_info_t;

typedef struct {
    uint32_t send_pkt_cnt_by_tx_port[NUM_OF_WIFI_HIF_TX_PORT];
    uint32_t total_send_pkt_cnt;
    uint32_t send_page_cnt_by_tx_port[NUM_OF_WIFI_HIF_TX_PORT];
    uint32_t total_send_page_cnt;
    uint32_t total_drop_pkt_cnt;

    uint32_t free_page_cnt_by_wifi_txq[NUM_OF_WIFI_TXQ];
    uint32_t total_free_page_cnt;
    int32_t available_page_cnt;
    int32_t current_page_cnt;
    int32_t max_page_cnt;
    uint32_t reserve_quota_page_cnt;
    uint32_t page_sz;

} wifi_hif_tx_flow_control_t;


typedef struct {
    uint32_t time[CFG_CONNSYS_MEASURE_TIME_NUM];
    uint32_t pkt_time[NUM_OF_WIFI_HIF_RX_PORT][WIFI_HIF_RX_FIFO_MAX_LEN];
    uint32_t pkt_cnt[NUM_OF_WIFI_HIF_RX_PORT];
} connsys_measure_time_t;



typedef struct _RX_ENHANCE_MODE_DATA_STRUCT_T {
    uint32_t padding_0;
    enhance_mode_data_struct_t enhance_info;
} RX_ENHANCE_MODE_DATA_STRUCT_T, *P_RX_ENHANCE_MODE_DATA_STRUCT_T;

typedef struct _connsys_irq_info_tag {
    uint8_t   irq_assert;
    enhance_mode_data_struct_t  irq_data;
} connsys_irq_info_t;

typedef struct _connsys_intr_enhance_arg_tag {
    uint8_t rxNum;
    uint8_t totalBytes;
} connsys_intr_enhance_arg_t;

struct connsys_func;

typedef void (*connsys_irq_handler_t)(struct connsys_func *);

struct connsys_func {
    connsys_irq_handler_t   irq_callback;  /* IRQ callback */
    connsys_irq_info_t     irq_info;
    uint32_t num;       /* function number */
    uint32_t blksize;       /* current block size */
    uint32_t use_dma;
    void   *task_handle;

#if (CM4_INDIUM_PROJ == 1)
    boolean fgRxEnhanceMode;
    PFN_SDIO_EVT_HANDLER rSdioEvtTable[SDIO_EVT_NUM];
    void *pPrivData;
    uint32_t    u4WASR;
#endif
};

typedef struct _connsys_info {
    uint32_t rx_packet_cnt;
    uint32_t rx_invalid_sz_packet_cnt;
    uint32_t rx_max_invalid_sz;
    uint32_t rx_error_cnt;
    uint32_t rx_allocate_fail_cnt;
} connsys_info;

typedef struct _connsys_stat {
    uint32_t number_of_int;
    uint32_t number_of_abnormal_int;
    uint32_t num_of_tx_int;
    uint32_t num_of_rx_int;
    uint32_t num_of_tx_wait;
    uint32_t number_of_fw_own_back;
    uint32_t process_start_time;
    uint32_t process_end_time;
    connsys_info rx_port[NUM_OF_WIFI_HIF_RX_PORT];
} connsys_stat_t;


typedef struct _connsys_balance_ctr_t {
    uint32_t    num_of_continuous_tx;
    uint32_t    num_of_continuous_rx;
    uint32_t    num_of_tx;
    uint32_t    num_of_rx;
    uint8_t     fg_tx_wait;
    uint8_t     fg_rx_wait;
    uint32_t    rx_yield_time;
    uint32_t    tx_yield_time;
    uint32_t    tx_handle;
    uint32_t    rx_handle;
    uint8_t     balance_en;


} connsys_balance_ctr_t;


enum SDIO_GEN3_RW_TYPE {
    SDIO_GEN3_READ,
    SDIO_GEN3_WRITE
};

enum SDIO_GEN3_TRANS_MODE {
    SDIO_GEN3_BYTE_MODE,
    SDIO_GEN3_BLOCK_MODE
};

enum SDIO_GEN3_OP_MODE {
    SDIO_GEN3_FIXED_PORT_MODE,
    SDIO_GEN3_INCREMENT_MODE
};

enum SDIO_GEN3_FUNCTION {
    SDIO_GEN3_FUNCTION_0,
    SDIO_GEN3_FUNCTION_WIFI,
    SDIO_GEN3_FUNCTION_BT,
};

enum CONNSYS_HEADROOM_OFFSET_TYPE {
    CONNSYS_HEADROOM_OFFSET_NON_QOS, /* 0 */
    CONNSYS_HEADROOM_OFFSET_QOS,     /* 1 */
};

typedef enum {
    DUMP_TRIGGER_SRC_CM4_BT,
    DUMP_TRIGGER_SRC_CM4_WIFI,
    DUMP_TRIGGER_SRC_CM4_N9_HEART,
    DUMP_TRIGGER_SRC_N9_SELFCHECK_CRASH
} dump_para_trigger_source_t;

typedef enum {
    DUMP_PARA_TYPE_MINI,
    DUMP_PARA_TYPE_FULL
} dump_para_type_t;

typedef enum {
    DUMP_PROCESS_FLAG_READY_FOR_WRITE,
    DUMP_PROCESS_FLAG_WRITING,
    DUMP_PROCESS_FLAG_FIN_WRITING
} dump_para_process_flag_t;

typedef union _connsys_gen3_cmd52_info {
    struct {
        uint32_t data : 8;             /* data for write, dummy for read */
        uint32_t reserved_8: 1;        /* stuff */
        uint32_t addr : 17;            /* register address */
        uint32_t reserved_26_27: 2;    /* raw flag / stuff */
        uint32_t func_num: 3;          /* function number */
        uint32_t rw_flag: 1;           /* read / write flag */
    } field;
    uint32_t word;
} connsys_gen3_cmd52_info;

typedef union _connsys_gen3_cmd53_info {
    struct {
        uint32_t count : 9;            /* block count for block mode, byte count for byte mode  */
        uint32_t addr : 17;            /* register address */
        uint32_t op_mode: 1;           /* 1 for increment mode, 0 for port mode */
        uint32_t block_mode: 1;        /* 1 for block mode, 0 for byte mode */
        uint32_t func_num: 3;          /* function number */
        uint32_t rw_flag: 1;           /* read / write flag */
    } field;
    uint32_t word;
} connsys_gen3_cmd53_info;

extern connsys_stat_t g_connsys_stat;
extern uint32_t g_connsys_debug_feature;
extern struct connsys_func g_connsys_func;
extern struct connsys_callback_func g_connsys_callback_func;
extern enhance_mode_data_struct_t g_last_enhance_mode_data_struct;
extern connsys_balance_ctr_t g_balance_ctr;


int32_t connsys_cccr_read(uint32_t addr, uint8_t *value);
int32_t connsys_cccr_write(uint32_t addr, uint8_t value);
int32_t connsys_cr_read(uint32_t addr, uint32_t *value);
int32_t connsys_cr_write(uint32_t addr, uint32_t value);
int32_t connsys_fifo_read(uint32_t addr, uint8_t *buf, size_t size);
int32_t connsys_fifo_write(uint8_t *buf, size_t size);

#if (CFG_CONNSYS_POLLING_IRQ_EN == 1)
uint32_t connsys_polling_irq();
#endif

void connsys_driver_interrupt(struct connsys_func *func);

uint8_t connsys_get_ownership(void);
int32_t connsys_abnormal_interrupt_check(uint32_t value);
int32_t connsys_open(void);
int32_t connsys_close(void);
void connsys_set_wifi_profile(sys_cfg_t *config);
int32_t connsys_enable_interrupt(void);
int32_t connsys_disable_interrupt(void);
int32_t connsys_disable_dump_n9_clr(void);
int32_t connsys_enable_dump_n9_set(void);


void connsys_tx_flow_control_init(void);
void connsys_tx_flow_control_show_info(void);
uint32_t connsys_tx_flow_control_get_page_size(void);
void connsys_tx_flow_control_update_free_page_cnt(void);
int32_t connsys_tx_flow_control_check_and_update_tx(int32_t port, uint32_t pkt_len);
void connsys_tx_flow_control_config(uint8_t hif_tx_flow_ctrl_en);
void connsys_tx_flow_control_set_reserve_page(uint32_t reserve_page);

void connsys_show_config_option(void);
void connsys_dump_cr(void);
void connsys_print_stat(void);
uint32_t connsys_get_stat_int_count(void);

void connsys_dump_whisr(void);

uint32_t connsys_get_headroom_offset(uint8_t qos_enable);
void connsys_set_headroom_offset(uint8_t qos_enable, uint8_t offset);

#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
void connsys_measure_time_set(uint8_t idx);
#ifdef MTK_MINICLI_ENABLE
void connsys_measure_time_show(void);
#endif
void connsys_measure_time_finish(void);
void connsys_measure_time_set_pkt_time(uint8_t port, uint8_t idx);
void connsys_measure_time_set_pkt_num(uint8_t port, uint8_t num);
#endif

void connsys_disable_whier_trx_int(void);
void connsys_enalbe_whier_rx_int(void);
void connsys_enalbe_whier_tx_int(void);
void connsys_enable_whier_dump_n9_int(void);
void connsys_disable_whier_dump_n9_int(void);


#if (CFG_WIFI_HIF_GDMA_EN == 1)
void connsys_enable_dma(void);
#endif

int32_t connsys_write_data(connsys_tx_scatter_info_t *tx_info);
void connsys_dump_tx_scatter_info(connsys_tx_scatter_info_t *tx_info);
void connsys_tx_flow_control_set_reserve_page_by_cr(void);
int32_t connsys_init(sys_cfg_t *sys_config);
int8_t lp_connsys_get_own_enable_int(void);
int8_t lp_connsys_give_n9_own(void);
uint8_t connsys_get_ownership(void);


#endif /* __CONNSYS_DRIVER_H__ */
