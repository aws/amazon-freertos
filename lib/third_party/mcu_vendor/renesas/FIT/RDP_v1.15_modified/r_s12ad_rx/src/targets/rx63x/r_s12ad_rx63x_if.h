/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2014-2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_s12ad_rx63x_if.h
* Description  : 12-bit A/D Converter driver interface file for RX63N, RX631.
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           22.02.2014 1.00    Initial Release.
*           01.10.2016 2.20    Define name commoditize for the same meaning but different name define.
*                              Delete parameter check by the enum value.
*                              R_ADC_ReadAll() parameter change.(adjust the parameter structure to RX130/RX230/RX231)
***********************************************************************************************************************/

#ifndef S12AD_RX63X_IF_H
#define S12AD_RX63X_IF_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/

/******************************************************************************
Macro definitions
*******************************************************************************/

/*****************************************************************************
Typedef definitions
******************************************************************************/

#ifdef BSP_MCU_RX63T
    #error ERROR - RX63T is not supported by this driver.
#endif


/* ADC_OPEN() ARGUMENT DEFINITIONS */

typedef enum e_adc_mode
{
    ADC_MODE_SS_TEMPERATURE,        // single scan temperature sensor
    ADC_MODE_SS_INT_REF_VOLT,       // single scan internal ref voltage sensor
    ADC_MODE_SS_ONE_CH,             // single scan one channel
    ADC_MODE_SS_MULTI_CH,           // 1 trigger source, scan multiple channels
    ADC_MODE_CONT_ONE_CH,           // continuous scan one channel
    ADC_MODE_CONT_MULTI_CH,         // continuous scan multiple channels
    ADC_MODE_MAX                    // This definition DO NOT USE for R_ADC_Open() argument
} adc_mode_t;

typedef enum e_adc_trig             // trigger sources (set to ADSTRS bit)
{
    ADC_TRIG_ASYNC_ADTRG           = 0,     // ADTRG#0
    ADC_TRIG_SYNC_TRG0AN           = 1,     // MTU0 TRGA
    ADC_TRIG_SYNC_TRG0BN           = 2,     // MTU0 TRGB
    ADC_TRIG_SYNC_TRGAN            = 3,     // MTUx TRGA
    ADC_TRIG_SYNC_TPUTRGAN         = 4,     // TPUx TRGA
    ADC_TRIG_SYNC_TRG0EN           = 5,     // MTU0 TRGE
    ADC_TRIG_SYNC_TRG0FN           = 6,     // MTU0 TRGF
    ADC_TRIG_SYNC_TRG4AN_OR_TRG4BN = 7,     // MTU4 TADCORA or MTU4 TADCORB
    ADC_TRIG_SYNC_TPUTRG0AN        = 8,     // TPU0 TRGA
    ADC_TRIG_SYNC_TMRTRG0AN        = 9,     // TMR0 TCORA
    ADC_TRIG_SYNC_TMRTRG2AN        = 10,    // TMR2 TCORA

    ADC_TRIG_SOFTWARE              = 16     // software trigger(This is not set to ADSTRS)
} adc_trig_t;

typedef enum e_adc_add
{
    ADC_ADD_OFF = 0,                // addition is turned off for chans/sensors
    ADC_ADD_TWO_SAMPLES = 1,
    ADC_ADD_THREE_SAMPLES = 2,
    ADC_ADD_FOUR_SAMPLES = 3
} adc_add_t;

typedef enum e_adc_align
{
    ADC_ALIGN_RIGHT = 0x0000,
    ADC_ALIGN_LEFT  = 0x8000
} adc_align_t;

typedef enum e_adc_clear
{
    ADC_CLEAR_AFTER_READ_OFF = 0x0000,
    ADC_CLEAR_AFTER_READ_ON  = 0x0020
} adc_clear_t;

typedef enum e_adc_speed
{
    ADC_CONVERT_SPEED_PCLK_DIV8 = 0x00,
    ADC_CONVERT_SPEED_PCLK_DIV4 = 0x01,
    ADC_CONVERT_SPEED_PCLK_DIV2 = 0x02,
    ADC_CONVERT_SPEED_PCLK = 0x03
} adc_speed_t;


typedef struct st_adc_cfg
{
    adc_add_t       add_cnt;
    adc_align_t     alignment;      // ignored if addition used
    adc_clear_t     clearing;
    adc_speed_t     conv_speed;
    adc_trig_t      trigger;
    uint8_t         priority;       // for S12ADIO int; 1=lo 15=hi 0=off/polled
} adc_cfg_t;


/* ADC_CONTROL() ARGUMENT DEFINITIONS */

