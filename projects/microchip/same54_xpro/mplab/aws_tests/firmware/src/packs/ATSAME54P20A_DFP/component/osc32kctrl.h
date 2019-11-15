/**
 * \brief Component description for OSC32KCTRL
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
#ifndef _SAME54_OSC32KCTRL_COMPONENT_H_
#define _SAME54_OSC32KCTRL_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR OSC32KCTRL                                   */
/* ************************************************************************** */

/* -------- OSC32KCTRL_INTENCLR : (OSC32KCTRL Offset: 0x00) (R/W 32) Interrupt Enable Clear -------- */
#define OSC32KCTRL_INTENCLR_RESETVALUE        _U_(0x00)                                            /**<  (OSC32KCTRL_INTENCLR) Interrupt Enable Clear  Reset Value */

#define OSC32KCTRL_INTENCLR_XOSC32KRDY_Pos    _U_(0)                                               /**< (OSC32KCTRL_INTENCLR) XOSC32K Ready Interrupt Enable Position */
#define OSC32KCTRL_INTENCLR_XOSC32KRDY_Msk    (_U_(0x1) << OSC32KCTRL_INTENCLR_XOSC32KRDY_Pos)     /**< (OSC32KCTRL_INTENCLR) XOSC32K Ready Interrupt Enable Mask */
#define OSC32KCTRL_INTENCLR_XOSC32KRDY(value) (OSC32KCTRL_INTENCLR_XOSC32KRDY_Msk & ((value) << OSC32KCTRL_INTENCLR_XOSC32KRDY_Pos))
#define OSC32KCTRL_INTENCLR_XOSC32KFAIL_Pos   _U_(2)                                               /**< (OSC32KCTRL_INTENCLR) XOSC32K Clock Failure Detector Interrupt Enable Position */
#define OSC32KCTRL_INTENCLR_XOSC32KFAIL_Msk   (_U_(0x1) << OSC32KCTRL_INTENCLR_XOSC32KFAIL_Pos)    /**< (OSC32KCTRL_INTENCLR) XOSC32K Clock Failure Detector Interrupt Enable Mask */
#define OSC32KCTRL_INTENCLR_XOSC32KFAIL(value) (OSC32KCTRL_INTENCLR_XOSC32KFAIL_Msk & ((value) << OSC32KCTRL_INTENCLR_XOSC32KFAIL_Pos))
#define OSC32KCTRL_INTENCLR_Msk               _U_(0x00000005)                                      /**< (OSC32KCTRL_INTENCLR) Register Mask  */


/* -------- OSC32KCTRL_INTENSET : (OSC32KCTRL Offset: 0x04) (R/W 32) Interrupt Enable Set -------- */
#define OSC32KCTRL_INTENSET_RESETVALUE        _U_(0x00)                                            /**<  (OSC32KCTRL_INTENSET) Interrupt Enable Set  Reset Value */

#define OSC32KCTRL_INTENSET_XOSC32KRDY_Pos    _U_(0)                                               /**< (OSC32KCTRL_INTENSET) XOSC32K Ready Interrupt Enable Position */
#define OSC32KCTRL_INTENSET_XOSC32KRDY_Msk    (_U_(0x1) << OSC32KCTRL_INTENSET_XOSC32KRDY_Pos)     /**< (OSC32KCTRL_INTENSET) XOSC32K Ready Interrupt Enable Mask */
#define OSC32KCTRL_INTENSET_XOSC32KRDY(value) (OSC32KCTRL_INTENSET_XOSC32KRDY_Msk & ((value) << OSC32KCTRL_INTENSET_XOSC32KRDY_Pos))
#define OSC32KCTRL_INTENSET_XOSC32KFAIL_Pos   _U_(2)                                               /**< (OSC32KCTRL_INTENSET) XOSC32K Clock Failure Detector Interrupt Enable Position */
#define OSC32KCTRL_INTENSET_XOSC32KFAIL_Msk   (_U_(0x1) << OSC32KCTRL_INTENSET_XOSC32KFAIL_Pos)    /**< (OSC32KCTRL_INTENSET) XOSC32K Clock Failure Detector Interrupt Enable Mask */
#define OSC32KCTRL_INTENSET_XOSC32KFAIL(value) (OSC32KCTRL_INTENSET_XOSC32KFAIL_Msk & ((value) << OSC32KCTRL_INTENSET_XOSC32KFAIL_Pos))
#define OSC32KCTRL_INTENSET_Msk               _U_(0x00000005)                                      /**< (OSC32KCTRL_INTENSET) Register Mask  */


