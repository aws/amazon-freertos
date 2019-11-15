/**
 * \brief Component description for DAC
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
#ifndef _SAME54_DAC_COMPONENT_H_
#define _SAME54_DAC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR DAC                                          */
/* ************************************************************************** */

/* -------- DAC_CTRLA : (DAC Offset: 0x00) (R/W 8) Control A -------- */
#define DAC_CTRLA_RESETVALUE                  _U_(0x00)                                            /**<  (DAC_CTRLA) Control A  Reset Value */

#define DAC_CTRLA_SWRST_Pos                   _U_(0)                                               /**< (DAC_CTRLA) Software Reset Position */
#define DAC_CTRLA_SWRST_Msk                   (_U_(0x1) << DAC_CTRLA_SWRST_Pos)                    /**< (DAC_CTRLA) Software Reset Mask */
#define DAC_CTRLA_SWRST(value)                (DAC_CTRLA_SWRST_Msk & ((value) << DAC_CTRLA_SWRST_Pos))
#define DAC_CTRLA_ENABLE_Pos                  _U_(1)                                               /**< (DAC_CTRLA) Enable DAC Controller Position */
#define DAC_CTRLA_ENABLE_Msk                  (_U_(0x1) << DAC_CTRLA_ENABLE_Pos)                   /**< (DAC_CTRLA) Enable DAC Controller Mask */
#define DAC_CTRLA_ENABLE(value)               (DAC_CTRLA_ENABLE_Msk & ((value) << DAC_CTRLA_ENABLE_Pos))
#define DAC_CTRLA_Msk                         _U_(0x03)                                            /**< (DAC_CTRLA) Register Mask  */


/* -------- DAC_CTRLB : (DAC Offset: 0x01) (R/W 8) Control B -------- */
#define DAC_CTRLB_RESETVALUE                  _U_(0x02)                                            /**<  (DAC_CTRLB) Control B  Reset Value */

#define DAC_CTRLB_DIFF_Pos                    _U_(0)                                               /**< (DAC_CTRLB) Differential mode enable Position */
#define DAC_CTRLB_DIFF_Msk                    (_U_(0x1) << DAC_CTRLB_DIFF_Pos)                     /**< (DAC_CTRLB) Differential mode enable Mask */
#define DAC_CTRLB_DIFF(value)                 (DAC_CTRLB_DIFF_Msk & ((value) << DAC_CTRLB_DIFF_Pos))
#define DAC_CTRLB_REFSEL_Pos                  _U_(1)                                               /**< (DAC_CTRLB) Reference Selection for DAC0/1 Position */
#define DAC_CTRLB_REFSEL_Msk                  (_U_(0x3) << DAC_CTRLB_REFSEL_Pos)                   /**< (DAC_CTRLB) Reference Selection for DAC0/1 Mask */
#define DAC_CTRLB_REFSEL(value)               (DAC_CTRLB_REFSEL_Msk & ((value) << DAC_CTRLB_REFSEL_Pos))
#define   DAC_CTRLB_REFSEL_VREFPU_Val         _U_(0x0)                                             /**< (DAC_CTRLB) External reference unbuffered  */
#define   DAC_CTRLB_REFSEL_VDDANA_Val         _U_(0x1)                                             /**< (DAC_CTRLB) Analog supply  */
#define   DAC_CTRLB_REFSEL_VREFPB_Val         _U_(0x2)                                             /**< (DAC_CTRLB) External reference buffered  */
#define   DAC_CTRLB_REFSEL_INTREF_Val         _U_(0x3)                                             /**< (DAC_CTRLB) Internal bandgap reference  */
#define DAC_CTRLB_REFSEL_VREFPU               (DAC_CTRLB_REFSEL_VREFPU_Val << DAC_CTRLB_REFSEL_Pos) /**< (DAC_CTRLB) External reference unbuffered Position  */
#define DAC_CTRLB_REFSEL_VDDANA               (DAC_CTRLB_REFSEL_VDDANA_Val << DAC_CTRLB_REFSEL_Pos) /**< (DAC_CTRLB) Analog supply Position  */
#define DAC_CTRLB_REFSEL_VREFPB               (DAC_CTRLB_REFSEL_VREFPB_Val << DAC_CTRLB_REFSEL_Pos) /**< (DAC_CTRLB) External reference buffered Position  */
#define DAC_CTRLB_REFSEL_INTREF               (DAC_CTRLB_REFSEL_INTREF_Val << DAC_CTRLB_REFSEL_Pos) /**< (DAC_CTRLB) Internal bandgap reference Position  */
#define DAC_CTRLB_Msk                         _U_(0x07)                                            /**< (DAC_CTRLB) Register Mask  */


/* -------- DAC_EVCTRL : (DAC Offset: 0x02) (R/W 8) Event Control -------- */
#define DAC_EVCTRL_RESETVALUE                 _U_(0x00)                                            /**<  (DAC_EVCTRL) Event Control  Reset Value */

#define DAC_EVCTRL_STARTEI0_Pos               _U_(0)                                               /**< (DAC_EVCTRL) Start Conversion Event Input DAC 0 Position */
#define DAC_EVCTRL_STARTEI0_Msk               (_U_(0x1) << DAC_EVCTRL_STARTEI0_Pos)                /**< (DAC_EVCTRL) Start Conversion Event Input DAC 0 Mask */
#define DAC_EVCTRL_STARTEI0(value)            (DAC_EVCTRL_STARTEI0_Msk & ((value) << DAC_EVCTRL_STARTEI0_Pos))
#define DAC_EVCTRL_STARTEI1_Pos               _U_(1)                                               /**< (DAC_EVCTRL) Start Conversion Event Input DAC 1 Position */
#define DAC_EVCTRL_STARTEI1_Msk               (_U_(0x1) << DAC_EVCTRL_STARTEI1_Pos)                /**< (DAC_EVCTRL) Start Conversion Event Input DAC 1 Mask */
#define DAC_EVCTRL_STARTEI1(value)            (DAC_EVCTRL_STARTEI1_Msk & ((value) << DAC_EVCTRL_STARTEI1_Pos))
#define DAC_EVCTRL_EMPTYEO0_Pos               _U_(2)                                               /**< (DAC_EVCTRL) Data Buffer Empty Event Output DAC 0 Position */
#define DAC_EVCTRL_EMPTYEO0_Msk               (_U_(0x1) << DAC_EVCTRL_EMPTYEO0_Pos)                /**< (DAC_EVCTRL) Data Buffer Empty Event Output DAC 0 Mask */
#define DAC_EVCTRL_EMPTYEO0(value)            (DAC_EVCTRL_EMPTYEO0_Msk & ((value) << DAC_EVCTRL_EMPTYEO0_Pos))
#define DAC_EVCTRL_EMPTYEO1_Pos               _U_(3)                                               /**< (DAC_EVCTRL) Data Buffer Empty Event Output DAC 1 Position */
#define DAC_EVCTRL_EMPTYEO1_Msk               (_U_(0x1) << DAC_EVCTRL_EMPTYEO1_Pos)                /**< (DAC_EVCTRL) Data Buffer Empty Event Output DAC 1 Mask */
#define DAC_EVCTRL_EMPTYEO1(value)            (DAC_EVCTRL_EMPTYEO1_Msk & ((value) << DAC_EVCTRL_EMPTYEO1_Pos))
#define DAC_EVCTRL_INVEI0_Pos                 _U_(4)                                               /**< (DAC_EVCTRL) Enable Invertion of DAC 0 input event Position */
#define DAC_EVCTRL_INVEI0_Msk                 (_U_(0x1) << DAC_EVCTRL_INVEI0_Pos)                  /**< (DAC_EVCTRL) Enable Invertion of DAC 0 input event Mask */
#define DAC_EVCTRL_INVEI0(value)              (DAC_EVCTRL_INVEI0_Msk & ((value) << DAC_EVCTRL_INVEI0_Pos))
#define DAC_EVCTRL_INVEI1_Pos                 _U_(5)                                               /**< (DAC_EVCTRL) Enable Invertion of DAC 1 input event Position */
#define DAC_EVCTRL_INVEI1_Msk                 (_U_(0x1) << DAC_EVCTRL_INVEI1_Pos)                  /**< (DAC_EVCTRL) Enable Invertion of DAC 1 input event Mask */
#define DAC_EVCTRL_INVEI1(value)              (DAC_EVCTRL_INVEI1_Msk & ((value) << DAC_EVCTRL_INVEI1_Pos))
#define DAC_EVCTRL_RESRDYEO0_Pos              _U_(6)                                               /**< (DAC_EVCTRL) Result Ready Event Output 0 Position */
#define DAC_EVCTRL_RESRDYEO0_Msk              (_U_(0x1) << DAC_EVCTRL_RESRDYEO0_Pos)               /**< (DAC_EVCTRL) Result Ready Event Output 0 Mask */
#define DAC_EVCTRL_RESRDYEO0(value)           (DAC_EVCTRL_RESRDYEO0_Msk & ((value) << DAC_EVCTRL_RESRDYEO0_Pos))
#define DAC_EVCTRL_RESRDYEO1_Pos              _U_(7)                                               /**< (DAC_EVCTRL) Result Ready Event Output 1 Position */
#define DAC_EVCTRL_RESRDYEO1_Msk              (_U_(0x1) << DAC_EVCTRL_RESRDYEO1_Pos)               /**< (DAC_EVCTRL) Result Ready Event Output 1 Mask */
#define DAC_EVCTRL_RESRDYEO1(value)           (DAC_EVCTRL_RESRDYEO1_Msk & ((value) << DAC_EVCTRL_RESRDYEO1_Pos))
#define DAC_EVCTRL_Msk                        _U_(0xFF)                                            /**< (DAC_EVCTRL) Register Mask  */

