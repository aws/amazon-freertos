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
 * Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_sci_iic_private.h
 * Description  : Functions for using SCI_IIC on RX devices. 
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.07.2013 1.00     First Release
 *         : 30.09.2013 1.10     Change symbol of return value and status
 *         : 01.07.2014 1.20     RX110 support added.
 *         : 22.09.2014 1.30     RX64M support added.
 *         : 01.12.2014 1.40     RX113 support added.
 *         : 15.12.2014 1.50     RX71M support added.
 *         : 27.02.2015 1.60     RX63N support added.
 *         : 29.05.2015 1.70     RX231 support added.
 *         : 31.10.2015 1.80     RX130, RX230, RX23T support added.
 *         : 04.03.2016 1.90     RX24T support added.Changed about the pin definisions.
 *         : 31.08.2017 2.20     Added parameter check of Configuration Options.
 *                               Added macro definition SCI_IIC_PRV_PCR_BASE_REG, SCI_IIC_PRV_PDR_BASE_REG,
 *                                SCI_IIC_PRV_PMR_BASE_REG, SCI_IIC_PRV_ODR0_BASE_REG, SCI_IIC_PRV_ODR1_BASE_REG,
 *                                SCI_IIC_PRV_PFS_BASE_REG and SCI_IIC_PRV_PIDR_BASE_REG.
 **********************************************************************************************************************/
/* Guards against multiple inclusion */
#ifndef SCI_IIC_PRIVATE_H
    #define SCI_IIC_PRIVATE_H
/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
/* Fixed width integer support. */
    #include <stdint.h>
/* bool support */
    #include <stdbool.h>

    #include "r_sci_iic_rx_if.h"
    #include "r_sci_iic_rx_pin_config.h"
    #include "r_sci_iic_rx_config.h"

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/*============================================================================*/
/*  Parameter check of Pin Configuration Options                              */
/*============================================================================*/
/* SCI_IIC port (ascii to number) */
/* SCI0 */
    #if ((('0') <= R_SCI_IIC_CFG_SCI0_SSCL0_PORT) && (('9') >= R_SCI_IIC_CFG_SCI0_SSCL0_PORT))
        #define SCI_IIC_CFG_CH0_SSCL_GP (R_SCI_IIC_CFG_SCI0_SSCL0_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI0_SSCL0_PORT) && (('H') >= R_SCI_IIC_CFG_SCI0_SSCL0_PORT))
        #define SCI_IIC_CFG_CH0_SSCL_GP (R_SCI_IIC_CFG_SCI0_SSCL0_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI0_SSCL0_PORT)
        #define SCI_IIC_CFG_CH0_SSCL_GP (R_SCI_IIC_CFG_SCI0_SSCL0_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI0_SSCL0_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI0_SSCL0_BIT) && (('7') >= R_SCI_IIC_CFG_SCI0_SSCL0_BIT))
        #define SCI_IIC_CFG_CH0_SSCL_PIN    (R_SCI_IIC_CFG_SCI0_SSCL0_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI0_SSCL0_BIT in r_sci_iic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_SCI_IIC_CFG_SCI0_SSDA0_PORT) && (('9') >= R_SCI_IIC_CFG_SCI0_SSDA0_PORT))
        #define SCI_IIC_CFG_CH0_SSDA_GP (R_SCI_IIC_CFG_SCI0_SSDA0_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI0_SSDA0_PORT) && (('H') >= R_SCI_IIC_CFG_SCI0_SSDA0_PORT))
        #define SCI_IIC_CFG_CH0_SSDA_GP (R_SCI_IIC_CFG_SCI0_SSDA0_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI0_SSDA0_PORT)
        #define SCI_IIC_CFG_CH0_SSDA_GP (R_SCI_IIC_CFG_SCI0_SSDA0_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI0_SSDA0_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI0_SSDA0_BIT) && (('7') >= R_SCI_IIC_CFG_SCI0_SSDA0_BIT))
        #define SCI_IIC_CFG_CH0_SSDA_PIN    (R_SCI_IIC_CFG_SCI0_SSDA0_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI0_SSDA0_BIT in r_sci_iic_rx_pin_config.h"
    #endif

/* SCI1 */
    #if ((('0') <= R_SCI_IIC_CFG_SCI1_SSCL1_PORT) && (('9') >= R_SCI_IIC_CFG_SCI1_SSCL1_PORT))
        #define SCI_IIC_CFG_CH1_SSCL_GP (R_SCI_IIC_CFG_SCI1_SSCL1_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI1_SSCL1_PORT) && (('H') >= R_SCI_IIC_CFG_SCI1_SSCL1_PORT))
        #define SCI_IIC_CFG_CH1_SSCL_GP (R_SCI_IIC_CFG_SCI1_SSCL1_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI1_SSCL1_PORT)
        #define SCI_IIC_CFG_CH1_SSCL_GP (R_SCI_IIC_CFG_SCI1_SSCL1_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI1_SSCL1_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI1_SSCL1_BIT) && (('7') >= R_SCI_IIC_CFG_SCI1_SSCL1_BIT))
        #define SCI_IIC_CFG_CH1_SSCL_PIN    (R_SCI_IIC_CFG_SCI1_SSCL1_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI1_SSCL1_BIT in r_sci_iic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_SCI_IIC_CFG_SCI1_SSDA1_PORT) && (('9') >= R_SCI_IIC_CFG_SCI1_SSDA1_PORT))
        #define SCI_IIC_CFG_CH1_SSDA_GP (R_SCI_IIC_CFG_SCI1_SSDA1_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI1_SSDA1_PORT) && (('H') >= R_SCI_IIC_CFG_SCI1_SSDA1_PORT))
        #define SCI_IIC_CFG_CH1_SSDA_GP (R_SCI_IIC_CFG_SCI1_SSDA1_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI1_SSDA1_PORT)
        #define SCI_IIC_CFG_CH1_SSDA_GP (R_SCI_IIC_CFG_SCI1_SSDA1_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI1_SSDA1_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI1_SSDA1_BIT) && (('7') >= R_SCI_IIC_CFG_SCI1_SSDA1_BIT))
        #define SCI_IIC_CFG_CH1_SSDA_PIN    (R_SCI_IIC_CFG_SCI1_SSDA1_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI1_SSDA1_BIT in r_sci_iic_rx_pin_config.h"
    #endif

/* SCI2 */
    #if ((('0') <= R_SCI_IIC_CFG_SCI2_SSCL2_PORT) && (('9') >= R_SCI_IIC_CFG_SCI2_SSCL2_PORT))
        #define SCI_IIC_CFG_CH2_SSCL_GP (R_SCI_IIC_CFG_SCI2_SSCL2_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI2_SSCL2_PORT) && (('H') >= R_SCI_IIC_CFG_SCI2_SSCL2_PORT))
        #define SCI_IIC_CFG_CH2_SSCL_GP (R_SCI_IIC_CFG_SCI2_SSCL2_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI2_SSCL2_PORT)
        #define SCI_IIC_CFG_CH2_SSCL_GP (R_SCI_IIC_CFG_SCI2_SSCL2_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI2_SSCL2_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI2_SSCL2_BIT) && (('7') >= R_SCI_IIC_CFG_SCI2_SSCL2_BIT))
        #define SCI_IIC_CFG_CH2_SSCL_PIN    (R_SCI_IIC_CFG_SCI2_SSCL2_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI2_SSCL2_BIT in r_sci_iic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_SCI_IIC_CFG_SCI2_SSDA2_PORT) && (('9') >= R_SCI_IIC_CFG_SCI2_SSDA2_PORT))
        #define SCI_IIC_CFG_CH2_SSDA_GP (R_SCI_IIC_CFG_SCI2_SSDA2_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI2_SSDA2_PORT) && (('H') >= R_SCI_IIC_CFG_SCI2_SSDA2_PORT))
        #define SCI_IIC_CFG_CH2_SSDA_GP (R_SCI_IIC_CFG_SCI2_SSDA2_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI2_SSDA2_PORT)
        #define SCI_IIC_CFG_CH2_SSDA_GP (R_SCI_IIC_CFG_SCI2_SSDA2_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI2_SSDA2_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI2_SSDA2_BIT) && (('7') >= R_SCI_IIC_CFG_SCI2_SSDA2_BIT))
        #define SCI_IIC_CFG_CH2_SSDA_PIN    (R_SCI_IIC_CFG_SCI2_SSDA2_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI2_SSDA2_BIT in r_sci_iic_rx_pin_config.h"
    #endif

