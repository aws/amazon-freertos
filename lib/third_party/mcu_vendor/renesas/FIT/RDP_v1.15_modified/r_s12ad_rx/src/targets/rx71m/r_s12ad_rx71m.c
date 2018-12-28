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
* Copyright (C) 2014-2017 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_s12ad_rx71m.c
* Description  : Primary source code for 12-bit A/D Converter driver.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*           13.06.2014 1.00    Initial Release.
*           01.10.2016 2.20    Bug fix for ADC_TRIG_NONE set to parameter when called R_ADC_Open then error occured.
*                              Bug fix for R_ADC_Open(temp sensor choice) error occured after use self-diag method.
*                              Bug fix for Can not change to other ADIN after self-diag setting.
*                              Bug fix for Can not set under limit voltage == upper limit voltage on Compare function.
*                              Corresponding to the notice for A/D convert stop.
*                              Corresponding to the notice for shift to Low power mode.
*                              Corresponding to the Procedure for Using the Temperature Sensor.
*                              Bug fix for Mask value for legal channel checking when ANEX enabled.
*                              Bug fix for parameter range check.
*                              Define name commoditize for the same meaning but different name define.
*                              Delete parameter check by the enum value.
*                              Initialize ADGSPCR register when mode without Group scan mode.
*                              Comparison parameter change.(adjust the parameter structure to RX65N)
*                              Bug fix for Comparison can not stop after start.
*                              Bug fix for callback function execute when pointer is NULL.
*                              Modified the register that was initialized when obtaining the compare match result.
*                              Bug fix for self-diag can set during Single scan mode(double trigger).
*                              adc_enable_s12adi() declaration change that same as prototype declaration.
*                              adc_enable_s12gbadi() declaration change that same as prototype declaration.
*           03.04.2017 2.30    R_ADC_Control function returns ADC_ERR_INVALID_ARG when set to group B or PGS bit
*                               except for Group scan mode.
*                              Corresponding to the notice when setting the PGS bit in the ADGSPCR register to 1.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Includes board and MCU related header files. */
#include <stdio.h>
#include "platform.h"

#if defined (BSP_MCU_RX71M)
#include "r_s12ad_rx_config.h"
#include "r_s12ad_rx_private.h"
#include "r_s12ad_rx_if.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Unit1 registers are a superset of Unit0 EXCEPT that they do not contain the S&H registers */
typedef volatile __evenaccess struct st_s12ad1  aregs_t;

#define GET_REGS_PTR(x)         ((x==0) ? (aregs_t *)&S12AD : &S12AD1)

#define GET_SST_ARR(x)          ((x==0) ? sreg0_ptrs : sreg1_ptrs)

#define ADC_LOCK(x)             ((x==0) ? BSP_LOCK_S12AD : BSP_LOCK_S12AD1)
#define ADC_INVALID_CH_MASK(x)  ((x==0) ? ADC_INVALID_CH_MASK0 : ADC_INVALID_CH_MASK1)

#define GRPBL1_S12CMPI0_FLAG    (1<<20)
#define GRPBL1_S12CMPI1_FLAG    (1<<22)

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
static adc_err_t adc_check_open_cfg(uint8_t                unit,
                                    adc_mode_t const       mode,
                                    adc_cfg_t * const      p_cfg,
                                    void         (* const  p_callback)(void *p_args));

static adc_err_t adc_check_scan_config(uint8_t const unit,  adc_ch_cfg_t *p_config);
#endif

static adc_err_t adc_configure_scan(uint8_t const unit,  adc_ch_cfg_t *p_config);
static void adc_configure_sensors(adc_ch_cfg_t  *p_config);
static void adc_enable_s12adi(uint8_t unit);
static void adc_enable_s12gbadi(uint8_t unit);
void adc_s12cmpi0_isr(void *cb_args);
void adc_s12cmpi1_isr(void *cb_args);
static adc_err_t adc_en_comparator_level0(uint8_t unit, adc_cmpwin_t  *p_cmpwin);
static uint32_t adc_get_and_clr_cmpi_flags(uint8_t unit);


/* In RAM */

adc_ctrl_t g_dcb[2] = {{ADC_MODE_MAX,
                        NULL,
                        false,
                        GRPBL1_S12CMPI0_FLAG},

                       {ADC_MODE_MAX,
                        NULL,
                        false,
                        GRPBL1_S12CMPI1_FLAG}};

/* In ROM. A/D Data Register pointers */

volatile __evenaccess uint16_t * const  dreg0_ptrs[ADC_REG_MAX+1] =
                      { &S12AD.ADDR0,       // channel 0
                        &S12AD.ADDR1,       // channel 1
                        &S12AD.ADDR2,       // channel 2
                        &S12AD.ADDR3,
                        &S12AD.ADDR4,
                        &S12AD.ADDR5,
                        &S12AD.ADDR6,
                        &S12AD.ADDR7,
                        NULL,
                        NULL,
                        NULL,               // channel 10
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL,               // channel 20
                        NULL,               // sensor
                        NULL,
                        &S12AD.ADDBLDR.WORD, // double trigger register
                        &S12AD.ADDBLDRA,    // double trigger registerA
                        &S12AD.ADDBLDRB,    // double trigger registerB
                        &S12AD.ADRD.WORD    // self-diagnosis register
                      };

volatile __evenaccess uint16_t * const  dreg1_ptrs[ADC_REG_MAX+1] =
                      { &S12AD1.ADDR0,       // channel 0
                        &S12AD1.ADDR1,       // channel 1
                        &S12AD1.ADDR2,       // channel 2
                        &S12AD1.ADDR3,
                        &S12AD1.ADDR4,
                        &S12AD1.ADDR5,
                        &S12AD1.ADDR6,
                        &S12AD1.ADDR7,
                        &S12AD1.ADDR8,
                        &S12AD1.ADDR9,
                        &S12AD1.ADDR10,
                        &S12AD1.ADDR11,
                        &S12AD1.ADDR12,
                        &S12AD1.ADDR13,
                        &S12AD1.ADDR14,
                        &S12AD1.ADDR15,
                        &S12AD1.ADDR16,
                        &S12AD1.ADDR17,
                        &S12AD1.ADDR18,
                        &S12AD1.ADDR19,
                        &S12AD1.ADDR20,
                        &S12AD1.ADTSDR,    // temperature sensor
                        &S12AD1.ADOCDR,    // voltage sensor
                        &S12AD1.ADDBLDR,   // double trigger register
                        &S12AD1.ADDBLDRA,  // double trigger registerA
                        &S12AD1.ADDBLDRB,  // double trigger registerB
                        &S12AD1.ADRD.WORD  // self-diagnosis register
                      };


/* In ROM. Sample State (SST) Register pointers */