typedef enum e_adc_cmd
{
    // Commands for special hardware configurations
    ADC_CMD_SET_SAMPLE_STATE_CNT,

    // Commands to enable channels or sensors
    ADC_CMD_ENABLE_CHANS,           // Enables channels. (INT enabled only if priority != 0)
    ADC_CMD_ENABLE_TEMP_SENSOR,     // enables sensor and INT if priority != 0
    ADC_CMD_ENABLE_VOLT_SENSOR,     // enables sensor and INT if priority != 0

    // Commands to enable hardware triggers or cause software trigger
    ADC_CMD_ENABLE_TRIG,            // ADCSR.TRGE=1 for sync/async triggers
    ADC_CMD_SCAN_NOW,               // software trigger start scan

    // Commands to poll for scan completion
    ADC_CMD_CHECK_SCAN_DONE,        // for Normal

    // Advanced control commands
    ADC_CMD_DISABLE_TRIG,           // ADCSR.TRGE=0 for sync/async trigs
    ADC_CMD_DISABLE_INT,            // interrupt disable; ADCSR.ADIE=0
    ADC_CMD_ENABLE_INT              // interrupt enable;  ADCSR.ADIE=1
} adc_cmd_t;


/* for ADC_CMD_SET_SAMPLE_STATE_CNT */

typedef enum e_adc_sst_reg          // sample state registers
{
    ADC_SST_CH0_TO_20,
    ADC_SST_TEMPERATURE
} adc_sst_reg_t;

#define ADC_SST_CNT_MIN     (10)
#define ADC_SST_CNT_MAX     (255)
#define ADC_SST_CNT_DEFAULT (20)

typedef struct st_adc_time
{
    adc_sst_reg_t   reg_id;
    uint8_t         num_states;
} adc_time_t;


/* for ADC_CMD_ENABLE_CHANS */

// Bitwise OR these masks together for desired channels
#define ADC_MASK_CH0    (1<<0)
#define ADC_MASK_CH1    (1<<1)
#define ADC_MASK_CH2    (1<<2)
#define ADC_MASK_CH3    (1<<3)
#define ADC_MASK_CH4    (1<<4)
#define ADC_MASK_CH5    (1<<5)
#define ADC_MASK_CH6    (1<<6)
#define ADC_MASK_CH7    (1<<7)
#define ADC_MASK_CH8    (1<<8)
#define ADC_MASK_CH9    (1<<9)
#define ADC_MASK_CH10   (1<<10)
#define ADC_MASK_CH11   (1<<11)
#define ADC_MASK_CH12   (1<<12)
#define ADC_MASK_CH13   (1<<13)
#define ADC_MASK_CH14   (1<<14)
#define ADC_MASK_CH15   (1<<15)
#define ADC_MASK_CH16   (1<<16)
#define ADC_MASK_CH17   (1<<17)
#define ADC_MASK_CH18   (1<<18)
#define ADC_MASK_CH19   (1<<19)
#define ADC_MASK_CH20   (1<<20)

#define ADC_MASK_ADD_OFF        (0)

typedef struct st_adc_ch_cfg            // bit 0 is ch0; bit 20 is ch20
{
    uint32_t        chan_mask;          // channels/bits 0-20
    uint32_t        add_mask;           // valid if add enabled in Open()
} adc_ch_cfg_t;


/* ADC_READ() ARGUMENT DEFINITIONS */

typedef enum e_adc_reg
{
    ADC_REG_CH0 = 0,
    ADC_REG_CH1,
    ADC_REG_CH2,
    ADC_REG_CH3,
    ADC_REG_CH4,
    ADC_REG_CH5,
    ADC_REG_CH6,
    ADC_REG_CH7,
    ADC_REG_CH8,
    ADC_REG_CH9,
    ADC_REG_CH10,
    ADC_REG_CH11,
    ADC_REG_CH12,
    ADC_REG_CH13,
    ADC_REG_CH14,
    ADC_REG_CH15,
    ADC_REG_CH16,
    ADC_REG_CH17,
    ADC_REG_CH18,
    ADC_REG_CH19,
    ADC_REG_CH20,
    ADC_REG_TEMP,
    ADC_REG_VOLT,
    ADC_REG_MAX = ADC_REG_VOLT
} adc_reg_t;

#define ADC_REG_ARRAY_MAX   (21)


/* ADC_READALL() ARGUMENT DEFINITIONS */

typedef struct st_adc_data
{
    uint16_t    chan[ADC_REG_ARRAY_MAX];
    uint16_t    temp;
    uint16_t    volt;
} adc_data_t;

                                   
#endif /* S12AD_RX63X_IF_H */