/* SCI3 */
    #if ((('0') <= R_SCI_IIC_CFG_SCI3_SSCL3_PORT) && (('9') >= R_SCI_IIC_CFG_SCI3_SSCL3_PORT))
        #define SCI_IIC_CFG_CH3_SSCL_GP (R_SCI_IIC_CFG_SCI3_SSCL3_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI3_SSCL3_PORT) && (('H') >= R_SCI_IIC_CFG_SCI3_SSCL3_PORT))
        #define SCI_IIC_CFG_CH3_SSCL_GP (R_SCI_IIC_CFG_SCI3_SSCL3_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI3_SSCL3_PORT)
        #define SCI_IIC_CFG_CH3_SSCL_GP (R_SCI_IIC_CFG_SCI3_SSCL3_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI3_SSCL3_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI3_SSCL3_BIT) && (('7') >= R_SCI_IIC_CFG_SCI3_SSCL3_BIT))
        #define SCI_IIC_CFG_CH3_SSCL_PIN    (R_SCI_IIC_CFG_SCI3_SSCL3_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI3_SSCL3_BIT in r_sci_iic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_SCI_IIC_CFG_SCI3_SSDA3_PORT) && (('9') >= R_SCI_IIC_CFG_SCI3_SSDA3_PORT))
        #define SCI_IIC_CFG_CH3_SSDA_GP (R_SCI_IIC_CFG_SCI3_SSDA3_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI3_SSDA3_PORT) && (('H') >= R_SCI_IIC_CFG_SCI3_SSDA3_PORT))
        #define SCI_IIC_CFG_CH3_SSDA_GP (R_SCI_IIC_CFG_SCI3_SSDA3_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI3_SSDA3_PORT)
        #define SCI_IIC_CFG_CH3_SSDA_GP (R_SCI_IIC_CFG_SCI3_SSDA3_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI3_SSDA3_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI3_SSDA3_BIT) && (('7') >= R_SCI_IIC_CFG_SCI3_SSDA3_BIT))
        #define SCI_IIC_CFG_CH3_SSDA_PIN    (R_SCI_IIC_CFG_SCI3_SSDA3_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI3_SSDA3_BIT in r_sci_iic_rx_pin_config.h"
    #endif

/* SCI4 */
    #if ((('0') <= R_SCI_IIC_CFG_SCI4_SSCL4_PORT) && (('9') >= R_SCI_IIC_CFG_SCI4_SSCL4_PORT))
        #define SCI_IIC_CFG_CH4_SSCL_GP (R_SCI_IIC_CFG_SCI4_SSCL4_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI4_SSCL4_PORT) && (('H') >= R_SCI_IIC_CFG_SCI4_SSCL4_PORT))
        #define SCI_IIC_CFG_CH4_SSCL_GP (R_SCI_IIC_CFG_SCI4_SSCL4_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI4_SSCL4_PORT)
        #define SCI_IIC_CFG_CH4_SSCL_GP (R_SCI_IIC_CFG_SCI4_SSCL4_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI4_SSCL4_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI4_SSCL4_BIT) && (('7') >= R_SCI_IIC_CFG_SCI4_SSCL4_BIT))
        #define SCI_IIC_CFG_CH4_SSCL_PIN    (R_SCI_IIC_CFG_SCI4_SSCL4_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI4_SSCL4_BIT in r_sci_iic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_SCI_IIC_CFG_SCI4_SSDA4_PORT) && (('9') >= R_SCI_IIC_CFG_SCI4_SSDA4_PORT))
        #define SCI_IIC_CFG_CH4_SSDA_GP (R_SCI_IIC_CFG_SCI4_SSDA4_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI4_SSDA4_PORT) && (('H') >= R_SCI_IIC_CFG_SCI4_SSDA4_PORT))
        #define SCI_IIC_CFG_CH4_SSDA_GP (R_SCI_IIC_CFG_SCI4_SSDA4_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI4_SSDA4_PORT)
        #define SCI_IIC_CFG_CH4_SSCL_GP (R_SCI_IIC_CFG_SCI4_SSDA4_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI4_SSDA4_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI4_SSDA4_BIT) && (('7') >= R_SCI_IIC_CFG_SCI4_SSDA4_BIT))
        #define SCI_IIC_CFG_CH4_SSDA_PIN    (R_SCI_IIC_CFG_SCI4_SSDA4_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI4_SSDA4_BIT in r_sci_iic_rx_pin_config.h"
    #endif

/* SCI5 */
    #if ((('0') <= R_SCI_IIC_CFG_SCI5_SSCL5_PORT) && (('9') >= R_SCI_IIC_CFG_SCI5_SSCL5_PORT))
        #define SCI_IIC_CFG_CH5_SSCL_GP (R_SCI_IIC_CFG_SCI5_SSCL5_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI5_SSCL5_PORT) && (('H') >= R_SCI_IIC_CFG_SCI5_SSCL5_PORT))
        #define SCI_IIC_CFG_CH5_SSCL_GP (R_SCI_IIC_CFG_SCI5_SSCL5_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI5_SSCL5_PORT)
        #define SCI_IIC_CFG_CH5_SSCL_GP (R_SCI_IIC_CFG_SCI5_SSCL5_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI5_SSCL5_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI5_SSCL5_BIT) && (('7') >= R_SCI_IIC_CFG_SCI5_SSCL5_BIT))
        #define SCI_IIC_CFG_CH5_SSCL_PIN    (R_SCI_IIC_CFG_SCI5_SSCL5_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI5_SSCL5_BIT in r_sci_iic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_SCI_IIC_CFG_SCI5_SSDA5_PORT) && (('9') >= R_SCI_IIC_CFG_SCI5_SSDA5_PORT))
        #define SCI_IIC_CFG_CH5_SSDA_GP (R_SCI_IIC_CFG_SCI5_SSDA5_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI5_SSDA5_PORT) && (('H') >= R_SCI_IIC_CFG_SCI5_SSDA5_PORT))
        #define SCI_IIC_CFG_CH5_SSDA_GP (R_SCI_IIC_CFG_SCI5_SSDA5_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI5_SSDA5_PORT)
        #define SCI_IIC_CFG_CH5_SSDAL_GP (R_SCI_IIC_CFG_SCI5_SSDA5_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI5_SSDA5_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI5_SSDA5_BIT) && (('7') >= R_SCI_IIC_CFG_SCI5_SSDA5_BIT))
        #define SCI_IIC_CFG_CH5_SSDA_PIN    (R_SCI_IIC_CFG_SCI5_SSDA5_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI5_SSDA5_BIT in r_sci_iic_rx_pin_config.h"
    #endif

/* SCI6 */
    #if ((('0') <= R_SCI_IIC_CFG_SCI6_SSCL6_PORT) && (('9') >= R_SCI_IIC_CFG_SCI6_SSCL6_PORT))
        #define SCI_IIC_CFG_CH6_SSCL_GP (R_SCI_IIC_CFG_SCI6_SSCL6_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI6_SSCL6_PORT) && (('H') >= R_SCI_IIC_CFG_SCI6_SSCL6_PORT))
        #define SCI_IIC_CFG_CH6_SSCL_GP (R_SCI_IIC_CFG_SCI6_SSCL6_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI6_SSCL6_PORT)
        #define SCI_IIC_CFG_CH6_SSCL_GP (R_SCI_IIC_CFG_SCI6_SSCL6_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI6_SSCL6_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI6_SSCL6_BIT) && (('7') >= R_SCI_IIC_CFG_SCI6_SSCL6_BIT))
        #define SCI_IIC_CFG_CH6_SSCL_PIN    (R_SCI_IIC_CFG_SCI6_SSCL6_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI6_SSCL6_BIT in r_sci_iic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_SCI_IIC_CFG_SCI6_SSDA6_PORT) && (('9') >= R_SCI_IIC_CFG_SCI6_SSDA6_PORT))
        #define SCI_IIC_CFG_CH6_SSDA_GP (R_SCI_IIC_CFG_SCI6_SSDA6_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI6_SSDA6_PORT) && (('H') >= R_SCI_IIC_CFG_SCI6_SSDA6_PORT))
        #define SCI_IIC_CFG_CH6_SSDA_GP (R_SCI_IIC_CFG_SCI6_SSDA6_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI6_SSDA6_PORT)
        #define SCI_IIC_CFG_CH6_SSDA_GP (R_SCI_IIC_CFG_SCI6_SSDA6_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI6_SSDA6_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI6_SSDA6_BIT) && (('7') >= R_SCI_IIC_CFG_SCI6_SSDA6_BIT))
        #define SCI_IIC_CFG_CH6_SSDA_PIN    (R_SCI_IIC_CFG_SCI6_SSDA6_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI6_SSDA6_BIT in r_sci_iic_rx_pin_config.h"
    #endif