/* -------- OSC32KCTRL_INTFLAG : (OSC32KCTRL Offset: 0x08) (R/W 32) Interrupt Flag Status and Clear -------- */
#define OSC32KCTRL_INTFLAG_RESETVALUE         _U_(0x00)                                            /**<  (OSC32KCTRL_INTFLAG) Interrupt Flag Status and Clear  Reset Value */

#define OSC32KCTRL_INTFLAG_XOSC32KRDY_Pos     _U_(0)                                               /**< (OSC32KCTRL_INTFLAG) XOSC32K Ready Position */
#define OSC32KCTRL_INTFLAG_XOSC32KRDY_Msk     (_U_(0x1) << OSC32KCTRL_INTFLAG_XOSC32KRDY_Pos)      /**< (OSC32KCTRL_INTFLAG) XOSC32K Ready Mask */
#define OSC32KCTRL_INTFLAG_XOSC32KRDY(value)  (OSC32KCTRL_INTFLAG_XOSC32KRDY_Msk & ((value) << OSC32KCTRL_INTFLAG_XOSC32KRDY_Pos))
#define OSC32KCTRL_INTFLAG_XOSC32KFAIL_Pos    _U_(2)                                               /**< (OSC32KCTRL_INTFLAG) XOSC32K Clock Failure Detector Position */
#define OSC32KCTRL_INTFLAG_XOSC32KFAIL_Msk    (_U_(0x1) << OSC32KCTRL_INTFLAG_XOSC32KFAIL_Pos)     /**< (OSC32KCTRL_INTFLAG) XOSC32K Clock Failure Detector Mask */
#define OSC32KCTRL_INTFLAG_XOSC32KFAIL(value) (OSC32KCTRL_INTFLAG_XOSC32KFAIL_Msk & ((value) << OSC32KCTRL_INTFLAG_XOSC32KFAIL_Pos))
#define OSC32KCTRL_INTFLAG_Msk                _U_(0x00000005)                                      /**< (OSC32KCTRL_INTFLAG) Register Mask  */


/* -------- OSC32KCTRL_STATUS : (OSC32KCTRL Offset: 0x0C) ( R/ 32) Power and Clocks Status -------- */
#define OSC32KCTRL_STATUS_RESETVALUE          _U_(0x00)                                            /**<  (OSC32KCTRL_STATUS) Power and Clocks Status  Reset Value */

#define OSC32KCTRL_STATUS_XOSC32KRDY_Pos      _U_(0)                                               /**< (OSC32KCTRL_STATUS) XOSC32K Ready Position */
#define OSC32KCTRL_STATUS_XOSC32KRDY_Msk      (_U_(0x1) << OSC32KCTRL_STATUS_XOSC32KRDY_Pos)       /**< (OSC32KCTRL_STATUS) XOSC32K Ready Mask */
#define OSC32KCTRL_STATUS_XOSC32KRDY(value)   (OSC32KCTRL_STATUS_XOSC32KRDY_Msk & ((value) << OSC32KCTRL_STATUS_XOSC32KRDY_Pos))
#define OSC32KCTRL_STATUS_XOSC32KFAIL_Pos     _U_(2)                                               /**< (OSC32KCTRL_STATUS) XOSC32K Clock Failure Detector Position */
#define OSC32KCTRL_STATUS_XOSC32KFAIL_Msk     (_U_(0x1) << OSC32KCTRL_STATUS_XOSC32KFAIL_Pos)      /**< (OSC32KCTRL_STATUS) XOSC32K Clock Failure Detector Mask */
#define OSC32KCTRL_STATUS_XOSC32KFAIL(value)  (OSC32KCTRL_STATUS_XOSC32KFAIL_Msk & ((value) << OSC32KCTRL_STATUS_XOSC32KFAIL_Pos))
#define OSC32KCTRL_STATUS_XOSC32KSW_Pos       _U_(3)                                               /**< (OSC32KCTRL_STATUS) XOSC32K Clock switch Position */
#define OSC32KCTRL_STATUS_XOSC32KSW_Msk       (_U_(0x1) << OSC32KCTRL_STATUS_XOSC32KSW_Pos)        /**< (OSC32KCTRL_STATUS) XOSC32K Clock switch Mask */
#define OSC32KCTRL_STATUS_XOSC32KSW(value)    (OSC32KCTRL_STATUS_XOSC32KSW_Msk & ((value) << OSC32KCTRL_STATUS_XOSC32KSW_Pos))
#define OSC32KCTRL_STATUS_Msk                 _U_(0x0000000D)                                      /**< (OSC32KCTRL_STATUS) Register Mask  */


