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
#include <stdlib.h>
#include <string.h>
#include "type_def.h"
#include "mem_util.h"
#include "mt_cmd_fmt.h"
#include "connsys_driver.h"
#include "connsys_bus.h"
#include "connsys_util.h"
#include "connsys_adapter.h"
#include "sfc.h"
#include "mt7687.h"
#include "nvic.h"
#include "_mtk_hal_debug.h"
#include "dma_sw.h"
#include "dma_hw.h"
#include "misc.h"
#include "toi.h"
#include "memory_attribute.h"
#include "connsys_profile.h"
#include "hal_log.h"
#include "syslog.h"
#include "wifi_rx_desc.h"
#include "hal_flash.h"
#include "flash_map.h"
#include "os.h"

#ifdef  MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE
#include "context_dump.h"
extern uint8_t          g_dump_param_ctl;
#endif

#define CFG_FPGA 0

sys_cfg_t *g_wifi_profile = NULL;

uint32_t connsys_fw_download_CmdAddressLenReq(uint32_t addr, uint32_t len, uint32_t data_mode, uint8_t *buffer);
int32_t connsys_pda_config(uint32_t dest_addr, uint32_t data_len, uint32_t data_mode);

//+++++++ Firmware Download +++++++++//
#if defined(__ICCARM__)
ATTR_4BYTE_ALIGN uint8_t g_tx_buf[MAX_BUF_SIZE + 0x10];
#else
uint8_t g_tx_buf[MAX_BUF_SIZE + 0x10] __attribute__((aligned(4)));
#endif

#define WIFI_PROFILE_LEN                (0x800) // 2K
#define WIFI_PROFILE_ADDR               (0x020B2000)
#define WIFI_PROFILE_DATA_MODE          (0x80000000) // plain (non-encrypted)
#define WIFI_PROFILE_KEY_INDEX          (0x0)

wifi_rx_handler_t connsys_raw_handler = NULL;

void connsys_get_ilm_image_info(fw_dl_data_t *output, ssize_t length)
{


    fw_image_tailer_t *info;
    info = (fw_image_tailer_t *) & (output->image[length - sizeof(fw_image_tailer_t)]);

    LOG_I(connsys, "ilm_addr = %08x, ilm_len = %x, feature_set = %d.\n",
          (unsigned int)info->ilm_info.addr,
          (unsigned int)info->ilm_info.len,
          (int)info->ilm_info.feature_set);
    LOG_I(connsys, "dlm_addr = %08x, dlm_len = %x, feature_set = %d.\n",
          (unsigned int)info->dlm_info.addr,
          (unsigned int)info->dlm_info.len,
          (int)info->dlm_info.feature_set);

    output->ilm_addr = info->ilm_info.addr;
    output->ilm_len = info->ilm_info.len;
    output->ilm_encrypt = ((info->ilm_info.feature_set) & FEATURE_MASK_ENCRYPT_MODE) & 0xFF;
    output->ilm_encrypt_key_index = ((info->ilm_info.feature_set) & FEATURE_MASK_KEY_INDEX) & 0xFF;

    output->dlm_addr = info->dlm_info.addr;
    output->dlm_len = info->dlm_info.len;
    output->dlm_encrypt = ((info->dlm_info.feature_set) & FEATURE_MASK_ENCRYPT_MODE) & 0xFF;
    output->dlm_encrypt_key_index = ((info->dlm_info.feature_set) & FEATURE_MASK_KEY_INDEX) & 0xFF;
}

static fw_image_tailer_t *n9_fw_info;

void connsys_util_get_n9_fw_ver(char *ver)
{
    n9_fw_info->ilm_info.ram_built_date[0] = ' '; // workaround to remove the prefix: '_'
    os_memcpy(ver, &(n9_fw_info->ilm_info.ram_built_date[1]), 14);
}

#ifdef MTK_SEC_FLASH_SCRAMBLE_ENABLE
extern uint8_t sboot_efuse_sbc_state;
#define SF_SEC_DESSCR 0x00011000
uint32_t addr_SF_SEC_DESSCR = SF_SEC_DESSCR;
extern uint32_t enable_descramble_ex(BOOLEAN enable, uint32_t dma_offset);
#endif

void connsys_get_ilm_image_info_from_flash(fw_dl_data_t *output)
{

    uint32_t length = 0;

#ifdef MTK_SEC_FLASH_SCRAMBLE_ENABLE
    if (sboot_efuse_sbc_state == 1) {
        addr_SF_SEC_DESSCR = SF_SEC_DESSCR + (N9_RAMCODE_BASE >> 16);
        //printf("addr_SF_SEC_DESSCR 0x%x\n",addr_SF_SEC_DESSCR);
        enable_descramble_ex(TRUE, addr_SF_SEC_DESSCR);
    }
#endif

    length = *((uint32_t *) & (output->image[0]));

#ifdef MTK_SEC_FLASH_SCRAMBLE_ENABLE
    if (sboot_efuse_sbc_state == 1) {
        addr_SF_SEC_DESSCR = SF_SEC_DESSCR + ((N9_RAMCODE_BASE + length - sizeof(fw_image_tailer_t)) >> 16);
        //printf("addr_SF_SEC_DESSCR 0x%x , length-sizeof(fw_image_tailer_t) 0x%x\n",addr_SF_SEC_DESSCR, length-sizeof(fw_image_tailer_t));
        enable_descramble_ex(TRUE, addr_SF_SEC_DESSCR);
    }
#endif

    n9_fw_info = (fw_image_tailer_t *)&(output->image[length-sizeof(fw_image_tailer_t)]);

    // print_content(sizeof(fw_image_tailer_t), (uint8_t *)n9_fw_info, "tailer");
    LOG_HEXDUMP_I(connsys, "tailer", (const char *)n9_fw_info, sizeof(fw_image_tailer_t));

    LOG_I(connsys, "ilm_addr = %08x, ilm_len = %x, feature_set = %d.\n",
          (unsigned int)n9_fw_info->ilm_info.addr,
          (unsigned int)n9_fw_info->ilm_info.len,
          (int)n9_fw_info->ilm_info.feature_set);
    LOG_I(connsys, "dlm_addr = %08x, dlm_len = %x, feature_set = %d.\n",
          (unsigned int)n9_fw_info->dlm_info.addr,
          (unsigned int)n9_fw_info->dlm_info.len,
          (int)n9_fw_info->dlm_info.feature_set);
    LOG_I(connsys, "N9 RAM build date: %s\n", n9_fw_info->ilm_info.ram_built_date);

    output->ilm_addr = n9_fw_info->ilm_info.addr;
    output->ilm_len = n9_fw_info->ilm_info.len;
    output->ilm_encrypt = ((n9_fw_info->ilm_info.feature_set) & FEATURE_MASK_ENCRYPT_MODE) & 0xFF;
    output->ilm_encrypt_key_index = ((n9_fw_info->ilm_info.feature_set) & FEATURE_MASK_KEY_INDEX) & 0xFF;

    output->dlm_addr = n9_fw_info->dlm_info.addr;
    output->dlm_len = n9_fw_info->dlm_info.len;
    output->dlm_encrypt = ((n9_fw_info->dlm_info.feature_set) & FEATURE_MASK_ENCRYPT_MODE) & 0xFF;
    output->dlm_encrypt_key_index = ((n9_fw_info->dlm_info.feature_set) & FEATURE_MASK_KEY_INDEX) & 0xFF;
}

uint32_t connsys_img_direct_download(uint32_t target_addr, uint32_t length, uint32_t src_addr, uint32_t data_mode, uint32_t key_index)
{
    uint32_t len = 0, reg = 0, total = 0;
    uint32_t volatile_reg1 = 0, volatile_reg2 = 0;

    connsys_pda_config(target_addr, length, data_mode);

    volatile_reg1 = REG32(PDA_SOURCE_CON);
    volatile_reg2 = REG32(PDA_DWLD_STATE);
    LOG_I(connsys, "Before Switch PDA source, con=0x%x, state=0x%x\n", volatile_reg1, volatile_reg2);
    // Step2: Set PDA source to PKT-GEN, instead of HIF
    REG32(PDA_SOURCE_CON) |= (PDA_SOURCE_SEL_BIT);
    volatile_reg1 = REG32(PDA_SOURCE_CON);
    volatile_reg2 = REG32(PDA_DWLD_STATE);
    LOG_I(connsys, "After Switch PDA source, con=0x%x, state=0x%x\n", volatile_reg1, volatile_reg2);

    (void)volatile_reg1;
    (void)volatile_reg2;

    while (total < length) {
#ifdef MTK_SEC_FLASH_SCRAMBLE_ENABLE
        if (sboot_efuse_sbc_state == 1) {
            //printf("addr_SF_SEC_DESSCR 0x%x \n",addr_SF_SEC_DESSCR);
            enable_descramble_ex(TRUE, addr_SF_SEC_DESSCR);
        }
#endif
        len = *((uint32_t *)src_addr);

        // Step3: Configure CM4 GDMA

        /* clear start bit  */
        REG32(GDMA_CH2_START) &= ~(0x1 << 15); /* clear start DMA bit */

        // Enable GDMA Channels
//        REG32(0xE000E100) = 1;
        REG32(GDMA_CHANNEL_ENABLE) = 0xFFFFFFFF;
        REG32(0xE000E100) = 2;

        // Configure GDMA : CH2
        REG32(GDMA_CH2_SRC) = src_addr;
        REG32(GDMA_CH2_DST) = PDA_TOP_WRAPPER_PORT;
        REG32(GDMA_CH2_COUNT) = (len >> 2); // by unit of  >>> [DW] <<<<, "len" includes 3DW header
        REG32(GDMA_CH2_CON) = 0x3F00206; // bit[1,2,9,20-25]


        // wait DMA finish
        LOG_I(connsys, "Wait GDMA finishing...\n");

        /* DMA kick out */
        REG32(GDMA_CH2_START) |= (0x1 << 15); /* Start DMA channel*/
        do {
            reg = REG32(GDMA_CH2_RLCT);
        } while (reg != 0x00);
        total += (len - PDA_PKT_HDR_SIZE);
//        LOG_I(connsys, "GDMA move from (0x%x) %d bytes to N9, total %d bytes.\n",
//              (unsigned int)src_addr,
//              (int)len,
//              (int)total);

        src_addr += len; // len includes 3DW header
#ifdef MTK_SEC_FLASH_SCRAMBLE_ENABLE
        if (sboot_efuse_sbc_state == 1) {
            addr_SF_SEC_DESSCR = addr_SF_SEC_DESSCR + 1;
            //printf("addr_SF_SEC_DESSCR 0x%x \n",addr_SF_SEC_DESSCR);
        }
#endif
    }

    // Step5: Switch back PDA source to HIF
    REG32(PDA_SOURCE_CON) &= ~(PDA_SOURCE_SEL_BIT);

    if (total != length) {
        LOG_E(connsys, "ERROR! the total %d bytes != assigned length(%d bytes)\n",
              (int) total, (int)length);
        return 0;
    }

    return src_addr;
}

static int connsys_download_wifi_profile(sys_cfg_t *wifi_profile)
{
    uint8_t err;
    uint8_t *wifi_profile_data;

    wifi_profile_data = os_zalloc(WIFI_PROFILE_LEN);

    os_memcpy(wifi_profile_data, wifi_profile, sizeof(sys_cfg_t));

    /* for debug */
    LOG_HEXDUMP_I(connsys, "wifi_profile_data:", wifi_profile_data, sizeof(sys_cfg_t));
    LOG_I(connsys, "wifi profile length is %d.", sizeof(sys_cfg_t));
#ifdef MTK_WIFI_PRIVILEGE_ENABLE
    LOG_I(connsys,"wifi privilege enable is %d.", wifi_profile->wifi_privilege_enable);
#endif /*MTK_WIFI_PRIVILEGE_ENABLE*/
    err = connsys_util_pda_download(WIFI_PROFILE_ADDR, WIFI_PROFILE_LEN, WIFI_PROFILE_DATA_MODE, 0, (unsigned char *)(wifi_profile_data));
    if (err) {
        LOG_E(connsys, "\n\n\n<<%s>> Send wifi profile download fail!\n\n\n", __FUNCTION__);
    }

    os_free(wifi_profile_data);

    return 1;
}

