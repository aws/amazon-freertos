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
* Copyright (C) 2016-2017 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_s12ad_rx_if.h
* Description  : 12-bit A/D Converter driver interface file.
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           22.07.2013 1.00    Initial Release.
*           21.04.2014 1.20    Updated for RX210 advanced features; RX110/63x support.
*           05.06.2014 1.30    Fixed channel mask bug in adc_enable_chans()
*           06.11.2014 1.40    Added RX113 support.
*           25.03.2015 2.00    Merged in r_s12ad_rx64m and added support for rx71m.
*           25.03.2015 2.10    Added rx231 support and updated the minor version information
*           01.03.2016 2.11    Added RX130 and RX230 support.
*           01.10.2016 2.20    Added RX65x support.
*           03.04.2017 2.30    Added RX65N-2MB and RX130-512KB support.
***********************************************************************************************************************/

#ifndef S12AD_IF_H
#define S12AD_IF_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "platform.h"

/* MCU SPECIFIC USER INTERFACE FILES */

#if defined(BSP_MCU_RX110)
#include "./src/targets/rx110/r_s12ad_rx110_if.h"
#elif defined(BSP_MCU_RX111)
#include "./src/targets/rx111/r_s12ad_rx111_if.h"
#elif defined(BSP_MCU_RX113)
#include "./src/targets/rx113/r_s12ad_rx113_if.h"
#elif defined(BSP_MCU_RX130)
#include "./src/targets/rx130/r_s12ad_rx130_if.h"
#elif defined(BSP_MCU_RX210)
#include "./src/targets/rx210/r_s12ad_rx210_if.h"
#elif defined(BSP_MCU_RX230)
#include "./src/targets/rx230/r_s12ad_rx230_if.h"
#elif defined(BSP_MCU_RX231)
#include "./src/targets/rx231/r_s12ad_rx231_if.h"
#elif defined(BSP_MCU_RX63_ALL)
#include "./src/targets/rx63x/r_s12ad_rx63x_if.h"
#elif defined(BSP_MCU_RX64M)
#include "./src/targets/rx64m/r_s12ad_rx64m_if.h"
#elif defined(BSP_MCU_RX65_ALL)
#include "./src/targets/rx65x/r_s12ad_rx65x_if.h"
#elif defined(BSP_MCU_RX71M)
#include "./src/targets/rx71m/r_s12ad_rx71m_if.h"
#endif

/******************************************************************************
Macro definitions
*******************************************************************************/
/* Version Number of API. */
#define ADC_VERSION_MAJOR       (2)
#define ADC_VERSION_MINOR       (30)

/*****************************************************************************
Typedef definitions
******************************************************************************/

typedef enum e_adc_err      // ADC API error codes
{
    ADC_SUCCESS = 0,
    ADC_ERR_AD_LOCKED,          // Open() call is in progress elsewhere
    ADC_ERR_AD_NOT_CLOSED,      // peripheral still running in another mode
    ADC_ERR_MISSING_PTR,        // missing required pointer argument
    ADC_ERR_INVALID_ARG,        // argument is not valid for parameter
    ADC_ERR_ILLEGAL_ARG,        // argument is illegal for mode
    ADC_ERR_SCAN_NOT_DONE,      // default, Group A, or Group B scan not done
    ADC_ERR_TRIG_ENABLED,       // scan running, cannot configure comparator
    ADC_ERR_CONDITION_NOT_MET,  // no chans/sensors passed comparator condition
    ADC_ERR_UNKNOWN             // unknown hardware error
} adc_err_t;


/* CALLBACK FUNCTION ARGUMENT DEFINITIONS */

typedef enum e_adc_cb_evt           // callback function events
{
    ADC_EVT_SCAN_COMPLETE,          // normal/Group A scan complete
    ADC_EVT_SCAN_COMPLETE_GROUPB,   // Group B scan complete
#if (defined(BSP_MCU_RX65_ALL))
    ADC_EVT_SCAN_COMPLETE_GROUPC,   // Group C scan complete
#endif    
#if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX65_ALL))
    ADC_EVT_CONDITION_MET,          // 1+ chans/sensors met comparator condition
#endif
#if (defined(BSP_MCU_RX65_ALL))
    ADC_EVT_CONDITION_METB          // 1+ chans/sensors met comparator condition
#endif
} adc_cb_evt_t;

typedef struct st_adc_cb_args       // callback arguments
{
    adc_cb_evt_t   event;
#if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX65_ALL))
    uint32_t       compare_flags;   // valid only for compare event in Window A
#if (defined(BSP_MCU_RX65_ALL))
    uint32_t       compare_flagsb;  // valid only for compare event in Window B
#endif
    uint8_t        unit;
#endif
} adc_cb_args_t;


/*****************************************************************************
Public Functions
******************************************************************************/
adc_err_t R_ADC_Open(uint8_t const      unit,
                     adc_mode_t const   mode,
                     adc_cfg_t * const  p_cfg,
                     void               (* const p_callback)(void *p_args));

adc_err_t R_ADC_Control(uint8_t const   unit,
                        adc_cmd_t const cmd,
                        void * const    p_args);

adc_err_t R_ADC_Read(uint8_t const      unit,
                     adc_reg_t const    reg_id,
                     uint16_t * const   p_data);

adc_err_t R_ADC_ReadAll(adc_data_t * const  p_all_data);

adc_err_t R_ADC_Close(uint8_t const unit);

uint32_t  R_ADC_GetVersion(void);

                                    
#endif /* S12AD_IF_H */
