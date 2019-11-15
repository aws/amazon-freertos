/**
 * \brief Component description for MATRIX
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
#ifndef _SAME70_MATRIX_COMPONENT_H_
#define _SAME70_MATRIX_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR MATRIX                                       */
/* ************************************************************************** */

/* -------- MATRIX_PRAS : (MATRIX Offset: 0x00) (R/W 32) Priority Register A for Slave 0 -------- */
#define MATRIX_PRAS_M0PR_Pos                  _U_(0)                                               /**< (MATRIX_PRAS) Master 0 Priority Position */
#define MATRIX_PRAS_M0PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M0PR_Pos)                   /**< (MATRIX_PRAS) Master 0 Priority Mask */
#define MATRIX_PRAS_M0PR(value)               (MATRIX_PRAS_M0PR_Msk & ((value) << MATRIX_PRAS_M0PR_Pos))
#define MATRIX_PRAS_M1PR_Pos                  _U_(4)                                               /**< (MATRIX_PRAS) Master 1 Priority Position */
#define MATRIX_PRAS_M1PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M1PR_Pos)                   /**< (MATRIX_PRAS) Master 1 Priority Mask */
#define MATRIX_PRAS_M1PR(value)               (MATRIX_PRAS_M1PR_Msk & ((value) << MATRIX_PRAS_M1PR_Pos))
#define MATRIX_PRAS_M2PR_Pos                  _U_(8)                                               /**< (MATRIX_PRAS) Master 2 Priority Position */
#define MATRIX_PRAS_M2PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M2PR_Pos)                   /**< (MATRIX_PRAS) Master 2 Priority Mask */
#define MATRIX_PRAS_M2PR(value)               (MATRIX_PRAS_M2PR_Msk & ((value) << MATRIX_PRAS_M2PR_Pos))
#define MATRIX_PRAS_M3PR_Pos                  _U_(12)                                              /**< (MATRIX_PRAS) Master 3 Priority Position */
#define MATRIX_PRAS_M3PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M3PR_Pos)                   /**< (MATRIX_PRAS) Master 3 Priority Mask */
#define MATRIX_PRAS_M3PR(value)               (MATRIX_PRAS_M3PR_Msk & ((value) << MATRIX_PRAS_M3PR_Pos))
#define MATRIX_PRAS_M4PR_Pos                  _U_(16)                                              /**< (MATRIX_PRAS) Master 4 Priority Position */
#define MATRIX_PRAS_M4PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M4PR_Pos)                   /**< (MATRIX_PRAS) Master 4 Priority Mask */
#define MATRIX_PRAS_M4PR(value)               (MATRIX_PRAS_M4PR_Msk & ((value) << MATRIX_PRAS_M4PR_Pos))
#define MATRIX_PRAS_M5PR_Pos                  _U_(20)                                              /**< (MATRIX_PRAS) Master 5 Priority Position */
#define MATRIX_PRAS_M5PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M5PR_Pos)                   /**< (MATRIX_PRAS) Master 5 Priority Mask */
#define MATRIX_PRAS_M5PR(value)               (MATRIX_PRAS_M5PR_Msk & ((value) << MATRIX_PRAS_M5PR_Pos))
#define MATRIX_PRAS_M6PR_Pos                  _U_(24)                                              /**< (MATRIX_PRAS) Master 6 Priority Position */
#define MATRIX_PRAS_M6PR_Msk                  (_U_(0x3) << MATRIX_PRAS_M6PR_Pos)                   /**< (MATRIX_PRAS) Master 6 Priority Mask */
#define MATRIX_PRAS_M6PR(value)               (MATRIX_PRAS_M6PR_Msk & ((value) << MATRIX_PRAS_M6PR_Pos))
#define MATRIX_PRAS_Msk                       _U_(0x03333333)                                      /**< (MATRIX_PRAS) Register Mask  */


/* -------- MATRIX_PRBS : (MATRIX Offset: 0x04) (R/W 32) Priority Register B for Slave 0 -------- */
#define MATRIX_PRBS_M8PR_Pos                  _U_(0)                                               /**< (MATRIX_PRBS) Master 8 Priority Position */
#define MATRIX_PRBS_M8PR_Msk                  (_U_(0x3) << MATRIX_PRBS_M8PR_Pos)                   /**< (MATRIX_PRBS) Master 8 Priority Mask */
#define MATRIX_PRBS_M8PR(value)               (MATRIX_PRBS_M8PR_Msk & ((value) << MATRIX_PRBS_M8PR_Pos))
#define MATRIX_PRBS_M9PR_Pos                  _U_(4)                                               /**< (MATRIX_PRBS) Master 9 Priority Position */
#define MATRIX_PRBS_M9PR_Msk                  (_U_(0x3) << MATRIX_PRBS_M9PR_Pos)                   /**< (MATRIX_PRBS) Master 9 Priority Mask */
#define MATRIX_PRBS_M9PR(value)               (MATRIX_PRBS_M9PR_Msk & ((value) << MATRIX_PRBS_M9PR_Pos))
#define MATRIX_PRBS_M10PR_Pos                 _U_(8)                                               /**< (MATRIX_PRBS) Master 10 Priority Position */
#define MATRIX_PRBS_M10PR_Msk                 (_U_(0x3) << MATRIX_PRBS_M10PR_Pos)                  /**< (MATRIX_PRBS) Master 10 Priority Mask */
#define MATRIX_PRBS_M10PR(value)              (MATRIX_PRBS_M10PR_Msk & ((value) << MATRIX_PRBS_M10PR_Pos))
#define MATRIX_PRBS_M11PR_Pos                 _U_(12)                                              /**< (MATRIX_PRBS) Master 11 Priority Position */
#define MATRIX_PRBS_M11PR_Msk                 (_U_(0x3) << MATRIX_PRBS_M11PR_Pos)                  /**< (MATRIX_PRBS) Master 11 Priority Mask */
#define MATRIX_PRBS_M11PR(value)              (MATRIX_PRBS_M11PR_Msk & ((value) << MATRIX_PRBS_M11PR_Pos))
#define MATRIX_PRBS_M12PR_Pos                 _U_(16)                                              /**< (MATRIX_PRBS) Master 12 Priority Position */
#define MATRIX_PRBS_M12PR_Msk                 (_U_(0x3) << MATRIX_PRBS_M12PR_Pos)                  /**< (MATRIX_PRBS) Master 12 Priority Mask */
#define MATRIX_PRBS_M12PR(value)              (MATRIX_PRBS_M12PR_Msk & ((value) << MATRIX_PRBS_M12PR_Pos))
#define MATRIX_PRBS_Msk                       _U_(0x00033333)                                      /**< (MATRIX_PRBS) Register Mask  */


