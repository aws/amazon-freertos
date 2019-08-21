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

#include "hal_gdma.h"

#ifdef HAL_GDMA_MODULE_ENABLED
#include "hal_pdma_internal.h"
#include "hal_gdma_internal.h"
#include "hal_clock.h" 
#include "assert.h"


/*peripheral dma base address array */
PDMA_REGISTER_T  *pdma[PDMA_NUMBER] = {PDMA3,PDMA4, PDMA5, PDMA6, PDMA7, PDMA8, PDMA9, PDMA10, PDMA11};

/*peripheral dma global status base address array */
PDMA_REGISTER_GLOBAL_T  *pdma_global[PDMA_NUMBER] = {PDMA_GLOAL1, PDMA_GLOAL1, PDMA_GLOAL1, PDMA_GLOAL1,PDMA_GLOAL1, PDMA_GLOAL1, PDMA_GLOAL1, PDMA_GLOAL1,
                                                      PDMA_GLOAL1};

/*peripheral dma global status running bit location array */
uint32_t g_pdma_global_running_bit[PDMA_NUMBER] = {PDMA_CHANNEL3_OFFSET,PDMA_CHANNEL4_OFFSET, PDMA_CHANNEL5_OFFSET, PDMA_CHANNEL6_OFFSET,PDMA_CHANNEL7_OFFSET, PDMA_CHANNEL8_OFFSET, PDMA_CHANNEL9_OFFSET,
                                                  PDMA_CHANNEL10_OFFSET, PDMA_CHANNEL11_OFFSET};

/*peripheral dma init status*/
volatile static uint8_t pdma_init_status[PDMA_END_CHANNEL] = {0};

/*virtual fifo dma base address array */
VDMA_REGISTER_T  *vdma[VDMA_NUMBER] = {VDMA12, VDMA13,VDMA14, VDMA15,VDMA16, VDMA17,VDMA18, VDMA19,VDMA20, VDMA21, VDMA22, 
                                       VDMA23, VDMA24, VDMA25};

/*virtual fifo dma port address array */
VDMA_REGISTER_PORT_T  *vdma_port[VDMA_NUMBER] = {VDMA12_PORT, VDMA13_PORT,VDMA14_PORT, VDMA15_PORT,VDMA16_PORT, VDMA17_PORT,VDMA18_PORT, VDMA19_PORT, VDMA20_PORT, VDMA21_PORT, VDMA22_PORT,
                                                 VDMA23_PORT, VDMA24_PORT,VDMA25_PORT};


/*virtual fifo dma global status base address array */
VDMA_REGISTER_GLOBAL_T  *vdma_global[VDMA_NUMBER] = {VDMA_GLOAL1, VDMA_GLOAL1,VDMA_GLOAL1, VDMA_GLOAL1, VDMA_GLOAL1, VDMA_GLOAL2, VDMA_GLOAL2,
                                                     VDMA_GLOAL2, VDMA_GLOAL2, VDMA_GLOAL2,VDMA_GLOAL2, VDMA_GLOAL2, VDMA_GLOAL2, VDMA_GLOAL2
                                                    };

/*virtual fifo dma global status running bit location array */
uint32_t g_vdma_global_running_bit[VDMA_NUMBER] = {VDMA_CHANNEL12_OFFSET, VDMA_CHANNEL13_OFFSET,VDMA_CHANNEL14_OFFSET, VDMA_CHANNEL15_OFFSET, 
                                                   VDMA_CHANNEL16_OFFSET, VDMA_CHANNEL17_OFFSET, VDMA_CHANNEL18_OFFSET, VDMA_CHANNEL19_OFFSET, 
                                                   VDMA_CHANNEL20_OFFSET, VDMA_CHANNEL21_OFFSET,VDMA_CHANNEL22_OFFSET, VDMA_CHANNEL23_OFFSET,
                                                   VDMA_CHANNEL24_OFFSET, VDMA_CHANNEL25_OFFSET
                                                  };

/*virtual fifo dma init status*/
volatile static uint8_t vdma_init_status[VDMA_END_CHANNEL] = {0};

