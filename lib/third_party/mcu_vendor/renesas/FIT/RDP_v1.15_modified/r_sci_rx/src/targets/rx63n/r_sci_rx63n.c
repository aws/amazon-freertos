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
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_sci_rx.c
* Description  : Functions for using SCI on the RX63N device.
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           17.06.2016 1.00    Initial Release.
*           19.12.2016 1.10    FIT_NO_PTR check added to NULL check.
*                              SCI_CMD_EN_TEI was changed to ineffective, because it is meaningless command.
***********************************************************************************************************************/

/*****************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "platform.h"

#include "r_sci_rx63n_private.h"

/*****************************************************************************
Typedef definitions
******************************************************************************/

/*****************************************************************************
Macro definitions
******************************************************************************/

/*****************************************************************************
Private global variables and functions
******************************************************************************/
#if SCI_CFG_CH0_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci0_txi0_isr, VECT(SCI0,TXI0))
R_PRAGMA_STATIC_INTERRUPT(sci0_rxi0_isr, VECT(SCI0,RXI0))
#if SCI_CFG_TEI_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci0_tei0_isr, VECT(SCI0,TEI0))
#endif
#endif

#if SCI_CFG_CH1_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci1_txi1_isr, VECT(SCI1,TXI1))
R_PRAGMA_STATIC_INTERRUPT(sci1_rxi1_isr, VECT(SCI1,RXI1))
#if SCI_CFG_TEI_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci1_tei1_isr, VECT(SCI1,TEI1))
#endif
#endif

#if SCI_CFG_CH2_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci2_txi2_isr, VECT(SCI2,TXI2))
R_PRAGMA_STATIC_INTERRUPT(sci2_rxi2_isr, VECT(SCI2,RXI2))
#if SCI_CFG_TEI_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci2_tei2_isr, VECT(SCI2,TEI2))
#endif
#endif

#if SCI_CFG_CH3_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci3_txi3_isr, VECT(SCI3,TXI3))
R_PRAGMA_STATIC_INTERRUPT(sci3_rxi3_isr, VECT(SCI3,RXI3))
#if SCI_CFG_TEI_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci3_tei3_isr, VECT(SCI3,TEI3))
#endif
#endif

#if SCI_CFG_CH4_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci4_txi4_isr, VECT(SCI4,TXI4))
R_PRAGMA_STATIC_INTERRUPT(sci4_rxi4_isr, VECT(SCI4,RXI4))
#if SCI_CFG_TEI_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci4_tei4_isr, VECT(SCI4,TEI4))
#endif
#endif

#if SCI_CFG_CH5_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci5_txi5_isr, VECT(SCI5,TXI5))
R_PRAGMA_STATIC_INTERRUPT(sci5_rxi5_isr, VECT(SCI5,RXI5))
#if SCI_CFG_TEI_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci5_tei5_isr, VECT(SCI5,TEI5))
#endif
#endif

#if SCI_CFG_CH6_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci6_txi6_isr, VECT(SCI6,TXI6))
R_PRAGMA_STATIC_INTERRUPT(sci6_rxi6_isr, VECT(SCI6,RXI6))
#if SCI_CFG_TEI_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci6_tei6_isr, VECT(SCI6,TEI6))
#endif
#endif

#if SCI_CFG_CH7_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci7_txi7_isr, VECT(SCI7,TXI7))
R_PRAGMA_STATIC_INTERRUPT(sci7_rxi7_isr, VECT(SCI7,RXI7))
#if SCI_CFG_TEI_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci7_tei7_isr, VECT(SCI7,TEI7))
#endif
#endif

#if SCI_CFG_CH8_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci8_txi8_isr, VECT(SCI8,TXI8))
R_PRAGMA_STATIC_INTERRUPT(sci8_rxi8_isr, VECT(SCI8,RXI8))
#if SCI_CFG_TEI_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci8_tei8_isr, VECT(SCI8,TEI8))
#endif
#endif

#if SCI_CFG_CH9_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci9_txi9_isr, VECT(SCI9,TXI9))
R_PRAGMA_STATIC_INTERRUPT(sci9_rxi9_isr, VECT(SCI9,RXI9))
#if SCI_CFG_TEI_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci9_tei9_isr, VECT(SCI9,TEI9))
#endif
#endif

