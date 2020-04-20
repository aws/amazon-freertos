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
* Description  : Functions for using SCI on the RX110 device.
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           03.06.2016 1.00    Initial Release.
*           19.12.2016 1.10    FIT_NO_PTR check added to NULL check.
*                              SCI_CMD_EN_TEI was changed to ineffective, because it is meaningless command.
*           20.05.2019 3.00    Added support for GNUC and ICCRX.
***********************************************************************************************************************/

/*****************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "platform.h"

#include "r_sci_rx110_private.h"

/*****************************************************************************
Typedef definitions
******************************************************************************/

/*****************************************************************************
Macro definitions
******************************************************************************/

/*****************************************************************************
Private global variables and functions
******************************************************************************/


#if SCI_CFG_CH1_INCLUDED
R_BSP_PRAGMA_STATIC_INTERRUPT(sci1_txi1_isr, VECT(SCI1,TXI1))
R_BSP_ATTRIB_STATIC_INTERRUPT void sci1_txi1_isr(void);
R_BSP_PRAGMA_STATIC_INTERRUPT(sci1_rxi1_isr, VECT(SCI1,RXI1))
R_BSP_ATTRIB_STATIC_INTERRUPT void sci1_rxi1_isr(void);
R_BSP_PRAGMA_STATIC_INTERRUPT(sci1_tei1_isr, VECT(SCI1,TEI1))
R_BSP_ATTRIB_STATIC_INTERRUPT void sci1_tei1_isr(void);
R_BSP_PRAGMA_STATIC_INTERRUPT(sci1_eri1_isr, VECT(SCI1,ERI1))
R_BSP_ATTRIB_STATIC_INTERRUPT void sci1_eri1_isr(void);

#endif

#if SCI_CFG_CH5_INCLUDED
R_BSP_PRAGMA_STATIC_INTERRUPT(sci5_txi5_isr, VECT(SCI5,TXI5))
R_BSP_ATTRIB_STATIC_INTERRUPT void sci5_txi5_isr(void);
R_BSP_PRAGMA_STATIC_INTERRUPT(sci5_rxi5_isr, VECT(SCI5,RXI5))
R_BSP_ATTRIB_STATIC_INTERRUPT void sci5_rxi5_isr(void);
R_BSP_PRAGMA_STATIC_INTERRUPT(sci5_tei5_isr, VECT(SCI5,TEI5))
R_BSP_ATTRIB_STATIC_INTERRUPT void sci5_tei5_isr(void);
R_BSP_PRAGMA_STATIC_INTERRUPT(sci5_eri5_isr, VECT(SCI5,ERI5))
R_BSP_ATTRIB_STATIC_INTERRUPT void sci5_eri5_isr(void);

#endif

#if SCI_CFG_CH12_INCLUDED
R_BSP_PRAGMA_STATIC_INTERRUPT(sci12_txi12_isr, VECT(SCI12,TXI12))
R_BSP_ATTRIB_STATIC_INTERRUPT void sci12_txi12_isr(void);
R_BSP_PRAGMA_STATIC_INTERRUPT(sci12_rxi12_isr, VECT(SCI12,RXI12))
R_BSP_ATTRIB_STATIC_INTERRUPT void sci12_rxi12_isr(void);
R_BSP_PRAGMA_STATIC_INTERRUPT(sci12_tei12_isr, VECT(SCI12,TEI12))
R_BSP_ATTRIB_STATIC_INTERRUPT void sci12_tei12_isr(void);
R_BSP_PRAGMA_STATIC_INTERRUPT(sci12_eri12_isr, VECT(SCI12,ERI12))
R_BSP_ATTRIB_STATIC_INTERRUPT void sci12_eri12_isr(void);

