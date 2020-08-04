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
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_s12ad_rx110.c
* Description  : ROM tables and support code for RX110 devices.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           22.02.2014 1.00    Initial Release.
*           05.06.2014 1.30    Fixed channel mask bug in adc_enable_chans()
*           01.10.2016 2.20    Define name commoditize for the same meaning but different name define.
*                              Delete parameter check by the enum value.
*           03.09.2018 3.00    Added the comment to while statement.
*           05.04.2019 4.00    Added support for GNUC and ICCRX.
*           22.11.2019 4.40    Added support for atomic control.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Includes board and MCU related header files. */
#include "platform.h"

#if defined (BSP_MCU_RX110)
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
/* In ROM. A/D Data Register pointers */

volatile uint16_t R_BSP_EVENACCESS_SFR * const  gp_dreg_ptrs[ADC_REG_MAX+1] =
                    {   &S12AD.ADDR0,       /* channel 0 */
                        &S12AD.ADDR1,       /* channel 1 */
                        &S12AD.ADDR2,       /* channel 2 */
                        &S12AD.ADDR3,
                        &S12AD.ADDR4,
                        NULL,
                        &S12AD.ADDR6,
                        NULL,
                        &S12AD.ADDR8,
                        &S12AD.ADDR9,
                        &S12AD.ADDR10,
                        &S12AD.ADDR11,
                        &S12AD.ADDR12,
                        &S12AD.ADDR13,
                        &S12AD.ADDR14,
                        &S12AD.ADDR15,
                        &S12AD.ADTSDR,    /* temperature sensor */
                        &S12AD.ADOCDR,    /* voltage sensor */
                        &S12AD.ADDBLDR,   /* double trigger register */
                    };


/* In ROM. Sample State (SST) Register pointers */

/* 8-bit register pointers */
volatile uint8_t R_BSP_EVENACCESS_SFR * const  gp_sreg_ptrs[ADC_SST_REG_MAX+1] =
                    {   &S12AD.ADSSTR0,     /* channel 0 */
                        &S12AD.ADSSTR1,     /* channel 1 */
                        &S12AD.ADSSTR2,     /* channel 2 */
                        &S12AD.ADSSTR3,
                        &S12AD.ADSSTR4,
                        &S12AD.ADSSTR6,
                        &S12AD.ADSSTRL,     /* channels 8-15 */
                        &S12AD.ADSSTRT,     /* temperature sensor */
                        &S12AD.ADSSTRO      /* voltage sensor */
                    };

extern adc_ctrl_t g_dcb;

static adc_err_t adc_enable_chans(adc_ch_cfg_t *p_config);


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
*                    Ignored for rx110/rx111. Pass 0 for this argument.
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

#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    bsp_int_ctrl_t int_ctrl;
#endif

#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
    if ((NULL == p_cfg) || (FIT_NO_PTR == p_cfg))
    {
        return ADC_ERR_MISSING_PTR;
    }

    /* Check for valid argument values */
    if (p_cfg->priority > BSP_MCU_IPL_MAX)  // priority is not enum value(can't check by a Compiler)
    {
        return ADC_ERR_INVALID_ARG;
    }

    /* If interrupt driven, must have callback function */
    if ((0 != p_cfg->priority)
    && ((NULL == p_callback) || (FIT_NO_FUNC == p_callback)))
    {
        return ADC_ERR_ILLEGAL_ARG;
    }

    /* In double trigger mode, SW and async triggers not allowed */
    if ((ADC_MODE_SS_ONE_CH_DBLTRIG == mode)
    && ((ADC_TRIG_SOFTWARE == p_cfg->trigger) || (ADC_TRIG_ASYNC_ADTRG == p_cfg->trigger)))
    {
        return ADC_ERR_ILLEGAL_ARG;
    }

    /* Group checking; only synchronous triggers allowed; must be unique */
    if ((ADC_MODE_SS_MULTI_CH_GROUPED == mode) || (ADC_MODE_SS_MULTI_CH_GROUPED_DBLTRIG_A == mode))
    {
        if ((ADC_TRIG_ASYNC_ADTRG == p_cfg->trigger)
        || (ADC_TRIG_ASYNC_ADTRG == p_cfg->trigger_groupb)
        || (p_cfg->trigger == p_cfg->trigger_groupb)
        || (ADC_TRIG_SOFTWARE == p_cfg->trigger)
        || (ADC_TRIG_SOFTWARE == p_cfg->trigger_groupb))
        {
            return ADC_ERR_ILLEGAL_ARG;
        }

        if (p_cfg->priority_groupb > BSP_MCU_IPL_MAX)
        {
            return ADC_ERR_INVALID_ARG;
        }

        if ((0 != p_cfg->priority_groupb)   // interrupt driven; must have callback func
        && ((NULL == p_callback) || (FIT_NO_FUNC == p_callback)))
        {
            return ADC_ERR_ILLEGAL_ARG;
        }
    }
