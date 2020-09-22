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
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : mcu_init.c
* Description  : Performs initialization common to all MCUs in this Group
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 01.12.2015 1.00     First Release
*         : 15.05.2017 1.01     Added port setting of 100 pins.
*         : 28.02.2019 1.02     Fixed coding style.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Get specifics on this MCU. */
#include "platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* RX MCUs come in different packages and different pin counts. For MCUs that do not have the maximum number of pins
 * for their group (e.g. MCU with 100 pins when maximum is 144 pins) these 'non-existent' pins that are not bonded out
 * need to be initialized to save power. The macros below define the non-existent pins on each port for smaller
 * pin count MCUs. If a pin is non-existent then its value is set to a 1. These values are then ORed into the
 * direction registers to set non-existent pins as outputs which can help save power.
 */
#if   BSP_PACKAGE_PINS == 100
    #define BSP_PRV_PORT0_NE_PIN_MASK     (0x07)    /* Missing pins: P00 P01 P02 */
    #define BSP_PRV_PORT1_NE_PIN_MASK     (0x03)    /* Missing pins: P10 P11 */
    #define BSP_PRV_PORT2_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT3_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT4_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT5_NE_PIN_MASK     (0xC0)    /* Missing pins: P56 P57 */
    #define BSP_PRV_PORTA_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTB_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTC_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTD_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTE_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTH_NE_PIN_MASK     (0xF0)    /* Missing pins: PH4 PH5 PH6 PH7 */
    #define BSP_PRV_PORTJ_NE_PIN_MASK     (0x35)    /* Missing pins: PJ0 PJ2 PJ4 PJ5 */
#elif   BSP_PACKAGE_PINS == 80
    #define BSP_PRV_PORT0_NE_PIN_MASK     (0x07)    /* Missing pins: P00 P01 P02 */
    #define BSP_PRV_PORT1_NE_PIN_MASK     (0x03)    /* Missing pins: P10 P11 */
    #define BSP_PRV_PORT2_NE_PIN_MASK     (0x3C)    /* Missing pins: P22 P23 P24 P25 */
    #define BSP_PRV_PORT3_NE_PIN_MASK     (0x08)    /* Missing pins: P33 */
    #define BSP_PRV_PORT4_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT5_NE_PIN_MASK     (0xCF)    /* Missing pins: P50 P51 P52 P53 P56 P57 */
    #define BSP_PRV_PORTA_NE_PIN_MASK     (0x80)    /* Missing pins: PA7 */
    #define BSP_PRV_PORTB_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORTC_NE_PIN_MASK     (0x03)    /* Missing pins: PC0 PC1 */
    #define BSP_PRV_PORTD_NE_PIN_MASK     (0xF8)    /* Missing pins: PD3 PD4 PD5 PD6 PD7 */
    #define BSP_PRV_PORTE_NE_PIN_MASK     (0xC0)    /* Missing pins: PE6 PE7 */
    #define BSP_PRV_PORTH_NE_PIN_MASK     (0xF0)    /* Missing pins: PH4 PH5 PH6 PH7 */
    #define BSP_PRV_PORTJ_NE_PIN_MASK     (0x3D)    /* Missing pins: PJ0 PJ2 PJ3 PJ4 PJ5 */
