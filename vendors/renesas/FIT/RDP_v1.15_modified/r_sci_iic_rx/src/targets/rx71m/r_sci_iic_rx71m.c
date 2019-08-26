/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
 * No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
 * applicable laws, including copyright laws. 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO 
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the 
 * following link:
 * http://www.renesas.com/disclaimer 
 *
 * Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_sci_iic_rx71m.c
 * Description  : Functions for using SCI_IIC on RX devices. 
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.12.2014 1.00     First Release
 *         : 27.02.2015 1.60     Revised the source code to solve the following issue: 
 *                                 There are errors in the processing for the sci_iic_set_frequency function
 *                                 to set the clock source (CKS bit in the SMR register) and the bit rate
 *                                 (BRR register) for the on-chip baud rate generator, so the set values may
 *                                 differ from the expected values.
 *                                   This problem occurs when either of the following conditions is met 
 *                                   while rev.1.50 or an earlier version of the  SCI simple I2C mode FIT module
 *                                   is used.
 *                                    - Divided-by-3 is selected as the PLL input frequency division ratio
 *                                      (PLIDIV bit in the PLLCR register).
 *                                    - The tenth place of the PLL frequency multiplication factor is 5
 *                                      (STC bit in the PLLCR register).
 *                                 There are errors in the processing for the sci_iic_set_frequency function
 *                                 to set the clock source (CKS bit in the SMR register) and the bit rate
 *                                 (BRR register) for the on-chip baud rate generator, so the program may
 *                                 go into an infinite loop when the bit rate is set to low.
 *                                   This problem occurs when the following two conditions are met:
 *                                    - Rev.1.50 or an earlier version of the SCI simple I2C mode FIT module
 *                                      is used.
 *                                    - The BRR register value calculated by the sci_iic_set_frequency function
 *                                      is greater than 255. 
 *         : 04.03.2016 1.90     Changed about the pin definisions.
 *         : 01.10.2016 2.00     Fixed a program according to the Renesas coding rules.
 *         : 31.08.2017 2.20     Changed include path becuase changed file structure.
 *                               Changed about the calculation processing for address of PFS, PCR, PDR, ODR0, ODR1,
 *                                DSCR and PMR register.
 *                               Corrected the drive capacity control setting process by r_sci_iic_io_open() function.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
/* Access to peripherals and board defines. */
#include "platform.h"

#ifdef BSP_MCU_RX71M
/* Defines for SCI_IIC support */
    #include "r_sci_iic_rx_pin_config.h"
    #include "r_sci_iic_rx_private.h"
    #include "r_sci_iic_rx71m_private.h"
    #include "r_sci_iic_rx_if.h"
    #include "r_sci_iic_rx_config.h"

/***********************************************************************************************************************
 Private global variables and functions
 **********************************************************************************************************************/
static void sci_iic_set_frequency (sci_iic_info_t * p_sci_iic_info);
static void sci_iic_clear_ir_flag (sci_iic_info_t * p_sci_iic_info);

/***********************************************************************************************************************
 Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/
/*----------------------------------------------------------------------------*/
/*   Defines specific information for each SCI IIC channel.                   */
/*----------------------------------------------------------------------------*/
    #if SCI_IIC_CFG_CH0_INCLUDED
/* rom info */
const sci_iic_ch_rom_t g_sci_iic_ch0_rom =
{
    BSP_LOCK_SCI0,
    (sci_regs_t)&SCI0,
    &SYSTEM.MSTPCRB.LONG, BIT31_MASK,
    &ICU.IPR[IPR_SCI0_TXI0].BYTE,
    &ICU.IR[IR_SCI0_TXI0].BYTE,
    &ICU.GRPBL0.LONG, BIT0_MASK,
    &ICU.IER[IER_SCI0_TXI0].BYTE, BIT3_MASK,
    &ICU.GENBL0.LONG, BIT0_MASK,
    BSP_INT_SRC_BL0_SCI0_TEI0,
    sci_iic_int_sci_iic0_tei_isr,
    SCI_IIC_CFG_CH0_INT_PRIORITY,
    SCI_IIC_CFG_CH0_SSCL_GP,
    SCI_IIC_CFG_CH0_SSCL_PIN,
    SCI_IIC_MPC_SSCL0_ENABLE,
    SCI_IIC_CFG_CH0_SSDA_GP,
    SCI_IIC_CFG_CH0_SSDA_PIN,
    SCI_IIC_MPC_SSDA0_ENABLE,
    SCI_IIC_CFG_CH0_SSDA_DELAY_SELECT,
    SCI_IIC_CFG_CH0_BITRATE_BPS,
    SCI_IIC_CFG_CH0_DIGITAL_FILTER,
    SCI_IIC_CFG_CH0_FILTER_CLOCK
};

/* channel control block */
sci_iic_ch_ctrl_t g_sci_iic_ch0_ctrl =
{
    &g_sci_iic_ch0_rom,
    SCI_IIC_EV_INIT,
    SCI_IIC_MODE_NONE,
    SCI_IIC_STS_NO_INIT,
    SCI_IIC_STS_NO_INIT,
    (uint8_t)NULL,
    (sci_iic_info_t *)NULL
};
    #endif

    #if SCI_IIC_CFG_CH1_INCLUDED
/* rom info */
const sci_iic_ch_rom_t g_sci_iic_ch1_rom =
{
    BSP_LOCK_SCI1,
    (sci_regs_t)&SCI1,
    &SYSTEM.MSTPCRB.LONG, BIT30_MASK,
    &ICU.IPR[IPR_SCI1_TXI1].BYTE,
    &ICU.IR[IR_SCI1_TXI1].BYTE,
    &ICU.GRPBL0.LONG, BIT2_MASK,
    &ICU.IER[IER_SCI1_TXI1].BYTE, BIT5_MASK,
    &ICU.GENBL0.LONG, BIT2_MASK,
    BSP_INT_SRC_BL0_SCI1_TEI1,
    sci_iic_int_sci_iic1_tei_isr,
    SCI_IIC_CFG_CH1_INT_PRIORITY,
    SCI_IIC_CFG_CH1_SSCL_GP,
    SCI_IIC_CFG_CH1_SSCL_PIN,
    SCI_IIC_MPC_SSCL1_ENABLE,
    SCI_IIC_CFG_CH1_SSDA_GP,
    SCI_IIC_CFG_CH1_SSDA_PIN,
    SCI_IIC_MPC_SSDA1_ENABLE,
    SCI_IIC_CFG_CH1_SSDA_DELAY_SELECT,
    SCI_IIC_CFG_CH1_BITRATE_BPS,
    SCI_IIC_CFG_CH1_DIGITAL_FILTER,
    SCI_IIC_CFG_CH1_FILTER_CLOCK
};

