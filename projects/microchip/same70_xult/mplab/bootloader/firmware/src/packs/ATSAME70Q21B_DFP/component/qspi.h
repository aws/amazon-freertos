/**
 * \brief Component description for QSPI
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

/* file generated from device description version 2019-07-24T15:04:36Z */
#ifndef _SAME70_QSPI_COMPONENT_H_
#define _SAME70_QSPI_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR QSPI                                         */
/* ************************************************************************** */

/* -------- QSPI_CR : (QSPI Offset: 0x00) ( /W 32) Control Register -------- */
#define QSPI_CR_QSPIEN_Pos                    _U_(0)                                               /**< (QSPI_CR) QSPI Enable Position */
#define QSPI_CR_QSPIEN_Msk                    (_U_(0x1) << QSPI_CR_QSPIEN_Pos)                     /**< (QSPI_CR) QSPI Enable Mask */
#define QSPI_CR_QSPIEN(value)                 (QSPI_CR_QSPIEN_Msk & ((value) << QSPI_CR_QSPIEN_Pos))
#define QSPI_CR_QSPIDIS_Pos                   _U_(1)                                               /**< (QSPI_CR) QSPI Disable Position */
#define QSPI_CR_QSPIDIS_Msk                   (_U_(0x1) << QSPI_CR_QSPIDIS_Pos)                    /**< (QSPI_CR) QSPI Disable Mask */
#define QSPI_CR_QSPIDIS(value)                (QSPI_CR_QSPIDIS_Msk & ((value) << QSPI_CR_QSPIDIS_Pos))
#define QSPI_CR_SWRST_Pos                     _U_(7)                                               /**< (QSPI_CR) QSPI Software Reset Position */
#define QSPI_CR_SWRST_Msk                     (_U_(0x1) << QSPI_CR_SWRST_Pos)                      /**< (QSPI_CR) QSPI Software Reset Mask */
#define QSPI_CR_SWRST(value)                  (QSPI_CR_SWRST_Msk & ((value) << QSPI_CR_SWRST_Pos))
#define QSPI_CR_LASTXFER_Pos                  _U_(24)                                              /**< (QSPI_CR) Last Transfer Position */
#define QSPI_CR_LASTXFER_Msk                  (_U_(0x1) << QSPI_CR_LASTXFER_Pos)                   /**< (QSPI_CR) Last Transfer Mask */
#define QSPI_CR_LASTXFER(value)               (QSPI_CR_LASTXFER_Msk & ((value) << QSPI_CR_LASTXFER_Pos))
#define QSPI_CR_Msk                           _U_(0x01000083)                                      /**< (QSPI_CR) Register Mask  */


/* -------- QSPI_MR : (QSPI Offset: 0x04) (R/W 32) Mode Register -------- */
#define QSPI_MR_SMM_Pos                       _U_(0)                                               /**< (QSPI_MR) Serial Memory Mode Position */
#define QSPI_MR_SMM_Msk                       (_U_(0x1) << QSPI_MR_SMM_Pos)                        /**< (QSPI_MR) Serial Memory Mode Mask */
#define QSPI_MR_SMM(value)                    (QSPI_MR_SMM_Msk & ((value) << QSPI_MR_SMM_Pos))    
#define   QSPI_MR_SMM_SPI_Val                 _U_(0x0)                                             /**< (QSPI_MR) The QSPI is in SPI mode.  */
#define   QSPI_MR_SMM_MEMORY_Val              _U_(0x1)                                             /**< (QSPI_MR) The QSPI is in Serial Memory mode.  */
#define QSPI_MR_SMM_SPI                       (QSPI_MR_SMM_SPI_Val << QSPI_MR_SMM_Pos)             /**< (QSPI_MR) The QSPI is in SPI mode. Position  */
#define QSPI_MR_SMM_MEMORY                    (QSPI_MR_SMM_MEMORY_Val << QSPI_MR_SMM_Pos)          /**< (QSPI_MR) The QSPI is in Serial Memory mode. Position  */
#define QSPI_MR_LLB_Pos                       _U_(1)                                               /**< (QSPI_MR) Local Loopback Enable Position */
#define QSPI_MR_LLB_Msk                       (_U_(0x1) << QSPI_MR_LLB_Pos)                        /**< (QSPI_MR) Local Loopback Enable Mask */
#define QSPI_MR_LLB(value)                    (QSPI_MR_LLB_Msk & ((value) << QSPI_MR_LLB_Pos))    
#define   QSPI_MR_LLB_DISABLED_Val            _U_(0x0)                                             /**< (QSPI_MR) Local loopback path disabled.  */
#define   QSPI_MR_LLB_ENABLED_Val             _U_(0x1)                                             /**< (QSPI_MR) Local loopback path enabled.  */
#define QSPI_MR_LLB_DISABLED                  (QSPI_MR_LLB_DISABLED_Val << QSPI_MR_LLB_Pos)        /**< (QSPI_MR) Local loopback path disabled. Position  */
#define QSPI_MR_LLB_ENABLED                   (QSPI_MR_LLB_ENABLED_Val << QSPI_MR_LLB_Pos)         /**< (QSPI_MR) Local loopback path enabled. Position  */
#define QSPI_MR_WDRBT_Pos                     _U_(2)                                               /**< (QSPI_MR) Wait Data Read Before Transfer Position */
#define QSPI_MR_WDRBT_Msk                     (_U_(0x1) << QSPI_MR_WDRBT_Pos)                      /**< (QSPI_MR) Wait Data Read Before Transfer Mask */
#define QSPI_MR_WDRBT(value)                  (QSPI_MR_WDRBT_Msk & ((value) << QSPI_MR_WDRBT_Pos))
#define   QSPI_MR_WDRBT_DISABLED_Val          _U_(0x0)                                             /**< (QSPI_MR) No effect. In SPI mode, a transfer can be initiated whatever the state of the QSPI_RDR is.  */
#define   QSPI_MR_WDRBT_ENABLED_Val           _U_(0x1)                                             /**< (QSPI_MR) In SPI mode, a transfer can start only if the QSPI_RDR is empty, i.e., does not contain any unread data. This mode prevents overrun error in reception.  */
#define QSPI_MR_WDRBT_DISABLED                (QSPI_MR_WDRBT_DISABLED_Val << QSPI_MR_WDRBT_Pos)    /**< (QSPI_MR) No effect. In SPI mode, a transfer can be initiated whatever the state of the QSPI_RDR is. Position  */
#define QSPI_MR_WDRBT_ENABLED                 (QSPI_MR_WDRBT_ENABLED_Val << QSPI_MR_WDRBT_Pos)     /**< (QSPI_MR) In SPI mode, a transfer can start only if the QSPI_RDR is empty, i.e., does not contain any unread data. This mode prevents overrun error in reception. Position  */
#define QSPI_MR_CSMODE_Pos                    _U_(4)                                               /**< (QSPI_MR) Chip Select Mode Position */
#define QSPI_MR_CSMODE_Msk                    (_U_(0x3) << QSPI_MR_CSMODE_Pos)                     /**< (QSPI_MR) Chip Select Mode Mask */
#define QSPI_MR_CSMODE(value)                 (QSPI_MR_CSMODE_Msk & ((value) << QSPI_MR_CSMODE_Pos))
#define   QSPI_MR_CSMODE_NOT_RELOADED_Val     _U_(0x0)                                             /**< (QSPI_MR) The chip select is deasserted if QSPI_TDR.TD has not been reloaded before the end of the current transfer.  */
#define   QSPI_MR_CSMODE_LASTXFER_Val         _U_(0x1)                                             /**< (QSPI_MR) The chip select is deasserted when the bit LASTXFER is written at 1 and the character written in QSPI_TDR.TD has been transferred.  */
#define   QSPI_MR_CSMODE_SYSTEMATICALLY_Val   _U_(0x2)                                             /**< (QSPI_MR) The chip select is deasserted systematically after each transfer.  */
#define QSPI_MR_CSMODE_NOT_RELOADED           (QSPI_MR_CSMODE_NOT_RELOADED_Val << QSPI_MR_CSMODE_Pos) /**< (QSPI_MR) The chip select is deasserted if QSPI_TDR.TD has not been reloaded before the end of the current transfer. Position  */
#define QSPI_MR_CSMODE_LASTXFER               (QSPI_MR_CSMODE_LASTXFER_Val << QSPI_MR_CSMODE_Pos)  /**< (QSPI_MR) The chip select is deasserted when the bit LASTXFER is written at 1 and the character written in QSPI_TDR.TD has been transferred. Position  */
#define QSPI_MR_CSMODE_SYSTEMATICALLY         (QSPI_MR_CSMODE_SYSTEMATICALLY_Val << QSPI_MR_CSMODE_Pos) /**< (QSPI_MR) The chip select is deasserted systematically after each transfer. Position  */
#define QSPI_MR_NBBITS_Pos                    _U_(8)                                               /**< (QSPI_MR) Number Of Bits Per Transfer Position */
#define QSPI_MR_NBBITS_Msk                    (_U_(0xF) << QSPI_MR_NBBITS_Pos)                     /**< (QSPI_MR) Number Of Bits Per Transfer Mask */
#define QSPI_MR_NBBITS(value)                 (QSPI_MR_NBBITS_Msk & ((value) << QSPI_MR_NBBITS_Pos))
#define   QSPI_MR_NBBITS_8_BIT_Val            _U_(0x0)                                             /**< (QSPI_MR) 8 bits for transfer  */
#define   QSPI_MR_NBBITS_16_BIT_Val           _U_(0x8)                                             /**< (QSPI_MR) 16 bits for transfer  */
#define QSPI_MR_NBBITS_8_BIT                  (QSPI_MR_NBBITS_8_BIT_Val << QSPI_MR_NBBITS_Pos)     /**< (QSPI_MR) 8 bits for transfer Position  */
#define QSPI_MR_NBBITS_16_BIT                 (QSPI_MR_NBBITS_16_BIT_Val << QSPI_MR_NBBITS_Pos)    /**< (QSPI_MR) 16 bits for transfer Position  */
#define QSPI_MR_DLYBCT_Pos                    _U_(16)                                              /**< (QSPI_MR) Delay Between Consecutive Transfers Position */
#define QSPI_MR_DLYBCT_Msk                    (_U_(0xFF) << QSPI_MR_DLYBCT_Pos)                    /**< (QSPI_MR) Delay Between Consecutive Transfers Mask */
#define QSPI_MR_DLYBCT(value)                 (QSPI_MR_DLYBCT_Msk & ((value) << QSPI_MR_DLYBCT_Pos))
#define QSPI_MR_DLYCS_Pos                     _U_(24)                                              /**< (QSPI_MR) Minimum Inactive QCS Delay Position */
#define QSPI_MR_DLYCS_Msk                     (_U_(0xFF) << QSPI_MR_DLYCS_Pos)                     /**< (QSPI_MR) Minimum Inactive QCS Delay Mask */
#define QSPI_MR_DLYCS(value)                  (QSPI_MR_DLYCS_Msk & ((value) << QSPI_MR_DLYCS_Pos))
#define QSPI_MR_Msk                           _U_(0xFFFF0F37)                                      /**< (QSPI_MR) Register Mask  */


