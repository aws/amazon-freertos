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
* Copyright (C) 2016, 2017 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_smc_interrupt.c
* Version      : 1.1.0
* Device(s)    : R5F565NEDxFC
* Description  : This file implements interrupt setting
* Creation Date: 2018-08-10
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_smc_interrupt.h"
#include "platform.h"

/***********************************************************************************************************************
* Function Name: R_Interrupt_Create
* Description  : This function Used to set the fast interrupt or group interrupt 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Interrupt_Create(void)
{
    /* Disable group AL1 interrupt*/
    IEN(ICU,GROUPAL1) = 0U;
    

    /* Set group AL1 interrupt priority level */
    IPR(ICU,GROUPAL1) = _02_ICU_PRIORITY_LEVEL2;

    /* Enable group AL1 interrupt */
    IEN(ICU,GROUPAL1) = 1U;
}

