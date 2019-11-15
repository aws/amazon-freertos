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

/* file generated from device description version 2019-06-03T16:18:52Z */
#ifndef _SAME54_QSPI_COMPONENT_H_
#define _SAME54_QSPI_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR QSPI                                         */
/* ************************************************************************** */

/* -------- QSPI_CTRLA : (QSPI Offset: 0x00) (R/W 32) Control A -------- */
#define QSPI_CTRLA_RESETVALUE                 _U_(0x00)                                            /**<  (QSPI_CTRLA) Control A  Reset Value */

#define QSPI_CTRLA_SWRST_Pos                  _U_(0)                                               /**< (QSPI_CTRLA) Software Reset Position */
#define QSPI_CTRLA_SWRST_Msk                  (_U_(0x1) << QSPI_CTRLA_SWRST_Pos)                   /**< (QSPI_CTRLA) Software Reset Mask */
#define QSPI_CTRLA_SWRST(value)               (QSPI_CTRLA_SWRST_Msk & ((value) << QSPI_CTRLA_SWRST_Pos))
#define QSPI_CTRLA_ENABLE_Pos                 _U_(1)                                               /**< (QSPI_CTRLA) Enable Position */
#define QSPI_CTRLA_ENABLE_Msk                 (_U_(0x1) << QSPI_CTRLA_ENABLE_Pos)                  /**< (QSPI_CTRLA) Enable Mask */
#define QSPI_CTRLA_ENABLE(value)              (QSPI_CTRLA_ENABLE_Msk & ((value) << QSPI_CTRLA_ENABLE_Pos))
#define QSPI_CTRLA_LASTXFER_Pos               _U_(24)                                              /**< (QSPI_CTRLA) Last Transfer Position */
#define QSPI_CTRLA_LASTXFER_Msk               (_U_(0x1) << QSPI_CTRLA_LASTXFER_Pos)                /**< (QSPI_CTRLA) Last Transfer Mask */
#define QSPI_CTRLA_LASTXFER(value)            (QSPI_CTRLA_LASTXFER_Msk & ((value) << QSPI_CTRLA_LASTXFER_Pos))
#define QSPI_CTRLA_Msk                        _U_(0x01000003)                                      /**< (QSPI_CTRLA) Register Mask  */


/* -------- QSPI_CTRLB : (QSPI Offset: 0x04) (R/W 32) Control B -------- */
#define QSPI_CTRLB_RESETVALUE                 _U_(0x00)                                            /**<  (QSPI_CTRLB) Control B  Reset Value */

