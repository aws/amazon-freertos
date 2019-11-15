/**
 * \brief Component description for AC
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
#ifndef _SAME54_AC_COMPONENT_H_
#define _SAME54_AC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR AC                                           */
/* ************************************************************************** */

/* -------- AC_CTRLA : (AC Offset: 0x00) (R/W 8) Control A -------- */
#define AC_CTRLA_RESETVALUE                   _U_(0x00)                                            /**<  (AC_CTRLA) Control A  Reset Value */

#define AC_CTRLA_SWRST_Pos                    _U_(0)                                               /**< (AC_CTRLA) Software Reset Position */
#define AC_CTRLA_SWRST_Msk                    (_U_(0x1) << AC_CTRLA_SWRST_Pos)                     /**< (AC_CTRLA) Software Reset Mask */
#define AC_CTRLA_SWRST(value)                 (AC_CTRLA_SWRST_Msk & ((value) << AC_CTRLA_SWRST_Pos))
#define AC_CTRLA_ENABLE_Pos                   _U_(1)                                               /**< (AC_CTRLA) Enable Position */
#define AC_CTRLA_ENABLE_Msk                   (_U_(0x1) << AC_CTRLA_ENABLE_Pos)                    /**< (AC_CTRLA) Enable Mask */
#define AC_CTRLA_ENABLE(value)                (AC_CTRLA_ENABLE_Msk & ((value) << AC_CTRLA_ENABLE_Pos))
#define AC_CTRLA_Msk                          _U_(0x03)                                            /**< (AC_CTRLA) Register Mask  */


/* -------- AC_CTRLB : (AC Offset: 0x01) ( /W 8) Control B -------- */
#define AC_CTRLB_RESETVALUE                   _U_(0x00)                                            /**<  (AC_CTRLB) Control B  Reset Value */

#define AC_CTRLB_START0_Pos                   _U_(0)                                               /**< (AC_CTRLB) Comparator 0 Start Comparison Position */
#define AC_CTRLB_START0_Msk                   (_U_(0x1) << AC_CTRLB_START0_Pos)                    /**< (AC_CTRLB) Comparator 0 Start Comparison Mask */
#define AC_CTRLB_START0(value)                (AC_CTRLB_START0_Msk & ((value) << AC_CTRLB_START0_Pos))
#define AC_CTRLB_START1_Pos                   _U_(1)                                               /**< (AC_CTRLB) Comparator 1 Start Comparison Position */
#define AC_CTRLB_START1_Msk                   (_U_(0x1) << AC_CTRLB_START1_Pos)                    /**< (AC_CTRLB) Comparator 1 Start Comparison Mask */
#define AC_CTRLB_START1(value)                (AC_CTRLB_START1_Msk & ((value) << AC_CTRLB_START1_Pos))
#define AC_CTRLB_Msk                          _U_(0x03)                                            /**< (AC_CTRLB) Register Mask  */

#define AC_CTRLB_START_Pos                    _U_(0)                                               /**< (AC_CTRLB Position) Comparator x Start Comparison */
#define AC_CTRLB_START_Msk                    (_U_(0x3) << AC_CTRLB_START_Pos)                     /**< (AC_CTRLB Mask) START */
#define AC_CTRLB_START(value)                 (AC_CTRLB_START_Msk & ((value) << AC_CTRLB_START_Pos)) 

/* -------- AC_EVCTRL : (AC Offset: 0x02) (R/W 16) Event Control -------- */
#define AC_EVCTRL_RESETVALUE                  _U_(0x00)                                            /**<  (AC_EVCTRL) Event Control  Reset Value */

#define AC_EVCTRL_COMPEO0_Pos                 _U_(0)                                               /**< (AC_EVCTRL) Comparator 0 Event Output Enable Position */
#define AC_EVCTRL_COMPEO0_Msk                 (_U_(0x1) << AC_EVCTRL_COMPEO0_Pos)                  /**< (AC_EVCTRL) Comparator 0 Event Output Enable Mask */
#define AC_EVCTRL_COMPEO0(value)              (AC_EVCTRL_COMPEO0_Msk & ((value) << AC_EVCTRL_COMPEO0_Pos))
#define AC_EVCTRL_COMPEO1_Pos                 _U_(1)                                               /**< (AC_EVCTRL) Comparator 1 Event Output Enable Position */
#define AC_EVCTRL_COMPEO1_Msk                 (_U_(0x1) << AC_EVCTRL_COMPEO1_Pos)                  /**< (AC_EVCTRL) Comparator 1 Event Output Enable Mask */
#define AC_EVCTRL_COMPEO1(value)              (AC_EVCTRL_COMPEO1_Msk & ((value) << AC_EVCTRL_COMPEO1_Pos))
#define AC_EVCTRL_WINEO0_Pos                  _U_(4)                                               /**< (AC_EVCTRL) Window 0 Event Output Enable Position */
#define AC_EVCTRL_WINEO0_Msk                  (_U_(0x1) << AC_EVCTRL_WINEO0_Pos)                   /**< (AC_EVCTRL) Window 0 Event Output Enable Mask */
#define AC_EVCTRL_WINEO0(value)               (AC_EVCTRL_WINEO0_Msk & ((value) << AC_EVCTRL_WINEO0_Pos))
#define AC_EVCTRL_COMPEI0_Pos                 _U_(8)                                               /**< (AC_EVCTRL) Comparator 0 Event Input Enable Position */
#define AC_EVCTRL_COMPEI0_Msk                 (_U_(0x1) << AC_EVCTRL_COMPEI0_Pos)                  /**< (AC_EVCTRL) Comparator 0 Event Input Enable Mask */
#define AC_EVCTRL_COMPEI0(value)              (AC_EVCTRL_COMPEI0_Msk & ((value) << AC_EVCTRL_COMPEI0_Pos))
#define AC_EVCTRL_COMPEI1_Pos                 _U_(9)                                               /**< (AC_EVCTRL) Comparator 1 Event Input Enable Position */
#define AC_EVCTRL_COMPEI1_Msk                 (_U_(0x1) << AC_EVCTRL_COMPEI1_Pos)                  /**< (AC_EVCTRL) Comparator 1 Event Input Enable Mask */
#define AC_EVCTRL_COMPEI1(value)              (AC_EVCTRL_COMPEI1_Msk & ((value) << AC_EVCTRL_COMPEI1_Pos))
#define AC_EVCTRL_INVEI0_Pos                  _U_(12)                                              /**< (AC_EVCTRL) Comparator 0 Input Event Invert Enable Position */
#define AC_EVCTRL_INVEI0_Msk                  (_U_(0x1) << AC_EVCTRL_INVEI0_Pos)                   /**< (AC_EVCTRL) Comparator 0 Input Event Invert Enable Mask */
#define AC_EVCTRL_INVEI0(value)               (AC_EVCTRL_INVEI0_Msk & ((value) << AC_EVCTRL_INVEI0_Pos))
#define AC_EVCTRL_INVEI1_Pos                  _U_(13)                                              /**< (AC_EVCTRL) Comparator 1 Input Event Invert Enable Position */
#define AC_EVCTRL_INVEI1_Msk                  (_U_(0x1) << AC_EVCTRL_INVEI1_Pos)                   /**< (AC_EVCTRL) Comparator 1 Input Event Invert Enable Mask */
#define AC_EVCTRL_INVEI1(value)               (AC_EVCTRL_INVEI1_Msk & ((value) << AC_EVCTRL_INVEI1_Pos))
#define AC_EVCTRL_Msk                         _U_(0x3313)                                          /**< (AC_EVCTRL) Register Mask  */

