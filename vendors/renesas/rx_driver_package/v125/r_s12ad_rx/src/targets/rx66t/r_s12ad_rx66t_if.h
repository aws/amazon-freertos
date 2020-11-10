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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_s12ad_rx66t_if.h
* Description  : 12-bit A/D Converter driver interface file.
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           03.09.2018 1.00    Initial Release.
*           05.04.2019 4.00    Modified comment.
***********************************************************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "platform.h"

#ifndef S12AD_PRV_RX66T_IF_H
#define S12AD_PRV_RX66T_IF_H

/******************************************************************************
Macro definitions
*******************************************************************************/
#define ADC_0_REG_ARRAY_MAX   (8)
#define ADC_1_REG_ARRAY_MAX   (8)
#define ADC_2_REG_ARRAY_MAX   (14)

#define ADC_DDA_STATE_CNT_MIN       (3)
#define ADC_DDA_STATE_CNT_6         (6)
#define ADC_DDA_STATE_CNT_9         (9)
#define ADC_DDA_STATE_CNT_12        (12)
#define ADC_DDA_STATE_CNT_MAX       (15)

#define ADC_SST_CNT_MIN     (12)
#define ADC_SST_CNT_MAX     (252)
#define ADC_SST_CNT_DEFAULT (27)

/* for ADC_CMD_ENABLE_CHANS */

/* Bitwise OR these masks together for desired channels and sensors
   Used for all commands containing a "mask" or "flags" field */
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
#define ADC_MASK_CH16   (1<<16)
#define ADC_MASK_CH17   (1<<17)
#define ADC_MASK_TEMP   (1<<18)     /* temperature sensor */
#define ADC_MASK_VOLT   (1<<19)     /* internal reference voltage sensor */

#define ADC_MASK_SENSORS            (ADC_MASK_TEMP | ADC_MASK_VOLT)
#define ADC_MASK_GROUPB_OFF         (0)
#define ADC_MASK_GROUPC_OFF         (0)
#define ADC_MASK_ADD_OFF            (0)
#define ADC_MASK_SAMPLE_HOLD_OFF    (0)
#define ADC_MASK_CONV_ORDER_OFF     (0)

#define ADC_SST_SH_CNT_MIN      (12)    /* minimum sample&hold states */
#define ADC_SST_SH_CNT_MAX      (252)   /* maximum sample&hold states */
#define ADC_SST_SH_CNT_DEFAULT  (27)    /* default sample&hold states */

#define ADC_PGA_CHANNEL_MAX   (3)  /* ch0-2 */

/* Window B Channel select */
#define     ADC_COMP_WINB_CH0   (0)
#define     ADC_COMP_WINB_CH1   (1)
#define     ADC_COMP_WINB_CH2   (2)
#define     ADC_COMP_WINB_CH3   (3)
#define     ADC_COMP_WINB_CH4   (4)
#define     ADC_COMP_WINB_CH5   (5)
#define     ADC_COMP_WINB_CH6   (6)
#define     ADC_COMP_WINB_CH7   (7)
#define     ADC_COMP_WINB_CH8   (8)
#define     ADC_COMP_WINB_CH9   (9)
#define     ADC_COMP_WINB_CH10  (10)
#define     ADC_COMP_WINB_CH11  (11)
#define     ADC_COMP_WINB_CH16  (16)
#define     ADC_COMP_WINB_CH17  (17)
#define     ADC_COMP_WINB_TEMP  (32)
#define     ADC_COMP_WINB_VOLT  (33)

/* Window B Comparison condition select */
#define     ADC_COMP_WINB_COND_BELOW    (0) /* condition met when outside range or below threshold */
#define     ADC_COMP_WINB_COND_ABOVE    (1) /* condition met when within range or above threshold */


/*****************************************************************************
Typedef definitions
******************************************************************************/



/***** ADC_OPEN() ARGUMENT DEFINITIONS *****/