#define QSPI_CTRLB_MODE_Pos                   _U_(0)                                               /**< (QSPI_CTRLB) Serial Memory Mode Position */
#define QSPI_CTRLB_MODE_Msk                   (_U_(0x1) << QSPI_CTRLB_MODE_Pos)                    /**< (QSPI_CTRLB) Serial Memory Mode Mask */
#define QSPI_CTRLB_MODE(value)                (QSPI_CTRLB_MODE_Msk & ((value) << QSPI_CTRLB_MODE_Pos))
#define   QSPI_CTRLB_MODE_SPI_Val             _U_(0x0)                                             /**< (QSPI_CTRLB) SPI operating mode  */
#define   QSPI_CTRLB_MODE_MEMORY_Val          _U_(0x1)                                             /**< (QSPI_CTRLB) Serial Memory operating mode  */
#define QSPI_CTRLB_MODE_SPI                   (QSPI_CTRLB_MODE_SPI_Val << QSPI_CTRLB_MODE_Pos)     /**< (QSPI_CTRLB) SPI operating mode Position  */
#define QSPI_CTRLB_MODE_MEMORY                (QSPI_CTRLB_MODE_MEMORY_Val << QSPI_CTRLB_MODE_Pos)  /**< (QSPI_CTRLB) Serial Memory operating mode Position  */
#define QSPI_CTRLB_LOOPEN_Pos                 _U_(1)                                               /**< (QSPI_CTRLB) Local Loopback Enable Position */
#define QSPI_CTRLB_LOOPEN_Msk                 (_U_(0x1) << QSPI_CTRLB_LOOPEN_Pos)                  /**< (QSPI_CTRLB) Local Loopback Enable Mask */
#define QSPI_CTRLB_LOOPEN(value)              (QSPI_CTRLB_LOOPEN_Msk & ((value) << QSPI_CTRLB_LOOPEN_Pos))
#define QSPI_CTRLB_WDRBT_Pos                  _U_(2)                                               /**< (QSPI_CTRLB) Wait Data Read Before Transfer Position */
#define QSPI_CTRLB_WDRBT_Msk                  (_U_(0x1) << QSPI_CTRLB_WDRBT_Pos)                   /**< (QSPI_CTRLB) Wait Data Read Before Transfer Mask */
#define QSPI_CTRLB_WDRBT(value)               (QSPI_CTRLB_WDRBT_Msk & ((value) << QSPI_CTRLB_WDRBT_Pos))
#define QSPI_CTRLB_SMEMREG_Pos                _U_(3)                                               /**< (QSPI_CTRLB) Serial Memory reg Position */
#define QSPI_CTRLB_SMEMREG_Msk                (_U_(0x1) << QSPI_CTRLB_SMEMREG_Pos)                 /**< (QSPI_CTRLB) Serial Memory reg Mask */
#define QSPI_CTRLB_SMEMREG(value)             (QSPI_CTRLB_SMEMREG_Msk & ((value) << QSPI_CTRLB_SMEMREG_Pos))
#define QSPI_CTRLB_CSMODE_Pos                 _U_(4)                                               /**< (QSPI_CTRLB) Chip Select Mode Position */
#define QSPI_CTRLB_CSMODE_Msk                 (_U_(0x3) << QSPI_CTRLB_CSMODE_Pos)                  /**< (QSPI_CTRLB) Chip Select Mode Mask */
#define QSPI_CTRLB_CSMODE(value)              (QSPI_CTRLB_CSMODE_Msk & ((value) << QSPI_CTRLB_CSMODE_Pos))
#define   QSPI_CTRLB_CSMODE_NORELOAD_Val      _U_(0x0)                                             /**< (QSPI_CTRLB) The chip select is deasserted if TD has not been reloaded before the end of the current transfer.  */
#define   QSPI_CTRLB_CSMODE_LASTXFER_Val      _U_(0x1)                                             /**< (QSPI_CTRLB) The chip select is deasserted when the bit LASTXFER is written at 1 and the character written in TD has been transferred.  */
#define   QSPI_CTRLB_CSMODE_SYSTEMATICALLY_Val _U_(0x2)                                             /**< (QSPI_CTRLB) The chip select is deasserted systematically after each transfer.  */
#define QSPI_CTRLB_CSMODE_NORELOAD            (QSPI_CTRLB_CSMODE_NORELOAD_Val << QSPI_CTRLB_CSMODE_Pos) /**< (QSPI_CTRLB) The chip select is deasserted if TD has not been reloaded before the end of the current transfer. Position  */
#define QSPI_CTRLB_CSMODE_LASTXFER            (QSPI_CTRLB_CSMODE_LASTXFER_Val << QSPI_CTRLB_CSMODE_Pos) /**< (QSPI_CTRLB) The chip select is deasserted when the bit LASTXFER is written at 1 and the character written in TD has been transferred. Position  */
#define QSPI_CTRLB_CSMODE_SYSTEMATICALLY      (QSPI_CTRLB_CSMODE_SYSTEMATICALLY_Val << QSPI_CTRLB_CSMODE_Pos) /**< (QSPI_CTRLB) The chip select is deasserted systematically after each transfer. Position  */
#define QSPI_CTRLB_DATALEN_Pos                _U_(8)                                               /**< (QSPI_CTRLB) Data Length Position */
#define QSPI_CTRLB_DATALEN_Msk                (_U_(0xF) << QSPI_CTRLB_DATALEN_Pos)                 /**< (QSPI_CTRLB) Data Length Mask */
#define QSPI_CTRLB_DATALEN(value)             (QSPI_CTRLB_DATALEN_Msk & ((value) << QSPI_CTRLB_DATALEN_Pos))
#define   QSPI_CTRLB_DATALEN_8BITS_Val        _U_(0x0)                                             /**< (QSPI_CTRLB) 8-bits transfer  */
#define   QSPI_CTRLB_DATALEN_9BITS_Val        _U_(0x1)                                             /**< (QSPI_CTRLB) 9 bits transfer  */
#define   QSPI_CTRLB_DATALEN_10BITS_Val       _U_(0x2)                                             /**< (QSPI_CTRLB) 10-bits transfer  */
#define   QSPI_CTRLB_DATALEN_11BITS_Val       _U_(0x3)                                             /**< (QSPI_CTRLB) 11-bits transfer  */
#define   QSPI_CTRLB_DATALEN_12BITS_Val       _U_(0x4)                                             /**< (QSPI_CTRLB) 12-bits transfer  */
#define   QSPI_CTRLB_DATALEN_13BITS_Val       _U_(0x5)                                             /**< (QSPI_CTRLB) 13-bits transfer  */
#define   QSPI_CTRLB_DATALEN_14BITS_Val       _U_(0x6)                                             /**< (QSPI_CTRLB) 14-bits transfer  */
#define   QSPI_CTRLB_DATALEN_15BITS_Val       _U_(0x7)                                             /**< (QSPI_CTRLB) 15-bits transfer  */
#define   QSPI_CTRLB_DATALEN_16BITS_Val       _U_(0x8)                                             /**< (QSPI_CTRLB) 16-bits transfer  */
#define QSPI_CTRLB_DATALEN_8BITS              (QSPI_CTRLB_DATALEN_8BITS_Val << QSPI_CTRLB_DATALEN_Pos) /**< (QSPI_CTRLB) 8-bits transfer Position  */
#define QSPI_CTRLB_DATALEN_9BITS              (QSPI_CTRLB_DATALEN_9BITS_Val << QSPI_CTRLB_DATALEN_Pos) /**< (QSPI_CTRLB) 9 bits transfer Position  */
#define QSPI_CTRLB_DATALEN_10BITS             (QSPI_CTRLB_DATALEN_10BITS_Val << QSPI_CTRLB_DATALEN_Pos) /**< (QSPI_CTRLB) 10-bits transfer Position  */
#define QSPI_CTRLB_DATALEN_11BITS             (QSPI_CTRLB_DATALEN_11BITS_Val << QSPI_CTRLB_DATALEN_Pos) /**< (QSPI_CTRLB) 11-bits transfer Position  */
#define QSPI_CTRLB_DATALEN_12BITS             (QSPI_CTRLB_DATALEN_12BITS_Val << QSPI_CTRLB_DATALEN_Pos) /**< (QSPI_CTRLB) 12-bits transfer Position  */
#define QSPI_CTRLB_DATALEN_13BITS             (QSPI_CTRLB_DATALEN_13BITS_Val << QSPI_CTRLB_DATALEN_Pos) /**< (QSPI_CTRLB) 13-bits transfer Position  */
#define QSPI_CTRLB_DATALEN_14BITS             (QSPI_CTRLB_DATALEN_14BITS_Val << QSPI_CTRLB_DATALEN_Pos) /**< (QSPI_CTRLB) 14-bits transfer Position  */
#define QSPI_CTRLB_DATALEN_15BITS             (QSPI_CTRLB_DATALEN_15BITS_Val << QSPI_CTRLB_DATALEN_Pos) /**< (QSPI_CTRLB) 15-bits transfer Position  */
#define QSPI_CTRLB_DATALEN_16BITS             (QSPI_CTRLB_DATALEN_16BITS_Val << QSPI_CTRLB_DATALEN_Pos) /**< (QSPI_CTRLB) 16-bits transfer Position  */
#define QSPI_CTRLB_DLYBCT_Pos                 _U_(16)                                              /**< (QSPI_CTRLB) Delay Between Consecutive Transfers Position */
#define QSPI_CTRLB_DLYBCT_Msk                 (_U_(0xFF) << QSPI_CTRLB_DLYBCT_Pos)                 /**< (QSPI_CTRLB) Delay Between Consecutive Transfers Mask */
#define QSPI_CTRLB_DLYBCT(value)              (QSPI_CTRLB_DLYBCT_Msk & ((value) << QSPI_CTRLB_DLYBCT_Pos))
#define QSPI_CTRLB_DLYCS_Pos                  _U_(24)                                              /**< (QSPI_CTRLB) Minimum Inactive CS Delay Position */
#define QSPI_CTRLB_DLYCS_Msk                  (_U_(0xFF) << QSPI_CTRLB_DLYCS_Pos)                  /**< (QSPI_CTRLB) Minimum Inactive CS Delay Mask */
#define QSPI_CTRLB_DLYCS(value)               (QSPI_CTRLB_DLYCS_Msk & ((value) << QSPI_CTRLB_DLYCS_Pos))
#define QSPI_CTRLB_Msk                        _U_(0xFFFF0F3F)                                      /**< (QSPI_CTRLB) Register Mask  */


/* -------- QSPI_BAUD : (QSPI Offset: 0x08) (R/W 32) Baud Rate -------- */
#define QSPI_BAUD_RESETVALUE                  _U_(0x00)                                            /**<  (QSPI_BAUD) Baud Rate  Reset Value */

