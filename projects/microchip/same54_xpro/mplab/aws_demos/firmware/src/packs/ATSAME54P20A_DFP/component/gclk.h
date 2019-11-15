/**
 * \brief Component description for GCLK
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
#ifndef _SAME54_GCLK_COMPONENT_H_
#define _SAME54_GCLK_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR GCLK                                         */
/* ************************************************************************** */

/* -------- GCLK_CTRLA : (GCLK Offset: 0x00) (R/W 8) Control -------- */
#define GCLK_CTRLA_RESETVALUE                 _U_(0x00)                                            /**<  (GCLK_CTRLA) Control  Reset Value */

#define GCLK_CTRLA_SWRST_Pos                  _U_(0)                                               /**< (GCLK_CTRLA) Software Reset Position */
#define GCLK_CTRLA_SWRST_Msk                  (_U_(0x1) << GCLK_CTRLA_SWRST_Pos)                   /**< (GCLK_CTRLA) Software Reset Mask */
#define GCLK_CTRLA_SWRST(value)               (GCLK_CTRLA_SWRST_Msk & ((value) << GCLK_CTRLA_SWRST_Pos))
#define GCLK_CTRLA_Msk                        _U_(0x01)                                            /**< (GCLK_CTRLA) Register Mask  */


/* -------- GCLK_SYNCBUSY : (GCLK Offset: 0x04) ( R/ 32) Synchronization Busy -------- */
#define GCLK_SYNCBUSY_RESETVALUE              _U_(0x00)                                            /**<  (GCLK_SYNCBUSY) Synchronization Busy  Reset Value */

