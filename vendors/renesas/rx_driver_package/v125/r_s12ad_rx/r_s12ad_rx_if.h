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
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
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
*           03.09.2018 3.00    Added RX66T support.
*           03.12.2018 3.01    Changed Minor version to '01' for update of xml file.
*           28.12.2018 3.10    Added RX72T support.
*           05.04.2019 4.00    Changed Major/Minor version to 4.00 for update below.
*                              - Added support of GNUC and ICCRX.
*                              - End of Update for RX210, RX631, and RX63N.
*                              Deleted include of RX210, RX631, and RX63N.
*                              Modified include pass of RX65N.
*           06.28.2019 4.10    Added RX23W support.
*           07.31.2019 4.20    Added RX72M support.
*           08.30.2019 4.30    Added RX13T support.
*           22.11.2019 4.40    Added RX66N and RX72N support.
*           31.01.2020 4.41    Added support for RX13T with IAR Compiler.
*           28.02.2020 4.50    Added RX23E-A support.
***********************************************************************************************************************/

#ifndef S12AD_PRV_IF_H
#define S12AD_PRV_IF_H

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
#elif defined(BSP_MCU_RX13T)
#include "./src/targets/rx13T/r_s12ad_rx13t_if.h"
#elif defined(BSP_MCU_RX230)
#include "./src/targets/rx230/r_s12ad_rx230_if.h"
#elif defined(BSP_MCU_RX231)
#include "./src/targets/rx231/r_s12ad_rx231_if.h"
#elif defined(BSP_MCU_RX23E_A)
#include "./src/targets/rx23e-a/r_s12ad_rx23e-a_if.h"
#elif defined(BSP_MCU_RX23W)
#include "./src/targets/rx23w/r_s12ad_rx23w_if.h"
#elif defined(BSP_MCU_RX64M)
#include "./src/targets/rx64m/r_s12ad_rx64m_if.h"
#elif defined(BSP_MCU_RX65_ALL)
#include "./src/targets/rx65n/r_s12ad_rx65n_if.h"
#elif defined(BSP_MCU_RX66N)
#include "./src/targets/rx66n/r_s12ad_rx66n_if.h"
#elif defined(BSP_MCU_RX66T)
#include "./src/targets/rx66t/r_s12ad_rx66t_if.h"
#elif defined(BSP_MCU_RX71M)
#include "./src/targets/rx71m/r_s12ad_rx71m_if.h"
#elif defined(BSP_MCU_RX72T)
#include "./src/targets/rx72t/r_s12ad_rx72t_if.h"
#elif defined(BSP_MCU_RX72M)
#include "./src/targets/rx72m/r_s12ad_rx72m_if.h"
#elif defined(BSP_MCU_RX72N)
#include "./src/targets/rx72n/r_s12ad_rx72n_if.h"
#endif

/******************************************************************************
Macro definitions
*******************************************************************************/
/* Version Number of API. */
#define ADC_VERSION_MAJOR       (4)
#define ADC_VERSION_MINOR       (50)

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
    ADC_ERR_CONFIGURABLE_INT,   // vector number of software configurable interrupt B is out of range
    ADC_ERR_UNKNOWN             // unknown hardware error
} adc_err_t;


/* CALLBACK FUNCTION ARGUMENT DEFINITIONS */

typedef enum e_adc_cb_evt           // callback function events
{
    ADC_EVT_SCAN_COMPLETE,          // normal/Group A scan complete
    ADC_EVT_SCAN_COMPLETE_GROUPB,   // Group B scan complete
#if (defined(BSP_MCU_RX65_ALL) || defined(BSP_MCU_RX66T) || defined(BSP_MCU_RX72T) \
    || defined(BSP_MCU_RX72M) || defined(BSP_MCU_RX13T) || defined(BSP_MCU_RX66N) \
    || defined(BSP_MCU_RX72N))
    ADC_EVT_SCAN_COMPLETE_GROUPC,   // Group C scan complete
#endif    
#if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX65_ALL) || defined(BSP_MCU_RX66T) \
    || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72T) || defined(BSP_MCU_RX72M) \
    || defined(BSP_MCU_RX66N) || defined(BSP_MCU_RX72N))
    ADC_EVT_CONDITION_MET,          // chans/sensors met comparator condition
#endif
#if (defined(BSP_MCU_RX65_ALL) || defined(BSP_MCU_RX66T) || defined(BSP_MCU_RX72T) \
    || defined(BSP_MCU_RX72M) || defined(BSP_MCU_RX66N) || defined(BSP_MCU_RX72N))
    ADC_EVT_CONDITION_METB          // chans/sensors met comparator condition
#endif
} adc_cb_evt_t;

typedef struct st_adc_cb_args       // callback arguments
{
    adc_cb_evt_t   event;
#if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX65_ALL) || defined(BSP_MCU_RX66T) \
    || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72T) || defined(BSP_MCU_RX72M) \
    || defined(BSP_MCU_RX66N) || defined(BSP_MCU_RX72N))
    uint32_t       compare_flags;   // valid only for compare event in Window A
#if (defined(BSP_MCU_RX65_ALL) || defined(BSP_MCU_RX66T) || defined(BSP_MCU_RX72T) \
    || defined(BSP_MCU_RX72M)  || defined(BSP_MCU_RX66N) || defined(BSP_MCU_RX72N))
    uint32_t       compare_flagsb;  // valid only for compare event in Window B
