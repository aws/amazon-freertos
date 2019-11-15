/**
 * \brief Component description for PORT
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
#ifndef _SAME54_PORT_COMPONENT_H_
#define _SAME54_PORT_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR PORT                                         */
/* ************************************************************************** */

/* -------- PORT_DIR : (PORT Offset: 0x00) (R/W 32) Data Direction -------- */
#define PORT_DIR_RESETVALUE                   _U_(0x00)                                            /**<  (PORT_DIR) Data Direction  Reset Value */

#define PORT_DIR_DIR_Pos                      _U_(0)                                               /**< (PORT_DIR) Port Data Direction Position */
#define PORT_DIR_DIR_Msk                      (_U_(0xFFFFFFFF) << PORT_DIR_DIR_Pos)                /**< (PORT_DIR) Port Data Direction Mask */
#define PORT_DIR_DIR(value)                   (PORT_DIR_DIR_Msk & ((value) << PORT_DIR_DIR_Pos))  
#define PORT_DIR_Msk                          _U_(0xFFFFFFFF)                                      /**< (PORT_DIR) Register Mask  */


/* -------- PORT_DIRCLR : (PORT Offset: 0x04) (R/W 32) Data Direction Clear -------- */
#define PORT_DIRCLR_RESETVALUE                _U_(0x00)                                            /**<  (PORT_DIRCLR) Data Direction Clear  Reset Value */

#define PORT_DIRCLR_DIRCLR_Pos                _U_(0)                                               /**< (PORT_DIRCLR) Port Data Direction Clear Position */
#define PORT_DIRCLR_DIRCLR_Msk                (_U_(0xFFFFFFFF) << PORT_DIRCLR_DIRCLR_Pos)          /**< (PORT_DIRCLR) Port Data Direction Clear Mask */
#define PORT_DIRCLR_DIRCLR(value)             (PORT_DIRCLR_DIRCLR_Msk & ((value) << PORT_DIRCLR_DIRCLR_Pos))
#define PORT_DIRCLR_Msk                       _U_(0xFFFFFFFF)                                      /**< (PORT_DIRCLR) Register Mask  */


/* -------- PORT_DIRSET : (PORT Offset: 0x08) (R/W 32) Data Direction Set -------- */
#define PORT_DIRSET_RESETVALUE                _U_(0x00)                                            /**<  (PORT_DIRSET) Data Direction Set  Reset Value */

#define PORT_DIRSET_DIRSET_Pos                _U_(0)                                               /**< (PORT_DIRSET) Port Data Direction Set Position */
#define PORT_DIRSET_DIRSET_Msk                (_U_(0xFFFFFFFF) << PORT_DIRSET_DIRSET_Pos)          /**< (PORT_DIRSET) Port Data Direction Set Mask */
#define PORT_DIRSET_DIRSET(value)             (PORT_DIRSET_DIRSET_Msk & ((value) << PORT_DIRSET_DIRSET_Pos))
#define PORT_DIRSET_Msk                       _U_(0xFFFFFFFF)                                      /**< (PORT_DIRSET) Register Mask  */


/* -------- PORT_DIRTGL : (PORT Offset: 0x0C) (R/W 32) Data Direction Toggle -------- */
#define PORT_DIRTGL_RESETVALUE                _U_(0x00)                                            /**<  (PORT_DIRTGL) Data Direction Toggle  Reset Value */

#define PORT_DIRTGL_DIRTGL_Pos                _U_(0)                                               /**< (PORT_DIRTGL) Port Data Direction Toggle Position */
#define PORT_DIRTGL_DIRTGL_Msk                (_U_(0xFFFFFFFF) << PORT_DIRTGL_DIRTGL_Pos)          /**< (PORT_DIRTGL) Port Data Direction Toggle Mask */
#define PORT_DIRTGL_DIRTGL(value)             (PORT_DIRTGL_DIRTGL_Msk & ((value) << PORT_DIRTGL_DIRTGL_Pos))
#define PORT_DIRTGL_Msk                       _U_(0xFFFFFFFF)                                      /**< (PORT_DIRTGL) Register Mask  */