/* -------- QSPI_RDR : (QSPI Offset: 0x08) ( R/ 32) Receive Data Register -------- */
#define QSPI_RDR_RD_Pos                       _U_(0)                                               /**< (QSPI_RDR) Receive Data Position */
#define QSPI_RDR_RD_Msk                       (_U_(0xFFFF) << QSPI_RDR_RD_Pos)                     /**< (QSPI_RDR) Receive Data Mask */
#define QSPI_RDR_RD(value)                    (QSPI_RDR_RD_Msk & ((value) << QSPI_RDR_RD_Pos))    
#define QSPI_RDR_Msk                          _U_(0x0000FFFF)                                      /**< (QSPI_RDR) Register Mask  */


/* -------- QSPI_TDR : (QSPI Offset: 0x0C) ( /W 32) Transmit Data Register -------- */
#define QSPI_TDR_TD_Pos                       _U_(0)                                               /**< (QSPI_TDR) Transmit Data Position */
#define QSPI_TDR_TD_Msk                       (_U_(0xFFFF) << QSPI_TDR_TD_Pos)                     /**< (QSPI_TDR) Transmit Data Mask */
#define QSPI_TDR_TD(value)                    (QSPI_TDR_TD_Msk & ((value) << QSPI_TDR_TD_Pos))    
#define QSPI_TDR_Msk                          _U_(0x0000FFFF)                                      /**< (QSPI_TDR) Register Mask  */