/* channel control block */
sci_iic_ch_ctrl_t g_sci_iic_ch1_ctrl =
{
    &g_sci_iic_ch1_rom,
    SCI_IIC_EV_INIT,
    SCI_IIC_MODE_NONE,
    SCI_IIC_STS_NO_INIT,
    SCI_IIC_STS_NO_INIT,
    (uint8_t)NULL,
    (sci_iic_info_t *)NULL
};
    #endif

    #if SCI_IIC_CFG_CH2_INCLUDED
/* rom info */
const sci_iic_ch_rom_t g_sci_iic_ch2_rom =
{
    BSP_LOCK_SCI2,
    (sci_regs_t)&SCI2,
    &SYSTEM.MSTPCRB.LONG, BIT29_MASK,
    &ICU.IPR[IPR_SCI2_TXI2].BYTE,
    &ICU.IR[IR_SCI2_TXI2].BYTE,
    &ICU.GRPBL0.LONG, BIT4_MASK,
    &ICU.IER[IER_SCI2_TXI2].BYTE, BIT7_MASK,
    &ICU.GENBL0.LONG, BIT4_MASK,
    BSP_INT_SRC_BL0_SCI2_TEI2,
    sci_iic_int_sci_iic2_tei_isr,
    SCI_IIC_CFG_CH2_INT_PRIORITY,
    SCI_IIC_CFG_CH2_SSCL_GP,
    SCI_IIC_CFG_CH2_SSCL_PIN,
    SCI_IIC_MPC_SSCL2_ENABLE,
    SCI_IIC_CFG_CH2_SSDA_GP,
    SCI_IIC_CFG_CH2_SSDA_PIN,
    SCI_IIC_MPC_SSDA2_ENABLE,
    SCI_IIC_CFG_CH2_SSDA_DELAY_SELECT,
    SCI_IIC_CFG_CH2_BITRATE_BPS,
    SCI_IIC_CFG_CH2_DIGITAL_FILTER,
    SCI_IIC_CFG_CH2_FILTER_CLOCK
};

/* channel control block */
sci_iic_ch_ctrl_t g_sci_iic_ch2_ctrl =
{
    &g_sci_iic_ch2_rom,
    SCI_IIC_EV_INIT,
    SCI_IIC_MODE_NONE,
    SCI_IIC_STS_NO_INIT,
    SCI_IIC_STS_NO_INIT,
    (uint8_t)NULL,
    (sci_iic_info_t *)NULL
};
    #endif

    #if SCI_IIC_CFG_CH3_INCLUDED
/* rom info */
const sci_iic_ch_rom_t g_sci_iic_ch3_rom =
{
    BSP_LOCK_SCI3,
    (sci_regs_t)&SCI3,
    &SYSTEM.MSTPCRB.LONG, BIT28_MASK,
    &ICU.IPR[IPR_SCI3_TXI3].BYTE,
    &ICU.IR[IR_SCI3_TXI3].BYTE,
    &ICU.GRPBL0.LONG, BIT6_MASK,
    &ICU.IER[IER_SCI3_TXI3].BYTE, BIT1_MASK,
    &ICU.GENBL0.LONG, BIT6_MASK,
    BSP_INT_SRC_BL0_SCI3_TEI3,
    sci_iic_int_sci_iic3_tei_isr,
    SCI_IIC_CFG_CH3_INT_PRIORITY,
    SCI_IIC_CFG_CH3_SSCL_GP,
    SCI_IIC_CFG_CH3_SSCL_PIN,
    SCI_IIC_MPC_SSCL3_ENABLE,
    SCI_IIC_CFG_CH3_SSDA_GP,
    SCI_IIC_CFG_CH3_SSDA_PIN,
    SCI_IIC_MPC_SSDA3_ENABLE,
    SCI_IIC_CFG_CH3_SSDA_DELAY_SELECT,
    SCI_IIC_CFG_CH3_BITRATE_BPS,
    SCI_IIC_CFG_CH3_DIGITAL_FILTER,
    SCI_IIC_CFG_CH3_FILTER_CLOCK
};

/* channel control block */
sci_iic_ch_ctrl_t g_sci_iic_ch3_ctrl =
{
    &g_sci_iic_ch3_rom,
    SCI_IIC_EV_INIT,
    SCI_IIC_MODE_NONE,
    SCI_IIC_STS_NO_INIT,
    SCI_IIC_STS_NO_INIT,
    (uint8_t)NULL,
    (sci_iic_info_t *)NULL
};
    #endif

    #if SCI_IIC_CFG_CH4_INCLUDED
/* rom info */
const sci_iic_ch_rom_t g_sci_iic_ch4_rom =
{
    BSP_LOCK_SCI4,
    (sci_regs_t)&SCI4,
    &SYSTEM.MSTPCRB.LONG, BIT27_MASK,
    &ICU.IPR[IPR_SCI4_TXI4].BYTE,
    &ICU.IR[IR_SCI4_TXI4].BYTE,
    &ICU.GRPBL0.LONG, BIT8_MASK,
    &ICU.IER[IER_SCI4_TXI4].BYTE, BIT3_MASK,
    &ICU.GENBL0.LONG, BIT8_MASK,
    BSP_INT_SRC_BL0_SCI4_TEI4,
    sci_iic_int_sci_iic4_tei_isr,
    SCI_IIC_CFG_CH4_INT_PRIORITY,
    SCI_IIC_CFG_CH4_SSCL_GP,
    SCI_IIC_CFG_CH4_SSCL_PIN,
    SCI_IIC_MPC_SSCL4_ENABLE,
    SCI_IIC_CFG_CH4_SSDA_GP,
    SCI_IIC_CFG_CH4_SSDA_PIN,
    SCI_IIC_MPC_SSDA4_ENABLE,
    SCI_IIC_CFG_CH4_SSDA_DELAY_SELECT,
    SCI_IIC_CFG_CH4_BITRATE_BPS,
    SCI_IIC_CFG_CH4_DIGITAL_FILTER,
    SCI_IIC_CFG_CH4_FILTER_CLOCK
};

