/**
 * \brief Component description for DMAC
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
#ifndef _SAME54_DMAC_COMPONENT_H_
#define _SAME54_DMAC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR DMAC                                         */
/* ************************************************************************** */

/* -------- DMAC_BTCTRL : (DMAC Offset: 0x00) (R/W 16) Block Transfer Control -------- */
#define DMAC_BTCTRL_RESETVALUE                _U_(0x00)                                            /**<  (DMAC_BTCTRL) Block Transfer Control  Reset Value */

#define DMAC_BTCTRL_VALID_Pos                 _U_(0)                                               /**< (DMAC_BTCTRL) Descriptor Valid Position */
#define DMAC_BTCTRL_VALID_Msk                 (_U_(0x1) << DMAC_BTCTRL_VALID_Pos)                  /**< (DMAC_BTCTRL) Descriptor Valid Mask */
#define DMAC_BTCTRL_VALID(value)              (DMAC_BTCTRL_VALID_Msk & ((value) << DMAC_BTCTRL_VALID_Pos))
#define DMAC_BTCTRL_EVOSEL_Pos                _U_(1)                                               /**< (DMAC_BTCTRL) Block Event Output Selection Position */
#define DMAC_BTCTRL_EVOSEL_Msk                (_U_(0x3) << DMAC_BTCTRL_EVOSEL_Pos)                 /**< (DMAC_BTCTRL) Block Event Output Selection Mask */
#define DMAC_BTCTRL_EVOSEL(value)             (DMAC_BTCTRL_EVOSEL_Msk & ((value) << DMAC_BTCTRL_EVOSEL_Pos))
#define   DMAC_BTCTRL_EVOSEL_DISABLE_Val      _U_(0x0)                                             /**< (DMAC_BTCTRL) Event generation disabled  */
#define   DMAC_BTCTRL_EVOSEL_BLOCK_Val        _U_(0x1)                                             /**< (DMAC_BTCTRL) Block event strobe  */
#define   DMAC_BTCTRL_EVOSEL_BURST_Val        _U_(0x3)                                             /**< (DMAC_BTCTRL) Burst event strobe  */
#define DMAC_BTCTRL_EVOSEL_DISABLE            (DMAC_BTCTRL_EVOSEL_DISABLE_Val << DMAC_BTCTRL_EVOSEL_Pos) /**< (DMAC_BTCTRL) Event generation disabled Position  */
#define DMAC_BTCTRL_EVOSEL_BLOCK              (DMAC_BTCTRL_EVOSEL_BLOCK_Val << DMAC_BTCTRL_EVOSEL_Pos) /**< (DMAC_BTCTRL) Block event strobe Position  */
#define DMAC_BTCTRL_EVOSEL_BURST              (DMAC_BTCTRL_EVOSEL_BURST_Val << DMAC_BTCTRL_EVOSEL_Pos) /**< (DMAC_BTCTRL) Burst event strobe Position  */
#define DMAC_BTCTRL_BLOCKACT_Pos              _U_(3)                                               /**< (DMAC_BTCTRL) Block Action Position */
#define DMAC_BTCTRL_BLOCKACT_Msk              (_U_(0x3) << DMAC_BTCTRL_BLOCKACT_Pos)               /**< (DMAC_BTCTRL) Block Action Mask */
#define DMAC_BTCTRL_BLOCKACT(value)           (DMAC_BTCTRL_BLOCKACT_Msk & ((value) << DMAC_BTCTRL_BLOCKACT_Pos))
#define   DMAC_BTCTRL_BLOCKACT_NOACT_Val      _U_(0x0)                                             /**< (DMAC_BTCTRL) Channel will be disabled if it is the last block transfer in the transaction  */
#define   DMAC_BTCTRL_BLOCKACT_INT_Val        _U_(0x1)                                             /**< (DMAC_BTCTRL) Channel will be disabled if it is the last block transfer in the transaction and block interrupt  */
#define   DMAC_BTCTRL_BLOCKACT_SUSPEND_Val    _U_(0x2)                                             /**< (DMAC_BTCTRL) Channel suspend operation is completed  */
#define   DMAC_BTCTRL_BLOCKACT_BOTH_Val       _U_(0x3)                                             /**< (DMAC_BTCTRL) Both channel suspend operation and block interrupt  */
#define DMAC_BTCTRL_BLOCKACT_NOACT            (DMAC_BTCTRL_BLOCKACT_NOACT_Val << DMAC_BTCTRL_BLOCKACT_Pos) /**< (DMAC_BTCTRL) Channel will be disabled if it is the last block transfer in the transaction Position  */
#define DMAC_BTCTRL_BLOCKACT_INT              (DMAC_BTCTRL_BLOCKACT_INT_Val << DMAC_BTCTRL_BLOCKACT_Pos) /**< (DMAC_BTCTRL) Channel will be disabled if it is the last block transfer in the transaction and block interrupt Position  */
#define DMAC_BTCTRL_BLOCKACT_SUSPEND          (DMAC_BTCTRL_BLOCKACT_SUSPEND_Val << DMAC_BTCTRL_BLOCKACT_Pos) /**< (DMAC_BTCTRL) Channel suspend operation is completed Position  */
#define DMAC_BTCTRL_BLOCKACT_BOTH             (DMAC_BTCTRL_BLOCKACT_BOTH_Val << DMAC_BTCTRL_BLOCKACT_Pos) /**< (DMAC_BTCTRL) Both channel suspend operation and block interrupt Position  */
#define DMAC_BTCTRL_BEATSIZE_Pos              _U_(8)                                               /**< (DMAC_BTCTRL) Beat Size Position */
#define DMAC_BTCTRL_BEATSIZE_Msk              (_U_(0x3) << DMAC_BTCTRL_BEATSIZE_Pos)               /**< (DMAC_BTCTRL) Beat Size Mask */
#define DMAC_BTCTRL_BEATSIZE(value)           (DMAC_BTCTRL_BEATSIZE_Msk & ((value) << DMAC_BTCTRL_BEATSIZE_Pos))
#define   DMAC_BTCTRL_BEATSIZE_BYTE_Val       _U_(0x0)                                             /**< (DMAC_BTCTRL) 8-bit bus transfer  */
#define   DMAC_BTCTRL_BEATSIZE_HWORD_Val      _U_(0x1)                                             /**< (DMAC_BTCTRL) 16-bit bus transfer  */
#define   DMAC_BTCTRL_BEATSIZE_WORD_Val       _U_(0x2)                                             /**< (DMAC_BTCTRL) 32-bit bus transfer  */
#define DMAC_BTCTRL_BEATSIZE_BYTE             (DMAC_BTCTRL_BEATSIZE_BYTE_Val << DMAC_BTCTRL_BEATSIZE_Pos) /**< (DMAC_BTCTRL) 8-bit bus transfer Position  */
#define DMAC_BTCTRL_BEATSIZE_HWORD            (DMAC_BTCTRL_BEATSIZE_HWORD_Val << DMAC_BTCTRL_BEATSIZE_Pos) /**< (DMAC_BTCTRL) 16-bit bus transfer Position  */
#define DMAC_BTCTRL_BEATSIZE_WORD             (DMAC_BTCTRL_BEATSIZE_WORD_Val << DMAC_BTCTRL_BEATSIZE_Pos) /**< (DMAC_BTCTRL) 32-bit bus transfer Position  */
#define DMAC_BTCTRL_SRCINC_Pos                _U_(10)                                              /**< (DMAC_BTCTRL) Source Address Increment Enable Position */
#define DMAC_BTCTRL_SRCINC_Msk                (_U_(0x1) << DMAC_BTCTRL_SRCINC_Pos)                 /**< (DMAC_BTCTRL) Source Address Increment Enable Mask */
#define DMAC_BTCTRL_SRCINC(value)             (DMAC_BTCTRL_SRCINC_Msk & ((value) << DMAC_BTCTRL_SRCINC_Pos))
#define DMAC_BTCTRL_DSTINC_Pos                _U_(11)                                              /**< (DMAC_BTCTRL) Destination Address Increment Enable Position */
#define DMAC_BTCTRL_DSTINC_Msk                (_U_(0x1) << DMAC_BTCTRL_DSTINC_Pos)                 /**< (DMAC_BTCTRL) Destination Address Increment Enable Mask */
#define DMAC_BTCTRL_DSTINC(value)             (DMAC_BTCTRL_DSTINC_Msk & ((value) << DMAC_BTCTRL_DSTINC_Pos))
#define DMAC_BTCTRL_STEPSEL_Pos               _U_(12)                                              /**< (DMAC_BTCTRL) Step Selection Position */
#define DMAC_BTCTRL_STEPSEL_Msk               (_U_(0x1) << DMAC_BTCTRL_STEPSEL_Pos)                /**< (DMAC_BTCTRL) Step Selection Mask */
#define DMAC_BTCTRL_STEPSEL(value)            (DMAC_BTCTRL_STEPSEL_Msk & ((value) << DMAC_BTCTRL_STEPSEL_Pos))
#define   DMAC_BTCTRL_STEPSEL_DST_Val         _U_(0x0)                                             /**< (DMAC_BTCTRL) Step size settings apply to the destination address  */
#define   DMAC_BTCTRL_STEPSEL_SRC_Val         _U_(0x1)                                             /**< (DMAC_BTCTRL) Step size settings apply to the source address  */
#define DMAC_BTCTRL_STEPSEL_DST               (DMAC_BTCTRL_STEPSEL_DST_Val << DMAC_BTCTRL_STEPSEL_Pos) /**< (DMAC_BTCTRL) Step size settings apply to the destination address Position  */
#define DMAC_BTCTRL_STEPSEL_SRC               (DMAC_BTCTRL_STEPSEL_SRC_Val << DMAC_BTCTRL_STEPSEL_Pos) /**< (DMAC_BTCTRL) Step size settings apply to the source address Position  */
#define DMAC_BTCTRL_STEPSIZE_Pos              _U_(13)                                              /**< (DMAC_BTCTRL) Address Increment Step Size Position */
#define DMAC_BTCTRL_STEPSIZE_Msk              (_U_(0x7) << DMAC_BTCTRL_STEPSIZE_Pos)               /**< (DMAC_BTCTRL) Address Increment Step Size Mask */
#define DMAC_BTCTRL_STEPSIZE(value)           (DMAC_BTCTRL_STEPSIZE_Msk & ((value) << DMAC_BTCTRL_STEPSIZE_Pos))
#define   DMAC_BTCTRL_STEPSIZE_X1_Val         _U_(0x0)                                             /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 1  */
#define   DMAC_BTCTRL_STEPSIZE_X2_Val         _U_(0x1)                                             /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 2  */
#define   DMAC_BTCTRL_STEPSIZE_X4_Val         _U_(0x2)                                             /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 4  */
#define   DMAC_BTCTRL_STEPSIZE_X8_Val         _U_(0x3)                                             /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 8  */
#define   DMAC_BTCTRL_STEPSIZE_X16_Val        _U_(0x4)                                             /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 16  */
#define   DMAC_BTCTRL_STEPSIZE_X32_Val        _U_(0x5)                                             /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 32  */
#define   DMAC_BTCTRL_STEPSIZE_X64_Val        _U_(0x6)                                             /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 64  */
#define   DMAC_BTCTRL_STEPSIZE_X128_Val       _U_(0x7)                                             /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 128  */
#define DMAC_BTCTRL_STEPSIZE_X1               (DMAC_BTCTRL_STEPSIZE_X1_Val << DMAC_BTCTRL_STEPSIZE_Pos) /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 1 Position  */
#define DMAC_BTCTRL_STEPSIZE_X2               (DMAC_BTCTRL_STEPSIZE_X2_Val << DMAC_BTCTRL_STEPSIZE_Pos) /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 2 Position  */
#define DMAC_BTCTRL_STEPSIZE_X4               (DMAC_BTCTRL_STEPSIZE_X4_Val << DMAC_BTCTRL_STEPSIZE_Pos) /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 4 Position  */
#define DMAC_BTCTRL_STEPSIZE_X8               (DMAC_BTCTRL_STEPSIZE_X8_Val << DMAC_BTCTRL_STEPSIZE_Pos) /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 8 Position  */
#define DMAC_BTCTRL_STEPSIZE_X16              (DMAC_BTCTRL_STEPSIZE_X16_Val << DMAC_BTCTRL_STEPSIZE_Pos) /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 16 Position  */
#define DMAC_BTCTRL_STEPSIZE_X32              (DMAC_BTCTRL_STEPSIZE_X32_Val << DMAC_BTCTRL_STEPSIZE_Pos) /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 32 Position  */
#define DMAC_BTCTRL_STEPSIZE_X64              (DMAC_BTCTRL_STEPSIZE_X64_Val << DMAC_BTCTRL_STEPSIZE_Pos) /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 64 Position  */
#define DMAC_BTCTRL_STEPSIZE_X128             (DMAC_BTCTRL_STEPSIZE_X128_Val << DMAC_BTCTRL_STEPSIZE_Pos) /**< (DMAC_BTCTRL) Next ADDR = ADDR + (1<<BEATSIZE) * 128 Position  */
#define DMAC_BTCTRL_Msk                       _U_(0xFF1F)                                          /**< (DMAC_BTCTRL) Register Mask  */


/* -------- DMAC_BTCNT : (DMAC Offset: 0x02) (R/W 16) Block Transfer Count -------- */
#define DMAC_BTCNT_RESETVALUE                 _U_(0x00)                                            /**<  (DMAC_BTCNT) Block Transfer Count  Reset Value */

#define DMAC_BTCNT_BTCNT_Pos                  _U_(0)                                               /**< (DMAC_BTCNT) Block Transfer Count Position */
#define DMAC_BTCNT_BTCNT_Msk                  (_U_(0xFFFF) << DMAC_BTCNT_BTCNT_Pos)                /**< (DMAC_BTCNT) Block Transfer Count Mask */
#define DMAC_BTCNT_BTCNT(value)               (DMAC_BTCNT_BTCNT_Msk & ((value) << DMAC_BTCNT_BTCNT_Pos))
#define DMAC_BTCNT_Msk                        _U_(0xFFFF)                                          /**< (DMAC_BTCNT) Register Mask  */


/* -------- DMAC_SRCADDR : (DMAC Offset: 0x04) (R/W 32) Block Transfer Source Address -------- */
#define DMAC_SRCADDR_RESETVALUE               _U_(0x00)                                            /**<  (DMAC_SRCADDR) Block Transfer Source Address  Reset Value */

#define DMAC_SRCADDR_SRCADDR_Pos              _U_(0)                                               /**< (DMAC_SRCADDR) Transfer Source Address Position */
#define DMAC_SRCADDR_SRCADDR_Msk              (_U_(0xFFFFFFFF) << DMAC_SRCADDR_SRCADDR_Pos)        /**< (DMAC_SRCADDR) Transfer Source Address Mask */
#define DMAC_SRCADDR_SRCADDR(value)           (DMAC_SRCADDR_SRCADDR_Msk & ((value) << DMAC_SRCADDR_SRCADDR_Pos))
#define DMAC_SRCADDR_Msk                      _U_(0xFFFFFFFF)                                      /**< (DMAC_SRCADDR) Register Mask  */


/* -------- DMAC_DSTADDR : (DMAC Offset: 0x08) (R/W 32) Block Transfer Destination Address -------- */
#define DMAC_DSTADDR_DSTADDR_Pos              _U_(0)                                               /**< (DMAC_DSTADDR) Transfer Destination Address Position */
#define DMAC_DSTADDR_DSTADDR_Msk              (_U_(0xFFFFFFFF) << DMAC_DSTADDR_DSTADDR_Pos)        /**< (DMAC_DSTADDR) Transfer Destination Address Mask */
#define DMAC_DSTADDR_DSTADDR(value)           (DMAC_DSTADDR_DSTADDR_Msk & ((value) << DMAC_DSTADDR_DSTADDR_Pos))
#define DMAC_DSTADDR_Msk                      _U_(0xFFFFFFFF)                                      /**< (DMAC_DSTADDR) Register Mask  */

/* CRC mode */
#define DMAC_DSTADDR_CRC_CHKINIT_Pos          _U_(0)                                               /**< (DMAC_DSTADDR) CRC Checksum Initial Value Position */
#define DMAC_DSTADDR_CRC_CHKINIT_Msk          (_U_(0xFFFFFFFF) << DMAC_DSTADDR_CRC_CHKINIT_Pos)    /**< (DMAC_DSTADDR) CRC Checksum Initial Value Mask */
#define DMAC_DSTADDR_CRC_CHKINIT(value)       (DMAC_DSTADDR_CRC_CHKINIT_Msk & ((value) << DMAC_DSTADDR_CRC_CHKINIT_Pos))
#define DMAC_DSTADDR_CRC_Msk                  _U_(0xFFFFFFFF)                                       /**< (DMAC_DSTADDR_CRC) Register Mask  */


/* -------- DMAC_DESCADDR : (DMAC Offset: 0x0C) (R/W 32) Next Descriptor Address -------- */
#define DMAC_DESCADDR_DESCADDR_Pos            _U_(0)                                               /**< (DMAC_DESCADDR) Next Descriptor Address Position */
#define DMAC_DESCADDR_DESCADDR_Msk            (_U_(0xFFFFFFFF) << DMAC_DESCADDR_DESCADDR_Pos)      /**< (DMAC_DESCADDR) Next Descriptor Address Mask */
#define DMAC_DESCADDR_DESCADDR(value)         (DMAC_DESCADDR_DESCADDR_Msk & ((value) << DMAC_DESCADDR_DESCADDR_Pos))
#define DMAC_DESCADDR_Msk                     _U_(0xFFFFFFFF)                                      /**< (DMAC_DESCADDR) Register Mask  */


/* -------- DMAC_CHCTRLA : (DMAC Offset: 0x00) (R/W 32) Channel n Control A -------- */
#define DMAC_CHCTRLA_RESETVALUE               _U_(0x00)                                            /**<  (DMAC_CHCTRLA) Channel n Control A  Reset Value */