#define DAC_EVCTRL_STARTEI_Pos                _U_(0)                                               /**< (DAC_EVCTRL Position) Start Conversion Event Input DAC x */
#define DAC_EVCTRL_STARTEI_Msk                (_U_(0x3) << DAC_EVCTRL_STARTEI_Pos)                 /**< (DAC_EVCTRL Mask) STARTEI */
#define DAC_EVCTRL_STARTEI(value)             (DAC_EVCTRL_STARTEI_Msk & ((value) << DAC_EVCTRL_STARTEI_Pos)) 
#define DAC_EVCTRL_EMPTYEO_Pos                _U_(2)                                               /**< (DAC_EVCTRL Position) Data Buffer Empty Event Output DAC x */
#define DAC_EVCTRL_EMPTYEO_Msk                (_U_(0x3) << DAC_EVCTRL_EMPTYEO_Pos)                 /**< (DAC_EVCTRL Mask) EMPTYEO */
#define DAC_EVCTRL_EMPTYEO(value)             (DAC_EVCTRL_EMPTYEO_Msk & ((value) << DAC_EVCTRL_EMPTYEO_Pos)) 
#define DAC_EVCTRL_INVEI_Pos                  _U_(4)                                               /**< (DAC_EVCTRL Position) Enable Invertion of DAC x input event */
#define DAC_EVCTRL_INVEI_Msk                  (_U_(0x3) << DAC_EVCTRL_INVEI_Pos)                   /**< (DAC_EVCTRL Mask) INVEI */
#define DAC_EVCTRL_INVEI(value)               (DAC_EVCTRL_INVEI_Msk & ((value) << DAC_EVCTRL_INVEI_Pos)) 
#define DAC_EVCTRL_RESRDYEO_Pos               _U_(6)                                               /**< (DAC_EVCTRL Position) Result Ready Event Output x */
#define DAC_EVCTRL_RESRDYEO_Msk               (_U_(0x3) << DAC_EVCTRL_RESRDYEO_Pos)                /**< (DAC_EVCTRL Mask) RESRDYEO */
#define DAC_EVCTRL_RESRDYEO(value)            (DAC_EVCTRL_RESRDYEO_Msk & ((value) << DAC_EVCTRL_RESRDYEO_Pos)) 

/* -------- DAC_INTENCLR : (DAC Offset: 0x04) (R/W 8) Interrupt Enable Clear -------- */
#define DAC_INTENCLR_RESETVALUE               _U_(0x00)                                            /**<  (DAC_INTENCLR) Interrupt Enable Clear  Reset Value */

#define DAC_INTENCLR_UNDERRUN0_Pos            _U_(0)                                               /**< (DAC_INTENCLR) Underrun 0 Interrupt Enable Position */
#define DAC_INTENCLR_UNDERRUN0_Msk            (_U_(0x1) << DAC_INTENCLR_UNDERRUN0_Pos)             /**< (DAC_INTENCLR) Underrun 0 Interrupt Enable Mask */
#define DAC_INTENCLR_UNDERRUN0(value)         (DAC_INTENCLR_UNDERRUN0_Msk & ((value) << DAC_INTENCLR_UNDERRUN0_Pos))
#define DAC_INTENCLR_UNDERRUN1_Pos            _U_(1)                                               /**< (DAC_INTENCLR) Underrun 1 Interrupt Enable Position */
#define DAC_INTENCLR_UNDERRUN1_Msk            (_U_(0x1) << DAC_INTENCLR_UNDERRUN1_Pos)             /**< (DAC_INTENCLR) Underrun 1 Interrupt Enable Mask */
#define DAC_INTENCLR_UNDERRUN1(value)         (DAC_INTENCLR_UNDERRUN1_Msk & ((value) << DAC_INTENCLR_UNDERRUN1_Pos))
#define DAC_INTENCLR_EMPTY0_Pos               _U_(2)                                               /**< (DAC_INTENCLR) Data Buffer 0 Empty Interrupt Enable Position */
#define DAC_INTENCLR_EMPTY0_Msk               (_U_(0x1) << DAC_INTENCLR_EMPTY0_Pos)                /**< (DAC_INTENCLR) Data Buffer 0 Empty Interrupt Enable Mask */
#define DAC_INTENCLR_EMPTY0(value)            (DAC_INTENCLR_EMPTY0_Msk & ((value) << DAC_INTENCLR_EMPTY0_Pos))
#define DAC_INTENCLR_EMPTY1_Pos               _U_(3)                                               /**< (DAC_INTENCLR) Data Buffer 1 Empty Interrupt Enable Position */
#define DAC_INTENCLR_EMPTY1_Msk               (_U_(0x1) << DAC_INTENCLR_EMPTY1_Pos)                /**< (DAC_INTENCLR) Data Buffer 1 Empty Interrupt Enable Mask */
#define DAC_INTENCLR_EMPTY1(value)            (DAC_INTENCLR_EMPTY1_Msk & ((value) << DAC_INTENCLR_EMPTY1_Pos))
#define DAC_INTENCLR_RESRDY0_Pos              _U_(4)                                               /**< (DAC_INTENCLR) Result 0 Ready Interrupt Enable Position */
#define DAC_INTENCLR_RESRDY0_Msk              (_U_(0x1) << DAC_INTENCLR_RESRDY0_Pos)               /**< (DAC_INTENCLR) Result 0 Ready Interrupt Enable Mask */
#define DAC_INTENCLR_RESRDY0(value)           (DAC_INTENCLR_RESRDY0_Msk & ((value) << DAC_INTENCLR_RESRDY0_Pos))
#define DAC_INTENCLR_RESRDY1_Pos              _U_(5)                                               /**< (DAC_INTENCLR) Result 1 Ready Interrupt Enable Position */
#define DAC_INTENCLR_RESRDY1_Msk              (_U_(0x1) << DAC_INTENCLR_RESRDY1_Pos)               /**< (DAC_INTENCLR) Result 1 Ready Interrupt Enable Mask */
#define DAC_INTENCLR_RESRDY1(value)           (DAC_INTENCLR_RESRDY1_Msk & ((value) << DAC_INTENCLR_RESRDY1_Pos))
#define DAC_INTENCLR_OVERRUN0_Pos             _U_(6)                                               /**< (DAC_INTENCLR) Overrun 0 Interrupt Enable Position */
#define DAC_INTENCLR_OVERRUN0_Msk             (_U_(0x1) << DAC_INTENCLR_OVERRUN0_Pos)              /**< (DAC_INTENCLR) Overrun 0 Interrupt Enable Mask */
#define DAC_INTENCLR_OVERRUN0(value)          (DAC_INTENCLR_OVERRUN0_Msk & ((value) << DAC_INTENCLR_OVERRUN0_Pos))
#define DAC_INTENCLR_OVERRUN1_Pos             _U_(7)                                               /**< (DAC_INTENCLR) Overrun 1 Interrupt Enable Position */
#define DAC_INTENCLR_OVERRUN1_Msk             (_U_(0x1) << DAC_INTENCLR_OVERRUN1_Pos)              /**< (DAC_INTENCLR) Overrun 1 Interrupt Enable Mask */
#define DAC_INTENCLR_OVERRUN1(value)          (DAC_INTENCLR_OVERRUN1_Msk & ((value) << DAC_INTENCLR_OVERRUN1_Pos))
#define DAC_INTENCLR_Msk                      _U_(0xFF)                                            /**< (DAC_INTENCLR) Register Mask  */