extern GDMA_REGISTER_GLOBAL_T  *gdma_global[GDMA_NUMBER];
extern GDMA_REGISTER_T  *gdma[GDMA_NUMBER];
extern volatile uint32_t gdma_status[GDMA_NUMBER];

extern gdma_user_callback_t g_gdma_callback[GDMA_NUMBER];
extern uint32_t g_gdma_global_running_bit[GDMA_NUMBER];


/*define peripheral dma's  callback function structure*/
typedef struct {
    pdma_callback_t func;
    void *argument;
} pdma_user_callback_t;

/*peripheral dma's  callback function array*/
static pdma_user_callback_t g_pdma_callback[PDMA_NUMBER] = {
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},    
    {NULL, NULL}
};

/*define virtual fifo  dma's  callback function structure*/
typedef struct {
    vdma_callback_t func;
    void *argument;
} vdma_user_callback_t;

/*virtual fifo  dma's  callback function array*/
static vdma_user_callback_t g_vdma_callback[VDMA_NUMBER] = {
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL}
};

static vdma_user_callback_t g_vdma_tocallback[VDMA_NUMBER] = {
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL}
};


void dma_enable_clock(uint8_t channel)
{
    *(volatile kal_uint32 *)(CONFG_MCCR_CLR_ADDR) = 1 << (channel);
}

void dma_disable_clock(uint8_t channel)
{
    *(volatile kal_uint32 *)(CONFG_MCCR_SET_ADDR) = 1 << (channel);
}

void gdma_set_control(uint32_t index, uint32_t control, uint32_t count)
{
    gdma[index]->GDMA_CON = control;
    gdma[index]->GDMA_COUNT = count;
}

void gdma_set_address(uint32_t index, uint32_t destination, uint32_t source)
{
    gdma[index]->GDMA_SRC = source;
    gdma[index]->GDMA_DST = destination;

}

/*************************************************************************************************************/
/************************************peripheral  dma function start  line********************************************/
/*************************************************************************************************************/

static uint32_t pdma_check_valid_channel(pdma_channel_t channel)
{
    /*define  peripheral dma base adddress array offset*/
    uint32_t offset = 0;

    /*calucate  current channel 's  offset in peripheral dma base adddress array */
    offset = channel - PDMA_START_CHANNEL;

    if (offset >= PDMA_NUMBER) {
        return INVALID_STATUS;
    } else {
        return offset;
    }
}


/* init peripheral dma pdn enable */
pdma_status_t pdma_init(pdma_channel_t channel)
{

    uint32_t offset = 0;

    offset = pdma_check_valid_channel(channel);

    if (0xff == offset) {
        return PDMA_ERROR_CHANNEL;
    }

    /*check pdma init*/
    PDMA_CHECK_AND_SET_INIT(channel);

    /*un-gating peripheral dma clock*/
	dma_enable_clock(channel);


    /*reset pdma default setting*/
    pdma[offset]->PDMA_CON= 0x0;
    pdma[offset]->PDMA_COUNT = 0x0;
    pdma[offset]->PDMA_START = 0x0;
    pdma[offset]->PDMA_INTSTA = 0x0;
    pdma[offset]->PDMA_PGMADDR = 0x0;

    /*register isr handler*/
    hal_nvic_register_isr_handler(CM4_DMA_IRQ, dma_interrupt_hander);
	NVIC_SetPriority((IRQn_Type)CM4_DMA_IRQ, CM4_DMA_PRI);
    NVIC_EnableIRQ(CM4_DMA_IRQ);
    return  PDMA_OK;
}



