/**
 * \brief Component description for SPI
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
#ifndef _SAME70_SPI_COMPONENT_H_
#define _SAME70_SPI_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR SPI                                          */
/* ************************************************************************** */

/* -------- SPI_CR : (SPI Offset: 0x00) ( /W 32) Control Register -------- */
#define SPI_CR_SPIEN_Pos                      _U_(0)                                               /**< (SPI_CR) SPI Enable Position */
#define SPI_CR_SPIEN_Msk                      (_U_(0x1) << SPI_CR_SPIEN_Pos)                       /**< (SPI_CR) SPI Enable Mask */
#define SPI_CR_SPIEN(value)                   (SPI_CR_SPIEN_Msk & ((value) << SPI_CR_SPIEN_Pos))  
#define SPI_CR_SPIDIS_Pos                     _U_(1)                                               /**< (SPI_CR) SPI Disable Position */
#define SPI_CR_SPIDIS_Msk                     (_U_(0x1) << SPI_CR_SPIDIS_Pos)                      /**< (SPI_CR) SPI Disable Mask */
#define SPI_CR_SPIDIS(value)                  (SPI_CR_SPIDIS_Msk & ((value) << SPI_CR_SPIDIS_Pos))
#define SPI_CR_SWRST_Pos                      _U_(7)                                               /**< (SPI_CR) SPI Software Reset Position */
#define SPI_CR_SWRST_Msk                      (_U_(0x1) << SPI_CR_SWRST_Pos)                       /**< (SPI_CR) SPI Software Reset Mask */
#define SPI_CR_SWRST(value)                   (SPI_CR_SWRST_Msk & ((value) << SPI_CR_SWRST_Pos))  
#define SPI_CR_REQCLR_Pos                     _U_(12)                                              /**< (SPI_CR) Request to Clear the Comparison Trigger Position */
#define SPI_CR_REQCLR_Msk                     (_U_(0x1) << SPI_CR_REQCLR_Pos)                      /**< (SPI_CR) Request to Clear the Comparison Trigger Mask */
#define SPI_CR_REQCLR(value)                  (SPI_CR_REQCLR_Msk & ((value) << SPI_CR_REQCLR_Pos))
#define SPI_CR_LASTXFER_Pos                   _U_(24)                                              /**< (SPI_CR) Last Transfer Position */
#define SPI_CR_LASTXFER_Msk                   (_U_(0x1) << SPI_CR_LASTXFER_Pos)                    /**< (SPI_CR) Last Transfer Mask */
#define SPI_CR_LASTXFER(value)                (SPI_CR_LASTXFER_Msk & ((value) << SPI_CR_LASTXFER_Pos))
#define SPI_CR_Msk                            _U_(0x01001083)                                      /**< (SPI_CR) Register Mask  */


