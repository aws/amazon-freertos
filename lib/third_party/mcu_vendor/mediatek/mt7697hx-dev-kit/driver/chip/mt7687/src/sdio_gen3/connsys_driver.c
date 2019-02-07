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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "type_def.h"
#include "nvic.h"
#include "cos_api.h"
#include "hal_gdma.h"
#include "hal_nvic.h"
#include "hal_lp.h"
#include "dma_sw.h"
#include "misc.h"
#include "connsys_driver.h"
#include "connsys_bus.h"
#include "connsys_util.h"
#include "mt_cmd_fmt.h"
#include <stdint.h>
#include "memory_attribute.h"
#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
#include "gpt.h"
#endif
#include "syslog.h"
#if (CFG_WIFI_HIF_GDMA_EN == 1)
#include "dma_hw.h"
#endif

#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
#include "net_task.h"
#endif

#ifdef  MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE
#include "context_dump.h"
#endif

#include "connsys_adapter.h"
#include "task_def.h"

void (*rx_interrupt_handle)(int32_t) = NULL;

struct connsys_func g_connsys_func;
static uint32_t g_whisr_val = 0;

#if defined(__ICCARM__)
ATTR_4BYTE_ALIGN enhance_mode_data_struct_t g_last_enhance_mode_data_struct;
#else
enhance_mode_data_struct_t g_last_enhance_mode_data_struct __attribute__((aligned(4)));
#endif

wifi_hif_tx_flow_control_t g_hif_tx_flow_control_stat;
uint8_t g_hif_tx_flow_ctrl_en = 0;
connsys_balance_ctr_t g_balance_ctr;

connsys_ops_t connsys_ops = NULL;

#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1 || defined(MTK_BSP_LOOPBACK_ENABLE))
connsys_measure_time_t g_connsys_time_current;
connsys_measure_time_t g_connsys_time_last;
connsys_measure_time_t g_connsys_time_max;
#endif

connsys_stat_t g_connsys_stat;
uint32_t g_connsys_debug_feature = 0;

#if defined(MTK_HAL_LOWPOWER_ENABLE)
extern int8_t (*ptr_lp_connsys_get_own_enable_int)(void);
extern int8_t (*ptr_lp_connsys_give_n9_own)(void);
extern uint8_t (*ptr_connsys_get_ownership)(void);
#endif

#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1 || defined(MTK_BSP_LOOPBACK_ENABLE))
uint32_t connsys_measure_convert2ms(uint32_t count)
{
    uint32_t ret;
    if (count) {
        ret = count / (192 * 1000);
    } else {
        ret = 0;
    }
    return ret;
}

void connsys_measure_time_init(void)
{
    CM4_GPT4Init();
    GPT_Stop(4);
    GPT_Start(4);
}

void connsys_measure_time_finish(void)
{
    uint32_t flags;

    local_irq_save(flags);
    memcpy(&g_connsys_time_last,
           &g_connsys_time_current,
           sizeof(connsys_measure_time_t));

#if 0
    for (i = 0; i < CFG_CONNSYS_MEASURE_TIME_NUM; i++) {
        if (g_connsys_time_max.time[i] < g_connsys_time_last.time[i]) {
            g_connsys_time_max.time[i] = g_connsys_time_last.time[i];
        }
    }

    for (i = 0; i < NUM_OF_WIFI_HIF_RX_PORT; i++) {
        if (g_connsys_time_max.pkt_cnt[i] < g_connsys_time_last.pkt_cnt[i]) {
            g_connsys_time_max.pkt_cnt[i] = g_connsys_time_last.pkt_cnt[i];
        }
    }
#endif
    if (g_connsys_time_max.time[3] < g_connsys_time_last.time[3]) {
        memcpy(&g_connsys_time_max,
               &g_connsys_time_last,
               sizeof(connsys_measure_time_t));
    }
    local_irq_restore(flags);
}


void connsys_measure_time_set(uint8_t idx)
{
    uint32_t flags;
    local_irq_save(flags);
    g_connsys_time_current.time[idx] = GPT_return_current_count(4);
    local_irq_restore(flags);
}

void connsys_measure_time_set_pkt_time(uint8_t port, uint8_t idx)
{
    uint32_t flags;
    local_irq_save(flags);
    g_connsys_time_current.pkt_time[port][idx] = GPT_return_current_count(4);
    local_irq_restore(flags);
}

void connsys_measure_time_set_pkt_num(uint8_t port, uint8_t num)
{
    uint32_t flags;
    local_irq_save(flags);
    g_connsys_time_current.pkt_cnt[port] = num;
    local_irq_restore(flags);
}

#ifdef MTK_MINICLI_ENABLE
void connsys_measure_time_show(void)
{
    connsys_measure_time_t tmp;
    uint32_t flags;
    uint32_t i;
    uint32_t port;

    printf("==>connsys_measure_time_show\n");
    local_irq_save(flags);
    memcpy(&tmp, &g_connsys_time_last, sizeof(connsys_measure_time_t));
    local_irq_restore(flags);
    printf("last measure time:\n");
    for (i = 0; i < CFG_CONNSYS_MEASURE_TIME_NUM; i++) {
        printf("%u: %u count, %u ms\n",
               (unsigned int)i,
               (unsigned int)tmp.time[i],
               (unsigned int)connsys_measure_convert2ms(tmp.time[i]));
    }
    printf("detail pkt time:\n");

    for (port = 0; port < NUM_OF_WIFI_HIF_RX_PORT; port ++) {
        printf("p[%u]:\n", (unsigned int)port);
        printf("\tmax pkt num: %u\n", (unsigned int)tmp.pkt_cnt[port]);
        for (i = 0; i < tmp.pkt_cnt[port]; i++) {
            printf("\tidx %u, %u count, %u ms\n",
                   (unsigned int)i,
                   (unsigned int)tmp.pkt_time[port][i],
                   (unsigned int)connsys_measure_convert2ms(tmp.pkt_time[port][i]));
        }
    }

    local_irq_save(flags);
    memcpy(&tmp, &g_connsys_time_max, sizeof(connsys_measure_time_t));
    local_irq_restore(flags);
    printf("max measure time:\n");
    for (i = 0; i < CFG_CONNSYS_MEASURE_TIME_NUM; i++) {
        printf("%u: %u count, %u ms\n",
               (unsigned int)i,
               (unsigned int)tmp.time[i],
               (unsigned int)connsys_measure_convert2ms(tmp.time[i]));
    }
    printf("detail pkt time:\n");

    for (port = 0; port < NUM_OF_WIFI_HIF_RX_PORT; port ++) {
        printf("p[%u]:\n", (unsigned int)port);
        printf("\tmax pkt num: %u\n", (unsigned int)tmp.pkt_cnt[port]);
        for (i = 0; i < tmp.pkt_cnt[port]; i++) {
            printf("\t%u: %u count, %u ms\n",
                   (unsigned int)i,
                   (unsigned int)tmp.pkt_time[port][i],
                   (unsigned int)connsys_measure_convert2ms(tmp.pkt_time[port][i]));
        }
    }
}
#endif // end of MTK_MINICLI_ENABLE
#endif