typedef enum e_adc_mode
{
    ADC_MODE_SS_ONE_CH,                             // single scan one channel
    ADC_MODE_SS_MULTI_CH,                           // 1 trigger source, scan multiple channels
    ADC_MODE_CONT_ONE_CH,                           // continuous scan one channel
    ADC_MODE_CONT_MULTI_CH,                         // continuous scan multiple channels
    ADC_MODE_SS_ONE_CH_DBLTRIG,                     // on even triggers save to ADDBLDR & interrupt
    ADC_MODE_SS_MULTI_CH_GROUPED,                   // 2 trigger sources, scan multiple channels
    ADC_MODE_SS_MULTI_CH_GROUPED_GROUPC,            // 3 trigger sources, scan multiple channels
    ADC_MODE_SS_MULTI_CH_GROUPED_DBLTRIG_A,
    ADC_MODE_SS_MULTI_CH_GROUPED_DBLTRIG_A_GROUPC,
    ADC_MODE_MAX                                    // This definition DO NOT USE for R_ADC_Open() argument.
} adc_mode_t;

typedef enum e_adc_align
{
    ADC_ALIGN_RIGHT = 0x0000,
    ADC_ALIGN_LEFT  = 0x8000
} adc_align_t;

typedef enum e_adc_add
{
    ADC_ADD_OFF = 0,                // addition is turned off for chans/sensors
    ADC_ADD_TWO_SAMPLES = 1,
    ADC_ADD_THREE_SAMPLES = 2,
    ADC_ADD_FOUR_SAMPLES = 3,
    ADC_ADD_SIXTEEN_SAMPLES = 5,
    ADC_ADD_AVG_2_SAMPLES = 0x81,   // average 2 samples
    ADC_ADD_AVG_4_SAMPLES = 0x83,   // average 4 samples
} adc_add_t;

typedef enum e_adc_clear
{
    ADC_CLEAR_AFTER_READ_OFF = 0x0000,
    ADC_CLEAR_AFTER_READ_ON  = 0x0020
} adc_clear_t;

