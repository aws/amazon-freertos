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
 * File Name    : r_riic_rx_config_reference.h
 * Description  : Configures the RIIC drivers
 **********************************************************************************************************************/
/***********************************************************************************************************************
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
 *         : 17.07.2014 1.30     Added the parameters of channel 2.
 *                               Deleted the parameters of PCLK.
 *                               Added the parameters of the port function assignment.
 *                               Changed the parameters of interrupt priority level.
 *                               Added the parameters for the time out detection.
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
 *         : 02.06.2017 2.10     Added RX24U support.
 *         : 31.08.2017 2.20     Added definitions for Channel 1.
 **********************************************************************************************************************/
/* Guards against multiple inclusion */
#ifndef RIIC_CONFIG_H
    #define RIIC_CONFIG_H
/***********************************************************************************************************************
 Configuration Options
 **********************************************************************************************************************/
/* SPECIFY WHETHER TO INCLUDE CODE FOR API PARAMETER CHECKING */
/* Setting to BSP_CFG_PARAM_CHECKING_ENABLE utilizes the system default setting */
/* Setting to 1 includes parameter checking; 0 compiles out parameter checking */
    #define RIIC_CFG_PARAM_CHECKING_ENABLE  (1U)

/* SPECIFY CHANNELS TO INCLUDE SOFTWARE SUPPORT FOR 1=included, 0=not */
/* mcu supported channels */
/*  RX110: ch0,    ,     */
/*  RX111: ch0,    ,     */
/*  RX113: ch0,    ,     */
/*  RX130: ch0,    ,     */
/*  RX230: ch0,    ,     */
/*  RX231: ch0,    ,     */
/*  RX23T: ch0,    ,     */
/*  RX24T: ch0,    ,     */
/*  RX24U: ch0,    ,     */
/*  RX64M: ch0,    , ch2 */
/*  RX65N: ch0, ch1, ch2 */
/*  RX71M: ch0,    , ch2 */
    #define RIIC_CFG_CH0_INCLUDED           (1U)
    #define RIIC_CFG_CH1_INCLUDED           (0U)
    #define RIIC_CFG_CH2_INCLUDED           (0U)

/* Set RIIC bps(kbps) */
    #define RIIC_CFG_CH0_kBPS               (400U)
    #define RIIC_CFG_CH1_kBPS               (400U)
    #define RIIC_CFG_CH2_kBPS               (400U)

/* Set using digital filter(Selected IIC phi cycle is filtered out) */
/* 0 = not, 1 = one IIC phi, 2 = two IIC phi, 3 = three IIC phi, 4 = four IIC phi */
    #define RIIC_CFG_CH0_DIGITAL_FILTER     (2U)
    #define RIIC_CFG_CH1_DIGITAL_FILTER     (2U)
    #define RIIC_CFG_CH2_DIGITAL_FILTER     (2U)

/* Setting to */
/* 1: includes riic port setting processing */
/* 0: compiles out riic port setting processing */
    #define RIIC_CFG_PORT_SET_PROCESSING    (1U)

/* Set mode */
/* 0 = single master mode, 1 = multi master mode(Master arbitration-lost detection is enabled.) */
    #define RIIC_CFG_CH0_MASTER_MODE        (1U)
    #define RIIC_CFG_CH1_MASTER_MODE        (1U)
    #define RIIC_CFG_CH2_MASTER_MODE        (1U)

/* Set slave address */
/* 0 = not, 1 = 7bit address format, 2 = 10bit address format */
    #define RIIC_CFG_CH0_SLV_ADDR0_FORMAT   (1U)
    #define RIIC_CFG_CH0_SLV_ADDR1_FORMAT   (0U)
    #define RIIC_CFG_CH0_SLV_ADDR2_FORMAT   (0U)

    #define RIIC_CFG_CH0_SLV_ADDR0          (0x0025)
    #define RIIC_CFG_CH0_SLV_ADDR1          (0x0000)
    #define RIIC_CFG_CH0_SLV_ADDR2          (0x0000)

    #define RIIC_CFG_CH1_SLV_ADDR0_FORMAT   (1U)
    #define RIIC_CFG_CH1_SLV_ADDR1_FORMAT   (0U)
    #define RIIC_CFG_CH1_SLV_ADDR2_FORMAT   (0U)

    #define RIIC_CFG_CH1_SLV_ADDR0          (0x0025)
    #define RIIC_CFG_CH1_SLV_ADDR1          (0x0000)
    #define RIIC_CFG_CH1_SLV_ADDR2          (0x0000)

    #define RIIC_CFG_CH2_SLV_ADDR0_FORMAT   (1U)
    #define RIIC_CFG_CH2_SLV_ADDR1_FORMAT   (0U)
    #define RIIC_CFG_CH2_SLV_ADDR2_FORMAT   (0U)

    #define RIIC_CFG_CH2_SLV_ADDR0          (0x0025)
    #define RIIC_CFG_CH2_SLV_ADDR1          (0x0000)
    #define RIIC_CFG_CH2_SLV_ADDR2          (0x0000)

