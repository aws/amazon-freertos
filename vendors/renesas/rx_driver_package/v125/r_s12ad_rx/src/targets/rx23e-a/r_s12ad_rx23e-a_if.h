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
* Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_s12ad_rx23w_if.h
* Description  : 12-bit A/D Converter driver interface file.
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           28.02.2020 1.00    Initial Release.
***********************************************************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "platform.h"

#ifndef S12AD_PRV_RX23E_A_IF_H
#define S12AD_PRV_RX23E_A_IF_H

/******************************************************************************
Macro definitions
*******************************************************************************/
#if (BSP_CFG_PCKB_DIV > BSP_CFG_PCKD_DIV)
#define ADC_SST_CNT_MIN     (6)    /* For PCLKB < ADCLK */
#else
#define ADC_SST_CNT_MIN     (5)    /* For PCLKB >= ADCLK */
#endif /* (BSP_CFG_PCKB_DIV > BSP_CFG_PCKD_DIV) */
#define ADC_SST_CNT_MAX     (255)
#define ADC_SST_CNT_DEFAULT (13)


/* for ADC_CMD_ENABLE_CHANS */
/* Bitwise OR these masks together for desired channels and sensors
   Used for all commands containing a "mask" or "flags" field */
#define ADC_MASK_CH0    (1<<0)
#define ADC_MASK_CH1    (1<<1)
#define ADC_MASK_CH2    (1<<2)
#define ADC_MASK_CH3    (1<<3)
#define ADC_MASK_CH4    (1<<4)
#define ADC_MASK_CH5    (1<<5)

#define ADC_MASK_GROUPB_OFF         (0)
#define ADC_MASK_ADD_OFF            (0)
#define ADC_MASK_SAMPLE_HOLD_OFF    (0)

#define ADC_DDA_STATE_CNT_MIN       (2)  /* ADDISCR.ADNDIS[0..3] bits. */
#define ADC_DDA_STATE_CNT_MAX       (15)


/*****************************************************************************
Typedef definitions
******************************************************************************/

/***** ADC_OPEN() ARGUMENT DEFINITIONS *****/

typedef enum e_adc_mode
{
    ADC_MODE_SS_ONE_CH,             // single scan one channel
    ADC_MODE_SS_MULTI_CH,           // 1 trigger source, scan multiple channels
    ADC_MODE_CONT_ONE_CH,           // continuous scan one channel
    ADC_MODE_CONT_MULTI_CH,         // continuous scan multiple channels
    ADC_MODE_SS_ONE_CH_DBLTRIG,     // on even triggers save to ADDBLDR & interrupt
    ADC_MODE_SS_MULTI_CH_GROUPED,   // 2 trigger sources, scan multiple channels
    ADC_MODE_SS_MULTI_CH_GROUPED_DBLTRIG_A,
    ADC_MODE_MAX                    // This definition DO NOT USE for R_ADC_Open() argument
} adc_mode_t;

typedef enum e_adc_align
{
    ADC_ALIGN_RIGHT = 0x0000,
    ADC_ALIGN_LEFT  = 0x8000
} adc_align_t;

typedef enum e_adc_add               // Addition and averaging of sampled data.
{
    ADC_ADD_OFF             = 0,     // addition is turned off for chans/sensors
    ADC_ADD_TWO_SAMPLES     = 1,     // Two samples are added,Register ADADC.ADC[1,0]
    ADC_ADD_THREE_SAMPLES   = 2,     // Three samples are added
    ADC_ADD_FOUR_SAMPLES    = 3,     // Four samples are added
    ADC_ADD_SIXTEEN_SAMPLES = 5,     // Sixteen samples are added
    ADC_ADD_AVG_2_SAMPLES   = 0x81,  // average 2 samples
    ADC_ADD_AVG_4_SAMPLES   = 0x83,  // average 4 samples
} adc_add_t;

typedef enum e_adc_clear
{
    ADC_CLEAR_AFTER_READ_OFF = 0x0000,
    ADC_CLEAR_AFTER_READ_ON  = 0x0020
} adc_clear_t;