/* -------- SPI_MR : (SPI Offset: 0x04) (R/W 32) Mode Register -------- */
#define SPI_MR_MSTR_Pos                       _U_(0)                                               /**< (SPI_MR) Master/Slave Mode Position */
#define SPI_MR_MSTR_Msk                       (_U_(0x1) << SPI_MR_MSTR_Pos)                        /**< (SPI_MR) Master/Slave Mode Mask */
#define SPI_MR_MSTR(value)                    (SPI_MR_MSTR_Msk & ((value) << SPI_MR_MSTR_Pos))    
#define   SPI_MR_MSTR_MASTER_Val              _U_(0x1)                                             /**< (SPI_MR) Master  */
#define   SPI_MR_MSTR_SLAVE_Val               _U_(0x0)                                             /**< (SPI_MR) Slave  */
#define SPI_MR_MSTR_MASTER                    (SPI_MR_MSTR_MASTER_Val << SPI_MR_MSTR_Pos)          /**< (SPI_MR) Master Position  */
#define SPI_MR_MSTR_SLAVE                     (SPI_MR_MSTR_SLAVE_Val << SPI_MR_MSTR_Pos)           /**< (SPI_MR) Slave Position  */
#define SPI_MR_PS_Pos                         _U_(1)                                               /**< (SPI_MR) Peripheral Select Position */
#define SPI_MR_PS_Msk                         (_U_(0x1) << SPI_MR_PS_Pos)                          /**< (SPI_MR) Peripheral Select Mask */
#define SPI_MR_PS(value)                      (SPI_MR_PS_Msk & ((value) << SPI_MR_PS_Pos))        
#define SPI_MR_PCSDEC_Pos                     _U_(2)                                               /**< (SPI_MR) Chip Select Decode Position */
#define SPI_MR_PCSDEC_Msk                     (_U_(0x1) << SPI_MR_PCSDEC_Pos)                      /**< (SPI_MR) Chip Select Decode Mask */
#define SPI_MR_PCSDEC(value)                  (SPI_MR_PCSDEC_Msk & ((value) << SPI_MR_PCSDEC_Pos))
#define SPI_MR_MODFDIS_Pos                    _U_(4)                                               /**< (SPI_MR) Mode Fault Detection Position */
#define SPI_MR_MODFDIS_Msk                    (_U_(0x1) << SPI_MR_MODFDIS_Pos)                     /**< (SPI_MR) Mode Fault Detection Mask */
#define SPI_MR_MODFDIS(value)                 (SPI_MR_MODFDIS_Msk & ((value) << SPI_MR_MODFDIS_Pos))
#define SPI_MR_WDRBT_Pos                      _U_(5)                                               /**< (SPI_MR) Wait Data Read Before Transfer Position */
#define SPI_MR_WDRBT_Msk                      (_U_(0x1) << SPI_MR_WDRBT_Pos)                       /**< (SPI_MR) Wait Data Read Before Transfer Mask */
#define SPI_MR_WDRBT(value)                   (SPI_MR_WDRBT_Msk & ((value) << SPI_MR_WDRBT_Pos))  
#define SPI_MR_LLB_Pos                        _U_(7)                                               /**< (SPI_MR) Local Loopback Enable Position */
#define SPI_MR_LLB_Msk                        (_U_(0x1) << SPI_MR_LLB_Pos)                         /**< (SPI_MR) Local Loopback Enable Mask */
#define SPI_MR_LLB(value)                     (SPI_MR_LLB_Msk & ((value) << SPI_MR_LLB_Pos))      
#define SPI_MR_PCS_Pos                        _U_(16)                                              /**< (SPI_MR) Peripheral Chip Select Position */
#define SPI_MR_PCS_Msk                        (_U_(0xF) << SPI_MR_PCS_Pos)                         /**< (SPI_MR) Peripheral Chip Select Mask */
#define SPI_MR_PCS(value)                     (SPI_MR_PCS_Msk & ((value) << SPI_MR_PCS_Pos))      
#define   SPI_MR_PCS_NPCS0_Val                _U_(0xE)                                             /**< (SPI_MR) NPCS0 as Chip Select  */
#define   SPI_MR_PCS_NPCS1_Val                _U_(0xD)                                             /**< (SPI_MR) NPCS1 as Chip Select  */
#define   SPI_MR_PCS_NPCS2_Val                _U_(0xB)                                             /**< (SPI_MR) NPCS2 as Chip Select  */
#define   SPI_MR_PCS_NPCS3_Val                _U_(0x7)                                             /**< (SPI_MR) NPCS3 as Chip Select  */
#define SPI_MR_PCS_NPCS0                      (SPI_MR_PCS_NPCS0_Val << SPI_MR_PCS_Pos)             /**< (SPI_MR) NPCS0 as Chip Select Position  */
#define SPI_MR_PCS_NPCS1                      (SPI_MR_PCS_NPCS1_Val << SPI_MR_PCS_Pos)             /**< (SPI_MR) NPCS1 as Chip Select Position  */
#define SPI_MR_PCS_NPCS2                      (SPI_MR_PCS_NPCS2_Val << SPI_MR_PCS_Pos)             /**< (SPI_MR) NPCS2 as Chip Select Position  */
#define SPI_MR_PCS_NPCS3                      (SPI_MR_PCS_NPCS3_Val << SPI_MR_PCS_Pos)             /**< (SPI_MR) NPCS3 as Chip Select Position  */
#define SPI_MR_DLYBCS_Pos                     _U_(24)                                              /**< (SPI_MR) Delay Between Chip Selects Position */
#define SPI_MR_DLYBCS_Msk                     (_U_(0xFF) << SPI_MR_DLYBCS_Pos)                     /**< (SPI_MR) Delay Between Chip Selects Mask */
#define SPI_MR_DLYBCS(value)                  (SPI_MR_DLYBCS_Msk & ((value) << SPI_MR_DLYBCS_Pos))
#define SPI_MR_Msk                            _U_(0xFF0F00B7)                                      /**< (SPI_MR) Register Mask  */


/* -------- SPI_RDR : (SPI Offset: 0x08) ( R/ 32) Receive Data Register -------- */
#define SPI_RDR_RD_Pos                        _U_(0)                                               /**< (SPI_RDR) Receive Data Position */
#define SPI_RDR_RD_Msk                        (_U_(0xFFFF) << SPI_RDR_RD_Pos)                      /**< (SPI_RDR) Receive Data Mask */
#define SPI_RDR_RD(value)                     (SPI_RDR_RD_Msk & ((value) << SPI_RDR_RD_Pos))      
#define SPI_RDR_PCS_Pos                       _U_(16)                                              /**< (SPI_RDR) Peripheral Chip Select Position */
#define SPI_RDR_PCS_Msk                       (_U_(0xF) << SPI_RDR_PCS_Pos)                        /**< (SPI_RDR) Peripheral Chip Select Mask */
#define SPI_RDR_PCS(value)                    (SPI_RDR_PCS_Msk & ((value) << SPI_RDR_PCS_Pos))    
#define SPI_RDR_Msk                           _U_(0x000FFFFF)                                      /**< (SPI_RDR) Register Mask  */