#define AC_EVCTRL_COMPEO_Pos                  _U_(0)                                               /**< (AC_EVCTRL Position) Comparator x Event Output Enable */
#define AC_EVCTRL_COMPEO_Msk                  (_U_(0x3) << AC_EVCTRL_COMPEO_Pos)                   /**< (AC_EVCTRL Mask) COMPEO */
#define AC_EVCTRL_COMPEO(value)               (AC_EVCTRL_COMPEO_Msk & ((value) << AC_EVCTRL_COMPEO_Pos)) 
#define AC_EVCTRL_WINEO_Pos                   _U_(4)                                               /**< (AC_EVCTRL Position) Window x Event Output Enable */
#define AC_EVCTRL_WINEO_Msk                   (_U_(0x1) << AC_EVCTRL_WINEO_Pos)                    /**< (AC_EVCTRL Mask) WINEO */
#define AC_EVCTRL_WINEO(value)                (AC_EVCTRL_WINEO_Msk & ((value) << AC_EVCTRL_WINEO_Pos)) 
#define AC_EVCTRL_COMPEI_Pos                  _U_(8)                                               /**< (AC_EVCTRL Position) Comparator x Event Input Enable */
#define AC_EVCTRL_COMPEI_Msk                  (_U_(0x3) << AC_EVCTRL_COMPEI_Pos)                   /**< (AC_EVCTRL Mask) COMPEI */
#define AC_EVCTRL_COMPEI(value)               (AC_EVCTRL_COMPEI_Msk & ((value) << AC_EVCTRL_COMPEI_Pos)) 
#define AC_EVCTRL_INVEI_Pos                   _U_(12)                                              /**< (AC_EVCTRL Position) Comparator x Input Event Invert Enable */
#define AC_EVCTRL_INVEI_Msk                   (_U_(0x3) << AC_EVCTRL_INVEI_Pos)                    /**< (AC_EVCTRL Mask) INVEI */
#define AC_EVCTRL_INVEI(value)                (AC_EVCTRL_INVEI_Msk & ((value) << AC_EVCTRL_INVEI_Pos)) 

/* -------- AC_INTENCLR : (AC Offset: 0x04) (R/W 8) Interrupt Enable Clear -------- */
#define AC_INTENCLR_RESETVALUE                _U_(0x00)                                            /**<  (AC_INTENCLR) Interrupt Enable Clear  Reset Value */

#define AC_INTENCLR_COMP0_Pos                 _U_(0)                                               /**< (AC_INTENCLR) Comparator 0 Interrupt Enable Position */
#define AC_INTENCLR_COMP0_Msk                 (_U_(0x1) << AC_INTENCLR_COMP0_Pos)                  /**< (AC_INTENCLR) Comparator 0 Interrupt Enable Mask */
#define AC_INTENCLR_COMP0(value)              (AC_INTENCLR_COMP0_Msk & ((value) << AC_INTENCLR_COMP0_Pos))
#define AC_INTENCLR_COMP1_Pos                 _U_(1)                                               /**< (AC_INTENCLR) Comparator 1 Interrupt Enable Position */
#define AC_INTENCLR_COMP1_Msk                 (_U_(0x1) << AC_INTENCLR_COMP1_Pos)                  /**< (AC_INTENCLR) Comparator 1 Interrupt Enable Mask */
#define AC_INTENCLR_COMP1(value)              (AC_INTENCLR_COMP1_Msk & ((value) << AC_INTENCLR_COMP1_Pos))
#define AC_INTENCLR_WIN0_Pos                  _U_(4)                                               /**< (AC_INTENCLR) Window 0 Interrupt Enable Position */
#define AC_INTENCLR_WIN0_Msk                  (_U_(0x1) << AC_INTENCLR_WIN0_Pos)                   /**< (AC_INTENCLR) Window 0 Interrupt Enable Mask */
#define AC_INTENCLR_WIN0(value)               (AC_INTENCLR_WIN0_Msk & ((value) << AC_INTENCLR_WIN0_Pos))
#define AC_INTENCLR_Msk                       _U_(0x13)                                            /**< (AC_INTENCLR) Register Mask  */

#define AC_INTENCLR_COMP_Pos                  _U_(0)                                               /**< (AC_INTENCLR Position) Comparator x Interrupt Enable */
#define AC_INTENCLR_COMP_Msk                  (_U_(0x3) << AC_INTENCLR_COMP_Pos)                   /**< (AC_INTENCLR Mask) COMP */
#define AC_INTENCLR_COMP(value)               (AC_INTENCLR_COMP_Msk & ((value) << AC_INTENCLR_COMP_Pos)) 
#define AC_INTENCLR_WIN_Pos                   _U_(4)                                               /**< (AC_INTENCLR Position) Window x Interrupt Enable */
#define AC_INTENCLR_WIN_Msk                   (_U_(0x1) << AC_INTENCLR_WIN_Pos)                    /**< (AC_INTENCLR Mask) WIN */
#define AC_INTENCLR_WIN(value)                (AC_INTENCLR_WIN_Msk & ((value) << AC_INTENCLR_WIN_Pos)) 

/* -------- AC_INTENSET : (AC Offset: 0x05) (R/W 8) Interrupt Enable Set -------- */
#define AC_INTENSET_RESETVALUE                _U_(0x00)                                            /**<  (AC_INTENSET) Interrupt Enable Set  Reset Value */

#define AC_INTENSET_COMP0_Pos                 _U_(0)                                               /**< (AC_INTENSET) Comparator 0 Interrupt Enable Position */
#define AC_INTENSET_COMP0_Msk                 (_U_(0x1) << AC_INTENSET_COMP0_Pos)                  /**< (AC_INTENSET) Comparator 0 Interrupt Enable Mask */
#define AC_INTENSET_COMP0(value)              (AC_INTENSET_COMP0_Msk & ((value) << AC_INTENSET_COMP0_Pos))
#define AC_INTENSET_COMP1_Pos                 _U_(1)                                               /**< (AC_INTENSET) Comparator 1 Interrupt Enable Position */
#define AC_INTENSET_COMP1_Msk                 (_U_(0x1) << AC_INTENSET_COMP1_Pos)                  /**< (AC_INTENSET) Comparator 1 Interrupt Enable Mask */
#define AC_INTENSET_COMP1(value)              (AC_INTENSET_COMP1_Msk & ((value) << AC_INTENSET_COMP1_Pos))
#define AC_INTENSET_WIN0_Pos                  _U_(4)                                               /**< (AC_INTENSET) Window 0 Interrupt Enable Position */
#define AC_INTENSET_WIN0_Msk                  (_U_(0x1) << AC_INTENSET_WIN0_Pos)                   /**< (AC_INTENSET) Window 0 Interrupt Enable Mask */
#define AC_INTENSET_WIN0(value)               (AC_INTENSET_WIN0_Msk & ((value) << AC_INTENSET_WIN0_Pos))
#define AC_INTENSET_Msk                       _U_(0x13)                                            /**< (AC_INTENSET) Register Mask  */

