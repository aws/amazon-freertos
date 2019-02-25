/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
 * No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
 * applicable laws, including copyright laws. 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO 
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the 
 * following link:
 * http://www.renesas.com/disclaimer 
 *
 * Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_riic_rx_pin_config_reference.h
 * Description  : Pin configures the RIIC drivers
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 04.03.2016 1.90     First Release
 *         : 02.06.2017 2.10     Deleted RIIC port definitions of RIIC3.
 **********************************************************************************************************************/
/* Guards against multiple inclusion */
#ifndef RIIC_PIN_CONFIG_H
    #define RIIC_PIN_CONFIG_H
/***********************************************************************************************************************
 Configuration Options
 **********************************************************************************************************************/
/*------------------------------------------------------------------------------*/
/*   Set using port as riic port                                                */
/*------------------------------------------------------------------------------*/
/* Set using port as riic port.                                                                 */
/* If you want to include the port configuration process(RIIC_CFG_PORT_SET_PROCESSING is "1"),  */
/* please choose which ports to use for the SCL/SDA of RIIC with the following setting.         */
/* Select the port group and pin used by setting
 "R_RIIC_CFG_RIICx_SCLx_PORT (select from port group 0 to J)"
 and "R_RIIC_CFG_RIICx_SCLx_BIT (select from pin number 0 to 7)"
 and "R_RIIC_CFG_RIICx_SDAx_PORT (select from port group 0 to J)"
 and "R_RIIC_CFG_RIICx_SDAx_BIT (select from pin number 0 to 7)",
 respectively. */

/* Select the ports (SCL0 and SDA0) to use in RIIC0 */
    #define R_RIIC_CFG_RIIC0_SCL0_PORT   '1'     /* Port Number */
    #define R_RIIC_CFG_RIIC0_SCL0_BIT    '2'     /* Bit Number  */

    #define R_RIIC_CFG_RIIC0_SDA0_PORT   '1'     /* Port Number */
    #define R_RIIC_CFG_RIIC0_SDA0_BIT    '3'     /* Bit Number  */

/* Select the ports (SCL1 and SDA1) to use in RIIC1 */
    #define R_RIIC_CFG_RIIC1_SCL1_PORT   '2'     /* Port Number */
    #define R_RIIC_CFG_RIIC1_SCL1_BIT    '1'     /* Bit Number  */

    #define R_RIIC_CFG_RIIC1_SDA1_PORT   '2'     /* Port Number */
    #define R_RIIC_CFG_RIIC1_SDA1_BIT    '0'     /* Bit Number  */

/* Select the ports (SCL2 and SDA2) to use in RIIC2 */
    #define R_RIIC_CFG_RIIC2_SCL2_PORT   '1'     /* Port Number */
    #define R_RIIC_CFG_RIIC2_SCL2_BIT    '6'     /* Bit Number  */

    #define R_RIIC_CFG_RIIC2_SDA2_PORT   '1'     /* Port Number */
    #define R_RIIC_CFG_RIIC2_SDA2_BIT    '7'     /* Bit Number  */

#endif /* RIIC_PIN_CONFIG_H */