void connsys_print_stat(void)
{

    connsys_stat_t stat;
    uint32_t flags;
    uint32_t port = 0;

    local_irq_save(flags);
    memcpy(&stat, &g_connsys_stat, sizeof(connsys_stat_t));
    local_irq_restore(flags);

    printf("==>connsys_print_stat\n");
    printf("number of interrupt: %u\n", (unsigned int)stat.number_of_int);
    printf("number of tx interrupt: %u\n", (unsigned int)stat.num_of_tx_int);
    printf("number of rx interrupt: %u\n", (unsigned int)stat.num_of_rx_int);
    printf("number of tx wait: %u\n", (unsigned int)stat.num_of_tx_wait);
    printf("number of abnormal interrupt: %u\n", (unsigned int)stat.number_of_abnormal_int);
    printf("number fw_own_back: %u\n", (unsigned int)stat.number_of_fw_own_back);

    for (port = 0; port < NUM_OF_WIFI_HIF_RX_PORT; port++) {
        printf("port index:%u\n", (unsigned int)port);
        printf("\trx_packet_cnt:%u\n", (unsigned int)stat.rx_port[port].rx_packet_cnt);
        printf("\trx_invalid_sz_packet_cnt:%u\n", (unsigned int)stat.rx_port[port].rx_invalid_sz_packet_cnt);
        printf("\trx_max_invalid_sz:%u\n", (unsigned int)stat.rx_port[port].rx_max_invalid_sz);
        printf("\trx_error_cnt:%u\n", (unsigned int)stat.rx_port[port].rx_error_cnt);
        printf("\trx_allocate_fail_cnt:%u\n", (unsigned int)stat.rx_port[port].rx_allocate_fail_cnt);
    }
}

void connsys_dump_cr(void)
{
    uint32_t value;

    printf("==>connsys_dump_cr\n");

    connsys_cr_read(WHLPCR, &value);
    printf("WHLPCR(0x%x): 0x%x\n", WHLPCR, (unsigned int)value);
    printf("\tW_INT_EN_SET:%u\n", (value & W_INT_EN_SET) ? 1 : 0);
    printf("\tW_INT_EN_CLR:%u\n", (value & W_INT_EN_CLR) ? 1 : 0);

    connsys_cr_read(WHCR, &value);
    printf("WHCR(0x%x): 0x%x\n", WHCR, (unsigned int)value);
    printf("\tRX_ENHANCE_MODE:%u\n", (value & RX_ENHANCE_MODE) ? 1 : 0);
    printf("\tMAX_HIF_RX_LEN_NUM:%u\n", (unsigned int)((value & MAX_HIF_RX_LEN_NUM_MASK) >> MAX_HIF_RX_LEN_NUM_OFFSET));

    printf("dump last intr enhance mode whisr\n");
    value = g_last_enhance_mode_data_struct.WHISR_reg_val;
    printf("WHISR(0x%x): 0x%x\n", WHISR, (unsigned int)value);
    printf("\tTX_DONE_INT:%u\n", (value & TX_DONE_INT) ? 1 : 0);
    printf("\tRX0_DONE_INT:%u\n", (value & RX0_DONE_INT) ? 1 : 0);
    printf("\tRX1_DONE_INT:%u\n", (value & RX1_DONE_INT) ? 1 : 0);
    printf("\tABNORMAL_INT:%u\n", (value & ABNORMAL_INT) ? 1 : 0);
    printf("\tFW_OWN_BACK_INT:%u\n", (value & FW_OWN_BACK_INT) ? 1 : 0);

    printf("dump last read whisr by cmd\n");
    value = g_whisr_val;
    printf("WHISR(0x%x): 0x%x\n", WHISR, (unsigned int)value);
    printf("\tTX_DONE_INT:%u\n", (value & TX_DONE_INT) ? 1 : 0);
    printf("\tRX0_DONE_INT:%u\n", (value & RX0_DONE_INT) ? 1 : 0);
    printf("\tRX1_DONE_INT:%u\n", (value & RX1_DONE_INT) ? 1 : 0);
    printf("\tABNORMAL_INT:%u\n", (value & ABNORMAL_INT) ? 1 : 0);
    printf("\tFW_OWN_BACK_INT:%u\n", (value & FW_OWN_BACK_INT) ? 1 : 0);


    connsys_cr_read(WHIER, &value);
    printf("WHIER(0x%x): 0x%x\n", WHIER, (unsigned int)value);
    printf("\tTX_DONE_INT_EN :%u\n", (value & TX_DONE_INT_EN) ? 1 : 0);
    printf("\tRX0_DONE_INT_EN :%u\n", (value & RX0_DONE_INT_EN) ? 1 : 0);
    printf("\tRX1_DONE_INT_EN :%u\n", (value & RX1_DONE_INT_EN) ? 1 : 0);
    printf("\tABNORMAL_INT_EN :%u\n", (value & ABNORMAL_INT_EN) ? 1 : 0);
    printf("\tFW_OWN_BACK_INT_EN :%u\n", (value & FW_OWN_BACK_INT_EN) ? 1 : 0);

    connsys_cr_read(WASR, &value);
    printf("WASR(0x%x): 0x%x\n", WASR, (unsigned int)value);
    printf("\tTX1_OVERFLOW :%u\n", (value & TX1_OVERFLOW) ? 1 : 0);
    printf("\tRX0_UNDERFLOW :%u\n", (value & RX0_UNDERFLOW) ? 1 : 0);
    printf("\tRX1_UNDERFLOW :%u\n", (value & RX1_UNDERFLOW) ? 1 : 0);
    printf("\tWASR2 :%u\n", (value & WASR_WASR2) ? 1 : 0);

    connsys_cr_read(WASR2, &value);
    printf("WASR2(0x%x): 0x%x (Note: all fields are read clear)\n", WASR2, (unsigned int)value);
    printf("\tWASR2_CMD53_ERR_CNT :%u\n",
           (unsigned int)((value & WASR2_CMD53_ERR_CNT_MASK) >> WASR2_CMD53_ERR_CNT_OFFSET));
    printf("\tWASR2_CMD53_ERR :%u\n", (value & WASR2_CMD53_ERR) ? 1 : 0);
    printf("\tWASR2_CMD53_RD_TIMEOUT :%u\n", (value & WASR2_CMD53_RD_TIMEOUT) ? 1 : 0);
    printf("\tWASR2_CMD53_WR_TIMEOUT :%u\n", (value & WASR2_CMD53_WR_TIMEOUT) ? 1 : 0);
    printf("\tWASR2_FW_OWN_INVALID_ACCESS :%u\n", (value & WASR2_FW_OWN_INVALID_ACCESS) ? 1 : 0);
}


