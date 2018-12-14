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

/*
** $Log: hal_ADC.c $
**
** 08 24 2015 pierre.chang
** Add adc sequence released by DE for IoT.
**
** 08 13 2015 pierre.chang
** update i2s and adc sample code
**
** 08 12 2015 pierre.chang
** Update UART, I2S and ADC code.
**
** 08 07 2015 pierre.chang
** Add for ADC and I2S halex.
**
** 06 11 2015 morris.yeh
** [CMM & SCN]
** delete & move cmm_test.c/scn_test.c to dawn_test.c
**
** 06 03 2015 morris.yeh
** [ADC]
** refine API(if on-off period < 200ms, not turn off ALDO)
**
** 06 01 2015 morris.yeh
** [ADC & PWM]
** Add test command for LPDVT
**
** 05 27 2015 morris.yeh
** [ADC]
** HEC verify
** 1. UART print will impact readwrite ptr & IIR info to check if the interrupt source is time out or trigger level
** 2. if use DMA mode, need to disable "RX full and time-out interrupt"->mSetHWEntry(ADC_RXFEN, 0);
**
** 05 14 2015 morris.yeh
** [ADC]
** Add test command for wake up ADC
**
** 04 30 2015 morris.yeh
** [ADC]
** Apply BUCK patch
**
** 04 28 2015 morris.yeh
** [ADC]
** refine code for 8ms test
** [CMM]
** query radio stats by index & size
**
** 04 16 2015 morris.yeh
** [ADC]
** refine code for ADC DVT
**
** 04 09 2015 morris.yeh
** [ADC]
** Add test code for HP ch10 & ch9
**
** 04 09 2015 morris.yeh
** [ADC]
** Add test code for CPU clock change
**
** 04 01 2015 morris.yeh
** [ADC]
** fix build error for IOT build
**
** 04 01 2015 morris.yeh
** [ADC]
** refine code for EVB DVT
**
** 04 01 2015 morris.yeh
** [ADC]
** Turn on GDMA clock for ADC DMA mode work,HAL_REG_32(0x83000018) = 0x03FFFFFF;
**
** 03 31 2015 maker.wang
** [ACCDET]
** Add accdetKeyPressDetection in ADC ISR
**
** 03 30 2015 morris.yeh
** [ADC]
** Refine ADC code for DVT test
**
** 01 29 2015 morris.yeh
** [ADC]
** fix bug of ADC test code for always zero in the last element of ring buffer
**
** 01 21 2015 morris.yeh
** [ADC]
** modify ADC test code for ACCDET use
**
** 01 12 2015 morris.yeh
** [ADC]
** Use 32bytes ADC sample instead of 16bytes
**
** 01 09 2015 morris.yeh
** [ADC]
** Re-Org HAL ADC driver
**
** 01 05 2015 morris.yeh
** [ADC]
** modify ADC test code
**
** 12 29 2014 morris.yeh
** [ADC]
** 1. modify hal ADC driver & ADC test code for ADC DMA mode
** 2. ADC direct mode & ADC DMA mode test pass
**
** 12 24 2014 morris.yeh
** [ADC]
** add DMA ADC test code
**
** 12 24 2014 pierre.chang
** Update ADC DMA mode settings.
**
** 12 23 2014 morris.yeh
** [ADC]
** Add IoT ADC comparator test code
**
** 12 18 2014 morris.yeh
** [ADC]
** modify ADC test code
**
** 12 11 2014 morris.yeh
** [ADC][GPIO]
** 1. Add ADC test code
** 2. Add pull-up/pull-down code for GPIO
**
** 12 10 2014 morris.yeh
** [ADC]
** fix bug of get FIFO length
**
** 11 28 2014 morris.yeh
** [ADC]
** 1. Modify HAL ADC API prototype to meet MTK coding style
** 2. Integrate MSFT Indium ADC API with HAL ADC API
**
** 11 21 2014 pierre.chang
** Modify DMA API.
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "hal_adc.h"

#ifdef HAL_ADC_MODULE_ENABLED

#include <stdlib.h>

#include "type_def.h"
#include "dma_hw.h"
#include "dma_sw.h"
#include "_mtk_hal_debug.h"
#include "hal_nvic_internal.h"
#include "mem_util.h"
//#include "top_cfg.h"
//#include "utils.h"
//#include "cos_api.h"
#include "timer.h"

#include <hal_platform.h>
//#include "hal_adc_internal.h"
#include "hal_efuse.h"

#include "low_hal_adc.h"
#include "hal_nvic.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define ADC_DRIVER_DEBUG//for driver debug

#define ADC_BUSY 1
#define ADC_IDLE 0

//remove adc data offset
#define REMOVE_DATA_OFFSET 1

#define MT7687_ASIC 1
#define CH_BIT_MAP              0x000F  //0xFFFF is all channel, for default setting
#define PMODE_PERIOD            16000    //8000(1M)//6500(812.5K)//16000(2M)   //16000T, T=0.5us(2M clock)

#define IOT_GPIO_AON_ADC_EN                             (IOT_GPIO_PINMUX_AON_BASE + 0x0000000C)
#define PAD_WF_RF_DIS_B_PINMUX_AON_ADC0_EN_MASK         (0x00000040)
#define PAD_BT_RF_DIS_B_PINMUX_AON_ADC1_EN_MASK         (0x00000080)
#define PAD_WF_LED_B_PINMUX_AON_ADC2_EN_MASK            (0x00000100)
#define PAD_BT_LED_B_PINMUX_AON_ADC3_EN_MASK            (0x00000200)

#define GPIO57_PAD_EN_MASK    (1<<25)//57-32
#define GPIO58_PAD_EN_MASK    (1<<26)//58-32
#define GPIO59_PAD_EN_MASK    (1<<27)//59-32
#define GPIO60_PAD_EN_MASK    (1<<28)//59-32



/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/


/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

ADC_INFO_T g_rAdcInfo;
volatile uint8_t g_adc_status = ADC_IDLE;
#if REMOVE_DATA_OFFSET
volatile uint8_t g_adc_efuse_offset;
volatile uint8_t g_adc_efuse_gain;
#endif


extern void delay_time(kal_uint32 count);



#ifdef HAL_ADC_FEATURE_PERIODIC_MODE_SUPPORT

typedef struct {
    hal_adc_vfifo_callback_t  func;
    void                      *parameter;
} adc_vfifo_callback_t;

static adc_vfifo_callback_t s_adc_vfifo_callback = {NULL, NULL};



hal_adc_status_t hal_adc_register_vfifo_callback(hal_adc_vfifo_callback_t adc_callback, void *user_data)
{
    s_adc_vfifo_callback.func = adc_callback;
    s_adc_vfifo_callback.parameter = user_data;

    return HAL_ADC_STATUS_OK;
}

hal_adc_status_t hal_adc_vfifo_mode_enable(hal_adc_config_t *config)
{
    if (config == NULL || config->vfifo_buffer == NULL) {
        return HAL_ADC_STATUS_INVALID_PARAMETER;
    }

    if (config->average_number !=  1 && config->average_number !=  2 &&
            config->average_number !=  4 && config->average_number !=  8 &&
            config->average_number != 16 && config->average_number != 32 &&
            config->average_number != 64) {
        return HAL_ADC_STATUS_INVALID_PARAMETER;
    }

    if (config->vfifo_trigger_level > config->vfifo_buffer_size) {
        return HAL_ADC_STATUS_INVALID_PARAMETER;
    }
    if (config->channel_map & 0xFFF0) {
        return HAL_ADC_STATUS_INVALID_PARAMETER;
    }
    ADC_FSM_PARAM_T rAdcFsmParam = {0};

    rAdcFsmParam.ePmode = ADC_PMODE_PERIODIC;
    switch (config->average_number) {
        case 1:
            rAdcFsmParam.eAvg_Mode = ADC_AVG_1_SAMPLE;
            break;
        case 2:
            rAdcFsmParam.eAvg_Mode = ADC_AVG_2_SAMPLE;
            break;
        case 4:
            rAdcFsmParam.eAvg_Mode = ADC_AVG_4_SAMPLE;
            break;
        case 8:
            rAdcFsmParam.eAvg_Mode = ADC_AVG_8_SAMPLE;
            break;
        case 16:
            rAdcFsmParam.eAvg_Mode = ADC_AVG_16_SAMPLE;
            break;
        case 32:
            rAdcFsmParam.eAvg_Mode = ADC_AVG_32_SAMPLE;
            break;
        case 64:
            rAdcFsmParam.eAvg_Mode = ADC_AVG_64_SAMPLE;
            break;
        default:
            rAdcFsmParam.eAvg_Mode = ADC_AVG_32_SAMPLE;
            break;
    }

    rAdcFsmParam.u2Channel_Map = config->channel_map;
    rAdcFsmParam.u4Period = config->period_time;
    rAdcFsmParam.eFifo_Mode = ADC_FIFO_DMA;
    rAdcFsmParam.pru4DmaVfifoAddr = (UINT32 *)config->vfifo_buffer;
    rAdcFsmParam.u4DmaVfifoLen = config->vfifo_buffer_size;
    rAdcFsmParam.u4DmaVfifoTriggerLevel = config->vfifo_trigger_level;

    halADC_Fsm_Param_Set(&rAdcFsmParam);
    mSetHWEntry(ADC_FSM_EN, 1);

    return HAL_ADC_STATUS_OK;
}

//return the actual size of data received.
uint32_t hal_adc_vfifo_get_data(uint32_t length, uint32_t *data)
{
    uint32_t actual_size, receive_size;

    halADC_Fifo_Get_Length(&actual_size);
    //printf("\r\n actual_size = %d", actual_size);
    actual_size >>= 2;
    if (actual_size > length) {
        receive_size = length;
    } else {
        receive_size = actual_size;
    }
    actual_size -= receive_size;
    halADC_Fifo_Read_Data(receive_size, (UINT32 *)data);
#if REMOVE_DATA_OFFSET
    halADC_Remove_Data_Offset(data, data, length);
#endif
    actual_size <<= 2;
    if (actual_size < g_rAdcInfo.rFsmParam.u4DmaVfifoTriggerLevel) {
        DMA_Vfifo_enable_interrupt(VDMA_ADC_RX_CH);
    }

    return receive_size;
}

#endif//HAL_ADC_FEATURE_PERIODIC_MODE_SUPPORT


#ifdef HAL_ADC_FEATURE_THRESHOLD_MODE_SUPPORT

typedef struct {
    hal_adc_threshold_callback_t  func;
    void                          *parameter;
} adc_threshold_callback_t;

static adc_threshold_callback_t   s_adc_threshold_callback[ HAL_ADC_CHANNEL_MAX ] = {
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL}
};

hal_adc_status_t hal_adc_register_threshold_callback(hal_adc_channel_t channel, hal_adc_threshold_callback_t adc_callback, void *user_data)
{
    if (channel >= HAL_ADC_CHANNEL_MAX) {
        return HAL_ADC_STATUS_ERROR_CHANNEL;
    }

    s_adc_threshold_callback[ channel ].func   = adc_callback;
    s_adc_threshold_callback[ channel ].parameter = user_data;

    hal_nvic_register_isr_handler(CM4_ADC_COMP_IRQ, halADC_COMP_LISR);
    NVIC_SetPriority(CM4_ADC_COMP_IRQ, CM4_ADC_COMP_PRI);
    NVIC_EnableIRQ(CM4_ADC_COMP_IRQ);

    return HAL_ADC_STATUS_OK;
}

hal_adc_status_t hal_adc_threshold_enable(hal_adc_channel_t channel, uint16_t threshold)
{
#ifdef ADC_DRIVER_DEBUG
    uint32_t times = 0;
#endif
    if (channel >= HAL_ADC_CHANNEL_MAX) {
        return HAL_ADC_STATUS_ERROR_CHANNEL;
    }

    /* callback must be set to enable trigeer. */
    if (s_adc_threshold_callback[ channel ].func == NULL) {
        return HAL_ADC_STATUS_ERROR;
    }

    mSetHWEntry(ADC_REG_COMP_IRQ_EN, 1 << channel); //enable comparator interrupt
    mSetHWEntry(ADC_REG_COMP_THRESHOLD, threshold);  //comparator threshold
    //should oprations below  update global structure??
    mSetHWEntry(ADC_REG_CH_MAP, 1 << channel);

    mSetHWEntry(ADC_PMODE_EN, ADC_PMODE_PERIODIC);//always use periodic mode???
    mSetHWEntry(ADC_REG_PERIOD, PMODE_PERIOD);
    mSetHWEntry(ADC_REG_AVG_MODE, ADC_AVG_32_SAMPLE);

    while (mGetHWEntry(ADC_RO_ADC_COMP_FLAG)) { //DE Sophia:Should read more than one times to clear the flag because of different clock domain reason.
#ifdef ADC_DRIVER_DEBUG
        printf("\r\ntimes = %d", times++);
#endif
    }
    mSetHWEntry(ADC_FSM_EN, 1);

    return HAL_ADC_STATUS_OK;
}

