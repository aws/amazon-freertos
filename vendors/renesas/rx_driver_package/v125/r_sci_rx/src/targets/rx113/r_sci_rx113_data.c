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
* File Name    : r_sci_rx113_data.c
* Description  : Functions for using SCI on the RX113 device.
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           20.06.2016 1.00    Initial Release.
*           20.05.2019 3.00    Added support for GNUC and ICCRX.
***********************************************************************************************************************/

/*****************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "platform.h"

#include "r_sci_rx113_private.h"

/*****************************************************************************
Typedef definitions
******************************************************************************/

/*****************************************************************************
Macro definitions
******************************************************************************/

/*****************************************************************************
Private global variables and functions
******************************************************************************/

/* BAUD DIVISOR INFO */

#if (SCI_CFG_ASYNC_INCLUDED)
/*
 * Asynchronous
 * BRR = (PCLK/(divisor * baud)) - 1
 * when abcs=1, divisor = 32*pow(2,2n-1)
 * when abcs=0, divisor = 64*pow(2,2n-1)
 */
const baud_divisor_t async_baud[NUM_DIVISORS_ASYNC]=
{
    /* divisor result, abcs, n */
    {16,   1, 0},
    {32,   0, 0},
    {64,   1, 1},
    {128,  0, 1},
    {256,  1, 2},
    {512,  0, 2},
    {1024, 1, 3},
    {2048, 0, 3}
};
#endif

#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
/* Synchronous and Simple SPI */
/* BRR = (PCLK/(divisor * baud)) - 1 */
/* abcs=0, divisor = 8*pow(2,2n-1) */

const baud_divisor_t sync_baud[NUM_DIVISORS_SYNC]=
{
    /* divisor result, abcs, n */
    {4,   0, 0},
    {16,  0, 1},
    {64,  0, 2},
    {256, 0, 3}
};
#endif


/* CHANNEL MEMORY ALLOCATIONS */

#if SCI_CFG_CH0_INCLUDED

/* rom info */
const sci_ch_rom_t  ch0_rom = {(volatile struct st_sci12 R_BSP_EVENACCESS_SFR *)&SCI0,
                                (volatile uint32_t R_BSP_EVENACCESS_SFR*)&SYSTEM.MSTPCRB.LONG, BIT31_MASK,
                                &ICU.IPR[IPR_SCI0_RXI0].BYTE,
                                &ICU.IR[IR_SCI0_RXI0].BYTE,
                                &ICU.IR[IR_SCI0_TXI0].BYTE,
                                &ICU.IR[IR_SCI0_TEI0].BYTE,
                                &ICU.IR[IR_SCI0_ERI0].BYTE,
                                &ICU.IER[IER_SCI0_RXI0].BYTE,
                                &ICU.IER[IER_SCI0_TXI0].BYTE,
                                &ICU.IER[IER_SCI0_TEI0].BYTE,
                                &ICU.IER[IER_SCI0_ERI0].BYTE,
                                BIT6_MASK, BIT7_MASK, BIT0_MASK, BIT1_MASK
                                };
/* channel control block */
sci_ch_ctrl_t   ch0_ctrl = {&ch0_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                            , BSP_PCLKB_HZ
                           };
#endif

#if SCI_CFG_CH1_INCLUDED

/* rom info */
const sci_ch_rom_t  ch1_rom = {(volatile struct st_sci12 R_BSP_EVENACCESS_SFR *)&SCI1,
                                (volatile uint32_t R_BSP_EVENACCESS_SFR*)&SYSTEM.MSTPCRB.LONG, BIT30_MASK,
                                &ICU.IPR[IPR_SCI1_RXI1].BYTE,
                                &ICU.IR[IR_SCI1_RXI1].BYTE,
                                &ICU.IR[IR_SCI1_TXI1].BYTE,
                                &ICU.IR[IR_SCI1_TEI1].BYTE,
                                &ICU.IR[IR_SCI1_ERI1].BYTE,
                                &ICU.IER[IER_SCI1_RXI1].BYTE,
                                &ICU.IER[IER_SCI1_TXI1].BYTE,
                                &ICU.IER[IER_SCI1_TEI1].BYTE,
                                &ICU.IER[IER_SCI1_ERI1].BYTE,
                                BIT2_MASK, BIT3_MASK, BIT4_MASK, BIT5_MASK
                                };
/* channel control block */
sci_ch_ctrl_t   ch1_ctrl = {&ch1_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                            , BSP_PCLKB_HZ
                           };
#endif

