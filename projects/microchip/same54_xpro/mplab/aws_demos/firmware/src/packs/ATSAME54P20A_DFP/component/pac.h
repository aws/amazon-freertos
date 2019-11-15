/**
 * \brief Component description for PAC
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
#ifndef _SAME54_PAC_COMPONENT_H_
#define _SAME54_PAC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR PAC                                          */
/* ************************************************************************** */

/* -------- PAC_WRCTRL : (PAC Offset: 0x00) (R/W 32) Write control -------- */
#define PAC_WRCTRL_RESETVALUE                 _U_(0x00)                                            /**<  (PAC_WRCTRL) Write control  Reset Value */

#define PAC_WRCTRL_PERID_Pos                  _U_(0)                                               /**< (PAC_WRCTRL) Peripheral identifier Position */
#define PAC_WRCTRL_PERID_Msk                  (_U_(0xFFFF) << PAC_WRCTRL_PERID_Pos)                /**< (PAC_WRCTRL) Peripheral identifier Mask */
#define PAC_WRCTRL_PERID(value)               (PAC_WRCTRL_PERID_Msk & ((value) << PAC_WRCTRL_PERID_Pos))
#define PAC_WRCTRL_KEY_Pos                    _U_(16)                                              /**< (PAC_WRCTRL) Peripheral access control key Position */
#define PAC_WRCTRL_KEY_Msk                    (_U_(0xFF) << PAC_WRCTRL_KEY_Pos)                    /**< (PAC_WRCTRL) Peripheral access control key Mask */
#define PAC_WRCTRL_KEY(value)                 (PAC_WRCTRL_KEY_Msk & ((value) << PAC_WRCTRL_KEY_Pos))
#define   PAC_WRCTRL_KEY_OFF_Val              _U_(0x0)                                             /**< (PAC_WRCTRL) No action  */
#define   PAC_WRCTRL_KEY_CLR_Val              _U_(0x1)                                             /**< (PAC_WRCTRL) Clear protection  */
#define   PAC_WRCTRL_KEY_SET_Val              _U_(0x2)                                             /**< (PAC_WRCTRL) Set protection  */
#define   PAC_WRCTRL_KEY_SETLCK_Val           _U_(0x3)                                             /**< (PAC_WRCTRL) Set and lock protection  */
#define PAC_WRCTRL_KEY_OFF                    (PAC_WRCTRL_KEY_OFF_Val << PAC_WRCTRL_KEY_Pos)       /**< (PAC_WRCTRL) No action Position  */
#define PAC_WRCTRL_KEY_CLR                    (PAC_WRCTRL_KEY_CLR_Val << PAC_WRCTRL_KEY_Pos)       /**< (PAC_WRCTRL) Clear protection Position  */
#define PAC_WRCTRL_KEY_SET                    (PAC_WRCTRL_KEY_SET_Val << PAC_WRCTRL_KEY_Pos)       /**< (PAC_WRCTRL) Set protection Position  */
#define PAC_WRCTRL_KEY_SETLCK                 (PAC_WRCTRL_KEY_SETLCK_Val << PAC_WRCTRL_KEY_Pos)    /**< (PAC_WRCTRL) Set and lock protection Position  */
#define PAC_WRCTRL_Msk                        _U_(0x00FFFFFF)                                      /**< (PAC_WRCTRL) Register Mask  */


/* -------- PAC_EVCTRL : (PAC Offset: 0x04) (R/W 8) Event control -------- */
#define PAC_EVCTRL_RESETVALUE                 _U_(0x00)                                            /**<  (PAC_EVCTRL) Event control  Reset Value */

#define PAC_EVCTRL_ERREO_Pos                  _U_(0)                                               /**< (PAC_EVCTRL) Peripheral acess error event output Position */
#define PAC_EVCTRL_ERREO_Msk                  (_U_(0x1) << PAC_EVCTRL_ERREO_Pos)                   /**< (PAC_EVCTRL) Peripheral acess error event output Mask */
#define PAC_EVCTRL_ERREO(value)               (PAC_EVCTRL_ERREO_Msk & ((value) << PAC_EVCTRL_ERREO_Pos))
#define PAC_EVCTRL_Msk                        _U_(0x01)                                            /**< (PAC_EVCTRL) Register Mask  */


/* -------- PAC_INTENCLR : (PAC Offset: 0x08) (R/W 8) Interrupt enable clear -------- */
#define PAC_INTENCLR_RESETVALUE               _U_(0x00)                                            /**<  (PAC_INTENCLR) Interrupt enable clear  Reset Value */

#define PAC_INTENCLR_ERR_Pos                  _U_(0)                                               /**< (PAC_INTENCLR) Peripheral access error interrupt disable Position */
#define PAC_INTENCLR_ERR_Msk                  (_U_(0x1) << PAC_INTENCLR_ERR_Pos)                   /**< (PAC_INTENCLR) Peripheral access error interrupt disable Mask */
#define PAC_INTENCLR_ERR(value)               (PAC_INTENCLR_ERR_Msk & ((value) << PAC_INTENCLR_ERR_Pos))
#define PAC_INTENCLR_Msk                      _U_(0x01)                                            /**< (PAC_INTENCLR) Register Mask  */


/* -------- PAC_INTENSET : (PAC Offset: 0x09) (R/W 8) Interrupt enable set -------- */
#define PAC_INTENSET_RESETVALUE               _U_(0x00)                                            /**<  (PAC_INTENSET) Interrupt enable set  Reset Value */

#define PAC_INTENSET_ERR_Pos                  _U_(0)                                               /**< (PAC_INTENSET) Peripheral access error interrupt enable Position */
#define PAC_INTENSET_ERR_Msk                  (_U_(0x1) << PAC_INTENSET_ERR_Pos)                   /**< (PAC_INTENSET) Peripheral access error interrupt enable Mask */
#define PAC_INTENSET_ERR(value)               (PAC_INTENSET_ERR_Msk & ((value) << PAC_INTENSET_ERR_Pos))
#define PAC_INTENSET_Msk                      _U_(0x01)                                            /**< (PAC_INTENSET) Register Mask  */


/* -------- PAC_INTFLAGAHB : (PAC Offset: 0x10) (R/W 32) Bridge interrupt flag status -------- */
#define PAC_INTFLAGAHB_RESETVALUE             _U_(0x00)                                            /**<  (PAC_INTFLAGAHB) Bridge interrupt flag status  Reset Value */

#define PAC_INTFLAGAHB_FLASH_Pos              _U_(0)                                               /**< (PAC_INTFLAGAHB) FLASH Position */
#define PAC_INTFLAGAHB_FLASH_Msk              (_U_(0x1) << PAC_INTFLAGAHB_FLASH_Pos)               /**< (PAC_INTFLAGAHB) FLASH Mask */
#define PAC_INTFLAGAHB_FLASH(value)           (PAC_INTFLAGAHB_FLASH_Msk & ((value) << PAC_INTFLAGAHB_FLASH_Pos))
#define PAC_INTFLAGAHB_FLASH_ALT_Pos          _U_(1)                                               /**< (PAC_INTFLAGAHB) FLASH_ALT Position */
#define PAC_INTFLAGAHB_FLASH_ALT_Msk          (_U_(0x1) << PAC_INTFLAGAHB_FLASH_ALT_Pos)           /**< (PAC_INTFLAGAHB) FLASH_ALT Mask */
#define PAC_INTFLAGAHB_FLASH_ALT(value)       (PAC_INTFLAGAHB_FLASH_ALT_Msk & ((value) << PAC_INTFLAGAHB_FLASH_ALT_Pos))
#define PAC_INTFLAGAHB_SEEPROM_Pos            _U_(2)                                               /**< (PAC_INTFLAGAHB) SEEPROM Position */
#define PAC_INTFLAGAHB_SEEPROM_Msk            (_U_(0x1) << PAC_INTFLAGAHB_SEEPROM_Pos)             /**< (PAC_INTFLAGAHB) SEEPROM Mask */
#define PAC_INTFLAGAHB_SEEPROM(value)         (PAC_INTFLAGAHB_SEEPROM_Msk & ((value) << PAC_INTFLAGAHB_SEEPROM_Pos))
#define PAC_INTFLAGAHB_RAMCM4S_Pos            _U_(3)                                               /**< (PAC_INTFLAGAHB) RAMCM4S Position */
#define PAC_INTFLAGAHB_RAMCM4S_Msk            (_U_(0x1) << PAC_INTFLAGAHB_RAMCM4S_Pos)             /**< (PAC_INTFLAGAHB) RAMCM4S Mask */
#define PAC_INTFLAGAHB_RAMCM4S(value)         (PAC_INTFLAGAHB_RAMCM4S_Msk & ((value) << PAC_INTFLAGAHB_RAMCM4S_Pos))
#define PAC_INTFLAGAHB_RAMPPPDSU_Pos          _U_(4)                                               /**< (PAC_INTFLAGAHB) RAMPPPDSU Position */
#define PAC_INTFLAGAHB_RAMPPPDSU_Msk          (_U_(0x1) << PAC_INTFLAGAHB_RAMPPPDSU_Pos)           /**< (PAC_INTFLAGAHB) RAMPPPDSU Mask */
#define PAC_INTFLAGAHB_RAMPPPDSU(value)       (PAC_INTFLAGAHB_RAMPPPDSU_Msk & ((value) << PAC_INTFLAGAHB_RAMPPPDSU_Pos))
#define PAC_INTFLAGAHB_RAMDMAWR_Pos           _U_(5)                                               /**< (PAC_INTFLAGAHB) RAMDMAWR Position */
#define PAC_INTFLAGAHB_RAMDMAWR_Msk           (_U_(0x1) << PAC_INTFLAGAHB_RAMDMAWR_Pos)            /**< (PAC_INTFLAGAHB) RAMDMAWR Mask */
#define PAC_INTFLAGAHB_RAMDMAWR(value)        (PAC_INTFLAGAHB_RAMDMAWR_Msk & ((value) << PAC_INTFLAGAHB_RAMDMAWR_Pos))
#define PAC_INTFLAGAHB_RAMDMACICM_Pos         _U_(6)                                               /**< (PAC_INTFLAGAHB) RAMDMACICM Position */
#define PAC_INTFLAGAHB_RAMDMACICM_Msk         (_U_(0x1) << PAC_INTFLAGAHB_RAMDMACICM_Pos)          /**< (PAC_INTFLAGAHB) RAMDMACICM Mask */
#define PAC_INTFLAGAHB_RAMDMACICM(value)      (PAC_INTFLAGAHB_RAMDMACICM_Msk & ((value) << PAC_INTFLAGAHB_RAMDMACICM_Pos))
#define PAC_INTFLAGAHB_HPB0_Pos               _U_(7)                                               /**< (PAC_INTFLAGAHB) HPB0 Position */
#define PAC_INTFLAGAHB_HPB0_Msk               (_U_(0x1) << PAC_INTFLAGAHB_HPB0_Pos)                /**< (PAC_INTFLAGAHB) HPB0 Mask */
#define PAC_INTFLAGAHB_HPB0(value)            (PAC_INTFLAGAHB_HPB0_Msk & ((value) << PAC_INTFLAGAHB_HPB0_Pos))
#define PAC_INTFLAGAHB_HPB1_Pos               _U_(8)                                               /**< (PAC_INTFLAGAHB) HPB1 Position */
#define PAC_INTFLAGAHB_HPB1_Msk               (_U_(0x1) << PAC_INTFLAGAHB_HPB1_Pos)                /**< (PAC_INTFLAGAHB) HPB1 Mask */
#define PAC_INTFLAGAHB_HPB1(value)            (PAC_INTFLAGAHB_HPB1_Msk & ((value) << PAC_INTFLAGAHB_HPB1_Pos))
#define PAC_INTFLAGAHB_HPB2_Pos               _U_(9)                                               /**< (PAC_INTFLAGAHB) HPB2 Position */
#define PAC_INTFLAGAHB_HPB2_Msk               (_U_(0x1) << PAC_INTFLAGAHB_HPB2_Pos)                /**< (PAC_INTFLAGAHB) HPB2 Mask */
#define PAC_INTFLAGAHB_HPB2(value)            (PAC_INTFLAGAHB_HPB2_Msk & ((value) << PAC_INTFLAGAHB_HPB2_Pos))
#define PAC_INTFLAGAHB_HPB3_Pos               _U_(10)                                              /**< (PAC_INTFLAGAHB) HPB3 Position */
#define PAC_INTFLAGAHB_HPB3_Msk               (_U_(0x1) << PAC_INTFLAGAHB_HPB3_Pos)                /**< (PAC_INTFLAGAHB) HPB3 Mask */
#define PAC_INTFLAGAHB_HPB3(value)            (PAC_INTFLAGAHB_HPB3_Msk & ((value) << PAC_INTFLAGAHB_HPB3_Pos))
#define PAC_INTFLAGAHB_PUKCC_Pos              _U_(11)                                              /**< (PAC_INTFLAGAHB) PUKCC Position */
#define PAC_INTFLAGAHB_PUKCC_Msk              (_U_(0x1) << PAC_INTFLAGAHB_PUKCC_Pos)               /**< (PAC_INTFLAGAHB) PUKCC Mask */
#define PAC_INTFLAGAHB_PUKCC(value)           (PAC_INTFLAGAHB_PUKCC_Msk & ((value) << PAC_INTFLAGAHB_PUKCC_Pos))
#define PAC_INTFLAGAHB_SDHC0_Pos              _U_(12)                                              /**< (PAC_INTFLAGAHB) SDHC0 Position */
#define PAC_INTFLAGAHB_SDHC0_Msk              (_U_(0x1) << PAC_INTFLAGAHB_SDHC0_Pos)               /**< (PAC_INTFLAGAHB) SDHC0 Mask */
#define PAC_INTFLAGAHB_SDHC0(value)           (PAC_INTFLAGAHB_SDHC0_Msk & ((value) << PAC_INTFLAGAHB_SDHC0_Pos))
#define PAC_INTFLAGAHB_SDHC1_Pos              _U_(13)                                              /**< (PAC_INTFLAGAHB) SDHC1 Position */
#define PAC_INTFLAGAHB_SDHC1_Msk              (_U_(0x1) << PAC_INTFLAGAHB_SDHC1_Pos)               /**< (PAC_INTFLAGAHB) SDHC1 Mask */
#define PAC_INTFLAGAHB_SDHC1(value)           (PAC_INTFLAGAHB_SDHC1_Msk & ((value) << PAC_INTFLAGAHB_SDHC1_Pos))
#define PAC_INTFLAGAHB_QSPI_Pos               _U_(14)                                              /**< (PAC_INTFLAGAHB) QSPI Position */
#define PAC_INTFLAGAHB_QSPI_Msk               (_U_(0x1) << PAC_INTFLAGAHB_QSPI_Pos)                /**< (PAC_INTFLAGAHB) QSPI Mask */
#define PAC_INTFLAGAHB_QSPI(value)            (PAC_INTFLAGAHB_QSPI_Msk & ((value) << PAC_INTFLAGAHB_QSPI_Pos))
#define PAC_INTFLAGAHB_BKUPRAM_Pos            _U_(15)                                              /**< (PAC_INTFLAGAHB) BKUPRAM Position */
#define PAC_INTFLAGAHB_BKUPRAM_Msk            (_U_(0x1) << PAC_INTFLAGAHB_BKUPRAM_Pos)             /**< (PAC_INTFLAGAHB) BKUPRAM Mask */
#define PAC_INTFLAGAHB_BKUPRAM(value)         (PAC_INTFLAGAHB_BKUPRAM_Msk & ((value) << PAC_INTFLAGAHB_BKUPRAM_Pos))
#define PAC_INTFLAGAHB_Msk                    _U_(0x0000FFFF)                                      /**< (PAC_INTFLAGAHB) Register Mask  */