#endif//HAL_ADC_THRESHOLD_MODE_SUPPORT



#if 0

hal_adc_status_t hal_adc_on(void)
{
    mSetHWEntry(ADC_ALDO_EN, 1);  //Set 0x81021438[1]=1, then wait 200us for 2.5V output ready    //EVB J49(not JP49) for ALDO 2.5V
    return HAL_ADC_STATUS_OK;
}

hal_adc_status_t hal_adc_off(void)
{
    halADC_Fsm_Disable(TRUE);
//    mSetHWEntry(ADC_ALDO_EN, 0);  //Set 0x81021438[1]=1, then wait 200us for 2.5V output ready    //EVB J49(not JP49) for ALDO 2.5V
    return HAL_ADC_STATUS_OK;
}

#endif

#if REMOVE_DATA_OFFSET

void halADC_Get_ADC_Efuse(void)
{
    uint16_t efuse_offset = 0x5E;
    uint16_t efuse_gain = 0x5F;

    uint8_t efuse_data_offset, efuse_data_gain;

    //Get efuse data for adc offset
    if (HAL_EFUSE_OK == hal_efuse_read(efuse_offset, &efuse_data_offset, 1)) {
        g_adc_efuse_offset = efuse_data_offset;
    } else {
        g_adc_efuse_offset = 0;
    }

    //Get efuse data for adc gain
    if (HAL_EFUSE_OK == hal_efuse_read(efuse_gain, &efuse_data_gain, 1)) {
        g_adc_efuse_gain = efuse_data_gain;
    } else {
        g_adc_efuse_gain = 0;
    }

    //printf("\r\n g_adc_efuse_offset = 0x%02x", g_adc_efuse_offset);
    //printf("\r\n g_adc_efuse_gain = 0x%02x", g_adc_efuse_gain);
}