#if SCI_CFG_CH10_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci10_txi10_isr, VECT(SCI10,TXI10))
R_PRAGMA_STATIC_INTERRUPT(sci10_rxi10_isr, VECT(SCI10,RXI10))
#if SCI_CFG_TEI_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci10_tei10_isr, VECT(SCI10,TEI10))
#endif
#endif

#if SCI_CFG_CH11_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci11_txi11_isr, VECT(SCI11,TXI11))
R_PRAGMA_STATIC_INTERRUPT(sci11_rxi11_isr, VECT(SCI11,RXI11))
#if SCI_CFG_TEI_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci11_tei11_isr, VECT(SCI11,TEI11))
#endif
#endif

#if SCI_CFG_CH12_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci12_txi12_isr, VECT(SCI12,TXI12))
R_PRAGMA_STATIC_INTERRUPT(sci12_rxi12_isr, VECT(SCI12,RXI12))
#if SCI_CFG_TEI_INCLUDED
R_PRAGMA_STATIC_INTERRUPT(sci12_tei12_isr, VECT(SCI12,TEI12))
#endif
#endif

R_PRAGMA_STATIC_INTERRUPT(icu_group12_isr, VECT(ICU,GROUP12))

extern const sci_hdl_t g_handles[SCI_NUM_CH];

/*****************************************************************************
* Function Name: sci_mcu_param_check
* Description  : This function parameters check on MCU.
*                 (channel range, interrupt priority, etc...)
* Arguments    : chan -
*                    channel to check
* Return Value : SCI_SUCCESS - 
*                    parameter check all successfully
*                SCI_ERR_BAD_CHAN -
*                    channel number invalid for part
*                SCI_ERR_INVALID_ARG -
*                    interrupt priority out of range
******************************************************************************/
sci_err_t sci_mcu_param_check(uint8_t const chan)
{
    /* channel range parameter check */
    if (SCI_NUM_CH <= chan)
    {
        return SCI_ERR_BAD_CHAN;
    }

    /* interrupt priority configuration parameter check */
    if ((1 > SCI_CFG_RXERR_PRIORITY) || (15 < SCI_CFG_RXERR_PRIORITY))
    {
        return SCI_ERR_INVALID_ARG;
    }

    return SCI_SUCCESS;
} /* End of function sci_mcu_param_check() */

/*****************************************************************************
* Function Name: sci_init_register
* Description  : This function initializes the register for SCI.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
void sci_init_register(sci_hdl_t const hdl)
{
    /* SCI transmit enable bit and receive enable bit check & disable */
    while ((0 != hdl->rom->regs->SCR.BIT.TE) || (0 != hdl->rom->regs->SCR.BIT.RE))
    {
        if (0 != hdl->rom->regs->SCR.BIT.TE)
        {
            hdl->rom->regs->SCR.BIT.TE = 0;  // transmit disable
        }
        if (0 != hdl->rom->regs->SCR.BIT.RE)
        {
            hdl->rom->regs->SCR.BIT.RE = 0;  // receive disable
        }
    }

    /* SMR register initialize */
    hdl->rom->regs->SMR.BYTE = 0x00;

    /* SCR register initialize */
    hdl->rom->regs->SCR.BYTE = 0x00;
    
    /* SSR register initialize */
    if (1 == SCI_SSR_ORER)
    {
        SCI_SSR_ORER = 0;
    }
    if (1 == SCI_SSR_PER)
    {
        SCI_SSR_PER = 0;
    }
    if (1 == SCI_SSR_FER)
    {
        SCI_SSR_FER = 0;
    }

    /* SCMR register initialize */
    hdl->rom->regs->SCMR.BIT.SMIF = 0;
    hdl->rom->regs->SCMR.BIT.SINV = 0;
    hdl->rom->regs->SCMR.BIT.SDIR = 0;

    /* BRR register initialize */
    hdl->rom->regs->BRR = 0xFF;

    /* SEMR register initialize */
    hdl->rom->regs->SEMR.BIT.ABCS    = 0;
    hdl->rom->regs->SEMR.BIT.NFEN    = 0;

    /* SNFR register initialize */
    hdl->rom->regs->SNFR.BYTE = 0;

    /* SPMR register initialize */
    hdl->rom->regs->SPMR.BIT.CTSE  = 0;
    hdl->rom->regs->SPMR.BIT.CKPOL = 0;
    hdl->rom->regs->SPMR.BIT.CKPH  = 0;

    return;
} /* End of function sci_init_register() */