#if SCI_CFG_CH2_INCLUDED

/* rom info */
const sci_ch_rom_t  ch2_rom = {(volatile struct st_sci12 R_BSP_EVENACCESS_SFR *)&SCI2,
                                (volatile uint32_t R_BSP_EVENACCESS_SFR*)&SYSTEM.MSTPCRB.LONG, BIT29_MASK,
                                &ICU.IPR[IPR_SCI2_RXI2].BYTE,
                                &ICU.IR[IR_SCI2_RXI2].BYTE,
                                &ICU.IR[IR_SCI2_TXI2].BYTE,
                                &ICU.IR[IR_SCI2_TEI2].BYTE,
                                &ICU.IR[IR_SCI2_ERI2].BYTE,
                                &ICU.IER[IER_SCI2_RXI2].BYTE,
                                &ICU.IER[IER_SCI2_TXI2].BYTE,
                                &ICU.IER[IER_SCI2_TEI2].BYTE,
                                &ICU.IER[IER_SCI2_ERI2].BYTE,
                                BIT2_MASK, BIT3_MASK, BIT4_MASK, BIT5_MASK
                                };
/* channel control block */
sci_ch_ctrl_t   ch2_ctrl = {&ch2_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                            , BSP_PCLKB_HZ
                           };
#endif

#if SCI_CFG_CH5_INCLUDED

/* rom info */
const sci_ch_rom_t  ch5_rom = {(volatile struct st_sci12 R_BSP_EVENACCESS_SFR *)&SCI5,
                                (volatile uint32_t R_BSP_EVENACCESS_SFR*)&SYSTEM.MSTPCRB.LONG, BIT26_MASK,
                                &ICU.IPR[IPR_SCI5_RXI5].BYTE,
                                &ICU.IR[IR_SCI5_RXI5].BYTE,
                                &ICU.IR[IR_SCI5_TXI5].BYTE,
                                &ICU.IR[IR_SCI5_TEI5].BYTE,
                                &ICU.IR[IR_SCI5_ERI5].BYTE,
                                &ICU.IER[IER_SCI5_RXI5].BYTE,
                                &ICU.IER[IER_SCI5_TXI5].BYTE,
                                &ICU.IER[IER_SCI5_TEI5].BYTE,
                                &ICU.IER[IER_SCI5_ERI5].BYTE,
                                BIT6_MASK, BIT7_MASK, BIT0_MASK, BIT1_MASK
                                };
/* channel control block */
sci_ch_ctrl_t   ch5_ctrl = {&ch5_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                            , BSP_PCLKB_HZ
                           };
#endif

#if SCI_CFG_CH6_INCLUDED

/* rom info */
const sci_ch_rom_t  ch6_rom = {(volatile struct st_sci12 R_BSP_EVENACCESS_SFR *)&SCI6,
                                (volatile uint32_t R_BSP_EVENACCESS_SFR*)&SYSTEM.MSTPCRB.LONG, BIT25_MASK,
                                &ICU.IPR[IPR_SCI6_RXI6].BYTE,
                                &ICU.IR[IR_SCI6_RXI6].BYTE,
                                &ICU.IR[IR_SCI6_TXI6].BYTE,
                                &ICU.IR[IR_SCI6_TEI6].BYTE,
                                &ICU.IR[IR_SCI6_ERI6].BYTE,
                                &ICU.IER[IER_SCI6_RXI6].BYTE,
                                &ICU.IER[IER_SCI6_TXI6].BYTE,
                                &ICU.IER[IER_SCI6_TEI6].BYTE,
                                &ICU.IER[IER_SCI6_ERI6].BYTE,
                                BIT2_MASK, BIT3_MASK, BIT4_MASK, BIT5_MASK
                                };
/* channel control block */
sci_ch_ctrl_t   ch6_ctrl = {&ch6_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                            , BSP_PCLKB_HZ
                           };
#endif

#if SCI_CFG_CH8_INCLUDED

/* rom info */
const sci_ch_rom_t  ch8_rom = {(volatile struct st_sci12 R_BSP_EVENACCESS_SFR *)&SCI8,
                                (volatile uint32_t R_BSP_EVENACCESS_SFR*)&SYSTEM.MSTPCRC.LONG, BIT27_MASK,
                                &ICU.IPR[IPR_SCI8_RXI8].BYTE,
                                &ICU.IR[IR_SCI8_RXI8].BYTE,
                                &ICU.IR[IR_SCI8_TXI8].BYTE,
                                &ICU.IR[IR_SCI8_TEI8].BYTE,
                                &ICU.IR[IR_SCI8_ERI8].BYTE,
                                &ICU.IER[IER_SCI8_RXI8].BYTE,
                                &ICU.IER[IER_SCI8_TXI8].BYTE,
                                &ICU.IER[IER_SCI8_TEI8].BYTE,
                                &ICU.IER[IER_SCI8_ERI8].BYTE,
                                BIT6_MASK, BIT7_MASK, BIT0_MASK, BIT1_MASK
                                };