void halADC_Remove_Data_Offset(uint32_t *input, uint32_t *output, uint32_t length)
{
    uint8_t efuse_data_offset, efuse_data_gain;
    uint8_t flag_offset, flag_gain;
    uint32_t data_len;
    uint16_t adc_data, adc_data_origin;

    efuse_data_offset = g_adc_efuse_offset;
    efuse_data_gain = g_adc_efuse_gain;

    //printf("\r\n efuse_data_offset = 0x%02x", efuse_data_offset);
    //printf("\r\n efuse_data_gain = 0x%02x", efuse_data_gain);
    flag_offset = (efuse_data_offset >> 7) & 1;
    flag_gain = (efuse_data_gain >> 7) & 1;
    efuse_data_offset &= ~(1 << 7);
    efuse_data_gain &= ~(1 << 7);

    for (data_len = 0; data_len < length; data_len ++) {
//		printf("\r\n 0000_adc_data = 0x%08x", input[data_len]);
        adc_data = (uint16_t)(input[data_len] & 0x0000FFF0) >> 4;
        adc_data_origin = adc_data;
//		printf("\r\n 00_adc_data = 0x%04x", adc_data);

        /* Remove ADC offset */
        if (0 == flag_offset) {
            adc_data -= efuse_data_offset;
            if (adc_data > adc_data_origin) { //overflow
                adc_data = 0;
            }
        } else {
            adc_data += efuse_data_offset;
            if (adc_data > 0x0FFF) { //overflow
                adc_data = 0x0FFF;
            }
        }

        adc_data_origin = adc_data;
        /* Remove ADC gain */
        if (0 == flag_gain) {
            adc_data += (adc_data * efuse_data_gain) / 2500;
            if (adc_data > 0x0FFF) { //overflow
                adc_data = 0x0FFF;
            }
        } else {
            adc_data -= (adc_data * efuse_data_gain) / 2500;
            //over flow will never happens
            if (adc_data > adc_data_origin) { //overflow
                adc_data = 0;
            }
        }

//		printf("\r\n 01_adc_data = 0x%04x", adc_data);
        output[data_len] = (input[data_len] & ~0x0000FFF0) | (adc_data << 4);
    }
}

#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_sleep_manager.h"
#include "hal_sleep_driver.h"

uint8_t adc_sleep_handle;
const static char *adc_lock_sleep_name = "AUXADC";

#endif

hal_adc_status_t hal_adc_init(void)
{
    ADC_FSM_PARAM_T rAdcFsmParam = {ADC_PMODE_ONE_TIME};
    uint32_t irq_flag;

    irq_flag = save_and_set_interrupt_mask();
    if (g_adc_status == ADC_BUSY) {
        restore_interrupt_mask(irq_flag);
        return HAL_ADC_STATUS_ERROR_BUSY;
    } else {
        g_adc_status = ADC_BUSY;
        restore_interrupt_mask(irq_flag);
    }
#ifdef HAL_SLEEP_MANAGER_ENABLED
    adc_sleep_handle = hal_sleep_manager_set_sleep_handle(adc_lock_sleep_name);
    hal_sleep_manager_lock_sleep(adc_sleep_handle);
#endif

    mSetHWEntry(ADC_ALDO_EN, 1);  //Set 0x81021438[1]=1, then wait 200us for 2.5V output ready    //EVB J49(not JP49) for ALDO 2.5V
    // delay_us(200);
    delay_time(6);

    // PAD control by CM4
    //DRV_Reg32(IOT_GPIO_PAD_CTRL1) = 0xFFFFFFFF;//GPIO0~GPIO31
    //DRV_Reg32(IOT_GPIO_PAD_CTRL2) = 0xFFFFFFFF;//GPIO32~GPIO63
    //DRV_Reg32(IOT_GPIO_PAD_CTRL3) = 0x0300FFFF;//GPIO64~

    //GPIO57/GPIO58/GPIO59/GPIO60 corresponding to ADC0/ADC1/ADC2/ADC3
    //Each bit corresponding to one GPIO, when the bit is set to 1, then CM4 controls the corresponding PAD.
    DRV_Reg32(IOT_GPIO_PAD_CTRL2) |= (GPIO57_PAD_EN_MASK |
                                      GPIO58_PAD_EN_MASK |
                                      GPIO59_PAD_EN_MASK |
                                      GPIO60_PAD_EN_MASK);

    // 0x8102140C [22] = 1'b1 and [14] = 1'b1
    // DRV_Reg32(TOP_GFG_AON_RG_PMU_03) |= 0x00404000;
    // cos_control_force_pwm_mode(COS_MODULE_ON_OFF_STATE_CM4_ADC);

    // Raise flag to claim using the PMU
    mSetHWEntry(TOP_CFG_AON_N9_CM4_MESSAGE_CM4_ADC, 1);
    // delay to prevent from racing
    delay_time(1);
    // Force PWM mode
    // Set 0x8102140C bit[22] and bit[14] = 11
    mSetHWEntry(TOP_CFG_AON_PMU_RG_BUCK_FORCE_PWM, 0x101);

    //0x81021410 [26:25] = 2'b11
    DRV_Reg32(TOP_GFG_AON_RG_PMU_04) |= 0x06000000;

    // 0x83008190[29:28] = 2'b10
    mSetHWEntry(ADC_REG_AUXADC_ADC_CLK_CTRL_MUX, 2);

    // 0x83008190[31] = 1'b1
    mSetHWEntry(ADC_REG_AUXADC_ADC_CLK_CTRL_INV_EN, 1);

    //0x83008190[26:25:24] = 000
    DRV_Reg32(TOP_CFG_ADC_AON_BASE + 0x0010) &= 0xF8FFFFFF;

    // 0x8300818C[17:16] = 2'b11
    //mSetHWEntry(ADC_REG_AUXADC_INVERT_PMU_CLK, 1);
    //mSetHWEntry(ADC_REG_AUXADC_ADC_CLK_SRC, 1);

    // 0x8300818C = 32'h70F3AA15 (for IoT)
    DRV_Reg32(TOP_CFG_ADC_AON_BASE + 0x000C) = 0x70F3AA15;

    // delay 100 us before enable ADC
    // delay_us(100);
    delay_time(3);
    rAdcFsmParam.ePmode = ADC_PMODE_ONE_TIME;
    rAdcFsmParam.eAvg_Mode = ADC_AVG_1_SAMPLE;
    rAdcFsmParam.u2Channel_Map = CH_BIT_MAP;
    rAdcFsmParam.u4Period = PMODE_PERIOD;
    rAdcFsmParam.eFifo_Mode = ADC_FIFO_DIRECT;
    rAdcFsmParam.pru4DmaVfifoAddr = (UINT32 *)NULL;
    rAdcFsmParam.u4DmaVfifoLen = 0;
    rAdcFsmParam.u4DmaVfifoTriggerLevel = 0;

    halADC_Fsm_Param_Set(&rAdcFsmParam);

#if REMOVE_DATA_OFFSET
    //DE jason request: get adc offset
    halADC_Get_ADC_Efuse();
#endif

    if (halADC_Fsm_Enable() != HAL_RET_SUCCESS) {
        return HAL_ADC_STATUS_ERROR;
    } else {
        return HAL_ADC_STATUS_OK;
    }
}

