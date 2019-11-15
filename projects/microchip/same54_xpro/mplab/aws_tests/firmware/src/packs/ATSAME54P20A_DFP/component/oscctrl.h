/**
 * \brief Component description for OSCCTRL
 *
 * Copyright (c) 2019 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip software and any derivatives
 * exclusively with Microchip products. It is your responsibility to comply with third party license
 * terms applicable to your use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY,
 * APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT
 * EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/* file generated from device description version 2019-06-03T16:18:52Z */
#ifndef _SAME54_OSCCTRL_COMPONENT_H_
#define _SAME54_OSCCTRL_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR OSCCTRL                                      */
/* ************************************************************************** */

/* -------- OSCCTRL_DPLLCTRLA : (OSCCTRL Offset: 0x00) (R/W 8) DPLL Control A -------- */
#define OSCCTRL_DPLLCTRLA_RESETVALUE          _U_(0x80)                                            /**<  (OSCCTRL_DPLLCTRLA) DPLL Control A  Reset Value */

#define OSCCTRL_DPLLCTRLA_ENABLE_Pos          _U_(1)                                               /**< (OSCCTRL_DPLLCTRLA) DPLL Enable Position */
#define OSCCTRL_DPLLCTRLA_ENABLE_Msk          (_U_(0x1) << OSCCTRL_DPLLCTRLA_ENABLE_Pos)           /**< (OSCCTRL_DPLLCTRLA) DPLL Enable Mask */
#define OSCCTRL_DPLLCTRLA_ENABLE(value)       (OSCCTRL_DPLLCTRLA_ENABLE_Msk & ((value) << OSCCTRL_DPLLCTRLA_ENABLE_Pos))
#define OSCCTRL_DPLLCTRLA_RUNSTDBY_Pos        _U_(6)                                               /**< (OSCCTRL_DPLLCTRLA) Run in Standby Position */
#define OSCCTRL_DPLLCTRLA_RUNSTDBY_Msk        (_U_(0x1) << OSCCTRL_DPLLCTRLA_RUNSTDBY_Pos)         /**< (OSCCTRL_DPLLCTRLA) Run in Standby Mask */
#define OSCCTRL_DPLLCTRLA_RUNSTDBY(value)     (OSCCTRL_DPLLCTRLA_RUNSTDBY_Msk & ((value) << OSCCTRL_DPLLCTRLA_RUNSTDBY_Pos))
#define OSCCTRL_DPLLCTRLA_ONDEMAND_Pos        _U_(7)                                               /**< (OSCCTRL_DPLLCTRLA) On Demand Control Position */
#define OSCCTRL_DPLLCTRLA_ONDEMAND_Msk        (_U_(0x1) << OSCCTRL_DPLLCTRLA_ONDEMAND_Pos)         /**< (OSCCTRL_DPLLCTRLA) On Demand Control Mask */
#define OSCCTRL_DPLLCTRLA_ONDEMAND(value)     (OSCCTRL_DPLLCTRLA_ONDEMAND_Msk & ((value) << OSCCTRL_DPLLCTRLA_ONDEMAND_Pos))
#define OSCCTRL_DPLLCTRLA_Msk                 _U_(0xC2)                                            /**< (OSCCTRL_DPLLCTRLA) Register Mask  */


/* -------- OSCCTRL_DPLLRATIO : (OSCCTRL Offset: 0x04) (R/W 32) DPLL Ratio Control -------- */
#define OSCCTRL_DPLLRATIO_RESETVALUE          _U_(0x00)                                            /**<  (OSCCTRL_DPLLRATIO) DPLL Ratio Control  Reset Value */

#define OSCCTRL_DPLLRATIO_LDR_Pos             _U_(0)                                               /**< (OSCCTRL_DPLLRATIO) Loop Divider Ratio Position */
#define OSCCTRL_DPLLRATIO_LDR_Msk             (_U_(0x1FFF) << OSCCTRL_DPLLRATIO_LDR_Pos)           /**< (OSCCTRL_DPLLRATIO) Loop Divider Ratio Mask */
#define OSCCTRL_DPLLRATIO_LDR(value)          (OSCCTRL_DPLLRATIO_LDR_Msk & ((value) << OSCCTRL_DPLLRATIO_LDR_Pos))
#define OSCCTRL_DPLLRATIO_LDRFRAC_Pos         _U_(16)                                              /**< (OSCCTRL_DPLLRATIO) Loop Divider Ratio Fractional Part Position */
#define OSCCTRL_DPLLRATIO_LDRFRAC_Msk         (_U_(0x1F) << OSCCTRL_DPLLRATIO_LDRFRAC_Pos)         /**< (OSCCTRL_DPLLRATIO) Loop Divider Ratio Fractional Part Mask */
#define OSCCTRL_DPLLRATIO_LDRFRAC(value)      (OSCCTRL_DPLLRATIO_LDRFRAC_Msk & ((value) << OSCCTRL_DPLLRATIO_LDRFRAC_Pos))
#define OSCCTRL_DPLLRATIO_Msk                 _U_(0x001F1FFF)                                      /**< (OSCCTRL_DPLLRATIO) Register Mask  */


/* -------- OSCCTRL_DPLLCTRLB : (OSCCTRL Offset: 0x08) (R/W 32) DPLL Control B -------- */
#define OSCCTRL_DPLLCTRLB_RESETVALUE          _U_(0x20)                                            /**<  (OSCCTRL_DPLLCTRLB) DPLL Control B  Reset Value */

#define OSCCTRL_DPLLCTRLB_FILTER_Pos          _U_(0)                                               /**< (OSCCTRL_DPLLCTRLB) Proportional Integral Filter Selection Position */
#define OSCCTRL_DPLLCTRLB_FILTER_Msk          (_U_(0xF) << OSCCTRL_DPLLCTRLB_FILTER_Pos)           /**< (OSCCTRL_DPLLCTRLB) Proportional Integral Filter Selection Mask */
#define OSCCTRL_DPLLCTRLB_FILTER(value)       (OSCCTRL_DPLLCTRLB_FILTER_Msk & ((value) << OSCCTRL_DPLLCTRLB_FILTER_Pos))
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER1_Val _U_(0x0)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 92.7Khz and Damping Factor = 0.76  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER2_Val _U_(0x1)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 131Khz and Damping Factor = 1.08  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER3_Val _U_(0x2)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 46.4Khz and Damping Factor = 0.38  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER4_Val _U_(0x3)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 65.6Khz and Damping Factor = 0.54  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER5_Val _U_(0x4)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 131Khz and Damping Factor = 0.56  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER6_Val _U_(0x5)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 185Khz and Damping Factor = 0.79  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER7_Val _U_(0x6)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 65.6Khz and Damping Factor = 0.28  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER8_Val _U_(0x7)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 92.7Khz and Damping Factor = 0.39  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER9_Val _U_(0x8)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 46.4Khz and Damping Factor = 1.49  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER10_Val _U_(0x9)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 65.6Khz and Damping Factor = 2.11  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER11_Val _U_(0xA)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 23.2Khz and Damping Factor = 0.75  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER12_Val _U_(0xB)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 32.8Khz and Damping Factor = 1.06  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER13_Val _U_(0xC)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 65.6Khz and Damping Factor = 1.07  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER14_Val _U_(0xD)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 92.7Khz and Damping Factor = 1.51  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER15_Val _U_(0xE)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 32.8Khz and Damping Factor = 0.53  */
#define   OSCCTRL_DPLLCTRLB_FILTER_FILTER16_Val _U_(0xF)                                             /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 46.4Khz and Damping Factor = 0.75  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER1      (OSCCTRL_DPLLCTRLB_FILTER_FILTER1_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 92.7Khz and Damping Factor = 0.76 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER2      (OSCCTRL_DPLLCTRLB_FILTER_FILTER2_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 131Khz and Damping Factor = 1.08 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER3      (OSCCTRL_DPLLCTRLB_FILTER_FILTER3_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 46.4Khz and Damping Factor = 0.38 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER4      (OSCCTRL_DPLLCTRLB_FILTER_FILTER4_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 65.6Khz and Damping Factor = 0.54 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER5      (OSCCTRL_DPLLCTRLB_FILTER_FILTER5_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 131Khz and Damping Factor = 0.56 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER6      (OSCCTRL_DPLLCTRLB_FILTER_FILTER6_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 185Khz and Damping Factor = 0.79 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER7      (OSCCTRL_DPLLCTRLB_FILTER_FILTER7_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 65.6Khz and Damping Factor = 0.28 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER8      (OSCCTRL_DPLLCTRLB_FILTER_FILTER8_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 92.7Khz and Damping Factor = 0.39 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER9      (OSCCTRL_DPLLCTRLB_FILTER_FILTER9_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 46.4Khz and Damping Factor = 1.49 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER10     (OSCCTRL_DPLLCTRLB_FILTER_FILTER10_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 65.6Khz and Damping Factor = 2.11 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER11     (OSCCTRL_DPLLCTRLB_FILTER_FILTER11_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 23.2Khz and Damping Factor = 0.75 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER12     (OSCCTRL_DPLLCTRLB_FILTER_FILTER12_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 32.8Khz and Damping Factor = 1.06 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER13     (OSCCTRL_DPLLCTRLB_FILTER_FILTER13_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 65.6Khz and Damping Factor = 1.07 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER14     (OSCCTRL_DPLLCTRLB_FILTER_FILTER14_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 92.7Khz and Damping Factor = 1.51 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER15     (OSCCTRL_DPLLCTRLB_FILTER_FILTER15_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 32.8Khz and Damping Factor = 0.53 Position  */
#define OSCCTRL_DPLLCTRLB_FILTER_FILTER16     (OSCCTRL_DPLLCTRLB_FILTER_FILTER16_Val << OSCCTRL_DPLLCTRLB_FILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Bandwidth = 46.4Khz and Damping Factor = 0.75 Position  */
#define OSCCTRL_DPLLCTRLB_WUF_Pos             _U_(4)                                               /**< (OSCCTRL_DPLLCTRLB) Wake Up Fast Position */
#define OSCCTRL_DPLLCTRLB_WUF_Msk             (_U_(0x1) << OSCCTRL_DPLLCTRLB_WUF_Pos)              /**< (OSCCTRL_DPLLCTRLB) Wake Up Fast Mask */
#define OSCCTRL_DPLLCTRLB_WUF(value)          (OSCCTRL_DPLLCTRLB_WUF_Msk & ((value) << OSCCTRL_DPLLCTRLB_WUF_Pos))
#define OSCCTRL_DPLLCTRLB_REFCLK_Pos          _U_(5)                                               /**< (OSCCTRL_DPLLCTRLB) Reference Clock Selection Position */
#define OSCCTRL_DPLLCTRLB_REFCLK_Msk          (_U_(0x7) << OSCCTRL_DPLLCTRLB_REFCLK_Pos)           /**< (OSCCTRL_DPLLCTRLB) Reference Clock Selection Mask */
#define OSCCTRL_DPLLCTRLB_REFCLK(value)       (OSCCTRL_DPLLCTRLB_REFCLK_Msk & ((value) << OSCCTRL_DPLLCTRLB_REFCLK_Pos))
#define   OSCCTRL_DPLLCTRLB_REFCLK_GCLK_Val   _U_(0x0)                                             /**< (OSCCTRL_DPLLCTRLB) Dedicated GCLK clock reference  */
#define   OSCCTRL_DPLLCTRLB_REFCLK_XOSC32_Val _U_(0x1)                                             /**< (OSCCTRL_DPLLCTRLB) XOSC32K clock reference  */
#define   OSCCTRL_DPLLCTRLB_REFCLK_XOSC0_Val  _U_(0x2)                                             /**< (OSCCTRL_DPLLCTRLB) XOSC0 clock reference  */
#define   OSCCTRL_DPLLCTRLB_REFCLK_XOSC1_Val  _U_(0x3)                                             /**< (OSCCTRL_DPLLCTRLB) XOSC1 clock reference  */
#define OSCCTRL_DPLLCTRLB_REFCLK_GCLK         (OSCCTRL_DPLLCTRLB_REFCLK_GCLK_Val << OSCCTRL_DPLLCTRLB_REFCLK_Pos) /**< (OSCCTRL_DPLLCTRLB) Dedicated GCLK clock reference Position  */
#define OSCCTRL_DPLLCTRLB_REFCLK_XOSC32       (OSCCTRL_DPLLCTRLB_REFCLK_XOSC32_Val << OSCCTRL_DPLLCTRLB_REFCLK_Pos) /**< (OSCCTRL_DPLLCTRLB) XOSC32K clock reference Position  */
#define OSCCTRL_DPLLCTRLB_REFCLK_XOSC0        (OSCCTRL_DPLLCTRLB_REFCLK_XOSC0_Val << OSCCTRL_DPLLCTRLB_REFCLK_Pos) /**< (OSCCTRL_DPLLCTRLB) XOSC0 clock reference Position  */
#define OSCCTRL_DPLLCTRLB_REFCLK_XOSC1        (OSCCTRL_DPLLCTRLB_REFCLK_XOSC1_Val << OSCCTRL_DPLLCTRLB_REFCLK_Pos) /**< (OSCCTRL_DPLLCTRLB) XOSC1 clock reference Position  */
#define OSCCTRL_DPLLCTRLB_LTIME_Pos           _U_(8)                                               /**< (OSCCTRL_DPLLCTRLB) Lock Time Position */
#define OSCCTRL_DPLLCTRLB_LTIME_Msk           (_U_(0x7) << OSCCTRL_DPLLCTRLB_LTIME_Pos)            /**< (OSCCTRL_DPLLCTRLB) Lock Time Mask */
#define OSCCTRL_DPLLCTRLB_LTIME(value)        (OSCCTRL_DPLLCTRLB_LTIME_Msk & ((value) << OSCCTRL_DPLLCTRLB_LTIME_Pos))
#define   OSCCTRL_DPLLCTRLB_LTIME_DEFAULT_Val _U_(0x0)                                             /**< (OSCCTRL_DPLLCTRLB) No time-out. Automatic lock  */
#define   OSCCTRL_DPLLCTRLB_LTIME_800US_Val   _U_(0x4)                                             /**< (OSCCTRL_DPLLCTRLB) Time-out if no lock within 800us  */
#define   OSCCTRL_DPLLCTRLB_LTIME_900US_Val   _U_(0x5)                                             /**< (OSCCTRL_DPLLCTRLB) Time-out if no lock within 900us  */
#define   OSCCTRL_DPLLCTRLB_LTIME_1MS_Val     _U_(0x6)                                             /**< (OSCCTRL_DPLLCTRLB) Time-out if no lock within 1ms  */
#define   OSCCTRL_DPLLCTRLB_LTIME_1P1MS_Val   _U_(0x7)                                             /**< (OSCCTRL_DPLLCTRLB) Time-out if no lock within 1.1ms  */
#define OSCCTRL_DPLLCTRLB_LTIME_DEFAULT       (OSCCTRL_DPLLCTRLB_LTIME_DEFAULT_Val << OSCCTRL_DPLLCTRLB_LTIME_Pos) /**< (OSCCTRL_DPLLCTRLB) No time-out. Automatic lock Position  */
#define OSCCTRL_DPLLCTRLB_LTIME_800US         (OSCCTRL_DPLLCTRLB_LTIME_800US_Val << OSCCTRL_DPLLCTRLB_LTIME_Pos) /**< (OSCCTRL_DPLLCTRLB) Time-out if no lock within 800us Position  */
#define OSCCTRL_DPLLCTRLB_LTIME_900US         (OSCCTRL_DPLLCTRLB_LTIME_900US_Val << OSCCTRL_DPLLCTRLB_LTIME_Pos) /**< (OSCCTRL_DPLLCTRLB) Time-out if no lock within 900us Position  */
#define OSCCTRL_DPLLCTRLB_LTIME_1MS           (OSCCTRL_DPLLCTRLB_LTIME_1MS_Val << OSCCTRL_DPLLCTRLB_LTIME_Pos) /**< (OSCCTRL_DPLLCTRLB) Time-out if no lock within 1ms Position  */
#define OSCCTRL_DPLLCTRLB_LTIME_1P1MS         (OSCCTRL_DPLLCTRLB_LTIME_1P1MS_Val << OSCCTRL_DPLLCTRLB_LTIME_Pos) /**< (OSCCTRL_DPLLCTRLB) Time-out if no lock within 1.1ms Position  */
#define OSCCTRL_DPLLCTRLB_LBYPASS_Pos         _U_(11)                                              /**< (OSCCTRL_DPLLCTRLB) Lock Bypass Position */
#define OSCCTRL_DPLLCTRLB_LBYPASS_Msk         (_U_(0x1) << OSCCTRL_DPLLCTRLB_LBYPASS_Pos)          /**< (OSCCTRL_DPLLCTRLB) Lock Bypass Mask */
#define OSCCTRL_DPLLCTRLB_LBYPASS(value)      (OSCCTRL_DPLLCTRLB_LBYPASS_Msk & ((value) << OSCCTRL_DPLLCTRLB_LBYPASS_Pos))
#define OSCCTRL_DPLLCTRLB_DCOFILTER_Pos       _U_(12)                                              /**< (OSCCTRL_DPLLCTRLB) Sigma-Delta DCO Filter Selection Position */
#define OSCCTRL_DPLLCTRLB_DCOFILTER_Msk       (_U_(0x7) << OSCCTRL_DPLLCTRLB_DCOFILTER_Pos)        /**< (OSCCTRL_DPLLCTRLB) Sigma-Delta DCO Filter Selection Mask */
#define OSCCTRL_DPLLCTRLB_DCOFILTER(value)    (OSCCTRL_DPLLCTRLB_DCOFILTER_Msk & ((value) << OSCCTRL_DPLLCTRLB_DCOFILTER_Pos))
#define   OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER1_Val _U_(0x0)                                             /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 0.5 and Bandwidth Fn (MHz) = 3.21  */
#define   OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER2_Val _U_(0x1)                                             /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 1 and Bandwidth Fn (MHz) = 1.6  */
#define   OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER3_Val _U_(0x2)                                             /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 1.5 and Bandwidth Fn (MHz) = 1.1  */
#define   OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER4_Val _U_(0x3)                                             /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 2 and Bandwidth Fn (MHz) = 0.8  */
#define   OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER5_Val _U_(0x4)                                             /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 2.5 and Bandwidth Fn (MHz) = 0.64  */
#define   OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER6_Val _U_(0x5)                                             /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 3 and Bandwidth Fn (MHz) = 0.55  */
#define   OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER7_Val _U_(0x6)                                             /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 3.5 and Bandwidth Fn (MHz) = 0.45  */
#define   OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER8_Val _U_(0x7)                                             /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 4 and Bandwidth Fn (MHz) = 0.4  */
#define OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER1   (OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER1_Val << OSCCTRL_DPLLCTRLB_DCOFILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 0.5 and Bandwidth Fn (MHz) = 3.21 Position  */
#define OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER2   (OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER2_Val << OSCCTRL_DPLLCTRLB_DCOFILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 1 and Bandwidth Fn (MHz) = 1.6 Position  */
#define OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER3   (OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER3_Val << OSCCTRL_DPLLCTRLB_DCOFILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 1.5 and Bandwidth Fn (MHz) = 1.1 Position  */
#define OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER4   (OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER4_Val << OSCCTRL_DPLLCTRLB_DCOFILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 2 and Bandwidth Fn (MHz) = 0.8 Position  */
#define OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER5   (OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER5_Val << OSCCTRL_DPLLCTRLB_DCOFILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 2.5 and Bandwidth Fn (MHz) = 0.64 Position  */
#define OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER6   (OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER6_Val << OSCCTRL_DPLLCTRLB_DCOFILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 3 and Bandwidth Fn (MHz) = 0.55 Position  */
#define OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER7   (OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER7_Val << OSCCTRL_DPLLCTRLB_DCOFILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 3.5 and Bandwidth Fn (MHz) = 0.45 Position  */
#define OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER8   (OSCCTRL_DPLLCTRLB_DCOFILTER_FILTER8_Val << OSCCTRL_DPLLCTRLB_DCOFILTER_Pos) /**< (OSCCTRL_DPLLCTRLB) Capacitor(pF) = 4 and Bandwidth Fn (MHz) = 0.4 Position  */
#define OSCCTRL_DPLLCTRLB_DCOEN_Pos           _U_(15)                                              /**< (OSCCTRL_DPLLCTRLB) DCO Filter Enable Position */
#define OSCCTRL_DPLLCTRLB_DCOEN_Msk           (_U_(0x1) << OSCCTRL_DPLLCTRLB_DCOEN_Pos)            /**< (OSCCTRL_DPLLCTRLB) DCO Filter Enable Mask */
#define OSCCTRL_DPLLCTRLB_DCOEN(value)        (OSCCTRL_DPLLCTRLB_DCOEN_Msk & ((value) << OSCCTRL_DPLLCTRLB_DCOEN_Pos))
#define OSCCTRL_DPLLCTRLB_DIV_Pos             _U_(16)                                              /**< (OSCCTRL_DPLLCTRLB) Clock Divider Position */
#define OSCCTRL_DPLLCTRLB_DIV_Msk             (_U_(0x7FF) << OSCCTRL_DPLLCTRLB_DIV_Pos)            /**< (OSCCTRL_DPLLCTRLB) Clock Divider Mask */
#define OSCCTRL_DPLLCTRLB_DIV(value)          (OSCCTRL_DPLLCTRLB_DIV_Msk & ((value) << OSCCTRL_DPLLCTRLB_DIV_Pos))
#define OSCCTRL_DPLLCTRLB_Msk                 _U_(0x07FFFFFF)                                      /**< (OSCCTRL_DPLLCTRLB) Register Mask  */


