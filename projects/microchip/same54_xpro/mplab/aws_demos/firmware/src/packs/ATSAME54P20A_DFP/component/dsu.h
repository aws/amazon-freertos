/**
 * \brief Component description for DSU
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
#ifndef _SAME54_DSU_COMPONENT_H_
#define _SAME54_DSU_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR DSU                                          */
/* ************************************************************************** */

/* -------- DSU_CTRL : (DSU Offset: 0x00) ( /W 8) Control -------- */
#define DSU_CTRL_RESETVALUE                   _U_(0x00)                                            /**<  (DSU_CTRL) Control  Reset Value */

#define DSU_CTRL_SWRST_Pos                    _U_(0)                                               /**< (DSU_CTRL) Software Reset Position */
#define DSU_CTRL_SWRST_Msk                    (_U_(0x1) << DSU_CTRL_SWRST_Pos)                     /**< (DSU_CTRL) Software Reset Mask */
#define DSU_CTRL_SWRST(value)                 (DSU_CTRL_SWRST_Msk & ((value) << DSU_CTRL_SWRST_Pos))
#define DSU_CTRL_CRC_Pos                      _U_(2)                                               /**< (DSU_CTRL) 32-bit Cyclic Redundancy Code Position */
#define DSU_CTRL_CRC_Msk                      (_U_(0x1) << DSU_CTRL_CRC_Pos)                       /**< (DSU_CTRL) 32-bit Cyclic Redundancy Code Mask */
#define DSU_CTRL_CRC(value)                   (DSU_CTRL_CRC_Msk & ((value) << DSU_CTRL_CRC_Pos))  
#define DSU_CTRL_MBIST_Pos                    _U_(3)                                               /**< (DSU_CTRL) Memory built-in self-test Position */
#define DSU_CTRL_MBIST_Msk                    (_U_(0x1) << DSU_CTRL_MBIST_Pos)                     /**< (DSU_CTRL) Memory built-in self-test Mask */
#define DSU_CTRL_MBIST(value)                 (DSU_CTRL_MBIST_Msk & ((value) << DSU_CTRL_MBIST_Pos))
#define DSU_CTRL_CE_Pos                       _U_(4)                                               /**< (DSU_CTRL) Chip-Erase Position */
#define DSU_CTRL_CE_Msk                       (_U_(0x1) << DSU_CTRL_CE_Pos)                        /**< (DSU_CTRL) Chip-Erase Mask */
#define DSU_CTRL_CE(value)                    (DSU_CTRL_CE_Msk & ((value) << DSU_CTRL_CE_Pos))    
#define DSU_CTRL_ARR_Pos                      _U_(6)                                               /**< (DSU_CTRL) Auxiliary Row Read Position */
#define DSU_CTRL_ARR_Msk                      (_U_(0x1) << DSU_CTRL_ARR_Pos)                       /**< (DSU_CTRL) Auxiliary Row Read Mask */
#define DSU_CTRL_ARR(value)                   (DSU_CTRL_ARR_Msk & ((value) << DSU_CTRL_ARR_Pos))  
#define DSU_CTRL_SMSA_Pos                     _U_(7)                                               /**< (DSU_CTRL) Start Memory Stream Access Position */
#define DSU_CTRL_SMSA_Msk                     (_U_(0x1) << DSU_CTRL_SMSA_Pos)                      /**< (DSU_CTRL) Start Memory Stream Access Mask */
#define DSU_CTRL_SMSA(value)                  (DSU_CTRL_SMSA_Msk & ((value) << DSU_CTRL_SMSA_Pos))
#define DSU_CTRL_Msk                          _U_(0xDD)                                            /**< (DSU_CTRL) Register Mask  */


/* -------- DSU_STATUSA : (DSU Offset: 0x01) (R/W 8) Status A -------- */
#define DSU_STATUSA_RESETVALUE                _U_(0x00)                                            /**<  (DSU_STATUSA) Status A  Reset Value */

#define DSU_STATUSA_DONE_Pos                  _U_(0)                                               /**< (DSU_STATUSA) Done Position */
#define DSU_STATUSA_DONE_Msk                  (_U_(0x1) << DSU_STATUSA_DONE_Pos)                   /**< (DSU_STATUSA) Done Mask */
#define DSU_STATUSA_DONE(value)               (DSU_STATUSA_DONE_Msk & ((value) << DSU_STATUSA_DONE_Pos))
#define DSU_STATUSA_CRSTEXT_Pos               _U_(1)                                               /**< (DSU_STATUSA) CPU Reset Phase Extension Position */
#define DSU_STATUSA_CRSTEXT_Msk               (_U_(0x1) << DSU_STATUSA_CRSTEXT_Pos)                /**< (DSU_STATUSA) CPU Reset Phase Extension Mask */
#define DSU_STATUSA_CRSTEXT(value)            (DSU_STATUSA_CRSTEXT_Msk & ((value) << DSU_STATUSA_CRSTEXT_Pos))
#define DSU_STATUSA_BERR_Pos                  _U_(2)                                               /**< (DSU_STATUSA) Bus Error Position */
#define DSU_STATUSA_BERR_Msk                  (_U_(0x1) << DSU_STATUSA_BERR_Pos)                   /**< (DSU_STATUSA) Bus Error Mask */
#define DSU_STATUSA_BERR(value)               (DSU_STATUSA_BERR_Msk & ((value) << DSU_STATUSA_BERR_Pos))
#define DSU_STATUSA_FAIL_Pos                  _U_(3)                                               /**< (DSU_STATUSA) Failure Position */
#define DSU_STATUSA_FAIL_Msk                  (_U_(0x1) << DSU_STATUSA_FAIL_Pos)                   /**< (DSU_STATUSA) Failure Mask */
#define DSU_STATUSA_FAIL(value)               (DSU_STATUSA_FAIL_Msk & ((value) << DSU_STATUSA_FAIL_Pos))
#define DSU_STATUSA_PERR_Pos                  _U_(4)                                               /**< (DSU_STATUSA) Protection Error Position */
#define DSU_STATUSA_PERR_Msk                  (_U_(0x1) << DSU_STATUSA_PERR_Pos)                   /**< (DSU_STATUSA) Protection Error Mask */
#define DSU_STATUSA_PERR(value)               (DSU_STATUSA_PERR_Msk & ((value) << DSU_STATUSA_PERR_Pos))
#define DSU_STATUSA_Msk                       _U_(0x1F)                                            /**< (DSU_STATUSA) Register Mask  */