#define PAC_INTFLAGAHB_HPB_Pos                _U_(7)                                               /**< (PAC_INTFLAGAHB Position) HPBx */
#define PAC_INTFLAGAHB_HPB_Msk                (_U_(0xF) << PAC_INTFLAGAHB_HPB_Pos)                 /**< (PAC_INTFLAGAHB Mask) HPB */
#define PAC_INTFLAGAHB_HPB(value)             (PAC_INTFLAGAHB_HPB_Msk & ((value) << PAC_INTFLAGAHB_HPB_Pos)) 
#define PAC_INTFLAGAHB_SDHC_Pos               _U_(12)                                              /**< (PAC_INTFLAGAHB Position) SDHCx */
#define PAC_INTFLAGAHB_SDHC_Msk               (_U_(0x3) << PAC_INTFLAGAHB_SDHC_Pos)                /**< (PAC_INTFLAGAHB Mask) SDHC */
#define PAC_INTFLAGAHB_SDHC(value)            (PAC_INTFLAGAHB_SDHC_Msk & ((value) << PAC_INTFLAGAHB_SDHC_Pos)) 

/* -------- PAC_INTFLAGA : (PAC Offset: 0x14) (R/W 32) Peripheral interrupt flag status - Bridge A -------- */
#define PAC_INTFLAGA_RESETVALUE               _U_(0x00)                                            /**<  (PAC_INTFLAGA) Peripheral interrupt flag status - Bridge A  Reset Value */

#define PAC_INTFLAGA_PAC_Pos                  _U_(0)                                               /**< (PAC_INTFLAGA) PAC Position */
#define PAC_INTFLAGA_PAC_Msk                  (_U_(0x1) << PAC_INTFLAGA_PAC_Pos)                   /**< (PAC_INTFLAGA) PAC Mask */
#define PAC_INTFLAGA_PAC(value)               (PAC_INTFLAGA_PAC_Msk & ((value) << PAC_INTFLAGA_PAC_Pos))
#define PAC_INTFLAGA_PM_Pos                   _U_(1)                                               /**< (PAC_INTFLAGA) PM Position */
#define PAC_INTFLAGA_PM_Msk                   (_U_(0x1) << PAC_INTFLAGA_PM_Pos)                    /**< (PAC_INTFLAGA) PM Mask */
#define PAC_INTFLAGA_PM(value)                (PAC_INTFLAGA_PM_Msk & ((value) << PAC_INTFLAGA_PM_Pos))
#define PAC_INTFLAGA_MCLK_Pos                 _U_(2)                                               /**< (PAC_INTFLAGA) MCLK Position */
#define PAC_INTFLAGA_MCLK_Msk                 (_U_(0x1) << PAC_INTFLAGA_MCLK_Pos)                  /**< (PAC_INTFLAGA) MCLK Mask */
#define PAC_INTFLAGA_MCLK(value)              (PAC_INTFLAGA_MCLK_Msk & ((value) << PAC_INTFLAGA_MCLK_Pos))
#define PAC_INTFLAGA_RSTC_Pos                 _U_(3)                                               /**< (PAC_INTFLAGA) RSTC Position */
#define PAC_INTFLAGA_RSTC_Msk                 (_U_(0x1) << PAC_INTFLAGA_RSTC_Pos)                  /**< (PAC_INTFLAGA) RSTC Mask */
#define PAC_INTFLAGA_RSTC(value)              (PAC_INTFLAGA_RSTC_Msk & ((value) << PAC_INTFLAGA_RSTC_Pos))
#define PAC_INTFLAGA_OSCCTRL_Pos              _U_(4)                                               /**< (PAC_INTFLAGA) OSCCTRL Position */
#define PAC_INTFLAGA_OSCCTRL_Msk              (_U_(0x1) << PAC_INTFLAGA_OSCCTRL_Pos)               /**< (PAC_INTFLAGA) OSCCTRL Mask */
#define PAC_INTFLAGA_OSCCTRL(value)           (PAC_INTFLAGA_OSCCTRL_Msk & ((value) << PAC_INTFLAGA_OSCCTRL_Pos))
#define PAC_INTFLAGA_OSC32KCTRL_Pos           _U_(5)                                               /**< (PAC_INTFLAGA) OSC32KCTRL Position */
#define PAC_INTFLAGA_OSC32KCTRL_Msk           (_U_(0x1) << PAC_INTFLAGA_OSC32KCTRL_Pos)            /**< (PAC_INTFLAGA) OSC32KCTRL Mask */
#define PAC_INTFLAGA_OSC32KCTRL(value)        (PAC_INTFLAGA_OSC32KCTRL_Msk & ((value) << PAC_INTFLAGA_OSC32KCTRL_Pos))
#define PAC_INTFLAGA_SUPC_Pos                 _U_(6)                                               /**< (PAC_INTFLAGA) SUPC Position */
#define PAC_INTFLAGA_SUPC_Msk                 (_U_(0x1) << PAC_INTFLAGA_SUPC_Pos)                  /**< (PAC_INTFLAGA) SUPC Mask */
#define PAC_INTFLAGA_SUPC(value)              (PAC_INTFLAGA_SUPC_Msk & ((value) << PAC_INTFLAGA_SUPC_Pos))
#define PAC_INTFLAGA_GCLK_Pos                 _U_(7)                                               /**< (PAC_INTFLAGA) GCLK Position */
#define PAC_INTFLAGA_GCLK_Msk                 (_U_(0x1) << PAC_INTFLAGA_GCLK_Pos)                  /**< (PAC_INTFLAGA) GCLK Mask */
#define PAC_INTFLAGA_GCLK(value)              (PAC_INTFLAGA_GCLK_Msk & ((value) << PAC_INTFLAGA_GCLK_Pos))
#define PAC_INTFLAGA_WDT_Pos                  _U_(8)                                               /**< (PAC_INTFLAGA) WDT Position */
#define PAC_INTFLAGA_WDT_Msk                  (_U_(0x1) << PAC_INTFLAGA_WDT_Pos)                   /**< (PAC_INTFLAGA) WDT Mask */
#define PAC_INTFLAGA_WDT(value)               (PAC_INTFLAGA_WDT_Msk & ((value) << PAC_INTFLAGA_WDT_Pos))
#define PAC_INTFLAGA_RTC_Pos                  _U_(9)                                               /**< (PAC_INTFLAGA) RTC Position */
#define PAC_INTFLAGA_RTC_Msk                  (_U_(0x1) << PAC_INTFLAGA_RTC_Pos)                   /**< (PAC_INTFLAGA) RTC Mask */
#define PAC_INTFLAGA_RTC(value)               (PAC_INTFLAGA_RTC_Msk & ((value) << PAC_INTFLAGA_RTC_Pos))
#define PAC_INTFLAGA_EIC_Pos                  _U_(10)                                              /**< (PAC_INTFLAGA) EIC Position */
#define PAC_INTFLAGA_EIC_Msk                  (_U_(0x1) << PAC_INTFLAGA_EIC_Pos)                   /**< (PAC_INTFLAGA) EIC Mask */
#define PAC_INTFLAGA_EIC(value)               (PAC_INTFLAGA_EIC_Msk & ((value) << PAC_INTFLAGA_EIC_Pos))
#define PAC_INTFLAGA_FREQM_Pos                _U_(11)                                              /**< (PAC_INTFLAGA) FREQM Position */
#define PAC_INTFLAGA_FREQM_Msk                (_U_(0x1) << PAC_INTFLAGA_FREQM_Pos)                 /**< (PAC_INTFLAGA) FREQM Mask */
#define PAC_INTFLAGA_FREQM(value)             (PAC_INTFLAGA_FREQM_Msk & ((value) << PAC_INTFLAGA_FREQM_Pos))
#define PAC_INTFLAGA_SERCOM0_Pos              _U_(12)                                              /**< (PAC_INTFLAGA) SERCOM0 Position */
#define PAC_INTFLAGA_SERCOM0_Msk              (_U_(0x1) << PAC_INTFLAGA_SERCOM0_Pos)               /**< (PAC_INTFLAGA) SERCOM0 Mask */
#define PAC_INTFLAGA_SERCOM0(value)           (PAC_INTFLAGA_SERCOM0_Msk & ((value) << PAC_INTFLAGA_SERCOM0_Pos))
#define PAC_INTFLAGA_SERCOM1_Pos              _U_(13)                                              /**< (PAC_INTFLAGA) SERCOM1 Position */
#define PAC_INTFLAGA_SERCOM1_Msk              (_U_(0x1) << PAC_INTFLAGA_SERCOM1_Pos)               /**< (PAC_INTFLAGA) SERCOM1 Mask */
#define PAC_INTFLAGA_SERCOM1(value)           (PAC_INTFLAGA_SERCOM1_Msk & ((value) << PAC_INTFLAGA_SERCOM1_Pos))
#define PAC_INTFLAGA_TC0_Pos                  _U_(14)                                              /**< (PAC_INTFLAGA) TC0 Position */
#define PAC_INTFLAGA_TC0_Msk                  (_U_(0x1) << PAC_INTFLAGA_TC0_Pos)                   /**< (PAC_INTFLAGA) TC0 Mask */
#define PAC_INTFLAGA_TC0(value)               (PAC_INTFLAGA_TC0_Msk & ((value) << PAC_INTFLAGA_TC0_Pos))
#define PAC_INTFLAGA_TC1_Pos                  _U_(15)                                              /**< (PAC_INTFLAGA) TC1 Position */
#define PAC_INTFLAGA_TC1_Msk                  (_U_(0x1) << PAC_INTFLAGA_TC1_Pos)                   /**< (PAC_INTFLAGA) TC1 Mask */
#define PAC_INTFLAGA_TC1(value)               (PAC_INTFLAGA_TC1_Msk & ((value) << PAC_INTFLAGA_TC1_Pos))
#define PAC_INTFLAGA_Msk                      _U_(0x0000FFFF)                                      /**< (PAC_INTFLAGA) Register Mask  */