#define DMAC_CHCTRLA_SWRST_Pos                _U_(0)                                               /**< (DMAC_CHCTRLA) Channel Software Reset Position */
#define DMAC_CHCTRLA_SWRST_Msk                (_U_(0x1) << DMAC_CHCTRLA_SWRST_Pos)                 /**< (DMAC_CHCTRLA) Channel Software Reset Mask */
#define DMAC_CHCTRLA_SWRST(value)             (DMAC_CHCTRLA_SWRST_Msk & ((value) << DMAC_CHCTRLA_SWRST_Pos))
#define DMAC_CHCTRLA_ENABLE_Pos               _U_(1)                                               /**< (DMAC_CHCTRLA) Channel Enable Position */
#define DMAC_CHCTRLA_ENABLE_Msk               (_U_(0x1) << DMAC_CHCTRLA_ENABLE_Pos)                /**< (DMAC_CHCTRLA) Channel Enable Mask */
#define DMAC_CHCTRLA_ENABLE(value)            (DMAC_CHCTRLA_ENABLE_Msk & ((value) << DMAC_CHCTRLA_ENABLE_Pos))
#define DMAC_CHCTRLA_RUNSTDBY_Pos             _U_(6)                                               /**< (DMAC_CHCTRLA) Channel Run in Standby Position */
#define DMAC_CHCTRLA_RUNSTDBY_Msk             (_U_(0x1) << DMAC_CHCTRLA_RUNSTDBY_Pos)              /**< (DMAC_CHCTRLA) Channel Run in Standby Mask */
#define DMAC_CHCTRLA_RUNSTDBY(value)          (DMAC_CHCTRLA_RUNSTDBY_Msk & ((value) << DMAC_CHCTRLA_RUNSTDBY_Pos))
#define DMAC_CHCTRLA_TRIGSRC_Pos              _U_(8)                                               /**< (DMAC_CHCTRLA) Trigger Source Position */
#define DMAC_CHCTRLA_TRIGSRC_Msk              (_U_(0x7F) << DMAC_CHCTRLA_TRIGSRC_Pos)              /**< (DMAC_CHCTRLA) Trigger Source Mask */
#define DMAC_CHCTRLA_TRIGSRC(value)           (DMAC_CHCTRLA_TRIGSRC_Msk & ((value) << DMAC_CHCTRLA_TRIGSRC_Pos))
#define   DMAC_CHCTRLA_TRIGSRC_DISABLE_Val    _U_(0x0)                                             /**< (DMAC_CHCTRLA) Only software/event triggers  */
#define DMAC_CHCTRLA_TRIGSRC_DISABLE          (DMAC_CHCTRLA_TRIGSRC_DISABLE_Val << DMAC_CHCTRLA_TRIGSRC_Pos) /**< (DMAC_CHCTRLA) Only software/event triggers Position  */
#define DMAC_CHCTRLA_TRIGACT_Pos              _U_(20)                                              /**< (DMAC_CHCTRLA) Trigger Action Position */
#define DMAC_CHCTRLA_TRIGACT_Msk              (_U_(0x3) << DMAC_CHCTRLA_TRIGACT_Pos)               /**< (DMAC_CHCTRLA) Trigger Action Mask */
#define DMAC_CHCTRLA_TRIGACT(value)           (DMAC_CHCTRLA_TRIGACT_Msk & ((value) << DMAC_CHCTRLA_TRIGACT_Pos))
#define   DMAC_CHCTRLA_TRIGACT_BLOCK_Val      _U_(0x0)                                             /**< (DMAC_CHCTRLA) One trigger required for each block transfer  */
#define   DMAC_CHCTRLA_TRIGACT_BURST_Val      _U_(0x2)                                             /**< (DMAC_CHCTRLA) One trigger required for each burst transfer  */
#define   DMAC_CHCTRLA_TRIGACT_TRANSACTION_Val _U_(0x3)                                             /**< (DMAC_CHCTRLA) One trigger required for each transaction  */
#define DMAC_CHCTRLA_TRIGACT_BLOCK            (DMAC_CHCTRLA_TRIGACT_BLOCK_Val << DMAC_CHCTRLA_TRIGACT_Pos) /**< (DMAC_CHCTRLA) One trigger required for each block transfer Position  */
#define DMAC_CHCTRLA_TRIGACT_BURST            (DMAC_CHCTRLA_TRIGACT_BURST_Val << DMAC_CHCTRLA_TRIGACT_Pos) /**< (DMAC_CHCTRLA) One trigger required for each burst transfer Position  */
#define DMAC_CHCTRLA_TRIGACT_TRANSACTION      (DMAC_CHCTRLA_TRIGACT_TRANSACTION_Val << DMAC_CHCTRLA_TRIGACT_Pos) /**< (DMAC_CHCTRLA) One trigger required for each transaction Position  */
#define DMAC_CHCTRLA_BURSTLEN_Pos             _U_(24)                                              /**< (DMAC_CHCTRLA) Burst Length Position */
#define DMAC_CHCTRLA_BURSTLEN_Msk             (_U_(0xF) << DMAC_CHCTRLA_BURSTLEN_Pos)              /**< (DMAC_CHCTRLA) Burst Length Mask */
#define DMAC_CHCTRLA_BURSTLEN(value)          (DMAC_CHCTRLA_BURSTLEN_Msk & ((value) << DMAC_CHCTRLA_BURSTLEN_Pos))
#define   DMAC_CHCTRLA_BURSTLEN_SINGLE_Val    _U_(0x0)                                             /**< (DMAC_CHCTRLA) Single-beat burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_2BEAT_Val     _U_(0x1)                                             /**< (DMAC_CHCTRLA) 2-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_3BEAT_Val     _U_(0x2)                                             /**< (DMAC_CHCTRLA) 3-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_4BEAT_Val     _U_(0x3)                                             /**< (DMAC_CHCTRLA) 4-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_5BEAT_Val     _U_(0x4)                                             /**< (DMAC_CHCTRLA) 5-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_6BEAT_Val     _U_(0x5)                                             /**< (DMAC_CHCTRLA) 6-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_7BEAT_Val     _U_(0x6)                                             /**< (DMAC_CHCTRLA) 7-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_8BEAT_Val     _U_(0x7)                                             /**< (DMAC_CHCTRLA) 8-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_9BEAT_Val     _U_(0x8)                                             /**< (DMAC_CHCTRLA) 9-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_10BEAT_Val    _U_(0x9)                                             /**< (DMAC_CHCTRLA) 10-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_11BEAT_Val    _U_(0xA)                                             /**< (DMAC_CHCTRLA) 11-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_12BEAT_Val    _U_(0xB)                                             /**< (DMAC_CHCTRLA) 12-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_13BEAT_Val    _U_(0xC)                                             /**< (DMAC_CHCTRLA) 13-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_14BEAT_Val    _U_(0xD)                                             /**< (DMAC_CHCTRLA) 14-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_15BEAT_Val    _U_(0xE)                                             /**< (DMAC_CHCTRLA) 15-beats burst length  */
#define   DMAC_CHCTRLA_BURSTLEN_16BEAT_Val    _U_(0xF)                                             /**< (DMAC_CHCTRLA) 16-beats burst length  */
#define DMAC_CHCTRLA_BURSTLEN_SINGLE          (DMAC_CHCTRLA_BURSTLEN_SINGLE_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) Single-beat burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_2BEAT           (DMAC_CHCTRLA_BURSTLEN_2BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 2-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_3BEAT           (DMAC_CHCTRLA_BURSTLEN_3BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 3-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_4BEAT           (DMAC_CHCTRLA_BURSTLEN_4BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 4-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_5BEAT           (DMAC_CHCTRLA_BURSTLEN_5BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 5-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_6BEAT           (DMAC_CHCTRLA_BURSTLEN_6BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 6-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_7BEAT           (DMAC_CHCTRLA_BURSTLEN_7BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 7-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_8BEAT           (DMAC_CHCTRLA_BURSTLEN_8BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 8-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_9BEAT           (DMAC_CHCTRLA_BURSTLEN_9BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 9-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_10BEAT          (DMAC_CHCTRLA_BURSTLEN_10BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 10-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_11BEAT          (DMAC_CHCTRLA_BURSTLEN_11BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 11-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_12BEAT          (DMAC_CHCTRLA_BURSTLEN_12BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 12-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_13BEAT          (DMAC_CHCTRLA_BURSTLEN_13BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 13-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_14BEAT          (DMAC_CHCTRLA_BURSTLEN_14BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 14-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_15BEAT          (DMAC_CHCTRLA_BURSTLEN_15BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 15-beats burst length Position  */
#define DMAC_CHCTRLA_BURSTLEN_16BEAT          (DMAC_CHCTRLA_BURSTLEN_16BEAT_Val << DMAC_CHCTRLA_BURSTLEN_Pos) /**< (DMAC_CHCTRLA) 16-beats burst length Position  */
#define DMAC_CHCTRLA_THRESHOLD_Pos            _U_(28)                                              /**< (DMAC_CHCTRLA) FIFO Threshold Position */
#define DMAC_CHCTRLA_THRESHOLD_Msk            (_U_(0x3) << DMAC_CHCTRLA_THRESHOLD_Pos)             /**< (DMAC_CHCTRLA) FIFO Threshold Mask */
#define DMAC_CHCTRLA_THRESHOLD(value)         (DMAC_CHCTRLA_THRESHOLD_Msk & ((value) << DMAC_CHCTRLA_THRESHOLD_Pos))
#define   DMAC_CHCTRLA_THRESHOLD_1BEAT_Val    _U_(0x0)                                             /**< (DMAC_CHCTRLA) Destination write starts after each beat source address read  */
#define   DMAC_CHCTRLA_THRESHOLD_2BEATS_Val   _U_(0x1)                                             /**< (DMAC_CHCTRLA) Destination write starts after 2-beats source address read  */
#define   DMAC_CHCTRLA_THRESHOLD_4BEATS_Val   _U_(0x2)                                             /**< (DMAC_CHCTRLA) Destination write starts after 4-beats source address read  */
#define   DMAC_CHCTRLA_THRESHOLD_8BEATS_Val   _U_(0x3)                                             /**< (DMAC_CHCTRLA) Destination write starts after 8-beats source address read  */
#define DMAC_CHCTRLA_THRESHOLD_1BEAT          (DMAC_CHCTRLA_THRESHOLD_1BEAT_Val << DMAC_CHCTRLA_THRESHOLD_Pos) /**< (DMAC_CHCTRLA) Destination write starts after each beat source address read Position  */
#define DMAC_CHCTRLA_THRESHOLD_2BEATS         (DMAC_CHCTRLA_THRESHOLD_2BEATS_Val << DMAC_CHCTRLA_THRESHOLD_Pos) /**< (DMAC_CHCTRLA) Destination write starts after 2-beats source address read Position  */
#define DMAC_CHCTRLA_THRESHOLD_4BEATS         (DMAC_CHCTRLA_THRESHOLD_4BEATS_Val << DMAC_CHCTRLA_THRESHOLD_Pos) /**< (DMAC_CHCTRLA) Destination write starts after 4-beats source address read Position  */
#define DMAC_CHCTRLA_THRESHOLD_8BEATS         (DMAC_CHCTRLA_THRESHOLD_8BEATS_Val << DMAC_CHCTRLA_THRESHOLD_Pos) /**< (DMAC_CHCTRLA) Destination write starts after 8-beats source address read Position  */
#define DMAC_CHCTRLA_Msk                      _U_(0x3F307F43)                                      /**< (DMAC_CHCTRLA) Register Mask  */


/* -------- DMAC_CHCTRLB : (DMAC Offset: 0x04) (R/W 8) Channel n Control B -------- */
#define DMAC_CHCTRLB_RESETVALUE               _U_(0x00)                                            /**<  (DMAC_CHCTRLB) Channel n Control B  Reset Value */

#define DMAC_CHCTRLB_CMD_Pos                  _U_(0)                                               /**< (DMAC_CHCTRLB) Software Command Position */
#define DMAC_CHCTRLB_CMD_Msk                  (_U_(0x3) << DMAC_CHCTRLB_CMD_Pos)                   /**< (DMAC_CHCTRLB) Software Command Mask */
#define DMAC_CHCTRLB_CMD(value)               (DMAC_CHCTRLB_CMD_Msk & ((value) << DMAC_CHCTRLB_CMD_Pos))
#define   DMAC_CHCTRLB_CMD_NOACT_Val          _U_(0x0)                                             /**< (DMAC_CHCTRLB) No action  */
#define   DMAC_CHCTRLB_CMD_SUSPEND_Val        _U_(0x1)                                             /**< (DMAC_CHCTRLB) Channel suspend operation  */
#define   DMAC_CHCTRLB_CMD_RESUME_Val         _U_(0x2)                                             /**< (DMAC_CHCTRLB) Channel resume operation  */
#define DMAC_CHCTRLB_CMD_NOACT                (DMAC_CHCTRLB_CMD_NOACT_Val << DMAC_CHCTRLB_CMD_Pos) /**< (DMAC_CHCTRLB) No action Position  */
#define DMAC_CHCTRLB_CMD_SUSPEND              (DMAC_CHCTRLB_CMD_SUSPEND_Val << DMAC_CHCTRLB_CMD_Pos) /**< (DMAC_CHCTRLB) Channel suspend operation Position  */
#define DMAC_CHCTRLB_CMD_RESUME               (DMAC_CHCTRLB_CMD_RESUME_Val << DMAC_CHCTRLB_CMD_Pos) /**< (DMAC_CHCTRLB) Channel resume operation Position  */
#define DMAC_CHCTRLB_Msk                      _U_(0x03)                                            /**< (DMAC_CHCTRLB) Register Mask  */


/* -------- DMAC_CHPRILVL : (DMAC Offset: 0x05) (R/W 8) Channel n Priority Level -------- */
#define DMAC_CHPRILVL_RESETVALUE              _U_(0x00)                                            /**<  (DMAC_CHPRILVL) Channel n Priority Level  Reset Value */

#define DMAC_CHPRILVL_PRILVL_Pos              _U_(0)                                               /**< (DMAC_CHPRILVL) Channel Priority Level Position */
#define DMAC_CHPRILVL_PRILVL_Msk              (_U_(0x3) << DMAC_CHPRILVL_PRILVL_Pos)               /**< (DMAC_CHPRILVL) Channel Priority Level Mask */
#define DMAC_CHPRILVL_PRILVL(value)           (DMAC_CHPRILVL_PRILVL_Msk & ((value) << DMAC_CHPRILVL_PRILVL_Pos))
#define   DMAC_CHPRILVL_PRILVL_LVL0_Val       _U_(0x0)                                             /**< (DMAC_CHPRILVL) Channel Priority Level 0 (Lowest Level)  */
#define   DMAC_CHPRILVL_PRILVL_LVL1_Val       _U_(0x1)                                             /**< (DMAC_CHPRILVL) Channel Priority Level 1  */
#define   DMAC_CHPRILVL_PRILVL_LVL2_Val       _U_(0x2)                                             /**< (DMAC_CHPRILVL) Channel Priority Level 2  */
#define   DMAC_CHPRILVL_PRILVL_LVL3_Val       _U_(0x3)                                             /**< (DMAC_CHPRILVL) Channel Priority Level 3 (Highest Level)  */
#define DMAC_CHPRILVL_PRILVL_LVL0             (DMAC_CHPRILVL_PRILVL_LVL0_Val << DMAC_CHPRILVL_PRILVL_Pos) /**< (DMAC_CHPRILVL) Channel Priority Level 0 (Lowest Level) Position  */
#define DMAC_CHPRILVL_PRILVL_LVL1             (DMAC_CHPRILVL_PRILVL_LVL1_Val << DMAC_CHPRILVL_PRILVL_Pos) /**< (DMAC_CHPRILVL) Channel Priority Level 1 Position  */
#define DMAC_CHPRILVL_PRILVL_LVL2             (DMAC_CHPRILVL_PRILVL_LVL2_Val << DMAC_CHPRILVL_PRILVL_Pos) /**< (DMAC_CHPRILVL) Channel Priority Level 2 Position  */
#define DMAC_CHPRILVL_PRILVL_LVL3             (DMAC_CHPRILVL_PRILVL_LVL3_Val << DMAC_CHPRILVL_PRILVL_Pos) /**< (DMAC_CHPRILVL) Channel Priority Level 3 (Highest Level) Position  */
#define DMAC_CHPRILVL_Msk                     _U_(0x03)                                            /**< (DMAC_CHPRILVL) Register Mask  */


/* -------- DMAC_CHEVCTRL : (DMAC Offset: 0x06) (R/W 8) Channel n Event Control -------- */
#define DMAC_CHEVCTRL_RESETVALUE              _U_(0x00)                                            /**<  (DMAC_CHEVCTRL) Channel n Event Control  Reset Value */

#define DMAC_CHEVCTRL_EVACT_Pos               _U_(0)                                               /**< (DMAC_CHEVCTRL) Channel Event Input Action Position */
#define DMAC_CHEVCTRL_EVACT_Msk               (_U_(0x7) << DMAC_CHEVCTRL_EVACT_Pos)                /**< (DMAC_CHEVCTRL) Channel Event Input Action Mask */
#define DMAC_CHEVCTRL_EVACT(value)            (DMAC_CHEVCTRL_EVACT_Msk & ((value) << DMAC_CHEVCTRL_EVACT_Pos))
#define   DMAC_CHEVCTRL_EVACT_NOACT_Val       _U_(0x0)                                             /**< (DMAC_CHEVCTRL) No action  */
#define   DMAC_CHEVCTRL_EVACT_TRIG_Val        _U_(0x1)                                             /**< (DMAC_CHEVCTRL) Transfer and periodic transfer trigger  */
#define   DMAC_CHEVCTRL_EVACT_CTRIG_Val       _U_(0x2)                                             /**< (DMAC_CHEVCTRL) Conditional transfer trigger  */
#define   DMAC_CHEVCTRL_EVACT_CBLOCK_Val      _U_(0x3)                                             /**< (DMAC_CHEVCTRL) Conditional block transfer  */
#define   DMAC_CHEVCTRL_EVACT_SUSPEND_Val     _U_(0x4)                                             /**< (DMAC_CHEVCTRL) Channel suspend operation  */
#define   DMAC_CHEVCTRL_EVACT_RESUME_Val      _U_(0x5)                                             /**< (DMAC_CHEVCTRL) Channel resume operation  */
#define   DMAC_CHEVCTRL_EVACT_SSKIP_Val       _U_(0x6)                                             /**< (DMAC_CHEVCTRL) Skip next block suspend action  */
#define   DMAC_CHEVCTRL_EVACT_INCPRI_Val      _U_(0x7)                                             /**< (DMAC_CHEVCTRL) Increase priority  */
#define DMAC_CHEVCTRL_EVACT_NOACT             (DMAC_CHEVCTRL_EVACT_NOACT_Val << DMAC_CHEVCTRL_EVACT_Pos) /**< (DMAC_CHEVCTRL) No action Position  */
#define DMAC_CHEVCTRL_EVACT_TRIG              (DMAC_CHEVCTRL_EVACT_TRIG_Val << DMAC_CHEVCTRL_EVACT_Pos) /**< (DMAC_CHEVCTRL) Transfer and periodic transfer trigger Position  */
#define DMAC_CHEVCTRL_EVACT_CTRIG             (DMAC_CHEVCTRL_EVACT_CTRIG_Val << DMAC_CHEVCTRL_EVACT_Pos) /**< (DMAC_CHEVCTRL) Conditional transfer trigger Position  */
#define DMAC_CHEVCTRL_EVACT_CBLOCK            (DMAC_CHEVCTRL_EVACT_CBLOCK_Val << DMAC_CHEVCTRL_EVACT_Pos) /**< (DMAC_CHEVCTRL) Conditional block transfer Position  */
#define DMAC_CHEVCTRL_EVACT_SUSPEND           (DMAC_CHEVCTRL_EVACT_SUSPEND_Val << DMAC_CHEVCTRL_EVACT_Pos) /**< (DMAC_CHEVCTRL) Channel suspend operation Position  */
#define DMAC_CHEVCTRL_EVACT_RESUME            (DMAC_CHEVCTRL_EVACT_RESUME_Val << DMAC_CHEVCTRL_EVACT_Pos) /**< (DMAC_CHEVCTRL) Channel resume operation Position  */
#define DMAC_CHEVCTRL_EVACT_SSKIP             (DMAC_CHEVCTRL_EVACT_SSKIP_Val << DMAC_CHEVCTRL_EVACT_Pos) /**< (DMAC_CHEVCTRL) Skip next block suspend action Position  */
#define DMAC_CHEVCTRL_EVACT_INCPRI            (DMAC_CHEVCTRL_EVACT_INCPRI_Val << DMAC_CHEVCTRL_EVACT_Pos) /**< (DMAC_CHEVCTRL) Increase priority Position  */
#define DMAC_CHEVCTRL_EVOMODE_Pos             _U_(4)                                               /**< (DMAC_CHEVCTRL) Channel Event Output Mode Position */
#define DMAC_CHEVCTRL_EVOMODE_Msk             (_U_(0x3) << DMAC_CHEVCTRL_EVOMODE_Pos)              /**< (DMAC_CHEVCTRL) Channel Event Output Mode Mask */
#define DMAC_CHEVCTRL_EVOMODE(value)          (DMAC_CHEVCTRL_EVOMODE_Msk & ((value) << DMAC_CHEVCTRL_EVOMODE_Pos))
#define   DMAC_CHEVCTRL_EVOMODE_DEFAULT_Val   _U_(0x0)                                             /**< (DMAC_CHEVCTRL) Block event output selection. Refer to BTCTRL.EVOSEL for available selections.  */
#define   DMAC_CHEVCTRL_EVOMODE_TRIGACT_Val   _U_(0x1)                                             /**< (DMAC_CHEVCTRL) Ongoing trigger action  */
#define DMAC_CHEVCTRL_EVOMODE_DEFAULT         (DMAC_CHEVCTRL_EVOMODE_DEFAULT_Val << DMAC_CHEVCTRL_EVOMODE_Pos) /**< (DMAC_CHEVCTRL) Block event output selection. Refer to BTCTRL.EVOSEL for available selections. Position  */
#define DMAC_CHEVCTRL_EVOMODE_TRIGACT         (DMAC_CHEVCTRL_EVOMODE_TRIGACT_Val << DMAC_CHEVCTRL_EVOMODE_Pos) /**< (DMAC_CHEVCTRL) Ongoing trigger action Position  */
#define DMAC_CHEVCTRL_EVIE_Pos                _U_(6)                                               /**< (DMAC_CHEVCTRL) Channel Event Input Enable Position */
#define DMAC_CHEVCTRL_EVIE_Msk                (_U_(0x1) << DMAC_CHEVCTRL_EVIE_Pos)                 /**< (DMAC_CHEVCTRL) Channel Event Input Enable Mask */
#define DMAC_CHEVCTRL_EVIE(value)             (DMAC_CHEVCTRL_EVIE_Msk & ((value) << DMAC_CHEVCTRL_EVIE_Pos))
#define DMAC_CHEVCTRL_EVOE_Pos                _U_(7)                                               /**< (DMAC_CHEVCTRL) Channel Event Output Enable Position */
#define DMAC_CHEVCTRL_EVOE_Msk                (_U_(0x1) << DMAC_CHEVCTRL_EVOE_Pos)                 /**< (DMAC_CHEVCTRL) Channel Event Output Enable Mask */
#define DMAC_CHEVCTRL_EVOE(value)             (DMAC_CHEVCTRL_EVOE_Msk & ((value) << DMAC_CHEVCTRL_EVOE_Pos))
#define DMAC_CHEVCTRL_Msk                     _U_(0xF7)                                            /**< (DMAC_CHEVCTRL) Register Mask  */