/* SCI7 */
    #if ((('0') <= R_SCI_IIC_CFG_SCI7_SSCL7_PORT) && (('9') >= R_SCI_IIC_CFG_SCI7_SSCL7_PORT))
        #define SCI_IIC_CFG_CH7_SSCL_GP (R_SCI_IIC_CFG_SCI7_SSCL7_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI7_SSCL7_PORT) && (('H') >= R_SCI_IIC_CFG_SCI7_SSCL7_PORT))
        #define SCI_IIC_CFG_CH7_SSCL_GP (R_SCI_IIC_CFG_SCI7_SSCL7_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI7_SSCL7_PORT)
        #define SCI_IIC_CFG_CH7_SSCL_GP (R_SCI_IIC_CFG_SCI7_SSCL7_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI7_SSCL7_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI7_SSCL7_BIT) && (('7') >= R_SCI_IIC_CFG_SCI7_SSCL7_BIT))
        #define SCI_IIC_CFG_CH7_SSCL_PIN    (R_SCI_IIC_CFG_SCI7_SSCL7_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI7_SSCL7_BIT in r_sci_iic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_SCI_IIC_CFG_SCI7_SSDA7_PORT) && (('9') >= R_SCI_IIC_CFG_SCI7_SSDA7_PORT))
        #define SCI_IIC_CFG_CH7_SSDA_GP (R_SCI_IIC_CFG_SCI7_SSDA7_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI7_SSDA7_PORT) && (('H') >= R_SCI_IIC_CFG_SCI7_SSDA7_PORT))
        #define SCI_IIC_CFG_CH7_SSDA_GP (R_SCI_IIC_CFG_SCI7_SSDA7_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI7_SSDA7_PORT)
        #define SCI_IIC_CFG_CH7_SSDA_GP (R_SCI_IIC_CFG_SCI7_SSDA7_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI7_SSDA7_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI7_SSDA7_BIT) && (('7') >= R_SCI_IIC_CFG_SCI7_SSDA7_BIT))
        #define SCI_IIC_CFG_CH7_SSDA_PIN    (R_SCI_IIC_CFG_SCI7_SSDA7_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI7_SSDA7_BIT in r_sci_iic_rx_pin_config.h"
    #endif

/* SCI8 */
    #if ((('0') <= R_SCI_IIC_CFG_SCI8_SSCL8_PORT) && (('9') >= R_SCI_IIC_CFG_SCI8_SSCL8_PORT))
        #define SCI_IIC_CFG_CH8_SSCL_GP (R_SCI_IIC_CFG_SCI8_SSCL8_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI8_SSCL8_PORT) && (('H') >= R_SCI_IIC_CFG_SCI8_SSCL8_PORT))
        #define SCI_IIC_CFG_CH8_SSCL_GP (R_SCI_IIC_CFG_SCI8_SSCL8_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI8_SSCL8_PORT)
        #define SCI_IIC_CFG_CH8_SSCL_GP (R_SCI_IIC_CFG_SCI8_SSCL8_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI8_SSCL8_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI8_SSCL8_BIT) && (('7') >= R_SCI_IIC_CFG_SCI8_SSCL8_BIT))
        #define SCI_IIC_CFG_CH8_SSCL_PIN    (R_SCI_IIC_CFG_SCI8_SSCL8_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI8_SSCL8_BIT in r_sci_iic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_SCI_IIC_CFG_SCI8_SSDA8_PORT) && (('9') >= R_SCI_IIC_CFG_SCI8_SSDA8_PORT))
        #define SCI_IIC_CFG_CH8_SSDA_GP (R_SCI_IIC_CFG_SCI8_SSDA8_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI8_SSDA8_PORT) && (('H') >= R_SCI_IIC_CFG_SCI8_SSDA8_PORT))
        #define SCI_IIC_CFG_CH8_SSDA_GP (R_SCI_IIC_CFG_SCI8_SSDA8_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI8_SSDA8_PORT)
        #define SCI_IIC_CFG_CH8_SSDA_GP (R_SCI_IIC_CFG_SCI8_SSDA8_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI8_SSDA8_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI8_SSDA8_BIT) && (('7') >= R_SCI_IIC_CFG_SCI8_SSDA8_BIT))
        #define SCI_IIC_CFG_CH8_SSDA_PIN    (R_SCI_IIC_CFG_SCI8_SSDA8_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI8_SSDA8_BIT in r_sci_iic_rx_pin_config.h"
    #endif

/* SCI9 */
    #if ((('0') <= R_SCI_IIC_CFG_SCI9_SSCL9_PORT) && (('9') >= R_SCI_IIC_CFG_SCI9_SSCL9_PORT))
        #define SCI_IIC_CFG_CH9_SSCL_GP (R_SCI_IIC_CFG_SCI9_SSCL9_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI9_SSCL9_PORT) && (('H') >= R_SCI_IIC_CFG_SCI9_SSCL9_PORT))
        #define SCI_IIC_CFG_CH9_SSCL_GP (R_SCI_IIC_CFG_SCI9_SSCL9_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI9_SSCL9_PORT)
        #define SCI_IIC_CFG_CH9_SSCL_GP (R_SCI_IIC_CFG_SCI9_SSCL9_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI9_SSCL9_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI9_SSCL9_BIT) && (('7') >= R_SCI_IIC_CFG_SCI9_SSCL9_BIT))
        #define SCI_IIC_CFG_CH9_SSCL_PIN    (R_SCI_IIC_CFG_SCI9_SSCL9_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI9_SSCL9_BIT in r_sci_iic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_SCI_IIC_CFG_SCI9_SSDA9_PORT) && (('9') >= R_SCI_IIC_CFG_SCI9_SSDA9_PORT))
        #define SCI_IIC_CFG_CH9_SSDA_GP (R_SCI_IIC_CFG_SCI9_SSDA9_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI9_SSDA9_PORT) && (('H') >= R_SCI_IIC_CFG_SCI9_SSDA9_PORT))
        #define SCI_IIC_CFG_CH9_SSDA_GP (R_SCI_IIC_CFG_SCI9_SSDA9_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI9_SSDA9_PORT)
        #define SCI_IIC_CFG_CH9_SSDA_GP (R_SCI_IIC_CFG_SCI9_SSDA9_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI9_SSDA9_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI9_SSDA9_BIT) && (('7') >= R_SCI_IIC_CFG_SCI9_SSDA9_BIT))
        #define SCI_IIC_CFG_CH9_SSDA_PIN    (R_SCI_IIC_CFG_SCI9_SSDA9_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI9_SSDA9_BIT in r_sci_iic_rx_pin_config.h"
    #endif

/* SCI10 */
    #if ((('0') <= R_SCI_IIC_CFG_SCI10_SSCL10_PORT) && (('9') >= R_SCI_IIC_CFG_SCI10_SSCL10_PORT))
        #define SCI_IIC_CFG_CH10_SSCL_GP (R_SCI_IIC_CFG_SCI10_SSCL10_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI10_SSCL10_PORT) && (('H') >= R_SCI_IIC_CFG_SCI10_SSCL10_PORT))
        #define SCI_IIC_CFG_CH10_SSCL_GP (R_SCI_IIC_CFG_SCI10_SSCL10_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI10_SSCL10_PORT)
        #define SCI_IIC_CFG_CH10_SSCL_GP (R_SCI_IIC_CFG_SCI10_SSCL10_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI10_SSCL10_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI10_SSCL10_BIT) && (('7') >= R_SCI_IIC_CFG_SCI10_SSCL10_BIT))
        #define SCI_IIC_CFG_CH10_SSCL_PIN    (R_SCI_IIC_CFG_SCI10_SSCL10_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI10_SSCL10_BIT in r_sci_iic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_SCI_IIC_CFG_SCI10_SSDA10_PORT) && (('9') >= R_SCI_IIC_CFG_SCI10_SSDA10_PORT))
        #define SCI_IIC_CFG_CH10_SSDA_GP (R_SCI_IIC_CFG_SCI10_SSDA10_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI10_SSDA10_PORT) && (('H') >= R_SCI_IIC_CFG_SCI10_SSDA10_PORT))
        #define SCI_IIC_CFG_CH10_SSDA_GP (R_SCI_IIC_CFG_SCI10_SSDA10_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI10_SSDA10_PORT)
        #define SCI_IIC_CFG_CH10_SSDA_GP (R_SCI_IIC_CFG_SCI10_SSDA10_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI10_SSDA10_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI10_SSDA10_BIT) && (('7') >= R_SCI_IIC_CFG_SCI10_SSDA10_BIT))
        #define SCI_IIC_CFG_CH10_SSDA_PIN    (R_SCI_IIC_CFG_SCI10_SSDA10_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI10_SSDA10_BIT in r_sci_iic_rx_pin_config.h"
    #endif

