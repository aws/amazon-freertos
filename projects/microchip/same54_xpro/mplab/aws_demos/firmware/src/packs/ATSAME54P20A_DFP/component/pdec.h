/**
 * \brief Component description for PDEC
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
#ifndef _SAME54_PDEC_COMPONENT_H_
#define _SAME54_PDEC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR PDEC                                         */
/* ************************************************************************** */

/* -------- PDEC_CTRLA : (PDEC Offset: 0x00) (R/W 32) Control A -------- */
#define PDEC_CTRLA_RESETVALUE                 _U_(0x00)                                            /**<  (PDEC_CTRLA) Control A  Reset Value */

#define PDEC_CTRLA_SWRST_Pos                  _U_(0)                                               /**< (PDEC_CTRLA) Software Reset Position */
#define PDEC_CTRLA_SWRST_Msk                  (_U_(0x1) << PDEC_CTRLA_SWRST_Pos)                   /**< (PDEC_CTRLA) Software Reset Mask */
#define PDEC_CTRLA_SWRST(value)               (PDEC_CTRLA_SWRST_Msk & ((value) << PDEC_CTRLA_SWRST_Pos))
#define PDEC_CTRLA_ENABLE_Pos                 _U_(1)                                               /**< (PDEC_CTRLA) Enable Position */
#define PDEC_CTRLA_ENABLE_Msk                 (_U_(0x1) << PDEC_CTRLA_ENABLE_Pos)                  /**< (PDEC_CTRLA) Enable Mask */
#define PDEC_CTRLA_ENABLE(value)              (PDEC_CTRLA_ENABLE_Msk & ((value) << PDEC_CTRLA_ENABLE_Pos))
#define PDEC_CTRLA_MODE_Pos                   _U_(2)                                               /**< (PDEC_CTRLA) Operation Mode Position */
#define PDEC_CTRLA_MODE_Msk                   (_U_(0x3) << PDEC_CTRLA_MODE_Pos)                    /**< (PDEC_CTRLA) Operation Mode Mask */
#define PDEC_CTRLA_MODE(value)                (PDEC_CTRLA_MODE_Msk & ((value) << PDEC_CTRLA_MODE_Pos))
#define   PDEC_CTRLA_MODE_QDEC_Val            _U_(0x0)                                             /**< (PDEC_CTRLA) QDEC operating mode  */
#define   PDEC_CTRLA_MODE_HALL_Val            _U_(0x1)                                             /**< (PDEC_CTRLA) HALL operating mode  */
#define   PDEC_CTRLA_MODE_COUNTER_Val         _U_(0x2)                                             /**< (PDEC_CTRLA) COUNTER operating mode  */
#define PDEC_CTRLA_MODE_QDEC                  (PDEC_CTRLA_MODE_QDEC_Val << PDEC_CTRLA_MODE_Pos)    /**< (PDEC_CTRLA) QDEC operating mode Position  */
#define PDEC_CTRLA_MODE_HALL                  (PDEC_CTRLA_MODE_HALL_Val << PDEC_CTRLA_MODE_Pos)    /**< (PDEC_CTRLA) HALL operating mode Position  */
#define PDEC_CTRLA_MODE_COUNTER               (PDEC_CTRLA_MODE_COUNTER_Val << PDEC_CTRLA_MODE_Pos) /**< (PDEC_CTRLA) COUNTER operating mode Position  */
#define PDEC_CTRLA_RUNSTDBY_Pos               _U_(6)                                               /**< (PDEC_CTRLA) Run in Standby Position */
#define PDEC_CTRLA_RUNSTDBY_Msk               (_U_(0x1) << PDEC_CTRLA_RUNSTDBY_Pos)                /**< (PDEC_CTRLA) Run in Standby Mask */
#define PDEC_CTRLA_RUNSTDBY(value)            (PDEC_CTRLA_RUNSTDBY_Msk & ((value) << PDEC_CTRLA_RUNSTDBY_Pos))
#define PDEC_CTRLA_CONF_Pos                   _U_(8)                                               /**< (PDEC_CTRLA) PDEC Configuration Position */
#define PDEC_CTRLA_CONF_Msk                   (_U_(0x7) << PDEC_CTRLA_CONF_Pos)                    /**< (PDEC_CTRLA) PDEC Configuration Mask */
#define PDEC_CTRLA_CONF(value)                (PDEC_CTRLA_CONF_Msk & ((value) << PDEC_CTRLA_CONF_Pos))
#define   PDEC_CTRLA_CONF_X4_Val              _U_(0x0)                                             /**< (PDEC_CTRLA) Quadrature decoder direction  */
#define   PDEC_CTRLA_CONF_X4S_Val             _U_(0x1)                                             /**< (PDEC_CTRLA) Secure Quadrature decoder direction  */
#define   PDEC_CTRLA_CONF_X2_Val              _U_(0x2)                                             /**< (PDEC_CTRLA) Decoder direction  */
#define   PDEC_CTRLA_CONF_X2S_Val             _U_(0x3)                                             /**< (PDEC_CTRLA) Secure decoder direction  */
#define   PDEC_CTRLA_CONF_AUTOC_Val           _U_(0x4)                                             /**< (PDEC_CTRLA) Auto correction mode  */
#define PDEC_CTRLA_CONF_X4                    (PDEC_CTRLA_CONF_X4_Val << PDEC_CTRLA_CONF_Pos)      /**< (PDEC_CTRLA) Quadrature decoder direction Position  */
#define PDEC_CTRLA_CONF_X4S                   (PDEC_CTRLA_CONF_X4S_Val << PDEC_CTRLA_CONF_Pos)     /**< (PDEC_CTRLA) Secure Quadrature decoder direction Position  */
#define PDEC_CTRLA_CONF_X2                    (PDEC_CTRLA_CONF_X2_Val << PDEC_CTRLA_CONF_Pos)      /**< (PDEC_CTRLA) Decoder direction Position  */
#define PDEC_CTRLA_CONF_X2S                   (PDEC_CTRLA_CONF_X2S_Val << PDEC_CTRLA_CONF_Pos)     /**< (PDEC_CTRLA) Secure decoder direction Position  */
#define PDEC_CTRLA_CONF_AUTOC                 (PDEC_CTRLA_CONF_AUTOC_Val << PDEC_CTRLA_CONF_Pos)   /**< (PDEC_CTRLA) Auto correction mode Position  */
#define PDEC_CTRLA_ALOCK_Pos                  _U_(11)                                              /**< (PDEC_CTRLA) Auto Lock Position */
#define PDEC_CTRLA_ALOCK_Msk                  (_U_(0x1) << PDEC_CTRLA_ALOCK_Pos)                   /**< (PDEC_CTRLA) Auto Lock Mask */
#define PDEC_CTRLA_ALOCK(value)               (PDEC_CTRLA_ALOCK_Msk & ((value) << PDEC_CTRLA_ALOCK_Pos))
#define PDEC_CTRLA_SWAP_Pos                   _U_(14)                                              /**< (PDEC_CTRLA) PDEC Phase A and B Swap Position */
#define PDEC_CTRLA_SWAP_Msk                   (_U_(0x1) << PDEC_CTRLA_SWAP_Pos)                    /**< (PDEC_CTRLA) PDEC Phase A and B Swap Mask */
#define PDEC_CTRLA_SWAP(value)                (PDEC_CTRLA_SWAP_Msk & ((value) << PDEC_CTRLA_SWAP_Pos))
#define PDEC_CTRLA_PEREN_Pos                  _U_(15)                                              /**< (PDEC_CTRLA) Period Enable Position */
#define PDEC_CTRLA_PEREN_Msk                  (_U_(0x1) << PDEC_CTRLA_PEREN_Pos)                   /**< (PDEC_CTRLA) Period Enable Mask */
#define PDEC_CTRLA_PEREN(value)               (PDEC_CTRLA_PEREN_Msk & ((value) << PDEC_CTRLA_PEREN_Pos))
#define PDEC_CTRLA_PINEN0_Pos                 _U_(16)                                              /**< (PDEC_CTRLA) PDEC Input From Pin 0 Enable Position */
#define PDEC_CTRLA_PINEN0_Msk                 (_U_(0x1) << PDEC_CTRLA_PINEN0_Pos)                  /**< (PDEC_CTRLA) PDEC Input From Pin 0 Enable Mask */
#define PDEC_CTRLA_PINEN0(value)              (PDEC_CTRLA_PINEN0_Msk & ((value) << PDEC_CTRLA_PINEN0_Pos))
#define PDEC_CTRLA_PINEN1_Pos                 _U_(17)                                              /**< (PDEC_CTRLA) PDEC Input From Pin 1 Enable Position */
#define PDEC_CTRLA_PINEN1_Msk                 (_U_(0x1) << PDEC_CTRLA_PINEN1_Pos)                  /**< (PDEC_CTRLA) PDEC Input From Pin 1 Enable Mask */
#define PDEC_CTRLA_PINEN1(value)              (PDEC_CTRLA_PINEN1_Msk & ((value) << PDEC_CTRLA_PINEN1_Pos))
#define PDEC_CTRLA_PINEN2_Pos                 _U_(18)                                              /**< (PDEC_CTRLA) PDEC Input From Pin 2 Enable Position */
#define PDEC_CTRLA_PINEN2_Msk                 (_U_(0x1) << PDEC_CTRLA_PINEN2_Pos)                  /**< (PDEC_CTRLA) PDEC Input From Pin 2 Enable Mask */
#define PDEC_CTRLA_PINEN2(value)              (PDEC_CTRLA_PINEN2_Msk & ((value) << PDEC_CTRLA_PINEN2_Pos))
#define PDEC_CTRLA_PINVEN0_Pos                _U_(20)                                              /**< (PDEC_CTRLA) IO Pin 0 Invert Enable Position */
#define PDEC_CTRLA_PINVEN0_Msk                (_U_(0x1) << PDEC_CTRLA_PINVEN0_Pos)                 /**< (PDEC_CTRLA) IO Pin 0 Invert Enable Mask */
#define PDEC_CTRLA_PINVEN0(value)             (PDEC_CTRLA_PINVEN0_Msk & ((value) << PDEC_CTRLA_PINVEN0_Pos))
#define PDEC_CTRLA_PINVEN1_Pos                _U_(21)                                              /**< (PDEC_CTRLA) IO Pin 1 Invert Enable Position */
#define PDEC_CTRLA_PINVEN1_Msk                (_U_(0x1) << PDEC_CTRLA_PINVEN1_Pos)                 /**< (PDEC_CTRLA) IO Pin 1 Invert Enable Mask */
#define PDEC_CTRLA_PINVEN1(value)             (PDEC_CTRLA_PINVEN1_Msk & ((value) << PDEC_CTRLA_PINVEN1_Pos))
#define PDEC_CTRLA_PINVEN2_Pos                _U_(22)                                              /**< (PDEC_CTRLA) IO Pin 2 Invert Enable Position */
#define PDEC_CTRLA_PINVEN2_Msk                (_U_(0x1) << PDEC_CTRLA_PINVEN2_Pos)                 /**< (PDEC_CTRLA) IO Pin 2 Invert Enable Mask */
#define PDEC_CTRLA_PINVEN2(value)             (PDEC_CTRLA_PINVEN2_Msk & ((value) << PDEC_CTRLA_PINVEN2_Pos))
#define PDEC_CTRLA_ANGULAR_Pos                _U_(24)                                              /**< (PDEC_CTRLA) Angular Counter Length Position */
#define PDEC_CTRLA_ANGULAR_Msk                (_U_(0x7) << PDEC_CTRLA_ANGULAR_Pos)                 /**< (PDEC_CTRLA) Angular Counter Length Mask */
#define PDEC_CTRLA_ANGULAR(value)             (PDEC_CTRLA_ANGULAR_Msk & ((value) << PDEC_CTRLA_ANGULAR_Pos))
#define PDEC_CTRLA_MAXCMP_Pos                 _U_(28)                                              /**< (PDEC_CTRLA) Maximum Consecutive Missing Pulses Position */
#define PDEC_CTRLA_MAXCMP_Msk                 (_U_(0xF) << PDEC_CTRLA_MAXCMP_Pos)                  /**< (PDEC_CTRLA) Maximum Consecutive Missing Pulses Mask */
#define PDEC_CTRLA_MAXCMP(value)              (PDEC_CTRLA_MAXCMP_Msk & ((value) << PDEC_CTRLA_MAXCMP_Pos))
#define PDEC_CTRLA_Msk                        _U_(0xF777CF4F)                                      /**< (PDEC_CTRLA) Register Mask  */