typedef enum e_adc_trig              // trigger sources (set to TRSA bit or TRSB bit)
{
    ADC_TRIG_ASYNC_ADTRG            = 0,    /* external asynchronous trigger; not for Group modes
                                               nor double trigger modes */
    ADC_TRIG_SYNC_TRG0AN            = 1,    // MTU0 TRGA
    ADC_TRIG_SYNC_TRG0BN            = 2,    // MTU0 TRGB
    ADC_TRIG_SYNC_TRGAN_OR_UDF4N    = 3,    // MTUx TRGA or MTU4 underflow(complementary PWM mode)
    ADC_TRIG_SYNC_TRG0EN            = 4,    // MTU0 TRGE
    ADC_TRIG_SYNC_TRG0FN            = 5,    // MTU0 TRGF
    ADC_TRIG_SYNC_TRG4AN            = 6,    // MTU4 TADCORA
    ADC_TRIG_SYNC_TRG4BN            = 7,    // MTU4 TADCORB
    ADC_TRIG_SYNC_TRG4AN_AND_TRG4BN = 8,    // MTU4 TADCORA and MTU4 TADCORB
    ADC_TRIG_SYNC_ELC               = 9,    // ELC

    ADC_TRIG_SOFTWARE               = 15,   /* software trigger; not for Group modes
                                               nor double trigger modes
                                               This is not set to TRSA or TRSB */
    ADC_TRIG_NONE                   = 0x3F
} adc_trig_t;

typedef enum e_adc_speed
{
    ADC_CONVERT_SPEED_DEFAULT = 0,
    ADC_CONVERT_SPEED_HIGH    = 0,
    ADC_CONVERT_CURRENT_LOW   = 1
} adc_speed_t;

typedef struct st_adc_cfg
{
    adc_speed_t     conv_speed;      // ADCSR.ADHSC, rx23e-a has two conversion speeds
    adc_align_t     alignment;       // ADCER.ADRFMT, Left or Right alignment,ignored if addition used
    adc_add_t       add_cnt;         // ADADC.ADC, Addition/average count select.
    adc_clear_t     clearing;        // ADCER.ACE, Automatic clearing enable/disable after read
    adc_trig_t      trigger;         // ADSTRGR.TRSA, default and Group A trigger source
    adc_trig_t      trigger_groupb;  // valid only for group modes
    uint8_t         priority;        // for S12ADIO int; 1=lo 15=hi 0=off/polled
    uint8_t         priority_groupb; // S12GBADI interrupt priority; 0-15
} adc_cfg_t;


/***** ADC_CONTROL() ARGUMENT DEFINITIONS *****/

typedef enum e_adc_cmd
{
    /* Commands for special hardware configurations */
    ADC_CMD_USE_VREFL0,             // Low reference. Default is to use AVSS0.
    ADC_CMD_USE_VREFH0,             // High reference. Default is to use AVCC0.
    ADC_CMD_SET_DDA_STATE_CNT,      // For Disconnect Detection Assist
    ADC_CMD_SET_SAMPLE_STATE_CNT,   // Set the conversion time

    /* Command to configure channels, sensors, and comparator */
    ADC_CMD_ENABLE_CHANS,           // Configure channels to scan
    ADC_CMD_EN_COMPARATOR_LEVEL,    // Enable comparator for threshold compare
    ADC_CMD_EN_COMPARATOR_WINDOW,   // Enable comparator for range compare

    /* Commands to enable hardware triggers or cause software trigger */
    ADC_CMD_ENABLE_TRIG,            // ADCSR.TRGE=1 for sync/asynchronous triggers
    ADC_CMD_SCAN_NOW,               // Software trigger start scan

    /* Commands to poll for scan completion and comparator results */
    ADC_CMD_CHECK_SCAN_DONE,        // For Normal or GroupA scan
    ADC_CMD_CHECK_SCAN_DONE_GROUPA,
    ADC_CMD_CHECK_SCAN_DONE_GROUPB,
    ADC_CMD_CHECK_CONDITION_MET,    // comparator condition

    /* Advanced control commands */
    ADC_CMD_DISABLE_TRIG,           // ADCSR.TRGE=0 for sync/async trigs
    ADC_CMD_DISABLE_INT,            // interrupt disable; ADCSR.ADIE=0
    ADC_CMD_ENABLE_INT,             // interrupt enable;  ADCSR.ADIE=1
    ADC_CMD_DISABLE_INT_GROUPB,     // interrupt disable; ADCSR.GBADIE=0
    ADC_CMD_ENABLE_INT_GROUPB       // interrupt enable;  ADCSR.GBADIE=1
} adc_cmd_t;


/* for ADC_CMD_SET_DDA_STATE_CNT */

