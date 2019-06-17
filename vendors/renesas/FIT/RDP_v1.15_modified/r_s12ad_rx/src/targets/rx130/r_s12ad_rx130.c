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
/***********************************************************************************************************************
* File Name    : r_s12ad_rx130.c
* Description  : Primary source code for 12-bit A/D Converter driver.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           01.03.2016 1.00    Initial Release.
*           01.10.2016 2.20    Define name commoditize for the same meaning but different name define.
*                              Delete parameter check by the enum value.
*                              Initialize ADGSPCR register when mode without Group scan mode.
*                              Comparison parameter change.(adjust the parameter structure to RX65N)
*                              Bug fix for Comparison can not stop after start.
*                              Bug fix for Can not choice both self-diag and dda.
*                              Bug fix for self-diag can set during Single scan mode(double trigger).
*                              adc_enable_s12gbadi() declaration change that same as prototype declaration.
*           03.04.2017 2.30    R_ADC_Control function returns ADC_ERR_INVALID_ARG when set to group B or PGS bit
*                               except for Group scan mode.
*                              Corresponding to the notice when setting the PGS bit in the ADGSPCR register to 1.
*                              Added RX130-512KB support.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Includes board and MCU related header files. */
#include <stdio.h>
#include "platform.h"

#if defined (BSP_MCU_RX130)
#include "r_s12ad_rx_config.h"
#include "r_s12ad_rx_private.h"
#include "r_s12ad_rx_if.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef volatile __evenaccess struct st_s12ad  aregs_t;

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
static adc_err_t adc_check_open_cfg(adc_mode_t  const  mode,
                                    adc_cfg_t * const  p_cfg,
                                    void     (* const  p_callback)(void *p_args));

static adc_err_t adc_check_scan_config(adc_ch_cfg_t *p_config);
#endif

static adc_err_t adc_configure_scan(adc_ch_cfg_t *p_config);
extern void adc_enable_s12adi0(void);
static void adc_enable_s12gbadi(void);
static adc_err_t adc_en_comparator_level0(adc_cmpwin_t  *p_cmpwin);
static uint32_t adc_get_and_clr_cmpi_flags(void);


/* In RAM */
extern adc_ctrl_t g_dcb;

/* In ROM. A/D Data Register pointers */
volatile __evenaccess uint16_t * const  dreg_ptrs[ADC_REG_MAX+1] =
                      { &S12AD.ADDR0,     // channel 0
                        &S12AD.ADDR1,     // channel 1
                        &S12AD.ADDR2,     // channel 2
                        &S12AD.ADDR3,     // channel 3
                        &S12AD.ADDR4,     // channel 4
                        &S12AD.ADDR5,     // channel 5
                        &S12AD.ADDR6,     // channel 6
                        &S12AD.ADDR7,     // channel 7
                        &S12AD.ADDR16,    // channel 16
                        &S12AD.ADDR17,    // channel 17
                        &S12AD.ADDR18,    // channel 18
                        &S12AD.ADDR19,    // channel 19
                        &S12AD.ADDR20,    // channel 20
                        &S12AD.ADDR21,    // channel 21
                        &S12AD.ADDR22,    // channel 22
                        &S12AD.ADDR23,    // channel 23
                        &S12AD.ADDR24,    // channel 24
                        &S12AD.ADDR25,    // channel 25
                        &S12AD.ADDR26,    // channel 26
                        &S12AD.ADDR27,    // channel 27
                        &S12AD.ADDR28,    // channel 28
                        &S12AD.ADDR29,    // channel 29
                        &S12AD.ADDR30,    // channel 30
                        &S12AD.ADDR31,    // channel 31
                        &S12AD.ADTSDR,    // temperature sensor
                        &S12AD.ADOCDR,    // voltage sensor
                        &S12AD.ADDBLDR,   // Data Duplication register
                        &S12AD.ADRD.WORD  // self-diagnosis register
                      };


/* In ROM. Sample State (SST) Register pointers */
// 8-bit register pointers
volatile __evenaccess uint8_t * const  sreg_ptrs[ADC_SST_REG_MAX+1] =
                      { &S12AD.ADSSTR0,     // channel 0
                        &S12AD.ADSSTR1,     // channel 1
                        &S12AD.ADSSTR2,     // channel 2
                        &S12AD.ADSSTR3,
                        &S12AD.ADSSTR4,
                        &S12AD.ADSSTR5,
                        &S12AD.ADSSTR6,
                        &S12AD.ADSSTR7,     // channel 7
                        &S12AD.ADSSTRL,     // channels 16 to 21, 24 to 26
                        &S12AD.ADSSTRT,     // temperature sensor output
                        &S12AD.ADSSTRO      // internal voltage reference
                      };


/******************************************************************************
* Function Name: adc_open
* Description  : This function applies power to the A/D peripheral, sets the
*                operational mode, trigger sources, interrupt priority, and
*                configurations common to all channels and sensors. If interrupt
*                priority is non-zero, the function takes a callback function
*                pointer for notifying the user at interrupt level whenever a
*                scan has completed.
*
* Arguments    : unit -
*                    Ignored for rx130. Pass 0 for this argument.
*                mode -
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
adc_err_t adc_open(uint8_t     const   unit,
                   adc_mode_t  const   mode,
                   adc_cfg_t * const   p_cfg,
                   void     (* const   p_callback)(void *p_args))
{
    aregs_t             *p_regs;
    volatile uint16_t   u16_dummy;  /* Dummy read for "1" change to "0".(read first) */
    volatile uint8_t    u8_dummy;   /* Dummy read for "1" change to "0".(read first) */

#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
    adc_err_t   err;

    if ((p_cfg == NULL) || (p_cfg == FIT_NO_PTR))
    {
        return ADC_ERR_MISSING_PTR;
    }

    err = adc_check_open_cfg(mode, p_cfg, p_callback);
    if (err != ADC_SUCCESS)
    {
        return err;
    }