#define PDEC_CTRLA_PINEN_Pos                  _U_(16)                                              /**< (PDEC_CTRLA Position) PDEC Input From Pin x Enable */
#define PDEC_CTRLA_PINEN_Msk                  (_U_(0x7) << PDEC_CTRLA_PINEN_Pos)                   /**< (PDEC_CTRLA Mask) PINEN */
#define PDEC_CTRLA_PINEN(value)               (PDEC_CTRLA_PINEN_Msk & ((value) << PDEC_CTRLA_PINEN_Pos)) 
#define PDEC_CTRLA_PINVEN_Pos                 _U_(20)                                              /**< (PDEC_CTRLA Position) IO Pin x Invert Enable */
#define PDEC_CTRLA_PINVEN_Msk                 (_U_(0x7) << PDEC_CTRLA_PINVEN_Pos)                  /**< (PDEC_CTRLA Mask) PINVEN */
#define PDEC_CTRLA_PINVEN(value)              (PDEC_CTRLA_PINVEN_Msk & ((value) << PDEC_CTRLA_PINVEN_Pos)) 

/* -------- PDEC_CTRLBCLR : (PDEC Offset: 0x04) (R/W 8) Control B Clear -------- */
#define PDEC_CTRLBCLR_RESETVALUE              _U_(0x00)                                            /**<  (PDEC_CTRLBCLR) Control B Clear  Reset Value */

#define PDEC_CTRLBCLR_LUPD_Pos                _U_(1)                                               /**< (PDEC_CTRLBCLR) Lock Update Position */
#define PDEC_CTRLBCLR_LUPD_Msk                (_U_(0x1) << PDEC_CTRLBCLR_LUPD_Pos)                 /**< (PDEC_CTRLBCLR) Lock Update Mask */
#define PDEC_CTRLBCLR_LUPD(value)             (PDEC_CTRLBCLR_LUPD_Msk & ((value) << PDEC_CTRLBCLR_LUPD_Pos))
#define PDEC_CTRLBCLR_CMD_Pos                 _U_(5)                                               /**< (PDEC_CTRLBCLR) Command Position */
#define PDEC_CTRLBCLR_CMD_Msk                 (_U_(0x7) << PDEC_CTRLBCLR_CMD_Pos)                  /**< (PDEC_CTRLBCLR) Command Mask */
#define PDEC_CTRLBCLR_CMD(value)              (PDEC_CTRLBCLR_CMD_Msk & ((value) << PDEC_CTRLBCLR_CMD_Pos))
#define   PDEC_CTRLBCLR_CMD_NONE_Val          _U_(0x0)                                             /**< (PDEC_CTRLBCLR) No action  */
#define   PDEC_CTRLBCLR_CMD_RETRIGGER_Val     _U_(0x1)                                             /**< (PDEC_CTRLBCLR) Force a counter restart or retrigger  */
#define   PDEC_CTRLBCLR_CMD_UPDATE_Val        _U_(0x2)                                             /**< (PDEC_CTRLBCLR) Force update of double buffered registers  */
#define   PDEC_CTRLBCLR_CMD_READSYNC_Val      _U_(0x3)                                             /**< (PDEC_CTRLBCLR) Force a read synchronization of COUNT  */
#define   PDEC_CTRLBCLR_CMD_START_Val         _U_(0x4)                                             /**< (PDEC_CTRLBCLR) Start QDEC/HALL  */
#define   PDEC_CTRLBCLR_CMD_STOP_Val          _U_(0x5)                                             /**< (PDEC_CTRLBCLR) Stop QDEC/HALL  */
#define PDEC_CTRLBCLR_CMD_NONE                (PDEC_CTRLBCLR_CMD_NONE_Val << PDEC_CTRLBCLR_CMD_Pos) /**< (PDEC_CTRLBCLR) No action Position  */
#define PDEC_CTRLBCLR_CMD_RETRIGGER           (PDEC_CTRLBCLR_CMD_RETRIGGER_Val << PDEC_CTRLBCLR_CMD_Pos) /**< (PDEC_CTRLBCLR) Force a counter restart or retrigger Position  */
#define PDEC_CTRLBCLR_CMD_UPDATE              (PDEC_CTRLBCLR_CMD_UPDATE_Val << PDEC_CTRLBCLR_CMD_Pos) /**< (PDEC_CTRLBCLR) Force update of double buffered registers Position  */
#define PDEC_CTRLBCLR_CMD_READSYNC            (PDEC_CTRLBCLR_CMD_READSYNC_Val << PDEC_CTRLBCLR_CMD_Pos) /**< (PDEC_CTRLBCLR) Force a read synchronization of COUNT Position  */
#define PDEC_CTRLBCLR_CMD_START               (PDEC_CTRLBCLR_CMD_START_Val << PDEC_CTRLBCLR_CMD_Pos) /**< (PDEC_CTRLBCLR) Start QDEC/HALL Position  */
#define PDEC_CTRLBCLR_CMD_STOP                (PDEC_CTRLBCLR_CMD_STOP_Val << PDEC_CTRLBCLR_CMD_Pos) /**< (PDEC_CTRLBCLR) Stop QDEC/HALL Position  */
#define PDEC_CTRLBCLR_Msk                     _U_(0xE2)                                            /**< (PDEC_CTRLBCLR) Register Mask  */


/* -------- PDEC_CTRLBSET : (PDEC Offset: 0x05) (R/W 8) Control B Set -------- */
#define PDEC_CTRLBSET_RESETVALUE              _U_(0x00)                                            /**<  (PDEC_CTRLBSET) Control B Set  Reset Value */