/* -------- SPI_TDR : (SPI Offset: 0x0C) ( /W 32) Transmit Data Register -------- */
#define SPI_TDR_TD_Pos                        _U_(0)                                               /**< (SPI_TDR) Transmit Data Position */
#define SPI_TDR_TD_Msk                        (_U_(0xFFFF) << SPI_TDR_TD_Pos)                      /**< (SPI_TDR) Transmit Data Mask */
#define SPI_TDR_TD(value)                     (SPI_TDR_TD_Msk & ((value) << SPI_TDR_TD_Pos))      
#define SPI_TDR_PCS_Pos                       _U_(16)                                              /**< (SPI_TDR) Peripheral Chip Select Position */
#define SPI_TDR_PCS_Msk                       (_U_(0xF) << SPI_TDR_PCS_Pos)                        /**< (SPI_TDR) Peripheral Chip Select Mask */
#define SPI_TDR_PCS(value)                    (SPI_TDR_PCS_Msk & ((value) << SPI_TDR_PCS_Pos))    
#define   SPI_TDR_PCS_NPCS0_Val               _U_(0xE)                                             /**< (SPI_TDR) NPCS0 as Chip Select  */
#define   SPI_TDR_PCS_NPCS1_Val               _U_(0xD)                                             /**< (SPI_TDR) NPCS1 as Chip Select  */
#define   SPI_TDR_PCS_NPCS2_Val               _U_(0xB)                                             /**< (SPI_TDR) NPCS2 as Chip Select  */
#define   SPI_TDR_PCS_NPCS3_Val               _U_(0x7)                                             /**< (SPI_TDR) NPCS3 as Chip Select  */
#define SPI_TDR_PCS_NPCS0                     (SPI_TDR_PCS_NPCS0_Val << SPI_TDR_PCS_Pos)           /**< (SPI_TDR) NPCS0 as Chip Select Position  */
#define SPI_TDR_PCS_NPCS1                     (SPI_TDR_PCS_NPCS1_Val << SPI_TDR_PCS_Pos)           /**< (SPI_TDR) NPCS1 as Chip Select Position  */
#define SPI_TDR_PCS_NPCS2                     (SPI_TDR_PCS_NPCS2_Val << SPI_TDR_PCS_Pos)           /**< (SPI_TDR) NPCS2 as Chip Select Position  */
#define SPI_TDR_PCS_NPCS3                     (SPI_TDR_PCS_NPCS3_Val << SPI_TDR_PCS_Pos)           /**< (SPI_TDR) NPCS3 as Chip Select Position  */
#define SPI_TDR_LASTXFER_Pos                  _U_(24)                                              /**< (SPI_TDR) Last Transfer Position */
#define SPI_TDR_LASTXFER_Msk                  (_U_(0x1) << SPI_TDR_LASTXFER_Pos)                   /**< (SPI_TDR) Last Transfer Mask */
#define SPI_TDR_LASTXFER(value)               (SPI_TDR_LASTXFER_Msk & ((value) << SPI_TDR_LASTXFER_Pos))
#define SPI_TDR_Msk                           _U_(0x010FFFFF)                                      /**< (SPI_TDR) Register Mask  */


/* -------- SPI_SR : (SPI Offset: 0x10) ( R/ 32) Status Register -------- */
#define SPI_SR_RDRF_Pos                       _U_(0)                                               /**< (SPI_SR) Receive Data Register Full (cleared by reading SPI_RDR) Position */
#define SPI_SR_RDRF_Msk                       (_U_(0x1) << SPI_SR_RDRF_Pos)                        /**< (SPI_SR) Receive Data Register Full (cleared by reading SPI_RDR) Mask */
#define SPI_SR_RDRF(value)                    (SPI_SR_RDRF_Msk & ((value) << SPI_SR_RDRF_Pos))    
#define SPI_SR_TDRE_Pos                       _U_(1)                                               /**< (SPI_SR) Transmit Data Register Empty (cleared by writing SPI_TDR) Position */
#define SPI_SR_TDRE_Msk                       (_U_(0x1) << SPI_SR_TDRE_Pos)                        /**< (SPI_SR) Transmit Data Register Empty (cleared by writing SPI_TDR) Mask */
#define SPI_SR_TDRE(value)                    (SPI_SR_TDRE_Msk & ((value) << SPI_SR_TDRE_Pos))    
#define SPI_SR_MODF_Pos                       _U_(2)                                               /**< (SPI_SR) Mode Fault Error (cleared on read) Position */
#define SPI_SR_MODF_Msk                       (_U_(0x1) << SPI_SR_MODF_Pos)                        /**< (SPI_SR) Mode Fault Error (cleared on read) Mask */
#define SPI_SR_MODF(value)                    (SPI_SR_MODF_Msk & ((value) << SPI_SR_MODF_Pos))    
#define SPI_SR_OVRES_Pos                      _U_(3)                                               /**< (SPI_SR) Overrun Error Status (cleared on read) Position */
#define SPI_SR_OVRES_Msk                      (_U_(0x1) << SPI_SR_OVRES_Pos)                       /**< (SPI_SR) Overrun Error Status (cleared on read) Mask */
#define SPI_SR_OVRES(value)                   (SPI_SR_OVRES_Msk & ((value) << SPI_SR_OVRES_Pos))  
#define SPI_SR_NSSR_Pos                       _U_(8)                                               /**< (SPI_SR) NSS Rising (cleared on read) Position */
#define SPI_SR_NSSR_Msk                       (_U_(0x1) << SPI_SR_NSSR_Pos)                        /**< (SPI_SR) NSS Rising (cleared on read) Mask */
#define SPI_SR_NSSR(value)                    (SPI_SR_NSSR_Msk & ((value) << SPI_SR_NSSR_Pos))    
#define SPI_SR_TXEMPTY_Pos                    _U_(9)                                               /**< (SPI_SR) Transmission Registers Empty (cleared by writing SPI_TDR) Position */
#define SPI_SR_TXEMPTY_Msk                    (_U_(0x1) << SPI_SR_TXEMPTY_Pos)                     /**< (SPI_SR) Transmission Registers Empty (cleared by writing SPI_TDR) Mask */
#define SPI_SR_TXEMPTY(value)                 (SPI_SR_TXEMPTY_Msk & ((value) << SPI_SR_TXEMPTY_Pos))
#define SPI_SR_UNDES_Pos                      _U_(10)                                              /**< (SPI_SR) Underrun Error Status (Slave mode only) (cleared on read) Position */
#define SPI_SR_UNDES_Msk                      (_U_(0x1) << SPI_SR_UNDES_Pos)                       /**< (SPI_SR) Underrun Error Status (Slave mode only) (cleared on read) Mask */
#define SPI_SR_UNDES(value)                   (SPI_SR_UNDES_Msk & ((value) << SPI_SR_UNDES_Pos))  
#define SPI_SR_SPIENS_Pos                     _U_(16)                                              /**< (SPI_SR) SPI Enable Status Position */
#define SPI_SR_SPIENS_Msk                     (_U_(0x1) << SPI_SR_SPIENS_Pos)                      /**< (SPI_SR) SPI Enable Status Mask */
#define SPI_SR_SPIENS(value)                  (SPI_SR_SPIENS_Msk & ((value) << SPI_SR_SPIENS_Pos))
#define SPI_SR_Msk                            _U_(0x0001070F)                                      /**< (SPI_SR) Register Mask  */