#define PAC_INTFLAGA_SERCOM_Pos               _U_(12)                                              /**< (PAC_INTFLAGA Position) SERCOMx */
#define PAC_INTFLAGA_SERCOM_Msk               (_U_(0x3) << PAC_INTFLAGA_SERCOM_Pos)                /**< (PAC_INTFLAGA Mask) SERCOM */
#define PAC_INTFLAGA_SERCOM(value)            (PAC_INTFLAGA_SERCOM_Msk & ((value) << PAC_INTFLAGA_SERCOM_Pos)) 
#define PAC_INTFLAGA_TC_Pos                   _U_(14)                                              /**< (PAC_INTFLAGA Position) TCx */
#define PAC_INTFLAGA_TC_Msk                   (_U_(0x3) << PAC_INTFLAGA_TC_Pos)                    /**< (PAC_INTFLAGA Mask) TC */
#define PAC_INTFLAGA_TC(value)                (PAC_INTFLAGA_TC_Msk & ((value) << PAC_INTFLAGA_TC_Pos)) 

/* -------- PAC_INTFLAGB : (PAC Offset: 0x18) (R/W 32) Peripheral interrupt flag status - Bridge B -------- */
#define PAC_INTFLAGB_RESETVALUE               _U_(0x00)                                            /**<  (PAC_INTFLAGB) Peripheral interrupt flag status - Bridge B  Reset Value */

#define PAC_INTFLAGB_USB_Pos                  _U_(0)                                               /**< (PAC_INTFLAGB) USB Position */
#define PAC_INTFLAGB_USB_Msk                  (_U_(0x1) << PAC_INTFLAGB_USB_Pos)                   /**< (PAC_INTFLAGB) USB Mask */
#define PAC_INTFLAGB_USB(value)               (PAC_INTFLAGB_USB_Msk & ((value) << PAC_INTFLAGB_USB_Pos))
#define PAC_INTFLAGB_DSU_Pos                  _U_(1)                                               /**< (PAC_INTFLAGB) DSU Position */
#define PAC_INTFLAGB_DSU_Msk                  (_U_(0x1) << PAC_INTFLAGB_DSU_Pos)                   /**< (PAC_INTFLAGB) DSU Mask */
#define PAC_INTFLAGB_DSU(value)               (PAC_INTFLAGB_DSU_Msk & ((value) << PAC_INTFLAGB_DSU_Pos))
#define PAC_INTFLAGB_NVMCTRL_Pos              _U_(2)                                               /**< (PAC_INTFLAGB) NVMCTRL Position */
#define PAC_INTFLAGB_NVMCTRL_Msk              (_U_(0x1) << PAC_INTFLAGB_NVMCTRL_Pos)               /**< (PAC_INTFLAGB) NVMCTRL Mask */
#define PAC_INTFLAGB_NVMCTRL(value)           (PAC_INTFLAGB_NVMCTRL_Msk & ((value) << PAC_INTFLAGB_NVMCTRL_Pos))
#define PAC_INTFLAGB_CMCC_Pos                 _U_(3)                                               /**< (PAC_INTFLAGB) CMCC Position */
#define PAC_INTFLAGB_CMCC_Msk                 (_U_(0x1) << PAC_INTFLAGB_CMCC_Pos)                  /**< (PAC_INTFLAGB) CMCC Mask */
#define PAC_INTFLAGB_CMCC(value)              (PAC_INTFLAGB_CMCC_Msk & ((value) << PAC_INTFLAGB_CMCC_Pos))
#define PAC_INTFLAGB_PORT_Pos                 _U_(4)                                               /**< (PAC_INTFLAGB) PORT Position */
#define PAC_INTFLAGB_PORT_Msk                 (_U_(0x1) << PAC_INTFLAGB_PORT_Pos)                  /**< (PAC_INTFLAGB) PORT Mask */
#define PAC_INTFLAGB_PORT(value)              (PAC_INTFLAGB_PORT_Msk & ((value) << PAC_INTFLAGB_PORT_Pos))
#define PAC_INTFLAGB_DMAC_Pos                 _U_(5)                                               /**< (PAC_INTFLAGB) DMAC Position */
#define PAC_INTFLAGB_DMAC_Msk                 (_U_(0x1) << PAC_INTFLAGB_DMAC_Pos)                  /**< (PAC_INTFLAGB) DMAC Mask */
#define PAC_INTFLAGB_DMAC(value)              (PAC_INTFLAGB_DMAC_Msk & ((value) << PAC_INTFLAGB_DMAC_Pos))
#define PAC_INTFLAGB_HMATRIX_Pos              _U_(6)                                               /**< (PAC_INTFLAGB) HMATRIX Position */
#define PAC_INTFLAGB_HMATRIX_Msk              (_U_(0x1) << PAC_INTFLAGB_HMATRIX_Pos)               /**< (PAC_INTFLAGB) HMATRIX Mask */
#define PAC_INTFLAGB_HMATRIX(value)           (PAC_INTFLAGB_HMATRIX_Msk & ((value) << PAC_INTFLAGB_HMATRIX_Pos))
#define PAC_INTFLAGB_EVSYS_Pos                _U_(7)                                               /**< (PAC_INTFLAGB) EVSYS Position */
#define PAC_INTFLAGB_EVSYS_Msk                (_U_(0x1) << PAC_INTFLAGB_EVSYS_Pos)                 /**< (PAC_INTFLAGB) EVSYS Mask */
#define PAC_INTFLAGB_EVSYS(value)             (PAC_INTFLAGB_EVSYS_Msk & ((value) << PAC_INTFLAGB_EVSYS_Pos))
#define PAC_INTFLAGB_SERCOM2_Pos              _U_(9)                                               /**< (PAC_INTFLAGB) SERCOM2 Position */
#define PAC_INTFLAGB_SERCOM2_Msk              (_U_(0x1) << PAC_INTFLAGB_SERCOM2_Pos)               /**< (PAC_INTFLAGB) SERCOM2 Mask */
#define PAC_INTFLAGB_SERCOM2(value)           (PAC_INTFLAGB_SERCOM2_Msk & ((value) << PAC_INTFLAGB_SERCOM2_Pos))
#define PAC_INTFLAGB_SERCOM3_Pos              _U_(10)                                              /**< (PAC_INTFLAGB) SERCOM3 Position */
#define PAC_INTFLAGB_SERCOM3_Msk              (_U_(0x1) << PAC_INTFLAGB_SERCOM3_Pos)               /**< (PAC_INTFLAGB) SERCOM3 Mask */
#define PAC_INTFLAGB_SERCOM3(value)           (PAC_INTFLAGB_SERCOM3_Msk & ((value) << PAC_INTFLAGB_SERCOM3_Pos))
#define PAC_INTFLAGB_TCC0_Pos                 _U_(11)                                              /**< (PAC_INTFLAGB) TCC0 Position */
#define PAC_INTFLAGB_TCC0_Msk                 (_U_(0x1) << PAC_INTFLAGB_TCC0_Pos)                  /**< (PAC_INTFLAGB) TCC0 Mask */
#define PAC_INTFLAGB_TCC0(value)              (PAC_INTFLAGB_TCC0_Msk & ((value) << PAC_INTFLAGB_TCC0_Pos))
#define PAC_INTFLAGB_TCC1_Pos                 _U_(12)                                              /**< (PAC_INTFLAGB) TCC1 Position */
#define PAC_INTFLAGB_TCC1_Msk                 (_U_(0x1) << PAC_INTFLAGB_TCC1_Pos)                  /**< (PAC_INTFLAGB) TCC1 Mask */
#define PAC_INTFLAGB_TCC1(value)              (PAC_INTFLAGB_TCC1_Msk & ((value) << PAC_INTFLAGB_TCC1_Pos))
#define PAC_INTFLAGB_TC2_Pos                  _U_(13)                                              /**< (PAC_INTFLAGB) TC2 Position */
#define PAC_INTFLAGB_TC2_Msk                  (_U_(0x1) << PAC_INTFLAGB_TC2_Pos)                   /**< (PAC_INTFLAGB) TC2 Mask */
#define PAC_INTFLAGB_TC2(value)               (PAC_INTFLAGB_TC2_Msk & ((value) << PAC_INTFLAGB_TC2_Pos))
#define PAC_INTFLAGB_TC3_Pos                  _U_(14)                                              /**< (PAC_INTFLAGB) TC3 Position */
#define PAC_INTFLAGB_TC3_Msk                  (_U_(0x1) << PAC_INTFLAGB_TC3_Pos)                   /**< (PAC_INTFLAGB) TC3 Mask */
#define PAC_INTFLAGB_TC3(value)               (PAC_INTFLAGB_TC3_Msk & ((value) << PAC_INTFLAGB_TC3_Pos))
#define PAC_INTFLAGB_RAMECC_Pos               _U_(16)                                              /**< (PAC_INTFLAGB) RAMECC Position */
#define PAC_INTFLAGB_RAMECC_Msk               (_U_(0x1) << PAC_INTFLAGB_RAMECC_Pos)                /**< (PAC_INTFLAGB) RAMECC Mask */
#define PAC_INTFLAGB_RAMECC(value)            (PAC_INTFLAGB_RAMECC_Msk & ((value) << PAC_INTFLAGB_RAMECC_Pos))
#define PAC_INTFLAGB_Msk                      _U_(0x00017EFF)                                      /**< (PAC_INTFLAGB) Register Mask  */

#define PAC_INTFLAGB_SERCOM_Pos               _U_(9)                                               /**< (PAC_INTFLAGB Position) SERCOM2 */
#define PAC_INTFLAGB_SERCOM_Msk               (_U_(0x3) << PAC_INTFLAGB_SERCOM_Pos)                /**< (PAC_INTFLAGB Mask) SERCOM */
#define PAC_INTFLAGB_SERCOM(value)            (PAC_INTFLAGB_SERCOM_Msk & ((value) << PAC_INTFLAGB_SERCOM_Pos)) 
#define PAC_INTFLAGB_TCC_Pos                  _U_(11)                                              /**< (PAC_INTFLAGB Position) TCCx */
#define PAC_INTFLAGB_TCC_Msk                  (_U_(0x3) << PAC_INTFLAGB_TCC_Pos)                   /**< (PAC_INTFLAGB Mask) TCC */
#define PAC_INTFLAGB_TCC(value)               (PAC_INTFLAGB_TCC_Msk & ((value) << PAC_INTFLAGB_TCC_Pos)) 
#define PAC_INTFLAGB_TC_Pos                   _U_(13)                                              /**< (PAC_INTFLAGB Position) TC2 */
#define PAC_INTFLAGB_TC_Msk                   (_U_(0x3) << PAC_INTFLAGB_TC_Pos)                    /**< (PAC_INTFLAGB Mask) TC */
#define PAC_INTFLAGB_TC(value)                (PAC_INTFLAGB_TC_Msk & ((value) << PAC_INTFLAGB_TC_Pos)) 