/*****************************************************************************
* Function Name: sci_init_bit_rate
* Description  : This function determines the best possible settings for the
*                baud rate registers for the specified peripheral clock speed
*                and baud rate. Note that this does not guarantee a low bit 
*                error rate, just the best possible one. The bit rate error is
*                returned in .1% increments. If the hardware cannot support
*                the specified combination, a value of 1000 (100% error) is
*                returned.
*
* NOTE: The transmitter and receiver (TE and RE bits in SCR) must be disabled 
*       prior to calling this function.
*
*       The application must pause for 1 bit time after the BRR register
*       is loaded before transmitting/receiving to allow time for the clock
*       to settle. 
*
* Arguments    : hdl -
*                    Handle for channel (ptr to chan control block)
*                    NOTE: mode element must be already set
*                pclk -
*                    Peripheral clock speed; e.g. 24000000 for 24MHz
*                baud -
*                    Baud rate; 19200, 57600, 115200, etc.
* Return Value : bit error in .1% increments; e.g. 16 = 1.6% bit rate error
*                a value of 1000 denotes 100% error; no registers set
******************************************************************************/
int32_t sci_init_bit_rate(sci_hdl_t const  hdl,
                          uint32_t const   pclk,
                          uint32_t const   baud)
{
    uint32_t i;
    uint32_t num_divisors = 0;
    uint32_t ratio;
    uint32_t tmp;
    baud_divisor_t const *p_baud_info=NULL;

    uint32_t pclk_calc;
    int32_t  bit_err;

#if SCI_CFG_PARAM_CHECKING_ENABLE
    if ((0 == pclk) || (0 == baud))
    {
        return 1000;
    }
#endif

    /* SELECT PROPER TABLE BASED UPON MODE */
    if (hdl->mode == SCI_MODE_ASYNC)
    {
#if (SCI_CFG_ASYNC_INCLUDED)
        p_baud_info = async_baud;
        num_divisors = NUM_DIVISORS_ASYNC;
#endif
    }
    else
    {   /* SYNC or SSPI */
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
        p_baud_info = sync_baud;
        num_divisors = NUM_DIVISORS_SYNC;
#endif
    }

    /* FIND DIVISOR; table has associated ABCS and CKS values */
    /* BRR must be 255 or less */
    /* the "- 1" is ignored in some steps for approximations */
    /* BRR = (PCLK/(divisor * baud)) - 1 */
    /* BRR = (ratio / divisor) - 1 */
    ratio = pclk/baud;
    for(i=0; i < num_divisors; i++)
    {
        if (ratio < (uint32_t)(p_baud_info[i].divisor * 256))
        {
            break;
        }
    }

    /* RETURN IF BRR WILL BE >255 OR LESS THAN 0 */
    if (i == num_divisors)
    {
        return(1000);           // impossible baud rate requested; return 100% error
    }

    /* ROUND UP/DOWN BRR AND SET BAUD RATE RELATED REGISTERS */

    /* divide ratio by only half the divisor and see if odd number */
    tmp = ratio/((uint32_t)p_baud_info[i].divisor/2);
    /* if odd, "round up" by ignoring -1; divide by 2 again for rest of divisor */
    hdl->rom->regs->BRR = (uint8_t)((tmp & 0x01) ? (tmp/2) : ((tmp/2)-1));

    hdl->rom->regs->SEMR.BIT.ABCS = p_baud_info[i].abcs;
    hdl->rom->regs->SMR.BIT.CKS = p_baud_info[i].cks;

    /* CALCULATE AND RETURN BIT RATE ERROR */

    // back calculate what PCLK should be:
    // BRR = (PCLK/(divisor * baud)) - 1
    // (BRR+1) * divisor * baud = PCLK
    pclk_calc = (uint32_t)(hdl->rom->regs->BRR +1) * (uint32_t)p_baud_info[i].divisor * baud;

    /* error represented in 0.1% increments */
    bit_err = (int32_t)(((pclk - pclk_calc) * 1000L) / pclk_calc);
    return bit_err;
} /* End of function sci_init_bit_rate() */