#endif

    /* ENSURE OPEN() NOT ALREADY IN PROGRESS */
    if (g_dcb.opened == true)
    {
        return ADC_ERR_AD_NOT_CLOSED;
    }
    if (R_BSP_HardwareLock(BSP_LOCK_S12AD) == false)
    {
        return ADC_ERR_AD_LOCKED;
    }

    /* APPLY POWER TO PERIPHERAL */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    MSTP(S12AD) = 0;
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

    p_regs = (aregs_t *)&S12AD;
    p_regs->ADCSR.WORD = p_regs->ADCSR.WORD & 0x0400;
    p_regs->ADEXICR.WORD = 0;  //Don't convert Temp Sensor and Internal Reference

    p_regs->ADANSA0.WORD = 0;
    p_regs->ADANSA1.WORD = 0;
    p_regs->ADANSB0.WORD = 0;
    p_regs->ADANSB1.WORD = 0;

    p_regs->ADADS0.WORD = 0;
    p_regs->ADADS1.WORD = 0;
    
    p_regs->ADADC.BYTE = 0;
    
    p_regs->ADCER.WORD = 0;
    p_regs->ADSTRGR.WORD = 0;

    p_regs->ADSSTR0 = 0x0d;
    p_regs->ADSSTR1 = 0x0d;
    p_regs->ADSSTR2 = 0x0d;
    p_regs->ADSSTR3 = 0x0d;
    p_regs->ADSSTR4 = 0x0d;
    p_regs->ADSSTR5 = 0x0d;
    p_regs->ADSSTR6 = 0x0d;
    p_regs->ADSSTR7 = 0x0d;
    p_regs->ADSSTRL = 0x0d;
    p_regs->ADSSTRT = 0x0d;
    p_regs->ADSSTRO = 0x0d;
    
    p_regs->ADDISCR.BYTE = 0;
    p_regs->ADELCCR.BYTE = 0;
    p_regs->ADGSPCR.WORD = 0;

    p_regs->ADCMPCR.WORD = 0;
    p_regs->ADCMPANSR0.WORD = 0;
    p_regs->ADCMPANSR1.WORD = 0;
    p_regs->ADCMPANSER.BYTE = 0;
    p_regs->ADCMPLR0.WORD = 0;
    p_regs->ADCMPLR1.WORD = 0;

    p_regs->ADCMPLER.BYTE = 0;
    
    p_regs->ADCMPDR0 = 0;
    p_regs->ADCMPDR1 = 0;
    u16_dummy = p_regs->ADCMPSR0.WORD;
    p_regs->ADCMPSR0.WORD = 0;          /* target bit clear by read-modify-write */
    u16_dummy = p_regs->ADCMPSR1.WORD;
    p_regs->ADCMPSR1.WORD = 0;          /* target bit clear by read-modify-write */
    
    u8_dummy = p_regs->ADCMPSER.BYTE;
    p_regs->ADCMPSER.BYTE = 0;          /* target bit clear by read-modify-write */
    p_regs->ADHVREFCNT.BYTE = 0;
    
    p_regs->ADCMPBNSR.BYTE = 0;
    p_regs->ADWINLLB = 0;
    p_regs->ADWINULB = 0;
    u8_dummy = p_regs->ADCMPBSR.BYTE;
    p_regs->ADCMPBSR.BYTE = 0;          /* target bit clear by read-modify-write */
    
    p_regs->ADBUFEN.BYTE = 0;
    p_regs->ADBUFPTR.BYTE = 0;
    
    /* SET MODE RELATED REGISTER FIELDS */
    g_dcb.mode = mode;

    if ((mode == ADC_MODE_SS_MULTI_CH_GROUPED)
      ||(mode == ADC_MODE_SS_MULTI_CH_GROUPED_DBLTRIG_A))
    {
        p_regs->ADCSR.BIT.ADCS = ADC_ADCS_GROUP_SCAN;
    }
    else
    {
        if ((mode == ADC_MODE_CONT_ONE_CH) || (mode == ADC_MODE_CONT_MULTI_CH))
        {
            p_regs->ADCSR.BIT.ADCS = ADC_ADCS_CONT_SCAN;
        } // other modes have ADCS=0
    }

    if ((mode == ADC_MODE_SS_ONE_CH_DBLTRIG)
     || (mode == ADC_MODE_SS_MULTI_CH_GROUPED_DBLTRIG_A))
    {
        p_regs->ADCSR.BIT.DBLE = 1;         // enable double trigger
    }

    /* SET TRIGGER AND INTERRUPT PRIORITY REGISTER FIELDS */
    if (p_cfg->trigger != ADC_TRIG_SOFTWARE)
    {
        p_regs->ADSTRGR.BIT.TRSA = p_cfg->trigger;
    }

    if (p_cfg->trigger == ADC_TRIG_ASYNC_ADTRG)
    {
        p_regs->ADCSR.BIT.EXTRG = 1;        // set ext trigger for async trigger
    }

    if (p_regs->ADCSR.BIT.ADCS == ADC_ADCS_GROUP_SCAN)
    {
        p_regs->ADSTRGR.BIT.TRSB = p_cfg->trigger_groupb;
        IPR(S12AD,GBADI) = p_cfg->priority_groupb;
    }

    IPR(S12AD,S12ADI0) = p_cfg->priority;

    /* SET REGISTER FIELDS FOR REMAINING PARAMETERS */
    p_regs->ADCER.BIT.ADRFMT = (p_cfg->alignment == ADC_ALIGN_LEFT) ? 1 : 0;
    p_regs->ADCER.BIT.ACE = (p_cfg->clearing == ADC_CLEAR_AFTER_READ_ON) ? 1 : 0;
    p_regs->ADADC.BYTE = p_cfg->add_cnt;

    /* SET THE A/D CONVERSION OPERATING SPEED. */
    p_regs->ADHVREFCNT.BIT.ADSLP = 1;             // Set the sleep bit to enter standby mode.
    R_BSP_SoftwareDelay(1, BSP_DELAY_MICROSECS);  // Wait at least 0.2us
    /* Set the A/D conversion speed select bit: 0=High Speed, 1=low current */
    p_regs->ADCSR.BIT.ADHSC = (p_cfg->conv_speed == ADC_CONVERT_CURRENT_LOW) ? 1 : 0;
    R_BSP_SoftwareDelay(5, BSP_DELAY_MICROSECS);  // Wait at least 4.8us
    p_regs->ADHVREFCNT.BIT.ADSLP = 0;             // Clear sleep bit for normal operation
    R_BSP_SoftwareDelay(1, BSP_DELAY_MICROSECS);  // Wait at least 1us

    /* SAVE CALLBACK FUNCTION POINTER */
    g_dcb.callback = p_callback;

    /* MARK DRIVER AS OPENED */
    g_dcb.opened = true;
    R_BSP_HardwareUnlock(BSP_LOCK_S12AD);

    return ADC_SUCCESS;
}