/* -------- PAC_INTFLAGC : (PAC Offset: 0x1C) (R/W 32) Peripheral interrupt flag status - Bridge C -------- */
#define PAC_INTFLAGC_RESETVALUE               _U_(0x00)                                            /**<  (PAC_INTFLAGC) Peripheral interrupt flag status - Bridge C  Reset Value */

#define PAC_INTFLAGC_CAN0_Pos                 _U_(0)                                               /**< (PAC_INTFLAGC) CAN0 Position */
#define PAC_INTFLAGC_CAN0_Msk                 (_U_(0x1) << PAC_INTFLAGC_CAN0_Pos)                  /**< (PAC_INTFLAGC) CAN0 Mask */
#define PAC_INTFLAGC_CAN0(value)              (PAC_INTFLAGC_CAN0_Msk & ((value) << PAC_INTFLAGC_CAN0_Pos))
#define PAC_INTFLAGC_CAN1_Pos                 _U_(1)                                               /**< (PAC_INTFLAGC) CAN1 Position */
#define PAC_INTFLAGC_CAN1_Msk                 (_U_(0x1) << PAC_INTFLAGC_CAN1_Pos)                  /**< (PAC_INTFLAGC) CAN1 Mask */
#define PAC_INTFLAGC_CAN1(value)              (PAC_INTFLAGC_CAN1_Msk & ((value) << PAC_INTFLAGC_CAN1_Pos))
#define PAC_INTFLAGC_GMAC_Pos                 _U_(2)                                               /**< (PAC_INTFLAGC) GMAC Position */
#define PAC_INTFLAGC_GMAC_Msk                 (_U_(0x1) << PAC_INTFLAGC_GMAC_Pos)                  /**< (PAC_INTFLAGC) GMAC Mask */
#define PAC_INTFLAGC_GMAC(value)              (PAC_INTFLAGC_GMAC_Msk & ((value) << PAC_INTFLAGC_GMAC_Pos))
#define PAC_INTFLAGC_TCC2_Pos                 _U_(3)                                               /**< (PAC_INTFLAGC) TCC2 Position */
#define PAC_INTFLAGC_TCC2_Msk                 (_U_(0x1) << PAC_INTFLAGC_TCC2_Pos)                  /**< (PAC_INTFLAGC) TCC2 Mask */
#define PAC_INTFLAGC_TCC2(value)              (PAC_INTFLAGC_TCC2_Msk & ((value) << PAC_INTFLAGC_TCC2_Pos))
#define PAC_INTFLAGC_TCC3_Pos                 _U_(4)                                               /**< (PAC_INTFLAGC) TCC3 Position */
#define PAC_INTFLAGC_TCC3_Msk                 (_U_(0x1) << PAC_INTFLAGC_TCC3_Pos)                  /**< (PAC_INTFLAGC) TCC3 Mask */
#define PAC_INTFLAGC_TCC3(value)              (PAC_INTFLAGC_TCC3_Msk & ((value) << PAC_INTFLAGC_TCC3_Pos))
#define PAC_INTFLAGC_TC4_Pos                  _U_(5)                                               /**< (PAC_INTFLAGC) TC4 Position */
#define PAC_INTFLAGC_TC4_Msk                  (_U_(0x1) << PAC_INTFLAGC_TC4_Pos)                   /**< (PAC_INTFLAGC) TC4 Mask */
#define PAC_INTFLAGC_TC4(value)               (PAC_INTFLAGC_TC4_Msk & ((value) << PAC_INTFLAGC_TC4_Pos))
#define PAC_INTFLAGC_TC5_Pos                  _U_(6)                                               /**< (PAC_INTFLAGC) TC5 Position */
#define PAC_INTFLAGC_TC5_Msk                  (_U_(0x1) << PAC_INTFLAGC_TC5_Pos)                   /**< (PAC_INTFLAGC) TC5 Mask */
#define PAC_INTFLAGC_TC5(value)               (PAC_INTFLAGC_TC5_Msk & ((value) << PAC_INTFLAGC_TC5_Pos))
#define PAC_INTFLAGC_PDEC_Pos                 _U_(7)                                               /**< (PAC_INTFLAGC) PDEC Position */
#define PAC_INTFLAGC_PDEC_Msk                 (_U_(0x1) << PAC_INTFLAGC_PDEC_Pos)                  /**< (PAC_INTFLAGC) PDEC Mask */
#define PAC_INTFLAGC_PDEC(value)              (PAC_INTFLAGC_PDEC_Msk & ((value) << PAC_INTFLAGC_PDEC_Pos))
#define PAC_INTFLAGC_AC_Pos                   _U_(8)                                               /**< (PAC_INTFLAGC) AC Position */
#define PAC_INTFLAGC_AC_Msk                   (_U_(0x1) << PAC_INTFLAGC_AC_Pos)                    /**< (PAC_INTFLAGC) AC Mask */
#define PAC_INTFLAGC_AC(value)                (PAC_INTFLAGC_AC_Msk & ((value) << PAC_INTFLAGC_AC_Pos))
#define PAC_INTFLAGC_AES_Pos                  _U_(9)                                               /**< (PAC_INTFLAGC) AES Position */
#define PAC_INTFLAGC_AES_Msk                  (_U_(0x1) << PAC_INTFLAGC_AES_Pos)                   /**< (PAC_INTFLAGC) AES Mask */
#define PAC_INTFLAGC_AES(value)               (PAC_INTFLAGC_AES_Msk & ((value) << PAC_INTFLAGC_AES_Pos))
#define PAC_INTFLAGC_TRNG_Pos                 _U_(10)                                              /**< (PAC_INTFLAGC) TRNG Position */
#define PAC_INTFLAGC_TRNG_Msk                 (_U_(0x1) << PAC_INTFLAGC_TRNG_Pos)                  /**< (PAC_INTFLAGC) TRNG Mask */
#define PAC_INTFLAGC_TRNG(value)              (PAC_INTFLAGC_TRNG_Msk & ((value) << PAC_INTFLAGC_TRNG_Pos))
#define PAC_INTFLAGC_ICM_Pos                  _U_(11)                                              /**< (PAC_INTFLAGC) ICM Position */
#define PAC_INTFLAGC_ICM_Msk                  (_U_(0x1) << PAC_INTFLAGC_ICM_Pos)                   /**< (PAC_INTFLAGC) ICM Mask */
#define PAC_INTFLAGC_ICM(value)               (PAC_INTFLAGC_ICM_Msk & ((value) << PAC_INTFLAGC_ICM_Pos))
#define PAC_INTFLAGC_PUKCC_Pos                _U_(12)                                              /**< (PAC_INTFLAGC) PUKCC Position */
#define PAC_INTFLAGC_PUKCC_Msk                (_U_(0x1) << PAC_INTFLAGC_PUKCC_Pos)                 /**< (PAC_INTFLAGC) PUKCC Mask */
#define PAC_INTFLAGC_PUKCC(value)             (PAC_INTFLAGC_PUKCC_Msk & ((value) << PAC_INTFLAGC_PUKCC_Pos))
#define PAC_INTFLAGC_QSPI_Pos                 _U_(13)                                              /**< (PAC_INTFLAGC) QSPI Position */
#define PAC_INTFLAGC_QSPI_Msk                 (_U_(0x1) << PAC_INTFLAGC_QSPI_Pos)                  /**< (PAC_INTFLAGC) QSPI Mask */
#define PAC_INTFLAGC_QSPI(value)              (PAC_INTFLAGC_QSPI_Msk & ((value) << PAC_INTFLAGC_QSPI_Pos))
#define PAC_INTFLAGC_CCL_Pos                  _U_(14)                                              /**< (PAC_INTFLAGC) CCL Position */
#define PAC_INTFLAGC_CCL_Msk                  (_U_(0x1) << PAC_INTFLAGC_CCL_Pos)                   /**< (PAC_INTFLAGC) CCL Mask */
#define PAC_INTFLAGC_CCL(value)               (PAC_INTFLAGC_CCL_Msk & ((value) << PAC_INTFLAGC_CCL_Pos))
#define PAC_INTFLAGC_Msk                      _U_(0x00007FFF)                                      /**< (PAC_INTFLAGC) Register Mask  */

#define PAC_INTFLAGC_CAN_Pos                  _U_(0)                                               /**< (PAC_INTFLAGC Position) CANx */
#define PAC_INTFLAGC_CAN_Msk                  (_U_(0x3) << PAC_INTFLAGC_CAN_Pos)                   /**< (PAC_INTFLAGC Mask) CAN */
#define PAC_INTFLAGC_CAN(value)               (PAC_INTFLAGC_CAN_Msk & ((value) << PAC_INTFLAGC_CAN_Pos)) 
#define PAC_INTFLAGC_TCC_Pos                  _U_(3)                                               /**< (PAC_INTFLAGC Position) TCC2 */
#define PAC_INTFLAGC_TCC_Msk                  (_U_(0x3) << PAC_INTFLAGC_TCC_Pos)                   /**< (PAC_INTFLAGC Mask) TCC */
#define PAC_INTFLAGC_TCC(value)               (PAC_INTFLAGC_TCC_Msk & ((value) << PAC_INTFLAGC_TCC_Pos)) 
#define PAC_INTFLAGC_TC_Pos                   _U_(5)                                               /**< (PAC_INTFLAGC Position) TC4 */
#define PAC_INTFLAGC_TC_Msk                   (_U_(0x3) << PAC_INTFLAGC_TC_Pos)                    /**< (PAC_INTFLAGC Mask) TC */
#define PAC_INTFLAGC_TC(value)                (PAC_INTFLAGC_TC_Msk & ((value) << PAC_INTFLAGC_TC_Pos)) 

/* -------- PAC_INTFLAGD : (PAC Offset: 0x20) (R/W 32) Peripheral interrupt flag status - Bridge D -------- */
#define PAC_INTFLAGD_RESETVALUE               _U_(0x00)                                            /**<  (PAC_INTFLAGD) Peripheral interrupt flag status - Bridge D  Reset Value */