void connsys_dump_whisr(void)
{
    uint32_t value;

    printf("==>connsys_dump_whisr\n");
    connsys_cr_read(WHISR, &value);
    g_whisr_val = value;
    printf("WHISR(0x%x): 0x%x\n", WHISR, (unsigned int)value);
    printf("\tTX_DONE_INT:%u\n", (value & TX_DONE_INT) ? 1 : 0);
    printf("\tRX0_DONE_INT:%u\n", (value & RX0_DONE_INT) ? 1 : 0);
    printf("\tRX1_DONE_INT:%u\n", (value & RX1_DONE_INT) ? 1 : 0);
    printf("\tABNORMAL_INT:%u\n", (value & ABNORMAL_INT) ? 1 : 0);
    printf("\tFW_OWN_BACK_INT:%u\n", (value & FW_OWN_BACK_INT) ? 1 : 0);
}

uint32_t connsys_get_stat_int_count(void)
{
    uint32_t flags;
    uint32_t number_of_int;

    local_irq_save(flags);
    number_of_int = g_connsys_stat.number_of_int;
    local_irq_restore(flags);

    return number_of_int;
}

int32_t connsys_cccr_read(uint32_t addr, uint8_t *value)
{
    int32_t ret = 0;
    struct connsys_func *dev_func = &g_connsys_func;

    connsys_bus_get_bus(dev_func);
    *value = connsys_bus_fn0_read_byte(dev_func, addr, &ret);
    connsys_bus_release_bus(dev_func);
    if (ret) {
        LOG_E(connsys, "<<%s>> Read CCCR 0x%02x failed. Error = %d\n",
              __FUNCTION__,
              (unsigned int)addr,
              (int)ret);
    }
    return ret;
}

int32_t connsys_cccr_write(uint32_t addr, uint8_t value)
{
    int32_t ret = CONNSYS_STATUS_SUCCESS;
    struct connsys_func *dev_func = &g_connsys_func;

    connsys_bus_get_bus(dev_func);
    connsys_bus_fn0_write_byte(dev_func, value, addr, &ret);
    connsys_bus_release_bus(dev_func);

    if (ret) {
        LOG_E(connsys, "<<%s>> Write register 0x%02x failed. Error = %d\n",
              __FUNCTION__,
              (unsigned int)addr,
              (int)ret);
    }

    return ret;
}

int32_t connsys_cr_read(uint32_t addr, uint32_t *value)
{

    int32_t ret = CONNSYS_STATUS_SUCCESS;
    struct connsys_func *dev_func = &g_connsys_func;

    connsys_bus_get_bus(dev_func);
    *value = connsys_bus_readl(dev_func, addr, &ret);
    connsys_bus_release_bus(dev_func);

    /* ret never be non-zero
    	if (ret)
        {
            LOG_E(connsys, "<<%s>> Read register 0x%08x failed. Error = %d\n",
                __FUNCTION__,
                (unsigned int)addr,
                (int)ret);
    	}
    	*/

    return ret;
}

int32_t connsys_cr_write(uint32_t addr, uint32_t value)
{
    int32_t ret = CONNSYS_STATUS_SUCCESS;
    struct connsys_func *dev_func = &g_connsys_func;

    connsys_bus_get_bus(dev_func);
    connsys_bus_writel(dev_func, value, addr, &ret);
    connsys_bus_release_bus(dev_func);

    /* comment this statement because ret never be non-zero value
        if (ret)
        {
            LOG_E(connsys, "<<%s>> Write register 0x%08x failed. Error = %d\n",
                __FUNCTION__,
                (unsigned int)addr,
                (int)ret);
        }
    */
    return ret;
}

/*
   use to read data
   @ addr:  WRDR0 / WRDR1/ WHISR
*/
int32_t connsys_fifo_read(uint32_t addr, uint8_t *buf, size_t size)
{
    int32_t ret = CONNSYS_STATUS_SUCCESS;
    struct connsys_func *dev_func = &g_connsys_func;

    ret = connsys_bus_read_port(dev_func, buf, addr, size);
    if (ret) {
        LOG_E(connsys, "<<%s>> SDIO read data failed. Error = %d \n", __FUNCTION__, (int)ret);
        ret = CONNSYS_STATUS_FAIL;
    }
    return ret;
}

/*use to write data*/
int32_t connsys_fifo_write(uint8_t *buf, size_t size)
{
    int32_t ret = CONNSYS_STATUS_SUCCESS;
    struct connsys_func *dev_func = &g_connsys_func;
    ret = connsys_bus_write_port(dev_func, WTDR1, buf, size);
    return ret;
}

void connsys_driver_interrupt(struct connsys_func *func)
{
    (void) func; // avoid compiler warning
    if (connsys_ops && connsys_ops->enqueue_bottom_half) {
        connsys_ops->enqueue_bottom_half(0, (PNETFUNC)connsys_util_intr_enhance_mode_receive_data);
    }

    return;
}

#if (CFG_CONNSYS_POLLING_IRQ_EN == 1)
uint32_t connsys_polling_irq(int32_t max_tries)
{
    int32_t count = 0;

    LOG_I(connsys, "Waiting for IRQ...\n");

    while (!g_connsys_func.irq_info.irq_assert) {

        //g_connsys_func.irq_handler(&g_connsys_func);
        if (count++ > max_tries) {
            break;
        }
        cos_delay_time(CONNSYS_POLLING_DELAY_TIME);; //delay 50us
    }

    if (g_connsys_func.irq_info.irq_assert) {
        g_connsys_func.irq_info.irq_assert = 0;
        LOG_I(connsys, "WHISR: 0x%x\n", (unsigned int)g_connsys_func.irq_info.irq_data.WHISR_reg_val);
        return 1; //g_connsys_func.irq_info.irq_data.WHISR_reg_val;
    }
    LOG_I(connsys, "==> No interrupt for %d loops.FUN[%d](0x%p) irq_asset=%d\n",
          (int)count,
          (int)g_connsys_func.num,
          (void *)&g_connsys_func,
          (int)g_connsys_func.irq_info.irq_assert);
    return 0;
}
#endif