/* channel control block */
sci_ch_ctrl_t   ch8_ctrl = {&ch8_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                            , BSP_PCLKB_HZ
                           };
#endif

#if SCI_CFG_CH9_INCLUDED

/* rom info */
const sci_ch_rom_t  ch9_rom = {(volatile struct st_sci12 R_BSP_EVENACCESS_SFR *)&SCI9,
                                (volatile uint32_t R_BSP_EVENACCESS_SFR*)&SYSTEM.MSTPCRC.LONG, BIT26_MASK,
                                &ICU.IPR[IPR_SCI9_RXI9].BYTE,
                                &ICU.IR[IR_SCI9_RXI9].BYTE,
                                &ICU.IR[IR_SCI9_TXI9].BYTE,
                                &ICU.IR[IR_SCI9_TEI9].BYTE,
                                &ICU.IR[IR_SCI9_ERI9].BYTE,
                                &ICU.IER[IER_SCI9_RXI9].BYTE,
                                &ICU.IER[IER_SCI9_TXI9].BYTE,
                                &ICU.IER[IER_SCI9_TEI9].BYTE,
                                &ICU.IER[IER_SCI9_ERI9].BYTE,
                                BIT2_MASK, BIT3_MASK, BIT4_MASK, BIT5_MASK
                                };
/* channel control block */
sci_ch_ctrl_t   ch9_ctrl = {&ch9_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                            , BSP_PCLKB_HZ
                           };
#endif

#if SCI_CFG_CH12_INCLUDED

/* rom info */
const sci_ch_rom_t  ch12_rom = {(volatile struct st_sci12 R_BSP_EVENACCESS_SFR *)&SCI12,
                                (volatile uint32_t R_BSP_EVENACCESS_SFR*)&SYSTEM.MSTPCRB.LONG, BIT4_MASK,
                                &ICU.IPR[IPR_SCI12_RXI12].BYTE,
                                &ICU.IR[IR_SCI12_RXI12].BYTE,
                                &ICU.IR[IR_SCI12_TXI12].BYTE,
                                &ICU.IR[IR_SCI12_TEI12].BYTE,
                                &ICU.IR[IR_SCI12_ERI12].BYTE,
                                &ICU.IER[IER_SCI12_RXI12].BYTE,
                                &ICU.IER[IER_SCI12_TXI12].BYTE,
                                &ICU.IER[IER_SCI12_TEI12].BYTE,
                                &ICU.IER[IER_SCI12_ERI12].BYTE,
                                BIT6_MASK, BIT7_MASK, BIT0_MASK, BIT1_MASK
                                };
/* channel control block */
sci_ch_ctrl_t   ch12_ctrl = {&ch12_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                             #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                             , true, 0, 0, false
                             #endif
                             , BSP_PCLKB_HZ
                            };
#endif


/* SCI HANDLE-ARRAY DECLARATION */

const sci_hdl_t g_handles[SCI_NUM_CH] =
{
#if SCI_CFG_CH0_INCLUDED
            &ch0_ctrl,
#else
            NULL,
#endif

#if SCI_CFG_CH1_INCLUDED
            &ch1_ctrl,
#else
            NULL,
#endif

#if SCI_CFG_CH2_INCLUDED
            &ch2_ctrl,
#else
            NULL,
#endif
            NULL,        /* ch3 */
            NULL,        /* ch4 */

#if SCI_CFG_CH5_INCLUDED
            &ch5_ctrl,
#else
            NULL,
#endif

#if SCI_CFG_CH6_INCLUDED
            &ch6_ctrl,
#else
            NULL,
#endif

            NULL,        /* ch7 */

#if SCI_CFG_CH8_INCLUDED
            &ch8_ctrl,
#else
            NULL,
#endif

#if SCI_CFG_CH9_INCLUDED
            &ch9_ctrl,
#else
            NULL,
#endif

            NULL,        /* ch10 */
            NULL,        /* ch11 */

#if SCI_CFG_CH12_INCLUDED
            &ch12_ctrl
#else
            NULL
#endif
};