/* -------- OSCCTRL_DPLLSYNCBUSY : (OSCCTRL Offset: 0x0C) ( R/ 32) DPLL Synchronization Busy -------- */
#define OSCCTRL_DPLLSYNCBUSY_RESETVALUE       _U_(0x00)                                            /**<  (OSCCTRL_DPLLSYNCBUSY) DPLL Synchronization Busy  Reset Value */

#define OSCCTRL_DPLLSYNCBUSY_ENABLE_Pos       _U_(1)                                               /**< (OSCCTRL_DPLLSYNCBUSY) DPLL Enable Synchronization Status Position */
#define OSCCTRL_DPLLSYNCBUSY_ENABLE_Msk       (_U_(0x1) << OSCCTRL_DPLLSYNCBUSY_ENABLE_Pos)        /**< (OSCCTRL_DPLLSYNCBUSY) DPLL Enable Synchronization Status Mask */
#define OSCCTRL_DPLLSYNCBUSY_ENABLE(value)    (OSCCTRL_DPLLSYNCBUSY_ENABLE_Msk & ((value) << OSCCTRL_DPLLSYNCBUSY_ENABLE_Pos))
#define OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Pos    _U_(2)                                               /**< (OSCCTRL_DPLLSYNCBUSY) DPLL Loop Divider Ratio Synchronization Status Position */
#define OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Msk    (_U_(0x1) << OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Pos)     /**< (OSCCTRL_DPLLSYNCBUSY) DPLL Loop Divider Ratio Synchronization Status Mask */
#define OSCCTRL_DPLLSYNCBUSY_DPLLRATIO(value) (OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Msk & ((value) << OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Pos))
#define OSCCTRL_DPLLSYNCBUSY_Msk              _U_(0x00000006)                                      /**< (OSCCTRL_DPLLSYNCBUSY) Register Mask  */


/* -------- OSCCTRL_DPLLSTATUS : (OSCCTRL Offset: 0x10) ( R/ 32) DPLL Status -------- */
#define OSCCTRL_DPLLSTATUS_RESETVALUE         _U_(0x00)                                            /**<  (OSCCTRL_DPLLSTATUS) DPLL Status  Reset Value */

#define OSCCTRL_DPLLSTATUS_LOCK_Pos           _U_(0)                                               /**< (OSCCTRL_DPLLSTATUS) DPLL Lock Status Position */
#define OSCCTRL_DPLLSTATUS_LOCK_Msk           (_U_(0x1) << OSCCTRL_DPLLSTATUS_LOCK_Pos)            /**< (OSCCTRL_DPLLSTATUS) DPLL Lock Status Mask */
#define OSCCTRL_DPLLSTATUS_LOCK(value)        (OSCCTRL_DPLLSTATUS_LOCK_Msk & ((value) << OSCCTRL_DPLLSTATUS_LOCK_Pos))
#define OSCCTRL_DPLLSTATUS_CLKRDY_Pos         _U_(1)                                               /**< (OSCCTRL_DPLLSTATUS) DPLL Clock Ready Position */
#define OSCCTRL_DPLLSTATUS_CLKRDY_Msk         (_U_(0x1) << OSCCTRL_DPLLSTATUS_CLKRDY_Pos)          /**< (OSCCTRL_DPLLSTATUS) DPLL Clock Ready Mask */
#define OSCCTRL_DPLLSTATUS_CLKRDY(value)      (OSCCTRL_DPLLSTATUS_CLKRDY_Msk & ((value) << OSCCTRL_DPLLSTATUS_CLKRDY_Pos))
#define OSCCTRL_DPLLSTATUS_Msk                _U_(0x00000003)                                      /**< (OSCCTRL_DPLLSTATUS) Register Mask  */


/* -------- OSCCTRL_EVCTRL : (OSCCTRL Offset: 0x00) (R/W 8) Event Control -------- */
#define OSCCTRL_EVCTRL_RESETVALUE             _U_(0x00)                                            /**<  (OSCCTRL_EVCTRL) Event Control  Reset Value */

#define OSCCTRL_EVCTRL_CFDEO0_Pos             _U_(0)                                               /**< (OSCCTRL_EVCTRL) Clock 0 Failure Detector Event Output Enable Position */
#define OSCCTRL_EVCTRL_CFDEO0_Msk             (_U_(0x1) << OSCCTRL_EVCTRL_CFDEO0_Pos)              /**< (OSCCTRL_EVCTRL) Clock 0 Failure Detector Event Output Enable Mask */
#define OSCCTRL_EVCTRL_CFDEO0(value)          (OSCCTRL_EVCTRL_CFDEO0_Msk & ((value) << OSCCTRL_EVCTRL_CFDEO0_Pos))
#define OSCCTRL_EVCTRL_CFDEO1_Pos             _U_(1)                                               /**< (OSCCTRL_EVCTRL) Clock 1 Failure Detector Event Output Enable Position */
#define OSCCTRL_EVCTRL_CFDEO1_Msk             (_U_(0x1) << OSCCTRL_EVCTRL_CFDEO1_Pos)              /**< (OSCCTRL_EVCTRL) Clock 1 Failure Detector Event Output Enable Mask */
#define OSCCTRL_EVCTRL_CFDEO1(value)          (OSCCTRL_EVCTRL_CFDEO1_Msk & ((value) << OSCCTRL_EVCTRL_CFDEO1_Pos))
#define OSCCTRL_EVCTRL_Msk                    _U_(0x03)                                            /**< (OSCCTRL_EVCTRL) Register Mask  */

#define OSCCTRL_EVCTRL_CFDEO_Pos              _U_(0)                                               /**< (OSCCTRL_EVCTRL Position) Clock x Failure Detector Event Output Enable */
#define OSCCTRL_EVCTRL_CFDEO_Msk              (_U_(0x3) << OSCCTRL_EVCTRL_CFDEO_Pos)               /**< (OSCCTRL_EVCTRL Mask) CFDEO */
#define OSCCTRL_EVCTRL_CFDEO(value)           (OSCCTRL_EVCTRL_CFDEO_Msk & ((value) << OSCCTRL_EVCTRL_CFDEO_Pos)) 

/* -------- OSCCTRL_INTENCLR : (OSCCTRL Offset: 0x04) (R/W 32) Interrupt Enable Clear -------- */
#define OSCCTRL_INTENCLR_RESETVALUE           _U_(0x00)                                            /**<  (OSCCTRL_INTENCLR) Interrupt Enable Clear  Reset Value */