/*****************************************************************************
* Function Name: sci_initialize_ints
* Description  : This function sets priority, clears flags, and sets 
*                interrupts in both the ICU and SCI peripheral. These include 
*                RXI, TXI, TEI, and ERI/GROUP12 interrupts.
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                priority -
*                    priority for interrupts
* Return Value : none
******************************************************************************/
void sci_initialize_ints(sci_hdl_t const hdl,
                         uint8_t const   priority)
{
    /* SET PRIORITY FOR INTERRUPTS */
    *hdl->rom->ipr = priority;
    if (SCI_CFG_RXERR_PRIORITY > IPR(ICU, GROUP12))
    {
        IPR(ICU,GROUP12) = SCI_CFG_RXERR_PRIORITY;   // RX63x ERI equivalent
    }

    /* DISABLE INTERRUPTS */
    DISABLE_ERI_INT;
    DISABLE_RXI_INT;
    DISABLE_TXI_INT;
    DISABLE_TEI_INT;

    /* CLEAR INTERRUPT FLAGS */
    *hdl->rom->ir_rxi = 0;
    *hdl->rom->ir_txi = 0;
    *hdl->rom->ir_tei = 0;

    /* ENABLE INTERRUPTS IN ICU */
    IEN(ICU,GROUP12) = 1;                            // enable RX63x rx err interrupts

    /* ENABLE ERI AND RXI INTERRUPTS REQUESTS */
    ENABLE_ERI_INT;
    ENABLE_RXI_INT;

    /* ENABLE INTERRUPTS IN SCI PERIPHERAL */
    /* Note: Enable interrupts after xcvr or will get "extra" interrupt */
    hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;    // enable TE, RE, TXI, and RXI/ERI

    return;    
} /* End of function sci_initialize_ints() */

/*****************************************************************************
* Function Name: sci_disable_ints
* Description  : This function disable interrupts in both the ICU and SCI 
*                peripheral. These include RXI, TXI, TEI, ERI, and group
*                interrupts.
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
void sci_disable_ints(sci_hdl_t const hdl)
{
    /* disable ICU interrupts */
    DISABLE_RXI_INT;
    DISABLE_TXI_INT;
    DISABLE_ERI_INT;
    DISABLE_TEI_INT;
    
    /* disable peripheral interrupts and xcvr (TE and RE) */
    hdl->rom->regs->SCR.BYTE = 0;

    return;
} /* End of function sci_disable_ints() */

#if (SCI_CFG_ASYNC_INCLUDED)
/*****************************************************************************
* sciN_txiN_isr 
*  
* Description  : TXI interrupt routines for every SCI channel
******************************************************************************/

#if SCI_CFG_CH0_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci0_txi0_isr(void)
{
    txi_handler(&ch0_ctrl);
} /* End of function sci0_txi0_isr() */
#endif

#if SCI_CFG_CH1_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci1_txi1_isr(void)
{
    txi_handler(&ch1_ctrl);
} /* End of function sci1_txi1_isr() */
#endif

#if SCI_CFG_CH2_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci2_txi2_isr(void)
{
    txi_handler(&ch2_ctrl);
} /* End of function sci2_txi2_isr() */
#endif

#if SCI_CFG_CH3_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci3_txi3_isr(void)
{
    txi_handler(&ch3_ctrl);
} /* End of function sci3_txi3_isr() */
#endif

#if SCI_CFG_CH4_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci4_txi4_isr(void)
{
    txi_handler(&ch4_ctrl);
} /* End of function sci4_txi4_isr() */
#endif

#if SCI_CFG_CH5_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci5_txi5_isr(void)
{
    txi_handler(&ch5_ctrl);
} /* End of function sci5_txi5_isr() */
#endif

#if SCI_CFG_CH6_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci6_txi6_isr(void)
{
    txi_handler(&ch6_ctrl);
} /* End of function sci6_txi6_isr() */
#endif

#if SCI_CFG_CH7_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci7_txi7_isr(void)
{
    txi_handler(&ch7_ctrl);
} /* End of function sci7_txi7_isr() */
#endif

#if SCI_CFG_CH8_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci8_txi8_isr(void)
{
    txi_handler(&ch8_ctrl);
} /* End of function sci8_txi8_isr() */
#endif