/* -------- QSPI_SR : (QSPI Offset: 0x10) ( R/ 32) Status Register -------- */
#define QSPI_SR_RDRF_Pos                      _U_(0)                                               /**< (QSPI_SR) Receive Data Register Full (cleared by reading SPI_RDR) Position */
#define QSPI_SR_RDRF_Msk                      (_U_(0x1) << QSPI_SR_RDRF_Pos)                       /**< (QSPI_SR) Receive Data Register Full (cleared by reading SPI_RDR) Mask */
#define QSPI_SR_RDRF(value)                   (QSPI_SR_RDRF_Msk & ((value) << QSPI_SR_RDRF_Pos))  
#define QSPI_SR_TDRE_Pos                      _U_(1)                                               /**< (QSPI_SR) Transmit Data Register Empty (cleared by writing SPI_TDR) Position */
#define QSPI_SR_TDRE_Msk                      (_U_(0x1) << QSPI_SR_TDRE_Pos)                       /**< (QSPI_SR) Transmit Data Register Empty (cleared by writing SPI_TDR) Mask */
#define QSPI_SR_TDRE(value)                   (QSPI_SR_TDRE_Msk & ((value) << QSPI_SR_TDRE_Pos))  
#define QSPI_SR_TXEMPTY_Pos                   _U_(2)                                               /**< (QSPI_SR) Transmission Registers Empty (cleared by writing SPI_TDR) Position */
#define QSPI_SR_TXEMPTY_Msk                   (_U_(0x1) << QSPI_SR_TXEMPTY_Pos)                    /**< (QSPI_SR) Transmission Registers Empty (cleared by writing SPI_TDR) Mask */
#define QSPI_SR_TXEMPTY(value)                (QSPI_SR_TXEMPTY_Msk & ((value) << QSPI_SR_TXEMPTY_Pos))
#define QSPI_SR_OVRES_Pos                     _U_(3)                                               /**< (QSPI_SR) Overrun Error Status (cleared on read) Position */
#define QSPI_SR_OVRES_Msk                     (_U_(0x1) << QSPI_SR_OVRES_Pos)                      /**< (QSPI_SR) Overrun Error Status (cleared on read) Mask */
#define QSPI_SR_OVRES(value)                  (QSPI_SR_OVRES_Msk & ((value) << QSPI_SR_OVRES_Pos))
#define QSPI_SR_CSR_Pos                       _U_(8)                                               /**< (QSPI_SR) Chip Select Rise (cleared on read) Position */
#define QSPI_SR_CSR_Msk                       (_U_(0x1) << QSPI_SR_CSR_Pos)                        /**< (QSPI_SR) Chip Select Rise (cleared on read) Mask */
#define QSPI_SR_CSR(value)                    (QSPI_SR_CSR_Msk & ((value) << QSPI_SR_CSR_Pos))    
#define QSPI_SR_CSS_Pos                       _U_(9)                                               /**< (QSPI_SR) Chip Select Status Position */
#define QSPI_SR_CSS_Msk                       (_U_(0x1) << QSPI_SR_CSS_Pos)                        /**< (QSPI_SR) Chip Select Status Mask */
#define QSPI_SR_CSS(value)                    (QSPI_SR_CSS_Msk & ((value) << QSPI_SR_CSS_Pos))    
#define QSPI_SR_INSTRE_Pos                    _U_(10)                                              /**< (QSPI_SR) Instruction End Status (cleared on read) Position */
#define QSPI_SR_INSTRE_Msk                    (_U_(0x1) << QSPI_SR_INSTRE_Pos)                     /**< (QSPI_SR) Instruction End Status (cleared on read) Mask */
#define QSPI_SR_INSTRE(value)                 (QSPI_SR_INSTRE_Msk & ((value) << QSPI_SR_INSTRE_Pos))
#define QSPI_SR_QSPIENS_Pos                   _U_(24)                                              /**< (QSPI_SR) QSPI Enable Status Position */
#define QSPI_SR_QSPIENS_Msk                   (_U_(0x1) << QSPI_SR_QSPIENS_Pos)                    /**< (QSPI_SR) QSPI Enable Status Mask */
#define QSPI_SR_QSPIENS(value)                (QSPI_SR_QSPIENS_Msk & ((value) << QSPI_SR_QSPIENS_Pos))
#define QSPI_SR_Msk                           _U_(0x0100070F)                                      /**< (QSPI_SR) Register Mask  */


/* -------- QSPI_IER : (QSPI Offset: 0x14) ( /W 32) Interrupt Enable Register -------- */
#define QSPI_IER_RDRF_Pos                     _U_(0)                                               /**< (QSPI_IER) Receive Data Register Full Interrupt Enable Position */
#define QSPI_IER_RDRF_Msk                     (_U_(0x1) << QSPI_IER_RDRF_Pos)                      /**< (QSPI_IER) Receive Data Register Full Interrupt Enable Mask */
#define QSPI_IER_RDRF(value)                  (QSPI_IER_RDRF_Msk & ((value) << QSPI_IER_RDRF_Pos))
#define QSPI_IER_TDRE_Pos                     _U_(1)                                               /**< (QSPI_IER) Transmit Data Register Empty Interrupt Enable Position */
#define QSPI_IER_TDRE_Msk                     (_U_(0x1) << QSPI_IER_TDRE_Pos)                      /**< (QSPI_IER) Transmit Data Register Empty Interrupt Enable Mask */
#define QSPI_IER_TDRE(value)                  (QSPI_IER_TDRE_Msk & ((value) << QSPI_IER_TDRE_Pos))
#define QSPI_IER_TXEMPTY_Pos                  _U_(2)                                               /**< (QSPI_IER) Transmission Registers Empty Enable Position */
#define QSPI_IER_TXEMPTY_Msk                  (_U_(0x1) << QSPI_IER_TXEMPTY_Pos)                   /**< (QSPI_IER) Transmission Registers Empty Enable Mask */
#define QSPI_IER_TXEMPTY(value)               (QSPI_IER_TXEMPTY_Msk & ((value) << QSPI_IER_TXEMPTY_Pos))
#define QSPI_IER_OVRES_Pos                    _U_(3)                                               /**< (QSPI_IER) Overrun Error Interrupt Enable Position */
#define QSPI_IER_OVRES_Msk                    (_U_(0x1) << QSPI_IER_OVRES_Pos)                     /**< (QSPI_IER) Overrun Error Interrupt Enable Mask */
#define QSPI_IER_OVRES(value)                 (QSPI_IER_OVRES_Msk & ((value) << QSPI_IER_OVRES_Pos))
#define QSPI_IER_CSR_Pos                      _U_(8)                                               /**< (QSPI_IER) Chip Select Rise Interrupt Enable Position */
#define QSPI_IER_CSR_Msk                      (_U_(0x1) << QSPI_IER_CSR_Pos)                       /**< (QSPI_IER) Chip Select Rise Interrupt Enable Mask */
#define QSPI_IER_CSR(value)                   (QSPI_IER_CSR_Msk & ((value) << QSPI_IER_CSR_Pos))  
#define QSPI_IER_CSS_Pos                      _U_(9)                                               /**< (QSPI_IER) Chip Select Status Interrupt Enable Position */
#define QSPI_IER_CSS_Msk                      (_U_(0x1) << QSPI_IER_CSS_Pos)                       /**< (QSPI_IER) Chip Select Status Interrupt Enable Mask */
#define QSPI_IER_CSS(value)                   (QSPI_IER_CSS_Msk & ((value) << QSPI_IER_CSS_Pos))  
#define QSPI_IER_INSTRE_Pos                   _U_(10)                                              /**< (QSPI_IER) Instruction End Interrupt Enable Position */
#define QSPI_IER_INSTRE_Msk                   (_U_(0x1) << QSPI_IER_INSTRE_Pos)                    /**< (QSPI_IER) Instruction End Interrupt Enable Mask */
#define QSPI_IER_INSTRE(value)                (QSPI_IER_INSTRE_Msk & ((value) << QSPI_IER_INSTRE_Pos))
#define QSPI_IER_Msk                          _U_(0x0000070F)                                      /**< (QSPI_IER) Register Mask  */