#define OSCCTRL_INTENCLR_XOSCRDY0_Pos         _U_(0)                                               /**< (OSCCTRL_INTENCLR) XOSC 0 Ready Interrupt Enable Position */
#define OSCCTRL_INTENCLR_XOSCRDY0_Msk         (_U_(0x1) << OSCCTRL_INTENCLR_XOSCRDY0_Pos)          /**< (OSCCTRL_INTENCLR) XOSC 0 Ready Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_XOSCRDY0(value)      (OSCCTRL_INTENCLR_XOSCRDY0_Msk & ((value) << OSCCTRL_INTENCLR_XOSCRDY0_Pos))
#define OSCCTRL_INTENCLR_XOSCRDY1_Pos         _U_(1)                                               /**< (OSCCTRL_INTENCLR) XOSC 1 Ready Interrupt Enable Position */
#define OSCCTRL_INTENCLR_XOSCRDY1_Msk         (_U_(0x1) << OSCCTRL_INTENCLR_XOSCRDY1_Pos)          /**< (OSCCTRL_INTENCLR) XOSC 1 Ready Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_XOSCRDY1(value)      (OSCCTRL_INTENCLR_XOSCRDY1_Msk & ((value) << OSCCTRL_INTENCLR_XOSCRDY1_Pos))
#define OSCCTRL_INTENCLR_XOSCFAIL0_Pos        _U_(2)                                               /**< (OSCCTRL_INTENCLR) XOSC 0 Clock Failure Detector Interrupt Enable Position */
#define OSCCTRL_INTENCLR_XOSCFAIL0_Msk        (_U_(0x1) << OSCCTRL_INTENCLR_XOSCFAIL0_Pos)         /**< (OSCCTRL_INTENCLR) XOSC 0 Clock Failure Detector Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_XOSCFAIL0(value)     (OSCCTRL_INTENCLR_XOSCFAIL0_Msk & ((value) << OSCCTRL_INTENCLR_XOSCFAIL0_Pos))
#define OSCCTRL_INTENCLR_XOSCFAIL1_Pos        _U_(3)                                               /**< (OSCCTRL_INTENCLR) XOSC 1 Clock Failure Detector Interrupt Enable Position */
#define OSCCTRL_INTENCLR_XOSCFAIL1_Msk        (_U_(0x1) << OSCCTRL_INTENCLR_XOSCFAIL1_Pos)         /**< (OSCCTRL_INTENCLR) XOSC 1 Clock Failure Detector Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_XOSCFAIL1(value)     (OSCCTRL_INTENCLR_XOSCFAIL1_Msk & ((value) << OSCCTRL_INTENCLR_XOSCFAIL1_Pos))
#define OSCCTRL_INTENCLR_DFLLRDY_Pos          _U_(8)                                               /**< (OSCCTRL_INTENCLR) DFLL Ready Interrupt Enable Position */
#define OSCCTRL_INTENCLR_DFLLRDY_Msk          (_U_(0x1) << OSCCTRL_INTENCLR_DFLLRDY_Pos)           /**< (OSCCTRL_INTENCLR) DFLL Ready Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_DFLLRDY(value)       (OSCCTRL_INTENCLR_DFLLRDY_Msk & ((value) << OSCCTRL_INTENCLR_DFLLRDY_Pos))
#define OSCCTRL_INTENCLR_DFLLOOB_Pos          _U_(9)                                               /**< (OSCCTRL_INTENCLR) DFLL Out Of Bounds Interrupt Enable Position */
#define OSCCTRL_INTENCLR_DFLLOOB_Msk          (_U_(0x1) << OSCCTRL_INTENCLR_DFLLOOB_Pos)           /**< (OSCCTRL_INTENCLR) DFLL Out Of Bounds Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_DFLLOOB(value)       (OSCCTRL_INTENCLR_DFLLOOB_Msk & ((value) << OSCCTRL_INTENCLR_DFLLOOB_Pos))
#define OSCCTRL_INTENCLR_DFLLLCKF_Pos         _U_(10)                                              /**< (OSCCTRL_INTENCLR) DFLL Lock Fine Interrupt Enable Position */
#define OSCCTRL_INTENCLR_DFLLLCKF_Msk         (_U_(0x1) << OSCCTRL_INTENCLR_DFLLLCKF_Pos)          /**< (OSCCTRL_INTENCLR) DFLL Lock Fine Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_DFLLLCKF(value)      (OSCCTRL_INTENCLR_DFLLLCKF_Msk & ((value) << OSCCTRL_INTENCLR_DFLLLCKF_Pos))
#define OSCCTRL_INTENCLR_DFLLLCKC_Pos         _U_(11)                                              /**< (OSCCTRL_INTENCLR) DFLL Lock Coarse Interrupt Enable Position */
#define OSCCTRL_INTENCLR_DFLLLCKC_Msk         (_U_(0x1) << OSCCTRL_INTENCLR_DFLLLCKC_Pos)          /**< (OSCCTRL_INTENCLR) DFLL Lock Coarse Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_DFLLLCKC(value)      (OSCCTRL_INTENCLR_DFLLLCKC_Msk & ((value) << OSCCTRL_INTENCLR_DFLLLCKC_Pos))
#define OSCCTRL_INTENCLR_DFLLRCS_Pos          _U_(12)                                              /**< (OSCCTRL_INTENCLR) DFLL Reference Clock Stopped Interrupt Enable Position */
#define OSCCTRL_INTENCLR_DFLLRCS_Msk          (_U_(0x1) << OSCCTRL_INTENCLR_DFLLRCS_Pos)           /**< (OSCCTRL_INTENCLR) DFLL Reference Clock Stopped Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_DFLLRCS(value)       (OSCCTRL_INTENCLR_DFLLRCS_Msk & ((value) << OSCCTRL_INTENCLR_DFLLRCS_Pos))
#define OSCCTRL_INTENCLR_DPLL0LCKR_Pos        _U_(16)                                              /**< (OSCCTRL_INTENCLR) DPLL0 Lock Rise Interrupt Enable Position */
#define OSCCTRL_INTENCLR_DPLL0LCKR_Msk        (_U_(0x1) << OSCCTRL_INTENCLR_DPLL0LCKR_Pos)         /**< (OSCCTRL_INTENCLR) DPLL0 Lock Rise Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_DPLL0LCKR(value)     (OSCCTRL_INTENCLR_DPLL0LCKR_Msk & ((value) << OSCCTRL_INTENCLR_DPLL0LCKR_Pos))
#define OSCCTRL_INTENCLR_DPLL0LCKF_Pos        _U_(17)                                              /**< (OSCCTRL_INTENCLR) DPLL0 Lock Fall Interrupt Enable Position */
#define OSCCTRL_INTENCLR_DPLL0LCKF_Msk        (_U_(0x1) << OSCCTRL_INTENCLR_DPLL0LCKF_Pos)         /**< (OSCCTRL_INTENCLR) DPLL0 Lock Fall Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_DPLL0LCKF(value)     (OSCCTRL_INTENCLR_DPLL0LCKF_Msk & ((value) << OSCCTRL_INTENCLR_DPLL0LCKF_Pos))
#define OSCCTRL_INTENCLR_DPLL0LTO_Pos         _U_(18)                                              /**< (OSCCTRL_INTENCLR) DPLL0 Lock Timeout Interrupt Enable Position */
#define OSCCTRL_INTENCLR_DPLL0LTO_Msk         (_U_(0x1) << OSCCTRL_INTENCLR_DPLL0LTO_Pos)          /**< (OSCCTRL_INTENCLR) DPLL0 Lock Timeout Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_DPLL0LTO(value)      (OSCCTRL_INTENCLR_DPLL0LTO_Msk & ((value) << OSCCTRL_INTENCLR_DPLL0LTO_Pos))
#define OSCCTRL_INTENCLR_DPLL0LDRTO_Pos       _U_(19)                                              /**< (OSCCTRL_INTENCLR) DPLL0 Loop Divider Ratio Update Complete Interrupt Enable Position */
#define OSCCTRL_INTENCLR_DPLL0LDRTO_Msk       (_U_(0x1) << OSCCTRL_INTENCLR_DPLL0LDRTO_Pos)        /**< (OSCCTRL_INTENCLR) DPLL0 Loop Divider Ratio Update Complete Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_DPLL0LDRTO(value)    (OSCCTRL_INTENCLR_DPLL0LDRTO_Msk & ((value) << OSCCTRL_INTENCLR_DPLL0LDRTO_Pos))
#define OSCCTRL_INTENCLR_DPLL1LCKR_Pos        _U_(24)                                              /**< (OSCCTRL_INTENCLR) DPLL1 Lock Rise Interrupt Enable Position */
#define OSCCTRL_INTENCLR_DPLL1LCKR_Msk        (_U_(0x1) << OSCCTRL_INTENCLR_DPLL1LCKR_Pos)         /**< (OSCCTRL_INTENCLR) DPLL1 Lock Rise Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_DPLL1LCKR(value)     (OSCCTRL_INTENCLR_DPLL1LCKR_Msk & ((value) << OSCCTRL_INTENCLR_DPLL1LCKR_Pos))
#define OSCCTRL_INTENCLR_DPLL1LCKF_Pos        _U_(25)                                              /**< (OSCCTRL_INTENCLR) DPLL1 Lock Fall Interrupt Enable Position */
#define OSCCTRL_INTENCLR_DPLL1LCKF_Msk        (_U_(0x1) << OSCCTRL_INTENCLR_DPLL1LCKF_Pos)         /**< (OSCCTRL_INTENCLR) DPLL1 Lock Fall Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_DPLL1LCKF(value)     (OSCCTRL_INTENCLR_DPLL1LCKF_Msk & ((value) << OSCCTRL_INTENCLR_DPLL1LCKF_Pos))
#define OSCCTRL_INTENCLR_DPLL1LTO_Pos         _U_(26)                                              /**< (OSCCTRL_INTENCLR) DPLL1 Lock Timeout Interrupt Enable Position */
#define OSCCTRL_INTENCLR_DPLL1LTO_Msk         (_U_(0x1) << OSCCTRL_INTENCLR_DPLL1LTO_Pos)          /**< (OSCCTRL_INTENCLR) DPLL1 Lock Timeout Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_DPLL1LTO(value)      (OSCCTRL_INTENCLR_DPLL1LTO_Msk & ((value) << OSCCTRL_INTENCLR_DPLL1LTO_Pos))
#define OSCCTRL_INTENCLR_DPLL1LDRTO_Pos       _U_(27)                                              /**< (OSCCTRL_INTENCLR) DPLL1 Loop Divider Ratio Update Complete Interrupt Enable Position */
#define OSCCTRL_INTENCLR_DPLL1LDRTO_Msk       (_U_(0x1) << OSCCTRL_INTENCLR_DPLL1LDRTO_Pos)        /**< (OSCCTRL_INTENCLR) DPLL1 Loop Divider Ratio Update Complete Interrupt Enable Mask */
#define OSCCTRL_INTENCLR_DPLL1LDRTO(value)    (OSCCTRL_INTENCLR_DPLL1LDRTO_Msk & ((value) << OSCCTRL_INTENCLR_DPLL1LDRTO_Pos))
#define OSCCTRL_INTENCLR_Msk                  _U_(0x0F0F1F0F)                                      /**< (OSCCTRL_INTENCLR) Register Mask  */

#define OSCCTRL_INTENCLR_XOSCRDY_Pos          _U_(0)                                               /**< (OSCCTRL_INTENCLR Position) XOSC x Ready Interrupt Enable */
#define OSCCTRL_INTENCLR_XOSCRDY_Msk          (_U_(0x3) << OSCCTRL_INTENCLR_XOSCRDY_Pos)           /**< (OSCCTRL_INTENCLR Mask) XOSCRDY */
#define OSCCTRL_INTENCLR_XOSCRDY(value)       (OSCCTRL_INTENCLR_XOSCRDY_Msk & ((value) << OSCCTRL_INTENCLR_XOSCRDY_Pos)) 
#define OSCCTRL_INTENCLR_XOSCFAIL_Pos         _U_(2)                                               /**< (OSCCTRL_INTENCLR Position) XOSC x Clock Failure Detector Interrupt Enable */
#define OSCCTRL_INTENCLR_XOSCFAIL_Msk         (_U_(0x3) << OSCCTRL_INTENCLR_XOSCFAIL_Pos)          /**< (OSCCTRL_INTENCLR Mask) XOSCFAIL */
#define OSCCTRL_INTENCLR_XOSCFAIL(value)      (OSCCTRL_INTENCLR_XOSCFAIL_Msk & ((value) << OSCCTRL_INTENCLR_XOSCFAIL_Pos)) 

/* -------- OSCCTRL_INTENSET : (OSCCTRL Offset: 0x08) (R/W 32) Interrupt Enable Set -------- */
#define OSCCTRL_INTENSET_RESETVALUE           _U_(0x00)                                            /**<  (OSCCTRL_INTENSET) Interrupt Enable Set  Reset Value */