/* -------- PORT_OUT : (PORT Offset: 0x10) (R/W 32) Data Output Value -------- */
#define PORT_OUT_RESETVALUE                   _U_(0x00)                                            /**<  (PORT_OUT) Data Output Value  Reset Value */

#define PORT_OUT_OUT_Pos                      _U_(0)                                               /**< (PORT_OUT) PORT Data Output Value Position */
#define PORT_OUT_OUT_Msk                      (_U_(0xFFFFFFFF) << PORT_OUT_OUT_Pos)                /**< (PORT_OUT) PORT Data Output Value Mask */
#define PORT_OUT_OUT(value)                   (PORT_OUT_OUT_Msk & ((value) << PORT_OUT_OUT_Pos))  
#define PORT_OUT_Msk                          _U_(0xFFFFFFFF)                                      /**< (PORT_OUT) Register Mask  */


/* -------- PORT_OUTCLR : (PORT Offset: 0x14) (R/W 32) Data Output Value Clear -------- */
#define PORT_OUTCLR_RESETVALUE                _U_(0x00)                                            /**<  (PORT_OUTCLR) Data Output Value Clear  Reset Value */

#define PORT_OUTCLR_OUTCLR_Pos                _U_(0)                                               /**< (PORT_OUTCLR) PORT Data Output Value Clear Position */
#define PORT_OUTCLR_OUTCLR_Msk                (_U_(0xFFFFFFFF) << PORT_OUTCLR_OUTCLR_Pos)          /**< (PORT_OUTCLR) PORT Data Output Value Clear Mask */
#define PORT_OUTCLR_OUTCLR(value)             (PORT_OUTCLR_OUTCLR_Msk & ((value) << PORT_OUTCLR_OUTCLR_Pos))
#define PORT_OUTCLR_Msk                       _U_(0xFFFFFFFF)                                      /**< (PORT_OUTCLR) Register Mask  */


/* -------- PORT_OUTSET : (PORT Offset: 0x18) (R/W 32) Data Output Value Set -------- */
#define PORT_OUTSET_RESETVALUE                _U_(0x00)                                            /**<  (PORT_OUTSET) Data Output Value Set  Reset Value */

#define PORT_OUTSET_OUTSET_Pos                _U_(0)                                               /**< (PORT_OUTSET) PORT Data Output Value Set Position */
#define PORT_OUTSET_OUTSET_Msk                (_U_(0xFFFFFFFF) << PORT_OUTSET_OUTSET_Pos)          /**< (PORT_OUTSET) PORT Data Output Value Set Mask */
#define PORT_OUTSET_OUTSET(value)             (PORT_OUTSET_OUTSET_Msk & ((value) << PORT_OUTSET_OUTSET_Pos))
#define PORT_OUTSET_Msk                       _U_(0xFFFFFFFF)                                      /**< (PORT_OUTSET) Register Mask  */


/* -------- PORT_OUTTGL : (PORT Offset: 0x1C) (R/W 32) Data Output Value Toggle -------- */
#define PORT_OUTTGL_RESETVALUE                _U_(0x00)                                            /**<  (PORT_OUTTGL) Data Output Value Toggle  Reset Value */

#define PORT_OUTTGL_OUTTGL_Pos                _U_(0)                                               /**< (PORT_OUTTGL) PORT Data Output Value Toggle Position */
#define PORT_OUTTGL_OUTTGL_Msk                (_U_(0xFFFFFFFF) << PORT_OUTTGL_OUTTGL_Pos)          /**< (PORT_OUTTGL) PORT Data Output Value Toggle Mask */
#define PORT_OUTTGL_OUTTGL(value)             (PORT_OUTTGL_OUTTGL_Msk & ((value) << PORT_OUTTGL_OUTTGL_Pos))
#define PORT_OUTTGL_Msk                       _U_(0xFFFFFFFF)                                      /**< (PORT_OUTTGL) Register Mask  */