#define PAC_INTFLAGD_SERCOM4_Pos              _U_(0)                                               /**< (PAC_INTFLAGD) SERCOM4 Position */
#define PAC_INTFLAGD_SERCOM4_Msk              (_U_(0x1) << PAC_INTFLAGD_SERCOM4_Pos)               /**< (PAC_INTFLAGD) SERCOM4 Mask */
#define PAC_INTFLAGD_SERCOM4(value)           (PAC_INTFLAGD_SERCOM4_Msk & ((value) << PAC_INTFLAGD_SERCOM4_Pos))
#define PAC_INTFLAGD_SERCOM5_Pos              _U_(1)                                               /**< (PAC_INTFLAGD) SERCOM5 Position */
#define PAC_INTFLAGD_SERCOM5_Msk              (_U_(0x1) << PAC_INTFLAGD_SERCOM5_Pos)               /**< (PAC_INTFLAGD) SERCOM5 Mask */
#define PAC_INTFLAGD_SERCOM5(value)           (PAC_INTFLAGD_SERCOM5_Msk & ((value) << PAC_INTFLAGD_SERCOM5_Pos))
#define PAC_INTFLAGD_SERCOM6_Pos              _U_(2)                                               /**< (PAC_INTFLAGD) SERCOM6 Position */
#define PAC_INTFLAGD_SERCOM6_Msk              (_U_(0x1) << PAC_INTFLAGD_SERCOM6_Pos)               /**< (PAC_INTFLAGD) SERCOM6 Mask */
#define PAC_INTFLAGD_SERCOM6(value)           (PAC_INTFLAGD_SERCOM6_Msk & ((value) << PAC_INTFLAGD_SERCOM6_Pos))
#define PAC_INTFLAGD_SERCOM7_Pos              _U_(3)                                               /**< (PAC_INTFLAGD) SERCOM7 Position */
#define PAC_INTFLAGD_SERCOM7_Msk              (_U_(0x1) << PAC_INTFLAGD_SERCOM7_Pos)               /**< (PAC_INTFLAGD) SERCOM7 Mask */
#define PAC_INTFLAGD_SERCOM7(value)           (PAC_INTFLAGD_SERCOM7_Msk & ((value) << PAC_INTFLAGD_SERCOM7_Pos))
#define PAC_INTFLAGD_TCC4_Pos                 _U_(4)                                               /**< (PAC_INTFLAGD) TCC4 Position */
#define PAC_INTFLAGD_TCC4_Msk                 (_U_(0x1) << PAC_INTFLAGD_TCC4_Pos)                  /**< (PAC_INTFLAGD) TCC4 Mask */
#define PAC_INTFLAGD_TCC4(value)              (PAC_INTFLAGD_TCC4_Msk & ((value) << PAC_INTFLAGD_TCC4_Pos))
#define PAC_INTFLAGD_TC6_Pos                  _U_(5)                                               /**< (PAC_INTFLAGD) TC6 Position */
#define PAC_INTFLAGD_TC6_Msk                  (_U_(0x1) << PAC_INTFLAGD_TC6_Pos)                   /**< (PAC_INTFLAGD) TC6 Mask */
#define PAC_INTFLAGD_TC6(value)               (PAC_INTFLAGD_TC6_Msk & ((value) << PAC_INTFLAGD_TC6_Pos))
#define PAC_INTFLAGD_TC7_Pos                  _U_(6)                                               /**< (PAC_INTFLAGD) TC7 Position */
#define PAC_INTFLAGD_TC7_Msk                  (_U_(0x1) << PAC_INTFLAGD_TC7_Pos)                   /**< (PAC_INTFLAGD) TC7 Mask */
#define PAC_INTFLAGD_TC7(value)               (PAC_INTFLAGD_TC7_Msk & ((value) << PAC_INTFLAGD_TC7_Pos))
#define PAC_INTFLAGD_ADC0_Pos                 _U_(7)                                               /**< (PAC_INTFLAGD) ADC0 Position */
#define PAC_INTFLAGD_ADC0_Msk                 (_U_(0x1) << PAC_INTFLAGD_ADC0_Pos)                  /**< (PAC_INTFLAGD) ADC0 Mask */
#define PAC_INTFLAGD_ADC0(value)              (PAC_INTFLAGD_ADC0_Msk & ((value) << PAC_INTFLAGD_ADC0_Pos))
#define PAC_INTFLAGD_ADC1_Pos                 _U_(8)                                               /**< (PAC_INTFLAGD) ADC1 Position */
#define PAC_INTFLAGD_ADC1_Msk                 (_U_(0x1) << PAC_INTFLAGD_ADC1_Pos)                  /**< (PAC_INTFLAGD) ADC1 Mask */
#define PAC_INTFLAGD_ADC1(value)              (PAC_INTFLAGD_ADC1_Msk & ((value) << PAC_INTFLAGD_ADC1_Pos))
#define PAC_INTFLAGD_DAC_Pos                  _U_(9)                                               /**< (PAC_INTFLAGD) DAC Position */
#define PAC_INTFLAGD_DAC_Msk                  (_U_(0x1) << PAC_INTFLAGD_DAC_Pos)                   /**< (PAC_INTFLAGD) DAC Mask */
#define PAC_INTFLAGD_DAC(value)               (PAC_INTFLAGD_DAC_Msk & ((value) << PAC_INTFLAGD_DAC_Pos))
#define PAC_INTFLAGD_I2S_Pos                  _U_(10)                                              /**< (PAC_INTFLAGD) I2S Position */
#define PAC_INTFLAGD_I2S_Msk                  (_U_(0x1) << PAC_INTFLAGD_I2S_Pos)                   /**< (PAC_INTFLAGD) I2S Mask */
#define PAC_INTFLAGD_I2S(value)               (PAC_INTFLAGD_I2S_Msk & ((value) << PAC_INTFLAGD_I2S_Pos))
#define PAC_INTFLAGD_PCC_Pos                  _U_(11)                                              /**< (PAC_INTFLAGD) PCC Position */
#define PAC_INTFLAGD_PCC_Msk                  (_U_(0x1) << PAC_INTFLAGD_PCC_Pos)                   /**< (PAC_INTFLAGD) PCC Mask */
#define PAC_INTFLAGD_PCC(value)               (PAC_INTFLAGD_PCC_Msk & ((value) << PAC_INTFLAGD_PCC_Pos))
#define PAC_INTFLAGD_Msk                      _U_(0x00000FFF)                                      /**< (PAC_INTFLAGD) Register Mask  */

#define PAC_INTFLAGD_SERCOM_Pos               _U_(0)                                               /**< (PAC_INTFLAGD Position) SERCOM4 */
#define PAC_INTFLAGD_SERCOM_Msk               (_U_(0xF) << PAC_INTFLAGD_SERCOM_Pos)                /**< (PAC_INTFLAGD Mask) SERCOM */
#define PAC_INTFLAGD_SERCOM(value)            (PAC_INTFLAGD_SERCOM_Msk & ((value) << PAC_INTFLAGD_SERCOM_Pos)) 
#define PAC_INTFLAGD_TCC_Pos                  _U_(4)                                               /**< (PAC_INTFLAGD Position) TCC4 */
#define PAC_INTFLAGD_TCC_Msk                  (_U_(0x1) << PAC_INTFLAGD_TCC_Pos)                   /**< (PAC_INTFLAGD Mask) TCC */
#define PAC_INTFLAGD_TCC(value)               (PAC_INTFLAGD_TCC_Msk & ((value) << PAC_INTFLAGD_TCC_Pos)) 
#define PAC_INTFLAGD_TC_Pos                   _U_(5)                                               /**< (PAC_INTFLAGD Position) TC6 */
#define PAC_INTFLAGD_TC_Msk                   (_U_(0x3) << PAC_INTFLAGD_TC_Pos)                    /**< (PAC_INTFLAGD Mask) TC */
#define PAC_INTFLAGD_TC(value)                (PAC_INTFLAGD_TC_Msk & ((value) << PAC_INTFLAGD_TC_Pos)) 
#define PAC_INTFLAGD_ADC_Pos                  _U_(7)                                               /**< (PAC_INTFLAGD Position) ADCx */
#define PAC_INTFLAGD_ADC_Msk                  (_U_(0x3) << PAC_INTFLAGD_ADC_Pos)                   /**< (PAC_INTFLAGD Mask) ADC */
#define PAC_INTFLAGD_ADC(value)               (PAC_INTFLAGD_ADC_Msk & ((value) << PAC_INTFLAGD_ADC_Pos)) 

/* -------- PAC_STATUSA : (PAC Offset: 0x34) ( R/ 32) Peripheral write protection status - Bridge A -------- */
#define PAC_STATUSA_RESETVALUE                _U_(0x10000)                                         /**<  (PAC_STATUSA) Peripheral write protection status - Bridge A  Reset Value */

#define PAC_STATUSA_PAC_Pos                   _U_(0)                                               /**< (PAC_STATUSA) PAC APB Protect Enable Position */
#define PAC_STATUSA_PAC_Msk                   (_U_(0x1) << PAC_STATUSA_PAC_Pos)                    /**< (PAC_STATUSA) PAC APB Protect Enable Mask */
#define PAC_STATUSA_PAC(value)                (PAC_STATUSA_PAC_Msk & ((value) << PAC_STATUSA_PAC_Pos))
#define PAC_STATUSA_PM_Pos                    _U_(1)                                               /**< (PAC_STATUSA) PM APB Protect Enable Position */
#define PAC_STATUSA_PM_Msk                    (_U_(0x1) << PAC_STATUSA_PM_Pos)                     /**< (PAC_STATUSA) PM APB Protect Enable Mask */
#define PAC_STATUSA_PM(value)                 (PAC_STATUSA_PM_Msk & ((value) << PAC_STATUSA_PM_Pos))
#define PAC_STATUSA_MCLK_Pos                  _U_(2)                                               /**< (PAC_STATUSA) MCLK APB Protect Enable Position */
#define PAC_STATUSA_MCLK_Msk                  (_U_(0x1) << PAC_STATUSA_MCLK_Pos)                   /**< (PAC_STATUSA) MCLK APB Protect Enable Mask */
#define PAC_STATUSA_MCLK(value)               (PAC_STATUSA_MCLK_Msk & ((value) << PAC_STATUSA_MCLK_Pos))
#define PAC_STATUSA_RSTC_Pos                  _U_(3)                                               /**< (PAC_STATUSA) RSTC APB Protect Enable Position */
#define PAC_STATUSA_RSTC_Msk                  (_U_(0x1) << PAC_STATUSA_RSTC_Pos)                   /**< (PAC_STATUSA) RSTC APB Protect Enable Mask */
#define PAC_STATUSA_RSTC(value)               (PAC_STATUSA_RSTC_Msk & ((value) << PAC_STATUSA_RSTC_Pos))
#define PAC_STATUSA_OSCCTRL_Pos               _U_(4)                                               /**< (PAC_STATUSA) OSCCTRL APB Protect Enable Position */
#define PAC_STATUSA_OSCCTRL_Msk               (_U_(0x1) << PAC_STATUSA_OSCCTRL_Pos)                /**< (PAC_STATUSA) OSCCTRL APB Protect Enable Mask */
#define PAC_STATUSA_OSCCTRL(value)            (PAC_STATUSA_OSCCTRL_Msk & ((value) << PAC_STATUSA_OSCCTRL_Pos))
#define PAC_STATUSA_OSC32KCTRL_Pos            _U_(5)                                               /**< (PAC_STATUSA) OSC32KCTRL APB Protect Enable Position */
#define PAC_STATUSA_OSC32KCTRL_Msk            (_U_(0x1) << PAC_STATUSA_OSC32KCTRL_Pos)             /**< (PAC_STATUSA) OSC32KCTRL APB Protect Enable Mask */
#define PAC_STATUSA_OSC32KCTRL(value)         (PAC_STATUSA_OSC32KCTRL_Msk & ((value) << PAC_STATUSA_OSC32KCTRL_Pos))
#define PAC_STATUSA_SUPC_Pos                  _U_(6)                                               /**< (PAC_STATUSA) SUPC APB Protect Enable Position */
#define PAC_STATUSA_SUPC_Msk                  (_U_(0x1) << PAC_STATUSA_SUPC_Pos)                   /**< (PAC_STATUSA) SUPC APB Protect Enable Mask */
#define PAC_STATUSA_SUPC(value)               (PAC_STATUSA_SUPC_Msk & ((value) << PAC_STATUSA_SUPC_Pos))
#define PAC_STATUSA_GCLK_Pos                  _U_(7)                                               /**< (PAC_STATUSA) GCLK APB Protect Enable Position */
#define PAC_STATUSA_GCLK_Msk                  (_U_(0x1) << PAC_STATUSA_GCLK_Pos)                   /**< (PAC_STATUSA) GCLK APB Protect Enable Mask */
#define PAC_STATUSA_GCLK(value)               (PAC_STATUSA_GCLK_Msk & ((value) << PAC_STATUSA_GCLK_Pos))
#define PAC_STATUSA_WDT_Pos                   _U_(8)                                               /**< (PAC_STATUSA) WDT APB Protect Enable Position */
#define PAC_STATUSA_WDT_Msk                   (_U_(0x1) << PAC_STATUSA_WDT_Pos)                    /**< (PAC_STATUSA) WDT APB Protect Enable Mask */
#define PAC_STATUSA_WDT(value)                (PAC_STATUSA_WDT_Msk & ((value) << PAC_STATUSA_WDT_Pos))
#define PAC_STATUSA_RTC_Pos                   _U_(9)                                               /**< (PAC_STATUSA) RTC APB Protect Enable Position */
#define PAC_STATUSA_RTC_Msk                   (_U_(0x1) << PAC_STATUSA_RTC_Pos)                    /**< (PAC_STATUSA) RTC APB Protect Enable Mask */
#define PAC_STATUSA_RTC(value)                (PAC_STATUSA_RTC_Msk & ((value) << PAC_STATUSA_RTC_Pos))
#define PAC_STATUSA_EIC_Pos                   _U_(10)                                              /**< (PAC_STATUSA) EIC APB Protect Enable Position */
#define PAC_STATUSA_EIC_Msk                   (_U_(0x1) << PAC_STATUSA_EIC_Pos)                    /**< (PAC_STATUSA) EIC APB Protect Enable Mask */
#define PAC_STATUSA_EIC(value)                (PAC_STATUSA_EIC_Msk & ((value) << PAC_STATUSA_EIC_Pos))
#define PAC_STATUSA_FREQM_Pos                 _U_(11)                                              /**< (PAC_STATUSA) FREQM APB Protect Enable Position */
#define PAC_STATUSA_FREQM_Msk                 (_U_(0x1) << PAC_STATUSA_FREQM_Pos)                  /**< (PAC_STATUSA) FREQM APB Protect Enable Mask */
#define PAC_STATUSA_FREQM(value)              (PAC_STATUSA_FREQM_Msk & ((value) << PAC_STATUSA_FREQM_Pos))
#define PAC_STATUSA_SERCOM0_Pos               _U_(12)                                              /**< (PAC_STATUSA) SERCOM0 APB Protect Enable Position */
#define PAC_STATUSA_SERCOM0_Msk               (_U_(0x1) << PAC_STATUSA_SERCOM0_Pos)                /**< (PAC_STATUSA) SERCOM0 APB Protect Enable Mask */
#define PAC_STATUSA_SERCOM0(value)            (PAC_STATUSA_SERCOM0_Msk & ((value) << PAC_STATUSA_SERCOM0_Pos))
#define PAC_STATUSA_SERCOM1_Pos               _U_(13)                                              /**< (PAC_STATUSA) SERCOM1 APB Protect Enable Position */
#define PAC_STATUSA_SERCOM1_Msk               (_U_(0x1) << PAC_STATUSA_SERCOM1_Pos)                /**< (PAC_STATUSA) SERCOM1 APB Protect Enable Mask */
#define PAC_STATUSA_SERCOM1(value)            (PAC_STATUSA_SERCOM1_Msk & ((value) << PAC_STATUSA_SERCOM1_Pos))
#define PAC_STATUSA_TC0_Pos                   _U_(14)                                              /**< (PAC_STATUSA) TC0 APB Protect Enable Position */
#define PAC_STATUSA_TC0_Msk                   (_U_(0x1) << PAC_STATUSA_TC0_Pos)                    /**< (PAC_STATUSA) TC0 APB Protect Enable Mask */
#define PAC_STATUSA_TC0(value)                (PAC_STATUSA_TC0_Msk & ((value) << PAC_STATUSA_TC0_Pos))
#define PAC_STATUSA_TC1_Pos                   _U_(15)                                              /**< (PAC_STATUSA) TC1 APB Protect Enable Position */
#define PAC_STATUSA_TC1_Msk                   (_U_(0x1) << PAC_STATUSA_TC1_Pos)                    /**< (PAC_STATUSA) TC1 APB Protect Enable Mask */
#define PAC_STATUSA_TC1(value)                (PAC_STATUSA_TC1_Msk & ((value) << PAC_STATUSA_TC1_Pos))
#define PAC_STATUSA_Msk                       _U_(0x0000FFFF)                                      /**< (PAC_STATUSA) Register Mask  */