#elif BSP_PACKAGE_PINS == 64
    #define BSP_PRV_PORT0_NE_PIN_MASK     (0xD7)    /* Missing pins: P00 P01 P02 P04 P06 P07 */
    #define BSP_PRV_PORT1_NE_PIN_MASK     (0x0F)    /* Missing pins: P10 P11 P12 P13 */
    #define BSP_PRV_PORT2_NE_PIN_MASK     (0x3F)    /* Missing pins: P20 P21 P22 P23 P24 P25 */
    #define BSP_PRV_PORT3_NE_PIN_MASK     (0x18)    /* Missing pins: P33 P34 */
    #define BSP_PRV_PORT4_NE_PIN_MASK     (0x00)    /* Missing pins: None */
    #define BSP_PRV_PORT5_NE_PIN_MASK     (0xCF)    /* Missing pins: P50 P51 P52 P53 P56 P57 */
    #define BSP_PRV_PORTA_NE_PIN_MASK     (0xA4)    /* Missing pins: PA2 PA5 PA7 */
    #define BSP_PRV_PORTB_NE_PIN_MASK     (0x14)    /* Missing pins: PB2 PB4 */
    #define BSP_PRV_PORTC_NE_PIN_MASK     (0x03)    /* Missing pins: PC0 PC1 */
    #define BSP_PRV_PORTD_NE_PIN_MASK     (0xFF)    /* Missing pins: PD0 PD1 PD2 PD3 PD4 PD5 PD6 PD7 */
    #define BSP_PRV_PORTE_NE_PIN_MASK     (0xC0)    /* Missing pins: PE6 PE7 */
    #define BSP_PRV_PORTH_NE_PIN_MASK     (0xF0)    /* Missing pins: PH4 PH5 PH6 PH7 */
    #define BSP_PRV_PORTJ_NE_PIN_MASK     (0x3F)    /* Missing pins: PJ0 PJ1 PJ2 PJ3 PJ4 PJ5 */
#elif BSP_PACKAGE_PINS == 48
    #define BSP_PRV_PORT0_NE_PIN_MASK     (0xFF)    /* Missing pins: P00 P01 P02 P03 P04 P05 P06 P07 */
    #define BSP_PRV_PORT1_NE_PIN_MASK     (0x0F)    /* Missing pins: P10 P11 P12 P13 */
    #define BSP_PRV_PORT2_NE_PIN_MASK     (0x3F)    /* Missing pins: P20 P21 P22 P23 P24 P25 */
    #define BSP_PRV_PORT3_NE_PIN_MASK     (0x1C)    /* Missing pins: P32 P33 P34 */
    #define BSP_PRV_PORT4_NE_PIN_MASK     (0x18)    /* Missing pins: P43 P44 */
    #define BSP_PRV_PORT5_NE_PIN_MASK     (0xFF)    /* Missing pins: P50 P51 P52 P53 P54 P55 P56 P57 */
    #define BSP_PRV_PORTA_NE_PIN_MASK     (0xA5)    /* Missing pins: PA0 PA2 PA5 PA7 */
    #define BSP_PRV_PORTB_NE_PIN_MASK     (0xD4)    /* Missing pins: PB2 PB4 PB6 PB7 */
    #define BSP_PRV_PORTC_NE_PIN_MASK     (0x0F)    /* Missing pins: PC0 PC1 PC2 PC3 */
    #define BSP_PRV_PORTD_NE_PIN_MASK     (0xFF)    /* Missing pins: PD0 PD1 PD2 PD3 PD4 PD5 PD6 PD7 */
    #define BSP_PRV_PORTE_NE_PIN_MASK     (0xE1)    /* Missing pins: PE0 PE5 PE6 PE7 */
    #define BSP_PRV_PORTH_NE_PIN_MASK     (0xF0)    /* Missing pins: PH4 PH5 PH6 PH7 */
    #define BSP_PRV_PORTJ_NE_PIN_MASK     (0x3F)    /* Missing pins: PJ0 PJ1 PJ2 PJ3 PJ4 PJ5 */
#else
    #error "ERROR - This package is not defined in mcu_init.c"
#endif

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
* Description  : For MCUs that do not have the 'non-existent' pins for their group (e.g. MCU with 100 pins when
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

    /* Set PORTH.PDR */
    PORTH.PDR.BYTE |= BSP_PRV_PORTH_NE_PIN_MASK;

    /* Set PORTJ.PDR */
    PORTJ.PDR.BYTE |= BSP_PRV_PORTJ_NE_PIN_MASK;
} /* End of function bsp_non_existent_port_init() */