#define PDEC_CTRLBSET_LUPD_Pos                _U_(1)                                               /**< (PDEC_CTRLBSET) Lock Update Position */
#define PDEC_CTRLBSET_LUPD_Msk                (_U_(0x1) << PDEC_CTRLBSET_LUPD_Pos)                 /**< (PDEC_CTRLBSET) Lock Update Mask */
#define PDEC_CTRLBSET_LUPD(value)             (PDEC_CTRLBSET_LUPD_Msk & ((value) << PDEC_CTRLBSET_LUPD_Pos))
#define PDEC_CTRLBSET_CMD_Pos                 _U_(5)                                               /**< (PDEC_CTRLBSET) Command Position */
#define PDEC_CTRLBSET_CMD_Msk                 (_U_(0x7) << PDEC_CTRLBSET_CMD_Pos)                  /**< (PDEC_CTRLBSET) Command Mask */
#define PDEC_CTRLBSET_CMD(value)              (PDEC_CTRLBSET_CMD_Msk & ((value) << PDEC_CTRLBSET_CMD_Pos))
#define   PDEC_CTRLBSET_CMD_NONE_Val          _U_(0x0)                                             /**< (PDEC_CTRLBSET) No action  */
#define   PDEC_CTRLBSET_CMD_RETRIGGER_Val     _U_(0x1)                                             /**< (PDEC_CTRLBSET) Force a counter restart or retrigger  */
#define   PDEC_CTRLBSET_CMD_UPDATE_Val        _U_(0x2)                                             /**< (PDEC_CTRLBSET) Force update of double buffered registers  */
#define   PDEC_CTRLBSET_CMD_READSYNC_Val      _U_(0x3)                                             /**< (PDEC_CTRLBSET) Force a read synchronization of COUNT  */
#define   PDEC_CTRLBSET_CMD_START_Val         _U_(0x4)                                             /**< (PDEC_CTRLBSET) Start QDEC/HALL  */
#define   PDEC_CTRLBSET_CMD_STOP_Val          _U_(0x5)                                             /**< (PDEC_CTRLBSET) Stop QDEC/HALL  */
#define PDEC_CTRLBSET_CMD_NONE                (PDEC_CTRLBSET_CMD_NONE_Val << PDEC_CTRLBSET_CMD_Pos) /**< (PDEC_CTRLBSET) No action Position  */
#define PDEC_CTRLBSET_CMD_RETRIGGER           (PDEC_CTRLBSET_CMD_RETRIGGER_Val << PDEC_CTRLBSET_CMD_Pos) /**< (PDEC_CTRLBSET) Force a counter restart or retrigger Position  */
#define PDEC_CTRLBSET_CMD_UPDATE              (PDEC_CTRLBSET_CMD_UPDATE_Val << PDEC_CTRLBSET_CMD_Pos) /**< (PDEC_CTRLBSET) Force update of double buffered registers Position  */
#define PDEC_CTRLBSET_CMD_READSYNC            (PDEC_CTRLBSET_CMD_READSYNC_Val << PDEC_CTRLBSET_CMD_Pos) /**< (PDEC_CTRLBSET) Force a read synchronization of COUNT Position  */
#define PDEC_CTRLBSET_CMD_START               (PDEC_CTRLBSET_CMD_START_Val << PDEC_CTRLBSET_CMD_Pos) /**< (PDEC_CTRLBSET) Start QDEC/HALL Position  */
#define PDEC_CTRLBSET_CMD_STOP                (PDEC_CTRLBSET_CMD_STOP_Val << PDEC_CTRLBSET_CMD_Pos) /**< (PDEC_CTRLBSET) Stop QDEC/HALL Position  */
#define PDEC_CTRLBSET_Msk                     _U_(0xE2)                                            /**< (PDEC_CTRLBSET) Register Mask  */


/* -------- PDEC_EVCTRL : (PDEC Offset: 0x06) (R/W 16) Event Control -------- */
#define PDEC_EVCTRL_RESETVALUE                _U_(0x00)                                            /**<  (PDEC_EVCTRL) Event Control  Reset Value */

#define PDEC_EVCTRL_EVACT_Pos                 _U_(0)                                               /**< (PDEC_EVCTRL) Event Action Position */
#define PDEC_EVCTRL_EVACT_Msk                 (_U_(0x3) << PDEC_EVCTRL_EVACT_Pos)                  /**< (PDEC_EVCTRL) Event Action Mask */
#define PDEC_EVCTRL_EVACT(value)              (PDEC_EVCTRL_EVACT_Msk & ((value) << PDEC_EVCTRL_EVACT_Pos))
#define   PDEC_EVCTRL_EVACT_OFF_Val           _U_(0x0)                                             /**< (PDEC_EVCTRL) Event action disabled  */
#define   PDEC_EVCTRL_EVACT_RETRIGGER_Val     _U_(0x1)                                             /**< (PDEC_EVCTRL) Start, restart or retrigger on event  */
#define   PDEC_EVCTRL_EVACT_COUNT_Val         _U_(0x2)                                             /**< (PDEC_EVCTRL) Count on event  */
#define PDEC_EVCTRL_EVACT_OFF                 (PDEC_EVCTRL_EVACT_OFF_Val << PDEC_EVCTRL_EVACT_Pos) /**< (PDEC_EVCTRL) Event action disabled Position  */
#define PDEC_EVCTRL_EVACT_RETRIGGER           (PDEC_EVCTRL_EVACT_RETRIGGER_Val << PDEC_EVCTRL_EVACT_Pos) /**< (PDEC_EVCTRL) Start, restart or retrigger on event Position  */
#define PDEC_EVCTRL_EVACT_COUNT               (PDEC_EVCTRL_EVACT_COUNT_Val << PDEC_EVCTRL_EVACT_Pos) /**< (PDEC_EVCTRL) Count on event Position  */
#define PDEC_EVCTRL_EVINV_Pos                 _U_(2)                                               /**< (PDEC_EVCTRL) Inverted Event Input Enable Position */
#define PDEC_EVCTRL_EVINV_Msk                 (_U_(0x7) << PDEC_EVCTRL_EVINV_Pos)                  /**< (PDEC_EVCTRL) Inverted Event Input Enable Mask */
#define PDEC_EVCTRL_EVINV(value)              (PDEC_EVCTRL_EVINV_Msk & ((value) << PDEC_EVCTRL_EVINV_Pos))
#define PDEC_EVCTRL_EVEI_Pos                  _U_(5)                                               /**< (PDEC_EVCTRL) Event Input Enable Position */
#define PDEC_EVCTRL_EVEI_Msk                  (_U_(0x7) << PDEC_EVCTRL_EVEI_Pos)                   /**< (PDEC_EVCTRL) Event Input Enable Mask */
#define PDEC_EVCTRL_EVEI(value)               (PDEC_EVCTRL_EVEI_Msk & ((value) << PDEC_EVCTRL_EVEI_Pos))
#define PDEC_EVCTRL_OVFEO_Pos                 _U_(8)                                               /**< (PDEC_EVCTRL) Overflow/Underflow Output Event Enable Position */
#define PDEC_EVCTRL_OVFEO_Msk                 (_U_(0x1) << PDEC_EVCTRL_OVFEO_Pos)                  /**< (PDEC_EVCTRL) Overflow/Underflow Output Event Enable Mask */
#define PDEC_EVCTRL_OVFEO(value)              (PDEC_EVCTRL_OVFEO_Msk & ((value) << PDEC_EVCTRL_OVFEO_Pos))
#define PDEC_EVCTRL_ERREO_Pos                 _U_(9)                                               /**< (PDEC_EVCTRL) Error  Output Event Enable Position */
#define PDEC_EVCTRL_ERREO_Msk                 (_U_(0x1) << PDEC_EVCTRL_ERREO_Pos)                  /**< (PDEC_EVCTRL) Error  Output Event Enable Mask */
#define PDEC_EVCTRL_ERREO(value)              (PDEC_EVCTRL_ERREO_Msk & ((value) << PDEC_EVCTRL_ERREO_Pos))
#define PDEC_EVCTRL_DIREO_Pos                 _U_(10)                                              /**< (PDEC_EVCTRL) Direction Output Event Enable Position */
#define PDEC_EVCTRL_DIREO_Msk                 (_U_(0x1) << PDEC_EVCTRL_DIREO_Pos)                  /**< (PDEC_EVCTRL) Direction Output Event Enable Mask */
#define PDEC_EVCTRL_DIREO(value)              (PDEC_EVCTRL_DIREO_Msk & ((value) << PDEC_EVCTRL_DIREO_Pos))
#define PDEC_EVCTRL_VLCEO_Pos                 _U_(11)                                              /**< (PDEC_EVCTRL) Velocity Output Event Enable Position */
#define PDEC_EVCTRL_VLCEO_Msk                 (_U_(0x1) << PDEC_EVCTRL_VLCEO_Pos)                  /**< (PDEC_EVCTRL) Velocity Output Event Enable Mask */
#define PDEC_EVCTRL_VLCEO(value)              (PDEC_EVCTRL_VLCEO_Msk & ((value) << PDEC_EVCTRL_VLCEO_Pos))
#define PDEC_EVCTRL_MCEO0_Pos                 _U_(12)                                              /**< (PDEC_EVCTRL) Match Channel 0 Event Output Enable Position */
#define PDEC_EVCTRL_MCEO0_Msk                 (_U_(0x1) << PDEC_EVCTRL_MCEO0_Pos)                  /**< (PDEC_EVCTRL) Match Channel 0 Event Output Enable Mask */
#define PDEC_EVCTRL_MCEO0(value)              (PDEC_EVCTRL_MCEO0_Msk & ((value) << PDEC_EVCTRL_MCEO0_Pos))
#define PDEC_EVCTRL_MCEO1_Pos                 _U_(13)                                              /**< (PDEC_EVCTRL) Match Channel 1 Event Output Enable Position */
#define PDEC_EVCTRL_MCEO1_Msk                 (_U_(0x1) << PDEC_EVCTRL_MCEO1_Pos)                  /**< (PDEC_EVCTRL) Match Channel 1 Event Output Enable Mask */
#define PDEC_EVCTRL_MCEO1(value)              (PDEC_EVCTRL_MCEO1_Msk & ((value) << PDEC_EVCTRL_MCEO1_Pos))
#define PDEC_EVCTRL_Msk                       _U_(0x3FFF)                                          /**< (PDEC_EVCTRL) Register Mask  */