/*de-init peripheral dma */
pdma_status_t pdma_deinit(pdma_channel_t channel)
{
    uint32_t offset = 0;
    uint32_t saved_mask;
    /*define peripheral dma global status tmp variable*/
    volatile uint32_t  global_status = 0;
    offset = pdma_check_valid_channel(channel);
    if (0xff == offset) {
        return PDMA_ERROR_CHANNEL;
    }

    saved_mask = save_and_set_interrupt_mask();
    global_status = pdma_global[offset]->PDMA_GLBSTA;

    if (global_status & (0x1 << (g_pdma_global_running_bit[offset] + 1))) {

        assert(0);
        restore_interrupt_mask(saved_mask);
        return PDMA_INVALID_PARAMETER;
    }

    /*check if this pdma channel  is running now  */
    if (global_status & (0x1 << g_pdma_global_running_bit[offset])) {

        assert(0);
        restore_interrupt_mask(saved_mask);
        return PDMA_INVALID_PARAMETER;
    }
    restore_interrupt_mask(saved_mask);

    /*set pdma idle*/
    PDMA_SET_IDLE(channel);

    /*gating peripheral dma clock*/
    dma_disable_clock(channel);

    return  PDMA_OK;

}

pdma_status_t pdma_set_ring_buffer(pdma_channel_t channel, uint32_t wppt, uint32_t wpto)

{
    uint32_t offset = 0;

    offset = pdma_check_valid_channel(channel);
    if (0xff == offset) {
        return PDMA_ERROR_CHANNEL;
    }

    /* check wppt value valid or not */
    if (wppt > 0xFFFF) {
        return PDMA_INVALID_PARAMETER;
    }

    /* set wppt value in wrap point address register */
    pdma[offset]->PDMA_WPPT = wppt;

    /* set wpto value in wrap to address register */
    pdma[offset]->PDMA_WPTO = wpto;

    return PDMA_OK;

}



pdma_status_t pdma_configure(pdma_channel_t channel, pdma_config_t *pdma_config)

{
    /* define  peripheral dma base adddress array offset */
    uint32_t offset = 0;

    uint32_t saved_mask;

    /* define  peripheral dma control tmp variable */
    uint32_t dma_control = 0;

    /*define peripheral dma global status tmp variable*/
    volatile uint32_t  global_status = 0;

    offset = pdma_check_valid_channel(channel);
    if (0xff == offset) {
        return PDMA_ERROR_CHANNEL;
    }


    /* burst mode if true*/
    if (pdma_config->burst_mode == true) {
        dma_control |= PDMA_CON_BURST_4BEAT_MASK;
    }
    /*single mode if false*/
    else if (pdma_config->burst_mode == false) {
        dma_control &= ~PDMA_CON_BURST_MASK;
    } else {
        return PDMA_INVALID_PARAMETER;
    }

    saved_mask = save_and_set_interrupt_mask();

    /* check whether pdma is in running mode  */
    global_status = pdma_global[offset]->PDMA_GLBSTA;

    if (global_status & (0x1 << g_pdma_global_running_bit[offset])) {

        /*pdma is running now,assert here may be better*/
        restore_interrupt_mask(saved_mask);
        return PDMA_ERROR;

    }

    /*check wether pdma 's interrrupt is triggered*/

    if (global_status & (0x1 << (g_pdma_global_running_bit[offset] + 1))) {

        /*pdma's interrupt is triggered ,assert here may be better*/
        restore_interrupt_mask(saved_mask);
        return PDMA_ERROR;

    }
    restore_interrupt_mask(saved_mask);

    /*make sure that  pdma is stopped before start pdma*/
    pdma[offset]->PDMA_START &= ~PDMA_START_BIT_MASK;

    switch (pdma_config->master_type) {

        case  PDMA_TX:
            dma_control |= PDMA_CON_SINC_MASK;
            break;
        case  PDMA_RX:
            dma_control |= PDMA_CON_DIR_MASK;
            dma_control |= PDMA_CON_DINC_MASK;
            break;
        case  PDMA_TX_RINGBUFF:
            dma_control |= PDMA_CON_SINC_MASK;
            dma_control |= PDMA_CON_WPEN_MASK;
            break;
        case  PDMA_RX_RINGBUFF:
            dma_control |= PDMA_CON_DIR_MASK;
            dma_control |= PDMA_CON_SINC_MASK;
            dma_control |= PDMA_CON_WPSD_MASK;
            dma_control |= PDMA_CON_WPEN_MASK;
            break;
        default:
            return  PDMA_ERROR;

    }

    switch (pdma_config->size) {

        case PDMA_BYTE:
            dma_control |= PDMA_CON_SIZE_BYTE_MASK;
            break;

        case PDMA_HALF_WORD:
            dma_control |= PDMA_CON_SIZE_HALF_WORD_MASK;
            break;
        case PDMA_WORD:
            dma_control |= PDMA_CON_SIZE_WORD_MASK;
            break;
        default:
            return  PDMA_ERROR;
    }

    if ((pdma_config->burst_mode == true) && (pdma_config->size == PDMA_BYTE)) {
        dma_control |= PDMA_CON_B2W_MASK;
    }

    dma_control |= PDMA_CON_DREQ_MASK;
    dma_control |= channel << DMA_MASTER_OFFSET;
  
   pdma[offset]->PDMA_CON= dma_control;
   pdma[offset]->PDMA_COUNT = pdma_config->count;

    return  PDMA_OK;

}