#define GCLK_SYNCBUSY_SWRST_Pos               _U_(0)                                               /**< (GCLK_SYNCBUSY) Software Reset Synchroniation Busy bit Position */
#define GCLK_SYNCBUSY_SWRST_Msk               (_U_(0x1) << GCLK_SYNCBUSY_SWRST_Pos)                /**< (GCLK_SYNCBUSY) Software Reset Synchroniation Busy bit Mask */
#define GCLK_SYNCBUSY_SWRST(value)            (GCLK_SYNCBUSY_SWRST_Msk & ((value) << GCLK_SYNCBUSY_SWRST_Pos))
#define GCLK_SYNCBUSY_GENCTRL_Pos             _U_(2)                                               /**< (GCLK_SYNCBUSY) Generic Clock Generator Control n Synchronization Busy bits Position */
#define GCLK_SYNCBUSY_GENCTRL_Msk             (_U_(0xFFF) << GCLK_SYNCBUSY_GENCTRL_Pos)            /**< (GCLK_SYNCBUSY) Generic Clock Generator Control n Synchronization Busy bits Mask */
#define GCLK_SYNCBUSY_GENCTRL(value)          (GCLK_SYNCBUSY_GENCTRL_Msk & ((value) << GCLK_SYNCBUSY_GENCTRL_Pos))
#define   GCLK_SYNCBUSY_GENCTRL_GCLK0_Val     _U_(0x1)                                             /**< (GCLK_SYNCBUSY) Generic clock generator 0  */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK1_Val     _U_(0x2)                                             /**< (GCLK_SYNCBUSY) Generic clock generator 1  */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK2_Val     _U_(0x4)                                             /**< (GCLK_SYNCBUSY) Generic clock generator 2  */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK3_Val     _U_(0x8)                                             /**< (GCLK_SYNCBUSY) Generic clock generator 3  */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK4_Val     _U_(0x10)                                            /**< (GCLK_SYNCBUSY) Generic clock generator 4  */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK5_Val     _U_(0x20)                                            /**< (GCLK_SYNCBUSY) Generic clock generator 5  */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK6_Val     _U_(0x40)                                            /**< (GCLK_SYNCBUSY) Generic clock generator 6  */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK7_Val     _U_(0x80)                                            /**< (GCLK_SYNCBUSY) Generic clock generator 7  */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK8_Val     _U_(0x100)                                           /**< (GCLK_SYNCBUSY) Generic clock generator 8  */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK9_Val     _U_(0x200)                                           /**< (GCLK_SYNCBUSY) Generic clock generator 9  */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK10_Val    _U_(0x400)                                           /**< (GCLK_SYNCBUSY) Generic clock generator 10  */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK11_Val    _U_(0x800)                                           /**< (GCLK_SYNCBUSY) Generic clock generator 11  */
#define GCLK_SYNCBUSY_GENCTRL_GCLK0           (GCLK_SYNCBUSY_GENCTRL_GCLK0_Val << GCLK_SYNCBUSY_GENCTRL_Pos) /**< (GCLK_SYNCBUSY) Generic clock generator 0 Position  */
#define GCLK_SYNCBUSY_GENCTRL_GCLK1           (GCLK_SYNCBUSY_GENCTRL_GCLK1_Val << GCLK_SYNCBUSY_GENCTRL_Pos) /**< (GCLK_SYNCBUSY) Generic clock generator 1 Position  */
#define GCLK_SYNCBUSY_GENCTRL_GCLK2           (GCLK_SYNCBUSY_GENCTRL_GCLK2_Val << GCLK_SYNCBUSY_GENCTRL_Pos) /**< (GCLK_SYNCBUSY) Generic clock generator 2 Position  */
#define GCLK_SYNCBUSY_GENCTRL_GCLK3           (GCLK_SYNCBUSY_GENCTRL_GCLK3_Val << GCLK_SYNCBUSY_GENCTRL_Pos) /**< (GCLK_SYNCBUSY) Generic clock generator 3 Position  */
#define GCLK_SYNCBUSY_GENCTRL_GCLK4           (GCLK_SYNCBUSY_GENCTRL_GCLK4_Val << GCLK_SYNCBUSY_GENCTRL_Pos) /**< (GCLK_SYNCBUSY) Generic clock generator 4 Position  */
#define GCLK_SYNCBUSY_GENCTRL_GCLK5           (GCLK_SYNCBUSY_GENCTRL_GCLK5_Val << GCLK_SYNCBUSY_GENCTRL_Pos) /**< (GCLK_SYNCBUSY) Generic clock generator 5 Position  */
#define GCLK_SYNCBUSY_GENCTRL_GCLK6           (GCLK_SYNCBUSY_GENCTRL_GCLK6_Val << GCLK_SYNCBUSY_GENCTRL_Pos) /**< (GCLK_SYNCBUSY) Generic clock generator 6 Position  */
#define GCLK_SYNCBUSY_GENCTRL_GCLK7           (GCLK_SYNCBUSY_GENCTRL_GCLK7_Val << GCLK_SYNCBUSY_GENCTRL_Pos) /**< (GCLK_SYNCBUSY) Generic clock generator 7 Position  */
#define GCLK_SYNCBUSY_GENCTRL_GCLK8           (GCLK_SYNCBUSY_GENCTRL_GCLK8_Val << GCLK_SYNCBUSY_GENCTRL_Pos) /**< (GCLK_SYNCBUSY) Generic clock generator 8 Position  */
#define GCLK_SYNCBUSY_GENCTRL_GCLK9           (GCLK_SYNCBUSY_GENCTRL_GCLK9_Val << GCLK_SYNCBUSY_GENCTRL_Pos) /**< (GCLK_SYNCBUSY) Generic clock generator 9 Position  */
#define GCLK_SYNCBUSY_GENCTRL_GCLK10          (GCLK_SYNCBUSY_GENCTRL_GCLK10_Val << GCLK_SYNCBUSY_GENCTRL_Pos) /**< (GCLK_SYNCBUSY) Generic clock generator 10 Position  */
#define GCLK_SYNCBUSY_GENCTRL_GCLK11          (GCLK_SYNCBUSY_GENCTRL_GCLK11_Val << GCLK_SYNCBUSY_GENCTRL_Pos) /**< (GCLK_SYNCBUSY) Generic clock generator 11 Position  */
#define GCLK_SYNCBUSY_Msk                     _U_(0x00003FFD)                                      /**< (GCLK_SYNCBUSY) Register Mask  */


/* -------- GCLK_GENCTRL : (GCLK Offset: 0x20) (R/W 32) Generic Clock Generator Control -------- */
#define GCLK_GENCTRL_RESETVALUE               _U_(0x00)                                            /**<  (GCLK_GENCTRL) Generic Clock Generator Control  Reset Value */