/******************************************************************************
* Function Name: adc_check_open_cfg
* Description  : If Parameter Checking is enabled, this function will simply
*                validate the parameters passed to the adc_open function.
** Arguments    :mode -
*                    Operational mode (see enumeration below)
*                p_cfg-
*                    Pointer to configuration structure (see below)
*                p_callback-
*                    Optional pointer to function called from interrupt when
*                    a scan completes
* Return Value : ADC_SUCCESS-
*                    Successful
*                ADC_ERR_INVALID_ARG-
*                    mode or element of p_cfg structure has invalid value.
*                ADC_ERR_ILLEGAL_ARG-
*                    an argument is illegal based upon mode
*******************************************************************************/
#if (ADC_CFG_PARAM_CHECKING_ENABLE == 1)
static adc_err_t adc_check_open_cfg(adc_mode_t   const  mode,
                                    adc_cfg_t  * const  p_cfg,
                                    void      (* const  p_callback)(void *p_args))
{
    /* Check for valid argument values */
    if (p_cfg->priority > BSP_MCU_IPL_MAX)  // priority is not enum value(can't check by a Compiler)
    {
        return ADC_ERR_INVALID_ARG;
    }

    /* If interrupt driven, must have callback function */
    if ((p_cfg->priority != 0)
     && ((p_callback == NULL) || (p_callback == FIT_NO_FUNC)))
    {
        return ADC_ERR_ILLEGAL_ARG;
    }

    /* In double trigger mode, SW and async triggers not allowed */
    if ((mode == ADC_MODE_SS_ONE_CH_DBLTRIG)
     && ((p_cfg->trigger == ADC_TRIG_SOFTWARE) || (p_cfg->trigger == ADC_TRIG_ASYNC_ADTRG)))
    {
        return ADC_ERR_ILLEGAL_ARG;
    }

    /* Group checking; only synchronous triggers allowed; must be unique */
    if ((mode == ADC_MODE_SS_MULTI_CH_GROUPED) || (mode == ADC_MODE_SS_MULTI_CH_GROUPED_DBLTRIG_A))
    {
        if ((p_cfg->trigger == ADC_TRIG_ASYNC_ADTRG)
         || (p_cfg->trigger_groupb == ADC_TRIG_ASYNC_ADTRG)
         || (p_cfg->trigger == p_cfg->trigger_groupb)
         || (p_cfg->trigger == ADC_TRIG_SOFTWARE)
         || (p_cfg->trigger_groupb == ADC_TRIG_SOFTWARE))
        {
            return ADC_ERR_ILLEGAL_ARG;
        }

        if (p_cfg->priority_groupb > BSP_MCU_IPL_MAX)
        {
            return ADC_ERR_INVALID_ARG;
        }

        if ((p_cfg->priority_groupb != 0)   // Interrupt driven, must have callback function
         && ((p_callback == NULL) || (p_callback == FIT_NO_FUNC)))
        {
            return ADC_ERR_ILLEGAL_ARG;
        }
    }

    return ADC_SUCCESS;
}
#endif