/* -------- SPI_IER : (SPI Offset: 0x14) ( /W 32) Interrupt Enable Register -------- */
#define SPI_IER_RDRF_Pos                      _U_(0)                                               /**< (SPI_IER) Receive Data Register Full Interrupt Enable Position */
#define SPI_IER_RDRF_Msk                      (_U_(0x1) << SPI_IER_RDRF_Pos)                       /**< (SPI_IER) Receive Data Register Full Interrupt Enable Mask */
#define SPI_IER_RDRF(value)                   (SPI_IER_RDRF_Msk & ((value) << SPI_IER_RDRF_Pos))  
#define SPI_IER_TDRE_Pos                      _U_(1)                                               /**< (SPI_IER) SPI Transmit Data Register Empty Interrupt Enable Position */
#define SPI_IER_TDRE_Msk                      (_U_(0x1) << SPI_IER_TDRE_Pos)                       /**< (SPI_IER) SPI Transmit Data Register Empty Interrupt Enable Mask */
#define SPI_IER_TDRE(value)                   (SPI_IER_TDRE_Msk & ((value) << SPI_IER_TDRE_Pos))  
#define SPI_IER_MODF_Pos                      _U_(2)                                               /**< (SPI_IER) Mode Fault Error Interrupt Enable Position */
#define SPI_IER_MODF_Msk                      (_U_(0x1) << SPI_IER_MODF_Pos)                       /**< (SPI_IER) Mode Fault Error Interrupt Enable Mask */
#define SPI_IER_MODF(value)                   (SPI_IER_MODF_Msk & ((value) << SPI_IER_MODF_Pos))  
#define SPI_IER_OVRES_Pos                     _U_(3)                                               /**< (SPI_IER) Overrun Error Interrupt Enable Position */
#define SPI_IER_OVRES_Msk                     (_U_(0x1) << SPI_IER_OVRES_Pos)                      /**< (SPI_IER) Overrun Error Interrupt Enable Mask */
#define SPI_IER_OVRES(value)                  (SPI_IER_OVRES_Msk & ((value) << SPI_IER_OVRES_Pos))
#define SPI_IER_NSSR_Pos                      _U_(8)                                               /**< (SPI_IER) NSS Rising Interrupt Enable Position */
#define SPI_IER_NSSR_Msk                      (_U_(0x1) << SPI_IER_NSSR_Pos)                       /**< (SPI_IER) NSS Rising Interrupt Enable Mask */
#define SPI_IER_NSSR(value)                   (SPI_IER_NSSR_Msk & ((value) << SPI_IER_NSSR_Pos))  
#define SPI_IER_TXEMPTY_Pos                   _U_(9)                                               /**< (SPI_IER) Transmission Registers Empty Enable Position */
#define SPI_IER_TXEMPTY_Msk                   (_U_(0x1) << SPI_IER_TXEMPTY_Pos)                    /**< (SPI_IER) Transmission Registers Empty Enable Mask */
#define SPI_IER_TXEMPTY(value)                (SPI_IER_TXEMPTY_Msk & ((value) << SPI_IER_TXEMPTY_Pos))
#define SPI_IER_UNDES_Pos                     _U_(10)                                              /**< (SPI_IER) Underrun Error Interrupt Enable Position */
#define SPI_IER_UNDES_Msk                     (_U_(0x1) << SPI_IER_UNDES_Pos)                      /**< (SPI_IER) Underrun Error Interrupt Enable Mask */
#define SPI_IER_UNDES(value)                  (SPI_IER_UNDES_Msk & ((value) << SPI_IER_UNDES_Pos))
#define SPI_IER_Msk                           _U_(0x0000070F)                                      /**< (SPI_IER) Register Mask  */


