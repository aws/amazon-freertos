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
int16_t phy_init (uint32_t channel);
void phy_start_autonegotiate (uint32_t channel, uint8_t pause);
int16_t phy_set_autonegotiate (uint32_t channel, uint16_t *pline_speed_duplex, uint16_t *plocal_pause,
        uint16_t *ppartner_pause);
int16_t phy_get_link_status (uint32_t channel);

#endif /* PHY_H */