/* SCI11 */
    #if ((('0') <= R_SCI_IIC_CFG_SCI11_SSCL11_PORT) && (('9') >= R_SCI_IIC_CFG_SCI11_SSCL11_PORT))
        #define SCI_IIC_CFG_CH11_SSCL_GP (R_SCI_IIC_CFG_SCI11_SSCL11_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI11_SSCL11_PORT) && (('H') >= R_SCI_IIC_CFG_SCI11_SSCL11_PORT))
        #define SCI_IIC_CFG_CH11_SSCL_GP (R_SCI_IIC_CFG_SCI11_SSCL11_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI11_SSCL11_PORT)
        #define SCI_IIC_CFG_CH11_SSCL_GP (R_SCI_IIC_CFG_SCI11_SSCL11_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI11_SSCL11_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI11_SSCL11_BIT) && (('7') >= R_SCI_IIC_CFG_SCI11_SSCL11_BIT))
        #define SCI_IIC_CFG_CH11_SSCL_PIN    (R_SCI_IIC_CFG_SCI11_SSCL11_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI11_SSCL11_BIT in r_sci_iic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_SCI_IIC_CFG_SCI11_SSDA11_PORT) && (('9') >= R_SCI_IIC_CFG_SCI11_SSDA11_PORT))
        #define SCI_IIC_CFG_CH11_SSDA_GP (R_SCI_IIC_CFG_SCI11_SSDA11_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI11_SSDA11_PORT) && (('H') >= R_SCI_IIC_CFG_SCI11_SSDA11_PORT))
        #define SCI_IIC_CFG_CH11_SSDA_GP (R_SCI_IIC_CFG_SCI11_SSDA11_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI11_SSDA11_PORT)
        #define SCI_IIC_CFG_CH11_SSDA_GP (R_SCI_IIC_CFG_SCI11_SSDA11_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI11_SSDA11_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI11_SSDA11_BIT) && (('7') >= R_SCI_IIC_CFG_SCI11_SSDA11_BIT))
        #define SCI_IIC_CFG_CH11_SSDA_PIN    (R_SCI_IIC_CFG_SCI11_SSDA11_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI11_SSDA11_BIT in r_sci_iic_rx_pin_config.h"
    #endif

/* SCI12 */
    #if ((('0') <= R_SCI_IIC_CFG_SCI12_SSCL12_PORT) && (('9') >= R_SCI_IIC_CFG_SCI12_SSCL12_PORT))
        #define SCI_IIC_CFG_CH12_SSCL_GP (R_SCI_IIC_CFG_SCI12_SSCL12_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI12_SSCL12_PORT) && (('H') >= R_SCI_IIC_CFG_SCI12_SSCL12_PORT))
        #define SCI_IIC_CFG_CH12_SSCL_GP (R_SCI_IIC_CFG_SCI12_SSCL12_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI12_SSCL12_PORT)
        #define SCI_IIC_CFG_CH12_SSCL_GP (R_SCI_IIC_CFG_SCI12_SSCL12_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI12_SSCL12_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI12_SSCL12_BIT) && (('7') >= R_SCI_IIC_CFG_SCI12_SSCL12_BIT))
        #define SCI_IIC_CFG_CH12_SSCL_PIN    (R_SCI_IIC_CFG_SCI12_SSCL12_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI12_SSCL12_BIT in r_sci_iic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_SCI_IIC_CFG_SCI12_SSDA12_PORT) && (('9') >= R_SCI_IIC_CFG_SCI12_SSDA12_PORT))
        #define SCI_IIC_CFG_CH12_SSDA_GP (R_SCI_IIC_CFG_SCI12_SSDA12_PORT - 0x30)
    #elif ((('A') <= R_SCI_IIC_CFG_SCI12_SSDA12_PORT) && (('H') >= R_SCI_IIC_CFG_SCI12_SSDA12_PORT))
        #define SCI_IIC_CFG_CH12_SSDA_GP (R_SCI_IIC_CFG_SCI12_SSDA12_PORT - 0x37)
    #elif (('J') == R_SCI_IIC_CFG_SCI12_SSDA12_PORT)
        #define SCI_IIC_CFG_CH12_SSDA_GP (R_SCI_IIC_CFG_SCI12_SSDA12_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI12_SSDA12_PORT in r_sci_iic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_SCI_IIC_CFG_SCI12_SSDA12_BIT) && (('7') >= R_SCI_IIC_CFG_SCI12_SSDA12_BIT))
        #define SCI_IIC_CFG_CH12_SSDA_PIN    (R_SCI_IIC_CFG_SCI12_SSDA12_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_SCI_IIC_CFG_SCI12_SSDA12_BIT in r_sci_iic_rx_pin_config.h"
    #endif

/*============================================================================*/
/*  Parameter check of Configuration Options                                  */
/*============================================================================*/
    #if ((0 != SCI_IIC_CFG_PARAM_CHECKING_ENABLE) && (1 != SCI_IIC_CFG_PARAM_CHECKING_ENABLE))
        #error "ERROR - SCI_IIC_CFG_PARAM_CHECKING_ENABLE - Parameter error in configures file.  "
    #endif

    #if ((0 > SCI_IIC_CFG_BUS_CHECK_COUNTER) || (0xFFFFFFFF < SCI_IIC_CFG_BUS_CHECK_COUNTER))
        #error "ERROR - SCI_IIC_CFG_BUS_CHECK_COUNTER - Parameter error in configures file.  "
    #endif

    #if ((0 != SCI_IIC_CFG_PORT_SETTING_PROCESSING) && (1 != SCI_IIC_CFG_PORT_SETTING_PROCESSING))
        #error "ERROR - SCI_IIC_CFG_PORT_SETTING_PROCESSING - Parameter error in configures file.  "
    #endif

/* check of channel 0 */
    #if (1 == SCI_IIC_CFG_CH0_INCLUDED)
        #if ((0 >= SCI_IIC_CFG_CH0_BITRATE_BPS) || (384000 < SCI_IIC_CFG_CH0_BITRATE_BPS))
            #error "ERROR - SCI_IIC_CFG_CH0_BITRATE_BPS - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH0_INT_PRIORITY) || ( 15 < SCI_IIC_CFG_CH0_INT_PRIORITY))
            #error "ERROR - SCI_IIC_CFG_CH0_INT_PRIORITY - Parameter error in configures file.  "
        #endif
        #if ((0 > SCI_IIC_CFG_CH0_DIGITAL_FILTER) || (1 < SCI_IIC_CFG_CH0_DIGITAL_FILTER))
            #error "ERROR - SCI_IIC_CFG_CH0_DIGITAL_FILTER - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH0_FILTER_CLOCK) || (4 < SCI_IIC_CFG_CH0_FILTER_CLOCK))
            #error "ERROR - SCI_IIC_CFG_CH0_FILTER_CLOCK - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH0_SSDA_DELAY_SELECT) || (31 < SCI_IIC_CFG_CH0_SSDA_DELAY_SELECT))
            #error "ERROR - SCI_IIC_CFG_CH0_SSDA_DELAY_SELECT - Parameter error in configures file.  "
        #endif
    #elif (0 == SCI_IIC_CFG_CH0_INCLUDED)
/* do not check */
    #else
        #error "ERROR - SCI_IIC_CFG_CH0_INCLUDED - Parameter error in configures file.  "
    #endif