#define AC_INTENSET_COMP_Pos                  _U_(0)                                               /**< (AC_INTENSET Position) Comparator x Interrupt Enable */
#define AC_INTENSET_COMP_Msk                  (_U_(0x3) << AC_INTENSET_COMP_Pos)                   /**< (AC_INTENSET Mask) COMP */
#define AC_INTENSET_COMP(value)               (AC_INTENSET_COMP_Msk & ((value) << AC_INTENSET_COMP_Pos)) 
#define AC_INTENSET_WIN_Pos                   _U_(4)                                               /**< (AC_INTENSET Position) Window x Interrupt Enable */
#define AC_INTENSET_WIN_Msk                   (_U_(0x1) << AC_INTENSET_WIN_Pos)                    /**< (AC_INTENSET Mask) WIN */
#define AC_INTENSET_WIN(value)                (AC_INTENSET_WIN_Msk & ((value) << AC_INTENSET_WIN_Pos)) 

/* -------- AC_INTFLAG : (AC Offset: 0x06) (R/W 8) Interrupt Flag Status and Clear -------- */
#define AC_INTFLAG_RESETVALUE                 _U_(0x00)                                            /**<  (AC_INTFLAG) Interrupt Flag Status and Clear  Reset Value */

#define AC_INTFLAG_COMP0_Pos                  _U_(0)                                               /**< (AC_INTFLAG) Comparator 0 Position */
#define AC_INTFLAG_COMP0_Msk                  (_U_(0x1) << AC_INTFLAG_COMP0_Pos)                   /**< (AC_INTFLAG) Comparator 0 Mask */
#define AC_INTFLAG_COMP0(value)               (AC_INTFLAG_COMP0_Msk & ((value) << AC_INTFLAG_COMP0_Pos))
#define AC_INTFLAG_COMP1_Pos                  _U_(1)                                               /**< (AC_INTFLAG) Comparator 1 Position */
#define AC_INTFLAG_COMP1_Msk                  (_U_(0x1) << AC_INTFLAG_COMP1_Pos)                   /**< (AC_INTFLAG) Comparator 1 Mask */
#define AC_INTFLAG_COMP1(value)               (AC_INTFLAG_COMP1_Msk & ((value) << AC_INTFLAG_COMP1_Pos))
#define AC_INTFLAG_WIN0_Pos                   _U_(4)                                               /**< (AC_INTFLAG) Window 0 Position */
#define AC_INTFLAG_WIN0_Msk                   (_U_(0x1) << AC_INTFLAG_WIN0_Pos)                    /**< (AC_INTFLAG) Window 0 Mask */
#define AC_INTFLAG_WIN0(value)                (AC_INTFLAG_WIN0_Msk & ((value) << AC_INTFLAG_WIN0_Pos))
#define AC_INTFLAG_Msk                        _U_(0x13)                                            /**< (AC_INTFLAG) Register Mask  */

#define AC_INTFLAG_COMP_Pos                   _U_(0)                                               /**< (AC_INTFLAG Position) Comparator x */
#define AC_INTFLAG_COMP_Msk                   (_U_(0x3) << AC_INTFLAG_COMP_Pos)                    /**< (AC_INTFLAG Mask) COMP */
#define AC_INTFLAG_COMP(value)                (AC_INTFLAG_COMP_Msk & ((value) << AC_INTFLAG_COMP_Pos)) 
#define AC_INTFLAG_WIN_Pos                    _U_(4)                                               /**< (AC_INTFLAG Position) Window x */
#define AC_INTFLAG_WIN_Msk                    (_U_(0x1) << AC_INTFLAG_WIN_Pos)                     /**< (AC_INTFLAG Mask) WIN */
#define AC_INTFLAG_WIN(value)                 (AC_INTFLAG_WIN_Msk & ((value) << AC_INTFLAG_WIN_Pos)) 

/* -------- AC_STATUSA : (AC Offset: 0x07) ( R/ 8) Status A -------- */
#define AC_STATUSA_RESETVALUE                 _U_(0x00)                                            /**<  (AC_STATUSA) Status A  Reset Value */

#define AC_STATUSA_STATE0_Pos                 _U_(0)                                               /**< (AC_STATUSA) Comparator 0 Current State Position */
#define AC_STATUSA_STATE0_Msk                 (_U_(0x1) << AC_STATUSA_STATE0_Pos)                  /**< (AC_STATUSA) Comparator 0 Current State Mask */
#define AC_STATUSA_STATE0(value)              (AC_STATUSA_STATE0_Msk & ((value) << AC_STATUSA_STATE0_Pos))
#define AC_STATUSA_STATE1_Pos                 _U_(1)                                               /**< (AC_STATUSA) Comparator 1 Current State Position */
#define AC_STATUSA_STATE1_Msk                 (_U_(0x1) << AC_STATUSA_STATE1_Pos)                  /**< (AC_STATUSA) Comparator 1 Current State Mask */
#define AC_STATUSA_STATE1(value)              (AC_STATUSA_STATE1_Msk & ((value) << AC_STATUSA_STATE1_Pos))
#define AC_STATUSA_WSTATE0_Pos                _U_(4)                                               /**< (AC_STATUSA) Window 0 Current State Position */
#define AC_STATUSA_WSTATE0_Msk                (_U_(0x3) << AC_STATUSA_WSTATE0_Pos)                 /**< (AC_STATUSA) Window 0 Current State Mask */
#define AC_STATUSA_WSTATE0(value)             (AC_STATUSA_WSTATE0_Msk & ((value) << AC_STATUSA_WSTATE0_Pos))
#define   AC_STATUSA_WSTATE0_ABOVE_Val        _U_(0x0)                                             /**< (AC_STATUSA) Signal is above window  */
#define   AC_STATUSA_WSTATE0_INSIDE_Val       _U_(0x1)                                             /**< (AC_STATUSA) Signal is inside window  */
#define   AC_STATUSA_WSTATE0_BELOW_Val        _U_(0x2)                                             /**< (AC_STATUSA) Signal is below window  */
#define AC_STATUSA_WSTATE0_ABOVE              (AC_STATUSA_WSTATE0_ABOVE_Val << AC_STATUSA_WSTATE0_Pos) /**< (AC_STATUSA) Signal is above window Position  */
#define AC_STATUSA_WSTATE0_INSIDE             (AC_STATUSA_WSTATE0_INSIDE_Val << AC_STATUSA_WSTATE0_Pos) /**< (AC_STATUSA) Signal is inside window Position  */
#define AC_STATUSA_WSTATE0_BELOW              (AC_STATUSA_WSTATE0_BELOW_Val << AC_STATUSA_WSTATE0_Pos) /**< (AC_STATUSA) Signal is below window Position  */
#define AC_STATUSA_Msk                        _U_(0x33)                                            /**< (AC_STATUSA) Register Mask  */