#define GCLK_GENCTRL_SRC_Pos                  _U_(0)                                               /**< (GCLK_GENCTRL) Source Select Position */
#define GCLK_GENCTRL_SRC_Msk                  (_U_(0xF) << GCLK_GENCTRL_SRC_Pos)                   /**< (GCLK_GENCTRL) Source Select Mask */
#define GCLK_GENCTRL_SRC(value)               (GCLK_GENCTRL_SRC_Msk & ((value) << GCLK_GENCTRL_SRC_Pos))
#define   GCLK_GENCTRL_SRC_XOSC0_Val          _U_(0x0)                                             /**< (GCLK_GENCTRL) XOSC0 oscillator output  */
#define   GCLK_GENCTRL_SRC_XOSC1_Val          _U_(0x1)                                             /**< (GCLK_GENCTRL) XOSC1 oscillator output  */
#define   GCLK_GENCTRL_SRC_GCLKIN_Val         _U_(0x2)                                             /**< (GCLK_GENCTRL) Generator input pad  */
#define   GCLK_GENCTRL_SRC_GCLKGEN1_Val       _U_(0x3)                                             /**< (GCLK_GENCTRL) Generic clock generator 1 output  */
#define   GCLK_GENCTRL_SRC_OSCULP32K_Val      _U_(0x4)                                             /**< (GCLK_GENCTRL) OSCULP32K oscillator output  */
#define   GCLK_GENCTRL_SRC_XOSC32K_Val        _U_(0x5)                                             /**< (GCLK_GENCTRL) XOSC32K oscillator output  */
#define   GCLK_GENCTRL_SRC_DFLL_Val           _U_(0x6)                                             /**< (GCLK_GENCTRL) DFLL output  */
#define   GCLK_GENCTRL_SRC_DPLL0_Val          _U_(0x7)                                             /**< (GCLK_GENCTRL) DPLL0 output  */
#define   GCLK_GENCTRL_SRC_DPLL1_Val          _U_(0x8)                                             /**< (GCLK_GENCTRL) DPLL1 output  */
#define GCLK_GENCTRL_SRC_XOSC0                (GCLK_GENCTRL_SRC_XOSC0_Val << GCLK_GENCTRL_SRC_Pos) /**< (GCLK_GENCTRL) XOSC0 oscillator output Position  */
#define GCLK_GENCTRL_SRC_XOSC1                (GCLK_GENCTRL_SRC_XOSC1_Val << GCLK_GENCTRL_SRC_Pos) /**< (GCLK_GENCTRL) XOSC1 oscillator output Position  */
#define GCLK_GENCTRL_SRC_GCLKIN               (GCLK_GENCTRL_SRC_GCLKIN_Val << GCLK_GENCTRL_SRC_Pos) /**< (GCLK_GENCTRL) Generator input pad Position  */
#define GCLK_GENCTRL_SRC_GCLKGEN1             (GCLK_GENCTRL_SRC_GCLKGEN1_Val << GCLK_GENCTRL_SRC_Pos) /**< (GCLK_GENCTRL) Generic clock generator 1 output Position  */
#define GCLK_GENCTRL_SRC_OSCULP32K            (GCLK_GENCTRL_SRC_OSCULP32K_Val << GCLK_GENCTRL_SRC_Pos) /**< (GCLK_GENCTRL) OSCULP32K oscillator output Position  */
#define GCLK_GENCTRL_SRC_XOSC32K              (GCLK_GENCTRL_SRC_XOSC32K_Val << GCLK_GENCTRL_SRC_Pos) /**< (GCLK_GENCTRL) XOSC32K oscillator output Position  */
#define GCLK_GENCTRL_SRC_DFLL                 (GCLK_GENCTRL_SRC_DFLL_Val << GCLK_GENCTRL_SRC_Pos)  /**< (GCLK_GENCTRL) DFLL output Position  */
#define GCLK_GENCTRL_SRC_DPLL0                (GCLK_GENCTRL_SRC_DPLL0_Val << GCLK_GENCTRL_SRC_Pos) /**< (GCLK_GENCTRL) DPLL0 output Position  */
#define GCLK_GENCTRL_SRC_DPLL1                (GCLK_GENCTRL_SRC_DPLL1_Val << GCLK_GENCTRL_SRC_Pos) /**< (GCLK_GENCTRL) DPLL1 output Position  */
#define GCLK_GENCTRL_GENEN_Pos                _U_(8)                                               /**< (GCLK_GENCTRL) Generic Clock Generator Enable Position */
#define GCLK_GENCTRL_GENEN_Msk                (_U_(0x1) << GCLK_GENCTRL_GENEN_Pos)                 /**< (GCLK_GENCTRL) Generic Clock Generator Enable Mask */
#define GCLK_GENCTRL_GENEN(value)             (GCLK_GENCTRL_GENEN_Msk & ((value) << GCLK_GENCTRL_GENEN_Pos))
#define GCLK_GENCTRL_IDC_Pos                  _U_(9)                                               /**< (GCLK_GENCTRL) Improve Duty Cycle Position */
#define GCLK_GENCTRL_IDC_Msk                  (_U_(0x1) << GCLK_GENCTRL_IDC_Pos)                   /**< (GCLK_GENCTRL) Improve Duty Cycle Mask */
#define GCLK_GENCTRL_IDC(value)               (GCLK_GENCTRL_IDC_Msk & ((value) << GCLK_GENCTRL_IDC_Pos))
#define GCLK_GENCTRL_OOV_Pos                  _U_(10)                                              /**< (GCLK_GENCTRL) Output Off Value Position */
#define GCLK_GENCTRL_OOV_Msk                  (_U_(0x1) << GCLK_GENCTRL_OOV_Pos)                   /**< (GCLK_GENCTRL) Output Off Value Mask */
#define GCLK_GENCTRL_OOV(value)               (GCLK_GENCTRL_OOV_Msk & ((value) << GCLK_GENCTRL_OOV_Pos))
#define GCLK_GENCTRL_OE_Pos                   _U_(11)                                              /**< (GCLK_GENCTRL) Output Enable Position */
#define GCLK_GENCTRL_OE_Msk                   (_U_(0x1) << GCLK_GENCTRL_OE_Pos)                    /**< (GCLK_GENCTRL) Output Enable Mask */
#define GCLK_GENCTRL_OE(value)                (GCLK_GENCTRL_OE_Msk & ((value) << GCLK_GENCTRL_OE_Pos))
#define GCLK_GENCTRL_DIVSEL_Pos               _U_(12)                                              /**< (GCLK_GENCTRL) Divide Selection Position */
#define GCLK_GENCTRL_DIVSEL_Msk               (_U_(0x1) << GCLK_GENCTRL_DIVSEL_Pos)                /**< (GCLK_GENCTRL) Divide Selection Mask */
#define GCLK_GENCTRL_DIVSEL(value)            (GCLK_GENCTRL_DIVSEL_Msk & ((value) << GCLK_GENCTRL_DIVSEL_Pos))
#define   GCLK_GENCTRL_DIVSEL_DIV1_Val        _U_(0x0)                                             /**< (GCLK_GENCTRL) Divide input directly by divider factor  */
#define   GCLK_GENCTRL_DIVSEL_DIV2_Val        _U_(0x1)                                             /**< (GCLK_GENCTRL) Divide input by 2^(divider factor+ 1)  */
#define GCLK_GENCTRL_DIVSEL_DIV1              (GCLK_GENCTRL_DIVSEL_DIV1_Val << GCLK_GENCTRL_DIVSEL_Pos) /**< (GCLK_GENCTRL) Divide input directly by divider factor Position  */
#define GCLK_GENCTRL_DIVSEL_DIV2              (GCLK_GENCTRL_DIVSEL_DIV2_Val << GCLK_GENCTRL_DIVSEL_Pos) /**< (GCLK_GENCTRL) Divide input by 2^(divider factor+ 1) Position  */
#define GCLK_GENCTRL_RUNSTDBY_Pos             _U_(13)                                              /**< (GCLK_GENCTRL) Run in Standby Position */
#define GCLK_GENCTRL_RUNSTDBY_Msk             (_U_(0x1) << GCLK_GENCTRL_RUNSTDBY_Pos)              /**< (GCLK_GENCTRL) Run in Standby Mask */
#define GCLK_GENCTRL_RUNSTDBY(value)          (GCLK_GENCTRL_RUNSTDBY_Msk & ((value) << GCLK_GENCTRL_RUNSTDBY_Pos))
#define GCLK_GENCTRL_DIV_Pos                  _U_(16)                                              /**< (GCLK_GENCTRL) Division Factor Position */
#define GCLK_GENCTRL_DIV_Msk                  (_U_(0xFFFF) << GCLK_GENCTRL_DIV_Pos)                /**< (GCLK_GENCTRL) Division Factor Mask */
#define GCLK_GENCTRL_DIV(value)               (GCLK_GENCTRL_DIV_Msk & ((value) << GCLK_GENCTRL_DIV_Pos))
#define GCLK_GENCTRL_Msk                      _U_(0xFFFF3F0F)                                      /**< (GCLK_GENCTRL) Register Mask  */