/* check of channel 1 */
    #if (1 == SCI_IIC_CFG_CH1_INCLUDED)
        #if ((0 >= SCI_IIC_CFG_CH1_BITRATE_BPS) || (384000 < SCI_IIC_CFG_CH1_BITRATE_BPS))
            #error "ERROR - SCI_IIC_CFG_CH1_BITRATE_BPS - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH1_INT_PRIORITY) || ( 15 < SCI_IIC_CFG_CH1_INT_PRIORITY))
            #error "ERROR - SCI_IIC_CFG_CH1_INT_PRIORITY - Parameter error in configures file.  "
        #endif
        #if ((0 > SCI_IIC_CFG_CH1_DIGITAL_FILTER) || (1 < SCI_IIC_CFG_CH1_DIGITAL_FILTER))
            #error "ERROR - SCI_IIC_CFG_CH1_DIGITAL_FILTER - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH1_FILTER_CLOCK) || (4 < SCI_IIC_CFG_CH1_FILTER_CLOCK))
            #error "ERROR - SCI_IIC_CFG_CH1_FILTER_CLOCK - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH1_SSDA_DELAY_SELECT) || (31 < SCI_IIC_CFG_CH1_SSDA_DELAY_SELECT))
            #error "ERROR - SCI_IIC_CFG_CH1_SSDA_DELAY_SELECT - Parameter error in configures file.  "
        #endif
    #elif (0 == SCI_IIC_CFG_CH1_INCLUDED)
/* do not check */
    #else
        #error "ERROR - SCI_IIC_CFG_CH1_INCLUDED - Parameter error in configures file.  "
    #endif

/* check of channel 2 */
    #if (1 == SCI_IIC_CFG_CH2_INCLUDED)
        #if ((0 >= SCI_IIC_CFG_CH2_BITRATE_BPS) || (384000 < SCI_IIC_CFG_CH2_BITRATE_BPS))
            #error "ERROR - SCI_IIC_CFG_CH2_BITRATE_BPS - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH2_INT_PRIORITY) || ( 15 < SCI_IIC_CFG_CH2_INT_PRIORITY))
            #error "ERROR - SCI_IIC_CFG_CH2_INT_PRIORITY - Parameter error in configures file.  "
        #endif
        #if ((0 > SCI_IIC_CFG_CH2_DIGITAL_FILTER) || (1 < SCI_IIC_CFG_CH2_DIGITAL_FILTER))
            #error "ERROR - SCI_IIC_CFG_CH2_DIGITAL_FILTER - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH2_FILTER_CLOCK) || (4 < SCI_IIC_CFG_CH2_FILTER_CLOCK))
            #error "ERROR - SCI_IIC_CFG_CH2_FILTER_CLOCK - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH2_SSDA_DELAY_SELECT) || (31 < SCI_IIC_CFG_CH2_SSDA_DELAY_SELECT))
            #error "ERROR - SCI_IIC_CFG_CH2_SSDA_DELAY_SELECT - Parameter error in configures file.  "
        #endif
    #elif (0 == SCI_IIC_CFG_CH2_INCLUDED)
/* do not check */
    #else
        #error "ERROR - SCI_IIC_CFG_CH2_INCLUDED - Parameter error in configures file.  "
    #endif

/* check of channel 3 */
    #if (1 == SCI_IIC_CFG_CH3_INCLUDED)
        #if ((0 >= SCI_IIC_CFG_CH3_BITRATE_BPS) || (384000 < SCI_IIC_CFG_CH3_BITRATE_BPS))
            #error "ERROR - SCI_IIC_CFG_CH3_BITRATE_BPS - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH3_INT_PRIORITY) || ( 15 < SCI_IIC_CFG_CH3_INT_PRIORITY))
            #error "ERROR - SCI_IIC_CFG_CH3_INT_PRIORITY - Parameter error in configures file.  "
        #endif
        #if ((0 > SCI_IIC_CFG_CH3_DIGITAL_FILTER) || (1 < SCI_IIC_CFG_CH3_DIGITAL_FILTER))
            #error "ERROR - SCI_IIC_CFG_CH3_DIGITAL_FILTER - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH3_FILTER_CLOCK) || (4 < SCI_IIC_CFG_CH3_FILTER_CLOCK))
            #error "ERROR - SCI_IIC_CFG_CH3_FILTER_CLOCK - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH3_SSDA_DELAY_SELECT) || (31 < SCI_IIC_CFG_CH3_SSDA_DELAY_SELECT))
            #error "ERROR - SCI_IIC_CFG_CH3_SSDA_DELAY_SELECT - Parameter error in configures file.  "
        #endif
    #elif (0 == SCI_IIC_CFG_CH3_INCLUDED)
/* do not check */
    #else
        #error "ERROR - SCI_IIC_CFG_CH3_INCLUDED - Parameter error in configures file.  "
    #endif

/* check of channel 4 */
    #if (1 == SCI_IIC_CFG_CH4_INCLUDED)
        #if ((0 >= SCI_IIC_CFG_CH4_BITRATE_BPS) || (384000 < SCI_IIC_CFG_CH4_BITRATE_BPS))
            #error "ERROR - SCI_IIC_CFG_CH4_BITRATE_BPS - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH4_INT_PRIORITY) || ( 15 < SCI_IIC_CFG_CH4_INT_PRIORITY))
            #error "ERROR - SCI_IIC_CFG_CH4_INT_PRIORITY - Parameter error in configures file.  "
        #endif
        #if ((0 > SCI_IIC_CFG_CH4_DIGITAL_FILTER) || (1 < SCI_IIC_CFG_CH4_DIGITAL_FILTER))
            #error "ERROR - SCI_IIC_CFG_CH4_DIGITAL_FILTER - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH4_FILTER_CLOCK) || (4 < SCI_IIC_CFG_CH4_FILTER_CLOCK))
            #error "ERROR - SCI_IIC_CFG_CH4_FILTER_CLOCK - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH4_SSDA_DELAY_SELECT) || (31 < SCI_IIC_CFG_CH4_SSDA_DELAY_SELECT))
            #error "ERROR - SCI_IIC_CFG_CH4_SSDA_DELAY_SELECT - Parameter error in configures file.  "
        #endif
    #elif (0 == SCI_IIC_CFG_CH4_INCLUDED)
/* do not check */
    #else
        #error "ERROR - SCI_IIC_CFG_CH4_INCLUDED - Parameter error in configures file.  "
    #endif

/* check of channel 5 */
    #if (1 == SCI_IIC_CFG_CH5_INCLUDED)
        #if ((0 >= SCI_IIC_CFG_CH5_BITRATE_BPS) || (384000 < SCI_IIC_CFG_CH5_BITRATE_BPS))
            #error "ERROR - SCI_IIC_CFG_CH5_BITRATE_BPS - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH5_INT_PRIORITY) || ( 15 < SCI_IIC_CFG_CH5_INT_PRIORITY))
            #error "ERROR - SCI_IIC_CFG_CH5_INT_PRIORITY - Parameter error in configures file.  "
        #endif
        #if ((0 > SCI_IIC_CFG_CH5_DIGITAL_FILTER) || (1 < SCI_IIC_CFG_CH5_DIGITAL_FILTER))
            #error "ERROR - SCI_IIC_CFG_CH5_DIGITAL_FILTER - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH5_FILTER_CLOCK) || (4 < SCI_IIC_CFG_CH5_FILTER_CLOCK))
            #error "ERROR - SCI_IIC_CFG_CH5_FILTER_CLOCK - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH5_SSDA_DELAY_SELECT) || (31 < SCI_IIC_CFG_CH5_SSDA_DELAY_SELECT))
            #error "ERROR - SCI_IIC_CFG_CH5_SSDA_DELAY_SELECT - Parameter error in configures file.  "
        #endif
    #elif (0 == SCI_IIC_CFG_CH5_INCLUDED)
/* do not check */
    #else
        #error "ERROR - SCI_IIC_CFG_CH5_INCLUDED - Parameter error in configures file.  "
    #endif

