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
/***********************************************************************************************************************
* File Name    : r_s12ad_rx63x.c
* Description  : ROM tables and support code for RX63N, RX631 devices.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*           22.02.2014 1.00    Initial Release.
*           05.06.2014 1.30    Fixed channel mask bug in adc_enable_chans_63x()
*           01.10.2016 2.20    Define name commoditize for the same meaning but different name define.
*                              Delete parameter check by the enum value.
*                              TEMPS register can change only during temperature sensor mode.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Includes board and MCU related header files. */
#include "platform.h"

#ifdef BSP_MCU_RX63_ALL
#include "r_s12ad_rx_config.h"
#include "r_s12ad_rx_private.h"
#include "r_s12ad_rx_if.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
 
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
extern adc_ctrl_t g_dcb;

adc_err_t adc_enable_chans_63x(adc_ch_cfg_t *p_config);
extern void adc_enable_s12adi0(void);


/* In ROM. A/D Data Register pointers */

volatile __evenaccess uint16_t * const  dreg_ptrs[ADC_REG_MAX+1] =
                      { &S12AD.ADDR0,       // channel 0
                        &S12AD.ADDR1,       // channel 1
                        &S12AD.ADDR2,       // channel 2...
                        &S12AD.ADDR3,
                        &S12AD.ADDR4,
                        &S12AD.ADDR5,
                        &S12AD.ADDR6,
                        &S12AD.ADDR7,
                        &S12AD.ADDR8,
                        &S12AD.ADDR9,
                        &S12AD.ADDR10,
                        &S12AD.ADDR11,
                        &S12AD.ADDR12,
                        &S12AD.ADDR13,
                        &S12AD.ADDR14,
                        &S12AD.ADDR15,
                        &S12AD.ADDR16,
                        &S12AD.ADDR17,
                        &S12AD.ADDR18,
                        &S12AD.ADDR19,
                        &S12AD.ADDR20,
                        &S12AD.ADTSDR,    // temperature sensor
                        &S12AD.ADOCDR,    // voltage sensor
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
*                    Ignored for rx63x. Pass 0 for this argument.
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
adc_err_t adc_open(uint8_t const          unit,
                   adc_mode_t const       mode,
                   adc_cfg_t * const      p_cfg,
                   void         (* const  p_callback)(void *p_args))
{

#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
    if ((p_cfg == NULL) || (p_cfg == FIT_NO_PTR))
    {
        return ADC_ERR_MISSING_PTR;
    }

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
#endif

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
    if (mode == ADC_MODE_SS_TEMPERATURE)
    {
        MSTP(TEMPS) = 0;
        TEMPS.TSCR.BYTE = 0;
    }
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

    S12AD.ADCSR.BYTE = 0;
    S12AD.ADEXICR.WORD = 0;

    S12AD.ADANS0.WORD = 0;
    S12AD.ADANS1.WORD = 0;

    S12AD.ADADS0.WORD = 0;
    S12AD.ADADS1.WORD = 0;

    S12AD.ADADC.BYTE = 0;

    S12AD.ADCER.WORD = 0;
    S12AD.ADSTRGR.BYTE = 0;

    S12AD.ADSSTR01.WORD = 0x1414;
    S12AD.ADSSTR23.WORD = 0x1414;

    /* SET MODE RELATED REGISTER FIELDS */

    g_dcb.mode = mode;

    if ((mode == ADC_MODE_CONT_ONE_CH) || (mode == ADC_MODE_CONT_MULTI_CH))
    {
        S12AD.ADCSR.BIT.ADCS = 1;
    }
    // other modes have ADCS=0


    /* SET TRIGGER AND INTERRUPT PRIORITY REGISTER FIELDS */

    if (p_cfg->trigger != ADC_TRIG_SOFTWARE)
    {
        S12AD.ADSTRGR.BIT.ADSTRS = p_cfg->trigger;
    }
    if (p_cfg->trigger == ADC_TRIG_ASYNC_ADTRG)
    {
        S12AD.ADCSR.BIT.EXTRG = 1;      // set ext trigger for async trigger
    }
    IPR(S12AD,S12ADI0) = p_cfg->priority;


    /* SET REGISTER FIELDS FOR REMAINING PARAMETERS */

    S12AD.ADADC.BIT.ADC = p_cfg->add_cnt;
    S12AD.ADCER.WORD = (uint16_t) (p_cfg->alignment | p_cfg->clearing);
    S12AD.ADCSR.BIT.CKS = p_cfg->conv_speed;


    /* SAVE CALLBACK FUNCTION POINTER */
    g_dcb.callback = p_callback;


    /* MARK DRIVER AS OPENED */
    g_dcb.opened = true;
    R_BSP_HardwareUnlock(BSP_LOCK_S12AD);

    return ADC_SUCCESS;
}


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
*       with this restriction include ADANS0, ADANS1, ADADS0, ADADS1, ADADC,
*       ADSSTR01, ADSSTR23, ADEXICR, and some bits in ADCSR and TSCR.
*       No runtime operational sequence checking of any kind is performed.
*
* Arguments    : unit -
*                    Ignored for rx63x. Pass 0 for this argument.
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
adc_err_t adc_control(uint8_t const       unit,
                      adc_cmd_t const     cmd,
                      void * const        p_args)
{
adc_err_t       err=ADC_SUCCESS;
adc_time_t      *p_sample;


    switch (cmd)
    {
    case ADC_CMD_SET_SAMPLE_STATE_CNT:
        p_sample = (adc_time_t *) p_args;

#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
        if (p_sample->num_states < ADC_SST_CNT_MIN)
        {
            return ADC_ERR_INVALID_ARG;
        }
#endif
        if (p_sample->reg_id == ADC_SST_CH0_TO_20)
        {
            S12AD.ADSSTR01.BIT.SST1 = p_sample->num_states;
        }
        else // temperature sensor
        {
            S12AD.ADSSTR23.BIT.SST2 = p_sample->num_states;
        }
    break;


    case ADC_CMD_ENABLE_CHANS:
        err = adc_enable_chans_63x((adc_ch_cfg_t *) p_args);
    break;


    case ADC_CMD_ENABLE_TEMP_SENSOR:
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
        if (g_dcb.mode != ADC_MODE_SS_TEMPERATURE)
        {
            return ADC_ERR_ILLEGAL_ARG;
        }
#endif
        S12AD.ADEXICR.BIT.TSS = 1;                  // select temperature sensor
        if (S12AD.ADADC.BIT.ADC != ADC_ADD_OFF)
        {
            S12AD.ADEXICR.BIT.TSSAD = 1;            // enable addition
        }
        TEMPS.TSCR.BIT.TSEN = 1;                    // enable sensor
        // NOTE: User application must wait 30us before doing a scan
        // TEMPS.TSCR.BIT.TSOE is set to 1 in ADC_CMD_ENABLE_TRIG and ADC_CMD_SCAN_NOW
        adc_enable_s12adi0();                       // setup interrupt handling
    break;


    case ADC_CMD_ENABLE_VOLT_SENSOR:
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
        if (g_dcb.mode != ADC_MODE_SS_INT_REF_VOLT)
        {
            return ADC_ERR_ILLEGAL_ARG;
        }
#endif
        S12AD.ADEXICR.BIT.OCS = 1;                  // select ref voltage sensor
        if (S12AD.ADADC.BIT.ADC != ADC_ADD_OFF)
        {
            S12AD.ADEXICR.BIT.OCSAD = 1;            // enable addition
        }
        adc_enable_s12adi0();                       // setup interrupt handling
    break;


    case ADC_CMD_ENABLE_TRIG:
        if (g_dcb.mode == ADC_MODE_SS_TEMPERATURE)
        {   // must be 30us after sensor enabled
            TEMPS.TSCR.BIT.TSOE = 1;        // enable output to ADC
        }
        S12AD.ADCSR.BIT.TRGE = 1;           // enable sync/async triggers
    break;


    case ADC_CMD_DISABLE_TRIG:
        S12AD.ADCSR.BIT.TRGE = 0;           // disable sync/async triggers
    break;


    case ADC_CMD_SCAN_NOW:
        if (S12AD.ADCSR.BIT.ADST == 0)
        {
           if (g_dcb.mode == ADC_MODE_SS_TEMPERATURE)
           {   // (technically only need to do once after 30us wait)
               TEMPS.TSCR.BIT.TSOE = 1;     // enable output to ADC
           }
           S12AD.ADCSR.BIT.ADST = 1;
        }
        else
        {
           err = ADC_ERR_SCAN_NOT_DONE;
        }
    break;


    case ADC_CMD_CHECK_SCAN_DONE:           // default/Group A or Group B
        if (S12AD.ADCSR.BIT.ADST == 1)
        {
            err = ADC_ERR_SCAN_NOT_DONE;
        }
    break;


    case ADC_CMD_ENABLE_INT:
        S12AD.ADCSR.BIT.ADIE = 1;           // enable S12ADI0 interrupt
    break;


    case ADC_CMD_DISABLE_INT:
        S12AD.ADCSR.BIT.ADIE = 0;           // disable S12ADI0 interrupt
    break;


    default:
        err = ADC_ERR_INVALID_ARG;
    break;
    }

    return err;
}


/******************************************************************************
* Function Name: adc_enable_chans_63x
* Description  : This function does extensive checking on channel mask
*                settings based upon operational mode. Mask registers are
*                initialized and interrupts enabled in peripheral. Interrupts
*                are also enabled in ICU if corresponding priority is not 0.
*
* NOTE: A negative number is stored in two's complement form.
*       A quick way to change a binary number into two's complement is to
*       start at the right (LSB) and moving left, don't change any bits
*       until after the first "1" is reached.
*       Number          2's complement
*       0010 0110       1101 1010
*       0000 0001       1111 1111
*       Another way is to do a 1's complement on the number, then add 1 to that.
*       Number          1's complement  + 1
*       0010 0110       1101 1001       1101 1010
*       0000 0001       1111 1110       1111 1111
*
* Arguments    : p_config
*                    Pointer to channel config structure containing masks
* Return Value : ADC_SUCCESS-
*                    Successful
*                ADC_ERR_INVALID_ARG-
*                    reg_id contains an invalid value.
*                ADC_ERR_ILLEGAL_ARG-
*                    an argument is illegal based upon mode
*******************************************************************************/
adc_err_t adc_enable_chans_63x(adc_ch_cfg_t *p_config)
{

#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
    uint32_t    tmp_mask=0;

    /* This command is illegal for sensor modes */
    if ((g_dcb.mode == ADC_MODE_SS_TEMPERATURE)
     || (g_dcb.mode == ADC_MODE_SS_INT_REF_VOLT))
    {
        return ADC_ERR_ILLEGAL_ARG;
    }

    /* Verify at least one bonded channel is selected */
    if ((p_config->chan_mask == 0)
     || ((p_config->chan_mask & ADC_INVALID_CH_MASK) != 0))
    {
        return ADC_ERR_INVALID_ARG;
    }

    /* Addition mask should not include bits from inactive channels */
    if (S12AD.ADADC.BIT.ADC != ADC_ADD_OFF)
    {
        // Bit-AND with 1s-complement
        if ((p_config->add_mask & ~p_config->chan_mask) != 0)
        {
            return ADC_ERR_INVALID_ARG;
        }
    }
    else
    {
        // WARNING! Other features messed up if add_mask is non-zero when addition is turned off!
        p_config->add_mask = 0;
    }

    /* Verify only 1 bit is set in mask */
    if ((g_dcb.mode == ADC_MODE_SS_ONE_CH)
     || (g_dcb.mode == ADC_MODE_CONT_ONE_CH))
    {
        tmp_mask = p_config->chan_mask;
        // Bit-AND with 2s-complement (see note in function header)
        if ((tmp_mask & (~tmp_mask + 1)) != tmp_mask)
        {
            return ADC_ERR_INVALID_ARG;
        }
    }
#endif

    /* SET MASKS FOR ALL CHANNELS */
    S12AD.ADANS0.WORD = (uint16_t) (p_config->chan_mask & 0xFFFF);
    S12AD.ADANS1.WORD = (uint16_t) ((p_config->chan_mask >> 16) & 0x001F);
    S12AD.ADADS0.WORD = (uint16_t) (p_config->add_mask & 0xFFFF);
    S12AD.ADADS1.WORD = (uint16_t) ((p_config->add_mask >> 16) & 0x001F);

    /* ENABLE INTERRUPTS */
    adc_enable_s12adi0();

    return ADC_SUCCESS;
}

#endif