/******************************************************************************
* Function Name: adc_control
* Description  : This function provides commands for enabling channels and
*                sensors and for runtime operations. These include enabling/
*                disabling trigger sources and interrupts, initiating a
*                software trigger, and checking for scan completion.
*
* NOTE: Enabling a channel or a sensor, or setting the sample state count reg
*       cannot be done while the ADCSR.ADST bit is set (conversion in progress).
*       Because these commands should only be called once during initialization
*       before triggers are enabled, this should not be an issue. Registers
*       with this restriction include ADANSA0, ADANSA1, ADANSB0, ADANSB1,
*       ADADC, ADADS0, ADADS1, ADSSTRx, ADEXICR, ADCMPCR, ADCMPANSR0,
*       ADCMPANSR1, ADCMPANSER, ADCMPLR0, ADCMPLR1, ADCMPLER, ADCMPDR0,
*       ADCMPDR1, and some bits in ADCSR, ADCMPCR, ADGSPCR.
*       No runtime operational sequence checking of any kind is performed.
*
* Arguments    : unit -
*                    Ignored for rx130. Pass 0 for this argument.
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
*                ADC_ERR_ILLEGAL_ARG-
*                    argument is illegal based upon rest of configuration
*                ADC_ERR_SCAN_NOT_DONE-
*                    The requested scan has not completed
*                ADC_ERR_TRIG_ENABLED -
*                    Cannot configure comparator because a scan is running
*                ADC_ERR_CONDITION_NOT_MET -
*                    No chans/sensors passed comparator condition
*                ADC_ERR_UNKNOWN
*                    Did not receive expected hardware response
*******************************************************************************/
adc_err_t adc_control(uint8_t   const   unit,
                      adc_cmd_t const   cmd,
                      void *    const   p_args)
{
adc_err_t       err=ADC_SUCCESS;
adc_sst_t       *p_sample;
adc_dda_t       *p_charge;
adc_cmpwin_t    *p_cmpwin;

aregs_t         *p_regs = (aregs_t *)&S12AD;
uint32_t        flags;

    /* DO UNIVERSAL PARAMETER CHECKING */

#if (ADC_CFG_PARAM_CHECKING_ENABLE == 1)

    if ((p_args == NULL) || (p_args == FIT_NO_PTR))
    {
        if ((cmd == ADC_CMD_SET_DDA_STATE_CNT)
         || (cmd == ADC_CMD_SET_SAMPLE_STATE_CNT)
         || (cmd == ADC_CMD_ENABLE_CHANS)
         || (cmd == ADC_CMD_EN_COMPARATOR_LEVEL)
         || (cmd == ADC_CMD_EN_COMPARATOR_WINDOW)
         || (cmd == ADC_CMD_CHECK_CONDITION_MET))
        {
            return ADC_ERR_MISSING_PTR;
        }
    }
#endif

    /* PROCESS INDIVIDUAL COMMANDS */

    switch (cmd)
    {
    case ADC_CMD_USE_VREFH0:
        S12AD.ADHVREFCNT.BIT.HVSEL = 1;
    break;

    case ADC_CMD_USE_VREFL0:
        S12AD.ADHVREFCNT.BIT.LVSEL = 1;
    break;

    /* Set up Disconnect Detection Assist*/
    case ADC_CMD_SET_DDA_STATE_CNT:
        p_charge = (adc_dda_t *)p_args;            //Get the command arguments
#if (ADC_CFG_PARAM_CHECKING_ENABLE == 1)
        if (p_charge->method != ADC_DDA_OFF)
        {
            /* Cannot have temp sensor output or V-reference conversion enabled and Disconnect Detection enabled. */
            if ((p_charge->num_states < ADC_DDA_STATE_CNT_MIN)
             || (p_charge->num_states > ADC_DDA_STATE_CNT_MAX)
             || ((p_regs->ADEXICR.WORD & 0x0300) != 0)) // Temp sensor or V-ref enabled
            {
                return ADC_ERR_ILLEGAL_ARG;
            }
        }
#endif
        if (p_charge->method == ADC_DDA_OFF)
        {
            p_regs->ADDISCR.BYTE = 0;              //Disable Disconnect Detection Assist
        }
        else
        {
            // NOTE: Using Disconnect Detection Assist adds num_states x (#chans) ADCLKS to scan time
            p_regs->ADDISCR.BYTE = (uint8_t)((p_charge->method == ADC_DDA_PRECHARGE) ? 0x10 : 0);
            p_regs->ADDISCR.BYTE |= p_charge->num_states;
        }
    break;

    case ADC_CMD_SET_SAMPLE_STATE_CNT:
        p_sample = (adc_sst_t *)p_args;            //Get the command arguments
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
        if (p_sample->num_states < ADC_SST_CNT_MIN)
        {
            return ADC_ERR_INVALID_ARG;
        }
#endif
        *(sreg_ptrs)[p_sample->reg_id] = p_sample->num_states;
    break;

    case ADC_CMD_ENABLE_CHANS:
        err = adc_configure_scan((adc_ch_cfg_t *)p_args);
    break;

    case ADC_CMD_EN_COMPARATOR_LEVEL:
        p_cmpwin = (adc_cmpwin_t *)p_args;
        p_regs->ADCMPCR.BIT.WCMPE = 0;          // Disable the window function. Compare level

        err = adc_en_comparator_level0(p_cmpwin);
    break;


    case ADC_CMD_ENABLE_TEMP_SENSOR:
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
        if (g_dcb.mode != ADC_MODE_SS_TEMPERATURE)
        {
            return ADC_ERR_ILLEGAL_ARG;
        }
#endif
        S12AD.ADEXICR.BIT.TSSA = 1;              // select temperature output on Single scan mode
        if (S12AD.ADADC.BIT.ADC != ADC_ADD_OFF)
        {
            S12AD.ADEXICR.BIT.TSSAD = 1;         // enable addition
        }
        adc_enable_s12adi0();                    // setup interrupt handling
    break;


    case ADC_CMD_ENABLE_VOLT_SENSOR:
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
        if (g_dcb.mode != ADC_MODE_SS_INT_REF_VOLT)
        {
            return ADC_ERR_ILLEGAL_ARG;
        }
#endif
        S12AD.ADEXICR.BIT.OCSA = 1;              // select internal reference voltage output on Single scan mode
        if (S12AD.ADADC.BIT.ADC != ADC_ADD_OFF)
        {
            S12AD.ADEXICR.BIT.OCSAD = 1;         // enable addition
        }
        adc_enable_s12adi0();                    // setup interrupt handling
    break;


    case ADC_CMD_EN_COMPARATOR_WINDOW:
        p_cmpwin = (adc_cmpwin_t *)p_args;
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
        if (p_cmpwin->level_lo > p_cmpwin->level_hi)
        {
            return ADC_ERR_ILLEGAL_ARG;
        }
#endif
        p_regs->ADCMPCR.BIT.WCMPE = 1;           // Enable window function

        err = adc_en_comparator_level0(p_cmpwin);
    break;

    /* Enable A/D conversion to be started by synchronous or asynchronous trigger */
    case ADC_CMD_ENABLE_TRIG:
        p_regs->ADCSR.BIT.TRGE = 1;           // enable sync/async triggers
    break;

    /* Disable A/D conversion to be started by synchronous or asynchronous trigger */
    case ADC_CMD_DISABLE_TRIG:
        p_regs->ADCSR.BIT.TRGE = 0;           // disable sync/async triggers
    break;

    /* Start the A/D conversion process */
    case ADC_CMD_SCAN_NOW:
        if (p_regs->ADCSR.BIT.ADST == 0)
        {
            p_regs->ADCSR.BIT.ADST = 1;
        }
        else
        {
            err = ADC_ERR_SCAN_NOT_DONE;
        }
    break;

    /* Check for completion of the current scan. */
    case ADC_CMD_CHECK_SCAN_DONE:
        if (p_regs->ADCSR.BIT.ADST == 1)
        {
            err = ADC_ERR_SCAN_NOT_DONE;
        }
    break;

    /* Check for completion of the current scan. */
    case ADC_CMD_CHECK_SCAN_DONE_GROUPA:
        if (IR(S12AD,S12ADI0) == 1)
        {
            IR(S12AD,S12ADI0) = 0;
        }
        else
        {
            err = ADC_ERR_SCAN_NOT_DONE;
        }
    break;

    /* Check for completion of the current groupB scan. */
    case ADC_CMD_CHECK_SCAN_DONE_GROUPB:
        if (IR(S12AD,GBADI) == 1)
        {
            IR(S12AD,GBADI) = 0;
        }
        else
        {
            err = ADC_ERR_SCAN_NOT_DONE;
        }
    break;

    case ADC_CMD_CHECK_CONDITION_MET:
        flags = adc_get_and_clr_cmpi_flags();
        if (flags == 0)
        {
            err = ADC_ERR_CONDITION_NOT_MET;
        }
        *((uint32_t *)p_args) = flags;
    break;

    case ADC_CMD_ENABLE_INT:
        p_regs->ADCSR.BIT.ADIE = 1;    // Enable Scan End Interrupt (S12ADI0)
    break;

    case ADC_CMD_DISABLE_INT:
        p_regs->ADCSR.BIT.ADIE = 0;    // Disable Scan End Interrupt (S12ADI0)
    break;

    case ADC_CMD_ENABLE_INT_GROUPB:
        p_regs->ADCSR.BIT.GBADIE = 1;  // Enable Group B Scan End Interrupt (GBADI)
    break;

    case ADC_CMD_DISABLE_INT_GROUPB:
        p_regs->ADCSR.BIT.GBADIE = 0;  // Disable Group B Scan End Interrupt (GBADI)
    break;

    default:
        err = ADC_ERR_INVALID_ARG;
    break;
    }

    return err;
}