/* -------- PORT_IN : (PORT Offset: 0x20) ( R/ 32) Data Input Value -------- */
#define PORT_IN_RESETVALUE                    _U_(0x00)                                            /**<  (PORT_IN) Data Input Value  Reset Value */

#define PORT_IN_IN_Pos                        _U_(0)                                               /**< (PORT_IN) PORT Data Input Value Position */
#define PORT_IN_IN_Msk                        (_U_(0xFFFFFFFF) << PORT_IN_IN_Pos)                  /**< (PORT_IN) PORT Data Input Value Mask */
#define PORT_IN_IN(value)                     (PORT_IN_IN_Msk & ((value) << PORT_IN_IN_Pos))      
#define PORT_IN_Msk                           _U_(0xFFFFFFFF)                                      /**< (PORT_IN) Register Mask  */


/* -------- PORT_CTRL : (PORT Offset: 0x24) (R/W 32) Control -------- */
#define PORT_CTRL_RESETVALUE                  _U_(0x00)                                            /**<  (PORT_CTRL) Control  Reset Value */

#define PORT_CTRL_SAMPLING_Pos                _U_(0)                                               /**< (PORT_CTRL) Input Sampling Mode Position */
#define PORT_CTRL_SAMPLING_Msk                (_U_(0xFFFFFFFF) << PORT_CTRL_SAMPLING_Pos)          /**< (PORT_CTRL) Input Sampling Mode Mask */
#define PORT_CTRL_SAMPLING(value)             (PORT_CTRL_SAMPLING_Msk & ((value) << PORT_CTRL_SAMPLING_Pos))
#define PORT_CTRL_Msk                         _U_(0xFFFFFFFF)                                      /**< (PORT_CTRL) Register Mask  */


/* -------- PORT_WRCONFIG : (PORT Offset: 0x28) ( /W 32) Write Configuration -------- */
#define PORT_WRCONFIG_RESETVALUE              _U_(0x00)                                            /**<  (PORT_WRCONFIG) Write Configuration  Reset Value */

