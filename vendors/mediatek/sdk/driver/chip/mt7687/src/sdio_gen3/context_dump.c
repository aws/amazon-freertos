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
#include "memory_attribute.h"
#include "connsys_profile.h"
#include "syslog.h"
#include "flash_map.h"

#include "task_def.h"
#include "context_dump.h"
#include "exception_handler.h"

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE) 

extern uint32_t get_n9_ext_crash_context_base(void);
extern uint32_t get_n9_ext_crash_context_length(void);

extern uint32_t get_n9_crash_context_base(void);
extern uint32_t get_n9_crash_context_length(void);

#if 0
static void connsys_dump_write_data_to_share()
{
    uint8_t i;
    uint8_t test[4]={0xa5, 0xa5, 0xa5, 0xa5};
    unsigned char *pch;

    pch = (unsigned char *)SHARE_MEM_START_ADDR;
    LOG_W(connsys, "connsys_dump_write_data_to_share\n");
    for( i=0; i<25; i++){
        memcpy(pch+(4*i), test, 4);
        //HAL_REG_32(0x2003EC00 + (i * 4)) = 0xa5a5a5a5;
    }
    LOG_W(connsys, "DATA Write complete\n");
//    connsys_enable_dump_n9_set();
}
#endif

uint8_t                  g_check_n9_no_inband_rsp_cnt = 0;
connsys_dump_parameter_t g_dump_param;
uint8_t                  g_dump_param_ctl = 0;
TimerHandle_t            g_check_n9_alive_timer;
uint8_t                  g_have_a_dump = 0;
SemaphoreHandle_t        g_trigger_dump_semaphore = NULL;

void context_dump_set_inband_cmd_no_rsp_cnt( uint8_t rsp_cnt)
{
    g_check_n9_no_inband_rsp_cnt = rsp_cnt;
}

uint8_t context_dump_get_inband_cmd_no_rsp_cnt(void)
{
    return g_check_n9_no_inband_rsp_cnt;
}

void context_dump_triger_task(void *param)
{
    while(1){
        if( g_trigger_dump_semaphore != NULL ){
           if( pdTRUE==xSemaphoreTake( g_trigger_dump_semaphore, portMAX_DELAY ) ) {
                printf( "context_dump_triger_task \n" );
                context_dump_trigger(&g_dump_param);
                printf("trigger dump finished\n");
            }
        }else{
            printf("g_trigger_dump_semaphore is NULL");
        }
    }
}


int context_dump_task_create(void)
{
    if ( xTaskCreate(context_dump_triger_task,
                    DUMP_TASK_NAME,
                    DUMP_TASK_STACKSIZE / sizeof(portSTACK_TYPE),
                    NULL,
                    DUMP_TASK_PRIORITY,
                    NULL) != pdPASS ) {
        LOG_E(common, "xTaskCreate fail");
        return -1;
    }
    printf("context_dump_task_create \n");
    return 0;
}

static void context_dump_check_n9_alive_proc(TimerHandle_t xTimer)
{
    uint8_t ch = 0;
    connsys_dump_parameter_t dump_temp;
    static uint8_t smock_cmd_send = 0;
    uint8_t i = 0;
    
    LOG_D(connsys, "context_dump_check_n9_alive_proc, %s, %d \n", __FUNCTION__, __LINE__);
    /*5 cmds no rsp, it will trigger dump N9*/
    if( 5 < context_dump_get_inband_cmd_no_rsp_cnt() ){
        printf("context_dump_check_n9_alive_proc dump \n");
        dump_temp.trigger_source = 1;
        dump_temp.dump_type = 0;
        dump_temp.process_flag = 0;
        context_dump_trigger(&dump_temp);
        xTimerDelete(g_check_n9_alive_timer, 0);
        context_dump_set_inband_cmd_no_rsp_cnt(0);
    }

    /*per 10s send 5 cms to N9*/
    smock_cmd_send++;
    if( 5==smock_cmd_send ){
        for(i=0; i<5; i++){
            wifi_config_get_channel(WIFI_PORT_STA, &ch);
        }
        LOG_D(connsys, "context_dump_check_n9_alive_proc send inband \n");
        smock_cmd_send = 0;
    }
}

static void context_dump_n9_alive_periodic( )
{
    /* per 2s check dump, per 10s send 5 inband cmds */
    g_check_n9_alive_timer = xTimerCreate("g_check_n9_alive_timer",
                                        (2*1000 / portTICK_PERIOD_MS), /*the period being used.*/
                                        pdTRUE,
                                        NULL,
                                        context_dump_check_n9_alive_proc);
    xTimerStart(g_check_n9_alive_timer, 0);
}