#define QSPI_BAUD_CPOL_Pos                    _U_(0)                                               /**< (QSPI_BAUD) Clock Polarity Position */
#define QSPI_BAUD_CPOL_Msk                    (_U_(0x1) << QSPI_BAUD_CPOL_Pos)                     /**< (QSPI_BAUD) Clock Polarity Mask */
#define QSPI_BAUD_CPOL(value)                 (QSPI_BAUD_CPOL_Msk & ((value) << QSPI_BAUD_CPOL_Pos))
#define QSPI_BAUD_CPHA_Pos                    _U_(1)                                               /**< (QSPI_BAUD) Clock Phase Position */
#define QSPI_BAUD_CPHA_Msk                    (_U_(0x1) << QSPI_BAUD_CPHA_Pos)                     /**< (QSPI_BAUD) Clock Phase Mask */
#define QSPI_BAUD_CPHA(value)                 (QSPI_BAUD_CPHA_Msk & ((value) << QSPI_BAUD_CPHA_Pos))
#define QSPI_BAUD_BAUD_Pos                    _U_(8)                                               /**< (QSPI_BAUD) Serial Clock Baud Rate Position */
#define QSPI_BAUD_BAUD_Msk                    (_U_(0xFF) << QSPI_BAUD_BAUD_Pos)                    /**< (QSPI_BAUD) Serial Clock Baud Rate Mask */
#define QSPI_BAUD_BAUD(value)                 (QSPI_BAUD_BAUD_Msk & ((value) << QSPI_BAUD_BAUD_Pos))
#define QSPI_BAUD_DLYBS_Pos                   _U_(16)                                              /**< (QSPI_BAUD) Delay Before SCK Position */
#define QSPI_BAUD_DLYBS_Msk                   (_U_(0xFF) << QSPI_BAUD_DLYBS_Pos)                   /**< (QSPI_BAUD) Delay Before SCK Mask */
#define QSPI_BAUD_DLYBS(value)                (QSPI_BAUD_DLYBS_Msk & ((value) << QSPI_BAUD_DLYBS_Pos))
#define QSPI_BAUD_Msk                         _U_(0x00FFFF03)                                      /**< (QSPI_BAUD) Register Mask  */


/* -------- QSPI_RXDATA : (QSPI Offset: 0x0C) ( R/ 32) Receive Data -------- */
#define QSPI_RXDATA_RESETVALUE                _U_(0x00)                                            /**<  (QSPI_RXDATA) Receive Data  Reset Value */

#define QSPI_RXDATA_DATA_Pos                  _U_(0)                                               /**< (QSPI_RXDATA) Receive Data Position */
#define QSPI_RXDATA_DATA_Msk                  (_U_(0xFFFF) << QSPI_RXDATA_DATA_Pos)                /**< (QSPI_RXDATA) Receive Data Mask */
#define QSPI_RXDATA_DATA(value)               (QSPI_RXDATA_DATA_Msk & ((value) << QSPI_RXDATA_DATA_Pos))
#define QSPI_RXDATA_Msk                       _U_(0x0000FFFF)                                      /**< (QSPI_RXDATA) Register Mask  */


/* -------- QSPI_TXDATA : (QSPI Offset: 0x10) ( /W 32) Transmit Data -------- */
#define QSPI_TXDATA_RESETVALUE                _U_(0x00)                                            /**<  (QSPI_TXDATA) Transmit Data  Reset Value */

#define QSPI_TXDATA_DATA_Pos                  _U_(0)                                               /**< (QSPI_TXDATA) Transmit Data Position */
#define QSPI_TXDATA_DATA_Msk                  (_U_(0xFFFF) << QSPI_TXDATA_DATA_Pos)                /**< (QSPI_TXDATA) Transmit Data Mask */
#define QSPI_TXDATA_DATA(value)               (QSPI_TXDATA_DATA_Msk & ((value) << QSPI_TXDATA_DATA_Pos))
#define QSPI_TXDATA_Msk                       _U_(0x0000FFFF)                                      /**< (QSPI_TXDATA) Register Mask  */


/* -------- QSPI_INTENCLR : (QSPI Offset: 0x14) (R/W 32) Interrupt Enable Clear -------- */
#define QSPI_INTENCLR_RESETVALUE              _U_(0x00)                                            /**<  (QSPI_INTENCLR) Interrupt Enable Clear  Reset Value */

#define QSPI_INTENCLR_RXC_Pos                 _U_(0)                                               /**< (QSPI_INTENCLR) Receive Data Register Full Interrupt Disable Position */
#define QSPI_INTENCLR_RXC_Msk                 (_U_(0x1) << QSPI_INTENCLR_RXC_Pos)                  /**< (QSPI_INTENCLR) Receive Data Register Full Interrupt Disable Mask */
#define QSPI_INTENCLR_RXC(value)              (QSPI_INTENCLR_RXC_Msk & ((value) << QSPI_INTENCLR_RXC_Pos))
#define QSPI_INTENCLR_DRE_Pos                 _U_(1)                                               /**< (QSPI_INTENCLR) Transmit Data Register Empty Interrupt Disable Position */
#define QSPI_INTENCLR_DRE_Msk                 (_U_(0x1) << QSPI_INTENCLR_DRE_Pos)                  /**< (QSPI_INTENCLR) Transmit Data Register Empty Interrupt Disable Mask */
#define QSPI_INTENCLR_DRE(value)              (QSPI_INTENCLR_DRE_Msk & ((value) << QSPI_INTENCLR_DRE_Pos))
#define QSPI_INTENCLR_TXC_Pos                 _U_(2)                                               /**< (QSPI_INTENCLR) Transmission Complete Interrupt Disable Position */
#define QSPI_INTENCLR_TXC_Msk                 (_U_(0x1) << QSPI_INTENCLR_TXC_Pos)                  /**< (QSPI_INTENCLR) Transmission Complete Interrupt Disable Mask */
#define QSPI_INTENCLR_TXC(value)              (QSPI_INTENCLR_TXC_Msk & ((value) << QSPI_INTENCLR_TXC_Pos))
#define QSPI_INTENCLR_ERROR_Pos               _U_(3)                                               /**< (QSPI_INTENCLR) Overrun Error Interrupt Disable Position */
#define QSPI_INTENCLR_ERROR_Msk               (_U_(0x1) << QSPI_INTENCLR_ERROR_Pos)                /**< (QSPI_INTENCLR) Overrun Error Interrupt Disable Mask */
#define QSPI_INTENCLR_ERROR(value)            (QSPI_INTENCLR_ERROR_Msk & ((value) << QSPI_INTENCLR_ERROR_Pos))
#define QSPI_INTENCLR_CSRISE_Pos              _U_(8)                                               /**< (QSPI_INTENCLR) Chip Select Rise Interrupt Disable Position */
#define QSPI_INTENCLR_CSRISE_Msk              (_U_(0x1) << QSPI_INTENCLR_CSRISE_Pos)               /**< (QSPI_INTENCLR) Chip Select Rise Interrupt Disable Mask */
#define QSPI_INTENCLR_CSRISE(value)           (QSPI_INTENCLR_CSRISE_Msk & ((value) << QSPI_INTENCLR_CSRISE_Pos))
#define QSPI_INTENCLR_INSTREND_Pos            _U_(10)                                              /**< (QSPI_INTENCLR) Instruction End Interrupt Disable Position */
#define QSPI_INTENCLR_INSTREND_Msk            (_U_(0x1) << QSPI_INTENCLR_INSTREND_Pos)             /**< (QSPI_INTENCLR) Instruction End Interrupt Disable Mask */
#define QSPI_INTENCLR_INSTREND(value)         (QSPI_INTENCLR_INSTREND_Msk & ((value) << QSPI_INTENCLR_INSTREND_Pos))
#define QSPI_INTENCLR_Msk                     _U_(0x0000050F)                                      /**< (QSPI_INTENCLR) Register Mask  */