/* -------- SPI_IDR : (SPI Offset: 0x18) ( /W 32) Interrupt Disable Register -------- */
#define SPI_IDR_RDRF_Pos                      _U_(0)                                               /**< (SPI_IDR) Receive Data Register Full Interrupt Disable Position */
#define SPI_IDR_RDRF_Msk                      (_U_(0x1) << SPI_IDR_RDRF_Pos)                       /**< (SPI_IDR) Receive Data Register Full Interrupt Disable Mask */
#define SPI_IDR_RDRF(value)                   (SPI_IDR_RDRF_Msk & ((value) << SPI_IDR_RDRF_Pos))  
#define SPI_IDR_TDRE_Pos                      _U_(1)                                               /**< (SPI_IDR) SPI Transmit Data Register Empty Interrupt Disable Position */
#define SPI_IDR_TDRE_Msk                      (_U_(0x1) << SPI_IDR_TDRE_Pos)                       /**< (SPI_IDR) SPI Transmit Data Register Empty Interrupt Disable Mask */
#define SPI_IDR_TDRE(value)                   (SPI_IDR_TDRE_Msk & ((value) << SPI_IDR_TDRE_Pos))  
#define SPI_IDR_MODF_Pos                      _U_(2)                                               /**< (SPI_IDR) Mode Fault Error Interrupt Disable Position */
#define SPI_IDR_MODF_Msk                      (_U_(0x1) << SPI_IDR_MODF_Pos)                       /**< (SPI_IDR) Mode Fault Error Interrupt Disable Mask */
#define SPI_IDR_MODF(value)                   (SPI_IDR_MODF_Msk & ((value) << SPI_IDR_MODF_Pos))  
#define SPI_IDR_OVRES_Pos                     _U_(3)                                               /**< (SPI_IDR) Overrun Error Interrupt Disable Position */
#define SPI_IDR_OVRES_Msk                     (_U_(0x1) << SPI_IDR_OVRES_Pos)                      /**< (SPI_IDR) Overrun Error Interrupt Disable Mask */
#define SPI_IDR_OVRES(value)                  (SPI_IDR_OVRES_Msk & ((value) << SPI_IDR_OVRES_Pos))
#define SPI_IDR_NSSR_Pos                      _U_(8)                                               /**< (SPI_IDR) NSS Rising Interrupt Disable Position */
#define SPI_IDR_NSSR_Msk                      (_U_(0x1) << SPI_IDR_NSSR_Pos)                       /**< (SPI_IDR) NSS Rising Interrupt Disable Mask */
#define SPI_IDR_NSSR(value)                   (SPI_IDR_NSSR_Msk & ((value) << SPI_IDR_NSSR_Pos))  
#define SPI_IDR_TXEMPTY_Pos                   _U_(9)                                               /**< (SPI_IDR) Transmission Registers Empty Disable Position */
#define SPI_IDR_TXEMPTY_Msk                   (_U_(0x1) << SPI_IDR_TXEMPTY_Pos)                    /**< (SPI_IDR) Transmission Registers Empty Disable Mask */
#define SPI_IDR_TXEMPTY(value)                (SPI_IDR_TXEMPTY_Msk & ((value) << SPI_IDR_TXEMPTY_Pos))
#define SPI_IDR_UNDES_Pos                     _U_(10)                                              /**< (SPI_IDR) Underrun Error Interrupt Disable Position */
#define SPI_IDR_UNDES_Msk                     (_U_(0x1) << SPI_IDR_UNDES_Pos)                      /**< (SPI_IDR) Underrun Error Interrupt Disable Mask */
#define SPI_IDR_UNDES(value)                  (SPI_IDR_UNDES_Msk & ((value) << SPI_IDR_UNDES_Pos))
#define SPI_IDR_Msk                           _U_(0x0000070F)                                      /**< (SPI_IDR) Register Mask  */


/* -------- SPI_IMR : (SPI Offset: 0x1C) ( R/ 32) Interrupt Mask Register -------- */
#define SPI_IMR_RDRF_Pos                      _U_(0)                                               /**< (SPI_IMR) Receive Data Register Full Interrupt Mask Position */
#define SPI_IMR_RDRF_Msk                      (_U_(0x1) << SPI_IMR_RDRF_Pos)                       /**< (SPI_IMR) Receive Data Register Full Interrupt Mask Mask */
#define SPI_IMR_RDRF(value)                   (SPI_IMR_RDRF_Msk & ((value) << SPI_IMR_RDRF_Pos))  
#define SPI_IMR_TDRE_Pos                      _U_(1)                                               /**< (SPI_IMR) SPI Transmit Data Register Empty Interrupt Mask Position */
#define SPI_IMR_TDRE_Msk                      (_U_(0x1) << SPI_IMR_TDRE_Pos)                       /**< (SPI_IMR) SPI Transmit Data Register Empty Interrupt Mask Mask */
#define SPI_IMR_TDRE(value)                   (SPI_IMR_TDRE_Msk & ((value) << SPI_IMR_TDRE_Pos))  
#define SPI_IMR_MODF_Pos                      _U_(2)                                               /**< (SPI_IMR) Mode Fault Error Interrupt Mask Position */
#define SPI_IMR_MODF_Msk                      (_U_(0x1) << SPI_IMR_MODF_Pos)                       /**< (SPI_IMR) Mode Fault Error Interrupt Mask Mask */
#define SPI_IMR_MODF(value)                   (SPI_IMR_MODF_Msk & ((value) << SPI_IMR_MODF_Pos))  
#define SPI_IMR_OVRES_Pos                     _U_(3)                                               /**< (SPI_IMR) Overrun Error Interrupt Mask Position */
#define SPI_IMR_OVRES_Msk                     (_U_(0x1) << SPI_IMR_OVRES_Pos)                      /**< (SPI_IMR) Overrun Error Interrupt Mask Mask */
#define SPI_IMR_OVRES(value)                  (SPI_IMR_OVRES_Msk & ((value) << SPI_IMR_OVRES_Pos))
#define SPI_IMR_NSSR_Pos                      _U_(8)                                               /**< (SPI_IMR) NSS Rising Interrupt Mask Position */
#define SPI_IMR_NSSR_Msk                      (_U_(0x1) << SPI_IMR_NSSR_Pos)                       /**< (SPI_IMR) NSS Rising Interrupt Mask Mask */
#define SPI_IMR_NSSR(value)                   (SPI_IMR_NSSR_Msk & ((value) << SPI_IMR_NSSR_Pos))  
#define SPI_IMR_TXEMPTY_Pos                   _U_(9)                                               /**< (SPI_IMR) Transmission Registers Empty Mask Position */
#define SPI_IMR_TXEMPTY_Msk                   (_U_(0x1) << SPI_IMR_TXEMPTY_Pos)                    /**< (SPI_IMR) Transmission Registers Empty Mask Mask */
#define SPI_IMR_TXEMPTY(value)                (SPI_IMR_TXEMPTY_Msk & ((value) << SPI_IMR_TXEMPTY_Pos))
#define SPI_IMR_UNDES_Pos                     _U_(10)                                              /**< (SPI_IMR) Underrun Error Interrupt Mask Position */
#define SPI_IMR_UNDES_Msk                     (_U_(0x1) << SPI_IMR_UNDES_Pos)                      /**< (SPI_IMR) Underrun Error Interrupt Mask Mask */
#define SPI_IMR_UNDES(value)                  (SPI_IMR_UNDES_Msk & ((value) << SPI_IMR_UNDES_Pos))
#define SPI_IMR_Msk                           _U_(0x0000070F)                                      /**< (SPI_IMR) Register Mask  */