/* -------- GCLK_PCHCTRL : (GCLK Offset: 0x80) (R/W 32) Peripheral Clock Control -------- */
#define GCLK_PCHCTRL_RESETVALUE               _U_(0x00)                                            /**<  (GCLK_PCHCTRL) Peripheral Clock Control  Reset Value */

#define GCLK_PCHCTRL_GEN_Pos                  _U_(0)                                               /**< (GCLK_PCHCTRL) Generic Clock Generator Position */
#define GCLK_PCHCTRL_GEN_Msk                  (_U_(0xF) << GCLK_PCHCTRL_GEN_Pos)                   /**< (GCLK_PCHCTRL) Generic Clock Generator Mask */
#define GCLK_PCHCTRL_GEN(value)               (GCLK_PCHCTRL_GEN_Msk & ((value) << GCLK_PCHCTRL_GEN_Pos))
#define   GCLK_PCHCTRL_GEN_GCLK0_Val          _U_(0x0)                                             /**< (GCLK_PCHCTRL) Generic clock generator 0  */
#define   GCLK_PCHCTRL_GEN_GCLK1_Val          _U_(0x1)                                             /**< (GCLK_PCHCTRL) Generic clock generator 1  */
#define   GCLK_PCHCTRL_GEN_GCLK2_Val          _U_(0x2)                                             /**< (GCLK_PCHCTRL) Generic clock generator 2  */
#define   GCLK_PCHCTRL_GEN_GCLK3_Val          _U_(0x3)                                             /**< (GCLK_PCHCTRL) Generic clock generator 3  */
#define   GCLK_PCHCTRL_GEN_GCLK4_Val          _U_(0x4)                                             /**< (GCLK_PCHCTRL) Generic clock generator 4  */
#define   GCLK_PCHCTRL_GEN_GCLK5_Val          _U_(0x5)                                             /**< (GCLK_PCHCTRL) Generic clock generator 5  */
#define   GCLK_PCHCTRL_GEN_GCLK6_Val          _U_(0x6)                                             /**< (GCLK_PCHCTRL) Generic clock generator 6  */
#define   GCLK_PCHCTRL_GEN_GCLK7_Val          _U_(0x7)                                             /**< (GCLK_PCHCTRL) Generic clock generator 7  */
#define   GCLK_PCHCTRL_GEN_GCLK8_Val          _U_(0x8)                                             /**< (GCLK_PCHCTRL) Generic clock generator 8  */
#define   GCLK_PCHCTRL_GEN_GCLK9_Val          _U_(0x9)                                             /**< (GCLK_PCHCTRL) Generic clock generator 9  */
#define   GCLK_PCHCTRL_GEN_GCLK10_Val         _U_(0xA)                                             /**< (GCLK_PCHCTRL) Generic clock generator 10  */
#define   GCLK_PCHCTRL_GEN_GCLK11_Val         _U_(0xB)                                             /**< (GCLK_PCHCTRL) Generic clock generator 11  */
#define GCLK_PCHCTRL_GEN_GCLK0                (GCLK_PCHCTRL_GEN_GCLK0_Val << GCLK_PCHCTRL_GEN_Pos) /**< (GCLK_PCHCTRL) Generic clock generator 0 Position  */
#define GCLK_PCHCTRL_GEN_GCLK1                (GCLK_PCHCTRL_GEN_GCLK1_Val << GCLK_PCHCTRL_GEN_Pos) /**< (GCLK_PCHCTRL) Generic clock generator 1 Position  */
#define GCLK_PCHCTRL_GEN_GCLK2                (GCLK_PCHCTRL_GEN_GCLK2_Val << GCLK_PCHCTRL_GEN_Pos) /**< (GCLK_PCHCTRL) Generic clock generator 2 Position  */
#define GCLK_PCHCTRL_GEN_GCLK3                (GCLK_PCHCTRL_GEN_GCLK3_Val << GCLK_PCHCTRL_GEN_Pos) /**< (GCLK_PCHCTRL) Generic clock generator 3 Position  */
#define GCLK_PCHCTRL_GEN_GCLK4                (GCLK_PCHCTRL_GEN_GCLK4_Val << GCLK_PCHCTRL_GEN_Pos) /**< (GCLK_PCHCTRL) Generic clock generator 4 Position  */
#define GCLK_PCHCTRL_GEN_GCLK5                (GCLK_PCHCTRL_GEN_GCLK5_Val << GCLK_PCHCTRL_GEN_Pos) /**< (GCLK_PCHCTRL) Generic clock generator 5 Position  */
#define GCLK_PCHCTRL_GEN_GCLK6                (GCLK_PCHCTRL_GEN_GCLK6_Val << GCLK_PCHCTRL_GEN_Pos) /**< (GCLK_PCHCTRL) Generic clock generator 6 Position  */
#define GCLK_PCHCTRL_GEN_GCLK7                (GCLK_PCHCTRL_GEN_GCLK7_Val << GCLK_PCHCTRL_GEN_Pos) /**< (GCLK_PCHCTRL) Generic clock generator 7 Position  */
#define GCLK_PCHCTRL_GEN_GCLK8                (GCLK_PCHCTRL_GEN_GCLK8_Val << GCLK_PCHCTRL_GEN_Pos) /**< (GCLK_PCHCTRL) Generic clock generator 8 Position  */
#define GCLK_PCHCTRL_GEN_GCLK9                (GCLK_PCHCTRL_GEN_GCLK9_Val << GCLK_PCHCTRL_GEN_Pos) /**< (GCLK_PCHCTRL) Generic clock generator 9 Position  */
#define GCLK_PCHCTRL_GEN_GCLK10               (GCLK_PCHCTRL_GEN_GCLK10_Val << GCLK_PCHCTRL_GEN_Pos) /**< (GCLK_PCHCTRL) Generic clock generator 10 Position  */
#define GCLK_PCHCTRL_GEN_GCLK11               (GCLK_PCHCTRL_GEN_GCLK11_Val << GCLK_PCHCTRL_GEN_Pos) /**< (GCLK_PCHCTRL) Generic clock generator 11 Position  */
#define GCLK_PCHCTRL_CHEN_Pos                 _U_(6)                                               /**< (GCLK_PCHCTRL) Channel Enable Position */
#define GCLK_PCHCTRL_CHEN_Msk                 (_U_(0x1) << GCLK_PCHCTRL_CHEN_Pos)                  /**< (GCLK_PCHCTRL) Channel Enable Mask */
#define GCLK_PCHCTRL_CHEN(value)              (GCLK_PCHCTRL_CHEN_Msk & ((value) << GCLK_PCHCTRL_CHEN_Pos))
#define GCLK_PCHCTRL_WRTLOCK_Pos              _U_(7)                                               /**< (GCLK_PCHCTRL) Write Lock Position */
#define GCLK_PCHCTRL_WRTLOCK_Msk              (_U_(0x1) << GCLK_PCHCTRL_WRTLOCK_Pos)               /**< (GCLK_PCHCTRL) Write Lock Mask */
#define GCLK_PCHCTRL_WRTLOCK(value)           (GCLK_PCHCTRL_WRTLOCK_Msk & ((value) << GCLK_PCHCTRL_WRTLOCK_Pos))
#define GCLK_PCHCTRL_Msk                      _U_(0x000000CF)                                      /**< (GCLK_PCHCTRL) Register Mask  */