#define DAC_INTENCLR_UNDERRUN_Pos             _U_(0)                                               /**< (DAC_INTENCLR Position) Underrun x Interrupt Enable */
#define DAC_INTENCLR_UNDERRUN_Msk             (_U_(0x3) << DAC_INTENCLR_UNDERRUN_Pos)              /**< (DAC_INTENCLR Mask) UNDERRUN */
#define DAC_INTENCLR_UNDERRUN(value)          (DAC_INTENCLR_UNDERRUN_Msk & ((value) << DAC_INTENCLR_UNDERRUN_Pos)) 
#define DAC_INTENCLR_EMPTY_Pos                _U_(2)                                               /**< (DAC_INTENCLR Position) Data Buffer x Empty Interrupt Enable */
#define DAC_INTENCLR_EMPTY_Msk                (_U_(0x3) << DAC_INTENCLR_EMPTY_Pos)                 /**< (DAC_INTENCLR Mask) EMPTY */
#define DAC_INTENCLR_EMPTY(value)             (DAC_INTENCLR_EMPTY_Msk & ((value) << DAC_INTENCLR_EMPTY_Pos)) 
#define DAC_INTENCLR_RESRDY_Pos               _U_(4)                                               /**< (DAC_INTENCLR Position) Result x Ready Interrupt Enable */
#define DAC_INTENCLR_RESRDY_Msk               (_U_(0x3) << DAC_INTENCLR_RESRDY_Pos)                /**< (DAC_INTENCLR Mask) RESRDY */
#define DAC_INTENCLR_RESRDY(value)            (DAC_INTENCLR_RESRDY_Msk & ((value) << DAC_INTENCLR_RESRDY_Pos)) 
#define DAC_INTENCLR_OVERRUN_Pos              _U_(6)                                               /**< (DAC_INTENCLR Position) Overrun x Interrupt Enable */
#define DAC_INTENCLR_OVERRUN_Msk              (_U_(0x3) << DAC_INTENCLR_OVERRUN_Pos)               /**< (DAC_INTENCLR Mask) OVERRUN */
#define DAC_INTENCLR_OVERRUN(value)           (DAC_INTENCLR_OVERRUN_Msk & ((value) << DAC_INTENCLR_OVERRUN_Pos)) 

/* -------- DAC_INTENSET : (DAC Offset: 0x05) (R/W 8) Interrupt Enable Set -------- */
#define DAC_INTENSET_RESETVALUE               _U_(0x00)                                            /**<  (DAC_INTENSET) Interrupt Enable Set  Reset Value */

#define DAC_INTENSET_UNDERRUN0_Pos            _U_(0)                                               /**< (DAC_INTENSET) Underrun 0 Interrupt Enable Position */
#define DAC_INTENSET_UNDERRUN0_Msk            (_U_(0x1) << DAC_INTENSET_UNDERRUN0_Pos)             /**< (DAC_INTENSET) Underrun 0 Interrupt Enable Mask */
#define DAC_INTENSET_UNDERRUN0(value)         (DAC_INTENSET_UNDERRUN0_Msk & ((value) << DAC_INTENSET_UNDERRUN0_Pos))
#define DAC_INTENSET_UNDERRUN1_Pos            _U_(1)                                               /**< (DAC_INTENSET) Underrun 1 Interrupt Enable Position */
#define DAC_INTENSET_UNDERRUN1_Msk            (_U_(0x1) << DAC_INTENSET_UNDERRUN1_Pos)             /**< (DAC_INTENSET) Underrun 1 Interrupt Enable Mask */
#define DAC_INTENSET_UNDERRUN1(value)         (DAC_INTENSET_UNDERRUN1_Msk & ((value) << DAC_INTENSET_UNDERRUN1_Pos))
#define DAC_INTENSET_EMPTY0_Pos               _U_(2)                                               /**< (DAC_INTENSET) Data Buffer 0 Empty Interrupt Enable Position */
#define DAC_INTENSET_EMPTY0_Msk               (_U_(0x1) << DAC_INTENSET_EMPTY0_Pos)                /**< (DAC_INTENSET) Data Buffer 0 Empty Interrupt Enable Mask */
#define DAC_INTENSET_EMPTY0(value)            (DAC_INTENSET_EMPTY0_Msk & ((value) << DAC_INTENSET_EMPTY0_Pos))
#define DAC_INTENSET_EMPTY1_Pos               _U_(3)                                               /**< (DAC_INTENSET) Data Buffer 1 Empty Interrupt Enable Position */
#define DAC_INTENSET_EMPTY1_Msk               (_U_(0x1) << DAC_INTENSET_EMPTY1_Pos)                /**< (DAC_INTENSET) Data Buffer 1 Empty Interrupt Enable Mask */
#define DAC_INTENSET_EMPTY1(value)            (DAC_INTENSET_EMPTY1_Msk & ((value) << DAC_INTENSET_EMPTY1_Pos))
#define DAC_INTENSET_RESRDY0_Pos              _U_(4)                                               /**< (DAC_INTENSET) Result 0 Ready Interrupt Enable Position */
#define DAC_INTENSET_RESRDY0_Msk              (_U_(0x1) << DAC_INTENSET_RESRDY0_Pos)               /**< (DAC_INTENSET) Result 0 Ready Interrupt Enable Mask */
#define DAC_INTENSET_RESRDY0(value)           (DAC_INTENSET_RESRDY0_Msk & ((value) << DAC_INTENSET_RESRDY0_Pos))
#define DAC_INTENSET_RESRDY1_Pos              _U_(5)                                               /**< (DAC_INTENSET) Result 1 Ready Interrupt Enable Position */
#define DAC_INTENSET_RESRDY1_Msk              (_U_(0x1) << DAC_INTENSET_RESRDY1_Pos)               /**< (DAC_INTENSET) Result 1 Ready Interrupt Enable Mask */
#define DAC_INTENSET_RESRDY1(value)           (DAC_INTENSET_RESRDY1_Msk & ((value) << DAC_INTENSET_RESRDY1_Pos))
#define DAC_INTENSET_OVERRUN0_Pos             _U_(6)                                               /**< (DAC_INTENSET) Overrun 0 Interrupt Enable Position */
#define DAC_INTENSET_OVERRUN0_Msk             (_U_(0x1) << DAC_INTENSET_OVERRUN0_Pos)              /**< (DAC_INTENSET) Overrun 0 Interrupt Enable Mask */
#define DAC_INTENSET_OVERRUN0(value)          (DAC_INTENSET_OVERRUN0_Msk & ((value) << DAC_INTENSET_OVERRUN0_Pos))
#define DAC_INTENSET_OVERRUN1_Pos             _U_(7)                                               /**< (DAC_INTENSET) Overrun 1 Interrupt Enable Position */
#define DAC_INTENSET_OVERRUN1_Msk             (_U_(0x1) << DAC_INTENSET_OVERRUN1_Pos)              /**< (DAC_INTENSET) Overrun 1 Interrupt Enable Mask */
#define DAC_INTENSET_OVERRUN1(value)          (DAC_INTENSET_OVERRUN1_Msk & ((value) << DAC_INTENSET_OVERRUN1_Pos))
#define DAC_INTENSET_Msk                      _U_(0xFF)                                            /**< (DAC_INTENSET) Register Mask  */