#define AC_STATUSA_STATE_Pos                  _U_(0)                                               /**< (AC_STATUSA Position) Comparator x Current State */
#define AC_STATUSA_STATE_Msk                  (_U_(0x3) << AC_STATUSA_STATE_Pos)                   /**< (AC_STATUSA Mask) STATE */
#define AC_STATUSA_STATE(value)               (AC_STATUSA_STATE_Msk & ((value) << AC_STATUSA_STATE_Pos)) 

/* -------- AC_STATUSB : (AC Offset: 0x08) ( R/ 8) Status B -------- */
#define AC_STATUSB_RESETVALUE                 _U_(0x00)                                            /**<  (AC_STATUSB) Status B  Reset Value */

#define AC_STATUSB_READY0_Pos                 _U_(0)                                               /**< (AC_STATUSB) Comparator 0 Ready Position */
#define AC_STATUSB_READY0_Msk                 (_U_(0x1) << AC_STATUSB_READY0_Pos)                  /**< (AC_STATUSB) Comparator 0 Ready Mask */
#define AC_STATUSB_READY0(value)              (AC_STATUSB_READY0_Msk & ((value) << AC_STATUSB_READY0_Pos))
#define AC_STATUSB_READY1_Pos                 _U_(1)                                               /**< (AC_STATUSB) Comparator 1 Ready Position */
#define AC_STATUSB_READY1_Msk                 (_U_(0x1) << AC_STATUSB_READY1_Pos)                  /**< (AC_STATUSB) Comparator 1 Ready Mask */
#define AC_STATUSB_READY1(value)              (AC_STATUSB_READY1_Msk & ((value) << AC_STATUSB_READY1_Pos))
#define AC_STATUSB_Msk                        _U_(0x03)                                            /**< (AC_STATUSB) Register Mask  */

#define AC_STATUSB_READY_Pos                  _U_(0)                                               /**< (AC_STATUSB Position) Comparator x Ready */
#define AC_STATUSB_READY_Msk                  (_U_(0x3) << AC_STATUSB_READY_Pos)                   /**< (AC_STATUSB Mask) READY */
#define AC_STATUSB_READY(value)               (AC_STATUSB_READY_Msk & ((value) << AC_STATUSB_READY_Pos)) 

/* -------- AC_DBGCTRL : (AC Offset: 0x09) (R/W 8) Debug Control -------- */
#define AC_DBGCTRL_RESETVALUE                 _U_(0x00)                                            /**<  (AC_DBGCTRL) Debug Control  Reset Value */

#define AC_DBGCTRL_DBGRUN_Pos                 _U_(0)                                               /**< (AC_DBGCTRL) Debug Run Position */
#define AC_DBGCTRL_DBGRUN_Msk                 (_U_(0x1) << AC_DBGCTRL_DBGRUN_Pos)                  /**< (AC_DBGCTRL) Debug Run Mask */
#define AC_DBGCTRL_DBGRUN(value)              (AC_DBGCTRL_DBGRUN_Msk & ((value) << AC_DBGCTRL_DBGRUN_Pos))
#define AC_DBGCTRL_Msk                        _U_(0x01)                                            /**< (AC_DBGCTRL) Register Mask  */


/* -------- AC_WINCTRL : (AC Offset: 0x0A) (R/W 8) Window Control -------- */
#define AC_WINCTRL_RESETVALUE                 _U_(0x00)                                            /**<  (AC_WINCTRL) Window Control  Reset Value */

#define AC_WINCTRL_WEN0_Pos                   _U_(0)                                               /**< (AC_WINCTRL) Window 0 Mode Enable Position */
#define AC_WINCTRL_WEN0_Msk                   (_U_(0x1) << AC_WINCTRL_WEN0_Pos)                    /**< (AC_WINCTRL) Window 0 Mode Enable Mask */
#define AC_WINCTRL_WEN0(value)                (AC_WINCTRL_WEN0_Msk & ((value) << AC_WINCTRL_WEN0_Pos))
#define AC_WINCTRL_WINTSEL0_Pos               _U_(1)                                               /**< (AC_WINCTRL) Window 0 Interrupt Selection Position */
#define AC_WINCTRL_WINTSEL0_Msk               (_U_(0x3) << AC_WINCTRL_WINTSEL0_Pos)                /**< (AC_WINCTRL) Window 0 Interrupt Selection Mask */
#define AC_WINCTRL_WINTSEL0(value)            (AC_WINCTRL_WINTSEL0_Msk & ((value) << AC_WINCTRL_WINTSEL0_Pos))
#define   AC_WINCTRL_WINTSEL0_ABOVE_Val       _U_(0x0)                                             /**< (AC_WINCTRL) Interrupt on signal above window  */
#define   AC_WINCTRL_WINTSEL0_INSIDE_Val      _U_(0x1)                                             /**< (AC_WINCTRL) Interrupt on signal inside window  */
#define   AC_WINCTRL_WINTSEL0_BELOW_Val       _U_(0x2)                                             /**< (AC_WINCTRL) Interrupt on signal below window  */
#define   AC_WINCTRL_WINTSEL0_OUTSIDE_Val     _U_(0x3)                                             /**< (AC_WINCTRL) Interrupt on signal outside window  */
#define AC_WINCTRL_WINTSEL0_ABOVE             (AC_WINCTRL_WINTSEL0_ABOVE_Val << AC_WINCTRL_WINTSEL0_Pos) /**< (AC_WINCTRL) Interrupt on signal above window Position  */
#define AC_WINCTRL_WINTSEL0_INSIDE            (AC_WINCTRL_WINTSEL0_INSIDE_Val << AC_WINCTRL_WINTSEL0_Pos) /**< (AC_WINCTRL) Interrupt on signal inside window Position  */
#define AC_WINCTRL_WINTSEL0_BELOW             (AC_WINCTRL_WINTSEL0_BELOW_Val << AC_WINCTRL_WINTSEL0_Pos) /**< (AC_WINCTRL) Interrupt on signal below window Position  */
#define AC_WINCTRL_WINTSEL0_OUTSIDE           (AC_WINCTRL_WINTSEL0_OUTSIDE_Val << AC_WINCTRL_WINTSEL0_Pos) /**< (AC_WINCTRL) Interrupt on signal outside window Position  */
#define AC_WINCTRL_Msk                        _U_(0x07)                                            /**< (AC_WINCTRL) Register Mask  */