/* -------- DSU_STATUSB : (DSU Offset: 0x02) ( R/ 8) Status B -------- */
#define DSU_STATUSB_RESETVALUE                _U_(0x00)                                            /**<  (DSU_STATUSB) Status B  Reset Value */

#define DSU_STATUSB_PROT_Pos                  _U_(0)                                               /**< (DSU_STATUSB) Protected Position */
#define DSU_STATUSB_PROT_Msk                  (_U_(0x1) << DSU_STATUSB_PROT_Pos)                   /**< (DSU_STATUSB) Protected Mask */
#define DSU_STATUSB_PROT(value)               (DSU_STATUSB_PROT_Msk & ((value) << DSU_STATUSB_PROT_Pos))
#define DSU_STATUSB_DBGPRES_Pos               _U_(1)                                               /**< (DSU_STATUSB) Debugger Present Position */
#define DSU_STATUSB_DBGPRES_Msk               (_U_(0x1) << DSU_STATUSB_DBGPRES_Pos)                /**< (DSU_STATUSB) Debugger Present Mask */
#define DSU_STATUSB_DBGPRES(value)            (DSU_STATUSB_DBGPRES_Msk & ((value) << DSU_STATUSB_DBGPRES_Pos))
#define DSU_STATUSB_DCCD0_Pos                 _U_(2)                                               /**< (DSU_STATUSB) Debug Communication Channel 0 Dirty Position */
#define DSU_STATUSB_DCCD0_Msk                 (_U_(0x1) << DSU_STATUSB_DCCD0_Pos)                  /**< (DSU_STATUSB) Debug Communication Channel 0 Dirty Mask */
#define DSU_STATUSB_DCCD0(value)              (DSU_STATUSB_DCCD0_Msk & ((value) << DSU_STATUSB_DCCD0_Pos))
#define DSU_STATUSB_DCCD1_Pos                 _U_(3)                                               /**< (DSU_STATUSB) Debug Communication Channel 1 Dirty Position */
#define DSU_STATUSB_DCCD1_Msk                 (_U_(0x1) << DSU_STATUSB_DCCD1_Pos)                  /**< (DSU_STATUSB) Debug Communication Channel 1 Dirty Mask */
#define DSU_STATUSB_DCCD1(value)              (DSU_STATUSB_DCCD1_Msk & ((value) << DSU_STATUSB_DCCD1_Pos))
#define DSU_STATUSB_HPE_Pos                   _U_(4)                                               /**< (DSU_STATUSB) Hot-Plugging Enable Position */
#define DSU_STATUSB_HPE_Msk                   (_U_(0x1) << DSU_STATUSB_HPE_Pos)                    /**< (DSU_STATUSB) Hot-Plugging Enable Mask */
#define DSU_STATUSB_HPE(value)                (DSU_STATUSB_HPE_Msk & ((value) << DSU_STATUSB_HPE_Pos))
#define DSU_STATUSB_CELCK_Pos                 _U_(5)                                               /**< (DSU_STATUSB) Chip Erase Locked Position */
#define DSU_STATUSB_CELCK_Msk                 (_U_(0x1) << DSU_STATUSB_CELCK_Pos)                  /**< (DSU_STATUSB) Chip Erase Locked Mask */
#define DSU_STATUSB_CELCK(value)              (DSU_STATUSB_CELCK_Msk & ((value) << DSU_STATUSB_CELCK_Pos))
#define DSU_STATUSB_TDCCD0_Pos                _U_(6)                                               /**< (DSU_STATUSB) Test Debug Communication Channel 0 Dirty Position */
#define DSU_STATUSB_TDCCD0_Msk                (_U_(0x1) << DSU_STATUSB_TDCCD0_Pos)                 /**< (DSU_STATUSB) Test Debug Communication Channel 0 Dirty Mask */
#define DSU_STATUSB_TDCCD0(value)             (DSU_STATUSB_TDCCD0_Msk & ((value) << DSU_STATUSB_TDCCD0_Pos))
#define DSU_STATUSB_TDCCD1_Pos                _U_(7)                                               /**< (DSU_STATUSB) Test Debug Communication Channel 1 Dirty Position */
#define DSU_STATUSB_TDCCD1_Msk                (_U_(0x1) << DSU_STATUSB_TDCCD1_Pos)                 /**< (DSU_STATUSB) Test Debug Communication Channel 1 Dirty Mask */
#define DSU_STATUSB_TDCCD1(value)             (DSU_STATUSB_TDCCD1_Msk & ((value) << DSU_STATUSB_TDCCD1_Pos))
#define DSU_STATUSB_Msk                       _U_(0xFF)                                            /**< (DSU_STATUSB) Register Mask  */

#define DSU_STATUSB_DCCD_Pos                  _U_(2)                                               /**< (DSU_STATUSB Position) Debug Communication Channel x Dirty */
#define DSU_STATUSB_DCCD_Msk                  (_U_(0x3) << DSU_STATUSB_DCCD_Pos)                   /**< (DSU_STATUSB Mask) DCCD */
#define DSU_STATUSB_DCCD(value)               (DSU_STATUSB_DCCD_Msk & ((value) << DSU_STATUSB_DCCD_Pos)) 
#define DSU_STATUSB_TDCCD_Pos                 _U_(6)                                               /**< (DSU_STATUSB Position) Test Debug Communication Channel x Dirty */
#define DSU_STATUSB_TDCCD_Msk                 (_U_(0x3) << DSU_STATUSB_TDCCD_Pos)                  /**< (DSU_STATUSB Mask) TDCCD */
#define DSU_STATUSB_TDCCD(value)              (DSU_STATUSB_TDCCD_Msk & ((value) << DSU_STATUSB_TDCCD_Pos)) 

/* -------- DSU_ADDR : (DSU Offset: 0x04) (R/W 32) Address -------- */
#define DSU_ADDR_RESETVALUE                   _U_(0x00)                                            /**<  (DSU_ADDR) Address  Reset Value */