/* -------- OSC32KCTRL_RTCCTRL : (OSC32KCTRL Offset: 0x10) (R/W 8) RTC Clock Selection -------- */
#define OSC32KCTRL_RTCCTRL_RESETVALUE         _U_(0x00)                                            /**<  (OSC32KCTRL_RTCCTRL) RTC Clock Selection  Reset Value */

#define OSC32KCTRL_RTCCTRL_RTCSEL_Pos         _U_(0)                                               /**< (OSC32KCTRL_RTCCTRL) RTC Clock Selection Position */
#define OSC32KCTRL_RTCCTRL_RTCSEL_Msk         (_U_(0x7) << OSC32KCTRL_RTCCTRL_RTCSEL_Pos)          /**< (OSC32KCTRL_RTCCTRL) RTC Clock Selection Mask */
#define OSC32KCTRL_RTCCTRL_RTCSEL(value)      (OSC32KCTRL_RTCCTRL_RTCSEL_Msk & ((value) << OSC32KCTRL_RTCCTRL_RTCSEL_Pos))
#define   OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K_Val _U_(0x0)                                             /**< (OSC32KCTRL_RTCCTRL) 1.024kHz from 32kHz internal ULP oscillator  */
#define   OSC32KCTRL_RTCCTRL_RTCSEL_ULP32K_Val _U_(0x1)                                             /**< (OSC32KCTRL_RTCCTRL) 32.768kHz from 32kHz internal ULP oscillator  */
#define   OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K_Val _U_(0x4)                                             /**< (OSC32KCTRL_RTCCTRL) 1.024kHz from 32.768kHz internal oscillator  */
#define   OSC32KCTRL_RTCCTRL_RTCSEL_XOSC32K_Val _U_(0x5)                                             /**< (OSC32KCTRL_RTCCTRL) 32.768kHz from 32.768kHz external crystal oscillator  */
#define OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K       (OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K_Val << OSC32KCTRL_RTCCTRL_RTCSEL_Pos) /**< (OSC32KCTRL_RTCCTRL) 1.024kHz from 32kHz internal ULP oscillator Position  */
#define OSC32KCTRL_RTCCTRL_RTCSEL_ULP32K      (OSC32KCTRL_RTCCTRL_RTCSEL_ULP32K_Val << OSC32KCTRL_RTCCTRL_RTCSEL_Pos) /**< (OSC32KCTRL_RTCCTRL) 32.768kHz from 32kHz internal ULP oscillator Position  */
#define OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K      (OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K_Val << OSC32KCTRL_RTCCTRL_RTCSEL_Pos) /**< (OSC32KCTRL_RTCCTRL) 1.024kHz from 32.768kHz internal oscillator Position  */
#define OSC32KCTRL_RTCCTRL_RTCSEL_XOSC32K     (OSC32KCTRL_RTCCTRL_RTCSEL_XOSC32K_Val << OSC32KCTRL_RTCCTRL_RTCSEL_Pos) /**< (OSC32KCTRL_RTCCTRL) 32.768kHz from 32.768kHz external crystal oscillator Position  */
#define OSC32KCTRL_RTCCTRL_Msk                _U_(0x07)                                            /**< (OSC32KCTRL_RTCCTRL) Register Mask  */


/* -------- OSC32KCTRL_XOSC32K : (OSC32KCTRL Offset: 0x14) (R/W 16) 32kHz External Crystal Oscillator (XOSC32K) Control -------- */
#define OSC32KCTRL_XOSC32K_RESETVALUE         _U_(0x2080)                                          /**<  (OSC32KCTRL_XOSC32K) 32kHz External Crystal Oscillator (XOSC32K) Control  Reset Value */