/* channel control block */
sci_iic_ch_ctrl_t g_sci_iic_ch4_ctrl =
{
    &g_sci_iic_ch4_rom,
    SCI_IIC_EV_INIT,
    SCI_IIC_MODE_NONE,
    SCI_IIC_STS_NO_INIT,
    SCI_IIC_STS_NO_INIT,
    (uint8_t)NULL,
    (sci_iic_info_t *)NULL
};
    #endif

    #if SCI_IIC_CFG_CH5_INCLUDED
/* rom info */
const sci_iic_ch_rom_t g_sci_iic_ch5_rom =
{
    BSP_LOCK_SCI5,
    (sci_regs_t)&SCI5,
    &SYSTEM.MSTPCRB.LONG, BIT26_MASK,
    &ICU.IPR[IPR_SCI5_TXI5].BYTE,
    &ICU.IR[IR_SCI5_TXI5].BYTE,
    &ICU.GRPBL0.LONG, BIT10_MASK,
    &ICU.IER[IER_SCI5_TXI5].BYTE, BIT5_MASK,
    &ICU.GENBL0.LONG, BIT10_MASK,
    BSP_INT_SRC_BL0_SCI5_TEI5,
    sci_iic_int_sci_iic5_tei_isr,
    SCI_IIC_CFG_CH5_INT_PRIORITY,
    SCI_IIC_CFG_CH5_SSCL_GP,
    SCI_IIC_CFG_CH5_SSCL_PIN,
    SCI_IIC_MPC_SSCL5_ENABLE,
    SCI_IIC_CFG_CH5_SSDA_GP,
    SCI_IIC_CFG_CH5_SSDA_PIN,
    SCI_IIC_MPC_SSDA5_ENABLE,
    SCI_IIC_CFG_CH5_SSDA_DELAY_SELECT,
    SCI_IIC_CFG_CH5_BITRATE_BPS,
    SCI_IIC_CFG_CH5_DIGITAL_FILTER,
    SCI_IIC_CFG_CH5_FILTER_CLOCK
};

/* channel control block */
sci_iic_ch_ctrl_t g_sci_iic_ch5_ctrl =
{
    &g_sci_iic_ch5_rom,
    SCI_IIC_EV_INIT,
    SCI_IIC_MODE_NONE,
    SCI_IIC_STS_NO_INIT,
    SCI_IIC_STS_NO_INIT,
    (uint8_t)NULL,
    (sci_iic_info_t *)NULL
};
    #endif

    #if SCI_IIC_CFG_CH6_INCLUDED
/* rom info */
const sci_iic_ch_rom_t g_sci_iic_ch6_rom =
{
    BSP_LOCK_SCI6,
    (sci_regs_t)&SCI6,
    &SYSTEM.MSTPCRB.LONG, BIT25_MASK,
    &ICU.IPR[IPR_SCI6_TXI6].BYTE,
    &ICU.IR[IR_SCI6_TXI6].BYTE,
    &ICU.GRPBL0.LONG, BIT12_MASK,
    &ICU.IER[IER_SCI6_TXI6].BYTE, BIT7_MASK,
    &ICU.GENBL0.LONG, BIT12_MASK,
    BSP_INT_SRC_BL0_SCI6_TEI6,
    sci_iic_int_sci_iic6_tei_isr,
    SCI_IIC_CFG_CH6_INT_PRIORITY,
    SCI_IIC_CFG_CH6_SSCL_GP,
    SCI_IIC_CFG_CH6_SSCL_PIN,
    SCI_IIC_MPC_SSCL6_ENABLE,
    SCI_IIC_CFG_CH6_SSDA_GP,
    SCI_IIC_CFG_CH6_SSDA_PIN,
    SCI_IIC_MPC_SSDA6_ENABLE,
    SCI_IIC_CFG_CH6_SSDA_DELAY_SELECT,
    SCI_IIC_CFG_CH6_BITRATE_BPS,
    SCI_IIC_CFG_CH6_DIGITAL_FILTER,
    SCI_IIC_CFG_CH6_FILTER_CLOCK
};

/* channel control block */
sci_iic_ch_ctrl_t g_sci_iic_ch6_ctrl =
{
    &g_sci_iic_ch6_rom,
    SCI_IIC_EV_INIT,
    SCI_IIC_MODE_NONE,
    SCI_IIC_STS_NO_INIT,
    SCI_IIC_STS_NO_INIT,
    (uint8_t)NULL,
    (sci_iic_info_t *)NULL
};
    #endif

    #if SCI_IIC_CFG_CH7_INCLUDED
/* rom info */
const sci_iic_ch_rom_t g_sci_iic_ch7_rom =
{
    BSP_LOCK_SCI7,
    (sci_regs_t)&SCI7,
    &SYSTEM.MSTPCRB.LONG, BIT24_MASK,
    &ICU.IPR[IPR_SCI7_TXI7].BYTE,
    &ICU.IR[IR_SCI7_TXI7].BYTE,
    &ICU.GRPBL0.LONG, BIT14_MASK,
    &ICU.IER[IER_SCI7_TXI7].BYTE, BIT3_MASK,
    &ICU.GENBL0.LONG, BIT14_MASK,
    BSP_INT_SRC_BL0_SCI7_TEI7,
    sci_iic_int_sci_iic7_tei_isr,
    SCI_IIC_CFG_CH7_INT_PRIORITY,
    SCI_IIC_CFG_CH7_SSCL_GP,
    SCI_IIC_CFG_CH7_SSCL_PIN,
    SCI_IIC_MPC_SSCL7_ENABLE,
    SCI_IIC_CFG_CH7_SSDA_GP,
    SCI_IIC_CFG_CH7_SSDA_PIN,
    SCI_IIC_MPC_SSDA7_ENABLE,
    SCI_IIC_CFG_CH7_SSDA_DELAY_SELECT,
    SCI_IIC_CFG_CH7_BITRATE_BPS,
    SCI_IIC_CFG_CH7_DIGITAL_FILTER,
    SCI_IIC_CFG_CH7_FILTER_CLOCK
};

/* channel control block */
sci_iic_ch_ctrl_t g_sci_iic_ch7_ctrl =
{
    &g_sci_iic_ch7_rom,
    SCI_IIC_EV_INIT,
    SCI_IIC_MODE_NONE,
    SCI_IIC_STS_NO_INIT,
    SCI_IIC_STS_NO_INIT,
    (uint8_t)NULL,
    (sci_iic_info_t *)NULL
};
    #endif

    #if SCI_IIC_CFG_CH12_INCLUDED