#define OSCCTRL_INTENSET_XOSCRDY0_Pos         _U_(0)                                               /**< (OSCCTRL_INTENSET) XOSC 0 Ready Interrupt Enable Position */
#define OSCCTRL_INTENSET_XOSCRDY0_Msk         (_U_(0x1) << OSCCTRL_INTENSET_XOSCRDY0_Pos)          /**< (OSCCTRL_INTENSET) XOSC 0 Ready Interrupt Enable Mask */
#define OSCCTRL_INTENSET_XOSCRDY0(value)      (OSCCTRL_INTENSET_XOSCRDY0_Msk & ((value) << OSCCTRL_INTENSET_XOSCRDY0_Pos))
#define OSCCTRL_INTENSET_XOSCRDY1_Pos         _U_(1)                                               /**< (OSCCTRL_INTENSET) XOSC 1 Ready Interrupt Enable Position */
#define OSCCTRL_INTENSET_XOSCRDY1_Msk         (_U_(0x1) << OSCCTRL_INTENSET_XOSCRDY1_Pos)          /**< (OSCCTRL_INTENSET) XOSC 1 Ready Interrupt Enable Mask */
#define OSCCTRL_INTENSET_XOSCRDY1(value)      (OSCCTRL_INTENSET_XOSCRDY1_Msk & ((value) << OSCCTRL_INTENSET_XOSCRDY1_Pos))
#define OSCCTRL_INTENSET_XOSCFAIL0_Pos        _U_(2)                                               /**< (OSCCTRL_INTENSET) XOSC 0 Clock Failure Detector Interrupt Enable Position */
#define OSCCTRL_INTENSET_XOSCFAIL0_Msk        (_U_(0x1) << OSCCTRL_INTENSET_XOSCFAIL0_Pos)         /**< (OSCCTRL_INTENSET) XOSC 0 Clock Failure Detector Interrupt Enable Mask */
#define OSCCTRL_INTENSET_XOSCFAIL0(value)     (OSCCTRL_INTENSET_XOSCFAIL0_Msk & ((value) << OSCCTRL_INTENSET_XOSCFAIL0_Pos))
#define OSCCTRL_INTENSET_XOSCFAIL1_Pos        _U_(3)                                               /**< (OSCCTRL_INTENSET) XOSC 1 Clock Failure Detector Interrupt Enable Position */
#define OSCCTRL_INTENSET_XOSCFAIL1_Msk        (_U_(0x1) << OSCCTRL_INTENSET_XOSCFAIL1_Pos)         /**< (OSCCTRL_INTENSET) XOSC 1 Clock Failure Detector Interrupt Enable Mask */
#define OSCCTRL_INTENSET_XOSCFAIL1(value)     (OSCCTRL_INTENSET_XOSCFAIL1_Msk & ((value) << OSCCTRL_INTENSET_XOSCFAIL1_Pos))
#define OSCCTRL_INTENSET_DFLLRDY_Pos          _U_(8)                                               /**< (OSCCTRL_INTENSET) DFLL Ready Interrupt Enable Position */
#define OSCCTRL_INTENSET_DFLLRDY_Msk          (_U_(0x1) << OSCCTRL_INTENSET_DFLLRDY_Pos)           /**< (OSCCTRL_INTENSET) DFLL Ready Interrupt Enable Mask */
#define OSCCTRL_INTENSET_DFLLRDY(value)       (OSCCTRL_INTENSET_DFLLRDY_Msk & ((value) << OSCCTRL_INTENSET_DFLLRDY_Pos))
#define OSCCTRL_INTENSET_DFLLOOB_Pos          _U_(9)                                               /**< (OSCCTRL_INTENSET) DFLL Out Of Bounds Interrupt Enable Position */
#define OSCCTRL_INTENSET_DFLLOOB_Msk          (_U_(0x1) << OSCCTRL_INTENSET_DFLLOOB_Pos)           /**< (OSCCTRL_INTENSET) DFLL Out Of Bounds Interrupt Enable Mask */
#define OSCCTRL_INTENSET_DFLLOOB(value)       (OSCCTRL_INTENSET_DFLLOOB_Msk & ((value) << OSCCTRL_INTENSET_DFLLOOB_Pos))
#define OSCCTRL_INTENSET_DFLLLCKF_Pos         _U_(10)                                              /**< (OSCCTRL_INTENSET) DFLL Lock Fine Interrupt Enable Position */
#define OSCCTRL_INTENSET_DFLLLCKF_Msk         (_U_(0x1) << OSCCTRL_INTENSET_DFLLLCKF_Pos)          /**< (OSCCTRL_INTENSET) DFLL Lock Fine Interrupt Enable Mask */
#define OSCCTRL_INTENSET_DFLLLCKF(value)      (OSCCTRL_INTENSET_DFLLLCKF_Msk & ((value) << OSCCTRL_INTENSET_DFLLLCKF_Pos))
#define OSCCTRL_INTENSET_DFLLLCKC_Pos         _U_(11)                                              /**< (OSCCTRL_INTENSET) DFLL Lock Coarse Interrupt Enable Position */
#define OSCCTRL_INTENSET_DFLLLCKC_Msk         (_U_(0x1) << OSCCTRL_INTENSET_DFLLLCKC_Pos)          /**< (OSCCTRL_INTENSET) DFLL Lock Coarse Interrupt Enable Mask */
#define OSCCTRL_INTENSET_DFLLLCKC(value)      (OSCCTRL_INTENSET_DFLLLCKC_Msk & ((value) << OSCCTRL_INTENSET_DFLLLCKC_Pos))
#define OSCCTRL_INTENSET_DFLLRCS_Pos          _U_(12)                                              /**< (OSCCTRL_INTENSET) DFLL Reference Clock Stopped Interrupt Enable Position */
#define OSCCTRL_INTENSET_DFLLRCS_Msk          (_U_(0x1) << OSCCTRL_INTENSET_DFLLRCS_Pos)           /**< (OSCCTRL_INTENSET) DFLL Reference Clock Stopped Interrupt Enable Mask */
#define OSCCTRL_INTENSET_DFLLRCS(value)       (OSCCTRL_INTENSET_DFLLRCS_Msk & ((value) << OSCCTRL_INTENSET_DFLLRCS_Pos))
#define OSCCTRL_INTENSET_DPLL0LCKR_Pos        _U_(16)                                              /**< (OSCCTRL_INTENSET) DPLL0 Lock Rise Interrupt Enable Position */
#define OSCCTRL_INTENSET_DPLL0LCKR_Msk        (_U_(0x1) << OSCCTRL_INTENSET_DPLL0LCKR_Pos)         /**< (OSCCTRL_INTENSET) DPLL0 Lock Rise Interrupt Enable Mask */
#define OSCCTRL_INTENSET_DPLL0LCKR(value)     (OSCCTRL_INTENSET_DPLL0LCKR_Msk & ((value) << OSCCTRL_INTENSET_DPLL0LCKR_Pos))
#define OSCCTRL_INTENSET_DPLL0LCKF_Pos        _U_(17)                                              /**< (OSCCTRL_INTENSET) DPLL0 Lock Fall Interrupt Enable Position */
#define OSCCTRL_INTENSET_DPLL0LCKF_Msk        (_U_(0x1) << OSCCTRL_INTENSET_DPLL0LCKF_Pos)         /**< (OSCCTRL_INTENSET) DPLL0 Lock Fall Interrupt Enable Mask */
#define OSCCTRL_INTENSET_DPLL0LCKF(value)     (OSCCTRL_INTENSET_DPLL0LCKF_Msk & ((value) << OSCCTRL_INTENSET_DPLL0LCKF_Pos))
#define OSCCTRL_INTENSET_DPLL0LTO_Pos         _U_(18)                                              /**< (OSCCTRL_INTENSET) DPLL0 Lock Timeout Interrupt Enable Position */
#define OSCCTRL_INTENSET_DPLL0LTO_Msk         (_U_(0x1) << OSCCTRL_INTENSET_DPLL0LTO_Pos)          /**< (OSCCTRL_INTENSET) DPLL0 Lock Timeout Interrupt Enable Mask */
#define OSCCTRL_INTENSET_DPLL0LTO(value)      (OSCCTRL_INTENSET_DPLL0LTO_Msk & ((value) << OSCCTRL_INTENSET_DPLL0LTO_Pos))
#define OSCCTRL_INTENSET_DPLL0LDRTO_Pos       _U_(19)                                              /**< (OSCCTRL_INTENSET) DPLL0 Loop Divider Ratio Update Complete Interrupt Enable Position */
#define OSCCTRL_INTENSET_DPLL0LDRTO_Msk       (_U_(0x1) << OSCCTRL_INTENSET_DPLL0LDRTO_Pos)        /**< (OSCCTRL_INTENSET) DPLL0 Loop Divider Ratio Update Complete Interrupt Enable Mask */
#define OSCCTRL_INTENSET_DPLL0LDRTO(value)    (OSCCTRL_INTENSET_DPLL0LDRTO_Msk & ((value) << OSCCTRL_INTENSET_DPLL0LDRTO_Pos))
#define OSCCTRL_INTENSET_DPLL1LCKR_Pos        _U_(24)                                              /**< (OSCCTRL_INTENSET) DPLL1 Lock Rise Interrupt Enable Position */
#define OSCCTRL_INTENSET_DPLL1LCKR_Msk        (_U_(0x1) << OSCCTRL_INTENSET_DPLL1LCKR_Pos)         /**< (OSCCTRL_INTENSET) DPLL1 Lock Rise Interrupt Enable Mask */
#define OSCCTRL_INTENSET_DPLL1LCKR(value)     (OSCCTRL_INTENSET_DPLL1LCKR_Msk & ((value) << OSCCTRL_INTENSET_DPLL1LCKR_Pos))
#define OSCCTRL_INTENSET_DPLL1LCKF_Pos        _U_(25)                                              /**< (OSCCTRL_INTENSET) DPLL1 Lock Fall Interrupt Enable Position */
#define OSCCTRL_INTENSET_DPLL1LCKF_Msk        (_U_(0x1) << OSCCTRL_INTENSET_DPLL1LCKF_Pos)         /**< (OSCCTRL_INTENSET) DPLL1 Lock Fall Interrupt Enable Mask */
#define OSCCTRL_INTENSET_DPLL1LCKF(value)     (OSCCTRL_INTENSET_DPLL1LCKF_Msk & ((value) << OSCCTRL_INTENSET_DPLL1LCKF_Pos))
#define OSCCTRL_INTENSET_DPLL1LTO_Pos         _U_(26)                                              /**< (OSCCTRL_INTENSET) DPLL1 Lock Timeout Interrupt Enable Position */
#define OSCCTRL_INTENSET_DPLL1LTO_Msk         (_U_(0x1) << OSCCTRL_INTENSET_DPLL1LTO_Pos)          /**< (OSCCTRL_INTENSET) DPLL1 Lock Timeout Interrupt Enable Mask */
#define OSCCTRL_INTENSET_DPLL1LTO(value)      (OSCCTRL_INTENSET_DPLL1LTO_Msk & ((value) << OSCCTRL_INTENSET_DPLL1LTO_Pos))
#define OSCCTRL_INTENSET_DPLL1LDRTO_Pos       _U_(27)                                              /**< (OSCCTRL_INTENSET) DPLL1 Loop Divider Ratio Update Complete Interrupt Enable Position */
#define OSCCTRL_INTENSET_DPLL1LDRTO_Msk       (_U_(0x1) << OSCCTRL_INTENSET_DPLL1LDRTO_Pos)        /**< (OSCCTRL_INTENSET) DPLL1 Loop Divider Ratio Update Complete Interrupt Enable Mask */
#define OSCCTRL_INTENSET_DPLL1LDRTO(value)    (OSCCTRL_INTENSET_DPLL1LDRTO_Msk & ((value) << OSCCTRL_INTENSET_DPLL1LDRTO_Pos))
#define OSCCTRL_INTENSET_Msk                  _U_(0x0F0F1F0F)                                      /**< (OSCCTRL_INTENSET) Register Mask  */

#define OSCCTRL_INTENSET_XOSCRDY_Pos          _U_(0)                                               /**< (OSCCTRL_INTENSET Position) XOSC x Ready Interrupt Enable */
#define OSCCTRL_INTENSET_XOSCRDY_Msk          (_U_(0x3) << OSCCTRL_INTENSET_XOSCRDY_Pos)           /**< (OSCCTRL_INTENSET Mask) XOSCRDY */
#define OSCCTRL_INTENSET_XOSCRDY(value)       (OSCCTRL_INTENSET_XOSCRDY_Msk & ((value) << OSCCTRL_INTENSET_XOSCRDY_Pos)) 
#define OSCCTRL_INTENSET_XOSCFAIL_Pos         _U_(2)                                               /**< (OSCCTRL_INTENSET Position) XOSC x Clock Failure Detector Interrupt Enable */
#define OSCCTRL_INTENSET_XOSCFAIL_Msk         (_U_(0x3) << OSCCTRL_INTENSET_XOSCFAIL_Pos)          /**< (OSCCTRL_INTENSET Mask) XOSCFAIL */
#define OSCCTRL_INTENSET_XOSCFAIL(value)      (OSCCTRL_INTENSET_XOSCFAIL_Msk & ((value) << OSCCTRL_INTENSET_XOSCFAIL_Pos)) 

/* -------- OSCCTRL_INTFLAG : (OSCCTRL Offset: 0x0C) (R/W 32) Interrupt Flag Status and Clear -------- */
#define OSCCTRL_INTFLAG_RESETVALUE            _U_(0x00)                                            /**<  (OSCCTRL_INTFLAG) Interrupt Flag Status and Clear  Reset Value */