// 8-bit register pointers
volatile __evenaccess uint8_t * const  sreg0_ptrs[ADC_SST_REG_MAX+1] =
                      { &S12AD.ADSSTR0,     // channel 0
                        &S12AD.ADSSTR1,     // channel 1
                        &S12AD.ADSSTR2,     // channel 2
                        &S12AD.ADSSTR3,
                        &S12AD.ADSSTR4,
                        &S12AD.ADSSTR5,
                        &S12AD.ADSSTR6,
                        &S12AD.ADSSTR7,
                        NULL,
                        NULL,
                        NULL
                      };


volatile __evenaccess uint8_t * const  sreg1_ptrs[ADC_SST_REG_MAX+1] =
                      { &S12AD1.ADSSTR0,     // channel 0
                        &S12AD1.ADSSTR1,     // channel 1
                        &S12AD1.ADSSTR2,     // channel 2
                        &S12AD1.ADSSTR3,
                        &S12AD1.ADSSTR4,
                        &S12AD1.ADSSTR5,
                        &S12AD1.ADSSTR6,
                        &S12AD1.ADSSTR7,
                        &S12AD1.ADSSTRL,     // channels 8-20
                        &S12AD1.ADSSTRT,     // temperature sensor
                        &S12AD1.ADSSTRO      // voltage sensor
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
*                    0 or 1
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
adc_err_t adc_open(uint8_t const          unit,
                   adc_mode_t const       mode,
                   adc_cfg_t * const      p_cfg,
                   void         (* const  p_callback)(void *p_args))
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

    err = adc_check_open_cfg(unit, mode, p_cfg, p_callback);
    if (err != ADC_SUCCESS)
    {
        return err;
    }
#endif

    /* ENSURE OPEN() NOT ALREADY IN PROGRESS */

    if (g_dcb[unit].opened == true)
    {
        return ADC_ERR_AD_NOT_CLOSED;
    }
    if (R_BSP_HardwareLock(ADC_LOCK(unit)) == false)
    {
        return ADC_ERR_AD_LOCKED;
    }


    /* APPLY POWER TO PERIPHERAL */

    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    if (unit == 0)
    {
        MSTP(S12AD) = 0;
    }
    else
    {
        MSTP(S12AD1) = 0;
    }
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

    p_regs = GET_REGS_PTR(unit);
    p_regs->ADCSR.WORD = 0;
    if (unit == 1)
    {
        S12AD1.ADEXICR.WORD = 0;
    }

    p_regs->ADANSA0.WORD = 0;
    p_regs->ADANSB0.WORD = 0;
    p_regs->ADADS0.WORD = 0;

    if (unit == 1)  /* These register are only in channel1 */
    {
        p_regs->ADANSA1.WORD = 0;
        p_regs->ADANSB1.WORD = 0;
        p_regs->ADADS1.WORD = 0;
    }
    
    p_regs->ADADC.BYTE = 0;
    
    p_regs->ADCER.WORD = 0;
    p_regs->ADSTRGR.WORD = 0;

    p_regs->ADSSTR0 = 0x0b;
    p_regs->ADSSTR1 = 0x0b;
    p_regs->ADSSTR2 = 0x0b;
    p_regs->ADSSTR3 = 0x0b;
    p_regs->ADSSTR4 = 0x0b;
    p_regs->ADSSTR5 = 0x0b;
    p_regs->ADSSTR6 = 0x0b;
    p_regs->ADSSTR7 = 0x0b;

    if (unit == 1)  /* These register are only in channel1 */
    {
        p_regs->ADSSTRL = 0x0b;
        p_regs->ADSSTRT = 0x0b;
        p_regs->ADSSTRO = 0x0b;
    }
    
    if (unit == 0)  /* This register is only in channel0 */
    {
        S12AD.ADSHMSR.BYTE = 0;
        S12AD.ADSHCR.WORD = 0x0018;
    }

    p_regs->ADDISCR.BYTE = 0;
    p_regs->ADGSPCR.WORD = 0;

    p_regs->ADCMPCR.BYTE = 0;
    p_regs->ADCMPANSR0.WORD = 0;

    if (unit == 1)  /* These register are only in channel1 */
    {
        p_regs->ADCMPANSR1.WORD = 0;
        p_regs->ADCMPANSER.BYTE = 0;
    }
    p_regs->ADCMPLR0.WORD = 0;

    if (unit == 1)  /* These register are only in channel1 */
    {
        p_regs->ADCMPLR1.WORD = 0;
        p_regs->ADCMPLER.BYTE = 0;
    }

    p_regs->ADCMPDR0 = 0;
    p_regs->ADCMPDR1 = 0;
    u16_dummy = p_regs->ADCMPSR0.WORD;
    p_regs->ADCMPSR0.WORD = 0;          /* target bit clear by read-modify-write */

    if (unit == 1)  /* These register are only in channel1 */
    {
        u16_dummy = p_regs->ADCMPSR1.WORD;
        p_regs->ADCMPSR1.WORD = 0;      /* target bit clear by read-modify-write */
        u8_dummy = p_regs->ADCMPSER.BYTE;
        p_regs->ADCMPSER.BYTE = 0;      /* target bit clear by read-modify-write */
    }
    
    /* SET MODE RELATED REGISTER FIELDS */

    g_dcb[unit].mode = mode;
    if ((mode == ADC_MODE_SS_MULTI_CH_GROUPED)
     || (mode == ADC_MODE_SS_MULTI_CH_GROUPED_DBLTRIG_A))
    {
        p_regs->ADCSR.BIT.ADCS = ADC_ADCS_GROUP_SCAN;
    }
    else
    {
        if ((mode == ADC_MODE_CONT_ONE_CH) || (mode == ADC_MODE_CONT_MULTI_CH))
        {
            p_regs->ADCSR.BIT.ADCS = ADC_ADCS_CONT_SCAN;
        }
        // other modes have ADCS=0
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
        if (unit == 0)
        {
            IPR(S12ADC0,S12GBADI0) = p_cfg->priority_groupb;
        }
        else
        {
            IPR(S12ADC1,S12GBADI1) = p_cfg->priority_groupb;
        }
    }

    if (unit == 0)
    {
        IPR(S12ADC0,S12ADI0) = p_cfg->priority;
    }
    else
    {
        IPR(S12ADC1,S12ADI1) = p_cfg->priority;
    }


    /* SET REGISTER FIELDS FOR REMAINING PARAMETERS */

    p_regs->ADCER.BIT.ADPRC = p_cfg->resolution;      // must all be separate
    p_regs->ADCER.BIT.ADRFMT = (p_cfg->alignment == ADC_ALIGN_LEFT) ? 1 : 0;
    p_regs->ADCER.BIT.ACE = (p_cfg->clearing == ADC_CLEAR_AFTER_READ_ON) ? 1 : 0;
    p_regs->ADADC.BYTE = p_cfg->add_cnt;


    /* SAVE CALLBACK FUNCTION POINTER */
    g_dcb[unit].callback = p_callback;


    /* MARK DRIVER AS OPENED */
    g_dcb[unit].opened = true;
    R_BSP_HardwareUnlock(ADC_LOCK(unit));

    return ADC_SUCCESS;
}