int32_t connsys_util_fw_direct_download(void)
{
    uint32_t data_mode = 0;
    fw_dl_data_t data;
    uint8_t *ptr = NULL;
#ifdef MTK_FW_DW_BY_CM4
    uint8_t mcu_mode = 1;
#else
    uint8_t mcu_mode = 0;
#endif

    // Step 0: get ownership form firmware
    if (TRUE == connsys_get_ownership()) {
        LOG_I(connsys, "<<%s>> Get ownership ok.\n", __FUNCTION__);
    } else {
        LOG_I(connsys, "<<%s>> Get ownership failed.\n", __FUNCTION__);
    }

    data.image = (uint8_t *)(WIFI_FW_ADDR_IN_FLASH);
    connsys_get_ilm_image_info_from_flash(&data);

    // add 4 byte CRC len
    data.ilm_len += LEN_4_BYTE_CRC;
    data.dlm_len += LEN_4_BYTE_CRC;

    // only support one data mode   (ILM data mode = DLM data mode)
    data_mode = (((data.dlm_encrypt) << DATA_MODE_BIT_SHFT_ENCRYPT_MODE) & DATA_MODE_MASK_ENCRYPT_MODE)
                | (((data.dlm_encrypt_key_index) << DATA_MODE_BIT_SHFT_KEY_INDEX) & DATA_MODE_MASK_KEY_INDEX)
                | ((data.dlm_encrypt == 1) ? (((uint32_t)1 << DATA_MODE_BIT_SHFT_RESET_IV) & DATA_MODE_MASK_RESET_IV) : (0))
                | (((uint32_t)1 << DATA_MODE_BIT_SHFT_NEED_ACK) & DATA_MODE_MASK_NEED_ACK);

    LOG_I(connsys, "DLM encrypt(%d), dlm key index(%d)\n",
          (int)data.dlm_encrypt,
          (int)data.dlm_encrypt_key_index);


    // Step3: download ILM
    LOG_I(connsys, "Download ILM (%d bytes to N9 0x%x, mode:0x%x)...\n",
          (int)data.ilm_len, (unsigned int)data.ilm_addr, (unsigned int)data_mode);

    if (mcu_mode) {
        int32_t err = 0;

        // get ownership form firmware
        if (TRUE == connsys_get_ownership()) {
            LOG_I(connsys, "<<%s>> Get ownership ok.\n", __FUNCTION__);
        } else {
            LOG_I(connsys, "<<%s>> Get ownership failed.\n", __FUNCTION__);
        }

        LOG_I(connsys, "<<%s>> ILM: Send fw download command...\n", __FUNCTION__);
        err = connsys_util_pda_download(data.ilm_addr, data.ilm_len, data_mode, 4, data.image);  /* skip total image size, offset=4 */
        if (err) {
            return err;
        }

        LOG_I(connsys, "<<%s>> DLM: Send fw download command...\n", __FUNCTION__);
        err = connsys_util_pda_download(data.dlm_addr, data.dlm_len, data_mode, 4 + data.ilm_len, data.image); /* skip total image size+ILM size, offset=4+ilm_len */
        if (err) {
            return err;
        }
    } else {

#ifdef MTK_SEC_FLASH_SCRAMBLE_ENABLE
        if (sboot_efuse_sbc_state == 1) {
            addr_SF_SEC_DESSCR = SF_SEC_DESSCR + (N9_RAMCODE_BASE >> 16);
            //printf("addr_SF_SEC_DESSCR 0x%x\n",addr_SF_SEC_DESSCR);
            enable_descramble_ex(TRUE, addr_SF_SEC_DESSCR);
        }
#endif

        ptr = &(data.image[4]);    /* skip total image size */
        ptr = (uint8_t *)connsys_img_direct_download(data.ilm_addr, data.ilm_len, (uint32_t)ptr,
                data_mode, data.ilm_encrypt_key_index);
        if (ptr == NULL) {
            LOG_E(connsys, "Download ILM Fail.\n");
            return 0;
        }

        // Step4: download DLM
        LOG_I(connsys, "Download DLM (%d bytes to N9 0x%x, mode:0x%x), ptr=0x%p...\n",
              (int)data.dlm_len, (unsigned int)data.dlm_addr, (unsigned int)data_mode, (void *)ptr);

#ifdef MTK_SEC_FLASH_SCRAMBLE_ENABLE


        uint32_t start_dlm_addr = (uint32_t)ptr;//fix KEIL build error "uint8_t * cannot be used to initialize an entity of type uint32_t"
        if (sboot_efuse_sbc_state == 1) {
            addr_SF_SEC_DESSCR = SF_SEC_DESSCR + ((start_dlm_addr >> 16) & 0xF);
            //printf("addr_SF_SEC_DESSCR 0x%x, start_dlm_addr 0x%x\n",addr_SF_SEC_DESSCR, start_dlm_addr);
            enable_descramble_ex(TRUE, addr_SF_SEC_DESSCR);
        }
#endif

        ptr = (uint8_t *)connsys_img_direct_download(data.dlm_addr, data.dlm_len, (uint32_t)ptr,
                data_mode, data.dlm_encrypt_key_index);
        if (ptr == NULL) {
            LOG_E(connsys, "Download DLM Fail.\n");
            return 0;
        }
    }
    // Step6: Check FW download done, and start FW
    connsys_util_fw_download_done();

    LOG_I(connsys, "Download done.\n");

    return 1;
}

int32_t connsys_util_firmware_download(void)
{
    connsys_util_patch_download();
    return connsys_util_fw_direct_download();
}


void connsys_fw_download(fw_dl_data_t *data)
{
    uint32_t data_mode = 0;
    uint8_t err = 0;

    LOG_I(connsys, "<<%s>> The Bin File , ilm_addr = %08x, ilm_tx_len = %x\n",
          __FUNCTION__,  (unsigned int)data->ilm_addr, (unsigned int)data->ilm_len);

    LOG_I(connsys, "<<%s>> dlm_addr = %08x, dlm_tx_len = %x, enrypt = %d, encrypt_key_index = %d\n",
          __FUNCTION__,
          (unsigned int)data->dlm_addr,
          (unsigned int)data->dlm_len,
          (int)data->dlm_encrypt,
          (int)data->dlm_encrypt_key_index);

    data_mode = (((data->dlm_encrypt) << DATA_MODE_BIT_SHFT_ENCRYPT_MODE) & DATA_MODE_MASK_ENCRYPT_MODE)
                | (((data->dlm_encrypt_key_index) << DATA_MODE_BIT_SHFT_KEY_INDEX) & DATA_MODE_MASK_KEY_INDEX)
                | ((data->dlm_encrypt == 1) ? (((uint32_t)1 << DATA_MODE_BIT_SHFT_RESET_IV) & DATA_MODE_MASK_RESET_IV) : (0))
                | (((uint32_t)1 << DATA_MODE_BIT_SHFT_NEED_ACK) & DATA_MODE_MASK_NEED_ACK);

    LOG_I(connsys, "<<%s>> data_mode = %08x\n", __FUNCTION__, (unsigned int)data_mode);


    // get ownership form firmware
    if (TRUE == connsys_get_ownership()) {
        LOG_I(connsys, "<<%s>> Get ownership ok.\n", __FUNCTION__);
    } else {
        LOG_I(connsys, "<<%s>> Get ownership failed.\n", __FUNCTION__);
    }

    LOG_I(connsys, "<<%s>> ILM: Send fw download command...\n", __FUNCTION__);
    err = connsys_util_pda_download(data->ilm_addr, data->ilm_len, data_mode, 0, data->image);
    if (err) {
        return;
    }

    LOG_I(connsys, "<<%s>> DLM: Send fw download command...\n", __FUNCTION__);
    err = connsys_util_pda_download(data->dlm_addr, data->dlm_len, data_mode, data->ilm_len, data->image);
    if (err) {
        return;
    }
    connsys_util_fw_download_done();
}
//================= Patch Download test ==========================================================//

uint8_t connsys_patch_finish(void)
{
    uint32_t counter = 0;
    uint32_t value; //, offset;
    uint32_t tx_len, cmd_len;
    uint32_t real_rx_len;
    int32_t ret;
    uint8_t wait_tx_done, wait_rx_done, err = 1;
    uint8_t *buffer;

    // Patch finish command
    cmd_len = sizeof(INIT_HIF_TX_HEADER_T);
    tx_len = LEN_SDIO_TX_AGG_WRAPPER(cmd_len);

    buffer = os_zalloc(SDIO_MAX_RW_SIZE);

    // fill in the txd
    connsys_util_create_inband_cmd_txd_rom(cmd_len, buffer, MT_PATCH_FINISH_REQ); //offset=

    // print_content(tx_len, buffer, "patch finish");
    LOG_HEXDUMP_I(connsys, "patch finish", (const char *)buffer, tx_len);

    // send to dut
    LOG_I(connsys, "<<%s>> Send Patch finish command ...\n", __FUNCTION__);
    // write connsys_util_fw_download cmd to dut
    ret = connsys_fifo_write(buffer, tx_len);
    if (ret != 0) {
        // error cuures
        os_free(buffer);
        return err;
    }

    // check tx done and rx0 done interrupt status
    wait_tx_done = 1;
    wait_rx_done = 1;
    counter = 0;
    while (1) {
        ret = connsys_cr_read(WHISR, &value);
        if (ret) {
            LOG_E(connsys, "<<%s>> Read WHISR failed. Error = %d.\n", __FUNCTION__, (int)ret);
            os_free(buffer);
            return err;
        }

        //LOG_I(connsys, "<<%s>> WHISR = %08x.\n", __FUNCTION__, (unsigned int)value);

        connsys_abnormal_interrupt_check(value);

        if (wait_tx_done == 1) {
            if (value & TX_DONE_INT) {
                wait_tx_done = 0;
            }
        }

        if (wait_rx_done == 1) {
            if (value & RX0_DONE_INT) {
                wait_rx_done = 0;
            }
        }

        if ((wait_tx_done == 0) && (wait_rx_done == 0)) {
            break;
        }

        if (counter > 200000) { // wait for at least 1 second
            LOG_E(connsys, "<<%s>> Check interrupt failed. wait_tx_done = %d, wait_tx_done = %d.\n",
                  __FUNCTION__,
                  (int)wait_tx_done,
                  (int)wait_rx_done);
            os_free(buffer);
            return err;
        }
        //udelay(50); //delay 50us

        counter++;
    }


    // Rx Patch finish response
    LOG_I(connsys, "<<%s>> Rx Patch finish event...\n", __FUNCTION__);
    ret = connsys_cr_read(WRPLR, &value);
    if (ret) {
        LOG_E(connsys, "<<%s>> Read WRPLR failed. Error = %d.\n", __FUNCTION__, (int)ret);
        os_free(buffer);
        return err;
    }

    real_rx_len = LEN_FW_DOWNLOAD_EVENT;

    if (GET_RX0_PACKET_LENGTH(value) != real_rx_len) {
        LOG_E(connsys, "<<%s>> Patch finish response length incoreect. WRPLR = %08x.\n",
              __FUNCTION__,
              (unsigned int)value);
        os_free(buffer);
        return err;
    }

    //LOG_I(connsys, "<<%s>> Receive Patch finish event ...\n", __FUNCTION__);

    connsys_fifo_read(WRDR0, buffer, real_rx_len);

    // print_content(real_rx_len, buffer, "patch finish..2");
    //LOG_HEXDUMP_I(connsys, "patch finish..2", (const char *)buffer, real_rx_len);

    // check event status
    if (connsys_util_fw_download_CmdAddressLenReq_event_check(buffer) != 0) {
        os_free(buffer);
        return err;
    }

    err = 0;
    os_free(buffer);
    return err;
}

/* ===============================================================
 * Function: connsys_util_create_inband_cmd_txd_rom()
 * Description: to fill the inband commnad header of rom code.
 * Input: total size of cmd, buffer, command id
 * Return: the header size
 * ===============================================================*/
uint32_t connsys_util_create_inband_cmd_txd_rom(uint32_t cmd_len, uint8_t *buffer, enum MT_CMD_TYPE cmd) {
    P_INIT_HIF_TX_HEADER_T p_txd;
    P_INIT_WIFI_CMD_T   p_cmd;

    p_txd = (P_INIT_HIF_TX_HEADER_T)buffer;
    p_txd->u2TxByteCount = cmd_len;
    p_txd->u2PQ_ID = P1_Q0;

    p_cmd = &(p_txd->rInitWifiCmd);
    p_cmd->ucCID = cmd;
    p_cmd->ucPktTypeID = PKT_ID_CMD;

    return LEN_INBAND_CMD_HDR_ROM;
}

//================= FW Download test ==========================================================//
uint32_t connsys_fw_download_CmdAddressLenReq(uint32_t addr, uint32_t len, uint32_t data_mode, uint8_t *buffer)
{
    uint32_t cmd_len, offset;
    P_INIT_CMD_DOWNLOAD_CONFIG p_data;

    cmd_len = sizeof(INIT_HIF_TX_HEADER_T) + sizeof(INIT_CMD_DOWNLOAD_CONFIG);

    // fill in the txd
    offset = connsys_util_create_inband_cmd_txd_rom(cmd_len, buffer, MT_TARGET_ADDRESS_LEN_REQ);

    // fill in the parameters
    p_data = (P_INIT_CMD_DOWNLOAD_CONFIG)(buffer + offset);
    p_data->u4Address = addr;
    p_data->u4Length = len;
    p_data->u4DataMode = data_mode;

    // print_content(cmd_len, buffer, "FW download: addr len req");
    LOG_HEXDUMP_I(connsys, "FW download: addr len req", (const char *)buffer, cmd_len);

    return cmd_len;
}

/* ===============================================================
 * Function: connsys_create_inband_fw_scatter_txd_rom()
 * Description: to fill the scatter commnad header of rom code.
 * Input: total size of cmd, buffer, command id
 * Return: the header size
 * ===============================================================*/
uint32_t connsys_create_inband_fw_scatter_txd_rom(uint32_t cmd_len, uint8_t *buffer)
{
    P_INIT_HIF_TX_HEADER_T p_txd;
    P_INIT_WIFI_CMD_T   p_cmd;

    p_txd = (P_INIT_HIF_TX_HEADER_T)buffer;
    p_txd->u2TxByteCount = cmd_len;
    p_txd->u2PQ_ID = PQ_TO_PDA;

    p_cmd = &(p_txd->rInitWifiCmd);
    p_cmd->ucCID = MT_FW_SCATTER;
    p_cmd->ucPktTypeID = PKT_ID_CMD;

    return LEN_INBAND_CMD_HDR_ROM;
}