#define DAC_INTENSET_UNDERRUN_Pos             _U_(0)                                               /**< (DAC_INTENSET Position) Underrun x Interrupt Enable */
#define DAC_INTENSET_UNDERRUN_Msk             (_U_(0x3) << DAC_INTENSET_UNDERRUN_Pos)              /**< (DAC_INTENSET Mask) UNDERRUN */
#define DAC_INTENSET_UNDERRUN(value)          (DAC_INTENSET_UNDERRUN_Msk & ((value) << DAC_INTENSET_UNDERRUN_Pos)) 
#define DAC_INTENSET_EMPTY_Pos                _U_(2)                                               /**< (DAC_INTENSET Position) Data Buffer x Empty Interrupt Enable */
#define DAC_INTENSET_EMPTY_Msk                (_U_(0x3) << DAC_INTENSET_EMPTY_Pos)                 /**< (DAC_INTENSET Mask) EMPTY */
#define DAC_INTENSET_EMPTY(value)             (DAC_INTENSET_EMPTY_Msk & ((value) << DAC_INTENSET_EMPTY_Pos)) 
#define DAC_INTENSET_RESRDY_Pos               _U_(4)                                               /**< (DAC_INTENSET Position) Result x Ready Interrupt Enable */
#define DAC_INTENSET_RESRDY_Msk               (_U_(0x3) << DAC_INTENSET_RESRDY_Pos)                /**< (DAC_INTENSET Mask) RESRDY */
#define DAC_INTENSET_RESRDY(value)            (DAC_INTENSET_RESRDY_Msk & ((value) << DAC_INTENSET_RESRDY_Pos)) 
#define DAC_INTENSET_OVERRUN_Pos              _U_(6)                                               /**< (DAC_INTENSET Position) Overrun x Interrupt Enable */
#define DAC_INTENSET_OVERRUN_Msk              (_U_(0x3) << DAC_INTENSET_OVERRUN_Pos)               /**< (DAC_INTENSET Mask) OVERRUN */
#define DAC_INTENSET_OVERRUN(value)           (DAC_INTENSET_OVERRUN_Msk & ((value) << DAC_INTENSET_OVERRUN_Pos)) 

/* -------- DAC_INTFLAG : (DAC Offset: 0x06) (R/W 8) Interrupt Flag Status and Clear -------- */
#define DAC_INTFLAG_RESETVALUE                _U_(0x00)                                            /**<  (DAC_INTFLAG) Interrupt Flag Status and Clear  Reset Value */

#define DAC_INTFLAG_UNDERRUN0_Pos             _U_(0)                                               /**< (DAC_INTFLAG) Result 0 Underrun Position */
#define DAC_INTFLAG_UNDERRUN0_Msk             (_U_(0x1) << DAC_INTFLAG_UNDERRUN0_Pos)              /**< (DAC_INTFLAG) Result 0 Underrun Mask */
#define DAC_INTFLAG_UNDERRUN0(value)          (DAC_INTFLAG_UNDERRUN0_Msk & ((value) << DAC_INTFLAG_UNDERRUN0_Pos))
#define DAC_INTFLAG_UNDERRUN1_Pos             _U_(1)                                               /**< (DAC_INTFLAG) Result 1 Underrun Position */
#define DAC_INTFLAG_UNDERRUN1_Msk             (_U_(0x1) << DAC_INTFLAG_UNDERRUN1_Pos)              /**< (DAC_INTFLAG) Result 1 Underrun Mask */
#define DAC_INTFLAG_UNDERRUN1(value)          (DAC_INTFLAG_UNDERRUN1_Msk & ((value) << DAC_INTFLAG_UNDERRUN1_Pos))
#define DAC_INTFLAG_EMPTY0_Pos                _U_(2)                                               /**< (DAC_INTFLAG) Data Buffer 0 Empty Position */
#define DAC_INTFLAG_EMPTY0_Msk                (_U_(0x1) << DAC_INTFLAG_EMPTY0_Pos)                 /**< (DAC_INTFLAG) Data Buffer 0 Empty Mask */
#define DAC_INTFLAG_EMPTY0(value)             (DAC_INTFLAG_EMPTY0_Msk & ((value) << DAC_INTFLAG_EMPTY0_Pos))
#define DAC_INTFLAG_EMPTY1_Pos                _U_(3)                                               /**< (DAC_INTFLAG) Data Buffer 1 Empty Position */
#define DAC_INTFLAG_EMPTY1_Msk                (_U_(0x1) << DAC_INTFLAG_EMPTY1_Pos)                 /**< (DAC_INTFLAG) Data Buffer 1 Empty Mask */
#define DAC_INTFLAG_EMPTY1(value)             (DAC_INTFLAG_EMPTY1_Msk & ((value) << DAC_INTFLAG_EMPTY1_Pos))
#define DAC_INTFLAG_RESRDY0_Pos               _U_(4)                                               /**< (DAC_INTFLAG) Result 0 Ready Position */
#define DAC_INTFLAG_RESRDY0_Msk               (_U_(0x1) << DAC_INTFLAG_RESRDY0_Pos)                /**< (DAC_INTFLAG) Result 0 Ready Mask */
#define DAC_INTFLAG_RESRDY0(value)            (DAC_INTFLAG_RESRDY0_Msk & ((value) << DAC_INTFLAG_RESRDY0_Pos))
#define DAC_INTFLAG_RESRDY1_Pos               _U_(5)                                               /**< (DAC_INTFLAG) Result 1 Ready Position */
#define DAC_INTFLAG_RESRDY1_Msk               (_U_(0x1) << DAC_INTFLAG_RESRDY1_Pos)                /**< (DAC_INTFLAG) Result 1 Ready Mask */
#define DAC_INTFLAG_RESRDY1(value)            (DAC_INTFLAG_RESRDY1_Msk & ((value) << DAC_INTFLAG_RESRDY1_Pos))
#define DAC_INTFLAG_OVERRUN0_Pos              _U_(6)                                               /**< (DAC_INTFLAG) Result 0 Overrun Position */
#define DAC_INTFLAG_OVERRUN0_Msk              (_U_(0x1) << DAC_INTFLAG_OVERRUN0_Pos)               /**< (DAC_INTFLAG) Result 0 Overrun Mask */
#define DAC_INTFLAG_OVERRUN0(value)           (DAC_INTFLAG_OVERRUN0_Msk & ((value) << DAC_INTFLAG_OVERRUN0_Pos))
#define DAC_INTFLAG_OVERRUN1_Pos              _U_(7)                                               /**< (DAC_INTFLAG) Result 1 Overrun Position */
#define DAC_INTFLAG_OVERRUN1_Msk              (_U_(0x1) << DAC_INTFLAG_OVERRUN1_Pos)               /**< (DAC_INTFLAG) Result 1 Overrun Mask */
#define DAC_INTFLAG_OVERRUN1(value)           (DAC_INTFLAG_OVERRUN1_Msk & ((value) << DAC_INTFLAG_OVERRUN1_Pos))
#define DAC_INTFLAG_Msk                       _U_(0xFF)                                            /**< (DAC_INTFLAG) Register Mask  */