#define PDEC_EVCTRL_MCEO_Pos                  _U_(12)                                              /**< (PDEC_EVCTRL Position) Match Channel x Event Output Enable */
#define PDEC_EVCTRL_MCEO_Msk                  (_U_(0x3) << PDEC_EVCTRL_MCEO_Pos)                   /**< (PDEC_EVCTRL Mask) MCEO */
#define PDEC_EVCTRL_MCEO(value)               (PDEC_EVCTRL_MCEO_Msk & ((value) << PDEC_EVCTRL_MCEO_Pos)) 

/* -------- PDEC_INTENCLR : (PDEC Offset: 0x08) (R/W 8) Interrupt Enable Clear -------- */
#define PDEC_INTENCLR_RESETVALUE              _U_(0x00)                                            /**<  (PDEC_INTENCLR) Interrupt Enable Clear  Reset Value */

#define PDEC_INTENCLR_OVF_Pos                 _U_(0)                                               /**< (PDEC_INTENCLR) Overflow/Underflow Interrupt Disable Position */
#define PDEC_INTENCLR_OVF_Msk                 (_U_(0x1) << PDEC_INTENCLR_OVF_Pos)                  /**< (PDEC_INTENCLR) Overflow/Underflow Interrupt Disable Mask */
#define PDEC_INTENCLR_OVF(value)              (PDEC_INTENCLR_OVF_Msk & ((value) << PDEC_INTENCLR_OVF_Pos))
#define PDEC_INTENCLR_ERR_Pos                 _U_(1)                                               /**< (PDEC_INTENCLR) Error Interrupt Disable Position */
#define PDEC_INTENCLR_ERR_Msk                 (_U_(0x1) << PDEC_INTENCLR_ERR_Pos)                  /**< (PDEC_INTENCLR) Error Interrupt Disable Mask */
#define PDEC_INTENCLR_ERR(value)              (PDEC_INTENCLR_ERR_Msk & ((value) << PDEC_INTENCLR_ERR_Pos))
#define PDEC_INTENCLR_DIR_Pos                 _U_(2)                                               /**< (PDEC_INTENCLR) Direction Interrupt Disable Position */
#define PDEC_INTENCLR_DIR_Msk                 (_U_(0x1) << PDEC_INTENCLR_DIR_Pos)                  /**< (PDEC_INTENCLR) Direction Interrupt Disable Mask */
#define PDEC_INTENCLR_DIR(value)              (PDEC_INTENCLR_DIR_Msk & ((value) << PDEC_INTENCLR_DIR_Pos))
#define PDEC_INTENCLR_VLC_Pos                 _U_(3)                                               /**< (PDEC_INTENCLR) Velocity Interrupt Disable Position */
#define PDEC_INTENCLR_VLC_Msk                 (_U_(0x1) << PDEC_INTENCLR_VLC_Pos)                  /**< (PDEC_INTENCLR) Velocity Interrupt Disable Mask */
#define PDEC_INTENCLR_VLC(value)              (PDEC_INTENCLR_VLC_Msk & ((value) << PDEC_INTENCLR_VLC_Pos))
#define PDEC_INTENCLR_MC0_Pos                 _U_(4)                                               /**< (PDEC_INTENCLR) Channel 0 Compare Match Disable Position */
#define PDEC_INTENCLR_MC0_Msk                 (_U_(0x1) << PDEC_INTENCLR_MC0_Pos)                  /**< (PDEC_INTENCLR) Channel 0 Compare Match Disable Mask */
#define PDEC_INTENCLR_MC0(value)              (PDEC_INTENCLR_MC0_Msk & ((value) << PDEC_INTENCLR_MC0_Pos))
#define PDEC_INTENCLR_MC1_Pos                 _U_(5)                                               /**< (PDEC_INTENCLR) Channel 1 Compare Match Disable Position */
#define PDEC_INTENCLR_MC1_Msk                 (_U_(0x1) << PDEC_INTENCLR_MC1_Pos)                  /**< (PDEC_INTENCLR) Channel 1 Compare Match Disable Mask */
#define PDEC_INTENCLR_MC1(value)              (PDEC_INTENCLR_MC1_Msk & ((value) << PDEC_INTENCLR_MC1_Pos))
#define PDEC_INTENCLR_Msk                     _U_(0x3F)                                            /**< (PDEC_INTENCLR) Register Mask  */

#define PDEC_INTENCLR_MC_Pos                  _U_(4)                                               /**< (PDEC_INTENCLR Position) Channel x Compare Match Disable */
#define PDEC_INTENCLR_MC_Msk                  (_U_(0x3) << PDEC_INTENCLR_MC_Pos)                   /**< (PDEC_INTENCLR Mask) MC */
#define PDEC_INTENCLR_MC(value)               (PDEC_INTENCLR_MC_Msk & ((value) << PDEC_INTENCLR_MC_Pos)) 

/* -------- PDEC_INTENSET : (PDEC Offset: 0x09) (R/W 8) Interrupt Enable Set -------- */
#define PDEC_INTENSET_RESETVALUE              _U_(0x00)                                            /**<  (PDEC_INTENSET) Interrupt Enable Set  Reset Value */

#define PDEC_INTENSET_OVF_Pos                 _U_(0)                                               /**< (PDEC_INTENSET) Overflow/Underflow Interrupt Enable Position */
#define PDEC_INTENSET_OVF_Msk                 (_U_(0x1) << PDEC_INTENSET_OVF_Pos)                  /**< (PDEC_INTENSET) Overflow/Underflow Interrupt Enable Mask */
#define PDEC_INTENSET_OVF(value)              (PDEC_INTENSET_OVF_Msk & ((value) << PDEC_INTENSET_OVF_Pos))
#define PDEC_INTENSET_ERR_Pos                 _U_(1)                                               /**< (PDEC_INTENSET) Error Interrupt Enable Position */
#define PDEC_INTENSET_ERR_Msk                 (_U_(0x1) << PDEC_INTENSET_ERR_Pos)                  /**< (PDEC_INTENSET) Error Interrupt Enable Mask */
#define PDEC_INTENSET_ERR(value)              (PDEC_INTENSET_ERR_Msk & ((value) << PDEC_INTENSET_ERR_Pos))
#define PDEC_INTENSET_DIR_Pos                 _U_(2)                                               /**< (PDEC_INTENSET) Direction Interrupt Enable Position */
#define PDEC_INTENSET_DIR_Msk                 (_U_(0x1) << PDEC_INTENSET_DIR_Pos)                  /**< (PDEC_INTENSET) Direction Interrupt Enable Mask */
#define PDEC_INTENSET_DIR(value)              (PDEC_INTENSET_DIR_Msk & ((value) << PDEC_INTENSET_DIR_Pos))
#define PDEC_INTENSET_VLC_Pos                 _U_(3)                                               /**< (PDEC_INTENSET) Velocity Interrupt Enable Position */
#define PDEC_INTENSET_VLC_Msk                 (_U_(0x1) << PDEC_INTENSET_VLC_Pos)                  /**< (PDEC_INTENSET) Velocity Interrupt Enable Mask */
#define PDEC_INTENSET_VLC(value)              (PDEC_INTENSET_VLC_Msk & ((value) << PDEC_INTENSET_VLC_Pos))
#define PDEC_INTENSET_MC0_Pos                 _U_(4)                                               /**< (PDEC_INTENSET) Channel 0 Compare Match Enable Position */
#define PDEC_INTENSET_MC0_Msk                 (_U_(0x1) << PDEC_INTENSET_MC0_Pos)                  /**< (PDEC_INTENSET) Channel 0 Compare Match Enable Mask */
#define PDEC_INTENSET_MC0(value)              (PDEC_INTENSET_MC0_Msk & ((value) << PDEC_INTENSET_MC0_Pos))
#define PDEC_INTENSET_MC1_Pos                 _U_(5)                                               /**< (PDEC_INTENSET) Channel 1 Compare Match Enable Position */
#define PDEC_INTENSET_MC1_Msk                 (_U_(0x1) << PDEC_INTENSET_MC1_Pos)                  /**< (PDEC_INTENSET) Channel 1 Compare Match Enable Mask */
#define PDEC_INTENSET_MC1(value)              (PDEC_INTENSET_MC1_Msk & ((value) << PDEC_INTENSET_MC1_Pos))
#define PDEC_INTENSET_Msk                     _U_(0x3F)                                            /**< (PDEC_INTENSET) Register Mask  */

#define PDEC_INTENSET_MC_Pos                  _U_(4)                                               /**< (PDEC_INTENSET Position) Channel x Compare Match Enable */
#define PDEC_INTENSET_MC_Msk                  (_U_(0x3) << PDEC_INTENSET_MC_Pos)                   /**< (PDEC_INTENSET Mask) MC */
#define PDEC_INTENSET_MC(value)               (PDEC_INTENSET_MC_Msk & ((value) << PDEC_INTENSET_MC_Pos)) 

/* -------- PDEC_INTFLAG : (PDEC Offset: 0x0A) (R/W 8) Interrupt Flag Status and Clear -------- */
#define PDEC_INTFLAG_RESETVALUE               _U_(0x00)                                            /**<  (PDEC_INTFLAG) Interrupt Flag Status and Clear  Reset Value */