typedef enum e_adc_charge           // Disconnection Detection Assist (DDA)
{
    ADC_DDA_DISCHARGE = 0x00,
    ADC_DDA_PRECHARGE = 0x01,
    ADC_DDA_OFF       = 0x02
} adc_charge_t;


typedef struct st_adc_dda
{
    adc_charge_t    method;         // Discharge or Precharge
    uint8_t         num_states;     // 2-15 (0 = DISABLED, 1 is invalid)
} adc_dda_t;


/* for ADC_CMD_SET_SAMPLE_STATE_CNT */

typedef enum e_adc_sst_reg          // sample state registers
{
    ADC_SST_CH0 = 0,
    ADC_SST_CH1,
    ADC_SST_CH2,
    ADC_SST_CH3,
    ADC_SST_CH4,
    ADC_SST_CH5,
    ADC_SST_REG_MAX = ADC_SST_CH5
} adc_sst_reg_t;


typedef struct st_adc_sst
{
    adc_sst_reg_t   reg_id;
    uint8_t         num_states;
} adc_sst_t;


typedef enum e_adc_grpa                 // action when groupa interrupts groupb scan
{
    ADC_GRPA_PRIORITY_OFF = 0,          // groupa ignored and does not interrupt groupb
    ADC_GRPA_GRPB_WAIT_TRIG = 1,        // groupb restarts at next trigger
    ADC_GRPA_GRPB_RESTART_SCAN = 3,     // groupb restarts immediately
    ADC_GRPA_GRPB_CONT_SCAN= 0x8001,    // groupb restarts immediately and scans continuously
} adc_grpa_t;

typedef enum e_adc_diag                 // Self-Diagnosis Channel
{
    ADC_DIAG_OFF          = 0x00,
    ADC_DIAG_0_VOLT       = 0x01,
    ADC_DIAG_HALF_VREFH0  = 0x02,
    ADC_DIAG_VREFH0       = 0x03,
    ADC_DIAG_ROTATE_VOLTS = 0x04
} adc_diag_t;

typedef enum e_adc_elc
{
    ADC_ELC_SCAN_DONE = 0,              // normal scan or Group A scan complete
    ADC_ELC_GROUPB_SCAN_DONE,
    ADC_ELC_ALL_SCANS_DONE
} adc_elc_t;

typedef struct st_adc_ch_cfg
{
    uint32_t        chan_mask;          // channels/bits 0-5
    uint32_t        chan_mask_groupb;   // valid for group modes
    adc_grpa_t      priority_groupa;    // valid for group modes
    uint32_t        add_mask;           // valid if add enabled in Open()
    adc_diag_t      diag_method;        // self-diagnosis virtual channel
    adc_elc_t       signal_elc;         //
} adc_ch_cfg_t;


/* for ADC_CMD_EN_COMPARATOR_LEVEL and ADC_CMD_EN_COMPARATOR_WINDOW */

typedef struct st_adc_cmpwin_cfg        /* bit-OR ADC_MASK_xxx to
                                           indicate channels/sensors */
{
    uint32_t        compare_mask;       // channels/sensors to compare
    uint32_t        inside_window_mask; /* condition met when within range
                                           default=0 met when outside range */
    uint16_t        level_lo;           // Low-value of window
    uint16_t        level_hi;           // High-value of window
    bool            windowa_enable;     // comparison window A enable
} adc_cmpwin_t;


/* for ADC_CMD_CHECK_CONDITION_MET use uint32_t */

/***** ADC_READ() ARGUMENT DEFINITIONS *****/
typedef enum e_adc_reg
{
    ADC_REG_CH0  = 0,     // Channel 0
    ADC_REG_CH1,
    ADC_REG_CH2,
    ADC_REG_CH3,
    ADC_REG_CH4,
    ADC_REG_CH5,
    ADC_REG_DBLTRIG,      // Data Duplication register
    ADC_REG_SELF_DIAG,    // self-diagnosis register
    ADC_REG_MAX = ADC_REG_SELF_DIAG
} adc_reg_t;

/* ADC_READALL() ARGUMENT DEFINITIONS */

typedef struct st_adc_data
{
    uint16_t    chan[6];
    uint16_t    dbltrig;
    uint16_t    self_diag;
} adc_data_t;



/*****************************************************************************
Public Functions
******************************************************************************/

#endif /* S12AD_PRV_RX23E_A_IF_H */