#define PAC_STATUSA_SERCOM_Pos                _U_(12)                                              /**< (PAC_STATUSA Position) SERCOMx APB Protect Enable */
#define PAC_STATUSA_SERCOM_Msk                (_U_(0x3) << PAC_STATUSA_SERCOM_Pos)                 /**< (PAC_STATUSA Mask) SERCOM */
#define PAC_STATUSA_SERCOM(value)             (PAC_STATUSA_SERCOM_Msk & ((value) << PAC_STATUSA_SERCOM_Pos)) 
#define PAC_STATUSA_TC_Pos                    _U_(14)                                              /**< (PAC_STATUSA Position) TCx APB Protect Enable */
#define PAC_STATUSA_TC_Msk                    (_U_(0x3) << PAC_STATUSA_TC_Pos)                     /**< (PAC_STATUSA Mask) TC */
#define PAC_STATUSA_TC(value)                 (PAC_STATUSA_TC_Msk & ((value) << PAC_STATUSA_TC_Pos)) 

/* -------- PAC_STATUSB : (PAC Offset: 0x38) ( R/ 32) Peripheral write protection status - Bridge B -------- */
#define PAC_STATUSB_RESETVALUE                _U_(0x02)                                            /**<  (PAC_STATUSB) Peripheral write protection status - Bridge B  Reset Value */

#define PAC_STATUSB_USB_Pos                   _U_(0)                                               /**< (PAC_STATUSB) USB APB Protect Enable Position */
#define PAC_STATUSB_USB_Msk                   (_U_(0x1) << PAC_STATUSB_USB_Pos)                    /**< (PAC_STATUSB) USB APB Protect Enable Mask */
#define PAC_STATUSB_USB(value)                (PAC_STATUSB_USB_Msk & ((value) << PAC_STATUSB_USB_Pos))
#define PAC_STATUSB_DSU_Pos                   _U_(1)                                               /**< (PAC_STATUSB) DSU APB Protect Enable Position */
#define PAC_STATUSB_DSU_Msk                   (_U_(0x1) << PAC_STATUSB_DSU_Pos)                    /**< (PAC_STATUSB) DSU APB Protect Enable Mask */
#define PAC_STATUSB_DSU(value)                (PAC_STATUSB_DSU_Msk & ((value) << PAC_STATUSB_DSU_Pos))
#define PAC_STATUSB_NVMCTRL_Pos               _U_(2)                                               /**< (PAC_STATUSB) NVMCTRL APB Protect Enable Position */
#define PAC_STATUSB_NVMCTRL_Msk               (_U_(0x1) << PAC_STATUSB_NVMCTRL_Pos)                /**< (PAC_STATUSB) NVMCTRL APB Protect Enable Mask */
#define PAC_STATUSB_NVMCTRL(value)            (PAC_STATUSB_NVMCTRL_Msk & ((value) << PAC_STATUSB_NVMCTRL_Pos))
#define PAC_STATUSB_CMCC_Pos                  _U_(3)                                               /**< (PAC_STATUSB) CMCC APB Protect Enable Position */
#define PAC_STATUSB_CMCC_Msk                  (_U_(0x1) << PAC_STATUSB_CMCC_Pos)                   /**< (PAC_STATUSB) CMCC APB Protect Enable Mask */
#define PAC_STATUSB_CMCC(value)               (PAC_STATUSB_CMCC_Msk & ((value) << PAC_STATUSB_CMCC_Pos))
#define PAC_STATUSB_PORT_Pos                  _U_(4)                                               /**< (PAC_STATUSB) PORT APB Protect Enable Position */
#define PAC_STATUSB_PORT_Msk                  (_U_(0x1) << PAC_STATUSB_PORT_Pos)                   /**< (PAC_STATUSB) PORT APB Protect Enable Mask */
#define PAC_STATUSB_PORT(value)               (PAC_STATUSB_PORT_Msk & ((value) << PAC_STATUSB_PORT_Pos))
#define PAC_STATUSB_DMAC_Pos                  _U_(5)                                               /**< (PAC_STATUSB) DMAC APB Protect Enable Position */
#define PAC_STATUSB_DMAC_Msk                  (_U_(0x1) << PAC_STATUSB_DMAC_Pos)                   /**< (PAC_STATUSB) DMAC APB Protect Enable Mask */
#define PAC_STATUSB_DMAC(value)               (PAC_STATUSB_DMAC_Msk & ((value) << PAC_STATUSB_DMAC_Pos))
#define PAC_STATUSB_HMATRIX_Pos               _U_(6)                                               /**< (PAC_STATUSB) HMATRIX APB Protect Enable Position */
#define PAC_STATUSB_HMATRIX_Msk               (_U_(0x1) << PAC_STATUSB_HMATRIX_Pos)                /**< (PAC_STATUSB) HMATRIX APB Protect Enable Mask */
#define PAC_STATUSB_HMATRIX(value)            (PAC_STATUSB_HMATRIX_Msk & ((value) << PAC_STATUSB_HMATRIX_Pos))
#define PAC_STATUSB_EVSYS_Pos                 _U_(7)                                               /**< (PAC_STATUSB) EVSYS APB Protect Enable Position */
#define PAC_STATUSB_EVSYS_Msk                 (_U_(0x1) << PAC_STATUSB_EVSYS_Pos)                  /**< (PAC_STATUSB) EVSYS APB Protect Enable Mask */
#define PAC_STATUSB_EVSYS(value)              (PAC_STATUSB_EVSYS_Msk & ((value) << PAC_STATUSB_EVSYS_Pos))
#define PAC_STATUSB_SERCOM2_Pos               _U_(9)                                               /**< (PAC_STATUSB) SERCOM2 APB Protect Enable Position */
#define PAC_STATUSB_SERCOM2_Msk               (_U_(0x1) << PAC_STATUSB_SERCOM2_Pos)                /**< (PAC_STATUSB) SERCOM2 APB Protect Enable Mask */
#define PAC_STATUSB_SERCOM2(value)            (PAC_STATUSB_SERCOM2_Msk & ((value) << PAC_STATUSB_SERCOM2_Pos))
#define PAC_STATUSB_SERCOM3_Pos               _U_(10)                                              /**< (PAC_STATUSB) SERCOM3 APB Protect Enable Position */
#define PAC_STATUSB_SERCOM3_Msk               (_U_(0x1) << PAC_STATUSB_SERCOM3_Pos)                /**< (PAC_STATUSB) SERCOM3 APB Protect Enable Mask */
#define PAC_STATUSB_SERCOM3(value)            (PAC_STATUSB_SERCOM3_Msk & ((value) << PAC_STATUSB_SERCOM3_Pos))
#define PAC_STATUSB_TCC0_Pos                  _U_(11)                                              /**< (PAC_STATUSB) TCC0 APB Protect Enable Position */
#define PAC_STATUSB_TCC0_Msk                  (_U_(0x1) << PAC_STATUSB_TCC0_Pos)                   /**< (PAC_STATUSB) TCC0 APB Protect Enable Mask */
#define PAC_STATUSB_TCC0(value)               (PAC_STATUSB_TCC0_Msk & ((value) << PAC_STATUSB_TCC0_Pos))
#define PAC_STATUSB_TCC1_Pos                  _U_(12)                                              /**< (PAC_STATUSB) TCC1 APB Protect Enable Position */
#define PAC_STATUSB_TCC1_Msk                  (_U_(0x1) << PAC_STATUSB_TCC1_Pos)                   /**< (PAC_STATUSB) TCC1 APB Protect Enable Mask */
#define PAC_STATUSB_TCC1(value)               (PAC_STATUSB_TCC1_Msk & ((value) << PAC_STATUSB_TCC1_Pos))
#define PAC_STATUSB_TC2_Pos                   _U_(13)                                              /**< (PAC_STATUSB) TC2 APB Protect Enable Position */
#define PAC_STATUSB_TC2_Msk                   (_U_(0x1) << PAC_STATUSB_TC2_Pos)                    /**< (PAC_STATUSB) TC2 APB Protect Enable Mask */
#define PAC_STATUSB_TC2(value)                (PAC_STATUSB_TC2_Msk & ((value) << PAC_STATUSB_TC2_Pos))
#define PAC_STATUSB_TC3_Pos                   _U_(14)                                              /**< (PAC_STATUSB) TC3 APB Protect Enable Position */
#define PAC_STATUSB_TC3_Msk                   (_U_(0x1) << PAC_STATUSB_TC3_Pos)                    /**< (PAC_STATUSB) TC3 APB Protect Enable Mask */
#define PAC_STATUSB_TC3(value)                (PAC_STATUSB_TC3_Msk & ((value) << PAC_STATUSB_TC3_Pos))
#define PAC_STATUSB_RAMECC_Pos                _U_(16)                                              /**< (PAC_STATUSB) RAMECC APB Protect Enable Position */
#define PAC_STATUSB_RAMECC_Msk                (_U_(0x1) << PAC_STATUSB_RAMECC_Pos)                 /**< (PAC_STATUSB) RAMECC APB Protect Enable Mask */
#define PAC_STATUSB_RAMECC(value)             (PAC_STATUSB_RAMECC_Msk & ((value) << PAC_STATUSB_RAMECC_Pos))
#define PAC_STATUSB_Msk                       _U_(0x00017EFF)                                      /**< (PAC_STATUSB) Register Mask  */