/* rom info */
const sci_iic_ch_rom_t g_sci_iic_ch12_rom =
{
    BSP_LOCK_SCI12,
    (sci_regs_t)&SCI12,
    &SYSTEM.MSTPCRB.LONG, BIT4_MASK,
    &ICU.IPR[IPR_SCI12_TXI12].BYTE,
    &ICU.IR[IR_SCI12_TXI12].BYTE,
    &ICU.GRPBL0.LONG, BIT16_MASK,
    &ICU.IER[IER_SCI12_TXI12].BYTE, BIT5_MASK,
    &ICU.GENBL0.LONG, BIT16_MASK,
    BSP_INT_SRC_BL0_SCI12_TEI12,
    sci_iic_int_sci_iic12_tei_isr,
    SCI_IIC_CFG_CH12_INT_PRIORITY,
    SCI_IIC_CFG_CH12_SSCL_GP,
    SCI_IIC_CFG_CH12_SSCL_PIN,
    SCI_IIC_MPC_SSCL12_ENABLE,
    SCI_IIC_CFG_CH12_SSDA_GP,
    SCI_IIC_CFG_CH12_SSDA_PIN,
    SCI_IIC_MPC_SSDA12_ENABLE,
    SCI_IIC_CFG_CH12_SSDA_DELAY_SELECT,
    SCI_IIC_CFG_CH12_BITRATE_BPS,
    SCI_IIC_CFG_CH12_DIGITAL_FILTER,
    SCI_IIC_CFG_CH12_FILTER_CLOCK
};

/* channel control block */
sci_iic_ch_ctrl_t g_sci_iic_ch12_ctrl =
{
    &g_sci_iic_ch12_rom,
    SCI_IIC_EV_INIT,
    SCI_IIC_MODE_NONE,
    SCI_IIC_STS_NO_INIT,
    SCI_IIC_STS_NO_INIT,
    (uint8_t)NULL,
    (sci_iic_info_t *)NULL
};
    #endif

/*----------------------------------------------------------------------------*/
/*   Array for start addresses of information for each SCI IIC channel        */
/*----------------------------------------------------------------------------*/
const sci_iic_hdl_t g_sci_iic_handles[SCI_IIC_NUM_CH_MAX] =
{
    #if SCI_IIC_CFG_CH0_INCLUDED
    &g_sci_iic_ch0_ctrl,
    #else
    NULL,
    #endif
    #if SCI_IIC_CFG_CH1_INCLUDED
    &g_sci_iic_ch1_ctrl,
    #else
    NULL,
    #endif
    #if SCI_IIC_CFG_CH2_INCLUDED
    &g_sci_iic_ch2_ctrl,
    #else
    NULL,
    #endif
    #if SCI_IIC_CFG_CH3_INCLUDED
    &g_sci_iic_ch3_ctrl,
    #else
    NULL,
    #endif
    #if SCI_IIC_CFG_CH4_INCLUDED
    &g_sci_iic_ch4_ctrl,
    #else
    NULL,
    #endif
    #if SCI_IIC_CFG_CH5_INCLUDED
    &g_sci_iic_ch5_ctrl,
    #else
    NULL,
    #endif
    #if SCI_IIC_CFG_CH6_INCLUDED
    &g_sci_iic_ch6_ctrl,
    #else
    NULL,
    #endif
    #if SCI_IIC_CFG_CH7_INCLUDED
    &g_sci_iic_ch7_ctrl,
    #else
    NULL,
    #endif
    NULL,
    NULL,
    NULL,
    NULL,
    #if SCI_IIC_CFG_CH12_INCLUDED
    &g_sci_iic_ch12_ctrl
    #else
    NULL
    #endif
};

/***********************************************************************************************************************
 * Function Name: r_sci_iic_check_arguments
 * Description  : check to parameter.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 *              ; sci_iic_api_mode_t called_api      ; Internal Mode
 * Return Value : none
 **********************************************************************************************************************/
sci_iic_return_t r_sci_iic_check_arguments (sci_iic_info_t * p_sci_iic_info, sci_iic_api_mode_t called_api)
{

    if (NULL == p_sci_iic_info)
    {
        return SCI_IIC_ERR_INVALID_ARG;
    }

    if (SCI_IIC_MODE_RECEIVE == called_api)
    {

        if (((NULL == p_sci_iic_info->p_slv_adr) || (NULL == p_sci_iic_info->p_data2nd))
                || (NULL == p_sci_iic_info->cnt2nd))
        {
            return SCI_IIC_ERR_INVALID_ARG;
        }

    }

    if ((SCI_IIC_MODE_SEND == called_api) || (SCI_IIC_MODE_RECEIVE == called_api))
    {

        if (NULL == p_sci_iic_info->callbackfunc)
        {
            return SCI_IIC_ERR_INVALID_ARG;
        }

    }

    if ((SCI_IIC_NUM_CH7 < p_sci_iic_info->ch_no) && (SCI_IIC_NUM_CH12 != p_sci_iic_info->ch_no))
    {
        return SCI_IIC_ERR_INVALID_CHAN;
    }

    return SCI_IIC_SUCCESS;
} /* End of function r_sci_iic_check_arguments() */

/***********************************************************************************************************************
 * Function Name: r_sci_iic_power_on
 * Description  : Turns on power to a SCI_IIC channel.
 * Arguments    : channel -
 *                    Which channel to use.
 * Return Value : none
 **********************************************************************************************************************/
void r_sci_iic_power_on (uint8_t channel)
{
    volatile __evenaccess const sci_iic_ch_rom_t * prom = g_sci_iic_handles[channel]->prom;

    /* Enable writing to MSTP registers. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);

    /* Enable selected SCI_IIC Channel. */
    /* Bring module out of stop state. */
    (*prom->pmstp) &= (~prom->stop_mask);

    /* Disable writing to MSTP registers. */
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);
} /* End of function r_sci_iic_power_on() */

/***********************************************************************************************************************
 * Function Name: r_sci_iic_power_off
 * Description  : Turns off power to a SCI_IIC channel.
 * Arguments    : channel -
 *                    Which channel to use.
 * Return Value : none
 **********************************************************************************************************************/
void r_sci_iic_power_off (uint8_t channel)
{
    volatile __evenaccess const sci_iic_ch_rom_t * prom = g_sci_iic_handles[channel]->prom;

    /* Enable writing to MSTP registers. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);

    /* Disable selected SCI_IIC Channel. */
    /* Put module in stop state. */
    (*prom->pmstp) |= prom->stop_mask;

    /* Disable writing to MSTP registers. */
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);
} /* End of function r_sci_iic_power_off() */