/* -------- MATRIX_MCFG : (MATRIX Offset: 0x00) (R/W 32) Master Configuration Register 0 -------- */
#define MATRIX_MCFG_ULBT_Pos                  _U_(0)                                               /**< (MATRIX_MCFG) Undefined Length Burst Type Position */
#define MATRIX_MCFG_ULBT_Msk                  (_U_(0x7) << MATRIX_MCFG_ULBT_Pos)                   /**< (MATRIX_MCFG) Undefined Length Burst Type Mask */
#define MATRIX_MCFG_ULBT(value)               (MATRIX_MCFG_ULBT_Msk & ((value) << MATRIX_MCFG_ULBT_Pos))
#define   MATRIX_MCFG_ULBT_UNLTD_LENGTH_Val   _U_(0x0)                                             /**< (MATRIX_MCFG) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave.  */
#define   MATRIX_MCFG_ULBT_SINGLE_ACCESS_Val  _U_(0x1)                                             /**< (MATRIX_MCFG) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence.  */
#define   MATRIX_MCFG_ULBT_4BEAT_BURST_Val    _U_(0x2)                                             /**< (MATRIX_MCFG) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats.  */
#define   MATRIX_MCFG_ULBT_8BEAT_BURST_Val    _U_(0x3)                                             /**< (MATRIX_MCFG) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats.  */
#define   MATRIX_MCFG_ULBT_16BEAT_BURST_Val   _U_(0x4)                                             /**< (MATRIX_MCFG) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats.  */
#define   MATRIX_MCFG_ULBT_32BEAT_BURST_Val   _U_(0x5)                                             /**< (MATRIX_MCFG) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats.  */
#define   MATRIX_MCFG_ULBT_64BEAT_BURST_Val   _U_(0x6)                                             /**< (MATRIX_MCFG) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats.  */
#define   MATRIX_MCFG_ULBT_128BEAT_BURST_Val  _U_(0x7)                                             /**< (MATRIX_MCFG) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats.  */
#define MATRIX_MCFG_ULBT_UNLTD_LENGTH         (MATRIX_MCFG_ULBT_UNLTD_LENGTH_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) Unlimited Length Burst-No predicted end of burst is generated, therefore INCR bursts coming from this master can only be broken if the Slave Slot Cycle Limit is reached. If the Slot Cycle Limit is not reached, the burst is normally completed by the master, at the latest, on the next AHB 1-Kbyte address boundary, allowing up to 256-beat word bursts or 128-beat double-word bursts.This value should not be used in the very particular case of a master capable of performing back-to-back undefined length bursts on a single slave, since this could indefinitely freeze the slave arbitration and thus prevent another master from accessing this slave. Position  */
#define MATRIX_MCFG_ULBT_SINGLE_ACCESS        (MATRIX_MCFG_ULBT_SINGLE_ACCESS_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) Single Access-The undefined length burst is treated as a succession of single accesses, allowing re-arbitration at each beat of the INCR burst or bursts sequence. Position  */
#define MATRIX_MCFG_ULBT_4BEAT_BURST          (MATRIX_MCFG_ULBT_4BEAT_BURST_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) 4-beat Burst-The undefined length burst or bursts sequence is split into 4-beat bursts or less, allowing re-arbitration every 4 beats. Position  */
#define MATRIX_MCFG_ULBT_8BEAT_BURST          (MATRIX_MCFG_ULBT_8BEAT_BURST_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) 8-beat Burst-The undefined length burst or bursts sequence is split into 8-beat bursts or less, allowing re-arbitration every 8 beats. Position  */
#define MATRIX_MCFG_ULBT_16BEAT_BURST         (MATRIX_MCFG_ULBT_16BEAT_BURST_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) 16-beat Burst-The undefined length burst or bursts sequence is split into 16-beat bursts or less, allowing re-arbitration every 16 beats. Position  */
#define MATRIX_MCFG_ULBT_32BEAT_BURST         (MATRIX_MCFG_ULBT_32BEAT_BURST_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) 32-beat Burst -The undefined length burst or bursts sequence is split into 32-beat bursts or less, allowing re-arbitration every 32 beats. Position  */
#define MATRIX_MCFG_ULBT_64BEAT_BURST         (MATRIX_MCFG_ULBT_64BEAT_BURST_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) 64-beat Burst-The undefined length burst or bursts sequence is split into 64-beat bursts or less, allowing re-arbitration every 64 beats. Position  */
#define MATRIX_MCFG_ULBT_128BEAT_BURST        (MATRIX_MCFG_ULBT_128BEAT_BURST_Val << MATRIX_MCFG_ULBT_Pos) /**< (MATRIX_MCFG) 128-beat Burst-The undefined length burst or bursts sequence is split into 128-beat bursts or less, allowing re-arbitration every 128 beats. Position  */
#define MATRIX_MCFG_Msk                       _U_(0x00000007)                                      /**< (MATRIX_MCFG) Register Mask  */