#define DSU_ADDR_AMOD_Pos                     _U_(0)                                               /**< (DSU_ADDR) Access Mode Position */
#define DSU_ADDR_AMOD_Msk                     (_U_(0x3) << DSU_ADDR_AMOD_Pos)                      /**< (DSU_ADDR) Access Mode Mask */
#define DSU_ADDR_AMOD(value)                  (DSU_ADDR_AMOD_Msk & ((value) << DSU_ADDR_AMOD_Pos))
#define DSU_ADDR_ADDR_Pos                     _U_(2)                                               /**< (DSU_ADDR) Address Position */
#define DSU_ADDR_ADDR_Msk                     (_U_(0x3FFFFFFF) << DSU_ADDR_ADDR_Pos)               /**< (DSU_ADDR) Address Mask */
#define DSU_ADDR_ADDR(value)                  (DSU_ADDR_ADDR_Msk & ((value) << DSU_ADDR_ADDR_Pos))
#define DSU_ADDR_Msk                          _U_(0xFFFFFFFF)                                      /**< (DSU_ADDR) Register Mask  */


/* -------- DSU_LENGTH : (DSU Offset: 0x08) (R/W 32) Length -------- */
#define DSU_LENGTH_RESETVALUE                 _U_(0x00)                                            /**<  (DSU_LENGTH) Length  Reset Value */

#define DSU_LENGTH_LENGTH_Pos                 _U_(2)                                               /**< (DSU_LENGTH) Length Position */
#define DSU_LENGTH_LENGTH_Msk                 (_U_(0x3FFFFFFF) << DSU_LENGTH_LENGTH_Pos)           /**< (DSU_LENGTH) Length Mask */
#define DSU_LENGTH_LENGTH(value)              (DSU_LENGTH_LENGTH_Msk & ((value) << DSU_LENGTH_LENGTH_Pos))
#define DSU_LENGTH_Msk                        _U_(0xFFFFFFFC)                                      /**< (DSU_LENGTH) Register Mask  */


/* -------- DSU_DATA : (DSU Offset: 0x0C) (R/W 32) Data -------- */
#define DSU_DATA_RESETVALUE                   _U_(0x00)                                            /**<  (DSU_DATA) Data  Reset Value */

#define DSU_DATA_DATA_Pos                     _U_(0)                                               /**< (DSU_DATA) Data Position */
#define DSU_DATA_DATA_Msk                     (_U_(0xFFFFFFFF) << DSU_DATA_DATA_Pos)               /**< (DSU_DATA) Data Mask */
#define DSU_DATA_DATA(value)                  (DSU_DATA_DATA_Msk & ((value) << DSU_DATA_DATA_Pos))
#define DSU_DATA_Msk                          _U_(0xFFFFFFFF)                                      /**< (DSU_DATA) Register Mask  */


/* -------- DSU_DCC : (DSU Offset: 0x10) (R/W 32) Debug Communication Channel n -------- */
#define DSU_DCC_RESETVALUE                    _U_(0x00)                                            /**<  (DSU_DCC) Debug Communication Channel n  Reset Value */

#define DSU_DCC_DATA_Pos                      _U_(0)                                               /**< (DSU_DCC) Data Position */
#define DSU_DCC_DATA_Msk                      (_U_(0xFFFFFFFF) << DSU_DCC_DATA_Pos)                /**< (DSU_DCC) Data Mask */
#define DSU_DCC_DATA(value)                   (DSU_DCC_DATA_Msk & ((value) << DSU_DCC_DATA_Pos))  
#define DSU_DCC_Msk                           _U_(0xFFFFFFFF)                                      /**< (DSU_DCC) Register Mask  */


/* -------- DSU_DID : (DSU Offset: 0x18) ( R/ 32) Device Identification -------- */
#define DSU_DID_RESETVALUE                    _U_(0x61840200)                                      /**<  (DSU_DID) Device Identification  Reset Value */