/***********************************************************************************************************************
 * Function Name: r_sci_iic_io_open
 * Description  : Open Port Processing
 *                Sets ports to input mode. Ports input pull-up becomes "Off".
 * Arguments    : uint8_t port_gr      ;   port group
 *                uint8_t pin_num      ;   pin number
 * Return Value : None
 **********************************************************************************************************************/
void r_sci_iic_io_open (uint8_t port_gr, uint8_t pin_num)
{
    volatile uint8_t __evenaccess * const ppcr = (uint8_t *)((uint32_t)SCI_IIC_PRV_PCR_BASE_REG + (uint32_t)port_gr);
    volatile uint8_t __evenaccess * const ppdr = (uint8_t *)((uint32_t)SCI_IIC_PRV_PDR_BASE_REG + (uint32_t)port_gr);
    volatile uint8_t __evenaccess * const podr0 = (uint8_t *)((uint32_t)SCI_IIC_PRV_ODR0_BASE_REG
                                                           + (uint32_t)(port_gr * 2));
    volatile uint8_t __evenaccess * const podr1 = (uint8_t *)((uint32_t)SCI_IIC_PRV_ODR1_BASE_REG
                                                           + (uint32_t)(port_gr * 2));
    volatile uint8_t __evenaccess * const pdscr = (uint8_t *)((uint32_t)SCI_IIC_PRV_DSCR_BASE_REG + (uint32_t)port_gr);

    /* Sets the port register */
    if (((PORT1_GR != port_gr) && (PORT3_GR != port_gr))
       && (PORTF_GR != port_gr))
    {
        (*pdscr) |= (1U << pin_num); /* drive capacity control : High-drive output */
    }
    (*ppcr) &= (~(1U << pin_num)); /* input pull-up resister : off */
    (*ppdr) &= (~(1U << pin_num)); /* input mode */

    /* Set N-channel open-drain output */
    if (pin_num <= 3)
    {
        (*podr0) |= (0x01 << (2 * pin_num));
    }
    else
    {
        (*podr1) |= (0x01 << (2 * (pin_num & 0x03)));
    }

    /* dummy read */
    if (*podr1)
    {
        R_NOP();
    }
} /* End of function r_sci_iic_io_open() */

/***********************************************************************************************************************
 * Function Name: r_sci_iic_init_io_register
 * Description  : Initializes SCI_IIC register.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
void r_sci_iic_init_io_register (sci_iic_info_t * p_sci_iic_info)
{
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;
    volatile __evenaccess const sci_iic_ch_rom_t * prom = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom;

    /* Sets SCLn and SDAn to non-driven state. */
    /* SCR - Serial Control Register 
     b5     TE - Transmit Enable - Serial transmission is enabled. */
    pregs->SCR.BYTE = SCI_IIC_SCR_INIT;

    /* Sets a transfer clock. */
    /* Includes I/O register read operation at the end of the following function. */
    sci_iic_set_frequency(p_sci_iic_info);

    /* SEMR - Serial Extended Mode Register
     b5    Digital Noise Filter Function Enable -
     Noise cancellation function for the SSCLn and SSDAn input signals enabled. */
    pregs->SEMR.BIT.NFEN = prom->df_sel;

    /* SNFR - Noise Filter Setting Register
     b7;b3   Reserved - These bits are read as 0.
     b2:b0   NFCS - The clock signal divided by 1 is used with the noise filter.
     b0      IICM  - Simple I2C Mode Select - Serial Interface Mode */
    pregs->SNFR.BIT.NFCS = prom->df_clk;

    /* Resets all SCI_IIC registers and the internal status. */
    /* SIMR1 - I2C Mode Register 1
     b7;b3   IICDL - SSDA Delay Output Select
     b2:b1   Reserved - These bits are read as 0.
     b0      IICM  - Simple I2C Mode Select - Serial Interface Mode */
    pregs->SIMR1.BIT.IICM = 0; /* Do not use Simple IIC mode. */

    /* SIMR1 - I2C Mode Register 1
     b7:b3  IICDL - SSDA Delay Output Select - 2 to 3 cycles */
    pregs->SIMR1.BIT.IICDL = prom->ssda_delay;

    /* SIMR2 - I2C Mode Register 2
     b5      IICACKT - ACK Transmission Data - NACK transmission and reception of ACK/NACK */
    pregs->SIMR2.BIT.IICACKT = SCI_IIC_NACK_TRANS;

    /* SIMR2 - I2C Mode Register 2
     b1      IICCSC - Clock Synchronization - Enable Clock Synchronization */
    pregs->SIMR2.BIT.IICCSC = SCI_IIC_SYNCHRO;

    /* SIMR2 - I2C Mode Register 2
     b0      IICINTM - I2C Interrupt Mode Select - Use reception and transmission interrupts. */
    pregs->SIMR2.BIT.IICINTM = SCI_IIC_RCV_TRS_INTERRUPT;

    /* SISR - I2C Status Register
     b0      IICACKR - ACK Reception Data Flag - ACK received */
    pregs->SISR.BIT.IICACKR = SCI_IIC_ACK_RCV;

    /* SPMR - SPI Mode Register
     b7      CKPH   - Clock Phase Select - Clock is not delayed.
     b6      CKPOL  - Clock Polarity Select - Clock polarity is not inverted.
     b5      Reserved - These bits are read as 0.
     b4      MFF    - Mode Fault Flag - No mode-fault error.
     b3      Reserved - These bits are read as 0.
     b2      MSS - Master Slave Select - master mode.
     b1      CTSE - CTS Enable - CTS function is disabled (RTS output function is enabled).
     b0      SSE - SS Pin Function Enable - SS pin function is disabled. */
    pregs->SPMR.BYTE = 0x00;
} /* End of function r_sci_iic_init_io_register() */

/***********************************************************************************************************************
 * Function Name: r_sci_iic_clear_io_register
 * Description  : Reset SCI I/O register.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
void r_sci_iic_clear_io_register (sci_iic_info_t * p_sci_iic_info)
{
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;

    /* Initializes SCI I/O register */
    pregs->SCR.BYTE = SCI_IIC_SCR_INIT;
    pregs->SMR.BYTE = SCI_IIC_SMR_INIT;
    pregs->SCMR.BYTE = SCI_IIC_SCMR_INIT;
    pregs->BRR = SCI_IIC_BRR_INIT;
    pregs->SEMR.BYTE = SCI_IIC_SEMR_INIT;
    pregs->SNFR.BYTE = SCI_IIC_SNFR_INIT;
    pregs->SIMR1.BYTE = SCI_IIC_SIMR1_INIT;
    pregs->SIMR2.BYTE = SCI_IIC_SIMR2_INIT;
    pregs->SIMR3.BYTE = SCI_IIC_SIMR3_INIT;
    pregs->SISR.BYTE = SCI_IIC_SISR_INIT;
    pregs->SPMR.BYTE = SCI_IIC_SPMR_INIT;
} /* End of function r_sci_iic_clear_io_register() */