/* -------- QSPI_IDR : (QSPI Offset: 0x18) ( /W 32) Interrupt Disable Register -------- */
#define QSPI_IDR_RDRF_Pos                     _U_(0)                                               /**< (QSPI_IDR) Receive Data Register Full Interrupt Disable Position */
#define QSPI_IDR_RDRF_Msk                     (_U_(0x1) << QSPI_IDR_RDRF_Pos)                      /**< (QSPI_IDR) Receive Data Register Full Interrupt Disable Mask */
#define QSPI_IDR_RDRF(value)                  (QSPI_IDR_RDRF_Msk & ((value) << QSPI_IDR_RDRF_Pos))
#define QSPI_IDR_TDRE_Pos                     _U_(1)                                               /**< (QSPI_IDR) Transmit Data Register Empty Interrupt Disable Position */
#define QSPI_IDR_TDRE_Msk                     (_U_(0x1) << QSPI_IDR_TDRE_Pos)                      /**< (QSPI_IDR) Transmit Data Register Empty Interrupt Disable Mask */
#define QSPI_IDR_TDRE(value)                  (QSPI_IDR_TDRE_Msk & ((value) << QSPI_IDR_TDRE_Pos))
#define QSPI_IDR_TXEMPTY_Pos                  _U_(2)                                               /**< (QSPI_IDR) Transmission Registers Empty Disable Position */
#define QSPI_IDR_TXEMPTY_Msk                  (_U_(0x1) << QSPI_IDR_TXEMPTY_Pos)                   /**< (QSPI_IDR) Transmission Registers Empty Disable Mask */
#define QSPI_IDR_TXEMPTY(value)               (QSPI_IDR_TXEMPTY_Msk & ((value) << QSPI_IDR_TXEMPTY_Pos))
#define QSPI_IDR_OVRES_Pos                    _U_(3)                                               /**< (QSPI_IDR) Overrun Error Interrupt Disable Position */
#define QSPI_IDR_OVRES_Msk                    (_U_(0x1) << QSPI_IDR_OVRES_Pos)                     /**< (QSPI_IDR) Overrun Error Interrupt Disable Mask */
#define QSPI_IDR_OVRES(value)                 (QSPI_IDR_OVRES_Msk & ((value) << QSPI_IDR_OVRES_Pos))
#define QSPI_IDR_CSR_Pos                      _U_(8)                                               /**< (QSPI_IDR) Chip Select Rise Interrupt Disable Position */
#define QSPI_IDR_CSR_Msk                      (_U_(0x1) << QSPI_IDR_CSR_Pos)                       /**< (QSPI_IDR) Chip Select Rise Interrupt Disable Mask */
#define QSPI_IDR_CSR(value)                   (QSPI_IDR_CSR_Msk & ((value) << QSPI_IDR_CSR_Pos))  
#define QSPI_IDR_CSS_Pos                      _U_(9)                                               /**< (QSPI_IDR) Chip Select Status Interrupt Disable Position */
#define QSPI_IDR_CSS_Msk                      (_U_(0x1) << QSPI_IDR_CSS_Pos)                       /**< (QSPI_IDR) Chip Select Status Interrupt Disable Mask */
#define QSPI_IDR_CSS(value)                   (QSPI_IDR_CSS_Msk & ((value) << QSPI_IDR_CSS_Pos))  
#define QSPI_IDR_INSTRE_Pos                   _U_(10)                                              /**< (QSPI_IDR) Instruction End Interrupt Disable Position */
#define QSPI_IDR_INSTRE_Msk                   (_U_(0x1) << QSPI_IDR_INSTRE_Pos)                    /**< (QSPI_IDR) Instruction End Interrupt Disable Mask */
#define QSPI_IDR_INSTRE(value)                (QSPI_IDR_INSTRE_Msk & ((value) << QSPI_IDR_INSTRE_Pos))
#define QSPI_IDR_Msk                          _U_(0x0000070F)                                      /**< (QSPI_IDR) Register Mask  */


/* -------- QSPI_IMR : (QSPI Offset: 0x1C) ( R/ 32) Interrupt Mask Register -------- */
#define QSPI_IMR_RDRF_Pos                     _U_(0)                                               /**< (QSPI_IMR) Receive Data Register Full Interrupt Mask Position */
#define QSPI_IMR_RDRF_Msk                     (_U_(0x1) << QSPI_IMR_RDRF_Pos)                      /**< (QSPI_IMR) Receive Data Register Full Interrupt Mask Mask */
#define QSPI_IMR_RDRF(value)                  (QSPI_IMR_RDRF_Msk & ((value) << QSPI_IMR_RDRF_Pos))
#define QSPI_IMR_TDRE_Pos                     _U_(1)                                               /**< (QSPI_IMR) Transmit Data Register Empty Interrupt Mask Position */
#define QSPI_IMR_TDRE_Msk                     (_U_(0x1) << QSPI_IMR_TDRE_Pos)                      /**< (QSPI_IMR) Transmit Data Register Empty Interrupt Mask Mask */
#define QSPI_IMR_TDRE(value)                  (QSPI_IMR_TDRE_Msk & ((value) << QSPI_IMR_TDRE_Pos))
#define QSPI_IMR_TXEMPTY_Pos                  _U_(2)                                               /**< (QSPI_IMR) Transmission Registers Empty Mask Position */
#define QSPI_IMR_TXEMPTY_Msk                  (_U_(0x1) << QSPI_IMR_TXEMPTY_Pos)                   /**< (QSPI_IMR) Transmission Registers Empty Mask Mask */
#define QSPI_IMR_TXEMPTY(value)               (QSPI_IMR_TXEMPTY_Msk & ((value) << QSPI_IMR_TXEMPTY_Pos))
#define QSPI_IMR_OVRES_Pos                    _U_(3)                                               /**< (QSPI_IMR) Overrun Error Interrupt Mask Position */
#define QSPI_IMR_OVRES_Msk                    (_U_(0x1) << QSPI_IMR_OVRES_Pos)                     /**< (QSPI_IMR) Overrun Error Interrupt Mask Mask */
#define QSPI_IMR_OVRES(value)                 (QSPI_IMR_OVRES_Msk & ((value) << QSPI_IMR_OVRES_Pos))
#define QSPI_IMR_CSR_Pos                      _U_(8)                                               /**< (QSPI_IMR) Chip Select Rise Interrupt Mask Position */
#define QSPI_IMR_CSR_Msk                      (_U_(0x1) << QSPI_IMR_CSR_Pos)                       /**< (QSPI_IMR) Chip Select Rise Interrupt Mask Mask */
#define QSPI_IMR_CSR(value)                   (QSPI_IMR_CSR_Msk & ((value) << QSPI_IMR_CSR_Pos))  
#define QSPI_IMR_CSS_Pos                      _U_(9)                                               /**< (QSPI_IMR) Chip Select Status Interrupt Mask Position */
#define QSPI_IMR_CSS_Msk                      (_U_(0x1) << QSPI_IMR_CSS_Pos)                       /**< (QSPI_IMR) Chip Select Status Interrupt Mask Mask */
#define QSPI_IMR_CSS(value)                   (QSPI_IMR_CSS_Msk & ((value) << QSPI_IMR_CSS_Pos))  
#define QSPI_IMR_INSTRE_Pos                   _U_(10)                                              /**< (QSPI_IMR) Instruction End Interrupt Mask Position */
#define QSPI_IMR_INSTRE_Msk                   (_U_(0x1) << QSPI_IMR_INSTRE_Pos)                    /**< (QSPI_IMR) Instruction End Interrupt Mask Mask */
#define QSPI_IMR_INSTRE(value)                (QSPI_IMR_INSTRE_Msk & ((value) << QSPI_IMR_INSTRE_Pos))
#define QSPI_IMR_Msk                          _U_(0x0000070F)                                      /**< (QSPI_IMR) Register Mask  */