#define PORT_WRCONFIG_PINMASK_Pos             _U_(0)                                               /**< (PORT_WRCONFIG) Pin Mask for Multiple Pin Configuration Position */
#define PORT_WRCONFIG_PINMASK_Msk             (_U_(0xFFFF) << PORT_WRCONFIG_PINMASK_Pos)           /**< (PORT_WRCONFIG) Pin Mask for Multiple Pin Configuration Mask */
#define PORT_WRCONFIG_PINMASK(value)          (PORT_WRCONFIG_PINMASK_Msk & ((value) << PORT_WRCONFIG_PINMASK_Pos))
#define PORT_WRCONFIG_PMUXEN_Pos              _U_(16)                                              /**< (PORT_WRCONFIG) Peripheral Multiplexer Enable Position */
#define PORT_WRCONFIG_PMUXEN_Msk              (_U_(0x1) << PORT_WRCONFIG_PMUXEN_Pos)               /**< (PORT_WRCONFIG) Peripheral Multiplexer Enable Mask */
#define PORT_WRCONFIG_PMUXEN(value)           (PORT_WRCONFIG_PMUXEN_Msk & ((value) << PORT_WRCONFIG_PMUXEN_Pos))
#define PORT_WRCONFIG_INEN_Pos                _U_(17)                                              /**< (PORT_WRCONFIG) Input Enable Position */
#define PORT_WRCONFIG_INEN_Msk                (_U_(0x1) << PORT_WRCONFIG_INEN_Pos)                 /**< (PORT_WRCONFIG) Input Enable Mask */
#define PORT_WRCONFIG_INEN(value)             (PORT_WRCONFIG_INEN_Msk & ((value) << PORT_WRCONFIG_INEN_Pos))
#define PORT_WRCONFIG_PULLEN_Pos              _U_(18)                                              /**< (PORT_WRCONFIG) Pull Enable Position */
#define PORT_WRCONFIG_PULLEN_Msk              (_U_(0x1) << PORT_WRCONFIG_PULLEN_Pos)               /**< (PORT_WRCONFIG) Pull Enable Mask */
#define PORT_WRCONFIG_PULLEN(value)           (PORT_WRCONFIG_PULLEN_Msk & ((value) << PORT_WRCONFIG_PULLEN_Pos))
#define PORT_WRCONFIG_DRVSTR_Pos              _U_(22)                                              /**< (PORT_WRCONFIG) Output Driver Strength Selection Position */
#define PORT_WRCONFIG_DRVSTR_Msk              (_U_(0x1) << PORT_WRCONFIG_DRVSTR_Pos)               /**< (PORT_WRCONFIG) Output Driver Strength Selection Mask */
#define PORT_WRCONFIG_DRVSTR(value)           (PORT_WRCONFIG_DRVSTR_Msk & ((value) << PORT_WRCONFIG_DRVSTR_Pos))
#define PORT_WRCONFIG_PMUX_Pos                _U_(24)                                              /**< (PORT_WRCONFIG) Peripheral Multiplexing Position */
#define PORT_WRCONFIG_PMUX_Msk                (_U_(0xF) << PORT_WRCONFIG_PMUX_Pos)                 /**< (PORT_WRCONFIG) Peripheral Multiplexing Mask */
#define PORT_WRCONFIG_PMUX(value)             (PORT_WRCONFIG_PMUX_Msk & ((value) << PORT_WRCONFIG_PMUX_Pos))
#define PORT_WRCONFIG_WRPMUX_Pos              _U_(28)                                              /**< (PORT_WRCONFIG) Write PMUX Position */
#define PORT_WRCONFIG_WRPMUX_Msk              (_U_(0x1) << PORT_WRCONFIG_WRPMUX_Pos)               /**< (PORT_WRCONFIG) Write PMUX Mask */
#define PORT_WRCONFIG_WRPMUX(value)           (PORT_WRCONFIG_WRPMUX_Msk & ((value) << PORT_WRCONFIG_WRPMUX_Pos))
#define PORT_WRCONFIG_WRPINCFG_Pos            _U_(30)                                              /**< (PORT_WRCONFIG) Write PINCFG Position */
#define PORT_WRCONFIG_WRPINCFG_Msk            (_U_(0x1) << PORT_WRCONFIG_WRPINCFG_Pos)             /**< (PORT_WRCONFIG) Write PINCFG Mask */
#define PORT_WRCONFIG_WRPINCFG(value)         (PORT_WRCONFIG_WRPINCFG_Msk & ((value) << PORT_WRCONFIG_WRPINCFG_Pos))
#define PORT_WRCONFIG_HWSEL_Pos               _U_(31)                                              /**< (PORT_WRCONFIG) Half-Word Select Position */
#define PORT_WRCONFIG_HWSEL_Msk               (_U_(0x1) << PORT_WRCONFIG_HWSEL_Pos)                /**< (PORT_WRCONFIG) Half-Word Select Mask */
#define PORT_WRCONFIG_HWSEL(value)            (PORT_WRCONFIG_HWSEL_Msk & ((value) << PORT_WRCONFIG_HWSEL_Pos))
#define PORT_WRCONFIG_Msk                     _U_(0xDF47FFFF)                                      /**< (PORT_WRCONFIG) Register Mask  */


/* -------- PORT_EVCTRL : (PORT Offset: 0x2C) (R/W 32) Event Input Control -------- */
#define PORT_EVCTRL_RESETVALUE                _U_(0x00)                                            /**<  (PORT_EVCTRL) Event Input Control  Reset Value */