#if SCI_CFG_CH9_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci9_txi9_isr(void)
{
    txi_handler(&ch9_ctrl);
} /* End of function sci9_txi9_isr() */
#endif

#if SCI_CFG_CH10_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci10_txi10_isr(void)
{
    txi_handler(&ch10_ctrl);
} /* End of function sci10_txi10_isr() */
#endif

#if SCI_CFG_CH11_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci11_txi11_isr(void)
{
    txi_handler(&ch11_ctrl);
} /* End of function sci11_txi11_isr() */
#endif

#if SCI_CFG_CH12_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci12_txi12_isr(void)
{
    txi_handler(&ch12_ctrl);
} /* End of function sci12_txi12_isr() */
#endif

#endif

#if SCI_CFG_TEI_INCLUDED
/*****************************************************************************
* sciN_teiN_isr 
*  
* Description  : TEI interrupt routines for every SCI channel.
*                BSP gets main group interrupt, then vectors to/calls these
*                "interrupts"/callbacks.
******************************************************************************/

#if SCI_CFG_CH0_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci0_tei0_isr(void *cb_args)
{
    R_INTERNAL_NOT_USED(cb_args);
    tei_handler(&ch0_ctrl);
} /* End of function sci0_tei0_isr() */
#endif

#if SCI_CFG_CH1_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci1_tei1_isr(void *cb_args)
{
    R_INTERNAL_NOT_USED(cb_args);
    tei_handler(&ch1_ctrl);
} /* End of function sci1_tei1_isr() */
#endif

#if SCI_CFG_CH2_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci2_tei2_isr(void *cb_args)
{
    R_INTERNAL_NOT_USED(cb_args);
    tei_handler(&ch2_ctrl);
} /* End of function sci2_tei2_isr() */
#endif

#if SCI_CFG_CH3_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci3_tei3_isr(void *cb_args)
{
    R_INTERNAL_NOT_USED(cb_args);
    tei_handler(&ch3_ctrl);
} /* End of function sci3_tei3_isr() */
#endif

#if SCI_CFG_CH4_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci4_tei4_isr(void *cb_args)
{
    R_INTERNAL_NOT_USED(cb_args);
    tei_handler(&ch4_ctrl);
} /* End of function sci4_tei4_isr() */
#endif

#if SCI_CFG_CH5_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci5_tei5_isr(void *cb_args)
{
    R_INTERNAL_NOT_USED(cb_args);
    tei_handler(&ch5_ctrl);
} /* End of function sci5_tei5_isr() */
#endif

#if SCI_CFG_CH6_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci6_tei6_isr(void *cb_args)
{
    R_INTERNAL_NOT_USED(cb_args);
    tei_handler(&ch6_ctrl);
} /* End of function sci6_tei6_isr() */
#endif

#if SCI_CFG_CH7_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci7_tei7_isr(void *cb_args)
{
    R_INTERNAL_NOT_USED(cb_args);
    tei_handler(&ch7_ctrl);
} /* End of function sci7_tei7_isr() */
#endif

#if SCI_CFG_CH8_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci8_tei8_isr(void *cb_args)
{
    R_INTERNAL_NOT_USED(cb_args);
    tei_handler(&ch8_ctrl);
} /* End of function sci8_tei8_isr() */
#endif

#if SCI_CFG_CH9_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci9_tei9_isr(void *cb_args)
{
    R_INTERNAL_NOT_USED(cb_args);
    tei_handler(&ch9_ctrl);
} /* End of function sci9_tei9_isr() */
#endif

#if SCI_CFG_CH10_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci10_tei10_isr(void *cb_args)
{
    R_INTERNAL_NOT_USED(cb_args);
    tei_handler(&ch10_ctrl);
} /* End of function sci10_tei10_isr() */
#endif

#if SCI_CFG_CH11_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci11_tei11_isr(void *cb_args)
{
    R_INTERNAL_NOT_USED(cb_args);
    tei_handler(&ch11_ctrl);
} /* End of function sci11_tei11_isr() */
#endif

#if SCI_CFG_CH12_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci12_tei12_isr(void *cb_args)
{
    R_INTERNAL_NOT_USED(cb_args);
    tei_handler(&ch12_ctrl);
} /* End of function sci12_tei12_isr() */
#endif