hal_adc_status_t hal_adc_deinit(void)
{
    /* Disable ADC IRQ */
    //NVIC_DisableIRQ(CM4_ADC_IRQ);
#ifdef HAL_ADC_FEATURE_THRESHOLD_MODE_SUPPORT
    /* Disable ADC Comparator IRQ */
    NVIC_DisableIRQ(CM4_ADC_COMP_IRQ);
    memset(s_adc_threshold_callback, 0, sizeof(s_adc_threshold_callback));
#endif//HAL_ADC_FEATURE_THRESHOLD_MODE_SUPPORT	

#ifdef HAL_ADC_FEATURE_PERIODIC_MODE_SUPPORT
    memset(s_adc_vfifo_callback, 0, sizeof(s_adc_vfifo_callback));
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_unlock_sleep(adc_sleep_handle);
    hal_sleep_manager_release_sleep_handle(adc_sleep_handle);
#endif

    if (halADC_Fsm_Disable(TRUE) != HAL_RET_SUCCESS) {
        g_adc_status = ADC_IDLE;
        return HAL_ADC_STATUS_ERROR;
    } else {
        g_adc_status = ADC_IDLE;
        return HAL_ADC_STATUS_OK;
    }
}


hal_adc_status_t hal_adc_get_data_polling(hal_adc_channel_t channel, uint32_t *data)
{
    uint32_t data_array[1], count;
    uint32_t actual_data;

    if (channel >= HAL_ADC_CHANNEL_MAX) {
        return HAL_ADC_STATUS_ERROR_CHANNEL;
    }
    if (data == NULL) {
        return HAL_ADC_STATUS_INVALID_PARAMETER;
    }

    switch (channel) {
        case HAL_ADC_CHANNEL_0:
            mSetHWEntry(ADC_REG_CH_MAP, 0x1);
            DRV_Reg32(IOT_GPIO_AON_ADC_EN) |= PAD_WF_RF_DIS_B_PINMUX_AON_ADC0_EN_MASK;
            break;
        case HAL_ADC_CHANNEL_1:
            mSetHWEntry(ADC_REG_CH_MAP, 0x2);
            DRV_Reg32(IOT_GPIO_AON_ADC_EN) |= PAD_BT_RF_DIS_B_PINMUX_AON_ADC1_EN_MASK;
            break;
        case HAL_ADC_CHANNEL_2:
            mSetHWEntry(ADC_REG_CH_MAP, 0x4);
            DRV_Reg32(IOT_GPIO_AON_ADC_EN) |= PAD_WF_LED_B_PINMUX_AON_ADC2_EN_MASK;
            break;
        case HAL_ADC_CHANNEL_3:
            mSetHWEntry(ADC_REG_CH_MAP, 0x8);
            DRV_Reg32(IOT_GPIO_AON_ADC_EN) |= PAD_BT_LED_B_PINMUX_AON_ADC3_EN_MASK;
            break;
        default:
            return HAL_ADC_STATUS_ERROR;
    }

    mSetHWEntry(ADC_FSM_EN, 1);

    do {
        halADC_Fifo_Get_Length((UINT32 *)&count);
    } while (count == 0);

    halADC_Fifo_Read_Data(1, (UINT32 *)data_array);

#if REMOVE_DATA_OFFSET
    //printf("\r\n data_array[0] = 0x%08x", data_array[0]);
    halADC_Remove_Data_Offset(data_array, &actual_data, 1);
    //printf("\r\n actual_data = 0x%08x", actual_data);
    *data = (uint16_t)(actual_data & 0x0000FFF0) >> 4;
#else
    *data = (uint16_t)(data_array[0] & 0x0000FFF0) >> 4;
#endif

    mSetHWEntry(ADC_FSM_EN, 0);

    switch (channel) {
        case HAL_ADC_CHANNEL_0:
            DRV_Reg32(IOT_GPIO_AON_ADC_EN) &= ~PAD_WF_RF_DIS_B_PINMUX_AON_ADC0_EN_MASK;
            break;
        case HAL_ADC_CHANNEL_1:
            DRV_Reg32(IOT_GPIO_AON_ADC_EN) &= ~PAD_BT_RF_DIS_B_PINMUX_AON_ADC1_EN_MASK;
            break;
        case HAL_ADC_CHANNEL_2:
            DRV_Reg32(IOT_GPIO_AON_ADC_EN) &= ~PAD_WF_LED_B_PINMUX_AON_ADC2_EN_MASK;
            break;
        case HAL_ADC_CHANNEL_3:
            DRV_Reg32(IOT_GPIO_AON_ADC_EN) &= ~PAD_BT_LED_B_PINMUX_AON_ADC3_EN_MASK;
            break;
        default:
            break;
    }

    return HAL_ADC_STATUS_OK;
}


//ADC_INFO_T g_rAdcInfo;
//UINT32 u48msCnt = 0;

//UINT16 g_aru2AdcDmaBuf[ADC_DMA_BUF_WORD_SIZE] = {0};

