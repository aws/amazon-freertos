/**
 * \brief Component description for USART
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
#ifndef _SAME70_USART_COMPONENT_H_
#define _SAME70_USART_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR USART                                        */
/* ************************************************************************** */

/* -------- US_CR : (USART Offset: 0x00) ( /W 32) Control Register -------- */
#define US_CR_Msk                             _U_(0x00000000)                                      /**< (US_CR) Register Mask  */

/* USART mode */
#define US_CR_USART_RSTRX_Pos                 _U_(2)                                               /**< (US_CR) Reset Receiver Position */
#define US_CR_USART_RSTRX_Msk                 (_U_(0x1) << US_CR_USART_RSTRX_Pos)                  /**< (US_CR) Reset Receiver Mask */
#define US_CR_USART_RSTRX(value)              (US_CR_USART_RSTRX_Msk & ((value) << US_CR_USART_RSTRX_Pos))
#define US_CR_USART_RSTTX_Pos                 _U_(3)                                               /**< (US_CR) Reset Transmitter Position */
#define US_CR_USART_RSTTX_Msk                 (_U_(0x1) << US_CR_USART_RSTTX_Pos)                  /**< (US_CR) Reset Transmitter Mask */
#define US_CR_USART_RSTTX(value)              (US_CR_USART_RSTTX_Msk & ((value) << US_CR_USART_RSTTX_Pos))
#define US_CR_USART_RXEN_Pos                  _U_(4)                                               /**< (US_CR) Receiver Enable Position */
#define US_CR_USART_RXEN_Msk                  (_U_(0x1) << US_CR_USART_RXEN_Pos)                   /**< (US_CR) Receiver Enable Mask */
#define US_CR_USART_RXEN(value)               (US_CR_USART_RXEN_Msk & ((value) << US_CR_USART_RXEN_Pos))
#define US_CR_USART_RXDIS_Pos                 _U_(5)                                               /**< (US_CR) Receiver Disable Position */
#define US_CR_USART_RXDIS_Msk                 (_U_(0x1) << US_CR_USART_RXDIS_Pos)                  /**< (US_CR) Receiver Disable Mask */
#define US_CR_USART_RXDIS(value)              (US_CR_USART_RXDIS_Msk & ((value) << US_CR_USART_RXDIS_Pos))
#define US_CR_USART_TXEN_Pos                  _U_(6)                                               /**< (US_CR) Transmitter Enable Position */
#define US_CR_USART_TXEN_Msk                  (_U_(0x1) << US_CR_USART_TXEN_Pos)                   /**< (US_CR) Transmitter Enable Mask */
#define US_CR_USART_TXEN(value)               (US_CR_USART_TXEN_Msk & ((value) << US_CR_USART_TXEN_Pos))
#define US_CR_USART_TXDIS_Pos                 _U_(7)                                               /**< (US_CR) Transmitter Disable Position */
#define US_CR_USART_TXDIS_Msk                 (_U_(0x1) << US_CR_USART_TXDIS_Pos)                  /**< (US_CR) Transmitter Disable Mask */
#define US_CR_USART_TXDIS(value)              (US_CR_USART_TXDIS_Msk & ((value) << US_CR_USART_TXDIS_Pos))
#define US_CR_USART_RSTSTA_Pos                _U_(8)                                               /**< (US_CR) Reset Status Bits Position */
#define US_CR_USART_RSTSTA_Msk                (_U_(0x1) << US_CR_USART_RSTSTA_Pos)                 /**< (US_CR) Reset Status Bits Mask */
#define US_CR_USART_RSTSTA(value)             (US_CR_USART_RSTSTA_Msk & ((value) << US_CR_USART_RSTSTA_Pos))
#define US_CR_USART_STTBRK_Pos                _U_(9)                                               /**< (US_CR) Start Break Position */
#define US_CR_USART_STTBRK_Msk                (_U_(0x1) << US_CR_USART_STTBRK_Pos)                 /**< (US_CR) Start Break Mask */
#define US_CR_USART_STTBRK(value)             (US_CR_USART_STTBRK_Msk & ((value) << US_CR_USART_STTBRK_Pos))
#define US_CR_USART_STPBRK_Pos                _U_(10)                                              /**< (US_CR) Stop Break Position */
#define US_CR_USART_STPBRK_Msk                (_U_(0x1) << US_CR_USART_STPBRK_Pos)                 /**< (US_CR) Stop Break Mask */
#define US_CR_USART_STPBRK(value)             (US_CR_USART_STPBRK_Msk & ((value) << US_CR_USART_STPBRK_Pos))
#define US_CR_USART_STTTO_Pos                 _U_(11)                                              /**< (US_CR) Clear TIMEOUT Flag and Start Timeout After Next Character Received Position */
#define US_CR_USART_STTTO_Msk                 (_U_(0x1) << US_CR_USART_STTTO_Pos)                  /**< (US_CR) Clear TIMEOUT Flag and Start Timeout After Next Character Received Mask */
#define US_CR_USART_STTTO(value)              (US_CR_USART_STTTO_Msk & ((value) << US_CR_USART_STTTO_Pos))
#define US_CR_USART_SENDA_Pos                 _U_(12)                                              /**< (US_CR) Send Address Position */
#define US_CR_USART_SENDA_Msk                 (_U_(0x1) << US_CR_USART_SENDA_Pos)                  /**< (US_CR) Send Address Mask */
#define US_CR_USART_SENDA(value)              (US_CR_USART_SENDA_Msk & ((value) << US_CR_USART_SENDA_Pos))
#define US_CR_USART_RSTIT_Pos                 _U_(13)                                              /**< (US_CR) Reset Iterations Position */
#define US_CR_USART_RSTIT_Msk                 (_U_(0x1) << US_CR_USART_RSTIT_Pos)                  /**< (US_CR) Reset Iterations Mask */
#define US_CR_USART_RSTIT(value)              (US_CR_USART_RSTIT_Msk & ((value) << US_CR_USART_RSTIT_Pos))
#define US_CR_USART_RSTNACK_Pos               _U_(14)                                              /**< (US_CR) Reset Non Acknowledge Position */
#define US_CR_USART_RSTNACK_Msk               (_U_(0x1) << US_CR_USART_RSTNACK_Pos)                /**< (US_CR) Reset Non Acknowledge Mask */
#define US_CR_USART_RSTNACK(value)            (US_CR_USART_RSTNACK_Msk & ((value) << US_CR_USART_RSTNACK_Pos))
#define US_CR_USART_RETTO_Pos                 _U_(15)                                              /**< (US_CR) Start Timeout Immediately Position */
#define US_CR_USART_RETTO_Msk                 (_U_(0x1) << US_CR_USART_RETTO_Pos)                  /**< (US_CR) Start Timeout Immediately Mask */
#define US_CR_USART_RETTO(value)              (US_CR_USART_RETTO_Msk & ((value) << US_CR_USART_RETTO_Pos))
#define US_CR_USART_DTREN_Pos                 _U_(16)                                              /**< (US_CR) Data Terminal Ready Enable Position */
#define US_CR_USART_DTREN_Msk                 (_U_(0x1) << US_CR_USART_DTREN_Pos)                  /**< (US_CR) Data Terminal Ready Enable Mask */
#define US_CR_USART_DTREN(value)              (US_CR_USART_DTREN_Msk & ((value) << US_CR_USART_DTREN_Pos))
#define US_CR_USART_DTRDIS_Pos                _U_(17)                                              /**< (US_CR) Data Terminal Ready Disable Position */
#define US_CR_USART_DTRDIS_Msk                (_U_(0x1) << US_CR_USART_DTRDIS_Pos)                 /**< (US_CR) Data Terminal Ready Disable Mask */
#define US_CR_USART_DTRDIS(value)             (US_CR_USART_DTRDIS_Msk & ((value) << US_CR_USART_DTRDIS_Pos))
#define US_CR_USART_RTSEN_Pos                 _U_(18)                                              /**< (US_CR) Request to Send Enable Position */
#define US_CR_USART_RTSEN_Msk                 (_U_(0x1) << US_CR_USART_RTSEN_Pos)                  /**< (US_CR) Request to Send Enable Mask */
#define US_CR_USART_RTSEN(value)              (US_CR_USART_RTSEN_Msk & ((value) << US_CR_USART_RTSEN_Pos))
#define US_CR_USART_RTSDIS_Pos                _U_(19)                                              /**< (US_CR) Request to Send Disable Position */
#define US_CR_USART_RTSDIS_Msk                (_U_(0x1) << US_CR_USART_RTSDIS_Pos)                 /**< (US_CR) Request to Send Disable Mask */
#define US_CR_USART_RTSDIS(value)             (US_CR_USART_RTSDIS_Msk & ((value) << US_CR_USART_RTSDIS_Pos))
#define US_CR_USART_Msk                       _U_(0x000FFFFC)                                       /**< (US_CR_USART) Register Mask  */

/* SPI mode */
#define US_CR_SPI_RSTRX_Pos                   _U_(2)                                               /**< (US_CR) Reset Receiver Position */
#define US_CR_SPI_RSTRX_Msk                   (_U_(0x1) << US_CR_SPI_RSTRX_Pos)                    /**< (US_CR) Reset Receiver Mask */
#define US_CR_SPI_RSTRX(value)                (US_CR_SPI_RSTRX_Msk & ((value) << US_CR_SPI_RSTRX_Pos))
#define US_CR_SPI_RSTTX_Pos                   _U_(3)                                               /**< (US_CR) Reset Transmitter Position */
#define US_CR_SPI_RSTTX_Msk                   (_U_(0x1) << US_CR_SPI_RSTTX_Pos)                    /**< (US_CR) Reset Transmitter Mask */
#define US_CR_SPI_RSTTX(value)                (US_CR_SPI_RSTTX_Msk & ((value) << US_CR_SPI_RSTTX_Pos))
#define US_CR_SPI_RXEN_Pos                    _U_(4)                                               /**< (US_CR) Receiver Enable Position */
#define US_CR_SPI_RXEN_Msk                    (_U_(0x1) << US_CR_SPI_RXEN_Pos)                     /**< (US_CR) Receiver Enable Mask */
#define US_CR_SPI_RXEN(value)                 (US_CR_SPI_RXEN_Msk & ((value) << US_CR_SPI_RXEN_Pos))
#define US_CR_SPI_RXDIS_Pos                   _U_(5)                                               /**< (US_CR) Receiver Disable Position */
#define US_CR_SPI_RXDIS_Msk                   (_U_(0x1) << US_CR_SPI_RXDIS_Pos)                    /**< (US_CR) Receiver Disable Mask */
#define US_CR_SPI_RXDIS(value)                (US_CR_SPI_RXDIS_Msk & ((value) << US_CR_SPI_RXDIS_Pos))
#define US_CR_SPI_TXEN_Pos                    _U_(6)                                               /**< (US_CR) Transmitter Enable Position */
#define US_CR_SPI_TXEN_Msk                    (_U_(0x1) << US_CR_SPI_TXEN_Pos)                     /**< (US_CR) Transmitter Enable Mask */
#define US_CR_SPI_TXEN(value)                 (US_CR_SPI_TXEN_Msk & ((value) << US_CR_SPI_TXEN_Pos))
#define US_CR_SPI_TXDIS_Pos                   _U_(7)                                               /**< (US_CR) Transmitter Disable Position */
#define US_CR_SPI_TXDIS_Msk                   (_U_(0x1) << US_CR_SPI_TXDIS_Pos)                    /**< (US_CR) Transmitter Disable Mask */
#define US_CR_SPI_TXDIS(value)                (US_CR_SPI_TXDIS_Msk & ((value) << US_CR_SPI_TXDIS_Pos))
#define US_CR_SPI_RSTSTA_Pos                  _U_(8)                                               /**< (US_CR) Reset Status Bits Position */
#define US_CR_SPI_RSTSTA_Msk                  (_U_(0x1) << US_CR_SPI_RSTSTA_Pos)                   /**< (US_CR) Reset Status Bits Mask */
#define US_CR_SPI_RSTSTA(value)               (US_CR_SPI_RSTSTA_Msk & ((value) << US_CR_SPI_RSTSTA_Pos))
#define US_CR_SPI_FCS_Pos                     _U_(18)                                              /**< (US_CR) Force SPI Chip Select Position */
#define US_CR_SPI_FCS_Msk                     (_U_(0x1) << US_CR_SPI_FCS_Pos)                      /**< (US_CR) Force SPI Chip Select Mask */
#define US_CR_SPI_FCS(value)                  (US_CR_SPI_FCS_Msk & ((value) << US_CR_SPI_FCS_Pos))
#define US_CR_SPI_RCS_Pos                     _U_(19)                                              /**< (US_CR) Release SPI Chip Select Position */
#define US_CR_SPI_RCS_Msk                     (_U_(0x1) << US_CR_SPI_RCS_Pos)                      /**< (US_CR) Release SPI Chip Select Mask */
#define US_CR_SPI_RCS(value)                  (US_CR_SPI_RCS_Msk & ((value) << US_CR_SPI_RCS_Pos))
#define US_CR_SPI_Msk                         _U_(0x000C01FC)                                       /**< (US_CR_SPI) Register Mask  */

/* LIN mode */
#define US_CR_LIN_RSTRX_Pos                   _U_(2)                                               /**< (US_CR) Reset Receiver Position */
#define US_CR_LIN_RSTRX_Msk                   (_U_(0x1) << US_CR_LIN_RSTRX_Pos)                    /**< (US_CR) Reset Receiver Mask */
#define US_CR_LIN_RSTRX(value)                (US_CR_LIN_RSTRX_Msk & ((value) << US_CR_LIN_RSTRX_Pos))
#define US_CR_LIN_RSTTX_Pos                   _U_(3)                                               /**< (US_CR) Reset Transmitter Position */
#define US_CR_LIN_RSTTX_Msk                   (_U_(0x1) << US_CR_LIN_RSTTX_Pos)                    /**< (US_CR) Reset Transmitter Mask */
#define US_CR_LIN_RSTTX(value)                (US_CR_LIN_RSTTX_Msk & ((value) << US_CR_LIN_RSTTX_Pos))
#define US_CR_LIN_RXEN_Pos                    _U_(4)                                               /**< (US_CR) Receiver Enable Position */
#define US_CR_LIN_RXEN_Msk                    (_U_(0x1) << US_CR_LIN_RXEN_Pos)                     /**< (US_CR) Receiver Enable Mask */
#define US_CR_LIN_RXEN(value)                 (US_CR_LIN_RXEN_Msk & ((value) << US_CR_LIN_RXEN_Pos))
#define US_CR_LIN_RXDIS_Pos                   _U_(5)                                               /**< (US_CR) Receiver Disable Position */
#define US_CR_LIN_RXDIS_Msk                   (_U_(0x1) << US_CR_LIN_RXDIS_Pos)                    /**< (US_CR) Receiver Disable Mask */
#define US_CR_LIN_RXDIS(value)                (US_CR_LIN_RXDIS_Msk & ((value) << US_CR_LIN_RXDIS_Pos))
#define US_CR_LIN_TXEN_Pos                    _U_(6)                                               /**< (US_CR) Transmitter Enable Position */
#define US_CR_LIN_TXEN_Msk                    (_U_(0x1) << US_CR_LIN_TXEN_Pos)                     /**< (US_CR) Transmitter Enable Mask */
#define US_CR_LIN_TXEN(value)                 (US_CR_LIN_TXEN_Msk & ((value) << US_CR_LIN_TXEN_Pos))
#define US_CR_LIN_TXDIS_Pos                   _U_(7)                                               /**< (US_CR) Transmitter Disable Position */
#define US_CR_LIN_TXDIS_Msk                   (_U_(0x1) << US_CR_LIN_TXDIS_Pos)                    /**< (US_CR) Transmitter Disable Mask */
#define US_CR_LIN_TXDIS(value)                (US_CR_LIN_TXDIS_Msk & ((value) << US_CR_LIN_TXDIS_Pos))
#define US_CR_LIN_RSTSTA_Pos                  _U_(8)                                               /**< (US_CR) Reset Status Bits Position */
#define US_CR_LIN_RSTSTA_Msk                  (_U_(0x1) << US_CR_LIN_RSTSTA_Pos)                   /**< (US_CR) Reset Status Bits Mask */
#define US_CR_LIN_RSTSTA(value)               (US_CR_LIN_RSTSTA_Msk & ((value) << US_CR_LIN_RSTSTA_Pos))
#define US_CR_LIN_LINABT_Pos                  _U_(20)                                              /**< (US_CR) Abort LIN Transmission Position */
#define US_CR_LIN_LINABT_Msk                  (_U_(0x1) << US_CR_LIN_LINABT_Pos)                   /**< (US_CR) Abort LIN Transmission Mask */
#define US_CR_LIN_LINABT(value)               (US_CR_LIN_LINABT_Msk & ((value) << US_CR_LIN_LINABT_Pos))
#define US_CR_LIN_LINWKUP_Pos                 _U_(21)                                              /**< (US_CR) Send LIN Wakeup Signal Position */
#define US_CR_LIN_LINWKUP_Msk                 (_U_(0x1) << US_CR_LIN_LINWKUP_Pos)                  /**< (US_CR) Send LIN Wakeup Signal Mask */
#define US_CR_LIN_LINWKUP(value)              (US_CR_LIN_LINWKUP_Msk & ((value) << US_CR_LIN_LINWKUP_Pos))
#define US_CR_LIN_Msk                         _U_(0x003001FC)                                       /**< (US_CR_LIN) Register Mask  */


/* -------- US_MR : (USART Offset: 0x04) (R/W 32) Mode Register -------- */
#define US_MR_Msk                             _U_(0x00000000)                                      /**< (US_MR) Register Mask  */