#define DSU_DID_DEVSEL_Pos                    _U_(0)                                               /**< (DSU_DID) Device Select Position */
#define DSU_DID_DEVSEL_Msk                    (_U_(0xFF) << DSU_DID_DEVSEL_Pos)                    /**< (DSU_DID) Device Select Mask */
#define DSU_DID_DEVSEL(value)                 (DSU_DID_DEVSEL_Msk & ((value) << DSU_DID_DEVSEL_Pos))
#define DSU_DID_REVISION_Pos                  _U_(8)                                               /**< (DSU_DID) Revision Number Position */
#define DSU_DID_REVISION_Msk                  (_U_(0xF) << DSU_DID_REVISION_Pos)                   /**< (DSU_DID) Revision Number Mask */
#define DSU_DID_REVISION(value)               (DSU_DID_REVISION_Msk & ((value) << DSU_DID_REVISION_Pos))
#define DSU_DID_DIE_Pos                       _U_(12)                                              /**< (DSU_DID) Die Number Position */
#define DSU_DID_DIE_Msk                       (_U_(0xF) << DSU_DID_DIE_Pos)                        /**< (DSU_DID) Die Number Mask */
#define DSU_DID_DIE(value)                    (DSU_DID_DIE_Msk & ((value) << DSU_DID_DIE_Pos))    
#define DSU_DID_SERIES_Pos                    _U_(16)                                              /**< (DSU_DID) Series Position */
#define DSU_DID_SERIES_Msk                    (_U_(0x3F) << DSU_DID_SERIES_Pos)                    /**< (DSU_DID) Series Mask */
#define DSU_DID_SERIES(value)                 (DSU_DID_SERIES_Msk & ((value) << DSU_DID_SERIES_Pos))
#define   DSU_DID_SERIES_0_Val                _U_(0x0)                                             /**< (DSU_DID) Cortex-M0+ processor, basic feature set  */
#define   DSU_DID_SERIES_1_Val                _U_(0x1)                                             /**< (DSU_DID) Cortex-M0+ processor, USB  */
#define DSU_DID_SERIES_0                      (DSU_DID_SERIES_0_Val << DSU_DID_SERIES_Pos)         /**< (DSU_DID) Cortex-M0+ processor, basic feature set Position  */
#define DSU_DID_SERIES_1                      (DSU_DID_SERIES_1_Val << DSU_DID_SERIES_Pos)         /**< (DSU_DID) Cortex-M0+ processor, USB Position  */
#define DSU_DID_FAMILY_Pos                    _U_(23)                                              /**< (DSU_DID) Family Position */
#define DSU_DID_FAMILY_Msk                    (_U_(0x1F) << DSU_DID_FAMILY_Pos)                    /**< (DSU_DID) Family Mask */
#define DSU_DID_FAMILY(value)                 (DSU_DID_FAMILY_Msk & ((value) << DSU_DID_FAMILY_Pos))
#define   DSU_DID_FAMILY_0_Val                _U_(0x0)                                             /**< (DSU_DID) General purpose microcontroller  */
#define   DSU_DID_FAMILY_1_Val                _U_(0x1)                                             /**< (DSU_DID) PicoPower  */
#define DSU_DID_FAMILY_0                      (DSU_DID_FAMILY_0_Val << DSU_DID_FAMILY_Pos)         /**< (DSU_DID) General purpose microcontroller Position  */
#define DSU_DID_FAMILY_1                      (DSU_DID_FAMILY_1_Val << DSU_DID_FAMILY_Pos)         /**< (DSU_DID) PicoPower Position  */
#define DSU_DID_PROCESSOR_Pos                 _U_(28)                                              /**< (DSU_DID) Processor Position */
#define DSU_DID_PROCESSOR_Msk                 (_U_(0xF) << DSU_DID_PROCESSOR_Pos)                  /**< (DSU_DID) Processor Mask */
#define DSU_DID_PROCESSOR(value)              (DSU_DID_PROCESSOR_Msk & ((value) << DSU_DID_PROCESSOR_Pos))
#define   DSU_DID_PROCESSOR_CM0P_Val          _U_(0x1)                                             /**< (DSU_DID) Cortex-M0+  */
#define   DSU_DID_PROCESSOR_CM23_Val          _U_(0x2)                                             /**< (DSU_DID) Cortex-M23  */
#define   DSU_DID_PROCESSOR_CM3_Val           _U_(0x3)                                             /**< (DSU_DID) Cortex-M3  */
#define   DSU_DID_PROCESSOR_CM4_Val           _U_(0x5)                                             /**< (DSU_DID) Cortex-M4  */
#define   DSU_DID_PROCESSOR_CM4F_Val          _U_(0x6)                                             /**< (DSU_DID) Cortex-M4 with FPU  */
#define   DSU_DID_PROCESSOR_CM33_Val          _U_(0x7)                                             /**< (DSU_DID) Cortex-M33  */
#define DSU_DID_PROCESSOR_CM0P                (DSU_DID_PROCESSOR_CM0P_Val << DSU_DID_PROCESSOR_Pos) /**< (DSU_DID) Cortex-M0+ Position  */
#define DSU_DID_PROCESSOR_CM23                (DSU_DID_PROCESSOR_CM23_Val << DSU_DID_PROCESSOR_Pos) /**< (DSU_DID) Cortex-M23 Position  */
#define DSU_DID_PROCESSOR_CM3                 (DSU_DID_PROCESSOR_CM3_Val << DSU_DID_PROCESSOR_Pos) /**< (DSU_DID) Cortex-M3 Position  */
#define DSU_DID_PROCESSOR_CM4                 (DSU_DID_PROCESSOR_CM4_Val << DSU_DID_PROCESSOR_Pos) /**< (DSU_DID) Cortex-M4 Position  */
#define DSU_DID_PROCESSOR_CM4F                (DSU_DID_PROCESSOR_CM4F_Val << DSU_DID_PROCESSOR_Pos) /**< (DSU_DID) Cortex-M4 with FPU Position  */
#define DSU_DID_PROCESSOR_CM33                (DSU_DID_PROCESSOR_CM33_Val << DSU_DID_PROCESSOR_Pos) /**< (DSU_DID) Cortex-M33 Position  */
#define DSU_DID_Msk                           _U_(0xFFBFFFFF)                                      /**< (DSU_DID) Register Mask  */


/* -------- DSU_CFG : (DSU Offset: 0x1C) (R/W 32) Configuration -------- */
#define DSU_CFG_RESETVALUE                    _U_(0x02)                                            /**<  (DSU_CFG) Configuration  Reset Value */

#define DSU_CFG_LQOS_Pos                      _U_(0)                                               /**< (DSU_CFG) Latency Quality Of Service Position */
#define DSU_CFG_LQOS_Msk                      (_U_(0x3) << DSU_CFG_LQOS_Pos)                       /**< (DSU_CFG) Latency Quality Of Service Mask */
#define DSU_CFG_LQOS(value)                   (DSU_CFG_LQOS_Msk & ((value) << DSU_CFG_LQOS_Pos))  
#define DSU_CFG_DCCDMALEVEL_Pos               _U_(2)                                               /**< (DSU_CFG) DMA Trigger Level Position */
#define DSU_CFG_DCCDMALEVEL_Msk               (_U_(0x3) << DSU_CFG_DCCDMALEVEL_Pos)                /**< (DSU_CFG) DMA Trigger Level Mask */
#define DSU_CFG_DCCDMALEVEL(value)            (DSU_CFG_DCCDMALEVEL_Msk & ((value) << DSU_CFG_DCCDMALEVEL_Pos))
#define   DSU_CFG_DCCDMALEVEL_EMPTY_Val       _U_(0x0)                                             /**< (DSU_CFG) Trigger rises when DCC is empty  */
#define   DSU_CFG_DCCDMALEVEL_FULL_Val        _U_(0x1)                                             /**< (DSU_CFG) Trigger rises when DCC is full  */
#define DSU_CFG_DCCDMALEVEL_EMPTY             (DSU_CFG_DCCDMALEVEL_EMPTY_Val << DSU_CFG_DCCDMALEVEL_Pos) /**< (DSU_CFG) Trigger rises when DCC is empty Position  */
#define DSU_CFG_DCCDMALEVEL_FULL              (DSU_CFG_DCCDMALEVEL_FULL_Val << DSU_CFG_DCCDMALEVEL_Pos) /**< (DSU_CFG) Trigger rises when DCC is full Position  */
#define DSU_CFG_ETBRAMEN_Pos                  _U_(4)                                               /**< (DSU_CFG) Trace Control Position */
#define DSU_CFG_ETBRAMEN_Msk                  (_U_(0x1) << DSU_CFG_ETBRAMEN_Pos)                   /**< (DSU_CFG) Trace Control Mask */
#define DSU_CFG_ETBRAMEN(value)               (DSU_CFG_ETBRAMEN_Msk & ((value) << DSU_CFG_ETBRAMEN_Pos))
#define DSU_CFG_Msk                           _U_(0x0000001F)                                      /**< (DSU_CFG) Register Mask  */