/* -------- MATRIX_SCFG : (MATRIX Offset: 0x40) (R/W 32) Slave Configuration Register 0 -------- */
#define MATRIX_SCFG_SLOT_CYCLE_Pos            _U_(0)                                               /**< (MATRIX_SCFG) Maximum Bus Grant Duration for Masters Position */
#define MATRIX_SCFG_SLOT_CYCLE_Msk            (_U_(0x1FF) << MATRIX_SCFG_SLOT_CYCLE_Pos)           /**< (MATRIX_SCFG) Maximum Bus Grant Duration for Masters Mask */
#define MATRIX_SCFG_SLOT_CYCLE(value)         (MATRIX_SCFG_SLOT_CYCLE_Msk & ((value) << MATRIX_SCFG_SLOT_CYCLE_Pos))
#define MATRIX_SCFG_DEFMSTR_TYPE_Pos          _U_(16)                                              /**< (MATRIX_SCFG) Default Master Type Position */
#define MATRIX_SCFG_DEFMSTR_TYPE_Msk          (_U_(0x3) << MATRIX_SCFG_DEFMSTR_TYPE_Pos)           /**< (MATRIX_SCFG) Default Master Type Mask */
#define MATRIX_SCFG_DEFMSTR_TYPE(value)       (MATRIX_SCFG_DEFMSTR_TYPE_Msk & ((value) << MATRIX_SCFG_DEFMSTR_TYPE_Pos))
#define   MATRIX_SCFG_DEFMSTR_TYPE_NONE_Val   _U_(0x0)                                             /**< (MATRIX_SCFG) No Default Master-At the end of the current slave access, if no other master request is pending, the slave is disconnected from all masters.This results in a one clock cycle latency for the first access of a burst transfer or for a single access.  */
#define   MATRIX_SCFG_DEFMSTR_TYPE_LAST_Val   _U_(0x1)                                             /**< (MATRIX_SCFG) Last Default Master-At the end of the current slave access, if no other master request is pending, the slave stays connected to the last master having accessed it.This results in not having one clock cycle latency when the last master tries to access the slave again.  */
#define   MATRIX_SCFG_DEFMSTR_TYPE_FIXED_Val  _U_(0x2)                                             /**< (MATRIX_SCFG) Fixed Default Master-At the end of the current slave access, if no other master request is pending, the slave connects to the fixed master the number that has been written in the FIXED_DEFMSTR field.This results in not having one clock cycle latency when the fixed master tries to access the slave again.  */
#define MATRIX_SCFG_DEFMSTR_TYPE_NONE         (MATRIX_SCFG_DEFMSTR_TYPE_NONE_Val << MATRIX_SCFG_DEFMSTR_TYPE_Pos) /**< (MATRIX_SCFG) No Default Master-At the end of the current slave access, if no other master request is pending, the slave is disconnected from all masters.This results in a one clock cycle latency for the first access of a burst transfer or for a single access. Position  */
#define MATRIX_SCFG_DEFMSTR_TYPE_LAST         (MATRIX_SCFG_DEFMSTR_TYPE_LAST_Val << MATRIX_SCFG_DEFMSTR_TYPE_Pos) /**< (MATRIX_SCFG) Last Default Master-At the end of the current slave access, if no other master request is pending, the slave stays connected to the last master having accessed it.This results in not having one clock cycle latency when the last master tries to access the slave again. Position  */
#define MATRIX_SCFG_DEFMSTR_TYPE_FIXED        (MATRIX_SCFG_DEFMSTR_TYPE_FIXED_Val << MATRIX_SCFG_DEFMSTR_TYPE_Pos) /**< (MATRIX_SCFG) Fixed Default Master-At the end of the current slave access, if no other master request is pending, the slave connects to the fixed master the number that has been written in the FIXED_DEFMSTR field.This results in not having one clock cycle latency when the fixed master tries to access the slave again. Position  */
#define MATRIX_SCFG_FIXED_DEFMSTR_Pos         _U_(18)                                              /**< (MATRIX_SCFG) Fixed Default Master Position */
#define MATRIX_SCFG_FIXED_DEFMSTR_Msk         (_U_(0xF) << MATRIX_SCFG_FIXED_DEFMSTR_Pos)          /**< (MATRIX_SCFG) Fixed Default Master Mask */
#define MATRIX_SCFG_FIXED_DEFMSTR(value)      (MATRIX_SCFG_FIXED_DEFMSTR_Msk & ((value) << MATRIX_SCFG_FIXED_DEFMSTR_Pos))
#define MATRIX_SCFG_Msk                       _U_(0x003F01FF)                                      /**< (MATRIX_SCFG) Register Mask  */