#define OSCCTRL_INTFLAG_XOSCRDY0_Pos          _U_(0)                                               /**< (OSCCTRL_INTFLAG) XOSC 0 Ready Position */
#define OSCCTRL_INTFLAG_XOSCRDY0_Msk          (_U_(0x1) << OSCCTRL_INTFLAG_XOSCRDY0_Pos)           /**< (OSCCTRL_INTFLAG) XOSC 0 Ready Mask */
#define OSCCTRL_INTFLAG_XOSCRDY0(value)       (OSCCTRL_INTFLAG_XOSCRDY0_Msk & ((value) << OSCCTRL_INTFLAG_XOSCRDY0_Pos))
#define OSCCTRL_INTFLAG_XOSCRDY1_Pos          _U_(1)                                               /**< (OSCCTRL_INTFLAG) XOSC 1 Ready Position */
#define OSCCTRL_INTFLAG_XOSCRDY1_Msk          (_U_(0x1) << OSCCTRL_INTFLAG_XOSCRDY1_Pos)           /**< (OSCCTRL_INTFLAG) XOSC 1 Ready Mask */
#define OSCCTRL_INTFLAG_XOSCRDY1(value)       (OSCCTRL_INTFLAG_XOSCRDY1_Msk & ((value) << OSCCTRL_INTFLAG_XOSCRDY1_Pos))
#define OSCCTRL_INTFLAG_XOSCFAIL0_Pos         _U_(2)                                               /**< (OSCCTRL_INTFLAG) XOSC 0 Clock Failure Detector Position */
#define OSCCTRL_INTFLAG_XOSCFAIL0_Msk         (_U_(0x1) << OSCCTRL_INTFLAG_XOSCFAIL0_Pos)          /**< (OSCCTRL_INTFLAG) XOSC 0 Clock Failure Detector Mask */
#define OSCCTRL_INTFLAG_XOSCFAIL0(value)      (OSCCTRL_INTFLAG_XOSCFAIL0_Msk & ((value) << OSCCTRL_INTFLAG_XOSCFAIL0_Pos))
#define OSCCTRL_INTFLAG_XOSCFAIL1_Pos         _U_(3)                                               /**< (OSCCTRL_INTFLAG) XOSC 1 Clock Failure Detector Position */
#define OSCCTRL_INTFLAG_XOSCFAIL1_Msk         (_U_(0x1) << OSCCTRL_INTFLAG_XOSCFAIL1_Pos)          /**< (OSCCTRL_INTFLAG) XOSC 1 Clock Failure Detector Mask */
#define OSCCTRL_INTFLAG_XOSCFAIL1(value)      (OSCCTRL_INTFLAG_XOSCFAIL1_Msk & ((value) << OSCCTRL_INTFLAG_XOSCFAIL1_Pos))
#define OSCCTRL_INTFLAG_DFLLRDY_Pos           _U_(8)                                               /**< (OSCCTRL_INTFLAG) DFLL Ready Position */
#define OSCCTRL_INTFLAG_DFLLRDY_Msk           (_U_(0x1) << OSCCTRL_INTFLAG_DFLLRDY_Pos)            /**< (OSCCTRL_INTFLAG) DFLL Ready Mask */
#define OSCCTRL_INTFLAG_DFLLRDY(value)        (OSCCTRL_INTFLAG_DFLLRDY_Msk & ((value) << OSCCTRL_INTFLAG_DFLLRDY_Pos))
#define OSCCTRL_INTFLAG_DFLLOOB_Pos           _U_(9)                                               /**< (OSCCTRL_INTFLAG) DFLL Out Of Bounds Position */
#define OSCCTRL_INTFLAG_DFLLOOB_Msk           (_U_(0x1) << OSCCTRL_INTFLAG_DFLLOOB_Pos)            /**< (OSCCTRL_INTFLAG) DFLL Out Of Bounds Mask */
#define OSCCTRL_INTFLAG_DFLLOOB(value)        (OSCCTRL_INTFLAG_DFLLOOB_Msk & ((value) << OSCCTRL_INTFLAG_DFLLOOB_Pos))
#define OSCCTRL_INTFLAG_DFLLLCKF_Pos          _U_(10)                                              /**< (OSCCTRL_INTFLAG) DFLL Lock Fine Position */
#define OSCCTRL_INTFLAG_DFLLLCKF_Msk          (_U_(0x1) << OSCCTRL_INTFLAG_DFLLLCKF_Pos)           /**< (OSCCTRL_INTFLAG) DFLL Lock Fine Mask */
#define OSCCTRL_INTFLAG_DFLLLCKF(value)       (OSCCTRL_INTFLAG_DFLLLCKF_Msk & ((value) << OSCCTRL_INTFLAG_DFLLLCKF_Pos))
#define OSCCTRL_INTFLAG_DFLLLCKC_Pos          _U_(11)                                              /**< (OSCCTRL_INTFLAG) DFLL Lock Coarse Position */
#define OSCCTRL_INTFLAG_DFLLLCKC_Msk          (_U_(0x1) << OSCCTRL_INTFLAG_DFLLLCKC_Pos)           /**< (OSCCTRL_INTFLAG) DFLL Lock Coarse Mask */
#define OSCCTRL_INTFLAG_DFLLLCKC(value)       (OSCCTRL_INTFLAG_DFLLLCKC_Msk & ((value) << OSCCTRL_INTFLAG_DFLLLCKC_Pos))
#define OSCCTRL_INTFLAG_DFLLRCS_Pos           _U_(12)                                              /**< (OSCCTRL_INTFLAG) DFLL Reference Clock Stopped Position */
#define OSCCTRL_INTFLAG_DFLLRCS_Msk           (_U_(0x1) << OSCCTRL_INTFLAG_DFLLRCS_Pos)            /**< (OSCCTRL_INTFLAG) DFLL Reference Clock Stopped Mask */
#define OSCCTRL_INTFLAG_DFLLRCS(value)        (OSCCTRL_INTFLAG_DFLLRCS_Msk & ((value) << OSCCTRL_INTFLAG_DFLLRCS_Pos))
#define OSCCTRL_INTFLAG_DPLL0LCKR_Pos         _U_(16)                                              /**< (OSCCTRL_INTFLAG) DPLL0 Lock Rise Position */
#define OSCCTRL_INTFLAG_DPLL0LCKR_Msk         (_U_(0x1) << OSCCTRL_INTFLAG_DPLL0LCKR_Pos)          /**< (OSCCTRL_INTFLAG) DPLL0 Lock Rise Mask */
#define OSCCTRL_INTFLAG_DPLL0LCKR(value)      (OSCCTRL_INTFLAG_DPLL0LCKR_Msk & ((value) << OSCCTRL_INTFLAG_DPLL0LCKR_Pos))
#define OSCCTRL_INTFLAG_DPLL0LCKF_Pos         _U_(17)                                              /**< (OSCCTRL_INTFLAG) DPLL0 Lock Fall Position */
#define OSCCTRL_INTFLAG_DPLL0LCKF_Msk         (_U_(0x1) << OSCCTRL_INTFLAG_DPLL0LCKF_Pos)          /**< (OSCCTRL_INTFLAG) DPLL0 Lock Fall Mask */
#define OSCCTRL_INTFLAG_DPLL0LCKF(value)      (OSCCTRL_INTFLAG_DPLL0LCKF_Msk & ((value) << OSCCTRL_INTFLAG_DPLL0LCKF_Pos))
#define OSCCTRL_INTFLAG_DPLL0LTO_Pos          _U_(18)                                              /**< (OSCCTRL_INTFLAG) DPLL0 Lock Timeout Position */
#define OSCCTRL_INTFLAG_DPLL0LTO_Msk          (_U_(0x1) << OSCCTRL_INTFLAG_DPLL0LTO_Pos)           /**< (OSCCTRL_INTFLAG) DPLL0 Lock Timeout Mask */
#define OSCCTRL_INTFLAG_DPLL0LTO(value)       (OSCCTRL_INTFLAG_DPLL0LTO_Msk & ((value) << OSCCTRL_INTFLAG_DPLL0LTO_Pos))
#define OSCCTRL_INTFLAG_DPLL0LDRTO_Pos        _U_(19)                                              /**< (OSCCTRL_INTFLAG) DPLL0 Loop Divider Ratio Update Complete Position */
#define OSCCTRL_INTFLAG_DPLL0LDRTO_Msk        (_U_(0x1) << OSCCTRL_INTFLAG_DPLL0LDRTO_Pos)         /**< (OSCCTRL_INTFLAG) DPLL0 Loop Divider Ratio Update Complete Mask */
#define OSCCTRL_INTFLAG_DPLL0LDRTO(value)     (OSCCTRL_INTFLAG_DPLL0LDRTO_Msk & ((value) << OSCCTRL_INTFLAG_DPLL0LDRTO_Pos))
#define OSCCTRL_INTFLAG_DPLL1LCKR_Pos         _U_(24)                                              /**< (OSCCTRL_INTFLAG) DPLL1 Lock Rise Position */
#define OSCCTRL_INTFLAG_DPLL1LCKR_Msk         (_U_(0x1) << OSCCTRL_INTFLAG_DPLL1LCKR_Pos)          /**< (OSCCTRL_INTFLAG) DPLL1 Lock Rise Mask */
#define OSCCTRL_INTFLAG_DPLL1LCKR(value)      (OSCCTRL_INTFLAG_DPLL1LCKR_Msk & ((value) << OSCCTRL_INTFLAG_DPLL1LCKR_Pos))
#define OSCCTRL_INTFLAG_DPLL1LCKF_Pos         _U_(25)                                              /**< (OSCCTRL_INTFLAG) DPLL1 Lock Fall Position */
#define OSCCTRL_INTFLAG_DPLL1LCKF_Msk         (_U_(0x1) << OSCCTRL_INTFLAG_DPLL1LCKF_Pos)          /**< (OSCCTRL_INTFLAG) DPLL1 Lock Fall Mask */
#define OSCCTRL_INTFLAG_DPLL1LCKF(value)      (OSCCTRL_INTFLAG_DPLL1LCKF_Msk & ((value) << OSCCTRL_INTFLAG_DPLL1LCKF_Pos))
#define OSCCTRL_INTFLAG_DPLL1LTO_Pos          _U_(26)                                              /**< (OSCCTRL_INTFLAG) DPLL1 Lock Timeout Position */
#define OSCCTRL_INTFLAG_DPLL1LTO_Msk          (_U_(0x1) << OSCCTRL_INTFLAG_DPLL1LTO_Pos)           /**< (OSCCTRL_INTFLAG) DPLL1 Lock Timeout Mask */
#define OSCCTRL_INTFLAG_DPLL1LTO(value)       (OSCCTRL_INTFLAG_DPLL1LTO_Msk & ((value) << OSCCTRL_INTFLAG_DPLL1LTO_Pos))
#define OSCCTRL_INTFLAG_DPLL1LDRTO_Pos        _U_(27)                                              /**< (OSCCTRL_INTFLAG) DPLL1 Loop Divider Ratio Update Complete Position */
#define OSCCTRL_INTFLAG_DPLL1LDRTO_Msk        (_U_(0x1) << OSCCTRL_INTFLAG_DPLL1LDRTO_Pos)         /**< (OSCCTRL_INTFLAG) DPLL1 Loop Divider Ratio Update Complete Mask */
#define OSCCTRL_INTFLAG_DPLL1LDRTO(value)     (OSCCTRL_INTFLAG_DPLL1LDRTO_Msk & ((value) << OSCCTRL_INTFLAG_DPLL1LDRTO_Pos))
#define OSCCTRL_INTFLAG_Msk                   _U_(0x0F0F1F0F)                                      /**< (OSCCTRL_INTFLAG) Register Mask  */

#define OSCCTRL_INTFLAG_XOSCRDY_Pos           _U_(0)                                               /**< (OSCCTRL_INTFLAG Position) XOSC x Ready */
#define OSCCTRL_INTFLAG_XOSCRDY_Msk           (_U_(0x3) << OSCCTRL_INTFLAG_XOSCRDY_Pos)            /**< (OSCCTRL_INTFLAG Mask) XOSCRDY */
#define OSCCTRL_INTFLAG_XOSCRDY(value)        (OSCCTRL_INTFLAG_XOSCRDY_Msk & ((value) << OSCCTRL_INTFLAG_XOSCRDY_Pos)) 
#define OSCCTRL_INTFLAG_XOSCFAIL_Pos          _U_(2)                                               /**< (OSCCTRL_INTFLAG Position) XOSC x Clock Failure Detector */
#define OSCCTRL_INTFLAG_XOSCFAIL_Msk          (_U_(0x3) << OSCCTRL_INTFLAG_XOSCFAIL_Pos)           /**< (OSCCTRL_INTFLAG Mask) XOSCFAIL */
#define OSCCTRL_INTFLAG_XOSCFAIL(value)       (OSCCTRL_INTFLAG_XOSCFAIL_Msk & ((value) << OSCCTRL_INTFLAG_XOSCFAIL_Pos)) 

/* -------- OSCCTRL_STATUS : (OSCCTRL Offset: 0x10) ( R/ 32) Status -------- */
#define OSCCTRL_STATUS_RESETVALUE             _U_(0x00)                                            /**<  (OSCCTRL_STATUS) Status  Reset Value */

#define OSCCTRL_STATUS_XOSCRDY0_Pos           _U_(0)                                               /**< (OSCCTRL_STATUS) XOSC 0 Ready Position */
#define OSCCTRL_STATUS_XOSCRDY0_Msk           (_U_(0x1) << OSCCTRL_STATUS_XOSCRDY0_Pos)            /**< (OSCCTRL_STATUS) XOSC 0 Ready Mask */
#define OSCCTRL_STATUS_XOSCRDY0(value)        (OSCCTRL_STATUS_XOSCRDY0_Msk & ((value) << OSCCTRL_STATUS_XOSCRDY0_Pos))
#define OSCCTRL_STATUS_XOSCRDY1_Pos           _U_(1)                                               /**< (OSCCTRL_STATUS) XOSC 1 Ready Position */
#define OSCCTRL_STATUS_XOSCRDY1_Msk           (_U_(0x1) << OSCCTRL_STATUS_XOSCRDY1_Pos)            /**< (OSCCTRL_STATUS) XOSC 1 Ready Mask */
#define OSCCTRL_STATUS_XOSCRDY1(value)        (OSCCTRL_STATUS_XOSCRDY1_Msk & ((value) << OSCCTRL_STATUS_XOSCRDY1_Pos))
#define OSCCTRL_STATUS_XOSCFAIL0_Pos          _U_(2)                                               /**< (OSCCTRL_STATUS) XOSC 0 Clock Failure Detector Position */
#define OSCCTRL_STATUS_XOSCFAIL0_Msk          (_U_(0x1) << OSCCTRL_STATUS_XOSCFAIL0_Pos)           /**< (OSCCTRL_STATUS) XOSC 0 Clock Failure Detector Mask */
#define OSCCTRL_STATUS_XOSCFAIL0(value)       (OSCCTRL_STATUS_XOSCFAIL0_Msk & ((value) << OSCCTRL_STATUS_XOSCFAIL0_Pos))
#define OSCCTRL_STATUS_XOSCFAIL1_Pos          _U_(3)                                               /**< (OSCCTRL_STATUS) XOSC 1 Clock Failure Detector Position */
#define OSCCTRL_STATUS_XOSCFAIL1_Msk          (_U_(0x1) << OSCCTRL_STATUS_XOSCFAIL1_Pos)           /**< (OSCCTRL_STATUS) XOSC 1 Clock Failure Detector Mask */
#define OSCCTRL_STATUS_XOSCFAIL1(value)       (OSCCTRL_STATUS_XOSCFAIL1_Msk & ((value) << OSCCTRL_STATUS_XOSCFAIL1_Pos))
#define OSCCTRL_STATUS_XOSCCKSW0_Pos          _U_(4)                                               /**< (OSCCTRL_STATUS) XOSC 0 Clock Switch Position */
#define OSCCTRL_STATUS_XOSCCKSW0_Msk          (_U_(0x1) << OSCCTRL_STATUS_XOSCCKSW0_Pos)           /**< (OSCCTRL_STATUS) XOSC 0 Clock Switch Mask */
#define OSCCTRL_STATUS_XOSCCKSW0(value)       (OSCCTRL_STATUS_XOSCCKSW0_Msk & ((value) << OSCCTRL_STATUS_XOSCCKSW0_Pos))
#define OSCCTRL_STATUS_XOSCCKSW1_Pos          _U_(5)                                               /**< (OSCCTRL_STATUS) XOSC 1 Clock Switch Position */
#define OSCCTRL_STATUS_XOSCCKSW1_Msk          (_U_(0x1) << OSCCTRL_STATUS_XOSCCKSW1_Pos)           /**< (OSCCTRL_STATUS) XOSC 1 Clock Switch Mask */
#define OSCCTRL_STATUS_XOSCCKSW1(value)       (OSCCTRL_STATUS_XOSCCKSW1_Msk & ((value) << OSCCTRL_STATUS_XOSCCKSW1_Pos))
#define OSCCTRL_STATUS_DFLLRDY_Pos            _U_(8)                                               /**< (OSCCTRL_STATUS) DFLL Ready Position */
#define OSCCTRL_STATUS_DFLLRDY_Msk            (_U_(0x1) << OSCCTRL_STATUS_DFLLRDY_Pos)             /**< (OSCCTRL_STATUS) DFLL Ready Mask */
#define OSCCTRL_STATUS_DFLLRDY(value)         (OSCCTRL_STATUS_DFLLRDY_Msk & ((value) << OSCCTRL_STATUS_DFLLRDY_Pos))
#define OSCCTRL_STATUS_DFLLOOB_Pos            _U_(9)                                               /**< (OSCCTRL_STATUS) DFLL Out Of Bounds Position */
#define OSCCTRL_STATUS_DFLLOOB_Msk            (_U_(0x1) << OSCCTRL_STATUS_DFLLOOB_Pos)             /**< (OSCCTRL_STATUS) DFLL Out Of Bounds Mask */
#define OSCCTRL_STATUS_DFLLOOB(value)         (OSCCTRL_STATUS_DFLLOOB_Msk & ((value) << OSCCTRL_STATUS_DFLLOOB_Pos))
#define OSCCTRL_STATUS_DFLLLCKF_Pos           _U_(10)                                              /**< (OSCCTRL_STATUS) DFLL Lock Fine Position */
#define OSCCTRL_STATUS_DFLLLCKF_Msk           (_U_(0x1) << OSCCTRL_STATUS_DFLLLCKF_Pos)            /**< (OSCCTRL_STATUS) DFLL Lock Fine Mask */
#define OSCCTRL_STATUS_DFLLLCKF(value)        (OSCCTRL_STATUS_DFLLLCKF_Msk & ((value) << OSCCTRL_STATUS_DFLLLCKF_Pos))
#define OSCCTRL_STATUS_DFLLLCKC_Pos           _U_(11)                                              /**< (OSCCTRL_STATUS) DFLL Lock Coarse Position */
#define OSCCTRL_STATUS_DFLLLCKC_Msk           (_U_(0x1) << OSCCTRL_STATUS_DFLLLCKC_Pos)            /**< (OSCCTRL_STATUS) DFLL Lock Coarse Mask */
#define OSCCTRL_STATUS_DFLLLCKC(value)        (OSCCTRL_STATUS_DFLLLCKC_Msk & ((value) << OSCCTRL_STATUS_DFLLLCKC_Pos))
#define OSCCTRL_STATUS_DFLLRCS_Pos            _U_(12)                                              /**< (OSCCTRL_STATUS) DFLL Reference Clock Stopped Position */
#define OSCCTRL_STATUS_DFLLRCS_Msk            (_U_(0x1) << OSCCTRL_STATUS_DFLLRCS_Pos)             /**< (OSCCTRL_STATUS) DFLL Reference Clock Stopped Mask */
#define OSCCTRL_STATUS_DFLLRCS(value)         (OSCCTRL_STATUS_DFLLRCS_Msk & ((value) << OSCCTRL_STATUS_DFLLRCS_Pos))
#define OSCCTRL_STATUS_DPLL0LCKR_Pos          _U_(16)                                              /**< (OSCCTRL_STATUS) DPLL0 Lock Rise Position */
#define OSCCTRL_STATUS_DPLL0LCKR_Msk          (_U_(0x1) << OSCCTRL_STATUS_DPLL0LCKR_Pos)           /**< (OSCCTRL_STATUS) DPLL0 Lock Rise Mask */
#define OSCCTRL_STATUS_DPLL0LCKR(value)       (OSCCTRL_STATUS_DPLL0LCKR_Msk & ((value) << OSCCTRL_STATUS_DPLL0LCKR_Pos))
#define OSCCTRL_STATUS_DPLL0LCKF_Pos          _U_(17)                                              /**< (OSCCTRL_STATUS) DPLL0 Lock Fall Position */
#define OSCCTRL_STATUS_DPLL0LCKF_Msk          (_U_(0x1) << OSCCTRL_STATUS_DPLL0LCKF_Pos)           /**< (OSCCTRL_STATUS) DPLL0 Lock Fall Mask */
#define OSCCTRL_STATUS_DPLL0LCKF(value)       (OSCCTRL_STATUS_DPLL0LCKF_Msk & ((value) << OSCCTRL_STATUS_DPLL0LCKF_Pos))
#define OSCCTRL_STATUS_DPLL0TO_Pos            _U_(18)                                              /**< (OSCCTRL_STATUS) DPLL0 Timeout Position */
#define OSCCTRL_STATUS_DPLL0TO_Msk            (_U_(0x1) << OSCCTRL_STATUS_DPLL0TO_Pos)             /**< (OSCCTRL_STATUS) DPLL0 Timeout Mask */
#define OSCCTRL_STATUS_DPLL0TO(value)         (OSCCTRL_STATUS_DPLL0TO_Msk & ((value) << OSCCTRL_STATUS_DPLL0TO_Pos))
#define OSCCTRL_STATUS_DPLL0LDRTO_Pos         _U_(19)                                              /**< (OSCCTRL_STATUS) DPLL0 Loop Divider Ratio Update Complete Position */
#define OSCCTRL_STATUS_DPLL0LDRTO_Msk         (_U_(0x1) << OSCCTRL_STATUS_DPLL0LDRTO_Pos)          /**< (OSCCTRL_STATUS) DPLL0 Loop Divider Ratio Update Complete Mask */
#define OSCCTRL_STATUS_DPLL0LDRTO(value)      (OSCCTRL_STATUS_DPLL0LDRTO_Msk & ((value) << OSCCTRL_STATUS_DPLL0LDRTO_Pos))
#define OSCCTRL_STATUS_DPLL1LCKR_Pos          _U_(24)                                              /**< (OSCCTRL_STATUS) DPLL1 Lock Rise Position */
#define OSCCTRL_STATUS_DPLL1LCKR_Msk          (_U_(0x1) << OSCCTRL_STATUS_DPLL1LCKR_Pos)           /**< (OSCCTRL_STATUS) DPLL1 Lock Rise Mask */
#define OSCCTRL_STATUS_DPLL1LCKR(value)       (OSCCTRL_STATUS_DPLL1LCKR_Msk & ((value) << OSCCTRL_STATUS_DPLL1LCKR_Pos))
#define OSCCTRL_STATUS_DPLL1LCKF_Pos          _U_(25)                                              /**< (OSCCTRL_STATUS) DPLL1 Lock Fall Position */
#define OSCCTRL_STATUS_DPLL1LCKF_Msk          (_U_(0x1) << OSCCTRL_STATUS_DPLL1LCKF_Pos)           /**< (OSCCTRL_STATUS) DPLL1 Lock Fall Mask */
#define OSCCTRL_STATUS_DPLL1LCKF(value)       (OSCCTRL_STATUS_DPLL1LCKF_Msk & ((value) << OSCCTRL_STATUS_DPLL1LCKF_Pos))
#define OSCCTRL_STATUS_DPLL1TO_Pos            _U_(26)                                              /**< (OSCCTRL_STATUS) DPLL1 Timeout Position */
#define OSCCTRL_STATUS_DPLL1TO_Msk            (_U_(0x1) << OSCCTRL_STATUS_DPLL1TO_Pos)             /**< (OSCCTRL_STATUS) DPLL1 Timeout Mask */
#define OSCCTRL_STATUS_DPLL1TO(value)         (OSCCTRL_STATUS_DPLL1TO_Msk & ((value) << OSCCTRL_STATUS_DPLL1TO_Pos))
#define OSCCTRL_STATUS_DPLL1LDRTO_Pos         _U_(27)                                              /**< (OSCCTRL_STATUS) DPLL1 Loop Divider Ratio Update Complete Position */
#define OSCCTRL_STATUS_DPLL1LDRTO_Msk         (_U_(0x1) << OSCCTRL_STATUS_DPLL1LDRTO_Pos)          /**< (OSCCTRL_STATUS) DPLL1 Loop Divider Ratio Update Complete Mask */
#define OSCCTRL_STATUS_DPLL1LDRTO(value)      (OSCCTRL_STATUS_DPLL1LDRTO_Msk & ((value) << OSCCTRL_STATUS_DPLL1LDRTO_Pos))
#define OSCCTRL_STATUS_Msk                    _U_(0x0F0F1F3F)                                      /**< (OSCCTRL_STATUS) Register Mask  */