/***********************************************************************************************************************
 * Function Name: r_sci_iic_int_disable
 * Description  : Disables interrupt.
 *              : Sets interrupt source priority.
 *              : Clears interrupt request register.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
void r_sci_iic_int_disable (sci_iic_info_t * p_sci_iic_info)
{
    volatile __evenaccess const sci_iic_ch_rom_t * prom = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom;
    uint32_t ipl;

    /* TEI interrupt set to callback function of group interrupt in bsp */
    R_BSP_InterruptWrite(prom->grp_tei_def, (bsp_int_cb_t) (FIT_NO_FUNC));

    /* Disables interrupt. */
    /* Disables TXI interrupt request enable register. */
    (*prom->picu_txi) &= (~prom->txi_en_mask);

    /* Disables TEI (in Group BL0) interrupt request enable register. */
    (*prom->picu_tei) &= (~prom->tei_en_mask);

    /* Clears interrupt source priority. */
    (*prom->pipr) = 0; /* Clears TXI interrupt source priority register. */

    /* dummy read */
    if (*prom->pipr)
    {
        R_NOP();
    }

    /* Disables GROUPBL0 interrupt request enable register. */
    ipl = 0; /* Clears TEI interrupt source priority register. */
    R_BSP_InterruptControl(prom->grp_tei_def, BSP_INT_CMD_GROUP_INTERRUPT_DISABLE, (void *) &ipl);

    /* Clears the interrupt request register. */
    /* Includes I/O register read operation at the end of the following function. */
    sci_iic_clear_ir_flag(p_sci_iic_info);
} /* End of function r_sci_iic_int_disable() */

/***********************************************************************************************************************
 * Function Name: r_sci_iic_int_enable
 * Description  : Clears interrupt request register.
 *              : Enables interrupt.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
void r_sci_iic_int_enable (sci_iic_info_t * p_sci_iic_info)
{
    volatile __evenaccess const sci_iic_ch_rom_t * prom = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom;
    uint32_t ipl;

    /* Clears the interrupt request register. */
    sci_iic_clear_ir_flag(p_sci_iic_info);

    /* TEI interrupt set to callback function of group interrupt in bsp */
    R_BSP_InterruptWrite(prom->grp_tei_def, (bsp_int_cb_t) (*prom->grp_tei_func));

    /* Enables interrupt. */
    /* Enables TXI interrupt request enable register. */
    (*prom->picu_txi) |= prom->txi_en_mask;

    /* Enables TEI (in Group BL0) interrupt request enable register. */
    (*prom->picu_tei) |= prom->tei_en_mask;

    /* Sets interrupt source priority. */
    (*prom->pipr) = prom->ipr_set_val; /* Sets TXI interrupt source priority register. */

    /* dummy read */
    if (*prom->pipr)
    {
        R_NOP();
    }

    /* Enables GROUPBL0 interrupt request enable register. */
    ipl = prom->ipr_set_val; /* Sets TEI interrupt source priority register. */
    R_BSP_InterruptControl(prom->grp_tei_def, BSP_INT_CMD_GROUP_INTERRUPT_ENABLE, (void *) &ipl);
} /* End of function r_sci_iic_int_enable() */

/***********************************************************************************************************************
 * Function Name: sci_iic_set_frequency
 * Description  : Set IIC Frequency Processing.
 *                Sets SMR,BRR registers.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_set_frequency (sci_iic_info_t * p_sci_iic_info)
{
    volatile uint16_t brr_n = 32U; /* default: 64*2^(2*0-1) = 32 */
    volatile uint8_t cks_value = 0U; /* default: PCLK/1 */
    volatile uint32_t brr_value = 0U;

    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;
    volatile __evenaccess const sci_iic_ch_rom_t * prom = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom;

    /*  ---- Calculation BRR ---- */
    /* Macro definition does not have the type of declaration. The cast for the calculation of the floating point, 
     has been cast in the double type. After that, it is cast to type long to fit on the left-hand side. 
     Calculation results are not affected by casting, it is no problem. */
    brr_value = (uint32_t) ((double) ((double) ((double) BSP_PCLKB_HZ / (brr_n * (prom->bitrate)))) - 0.1);

    /* Set clock source */
    while (brr_value > 255)
    {
        /* Counter over 0xff */
        switch (brr_n)
        {
            case 32 :
                brr_n = 128; /* 64*(2^(2*1-1)) */
                cks_value = 1; /* clock select: PCLK/4 */
            break;

            case 128 :
                brr_n = 512; /* 64*(2^(2*2-1))  */
                cks_value = 2; /* clock select: PCLK/16 */
            break;

            case 512 :
                brr_n = 2048; /* 64*(2^(2*3-1)) */
                cks_value = 3; /* clock select: PCLK/64 */
            break;

            default :

                /* nothing to do */
            break;
        }

        /*  ---- Calculation BRR ---- */
        /* Macro definition does not have the type of declaration. The cast for the calculation of the floating point, 
         has been cast in the double type. After that, it is cast to type long to fit on the left-hand side. 
         Calculation results are not affected by casting, it is no problem. */
        brr_value = (uint32_t) ((double) (((double) BSP_PCLKB_HZ / (brr_n * (prom->bitrate)))) - 0.1);

        /* When the clock source of the on-chip baud rate generator is PCLK/64 and when the value
         of brr_value is greater than 255. */
        if ((3 == cks_value) && (255 < brr_value))
        {
            brr_value = 255;
        }
    }

    pregs->SMR.BYTE |= cks_value; /* Sets SMR */
    pregs->BRR = brr_value; /* Sets BRR */
} /* End of function sci_iic_set_frequency() */

/***********************************************************************************************************************
 * Function Name: r_sci_iic_mpc_setting
 * Description  : Set SCI_IIC multi-function pin controller.
 * Arguments    : uint8_t port_gr      ;   port group
 *                uint8_t pin_num      ;   pin number
 *                uint8_t set_value    ;   value of pin function select setting
 * Return Value : None
 **********************************************************************************************************************/