/* -------- MATRIX_MRCR : (MATRIX Offset: 0x100) (R/W 32) Master Remap Control Register -------- */
#define MATRIX_MRCR_RCB0_Pos                  _U_(0)                                               /**< (MATRIX_MRCR) Remap Command Bit for Master 0 Position */
#define MATRIX_MRCR_RCB0_Msk                  (_U_(0x1) << MATRIX_MRCR_RCB0_Pos)                   /**< (MATRIX_MRCR) Remap Command Bit for Master 0 Mask */
#define MATRIX_MRCR_RCB0(value)               (MATRIX_MRCR_RCB0_Msk & ((value) << MATRIX_MRCR_RCB0_Pos))
#define MATRIX_MRCR_RCB1_Pos                  _U_(1)                                               /**< (MATRIX_MRCR) Remap Command Bit for Master 1 Position */
#define MATRIX_MRCR_RCB1_Msk                  (_U_(0x1) << MATRIX_MRCR_RCB1_Pos)                   /**< (MATRIX_MRCR) Remap Command Bit for Master 1 Mask */
#define MATRIX_MRCR_RCB1(value)               (MATRIX_MRCR_RCB1_Msk & ((value) << MATRIX_MRCR_RCB1_Pos))
#define MATRIX_MRCR_RCB2_Pos                  _U_(2)                                               /**< (MATRIX_MRCR) Remap Command Bit for Master 2 Position */
#define MATRIX_MRCR_RCB2_Msk                  (_U_(0x1) << MATRIX_MRCR_RCB2_Pos)                   /**< (MATRIX_MRCR) Remap Command Bit for Master 2 Mask */
#define MATRIX_MRCR_RCB2(value)               (MATRIX_MRCR_RCB2_Msk & ((value) << MATRIX_MRCR_RCB2_Pos))
#define MATRIX_MRCR_RCB3_Pos                  _U_(3)                                               /**< (MATRIX_MRCR) Remap Command Bit for Master 3 Position */
#define MATRIX_MRCR_RCB3_Msk                  (_U_(0x1) << MATRIX_MRCR_RCB3_Pos)                   /**< (MATRIX_MRCR) Remap Command Bit for Master 3 Mask */
#define MATRIX_MRCR_RCB3(value)               (MATRIX_MRCR_RCB3_Msk & ((value) << MATRIX_MRCR_RCB3_Pos))
#define MATRIX_MRCR_RCB4_Pos                  _U_(4)                                               /**< (MATRIX_MRCR) Remap Command Bit for Master 4 Position */
#define MATRIX_MRCR_RCB4_Msk                  (_U_(0x1) << MATRIX_MRCR_RCB4_Pos)                   /**< (MATRIX_MRCR) Remap Command Bit for Master 4 Mask */
#define MATRIX_MRCR_RCB4(value)               (MATRIX_MRCR_RCB4_Msk & ((value) << MATRIX_MRCR_RCB4_Pos))
#define MATRIX_MRCR_RCB5_Pos                  _U_(5)                                               /**< (MATRIX_MRCR) Remap Command Bit for Master 5 Position */
#define MATRIX_MRCR_RCB5_Msk                  (_U_(0x1) << MATRIX_MRCR_RCB5_Pos)                   /**< (MATRIX_MRCR) Remap Command Bit for Master 5 Mask */
#define MATRIX_MRCR_RCB5(value)               (MATRIX_MRCR_RCB5_Msk & ((value) << MATRIX_MRCR_RCB5_Pos))
#define MATRIX_MRCR_RCB6_Pos                  _U_(6)                                               /**< (MATRIX_MRCR) Remap Command Bit for Master 6 Position */
#define MATRIX_MRCR_RCB6_Msk                  (_U_(0x1) << MATRIX_MRCR_RCB6_Pos)                   /**< (MATRIX_MRCR) Remap Command Bit for Master 6 Mask */
#define MATRIX_MRCR_RCB6(value)               (MATRIX_MRCR_RCB6_Msk & ((value) << MATRIX_MRCR_RCB6_Pos))
#define MATRIX_MRCR_RCB8_Pos                  _U_(8)                                               /**< (MATRIX_MRCR) Remap Command Bit for Master 8 Position */
#define MATRIX_MRCR_RCB8_Msk                  (_U_(0x1) << MATRIX_MRCR_RCB8_Pos)                   /**< (MATRIX_MRCR) Remap Command Bit for Master 8 Mask */
#define MATRIX_MRCR_RCB8(value)               (MATRIX_MRCR_RCB8_Msk & ((value) << MATRIX_MRCR_RCB8_Pos))
#define MATRIX_MRCR_RCB9_Pos                  _U_(9)                                               /**< (MATRIX_MRCR) Remap Command Bit for Master 9 Position */
#define MATRIX_MRCR_RCB9_Msk                  (_U_(0x1) << MATRIX_MRCR_RCB9_Pos)                   /**< (MATRIX_MRCR) Remap Command Bit for Master 9 Mask */
#define MATRIX_MRCR_RCB9(value)               (MATRIX_MRCR_RCB9_Msk & ((value) << MATRIX_MRCR_RCB9_Pos))
#define MATRIX_MRCR_RCB10_Pos                 _U_(10)                                              /**< (MATRIX_MRCR) Remap Command Bit for Master 10 Position */
#define MATRIX_MRCR_RCB10_Msk                 (_U_(0x1) << MATRIX_MRCR_RCB10_Pos)                  /**< (MATRIX_MRCR) Remap Command Bit for Master 10 Mask */
#define MATRIX_MRCR_RCB10(value)              (MATRIX_MRCR_RCB10_Msk & ((value) << MATRIX_MRCR_RCB10_Pos))
#define MATRIX_MRCR_RCB11_Pos                 _U_(11)                                              /**< (MATRIX_MRCR) Remap Command Bit for Master 11 Position */
#define MATRIX_MRCR_RCB11_Msk                 (_U_(0x1) << MATRIX_MRCR_RCB11_Pos)                  /**< (MATRIX_MRCR) Remap Command Bit for Master 11 Mask */
#define MATRIX_MRCR_RCB11(value)              (MATRIX_MRCR_RCB11_Msk & ((value) << MATRIX_MRCR_RCB11_Pos))
#define MATRIX_MRCR_RCB12_Pos                 _U_(12)                                              /**< (MATRIX_MRCR) Remap Command Bit for Master 12 Position */
#define MATRIX_MRCR_RCB12_Msk                 (_U_(0x1) << MATRIX_MRCR_RCB12_Pos)                  /**< (MATRIX_MRCR) Remap Command Bit for Master 12 Mask */
#define MATRIX_MRCR_RCB12(value)              (MATRIX_MRCR_RCB12_Msk & ((value) << MATRIX_MRCR_RCB12_Pos))
#define MATRIX_MRCR_Msk                       _U_(0x00001F7F)                                      /**< (MATRIX_MRCR) Register Mask  */