#define AC_WINCTRL_WEN_Pos                    _U_(0)                                               /**< (AC_WINCTRL Position) Window x Mode Enable */
#define AC_WINCTRL_WEN_Msk                    (_U_(0x1) << AC_WINCTRL_WEN_Pos)                     /**< (AC_WINCTRL Mask) WEN */
#define AC_WINCTRL_WEN(value)                 (AC_WINCTRL_WEN_Msk & ((value) << AC_WINCTRL_WEN_Pos)) 

/* -------- AC_SCALER : (AC Offset: 0x0C) (R/W 8) Scaler n -------- */
#define AC_SCALER_RESETVALUE                  _U_(0x00)                                            /**<  (AC_SCALER) Scaler n  Reset Value */

#define AC_SCALER_VALUE_Pos                   _U_(0)                                               /**< (AC_SCALER) Scaler Value Position */
#define AC_SCALER_VALUE_Msk                   (_U_(0x3F) << AC_SCALER_VALUE_Pos)                   /**< (AC_SCALER) Scaler Value Mask */
#define AC_SCALER_VALUE(value)                (AC_SCALER_VALUE_Msk & ((value) << AC_SCALER_VALUE_Pos))
#define AC_SCALER_Msk                         _U_(0x3F)                                            /**< (AC_SCALER) Register Mask  */


/* -------- AC_COMPCTRL : (AC Offset: 0x10) (R/W 32) Comparator Control n -------- */
#define AC_COMPCTRL_RESETVALUE                _U_(0x00)                                            /**<  (AC_COMPCTRL) Comparator Control n  Reset Value */