#define DAC_INTFLAG_UNDERRUN_Pos              _U_(0)                                               /**< (DAC_INTFLAG Position) Result x Underrun */
#define DAC_INTFLAG_UNDERRUN_Msk              (_U_(0x3) << DAC_INTFLAG_UNDERRUN_Pos)               /**< (DAC_INTFLAG Mask) UNDERRUN */
#define DAC_INTFLAG_UNDERRUN(value)           (DAC_INTFLAG_UNDERRUN_Msk & ((value) << DAC_INTFLAG_UNDERRUN_Pos)) 
#define DAC_INTFLAG_EMPTY_Pos                 _U_(2)                                               /**< (DAC_INTFLAG Position) Data Buffer x Empty */
#define DAC_INTFLAG_EMPTY_Msk                 (_U_(0x3) << DAC_INTFLAG_EMPTY_Pos)                  /**< (DAC_INTFLAG Mask) EMPTY */
#define DAC_INTFLAG_EMPTY(value)              (DAC_INTFLAG_EMPTY_Msk & ((value) << DAC_INTFLAG_EMPTY_Pos)) 
#define DAC_INTFLAG_RESRDY_Pos                _U_(4)                                               /**< (DAC_INTFLAG Position) Result x Ready */
#define DAC_INTFLAG_RESRDY_Msk                (_U_(0x3) << DAC_INTFLAG_RESRDY_Pos)                 /**< (DAC_INTFLAG Mask) RESRDY */
#define DAC_INTFLAG_RESRDY(value)             (DAC_INTFLAG_RESRDY_Msk & ((value) << DAC_INTFLAG_RESRDY_Pos)) 
#define DAC_INTFLAG_OVERRUN_Pos               _U_(6)                                               /**< (DAC_INTFLAG Position) Result x Overrun */
#define DAC_INTFLAG_OVERRUN_Msk               (_U_(0x3) << DAC_INTFLAG_OVERRUN_Pos)                /**< (DAC_INTFLAG Mask) OVERRUN */
#define DAC_INTFLAG_OVERRUN(value)            (DAC_INTFLAG_OVERRUN_Msk & ((value) << DAC_INTFLAG_OVERRUN_Pos)) 

/* -------- DAC_STATUS : (DAC Offset: 0x07) ( R/ 8) Status -------- */
#define DAC_STATUS_RESETVALUE                 _U_(0x00)                                            /**<  (DAC_STATUS) Status  Reset Value */

#define DAC_STATUS_READY0_Pos                 _U_(0)                                               /**< (DAC_STATUS) DAC 0 Startup Ready Position */
#define DAC_STATUS_READY0_Msk                 (_U_(0x1) << DAC_STATUS_READY0_Pos)                  /**< (DAC_STATUS) DAC 0 Startup Ready Mask */
#define DAC_STATUS_READY0(value)              (DAC_STATUS_READY0_Msk & ((value) << DAC_STATUS_READY0_Pos))
#define DAC_STATUS_READY1_Pos                 _U_(1)                                               /**< (DAC_STATUS) DAC 1 Startup Ready Position */
#define DAC_STATUS_READY1_Msk                 (_U_(0x1) << DAC_STATUS_READY1_Pos)                  /**< (DAC_STATUS) DAC 1 Startup Ready Mask */
#define DAC_STATUS_READY1(value)              (DAC_STATUS_READY1_Msk & ((value) << DAC_STATUS_READY1_Pos))
#define DAC_STATUS_EOC0_Pos                   _U_(2)                                               /**< (DAC_STATUS) DAC 0 End of Conversion Position */
#define DAC_STATUS_EOC0_Msk                   (_U_(0x1) << DAC_STATUS_EOC0_Pos)                    /**< (DAC_STATUS) DAC 0 End of Conversion Mask */
#define DAC_STATUS_EOC0(value)                (DAC_STATUS_EOC0_Msk & ((value) << DAC_STATUS_EOC0_Pos))
#define DAC_STATUS_EOC1_Pos                   _U_(3)                                               /**< (DAC_STATUS) DAC 1 End of Conversion Position */
#define DAC_STATUS_EOC1_Msk                   (_U_(0x1) << DAC_STATUS_EOC1_Pos)                    /**< (DAC_STATUS) DAC 1 End of Conversion Mask */
#define DAC_STATUS_EOC1(value)                (DAC_STATUS_EOC1_Msk & ((value) << DAC_STATUS_EOC1_Pos))
#define DAC_STATUS_Msk                        _U_(0x0F)                                            /**< (DAC_STATUS) Register Mask  */

#define DAC_STATUS_READY_Pos                  _U_(0)                                               /**< (DAC_STATUS Position) DAC x Startup Ready */
#define DAC_STATUS_READY_Msk                  (_U_(0x3) << DAC_STATUS_READY_Pos)                   /**< (DAC_STATUS Mask) READY */
#define DAC_STATUS_READY(value)               (DAC_STATUS_READY_Msk & ((value) << DAC_STATUS_READY_Pos)) 
#define DAC_STATUS_EOC_Pos                    _U_(2)                                               /**< (DAC_STATUS Position) DAC x End of Conversion */
#define DAC_STATUS_EOC_Msk                    (_U_(0x3) << DAC_STATUS_EOC_Pos)                     /**< (DAC_STATUS Mask) EOC */
#define DAC_STATUS_EOC(value)                 (DAC_STATUS_EOC_Msk & ((value) << DAC_STATUS_EOC_Pos)) 

/* -------- DAC_SYNCBUSY : (DAC Offset: 0x08) ( R/ 32) Synchronization Busy -------- */
#define DAC_SYNCBUSY_RESETVALUE               _U_(0x00)                                            /**<  (DAC_SYNCBUSY) Synchronization Busy  Reset Value */