#define MATRIX_MRCR_RCB_Pos                   _U_(0)                                               /**< (MATRIX_MRCR Position) Remap Command Bit for Master x2 */
#define MATRIX_MRCR_RCB_Msk                   (_U_(0xFFF) << MATRIX_MRCR_RCB_Pos)                  /**< (MATRIX_MRCR Mask) RCB */
#define MATRIX_MRCR_RCB(value)                (MATRIX_MRCR_RCB_Msk & ((value) << MATRIX_MRCR_RCB_Pos)) 

/* -------- CCFG_CAN0 : (MATRIX Offset: 0x110) (R/W 32) CAN0 Configuration Register -------- */
#define CCFG_CAN0_CAN0DMABA_Pos               _U_(16)                                              /**< (CCFG_CAN0) CAN0 DMA Base Address Position */
#define CCFG_CAN0_CAN0DMABA_Msk               (_U_(0xFFFF) << CCFG_CAN0_CAN0DMABA_Pos)             /**< (CCFG_CAN0) CAN0 DMA Base Address Mask */
#define CCFG_CAN0_CAN0DMABA(value)            (CCFG_CAN0_CAN0DMABA_Msk & ((value) << CCFG_CAN0_CAN0DMABA_Pos))
#define CCFG_CAN0_Msk                         _U_(0xFFFF0000)                                      /**< (CCFG_CAN0) Register Mask  */


/* -------- CCFG_SYSIO : (MATRIX Offset: 0x114) (R/W 32) System I/O and CAN1 Configuration Register -------- */
#define CCFG_SYSIO_SYSIO4_Pos                 _U_(4)                                               /**< (CCFG_SYSIO) PB4 or TDI Assignment Position */
#define CCFG_SYSIO_SYSIO4_Msk                 (_U_(0x1) << CCFG_SYSIO_SYSIO4_Pos)                  /**< (CCFG_SYSIO) PB4 or TDI Assignment Mask */
#define CCFG_SYSIO_SYSIO4(value)              (CCFG_SYSIO_SYSIO4_Msk & ((value) << CCFG_SYSIO_SYSIO4_Pos))
#define CCFG_SYSIO_SYSIO5_Pos                 _U_(5)                                               /**< (CCFG_SYSIO) PB5 or TDO/TRACESWO Assignment Position */
#define CCFG_SYSIO_SYSIO5_Msk                 (_U_(0x1) << CCFG_SYSIO_SYSIO5_Pos)                  /**< (CCFG_SYSIO) PB5 or TDO/TRACESWO Assignment Mask */
#define CCFG_SYSIO_SYSIO5(value)              (CCFG_SYSIO_SYSIO5_Msk & ((value) << CCFG_SYSIO_SYSIO5_Pos))
#define CCFG_SYSIO_SYSIO6_Pos                 _U_(6)                                               /**< (CCFG_SYSIO) PB6 or TMS/SWDIO Assignment Position */
#define CCFG_SYSIO_SYSIO6_Msk                 (_U_(0x1) << CCFG_SYSIO_SYSIO6_Pos)                  /**< (CCFG_SYSIO) PB6 or TMS/SWDIO Assignment Mask */
#define CCFG_SYSIO_SYSIO6(value)              (CCFG_SYSIO_SYSIO6_Msk & ((value) << CCFG_SYSIO_SYSIO6_Pos))
#define CCFG_SYSIO_SYSIO7_Pos                 _U_(7)                                               /**< (CCFG_SYSIO) PB7 or TCK/SWCLK Assignment Position */
#define CCFG_SYSIO_SYSIO7_Msk                 (_U_(0x1) << CCFG_SYSIO_SYSIO7_Pos)                  /**< (CCFG_SYSIO) PB7 or TCK/SWCLK Assignment Mask */
#define CCFG_SYSIO_SYSIO7(value)              (CCFG_SYSIO_SYSIO7_Msk & ((value) << CCFG_SYSIO_SYSIO7_Pos))
#define CCFG_SYSIO_SYSIO12_Pos                _U_(12)                                              /**< (CCFG_SYSIO) PB12 or ERASE Assignment Position */
#define CCFG_SYSIO_SYSIO12_Msk                (_U_(0x1) << CCFG_SYSIO_SYSIO12_Pos)                 /**< (CCFG_SYSIO) PB12 or ERASE Assignment Mask */
#define CCFG_SYSIO_SYSIO12(value)             (CCFG_SYSIO_SYSIO12_Msk & ((value) << CCFG_SYSIO_SYSIO12_Pos))
#define CCFG_SYSIO_CAN1DMABA_Pos              _U_(16)                                              /**< (CCFG_SYSIO) CAN1 DMA Base Address Position */
#define CCFG_SYSIO_CAN1DMABA_Msk              (_U_(0xFFFF) << CCFG_SYSIO_CAN1DMABA_Pos)            /**< (CCFG_SYSIO) CAN1 DMA Base Address Mask */
#define CCFG_SYSIO_CAN1DMABA(value)           (CCFG_SYSIO_CAN1DMABA_Msk & ((value) << CCFG_SYSIO_CAN1DMABA_Pos))
#define CCFG_SYSIO_Msk                        _U_(0xFFFF10F0)                                      /**< (CCFG_SYSIO) Register Mask  */