#define PAC_STATUSB_SERCOM_Pos                _U_(9)                                               /**< (PAC_STATUSB Position) SERCOM2 APB Protect Enable */
#define PAC_STATUSB_SERCOM_Msk                (_U_(0x3) << PAC_STATUSB_SERCOM_Pos)                 /**< (PAC_STATUSB Mask) SERCOM */
#define PAC_STATUSB_SERCOM(value)             (PAC_STATUSB_SERCOM_Msk & ((value) << PAC_STATUSB_SERCOM_Pos)) 
#define PAC_STATUSB_TCC_Pos                   _U_(11)                                              /**< (PAC_STATUSB Position) TCCx APB Protect Enable */
#define PAC_STATUSB_TCC_Msk                   (_U_(0x3) << PAC_STATUSB_TCC_Pos)                    /**< (PAC_STATUSB Mask) TCC */
#define PAC_STATUSB_TCC(value)                (PAC_STATUSB_TCC_Msk & ((value) << PAC_STATUSB_TCC_Pos)) 
#define PAC_STATUSB_TC_Pos                    _U_(13)                                              /**< (PAC_STATUSB Position) TC2 APB Protect Enable */
#define PAC_STATUSB_TC_Msk                    (_U_(0x3) << PAC_STATUSB_TC_Pos)                     /**< (PAC_STATUSB Mask) TC */
#define PAC_STATUSB_TC(value)                 (PAC_STATUSB_TC_Msk & ((value) << PAC_STATUSB_TC_Pos)) 

/* -------- PAC_STATUSC : (PAC Offset: 0x3C) ( R/ 32) Peripheral write protection status - Bridge C -------- */
#define PAC_STATUSC_RESETVALUE                _U_(0x00)                                            /**<  (PAC_STATUSC) Peripheral write protection status - Bridge C  Reset Value */

#define PAC_STATUSC_CAN0_Pos                  _U_(0)                                               /**< (PAC_STATUSC) CAN0 APB Protect Enable Position */
#define PAC_STATUSC_CAN0_Msk                  (_U_(0x1) << PAC_STATUSC_CAN0_Pos)                   /**< (PAC_STATUSC) CAN0 APB Protect Enable Mask */
#define PAC_STATUSC_CAN0(value)               (PAC_STATUSC_CAN0_Msk & ((value) << PAC_STATUSC_CAN0_Pos))
#define PAC_STATUSC_CAN1_Pos                  _U_(1)                                               /**< (PAC_STATUSC) CAN1 APB Protect Enable Position */
#define PAC_STATUSC_CAN1_Msk                  (_U_(0x1) << PAC_STATUSC_CAN1_Pos)                   /**< (PAC_STATUSC) CAN1 APB Protect Enable Mask */
#define PAC_STATUSC_CAN1(value)               (PAC_STATUSC_CAN1_Msk & ((value) << PAC_STATUSC_CAN1_Pos))
#define PAC_STATUSC_GMAC_Pos                  _U_(2)                                               /**< (PAC_STATUSC) GMAC APB Protect Enable Position */
#define PAC_STATUSC_GMAC_Msk                  (_U_(0x1) << PAC_STATUSC_GMAC_Pos)                   /**< (PAC_STATUSC) GMAC APB Protect Enable Mask */
#define PAC_STATUSC_GMAC(value)               (PAC_STATUSC_GMAC_Msk & ((value) << PAC_STATUSC_GMAC_Pos))
#define PAC_STATUSC_TCC2_Pos                  _U_(3)                                               /**< (PAC_STATUSC) TCC2 APB Protect Enable Position */
#define PAC_STATUSC_TCC2_Msk                  (_U_(0x1) << PAC_STATUSC_TCC2_Pos)                   /**< (PAC_STATUSC) TCC2 APB Protect Enable Mask */
#define PAC_STATUSC_TCC2(value)               (PAC_STATUSC_TCC2_Msk & ((value) << PAC_STATUSC_TCC2_Pos))
#define PAC_STATUSC_TCC3_Pos                  _U_(4)                                               /**< (PAC_STATUSC) TCC3 APB Protect Enable Position */
#define PAC_STATUSC_TCC3_Msk                  (_U_(0x1) << PAC_STATUSC_TCC3_Pos)                   /**< (PAC_STATUSC) TCC3 APB Protect Enable Mask */
#define PAC_STATUSC_TCC3(value)               (PAC_STATUSC_TCC3_Msk & ((value) << PAC_STATUSC_TCC3_Pos))
#define PAC_STATUSC_TC4_Pos                   _U_(5)                                               /**< (PAC_STATUSC) TC4 APB Protect Enable Position */
#define PAC_STATUSC_TC4_Msk                   (_U_(0x1) << PAC_STATUSC_TC4_Pos)                    /**< (PAC_STATUSC) TC4 APB Protect Enable Mask */
#define PAC_STATUSC_TC4(value)                (PAC_STATUSC_TC4_Msk & ((value) << PAC_STATUSC_TC4_Pos))
#define PAC_STATUSC_TC5_Pos                   _U_(6)                                               /**< (PAC_STATUSC) TC5 APB Protect Enable Position */
#define PAC_STATUSC_TC5_Msk                   (_U_(0x1) << PAC_STATUSC_TC5_Pos)                    /**< (PAC_STATUSC) TC5 APB Protect Enable Mask */
#define PAC_STATUSC_TC5(value)                (PAC_STATUSC_TC5_Msk & ((value) << PAC_STATUSC_TC5_Pos))
#define PAC_STATUSC_PDEC_Pos                  _U_(7)                                               /**< (PAC_STATUSC) PDEC APB Protect Enable Position */
#define PAC_STATUSC_PDEC_Msk                  (_U_(0x1) << PAC_STATUSC_PDEC_Pos)                   /**< (PAC_STATUSC) PDEC APB Protect Enable Mask */
#define PAC_STATUSC_PDEC(value)               (PAC_STATUSC_PDEC_Msk & ((value) << PAC_STATUSC_PDEC_Pos))
#define PAC_STATUSC_AC_Pos                    _U_(8)                                               /**< (PAC_STATUSC) AC APB Protect Enable Position */
#define PAC_STATUSC_AC_Msk                    (_U_(0x1) << PAC_STATUSC_AC_Pos)                     /**< (PAC_STATUSC) AC APB Protect Enable Mask */
#define PAC_STATUSC_AC(value)                 (PAC_STATUSC_AC_Msk & ((value) << PAC_STATUSC_AC_Pos))
#define PAC_STATUSC_AES_Pos                   _U_(9)                                               /**< (PAC_STATUSC) AES APB Protect Enable Position */
#define PAC_STATUSC_AES_Msk                   (_U_(0x1) << PAC_STATUSC_AES_Pos)                    /**< (PAC_STATUSC) AES APB Protect Enable Mask */
#define PAC_STATUSC_AES(value)                (PAC_STATUSC_AES_Msk & ((value) << PAC_STATUSC_AES_Pos))
#define PAC_STATUSC_TRNG_Pos                  _U_(10)                                              /**< (PAC_STATUSC) TRNG APB Protect Enable Position */
#define PAC_STATUSC_TRNG_Msk                  (_U_(0x1) << PAC_STATUSC_TRNG_Pos)                   /**< (PAC_STATUSC) TRNG APB Protect Enable Mask */
#define PAC_STATUSC_TRNG(value)               (PAC_STATUSC_TRNG_Msk & ((value) << PAC_STATUSC_TRNG_Pos))
#define PAC_STATUSC_ICM_Pos                   _U_(11)                                              /**< (PAC_STATUSC) ICM APB Protect Enable Position */
#define PAC_STATUSC_ICM_Msk                   (_U_(0x1) << PAC_STATUSC_ICM_Pos)                    /**< (PAC_STATUSC) ICM APB Protect Enable Mask */
#define PAC_STATUSC_ICM(value)                (PAC_STATUSC_ICM_Msk & ((value) << PAC_STATUSC_ICM_Pos))
#define PAC_STATUSC_PUKCC_Pos                 _U_(12)                                              /**< (PAC_STATUSC) PUKCC APB Protect Enable Position */
#define PAC_STATUSC_PUKCC_Msk                 (_U_(0x1) << PAC_STATUSC_PUKCC_Pos)                  /**< (PAC_STATUSC) PUKCC APB Protect Enable Mask */
#define PAC_STATUSC_PUKCC(value)              (PAC_STATUSC_PUKCC_Msk & ((value) << PAC_STATUSC_PUKCC_Pos))
#define PAC_STATUSC_QSPI_Pos                  _U_(13)                                              /**< (PAC_STATUSC) QSPI APB Protect Enable Position */
#define PAC_STATUSC_QSPI_Msk                  (_U_(0x1) << PAC_STATUSC_QSPI_Pos)                   /**< (PAC_STATUSC) QSPI APB Protect Enable Mask */
#define PAC_STATUSC_QSPI(value)               (PAC_STATUSC_QSPI_Msk & ((value) << PAC_STATUSC_QSPI_Pos))
#define PAC_STATUSC_CCL_Pos                   _U_(14)                                              /**< (PAC_STATUSC) CCL APB Protect Enable Position */
#define PAC_STATUSC_CCL_Msk                   (_U_(0x1) << PAC_STATUSC_CCL_Pos)                    /**< (PAC_STATUSC) CCL APB Protect Enable Mask */
#define PAC_STATUSC_CCL(value)                (PAC_STATUSC_CCL_Msk & ((value) << PAC_STATUSC_CCL_Pos))
#define PAC_STATUSC_Msk                       _U_(0x00007FFF)                                      /**< (PAC_STATUSC) Register Mask  */

#define PAC_STATUSC_CAN_Pos                   _U_(0)                                               /**< (PAC_STATUSC Position) CANx APB Protect Enable */
#define PAC_STATUSC_CAN_Msk                   (_U_(0x3) << PAC_STATUSC_CAN_Pos)                    /**< (PAC_STATUSC Mask) CAN */
#define PAC_STATUSC_CAN(value)                (PAC_STATUSC_CAN_Msk & ((value) << PAC_STATUSC_CAN_Pos)) 
#define PAC_STATUSC_TCC_Pos                   _U_(3)                                               /**< (PAC_STATUSC Position) TCC2 APB Protect Enable */
#define PAC_STATUSC_TCC_Msk                   (_U_(0x3) << PAC_STATUSC_TCC_Pos)                    /**< (PAC_STATUSC Mask) TCC */
#define PAC_STATUSC_TCC(value)                (PAC_STATUSC_TCC_Msk & ((value) << PAC_STATUSC_TCC_Pos)) 
#define PAC_STATUSC_TC_Pos                    _U_(5)                                               /**< (PAC_STATUSC Position) TC4 APB Protect Enable */
#define PAC_STATUSC_TC_Msk                    (_U_(0x3) << PAC_STATUSC_TC_Pos)                     /**< (PAC_STATUSC Mask) TC */
#define PAC_STATUSC_TC(value)                 (PAC_STATUSC_TC_Msk & ((value) << PAC_STATUSC_TC_Pos)) 

/* -------- PAC_STATUSD : (PAC Offset: 0x40) ( R/ 32) Peripheral write protection status - Bridge D -------- */
#define PAC_STATUSD_RESETVALUE                _U_(0x00)                                            /**<  (PAC_STATUSD) Peripheral write protection status - Bridge D  Reset Value */