/******************************************************************************
* Function Name: adc_en_comparator_level0
* Description  : This function sets up the comparator for level0 threshold
*              : compare. The processing is identical for window except
*              : for the setting of the window enable bit and the level1
*              : value (ADCMPDR1).
*              :
* Arguments    : p_cmpwin -
*              :     Pointer to comparator level/window compare structure
*              :
* Return Value :ADC_SUCCESS-
*              :     Successful
*              : ADC_ERR_INVALID_ARG-
*              :     reg_id contains an invalid value.
*              : ADC_ERR_ILLEGAL_ARG-
*              :     an argument is illegal based upon rest of configuration
*              : ADC_ERR_TRIG_ENABLED -
*              :     Cannot configure comparator while scans are running
*              :
*******************************************************************************/
static adc_err_t adc_en_comparator_level0(adc_cmpwin_t  *p_cmpwin)
{
    aregs_t *p_regs = (aregs_t *)&S12AD;

#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
    if ((p_cmpwin->compare_mask & ADC_INVALID_CH_MASK) != 0)    // checking exist channel
    {
        return ADC_ERR_INVALID_ARG;
    }
    if ((p_cmpwin->inside_window_mask | p_cmpwin->compare_mask) != p_cmpwin->compare_mask)  // checking compare channel combination
    {
        return ADC_ERR_INVALID_ARG;
    }
    if (p_regs->ADCSR.BIT.DBLE == 1)                            // Comparison disable when Double trigger mode
    {
        return ADC_ERR_INVALID_ARG;
    }
    if (p_regs->ADCER.BIT.DIAGM != 0)                           // Comparison disable when Self-diagnosis mode
    {
        return ADC_ERR_INVALID_ARG;
    }
    if ((g_dcb.mode == ADC_MODE_SS_TEMPERATURE) && (p_cmpwin->compare_mask != ADC_MASK_TEMP))   // checking use channel
    {
        return ADC_ERR_INVALID_ARG;
    }
    if ((g_dcb.mode == ADC_MODE_SS_INT_REF_VOLT) && (p_cmpwin->compare_mask != ADC_MASK_VOLT))  // checking use channel
    {
        return ADC_ERR_INVALID_ARG;
    }
#endif

    if (p_regs->ADCSR.BIT.TRGE == 1)
    {
        return ADC_ERR_TRIG_ENABLED;    // ADST must be 0 to configure comparator
    }


    /* CONFIGURE COMPARATOR */

    if (true == p_cmpwin->windowa_enable) // COMPARATOR A
    {
        p_regs->ADCMPCR.BIT.CMPAE = 0;          // Compare window A operation is disabled

        p_regs->ADCMPDR0 = p_cmpwin->level_lo;      // set lo end of window or set level
        if (p_regs->ADCMPCR.BIT.WCMPE == 1)
        {
            p_regs->ADCMPDR1 = p_cmpwin->level_hi;  // set hi end of window
        }

        p_regs->ADCMPANSR0.WORD = p_cmpwin->compare_mask & 0x00FF;      // Select channels to compare.  Channels 0 to 7.
        p_regs->ADCMPLR0.WORD = p_cmpwin->inside_window_mask & 0x00FF;  // Select the CoMPare Level. Channels 0 to 7.

        S12AD.ADCMPANSR1.WORD = (p_cmpwin->compare_mask>>16) & 0xFFFF;  // Select channels to compare. 16 to 31.
        S12AD.ADCMPLR1.WORD = (p_cmpwin->inside_window_mask>>16) & 0xFFFF;

        S12AD.ADCMPANSER.BIT.CMPTSA = (p_cmpwin->compare_mask & ADC_MASK_TEMP) ? 1 : 0;
        S12AD.ADCMPANSER.BIT.CMPOCA = (p_cmpwin->compare_mask & ADC_MASK_VOLT) ? 1 : 0;

        S12AD.ADCMPLER.BIT.CMPLTSA = (p_cmpwin->inside_window_mask & ADC_MASK_TEMP) ? 1 : 0;
        S12AD.ADCMPLER.BIT.CMPLOCA = (p_cmpwin->inside_window_mask & ADC_MASK_VOLT) ? 1 : 0;

        p_regs->ADCMPCR.BIT.CMPAE = 1;          // Compare window A operation is enabled
    }
    else
    {
        p_regs->ADCMPCR.BIT.CMPAE = 0;          // Compare window A operation is disabled
    }

    return ADC_SUCCESS;
}