/* -------- DSU_DCFG : (DSU Offset: 0xF0) (R/W 32) Device Configuration -------- */
#define DSU_DCFG_RESETVALUE                   _U_(0x00)                                            /**<  (DSU_DCFG) Device Configuration  Reset Value */

#define DSU_DCFG_DCFG_Pos                     _U_(0)                                               /**< (DSU_DCFG) Device Configuration Position */
#define DSU_DCFG_DCFG_Msk                     (_U_(0xFFFFFFFF) << DSU_DCFG_DCFG_Pos)               /**< (DSU_DCFG) Device Configuration Mask */
#define DSU_DCFG_DCFG(value)                  (DSU_DCFG_DCFG_Msk & ((value) << DSU_DCFG_DCFG_Pos))
#define DSU_DCFG_Msk                          _U_(0xFFFFFFFF)                                      /**< (DSU_DCFG) Register Mask  */


/* -------- DSU_ENTRY0 : (DSU Offset: 0x1000) ( R/ 32) CoreSight ROM Table Entry 0 -------- */
#define DSU_ENTRY0_RESETVALUE                 _U_(0x9F0FC002)                                      /**<  (DSU_ENTRY0) CoreSight ROM Table Entry 0  Reset Value */

#define DSU_ENTRY0_EPRES_Pos                  _U_(0)                                               /**< (DSU_ENTRY0) Entry Present Position */
#define DSU_ENTRY0_EPRES_Msk                  (_U_(0x1) << DSU_ENTRY0_EPRES_Pos)                   /**< (DSU_ENTRY0) Entry Present Mask */
#define DSU_ENTRY0_EPRES(value)               (DSU_ENTRY0_EPRES_Msk & ((value) << DSU_ENTRY0_EPRES_Pos))
#define DSU_ENTRY0_FMT_Pos                    _U_(1)                                               /**< (DSU_ENTRY0) Format Position */
#define DSU_ENTRY0_FMT_Msk                    (_U_(0x1) << DSU_ENTRY0_FMT_Pos)                     /**< (DSU_ENTRY0) Format Mask */
#define DSU_ENTRY0_FMT(value)                 (DSU_ENTRY0_FMT_Msk & ((value) << DSU_ENTRY0_FMT_Pos))
#define DSU_ENTRY0_ADDOFF_Pos                 _U_(12)                                              /**< (DSU_ENTRY0) Address Offset Position */
#define DSU_ENTRY0_ADDOFF_Msk                 (_U_(0xFFFFF) << DSU_ENTRY0_ADDOFF_Pos)              /**< (DSU_ENTRY0) Address Offset Mask */
#define DSU_ENTRY0_ADDOFF(value)              (DSU_ENTRY0_ADDOFF_Msk & ((value) << DSU_ENTRY0_ADDOFF_Pos))
#define DSU_ENTRY0_Msk                        _U_(0xFFFFF003)                                      /**< (DSU_ENTRY0) Register Mask  */


/* -------- DSU_ENTRY1 : (DSU Offset: 0x1004) ( R/ 32) CoreSight ROM Table Entry 1 -------- */
#define DSU_ENTRY1_RESETVALUE                 _U_(0x00)                                            /**<  (DSU_ENTRY1) CoreSight ROM Table Entry 1  Reset Value */

#define DSU_ENTRY1_Msk                        _U_(0x00000000)                                      /**< (DSU_ENTRY1) Register Mask  */


/* -------- DSU_END : (DSU Offset: 0x1008) ( R/ 32) CoreSight ROM Table End -------- */
#define DSU_END_RESETVALUE                    _U_(0x00)                                            /**<  (DSU_END) CoreSight ROM Table End  Reset Value */

#define DSU_END_END_Pos                       _U_(0)                                               /**< (DSU_END) End Marker Position */
#define DSU_END_END_Msk                       (_U_(0xFFFFFFFF) << DSU_END_END_Pos)                 /**< (DSU_END) End Marker Mask */
#define DSU_END_END(value)                    (DSU_END_END_Msk & ((value) << DSU_END_END_Pos))    
#define DSU_END_Msk                           _U_(0xFFFFFFFF)                                      /**< (DSU_END) Register Mask  */


/* -------- DSU_MEMTYPE : (DSU Offset: 0x1FCC) ( R/ 32) CoreSight ROM Table Memory Type -------- */
#define DSU_MEMTYPE_RESETVALUE                _U_(0x00)                                            /**<  (DSU_MEMTYPE) CoreSight ROM Table Memory Type  Reset Value */

#define DSU_MEMTYPE_SMEMP_Pos                 _U_(0)                                               /**< (DSU_MEMTYPE) System Memory Present Position */
#define DSU_MEMTYPE_SMEMP_Msk                 (_U_(0x1) << DSU_MEMTYPE_SMEMP_Pos)                  /**< (DSU_MEMTYPE) System Memory Present Mask */
#define DSU_MEMTYPE_SMEMP(value)              (DSU_MEMTYPE_SMEMP_Msk & ((value) << DSU_MEMTYPE_SMEMP_Pos))
#define DSU_MEMTYPE_Msk                       _U_(0x00000001)                                      /**< (DSU_MEMTYPE) Register Mask  */


/* -------- DSU_PID4 : (DSU Offset: 0x1FD0) ( R/ 32) Peripheral Identification 4 -------- */
#define DSU_PID4_RESETVALUE                   _U_(0x00)                                            /**<  (DSU_PID4) Peripheral Identification 4  Reset Value */