pdma_status_t pdma_start_polling(pdma_channel_t channel, uint32_t address)

{
    /* define	peripheral dma base adddress array offset */
    uint32_t offset = 0;


    pdma_running_status_t  running_status = PDMA_IDLE;

    offset = pdma_check_valid_channel(channel);
    if (0xff == offset) {
        return PDMA_ERROR_CHANNEL;
    }




    pdma[offset]->PDMA_PGMADDR = address;
	
     pdma[offset]->PDMA_CON &= ~PDMA_CON_ITEN_MASK;
	
    pdma[offset]->PDMA_START =  PDMA_START_BIT_MASK;

    pdma_get_running_status(channel, &running_status);
    while (running_status) {

        pdma_get_running_status(channel, &running_status);
    };

    pdma_stop(channel);
    return  PDMA_OK;

}

/*pdma_start_without_polling is  dedicate for I2C user*/
pdma_status_t pdma_start_without_polling(pdma_channel_t channel, uint32_t address)

{
    /* define	peripheral dma base adddress array offset */
    uint32_t offset = 0;

    offset = pdma_check_valid_channel(channel);
    if (0xff == offset) {
        return PDMA_ERROR_CHANNEL;
    }

    pdma[offset]->PDMA_PGMADDR = address;
	
     pdma[offset]->PDMA_CON &= ~PDMA_CON_ITEN_MASK;
	
    pdma[offset]->PDMA_START =  PDMA_START_BIT_MASK;

    return  PDMA_OK;

}

pdma_status_t pdma_set_limiter(pdma_channel_t channel, uint8_t limiter)

{
    uint32_t offset = 0;

    offset = pdma_check_valid_channel(channel);
    if (0xff == offset) {
        return PDMA_ERROR_CHANNEL;
    }

    /* check limiter valid or not */
    if (limiter > 0xFF) {
        return PDMA_INVALID_PARAMETER;
    }

    /* set limiter value in wrap point address register */
    pdma[offset]->PDMA_LIMITER= limiter;


    return PDMA_OK;

}

pdma_status_t pdma_start_interrupt(pdma_channel_t channel, uint32_t address)
{
    /* define  peripheral dma base adddress array offset */
    uint32_t offset = 0;

    offset = pdma_check_valid_channel(channel);
    if (0xff == offset) {
        return PDMA_ERROR_CHANNEL;
    }



    pdma[offset]->PDMA_PGMADDR = address;
    pdma[offset]->PDMA_CON |= PDMA_CON_ITEN_MASK;
    pdma[offset]->PDMA_START =  PDMA_START_BIT_MASK;

    return  PDMA_OK;

}

pdma_status_t pdma_stop(pdma_channel_t channel)

{
    /* define  peripheral dma base adddress array offset */
    uint32_t offset = 0;

    offset = pdma_check_valid_channel(channel);
    if (0xff == offset) {
        return PDMA_ERROR_CHANNEL;
    };


    pdma[offset]->PDMA_START &= ~PDMA_START_BIT_MASK;
    pdma[offset]->PDMA_ACKINT = PDMA_ACKINT_BIT_MASK;

    return  PDMA_OK;
}

pdma_status_t pdma_register_callback(pdma_channel_t channel, pdma_callback_t callback, void *user_data)

