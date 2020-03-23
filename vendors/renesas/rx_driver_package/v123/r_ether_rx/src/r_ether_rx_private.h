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
 * File Name    : r_ether_rx_private.h
 * Version      : 1.17
 * Device       : RX63N/RX65N/RX64M/RX71M/RX72M
 * Tool-Chain   : RX Family C Compiler
 * H/W Platform : 
 * Description  : File that defines macro and structure seen only in "r_ether_rx.c" file.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 22.07.2014 1.00     First Release
 *         : 16.12.2014 1.01     Added changes for RX71M.
 *         : 29.01.2015 1.02     Correction of version check.
 *         : 31.03.2016 1.03     Added changes for RX63N.
 *         : 01.10.2016 1.04     Added changes for RX65N.
 *         : 01.10.2017 1.13     Removed BSP version error.
 *         : 08.01.2018 1.14     Changed setting value check.
 *         : 20.05.2019 1.16     Added support for GNUC and ICCRX.
 *                               Fixed coding style.
 *         : 30.07.2019 1.17     Added changes for RX72M.
  ***********************************************************************************************************************/

/* Guards against multiple inclusion */
#ifndef R_ETHER_PRIVATE_H
    #define R_ETHER_PRIVATE_H

/* This checks that the module of the Ethernet is supported to the MCU that has been selected for sure. */
    #if (defined(BSP_MCU_RX63N) || defined(BSP_MCU_RX65N) || defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72M))

    #else
        #error "This MCU is not supported by the current r_ether_rx module."
    #endif

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
    #include "r_ether_rx_if.h"
    #include "r_ether_rx_config.h"

/***********************************************************************************************************************
 Macro definitions
 ***********************************************************************************************************************/

/* Check the setting values is valid. Please review the setting values in r_ether_rx_config.h if error message is output */
    #if !((ETHER_CFG_MODE_SEL == 0) || (ETHER_CFG_MODE_SEL == 1))
        #error "ERROR- ETHER_CFG_MODE_SEL - Ethernet interface select is out of range defined in r_ether_rx_config.h."
    #endif

    #if !((ETHER_CFG_CH0_PHY_ADDRESS >= 0) && (ETHER_CFG_CH0_PHY_ADDRESS <= 31))
        #error "ERROR- ETHER_CFG_CH0_PHY_ADDRESS - PHY-LSI address is out of range defined in r_ether_rx_config.h."
    #endif

    #if !((ETHER_CFG_CH1_PHY_ADDRESS >= 0) && (ETHER_CFG_CH1_PHY_ADDRESS <= 31))
        #error "ERROR- ETHER_CFG_CH1_PHY_ADDRESS - PHY-LSI address is out of range defined in r_ether_rx_config.h."
    #endif

    #if !(ETHER_CFG_EMAC_RX_DESCRIPTORS >= 1)
        #error "ERROR- ETHER_CFG_EMAC_RX_DESCRIPTORS - Transmission descriptors is out of range defined in r_ether_rx_config.h."
    #endif

    #if !(ETHER_CFG_EMAC_TX_DESCRIPTORS >= 1)
        #error "ERROR- ETHER_CFG_EMAC_TX_DESCRIPTORS - Receive descriptors is out of range defined in r_ether_rx_config.h."
    #endif

    #if !((ETHER_CFG_BUFSIZE % 32) == 0)
        #error "ERROR- ETHER_CFG_BUFSIZE - transmission and receive buffers is not 32-byte aligned in r_ether_rx_config.h."
    #endif

    #if !((ETHER_CFG_CH0_PHY_ACCESS == 0) || (ETHER_CFG_CH0_PHY_ACCESS == 1))
        #error "ERROR- ETHER_CFG_CH0_PHY_ACCESS - PHY-LSI access channel is out of range defined in r_ether_rx_config.h."
    #endif

    #if !((ETHER_CFG_CH1_PHY_ACCESS == 0) || (ETHER_CFG_CH1_PHY_ACCESS == 1))
        #error "ERROR- ETHER_CFG_CH1_PHY_ACCESS - PHY-LSI access channel is out of range defined in r_ether_rx_config.h."
    #endif

    #if !(ETHER_CFG_PHY_MII_WAIT >= 1)
        #error "ERROR- ETHER_CFG_PHY_MII_WAIT - PHY-LSI access timing is out of range defined in r_ether_rx_config.h."
    #endif

    #if !((ETHER_CFG_LINK_PRESENT == 0) || (ETHER_CFG_LINK_PRESENT == 1))
        #error "ERROR- ETHER_CFG_LINK_PRESENT - Link signal polarity of PHY-LSI is out of range defined in r_ether_rx_config.h."
    #endif

    #if !((ETHER_CFG_USE_LINKSTA == 0) || (ETHER_CFG_USE_LINKSTA == 1))
        #error "ERROR- ETHER_CFG_USE_LINKSTA - Use LINKSTA select is out of range defined in r_ether_rx_config.h."
    #endif

    #if !((ETHER_CFG_USE_PHY_KSZ8041NL == 0) || (ETHER_CFG_USE_PHY_KSZ8041NL == 1))
        #error "ERROR- ETHER_CFG_USE_PHY_KSZ8041NL - use KSZ8041NL is out of range defined in r_ether_rx_config.h."
    #endif