typedef enum e_adc_trig           // trigger sources (set to TRSA bit or TRSB bit)
{
    ADC_TRIG_ASYNC_ADTRG                 = 0,   /* ext asynchronous trigger; not for Group modes
                                                   nor double trigger modes */
    ADC_TRIG_SYNC_TRG0AN                 = 1,   // MTU0 TRGA
    ADC_TRIG_SYNC_TRG1AN                 = 2,   // MTU1 TRGA
    ADC_TRIG_SYNC_TRG2AN                 = 3,   // MTU2 TRGA
    ADC_TRIG_SYNC_TRG3AN                 = 4,   // MTU3 TRGA
    ADC_TRIG_SYNC_TRG4AN_OR_UDF4N        = 5,   // MTU4 TRGA or MTU4 underflow(complementary PWM mode)
    ADC_TRIG_SYNC_TRG6AN                 = 6,   // MTU6 TRGA
    ADC_TRIG_SYNC_TRG7AN_OR_UDF7N        = 7,   // MTU7 TRGA or MTU7 underflow(complementary PWM mode)
    ADC_TRIG_SYNC_TRG0EN                 = 8,   // MTU0 TRGE
    ADC_TRIG_SYNC_TRG4AN                 = 9,   // MTU4 TADCORA
    ADC_TRIG_SYNC_TRG4BN                 = 10,  // MTU4 TADCORB
    ADC_TRIG_SYNC_TRG4AN_OR_TRG4BN       = 11,  // MTU4 TADCORA or MTU4 TADCORB
    ADC_TRIG_SYNC_TRG4AN_AND_TRG4BN      = 12,  // MTU4 TADCORA and MTU4 TADCORB
    ADC_TRIG_SYNC_TRG7AN                 = 13,  // MTU7 TADCORA
    ADC_TRIG_SYNC_TRG7BN                 = 14,  // MTU7 TADCORB
    ADC_TRIG_SYNC_TRG7AN_OR_TRG7BN       = 15,  // MTU7 TADCORA or MTU7 TADCORB
    ADC_TRIG_SYNC_TRG7AN_AND_TRG7BN      = 16,  // MTU7 TADCORA and MTU7 TADCORB
    ADC_TRIG_SYNC_TRG9AN                 = 19,  // MTU9 TRGA
    ADC_TRIG_SYNC_TRG9EN                 = 20,  // MTU9 TRGE
    ADC_TRIG_SYNC_TRG0AN_OR_TRG0EN       = 25,  // MTU0 TRGA or MTU0 TRGE
    ADC_TRIG_SYNC_TRG9AN_OR_TRG9EN       = 26,  // MTU9 TRGA or MTU9 TRGE
    ADC_TRIG_SYNC_TRG0AN_OR_TRG9AN       = 27,  // MTU0 TRGA or MTU9 TRGA
    ADC_TRIG_SYNC_TRG0EN_OR_TRG9EN       = 28,  // MTU0 TRGE or MTU9 TRGE
    ADC_TRIG_SYNC_TRG9AN_AND_TRG9EN      = 33,  // MTU0 TRGA and MTU0 TRGE
    ADC_TRIG_SYNC_TRG0AN_AND_TRG0EN      = 34,  // MTU9 TRGA and MTU9 TRGE
    ADC_TRIG_SYNC_TRG0AN_AND_TRG9AN      = 35,  // MTU0 TRGA and MTU9 TRGA
    ADC_TRIG_SYNC_TRG0EN_AND_TRG9EN      = 36,  // MTU0 TRGE and MTU9 TRGE
    ADC_TRIG_SYNC_TMRTRG0AN              = 29,  // TMR0 TCORA
    ADC_TRIG_SYNC_TMRTRG2AN              = 30,  // TMR2 TCORA
    ADC_TRIG_SYNC_TMRTRG4AN              = 31,  // TMR4 TCORA
    ADC_TRIG_SYNC_TMRTRG6AN              = 32,  // TMR6 TCORA
    ADC_TRIG_SYNC_ELCTRG0                = 50,  // Activation source 0 from ELC (ELCTRG00N, ELCTRG10N or ELCTRG20N)
    ADC_TRIG_SYNC_ELCTRG1                = 51,  // Activation source 1 from ELC (ELCTRG01N, ELCTRG11N or ELCTRG21N)
    ADC_TRIG_SYNC_ELCTRG0_OR_ELCTRG1     = 58,  // Activation source 0 from ELC or Activation source 1 from ELC
    ADC_TRIG_SOFTWARE                    = 64,  /* software trigger; not for Group modes
                                                   nor double trigger modes
                                                   This is not set to TRSA or TRSB */
    ADC_TRIG_NONE                        = 0x3F
} adc_trig_t;

typedef enum e_adc_temp
{
    ADC_TEMP_SENSOR_NOT_AD_CONVERTED = 0,   // Temperature sensor output is not A/D-converted.
    ADC_TEMP_SENSOR_AD_CONVERTED,           /* Temperature sensor output is A/D-converted.
                                               (Group A temperature sensor output is A/D-converted.) */
    ADC_TEMP_SENSOR_AD_CONVERTED_GROUPB,    // Group B temperature sensor output is A/D-converted.
    ADC_TEMP_SENSOR_AD_CONVERTED_GROUPC     // Group C temperature sensor output is A/D-converted.
} adc_temp_t;

typedef enum e_adc_add_temp
{
    ADC_TEMP_SENSOR_ADD_OFF = 0,   // addition/average is turned off for temperature sensors
    ADC_TEMP_SENSOR_ADD_ON         // addition/average is turned on for temperature sensors
} adc_add_temp_t;