#define OSC32KCTRL_XOSC32K_ENABLE_Pos         _U_(1)                                               /**< (OSC32KCTRL_XOSC32K) Oscillator Enable Position */
#define OSC32KCTRL_XOSC32K_ENABLE_Msk         (_U_(0x1) << OSC32KCTRL_XOSC32K_ENABLE_Pos)          /**< (OSC32KCTRL_XOSC32K) Oscillator Enable Mask */
#define OSC32KCTRL_XOSC32K_ENABLE(value)      (OSC32KCTRL_XOSC32K_ENABLE_Msk & ((value) << OSC32KCTRL_XOSC32K_ENABLE_Pos))
#define OSC32KCTRL_XOSC32K_XTALEN_Pos         _U_(2)                                               /**< (OSC32KCTRL_XOSC32K) Crystal Oscillator Enable Position */
#define OSC32KCTRL_XOSC32K_XTALEN_Msk         (_U_(0x1) << OSC32KCTRL_XOSC32K_XTALEN_Pos)          /**< (OSC32KCTRL_XOSC32K) Crystal Oscillator Enable Mask */
#define OSC32KCTRL_XOSC32K_XTALEN(value)      (OSC32KCTRL_XOSC32K_XTALEN_Msk & ((value) << OSC32KCTRL_XOSC32K_XTALEN_Pos))
#define OSC32KCTRL_XOSC32K_EN32K_Pos          _U_(3)                                               /**< (OSC32KCTRL_XOSC32K) 32kHz Output Enable Position */
#define OSC32KCTRL_XOSC32K_EN32K_Msk          (_U_(0x1) << OSC32KCTRL_XOSC32K_EN32K_Pos)           /**< (OSC32KCTRL_XOSC32K) 32kHz Output Enable Mask */
#define OSC32KCTRL_XOSC32K_EN32K(value)       (OSC32KCTRL_XOSC32K_EN32K_Msk & ((value) << OSC32KCTRL_XOSC32K_EN32K_Pos))
#define OSC32KCTRL_XOSC32K_EN1K_Pos           _U_(4)                                               /**< (OSC32KCTRL_XOSC32K) 1kHz Output Enable Position */
#define OSC32KCTRL_XOSC32K_EN1K_Msk           (_U_(0x1) << OSC32KCTRL_XOSC32K_EN1K_Pos)            /**< (OSC32KCTRL_XOSC32K) 1kHz Output Enable Mask */
#define OSC32KCTRL_XOSC32K_EN1K(value)        (OSC32KCTRL_XOSC32K_EN1K_Msk & ((value) << OSC32KCTRL_XOSC32K_EN1K_Pos))
#define OSC32KCTRL_XOSC32K_RUNSTDBY_Pos       _U_(6)                                               /**< (OSC32KCTRL_XOSC32K) Run in Standby Position */
#define OSC32KCTRL_XOSC32K_RUNSTDBY_Msk       (_U_(0x1) << OSC32KCTRL_XOSC32K_RUNSTDBY_Pos)        /**< (OSC32KCTRL_XOSC32K) Run in Standby Mask */
#define OSC32KCTRL_XOSC32K_RUNSTDBY(value)    (OSC32KCTRL_XOSC32K_RUNSTDBY_Msk & ((value) << OSC32KCTRL_XOSC32K_RUNSTDBY_Pos))
#define OSC32KCTRL_XOSC32K_ONDEMAND_Pos       _U_(7)                                               /**< (OSC32KCTRL_XOSC32K) On Demand Control Position */
#define OSC32KCTRL_XOSC32K_ONDEMAND_Msk       (_U_(0x1) << OSC32KCTRL_XOSC32K_ONDEMAND_Pos)        /**< (OSC32KCTRL_XOSC32K) On Demand Control Mask */
#define OSC32KCTRL_XOSC32K_ONDEMAND(value)    (OSC32KCTRL_XOSC32K_ONDEMAND_Msk & ((value) << OSC32KCTRL_XOSC32K_ONDEMAND_Pos))
#define OSC32KCTRL_XOSC32K_STARTUP_Pos        _U_(8)                                               /**< (OSC32KCTRL_XOSC32K) Oscillator Start-Up Time Position */
#define OSC32KCTRL_XOSC32K_STARTUP_Msk        (_U_(0x7) << OSC32KCTRL_XOSC32K_STARTUP_Pos)         /**< (OSC32KCTRL_XOSC32K) Oscillator Start-Up Time Mask */
#define OSC32KCTRL_XOSC32K_STARTUP(value)     (OSC32KCTRL_XOSC32K_STARTUP_Msk & ((value) << OSC32KCTRL_XOSC32K_STARTUP_Pos))
#define   OSC32KCTRL_XOSC32K_STARTUP_CYCLE2048_Val _U_(0x0)                                             /**< (OSC32KCTRL_XOSC32K) 62.6 ms  */
#define   OSC32KCTRL_XOSC32K_STARTUP_CYCLE4096_Val _U_(0x1)                                             /**< (OSC32KCTRL_XOSC32K) 125 ms  */
#define   OSC32KCTRL_XOSC32K_STARTUP_CYCLE16384_Val _U_(0x2)                                             /**< (OSC32KCTRL_XOSC32K) 500 ms  */
#define   OSC32KCTRL_XOSC32K_STARTUP_CYCLE32768_Val _U_(0x3)                                             /**< (OSC32KCTRL_XOSC32K) 1000 ms  */
#define   OSC32KCTRL_XOSC32K_STARTUP_CYCLE65536_Val _U_(0x4)                                             /**< (OSC32KCTRL_XOSC32K) 2000 ms  */
#define   OSC32KCTRL_XOSC32K_STARTUP_CYCLE131072_Val _U_(0x5)                                             /**< (OSC32KCTRL_XOSC32K) 4000 ms  */
#define   OSC32KCTRL_XOSC32K_STARTUP_CYCLE262144_Val _U_(0x6)                                             /**< (OSC32KCTRL_XOSC32K) 8000 ms  */
#define OSC32KCTRL_XOSC32K_STARTUP_CYCLE2048  (OSC32KCTRL_XOSC32K_STARTUP_CYCLE2048_Val << OSC32KCTRL_XOSC32K_STARTUP_Pos) /**< (OSC32KCTRL_XOSC32K) 62.6 ms Position  */
#define OSC32KCTRL_XOSC32K_STARTUP_CYCLE4096  (OSC32KCTRL_XOSC32K_STARTUP_CYCLE4096_Val << OSC32KCTRL_XOSC32K_STARTUP_Pos) /**< (OSC32KCTRL_XOSC32K) 125 ms Position  */
#define OSC32KCTRL_XOSC32K_STARTUP_CYCLE16384 (OSC32KCTRL_XOSC32K_STARTUP_CYCLE16384_Val << OSC32KCTRL_XOSC32K_STARTUP_Pos) /**< (OSC32KCTRL_XOSC32K) 500 ms Position  */
#define OSC32KCTRL_XOSC32K_STARTUP_CYCLE32768 (OSC32KCTRL_XOSC32K_STARTUP_CYCLE32768_Val << OSC32KCTRL_XOSC32K_STARTUP_Pos) /**< (OSC32KCTRL_XOSC32K) 1000 ms Position  */
#define OSC32KCTRL_XOSC32K_STARTUP_CYCLE65536 (OSC32KCTRL_XOSC32K_STARTUP_CYCLE65536_Val << OSC32KCTRL_XOSC32K_STARTUP_Pos) /**< (OSC32KCTRL_XOSC32K) 2000 ms Position  */
#define OSC32KCTRL_XOSC32K_STARTUP_CYCLE131072 (OSC32KCTRL_XOSC32K_STARTUP_CYCLE131072_Val << OSC32KCTRL_XOSC32K_STARTUP_Pos) /**< (OSC32KCTRL_XOSC32K) 4000 ms Position  */
#define OSC32KCTRL_XOSC32K_STARTUP_CYCLE262144 (OSC32KCTRL_XOSC32K_STARTUP_CYCLE262144_Val << OSC32KCTRL_XOSC32K_STARTUP_Pos) /**< (OSC32KCTRL_XOSC32K) 8000 ms Position  */
#define OSC32KCTRL_XOSC32K_WRTLOCK_Pos        _U_(12)                                              /**< (OSC32KCTRL_XOSC32K) Write Lock Position */
#define OSC32KCTRL_XOSC32K_WRTLOCK_Msk        (_U_(0x1) << OSC32KCTRL_XOSC32K_WRTLOCK_Pos)         /**< (OSC32KCTRL_XOSC32K) Write Lock Mask */
#define OSC32KCTRL_XOSC32K_WRTLOCK(value)     (OSC32KCTRL_XOSC32K_WRTLOCK_Msk & ((value) << OSC32KCTRL_XOSC32K_WRTLOCK_Pos))
#define OSC32KCTRL_XOSC32K_CGM_Pos            _U_(13)                                              /**< (OSC32KCTRL_XOSC32K) Control Gain Mode Position */
#define OSC32KCTRL_XOSC32K_CGM_Msk            (_U_(0x3) << OSC32KCTRL_XOSC32K_CGM_Pos)             /**< (OSC32KCTRL_XOSC32K) Control Gain Mode Mask */
#define OSC32KCTRL_XOSC32K_CGM(value)         (OSC32KCTRL_XOSC32K_CGM_Msk & ((value) << OSC32KCTRL_XOSC32K_CGM_Pos))
#define   OSC32KCTRL_XOSC32K_CGM_XT_Val       _U_(0x1)                                             /**< (OSC32KCTRL_XOSC32K) Standard mode  */
#define   OSC32KCTRL_XOSC32K_CGM_HS_Val       _U_(0x2)                                             /**< (OSC32KCTRL_XOSC32K) High Speed mode  */
#define OSC32KCTRL_XOSC32K_CGM_XT             (OSC32KCTRL_XOSC32K_CGM_XT_Val << OSC32KCTRL_XOSC32K_CGM_Pos) /**< (OSC32KCTRL_XOSC32K) Standard mode Position  */
#define OSC32KCTRL_XOSC32K_CGM_HS             (OSC32KCTRL_XOSC32K_CGM_HS_Val << OSC32KCTRL_XOSC32K_CGM_Pos) /**< (OSC32KCTRL_XOSC32K) High Speed mode Position  */
#define OSC32KCTRL_XOSC32K_Msk                _U_(0x77DE)                                          /**< (OSC32KCTRL_XOSC32K) Register Mask  */