#endif

/*****************************************************************************
* sciN_rxiN_isr
*  
* Description  : RXI interrupt routines for every SCI channel
******************************************************************************/

#if SCI_CFG_CH0_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci0_rxi0_isr(void)
{
    rxi_handler(&ch0_ctrl);
} /* End of function sci0_rxi0_isr() */
#endif

#if SCI_CFG_CH1_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci1_rxi1_isr(void)
{
    rxi_handler(&ch1_ctrl);
} /* End of function sci1_rxi1_isr() */
#endif

#if SCI_CFG_CH2_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci2_rxi2_isr(void)
{
    rxi_handler(&ch2_ctrl);
} /* End of function sci2_rxi2_isr() */
#endif

#if SCI_CFG_CH3_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci3_rxi3_isr(void)
{
    rxi_handler(&ch3_ctrl);
} /* End of function sci3_rxi3_isr() */
#endif

#if SCI_CFG_CH4_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci4_rxi4_isr(void)
{
    rxi_handler(&ch4_ctrl);
} /* End of function sci4_rxi4_isr() */
#endif

#if SCI_CFG_CH5_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci5_rxi5_isr(void)
{
    rxi_handler(&ch5_ctrl);
} /* End of function sci5_rxi5_isr() */
#endif

#if SCI_CFG_CH6_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci6_rxi6_isr(void)
{
    rxi_handler(&ch6_ctrl);
} /* End of function sci6_rxi6_isr() */
#endif

#if SCI_CFG_CH7_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci7_rxi7_isr(void)
{
    rxi_handler(&ch7_ctrl);
} /* End of function sci7_rxi7_isr() */
#endif

#if SCI_CFG_CH8_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci8_rxi8_isr(void)
{
    rxi_handler(&ch8_ctrl);
} /* End of function sci8_rxi8_isr() */
#endif

#if SCI_CFG_CH9_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci9_rxi9_isr(void)
{
    rxi_handler(&ch9_ctrl);
} /* End of function sci9_rxi9_isr() */
#endif

#if SCI_CFG_CH10_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci10_rxi10_isr(void)
{
    rxi_handler(&ch10_ctrl);
} /* End of function sci10_rxi10_isr() */
#endif

#if SCI_CFG_CH11_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci11_rxi11_isr(void)
{
    rxi_handler(&ch11_ctrl);
} /* End of function sci11_rxi11_isr() */
#endif

#if SCI_CFG_CH12_INCLUDED
R_ATTRIB_STATIC_INTERRUPT void sci12_rxi12_isr(void)
{
    rxi_handler(&ch12_ctrl);
} /* End of function sci12_rxi12_isr() */
#endif

/*****************************************************************************
* Function Name: icu_group12_isr
* Description  : SCI receiver error interrupt handler for all channels
* Arguments    : none
* Return Value : none
******************************************************************************/
R_ATTRIB_STATIC_INTERRUPT void icu_group12_isr(void)
{
    uint32_t        err;
    uint32_t        mask;
    uint8_t         i;

    /*
    * Get group12 error value. Bit 0 corresponds to ch0, bit 1 to ch1, etc.
    * If a bit is set, an error occurred on that corresponding channel.
    * Loop through each bit (channel), and if an error occurred on that
    * channel and it is enabled by this driver, process the error.
    */
    err = ICU.GRP[12].LONG;
    for (i=0,mask=1; i < SCI_NUM_CH; i++, mask<<=1)
    {
        if ((err & mask & SCI_CFG_CH_INCLUDED_MASK) != 0)
        {
            eri_handler(g_handles[i]);
        }
    }   
}