/*
 * The total number of EMAC buffers to allocate. The number of
 * total buffers is simply the sum of the number of transmit and
 * receive buffers.
 */
    #define EMAC_NUM_BUFFERS            (ETHER_CFG_EMAC_RX_DESCRIPTORS + (ETHER_CFG_EMAC_TX_DESCRIPTORS))

/* Definition of the maximum / minimum number of data that can be sent at one time in the Ethernet */
    #define ETHER_BUFSIZE_MAX           (1514)          /* Maximum number of transmitted data */
    #define ETHER_BUFSIZE_MIN           (60)            /* Minimum number of transmitted data */

/* Bit definition of interrupt factor of Ethernet interrupt */
    #define EMAC_LCHNG_INT      (1UL << 2)
    #define EMAC_MPD_INT        (1UL << 1)

    #define EMAC_RFCOF_INT      (1UL << 24)
    #define EMAC_ECI_INT        (1UL << 22)
    #define EMAC_TC_INT         (1UL << 21)
    #define EMAC_FR_INT         (1UL << 18)
    #define EMAC_RDE_INT        (1UL << 17)
    #define EMAC_RFOF_INT       (1UL << 16)

/* Bit definitions of status member of DescriptorS */
    #define  TACT               (0x80000000)
    #define  RACT               (0x80000000)
    #define  TDLE               (0x40000000)
    #define  RDLE               (0x40000000)
    #define  TFP1               (0x20000000)
    #define  RFP1               (0x20000000)
    #define  TFP0               (0x10000000)
    #define  RFP0               (0x10000000)
    #define  TFE                (0x08000000)
    #define  RFE                (0x08000000)

    #define  RFS9_RFOVER        (0x00000200)
    #define  RFS8_RAD           (0x00000100)
    #define  RFS7_RMAF          (0x00000080)
    #define  RFS4_RRF           (0x00000010)
    #define  RFS3_RTLF          (0x00000008)
    #define  RFS2_RTSF          (0x00000004)
    #define  RFS1_PRE           (0x00000002)
    #define  RFS0_CERF          (0x00000001)

    #define  TWBI               (0x04000000)
    #define  TFS8_TAD           (0x00000100)
    #define  TFS3_CND           (0x00000008)
    #define  TFS2_DLC           (0x00000004)
    #define  TFS1_CD            (0x00000002)
    #define  TFS0_TRO           (0x00000001)

/* Number of entries in PAUSE resolution table */
    #define PAUSE_TABLE_ENTRIES (8)

/* Local device and link partner PAUSE settings */
    #define XMIT_PAUSE_OFF      (0) /* The pause frame transmission is prohibited. */
    #define RECV_PAUSE_OFF      (0) /* The pause frame reception is prohibited.    */
    #define XMIT_PAUSE_ON       (1) /* The pause frame transmission is permitted.  */
    #define RECV_PAUSE_ON       (1) /* The pause frame reception is permitted.     */

/* PAUSE link mask and shift values */
/*
 * The mask value and shift value which are for that shift the bits form a line and 
 * for comparing the bit information of PAUSE function which support the local device and 
 * Link partner with the assorted table(pause_resolution) which enable or disable the PAUSE frame. 
 */
    #define LINK_RES_ABILITY_MASK           (3)
    #define LINK_RES_LOCAL_ABILITY_BITSHIFT (2)