/* USART mode */
#define US_MR_USART_MODE_Pos                  _U_(0)                                               /**< (US_MR) USART Mode of Operation Position */
#define US_MR_USART_MODE_Msk                  (_U_(0xF) << US_MR_USART_MODE_Pos)                   /**< (US_MR) USART Mode of Operation Mask */
#define US_MR_USART_MODE(value)               (US_MR_USART_MODE_Msk & ((value) << US_MR_USART_MODE_Pos))
#define   US_MR_USART_MODE_NORMAL_Val         _U_(0x0)                                             /**< (US_MR) Normal mode  */
#define   US_MR_USART_MODE_RS485_Val          _U_(0x1)                                             /**< (US_MR) RS485  */
#define   US_MR_USART_MODE_HW_HANDSHAKING_Val _U_(0x2)                                             /**< (US_MR) Hardware handshaking  */
#define   US_MR_USART_MODE_MODEM_Val          _U_(0x3)                                             /**< (US_MR) Modem  */
#define   US_MR_USART_MODE_IS07816_T_0_Val    _U_(0x4)                                             /**< (US_MR) IS07816 Protocol: T = 0  */
#define   US_MR_USART_MODE_IS07816_T_1_Val    _U_(0x6)                                             /**< (US_MR) IS07816 Protocol: T = 1  */
#define   US_MR_USART_MODE_IRDA_Val           _U_(0x8)                                             /**< (US_MR) IrDA  */
#define   US_MR_USART_MODE_LON_Val            _U_(0x9)                                             /**< (US_MR) LON  */
#define   US_MR_USART_MODE_LIN_MASTER_Val     _U_(0xA)                                             /**< (US_MR) LIN Master mode  */
#define   US_MR_USART_MODE_LIN_SLAVE_Val      _U_(0xB)                                             /**< (US_MR) LIN Slave mode  */
#define   US_MR_USART_MODE_SPI_MASTER_Val     _U_(0xE)                                             /**< (US_MR) SPI Master mode (CLKO must be written to 1 and USCLKS = 0, 1 or 2)  */
#define   US_MR_USART_MODE_SPI_SLAVE_Val      _U_(0xF)                                             /**< (US_MR) SPI Slave mode  */
#define US_MR_USART_MODE_NORMAL               (US_MR_USART_MODE_NORMAL_Val << US_MR_USART_MODE_Pos) /**< (US_MR) Normal mode Position  */
#define US_MR_USART_MODE_RS485                (US_MR_USART_MODE_RS485_Val << US_MR_USART_MODE_Pos) /**< (US_MR) RS485 Position  */
#define US_MR_USART_MODE_HW_HANDSHAKING       (US_MR_USART_MODE_HW_HANDSHAKING_Val << US_MR_USART_MODE_Pos) /**< (US_MR) Hardware handshaking Position  */
#define US_MR_USART_MODE_MODEM                (US_MR_USART_MODE_MODEM_Val << US_MR_USART_MODE_Pos) /**< (US_MR) Modem Position  */
#define US_MR_USART_MODE_IS07816_T_0          (US_MR_USART_MODE_IS07816_T_0_Val << US_MR_USART_MODE_Pos) /**< (US_MR) IS07816 Protocol: T = 0 Position  */
#define US_MR_USART_MODE_IS07816_T_1          (US_MR_USART_MODE_IS07816_T_1_Val << US_MR_USART_MODE_Pos) /**< (US_MR) IS07816 Protocol: T = 1 Position  */
#define US_MR_USART_MODE_IRDA                 (US_MR_USART_MODE_IRDA_Val << US_MR_USART_MODE_Pos)  /**< (US_MR) IrDA Position  */
#define US_MR_USART_MODE_LON                  (US_MR_USART_MODE_LON_Val << US_MR_USART_MODE_Pos)   /**< (US_MR) LON Position  */
#define US_MR_USART_MODE_LIN_MASTER           (US_MR_USART_MODE_LIN_MASTER_Val << US_MR_USART_MODE_Pos) /**< (US_MR) LIN Master mode Position  */
#define US_MR_USART_MODE_LIN_SLAVE            (US_MR_USART_MODE_LIN_SLAVE_Val << US_MR_USART_MODE_Pos) /**< (US_MR) LIN Slave mode Position  */
#define US_MR_USART_MODE_SPI_MASTER           (US_MR_USART_MODE_SPI_MASTER_Val << US_MR_USART_MODE_Pos) /**< (US_MR) SPI Master mode (CLKO must be written to 1 and USCLKS = 0, 1 or 2) Position  */
#define US_MR_USART_MODE_SPI_SLAVE            (US_MR_USART_MODE_SPI_SLAVE_Val << US_MR_USART_MODE_Pos) /**< (US_MR) SPI Slave mode Position  */
#define US_MR_USART_USCLKS_Pos                _U_(4)                                               /**< (US_MR) Clock Selection Position */
#define US_MR_USART_USCLKS_Msk                (_U_(0x3) << US_MR_USART_USCLKS_Pos)                 /**< (US_MR) Clock Selection Mask */
#define US_MR_USART_USCLKS(value)             (US_MR_USART_USCLKS_Msk & ((value) << US_MR_USART_USCLKS_Pos))
#define   US_MR_USART_USCLKS_MCK_Val          _U_(0x0)                                             /**< (US_MR) Peripheral clock is selected  */
#define   US_MR_USART_USCLKS_DIV_Val          _U_(0x1)                                             /**< (US_MR) Peripheral clock divided (DIV = 8) is selected  */
#define   US_MR_USART_USCLKS_PCK_Val          _U_(0x2)                                             /**< (US_MR) PMC programmable clock (PCK) is selected. If the SCK pin is driven (CLKO = 1), the CD field must be greater than 1.  */
#define   US_MR_USART_USCLKS_SCK_Val          _U_(0x3)                                             /**< (US_MR) Serial clock (SCK) is selected  */
#define US_MR_USART_USCLKS_MCK                (US_MR_USART_USCLKS_MCK_Val << US_MR_USART_USCLKS_Pos) /**< (US_MR) Peripheral clock is selected Position  */
#define US_MR_USART_USCLKS_DIV                (US_MR_USART_USCLKS_DIV_Val << US_MR_USART_USCLKS_Pos) /**< (US_MR) Peripheral clock divided (DIV = 8) is selected Position  */
#define US_MR_USART_USCLKS_PCK                (US_MR_USART_USCLKS_PCK_Val << US_MR_USART_USCLKS_Pos) /**< (US_MR) PMC programmable clock (PCK) is selected. If the SCK pin is driven (CLKO = 1), the CD field must be greater than 1. Position  */
#define US_MR_USART_USCLKS_SCK                (US_MR_USART_USCLKS_SCK_Val << US_MR_USART_USCLKS_Pos) /**< (US_MR) Serial clock (SCK) is selected Position  */
#define US_MR_USART_CHRL_Pos                  _U_(6)                                               /**< (US_MR) Character Length Position */
#define US_MR_USART_CHRL_Msk                  (_U_(0x3) << US_MR_USART_CHRL_Pos)                   /**< (US_MR) Character Length Mask */
#define US_MR_USART_CHRL(value)               (US_MR_USART_CHRL_Msk & ((value) << US_MR_USART_CHRL_Pos))
#define   US_MR_USART_CHRL_5_BIT_Val          _U_(0x0)                                             /**< (US_MR) Character length is 5 bits  */
#define   US_MR_USART_CHRL_6_BIT_Val          _U_(0x1)                                             /**< (US_MR) Character length is 6 bits  */
#define   US_MR_USART_CHRL_7_BIT_Val          _U_(0x2)                                             /**< (US_MR) Character length is 7 bits  */
#define   US_MR_USART_CHRL_8_BIT_Val          _U_(0x3)                                             /**< (US_MR) Character length is 8 bits  */
#define US_MR_USART_CHRL_5_BIT                (US_MR_USART_CHRL_5_BIT_Val << US_MR_USART_CHRL_Pos) /**< (US_MR) Character length is 5 bits Position  */
#define US_MR_USART_CHRL_6_BIT                (US_MR_USART_CHRL_6_BIT_Val << US_MR_USART_CHRL_Pos) /**< (US_MR) Character length is 6 bits Position  */
#define US_MR_USART_CHRL_7_BIT                (US_MR_USART_CHRL_7_BIT_Val << US_MR_USART_CHRL_Pos) /**< (US_MR) Character length is 7 bits Position  */
#define US_MR_USART_CHRL_8_BIT                (US_MR_USART_CHRL_8_BIT_Val << US_MR_USART_CHRL_Pos) /**< (US_MR) Character length is 8 bits Position  */
#define US_MR_USART_SYNC_Pos                  _U_(8)                                               /**< (US_MR) Synchronous Mode Select Position */
#define US_MR_USART_SYNC_Msk                  (_U_(0x1) << US_MR_USART_SYNC_Pos)                   /**< (US_MR) Synchronous Mode Select Mask */
#define US_MR_USART_SYNC(value)               (US_MR_USART_SYNC_Msk & ((value) << US_MR_USART_SYNC_Pos))
#define US_MR_USART_PAR_Pos                   _U_(9)                                               /**< (US_MR) Parity Type Position */
#define US_MR_USART_PAR_Msk                   (_U_(0x7) << US_MR_USART_PAR_Pos)                    /**< (US_MR) Parity Type Mask */
#define US_MR_USART_PAR(value)                (US_MR_USART_PAR_Msk & ((value) << US_MR_USART_PAR_Pos))
#define   US_MR_USART_PAR_EVEN_Val            _U_(0x0)                                             /**< (US_MR) Even parity  */
#define   US_MR_USART_PAR_ODD_Val             _U_(0x1)                                             /**< (US_MR) Odd parity  */
#define   US_MR_USART_PAR_SPACE_Val           _U_(0x2)                                             /**< (US_MR) Parity forced to 0 (Space)  */
#define   US_MR_USART_PAR_MARK_Val            _U_(0x3)                                             /**< (US_MR) Parity forced to 1 (Mark)  */
#define   US_MR_USART_PAR_NO_Val              _U_(0x4)                                             /**< (US_MR) No parity  */
#define   US_MR_USART_PAR_MULTIDROP_Val       _U_(0x6)                                             /**< (US_MR) Multidrop mode  */
#define US_MR_USART_PAR_EVEN                  (US_MR_USART_PAR_EVEN_Val << US_MR_USART_PAR_Pos)    /**< (US_MR) Even parity Position  */
#define US_MR_USART_PAR_ODD                   (US_MR_USART_PAR_ODD_Val << US_MR_USART_PAR_Pos)     /**< (US_MR) Odd parity Position  */
#define US_MR_USART_PAR_SPACE                 (US_MR_USART_PAR_SPACE_Val << US_MR_USART_PAR_Pos)   /**< (US_MR) Parity forced to 0 (Space) Position  */
#define US_MR_USART_PAR_MARK                  (US_MR_USART_PAR_MARK_Val << US_MR_USART_PAR_Pos)    /**< (US_MR) Parity forced to 1 (Mark) Position  */
#define US_MR_USART_PAR_NO                    (US_MR_USART_PAR_NO_Val << US_MR_USART_PAR_Pos)      /**< (US_MR) No parity Position  */
#define US_MR_USART_PAR_MULTIDROP             (US_MR_USART_PAR_MULTIDROP_Val << US_MR_USART_PAR_Pos) /**< (US_MR) Multidrop mode Position  */
#define US_MR_USART_NBSTOP_Pos                _U_(12)                                              /**< (US_MR) Number of Stop Bits Position */
#define US_MR_USART_NBSTOP_Msk                (_U_(0x3) << US_MR_USART_NBSTOP_Pos)                 /**< (US_MR) Number of Stop Bits Mask */
#define US_MR_USART_NBSTOP(value)             (US_MR_USART_NBSTOP_Msk & ((value) << US_MR_USART_NBSTOP_Pos))
#define   US_MR_USART_NBSTOP_1_BIT_Val        _U_(0x0)                                             /**< (US_MR) 1 stop bit  */
#define   US_MR_USART_NBSTOP_1_5_BIT_Val      _U_(0x1)                                             /**< (US_MR) 1.5 stop bit (SYNC = 0) or reserved (SYNC = 1)  */
#define   US_MR_USART_NBSTOP_2_BIT_Val        _U_(0x2)                                             /**< (US_MR) 2 stop bits  */
#define US_MR_USART_NBSTOP_1_BIT              (US_MR_USART_NBSTOP_1_BIT_Val << US_MR_USART_NBSTOP_Pos) /**< (US_MR) 1 stop bit Position  */
#define US_MR_USART_NBSTOP_1_5_BIT            (US_MR_USART_NBSTOP_1_5_BIT_Val << US_MR_USART_NBSTOP_Pos) /**< (US_MR) 1.5 stop bit (SYNC = 0) or reserved (SYNC = 1) Position  */
#define US_MR_USART_NBSTOP_2_BIT              (US_MR_USART_NBSTOP_2_BIT_Val << US_MR_USART_NBSTOP_Pos) /**< (US_MR) 2 stop bits Position  */
#define US_MR_USART_CHMODE_Pos                _U_(14)                                              /**< (US_MR) Channel Mode Position */
#define US_MR_USART_CHMODE_Msk                (_U_(0x3) << US_MR_USART_CHMODE_Pos)                 /**< (US_MR) Channel Mode Mask */
#define US_MR_USART_CHMODE(value)             (US_MR_USART_CHMODE_Msk & ((value) << US_MR_USART_CHMODE_Pos))
#define   US_MR_USART_CHMODE_NORMAL_Val       _U_(0x0)                                             /**< (US_MR) Normal mode  */
#define   US_MR_USART_CHMODE_AUTOMATIC_Val    _U_(0x1)                                             /**< (US_MR) Automatic Echo. Receiver input is connected to the TXD pin.  */
#define   US_MR_USART_CHMODE_LOCAL_LOOPBACK_Val _U_(0x2)                                             /**< (US_MR) Local Loopback. Transmitter output is connected to the Receiver Input.  */
#define   US_MR_USART_CHMODE_REMOTE_LOOPBACK_Val _U_(0x3)                                             /**< (US_MR) Remote Loopback. RXD pin is internally connected to the TXD pin.  */
#define US_MR_USART_CHMODE_NORMAL             (US_MR_USART_CHMODE_NORMAL_Val << US_MR_USART_CHMODE_Pos) /**< (US_MR) Normal mode Position  */
#define US_MR_USART_CHMODE_AUTOMATIC          (US_MR_USART_CHMODE_AUTOMATIC_Val << US_MR_USART_CHMODE_Pos) /**< (US_MR) Automatic Echo. Receiver input is connected to the TXD pin. Position  */
#define US_MR_USART_CHMODE_LOCAL_LOOPBACK     (US_MR_USART_CHMODE_LOCAL_LOOPBACK_Val << US_MR_USART_CHMODE_Pos) /**< (US_MR) Local Loopback. Transmitter output is connected to the Receiver Input. Position  */
#define US_MR_USART_CHMODE_REMOTE_LOOPBACK    (US_MR_USART_CHMODE_REMOTE_LOOPBACK_Val << US_MR_USART_CHMODE_Pos) /**< (US_MR) Remote Loopback. RXD pin is internally connected to the TXD pin. Position  */
#define US_MR_USART_MSBF_Pos                  _U_(16)                                              /**< (US_MR) Bit Order Position */
#define US_MR_USART_MSBF_Msk                  (_U_(0x1) << US_MR_USART_MSBF_Pos)                   /**< (US_MR) Bit Order Mask */
#define US_MR_USART_MSBF(value)               (US_MR_USART_MSBF_Msk & ((value) << US_MR_USART_MSBF_Pos))
#define US_MR_USART_MODE9_Pos                 _U_(17)                                              /**< (US_MR) 9-bit Character Length Position */
#define US_MR_USART_MODE9_Msk                 (_U_(0x1) << US_MR_USART_MODE9_Pos)                  /**< (US_MR) 9-bit Character Length Mask */
#define US_MR_USART_MODE9(value)              (US_MR_USART_MODE9_Msk & ((value) << US_MR_USART_MODE9_Pos))
#define US_MR_USART_CLKO_Pos                  _U_(18)                                              /**< (US_MR) Clock Output Select Position */
#define US_MR_USART_CLKO_Msk                  (_U_(0x1) << US_MR_USART_CLKO_Pos)                   /**< (US_MR) Clock Output Select Mask */
#define US_MR_USART_CLKO(value)               (US_MR_USART_CLKO_Msk & ((value) << US_MR_USART_CLKO_Pos))
#define US_MR_USART_OVER_Pos                  _U_(19)                                              /**< (US_MR) Oversampling Mode Position */
#define US_MR_USART_OVER_Msk                  (_U_(0x1) << US_MR_USART_OVER_Pos)                   /**< (US_MR) Oversampling Mode Mask */
#define US_MR_USART_OVER(value)               (US_MR_USART_OVER_Msk & ((value) << US_MR_USART_OVER_Pos))
#define US_MR_USART_INACK_Pos                 _U_(20)                                              /**< (US_MR) Inhibit Non Acknowledge Position */
#define US_MR_USART_INACK_Msk                 (_U_(0x1) << US_MR_USART_INACK_Pos)                  /**< (US_MR) Inhibit Non Acknowledge Mask */
#define US_MR_USART_INACK(value)              (US_MR_USART_INACK_Msk & ((value) << US_MR_USART_INACK_Pos))
#define US_MR_USART_DSNACK_Pos                _U_(21)                                              /**< (US_MR) Disable Successive NACK Position */
#define US_MR_USART_DSNACK_Msk                (_U_(0x1) << US_MR_USART_DSNACK_Pos)                 /**< (US_MR) Disable Successive NACK Mask */
#define US_MR_USART_DSNACK(value)             (US_MR_USART_DSNACK_Msk & ((value) << US_MR_USART_DSNACK_Pos))
#define US_MR_USART_VAR_SYNC_Pos              _U_(22)                                              /**< (US_MR) Variable Synchronization of Command/Data Sync Start Frame Delimiter Position */
#define US_MR_USART_VAR_SYNC_Msk              (_U_(0x1) << US_MR_USART_VAR_SYNC_Pos)               /**< (US_MR) Variable Synchronization of Command/Data Sync Start Frame Delimiter Mask */
#define US_MR_USART_VAR_SYNC(value)           (US_MR_USART_VAR_SYNC_Msk & ((value) << US_MR_USART_VAR_SYNC_Pos))
#define US_MR_USART_INVDATA_Pos               _U_(23)                                              /**< (US_MR) Inverted Data Position */
#define US_MR_USART_INVDATA_Msk               (_U_(0x1) << US_MR_USART_INVDATA_Pos)                /**< (US_MR) Inverted Data Mask */
#define US_MR_USART_INVDATA(value)            (US_MR_USART_INVDATA_Msk & ((value) << US_MR_USART_INVDATA_Pos))
#define US_MR_USART_MAX_ITERATION_Pos         _U_(24)                                              /**< (US_MR) Maximum Number of Automatic Iteration Position */
#define US_MR_USART_MAX_ITERATION_Msk         (_U_(0x7) << US_MR_USART_MAX_ITERATION_Pos)          /**< (US_MR) Maximum Number of Automatic Iteration Mask */
#define US_MR_USART_MAX_ITERATION(value)      (US_MR_USART_MAX_ITERATION_Msk & ((value) << US_MR_USART_MAX_ITERATION_Pos))
#define US_MR_USART_FILTER_Pos                _U_(28)                                              /**< (US_MR) Receive Line Filter Position */
#define US_MR_USART_FILTER_Msk                (_U_(0x1) << US_MR_USART_FILTER_Pos)                 /**< (US_MR) Receive Line Filter Mask */
#define US_MR_USART_FILTER(value)             (US_MR_USART_FILTER_Msk & ((value) << US_MR_USART_FILTER_Pos))
#define US_MR_USART_MAN_Pos                   _U_(29)                                              /**< (US_MR) Manchester Encoder/Decoder Enable Position */
#define US_MR_USART_MAN_Msk                   (_U_(0x1) << US_MR_USART_MAN_Pos)                    /**< (US_MR) Manchester Encoder/Decoder Enable Mask */
#define US_MR_USART_MAN(value)                (US_MR_USART_MAN_Msk & ((value) << US_MR_USART_MAN_Pos))
#define US_MR_USART_MODSYNC_Pos               _U_(30)                                              /**< (US_MR) Manchester Synchronization Mode Position */
#define US_MR_USART_MODSYNC_Msk               (_U_(0x1) << US_MR_USART_MODSYNC_Pos)                /**< (US_MR) Manchester Synchronization Mode Mask */
#define US_MR_USART_MODSYNC(value)            (US_MR_USART_MODSYNC_Msk & ((value) << US_MR_USART_MODSYNC_Pos))
#define US_MR_USART_ONEBIT_Pos                _U_(31)                                              /**< (US_MR) Start Frame Delimiter Selector Position */
#define US_MR_USART_ONEBIT_Msk                (_U_(0x1) << US_MR_USART_ONEBIT_Pos)                 /**< (US_MR) Start Frame Delimiter Selector Mask */
#define US_MR_USART_ONEBIT(value)             (US_MR_USART_ONEBIT_Msk & ((value) << US_MR_USART_ONEBIT_Pos))
#define US_MR_USART_Msk                       _U_(0xF7FFFFFF)                                       /**< (US_MR_USART) Register Mask  */

/* SPI mode */
#define US_MR_SPI_USART_MODE_Pos              _U_(0)                                               /**< (US_MR) USART Mode of Operation Position */
#define US_MR_SPI_USART_MODE_Msk              (_U_(0xF) << US_MR_SPI_USART_MODE_Pos)               /**< (US_MR) USART Mode of Operation Mask */
#define US_MR_SPI_USART_MODE(value)           (US_MR_SPI_USART_MODE_Msk & ((value) << US_MR_SPI_USART_MODE_Pos))
#define   US_MR_SPI_USART_MODE_NORMAL_Val     _U_(0x0)                                             /**< (US_MR) Normal mode  */
#define   US_MR_SPI_USART_MODE_RS485_Val      _U_(0x1)                                             /**< (US_MR) RS485  */
#define   US_MR_SPI_USART_MODE_HW_HANDSHAKING_Val _U_(0x2)                                             /**< (US_MR) Hardware handshaking  */
#define   US_MR_SPI_USART_MODE_MODEM_Val      _U_(0x3)                                             /**< (US_MR) Modem  */
#define   US_MR_SPI_USART_MODE_IS07816_T_0_Val _U_(0x4)                                             /**< (US_MR) IS07816 Protocol: T = 0  */
#define   US_MR_SPI_USART_MODE_IS07816_T_1_Val _U_(0x6)                                             /**< (US_MR) IS07816 Protocol: T = 1  */
#define   US_MR_SPI_USART_MODE_IRDA_Val       _U_(0x8)                                             /**< (US_MR) IrDA  */
#define   US_MR_SPI_USART_MODE_LON_Val        _U_(0x9)                                             /**< (US_MR) LON  */
#define   US_MR_SPI_USART_MODE_LIN_MASTER_Val _U_(0xA)                                             /**< (US_MR) LIN Master mode  */
#define   US_MR_SPI_USART_MODE_LIN_SLAVE_Val  _U_(0xB)                                             /**< (US_MR) LIN Slave mode  */
#define   US_MR_SPI_USART_MODE_SPI_MASTER_Val _U_(0xE)                                             /**< (US_MR) SPI Master mode (CLKO must be written to 1 and USCLKS = 0, 1 or 2)  */
#define   US_MR_SPI_USART_MODE_SPI_SLAVE_Val  _U_(0xF)                                             /**< (US_MR) SPI Slave mode  */
#define US_MR_SPI_USART_MODE_NORMAL           (US_MR_SPI_USART_MODE_NORMAL_Val << US_MR_SPI_USART_MODE_Pos) /**< (US_MR) Normal mode Position  */
#define US_MR_SPI_USART_MODE_RS485            (US_MR_SPI_USART_MODE_RS485_Val << US_MR_SPI_USART_MODE_Pos) /**< (US_MR) RS485 Position  */
#define US_MR_SPI_USART_MODE_HW_HANDSHAKING   (US_MR_SPI_USART_MODE_HW_HANDSHAKING_Val << US_MR_SPI_USART_MODE_Pos) /**< (US_MR) Hardware handshaking Position  */
#define US_MR_SPI_USART_MODE_MODEM            (US_MR_SPI_USART_MODE_MODEM_Val << US_MR_SPI_USART_MODE_Pos) /**< (US_MR) Modem Position  */
#define US_MR_SPI_USART_MODE_IS07816_T_0      (US_MR_SPI_USART_MODE_IS07816_T_0_Val << US_MR_SPI_USART_MODE_Pos) /**< (US_MR) IS07816 Protocol: T = 0 Position  */
#define US_MR_SPI_USART_MODE_IS07816_T_1      (US_MR_SPI_USART_MODE_IS07816_T_1_Val << US_MR_SPI_USART_MODE_Pos) /**< (US_MR) IS07816 Protocol: T = 1 Position  */
#define US_MR_SPI_USART_MODE_IRDA             (US_MR_SPI_USART_MODE_IRDA_Val << US_MR_SPI_USART_MODE_Pos) /**< (US_MR) IrDA Position  */
#define US_MR_SPI_USART_MODE_LON              (US_MR_SPI_USART_MODE_LON_Val << US_MR_SPI_USART_MODE_Pos) /**< (US_MR) LON Position  */
#define US_MR_SPI_USART_MODE_LIN_MASTER       (US_MR_SPI_USART_MODE_LIN_MASTER_Val << US_MR_SPI_USART_MODE_Pos) /**< (US_MR) LIN Master mode Position  */
#define US_MR_SPI_USART_MODE_LIN_SLAVE        (US_MR_SPI_USART_MODE_LIN_SLAVE_Val << US_MR_SPI_USART_MODE_Pos) /**< (US_MR) LIN Slave mode Position  */
#define US_MR_SPI_USART_MODE_SPI_MASTER       (US_MR_SPI_USART_MODE_SPI_MASTER_Val << US_MR_SPI_USART_MODE_Pos) /**< (US_MR) SPI Master mode (CLKO must be written to 1 and USCLKS = 0, 1 or 2) Position  */
#define US_MR_SPI_USART_MODE_SPI_SLAVE        (US_MR_SPI_USART_MODE_SPI_SLAVE_Val << US_MR_SPI_USART_MODE_Pos) /**< (US_MR) SPI Slave mode Position  */
#define US_MR_SPI_USCLKS_Pos                  _U_(4)                                               /**< (US_MR) Clock Selection Position */
#define US_MR_SPI_USCLKS_Msk                  (_U_(0x3) << US_MR_SPI_USCLKS_Pos)                   /**< (US_MR) Clock Selection Mask */
#define US_MR_SPI_USCLKS(value)               (US_MR_SPI_USCLKS_Msk & ((value) << US_MR_SPI_USCLKS_Pos))
#define   US_MR_SPI_USCLKS_MCK_Val            _U_(0x0)                                             /**< (US_MR) Peripheral clock is selected  */
#define   US_MR_SPI_USCLKS_DIV_Val            _U_(0x1)                                             /**< (US_MR) Peripheral clock divided (DIV = 8) is selected  */
#define   US_MR_SPI_USCLKS_PCK_Val            _U_(0x2)                                             /**< (US_MR) PMC programmable clock (PCK) is selected. If the SCK pin is driven (CLKO = 1), the CD field must be greater than 1.  */
#define   US_MR_SPI_USCLKS_SCK_Val            _U_(0x3)                                             /**< (US_MR) Serial clock (SCK) is selected  */
#define US_MR_SPI_USCLKS_MCK                  (US_MR_SPI_USCLKS_MCK_Val << US_MR_SPI_USCLKS_Pos)   /**< (US_MR) Peripheral clock is selected Position  */
#define US_MR_SPI_USCLKS_DIV                  (US_MR_SPI_USCLKS_DIV_Val << US_MR_SPI_USCLKS_Pos)   /**< (US_MR) Peripheral clock divided (DIV = 8) is selected Position  */
#define US_MR_SPI_USCLKS_PCK                  (US_MR_SPI_USCLKS_PCK_Val << US_MR_SPI_USCLKS_Pos)   /**< (US_MR) PMC programmable clock (PCK) is selected. If the SCK pin is driven (CLKO = 1), the CD field must be greater than 1. Position  */
#define US_MR_SPI_USCLKS_SCK                  (US_MR_SPI_USCLKS_SCK_Val << US_MR_SPI_USCLKS_Pos)   /**< (US_MR) Serial clock (SCK) is selected Position  */
#define US_MR_SPI_CHRL_Pos                    _U_(6)                                               /**< (US_MR) Character Length Position */
#define US_MR_SPI_CHRL_Msk                    (_U_(0x3) << US_MR_SPI_CHRL_Pos)                     /**< (US_MR) Character Length Mask */
#define US_MR_SPI_CHRL(value)                 (US_MR_SPI_CHRL_Msk & ((value) << US_MR_SPI_CHRL_Pos))
#define   US_MR_SPI_CHRL_5_BIT_Val            _U_(0x0)                                             /**< (US_MR) Character length is 5 bits  */
#define   US_MR_SPI_CHRL_6_BIT_Val            _U_(0x1)                                             /**< (US_MR) Character length is 6 bits  */
#define   US_MR_SPI_CHRL_7_BIT_Val            _U_(0x2)                                             /**< (US_MR) Character length is 7 bits  */
#define   US_MR_SPI_CHRL_8_BIT_Val            _U_(0x3)                                             /**< (US_MR) Character length is 8 bits  */
#define US_MR_SPI_CHRL_5_BIT                  (US_MR_SPI_CHRL_5_BIT_Val << US_MR_SPI_CHRL_Pos)     /**< (US_MR) Character length is 5 bits Position  */
#define US_MR_SPI_CHRL_6_BIT                  (US_MR_SPI_CHRL_6_BIT_Val << US_MR_SPI_CHRL_Pos)     /**< (US_MR) Character length is 6 bits Position  */
#define US_MR_SPI_CHRL_7_BIT                  (US_MR_SPI_CHRL_7_BIT_Val << US_MR_SPI_CHRL_Pos)     /**< (US_MR) Character length is 7 bits Position  */
#define US_MR_SPI_CHRL_8_BIT                  (US_MR_SPI_CHRL_8_BIT_Val << US_MR_SPI_CHRL_Pos)     /**< (US_MR) Character length is 8 bits Position  */
#define US_MR_SPI_CPHA_Pos                    _U_(8)                                               /**< (US_MR) SPI Clock Phase Position */
#define US_MR_SPI_CPHA_Msk                    (_U_(0x1) << US_MR_SPI_CPHA_Pos)                     /**< (US_MR) SPI Clock Phase Mask */
#define US_MR_SPI_CPHA(value)                 (US_MR_SPI_CPHA_Msk & ((value) << US_MR_SPI_CPHA_Pos))
#define US_MR_SPI_CPOL_Pos                    _U_(16)                                              /**< (US_MR) SPI Clock Polarity Position */
#define US_MR_SPI_CPOL_Msk                    (_U_(0x1) << US_MR_SPI_CPOL_Pos)                     /**< (US_MR) SPI Clock Polarity Mask */
#define US_MR_SPI_CPOL(value)                 (US_MR_SPI_CPOL_Msk & ((value) << US_MR_SPI_CPOL_Pos))
#define US_MR_SPI_CLKO_Pos                    _U_(18)                                              /**< (US_MR) Clock Output Select Position */
#define US_MR_SPI_CLKO_Msk                    (_U_(0x1) << US_MR_SPI_CLKO_Pos)                     /**< (US_MR) Clock Output Select Mask */
#define US_MR_SPI_CLKO(value)                 (US_MR_SPI_CLKO_Msk & ((value) << US_MR_SPI_CLKO_Pos))
#define US_MR_SPI_WRDBT_Pos                   _U_(20)                                              /**< (US_MR) Wait Read Data Before Transfer Position */
#define US_MR_SPI_WRDBT_Msk                   (_U_(0x1) << US_MR_SPI_WRDBT_Pos)                    /**< (US_MR) Wait Read Data Before Transfer Mask */
#define US_MR_SPI_WRDBT(value)                (US_MR_SPI_WRDBT_Msk & ((value) << US_MR_SPI_WRDBT_Pos))
#define US_MR_SPI_Msk                         _U_(0x001501FF)                                       /**< (US_MR_SPI) Register Mask  */


/* -------- US_IER : (USART Offset: 0x08) ( /W 32) Interrupt Enable Register -------- */
#define US_IER_Msk                            _U_(0x00000000)                                      /**< (US_IER) Register Mask  */