#endif /* ADC_CFG_PARAM_CHECKING_ENABLE == 1 */

    if (true == g_dcb.opened)
    {
        return ADC_ERR_AD_NOT_CLOSED;
    }
    if (false == R_BSP_HardwareLock(BSP_LOCK_S12AD))
    {
        return ADC_ERR_AD_LOCKED;
    }

    /* APPLY POWER TO PERIPHERAL */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);

#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_DISABLE, &int_ctrl);
#endif
    MSTP(S12AD) = 0;
#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_ENABLE, &int_ctrl);
#endif

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

    S12AD.ADCSR.WORD = 0;
    S12AD.ADEXICR.WORD = 0;

    S12AD.ADANSA.WORD = 0;
    S12AD.ADANSB.WORD = 0;

    S12AD.ADADS.WORD = 0;

    S12AD.ADADC.BYTE = 0;

    S12AD.ADCER.WORD = 0;
    S12AD.ADSTRGR.WORD = 0;

    S12AD.ADSSTR0 = 0x14;
    S12AD.ADSSTR1 = 0x14;
    S12AD.ADSSTR2 = 0x14;
    S12AD.ADSSTR3 = 0x14;
    S12AD.ADSSTR4 = 0x14;
    S12AD.ADSSTR6 = 0x14;
    S12AD.ADSSTRL = 0x14;
    S12AD.ADSSTRT = 0x14;
    S12AD.ADSSTRO = 0x14;

    /* SET MODE RELATED REGISTER FIELDS */
    g_dcb.mode = mode;
    if ((ADC_MODE_SS_MULTI_CH_GROUPED == mode)
    || (ADC_MODE_SS_MULTI_CH_GROUPED_DBLTRIG_A == mode))
    {
        S12AD.ADCSR.BIT.ADCS = ADC_ADCS_GROUP_SCAN;
    }
    else
    {
        if ((ADC_MODE_CONT_ONE_CH == mode) || (ADC_MODE_CONT_MULTI_CH == mode))
        {
            S12AD.ADCSR.BIT.ADCS = ADC_ADCS_CONT_SCAN;
        }

        /* other modes have ADCS=0 */
    }

    if ((ADC_MODE_SS_ONE_CH_DBLTRIG == mode)
    || (ADC_MODE_SS_MULTI_CH_GROUPED_DBLTRIG_A == mode))
    {
        S12AD.ADCSR.BIT.DBLE = 1;                   // enable double trigger
    }

    /* SET TRIGGER AND INTERRUPT PRIORITY REGISTER FIELDS */
    if (ADC_TRIG_SOFTWARE != p_cfg->trigger)
    {
        S12AD.ADSTRGR.BIT.TRSA = p_cfg->trigger;
    }
    if (ADC_TRIG_ASYNC_ADTRG == p_cfg->trigger)
    {
        S12AD.ADCSR.BIT.EXTRG = 1;      // set ext trigger for async trigger
    }
    if (ADC_ADCS_GROUP_SCAN == S12AD.ADCSR.BIT.ADCS)
    {
        S12AD.ADSTRGR.BIT.TRSB = p_cfg->trigger_groupb;
        IPR(S12AD,GBADI) = p_cfg->priority_groupb;
    }
    IPR(S12AD,S12ADI0) = p_cfg->priority;

    /* SET REGISTER FIELDS FOR REMAINING PARAMETERS */
    S12AD.ADADC.BIT.ADC = p_cfg->add_cnt;

    /* Cast to match the register type */
    S12AD.ADCER.WORD = ((uint16_t)p_cfg->alignment | (uint16_t)p_cfg->clearing);
    S12AD.ADCSR.WORD |= p_cfg->conv_speed;

    /* SAVE CALLBACK FUNCTION POINTER */
    g_dcb.p_callback = p_callback;

    /* MARK DRIVER AS OPENED */
    g_dcb.opened = true;
    R_BSP_HardwareUnlock(BSP_LOCK_S12AD);

    return ADC_SUCCESS;
} /* End of function adc_open() */


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
*       with this restriction include ADANSA, ADANSB, ADADS, ADADC, ADSSTR,
*       ADEXICR, and some bits in ADCSR.
*       No runtime operational sequence checking of any kind is performed.
*
* Arguments    : unit -
*                    Ignored for rx110/rx111. Pass 0 for this argument.
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

            /* Cast to match variable type */
            p_sample = (adc_time_t *) p_args;
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
            if (p_sample->num_states < ADC_SST_CNT_MIN)
            {
                return ADC_ERR_INVALID_ARG;
            }