/* -------- QSPI_INTENSET : (QSPI Offset: 0x18) (R/W 32) Interrupt Enable Set -------- */
#define QSPI_INTENSET_RESETVALUE              _U_(0x00)                                            /**<  (QSPI_INTENSET) Interrupt Enable Set  Reset Value */

#define QSPI_INTENSET_RXC_Pos                 _U_(0)                                               /**< (QSPI_INTENSET) Receive Data Register Full Interrupt Enable Position */
#define QSPI_INTENSET_RXC_Msk                 (_U_(0x1) << QSPI_INTENSET_RXC_Pos)                  /**< (QSPI_INTENSET) Receive Data Register Full Interrupt Enable Mask */
#define QSPI_INTENSET_RXC(value)              (QSPI_INTENSET_RXC_Msk & ((value) << QSPI_INTENSET_RXC_Pos))
#define QSPI_INTENSET_DRE_Pos                 _U_(1)                                               /**< (QSPI_INTENSET) Transmit Data Register Empty Interrupt Enable Position */
#define QSPI_INTENSET_DRE_Msk                 (_U_(0x1) << QSPI_INTENSET_DRE_Pos)                  /**< (QSPI_INTENSET) Transmit Data Register Empty Interrupt Enable Mask */
#define QSPI_INTENSET_DRE(value)              (QSPI_INTENSET_DRE_Msk & ((value) << QSPI_INTENSET_DRE_Pos))
#define QSPI_INTENSET_TXC_Pos                 _U_(2)                                               /**< (QSPI_INTENSET) Transmission Complete Interrupt Enable Position */
#define QSPI_INTENSET_TXC_Msk                 (_U_(0x1) << QSPI_INTENSET_TXC_Pos)                  /**< (QSPI_INTENSET) Transmission Complete Interrupt Enable Mask */
#define QSPI_INTENSET_TXC(value)              (QSPI_INTENSET_TXC_Msk & ((value) << QSPI_INTENSET_TXC_Pos))
#define QSPI_INTENSET_ERROR_Pos               _U_(3)                                               /**< (QSPI_INTENSET) Overrun Error Interrupt Enable Position */
#define QSPI_INTENSET_ERROR_Msk               (_U_(0x1) << QSPI_INTENSET_ERROR_Pos)                /**< (QSPI_INTENSET) Overrun Error Interrupt Enable Mask */
#define QSPI_INTENSET_ERROR(value)            (QSPI_INTENSET_ERROR_Msk & ((value) << QSPI_INTENSET_ERROR_Pos))
#define QSPI_INTENSET_CSRISE_Pos              _U_(8)                                               /**< (QSPI_INTENSET) Chip Select Rise Interrupt Enable Position */
#define QSPI_INTENSET_CSRISE_Msk              (_U_(0x1) << QSPI_INTENSET_CSRISE_Pos)               /**< (QSPI_INTENSET) Chip Select Rise Interrupt Enable Mask */
#define QSPI_INTENSET_CSRISE(value)           (QSPI_INTENSET_CSRISE_Msk & ((value) << QSPI_INTENSET_CSRISE_Pos))
#define QSPI_INTENSET_INSTREND_Pos            _U_(10)                                              /**< (QSPI_INTENSET) Instruction End Interrupt Enable Position */
#define QSPI_INTENSET_INSTREND_Msk            (_U_(0x1) << QSPI_INTENSET_INSTREND_Pos)             /**< (QSPI_INTENSET) Instruction End Interrupt Enable Mask */
#define QSPI_INTENSET_INSTREND(value)         (QSPI_INTENSET_INSTREND_Msk & ((value) << QSPI_INTENSET_INSTREND_Pos))
#define QSPI_INTENSET_Msk                     _U_(0x0000050F)                                      /**< (QSPI_INTENSET) Register Mask  */


/* -------- QSPI_INTFLAG : (QSPI Offset: 0x1C) (R/W 32) Interrupt Flag Status and Clear -------- */
#define QSPI_INTFLAG_RESETVALUE               _U_(0x00)                                            /**<  (QSPI_INTFLAG) Interrupt Flag Status and Clear  Reset Value */

#define QSPI_INTFLAG_RXC_Pos                  _U_(0)                                               /**< (QSPI_INTFLAG) Receive Data Register Full Position */
#define QSPI_INTFLAG_RXC_Msk                  (_U_(0x1) << QSPI_INTFLAG_RXC_Pos)                   /**< (QSPI_INTFLAG) Receive Data Register Full Mask */
#define QSPI_INTFLAG_RXC(value)               (QSPI_INTFLAG_RXC_Msk & ((value) << QSPI_INTFLAG_RXC_Pos))
#define QSPI_INTFLAG_DRE_Pos                  _U_(1)                                               /**< (QSPI_INTFLAG) Transmit Data Register Empty Position */
#define QSPI_INTFLAG_DRE_Msk                  (_U_(0x1) << QSPI_INTFLAG_DRE_Pos)                   /**< (QSPI_INTFLAG) Transmit Data Register Empty Mask */
#define QSPI_INTFLAG_DRE(value)               (QSPI_INTFLAG_DRE_Msk & ((value) << QSPI_INTFLAG_DRE_Pos))
#define QSPI_INTFLAG_TXC_Pos                  _U_(2)                                               /**< (QSPI_INTFLAG) Transmission Complete Position */
#define QSPI_INTFLAG_TXC_Msk                  (_U_(0x1) << QSPI_INTFLAG_TXC_Pos)                   /**< (QSPI_INTFLAG) Transmission Complete Mask */
#define QSPI_INTFLAG_TXC(value)               (QSPI_INTFLAG_TXC_Msk & ((value) << QSPI_INTFLAG_TXC_Pos))
#define QSPI_INTFLAG_ERROR_Pos                _U_(3)                                               /**< (QSPI_INTFLAG) Overrun Error Position */
#define QSPI_INTFLAG_ERROR_Msk                (_U_(0x1) << QSPI_INTFLAG_ERROR_Pos)                 /**< (QSPI_INTFLAG) Overrun Error Mask */
#define QSPI_INTFLAG_ERROR(value)             (QSPI_INTFLAG_ERROR_Msk & ((value) << QSPI_INTFLAG_ERROR_Pos))
#define QSPI_INTFLAG_CSRISE_Pos               _U_(8)                                               /**< (QSPI_INTFLAG) Chip Select Rise Position */
#define QSPI_INTFLAG_CSRISE_Msk               (_U_(0x1) << QSPI_INTFLAG_CSRISE_Pos)                /**< (QSPI_INTFLAG) Chip Select Rise Mask */
#define QSPI_INTFLAG_CSRISE(value)            (QSPI_INTFLAG_CSRISE_Msk & ((value) << QSPI_INTFLAG_CSRISE_Pos))
#define QSPI_INTFLAG_INSTREND_Pos             _U_(10)                                              /**< (QSPI_INTFLAG) Instruction End Position */
#define QSPI_INTFLAG_INSTREND_Msk             (_U_(0x1) << QSPI_INTFLAG_INSTREND_Pos)              /**< (QSPI_INTFLAG) Instruction End Mask */
#define QSPI_INTFLAG_INSTREND(value)          (QSPI_INTFLAG_INSTREND_Msk & ((value) << QSPI_INTFLAG_INSTREND_Pos))
#define QSPI_INTFLAG_Msk                      _U_(0x0000050F)                                      /**< (QSPI_INTFLAG) Register Mask  */