/* -------- DMAC_CHINTENCLR : (DMAC Offset: 0x0C) (R/W 8) Channel n Interrupt Enable Clear -------- */
#define DMAC_CHINTENCLR_RESETVALUE            _U_(0x00)                                            /**<  (DMAC_CHINTENCLR) Channel n Interrupt Enable Clear  Reset Value */

#define DMAC_CHINTENCLR_TERR_Pos              _U_(0)                                               /**< (DMAC_CHINTENCLR) Channel Transfer Error Interrupt Enable Position */
#define DMAC_CHINTENCLR_TERR_Msk              (_U_(0x1) << DMAC_CHINTENCLR_TERR_Pos)               /**< (DMAC_CHINTENCLR) Channel Transfer Error Interrupt Enable Mask */
#define DMAC_CHINTENCLR_TERR(value)           (DMAC_CHINTENCLR_TERR_Msk & ((value) << DMAC_CHINTENCLR_TERR_Pos))
#define DMAC_CHINTENCLR_TCMPL_Pos             _U_(1)                                               /**< (DMAC_CHINTENCLR) Channel Transfer Complete Interrupt Enable Position */
#define DMAC_CHINTENCLR_TCMPL_Msk             (_U_(0x1) << DMAC_CHINTENCLR_TCMPL_Pos)              /**< (DMAC_CHINTENCLR) Channel Transfer Complete Interrupt Enable Mask */
#define DMAC_CHINTENCLR_TCMPL(value)          (DMAC_CHINTENCLR_TCMPL_Msk & ((value) << DMAC_CHINTENCLR_TCMPL_Pos))
#define DMAC_CHINTENCLR_SUSP_Pos              _U_(2)                                               /**< (DMAC_CHINTENCLR) Channel Suspend Interrupt Enable Position */
#define DMAC_CHINTENCLR_SUSP_Msk              (_U_(0x1) << DMAC_CHINTENCLR_SUSP_Pos)               /**< (DMAC_CHINTENCLR) Channel Suspend Interrupt Enable Mask */
#define DMAC_CHINTENCLR_SUSP(value)           (DMAC_CHINTENCLR_SUSP_Msk & ((value) << DMAC_CHINTENCLR_SUSP_Pos))
#define DMAC_CHINTENCLR_Msk                   _U_(0x07)                                            /**< (DMAC_CHINTENCLR) Register Mask  */


/* -------- DMAC_CHINTENSET : (DMAC Offset: 0x0D) (R/W 8) Channel n Interrupt Enable Set -------- */
#define DMAC_CHINTENSET_RESETVALUE            _U_(0x00)                                            /**<  (DMAC_CHINTENSET) Channel n Interrupt Enable Set  Reset Value */

#define DMAC_CHINTENSET_TERR_Pos              _U_(0)                                               /**< (DMAC_CHINTENSET) Channel Transfer Error Interrupt Enable Position */
#define DMAC_CHINTENSET_TERR_Msk              (_U_(0x1) << DMAC_CHINTENSET_TERR_Pos)               /**< (DMAC_CHINTENSET) Channel Transfer Error Interrupt Enable Mask */
#define DMAC_CHINTENSET_TERR(value)           (DMAC_CHINTENSET_TERR_Msk & ((value) << DMAC_CHINTENSET_TERR_Pos))
#define DMAC_CHINTENSET_TCMPL_Pos             _U_(1)                                               /**< (DMAC_CHINTENSET) Channel Transfer Complete Interrupt Enable Position */
#define DMAC_CHINTENSET_TCMPL_Msk             (_U_(0x1) << DMAC_CHINTENSET_TCMPL_Pos)              /**< (DMAC_CHINTENSET) Channel Transfer Complete Interrupt Enable Mask */
#define DMAC_CHINTENSET_TCMPL(value)          (DMAC_CHINTENSET_TCMPL_Msk & ((value) << DMAC_CHINTENSET_TCMPL_Pos))
#define DMAC_CHINTENSET_SUSP_Pos              _U_(2)                                               /**< (DMAC_CHINTENSET) Channel Suspend Interrupt Enable Position */
#define DMAC_CHINTENSET_SUSP_Msk              (_U_(0x1) << DMAC_CHINTENSET_SUSP_Pos)               /**< (DMAC_CHINTENSET) Channel Suspend Interrupt Enable Mask */
#define DMAC_CHINTENSET_SUSP(value)           (DMAC_CHINTENSET_SUSP_Msk & ((value) << DMAC_CHINTENSET_SUSP_Pos))
#define DMAC_CHINTENSET_Msk                   _U_(0x07)                                            /**< (DMAC_CHINTENSET) Register Mask  */


/* -------- DMAC_CHINTFLAG : (DMAC Offset: 0x0E) (R/W 8) Channel n Interrupt Flag Status and Clear -------- */
#define DMAC_CHINTFLAG_RESETVALUE             _U_(0x00)                                            /**<  (DMAC_CHINTFLAG) Channel n Interrupt Flag Status and Clear  Reset Value */

#define DMAC_CHINTFLAG_TERR_Pos               _U_(0)                                               /**< (DMAC_CHINTFLAG) Channel Transfer Error Position */
#define DMAC_CHINTFLAG_TERR_Msk               (_U_(0x1) << DMAC_CHINTFLAG_TERR_Pos)                /**< (DMAC_CHINTFLAG) Channel Transfer Error Mask */
#define DMAC_CHINTFLAG_TERR(value)            (DMAC_CHINTFLAG_TERR_Msk & ((value) << DMAC_CHINTFLAG_TERR_Pos))
#define DMAC_CHINTFLAG_TCMPL_Pos              _U_(1)                                               /**< (DMAC_CHINTFLAG) Channel Transfer Complete Position */
#define DMAC_CHINTFLAG_TCMPL_Msk              (_U_(0x1) << DMAC_CHINTFLAG_TCMPL_Pos)               /**< (DMAC_CHINTFLAG) Channel Transfer Complete Mask */
#define DMAC_CHINTFLAG_TCMPL(value)           (DMAC_CHINTFLAG_TCMPL_Msk & ((value) << DMAC_CHINTFLAG_TCMPL_Pos))
#define DMAC_CHINTFLAG_SUSP_Pos               _U_(2)                                               /**< (DMAC_CHINTFLAG) Channel Suspend Position */
#define DMAC_CHINTFLAG_SUSP_Msk               (_U_(0x1) << DMAC_CHINTFLAG_SUSP_Pos)                /**< (DMAC_CHINTFLAG) Channel Suspend Mask */
#define DMAC_CHINTFLAG_SUSP(value)            (DMAC_CHINTFLAG_SUSP_Msk & ((value) << DMAC_CHINTFLAG_SUSP_Pos))
#define DMAC_CHINTFLAG_Msk                    _U_(0x07)                                            /**< (DMAC_CHINTFLAG) Register Mask  */


/* -------- DMAC_CHSTATUS : (DMAC Offset: 0x0F) (R/W 8) Channel n Status -------- */
#define DMAC_CHSTATUS_RESETVALUE              _U_(0x00)                                            /**<  (DMAC_CHSTATUS) Channel n Status  Reset Value */

#define DMAC_CHSTATUS_PEND_Pos                _U_(0)                                               /**< (DMAC_CHSTATUS) Channel Pending Position */
#define DMAC_CHSTATUS_PEND_Msk                (_U_(0x1) << DMAC_CHSTATUS_PEND_Pos)                 /**< (DMAC_CHSTATUS) Channel Pending Mask */
#define DMAC_CHSTATUS_PEND(value)             (DMAC_CHSTATUS_PEND_Msk & ((value) << DMAC_CHSTATUS_PEND_Pos))
#define DMAC_CHSTATUS_BUSY_Pos                _U_(1)                                               /**< (DMAC_CHSTATUS) Channel Busy Position */
#define DMAC_CHSTATUS_BUSY_Msk                (_U_(0x1) << DMAC_CHSTATUS_BUSY_Pos)                 /**< (DMAC_CHSTATUS) Channel Busy Mask */
#define DMAC_CHSTATUS_BUSY(value)             (DMAC_CHSTATUS_BUSY_Msk & ((value) << DMAC_CHSTATUS_BUSY_Pos))
#define DMAC_CHSTATUS_FERR_Pos                _U_(2)                                               /**< (DMAC_CHSTATUS) Channel Fetch Error Position */
#define DMAC_CHSTATUS_FERR_Msk                (_U_(0x1) << DMAC_CHSTATUS_FERR_Pos)                 /**< (DMAC_CHSTATUS) Channel Fetch Error Mask */
#define DMAC_CHSTATUS_FERR(value)             (DMAC_CHSTATUS_FERR_Msk & ((value) << DMAC_CHSTATUS_FERR_Pos))
#define DMAC_CHSTATUS_CRCERR_Pos              _U_(3)                                               /**< (DMAC_CHSTATUS) Channel CRC Error Position */
#define DMAC_CHSTATUS_CRCERR_Msk              (_U_(0x1) << DMAC_CHSTATUS_CRCERR_Pos)               /**< (DMAC_CHSTATUS) Channel CRC Error Mask */
#define DMAC_CHSTATUS_CRCERR(value)           (DMAC_CHSTATUS_CRCERR_Msk & ((value) << DMAC_CHSTATUS_CRCERR_Pos))
#define DMAC_CHSTATUS_Msk                     _U_(0x0F)                                            /**< (DMAC_CHSTATUS) Register Mask  */


/* -------- DMAC_CTRL : (DMAC Offset: 0x00) (R/W 16) Control -------- */
#define DMAC_CTRL_RESETVALUE                  _U_(0x00)                                            /**<  (DMAC_CTRL) Control  Reset Value */

#define DMAC_CTRL_SWRST_Pos                   _U_(0)                                               /**< (DMAC_CTRL) Software Reset Position */
#define DMAC_CTRL_SWRST_Msk                   (_U_(0x1) << DMAC_CTRL_SWRST_Pos)                    /**< (DMAC_CTRL) Software Reset Mask */
#define DMAC_CTRL_SWRST(value)                (DMAC_CTRL_SWRST_Msk & ((value) << DMAC_CTRL_SWRST_Pos))
#define DMAC_CTRL_DMAENABLE_Pos               _U_(1)                                               /**< (DMAC_CTRL) DMA Enable Position */
#define DMAC_CTRL_DMAENABLE_Msk               (_U_(0x1) << DMAC_CTRL_DMAENABLE_Pos)                /**< (DMAC_CTRL) DMA Enable Mask */
#define DMAC_CTRL_DMAENABLE(value)            (DMAC_CTRL_DMAENABLE_Msk & ((value) << DMAC_CTRL_DMAENABLE_Pos))
#define DMAC_CTRL_LVLEN0_Pos                  _U_(8)                                               /**< (DMAC_CTRL) Priority Level 0 Enable Position */
#define DMAC_CTRL_LVLEN0_Msk                  (_U_(0x1) << DMAC_CTRL_LVLEN0_Pos)                   /**< (DMAC_CTRL) Priority Level 0 Enable Mask */
#define DMAC_CTRL_LVLEN0(value)               (DMAC_CTRL_LVLEN0_Msk & ((value) << DMAC_CTRL_LVLEN0_Pos))
#define DMAC_CTRL_LVLEN1_Pos                  _U_(9)                                               /**< (DMAC_CTRL) Priority Level 1 Enable Position */
#define DMAC_CTRL_LVLEN1_Msk                  (_U_(0x1) << DMAC_CTRL_LVLEN1_Pos)                   /**< (DMAC_CTRL) Priority Level 1 Enable Mask */
#define DMAC_CTRL_LVLEN1(value)               (DMAC_CTRL_LVLEN1_Msk & ((value) << DMAC_CTRL_LVLEN1_Pos))
#define DMAC_CTRL_LVLEN2_Pos                  _U_(10)                                              /**< (DMAC_CTRL) Priority Level 2 Enable Position */
#define DMAC_CTRL_LVLEN2_Msk                  (_U_(0x1) << DMAC_CTRL_LVLEN2_Pos)                   /**< (DMAC_CTRL) Priority Level 2 Enable Mask */
#define DMAC_CTRL_LVLEN2(value)               (DMAC_CTRL_LVLEN2_Msk & ((value) << DMAC_CTRL_LVLEN2_Pos))
#define DMAC_CTRL_LVLEN3_Pos                  _U_(11)                                              /**< (DMAC_CTRL) Priority Level 3 Enable Position */
#define DMAC_CTRL_LVLEN3_Msk                  (_U_(0x1) << DMAC_CTRL_LVLEN3_Pos)                   /**< (DMAC_CTRL) Priority Level 3 Enable Mask */
#define DMAC_CTRL_LVLEN3(value)               (DMAC_CTRL_LVLEN3_Msk & ((value) << DMAC_CTRL_LVLEN3_Pos))
#define DMAC_CTRL_Msk                         _U_(0x0F03)                                          /**< (DMAC_CTRL) Register Mask  */

#define DMAC_CTRL_LVLEN_Pos                   _U_(8)                                               /**< (DMAC_CTRL Position) Priority Level 3 Enable */
#define DMAC_CTRL_LVLEN_Msk                   (_U_(0xF) << DMAC_CTRL_LVLEN_Pos)                    /**< (DMAC_CTRL Mask) LVLEN */
#define DMAC_CTRL_LVLEN(value)                (DMAC_CTRL_LVLEN_Msk & ((value) << DMAC_CTRL_LVLEN_Pos)) 

/* -------- DMAC_CRCCTRL : (DMAC Offset: 0x02) (R/W 16) CRC Control -------- */
#define DMAC_CRCCTRL_RESETVALUE               _U_(0x00)                                            /**<  (DMAC_CRCCTRL) CRC Control  Reset Value */

#define DMAC_CRCCTRL_CRCBEATSIZE_Pos          _U_(0)                                               /**< (DMAC_CRCCTRL) CRC Beat Size Position */
#define DMAC_CRCCTRL_CRCBEATSIZE_Msk          (_U_(0x3) << DMAC_CRCCTRL_CRCBEATSIZE_Pos)           /**< (DMAC_CRCCTRL) CRC Beat Size Mask */
#define DMAC_CRCCTRL_CRCBEATSIZE(value)       (DMAC_CRCCTRL_CRCBEATSIZE_Msk & ((value) << DMAC_CRCCTRL_CRCBEATSIZE_Pos))
#define   DMAC_CRCCTRL_CRCBEATSIZE_BYTE_Val   _U_(0x0)                                             /**< (DMAC_CRCCTRL) 8-bit bus transfer  */
#define   DMAC_CRCCTRL_CRCBEATSIZE_HWORD_Val  _U_(0x1)                                             /**< (DMAC_CRCCTRL) 16-bit bus transfer  */
#define   DMAC_CRCCTRL_CRCBEATSIZE_WORD_Val   _U_(0x2)                                             /**< (DMAC_CRCCTRL) 32-bit bus transfer  */
#define DMAC_CRCCTRL_CRCBEATSIZE_BYTE         (DMAC_CRCCTRL_CRCBEATSIZE_BYTE_Val << DMAC_CRCCTRL_CRCBEATSIZE_Pos) /**< (DMAC_CRCCTRL) 8-bit bus transfer Position  */
#define DMAC_CRCCTRL_CRCBEATSIZE_HWORD        (DMAC_CRCCTRL_CRCBEATSIZE_HWORD_Val << DMAC_CRCCTRL_CRCBEATSIZE_Pos) /**< (DMAC_CRCCTRL) 16-bit bus transfer Position  */
#define DMAC_CRCCTRL_CRCBEATSIZE_WORD         (DMAC_CRCCTRL_CRCBEATSIZE_WORD_Val << DMAC_CRCCTRL_CRCBEATSIZE_Pos) /**< (DMAC_CRCCTRL) 32-bit bus transfer Position  */
#define DMAC_CRCCTRL_CRCPOLY_Pos              _U_(2)                                               /**< (DMAC_CRCCTRL) CRC Polynomial Type Position */
#define DMAC_CRCCTRL_CRCPOLY_Msk              (_U_(0x3) << DMAC_CRCCTRL_CRCPOLY_Pos)               /**< (DMAC_CRCCTRL) CRC Polynomial Type Mask */
#define DMAC_CRCCTRL_CRCPOLY(value)           (DMAC_CRCCTRL_CRCPOLY_Msk & ((value) << DMAC_CRCCTRL_CRCPOLY_Pos))
#define   DMAC_CRCCTRL_CRCPOLY_CRC16_Val      _U_(0x0)                                             /**< (DMAC_CRCCTRL) CRC-16 (CRC-CCITT)  */
#define   DMAC_CRCCTRL_CRCPOLY_CRC32_Val      _U_(0x1)                                             /**< (DMAC_CRCCTRL) CRC32 (IEEE 802.3)  */
#define DMAC_CRCCTRL_CRCPOLY_CRC16            (DMAC_CRCCTRL_CRCPOLY_CRC16_Val << DMAC_CRCCTRL_CRCPOLY_Pos) /**< (DMAC_CRCCTRL) CRC-16 (CRC-CCITT) Position  */
#define DMAC_CRCCTRL_CRCPOLY_CRC32            (DMAC_CRCCTRL_CRCPOLY_CRC32_Val << DMAC_CRCCTRL_CRCPOLY_Pos) /**< (DMAC_CRCCTRL) CRC32 (IEEE 802.3) Position  */
#define DMAC_CRCCTRL_CRCSRC_Pos               _U_(8)                                               /**< (DMAC_CRCCTRL) CRC Input Source Position */
#define DMAC_CRCCTRL_CRCSRC_Msk               (_U_(0x3F) << DMAC_CRCCTRL_CRCSRC_Pos)               /**< (DMAC_CRCCTRL) CRC Input Source Mask */
#define DMAC_CRCCTRL_CRCSRC(value)            (DMAC_CRCCTRL_CRCSRC_Msk & ((value) << DMAC_CRCCTRL_CRCSRC_Pos))
#define   DMAC_CRCCTRL_CRCSRC_DISABLE_Val     _U_(0x0)                                             /**< (DMAC_CRCCTRL) CRC Disabled  */
#define   DMAC_CRCCTRL_CRCSRC_IO_Val          _U_(0x1)                                             /**< (DMAC_CRCCTRL) I/O interface  */
#define DMAC_CRCCTRL_CRCSRC_DISABLE           (DMAC_CRCCTRL_CRCSRC_DISABLE_Val << DMAC_CRCCTRL_CRCSRC_Pos) /**< (DMAC_CRCCTRL) CRC Disabled Position  */
#define DMAC_CRCCTRL_CRCSRC_IO                (DMAC_CRCCTRL_CRCSRC_IO_Val << DMAC_CRCCTRL_CRCSRC_Pos) /**< (DMAC_CRCCTRL) I/O interface Position  */
#define DMAC_CRCCTRL_CRCMODE_Pos              _U_(14)                                              /**< (DMAC_CRCCTRL) CRC Operating Mode Position */
#define DMAC_CRCCTRL_CRCMODE_Msk              (_U_(0x3) << DMAC_CRCCTRL_CRCMODE_Pos)               /**< (DMAC_CRCCTRL) CRC Operating Mode Mask */
#define DMAC_CRCCTRL_CRCMODE(value)           (DMAC_CRCCTRL_CRCMODE_Msk & ((value) << DMAC_CRCCTRL_CRCMODE_Pos))
#define   DMAC_CRCCTRL_CRCMODE_DEFAULT_Val    _U_(0x0)                                             /**< (DMAC_CRCCTRL) Default operating mode  */
#define   DMAC_CRCCTRL_CRCMODE_CRCMON_Val     _U_(0x2)                                             /**< (DMAC_CRCCTRL) Memory CRC monitor operating mode  */
#define   DMAC_CRCCTRL_CRCMODE_CRCGEN_Val     _U_(0x3)                                             /**< (DMAC_CRCCTRL) Memory CRC generation operating mode  */
#define DMAC_CRCCTRL_CRCMODE_DEFAULT          (DMAC_CRCCTRL_CRCMODE_DEFAULT_Val << DMAC_CRCCTRL_CRCMODE_Pos) /**< (DMAC_CRCCTRL) Default operating mode Position  */
#define DMAC_CRCCTRL_CRCMODE_CRCMON           (DMAC_CRCCTRL_CRCMODE_CRCMON_Val << DMAC_CRCCTRL_CRCMODE_Pos) /**< (DMAC_CRCCTRL) Memory CRC monitor operating mode Position  */
#define DMAC_CRCCTRL_CRCMODE_CRCGEN           (DMAC_CRCCTRL_CRCMODE_CRCGEN_Val << DMAC_CRCCTRL_CRCMODE_Pos) /**< (DMAC_CRCCTRL) Memory CRC generation operating mode Position  */
#define DMAC_CRCCTRL_Msk                      _U_(0xFF0F)                                          /**< (DMAC_CRCCTRL) Register Mask  */