#if (SCI_CFG_ASYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_async_cmds
* Description  : This function configures non-standard UART hardware and
*                performs special software operations.
*
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                cmd -
*                    command to run
*                p_args -
*                    pointer argument(s) specific to command
* Return Value : SCI_SUCCESS -
*                    Command completed successfully.
*                SCI_ERR_NULL_PTR -
*                    p_args is NULL when required for cmd
*                SCI_ERR_INVALID_ARG -
*                    The cmd value or p_args contains an invalid value.
******************************************************************************/
sci_err_t sci_async_cmds(sci_hdl_t const hdl,
                         sci_cmd_t const cmd,
                         void            *p_args)
{
    sci_err_t   err=SCI_SUCCESS;
    int32_t     bit_err;
    uint32_t    slow_baud;

#if SCI_CFG_PARAM_CHECKING_ENABLE
    if (((NULL == p_args) || (FIT_NO_PTR == p_args))
     && ((SCI_CMD_TX_Q_BYTES_FREE == cmd) || (SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ == cmd)))
    {
        return SCI_ERR_NULL_PTR;
    }
#endif

    switch(cmd)
    {
    case (SCI_CMD_EN_NOISE_CANCEL):
        hdl->rom->regs->SCR.BYTE &= (uint8_t)(~SCI_EN_XCVR_MASK);
        SCI_SCR_DUMMY_READ;
        hdl->rom->regs->SEMR.BIT.NFEN = 1;      /* enable noise filter */
        hdl->rom->regs->SNFR.BYTE = 0;          /* clock divided by 1 (default) */
        SCI_IR_TXI_CLEAR;
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
    break;

    case (SCI_CMD_OUTPUT_BAUD_CLK):
        hdl->rom->regs->SCR.BYTE &= (uint8_t)(~SCI_EN_XCVR_MASK);
        SCI_SCR_DUMMY_READ;
        hdl->rom->regs->SCR.BIT.CKE = 0x01;     /* output baud clock on SCK pin */
        SCI_IR_TXI_CLEAR;
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
    break;

#if SCI_CFG_TEI_INCLUDED
    case (SCI_CMD_EN_TEI):  /* SCI_CMD_EN_TEI is obsolete command, but it exists only for compatibility with older version. */
    break;
#endif

    case (SCI_CMD_TX_Q_FLUSH):
        DISABLE_TXI_INT;
        R_BYTEQ_Flush(hdl->u_tx_data.que);
        ENABLE_TXI_INT;
    break;

    case (SCI_CMD_RX_Q_FLUSH):
        DISABLE_RXI_INT;
        R_BYTEQ_Flush(hdl->u_rx_data.que);
        ENABLE_RXI_INT;
    break;

    case (SCI_CMD_TX_Q_BYTES_FREE):
        R_BYTEQ_Unused(hdl->u_tx_data.que, (uint16_t *) p_args);
    break;

    case (SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ):
        R_BYTEQ_Used(hdl->u_rx_data.que, (uint16_t *) p_args);
    break;

    case (SCI_CMD_GENERATE_BREAK):

        /* flush transmit queue */
        DISABLE_TXI_INT;
        R_BYTEQ_Flush(hdl->u_tx_data.que);
        ENABLE_TXI_INT;

        /* NOTE: the following steps will abort anything being sent */

        /* set baud rate 1.5x slower */
        slow_baud = (hdl->baud_rate << 1) / 3;
        hdl->rom->regs->SCR.BYTE &= (uint8_t)(~SCI_EN_XCVR_MASK);
        SCI_SCR_DUMMY_READ;
        bit_err = sci_init_bit_rate(hdl, hdl->pclk_speed, slow_baud);
        SCI_IR_TXI_CLEAR;
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
        if (1000 == bit_err)
        {
            err = SCI_ERR_INVALID_ARG;
        }
        else
        {   
            /* transmit "0" and wait for completion */
            SCI_TDR(0);
            while (0 == hdl->rom->regs->SSR.BIT.TEND)
            {
                R_NOP();
            }

            /* restore original baud rate */
            hdl->rom->regs->SCR.BYTE &= (uint8_t)(~SCI_EN_XCVR_MASK);
            SCI_SCR_DUMMY_READ;
            sci_init_bit_rate(hdl, hdl->pclk_speed, hdl->baud_rate);
            SCI_IR_TXI_CLEAR;
            hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
        }
    break;

    default:
        err = SCI_ERR_INVALID_ARG;
    break;
    }

    return err;
} /* End of function sci_async_cmds() */
#endif