#define PDEC_INTFLAG_OVF_Pos                  _U_(0)                                               /**< (PDEC_INTFLAG) Overflow/Underflow Position */
#define PDEC_INTFLAG_OVF_Msk                  (_U_(0x1) << PDEC_INTFLAG_OVF_Pos)                   /**< (PDEC_INTFLAG) Overflow/Underflow Mask */
#define PDEC_INTFLAG_OVF(value)               (PDEC_INTFLAG_OVF_Msk & ((value) << PDEC_INTFLAG_OVF_Pos))
#define PDEC_INTFLAG_ERR_Pos                  _U_(1)                                               /**< (PDEC_INTFLAG) Error Position */
#define PDEC_INTFLAG_ERR_Msk                  (_U_(0x1) << PDEC_INTFLAG_ERR_Pos)                   /**< (PDEC_INTFLAG) Error Mask */
#define PDEC_INTFLAG_ERR(value)               (PDEC_INTFLAG_ERR_Msk & ((value) << PDEC_INTFLAG_ERR_Pos))
#define PDEC_INTFLAG_DIR_Pos                  _U_(2)                                               /**< (PDEC_INTFLAG) Direction Change Position */
#define PDEC_INTFLAG_DIR_Msk                  (_U_(0x1) << PDEC_INTFLAG_DIR_Pos)                   /**< (PDEC_INTFLAG) Direction Change Mask */
#define PDEC_INTFLAG_DIR(value)               (PDEC_INTFLAG_DIR_Msk & ((value) << PDEC_INTFLAG_DIR_Pos))
#define PDEC_INTFLAG_VLC_Pos                  _U_(3)                                               /**< (PDEC_INTFLAG) Velocity Position */
#define PDEC_INTFLAG_VLC_Msk                  (_U_(0x1) << PDEC_INTFLAG_VLC_Pos)                   /**< (PDEC_INTFLAG) Velocity Mask */
#define PDEC_INTFLAG_VLC(value)               (PDEC_INTFLAG_VLC_Msk & ((value) << PDEC_INTFLAG_VLC_Pos))
#define PDEC_INTFLAG_MC0_Pos                  _U_(4)                                               /**< (PDEC_INTFLAG) Channel 0 Compare Match Position */
#define PDEC_INTFLAG_MC0_Msk                  (_U_(0x1) << PDEC_INTFLAG_MC0_Pos)                   /**< (PDEC_INTFLAG) Channel 0 Compare Match Mask */
#define PDEC_INTFLAG_MC0(value)               (PDEC_INTFLAG_MC0_Msk & ((value) << PDEC_INTFLAG_MC0_Pos))
#define PDEC_INTFLAG_MC1_Pos                  _U_(5)                                               /**< (PDEC_INTFLAG) Channel 1 Compare Match Position */
#define PDEC_INTFLAG_MC1_Msk                  (_U_(0x1) << PDEC_INTFLAG_MC1_Pos)                   /**< (PDEC_INTFLAG) Channel 1 Compare Match Mask */
#define PDEC_INTFLAG_MC1(value)               (PDEC_INTFLAG_MC1_Msk & ((value) << PDEC_INTFLAG_MC1_Pos))
#define PDEC_INTFLAG_Msk                      _U_(0x3F)                                            /**< (PDEC_INTFLAG) Register Mask  */

#define PDEC_INTFLAG_MC_Pos                   _U_(4)                                               /**< (PDEC_INTFLAG Position) Channel x Compare Match */
#define PDEC_INTFLAG_MC_Msk                   (_U_(0x3) << PDEC_INTFLAG_MC_Pos)                    /**< (PDEC_INTFLAG Mask) MC */
#define PDEC_INTFLAG_MC(value)                (PDEC_INTFLAG_MC_Msk & ((value) << PDEC_INTFLAG_MC_Pos)) 

/* -------- PDEC_STATUS : (PDEC Offset: 0x0C) (R/W 16) Status -------- */
#define PDEC_STATUS_RESETVALUE                _U_(0x40)                                            /**<  (PDEC_STATUS) Status  Reset Value */

#define PDEC_STATUS_QERR_Pos                  _U_(0)                                               /**< (PDEC_STATUS) Quadrature Error Flag Position */
#define PDEC_STATUS_QERR_Msk                  (_U_(0x1) << PDEC_STATUS_QERR_Pos)                   /**< (PDEC_STATUS) Quadrature Error Flag Mask */
#define PDEC_STATUS_QERR(value)               (PDEC_STATUS_QERR_Msk & ((value) << PDEC_STATUS_QERR_Pos))
#define PDEC_STATUS_IDXERR_Pos                _U_(1)                                               /**< (PDEC_STATUS) Index Error Flag Position */
#define PDEC_STATUS_IDXERR_Msk                (_U_(0x1) << PDEC_STATUS_IDXERR_Pos)                 /**< (PDEC_STATUS) Index Error Flag Mask */
#define PDEC_STATUS_IDXERR(value)             (PDEC_STATUS_IDXERR_Msk & ((value) << PDEC_STATUS_IDXERR_Pos))
#define PDEC_STATUS_MPERR_Pos                 _U_(2)                                               /**< (PDEC_STATUS) Missing Pulse Error flag Position */
#define PDEC_STATUS_MPERR_Msk                 (_U_(0x1) << PDEC_STATUS_MPERR_Pos)                  /**< (PDEC_STATUS) Missing Pulse Error flag Mask */
#define PDEC_STATUS_MPERR(value)              (PDEC_STATUS_MPERR_Msk & ((value) << PDEC_STATUS_MPERR_Pos))
#define PDEC_STATUS_WINERR_Pos                _U_(4)                                               /**< (PDEC_STATUS) Window Error Flag Position */
#define PDEC_STATUS_WINERR_Msk                (_U_(0x1) << PDEC_STATUS_WINERR_Pos)                 /**< (PDEC_STATUS) Window Error Flag Mask */
#define PDEC_STATUS_WINERR(value)             (PDEC_STATUS_WINERR_Msk & ((value) << PDEC_STATUS_WINERR_Pos))
#define PDEC_STATUS_HERR_Pos                  _U_(5)                                               /**< (PDEC_STATUS) Hall Error Flag Position */
#define PDEC_STATUS_HERR_Msk                  (_U_(0x1) << PDEC_STATUS_HERR_Pos)                   /**< (PDEC_STATUS) Hall Error Flag Mask */
#define PDEC_STATUS_HERR(value)               (PDEC_STATUS_HERR_Msk & ((value) << PDEC_STATUS_HERR_Pos))
#define PDEC_STATUS_STOP_Pos                  _U_(6)                                               /**< (PDEC_STATUS) Stop Position */
#define PDEC_STATUS_STOP_Msk                  (_U_(0x1) << PDEC_STATUS_STOP_Pos)                   /**< (PDEC_STATUS) Stop Mask */
#define PDEC_STATUS_STOP(value)               (PDEC_STATUS_STOP_Msk & ((value) << PDEC_STATUS_STOP_Pos))
#define PDEC_STATUS_DIR_Pos                   _U_(7)                                               /**< (PDEC_STATUS) Direction Status Flag Position */
#define PDEC_STATUS_DIR_Msk                   (_U_(0x1) << PDEC_STATUS_DIR_Pos)                    /**< (PDEC_STATUS) Direction Status Flag Mask */
#define PDEC_STATUS_DIR(value)                (PDEC_STATUS_DIR_Msk & ((value) << PDEC_STATUS_DIR_Pos))
#define PDEC_STATUS_PRESCBUFV_Pos             _U_(8)                                               /**< (PDEC_STATUS) Prescaler Buffer Valid Position */
#define PDEC_STATUS_PRESCBUFV_Msk             (_U_(0x1) << PDEC_STATUS_PRESCBUFV_Pos)              /**< (PDEC_STATUS) Prescaler Buffer Valid Mask */
#define PDEC_STATUS_PRESCBUFV(value)          (PDEC_STATUS_PRESCBUFV_Msk & ((value) << PDEC_STATUS_PRESCBUFV_Pos))
#define PDEC_STATUS_FILTERBUFV_Pos            _U_(9)                                               /**< (PDEC_STATUS) Filter Buffer Valid Position */
#define PDEC_STATUS_FILTERBUFV_Msk            (_U_(0x1) << PDEC_STATUS_FILTERBUFV_Pos)             /**< (PDEC_STATUS) Filter Buffer Valid Mask */
#define PDEC_STATUS_FILTERBUFV(value)         (PDEC_STATUS_FILTERBUFV_Msk & ((value) << PDEC_STATUS_FILTERBUFV_Pos))
#define PDEC_STATUS_CCBUFV0_Pos               _U_(12)                                              /**< (PDEC_STATUS) Compare Channel 0 Buffer Valid Position */
#define PDEC_STATUS_CCBUFV0_Msk               (_U_(0x1) << PDEC_STATUS_CCBUFV0_Pos)                /**< (PDEC_STATUS) Compare Channel 0 Buffer Valid Mask */
#define PDEC_STATUS_CCBUFV0(value)            (PDEC_STATUS_CCBUFV0_Msk & ((value) << PDEC_STATUS_CCBUFV0_Pos))
#define PDEC_STATUS_CCBUFV1_Pos               _U_(13)                                              /**< (PDEC_STATUS) Compare Channel 1 Buffer Valid Position */
#define PDEC_STATUS_CCBUFV1_Msk               (_U_(0x1) << PDEC_STATUS_CCBUFV1_Pos)                /**< (PDEC_STATUS) Compare Channel 1 Buffer Valid Mask */
#define PDEC_STATUS_CCBUFV1(value)            (PDEC_STATUS_CCBUFV1_Msk & ((value) << PDEC_STATUS_CCBUFV1_Pos))
#define PDEC_STATUS_Msk                       _U_(0x33F7)                                          /**< (PDEC_STATUS) Register Mask  */