uint8_t connsys_util_fw_download_CmdAddressLenReq_event_check(uint8_t *buffer)
{
    P_INIT_WIFI_EVENT_T p_event;
    P_INIT_EVENT_CMD_RESULT p_status;
    uint8_t err = 0;

    // get stauts
    p_event = (P_INIT_WIFI_EVENT_T)buffer;
    p_status = (P_INIT_EVENT_CMD_RESULT)p_event->aucBuffer;

    // print_content(p_event->u2RxByteCount, buffer, "FW download: addr len req evt check");
    LOG_HEXDUMP_I(connsys, "FW download: addr len req evt check", (const char *)buffer, p_event->u2RxByteCount);

    if ((p_status->ucStatus) || (p_event->ucEID != INIT_EVENT_ID_CMD_RESULT)) {
        LOG_E(connsys, "<<%s>> Download config event incorrect. EID = %d, Status = %d\n",
              __FUNCTION__,
              (int)p_event->ucEID,
              (int)p_status->ucStatus);
        err = 1;
    } else {
        //LOG_I(connsys, "<<%s>> Download config set Successfully.\n", __FUNCTION__);
    }

    return err;
}

uint8_t connsys_util_patch_download_get_semaphore_event_check(uint8_t *buffer, uint8_t *status)
{
    P_INIT_WIFI_EVENT_T p_event;
    P_INIT_EVENT_PATCH_SEMA_CTRL_T p_status;
    uint8_t err = 0;

    // get stauts
    p_event = (P_INIT_WIFI_EVENT_T)buffer;
    p_status = (P_INIT_EVENT_PATCH_SEMA_CTRL_T)p_event->aucBuffer;

    // print_content(p_event->u2RxByteCount, buffer, "Patch download");
    LOG_HEXDUMP_I(connsys, "Patch download", (const char *)buffer, p_event->u2RxByteCount);

    if (p_event->ucEID != INIT_EVENT_ID_PATCH_SEMA_CTRL) {
        LOG_E(connsys, "<<%s>> Patch semaphore event incorrect. EID = %d\n",
              __FUNCTION__,
              (int)p_event->ucEID);
        err = 1;
    } else {
        //LOG_I(connsys, "<<%s>> Patch semaphore event ok.\n", __FUNCTION__);

        *status = p_status->ucStatus;
    }


    return err;
}

int32_t connsys_fw_download_Tx_Scatter(uint8_t *image, uint32_t len)
{
    int32_t err = 0;
    int32_t ret;
    uint32_t left_len = len;
    uint32_t read_len = 0;
    uint32_t pos = 0, offset = 0;
    uint32_t tx_len;

    //LOG_I(connsys, "<<%s>>\n", __FUNCTION__);
    while (left_len) {
        if (left_len < LEN_FW_SCATTER) {
            read_len = left_len;
        } else {
            read_len = LEN_FW_SCATTER;
        }

        // prepare txd
        tx_len = LEN_SDIO_TX_AGG_WRAPPER(LEN_INBAND_CMD_HDR_ROM + read_len);

        os_memset(g_tx_buf, 0, tx_len);
        offset = connsys_create_inband_fw_scatter_txd_rom(LEN_INBAND_CMD_HDR_ROM + read_len, g_tx_buf);

        os_memcpy(&g_tx_buf[offset], &image[pos], read_len);

        pos += read_len;
        left_len -= read_len;

        //LOG_I(connsys, "<<%s>> Read file total_len: %d.\n", __FUNCTION__, (int)pos);
        // print_content(32, g_tx_buf, "Send FW (first 32 bytes)");
        //LOG_HEXDUMP_I(connsys, "Send FW (first 32 bytes)", (const char *)g_tx_buf, 32);

        // write to dut
        ret = connsys_fifo_write(g_tx_buf, tx_len);
        if (ret != 0) {
            // error cuures
            LOG_E(connsys, "<<%s>> Read scatter failed. Error = %d.\n", __FUNCTION__, (int)ret);
            err = 1;;
        }
    }
    return err;
}

int32_t connsys_pda_config(uint32_t dest_addr, uint32_t data_len, uint32_t data_mode)
{
    uint32_t value, rx_size, counter = 0;
    int32_t ret;
    uint8_t wait_tx_done, wait_rx_done;
    int32_t err = 1;
    uint8_t *buffer;

    buffer = os_zalloc(SDIO_MAX_RW_SIZE);

    //LOG_I(connsys, "<<%s>> Send fw download command...\n", __FUNCTION__);

    value = connsys_fw_download_CmdAddressLenReq(dest_addr, data_len, data_mode, buffer);

    // write connsys_util_fw_download cmd to dut
    ret = connsys_fifo_write(buffer, LEN_SDIO_TX_AGG_WRAPPER(value));
    if (ret != 0) {
        // error cuures
        os_free(buffer);
        return err;
    }

    wait_tx_done = 1;
    wait_rx_done = 1;
    counter = 0;
    while (1) {
        ret = connsys_cr_read(WHISR, &value);
        if (ret) {
            LOG_E(connsys, "<<%s>> Read WHISR failed. Error = %d.\n", __FUNCTION__, (int)ret);
            os_free(buffer);
            return err;
        }

        //LOG_I(connsys, "<<%s>> WHISR = %08x.\n", __FUNCTION__, value);

        connsys_abnormal_interrupt_check(value);

        if (wait_tx_done == 1) {
            if (value & TX_DONE_INT) {
                wait_tx_done = 0;
            }
        }

        if (wait_rx_done == 1) {
            if (value & RX0_DONE_INT) {
                wait_rx_done = 0;
            }
        }

        if ((wait_tx_done == 0) && (wait_rx_done == 0)) {
            break;
        }

        if (counter > 2000000) { // wait for at least 1 second
            LOG_E(connsys, "<<%s>> Check interrupt failed. wait_tx_done = %d, wait_tx_done = %d.\n",
                  __FUNCTION__,
                  (int)wait_tx_done,
                  (int)wait_rx_done);
            os_free(buffer);
            return err;
        }
        //udelay(50); //delay 50us

        counter++;
    }

    // rx event from dut
    //LOG_I(connsys, "<<%s>> Rx fw download event...\n", __FUNCTION__);
    ret = connsys_cr_read(WRPLR, &value);
    if (ret) {
        LOG_E(connsys, "<<%s>> Read WRPLR failed. Error = %d.\n", __FUNCTION__, (int)ret);
        os_free(buffer);
        return err;
    }

    //LOG_I(connsys, "<<%s>> WRPLR = %08x.\n", __FUNCTION__, (unsigned int)value);


    //rx_size = LEN_SDIO_RX_PACKET_WRAPPER(LEN_FW_DOWNLOAD_EVENT); //use this one will cause data incorrect. all 0.
    rx_size = LEN_FW_DOWNLOAD_EVENT;

    if (GET_RX0_PACKET_LENGTH(value) != rx_size) {
        LOG_E(connsys, "<<%s>> download config event response length incoreect. WRPLR = %08x.\n",
              __FUNCTION__,
              (unsigned int)value);
        os_free(buffer);
        return err;
    }
    connsys_fifo_read(WRDR0, buffer, rx_size);
    ret = connsys_util_fw_download_CmdAddressLenReq_event_check(buffer);
    os_free(buffer);
    return err;
}

int32_t connsys_util_pda_download(uint32_t dest_addr, uint32_t data_len, uint32_t data_mode, uint32_t data_offset, uint8_t *image)
{
    int32_t ret;

    ret = connsys_pda_config(dest_addr, data_len, data_mode);


    if (image != NULL) {
        ret = connsys_fw_download_Tx_Scatter(&(image[data_offset]), data_len);
    }
    return ret;
}

void connsys_util_fw_download_done()
{
    uint32_t counter = 0;
    uint32_t tx_len, cmd_len;
    uint32_t value, offset;
    P_INIT_CMD_WIFI_START p_data;
    uint8_t wait_tx_done, wait_rx_done;
    uint8_t *buffer;
    uint32_t real_rx_len, valid_rx_len;
    int32_t ret;

    buffer = os_zalloc(SDIO_MAX_RW_SIZE);

    // FW Start command
    cmd_len = sizeof(INIT_HIF_TX_HEADER_T) + sizeof(INIT_CMD_WIFI_START);
    tx_len = LEN_SDIO_TX_AGG_WRAPPER(cmd_len);

    // fill in the txd
    offset = connsys_util_create_inband_cmd_txd_rom(cmd_len, buffer, MT_FW_START_REQ);

    // fill in the parameters
    p_data = (P_INIT_CMD_WIFI_START)(buffer + offset);
    p_data->u4Address = 0;
    p_data->u4Override = 0;
    // print_content(tx_len, buffer, "fw download test");
    LOG_HEXDUMP_I(connsys, "fw download test", (const char *)buffer, tx_len);

    // send to dut
    //LOG_I(connsys, "<<%s>> Send FW_START command ...\n", __FUNCTION__);
    // write connsys_util_fw_download cmd to dut
    ret = connsys_fifo_write(buffer, tx_len);
    if (ret != 0) {
        // error cuures
        os_free(buffer);
        return;
    }


    // check tx done and rx0 done interrupt status
    wait_tx_done = 1;
    wait_rx_done = 1;
    counter = 0;
    while (1) {
        ret = connsys_cr_read(WHISR, &value);
        if (ret) {
            LOG_E(connsys, "<<%s>> Read WHISR failed. Error = %d.\n", __FUNCTION__, (int)ret);
            os_free(buffer);
            return;
        }

        //LOG_I(connsys, "<<%s>> WHISR = %08x.\n", __FUNCTION__, value);

        connsys_abnormal_interrupt_check(value);


        if (wait_tx_done == 1) {
            if (value & TX_DONE_INT) {
                wait_tx_done = 0;
            }
        }

        if (wait_rx_done == 1) {
            if (value & RX0_DONE_INT) {
                wait_rx_done = 0;
            }
        }

        if ((wait_tx_done == 0) && (wait_rx_done == 0)) {
            break;
        }

        if (counter > 20000000) { // wait for at least 1 second
            LOG_E(connsys, "<<%s>> Check interrupt failed. wait_tx_done = %d, wait_tx_done = %d.\n",
                  __FUNCTION__,
                  (int)wait_tx_done,
                  (int)wait_rx_done);
            os_free(buffer);
            return;
        }
        //udelay(50); //delay 50us

        counter++;
    }


    // Rx FW start response
    //LOG_I(connsys, "<<%s>> Rx fw download event...\n", __FUNCTION__);
    ret = connsys_cr_read(WRPLR, &value);
    if (ret) {
        LOG_E(connsys, "<<%s>> Read WRPLR failed. Error = %d.\n", __FUNCTION__, (int)ret);
        os_free(buffer);
        return;
    }

    valid_rx_len = LEN_FW_DOWNLOAD_EVENT;
    real_rx_len = valid_rx_len;

    if (GET_RX0_PACKET_LENGTH(value) != real_rx_len) {
        LOG_E(connsys, "<<%s>> FW start response length incoreect. WRPLR = %08x.\n",
              __FUNCTION__,
              (unsigned int)value);
        os_free(buffer);
        return;
    }

    //LOG_I(connsys, "<<%s>> Receive FW_START event ...\n", __FUNCTION__);

    connsys_fifo_read(WRDR0, buffer, real_rx_len);
    // print_content(real_rx_len, buffer, "fw download test..2");
    LOG_HEXDUMP_I(connsys, "fw download test..2", (const char *)buffer, real_rx_len);

    // check event status
    if (connsys_util_fw_download_CmdAddressLenReq_event_check(buffer) != 0) {
        os_free(buffer);
        return;
    }

    // polling function ready bit.
    counter = 0;
    while (1) {
        ret = connsys_cr_read(WCIR, &value);
        if (ret) {
            LOG_E(connsys, "<<%s>> Read WCIR failed. Error = %d.\n", __FUNCTION__, (int)ret);
            os_free(buffer);
            return;
        }

        //LOG_I(connsys, "<<%s>> WCIR = %08x.\n", __FUNCTION__, (unsigned int)value);

        if (value & W_FUNC_RDY) {
            break;
        }

        if (counter > 20000) { // wait for at least 1 second
            LOG_E(connsys, "<<%s>> Check interrupt failed. wait_tx_done = %d, wait_tx_done = %d.\n",
                  __FUNCTION__,
                  (int)wait_tx_done,
                  (int)wait_rx_done);
            os_free(buffer);
            return;
        }
        //udelay(50); //delay 50us

        counter++;
    }
    //LOG_I(connsys, "<<%s>> FW Download Successfully.\n", __FUNCTION__);
    os_free(buffer);
}