#define CCFG_SYSIO_SYSIO_Pos                  _U_(4)                                               /**< (CCFG_SYSIO Position) PB4 or TDI Assignment */
#define CCFG_SYSIO_SYSIO_Msk                  (_U_(0x1F) << CCFG_SYSIO_SYSIO_Pos)                  /**< (CCFG_SYSIO Mask) SYSIO */
#define CCFG_SYSIO_SYSIO(value)               (CCFG_SYSIO_SYSIO_Msk & ((value) << CCFG_SYSIO_SYSIO_Pos)) 

/* -------- CCFG_PCCR : (MATRIX Offset: 0x118) (R/W 32) Peripheral Clock Configuration Register -------- */
#define CCFG_PCCR_TC0CC_Pos                   _U_(20)                                              /**< (CCFG_PCCR) TC0 Clock Configuration Position */
#define CCFG_PCCR_TC0CC_Msk                   (_U_(0x1) << CCFG_PCCR_TC0CC_Pos)                    /**< (CCFG_PCCR) TC0 Clock Configuration Mask */
#define CCFG_PCCR_TC0CC(value)                (CCFG_PCCR_TC0CC_Msk & ((value) << CCFG_PCCR_TC0CC_Pos))
#define CCFG_PCCR_I2SC0CC_Pos                 _U_(21)                                              /**< (CCFG_PCCR) I2SC0 Clock Configuration Position */
#define CCFG_PCCR_I2SC0CC_Msk                 (_U_(0x1) << CCFG_PCCR_I2SC0CC_Pos)                  /**< (CCFG_PCCR) I2SC0 Clock Configuration Mask */
#define CCFG_PCCR_I2SC0CC(value)              (CCFG_PCCR_I2SC0CC_Msk & ((value) << CCFG_PCCR_I2SC0CC_Pos))
#define CCFG_PCCR_I2SC1CC_Pos                 _U_(22)                                              /**< (CCFG_PCCR) I2SC1 Clock Configuration Position */
#define CCFG_PCCR_I2SC1CC_Msk                 (_U_(0x1) << CCFG_PCCR_I2SC1CC_Pos)                  /**< (CCFG_PCCR) I2SC1 Clock Configuration Mask */
#define CCFG_PCCR_I2SC1CC(value)              (CCFG_PCCR_I2SC1CC_Msk & ((value) << CCFG_PCCR_I2SC1CC_Pos))
#define CCFG_PCCR_Msk                         _U_(0x00700000)                                      /**< (CCFG_PCCR) Register Mask  */


/* -------- CCFG_DYNCKG : (MATRIX Offset: 0x11C) (R/W 32) Dynamic Clock Gating Register -------- */
#define CCFG_DYNCKG_MATCKG_Pos                _U_(0)                                               /**< (CCFG_DYNCKG) MATRIX Dynamic Clock Gating Position */
#define CCFG_DYNCKG_MATCKG_Msk                (_U_(0x1) << CCFG_DYNCKG_MATCKG_Pos)                 /**< (CCFG_DYNCKG) MATRIX Dynamic Clock Gating Mask */
#define CCFG_DYNCKG_MATCKG(value)             (CCFG_DYNCKG_MATCKG_Msk & ((value) << CCFG_DYNCKG_MATCKG_Pos))
#define CCFG_DYNCKG_BRIDCKG_Pos               _U_(1)                                               /**< (CCFG_DYNCKG) Bridge Dynamic Clock Gating Enable Position */
#define CCFG_DYNCKG_BRIDCKG_Msk               (_U_(0x1) << CCFG_DYNCKG_BRIDCKG_Pos)                /**< (CCFG_DYNCKG) Bridge Dynamic Clock Gating Enable Mask */
#define CCFG_DYNCKG_BRIDCKG(value)            (CCFG_DYNCKG_BRIDCKG_Msk & ((value) << CCFG_DYNCKG_BRIDCKG_Pos))
#define CCFG_DYNCKG_EFCCKG_Pos                _U_(2)                                               /**< (CCFG_DYNCKG) EFC Dynamic Clock Gating Enable Position */
#define CCFG_DYNCKG_EFCCKG_Msk                (_U_(0x1) << CCFG_DYNCKG_EFCCKG_Pos)                 /**< (CCFG_DYNCKG) EFC Dynamic Clock Gating Enable Mask */
#define CCFG_DYNCKG_EFCCKG(value)             (CCFG_DYNCKG_EFCCKG_Msk & ((value) << CCFG_DYNCKG_EFCCKG_Pos))
#define CCFG_DYNCKG_Msk                       _U_(0x00000007)                                      /**< (CCFG_DYNCKG) Register Mask  */