#define PDEC_STATUS_CCBUFV_Pos                _U_(12)                                              /**< (PDEC_STATUS Position) Compare Channel x Buffer Valid */
#define PDEC_STATUS_CCBUFV_Msk                (_U_(0x3) << PDEC_STATUS_CCBUFV_Pos)                 /**< (PDEC_STATUS Mask) CCBUFV */
#define PDEC_STATUS_CCBUFV(value)             (PDEC_STATUS_CCBUFV_Msk & ((value) << PDEC_STATUS_CCBUFV_Pos)) 

/* -------- PDEC_DBGCTRL : (PDEC Offset: 0x0F) (R/W 8) Debug Control -------- */
#define PDEC_DBGCTRL_RESETVALUE               _U_(0x00)                                            /**<  (PDEC_DBGCTRL) Debug Control  Reset Value */

#define PDEC_DBGCTRL_DBGRUN_Pos               _U_(0)                                               /**< (PDEC_DBGCTRL) Debug Run Mode Position */
#define PDEC_DBGCTRL_DBGRUN_Msk               (_U_(0x1) << PDEC_DBGCTRL_DBGRUN_Pos)                /**< (PDEC_DBGCTRL) Debug Run Mode Mask */
#define PDEC_DBGCTRL_DBGRUN(value)            (PDEC_DBGCTRL_DBGRUN_Msk & ((value) << PDEC_DBGCTRL_DBGRUN_Pos))
#define PDEC_DBGCTRL_Msk                      _U_(0x01)                                            /**< (PDEC_DBGCTRL) Register Mask  */


/* -------- PDEC_SYNCBUSY : (PDEC Offset: 0x10) ( R/ 32) Synchronization Status -------- */
#define PDEC_SYNCBUSY_RESETVALUE              _U_(0x00)                                            /**<  (PDEC_SYNCBUSY) Synchronization Status  Reset Value */

#define PDEC_SYNCBUSY_SWRST_Pos               _U_(0)                                               /**< (PDEC_SYNCBUSY) Software Reset Synchronization Busy Position */
#define PDEC_SYNCBUSY_SWRST_Msk               (_U_(0x1) << PDEC_SYNCBUSY_SWRST_Pos)                /**< (PDEC_SYNCBUSY) Software Reset Synchronization Busy Mask */
#define PDEC_SYNCBUSY_SWRST(value)            (PDEC_SYNCBUSY_SWRST_Msk & ((value) << PDEC_SYNCBUSY_SWRST_Pos))
#define PDEC_SYNCBUSY_ENABLE_Pos              _U_(1)                                               /**< (PDEC_SYNCBUSY) Enable Synchronization Busy Position */
#define PDEC_SYNCBUSY_ENABLE_Msk              (_U_(0x1) << PDEC_SYNCBUSY_ENABLE_Pos)               /**< (PDEC_SYNCBUSY) Enable Synchronization Busy Mask */
#define PDEC_SYNCBUSY_ENABLE(value)           (PDEC_SYNCBUSY_ENABLE_Msk & ((value) << PDEC_SYNCBUSY_ENABLE_Pos))
#define PDEC_SYNCBUSY_CTRLB_Pos               _U_(2)                                               /**< (PDEC_SYNCBUSY) Control B Synchronization Busy Position */
#define PDEC_SYNCBUSY_CTRLB_Msk               (_U_(0x1) << PDEC_SYNCBUSY_CTRLB_Pos)                /**< (PDEC_SYNCBUSY) Control B Synchronization Busy Mask */
#define PDEC_SYNCBUSY_CTRLB(value)            (PDEC_SYNCBUSY_CTRLB_Msk & ((value) << PDEC_SYNCBUSY_CTRLB_Pos))
#define PDEC_SYNCBUSY_STATUS_Pos              _U_(3)                                               /**< (PDEC_SYNCBUSY) Status Synchronization Busy Position */
#define PDEC_SYNCBUSY_STATUS_Msk              (_U_(0x1) << PDEC_SYNCBUSY_STATUS_Pos)               /**< (PDEC_SYNCBUSY) Status Synchronization Busy Mask */
#define PDEC_SYNCBUSY_STATUS(value)           (PDEC_SYNCBUSY_STATUS_Msk & ((value) << PDEC_SYNCBUSY_STATUS_Pos))
#define PDEC_SYNCBUSY_PRESC_Pos               _U_(4)                                               /**< (PDEC_SYNCBUSY) Prescaler Synchronization Busy Position */
#define PDEC_SYNCBUSY_PRESC_Msk               (_U_(0x1) << PDEC_SYNCBUSY_PRESC_Pos)                /**< (PDEC_SYNCBUSY) Prescaler Synchronization Busy Mask */
#define PDEC_SYNCBUSY_PRESC(value)            (PDEC_SYNCBUSY_PRESC_Msk & ((value) << PDEC_SYNCBUSY_PRESC_Pos))
#define PDEC_SYNCBUSY_FILTER_Pos              _U_(5)                                               /**< (PDEC_SYNCBUSY) Filter Synchronization Busy Position */
#define PDEC_SYNCBUSY_FILTER_Msk              (_U_(0x1) << PDEC_SYNCBUSY_FILTER_Pos)               /**< (PDEC_SYNCBUSY) Filter Synchronization Busy Mask */
#define PDEC_SYNCBUSY_FILTER(value)           (PDEC_SYNCBUSY_FILTER_Msk & ((value) << PDEC_SYNCBUSY_FILTER_Pos))
#define PDEC_SYNCBUSY_COUNT_Pos               _U_(6)                                               /**< (PDEC_SYNCBUSY) Count Synchronization Busy Position */
#define PDEC_SYNCBUSY_COUNT_Msk               (_U_(0x1) << PDEC_SYNCBUSY_COUNT_Pos)                /**< (PDEC_SYNCBUSY) Count Synchronization Busy Mask */
#define PDEC_SYNCBUSY_COUNT(value)            (PDEC_SYNCBUSY_COUNT_Msk & ((value) << PDEC_SYNCBUSY_COUNT_Pos))
#define PDEC_SYNCBUSY_CC0_Pos                 _U_(7)                                               /**< (PDEC_SYNCBUSY) Compare Channel 0 Synchronization Busy Position */
#define PDEC_SYNCBUSY_CC0_Msk                 (_U_(0x1) << PDEC_SYNCBUSY_CC0_Pos)                  /**< (PDEC_SYNCBUSY) Compare Channel 0 Synchronization Busy Mask */
#define PDEC_SYNCBUSY_CC0(value)              (PDEC_SYNCBUSY_CC0_Msk & ((value) << PDEC_SYNCBUSY_CC0_Pos))
#define PDEC_SYNCBUSY_CC1_Pos                 _U_(8)                                               /**< (PDEC_SYNCBUSY) Compare Channel 1 Synchronization Busy Position */
#define PDEC_SYNCBUSY_CC1_Msk                 (_U_(0x1) << PDEC_SYNCBUSY_CC1_Pos)                  /**< (PDEC_SYNCBUSY) Compare Channel 1 Synchronization Busy Mask */
#define PDEC_SYNCBUSY_CC1(value)              (PDEC_SYNCBUSY_CC1_Msk & ((value) << PDEC_SYNCBUSY_CC1_Pos))
#define PDEC_SYNCBUSY_Msk                     _U_(0x000001FF)                                      /**< (PDEC_SYNCBUSY) Register Mask  */

#define PDEC_SYNCBUSY_CC_Pos                  _U_(7)                                               /**< (PDEC_SYNCBUSY Position) Compare Channel x Synchronization Busy */
#define PDEC_SYNCBUSY_CC_Msk                  (_U_(0x3) << PDEC_SYNCBUSY_CC_Pos)                   /**< (PDEC_SYNCBUSY Mask) CC */
#define PDEC_SYNCBUSY_CC(value)               (PDEC_SYNCBUSY_CC_Msk & ((value) << PDEC_SYNCBUSY_CC_Pos)) 

/* -------- PDEC_PRESC : (PDEC Offset: 0x14) (R/W 8) Prescaler Value -------- */
#define PDEC_PRESC_RESETVALUE                 _U_(0x00)                                            /**<  (PDEC_PRESC) Prescaler Value  Reset Value */