uint8_t connsys_get_patch_semaphore(void)
{
    uint8_t err = 1, wait_tx_done, wait_rx_done, get_semaphore, status;
    uint32_t cmd_len, tx_len, offset, counter, value, valid_rx_len, get_patch_delay;
    P_INIT_CMD_PATCH_SEMA_CTRL p_data;
    ssize_t ret;
    uint8_t *buffer;

    buffer = os_zalloc(SDIO_MAX_RW_SIZE);

    cmd_len = sizeof(INIT_HIF_TX_HEADER_T) + sizeof(INIT_CMD_PATCH_SEMA_CTRL);
    tx_len = LEN_SDIO_TX_AGG_WRAPPER(cmd_len);

    get_semaphore = 1;
    get_patch_delay = 0;
    while (get_semaphore) {
        // fill in the txd
        offset = connsys_util_create_inband_cmd_txd_rom(cmd_len, buffer, MT_PATCH_SEM_CONTROL);

        // fill in the parameters
        p_data = (P_INIT_CMD_PATCH_SEMA_CTRL)(buffer + offset);
        p_data->ucGetSemaphore = 1; //get semaphore
        // print_content(tx_len, buffer, "get patch semaphore");
        LOG_HEXDUMP_I(connsys, "get patch semaphore", (const char *)buffer, tx_len);

        // send to dut
        //LOG_I(connsys, "<<%s>> Send Get Patch Semaphore command ...\n", __FUNCTION__);
        // write connsys_util_fw_download cmd to dut
        ret = connsys_fifo_write(buffer, tx_len);
        if (ret != 0) {
            // error cuures
            os_free(buffer);
            return err;
        }

        // check tx done and rx0 done interrupt status
        wait_tx_done = 1;
        wait_rx_done = 1;
        counter = 0;
        while (1) {
            ret = connsys_cr_read(WHISR, &value);
            if (ret) {
                LOG_E(connsys, "<<%s>> Read WHISR failed. Error = %d.\n", __FUNCTION__, (int)ret);
                os_free(buffer);
                return err;
            }

            //LOG_I(connsys, "<<%s>> WHISR = %08x.\n", __FUNCTION__, (unsigned int)value);

            connsys_abnormal_interrupt_check(value);


            if (wait_tx_done == 1) {
                if (value & TX_DONE_INT) {
                    wait_tx_done = 0;
                }
            }

            if (wait_rx_done == 1) {
                if (value & RX0_DONE_INT) {
                    wait_rx_done = 0;
                }


                if (value & RX1_DONE_INT) {
                    wait_rx_done = 0;
                }
            }

            if ((wait_tx_done == 0) && (wait_rx_done == 0)) {
                break;
            }

            if (counter > 20000) { // wait for at least 1 seconds
                LOG_E(connsys, "<<%s>> Check interrupt failed. wait_tx_done = %d, wait_tx_done = %d.\n",
                      __FUNCTION__,
                      (int)wait_tx_done,
                      (int)wait_rx_done);
                os_free(buffer);
                return err;
            }
            //udelay(50); //delay 50us

            counter++;
        }


        // Rx get_patch_semaphore response
        //LOG_I(connsys, "<<%s>> Rx fw download event...\n", __FUNCTION__);
        ret = connsys_cr_read(WRPLR, &value);
        if (ret) {
            LOG_E(connsys, "<<%s>> Read WRPLR failed. Error = %d.\n", __FUNCTION__, (int)ret);
            os_free(buffer);
            return err;
        }

        valid_rx_len = LEN_GET_PATCH_SEMAPHORE_EVENT;

        if (GET_RX0_PACKET_LENGTH(value) != valid_rx_len) {
            LOG_E(connsys, "<<%s>> Get patch semaphore response length incoreect. WRPLR = %08x.\n",
                  __FUNCTION__,
                  (unsigned int)value);
            os_free(buffer);
            return err;
        }

        //LOG_I(connsys, "<<%s>> Receive Get patch semaphore event ...\n", __FUNCTION__);

        connsys_fifo_read(WRDR0, buffer, valid_rx_len);
        // print_content(valid_rx_len, buffer, "get patch semaphore...2");
        LOG_HEXDUMP_I(connsys, "get patch semaphore...2", (const char *)buffer, valid_rx_len);

        // check event status
        if (connsys_util_patch_download_get_semaphore_event_check(buffer, &status)) {
            os_free(buffer);
            return err;
        } else {
            if (status == WIFI_PATCH_SEMA_NEED_PATCH_SEMA_FAIL) {
                if (get_patch_delay > 20000) {  //wait 1 second
                    LOG_E(connsys, "<<%s>> FAIL. Get patch semaphore fail.\n", __FUNCTION__);
                    os_free(buffer);
                    return err;
                }

                get_patch_delay += 1;
                //udelay(50); //delay 50us
            } else if (status == WIFI_PATCH_SEMA_PATCH_DONE_SEMA_IGNORE) {
                LOG_E(connsys, "<<%s>> FAIL. Get patch semaphore fail.\n", __FUNCTION__);
                get_semaphore = 0;

            } else if (status == WIFI_PATCH_SEMA_NEED_PATCH_SEMA_SUCCESS) {
                get_semaphore = 0;
            } else if (status == WIFI_PATCH_SEMA_RELEASE_SUCCESS) {
            }
        }
    }

    os_free(buffer);
    return status;
}

void connsys_patch_direct_download()
{
    uint32_t data_mode = 0, length = 0;
    uint8_t status = 0;
    patch_dl_data_t data;
    uint8_t *ptr = NULL;

#if 0
    LOG_I(connsys, "<<%s>> The Bin File = %s, ilm_addr = %08x, ilm_tx_len = %x\n",
          __FUNCTION__, data->file_name, data->ilm_addr, data->ilm_len);
#endif

    data.image = (uint8_t *)(WIFI_FW_ADDR_IN_FLASH);
    data.ilm_addr = MT7687_PATCH_START_ADDR;

    length = *((uint32_t *) & (data.image[0]));

    data.ilm_len = length - 4 - LEN_PATCH_FILE_HEADER; //??? LEN_4_BYTE_CRC;

    data_mode |= (((uint32_t)1 << DATA_MODE_BIT_SHFT_NEED_ACK) & DATA_MODE_MASK_NEED_ACK);

    //LOG_I(connsys, "<<%s>> data_mode = %08x\n", __FUNCTION__, (unsigned int)data_mode);

    // get ownership form firmware
    if (TRUE == connsys_get_ownership()) {
        //LOG_I(connsys, "<<%s>> Get ownership ok.\n", __FUNCTION__);
    } else {
        LOG_W(connsys, "<<%s>> Get ownership failed.\n", __FUNCTION__);
    }

    // get semaphore
    status = connsys_get_patch_semaphore();
    if (status == WIFI_PATCH_SEMA_NEED_PATCH_SEMA_SUCCESS) {
        //LOG_I(connsys, "<<%s>> Send patch download command...\n", __FUNCTION__);

        ptr = &(data.image[4 + LEN_PATCH_FILE_HEADER]);    /* skip total image size */
        ptr = (uint8_t *)connsys_img_direct_download(data.ilm_addr, data.ilm_len, (uint32_t)ptr,
                data_mode, 0); // data.ilm_encrypt_key_index = 0 , just a dummy for PATCH has no encryption
        if (ptr == NULL) {
            LOG_E(connsys, "Download PATCH Fail.\n");
            return ;
        }
        // send patch finish command
        status = connsys_patch_finish();
        if (status) {
            LOG_E(connsys, "\n\n\n<<%s>> Patch Download Fail.\n\n\n", __FUNCTION__);
            return;
        }

        //LOG_I(connsys, "<<%s>> Patch Direct Download Successfully.\n", __FUNCTION__);
    } else if (status == WIFI_PATCH_SEMA_PATCH_DONE_SEMA_IGNORE) {
        LOG_W(connsys, "<<%s>> Patch has been done, needn't get semaphore.\n", __FUNCTION__);
    } else if (status == WIFI_PATCH_SEMA_RELEASE_SUCCESS) {
        LOG_I(connsys, "<<%s>> Fail! Get semaphore instead of release patch semaphore.\n", __FUNCTION__);
    } else if (status == WIFI_PATCH_SEMA_NEED_PATCH_SEMA_FAIL) {
        LOG_W(connsys, "<<%s>> Fail! Get patch semaphore failed.\n", __FUNCTION__);
    }

}

void connsys_set_wifi_profile(sys_cfg_t *config)
{
    g_wifi_profile = config;
}

void connsys_patch_download_body(const patch_dl_data_t *data)
{
    uint32_t data_mode = 0;
    uint8_t err = 0;
    uint8_t status;

#if 0
    LOG_I(connsys, "<<%s>> The Bin File = %s, ilm_addr = %08x, ilm_tx_len = %x\n",
          __FUNCTION__, data->file_name, data->ilm_addr, data->ilm_len);
#endif
    data_mode |= (((uint32_t)1 << DATA_MODE_BIT_SHFT_NEED_ACK) & DATA_MODE_MASK_NEED_ACK);


    //LOG_I(connsys, "<<%s>> data_mode = %08x\n", __FUNCTION__, (unsigned int)data_mode);

    // get ownership form firmware
    if (TRUE == connsys_get_ownership()) {
        //LOG_I(connsys, "<<%s>> Get ownership ok.\n", __FUNCTION__);
    } else {
        LOG_W(connsys, "<<%s>> Get ownership failed.\n", __FUNCTION__);
    }

    // get semaphore
    status = connsys_get_patch_semaphore();
    if (status == WIFI_PATCH_SEMA_NEED_PATCH_SEMA_SUCCESS) {
#if (CFG_FPGA == 0)
        //LOG_I(connsys, "<<%s>> Send patch download command...\n", __FUNCTION__);
        err = connsys_util_pda_download(data->ilm_addr, data->ilm_len, data_mode, LEN_PATCH_FILE_HEADER, data->image);
        if (err) {
            LOG_E(connsys, "\n\n\n<<%s>> Send patch download fail!\n\n\n", __FUNCTION__);
            return;
        }
#endif // CFG_FPGA

        // Download N9 wifi profile
        if (NULL != g_wifi_profile) {
            connsys_download_wifi_profile(g_wifi_profile);
        }

        // send patch finish command
        err = connsys_patch_finish();
        if (err) {
            LOG_E(connsys, "\n\n\n<<%s>> Patch Download Fail.\n\n\n", __FUNCTION__);
            return;
        }

        //LOG_I(connsys, "<<%s>> Patch download Successfully.\n", __FUNCTION__);
    } else if (status == WIFI_PATCH_SEMA_PATCH_DONE_SEMA_IGNORE) {
        LOG_I(connsys, "<<%s>> Patch has been done, needn't get semaphore.\n", __FUNCTION__);
    } else if (status == WIFI_PATCH_SEMA_RELEASE_SUCCESS) {
        LOG_I(connsys, "<<%s>> Fail! Get semaphore instead of release patch semaphore.\n", __FUNCTION__);
    } else if (status == WIFI_PATCH_SEMA_NEED_PATCH_SEMA_FAIL) {
        LOG_I(connsys, "<<%s>> Fail! Get patch semaphore failed.\n", __FUNCTION__);
    }

}


patch_header_t *g_patch_info;
void connsys_util_get_ncp_patch_ver(char *ver)
{
    os_memcpy(ver, g_patch_info->built_date, 14);
}

void connsys_util_patch_download(void)
{
    uint32_t fw_size;
    patch_dl_data_t data = {
        .ilm_addr = MT7687_PATCH_START_ADDR,
        .ilm_len  = 0,
        .image    = NULL,
    };
    //LOG_I(connsys, "==> HW ID=0x%x\n", REG32(CM4_CONFIG_BASE));

#ifdef MTK_SEC_FLASH_SCRAMBLE_ENABLE
    if (sboot_efuse_sbc_state == 1) {
        data.image = (uint8_t *)((MTK_FW_DW_BY_CM4_ADDR + N9_RAMCODE_BASE));
    } else {
        data.image = (uint8_t *)(WIFI_FW_ADDR_IN_FLASH);
    }
#else
    data.image = (uint8_t *)(WIFI_FW_ADDR_IN_FLASH);
#endif
    fw_size = *((uint32_t *)(data.image)); // include 4-byte total_length  itself
    data.image += fw_size;
    g_patch_info = (patch_header_t *)(data.image + 4);
    data.ilm_len = *((uint32_t *)(data.image)); // patch image size, include 4-byte patch length
    data.ilm_len -= (LEN_PATCH_FILE_HEADER + 4); // 4-byte patch_length
    data.image   += 4; // patch image start, including header
    //LOG_I(connsys, "==> PATCH size: %d bytes, build time: %s\n", (int)data.ilm_len,	g_patch_info->built_date);

    //LOG_I(connsys, "patch ilm_addr = %08x, ilm_len = %x.\n", (unsigned int)data.ilm_addr, (unsigned int)data.ilm_len);
    connsys_patch_download_body(&data);
}

#if 0
#if 1
#else
uint8_t MT7687_FirmwareImage[] = {0x00}; // for test, avoid LARGE array lead to memory overflow
#endif


void connsys_util_fw_download()
{
    fw_dl_data_t data;
    //uint8_t input[128];

    data.image = MT7687_FirmwareImage;
    connsys_get_ilm_image_info(&data, sizeof(MT7687_FirmwareImage));

    // add 4 byte CRC len
    data.ilm_len += LEN_4_BYTE_CRC;
    data.dlm_len += LEN_4_BYTE_CRC;

    //LOG_I(connsys, "ilm_addr = %08x, ilm_len = %x, encryt = %d, key_index = %d.\n", data.ilm_addr, data.ilm_len, data.ilm_encrypt, data.ilm_encrypt_key_index);
    //LOG_I(connsys, "dlm_addr = %08x, dlm_len = %x.\n", data.dlm_addr, data.dlm_len);

    connsys_fw_download((fw_dl_data_t *)&data);
}
#endif

int32_t connsys_util_tx_data(uint8_t *buf, ssize_t len)
{
    int32_t ret;



    if (CONNSYS_TEST_DEBUG(CONNSYS_DBG_TX_Q0) || CONNSYS_TEST_DEBUG(CONNSYS_DBG_TX_Q1)) {
        P_INIT_HIF_TX_HEADER_T p_txd = (P_INIT_HIF_TX_HEADER_T)buf;
        if (p_txd->u2PQ_ID == P1_Q0) {
            LOG_HEXDUMP_I(connsys, "P1_Q0 tx_buf", (uint8_t *)buf, (uint32_t)len);
        } else {
            LOG_HEXDUMP_I(connsys, "P1_Q1 tx_buf", (uint8_t *)buf, (uint32_t)len);
        }
    }

    ret = connsys_fifo_write(buf, len);
    return ret;
}