#define OSCCTRL_STATUS_XOSCRDY_Pos            _U_(0)                                               /**< (OSCCTRL_STATUS Position) XOSC x Ready */
#define OSCCTRL_STATUS_XOSCRDY_Msk            (_U_(0x3) << OSCCTRL_STATUS_XOSCRDY_Pos)             /**< (OSCCTRL_STATUS Mask) XOSCRDY */
#define OSCCTRL_STATUS_XOSCRDY(value)         (OSCCTRL_STATUS_XOSCRDY_Msk & ((value) << OSCCTRL_STATUS_XOSCRDY_Pos)) 
#define OSCCTRL_STATUS_XOSCFAIL_Pos           _U_(2)                                               /**< (OSCCTRL_STATUS Position) XOSC x Clock Failure Detector */
#define OSCCTRL_STATUS_XOSCFAIL_Msk           (_U_(0x3) << OSCCTRL_STATUS_XOSCFAIL_Pos)            /**< (OSCCTRL_STATUS Mask) XOSCFAIL */
#define OSCCTRL_STATUS_XOSCFAIL(value)        (OSCCTRL_STATUS_XOSCFAIL_Msk & ((value) << OSCCTRL_STATUS_XOSCFAIL_Pos)) 
#define OSCCTRL_STATUS_XOSCCKSW_Pos           _U_(4)                                               /**< (OSCCTRL_STATUS Position) XOSC x Clock Switch */
#define OSCCTRL_STATUS_XOSCCKSW_Msk           (_U_(0x3) << OSCCTRL_STATUS_XOSCCKSW_Pos)            /**< (OSCCTRL_STATUS Mask) XOSCCKSW */
#define OSCCTRL_STATUS_XOSCCKSW(value)        (OSCCTRL_STATUS_XOSCCKSW_Msk & ((value) << OSCCTRL_STATUS_XOSCCKSW_Pos)) 

/* -------- OSCCTRL_XOSCCTRL : (OSCCTRL Offset: 0x14) (R/W 32) External Multipurpose Crystal Oscillator Control -------- */
#define OSCCTRL_XOSCCTRL_RESETVALUE           _U_(0x80)                                            /**<  (OSCCTRL_XOSCCTRL) External Multipurpose Crystal Oscillator Control  Reset Value */