/* -------- DMAC_CRCDATAIN : (DMAC Offset: 0x04) (R/W 32) CRC Data Input -------- */
#define DMAC_CRCDATAIN_RESETVALUE             _U_(0x00)                                            /**<  (DMAC_CRCDATAIN) CRC Data Input  Reset Value */

#define DMAC_CRCDATAIN_CRCDATAIN_Pos          _U_(0)                                               /**< (DMAC_CRCDATAIN) CRC Data Input Position */
#define DMAC_CRCDATAIN_CRCDATAIN_Msk          (_U_(0xFFFFFFFF) << DMAC_CRCDATAIN_CRCDATAIN_Pos)    /**< (DMAC_CRCDATAIN) CRC Data Input Mask */
#define DMAC_CRCDATAIN_CRCDATAIN(value)       (DMAC_CRCDATAIN_CRCDATAIN_Msk & ((value) << DMAC_CRCDATAIN_CRCDATAIN_Pos))
#define DMAC_CRCDATAIN_Msk                    _U_(0xFFFFFFFF)                                      /**< (DMAC_CRCDATAIN) Register Mask  */


/* -------- DMAC_CRCCHKSUM : (DMAC Offset: 0x08) (R/W 32) CRC Checksum -------- */
#define DMAC_CRCCHKSUM_RESETVALUE             _U_(0x00)                                            /**<  (DMAC_CRCCHKSUM) CRC Checksum  Reset Value */

#define DMAC_CRCCHKSUM_CRCCHKSUM_Pos          _U_(0)                                               /**< (DMAC_CRCCHKSUM) CRC Checksum Position */
#define DMAC_CRCCHKSUM_CRCCHKSUM_Msk          (_U_(0xFFFFFFFF) << DMAC_CRCCHKSUM_CRCCHKSUM_Pos)    /**< (DMAC_CRCCHKSUM) CRC Checksum Mask */
#define DMAC_CRCCHKSUM_CRCCHKSUM(value)       (DMAC_CRCCHKSUM_CRCCHKSUM_Msk & ((value) << DMAC_CRCCHKSUM_CRCCHKSUM_Pos))
#define DMAC_CRCCHKSUM_Msk                    _U_(0xFFFFFFFF)                                      /**< (DMAC_CRCCHKSUM) Register Mask  */


/* -------- DMAC_CRCSTATUS : (DMAC Offset: 0x0C) (R/W 8) CRC Status -------- */
#define DMAC_CRCSTATUS_RESETVALUE             _U_(0x00)                                            /**<  (DMAC_CRCSTATUS) CRC Status  Reset Value */

#define DMAC_CRCSTATUS_CRCBUSY_Pos            _U_(0)                                               /**< (DMAC_CRCSTATUS) CRC Module Busy Position */
#define DMAC_CRCSTATUS_CRCBUSY_Msk            (_U_(0x1) << DMAC_CRCSTATUS_CRCBUSY_Pos)             /**< (DMAC_CRCSTATUS) CRC Module Busy Mask */
#define DMAC_CRCSTATUS_CRCBUSY(value)         (DMAC_CRCSTATUS_CRCBUSY_Msk & ((value) << DMAC_CRCSTATUS_CRCBUSY_Pos))
#define DMAC_CRCSTATUS_CRCZERO_Pos            _U_(1)                                               /**< (DMAC_CRCSTATUS) CRC Zero Position */
#define DMAC_CRCSTATUS_CRCZERO_Msk            (_U_(0x1) << DMAC_CRCSTATUS_CRCZERO_Pos)             /**< (DMAC_CRCSTATUS) CRC Zero Mask */
#define DMAC_CRCSTATUS_CRCZERO(value)         (DMAC_CRCSTATUS_CRCZERO_Msk & ((value) << DMAC_CRCSTATUS_CRCZERO_Pos))
#define DMAC_CRCSTATUS_CRCERR_Pos             _U_(2)                                               /**< (DMAC_CRCSTATUS) CRC Error Position */
#define DMAC_CRCSTATUS_CRCERR_Msk             (_U_(0x1) << DMAC_CRCSTATUS_CRCERR_Pos)              /**< (DMAC_CRCSTATUS) CRC Error Mask */
#define DMAC_CRCSTATUS_CRCERR(value)          (DMAC_CRCSTATUS_CRCERR_Msk & ((value) << DMAC_CRCSTATUS_CRCERR_Pos))
#define DMAC_CRCSTATUS_Msk                    _U_(0x07)                                            /**< (DMAC_CRCSTATUS) Register Mask  */


/* -------- DMAC_DBGCTRL : (DMAC Offset: 0x0D) (R/W 8) Debug Control -------- */
#define DMAC_DBGCTRL_RESETVALUE               _U_(0x00)                                            /**<  (DMAC_DBGCTRL) Debug Control  Reset Value */

#define DMAC_DBGCTRL_DBGRUN_Pos               _U_(0)                                               /**< (DMAC_DBGCTRL) Debug Run Position */
#define DMAC_DBGCTRL_DBGRUN_Msk               (_U_(0x1) << DMAC_DBGCTRL_DBGRUN_Pos)                /**< (DMAC_DBGCTRL) Debug Run Mask */
#define DMAC_DBGCTRL_DBGRUN(value)            (DMAC_DBGCTRL_DBGRUN_Msk & ((value) << DMAC_DBGCTRL_DBGRUN_Pos))
#define DMAC_DBGCTRL_Msk                      _U_(0x01)                                            /**< (DMAC_DBGCTRL) Register Mask  */


/* -------- DMAC_SWTRIGCTRL : (DMAC Offset: 0x10) (R/W 32) Software Trigger Control -------- */
#define DMAC_SWTRIGCTRL_RESETVALUE            _U_(0x00)                                            /**<  (DMAC_SWTRIGCTRL) Software Trigger Control  Reset Value */

#define DMAC_SWTRIGCTRL_SWTRIG0_Pos           _U_(0)                                               /**< (DMAC_SWTRIGCTRL) Channel 0 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG0_Msk           (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG0_Pos)            /**< (DMAC_SWTRIGCTRL) Channel 0 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG0(value)        (DMAC_SWTRIGCTRL_SWTRIG0_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG0_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG1_Pos           _U_(1)                                               /**< (DMAC_SWTRIGCTRL) Channel 1 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG1_Msk           (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG1_Pos)            /**< (DMAC_SWTRIGCTRL) Channel 1 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG1(value)        (DMAC_SWTRIGCTRL_SWTRIG1_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG1_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG2_Pos           _U_(2)                                               /**< (DMAC_SWTRIGCTRL) Channel 2 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG2_Msk           (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG2_Pos)            /**< (DMAC_SWTRIGCTRL) Channel 2 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG2(value)        (DMAC_SWTRIGCTRL_SWTRIG2_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG2_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG3_Pos           _U_(3)                                               /**< (DMAC_SWTRIGCTRL) Channel 3 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG3_Msk           (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG3_Pos)            /**< (DMAC_SWTRIGCTRL) Channel 3 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG3(value)        (DMAC_SWTRIGCTRL_SWTRIG3_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG3_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG4_Pos           _U_(4)                                               /**< (DMAC_SWTRIGCTRL) Channel 4 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG4_Msk           (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG4_Pos)            /**< (DMAC_SWTRIGCTRL) Channel 4 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG4(value)        (DMAC_SWTRIGCTRL_SWTRIG4_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG4_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG5_Pos           _U_(5)                                               /**< (DMAC_SWTRIGCTRL) Channel 5 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG5_Msk           (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG5_Pos)            /**< (DMAC_SWTRIGCTRL) Channel 5 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG5(value)        (DMAC_SWTRIGCTRL_SWTRIG5_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG5_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG6_Pos           _U_(6)                                               /**< (DMAC_SWTRIGCTRL) Channel 6 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG6_Msk           (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG6_Pos)            /**< (DMAC_SWTRIGCTRL) Channel 6 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG6(value)        (DMAC_SWTRIGCTRL_SWTRIG6_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG6_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG7_Pos           _U_(7)                                               /**< (DMAC_SWTRIGCTRL) Channel 7 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG7_Msk           (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG7_Pos)            /**< (DMAC_SWTRIGCTRL) Channel 7 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG7(value)        (DMAC_SWTRIGCTRL_SWTRIG7_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG7_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG8_Pos           _U_(8)                                               /**< (DMAC_SWTRIGCTRL) Channel 8 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG8_Msk           (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG8_Pos)            /**< (DMAC_SWTRIGCTRL) Channel 8 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG8(value)        (DMAC_SWTRIGCTRL_SWTRIG8_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG8_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG9_Pos           _U_(9)                                               /**< (DMAC_SWTRIGCTRL) Channel 9 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG9_Msk           (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG9_Pos)            /**< (DMAC_SWTRIGCTRL) Channel 9 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG9(value)        (DMAC_SWTRIGCTRL_SWTRIG9_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG9_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG10_Pos          _U_(10)                                              /**< (DMAC_SWTRIGCTRL) Channel 10 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG10_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG10_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 10 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG10(value)       (DMAC_SWTRIGCTRL_SWTRIG10_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG10_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG11_Pos          _U_(11)                                              /**< (DMAC_SWTRIGCTRL) Channel 11 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG11_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG11_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 11 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG11(value)       (DMAC_SWTRIGCTRL_SWTRIG11_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG11_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG12_Pos          _U_(12)                                              /**< (DMAC_SWTRIGCTRL) Channel 12 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG12_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG12_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 12 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG12(value)       (DMAC_SWTRIGCTRL_SWTRIG12_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG12_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG13_Pos          _U_(13)                                              /**< (DMAC_SWTRIGCTRL) Channel 13 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG13_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG13_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 13 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG13(value)       (DMAC_SWTRIGCTRL_SWTRIG13_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG13_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG14_Pos          _U_(14)                                              /**< (DMAC_SWTRIGCTRL) Channel 14 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG14_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG14_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 14 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG14(value)       (DMAC_SWTRIGCTRL_SWTRIG14_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG14_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG15_Pos          _U_(15)                                              /**< (DMAC_SWTRIGCTRL) Channel 15 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG15_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG15_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 15 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG15(value)       (DMAC_SWTRIGCTRL_SWTRIG15_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG15_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG16_Pos          _U_(16)                                              /**< (DMAC_SWTRIGCTRL) Channel 16 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG16_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG16_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 16 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG16(value)       (DMAC_SWTRIGCTRL_SWTRIG16_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG16_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG17_Pos          _U_(17)                                              /**< (DMAC_SWTRIGCTRL) Channel 17 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG17_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG17_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 17 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG17(value)       (DMAC_SWTRIGCTRL_SWTRIG17_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG17_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG18_Pos          _U_(18)                                              /**< (DMAC_SWTRIGCTRL) Channel 18 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG18_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG18_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 18 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG18(value)       (DMAC_SWTRIGCTRL_SWTRIG18_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG18_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG19_Pos          _U_(19)                                              /**< (DMAC_SWTRIGCTRL) Channel 19 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG19_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG19_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 19 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG19(value)       (DMAC_SWTRIGCTRL_SWTRIG19_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG19_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG20_Pos          _U_(20)                                              /**< (DMAC_SWTRIGCTRL) Channel 20 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG20_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG20_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 20 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG20(value)       (DMAC_SWTRIGCTRL_SWTRIG20_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG20_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG21_Pos          _U_(21)                                              /**< (DMAC_SWTRIGCTRL) Channel 21 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG21_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG21_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 21 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG21(value)       (DMAC_SWTRIGCTRL_SWTRIG21_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG21_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG22_Pos          _U_(22)                                              /**< (DMAC_SWTRIGCTRL) Channel 22 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG22_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG22_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 22 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG22(value)       (DMAC_SWTRIGCTRL_SWTRIG22_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG22_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG23_Pos          _U_(23)                                              /**< (DMAC_SWTRIGCTRL) Channel 23 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG23_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG23_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 23 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG23(value)       (DMAC_SWTRIGCTRL_SWTRIG23_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG23_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG24_Pos          _U_(24)                                              /**< (DMAC_SWTRIGCTRL) Channel 24 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG24_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG24_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 24 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG24(value)       (DMAC_SWTRIGCTRL_SWTRIG24_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG24_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG25_Pos          _U_(25)                                              /**< (DMAC_SWTRIGCTRL) Channel 25 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG25_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG25_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 25 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG25(value)       (DMAC_SWTRIGCTRL_SWTRIG25_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG25_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG26_Pos          _U_(26)                                              /**< (DMAC_SWTRIGCTRL) Channel 26 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG26_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG26_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 26 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG26(value)       (DMAC_SWTRIGCTRL_SWTRIG26_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG26_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG27_Pos          _U_(27)                                              /**< (DMAC_SWTRIGCTRL) Channel 27 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG27_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG27_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 27 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG27(value)       (DMAC_SWTRIGCTRL_SWTRIG27_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG27_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG28_Pos          _U_(28)                                              /**< (DMAC_SWTRIGCTRL) Channel 28 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG28_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG28_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 28 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG28(value)       (DMAC_SWTRIGCTRL_SWTRIG28_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG28_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG29_Pos          _U_(29)                                              /**< (DMAC_SWTRIGCTRL) Channel 29 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG29_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG29_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 29 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG29(value)       (DMAC_SWTRIGCTRL_SWTRIG29_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG29_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG30_Pos          _U_(30)                                              /**< (DMAC_SWTRIGCTRL) Channel 30 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG30_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG30_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 30 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG30(value)       (DMAC_SWTRIGCTRL_SWTRIG30_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG30_Pos))
#define DMAC_SWTRIGCTRL_SWTRIG31_Pos          _U_(31)                                              /**< (DMAC_SWTRIGCTRL) Channel 31 Software Trigger Position */
#define DMAC_SWTRIGCTRL_SWTRIG31_Msk          (_U_(0x1) << DMAC_SWTRIGCTRL_SWTRIG31_Pos)           /**< (DMAC_SWTRIGCTRL) Channel 31 Software Trigger Mask */
#define DMAC_SWTRIGCTRL_SWTRIG31(value)       (DMAC_SWTRIGCTRL_SWTRIG31_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG31_Pos))
#define DMAC_SWTRIGCTRL_Msk                   _U_(0xFFFFFFFF)                                      /**< (DMAC_SWTRIGCTRL) Register Mask  */

#define DMAC_SWTRIGCTRL_SWTRIG_Pos            _U_(0)                                               /**< (DMAC_SWTRIGCTRL Position) Channel 3x Software Trigger */
#define DMAC_SWTRIGCTRL_SWTRIG_Msk            (_U_(0xFFFFFFFF) << DMAC_SWTRIGCTRL_SWTRIG_Pos)      /**< (DMAC_SWTRIGCTRL Mask) SWTRIG */
#define DMAC_SWTRIGCTRL_SWTRIG(value)         (DMAC_SWTRIGCTRL_SWTRIG_Msk & ((value) << DMAC_SWTRIGCTRL_SWTRIG_Pos)) 

/* -------- DMAC_PRICTRL0 : (DMAC Offset: 0x14) (R/W 32) Priority Control 0 -------- */
#define DMAC_PRICTRL0_RESETVALUE              _U_(0x40404040)                                      /**<  (DMAC_PRICTRL0) Priority Control 0  Reset Value */