/* -------- SPI_CSR : (SPI Offset: 0x30) (R/W 32) Chip Select Register -------- */
#define SPI_CSR_CPOL_Pos                      _U_(0)                                               /**< (SPI_CSR) Clock Polarity Position */
#define SPI_CSR_CPOL_Msk                      (_U_(0x1) << SPI_CSR_CPOL_Pos)                       /**< (SPI_CSR) Clock Polarity Mask */
#define SPI_CSR_CPOL(value)                   (SPI_CSR_CPOL_Msk & ((value) << SPI_CSR_CPOL_Pos))  
#define   SPI_CSR_CPOL_IDLE_LOW_Val           _U_(0x0)                                             /**< (SPI_CSR) Clock is low when inactive (CPOL=0)  */
#define   SPI_CSR_CPOL_IDLE_HIGH_Val          _U_(0x1)                                             /**< (SPI_CSR) Clock is high when inactive (CPOL=1)  */
#define SPI_CSR_CPOL_IDLE_LOW                 (SPI_CSR_CPOL_IDLE_LOW_Val << SPI_CSR_CPOL_Pos)      /**< (SPI_CSR) Clock is low when inactive (CPOL=0) Position  */
#define SPI_CSR_CPOL_IDLE_HIGH                (SPI_CSR_CPOL_IDLE_HIGH_Val << SPI_CSR_CPOL_Pos)     /**< (SPI_CSR) Clock is high when inactive (CPOL=1) Position  */
#define SPI_CSR_NCPHA_Pos                     _U_(1)                                               /**< (SPI_CSR) Clock Phase Position */
#define SPI_CSR_NCPHA_Msk                     (_U_(0x1) << SPI_CSR_NCPHA_Pos)                      /**< (SPI_CSR) Clock Phase Mask */
#define SPI_CSR_NCPHA(value)                  (SPI_CSR_NCPHA_Msk & ((value) << SPI_CSR_NCPHA_Pos))
#define   SPI_CSR_NCPHA_VALID_LEADING_EDGE_Val _U_(0x1)                                             /**< (SPI_CSR) Data is valid on clock leading edge (CPHA=0)  */
#define   SPI_CSR_NCPHA_VALID_TRAILING_EDGE_Val _U_(0x0)                                             /**< (SPI_CSR) Data is valid on clock trailing edge (CPHA=1)  */
#define SPI_CSR_NCPHA_VALID_LEADING_EDGE      (SPI_CSR_NCPHA_VALID_LEADING_EDGE_Val << SPI_CSR_NCPHA_Pos) /**< (SPI_CSR) Data is valid on clock leading edge (CPHA=0) Position  */
#define SPI_CSR_NCPHA_VALID_TRAILING_EDGE     (SPI_CSR_NCPHA_VALID_TRAILING_EDGE_Val << SPI_CSR_NCPHA_Pos) /**< (SPI_CSR) Data is valid on clock trailing edge (CPHA=1) Position  */
#define SPI_CSR_CSNAAT_Pos                    _U_(2)                                               /**< (SPI_CSR) Chip Select Not Active After Transfer (Ignored if CSAAT = 1) Position */
#define SPI_CSR_CSNAAT_Msk                    (_U_(0x1) << SPI_CSR_CSNAAT_Pos)                     /**< (SPI_CSR) Chip Select Not Active After Transfer (Ignored if CSAAT = 1) Mask */
#define SPI_CSR_CSNAAT(value)                 (SPI_CSR_CSNAAT_Msk & ((value) << SPI_CSR_CSNAAT_Pos))
#define SPI_CSR_CSAAT_Pos                     _U_(3)                                               /**< (SPI_CSR) Chip Select Active After Transfer Position */
#define SPI_CSR_CSAAT_Msk                     (_U_(0x1) << SPI_CSR_CSAAT_Pos)                      /**< (SPI_CSR) Chip Select Active After Transfer Mask */
#define SPI_CSR_CSAAT(value)                  (SPI_CSR_CSAAT_Msk & ((value) << SPI_CSR_CSAAT_Pos))
#define SPI_CSR_BITS_Pos                      _U_(4)                                               /**< (SPI_CSR) Bits Per Transfer Position */
#define SPI_CSR_BITS_Msk                      (_U_(0xF) << SPI_CSR_BITS_Pos)                       /**< (SPI_CSR) Bits Per Transfer Mask */
#define SPI_CSR_BITS(value)                   (SPI_CSR_BITS_Msk & ((value) << SPI_CSR_BITS_Pos))  
#define   SPI_CSR_BITS_8_BIT_Val              _U_(0x0)                                             /**< (SPI_CSR) 8 bits for transfer  */
#define   SPI_CSR_BITS_9_BIT_Val              _U_(0x1)                                             /**< (SPI_CSR) 9 bits for transfer  */
#define   SPI_CSR_BITS_10_BIT_Val             _U_(0x2)                                             /**< (SPI_CSR) 10 bits for transfer  */
#define   SPI_CSR_BITS_11_BIT_Val             _U_(0x3)                                             /**< (SPI_CSR) 11 bits for transfer  */
#define   SPI_CSR_BITS_12_BIT_Val             _U_(0x4)                                             /**< (SPI_CSR) 12 bits for transfer  */
#define   SPI_CSR_BITS_13_BIT_Val             _U_(0x5)                                             /**< (SPI_CSR) 13 bits for transfer  */
#define   SPI_CSR_BITS_14_BIT_Val             _U_(0x6)                                             /**< (SPI_CSR) 14 bits for transfer  */
#define   SPI_CSR_BITS_15_BIT_Val             _U_(0x7)                                             /**< (SPI_CSR) 15 bits for transfer  */
#define   SPI_CSR_BITS_16_BIT_Val             _U_(0x8)                                             /**< (SPI_CSR) 16 bits for transfer  */
#define SPI_CSR_BITS_8_BIT                    (SPI_CSR_BITS_8_BIT_Val << SPI_CSR_BITS_Pos)         /**< (SPI_CSR) 8 bits for transfer Position  */
#define SPI_CSR_BITS_9_BIT                    (SPI_CSR_BITS_9_BIT_Val << SPI_CSR_BITS_Pos)         /**< (SPI_CSR) 9 bits for transfer Position  */
#define SPI_CSR_BITS_10_BIT                   (SPI_CSR_BITS_10_BIT_Val << SPI_CSR_BITS_Pos)        /**< (SPI_CSR) 10 bits for transfer Position  */
#define SPI_CSR_BITS_11_BIT                   (SPI_CSR_BITS_11_BIT_Val << SPI_CSR_BITS_Pos)        /**< (SPI_CSR) 11 bits for transfer Position  */
#define SPI_CSR_BITS_12_BIT                   (SPI_CSR_BITS_12_BIT_Val << SPI_CSR_BITS_Pos)        /**< (SPI_CSR) 12 bits for transfer Position  */
#define SPI_CSR_BITS_13_BIT                   (SPI_CSR_BITS_13_BIT_Val << SPI_CSR_BITS_Pos)        /**< (SPI_CSR) 13 bits for transfer Position  */
#define SPI_CSR_BITS_14_BIT                   (SPI_CSR_BITS_14_BIT_Val << SPI_CSR_BITS_Pos)        /**< (SPI_CSR) 14 bits for transfer Position  */
#define SPI_CSR_BITS_15_BIT                   (SPI_CSR_BITS_15_BIT_Val << SPI_CSR_BITS_Pos)        /**< (SPI_CSR) 15 bits for transfer Position  */
#define SPI_CSR_BITS_16_BIT                   (SPI_CSR_BITS_16_BIT_Val << SPI_CSR_BITS_Pos)        /**< (SPI_CSR) 16 bits for transfer Position  */
#define SPI_CSR_SCBR_Pos                      _U_(8)                                               /**< (SPI_CSR) Serial Clock Bit Rate Position */
#define SPI_CSR_SCBR_Msk                      (_U_(0xFF) << SPI_CSR_SCBR_Pos)                      /**< (SPI_CSR) Serial Clock Bit Rate Mask */
#define SPI_CSR_SCBR(value)                   (SPI_CSR_SCBR_Msk & ((value) << SPI_CSR_SCBR_Pos))  
#define SPI_CSR_DLYBS_Pos                     _U_(16)                                              /**< (SPI_CSR) Delay Before SPCK Position */
#define SPI_CSR_DLYBS_Msk                     (_U_(0xFF) << SPI_CSR_DLYBS_Pos)                     /**< (SPI_CSR) Delay Before SPCK Mask */
#define SPI_CSR_DLYBS(value)                  (SPI_CSR_DLYBS_Msk & ((value) << SPI_CSR_DLYBS_Pos))
#define SPI_CSR_DLYBCT_Pos                    _U_(24)                                              /**< (SPI_CSR) Delay Between Consecutive Transfers Position */
#define SPI_CSR_DLYBCT_Msk                    (_U_(0xFF) << SPI_CSR_DLYBCT_Pos)                    /**< (SPI_CSR) Delay Between Consecutive Transfers Mask */
#define SPI_CSR_DLYBCT(value)                 (SPI_CSR_DLYBCT_Msk & ((value) << SPI_CSR_DLYBCT_Pos))
#define SPI_CSR_Msk                           _U_(0xFFFFFFFF)                                      /**< (SPI_CSR) Register Mask  */