/* -------- QSPI_SCR : (QSPI Offset: 0x20) (R/W 32) Serial Clock Register -------- */
#define QSPI_SCR_CPOL_Pos                     _U_(0)                                               /**< (QSPI_SCR) Clock Polarity Position */
#define QSPI_SCR_CPOL_Msk                     (_U_(0x1) << QSPI_SCR_CPOL_Pos)                      /**< (QSPI_SCR) Clock Polarity Mask */
#define QSPI_SCR_CPOL(value)                  (QSPI_SCR_CPOL_Msk & ((value) << QSPI_SCR_CPOL_Pos))
#define QSPI_SCR_CPHA_Pos                     _U_(1)                                               /**< (QSPI_SCR) Clock Phase Position */
#define QSPI_SCR_CPHA_Msk                     (_U_(0x1) << QSPI_SCR_CPHA_Pos)                      /**< (QSPI_SCR) Clock Phase Mask */
#define QSPI_SCR_CPHA(value)                  (QSPI_SCR_CPHA_Msk & ((value) << QSPI_SCR_CPHA_Pos))
#define QSPI_SCR_SCBR_Pos                     _U_(8)                                               /**< (QSPI_SCR) Serial Clock Baud Rate Position */
#define QSPI_SCR_SCBR_Msk                     (_U_(0xFF) << QSPI_SCR_SCBR_Pos)                     /**< (QSPI_SCR) Serial Clock Baud Rate Mask */
#define QSPI_SCR_SCBR(value)                  (QSPI_SCR_SCBR_Msk & ((value) << QSPI_SCR_SCBR_Pos))
#define QSPI_SCR_DLYBS_Pos                    _U_(16)                                              /**< (QSPI_SCR) Delay Before QSCK Position */
#define QSPI_SCR_DLYBS_Msk                    (_U_(0xFF) << QSPI_SCR_DLYBS_Pos)                    /**< (QSPI_SCR) Delay Before QSCK Mask */
#define QSPI_SCR_DLYBS(value)                 (QSPI_SCR_DLYBS_Msk & ((value) << QSPI_SCR_DLYBS_Pos))
#define QSPI_SCR_Msk                          _U_(0x00FFFF03)                                      /**< (QSPI_SCR) Register Mask  */


/* -------- QSPI_IAR : (QSPI Offset: 0x30) (R/W 32) Instruction Address Register -------- */
#define QSPI_IAR_ADDR_Pos                     _U_(0)                                               /**< (QSPI_IAR) Address Position */
#define QSPI_IAR_ADDR_Msk                     (_U_(0xFFFFFFFF) << QSPI_IAR_ADDR_Pos)               /**< (QSPI_IAR) Address Mask */
#define QSPI_IAR_ADDR(value)                  (QSPI_IAR_ADDR_Msk & ((value) << QSPI_IAR_ADDR_Pos))
#define QSPI_IAR_Msk                          _U_(0xFFFFFFFF)                                      /**< (QSPI_IAR) Register Mask  */


/* -------- QSPI_ICR : (QSPI Offset: 0x34) (R/W 32) Instruction Code Register -------- */
#define QSPI_ICR_INST_Pos                     _U_(0)                                               /**< (QSPI_ICR) Instruction Code Position */
#define QSPI_ICR_INST_Msk                     (_U_(0xFF) << QSPI_ICR_INST_Pos)                     /**< (QSPI_ICR) Instruction Code Mask */
#define QSPI_ICR_INST(value)                  (QSPI_ICR_INST_Msk & ((value) << QSPI_ICR_INST_Pos))
#define QSPI_ICR_OPT_Pos                      _U_(16)                                              /**< (QSPI_ICR) Option Code Position */
#define QSPI_ICR_OPT_Msk                      (_U_(0xFF) << QSPI_ICR_OPT_Pos)                      /**< (QSPI_ICR) Option Code Mask */
#define QSPI_ICR_OPT(value)                   (QSPI_ICR_OPT_Msk & ((value) << QSPI_ICR_OPT_Pos))  
#define QSPI_ICR_Msk                          _U_(0x00FF00FF)                                      /**< (QSPI_ICR) Register Mask  */