/* -------- OSC32KCTRL_CFDCTRL : (OSC32KCTRL Offset: 0x16) (R/W 8) Clock Failure Detector Control -------- */
#define OSC32KCTRL_CFDCTRL_RESETVALUE         _U_(0x00)                                            /**<  (OSC32KCTRL_CFDCTRL) Clock Failure Detector Control  Reset Value */

#define OSC32KCTRL_CFDCTRL_CFDEN_Pos          _U_(0)                                               /**< (OSC32KCTRL_CFDCTRL) Clock Failure Detector Enable Position */
#define OSC32KCTRL_CFDCTRL_CFDEN_Msk          (_U_(0x1) << OSC32KCTRL_CFDCTRL_CFDEN_Pos)           /**< (OSC32KCTRL_CFDCTRL) Clock Failure Detector Enable Mask */
#define OSC32KCTRL_CFDCTRL_CFDEN(value)       (OSC32KCTRL_CFDCTRL_CFDEN_Msk & ((value) << OSC32KCTRL_CFDCTRL_CFDEN_Pos))
#define OSC32KCTRL_CFDCTRL_SWBACK_Pos         _U_(1)                                               /**< (OSC32KCTRL_CFDCTRL) Clock Switch Back Position */
#define OSC32KCTRL_CFDCTRL_SWBACK_Msk         (_U_(0x1) << OSC32KCTRL_CFDCTRL_SWBACK_Pos)          /**< (OSC32KCTRL_CFDCTRL) Clock Switch Back Mask */
#define OSC32KCTRL_CFDCTRL_SWBACK(value)      (OSC32KCTRL_CFDCTRL_SWBACK_Msk & ((value) << OSC32KCTRL_CFDCTRL_SWBACK_Pos))
#define OSC32KCTRL_CFDCTRL_CFDPRESC_Pos       _U_(2)                                               /**< (OSC32KCTRL_CFDCTRL) Clock Failure Detector Prescaler Position */
#define OSC32KCTRL_CFDCTRL_CFDPRESC_Msk       (_U_(0x1) << OSC32KCTRL_CFDCTRL_CFDPRESC_Pos)        /**< (OSC32KCTRL_CFDCTRL) Clock Failure Detector Prescaler Mask */
#define OSC32KCTRL_CFDCTRL_CFDPRESC(value)    (OSC32KCTRL_CFDCTRL_CFDPRESC_Msk & ((value) << OSC32KCTRL_CFDCTRL_CFDPRESC_Pos))
#define OSC32KCTRL_CFDCTRL_Msk                _U_(0x07)                                            /**< (OSC32KCTRL_CFDCTRL) Register Mask  */