/* USART mode */
#define US_IER_USART_RXRDY_Pos                _U_(0)                                               /**< (US_IER) RXRDY Interrupt Enable Position */
#define US_IER_USART_RXRDY_Msk                (_U_(0x1) << US_IER_USART_RXRDY_Pos)                 /**< (US_IER) RXRDY Interrupt Enable Mask */
#define US_IER_USART_RXRDY(value)             (US_IER_USART_RXRDY_Msk & ((value) << US_IER_USART_RXRDY_Pos))
#define US_IER_USART_TXRDY_Pos                _U_(1)                                               /**< (US_IER) TXRDY Interrupt Enable Position */
#define US_IER_USART_TXRDY_Msk                (_U_(0x1) << US_IER_USART_TXRDY_Pos)                 /**< (US_IER) TXRDY Interrupt Enable Mask */
#define US_IER_USART_TXRDY(value)             (US_IER_USART_TXRDY_Msk & ((value) << US_IER_USART_TXRDY_Pos))
#define US_IER_USART_RXBRK_Pos                _U_(2)                                               /**< (US_IER) Receiver Break Interrupt Enable Position */
#define US_IER_USART_RXBRK_Msk                (_U_(0x1) << US_IER_USART_RXBRK_Pos)                 /**< (US_IER) Receiver Break Interrupt Enable Mask */
#define US_IER_USART_RXBRK(value)             (US_IER_USART_RXBRK_Msk & ((value) << US_IER_USART_RXBRK_Pos))
#define US_IER_USART_OVRE_Pos                 _U_(5)                                               /**< (US_IER) Overrun Error Interrupt Enable Position */
#define US_IER_USART_OVRE_Msk                 (_U_(0x1) << US_IER_USART_OVRE_Pos)                  /**< (US_IER) Overrun Error Interrupt Enable Mask */
#define US_IER_USART_OVRE(value)              (US_IER_USART_OVRE_Msk & ((value) << US_IER_USART_OVRE_Pos))
#define US_IER_USART_FRAME_Pos                _U_(6)                                               /**< (US_IER) Framing Error Interrupt Enable Position */
#define US_IER_USART_FRAME_Msk                (_U_(0x1) << US_IER_USART_FRAME_Pos)                 /**< (US_IER) Framing Error Interrupt Enable Mask */
#define US_IER_USART_FRAME(value)             (US_IER_USART_FRAME_Msk & ((value) << US_IER_USART_FRAME_Pos))
#define US_IER_USART_PARE_Pos                 _U_(7)                                               /**< (US_IER) Parity Error Interrupt Enable Position */
#define US_IER_USART_PARE_Msk                 (_U_(0x1) << US_IER_USART_PARE_Pos)                  /**< (US_IER) Parity Error Interrupt Enable Mask */
#define US_IER_USART_PARE(value)              (US_IER_USART_PARE_Msk & ((value) << US_IER_USART_PARE_Pos))
#define US_IER_USART_TIMEOUT_Pos              _U_(8)                                               /**< (US_IER) Timeout Interrupt Enable Position */
#define US_IER_USART_TIMEOUT_Msk              (_U_(0x1) << US_IER_USART_TIMEOUT_Pos)               /**< (US_IER) Timeout Interrupt Enable Mask */
#define US_IER_USART_TIMEOUT(value)           (US_IER_USART_TIMEOUT_Msk & ((value) << US_IER_USART_TIMEOUT_Pos))
#define US_IER_USART_TXEMPTY_Pos              _U_(9)                                               /**< (US_IER) TXEMPTY Interrupt Enable Position */
#define US_IER_USART_TXEMPTY_Msk              (_U_(0x1) << US_IER_USART_TXEMPTY_Pos)               /**< (US_IER) TXEMPTY Interrupt Enable Mask */
#define US_IER_USART_TXEMPTY(value)           (US_IER_USART_TXEMPTY_Msk & ((value) << US_IER_USART_TXEMPTY_Pos))
#define US_IER_USART_ITER_Pos                 _U_(10)                                              /**< (US_IER) Max number of Repetitions Reached Interrupt Enable Position */
#define US_IER_USART_ITER_Msk                 (_U_(0x1) << US_IER_USART_ITER_Pos)                  /**< (US_IER) Max number of Repetitions Reached Interrupt Enable Mask */
#define US_IER_USART_ITER(value)              (US_IER_USART_ITER_Msk & ((value) << US_IER_USART_ITER_Pos))
#define US_IER_USART_NACK_Pos                 _U_(13)                                              /**< (US_IER) Non Acknowledge Interrupt Enable Position */
#define US_IER_USART_NACK_Msk                 (_U_(0x1) << US_IER_USART_NACK_Pos)                  /**< (US_IER) Non Acknowledge Interrupt Enable Mask */
#define US_IER_USART_NACK(value)              (US_IER_USART_NACK_Msk & ((value) << US_IER_USART_NACK_Pos))
#define US_IER_USART_RIIC_Pos                 _U_(16)                                              /**< (US_IER) Ring Indicator Input Change Enable Position */
#define US_IER_USART_RIIC_Msk                 (_U_(0x1) << US_IER_USART_RIIC_Pos)                  /**< (US_IER) Ring Indicator Input Change Enable Mask */
#define US_IER_USART_RIIC(value)              (US_IER_USART_RIIC_Msk & ((value) << US_IER_USART_RIIC_Pos))
#define US_IER_USART_DSRIC_Pos                _U_(17)                                              /**< (US_IER) Data Set Ready Input Change Enable Position */
#define US_IER_USART_DSRIC_Msk                (_U_(0x1) << US_IER_USART_DSRIC_Pos)                 /**< (US_IER) Data Set Ready Input Change Enable Mask */
#define US_IER_USART_DSRIC(value)             (US_IER_USART_DSRIC_Msk & ((value) << US_IER_USART_DSRIC_Pos))
#define US_IER_USART_DCDIC_Pos                _U_(18)                                              /**< (US_IER) Data Carrier Detect Input Change Interrupt Enable Position */
#define US_IER_USART_DCDIC_Msk                (_U_(0x1) << US_IER_USART_DCDIC_Pos)                 /**< (US_IER) Data Carrier Detect Input Change Interrupt Enable Mask */
#define US_IER_USART_DCDIC(value)             (US_IER_USART_DCDIC_Msk & ((value) << US_IER_USART_DCDIC_Pos))
#define US_IER_USART_CTSIC_Pos                _U_(19)                                              /**< (US_IER) Clear to Send Input Change Interrupt Enable Position */
#define US_IER_USART_CTSIC_Msk                (_U_(0x1) << US_IER_USART_CTSIC_Pos)                 /**< (US_IER) Clear to Send Input Change Interrupt Enable Mask */
#define US_IER_USART_CTSIC(value)             (US_IER_USART_CTSIC_Msk & ((value) << US_IER_USART_CTSIC_Pos))
#define US_IER_USART_MANE_Pos                 _U_(24)                                              /**< (US_IER) Manchester Error Interrupt Enable Position */
#define US_IER_USART_MANE_Msk                 (_U_(0x1) << US_IER_USART_MANE_Pos)                  /**< (US_IER) Manchester Error Interrupt Enable Mask */
#define US_IER_USART_MANE(value)              (US_IER_USART_MANE_Msk & ((value) << US_IER_USART_MANE_Pos))
#define US_IER_USART_Msk                      _U_(0x010F27E7)                                       /**< (US_IER_USART) Register Mask  */

/* SPI mode */
#define US_IER_SPI_RXRDY_Pos                  _U_(0)                                               /**< (US_IER) RXRDY Interrupt Enable Position */
#define US_IER_SPI_RXRDY_Msk                  (_U_(0x1) << US_IER_SPI_RXRDY_Pos)                   /**< (US_IER) RXRDY Interrupt Enable Mask */
#define US_IER_SPI_RXRDY(value)               (US_IER_SPI_RXRDY_Msk & ((value) << US_IER_SPI_RXRDY_Pos))
#define US_IER_SPI_TXRDY_Pos                  _U_(1)                                               /**< (US_IER) TXRDY Interrupt Enable Position */
#define US_IER_SPI_TXRDY_Msk                  (_U_(0x1) << US_IER_SPI_TXRDY_Pos)                   /**< (US_IER) TXRDY Interrupt Enable Mask */
#define US_IER_SPI_TXRDY(value)               (US_IER_SPI_TXRDY_Msk & ((value) << US_IER_SPI_TXRDY_Pos))
#define US_IER_SPI_OVRE_Pos                   _U_(5)                                               /**< (US_IER) Overrun Error Interrupt Enable Position */
#define US_IER_SPI_OVRE_Msk                   (_U_(0x1) << US_IER_SPI_OVRE_Pos)                    /**< (US_IER) Overrun Error Interrupt Enable Mask */
#define US_IER_SPI_OVRE(value)                (US_IER_SPI_OVRE_Msk & ((value) << US_IER_SPI_OVRE_Pos))
#define US_IER_SPI_TXEMPTY_Pos                _U_(9)                                               /**< (US_IER) TXEMPTY Interrupt Enable Position */
#define US_IER_SPI_TXEMPTY_Msk                (_U_(0x1) << US_IER_SPI_TXEMPTY_Pos)                 /**< (US_IER) TXEMPTY Interrupt Enable Mask */
#define US_IER_SPI_TXEMPTY(value)             (US_IER_SPI_TXEMPTY_Msk & ((value) << US_IER_SPI_TXEMPTY_Pos))
#define US_IER_SPI_UNRE_Pos                   _U_(10)                                              /**< (US_IER) Underrun Error Interrupt Enable Position */
#define US_IER_SPI_UNRE_Msk                   (_U_(0x1) << US_IER_SPI_UNRE_Pos)                    /**< (US_IER) Underrun Error Interrupt Enable Mask */
#define US_IER_SPI_UNRE(value)                (US_IER_SPI_UNRE_Msk & ((value) << US_IER_SPI_UNRE_Pos))
#define US_IER_SPI_NSSE_Pos                   _U_(19)                                              /**< (US_IER) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Position */
#define US_IER_SPI_NSSE_Msk                   (_U_(0x1) << US_IER_SPI_NSSE_Pos)                    /**< (US_IER) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Mask */
#define US_IER_SPI_NSSE(value)                (US_IER_SPI_NSSE_Msk & ((value) << US_IER_SPI_NSSE_Pos))
#define US_IER_SPI_Msk                        _U_(0x00080623)                                       /**< (US_IER_SPI) Register Mask  */

/* LIN mode */
#define US_IER_LIN_RXRDY_Pos                  _U_(0)                                               /**< (US_IER) RXRDY Interrupt Enable Position */
#define US_IER_LIN_RXRDY_Msk                  (_U_(0x1) << US_IER_LIN_RXRDY_Pos)                   /**< (US_IER) RXRDY Interrupt Enable Mask */
#define US_IER_LIN_RXRDY(value)               (US_IER_LIN_RXRDY_Msk & ((value) << US_IER_LIN_RXRDY_Pos))
#define US_IER_LIN_TXRDY_Pos                  _U_(1)                                               /**< (US_IER) TXRDY Interrupt Enable Position */
#define US_IER_LIN_TXRDY_Msk                  (_U_(0x1) << US_IER_LIN_TXRDY_Pos)                   /**< (US_IER) TXRDY Interrupt Enable Mask */
#define US_IER_LIN_TXRDY(value)               (US_IER_LIN_TXRDY_Msk & ((value) << US_IER_LIN_TXRDY_Pos))
#define US_IER_LIN_OVRE_Pos                   _U_(5)                                               /**< (US_IER) Overrun Error Interrupt Enable Position */
#define US_IER_LIN_OVRE_Msk                   (_U_(0x1) << US_IER_LIN_OVRE_Pos)                    /**< (US_IER) Overrun Error Interrupt Enable Mask */
#define US_IER_LIN_OVRE(value)                (US_IER_LIN_OVRE_Msk & ((value) << US_IER_LIN_OVRE_Pos))
#define US_IER_LIN_FRAME_Pos                  _U_(6)                                               /**< (US_IER) Framing Error Interrupt Enable Position */
#define US_IER_LIN_FRAME_Msk                  (_U_(0x1) << US_IER_LIN_FRAME_Pos)                   /**< (US_IER) Framing Error Interrupt Enable Mask */
#define US_IER_LIN_FRAME(value)               (US_IER_LIN_FRAME_Msk & ((value) << US_IER_LIN_FRAME_Pos))
#define US_IER_LIN_PARE_Pos                   _U_(7)                                               /**< (US_IER) Parity Error Interrupt Enable Position */
#define US_IER_LIN_PARE_Msk                   (_U_(0x1) << US_IER_LIN_PARE_Pos)                    /**< (US_IER) Parity Error Interrupt Enable Mask */
#define US_IER_LIN_PARE(value)                (US_IER_LIN_PARE_Msk & ((value) << US_IER_LIN_PARE_Pos))
#define US_IER_LIN_TIMEOUT_Pos                _U_(8)                                               /**< (US_IER) Timeout Interrupt Enable Position */
#define US_IER_LIN_TIMEOUT_Msk                (_U_(0x1) << US_IER_LIN_TIMEOUT_Pos)                 /**< (US_IER) Timeout Interrupt Enable Mask */
#define US_IER_LIN_TIMEOUT(value)             (US_IER_LIN_TIMEOUT_Msk & ((value) << US_IER_LIN_TIMEOUT_Pos))
#define US_IER_LIN_TXEMPTY_Pos                _U_(9)                                               /**< (US_IER) TXEMPTY Interrupt Enable Position */
#define US_IER_LIN_TXEMPTY_Msk                (_U_(0x1) << US_IER_LIN_TXEMPTY_Pos)                 /**< (US_IER) TXEMPTY Interrupt Enable Mask */
#define US_IER_LIN_TXEMPTY(value)             (US_IER_LIN_TXEMPTY_Msk & ((value) << US_IER_LIN_TXEMPTY_Pos))
#define US_IER_LIN_LINBK_Pos                  _U_(13)                                              /**< (US_IER) LIN Break Sent or LIN Break Received Interrupt Enable Position */
#define US_IER_LIN_LINBK_Msk                  (_U_(0x1) << US_IER_LIN_LINBK_Pos)                   /**< (US_IER) LIN Break Sent or LIN Break Received Interrupt Enable Mask */
#define US_IER_LIN_LINBK(value)               (US_IER_LIN_LINBK_Msk & ((value) << US_IER_LIN_LINBK_Pos))
#define US_IER_LIN_LINID_Pos                  _U_(14)                                              /**< (US_IER) LIN Identifier Sent or LIN Identifier Received Interrupt Enable Position */
#define US_IER_LIN_LINID_Msk                  (_U_(0x1) << US_IER_LIN_LINID_Pos)                   /**< (US_IER) LIN Identifier Sent or LIN Identifier Received Interrupt Enable Mask */
#define US_IER_LIN_LINID(value)               (US_IER_LIN_LINID_Msk & ((value) << US_IER_LIN_LINID_Pos))
#define US_IER_LIN_LINTC_Pos                  _U_(15)                                              /**< (US_IER) LIN Transfer Completed Interrupt Enable Position */
#define US_IER_LIN_LINTC_Msk                  (_U_(0x1) << US_IER_LIN_LINTC_Pos)                   /**< (US_IER) LIN Transfer Completed Interrupt Enable Mask */
#define US_IER_LIN_LINTC(value)               (US_IER_LIN_LINTC_Msk & ((value) << US_IER_LIN_LINTC_Pos))
#define US_IER_LIN_LINBE_Pos                  _U_(25)                                              /**< (US_IER) LIN Bus Error Interrupt Enable Position */
#define US_IER_LIN_LINBE_Msk                  (_U_(0x1) << US_IER_LIN_LINBE_Pos)                   /**< (US_IER) LIN Bus Error Interrupt Enable Mask */
#define US_IER_LIN_LINBE(value)               (US_IER_LIN_LINBE_Msk & ((value) << US_IER_LIN_LINBE_Pos))
#define US_IER_LIN_LINISFE_Pos                _U_(26)                                              /**< (US_IER) LIN Inconsistent Synch Field Error Interrupt Enable Position */
#define US_IER_LIN_LINISFE_Msk                (_U_(0x1) << US_IER_LIN_LINISFE_Pos)                 /**< (US_IER) LIN Inconsistent Synch Field Error Interrupt Enable Mask */
#define US_IER_LIN_LINISFE(value)             (US_IER_LIN_LINISFE_Msk & ((value) << US_IER_LIN_LINISFE_Pos))
#define US_IER_LIN_LINIPE_Pos                 _U_(27)                                              /**< (US_IER) LIN Identifier Parity Interrupt Enable Position */
#define US_IER_LIN_LINIPE_Msk                 (_U_(0x1) << US_IER_LIN_LINIPE_Pos)                  /**< (US_IER) LIN Identifier Parity Interrupt Enable Mask */
#define US_IER_LIN_LINIPE(value)              (US_IER_LIN_LINIPE_Msk & ((value) << US_IER_LIN_LINIPE_Pos))
#define US_IER_LIN_LINCE_Pos                  _U_(28)                                              /**< (US_IER) LIN Checksum Error Interrupt Enable Position */
#define US_IER_LIN_LINCE_Msk                  (_U_(0x1) << US_IER_LIN_LINCE_Pos)                   /**< (US_IER) LIN Checksum Error Interrupt Enable Mask */
#define US_IER_LIN_LINCE(value)               (US_IER_LIN_LINCE_Msk & ((value) << US_IER_LIN_LINCE_Pos))
#define US_IER_LIN_LINSNRE_Pos                _U_(29)                                              /**< (US_IER) LIN Slave Not Responding Error Interrupt Enable Position */
#define US_IER_LIN_LINSNRE_Msk                (_U_(0x1) << US_IER_LIN_LINSNRE_Pos)                 /**< (US_IER) LIN Slave Not Responding Error Interrupt Enable Mask */
#define US_IER_LIN_LINSNRE(value)             (US_IER_LIN_LINSNRE_Msk & ((value) << US_IER_LIN_LINSNRE_Pos))
#define US_IER_LIN_LINSTE_Pos                 _U_(30)                                              /**< (US_IER) LIN Synch Tolerance Error Interrupt Enable Position */
#define US_IER_LIN_LINSTE_Msk                 (_U_(0x1) << US_IER_LIN_LINSTE_Pos)                  /**< (US_IER) LIN Synch Tolerance Error Interrupt Enable Mask */
#define US_IER_LIN_LINSTE(value)              (US_IER_LIN_LINSTE_Msk & ((value) << US_IER_LIN_LINSTE_Pos))
#define US_IER_LIN_LINHTE_Pos                 _U_(31)                                              /**< (US_IER) LIN Header Timeout Error Interrupt Enable Position */
#define US_IER_LIN_LINHTE_Msk                 (_U_(0x1) << US_IER_LIN_LINHTE_Pos)                  /**< (US_IER) LIN Header Timeout Error Interrupt Enable Mask */
#define US_IER_LIN_LINHTE(value)              (US_IER_LIN_LINHTE_Msk & ((value) << US_IER_LIN_LINHTE_Pos))
#define US_IER_LIN_Msk                        _U_(0xFE00E3E3)                                       /**< (US_IER_LIN) Register Mask  */

/* LON mode */
#define US_IER_LON_RXRDY_Pos                  _U_(0)                                               /**< (US_IER) RXRDY Interrupt Enable Position */
#define US_IER_LON_RXRDY_Msk                  (_U_(0x1) << US_IER_LON_RXRDY_Pos)                   /**< (US_IER) RXRDY Interrupt Enable Mask */
#define US_IER_LON_RXRDY(value)               (US_IER_LON_RXRDY_Msk & ((value) << US_IER_LON_RXRDY_Pos))
#define US_IER_LON_TXRDY_Pos                  _U_(1)                                               /**< (US_IER) TXRDY Interrupt Enable Position */
#define US_IER_LON_TXRDY_Msk                  (_U_(0x1) << US_IER_LON_TXRDY_Pos)                   /**< (US_IER) TXRDY Interrupt Enable Mask */
#define US_IER_LON_TXRDY(value)               (US_IER_LON_TXRDY_Msk & ((value) << US_IER_LON_TXRDY_Pos))
#define US_IER_LON_OVRE_Pos                   _U_(5)                                               /**< (US_IER) Overrun Error Interrupt Enable Position */
#define US_IER_LON_OVRE_Msk                   (_U_(0x1) << US_IER_LON_OVRE_Pos)                    /**< (US_IER) Overrun Error Interrupt Enable Mask */
#define US_IER_LON_OVRE(value)                (US_IER_LON_OVRE_Msk & ((value) << US_IER_LON_OVRE_Pos))
#define US_IER_LON_LSFE_Pos                   _U_(6)                                               /**< (US_IER) LON Short Frame Error Interrupt Enable Position */
#define US_IER_LON_LSFE_Msk                   (_U_(0x1) << US_IER_LON_LSFE_Pos)                    /**< (US_IER) LON Short Frame Error Interrupt Enable Mask */
#define US_IER_LON_LSFE(value)                (US_IER_LON_LSFE_Msk & ((value) << US_IER_LON_LSFE_Pos))
#define US_IER_LON_LCRCE_Pos                  _U_(7)                                               /**< (US_IER) LON CRC Error Interrupt Enable Position */
#define US_IER_LON_LCRCE_Msk                  (_U_(0x1) << US_IER_LON_LCRCE_Pos)                   /**< (US_IER) LON CRC Error Interrupt Enable Mask */
#define US_IER_LON_LCRCE(value)               (US_IER_LON_LCRCE_Msk & ((value) << US_IER_LON_LCRCE_Pos))
#define US_IER_LON_TXEMPTY_Pos                _U_(9)                                               /**< (US_IER) TXEMPTY Interrupt Enable Position */
#define US_IER_LON_TXEMPTY_Msk                (_U_(0x1) << US_IER_LON_TXEMPTY_Pos)                 /**< (US_IER) TXEMPTY Interrupt Enable Mask */
#define US_IER_LON_TXEMPTY(value)             (US_IER_LON_TXEMPTY_Msk & ((value) << US_IER_LON_TXEMPTY_Pos))
#define US_IER_LON_UNRE_Pos                   _U_(10)                                              /**< (US_IER) Underrun Error Interrupt Enable Position */
#define US_IER_LON_UNRE_Msk                   (_U_(0x1) << US_IER_LON_UNRE_Pos)                    /**< (US_IER) Underrun Error Interrupt Enable Mask */
#define US_IER_LON_UNRE(value)                (US_IER_LON_UNRE_Msk & ((value) << US_IER_LON_UNRE_Pos))
#define US_IER_LON_LTXD_Pos                   _U_(24)                                              /**< (US_IER) LON Transmission Done Interrupt Enable Position */
#define US_IER_LON_LTXD_Msk                   (_U_(0x1) << US_IER_LON_LTXD_Pos)                    /**< (US_IER) LON Transmission Done Interrupt Enable Mask */
#define US_IER_LON_LTXD(value)                (US_IER_LON_LTXD_Msk & ((value) << US_IER_LON_LTXD_Pos))
#define US_IER_LON_LCOL_Pos                   _U_(25)                                              /**< (US_IER) LON Collision Interrupt Enable Position */
#define US_IER_LON_LCOL_Msk                   (_U_(0x1) << US_IER_LON_LCOL_Pos)                    /**< (US_IER) LON Collision Interrupt Enable Mask */
#define US_IER_LON_LCOL(value)                (US_IER_LON_LCOL_Msk & ((value) << US_IER_LON_LCOL_Pos))
#define US_IER_LON_LFET_Pos                   _U_(26)                                              /**< (US_IER) LON Frame Early Termination Interrupt Enable Position */
#define US_IER_LON_LFET_Msk                   (_U_(0x1) << US_IER_LON_LFET_Pos)                    /**< (US_IER) LON Frame Early Termination Interrupt Enable Mask */
#define US_IER_LON_LFET(value)                (US_IER_LON_LFET_Msk & ((value) << US_IER_LON_LFET_Pos))
#define US_IER_LON_LRXD_Pos                   _U_(27)                                              /**< (US_IER) LON Reception Done Interrupt Enable Position */
#define US_IER_LON_LRXD_Msk                   (_U_(0x1) << US_IER_LON_LRXD_Pos)                    /**< (US_IER) LON Reception Done Interrupt Enable Mask */
#define US_IER_LON_LRXD(value)                (US_IER_LON_LRXD_Msk & ((value) << US_IER_LON_LRXD_Pos))
#define US_IER_LON_LBLOVFE_Pos                _U_(28)                                              /**< (US_IER) LON Backlog Overflow Error Interrupt Enable Position */
#define US_IER_LON_LBLOVFE_Msk                (_U_(0x1) << US_IER_LON_LBLOVFE_Pos)                 /**< (US_IER) LON Backlog Overflow Error Interrupt Enable Mask */
#define US_IER_LON_LBLOVFE(value)             (US_IER_LON_LBLOVFE_Msk & ((value) << US_IER_LON_LBLOVFE_Pos))
#define US_IER_LON_Msk                        _U_(0x1F0006E3)                                       /**< (US_IER_LON) Register Mask  */


/* -------- US_IDR : (USART Offset: 0x0C) ( /W 32) Interrupt Disable Register -------- */
#define US_IDR_Msk                            _U_(0x00000000)                                      /**< (US_IDR) Register Mask  */

