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
 * File Name    : phy.h
 * Version      : 1.01
 * Description  : Ethernet PHY device driver
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 22.07.2014 1.00     First Release
 *         : 16.12.2014 1.01     Deleted header file include.
 *         : 22.11.2019 1.20     Added the macro of the phy register
 *                               Added PMGI API function
 ***********************************************************************************************************************/

/* Guards against multiple inclusion */
#ifndef PHY_H
    #define PHY_H

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
    #include <stdint.h>

/***********************************************************************************************************************
 Macro definitions
 ***********************************************************************************************************************/
/* PHY return definitions */
    #define R_PHY_OK        (0)
    #define R_PHY_ERROR     (-1)

/* Standard PHY Registers */
    #define PHY_REG_CONTROL                 (0)
    #define PHY_REG_STATUS                  (1)
    #define PHY_REG_IDENTIFIER1             (2)
    #define PHY_REG_IDENTIFIER2             (3)
    #define PHY_REG_AN_ADVERTISEMENT        (4)
    #define PHY_REG_AN_LINK_PARTNER         (5)
    #define PHY_REG_AN_EXPANSION            (6)

/* Vendor Specific PHY Registers */
#ifdef ETHER_CFG_USE_PHY_KSZ8041NL
    #define PHY_REG_PHY_CONTROL_1           (0x1E)
#endif /* MICREL_KSZ8041NL */

/* Basic Mode Control Register Bit Definitions */
    #define PHY_CONTROL_RESET               (1 << 15)
    #define PHY_CONTROL_LOOPBACK            (1 << 14)
    #define PHY_CONTROL_100_MBPS            (1 << 13)
    #define PHY_CONTROL_AN_ENABLE           (1 << 12)
    #define PHY_CONTROL_POWER_DOWN          (1 << 11)
    #define PHY_CONTROL_ISOLATE             (1 << 10)
    #define PHY_CONTROL_AN_RESTART          (1 << 9)
    #define PHY_CONTROL_FULL_DUPLEX         (1 << 8)
    #define PHY_CONTROL_COLLISION           (1 << 7)

/* Basic Mode Status Register Bit Definitions */
    #define PHY_STATUS_100_T4               (1 << 15)
    #define PHY_STATUS_100F                 (1 << 14)
    #define PHY_STATUS_100H                 (1 << 13)
    #define PHY_STATUS_10F                  (1 << 12)
    #define PHY_STATUS_10H                  (1 << 11)
    #define PHY_STATUS_AN_COMPLETE          (1 << 5)
    #define PHY_STATUS_RM_FAULT             (1 << 4)
    #define PHY_STATUS_AN_ABILITY           (1 << 3)
    #define PHY_STATUS_LINK_UP              (1 << 2)
    #define PHY_STATUS_JABBER               (1 << 1)
    #define PHY_STATUS_EX_CAPABILITY        (1 << 0)

/* Auto Negotiation Advertisement Bit Definitions */
    #define PHY_AN_ADVERTISEMENT_NEXT_PAGE  (1 << 15)
    #define PHY_AN_ADVERTISEMENT_RM_FAULT   (1 << 13)
    #define PHY_AN_ADVERTISEMENT_ASM_DIR    (1 << 11)
    #define PHY_AN_ADVERTISEMENT_PAUSE      (1 << 10)
    #define PHY_AN_ADVERTISEMENT_100_T4     (1 << 9)
    #define PHY_AN_ADVERTISEMENT_100F       (1 << 8)
    #define PHY_AN_ADVERTISEMENT_100H       (1 << 7)
    #define PHY_AN_ADVERTISEMENT_10F        (1 << 6)
    #define PHY_AN_ADVERTISEMENT_10H        (1 << 5)
    #define PHY_AN_ADVERTISEMENT_SELECTOR   (1 << 0)

/* Auto Negotiate Link Partner Ability Bit Definitions */
    #define PHY_AN_LINK_PARTNER_NEXT_PAGE   (1 << 15)
    #define PHY_AN_LINK_PARTNER_ACK         (1 << 14)
    #define PHY_AN_LINK_PARTNER_RM_FAULT    (1 << 13)
    #define PHY_AN_LINK_PARTNER_ASM_DIR     (1 << 11)
    #define PHY_AN_LINK_PARTNER_PAUSE       (1 << 10)
    #define PHY_AN_LINK_PARTNER_100_T4      (1 << 9)
    #define PHY_AN_LINK_PARTNER_100F        (1 << 8)
    #define PHY_AN_LINK_PARTNER_100H        (1 << 7)
    #define PHY_AN_LINK_PARTNER_10F         (1 << 6)
    #define PHY_AN_LINK_PARTNER_10H         (1 << 5)
    #define PHY_AN_LINK_PARTNER_SELECTOR    (1 << 0)

    #define PMGI_READ                       (0)
    #define PMGI_WRITE                      (1)
    #define PMGI_START                      (1)
/***********************************************************************************************************************
 Typedef definitions
 ***********************************************************************************************************************/
typedef enum LinkStatE
{
    PHY_NO_LINK = 0, PHY_LINK_10H, PHY_LINK_10F, PHY_LINK_100H, PHY_LINK_100F
} linkstat_t;

/***********************************************************************************************************************
 Exported global variables
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/
 #if (ETHER_CFG_NON_BLOCKING == 0)
int16_t phy_init (uint32_t channel);
void phy_start_autonegotiate (uint32_t channel, uint8_t pause);
int16_t phy_set_autonegotiate (uint32_t channel, uint16_t *pline_speed_duplex, uint16_t *plocal_pause,
        uint16_t *ppartner_pause);
int16_t phy_get_link_status (uint32_t channel);
uint16_t phy_read (uint32_t ether_channel, uint16_t reg_addr);
void phy_write (uint32_t ether_channel, uint16_t reg_addr, uint16_t data);
#elif (ETHER_CFG_NON_BLOCKING == 1)
int16_t pmgi_initial(uint16_t pmgi_channel);
uint16_t pmgi_read_reg(uint16_t pmgi_channel);
int16_t pmgi_close(uint16_t pmgi_channel);
int16_t pmgi_access(uint32_t channel, uint16_t address, uint16_t data, uint16_t dir);
#endif
#endif /* PHY_H */