#if 0
// XTalFreq , currently are used by uart1 boudrate setting
UINT32	g_XTALFreq;
//
//  Using XTAL as MCU clock
//
VOID
cmnCpuClkConfigureToXtal_(
    VOID
)
{
    TOP_CFG_AON_BondingAndStrap bs;
    volatile UINT32 reg;
    volatile UINT32 *pTopCfgCM4CKG = (volatile UINT32 *)TOP_CFG_CM4_CKG_EN0;
    volatile UINT32 *pBS = (volatile UINT32 *)TOP_CFG_CM4_CM4_STRAP_STA;

    // Step1. CM4_HCLK_SW set to XTAL
    reg = cmnReadRegister32(pTopCfgCM4CKG);
    reg = reg & ~(CM4_HCLK_SEL_MASK << CM4_HCLK_SEL_SHIFT);
    reg = reg | (CM4_HCLK_SEL_OSC << CM4_HCLK_SEL_SHIFT);
    cmnWriteRegister32(pTopCfgCM4CKG, reg);

    // Step2. CM4_RF_CLK_SW set to XTAL
    reg = cmnReadRegister32(pTopCfgCM4CKG);
    reg = reg & ~(CM4_WBTAC_MCU_CK_SEL_MASK << CM4_WBTAC_MCU_CK_SEL_SHIFT);
    reg = reg | (CM4_WBTAC_MCU_CK_SEL_XTAL << CM4_WBTAC_MCU_CK_SEL_SHIFT);
    cmnWriteRegister32(pTopCfgCM4CKG, reg);

    while (reg != cmnReadRegister32(pTopCfgCM4CKG));


    // Get frequency of Xtal/hclk
#if defined(MT7687_ASIC)
    bs.AsULONG = cmnReadRegister32(pBS);

    switch (bs.XTAL_FREQ) {
        case 0:
            g_XTALFreq = 20000000;	/* 20Mhz */
            break;
        case 1:
            g_XTALFreq = 40000000;	/* 40Mhz */
            break;
        case 2:
            g_XTALFreq = 26000000;	/* 26Mhz */
            break;
        case 3:
            g_XTALFreq = 52000000;	/* 52Mhz */
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            g_XTALFreq = 40000000;	/* fall through */
            break;
    }
#elif defined(MT7687_FPGA)
    /* MT7687 FPGA is fixed to 30Mhz */
    g_XTALFreq = 30000000;
    bs = bs;
    pBS = pBS;
#else
#error "please define here"
#endif
    return;
}

void *hw_memcpy32(void *dest, const void *src, size_t size)
{
    u_int32 *dest_cpy = (u_int32 *)dest;
    u_int32 *src_cpy = (u_int32 *)src;

    size = ((size + 3) >> 2);

    while (size--) {
        *dest_cpy++ = *src_cpy;
        src_cpy++;
    }

    return dest;
}

#endif

//UINT32  g_8SecCount = 0;

void halADC_VDMA_Callback(void)
{
#ifdef HAL_ADC_FEATURE_PERIODIC_MODE_SUPPORT
    //UINT32  i = 0;
    //UINT32* DmaPtr1 = (UINT32*)0x83011900;
    //UINT32* DmaPtr2 = (UINT32*)0x8301194c;
    //UINT32* PrintDmaPtr = NULL;
    //UINT32* pru4FifoReadPtr = (UINT32*)0x830D00D4;
    uint32_t data_len;

    //printf("\r\n-Enter halADC_VDMA_Callback-\n");
    //user callback
    s_adc_vfifo_callback.func(HAL_ADC_EVENT_OK, s_adc_vfifo_callback.parameter);

    halADC_Fifo_Get_Length(&data_len);
    if (data_len > g_rAdcInfo.rFsmParam.u4DmaVfifoTriggerLevel) {
        DMA_Vfifo_disable_interrupt(VDMA_ADC_RX_CH);
#ifdef ADC_DRIVER_DEBUG
        printf("\r\n-----data_len----- = %d", data_len);
        fflush(stdout);
#endif
    }
#endif//HAL_ADC_FEATURE_PERIODIC_MODE_SUPPORT	

#if 0
    uint32_t data_len;
    uint32_t data[64];
    uint32_t index;

    halADC_Fifo_Get_Length(&data_len);
    while (data_len != 0) {
        halADC_Fifo_Read_Data(data_len, data);
        printf("\r\n ########data_len = %d########", data_len);
        for (index = 0; index < data_len; index++) {
            printf("\r\ndata[%d] = 0x%08x", index, data[index]);
        }
        halADC_Fifo_Get_Length(&data_len);
    }
    while (1);
#endif
#if 0
    if (g_rAdcInfo.u48msCnt >= COUNT_8MS) { //8ms accuracy test
        //printf("\t-Enter halADC_VDMA_Callback-\n");
#if CPU_CLK_DVT
        printf("\t-u48msCnt(%d)-\n", g_rAdcInfo.u48msCnt);
        switch (g_8SecCount % 3) {
            case 0:
                printf("\t-=================DMA=======================-\n");
                printf("\t-cmnCpuClkConfigureTo192M-\n");
                cmnCpuClkConfigureTo192M();
                break;
            case 1:
                printf("\t-$$$$$$$$$$$$$$$$$DMA$$$$$$$$$$$$$$$$$$$$$$$-\n");
                printf("\t-cmnCpuClkConfigureTo160M-\n");   //192M->160M & 160M -> 192M has issue
                cmnCpuClkConfigureToXtal_();
                cmnCpuClkConfigureTo160M();
                break;
            case 2:
                printf("\t-#################DMA#######################-\n");
                printf("\t-cmnCpuClkConfigureTo64M-\n");
                cmnCpuClkConfigureTo64M();
                break;
        }
        g_8SecCount++;
#endif

#if DBG_8MS_DVT
        printf("\t-u48msCntDMA(%d)-\n", g_rAdcInfo.u48msCnt);
        printf("\t-================DMA Interrupt========================-\n");
#endif

        //printf("\tDMAFifoReadPtr/0x830D00D4(%08X) \n",pru4FifoReadPtr[0]);
        //for(i = (UINT32)DmaPtr1; i < (UINT32)DmaPtr2; i=i+4)
        //{
        //    PrintDmaPtr = (UINT32*)i;
        //    printf("\t%08X(%08X) \n", i, PrintDmaPtr[0]);
        //}
    }

#if (CM4_INDIUM_PROJ == 1)
    CHIP_ADC_Periodic_DMA_MultiCh_Test();
    accdetKeyPressDetection(CHIP_ADC_GetAdcData(10));
    hpdetHpImpedanceDetection(CHIP_ADC_GetAdcData(9));
#endif

    if (g_rAdcInfo.u48msCnt >= COUNT_8MS) {
        g_rAdcInfo.u48msCnt = 0;
        //printf("\t-Leave halADC_VDMA_Callback-\n");
        //printf("\tDMAFifoReadPtr/0x830D00D4(%08X) \n",pru4FifoReadPtr[0]);
        //for(i = (UINT32)DmaPtr1; i < (UINT32)DmaPtr2; i=i+4)
        //{
        //    PrintDmaPtr = (UINT32*)i;
        //    printf("\t%08X(%08X) \n", i, PrintDmaPtr[0]);
        //}
    }

    g_rAdcInfo.u48msCnt++;


    if (g_rAdcInfo.rFsmParam.ePmode == ADC_PMODE_ONE_TIME) {
        halADC_Fsm_Disable(FALSE);
    }
#endif
}

#if CHK_TIME_OUT_INT_TRIGER_LVL_INT_DVT
UINT32 g_u4ReadWritePtr = 0;
UINT32 g_u4IIR = 0;
#endif