int32_t connsys_util_rx_data(uint8_t *buf, ssize_t len, int32_t port)
{
    ssize_t err = CONNSYS_STATUS_SUCCESS, ret;

    if (port == WIFI_HIF_RX_PORT0_IDX) {
        ret = connsys_fifo_read(WRDR0, buf, len);
    } else {
        ret = connsys_fifo_read(WRDR1, buf, len);
    }

    if (ret) {
        LOG_E(connsys, "<<%s>> Failed. Rx data, port(0x%x)\n", __FUNCTION__, WRDR1);
        err = CONNSYS_STATUS_FAIL;
    }

    return err;
}

#if (CFG_CONNSYS_DVT_RX_ENHANCE_MODE_EN == 1)
// return value: total read size
int32_t connsys_util_rx_data_enhance_mode(uint8_t *buf, ssize_t *len, int32_t port)
{
#if defined(__ICCARM__)
    ATTR_4BYTE_ALIGN uint8_t input[LEN_INT_ENHANCE_MODE];
#else
    uint8_t input[LEN_INT_ENHANCE_MODE] __attribute__((aligned(4)));
#endif
    uint32_t rx_len = 0, addr = WRDR0;
    uint32_t reg_value = 0;
    enhance_mode_data_struct_t *p_int_enhance;
    int32_t i;

    if (port == 0) {
        addr = WRDR0;
    } else if (port == 1) {
        addr = WRDR1;
    } else {
        LOG_E(connsys, "Invalid port:%d\n", port);
        return CONNSYS_STATUS_FAIL;
    }

    // enable RX enhance mode
    if (connsys_cr_read(WHCR, &reg_value)) {
        LOG_E(connsys, "FAIL. read WHCR.\n");
        return CONNSYS_STATUS_FAIL;
    }
    reg_value |= RX_ENHANCE_MODE;
    if (connsys_cr_write(WHCR, reg_value)) {
        LOG_E(connsys, "FAIL. write WHCR.\n");
        return CONNSYS_STATUS_FAIL;
    }
    reg_value = 0;
    if (connsys_cr_read(WHCR, &reg_value)) {
        LOG_E(connsys, "FAIL. read WHCR.\n");
        return CONNSYS_STATUS_FAIL;
    } else {
        if (!(reg_value & RX_ENHANCE_MODE)) {
            LOG_E(connsys, "FAIL. write RX_ENHANCE_MODE fail. WHCR = 0x%08x.\n", reg_value);
            return CONNSYS_STATUS_FAIL;
        }
    }
    //LOG_I(connsys, "Enable enhance mode, WHCR=0x%x\n", reg_value);

    os_memset(input, 0, sizeof(input));
    connsys_fifo_read(WHISR, (uint8_t *)input, LEN_INT_ENHANCE_MODE);

    p_int_enhance = (enhance_mode_data_struct_t *)input;

    for (i = 0; i < p_int_enhance->rx_info.u.valid_len_num[port]; i++) {
        if (p_int_enhance->rx_info.u.each_rx_len[port][i] == 0) {
            LOG_E(connsys, "FAIL. au2Rx[%d]Len[%d] = %d. ()\n", port, i, p_int_enhance->rx_info.u.each_rx_len[port][i]);
            continue;
        }
        rx_len = p_int_enhance->rx_info.u.each_rx_len[port][i];
        //LOG_I(connsys, "rx len = %d\n", rx_len);
        connsys_fifo_read(addr, (uint8_t *)buf, rx_len);
    }
    *len = rx_len;

    return CONNSYS_STATUS_SUCCESS;
}

#endif /* CFG_CONNSYS_DVT_RX_ENHANCE_MODE_EN */


#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
void connnsys_balance_init(void)
{
    uint32_t flags;
    local_irq_save(flags);

    os_memset(&g_balance_ctr, 0, sizeof(connsys_balance_ctr_t));
    g_balance_ctr.num_of_continuous_rx = CFG_CONNSYS_BALANCE_DEFAULT_RX_NUM;
    g_balance_ctr.num_of_continuous_tx = CFG_CONNSYS_BALANCE_DEFAULT_TX_NUM;
    g_balance_ctr.balance_en = FALSE;
    local_irq_restore(flags);
}

void connnsys_balance_set_max_num_of_continuous_rx(uint32_t num)
{
    uint32_t flags;
    local_irq_save(flags);
    g_balance_ctr.num_of_continuous_rx = num;
    local_irq_restore(flags);
}

void connnsys_balance_set_max_num_of_continuous_tx(uint32_t num)
{
    uint32_t flags;
    local_irq_save(flags);
    g_balance_ctr.num_of_continuous_tx = num;
    local_irq_restore(flags);
}

void connnsys_balance_set_enable(uint32_t enable)
{
    uint32_t flags;
    local_irq_save(flags);
    g_balance_ctr.balance_en = (uint8_t)enable;
    local_irq_restore(flags);
}


void connnsys_balance_show_info(void)
{
    uint32_t flags;
    connsys_balance_ctr_t ctr;
    local_irq_save(flags);
    os_memcpy(&ctr, &g_balance_ctr, sizeof(connsys_balance_ctr_t));
    local_irq_restore(flags);

    printf("show connsys balance info:\n");
    printf("\tnum_of_continuous_tx = %u\n", (unsigned int)ctr.num_of_continuous_tx);
    printf("\tnum_of_continuous_rx = %u\n", (unsigned int)ctr.num_of_continuous_rx);
    printf("\tnum_of_tx = %u\n", (unsigned int)ctr.num_of_tx);
    printf("\tnum_of_rx = %u\n", (unsigned int)ctr.num_of_rx);
    printf("\tfg_tx_wait = %u\n", (unsigned int)ctr.fg_tx_wait);
    printf("\tfg_rx_wait = %u\n", (unsigned int)ctr.fg_rx_wait);
    printf("\ttx_yield_time = %u\n", (unsigned int)ctr.tx_yield_time);
    printf("\trx_yield_time = %u\n", (unsigned int)ctr.rx_yield_time);
    printf("\ttx_handle = %u\n", (unsigned int)ctr.tx_handle);
    printf("\trx_handle = %u\n", (unsigned int)ctr.rx_handle);
    printf("\tbalance_en = %u\n", (unsigned int)ctr.balance_en);
}

uint8_t connsys_balance_check_tx_yield(void)
{
    uint32_t flags;
    uint8_t ret = FALSE;

    local_irq_save(flags);
    if (g_balance_ctr.fg_rx_wait &&
            (g_balance_ctr.num_of_tx >= g_balance_ctr.num_of_continuous_tx)) {
        g_balance_ctr.num_of_tx = 0;
        g_balance_ctr.fg_tx_wait = TRUE;
        g_balance_ctr.tx_yield_time ++;
        ret = TRUE;
    } else {
        g_balance_ctr.num_of_tx++;
        ret = FALSE;
    }
    local_irq_restore(flags);
    return ret;
}

void connsys_balance_clear_tx_wait(void)
{
    uint32_t flags;
    local_irq_save(flags);
    g_balance_ctr.fg_tx_wait = FALSE;
    local_irq_restore(flags);
}

uint8_t connsys_balance_check_rx_yield(void)
{
    uint32_t flags;
    uint8_t ret = FALSE;


    if (g_balance_ctr.balance_en == FALSE) {
        return FALSE;
    }

    local_irq_save(flags);
    if (g_balance_ctr.fg_tx_wait &&
            (g_balance_ctr.num_of_rx >= g_balance_ctr.num_of_continuous_rx)) {
        g_balance_ctr.num_of_rx = 0;
        g_balance_ctr.rx_yield_time ++;
        ret = TRUE;
    } else {
        g_balance_ctr.num_of_rx++;
        ret = FALSE;
    }
    local_irq_restore(flags);
    return ret;
}

void connsys_balance_clear_rx_wait(void)
{
    uint32_t flags;
    local_irq_save(flags);
    g_balance_ctr.fg_rx_wait = FALSE;
    local_irq_restore(flags);
}


#endif /* CFG_CONNSYS_TRX_BALANCE_EN */


void connsys_intr_enhance_mode_dump_struct(enhance_mode_data_struct_t *p_enhance_mode_struct)
{
    uint32_t i, j;
    printf("==>connsys_intr_enhance_mode_dump_struct\n");
    printf("WHISR_reg_val = 0x%x\n", (unsigned int)p_enhance_mode_struct->WHISR_reg_val);
    printf("TX:");
    for (i = 0; i < NUM_OF_WIFI_TXQ; i++) {
        printf("\ttx_info.u.free_page_num[%u] = %u\n",
               (unsigned int)i,
               (unsigned int)p_enhance_mode_struct->tx_info.u.free_page_num[i]);
    }

    printf("RX:");
    for (i = 0; i < NUM_OF_WIFI_HIF_RX_PORT; i++) {
        printf("Port %u\n", (unsigned int)i);
        printf("\tvalid_len_num: %u\n",
               (unsigned short)p_enhance_mode_struct->rx_info.u.valid_len_num[i]);

        printf("\teach_rx_len:\n");
        for (j = 0; j < WIFI_HIF_RX_FIFO_MAX_LEN; j++) {
            printf("\t[%2u]: %8u", (unsigned int)j, (unsigned short)p_enhance_mode_struct->rx_info.u.each_rx_len[i][j]);
            if ((j % 4) == 3) {
                printf("\n");
            }
        }
    }
    printf("receive_mail_box_0 = 0x%x\n", (unsigned int)p_enhance_mode_struct->receive_mail_box_0);
    printf("receive_mail_box_1 = 0x%x\n", (unsigned int)p_enhance_mode_struct->receive_mail_box_1);
}

void connsys_util_intr_enhance_mode_dump_last_struct(void)
{
    connsys_intr_enhance_mode_dump_struct(&g_last_enhance_mode_data_struct);
}


uint8_t rssi_threshold_enable = 0;
int8_t rssi_threshold = 0;

uint8_t raw_packet_handler_enabled(void)
{
    return (connsys_raw_handler != NULL);
}

void wifi_set_raw_rssi_threshold(uint8_t enable, int8_t rssi)
{
    rssi_threshold_enable = enable;
    rssi_threshold = rssi;
}

void wifi_get_raw_rssi_threshold(uint8_t *enable, int8_t *rssi)
{
    *enable = rssi_threshold_enable;
    *rssi = rssi_threshold;
}

uint32_t get_rx_vector(uint8_t group_vld)
{
    uint32_t grp3_offset = 0;

    // RX Status Group
    grp3_offset = sizeof(HW_MAC_RX_DESC_T);
    if (group_vld & BIT(RX_GROUP_VLD_4)) {
        grp3_offset += sizeof(HW_MAC_RX_STS_GROUP_4_T);
    }
    if (group_vld & BIT(RX_GROUP_VLD_1)) {
        grp3_offset += sizeof(HW_MAC_RX_STS_GROUP_1_T);
    }
    if (group_vld & BIT(RX_GROUP_VLD_2)) {
        grp3_offset += sizeof(HW_MAC_RX_STS_GROUP_2_T);
    }
    if (group_vld & BIT(RX_GROUP_VLD_3)) {
        return (uint32_t)grp3_offset;
    } else {
        return 0;
    }
}

int32_t check_raw_pkt_rssi_threshold(uint8_t *payload, uint32_t len)
{
    if (rssi_threshold_enable == 0) {
        return 0;
    }

    int32_t ret = 0;
    P_HW_MAC_RX_STS_GROUP_3_T grp3;
    uint32_t grp3_offset = 0;
    int8_t rssi_info;
    uint8_t grp_vld;

    /* grp3_offset = get_rx_vector(((*(first_dw)) >> 25) & 0xf);
     * same to the following two line
     */
    grp_vld = HAL_RX_STATUS_GET_GROUP_VLD((P_HW_MAC_RX_DESC_T)payload);
    grp3_offset = get_rx_vector(grp_vld);
    if (grp3_offset != 0) {
        grp3 = (P_HW_MAC_RX_STS_GROUP_3_T)(payload + grp3_offset);
        /* RSSI = RCPI/2 - 110
         * rssi_info = ((((grp3[2]) >> 9) & 0x7f) - 110) & 0xff;
         * The same to the following line code
         */
        rssi_info = (((HAL_RX_STATUS_GET_RCPI(grp3) >> 1) & 0x7f) - 110) & 0xff;
    } else {
        rssi_info = -110;
    }

    /*
        hex_dump("pkt rssi:", payload, grp3_offset+40);
        if(grp3_offset != 0){
            hex_dump("grp3:", grp3, sizeof(HW_MAC_RX_STS_GROUP_3_T));
        }
        printf("rssi:%d\n", rssi_info);
        */

    if (rssi_info < rssi_threshold) {
        ret = -1;
    }

    return ret;
}

#if 0
typedef struct __pkt_lock_tickless {
    struct os_time timeout;
    int32_t pkt_rcv_num_in_sec;
    uint8_t lock_status;
    uint8_t sleep_lock;
} pkt_lock_tickless_t;

static pkt_lock_tickless_t pkt_lock;