#define DSU_PID4_JEPCC_Pos                    _U_(0)                                               /**< (DSU_PID4) JEP-106 Continuation Code Position */
#define DSU_PID4_JEPCC_Msk                    (_U_(0xF) << DSU_PID4_JEPCC_Pos)                     /**< (DSU_PID4) JEP-106 Continuation Code Mask */
#define DSU_PID4_JEPCC(value)                 (DSU_PID4_JEPCC_Msk & ((value) << DSU_PID4_JEPCC_Pos))
#define DSU_PID4_FKBC_Pos                     _U_(4)                                               /**< (DSU_PID4) 4KB count Position */
#define DSU_PID4_FKBC_Msk                     (_U_(0xF) << DSU_PID4_FKBC_Pos)                      /**< (DSU_PID4) 4KB count Mask */
#define DSU_PID4_FKBC(value)                  (DSU_PID4_FKBC_Msk & ((value) << DSU_PID4_FKBC_Pos))
#define DSU_PID4_Msk                          _U_(0x000000FF)                                      /**< (DSU_PID4) Register Mask  */


/* -------- DSU_PID5 : (DSU Offset: 0x1FD4) ( R/ 32) Peripheral Identification 5 -------- */
#define DSU_PID5_RESETVALUE                   _U_(0x00)                                            /**<  (DSU_PID5) Peripheral Identification 5  Reset Value */

#define DSU_PID5_Msk                          _U_(0x00000000)                                      /**< (DSU_PID5) Register Mask  */


/* -------- DSU_PID6 : (DSU Offset: 0x1FD8) ( R/ 32) Peripheral Identification 6 -------- */
#define DSU_PID6_RESETVALUE                   _U_(0x00)                                            /**<  (DSU_PID6) Peripheral Identification 6  Reset Value */

#define DSU_PID6_Msk                          _U_(0x00000000)                                      /**< (DSU_PID6) Register Mask  */


/* -------- DSU_PID7 : (DSU Offset: 0x1FDC) ( R/ 32) Peripheral Identification 7 -------- */
#define DSU_PID7_RESETVALUE                   _U_(0x00)                                            /**<  (DSU_PID7) Peripheral Identification 7  Reset Value */

#define DSU_PID7_Msk                          _U_(0x00000000)                                      /**< (DSU_PID7) Register Mask  */


/* -------- DSU_PID0 : (DSU Offset: 0x1FE0) ( R/ 32) Peripheral Identification 0 -------- */
#define DSU_PID0_RESETVALUE                   _U_(0xD0)                                            /**<  (DSU_PID0) Peripheral Identification 0  Reset Value */

#define DSU_PID0_PARTNBL_Pos                  _U_(0)                                               /**< (DSU_PID0) Part Number Low Position */
#define DSU_PID0_PARTNBL_Msk                  (_U_(0xFF) << DSU_PID0_PARTNBL_Pos)                  /**< (DSU_PID0) Part Number Low Mask */
#define DSU_PID0_PARTNBL(value)               (DSU_PID0_PARTNBL_Msk & ((value) << DSU_PID0_PARTNBL_Pos))
#define DSU_PID0_Msk                          _U_(0x000000FF)                                      /**< (DSU_PID0) Register Mask  */


/* -------- DSU_PID1 : (DSU Offset: 0x1FE4) ( R/ 32) Peripheral Identification 1 -------- */
#define DSU_PID1_RESETVALUE                   _U_(0xFC)                                            /**<  (DSU_PID1) Peripheral Identification 1  Reset Value */

#define DSU_PID1_PARTNBH_Pos                  _U_(0)                                               /**< (DSU_PID1) Part Number High Position */
#define DSU_PID1_PARTNBH_Msk                  (_U_(0xF) << DSU_PID1_PARTNBH_Pos)                   /**< (DSU_PID1) Part Number High Mask */
#define DSU_PID1_PARTNBH(value)               (DSU_PID1_PARTNBH_Msk & ((value) << DSU_PID1_PARTNBH_Pos))
#define DSU_PID1_JEPIDCL_Pos                  _U_(4)                                               /**< (DSU_PID1) Low part of the JEP-106 Identity Code Position */
#define DSU_PID1_JEPIDCL_Msk                  (_U_(0xF) << DSU_PID1_JEPIDCL_Pos)                   /**< (DSU_PID1) Low part of the JEP-106 Identity Code Mask */
#define DSU_PID1_JEPIDCL(value)               (DSU_PID1_JEPIDCL_Msk & ((value) << DSU_PID1_JEPIDCL_Pos))
#define DSU_PID1_Msk                          _U_(0x000000FF)                                      /**< (DSU_PID1) Register Mask  */


/* -------- DSU_PID2 : (DSU Offset: 0x1FE8) ( R/ 32) Peripheral Identification 2 -------- */
#define DSU_PID2_RESETVALUE                   _U_(0x09)                                            /**<  (DSU_PID2) Peripheral Identification 2  Reset Value */

#define DSU_PID2_JEPIDCH_Pos                  _U_(0)                                               /**< (DSU_PID2) JEP-106 Identity Code High Position */
#define DSU_PID2_JEPIDCH_Msk                  (_U_(0x7) << DSU_PID2_JEPIDCH_Pos)                   /**< (DSU_PID2) JEP-106 Identity Code High Mask */
#define DSU_PID2_JEPIDCH(value)               (DSU_PID2_JEPIDCH_Msk & ((value) << DSU_PID2_JEPIDCH_Pos))
#define DSU_PID2_JEPU_Pos                     _U_(3)                                               /**< (DSU_PID2) JEP-106 Identity Code is used Position */
#define DSU_PID2_JEPU_Msk                     (_U_(0x1) << DSU_PID2_JEPU_Pos)                      /**< (DSU_PID2) JEP-106 Identity Code is used Mask */
#define DSU_PID2_JEPU(value)                  (DSU_PID2_JEPU_Msk & ((value) << DSU_PID2_JEPU_Pos))
#define DSU_PID2_REVISION_Pos                 _U_(4)                                               /**< (DSU_PID2) Revision Number Position */
#define DSU_PID2_REVISION_Msk                 (_U_(0xF) << DSU_PID2_REVISION_Pos)                  /**< (DSU_PID2) Revision Number Mask */
#define DSU_PID2_REVISION(value)              (DSU_PID2_REVISION_Msk & ((value) << DSU_PID2_REVISION_Pos))
#define DSU_PID2_Msk                          _U_(0x000000FF)                                      /**< (DSU_PID2) Register Mask  */