typedef struct st_adc_cfg
{
    adc_align_t     alignment;        // ignored if addition used
    adc_add_t       add_cnt;          // add or average samples
    adc_clear_t     clearing;         // clear after read
    adc_trig_t      trigger;          // default and Group A trigger source
    adc_trig_t      trigger_groupb;   // valid only for group modes
    adc_trig_t      trigger_groupc;   // valid only for group modes
    uint8_t         priority;         // for S12ADIO int; 1=lo 15=hi 0=off/polled
    uint8_t         priority_groupb;  // S12GBADI interrupt priority; 0-15
    uint8_t         priority_groupc;  // S12GCADI interrupt priority; 0-15
    adc_temp_t      temp_sensor;      // temperature sensor output A/D conversion select
    adc_add_temp_t  add_temp_sensor;  // temperature sensor output A/D converted value addition/average mode select
} adc_cfg_t;


/***** ADC_CONTROL() ARGUMENT DEFINITIONS *****/

typedef enum e_adc_cmd
{
    /* Commands for special hardware configurations */
    ADC_CMD_SET_DDA_STATE_CNT,      // for Disconnect Detection Assist
    ADC_CMD_SET_SAMPLE_STATE_CNT,

    /* Command to configure channels, sensors, and comparator */
    ADC_CMD_ENABLE_CHANS,           // configure channels and sensors to scan
    ADC_CMD_EN_COMPARATOR_LEVEL,    // enable comparator for threshold compare
    ADC_CMD_EN_COMPARATOR_WINDOW,   // enable comparator for range compare
    ADC_CMD_COMP_COMB_STATUS,       // get status for WindowA/B composite condition

    /* Commands to enable hardware triggers or cause software trigger */
    ADC_CMD_ENABLE_TRIG,            // ADCSR.TRGE=1 for sync/async triggers
    ADC_CMD_SCAN_NOW,               // software trigger start scan

    /* Commands to poll for scan completion and comparator results */
    ADC_CMD_CHECK_SCAN_DONE,        // for Normal or GroupA scan
    ADC_CMD_CHECK_SCAN_DONE_GROUPA,
    ADC_CMD_CHECK_SCAN_DONE_GROUPB,
    ADC_CMD_CHECK_SCAN_DONE_GROUPC,
    ADC_CMD_CHECK_CONDITION_MET,    // comparator condition
    ADC_CMD_CHECK_CONDITION_METB,   // comparator condition

    /* Advanced control commands */
    ADC_CMD_DISABLE_TRIG,           // ADCSR.TRGE=0 for sync/async trigs
    ADC_CMD_DISABLE_INT,            // interrupt disable; ADCSR.ADIE=0
    ADC_CMD_ENABLE_INT,             // interrupt enable;  ADCSR.ADIE=1
    ADC_CMD_DISABLE_INT_GROUPB,     // interrupt disable; ADCSR.GBADIE=0
    ADC_CMD_ENABLE_INT_GROUPB,      // interrupt enable;  ADCSR.GBADIE=1
    ADC_CMD_DISABLE_INT_GROUPC,     // interrupt disable; ADGCTRGR.GCADIE=0
    ADC_CMD_ENABLE_INT_GROUPC       // interrupt enable;  ADGCTRGR.GCADIE=1
} adc_cmd_t;

/* for ADC_CMD_SET_DDA_STATE_CNT */

typedef enum e_adc_charge           // Disconnection Detection Assist (DDA)
{
    ADC_DDA_DISCHARGE = 0x00,
    ADC_DDA_PRECHARGE = 0x01,
    ADC_DDA_OFF = 0x02
} adc_charge_t;


typedef struct st_adc_dda
{
    adc_charge_t    method;
    uint8_t         num_states;     // 0, 3, 6, 9, 12, 15
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
    ADC_SST_CH6,
    ADC_SST_CH7,                    // end unit0 and unit1 
    ADC_SST_CH8,
    ADC_SST_CH9,
    ADC_SST_CH10,
    ADC_SST_CH11,
    ADC_SST_CH16_17,
    ADC_SST_TEMPERATURE,
    ADC_SST_VOLTAGE,
    ADC_SST_REG_MAX = ADC_SST_VOLTAGE
} adc_sst_reg_t;