/* -------- QSPI_STATUS : (QSPI Offset: 0x20) ( R/ 32) Status Register -------- */
#define QSPI_STATUS_RESETVALUE                _U_(0x200)                                           /**<  (QSPI_STATUS) Status Register  Reset Value */

#define QSPI_STATUS_ENABLE_Pos                _U_(1)                                               /**< (QSPI_STATUS) Enable Position */
#define QSPI_STATUS_ENABLE_Msk                (_U_(0x1) << QSPI_STATUS_ENABLE_Pos)                 /**< (QSPI_STATUS) Enable Mask */
#define QSPI_STATUS_ENABLE(value)             (QSPI_STATUS_ENABLE_Msk & ((value) << QSPI_STATUS_ENABLE_Pos))
#define QSPI_STATUS_CSSTATUS_Pos              _U_(9)                                               /**< (QSPI_STATUS) Chip Select Position */
#define QSPI_STATUS_CSSTATUS_Msk              (_U_(0x1) << QSPI_STATUS_CSSTATUS_Pos)               /**< (QSPI_STATUS) Chip Select Mask */
#define QSPI_STATUS_CSSTATUS(value)           (QSPI_STATUS_CSSTATUS_Msk & ((value) << QSPI_STATUS_CSSTATUS_Pos))
#define QSPI_STATUS_Msk                       _U_(0x00000202)                                      /**< (QSPI_STATUS) Register Mask  */


/* -------- QSPI_INSTRADDR : (QSPI Offset: 0x30) (R/W 32) Instruction Address -------- */
#define QSPI_INSTRADDR_RESETVALUE             _U_(0x00)                                            /**<  (QSPI_INSTRADDR) Instruction Address  Reset Value */

#define QSPI_INSTRADDR_ADDR_Pos               _U_(0)                                               /**< (QSPI_INSTRADDR) Instruction Address Position */
#define QSPI_INSTRADDR_ADDR_Msk               (_U_(0xFFFFFFFF) << QSPI_INSTRADDR_ADDR_Pos)         /**< (QSPI_INSTRADDR) Instruction Address Mask */
#define QSPI_INSTRADDR_ADDR(value)            (QSPI_INSTRADDR_ADDR_Msk & ((value) << QSPI_INSTRADDR_ADDR_Pos))
#define QSPI_INSTRADDR_Msk                    _U_(0xFFFFFFFF)                                      /**< (QSPI_INSTRADDR) Register Mask  */


/* -------- QSPI_INSTRCTRL : (QSPI Offset: 0x34) (R/W 32) Instruction Code -------- */
#define QSPI_INSTRCTRL_RESETVALUE             _U_(0x00)                                            /**<  (QSPI_INSTRCTRL) Instruction Code  Reset Value */

#define QSPI_INSTRCTRL_INSTR_Pos              _U_(0)                                               /**< (QSPI_INSTRCTRL) Instruction Code Position */
#define QSPI_INSTRCTRL_INSTR_Msk              (_U_(0xFF) << QSPI_INSTRCTRL_INSTR_Pos)              /**< (QSPI_INSTRCTRL) Instruction Code Mask */
#define QSPI_INSTRCTRL_INSTR(value)           (QSPI_INSTRCTRL_INSTR_Msk & ((value) << QSPI_INSTRCTRL_INSTR_Pos))
#define QSPI_INSTRCTRL_OPTCODE_Pos            _U_(16)                                              /**< (QSPI_INSTRCTRL) Option Code Position */
#define QSPI_INSTRCTRL_OPTCODE_Msk            (_U_(0xFF) << QSPI_INSTRCTRL_OPTCODE_Pos)            /**< (QSPI_INSTRCTRL) Option Code Mask */
#define QSPI_INSTRCTRL_OPTCODE(value)         (QSPI_INSTRCTRL_OPTCODE_Msk & ((value) << QSPI_INSTRCTRL_OPTCODE_Pos))
#define QSPI_INSTRCTRL_Msk                    _U_(0x00FF00FF)                                      /**< (QSPI_INSTRCTRL) Register Mask  */


/* -------- QSPI_INSTRFRAME : (QSPI Offset: 0x38) (R/W 32) Instruction Frame -------- */
#define QSPI_INSTRFRAME_RESETVALUE            _U_(0x00)                                            /**<  (QSPI_INSTRFRAME) Instruction Frame  Reset Value */