#define DMAC_PRICTRL0_LVLPRI0_Pos             _U_(0)                                               /**< (DMAC_PRICTRL0) Level 0 Channel Priority Number Position */
#define DMAC_PRICTRL0_LVLPRI0_Msk             (_U_(0x1F) << DMAC_PRICTRL0_LVLPRI0_Pos)             /**< (DMAC_PRICTRL0) Level 0 Channel Priority Number Mask */
#define DMAC_PRICTRL0_LVLPRI0(value)          (DMAC_PRICTRL0_LVLPRI0_Msk & ((value) << DMAC_PRICTRL0_LVLPRI0_Pos))
#define DMAC_PRICTRL0_QOS0_Pos                _U_(5)                                               /**< (DMAC_PRICTRL0) Level 0 Quality of Service Position */
#define DMAC_PRICTRL0_QOS0_Msk                (_U_(0x3) << DMAC_PRICTRL0_QOS0_Pos)                 /**< (DMAC_PRICTRL0) Level 0 Quality of Service Mask */
#define DMAC_PRICTRL0_QOS0(value)             (DMAC_PRICTRL0_QOS0_Msk & ((value) << DMAC_PRICTRL0_QOS0_Pos))
#define   DMAC_PRICTRL0_QOS0_REGULAR_Val      _U_(0x0)                                             /**< (DMAC_PRICTRL0) Regular delivery  */
#define   DMAC_PRICTRL0_QOS0_SHORTAGE_Val     _U_(0x1)                                             /**< (DMAC_PRICTRL0) Bandwidth shortage  */
#define   DMAC_PRICTRL0_QOS0_SENSITIVE_Val    _U_(0x2)                                             /**< (DMAC_PRICTRL0) Latency sensitive  */
#define   DMAC_PRICTRL0_QOS0_CRITICAL_Val     _U_(0x3)                                             /**< (DMAC_PRICTRL0) Latency critical  */
#define DMAC_PRICTRL0_QOS0_REGULAR            (DMAC_PRICTRL0_QOS0_REGULAR_Val << DMAC_PRICTRL0_QOS0_Pos) /**< (DMAC_PRICTRL0) Regular delivery Position  */
#define DMAC_PRICTRL0_QOS0_SHORTAGE           (DMAC_PRICTRL0_QOS0_SHORTAGE_Val << DMAC_PRICTRL0_QOS0_Pos) /**< (DMAC_PRICTRL0) Bandwidth shortage Position  */
#define DMAC_PRICTRL0_QOS0_SENSITIVE          (DMAC_PRICTRL0_QOS0_SENSITIVE_Val << DMAC_PRICTRL0_QOS0_Pos) /**< (DMAC_PRICTRL0) Latency sensitive Position  */
#define DMAC_PRICTRL0_QOS0_CRITICAL           (DMAC_PRICTRL0_QOS0_CRITICAL_Val << DMAC_PRICTRL0_QOS0_Pos) /**< (DMAC_PRICTRL0) Latency critical Position  */
#define DMAC_PRICTRL0_RRLVLEN0_Pos            _U_(7)                                               /**< (DMAC_PRICTRL0) Level 0 Round-Robin Scheduling Enable Position */
#define DMAC_PRICTRL0_RRLVLEN0_Msk            (_U_(0x1) << DMAC_PRICTRL0_RRLVLEN0_Pos)             /**< (DMAC_PRICTRL0) Level 0 Round-Robin Scheduling Enable Mask */
#define DMAC_PRICTRL0_RRLVLEN0(value)         (DMAC_PRICTRL0_RRLVLEN0_Msk & ((value) << DMAC_PRICTRL0_RRLVLEN0_Pos))
#define DMAC_PRICTRL0_LVLPRI1_Pos             _U_(8)                                               /**< (DMAC_PRICTRL0) Level 1 Channel Priority Number Position */
#define DMAC_PRICTRL0_LVLPRI1_Msk             (_U_(0x1F) << DMAC_PRICTRL0_LVLPRI1_Pos)             /**< (DMAC_PRICTRL0) Level 1 Channel Priority Number Mask */
#define DMAC_PRICTRL0_LVLPRI1(value)          (DMAC_PRICTRL0_LVLPRI1_Msk & ((value) << DMAC_PRICTRL0_LVLPRI1_Pos))
#define DMAC_PRICTRL0_QOS1_Pos                _U_(13)                                              /**< (DMAC_PRICTRL0) Level 1 Quality of Service Position */
#define DMAC_PRICTRL0_QOS1_Msk                (_U_(0x3) << DMAC_PRICTRL0_QOS1_Pos)                 /**< (DMAC_PRICTRL0) Level 1 Quality of Service Mask */
#define DMAC_PRICTRL0_QOS1(value)             (DMAC_PRICTRL0_QOS1_Msk & ((value) << DMAC_PRICTRL0_QOS1_Pos))
#define   DMAC_PRICTRL0_QOS1_REGULAR_Val      _U_(0x0)                                             /**< (DMAC_PRICTRL0) Regular delivery  */
#define   DMAC_PRICTRL0_QOS1_SHORTAGE_Val     _U_(0x1)                                             /**< (DMAC_PRICTRL0) Bandwidth shortage  */
#define   DMAC_PRICTRL0_QOS1_SENSITIVE_Val    _U_(0x2)                                             /**< (DMAC_PRICTRL0) Latency sensitive  */
#define   DMAC_PRICTRL0_QOS1_CRITICAL_Val     _U_(0x3)                                             /**< (DMAC_PRICTRL0) Latency critical  */
#define DMAC_PRICTRL0_QOS1_REGULAR            (DMAC_PRICTRL0_QOS1_REGULAR_Val << DMAC_PRICTRL0_QOS1_Pos) /**< (DMAC_PRICTRL0) Regular delivery Position  */
#define DMAC_PRICTRL0_QOS1_SHORTAGE           (DMAC_PRICTRL0_QOS1_SHORTAGE_Val << DMAC_PRICTRL0_QOS1_Pos) /**< (DMAC_PRICTRL0) Bandwidth shortage Position  */
#define DMAC_PRICTRL0_QOS1_SENSITIVE          (DMAC_PRICTRL0_QOS1_SENSITIVE_Val << DMAC_PRICTRL0_QOS1_Pos) /**< (DMAC_PRICTRL0) Latency sensitive Position  */
#define DMAC_PRICTRL0_QOS1_CRITICAL           (DMAC_PRICTRL0_QOS1_CRITICAL_Val << DMAC_PRICTRL0_QOS1_Pos) /**< (DMAC_PRICTRL0) Latency critical Position  */
#define DMAC_PRICTRL0_RRLVLEN1_Pos            _U_(15)                                              /**< (DMAC_PRICTRL0) Level 1 Round-Robin Scheduling Enable Position */
#define DMAC_PRICTRL0_RRLVLEN1_Msk            (_U_(0x1) << DMAC_PRICTRL0_RRLVLEN1_Pos)             /**< (DMAC_PRICTRL0) Level 1 Round-Robin Scheduling Enable Mask */
#define DMAC_PRICTRL0_RRLVLEN1(value)         (DMAC_PRICTRL0_RRLVLEN1_Msk & ((value) << DMAC_PRICTRL0_RRLVLEN1_Pos))
#define DMAC_PRICTRL0_LVLPRI2_Pos             _U_(16)                                              /**< (DMAC_PRICTRL0) Level 2 Channel Priority Number Position */
#define DMAC_PRICTRL0_LVLPRI2_Msk             (_U_(0x1F) << DMAC_PRICTRL0_LVLPRI2_Pos)             /**< (DMAC_PRICTRL0) Level 2 Channel Priority Number Mask */
#define DMAC_PRICTRL0_LVLPRI2(value)          (DMAC_PRICTRL0_LVLPRI2_Msk & ((value) << DMAC_PRICTRL0_LVLPRI2_Pos))
#define DMAC_PRICTRL0_QOS2_Pos                _U_(21)                                              /**< (DMAC_PRICTRL0) Level 2 Quality of Service Position */
#define DMAC_PRICTRL0_QOS2_Msk                (_U_(0x3) << DMAC_PRICTRL0_QOS2_Pos)                 /**< (DMAC_PRICTRL0) Level 2 Quality of Service Mask */
#define DMAC_PRICTRL0_QOS2(value)             (DMAC_PRICTRL0_QOS2_Msk & ((value) << DMAC_PRICTRL0_QOS2_Pos))
#define   DMAC_PRICTRL0_QOS2_REGULAR_Val      _U_(0x0)                                             /**< (DMAC_PRICTRL0) Regular delivery  */
#define   DMAC_PRICTRL0_QOS2_SHORTAGE_Val     _U_(0x1)                                             /**< (DMAC_PRICTRL0) Bandwidth shortage  */
#define   DMAC_PRICTRL0_QOS2_SENSITIVE_Val    _U_(0x2)                                             /**< (DMAC_PRICTRL0) Latency sensitive  */
#define   DMAC_PRICTRL0_QOS2_CRITICAL_Val     _U_(0x3)                                             /**< (DMAC_PRICTRL0) Latency critical  */
#define DMAC_PRICTRL0_QOS2_REGULAR            (DMAC_PRICTRL0_QOS2_REGULAR_Val << DMAC_PRICTRL0_QOS2_Pos) /**< (DMAC_PRICTRL0) Regular delivery Position  */
#define DMAC_PRICTRL0_QOS2_SHORTAGE           (DMAC_PRICTRL0_QOS2_SHORTAGE_Val << DMAC_PRICTRL0_QOS2_Pos) /**< (DMAC_PRICTRL0) Bandwidth shortage Position  */
#define DMAC_PRICTRL0_QOS2_SENSITIVE          (DMAC_PRICTRL0_QOS2_SENSITIVE_Val << DMAC_PRICTRL0_QOS2_Pos) /**< (DMAC_PRICTRL0) Latency sensitive Position  */
#define DMAC_PRICTRL0_QOS2_CRITICAL           (DMAC_PRICTRL0_QOS2_CRITICAL_Val << DMAC_PRICTRL0_QOS2_Pos) /**< (DMAC_PRICTRL0) Latency critical Position  */
#define DMAC_PRICTRL0_RRLVLEN2_Pos            _U_(23)                                              /**< (DMAC_PRICTRL0) Level 2 Round-Robin Scheduling Enable Position */
#define DMAC_PRICTRL0_RRLVLEN2_Msk            (_U_(0x1) << DMAC_PRICTRL0_RRLVLEN2_Pos)             /**< (DMAC_PRICTRL0) Level 2 Round-Robin Scheduling Enable Mask */
#define DMAC_PRICTRL0_RRLVLEN2(value)         (DMAC_PRICTRL0_RRLVLEN2_Msk & ((value) << DMAC_PRICTRL0_RRLVLEN2_Pos))
#define DMAC_PRICTRL0_LVLPRI3_Pos             _U_(24)                                              /**< (DMAC_PRICTRL0) Level 3 Channel Priority Number Position */
#define DMAC_PRICTRL0_LVLPRI3_Msk             (_U_(0x1F) << DMAC_PRICTRL0_LVLPRI3_Pos)             /**< (DMAC_PRICTRL0) Level 3 Channel Priority Number Mask */
#define DMAC_PRICTRL0_LVLPRI3(value)          (DMAC_PRICTRL0_LVLPRI3_Msk & ((value) << DMAC_PRICTRL0_LVLPRI3_Pos))
#define DMAC_PRICTRL0_QOS3_Pos                _U_(29)                                              /**< (DMAC_PRICTRL0) Level 3 Quality of Service Position */
#define DMAC_PRICTRL0_QOS3_Msk                (_U_(0x3) << DMAC_PRICTRL0_QOS3_Pos)                 /**< (DMAC_PRICTRL0) Level 3 Quality of Service Mask */
#define DMAC_PRICTRL0_QOS3(value)             (DMAC_PRICTRL0_QOS3_Msk & ((value) << DMAC_PRICTRL0_QOS3_Pos))
#define   DMAC_PRICTRL0_QOS3_REGULAR_Val      _U_(0x0)                                             /**< (DMAC_PRICTRL0) Regular delivery  */
#define   DMAC_PRICTRL0_QOS3_SHORTAGE_Val     _U_(0x1)                                             /**< (DMAC_PRICTRL0) Bandwidth shortage  */
#define   DMAC_PRICTRL0_QOS3_SENSITIVE_Val    _U_(0x2)                                             /**< (DMAC_PRICTRL0) Latency sensitive  */
#define   DMAC_PRICTRL0_QOS3_CRITICAL_Val     _U_(0x3)                                             /**< (DMAC_PRICTRL0) Latency critical  */
#define DMAC_PRICTRL0_QOS3_REGULAR            (DMAC_PRICTRL0_QOS3_REGULAR_Val << DMAC_PRICTRL0_QOS3_Pos) /**< (DMAC_PRICTRL0) Regular delivery Position  */
#define DMAC_PRICTRL0_QOS3_SHORTAGE           (DMAC_PRICTRL0_QOS3_SHORTAGE_Val << DMAC_PRICTRL0_QOS3_Pos) /**< (DMAC_PRICTRL0) Bandwidth shortage Position  */
#define DMAC_PRICTRL0_QOS3_SENSITIVE          (DMAC_PRICTRL0_QOS3_SENSITIVE_Val << DMAC_PRICTRL0_QOS3_Pos) /**< (DMAC_PRICTRL0) Latency sensitive Position  */
#define DMAC_PRICTRL0_QOS3_CRITICAL           (DMAC_PRICTRL0_QOS3_CRITICAL_Val << DMAC_PRICTRL0_QOS3_Pos) /**< (DMAC_PRICTRL0) Latency critical Position  */
#define DMAC_PRICTRL0_RRLVLEN3_Pos            _U_(31)                                              /**< (DMAC_PRICTRL0) Level 3 Round-Robin Scheduling Enable Position */
#define DMAC_PRICTRL0_RRLVLEN3_Msk            (_U_(0x1) << DMAC_PRICTRL0_RRLVLEN3_Pos)             /**< (DMAC_PRICTRL0) Level 3 Round-Robin Scheduling Enable Mask */
#define DMAC_PRICTRL0_RRLVLEN3(value)         (DMAC_PRICTRL0_RRLVLEN3_Msk & ((value) << DMAC_PRICTRL0_RRLVLEN3_Pos))
#define DMAC_PRICTRL0_Msk                     _U_(0xFFFFFFFF)                                      /**< (DMAC_PRICTRL0) Register Mask  */


/* -------- DMAC_INTPEND : (DMAC Offset: 0x20) (R/W 16) Interrupt Pending -------- */
#define DMAC_INTPEND_RESETVALUE               _U_(0x00)                                            /**<  (DMAC_INTPEND) Interrupt Pending  Reset Value */

#define DMAC_INTPEND_ID_Pos                   _U_(0)                                               /**< (DMAC_INTPEND) Channel ID Position */
#define DMAC_INTPEND_ID_Msk                   (_U_(0x1F) << DMAC_INTPEND_ID_Pos)                   /**< (DMAC_INTPEND) Channel ID Mask */
#define DMAC_INTPEND_ID(value)                (DMAC_INTPEND_ID_Msk & ((value) << DMAC_INTPEND_ID_Pos))
#define DMAC_INTPEND_TERR_Pos                 _U_(8)                                               /**< (DMAC_INTPEND) Transfer Error Position */
#define DMAC_INTPEND_TERR_Msk                 (_U_(0x1) << DMAC_INTPEND_TERR_Pos)                  /**< (DMAC_INTPEND) Transfer Error Mask */
#define DMAC_INTPEND_TERR(value)              (DMAC_INTPEND_TERR_Msk & ((value) << DMAC_INTPEND_TERR_Pos))
#define DMAC_INTPEND_TCMPL_Pos                _U_(9)                                               /**< (DMAC_INTPEND) Transfer Complete Position */
#define DMAC_INTPEND_TCMPL_Msk                (_U_(0x1) << DMAC_INTPEND_TCMPL_Pos)                 /**< (DMAC_INTPEND) Transfer Complete Mask */
#define DMAC_INTPEND_TCMPL(value)             (DMAC_INTPEND_TCMPL_Msk & ((value) << DMAC_INTPEND_TCMPL_Pos))
#define DMAC_INTPEND_SUSP_Pos                 _U_(10)                                              /**< (DMAC_INTPEND) Channel Suspend Position */
#define DMAC_INTPEND_SUSP_Msk                 (_U_(0x1) << DMAC_INTPEND_SUSP_Pos)                  /**< (DMAC_INTPEND) Channel Suspend Mask */
#define DMAC_INTPEND_SUSP(value)              (DMAC_INTPEND_SUSP_Msk & ((value) << DMAC_INTPEND_SUSP_Pos))
#define DMAC_INTPEND_CRCERR_Pos               _U_(12)                                              /**< (DMAC_INTPEND) CRC Error Position */
#define DMAC_INTPEND_CRCERR_Msk               (_U_(0x1) << DMAC_INTPEND_CRCERR_Pos)                /**< (DMAC_INTPEND) CRC Error Mask */
#define DMAC_INTPEND_CRCERR(value)            (DMAC_INTPEND_CRCERR_Msk & ((value) << DMAC_INTPEND_CRCERR_Pos))
#define DMAC_INTPEND_FERR_Pos                 _U_(13)                                              /**< (DMAC_INTPEND) Fetch Error Position */
#define DMAC_INTPEND_FERR_Msk                 (_U_(0x1) << DMAC_INTPEND_FERR_Pos)                  /**< (DMAC_INTPEND) Fetch Error Mask */
#define DMAC_INTPEND_FERR(value)              (DMAC_INTPEND_FERR_Msk & ((value) << DMAC_INTPEND_FERR_Pos))
#define DMAC_INTPEND_BUSY_Pos                 _U_(14)                                              /**< (DMAC_INTPEND) Busy Position */
#define DMAC_INTPEND_BUSY_Msk                 (_U_(0x1) << DMAC_INTPEND_BUSY_Pos)                  /**< (DMAC_INTPEND) Busy Mask */
#define DMAC_INTPEND_BUSY(value)              (DMAC_INTPEND_BUSY_Msk & ((value) << DMAC_INTPEND_BUSY_Pos))
#define DMAC_INTPEND_PEND_Pos                 _U_(15)                                              /**< (DMAC_INTPEND) Pending Position */
#define DMAC_INTPEND_PEND_Msk                 (_U_(0x1) << DMAC_INTPEND_PEND_Pos)                  /**< (DMAC_INTPEND) Pending Mask */
#define DMAC_INTPEND_PEND(value)              (DMAC_INTPEND_PEND_Msk & ((value) << DMAC_INTPEND_PEND_Pos))
#define DMAC_INTPEND_Msk                      _U_(0xF71F)                                          /**< (DMAC_INTPEND) Register Mask  */


/* -------- DMAC_INTSTATUS : (DMAC Offset: 0x24) ( R/ 32) Interrupt Status -------- */
#define DMAC_INTSTATUS_RESETVALUE             _U_(0x00)                                            /**<  (DMAC_INTSTATUS) Interrupt Status  Reset Value */