static void reset_pkt_lock_status(uint8_t status, struct os_time *timeout_value)
{
    struct os_time current;

    os_get_time(&current);
    if (timeout_value) {
        pkt_lock.timeout.sec = current.sec + timeout_value->sec;
        pkt_lock.timeout.usec = current.usec + timeout_value->usec;
    } else {
        pkt_lock.timeout.sec = current.sec + 1;
    }

    pkt_lock.lock_status = status;
    pkt_lock.pkt_rcv_num_in_sec = 0;
}


static void enter_pkt_lock(void)
{
    //LOG_I(lwip, "enter lock\n");

    reset_pkt_lock_status(1, NULL);
    //hal_sleep_manager_lock_sleep(pkt_lock.sleep_lock);
    wifi_lock_sleep(pkt_lock.sleep_lock);
}

static void exit_pkt_lock(void)
{
    //LOG_I(lwip, "unlock\n");

    reset_pkt_lock_status(0, NULL);
    //hal_sleep_manager_unlock_sleep(pkt_lock.sleep_lock);
    wifi_unlock_sleep(pkt_lock.sleep_lock);
}

static void inc_pkt_rcv_num(void)
{
    pkt_lock.pkt_rcv_num_in_sec++;

    if (pkt_lock.lock_status == 0) {
        if (pkt_lock.pkt_rcv_num_in_sec >= 16) {
            enter_pkt_lock();
        }
    }
}

static void extend_pkt_lock_time(struct os_time *timeout_value)
{
    //LOG_I(lwip, "extend lock");
    reset_pkt_lock_status(1, timeout_value);
}

int32_t pkt_lock_init(struct os_time *timeout_value)
{
    struct os_time current;

    os_memset(&pkt_lock, 0, sizeof(pkt_lock));
    pkt_lock.sleep_lock = 0xff;
    os_get_time(&current);

    if (timeout_value) {
        pkt_lock.timeout.sec = current.sec + timeout_value->sec;
        pkt_lock.timeout.usec = current.usec + timeout_value->usec;
    } else {
        pkt_lock.timeout.sec = current.sec + 1;
    }

    //pkt_lock.sleep_lock = hal_sleep_manager_set_sleep_handle("pkt_lock");
    pkt_lock.sleep_lock = wifi_set_sleep_handle("pkt_lock");
    if (pkt_lock.sleep_lock == 0xff) {
        return -1;
    }

    //LOG_I(lwip, "sleep lock:%d\n", pkt_lock.sleep_lock);

    return 0;
}

static void pkt_lock_timer_handler(void)
{
    if (pkt_lock.lock_status != 0) {
        if (pkt_lock.pkt_rcv_num_in_sec > 16) {
            extend_pkt_lock_time(NULL);
        } else {
            exit_pkt_lock();
        }
    }  else {
        if (pkt_lock.pkt_rcv_num_in_sec >= 16) {
            enter_pkt_lock();
        } else {
            reset_pkt_lock_status(0, NULL);
            //LOG_I(lwip, "continue unlock\n");
        }
    }
}

#define my_time_before(a, b) \
	((a)->sec < (b)->sec || \
	 ((a)->sec == (b)->sec && (a)->usec < (b)->usec))

static void check_pkt_lock(void)
{
    struct os_time now;
    uint32_t idx;

    /*for debug
    if(pkt_lock.pkt_rcv_num_in_sec == 0){
        enter_pkt_lock();
    }

    if(idx = hal_sleep_manager_get_lock_status())
        LOG_E(lwip,"lock status:%d\n", idx);
    else
        LOG_E(lwip,"unlock status\n");
        */

    inc_pkt_rcv_num();

    os_get_time(&now);
    if (my_time_before(&pkt_lock.timeout, &now)) {
        pkt_lock_timer_handler();
    }
}
#endif

void connsys_set_rxraw_handler(wifi_rx_handler_t handler)
{
    connsys_raw_handler = handler;
}

int32_t wifi_util_get_inf_number(uint8_t *buf)
{
    P_INIT_WIFI_EVENT_T header = (P_INIT_WIFI_EVENT_T)(buf);
    if ((header->aucReserved[0] & IOT_INF_MASK) == IOT_PACKET_TYPE_INF_1) {
        return 1;
    }

    return 0;
}

void connsys_dispatch(void *pkt, uint8_t *payload, int port, unsigned int len)
{
    if (pkt == NULL || payload == NULL) {
        return;
    }

    PKT_HANDLE_RESULT_T handled = PKT_HANDLE_NON_COMPLETE;
    WIFI_EVENT_DATA_PORT_T *wifi_event_hdr = (WIFI_EVENT_DATA_PORT_T *)payload;
    unsigned int packet_total_offset;

    if (pkt != NULL) {
#if (CONNSYS_DEBUG_MODE_EN == 1)
        if (CONNSYS_TEST_DEBUG(CONNSYS_DBG_RX)) {
            DBG_CONNSYS(CONNSYS_DBG_RX, ("\n== Connsys_RX Start ==:\n"));
            DBG_CONNSYS(CONNSYS_DBG_RX, ("pkt = 0x%x, port = %d, len = %u\n",
                                         (unsigned int)pkt,
                                         (int)port,
                                         (unsigned int)len));
            DBG_CONNSYS(CONNSYS_DBG_RX, ("wifi_event_hdr:\n"));
            DBG_CONNSYS(CONNSYS_DBG_RX, ("\tu2Length = 0x%x, u2PktTypeID = 0x%x, ucPacketOffset = 0x%x\n",
                                         wifi_event_hdr->u2Length,
                                         wifi_event_hdr->u2PktTypeID,
                                         wifi_event_hdr->ucPacketOffset));
        }
#endif

        if (len >= WIFI_HIF_HEADER_LEN) {

            if (port == WIFI_HIF_RX_PORT0_IDX) {
                /* cmd packet */
                if (connsys_ops && connsys_ops->deliver_inband) {
                    handled = connsys_ops->deliver_inband(pkt, payload, len);
                }
            } else {
                /* data packet */

                //check_pkt_lock();
                if (connsys_raw_handler != NULL) {
                    if (check_raw_pkt_rssi_threshold(payload, len) < 0) {
                        handled = PKT_HANDLE_COMPLETE;
                        goto complete;
                    }
                    handled = (PKT_HANDLE_RESULT_T)!!(*connsys_raw_handler)(payload, len);
                    if (handled == PKT_HANDLE_COMPLETE) {
                        goto complete;
                    }
                }

                if (handled != PKT_HANDLE_COMPLETE) {
#if (CFG_CONNSYS_IOT_RX_ZERO_COPY_EN == 1)
                    packet_total_offset = WIFI_HIF_HEADER_LEN + wifi_event_hdr->ucPacketOffset;
#else
                    packet_total_offset = WIFI_HIF_HEADER_LEN;
#endif
#if defined(MTK_BSP_LOOPBACK_ENABLE)
                    if (g_loopback_start) {
                        static unsigned int cnt = 0;
                        static unsigned int now = 0, start = 0;
                        cnt += (len - (WIFI_HIF_HEADER_LEN + wifi_event_hdr->aucReserved2[0]));
                        if (now == 0) {
                            now = GPT_return_current_count(2); //DRV_Reg32(0x83050068);
                            start = GPT_return_current_count(2); //DRV_Reg32(0x83050068);
                        }
                        if (cnt > 1024000) {
                            now = GPT_return_current_count(2); //DRV_Reg32(0x83050068);
                            //LOG_I(connsys, "[LB] %d bytes, %d ms, %d Mbps\n", (int)cnt, (now - start) / 32, (int)((cnt * 8) * 32 / (now - start) / 1000));
                            start = now;
                            cnt = 0;
                        }
                        packet_total_offset = WIFI_HIF_HEADER_LEN + wifi_event_hdr->aucReserved2[0]; // Use TX offset field, due to loopback
                    }
#endif // MTK_BSP_LOOPBACK_ENABLE

                    if (len >= packet_total_offset) {
                        int inf = 0;
                        inf = wifi_util_get_inf_number(payload);

                        /* skip HIF header 3DW = 12 bytes */
                        connsys_ops->advance_pkt_hdr(pkt, packet_total_offset);
                        connsys_ops->deliver_tcpip(pkt, payload, len, inf);
                    } else {
                        LOG_HEXDUMP_I(connsys, "len is less than WIFI HIF RX offset", payload, len);
                        handled = PKT_HANDLE_COMPLETE;
                    }
                }
            }
        } else {
            //LOG_I(connsys, "WARN! len is less than WIFI HIF header length.\n");
            handled = PKT_HANDLE_COMPLETE;
        }
    }

complete:
    if (handled == PKT_HANDLE_COMPLETE) {
        connsys_ops->free(pkt);
    }
}

void connsys_intr_enhance_mode_receive_one_data(int32_t port, int16_t rx_len)
{
    void *pkt_ptr = NULL;
    uint8_t *payload_ptr = NULL;
    uint32_t read_len = 0, addr = WRDR1;
#if (CONNSYS_DEBUG_MODE_EN == 1)
    uint32_t flags;
    static uint32_t continuous_allocate_fail_cnt = 0;
#endif

    if (port == WIFI_HIF_RX_PORT0_IDX) {
        addr = WRDR0;
    }

    read_len = rx_len + WIFI_HIF_RX_CS_OFFLOAD_STATUS_LEN;

#if (CONNSYS_DEBUG_MODE_EN == 1)
    if (rx_len > CONNSYS_MAX_RX_PKT_SIZE) {
        DBG_CONNSYS(CONNSYS_DBG_RX,
                    ("WARN! connsys: receive too large pkt, rx_len = %u\n", rx_len));

        /* Driver must read the RX0/1 data completely reported by
           interrupt enhance mode. No interrupt is trigger again for
           the remaining packets.
         */
        local_irq_save(flags);
        g_connsys_stat.rx_port[port].rx_invalid_sz_packet_cnt++;
        if (rx_len > g_connsys_stat.rx_port[port].rx_max_invalid_sz) {
            g_connsys_stat.rx_port[port].rx_max_invalid_sz = rx_len;
        }
        local_irq_restore(flags);
        connsys_bus_read_port_garbage(addr, read_len);
        return;
    }
#endif

    //g_connsys_callback_func.allocate_pkt_callback(CONNSYS_MAX_RX_PKT_SIZE, &pkt_ptr, &payload_ptr);
    connsys_ops->alloc(read_len, &pkt_ptr, &payload_ptr);
    if (pkt_ptr != NULL) {

        connsys_fifo_read(addr, payload_ptr, read_len);
#if (CONNSYS_DEBUG_MODE_EN == 1)
        continuous_allocate_fail_cnt = 0;
#endif
    } else {
#if (CONNSYS_DEBUG_MODE_EN == 1)
        DBG_CONNSYS(CONNSYS_DBG_RX, ("WARN! connsys: can't allocate buffer\n"));
        /* Driver must read the RX0/1 data completely reported by
           interrupt enhance mode. No interrupt is trigger again for
           the remaining packets.
         */
        continuous_allocate_fail_cnt ++;
        if ((continuous_allocate_fail_cnt > CFG_CONNSYS_CONTINOUS_ALLOCATE_FAIL_PRINT_CNT_VAL) &&
                (continuous_allocate_fail_cnt % CFG_CONNSYS_CONTINOUS_ALLOCATE_FAIL_PRINT_CNT_VAL == 1))
            LOG_E(connsys, "WARN! connsys: can't allocate buffer for %u times, %u bytes\n",
                  (unsigned int)continuous_allocate_fail_cnt, read_len);
#endif
        connsys_bus_read_port_garbage(addr, read_len);

#if (CONNSYS_DEBUG_MODE_EN == 1)
        local_irq_save(flags);
        g_connsys_stat.rx_port[port].rx_allocate_fail_cnt ++;
        local_irq_restore(flags);
#endif
        return;
    }

    connsys_dispatch(pkt_ptr, payload_ptr, port, rx_len);

#if (CONNSYS_DEBUG_MODE_EN == 1)
    local_irq_save(flags);
    g_connsys_stat.rx_port[port].rx_packet_cnt ++;
    local_irq_restore(flags);
#endif
}