/* check of channel 6 */
    #if (1 == SCI_IIC_CFG_CH6_INCLUDED)
        #if ((0 >= SCI_IIC_CFG_CH6_BITRATE_BPS) || (384000 < SCI_IIC_CFG_CH6_BITRATE_BPS))
            #error "ERROR - SCI_IIC_CFG_CH6_BITRATE_BPS - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH6_INT_PRIORITY) || ( 15 < SCI_IIC_CFG_CH6_INT_PRIORITY))
            #error "ERROR - SCI_IIC_CFG_CH6_INT_PRIORITY - Parameter error in configures file.  "
        #endif
        #if ((0 > SCI_IIC_CFG_CH6_DIGITAL_FILTER) || (1 < SCI_IIC_CFG_CH6_DIGITAL_FILTER))
            #error "ERROR - SCI_IIC_CFG_CH6_DIGITAL_FILTER - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH6_FILTER_CLOCK) || (4 < SCI_IIC_CFG_CH6_FILTER_CLOCK))
            #error "ERROR - SCI_IIC_CFG_CH6_FILTER_CLOCK - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH6_SSDA_DELAY_SELECT) || (31 < SCI_IIC_CFG_CH6_SSDA_DELAY_SELECT))
            #error "ERROR - SCI_IIC_CFG_CH6_SSDA_DELAY_SELECT - Parameter error in configures file.  "
        #endif
    #elif (0 == SCI_IIC_CFG_CH6_INCLUDED)
/* do not check */
    #else
        #error "ERROR - SCI_IIC_CFG_CH6_INCLUDED - Parameter error in configures file.  "
    #endif

/* check of channel 7 */
    #if (1 == SCI_IIC_CFG_CH7_INCLUDED)
        #if ((0 >= SCI_IIC_CFG_CH7_BITRATE_BPS) || (384000 < SCI_IIC_CFG_CH7_BITRATE_BPS))
            #error "ERROR - SCI_IIC_CFG_CH7_BITRATE_BPS - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH7_INT_PRIORITY) || ( 15 < SCI_IIC_CFG_CH7_INT_PRIORITY))
            #error "ERROR - SCI_IIC_CFG_CH7_INT_PRIORITY - Parameter error in configures file.  "
        #endif
        #if ((0 > SCI_IIC_CFG_CH7_DIGITAL_FILTER) || (1 < SCI_IIC_CFG_CH7_DIGITAL_FILTER))
            #error "ERROR - SCI_IIC_CFG_CH7_DIGITAL_FILTER - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH7_FILTER_CLOCK) || (4 < SCI_IIC_CFG_CH7_FILTER_CLOCK))
            #error "ERROR - SCI_IIC_CFG_CH7_FILTER_CLOCK - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH7_SSDA_DELAY_SELECT) || (31 < SCI_IIC_CFG_CH7_SSDA_DELAY_SELECT))
            #error "ERROR - SCI_IIC_CFG_CH7_SSDA_DELAY_SELECT - Parameter error in configures file.  "
        #endif
    #elif (0 == SCI_IIC_CFG_CH7_INCLUDED)
/* do not check */
    #else
        #error "ERROR - SCI_IIC_CFG_CH7_INCLUDED - Parameter error in configures file.  "
    #endif

/* check of channel 8 */
    #if (1 == SCI_IIC_CFG_CH8_INCLUDED)
        #if ((0 >= SCI_IIC_CFG_CH8_BITRATE_BPS) || (384000 < SCI_IIC_CFG_CH8_BITRATE_BPS))
            #error "ERROR - SCI_IIC_CFG_CH8_BITRATE_BPS - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH8_INT_PRIORITY) || ( 15 < SCI_IIC_CFG_CH8_INT_PRIORITY))
            #error "ERROR - SCI_IIC_CFG_CH8_INT_PRIORITY - Parameter error in configures file.  "
        #endif
        #if ((0 > SCI_IIC_CFG_CH8_DIGITAL_FILTER) || (1 < SCI_IIC_CFG_CH8_DIGITAL_FILTER))
            #error "ERROR - SCI_IIC_CFG_CH8_DIGITAL_FILTER - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH8_FILTER_CLOCK) || (4 < SCI_IIC_CFG_CH8_FILTER_CLOCK))
            #error "ERROR - SCI_IIC_CFG_CH8_FILTER_CLOCK - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH8_SSDA_DELAY_SELECT) || (31 < SCI_IIC_CFG_CH8_SSDA_DELAY_SELECT))
            #error "ERROR - SCI_IIC_CFG_CH8_SSDA_DELAY_SELECT - Parameter error in configures file.  "
        #endif
    #elif (0 == SCI_IIC_CFG_CH8_INCLUDED)
/* do not check */
    #else
        #error "ERROR - SCI_IIC_CFG_CH8_INCLUDED - Parameter error in configures file.  "
    #endif

/* check of channel 9 */
    #if (1 == SCI_IIC_CFG_CH9_INCLUDED)
        #if ((0 >= SCI_IIC_CFG_CH9_BITRATE_BPS) || (384000 < SCI_IIC_CFG_CH9_BITRATE_BPS))
            #error "ERROR - SCI_IIC_CFG_CH9_BITRATE_BPS - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH9_INT_PRIORITY) || ( 15 < SCI_IIC_CFG_CH9_INT_PRIORITY))
            #error "ERROR - SCI_IIC_CFG_CH9_INT_PRIORITY - Parameter error in configures file.  "
        #endif
        #if ((0 > SCI_IIC_CFG_CH9_DIGITAL_FILTER) || (1 < SCI_IIC_CFG_CH9_DIGITAL_FILTER))
            #error "ERROR - SCI_IIC_CFG_CH9_DIGITAL_FILTER - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH9_FILTER_CLOCK) || (4 < SCI_IIC_CFG_CH9_FILTER_CLOCK))
            #error "ERROR - SCI_IIC_CFG_CH9_FILTER_CLOCK - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH9_SSDA_DELAY_SELECT) || (31 < SCI_IIC_CFG_CH9_SSDA_DELAY_SELECT))
            #error "ERROR - SCI_IIC_CFG_CH9_SSDA_DELAY_SELECT - Parameter error in configures file.  "
        #endif
    #elif (0 == SCI_IIC_CFG_CH9_INCLUDED)
/* do not check */
    #else
        #error "ERROR - SCI_IIC_CFG_CH9_INCLUDED - Parameter error in configures file.  "
    #endif

/* check of channel 10 */
    #if (1 == SCI_IIC_CFG_CH10_INCLUDED)
        #if ((0 >= SCI_IIC_CFG_CH10_BITRATE_BPS) || (384000 < SCI_IIC_CFG_CH10_BITRATE_BPS))
            #error "ERROR - SCI_IIC_CFG_CH10_BITRATE_BPS - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH10_INT_PRIORITY) || ( 15 < SCI_IIC_CFG_CH10_INT_PRIORITY))
            #error "ERROR - SCI_IIC_CFG_CH10_INT_PRIORITY - Parameter error in configures file.  "
        #endif
        #if ((0 > SCI_IIC_CFG_CH10_DIGITAL_FILTER) || (1 < SCI_IIC_CFG_CH10_DIGITAL_FILTER))
            #error "ERROR - SCI_IIC_CFG_CH10_DIGITAL_FILTER - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH10_FILTER_CLOCK) || (4 < SCI_IIC_CFG_CH10_FILTER_CLOCK))
            #error "ERROR - SCI_IIC_CFG_CH10_FILTER_CLOCK - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH10_SSDA_DELAY_SELECT) || (31 < SCI_IIC_CFG_CH10_SSDA_DELAY_SELECT))
            #error "ERROR - SCI_IIC_CFG_CH10_SSDA_DELAY_SELECT - Parameter error in configures file.  "
        #endif
    #elif (0 == SCI_IIC_CFG_CH10_INCLUDED)
/* do not check */
    #else
        #error "ERROR - SCI_IIC_CFG_CH10_INCLUDED - Parameter error in configures file.  "
    #endif