#define DMAC_INTSTATUS_CHINT0_Pos             _U_(0)                                               /**< (DMAC_INTSTATUS) Channel 0 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT0_Msk             (_U_(0x1) << DMAC_INTSTATUS_CHINT0_Pos)              /**< (DMAC_INTSTATUS) Channel 0 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT0(value)          (DMAC_INTSTATUS_CHINT0_Msk & ((value) << DMAC_INTSTATUS_CHINT0_Pos))
#define DMAC_INTSTATUS_CHINT1_Pos             _U_(1)                                               /**< (DMAC_INTSTATUS) Channel 1 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT1_Msk             (_U_(0x1) << DMAC_INTSTATUS_CHINT1_Pos)              /**< (DMAC_INTSTATUS) Channel 1 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT1(value)          (DMAC_INTSTATUS_CHINT1_Msk & ((value) << DMAC_INTSTATUS_CHINT1_Pos))
#define DMAC_INTSTATUS_CHINT2_Pos             _U_(2)                                               /**< (DMAC_INTSTATUS) Channel 2 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT2_Msk             (_U_(0x1) << DMAC_INTSTATUS_CHINT2_Pos)              /**< (DMAC_INTSTATUS) Channel 2 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT2(value)          (DMAC_INTSTATUS_CHINT2_Msk & ((value) << DMAC_INTSTATUS_CHINT2_Pos))
#define DMAC_INTSTATUS_CHINT3_Pos             _U_(3)                                               /**< (DMAC_INTSTATUS) Channel 3 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT3_Msk             (_U_(0x1) << DMAC_INTSTATUS_CHINT3_Pos)              /**< (DMAC_INTSTATUS) Channel 3 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT3(value)          (DMAC_INTSTATUS_CHINT3_Msk & ((value) << DMAC_INTSTATUS_CHINT3_Pos))
#define DMAC_INTSTATUS_CHINT4_Pos             _U_(4)                                               /**< (DMAC_INTSTATUS) Channel 4 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT4_Msk             (_U_(0x1) << DMAC_INTSTATUS_CHINT4_Pos)              /**< (DMAC_INTSTATUS) Channel 4 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT4(value)          (DMAC_INTSTATUS_CHINT4_Msk & ((value) << DMAC_INTSTATUS_CHINT4_Pos))
#define DMAC_INTSTATUS_CHINT5_Pos             _U_(5)                                               /**< (DMAC_INTSTATUS) Channel 5 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT5_Msk             (_U_(0x1) << DMAC_INTSTATUS_CHINT5_Pos)              /**< (DMAC_INTSTATUS) Channel 5 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT5(value)          (DMAC_INTSTATUS_CHINT5_Msk & ((value) << DMAC_INTSTATUS_CHINT5_Pos))
#define DMAC_INTSTATUS_CHINT6_Pos             _U_(6)                                               /**< (DMAC_INTSTATUS) Channel 6 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT6_Msk             (_U_(0x1) << DMAC_INTSTATUS_CHINT6_Pos)              /**< (DMAC_INTSTATUS) Channel 6 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT6(value)          (DMAC_INTSTATUS_CHINT6_Msk & ((value) << DMAC_INTSTATUS_CHINT6_Pos))
#define DMAC_INTSTATUS_CHINT7_Pos             _U_(7)                                               /**< (DMAC_INTSTATUS) Channel 7 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT7_Msk             (_U_(0x1) << DMAC_INTSTATUS_CHINT7_Pos)              /**< (DMAC_INTSTATUS) Channel 7 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT7(value)          (DMAC_INTSTATUS_CHINT7_Msk & ((value) << DMAC_INTSTATUS_CHINT7_Pos))
#define DMAC_INTSTATUS_CHINT8_Pos             _U_(8)                                               /**< (DMAC_INTSTATUS) Channel 8 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT8_Msk             (_U_(0x1) << DMAC_INTSTATUS_CHINT8_Pos)              /**< (DMAC_INTSTATUS) Channel 8 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT8(value)          (DMAC_INTSTATUS_CHINT8_Msk & ((value) << DMAC_INTSTATUS_CHINT8_Pos))
#define DMAC_INTSTATUS_CHINT9_Pos             _U_(9)                                               /**< (DMAC_INTSTATUS) Channel 9 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT9_Msk             (_U_(0x1) << DMAC_INTSTATUS_CHINT9_Pos)              /**< (DMAC_INTSTATUS) Channel 9 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT9(value)          (DMAC_INTSTATUS_CHINT9_Msk & ((value) << DMAC_INTSTATUS_CHINT9_Pos))
#define DMAC_INTSTATUS_CHINT10_Pos            _U_(10)                                              /**< (DMAC_INTSTATUS) Channel 10 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT10_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT10_Pos)             /**< (DMAC_INTSTATUS) Channel 10 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT10(value)         (DMAC_INTSTATUS_CHINT10_Msk & ((value) << DMAC_INTSTATUS_CHINT10_Pos))
#define DMAC_INTSTATUS_CHINT11_Pos            _U_(11)                                              /**< (DMAC_INTSTATUS) Channel 11 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT11_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT11_Pos)             /**< (DMAC_INTSTATUS) Channel 11 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT11(value)         (DMAC_INTSTATUS_CHINT11_Msk & ((value) << DMAC_INTSTATUS_CHINT11_Pos))
#define DMAC_INTSTATUS_CHINT12_Pos            _U_(12)                                              /**< (DMAC_INTSTATUS) Channel 12 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT12_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT12_Pos)             /**< (DMAC_INTSTATUS) Channel 12 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT12(value)         (DMAC_INTSTATUS_CHINT12_Msk & ((value) << DMAC_INTSTATUS_CHINT12_Pos))
#define DMAC_INTSTATUS_CHINT13_Pos            _U_(13)                                              /**< (DMAC_INTSTATUS) Channel 13 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT13_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT13_Pos)             /**< (DMAC_INTSTATUS) Channel 13 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT13(value)         (DMAC_INTSTATUS_CHINT13_Msk & ((value) << DMAC_INTSTATUS_CHINT13_Pos))
#define DMAC_INTSTATUS_CHINT14_Pos            _U_(14)                                              /**< (DMAC_INTSTATUS) Channel 14 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT14_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT14_Pos)             /**< (DMAC_INTSTATUS) Channel 14 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT14(value)         (DMAC_INTSTATUS_CHINT14_Msk & ((value) << DMAC_INTSTATUS_CHINT14_Pos))
#define DMAC_INTSTATUS_CHINT15_Pos            _U_(15)                                              /**< (DMAC_INTSTATUS) Channel 15 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT15_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT15_Pos)             /**< (DMAC_INTSTATUS) Channel 15 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT15(value)         (DMAC_INTSTATUS_CHINT15_Msk & ((value) << DMAC_INTSTATUS_CHINT15_Pos))
#define DMAC_INTSTATUS_CHINT16_Pos            _U_(16)                                              /**< (DMAC_INTSTATUS) Channel 16 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT16_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT16_Pos)             /**< (DMAC_INTSTATUS) Channel 16 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT16(value)         (DMAC_INTSTATUS_CHINT16_Msk & ((value) << DMAC_INTSTATUS_CHINT16_Pos))
#define DMAC_INTSTATUS_CHINT17_Pos            _U_(17)                                              /**< (DMAC_INTSTATUS) Channel 17 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT17_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT17_Pos)             /**< (DMAC_INTSTATUS) Channel 17 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT17(value)         (DMAC_INTSTATUS_CHINT17_Msk & ((value) << DMAC_INTSTATUS_CHINT17_Pos))
#define DMAC_INTSTATUS_CHINT18_Pos            _U_(18)                                              /**< (DMAC_INTSTATUS) Channel 18 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT18_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT18_Pos)             /**< (DMAC_INTSTATUS) Channel 18 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT18(value)         (DMAC_INTSTATUS_CHINT18_Msk & ((value) << DMAC_INTSTATUS_CHINT18_Pos))
#define DMAC_INTSTATUS_CHINT19_Pos            _U_(19)                                              /**< (DMAC_INTSTATUS) Channel 19 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT19_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT19_Pos)             /**< (DMAC_INTSTATUS) Channel 19 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT19(value)         (DMAC_INTSTATUS_CHINT19_Msk & ((value) << DMAC_INTSTATUS_CHINT19_Pos))
#define DMAC_INTSTATUS_CHINT20_Pos            _U_(20)                                              /**< (DMAC_INTSTATUS) Channel 20 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT20_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT20_Pos)             /**< (DMAC_INTSTATUS) Channel 20 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT20(value)         (DMAC_INTSTATUS_CHINT20_Msk & ((value) << DMAC_INTSTATUS_CHINT20_Pos))
#define DMAC_INTSTATUS_CHINT21_Pos            _U_(21)                                              /**< (DMAC_INTSTATUS) Channel 21 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT21_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT21_Pos)             /**< (DMAC_INTSTATUS) Channel 21 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT21(value)         (DMAC_INTSTATUS_CHINT21_Msk & ((value) << DMAC_INTSTATUS_CHINT21_Pos))
#define DMAC_INTSTATUS_CHINT22_Pos            _U_(22)                                              /**< (DMAC_INTSTATUS) Channel 22 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT22_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT22_Pos)             /**< (DMAC_INTSTATUS) Channel 22 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT22(value)         (DMAC_INTSTATUS_CHINT22_Msk & ((value) << DMAC_INTSTATUS_CHINT22_Pos))
#define DMAC_INTSTATUS_CHINT23_Pos            _U_(23)                                              /**< (DMAC_INTSTATUS) Channel 23 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT23_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT23_Pos)             /**< (DMAC_INTSTATUS) Channel 23 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT23(value)         (DMAC_INTSTATUS_CHINT23_Msk & ((value) << DMAC_INTSTATUS_CHINT23_Pos))
#define DMAC_INTSTATUS_CHINT24_Pos            _U_(24)                                              /**< (DMAC_INTSTATUS) Channel 24 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT24_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT24_Pos)             /**< (DMAC_INTSTATUS) Channel 24 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT24(value)         (DMAC_INTSTATUS_CHINT24_Msk & ((value) << DMAC_INTSTATUS_CHINT24_Pos))
#define DMAC_INTSTATUS_CHINT25_Pos            _U_(25)                                              /**< (DMAC_INTSTATUS) Channel 25 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT25_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT25_Pos)             /**< (DMAC_INTSTATUS) Channel 25 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT25(value)         (DMAC_INTSTATUS_CHINT25_Msk & ((value) << DMAC_INTSTATUS_CHINT25_Pos))
#define DMAC_INTSTATUS_CHINT26_Pos            _U_(26)                                              /**< (DMAC_INTSTATUS) Channel 26 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT26_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT26_Pos)             /**< (DMAC_INTSTATUS) Channel 26 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT26(value)         (DMAC_INTSTATUS_CHINT26_Msk & ((value) << DMAC_INTSTATUS_CHINT26_Pos))
#define DMAC_INTSTATUS_CHINT27_Pos            _U_(27)                                              /**< (DMAC_INTSTATUS) Channel 27 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT27_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT27_Pos)             /**< (DMAC_INTSTATUS) Channel 27 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT27(value)         (DMAC_INTSTATUS_CHINT27_Msk & ((value) << DMAC_INTSTATUS_CHINT27_Pos))
#define DMAC_INTSTATUS_CHINT28_Pos            _U_(28)                                              /**< (DMAC_INTSTATUS) Channel 28 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT28_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT28_Pos)             /**< (DMAC_INTSTATUS) Channel 28 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT28(value)         (DMAC_INTSTATUS_CHINT28_Msk & ((value) << DMAC_INTSTATUS_CHINT28_Pos))
#define DMAC_INTSTATUS_CHINT29_Pos            _U_(29)                                              /**< (DMAC_INTSTATUS) Channel 29 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT29_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT29_Pos)             /**< (DMAC_INTSTATUS) Channel 29 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT29(value)         (DMAC_INTSTATUS_CHINT29_Msk & ((value) << DMAC_INTSTATUS_CHINT29_Pos))
#define DMAC_INTSTATUS_CHINT30_Pos            _U_(30)                                              /**< (DMAC_INTSTATUS) Channel 30 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT30_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT30_Pos)             /**< (DMAC_INTSTATUS) Channel 30 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT30(value)         (DMAC_INTSTATUS_CHINT30_Msk & ((value) << DMAC_INTSTATUS_CHINT30_Pos))
#define DMAC_INTSTATUS_CHINT31_Pos            _U_(31)                                              /**< (DMAC_INTSTATUS) Channel 31 Pending Interrupt Position */
#define DMAC_INTSTATUS_CHINT31_Msk            (_U_(0x1) << DMAC_INTSTATUS_CHINT31_Pos)             /**< (DMAC_INTSTATUS) Channel 31 Pending Interrupt Mask */
#define DMAC_INTSTATUS_CHINT31(value)         (DMAC_INTSTATUS_CHINT31_Msk & ((value) << DMAC_INTSTATUS_CHINT31_Pos))
#define DMAC_INTSTATUS_Msk                    _U_(0xFFFFFFFF)                                      /**< (DMAC_INTSTATUS) Register Mask  */

#define DMAC_INTSTATUS_CHINT_Pos              _U_(0)                                               /**< (DMAC_INTSTATUS Position) Channel 3x Pending Interrupt */
#define DMAC_INTSTATUS_CHINT_Msk              (_U_(0xFFFFFFFF) << DMAC_INTSTATUS_CHINT_Pos)        /**< (DMAC_INTSTATUS Mask) CHINT */
#define DMAC_INTSTATUS_CHINT(value)           (DMAC_INTSTATUS_CHINT_Msk & ((value) << DMAC_INTSTATUS_CHINT_Pos)) 

/* -------- DMAC_BUSYCH : (DMAC Offset: 0x28) ( R/ 32) Busy Channels -------- */
#define DMAC_BUSYCH_RESETVALUE                _U_(0x00)                                            /**<  (DMAC_BUSYCH) Busy Channels  Reset Value */

#define DMAC_BUSYCH_BUSYCH0_Pos               _U_(0)                                               /**< (DMAC_BUSYCH) Busy Channel 0 Position */
#define DMAC_BUSYCH_BUSYCH0_Msk               (_U_(0x1) << DMAC_BUSYCH_BUSYCH0_Pos)                /**< (DMAC_BUSYCH) Busy Channel 0 Mask */
#define DMAC_BUSYCH_BUSYCH0(value)            (DMAC_BUSYCH_BUSYCH0_Msk & ((value) << DMAC_BUSYCH_BUSYCH0_Pos))
#define DMAC_BUSYCH_BUSYCH1_Pos               _U_(1)                                               /**< (DMAC_BUSYCH) Busy Channel 1 Position */
#define DMAC_BUSYCH_BUSYCH1_Msk               (_U_(0x1) << DMAC_BUSYCH_BUSYCH1_Pos)                /**< (DMAC_BUSYCH) Busy Channel 1 Mask */
#define DMAC_BUSYCH_BUSYCH1(value)            (DMAC_BUSYCH_BUSYCH1_Msk & ((value) << DMAC_BUSYCH_BUSYCH1_Pos))
#define DMAC_BUSYCH_BUSYCH2_Pos               _U_(2)                                               /**< (DMAC_BUSYCH) Busy Channel 2 Position */
#define DMAC_BUSYCH_BUSYCH2_Msk               (_U_(0x1) << DMAC_BUSYCH_BUSYCH2_Pos)                /**< (DMAC_BUSYCH) Busy Channel 2 Mask */
#define DMAC_BUSYCH_BUSYCH2(value)            (DMAC_BUSYCH_BUSYCH2_Msk & ((value) << DMAC_BUSYCH_BUSYCH2_Pos))
#define DMAC_BUSYCH_BUSYCH3_Pos               _U_(3)                                               /**< (DMAC_BUSYCH) Busy Channel 3 Position */
#define DMAC_BUSYCH_BUSYCH3_Msk               (_U_(0x1) << DMAC_BUSYCH_BUSYCH3_Pos)                /**< (DMAC_BUSYCH) Busy Channel 3 Mask */
#define DMAC_BUSYCH_BUSYCH3(value)            (DMAC_BUSYCH_BUSYCH3_Msk & ((value) << DMAC_BUSYCH_BUSYCH3_Pos))
#define DMAC_BUSYCH_BUSYCH4_Pos               _U_(4)                                               /**< (DMAC_BUSYCH) Busy Channel 4 Position */
#define DMAC_BUSYCH_BUSYCH4_Msk               (_U_(0x1) << DMAC_BUSYCH_BUSYCH4_Pos)                /**< (DMAC_BUSYCH) Busy Channel 4 Mask */
#define DMAC_BUSYCH_BUSYCH4(value)            (DMAC_BUSYCH_BUSYCH4_Msk & ((value) << DMAC_BUSYCH_BUSYCH4_Pos))
#define DMAC_BUSYCH_BUSYCH5_Pos               _U_(5)                                               /**< (DMAC_BUSYCH) Busy Channel 5 Position */
#define DMAC_BUSYCH_BUSYCH5_Msk               (_U_(0x1) << DMAC_BUSYCH_BUSYCH5_Pos)                /**< (DMAC_BUSYCH) Busy Channel 5 Mask */
#define DMAC_BUSYCH_BUSYCH5(value)            (DMAC_BUSYCH_BUSYCH5_Msk & ((value) << DMAC_BUSYCH_BUSYCH5_Pos))
#define DMAC_BUSYCH_BUSYCH6_Pos               _U_(6)                                               /**< (DMAC_BUSYCH) Busy Channel 6 Position */
#define DMAC_BUSYCH_BUSYCH6_Msk               (_U_(0x1) << DMAC_BUSYCH_BUSYCH6_Pos)                /**< (DMAC_BUSYCH) Busy Channel 6 Mask */
#define DMAC_BUSYCH_BUSYCH6(value)            (DMAC_BUSYCH_BUSYCH6_Msk & ((value) << DMAC_BUSYCH_BUSYCH6_Pos))
#define DMAC_BUSYCH_BUSYCH7_Pos               _U_(7)                                               /**< (DMAC_BUSYCH) Busy Channel 7 Position */
#define DMAC_BUSYCH_BUSYCH7_Msk               (_U_(0x1) << DMAC_BUSYCH_BUSYCH7_Pos)                /**< (DMAC_BUSYCH) Busy Channel 7 Mask */
#define DMAC_BUSYCH_BUSYCH7(value)            (DMAC_BUSYCH_BUSYCH7_Msk & ((value) << DMAC_BUSYCH_BUSYCH7_Pos))
#define DMAC_BUSYCH_BUSYCH8_Pos               _U_(8)                                               /**< (DMAC_BUSYCH) Busy Channel 8 Position */
#define DMAC_BUSYCH_BUSYCH8_Msk               (_U_(0x1) << DMAC_BUSYCH_BUSYCH8_Pos)                /**< (DMAC_BUSYCH) Busy Channel 8 Mask */
#define DMAC_BUSYCH_BUSYCH8(value)            (DMAC_BUSYCH_BUSYCH8_Msk & ((value) << DMAC_BUSYCH_BUSYCH8_Pos))
#define DMAC_BUSYCH_BUSYCH9_Pos               _U_(9)                                               /**< (DMAC_BUSYCH) Busy Channel 9 Position */
#define DMAC_BUSYCH_BUSYCH9_Msk               (_U_(0x1) << DMAC_BUSYCH_BUSYCH9_Pos)                /**< (DMAC_BUSYCH) Busy Channel 9 Mask */
#define DMAC_BUSYCH_BUSYCH9(value)            (DMAC_BUSYCH_BUSYCH9_Msk & ((value) << DMAC_BUSYCH_BUSYCH9_Pos))
#define DMAC_BUSYCH_BUSYCH10_Pos              _U_(10)                                              /**< (DMAC_BUSYCH) Busy Channel 10 Position */
#define DMAC_BUSYCH_BUSYCH10_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH10_Pos)               /**< (DMAC_BUSYCH) Busy Channel 10 Mask */
#define DMAC_BUSYCH_BUSYCH10(value)           (DMAC_BUSYCH_BUSYCH10_Msk & ((value) << DMAC_BUSYCH_BUSYCH10_Pos))
#define DMAC_BUSYCH_BUSYCH11_Pos              _U_(11)                                              /**< (DMAC_BUSYCH) Busy Channel 11 Position */
#define DMAC_BUSYCH_BUSYCH11_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH11_Pos)               /**< (DMAC_BUSYCH) Busy Channel 11 Mask */
#define DMAC_BUSYCH_BUSYCH11(value)           (DMAC_BUSYCH_BUSYCH11_Msk & ((value) << DMAC_BUSYCH_BUSYCH11_Pos))
#define DMAC_BUSYCH_BUSYCH12_Pos              _U_(12)                                              /**< (DMAC_BUSYCH) Busy Channel 12 Position */
#define DMAC_BUSYCH_BUSYCH12_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH12_Pos)               /**< (DMAC_BUSYCH) Busy Channel 12 Mask */
#define DMAC_BUSYCH_BUSYCH12(value)           (DMAC_BUSYCH_BUSYCH12_Msk & ((value) << DMAC_BUSYCH_BUSYCH12_Pos))
#define DMAC_BUSYCH_BUSYCH13_Pos              _U_(13)                                              /**< (DMAC_BUSYCH) Busy Channel 13 Position */
#define DMAC_BUSYCH_BUSYCH13_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH13_Pos)               /**< (DMAC_BUSYCH) Busy Channel 13 Mask */
#define DMAC_BUSYCH_BUSYCH13(value)           (DMAC_BUSYCH_BUSYCH13_Msk & ((value) << DMAC_BUSYCH_BUSYCH13_Pos))
#define DMAC_BUSYCH_BUSYCH14_Pos              _U_(14)                                              /**< (DMAC_BUSYCH) Busy Channel 14 Position */
#define DMAC_BUSYCH_BUSYCH14_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH14_Pos)               /**< (DMAC_BUSYCH) Busy Channel 14 Mask */
#define DMAC_BUSYCH_BUSYCH14(value)           (DMAC_BUSYCH_BUSYCH14_Msk & ((value) << DMAC_BUSYCH_BUSYCH14_Pos))
#define DMAC_BUSYCH_BUSYCH15_Pos              _U_(15)                                              /**< (DMAC_BUSYCH) Busy Channel 15 Position */
#define DMAC_BUSYCH_BUSYCH15_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH15_Pos)               /**< (DMAC_BUSYCH) Busy Channel 15 Mask */
#define DMAC_BUSYCH_BUSYCH15(value)           (DMAC_BUSYCH_BUSYCH15_Msk & ((value) << DMAC_BUSYCH_BUSYCH15_Pos))
#define DMAC_BUSYCH_BUSYCH16_Pos              _U_(16)                                              /**< (DMAC_BUSYCH) Busy Channel 16 Position */
#define DMAC_BUSYCH_BUSYCH16_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH16_Pos)               /**< (DMAC_BUSYCH) Busy Channel 16 Mask */
#define DMAC_BUSYCH_BUSYCH16(value)           (DMAC_BUSYCH_BUSYCH16_Msk & ((value) << DMAC_BUSYCH_BUSYCH16_Pos))
#define DMAC_BUSYCH_BUSYCH17_Pos              _U_(17)                                              /**< (DMAC_BUSYCH) Busy Channel 17 Position */
#define DMAC_BUSYCH_BUSYCH17_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH17_Pos)               /**< (DMAC_BUSYCH) Busy Channel 17 Mask */
#define DMAC_BUSYCH_BUSYCH17(value)           (DMAC_BUSYCH_BUSYCH17_Msk & ((value) << DMAC_BUSYCH_BUSYCH17_Pos))
#define DMAC_BUSYCH_BUSYCH18_Pos              _U_(18)                                              /**< (DMAC_BUSYCH) Busy Channel 18 Position */
#define DMAC_BUSYCH_BUSYCH18_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH18_Pos)               /**< (DMAC_BUSYCH) Busy Channel 18 Mask */
#define DMAC_BUSYCH_BUSYCH18(value)           (DMAC_BUSYCH_BUSYCH18_Msk & ((value) << DMAC_BUSYCH_BUSYCH18_Pos))
#define DMAC_BUSYCH_BUSYCH19_Pos              _U_(19)                                              /**< (DMAC_BUSYCH) Busy Channel 19 Position */
#define DMAC_BUSYCH_BUSYCH19_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH19_Pos)               /**< (DMAC_BUSYCH) Busy Channel 19 Mask */
#define DMAC_BUSYCH_BUSYCH19(value)           (DMAC_BUSYCH_BUSYCH19_Msk & ((value) << DMAC_BUSYCH_BUSYCH19_Pos))
#define DMAC_BUSYCH_BUSYCH20_Pos              _U_(20)                                              /**< (DMAC_BUSYCH) Busy Channel 20 Position */
#define DMAC_BUSYCH_BUSYCH20_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH20_Pos)               /**< (DMAC_BUSYCH) Busy Channel 20 Mask */
#define DMAC_BUSYCH_BUSYCH20(value)           (DMAC_BUSYCH_BUSYCH20_Msk & ((value) << DMAC_BUSYCH_BUSYCH20_Pos))
#define DMAC_BUSYCH_BUSYCH21_Pos              _U_(21)                                              /**< (DMAC_BUSYCH) Busy Channel 21 Position */
#define DMAC_BUSYCH_BUSYCH21_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH21_Pos)               /**< (DMAC_BUSYCH) Busy Channel 21 Mask */
#define DMAC_BUSYCH_BUSYCH21(value)           (DMAC_BUSYCH_BUSYCH21_Msk & ((value) << DMAC_BUSYCH_BUSYCH21_Pos))
#define DMAC_BUSYCH_BUSYCH22_Pos              _U_(22)                                              /**< (DMAC_BUSYCH) Busy Channel 22 Position */
#define DMAC_BUSYCH_BUSYCH22_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH22_Pos)               /**< (DMAC_BUSYCH) Busy Channel 22 Mask */
#define DMAC_BUSYCH_BUSYCH22(value)           (DMAC_BUSYCH_BUSYCH22_Msk & ((value) << DMAC_BUSYCH_BUSYCH22_Pos))
#define DMAC_BUSYCH_BUSYCH23_Pos              _U_(23)                                              /**< (DMAC_BUSYCH) Busy Channel 23 Position */
#define DMAC_BUSYCH_BUSYCH23_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH23_Pos)               /**< (DMAC_BUSYCH) Busy Channel 23 Mask */
#define DMAC_BUSYCH_BUSYCH23(value)           (DMAC_BUSYCH_BUSYCH23_Msk & ((value) << DMAC_BUSYCH_BUSYCH23_Pos))
#define DMAC_BUSYCH_BUSYCH24_Pos              _U_(24)                                              /**< (DMAC_BUSYCH) Busy Channel 24 Position */
#define DMAC_BUSYCH_BUSYCH24_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH24_Pos)               /**< (DMAC_BUSYCH) Busy Channel 24 Mask */
#define DMAC_BUSYCH_BUSYCH24(value)           (DMAC_BUSYCH_BUSYCH24_Msk & ((value) << DMAC_BUSYCH_BUSYCH24_Pos))
#define DMAC_BUSYCH_BUSYCH25_Pos              _U_(25)                                              /**< (DMAC_BUSYCH) Busy Channel 25 Position */
#define DMAC_BUSYCH_BUSYCH25_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH25_Pos)               /**< (DMAC_BUSYCH) Busy Channel 25 Mask */
#define DMAC_BUSYCH_BUSYCH25(value)           (DMAC_BUSYCH_BUSYCH25_Msk & ((value) << DMAC_BUSYCH_BUSYCH25_Pos))
#define DMAC_BUSYCH_BUSYCH26_Pos              _U_(26)                                              /**< (DMAC_BUSYCH) Busy Channel 26 Position */
#define DMAC_BUSYCH_BUSYCH26_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH26_Pos)               /**< (DMAC_BUSYCH) Busy Channel 26 Mask */
#define DMAC_BUSYCH_BUSYCH26(value)           (DMAC_BUSYCH_BUSYCH26_Msk & ((value) << DMAC_BUSYCH_BUSYCH26_Pos))
#define DMAC_BUSYCH_BUSYCH27_Pos              _U_(27)                                              /**< (DMAC_BUSYCH) Busy Channel 27 Position */
#define DMAC_BUSYCH_BUSYCH27_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH27_Pos)               /**< (DMAC_BUSYCH) Busy Channel 27 Mask */
#define DMAC_BUSYCH_BUSYCH27(value)           (DMAC_BUSYCH_BUSYCH27_Msk & ((value) << DMAC_BUSYCH_BUSYCH27_Pos))
#define DMAC_BUSYCH_BUSYCH28_Pos              _U_(28)                                              /**< (DMAC_BUSYCH) Busy Channel 28 Position */
#define DMAC_BUSYCH_BUSYCH28_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH28_Pos)               /**< (DMAC_BUSYCH) Busy Channel 28 Mask */
#define DMAC_BUSYCH_BUSYCH28(value)           (DMAC_BUSYCH_BUSYCH28_Msk & ((value) << DMAC_BUSYCH_BUSYCH28_Pos))
#define DMAC_BUSYCH_BUSYCH29_Pos              _U_(29)                                              /**< (DMAC_BUSYCH) Busy Channel 29 Position */
#define DMAC_BUSYCH_BUSYCH29_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH29_Pos)               /**< (DMAC_BUSYCH) Busy Channel 29 Mask */
#define DMAC_BUSYCH_BUSYCH29(value)           (DMAC_BUSYCH_BUSYCH29_Msk & ((value) << DMAC_BUSYCH_BUSYCH29_Pos))
#define DMAC_BUSYCH_BUSYCH30_Pos              _U_(30)                                              /**< (DMAC_BUSYCH) Busy Channel 30 Position */
#define DMAC_BUSYCH_BUSYCH30_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH30_Pos)               /**< (DMAC_BUSYCH) Busy Channel 30 Mask */
#define DMAC_BUSYCH_BUSYCH30(value)           (DMAC_BUSYCH_BUSYCH30_Msk & ((value) << DMAC_BUSYCH_BUSYCH30_Pos))
#define DMAC_BUSYCH_BUSYCH31_Pos              _U_(31)                                              /**< (DMAC_BUSYCH) Busy Channel 31 Position */
#define DMAC_BUSYCH_BUSYCH31_Msk              (_U_(0x1) << DMAC_BUSYCH_BUSYCH31_Pos)               /**< (DMAC_BUSYCH) Busy Channel 31 Mask */
#define DMAC_BUSYCH_BUSYCH31(value)           (DMAC_BUSYCH_BUSYCH31_Msk & ((value) << DMAC_BUSYCH_BUSYCH31_Pos))
#define DMAC_BUSYCH_Msk                       _U_(0xFFFFFFFF)                                      /**< (DMAC_BUSYCH) Register Mask  */