#define QSPI_INSTRFRAME_WIDTH_Pos             _U_(0)                                               /**< (QSPI_INSTRFRAME) Instruction Code, Address, Option Code and Data Width Position */
#define QSPI_INSTRFRAME_WIDTH_Msk             (_U_(0x7) << QSPI_INSTRFRAME_WIDTH_Pos)              /**< (QSPI_INSTRFRAME) Instruction Code, Address, Option Code and Data Width Mask */
#define QSPI_INSTRFRAME_WIDTH(value)          (QSPI_INSTRFRAME_WIDTH_Msk & ((value) << QSPI_INSTRFRAME_WIDTH_Pos))
#define   QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI_Val _U_(0x0)                                             /**< (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Single-bit SPI  */
#define   QSPI_INSTRFRAME_WIDTH_DUAL_OUTPUT_Val _U_(0x1)                                             /**< (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Dual SPI  */
#define   QSPI_INSTRFRAME_WIDTH_QUAD_OUTPUT_Val _U_(0x2)                                             /**< (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Quad SPI  */
#define   QSPI_INSTRFRAME_WIDTH_DUAL_IO_Val   _U_(0x3)                                             /**< (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Dual SPI / Data: Dual SPI  */
#define   QSPI_INSTRFRAME_WIDTH_QUAD_IO_Val   _U_(0x4)                                             /**< (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Quad SPI / Data: Quad SPI  */
#define   QSPI_INSTRFRAME_WIDTH_DUAL_CMD_Val  _U_(0x5)                                             /**< (QSPI_INSTRFRAME) Instruction: Dual SPI / Address-Option: Dual SPI / Data: Dual SPI  */
#define   QSPI_INSTRFRAME_WIDTH_QUAD_CMD_Val  _U_(0x6)                                             /**< (QSPI_INSTRFRAME) Instruction: Quad SPI / Address-Option: Quad SPI / Data: Quad SPI  */
#define QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI  (QSPI_INSTRFRAME_WIDTH_SINGLE_BIT_SPI_Val << QSPI_INSTRFRAME_WIDTH_Pos) /**< (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Single-bit SPI Position  */
#define QSPI_INSTRFRAME_WIDTH_DUAL_OUTPUT     (QSPI_INSTRFRAME_WIDTH_DUAL_OUTPUT_Val << QSPI_INSTRFRAME_WIDTH_Pos) /**< (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Dual SPI Position  */
#define QSPI_INSTRFRAME_WIDTH_QUAD_OUTPUT     (QSPI_INSTRFRAME_WIDTH_QUAD_OUTPUT_Val << QSPI_INSTRFRAME_WIDTH_Pos) /**< (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Single-bit SPI / Data: Quad SPI Position  */
#define QSPI_INSTRFRAME_WIDTH_DUAL_IO         (QSPI_INSTRFRAME_WIDTH_DUAL_IO_Val << QSPI_INSTRFRAME_WIDTH_Pos) /**< (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Dual SPI / Data: Dual SPI Position  */
#define QSPI_INSTRFRAME_WIDTH_QUAD_IO         (QSPI_INSTRFRAME_WIDTH_QUAD_IO_Val << QSPI_INSTRFRAME_WIDTH_Pos) /**< (QSPI_INSTRFRAME) Instruction: Single-bit SPI / Address-Option: Quad SPI / Data: Quad SPI Position  */
#define QSPI_INSTRFRAME_WIDTH_DUAL_CMD        (QSPI_INSTRFRAME_WIDTH_DUAL_CMD_Val << QSPI_INSTRFRAME_WIDTH_Pos) /**< (QSPI_INSTRFRAME) Instruction: Dual SPI / Address-Option: Dual SPI / Data: Dual SPI Position  */
#define QSPI_INSTRFRAME_WIDTH_QUAD_CMD        (QSPI_INSTRFRAME_WIDTH_QUAD_CMD_Val << QSPI_INSTRFRAME_WIDTH_Pos) /**< (QSPI_INSTRFRAME) Instruction: Quad SPI / Address-Option: Quad SPI / Data: Quad SPI Position  */
#define QSPI_INSTRFRAME_INSTREN_Pos           _U_(4)                                               /**< (QSPI_INSTRFRAME) Instruction Enable Position */
#define QSPI_INSTRFRAME_INSTREN_Msk           (_U_(0x1) << QSPI_INSTRFRAME_INSTREN_Pos)            /**< (QSPI_INSTRFRAME) Instruction Enable Mask */
#define QSPI_INSTRFRAME_INSTREN(value)        (QSPI_INSTRFRAME_INSTREN_Msk & ((value) << QSPI_INSTRFRAME_INSTREN_Pos))
#define QSPI_INSTRFRAME_ADDREN_Pos            _U_(5)                                               /**< (QSPI_INSTRFRAME) Address Enable Position */
#define QSPI_INSTRFRAME_ADDREN_Msk            (_U_(0x1) << QSPI_INSTRFRAME_ADDREN_Pos)             /**< (QSPI_INSTRFRAME) Address Enable Mask */
#define QSPI_INSTRFRAME_ADDREN(value)         (QSPI_INSTRFRAME_ADDREN_Msk & ((value) << QSPI_INSTRFRAME_ADDREN_Pos))
#define QSPI_INSTRFRAME_OPTCODEEN_Pos         _U_(6)                                               /**< (QSPI_INSTRFRAME) Option Enable Position */
#define QSPI_INSTRFRAME_OPTCODEEN_Msk         (_U_(0x1) << QSPI_INSTRFRAME_OPTCODEEN_Pos)          /**< (QSPI_INSTRFRAME) Option Enable Mask */
#define QSPI_INSTRFRAME_OPTCODEEN(value)      (QSPI_INSTRFRAME_OPTCODEEN_Msk & ((value) << QSPI_INSTRFRAME_OPTCODEEN_Pos))
#define QSPI_INSTRFRAME_DATAEN_Pos            _U_(7)                                               /**< (QSPI_INSTRFRAME) Data Enable Position */
#define QSPI_INSTRFRAME_DATAEN_Msk            (_U_(0x1) << QSPI_INSTRFRAME_DATAEN_Pos)             /**< (QSPI_INSTRFRAME) Data Enable Mask */
#define QSPI_INSTRFRAME_DATAEN(value)         (QSPI_INSTRFRAME_DATAEN_Msk & ((value) << QSPI_INSTRFRAME_DATAEN_Pos))
#define QSPI_INSTRFRAME_OPTCODELEN_Pos        _U_(8)                                               /**< (QSPI_INSTRFRAME) Option Code Length Position */
#define QSPI_INSTRFRAME_OPTCODELEN_Msk        (_U_(0x3) << QSPI_INSTRFRAME_OPTCODELEN_Pos)         /**< (QSPI_INSTRFRAME) Option Code Length Mask */
#define QSPI_INSTRFRAME_OPTCODELEN(value)     (QSPI_INSTRFRAME_OPTCODELEN_Msk & ((value) << QSPI_INSTRFRAME_OPTCODELEN_Pos))
#define   QSPI_INSTRFRAME_OPTCODELEN_1BIT_Val _U_(0x0)                                             /**< (QSPI_INSTRFRAME) 1-bit length option code  */
#define   QSPI_INSTRFRAME_OPTCODELEN_2BITS_Val _U_(0x1)                                             /**< (QSPI_INSTRFRAME) 2-bits length option code  */
#define   QSPI_INSTRFRAME_OPTCODELEN_4BITS_Val _U_(0x2)                                             /**< (QSPI_INSTRFRAME) 4-bits length option code  */
#define   QSPI_INSTRFRAME_OPTCODELEN_8BITS_Val _U_(0x3)                                             /**< (QSPI_INSTRFRAME) 8-bits length option code  */
#define QSPI_INSTRFRAME_OPTCODELEN_1BIT       (QSPI_INSTRFRAME_OPTCODELEN_1BIT_Val << QSPI_INSTRFRAME_OPTCODELEN_Pos) /**< (QSPI_INSTRFRAME) 1-bit length option code Position  */
#define QSPI_INSTRFRAME_OPTCODELEN_2BITS      (QSPI_INSTRFRAME_OPTCODELEN_2BITS_Val << QSPI_INSTRFRAME_OPTCODELEN_Pos) /**< (QSPI_INSTRFRAME) 2-bits length option code Position  */
#define QSPI_INSTRFRAME_OPTCODELEN_4BITS      (QSPI_INSTRFRAME_OPTCODELEN_4BITS_Val << QSPI_INSTRFRAME_OPTCODELEN_Pos) /**< (QSPI_INSTRFRAME) 4-bits length option code Position  */
#define QSPI_INSTRFRAME_OPTCODELEN_8BITS      (QSPI_INSTRFRAME_OPTCODELEN_8BITS_Val << QSPI_INSTRFRAME_OPTCODELEN_Pos) /**< (QSPI_INSTRFRAME) 8-bits length option code Position  */
#define QSPI_INSTRFRAME_ADDRLEN_Pos           _U_(10)                                              /**< (QSPI_INSTRFRAME) Address Length Position */
#define QSPI_INSTRFRAME_ADDRLEN_Msk           (_U_(0x1) << QSPI_INSTRFRAME_ADDRLEN_Pos)            /**< (QSPI_INSTRFRAME) Address Length Mask */
#define QSPI_INSTRFRAME_ADDRLEN(value)        (QSPI_INSTRFRAME_ADDRLEN_Msk & ((value) << QSPI_INSTRFRAME_ADDRLEN_Pos))
#define   QSPI_INSTRFRAME_ADDRLEN_24BITS_Val  _U_(0x0)                                             /**< (QSPI_INSTRFRAME) 24-bits address length  */
#define   QSPI_INSTRFRAME_ADDRLEN_32BITS_Val  _U_(0x1)                                             /**< (QSPI_INSTRFRAME) 32-bits address length  */
#define QSPI_INSTRFRAME_ADDRLEN_24BITS        (QSPI_INSTRFRAME_ADDRLEN_24BITS_Val << QSPI_INSTRFRAME_ADDRLEN_Pos) /**< (QSPI_INSTRFRAME) 24-bits address length Position  */
#define QSPI_INSTRFRAME_ADDRLEN_32BITS        (QSPI_INSTRFRAME_ADDRLEN_32BITS_Val << QSPI_INSTRFRAME_ADDRLEN_Pos) /**< (QSPI_INSTRFRAME) 32-bits address length Position  */
#define QSPI_INSTRFRAME_TFRTYPE_Pos           _U_(12)                                              /**< (QSPI_INSTRFRAME) Data Transfer Type Position */
#define QSPI_INSTRFRAME_TFRTYPE_Msk           (_U_(0x3) << QSPI_INSTRFRAME_TFRTYPE_Pos)            /**< (QSPI_INSTRFRAME) Data Transfer Type Mask */
#define QSPI_INSTRFRAME_TFRTYPE(value)        (QSPI_INSTRFRAME_TFRTYPE_Msk & ((value) << QSPI_INSTRFRAME_TFRTYPE_Pos))
#define   QSPI_INSTRFRAME_TFRTYPE_READ_Val    _U_(0x0)                                             /**< (QSPI_INSTRFRAME) Read transfer from the serial memory.Scrambling is not performed.Read at random location (fetch) in the serial flash memory is not possible.  */
#define   QSPI_INSTRFRAME_TFRTYPE_READMEMORY_Val _U_(0x1)                                             /**< (QSPI_INSTRFRAME) Read data transfer from the serial memory.If enabled, scrambling is performed.Read at random location (fetch) in the serial flash memory is possible.  */
#define   QSPI_INSTRFRAME_TFRTYPE_WRITE_Val   _U_(0x2)                                             /**< (QSPI_INSTRFRAME) Write transfer into the serial memory.Scrambling is not performed.  */
#define   QSPI_INSTRFRAME_TFRTYPE_WRITEMEMORY_Val _U_(0x3)                                             /**< (QSPI_INSTRFRAME) Write data transfer into the serial memory.If enabled, scrambling is performed.  */
#define QSPI_INSTRFRAME_TFRTYPE_READ          (QSPI_INSTRFRAME_TFRTYPE_READ_Val << QSPI_INSTRFRAME_TFRTYPE_Pos) /**< (QSPI_INSTRFRAME) Read transfer from the serial memory.Scrambling is not performed.Read at random location (fetch) in the serial flash memory is not possible. Position  */
#define QSPI_INSTRFRAME_TFRTYPE_READMEMORY    (QSPI_INSTRFRAME_TFRTYPE_READMEMORY_Val << QSPI_INSTRFRAME_TFRTYPE_Pos) /**< (QSPI_INSTRFRAME) Read data transfer from the serial memory.If enabled, scrambling is performed.Read at random location (fetch) in the serial flash memory is possible. Position  */
#define QSPI_INSTRFRAME_TFRTYPE_WRITE         (QSPI_INSTRFRAME_TFRTYPE_WRITE_Val << QSPI_INSTRFRAME_TFRTYPE_Pos) /**< (QSPI_INSTRFRAME) Write transfer into the serial memory.Scrambling is not performed. Position  */
#define QSPI_INSTRFRAME_TFRTYPE_WRITEMEMORY   (QSPI_INSTRFRAME_TFRTYPE_WRITEMEMORY_Val << QSPI_INSTRFRAME_TFRTYPE_Pos) /**< (QSPI_INSTRFRAME) Write data transfer into the serial memory.If enabled, scrambling is performed. Position  */
#define QSPI_INSTRFRAME_CRMODE_Pos            _U_(14)                                              /**< (QSPI_INSTRFRAME) Continuous Read Mode Position */
#define QSPI_INSTRFRAME_CRMODE_Msk            (_U_(0x1) << QSPI_INSTRFRAME_CRMODE_Pos)             /**< (QSPI_INSTRFRAME) Continuous Read Mode Mask */
#define QSPI_INSTRFRAME_CRMODE(value)         (QSPI_INSTRFRAME_CRMODE_Msk & ((value) << QSPI_INSTRFRAME_CRMODE_Pos))
#define QSPI_INSTRFRAME_DDREN_Pos             _U_(15)                                              /**< (QSPI_INSTRFRAME) Double Data Rate Enable Position */
#define QSPI_INSTRFRAME_DDREN_Msk             (_U_(0x1) << QSPI_INSTRFRAME_DDREN_Pos)              /**< (QSPI_INSTRFRAME) Double Data Rate Enable Mask */
#define QSPI_INSTRFRAME_DDREN(value)          (QSPI_INSTRFRAME_DDREN_Msk & ((value) << QSPI_INSTRFRAME_DDREN_Pos))
#define QSPI_INSTRFRAME_DUMMYLEN_Pos          _U_(16)                                              /**< (QSPI_INSTRFRAME) Dummy Cycles Length Position */
#define QSPI_INSTRFRAME_DUMMYLEN_Msk          (_U_(0x1F) << QSPI_INSTRFRAME_DUMMYLEN_Pos)          /**< (QSPI_INSTRFRAME) Dummy Cycles Length Mask */
#define QSPI_INSTRFRAME_DUMMYLEN(value)       (QSPI_INSTRFRAME_DUMMYLEN_Msk & ((value) << QSPI_INSTRFRAME_DUMMYLEN_Pos))
#define QSPI_INSTRFRAME_Msk                   _U_(0x001FF7F7)                                      /**< (QSPI_INSTRFRAME) Register Mask  */