typedef struct st_adc_sst
{
    adc_sst_reg_t   reg_id;
    uint8_t         num_states;     // ch16-17 use the same value
} adc_sst_t;



typedef enum e_adc_grpa                 // action when groupa interrupts groupb scan
{
    ADC_GRPA_PRIORITY_OFF = 0,                          // groupa ignored and does not interrupt groupb and groupc
    ADC_GRPA_GRPB_GRPC_WAIT_TRIG = 1,                   // groupb and groupc restart at next trigger
    ADC_GRPA_GRPB_GRPC_TOP_RESTART_SCAN = 3,            /* groupb and groupc restart immediately
                                                           and scans from the head of the channel */
    ADC_GRPA_GRPB_GRPC_RESTART_TOP_CONT_SCAN = 0x8003,  /* groupb and groupc restart immediately
                                                           and scans continuously from the head of the channel */
    ADC_GRPA_GRPB_GRPC_RESTART_SCAN = 0x4003,           // groupb and groupc restart immediately and scans continuously
    ADC_GRPA_GRPB_GRPC_TOP_CONT_SCAN = 0x8001,          /* groupb and groupc restart
                                                           and scans continuously from the head of the channel */
    ADC_GRPA_GRPB_GRPC_RESTART_CONT_SCAN = 0xC003,      // groupb and groupc restart immediately and scans continuously
} adc_grpa_t;

typedef enum e_adc_diag                 // Self-Diagnosis Channel
{
    ADC_DIAG_OFF = 0x00,
    ADC_DIAG_0_VOLT = 0x01,
    ADC_DIAG_HALF_VREFH0 = 0x02,
    ADC_DIAG_VREFH0 = 0x03,
    ADC_DIAG_ROTATE_VOLTS = 0x04
} adc_diag_t;

typedef enum e_adc_elc
{
    ADC_ELC_GROUPA_SCAN_DONE = 0x00,      // normal scan or Group A scan complete
    ADC_ELC_GROUPB_SCAN_DONE = 0x01,
    ADC_ELC_ANY_ONE_OF_SCAN_DONE = 0x02,
    ADC_ELC_GROUPC_SCAN_DONE = 0x04
} adc_elc_t;


typedef enum e_adc_conv_order_stat
{
    ADC_CONV_ORDER_AUTO_SETTING = 0,
    ADC_CONV_ORDER_MANUAL_SETTING = 1
} adc_conv_order_stat_t;


/* 
 * Control the output destination of the analog signal and PGA.
 * (x = 000-002, 100-102; m = 0-5;)
 * ----------------------------------------------------------------------------------------
 *  adc_path_ctrl_t              || PGA                | ADC       | CMPCm0    | CMPCm1    |
 * ------------------------------||--------------------|-----------|-----------|-----------|
 *  ADC_ANALOG_INPUT_1           || Disable            | ANx       | -         | -         |
 *  ADC_ANALOG_INPUT_2           || Disable            | -         | ANx       | -         |
 *  ADC_ANALOG_INPUT_3           || Disable            | ANx       | ANx       | -         |
 *  ADC_PGA_SINGLE_END_INPUT_1   || Single end         | -         | ANx       | Px output |
 *  ADC_PGA_SINGLE_END_INPUT_2   || Single end         | ANx       | ANx       | Px output |
 *  ADC_PGA_SINGLE_END_INPUT_3   || Single end         | Px output | ANx       | Px output |
 *  ADC_PGA_DIFFERENTIAL_INPUT_1 || Differential input | -         | -         | Px output |
 *  ADC_PGA_DIFFERENTIAL_INPUT_2 || Differential input | ANx       | -         | Px output |
 *  ADC_PGA_DIFFERENTIAL_INPUT_3 || Differential input | Px output | -         | Px output |
 *  ADC_GENERAL_PORT_1           || Disable            | -         | -         | -         |
 * ----------------------------------------------------------------------------------------
 */