{
    /* define  peripheral dma base adddress array offset */
    uint32_t offset = 0;

    offset = pdma_check_valid_channel(channel);
    if (0xff == offset) {
        return PDMA_ERROR_CHANNEL;
    }
    if (g_pdma_callback[offset].func == NULL) {
        g_pdma_callback[offset].func = callback;
        g_pdma_callback[offset].argument  = user_data;
    }

    return PDMA_OK;
}
pdma_status_t pdma_get_running_status(pdma_channel_t channel, pdma_running_status_t *running_status)

{
    /* define  peripheral dma base adddress array offset */
    uint32_t offset = 0;

    /*define peripheral dma global status tmp variable*/
    volatile uint32_t  global_status = 0;

    offset = pdma_check_valid_channel(channel);
    if (0xff == offset) {
        return PDMA_ERROR_CHANNEL;
    }

    /*  read pdma running  status	*/
    global_status = pdma_global[offset]->PDMA_GLBSTA;

    if (global_status & (0x1 << g_pdma_global_running_bit[offset])) {

        *running_status = PDMA_BUSY;
    } else {
        *running_status = PDMA_IDLE;
    }

    return PDMA_OK;

}



/*************************************************************************************************************/
/************************************peripheral  dma function end line**********************************************/
/*************************************************************************************************************/



/*************************************************************************************************************/
/************************************virtual fifo dma function start line*********************************************/
/*************************************************************************************************************/

static uint32_t vdma_check_valid_channel(vdma_channel_t channel)
{
    /*define  peripheral dma base adddress array offset*/
    uint32_t offset = 0;

    /*calucate  current channel 's  offset in peripheral dma base adddress array */
    offset = channel - VDMA_START_CHANNEL;

    if (offset > (VDMA_NUMBER - 1)) {
        return INVALID_STATUS;
    } else {
        return offset;
    }
}


vdma_status_t vdma_init(vdma_channel_t channel)
{
    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;

    /* define  virtual fifo dma control tmp variable */
    uint32_t dma_control = 0;

    offset = vdma_check_valid_channel(channel);
    if (0xff == offset) {
        return VDMA_ERROR_CHANNEL;
    }

    /*check vdma init*/
    VDMA_CHECK_AND_SET_INIT(channel);

    /*default setting */
    vdma[offset]->VDMA_CON= 0x0;
    vdma[offset]->VDMA_COUNT = 0x0;
    vdma[offset]->VDMA_START = 0x0;
    vdma[offset]->VDMA_INTSTA = 0x0;
    vdma[offset]->VDMA_PGMADDR = 0x0;
    vdma[offset]->VDMA_ALTLEN = 0x0;
    vdma[offset]->VDMA_FFSIZE = 0x0;

      /*un-gating vfifo dma clock*/
	  dma_enable_clock(channel);

    switch (channel) {

        case  VDMA_I2STX:
			dma_control = VDMA_CON_MASTER_I2S_TX_MASK  | VDMA_CON_BURST_16BEAT | VDMA_CON_DREQ_MASK | PDMA_CON_SIZE_WORD_MASK;
            vdma[0]->VDMA_CON = dma_control;
            break;

        case  VDMA_I2SRX:
			dma_control = VDMA_CON_MASTER_I2S_RX_MASK | VDMA_CON_DIR_MASK | VDMA_CON_BURST_16BEAT | VDMA_CON_DREQ_MASK | PDMA_CON_SIZE_WORD_MASK;
			vdma[1]->VDMA_CON = dma_control;
            break;

        case  VDMA_UART0TX:
	        dma_control = VDMA_CON_MASTER_UART0TX_MASK  | VDMA_CON_DREQ_MASK | PDMA_CON_SIZE_BYTE_MASK;
			vdma[2]->VDMA_CON = dma_control;
            break;

        case  VDMA_UART0RX:
			dma_control = VDMA_CON_MASTER_UART0RX_MASK | VDMA_CON_DIR_MASK  | VDMA_CON_DREQ_MASK | PDMA_CON_SIZE_BYTE_MASK;
            vdma[3]->VDMA_CON = dma_control;
            break;

		case  VDMA_UART1TX:
	        dma_control = VDMA_CON_MASTER_UART1TX_MASK  | VDMA_CON_DREQ_MASK | PDMA_CON_SIZE_BYTE_MASK;
			vdma[4]->VDMA_CON = dma_control;
            break;
		
        case  VDMA_UART1RX:
			      dma_control = VDMA_CON_MASTER_UART1RX_MASK | VDMA_CON_DIR_MASK  | VDMA_CON_DREQ_MASK | PDMA_CON_SIZE_BYTE_MASK;
            vdma[5]->VDMA_CON = dma_control;
            break;

        default:
            return  VDMA_ERROR;

    }


    hal_nvic_register_isr_handler(CM4_DMA_IRQ, dma_interrupt_hander);
	NVIC_SetPriority((IRQn_Type)CM4_DMA_IRQ, CM4_DMA_PRI);
    NVIC_EnableIRQ(CM4_DMA_IRQ);


    return VDMA_OK;

}