#define DMAC_BUSYCH_BUSYCH_Pos                _U_(0)                                               /**< (DMAC_BUSYCH Position) Busy Channel 3x */
#define DMAC_BUSYCH_BUSYCH_Msk                (_U_(0xFFFFFFFF) << DMAC_BUSYCH_BUSYCH_Pos)          /**< (DMAC_BUSYCH Mask) BUSYCH */
#define DMAC_BUSYCH_BUSYCH(value)             (DMAC_BUSYCH_BUSYCH_Msk & ((value) << DMAC_BUSYCH_BUSYCH_Pos)) 

/* -------- DMAC_PENDCH : (DMAC Offset: 0x2C) ( R/ 32) Pending Channels -------- */
#define DMAC_PENDCH_RESETVALUE                _U_(0x00)                                            /**<  (DMAC_PENDCH) Pending Channels  Reset Value */

#define DMAC_PENDCH_PENDCH0_Pos               _U_(0)                                               /**< (DMAC_PENDCH) Pending Channel 0 Position */
#define DMAC_PENDCH_PENDCH0_Msk               (_U_(0x1) << DMAC_PENDCH_PENDCH0_Pos)                /**< (DMAC_PENDCH) Pending Channel 0 Mask */
#define DMAC_PENDCH_PENDCH0(value)            (DMAC_PENDCH_PENDCH0_Msk & ((value) << DMAC_PENDCH_PENDCH0_Pos))
#define DMAC_PENDCH_PENDCH1_Pos               _U_(1)                                               /**< (DMAC_PENDCH) Pending Channel 1 Position */
#define DMAC_PENDCH_PENDCH1_Msk               (_U_(0x1) << DMAC_PENDCH_PENDCH1_Pos)                /**< (DMAC_PENDCH) Pending Channel 1 Mask */
#define DMAC_PENDCH_PENDCH1(value)            (DMAC_PENDCH_PENDCH1_Msk & ((value) << DMAC_PENDCH_PENDCH1_Pos))
#define DMAC_PENDCH_PENDCH2_Pos               _U_(2)                                               /**< (DMAC_PENDCH) Pending Channel 2 Position */
#define DMAC_PENDCH_PENDCH2_Msk               (_U_(0x1) << DMAC_PENDCH_PENDCH2_Pos)                /**< (DMAC_PENDCH) Pending Channel 2 Mask */
#define DMAC_PENDCH_PENDCH2(value)            (DMAC_PENDCH_PENDCH2_Msk & ((value) << DMAC_PENDCH_PENDCH2_Pos))
#define DMAC_PENDCH_PENDCH3_Pos               _U_(3)                                               /**< (DMAC_PENDCH) Pending Channel 3 Position */
#define DMAC_PENDCH_PENDCH3_Msk               (_U_(0x1) << DMAC_PENDCH_PENDCH3_Pos)                /**< (DMAC_PENDCH) Pending Channel 3 Mask */
#define DMAC_PENDCH_PENDCH3(value)            (DMAC_PENDCH_PENDCH3_Msk & ((value) << DMAC_PENDCH_PENDCH3_Pos))
#define DMAC_PENDCH_PENDCH4_Pos               _U_(4)                                               /**< (DMAC_PENDCH) Pending Channel 4 Position */
#define DMAC_PENDCH_PENDCH4_Msk               (_U_(0x1) << DMAC_PENDCH_PENDCH4_Pos)                /**< (DMAC_PENDCH) Pending Channel 4 Mask */
#define DMAC_PENDCH_PENDCH4(value)            (DMAC_PENDCH_PENDCH4_Msk & ((value) << DMAC_PENDCH_PENDCH4_Pos))
#define DMAC_PENDCH_PENDCH5_Pos               _U_(5)                                               /**< (DMAC_PENDCH) Pending Channel 5 Position */
#define DMAC_PENDCH_PENDCH5_Msk               (_U_(0x1) << DMAC_PENDCH_PENDCH5_Pos)                /**< (DMAC_PENDCH) Pending Channel 5 Mask */
#define DMAC_PENDCH_PENDCH5(value)            (DMAC_PENDCH_PENDCH5_Msk & ((value) << DMAC_PENDCH_PENDCH5_Pos))
#define DMAC_PENDCH_PENDCH6_Pos               _U_(6)                                               /**< (DMAC_PENDCH) Pending Channel 6 Position */
#define DMAC_PENDCH_PENDCH6_Msk               (_U_(0x1) << DMAC_PENDCH_PENDCH6_Pos)                /**< (DMAC_PENDCH) Pending Channel 6 Mask */
#define DMAC_PENDCH_PENDCH6(value)            (DMAC_PENDCH_PENDCH6_Msk & ((value) << DMAC_PENDCH_PENDCH6_Pos))
#define DMAC_PENDCH_PENDCH7_Pos               _U_(7)                                               /**< (DMAC_PENDCH) Pending Channel 7 Position */
#define DMAC_PENDCH_PENDCH7_Msk               (_U_(0x1) << DMAC_PENDCH_PENDCH7_Pos)                /**< (DMAC_PENDCH) Pending Channel 7 Mask */
#define DMAC_PENDCH_PENDCH7(value)            (DMAC_PENDCH_PENDCH7_Msk & ((value) << DMAC_PENDCH_PENDCH7_Pos))
#define DMAC_PENDCH_PENDCH8_Pos               _U_(8)                                               /**< (DMAC_PENDCH) Pending Channel 8 Position */
#define DMAC_PENDCH_PENDCH8_Msk               (_U_(0x1) << DMAC_PENDCH_PENDCH8_Pos)                /**< (DMAC_PENDCH) Pending Channel 8 Mask */
#define DMAC_PENDCH_PENDCH8(value)            (DMAC_PENDCH_PENDCH8_Msk & ((value) << DMAC_PENDCH_PENDCH8_Pos))
#define DMAC_PENDCH_PENDCH9_Pos               _U_(9)                                               /**< (DMAC_PENDCH) Pending Channel 9 Position */
#define DMAC_PENDCH_PENDCH9_Msk               (_U_(0x1) << DMAC_PENDCH_PENDCH9_Pos)                /**< (DMAC_PENDCH) Pending Channel 9 Mask */
#define DMAC_PENDCH_PENDCH9(value)            (DMAC_PENDCH_PENDCH9_Msk & ((value) << DMAC_PENDCH_PENDCH9_Pos))
#define DMAC_PENDCH_PENDCH10_Pos              _U_(10)                                              /**< (DMAC_PENDCH) Pending Channel 10 Position */
#define DMAC_PENDCH_PENDCH10_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH10_Pos)               /**< (DMAC_PENDCH) Pending Channel 10 Mask */
#define DMAC_PENDCH_PENDCH10(value)           (DMAC_PENDCH_PENDCH10_Msk & ((value) << DMAC_PENDCH_PENDCH10_Pos))
#define DMAC_PENDCH_PENDCH11_Pos              _U_(11)                                              /**< (DMAC_PENDCH) Pending Channel 11 Position */
#define DMAC_PENDCH_PENDCH11_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH11_Pos)               /**< (DMAC_PENDCH) Pending Channel 11 Mask */
#define DMAC_PENDCH_PENDCH11(value)           (DMAC_PENDCH_PENDCH11_Msk & ((value) << DMAC_PENDCH_PENDCH11_Pos))
#define DMAC_PENDCH_PENDCH12_Pos              _U_(12)                                              /**< (DMAC_PENDCH) Pending Channel 12 Position */
#define DMAC_PENDCH_PENDCH12_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH12_Pos)               /**< (DMAC_PENDCH) Pending Channel 12 Mask */
#define DMAC_PENDCH_PENDCH12(value)           (DMAC_PENDCH_PENDCH12_Msk & ((value) << DMAC_PENDCH_PENDCH12_Pos))
#define DMAC_PENDCH_PENDCH13_Pos              _U_(13)                                              /**< (DMAC_PENDCH) Pending Channel 13 Position */
#define DMAC_PENDCH_PENDCH13_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH13_Pos)               /**< (DMAC_PENDCH) Pending Channel 13 Mask */
#define DMAC_PENDCH_PENDCH13(value)           (DMAC_PENDCH_PENDCH13_Msk & ((value) << DMAC_PENDCH_PENDCH13_Pos))
#define DMAC_PENDCH_PENDCH14_Pos              _U_(14)                                              /**< (DMAC_PENDCH) Pending Channel 14 Position */
#define DMAC_PENDCH_PENDCH14_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH14_Pos)               /**< (DMAC_PENDCH) Pending Channel 14 Mask */
#define DMAC_PENDCH_PENDCH14(value)           (DMAC_PENDCH_PENDCH14_Msk & ((value) << DMAC_PENDCH_PENDCH14_Pos))
#define DMAC_PENDCH_PENDCH15_Pos              _U_(15)                                              /**< (DMAC_PENDCH) Pending Channel 15 Position */
#define DMAC_PENDCH_PENDCH15_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH15_Pos)               /**< (DMAC_PENDCH) Pending Channel 15 Mask */
#define DMAC_PENDCH_PENDCH15(value)           (DMAC_PENDCH_PENDCH15_Msk & ((value) << DMAC_PENDCH_PENDCH15_Pos))
#define DMAC_PENDCH_PENDCH16_Pos              _U_(16)                                              /**< (DMAC_PENDCH) Pending Channel 16 Position */
#define DMAC_PENDCH_PENDCH16_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH16_Pos)               /**< (DMAC_PENDCH) Pending Channel 16 Mask */
#define DMAC_PENDCH_PENDCH16(value)           (DMAC_PENDCH_PENDCH16_Msk & ((value) << DMAC_PENDCH_PENDCH16_Pos))
#define DMAC_PENDCH_PENDCH17_Pos              _U_(17)                                              /**< (DMAC_PENDCH) Pending Channel 17 Position */
#define DMAC_PENDCH_PENDCH17_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH17_Pos)               /**< (DMAC_PENDCH) Pending Channel 17 Mask */
#define DMAC_PENDCH_PENDCH17(value)           (DMAC_PENDCH_PENDCH17_Msk & ((value) << DMAC_PENDCH_PENDCH17_Pos))
#define DMAC_PENDCH_PENDCH18_Pos              _U_(18)                                              /**< (DMAC_PENDCH) Pending Channel 18 Position */
#define DMAC_PENDCH_PENDCH18_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH18_Pos)               /**< (DMAC_PENDCH) Pending Channel 18 Mask */
#define DMAC_PENDCH_PENDCH18(value)           (DMAC_PENDCH_PENDCH18_Msk & ((value) << DMAC_PENDCH_PENDCH18_Pos))
#define DMAC_PENDCH_PENDCH19_Pos              _U_(19)                                              /**< (DMAC_PENDCH) Pending Channel 19 Position */
#define DMAC_PENDCH_PENDCH19_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH19_Pos)               /**< (DMAC_PENDCH) Pending Channel 19 Mask */
#define DMAC_PENDCH_PENDCH19(value)           (DMAC_PENDCH_PENDCH19_Msk & ((value) << DMAC_PENDCH_PENDCH19_Pos))
#define DMAC_PENDCH_PENDCH20_Pos              _U_(20)                                              /**< (DMAC_PENDCH) Pending Channel 20 Position */
#define DMAC_PENDCH_PENDCH20_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH20_Pos)               /**< (DMAC_PENDCH) Pending Channel 20 Mask */
#define DMAC_PENDCH_PENDCH20(value)           (DMAC_PENDCH_PENDCH20_Msk & ((value) << DMAC_PENDCH_PENDCH20_Pos))
#define DMAC_PENDCH_PENDCH21_Pos              _U_(21)                                              /**< (DMAC_PENDCH) Pending Channel 21 Position */
#define DMAC_PENDCH_PENDCH21_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH21_Pos)               /**< (DMAC_PENDCH) Pending Channel 21 Mask */
#define DMAC_PENDCH_PENDCH21(value)           (DMAC_PENDCH_PENDCH21_Msk & ((value) << DMAC_PENDCH_PENDCH21_Pos))
#define DMAC_PENDCH_PENDCH22_Pos              _U_(22)                                              /**< (DMAC_PENDCH) Pending Channel 22 Position */
#define DMAC_PENDCH_PENDCH22_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH22_Pos)               /**< (DMAC_PENDCH) Pending Channel 22 Mask */
#define DMAC_PENDCH_PENDCH22(value)           (DMAC_PENDCH_PENDCH22_Msk & ((value) << DMAC_PENDCH_PENDCH22_Pos))
#define DMAC_PENDCH_PENDCH23_Pos              _U_(23)                                              /**< (DMAC_PENDCH) Pending Channel 23 Position */
#define DMAC_PENDCH_PENDCH23_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH23_Pos)               /**< (DMAC_PENDCH) Pending Channel 23 Mask */
#define DMAC_PENDCH_PENDCH23(value)           (DMAC_PENDCH_PENDCH23_Msk & ((value) << DMAC_PENDCH_PENDCH23_Pos))
#define DMAC_PENDCH_PENDCH24_Pos              _U_(24)                                              /**< (DMAC_PENDCH) Pending Channel 24 Position */
#define DMAC_PENDCH_PENDCH24_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH24_Pos)               /**< (DMAC_PENDCH) Pending Channel 24 Mask */
#define DMAC_PENDCH_PENDCH24(value)           (DMAC_PENDCH_PENDCH24_Msk & ((value) << DMAC_PENDCH_PENDCH24_Pos))
#define DMAC_PENDCH_PENDCH25_Pos              _U_(25)                                              /**< (DMAC_PENDCH) Pending Channel 25 Position */
#define DMAC_PENDCH_PENDCH25_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH25_Pos)               /**< (DMAC_PENDCH) Pending Channel 25 Mask */
#define DMAC_PENDCH_PENDCH25(value)           (DMAC_PENDCH_PENDCH25_Msk & ((value) << DMAC_PENDCH_PENDCH25_Pos))
#define DMAC_PENDCH_PENDCH26_Pos              _U_(26)                                              /**< (DMAC_PENDCH) Pending Channel 26 Position */
#define DMAC_PENDCH_PENDCH26_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH26_Pos)               /**< (DMAC_PENDCH) Pending Channel 26 Mask */
#define DMAC_PENDCH_PENDCH26(value)           (DMAC_PENDCH_PENDCH26_Msk & ((value) << DMAC_PENDCH_PENDCH26_Pos))
#define DMAC_PENDCH_PENDCH27_Pos              _U_(27)                                              /**< (DMAC_PENDCH) Pending Channel 27 Position */
#define DMAC_PENDCH_PENDCH27_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH27_Pos)               /**< (DMAC_PENDCH) Pending Channel 27 Mask */
#define DMAC_PENDCH_PENDCH27(value)           (DMAC_PENDCH_PENDCH27_Msk & ((value) << DMAC_PENDCH_PENDCH27_Pos))
#define DMAC_PENDCH_PENDCH28_Pos              _U_(28)                                              /**< (DMAC_PENDCH) Pending Channel 28 Position */
#define DMAC_PENDCH_PENDCH28_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH28_Pos)               /**< (DMAC_PENDCH) Pending Channel 28 Mask */
#define DMAC_PENDCH_PENDCH28(value)           (DMAC_PENDCH_PENDCH28_Msk & ((value) << DMAC_PENDCH_PENDCH28_Pos))
#define DMAC_PENDCH_PENDCH29_Pos              _U_(29)                                              /**< (DMAC_PENDCH) Pending Channel 29 Position */
#define DMAC_PENDCH_PENDCH29_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH29_Pos)               /**< (DMAC_PENDCH) Pending Channel 29 Mask */
#define DMAC_PENDCH_PENDCH29(value)           (DMAC_PENDCH_PENDCH29_Msk & ((value) << DMAC_PENDCH_PENDCH29_Pos))
#define DMAC_PENDCH_PENDCH30_Pos              _U_(30)                                              /**< (DMAC_PENDCH) Pending Channel 30 Position */
#define DMAC_PENDCH_PENDCH30_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH30_Pos)               /**< (DMAC_PENDCH) Pending Channel 30 Mask */
#define DMAC_PENDCH_PENDCH30(value)           (DMAC_PENDCH_PENDCH30_Msk & ((value) << DMAC_PENDCH_PENDCH30_Pos))
#define DMAC_PENDCH_PENDCH31_Pos              _U_(31)                                              /**< (DMAC_PENDCH) Pending Channel 31 Position */
#define DMAC_PENDCH_PENDCH31_Msk              (_U_(0x1) << DMAC_PENDCH_PENDCH31_Pos)               /**< (DMAC_PENDCH) Pending Channel 31 Mask */
#define DMAC_PENDCH_PENDCH31(value)           (DMAC_PENDCH_PENDCH31_Msk & ((value) << DMAC_PENDCH_PENDCH31_Pos))
#define DMAC_PENDCH_Msk                       _U_(0xFFFFFFFF)                                      /**< (DMAC_PENDCH) Register Mask  */

