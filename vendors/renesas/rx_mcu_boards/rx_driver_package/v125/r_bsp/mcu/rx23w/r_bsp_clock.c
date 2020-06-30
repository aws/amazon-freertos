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
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_bsp_clock.c
* Description  : Contains clock specific routines
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 29.03.2019 1.00     First Release
*         : 26.07.2019 1.01     Modified comment of API function to Doxygen style.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
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
/* When using the user startup program, disable the following code. */
#if BSP_CFG_STARTUP_DISABLE == 0

#if BSP_CFG_CLKOUT_RF_MAIN == 1
/**********************************************************************************************************************
 * Function Name: R_BSP_ConfigClockSetting
 ******************************************************************************************************************//**
 * @brief This function is available only on the RX23W. This function is used by Bluetooth Low Energy Protocol Stack 
 * Basic Package.
 * @details Under certain conditions, Bluetooth Low Energy Protocol Stack Basic Package uses this function to set the 
 * clock. For details, refer to Bluetooth Low Energy Protocol Stack Basic Package User's Manual (R01UW0205)
 */
void R_BSP_ConfigClockSetting(void)
{
#if BSP_CFG_CLOCK_SOURCE == 1
#if BSP_CFG_HOCO_FREQUENCY == 3

    volatile uint16_t tmp_clock = 0;
    
    /* Protect off. */
    SYSTEM.PRCR.WORD = 0xA50B;
    
    /* Casting is valid because it matches the type to the retern value. */
    SYSTEM.SCKCR3.WORD = 0x00;

    /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual. */
    if(tmp_clock ==  SYSTEM.SCKCR3.WORD)
    {
        R_BSP_NOP();
    }
    
    /* WAIT_LOOP */
    while(1 != SYSTEM.OSCOVFSR.BIT.HCOVF)
    {
        /* The delay period needed is to make sure that the HOCO has stopped. */
        R_BSP_NOP();
    }

    /* HOCO is chosen. Start it operating if it is not already operating. */
    if (0 == SYSTEM.HOCOCR.BIT.HCSTP)
    {
        /* HOCO is chosen. Start it operating. */
        SYSTEM.HOCOCR.BYTE = 0x01;    
    }
    
    /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual. */
    if(0x01 ==  SYSTEM.HOCOCR.BYTE)
    {
        R_BSP_NOP();
    }
    
    /* WAIT_LOOP */
    while(0 != SYSTEM.OSCOVFSR.BIT.HCOVF)
    {
        /* The delay period needed is to make sure that the HOCO has stabilized. */
        R_BSP_NOP();
    }
    
    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;
    
#endif /* BSP_CFG_CLOCK_SOURCE == 1 */
#endif /* BSP_CFG_HOCO_FREQUENCY == 3 */

    operating_frequency_set();
    usb_lpc_clock_source_select();

    /* Protect off. */
    SYSTEM.PRCR.WORD = 0xA50B;
    
#if BSP_CFG_CLOCK_SOURCE != 1
#if (0x00000100 == (BSP_CFG_OFS1_REG_VALUE & 0x00000100))

    /* Stop HOCO. */
    SYSTEM.HOCOCR.BYTE = 0x01;
    
    /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual. */
    if(0x01 ==  SYSTEM.HOCOCR.BYTE)
    {
        R_BSP_NOP();
    }

    /* WAIT_LOOP */
    while(1 == SYSTEM.OSCOVFSR.BIT.HCOVF)
    {
        /* The delay period needed is to make sure that the HOCO has stopped. */
        R_BSP_NOP();
    }
#endif
#endif

    /* Set to Middle-speed operating mode if clock is <= 12MHz. */
    /* Because ICLK may not be maximum frequency, it is necessary to check all clock frequency. */
    if( (BSP_ICLK_HZ  <= BSP_MIDDLE_SPEED_MAX_FREQUENCY)&&
        (BSP_PCLKA_HZ <= BSP_MIDDLE_SPEED_MAX_FREQUENCY)&&
        (BSP_PCLKB_HZ <= BSP_MIDDLE_SPEED_MAX_FREQUENCY)&&
        (BSP_PCLKD_HZ <= BSP_MIDDLE_SPEED_MAX_FREQUENCY)&&
        (BSP_FCLK_HZ  <= BSP_MIDDLE_SPEED_MAX_FREQUENCY))
    {
        /* set to middle-speed mode */
        SYSTEM.OPCCR.BYTE = 0x02;

        /* WAIT_LOOP */
        while(1 == SYSTEM.OPCCR.BIT.OPCMTSF)
        {
            /* Wait for transition to finish. */
            R_BSP_NOP();
        }
    }
    
    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;
    
#if BSP_CFG_CONFIGURATOR_SELECT == 1
    /* Smart Configurator initialization function */
    R_Systeminit();
#endif
} /* End of function R_BSP_ConfigClockSetting() */

#endif /* BSP_CFG_CLKOUT_RF_MAIN == 1 */
#endif /* BSP_CFG_STARTUP_DISABLE == 0 */