vdma_status_t vdma_deinit(vdma_channel_t channel)
{

    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = INVALID_STATUS;
    uint32_t saved_mask;

    /*define virtual fifo dma global status tmp variable*/
    volatile uint32_t  global_status = 0;

    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }

    saved_mask = save_and_set_interrupt_mask();

    global_status = vdma_global[offset]->VDMA_GLBSTA;

    if (global_status & (0x1 << (g_vdma_global_running_bit[offset] + 1))) {
        assert(0);
        restore_interrupt_mask(saved_mask);
        return VDMA_ERROR;

    }
    if (global_status & (0x1 << g_vdma_global_running_bit[offset])) {
        assert(0);
        restore_interrupt_mask(saved_mask);
        return VDMA_ERROR;

    }
    restore_interrupt_mask(saved_mask);

    /*set vdma idle*/
    VDMA_SET_IDLE(channel);

    ///TODO:  gating vfifo dma clock*/
	dma_disable_clock(channel);


    return VDMA_OK;
}

vdma_status_t vdma_configure(vdma_channel_t channel, vdma_config_t *vdma_config)
{

    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;


    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }

    /* the address for DMA buffer must be 4 bytes aligned */
    if ((vdma_config->base_address % 4) > 0) {
        assert(0);
        return VDMA_INVALID_PARAMETER;
    }

    vdma[offset]->VDMA_FFSIZE = vdma_config->size;
    vdma[offset]->VDMA_PGMADDR = vdma_config->base_address;

    return VDMA_OK;
}

vdma_status_t vdma_enable_interrupt(vdma_channel_t channel)
{
    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;
	
    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }
	 
	vdma[offset]->VDMA_CON |= VDMA_CON_ITEN_MASK | VDMA_CON_TOEN_MASK ;
    return VDMA_OK;

}


vdma_status_t vdma_start(vdma_channel_t channel)
{
    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;
    uint32_t saved_mask;
    /*define virtual fifol dma global status tmp variable*/
    volatile uint32_t  global_status = 0;

    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }

    saved_mask = save_and_set_interrupt_mask();
    /* check whether vdma is in running mode  */
    global_status = vdma_global[offset]->VDMA_GLBSTA;

    if (global_status & (0x1 << g_vdma_global_running_bit[offset])) {

        /*vdma is running now,assert here may be better*/
        restore_interrupt_mask(saved_mask);
        return VDMA_ERROR;

    }

    /*check wether vdma 's interrrupt is triggered*/

    if (global_status & (0x1 << (g_vdma_global_running_bit[offset] + 1))) {

        /*vdma's interrupt is triggered ,assert here may be better*/
        restore_interrupt_mask(saved_mask);
        return VDMA_ERROR;

    }
    restore_interrupt_mask(saved_mask);

    vdma[offset]->VDMA_START = VDMA_START_BIT_MASK;
    return VDMA_OK;

}