#define DAC_SYNCBUSY_SWRST_Pos                _U_(0)                                               /**< (DAC_SYNCBUSY) Software Reset Position */
#define DAC_SYNCBUSY_SWRST_Msk                (_U_(0x1) << DAC_SYNCBUSY_SWRST_Pos)                 /**< (DAC_SYNCBUSY) Software Reset Mask */
#define DAC_SYNCBUSY_SWRST(value)             (DAC_SYNCBUSY_SWRST_Msk & ((value) << DAC_SYNCBUSY_SWRST_Pos))
#define DAC_SYNCBUSY_ENABLE_Pos               _U_(1)                                               /**< (DAC_SYNCBUSY) DAC Enable Status Position */
#define DAC_SYNCBUSY_ENABLE_Msk               (_U_(0x1) << DAC_SYNCBUSY_ENABLE_Pos)                /**< (DAC_SYNCBUSY) DAC Enable Status Mask */
#define DAC_SYNCBUSY_ENABLE(value)            (DAC_SYNCBUSY_ENABLE_Msk & ((value) << DAC_SYNCBUSY_ENABLE_Pos))
#define DAC_SYNCBUSY_DATA0_Pos                _U_(2)                                               /**< (DAC_SYNCBUSY) Data DAC 0 Position */
#define DAC_SYNCBUSY_DATA0_Msk                (_U_(0x1) << DAC_SYNCBUSY_DATA0_Pos)                 /**< (DAC_SYNCBUSY) Data DAC 0 Mask */
#define DAC_SYNCBUSY_DATA0(value)             (DAC_SYNCBUSY_DATA0_Msk & ((value) << DAC_SYNCBUSY_DATA0_Pos))
#define DAC_SYNCBUSY_DATA1_Pos                _U_(3)                                               /**< (DAC_SYNCBUSY) Data DAC 1 Position */
#define DAC_SYNCBUSY_DATA1_Msk                (_U_(0x1) << DAC_SYNCBUSY_DATA1_Pos)                 /**< (DAC_SYNCBUSY) Data DAC 1 Mask */
#define DAC_SYNCBUSY_DATA1(value)             (DAC_SYNCBUSY_DATA1_Msk & ((value) << DAC_SYNCBUSY_DATA1_Pos))
#define DAC_SYNCBUSY_DATABUF0_Pos             _U_(4)                                               /**< (DAC_SYNCBUSY) Data Buffer DAC 0 Position */
#define DAC_SYNCBUSY_DATABUF0_Msk             (_U_(0x1) << DAC_SYNCBUSY_DATABUF0_Pos)              /**< (DAC_SYNCBUSY) Data Buffer DAC 0 Mask */
#define DAC_SYNCBUSY_DATABUF0(value)          (DAC_SYNCBUSY_DATABUF0_Msk & ((value) << DAC_SYNCBUSY_DATABUF0_Pos))
#define DAC_SYNCBUSY_DATABUF1_Pos             _U_(5)                                               /**< (DAC_SYNCBUSY) Data Buffer DAC 1 Position */
#define DAC_SYNCBUSY_DATABUF1_Msk             (_U_(0x1) << DAC_SYNCBUSY_DATABUF1_Pos)              /**< (DAC_SYNCBUSY) Data Buffer DAC 1 Mask */
#define DAC_SYNCBUSY_DATABUF1(value)          (DAC_SYNCBUSY_DATABUF1_Msk & ((value) << DAC_SYNCBUSY_DATABUF1_Pos))
#define DAC_SYNCBUSY_Msk                      _U_(0x0000003F)                                      /**< (DAC_SYNCBUSY) Register Mask  */

#define DAC_SYNCBUSY_DATA_Pos                 _U_(2)                                               /**< (DAC_SYNCBUSY Position) Data DAC x */
#define DAC_SYNCBUSY_DATA_Msk                 (_U_(0x3) << DAC_SYNCBUSY_DATA_Pos)                  /**< (DAC_SYNCBUSY Mask) DATA */
#define DAC_SYNCBUSY_DATA(value)              (DAC_SYNCBUSY_DATA_Msk & ((value) << DAC_SYNCBUSY_DATA_Pos)) 
#define DAC_SYNCBUSY_DATABUF_Pos              _U_(4)                                               /**< (DAC_SYNCBUSY Position) Data Buffer DAC x */
#define DAC_SYNCBUSY_DATABUF_Msk              (_U_(0x3) << DAC_SYNCBUSY_DATABUF_Pos)               /**< (DAC_SYNCBUSY Mask) DATABUF */
#define DAC_SYNCBUSY_DATABUF(value)           (DAC_SYNCBUSY_DATABUF_Msk & ((value) << DAC_SYNCBUSY_DATABUF_Pos)) 

/* -------- DAC_DACCTRL : (DAC Offset: 0x0C) (R/W 16) DAC n Control -------- */
#define DAC_DACCTRL_RESETVALUE                _U_(0x00)                                            /**<  (DAC_DACCTRL) DAC n Control  Reset Value */