/*****************************************************************************
* Function Name: adc_get_and_clr_cmpi_flags
* Description  : Returns flags for all channels/sensors that met comparator
*                condition and clears the flags for next interrupt.
* Arguments    : none
* Return Value : channels/sensors that met comparator condition
******************************************************************************/
static uint32_t adc_get_and_clr_cmpi_flags(void)
{
uint32_t    flags;

    /* Get flags for channels AN00 to AN07 */
    flags = (uint32_t)S12AD.ADCMPSR0.WORD;
    S12AD.ADCMPSR0.WORD = 0;

    /* Get flags for channels AN16 to AN31 */
    flags |= (uint32_t)S12AD.ADCMPSR1.WORD << 16;
    S12AD.ADCMPSR1.WORD = 0;

    /* Get flags for Temperature sensor and internal Voltage Reference */
    flags |= (S12AD.ADCMPSER.BIT.CMPSTTSA == 1) ? ADC_MASK_TEMP : 0;
    flags |= (S12AD.ADCMPSER.BIT.CMPSTOCA == 1) ? ADC_MASK_VOLT : 0;
    S12AD.ADCMPSER.BYTE = 0;

    return flags;
}

/******************************************************************************
* Function Name: adc_configure_scan
* Description  : This function does extensive checking on channel mask
*                settings based upon operational mode. Mask registers are
*                initialized and interrupts enabled in peripheral. Interrupts
*                are also enabled in ICU if corresponding priority is not 0.
*
* Arguments    : p_config -
*                    Pointer to channel config structure containing masks
* Return Value : ADC_SUCCESS-
*                    Successful
*******************************************************************************/
static adc_err_t adc_configure_scan(adc_ch_cfg_t   *p_config)
{
uint16_t  i=0;
uint32_t  tmp_mask=0;
aregs_t   *p_regs = (aregs_t *)&S12AD;
uint16_t  trig_a;
uint16_t  trig_b;

#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
    adc_err_t   err;

    err = adc_check_scan_config(p_config);
    if (err != ADC_SUCCESS)
    {
        return err;
    }
#endif // parameter checking

    /* SET MASKS FOR CHANNELS AND SENSORS */
    /* Select the channels from among Ch0 to Ch7 for conversion */
    p_regs->ADANSA0.WORD = (uint16_t) (p_config->chan_mask & 0x00FF);            //CH0 to CH7
    p_regs->ADANSB0.WORD = (uint16_t) (p_config->chan_mask_groupb & 0x00FF);     //CH0 to CH7

    /* Select the channels for Addition/Averaging (ch0 to ch7) */
    p_regs->ADADS0.WORD  = (uint16_t) (p_config->add_mask & 0x00FF);             //CH0 to CH7

    /* Select the channels from among Ch16 to Ch31 for conversion */
    S12AD.ADANSA1.WORD = (uint16_t) ((p_config->chan_mask >> 16) & 0xFFFFF);      //CH16 to CH31
    S12AD.ADANSB1.WORD = (uint16_t) ((p_config->chan_mask_groupb >> 16) & 0xFFFF);

    /* Select the channels for Addition/Averaging (Ch16 to Ch31) */
    S12AD.ADADS1.WORD  = (uint16_t) ((p_config->add_mask >> 16) & 0xFFFF);

    /* SET GROUP A PRIORITY ACTION (not interrupt priority!) */

    if (ADC_GRPA_PRIORITY_OFF != p_config->priority_groupa)
    {
        trig_a = p_regs->ADSTRGR.BIT.TRSA;              // save TRSA
        trig_b = p_regs->ADSTRGR.BIT.TRSB;              // save TRSB
        p_regs->ADSTRGR.BIT.TRSA = ADC_TRIG_NONE;
        p_regs->ADSTRGR.BIT.TRSB = ADC_TRIG_NONE;
        if (ADC_TRIG_NONE != p_regs->ADSTRGR.BIT.TRSA)  // bus wait for ADSTRGR write
        {
            nop();
        }
        if (ADC_TRIG_NONE != p_regs->ADSTRGR.BIT.TRSB)  // bus wait for ADSTRGR write
        {
            nop();
        }

        p_regs->ADGSPCR.WORD = p_config->priority_groupa;

        p_regs->ADSTRGR.BIT.TRSA = trig_a;             // restore TRSA
        if (1 != p_regs->ADGSPCR.BIT.GBRP)
        {
            p_regs->ADSTRGR.BIT.TRSB = trig_b;         // restore TRSB
        }
    }
    else
    {
        p_regs->ADGSPCR.WORD = p_config->priority_groupa;
    }

    /* SET SELF DIAGNOSIS REGISTERS (VIRTUAL CHANNEL) */

    if (p_config->diag_method == ADC_DIAG_OFF)
    {
        p_regs->ADCER.BIT.DIAGM = 0;
    }
    else
    {
        // NOTE: Using Self Diagnosis adds 15(resolution) + (ch0 SST) ADCLKS to scan time.
        // (ch0 can still be used with self diagnosis on)
        if (p_config->diag_method == ADC_DIAG_ROTATE_VOLTS)
        {
            p_regs->ADCER.BIT.DIAGLD = 0;    //Rotate test voltages
            p_regs->ADCER.BIT.DIAGVAL = 1;   //Start with 0 V
        }
        else
        {
            /* Make sure DIAGVAL bits are not zero before setting the DIAGLD bit */
            p_regs->ADCER.BIT.DIAGVAL = (uint8_t)(p_config->diag_method & 0x3);   //Select diag. voltage
            p_regs->ADCER.BIT.DIAGLD = 1;    //Do not rotate test voltages
        }
        p_regs->ADCER.BIT.DIAGM = 1;         //Enable self diagnosis
    }

    /* SET DOUBLE TRIGGER CHANNEL */

    if (p_regs->ADCSR.BIT.DBLE == 1)         // If double-trigger mode is already selected.
    {
        tmp_mask = p_config->chan_mask;      // tmp_mask is non-Group/Group A chans
        while (tmp_mask >>= 1)               // determine bit/ch number
        {
            i++;
        }
        p_regs->ADCSR.BIT.DBLANS = i;
    }

    p_regs->ADELCCR.BIT.ELCC = p_config->signal_elc;

    /* ENABLE INTERRUPTS */
    adc_enable_s12adi0();
    if (p_regs->ADCSR.BIT.ADCS == ADC_ADCS_GROUP_SCAN)
    {
        adc_enable_s12gbadi();
    }

    return ADC_SUCCESS;
}