vdma_status_t vdma_disable_interrupt(vdma_channel_t channel)
{
    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;

    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }

	vdma[offset]->VDMA_CON &= ~VDMA_CON_ITEN_MASK;
	vdma[offset]->VDMA_CON &= ~VDMA_CON_TOEN_MASK;

    return VDMA_OK;
}


vdma_status_t vdma_stop(vdma_channel_t channel)
{
    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;

    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }

    vdma[offset]->VDMA_START = VDMA_STOP_BIT_MASK;
    return VDMA_OK;
}

vdma_status_t vdma_set_threshold(vdma_channel_t channel, uint32_t threshold)

{
    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;

    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }

    if (threshold > VDMA_MAX_THRESHOLD) {
        return  VDMA_INVALID_PARAMETER;
    }
    vdma[offset]->VDMA_COUNT = threshold;

    return VDMA_OK;

}

vdma_status_t vdma_set_timeout(vdma_channel_t channel, uint32_t timeout)
{

/*define  virtual fifo dma base adddress array offset*/
uint32_t offset = 0;

offset = vdma_check_valid_channel(channel);
if (INVALID_STATUS == offset) {
   return VDMA_ERROR_CHANNEL;
}


vdma[offset]->VDMA_TO= timeout;

return VDMA_OK;

}


vdma_status_t vdma_set_alert_length(vdma_channel_t channel, uint32_t alert_length)

{
    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;

    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }

    if (alert_length > VDMA_MAX_ALERT_LENGTH) {
        return  VDMA_INVALID_PARAMETER;
    }
    vdma[offset]->VDMA_ALTLEN = alert_length;

    return VDMA_OK;

}

vdma_status_t vdma_push_data(vdma_channel_t channel, uint8_t data)

{

    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;

    offset = channel - VDMA_START_CHANNEL;

    /*put data into virtual fifo dma */

    vdma_port[offset]->VDMA_PORT = data;

    return VDMA_OK;
}

vdma_status_t vdma_push_data_4bytes(vdma_channel_t channel, uint32_t data)

{

    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;
    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }


    /*put data into virtual fifo dma */

    vdma_port[offset]->VDMA_PORT = data;

    return VDMA_OK;
}

vdma_status_t vdma_pop_data(vdma_channel_t channel, uint8_t *data)

{

    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;

	offset = channel - VDMA_START_CHANNEL;

    /*get data from  virtual fifo dma */

    *data = vdma_port[offset]->VDMA_PORT ;

    return VDMA_OK;
}

vdma_status_t vdma_pop_data_4bytes(vdma_channel_t channel, uint32_t *data)

{

    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;

    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }


    /*get data from  virtual fifo dma */

    *data = vdma_port[offset]->VDMA_PORT ;

    return VDMA_OK;
}


vdma_status_t vdma_get_available_receive_bytes(vdma_channel_t channel, uint32_t *receive_bytes)

{
    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;

    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }

    *receive_bytes = vdma[offset]->VDMA_FFCNT;

    return   VDMA_OK;

}

vdma_status_t vdma_get_available_send_space(vdma_channel_t channel, uint32_t *available_space)

{

    /*define virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;

    /*define virtual fifo dma total fifo size tmp  variable*/
    uint32_t total_size = 0;

    /*define virtual fifo dma  valid  fifo size tmp  variable*/
    uint32_t valid_size = 0;

    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }


    total_size = vdma[offset]->VDMA_FFSIZE;
    valid_size = vdma[offset]->VDMA_FFCNT;

    *available_space = total_size - valid_size;

    return VDMA_OK;

}


vdma_status_t vdma_register_callback(vdma_channel_t channel, vdma_callback_t callback, void *user_data)

{
    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;

    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }

    if (g_vdma_callback[offset].func == NULL) {
        g_vdma_callback[offset].func = callback;
        g_vdma_callback[offset].argument  = user_data;
    }

    return VDMA_OK;

}

vdma_status_t vdma_register_timeout_callback(vdma_channel_t channel, vdma_callback_t timeout_callback, void *user_data)