/* -------- SPI_WPMR : (SPI Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define SPI_WPMR_WPEN_Pos                     _U_(0)                                               /**< (SPI_WPMR) Write Protection Enable Position */
#define SPI_WPMR_WPEN_Msk                     (_U_(0x1) << SPI_WPMR_WPEN_Pos)                      /**< (SPI_WPMR) Write Protection Enable Mask */
#define SPI_WPMR_WPEN(value)                  (SPI_WPMR_WPEN_Msk & ((value) << SPI_WPMR_WPEN_Pos))
#define SPI_WPMR_WPKEY_Pos                    _U_(8)                                               /**< (SPI_WPMR) Write Protection Key Position */
#define SPI_WPMR_WPKEY_Msk                    (_U_(0xFFFFFF) << SPI_WPMR_WPKEY_Pos)                /**< (SPI_WPMR) Write Protection Key Mask */
#define SPI_WPMR_WPKEY(value)                 (SPI_WPMR_WPKEY_Msk & ((value) << SPI_WPMR_WPKEY_Pos))
#define   SPI_WPMR_WPKEY_PASSWD_Val           _U_(0x535049)                                        /**< (SPI_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0.  */
#define SPI_WPMR_WPKEY_PASSWD                 (SPI_WPMR_WPKEY_PASSWD_Val << SPI_WPMR_WPKEY_Pos)    /**< (SPI_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0. Position  */
#define SPI_WPMR_Msk                          _U_(0xFFFFFF01)                                      /**< (SPI_WPMR) Register Mask  */