#define AC_COMPCTRL_ENABLE_Pos                _U_(1)                                               /**< (AC_COMPCTRL) Enable Position */
#define AC_COMPCTRL_ENABLE_Msk                (_U_(0x1) << AC_COMPCTRL_ENABLE_Pos)                 /**< (AC_COMPCTRL) Enable Mask */
#define AC_COMPCTRL_ENABLE(value)             (AC_COMPCTRL_ENABLE_Msk & ((value) << AC_COMPCTRL_ENABLE_Pos))
#define AC_COMPCTRL_SINGLE_Pos                _U_(2)                                               /**< (AC_COMPCTRL) Single-Shot Mode Position */
#define AC_COMPCTRL_SINGLE_Msk                (_U_(0x1) << AC_COMPCTRL_SINGLE_Pos)                 /**< (AC_COMPCTRL) Single-Shot Mode Mask */
#define AC_COMPCTRL_SINGLE(value)             (AC_COMPCTRL_SINGLE_Msk & ((value) << AC_COMPCTRL_SINGLE_Pos))
#define AC_COMPCTRL_INTSEL_Pos                _U_(3)                                               /**< (AC_COMPCTRL) Interrupt Selection Position */
#define AC_COMPCTRL_INTSEL_Msk                (_U_(0x3) << AC_COMPCTRL_INTSEL_Pos)                 /**< (AC_COMPCTRL) Interrupt Selection Mask */
#define AC_COMPCTRL_INTSEL(value)             (AC_COMPCTRL_INTSEL_Msk & ((value) << AC_COMPCTRL_INTSEL_Pos))
#define   AC_COMPCTRL_INTSEL_TOGGLE_Val       _U_(0x0)                                             /**< (AC_COMPCTRL) Interrupt on comparator output toggle  */
#define   AC_COMPCTRL_INTSEL_RISING_Val       _U_(0x1)                                             /**< (AC_COMPCTRL) Interrupt on comparator output rising  */
#define   AC_COMPCTRL_INTSEL_FALLING_Val      _U_(0x2)                                             /**< (AC_COMPCTRL) Interrupt on comparator output falling  */
#define   AC_COMPCTRL_INTSEL_EOC_Val          _U_(0x3)                                             /**< (AC_COMPCTRL) Interrupt on end of comparison (single-shot mode only)  */
#define AC_COMPCTRL_INTSEL_TOGGLE             (AC_COMPCTRL_INTSEL_TOGGLE_Val << AC_COMPCTRL_INTSEL_Pos) /**< (AC_COMPCTRL) Interrupt on comparator output toggle Position  */
#define AC_COMPCTRL_INTSEL_RISING             (AC_COMPCTRL_INTSEL_RISING_Val << AC_COMPCTRL_INTSEL_Pos) /**< (AC_COMPCTRL) Interrupt on comparator output rising Position  */
#define AC_COMPCTRL_INTSEL_FALLING            (AC_COMPCTRL_INTSEL_FALLING_Val << AC_COMPCTRL_INTSEL_Pos) /**< (AC_COMPCTRL) Interrupt on comparator output falling Position  */
#define AC_COMPCTRL_INTSEL_EOC                (AC_COMPCTRL_INTSEL_EOC_Val << AC_COMPCTRL_INTSEL_Pos) /**< (AC_COMPCTRL) Interrupt on end of comparison (single-shot mode only) Position  */
#define AC_COMPCTRL_RUNSTDBY_Pos              _U_(6)                                               /**< (AC_COMPCTRL) Run in Standby Position */
#define AC_COMPCTRL_RUNSTDBY_Msk              (_U_(0x1) << AC_COMPCTRL_RUNSTDBY_Pos)               /**< (AC_COMPCTRL) Run in Standby Mask */
#define AC_COMPCTRL_RUNSTDBY(value)           (AC_COMPCTRL_RUNSTDBY_Msk & ((value) << AC_COMPCTRL_RUNSTDBY_Pos))
#define AC_COMPCTRL_MUXNEG_Pos                _U_(8)                                               /**< (AC_COMPCTRL) Negative Input Mux Selection Position */
#define AC_COMPCTRL_MUXNEG_Msk                (_U_(0x7) << AC_COMPCTRL_MUXNEG_Pos)                 /**< (AC_COMPCTRL) Negative Input Mux Selection Mask */
#define AC_COMPCTRL_MUXNEG(value)             (AC_COMPCTRL_MUXNEG_Msk & ((value) << AC_COMPCTRL_MUXNEG_Pos))
#define   AC_COMPCTRL_MUXNEG_PIN0_Val         _U_(0x0)                                             /**< (AC_COMPCTRL) I/O pin 0  */
#define   AC_COMPCTRL_MUXNEG_PIN1_Val         _U_(0x1)                                             /**< (AC_COMPCTRL) I/O pin 1  */
#define   AC_COMPCTRL_MUXNEG_PIN2_Val         _U_(0x2)                                             /**< (AC_COMPCTRL) I/O pin 2  */
#define   AC_COMPCTRL_MUXNEG_PIN3_Val         _U_(0x3)                                             /**< (AC_COMPCTRL) I/O pin 3  */
#define   AC_COMPCTRL_MUXNEG_GND_Val          _U_(0x4)                                             /**< (AC_COMPCTRL) Ground  */
#define   AC_COMPCTRL_MUXNEG_VSCALE_Val       _U_(0x5)                                             /**< (AC_COMPCTRL) VDD scaler  */
#define   AC_COMPCTRL_MUXNEG_BANDGAP_Val      _U_(0x6)                                             /**< (AC_COMPCTRL) Internal bandgap voltage  */
#define   AC_COMPCTRL_MUXNEG_DAC_Val          _U_(0x7)                                             /**< (AC_COMPCTRL) DAC output  */
#define AC_COMPCTRL_MUXNEG_PIN0               (AC_COMPCTRL_MUXNEG_PIN0_Val << AC_COMPCTRL_MUXNEG_Pos) /**< (AC_COMPCTRL) I/O pin 0 Position  */
#define AC_COMPCTRL_MUXNEG_PIN1               (AC_COMPCTRL_MUXNEG_PIN1_Val << AC_COMPCTRL_MUXNEG_Pos) /**< (AC_COMPCTRL) I/O pin 1 Position  */
#define AC_COMPCTRL_MUXNEG_PIN2               (AC_COMPCTRL_MUXNEG_PIN2_Val << AC_COMPCTRL_MUXNEG_Pos) /**< (AC_COMPCTRL) I/O pin 2 Position  */
#define AC_COMPCTRL_MUXNEG_PIN3               (AC_COMPCTRL_MUXNEG_PIN3_Val << AC_COMPCTRL_MUXNEG_Pos) /**< (AC_COMPCTRL) I/O pin 3 Position  */
#define AC_COMPCTRL_MUXNEG_GND                (AC_COMPCTRL_MUXNEG_GND_Val << AC_COMPCTRL_MUXNEG_Pos) /**< (AC_COMPCTRL) Ground Position  */
#define AC_COMPCTRL_MUXNEG_VSCALE             (AC_COMPCTRL_MUXNEG_VSCALE_Val << AC_COMPCTRL_MUXNEG_Pos) /**< (AC_COMPCTRL) VDD scaler Position  */
#define AC_COMPCTRL_MUXNEG_BANDGAP            (AC_COMPCTRL_MUXNEG_BANDGAP_Val << AC_COMPCTRL_MUXNEG_Pos) /**< (AC_COMPCTRL) Internal bandgap voltage Position  */
#define AC_COMPCTRL_MUXNEG_DAC                (AC_COMPCTRL_MUXNEG_DAC_Val << AC_COMPCTRL_MUXNEG_Pos) /**< (AC_COMPCTRL) DAC output Position  */
#define AC_COMPCTRL_MUXPOS_Pos                _U_(12)                                              /**< (AC_COMPCTRL) Positive Input Mux Selection Position */
#define AC_COMPCTRL_MUXPOS_Msk                (_U_(0x7) << AC_COMPCTRL_MUXPOS_Pos)                 /**< (AC_COMPCTRL) Positive Input Mux Selection Mask */
#define AC_COMPCTRL_MUXPOS(value)             (AC_COMPCTRL_MUXPOS_Msk & ((value) << AC_COMPCTRL_MUXPOS_Pos))
#define   AC_COMPCTRL_MUXPOS_PIN0_Val         _U_(0x0)                                             /**< (AC_COMPCTRL) I/O pin 0  */
#define   AC_COMPCTRL_MUXPOS_PIN1_Val         _U_(0x1)                                             /**< (AC_COMPCTRL) I/O pin 1  */
#define   AC_COMPCTRL_MUXPOS_PIN2_Val         _U_(0x2)                                             /**< (AC_COMPCTRL) I/O pin 2  */
#define   AC_COMPCTRL_MUXPOS_PIN3_Val         _U_(0x3)                                             /**< (AC_COMPCTRL) I/O pin 3  */
#define   AC_COMPCTRL_MUXPOS_VSCALE_Val       _U_(0x4)                                             /**< (AC_COMPCTRL) VDD Scaler  */
#define AC_COMPCTRL_MUXPOS_PIN0               (AC_COMPCTRL_MUXPOS_PIN0_Val << AC_COMPCTRL_MUXPOS_Pos) /**< (AC_COMPCTRL) I/O pin 0 Position  */
#define AC_COMPCTRL_MUXPOS_PIN1               (AC_COMPCTRL_MUXPOS_PIN1_Val << AC_COMPCTRL_MUXPOS_Pos) /**< (AC_COMPCTRL) I/O pin 1 Position  */
#define AC_COMPCTRL_MUXPOS_PIN2               (AC_COMPCTRL_MUXPOS_PIN2_Val << AC_COMPCTRL_MUXPOS_Pos) /**< (AC_COMPCTRL) I/O pin 2 Position  */
#define AC_COMPCTRL_MUXPOS_PIN3               (AC_COMPCTRL_MUXPOS_PIN3_Val << AC_COMPCTRL_MUXPOS_Pos) /**< (AC_COMPCTRL) I/O pin 3 Position  */
#define AC_COMPCTRL_MUXPOS_VSCALE             (AC_COMPCTRL_MUXPOS_VSCALE_Val << AC_COMPCTRL_MUXPOS_Pos) /**< (AC_COMPCTRL) VDD Scaler Position  */
#define AC_COMPCTRL_SWAP_Pos                  _U_(15)                                              /**< (AC_COMPCTRL) Swap Inputs and Invert Position */
#define AC_COMPCTRL_SWAP_Msk                  (_U_(0x1) << AC_COMPCTRL_SWAP_Pos)                   /**< (AC_COMPCTRL) Swap Inputs and Invert Mask */
#define AC_COMPCTRL_SWAP(value)               (AC_COMPCTRL_SWAP_Msk & ((value) << AC_COMPCTRL_SWAP_Pos))
#define AC_COMPCTRL_SPEED_Pos                 _U_(16)                                              /**< (AC_COMPCTRL) Speed Selection Position */
#define AC_COMPCTRL_SPEED_Msk                 (_U_(0x3) << AC_COMPCTRL_SPEED_Pos)                  /**< (AC_COMPCTRL) Speed Selection Mask */
#define AC_COMPCTRL_SPEED(value)              (AC_COMPCTRL_SPEED_Msk & ((value) << AC_COMPCTRL_SPEED_Pos))
#define   AC_COMPCTRL_SPEED_HIGH_Val          _U_(0x3)                                             /**< (AC_COMPCTRL) High speed  */
#define AC_COMPCTRL_SPEED_HIGH                (AC_COMPCTRL_SPEED_HIGH_Val << AC_COMPCTRL_SPEED_Pos) /**< (AC_COMPCTRL) High speed Position  */
#define AC_COMPCTRL_HYSTEN_Pos                _U_(19)                                              /**< (AC_COMPCTRL) Hysteresis Enable Position */
#define AC_COMPCTRL_HYSTEN_Msk                (_U_(0x1) << AC_COMPCTRL_HYSTEN_Pos)                 /**< (AC_COMPCTRL) Hysteresis Enable Mask */
#define AC_COMPCTRL_HYSTEN(value)             (AC_COMPCTRL_HYSTEN_Msk & ((value) << AC_COMPCTRL_HYSTEN_Pos))
#define AC_COMPCTRL_HYST_Pos                  _U_(20)                                              /**< (AC_COMPCTRL) Hysteresis Level Position */
#define AC_COMPCTRL_HYST_Msk                  (_U_(0x3) << AC_COMPCTRL_HYST_Pos)                   /**< (AC_COMPCTRL) Hysteresis Level Mask */
#define AC_COMPCTRL_HYST(value)               (AC_COMPCTRL_HYST_Msk & ((value) << AC_COMPCTRL_HYST_Pos))
#define   AC_COMPCTRL_HYST_HYST50_Val         _U_(0x0)                                             /**< (AC_COMPCTRL) 50mV  */
#define   AC_COMPCTRL_HYST_HYST100_Val        _U_(0x1)                                             /**< (AC_COMPCTRL) 100mV  */
#define   AC_COMPCTRL_HYST_HYST150_Val        _U_(0x2)                                             /**< (AC_COMPCTRL) 150mV  */
#define AC_COMPCTRL_HYST_HYST50               (AC_COMPCTRL_HYST_HYST50_Val << AC_COMPCTRL_HYST_Pos) /**< (AC_COMPCTRL) 50mV Position  */
#define AC_COMPCTRL_HYST_HYST100              (AC_COMPCTRL_HYST_HYST100_Val << AC_COMPCTRL_HYST_Pos) /**< (AC_COMPCTRL) 100mV Position  */
#define AC_COMPCTRL_HYST_HYST150              (AC_COMPCTRL_HYST_HYST150_Val << AC_COMPCTRL_HYST_Pos) /**< (AC_COMPCTRL) 150mV Position  */
#define AC_COMPCTRL_FLEN_Pos                  _U_(24)                                              /**< (AC_COMPCTRL) Filter Length Position */
#define AC_COMPCTRL_FLEN_Msk                  (_U_(0x7) << AC_COMPCTRL_FLEN_Pos)                   /**< (AC_COMPCTRL) Filter Length Mask */
#define AC_COMPCTRL_FLEN(value)               (AC_COMPCTRL_FLEN_Msk & ((value) << AC_COMPCTRL_FLEN_Pos))
#define   AC_COMPCTRL_FLEN_OFF_Val            _U_(0x0)                                             /**< (AC_COMPCTRL) No filtering  */
#define   AC_COMPCTRL_FLEN_MAJ3_Val           _U_(0x1)                                             /**< (AC_COMPCTRL) 3-bit majority function (2 of 3)  */
#define   AC_COMPCTRL_FLEN_MAJ5_Val           _U_(0x2)                                             /**< (AC_COMPCTRL) 5-bit majority function (3 of 5)  */
#define AC_COMPCTRL_FLEN_OFF                  (AC_COMPCTRL_FLEN_OFF_Val << AC_COMPCTRL_FLEN_Pos)   /**< (AC_COMPCTRL) No filtering Position  */
#define AC_COMPCTRL_FLEN_MAJ3                 (AC_COMPCTRL_FLEN_MAJ3_Val << AC_COMPCTRL_FLEN_Pos)  /**< (AC_COMPCTRL) 3-bit majority function (2 of 3) Position  */
#define AC_COMPCTRL_FLEN_MAJ5                 (AC_COMPCTRL_FLEN_MAJ5_Val << AC_COMPCTRL_FLEN_Pos)  /**< (AC_COMPCTRL) 5-bit majority function (3 of 5) Position  */
#define AC_COMPCTRL_OUT_Pos                   _U_(28)                                              /**< (AC_COMPCTRL) Output Position */
#define AC_COMPCTRL_OUT_Msk                   (_U_(0x3) << AC_COMPCTRL_OUT_Pos)                    /**< (AC_COMPCTRL) Output Mask */
#define AC_COMPCTRL_OUT(value)                (AC_COMPCTRL_OUT_Msk & ((value) << AC_COMPCTRL_OUT_Pos))
#define   AC_COMPCTRL_OUT_OFF_Val             _U_(0x0)                                             /**< (AC_COMPCTRL) The output of COMPn is not routed to the COMPn I/O port  */
#define   AC_COMPCTRL_OUT_ASYNC_Val           _U_(0x1)                                             /**< (AC_COMPCTRL) The asynchronous output of COMPn is routed to the COMPn I/O port  */
#define   AC_COMPCTRL_OUT_SYNC_Val            _U_(0x2)                                             /**< (AC_COMPCTRL) The synchronous output (including filtering) of COMPn is routed to the COMPn I/O port  */
#define AC_COMPCTRL_OUT_OFF                   (AC_COMPCTRL_OUT_OFF_Val << AC_COMPCTRL_OUT_Pos)     /**< (AC_COMPCTRL) The output of COMPn is not routed to the COMPn I/O port Position  */
#define AC_COMPCTRL_OUT_ASYNC                 (AC_COMPCTRL_OUT_ASYNC_Val << AC_COMPCTRL_OUT_Pos)   /**< (AC_COMPCTRL) The asynchronous output of COMPn is routed to the COMPn I/O port Position  */
#define AC_COMPCTRL_OUT_SYNC                  (AC_COMPCTRL_OUT_SYNC_Val << AC_COMPCTRL_OUT_Pos)    /**< (AC_COMPCTRL) The synchronous output (including filtering) of COMPn is routed to the COMPn I/O port Position  */
#define AC_COMPCTRL_Msk                       _U_(0x373BF75E)                                      /**< (AC_COMPCTRL) Register Mask  */