#endif
    uint8_t        unit;
#endif
} adc_cb_args_t;


/*****************************************************************************
Public Functions
******************************************************************************/
/******************************************************************************
* Function Name: R_ADC_Open
* Description  : This function applies power to the A/D peripheral, sets the
*                operational mode, trigger sources, interrupt priority, and
*                configurations common to all channels and sensors. If interrupt
*                priority is non-zero, the function takes a callback function
*                pointer for notifying the user at interrupt level whenever a
*                scan has completed.
*
* Arguments    : unit -
*                    Unit number
*                mode-
*                    Operational mode (see enumeration below)
*                p_cfg-
*                    Pointer to configuration structure (see below)
*                p_callback-
*                    Optional pointer to function called from interrupt when
*                    a scan completes
* Return Value : ADC_SUCCESS-
*                    Successful
*                ADC_ERR_AD_LOCKED-
*                    Open() call is in progress elsewhere
*                ADC_ERR_AD_NOT_CLOSED-
*                    Peripheral is still running in another mode; Perform
*                    R_ADC_Close() first
*                ADC_ERR_INVALID_ARG-
*                    mode or element of p_cfg structure has invalid value.
*                ADC_ERR_ILLEGAL_ARG-
*                    an argument is illegal based upon mode
*                ADC_ERR_MISSING_PTR-
*                    p_cfg pointer is FIT_NO_PTR/NULL
*******************************************************************************/
adc_err_t R_ADC_Open(uint8_t const      unit,
                    adc_mode_t const   mode,
                    adc_cfg_t * const  p_cfg,
                    void               (* const p_callback)(void *p_args));

/******************************************************************************
* Function Name: R_ADC_Control
* Description  : This function provides commands for enabling channels and
*                sensors and for runtime operations. These include enabling/
*                disabling trigger sources and interrupts, initiating a
*                software trigger, and checking for scan completion.
*
* NOTE: Enabling a channel or a sensor, or setting the sample state count reg
*       cannot be done while the ADCSR.ADST bit is set (conversion in progress).
*       Because these commands should only be called once during initialization
*       before triggers are enabled, this should not be an issue. Registers
*       with this restriction include ADANSA, ADANSB, ADADS, ADADC, ADSSTR,
*       ADEXICR, and some bits in ADCSR and TSCR.
*       No runtime operational sequence checking of any kind is performed.
*
* Arguments    : unit -
*                    Unit number
*                cmd-
*                    Command to run
*                p_args-
*                    Pointer to optional configuration structure
* Return Value : ADC_SUCCESS-
*                    Successful
*                ADC_ERR_MISSING_PTR-
*                    p_args pointer is FIT_NO_PTR/NULL when required as an argument
*                ADC_ERR_INVALID_ARG-
*                    cmd or element of p_args structure has invalid value.
*                ADC_ERR_ILLEGAL_CMD-
*                    cmd is illegal based upon mode
*                ADC_ERR_SCAN_NOT_DONE-
*                    The requested scan has not completed
*                ADC_ERR_UNKNOWN
*                    Did not receive expected hardware response
*******************************************************************************/
adc_err_t R_ADC_Control(uint8_t const   unit,
                        adc_cmd_t const cmd,
                        void * const    p_args);

/******************************************************************************
* Function Name: R_ADC_Read
* Description  : This function reads conversion results from a single channel,
*                sensor, or the double trigger register.
* Arguments    : unit -
*                    Unit number
*                reg_id-
*                    Id for the register to read (see enum below)
*                p_data-
*                    Pointer to variable to load value into.
* Return Value : ADC_SUCCESS-
*                    Successful
*                ADC_ERR_INVALID_ARG-
*                    reg_id contains an invalid value.
*                ADC_ERR_MISSING _PTR-
*                    p_data is FIT_NO_PTR/NULL
*******************************************************************************/
adc_err_t R_ADC_Read(uint8_t const      unit,
                    adc_reg_t const    reg_id,
                    uint16_t * const   p_data);


/******************************************************************************
* Function Name: R_ADC_ReadAll
* Description  : This function reads conversion results from all potential
*                sources, enabled or not.
* Arguments    : p_all_data-
*                    Pointer to structure to load register values into.
* Return Value : ADC_SUCCESS-
*                    Successful
*                ADC_ERR_MISSING _PTR-
*                    p_data is FIT_NO_PTR/NULL
*******************************************************************************/
adc_err_t R_ADC_ReadAll(adc_data_t * const  p_all_data);

/******************************************************************************
* Function Name: R_ADC_Close
* Description  : This function ends any scan in progress, disables interrupts,
*                and removes power to the A/D peripheral.
* Arguments    : unit - Unit number
* Return Value : ADC_SUCCESS - Successful
*                ADC_ERR_INVALID_ARG - unit contains an invalid value.
*******************************************************************************/
adc_err_t R_ADC_Close(uint8_t const unit);

/*****************************************************************************
* Function Name: R_ADC_GetVersion
* Description  : Returns the version of this module. The version number is
*                encoded such that the top two bytes are the major version
*                number and the bottom two bytes are the minor version number.
* Arguments    : none
* Return Value : version number
******************************************************************************/
uint32_t  R_ADC_GetVersion(void);

                                    
#endif /* S12AD_PRV_IF_H */