uint8_t connsys_giveup_ownership(void)
{
    uint32_t value = 0, counter = 0;
    int32_t ret = 0;
    uint8_t status = TRUE;

    // 1. check if it is already driver own
    ret = connsys_cr_read(WHLPCR, &value);
    if (ret) {
        LOG_E(connsys, "Ownership read failed.\n");
        goto err;
    }

    if (GET_W_FW_OWN_REQ_SET(value)) {
        // give up owner ship
        value |= W_FW_OWN_REQ_SET;
        ret = connsys_cr_write(WHLPCR, value);
        if (ret) {
            LOG_E(connsys, "request owner ship write fail\n");
            goto err;
        }

        // check if the ownership in FW
        counter = 0;
        while (GET_W_FW_OWN_REQ_SET(value)) {
            if (counter > 200000) { // wait for at least 1 second
                status = FALSE;
                break;
            }
            //udelay(50); //delay 50us

            ret = connsys_cr_read(WHLPCR, &value);
            if (ret) {
                goto err;
            }
            counter++;
        }
    }

err:
    /*io read/write fail*/
    if (ret) {
        status = FALSE;
    }
    LOG_I(connsys, "Give up Own Bit, succ(%d), value(0x%x)\n", (int)status, (int)value);
    return status;
}

uint8_t connsys_get_ownership(void)
{
    uint32_t value = 0, counter = 0;
    int32_t ret = 0;
    uint8_t status = TRUE;

    // 1. check if it is already driver own

    ret = connsys_cr_read(WHLPCR, &value);
    if (ret)
    {
        LOG_E(connsys, "Ownership is already driver\n");
        goto err;
    }

    if (!GET_W_FW_OWN_REQ_SET(value)) {
        // request owner ship
        value |= W_FW_OWN_REQ_CLR;
        ret = connsys_cr_write(WHLPCR, value);
        if (ret)
        {
            LOG_E(connsys, "request owner ship write fail\n");
            goto err;
        }

        // check if the ownership back
        counter = 0;
        while (!GET_W_FW_OWN_REQ_SET(value)) {
            if (counter > 2000000) { // wait for at least 1 second
                status = FALSE;
                break;
            }
            //udelay(50); //delay 50us

            ret = connsys_cr_read(WHLPCR, &value);
            if (ret)
            {
                goto err;
            }
            counter++;
        }
    }

err:
    /*io read/write fail*/
    if (ret) {
        status = FALSE;
    }

    return status;
}

int32_t connsys_abnormal_interrupt_check(uint32_t value)
{
    uint32_t reason;
    int32_t err = CONNSYS_STATUS_SUCCESS;

    if (value & ABNORMAL_INT) {
        err = connsys_cr_read(WASR, &reason);
        if (err) {
            LOG_E(connsys, "<<%s>> Read WASR failed. Error = %d.\n", __FUNCTION__, (int)err);
        } else {
            LOG_I(connsys, "<<%s>> Read WASR = %08x.\n", __FUNCTION__, (unsigned int)reason);
        }
    }
    return err;
}

int32_t connsys_dump_n9_interrupt_check(uint32_t value)
{
    uint32_t reason;
    int32_t err = CONNSYS_STATUS_SUCCESS;

    if (value & DUPM_N9_QUERY_STATUS_INT) {
        err = connsys_cr_read(WASR, &reason);
        if (err) {
            LOG_E(connsys, "<<%s>> Read WASR failed. Error = %d.\n", __FUNCTION__, (int)err);
        } else {
            LOG_I(connsys, "<<%s>> Read WASR = %08x.\n", __FUNCTION__, (unsigned int)reason);
        }
    }
    return err;
}

/*
void connsys_irq_handler(hal_nvic_irq_t irq_number)
{
    uint8_t input[LEN_INT_ENHANCE_MODE] __attribute__ ((aligned (4)));
    uint32_t flags;
    enhance_mode_data_struct_t *p_int_enhance;
#if (CONNSYS_DEBUG_MODE_EN == 1)
    g_connsys_stat.number_of_int ++;
#endif
    connsys_disable_interrupt();

#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
    connsys_measure_time_init();
#endif
    connsys_disable_whier_trx_int();

    memset(input, 0, sizeof(input));
    connsys_fifo_read(WHISR, (uint8_t *)input, LEN_INT_ENHANCE_MODE);
    p_int_enhance = (enhance_mode_data_struct_t *)input;

    if (p_int_enhance->WHISR_reg_val == 0)
    {
        LOG_E(connsys, "WHISR=0x%x, abort...\n", (unsigned int)p_int_enhance->WHISR_reg_val);
        connsys_cr_write(WHLPCR, W_INT_EN_SET);
        connsys_enable_interrupt();
        return;
    }

#if (CONNSYS_DEBUG_MODE_EN == 1)
    if ((p_int_enhance->WHISR_reg_val) & ABNORMAL_INT)
	{
		uint32_t reg1, reg2;
		connsys_cr_read(WASR, &reg1);
		connsys_cr_read(0xE0, &reg2);
        local_irq_save(flags);
        g_connsys_stat.number_of_abnormal_int ++;
        local_irq_restore(flags);
		//LOG_E(connsys, "Abnormal(0x%x), 0xE0(0x%x)\n", (unsigned int)reg1, (unsigned int)reg2);
	}

    if ((p_int_enhance->WHISR_reg_val) & FW_OWN_BACK_INT)
    {
		uint32_t reg1;
		connsys_cr_read(WASR, &reg1);
        local_irq_save(flags);
        g_connsys_stat.number_of_fw_own_back ++;
        local_irq_restore(flags);
		//LOG_E(connsys, "FW_OWN_BACK_INT: WASR(0x%x)\n", (unsigned int)reg1);
    }
#endif

    if ((p_int_enhance->WHISR_reg_val) & TX_DONE_INT)
    {
#if (CONNSYS_DEBUG_MODE_EN == 1)
        g_connsys_stat.num_of_tx_int ++;
#endif
        connsys_tx_flow_control_update_free_page_cnt(p_int_enhance);
    }

    if ((p_int_enhance->WHISR_reg_val) & (RX0_DONE_INT | RX1_DONE_INT))
    {
        if (((p_int_enhance->WHISR_reg_val) & TX_DONE_INT) == 0)
        {
            connsys_tx_flow_control_update_free_page_cnt(p_int_enhance);
        }
#if (CONNSYS_DEBUG_MODE_EN == 1)
        g_connsys_stat.num_of_rx_int ++;
#endif
        local_irq_save(flags);
        memcpy(&g_last_enhance_mode_data_struct, input, LEN_INT_ENHANCE_MODE);
#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
        g_balance_ctr.fg_rx_wait = TRUE;
#endif
        local_irq_restore(flags);
        g_connsys_func.irq_callback(&g_connsys_func);
    }
    else
    {
        connsys_enalbe_whier_rx_int();
    }
    connsys_enalbe_whier_tx_int();
    connsys_enable_interrupt();
#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
    connsys_measure_time_set(1);
#endif
    return;
}
*/