/* check of channel 11 */
    #if (1 == SCI_IIC_CFG_CH11_INCLUDED)
        #if ((0 >= SCI_IIC_CFG_CH11_BITRATE_BPS) || (384000 < SCI_IIC_CFG_CH11_BITRATE_BPS))
            #error "ERROR - SCI_IIC_CFG_CH11_BITRATE_BPS - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH11_INT_PRIORITY) || ( 15 < SCI_IIC_CFG_CH11_INT_PRIORITY))
            #error "ERROR - SCI_IIC_CFG_CH11_INT_PRIORITY - Parameter error in configures file.  "
        #endif
        #if ((0 > SCI_IIC_CFG_CH11_DIGITAL_FILTER) || (1 < SCI_IIC_CFG_CH11_DIGITAL_FILTER))
            #error "ERROR - SCI_IIC_CFG_CH11_DIGITAL_FILTER - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH11_FILTER_CLOCK) || (4 < SCI_IIC_CFG_CH11_FILTER_CLOCK))
            #error "ERROR - SCI_IIC_CFG_CH11_FILTER_CLOCK - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH11_SSDA_DELAY_SELECT) || (31 < SCI_IIC_CFG_CH11_SSDA_DELAY_SELECT))
            #error "ERROR - SCI_IIC_CFG_CH11_SSDA_DELAY_SELECT - Parameter error in configures file.  "
        #endif
    #elif (0 == SCI_IIC_CFG_CH11_INCLUDED)
/* do not check */
    #else
        #error "ERROR - SCI_IIC_CFG_CH11_INCLUDED - Parameter error in configures file.  "
    #endif

/* check of channel 12 */
    #if (1 == SCI_IIC_CFG_CH12_INCLUDED)
        #if ((0 >= SCI_IIC_CFG_CH12_BITRATE_BPS) || (384000 < SCI_IIC_CFG_CH12_BITRATE_BPS))
            #error "ERROR - SCI_IIC_CFG_CH12_BITRATE_BPS - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH12_INT_PRIORITY) || ( 15 < SCI_IIC_CFG_CH12_INT_PRIORITY))
            #error "ERROR - SCI_IIC_CFG_CH12_INT_PRIORITY - Parameter error in configures file.  "
        #endif
        #if ((0 > SCI_IIC_CFG_CH12_DIGITAL_FILTER) || (1 < SCI_IIC_CFG_CH12_DIGITAL_FILTER))
            #error "ERROR - SCI_IIC_CFG_CH12_DIGITAL_FILTER - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH12_FILTER_CLOCK) || (4 < SCI_IIC_CFG_CH12_FILTER_CLOCK))
            #error "ERROR - SCI_IIC_CFG_CH12_FILTER_CLOCK - Parameter error in configures file.  "
        #endif
        #if ((1 > SCI_IIC_CFG_CH12_SSDA_DELAY_SELECT) || (31 < SCI_IIC_CFG_CH12_SSDA_DELAY_SELECT))
            #error "ERROR - SCI_IIC_CFG_CH12_SSDA_DELAY_SELECT - Parameter error in configures file.  "
        #endif
    #elif (0 == SCI_IIC_CFG_CH12_INCLUDED)
/* do not check */
    #else
        #error "ERROR - SCI_IIC_CFG_CH12_INCLUDED - Parameter error in configures file.  "
    #endif

/* Channel select not changed. */
    #if ( (0 == SCI_IIC_CFG_CH0_INCLUDED) && (0 == SCI_IIC_CFG_CH1_INCLUDED) && (0 == SCI_IIC_CFG_CH2_INCLUDED) \
       && (0 == SCI_IIC_CFG_CH3_INCLUDED) && (0 == SCI_IIC_CFG_CH4_INCLUDED) && (0 == SCI_IIC_CFG_CH5_INCLUDED) \
       && (0 == SCI_IIC_CFG_CH6_INCLUDED) && (0 == SCI_IIC_CFG_CH7_INCLUDED) && (0 == SCI_IIC_CFG_CH8_INCLUDED) \
       && (0 == SCI_IIC_CFG_CH9_INCLUDED) && (0 == SCI_IIC_CFG_CH10_INCLUDED) && (0 == SCI_IIC_CFG_CH11_INCLUDED) \
       && (0 == SCI_IIC_CFG_CH12_INCLUDED) )
        #error "ERROR - SCI_IIC_CFG_CHx_INCLUDED (x is 0 to 12) is all 0.- Parameter error in r_sci_iic_rx_config.h."
    #endif

/*----------------------------------------------------------------------------*/
/*   Definitions for ports group.                                             */
/*----------------------------------------------------------------------------*/
    #define PORT0_GR    (0)
    #define PORT1_GR    (1)
    #define PORT2_GR    (2)
    #define PORT3_GR    (3)
    #define PORT4_GR    (4)
    #define PORT5_GR    (5)
    #define PORT6_GR    (6)
    #define PORT7_GR    (7)
    #define PORT8_GR    (8)
    #define PORT9_GR    (9)
    #define PORTA_GR    (0xA)
    #define PORTB_GR    (0xB)
    #define PORTC_GR    (0xC)
    #define PORTD_GR    (0xD)
    #define PORTE_GR    (0xE)
    #define PORTF_GR    (0xF)
    #define PORTG_GR    (0x10)
    #define PORTH_GR    (0x11)
    #define PORTI_GR    (0x12)
    #define PORTJ_GR    (0x13)
    #define PORT_GR_MAX (0x14)

/*----------------------------------------------------------------------------*/
/*  Bit position masks                                                        */
/*----------------------------------------------------------------------------*/
    #define BIT0_MASK   (0x01U)
    #define BIT1_MASK   (0x02U)
    #define BIT2_MASK   (0x04U)
    #define BIT3_MASK   (0x08U)
    #define BIT4_MASK   (0x10U)
    #define BIT5_MASK   (0x20U)
    #define BIT6_MASK   (0x40U)
    #define BIT7_MASK   (0x80U)
    #define BIT8_MASK   (0x0100U)
    #define BIT9_MASK   (0x0200U)
    #define BIT10_MASK  (0x0400U)
    #define BIT11_MASK  (0x0800U)
    #define BIT12_MASK  (0x1000U)
    #define BIT13_MASK  (0x2000U)
    #define BIT14_MASK  (0x4000U)
    #define BIT15_MASK  (0x8000U)
    #define BIT16_MASK  (0x010000U)
    #define BIT17_MASK  (0x020000U)
    #define BIT18_MASK  (0x040000U)
    #define BIT19_MASK  (0x080000U)
    #define BIT20_MASK  (0x100000U)
    #define BIT21_MASK  (0x200000U)
    #define BIT22_MASK  (0x400000U)
    #define BIT23_MASK  (0x800000U)
    #define BIT24_MASK  (0x01000000U)
    #define BIT25_MASK  (0x02000000U)
    #define BIT26_MASK  (0x04000000U)
    #define BIT27_MASK  (0x08000000U)
    #define BIT28_MASK  (0x10000000U)
    #define BIT29_MASK  (0x20000000U)
    #define BIT30_MASK  (0x40000000U)
    #define BIT31_MASK  (0x80000000U)

/*----------------------------------------------------------------------------*/
/*  Define counter.                                                           */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_STOP_COND_WAIT      ((uint16_t)(1000))  /* Counter of waiting stop condition generation */

/*----------------------------------------------------------------------------*/
/*  Define R/W code for slave address.                                        */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_W_CODE              ((uint8_t)(0xFE))   /* Write code for slave address */
    #define SCI_IIC_R_CODE              ((uint8_t)(0x01))   /* Read code for slave address */

/*----------------------------------------------------------------------------*/
/*  Define of port control.                                                   */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_HI                  ((uint8_t)(0x01))   /* Port "H" */
    #define SCI_IIC_LOW                 ((uint8_t)(0x00))   /* Port "L" */
    #define SCI_IIC_OUT                 ((uint8_t)(0x01))   /* Port Output */
    #define SCI_IIC_IN                  ((uint8_t)(0x00))   /* Port Input */
    #define SCI_IIC_BUSY                ((uint8_t)(0x01))   /* Port use as I/O port for peripheral functions. */
    #define SCI_IIC_NOT_BUSY            ((uint8_t)(0x00))   /* Port Use as general I/O port. */

/*----------------------------------------------------------------------------*/
/*  Define bool type.                                                         */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_FALSE               ((bool)(0x00))      /* False */
    #define SCI_IIC_TRUE                ((bool)(0x01))      /* True */

/*----------------------------------------------------------------------------*/
/*  Define digital filter enable bit.                                         */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_DF_ENABLE           ((uint8_t)(1))      /* Enable */
    #define SCI_IIC_DF_DISABLE          ((uint8_t)(0))      /* Disable */

/*----------------------------------------------------------------------------*/
/*  Condition bit.                                                            */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_STAREQ              ((uint8_t)(1))
    #define SCI_IIC_RSTAREQ             ((uint8_t)(2))
    #define SCI_IIC_STPREQ              ((uint8_t)(4))