/* -------- QSPI_IFR : (QSPI Offset: 0x38) (R/W 32) Instruction Frame Register -------- */
#define QSPI_IFR_WIDTH_Pos                    _U_(0)                                               /**< (QSPI_IFR) Width of Instruction Code, Address, Option Code and Data Position */
#define QSPI_IFR_WIDTH_Msk                    (_U_(0x7) << QSPI_IFR_WIDTH_Pos)                     /**< (QSPI_IFR) Width of Instruction Code, Address, Option Code and Data Mask */
#define QSPI_IFR_WIDTH(value)                 (QSPI_IFR_WIDTH_Msk & ((value) << QSPI_IFR_WIDTH_Pos))
#define   QSPI_IFR_WIDTH_SINGLE_BIT_SPI_Val   _U_(0x0)                                             /**< (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Single-bit SPI  */
#define   QSPI_IFR_WIDTH_DUAL_OUTPUT_Val      _U_(0x1)                                             /**< (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Dual SPI  */
#define   QSPI_IFR_WIDTH_QUAD_OUTPUT_Val      _U_(0x2)                                             /**< (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Quad SPI  */
#define   QSPI_IFR_WIDTH_DUAL_IO_Val          _U_(0x3)                                             /**< (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Dual SPI / Data: Dual SPI  */
#define   QSPI_IFR_WIDTH_QUAD_IO_Val          _U_(0x4)                                             /**< (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Quad SPI / Data: Quad SPI  */
#define   QSPI_IFR_WIDTH_DUAL_CMD_Val         _U_(0x5)                                             /**< (QSPI_IFR) Instruction: Dual SPI / Address-Option: Dual SPI / Data: Dual SPI  */
#define   QSPI_IFR_WIDTH_QUAD_CMD_Val         _U_(0x6)                                             /**< (QSPI_IFR) Instruction: Quad SPI / Address-Option: Quad SPI / Data: Quad SPI  */
#define QSPI_IFR_WIDTH_SINGLE_BIT_SPI         (QSPI_IFR_WIDTH_SINGLE_BIT_SPI_Val << QSPI_IFR_WIDTH_Pos) /**< (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Single-bit SPI Position  */
#define QSPI_IFR_WIDTH_DUAL_OUTPUT            (QSPI_IFR_WIDTH_DUAL_OUTPUT_Val << QSPI_IFR_WIDTH_Pos) /**< (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Dual SPI Position  */
#define QSPI_IFR_WIDTH_QUAD_OUTPUT            (QSPI_IFR_WIDTH_QUAD_OUTPUT_Val << QSPI_IFR_WIDTH_Pos) /**< (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Quad SPI Position  */
#define QSPI_IFR_WIDTH_DUAL_IO                (QSPI_IFR_WIDTH_DUAL_IO_Val << QSPI_IFR_WIDTH_Pos)   /**< (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Dual SPI / Data: Dual SPI Position  */
#define QSPI_IFR_WIDTH_QUAD_IO                (QSPI_IFR_WIDTH_QUAD_IO_Val << QSPI_IFR_WIDTH_Pos)   /**< (QSPI_IFR) Instruction: Single-bit SPI / Address-Option: Quad SPI / Data: Quad SPI Position  */
#define QSPI_IFR_WIDTH_DUAL_CMD               (QSPI_IFR_WIDTH_DUAL_CMD_Val << QSPI_IFR_WIDTH_Pos)  /**< (QSPI_IFR) Instruction: Dual SPI / Address-Option: Dual SPI / Data: Dual SPI Position  */
#define QSPI_IFR_WIDTH_QUAD_CMD               (QSPI_IFR_WIDTH_QUAD_CMD_Val << QSPI_IFR_WIDTH_Pos)  /**< (QSPI_IFR) Instruction: Quad SPI / Address-Option: Quad SPI / Data: Quad SPI Position  */
#define QSPI_IFR_INSTEN_Pos                   _U_(4)                                               /**< (QSPI_IFR) Instruction Enable Position */
#define QSPI_IFR_INSTEN_Msk                   (_U_(0x1) << QSPI_IFR_INSTEN_Pos)                    /**< (QSPI_IFR) Instruction Enable Mask */
#define QSPI_IFR_INSTEN(value)                (QSPI_IFR_INSTEN_Msk & ((value) << QSPI_IFR_INSTEN_Pos))
#define QSPI_IFR_ADDREN_Pos                   _U_(5)                                               /**< (QSPI_IFR) Address Enable Position */
#define QSPI_IFR_ADDREN_Msk                   (_U_(0x1) << QSPI_IFR_ADDREN_Pos)                    /**< (QSPI_IFR) Address Enable Mask */
#define QSPI_IFR_ADDREN(value)                (QSPI_IFR_ADDREN_Msk & ((value) << QSPI_IFR_ADDREN_Pos))
#define QSPI_IFR_OPTEN_Pos                    _U_(6)                                               /**< (QSPI_IFR) Option Enable Position */
#define QSPI_IFR_OPTEN_Msk                    (_U_(0x1) << QSPI_IFR_OPTEN_Pos)                     /**< (QSPI_IFR) Option Enable Mask */
#define QSPI_IFR_OPTEN(value)                 (QSPI_IFR_OPTEN_Msk & ((value) << QSPI_IFR_OPTEN_Pos))
#define QSPI_IFR_DATAEN_Pos                   _U_(7)                                               /**< (QSPI_IFR) Data Enable Position */
#define QSPI_IFR_DATAEN_Msk                   (_U_(0x1) << QSPI_IFR_DATAEN_Pos)                    /**< (QSPI_IFR) Data Enable Mask */
#define QSPI_IFR_DATAEN(value)                (QSPI_IFR_DATAEN_Msk & ((value) << QSPI_IFR_DATAEN_Pos))
#define QSPI_IFR_OPTL_Pos                     _U_(8)                                               /**< (QSPI_IFR) Option Code Length Position */
#define QSPI_IFR_OPTL_Msk                     (_U_(0x3) << QSPI_IFR_OPTL_Pos)                      /**< (QSPI_IFR) Option Code Length Mask */
#define QSPI_IFR_OPTL(value)                  (QSPI_IFR_OPTL_Msk & ((value) << QSPI_IFR_OPTL_Pos))
#define   QSPI_IFR_OPTL_OPTION_1BIT_Val       _U_(0x0)                                             /**< (QSPI_IFR) The option code is 1 bit long.  */
#define   QSPI_IFR_OPTL_OPTION_2BIT_Val       _U_(0x1)                                             /**< (QSPI_IFR) The option code is 2 bits long.  */
#define   QSPI_IFR_OPTL_OPTION_4BIT_Val       _U_(0x2)                                             /**< (QSPI_IFR) The option code is 4 bits long.  */
#define   QSPI_IFR_OPTL_OPTION_8BIT_Val       _U_(0x3)                                             /**< (QSPI_IFR) The option code is 8 bits long.  */
#define QSPI_IFR_OPTL_OPTION_1BIT             (QSPI_IFR_OPTL_OPTION_1BIT_Val << QSPI_IFR_OPTL_Pos) /**< (QSPI_IFR) The option code is 1 bit long. Position  */
#define QSPI_IFR_OPTL_OPTION_2BIT             (QSPI_IFR_OPTL_OPTION_2BIT_Val << QSPI_IFR_OPTL_Pos) /**< (QSPI_IFR) The option code is 2 bits long. Position  */
#define QSPI_IFR_OPTL_OPTION_4BIT             (QSPI_IFR_OPTL_OPTION_4BIT_Val << QSPI_IFR_OPTL_Pos) /**< (QSPI_IFR) The option code is 4 bits long. Position  */
#define QSPI_IFR_OPTL_OPTION_8BIT             (QSPI_IFR_OPTL_OPTION_8BIT_Val << QSPI_IFR_OPTL_Pos) /**< (QSPI_IFR) The option code is 8 bits long. Position  */
#define QSPI_IFR_ADDRL_Pos                    _U_(10)                                              /**< (QSPI_IFR) Address Length Position */
#define QSPI_IFR_ADDRL_Msk                    (_U_(0x1) << QSPI_IFR_ADDRL_Pos)                     /**< (QSPI_IFR) Address Length Mask */
#define QSPI_IFR_ADDRL(value)                 (QSPI_IFR_ADDRL_Msk & ((value) << QSPI_IFR_ADDRL_Pos))
#define   QSPI_IFR_ADDRL_24_BIT_Val           _U_(0x0)                                             /**< (QSPI_IFR) The address is 24 bits long.  */
#define   QSPI_IFR_ADDRL_32_BIT_Val           _U_(0x1)                                             /**< (QSPI_IFR) The address is 32 bits long.  */
#define QSPI_IFR_ADDRL_24_BIT                 (QSPI_IFR_ADDRL_24_BIT_Val << QSPI_IFR_ADDRL_Pos)    /**< (QSPI_IFR) The address is 24 bits long. Position  */
#define QSPI_IFR_ADDRL_32_BIT                 (QSPI_IFR_ADDRL_32_BIT_Val << QSPI_IFR_ADDRL_Pos)    /**< (QSPI_IFR) The address is 32 bits long. Position  */
#define QSPI_IFR_TFRTYP_Pos                   _U_(12)                                              /**< (QSPI_IFR) Data Transfer Type Position */
#define QSPI_IFR_TFRTYP_Msk                   (_U_(0x3) << QSPI_IFR_TFRTYP_Pos)                    /**< (QSPI_IFR) Data Transfer Type Mask */
#define QSPI_IFR_TFRTYP(value)                (QSPI_IFR_TFRTYP_Msk & ((value) << QSPI_IFR_TFRTYP_Pos))
#define   QSPI_IFR_TFRTYP_TRSFR_READ_Val      _U_(0x0)                                             /**< (QSPI_IFR) Read transfer from the serial memory.Scrambling is not performed.Read at random location (fetch) in the serial Flash memory is not possible.  */
#define   QSPI_IFR_TFRTYP_TRSFR_READ_MEMORY_Val _U_(0x1)                                             /**< (QSPI_IFR) Read data transfer from the serial memory.If enabled, scrambling is performed.Read at random location (fetch) in the serial Flash memory is possible.  */
#define   QSPI_IFR_TFRTYP_TRSFR_WRITE_Val     _U_(0x2)                                             /**< (QSPI_IFR) Write transfer into the serial memory.Scrambling is not performed.  */
#define   QSPI_IFR_TFRTYP_TRSFR_WRITE_MEMORY_Val _U_(0x3)                                             /**< (QSPI_IFR) Write data transfer into the serial memory.If enabled, scrambling is performed.  */
#define QSPI_IFR_TFRTYP_TRSFR_READ            (QSPI_IFR_TFRTYP_TRSFR_READ_Val << QSPI_IFR_TFRTYP_Pos) /**< (QSPI_IFR) Read transfer from the serial memory.Scrambling is not performed.Read at random location (fetch) in the serial Flash memory is not possible. Position  */
#define QSPI_IFR_TFRTYP_TRSFR_READ_MEMORY     (QSPI_IFR_TFRTYP_TRSFR_READ_MEMORY_Val << QSPI_IFR_TFRTYP_Pos) /**< (QSPI_IFR) Read data transfer from the serial memory.If enabled, scrambling is performed.Read at random location (fetch) in the serial Flash memory is possible. Position  */
#define QSPI_IFR_TFRTYP_TRSFR_WRITE           (QSPI_IFR_TFRTYP_TRSFR_WRITE_Val << QSPI_IFR_TFRTYP_Pos) /**< (QSPI_IFR) Write transfer into the serial memory.Scrambling is not performed. Position  */
#define QSPI_IFR_TFRTYP_TRSFR_WRITE_MEMORY    (QSPI_IFR_TFRTYP_TRSFR_WRITE_MEMORY_Val << QSPI_IFR_TFRTYP_Pos) /**< (QSPI_IFR) Write data transfer into the serial memory.If enabled, scrambling is performed. Position  */
#define QSPI_IFR_CRM_Pos                      _U_(14)                                              /**< (QSPI_IFR) Continuous Read Mode Position */
#define QSPI_IFR_CRM_Msk                      (_U_(0x1) << QSPI_IFR_CRM_Pos)                       /**< (QSPI_IFR) Continuous Read Mode Mask */
#define QSPI_IFR_CRM(value)                   (QSPI_IFR_CRM_Msk & ((value) << QSPI_IFR_CRM_Pos))  
#define   QSPI_IFR_CRM_DISABLED_Val           _U_(0x0)                                             /**< (QSPI_IFR) The Continuous Read mode is disabled.  */
#define   QSPI_IFR_CRM_ENABLED_Val            _U_(0x1)                                             /**< (QSPI_IFR) The Continuous Read mode is enabled.  */
#define QSPI_IFR_CRM_DISABLED                 (QSPI_IFR_CRM_DISABLED_Val << QSPI_IFR_CRM_Pos)      /**< (QSPI_IFR) The Continuous Read mode is disabled. Position  */
#define QSPI_IFR_CRM_ENABLED                  (QSPI_IFR_CRM_ENABLED_Val << QSPI_IFR_CRM_Pos)       /**< (QSPI_IFR) The Continuous Read mode is enabled. Position  */
#define QSPI_IFR_NBDUM_Pos                    _U_(16)                                              /**< (QSPI_IFR) Number Of Dummy Cycles Position */
#define QSPI_IFR_NBDUM_Msk                    (_U_(0x1F) << QSPI_IFR_NBDUM_Pos)                    /**< (QSPI_IFR) Number Of Dummy Cycles Mask */
#define QSPI_IFR_NBDUM(value)                 (QSPI_IFR_NBDUM_Msk & ((value) << QSPI_IFR_NBDUM_Pos))
#define QSPI_IFR_Msk                          _U_(0x001F77F7)                                      /**< (QSPI_IFR) Register Mask  */