/* Select General call address */
/* 0 = not use, 1 = use(General call address detection is enabled.) */
    #define RIIC_CFG_CH0_SLV_GCA_ENABLE     (0U)
    #define RIIC_CFG_CH1_SLV_GCA_ENABLE     (0U)
    #define RIIC_CFG_CH2_SLV_GCA_ENABLE     (0U)

/* This #define sets the priority level for the riic interrupt */
/* 1 lowest, 15 highest */
/* The following devices can not individually specify the interrupt priority level for EEI0, TEI0, EEI2, TEI2. */
/* EEI and TEI interrupts are grouped as the BL1 interrupt in the RX64M and RX71M group. */
    #define RIIC_CFG_CH0_RXI_INT_PRIORITY   (1U)
    #define RIIC_CFG_CH0_TXI_INT_PRIORITY   (1U)
/* The priority level of the EEI, please do not lower than the priority level of TXI and RXI */
    #define RIIC_CFG_CH0_EEI_INT_PRIORITY   (1U)
/* The priority level of the TEI, please do not lower than the priority level of TXI and RXI */
    #define RIIC_CFG_CH0_TEI_INT_PRIORITY   (1U)

    #define RIIC_CFG_CH1_RXI_INT_PRIORITY   (1U)
    #define RIIC_CFG_CH1_TXI_INT_PRIORITY   (1U)
/* The priority level of the EEI, please do not lower than the priority level of TXI and RXI */
    #define RIIC_CFG_CH1_EEI_INT_PRIORITY   (1U)
/* The priority level of the TEI, please do not lower than the priority level of TXI and RXI */
    #define RIIC_CFG_CH1_TEI_INT_PRIORITY   (1U)

    #define RIIC_CFG_CH2_RXI_INT_PRIORITY   (1U)
    #define RIIC_CFG_CH2_TXI_INT_PRIORITY   (1U)
/* The priority level of the EEI, please do not lower than the priority level of TXI and RXI */
    #define RIIC_CFG_CH2_EEI_INT_PRIORITY   (1U)
/* The priority level of the TEI, please do not lower than the priority level of TXI and RXI */
    #define RIIC_CFG_CH2_TEI_INT_PRIORITY   (1U)

/* Select Timeout function enable or disable */
/* 0 = disable, 1 = enable */
    #define RIIC_CFG_CH0_TMO_ENABLE         (1U)
    #define RIIC_CFG_CH1_TMO_ENABLE         (1U)
    #define RIIC_CFG_CH2_TMO_ENABLE         (1U)

/* Select long mode or short mode for the timeout detection time */
/*    when the timeout function is enabled. */
/* 0 = Long mode, 1 = short mode */
    #define RIIC_CFG_CH0_TMO_DET_TIME       (0U)
    #define RIIC_CFG_CH1_TMO_DET_TIME       (0U)
    #define RIIC_CFG_CH2_TMO_DET_TIME       (0U)

/* Select enable or disable the internal counter of the timeout function to count up while the */
/* SCL line is held LOW when the timeout function is enabled. */
/* 0 = Count is disabled, 1 = Count is enabled */
    #define RIIC_CFG_CH0_TMO_LCNT           (1U)
    #define RIIC_CFG_CH1_TMO_LCNT           (1U)
    #define RIIC_CFG_CH2_TMO_LCNT           (1U)

/* Select enable or disable the internal counter of the timeout function to count up while the */
/* SCL line is held HIGH when the timeout function is enabled. */
/* 0 = Count is disabled, 1 = Count is enabled */
    #define RIIC_CFG_CH0_TMO_HCNT           (1U)
    #define RIIC_CFG_CH1_TMO_HCNT           (1U)
    #define RIIC_CFG_CH2_TMO_HCNT           (1U)

/* Define software bus busy check counter. */
    #define RIIC_CFG_BUS_CHECK_COUNTER      (1000U)     /* Counter of checking bus busy */

#endif /* RIIC_CONFIG_H */