void r_sci_iic_mpc_setting (uint8_t port_gr, uint8_t pin_num, uint8_t set_value)
{
    volatile uint8_t __evenaccess * const ppmr = (uint8_t *)((uint32_t)SCI_IIC_PRV_PMR_BASE_REG + (uint32_t)port_gr);
    volatile uint8_t __evenaccess * const ppfs = (uint8_t *)((uint32_t)SCI_IIC_PRV_PFS_BASE_REG
                                                           + (uint32_t)((port_gr * 8) + pin_num));

    if ((*ppfs) != set_value)
    {
        (*ppmr) &= (~(1U << pin_num)); /* Uses as a GPIO (Input port). */
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC); /* Enables the PFS register writing. */
        (*ppfs) = set_value; /* Pin function select to "SSCL/SSDA" or "Hi-Z" pin. */
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC); /* Disables the PFS register writing. */

        /* Verifies "set_value" (setting value for the PFS register). */
        /* If "set_value" is a value other than 0 (default), sets the pin mode control bit to be used 
         as I/O port for peripheral functions. */
        if (SCI_IIC_MPC_SSCL_INIT != set_value)
        {
            (*ppmr) |= (1U << pin_num); /* Uses as SCI_IIC (SSCL/SSDA). */
        }

        /* dummy read */
        if (*ppmr)
        {
            R_NOP();
        }
    }
} /* End of function r_sci_iic_mpc_setting() */

/***********************************************************************************************************************
 * Function Name: sci_iic_clear_ir_flag
 * Description  : Clears Interrupt Request Flag Processing.
 *                Clears interrupt request register.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_clear_ir_flag (sci_iic_info_t * p_sci_iic_info)
{
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;
    volatile __evenaccess const sci_iic_ch_rom_t * prom = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom;

    /* Checks IR flag. */
    /* If IR flag is set, clears IR flag. */
    if (SCI_IIC_IR_SET == (*prom->pir_txi))
    {
        /* Initializes ICIER->SCR register. */
        pregs->SCR.BYTE = SCI_IIC_SCR_INIT;

        while (SCI_IIC_SCR_INIT != pregs->SCR.BYTE)
        {
            /* nothing to do */
        }

        /* Clears TXI1 interrupt request register. */
        (*prom->pir_txi) = SCI_IIC_IR_CLR;

        /* dummy read */
        if (*prom->pir_txi)
        {
            R_NOP();
        }

        /* Re-initializes SCI_IIC register because cleared ICCR1.ICE bit. */
        /* Includes I/O register read operation at the end of the following function. */
        r_sci_iic_init_io_register(p_sci_iic_info);
    }
} /* End of function sci_iic_clear_ir_flag() */

    #if SCI_IIC_CFG_CH0_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic0_txi_isr
 * Description  : Interrupt TXI handler for channel 0.
 *                Types of interrupt requests transmission end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
R_PRAGMA_STATIC_INTERRUPT(sci_iic_int_sci_iic0_txi_isr, VECT_SCI0_TXI0)
R_ATTRIB_STATIC_INTERRUPT void sci_iic_int_sci_iic0_txi_isr (void)
{
    r_sci_iic_txi_isr_processing(SCI_IIC_NUM_CH0);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH0]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic0_txi_isr() */
    #endif

    #if SCI_IIC_CFG_CH1_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic1_txi_isr
 * Description  : Interrupt TXI handler for channel 1.
 *                Types of interrupt requests transmission end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
R_PRAGMA_STATIC_INTERRUPT(sci_iic_int_sci_iic1_txi_isr, VECT_SCI1_TXI1)
R_ATTRIB_STATIC_INTERRUPT void sci_iic_int_sci_iic1_txi_isr (void)
{
    r_sci_iic_txi_isr_processing(SCI_IIC_NUM_CH1);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH1]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic1_txi_isr() */
    #endif

    #if SCI_IIC_CFG_CH2_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic2_txi_isr
 * Description  : Interrupt TXI handler for channel 2.
 *                Types of interrupt requests transmission end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
R_PRAGMA_STATIC_INTERRUPT(sci_iic_int_sci_iic2_txi_isr, VECT_SCI2_TXI2)
R_ATTRIB_STATIC_INTERRUPT void sci_iic_int_sci_iic2_txi_isr (void)
{
    r_sci_iic_txi_isr_processing(SCI_IIC_NUM_CH2);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH2]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic2_txi_isr() */
    #endif

    #if SCI_IIC_CFG_CH3_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic3_txi_isr
 * Description  : Interrupt TXI handler for channel 3.
 *                Types of interrupt requests transmission end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
R_PRAGMA_STATIC_INTERRUPT(sci_iic_int_sci_iic3_txi_isr, VECT_SCI3_TXI3)
R_ATTRIB_STATIC_INTERRUPT void sci_iic_int_sci_iic3_txi_isr (void)
{
    r_sci_iic_txi_isr_processing(SCI_IIC_NUM_CH3);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH3]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic3_txi_isr() */
    #endif

    #if SCI_IIC_CFG_CH4_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic4_txi_isr
 * Description  : Interrupt TXI handler for channel 4.
 *                Types of interrupt requests transmission end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
R_PRAGMA_STATIC_INTERRUPT(sci_iic_int_sci_iic4_txi_isr, VECT_SCI4_TXI4)
R_ATTRIB_STATIC_INTERRUPT void sci_iic_int_sci_iic4_txi_isr (void)
{
    r_sci_iic_txi_isr_processing(SCI_IIC_NUM_CH4);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH4]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic4_txi_isr() */
    #endif

    #if SCI_IIC_CFG_CH5_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic5_txi_isr
 * Description  : Interrupt TXI handler for channel 5.
 *                Types of interrupt requests transmission end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
R_PRAGMA_STATIC_INTERRUPT(sci_iic_int_sci_iic5_txi_isr, VECT_SCI5_TXI5)
R_ATTRIB_STATIC_INTERRUPT void sci_iic_int_sci_iic5_txi_isr (void)
{
    r_sci_iic_txi_isr_processing(SCI_IIC_NUM_CH5);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH5]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic5_txi_isr() */
    #endif

    #if SCI_IIC_CFG_CH6_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic6_txi_isr
 * Description  : Interrupt TXI handler for channel 6.
 *                Types of interrupt requests transmission end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