#define PDEC_PRESC_PRESC_Pos                  _U_(0)                                               /**< (PDEC_PRESC) Prescaler Value Position */
#define PDEC_PRESC_PRESC_Msk                  (_U_(0xF) << PDEC_PRESC_PRESC_Pos)                   /**< (PDEC_PRESC) Prescaler Value Mask */
#define PDEC_PRESC_PRESC(value)               (PDEC_PRESC_PRESC_Msk & ((value) << PDEC_PRESC_PRESC_Pos))
#define   PDEC_PRESC_PRESC_DIV1_Val           _U_(0x0)                                             /**< (PDEC_PRESC) No division  */
#define   PDEC_PRESC_PRESC_DIV2_Val           _U_(0x1)                                             /**< (PDEC_PRESC) Divide by 2  */
#define   PDEC_PRESC_PRESC_DIV4_Val           _U_(0x2)                                             /**< (PDEC_PRESC) Divide by 4  */
#define   PDEC_PRESC_PRESC_DIV8_Val           _U_(0x3)                                             /**< (PDEC_PRESC) Divide by 8  */
#define   PDEC_PRESC_PRESC_DIV16_Val          _U_(0x4)                                             /**< (PDEC_PRESC) Divide by 16  */
#define   PDEC_PRESC_PRESC_DIV32_Val          _U_(0x5)                                             /**< (PDEC_PRESC) Divide by 32  */
#define   PDEC_PRESC_PRESC_DIV64_Val          _U_(0x6)                                             /**< (PDEC_PRESC) Divide by 64  */
#define   PDEC_PRESC_PRESC_DIV128_Val         _U_(0x7)                                             /**< (PDEC_PRESC) Divide by 128  */
#define   PDEC_PRESC_PRESC_DIV256_Val         _U_(0x8)                                             /**< (PDEC_PRESC) Divide by 256  */
#define   PDEC_PRESC_PRESC_DIV512_Val         _U_(0x9)                                             /**< (PDEC_PRESC) Divide by 512  */
#define   PDEC_PRESC_PRESC_DIV1024_Val        _U_(0xA)                                             /**< (PDEC_PRESC) Divide by 1024  */
#define PDEC_PRESC_PRESC_DIV1                 (PDEC_PRESC_PRESC_DIV1_Val << PDEC_PRESC_PRESC_Pos)  /**< (PDEC_PRESC) No division Position  */
#define PDEC_PRESC_PRESC_DIV2                 (PDEC_PRESC_PRESC_DIV2_Val << PDEC_PRESC_PRESC_Pos)  /**< (PDEC_PRESC) Divide by 2 Position  */
#define PDEC_PRESC_PRESC_DIV4                 (PDEC_PRESC_PRESC_DIV4_Val << PDEC_PRESC_PRESC_Pos)  /**< (PDEC_PRESC) Divide by 4 Position  */
#define PDEC_PRESC_PRESC_DIV8                 (PDEC_PRESC_PRESC_DIV8_Val << PDEC_PRESC_PRESC_Pos)  /**< (PDEC_PRESC) Divide by 8 Position  */
#define PDEC_PRESC_PRESC_DIV16                (PDEC_PRESC_PRESC_DIV16_Val << PDEC_PRESC_PRESC_Pos) /**< (PDEC_PRESC) Divide by 16 Position  */
#define PDEC_PRESC_PRESC_DIV32                (PDEC_PRESC_PRESC_DIV32_Val << PDEC_PRESC_PRESC_Pos) /**< (PDEC_PRESC) Divide by 32 Position  */
#define PDEC_PRESC_PRESC_DIV64                (PDEC_PRESC_PRESC_DIV64_Val << PDEC_PRESC_PRESC_Pos) /**< (PDEC_PRESC) Divide by 64 Position  */
#define PDEC_PRESC_PRESC_DIV128               (PDEC_PRESC_PRESC_DIV128_Val << PDEC_PRESC_PRESC_Pos) /**< (PDEC_PRESC) Divide by 128 Position  */
#define PDEC_PRESC_PRESC_DIV256               (PDEC_PRESC_PRESC_DIV256_Val << PDEC_PRESC_PRESC_Pos) /**< (PDEC_PRESC) Divide by 256 Position  */
#define PDEC_PRESC_PRESC_DIV512               (PDEC_PRESC_PRESC_DIV512_Val << PDEC_PRESC_PRESC_Pos) /**< (PDEC_PRESC) Divide by 512 Position  */
#define PDEC_PRESC_PRESC_DIV1024              (PDEC_PRESC_PRESC_DIV1024_Val << PDEC_PRESC_PRESC_Pos) /**< (PDEC_PRESC) Divide by 1024 Position  */
#define PDEC_PRESC_Msk                        _U_(0x0F)                                            /**< (PDEC_PRESC) Register Mask  */


/* -------- PDEC_FILTER : (PDEC Offset: 0x15) (R/W 8) Filter Value -------- */
#define PDEC_FILTER_RESETVALUE                _U_(0x00)                                            /**<  (PDEC_FILTER) Filter Value  Reset Value */

#define PDEC_FILTER_FILTER_Pos                _U_(0)                                               /**< (PDEC_FILTER) Filter Value Position */
#define PDEC_FILTER_FILTER_Msk                (_U_(0xFF) << PDEC_FILTER_FILTER_Pos)                /**< (PDEC_FILTER) Filter Value Mask */
#define PDEC_FILTER_FILTER(value)             (PDEC_FILTER_FILTER_Msk & ((value) << PDEC_FILTER_FILTER_Pos))
#define PDEC_FILTER_Msk                       _U_(0xFF)                                            /**< (PDEC_FILTER) Register Mask  */


/* -------- PDEC_PRESCBUF : (PDEC Offset: 0x18) (R/W 8) Prescaler Buffer Value -------- */
#define PDEC_PRESCBUF_RESETVALUE              _U_(0x00)                                            /**<  (PDEC_PRESCBUF) Prescaler Buffer Value  Reset Value */

#define PDEC_PRESCBUF_PRESCBUF_Pos            _U_(0)                                               /**< (PDEC_PRESCBUF) Prescaler Buffer Value Position */
#define PDEC_PRESCBUF_PRESCBUF_Msk            (_U_(0xF) << PDEC_PRESCBUF_PRESCBUF_Pos)             /**< (PDEC_PRESCBUF) Prescaler Buffer Value Mask */
#define PDEC_PRESCBUF_PRESCBUF(value)         (PDEC_PRESCBUF_PRESCBUF_Msk & ((value) << PDEC_PRESCBUF_PRESCBUF_Pos))
#define   PDEC_PRESCBUF_PRESCBUF_DIV1_Val     _U_(0x0)                                             /**< (PDEC_PRESCBUF) No division  */
#define   PDEC_PRESCBUF_PRESCBUF_DIV2_Val     _U_(0x1)                                             /**< (PDEC_PRESCBUF) Divide by 2  */
#define   PDEC_PRESCBUF_PRESCBUF_DIV4_Val     _U_(0x2)                                             /**< (PDEC_PRESCBUF) Divide by 4  */
#define   PDEC_PRESCBUF_PRESCBUF_DIV8_Val     _U_(0x3)                                             /**< (PDEC_PRESCBUF) Divide by 8  */
#define   PDEC_PRESCBUF_PRESCBUF_DIV16_Val    _U_(0x4)                                             /**< (PDEC_PRESCBUF) Divide by 16  */
#define   PDEC_PRESCBUF_PRESCBUF_DIV32_Val    _U_(0x5)                                             /**< (PDEC_PRESCBUF) Divide by 32  */
#define   PDEC_PRESCBUF_PRESCBUF_DIV64_Val    _U_(0x6)                                             /**< (PDEC_PRESCBUF) Divide by 64  */
#define   PDEC_PRESCBUF_PRESCBUF_DIV128_Val   _U_(0x7)                                             /**< (PDEC_PRESCBUF) Divide by 128  */
#define   PDEC_PRESCBUF_PRESCBUF_DIV256_Val   _U_(0x8)                                             /**< (PDEC_PRESCBUF) Divide by 256  */
#define   PDEC_PRESCBUF_PRESCBUF_DIV512_Val   _U_(0x9)                                             /**< (PDEC_PRESCBUF) Divide by 512  */
#define   PDEC_PRESCBUF_PRESCBUF_DIV1024_Val  _U_(0xA)                                             /**< (PDEC_PRESCBUF) Divide by 1024  */
#define PDEC_PRESCBUF_PRESCBUF_DIV1           (PDEC_PRESCBUF_PRESCBUF_DIV1_Val << PDEC_PRESCBUF_PRESCBUF_Pos) /**< (PDEC_PRESCBUF) No division Position  */
#define PDEC_PRESCBUF_PRESCBUF_DIV2           (PDEC_PRESCBUF_PRESCBUF_DIV2_Val << PDEC_PRESCBUF_PRESCBUF_Pos) /**< (PDEC_PRESCBUF) Divide by 2 Position  */
#define PDEC_PRESCBUF_PRESCBUF_DIV4           (PDEC_PRESCBUF_PRESCBUF_DIV4_Val << PDEC_PRESCBUF_PRESCBUF_Pos) /**< (PDEC_PRESCBUF) Divide by 4 Position  */
#define PDEC_PRESCBUF_PRESCBUF_DIV8           (PDEC_PRESCBUF_PRESCBUF_DIV8_Val << PDEC_PRESCBUF_PRESCBUF_Pos) /**< (PDEC_PRESCBUF) Divide by 8 Position  */
#define PDEC_PRESCBUF_PRESCBUF_DIV16          (PDEC_PRESCBUF_PRESCBUF_DIV16_Val << PDEC_PRESCBUF_PRESCBUF_Pos) /**< (PDEC_PRESCBUF) Divide by 16 Position  */
#define PDEC_PRESCBUF_PRESCBUF_DIV32          (PDEC_PRESCBUF_PRESCBUF_DIV32_Val << PDEC_PRESCBUF_PRESCBUF_Pos) /**< (PDEC_PRESCBUF) Divide by 32 Position  */
#define PDEC_PRESCBUF_PRESCBUF_DIV64          (PDEC_PRESCBUF_PRESCBUF_DIV64_Val << PDEC_PRESCBUF_PRESCBUF_Pos) /**< (PDEC_PRESCBUF) Divide by 64 Position  */
#define PDEC_PRESCBUF_PRESCBUF_DIV128         (PDEC_PRESCBUF_PRESCBUF_DIV128_Val << PDEC_PRESCBUF_PRESCBUF_Pos) /**< (PDEC_PRESCBUF) Divide by 128 Position  */
#define PDEC_PRESCBUF_PRESCBUF_DIV256         (PDEC_PRESCBUF_PRESCBUF_DIV256_Val << PDEC_PRESCBUF_PRESCBUF_Pos) /**< (PDEC_PRESCBUF) Divide by 256 Position  */
#define PDEC_PRESCBUF_PRESCBUF_DIV512         (PDEC_PRESCBUF_PRESCBUF_DIV512_Val << PDEC_PRESCBUF_PRESCBUF_Pos) /**< (PDEC_PRESCBUF) Divide by 512 Position  */
#define PDEC_PRESCBUF_PRESCBUF_DIV1024        (PDEC_PRESCBUF_PRESCBUF_DIV1024_Val << PDEC_PRESCBUF_PRESCBUF_Pos) /**< (PDEC_PRESCBUF) Divide by 1024 Position  */
#define PDEC_PRESCBUF_Msk                     _U_(0x0F)                                            /**< (PDEC_PRESCBUF) Register Mask  */