#define PORT_EVCTRL_PID0_Pos                  _U_(0)                                               /**< (PORT_EVCTRL) PORT Event Pin Identifier 0 Position */
#define PORT_EVCTRL_PID0_Msk                  (_U_(0x1F) << PORT_EVCTRL_PID0_Pos)                  /**< (PORT_EVCTRL) PORT Event Pin Identifier 0 Mask */
#define PORT_EVCTRL_PID0(value)               (PORT_EVCTRL_PID0_Msk & ((value) << PORT_EVCTRL_PID0_Pos))
#define PORT_EVCTRL_EVACT0_Pos                _U_(5)                                               /**< (PORT_EVCTRL) PORT Event Action 0 Position */
#define PORT_EVCTRL_EVACT0_Msk                (_U_(0x3) << PORT_EVCTRL_EVACT0_Pos)                 /**< (PORT_EVCTRL) PORT Event Action 0 Mask */
#define PORT_EVCTRL_EVACT0(value)             (PORT_EVCTRL_EVACT0_Msk & ((value) << PORT_EVCTRL_EVACT0_Pos))
#define   PORT_EVCTRL_EVACT0_OUT_Val          _U_(0x0)                                             /**< (PORT_EVCTRL) Event output to pin  */
#define   PORT_EVCTRL_EVACT0_SET_Val          _U_(0x1)                                             /**< (PORT_EVCTRL) Set output register of pin on event  */
#define   PORT_EVCTRL_EVACT0_CLR_Val          _U_(0x2)                                             /**< (PORT_EVCTRL) Clear output register of pin on event  */
#define   PORT_EVCTRL_EVACT0_TGL_Val          _U_(0x3)                                             /**< (PORT_EVCTRL) Toggle output register of pin on event  */
#define PORT_EVCTRL_EVACT0_OUT                (PORT_EVCTRL_EVACT0_OUT_Val << PORT_EVCTRL_EVACT0_Pos) /**< (PORT_EVCTRL) Event output to pin Position  */
#define PORT_EVCTRL_EVACT0_SET                (PORT_EVCTRL_EVACT0_SET_Val << PORT_EVCTRL_EVACT0_Pos) /**< (PORT_EVCTRL) Set output register of pin on event Position  */
#define PORT_EVCTRL_EVACT0_CLR                (PORT_EVCTRL_EVACT0_CLR_Val << PORT_EVCTRL_EVACT0_Pos) /**< (PORT_EVCTRL) Clear output register of pin on event Position  */
#define PORT_EVCTRL_EVACT0_TGL                (PORT_EVCTRL_EVACT0_TGL_Val << PORT_EVCTRL_EVACT0_Pos) /**< (PORT_EVCTRL) Toggle output register of pin on event Position  */
#define PORT_EVCTRL_PORTEI0_Pos               _U_(7)                                               /**< (PORT_EVCTRL) PORT Event Input Enable 0 Position */
#define PORT_EVCTRL_PORTEI0_Msk               (_U_(0x1) << PORT_EVCTRL_PORTEI0_Pos)                /**< (PORT_EVCTRL) PORT Event Input Enable 0 Mask */
#define PORT_EVCTRL_PORTEI0(value)            (PORT_EVCTRL_PORTEI0_Msk & ((value) << PORT_EVCTRL_PORTEI0_Pos))
#define PORT_EVCTRL_PID1_Pos                  _U_(8)                                               /**< (PORT_EVCTRL) PORT Event Pin Identifier 1 Position */
#define PORT_EVCTRL_PID1_Msk                  (_U_(0x1F) << PORT_EVCTRL_PID1_Pos)                  /**< (PORT_EVCTRL) PORT Event Pin Identifier 1 Mask */
#define PORT_EVCTRL_PID1(value)               (PORT_EVCTRL_PID1_Msk & ((value) << PORT_EVCTRL_PID1_Pos))
#define PORT_EVCTRL_EVACT1_Pos                _U_(13)                                              /**< (PORT_EVCTRL) PORT Event Action 1 Position */
#define PORT_EVCTRL_EVACT1_Msk                (_U_(0x3) << PORT_EVCTRL_EVACT1_Pos)                 /**< (PORT_EVCTRL) PORT Event Action 1 Mask */
#define PORT_EVCTRL_EVACT1(value)             (PORT_EVCTRL_EVACT1_Msk & ((value) << PORT_EVCTRL_EVACT1_Pos))
#define PORT_EVCTRL_PORTEI1_Pos               _U_(15)                                              /**< (PORT_EVCTRL) PORT Event Input Enable 1 Position */
#define PORT_EVCTRL_PORTEI1_Msk               (_U_(0x1) << PORT_EVCTRL_PORTEI1_Pos)                /**< (PORT_EVCTRL) PORT Event Input Enable 1 Mask */
#define PORT_EVCTRL_PORTEI1(value)            (PORT_EVCTRL_PORTEI1_Msk & ((value) << PORT_EVCTRL_PORTEI1_Pos))
#define PORT_EVCTRL_PID2_Pos                  _U_(16)                                              /**< (PORT_EVCTRL) PORT Event Pin Identifier 2 Position */
#define PORT_EVCTRL_PID2_Msk                  (_U_(0x1F) << PORT_EVCTRL_PID2_Pos)                  /**< (PORT_EVCTRL) PORT Event Pin Identifier 2 Mask */
#define PORT_EVCTRL_PID2(value)               (PORT_EVCTRL_PID2_Msk & ((value) << PORT_EVCTRL_PID2_Pos))
#define PORT_EVCTRL_EVACT2_Pos                _U_(21)                                              /**< (PORT_EVCTRL) PORT Event Action 2 Position */
#define PORT_EVCTRL_EVACT2_Msk                (_U_(0x3) << PORT_EVCTRL_EVACT2_Pos)                 /**< (PORT_EVCTRL) PORT Event Action 2 Mask */
#define PORT_EVCTRL_EVACT2(value)             (PORT_EVCTRL_EVACT2_Msk & ((value) << PORT_EVCTRL_EVACT2_Pos))
#define PORT_EVCTRL_PORTEI2_Pos               _U_(23)                                              /**< (PORT_EVCTRL) PORT Event Input Enable 2 Position */
#define PORT_EVCTRL_PORTEI2_Msk               (_U_(0x1) << PORT_EVCTRL_PORTEI2_Pos)                /**< (PORT_EVCTRL) PORT Event Input Enable 2 Mask */
#define PORT_EVCTRL_PORTEI2(value)            (PORT_EVCTRL_PORTEI2_Msk & ((value) << PORT_EVCTRL_PORTEI2_Pos))
#define PORT_EVCTRL_PID3_Pos                  _U_(24)                                              /**< (PORT_EVCTRL) PORT Event Pin Identifier 3 Position */
#define PORT_EVCTRL_PID3_Msk                  (_U_(0x1F) << PORT_EVCTRL_PID3_Pos)                  /**< (PORT_EVCTRL) PORT Event Pin Identifier 3 Mask */
#define PORT_EVCTRL_PID3(value)               (PORT_EVCTRL_PID3_Msk & ((value) << PORT_EVCTRL_PID3_Pos))
#define PORT_EVCTRL_EVACT3_Pos                _U_(29)                                              /**< (PORT_EVCTRL) PORT Event Action 3 Position */
#define PORT_EVCTRL_EVACT3_Msk                (_U_(0x3) << PORT_EVCTRL_EVACT3_Pos)                 /**< (PORT_EVCTRL) PORT Event Action 3 Mask */
#define PORT_EVCTRL_EVACT3(value)             (PORT_EVCTRL_EVACT3_Msk & ((value) << PORT_EVCTRL_EVACT3_Pos))
#define PORT_EVCTRL_PORTEI3_Pos               _U_(31)                                              /**< (PORT_EVCTRL) PORT Event Input Enable 3 Position */
#define PORT_EVCTRL_PORTEI3_Msk               (_U_(0x1) << PORT_EVCTRL_PORTEI3_Pos)                /**< (PORT_EVCTRL) PORT Event Input Enable 3 Mask */
#define PORT_EVCTRL_PORTEI3(value)            (PORT_EVCTRL_PORTEI3_Msk & ((value) << PORT_EVCTRL_PORTEI3_Pos))
#define PORT_EVCTRL_Msk                       _U_(0xFFFFFFFF)                                      /**< (PORT_EVCTRL) Register Mask  */