extern void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen);
static void context_dump_read_from_share()
{
    unsigned char *pch = NULL;

    pch = (unsigned char *)SHARE_MEM_START_ADDR;
    printf("context_dump_read_from_share\n");
    hex_dump("context_dump_read_from_share", pch, SHARE_MEM_LENGTH);
}

static void context_dump_clean(void)
{   
    uint32_t i;
    uint8_t test[4]={0x00, 0x00, 0x00, 0x00};
    unsigned char *pch;

    pch = (unsigned char *)SHARE_MEM_START_ADDR;
    printf("context_dump_clean\n");
    for( i=0; i<(SHARE_MEM_LENGTH/4); i++){
        memcpy(pch+(4*i), test, 4);
        //HAL_REG_32(0x2003EC00 + (i * 4)) = 0xa5a5a5a5;
    }
    g_dump_param_ctl = 0;
    printf("DATA clean complete\n");
}

/*
    * @return  >=0 means success, <0 means fail
*/
static int32_t context_dump_intr_save_dump_data(int32_t start_addr, int16_t data_len)
{
    uint8_t *pdump_temp = NULL;
    int16_t dump_header_len = 0;
    int16_t dump_len = 0;
	uint32_t address = 0;
    
    LOG_I(connsys,"==>");
    dump_header_len = sizeof(connsys_dump_parameter_t) + 4;
    dump_len = dump_header_len + data_len;
    if( (start_addr < SHARE_MEM_START_ADDR) || (dump_len > SHARE_MEM_LENGTH) ){
         LOG_E(connsys, "<<%s>> Read connsys_dump_parameter_t check failed. Error = %d.\n", __FUNCTION__, __LINE__);
         return CONNSYS_STATUS_FAIL;
    }
    
    pdump_temp = (uint8_t *)os_malloc(dump_len);
    if(pdump_temp == NULL) {
        LOG_E(connsys,"context_dump_intr_save_dump_data, malloc fail");
        return CONNSYS_STATUS_FAIL;
    }
    
    /*copy header, copy data*/
    memset(pdump_temp, 0, dump_len);
    memcpy(pdump_temp, (unsigned char *)SHARE_MEM_START_ADDR, dump_header_len);
    memcpy(pdump_temp+dump_header_len, (unsigned char *)start_addr, data_len);

    /* Save to falsh*/
    if( do_write_to_ext_flash() ){
		address = get_n9_ext_crash_context_base();
        hal_flash_erase_wrap(address, HAL_FLASH_BLOCK_4K);
        hal_flash_write_wrap(address, pdump_temp, dump_len);
    }else{
         address = get_n9_crash_context_base();
        hal_flash_erase(address, HAL_FLASH_BLOCK_4K);
        hal_flash_write(address, pdump_temp, dump_len);
    }
    if(pdump_temp != NULL) {
        os_free(pdump_temp);
    }
    LOG_I(connsys, "context_dump_intr_save_dump_data, finish write to flash,CRASH_N9_DUMP_BASE=0x %x data_len=%d\n",CRASH_N9_DUMP_BASE, data_len);
    return CONNSYS_STATUS_SUCCESS;
}

 /*
     * @return  >=0 means success, <0 means fail
 */
int32_t context_dump_intr_receive_process(void)
{
    connsys_dump_parameter_t dump_param;
    unsigned char *pch;

    pch = (unsigned char *)SHARE_MEM_START_ADDR;
    LOG_I(connsys,"==>");

    /* Read share memory*/
    memset(&dump_param, 0, sizeof(connsys_dump_parameter_t));
    memcpy(&dump_param, pch+4, sizeof(connsys_dump_parameter_t));
    LOG_I(connsys, "connsys_dump_parameter. trig_src=%x, type=%x, process_flag=%x, START_ADDR=0x%x, dump_len=%x,g_.trigger_source=%x\n", 
       dump_param.trigger_source, dump_param.dump_type, dump_param.process_flag, dump_param.dump_start_addr, dump_param.dump_len, g_dump_param.trigger_source);

    if( 2==g_dump_param_ctl ){
        /*trigger by cm4*/
        LOG_D(connsys, "cmd source is cm4\n"); 
        g_dump_param_ctl = 3;
        //connsys_disable_dump_n9_clr();
        if( (dump_param.trigger_source==g_dump_param.trigger_source)&& (DUMP_PROCESS_FLAG_FIN_WRITING == dump_param.process_flag) ){
           g_dump_param_ctl = 4;
           if( 0==context_dump_intr_save_dump_data(dump_param.dump_start_addr, dump_param.dump_len) ){
              g_dump_param_ctl = 5;
           }
        }
    }else{
        /*trigger by N9*/
        LOG_I(connsys, "received int from n9, cmd source is not cm4\n"); 
        if( (DUMP_TRIGGER_SRC_N9_SELFCHECK_CRASH==dump_param.trigger_source) && (DUMP_PROCESS_FLAG_FIN_WRITING==dump_param.process_flag)){
            if( 0==context_dump_intr_save_dump_data(dump_param.dump_start_addr, dump_param.dump_len) ){
              g_dump_param_ctl = 5;
            }
        }
    }
    
    if( 5== g_dump_param_ctl)
    {
        context_dump_clean();
    }

    return CONNSYS_STATUS_SUCCESS;
}