int32_t connsys_util_intr_enhance_mode_receive_data(void)
{
    uint32_t cr_whisr;
    uint32_t cr_wrplr;
    uint32_t rx_len;
    int32_t  port;
    ssize_t  err;
    uint32_t retry;

    rx_len = 0;
    retry = 0;
    err = 0;

#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
    connsys_measure_time_set(2);
#endif

    // 0. Disable Interrupt
    connsys_disable_interrupt();       //connsys_cr_write(WHLPCR, W_INT_EN_CLR)
    connsys_disable_whier_trx_int();   //WHIER, val &= ~(TX_DONE_INT_EN | RX0_DONE_INT_EN | RX1_DONE_INT_EN);
    connsys_disable_whier_dump_n9_int();

    // 1. Packet Process
    while (retry < 10)
        //while (retry<rtry_level)
    {
        // Read WHISR to Check Interrupt/TX/RX Status
        connsys_cr_read(WHISR, &cr_whisr);
        if (cr_whisr == 0) {
            if (!retry) {
                LOG_E(connsys, "WHISR=0x%x, abort...\n", (unsigned int)cr_whisr);
            }

            // Enable Interrupt
            connsys_enalbe_whier_tx_int();
            connsys_enalbe_whier_rx_int();
            connsys_enable_whier_dump_n9_int();
            connsys_enable_interrupt();
            return (retry) ? CONNSYS_STATUS_SUCCESS : CONNSYS_STATUS_FAIL;
        }

#if (CONNSYS_DEBUG_MODE_EN == 1)
        if (cr_whisr & ABNORMAL_INT) {
            uint32_t reg1, reg2;

            connsys_cr_read(WASR, &reg1);
            connsys_cr_read(WASR2, &reg2);

            g_connsys_stat.number_of_abnormal_int ++;

            LOG_E(connsys, "Abnormal Interrupt: WASR(0x%x), WASR2(0x%x)\n", (unsigned int)reg1, (unsigned int)reg2);
        }

        if (cr_whisr & FW_OWN_BACK_INT) {
            uint32_t reg1;
            connsys_cr_read(WASR, &reg1);
            g_connsys_stat.number_of_fw_own_back ++;
            //LOG_I(connsys,"FW_OWN_BACK_INT: WASR(0x%x)\n", (unsigned int)reg1);
        }
#endif

        // Update TX Status
        if (cr_whisr & TX_DONE_INT) {
#if (CONNSYS_DEBUG_MODE_EN == 1)
            g_connsys_stat.num_of_tx_int ++;
#endif
            connsys_tx_flow_control_update_free_page_cnt();
        }

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
        // Process dump_n9 Packets
       if (cr_whisr & DUPM_N9_QUERY_STATUS_INT) {
#if (CONNSYS_DEBUG_MODE_EN == 1)
           g_connsys_stat.num_of_tx_int ++;
#endif
            //LOG_I(connsys, "receive DUPM_N9_QUERY_STATUS_INT");
            if( context_dump_intr_receive_process()<0 ){
                //LOG_I(connsys, "connsys_dump_intr_receive_process");
                g_dump_param_ctl = 0;
                //goto fail;
            }
            //LOG_I(connsys, "g_dump_param_ctl=%d \n",g_dump_param_ctl);
        }
#endif
        // Process RX0/RX1 Packets
        if (cr_whisr & (RX0_DONE_INT | RX1_DONE_INT)) {
#if (CONNSYS_DEBUG_MODE_EN == 1)
            g_connsys_stat.num_of_rx_int ++;
#endif
            while (1) {
                // Read Port Select
                if (cr_whisr & RX0_DONE_INT) {
                    port = WIFI_HIF_RX_PORT0_IDX;
                    cr_whisr &= ~RX0_DONE_INT;
                } else if (cr_whisr & RX1_DONE_INT) {
                    port = WIFI_HIF_RX_PORT1_IDX;
                    cr_whisr &= ~RX1_DONE_INT;
                } else {
                    break;
                }

                // Read Packet Length
                err = connsys_cr_read(WRPLR, &cr_wrplr);
                if (!err) {
                    rx_len = (port == WIFI_HIF_RX_PORT0_IDX) ? GET_RX0_PACKET_LENGTH(cr_wrplr) : GET_RX1_PACKET_LENGTH(cr_wrplr);
                }

                if (err || !rx_len) {
#if (CONNSYS_DEBUG_MODE_EN == 1)
                    g_connsys_stat.rx_port[port].rx_error_cnt++;
#endif
                    LOG_E(connsys, "Read WRPLR failed. Error = %d", (int)err);
                    // Enable Interrupt
                    connsys_enalbe_whier_tx_int();
                    connsys_enalbe_whier_rx_int();
                    connsys_enable_whier_dump_n9_int();
                    connsys_enable_interrupt();
                    return CONNSYS_STATUS_FAIL;
                }

                // Read Packet Data
                connsys_intr_enhance_mode_receive_one_data(port, rx_len);

                //#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
                //    connsys_measure_time_set_pkt_time(port, i);
                //#endif
            }
        } else {
            break;
        }
        retry++;
    }

#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
    connsys_measure_time_set(3);
    connsys_measure_time_finish();
#endif

    // Enable Interrupt
    connsys_enalbe_whier_tx_int();
    connsys_enalbe_whier_rx_int();
    connsys_enable_whier_dump_n9_int();
    connsys_enable_interrupt();

#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
    connsys_balance_clear_rx_wait();
#endif

    return CONNSYS_STATUS_SUCCESS;
#if 0
fail:
    // Enable Interrupt
    connsys_enalbe_whier_tx_int();
    connsys_enalbe_whier_rx_int();
    connsys_enable_whier_dump_n9_int();
    connsys_enable_interrupt();

    return CONNSYS_STATUS_FAIL;
#endif
}


int32_t connsys_util_low_level_output(uint8_t *buf, ssize_t len, int32_t inf_num)
{
#if defined(__ICCARM__)
    ATTR_4BYTE_ALIGN static uint8_t connsys_tx_buf[SDIO_TX_OUTPUT_BUF_SIZE];
#else
    static uint8_t connsys_tx_buf[SDIO_TX_OUTPUT_BUF_SIZE] __attribute__((aligned(4)));
#endif

    uint8_t *tx_buf = connsys_tx_buf;
    uint8_t fg_need_copy = TRUE;
    uint32_t real_tx_len, tx_len;
    uint32_t clear_len;
    P_INIT_HIF_TX_HEADER_T p_txd;
    int32_t ret = CONNSYS_STATUS_SUCCESS;

#if (CFG_CONNSYS_TXD_PAD_SIZE != 0)
    if (IS_NOT_ALIGN_4((uint32_t)buf) && (g_connsys_func.use_dma)) {
        tx_buf = connsys_tx_buf;
        fg_need_copy = TRUE;
    } else {
        tx_buf = buf;
        fg_need_copy = FALSE;
    }

#endif

    DBG_CONNSYS(CONNSYS_DBG_TX_Q1, ("fg_need_copy = %u\n",
                                    (unsigned int)fg_need_copy));

    tx_len = len + sizeof(INIT_HIF_TX_HEADER_T);
    real_tx_len = ALIGN_4BYTE(tx_len);

    if (real_tx_len > SDIO_TX_OUTPUT_BUF_SIZE) {
        return CONNSYS_STATUS_FAIL;
    }

    clear_len = real_tx_len + WIFI_HIF_TX_BYTE_CNT_LEN;
    if (clear_len > SDIO_TX_OUTPUT_BUF_SIZE) {
        clear_len = SDIO_TX_OUTPUT_BUF_SIZE;
    }
    os_memset(tx_buf, 0, sizeof(INIT_HIF_TX_HEADER_T));

    /*
     * tx_buf = INIT_HIF_TX_HEADER + packet_data
     */
    p_txd = (P_INIT_HIF_TX_HEADER_T)tx_buf;
    p_txd->u2TxByteCount = tx_len;
    p_txd->u2PQ_ID = P1_Q1;
    p_txd->rInitWifiCmd.ucPktTypeID = PKT_ID_CMD;

    if (inf_num == IOT_PACKET_TYPE_INF_1_IDX) {
        p_txd->rInitWifiCmd.ucReserved = IOT_PACKET_TYPE_INF_1;
    } else {
        p_txd->rInitWifiCmd.ucReserved = IOT_PACKET_TYPE_INF_0;
    }


    if (fg_need_copy) {
        /* Fill in packet data */
        os_memmove(p_txd->rInitWifiCmd.aucBuffer, buf, len);
    }

    os_memset((void *)((unsigned int)tx_buf + tx_len), 0, clear_len - tx_len);

    {
        signed int flow_control_status;
        flow_control_status =
            connsys_tx_flow_control_check_and_update_tx(WIFI_HIF_TX_PORT_IDX, real_tx_len);

        if (flow_control_status == CONNSYS_STATUS_SUCCESS) {
            ret = connsys_util_tx_data((uint8_t *)tx_buf, tx_len);
        } else {
            ret = CONNSYS_STATUS_FAIL;
        }
    }

    return ret;
}


#if (WIFI_BUILD_OPTION == 1)
#include "dma_sw.h"

static void connsys_dma_LP_irq_handler()
{
    printf("DMA handler for LP...\n");
}

// only for Low Power emulation
int32_t connsys_emulLP_connsys_open()
{
    struct connsys_func *func = &g_connsys_func;
    int32_t  ret = 0;

    g_connsys_func.blksize = MY_CONNSYS_BLOCK_SIZE;
    g_connsys_func.num = SDIO_GEN3_FUNCTION_WIFI;
    g_connsys_func.irq_callback = NULL;
    g_connsys_func.use_dma = 0; // 1 for DMA mode

    if (g_connsys_func.use_dma) {
        DMA_Register(SDIO_GDMA_CH, connsys_dma_LP_irq_handler);
    }

    // Enable GDMA
    if (g_connsys_func.use_dma) {
        REG32(GDMA_CHANNEL_ENABLE) = 0xFFFFFFFF;
    }

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
    ret = connsys_bus_get_irq(func, connsys_driver_interrupt); /* Interrupt IRQ handler */
    connsys_bus_release_bus(func);
    if (ret) {
        LOG_E(connsys, "<<%s>> Claim irq failed. Error = %d.\n", __FUNCTION__, (int)ret);
        goto err;
    }

    // register to MCU IRQ
//    NVIC_Register(CM4_HIF_IRQ, connsys_irq_handler);
//    NVIC_EnableIRQ(CM4_HIF_IRQ);

    //connsys_bus_writel(func, 0x0, 0x160, &ret);
    //LOG_I(connsys, "write 0x160 done, ret=0x%x\n", ret);

    //LOG_I(connsys, " End of %s\n", __FUNCTION__);
err:
    return ret;
}
#endif //#if (WIFI_BUILD_OPTION == 1)

void connsys_cli_set_debug_falg(uint32_t debug)
{
    //LOG_I(connsys, "set debug flag %u\n", (unsigned int)debug);
    if (debug <= CONNSYS_DBG_MAX_NUM) {
        CONNSYS_SET_DEBUG((1 << debug));
    } else {
        LOG_W(connsys, "WARN! wrong argument. connsys has no such debug flag.\n");
    }
}

void connsys_cli_clear_debug_falg(uint32_t debug)
{
    //LOG_I(connsys, "clear debug flag %u\n", (unsigned int)debug);

    if (debug <= CONNSYS_DBG_MAX_NUM) {
        CONNSYS_CLEAR_DEBUG((1 << debug));
    } else {
        LOG_E(connsys, "WARN! wrong argument. connsys has no such debug flag.\n");
    }
}


void connsys_cli_show_debug_flag()
{
    printf("all debug falgs:\n");
    printf("0:CONNSYS_DBG_RX\n");
    printf("1:CONNSYS_DBG_TX_Q0 (in-band cmd)\n");
    printf("2:CONNSYS_DBG_TX_Q1 (data)\n");
    printf("3:CONNSYS_DBG_PORT\n");

}


void connsys_cli_show_current_debug_flag()
{
    printf("current debug flags:\n");

    if (CONNSYS_TEST_DEBUG(CONNSYS_DBG_RX)) {
        printf("0:CONNSYS_DBG_RX\n");
    }

    if (CONNSYS_TEST_DEBUG(CONNSYS_DBG_TX_Q0)) {
        printf("1:CONNSYS_DBG_TX_Q0\n");
    }

    if (CONNSYS_TEST_DEBUG(CONNSYS_DBG_TX_Q1)) {
        printf("2:CONNSYS_DBG_TX_Q1\n");
    }

    if (CONNSYS_TEST_DEBUG(CONNSYS_DBG_PORT)) {
        printf("3:CONNSYS_DBG_PORT\n");
    }
}


/*
    unit:
        0: in numbers of frames
        1: in numbers of pages
 */

uint32_t pse_get_queue_len(uint32_t pid, uint32_t qid, uint8_t unit)
{
    uint32_t q_len;

    if (unit) {
        REG32(PSE_GQC) = PSE_GQC_QLEN_IN_PAGE_PAGES |
                         (pid << PSE_GQC_GET_QLEN_PID_OFFSET) |
                         (qid << PSE_GQC_GET_QLEN_QID_OFFSET);
    } else {
        REG32(PSE_GQC) = PSE_GQC_QLEN_IN_PAGE_FRAMES |
                         (pid << PSE_GQC_GET_QLEN_PID_OFFSET) |
                         (qid << PSE_GQC_GET_QLEN_QID_OFFSET);
    }

    q_len = (REG32(PSE_GQC) & PSE_GQC_QLEN_RETURN_VALUE_MASK) >>
            PSE_GQC_QLEN_RETURN_VALUE_OFFSET;

    return q_len;
}

uint32_t pse_get_total_page_num(void)
{
    uint32_t total_page_num;
    total_page_num = (REG32(PSE_BC)&PSE_BC_TOTAL_PAGE_NUM_MASK) >> PSE_BC_TOTAL_PAGE_NUM_OFFSET;
    return total_page_num;
}

void pse_show_flow_control_setting(char *queue_name, uint32_t addr)
{
    uint32_t max_quota;
    uint32_t min_reserve;


    max_quota = (REG32(addr)&PSE_FC_MAX_QUOTA_MASK) >> PSE_FC_MAX_QUOTA_OFFSET;
    min_reserve = (REG32(addr)&PSE_FC_MIN_RSRV_MASK) >> PSE_FC_MIN_RSRV_OFFSET;


    if (max_quota == 0xFFF) {
        printf("\t%s(0x%x): max quota setting disable, min reserv setting %u\n",
               queue_name,
               (unsigned int)addr,
               (unsigned int)min_reserve);
    } else {
        printf("\t%s(0x%x): max quota setting %u, min reserv setting %u\n",
               queue_name,
               (unsigned int)addr,
               (unsigned int)max_quota,
               (unsigned int)min_reserve);
    }

}