/* -------- PORT_PMUX : (PORT Offset: 0x30) (R/W 8) Peripheral Multiplexing -------- */
#define PORT_PMUX_RESETVALUE                  _U_(0x00)                                            /**<  (PORT_PMUX) Peripheral Multiplexing  Reset Value */

#define PORT_PMUX_PMUXE_Pos                   _U_(0)                                               /**< (PORT_PMUX) Peripheral Multiplexing for Even-Numbered Pin Position */
#define PORT_PMUX_PMUXE_Msk                   (_U_(0xF) << PORT_PMUX_PMUXE_Pos)                    /**< (PORT_PMUX) Peripheral Multiplexing for Even-Numbered Pin Mask */
#define PORT_PMUX_PMUXE(value)                (PORT_PMUX_PMUXE_Msk & ((value) << PORT_PMUX_PMUXE_Pos))
#define PORT_PMUX_PMUXO_Pos                   _U_(4)                                               /**< (PORT_PMUX) Peripheral Multiplexing for Odd-Numbered Pin Position */
#define PORT_PMUX_PMUXO_Msk                   (_U_(0xF) << PORT_PMUX_PMUXO_Pos)                    /**< (PORT_PMUX) Peripheral Multiplexing for Odd-Numbered Pin Mask */
#define PORT_PMUX_PMUXO(value)                (PORT_PMUX_PMUXO_Msk & ((value) << PORT_PMUX_PMUXO_Pos))
#define PORT_PMUX_Msk                         _U_(0xFF)                                            /**< (PORT_PMUX) Register Mask  */