#define OSCCTRL_XOSCCTRL_ENABLE_Pos           _U_(1)                                               /**< (OSCCTRL_XOSCCTRL) Oscillator Enable Position */
#define OSCCTRL_XOSCCTRL_ENABLE_Msk           (_U_(0x1) << OSCCTRL_XOSCCTRL_ENABLE_Pos)            /**< (OSCCTRL_XOSCCTRL) Oscillator Enable Mask */
#define OSCCTRL_XOSCCTRL_ENABLE(value)        (OSCCTRL_XOSCCTRL_ENABLE_Msk & ((value) << OSCCTRL_XOSCCTRL_ENABLE_Pos))
#define OSCCTRL_XOSCCTRL_XTALEN_Pos           _U_(2)                                               /**< (OSCCTRL_XOSCCTRL) Crystal Oscillator Enable Position */
#define OSCCTRL_XOSCCTRL_XTALEN_Msk           (_U_(0x1) << OSCCTRL_XOSCCTRL_XTALEN_Pos)            /**< (OSCCTRL_XOSCCTRL) Crystal Oscillator Enable Mask */
#define OSCCTRL_XOSCCTRL_XTALEN(value)        (OSCCTRL_XOSCCTRL_XTALEN_Msk & ((value) << OSCCTRL_XOSCCTRL_XTALEN_Pos))
#define OSCCTRL_XOSCCTRL_RUNSTDBY_Pos         _U_(6)                                               /**< (OSCCTRL_XOSCCTRL) Run in Standby Position */
#define OSCCTRL_XOSCCTRL_RUNSTDBY_Msk         (_U_(0x1) << OSCCTRL_XOSCCTRL_RUNSTDBY_Pos)          /**< (OSCCTRL_XOSCCTRL) Run in Standby Mask */
#define OSCCTRL_XOSCCTRL_RUNSTDBY(value)      (OSCCTRL_XOSCCTRL_RUNSTDBY_Msk & ((value) << OSCCTRL_XOSCCTRL_RUNSTDBY_Pos))
#define OSCCTRL_XOSCCTRL_ONDEMAND_Pos         _U_(7)                                               /**< (OSCCTRL_XOSCCTRL) On Demand Control Position */
#define OSCCTRL_XOSCCTRL_ONDEMAND_Msk         (_U_(0x1) << OSCCTRL_XOSCCTRL_ONDEMAND_Pos)          /**< (OSCCTRL_XOSCCTRL) On Demand Control Mask */
#define OSCCTRL_XOSCCTRL_ONDEMAND(value)      (OSCCTRL_XOSCCTRL_ONDEMAND_Msk & ((value) << OSCCTRL_XOSCCTRL_ONDEMAND_Pos))
#define OSCCTRL_XOSCCTRL_LOWBUFGAIN_Pos       _U_(8)                                               /**< (OSCCTRL_XOSCCTRL) Low Buffer Gain Enable Position */
#define OSCCTRL_XOSCCTRL_LOWBUFGAIN_Msk       (_U_(0x1) << OSCCTRL_XOSCCTRL_LOWBUFGAIN_Pos)        /**< (OSCCTRL_XOSCCTRL) Low Buffer Gain Enable Mask */
#define OSCCTRL_XOSCCTRL_LOWBUFGAIN(value)    (OSCCTRL_XOSCCTRL_LOWBUFGAIN_Msk & ((value) << OSCCTRL_XOSCCTRL_LOWBUFGAIN_Pos))
#define OSCCTRL_XOSCCTRL_IPTAT_Pos            _U_(9)                                               /**< (OSCCTRL_XOSCCTRL) Oscillator Current Reference Position */
#define OSCCTRL_XOSCCTRL_IPTAT_Msk            (_U_(0x3) << OSCCTRL_XOSCCTRL_IPTAT_Pos)             /**< (OSCCTRL_XOSCCTRL) Oscillator Current Reference Mask */
#define OSCCTRL_XOSCCTRL_IPTAT(value)         (OSCCTRL_XOSCCTRL_IPTAT_Msk & ((value) << OSCCTRL_XOSCCTRL_IPTAT_Pos))
#define OSCCTRL_XOSCCTRL_IMULT_Pos            _U_(11)                                              /**< (OSCCTRL_XOSCCTRL) Oscillator Current Multiplier Position */
#define OSCCTRL_XOSCCTRL_IMULT_Msk            (_U_(0xF) << OSCCTRL_XOSCCTRL_IMULT_Pos)             /**< (OSCCTRL_XOSCCTRL) Oscillator Current Multiplier Mask */
#define OSCCTRL_XOSCCTRL_IMULT(value)         (OSCCTRL_XOSCCTRL_IMULT_Msk & ((value) << OSCCTRL_XOSCCTRL_IMULT_Pos))
#define OSCCTRL_XOSCCTRL_ENALC_Pos            _U_(15)                                              /**< (OSCCTRL_XOSCCTRL) Automatic Loop Control Enable Position */
#define OSCCTRL_XOSCCTRL_ENALC_Msk            (_U_(0x1) << OSCCTRL_XOSCCTRL_ENALC_Pos)             /**< (OSCCTRL_XOSCCTRL) Automatic Loop Control Enable Mask */
#define OSCCTRL_XOSCCTRL_ENALC(value)         (OSCCTRL_XOSCCTRL_ENALC_Msk & ((value) << OSCCTRL_XOSCCTRL_ENALC_Pos))
#define OSCCTRL_XOSCCTRL_CFDEN_Pos            _U_(16)                                              /**< (OSCCTRL_XOSCCTRL) Clock Failure Detector Enable Position */
#define OSCCTRL_XOSCCTRL_CFDEN_Msk            (_U_(0x1) << OSCCTRL_XOSCCTRL_CFDEN_Pos)             /**< (OSCCTRL_XOSCCTRL) Clock Failure Detector Enable Mask */
#define OSCCTRL_XOSCCTRL_CFDEN(value)         (OSCCTRL_XOSCCTRL_CFDEN_Msk & ((value) << OSCCTRL_XOSCCTRL_CFDEN_Pos))
#define OSCCTRL_XOSCCTRL_SWBEN_Pos            _U_(17)                                              /**< (OSCCTRL_XOSCCTRL) Xosc Clock Switch Enable Position */
#define OSCCTRL_XOSCCTRL_SWBEN_Msk            (_U_(0x1) << OSCCTRL_XOSCCTRL_SWBEN_Pos)             /**< (OSCCTRL_XOSCCTRL) Xosc Clock Switch Enable Mask */
#define OSCCTRL_XOSCCTRL_SWBEN(value)         (OSCCTRL_XOSCCTRL_SWBEN_Msk & ((value) << OSCCTRL_XOSCCTRL_SWBEN_Pos))
#define OSCCTRL_XOSCCTRL_STARTUP_Pos          _U_(20)                                              /**< (OSCCTRL_XOSCCTRL) Start-Up Time Position */
#define OSCCTRL_XOSCCTRL_STARTUP_Msk          (_U_(0xF) << OSCCTRL_XOSCCTRL_STARTUP_Pos)           /**< (OSCCTRL_XOSCCTRL) Start-Up Time Mask */
#define OSCCTRL_XOSCCTRL_STARTUP(value)       (OSCCTRL_XOSCCTRL_STARTUP_Msk & ((value) << OSCCTRL_XOSCCTRL_STARTUP_Pos))
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE1_Val _U_(0x0)                                             /**< (OSCCTRL_XOSCCTRL) 31 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE2_Val _U_(0x1)                                             /**< (OSCCTRL_XOSCCTRL) 61 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE4_Val _U_(0x2)                                             /**< (OSCCTRL_XOSCCTRL) 122 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE8_Val _U_(0x3)                                             /**< (OSCCTRL_XOSCCTRL) 244 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE16_Val _U_(0x4)                                             /**< (OSCCTRL_XOSCCTRL) 488 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE32_Val _U_(0x5)                                             /**< (OSCCTRL_XOSCCTRL) 977 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE64_Val _U_(0x6)                                             /**< (OSCCTRL_XOSCCTRL) 1953 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE128_Val _U_(0x7)                                             /**< (OSCCTRL_XOSCCTRL) 3906 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE256_Val _U_(0x8)                                             /**< (OSCCTRL_XOSCCTRL) 7813 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE512_Val _U_(0x9)                                             /**< (OSCCTRL_XOSCCTRL) 15625 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE1024_Val _U_(0xA)                                             /**< (OSCCTRL_XOSCCTRL) 31250 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE2048_Val _U_(0xB)                                             /**< (OSCCTRL_XOSCCTRL) 62500 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE4096_Val _U_(0xC)                                             /**< (OSCCTRL_XOSCCTRL) 125000 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE8192_Val _U_(0xD)                                             /**< (OSCCTRL_XOSCCTRL) 250000 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE16384_Val _U_(0xE)                                             /**< (OSCCTRL_XOSCCTRL) 500000 us  */
#define   OSCCTRL_XOSCCTRL_STARTUP_CYCLE32768_Val _U_(0xF)                                             /**< (OSCCTRL_XOSCCTRL) 1000000 us  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE1       (OSCCTRL_XOSCCTRL_STARTUP_CYCLE1_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 31 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE2       (OSCCTRL_XOSCCTRL_STARTUP_CYCLE2_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 61 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE4       (OSCCTRL_XOSCCTRL_STARTUP_CYCLE4_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 122 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE8       (OSCCTRL_XOSCCTRL_STARTUP_CYCLE8_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 244 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE16      (OSCCTRL_XOSCCTRL_STARTUP_CYCLE16_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 488 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE32      (OSCCTRL_XOSCCTRL_STARTUP_CYCLE32_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 977 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE64      (OSCCTRL_XOSCCTRL_STARTUP_CYCLE64_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 1953 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE128     (OSCCTRL_XOSCCTRL_STARTUP_CYCLE128_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 3906 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE256     (OSCCTRL_XOSCCTRL_STARTUP_CYCLE256_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 7813 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE512     (OSCCTRL_XOSCCTRL_STARTUP_CYCLE512_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 15625 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE1024    (OSCCTRL_XOSCCTRL_STARTUP_CYCLE1024_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 31250 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE2048    (OSCCTRL_XOSCCTRL_STARTUP_CYCLE2048_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 62500 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE4096    (OSCCTRL_XOSCCTRL_STARTUP_CYCLE4096_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 125000 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE8192    (OSCCTRL_XOSCCTRL_STARTUP_CYCLE8192_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 250000 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE16384   (OSCCTRL_XOSCCTRL_STARTUP_CYCLE16384_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 500000 us Position  */
#define OSCCTRL_XOSCCTRL_STARTUP_CYCLE32768   (OSCCTRL_XOSCCTRL_STARTUP_CYCLE32768_Val << OSCCTRL_XOSCCTRL_STARTUP_Pos) /**< (OSCCTRL_XOSCCTRL) 1000000 us Position  */
#define OSCCTRL_XOSCCTRL_CFDPRESC_Pos         _U_(24)                                              /**< (OSCCTRL_XOSCCTRL) Clock Failure Detector Prescaler Position */
#define OSCCTRL_XOSCCTRL_CFDPRESC_Msk         (_U_(0xF) << OSCCTRL_XOSCCTRL_CFDPRESC_Pos)          /**< (OSCCTRL_XOSCCTRL) Clock Failure Detector Prescaler Mask */
#define OSCCTRL_XOSCCTRL_CFDPRESC(value)      (OSCCTRL_XOSCCTRL_CFDPRESC_Msk & ((value) << OSCCTRL_XOSCCTRL_CFDPRESC_Pos))
#define   OSCCTRL_XOSCCTRL_CFDPRESC_DIV1_Val  _U_(0x0)                                             /**< (OSCCTRL_XOSCCTRL) 48 MHz  */
#define   OSCCTRL_XOSCCTRL_CFDPRESC_DIV2_Val  _U_(0x1)                                             /**< (OSCCTRL_XOSCCTRL) 24 MHz  */
#define   OSCCTRL_XOSCCTRL_CFDPRESC_DIV4_Val  _U_(0x2)                                             /**< (OSCCTRL_XOSCCTRL) 12 MHz  */
#define   OSCCTRL_XOSCCTRL_CFDPRESC_DIV8_Val  _U_(0x3)                                             /**< (OSCCTRL_XOSCCTRL) 6 MHz  */
#define   OSCCTRL_XOSCCTRL_CFDPRESC_DIV16_Val _U_(0x4)                                             /**< (OSCCTRL_XOSCCTRL) 3 MHz  */
#define   OSCCTRL_XOSCCTRL_CFDPRESC_DIV32_Val _U_(0x5)                                             /**< (OSCCTRL_XOSCCTRL) 1.5 MHz  */
#define   OSCCTRL_XOSCCTRL_CFDPRESC_DIV64_Val _U_(0x6)                                             /**< (OSCCTRL_XOSCCTRL) 0.75 MHz  */
#define   OSCCTRL_XOSCCTRL_CFDPRESC_DIV128_Val _U_(0x7)                                             /**< (OSCCTRL_XOSCCTRL) 0.3125 MHz  */
#define OSCCTRL_XOSCCTRL_CFDPRESC_DIV1        (OSCCTRL_XOSCCTRL_CFDPRESC_DIV1_Val << OSCCTRL_XOSCCTRL_CFDPRESC_Pos) /**< (OSCCTRL_XOSCCTRL) 48 MHz Position  */
#define OSCCTRL_XOSCCTRL_CFDPRESC_DIV2        (OSCCTRL_XOSCCTRL_CFDPRESC_DIV2_Val << OSCCTRL_XOSCCTRL_CFDPRESC_Pos) /**< (OSCCTRL_XOSCCTRL) 24 MHz Position  */
#define OSCCTRL_XOSCCTRL_CFDPRESC_DIV4        (OSCCTRL_XOSCCTRL_CFDPRESC_DIV4_Val << OSCCTRL_XOSCCTRL_CFDPRESC_Pos) /**< (OSCCTRL_XOSCCTRL) 12 MHz Position  */
#define OSCCTRL_XOSCCTRL_CFDPRESC_DIV8        (OSCCTRL_XOSCCTRL_CFDPRESC_DIV8_Val << OSCCTRL_XOSCCTRL_CFDPRESC_Pos) /**< (OSCCTRL_XOSCCTRL) 6 MHz Position  */
#define OSCCTRL_XOSCCTRL_CFDPRESC_DIV16       (OSCCTRL_XOSCCTRL_CFDPRESC_DIV16_Val << OSCCTRL_XOSCCTRL_CFDPRESC_Pos) /**< (OSCCTRL_XOSCCTRL) 3 MHz Position  */
#define OSCCTRL_XOSCCTRL_CFDPRESC_DIV32       (OSCCTRL_XOSCCTRL_CFDPRESC_DIV32_Val << OSCCTRL_XOSCCTRL_CFDPRESC_Pos) /**< (OSCCTRL_XOSCCTRL) 1.5 MHz Position  */
#define OSCCTRL_XOSCCTRL_CFDPRESC_DIV64       (OSCCTRL_XOSCCTRL_CFDPRESC_DIV64_Val << OSCCTRL_XOSCCTRL_CFDPRESC_Pos) /**< (OSCCTRL_XOSCCTRL) 0.75 MHz Position  */
#define OSCCTRL_XOSCCTRL_CFDPRESC_DIV128      (OSCCTRL_XOSCCTRL_CFDPRESC_DIV128_Val << OSCCTRL_XOSCCTRL_CFDPRESC_Pos) /**< (OSCCTRL_XOSCCTRL) 0.3125 MHz Position  */
#define OSCCTRL_XOSCCTRL_Msk                  _U_(0x0FF3FFC6)                                      /**< (OSCCTRL_XOSCCTRL) Register Mask  */


/* -------- OSCCTRL_DFLLCTRLA : (OSCCTRL Offset: 0x1C) (R/W 8) DFLL48M Control A -------- */
#define OSCCTRL_DFLLCTRLA_RESETVALUE          _U_(0x82)                                            /**<  (OSCCTRL_DFLLCTRLA) DFLL48M Control A  Reset Value */

#define OSCCTRL_DFLLCTRLA_ENABLE_Pos          _U_(1)                                               /**< (OSCCTRL_DFLLCTRLA) DFLL Enable Position */
#define OSCCTRL_DFLLCTRLA_ENABLE_Msk          (_U_(0x1) << OSCCTRL_DFLLCTRLA_ENABLE_Pos)           /**< (OSCCTRL_DFLLCTRLA) DFLL Enable Mask */
#define OSCCTRL_DFLLCTRLA_ENABLE(value)       (OSCCTRL_DFLLCTRLA_ENABLE_Msk & ((value) << OSCCTRL_DFLLCTRLA_ENABLE_Pos))
#define OSCCTRL_DFLLCTRLA_RUNSTDBY_Pos        _U_(6)                                               /**< (OSCCTRL_DFLLCTRLA) Run in Standby Position */
#define OSCCTRL_DFLLCTRLA_RUNSTDBY_Msk        (_U_(0x1) << OSCCTRL_DFLLCTRLA_RUNSTDBY_Pos)         /**< (OSCCTRL_DFLLCTRLA) Run in Standby Mask */
#define OSCCTRL_DFLLCTRLA_RUNSTDBY(value)     (OSCCTRL_DFLLCTRLA_RUNSTDBY_Msk & ((value) << OSCCTRL_DFLLCTRLA_RUNSTDBY_Pos))
#define OSCCTRL_DFLLCTRLA_ONDEMAND_Pos        _U_(7)                                               /**< (OSCCTRL_DFLLCTRLA) On Demand Control Position */
#define OSCCTRL_DFLLCTRLA_ONDEMAND_Msk        (_U_(0x1) << OSCCTRL_DFLLCTRLA_ONDEMAND_Pos)         /**< (OSCCTRL_DFLLCTRLA) On Demand Control Mask */
#define OSCCTRL_DFLLCTRLA_ONDEMAND(value)     (OSCCTRL_DFLLCTRLA_ONDEMAND_Msk & ((value) << OSCCTRL_DFLLCTRLA_ONDEMAND_Pos))
#define OSCCTRL_DFLLCTRLA_Msk                 _U_(0xC2)                                            /**< (OSCCTRL_DFLLCTRLA) Register Mask  */


/* -------- OSCCTRL_DFLLCTRLB : (OSCCTRL Offset: 0x20) (R/W 8) DFLL48M Control B -------- */
#define OSCCTRL_DFLLCTRLB_RESETVALUE          _U_(0x00)                                            /**<  (OSCCTRL_DFLLCTRLB) DFLL48M Control B  Reset Value */

#define OSCCTRL_DFLLCTRLB_MODE_Pos            _U_(0)                                               /**< (OSCCTRL_DFLLCTRLB) Operating Mode Selection Position */
#define OSCCTRL_DFLLCTRLB_MODE_Msk            (_U_(0x1) << OSCCTRL_DFLLCTRLB_MODE_Pos)             /**< (OSCCTRL_DFLLCTRLB) Operating Mode Selection Mask */
#define OSCCTRL_DFLLCTRLB_MODE(value)         (OSCCTRL_DFLLCTRLB_MODE_Msk & ((value) << OSCCTRL_DFLLCTRLB_MODE_Pos))
#define OSCCTRL_DFLLCTRLB_STABLE_Pos          _U_(1)                                               /**< (OSCCTRL_DFLLCTRLB) Stable DFLL Frequency Position */
#define OSCCTRL_DFLLCTRLB_STABLE_Msk          (_U_(0x1) << OSCCTRL_DFLLCTRLB_STABLE_Pos)           /**< (OSCCTRL_DFLLCTRLB) Stable DFLL Frequency Mask */
#define OSCCTRL_DFLLCTRLB_STABLE(value)       (OSCCTRL_DFLLCTRLB_STABLE_Msk & ((value) << OSCCTRL_DFLLCTRLB_STABLE_Pos))
#define OSCCTRL_DFLLCTRLB_LLAW_Pos            _U_(2)                                               /**< (OSCCTRL_DFLLCTRLB) Lose Lock After Wake Position */
#define OSCCTRL_DFLLCTRLB_LLAW_Msk            (_U_(0x1) << OSCCTRL_DFLLCTRLB_LLAW_Pos)             /**< (OSCCTRL_DFLLCTRLB) Lose Lock After Wake Mask */
#define OSCCTRL_DFLLCTRLB_LLAW(value)         (OSCCTRL_DFLLCTRLB_LLAW_Msk & ((value) << OSCCTRL_DFLLCTRLB_LLAW_Pos))
#define OSCCTRL_DFLLCTRLB_USBCRM_Pos          _U_(3)                                               /**< (OSCCTRL_DFLLCTRLB) USB Clock Recovery Mode Position */
#define OSCCTRL_DFLLCTRLB_USBCRM_Msk          (_U_(0x1) << OSCCTRL_DFLLCTRLB_USBCRM_Pos)           /**< (OSCCTRL_DFLLCTRLB) USB Clock Recovery Mode Mask */
#define OSCCTRL_DFLLCTRLB_USBCRM(value)       (OSCCTRL_DFLLCTRLB_USBCRM_Msk & ((value) << OSCCTRL_DFLLCTRLB_USBCRM_Pos))
#define OSCCTRL_DFLLCTRLB_CCDIS_Pos           _U_(4)                                               /**< (OSCCTRL_DFLLCTRLB) Chill Cycle Disable Position */
#define OSCCTRL_DFLLCTRLB_CCDIS_Msk           (_U_(0x1) << OSCCTRL_DFLLCTRLB_CCDIS_Pos)            /**< (OSCCTRL_DFLLCTRLB) Chill Cycle Disable Mask */
#define OSCCTRL_DFLLCTRLB_CCDIS(value)        (OSCCTRL_DFLLCTRLB_CCDIS_Msk & ((value) << OSCCTRL_DFLLCTRLB_CCDIS_Pos))
#define OSCCTRL_DFLLCTRLB_QLDIS_Pos           _U_(5)                                               /**< (OSCCTRL_DFLLCTRLB) Quick Lock Disable Position */
#define OSCCTRL_DFLLCTRLB_QLDIS_Msk           (_U_(0x1) << OSCCTRL_DFLLCTRLB_QLDIS_Pos)            /**< (OSCCTRL_DFLLCTRLB) Quick Lock Disable Mask */
#define OSCCTRL_DFLLCTRLB_QLDIS(value)        (OSCCTRL_DFLLCTRLB_QLDIS_Msk & ((value) << OSCCTRL_DFLLCTRLB_QLDIS_Pos))
#define OSCCTRL_DFLLCTRLB_BPLCKC_Pos          _U_(6)                                               /**< (OSCCTRL_DFLLCTRLB) Bypass Coarse Lock Position */
#define OSCCTRL_DFLLCTRLB_BPLCKC_Msk          (_U_(0x1) << OSCCTRL_DFLLCTRLB_BPLCKC_Pos)           /**< (OSCCTRL_DFLLCTRLB) Bypass Coarse Lock Mask */
#define OSCCTRL_DFLLCTRLB_BPLCKC(value)       (OSCCTRL_DFLLCTRLB_BPLCKC_Msk & ((value) << OSCCTRL_DFLLCTRLB_BPLCKC_Pos))
#define OSCCTRL_DFLLCTRLB_WAITLOCK_Pos        _U_(7)                                               /**< (OSCCTRL_DFLLCTRLB) Wait Lock Position */
#define OSCCTRL_DFLLCTRLB_WAITLOCK_Msk        (_U_(0x1) << OSCCTRL_DFLLCTRLB_WAITLOCK_Pos)         /**< (OSCCTRL_DFLLCTRLB) Wait Lock Mask */
#define OSCCTRL_DFLLCTRLB_WAITLOCK(value)     (OSCCTRL_DFLLCTRLB_WAITLOCK_Msk & ((value) << OSCCTRL_DFLLCTRLB_WAITLOCK_Pos))
#define OSCCTRL_DFLLCTRLB_Msk                 _U_(0xFF)                                            /**< (OSCCTRL_DFLLCTRLB) Register Mask  */