void halADC_LISR(hal_nvic_irq_t irq_number)
{
#if 0
#if DBG_8MS_DVT | CHK_TIME_OUT_INT_TRIGER_LVL_INT_DVT
    UINT32 *pru4FifoReadPtr = (UINT32 *)0x830D00D4;
    UINT32 *pru4IIR = (UINT32 *)0x830D0008;
#endif

#if CHK_TIME_OUT_INT_TRIGER_LVL_INT_DVT //DE Ethan suggest save readwrite ptr & IIR info to SRAM instead of UART log to check if the interrupt source is time out or trigger level
    //printf("\t-halADC_LISR(0)-\n"); //UART print will impact readwrite ptr & IIR info to check if the interrupt source is time out or trigger level
    g_u4ReadWritePtr = pru4FifoReadPtr[0];
    g_u4IIR = pru4IIR[0];
    printf("\tg_u4ReadWritePtr/0x830D00D4(%08X) \n", g_u4ReadWritePtr);
    printf("\tg_u4IIR/0x830D0008(%08X) \n", g_u4IIR);

    printf("\tFifoReadPtr/0x830D00D4(%08X) \n", pru4FifoReadPtr[0]);
    printf("\tIIRPtr/0x830D0008(%08X) \n", pru4IIR[0]);
#endif

    if (g_rAdcInfo.u48msCntFiFo >= COUNT_8MS) { //8ms accuracy test
        //printf("\t-Enter halADC_VDMA_Callback-\n");
#if CPU_CLK_DVT
        printf("\t-u48msCntFiFo(%d)-\n", g_rAdcInfo.u48msCntFiFo);
        switch (g_8SecCount % 3) {
            case 0:
                printf("\t-================FIFO========================-\n");
                printf("\t-cmnCpuClkConfigureTo192M-\n");
                cmnCpuClkConfigureTo192M();
                break;
            case 1:
                printf("\t-$$$$$$$$$$$$$$$$FIFO$$$$$$$$$$$$$$$$$$$$$$$$-\n");
                printf("\t-cmnCpuClkConfigureTo160M-\n");   //192M->160M & 160M -> 192M has issue
                cmnCpuClkConfigureToXtal_();
                cmnCpuClkConfigureTo160M();
                break;
            case 2:
                printf("\t-################FIFO########################-\n");
                printf("\t-cmnCpuClkConfigureTo64M-\n");
                cmnCpuClkConfigureTo64M();
                break;
        }
        g_8SecCount++;
#endif

#if DBG_8MS_DVT
        printf("\t-u48msCntFiFo(%d)-\n", g_rAdcInfo.u48msCntFiFo);
        printf("\t-================FIFO Interrupt========================-\n");
        printf("\tFifoReadPtr/0x830D00D4(%08X) \n", pru4FifoReadPtr[0]);
#endif

        //printf("\tDMAFifoReadPtr/0x830D00D4(%08X) \n",pru4FifoReadPtr[0]);
        //for(i = (UINT32)DmaPtr1; i < (UINT32)DmaPtr2; i=i+4)
        //{
        //    PrintDmaPtr = (UINT32*)i;
        //    printf("\t%08X(%08X) \n", i, PrintDmaPtr[0]);
        //}
    }

#if (CM4_INDIUM_PROJ == 1)
    //CHIP_ADC_Periodic_MultiCh8Ch15_Test();
    CHIP_ADC_Periodic_MultiCh_Test();
    accdetKeyPressDetection(CHIP_ADC_GetAdcData(10));
    hpdetHpImpedanceDetection(CHIP_ADC_GetAdcData(9));
#endif

    if (g_rAdcInfo.u48msCntFiFo >= COUNT_8MS) {
        g_rAdcInfo.u48msCntFiFo = 0;
        //printf("\t-Leave halADC_VDMA_Callback-\n");
        //printf("\tDMAFifoReadPtr/0x830D00D4(%08X) \n",pru4FifoReadPtr[0]);
        //for(i = (UINT32)DmaPtr1; i < (UINT32)DmaPtr2; i=i+4)
        //{
        //    PrintDmaPtr = (UINT32*)i;
        //    printf("\t%08X(%08X) \n", i, PrintDmaPtr[0]);
        //}
    }

    g_rAdcInfo.u48msCntFiFo++;

    if (g_rAdcInfo.rFsmParam.ePmode == ADC_PMODE_ONE_TIME) {
        halADC_Fsm_Disable(FALSE);
    }
#endif
}

void halADC_COMP_LISR(hal_nvic_irq_t irq_number)
{
#ifdef HAL_ADC_FEATURE_THRESHOLD_MODE_SUPPORT
    uint8_t compare_flag;
    hal_adc_channel_t channel;

    //NVIC_DisableIRQ(CM4_ADC_COMP_IRQ);

    //printf("\r\n-halADC_COMP_LISR-\n");
    /* Disable ADC */
    mSetHWEntry(ADC_FSM_EN, 0);
    mSetHWEntry(ADC_REG_COMP_IRQ_EN, 0);
    //mSetHWEntry(ADC_ALDO_EN, 0);//close LDO for power saving???
    compare_flag = mGetHWEntry(ADC_RO_ADC_COMP_FLAG);

#ifdef ADC_DRIVER_DEBUG
    printf("\r\n compare_flag = 0x%02x", compare_flag);
#endif
    //for multiple channels detection support extension.
    for (channel = HAL_ADC_CHANNEL_0; channel < HAL_ADC_CHANNEL_MAX; channel ++) {
        if ((compare_flag & 1) && (s_adc_threshold_callback[ channel ].func != NULL)) {
            s_adc_threshold_callback[ channel ].func(HAL_ADC_EVENT_OK, s_adc_threshold_callback[ channel ].parameter);
        }
        compare_flag >>= 1;
    }

    //NVIC_EnableIRQ(CM4_ADC_COMP_IRQ);
#endif
}

ENUM_HAL_RET_T halADC_Fsm_Enable(void)
{

#if ASIC_DVT
    //ADCT_ACD6_DE2 Jason suggestion
    //HAL_REG_32(0x83008184) = 0;   //ADC period
    //u4Tmp = HAL_REG_32(0x81021438);
    //HAL_REG_32(0x81021438) = (u4Tmp | BIT(1));  //Set 0x81021438[1]=1, then wait 200us for 2.5V output ready    //EVB J49(not JP49) for ALDO 2.5V
//    mSetHWEntry(ADC_ALDO_EN, 1);  //Set 0x81021438[1]=1, then wait 200us for 2.5V output ready    //EVB J49(not JP49) for ALDO 2.5V
#endif
#if 0
    if (ADC_FIFO_DIRECT == g_rAdcInfo.rFsmParam.eFifo_Mode) {
        /* Enable ADC IRQ */
        NVIC_EnableIRQ(CM4_ADC_IRQ);

        /* Enable ADC Comparator IRQ */
//        NVIC_EnableIRQ(CM4_ADC_COMP_IRQ);
    }
#endif
    /* Enable ADC */
//    mSetHWEntry(ADC_FSM_EN, 1);

    return HAL_RET_SUCCESS;
}