/* -------- PORT_PINCFG : (PORT Offset: 0x40) (R/W 8) Pin Configuration -------- */
#define PORT_PINCFG_RESETVALUE                _U_(0x00)                                            /**<  (PORT_PINCFG) Pin Configuration  Reset Value */

#define PORT_PINCFG_PMUXEN_Pos                _U_(0)                                               /**< (PORT_PINCFG) Peripheral Multiplexer Enable Position */
#define PORT_PINCFG_PMUXEN_Msk                (_U_(0x1) << PORT_PINCFG_PMUXEN_Pos)                 /**< (PORT_PINCFG) Peripheral Multiplexer Enable Mask */
#define PORT_PINCFG_PMUXEN(value)             (PORT_PINCFG_PMUXEN_Msk & ((value) << PORT_PINCFG_PMUXEN_Pos))
#define PORT_PINCFG_INEN_Pos                  _U_(1)                                               /**< (PORT_PINCFG) Input Enable Position */
#define PORT_PINCFG_INEN_Msk                  (_U_(0x1) << PORT_PINCFG_INEN_Pos)                   /**< (PORT_PINCFG) Input Enable Mask */
#define PORT_PINCFG_INEN(value)               (PORT_PINCFG_INEN_Msk & ((value) << PORT_PINCFG_INEN_Pos))
#define PORT_PINCFG_PULLEN_Pos                _U_(2)                                               /**< (PORT_PINCFG) Pull Enable Position */
#define PORT_PINCFG_PULLEN_Msk                (_U_(0x1) << PORT_PINCFG_PULLEN_Pos)                 /**< (PORT_PINCFG) Pull Enable Mask */
#define PORT_PINCFG_PULLEN(value)             (PORT_PINCFG_PULLEN_Msk & ((value) << PORT_PINCFG_PULLEN_Pos))
#define PORT_PINCFG_DRVSTR_Pos                _U_(6)                                               /**< (PORT_PINCFG) Output Driver Strength Selection Position */
#define PORT_PINCFG_DRVSTR_Msk                (_U_(0x1) << PORT_PINCFG_DRVSTR_Pos)                 /**< (PORT_PINCFG) Output Driver Strength Selection Mask */
#define PORT_PINCFG_DRVSTR(value)             (PORT_PINCFG_DRVSTR_Msk & ((value) << PORT_PINCFG_DRVSTR_Pos))
#define PORT_PINCFG_Msk                       _U_(0x47)                                            /**< (PORT_PINCFG) Register Mask  */