typedef enum e_adc_path_ctrl
{
    ADC_ANALOG_INPUT_1 = 0,
    ADC_ANALOG_INPUT_2,
    ADC_ANALOG_INPUT_3,
    ADC_PGA_SINGLE_END_INPUT_1,
    ADC_PGA_SINGLE_END_INPUT_2,
    ADC_PGA_SINGLE_END_INPUT_3,
    ADC_PGA_DIFFERENTIAL_INPUT_1,
    ADC_PGA_DIFFERENTIAL_INPUT_2,
    ADC_PGA_DIFFERENTIAL_INPUT_3,
    ADC_GENERAL_PORT_1,
    ADC_PATH_MAX
} adc_path_ctrl_t;

typedef enum e_adc_pga_gain
{
    /* PGA_DISABLE */
    ADC_PGA_GAIN_OFF = 0x00,

    /* PGA_SINGLE_END */
    ADC_PGA_GAIN_2_000  = 0x10,    /* x 2.000 */
    ADC_PGA_GAIN_2_500  = 0x11,    /* x 2.500 */
    ADC_PGA_GAIN_3_077  = 0x13,    /* x 3.077 */
    ADC_PGA_GAIN_3_636  = 0x15,    /* x 3.636 */
    ADC_PGA_GAIN_4_000  = 0x16,    /* x 4.000 */
    ADC_PGA_GAIN_4_444  = 0x17,    /* x 4.444 */
    ADC_PGA_GAIN_5_000  = 0x18,    /* x 5.000 */
    ADC_PGA_GAIN_6_667  = 0x1A,    /* x 6.667 */
    ADC_PGA_GAIN_8_000  = 0x1B,    /* x 8.000 */
    ADC_PGA_GAIN_10_000 = 0x1C,    /* x 10.000 */
    ADC_PGA_GAIN_13_333 = 0x1D,    /* x 13.333 */
    ADC_PGA_GAIN_20_000 = 0x1E,    /* x 20.000 */

    /* PGA_DIFFERENTIAL_INPUT */
    ADC_PGA_GAIN_1_500_DIFF  = 0x21,    /* x 1.500 */
    ADC_PGA_GAIN_4_000_DIFF  = 0x28,    /* x 4.000 */
    ADC_PGA_GAIN_7_000_DIFF  = 0x2B,    /* x 7.000 */
    ADC_PGA_GAIN_12_333_DIFF = 0x2D     /* x 12.333 */
} adc_pga_gain_t;

typedef struct st_adc_ch_cfg            // bit 0 is ch0; bit 15 is ch15
{
    uint32_t               chan_mask;           // channels/bits 0-15
    uint32_t               chan_mask_groupb;    // valid for group modes
    uint32_t               chan_mask_groupc;    // valid for group modes
    adc_grpa_t             priority_groupa;     // valid for group modes
    uint32_t               add_mask;            // valid if add enabled in Open()
    adc_diag_t             diag_method;         // self-diagnosis virtual channel
    adc_elc_t              signal_elc;
    uint8_t                sample_hold_mask;    // channels/bits 0-2
    uint8_t                sample_hold_states;  // minimum .4us
    adc_conv_order_stat_t  conv_order_status;
    uint32_t               conversion_order[ADC_2_REG_ARRAY_MAX];
    adc_path_ctrl_t        signal_path_control[ADC_PGA_CHANNEL_MAX];
    adc_pga_gain_t         pga_gain[ADC_PGA_CHANNEL_MAX];
} adc_ch_cfg_t;

/* COMP */

typedef enum e_adc_comp_cond            // Window A/B Composite Conditions
{
    ADC_COND_OR = 0x00,
    ADC_COND_EXOR = 0x01,
    ADC_COND_AND = 0x02
} adc_comp_cond_t;