/******************************************************************************
* Function Name: adc_check_scan_config
* Description  : This function does extensive checking on channel mask
*                settings based upon operational mode.
*
* NOTE: A negative number is stored in two's complement form.
*       A quick way to change a binary number into two's complement is to
*       start at the right (LSB) and moving left, don't change any bits
*       until after the first "1" is reached.
*       Number          2's complement
*       0010 0110       1101 1010
*       Another way is to do a 1's complement on the number, then add 1 to that.
*       Number          1's complement  + 1
*       0010 0110       1101 1001       1101 1010
*
* Arguments    : p_config
*                    Pointer to channel config structure containing masks
* Return Value : ADC_SUCCESS-
*                    Successful
*                ADC_ERR_INVALID_ARG-
*                    reg_id contains an invalid value.
*                ADC_ERR_ILLEGAL_ARG-
*                    an argument is illegal based upon rest of configuration
*******************************************************************************/
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
static adc_err_t adc_check_scan_config(adc_ch_cfg_t   *p_config)
{
uint32_t    tmp_mask=0;
aregs_t     *p_regs = (aregs_t *)&S12AD;

    /* Verify at least one bonded channel is selected */
    if ((p_config->chan_mask == 0)
     || ((p_config->chan_mask & ADC_INVALID_CH_MASK) != 0)
     || ((g_dcb.mode == ADC_MODE_SS_TEMPERATURE) && ( p_config->chan_mask != ADC_MASK_TEMP))
     || ((g_dcb.mode == ADC_MODE_SS_INT_REF_VOLT) && ( p_config->chan_mask != ADC_MASK_VOLT)))
    {
        return ADC_ERR_INVALID_ARG;
    }

    /* Verify at least one unique bonded channel is selected for Group B
     * and GroupA priority setting is valid.
     */
    if (p_regs->ADCSR.BIT.ADCS == ADC_ADCS_GROUP_SCAN)
    {
        if ((p_config->chan_mask_groupb == 0)
         || ((p_config->chan_mask_groupb & ADC_INVALID_CH_MASK) != 0)
         || ((g_dcb.mode == ADC_MODE_SS_TEMPERATURE) && ( p_config->chan_mask_groupb != ADC_MASK_TEMP))
         || ((g_dcb.mode == ADC_MODE_SS_INT_REF_VOLT) && ( p_config->chan_mask_groupb != ADC_MASK_VOLT)))
        {
            return ADC_ERR_INVALID_ARG;
        }
        else if ((p_config->chan_mask & p_config->chan_mask_groupb) != 0)
        {
            return ADC_ERR_ILLEGAL_ARG;         // same chan in both groups
        }
        else
        {
            tmp_mask = p_config->chan_mask_groupb;
        }
    }
    else
    {
        /* for addition mask checking */
        if (0 != p_config->chan_mask_groupb)
        {
            return ADC_ERR_INVALID_ARG;
        }

        /* PGS bit must be set when group scan mode */
        if (ADC_GRPA_PRIORITY_OFF != p_config->priority_groupa)
        {
            return ADC_ERR_INVALID_ARG;
        }
    }

    /* If sensors specified, verify in legal configuation */
    if ((g_dcb.mode == ADC_MODE_SS_TEMPERATURE) || (g_dcb.mode == ADC_MODE_SS_INT_REF_VOLT))
    {
        if ((S12AD.ADCSR.BIT.DBLE == 1)       // Select double trigger mode
         || (S12AD.ADDISCR.BYTE != 0))        // disconnect detection assist enabled
        {
            return ADC_ERR_ILLEGAL_ARG;
        }
    }

    /* Addition mask should not include bits from inactive channels */
    if (p_regs->ADADC.BIT.ADC != ADC_ADD_OFF)
    {
        tmp_mask |= p_config->chan_mask;        // tmp_mask is Group A and B combined
        // Bit-AND with 1s-complement
        if ((p_config->add_mask & ~tmp_mask) != 0)
        {
            return ADC_ERR_INVALID_ARG;
        }
    }
    else
    {
        /* WARNING! Other features messed up if add_mask is non-zero when addition is turned off! */
        p_config->add_mask = 0;
    }

    /* Verify only 1 bit is set in default/Group A mask */
    if ((g_dcb.mode == ADC_MODE_SS_ONE_CH)
     || (g_dcb.mode == ADC_MODE_CONT_ONE_CH)
     || (p_regs->ADCSR.BIT.DBLE == 1))          // double trigger mode
    {
        tmp_mask = p_config->chan_mask;         // tmp_mask is non-Group/Group A chans
        /* Bit-AND with 2s-complement (see note in function header) */
        if ((tmp_mask & (~tmp_mask + 1)) != tmp_mask)
        {
            return ADC_ERR_ILLEGAL_ARG;
        }
    }

    if ((p_config->diag_method != ADC_DIAG_OFF)
        && ((p_regs->ADADC.BIT.ADC != 0)           // addition
        ||  (p_regs->ADCSR.BIT.DBLE == 1)))        // double trigger mode
    {
        return ADC_ERR_ILLEGAL_ARG;
    }

    return ADC_SUCCESS;
}
#endif