/* -------- DSU_PID3 : (DSU Offset: 0x1FEC) ( R/ 32) Peripheral Identification 3 -------- */
#define DSU_PID3_RESETVALUE                   _U_(0x00)                                            /**<  (DSU_PID3) Peripheral Identification 3  Reset Value */

#define DSU_PID3_CUSMOD_Pos                   _U_(0)                                               /**< (DSU_PID3) ARM CUSMOD Position */
#define DSU_PID3_CUSMOD_Msk                   (_U_(0xF) << DSU_PID3_CUSMOD_Pos)                    /**< (DSU_PID3) ARM CUSMOD Mask */
#define DSU_PID3_CUSMOD(value)                (DSU_PID3_CUSMOD_Msk & ((value) << DSU_PID3_CUSMOD_Pos))
#define DSU_PID3_REVAND_Pos                   _U_(4)                                               /**< (DSU_PID3) Revision Number Position */
#define DSU_PID3_REVAND_Msk                   (_U_(0xF) << DSU_PID3_REVAND_Pos)                    /**< (DSU_PID3) Revision Number Mask */
#define DSU_PID3_REVAND(value)                (DSU_PID3_REVAND_Msk & ((value) << DSU_PID3_REVAND_Pos))
#define DSU_PID3_Msk                          _U_(0x000000FF)                                      /**< (DSU_PID3) Register Mask  */


/* -------- DSU_CID0 : (DSU Offset: 0x1FF0) ( R/ 32) Component Identification 0 -------- */
#define DSU_CID0_RESETVALUE                   _U_(0x0D)                                            /**<  (DSU_CID0) Component Identification 0  Reset Value */

#define DSU_CID0_PREAMBLEB0_Pos               _U_(0)                                               /**< (DSU_CID0) Preamble Byte 0 Position */
#define DSU_CID0_PREAMBLEB0_Msk               (_U_(0xFF) << DSU_CID0_PREAMBLEB0_Pos)               /**< (DSU_CID0) Preamble Byte 0 Mask */
#define DSU_CID0_PREAMBLEB0(value)            (DSU_CID0_PREAMBLEB0_Msk & ((value) << DSU_CID0_PREAMBLEB0_Pos))
#define DSU_CID0_Msk                          _U_(0x000000FF)                                      /**< (DSU_CID0) Register Mask  */


/* -------- DSU_CID1 : (DSU Offset: 0x1FF4) ( R/ 32) Component Identification 1 -------- */
#define DSU_CID1_RESETVALUE                   _U_(0x10)                                            /**<  (DSU_CID1) Component Identification 1  Reset Value */

#define DSU_CID1_PREAMBLE_Pos                 _U_(0)                                               /**< (DSU_CID1) Preamble Position */
#define DSU_CID1_PREAMBLE_Msk                 (_U_(0xF) << DSU_CID1_PREAMBLE_Pos)                  /**< (DSU_CID1) Preamble Mask */
#define DSU_CID1_PREAMBLE(value)              (DSU_CID1_PREAMBLE_Msk & ((value) << DSU_CID1_PREAMBLE_Pos))
#define DSU_CID1_CCLASS_Pos                   _U_(4)                                               /**< (DSU_CID1) Component Class Position */
#define DSU_CID1_CCLASS_Msk                   (_U_(0xF) << DSU_CID1_CCLASS_Pos)                    /**< (DSU_CID1) Component Class Mask */
#define DSU_CID1_CCLASS(value)                (DSU_CID1_CCLASS_Msk & ((value) << DSU_CID1_CCLASS_Pos))
#define DSU_CID1_Msk                          _U_(0x000000FF)                                      /**< (DSU_CID1) Register Mask  */


/* -------- DSU_CID2 : (DSU Offset: 0x1FF8) ( R/ 32) Component Identification 2 -------- */
#define DSU_CID2_RESETVALUE                   _U_(0x05)                                            /**<  (DSU_CID2) Component Identification 2  Reset Value */

#define DSU_CID2_PREAMBLEB2_Pos               _U_(0)                                               /**< (DSU_CID2) Preamble Byte 2 Position */
#define DSU_CID2_PREAMBLEB2_Msk               (_U_(0xFF) << DSU_CID2_PREAMBLEB2_Pos)               /**< (DSU_CID2) Preamble Byte 2 Mask */
#define DSU_CID2_PREAMBLEB2(value)            (DSU_CID2_PREAMBLEB2_Msk & ((value) << DSU_CID2_PREAMBLEB2_Pos))
#define DSU_CID2_Msk                          _U_(0x000000FF)                                      /**< (DSU_CID2) Register Mask  */


/* -------- DSU_CID3 : (DSU Offset: 0x1FFC) ( R/ 32) Component Identification 3 -------- */
#define DSU_CID3_RESETVALUE                   _U_(0xB1)                                            /**<  (DSU_CID3) Component Identification 3  Reset Value */

#define DSU_CID3_PREAMBLEB3_Pos               _U_(0)                                               /**< (DSU_CID3) Preamble Byte 3 Position */
#define DSU_CID3_PREAMBLEB3_Msk               (_U_(0xFF) << DSU_CID3_PREAMBLEB3_Pos)               /**< (DSU_CID3) Preamble Byte 3 Mask */
#define DSU_CID3_PREAMBLEB3(value)            (DSU_CID3_PREAMBLEB3_Msk & ((value) << DSU_CID3_PREAMBLEB3_Pos))
#define DSU_CID3_Msk                          _U_(0x000000FF)                                      /**< (DSU_CID3) Register Mask  */