ENUM_HAL_RET_T halADC_Fsm_Disable(BOOLEAN fgOffAldo)    //DE conclude if on-off period < 200ms, not turn off ALDO for LPDVT
{
    /* Disable ADC */
    mSetHWEntry(ADC_FSM_EN, 0);

    if (ADC_FIFO_DIRECT == g_rAdcInfo.rFsmParam.eFifo_Mode) {
        /* Disable ADC IRQ */
        NVIC_DisableIRQ((IRQn_Type) CM4_ADC_IRQ);

        /* Disable ADC Comparator IRQ */
        NVIC_DisableIRQ((IRQn_Type) CM4_ADC_COMP_IRQ);
    }

#if ASIC_DVT
    //ADCT_ACD6_DE2 Jason suggestion
    //HAL_REG_32(0x83008184) = 0;   //ADC period
    //u4Tmp = HAL_REG_32(0x81021438);
    //HAL_REG_32(0x81021438) = (u4Tmp | BIT(1));  //Set 0x81021438[1]=1, then wait 200us for 2.5V output ready    //EVB J49(not JP49) for ALDO 2.5V
    if (fgOffAldo) {
        mSetHWEntry(ADC_ALDO_EN, 0);  //Set 0x81021438[1]=1, then wait 200us for 2.5V output ready    //EVB J49(not JP49) for ALDO 2.5V
    }
#endif

    return HAL_RET_SUCCESS;
}

ENUM_HAL_RET_T halADC_Fsm_Param_Set(P_ADC_FSM_PARAM_T prAdcFsmParam)
{
    UINT8   u1ChnlNum = 0, i = 0;
    //UINT32  u4Tmp = 0;

    //ASSERT(prAdcFsmParam);

    /* Parameter check */
    if (prAdcFsmParam->u2Channel_Map == 0) {
        //printf("\t[ADC] illegal channel map\n");
        return HAL_RET_FAIL;
    }

    for (i = 0; i < MAX_CH_NUM; i++) {
        if (prAdcFsmParam->u2Channel_Map & BIT(i)) {
            u1ChnlNum++;
        }
    }

    //g_rAdcInfo.u48msCnt = 0;
    //g_rAdcInfo.u48msCntFiFo = 0;
    g_rAdcInfo.rFsmParam.ePmode = prAdcFsmParam->ePmode;
    g_rAdcInfo.rFsmParam.eAvg_Mode = prAdcFsmParam->eAvg_Mode;
    g_rAdcInfo.rFsmParam.u2Channel_Map = prAdcFsmParam->u2Channel_Map;
    g_rAdcInfo.rFsmParam.u4Period = prAdcFsmParam->u4Period;
    g_rAdcInfo.rFsmParam.eFifo_Mode = prAdcFsmParam->eFifo_Mode;
    g_rAdcInfo.rFsmParam.pru4DmaVfifoAddr = prAdcFsmParam->pru4DmaVfifoAddr;
    g_rAdcInfo.rFsmParam.u4DmaVfifoLen = prAdcFsmParam->u4DmaVfifoLen;
    g_rAdcInfo.rFsmParam.u4DmaVfifoTriggerLevel = prAdcFsmParam->u4DmaVfifoTriggerLevel;
    g_rAdcInfo.u1ChnlNum = u1ChnlNum;

    //printf("\t[ADC] Apply Init 64 CPU\n");
    //cmnCpuClkConfigureTo64M();

#if 0
#if ASIC_DVT
    printf("\t[ADC] Apply ALDO setting\n");
    //ADCT_ACD6_DE2 Jason suggestion
    //HAL_REG_32(0x83008184) = 0;   //ADC period
    //u4Tmp = HAL_REG_32(0x81021438);
    //HAL_REG_32(0x81021438) = (u4Tmp | BIT(1));  //Set 0x81021438[1]=1, then wait 200us for 2.5V output ready    //EVB J49(not JP49) for ALDO 2.5V
//    mSetHWEntry(ADC_ALDO_EN, 1);  //Set 0x81021438[1]=1, then wait 200us for 2.5V output ready    //EVB J49(not JP49) for ALDO 2.5V
#endif
#endif

    /*
    #if HP_DVT
        //DE Sophia suggestion to fix ACCDET read no ADC data
        printf("\t[ADC] Apply HP DVT setting\n");
        HAL_REG_32(0x830B252C) = 0x00000000;
        HAL_REG_32(0x830B2518) = 0x0000C67F;
        HAL_REG_32(0x830B2414) = 0x00000001;
        HAL_REG_32(0x830B2418) = 0x00000003;
    #endif
    */

#if AUDIO_DVT   //ADCT_ACD1_DE6 Chen-Chien request to down clock(812.5K) to meet MSFT requirement 500K samples/sec
    printf("\t[ADC] Apply AUDIO DVT setting(ADC-812.5K)\n");
    mSetHWEntry(HCLK_2M_DIV_SEL_SW, 1);
    mSetHWEntry(HCLK_2M_SW_DIV_SEL, 0x0C);  //0x0C(2M)//0x19(1M)//0x1F(812.5K)
#endif

#if BUCK_DVT
    //ADCT_ACD6_DE2 Jason suggestion, BUCK regulator clock need to be set as 2MHZ(AUXADC)
    printf("\t[ADC] Apply BUCK DVT setting\n");
    HAL_REG_32(0x8102140C) = 0x80407000;    //Set BUCK regulator to PWM mode
    HAL_REG_32(0x8102112C) = 0x00008C04;    //Set extclk clock for BUCK regulator to 2MHz
    HAL_REG_32(0x81021430) = 0x00030001;    //Set BUCK regulator clock to extclk
    HAL_REG_32(0x8300818C) = 0xF0F0AB58;    //Set REG_AUXADC
#endif

#if TIME_OUT_DVT   //Time out extend
    printf("\t[ADC] Apply Time out DVT setting\n");
    mSetHWEntry(ADC_RTO_EXT, 0);
    mSetHWEntry(ADC_RTOCNT, 0x40);
#endif

    //printf("\t[ADC] Apply cmnCpuClkConfigureTo192M() setting\n");
    //cmnCpuClkConfigureTo192M();

    /* Set ADC parameters */
    mSetHWEntry(ADC_PMODE_EN, prAdcFsmParam->ePmode);
    mSetHWEntry(ADC_REG_AVG_MODE, prAdcFsmParam->eAvg_Mode);
    mSetHWEntry(ADC_REG_CH_MAP, prAdcFsmParam->u2Channel_Map);
    mSetHWEntry(ADC_REG_PERIOD, prAdcFsmParam->u4Period);

    //DE sophia tmp solution
    //mSetHWEntry(ADC_REG_ADC_DATA_SYNC_MODE, 1); //negative edge trigger
    mSetHWEntry(ADC_REG_ADC_DATA_SYNC_MODE, 0); //positive edge trigger
    mSetHWEntry(ADC_REG_ADC_TIMESTAMP_EN, 1);   //Timestamp enable
    //mSetHWEntry(ADC_REG_INTERNAL_CLK_SAMPLE_DATA_EN, 1);    //use internal clk
    //mSetHWEntry(ADC_RXFEN, 1);  //enable interrupt    //DE Ethan suggest trigger interrupt & timeout interrupt only used in FIFO mode

    switch (prAdcFsmParam->eFifo_Mode) {
        case ADC_FIFO_DMA:
            /* Initialize ADC Virtual FIFO DMA*/
            DMA_Init();
            DMA_Vfifo_init();
            //set alert
#if 0
            printf("\t-DMA Alert Enable-\n");
            printf("\t-u1ChnlNum(%d)-\n", u1ChnlNum);
            printf("\t-pru4DmaVfifoAddr(0x%08X)/u4DmaVfifoLen(%d)-\n", (uint32_t)prAdcFsmParam->pru4DmaVfifoAddr, prAdcFsmParam->u4DmaVfifoLen);
#endif

            DMA_Vfifo_SetAdrs((UINT32)prAdcFsmParam->pru4DmaVfifoAddr, prAdcFsmParam->u4DmaVfifoLen, VDMA_ADC_RX_CH, 0 /* alert length */, prAdcFsmParam->u4DmaVfifoTriggerLevel /* threshold */, 0 /* no need to use timeout */);
            DMA_Vfifo_Flush(VDMA_ADC_RX_CH);
            DMA_Vfifo_Register_Callback(VDMA_ADC_RX_CH, halADC_VDMA_Callback);

            /* DMA mode */
            mSetHWEntry(ADC_RX_DMA_EN, 1);
            mSetHWEntry(ADC_RXFEN, 0);  //enable interrupt    //DE Ethan suggest trigger interrupt & timeout interrupt only used in FIFO mode
            break;
        case ADC_FIFO_DIRECT:
#if 0
            if (u1ChnlNum == MAX_CH_NUM) {
                printf("\t[ADC] We recommend to use DMA mode to handle this case!\n");
                printf("\t[ADC] Please check ADC datasheet!\n");
                return HAL_RET_FAIL;
            }
#endif

//        printf("\t[ADC] FIFO mode\n");

            /* Direct mode */
            mSetHWEntry(ADC_RX_DMA_EN, 0);
            mSetHWEntry(ADC_RXFEN, 0);  //disable interrupt    //DE Ethan/Sophia suggest to disable and use polling mode instaed, due to trigger interrupt & timeout interrupt may occur in FIFO mode

            /* Register ADC IRQ */
            hal_nvic_register_isr_handler(CM4_ADC_IRQ, halADC_LISR);
            NVIC_SetPriority((IRQn_Type) CM4_ADC_IRQ, CM4_ADC_PRI);

            /* Register ADC Comparator IRQ */
            //NVIC_Register(CM4_ADC_COMP_IRQ, halADC_COMP_LISR);
            //NVIC_SetPriority(CM4_ADC_COMP_IRQ, CM4_ADC_COMP_PRI);

            //DE sophia suggest solution
            mSetHWEntry(ADC_RX_TRI_LVL, u1ChnlNum);    //must set trigger level equal to used channel number!!

            //IoT comparator
            //mSetHWEntry(ADC_REG_COMP_IRQ_EN, BITS(0,1));  //enable comparator interrupt
            //mSetHWEntry(ADC_REG_COMP_THRESHOLD, 0x400);  //comparator threshold
            break;
    }

//    printf("\t[ADC] Fsm param setting OK!\n");
    return HAL_RET_SUCCESS;
}