/* USART mode */
#define US_IDR_USART_RXRDY_Pos                _U_(0)                                               /**< (US_IDR) RXRDY Interrupt Disable Position */
#define US_IDR_USART_RXRDY_Msk                (_U_(0x1) << US_IDR_USART_RXRDY_Pos)                 /**< (US_IDR) RXRDY Interrupt Disable Mask */
#define US_IDR_USART_RXRDY(value)             (US_IDR_USART_RXRDY_Msk & ((value) << US_IDR_USART_RXRDY_Pos))
#define US_IDR_USART_TXRDY_Pos                _U_(1)                                               /**< (US_IDR) TXRDY Interrupt Disable Position */
#define US_IDR_USART_TXRDY_Msk                (_U_(0x1) << US_IDR_USART_TXRDY_Pos)                 /**< (US_IDR) TXRDY Interrupt Disable Mask */
#define US_IDR_USART_TXRDY(value)             (US_IDR_USART_TXRDY_Msk & ((value) << US_IDR_USART_TXRDY_Pos))
#define US_IDR_USART_RXBRK_Pos                _U_(2)                                               /**< (US_IDR) Receiver Break Interrupt Disable Position */
#define US_IDR_USART_RXBRK_Msk                (_U_(0x1) << US_IDR_USART_RXBRK_Pos)                 /**< (US_IDR) Receiver Break Interrupt Disable Mask */
#define US_IDR_USART_RXBRK(value)             (US_IDR_USART_RXBRK_Msk & ((value) << US_IDR_USART_RXBRK_Pos))
#define US_IDR_USART_OVRE_Pos                 _U_(5)                                               /**< (US_IDR) Overrun Error Interrupt Enable Position */
#define US_IDR_USART_OVRE_Msk                 (_U_(0x1) << US_IDR_USART_OVRE_Pos)                  /**< (US_IDR) Overrun Error Interrupt Enable Mask */
#define US_IDR_USART_OVRE(value)              (US_IDR_USART_OVRE_Msk & ((value) << US_IDR_USART_OVRE_Pos))
#define US_IDR_USART_FRAME_Pos                _U_(6)                                               /**< (US_IDR) Framing Error Interrupt Disable Position */
#define US_IDR_USART_FRAME_Msk                (_U_(0x1) << US_IDR_USART_FRAME_Pos)                 /**< (US_IDR) Framing Error Interrupt Disable Mask */
#define US_IDR_USART_FRAME(value)             (US_IDR_USART_FRAME_Msk & ((value) << US_IDR_USART_FRAME_Pos))
#define US_IDR_USART_PARE_Pos                 _U_(7)                                               /**< (US_IDR) Parity Error Interrupt Disable Position */
#define US_IDR_USART_PARE_Msk                 (_U_(0x1) << US_IDR_USART_PARE_Pos)                  /**< (US_IDR) Parity Error Interrupt Disable Mask */
#define US_IDR_USART_PARE(value)              (US_IDR_USART_PARE_Msk & ((value) << US_IDR_USART_PARE_Pos))
#define US_IDR_USART_TIMEOUT_Pos              _U_(8)                                               /**< (US_IDR) Timeout Interrupt Disable Position */
#define US_IDR_USART_TIMEOUT_Msk              (_U_(0x1) << US_IDR_USART_TIMEOUT_Pos)               /**< (US_IDR) Timeout Interrupt Disable Mask */
#define US_IDR_USART_TIMEOUT(value)           (US_IDR_USART_TIMEOUT_Msk & ((value) << US_IDR_USART_TIMEOUT_Pos))
#define US_IDR_USART_TXEMPTY_Pos              _U_(9)                                               /**< (US_IDR) TXEMPTY Interrupt Disable Position */
#define US_IDR_USART_TXEMPTY_Msk              (_U_(0x1) << US_IDR_USART_TXEMPTY_Pos)               /**< (US_IDR) TXEMPTY Interrupt Disable Mask */
#define US_IDR_USART_TXEMPTY(value)           (US_IDR_USART_TXEMPTY_Msk & ((value) << US_IDR_USART_TXEMPTY_Pos))
#define US_IDR_USART_ITER_Pos                 _U_(10)                                              /**< (US_IDR) Max Number of Repetitions Reached Interrupt Disable Position */
#define US_IDR_USART_ITER_Msk                 (_U_(0x1) << US_IDR_USART_ITER_Pos)                  /**< (US_IDR) Max Number of Repetitions Reached Interrupt Disable Mask */
#define US_IDR_USART_ITER(value)              (US_IDR_USART_ITER_Msk & ((value) << US_IDR_USART_ITER_Pos))
#define US_IDR_USART_NACK_Pos                 _U_(13)                                              /**< (US_IDR) Non Acknowledge Interrupt Disable Position */
#define US_IDR_USART_NACK_Msk                 (_U_(0x1) << US_IDR_USART_NACK_Pos)                  /**< (US_IDR) Non Acknowledge Interrupt Disable Mask */
#define US_IDR_USART_NACK(value)              (US_IDR_USART_NACK_Msk & ((value) << US_IDR_USART_NACK_Pos))
#define US_IDR_USART_RIIC_Pos                 _U_(16)                                              /**< (US_IDR) Ring Indicator Input Change Disable Position */
#define US_IDR_USART_RIIC_Msk                 (_U_(0x1) << US_IDR_USART_RIIC_Pos)                  /**< (US_IDR) Ring Indicator Input Change Disable Mask */
#define US_IDR_USART_RIIC(value)              (US_IDR_USART_RIIC_Msk & ((value) << US_IDR_USART_RIIC_Pos))
#define US_IDR_USART_DSRIC_Pos                _U_(17)                                              /**< (US_IDR) Data Set Ready Input Change Disable Position */
#define US_IDR_USART_DSRIC_Msk                (_U_(0x1) << US_IDR_USART_DSRIC_Pos)                 /**< (US_IDR) Data Set Ready Input Change Disable Mask */
#define US_IDR_USART_DSRIC(value)             (US_IDR_USART_DSRIC_Msk & ((value) << US_IDR_USART_DSRIC_Pos))
#define US_IDR_USART_DCDIC_Pos                _U_(18)                                              /**< (US_IDR) Data Carrier Detect Input Change Interrupt Disable Position */
#define US_IDR_USART_DCDIC_Msk                (_U_(0x1) << US_IDR_USART_DCDIC_Pos)                 /**< (US_IDR) Data Carrier Detect Input Change Interrupt Disable Mask */
#define US_IDR_USART_DCDIC(value)             (US_IDR_USART_DCDIC_Msk & ((value) << US_IDR_USART_DCDIC_Pos))
#define US_IDR_USART_CTSIC_Pos                _U_(19)                                              /**< (US_IDR) Clear to Send Input Change Interrupt Disable Position */
#define US_IDR_USART_CTSIC_Msk                (_U_(0x1) << US_IDR_USART_CTSIC_Pos)                 /**< (US_IDR) Clear to Send Input Change Interrupt Disable Mask */
#define US_IDR_USART_CTSIC(value)             (US_IDR_USART_CTSIC_Msk & ((value) << US_IDR_USART_CTSIC_Pos))
#define US_IDR_USART_MANE_Pos                 _U_(24)                                              /**< (US_IDR) Manchester Error Interrupt Disable Position */
#define US_IDR_USART_MANE_Msk                 (_U_(0x1) << US_IDR_USART_MANE_Pos)                  /**< (US_IDR) Manchester Error Interrupt Disable Mask */
#define US_IDR_USART_MANE(value)              (US_IDR_USART_MANE_Msk & ((value) << US_IDR_USART_MANE_Pos))
#define US_IDR_USART_Msk                      _U_(0x010F27E7)                                       /**< (US_IDR_USART) Register Mask  */

/* SPI mode */
#define US_IDR_SPI_RXRDY_Pos                  _U_(0)                                               /**< (US_IDR) RXRDY Interrupt Disable Position */
#define US_IDR_SPI_RXRDY_Msk                  (_U_(0x1) << US_IDR_SPI_RXRDY_Pos)                   /**< (US_IDR) RXRDY Interrupt Disable Mask */
#define US_IDR_SPI_RXRDY(value)               (US_IDR_SPI_RXRDY_Msk & ((value) << US_IDR_SPI_RXRDY_Pos))
#define US_IDR_SPI_TXRDY_Pos                  _U_(1)                                               /**< (US_IDR) TXRDY Interrupt Disable Position */
#define US_IDR_SPI_TXRDY_Msk                  (_U_(0x1) << US_IDR_SPI_TXRDY_Pos)                   /**< (US_IDR) TXRDY Interrupt Disable Mask */
#define US_IDR_SPI_TXRDY(value)               (US_IDR_SPI_TXRDY_Msk & ((value) << US_IDR_SPI_TXRDY_Pos))
#define US_IDR_SPI_OVRE_Pos                   _U_(5)                                               /**< (US_IDR) Overrun Error Interrupt Enable Position */
#define US_IDR_SPI_OVRE_Msk                   (_U_(0x1) << US_IDR_SPI_OVRE_Pos)                    /**< (US_IDR) Overrun Error Interrupt Enable Mask */
#define US_IDR_SPI_OVRE(value)                (US_IDR_SPI_OVRE_Msk & ((value) << US_IDR_SPI_OVRE_Pos))
#define US_IDR_SPI_TXEMPTY_Pos                _U_(9)                                               /**< (US_IDR) TXEMPTY Interrupt Disable Position */
#define US_IDR_SPI_TXEMPTY_Msk                (_U_(0x1) << US_IDR_SPI_TXEMPTY_Pos)                 /**< (US_IDR) TXEMPTY Interrupt Disable Mask */
#define US_IDR_SPI_TXEMPTY(value)             (US_IDR_SPI_TXEMPTY_Msk & ((value) << US_IDR_SPI_TXEMPTY_Pos))
#define US_IDR_SPI_UNRE_Pos                   _U_(10)                                              /**< (US_IDR) SPI Underrun Error Interrupt Disable Position */
#define US_IDR_SPI_UNRE_Msk                   (_U_(0x1) << US_IDR_SPI_UNRE_Pos)                    /**< (US_IDR) SPI Underrun Error Interrupt Disable Mask */
#define US_IDR_SPI_UNRE(value)                (US_IDR_SPI_UNRE_Msk & ((value) << US_IDR_SPI_UNRE_Pos))
#define US_IDR_SPI_NSSE_Pos                   _U_(19)                                              /**< (US_IDR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Position */
#define US_IDR_SPI_NSSE_Msk                   (_U_(0x1) << US_IDR_SPI_NSSE_Pos)                    /**< (US_IDR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Mask */
#define US_IDR_SPI_NSSE(value)                (US_IDR_SPI_NSSE_Msk & ((value) << US_IDR_SPI_NSSE_Pos))
#define US_IDR_SPI_Msk                        _U_(0x00080623)                                       /**< (US_IDR_SPI) Register Mask  */

/* LIN mode */
#define US_IDR_LIN_RXRDY_Pos                  _U_(0)                                               /**< (US_IDR) RXRDY Interrupt Disable Position */
#define US_IDR_LIN_RXRDY_Msk                  (_U_(0x1) << US_IDR_LIN_RXRDY_Pos)                   /**< (US_IDR) RXRDY Interrupt Disable Mask */
#define US_IDR_LIN_RXRDY(value)               (US_IDR_LIN_RXRDY_Msk & ((value) << US_IDR_LIN_RXRDY_Pos))
#define US_IDR_LIN_TXRDY_Pos                  _U_(1)                                               /**< (US_IDR) TXRDY Interrupt Disable Position */
#define US_IDR_LIN_TXRDY_Msk                  (_U_(0x1) << US_IDR_LIN_TXRDY_Pos)                   /**< (US_IDR) TXRDY Interrupt Disable Mask */
#define US_IDR_LIN_TXRDY(value)               (US_IDR_LIN_TXRDY_Msk & ((value) << US_IDR_LIN_TXRDY_Pos))
#define US_IDR_LIN_OVRE_Pos                   _U_(5)                                               /**< (US_IDR) Overrun Error Interrupt Enable Position */
#define US_IDR_LIN_OVRE_Msk                   (_U_(0x1) << US_IDR_LIN_OVRE_Pos)                    /**< (US_IDR) Overrun Error Interrupt Enable Mask */
#define US_IDR_LIN_OVRE(value)                (US_IDR_LIN_OVRE_Msk & ((value) << US_IDR_LIN_OVRE_Pos))
#define US_IDR_LIN_FRAME_Pos                  _U_(6)                                               /**< (US_IDR) Framing Error Interrupt Disable Position */
#define US_IDR_LIN_FRAME_Msk                  (_U_(0x1) << US_IDR_LIN_FRAME_Pos)                   /**< (US_IDR) Framing Error Interrupt Disable Mask */
#define US_IDR_LIN_FRAME(value)               (US_IDR_LIN_FRAME_Msk & ((value) << US_IDR_LIN_FRAME_Pos))
#define US_IDR_LIN_PARE_Pos                   _U_(7)                                               /**< (US_IDR) Parity Error Interrupt Disable Position */
#define US_IDR_LIN_PARE_Msk                   (_U_(0x1) << US_IDR_LIN_PARE_Pos)                    /**< (US_IDR) Parity Error Interrupt Disable Mask */
#define US_IDR_LIN_PARE(value)                (US_IDR_LIN_PARE_Msk & ((value) << US_IDR_LIN_PARE_Pos))
#define US_IDR_LIN_TIMEOUT_Pos                _U_(8)                                               /**< (US_IDR) Timeout Interrupt Disable Position */
#define US_IDR_LIN_TIMEOUT_Msk                (_U_(0x1) << US_IDR_LIN_TIMEOUT_Pos)                 /**< (US_IDR) Timeout Interrupt Disable Mask */
#define US_IDR_LIN_TIMEOUT(value)             (US_IDR_LIN_TIMEOUT_Msk & ((value) << US_IDR_LIN_TIMEOUT_Pos))
#define US_IDR_LIN_TXEMPTY_Pos                _U_(9)                                               /**< (US_IDR) TXEMPTY Interrupt Disable Position */
#define US_IDR_LIN_TXEMPTY_Msk                (_U_(0x1) << US_IDR_LIN_TXEMPTY_Pos)                 /**< (US_IDR) TXEMPTY Interrupt Disable Mask */
#define US_IDR_LIN_TXEMPTY(value)             (US_IDR_LIN_TXEMPTY_Msk & ((value) << US_IDR_LIN_TXEMPTY_Pos))
#define US_IDR_LIN_LINBK_Pos                  _U_(13)                                              /**< (US_IDR) LIN Break Sent or LIN Break Received Interrupt Disable Position */
#define US_IDR_LIN_LINBK_Msk                  (_U_(0x1) << US_IDR_LIN_LINBK_Pos)                   /**< (US_IDR) LIN Break Sent or LIN Break Received Interrupt Disable Mask */
#define US_IDR_LIN_LINBK(value)               (US_IDR_LIN_LINBK_Msk & ((value) << US_IDR_LIN_LINBK_Pos))
#define US_IDR_LIN_LINID_Pos                  _U_(14)                                              /**< (US_IDR) LIN Identifier Sent or LIN Identifier Received Interrupt Disable Position */
#define US_IDR_LIN_LINID_Msk                  (_U_(0x1) << US_IDR_LIN_LINID_Pos)                   /**< (US_IDR) LIN Identifier Sent or LIN Identifier Received Interrupt Disable Mask */
#define US_IDR_LIN_LINID(value)               (US_IDR_LIN_LINID_Msk & ((value) << US_IDR_LIN_LINID_Pos))
#define US_IDR_LIN_LINTC_Pos                  _U_(15)                                              /**< (US_IDR) LIN Transfer Completed Interrupt Disable Position */
#define US_IDR_LIN_LINTC_Msk                  (_U_(0x1) << US_IDR_LIN_LINTC_Pos)                   /**< (US_IDR) LIN Transfer Completed Interrupt Disable Mask */
#define US_IDR_LIN_LINTC(value)               (US_IDR_LIN_LINTC_Msk & ((value) << US_IDR_LIN_LINTC_Pos))
#define US_IDR_LIN_LINBE_Pos                  _U_(25)                                              /**< (US_IDR) LIN Bus Error Interrupt Disable Position */
#define US_IDR_LIN_LINBE_Msk                  (_U_(0x1) << US_IDR_LIN_LINBE_Pos)                   /**< (US_IDR) LIN Bus Error Interrupt Disable Mask */
#define US_IDR_LIN_LINBE(value)               (US_IDR_LIN_LINBE_Msk & ((value) << US_IDR_LIN_LINBE_Pos))
#define US_IDR_LIN_LINISFE_Pos                _U_(26)                                              /**< (US_IDR) LIN Inconsistent Synch Field Error Interrupt Disable Position */
#define US_IDR_LIN_LINISFE_Msk                (_U_(0x1) << US_IDR_LIN_LINISFE_Pos)                 /**< (US_IDR) LIN Inconsistent Synch Field Error Interrupt Disable Mask */
#define US_IDR_LIN_LINISFE(value)             (US_IDR_LIN_LINISFE_Msk & ((value) << US_IDR_LIN_LINISFE_Pos))
#define US_IDR_LIN_LINIPE_Pos                 _U_(27)                                              /**< (US_IDR) LIN Identifier Parity Interrupt Disable Position */
#define US_IDR_LIN_LINIPE_Msk                 (_U_(0x1) << US_IDR_LIN_LINIPE_Pos)                  /**< (US_IDR) LIN Identifier Parity Interrupt Disable Mask */
#define US_IDR_LIN_LINIPE(value)              (US_IDR_LIN_LINIPE_Msk & ((value) << US_IDR_LIN_LINIPE_Pos))
#define US_IDR_LIN_LINCE_Pos                  _U_(28)                                              /**< (US_IDR) LIN Checksum Error Interrupt Disable Position */
#define US_IDR_LIN_LINCE_Msk                  (_U_(0x1) << US_IDR_LIN_LINCE_Pos)                   /**< (US_IDR) LIN Checksum Error Interrupt Disable Mask */
#define US_IDR_LIN_LINCE(value)               (US_IDR_LIN_LINCE_Msk & ((value) << US_IDR_LIN_LINCE_Pos))
#define US_IDR_LIN_LINSNRE_Pos                _U_(29)                                              /**< (US_IDR) LIN Slave Not Responding Error Interrupt Disable Position */
#define US_IDR_LIN_LINSNRE_Msk                (_U_(0x1) << US_IDR_LIN_LINSNRE_Pos)                 /**< (US_IDR) LIN Slave Not Responding Error Interrupt Disable Mask */
#define US_IDR_LIN_LINSNRE(value)             (US_IDR_LIN_LINSNRE_Msk & ((value) << US_IDR_LIN_LINSNRE_Pos))
#define US_IDR_LIN_LINSTE_Pos                 _U_(30)                                              /**< (US_IDR) LIN Synch Tolerance Error Interrupt Disable Position */
#define US_IDR_LIN_LINSTE_Msk                 (_U_(0x1) << US_IDR_LIN_LINSTE_Pos)                  /**< (US_IDR) LIN Synch Tolerance Error Interrupt Disable Mask */
#define US_IDR_LIN_LINSTE(value)              (US_IDR_LIN_LINSTE_Msk & ((value) << US_IDR_LIN_LINSTE_Pos))
#define US_IDR_LIN_LINHTE_Pos                 _U_(31)                                              /**< (US_IDR) LIN Header Timeout Error Interrupt Disable Position */
#define US_IDR_LIN_LINHTE_Msk                 (_U_(0x1) << US_IDR_LIN_LINHTE_Pos)                  /**< (US_IDR) LIN Header Timeout Error Interrupt Disable Mask */
#define US_IDR_LIN_LINHTE(value)              (US_IDR_LIN_LINHTE_Msk & ((value) << US_IDR_LIN_LINHTE_Pos))
#define US_IDR_LIN_Msk                        _U_(0xFE00E3E3)                                       /**< (US_IDR_LIN) Register Mask  */

/* LON mode */
#define US_IDR_LON_RXRDY_Pos                  _U_(0)                                               /**< (US_IDR) RXRDY Interrupt Disable Position */
#define US_IDR_LON_RXRDY_Msk                  (_U_(0x1) << US_IDR_LON_RXRDY_Pos)                   /**< (US_IDR) RXRDY Interrupt Disable Mask */
#define US_IDR_LON_RXRDY(value)               (US_IDR_LON_RXRDY_Msk & ((value) << US_IDR_LON_RXRDY_Pos))
#define US_IDR_LON_TXRDY_Pos                  _U_(1)                                               /**< (US_IDR) TXRDY Interrupt Disable Position */
#define US_IDR_LON_TXRDY_Msk                  (_U_(0x1) << US_IDR_LON_TXRDY_Pos)                   /**< (US_IDR) TXRDY Interrupt Disable Mask */
#define US_IDR_LON_TXRDY(value)               (US_IDR_LON_TXRDY_Msk & ((value) << US_IDR_LON_TXRDY_Pos))
#define US_IDR_LON_OVRE_Pos                   _U_(5)                                               /**< (US_IDR) Overrun Error Interrupt Enable Position */
#define US_IDR_LON_OVRE_Msk                   (_U_(0x1) << US_IDR_LON_OVRE_Pos)                    /**< (US_IDR) Overrun Error Interrupt Enable Mask */
#define US_IDR_LON_OVRE(value)                (US_IDR_LON_OVRE_Msk & ((value) << US_IDR_LON_OVRE_Pos))
#define US_IDR_LON_LSFE_Pos                   _U_(6)                                               /**< (US_IDR) LON Short Frame Error Interrupt Disable Position */
#define US_IDR_LON_LSFE_Msk                   (_U_(0x1) << US_IDR_LON_LSFE_Pos)                    /**< (US_IDR) LON Short Frame Error Interrupt Disable Mask */
#define US_IDR_LON_LSFE(value)                (US_IDR_LON_LSFE_Msk & ((value) << US_IDR_LON_LSFE_Pos))
#define US_IDR_LON_LCRCE_Pos                  _U_(7)                                               /**< (US_IDR) LON CRC Error Interrupt Disable Position */
#define US_IDR_LON_LCRCE_Msk                  (_U_(0x1) << US_IDR_LON_LCRCE_Pos)                   /**< (US_IDR) LON CRC Error Interrupt Disable Mask */
#define US_IDR_LON_LCRCE(value)               (US_IDR_LON_LCRCE_Msk & ((value) << US_IDR_LON_LCRCE_Pos))
#define US_IDR_LON_TXEMPTY_Pos                _U_(9)                                               /**< (US_IDR) TXEMPTY Interrupt Disable Position */
#define US_IDR_LON_TXEMPTY_Msk                (_U_(0x1) << US_IDR_LON_TXEMPTY_Pos)                 /**< (US_IDR) TXEMPTY Interrupt Disable Mask */
#define US_IDR_LON_TXEMPTY(value)             (US_IDR_LON_TXEMPTY_Msk & ((value) << US_IDR_LON_TXEMPTY_Pos))
#define US_IDR_LON_UNRE_Pos                   _U_(10)                                              /**< (US_IDR) SPI Underrun Error Interrupt Disable Position */
#define US_IDR_LON_UNRE_Msk                   (_U_(0x1) << US_IDR_LON_UNRE_Pos)                    /**< (US_IDR) SPI Underrun Error Interrupt Disable Mask */
#define US_IDR_LON_UNRE(value)                (US_IDR_LON_UNRE_Msk & ((value) << US_IDR_LON_UNRE_Pos))
#define US_IDR_LON_LTXD_Pos                   _U_(24)                                              /**< (US_IDR) LON Transmission Done Interrupt Disable Position */
#define US_IDR_LON_LTXD_Msk                   (_U_(0x1) << US_IDR_LON_LTXD_Pos)                    /**< (US_IDR) LON Transmission Done Interrupt Disable Mask */
#define US_IDR_LON_LTXD(value)                (US_IDR_LON_LTXD_Msk & ((value) << US_IDR_LON_LTXD_Pos))
#define US_IDR_LON_LCOL_Pos                   _U_(25)                                              /**< (US_IDR) LON Collision Interrupt Disable Position */
#define US_IDR_LON_LCOL_Msk                   (_U_(0x1) << US_IDR_LON_LCOL_Pos)                    /**< (US_IDR) LON Collision Interrupt Disable Mask */
#define US_IDR_LON_LCOL(value)                (US_IDR_LON_LCOL_Msk & ((value) << US_IDR_LON_LCOL_Pos))
#define US_IDR_LON_LFET_Pos                   _U_(26)                                              /**< (US_IDR) LON Frame Early Termination Interrupt Disable Position */
#define US_IDR_LON_LFET_Msk                   (_U_(0x1) << US_IDR_LON_LFET_Pos)                    /**< (US_IDR) LON Frame Early Termination Interrupt Disable Mask */
#define US_IDR_LON_LFET(value)                (US_IDR_LON_LFET_Msk & ((value) << US_IDR_LON_LFET_Pos))
#define US_IDR_LON_LRXD_Pos                   _U_(27)                                              /**< (US_IDR) LON Reception Done Interrupt Disable Position */
#define US_IDR_LON_LRXD_Msk                   (_U_(0x1) << US_IDR_LON_LRXD_Pos)                    /**< (US_IDR) LON Reception Done Interrupt Disable Mask */
#define US_IDR_LON_LRXD(value)                (US_IDR_LON_LRXD_Msk & ((value) << US_IDR_LON_LRXD_Pos))
#define US_IDR_LON_LBLOVFE_Pos                _U_(28)                                              /**< (US_IDR) LON Backlog Overflow Error Interrupt Disable Position */
#define US_IDR_LON_LBLOVFE_Msk                (_U_(0x1) << US_IDR_LON_LBLOVFE_Pos)                 /**< (US_IDR) LON Backlog Overflow Error Interrupt Disable Mask */
#define US_IDR_LON_LBLOVFE(value)             (US_IDR_LON_LBLOVFE_Msk & ((value) << US_IDR_LON_LBLOVFE_Pos))
#define US_IDR_LON_Msk                        _U_(0x1F0006E3)                                       /**< (US_IDR_LON) Register Mask  */