/* -------- CCFG_SMCNFCS : (MATRIX Offset: 0x124) (R/W 32) SMC NAND Flash Chip Select Configuration Register -------- */
#define CCFG_SMCNFCS_SMC_NFCS0_Pos            _U_(0)                                               /**< (CCFG_SMCNFCS) SMC NAND Flash Chip Select 0 Assignment Position */
#define CCFG_SMCNFCS_SMC_NFCS0_Msk            (_U_(0x1) << CCFG_SMCNFCS_SMC_NFCS0_Pos)             /**< (CCFG_SMCNFCS) SMC NAND Flash Chip Select 0 Assignment Mask */
#define CCFG_SMCNFCS_SMC_NFCS0(value)         (CCFG_SMCNFCS_SMC_NFCS0_Msk & ((value) << CCFG_SMCNFCS_SMC_NFCS0_Pos))
#define CCFG_SMCNFCS_SMC_NFCS1_Pos            _U_(1)                                               /**< (CCFG_SMCNFCS) SMC NAND Flash Chip Select 1 Assignment Position */
#define CCFG_SMCNFCS_SMC_NFCS1_Msk            (_U_(0x1) << CCFG_SMCNFCS_SMC_NFCS1_Pos)             /**< (CCFG_SMCNFCS) SMC NAND Flash Chip Select 1 Assignment Mask */
#define CCFG_SMCNFCS_SMC_NFCS1(value)         (CCFG_SMCNFCS_SMC_NFCS1_Msk & ((value) << CCFG_SMCNFCS_SMC_NFCS1_Pos))
#define CCFG_SMCNFCS_SMC_NFCS2_Pos            _U_(2)                                               /**< (CCFG_SMCNFCS) SMC NAND Flash Chip Select 2 Assignment Position */
#define CCFG_SMCNFCS_SMC_NFCS2_Msk            (_U_(0x1) << CCFG_SMCNFCS_SMC_NFCS2_Pos)             /**< (CCFG_SMCNFCS) SMC NAND Flash Chip Select 2 Assignment Mask */
#define CCFG_SMCNFCS_SMC_NFCS2(value)         (CCFG_SMCNFCS_SMC_NFCS2_Msk & ((value) << CCFG_SMCNFCS_SMC_NFCS2_Pos))
#define CCFG_SMCNFCS_SMC_NFCS3_Pos            _U_(3)                                               /**< (CCFG_SMCNFCS) SMC NAND Flash Chip Select 3 Assignment Position */
#define CCFG_SMCNFCS_SMC_NFCS3_Msk            (_U_(0x1) << CCFG_SMCNFCS_SMC_NFCS3_Pos)             /**< (CCFG_SMCNFCS) SMC NAND Flash Chip Select 3 Assignment Mask */
#define CCFG_SMCNFCS_SMC_NFCS3(value)         (CCFG_SMCNFCS_SMC_NFCS3_Msk & ((value) << CCFG_SMCNFCS_SMC_NFCS3_Pos))
#define CCFG_SMCNFCS_SDRAMEN_Pos              _U_(4)                                               /**< (CCFG_SMCNFCS) SDRAM Enable Position */
#define CCFG_SMCNFCS_SDRAMEN_Msk              (_U_(0x1) << CCFG_SMCNFCS_SDRAMEN_Pos)               /**< (CCFG_SMCNFCS) SDRAM Enable Mask */
#define CCFG_SMCNFCS_SDRAMEN(value)           (CCFG_SMCNFCS_SDRAMEN_Msk & ((value) << CCFG_SMCNFCS_SDRAMEN_Pos))
#define CCFG_SMCNFCS_Msk                      _U_(0x0000001F)                                      /**< (CCFG_SMCNFCS) Register Mask  */

#define CCFG_SMCNFCS_SMC_NFCS_Pos             _U_(0)                                               /**< (CCFG_SMCNFCS Position) SMC NAND Flash Chip Select x Assignment */
#define CCFG_SMCNFCS_SMC_NFCS_Msk             (_U_(0xF) << CCFG_SMCNFCS_SMC_NFCS_Pos)              /**< (CCFG_SMCNFCS Mask) SMC_NFCS */
#define CCFG_SMCNFCS_SMC_NFCS(value)          (CCFG_SMCNFCS_SMC_NFCS_Msk & ((value) << CCFG_SMCNFCS_SMC_NFCS_Pos)) 

/* -------- MATRIX_WPMR : (MATRIX Offset: 0x1E4) (R/W 32) Write Protection Mode Register -------- */
#define MATRIX_WPMR_WPEN_Pos                  _U_(0)                                               /**< (MATRIX_WPMR) Write Protection Enable Position */
#define MATRIX_WPMR_WPEN_Msk                  (_U_(0x1) << MATRIX_WPMR_WPEN_Pos)                   /**< (MATRIX_WPMR) Write Protection Enable Mask */
#define MATRIX_WPMR_WPEN(value)               (MATRIX_WPMR_WPEN_Msk & ((value) << MATRIX_WPMR_WPEN_Pos))
#define MATRIX_WPMR_WPKEY_Pos                 _U_(8)                                               /**< (MATRIX_WPMR) Write Protection Key Position */
#define MATRIX_WPMR_WPKEY_Msk                 (_U_(0xFFFFFF) << MATRIX_WPMR_WPKEY_Pos)             /**< (MATRIX_WPMR) Write Protection Key Mask */
#define MATRIX_WPMR_WPKEY(value)              (MATRIX_WPMR_WPKEY_Msk & ((value) << MATRIX_WPMR_WPKEY_Pos))
#define   MATRIX_WPMR_WPKEY_PASSWD_Val        _U_(0x4D4154)                                        /**< (MATRIX_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0.  */
#define MATRIX_WPMR_WPKEY_PASSWD              (MATRIX_WPMR_WPKEY_PASSWD_Val << MATRIX_WPMR_WPKEY_Pos) /**< (MATRIX_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0. Position  */
#define MATRIX_WPMR_Msk                       _U_(0xFFFFFF01)                                      /**< (MATRIX_WPMR) Register Mask  */