/* -------- OSC32KCTRL_EVCTRL : (OSC32KCTRL Offset: 0x17) (R/W 8) Event Control -------- */
#define OSC32KCTRL_EVCTRL_RESETVALUE          _U_(0x00)                                            /**<  (OSC32KCTRL_EVCTRL) Event Control  Reset Value */

#define OSC32KCTRL_EVCTRL_CFDEO_Pos           _U_(0)                                               /**< (OSC32KCTRL_EVCTRL) Clock Failure Detector Event Output Enable Position */
#define OSC32KCTRL_EVCTRL_CFDEO_Msk           (_U_(0x1) << OSC32KCTRL_EVCTRL_CFDEO_Pos)            /**< (OSC32KCTRL_EVCTRL) Clock Failure Detector Event Output Enable Mask */
#define OSC32KCTRL_EVCTRL_CFDEO(value)        (OSC32KCTRL_EVCTRL_CFDEO_Msk & ((value) << OSC32KCTRL_EVCTRL_CFDEO_Pos))
#define OSC32KCTRL_EVCTRL_Msk                 _U_(0x01)                                            /**< (OSC32KCTRL_EVCTRL) Register Mask  */


/* -------- OSC32KCTRL_OSCULP32K : (OSC32KCTRL Offset: 0x1C) (R/W 32) 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control -------- */
#define OSC32KCTRL_OSCULP32K_RESETVALUE       _U_(0x00)                                            /**<  (OSC32KCTRL_OSCULP32K) 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control  Reset Value */

