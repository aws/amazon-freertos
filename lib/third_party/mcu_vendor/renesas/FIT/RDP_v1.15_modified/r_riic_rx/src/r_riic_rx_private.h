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
 * File Name    : r_riic_private.h
 * Description  : Functions for using RIIC on RX devices. 
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.07.2013 1.00     First Release
 *         : 30.09.2013 1.10     Change symbol of return value and status
 *         : 08.10.2013 1.20     Modified processing for the I/O register initialization and mode transition 
 *                                when a stop condition is detected while the slave is communicating.
 *                               Modified processing for mode transition when an API function is called
 *                                while the bus is busy.
 *                               Modified processing for mode transition
 *                                when an arbitration lost occurred and the addresses do not match.
 *                               Modified incorrect slave transmission after the master reception.
 *                               Modified processing for the I/O register initialization
 *                                when generating a start condition and receiving the slave address.
 *         : 17.07.2014 1.30     Added the parameters for the timeout detection.
 *                               The number of channels varies depending on the MCU.
 *                               Therefore, parameter setting for each channel is set 
 *                               in the private header file(r_riic_rx###_private.h) for each MCU folder
 *                               instead of this file.
 *                               (### is 111 or 110 or 113 or 64m)
 *         : 22.09.2014 1.40     The module is updated to measure the issue that slave communication 
 *                               is not available after an arbitration-lost occurs and the bus is locked. 
 *                                 The issue occurs when the following four conditions are all met.
 *                                    - RIIC FIT module rev. 1.30 or earlier is used.
 *                                    - RX device operates as both the master and the slave 
 *                                      in multi-master communication.
 *                                    - An arbitration-lost is detected when communicating as the master.
 *                                    - Communication other than master reception or slave reception is performed.
 *         : 14.11.2014 1.50     Added RX113 support.
 *         : 09.10.2014 1.60     Added RX71M support.
 *         : 20.10.2014 1.70     Added RX231 support.
 *         : 31.10.2015 1.80     Added RX130, RX230, RX23T support.
 *         : 04.03.2016 1.90     Added RX24T support.Changed about the pin definisions.
 *         : 01.10.2016 2.00     Added RX65N support.
 *         : 02.06.2017 2.10     Deleted RIIC port definitions of RIIC3.
 *                               Changed include path becuase changed file structure.
 *                               Added include of the RX24U header file.
 *                               Added macro definition RIIC_PRV_PCR_BASE_REG, RIIC_PRV_PDR_BASE_REG,
 *                                RIIC_PRV_PMR_BASE_REG and RIIC_PRV_PFS_BASE_REG.
 *                               Deleted interrupt functions of RIIC3.
 *                               Deleted macro definition RIIC_TMOE.
 *         : 31.08.2017 2.20     Fixed the macro definition conversion processing of pin.
 **********************************************************************************************************************/
/* Guards against multiple inclusion */
#ifndef RIIC_PRIVATE_H
    #define RIIC_PRIVATE_H
/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
/* Fixed width integer support. */
    #include "r_riic_rx_if.h"
    #include "r_riic_rx_pin_config.h"
    #include "r_riic_rx_config.h"

/* Include specifics for chosen MCU.  */
    #if defined(BSP_MCU_RX111)
        #include "./targets/rx111/r_riic_rx111_private.h"
    #elif defined(BSP_MCU_RX110)
        #include "./targets/rx110/r_riic_rx110_private.h"
    #elif defined(BSP_MCU_RX113)
        #include "./targets/rx113/r_riic_rx113_private.h"
    #elif defined(BSP_MCU_RX64M)
        #include "./targets/rx64m/r_riic_rx64m_private.h"
    #elif defined(BSP_MCU_RX65N)
        #include "./targets/rx65n/r_riic_rx65n_private.h"
    #elif defined(BSP_MCU_RX71M)
        #include "./targets/rx71m/r_riic_rx71m_private.h"
    #elif defined(BSP_MCU_RX231)
        #include "./targets/rx231/r_riic_rx231_private.h"
    #elif defined(BSP_MCU_RX130)
        #include "./targets/rx130/r_riic_rx130_private.h"
    #elif defined(BSP_MCU_RX230)
        #include "./targets/rx230/r_riic_rx230_private.h"
    #elif defined(BSP_MCU_RX23T)
        #include "./targets/rx23t/r_riic_rx23t_private.h"
    #elif defined(BSP_MCU_RX24T)
        #include "./targets/rx24t/r_riic_rx24t_private.h"
    #elif defined(BSP_MCU_RX24U)
        #include "./targets/rx24u/r_riic_rx24u_private.h"
    #else
        #error "This MCU is not supported by the current r_riic_rx module."
    #endif

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/*----------------------------------------------------------------------------*/
/*   Define for calculation processing.                                       */
/*----------------------------------------------------------------------------*/
/* Bit position masks */
    #define BIT0                      ((uint8_t)(0x01))
    #define BIT1                      ((uint8_t)(0x02))
    #define BIT2                      ((uint8_t)(0x04))
    #define BIT3                      ((uint8_t)(0x08))
    #define BIT4                      ((uint8_t)(0x10))
    #define BIT5                      ((uint8_t)(0x20))
    #define BIT6                      ((uint8_t)(0x40))
    #define BIT7                      ((uint8_t)(0x80))

    #define RIIC_MAX_DIV              ((uint8_t)(0x08))
    #define RIIC_STAD_SPPED_MAX       ((uint8_t)(100))
    #define RIIC_FAST_SPPED_MAX       ((uint16_t)(400))
    #define RIIC_ICBR_MAX             ((uint8_t)(32))

/*----------------------------------------------------------------------------*/
/*   Define send data of blank.                                               */
/*----------------------------------------------------------------------------*/
    #define BLANK                     ((uint8_t)(0xFF))

/*----------------------------------------------------------------------------*/
/*   Define R/W code for slave address.                                       */
/*----------------------------------------------------------------------------*/
    #define W_CODE                    ((uint8_t)(0xFE))       /* Write code for slave address */
    #define R_CODE                    ((uint8_t)(0x01))       /* Read code for slave address */

/*----------------------------------------------------------------------------*/
/*   Define of port control.                                                  */
/*----------------------------------------------------------------------------*/
    #define RIIC_OUT                  ((uint8_t)(0x01))       /* Port Output */
    #define RIIC_IN                   ((uint8_t)(0x00))       /* Port Input */

/*----------------------------------------------------------------------------*/
/*   Define bool type.                                                        */
/*----------------------------------------------------------------------------*/
    #define RIIC_FALSE                ((bool)(0))             /* False */
    #define RIIC_TRUE                 ((bool)(1))             /* True */

/*----------------------------------------------------------------------------*/
/* Define software SCL Low check counter                                      */
/*----------------------------------------------------------------------------*/
    #define RIIC_CFG_SCL_CHECK_COUNTER      (1000U)

/* RIIC port (ascii to number) */
/* RIIC0 */
    #if ((('0') <= R_RIIC_CFG_RIIC0_SCL0_PORT) && (('9') >= R_RIIC_CFG_RIIC0_SCL0_PORT))
        #define RIIC_CH0_SCL0_GP (R_RIIC_CFG_RIIC0_SCL0_PORT - 0x30)
    #elif ((('A') <= R_RIIC_CFG_RIIC0_SCL0_PORT) && (('H') >= R_RIIC_CFG_RIIC0_SCL0_PORT))
        #define RIIC_CH0_SCL0_GP (R_RIIC_CFG_RIIC0_SCL0_PORT - 0x37)
    #elif (('J') == R_RIIC_CFG_RIIC0_SCL0_PORT)
/* PORTI does not exist in the PORT group. PORTJ register is placed at the position of PORTI register. */
        #define RIIC_CH0_SCL0_GP (R_RIIC_CFG_RIIC0_SCL0_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_RIIC_CFG_RIIC0_SCL0_PORT in r_riic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_RIIC_CFG_RIIC0_SCL0_BIT) && (('7') >= R_RIIC_CFG_RIIC0_SCL0_BIT))
        #define RIIC_CH0_SCL0_PIN    (R_RIIC_CFG_RIIC0_SCL0_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_RIIC_CFG_RIIC0_SCL0_BIT in r_riic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_RIIC_CFG_RIIC0_SDA0_PORT) && (('9') >= R_RIIC_CFG_RIIC0_SDA0_PORT))
        #define RIIC_CH0_SDA0_GP (R_RIIC_CFG_RIIC0_SDA0_PORT - 0x30)
    #elif ((('A') <= R_RIIC_CFG_RIIC0_SDA0_PORT) && (('H') >= R_RIIC_CFG_RIIC0_SDA0_PORT))
        #define RIIC_CH0_SDA0_GP (R_RIIC_CFG_RIIC0_SDA0_PORT - 0x37)
    #elif (('J') == R_RIIC_CFG_RIIC0_SDA0_PORT)
/* PORTI does not exist in the PORT group. PORTJ register is placed at the position of PORTI register. */
        #define RIIC_CH0_SDA0_GP (R_RIIC_CFG_RIIC0_SDA0_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_RIIC_CFG_RIIC0_SDA0_PORT in r_riic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_RIIC_CFG_RIIC0_SDA0_BIT) && (('7') >= R_RIIC_CFG_RIIC0_SDA0_BIT))
        #define RIIC_CH0_SDA0_PIN    (R_RIIC_CFG_RIIC0_SDA0_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_RIIC_CFG_RIIC0_SDA0_BIT in r_riic_rx_pin_config.h"
    #endif

/* RIIC1 */
    #if ((('0') <= R_RIIC_CFG_RIIC1_SCL1_PORT) && (('9') >= R_RIIC_CFG_RIIC1_SCL1_PORT))
        #define RIIC_CH1_SCL1_GP (R_RIIC_CFG_RIIC1_SCL1_PORT - 0x30)
    #elif ((('A') <= R_RIIC_CFG_RIIC1_SCL1_PORT) && (('H') >= R_RIIC_CFG_RIIC1_SCL1_PORT))
        #define RIIC_CH1_SCL1_GP (R_RIIC_CFG_RIIC1_SCL1_PORT - 0x37)
    #elif (('J') == R_RIIC_CFG_RIIC1_SCL1_PORT)
/* PORTI does not exist in the PORT group. PORTJ register is placed at the position of PORTI register. */
        #define RIIC_CH1_SCL1_GP (R_RIIC_CFG_RIIC1_SCL1_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_RIIC_CFG_RIIC1_SCL1_PORT in r_riic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_RIIC_CFG_RIIC1_SCL1_BIT) && (('7') >= R_RIIC_CFG_RIIC1_SCL1_BIT))
        #define RIIC_CH1_SCL1_PIN    (R_RIIC_CFG_RIIC1_SCL1_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_RIIC_CFG_RIIC1_SCL1_BIT in r_riic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_RIIC_CFG_RIIC1_SDA1_PORT) && (('9') >= R_RIIC_CFG_RIIC1_SDA1_PORT))
        #define RIIC_CH1_SDA1_GP (R_RIIC_CFG_RIIC1_SDA1_PORT - 0x30)
    #elif ((('A') <= R_RIIC_CFG_RIIC1_SDA1_PORT) && (('H') >= R_RIIC_CFG_RIIC1_SDA1_PORT))
        #define RIIC_CH1_SDA1_GP (R_RIIC_CFG_RIIC1_SDA1_PORT - 0x37)
    #elif (('J') == R_RIIC_CFG_RIIC1_SDA1_PORT)
/* PORTI does not exist in the PORT group. PORTJ register is placed at the position of PORTI register. */
        #define RIIC_CH1_SDA1_GP (R_RIIC_CFG_RIIC1_SDA1_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_RIIC_CFG_RIIC1_SDA1_PORT in r_riic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_RIIC_CFG_RIIC1_SDA1_BIT) && (('7') >= R_RIIC_CFG_RIIC1_SDA1_BIT))
        #define RIIC_CH1_SDA1_PIN    (R_RIIC_CFG_RIIC1_SDA1_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_RIIC_CFG_RIIC1_SDA1_BIT in r_riic_rx_pin_config.h"
    #endif

/* RIIC2 */
    #if ((('0') <= R_RIIC_CFG_RIIC2_SCL2_PORT) && (('9') >= R_RIIC_CFG_RIIC2_SCL2_PORT))
        #define RIIC_CH2_SCL2_GP (R_RIIC_CFG_RIIC2_SCL2_PORT - 0x30)
    #elif ((('A') <= R_RIIC_CFG_RIIC2_SCL2_PORT) && (('H') >= R_RIIC_CFG_RIIC2_SCL2_PORT))
        #define RIIC_CH2_SCL2_GP (R_RIIC_CFG_RIIC2_SCL2_PORT - 0x37)
    #elif (('J') == R_RIIC_CFG_RIIC2_SCL2_PORT)
/* PORTI does not exist in the PORT group. PORTJ register is placed at the position of PORTI register. */
        #define RIIC_CH2_SCL2_GP (R_RIIC_CFG_RIIC2_SCL2_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_RIIC_CFG_RIIC2_SCL2_PORT in r_riic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_RIIC_CFG_RIIC2_SCL2_BIT) && (('7') >= R_RIIC_CFG_RIIC2_SCL2_BIT))
        #define RIIC_CH2_SCL2_PIN    (R_RIIC_CFG_RIIC2_SCL2_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_RIIC_CFG_RIIC2_SCL2_BIT in r_riic_rx_pin_config.h"
    #endif

    #if ((('0') <= R_RIIC_CFG_RIIC2_SDA2_PORT) && (('9') >= R_RIIC_CFG_RIIC2_SDA2_PORT))
        #define RIIC_CH2_SDA2_GP (R_RIIC_CFG_RIIC2_SDA2_PORT - 0x30)
    #elif ((('A') <= R_RIIC_CFG_RIIC2_SDA2_PORT) && (('H') >= R_RIIC_CFG_RIIC2_SDA2_PORT))
        #define RIIC_CH2_SDA2_GP (R_RIIC_CFG_RIIC2_SDA2_PORT - 0x37)
    #elif (('J') == R_RIIC_CFG_RIIC2_SDA2_PORT)
/* PORTI does not exist in the PORT group. PORTJ register is placed at the position of PORTI register. */
        #define RIIC_CH2_SDA2_GP (R_RIIC_CFG_RIIC2_SDA2_PORT - 0x38)
    #else
        #error "Error! Invalid setting for R_RIIC_CFG_RIIC2_SDA2_PORT in r_riic_rx_pin_config.h"
    #endif
    #if ((('0') <= R_RIIC_CFG_RIIC2_SDA2_BIT) && (('7') >= R_RIIC_CFG_RIIC2_SDA2_BIT))
        #define RIIC_CH2_SDA2_PIN    (R_RIIC_CFG_RIIC2_SDA2_BIT - 0x30)
    #else
        #error "Error! Invalid setting for R_RIIC_CFG_RIIC2_SDA2_BIT in r_riic_rx_pin_config.h"
    #endif

/*============================================================================*/
/*  Configuration Options of non support                                      */
/*============================================================================*/
/* Setting to */
/* 0 = disable, 1 = enable */
    #define RIIC_CFG_NACK_AL_ENABLE         (0)
    #define RIIC_CFG_SLAVE_AL_ENABLE        (0)
    #define RIIC_CFG_NACK_SUSPEND_ENABLE    (1)
    #define RIIC_CFG_SCL_SYNC_ENABLE        (1)

/*============================================================================*/
/*  Parameter check of Configuration Options                                  */
/*============================================================================*/
    #if ((0 != RIIC_CFG_PARAM_CHECKING_ENABLE) && (1 != RIIC_CFG_PARAM_CHECKING_ENABLE))
        #error "ERROR - RIIC_CFG_PARAM_CHECKING_ENABLE - Parameter error in configures file.  "
    #endif

    #if ((0 != RIIC_CFG_PORT_SET_PROCESSING) && (1 != RIIC_CFG_PORT_SET_PROCESSING))
        #error "ERROR - RIIC_CFG_PORT_SET_PROCESSING - Parameter error in configures file.  "
    #endif

    #if ((0 != RIIC_CFG_NACK_AL_ENABLE) && (1 != RIIC_CFG_NACK_AL_ENABLE))
        #error "ERROR - RIIC_CFG_NACK_AL_ENABLE - Parameter error in configures file.  "
    #endif

    #if ((0 != RIIC_CFG_SLAVE_AL_ENABLE) && (1 != RIIC_CFG_SLAVE_AL_ENABLE))
        #error "ERROR - RIIC_CFG_SLAVE_AL_ENABLE - Parameter error in configures file.  "
    #endif

    #if ((0 != RIIC_CFG_NACK_SUSPEND_ENABLE) && (1 != RIIC_CFG_NACK_SUSPEND_ENABLE))
        #error "ERROR - RIIC_CFG_NACK_SUSPEND_ENABLE - Parameter error in configures file.  "
    #endif

    #if ((0 != RIIC_CFG_SCL_SYNC_ENABLE) && (1 != RIIC_CFG_SCL_SYNC_ENABLE))
        #error "ERROR - RIIC_CFG_SCL_SYNC_ENABLE - Parameter error in configures file.  "
    #endif

    #if ((0x00000000 > RIIC_CFG_BUS_CHECK_COUNTER) || (0xFFFFFFFF < RIIC_CFG_BUS_CHECK_COUNTER))
        #error "ERROR - RIIC_CFG_BUS_CHECK_COUNTER - Parameter error in configures file.  "
    #endif

/*============================================================================*/
/*   Define for register setting.                                             */
/*============================================================================*/
/* Interrupt register setting */
    #define RIIC_IR_CLR             ((uint8_t)(0x00))       /* Clears interrupt request register. */
    #define RIIC_IR_SET             ((uint8_t)(0x01))       /* Sets interrupt request register. */
    #define RIIC_IER_DISABLE        (0U)                    /* Disables interrupt request enable register. */
    #define RIIC_IER_ENABLE         (1U)                    /* Enables interrupt request enable register. */

/* RIIC register setting */
    #define RIIC_ICCR1_NOT_DRIVEN   ((uint8_t)(0x00))       /* Clears ICCR1.ICE bit. */
    #define RIIC_ICCR1_ICE          ((uint8_t)(0x80))       /* ICCR1.ICE bit. */
    #define RIIC_ICCR1_ICE_SET      ((uint8_t)(0x80))       /* Sets ICCR1.ICE bit. */
    #define RIIC_ICCR1_ICE_CLR      ((uint8_t)(0x7F))       /* Clears ICCR1.ICE bit. */
    #define RIIC_ICCR1_RIIC_RESET   ((uint8_t)(0x40))       /* Sets ICCR1.IICRST bit. */
    #define RIIC_ICCR1_ENABLE       ((uint8_t)(0xBF))       /* Clears ICCR1.IICRST bit. */
    #define RIIC_ICCR1_CLO_SET      ((uint8_t)(0x20))       /* Sets ICCR1.CLO bit. */
    #define RIIC_ICCR1_SOWP         ((uint8_t)(0x10))       /* ICCR1.SOWP bit. */
    #define RIIC_ICCR1_SOWP_SET     ((uint8_t)(0x10))       /* Sets ICCR1.SOWP bit. */
    #define RIIC_ICCR1_SOWP_CLR     ((uint8_t)(0xEF))       /* Clears ICCR1.SOWP bit. */
    #define RIIC_ICCR1_SDAO_SCLO_SET ((uint8_t)(0x1F))      /* Clears ICCR1.SOWP bit. Sets ICCR1.SDAO and SCL0 bit*/
    #define RIIC_ICCR1_SDAI         ((uint8_t)(0x01))       /* Mask for ICCR1.SDAI bit. */
    #define RIIC_ICCR1_SDAI_SET     ((uint8_t)(0x01))       /* Sets for ICCR1.SDAI bit. */
    #define RIIC_ICCR1_SCLI         ((uint8_t)(0x02))       /* Mask for ICCR1.SCLI bit. */
    #define RIIC_ICCR1_SCLI_SET     ((uint8_t)(0x02))       /* Sets for ICCR1.SCLI bit. */
    #define RIIC_ICCR1_SDAO         ((uint8_t)(0x04))       /* Mask for ICCR1.SDAO bit. */
    #define RIIC_ICCR1_SDAO_SET     ((uint8_t)(0x04))       /* Sets for ICCR1.SDAO bit. */
    #define RIIC_ICCR1_SCLO         ((uint8_t)(0x08))       /* Mask for ICCR1.SCLO bit. */
    #define RIIC_ICCR1_SCLO_SET     ((uint8_t)(0x08))       /* Sets for ICCR1.SCLO bit. */
    #define RIIC_SARL0_INIT         ((uint8_t)(0x00))       /* Sets SARLy and SARUy.(y=0,1,2) */
    #define RIIC_SARU0_INIT         ((uint8_t)(0x00))       /* Sets SARLy and SARUy.(y=0,1,2) */
    #define RIIC_SARL1_INIT         ((uint8_t)(0x00))       /* Sets SARLy and SARUy.(y=0,1,2) */
    #define RIIC_SARU1_INIT         ((uint8_t)(0x00))       /* Sets SARLy and SARUy.(y=0,1,2) */
    #define RIIC_SARL2_INIT         ((uint8_t)(0x00))       /* Sets SARLy and SARUy.(y=0,1,2) */
    #define RIIC_SARU2_INIT         ((uint8_t)(0x00))       /* Sets SARLy and SARUy.(y=0,1,2) */
    #define RIIC_SARL_MASK          ((uint16_t)(0x00FF))    /* Mask for SARLy setting. */
    #define RIIC_SARU_MASK          ((uint16_t)(0x0300))    /* Mask for SARUy setting. */
    #define RIIC_SARU_10BIT_FORMAT_SET ((uint8_t)(0x01))    /* Sets SARUy.FS bit. */
    #define RIIC_ICSER_INIT         ((uint8_t)(0x00))       /* Sets ICSER */
    #define RIIC_ICSER_SAR_DASABLE  ((uint8_t)(0xF8))       /* Clears ICSER.SAR0E,SAR1E,SAR2E bit */
    #define RIIC_ICSER_SAR0E_SET    ((uint8_t)(0x01))       /* Sets ICSER.SAR0E bit */
    #define RIIC_ICSER_SAR0E_CLR    ((uint8_t)(0xFE))       /* Clears ICSER.SAR0E bit */
    #define RIIC_ICSER_SAR1E_SET    ((uint8_t)(0x02))       /* Sets ICSER.SAR1E bit */
    #define RIIC_ICSER_SAR1E_CLR    ((uint8_t)(0xFD))       /* Clears ICSER.SAR1E bit */
    #define RIIC_ICSER_SAR2E_SET    ((uint8_t)(0x04))       /* Sets ICSER.SAR2E bit */
    #define RIIC_ICSER_SAR2E_CLR    ((uint8_t)(0xFB))       /* Clears ICSER.SAR2E bit */
    #define RIIC_ICSER_GCAE_SET     ((uint8_t)(0x08))       /* Sets ICSER.GCAE bit */
    #define RIIC_ICSER_GCAE_CLR     ((uint8_t)(0xF7))       /* Clears ICSER.GCAE bit */
    #define RIIC_ICSER_DIDE_SET     ((uint8_t)(0x20))       /* Sets ICSER.DIDE bit */
    #define RIIC_ICSER_DIDE_CLR     ((uint8_t)(0xDF))       /* Clears ICSER.DIDE bit */

    #define RIIC_ICMR1_MTWP_SET     ((uint8_t)(0x80))       /* Sets ICMR1.MTWP bit. */
    #define RIIC_ICMR1_MTWP_CLR     ((uint8_t)(0x7F))       /* Clears ICMR1.MTWP bit. */
    #define RIIC_ICMR2_INIT         ((uint8_t)(0x00))       /* Sets ICMR2 */

    #if RIIC_CFG_NACK_AL_ENABLE == 0
        #define RIIC_NALE               ((uint8_t)(0x00))       /* Mask for ICFER.NALE bit. */
    #elif RIIC_CFG_NACK_AL_ENABLE == 1
        #define RIIC_NALE               ((uint8_t)(0x04))       /* Mask for ICFER.NALE bit. */
    #endif

    #if RIIC_CFG_SLAVE_AL_ENABLE == 0
        #define RIIC_SALE               ((uint8_t)(0x00))       /* Mask for ICFER.SALE bit. */
    #elif RIIC_CFG_SLAVE_AL_ENABLE == 1
        #define RIIC_SALE               ((uint8_t)(0x08))       /* Mask for ICFER.SALE bit. */
    #endif

    #if RIIC_CFG_NACK_SUSPEND_ENABLE == 0
        #define RIIC_NACKE              ((uint8_t)(0x00))       /* Mask for ICFER.NACKE bit. */
    #elif RIIC_CFG_NACK_SUSPEND_ENABLE == 1
        #define RIIC_NACKE              ((uint8_t)(0x10))       /* Mask for ICFER.NACKE bit. */
    #endif

    #if RIIC_CFG_SCL_SYNC_ENABLE == 0
        #define RIIC_SCLE               ((uint8_t)(0x00))       /* Mask for ICFER.SCLE bit. */
    #elif RIIC_CFG_SCL_SYNC_ENABLE == 1
        #define RIIC_SCLE               ((uint8_t)(0x40))       /* Mask for ICFER.SCLE bit. */
    #endif

    #define RIIC_ICCR2_ST           ((uint8_t)(0x02))       /* Mask for ICCR2.ST bit. */
    #define RIIC_ICCR2_RS           ((uint8_t)(0x04))       /* Mask for ICCR2.RS bit. */
    #define RIIC_ICCR2_SP           ((uint8_t)(0x08))       /* Mask for ICCR2.SP bit. */
    #define RIIC_ICCR2_TRS          ((uint8_t)(0x20))       /* Mask for ICCR2.TRS bit. */
    #define RIIC_ICCR2_TRS_SET      ((uint8_t)(0x20))       /* Sets ICCR2.TRS bit. */
    #define RIIC_ICCR2_TRS_CLR      ((uint8_t)(0xDF))       /* Clears ICCR2.TRS bit. */
    #define RIIC_ICCR2_MST          ((uint8_t)(0x40))       /* Mask for ICCR2.MST bit. */
    #define RIIC_ICCR2_MST_SET      ((uint8_t)(0x40))       /* Sets ICCR2.MST bit. */
    #define RIIC_ICCR2_BBSY         ((uint8_t)(0x80))       /* Mask for ICCR2.BBSY bit. */
    #define RIIC_ICCR2_BBSY_SET     ((uint8_t)(0x80))       /* Sets ICCR2.BBSY bit. */
    #define RIIC_ICMR2_TMWE         ((uint8_t)(0x08))       /* Mask for ICMR2.TMWE bit. */
    #define RIIC_ICMR2_TMWE_SET     ((uint8_t)(0x08))       /* Sets ICMR2.TMWE bit. */
    #define RIIC_ICMR2_TMWE_CLR     ((uint8_t)(0xF7))       /* Clear ICMR2.TMWE bit. */
    #define RIIC_ICMR3_WAIT_SET     ((uint8_t)(0x40))       /* Sets ICMR3.WAIT bit. */
    #define RIIC_ICMR3_WAIT_CLR     ((uint8_t)(0xBF))       /* Clears ICMR3.WAIT bit. */
    #define RIIC_ICMR3_RDRFS_SET    ((uint8_t)(0x20))       /* Sets ICMR3.RDRFS bit. */
    #define RIIC_ICMR3_RDRFS_CLR    ((uint8_t)(0xDF))       /* Clears ICMR3.RDRFS bit. */
    #define RIIC_ICMR3_ACKWP_SET    ((uint8_t)(0x10))       /* Sets ICMR3.ACKWP bit. */
    #define RIIC_ICMR3_ACKWP_CLR    ((uint8_t)(0xEF))       /* Clears ICMR3.ACKWP bit. */
    #define RIIC_ICMR3_ACKBT_SET    ((uint8_t)(0x08))       /* Sets ICMR3.ACKBT bit. */
    #define RIIC_ICMR3_ACKBT_CLR    ((uint8_t)(0xF7))       /* Clears ICMR3.ACKBT bit. */
    #define RIIC_ICSR1_AAS0         ((uint8_t)(0x01))       /* Mask for ICSR1.AAS0 bit. */
    #define RIIC_ICSR1_AAS0_SET     ((uint8_t)(0x01))       /* Sets ICSR1.AAS0 bit. */
    #define RIIC_ICSR1_AAS1         ((uint8_t)(0x02))       /* Mask for ICSR1.AAS1 bit. */
    #define RIIC_ICSR1_AAS1_SET     ((uint8_t)(0x02))       /* Sets ICSR1.AAS1 bit. */
    #define RIIC_ICSR1_AAS2         ((uint8_t)(0x04))       /* Mask for ICSR1.AAS2 bit. */
    #define RIIC_ICSR1_AAS2_SET     ((uint8_t)(0x04))       /* Sets ICSR1.AAS2 bit. */
    #define RIIC_ICSR1_GCA          ((uint8_t)(0x08))       /* Mask for ICSR1.GCA bit. */
    #define RIIC_ICSR1_GCA_SET      ((uint8_t)(0x08))       /* Sets ICSR1.GCA bit. */
    #define RIIC_ICSR1_DID          ((uint8_t)(0x20))       /* Mask for ICSR1.DID bit. */
    #define RIIC_ICSR1_DID_SET      ((uint8_t)(0x20))       /* Sets ICSR1.DID bit. */
    #define RIIC_ICSR1_HOA          ((uint8_t)(0x80))       /* Mask for ICSR1.HOA bit. */
    #define RIIC_ICSR1_HOA_SET      ((uint8_t)(0x80))       /* Sets ICSR1.HOA bit. */
    #define RIIC_ICSR2_TMOF         ((uint8_t)(0x01))       /* Mask for ICSR2.TMOF bit. */
    #define RIIC_ICSR2_TMOF_SET     ((uint8_t)(0x01))       /* Sets ICSR2.TMOF bit. */
    #define RIIC_ICSR2_AL           ((uint8_t)(0x02))       /* Mask for ICSR2.AL bit. */
    #define RIIC_ICSR2_AL_SET       ((uint8_t)(0x02))       /* Sets ICSR2.AL bit. */
    #define RIIC_ICSR2_AL_CLR       ((uint8_t)(0xFD))       /* Clears ICSR2.AL bit. */
    #define RIIC_ICSR2_START        ((uint8_t)(0x04))       /* Mask for ICSR2.START bit. */
    #define RIIC_ICSR2_START_SET    ((uint8_t)(0x04))       /* Sets ICSR2.START bit. */
    #define RIIC_ICSR2_START_CLR    ((uint8_t)(0xFB))       /* Clears ICSR2.START bit. */
    #define RIIC_ICSR2_STOP         ((uint8_t)(0x08))       /* Mask for ICSR2.STOP bit. */
    #define RIIC_ICSR2_STOP_SET     ((uint8_t)(0x08))       /* Sets ICSR2.STOP bit. */
    #define RIIC_ICSR2_STOP_CLR     ((uint8_t)(0xF7))       /* Clears ICSR2.STOP bit. */
    #define RIIC_ICSR2_NACKF        ((uint8_t)(0x10))       /* Mask for ICSR2.NACKF bit. */
    #define RIIC_ICSR2_NACKF_SET    ((uint8_t)(0x10))       /* Sets ICSR2.NACKF bit. */
    #define RIIC_ICSR2_NACKF_CLR    ((uint8_t)(0xEF))       /* Clears ICSR2.NACKF bit. */
    #define RIIC_ICSR2_RDRF         ((uint8_t)(0x20))       /* Mask for ICSR2.RDRF bit. */
    #define RIIC_ICSR2_RDRF_SET     ((uint8_t)(0x20))       /* Sets ICSR2.RDRF bit. */
    #define RIIC_ICSR2_TDRE         ((uint8_t)(0x80))       /* Mask for ICSR2.TDRE bit. */
    #define RIIC_ICSR2_TDRE_SET     ((uint8_t)(0x80))       /* Sets ICSR2.TDRE bit. */
    #define RIIC_ICSR2_DISABLE      ((uint8_t)(0xE7))       /* Clears ICSR2.NACKF bit and STOP bit. */

    #define RIIC_ICIER_INIT         ((uint8_t)(0x00))       /* Initializes ICIER. */
    #define RIIC_ICIER_ST_NAK_AL    ((uint8_t)(0x16))       /* Sets ICIER.STIE bit. */
/* Enable bit  : ALIE, STIE, NAKIE          */
/* Disable bit : TMOIE, SPIE, RIE, TEIE, TIE */
    #define RIIC_ICIER_SP_NAK_AL    ((uint8_t)(0x1A))       /* Sets ICIER.SPIE bit and NAKIE. */
/* Enable bit  : ALIE, SPIE, NAKIE          */
/* Disable bit : TMOIE, STIE, RIE, TEIE, TIE */
    #define RIIC_ICIER_TEND_NAK_AL  ((uint8_t)(0x52))       /* Sets ICIER.TEIE bit. */
/* Enable bit  : ALIE, NAKIE, TEIE          */
/* Disable bit : TMOIE, STIE, SPIE, RIE, TIE */
    #define RIIC_ICIER_RX_NAK_AL    ((uint8_t)(0x32))       /* Sets ICIER.RIE bit. */
/* Enable bit  : ALIE, NAKIE, RIE           */
/* Disable bit : TMOIE, STIE, SPIE, TEIE, TIE */
    #define RIIC_ICIER_SP_AL        ((uint8_t)(0x0A))       /* Sets ICIER.SPIE bit and clears ICIER.NAKIE bit. */
/* Enable bit  : ALIE, SPIE                 */
/* Disable bit : TMOIE, STIE, NAKIE, RIE, TEIE, TIE */
    #define RIIC_ICIER_AL           ((uint8_t)(0x02))       /* Sets ICIER.ALIE bit. */
/* Enable bit  : ALIE                       */
/* Disable bit : TMOIE, STIE, SPIE, NAKIE, RIE, TEIE, TIE */
    #define RIIC_ICIER_SP           ((uint8_t)(0x08))       /* Sets ICIER.SPIE bit. */
/* Enable bit  : SPIE                       */
/* Disable bit : TMOIE, ALIE, STIE, NAKIE, RIE, TEIE, TIE */
    #define RIIC_ICIER_TX_RX_SP_NAK_AL ((uint8_t)(0xBA))    /* Sets ICIER register. */
/* Enable bit  : ALIE, SPIE, NAKIE, RIE, TIE */
/* Disable bit : TMOIE, STIE, TEIE          */
    #define RIIC_ICIER_TX_SP_NAK_AL ((uint8_t)(0x9A))       /* Sets ICIER register. */
/* Enable bit  : ALIE, SPIE, NAKIE, TIE     */
/* Disable bit : TMOIE, STIE, TEIE, RIE     */
    #define RIIC_ICIER_RX_SP_NAK_AL ((uint8_t)(0x3A))       /* Sets ICIER register. */
/* Enable bit  : ALIE, SPIE, NAKIE, RIE     */
/* Disable bit : TMOIE, STIE, TEIE, TIE     */
    #define RIIC_ICIER_TX_RX_SP_NAK ((uint8_t)(0xB8))       /* Sets ICIER register. */
/* Enable bit  : SPIE, NAKIE, RIE, TIE      */
/* Disable bit : ALIE, TMOIE, STIE, TEIE    */
    #define RIIC_ICIER_TX_SP_NAK    ((uint8_t)(0x98))       /* Sets ICIER register. */
/* Enable bit  : SPIE, NAKIE, TIE           */
/* Disable bit : ALIE, TMOIE, STIE, TEIE, RIE */
    #define RIIC_ICIER_RX_SP        ((uint8_t)(0x28))       /* Sets ICIER register. */
/* Enable bit  : SPIE, RIE           */
/* Disable bit : ALIE, TMOIE, NAKIE, STIE, TEIE, TIE */
    #define RIIC_ICIER_TX_RX        ((uint8_t)(0xA0))       /* Sets ICIER register. */
/* Enable bit  : RIE, TIE                   */
/* Disable bit : SPIE, NAKIE, ALIE, TMOIE, STIE, TEIE */
    #define RIIC_ICIER_TX           ((uint8_t)(0x80))       /* Sets ICIER register. */
/* Enable bit  : TIE                        */
/* Disable bit : SPIE, NAKIE, ALIE, TMOIE, STIE, RIE, TEIE */
    #define RIIC_ICIER_RX           ((uint8_t)(0x20))       /* Sets ICIER register. */
/* Enable bit  : RIE                        */
/* Disable bit : SPIE, NAKIE, ALIE, TMOIE, STIE, TEIE, TIE */
    #define RIIC_ICIER_TMO          ((uint8_t)(0x01))       /* Sets ICIER register. */
/* Enable bit  : TMOIE                        */
    #define RIIC_MSK_BBSY           ((uint8_t)(0x80))       /* Mask ICCR2.BBSY bit  */
    #define RIIC_MSK_SCLSDA         ((uint8_t)(0x03))       /* Mask ICCR1.SDAI bit and SCLI bit */
    #define RIIC_SCLI_LOW           ((uint8_t)(0x00))       /* input SCL Low */

/* Common registers setting */
    #define RIIC_PWPR_BOWI          (MPC.PWPR.BIT.B0WI)     /* PWPR PFSWE Bit Write Disable */

/* Control registers address defines */
    #define RIIC_ICCR1_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x00 + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_ICCR2_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x01 + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_ICMR1_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x02 + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_ICMR2_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x03 + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_ICMR3_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x04 + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_ICFER_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x05 + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_ICSER_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x06 + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_ICIER_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x07 + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_ICSR1_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x08 + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_ICSR2_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x09 + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_SARL0_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x0A + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_SARU0_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x0B + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_SARL1_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x0C + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_SARU1_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x0D + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_SARL2_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x0E + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_SARU2_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x0F + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_ICBRL_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x10 + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_ICBRH_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x11 + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_ICDRT_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x12 + ((32 * (n)) / sizeof(uint8_t)) )
    #define RIIC_ICDRR_ADR(n)    ( (volatile uint8_t *)&RIIC0 + 0x13 + ((32 * (n)) / sizeof(uint8_t)) )

    #ifdef TN_RXA012A
        #define RIIC_TMOCNTL_ADR(n)  ( (volatile uint8_t *)&RIIC0 + 0x0A + ((32 * (n)) / sizeof(uint8_t)) )
        #define RIIC_TMOCNTH_ADR(n)  ( (volatile uint8_t *)&RIIC0 + 0x0B + ((32 * (n)) / sizeof(uint8_t)) )
    #endif

/* Base register of PCR used to calculate all PCR register addresses. This is constant for all supported MCUs. */
    #define RIIC_PRV_PCR_BASE_REG ((uint8_t volatile *)(&MPC.PWPR.BYTE-95))
/* Base register of PDR used to calculate all PDR register addresses. This is constant for all supported MCUs. */
    #define RIIC_PRV_PDR_BASE_REG ((uint8_t volatile *)(&MPC.PWPR.BYTE-287))
/* Base register of PMR used to calculate all PMR register addresses. This is constant for all supported MCUs. */
    #define RIIC_PRV_PMR_BASE_REG ((uint8_t volatile *)(&MPC.PWPR.BYTE-191))
/* Base register of PFS used to calculate all PFS register addresses. This is constant for all supported MCUs. */
    #define RIIC_PRV_PFS_BASE_REG ((uint8_t volatile *)(&MPC.PWPR.BYTE+33))

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
/*----------------------------------------------------------------------------*/
/*   Define internal iic information structure type.                          */
/*----------------------------------------------------------------------------*/
/* ---- Internal Mode. ---- */
typedef enum
{
    RIIC_MODE_NONE = 0U, /* Non-communication mode */
    RIIC_MODE_M_SEND, /* Master transmission mode */
    RIIC_MODE_M_RECEIVE, /* Master reception mode */
    RIIC_MODE_M_SEND_RECEIVE, /* Master transmission reception mode */
    RIIC_MODE_S_READY, /* Multi master transfer mode */
    RIIC_MODE_S_SEND, /* Slave transmission mode */
    RIIC_MODE_S_RECEIVE /* Slave reception mode */
} riic_api_mode_t;

/* ---- Internal Status. ---- */
typedef enum
{
    RIIC_STS_NO_INIT = 0U, /* None initialization state */
    RIIC_STS_IDLE, /* Idle state */
    RIIC_STS_IDLE_EN_SLV, /* Idle state on enable slave transfer */
    RIIC_STS_ST_COND_WAIT, /* Start condition generation completion wait state */
    RIIC_STS_SEND_SLVADR_W_WAIT, /* Slave address [Write] transmission completion wait state */
    RIIC_STS_SEND_SLVADR_R_WAIT, /* Slave address [Read] transmission completion wait state */
    RIIC_STS_SEND_DATA_WAIT, /* Data transmission completion wait state */
    RIIC_STS_RECEIVE_DATA_WAIT, /* Data reception completion wait state */
    RIIC_STS_SP_COND_WAIT, /* Stop condition generation completion wait state */
    RIIC_STS_AL, /* Detect Arbitration Lost */
    RIIC_STS_TMO, /* Detect Time out */
    RIIC_STS_MAX /* Prohibition of setup above here */

} riic_api_status_t;

/* ---- Internal Event. ---- */
typedef enum
{
    RIIC_EV_INIT = 0U, /* Called function of Initializes IIC driver */
    RIIC_EV_EN_SLV_TRANSFER, /* Enable Slave Transfer */
    RIIC_EV_GEN_START_COND, /* Called function of Start condition generation */
    RIIC_EV_INT_START, /* Interrupted start codition generation */
    RIIC_EV_INT_ADD, /* Interrupted address sending */
    RIIC_EV_INT_SEND, /* Interrupted data sending */
    RIIC_EV_INT_RECEIVE, /* Interrupted data receiving */
    RIIC_EV_INT_STOP, /* Interrupted Stop condition generation */
    RIIC_EV_INT_AL, /* Interrupted Arbitration-Lost */
    RIIC_EV_INT_NACK, /* Interrupted No Acknowledge */
    RIIC_EV_INT_TMO, /* Interrupted Time out */
    RIIC_EV_MAX /* Prohibition of setup above here */
} riic_api_event_t;

/* ---- Internal Infomation structure. ---- */
typedef struct
{
    riic_api_mode_t   N_Mode; /* Internal mode of control protocol */
    riic_api_mode_t   B_Mode; /* Internal mode of control protocol */
    riic_api_status_t N_status; /* Internal Status of Now */
    riic_api_status_t B_status; /* Internal Status of Before */
} riic_api_info_t;

/* ---- Internal state transition structure ---- */
typedef struct
{
    riic_api_event_t event_type; /* Event */
    riic_return_t    (*proc) (riic_info_t *); /* handler */
} riic_mtx_t;

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
bool riic_mcu_check_channel (uint8_t channe);
void riic_mcu_int_init (uint8_t channe);
uint8_t riic_mcu_check_ir_txi (uint8_t channel);
uint8_t riic_mcu_check_ir_rxi (uint8_t channel);
void riic_mcu_clear_ir_txi (uint8_t channel);
void riic_mcu_clear_ir_rxi (uint8_t channel);
void riic_mcu_int_enable (uint8_t channe);
void riic_mcu_int_disable (uint8_t channe);
void riic_mcu_power_on (uint8_t channel);
void riic_mcu_power_off (uint8_t channel);
bool riic_mcu_hardware_lock (uint8_t channel);
void riic_mcu_hardware_unlock (uint8_t channel);
double riic_mcu_check_freq (void);
void riic_mcu_int_icier_setting (uint8_t channel, uint8_t New_icier);

    #if (1U == RIIC_CFG_PORT_SET_PROCESSING)
void riic_mcu_io_open (uint8_t channe);
void riic_mcu_mpc_enable (uint8_t channe);
void riic_mcu_mpc_disable (uint8_t channe);
    #endif

    #if (1U == RIIC_CFG_CH0_INCLUDED)
void riic0_eei_sub (void);
void riic0_txi_sub (void);
void riic0_rxi_sub (void);
void riic0_tei_sub (void);
    #endif

    #if (1U == RIIC_CFG_CH1_INCLUDED)
void riic1_eei_sub (void);
void riic1_txi_sub (void);
void riic1_rxi_sub (void);
void riic1_tei_sub (void);
    #endif

    #if (1U == RIIC_CFG_CH2_INCLUDED)
void riic2_eei_sub (void);
void riic2_txi_sub (void);
void riic2_rxi_sub (void);
void riic2_tei_sub (void);
    #endif

#endif /* RIIC_PRIVATE_H */