{
    /*define  virtual fifo dma base adddress array offset*/
    uint32_t offset = 0;

    offset = vdma_check_valid_channel(channel);
    if (INVALID_STATUS == offset) {
        return VDMA_ERROR_CHANNEL;
    }

    if (g_vdma_tocallback[offset].func == NULL) {
        g_vdma_tocallback[offset].func = timeout_callback;
        g_vdma_tocallback[offset].argument  = user_data;
    }

    return VDMA_OK;

}


void dma_interrupt_hander(hal_nvic_irq_t irq_number)

{
    /*define peripheral dma global status tmp variable*/
    volatile uint32_t  global_status = 0;

    /*define tmp callback variable */
    hal_gdma_callback_t     gdma_callback;

	/*define tmp callback variable */
    pdma_callback_t     pdma_callback;

	/*define tmp callback variable */
    vdma_callback_t     vdma_callback;
    void *argument;
    vdma_callback_t     vdma_tocallback;
    void *to_argument;

    /*define tmp variable for loop count*/
    static uint32_t count = 0;
    /*mask PD DMA interrupt */

    /* step 1: handle gdma in pd domain --> read pdma  status ,this is for checking gdma interrupt status*/
	for(count = 0; count < GDMA_NUMBER; count++) {
    global_status = gdma_global[count]->GDMA_GLBSTA;
    if (global_status & (0x1 << (g_gdma_global_running_bit[count] + 1))) {
        /*clear gdma interrupt*/

        gdma[count]->GDMA_ACKINT = GDMA_ACKINT_BIT_MASK;
	    gdma[count]->GDMA_START &= ~GDMA_START_BIT_MASK;
        /*call pdma user's callback */
        gdma_callback = g_gdma_callback[count].func;
        argument = g_gdma_callback[count].argument;
        if (gdma_callback != NULL) {
            gdma_callback(HAL_GDMA_EVENT_TRANSACTION_SUCCESS, argument);
        } else {
            assert(0);
        }
         gdma_status[count] = 1;
        
    }
    }
	
	/*step 2: handle pdma  */
	
		for (count = 0; count < PDMA_NUMBER; count++) {
			global_status = pdma_global[count]->PDMA_GLBSTA;
	
	
			if (global_status & (0x1 << (g_pdma_global_running_bit[count] + 1))) {
	
				{
					/*clear pdma interrupt*/
					pdma[count]->PDMA_ACKINT = PDMA_ACKINT_BIT_MASK;
	                pdma[count]->PDMA_START &= ~PDMA_START_BIT_MASK;
					/*call pdma user's callback */
					pdma_callback = g_pdma_callback[count].func;
					argument = g_pdma_callback[count].argument;
					if (pdma_callback != NULL) {
						pdma_callback(PDMA_EVENT_TRANSACTION_SUCCESS, argument);
					} else {
						assert(0);
					}
					
				}
			}
		}
	
		/*step 3: handle vdma */
	
		for (count = 0; count < VDMA_NUMBER; count++) {
			global_status = vdma_global[count]->VDMA_GLBSTA;
	
	
			if (global_status & (0x1 << (g_vdma_global_running_bit[count] + 1))) {
	
				{
					/*clear vdma interrupt*/
					vdma[count]->VDMA_ACKINT = VDMA_ACKINT_BIT_MASK;

					/*call vdma user's callback */
					vdma_callback = g_vdma_callback[count].func;
					argument = g_vdma_callback[count].argument;
					if (vdma_callback != NULL) {
						vdma_callback(VDMA_EVENT_TRANSACTION_SUCCESS, argument);
					} else {
						assert(0);
					}
                    if ((count == 3) || (count ==5)) {
                        /*call vdma user's timeoutcallback */
                        vdma[count]->VDMA_ACKINT = VDMA_ACKTO_BIT_MASK;
                        vdma_tocallback = g_vdma_tocallback[count].func;
                        to_argument = g_vdma_tocallback[count].argument;
                        if (vdma_tocallback != NULL) {
                            vdma_tocallback(VDMA_EVENT_TRANSACTION_SUCCESS, to_argument);
                        } else {
                            assert(0);
                        }
                    }
				}
			}
		}

}

#endif /*HAL_GDMA_MODULE_ENABLED*/