/* -------- MATRIX_WPSR : (MATRIX Offset: 0x1E8) ( R/ 32) Write Protection Status Register -------- */
#define MATRIX_WPSR_WPVS_Pos                  _U_(0)                                               /**< (MATRIX_WPSR) Write Protection Violation Status Position */
#define MATRIX_WPSR_WPVS_Msk                  (_U_(0x1) << MATRIX_WPSR_WPVS_Pos)                   /**< (MATRIX_WPSR) Write Protection Violation Status Mask */
#define MATRIX_WPSR_WPVS(value)               (MATRIX_WPSR_WPVS_Msk & ((value) << MATRIX_WPSR_WPVS_Pos))
#define MATRIX_WPSR_WPVSRC_Pos                _U_(8)                                               /**< (MATRIX_WPSR) Write Protection Violation Source Position */
#define MATRIX_WPSR_WPVSRC_Msk                (_U_(0xFFFF) << MATRIX_WPSR_WPVSRC_Pos)              /**< (MATRIX_WPSR) Write Protection Violation Source Mask */
#define MATRIX_WPSR_WPVSRC(value)             (MATRIX_WPSR_WPVSRC_Msk & ((value) << MATRIX_WPSR_WPVSRC_Pos))
#define MATRIX_WPSR_Msk                       _U_(0x00FFFF01)                                      /**< (MATRIX_WPSR) Register Mask  */


/** \brief MATRIX register offsets definitions */
#define MATRIX_PRAS_REG_OFST           (0x00)              /**< (MATRIX_PRAS) Priority Register A for Slave 0 Offset */
#define MATRIX_PRBS_REG_OFST           (0x04)              /**< (MATRIX_PRBS) Priority Register B for Slave 0 Offset */
#define MATRIX_MCFG_REG_OFST           (0x00)              /**< (MATRIX_MCFG) Master Configuration Register 0 Offset */
#define MATRIX_SCFG_REG_OFST           (0x40)              /**< (MATRIX_SCFG) Slave Configuration Register 0 Offset */
#define MATRIX_MRCR_REG_OFST           (0x100)             /**< (MATRIX_MRCR) Master Remap Control Register Offset */
#define CCFG_CAN0_REG_OFST             (0x110)             /**< (CCFG_CAN0) CAN0 Configuration Register Offset */
#define CCFG_SYSIO_REG_OFST            (0x114)             /**< (CCFG_SYSIO) System I/O and CAN1 Configuration Register Offset */
#define CCFG_PCCR_REG_OFST             (0x118)             /**< (CCFG_PCCR) Peripheral Clock Configuration Register Offset */
#define CCFG_DYNCKG_REG_OFST           (0x11C)             /**< (CCFG_DYNCKG) Dynamic Clock Gating Register Offset */
#define CCFG_SMCNFCS_REG_OFST          (0x124)             /**< (CCFG_SMCNFCS) SMC NAND Flash Chip Select Configuration Register Offset */
#define MATRIX_WPMR_REG_OFST           (0x1E4)             /**< (MATRIX_WPMR) Write Protection Mode Register Offset */
#define MATRIX_WPSR_REG_OFST           (0x1E8)             /**< (MATRIX_WPSR) Write Protection Status Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief MATRIX_PR register API structure */
typedef struct
{
  __IO  uint32_t                       MATRIX_PRAS;        /**< Offset: 0x00 (R/W  32) Priority Register A for Slave 0 */
  __IO  uint32_t                       MATRIX_PRBS;        /**< Offset: 0x04 (R/W  32) Priority Register B for Slave 0 */
} matrix_pr_registers_t;

#define MATRIX_PR_NUMBER _U_(9)

/** \brief MATRIX register API structure */
typedef struct
{
  __IO  uint32_t                       MATRIX_MCFG[13];    /**< Offset: 0x00 (R/W  32) Master Configuration Register 0 */
  __I   uint8_t                        Reserved1[0x0C];
  __IO  uint32_t                       MATRIX_SCFG[9];     /**< Offset: 0x40 (R/W  32) Slave Configuration Register 0 */
  __I   uint8_t                        Reserved2[0x1C];
        matrix_pr_registers_t          MATRIX_PR[MATRIX_PR_NUMBER]; /**< Offset: 0x80 Priority Register A for Slave 0 */
  __I   uint8_t                        Reserved3[0x38];
  __IO  uint32_t                       MATRIX_MRCR;        /**< Offset: 0x100 (R/W  32) Master Remap Control Register */
  __I   uint8_t                        Reserved4[0x0C];
  __IO  uint32_t                       CCFG_CAN0;          /**< Offset: 0x110 (R/W  32) CAN0 Configuration Register */
  __IO  uint32_t                       CCFG_SYSIO;         /**< Offset: 0x114 (R/W  32) System I/O and CAN1 Configuration Register */
  __IO  uint32_t                       CCFG_PCCR;          /**< Offset: 0x118 (R/W  32) Peripheral Clock Configuration Register */
  __IO  uint32_t                       CCFG_DYNCKG;        /**< Offset: 0x11C (R/W  32) Dynamic Clock Gating Register */
  __I   uint8_t                        Reserved5[0x04];
  __IO  uint32_t                       CCFG_SMCNFCS;       /**< Offset: 0x124 (R/W  32) SMC NAND Flash Chip Select Configuration Register */
  __I   uint8_t                        Reserved6[0xBC];
  __IO  uint32_t                       MATRIX_WPMR;        /**< Offset: 0x1E4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       MATRIX_WPSR;        /**< Offset: 0x1E8 (R/   32) Write Protection Status Register */
} matrix_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_MATRIX_COMPONENT_H_ */