/* -------- QSPI_SCRAMBCTRL : (QSPI Offset: 0x40) (R/W 32) Scrambling Mode -------- */
#define QSPI_SCRAMBCTRL_RESETVALUE            _U_(0x00)                                            /**<  (QSPI_SCRAMBCTRL) Scrambling Mode  Reset Value */

#define QSPI_SCRAMBCTRL_ENABLE_Pos            _U_(0)                                               /**< (QSPI_SCRAMBCTRL) Scrambling/Unscrambling Enable Position */
#define QSPI_SCRAMBCTRL_ENABLE_Msk            (_U_(0x1) << QSPI_SCRAMBCTRL_ENABLE_Pos)             /**< (QSPI_SCRAMBCTRL) Scrambling/Unscrambling Enable Mask */
#define QSPI_SCRAMBCTRL_ENABLE(value)         (QSPI_SCRAMBCTRL_ENABLE_Msk & ((value) << QSPI_SCRAMBCTRL_ENABLE_Pos))
#define QSPI_SCRAMBCTRL_RANDOMDIS_Pos         _U_(1)                                               /**< (QSPI_SCRAMBCTRL) Scrambling/Unscrambling Random Value Disable Position */
#define QSPI_SCRAMBCTRL_RANDOMDIS_Msk         (_U_(0x1) << QSPI_SCRAMBCTRL_RANDOMDIS_Pos)          /**< (QSPI_SCRAMBCTRL) Scrambling/Unscrambling Random Value Disable Mask */
#define QSPI_SCRAMBCTRL_RANDOMDIS(value)      (QSPI_SCRAMBCTRL_RANDOMDIS_Msk & ((value) << QSPI_SCRAMBCTRL_RANDOMDIS_Pos))
#define QSPI_SCRAMBCTRL_Msk                   _U_(0x00000003)                                      /**< (QSPI_SCRAMBCTRL) Register Mask  */