/** \brief GCLK register offsets definitions */
#define GCLK_CTRLA_REG_OFST            (0x00)              /**< (GCLK_CTRLA) Control Offset */
#define GCLK_SYNCBUSY_REG_OFST         (0x04)              /**< (GCLK_SYNCBUSY) Synchronization Busy Offset */
#define GCLK_GENCTRL_REG_OFST          (0x20)              /**< (GCLK_GENCTRL) Generic Clock Generator Control Offset */
#define GCLK_PCHCTRL_REG_OFST          (0x80)              /**< (GCLK_PCHCTRL) Peripheral Clock Control Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief GCLK register API structure */
typedef struct
{  /* Generic Clock Generator */
  __IO  uint8_t                        GCLK_CTRLA;         /**< Offset: 0x00 (R/W  8) Control */
  __I   uint8_t                        Reserved1[0x03];
  __I   uint32_t                       GCLK_SYNCBUSY;      /**< Offset: 0x04 (R/   32) Synchronization Busy */
  __I   uint8_t                        Reserved2[0x18];
  __IO  uint32_t                       GCLK_GENCTRL[12];   /**< Offset: 0x20 (R/W  32) Generic Clock Generator Control */
  __I   uint8_t                        Reserved3[0x30];
  __IO  uint32_t                       GCLK_PCHCTRL[48];   /**< Offset: 0x80 (R/W  32) Peripheral Clock Control */
} gclk_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_GCLK_COMPONENT_H_ */