/* -------- AC_SYNCBUSY : (AC Offset: 0x20) ( R/ 32) Synchronization Busy -------- */
#define AC_SYNCBUSY_RESETVALUE                _U_(0x00)                                            /**<  (AC_SYNCBUSY) Synchronization Busy  Reset Value */

#define AC_SYNCBUSY_SWRST_Pos                 _U_(0)                                               /**< (AC_SYNCBUSY) Software Reset Synchronization Busy Position */
#define AC_SYNCBUSY_SWRST_Msk                 (_U_(0x1) << AC_SYNCBUSY_SWRST_Pos)                  /**< (AC_SYNCBUSY) Software Reset Synchronization Busy Mask */
#define AC_SYNCBUSY_SWRST(value)              (AC_SYNCBUSY_SWRST_Msk & ((value) << AC_SYNCBUSY_SWRST_Pos))
#define AC_SYNCBUSY_ENABLE_Pos                _U_(1)                                               /**< (AC_SYNCBUSY) Enable Synchronization Busy Position */
#define AC_SYNCBUSY_ENABLE_Msk                (_U_(0x1) << AC_SYNCBUSY_ENABLE_Pos)                 /**< (AC_SYNCBUSY) Enable Synchronization Busy Mask */
#define AC_SYNCBUSY_ENABLE(value)             (AC_SYNCBUSY_ENABLE_Msk & ((value) << AC_SYNCBUSY_ENABLE_Pos))
#define AC_SYNCBUSY_WINCTRL_Pos               _U_(2)                                               /**< (AC_SYNCBUSY) WINCTRL Synchronization Busy Position */
#define AC_SYNCBUSY_WINCTRL_Msk               (_U_(0x1) << AC_SYNCBUSY_WINCTRL_Pos)                /**< (AC_SYNCBUSY) WINCTRL Synchronization Busy Mask */
#define AC_SYNCBUSY_WINCTRL(value)            (AC_SYNCBUSY_WINCTRL_Msk & ((value) << AC_SYNCBUSY_WINCTRL_Pos))
#define AC_SYNCBUSY_COMPCTRL0_Pos             _U_(3)                                               /**< (AC_SYNCBUSY) COMPCTRL 0 Synchronization Busy Position */
#define AC_SYNCBUSY_COMPCTRL0_Msk             (_U_(0x1) << AC_SYNCBUSY_COMPCTRL0_Pos)              /**< (AC_SYNCBUSY) COMPCTRL 0 Synchronization Busy Mask */
#define AC_SYNCBUSY_COMPCTRL0(value)          (AC_SYNCBUSY_COMPCTRL0_Msk & ((value) << AC_SYNCBUSY_COMPCTRL0_Pos))
#define AC_SYNCBUSY_COMPCTRL1_Pos             _U_(4)                                               /**< (AC_SYNCBUSY) COMPCTRL 1 Synchronization Busy Position */
#define AC_SYNCBUSY_COMPCTRL1_Msk             (_U_(0x1) << AC_SYNCBUSY_COMPCTRL1_Pos)              /**< (AC_SYNCBUSY) COMPCTRL 1 Synchronization Busy Mask */
#define AC_SYNCBUSY_COMPCTRL1(value)          (AC_SYNCBUSY_COMPCTRL1_Msk & ((value) << AC_SYNCBUSY_COMPCTRL1_Pos))
#define AC_SYNCBUSY_Msk                       _U_(0x0000001F)                                      /**< (AC_SYNCBUSY) Register Mask  */