#define OSC32KCTRL_OSCULP32K_EN32K_Pos        _U_(1)                                               /**< (OSC32KCTRL_OSCULP32K) Enable Out 32k Position */
#define OSC32KCTRL_OSCULP32K_EN32K_Msk        (_U_(0x1) << OSC32KCTRL_OSCULP32K_EN32K_Pos)         /**< (OSC32KCTRL_OSCULP32K) Enable Out 32k Mask */
#define OSC32KCTRL_OSCULP32K_EN32K(value)     (OSC32KCTRL_OSCULP32K_EN32K_Msk & ((value) << OSC32KCTRL_OSCULP32K_EN32K_Pos))
#define OSC32KCTRL_OSCULP32K_EN1K_Pos         _U_(2)                                               /**< (OSC32KCTRL_OSCULP32K) Enable Out 1k Position */
#define OSC32KCTRL_OSCULP32K_EN1K_Msk         (_U_(0x1) << OSC32KCTRL_OSCULP32K_EN1K_Pos)          /**< (OSC32KCTRL_OSCULP32K) Enable Out 1k Mask */
#define OSC32KCTRL_OSCULP32K_EN1K(value)      (OSC32KCTRL_OSCULP32K_EN1K_Msk & ((value) << OSC32KCTRL_OSCULP32K_EN1K_Pos))
#define OSC32KCTRL_OSCULP32K_CALIB_Pos        _U_(8)                                               /**< (OSC32KCTRL_OSCULP32K) Oscillator Calibration Position */
#define OSC32KCTRL_OSCULP32K_CALIB_Msk        (_U_(0x3F) << OSC32KCTRL_OSCULP32K_CALIB_Pos)        /**< (OSC32KCTRL_OSCULP32K) Oscillator Calibration Mask */
#define OSC32KCTRL_OSCULP32K_CALIB(value)     (OSC32KCTRL_OSCULP32K_CALIB_Msk & ((value) << OSC32KCTRL_OSCULP32K_CALIB_Pos))
#define OSC32KCTRL_OSCULP32K_WRTLOCK_Pos      _U_(15)                                              /**< (OSC32KCTRL_OSCULP32K) Write Lock Position */
#define OSC32KCTRL_OSCULP32K_WRTLOCK_Msk      (_U_(0x1) << OSC32KCTRL_OSCULP32K_WRTLOCK_Pos)       /**< (OSC32KCTRL_OSCULP32K) Write Lock Mask */
#define OSC32KCTRL_OSCULP32K_WRTLOCK(value)   (OSC32KCTRL_OSCULP32K_WRTLOCK_Msk & ((value) << OSC32KCTRL_OSCULP32K_WRTLOCK_Pos))
#define OSC32KCTRL_OSCULP32K_Msk              _U_(0x0000BF06)                                      /**< (OSC32KCTRL_OSCULP32K) Register Mask  */