/* Etherc mode */
    #define NO_USE_MAGIC_PACKET_DETECT  (0)
    #define USE_MAGIC_PACKET_DETECT     (1)

/* Defines the port connection to be used in the Ether */
    #define PORT_CONNECT_ET0            (0x01)
    #define PORT_CONNECT_ET1            (0x02)
    #define PORT_CONNECT_ET0_ET1        (0x03)

/** PHY access channel */
    #define ETHER_PHY_ACCESS_CHANNEL_0      (0)
    #define ETHER_PHY_ACCESS_CHANNEL_1      (1)
    #define ETHER_PHY_ACCESS_CHANNEL_MAX    (ETHER_CHANNEL_MAX)

/** Ethernet module usage status */
    #define ETEHR_MODULE_NOT_USE    (0) /* Ethernet module is not used */
    #define ETHER_MODULE_USE        (1) /* Ethernet module is used */

/***********************************************************************************************************************
 Typedef definitions
 ***********************************************************************************************************************/
/*
 * EDMAC descriptor as defined in the hardware manual. It is
 * modified to support little endian CPU mode.
 */
R_BSP_PRAGMA_PACK;
typedef struct DescriptorS
{
    R_BSP_EVENACCESS uint32_t           status;
    #if __LIT
    /* Little endian */
    R_BSP_EVENACCESS uint16_t           size;
    R_BSP_EVENACCESS uint16_t           bufsize;
    #else
    /* Big endian */
    R_BSP_EVENACCESS uint16_t bufsize;
    R_BSP_EVENACCESS uint16_t size;

    #endif
    uint8_t            *buf_p;
    struct DescriptorS *next;
} descriptor_t;

/*
 * Ethernet buffer type definition.  
 */
typedef struct EtherBufferS
{
    uint8_t buffer[EMAC_NUM_BUFFERS][ETHER_CFG_BUFSIZE];

} etherbuffer_t;

/*
 * PauseMaskE, PauseValE and pause_resolutionS are use to create
 * PAUSE resolution Table 28B-3 in IEEE 802.3-2008 standard.
 */
typedef enum PauseMaskE
{
    PAUSE_MASK0,
    PAUSE_MASK1,
    PAUSE_MASK2,
    PAUSE_MASK3,
    PAUSE_MASK4,
    PAUSE_MASK5,
    PAUSE_MASK6,
    PAUSE_MASK7,
    PAUSE_MASK8,
    PAUSE_MASK9,
    PAUSE_MASKA,
    PAUSE_MASKB,
    PAUSE_MASKC,
    PAUSE_MASKD,
    PAUSE_MASKE,
    PAUSE_MASKF
} pausemask_t;

typedef enum PauseValE
{
    PAUSE_VAL0,
    PAUSE_VAL1,
    PAUSE_VAL2,
    PAUSE_VAL3,
    PAUSE_VAL4,
    PAUSE_VAL5,
    PAUSE_VAL6,
    PAUSE_VAL7,
    PAUSE_VAL8,
    PAUSE_VAL9,
    PAUSE_VALA,
    PAUSE_VALB,
    PAUSE_VALC,
    PAUSE_VALD,
    PAUSE_VALE,
    PAUSE_VALF
} pauseval_t;

typedef struct pause_resolutionS
{
    pausemask_t mask;
    pauseval_t  value;
    uint8_t     transmit;
    uint8_t     receive;
} pauseresolution_t;

typedef struct
{
    volatile struct st_etherc R_BSP_EVENACCESS_SFR * petherc; /* ETHERC module */
    volatile struct st_edmac R_BSP_EVENACCESS_SFR * pedmac; /* EDMAC */
    volatile uint32_t         R_BSP_EVENACCESS_SFR * preg_pir;
    uint32_t                  phy_address;
    uint8_t                   port_connect;
} ether_control_t;

typedef struct
{
    const ether_control_t * pether_control;
    uint32_t              phy_access;
} ether_ch_control_t;

R_BSP_PRAGMA_PACKOPTION;

/***********************************************************************************************************************
 Exported global variables
 ***********************************************************************************************************************/
extern const ether_ch_control_t g_eth_control_ch[];

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/
void ether_enable_icu (uint32_t channel);
void ether_disable_icu (uint32_t channel);
void ether_set_phy_mode (uint8_t connect);

#endif /* R_ETHER_PRIVATE_H */