/******************************************************************************
* Function Name: adc_check_open_cfg
* Description  : If Parameter Checking is enabled, this function will simply
*                validate the parameters passed to the adc_open function.
** Arguments    : unit -
*                    0 or 1
*                mode -
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
static adc_err_t adc_check_open_cfg(uint8_t                unit,
                                    adc_mode_t const       mode,
                                    adc_cfg_t * const      p_cfg,
                                    void         (* const  p_callback)(void *p_args))
{

    /* Check for valid argument values */
    if (unit > 1)                           // unit is not enum value(can't check by a Compiler)
    {
        return ADC_ERR_INVALID_ARG;
    }
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
*       with this restriction include ADANSA0, ADANSA1, ADANSB0, ADANSB1, ADADS,
*       ADADC, ADADS0, ADADS1, ADSSTRx, ADSHCR, ADSHMSR, ADEXICR, ADCMPCR,
*       ADCMPANSR0, ADCMPANSR1, ADCMPANSER, ADCMPLR0, ADCMPLR1, ADCMPLER,
*       ADCMPDR0, ADCMPDR1, and some bits in ADCSR, ADCMPCR, ADGSPCR and TSCR.
*       No runtime operational sequence checking of any kind is performed.
*
* Arguments    : cmd-
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
adc_err_t adc_control(uint8_t const       unit,
                      adc_cmd_t const     cmd,
                      void * const        p_args)
{
adc_err_t       err=ADC_SUCCESS;
adc_sst_t       *p_sample;
adc_dda_t       *p_charge;
adc_cmpwin_t    *p_cmpwin;
aregs_t         *p_regs = GET_REGS_PTR(unit);
uint32_t        flags;


    /* DO UNIVERSAL PARAMETER CHECKING */

#if (ADC_CFG_PARAM_CHECKING_ENABLE == 1)
    if (unit > 1)
    {
        return ADC_ERR_INVALID_ARG;
    }

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
    case ADC_CMD_SET_DDA_STATE_CNT:
        p_charge = (adc_dda_t *)p_args;
#if (ADC_CFG_PARAM_CHECKING_ENABLE == 1)
        if (p_charge->method != ADC_DDA_OFF)
        {
            if ((p_charge->num_states < ADC_DDA_STATE_CNT_MIN)
             || (p_charge->num_states > ADC_DDA_STATE_CNT_MAX)
             || (p_regs->ADCER.BIT.DIAGM != 0)          // self-diagnosis enabled
             || (p_regs->ADEXICR.BIT.EXOEN != 0)        // ANEX enabled
             || ((p_regs->ADEXICR.WORD & 0x0F00) != 0)) // sensors enabled
            {
                return ADC_ERR_ILLEGAL_ARG;
            }
        }
#endif
        if (p_charge->method == ADC_DDA_OFF)
        {
            p_regs->ADDISCR.BYTE = 0;
        }
        else
        {
            // NOTE: Using Disconnect Detection Assist adds num_states x (#chans) ADCLKS to scan time
            p_regs->ADDISCR.BYTE = (uint8_t)((p_charge->method == ADC_DDA_PRECHARGE) ? 0x10 : 0);
            p_regs->ADDISCR.BYTE |= p_charge->num_states;
        }
    break;


    case ADC_CMD_SET_SAMPLE_STATE_CNT:
        p_sample = (adc_sst_t *)p_args;
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
        if (unit == 0)
        {
            if (p_sample->reg_id >= ADC_SST_CH8_TO_20)
            {
                return ADC_ERR_INVALID_ARG;
            }
        }

        if (p_sample->num_states < ADC_SST_CNT_MIN)
        {
            return ADC_ERR_INVALID_ARG;
        }
#endif
        *(GET_SST_ARR(unit))[p_sample->reg_id] = p_sample->num_states;
    break;


    case ADC_CMD_ENABLE_CHANS:
        err = adc_configure_scan(unit, (adc_ch_cfg_t *)p_args);
    break;


    case ADC_CMD_EN_COMPARATOR_LEVEL:
        p_cmpwin = (adc_cmpwin_t *)p_args;

        p_regs->ADCMPCR.BIT.WCMPE = 0;          // disable window function

        err = adc_en_comparator_level0(unit, p_cmpwin);
    break;


    case ADC_CMD_EN_COMPARATOR_WINDOW:
        p_cmpwin = (adc_cmpwin_t *)p_args;
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
        if (p_cmpwin->level_lo > p_cmpwin->level_hi)
        {
            return ADC_ERR_ILLEGAL_ARG;
        }
#endif
        p_regs->ADCMPCR.BIT.WCMPE = 1;          // enable window function

        err = adc_en_comparator_level0(unit, p_cmpwin);
    break;


    case ADC_CMD_ENABLE_TRIG:
        p_regs->ADCSR.BIT.TRGE = 1;           // enable sync/async triggers
    break;


    case ADC_CMD_DISABLE_TRIG:
        p_regs->ADCSR.BIT.TRGE = 0;           // disable sync/async triggers
    break;


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


    case ADC_CMD_CHECK_SCAN_DONE:           // default/Group A or Group B
        if (p_regs->ADCSR.BIT.ADST == 1)
        {
            err = ADC_ERR_SCAN_NOT_DONE;
        }
    break;


    case ADC_CMD_CHECK_SCAN_DONE_GROUPA:
        if (unit == 0)
        {
            if (IR(S12ADC0,S12ADI0) == 1)
            {
                IR(S12ADC0,S12ADI0) = 0;
            }
            else
            {
                err = ADC_ERR_SCAN_NOT_DONE;
            }
        }
        else
        {
            if (IR(S12ADC1,S12ADI1) == 1)
            {
                IR(S12ADC1,S12ADI1) = 0;
            }
            else
            {
                err = ADC_ERR_SCAN_NOT_DONE;
            }
        }
    break;


    case ADC_CMD_CHECK_SCAN_DONE_GROUPB:
        if (unit == 0)
        {
            if (IR(S12ADC0,S12GBADI0) == 1)
            {
                IR(S12ADC0,S12GBADI0) = 0;
            }
            else
            {
                err = ADC_ERR_SCAN_NOT_DONE;
            }
        }
        else
        {
            if (IR(S12ADC1,S12GBADI1) == 1)
            {
                IR(S12ADC1,S12GBADI1) = 0;
            }
            else
            {
                err = ADC_ERR_SCAN_NOT_DONE;
            }
        }
    break;


    case ADC_CMD_CHECK_CONDITION_MET:
#if 0   // Do NOT use this method. Another driver may make the GROUPBL1 priority non-zero
        // and there will be no subvector interrupt/callback here to clear the ICU flag.
        // The result will be that the BSP will be in an "infinite loop" processing the
        // group interrupt because the flag never resets, and the app level will not run.
        if (ICU.GRPBL1.LONG & g_dcb[unit].cmpi_mask)
        {
            *((uint32_t *)p_args) = adc_get_and_clr_cmpi_flags(unit);
            ICU.GRPBL1.LONG &= ~g_dcb[unit].cmpi_mask;
        }
        else
        {
            err = ADC_ERR_CONDITION_NOT_MET;
        }
#endif
        flags = adc_get_and_clr_cmpi_flags(unit);
        if (flags == 0)
        {
            err = ADC_ERR_CONDITION_NOT_MET;
        }
        *((uint32_t *)p_args) = flags;
    break;


    case ADC_CMD_ENABLE_INT:
        p_regs->ADCSR.BIT.ADIE = 1;           // enable S12ADI0 interrupt
    break;


    case ADC_CMD_DISABLE_INT:
        p_regs->ADCSR.BIT.ADIE = 0;           // disable S12ADI0 interrupt
    break;


    case ADC_CMD_ENABLE_INT_GROUPB:
        p_regs->ADCSR.BIT.GBADIE = 1;         // enable GBADI interrupt
    break;


    case ADC_CMD_DISABLE_INT_GROUPB:
        p_regs->ADCSR.BIT.GBADIE = 0;         // disable GBADI interrupt
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
*                compare. The processing is identical for window except
*                for the setting of the window enable bit and the level1
*                value (ADCMPDR1).
*
* Arguments    : unit -
*                    0 or 1
*                p_cmpwin -
*                    Pointer to comparator level/window compare structure
* Return Value : ADC_SUCCESS-
*                    Successful
*                ADC_ERR_INVALID_ARG-
*                    reg_id contains an invalid value.
*                ADC_ERR_ILLEGAL_ARG-
*                    an argument is illegal based upon rest of configuration
*                ADC_ERR_TRIG_ENABLED -
*                    Cannot configure comparator while scans are running
*******************************************************************************/
static adc_err_t adc_en_comparator_level0(uint8_t   unit, adc_cmpwin_t  *p_cmpwin)
{
aregs_t         *p_regs = GET_REGS_PTR(unit);
bsp_int_ctrl_t  group_priority;

#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
    if (p_cmpwin->int_priority > BSP_MCU_IPL_MAX)                   // interrupt priority only use 0 to 15
    {
        return ADC_ERR_INVALID_ARG;
    }
    if ((p_cmpwin->compare_mask & ADC_INVALID_CH_MASK(unit)) != 0)  // checking exist channel
    {
        return ADC_ERR_INVALID_ARG;
    }
    if ((p_cmpwin->inside_window_mask | p_cmpwin->compare_mask) != p_cmpwin->compare_mask)  // checking compare channel combination
    {
        return ADC_ERR_INVALID_ARG;
    }
    if (p_regs->ADCSR.BIT.DBLE == 1)                                // Comparison disable when Double trigger mode
    {
        return ADC_ERR_INVALID_ARG;
    }
    if (p_regs->ADCER.BIT.DIAGM != 0)                               // Comparison disable when Self-diagnosis mode
    {
        return ADC_ERR_INVALID_ARG;
    }
#endif

    if (p_regs->ADCSR.BIT.TRGE == 1)
    {
        return ADC_ERR_TRIG_ENABLED;    // ADST must be 0 to configure comparator
    }


    /* CONFIGURE COMPARATOR */

    p_regs->ADCMPDR0 = p_cmpwin->level_lo;      // set lo end of window or set level
    if (p_regs->ADCMPCR.BIT.WCMPE == 1)
    {
        p_regs->ADCMPDR1 = p_cmpwin->level_hi;  // set hi end of window
    }

    p_regs->ADCMPANSR0.WORD = p_cmpwin->compare_mask & 0x0FFFF;
    p_regs->ADCMPLR0.WORD = p_cmpwin->inside_window_mask & 0x0FFFF;
    if (unit == 1)
    {
        S12AD1.ADCMPANSR1.BIT.CMPS1 = (p_cmpwin->compare_mask>>16) & 0x001F;
        S12AD1.ADCMPANSER.BIT.CMPSTS = (p_cmpwin->compare_mask & ADC_MASK_TEMP) ? 1 : 0;
        S12AD1.ADCMPANSER.BIT.CMPSOC = (p_cmpwin->compare_mask & ADC_MASK_VOLT) ? 1 : 0;

        S12AD1.ADCMPLR1.BIT.CMPL1 = (p_cmpwin->inside_window_mask>>16) & 0x0001F;
        S12AD1.ADCMPLER.BIT.CMPLTS = (p_cmpwin->inside_window_mask & ADC_MASK_TEMP) ? 1 : 0;
        S12AD1.ADCMPLER.BIT.CMPLOC = (p_cmpwin->inside_window_mask & ADC_MASK_VOLT) ? 1 : 0;
    }


    /* ENABLE INTERRUPT */

    if (p_cmpwin->int_priority != 0)
    {
        group_priority.ipl = (uint32_t)(p_cmpwin->int_priority);

        R_BSP_InterruptWrite(BSP_INT_SRC_BL1_S12AD0_S12CMPI0, adc_s12cmpi0_isr);    // callback function setting(GROUPBL1.IS20)
        R_BSP_InterruptWrite(BSP_INT_SRC_BL1_S12AD1_S12CMPI1, adc_s12cmpi1_isr);    // callback function setting(GROUPBL1.IS22)
        p_regs->ADCMPCR.BIT.CMPIE = 1;              // enable interrupt in ADC
        ICU.GENBL1.LONG |= g_dcb[unit].cmpi_mask;   // enable group subvector

        /* set priority for S12CMPI interrupts */
        if (unit == 0)
        {
            R_BSP_InterruptControl(BSP_INT_SRC_BL1_S12AD0_S12CMPI0, BSP_INT_CMD_GROUP_INTERRUPT_ENABLE, (void *)&group_priority);
        }
        else
        {
            R_BSP_InterruptControl(BSP_INT_SRC_BL1_S12AD1_S12CMPI1, BSP_INT_CMD_GROUP_INTERRUPT_ENABLE, (void *)&group_priority);
        }
    }

    /* Do NOT enable GENBL1 or CMPIE here with the intention of polling the GRPBL1 bit
     * for condition met. Another driver may make the GROUPBL1 priority non-zero
     * and there will be no subvector interrupt/callback here to clear the ICU flag.
     * The result will be that the BSP will be in an "infinite loop" processing the
     * group interrupt because the flag never resets, and the app level will not run.
     */
    return ADC_SUCCESS;
}


/*****************************************************************************
* Function Name: adc_s12cmpi0_isr
* Description  : Unit0 interrupt handler (bsp GRPBL1 callback) for comparator.
* Arguments    : none
* Return Value : none
******************************************************************************/
void adc_s12cmpi0_isr(void *cb_args)
{
adc_cb_args_t   args;

    if ((g_dcb[0].callback != NULL) && (g_dcb[0].callback != FIT_NO_FUNC))
    {
        /* NOTE: S12CMPI interrupts are level-based, not edge, so there will be
         * continuous triggers while condition is met. When the condition is no
         * longer met, both the ICU.GRPBL1 flag and subvector flag are
         * automatically cleared.
         */
        args.event = ADC_EVT_CONDITION_MET;
        args.unit = 0;
        args.compare_flags = adc_get_and_clr_cmpi_flags(0);
        g_dcb[0].callback(&args);
    }
}


/*****************************************************************************
* Function Name: adc_s12cmpi1_isr
* Description  : Unit1 interrupt handler (bsp GRPBL1 callback) for comparator.
* Arguments    : none
* Return Value : none
******************************************************************************/
void adc_s12cmpi1_isr(void *cb_args)
{
adc_cb_args_t   args;

    if ((g_dcb[1].callback != NULL) && (g_dcb[1].callback != FIT_NO_FUNC))
    {
        /* NOTE: S12CMPI interrupts are level-based, not edge, so there will be
         * continuous triggers while condition is met. When the condition is no
         * longer met, both the ICU.GRPBL1 flag and subvector flag are
         * automatically cleared.
         */
        args.event = ADC_EVT_CONDITION_MET;
        args.unit = 1;
        args.compare_flags = adc_get_and_clr_cmpi_flags(1);
        g_dcb[1].callback(&args);
    }
}


/*****************************************************************************
* Function Name: adc_get_and_clr_cmpi_flags
* Description  : Returns flags for all channels/sensors that met comparator
*                condition and clears the flags for next interrupt.
* Arguments    : unit -
*                   0 or 1
* Return Value : channels/sensors that met comparator condition
******************************************************************************/
static uint32_t adc_get_and_clr_cmpi_flags(uint8_t unit)
{
uint32_t    flags;

    /* NOTE: S12CMPI interrupts are level-based, not edge, so there will be
     * continuous triggers while condition is met. Writing 0 to the flags has
     * no effect UNTIL the condition clears.
     *
     * Also, the data register for the channel/sensor will still be written to.
     * The S12CMPI interrupt does not replace it but is in addition to it.
     */

    if (unit == 0)
    {
        flags = (uint32_t)S12AD.ADCMPSR0.WORD;
        S12AD.ADCMPSR0.WORD = 0;
    }
    else
    {
        flags = (uint32_t)S12AD1.ADCMPSR0.WORD;
        S12AD1.ADCMPSR0.WORD = 0;

        flags |= ((uint32_t)S12AD1.ADCMPSR1.BIT.CMPF1) << 16;
        S12AD1.ADCMPSR1.WORD = 0;

        flags |= (S12AD1.ADCMPSER.BIT.CMPFTS == 1) ? ADC_MASK_TEMP : 0;
        flags |= (S12AD1.ADCMPSER.BIT.CMPFOC == 1) ? ADC_MASK_VOLT : 0;
        S12AD1.ADCMPSER.BYTE = 0;
    }

    return flags;
}


/******************************************************************************
* Function Name: adc_configure_scan
* Description  : This function does extensive checking on channel mask
*                settings based upon operational mode. Mask registers are
*                initialized and interrupts enabled in peripheral. Interrupts
*                are also enabled in ICU if corresponding priority is not 0.
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
static adc_err_t adc_configure_scan(uint8_t const   unit,
                                    adc_ch_cfg_t   *p_config)
{
uint16_t    i=0;
uint32_t    tmp_mask=0;
aregs_t     *p_regs = GET_REGS_PTR(unit);
uint16_t    trig_a;
uint16_t    trig_b;

#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
    adc_err_t   err;

    err = adc_check_scan_config(unit, p_config);
    if (err != ADC_SUCCESS)
    {
        return err;
    }
#endif // parameter checking


    /* SET MASKS FOR CHANNELS AND SENSORS */

    p_regs->ADANSA0.WORD = (uint16_t) (p_config->chan_mask & 0xFFFF);
    p_regs->ADANSB0.WORD = (uint16_t) (p_config->chan_mask_groupb & 0xFFFF);
    p_regs->ADADS0.WORD = (uint16_t) (p_config->add_mask & 0xFFFF);

    if (unit == 1)
    {
        S12AD1.ADANSA1.WORD = (uint16_t) ((p_config->chan_mask >> 16) & 0x001F);
        S12AD1.ADANSB1.WORD = (uint16_t) ((p_config->chan_mask_groupb >> 16) & 0x001F);
        S12AD1.ADADS1.WORD = (uint16_t) ((p_config->add_mask >> 16) & 0x001F);
        adc_configure_sensors(p_config);
    }


    // NOTE: S&H adds to scan time because normal state machine still runs.
    // adds 12 + sample_hold_states ADCLKS to scan time
    if (unit == 0)
    {
        S12AD.ADSHCR.BIT.SHANS = p_config->sample_hold_mask;
        S12AD.ADSHCR.BIT.SSTSH = p_config->sample_hold_states;
    }


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


    /* SET ANEX USAGE */

    if ((unit == 1) && (p_config->anex_enable))
    {
        S12AD1.ADEXICR.BIT.EXSEL = 1;
        S12AD1.ADEXICR.BIT.EXOEN = 1;
    }


    /* SET SELF DIAGNOSIS REGISTERS (VIRTUAL CHANNEL) */

    if (p_config->diag_method == ADC_DIAG_OFF)
    {
        p_regs->ADCER.BIT.DIAGM = 0;
    }
    else
    {
        // NOTE: Using Self Diagnosis adds 11/13/15(resolution) + (ch0 SST) ADCLKS to scan time.
        // (ch0 can still be used with self diagnosis on)
        if (p_config->diag_method == ADC_DIAG_ROTATE_VOLTS)
        {
            p_regs->ADCER.BIT.DIAGLD = 0;
            p_regs->ADCER.BIT.DIAGVAL = 1;
        }
        else
        {
            p_regs->ADCER.BIT.DIAGLD = 1;
            p_regs->ADCER.BIT.DIAGVAL = (uint8_t)(p_config->diag_method & 0x3);   // 2-bit value;
        }
        p_regs->ADCER.BIT.DIAGM = 1;
    }


    /* SET DOUBLE TRIGGER CHANNEL */

    if (p_regs->ADCSR.BIT.DBLE == 1)
    {
        tmp_mask = p_config->chan_mask;         // tmp_mask is non-Group/Group A chans
        while (tmp_mask >>= 1)                  // determine bit/ch number
        {
            i++;
        }
        p_regs->ADCSR.BIT.DBLANS = i;
    }


    /* ENABLE INTERRUPTS */

    adc_enable_s12adi(unit);
    if (p_regs->ADCSR.BIT.ADCS == ADC_ADCS_GROUP_SCAN)
    {
        adc_enable_s12gbadi(unit);
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
static adc_err_t adc_check_scan_config(uint8_t const   unit,
                                        adc_ch_cfg_t   *p_config)
{
uint32_t    tmp_mask=0;
uint32_t    a_mask,b_mask;
aregs_t     *p_regs = GET_REGS_PTR(unit);

    /* Verify at least one bonded channel is selected */
     if ((p_config->chan_mask == 0)
     || ((p_config->chan_mask & ADC_INVALID_CH_MASK(unit)) != 0))
    {
        return ADC_ERR_INVALID_ARG;
    }

    /* Verify at least one unique bonded channel is selected for Group B
     * and GroupA priority setting is valid.
     */
    if (p_regs->ADCSR.BIT.ADCS == ADC_ADCS_GROUP_SCAN)
    {
        if ((p_config->chan_mask_groupb == 0)
         || ((p_config->chan_mask_groupb & ADC_INVALID_CH_MASK(unit)) != 0))
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

    /* If sensors specified, verify in legal configuration */
    if ((p_config->chan_mask & ADC_MASK_SENSORS) || (p_config->chan_mask_groupb & ADC_MASK_SENSORS))
    {
        if ((unit == 0)
         || (S12AD1.ADCSR.BIT.DBLE == 1)       // double trigger mode
         || (S12AD1.ADDISCR.BYTE != 0)         // disconnect detection assist enabled
         || (p_config->anex_enable == true))   // external amplifier enabled
        {
            return ADC_ERR_ILLEGAL_ARG;
        }
    }

    /* If ANEX enabled verify no group modes, no self-diagnosis (below), no DDA, no sensors (above) */
    if (p_config->anex_enable == true)
    {
        if ((p_regs->ADCSR.BIT.ADCS == ADC_ADCS_GROUP_SCAN)
         || (p_regs->ADDISCR.BYTE != 0)
         || (unit == 0)                         // cannot use unit 0
         || (p_config->chan_mask & 0x1FFF00))   // cannot use channels 8-20
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
        // WARNING! Other features messed up if add_mask is non-zero when addition is turned off!
        p_config->add_mask = 0;
    }

    /* Verify only 1 bit is set in default/Group A mask */
    if ((g_dcb[unit].mode == ADC_MODE_SS_ONE_CH)
     || (g_dcb[unit].mode == ADC_MODE_CONT_ONE_CH)
     || (p_regs->ADCSR.BIT.DBLE == 1))          // double trigger mode
    {
        tmp_mask = p_config->chan_mask;         // tmp_mask is non-Group/Group A chans
        // Bit-AND with 2s-complement (see note in function header)
        if ((tmp_mask & (~tmp_mask + 1)) != tmp_mask)
        {
            return ADC_ERR_ILLEGAL_ARG;
        }
    }

    if (p_config->sample_hold_mask != 0)
    {
        if (unit == 1)                          // S&H only on unit 0
        {
            return ADC_ERR_ILLEGAL_ARG;
        }

        /* S&H chans can only be 0,1,2 and must have at least minimum state count specified */
        if ((p_config->sample_hold_mask > 0x7)
         || (p_config->sample_hold_states < ADC_SST_SH_CNT_MIN))
        {
            return ADC_ERR_INVALID_ARG;
        }

        /* S&H channels cannot not be split across groups */
        a_mask = p_config->sample_hold_mask & p_config->chan_mask;
        b_mask = p_config->sample_hold_mask & p_config->chan_mask_groupb;
        if (S12AD.ADCSR.BIT.ADCS == ADC_ADCS_GROUP_SCAN)
        {
            if ((a_mask != p_config->sample_hold_mask) && (b_mask != p_config->sample_hold_mask))
                return ADC_ERR_ILLEGAL_ARG;
        }
        else if (a_mask != p_config->sample_hold_mask)
        {
            return ADC_ERR_ILLEGAL_ARG;
        }


        /* S&H channels cannot be a double trigger channel (can be in group B) */
        if ((S12AD.ADCSR.BIT.DBLE == 1) && (a_mask != 0))
        {
            return ADC_ERR_ILLEGAL_ARG;
        }

        /* S&H channels cannot be in GroupB if GroupA priority enabled */
        if ((b_mask != 0) && (p_config->priority_groupa != ADC_GRPA_PRIORITY_OFF))
        {
            return ADC_ERR_ILLEGAL_ARG;
        }
    }

    if ((p_config->diag_method != ADC_DIAG_OFF)
     &&   ((p_regs->ADADC.BIT.ADC != 0)         // addition
        || (p_regs->ADCSR.BIT.DBLE == 1)        // double trigger mode
        || (p_config->anex_enable == true)      // ANEX
        || (p_regs->ADCMPCR.BIT.CMPIE == 1)))   // comparator
    {
        return ADC_ERR_ILLEGAL_ARG;
    }


    return ADC_SUCCESS;
}
#endif


/******************************************************************************
* Function Name: adc_configure_sensors
* Description  : This function set the sensor bits taking into account group
*                inclusion and addition/average mode.
* Arguments    : p_config -
*                   The configuration argument passed to ADC_CMD_ENABLE_CHANS.
* Return Value : none
*******************************************************************************/
static void adc_configure_sensors(adc_ch_cfg_t   *p_config)
{

    /* TEMPERATURE SENSOR */

    if ((p_config->chan_mask & ADC_MASK_TEMP) || (p_config->chan_mask_groupb & ADC_MASK_TEMP))
    {
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
        MSTP(TEMPS) = 0;
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

        if (p_config->chan_mask & ADC_MASK_TEMP)
        {
            S12AD1.ADEXICR.BIT.TSSA = 1;            // select temperature output to GroupA
            S12AD1.ADEXICR.BIT.TSSB = 0;
        }
        else
        {
            S12AD1.ADEXICR.BIT.TSSA = 0;
            S12AD1.ADEXICR.BIT.TSSB = 1;            // select temperature output to GroupB
        }

        S12AD1.ADEXICR.BIT.TSSAD = (p_config->add_mask & ADC_MASK_TEMP) ? 1 : 0;

        TEMPS.TSCR.BIT.TSEN = 1;                    // enable temp sensor

        if (1 != TEMPS.TSCR.BIT.TSEN)
        {
            /* Dommy read and compare */
            nop();
        }

        /* 30us delay */
        R_BSP_SoftwareDelay(30, BSP_DELAY_MICROSECS);

        TEMPS.TSCR.BIT.TSOE = 1;                    // enable temp sensor output to adc
    }


    /* INTERNAL REFERENCE VOLTAGE SENSOR */

    if ((p_config->chan_mask & ADC_MASK_VOLT) || (p_config->chan_mask_groupb & ADC_MASK_VOLT))
    {
        if (p_config->chan_mask & ADC_MASK_VOLT)
        {
            S12AD1.ADEXICR.BIT.OCSA = 1;            // select internal reference voltage output to GroupA
            S12AD1.ADEXICR.BIT.OCSB = 0;
        }
        else
        {
            S12AD1.ADEXICR.BIT.OCSA = 0;
            S12AD1.ADEXICR.BIT.OCSB = 1;            // select internal reference voltage output to GroupB
        }

        S12AD1.ADEXICR.BIT.OCSAD = (p_config->add_mask & ADC_MASK_VOLT) ? 1 : 0;
    }

    return;
}


/******************************************************************************
* Function Name: adc_enable_s12adi
* Description  : This function clears the S12ADI interrupt flag and enables
*                interrupts in the peripheral (for IR flag usage). If priority
*                is not 0, interrupts are enabled in the ICU.
*                NOTE: This has nothing to do with enabling triggers.
* Arguments    : unit -
*                   0 or 1
* Return Value : none
*******************************************************************************/
static void adc_enable_s12adi(uint8_t unit)
{

    if (unit == 0)
    {
        IR(S12ADC0,S12ADI0) = 0;            // clear flag
        S12AD.ADCSR.BIT.ADIE = 1;           // enable in peripheral
        if (IPR(S12ADC0,S12ADI0) != 0)
        {
            IEN(S12ADC0,S12ADI0) = 1;       // enable in ICU
        }
    }
    else // Unit1
    {

        IR(S12ADC1,S12ADI1) = 0;            // clear flag
        S12AD1.ADCSR.BIT.ADIE = 1;          // enable in peripheral
        if (IPR(S12ADC1,S12ADI1) != 0)
        {
            IEN(S12ADC1,S12ADI1) = 1;       // enable in ICU
        }
    }
}


/******************************************************************************
* Function Name: adc_enable_s12gbadi
* Description  : This function clears the S12GBADI interrupt flag and enables
*                interrupts in the peripheral (for IR flag usage). If priority
*                is not 0, interrupts are enabled in the ICU.
*                NOTE: This has nothing to do with enabling triggers.
* Arguments    : unit -
*                   0 or 1
* Return Value : none
*******************************************************************************/
static void adc_enable_s12gbadi(uint8_t unit)
{

    if (unit == 0)
    {
        IR(S12ADC0,S12GBADI0) = 0;            // clear flag
        S12AD.ADCSR.BIT.GBADIE = 1;           // enable in peripheral
        if (IPR(S12ADC0,S12GBADI0) != 0)
        {
            IEN(S12ADC0,S12GBADI0) = 1;       // enable in ICU
        }
    }
    else // Unit1
    {

        IR(S12ADC1,S12GBADI1) = 0;            // clear flag
        S12AD1.ADCSR.BIT.GBADIE = 1;          // enable in peripheral
        if (IPR(S12ADC1,S12GBADI1) != 0)
        {
            IEN(S12ADC1,S12GBADI1) = 1;       // enable in ICU
        }
    }
}


/*****************************************************************************
* Function Name: adc_s12adi0_isr
* Description  : Unit0 interrupt handler for normal/Group A/double trigger
*                scan complete.
* Arguments    : none
* Return Value : none
******************************************************************************/
R_PRAGMA_STATIC_INTERRUPT(adc_s12adi0_isr, VECT(S12ADC0,S12ADI0))
//#pragma interrupt adc_s12adi0_isr(vect=VECT(S12ADC0,S12ADI0), enable) // FOR TESTING ONLY!
static void adc_s12adi0_isr(void)
{
adc_cb_args_t   args;

    if ((g_dcb[0].callback != NULL) && (g_dcb[0].callback != FIT_NO_FUNC))
    {
        args.event = ADC_EVT_SCAN_COMPLETE;
        args.unit = 0;
        g_dcb[0].callback(&args);
    }
}


/*****************************************************************************
* Function Name: adc_s12adi1_isr
* Description  : Unit1 interrupt handler for normal/Group A/double trigger
*                scan complete.
* Arguments    : none
* Return Value : none
******************************************************************************/
R_PRAGMA_STATIC_INTERRUPT(adc_s12adi1_isr, VECT(S12ADC1,S12ADI1))
//#pragma interrupt adc_s12adi0_isr(vect=VECT(S12ADC1,S12ADI1), enable) // FOR TESTING ONLY!
static void adc_s12adi1_isr(void)
{
adc_cb_args_t   args;

    if ((g_dcb[1].callback != NULL) && (g_dcb[1].callback != FIT_NO_FUNC))
    {
        args.event = ADC_EVT_SCAN_COMPLETE;
        args.unit = 1;
        g_dcb[1].callback(&args);
    }
}


/*****************************************************************************
* Function Name: adc_s12gbadi0_isr
* Description  : Unit0 interrupt handler for Group B scan complete.
* Arguments    : none
* Return Value : none
******************************************************************************/
R_PRAGMA_STATIC_INTERRUPT(adc_s12gbadi0_isr, VECT(S12ADC0,S12GBADI0))
//#pragma interrupt adc_s12gbadi0_isr(vect=VECT(S12ADC0,S12GBADI0),enable)  // FOR TESTING ONLY!
static void adc_s12gbadi0_isr(void)
{
adc_cb_args_t   args;

    if ((g_dcb[0].callback != NULL) && (g_dcb[0].callback != FIT_NO_FUNC))
    {
        args.event = ADC_EVT_SCAN_COMPLETE_GROUPB;
        args.unit = 0;
        g_dcb[0].callback(&args);
    }
}


/*****************************************************************************
* Function Name: adc_s12gbadi1_isr
* Description  : Unit1 interrupt handler for Group B scan complete.
* Arguments    : none
* Return Value : none
******************************************************************************/
R_PRAGMA_STATIC_INTERRUPT(adc_s12gbadi1_isr, VECT(S12ADC1,S12GBADI1))
//#pragma interrupt adc_s12gbadi1_isr(vect=VECT(S12ADC1,S12GBADI1),enable)  // FOR TESTING ONLY!
static void adc_s12gbadi1_isr(void)
{
adc_cb_args_t   args;

    if ((g_dcb[1].callback != NULL) && (g_dcb[1].callback != FIT_NO_FUNC))
    {
        args.event = ADC_EVT_SCAN_COMPLETE_GROUPB;
        args.unit = 1;
        g_dcb[1].callback(&args);
    }
}


/******************************************************************************
* Function Name: adc_close
* Description  : This is the RX71M implementation of R_ADC_Close().
*                This function ends any scan in progress, disables interrupts,
*                and removes power to the A/D peripheral.
* Arguments    : none
* Return Value : none
*******************************************************************************/
adc_err_t adc_close(uint8_t const  unit)
{
    volatile uint8_t i;
    aregs_t     *p_regs = GET_REGS_PTR(unit);

#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
    if (unit > 1)
    {
        return ADC_ERR_INVALID_ARG;
    }
#endif


    /* Stop triggers & conversions, and disable peripheral interrupts */
    if (unit == 0)
    {
        IEN(S12ADC0,S12ADI0) = 0;         // disable interrupts in ICU
        IEN(S12ADC0,S12GBADI0) = 0;       // disable interrupts in ICU

        IPR(S12ADC0,S12ADI0) = 0;         // clear interrupt priority
        IPR(S12ADC0,S12GBADI0) = 0;       // clear interrupt priority
    }
    else // Unit1
    {
        IEN(S12ADC1,S12ADI1) = 0;         // disable interrupts in ICU
        IEN(S12ADC1,S12GBADI1) = 0;       // disable interrupts in ICU

        IPR(S12ADC1,S12ADI1) = 0;         // clear interrupt priority
        IPR(S12ADC1,S12GBADI1) = 0;       // clear interrupt priority
    }

    /* This program does not disable IPR register of Group interrupt. Because,
       other peripheral is possible to use same vector Group interrupt. */
    IEN(ICU,GROUPBL1) = 0;                      // disable group
    ICU.GENBL1.LONG &= ~g_dcb[unit].cmpi_mask;  // disable group subvector
    p_regs->ADCMPCR.BIT.CMPIE = 0;              // disable interrupt in ADC
    if (0 != p_regs->ADCMPCR.BIT.CMPIE)         // dummy read for waiting until set the value of CMPIE
    {
        nop();
    }

    p_regs->ADGSPCR.BIT.PGS = 0;
    if (ADC_ADCS_GROUP_SCAN == p_regs->ADCSR.BIT.ADCS)
    {
        p_regs->ADSTRGR.WORD = 0x3F3F;
        p_regs->ADCSR.BIT.ADIE = 0;
        p_regs->ADCSR.BIT.GBADIE = 0;
    }
    else
    {
        p_regs->ADSTRGR.BIT.TRSA = 0x3F;
        p_regs->ADCSR.BIT.ADIE = 0;
    }

    p_regs->ADCSR.WORD = 0;
    if (0 != p_regs->ADCSR.WORD)                // dummy read for waiting until set the value of ADCSR
    {
        nop();
    }

    /* Wait for 2 ADCLK cycles (MAX: 128 ICLK cycles) */
    for (i = 0; i < 128; i++)
    {
        nop();
    }

    if (0 == unit)
    {
        IR(S12ADC0,S12ADI0) = 0;          // clear interrupt flag
        IR(S12ADC0,S12GBADI0) = 0;        // clear interrupt flag
        ICU.GRPBL1.LONG &= ~g_dcb[unit].cmpi_mask;  // claer interrupt flag
    }
    else /* Unit1 */
    {
        IR(S12ADC1,S12ADI1) = 0;          // clear interrupt flag
        IR(S12ADC1,S12GBADI1) = 0;        // clear interrupt flag
        ICU.GRPBL1.LONG &= ~g_dcb[unit].cmpi_mask;  // claer interrupt flag
    }

    /* if S12AD don't use group interrupt,
     * but other FIT modules use group interrupt when enable group interrupt.
     */
    if (ICU.GENBL1.LONG != 0)
    {
        IEN(ICU,GROUPBL1) = 1;                  // enable group
    }

    /* Power down peripheral */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    if (unit == 0)
    {
        MSTP(S12AD) = 1;
    }
    else
    {
        TEMPS.TSCR.BYTE = 0;
        MSTP(TEMPS) = 1;
        MSTP(S12AD1) = 1;
    }
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);


    /* Show driver as closed */

    g_dcb[unit].opened = false;

    return ADC_SUCCESS;
}


/******************************************************************************
* Function Name: adc_read_all
* Description  : This is the RX71M implementation of R_ADC_ReadAll().
*                It reads conversion results from all potential sources,
*                enabled or not.
* Arguments    : p_data-
*                    Pointer to structure to load register values into.
* Return Value : ADC_SUCCESS-
*                    Successful
*******************************************************************************/
adc_err_t adc_read_all(adc_data_t * const  p_all_data)
{
    p_all_data->unit0.chan[ADC_REG_CH0]  = S12AD.ADDR0;
    p_all_data->unit0.chan[ADC_REG_CH1]  = S12AD.ADDR1;
    p_all_data->unit0.chan[ADC_REG_CH2]  = S12AD.ADDR2;
    p_all_data->unit0.chan[ADC_REG_CH3]  = S12AD.ADDR3;
    p_all_data->unit0.chan[ADC_REG_CH4]  = S12AD.ADDR4;
    p_all_data->unit0.chan[ADC_REG_CH5]  = S12AD.ADDR5;
    p_all_data->unit0.chan[ADC_REG_CH6]  = S12AD.ADDR6;
    p_all_data->unit0.chan[ADC_REG_CH7]  = S12AD.ADDR7;
    p_all_data->unit0.dbltrig            = S12AD.ADDBLDR.WORD;
    p_all_data->unit0.dbltrigA           = S12AD.ADDBLDRA;
    p_all_data->unit0.dbltrigB           = S12AD.ADDBLDRB;
    p_all_data->unit0.self_diag          = S12AD.ADRD.WORD;

    p_all_data->unit1.chan[ADC_REG_CH0]  = S12AD1.ADDR0;
    p_all_data->unit1.chan[ADC_REG_CH1]  = S12AD1.ADDR1;
    p_all_data->unit1.chan[ADC_REG_CH2]  = S12AD1.ADDR2;
    p_all_data->unit1.chan[ADC_REG_CH3]  = S12AD1.ADDR3;
    p_all_data->unit1.chan[ADC_REG_CH4]  = S12AD1.ADDR4;
    p_all_data->unit1.chan[ADC_REG_CH5]  = S12AD1.ADDR5;
    p_all_data->unit1.chan[ADC_REG_CH6]  = S12AD1.ADDR6;
    p_all_data->unit1.chan[ADC_REG_CH7]  = S12AD1.ADDR7;
    p_all_data->unit1.chan[ADC_REG_CH8]  = S12AD1.ADDR8;
    p_all_data->unit1.chan[ADC_REG_CH9]  = S12AD1.ADDR9;
    p_all_data->unit1.chan[ADC_REG_CH10] = S12AD1.ADDR10;
    p_all_data->unit1.chan[ADC_REG_CH11] = S12AD1.ADDR11;
    p_all_data->unit1.chan[ADC_REG_CH12] = S12AD1.ADDR12;
    p_all_data->unit1.chan[ADC_REG_CH13] = S12AD1.ADDR13;
    p_all_data->unit1.chan[ADC_REG_CH14] = S12AD1.ADDR14;
    p_all_data->unit1.chan[ADC_REG_CH15] = S12AD1.ADDR15;
    p_all_data->unit1.chan[ADC_REG_CH16] = S12AD1.ADDR16;
    p_all_data->unit1.chan[ADC_REG_CH17] = S12AD1.ADDR17;
    p_all_data->unit1.chan[ADC_REG_CH18] = S12AD1.ADDR18;
    p_all_data->unit1.chan[ADC_REG_CH19] = S12AD1.ADDR19;
    p_all_data->unit1.chan[ADC_REG_CH20] = S12AD1.ADDR20;
    p_all_data->unit1.temp               = S12AD1.ADTSDR;
    p_all_data->unit1.volt               = S12AD1.ADOCDR;
    p_all_data->unit1.dbltrig            = S12AD1.ADDBLDR;
    p_all_data->unit1.dbltrigA           = S12AD1.ADDBLDRA;
    p_all_data->unit1.dbltrigB           = S12AD1.ADDBLDRB;
    p_all_data->unit1.self_diag          = S12AD1.ADRD.WORD;

    return ADC_SUCCESS;
}


#endif // #if defined (BSP_MCU_RX71M)