/******************************************************************************
* Function Name: adc_enable_s12gbadi
* Description  : This function clears the S12GBADI interrupt flag and enables
*                interrupts in the peripheral (for IR flag usage). If priority
*                is not 0, interrupts are enabled in the ICU.
*                NOTE: This has nothing to do with enabling triggers.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void adc_enable_s12gbadi(void)
{
    IR(S12AD,GBADI) = 0;            // clear flag
    S12AD.ADCSR.BIT.GBADIE = 1;     // enable in peripheral
    if (IPR(S12AD,GBADI) != 0)
    {
        IEN(S12AD,GBADI) = 1;       // enable in ICU
    }
}


/******************************************************************************
* Function Name: adc_close
* Description  : This is the RX130 implementation of R_ADC_Close().
*                This function ends any scan in progress, disables interrupts,
*                and removes power to the A/D peripheral.
* Arguments    : unit - This argument is ignored for the RX130.
* Return Value : ADC_SUCCESS
*******************************************************************************/
adc_err_t adc_close(uint8_t const  unit)
{
    volatile uint8_t i;
    aregs_t     *p_regs = (aregs_t *)&S12AD;

    /* Stop triggers & conversions, and disable peripheral interrupts */
    IEN(S12AD,S12ADI0) = 0;     // disable interrupts in ICU
    IR(S12AD,S12ADI0) = 0;      // clear interrupt flag
    IEN(S12AD,GBADI) = 0;       // disable interrupts in ICU
    IR(S12AD,GBADI) = 0;        // clear interrupt flag

    p_regs->ADGSPCR.BIT.PGS = 0;
    if (ADC_ADCS_GROUP_SCAN == p_regs->ADCSR.BIT.ADCS)
    {
        p_regs->ADSTRGR.WORD = 0x3F3F;
    }
    else
    {
        p_regs->ADSTRGR.BIT.TRSA = 0x3F;
    }
    
    p_regs->ADCSR.WORD = p_regs->ADCSR.WORD & 0x0400;
    if (0x0000 == (p_regs->ADCSR.WORD & 0xFBFF))   // dummy read for waiting until set the value of ADCSR
    {
        nop();
    }
    
    /* Wait for 2 ADCLK cycles (MAX: 128 ICLK cycles) */
    for (i = 0; i < 128; i++)
    {
        nop();
    }
    
    /* Power down peripheral */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    MSTP(S12AD) = 1;
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

    /* Show driver as closed */

    g_dcb.opened = false;

    return ADC_SUCCESS;
}


/******************************************************************************
* Function Name: adc_read_all
* Description  : This is the RX130 implementation of R_ADC_ReadAll().
*                It reads conversion results from all potential sources,
*                enabled or not.
* Arguments    : p_all_data-
*                    Pointer to structure to load register values into.
* Return Value : ADC_SUCCESS-
*                    Successful
*******************************************************************************/
adc_err_t adc_read_all(adc_data_t * const  p_all_data)
{
    p_all_data->chan[ADC_REG_CH0]  = S12AD.ADDR0;
    p_all_data->chan[ADC_REG_CH1]  = S12AD.ADDR1;
    p_all_data->chan[ADC_REG_CH2]  = S12AD.ADDR2;
    p_all_data->chan[ADC_REG_CH3]  = S12AD.ADDR3;
    p_all_data->chan[ADC_REG_CH4]  = S12AD.ADDR4;
    p_all_data->chan[ADC_REG_CH5]  = S12AD.ADDR5;
    p_all_data->chan[ADC_REG_CH6]  = S12AD.ADDR6;
    p_all_data->chan[ADC_REG_CH7]  = S12AD.ADDR7;

    p_all_data->chan[ADC_REG_CH16] = S12AD.ADDR16;
    p_all_data->chan[ADC_REG_CH17] = S12AD.ADDR17;
    p_all_data->chan[ADC_REG_CH18] = S12AD.ADDR18;
    p_all_data->chan[ADC_REG_CH19] = S12AD.ADDR19;
    p_all_data->chan[ADC_REG_CH20] = S12AD.ADDR20;
    p_all_data->chan[ADC_REG_CH21] = S12AD.ADDR21;
    p_all_data->chan[ADC_REG_CH22] = S12AD.ADDR22;
    p_all_data->chan[ADC_REG_CH23] = S12AD.ADDR23;
    p_all_data->chan[ADC_REG_CH24] = S12AD.ADDR24;
    p_all_data->chan[ADC_REG_CH25] = S12AD.ADDR25;
    p_all_data->chan[ADC_REG_CH26] = S12AD.ADDR26;
    p_all_data->chan[ADC_REG_CH27] = S12AD.ADDR27;
    p_all_data->chan[ADC_REG_CH28] = S12AD.ADDR28;
    p_all_data->chan[ADC_REG_CH29] = S12AD.ADDR29;
    p_all_data->chan[ADC_REG_CH30] = S12AD.ADDR30;
    p_all_data->chan[ADC_REG_CH31] = S12AD.ADDR31;

    p_all_data->temp               = S12AD.ADTSDR;
    p_all_data->volt               = S12AD.ADOCDR;

    p_all_data->dbltrig            = S12AD.ADDBLDR;
    p_all_data->self_diag          = S12AD.ADRD.WORD;

    return ADC_SUCCESS;
}


#endif // #if defined (BSP_MCU_RX130)
