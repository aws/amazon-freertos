/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_sci_rx_pinset.c
* Version      : 1.0.2
* Device(s)    : R5F565NEDxFP
* Tool-Chain   : RXC toolchain
* Description  : Setting of port and mpc registers
* Creation Date: 2019-11-30
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_sci_rx_pinset.h"
#include "platform.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_SCI_PinSet_SCI5
* Description  : This function initializes pins for r_sci_rx module
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_SCI_PinSet_SCI5()
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Set RXD5/SMISO5 pin */
    MPC.PA3PFS.BYTE = 0x0AU;
    PORTA.PMR.BIT.B3 = 1U;

    /* Set TXD5/SMOSI5 pin */
    MPC.PA4PFS.BYTE = 0x0AU;
    PORTA.PMR.BIT.B4 = 1U;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