/** \brief OSC32KCTRL register offsets definitions */
#define OSC32KCTRL_INTENCLR_REG_OFST   (0x00)              /**< (OSC32KCTRL_INTENCLR) Interrupt Enable Clear Offset */
#define OSC32KCTRL_INTENSET_REG_OFST   (0x04)              /**< (OSC32KCTRL_INTENSET) Interrupt Enable Set Offset */
#define OSC32KCTRL_INTFLAG_REG_OFST    (0x08)              /**< (OSC32KCTRL_INTFLAG) Interrupt Flag Status and Clear Offset */
#define OSC32KCTRL_STATUS_REG_OFST     (0x0C)              /**< (OSC32KCTRL_STATUS) Power and Clocks Status Offset */
#define OSC32KCTRL_RTCCTRL_REG_OFST    (0x10)              /**< (OSC32KCTRL_RTCCTRL) RTC Clock Selection Offset */
#define OSC32KCTRL_XOSC32K_REG_OFST    (0x14)              /**< (OSC32KCTRL_XOSC32K) 32kHz External Crystal Oscillator (XOSC32K) Control Offset */
#define OSC32KCTRL_CFDCTRL_REG_OFST    (0x16)              /**< (OSC32KCTRL_CFDCTRL) Clock Failure Detector Control Offset */
#define OSC32KCTRL_EVCTRL_REG_OFST     (0x17)              /**< (OSC32KCTRL_EVCTRL) Event Control Offset */
#define OSC32KCTRL_OSCULP32K_REG_OFST  (0x1C)              /**< (OSC32KCTRL_OSCULP32K) 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief OSC32KCTRL register API structure */
typedef struct
{  /* 32kHz Oscillators Control */
  __IO  uint32_t                       OSC32KCTRL_INTENCLR; /**< Offset: 0x00 (R/W  32) Interrupt Enable Clear */
  __IO  uint32_t                       OSC32KCTRL_INTENSET; /**< Offset: 0x04 (R/W  32) Interrupt Enable Set */
  __IO  uint32_t                       OSC32KCTRL_INTFLAG; /**< Offset: 0x08 (R/W  32) Interrupt Flag Status and Clear */
  __I   uint32_t                       OSC32KCTRL_STATUS;  /**< Offset: 0x0C (R/   32) Power and Clocks Status */
  __IO  uint8_t                        OSC32KCTRL_RTCCTRL; /**< Offset: 0x10 (R/W  8) RTC Clock Selection */
  __I   uint8_t                        Reserved1[0x03];
  __IO  uint16_t                       OSC32KCTRL_XOSC32K; /**< Offset: 0x14 (R/W  16) 32kHz External Crystal Oscillator (XOSC32K) Control */
  __IO  uint8_t                        OSC32KCTRL_CFDCTRL; /**< Offset: 0x16 (R/W  8) Clock Failure Detector Control */
  __IO  uint8_t                        OSC32KCTRL_EVCTRL;  /**< Offset: 0x17 (R/W  8) Event Control */
  __I   uint8_t                        Reserved2[0x04];
  __IO  uint32_t                       OSC32KCTRL_OSCULP32K; /**< Offset: 0x1C (R/W  32) 32kHz Ultra Low Power Internal Oscillator (OSCULP32K) Control */
} osc32kctrl_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_OSC32KCTRL_COMPONENT_H_ */
