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
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name     : r_s12ad_rx_config.h
* Description   : Configures the 12-bit A/D driver
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           22.07.2013 1.00    Initial Release.
*           21.04.2014 1.20    Updated for RX210 advanced features; RX110/63x support.
***********************************************************************************************************************/
#ifndef S12AD_CONFIG_H
#define S12AD_CONFIG_H

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/

/*
 * SPECIFY WHETHER TO INCLUDE CODE FOR API PARAMETER CHECKING
 * Setting to BSP_CFG_PARAM_CHECKING_ENABLE utilizes the system default setting.
 * Setting to 1 includes parameter checking; 0 compiles out parameter checking.
 */
#define ADC_CFG_PARAM_CHECKING_ENABLE   BSP_CFG_PARAM_CHECKING_ENABLE


/*
 * RX210 Series only; Temperature Sensor Programmable Gain Amplifier (PGA)
 * SELECT PGA GAIN TO MATCH AVCC0 VOLTAGE RANGE:
 *   0:   1.8V <= AVcc0 <  2.7V; default; good for all actual voltages
 *   1:   2.7V <= AVcc0 <  3.6V
 *   2:   3.6V <= AVcc0 <  4.5V
 *   3:   4.5V <= AVcc0 <= 5.5V
 */
#define ADC_CFG_PGA_GAIN    (0)


#endif /* S12AD_CONFIG_H */