#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_sync_cmds
* Description  : This function performs special software operations specific
*                to the SSPI and SYNC protocols.
*
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                cmd -
*                    command to run
*                p_args -
*                    pointer argument(s) specific to command
* Return Value : SCI_SUCCESS -
*                    Command completed successfully.
*                SCI_ERR_NULL_PTR -
*                    p_args is NULL when required for cmd
*                SCI_ERR_INVALID_ARG -
*                    The cmd value or p_args contains an invalid value.
*                    May be due to mode channel is operating in.
******************************************************************************/
sci_err_t sci_sync_cmds(sci_hdl_t const hdl,
                        sci_cmd_t const cmd,
                        void            *p_args)
{
    sci_spi_mode_t  spi_mode;
    sci_cb_args_t   args;
    sci_err_t       err=SCI_SUCCESS;

    switch (cmd)
    {
    case (SCI_CMD_CHECK_XFER_DONE):
        if (hdl->tx_idle == false)
        {
            err = SCI_ERR_XFER_NOT_DONE;
        }
    break;

    case (SCI_CMD_XFER_LSB_FIRST):
        hdl->rom->regs->SCR.BYTE &= ~SCI_EN_XCVR_MASK;
        SCI_SCR_DUMMY_READ;
        hdl->rom->regs->SCMR.BIT.SDIR = 0;
        SCI_IR_TXI_CLEAR;
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
    break;

    case (SCI_CMD_XFER_MSB_FIRST):
        hdl->rom->regs->SCR.BYTE &= ~SCI_EN_XCVR_MASK;
        SCI_SCR_DUMMY_READ;
        hdl->rom->regs->SCMR.BIT.SDIR = 1;
        SCI_IR_TXI_CLEAR;
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
    break;

    case (SCI_CMD_INVERT_DATA):
        hdl->rom->regs->SCR.BYTE &= ~SCI_EN_XCVR_MASK;
        SCI_SCR_DUMMY_READ;
        hdl->rom->regs->SCMR.BIT.SINV ^= 1;
        SCI_IR_TXI_CLEAR;
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
    break;

    case (SCI_CMD_ABORT_XFER):

        /* disable receive interrupts in ICU and peripheral */
        DISABLE_RXI_INT;
        DISABLE_ERI_INT;

        hdl->rom->regs->SCR.BYTE &= ~(SCI_SCR_REI_MASK | SCI_SCR_RE_MASK | SCI_SCR_TE_MASK);

        hdl->tx_cnt = 0;
        hdl->tx_dummy = false;
        hdl->tx_idle = true;

        /* Do callback if available */
        if ((hdl->callback != NULL) && (hdl->callback != FIT_NO_FUNC))
        {
            args.hdl = hdl;
            args.event = SCI_EVT_XFER_ABORTED;
            hdl->callback((void *)&args);
        }

        *hdl->rom->ir_rxi = 0;                  /* clear rxi interrupt flag */

        ENABLE_ERI_INT;                         /* enable rx err interrupts in ICU */
        ENABLE_RXI_INT;                         /* enable receive interrupts in ICU */

        /* Enable receive interrupt in peripheral after rcvr or will get "extra" interrupt */
        hdl->rom->regs->SCR.BYTE |= SCI_SCR_RE_MASK | SCI_SCR_TE_MASK;
        hdl->rom->regs->SCR.BYTE |= SCI_SCR_REI_MASK;
    break;

    case (SCI_CMD_CHANGE_SPI_MODE):
#if SCI_CFG_PARAM_CHECKING_ENABLE
        spi_mode = *((sci_spi_mode_t *)p_args);

        if (hdl->mode != SCI_MODE_SSPI)
        {
            return SCI_ERR_INVALID_ARG;
        }
        if ((p_args == NULL) || (p_args == FIT_NO_PTR))
        {
            return SCI_ERR_NULL_PTR;
        }
        if ((spi_mode != SCI_SPI_MODE_0) && (spi_mode != SCI_SPI_MODE_1)
          && (spi_mode != SCI_SPI_MODE_2) && (spi_mode != SCI_SPI_MODE_3))
        {
            return SCI_ERR_INVALID_ARG;
        }
#endif
        hdl->rom->regs->SCR.BYTE &= ~SCI_EN_XCVR_MASK;
        SCI_SCR_DUMMY_READ;
        hdl->rom->regs->SPMR.BYTE &= 0x3F;      /* clear previous mode */
        hdl->rom->regs->SPMR.BYTE |= *((uint8_t *)p_args);
        SCI_IR_TXI_CLEAR;
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
    break;

    default:
        err = SCI_ERR_INVALID_ARG;
    break;
    }

    return err;
} /* End of function sci_sync_cmds() */
#endif