/* -------- US_IMR : (USART Offset: 0x10) ( R/ 32) Interrupt Mask Register -------- */
#define US_IMR_Msk                            _U_(0x00000000)                                      /**< (US_IMR) Register Mask  */

/* USART mode */
#define US_IMR_USART_RXRDY_Pos                _U_(0)                                               /**< (US_IMR) RXRDY Interrupt Mask Position */
#define US_IMR_USART_RXRDY_Msk                (_U_(0x1) << US_IMR_USART_RXRDY_Pos)                 /**< (US_IMR) RXRDY Interrupt Mask Mask */
#define US_IMR_USART_RXRDY(value)             (US_IMR_USART_RXRDY_Msk & ((value) << US_IMR_USART_RXRDY_Pos))
#define US_IMR_USART_TXRDY_Pos                _U_(1)                                               /**< (US_IMR) TXRDY Interrupt Mask Position */
#define US_IMR_USART_TXRDY_Msk                (_U_(0x1) << US_IMR_USART_TXRDY_Pos)                 /**< (US_IMR) TXRDY Interrupt Mask Mask */
#define US_IMR_USART_TXRDY(value)             (US_IMR_USART_TXRDY_Msk & ((value) << US_IMR_USART_TXRDY_Pos))
#define US_IMR_USART_RXBRK_Pos                _U_(2)                                               /**< (US_IMR) Receiver Break Interrupt Mask Position */
#define US_IMR_USART_RXBRK_Msk                (_U_(0x1) << US_IMR_USART_RXBRK_Pos)                 /**< (US_IMR) Receiver Break Interrupt Mask Mask */
#define US_IMR_USART_RXBRK(value)             (US_IMR_USART_RXBRK_Msk & ((value) << US_IMR_USART_RXBRK_Pos))
#define US_IMR_USART_OVRE_Pos                 _U_(5)                                               /**< (US_IMR) Overrun Error Interrupt Mask Position */
#define US_IMR_USART_OVRE_Msk                 (_U_(0x1) << US_IMR_USART_OVRE_Pos)                  /**< (US_IMR) Overrun Error Interrupt Mask Mask */
#define US_IMR_USART_OVRE(value)              (US_IMR_USART_OVRE_Msk & ((value) << US_IMR_USART_OVRE_Pos))
#define US_IMR_USART_FRAME_Pos                _U_(6)                                               /**< (US_IMR) Framing Error Interrupt Mask Position */
#define US_IMR_USART_FRAME_Msk                (_U_(0x1) << US_IMR_USART_FRAME_Pos)                 /**< (US_IMR) Framing Error Interrupt Mask Mask */
#define US_IMR_USART_FRAME(value)             (US_IMR_USART_FRAME_Msk & ((value) << US_IMR_USART_FRAME_Pos))
#define US_IMR_USART_PARE_Pos                 _U_(7)                                               /**< (US_IMR) Parity Error Interrupt Mask Position */
#define US_IMR_USART_PARE_Msk                 (_U_(0x1) << US_IMR_USART_PARE_Pos)                  /**< (US_IMR) Parity Error Interrupt Mask Mask */
#define US_IMR_USART_PARE(value)              (US_IMR_USART_PARE_Msk & ((value) << US_IMR_USART_PARE_Pos))
#define US_IMR_USART_TIMEOUT_Pos              _U_(8)                                               /**< (US_IMR) Timeout Interrupt Mask Position */
#define US_IMR_USART_TIMEOUT_Msk              (_U_(0x1) << US_IMR_USART_TIMEOUT_Pos)               /**< (US_IMR) Timeout Interrupt Mask Mask */
#define US_IMR_USART_TIMEOUT(value)           (US_IMR_USART_TIMEOUT_Msk & ((value) << US_IMR_USART_TIMEOUT_Pos))
#define US_IMR_USART_TXEMPTY_Pos              _U_(9)                                               /**< (US_IMR) TXEMPTY Interrupt Mask Position */
#define US_IMR_USART_TXEMPTY_Msk              (_U_(0x1) << US_IMR_USART_TXEMPTY_Pos)               /**< (US_IMR) TXEMPTY Interrupt Mask Mask */
#define US_IMR_USART_TXEMPTY(value)           (US_IMR_USART_TXEMPTY_Msk & ((value) << US_IMR_USART_TXEMPTY_Pos))
#define US_IMR_USART_ITER_Pos                 _U_(10)                                              /**< (US_IMR) Max Number of Repetitions Reached Interrupt Mask Position */
#define US_IMR_USART_ITER_Msk                 (_U_(0x1) << US_IMR_USART_ITER_Pos)                  /**< (US_IMR) Max Number of Repetitions Reached Interrupt Mask Mask */
#define US_IMR_USART_ITER(value)              (US_IMR_USART_ITER_Msk & ((value) << US_IMR_USART_ITER_Pos))
#define US_IMR_USART_NACK_Pos                 _U_(13)                                              /**< (US_IMR) Non Acknowledge Interrupt Mask Position */
#define US_IMR_USART_NACK_Msk                 (_U_(0x1) << US_IMR_USART_NACK_Pos)                  /**< (US_IMR) Non Acknowledge Interrupt Mask Mask */
#define US_IMR_USART_NACK(value)              (US_IMR_USART_NACK_Msk & ((value) << US_IMR_USART_NACK_Pos))
#define US_IMR_USART_RIIC_Pos                 _U_(16)                                              /**< (US_IMR) Ring Indicator Input Change Mask Position */
#define US_IMR_USART_RIIC_Msk                 (_U_(0x1) << US_IMR_USART_RIIC_Pos)                  /**< (US_IMR) Ring Indicator Input Change Mask Mask */
#define US_IMR_USART_RIIC(value)              (US_IMR_USART_RIIC_Msk & ((value) << US_IMR_USART_RIIC_Pos))
#define US_IMR_USART_DSRIC_Pos                _U_(17)                                              /**< (US_IMR) Data Set Ready Input Change Mask Position */
#define US_IMR_USART_DSRIC_Msk                (_U_(0x1) << US_IMR_USART_DSRIC_Pos)                 /**< (US_IMR) Data Set Ready Input Change Mask Mask */
#define US_IMR_USART_DSRIC(value)             (US_IMR_USART_DSRIC_Msk & ((value) << US_IMR_USART_DSRIC_Pos))
#define US_IMR_USART_DCDIC_Pos                _U_(18)                                              /**< (US_IMR) Data Carrier Detect Input Change Interrupt Mask Position */
#define US_IMR_USART_DCDIC_Msk                (_U_(0x1) << US_IMR_USART_DCDIC_Pos)                 /**< (US_IMR) Data Carrier Detect Input Change Interrupt Mask Mask */
#define US_IMR_USART_DCDIC(value)             (US_IMR_USART_DCDIC_Msk & ((value) << US_IMR_USART_DCDIC_Pos))
#define US_IMR_USART_CTSIC_Pos                _U_(19)                                              /**< (US_IMR) Clear to Send Input Change Interrupt Mask Position */
#define US_IMR_USART_CTSIC_Msk                (_U_(0x1) << US_IMR_USART_CTSIC_Pos)                 /**< (US_IMR) Clear to Send Input Change Interrupt Mask Mask */
#define US_IMR_USART_CTSIC(value)             (US_IMR_USART_CTSIC_Msk & ((value) << US_IMR_USART_CTSIC_Pos))
#define US_IMR_USART_MANE_Pos                 _U_(24)                                              /**< (US_IMR) Manchester Error Interrupt Mask Position */
#define US_IMR_USART_MANE_Msk                 (_U_(0x1) << US_IMR_USART_MANE_Pos)                  /**< (US_IMR) Manchester Error Interrupt Mask Mask */
#define US_IMR_USART_MANE(value)              (US_IMR_USART_MANE_Msk & ((value) << US_IMR_USART_MANE_Pos))
#define US_IMR_USART_Msk                      _U_(0x010F27E7)                                       /**< (US_IMR_USART) Register Mask  */

/* SPI mode */
#define US_IMR_SPI_RXRDY_Pos                  _U_(0)                                               /**< (US_IMR) RXRDY Interrupt Mask Position */
#define US_IMR_SPI_RXRDY_Msk                  (_U_(0x1) << US_IMR_SPI_RXRDY_Pos)                   /**< (US_IMR) RXRDY Interrupt Mask Mask */
#define US_IMR_SPI_RXRDY(value)               (US_IMR_SPI_RXRDY_Msk & ((value) << US_IMR_SPI_RXRDY_Pos))
#define US_IMR_SPI_TXRDY_Pos                  _U_(1)                                               /**< (US_IMR) TXRDY Interrupt Mask Position */
#define US_IMR_SPI_TXRDY_Msk                  (_U_(0x1) << US_IMR_SPI_TXRDY_Pos)                   /**< (US_IMR) TXRDY Interrupt Mask Mask */
#define US_IMR_SPI_TXRDY(value)               (US_IMR_SPI_TXRDY_Msk & ((value) << US_IMR_SPI_TXRDY_Pos))
#define US_IMR_SPI_OVRE_Pos                   _U_(5)                                               /**< (US_IMR) Overrun Error Interrupt Mask Position */
#define US_IMR_SPI_OVRE_Msk                   (_U_(0x1) << US_IMR_SPI_OVRE_Pos)                    /**< (US_IMR) Overrun Error Interrupt Mask Mask */
#define US_IMR_SPI_OVRE(value)                (US_IMR_SPI_OVRE_Msk & ((value) << US_IMR_SPI_OVRE_Pos))
#define US_IMR_SPI_TXEMPTY_Pos                _U_(9)                                               /**< (US_IMR) TXEMPTY Interrupt Mask Position */
#define US_IMR_SPI_TXEMPTY_Msk                (_U_(0x1) << US_IMR_SPI_TXEMPTY_Pos)                 /**< (US_IMR) TXEMPTY Interrupt Mask Mask */
#define US_IMR_SPI_TXEMPTY(value)             (US_IMR_SPI_TXEMPTY_Msk & ((value) << US_IMR_SPI_TXEMPTY_Pos))
#define US_IMR_SPI_UNRE_Pos                   _U_(10)                                              /**< (US_IMR) SPI Underrun Error Interrupt Mask Position */
#define US_IMR_SPI_UNRE_Msk                   (_U_(0x1) << US_IMR_SPI_UNRE_Pos)                    /**< (US_IMR) SPI Underrun Error Interrupt Mask Mask */
#define US_IMR_SPI_UNRE(value)                (US_IMR_SPI_UNRE_Msk & ((value) << US_IMR_SPI_UNRE_Pos))
#define US_IMR_SPI_NSSE_Pos                   _U_(19)                                              /**< (US_IMR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Position */
#define US_IMR_SPI_NSSE_Msk                   (_U_(0x1) << US_IMR_SPI_NSSE_Pos)                    /**< (US_IMR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Mask */
#define US_IMR_SPI_NSSE(value)                (US_IMR_SPI_NSSE_Msk & ((value) << US_IMR_SPI_NSSE_Pos))
#define US_IMR_SPI_Msk                        _U_(0x00080623)                                       /**< (US_IMR_SPI) Register Mask  */

/* LIN mode */
#define US_IMR_LIN_RXRDY_Pos                  _U_(0)                                               /**< (US_IMR) RXRDY Interrupt Mask Position */
#define US_IMR_LIN_RXRDY_Msk                  (_U_(0x1) << US_IMR_LIN_RXRDY_Pos)                   /**< (US_IMR) RXRDY Interrupt Mask Mask */
#define US_IMR_LIN_RXRDY(value)               (US_IMR_LIN_RXRDY_Msk & ((value) << US_IMR_LIN_RXRDY_Pos))
#define US_IMR_LIN_TXRDY_Pos                  _U_(1)                                               /**< (US_IMR) TXRDY Interrupt Mask Position */
#define US_IMR_LIN_TXRDY_Msk                  (_U_(0x1) << US_IMR_LIN_TXRDY_Pos)                   /**< (US_IMR) TXRDY Interrupt Mask Mask */
#define US_IMR_LIN_TXRDY(value)               (US_IMR_LIN_TXRDY_Msk & ((value) << US_IMR_LIN_TXRDY_Pos))
#define US_IMR_LIN_OVRE_Pos                   _U_(5)                                               /**< (US_IMR) Overrun Error Interrupt Mask Position */
#define US_IMR_LIN_OVRE_Msk                   (_U_(0x1) << US_IMR_LIN_OVRE_Pos)                    /**< (US_IMR) Overrun Error Interrupt Mask Mask */
#define US_IMR_LIN_OVRE(value)                (US_IMR_LIN_OVRE_Msk & ((value) << US_IMR_LIN_OVRE_Pos))
#define US_IMR_LIN_FRAME_Pos                  _U_(6)                                               /**< (US_IMR) Framing Error Interrupt Mask Position */
#define US_IMR_LIN_FRAME_Msk                  (_U_(0x1) << US_IMR_LIN_FRAME_Pos)                   /**< (US_IMR) Framing Error Interrupt Mask Mask */
#define US_IMR_LIN_FRAME(value)               (US_IMR_LIN_FRAME_Msk & ((value) << US_IMR_LIN_FRAME_Pos))
#define US_IMR_LIN_PARE_Pos                   _U_(7)                                               /**< (US_IMR) Parity Error Interrupt Mask Position */
#define US_IMR_LIN_PARE_Msk                   (_U_(0x1) << US_IMR_LIN_PARE_Pos)                    /**< (US_IMR) Parity Error Interrupt Mask Mask */
#define US_IMR_LIN_PARE(value)                (US_IMR_LIN_PARE_Msk & ((value) << US_IMR_LIN_PARE_Pos))
#define US_IMR_LIN_TIMEOUT_Pos                _U_(8)                                               /**< (US_IMR) Timeout Interrupt Mask Position */
#define US_IMR_LIN_TIMEOUT_Msk                (_U_(0x1) << US_IMR_LIN_TIMEOUT_Pos)                 /**< (US_IMR) Timeout Interrupt Mask Mask */
#define US_IMR_LIN_TIMEOUT(value)             (US_IMR_LIN_TIMEOUT_Msk & ((value) << US_IMR_LIN_TIMEOUT_Pos))
#define US_IMR_LIN_TXEMPTY_Pos                _U_(9)                                               /**< (US_IMR) TXEMPTY Interrupt Mask Position */
#define US_IMR_LIN_TXEMPTY_Msk                (_U_(0x1) << US_IMR_LIN_TXEMPTY_Pos)                 /**< (US_IMR) TXEMPTY Interrupt Mask Mask */
#define US_IMR_LIN_TXEMPTY(value)             (US_IMR_LIN_TXEMPTY_Msk & ((value) << US_IMR_LIN_TXEMPTY_Pos))
#define US_IMR_LIN_LINBK_Pos                  _U_(13)                                              /**< (US_IMR) LIN Break Sent or LIN Break Received Interrupt Mask Position */
#define US_IMR_LIN_LINBK_Msk                  (_U_(0x1) << US_IMR_LIN_LINBK_Pos)                   /**< (US_IMR) LIN Break Sent or LIN Break Received Interrupt Mask Mask */
#define US_IMR_LIN_LINBK(value)               (US_IMR_LIN_LINBK_Msk & ((value) << US_IMR_LIN_LINBK_Pos))
#define US_IMR_LIN_LINID_Pos                  _U_(14)                                              /**< (US_IMR) LIN Identifier Sent or LIN Identifier Received Interrupt Mask Position */
#define US_IMR_LIN_LINID_Msk                  (_U_(0x1) << US_IMR_LIN_LINID_Pos)                   /**< (US_IMR) LIN Identifier Sent or LIN Identifier Received Interrupt Mask Mask */
#define US_IMR_LIN_LINID(value)               (US_IMR_LIN_LINID_Msk & ((value) << US_IMR_LIN_LINID_Pos))
#define US_IMR_LIN_LINTC_Pos                  _U_(15)                                              /**< (US_IMR) LIN Transfer Completed Interrupt Mask Position */
#define US_IMR_LIN_LINTC_Msk                  (_U_(0x1) << US_IMR_LIN_LINTC_Pos)                   /**< (US_IMR) LIN Transfer Completed Interrupt Mask Mask */
#define US_IMR_LIN_LINTC(value)               (US_IMR_LIN_LINTC_Msk & ((value) << US_IMR_LIN_LINTC_Pos))
#define US_IMR_LIN_LINBE_Pos                  _U_(25)                                              /**< (US_IMR) LIN Bus Error Interrupt Mask Position */
#define US_IMR_LIN_LINBE_Msk                  (_U_(0x1) << US_IMR_LIN_LINBE_Pos)                   /**< (US_IMR) LIN Bus Error Interrupt Mask Mask */
#define US_IMR_LIN_LINBE(value)               (US_IMR_LIN_LINBE_Msk & ((value) << US_IMR_LIN_LINBE_Pos))
#define US_IMR_LIN_LINISFE_Pos                _U_(26)                                              /**< (US_IMR) LIN Inconsistent Synch Field Error Interrupt Mask Position */
#define US_IMR_LIN_LINISFE_Msk                (_U_(0x1) << US_IMR_LIN_LINISFE_Pos)                 /**< (US_IMR) LIN Inconsistent Synch Field Error Interrupt Mask Mask */
#define US_IMR_LIN_LINISFE(value)             (US_IMR_LIN_LINISFE_Msk & ((value) << US_IMR_LIN_LINISFE_Pos))
#define US_IMR_LIN_LINIPE_Pos                 _U_(27)                                              /**< (US_IMR) LIN Identifier Parity Interrupt Mask Position */
#define US_IMR_LIN_LINIPE_Msk                 (_U_(0x1) << US_IMR_LIN_LINIPE_Pos)                  /**< (US_IMR) LIN Identifier Parity Interrupt Mask Mask */
#define US_IMR_LIN_LINIPE(value)              (US_IMR_LIN_LINIPE_Msk & ((value) << US_IMR_LIN_LINIPE_Pos))
#define US_IMR_LIN_LINCE_Pos                  _U_(28)                                              /**< (US_IMR) LIN Checksum Error Interrupt Mask Position */
#define US_IMR_LIN_LINCE_Msk                  (_U_(0x1) << US_IMR_LIN_LINCE_Pos)                   /**< (US_IMR) LIN Checksum Error Interrupt Mask Mask */
#define US_IMR_LIN_LINCE(value)               (US_IMR_LIN_LINCE_Msk & ((value) << US_IMR_LIN_LINCE_Pos))
#define US_IMR_LIN_LINSNRE_Pos                _U_(29)                                              /**< (US_IMR) LIN Slave Not Responding Error Interrupt Mask Position */
#define US_IMR_LIN_LINSNRE_Msk                (_U_(0x1) << US_IMR_LIN_LINSNRE_Pos)                 /**< (US_IMR) LIN Slave Not Responding Error Interrupt Mask Mask */
#define US_IMR_LIN_LINSNRE(value)             (US_IMR_LIN_LINSNRE_Msk & ((value) << US_IMR_LIN_LINSNRE_Pos))
#define US_IMR_LIN_LINSTE_Pos                 _U_(30)                                              /**< (US_IMR) LIN Synch Tolerance Error Interrupt Mask Position */
#define US_IMR_LIN_LINSTE_Msk                 (_U_(0x1) << US_IMR_LIN_LINSTE_Pos)                  /**< (US_IMR) LIN Synch Tolerance Error Interrupt Mask Mask */
#define US_IMR_LIN_LINSTE(value)              (US_IMR_LIN_LINSTE_Msk & ((value) << US_IMR_LIN_LINSTE_Pos))
#define US_IMR_LIN_LINHTE_Pos                 _U_(31)                                              /**< (US_IMR) LIN Header Timeout Error Interrupt Mask Position */
#define US_IMR_LIN_LINHTE_Msk                 (_U_(0x1) << US_IMR_LIN_LINHTE_Pos)                  /**< (US_IMR) LIN Header Timeout Error Interrupt Mask Mask */
#define US_IMR_LIN_LINHTE(value)              (US_IMR_LIN_LINHTE_Msk & ((value) << US_IMR_LIN_LINHTE_Pos))
#define US_IMR_LIN_Msk                        _U_(0xFE00E3E3)                                       /**< (US_IMR_LIN) Register Mask  */

/* LON mode */
#define US_IMR_LON_RXRDY_Pos                  _U_(0)                                               /**< (US_IMR) RXRDY Interrupt Mask Position */
#define US_IMR_LON_RXRDY_Msk                  (_U_(0x1) << US_IMR_LON_RXRDY_Pos)                   /**< (US_IMR) RXRDY Interrupt Mask Mask */
#define US_IMR_LON_RXRDY(value)               (US_IMR_LON_RXRDY_Msk & ((value) << US_IMR_LON_RXRDY_Pos))
#define US_IMR_LON_TXRDY_Pos                  _U_(1)                                               /**< (US_IMR) TXRDY Interrupt Mask Position */
#define US_IMR_LON_TXRDY_Msk                  (_U_(0x1) << US_IMR_LON_TXRDY_Pos)                   /**< (US_IMR) TXRDY Interrupt Mask Mask */
#define US_IMR_LON_TXRDY(value)               (US_IMR_LON_TXRDY_Msk & ((value) << US_IMR_LON_TXRDY_Pos))
#define US_IMR_LON_OVRE_Pos                   _U_(5)                                               /**< (US_IMR) Overrun Error Interrupt Mask Position */
#define US_IMR_LON_OVRE_Msk                   (_U_(0x1) << US_IMR_LON_OVRE_Pos)                    /**< (US_IMR) Overrun Error Interrupt Mask Mask */
#define US_IMR_LON_OVRE(value)                (US_IMR_LON_OVRE_Msk & ((value) << US_IMR_LON_OVRE_Pos))
#define US_IMR_LON_LSFE_Pos                   _U_(6)                                               /**< (US_IMR) LON Short Frame Error Interrupt Mask Position */
#define US_IMR_LON_LSFE_Msk                   (_U_(0x1) << US_IMR_LON_LSFE_Pos)                    /**< (US_IMR) LON Short Frame Error Interrupt Mask Mask */
#define US_IMR_LON_LSFE(value)                (US_IMR_LON_LSFE_Msk & ((value) << US_IMR_LON_LSFE_Pos))
#define US_IMR_LON_LCRCE_Pos                  _U_(7)                                               /**< (US_IMR) LON CRC Error Interrupt Mask Position */
#define US_IMR_LON_LCRCE_Msk                  (_U_(0x1) << US_IMR_LON_LCRCE_Pos)                   /**< (US_IMR) LON CRC Error Interrupt Mask Mask */
#define US_IMR_LON_LCRCE(value)               (US_IMR_LON_LCRCE_Msk & ((value) << US_IMR_LON_LCRCE_Pos))
#define US_IMR_LON_TXEMPTY_Pos                _U_(9)                                               /**< (US_IMR) TXEMPTY Interrupt Mask Position */
#define US_IMR_LON_TXEMPTY_Msk                (_U_(0x1) << US_IMR_LON_TXEMPTY_Pos)                 /**< (US_IMR) TXEMPTY Interrupt Mask Mask */
#define US_IMR_LON_TXEMPTY(value)             (US_IMR_LON_TXEMPTY_Msk & ((value) << US_IMR_LON_TXEMPTY_Pos))
#define US_IMR_LON_UNRE_Pos                   _U_(10)                                              /**< (US_IMR) SPI Underrun Error Interrupt Mask Position */
#define US_IMR_LON_UNRE_Msk                   (_U_(0x1) << US_IMR_LON_UNRE_Pos)                    /**< (US_IMR) SPI Underrun Error Interrupt Mask Mask */
#define US_IMR_LON_UNRE(value)                (US_IMR_LON_UNRE_Msk & ((value) << US_IMR_LON_UNRE_Pos))
#define US_IMR_LON_LTXD_Pos                   _U_(24)                                              /**< (US_IMR) LON Transmission Done Interrupt Mask Position */
#define US_IMR_LON_LTXD_Msk                   (_U_(0x1) << US_IMR_LON_LTXD_Pos)                    /**< (US_IMR) LON Transmission Done Interrupt Mask Mask */
#define US_IMR_LON_LTXD(value)                (US_IMR_LON_LTXD_Msk & ((value) << US_IMR_LON_LTXD_Pos))
#define US_IMR_LON_LCOL_Pos                   _U_(25)                                              /**< (US_IMR) LON Collision Interrupt Mask Position */
#define US_IMR_LON_LCOL_Msk                   (_U_(0x1) << US_IMR_LON_LCOL_Pos)                    /**< (US_IMR) LON Collision Interrupt Mask Mask */
#define US_IMR_LON_LCOL(value)                (US_IMR_LON_LCOL_Msk & ((value) << US_IMR_LON_LCOL_Pos))
#define US_IMR_LON_LFET_Pos                   _U_(26)                                              /**< (US_IMR) LON Frame Early Termination Interrupt Mask Position */
#define US_IMR_LON_LFET_Msk                   (_U_(0x1) << US_IMR_LON_LFET_Pos)                    /**< (US_IMR) LON Frame Early Termination Interrupt Mask Mask */
#define US_IMR_LON_LFET(value)                (US_IMR_LON_LFET_Msk & ((value) << US_IMR_LON_LFET_Pos))
#define US_IMR_LON_LRXD_Pos                   _U_(27)                                              /**< (US_IMR) LON Reception Done Interrupt Mask Position */
#define US_IMR_LON_LRXD_Msk                   (_U_(0x1) << US_IMR_LON_LRXD_Pos)                    /**< (US_IMR) LON Reception Done Interrupt Mask Mask */
#define US_IMR_LON_LRXD(value)                (US_IMR_LON_LRXD_Msk & ((value) << US_IMR_LON_LRXD_Pos))
#define US_IMR_LON_LBLOVFE_Pos                _U_(28)                                              /**< (US_IMR) LON Backlog Overflow Error Interrupt Mask Position */
#define US_IMR_LON_LBLOVFE_Msk                (_U_(0x1) << US_IMR_LON_LBLOVFE_Pos)                 /**< (US_IMR) LON Backlog Overflow Error Interrupt Mask Mask */
#define US_IMR_LON_LBLOVFE(value)             (US_IMR_LON_LBLOVFE_Msk & ((value) << US_IMR_LON_LBLOVFE_Pos))
#define US_IMR_LON_Msk                        _U_(0x1F0006E3)                                       /**< (US_IMR_LON) Register Mask  */