/* -------- PDEC_FILTERBUF : (PDEC Offset: 0x19) (R/W 8) Filter Buffer Value -------- */
#define PDEC_FILTERBUF_RESETVALUE             _U_(0x00)                                            /**<  (PDEC_FILTERBUF) Filter Buffer Value  Reset Value */

#define PDEC_FILTERBUF_FILTERBUF_Pos          _U_(0)                                               /**< (PDEC_FILTERBUF) Filter Buffer Value Position */
#define PDEC_FILTERBUF_FILTERBUF_Msk          (_U_(0xFF) << PDEC_FILTERBUF_FILTERBUF_Pos)          /**< (PDEC_FILTERBUF) Filter Buffer Value Mask */
#define PDEC_FILTERBUF_FILTERBUF(value)       (PDEC_FILTERBUF_FILTERBUF_Msk & ((value) << PDEC_FILTERBUF_FILTERBUF_Pos))
#define PDEC_FILTERBUF_Msk                    _U_(0xFF)                                            /**< (PDEC_FILTERBUF) Register Mask  */


/* -------- PDEC_COUNT : (PDEC Offset: 0x1C) (R/W 32) Counter Value -------- */
#define PDEC_COUNT_RESETVALUE                 _U_(0x00)                                            /**<  (PDEC_COUNT) Counter Value  Reset Value */

#define PDEC_COUNT_COUNT_Pos                  _U_(0)                                               /**< (PDEC_COUNT) Counter Value Position */
#define PDEC_COUNT_COUNT_Msk                  (_U_(0xFFFF) << PDEC_COUNT_COUNT_Pos)                /**< (PDEC_COUNT) Counter Value Mask */
#define PDEC_COUNT_COUNT(value)               (PDEC_COUNT_COUNT_Msk & ((value) << PDEC_COUNT_COUNT_Pos))
#define PDEC_COUNT_Msk                        _U_(0x0000FFFF)                                      /**< (PDEC_COUNT) Register Mask  */


/* -------- PDEC_CC : (PDEC Offset: 0x20) (R/W 32) Channel n Compare Value -------- */
#define PDEC_CC_RESETVALUE                    _U_(0x00)                                            /**<  (PDEC_CC) Channel n Compare Value  Reset Value */

#define PDEC_CC_CC_Pos                        _U_(0)                                               /**< (PDEC_CC) Channel Compare Value Position */
#define PDEC_CC_CC_Msk                        (_U_(0xFFFF) << PDEC_CC_CC_Pos)                      /**< (PDEC_CC) Channel Compare Value Mask */
#define PDEC_CC_CC(value)                     (PDEC_CC_CC_Msk & ((value) << PDEC_CC_CC_Pos))      
#define PDEC_CC_Msk                           _U_(0x0000FFFF)                                      /**< (PDEC_CC) Register Mask  */


/* -------- PDEC_CCBUF : (PDEC Offset: 0x30) (R/W 32) Channel Compare Buffer Value -------- */
#define PDEC_CCBUF_RESETVALUE                 _U_(0x00)                                            /**<  (PDEC_CCBUF) Channel Compare Buffer Value  Reset Value */

#define PDEC_CCBUF_CCBUF_Pos                  _U_(0)                                               /**< (PDEC_CCBUF) Channel Compare Buffer Value Position */
#define PDEC_CCBUF_CCBUF_Msk                  (_U_(0xFFFF) << PDEC_CCBUF_CCBUF_Pos)                /**< (PDEC_CCBUF) Channel Compare Buffer Value Mask */
#define PDEC_CCBUF_CCBUF(value)               (PDEC_CCBUF_CCBUF_Msk & ((value) << PDEC_CCBUF_CCBUF_Pos))
#define PDEC_CCBUF_Msk                        _U_(0x0000FFFF)                                      /**< (PDEC_CCBUF) Register Mask  */


/** \brief PDEC register offsets definitions */
#define PDEC_CTRLA_REG_OFST            (0x00)              /**< (PDEC_CTRLA) Control A Offset */
#define PDEC_CTRLBCLR_REG_OFST         (0x04)              /**< (PDEC_CTRLBCLR) Control B Clear Offset */
#define PDEC_CTRLBSET_REG_OFST         (0x05)              /**< (PDEC_CTRLBSET) Control B Set Offset */
#define PDEC_EVCTRL_REG_OFST           (0x06)              /**< (PDEC_EVCTRL) Event Control Offset */
#define PDEC_INTENCLR_REG_OFST         (0x08)              /**< (PDEC_INTENCLR) Interrupt Enable Clear Offset */
#define PDEC_INTENSET_REG_OFST         (0x09)              /**< (PDEC_INTENSET) Interrupt Enable Set Offset */
#define PDEC_INTFLAG_REG_OFST          (0x0A)              /**< (PDEC_INTFLAG) Interrupt Flag Status and Clear Offset */
#define PDEC_STATUS_REG_OFST           (0x0C)              /**< (PDEC_STATUS) Status Offset */
#define PDEC_DBGCTRL_REG_OFST          (0x0F)              /**< (PDEC_DBGCTRL) Debug Control Offset */
#define PDEC_SYNCBUSY_REG_OFST         (0x10)              /**< (PDEC_SYNCBUSY) Synchronization Status Offset */
#define PDEC_PRESC_REG_OFST            (0x14)              /**< (PDEC_PRESC) Prescaler Value Offset */
#define PDEC_FILTER_REG_OFST           (0x15)              /**< (PDEC_FILTER) Filter Value Offset */
#define PDEC_PRESCBUF_REG_OFST         (0x18)              /**< (PDEC_PRESCBUF) Prescaler Buffer Value Offset */
#define PDEC_FILTERBUF_REG_OFST        (0x19)              /**< (PDEC_FILTERBUF) Filter Buffer Value Offset */
#define PDEC_COUNT_REG_OFST            (0x1C)              /**< (PDEC_COUNT) Counter Value Offset */
#define PDEC_CC_REG_OFST               (0x20)              /**< (PDEC_CC) Channel n Compare Value Offset */
#define PDEC_CCBUF_REG_OFST            (0x30)              /**< (PDEC_CCBUF) Channel Compare Buffer Value Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief PDEC register API structure */
typedef struct
{  /* Quadrature Decodeur */
  __IO  uint32_t                       PDEC_CTRLA;         /**< Offset: 0x00 (R/W  32) Control A */
  __IO  uint8_t                        PDEC_CTRLBCLR;      /**< Offset: 0x04 (R/W  8) Control B Clear */
  __IO  uint8_t                        PDEC_CTRLBSET;      /**< Offset: 0x05 (R/W  8) Control B Set */
  __IO  uint16_t                       PDEC_EVCTRL;        /**< Offset: 0x06 (R/W  16) Event Control */
  __IO  uint8_t                        PDEC_INTENCLR;      /**< Offset: 0x08 (R/W  8) Interrupt Enable Clear */
  __IO  uint8_t                        PDEC_INTENSET;      /**< Offset: 0x09 (R/W  8) Interrupt Enable Set */
  __IO  uint8_t                        PDEC_INTFLAG;       /**< Offset: 0x0A (R/W  8) Interrupt Flag Status and Clear */
  __I   uint8_t                        Reserved1[0x01];
  __IO  uint16_t                       PDEC_STATUS;        /**< Offset: 0x0C (R/W  16) Status */
  __I   uint8_t                        Reserved2[0x01];
  __IO  uint8_t                        PDEC_DBGCTRL;       /**< Offset: 0x0F (R/W  8) Debug Control */
  __I   uint32_t                       PDEC_SYNCBUSY;      /**< Offset: 0x10 (R/   32) Synchronization Status */
  __IO  uint8_t                        PDEC_PRESC;         /**< Offset: 0x14 (R/W  8) Prescaler Value */
  __IO  uint8_t                        PDEC_FILTER;        /**< Offset: 0x15 (R/W  8) Filter Value */
  __I   uint8_t                        Reserved3[0x02];
  __IO  uint8_t                        PDEC_PRESCBUF;      /**< Offset: 0x18 (R/W  8) Prescaler Buffer Value */
  __IO  uint8_t                        PDEC_FILTERBUF;     /**< Offset: 0x19 (R/W  8) Filter Buffer Value */
  __I   uint8_t                        Reserved4[0x02];
  __IO  uint32_t                       PDEC_COUNT;         /**< Offset: 0x1C (R/W  32) Counter Value */
  __IO  uint32_t                       PDEC_CC[2];         /**< Offset: 0x20 (R/W  32) Channel n Compare Value */
  __I   uint8_t                        Reserved5[0x08];
  __IO  uint32_t                       PDEC_CCBUF[2];      /**< Offset: 0x30 (R/W  32) Channel Compare Buffer Value */
} pdec_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_PDEC_COMPONENT_H_ */
