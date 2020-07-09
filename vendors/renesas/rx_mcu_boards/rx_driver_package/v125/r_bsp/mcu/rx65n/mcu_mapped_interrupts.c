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
/***********************************************************************************************************************
* File Name    : mcu_mapped_interrupts.c
* Description  : This module maps Interrupt A & B interrupts. Which interrupts are mapped depends on the macros in
*                r_bsp_interrupt_config.h.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 01.10.2016 1.00      First Release
*         : 15.05.2017 2.00      Changed the name of the following macro definition, because there was a mistake
*                                in the name of macro definition.
*                                 (XXX = BSP_MAPPED_INT_CFG_B_VECT, BSP_MAPPED_INT_CFG_A_VECT,
*                                        BSP_PRV_INT_B_NUM, BSP_PRV_INT_A_NUM)
*                                - From XXX_TPU0_TGI0V to XXX_TPU0_TCI0V.
*                                - From XXX_TPU1_TGI1V to XXX_TPU1_TCI1V.
*                                - From XXX_TPU1_TGI1U to XXX_TPU1_TCI1U.
*                                - From XXX_TPU2_TGI2V to XXX_TPU2_TCI2V.
*                                - From XXX_TPU2_TGI2U to XXX_TPU2_TCI2U.
*                                - From XXX_TPU3_TGI3V to XXX_TPU3_TCI3V.
*                                - From XXX_TPU4_TGI4V to XXX_TPU4_TCI4V.
*                                - From XXX_TPU4_TGI4U to XXX_TPU4_TCI4U.
*                                - From XXX_TPU5_TGI5V to XXX_TPU5_TCI5V.
*                                - From XXX_TPU5_TGI5U to XXX_TPU5_TCI5U.
*                                - From XXX_MTU0_TGIV0 to XXX_MTU0_TCIV0.
*                                - From XXX_MTU1_TGIV1 to XXX_MTU1_TCIV1.
*                                - From XXX_MTU1_TGIU1 to XXX_MTU1_TCIU1.
*                                - From XXX_MTU2_TGIV2 to XXX_MTU2_TCIV2.
*                                - From XXX_MTU2_TGIU2 to XXX_MTU2_TCIU2.
*                                - From XXX_MTU3_TGIV3 to XXX_MTU3_TCIV3.
*                                - From XXX_MTU4_TGIV4 to XXX_MTU4_TCIV4.
*                                - From XXX_MTU6_TGIV6 to XXX_MTU6_TCIV6.
*                                - From XXX_MTU7_TGIV7 to XXX_MTU7_TCIV7.
*                                - From XXX_MTU8_TGIV8 to XXX_MTU8_TCIV8.
*                                Added select processing of the following software configurable interrupt source.
*                                - TSIP_PROC_BUSY
*                                - TSIP_ROMOK
*                                - TSIP_LONG_PLG
*                                - TSIP_TEST_BUSY
*                                - TSIP_WRRDY0
*                                - TSIP_WRRDY1
*                                - TSIP_WRRDY4
*                                - TSIP_RDRDY0
*                                - TSIP_RDRDY1
*                                - TSIP_INTEGRATE_WRRDY
*                                - TSIP_INTEGRATE_RDRDY
*         : 28.02.2019 2.01      Fixed coding style.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Access to r_bsp. */
#include "platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/
 
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: bsp_mapped_interrupt_open
* Description  : Initializes mapped interrupts. This code does the following for each possible mapped interrupt:
*                1) PREPROCCESOR - Test to see if this interrupt is chosen to be used
*                2) PREPROCESSOR - Figure out which interrupt select register needs to be written to
*                3) RUNTIME C    - Set the appropriate select register with the number of this mapped interrupt
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void bsp_mapped_interrupt_open (void)
{
#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CMT2_CMI2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CMT2_CMI2) = BSP_PRV_INT_B_NUM_CMT2_CMI2;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CMT3_CMI3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CMT3_CMI3) = BSP_PRV_INT_B_NUM_CMT3_CMI3;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TMR0_CMIA0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TMR0_CMIA0) = BSP_PRV_INT_B_NUM_TMR0_CMIA0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TMR0_CMIB0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TMR0_CMIB0) = BSP_PRV_INT_B_NUM_TMR0_CMIB0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TMR0_OVI0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TMR0_OVI0) = BSP_PRV_INT_B_NUM_TMR0_OVI0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TMR1_CMIA1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TMR1_CMIA1) = BSP_PRV_INT_B_NUM_TMR1_CMIA1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TMR1_CMIB1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TMR1_CMIB1) = BSP_PRV_INT_B_NUM_TMR1_CMIB1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TMR1_OVI1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TMR1_OVI1) = BSP_PRV_INT_B_NUM_TMR1_OVI1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TMR2_CMIA2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TMR2_CMIA2) = BSP_PRV_INT_B_NUM_TMR2_CMIA2;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TMR2_CMIB2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TMR2_CMIB2) = BSP_PRV_INT_B_NUM_TMR2_CMIB2;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TMR2_OVI2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TMR2_OVI2) = BSP_PRV_INT_B_NUM_TMR2_OVI2;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TMR3_CMIA3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TMR3_CMIA3) = BSP_PRV_INT_B_NUM_TMR3_CMIA3;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TMR3_CMIB3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TMR3_CMIB3) = BSP_PRV_INT_B_NUM_TMR3_CMIB3;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TMR3_OVI3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TMR3_OVI3) = BSP_PRV_INT_B_NUM_TMR3_OVI3;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU0_TGI0A)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU0_TGI0A) = BSP_PRV_INT_B_NUM_TPU0_TGI0A;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU0_TGI0B)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU0_TGI0B) = BSP_PRV_INT_B_NUM_TPU0_TGI0B;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU0_TGI0C)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU0_TGI0C) = BSP_PRV_INT_B_NUM_TPU0_TGI0C;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU0_TGI0D)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU0_TGI0D) = BSP_PRV_INT_B_NUM_TPU0_TGI0D;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU0_TCI0V)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU0_TCI0V) = BSP_PRV_INT_B_NUM_TPU0_TCI0V;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU1_TGI1A)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU1_TGI1A) = BSP_PRV_INT_B_NUM_TPU1_TGI1A;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU1_TGI1B)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU1_TGI1B) = BSP_PRV_INT_B_NUM_TPU1_TGI1B;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU1_TCI1V)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU1_TCI1V) = BSP_PRV_INT_B_NUM_TPU1_TCI1V;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU1_TCI1U)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU1_TCI1U) = BSP_PRV_INT_B_NUM_TPU1_TCI1U;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU2_TGI2A)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU2_TGI2A) = BSP_PRV_INT_B_NUM_TPU2_TGI2A;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU2_TGI2B)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU2_TGI2B) = BSP_PRV_INT_B_NUM_TPU2_TGI2B;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU2_TCI2V)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU2_TCI2V) = BSP_PRV_INT_B_NUM_TPU2_TCI2V;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU2_TCI2U)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU2_TCI2U) = BSP_PRV_INT_B_NUM_TPU2_TCI2U;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU3_TGI3A)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU3_TGI3A) = BSP_PRV_INT_B_NUM_TPU3_TGI3A;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU3_TGI3B)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU3_TGI3B) = BSP_PRV_INT_B_NUM_TPU3_TGI3B;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU3_TGI3C)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU3_TGI3C) = BSP_PRV_INT_B_NUM_TPU3_TGI3C;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU3_TGI3D)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU3_TGI3D) = BSP_PRV_INT_B_NUM_TPU3_TGI3D;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU3_TCI3V)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU3_TCI3V) = BSP_PRV_INT_B_NUM_TPU3_TCI3V;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU4_TGI4A)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU4_TGI4A) = BSP_PRV_INT_B_NUM_TPU4_TGI4A;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU4_TGI4B)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU4_TGI4B) = BSP_PRV_INT_B_NUM_TPU4_TGI4B;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU4_TCI4V)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU4_TCI4V) = BSP_PRV_INT_B_NUM_TPU4_TCI4V;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU4_TCI4U)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU4_TCI4U) = BSP_PRV_INT_B_NUM_TPU4_TCI4U;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU5_TGI5A)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU5_TGI5A) = BSP_PRV_INT_B_NUM_TPU5_TGI5A;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU5_TGI5B)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU5_TGI5B) = BSP_PRV_INT_B_NUM_TPU5_TGI5B;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU5_TCI5V)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU5_TCI5V) = BSP_PRV_INT_B_NUM_TPU5_TCI5V;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TPU5_TCI5U)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TPU5_TCI5U) = BSP_PRV_INT_B_NUM_TPU5_TCI5U;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CMTW0_IC0I0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CMTW0_IC0I0) = BSP_PRV_INT_B_NUM_CMTW0_IC0I0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CMTW0_IC1I0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CMTW0_IC1I0) = BSP_PRV_INT_B_NUM_CMTW0_IC1I0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CMTW0_OC0I0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CMTW0_OC0I0) = BSP_PRV_INT_B_NUM_CMTW0_OC0I0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CMTW0_OC1I0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CMTW0_OC1I0) = BSP_PRV_INT_B_NUM_CMTW0_OC1I0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CMTW1_IC0I1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CMTW1_IC0I1) = BSP_PRV_INT_B_NUM_CMTW1_IC0I1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CMTW1_IC1I1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CMTW1_IC1I1) = BSP_PRV_INT_B_NUM_CMTW1_IC1I1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CMTW1_OC0I1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CMTW1_OC0I1) = BSP_PRV_INT_B_NUM_CMTW1_OC0I1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CMTW1_OC1I1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CMTW1_OC1I1) = BSP_PRV_INT_B_NUM_CMTW1_OC1I1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_RTC_CUP)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_RTC_CUP) = BSP_PRV_INT_B_NUM_RTC_CUP;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CAN0_RXF0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CAN0_RXF0) = BSP_PRV_INT_B_NUM_CAN0_RXF0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CAN0_TXF0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CAN0_TXF0) = BSP_PRV_INT_B_NUM_CAN0_TXF0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CAN0_RXM0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CAN0_RXM0) = BSP_PRV_INT_B_NUM_CAN0_RXM0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CAN0_TXM0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CAN0_TXM0) = BSP_PRV_INT_B_NUM_CAN0_TXM0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CAN1_RXF1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CAN1_RXF1) = BSP_PRV_INT_B_NUM_CAN1_RXF1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CAN1_TXF1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CAN1_TXF1) = BSP_PRV_INT_B_NUM_CAN1_TXF1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CAN1_RXM1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CAN1_RXM1) = BSP_PRV_INT_B_NUM_CAN1_RXM1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_CAN1_TXM1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_CAN1_TXM1) = BSP_PRV_INT_B_NUM_CAN1_TXM1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_USB0_USBI0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_USB0_USBI0) = BSP_PRV_INT_B_NUM_USB0_USBI0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_S12ADC0_S12ADI0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_S12ADC0_S12ADI0) = BSP_PRV_INT_B_NUM_S12ADC0_S12ADI0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_S12ADC0_S12GBADI0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_S12ADC0_S12GBADI0) = BSP_PRV_INT_B_NUM_S12ADC0_S12GBADI0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_S12ADC0_S12GCADI0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_S12ADC0_S12GCADI0) = BSP_PRV_INT_B_NUM_S12ADC0_S12GCADI0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_S12ADC1_S12ADI1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_S12ADC1_S12ADI1) = BSP_PRV_INT_B_NUM_S12ADC1_S12ADI1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_S12ADC1_S12GBADI1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_S12ADC1_S12GBADI1) = BSP_PRV_INT_B_NUM_S12ADC1_S12GBADI1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_S12ADC1_S12GCADI1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_S12ADC1_S12GCADI1) = BSP_PRV_INT_B_NUM_S12ADC1_S12GCADI1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_RNG_RNGEND)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_RNG_RNGEND) = BSP_PRV_INT_B_NUM_RNG_RNGEND;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_ELC_ELSR18I)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_ELC_ELSR18I) = BSP_PRV_INT_B_NUM_ELC_ELSR18I;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_ELC_ELSR19I)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_ELC_ELSR19I) = BSP_PRV_INT_B_NUM_ELC_ELSR19I;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_PROC_BUSY)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_PROC_BUSY) = BSP_PRV_INT_B_NUM_TSIP_PROC_BUSY;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_ROMOK)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_ROMOK) = BSP_PRV_INT_B_NUM_TSIP_ROMOK;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_LONG_PLG)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_LONG_PLG) = BSP_PRV_INT_B_NUM_TSIP_LONG_PLG;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_TEST_BUSY)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_TEST_BUSY) = BSP_PRV_INT_B_NUM_TSIP_TEST_BUSY;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_WRRDY0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_WRRDY0) = BSP_PRV_INT_B_NUM_TSIP_WRRDY0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_WRRDY1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_WRRDY1) = BSP_PRV_INT_B_NUM_TSIP_WRRDY1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_WRRDY4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_WRRDY4) = BSP_PRV_INT_B_NUM_TSIP_WRRDY4;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_RDRDY0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_RDRDY0) = BSP_PRV_INT_B_NUM_TSIP_RDRDY0;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_RDRDY1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_RDRDY1) = BSP_PRV_INT_B_NUM_TSIP_RDRDY1;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_INTEGRATE_WRRDY)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_INTEGRATE_WRRDY) = BSP_PRV_INT_B_NUM_TSIP_INTEGRATE_WRRDY;
#endif

#if BSP_PRV_VALID_MAP_INT(B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_INTEGRATE_RDRDY)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_B, BSP_MAPPED_INT_CFG_B_VECT_TSIP_INTEGRATE_RDRDY) = BSP_PRV_INT_B_NUM_TSIP_INTEGRATE_RDRDY;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIA0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIA0) = BSP_PRV_INT_A_NUM_MTU0_TGIA0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIB0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIB0) = BSP_PRV_INT_A_NUM_MTU0_TGIB0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIC0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIC0) = BSP_PRV_INT_A_NUM_MTU0_TGIC0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGID0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGID0) = BSP_PRV_INT_A_NUM_MTU0_TGID0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TCIV0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TCIV0) = BSP_PRV_INT_A_NUM_MTU0_TCIV0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIE0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIE0) = BSP_PRV_INT_A_NUM_MTU0_TGIE0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIF0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU0_TGIF0) = BSP_PRV_INT_A_NUM_MTU0_TGIF0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU1_TGIA1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU1_TGIA1) = BSP_PRV_INT_A_NUM_MTU1_TGIA1;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU1_TGIB1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU1_TGIB1) = BSP_PRV_INT_A_NUM_MTU1_TGIB1;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU1_TCIV1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU1_TCIV1) = BSP_PRV_INT_A_NUM_MTU1_TCIV1;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU1_TCIU1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU1_TCIU1) = BSP_PRV_INT_A_NUM_MTU1_TCIU1;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU2_TGIA2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU2_TGIA2) = BSP_PRV_INT_A_NUM_MTU2_TGIA2;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU2_TGIB2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU2_TGIB2) = BSP_PRV_INT_A_NUM_MTU2_TGIB2;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU2_TCIV2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU2_TCIV2) = BSP_PRV_INT_A_NUM_MTU2_TCIV2;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU2_TCIU2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU2_TCIU2) = BSP_PRV_INT_A_NUM_MTU2_TCIU2;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGIA3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGIA3) = BSP_PRV_INT_A_NUM_MTU3_TGIA3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGIB3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGIB3) = BSP_PRV_INT_A_NUM_MTU3_TGIB3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGIC3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGIC3) = BSP_PRV_INT_A_NUM_MTU3_TGIC3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGID3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU3_TGID3) = BSP_PRV_INT_A_NUM_MTU3_TGID3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU3_TCIV3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU3_TCIV3) = BSP_PRV_INT_A_NUM_MTU3_TCIV3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGIA4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGIA4) = BSP_PRV_INT_A_NUM_MTU4_TGIA4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGIB4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGIB4) = BSP_PRV_INT_A_NUM_MTU4_TGIB4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGIC4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGIC4) = BSP_PRV_INT_A_NUM_MTU4_TGIC4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGID4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU4_TGID4) = BSP_PRV_INT_A_NUM_MTU4_TGID4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU4_TCIV4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU4_TCIV4) = BSP_PRV_INT_A_NUM_MTU4_TCIV4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU5_TGIU5)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU5_TGIU5) = BSP_PRV_INT_A_NUM_MTU5_TGIU5;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU5_TGIV5)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU5_TGIV5) = BSP_PRV_INT_A_NUM_MTU5_TGIV5;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU5_TGIW5)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU5_TGIW5) = BSP_PRV_INT_A_NUM_MTU5_TGIW5;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGIA6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGIA6) = BSP_PRV_INT_A_NUM_MTU6_TGIA6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGIB6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGIB6) = BSP_PRV_INT_A_NUM_MTU6_TGIB6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGIC6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGIC6) = BSP_PRV_INT_A_NUM_MTU6_TGIC6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGID6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU6_TGID6) = BSP_PRV_INT_A_NUM_MTU6_TGID6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU6_TCIV6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU6_TCIV6) = BSP_PRV_INT_A_NUM_MTU6_TCIV6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGIA7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGIA7) = BSP_PRV_INT_A_NUM_MTU7_TGIA7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGIB7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGIB7) = BSP_PRV_INT_A_NUM_MTU7_TGIB7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGIC7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGIC7) = BSP_PRV_INT_A_NUM_MTU7_TGIC7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGID7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU7_TGID7) = BSP_PRV_INT_A_NUM_MTU7_TGID7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU7_TCIV7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU7_TCIV7) = BSP_PRV_INT_A_NUM_MTU7_TCIV7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU8_TGIA8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU8_TGIA8) = BSP_PRV_INT_A_NUM_MTU8_TGIA8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU8_TGIB8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU8_TGIB8) = BSP_PRV_INT_A_NUM_MTU8_TGIB8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU8_TGIC8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU8_TGIC8) = BSP_PRV_INT_A_NUM_MTU8_TGIC8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU8_TGID8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU8_TGID8) = BSP_PRV_INT_A_NUM_MTU8_TGID8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU8_TCIV8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU8_TCIV8) = BSP_PRV_INT_A_NUM_MTU8_TCIV8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_AES_AESRDY)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_AES_AESRDY) = BSP_PRV_INT_A_NUM_AES_AESRDY;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_AES_AESEND)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_AES_AESEND) = BSP_PRV_INT_A_NUM_AES_AESEND;
#endif

} /* End of function bsp_mapped_interrupt_open() */