/** \brief DSU register offsets definitions */
#define DSU_CTRL_REG_OFST              (0x00)              /**< (DSU_CTRL) Control Offset */
#define DSU_STATUSA_REG_OFST           (0x01)              /**< (DSU_STATUSA) Status A Offset */
#define DSU_STATUSB_REG_OFST           (0x02)              /**< (DSU_STATUSB) Status B Offset */
#define DSU_ADDR_REG_OFST              (0x04)              /**< (DSU_ADDR) Address Offset */
#define DSU_LENGTH_REG_OFST            (0x08)              /**< (DSU_LENGTH) Length Offset */
#define DSU_DATA_REG_OFST              (0x0C)              /**< (DSU_DATA) Data Offset */
#define DSU_DCC_REG_OFST               (0x10)              /**< (DSU_DCC) Debug Communication Channel n Offset */
#define DSU_DID_REG_OFST               (0x18)              /**< (DSU_DID) Device Identification Offset */
#define DSU_CFG_REG_OFST               (0x1C)              /**< (DSU_CFG) Configuration Offset */
#define DSU_DCFG_REG_OFST              (0xF0)              /**< (DSU_DCFG) Device Configuration Offset */
#define DSU_ENTRY0_REG_OFST            (0x1000)            /**< (DSU_ENTRY0) CoreSight ROM Table Entry 0 Offset */
#define DSU_ENTRY1_REG_OFST            (0x1004)            /**< (DSU_ENTRY1) CoreSight ROM Table Entry 1 Offset */
#define DSU_END_REG_OFST               (0x1008)            /**< (DSU_END) CoreSight ROM Table End Offset */
#define DSU_MEMTYPE_REG_OFST           (0x1FCC)            /**< (DSU_MEMTYPE) CoreSight ROM Table Memory Type Offset */
#define DSU_PID4_REG_OFST              (0x1FD0)            /**< (DSU_PID4) Peripheral Identification 4 Offset */
#define DSU_PID5_REG_OFST              (0x1FD4)            /**< (DSU_PID5) Peripheral Identification 5 Offset */
#define DSU_PID6_REG_OFST              (0x1FD8)            /**< (DSU_PID6) Peripheral Identification 6 Offset */
#define DSU_PID7_REG_OFST              (0x1FDC)            /**< (DSU_PID7) Peripheral Identification 7 Offset */
#define DSU_PID0_REG_OFST              (0x1FE0)            /**< (DSU_PID0) Peripheral Identification 0 Offset */
#define DSU_PID1_REG_OFST              (0x1FE4)            /**< (DSU_PID1) Peripheral Identification 1 Offset */
#define DSU_PID2_REG_OFST              (0x1FE8)            /**< (DSU_PID2) Peripheral Identification 2 Offset */
#define DSU_PID3_REG_OFST              (0x1FEC)            /**< (DSU_PID3) Peripheral Identification 3 Offset */
#define DSU_CID0_REG_OFST              (0x1FF0)            /**< (DSU_CID0) Component Identification 0 Offset */
#define DSU_CID1_REG_OFST              (0x1FF4)            /**< (DSU_CID1) Component Identification 1 Offset */
#define DSU_CID2_REG_OFST              (0x1FF8)            /**< (DSU_CID2) Component Identification 2 Offset */
#define DSU_CID3_REG_OFST              (0x1FFC)            /**< (DSU_CID3) Component Identification 3 Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief DSU register API structure */
typedef struct
{  /* Device Service Unit */
  __O   uint8_t                        DSU_CTRL;           /**< Offset: 0x00 ( /W  8) Control */
  __IO  uint8_t                        DSU_STATUSA;        /**< Offset: 0x01 (R/W  8) Status A */
  __I   uint8_t                        DSU_STATUSB;        /**< Offset: 0x02 (R/   8) Status B */
  __I   uint8_t                        Reserved1[0x01];
  __IO  uint32_t                       DSU_ADDR;           /**< Offset: 0x04 (R/W  32) Address */
  __IO  uint32_t                       DSU_LENGTH;         /**< Offset: 0x08 (R/W  32) Length */
  __IO  uint32_t                       DSU_DATA;           /**< Offset: 0x0C (R/W  32) Data */
  __IO  uint32_t                       DSU_DCC[2];         /**< Offset: 0x10 (R/W  32) Debug Communication Channel n */
  __I   uint32_t                       DSU_DID;            /**< Offset: 0x18 (R/   32) Device Identification */
  __IO  uint32_t                       DSU_CFG;            /**< Offset: 0x1C (R/W  32) Configuration */
  __I   uint8_t                        Reserved2[0xD0];
  __IO  uint32_t                       DSU_DCFG[2];        /**< Offset: 0xF0 (R/W  32) Device Configuration */
  __I   uint8_t                        Reserved3[0xF08];
  __I   uint32_t                       DSU_ENTRY0;         /**< Offset: 0x1000 (R/   32) CoreSight ROM Table Entry 0 */
  __I   uint32_t                       DSU_ENTRY1;         /**< Offset: 0x1004 (R/   32) CoreSight ROM Table Entry 1 */
  __I   uint32_t                       DSU_END;            /**< Offset: 0x1008 (R/   32) CoreSight ROM Table End */
  __I   uint8_t                        Reserved4[0xFC0];
  __I   uint32_t                       DSU_MEMTYPE;        /**< Offset: 0x1FCC (R/   32) CoreSight ROM Table Memory Type */
  __I   uint32_t                       DSU_PID4;           /**< Offset: 0x1FD0 (R/   32) Peripheral Identification 4 */
  __I   uint32_t                       DSU_PID5;           /**< Offset: 0x1FD4 (R/   32) Peripheral Identification 5 */
  __I   uint32_t                       DSU_PID6;           /**< Offset: 0x1FD8 (R/   32) Peripheral Identification 6 */
  __I   uint32_t                       DSU_PID7;           /**< Offset: 0x1FDC (R/   32) Peripheral Identification 7 */
  __I   uint32_t                       DSU_PID0;           /**< Offset: 0x1FE0 (R/   32) Peripheral Identification 0 */
  __I   uint32_t                       DSU_PID1;           /**< Offset: 0x1FE4 (R/   32) Peripheral Identification 1 */
  __I   uint32_t                       DSU_PID2;           /**< Offset: 0x1FE8 (R/   32) Peripheral Identification 2 */
  __I   uint32_t                       DSU_PID3;           /**< Offset: 0x1FEC (R/   32) Peripheral Identification 3 */
  __I   uint32_t                       DSU_CID0;           /**< Offset: 0x1FF0 (R/   32) Component Identification 0 */
  __I   uint32_t                       DSU_CID1;           /**< Offset: 0x1FF4 (R/   32) Component Identification 1 */
  __I   uint32_t                       DSU_CID2;           /**< Offset: 0x1FF8 (R/   32) Component Identification 2 */
  __I   uint32_t                       DSU_CID3;           /**< Offset: 0x1FFC (R/   32) Component Identification 3 */
} dsu_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_DSU_COMPONENT_H_ */
