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
 * File Name    : r_sci_iic_rx_pin_config_reference.h
 * Description  : Pin configures the SCI IIC drivers
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 04.03.2016 1.90     First Release
 *         : 31.08.2017 2.20     Changed default value of macro definitions, below.
 *         :                      - R_SCI_IIC_CFG_SCI2_SSCL2_PORT
 *         :                      - R_SCI_IIC_CFG_SCI2_SSDA2_PORT, R_SCI_IIC_CFG_SCI2_SSDA2_BIT
 *         :                      - R_SCI_IIC_CFG_SCI3_SSCL3_PORT, R_SCI_IIC_CFG_SCI3_SSCL3_BIT
 *         :                      - R_SCI_IIC_CFG_SCI3_SSDA3_PORT, R_SCI_IIC_CFG_SCI3_SSDA3_BIT
 *         :                      - R_SCI_IIC_CFG_SCI5_SSCL5_PORT, R_SCI_IIC_CFG_SCI5_SSCL5_BIT
 *         :                      - R_SCI_IIC_CFG_SCI5_SSDA5_PORT, R_SCI_IIC_CFG_SCI5_SSDA5_BIT
 *         :                      - R_SCI_IIC_CFG_SCI6_SSCL6_BIT
 *         :                      - R_SCI_IIC_CFG_SCI6_SSDA6_BIT
 **********************************************************************************************************************/
/* Guards against multiple inclusion */
#ifndef SCI_IIC_PIN_CONFIG_H
    #define SCI_IIC_PIN_CONFIG_H
/***********************************************************************************************************************
 Configuration Options
 **********************************************************************************************************************/
/*------------------------------------------------------------------------------*/
/*   Set using port as sci iic port                                             */
/*------------------------------------------------------------------------------*/
/* Select the port group and pin used by setting
 "R_SCI_IIC_CFG_SCIx_SSCLx_PORT (select from port group 0 to J)"
 and "R_SCI_IIC_CFG_SCIx_SSCLx_BIT (select from pin number 0 to 7)"
 and "R_SCI_IIC_CFG_SCIx_SSDAx_PORT (select from port group 0 to J)"
 and "R_SCI_IIC_CFG_SCIx_SSDAx_BIT (select from pin number 0 to 7)",
 respectively. */

/* Select the ports (SSCL0 and SSDA0) to use in SCI0 */  
    #define R_SCI_IIC_CFG_SCI0_SSCL0_PORT   '2'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI0_SSCL0_BIT    '1'        /* Bit Number  */  

    #define R_SCI_IIC_CFG_SCI0_SSDA0_PORT   '2'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI0_SSDA0_BIT    '0'        /* Bit Number  */  

/* Select the ports (SSCL1 and SSDA1) to use in SCI1 */
    #define R_SCI_IIC_CFG_SCI1_SSCL1_PORT   '1'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI1_SSCL1_BIT    '5'        /* Bit Number  */  

    #define R_SCI_IIC_CFG_SCI1_SSDA1_PORT   '1'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI1_SSDA1_BIT    '6'        /* Bit Number  */  

/* Select the ports (SSCL2 and SSDA2) to use in SCI2 */
    #define R_SCI_IIC_CFG_SCI2_SSCL2_PORT   '1'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI2_SSCL2_BIT    '2'        /* Bit Number  */  

    #define R_SCI_IIC_CFG_SCI2_SSDA2_PORT   '1'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI2_SSDA2_BIT    '3'        /* Bit Number  */  

/* Select the ports (SSCL3 and SSDA3) to use in SCI3 */
    #define R_SCI_IIC_CFG_SCI3_SSCL3_PORT   '1'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI3_SSCL3_BIT    '6'        /* Bit Number  */  

    #define R_SCI_IIC_CFG_SCI3_SSDA3_PORT   '1'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI3_SSDA3_BIT    '7'        /* Bit Number  */  

/* Select the ports (SSCL4 and SSDA4) to use in SCI4 */
    #define R_SCI_IIC_CFG_SCI4_SSCL4_PORT   'B'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI4_SSCL4_BIT    '0'        /* Bit Number  */  

    #define R_SCI_IIC_CFG_SCI4_SSDA4_PORT   'B'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI4_SSDA4_BIT    '1'        /* Bit Number  */  

/* Select the ports (SSCL5 and SSDA5) to use in SCI5 */
    #define R_SCI_IIC_CFG_SCI5_SSCL5_PORT   'B'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI5_SSCL5_BIT    '1'        /* Bit Number  */  

    #define R_SCI_IIC_CFG_SCI5_SSDA5_PORT   'B'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI5_SSDA5_BIT    '2'        /* Bit Number  */  

/* Select the ports (SSCL6 and SSDA6) to use in SCI6 */
    #define R_SCI_IIC_CFG_SCI6_SSCL6_PORT   'B'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI6_SSCL6_BIT    '1'        /* Bit Number  */  

    #define R_SCI_IIC_CFG_SCI6_SSDA6_PORT   'B'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI6_SSDA6_BIT    '2'        /* Bit Number  */  

/* Select the ports (SSCL7 and SSDA7) to use in SCI7 */
    #define R_SCI_IIC_CFG_SCI7_SSCL7_PORT   '9'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI7_SSCL7_BIT    '2'        /* Bit Number  */  

    #define R_SCI_IIC_CFG_SCI7_SSDA7_PORT   '9'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI7_SSDA7_BIT    '0'        /* Bit Number  */  

/* Select the ports (SSCL8 and SSDA8) to use in SCI8 */
    #define R_SCI_IIC_CFG_SCI8_SSCL8_PORT   'C'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI8_SSCL8_BIT    '6'        /* Bit Number  */  

    #define R_SCI_IIC_CFG_SCI8_SSDA8_PORT   'C'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI8_SSDA8_BIT    '7'        /* Bit Number  */  

/* Select the ports (SSCL9 and SSDA9) to use in SCI9 */
    #define R_SCI_IIC_CFG_SCI9_SSCL9_PORT   'B'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI9_SSCL9_BIT    '6'        /* Bit Number  */  

    #define R_SCI_IIC_CFG_SCI9_SSDA9_PORT   'B'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI9_SSDA9_BIT    '7'        /* Bit Number  */  

/* Select the ports (SSCL10 and SSDA10) to use in SCI10 */
    #define R_SCI_IIC_CFG_SCI10_SSCL10_PORT   '8'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI10_SSCL10_BIT    '1'        /* Bit Number  */  

    #define R_SCI_IIC_CFG_SCI10_SSDA10_PORT   '8'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI10_SSDA10_BIT    '2'        /* Bit Number  */  

/* Select the ports (SSCL11 and SSDA11) to use in SCI11 */
    #define R_SCI_IIC_CFG_SCI11_SSCL11_PORT   '7'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI11_SSCL11_BIT    '6'        /* Bit Number  */  

    #define R_SCI_IIC_CFG_SCI11_SSDA11_PORT   '7'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI11_SSDA11_BIT    '7'        /* Bit Number  */  

/* Select the ports (SSCL12 and SSDA12) to use in SCI12 */
    #define R_SCI_IIC_CFG_SCI12_SSCL12_PORT   'E'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI12_SSCL12_BIT    '2'        /* Bit Number  */  

    #define R_SCI_IIC_CFG_SCI12_SSDA12_PORT   'E'        /* Port Number */  
    #define R_SCI_IIC_CFG_SCI12_SSDA12_BIT    '1'        /* Bit Number  */  

#endif /* SCI_IIC_PIN_CONFIG_H */