/* -------- OSCCTRL_DFLLVAL : (OSCCTRL Offset: 0x24) (R/W 32) DFLL48M Value -------- */
#define OSCCTRL_DFLLVAL_RESETVALUE            _U_(0x00)                                            /**<  (OSCCTRL_DFLLVAL) DFLL48M Value  Reset Value */

#define OSCCTRL_DFLLVAL_FINE_Pos              _U_(0)                                               /**< (OSCCTRL_DFLLVAL) Fine Value Position */
#define OSCCTRL_DFLLVAL_FINE_Msk              (_U_(0xFF) << OSCCTRL_DFLLVAL_FINE_Pos)              /**< (OSCCTRL_DFLLVAL) Fine Value Mask */
#define OSCCTRL_DFLLVAL_FINE(value)           (OSCCTRL_DFLLVAL_FINE_Msk & ((value) << OSCCTRL_DFLLVAL_FINE_Pos))
#define OSCCTRL_DFLLVAL_COARSE_Pos            _U_(10)                                              /**< (OSCCTRL_DFLLVAL) Coarse Value Position */
#define OSCCTRL_DFLLVAL_COARSE_Msk            (_U_(0x3F) << OSCCTRL_DFLLVAL_COARSE_Pos)            /**< (OSCCTRL_DFLLVAL) Coarse Value Mask */
#define OSCCTRL_DFLLVAL_COARSE(value)         (OSCCTRL_DFLLVAL_COARSE_Msk & ((value) << OSCCTRL_DFLLVAL_COARSE_Pos))
#define OSCCTRL_DFLLVAL_DIFF_Pos              _U_(16)                                              /**< (OSCCTRL_DFLLVAL) Multiplication Ratio Difference Position */
#define OSCCTRL_DFLLVAL_DIFF_Msk              (_U_(0xFFFF) << OSCCTRL_DFLLVAL_DIFF_Pos)            /**< (OSCCTRL_DFLLVAL) Multiplication Ratio Difference Mask */
#define OSCCTRL_DFLLVAL_DIFF(value)           (OSCCTRL_DFLLVAL_DIFF_Msk & ((value) << OSCCTRL_DFLLVAL_DIFF_Pos))
#define OSCCTRL_DFLLVAL_Msk                   _U_(0xFFFFFCFF)                                      /**< (OSCCTRL_DFLLVAL) Register Mask  */


/* -------- OSCCTRL_DFLLMUL : (OSCCTRL Offset: 0x28) (R/W 32) DFLL48M Multiplier -------- */
#define OSCCTRL_DFLLMUL_RESETVALUE            _U_(0x00)                                            /**<  (OSCCTRL_DFLLMUL) DFLL48M Multiplier  Reset Value */

#define OSCCTRL_DFLLMUL_MUL_Pos               _U_(0)                                               /**< (OSCCTRL_DFLLMUL) DFLL Multiply Factor Position */
#define OSCCTRL_DFLLMUL_MUL_Msk               (_U_(0xFFFF) << OSCCTRL_DFLLMUL_MUL_Pos)             /**< (OSCCTRL_DFLLMUL) DFLL Multiply Factor Mask */
#define OSCCTRL_DFLLMUL_MUL(value)            (OSCCTRL_DFLLMUL_MUL_Msk & ((value) << OSCCTRL_DFLLMUL_MUL_Pos))
#define OSCCTRL_DFLLMUL_FSTEP_Pos             _U_(16)                                              /**< (OSCCTRL_DFLLMUL) Fine Maximum Step Position */
#define OSCCTRL_DFLLMUL_FSTEP_Msk             (_U_(0xFF) << OSCCTRL_DFLLMUL_FSTEP_Pos)             /**< (OSCCTRL_DFLLMUL) Fine Maximum Step Mask */
#define OSCCTRL_DFLLMUL_FSTEP(value)          (OSCCTRL_DFLLMUL_FSTEP_Msk & ((value) << OSCCTRL_DFLLMUL_FSTEP_Pos))
#define OSCCTRL_DFLLMUL_CSTEP_Pos             _U_(26)                                              /**< (OSCCTRL_DFLLMUL) Coarse Maximum Step Position */
#define OSCCTRL_DFLLMUL_CSTEP_Msk             (_U_(0x3F) << OSCCTRL_DFLLMUL_CSTEP_Pos)             /**< (OSCCTRL_DFLLMUL) Coarse Maximum Step Mask */
#define OSCCTRL_DFLLMUL_CSTEP(value)          (OSCCTRL_DFLLMUL_CSTEP_Msk & ((value) << OSCCTRL_DFLLMUL_CSTEP_Pos))
#define OSCCTRL_DFLLMUL_Msk                   _U_(0xFCFFFFFF)                                      /**< (OSCCTRL_DFLLMUL) Register Mask  */


/* -------- OSCCTRL_DFLLSYNC : (OSCCTRL Offset: 0x2C) (R/W 8) DFLL48M Synchronization -------- */
#define OSCCTRL_DFLLSYNC_RESETVALUE           _U_(0x00)                                            /**<  (OSCCTRL_DFLLSYNC) DFLL48M Synchronization  Reset Value */

#define OSCCTRL_DFLLSYNC_ENABLE_Pos           _U_(1)                                               /**< (OSCCTRL_DFLLSYNC) ENABLE Synchronization Busy Position */
#define OSCCTRL_DFLLSYNC_ENABLE_Msk           (_U_(0x1) << OSCCTRL_DFLLSYNC_ENABLE_Pos)            /**< (OSCCTRL_DFLLSYNC) ENABLE Synchronization Busy Mask */
#define OSCCTRL_DFLLSYNC_ENABLE(value)        (OSCCTRL_DFLLSYNC_ENABLE_Msk & ((value) << OSCCTRL_DFLLSYNC_ENABLE_Pos))
#define OSCCTRL_DFLLSYNC_DFLLCTRLB_Pos        _U_(2)                                               /**< (OSCCTRL_DFLLSYNC) DFLLCTRLB Synchronization Busy Position */
#define OSCCTRL_DFLLSYNC_DFLLCTRLB_Msk        (_U_(0x1) << OSCCTRL_DFLLSYNC_DFLLCTRLB_Pos)         /**< (OSCCTRL_DFLLSYNC) DFLLCTRLB Synchronization Busy Mask */
#define OSCCTRL_DFLLSYNC_DFLLCTRLB(value)     (OSCCTRL_DFLLSYNC_DFLLCTRLB_Msk & ((value) << OSCCTRL_DFLLSYNC_DFLLCTRLB_Pos))
#define OSCCTRL_DFLLSYNC_DFLLVAL_Pos          _U_(3)                                               /**< (OSCCTRL_DFLLSYNC) DFLLVAL Synchronization Busy Position */
#define OSCCTRL_DFLLSYNC_DFLLVAL_Msk          (_U_(0x1) << OSCCTRL_DFLLSYNC_DFLLVAL_Pos)           /**< (OSCCTRL_DFLLSYNC) DFLLVAL Synchronization Busy Mask */
#define OSCCTRL_DFLLSYNC_DFLLVAL(value)       (OSCCTRL_DFLLSYNC_DFLLVAL_Msk & ((value) << OSCCTRL_DFLLSYNC_DFLLVAL_Pos))
#define OSCCTRL_DFLLSYNC_DFLLMUL_Pos          _U_(4)                                               /**< (OSCCTRL_DFLLSYNC) DFLLMUL Synchronization Busy Position */
#define OSCCTRL_DFLLSYNC_DFLLMUL_Msk          (_U_(0x1) << OSCCTRL_DFLLSYNC_DFLLMUL_Pos)           /**< (OSCCTRL_DFLLSYNC) DFLLMUL Synchronization Busy Mask */
#define OSCCTRL_DFLLSYNC_DFLLMUL(value)       (OSCCTRL_DFLLSYNC_DFLLMUL_Msk & ((value) << OSCCTRL_DFLLSYNC_DFLLMUL_Pos))
#define OSCCTRL_DFLLSYNC_Msk                  _U_(0x1E)                                            /**< (OSCCTRL_DFLLSYNC) Register Mask  */


/** \brief OSCCTRL register offsets definitions */
#define OSCCTRL_DPLLCTRLA_REG_OFST     (0x00)              /**< (OSCCTRL_DPLLCTRLA) DPLL Control A Offset */
#define OSCCTRL_DPLLRATIO_REG_OFST     (0x04)              /**< (OSCCTRL_DPLLRATIO) DPLL Ratio Control Offset */
#define OSCCTRL_DPLLCTRLB_REG_OFST     (0x08)              /**< (OSCCTRL_DPLLCTRLB) DPLL Control B Offset */
#define OSCCTRL_DPLLSYNCBUSY_REG_OFST  (0x0C)              /**< (OSCCTRL_DPLLSYNCBUSY) DPLL Synchronization Busy Offset */
#define OSCCTRL_DPLLSTATUS_REG_OFST    (0x10)              /**< (OSCCTRL_DPLLSTATUS) DPLL Status Offset */
#define OSCCTRL_EVCTRL_REG_OFST        (0x00)              /**< (OSCCTRL_EVCTRL) Event Control Offset */
#define OSCCTRL_INTENCLR_REG_OFST      (0x04)              /**< (OSCCTRL_INTENCLR) Interrupt Enable Clear Offset */
#define OSCCTRL_INTENSET_REG_OFST      (0x08)              /**< (OSCCTRL_INTENSET) Interrupt Enable Set Offset */
#define OSCCTRL_INTFLAG_REG_OFST       (0x0C)              /**< (OSCCTRL_INTFLAG) Interrupt Flag Status and Clear Offset */
#define OSCCTRL_STATUS_REG_OFST        (0x10)              /**< (OSCCTRL_STATUS) Status Offset */
#define OSCCTRL_XOSCCTRL_REG_OFST      (0x14)              /**< (OSCCTRL_XOSCCTRL) External Multipurpose Crystal Oscillator Control Offset */
#define OSCCTRL_DFLLCTRLA_REG_OFST     (0x1C)              /**< (OSCCTRL_DFLLCTRLA) DFLL48M Control A Offset */
#define OSCCTRL_DFLLCTRLB_REG_OFST     (0x20)              /**< (OSCCTRL_DFLLCTRLB) DFLL48M Control B Offset */
#define OSCCTRL_DFLLVAL_REG_OFST       (0x24)              /**< (OSCCTRL_DFLLVAL) DFLL48M Value Offset */
#define OSCCTRL_DFLLMUL_REG_OFST       (0x28)              /**< (OSCCTRL_DFLLMUL) DFLL48M Multiplier Offset */
#define OSCCTRL_DFLLSYNC_REG_OFST      (0x2C)              /**< (OSCCTRL_DFLLSYNC) DFLL48M Synchronization Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief DPLL register API structure */
typedef struct
{
  __IO  uint8_t                        OSCCTRL_DPLLCTRLA;  /**< Offset: 0x00 (R/W  8) DPLL Control A */
  __I   uint8_t                        Reserved1[0x03];
  __IO  uint32_t                       OSCCTRL_DPLLRATIO;  /**< Offset: 0x04 (R/W  32) DPLL Ratio Control */
  __IO  uint32_t                       OSCCTRL_DPLLCTRLB;  /**< Offset: 0x08 (R/W  32) DPLL Control B */
  __I   uint32_t                       OSCCTRL_DPLLSYNCBUSY; /**< Offset: 0x0C (R/   32) DPLL Synchronization Busy */
  __I   uint32_t                       OSCCTRL_DPLLSTATUS; /**< Offset: 0x10 (R/   32) DPLL Status */
} oscctrl_dpll_registers_t;

#define DPLL_NUMBER _U_(2)

/** \brief OSCCTRL register API structure */
typedef struct
{  /* Oscillators Control */
  __IO  uint8_t                        OSCCTRL_EVCTRL;     /**< Offset: 0x00 (R/W  8) Event Control */
  __I   uint8_t                        Reserved1[0x03];
  __IO  uint32_t                       OSCCTRL_INTENCLR;   /**< Offset: 0x04 (R/W  32) Interrupt Enable Clear */
  __IO  uint32_t                       OSCCTRL_INTENSET;   /**< Offset: 0x08 (R/W  32) Interrupt Enable Set */
  __IO  uint32_t                       OSCCTRL_INTFLAG;    /**< Offset: 0x0C (R/W  32) Interrupt Flag Status and Clear */
  __I   uint32_t                       OSCCTRL_STATUS;     /**< Offset: 0x10 (R/   32) Status */
  __IO  uint32_t                       OSCCTRL_XOSCCTRL[2]; /**< Offset: 0x14 (R/W  32) External Multipurpose Crystal Oscillator Control */
  __IO  uint8_t                        OSCCTRL_DFLLCTRLA;  /**< Offset: 0x1C (R/W  8) DFLL48M Control A */
  __I   uint8_t                        Reserved2[0x03];
  __IO  uint8_t                        OSCCTRL_DFLLCTRLB;  /**< Offset: 0x20 (R/W  8) DFLL48M Control B */
  __I   uint8_t                        Reserved3[0x03];
  __IO  uint32_t                       OSCCTRL_DFLLVAL;    /**< Offset: 0x24 (R/W  32) DFLL48M Value */
  __IO  uint32_t                       OSCCTRL_DFLLMUL;    /**< Offset: 0x28 (R/W  32) DFLL48M Multiplier */
  __IO  uint8_t                        OSCCTRL_DFLLSYNC;   /**< Offset: 0x2C (R/W  8) DFLL48M Synchronization */
  __I   uint8_t                        Reserved4[0x03];
        oscctrl_dpll_registers_t       DPLL[DPLL_NUMBER]; /**< Offset: 0x30  */
} oscctrl_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_OSCCTRL_COMPONENT_H_ */