/* -------- QSPI_SMR : (QSPI Offset: 0x40) (R/W 32) Scrambling Mode Register -------- */
#define QSPI_SMR_SCREN_Pos                    _U_(0)                                               /**< (QSPI_SMR) Scrambling/Unscrambling Enable Position */
#define QSPI_SMR_SCREN_Msk                    (_U_(0x1) << QSPI_SMR_SCREN_Pos)                     /**< (QSPI_SMR) Scrambling/Unscrambling Enable Mask */
#define QSPI_SMR_SCREN(value)                 (QSPI_SMR_SCREN_Msk & ((value) << QSPI_SMR_SCREN_Pos))
#define   QSPI_SMR_SCREN_DISABLED_Val         _U_(0x0)                                             /**< (QSPI_SMR) The scrambling/unscrambling is disabled.  */
#define   QSPI_SMR_SCREN_ENABLED_Val          _U_(0x1)                                             /**< (QSPI_SMR) The scrambling/unscrambling is enabled.  */
#define QSPI_SMR_SCREN_DISABLED               (QSPI_SMR_SCREN_DISABLED_Val << QSPI_SMR_SCREN_Pos)  /**< (QSPI_SMR) The scrambling/unscrambling is disabled. Position  */
#define QSPI_SMR_SCREN_ENABLED                (QSPI_SMR_SCREN_ENABLED_Val << QSPI_SMR_SCREN_Pos)   /**< (QSPI_SMR) The scrambling/unscrambling is enabled. Position  */
#define QSPI_SMR_RVDIS_Pos                    _U_(1)                                               /**< (QSPI_SMR) Scrambling/Unscrambling Random Value Disable Position */
#define QSPI_SMR_RVDIS_Msk                    (_U_(0x1) << QSPI_SMR_RVDIS_Pos)                     /**< (QSPI_SMR) Scrambling/Unscrambling Random Value Disable Mask */
#define QSPI_SMR_RVDIS(value)                 (QSPI_SMR_RVDIS_Msk & ((value) << QSPI_SMR_RVDIS_Pos))
#define QSPI_SMR_Msk                          _U_(0x00000003)                                      /**< (QSPI_SMR) Register Mask  */


/* -------- QSPI_SKR : (QSPI Offset: 0x44) ( /W 32) Scrambling Key Register -------- */
#define QSPI_SKR_USRK_Pos                     _U_(0)                                               /**< (QSPI_SKR) User Scrambling Key Position */
#define QSPI_SKR_USRK_Msk                     (_U_(0xFFFFFFFF) << QSPI_SKR_USRK_Pos)               /**< (QSPI_SKR) User Scrambling Key Mask */
#define QSPI_SKR_USRK(value)                  (QSPI_SKR_USRK_Msk & ((value) << QSPI_SKR_USRK_Pos))
#define QSPI_SKR_Msk                          _U_(0xFFFFFFFF)                                      /**< (QSPI_SKR) Register Mask  */