#define AC_SYNCBUSY_COMPCTRL_Pos              _U_(3)                                               /**< (AC_SYNCBUSY Position) COMPCTRL x Synchronization Busy */
#define AC_SYNCBUSY_COMPCTRL_Msk              (_U_(0x3) << AC_SYNCBUSY_COMPCTRL_Pos)               /**< (AC_SYNCBUSY Mask) COMPCTRL */
#define AC_SYNCBUSY_COMPCTRL(value)           (AC_SYNCBUSY_COMPCTRL_Msk & ((value) << AC_SYNCBUSY_COMPCTRL_Pos)) 

/* -------- AC_CALIB : (AC Offset: 0x24) (R/W 16) Calibration -------- */
#define AC_CALIB_RESETVALUE                   _U_(0x101)                                           /**<  (AC_CALIB) Calibration  Reset Value */

#define AC_CALIB_BIAS0_Pos                    _U_(0)                                               /**< (AC_CALIB) COMP0/1 Bias Scaling Position */
#define AC_CALIB_BIAS0_Msk                    (_U_(0x3) << AC_CALIB_BIAS0_Pos)                     /**< (AC_CALIB) COMP0/1 Bias Scaling Mask */
#define AC_CALIB_BIAS0(value)                 (AC_CALIB_BIAS0_Msk & ((value) << AC_CALIB_BIAS0_Pos))
#define AC_CALIB_Msk                          _U_(0x0003)                                          /**< (AC_CALIB) Register Mask  */


/** \brief AC register offsets definitions */
#define AC_CTRLA_REG_OFST              (0x00)              /**< (AC_CTRLA) Control A Offset */
#define AC_CTRLB_REG_OFST              (0x01)              /**< (AC_CTRLB) Control B Offset */
#define AC_EVCTRL_REG_OFST             (0x02)              /**< (AC_EVCTRL) Event Control Offset */
#define AC_INTENCLR_REG_OFST           (0x04)              /**< (AC_INTENCLR) Interrupt Enable Clear Offset */
#define AC_INTENSET_REG_OFST           (0x05)              /**< (AC_INTENSET) Interrupt Enable Set Offset */
#define AC_INTFLAG_REG_OFST            (0x06)              /**< (AC_INTFLAG) Interrupt Flag Status and Clear Offset */
#define AC_STATUSA_REG_OFST            (0x07)              /**< (AC_STATUSA) Status A Offset */
#define AC_STATUSB_REG_OFST            (0x08)              /**< (AC_STATUSB) Status B Offset */
#define AC_DBGCTRL_REG_OFST            (0x09)              /**< (AC_DBGCTRL) Debug Control Offset */
#define AC_WINCTRL_REG_OFST            (0x0A)              /**< (AC_WINCTRL) Window Control Offset */
#define AC_SCALER_REG_OFST             (0x0C)              /**< (AC_SCALER) Scaler n Offset */
#define AC_COMPCTRL_REG_OFST           (0x10)              /**< (AC_COMPCTRL) Comparator Control n Offset */
#define AC_SYNCBUSY_REG_OFST           (0x20)              /**< (AC_SYNCBUSY) Synchronization Busy Offset */
#define AC_CALIB_REG_OFST              (0x24)              /**< (AC_CALIB) Calibration Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief AC register API structure */
typedef struct
{  /* Analog Comparators */
  __IO  uint8_t                        AC_CTRLA;           /**< Offset: 0x00 (R/W  8) Control A */
  __O   uint8_t                        AC_CTRLB;           /**< Offset: 0x01 ( /W  8) Control B */
  __IO  uint16_t                       AC_EVCTRL;          /**< Offset: 0x02 (R/W  16) Event Control */
  __IO  uint8_t                        AC_INTENCLR;        /**< Offset: 0x04 (R/W  8) Interrupt Enable Clear */
  __IO  uint8_t                        AC_INTENSET;        /**< Offset: 0x05 (R/W  8) Interrupt Enable Set */
  __IO  uint8_t                        AC_INTFLAG;         /**< Offset: 0x06 (R/W  8) Interrupt Flag Status and Clear */
  __I   uint8_t                        AC_STATUSA;         /**< Offset: 0x07 (R/   8) Status A */
  __I   uint8_t                        AC_STATUSB;         /**< Offset: 0x08 (R/   8) Status B */
  __IO  uint8_t                        AC_DBGCTRL;         /**< Offset: 0x09 (R/W  8) Debug Control */
  __IO  uint8_t                        AC_WINCTRL;         /**< Offset: 0x0A (R/W  8) Window Control */
  __I   uint8_t                        Reserved1[0x01];
  __IO  uint8_t                        AC_SCALER[2];       /**< Offset: 0x0C (R/W  8) Scaler n */
  __I   uint8_t                        Reserved2[0x02];
  __IO  uint32_t                       AC_COMPCTRL[2];     /**< Offset: 0x10 (R/W  32) Comparator Control n */
  __I   uint8_t                        Reserved3[0x08];
  __I   uint32_t                       AC_SYNCBUSY;        /**< Offset: 0x20 (R/   32) Synchronization Busy */
  __IO  uint16_t                       AC_CALIB;           /**< Offset: 0x24 (R/W  16) Calibration */
} ac_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_AC_COMPONENT_H_ */