/** \brief PORT register offsets definitions */
#define PORT_DIR_REG_OFST              (0x00)              /**< (PORT_DIR) Data Direction Offset */
#define PORT_DIRCLR_REG_OFST           (0x04)              /**< (PORT_DIRCLR) Data Direction Clear Offset */
#define PORT_DIRSET_REG_OFST           (0x08)              /**< (PORT_DIRSET) Data Direction Set Offset */
#define PORT_DIRTGL_REG_OFST           (0x0C)              /**< (PORT_DIRTGL) Data Direction Toggle Offset */
#define PORT_OUT_REG_OFST              (0x10)              /**< (PORT_OUT) Data Output Value Offset */
#define PORT_OUTCLR_REG_OFST           (0x14)              /**< (PORT_OUTCLR) Data Output Value Clear Offset */
#define PORT_OUTSET_REG_OFST           (0x18)              /**< (PORT_OUTSET) Data Output Value Set Offset */
#define PORT_OUTTGL_REG_OFST           (0x1C)              /**< (PORT_OUTTGL) Data Output Value Toggle Offset */
#define PORT_IN_REG_OFST               (0x20)              /**< (PORT_IN) Data Input Value Offset */
#define PORT_CTRL_REG_OFST             (0x24)              /**< (PORT_CTRL) Control Offset */
#define PORT_WRCONFIG_REG_OFST         (0x28)              /**< (PORT_WRCONFIG) Write Configuration Offset */
#define PORT_EVCTRL_REG_OFST           (0x2C)              /**< (PORT_EVCTRL) Event Input Control Offset */
#define PORT_PMUX_REG_OFST             (0x30)              /**< (PORT_PMUX) Peripheral Multiplexing Offset */
#define PORT_PINCFG_REG_OFST           (0x40)              /**< (PORT_PINCFG) Pin Configuration Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief GROUP register API structure */
typedef struct
{
  __IO  uint32_t                       PORT_DIR;           /**< Offset: 0x00 (R/W  32) Data Direction */
  __IO  uint32_t                       PORT_DIRCLR;        /**< Offset: 0x04 (R/W  32) Data Direction Clear */
  __IO  uint32_t                       PORT_DIRSET;        /**< Offset: 0x08 (R/W  32) Data Direction Set */
  __IO  uint32_t                       PORT_DIRTGL;        /**< Offset: 0x0C (R/W  32) Data Direction Toggle */
  __IO  uint32_t                       PORT_OUT;           /**< Offset: 0x10 (R/W  32) Data Output Value */
  __IO  uint32_t                       PORT_OUTCLR;        /**< Offset: 0x14 (R/W  32) Data Output Value Clear */
  __IO  uint32_t                       PORT_OUTSET;        /**< Offset: 0x18 (R/W  32) Data Output Value Set */
  __IO  uint32_t                       PORT_OUTTGL;        /**< Offset: 0x1C (R/W  32) Data Output Value Toggle */
  __I   uint32_t                       PORT_IN;            /**< Offset: 0x20 (R/   32) Data Input Value */
  __IO  uint32_t                       PORT_CTRL;          /**< Offset: 0x24 (R/W  32) Control */
  __O   uint32_t                       PORT_WRCONFIG;      /**< Offset: 0x28 ( /W  32) Write Configuration */
  __IO  uint32_t                       PORT_EVCTRL;        /**< Offset: 0x2C (R/W  32) Event Input Control */
  __IO  uint8_t                        PORT_PMUX[16];      /**< Offset: 0x30 (R/W  8) Peripheral Multiplexing */
  __IO  uint8_t                        PORT_PINCFG[32];    /**< Offset: 0x40 (R/W  8) Pin Configuration */
  __I   uint8_t                        Reserved1[0x20];
} port_group_registers_t;

#define GROUP_NUMBER _U_(4)

/** \brief PORT register API structure */
typedef struct
{  /* Port Module */
        port_group_registers_t         GROUP[GROUP_NUMBER]; /**< Offset: 0x00  */
} port_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_PORT_COMPONENT_H_ */