#endif
            *gp_sreg_ptrs[p_sample->reg_id] = p_sample->num_states;
            break;

        case ADC_CMD_ENABLE_CHANS:

            /* Cast to match the argument type */
            err = adc_enable_chans((adc_ch_cfg_t *) p_args);
            break;

        case ADC_CMD_ENABLE_TEMP_SENSOR:
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
            if (ADC_MODE_SS_TEMPERATURE != g_dcb.mode)
            {
                return ADC_ERR_ILLEGAL_ARG;
            }
#endif
            S12AD.ADEXICR.BIT.TSS = 1;                  // select temperature sensor
            if (ADC_ADD_OFF != S12AD.ADADC.BIT.ADC)
            {
                S12AD.ADEXICR.BIT.TSSAD = 1;            // enable addition
            }
            adc_enable_s12adi0();                       // setup interrupt handling
            break;

        case ADC_CMD_ENABLE_VOLT_SENSOR:
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
            if (ADC_MODE_SS_INT_REF_VOLT != g_dcb.mode)
            {
                return ADC_ERR_ILLEGAL_ARG;
            }
#endif
            S12AD.ADEXICR.BIT.OCS = 1;                  // select ref voltage sensor
            if (ADC_ADD_OFF != S12AD.ADADC.BIT.ADC)
            {
                S12AD.ADEXICR.BIT.OCSAD = 1;            // enable addition
            }
            adc_enable_s12adi0();                       // setup interrupt handling
            break;

        case ADC_CMD_ENABLE_TRIG:
            S12AD.ADCSR.BIT.TRGE = 1;           // enable sync/async triggers
            break;

        case ADC_CMD_DISABLE_TRIG:
            S12AD.ADCSR.BIT.TRGE = 0;           // disable sync/async triggers
            break;

        case ADC_CMD_SCAN_NOW:
            if (0 == S12AD.ADCSR.BIT.ADST)
            {
                S12AD.ADCSR.BIT.ADST = 1;
            }
            else
            {
                err = ADC_ERR_SCAN_NOT_DONE;
            }
            break;

        case ADC_CMD_CHECK_SCAN_DONE:           // default/Group A or Group B
            if (1 == S12AD.ADCSR.BIT.ADST)
            {
                err = ADC_ERR_SCAN_NOT_DONE;
            }
            break;

        case ADC_CMD_CHECK_SCAN_DONE_GROUPA:
            if (1 == ICU.IR[IR_S12AD_S12ADI0].BIT.IR)
            {
                ICU.IR[IR_S12AD_S12ADI0].BIT.IR = 0;
            }
            else
            {
                err = ADC_ERR_SCAN_NOT_DONE;
            }
            break;

        case ADC_CMD_CHECK_SCAN_DONE_GROUPB:
            if (1 == ICU.IR[IR_S12AD_GBADI].BIT.IR)
            {
                ICU.IR[IR_S12AD_GBADI].BIT.IR = 0;
            }
            else
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

        case ADC_CMD_ENABLE_INT_GROUPB:
            S12AD.ADCSR.BIT.GBADIE = 1;         // enable GBADI interrupt
            break;

        case ADC_CMD_DISABLE_INT_GROUPB:
            S12AD.ADCSR.BIT.GBADIE = 0;         // disable GBADI interrupt
            break;

        default:
            err = ADC_ERR_INVALID_ARG;
            break;
    }

    return err;
} /* End of function adc_control() */