#endif


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
    if ((chan != SCI_CH1) && (chan != SCI_CH5) && (chan != SCI_CH12))
    {
        return SCI_ERR_BAD_CHAN;
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
    hdl->rom->regs->SEMR.BIT.RXDESEL = 0;

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
*                RXI, TXI, TEI, and ERI interrupts.
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

    /* DISABLE INTERRUPTS */
    DISABLE_ERI_INT;
    DISABLE_RXI_INT;
    DISABLE_TXI_INT;
    DISABLE_TEI_INT;

    /* CLEAR INTERRUPT FLAGS */
    *hdl->rom->ir_rxi = 0;
    *hdl->rom->ir_txi = 0;
    *hdl->rom->ir_tei = 0;
    *hdl->rom->ir_eri = 0;

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
*                peripheral. These include RXI, TXI, TEI, and ERI interrupts.
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
* Description  : TXI interrupt routines for every SCI channel.
******************************************************************************/
#if SCI_CFG_CH1_INCLUDED
R_BSP_ATTRIB_STATIC_INTERRUPT void sci1_txi1_isr(void)
{
    txi_handler(&ch1_ctrl);
} /* End of function sci1_txi1_isr() */

#endif

#if SCI_CFG_CH5_INCLUDED
R_BSP_ATTRIB_STATIC_INTERRUPT void sci5_txi5_isr(void)
{
    txi_handler(&ch5_ctrl);
} /* End of function sci5_txi5_isr() */

#endif

#if SCI_CFG_CH12_INCLUDED
R_BSP_ATTRIB_STATIC_INTERRUPT void sci12_txi12_isr(void)
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
******************************************************************************/
#if SCI_CFG_CH1_INCLUDED
R_BSP_ATTRIB_STATIC_INTERRUPT void sci1_tei1_isr(void)
{
    tei_handler(&ch1_ctrl);
} /* End of function sci1_tei1_isr() */

#endif

#if SCI_CFG_CH5_INCLUDED
R_BSP_ATTRIB_STATIC_INTERRUPT void sci5_tei5_isr(void)
{
    tei_handler(&ch5_ctrl);
} /* End of function sci5_tei5_isr() */

#endif

#if SCI_CFG_CH12_INCLUDED
R_BSP_ATTRIB_STATIC_INTERRUPT void sci12_tei12_isr(void)
{
    tei_handler(&ch12_ctrl);
} /* End of function sci12_tei12_isr() */

#endif

#endif

/*****************************************************************************
* sciN_rxiN_isr
*  
* Description  : RXI interrupt routines for every SCI channel.
******************************************************************************/
#if SCI_CFG_CH1_INCLUDED
R_BSP_ATTRIB_STATIC_INTERRUPT void sci1_rxi1_isr(void)
{
    rxi_handler(&ch1_ctrl);
} /* End of function sci1_rxi1_isr() */

#endif

#if SCI_CFG_CH5_INCLUDED
R_BSP_ATTRIB_STATIC_INTERRUPT void sci5_rxi5_isr(void)
{
    rxi_handler(&ch5_ctrl);
} /* End of function sci5_rxi5_isr() */

#endif

#if SCI_CFG_CH12_INCLUDED
R_BSP_ATTRIB_STATIC_INTERRUPT void sci12_rxi12_isr(void)
{
    rxi_handler(&ch12_ctrl);
} /* End of function sci12_rxi12_isr() */

#endif

/*****************************************************************************
* sciN_eriN_isr 
*  
* Description  : ERI interrupt routines for every SCI channel.
******************************************************************************/

#if SCI_CFG_CH1_INCLUDED
R_BSP_ATTRIB_STATIC_INTERRUPT void sci1_eri1_isr(void)
{
    eri_handler(&ch1_ctrl);
} /* End of function sci1_eri1_isr() */

#endif

#if SCI_CFG_CH5_INCLUDED
R_BSP_ATTRIB_STATIC_INTERRUPT void sci5_eri5_isr(void)
{
    eri_handler(&ch5_ctrl);
} /* End of function sci5_eri5_isr() */
#endif

#if SCI_CFG_CH12_INCLUDED
R_BSP_ATTRIB_STATIC_INTERRUPT void sci12_eri12_isr(void)
{
    eri_handler(&ch12_ctrl);
} /* End of function sci12_eri12_isr() */

#endif

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
        hdl->rom->regs->SCR.BYTE &= (~SCI_EN_XCVR_MASK);
        SCI_SCR_DUMMY_READ;
        hdl->rom->regs->SEMR.BIT.NFEN = 1;      /* enable noise filter */
        hdl->rom->regs->SNFR.BYTE = 0;          /* clock divided by 1 (default) */
        SCI_IR_TXI_CLEAR;
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
    break;

    case (SCI_CMD_OUTPUT_BAUD_CLK):
        hdl->rom->regs->SCR.BYTE &= (~SCI_EN_XCVR_MASK);
        SCI_SCR_DUMMY_READ;
        hdl->rom->regs->SCR.BIT.CKE = 0x01;     /* output baud clock on SCK pin */
        SCI_IR_TXI_CLEAR;
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
    break;

    case (SCI_CMD_START_BIT_EDGE):
        hdl->rom->regs->SCR.BYTE &= (~SCI_EN_XCVR_MASK);
        SCI_SCR_DUMMY_READ;
        hdl->rom->regs->SEMR.BIT.RXDESEL = 1;   /* detect start bit on falling edge */
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
        hdl->rom->regs->SCR.BYTE &= (~SCI_EN_XCVR_MASK);
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
                R_BSP_NOP();
            }

            /* restore original baud rate */
            hdl->rom->regs->SCR.BYTE &= (~SCI_EN_XCVR_MASK);
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
        *hdl->rom->ir_eri = 0;                  /* clear eri interrupt flag */

        ENABLE_ERI_INT;                         /* enable rx err interrupts in ICU */
        ENABLE_RXI_INT;                         /* enable receive interrupts in ICU */

        /* Enable receive interrupt in peripheral after rcvr or will get "extra" interrupt */
        hdl->rom->regs->SCR.BYTE |= SCI_SCR_RE_MASK | SCI_SCR_TE_MASK;
        hdl->rom->regs->SCR.BYTE |= SCI_SCR_REI_MASK;
    break;

    case (SCI_CMD_CHANGE_SPI_MODE):
#if SCI_CFG_PARAM_CHECKING_ENABLE

        if (hdl->mode != SCI_MODE_SSPI)
        {
            return SCI_ERR_INVALID_ARG;
        }

        /* Check parameters */
        if ((p_args == NULL) || (p_args == FIT_NO_PTR))
        {
            return SCI_ERR_NULL_PTR;
        }

        /* Casting pointer void* type is valid */
        spi_mode = *((sci_spi_mode_t *)p_args);

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