/* -------- SPI_WPSR : (SPI Offset: 0xE8) ( R/ 32) Write Protection Status Register -------- */
#define SPI_WPSR_WPVS_Pos                     _U_(0)                                               /**< (SPI_WPSR) Write Protection Violation Status Position */
#define SPI_WPSR_WPVS_Msk                     (_U_(0x1) << SPI_WPSR_WPVS_Pos)                      /**< (SPI_WPSR) Write Protection Violation Status Mask */
#define SPI_WPSR_WPVS(value)                  (SPI_WPSR_WPVS_Msk & ((value) << SPI_WPSR_WPVS_Pos))
#define SPI_WPSR_WPVSRC_Pos                   _U_(8)                                               /**< (SPI_WPSR) Write Protection Violation Source Position */
#define SPI_WPSR_WPVSRC_Msk                   (_U_(0xFF) << SPI_WPSR_WPVSRC_Pos)                   /**< (SPI_WPSR) Write Protection Violation Source Mask */
#define SPI_WPSR_WPVSRC(value)                (SPI_WPSR_WPVSRC_Msk & ((value) << SPI_WPSR_WPVSRC_Pos))
#define SPI_WPSR_Msk                          _U_(0x0000FF01)                                      /**< (SPI_WPSR) Register Mask  */


/** \brief SPI register offsets definitions */
#define SPI_CR_REG_OFST                (0x00)              /**< (SPI_CR) Control Register Offset */
#define SPI_MR_REG_OFST                (0x04)              /**< (SPI_MR) Mode Register Offset */
#define SPI_RDR_REG_OFST               (0x08)              /**< (SPI_RDR) Receive Data Register Offset */
#define SPI_TDR_REG_OFST               (0x0C)              /**< (SPI_TDR) Transmit Data Register Offset */
#define SPI_SR_REG_OFST                (0x10)              /**< (SPI_SR) Status Register Offset */
#define SPI_IER_REG_OFST               (0x14)              /**< (SPI_IER) Interrupt Enable Register Offset */
#define SPI_IDR_REG_OFST               (0x18)              /**< (SPI_IDR) Interrupt Disable Register Offset */
#define SPI_IMR_REG_OFST               (0x1C)              /**< (SPI_IMR) Interrupt Mask Register Offset */
#define SPI_CSR_REG_OFST               (0x30)              /**< (SPI_CSR) Chip Select Register Offset */
#define SPI_WPMR_REG_OFST              (0xE4)              /**< (SPI_WPMR) Write Protection Mode Register Offset */
#define SPI_WPSR_REG_OFST              (0xE8)              /**< (SPI_WPSR) Write Protection Status Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief SPI register API structure */
typedef struct
{
  __O   uint32_t                       SPI_CR;             /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       SPI_MR;             /**< Offset: 0x04 (R/W  32) Mode Register */
  __I   uint32_t                       SPI_RDR;            /**< Offset: 0x08 (R/   32) Receive Data Register */
  __O   uint32_t                       SPI_TDR;            /**< Offset: 0x0C ( /W  32) Transmit Data Register */
  __I   uint32_t                       SPI_SR;             /**< Offset: 0x10 (R/   32) Status Register */
  __O   uint32_t                       SPI_IER;            /**< Offset: 0x14 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       SPI_IDR;            /**< Offset: 0x18 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       SPI_IMR;            /**< Offset: 0x1C (R/   32) Interrupt Mask Register */
  __I   uint8_t                        Reserved1[0x10];
  __IO  uint32_t                       SPI_CSR[4];         /**< Offset: 0x30 (R/W  32) Chip Select Register */
  __I   uint8_t                        Reserved2[0xA4];
  __IO  uint32_t                       SPI_WPMR;           /**< Offset: 0xE4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       SPI_WPSR;           /**< Offset: 0xE8 (R/   32) Write Protection Status Register */
} spi_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_SPI_COMPONENT_H_ */