R_PRAGMA_STATIC_INTERRUPT(sci_iic_int_sci_iic6_txi_isr, VECT_SCI6_TXI6)
R_ATTRIB_STATIC_INTERRUPT void sci_iic_int_sci_iic6_txi_isr (void)
{
    r_sci_iic_txi_isr_processing(SCI_IIC_NUM_CH6);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH6]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic6_txi_isr() */
    #endif

    #if SCI_IIC_CFG_CH7_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic7_txi_isr
 * Description  : Interrupt TXI handler for channel 7.
 *                Types of interrupt requests transmission end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
R_PRAGMA_STATIC_INTERRUPT(sci_iic_int_sci_iic7_txi_isr, VECT_SCI7_TXI7)
R_ATTRIB_STATIC_INTERRUPT void sci_iic_int_sci_iic7_txi_isr (void)
{
    r_sci_iic_txi_isr_processing(SCI_IIC_NUM_CH7);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH7]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic7_txi_isr() */
    #endif

    #if SCI_IIC_CFG_CH12_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic12_txi_isr
 * Description  : Interrupt TXI handler for channel 12.
 *                Types of interrupt requests transmission end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
R_PRAGMA_STATIC_INTERRUPT(sci_iic_int_sci_iic12_txi_isr, VECT_SCI12_TXI12)
R_ATTRIB_STATIC_INTERRUPT void sci_iic_int_sci_iic12_txi_isr (void)
{
    r_sci_iic_txi_isr_processing(SCI_IIC_NUM_CH12);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH12]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic12_txi_isr() */
    #endif

    #if SCI_IIC_CFG_CH0_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic0_tei_isr
 * Description  : Interrupt TEI handler for channel 0.
 *                Types of interrupt requests transfer error or event generation.
 *              : The event generations are arbitration-lost, NACK detection, timeout detection, 
 *              : start condition detection, and stop condition detection.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void sci_iic_int_sci_iic0_tei_isr (void)
{
    r_sci_iic_tei_isr_processing(SCI_IIC_NUM_CH0);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH0]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic0_tei_isr() */
    #endif

    #if SCI_IIC_CFG_CH1_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic1_tei_isr
 * Description  : Interrupt TEI handler for channel 1.
 *                Types of interrupt requests transfer error or event generation.
 *              : The event generations are arbitration-lost, NACK detection, timeout detection, 
 *              : start condition detection, and stop condition detection.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void sci_iic_int_sci_iic1_tei_isr (void)
{
    r_sci_iic_tei_isr_processing(SCI_IIC_NUM_CH1);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH1]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic1_tei_isr() */
    #endif

    #if SCI_IIC_CFG_CH2_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic2_tei_isr
 * Description  : Interrupt TEI handler for channel 2.
 *                Types of interrupt requests transfer error or event generation.
 *              : The event generations are arbitration-lost, NACK detection, timeout detection, 
 *              : start condition detection, and stop condition detection.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void sci_iic_int_sci_iic2_tei_isr (void)
{
    r_sci_iic_tei_isr_processing(SCI_IIC_NUM_CH2);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH2]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic2_tei_isr() */
    #endif

    #if SCI_IIC_CFG_CH3_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic3_tei_isr
 * Description  : Interrupt TEI handler for channel 3.
 *                Types of interrupt requests transfer error or event generation.
 *              : The event generations are arbitration-lost, NACK detection, timeout detection, 
 *              : start condition detection, and stop condition detection.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void sci_iic_int_sci_iic3_tei_isr (void)
{
    r_sci_iic_tei_isr_processing(SCI_IIC_NUM_CH3);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH3]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic3_tei_isr() */
    #endif

    #if SCI_IIC_CFG_CH4_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic4_tei_isr
 * Description  : Interrupt TEI handler for channel 4.
 *                Types of interrupt requests transfer error or event generation.
 *              : The event generations are arbitration-lost, NACK detection, timeout detection, 
 *              : start condition detection, and stop condition detection.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void sci_iic_int_sci_iic4_tei_isr (void)
{
    r_sci_iic_tei_isr_processing(SCI_IIC_NUM_CH4);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH4]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic4_tei_isr() */
    #endif

    #if SCI_IIC_CFG_CH5_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic5_tei_isr
 * Description  : Interrupt TEI handler for channel 5.
 *                Types of interrupt requests transfer error or event generation.
 *              : The event generations are arbitration-lost, NACK detection, timeout detection, 
 *              : start condition detection, and stop condition detection.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void sci_iic_int_sci_iic5_tei_isr (void)
{
    r_sci_iic_tei_isr_processing(SCI_IIC_NUM_CH5);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH5]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic5_tei_isr() */
    #endif

    #if SCI_IIC_CFG_CH6_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic6_tei_isr
 * Description  : Interrupt TEI handler for channel 6.
 *                Types of interrupt requests transfer error or event generation.
 *              : The event generations are arbitration-lost, NACK detection, timeout detection, 
 *              : start condition detection, and stop condition detection.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void sci_iic_int_sci_iic6_tei_isr (void)
{
    r_sci_iic_tei_isr_processing(SCI_IIC_NUM_CH6);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH6]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic6_tei_isr() */
    #endif

    #if SCI_IIC_CFG_CH7_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic7_tei_isr
 * Description  : Interrupt TEI handler for channel 7.
 *                Types of interrupt requests transfer error or event generation.
 *              : The event generations are arbitration-lost, NACK detection, timeout detection, 
 *              : start condition detection, and stop condition detection.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void sci_iic_int_sci_iic7_tei_isr (void)
{
    r_sci_iic_tei_isr_processing(SCI_IIC_NUM_CH7);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH7]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic7_tei_isr() */
    #endif

    #if SCI_IIC_CFG_CH12_INCLUDED
/***********************************************************************************************************************
 * Function Name: sci_iic_int_sci_iic12_tei_isr
 * Description  : Interrupt TEI handler for channel 12.
 *                Types of interrupt requests transfer error or event generation.
 *              : The event generations are arbitration-lost, NACK detection, timeout detection, 
 *              : start condition detection, and stop condition detection.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void sci_iic_int_sci_iic12_tei_isr (void)
{
    r_sci_iic_tei_isr_processing(SCI_IIC_NUM_CH12);

    /* Calls advance function */
    r_sci_iic_advance(g_sci_iic_handles[SCI_IIC_NUM_CH12]->psci_iic_info_ch);
} /* End of function sci_iic_int_sci_iic12_tei_isr() */
    #endif

#endif /* BSP_MCU_RX71M */