/* -------- QSPI_WPMR : (QSPI Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define QSPI_WPMR_WPEN_Pos                    _U_(0)                                               /**< (QSPI_WPMR) Write Protection Enable Position */
#define QSPI_WPMR_WPEN_Msk                    (_U_(0x1) << QSPI_WPMR_WPEN_Pos)                     /**< (QSPI_WPMR) Write Protection Enable Mask */
#define QSPI_WPMR_WPEN(value)                 (QSPI_WPMR_WPEN_Msk & ((value) << QSPI_WPMR_WPEN_Pos))
#define QSPI_WPMR_WPKEY_Pos                   _U_(8)                                               /**< (QSPI_WPMR) Write Protection Key Position */
#define QSPI_WPMR_WPKEY_Msk                   (_U_(0xFFFFFF) << QSPI_WPMR_WPKEY_Pos)               /**< (QSPI_WPMR) Write Protection Key Mask */
#define QSPI_WPMR_WPKEY(value)                (QSPI_WPMR_WPKEY_Msk & ((value) << QSPI_WPMR_WPKEY_Pos))
#define   QSPI_WPMR_WPKEY_PASSWD_Val          _U_(0x515350)                                        /**< (QSPI_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0.  */
#define QSPI_WPMR_WPKEY_PASSWD                (QSPI_WPMR_WPKEY_PASSWD_Val << QSPI_WPMR_WPKEY_Pos)  /**< (QSPI_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0. Position  */
#define QSPI_WPMR_Msk                         _U_(0xFFFFFF01)                                      /**< (QSPI_WPMR) Register Mask  */


/* -------- QSPI_WPSR : (QSPI Offset: 0xE8) ( R/ 32) Write Protection Status Register -------- */
#define QSPI_WPSR_WPVS_Pos                    _U_(0)                                               /**< (QSPI_WPSR) Write Protection Violation Status Position */
#define QSPI_WPSR_WPVS_Msk                    (_U_(0x1) << QSPI_WPSR_WPVS_Pos)                     /**< (QSPI_WPSR) Write Protection Violation Status Mask */
#define QSPI_WPSR_WPVS(value)                 (QSPI_WPSR_WPVS_Msk & ((value) << QSPI_WPSR_WPVS_Pos))
#define QSPI_WPSR_WPVSRC_Pos                  _U_(8)                                               /**< (QSPI_WPSR) Write Protection Violation Source Position */
#define QSPI_WPSR_WPVSRC_Msk                  (_U_(0xFF) << QSPI_WPSR_WPVSRC_Pos)                  /**< (QSPI_WPSR) Write Protection Violation Source Mask */
#define QSPI_WPSR_WPVSRC(value)               (QSPI_WPSR_WPVSRC_Msk & ((value) << QSPI_WPSR_WPVSRC_Pos))
#define QSPI_WPSR_Msk                         _U_(0x0000FF01)                                      /**< (QSPI_WPSR) Register Mask  */


/** \brief QSPI register offsets definitions */
#define QSPI_CR_REG_OFST               (0x00)              /**< (QSPI_CR) Control Register Offset */
#define QSPI_MR_REG_OFST               (0x04)              /**< (QSPI_MR) Mode Register Offset */
#define QSPI_RDR_REG_OFST              (0x08)              /**< (QSPI_RDR) Receive Data Register Offset */
#define QSPI_TDR_REG_OFST              (0x0C)              /**< (QSPI_TDR) Transmit Data Register Offset */
#define QSPI_SR_REG_OFST               (0x10)              /**< (QSPI_SR) Status Register Offset */
#define QSPI_IER_REG_OFST              (0x14)              /**< (QSPI_IER) Interrupt Enable Register Offset */
#define QSPI_IDR_REG_OFST              (0x18)              /**< (QSPI_IDR) Interrupt Disable Register Offset */
#define QSPI_IMR_REG_OFST              (0x1C)              /**< (QSPI_IMR) Interrupt Mask Register Offset */
#define QSPI_SCR_REG_OFST              (0x20)              /**< (QSPI_SCR) Serial Clock Register Offset */
#define QSPI_IAR_REG_OFST              (0x30)              /**< (QSPI_IAR) Instruction Address Register Offset */
#define QSPI_ICR_REG_OFST              (0x34)              /**< (QSPI_ICR) Instruction Code Register Offset */
#define QSPI_IFR_REG_OFST              (0x38)              /**< (QSPI_IFR) Instruction Frame Register Offset */
#define QSPI_SMR_REG_OFST              (0x40)              /**< (QSPI_SMR) Scrambling Mode Register Offset */
#define QSPI_SKR_REG_OFST              (0x44)              /**< (QSPI_SKR) Scrambling Key Register Offset */
#define QSPI_WPMR_REG_OFST             (0xE4)              /**< (QSPI_WPMR) Write Protection Mode Register Offset */
#define QSPI_WPSR_REG_OFST             (0xE8)              /**< (QSPI_WPSR) Write Protection Status Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief QSPI register API structure */
typedef struct
{
  __O   uint32_t                       QSPI_CR;            /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       QSPI_MR;            /**< Offset: 0x04 (R/W  32) Mode Register */
  __I   uint32_t                       QSPI_RDR;           /**< Offset: 0x08 (R/   32) Receive Data Register */
  __O   uint32_t                       QSPI_TDR;           /**< Offset: 0x0C ( /W  32) Transmit Data Register */
  __I   uint32_t                       QSPI_SR;            /**< Offset: 0x10 (R/   32) Status Register */
  __O   uint32_t                       QSPI_IER;           /**< Offset: 0x14 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       QSPI_IDR;           /**< Offset: 0x18 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       QSPI_IMR;           /**< Offset: 0x1C (R/   32) Interrupt Mask Register */
  __IO  uint32_t                       QSPI_SCR;           /**< Offset: 0x20 (R/W  32) Serial Clock Register */
  __I   uint8_t                        Reserved1[0x0C];
  __IO  uint32_t                       QSPI_IAR;           /**< Offset: 0x30 (R/W  32) Instruction Address Register */
  __IO  uint32_t                       QSPI_ICR;           /**< Offset: 0x34 (R/W  32) Instruction Code Register */
  __IO  uint32_t                       QSPI_IFR;           /**< Offset: 0x38 (R/W  32) Instruction Frame Register */
  __I   uint8_t                        Reserved2[0x04];
  __IO  uint32_t                       QSPI_SMR;           /**< Offset: 0x40 (R/W  32) Scrambling Mode Register */
  __O   uint32_t                       QSPI_SKR;           /**< Offset: 0x44 ( /W  32) Scrambling Key Register */
  __I   uint8_t                        Reserved3[0x9C];
  __IO  uint32_t                       QSPI_WPMR;          /**< Offset: 0xE4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       QSPI_WPSR;          /**< Offset: 0xE8 (R/   32) Write Protection Status Register */
} qspi_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_QSPI_COMPONENT_H_ */