#if 0
ENUM_HAL_RET_T halADC_Fsm_Param_Get(P_ADC_FSM_PARAM_T prAdcFsmParam)
{
    ASSERT(prAdcFsmParam);

    prAdcFsmParam->ePmode = g_rAdcInfo.rFsmParam.ePmode;
    prAdcFsmParam->eAvg_Mode = g_rAdcInfo.rFsmParam.eAvg_Mode;
    prAdcFsmParam->u2Channel_Map = g_rAdcInfo.rFsmParam.u2Channel_Map;
    prAdcFsmParam->u4Period = g_rAdcInfo.rFsmParam.u4Period;
    prAdcFsmParam->eFifo_Mode = g_rAdcInfo.rFsmParam.eFifo_Mode;
    prAdcFsmParam->pru4DmaVfifoAddr = g_rAdcInfo.rFsmParam.pru4DmaVfifoAddr;
    prAdcFsmParam->u4DmaVfifoLen = g_rAdcInfo.rFsmParam.u4DmaVfifoLen;
    prAdcFsmParam->u4DmaVfifoTriggerLevel = g_rAdcInfo.rFsmParam.u4DmaVfifoTriggerLevel;

    return HAL_RET_SUCCESS;
}
#endif

ENUM_HAL_RET_T halADC_Fifo_Get_Length(UINT32 *pru4Len)
{
    UINT32 u4ReadPos = 0, u4WritePos = 0;

    switch (g_rAdcInfo.rFsmParam.eFifo_Mode) {
        case ADC_FIFO_DMA:
            /* Get the data length from VFIFO DMA FFCNT */
            *pru4Len = DMA_GetVFIFO_Avail(VDMA_ADC_RX_CH);
            break;
        case ADC_FIFO_DIRECT:
            /* Get the length from read/write pointer */
            u4ReadPos = mGetHWEntry(ADC_RX_PTR_READ);
            u4WritePos = mGetHWEntry(ADC_RX_PTR_WRITE);

            if (u4WritePos == u4ReadPos) {
                *pru4Len = 0;
            } else if (u4WritePos > u4ReadPos) {
                *pru4Len = (u4WritePos - u4ReadPos);
            } else {
                /* u4WritePos < u4ReadPos */
                *pru4Len = (u4WritePos + ADC_FIFO_SIZE - u4ReadPos);
            }
            break;
    }

    return HAL_RET_SUCCESS;
}

ENUM_HAL_RET_T halADC_Fifo_Read_Data(UINT32 u4Len, UINT32 *pru4Samples)
{
    UINT32 u4Counter = 0;

    switch (g_rAdcInfo.rFsmParam.eFifo_Mode) {
        case ADC_FIFO_DMA:
            /* Pull out data from ADC VFIFO DMA virtual port */
#if 0
            if (g_rAdcInfo.u48msCnt <= COUNT_8MS) { //8s
                //printf("\t-DMAReaPort/u4Len(%d)-\n",u4Len);
            }
#endif
            for (u4Counter = 0; u4Counter < u4Len; u4Counter++) {
                pru4Samples[u4Counter] = (UINT32)(*(volatile UINT32 *)DMA_VPORT(VDMA_ADC_RX_CH));
            }
            break;
        case ADC_FIFO_DIRECT:
            /* Pull out data from RX buffer register */
            //printf("\t-ReaPort/u4Len(%d)-\n",u4Len);
            for (u4Counter = 0; u4Counter < u4Len; u4Counter++) {
                pru4Samples[u4Counter] = (UINT32)mGetHWEntry(ADC_RBR);
            }
            break;
    }

    return HAL_RET_SUCCESS;
}


#ifdef __cplusplus
}
#endif

#endif /* HAL_ADC_MODULE_ENABLED */