/* -------- QSPI_SCRAMBKEY : (QSPI Offset: 0x44) ( /W 32) Scrambling Key -------- */
#define QSPI_SCRAMBKEY_RESETVALUE             _U_(0x00)                                            /**<  (QSPI_SCRAMBKEY) Scrambling Key  Reset Value */

#define QSPI_SCRAMBKEY_KEY_Pos                _U_(0)                                               /**< (QSPI_SCRAMBKEY) Scrambling User Key Position */
#define QSPI_SCRAMBKEY_KEY_Msk                (_U_(0xFFFFFFFF) << QSPI_SCRAMBKEY_KEY_Pos)          /**< (QSPI_SCRAMBKEY) Scrambling User Key Mask */
#define QSPI_SCRAMBKEY_KEY(value)             (QSPI_SCRAMBKEY_KEY_Msk & ((value) << QSPI_SCRAMBKEY_KEY_Pos))
#define QSPI_SCRAMBKEY_Msk                    _U_(0xFFFFFFFF)                                      /**< (QSPI_SCRAMBKEY) Register Mask  */


/** \brief QSPI register offsets definitions */
#define QSPI_CTRLA_REG_OFST            (0x00)              /**< (QSPI_CTRLA) Control A Offset */
#define QSPI_CTRLB_REG_OFST            (0x04)              /**< (QSPI_CTRLB) Control B Offset */
#define QSPI_BAUD_REG_OFST             (0x08)              /**< (QSPI_BAUD) Baud Rate Offset */
#define QSPI_RXDATA_REG_OFST           (0x0C)              /**< (QSPI_RXDATA) Receive Data Offset */
#define QSPI_TXDATA_REG_OFST           (0x10)              /**< (QSPI_TXDATA) Transmit Data Offset */
#define QSPI_INTENCLR_REG_OFST         (0x14)              /**< (QSPI_INTENCLR) Interrupt Enable Clear Offset */
#define QSPI_INTENSET_REG_OFST         (0x18)              /**< (QSPI_INTENSET) Interrupt Enable Set Offset */
#define QSPI_INTFLAG_REG_OFST          (0x1C)              /**< (QSPI_INTFLAG) Interrupt Flag Status and Clear Offset */
#define QSPI_STATUS_REG_OFST           (0x20)              /**< (QSPI_STATUS) Status Register Offset */
#define QSPI_INSTRADDR_REG_OFST        (0x30)              /**< (QSPI_INSTRADDR) Instruction Address Offset */
#define QSPI_INSTRCTRL_REG_OFST        (0x34)              /**< (QSPI_INSTRCTRL) Instruction Code Offset */
#define QSPI_INSTRFRAME_REG_OFST       (0x38)              /**< (QSPI_INSTRFRAME) Instruction Frame Offset */
#define QSPI_SCRAMBCTRL_REG_OFST       (0x40)              /**< (QSPI_SCRAMBCTRL) Scrambling Mode Offset */
#define QSPI_SCRAMBKEY_REG_OFST        (0x44)              /**< (QSPI_SCRAMBKEY) Scrambling Key Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief QSPI register API structure */
typedef struct
{  /* Quad SPI interface */
  __IO  uint32_t                       QSPI_CTRLA;         /**< Offset: 0x00 (R/W  32) Control A */
  __IO  uint32_t                       QSPI_CTRLB;         /**< Offset: 0x04 (R/W  32) Control B */
  __IO  uint32_t                       QSPI_BAUD;          /**< Offset: 0x08 (R/W  32) Baud Rate */
  __I   uint32_t                       QSPI_RXDATA;        /**< Offset: 0x0C (R/   32) Receive Data */
  __O   uint32_t                       QSPI_TXDATA;        /**< Offset: 0x10 ( /W  32) Transmit Data */
  __IO  uint32_t                       QSPI_INTENCLR;      /**< Offset: 0x14 (R/W  32) Interrupt Enable Clear */
  __IO  uint32_t                       QSPI_INTENSET;      /**< Offset: 0x18 (R/W  32) Interrupt Enable Set */
  __IO  uint32_t                       QSPI_INTFLAG;       /**< Offset: 0x1C (R/W  32) Interrupt Flag Status and Clear */
  __I   uint32_t                       QSPI_STATUS;        /**< Offset: 0x20 (R/   32) Status Register */
  __I   uint8_t                        Reserved1[0x0C];
  __IO  uint32_t                       QSPI_INSTRADDR;     /**< Offset: 0x30 (R/W  32) Instruction Address */
  __IO  uint32_t                       QSPI_INSTRCTRL;     /**< Offset: 0x34 (R/W  32) Instruction Code */
  __IO  uint32_t                       QSPI_INSTRFRAME;    /**< Offset: 0x38 (R/W  32) Instruction Frame */
  __I   uint8_t                        Reserved2[0x04];
  __IO  uint32_t                       QSPI_SCRAMBCTRL;    /**< Offset: 0x40 (R/W  32) Scrambling Mode */
  __O   uint32_t                       QSPI_SCRAMBKEY;     /**< Offset: 0x44 ( /W  32) Scrambling Key */
} qspi_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_QSPI_COMPONENT_H_ */