typedef enum e_adc_comp_stat           // Window A/B Composite Status
{
    ADC_COMP_COND_NOTMET = 0x00,       // Window A/B Composite Condition not met
    ADC_COMP_COND_MET    = 0x01        // Window A/B Composite Condition met
} adc_comp_stat_t;


/* for ADC_CMD_EN_COMPARATOR_LEVEL and ADC_CMD_EN_COMPARATOR_WINDOW */

typedef struct st_adc_cmpwin_cfg                /* Window A bit-OR ADC_MASK_xxx to indicate channels/sensors
                                                   Window B ADC_COMP_WINB_xxx to indicate channels/sensors */
{
    uint32_t            compare_mask;           // channels/sensors to compare
    uint32_t            compare_maskb;          // channels/sensors to compareb
    uint32_t            inside_window_mask;     /* condition met when within range
                                                   default=0 met when outside range */
    uint32_t            inside_window_maskb;    /* condition met when within range
                                                   default=0 met when outside range */
    uint16_t            level_lo;
    uint16_t            level_lob;
    uint16_t            level_hi;
    uint16_t            level_hib;
    adc_comp_cond_t     comp_cond;              // window A/B composite conditions setting
    uint8_t             int_priority;           /* S12CMPAI and S12CMPBI priority level
                                                   1=low 15=hi 0=polled */
    bool                windowa_enable;         // comparison window A enable
    bool                windowb_enable;         // comparison window B enable
} adc_cmpwin_t;

/* for ADC_CMD_CHECK_CONDITION_MET use uint32_t */

/***** ADC_READ() ARGUMENT DEFINITIONS *****/

typedef enum e_adc_reg
{
    ADC_REG_CH0 = 0,
    ADC_REG_CH1 = 1,
    ADC_REG_CH2 = 2,
    ADC_REG_CH3 = 3,
    ADC_REG_CH4 = 4,
    ADC_REG_CH5 = 5,
    ADC_REG_CH6 = 6,
    ADC_REG_CH7 = 7,    // end unit0 and unit1 
    ADC_REG_CH8 = 8,
    ADC_REG_CH9 = 9,
    ADC_REG_CH10 = 10,
    ADC_REG_CH11 = 11,
    ADC_REG_CH16 = 12,
    ADC_REG_CH17 = 13,
    ADC_REG_TEMP,
    ADC_REG_VOLT,

    /* both units */
    ADC_REG_DBLTRIG,
    ADC_REG_DBLTRIGA,   // loaded when first multi-source trigger was A
    ADC_REG_DBLTRIGB,   // loaded when first multi-source trigger was B
    ADC_REG_SELF_DIAG,
    ADC_REG_MAX = ADC_REG_SELF_DIAG
} adc_reg_t;

/* ADC_READALL() ARGUMENT DEFINITIONS */

typedef struct st_adc_unit0_data
{
    uint16_t    chan[ADC_0_REG_ARRAY_MAX];
    uint16_t    dbltrig;
    uint16_t    dbltrigA;
    uint16_t    dbltrigB;
    uint16_t    self_diag;
} adc_unit0_data_t;

typedef struct st_adc_unit1_data
{
    uint16_t    chan[ADC_1_REG_ARRAY_MAX];
    uint16_t    dbltrig;
    uint16_t    dbltrigA;
    uint16_t    dbltrigB;
    uint16_t    self_diag;
} adc_unit1_data_t;

typedef struct st_adc_unit2_data
{
    uint16_t    chan[ADC_2_REG_ARRAY_MAX];
    uint16_t    temp;
    uint16_t    volt;
    uint16_t    dbltrig;
    uint16_t    dbltrigA;
    uint16_t    dbltrigB;
    uint16_t    self_diag;
} adc_unit2_data_t;

typedef struct st_adc_data
{
    adc_unit0_data_t   unit0;
    adc_unit1_data_t   unit1;
    adc_unit2_data_t   unit2;
} adc_data_t;

/*****************************************************************************
Public Functions
******************************************************************************/


#endif /* S12AD_PRV_RX66T_IF_H */