void context_dump_init(void)
{
    printf("context_dump_init \n");
    if (g_trigger_dump_semaphore == NULL) {
        g_trigger_dump_semaphore = xSemaphoreCreateBinary();
        if (g_trigger_dump_semaphore == NULL) {
            LOG_E(connsys, "g_trigger_dump_semaphore create failed.\n");
            return ;
        }
    }
    printf("g_trigger_dump_semaphore create \n");
    context_dump_task_create();
    context_dump_n9_alive_periodic();
    context_dump_clean();
}

/*0 no dump cmd
*  1 cm4 get dump_n9 cmd
*  2 cm4 wirte finished share memory and send INT to N9
*  3 cm4 received N9 INT
*  4 cm4 start read share data
*  5 cm4 finish read share memory/reset share memory
*/

/*
    * @return  >=0 means success, <0 means fail
*/
int8_t context_dump_trigger(connsys_dump_parameter_t *dump_param)
{
    unsigned char *pch = NULL;

    /*Check use have set flag*/
    if( !is_crash_dump_save_to_flash() ){
        LOG_E(connsys, "crash_dump_save_to_flash flag is disable, can not save.\n");
        return -1;
    }
    printf("context_dump_trigger, trigger_source=%d g.trigger_source=%d \n",dump_param->trigger_source, g_dump_param.trigger_source);
    /* Check last dump*/
    if( g_dump_param_ctl!=0 ){
        LOG_W(connsys, "context_dump_trigger is ongoing, waiting to finish dump, g_dump_param_ctl=%d g.trigger_src=%d \n", 
            g_dump_param_ctl, g_dump_param.trigger_source);  
        return -1;
    }
    context_dump_clean();
    /* Check input*/
    if( NULL == dump_param ){
        return -1;
    }
    g_dump_param_ctl = 1;
    g_have_a_dump = 1;
   /* write data to share memory*/
    pch = (unsigned char *)SHARE_MEM_START_ADDR;
    memcpy(pch+4, dump_param, sizeof(connsys_dump_parameter_t));
    g_dump_param.trigger_source = dump_param->trigger_source;

    /* trigger INT to notice N9*/
    printf("trigger_source=%d g.trigger_source=%d \n",dump_param->trigger_source, g_dump_param.trigger_source);
#if 1   
    if(CONNSYS_STATUS_SUCCESS != connsys_enable_dump_n9_set()){
        LOG_E(connsys, "FAIL. write WSICR failed.\n");
        g_dump_param_ctl = 0;
        return -1;  
    }
#endif
    g_dump_param_ctl = 2;
    return 0;
}

int8_t context_dump_trigger_by_sema(connsys_dump_parameter_t *dump_param)
{
    connsys_dump_parameter_t *pconn_dump_param = NULL;

    pconn_dump_param = &g_dump_param;
    memset(pconn_dump_param, 0, sizeof(connsys_dump_parameter_t));
    pconn_dump_param->trigger_source = dump_param->trigger_source;
    pconn_dump_param->dump_type = dump_param->dump_type;
    pconn_dump_param->process_flag = dump_param->process_flag;
    if( NULL!=g_trigger_dump_semaphore ){
        printf("trigger g_trigger_dump_semaphore \n");
        xSemaphoreGive( g_trigger_dump_semaphore );
    }else{
        printf("g_trigger_dump_semaphore have not created\n");
        return -1;
    }
    return 0;
}

/*
    * @return  =0 means dumped, 1 means dumping, -1 means no dump cmd
*/
int8_t context_dump_status(void)
{
    LOG_I(connsys, "connsys_trigger_dump_status. g_have_a_dump=%d g_dump_param_ctl=%d \n", 
        g_have_a_dump, g_dump_param_ctl);
    if( 1==g_have_a_dump ){
        if(0==g_dump_param_ctl){
            g_have_a_dump = 0;
            return 0;
        }else{
            return 1;
        }
    }else{
        return -1;
    }
    
}

void context_dump_n9_debug(void)
{
    context_dump_read_from_share();
    printf("g_dump_param_ctl=%d g.trigger_source=%d g.process_flag=%d g_check_n9_no_inband_rsp_cnt=%d \n", 
        g_dump_param_ctl, g_dump_param.trigger_source, g_dump_param.process_flag, context_dump_get_inband_cmd_no_rsp_cnt());
}

#endif