/* -------- US_CSR : (USART Offset: 0x14) ( R/ 32) Channel Status Register -------- */
#define US_CSR_Msk                            _U_(0x00000000)                                      /**< (US_CSR) Register Mask  */

/* USART mode */
#define US_CSR_USART_RXRDY_Pos                _U_(0)                                               /**< (US_CSR) Receiver Ready (cleared by reading US_RHR) Position */
#define US_CSR_USART_RXRDY_Msk                (_U_(0x1) << US_CSR_USART_RXRDY_Pos)                 /**< (US_CSR) Receiver Ready (cleared by reading US_RHR) Mask */
#define US_CSR_USART_RXRDY(value)             (US_CSR_USART_RXRDY_Msk & ((value) << US_CSR_USART_RXRDY_Pos))
#define US_CSR_USART_TXRDY_Pos                _U_(1)                                               /**< (US_CSR) Transmitter Ready (cleared by writing US_THR) Position */
#define US_CSR_USART_TXRDY_Msk                (_U_(0x1) << US_CSR_USART_TXRDY_Pos)                 /**< (US_CSR) Transmitter Ready (cleared by writing US_THR) Mask */
#define US_CSR_USART_TXRDY(value)             (US_CSR_USART_TXRDY_Msk & ((value) << US_CSR_USART_TXRDY_Pos))
#define US_CSR_USART_RXBRK_Pos                _U_(2)                                               /**< (US_CSR) Break Received/End of Break (cleared by writing a one to bit US_CR.RSTSTA) Position */
#define US_CSR_USART_RXBRK_Msk                (_U_(0x1) << US_CSR_USART_RXBRK_Pos)                 /**< (US_CSR) Break Received/End of Break (cleared by writing a one to bit US_CR.RSTSTA) Mask */
#define US_CSR_USART_RXBRK(value)             (US_CSR_USART_RXBRK_Msk & ((value) << US_CSR_USART_RXBRK_Pos))
#define US_CSR_USART_OVRE_Pos                 _U_(5)                                               /**< (US_CSR) Overrun Error (cleared by writing a one to bit US_CR.RSTSTA) Position */
#define US_CSR_USART_OVRE_Msk                 (_U_(0x1) << US_CSR_USART_OVRE_Pos)                  /**< (US_CSR) Overrun Error (cleared by writing a one to bit US_CR.RSTSTA) Mask */
#define US_CSR_USART_OVRE(value)              (US_CSR_USART_OVRE_Msk & ((value) << US_CSR_USART_OVRE_Pos))
#define US_CSR_USART_FRAME_Pos                _U_(6)                                               /**< (US_CSR) Framing Error (cleared by writing a one to bit US_CR.RSTSTA) Position */
#define US_CSR_USART_FRAME_Msk                (_U_(0x1) << US_CSR_USART_FRAME_Pos)                 /**< (US_CSR) Framing Error (cleared by writing a one to bit US_CR.RSTSTA) Mask */
#define US_CSR_USART_FRAME(value)             (US_CSR_USART_FRAME_Msk & ((value) << US_CSR_USART_FRAME_Pos))
#define US_CSR_USART_PARE_Pos                 _U_(7)                                               /**< (US_CSR) Parity Error (cleared by writing a one to bit US_CR.RSTSTA) Position */
#define US_CSR_USART_PARE_Msk                 (_U_(0x1) << US_CSR_USART_PARE_Pos)                  /**< (US_CSR) Parity Error (cleared by writing a one to bit US_CR.RSTSTA) Mask */
#define US_CSR_USART_PARE(value)              (US_CSR_USART_PARE_Msk & ((value) << US_CSR_USART_PARE_Pos))
#define US_CSR_USART_TIMEOUT_Pos              _U_(8)                                               /**< (US_CSR) Receiver Timeout (cleared by writing a one to bit US_CR.STTTO) Position */
#define US_CSR_USART_TIMEOUT_Msk              (_U_(0x1) << US_CSR_USART_TIMEOUT_Pos)               /**< (US_CSR) Receiver Timeout (cleared by writing a one to bit US_CR.STTTO) Mask */
#define US_CSR_USART_TIMEOUT(value)           (US_CSR_USART_TIMEOUT_Msk & ((value) << US_CSR_USART_TIMEOUT_Pos))
#define US_CSR_USART_TXEMPTY_Pos              _U_(9)                                               /**< (US_CSR) Transmitter Empty (cleared by writing US_THR) Position */
#define US_CSR_USART_TXEMPTY_Msk              (_U_(0x1) << US_CSR_USART_TXEMPTY_Pos)               /**< (US_CSR) Transmitter Empty (cleared by writing US_THR) Mask */
#define US_CSR_USART_TXEMPTY(value)           (US_CSR_USART_TXEMPTY_Msk & ((value) << US_CSR_USART_TXEMPTY_Pos))
#define US_CSR_USART_ITER_Pos                 _U_(10)                                              /**< (US_CSR) Max Number of Repetitions Reached (cleared by writing a one to bit US_CR.RSTIT) Position */
#define US_CSR_USART_ITER_Msk                 (_U_(0x1) << US_CSR_USART_ITER_Pos)                  /**< (US_CSR) Max Number of Repetitions Reached (cleared by writing a one to bit US_CR.RSTIT) Mask */
#define US_CSR_USART_ITER(value)              (US_CSR_USART_ITER_Msk & ((value) << US_CSR_USART_ITER_Pos))
#define US_CSR_USART_NACK_Pos                 _U_(13)                                              /**< (US_CSR) Non Acknowledge Interrupt (cleared by writing a one to bit US_CR.RSTNACK) Position */
#define US_CSR_USART_NACK_Msk                 (_U_(0x1) << US_CSR_USART_NACK_Pos)                  /**< (US_CSR) Non Acknowledge Interrupt (cleared by writing a one to bit US_CR.RSTNACK) Mask */
#define US_CSR_USART_NACK(value)              (US_CSR_USART_NACK_Msk & ((value) << US_CSR_USART_NACK_Pos))
#define US_CSR_USART_RIIC_Pos                 _U_(16)                                              /**< (US_CSR) Ring Indicator Input Change Flag (cleared on read) Position */
#define US_CSR_USART_RIIC_Msk                 (_U_(0x1) << US_CSR_USART_RIIC_Pos)                  /**< (US_CSR) Ring Indicator Input Change Flag (cleared on read) Mask */
#define US_CSR_USART_RIIC(value)              (US_CSR_USART_RIIC_Msk & ((value) << US_CSR_USART_RIIC_Pos))
#define US_CSR_USART_DSRIC_Pos                _U_(17)                                              /**< (US_CSR) Data Set Ready Input Change Flag (cleared on read) Position */
#define US_CSR_USART_DSRIC_Msk                (_U_(0x1) << US_CSR_USART_DSRIC_Pos)                 /**< (US_CSR) Data Set Ready Input Change Flag (cleared on read) Mask */
#define US_CSR_USART_DSRIC(value)             (US_CSR_USART_DSRIC_Msk & ((value) << US_CSR_USART_DSRIC_Pos))
#define US_CSR_USART_DCDIC_Pos                _U_(18)                                              /**< (US_CSR) Data Carrier Detect Input Change Flag (cleared on read) Position */
#define US_CSR_USART_DCDIC_Msk                (_U_(0x1) << US_CSR_USART_DCDIC_Pos)                 /**< (US_CSR) Data Carrier Detect Input Change Flag (cleared on read) Mask */
#define US_CSR_USART_DCDIC(value)             (US_CSR_USART_DCDIC_Msk & ((value) << US_CSR_USART_DCDIC_Pos))
#define US_CSR_USART_CTSIC_Pos                _U_(19)                                              /**< (US_CSR) Clear to Send Input Change Flag (cleared on read) Position */
#define US_CSR_USART_CTSIC_Msk                (_U_(0x1) << US_CSR_USART_CTSIC_Pos)                 /**< (US_CSR) Clear to Send Input Change Flag (cleared on read) Mask */
#define US_CSR_USART_CTSIC(value)             (US_CSR_USART_CTSIC_Msk & ((value) << US_CSR_USART_CTSIC_Pos))
#define US_CSR_USART_RI_Pos                   _U_(20)                                              /**< (US_CSR) Image of RI Input Position */
#define US_CSR_USART_RI_Msk                   (_U_(0x1) << US_CSR_USART_RI_Pos)                    /**< (US_CSR) Image of RI Input Mask */
#define US_CSR_USART_RI(value)                (US_CSR_USART_RI_Msk & ((value) << US_CSR_USART_RI_Pos))
#define US_CSR_USART_DSR_Pos                  _U_(21)                                              /**< (US_CSR) Image of DSR Input Position */
#define US_CSR_USART_DSR_Msk                  (_U_(0x1) << US_CSR_USART_DSR_Pos)                   /**< (US_CSR) Image of DSR Input Mask */
#define US_CSR_USART_DSR(value)               (US_CSR_USART_DSR_Msk & ((value) << US_CSR_USART_DSR_Pos))
#define US_CSR_USART_DCD_Pos                  _U_(22)                                              /**< (US_CSR) Image of DCD Input Position */
#define US_CSR_USART_DCD_Msk                  (_U_(0x1) << US_CSR_USART_DCD_Pos)                   /**< (US_CSR) Image of DCD Input Mask */
#define US_CSR_USART_DCD(value)               (US_CSR_USART_DCD_Msk & ((value) << US_CSR_USART_DCD_Pos))
#define US_CSR_USART_CTS_Pos                  _U_(23)                                              /**< (US_CSR) Image of CTS Input Position */
#define US_CSR_USART_CTS_Msk                  (_U_(0x1) << US_CSR_USART_CTS_Pos)                   /**< (US_CSR) Image of CTS Input Mask */
#define US_CSR_USART_CTS(value)               (US_CSR_USART_CTS_Msk & ((value) << US_CSR_USART_CTS_Pos))
#define US_CSR_USART_MANERR_Pos               _U_(24)                                              /**< (US_CSR) Manchester Error (cleared by writing a one to the bit US_CR.RSTSTA) Position */
#define US_CSR_USART_MANERR_Msk               (_U_(0x1) << US_CSR_USART_MANERR_Pos)                /**< (US_CSR) Manchester Error (cleared by writing a one to the bit US_CR.RSTSTA) Mask */
#define US_CSR_USART_MANERR(value)            (US_CSR_USART_MANERR_Msk & ((value) << US_CSR_USART_MANERR_Pos))
#define US_CSR_USART_Msk                      _U_(0x01FF27E7)                                       /**< (US_CSR_USART) Register Mask  */

/* SPI mode */
#define US_CSR_SPI_RXRDY_Pos                  _U_(0)                                               /**< (US_CSR) Receiver Ready (cleared by reading US_RHR) Position */
#define US_CSR_SPI_RXRDY_Msk                  (_U_(0x1) << US_CSR_SPI_RXRDY_Pos)                   /**< (US_CSR) Receiver Ready (cleared by reading US_RHR) Mask */
#define US_CSR_SPI_RXRDY(value)               (US_CSR_SPI_RXRDY_Msk & ((value) << US_CSR_SPI_RXRDY_Pos))
#define US_CSR_SPI_TXRDY_Pos                  _U_(1)                                               /**< (US_CSR) Transmitter Ready (cleared by writing US_THR) Position */
#define US_CSR_SPI_TXRDY_Msk                  (_U_(0x1) << US_CSR_SPI_TXRDY_Pos)                   /**< (US_CSR) Transmitter Ready (cleared by writing US_THR) Mask */
#define US_CSR_SPI_TXRDY(value)               (US_CSR_SPI_TXRDY_Msk & ((value) << US_CSR_SPI_TXRDY_Pos))
#define US_CSR_SPI_OVRE_Pos                   _U_(5)                                               /**< (US_CSR) Overrun Error (cleared by writing a one to bit US_CR.RSTSTA) Position */
#define US_CSR_SPI_OVRE_Msk                   (_U_(0x1) << US_CSR_SPI_OVRE_Pos)                    /**< (US_CSR) Overrun Error (cleared by writing a one to bit US_CR.RSTSTA) Mask */
#define US_CSR_SPI_OVRE(value)                (US_CSR_SPI_OVRE_Msk & ((value) << US_CSR_SPI_OVRE_Pos))
#define US_CSR_SPI_TXEMPTY_Pos                _U_(9)                                               /**< (US_CSR) Transmitter Empty (cleared by writing US_THR) Position */
#define US_CSR_SPI_TXEMPTY_Msk                (_U_(0x1) << US_CSR_SPI_TXEMPTY_Pos)                 /**< (US_CSR) Transmitter Empty (cleared by writing US_THR) Mask */
#define US_CSR_SPI_TXEMPTY(value)             (US_CSR_SPI_TXEMPTY_Msk & ((value) << US_CSR_SPI_TXEMPTY_Pos))
#define US_CSR_SPI_UNRE_Pos                   _U_(10)                                              /**< (US_CSR) SPI Underrun Error Position */
#define US_CSR_SPI_UNRE_Msk                   (_U_(0x1) << US_CSR_SPI_UNRE_Pos)                    /**< (US_CSR) SPI Underrun Error Mask */
#define US_CSR_SPI_UNRE(value)                (US_CSR_SPI_UNRE_Msk & ((value) << US_CSR_SPI_UNRE_Pos))
#define US_CSR_SPI_NSSE_Pos                   _U_(19)                                              /**< (US_CSR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Position */
#define US_CSR_SPI_NSSE_Msk                   (_U_(0x1) << US_CSR_SPI_NSSE_Pos)                    /**< (US_CSR) NSS Line (Driving CTS Pin) Rising or Falling Edge Event Mask */
#define US_CSR_SPI_NSSE(value)                (US_CSR_SPI_NSSE_Msk & ((value) << US_CSR_SPI_NSSE_Pos))
#define US_CSR_SPI_NSS_Pos                    _U_(23)                                              /**< (US_CSR) Image of NSS Line Position */
#define US_CSR_SPI_NSS_Msk                    (_U_(0x1) << US_CSR_SPI_NSS_Pos)                     /**< (US_CSR) Image of NSS Line Mask */
#define US_CSR_SPI_NSS(value)                 (US_CSR_SPI_NSS_Msk & ((value) << US_CSR_SPI_NSS_Pos))
#define US_CSR_SPI_Msk                        _U_(0x00880623)                                       /**< (US_CSR_SPI) Register Mask  */

/* LIN mode */
#define US_CSR_LIN_RXRDY_Pos                  _U_(0)                                               /**< (US_CSR) Receiver Ready (cleared by reading US_RHR) Position */
#define US_CSR_LIN_RXRDY_Msk                  (_U_(0x1) << US_CSR_LIN_RXRDY_Pos)                   /**< (US_CSR) Receiver Ready (cleared by reading US_RHR) Mask */
#define US_CSR_LIN_RXRDY(value)               (US_CSR_LIN_RXRDY_Msk & ((value) << US_CSR_LIN_RXRDY_Pos))
#define US_CSR_LIN_TXRDY_Pos                  _U_(1)                                               /**< (US_CSR) Transmitter Ready (cleared by writing US_THR) Position */
#define US_CSR_LIN_TXRDY_Msk                  (_U_(0x1) << US_CSR_LIN_TXRDY_Pos)                   /**< (US_CSR) Transmitter Ready (cleared by writing US_THR) Mask */
#define US_CSR_LIN_TXRDY(value)               (US_CSR_LIN_TXRDY_Msk & ((value) << US_CSR_LIN_TXRDY_Pos))
#define US_CSR_LIN_OVRE_Pos                   _U_(5)                                               /**< (US_CSR) Overrun Error (cleared by writing a one to bit US_CR.RSTSTA) Position */
#define US_CSR_LIN_OVRE_Msk                   (_U_(0x1) << US_CSR_LIN_OVRE_Pos)                    /**< (US_CSR) Overrun Error (cleared by writing a one to bit US_CR.RSTSTA) Mask */
#define US_CSR_LIN_OVRE(value)                (US_CSR_LIN_OVRE_Msk & ((value) << US_CSR_LIN_OVRE_Pos))
#define US_CSR_LIN_FRAME_Pos                  _U_(6)                                               /**< (US_CSR) Framing Error (cleared by writing a one to bit US_CR.RSTSTA) Position */
#define US_CSR_LIN_FRAME_Msk                  (_U_(0x1) << US_CSR_LIN_FRAME_Pos)                   /**< (US_CSR) Framing Error (cleared by writing a one to bit US_CR.RSTSTA) Mask */
#define US_CSR_LIN_FRAME(value)               (US_CSR_LIN_FRAME_Msk & ((value) << US_CSR_LIN_FRAME_Pos))
#define US_CSR_LIN_PARE_Pos                   _U_(7)                                               /**< (US_CSR) Parity Error (cleared by writing a one to bit US_CR.RSTSTA) Position */
#define US_CSR_LIN_PARE_Msk                   (_U_(0x1) << US_CSR_LIN_PARE_Pos)                    /**< (US_CSR) Parity Error (cleared by writing a one to bit US_CR.RSTSTA) Mask */
#define US_CSR_LIN_PARE(value)                (US_CSR_LIN_PARE_Msk & ((value) << US_CSR_LIN_PARE_Pos))
#define US_CSR_LIN_TIMEOUT_Pos                _U_(8)                                               /**< (US_CSR) Receiver Timeout (cleared by writing a one to bit US_CR.STTTO) Position */
#define US_CSR_LIN_TIMEOUT_Msk                (_U_(0x1) << US_CSR_LIN_TIMEOUT_Pos)                 /**< (US_CSR) Receiver Timeout (cleared by writing a one to bit US_CR.STTTO) Mask */
#define US_CSR_LIN_TIMEOUT(value)             (US_CSR_LIN_TIMEOUT_Msk & ((value) << US_CSR_LIN_TIMEOUT_Pos))
#define US_CSR_LIN_TXEMPTY_Pos                _U_(9)                                               /**< (US_CSR) Transmitter Empty (cleared by writing US_THR) Position */
#define US_CSR_LIN_TXEMPTY_Msk                (_U_(0x1) << US_CSR_LIN_TXEMPTY_Pos)                 /**< (US_CSR) Transmitter Empty (cleared by writing US_THR) Mask */
#define US_CSR_LIN_TXEMPTY(value)             (US_CSR_LIN_TXEMPTY_Msk & ((value) << US_CSR_LIN_TXEMPTY_Pos))
#define US_CSR_LIN_LINBK_Pos                  _U_(13)                                              /**< (US_CSR) LIN Break Sent or LIN Break Received Position */
#define US_CSR_LIN_LINBK_Msk                  (_U_(0x1) << US_CSR_LIN_LINBK_Pos)                   /**< (US_CSR) LIN Break Sent or LIN Break Received Mask */
#define US_CSR_LIN_LINBK(value)               (US_CSR_LIN_LINBK_Msk & ((value) << US_CSR_LIN_LINBK_Pos))
#define US_CSR_LIN_LINID_Pos                  _U_(14)                                              /**< (US_CSR) LIN Identifier Sent or LIN Identifier Received Position */
#define US_CSR_LIN_LINID_Msk                  (_U_(0x1) << US_CSR_LIN_LINID_Pos)                   /**< (US_CSR) LIN Identifier Sent or LIN Identifier Received Mask */
#define US_CSR_LIN_LINID(value)               (US_CSR_LIN_LINID_Msk & ((value) << US_CSR_LIN_LINID_Pos))
#define US_CSR_LIN_LINTC_Pos                  _U_(15)                                              /**< (US_CSR) LIN Transfer Completed Position */
#define US_CSR_LIN_LINTC_Msk                  (_U_(0x1) << US_CSR_LIN_LINTC_Pos)                   /**< (US_CSR) LIN Transfer Completed Mask */
#define US_CSR_LIN_LINTC(value)               (US_CSR_LIN_LINTC_Msk & ((value) << US_CSR_LIN_LINTC_Pos))
#define US_CSR_LIN_LINBLS_Pos                 _U_(23)                                              /**< (US_CSR) LIN Bus Line Status Position */
#define US_CSR_LIN_LINBLS_Msk                 (_U_(0x1) << US_CSR_LIN_LINBLS_Pos)                  /**< (US_CSR) LIN Bus Line Status Mask */
#define US_CSR_LIN_LINBLS(value)              (US_CSR_LIN_LINBLS_Msk & ((value) << US_CSR_LIN_LINBLS_Pos))
#define US_CSR_LIN_LINBE_Pos                  _U_(25)                                              /**< (US_CSR) LIN Bus Error Position */
#define US_CSR_LIN_LINBE_Msk                  (_U_(0x1) << US_CSR_LIN_LINBE_Pos)                   /**< (US_CSR) LIN Bus Error Mask */
#define US_CSR_LIN_LINBE(value)               (US_CSR_LIN_LINBE_Msk & ((value) << US_CSR_LIN_LINBE_Pos))
#define US_CSR_LIN_LINISFE_Pos                _U_(26)                                              /**< (US_CSR) LIN Inconsistent Synch Field Error Position */
#define US_CSR_LIN_LINISFE_Msk                (_U_(0x1) << US_CSR_LIN_LINISFE_Pos)                 /**< (US_CSR) LIN Inconsistent Synch Field Error Mask */
#define US_CSR_LIN_LINISFE(value)             (US_CSR_LIN_LINISFE_Msk & ((value) << US_CSR_LIN_LINISFE_Pos))
#define US_CSR_LIN_LINIPE_Pos                 _U_(27)                                              /**< (US_CSR) LIN Identifier Parity Error Position */
#define US_CSR_LIN_LINIPE_Msk                 (_U_(0x1) << US_CSR_LIN_LINIPE_Pos)                  /**< (US_CSR) LIN Identifier Parity Error Mask */
#define US_CSR_LIN_LINIPE(value)              (US_CSR_LIN_LINIPE_Msk & ((value) << US_CSR_LIN_LINIPE_Pos))
#define US_CSR_LIN_LINCE_Pos                  _U_(28)                                              /**< (US_CSR) LIN Checksum Error Position */
#define US_CSR_LIN_LINCE_Msk                  (_U_(0x1) << US_CSR_LIN_LINCE_Pos)                   /**< (US_CSR) LIN Checksum Error Mask */
#define US_CSR_LIN_LINCE(value)               (US_CSR_LIN_LINCE_Msk & ((value) << US_CSR_LIN_LINCE_Pos))
#define US_CSR_LIN_LINSNRE_Pos                _U_(29)                                              /**< (US_CSR) LIN Slave Not Responding Error Interrupt Mask Position */
#define US_CSR_LIN_LINSNRE_Msk                (_U_(0x1) << US_CSR_LIN_LINSNRE_Pos)                 /**< (US_CSR) LIN Slave Not Responding Error Interrupt Mask Mask */
#define US_CSR_LIN_LINSNRE(value)             (US_CSR_LIN_LINSNRE_Msk & ((value) << US_CSR_LIN_LINSNRE_Pos))
#define US_CSR_LIN_LINSTE_Pos                 _U_(30)                                              /**< (US_CSR) LIN Synch Tolerance Error Position */
#define US_CSR_LIN_LINSTE_Msk                 (_U_(0x1) << US_CSR_LIN_LINSTE_Pos)                  /**< (US_CSR) LIN Synch Tolerance Error Mask */
#define US_CSR_LIN_LINSTE(value)              (US_CSR_LIN_LINSTE_Msk & ((value) << US_CSR_LIN_LINSTE_Pos))
#define US_CSR_LIN_LINHTE_Pos                 _U_(31)                                              /**< (US_CSR) LIN Header Timeout Error Position */
#define US_CSR_LIN_LINHTE_Msk                 (_U_(0x1) << US_CSR_LIN_LINHTE_Pos)                  /**< (US_CSR) LIN Header Timeout Error Mask */
#define US_CSR_LIN_LINHTE(value)              (US_CSR_LIN_LINHTE_Msk & ((value) << US_CSR_LIN_LINHTE_Pos))
#define US_CSR_LIN_Msk                        _U_(0xFE80E3E3)                                       /**< (US_CSR_LIN) Register Mask  */

