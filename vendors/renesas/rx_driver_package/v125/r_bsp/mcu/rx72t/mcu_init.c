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
* File Name    : mcu_init.c
* Description  : Performs initialization common to all MCUs in this Group
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.10.2018 1.00     First Release
*         : 28.02.2019 1.01     Modified comment.
*                               Fixed coding style.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Get specifics on this MCU. */
#include "platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* RX MCUs come in different packages and different pin counts.
 * Each bit of PORTm.PDR corresponds to each pin of port m; I/O direction can be specified in 1-bit units.
 * Each bit of PDR corresponding to port m that does not exist is reserved.
 * Also, each bit of PDR corresponding to P40 to P42, P44 to P46, PH0, PH4, and PE2 pins is reserved, because such pins
 * are input only.
 * Make settings of the reserved bit according to the description in section 20.4, Initialization of the Port Direction
 * Register (PDR). These values are then ORed into the direction registers to set non-existent pins as outputs or 
 * inputs, which can help save power.
 */
#if BSP_PACKAGE_PINS == 144
    /* Refer User's Manual: Hardware Table 20.7. */
    #define BSP_PRV_PORT0_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORT1_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORT2_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORT3_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORT4_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORT5_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORT6_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORT7_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORT8_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORT9_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORTA_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORTB_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORTC_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORTD_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORTE_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORTF_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORTG_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORTH_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORTK_NE_PIN_MASK     (0x00)
#elif   BSP_PACKAGE_PINS == 100
    /* Refer User's Manual: Hardware Table 20.8 to 20.10. */
    #define BSP_PRV_PORT0_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORT1_NE_PIN_MASK     (0xFC)
#if (0xB == BSP_CFG_MCU_PART_FUNCTION) || (0xF == BSP_CFG_MCU_PART_FUNCTION)
    #define BSP_PRV_PORT2_NE_PIN_MASK     (0xE0)
#else
    #define BSP_PRV_PORT2_NE_PIN_MASK     (0x60)
#endif
    #define BSP_PRV_PORT3_NE_PIN_MASK     (0x30)
    #define BSP_PRV_PORT4_NE_PIN_MASK     (0x00)
#if (0xB == BSP_CFG_MCU_PART_FUNCTION) || (0xF == BSP_CFG_MCU_PART_FUNCTION)
    #define BSP_PRV_PORT5_NE_PIN_MASK     (0x00)
#else
    #define BSP_PRV_PORT5_NE_PIN_MASK     (0x03)
#endif
    #define BSP_PRV_PORT6_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORT7_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORT8_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORT9_NE_PIN_MASK     (0x00)
    #define BSP_PRV_PORTA_NE_PIN_MASK     (0xC0)
#if (0xC == BSP_CFG_MCU_PART_FUNCTION) || (0x10 == BSP_CFG_MCU_PART_FUNCTION)
    #define BSP_PRV_PORTB_NE_PIN_MASK     (0x80)
#else
    #define BSP_PRV_PORTB_NE_PIN_MASK     (0x00)
#endif
    #define BSP_PRV_PORTC_NE_PIN_MASK     (0x7F)
#if (0xC == BSP_CFG_MCU_PART_FUNCTION) || (0x10 == BSP_CFG_MCU_PART_FUNCTION)
    #define BSP_PRV_PORTD_NE_PIN_MASK     (0x03)
#else
    #define BSP_PRV_PORTD_NE_PIN_MASK     (0x00)
#endif
    #define BSP_PRV_PORTE_NE_PIN_MASK     (0x40)
    #define BSP_PRV_PORTF_NE_PIN_MASK     (0x0F)
    #define BSP_PRV_PORTG_NE_PIN_MASK     (0x07)
    #define BSP_PRV_PORTH_NE_PIN_MASK     (0xEE)
    #define BSP_PRV_PORTK_NE_PIN_MASK     (0x07)
#else /* (BSP_PACKAGE_PINS != 144) && (BSP_PACKAGE_PINS != 100) */
    #error "ERROR - This package is not defined in mcu_init.c"
#endif /* (BSP_PACKAGE_PINS == 144) && (BSP_PACKAGE_PINS == 100) */

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
* Function Name: bsp_non_existent_port_init
* Description  : For MCUs that do not have the maximum number of pins for their group (e.g. MCU with 100 pins when
*                maximum is 144 pins) these 'non-existent' pins that are not bonded out need to be initialized to save
*                power.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void bsp_non_existent_port_init (void)
{
    /* OR in missing pin masks from above. */

    /* Set PORT0.PDR */
    PORT0.PDR.BYTE |= BSP_PRV_PORT0_NE_PIN_MASK;

    /* Set PORT1.PDR */
    PORT1.PDR.BYTE |= BSP_PRV_PORT1_NE_PIN_MASK;

    /* Set PORT2.PDR */
    PORT2.PDR.BYTE |= BSP_PRV_PORT2_NE_PIN_MASK;

    /* Set PORT3.PDR */
    PORT3.PDR.BYTE |= BSP_PRV_PORT3_NE_PIN_MASK;

    /* Set PORT4.PDR */
    PORT4.PDR.BYTE |= BSP_PRV_PORT4_NE_PIN_MASK;

    /* Set PORT5.PDR */
    PORT5.PDR.BYTE |= BSP_PRV_PORT5_NE_PIN_MASK;

    /* Set PORT6.PDR */
    PORT6.PDR.BYTE |= BSP_PRV_PORT6_NE_PIN_MASK;

    /* Set PORT7.PDR */
    PORT7.PDR.BYTE |= BSP_PRV_PORT7_NE_PIN_MASK;

    /* Set PORT8.PDR */
    PORT8.PDR.BYTE |= BSP_PRV_PORT8_NE_PIN_MASK;

    /* Set PORT9.PDR */
    PORT9.PDR.BYTE |= BSP_PRV_PORT9_NE_PIN_MASK;

    /* Set PORTA.PDR */
    PORTA.PDR.BYTE |= BSP_PRV_PORTA_NE_PIN_MASK;

    /* Set PORTB.PDR */
    PORTB.PDR.BYTE |= BSP_PRV_PORTB_NE_PIN_MASK;

    /* Set PORTC.PDR */
    PORTC.PDR.BYTE |= BSP_PRV_PORTC_NE_PIN_MASK;

    /* Set PORTD.PDR */
    PORTD.PDR.BYTE |= BSP_PRV_PORTD_NE_PIN_MASK;

    /* Set PORTE.PDR */
    PORTE.PDR.BYTE |= BSP_PRV_PORTE_NE_PIN_MASK;

    /* Set PORTF.PDR */
    PORTF.PDR.BYTE |= BSP_PRV_PORTF_NE_PIN_MASK;

    /* Set PORTG.PDR */
    PORTG.PDR.BYTE |= BSP_PRV_PORTG_NE_PIN_MASK;

    /* Set PORTH.PDR */
    PORTH.PDR.BYTE |= BSP_PRV_PORTH_NE_PIN_MASK;

    /* Set PORTK.PDR */
    PORTK.PDR.BYTE |= BSP_PRV_PORTK_NE_PIN_MASK;
} /* End of function bsp_non_existent_port_init() */

