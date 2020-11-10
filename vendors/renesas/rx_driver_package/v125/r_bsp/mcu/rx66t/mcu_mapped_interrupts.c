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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : mcu_mapped_interrupts.c
* Description  : This module maps Interrupt A interrupts. Which interrupts are mapped depends on the macros in
*                r_bsp_interrupt_config.h.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 27.07.2018 1.00      First Release
*         : 28.02.2019 1.01      Fixed coding style.
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

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIA9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIA9) = BSP_PRV_INT_A_NUM_MTU9_TGIA9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIB9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIB9) = BSP_PRV_INT_A_NUM_MTU9_TGIB9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIC9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIC9) = BSP_PRV_INT_A_NUM_MTU9_TGIC9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGID9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGID9) = BSP_PRV_INT_A_NUM_MTU9_TGID9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TCIV9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TCIV9) = BSP_PRV_INT_A_NUM_MTU9_TCIV9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIE9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIE9) = BSP_PRV_INT_A_NUM_MTU9_TGIE9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIF9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_MTU9_TGIF9) = BSP_PRV_INT_A_NUM_MTU9_TGIF9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIA0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIA0) = BSP_PRV_INT_A_NUM_GPTW0_GTCIA0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIB0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIB0) = BSP_PRV_INT_A_NUM_GPTW0_GTCIB0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIC0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIC0) = BSP_PRV_INT_A_NUM_GPTW0_GTCIC0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCID0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCID0) = BSP_PRV_INT_A_NUM_GPTW0_GTCID0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GDTE0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GDTE0) = BSP_PRV_INT_A_NUM_GPTW0_GDTE0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIE0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIE0) = BSP_PRV_INT_A_NUM_GPTW0_GTCIE0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIF0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIF0) = BSP_PRV_INT_A_NUM_GPTW0_GTCIF0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIV0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIV0) = BSP_PRV_INT_A_NUM_GPTW0_GTCIV0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIU0)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW0_GTCIU0) = BSP_PRV_INT_A_NUM_GPTW0_GTCIU0;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIA1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIA1) = BSP_PRV_INT_A_NUM_GPTW1_GTCIA1;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIB1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIB1) = BSP_PRV_INT_A_NUM_GPTW1_GTCIB1;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIC1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIC1) = BSP_PRV_INT_A_NUM_GPTW1_GTCIC1;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCID1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCID1) = BSP_PRV_INT_A_NUM_GPTW1_GTCID1;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GDTE1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GDTE1) = BSP_PRV_INT_A_NUM_GPTW1_GDTE1;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIE1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIE1) = BSP_PRV_INT_A_NUM_GPTW1_GTCIE1;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIF1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIF1) = BSP_PRV_INT_A_NUM_GPTW1_GTCIF1;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIV1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIV1) = BSP_PRV_INT_A_NUM_GPTW1_GTCIV1;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIU1)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW1_GTCIU1) = BSP_PRV_INT_A_NUM_GPTW1_GTCIU1;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIA2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIA2) = BSP_PRV_INT_A_NUM_GPTW2_GTCIA2;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIB2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIB2) = BSP_PRV_INT_A_NUM_GPTW2_GTCIB2;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIC2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIC2) = BSP_PRV_INT_A_NUM_GPTW2_GTCIC2;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCID2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCID2) = BSP_PRV_INT_A_NUM_GPTW2_GTCID2;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GDTE2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GDTE2) = BSP_PRV_INT_A_NUM_GPTW2_GDTE2;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIE2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIE2) = BSP_PRV_INT_A_NUM_GPTW2_GTCIE2;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIF2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIF2) = BSP_PRV_INT_A_NUM_GPTW2_GTCIF2;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIV2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIV2) = BSP_PRV_INT_A_NUM_GPTW2_GTCIV2;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIU2)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW2_GTCIU2) = BSP_PRV_INT_A_NUM_GPTW2_GTCIU2;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIA3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIA3) = BSP_PRV_INT_A_NUM_GPTW3_GTCIA3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIB3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIB3) = BSP_PRV_INT_A_NUM_GPTW3_GTCIB3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIC3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIC3) = BSP_PRV_INT_A_NUM_GPTW3_GTCIC3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCID3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCID3) = BSP_PRV_INT_A_NUM_GPTW3_GTCID3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GDTE3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GDTE3) = BSP_PRV_INT_A_NUM_GPTW3_GDTE3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIE3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIE3) = BSP_PRV_INT_A_NUM_GPTW3_GTCIE3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIF3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIF3) = BSP_PRV_INT_A_NUM_GPTW3_GTCIF3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIV3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIV3) = BSP_PRV_INT_A_NUM_GPTW3_GTCIV3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIU3)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW3_GTCIU3) = BSP_PRV_INT_A_NUM_GPTW3_GTCIU3;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIA4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIA4) = BSP_PRV_INT_A_NUM_GPTW4_GTCIA4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIB4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIB4) = BSP_PRV_INT_A_NUM_GPTW4_GTCIB4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIC4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIC4) = BSP_PRV_INT_A_NUM_GPTW4_GTCIC4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCID4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCID4) = BSP_PRV_INT_A_NUM_GPTW4_GTCID4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GDTE4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GDTE4) = BSP_PRV_INT_A_NUM_GPTW4_GDTE4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIE4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIE4) = BSP_PRV_INT_A_NUM_GPTW4_GTCIE4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIF4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIF4) = BSP_PRV_INT_A_NUM_GPTW4_GTCIF4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIV4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIV4) = BSP_PRV_INT_A_NUM_GPTW4_GTCIV4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIU4)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW4_GTCIU4) = BSP_PRV_INT_A_NUM_GPTW4_GTCIU4;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIA5)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIA5) = BSP_PRV_INT_A_NUM_GPTW5_GTCIA5;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIB5)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIB5) = BSP_PRV_INT_A_NUM_GPTW5_GTCIB5;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIC5)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIC5) = BSP_PRV_INT_A_NUM_GPTW5_GTCIC5;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCID5)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCID5) = BSP_PRV_INT_A_NUM_GPTW5_GTCID5;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GDTE5)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GDTE5) = BSP_PRV_INT_A_NUM_GPTW5_GDTE5;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIE5)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIE5) = BSP_PRV_INT_A_NUM_GPTW5_GTCIE5;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIF5)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIF5) = BSP_PRV_INT_A_NUM_GPTW5_GTCIF5;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIV5)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIV5) = BSP_PRV_INT_A_NUM_GPTW5_GTCIV5;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIU5)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW5_GTCIU5) = BSP_PRV_INT_A_NUM_GPTW5_GTCIU5;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIA6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIA6) = BSP_PRV_INT_A_NUM_GPTW6_GTCIA6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIB6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIB6) = BSP_PRV_INT_A_NUM_GPTW6_GTCIB6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIC6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIC6) = BSP_PRV_INT_A_NUM_GPTW6_GTCIC6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCID6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCID6) = BSP_PRV_INT_A_NUM_GPTW6_GTCID6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GDTE6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GDTE6) = BSP_PRV_INT_A_NUM_GPTW6_GDTE6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIE6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIE6) = BSP_PRV_INT_A_NUM_GPTW6_GTCIE6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIF6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIF6) = BSP_PRV_INT_A_NUM_GPTW6_GTCIF6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIV6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIV6) = BSP_PRV_INT_A_NUM_GPTW6_GTCIV6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIU6)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW6_GTCIU6) = BSP_PRV_INT_A_NUM_GPTW6_GTCIU6;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIA7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIA7) = BSP_PRV_INT_A_NUM_GPTW7_GTCIA7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIB7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIB7) = BSP_PRV_INT_A_NUM_GPTW7_GTCIB7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIC7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIC7) = BSP_PRV_INT_A_NUM_GPTW7_GTCIC7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCID7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCID7) = BSP_PRV_INT_A_NUM_GPTW7_GTCID7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GDTE7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GDTE7) = BSP_PRV_INT_A_NUM_GPTW7_GDTE7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIE7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIE7) = BSP_PRV_INT_A_NUM_GPTW7_GTCIE7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIF7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIF7) = BSP_PRV_INT_A_NUM_GPTW7_GTCIF7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIV7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIV7) = BSP_PRV_INT_A_NUM_GPTW7_GTCIV7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIU7)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW7_GTCIU7) = BSP_PRV_INT_A_NUM_GPTW7_GTCIU7;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIA8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIA8) = BSP_PRV_INT_A_NUM_GPTW8_GTCIA8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIB8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIB8) = BSP_PRV_INT_A_NUM_GPTW8_GTCIB8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIC8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIC8) = BSP_PRV_INT_A_NUM_GPTW8_GTCIC8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCID8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCID8) = BSP_PRV_INT_A_NUM_GPTW8_GTCID8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GDTE8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GDTE8) = BSP_PRV_INT_A_NUM_GPTW8_GDTE8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIE8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIE8) = BSP_PRV_INT_A_NUM_GPTW8_GTCIE8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIF8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIF8) = BSP_PRV_INT_A_NUM_GPTW8_GTCIF8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIV8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIV8) = BSP_PRV_INT_A_NUM_GPTW8_GTCIV8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIU8)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW8_GTCIU8) = BSP_PRV_INT_A_NUM_GPTW8_GTCIU8;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIA9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIA9) = BSP_PRV_INT_A_NUM_GPTW9_GTCIA9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIB9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIB9) = BSP_PRV_INT_A_NUM_GPTW9_GTCIB9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIC9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIC9) = BSP_PRV_INT_A_NUM_GPTW9_GTCIC9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCID9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCID9) = BSP_PRV_INT_A_NUM_GPTW9_GTCID9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GDTE9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GDTE9) = BSP_PRV_INT_A_NUM_GPTW9_GDTE9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIE9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIE9) = BSP_PRV_INT_A_NUM_GPTW9_GTCIE9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIF9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIF9) = BSP_PRV_INT_A_NUM_GPTW9_GTCIF9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIV9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIV9) = BSP_PRV_INT_A_NUM_GPTW9_GTCIV9;
#endif

#if BSP_PRV_VALID_MAP_INT(A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIU9)
    /* Casting is valid because it matches the type to the right side or argument. */
    BSP_PRV_INT_SELECT(BSP_PRV_A, BSP_MAPPED_INT_CFG_A_VECT_GPTW9_GTCIU9) = BSP_PRV_INT_A_NUM_GPTW9_GTCIU9;
#endif

} /* End of function bsp_mapped_interrupt_open() */