#define DMAC_PENDCH_PENDCH_Pos                _U_(0)                                               /**< (DMAC_PENDCH Position) Pending Channel 3x */
#define DMAC_PENDCH_PENDCH_Msk                (_U_(0xFFFFFFFF) << DMAC_PENDCH_PENDCH_Pos)          /**< (DMAC_PENDCH Mask) PENDCH */
#define DMAC_PENDCH_PENDCH(value)             (DMAC_PENDCH_PENDCH_Msk & ((value) << DMAC_PENDCH_PENDCH_Pos)) 

/* -------- DMAC_ACTIVE : (DMAC Offset: 0x30) ( R/ 32) Active Channel and Levels -------- */
#define DMAC_ACTIVE_RESETVALUE                _U_(0x00)                                            /**<  (DMAC_ACTIVE) Active Channel and Levels  Reset Value */

#define DMAC_ACTIVE_LVLEX0_Pos                _U_(0)                                               /**< (DMAC_ACTIVE) Level 0 Channel Trigger Request Executing Position */
#define DMAC_ACTIVE_LVLEX0_Msk                (_U_(0x1) << DMAC_ACTIVE_LVLEX0_Pos)                 /**< (DMAC_ACTIVE) Level 0 Channel Trigger Request Executing Mask */
#define DMAC_ACTIVE_LVLEX0(value)             (DMAC_ACTIVE_LVLEX0_Msk & ((value) << DMAC_ACTIVE_LVLEX0_Pos))
#define DMAC_ACTIVE_LVLEX1_Pos                _U_(1)                                               /**< (DMAC_ACTIVE) Level 1 Channel Trigger Request Executing Position */
#define DMAC_ACTIVE_LVLEX1_Msk                (_U_(0x1) << DMAC_ACTIVE_LVLEX1_Pos)                 /**< (DMAC_ACTIVE) Level 1 Channel Trigger Request Executing Mask */
#define DMAC_ACTIVE_LVLEX1(value)             (DMAC_ACTIVE_LVLEX1_Msk & ((value) << DMAC_ACTIVE_LVLEX1_Pos))
#define DMAC_ACTIVE_LVLEX2_Pos                _U_(2)                                               /**< (DMAC_ACTIVE) Level 2 Channel Trigger Request Executing Position */
#define DMAC_ACTIVE_LVLEX2_Msk                (_U_(0x1) << DMAC_ACTIVE_LVLEX2_Pos)                 /**< (DMAC_ACTIVE) Level 2 Channel Trigger Request Executing Mask */
#define DMAC_ACTIVE_LVLEX2(value)             (DMAC_ACTIVE_LVLEX2_Msk & ((value) << DMAC_ACTIVE_LVLEX2_Pos))
#define DMAC_ACTIVE_LVLEX3_Pos                _U_(3)                                               /**< (DMAC_ACTIVE) Level 3 Channel Trigger Request Executing Position */
#define DMAC_ACTIVE_LVLEX3_Msk                (_U_(0x1) << DMAC_ACTIVE_LVLEX3_Pos)                 /**< (DMAC_ACTIVE) Level 3 Channel Trigger Request Executing Mask */
#define DMAC_ACTIVE_LVLEX3(value)             (DMAC_ACTIVE_LVLEX3_Msk & ((value) << DMAC_ACTIVE_LVLEX3_Pos))
#define DMAC_ACTIVE_ID_Pos                    _U_(8)                                               /**< (DMAC_ACTIVE) Active Channel ID Position */
#define DMAC_ACTIVE_ID_Msk                    (_U_(0x1F) << DMAC_ACTIVE_ID_Pos)                    /**< (DMAC_ACTIVE) Active Channel ID Mask */
#define DMAC_ACTIVE_ID(value)                 (DMAC_ACTIVE_ID_Msk & ((value) << DMAC_ACTIVE_ID_Pos))
#define DMAC_ACTIVE_ABUSY_Pos                 _U_(15)                                              /**< (DMAC_ACTIVE) Active Channel Busy Position */
#define DMAC_ACTIVE_ABUSY_Msk                 (_U_(0x1) << DMAC_ACTIVE_ABUSY_Pos)                  /**< (DMAC_ACTIVE) Active Channel Busy Mask */
#define DMAC_ACTIVE_ABUSY(value)              (DMAC_ACTIVE_ABUSY_Msk & ((value) << DMAC_ACTIVE_ABUSY_Pos))
#define DMAC_ACTIVE_BTCNT_Pos                 _U_(16)                                              /**< (DMAC_ACTIVE) Active Channel Block Transfer Count Position */
#define DMAC_ACTIVE_BTCNT_Msk                 (_U_(0xFFFF) << DMAC_ACTIVE_BTCNT_Pos)               /**< (DMAC_ACTIVE) Active Channel Block Transfer Count Mask */
#define DMAC_ACTIVE_BTCNT(value)              (DMAC_ACTIVE_BTCNT_Msk & ((value) << DMAC_ACTIVE_BTCNT_Pos))
#define DMAC_ACTIVE_Msk                       _U_(0xFFFF9F0F)                                      /**< (DMAC_ACTIVE) Register Mask  */

#define DMAC_ACTIVE_LVLEX_Pos                 _U_(0)                                               /**< (DMAC_ACTIVE Position) Level x Channel Trigger Request Executing */
#define DMAC_ACTIVE_LVLEX_Msk                 (_U_(0xF) << DMAC_ACTIVE_LVLEX_Pos)                  /**< (DMAC_ACTIVE Mask) LVLEX */
#define DMAC_ACTIVE_LVLEX(value)              (DMAC_ACTIVE_LVLEX_Msk & ((value) << DMAC_ACTIVE_LVLEX_Pos)) 

/* -------- DMAC_BASEADDR : (DMAC Offset: 0x34) (R/W 32) Descriptor Memory Section Base Address -------- */
#define DMAC_BASEADDR_RESETVALUE              _U_(0x00)                                            /**<  (DMAC_BASEADDR) Descriptor Memory Section Base Address  Reset Value */

#define DMAC_BASEADDR_BASEADDR_Pos            _U_(0)                                               /**< (DMAC_BASEADDR) Descriptor Memory Base Address Position */
#define DMAC_BASEADDR_BASEADDR_Msk            (_U_(0xFFFFFFFF) << DMAC_BASEADDR_BASEADDR_Pos)      /**< (DMAC_BASEADDR) Descriptor Memory Base Address Mask */
#define DMAC_BASEADDR_BASEADDR(value)         (DMAC_BASEADDR_BASEADDR_Msk & ((value) << DMAC_BASEADDR_BASEADDR_Pos))
#define DMAC_BASEADDR_Msk                     _U_(0xFFFFFFFF)                                      /**< (DMAC_BASEADDR) Register Mask  */


/* -------- DMAC_WRBADDR : (DMAC Offset: 0x38) (R/W 32) Write-Back Memory Section Base Address -------- */
#define DMAC_WRBADDR_RESETVALUE               _U_(0x00)                                            /**<  (DMAC_WRBADDR) Write-Back Memory Section Base Address  Reset Value */

#define DMAC_WRBADDR_WRBADDR_Pos              _U_(0)                                               /**< (DMAC_WRBADDR) Write-Back Memory Base Address Position */
#define DMAC_WRBADDR_WRBADDR_Msk              (_U_(0xFFFFFFFF) << DMAC_WRBADDR_WRBADDR_Pos)        /**< (DMAC_WRBADDR) Write-Back Memory Base Address Mask */
#define DMAC_WRBADDR_WRBADDR(value)           (DMAC_WRBADDR_WRBADDR_Msk & ((value) << DMAC_WRBADDR_WRBADDR_Pos))
#define DMAC_WRBADDR_Msk                      _U_(0xFFFFFFFF)                                      /**< (DMAC_WRBADDR) Register Mask  */


/** \brief DMAC register offsets definitions */
#define DMAC_BTCTRL_REG_OFST           (0x00)              /**< (DMAC_BTCTRL) Block Transfer Control Offset */
#define DMAC_BTCNT_REG_OFST            (0x02)              /**< (DMAC_BTCNT) Block Transfer Count Offset */
#define DMAC_SRCADDR_REG_OFST          (0x04)              /**< (DMAC_SRCADDR) Block Transfer Source Address Offset */
#define DMAC_DSTADDR_REG_OFST          (0x08)              /**< (DMAC_DSTADDR) Block Transfer Destination Address Offset */
#define DMAC_DESCADDR_REG_OFST         (0x0C)              /**< (DMAC_DESCADDR) Next Descriptor Address Offset */
#define DMAC_CHCTRLA_REG_OFST          (0x00)              /**< (DMAC_CHCTRLA) Channel n Control A Offset */
#define DMAC_CHCTRLB_REG_OFST          (0x04)              /**< (DMAC_CHCTRLB) Channel n Control B Offset */
#define DMAC_CHPRILVL_REG_OFST         (0x05)              /**< (DMAC_CHPRILVL) Channel n Priority Level Offset */
#define DMAC_CHEVCTRL_REG_OFST         (0x06)              /**< (DMAC_CHEVCTRL) Channel n Event Control Offset */
#define DMAC_CHINTENCLR_REG_OFST       (0x0C)              /**< (DMAC_CHINTENCLR) Channel n Interrupt Enable Clear Offset */
#define DMAC_CHINTENSET_REG_OFST       (0x0D)              /**< (DMAC_CHINTENSET) Channel n Interrupt Enable Set Offset */
#define DMAC_CHINTFLAG_REG_OFST        (0x0E)              /**< (DMAC_CHINTFLAG) Channel n Interrupt Flag Status and Clear Offset */
#define DMAC_CHSTATUS_REG_OFST         (0x0F)              /**< (DMAC_CHSTATUS) Channel n Status Offset */
#define DMAC_CTRL_REG_OFST             (0x00)              /**< (DMAC_CTRL) Control Offset */
#define DMAC_CRCCTRL_REG_OFST          (0x02)              /**< (DMAC_CRCCTRL) CRC Control Offset */
#define DMAC_CRCDATAIN_REG_OFST        (0x04)              /**< (DMAC_CRCDATAIN) CRC Data Input Offset */
#define DMAC_CRCCHKSUM_REG_OFST        (0x08)              /**< (DMAC_CRCCHKSUM) CRC Checksum Offset */
#define DMAC_CRCSTATUS_REG_OFST        (0x0C)              /**< (DMAC_CRCSTATUS) CRC Status Offset */
#define DMAC_DBGCTRL_REG_OFST          (0x0D)              /**< (DMAC_DBGCTRL) Debug Control Offset */
#define DMAC_SWTRIGCTRL_REG_OFST       (0x10)              /**< (DMAC_SWTRIGCTRL) Software Trigger Control Offset */
#define DMAC_PRICTRL0_REG_OFST         (0x14)              /**< (DMAC_PRICTRL0) Priority Control 0 Offset */
#define DMAC_INTPEND_REG_OFST          (0x20)              /**< (DMAC_INTPEND) Interrupt Pending Offset */
#define DMAC_INTSTATUS_REG_OFST        (0x24)              /**< (DMAC_INTSTATUS) Interrupt Status Offset */
#define DMAC_BUSYCH_REG_OFST           (0x28)              /**< (DMAC_BUSYCH) Busy Channels Offset */
#define DMAC_PENDCH_REG_OFST           (0x2C)              /**< (DMAC_PENDCH) Pending Channels Offset */
#define DMAC_ACTIVE_REG_OFST           (0x30)              /**< (DMAC_ACTIVE) Active Channel and Levels Offset */
#define DMAC_BASEADDR_REG_OFST         (0x34)              /**< (DMAC_BASEADDR) Descriptor Memory Section Base Address Offset */
#define DMAC_WRBADDR_REG_OFST          (0x38)              /**< (DMAC_WRBADDR) Write-Back Memory Section Base Address Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief DMAC_DESCRIPTOR register API structure */
typedef struct
{  /* Direct Memory Access Controller */
  __IO  uint16_t                       DMAC_BTCTRL;        /**< Offset: 0x00 (R/W  16) Block Transfer Control */
  __IO  uint16_t                       DMAC_BTCNT;         /**< Offset: 0x02 (R/W  16) Block Transfer Count */
  __IO  uint32_t                       DMAC_SRCADDR;       /**< Offset: 0x04 (R/W  32) Block Transfer Source Address */
  __IO  uint32_t                       DMAC_DSTADDR;       /**< Offset: 0x08 (R/W  32) Block Transfer Destination Address */
  __IO  uint32_t                       DMAC_DESCADDR;      /**< Offset: 0x0C (R/W  32) Next Descriptor Address */
} dmac_descriptor_registers_t
#ifdef __GNUC__
  __attribute__ ((aligned (8)))
#endif
;

/** \brief CHANNEL register API structure */
typedef struct
{
  __IO  uint32_t                       DMAC_CHCTRLA;       /**< Offset: 0x00 (R/W  32) Channel n Control A */
  __IO  uint8_t                        DMAC_CHCTRLB;       /**< Offset: 0x04 (R/W  8) Channel n Control B */
  __IO  uint8_t                        DMAC_CHPRILVL;      /**< Offset: 0x05 (R/W  8) Channel n Priority Level */
  __IO  uint8_t                        DMAC_CHEVCTRL;      /**< Offset: 0x06 (R/W  8) Channel n Event Control */
  __I   uint8_t                        Reserved1[0x05];
  __IO  uint8_t                        DMAC_CHINTENCLR;    /**< Offset: 0x0C (R/W  8) Channel n Interrupt Enable Clear */
  __IO  uint8_t                        DMAC_CHINTENSET;    /**< Offset: 0x0D (R/W  8) Channel n Interrupt Enable Set */
  __IO  uint8_t                        DMAC_CHINTFLAG;     /**< Offset: 0x0E (R/W  8) Channel n Interrupt Flag Status and Clear */
  __IO  uint8_t                        DMAC_CHSTATUS;      /**< Offset: 0x0F (R/W  8) Channel n Status */
} dmac_channel_registers_t;

#define CHANNEL_NUMBER _U_(32)

/** \brief DMAC register API structure */
typedef struct
{  /* Direct Memory Access Controller */
  __IO  uint16_t                       DMAC_CTRL;          /**< Offset: 0x00 (R/W  16) Control */
  __IO  uint16_t                       DMAC_CRCCTRL;       /**< Offset: 0x02 (R/W  16) CRC Control */
  __IO  uint32_t                       DMAC_CRCDATAIN;     /**< Offset: 0x04 (R/W  32) CRC Data Input */
  __IO  uint32_t                       DMAC_CRCCHKSUM;     /**< Offset: 0x08 (R/W  32) CRC Checksum */
  __IO  uint8_t                        DMAC_CRCSTATUS;     /**< Offset: 0x0C (R/W  8) CRC Status */
  __IO  uint8_t                        DMAC_DBGCTRL;       /**< Offset: 0x0D (R/W  8) Debug Control */
  __I   uint8_t                        Reserved1[0x02];
  __IO  uint32_t                       DMAC_SWTRIGCTRL;    /**< Offset: 0x10 (R/W  32) Software Trigger Control */
  __IO  uint32_t                       DMAC_PRICTRL0;      /**< Offset: 0x14 (R/W  32) Priority Control 0 */
  __I   uint8_t                        Reserved2[0x08];
  __IO  uint16_t                       DMAC_INTPEND;       /**< Offset: 0x20 (R/W  16) Interrupt Pending */
  __I   uint8_t                        Reserved3[0x02];
  __I   uint32_t                       DMAC_INTSTATUS;     /**< Offset: 0x24 (R/   32) Interrupt Status */
  __I   uint32_t                       DMAC_BUSYCH;        /**< Offset: 0x28 (R/   32) Busy Channels */
  __I   uint32_t                       DMAC_PENDCH;        /**< Offset: 0x2C (R/   32) Pending Channels */
  __I   uint32_t                       DMAC_ACTIVE;        /**< Offset: 0x30 (R/   32) Active Channel and Levels */
  __IO  uint32_t                       DMAC_BASEADDR;      /**< Offset: 0x34 (R/W  32) Descriptor Memory Section Base Address */
  __IO  uint32_t                       DMAC_WRBADDR;       /**< Offset: 0x38 (R/W  32) Write-Back Memory Section Base Address */
  __I   uint8_t                        Reserved4[0x04];
        dmac_channel_registers_t       CHANNEL[CHANNEL_NUMBER]; /**< Offset: 0x40  */
} dmac_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
/** \brief DMAC_DESCRIPTOR memory section attribute */
#if defined (__GNUC__) || defined (__CC_ARM)
 #define SECTION_DMAC_DESCRIPTOR      __attribute__ ((section(".hsram")))
#elif defined (__ICCARM__)
 #define SECTION_DMAC_DESCRIPTOR      @".hsram"
#else
 #ifndef SECTION_DMAC_DESCRIPTOR
  #error SECTION_DMAC_DESCRIPTOR must be defined for compiler to use memory section .hsram
 #endif
#endif

#endif /* _SAME54_DMAC_COMPONENT_H_ */