/*----------------------------------------------------------------------------*/
/*  I2C Interrupt Mode Select (IICINTM bit).                                  */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_RCV_TRS_INTERRUPT   ((uint8_t)(1))      /* Synchronization with the clock signal */

/*----------------------------------------------------------------------------*/
/*  Clock Synchronization (IICCSC bit).                                       */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_NO_SYNCHRO          ((uint8_t)(0))      /* No synchronization with the clock signal */
    #define SCI_IIC_SYNCHRO             ((uint8_t)(1))      /* Synchronization with the clock signal */

/*----------------------------------------------------------------------------*/
/*  ACK Transmission Data bit (IICACKT bit).                                  */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_ACK_TRANS           ((uint8_t)(0))
    #define SCI_IIC_NACK_TRANS          ((uint8_t)(1))

/*----------------------------------------------------------------------------*/
/*  ACK Reception Data Flag (IICACKR bit).                                    */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_ACK_RCV             ((uint8_t)(0))

/*----------------------------------------------------------------------------*/
/*  SSDA/SSCL Output Select (IICSDAS bit/ IICSCLS bit).                       */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_CLOCK_OUTPUT        ((uint8_t)(0))
    #define SCI_IIC_CON_OUTPUT          ((uint8_t)(1))
    #define SCI_IIC_LOW_OUTPUT          ((uint8_t)(2))
    #define SCI_IIC_HI_Z_OUTPUT         ((uint8_t)(3))

/*----------------------------------------------------------------------------*/
/*  Transimitted/Received Data Transfer Direction bit (SDIR bit).             */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_LSB_FIRST           ((uint8_t)(0))
    #define SCI_IIC_MSB_FIRST           ((uint8_t)(1))

/*----------------------------------------------------------------------------*/
/*  Define register setting.                                                  */
/*----------------------------------------------------------------------------*/
/* Common registers setting */
    #define SCI_IIC_IICSTIF_CLEAR       ((uint8_t)(0))
    #define SCI_IIC_ST_CON_GENERATED    ((uint8_t)(0x51))
    #define SCI_IIC_RST_CON_GENERATED   ((uint8_t)(0x52))
    #define SCI_IIC_SP_CON_GENERATED    ((uint8_t)(0x54))

    #define SCI_IIC_ONESHOT_WAIT        ((uint8_t)(40))

/* Base register of PCR used to calculate all PCR register addresses. This is constant for all supported MCUs. */
    #define SCI_IIC_PRV_PCR_BASE_REG ((uint8_t volatile *)(&MPC.PWPR.BYTE-95))
/* Base register of PDR used to calculate all PDR register addresses. This is constant for all supported MCUs. */
    #define SCI_IIC_PRV_PDR_BASE_REG ((uint8_t volatile *)(&MPC.PWPR.BYTE-287))
/* Base register of PMR used to calculate all PMR register addresses. This is constant for all supported MCUs. */
    #define SCI_IIC_PRV_PMR_BASE_REG ((uint8_t volatile *)(&MPC.PWPR.BYTE-191))
/* Base register of ODR0 used to calculate all ODR0 register addresses. This is constant for all supported MCUs. */
    #define SCI_IIC_PRV_ODR0_BASE_REG ((uint8_t volatile *)(&MPC.PWPR.BYTE-159))
/* Base register of ODR1 used to calculate all ODR1 register addresses. This is constant for all supported MCUs. */
    #define SCI_IIC_PRV_ODR1_BASE_REG ((uint8_t volatile *)(&MPC.PWPR.BYTE-158))
/* Base register of PFS used to calculate all PFS register addresses. This is constant for all supported MCUs. */
    #define SCI_IIC_PRV_PFS_BASE_REG ((uint8_t volatile *)(&MPC.PWPR.BYTE+33))
/* Base register of PIDR used to calculate all PIDR register addresses. This is constant for all supported MCUs. */
    #define SCI_IIC_PRV_PIDR_BASE_REG ((uint8_t volatile *)(&MPC.PWPR.BYTE-223))

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
/*----------------------------------------------------------------------------*/
/*  Define internal sci_iic information structure type.                       */
/*----------------------------------------------------------------------------*/
/* Set number of SCI_IIC channel */
typedef enum
{
    SCI_IIC_NUM_CH0 = 0U,
    SCI_IIC_NUM_CH1,
    SCI_IIC_NUM_CH2,
    SCI_IIC_NUM_CH3,
    SCI_IIC_NUM_CH4,
    SCI_IIC_NUM_CH5,
    SCI_IIC_NUM_CH6,
    SCI_IIC_NUM_CH7,
    SCI_IIC_NUM_CH8,
    SCI_IIC_NUM_CH9,
    SCI_IIC_NUM_CH10,
    SCI_IIC_NUM_CH11,
    SCI_IIC_NUM_CH12,
    SCI_IIC_NUM_CH_MAX
} sci_iic_num_ch_t;

/* Internal Mode. */
typedef enum
{
    SCI_IIC_MODE_NONE = 0U, /* Non-communication mode */
    SCI_IIC_MODE_OPEN, /* Open mode */
    SCI_IIC_MODE_SEND, /* Single master transmission mode */
    SCI_IIC_MODE_RECEIVE, /* Single master reception mode */
    SCI_IIC_MODE_SEND_RECEIVE, /* Single master transmission reception mode */
    SCI_IIC_MODE_GETSTATUS, /* Get status mode */
    SCI_IIC_MODE_CONTROL, /* Control mode */
    SCI_IIC_MODE_CLOSE, /* Close mode */
} sci_iic_api_mode_t;

/* Internal Status. */
typedef enum
{
    SCI_IIC_STS_NO_INIT = 0U, /* None initialization state */
    SCI_IIC_STS_IDLE, /* Idle state */
    SCI_IIC_STS_ST_COND_WAIT, /* Start condition generation completion wait state */
    SCI_IIC_STS_SEND_SLVADR_W_WAIT, /* Slave address [Write] transmission completion wait state */
    SCI_IIC_STS_SEND_SLVADR_R_WAIT, /* Slave address [Read] transmission completion wait state */
    SCI_IIC_STS_SEND_DATA_WAIT, /* Data transmission completion wait state */
    SCI_IIC_STS_RECEIVE_DATA_WAIT, /* Data reception completion wait state */
    SCI_IIC_STS_SP_COND_WAIT, /* Stop condition generation completion wait state */
    SCI_IIC_STS_MAX /* Prohibition of setup above here */
} sci_iic_api_status_t;

/* Internal Event. */
typedef enum
{
    SCI_IIC_EV_INIT = 0U, /* Called function of Initializes IIC driver */
    SCI_IIC_EV_GEN_START_COND, /* Called function of Start condition generation */
    SCI_IIC_EV_INT_START, /* Interrupted start condition generation */
    SCI_IIC_EV_INT_ADD, /* Interrupted address sending */
    SCI_IIC_EV_INT_SEND, /* Interrupted data sending */
    SCI_IIC_EV_INT_STOP, /* Interrupted Stop condition generation */
    SCI_IIC_EV_INT_NACK, /* Interrupted No Acknowledge */
    SCI_IIC_EV_MAX /* Prohibition of setup above here */
} sci_iic_api_event_t;

/*----- Structure type -----*/
/* Internal state transition structure */
typedef struct
{
    sci_iic_api_event_t event_type; /* Event */
    sci_iic_return_t    (*proc) (sci_iic_info_t *); /* handler */
} sci_iic_mtx_t;

/***********************************************************************************************************************
 Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
/*----------------------------------------------------------------------------*/
/*  external function                                                         */
/*----------------------------------------------------------------------------*/
extern sci_iic_return_t r_sci_iic_advance (sci_iic_info_t * p_sci_iic_info);
extern sci_iic_return_t r_sci_iic_check_arguments (sci_iic_info_t * p_sci_iic_info, sci_iic_api_mode_t called_api);

/*----------------------------------------------------------------------------*/
/*  Interrupt Function                                                        */
/*----------------------------------------------------------------------------*/
extern void r_sci_iic_txi_isr_processing (uint8_t ch_no);
extern void r_sci_iic_tei_isr_processing (uint8_t ch_no);

#endif /* SCI_IIC_PRIVATE_H */