void connsys_irq_handler(hal_nvic_irq_t irq_number)
{
    // 0. Disable HIF TX/RX Interrupt
    connsys_disable_interrupt();       //connsys_cr_write(WHLPCR, W_INT_EN_CLR)
    connsys_disable_whier_trx_int();   //WHIER, val &= ~(TX_DONE_INT_EN | RX0_DONE_INT_EN | RX1_DONE_INT_EN);
    connsys_disable_whier_dump_n9_int();

    // 1. Record Debug Information if need
#if (CONNSYS_DEBUG_MODE_EN == 1)
    g_connsys_stat.number_of_int ++;
#endif

#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
    connsys_measure_time_init();
#endif

    // 2. Notify HIF Packet Handler
    g_connsys_func.irq_callback(&g_connsys_func);  //Add to netjob task

    // 3. Enable Interrupt - It will be implemented by netjob callback (TX/RX Packet Handler)

#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
    connsys_measure_time_set(1);
#endif

    return;
}


int32_t connsys_enable_enhance_mode(void)
{
    uint32_t reg_value = 0;

    /* enable RX enhance mode */

    LOG_I(connsys, "enable connsys enhance mode.\n");

    if (connsys_cr_read(WHCR, &reg_value)) {
        LOG_E(connsys, "FAIL. read WHCR.\n");
        return -1;
    }
//    reg_value |= W_INT_CLR_CTRL;
    reg_value &= ~MAX_HIF_RX_LEN_NUM_MASK;
    reg_value |= MAX_HIF_RX_LEN_NUM(CFG_MAX_HIF_RX_LEN_NUM);
//    reg_value |= RX_ENHANCE_MODE;

#if (CFG_RPT_OWN_RX_PACKET_LEN_EN == 1)
    reg_value |= RPT_OWN_RX_PACKET_LEN;
#endif

    LOG_I(connsys, "write 0x%x to WHCR\n", (unsigned int)reg_value);

    if (connsys_cr_write(WHCR, reg_value)) {
        LOG_E(connsys, "FAIL. write WHCR.\n");
        return -1;
    }
    reg_value = 0;
    if (connsys_cr_read(WHCR, &reg_value)) {
        LOG_E(connsys, "FAIL. read WHCR.\n");
        return -1;
    } else {
#if 0
        if (!(reg_value & RX_ENHANCE_MODE)) {
            LOG_E(connsys, "FAIL. write RX_ENHANCE_MODE fail. WHCR = 0x%08x.\n", (unsigned int)reg_value);
            return -1;
        }
#endif
    }
//    LOG_I(connsys, "Enable enhance mode, WHCR=0x%x\n", (unsigned int)reg_value);

#if (CFG_RPT_OWN_RX_PACKET_LEN_EN == 1)
    reg_value = (RX0_RPT_PKT_LEN(CFG_RX0_RPT_PKT_LEN) |
                 (RX1_RPT_PKT_LEN(CFG_RX1_RPT_PKT_LEN);
    if (connsys_cr_write(WPLRCR, reg_value)) {
    LOG_E(connsys, "FAIL. write WPLRCR.\n");
        return -1;
    }
#endif
    return 0;
}

#if (CFG_WIFI_HIF_GDMA_EN == 1)
void connsys_enable_dma(void)
{
    LOG_I(connsys, "==>connsys_enable_dma\n");
    g_connsys_func.use_dma = 1;
}
#endif
void connsys_dma_callback()
{
    //printf("DMA callback\n"); //No any printf in isr
    LOG_I(connsys, "DMA callback\n");
}

int32_t connsys_open()
{
    struct connsys_func *func = &g_connsys_func;
    int32_t  ret = CONNSYS_STATUS_SUCCESS;

#if defined(MTK_HAL_LOWPOWER_ENABLE)
    ptr_lp_connsys_get_own_enable_int = lp_connsys_get_own_enable_int;
    ptr_lp_connsys_give_n9_own = lp_connsys_give_n9_own;
    ptr_connsys_get_ownership = connsys_get_ownership;
#endif

    g_connsys_func.blksize = MY_CONNSYS_BLOCK_SIZE;
    g_connsys_func.num = SDIO_GEN3_FUNCTION_WIFI;
    g_connsys_func.irq_callback = NULL;
#if (CFG_WIFI_HIF_GDMA_EN == 1)
    g_connsys_func.use_dma = 1;
#else
    g_connsys_func.use_dma = 0;
#endif
    // Enable GDMA
    if (g_connsys_func.use_dma) {
        DMA_Init();
        DMA_Register(CONNSYS_GDMA_CH, connsys_dma_callback);
    }

    LOG_I(connsys, "============> SDIO open, (0x%p) use DMA(%d)\n",
          &g_connsys_func, (int)g_connsys_func.use_dma);

    // function enable
    connsys_bus_get_bus(func);
    ret = connsys_bus_enable_func(func);
    connsys_bus_release_bus(func);
    if (ret) {
        LOG_E(connsys, "<<%s>> Enable function failed. Error = %d.\n", __FUNCTION__, (int)ret);
        goto err;
    }

    // set block size
    connsys_bus_get_bus(func);
    ret = connsys_bus_set_block_size(func, func->blksize);
    connsys_bus_release_bus(func);

    if (ret) {
        LOG_E(connsys, "<<%s>> Set block size failed. Error = %d.\n", __FUNCTION__, (int)ret);
        goto err;
    }

    // register sdio irq
    connsys_bus_get_bus(func);
    ret = connsys_bus_get_irq(func, &connsys_driver_interrupt); /* Interrupt IRQ handler */
    connsys_bus_release_bus(func);
    if (ret) {
        LOG_E(connsys, "<<%s>> Claim irq failed. Error = %d.\n", __FUNCTION__, (int)ret);
        goto err;
    }

    // register to MCU IRQ
    hal_nvic_register_isr_handler(CM4_HIF_IRQ, connsys_irq_handler);
    NVIC_SetPriority(CM4_HIF_IRQ, CM4_HIF_PRI);
    NVIC_EnableIRQ(CM4_HIF_IRQ);

    connsys_giveup_ownership();

    if (connsys_get_ownership() == FALSE) {
        LOG_E(connsys, "connsys_get_ownership failed.\n");
        ret = -1;
        goto err;
    }
    if (connsys_enable_enhance_mode() != 0) {
        LOG_E(connsys, "connsys_enable_enhance_mode failed.\n");
        ret = -1;
        goto err;
    }
    connsys_tx_flow_control_init();

#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
    connnsys_balance_init();
#endif

    if (connsys_cr_write(WHIER, (RX0_DONE_INT_EN | RX1_DONE_INT_EN))) {
        LOG_E(connsys, "FAIL. write WHIER failed (1).\n");
        ret = CONNSYS_STATUS_FAIL;
        goto err;
    } else {
        uint32_t reg_value = 0;
        connsys_cr_read(WHISR, &reg_value);
        if ((reg_value & (RX0_DONE_INT_EN | RX1_DONE_INT_EN))) {
            LOG_E(connsys, "FAIL. WHISR.RX0/1_DONE interrupt should be cleared first. (2). WHISR = 0x%08x.\n", (unsigned int)reg_value);
            ret = CONNSYS_STATUS_FAIL;
            goto err;
        }
        reg_value = 0;
        connsys_cr_read(WHIER, &reg_value);
        if (!(reg_value & (RX0_DONE_INT_EN | RX1_DONE_INT_EN))) {
            LOG_E(connsys, "FAIL. write WHIER failed (2). WHIER = 0x%08x.\n", (unsigned int)reg_value);
            ret = CONNSYS_STATUS_FAIL;
            goto err;
        }
    }
err:
    return ret;
}

int32_t connsys_close()
{
    struct connsys_func *func = &g_connsys_func;
    int32_t ret = CONNSYS_STATUS_SUCCESS;


    if (g_connsys_func.use_dma) {
        DMA_UnRegister(CONNSYS_GDMA_CH);
    }

    // release sdio irq
    connsys_bus_get_bus(func);
    ret = connsys_bus_release_irq(func);
    connsys_bus_release_bus(func);
    if (ret) {
        LOG_E(connsys, "<<%s>> Release irq failed. Error = %d.\n", __FUNCTION__, (int)ret);
    }

    // disable function
    connsys_bus_get_bus(func);
    ret =  connsys_bus_disable_func(func);
    connsys_bus_release_bus(func);
    if (ret) {
        LOG_E(connsys, "<<%s>> Disable function failed. Error = %d.\n", __FUNCTION__, (int)ret);
    }

    LOG_I(connsys, "<<%s>> Yes. Release Done.\n", __FUNCTION__);

    return ret;
}

int32_t connsys_disable_interrupt(void)
{
    int32_t ret;
    ret = connsys_cr_write(WHLPCR, W_INT_EN_CLR);
    return ret;
}

int32_t connsys_enable_interrupt(void)
{
    connsys_cr_write(WHLPCR, W_INT_EN_SET);
    return CONNSYS_STATUS_SUCCESS;
}

int32_t connsys_disable_dump_n9_clr(void)
{
    int32_t ret;
    ret = connsys_cr_write(WSICR, NUMP_N9_TRIGGER_INT_CLR);
    return ret;
}

int32_t connsys_enable_dump_n9_set(void)
{
    connsys_cr_write(WSICR, NUMP_N9_TRIGGER_INT_SET);
    return CONNSYS_STATUS_SUCCESS;
}

void connsys_disable_whier_trx_int(void)
{
    uint32_t flags;
    uint32_t val;
    local_irq_save(flags);
    connsys_cr_read(WHIER, &val);
    val &= ~(TX_DONE_INT_EN | RX0_DONE_INT_EN | RX1_DONE_INT_EN);
    connsys_cr_write(WHIER, val);
    local_irq_restore(flags);
}

void connsys_enalbe_whier_rx_int(void)
{
    uint32_t flags;
    uint32_t val;
    local_irq_save(flags);
    connsys_cr_read(WHIER, &val);
    val |= (RX0_DONE_INT_EN | RX1_DONE_INT_EN);
    connsys_cr_write(WHIER, val);
    local_irq_restore(flags);
}

void connsys_enalbe_whier_tx_int(void)
{
    uint32_t flags;
    uint32_t val;
    local_irq_save(flags);
    connsys_cr_read(WHIER, &val);
    val |= (TX_DONE_INT_EN);
    connsys_cr_write(WHIER, val);
    local_irq_restore(flags);
}

void connsys_enable_whier_dump_n9_int(void)
{
    uint32_t flags;
    uint32_t val;
    local_irq_save(flags);
    connsys_cr_read(WHIER, &val);
    val |= (DUMP_N9_INT_EN);
    connsys_cr_write(WHIER, val);
    local_irq_restore(flags);
}

void connsys_disable_whier_dump_n9_int(void)
{
    uint32_t flags;
    uint32_t val;
    local_irq_save(flags);
    connsys_cr_read(WHIER, &val);
    val &= ~(DUMP_N9_INT_EN);
    connsys_cr_write(WHIER, val);
    local_irq_restore(flags);
}

void connsys_tx_flow_control_init(void)
{
    uint32_t flags;
    local_irq_save(flags);
    memset(&g_hif_tx_flow_control_stat, 0, sizeof(wifi_hif_tx_flow_control_t));
    g_hif_tx_flow_control_stat.reserve_quota_page_cnt = DEFAULT_N9_PSE_PAGE_QUOTA;
    g_hif_tx_flow_control_stat.page_sz = DEFAULT_N9_PSE_PAGE_SIZE;
    g_hif_tx_flow_control_stat.available_page_cnt = DEFAULT_N9_PSE_PAGE_QUOTA;
    g_hif_tx_flow_ctrl_en = 1;
    local_irq_restore(flags);
}

uint32_t connsys_tx_flow_control_get_page_size(void)
{
    uint32_t flags;
    uint32_t page_size;
    local_irq_save(flags);
    page_size = g_hif_tx_flow_control_stat.page_sz;
    local_irq_restore(flags);
    return page_size;
}



void connsys_tx_flow_control_update_free_page_cnt(void)
{
    uint32_t  flags;
    uint32_t  cr_wtqcr7;
    wifi_hif_tx_flow_control_t *ctrl = &g_hif_tx_flow_control_stat;

    // Disable Interrupt
    local_irq_save(flags);

    // Read WLAN TXQ Count Register 7 (For N9 only use this now)
    connsys_cr_read(WTQCR7, &cr_wtqcr7);

#if (CONNSYS_DEBUG_MODE_EN == 1)
    ctrl->free_page_cnt_by_wifi_txq[WIFI_TXQ_CNT_IDX_14_TXCFFA] += (cr_wtqcr7 & 0xffff);
    ctrl->free_page_cnt_by_wifi_txq[WIFI_TXQ_CNT_IDX_15_TXCCPU] += ((cr_wtqcr7 & (0xffffU << 16U)) >> 16U);
    ctrl->total_free_page_cnt += (cr_wtqcr7 & 0xffff);
#endif

    // Update Current Page Count
    ctrl->current_page_cnt -= (cr_wtqcr7 & 0xffff);

    // Update Available Page Count
    ctrl->available_page_cnt = ctrl->reserve_quota_page_cnt - ctrl->current_page_cnt;

    // Enable Interrupt
    local_irq_restore(flags);
}

int32_t connsys_tx_flow_control_check_and_update_tx(int32_t port, uint32_t pkt_len)
{
    uint32_t flags;
    uint32_t send_page = 0;
    int32_t ret = CONNSYS_STATUS_SUCCESS;
    wifi_hif_tx_flow_control_t *ctrl = &g_hif_tx_flow_control_stat;

    //added by guofu
    if (0 == ctrl->page_sz) {
        return CONNSYS_STATUS_FAIL;
    }

    local_irq_save(flags);
    send_page = pkt_len / ctrl->page_sz;

    if ((pkt_len % ctrl->page_sz) > 0) {
        send_page ++;
    }

#if (CONNSYS_DEBUG_MODE_EN == 1)
    if (g_hif_tx_flow_ctrl_en) {
        if (send_page <= ctrl->available_page_cnt) {
            ctrl->send_page_cnt_by_tx_port[port] += send_page;
            ctrl->total_send_page_cnt += send_page;
            ctrl->send_pkt_cnt_by_tx_port[port] ++;
            ctrl->total_send_pkt_cnt ++;

            ctrl->current_page_cnt += send_page;

            if (ctrl->max_page_cnt < ctrl->current_page_cnt) {
                ctrl->max_page_cnt = ctrl->current_page_cnt;
            }

            ctrl->available_page_cnt =
                ctrl->reserve_quota_page_cnt - ctrl->current_page_cnt;

            ret = CONNSYS_STATUS_SUCCESS;
        } else {
            ctrl->total_drop_pkt_cnt ++;
            ret = CONNSYS_STATUS_FAIL;
        }
    } else {
        ctrl->send_page_cnt_by_tx_port[port] += send_page;
        ctrl->total_send_page_cnt += send_page;
        ctrl->send_pkt_cnt_by_tx_port[port] ++;
        ctrl->total_send_pkt_cnt ++;

        ctrl->current_page_cnt += send_page;

        if (ctrl->max_page_cnt < ctrl->current_page_cnt) {
            ctrl->max_page_cnt = ctrl->current_page_cnt;
        }

        ctrl->available_page_cnt =
            ctrl->reserve_quota_page_cnt - ctrl->current_page_cnt;
        ret = CONNSYS_STATUS_SUCCESS;
    }
#else
    if (send_page <= ctrl->available_page_cnt) {
        ctrl->current_page_cnt += send_page;
        ctrl->available_page_cnt =
            ctrl->reserve_quota_page_cnt - ctrl->current_page_cnt;
        ret = CONNSYS_STATUS_SUCCESS;
    } else {
        ctrl->total_drop_pkt_cnt ++;
        ret = CONNSYS_STATUS_FAIL;
    }
#endif /* (CONNSYS_DEBUG_MODE_EN == 1) */

    local_irq_restore(flags);

    return ret;
}

void connsys_tx_flow_control_config(uint8_t hif_tx_flow_ctrl_en)
{
    uint32_t flags;
    LOG_I(connsys, "==>connsys_tx_flow_control_config, hif_tx_flow_ctrl_en = %u\n",
          (unsigned int)hif_tx_flow_ctrl_en);
    local_irq_save(flags);
    g_hif_tx_flow_ctrl_en = hif_tx_flow_ctrl_en;
    local_irq_restore(flags);
}

void connsys_tx_flow_control_set_reserve_page(uint32_t reserve_page)
{
    uint32_t flags;
    wifi_hif_tx_flow_control_t *ctrl = &g_hif_tx_flow_control_stat;
    LOG_I(connsys, "==>connsys_tx_flow_control_set_reserve_page, reserve_page = %u\n",
          (unsigned int)reserve_page);
    local_irq_save(flags);
    ctrl->reserve_quota_page_cnt = reserve_page;
    ctrl->available_page_cnt =
        ctrl->reserve_quota_page_cnt - ctrl->current_page_cnt;
    local_irq_restore(flags);
}


void connsys_tx_flow_control_set_reserve_page_by_cr(void)
{
    uint32_t flags;
    wifi_hif_tx_flow_control_t *ctrl = &g_hif_tx_flow_control_stat;
    local_irq_save(flags);
    ctrl->reserve_quota_page_cnt = connsys_util_pse_get_p0_min_resv();
    ctrl->available_page_cnt =
        ctrl->reserve_quota_page_cnt - ctrl->current_page_cnt;
    local_irq_restore(flags);
}


void connsys_tx_flow_control_show_info(void)
{
    uint32_t flags;
    uint32_t port;
    uint32_t txq_idx;
    wifi_hif_tx_flow_control_t wifi_hif_tx_flow_control;
    local_irq_save(flags);
    memcpy(&wifi_hif_tx_flow_control,
           &g_hif_tx_flow_control_stat,
           sizeof(wifi_hif_tx_flow_control_t));
    local_irq_restore(flags);

    printf("==>connsys_tx_flow_control_show_info\n");


    printf("total_send_pkt_cnt: %u\n", (unsigned int)wifi_hif_tx_flow_control.total_send_pkt_cnt);
    printf("send_pkt_cnt_by_tx_port: \n");
    for (port = 0; port < NUM_OF_WIFI_HIF_TX_PORT; port++) {
        printf("\t[%u]: %u\n",
               (unsigned int)port ,
               (unsigned int)wifi_hif_tx_flow_control.send_pkt_cnt_by_tx_port[port]);
    }

    printf("total_send_page_cnt: %u\n", (unsigned int)wifi_hif_tx_flow_control.total_send_page_cnt);
    printf("send_page_cnt_by_tx_port: \n");
    for (port = 0; port < NUM_OF_WIFI_HIF_TX_PORT; port++) {
        printf("\t[%u]: %u\n",
               (unsigned int)port,
               (unsigned int)wifi_hif_tx_flow_control.send_page_cnt_by_tx_port[port]);
    }


    printf("total_free_page_cnt: %u\n", (unsigned int)wifi_hif_tx_flow_control.total_free_page_cnt);
    printf("free_page_cnt_by_wifi_txq: \n");
    for (txq_idx = 0; txq_idx < NUM_OF_WIFI_TXQ; txq_idx ++) {
        printf("[%2u]:%8u\t",
               (unsigned int)txq_idx,
               (unsigned int)wifi_hif_tx_flow_control.free_page_cnt_by_wifi_txq[txq_idx]);
        if ((txq_idx % 4) == 3) {
            printf("\n");
        }
    }
    printf("available_page_cnt: %u\n", (int)wifi_hif_tx_flow_control.available_page_cnt);
    printf("current_page_cnt: %d\n", (int)wifi_hif_tx_flow_control.current_page_cnt);
    printf("max_page_cnt: %d\n", (int)wifi_hif_tx_flow_control.max_page_cnt);
    printf("reserve_quota_page_cnt: %u\n", (unsigned int)wifi_hif_tx_flow_control.reserve_quota_page_cnt);
    printf("page_sz: %u\n", (unsigned int)wifi_hif_tx_flow_control.page_sz);
    printf("total_drop_pkt_cnt: %u\n", (unsigned int)wifi_hif_tx_flow_control.total_drop_pkt_cnt);
    printf("g_hif_tx_flow_ctrl_en: %u\n", (unsigned int)g_hif_tx_flow_ctrl_en);
}

void connsys_show_config_option(void)
{
    printf("==>connsys_show_config_option\n");
    printf("CONNSYS_MAX_RX_PKT_SIZE = %u\n", CONNSYS_MAX_RX_PKT_SIZE);
    printf("MY_CONNSYS_BLOCK_SIZE = %u\n", MY_CONNSYS_BLOCK_SIZE);
    printf("CFG_WIFI_HIF_GDMA_EN = %u\n", CFG_WIFI_HIF_GDMA_EN);
    printf("CFG_CONNSYS_IOT_RX_ZERO_COPY_EN = %u\n", CFG_CONNSYS_IOT_RX_ZERO_COPY_EN);
    printf("CFG_CONNSYS_IOT_TX_ZERO_COPY_EN = %u\n", CFG_CONNSYS_IOT_TX_ZERO_COPY_EN);
}

/* Extra headroom lenght when N9 zero copy is enabled. */
static uint8_t EXTRA_HEADROOM_LEN_FOR_NON_QOS      = 38;
static uint8_t EXTRA_HEADROOM_LEN_FOR_QOS_ENABLE   = 42;

uint32_t connsys_get_headroom_offset(uint8_t qos_enable)
{
    if (qos_enable) {
        return EXTRA_HEADROOM_LEN_FOR_QOS_ENABLE;
    }

    return EXTRA_HEADROOM_LEN_FOR_NON_QOS;
}

void connsys_set_headroom_offset(uint8_t qos_enable, uint8_t offset)
{
    if (qos_enable) {
        EXTRA_HEADROOM_LEN_FOR_QOS_ENABLE = offset;
    } else {
        EXTRA_HEADROOM_LEN_FOR_NON_QOS = offset;
    }

}

int8_t lp_connsys_get_own_enable_int()
{
    uint32_t flags;
    uint32_t reg_value;

    if (connsys_get_ownership() == FALSE) {
        LOG_E(connsys, "FAIL get connsys ownership.\n");
        return -1;
    }

    /* enable FW_OWN_BACK_INT interrupt */
    local_irq_save(flags);
    connsys_cr_read(WHIER, &reg_value);
    reg_value |= FW_OWN_BACK_INT_EN;
    connsys_cr_write(WHIER, reg_value);
    local_irq_restore(flags);

    {
        reg_value = 0;
        connsys_cr_read(WHIER, &reg_value);
        if (!(reg_value & FW_OWN_BACK_INT_EN)) {
            LOG_E(connsys, "FAIL. write WHIER failed (2). WHIER = 0x%08x \n", (unsigned int)reg_value);
            return -1;
        }
    }

    /* enable W_INT_EN_SET */
    if (connsys_cr_write(WHLPCR, W_INT_EN_SET)) {
        LOG_E(connsys, "FAIL. write WHLPCR failed.\n");
        return -1;
    }

    return 0;
}


int8_t lp_connsys_give_n9_own()
{
    uint32_t reg_value;

    if (connsys_get_ownership() == FALSE) {
        LOG_E(connsys, "FAIL. get connsys ownership.\n");
        return -1;
    }

    /* set fw own and enable W_INT_EN_SET */
    if (connsys_cr_write(WHLPCR, (W_INT_EN_SET | W_FW_OWN_REQ_SET))) {
        LOG_E(connsys, "FAIL. write WHLPCR failed.\n");
        return -1;
    }

    /* check own is in fw side */
    if (connsys_cr_read(WHLPCR, &reg_value)) {
        LOG_E(connsys, "FAIL. read WHLPCR failed.\n");
        return -1;
    }

    if (GET_W_FW_OWN_REQ_SET(reg_value)) {
        /* driver still have the ownership */
        printf("FAIL. driver still have the ownership.\n");
        return -1;
    }

    return 0;

}

int32_t connsys_init(sys_cfg_t *sys_config)
{
#if defined(MTK_HAL_LOWPOWER_ENABLE)
    if ((hal_lp_get_wic_status()) || (1 == hal_lp_get_wic_wakeup())) {
        /* N9 MUST be active for clock switch and pinmux config*/
        /* Wakeup N9 by connsys ownership */
        connsys_open();
        connsys_close();
    }
#endif
    connsys_set_headroom_offset(CONNSYS_HEADROOM_OFFSET_QOS, 58);
    connsys_set_headroom_offset(CONNSYS_HEADROOM_OFFSET_NON_QOS, 54);

    connsys_set_wifi_profile(sys_config);

    connsys_open();

#if defined(MTK_HAL_LOWPOWER_ENABLE)
    if ((hal_lp_get_wic_status() == 0) && (0 == hal_lp_get_wic_wakeup())) {
        /* NO need to reload N9 patch and FW after wakeup from sleep */
#endif
        connsys_util_firmware_download();
#if defined(MTK_HAL_LOWPOWER_ENABLE)
    }
#endif

#if (CFG_WIFI_HIF_GDMA_EN == 1)
    connsys_enable_dma();
    /* Enable LWIP DMA copy */
    if (hal_gdma_init(HAL_GDMA_CHANNEL_0) != HAL_GDMA_STATUS_OK) {
        LOG_W(connsys, "WARN! LWIP DMA data copy disabled..\n");
    }

    //hal_gdma_deinit();   // TODO?
#endif /* CFG_WIFI_HIF_GDMA_EN */

    connsys_tx_flow_control_set_reserve_page_by_cr();
    connsys_enable_interrupt();

#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
    g_balance_ctr.rx_handle = NetJobGetTaskId();
#endif

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE) 
    context_dump_init();
#endif

    return 0;
}