#define PAC_STATUSD_SERCOM4_Pos               _U_(0)                                               /**< (PAC_STATUSD) SERCOM4 APB Protect Enable Position */
#define PAC_STATUSD_SERCOM4_Msk               (_U_(0x1) << PAC_STATUSD_SERCOM4_Pos)                /**< (PAC_STATUSD) SERCOM4 APB Protect Enable Mask */
#define PAC_STATUSD_SERCOM4(value)            (PAC_STATUSD_SERCOM4_Msk & ((value) << PAC_STATUSD_SERCOM4_Pos))
#define PAC_STATUSD_SERCOM5_Pos               _U_(1)                                               /**< (PAC_STATUSD) SERCOM5 APB Protect Enable Position */
#define PAC_STATUSD_SERCOM5_Msk               (_U_(0x1) << PAC_STATUSD_SERCOM5_Pos)                /**< (PAC_STATUSD) SERCOM5 APB Protect Enable Mask */
#define PAC_STATUSD_SERCOM5(value)            (PAC_STATUSD_SERCOM5_Msk & ((value) << PAC_STATUSD_SERCOM5_Pos))
#define PAC_STATUSD_SERCOM6_Pos               _U_(2)                                               /**< (PAC_STATUSD) SERCOM6 APB Protect Enable Position */
#define PAC_STATUSD_SERCOM6_Msk               (_U_(0x1) << PAC_STATUSD_SERCOM6_Pos)                /**< (PAC_STATUSD) SERCOM6 APB Protect Enable Mask */
#define PAC_STATUSD_SERCOM6(value)            (PAC_STATUSD_SERCOM6_Msk & ((value) << PAC_STATUSD_SERCOM6_Pos))
#define PAC_STATUSD_SERCOM7_Pos               _U_(3)                                               /**< (PAC_STATUSD) SERCOM7 APB Protect Enable Position */
#define PAC_STATUSD_SERCOM7_Msk               (_U_(0x1) << PAC_STATUSD_SERCOM7_Pos)                /**< (PAC_STATUSD) SERCOM7 APB Protect Enable Mask */
#define PAC_STATUSD_SERCOM7(value)            (PAC_STATUSD_SERCOM7_Msk & ((value) << PAC_STATUSD_SERCOM7_Pos))
#define PAC_STATUSD_TCC4_Pos                  _U_(4)                                               /**< (PAC_STATUSD) TCC4 APB Protect Enable Position */
#define PAC_STATUSD_TCC4_Msk                  (_U_(0x1) << PAC_STATUSD_TCC4_Pos)                   /**< (PAC_STATUSD) TCC4 APB Protect Enable Mask */
#define PAC_STATUSD_TCC4(value)               (PAC_STATUSD_TCC4_Msk & ((value) << PAC_STATUSD_TCC4_Pos))
#define PAC_STATUSD_TC6_Pos                   _U_(5)                                               /**< (PAC_STATUSD) TC6 APB Protect Enable Position */
#define PAC_STATUSD_TC6_Msk                   (_U_(0x1) << PAC_STATUSD_TC6_Pos)                    /**< (PAC_STATUSD) TC6 APB Protect Enable Mask */
#define PAC_STATUSD_TC6(value)                (PAC_STATUSD_TC6_Msk & ((value) << PAC_STATUSD_TC6_Pos))
#define PAC_STATUSD_TC7_Pos                   _U_(6)                                               /**< (PAC_STATUSD) TC7 APB Protect Enable Position */
#define PAC_STATUSD_TC7_Msk                   (_U_(0x1) << PAC_STATUSD_TC7_Pos)                    /**< (PAC_STATUSD) TC7 APB Protect Enable Mask */
#define PAC_STATUSD_TC7(value)                (PAC_STATUSD_TC7_Msk & ((value) << PAC_STATUSD_TC7_Pos))
#define PAC_STATUSD_ADC0_Pos                  _U_(7)                                               /**< (PAC_STATUSD) ADC0 APB Protect Enable Position */
#define PAC_STATUSD_ADC0_Msk                  (_U_(0x1) << PAC_STATUSD_ADC0_Pos)                   /**< (PAC_STATUSD) ADC0 APB Protect Enable Mask */
#define PAC_STATUSD_ADC0(value)               (PAC_STATUSD_ADC0_Msk & ((value) << PAC_STATUSD_ADC0_Pos))
#define PAC_STATUSD_ADC1_Pos                  _U_(8)                                               /**< (PAC_STATUSD) ADC1 APB Protect Enable Position */
#define PAC_STATUSD_ADC1_Msk                  (_U_(0x1) << PAC_STATUSD_ADC1_Pos)                   /**< (PAC_STATUSD) ADC1 APB Protect Enable Mask */
#define PAC_STATUSD_ADC1(value)               (PAC_STATUSD_ADC1_Msk & ((value) << PAC_STATUSD_ADC1_Pos))
#define PAC_STATUSD_DAC_Pos                   _U_(9)                                               /**< (PAC_STATUSD) DAC APB Protect Enable Position */
#define PAC_STATUSD_DAC_Msk                   (_U_(0x1) << PAC_STATUSD_DAC_Pos)                    /**< (PAC_STATUSD) DAC APB Protect Enable Mask */
#define PAC_STATUSD_DAC(value)                (PAC_STATUSD_DAC_Msk & ((value) << PAC_STATUSD_DAC_Pos))
#define PAC_STATUSD_I2S_Pos                   _U_(10)                                              /**< (PAC_STATUSD) I2S APB Protect Enable Position */
#define PAC_STATUSD_I2S_Msk                   (_U_(0x1) << PAC_STATUSD_I2S_Pos)                    /**< (PAC_STATUSD) I2S APB Protect Enable Mask */
#define PAC_STATUSD_I2S(value)                (PAC_STATUSD_I2S_Msk & ((value) << PAC_STATUSD_I2S_Pos))
#define PAC_STATUSD_PCC_Pos                   _U_(11)                                              /**< (PAC_STATUSD) PCC APB Protect Enable Position */
#define PAC_STATUSD_PCC_Msk                   (_U_(0x1) << PAC_STATUSD_PCC_Pos)                    /**< (PAC_STATUSD) PCC APB Protect Enable Mask */
#define PAC_STATUSD_PCC(value)                (PAC_STATUSD_PCC_Msk & ((value) << PAC_STATUSD_PCC_Pos))
#define PAC_STATUSD_Msk                       _U_(0x00000FFF)                                      /**< (PAC_STATUSD) Register Mask  */

#define PAC_STATUSD_SERCOM_Pos                _U_(0)                                               /**< (PAC_STATUSD Position) SERCOM4 APB Protect Enable */
#define PAC_STATUSD_SERCOM_Msk                (_U_(0xF) << PAC_STATUSD_SERCOM_Pos)                 /**< (PAC_STATUSD Mask) SERCOM */
#define PAC_STATUSD_SERCOM(value)             (PAC_STATUSD_SERCOM_Msk & ((value) << PAC_STATUSD_SERCOM_Pos)) 
#define PAC_STATUSD_TCC_Pos                   _U_(4)                                               /**< (PAC_STATUSD Position) TCC4 APB Protect Enable */
#define PAC_STATUSD_TCC_Msk                   (_U_(0x1) << PAC_STATUSD_TCC_Pos)                    /**< (PAC_STATUSD Mask) TCC */
#define PAC_STATUSD_TCC(value)                (PAC_STATUSD_TCC_Msk & ((value) << PAC_STATUSD_TCC_Pos)) 
#define PAC_STATUSD_TC_Pos                    _U_(5)                                               /**< (PAC_STATUSD Position) TC6 APB Protect Enable */
#define PAC_STATUSD_TC_Msk                    (_U_(0x3) << PAC_STATUSD_TC_Pos)                     /**< (PAC_STATUSD Mask) TC */
#define PAC_STATUSD_TC(value)                 (PAC_STATUSD_TC_Msk & ((value) << PAC_STATUSD_TC_Pos)) 
#define PAC_STATUSD_ADC_Pos                   _U_(7)                                               /**< (PAC_STATUSD Position) ADCx APB Protect Enable */
#define PAC_STATUSD_ADC_Msk                   (_U_(0x3) << PAC_STATUSD_ADC_Pos)                    /**< (PAC_STATUSD Mask) ADC */
#define PAC_STATUSD_ADC(value)                (PAC_STATUSD_ADC_Msk & ((value) << PAC_STATUSD_ADC_Pos)) 

/** \brief PAC register offsets definitions */
#define PAC_WRCTRL_REG_OFST            (0x00)              /**< (PAC_WRCTRL) Write control Offset */
#define PAC_EVCTRL_REG_OFST            (0x04)              /**< (PAC_EVCTRL) Event control Offset */
#define PAC_INTENCLR_REG_OFST          (0x08)              /**< (PAC_INTENCLR) Interrupt enable clear Offset */
#define PAC_INTENSET_REG_OFST          (0x09)              /**< (PAC_INTENSET) Interrupt enable set Offset */
#define PAC_INTFLAGAHB_REG_OFST        (0x10)              /**< (PAC_INTFLAGAHB) Bridge interrupt flag status Offset */
#define PAC_INTFLAGA_REG_OFST          (0x14)              /**< (PAC_INTFLAGA) Peripheral interrupt flag status - Bridge A Offset */
#define PAC_INTFLAGB_REG_OFST          (0x18)              /**< (PAC_INTFLAGB) Peripheral interrupt flag status - Bridge B Offset */
#define PAC_INTFLAGC_REG_OFST          (0x1C)              /**< (PAC_INTFLAGC) Peripheral interrupt flag status - Bridge C Offset */
#define PAC_INTFLAGD_REG_OFST          (0x20)              /**< (PAC_INTFLAGD) Peripheral interrupt flag status - Bridge D Offset */
#define PAC_STATUSA_REG_OFST           (0x34)              /**< (PAC_STATUSA) Peripheral write protection status - Bridge A Offset */
#define PAC_STATUSB_REG_OFST           (0x38)              /**< (PAC_STATUSB) Peripheral write protection status - Bridge B Offset */
#define PAC_STATUSC_REG_OFST           (0x3C)              /**< (PAC_STATUSC) Peripheral write protection status - Bridge C Offset */
#define PAC_STATUSD_REG_OFST           (0x40)              /**< (PAC_STATUSD) Peripheral write protection status - Bridge D Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief PAC register API structure */
typedef struct
{  /* Peripheral Access Controller */
  __IO  uint32_t                       PAC_WRCTRL;         /**< Offset: 0x00 (R/W  32) Write control */
  __IO  uint8_t                        PAC_EVCTRL;         /**< Offset: 0x04 (R/W  8) Event control */
  __I   uint8_t                        Reserved1[0x03];
  __IO  uint8_t                        PAC_INTENCLR;       /**< Offset: 0x08 (R/W  8) Interrupt enable clear */
  __IO  uint8_t                        PAC_INTENSET;       /**< Offset: 0x09 (R/W  8) Interrupt enable set */
  __I   uint8_t                        Reserved2[0x06];
  __IO  uint32_t                       PAC_INTFLAGAHB;     /**< Offset: 0x10 (R/W  32) Bridge interrupt flag status */
  __IO  uint32_t                       PAC_INTFLAGA;       /**< Offset: 0x14 (R/W  32) Peripheral interrupt flag status - Bridge A */
  __IO  uint32_t                       PAC_INTFLAGB;       /**< Offset: 0x18 (R/W  32) Peripheral interrupt flag status - Bridge B */
  __IO  uint32_t                       PAC_INTFLAGC;       /**< Offset: 0x1C (R/W  32) Peripheral interrupt flag status - Bridge C */
  __IO  uint32_t                       PAC_INTFLAGD;       /**< Offset: 0x20 (R/W  32) Peripheral interrupt flag status - Bridge D */
  __I   uint8_t                        Reserved3[0x10];
  __I   uint32_t                       PAC_STATUSA;        /**< Offset: 0x34 (R/   32) Peripheral write protection status - Bridge A */
  __I   uint32_t                       PAC_STATUSB;        /**< Offset: 0x38 (R/   32) Peripheral write protection status - Bridge B */
  __I   uint32_t                       PAC_STATUSC;        /**< Offset: 0x3C (R/   32) Peripheral write protection status - Bridge C */
  __I   uint32_t                       PAC_STATUSD;        /**< Offset: 0x40 (R/   32) Peripheral write protection status - Bridge D */
} pac_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_PAC_COMPONENT_H_ */