/* LON mode */
#define US_CSR_LON_RXRDY_Pos                  _U_(0)                                               /**< (US_CSR) Receiver Ready (cleared by reading US_RHR) Position */
#define US_CSR_LON_RXRDY_Msk                  (_U_(0x1) << US_CSR_LON_RXRDY_Pos)                   /**< (US_CSR) Receiver Ready (cleared by reading US_RHR) Mask */
#define US_CSR_LON_RXRDY(value)               (US_CSR_LON_RXRDY_Msk & ((value) << US_CSR_LON_RXRDY_Pos))
#define US_CSR_LON_TXRDY_Pos                  _U_(1)                                               /**< (US_CSR) Transmitter Ready (cleared by writing US_THR) Position */
#define US_CSR_LON_TXRDY_Msk                  (_U_(0x1) << US_CSR_LON_TXRDY_Pos)                   /**< (US_CSR) Transmitter Ready (cleared by writing US_THR) Mask */
#define US_CSR_LON_TXRDY(value)               (US_CSR_LON_TXRDY_Msk & ((value) << US_CSR_LON_TXRDY_Pos))
#define US_CSR_LON_OVRE_Pos                   _U_(5)                                               /**< (US_CSR) Overrun Error (cleared by writing a one to bit US_CR.RSTSTA) Position */
#define US_CSR_LON_OVRE_Msk                   (_U_(0x1) << US_CSR_LON_OVRE_Pos)                    /**< (US_CSR) Overrun Error (cleared by writing a one to bit US_CR.RSTSTA) Mask */
#define US_CSR_LON_OVRE(value)                (US_CSR_LON_OVRE_Msk & ((value) << US_CSR_LON_OVRE_Pos))
#define US_CSR_LON_LSFE_Pos                   _U_(6)                                               /**< (US_CSR) LON Short Frame Error Position */
#define US_CSR_LON_LSFE_Msk                   (_U_(0x1) << US_CSR_LON_LSFE_Pos)                    /**< (US_CSR) LON Short Frame Error Mask */
#define US_CSR_LON_LSFE(value)                (US_CSR_LON_LSFE_Msk & ((value) << US_CSR_LON_LSFE_Pos))
#define US_CSR_LON_LCRCE_Pos                  _U_(7)                                               /**< (US_CSR) LON CRC Error Position */
#define US_CSR_LON_LCRCE_Msk                  (_U_(0x1) << US_CSR_LON_LCRCE_Pos)                   /**< (US_CSR) LON CRC Error Mask */
#define US_CSR_LON_LCRCE(value)               (US_CSR_LON_LCRCE_Msk & ((value) << US_CSR_LON_LCRCE_Pos))
#define US_CSR_LON_TXEMPTY_Pos                _U_(9)                                               /**< (US_CSR) Transmitter Empty (cleared by writing US_THR) Position */
#define US_CSR_LON_TXEMPTY_Msk                (_U_(0x1) << US_CSR_LON_TXEMPTY_Pos)                 /**< (US_CSR) Transmitter Empty (cleared by writing US_THR) Mask */
#define US_CSR_LON_TXEMPTY(value)             (US_CSR_LON_TXEMPTY_Msk & ((value) << US_CSR_LON_TXEMPTY_Pos))
#define US_CSR_LON_UNRE_Pos                   _U_(10)                                              /**< (US_CSR) Underrun Error Position */
#define US_CSR_LON_UNRE_Msk                   (_U_(0x1) << US_CSR_LON_UNRE_Pos)                    /**< (US_CSR) Underrun Error Mask */
#define US_CSR_LON_UNRE(value)                (US_CSR_LON_UNRE_Msk & ((value) << US_CSR_LON_UNRE_Pos))
#define US_CSR_LON_LTXD_Pos                   _U_(24)                                              /**< (US_CSR) LON Transmission End Flag Position */
#define US_CSR_LON_LTXD_Msk                   (_U_(0x1) << US_CSR_LON_LTXD_Pos)                    /**< (US_CSR) LON Transmission End Flag Mask */
#define US_CSR_LON_LTXD(value)                (US_CSR_LON_LTXD_Msk & ((value) << US_CSR_LON_LTXD_Pos))
#define US_CSR_LON_LCOL_Pos                   _U_(25)                                              /**< (US_CSR) LON Collision Detected Flag Position */
#define US_CSR_LON_LCOL_Msk                   (_U_(0x1) << US_CSR_LON_LCOL_Pos)                    /**< (US_CSR) LON Collision Detected Flag Mask */
#define US_CSR_LON_LCOL(value)                (US_CSR_LON_LCOL_Msk & ((value) << US_CSR_LON_LCOL_Pos))
#define US_CSR_LON_LFET_Pos                   _U_(26)                                              /**< (US_CSR) LON Frame Early Termination Position */
#define US_CSR_LON_LFET_Msk                   (_U_(0x1) << US_CSR_LON_LFET_Pos)                    /**< (US_CSR) LON Frame Early Termination Mask */
#define US_CSR_LON_LFET(value)                (US_CSR_LON_LFET_Msk & ((value) << US_CSR_LON_LFET_Pos))
#define US_CSR_LON_LRXD_Pos                   _U_(27)                                              /**< (US_CSR) LON Reception End Flag Position */
#define US_CSR_LON_LRXD_Msk                   (_U_(0x1) << US_CSR_LON_LRXD_Pos)                    /**< (US_CSR) LON Reception End Flag Mask */
#define US_CSR_LON_LRXD(value)                (US_CSR_LON_LRXD_Msk & ((value) << US_CSR_LON_LRXD_Pos))
#define US_CSR_LON_LBLOVFE_Pos                _U_(28)                                              /**< (US_CSR) LON Backlog Overflow Error Position */
#define US_CSR_LON_LBLOVFE_Msk                (_U_(0x1) << US_CSR_LON_LBLOVFE_Pos)                 /**< (US_CSR) LON Backlog Overflow Error Mask */
#define US_CSR_LON_LBLOVFE(value)             (US_CSR_LON_LBLOVFE_Msk & ((value) << US_CSR_LON_LBLOVFE_Pos))
#define US_CSR_LON_Msk                        _U_(0x1F0006E3)                                       /**< (US_CSR_LON) Register Mask  */


/* -------- US_RHR : (USART Offset: 0x18) ( R/ 32) Receive Holding Register -------- */
#define US_RHR_RXCHR_Pos                      _U_(0)                                               /**< (US_RHR) Received Character Position */
#define US_RHR_RXCHR_Msk                      (_U_(0x1FF) << US_RHR_RXCHR_Pos)                     /**< (US_RHR) Received Character Mask */
#define US_RHR_RXCHR(value)                   (US_RHR_RXCHR_Msk & ((value) << US_RHR_RXCHR_Pos))  
#define US_RHR_RXSYNH_Pos                     _U_(15)                                              /**< (US_RHR) Received Sync Position */
#define US_RHR_RXSYNH_Msk                     (_U_(0x1) << US_RHR_RXSYNH_Pos)                      /**< (US_RHR) Received Sync Mask */
#define US_RHR_RXSYNH(value)                  (US_RHR_RXSYNH_Msk & ((value) << US_RHR_RXSYNH_Pos))
#define US_RHR_Msk                            _U_(0x000081FF)                                      /**< (US_RHR) Register Mask  */


/* -------- US_THR : (USART Offset: 0x1C) ( /W 32) Transmit Holding Register -------- */
#define US_THR_TXCHR_Pos                      _U_(0)                                               /**< (US_THR) Character to be Transmitted Position */
#define US_THR_TXCHR_Msk                      (_U_(0x1FF) << US_THR_TXCHR_Pos)                     /**< (US_THR) Character to be Transmitted Mask */
#define US_THR_TXCHR(value)                   (US_THR_TXCHR_Msk & ((value) << US_THR_TXCHR_Pos))  
#define US_THR_TXSYNH_Pos                     _U_(15)                                              /**< (US_THR) Sync Field to be Transmitted Position */
#define US_THR_TXSYNH_Msk                     (_U_(0x1) << US_THR_TXSYNH_Pos)                      /**< (US_THR) Sync Field to be Transmitted Mask */
#define US_THR_TXSYNH(value)                  (US_THR_TXSYNH_Msk & ((value) << US_THR_TXSYNH_Pos))
#define US_THR_Msk                            _U_(0x000081FF)                                      /**< (US_THR) Register Mask  */


/* -------- US_BRGR : (USART Offset: 0x20) (R/W 32) Baud Rate Generator Register -------- */
#define US_BRGR_CD_Pos                        _U_(0)                                               /**< (US_BRGR) Clock Divider Position */
#define US_BRGR_CD_Msk                        (_U_(0xFFFF) << US_BRGR_CD_Pos)                      /**< (US_BRGR) Clock Divider Mask */
#define US_BRGR_CD(value)                     (US_BRGR_CD_Msk & ((value) << US_BRGR_CD_Pos))      
#define US_BRGR_FP_Pos                        _U_(16)                                              /**< (US_BRGR) Fractional Part Position */
#define US_BRGR_FP_Msk                        (_U_(0x7) << US_BRGR_FP_Pos)                         /**< (US_BRGR) Fractional Part Mask */
#define US_BRGR_FP(value)                     (US_BRGR_FP_Msk & ((value) << US_BRGR_FP_Pos))      
#define US_BRGR_Msk                           _U_(0x0007FFFF)                                      /**< (US_BRGR) Register Mask  */


/* -------- US_RTOR : (USART Offset: 0x24) (R/W 32) Receiver Timeout Register -------- */
#define US_RTOR_TO_Pos                        _U_(0)                                               /**< (US_RTOR) Timeout Value Position */
#define US_RTOR_TO_Msk                        (_U_(0x1FFFF) << US_RTOR_TO_Pos)                     /**< (US_RTOR) Timeout Value Mask */
#define US_RTOR_TO(value)                     (US_RTOR_TO_Msk & ((value) << US_RTOR_TO_Pos))      
#define US_RTOR_Msk                           _U_(0x0001FFFF)                                      /**< (US_RTOR) Register Mask  */


/* -------- US_TTGR : (USART Offset: 0x28) (R/W 32) Transmitter Timeguard Register -------- */
#define US_TTGR_Msk                           _U_(0x00000000)                                      /**< (US_TTGR) Register Mask  */

/* USART mode */
#define US_TTGR_USART_TG_Pos                  _U_(0)                                               /**< (US_TTGR) Timeguard Value Position */
#define US_TTGR_USART_TG_Msk                  (_U_(0xFF) << US_TTGR_USART_TG_Pos)                  /**< (US_TTGR) Timeguard Value Mask */
#define US_TTGR_USART_TG(value)               (US_TTGR_USART_TG_Msk & ((value) << US_TTGR_USART_TG_Pos))
#define US_TTGR_USART_Msk                     _U_(0x000000FF)                                       /**< (US_TTGR_USART) Register Mask  */

/* LON mode */
#define US_TTGR_LON_PCYCLE_Pos                _U_(0)                                               /**< (US_TTGR) LON PCYCLE Length Position */
#define US_TTGR_LON_PCYCLE_Msk                (_U_(0xFFFFFF) << US_TTGR_LON_PCYCLE_Pos)            /**< (US_TTGR) LON PCYCLE Length Mask */
#define US_TTGR_LON_PCYCLE(value)             (US_TTGR_LON_PCYCLE_Msk & ((value) << US_TTGR_LON_PCYCLE_Pos))
#define US_TTGR_LON_Msk                       _U_(0x00FFFFFF)                                       /**< (US_TTGR_LON) Register Mask  */


/* -------- US_FIDI : (USART Offset: 0x40) (R/W 32) FI DI Ratio Register -------- */
#define US_FIDI_Msk                           _U_(0x00000000)                                      /**< (US_FIDI) Register Mask  */

/* USART mode */
#define US_FIDI_USART_FI_DI_RATIO_Pos         _U_(0)                                               /**< (US_FIDI) FI Over DI Ratio Value Position */
#define US_FIDI_USART_FI_DI_RATIO_Msk         (_U_(0xFFFF) << US_FIDI_USART_FI_DI_RATIO_Pos)       /**< (US_FIDI) FI Over DI Ratio Value Mask */
#define US_FIDI_USART_FI_DI_RATIO(value)      (US_FIDI_USART_FI_DI_RATIO_Msk & ((value) << US_FIDI_USART_FI_DI_RATIO_Pos))
#define US_FIDI_USART_Msk                     _U_(0x0000FFFF)                                       /**< (US_FIDI_USART) Register Mask  */

/* LON mode */
#define US_FIDI_LON_BETA2_Pos                 _U_(0)                                               /**< (US_FIDI) LON BETA2 Length Position */
#define US_FIDI_LON_BETA2_Msk                 (_U_(0xFFFFFF) << US_FIDI_LON_BETA2_Pos)             /**< (US_FIDI) LON BETA2 Length Mask */
#define US_FIDI_LON_BETA2(value)              (US_FIDI_LON_BETA2_Msk & ((value) << US_FIDI_LON_BETA2_Pos))
#define US_FIDI_LON_Msk                       _U_(0x00FFFFFF)                                       /**< (US_FIDI_LON) Register Mask  */


/* -------- US_NER : (USART Offset: 0x44) ( R/ 32) Number of Errors Register -------- */
#define US_NER_NB_ERRORS_Pos                  _U_(0)                                               /**< (US_NER) Number of Errors Position */
#define US_NER_NB_ERRORS_Msk                  (_U_(0xFF) << US_NER_NB_ERRORS_Pos)                  /**< (US_NER) Number of Errors Mask */
#define US_NER_NB_ERRORS(value)               (US_NER_NB_ERRORS_Msk & ((value) << US_NER_NB_ERRORS_Pos))
#define US_NER_Msk                            _U_(0x000000FF)                                      /**< (US_NER) Register Mask  */


/* -------- US_IF : (USART Offset: 0x4C) (R/W 32) IrDA Filter Register -------- */
#define US_IF_IRDA_FILTER_Pos                 _U_(0)                                               /**< (US_IF) IrDA Filter Position */
#define US_IF_IRDA_FILTER_Msk                 (_U_(0xFF) << US_IF_IRDA_FILTER_Pos)                 /**< (US_IF) IrDA Filter Mask */
#define US_IF_IRDA_FILTER(value)              (US_IF_IRDA_FILTER_Msk & ((value) << US_IF_IRDA_FILTER_Pos))
#define US_IF_Msk                             _U_(0x000000FF)                                      /**< (US_IF) Register Mask  */


/* -------- US_MAN : (USART Offset: 0x50) (R/W 32) Manchester Configuration Register -------- */
#define US_MAN_TX_PL_Pos                      _U_(0)                                               /**< (US_MAN) Transmitter Preamble Length Position */
#define US_MAN_TX_PL_Msk                      (_U_(0xF) << US_MAN_TX_PL_Pos)                       /**< (US_MAN) Transmitter Preamble Length Mask */
#define US_MAN_TX_PL(value)                   (US_MAN_TX_PL_Msk & ((value) << US_MAN_TX_PL_Pos))  
#define US_MAN_TX_PP_Pos                      _U_(8)                                               /**< (US_MAN) Transmitter Preamble Pattern Position */
#define US_MAN_TX_PP_Msk                      (_U_(0x3) << US_MAN_TX_PP_Pos)                       /**< (US_MAN) Transmitter Preamble Pattern Mask */
#define US_MAN_TX_PP(value)                   (US_MAN_TX_PP_Msk & ((value) << US_MAN_TX_PP_Pos))  
#define   US_MAN_TX_PP_ALL_ONE_Val            _U_(0x0)                                             /**< (US_MAN) The preamble is composed of '1's  */
#define   US_MAN_TX_PP_ALL_ZERO_Val           _U_(0x1)                                             /**< (US_MAN) The preamble is composed of '0's  */
#define   US_MAN_TX_PP_ZERO_ONE_Val           _U_(0x2)                                             /**< (US_MAN) The preamble is composed of '01's  */
#define   US_MAN_TX_PP_ONE_ZERO_Val           _U_(0x3)                                             /**< (US_MAN) The preamble is composed of '10's  */
#define US_MAN_TX_PP_ALL_ONE                  (US_MAN_TX_PP_ALL_ONE_Val << US_MAN_TX_PP_Pos)       /**< (US_MAN) The preamble is composed of '1's Position  */
#define US_MAN_TX_PP_ALL_ZERO                 (US_MAN_TX_PP_ALL_ZERO_Val << US_MAN_TX_PP_Pos)      /**< (US_MAN) The preamble is composed of '0's Position  */
#define US_MAN_TX_PP_ZERO_ONE                 (US_MAN_TX_PP_ZERO_ONE_Val << US_MAN_TX_PP_Pos)      /**< (US_MAN) The preamble is composed of '01's Position  */
#define US_MAN_TX_PP_ONE_ZERO                 (US_MAN_TX_PP_ONE_ZERO_Val << US_MAN_TX_PP_Pos)      /**< (US_MAN) The preamble is composed of '10's Position  */
#define US_MAN_TX_MPOL_Pos                    _U_(12)                                              /**< (US_MAN) Transmitter Manchester Polarity Position */
#define US_MAN_TX_MPOL_Msk                    (_U_(0x1) << US_MAN_TX_MPOL_Pos)                     /**< (US_MAN) Transmitter Manchester Polarity Mask */
#define US_MAN_TX_MPOL(value)                 (US_MAN_TX_MPOL_Msk & ((value) << US_MAN_TX_MPOL_Pos))
#define US_MAN_RX_PL_Pos                      _U_(16)                                              /**< (US_MAN) Receiver Preamble Length Position */
#define US_MAN_RX_PL_Msk                      (_U_(0xF) << US_MAN_RX_PL_Pos)                       /**< (US_MAN) Receiver Preamble Length Mask */
#define US_MAN_RX_PL(value)                   (US_MAN_RX_PL_Msk & ((value) << US_MAN_RX_PL_Pos))  
#define US_MAN_RX_PP_Pos                      _U_(24)                                              /**< (US_MAN) Receiver Preamble Pattern detected Position */
#define US_MAN_RX_PP_Msk                      (_U_(0x3) << US_MAN_RX_PP_Pos)                       /**< (US_MAN) Receiver Preamble Pattern detected Mask */
#define US_MAN_RX_PP(value)                   (US_MAN_RX_PP_Msk & ((value) << US_MAN_RX_PP_Pos))  
#define   US_MAN_RX_PP_ALL_ONE_Val            _U_(0x0)                                             /**< (US_MAN) The preamble is composed of '1's  */
#define   US_MAN_RX_PP_ALL_ZERO_Val           _U_(0x1)                                             /**< (US_MAN) The preamble is composed of '0's  */
#define   US_MAN_RX_PP_ZERO_ONE_Val           _U_(0x2)                                             /**< (US_MAN) The preamble is composed of '01's  */
#define   US_MAN_RX_PP_ONE_ZERO_Val           _U_(0x3)                                             /**< (US_MAN) The preamble is composed of '10's  */
#define US_MAN_RX_PP_ALL_ONE                  (US_MAN_RX_PP_ALL_ONE_Val << US_MAN_RX_PP_Pos)       /**< (US_MAN) The preamble is composed of '1's Position  */
#define US_MAN_RX_PP_ALL_ZERO                 (US_MAN_RX_PP_ALL_ZERO_Val << US_MAN_RX_PP_Pos)      /**< (US_MAN) The preamble is composed of '0's Position  */
#define US_MAN_RX_PP_ZERO_ONE                 (US_MAN_RX_PP_ZERO_ONE_Val << US_MAN_RX_PP_Pos)      /**< (US_MAN) The preamble is composed of '01's Position  */
#define US_MAN_RX_PP_ONE_ZERO                 (US_MAN_RX_PP_ONE_ZERO_Val << US_MAN_RX_PP_Pos)      /**< (US_MAN) The preamble is composed of '10's Position  */
#define US_MAN_RX_MPOL_Pos                    _U_(28)                                              /**< (US_MAN) Receiver Manchester Polarity Position */
#define US_MAN_RX_MPOL_Msk                    (_U_(0x1) << US_MAN_RX_MPOL_Pos)                     /**< (US_MAN) Receiver Manchester Polarity Mask */
#define US_MAN_RX_MPOL(value)                 (US_MAN_RX_MPOL_Msk & ((value) << US_MAN_RX_MPOL_Pos))
#define US_MAN_ONE_Pos                        _U_(29)                                              /**< (US_MAN) Must Be Set to 1 Position */
#define US_MAN_ONE_Msk                        (_U_(0x1) << US_MAN_ONE_Pos)                         /**< (US_MAN) Must Be Set to 1 Mask */
#define US_MAN_ONE(value)                     (US_MAN_ONE_Msk & ((value) << US_MAN_ONE_Pos))      
#define US_MAN_DRIFT_Pos                      _U_(30)                                              /**< (US_MAN) Drift Compensation Position */
#define US_MAN_DRIFT_Msk                      (_U_(0x1) << US_MAN_DRIFT_Pos)                       /**< (US_MAN) Drift Compensation Mask */
#define US_MAN_DRIFT(value)                   (US_MAN_DRIFT_Msk & ((value) << US_MAN_DRIFT_Pos))  
#define US_MAN_RXIDLEV_Pos                    _U_(31)                                              /**< (US_MAN) Receiver Idle Value Position */
#define US_MAN_RXIDLEV_Msk                    (_U_(0x1) << US_MAN_RXIDLEV_Pos)                     /**< (US_MAN) Receiver Idle Value Mask */
#define US_MAN_RXIDLEV(value)                 (US_MAN_RXIDLEV_Msk & ((value) << US_MAN_RXIDLEV_Pos))
#define US_MAN_Msk                            _U_(0xF30F130F)                                      /**< (US_MAN) Register Mask  */


/* -------- US_LINMR : (USART Offset: 0x54) (R/W 32) LIN Mode Register -------- */
#define US_LINMR_NACT_Pos                     _U_(0)                                               /**< (US_LINMR) LIN Node Action Position */
#define US_LINMR_NACT_Msk                     (_U_(0x3) << US_LINMR_NACT_Pos)                      /**< (US_LINMR) LIN Node Action Mask */
#define US_LINMR_NACT(value)                  (US_LINMR_NACT_Msk & ((value) << US_LINMR_NACT_Pos))
#define   US_LINMR_NACT_PUBLISH_Val           _U_(0x0)                                             /**< (US_LINMR) The USART transmits the response.  */
#define   US_LINMR_NACT_SUBSCRIBE_Val         _U_(0x1)                                             /**< (US_LINMR) The USART receives the response.  */
#define   US_LINMR_NACT_IGNORE_Val            _U_(0x2)                                             /**< (US_LINMR) The USART does not transmit and does not receive the response.  */
#define US_LINMR_NACT_PUBLISH                 (US_LINMR_NACT_PUBLISH_Val << US_LINMR_NACT_Pos)     /**< (US_LINMR) The USART transmits the response. Position  */
#define US_LINMR_NACT_SUBSCRIBE               (US_LINMR_NACT_SUBSCRIBE_Val << US_LINMR_NACT_Pos)   /**< (US_LINMR) The USART receives the response. Position  */
#define US_LINMR_NACT_IGNORE                  (US_LINMR_NACT_IGNORE_Val << US_LINMR_NACT_Pos)      /**< (US_LINMR) The USART does not transmit and does not receive the response. Position  */
#define US_LINMR_PARDIS_Pos                   _U_(2)                                               /**< (US_LINMR) Parity Disable Position */
#define US_LINMR_PARDIS_Msk                   (_U_(0x1) << US_LINMR_PARDIS_Pos)                    /**< (US_LINMR) Parity Disable Mask */
#define US_LINMR_PARDIS(value)                (US_LINMR_PARDIS_Msk & ((value) << US_LINMR_PARDIS_Pos))
#define US_LINMR_CHKDIS_Pos                   _U_(3)                                               /**< (US_LINMR) Checksum Disable Position */
#define US_LINMR_CHKDIS_Msk                   (_U_(0x1) << US_LINMR_CHKDIS_Pos)                    /**< (US_LINMR) Checksum Disable Mask */
#define US_LINMR_CHKDIS(value)                (US_LINMR_CHKDIS_Msk & ((value) << US_LINMR_CHKDIS_Pos))
#define US_LINMR_CHKTYP_Pos                   _U_(4)                                               /**< (US_LINMR) Checksum Type Position */
#define US_LINMR_CHKTYP_Msk                   (_U_(0x1) << US_LINMR_CHKTYP_Pos)                    /**< (US_LINMR) Checksum Type Mask */
#define US_LINMR_CHKTYP(value)                (US_LINMR_CHKTYP_Msk & ((value) << US_LINMR_CHKTYP_Pos))
#define US_LINMR_DLM_Pos                      _U_(5)                                               /**< (US_LINMR) Data Length Mode Position */
#define US_LINMR_DLM_Msk                      (_U_(0x1) << US_LINMR_DLM_Pos)                       /**< (US_LINMR) Data Length Mode Mask */
#define US_LINMR_DLM(value)                   (US_LINMR_DLM_Msk & ((value) << US_LINMR_DLM_Pos))  
#define US_LINMR_FSDIS_Pos                    _U_(6)                                               /**< (US_LINMR) Frame Slot Mode Disable Position */
#define US_LINMR_FSDIS_Msk                    (_U_(0x1) << US_LINMR_FSDIS_Pos)                     /**< (US_LINMR) Frame Slot Mode Disable Mask */
#define US_LINMR_FSDIS(value)                 (US_LINMR_FSDIS_Msk & ((value) << US_LINMR_FSDIS_Pos))
#define US_LINMR_WKUPTYP_Pos                  _U_(7)                                               /**< (US_LINMR) Wakeup Signal Type Position */
#define US_LINMR_WKUPTYP_Msk                  (_U_(0x1) << US_LINMR_WKUPTYP_Pos)                   /**< (US_LINMR) Wakeup Signal Type Mask */
#define US_LINMR_WKUPTYP(value)               (US_LINMR_WKUPTYP_Msk & ((value) << US_LINMR_WKUPTYP_Pos))
#define US_LINMR_DLC_Pos                      _U_(8)                                               /**< (US_LINMR) Data Length Control Position */
#define US_LINMR_DLC_Msk                      (_U_(0xFF) << US_LINMR_DLC_Pos)                      /**< (US_LINMR) Data Length Control Mask */
#define US_LINMR_DLC(value)                   (US_LINMR_DLC_Msk & ((value) << US_LINMR_DLC_Pos))  
#define US_LINMR_PDCM_Pos                     _U_(16)                                              /**< (US_LINMR) DMAC Mode Position */
#define US_LINMR_PDCM_Msk                     (_U_(0x1) << US_LINMR_PDCM_Pos)                      /**< (US_LINMR) DMAC Mode Mask */
#define US_LINMR_PDCM(value)                  (US_LINMR_PDCM_Msk & ((value) << US_LINMR_PDCM_Pos))
#define US_LINMR_SYNCDIS_Pos                  _U_(17)                                              /**< (US_LINMR) Synchronization Disable Position */
#define US_LINMR_SYNCDIS_Msk                  (_U_(0x1) << US_LINMR_SYNCDIS_Pos)                   /**< (US_LINMR) Synchronization Disable Mask */
#define US_LINMR_SYNCDIS(value)               (US_LINMR_SYNCDIS_Msk & ((value) << US_LINMR_SYNCDIS_Pos))
#define US_LINMR_Msk                          _U_(0x0003FFFF)                                      /**< (US_LINMR) Register Mask  */