#define DAC_DACCTRL_LEFTADJ_Pos               _U_(0)                                               /**< (DAC_DACCTRL) Left Adjusted Data Position */
#define DAC_DACCTRL_LEFTADJ_Msk               (_U_(0x1) << DAC_DACCTRL_LEFTADJ_Pos)                /**< (DAC_DACCTRL) Left Adjusted Data Mask */
#define DAC_DACCTRL_LEFTADJ(value)            (DAC_DACCTRL_LEFTADJ_Msk & ((value) << DAC_DACCTRL_LEFTADJ_Pos))
#define DAC_DACCTRL_ENABLE_Pos                _U_(1)                                               /**< (DAC_DACCTRL) Enable DAC0 Position */
#define DAC_DACCTRL_ENABLE_Msk                (_U_(0x1) << DAC_DACCTRL_ENABLE_Pos)                 /**< (DAC_DACCTRL) Enable DAC0 Mask */
#define DAC_DACCTRL_ENABLE(value)             (DAC_DACCTRL_ENABLE_Msk & ((value) << DAC_DACCTRL_ENABLE_Pos))
#define DAC_DACCTRL_CCTRL_Pos                 _U_(2)                                               /**< (DAC_DACCTRL) Current Control Position */
#define DAC_DACCTRL_CCTRL_Msk                 (_U_(0x3) << DAC_DACCTRL_CCTRL_Pos)                  /**< (DAC_DACCTRL) Current Control Mask */
#define DAC_DACCTRL_CCTRL(value)              (DAC_DACCTRL_CCTRL_Msk & ((value) << DAC_DACCTRL_CCTRL_Pos))
#define   DAC_DACCTRL_CCTRL_CC100K_Val        _U_(0x0)                                             /**< (DAC_DACCTRL) 100kSPS  */
#define   DAC_DACCTRL_CCTRL_CC1M_Val          _U_(0x1)                                             /**< (DAC_DACCTRL) 500kSPS  */
#define   DAC_DACCTRL_CCTRL_CC12M_Val         _U_(0x2)                                             /**< (DAC_DACCTRL) 1MSPS  */
#define DAC_DACCTRL_CCTRL_CC100K              (DAC_DACCTRL_CCTRL_CC100K_Val << DAC_DACCTRL_CCTRL_Pos) /**< (DAC_DACCTRL) 100kSPS Position  */
#define DAC_DACCTRL_CCTRL_CC1M                (DAC_DACCTRL_CCTRL_CC1M_Val << DAC_DACCTRL_CCTRL_Pos) /**< (DAC_DACCTRL) 500kSPS Position  */
#define DAC_DACCTRL_CCTRL_CC12M               (DAC_DACCTRL_CCTRL_CC12M_Val << DAC_DACCTRL_CCTRL_Pos) /**< (DAC_DACCTRL) 1MSPS Position  */
#define DAC_DACCTRL_FEXT_Pos                  _U_(5)                                               /**< (DAC_DACCTRL) Standalone Filter Position */
#define DAC_DACCTRL_FEXT_Msk                  (_U_(0x1) << DAC_DACCTRL_FEXT_Pos)                   /**< (DAC_DACCTRL) Standalone Filter Mask */
#define DAC_DACCTRL_FEXT(value)               (DAC_DACCTRL_FEXT_Msk & ((value) << DAC_DACCTRL_FEXT_Pos))
#define DAC_DACCTRL_RUNSTDBY_Pos              _U_(6)                                               /**< (DAC_DACCTRL) Run in Standby Position */
#define DAC_DACCTRL_RUNSTDBY_Msk              (_U_(0x1) << DAC_DACCTRL_RUNSTDBY_Pos)               /**< (DAC_DACCTRL) Run in Standby Mask */
#define DAC_DACCTRL_RUNSTDBY(value)           (DAC_DACCTRL_RUNSTDBY_Msk & ((value) << DAC_DACCTRL_RUNSTDBY_Pos))
#define DAC_DACCTRL_DITHER_Pos                _U_(7)                                               /**< (DAC_DACCTRL) Dithering Mode Position */
#define DAC_DACCTRL_DITHER_Msk                (_U_(0x1) << DAC_DACCTRL_DITHER_Pos)                 /**< (DAC_DACCTRL) Dithering Mode Mask */
#define DAC_DACCTRL_DITHER(value)             (DAC_DACCTRL_DITHER_Msk & ((value) << DAC_DACCTRL_DITHER_Pos))
#define DAC_DACCTRL_REFRESH_Pos               _U_(8)                                               /**< (DAC_DACCTRL) Refresh period Position */
#define DAC_DACCTRL_REFRESH_Msk               (_U_(0xF) << DAC_DACCTRL_REFRESH_Pos)                /**< (DAC_DACCTRL) Refresh period Mask */
#define DAC_DACCTRL_REFRESH(value)            (DAC_DACCTRL_REFRESH_Msk & ((value) << DAC_DACCTRL_REFRESH_Pos))
#define   DAC_DACCTRL_REFRESH_REFRESH_0_Val   _U_(0x0)                                             /**< (DAC_DACCTRL) Do not Refresh  */
#define   DAC_DACCTRL_REFRESH_REFRESH_1_Val   _U_(0x1)                                             /**< (DAC_DACCTRL) Refresh every 30 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_2_Val   _U_(0x2)                                             /**< (DAC_DACCTRL) Refresh every 60 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_3_Val   _U_(0x3)                                             /**< (DAC_DACCTRL) Refresh every 90 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_4_Val   _U_(0x4)                                             /**< (DAC_DACCTRL) Refresh every 120 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_5_Val   _U_(0x5)                                             /**< (DAC_DACCTRL) Refresh every 150 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_6_Val   _U_(0x6)                                             /**< (DAC_DACCTRL) Refresh every 180 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_7_Val   _U_(0x7)                                             /**< (DAC_DACCTRL) Refresh every 210 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_8_Val   _U_(0x8)                                             /**< (DAC_DACCTRL) Refresh every 240 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_9_Val   _U_(0x9)                                             /**< (DAC_DACCTRL) Refresh every 270 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_10_Val  _U_(0xA)                                             /**< (DAC_DACCTRL) Refresh every 300 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_11_Val  _U_(0xB)                                             /**< (DAC_DACCTRL) Refresh every 330 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_12_Val  _U_(0xC)                                             /**< (DAC_DACCTRL) Refresh every 360 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_13_Val  _U_(0xD)                                             /**< (DAC_DACCTRL) Refresh every 390 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_14_Val  _U_(0xE)                                             /**< (DAC_DACCTRL) Refresh every 420 us  */
#define   DAC_DACCTRL_REFRESH_REFRESH_15_Val  _U_(0xF)                                             /**< (DAC_DACCTRL) Refresh every 450 us  */
#define DAC_DACCTRL_REFRESH_REFRESH_0         (DAC_DACCTRL_REFRESH_REFRESH_0_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Do not Refresh Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_1         (DAC_DACCTRL_REFRESH_REFRESH_1_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 30 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_2         (DAC_DACCTRL_REFRESH_REFRESH_2_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 60 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_3         (DAC_DACCTRL_REFRESH_REFRESH_3_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 90 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_4         (DAC_DACCTRL_REFRESH_REFRESH_4_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 120 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_5         (DAC_DACCTRL_REFRESH_REFRESH_5_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 150 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_6         (DAC_DACCTRL_REFRESH_REFRESH_6_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 180 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_7         (DAC_DACCTRL_REFRESH_REFRESH_7_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 210 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_8         (DAC_DACCTRL_REFRESH_REFRESH_8_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 240 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_9         (DAC_DACCTRL_REFRESH_REFRESH_9_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 270 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_10        (DAC_DACCTRL_REFRESH_REFRESH_10_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 300 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_11        (DAC_DACCTRL_REFRESH_REFRESH_11_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 330 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_12        (DAC_DACCTRL_REFRESH_REFRESH_12_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 360 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_13        (DAC_DACCTRL_REFRESH_REFRESH_13_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 390 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_14        (DAC_DACCTRL_REFRESH_REFRESH_14_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 420 us Position  */
#define DAC_DACCTRL_REFRESH_REFRESH_15        (DAC_DACCTRL_REFRESH_REFRESH_15_Val << DAC_DACCTRL_REFRESH_Pos) /**< (DAC_DACCTRL) Refresh every 450 us Position  */
#define DAC_DACCTRL_OSR_Pos                   _U_(13)                                              /**< (DAC_DACCTRL) Sampling Rate Position */
#define DAC_DACCTRL_OSR_Msk                   (_U_(0x7) << DAC_DACCTRL_OSR_Pos)                    /**< (DAC_DACCTRL) Sampling Rate Mask */
#define DAC_DACCTRL_OSR(value)                (DAC_DACCTRL_OSR_Msk & ((value) << DAC_DACCTRL_OSR_Pos))
#define   DAC_DACCTRL_OSR_OSR_1_Val           _U_(0x0)                                             /**< (DAC_DACCTRL) No Over Sampling  */
#define   DAC_DACCTRL_OSR_OSR_2_Val           _U_(0x1)                                             /**< (DAC_DACCTRL) 2x Over Sampling Ratio  */
#define   DAC_DACCTRL_OSR_OSR_4_Val           _U_(0x2)                                             /**< (DAC_DACCTRL) 4x Over Sampling Ratio  */
#define   DAC_DACCTRL_OSR_OSR_8_Val           _U_(0x3)                                             /**< (DAC_DACCTRL) 8x Over Sampling Ratio  */
#define   DAC_DACCTRL_OSR_OSR_16_Val          _U_(0x4)                                             /**< (DAC_DACCTRL) 16x Over Sampling Ratio  */
#define   DAC_DACCTRL_OSR_OSR_32_Val          _U_(0x5)                                             /**< (DAC_DACCTRL) 32x Over Sampling Ratio  */
#define DAC_DACCTRL_OSR_OSR_1                 (DAC_DACCTRL_OSR_OSR_1_Val << DAC_DACCTRL_OSR_Pos)   /**< (DAC_DACCTRL) No Over Sampling Position  */
#define DAC_DACCTRL_OSR_OSR_2                 (DAC_DACCTRL_OSR_OSR_2_Val << DAC_DACCTRL_OSR_Pos)   /**< (DAC_DACCTRL) 2x Over Sampling Ratio Position  */
#define DAC_DACCTRL_OSR_OSR_4                 (DAC_DACCTRL_OSR_OSR_4_Val << DAC_DACCTRL_OSR_Pos)   /**< (DAC_DACCTRL) 4x Over Sampling Ratio Position  */
#define DAC_DACCTRL_OSR_OSR_8                 (DAC_DACCTRL_OSR_OSR_8_Val << DAC_DACCTRL_OSR_Pos)   /**< (DAC_DACCTRL) 8x Over Sampling Ratio Position  */
#define DAC_DACCTRL_OSR_OSR_16                (DAC_DACCTRL_OSR_OSR_16_Val << DAC_DACCTRL_OSR_Pos)  /**< (DAC_DACCTRL) 16x Over Sampling Ratio Position  */
#define DAC_DACCTRL_OSR_OSR_32                (DAC_DACCTRL_OSR_OSR_32_Val << DAC_DACCTRL_OSR_Pos)  /**< (DAC_DACCTRL) 32x Over Sampling Ratio Position  */
#define DAC_DACCTRL_Msk                       _U_(0xEFEF)                                          /**< (DAC_DACCTRL) Register Mask  */