uint32_t connsys_util_pse_get_p0_min_resv(void)
{
    uint32_t min_reserve;
    min_reserve = (REG32(PSE_FC_P0)&PSE_FC_MIN_RSRV_MASK) >> PSE_FC_MIN_RSRV_OFFSET;
    return min_reserve;
}

void connsys_tx_query_whisr(void)
{
    uint32_t cr_whisr;

    connsys_disable_interrupt();
    connsys_disable_whier_trx_int();
    connsys_disable_whier_dump_n9_int();
    connsys_cr_read(WHISR, &cr_whisr);
    if (cr_whisr & TX_DONE_INT) {
#if (CONNSYS_DEBUG_MODE_EN == 1)
        g_connsys_stat.num_of_tx_int ++;
#endif
        connsys_tx_flow_control_update_free_page_cnt();
    }
    connsys_enalbe_whier_tx_int();
    connsys_enalbe_whier_rx_int();
    connsys_enable_whier_dump_n9_int();
    connsys_enable_interrupt();
}

#ifdef MTK_MINICLI_ENABLE
void pse_show_ffc(void)
{
    uint32_t ffa_cnt;
    uint32_t free_page_cnt;
    ffa_cnt = (REG32(PSE_FC_FFC)&PSE_FC_FFC_FFA_CNT_MASK) >> PSE_FC_FFC_FFA_CNT_OFFSET;
    free_page_cnt = (REG32(PSE_FC_FFC)&PSE_FC_FFC_FREE_PAGE_CNTT_MASK) >> PSE_FC_FFC_FREE_PAGE_CNTT_OFFSET;

    printf("\tFC_FFC(0x%x):\n", (unsigned int)PSE_FC_FFC);
    printf("\t\tFree for all counter status:%u\n", (unsigned int)ffa_cnt);
    printf("\t\tFree page counter status:%u\n", (unsigned int)free_page_cnt);
}


void connsys_cli_show_pse(void)
{
    uint32_t pid;
    uint32_t qid;
    printf("==>connsys_cli_show_pse\n");

    printf("PSE queue length\n");
    printf("Host (P0):\n");
    printf("\tHTX Qs\n");
    pid = 0;
    for (qid = 0; qid < 2; qid++) {
        printf("\tQ%u: %8u frames, %8u pages\n",
               (unsigned int)qid,
               (unsigned int)pse_get_queue_len(pid, qid, 0),
               (unsigned int)pse_get_queue_len(pid, qid, 1));
    }
    printf("\tHRX Qs\n");
    qid = 2;
    printf("\tQ0: %8u frames, %8u pages\n",
           (unsigned int)pse_get_queue_len(pid, qid, 0),
           (unsigned int)pse_get_queue_len(pid, qid, 1));

    pid = 1;
    printf("CPU (P1):\n");
    for (qid = 0; qid < 4; qid++) {
        printf("\tCTX_Q%u: %8u frames, %8u pages\n",
               (unsigned int)qid,
               (unsigned int)pse_get_queue_len(pid, qid, 0),
               (unsigned int)pse_get_queue_len(pid, qid, 1));
    }

    printf("WLAN (P2):\n");
    pid = 2;
    for (qid = 0; qid < 14; qid++) {
        if (qid == 7) {
            printf("\tAC%u(BC/MC): %8u frames, %8u pages\n",
                   (unsigned int)qid,
                   (unsigned int)pse_get_queue_len(pid, qid, 0),
                   (unsigned int)pse_get_queue_len(pid, qid, 1));

        } else if (qid == 8) {
            printf("\tAC%u(Beacon): %8u frames, %8u pages\n",
                   (unsigned int)qid,
                   (unsigned int)pse_get_queue_len(pid, qid, 0),
                   (unsigned int)pse_get_queue_len(pid, qid, 1));
        } else {
            printf("\tAC%u: %8u frames, %8u pages\n",
                   (unsigned int)qid,
                   (unsigned int)pse_get_queue_len(pid, qid, 0),
                   (unsigned int)pse_get_queue_len(pid, qid, 1));
        }
    }

    pid = 3;
    printf("PSE (P3):\n");
    printf("\tSWitch Qs:\n");
    for (qid = 0; qid < 8; qid++) {
        printf("\tSW%u: %8u frames, %8u pages\n",
               (unsigned int)qid,
               (unsigned int)pse_get_queue_len(pid, qid, 0),
               (unsigned int)pse_get_queue_len(pid, qid, 1));
    }
    printf("\tFree Q:\n");
    qid = 8;
    printf("\tFree: %8u frames, %8u pages\n",
           (unsigned int)pse_get_queue_len(pid, qid, 0),
           (unsigned int)pse_get_queue_len(pid, qid, 1));

    printf("Free control related:\n");
    printf("Total page number:%u\n", (unsigned int)pse_get_total_page_num());
    pse_show_flow_control_setting("P0", PSE_FC_P0);
    pse_show_flow_control_setting("P1", PSE_FC_P1);
    pse_show_flow_control_setting("P2 Q0", PSE_FC_P2_Q0);
    pse_show_flow_control_setting("P2 Q1", PSE_FC_P2_Q1);
    pse_show_flow_control_setting("P2 Q2", PSE_FC_P2_Q2);
    pse_show_ffc();
}


void connsys_cli_dump_pse_reg(void)
{
    uint32_t offset;
    uint32_t addr;

    printf("==>connsys_cli_dump_pse_reg\n");
    printf("WF_PSE_TOP_BASE: 0x%x\n", WF_PSE_TOP_BASE);


    for (offset = 0x0; offset <= 0x18; offset += 4) {
        addr = offset + WF_PSE_TOP_BASE;
        printf("Addr: 0x%x, 0x%x\n",
               (unsigned int)addr,
               (unsigned int)REG32(addr));
    }


    for (offset = 0x118; offset <= 0x150; offset += 4) {
        addr = offset + WF_PSE_TOP_BASE;
        printf("Addr: 0x%x, 0x%x\n",
               (unsigned int)addr,
               (unsigned int)REG32(addr));
    }
}


void connsys_cli_show_help(void)
{
    printf("connsys cli help:\n");
    printf("Usage: c <param1> <param2>\n");
    printf("The following are description for param1:\n");
    printf("\t0: show connsys debug flag\n");
    printf("\t1: set connsys debug flag\n");
    printf("\t    e.g. '$c 1 2' ==>enable TX Q1 debug\n");
    printf("\t2: clear connsys debug flag\n");
    printf("\t    e.g. '$c 2 1' ==>disable TX Q0 debug\n");
    printf("\t3: show current connsys debug flag\n");
    printf("\t4: show stat\n");
    printf("\t5: dump CR\n");
    printf("\t6: show tx flow control info\n");
    printf("\t7: show connsys compiler option\n");
    printf("\t8: read whisr register\n");
    printf("\t9: trigger connsys interrupt handler\n");
    printf("\t10: show connsys time measure\n");
    printf("\t11: set tx flow control config, 1: enable, 0: disable\n");
    printf("\t12: show pse info \n");
    printf("\t13: dump pse reg \n");
    printf("\t14: set tx flow control reserve page \n");
    printf("\t15: show last interrupt enhance mode struct\n");
    printf("\t16: set tx/rx balance config, 1: enable, 0: disable\n");
    printf("\t17: set max continus tx num for balance\n");
    printf("\t18: set max continus rx num for balance\n");
    printf("\t19: show tx/rx balance info\n");
    printf("\t20: show headroom offset for QoS/non-QoS\n");
    printf("\t21: set headroom offset for QoS, ex: c 21 42 => set offset to 42 bytes\n");
    printf("\t22: set headroom offset for non-QoS, ex: c 22 38 => set offset to 38 bytes\n");
    printf("\t23: get connsys driver domain CR, ex: c 23 0x14      => get WHIER(0x14)\n");
    printf("\t24: set connsys driver domain CR, ex: c 24 0x14  0x0 => set WHIER(0x14) to 0\n");
#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
    printf("\t25: dump n9 test\n");
    printf("\t26: dump n9 debug\n");
#endif
}

uint8_t connsys_util_cli_handler(uint8_t len, char *param[])
{
    //LOG_I(connsys, "==>connsys_util_cli_handler\n");

    if (len == 0) {
        printf("incomplete command\n");
        connsys_cli_show_help();
        return 0;
    }


    switch (atoi(param[0])) {
        case 0: {
            connsys_cli_show_debug_flag();
        }
        break;

        case 1: {
            uint32_t debug_flag;
            debug_flag = atoi(param[1]);
            if (len != 2) {
                LOG_E(connsys, "WARN! wrong argument\n");
            } else {
                connsys_cli_set_debug_falg(debug_flag);
            }
        }
        break;

        case 2: {
            uint32_t debug_flag;
            debug_flag = atoi(param[1]);
            if (len != 2) {
                printf("WARN! wrong argument\n");
            } else {
                connsys_cli_clear_debug_falg(debug_flag);
            }
        }
        break;

        case 3: {
            connsys_cli_show_current_debug_flag();
        }
        break;

        case 4: {
            connsys_print_stat();
        }
        break;

        case 5: {
            connsys_dump_cr();
        }
        break;

        case 6: {
            connsys_tx_flow_control_show_info();
        }
        break;
        case 7: {
            connsys_show_config_option();
        }
        break;

        case 8: {
            connsys_dump_whisr();
        }
        break;
        case 9: {
            printf("trigger connsys interrupt by cmd\n");
            connsys_driver_interrupt(&g_connsys_func);
        }
        break;


        case 10: {
#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
            connsys_measure_time_show();
#else
            printf("WARN! not support\n");
#endif
        }
        break;
        case 11: {
            uint8_t config;
            config = (uint8_t)atoi(param[1]);
            printf("Configure tx flow control, 1: enable, 0: disable\n");
            if (len != 2) {
                printf("WARN! wrong argument\n");
            } else {
                connsys_tx_flow_control_config(config);
            }
        }
        break;
        case 12: {
            connsys_cli_show_pse();
        }
        break;

        case 13: {
            connsys_cli_dump_pse_reg();
        }
        break;

        case 14: {
            uint32_t reserve_page_num;
            reserve_page_num = (uint32_t)atoi(param[1]);
            printf("set tx flow control reserve page\n");
            if (len != 2) {
                printf("WARN! wrong argument\n");
            } else {
                connsys_tx_flow_control_set_reserve_page(reserve_page_num);
            }
        }
        break;

        case 15: {
            connsys_util_intr_enhance_mode_dump_last_struct();
        }
        break;

#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
        case 16: {
            uint8_t config;
            config = (uint32_t)atoi(param[1]);
            printf("Configure tx flow control, 1: enable, 0: disable\n");
            if (len != 2) {
                printf("WARN! wrong argument\n");
            } else {
                connnsys_balance_set_enable(config);
            }
        }
        break;

        case 17: {
            uint32_t num;
            num = (uint32_t)atoi(param[1]);
            printf("set max continus tx num for balance\n");
            if (len != 2) {
                printf("WARN! wrong argument\n");
            } else {
                connnsys_balance_set_max_num_of_continuous_tx(num);
            }
        }
        break;
        case 18: {
            uint32_t num;
            num = (uint32_t)atoi(param[1]);
            printf("set max continus rx num for balance\n");
            if (len != 2) {
                printf("WARN! wrong argument\n");
            } else {
                connnsys_balance_set_max_num_of_continuous_rx(num);
            }
        }
        break;

        case 19: {
            connnsys_balance_show_info();
        }
        break;
#endif /* (CFG_CONNSYS_TRX_BALANCE_EN == 1) */
        case 20: {
            printf("Headroom offset for QoS=%d bytes, for non-QoS=%d bytes\n",
                   (int)connsys_get_headroom_offset(CONNSYS_HEADROOM_OFFSET_QOS),
                   (int)connsys_get_headroom_offset(CONNSYS_HEADROOM_OFFSET_NON_QOS));
        }
        break;
        case 21: {
            uint32_t offset;
            offset = (uint32_t)atoi(param[1]);
            connsys_set_headroom_offset(CONNSYS_HEADROOM_OFFSET_QOS, offset);
            printf("set headroom offset for QoS=%d bytes\n", (int)offset);
        }
        break;
        case 22: {
            uint32_t offset;
            offset = (uint32_t)atoi(param[1]);
            connsys_set_headroom_offset(CONNSYS_HEADROOM_OFFSET_NON_QOS, offset);
            printf("set headroom offset for Non-QoS=%d bytes\n", (int)offset);
        }
        break;
        case 23: {
            uint8_t  type;
            uint32_t reg, value;

            reg = toi(param[1], &type);
            connsys_cr_read(reg, &value);
            printf("CR[0x%x]=0x%x\n", (unsigned int)reg, (unsigned int)value);
            break;
        }

        case 24: {
            uint8_t  type;
            uint32_t reg, value;

            reg = toi(param[1], &type);
            value = toi(param[2], &type);
            connsys_cr_write(reg, value);
            printf("Write CR[0x%x]=0x%x\n", (unsigned int)reg, (unsigned int)value);
            break;
        }
#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
        case 25: {
            connsys_dump_parameter_t dump_temp;
            dump_temp.trigger_source = 1;
            dump_temp.dump_type = 0;
            dump_temp.process_flag = 0;
            //context_dump_trigger(&dump_temp);
            context_dump_trigger_by_sema(&dump_temp);
            break;
        }
        case 26: {
            context_dump_n9_debug( );
            context_dump_status();
            break;
        }
#endif
        default:
            printf("WARN! invalid option %s\n", param[0]);
            connsys_cli_show_help();
            break;
    }

    return 0;
}
#endif