/* -------- US_LINIR : (USART Offset: 0x58) (R/W 32) LIN Identifier Register -------- */
#define US_LINIR_IDCHR_Pos                    _U_(0)                                               /**< (US_LINIR) Identifier Character Position */
#define US_LINIR_IDCHR_Msk                    (_U_(0xFF) << US_LINIR_IDCHR_Pos)                    /**< (US_LINIR) Identifier Character Mask */
#define US_LINIR_IDCHR(value)                 (US_LINIR_IDCHR_Msk & ((value) << US_LINIR_IDCHR_Pos))
#define US_LINIR_Msk                          _U_(0x000000FF)                                      /**< (US_LINIR) Register Mask  */


/* -------- US_LINBRR : (USART Offset: 0x5C) ( R/ 32) LIN Baud Rate Register -------- */
#define US_LINBRR_LINCD_Pos                   _U_(0)                                               /**< (US_LINBRR) Clock Divider after Synchronization Position */
#define US_LINBRR_LINCD_Msk                   (_U_(0xFFFF) << US_LINBRR_LINCD_Pos)                 /**< (US_LINBRR) Clock Divider after Synchronization Mask */
#define US_LINBRR_LINCD(value)                (US_LINBRR_LINCD_Msk & ((value) << US_LINBRR_LINCD_Pos))
#define US_LINBRR_LINFP_Pos                   _U_(16)                                              /**< (US_LINBRR) Fractional Part after Synchronization Position */
#define US_LINBRR_LINFP_Msk                   (_U_(0x7) << US_LINBRR_LINFP_Pos)                    /**< (US_LINBRR) Fractional Part after Synchronization Mask */
#define US_LINBRR_LINFP(value)                (US_LINBRR_LINFP_Msk & ((value) << US_LINBRR_LINFP_Pos))
#define US_LINBRR_Msk                         _U_(0x0007FFFF)                                      /**< (US_LINBRR) Register Mask  */


/* -------- US_LONMR : (USART Offset: 0x60) (R/W 32) LON Mode Register -------- */
#define US_LONMR_COMMT_Pos                    _U_(0)                                               /**< (US_LONMR) LON comm_type Parameter Value Position */
#define US_LONMR_COMMT_Msk                    (_U_(0x1) << US_LONMR_COMMT_Pos)                     /**< (US_LONMR) LON comm_type Parameter Value Mask */
#define US_LONMR_COMMT(value)                 (US_LONMR_COMMT_Msk & ((value) << US_LONMR_COMMT_Pos))
#define US_LONMR_COLDET_Pos                   _U_(1)                                               /**< (US_LONMR) LON Collision Detection Feature Position */
#define US_LONMR_COLDET_Msk                   (_U_(0x1) << US_LONMR_COLDET_Pos)                    /**< (US_LONMR) LON Collision Detection Feature Mask */
#define US_LONMR_COLDET(value)                (US_LONMR_COLDET_Msk & ((value) << US_LONMR_COLDET_Pos))
#define US_LONMR_TCOL_Pos                     _U_(2)                                               /**< (US_LONMR) Terminate Frame upon Collision Notification Position */
#define US_LONMR_TCOL_Msk                     (_U_(0x1) << US_LONMR_TCOL_Pos)                      /**< (US_LONMR) Terminate Frame upon Collision Notification Mask */
#define US_LONMR_TCOL(value)                  (US_LONMR_TCOL_Msk & ((value) << US_LONMR_TCOL_Pos))
#define US_LONMR_CDTAIL_Pos                   _U_(3)                                               /**< (US_LONMR) LON Collision Detection on Frame Tail Position */
#define US_LONMR_CDTAIL_Msk                   (_U_(0x1) << US_LONMR_CDTAIL_Pos)                    /**< (US_LONMR) LON Collision Detection on Frame Tail Mask */
#define US_LONMR_CDTAIL(value)                (US_LONMR_CDTAIL_Msk & ((value) << US_LONMR_CDTAIL_Pos))
#define US_LONMR_DMAM_Pos                     _U_(4)                                               /**< (US_LONMR) LON DMA Mode Position */
#define US_LONMR_DMAM_Msk                     (_U_(0x1) << US_LONMR_DMAM_Pos)                      /**< (US_LONMR) LON DMA Mode Mask */
#define US_LONMR_DMAM(value)                  (US_LONMR_DMAM_Msk & ((value) << US_LONMR_DMAM_Pos))
#define US_LONMR_LCDS_Pos                     _U_(5)                                               /**< (US_LONMR) LON Collision Detection Source Position */
#define US_LONMR_LCDS_Msk                     (_U_(0x1) << US_LONMR_LCDS_Pos)                      /**< (US_LONMR) LON Collision Detection Source Mask */
#define US_LONMR_LCDS(value)                  (US_LONMR_LCDS_Msk & ((value) << US_LONMR_LCDS_Pos))
#define US_LONMR_EOFS_Pos                     _U_(16)                                              /**< (US_LONMR) End of Frame Condition Size Position */
#define US_LONMR_EOFS_Msk                     (_U_(0xFF) << US_LONMR_EOFS_Pos)                     /**< (US_LONMR) End of Frame Condition Size Mask */
#define US_LONMR_EOFS(value)                  (US_LONMR_EOFS_Msk & ((value) << US_LONMR_EOFS_Pos))
#define US_LONMR_Msk                          _U_(0x00FF003F)                                      /**< (US_LONMR) Register Mask  */


/* -------- US_LONPR : (USART Offset: 0x64) (R/W 32) LON Preamble Register -------- */
#define US_LONPR_LONPL_Pos                    _U_(0)                                               /**< (US_LONPR) LON Preamble Length Position */
#define US_LONPR_LONPL_Msk                    (_U_(0x3FFF) << US_LONPR_LONPL_Pos)                  /**< (US_LONPR) LON Preamble Length Mask */
#define US_LONPR_LONPL(value)                 (US_LONPR_LONPL_Msk & ((value) << US_LONPR_LONPL_Pos))
#define US_LONPR_Msk                          _U_(0x00003FFF)                                      /**< (US_LONPR) Register Mask  */


/* -------- US_LONDL : (USART Offset: 0x68) (R/W 32) LON Data Length Register -------- */
#define US_LONDL_LONDL_Pos                    _U_(0)                                               /**< (US_LONDL) LON Data Length Position */
#define US_LONDL_LONDL_Msk                    (_U_(0xFF) << US_LONDL_LONDL_Pos)                    /**< (US_LONDL) LON Data Length Mask */
#define US_LONDL_LONDL(value)                 (US_LONDL_LONDL_Msk & ((value) << US_LONDL_LONDL_Pos))
#define US_LONDL_Msk                          _U_(0x000000FF)                                      /**< (US_LONDL) Register Mask  */


/* -------- US_LONL2HDR : (USART Offset: 0x6C) (R/W 32) LON L2HDR Register -------- */
#define US_LONL2HDR_BLI_Pos                   _U_(0)                                               /**< (US_LONL2HDR) LON Backlog Increment Position */
#define US_LONL2HDR_BLI_Msk                   (_U_(0x3F) << US_LONL2HDR_BLI_Pos)                   /**< (US_LONL2HDR) LON Backlog Increment Mask */
#define US_LONL2HDR_BLI(value)                (US_LONL2HDR_BLI_Msk & ((value) << US_LONL2HDR_BLI_Pos))
#define US_LONL2HDR_ALTP_Pos                  _U_(6)                                               /**< (US_LONL2HDR) LON Alternate Path Bit Position */
#define US_LONL2HDR_ALTP_Msk                  (_U_(0x1) << US_LONL2HDR_ALTP_Pos)                   /**< (US_LONL2HDR) LON Alternate Path Bit Mask */
#define US_LONL2HDR_ALTP(value)               (US_LONL2HDR_ALTP_Msk & ((value) << US_LONL2HDR_ALTP_Pos))
#define US_LONL2HDR_PB_Pos                    _U_(7)                                               /**< (US_LONL2HDR) LON Priority Bit Position */
#define US_LONL2HDR_PB_Msk                    (_U_(0x1) << US_LONL2HDR_PB_Pos)                     /**< (US_LONL2HDR) LON Priority Bit Mask */
#define US_LONL2HDR_PB(value)                 (US_LONL2HDR_PB_Msk & ((value) << US_LONL2HDR_PB_Pos))
#define US_LONL2HDR_Msk                       _U_(0x000000FF)                                      /**< (US_LONL2HDR) Register Mask  */


/* -------- US_LONBL : (USART Offset: 0x70) ( R/ 32) LON Backlog Register -------- */
#define US_LONBL_LONBL_Pos                    _U_(0)                                               /**< (US_LONBL) LON Node Backlog Value Position */
#define US_LONBL_LONBL_Msk                    (_U_(0x3F) << US_LONBL_LONBL_Pos)                    /**< (US_LONBL) LON Node Backlog Value Mask */
#define US_LONBL_LONBL(value)                 (US_LONBL_LONBL_Msk & ((value) << US_LONBL_LONBL_Pos))
#define US_LONBL_Msk                          _U_(0x0000003F)                                      /**< (US_LONBL) Register Mask  */


/* -------- US_LONB1TX : (USART Offset: 0x74) (R/W 32) LON Beta1 Tx Register -------- */
#define US_LONB1TX_BETA1TX_Pos                _U_(0)                                               /**< (US_LONB1TX) LON Beta1 Length after Transmission Position */
#define US_LONB1TX_BETA1TX_Msk                (_U_(0xFFFFFF) << US_LONB1TX_BETA1TX_Pos)            /**< (US_LONB1TX) LON Beta1 Length after Transmission Mask */
#define US_LONB1TX_BETA1TX(value)             (US_LONB1TX_BETA1TX_Msk & ((value) << US_LONB1TX_BETA1TX_Pos))
#define US_LONB1TX_Msk                        _U_(0x00FFFFFF)                                      /**< (US_LONB1TX) Register Mask  */


/* -------- US_LONB1RX : (USART Offset: 0x78) (R/W 32) LON Beta1 Rx Register -------- */
#define US_LONB1RX_BETA1RX_Pos                _U_(0)                                               /**< (US_LONB1RX) LON Beta1 Length after Reception Position */
#define US_LONB1RX_BETA1RX_Msk                (_U_(0xFFFFFF) << US_LONB1RX_BETA1RX_Pos)            /**< (US_LONB1RX) LON Beta1 Length after Reception Mask */
#define US_LONB1RX_BETA1RX(value)             (US_LONB1RX_BETA1RX_Msk & ((value) << US_LONB1RX_BETA1RX_Pos))
#define US_LONB1RX_Msk                        _U_(0x00FFFFFF)                                      /**< (US_LONB1RX) Register Mask  */


/* -------- US_LONPRIO : (USART Offset: 0x7C) (R/W 32) LON Priority Register -------- */
#define US_LONPRIO_PSNB_Pos                   _U_(0)                                               /**< (US_LONPRIO) LON Priority Slot Number Position */
#define US_LONPRIO_PSNB_Msk                   (_U_(0x7F) << US_LONPRIO_PSNB_Pos)                   /**< (US_LONPRIO) LON Priority Slot Number Mask */
#define US_LONPRIO_PSNB(value)                (US_LONPRIO_PSNB_Msk & ((value) << US_LONPRIO_PSNB_Pos))
#define US_LONPRIO_NPS_Pos                    _U_(8)                                               /**< (US_LONPRIO) LON Node Priority Slot Position */
#define US_LONPRIO_NPS_Msk                    (_U_(0x7F) << US_LONPRIO_NPS_Pos)                    /**< (US_LONPRIO) LON Node Priority Slot Mask */
#define US_LONPRIO_NPS(value)                 (US_LONPRIO_NPS_Msk & ((value) << US_LONPRIO_NPS_Pos))
#define US_LONPRIO_Msk                        _U_(0x00007F7F)                                      /**< (US_LONPRIO) Register Mask  */


/* -------- US_IDTTX : (USART Offset: 0x80) (R/W 32) LON IDT Tx Register -------- */
#define US_IDTTX_IDTTX_Pos                    _U_(0)                                               /**< (US_IDTTX) LON Indeterminate Time after Transmission (comm_type = 1 mode only) Position */
#define US_IDTTX_IDTTX_Msk                    (_U_(0xFFFFFF) << US_IDTTX_IDTTX_Pos)                /**< (US_IDTTX) LON Indeterminate Time after Transmission (comm_type = 1 mode only) Mask */
#define US_IDTTX_IDTTX(value)                 (US_IDTTX_IDTTX_Msk & ((value) << US_IDTTX_IDTTX_Pos))
#define US_IDTTX_Msk                          _U_(0x00FFFFFF)                                      /**< (US_IDTTX) Register Mask  */


/* -------- US_IDTRX : (USART Offset: 0x84) (R/W 32) LON IDT Rx Register -------- */
#define US_IDTRX_IDTRX_Pos                    _U_(0)                                               /**< (US_IDTRX) LON Indeterminate Time after Reception (comm_type = 1 mode only) Position */
#define US_IDTRX_IDTRX_Msk                    (_U_(0xFFFFFF) << US_IDTRX_IDTRX_Pos)                /**< (US_IDTRX) LON Indeterminate Time after Reception (comm_type = 1 mode only) Mask */
#define US_IDTRX_IDTRX(value)                 (US_IDTRX_IDTRX_Msk & ((value) << US_IDTRX_IDTRX_Pos))
#define US_IDTRX_Msk                          _U_(0x00FFFFFF)                                      /**< (US_IDTRX) Register Mask  */


/* -------- US_ICDIFF : (USART Offset: 0x88) (R/W 32) IC DIFF Register -------- */
#define US_ICDIFF_ICDIFF_Pos                  _U_(0)                                               /**< (US_ICDIFF) IC Differentiator Number Position */
#define US_ICDIFF_ICDIFF_Msk                  (_U_(0xF) << US_ICDIFF_ICDIFF_Pos)                   /**< (US_ICDIFF) IC Differentiator Number Mask */
#define US_ICDIFF_ICDIFF(value)               (US_ICDIFF_ICDIFF_Msk & ((value) << US_ICDIFF_ICDIFF_Pos))
#define US_ICDIFF_Msk                         _U_(0x0000000F)                                      /**< (US_ICDIFF) Register Mask  */


/* -------- US_WPMR : (USART Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define US_WPMR_WPEN_Pos                      _U_(0)                                               /**< (US_WPMR) Write Protection Enable Position */
#define US_WPMR_WPEN_Msk                      (_U_(0x1) << US_WPMR_WPEN_Pos)                       /**< (US_WPMR) Write Protection Enable Mask */
#define US_WPMR_WPEN(value)                   (US_WPMR_WPEN_Msk & ((value) << US_WPMR_WPEN_Pos))  
#define US_WPMR_WPKEY_Pos                     _U_(8)                                               /**< (US_WPMR) Write Protection Key Position */
#define US_WPMR_WPKEY_Msk                     (_U_(0xFFFFFF) << US_WPMR_WPKEY_Pos)                 /**< (US_WPMR) Write Protection Key Mask */
#define US_WPMR_WPKEY(value)                  (US_WPMR_WPKEY_Msk & ((value) << US_WPMR_WPKEY_Pos))
#define   US_WPMR_WPKEY_PASSWD_Val            _U_(0x555341)                                        /**< (US_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0.  */
#define US_WPMR_WPKEY_PASSWD                  (US_WPMR_WPKEY_PASSWD_Val << US_WPMR_WPKEY_Pos)      /**< (US_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0. Position  */
#define US_WPMR_Msk                           _U_(0xFFFFFF01)                                      /**< (US_WPMR) Register Mask  */


/* -------- US_WPSR : (USART Offset: 0xE8) ( R/ 32) Write Protection Status Register -------- */
#define US_WPSR_WPVS_Pos                      _U_(0)                                               /**< (US_WPSR) Write Protection Violation Status Position */
#define US_WPSR_WPVS_Msk                      (_U_(0x1) << US_WPSR_WPVS_Pos)                       /**< (US_WPSR) Write Protection Violation Status Mask */
#define US_WPSR_WPVS(value)                   (US_WPSR_WPVS_Msk & ((value) << US_WPSR_WPVS_Pos))  
#define US_WPSR_WPVSRC_Pos                    _U_(8)                                               /**< (US_WPSR) Write Protection Violation Source Position */
#define US_WPSR_WPVSRC_Msk                    (_U_(0xFFFF) << US_WPSR_WPVSRC_Pos)                  /**< (US_WPSR) Write Protection Violation Source Mask */
#define US_WPSR_WPVSRC(value)                 (US_WPSR_WPVSRC_Msk & ((value) << US_WPSR_WPVSRC_Pos))
#define US_WPSR_Msk                           _U_(0x00FFFF01)                                      /**< (US_WPSR) Register Mask  */


/** \brief USART register offsets definitions */
#define US_CR_REG_OFST                 (0x00)              /**< (US_CR) Control Register Offset */
#define US_MR_REG_OFST                 (0x04)              /**< (US_MR) Mode Register Offset */
#define US_IER_REG_OFST                (0x08)              /**< (US_IER) Interrupt Enable Register Offset */
#define US_IDR_REG_OFST                (0x0C)              /**< (US_IDR) Interrupt Disable Register Offset */
#define US_IMR_REG_OFST                (0x10)              /**< (US_IMR) Interrupt Mask Register Offset */
#define US_CSR_REG_OFST                (0x14)              /**< (US_CSR) Channel Status Register Offset */
#define US_RHR_REG_OFST                (0x18)              /**< (US_RHR) Receive Holding Register Offset */
#define US_THR_REG_OFST                (0x1C)              /**< (US_THR) Transmit Holding Register Offset */
#define US_BRGR_REG_OFST               (0x20)              /**< (US_BRGR) Baud Rate Generator Register Offset */
#define US_RTOR_REG_OFST               (0x24)              /**< (US_RTOR) Receiver Timeout Register Offset */
#define US_TTGR_REG_OFST               (0x28)              /**< (US_TTGR) Transmitter Timeguard Register Offset */
#define US_FIDI_REG_OFST               (0x40)              /**< (US_FIDI) FI DI Ratio Register Offset */
#define US_NER_REG_OFST                (0x44)              /**< (US_NER) Number of Errors Register Offset */
#define US_IF_REG_OFST                 (0x4C)              /**< (US_IF) IrDA Filter Register Offset */
#define US_MAN_REG_OFST                (0x50)              /**< (US_MAN) Manchester Configuration Register Offset */
#define US_LINMR_REG_OFST              (0x54)              /**< (US_LINMR) LIN Mode Register Offset */
#define US_LINIR_REG_OFST              (0x58)              /**< (US_LINIR) LIN Identifier Register Offset */
#define US_LINBRR_REG_OFST             (0x5C)              /**< (US_LINBRR) LIN Baud Rate Register Offset */
#define US_LONMR_REG_OFST              (0x60)              /**< (US_LONMR) LON Mode Register Offset */
#define US_LONPR_REG_OFST              (0x64)              /**< (US_LONPR) LON Preamble Register Offset */
#define US_LONDL_REG_OFST              (0x68)              /**< (US_LONDL) LON Data Length Register Offset */
#define US_LONL2HDR_REG_OFST           (0x6C)              /**< (US_LONL2HDR) LON L2HDR Register Offset */
#define US_LONBL_REG_OFST              (0x70)              /**< (US_LONBL) LON Backlog Register Offset */
#define US_LONB1TX_REG_OFST            (0x74)              /**< (US_LONB1TX) LON Beta1 Tx Register Offset */
#define US_LONB1RX_REG_OFST            (0x78)              /**< (US_LONB1RX) LON Beta1 Rx Register Offset */
#define US_LONPRIO_REG_OFST            (0x7C)              /**< (US_LONPRIO) LON Priority Register Offset */
#define US_IDTTX_REG_OFST              (0x80)              /**< (US_IDTTX) LON IDT Tx Register Offset */
#define US_IDTRX_REG_OFST              (0x84)              /**< (US_IDTRX) LON IDT Rx Register Offset */
#define US_ICDIFF_REG_OFST             (0x88)              /**< (US_ICDIFF) IC DIFF Register Offset */
#define US_WPMR_REG_OFST               (0xE4)              /**< (US_WPMR) Write Protection Mode Register Offset */
#define US_WPSR_REG_OFST               (0xE8)              /**< (US_WPSR) Write Protection Status Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief USART register API structure */
typedef struct
{
  __O   uint32_t                       US_CR;              /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       US_MR;              /**< Offset: 0x04 (R/W  32) Mode Register */
  __O   uint32_t                       US_IER;             /**< Offset: 0x08 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       US_IDR;             /**< Offset: 0x0C ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       US_IMR;             /**< Offset: 0x10 (R/   32) Interrupt Mask Register */
  __I   uint32_t                       US_CSR;             /**< Offset: 0x14 (R/   32) Channel Status Register */
  __I   uint32_t                       US_RHR;             /**< Offset: 0x18 (R/   32) Receive Holding Register */
  __O   uint32_t                       US_THR;             /**< Offset: 0x1C ( /W  32) Transmit Holding Register */
  __IO  uint32_t                       US_BRGR;            /**< Offset: 0x20 (R/W  32) Baud Rate Generator Register */
  __IO  uint32_t                       US_RTOR;            /**< Offset: 0x24 (R/W  32) Receiver Timeout Register */
  __IO  uint32_t                       US_TTGR;            /**< Offset: 0x28 (R/W  32) Transmitter Timeguard Register */
  __I   uint8_t                        Reserved1[0x14];
  __IO  uint32_t                       US_FIDI;            /**< Offset: 0x40 (R/W  32) FI DI Ratio Register */
  __I   uint32_t                       US_NER;             /**< Offset: 0x44 (R/   32) Number of Errors Register */
  __I   uint8_t                        Reserved2[0x04];
  __IO  uint32_t                       US_IF;              /**< Offset: 0x4C (R/W  32) IrDA Filter Register */
  __IO  uint32_t                       US_MAN;             /**< Offset: 0x50 (R/W  32) Manchester Configuration Register */
  __IO  uint32_t                       US_LINMR;           /**< Offset: 0x54 (R/W  32) LIN Mode Register */
  __IO  uint32_t                       US_LINIR;           /**< Offset: 0x58 (R/W  32) LIN Identifier Register */
  __I   uint32_t                       US_LINBRR;          /**< Offset: 0x5C (R/   32) LIN Baud Rate Register */
  __IO  uint32_t                       US_LONMR;           /**< Offset: 0x60 (R/W  32) LON Mode Register */
  __IO  uint32_t                       US_LONPR;           /**< Offset: 0x64 (R/W  32) LON Preamble Register */
  __IO  uint32_t                       US_LONDL;           /**< Offset: 0x68 (R/W  32) LON Data Length Register */
  __IO  uint32_t                       US_LONL2HDR;        /**< Offset: 0x6C (R/W  32) LON L2HDR Register */
  __I   uint32_t                       US_LONBL;           /**< Offset: 0x70 (R/   32) LON Backlog Register */
  __IO  uint32_t                       US_LONB1TX;         /**< Offset: 0x74 (R/W  32) LON Beta1 Tx Register */
  __IO  uint32_t                       US_LONB1RX;         /**< Offset: 0x78 (R/W  32) LON Beta1 Rx Register */
  __IO  uint32_t                       US_LONPRIO;         /**< Offset: 0x7C (R/W  32) LON Priority Register */
  __IO  uint32_t                       US_IDTTX;           /**< Offset: 0x80 (R/W  32) LON IDT Tx Register */
  __IO  uint32_t                       US_IDTRX;           /**< Offset: 0x84 (R/W  32) LON IDT Rx Register */
  __IO  uint32_t                       US_ICDIFF;          /**< Offset: 0x88 (R/W  32) IC DIFF Register */
  __I   uint8_t                        Reserved3[0x58];
  __IO  uint32_t                       US_WPMR;            /**< Offset: 0xE4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       US_WPSR;            /**< Offset: 0xE8 (R/   32) Write Protection Status Register */
} usart_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_USART_COMPONENT_H_ */