/* -------- DAC_DATA : (DAC Offset: 0x10) ( /W 16) DAC n Data -------- */
#define DAC_DATA_RESETVALUE                   _U_(0x00)                                            /**<  (DAC_DATA) DAC n Data  Reset Value */

#define DAC_DATA_DATA_Pos                     _U_(0)                                               /**< (DAC_DATA) DAC0 Data Position */
#define DAC_DATA_DATA_Msk                     (_U_(0xFFFF) << DAC_DATA_DATA_Pos)                   /**< (DAC_DATA) DAC0 Data Mask */
#define DAC_DATA_DATA(value)                  (DAC_DATA_DATA_Msk & ((value) << DAC_DATA_DATA_Pos))
#define DAC_DATA_Msk                          _U_(0xFFFF)                                          /**< (DAC_DATA) Register Mask  */


/* -------- DAC_DATABUF : (DAC Offset: 0x14) ( /W 16) DAC n Data Buffer -------- */
#define DAC_DATABUF_RESETVALUE                _U_(0x00)                                            /**<  (DAC_DATABUF) DAC n Data Buffer  Reset Value */

#define DAC_DATABUF_DATABUF_Pos               _U_(0)                                               /**< (DAC_DATABUF) DAC0 Data Buffer Position */
#define DAC_DATABUF_DATABUF_Msk               (_U_(0xFFFF) << DAC_DATABUF_DATABUF_Pos)             /**< (DAC_DATABUF) DAC0 Data Buffer Mask */
#define DAC_DATABUF_DATABUF(value)            (DAC_DATABUF_DATABUF_Msk & ((value) << DAC_DATABUF_DATABUF_Pos))
#define DAC_DATABUF_Msk                       _U_(0xFFFF)                                          /**< (DAC_DATABUF) Register Mask  */


/* -------- DAC_DBGCTRL : (DAC Offset: 0x18) (R/W 8) Debug Control -------- */
#define DAC_DBGCTRL_RESETVALUE                _U_(0x00)                                            /**<  (DAC_DBGCTRL) Debug Control  Reset Value */

#define DAC_DBGCTRL_DBGRUN_Pos                _U_(0)                                               /**< (DAC_DBGCTRL) Debug Run Position */
#define DAC_DBGCTRL_DBGRUN_Msk                (_U_(0x1) << DAC_DBGCTRL_DBGRUN_Pos)                 /**< (DAC_DBGCTRL) Debug Run Mask */
#define DAC_DBGCTRL_DBGRUN(value)             (DAC_DBGCTRL_DBGRUN_Msk & ((value) << DAC_DBGCTRL_DBGRUN_Pos))
#define DAC_DBGCTRL_Msk                       _U_(0x01)                                            /**< (DAC_DBGCTRL) Register Mask  */


/* -------- DAC_RESULT : (DAC Offset: 0x1C) ( R/ 16) Filter Result -------- */
#define DAC_RESULT_RESETVALUE                 _U_(0x00)                                            /**<  (DAC_RESULT) Filter Result  Reset Value */

#define DAC_RESULT_RESULT_Pos                 _U_(0)                                               /**< (DAC_RESULT) Filter Result Position */
#define DAC_RESULT_RESULT_Msk                 (_U_(0xFFFF) << DAC_RESULT_RESULT_Pos)               /**< (DAC_RESULT) Filter Result Mask */
#define DAC_RESULT_RESULT(value)              (DAC_RESULT_RESULT_Msk & ((value) << DAC_RESULT_RESULT_Pos))
#define DAC_RESULT_Msk                        _U_(0xFFFF)                                          /**< (DAC_RESULT) Register Mask  */


/** \brief DAC register offsets definitions */
#define DAC_CTRLA_REG_OFST             (0x00)              /**< (DAC_CTRLA) Control A Offset */
#define DAC_CTRLB_REG_OFST             (0x01)              /**< (DAC_CTRLB) Control B Offset */
#define DAC_EVCTRL_REG_OFST            (0x02)              /**< (DAC_EVCTRL) Event Control Offset */
#define DAC_INTENCLR_REG_OFST          (0x04)              /**< (DAC_INTENCLR) Interrupt Enable Clear Offset */
#define DAC_INTENSET_REG_OFST          (0x05)              /**< (DAC_INTENSET) Interrupt Enable Set Offset */
#define DAC_INTFLAG_REG_OFST           (0x06)              /**< (DAC_INTFLAG) Interrupt Flag Status and Clear Offset */
#define DAC_STATUS_REG_OFST            (0x07)              /**< (DAC_STATUS) Status Offset */
#define DAC_SYNCBUSY_REG_OFST          (0x08)              /**< (DAC_SYNCBUSY) Synchronization Busy Offset */
#define DAC_DACCTRL_REG_OFST           (0x0C)              /**< (DAC_DACCTRL) DAC n Control Offset */
#define DAC_DATA_REG_OFST              (0x10)              /**< (DAC_DATA) DAC n Data Offset */
#define DAC_DATABUF_REG_OFST           (0x14)              /**< (DAC_DATABUF) DAC n Data Buffer Offset */
#define DAC_DBGCTRL_REG_OFST           (0x18)              /**< (DAC_DBGCTRL) Debug Control Offset */
#define DAC_RESULT_REG_OFST            (0x1C)              /**< (DAC_RESULT) Filter Result Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief DAC register API structure */
typedef struct
{  /* Digital-to-Analog Converter */
  __IO  uint8_t                        DAC_CTRLA;          /**< Offset: 0x00 (R/W  8) Control A */
  __IO  uint8_t                        DAC_CTRLB;          /**< Offset: 0x01 (R/W  8) Control B */
  __IO  uint8_t                        DAC_EVCTRL;         /**< Offset: 0x02 (R/W  8) Event Control */
  __I   uint8_t                        Reserved1[0x01];
  __IO  uint8_t                        DAC_INTENCLR;       /**< Offset: 0x04 (R/W  8) Interrupt Enable Clear */
  __IO  uint8_t                        DAC_INTENSET;       /**< Offset: 0x05 (R/W  8) Interrupt Enable Set */
  __IO  uint8_t                        DAC_INTFLAG;        /**< Offset: 0x06 (R/W  8) Interrupt Flag Status and Clear */
  __I   uint8_t                        DAC_STATUS;         /**< Offset: 0x07 (R/   8) Status */
  __I   uint32_t                       DAC_SYNCBUSY;       /**< Offset: 0x08 (R/   32) Synchronization Busy */
  __IO  uint16_t                       DAC_DACCTRL[2];     /**< Offset: 0x0C (R/W  16) DAC n Control */
  __O   uint16_t                       DAC_DATA[2];        /**< Offset: 0x10 ( /W  16) DAC n Data */
  __O   uint16_t                       DAC_DATABUF[2];     /**< Offset: 0x14 ( /W  16) DAC n Data Buffer */
  __IO  uint8_t                        DAC_DBGCTRL;        /**< Offset: 0x18 (R/W  8) Debug Control */
  __I   uint8_t                        Reserved2[0x03];
  __I   uint16_t                       DAC_RESULT[2];      /**< Offset: 0x1C (R/   16) Filter Result */
} dac_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_DAC_COMPONENT_H_ */