/******************************************************************************
* Function Name: adc_enable_chans
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
static adc_err_t adc_enable_chans(adc_ch_cfg_t *p_config)
{
    uint16_t    i=0;
    uint32_t    tmp_mask=0;

#if ADC_CFG_PARAM_CHECKING_ENABLE == 1

    /* This command is illegal for sensor modes */
    if ((ADC_MODE_SS_TEMPERATURE == g_dcb.mode)
    || (ADC_MODE_SS_INT_REF_VOLT == g_dcb.mode))
    {
        return ADC_ERR_ILLEGAL_ARG;
    }

    /* Verify at least one bonded channel is selected */
    if ((0 == p_config->chan_mask)
    || (0 != (p_config->chan_mask & ADC_PRV_INVALID_CH_MASK)))
    {
        return ADC_ERR_INVALID_ARG;
    }

    /* Verify at least one unique bonded channel is selected for Group B */
    if (ADC_ADCS_GROUP_SCAN == S12AD.ADCSR.BIT.ADCS)
    {
        if ((0 == p_config->chan_mask_groupb)
        || (0 != (p_config->chan_mask_groupb & ADC_PRV_INVALID_CH_MASK)))
        {
            return ADC_ERR_INVALID_ARG;
        }
        else if (0 != (p_config->chan_mask & p_config->chan_mask_groupb))
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
        p_config->chan_mask_groupb = 0;         // for addition mask checking
    }

    /* Addition mask should not include bits from inactive channels */
    if (ADC_ADD_OFF != S12AD.ADADC.BIT.ADC)
    {
        tmp_mask |= p_config->chan_mask;        // tmp_mask is Group A and B combined

        /* Bit-AND with 1s-complement */
        if ((p_config->add_mask & (~tmp_mask)) != 0)
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
    if ((ADC_MODE_SS_ONE_CH == g_dcb.mode)
    || (ADC_MODE_CONT_ONE_CH == g_dcb.mode)
    || (1 == S12AD.ADCSR.BIT.DBLE))        // double trigger mode
    {
        tmp_mask = p_config->chan_mask;     // tmp_mask is non-Group/Group A chans

        /* Bit-AND with 2s-complement (see note in function header) */
        if ((tmp_mask & ((~tmp_mask) + 1)) != tmp_mask)
        {
            return ADC_ERR_INVALID_ARG;
        }
    }

#endif /* ADC_CFG_PARAM_CHECKING_ENABLE == 1 */

    /* SET MASKS FOR ALL CHANNELS */

    /* Cast to match the register type */
    S12AD.ADANSA.WORD = (uint16_t) (p_config->chan_mask & 0xFF5F);

    /* Cast to match the register type */
    S12AD.ADANSB.WORD = (uint16_t) (p_config->chan_mask_groupb & 0xFF5F);

    /* Cast to match the register type */
    S12AD.ADADS.WORD = (uint16_t) (p_config->add_mask & 0xFF5F);

    /* SET DOUBLE TRIGGER CHANNEL */
    if (1 == S12AD.ADCSR.BIT.DBLE)
    {
        tmp_mask = p_config->chan_mask;     // tmp_mask is non-Group/Group A chans

        /* WAIT_LOOP */
        while (tmp_mask >>= 1)              // determine bit/ch number
        {
            i++;
        }
        S12AD.ADCSR.BIT.DBLANS = i;
    }

    /* ENABLE INTERRUPTS */
    adc_enable_s12adi0();
    if (ADC_ADCS_GROUP_SCAN == S12AD.ADCSR.BIT.ADCS)
    {
        IR(S12AD,GBADI) = 0;                // clear flag
        S12AD.ADCSR.BIT.GBADIE = 1;         // enable in peripheral
        if (0 != ICU.IPR[IPR_S12AD_GBADI].BYTE)
        {
            R_BSP_InterruptRequestEnable(VECT(S12AD,GBADI));           // enable in ICU
        }
    }

    return ADC_SUCCESS;
} /* End of function adc_enable_chans() */

#endif /* defined BSP_MCU_RX110 */

