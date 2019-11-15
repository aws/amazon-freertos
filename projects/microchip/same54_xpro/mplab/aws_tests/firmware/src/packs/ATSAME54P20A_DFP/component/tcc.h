/**
 * \brief Component description for TCC
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
#ifndef _SAME54_TCC_COMPONENT_H_
#define _SAME54_TCC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR TCC                                          */
/* ************************************************************************** */

/* -------- TCC_CTRLA : (TCC Offset: 0x00) (R/W 32) Control A -------- */
#define TCC_CTRLA_RESETVALUE                  _U_(0x00)                                            /**<  (TCC_CTRLA) Control A  Reset Value */

#define TCC_CTRLA_SWRST_Pos                   _U_(0)                                               /**< (TCC_CTRLA) Software Reset Position */
#define TCC_CTRLA_SWRST_Msk                   (_U_(0x1) << TCC_CTRLA_SWRST_Pos)                    /**< (TCC_CTRLA) Software Reset Mask */
#define TCC_CTRLA_SWRST(value)                (TCC_CTRLA_SWRST_Msk & ((value) << TCC_CTRLA_SWRST_Pos))
#define TCC_CTRLA_ENABLE_Pos                  _U_(1)                                               /**< (TCC_CTRLA) Enable Position */
#define TCC_CTRLA_ENABLE_Msk                  (_U_(0x1) << TCC_CTRLA_ENABLE_Pos)                   /**< (TCC_CTRLA) Enable Mask */
#define TCC_CTRLA_ENABLE(value)               (TCC_CTRLA_ENABLE_Msk & ((value) << TCC_CTRLA_ENABLE_Pos))
#define TCC_CTRLA_RESOLUTION_Pos              _U_(5)                                               /**< (TCC_CTRLA) Enhanced Resolution Position */
#define TCC_CTRLA_RESOLUTION_Msk              (_U_(0x3) << TCC_CTRLA_RESOLUTION_Pos)               /**< (TCC_CTRLA) Enhanced Resolution Mask */
#define TCC_CTRLA_RESOLUTION(value)           (TCC_CTRLA_RESOLUTION_Msk & ((value) << TCC_CTRLA_RESOLUTION_Pos))
#define   TCC_CTRLA_RESOLUTION_NONE_Val       _U_(0x0)                                             /**< (TCC_CTRLA) Dithering is disabled  */
#define   TCC_CTRLA_RESOLUTION_DITH4_Val      _U_(0x1)                                             /**< (TCC_CTRLA) Dithering is done every 16 PWM frames  */
#define   TCC_CTRLA_RESOLUTION_DITH5_Val      _U_(0x2)                                             /**< (TCC_CTRLA) Dithering is done every 32 PWM frames  */
#define   TCC_CTRLA_RESOLUTION_DITH6_Val      _U_(0x3)                                             /**< (TCC_CTRLA) Dithering is done every 64 PWM frames  */
#define TCC_CTRLA_RESOLUTION_NONE             (TCC_CTRLA_RESOLUTION_NONE_Val << TCC_CTRLA_RESOLUTION_Pos) /**< (TCC_CTRLA) Dithering is disabled Position  */
#define TCC_CTRLA_RESOLUTION_DITH4            (TCC_CTRLA_RESOLUTION_DITH4_Val << TCC_CTRLA_RESOLUTION_Pos) /**< (TCC_CTRLA) Dithering is done every 16 PWM frames Position  */
#define TCC_CTRLA_RESOLUTION_DITH5            (TCC_CTRLA_RESOLUTION_DITH5_Val << TCC_CTRLA_RESOLUTION_Pos) /**< (TCC_CTRLA) Dithering is done every 32 PWM frames Position  */
#define TCC_CTRLA_RESOLUTION_DITH6            (TCC_CTRLA_RESOLUTION_DITH6_Val << TCC_CTRLA_RESOLUTION_Pos) /**< (TCC_CTRLA) Dithering is done every 64 PWM frames Position  */
#define TCC_CTRLA_PRESCALER_Pos               _U_(8)                                               /**< (TCC_CTRLA) Prescaler Position */
#define TCC_CTRLA_PRESCALER_Msk               (_U_(0x7) << TCC_CTRLA_PRESCALER_Pos)                /**< (TCC_CTRLA) Prescaler Mask */
#define TCC_CTRLA_PRESCALER(value)            (TCC_CTRLA_PRESCALER_Msk & ((value) << TCC_CTRLA_PRESCALER_Pos))
#define   TCC_CTRLA_PRESCALER_DIV1_Val        _U_(0x0)                                             /**< (TCC_CTRLA) No division  */
#define   TCC_CTRLA_PRESCALER_DIV2_Val        _U_(0x1)                                             /**< (TCC_CTRLA) Divide by 2  */
#define   TCC_CTRLA_PRESCALER_DIV4_Val        _U_(0x2)                                             /**< (TCC_CTRLA) Divide by 4  */
#define   TCC_CTRLA_PRESCALER_DIV8_Val        _U_(0x3)                                             /**< (TCC_CTRLA) Divide by 8  */
#define   TCC_CTRLA_PRESCALER_DIV16_Val       _U_(0x4)                                             /**< (TCC_CTRLA) Divide by 16  */
#define   TCC_CTRLA_PRESCALER_DIV64_Val       _U_(0x5)                                             /**< (TCC_CTRLA) Divide by 64  */
#define   TCC_CTRLA_PRESCALER_DIV256_Val      _U_(0x6)                                             /**< (TCC_CTRLA) Divide by 256  */
#define   TCC_CTRLA_PRESCALER_DIV1024_Val     _U_(0x7)                                             /**< (TCC_CTRLA) Divide by 1024  */
#define TCC_CTRLA_PRESCALER_DIV1              (TCC_CTRLA_PRESCALER_DIV1_Val << TCC_CTRLA_PRESCALER_Pos) /**< (TCC_CTRLA) No division Position  */
#define TCC_CTRLA_PRESCALER_DIV2              (TCC_CTRLA_PRESCALER_DIV2_Val << TCC_CTRLA_PRESCALER_Pos) /**< (TCC_CTRLA) Divide by 2 Position  */
#define TCC_CTRLA_PRESCALER_DIV4              (TCC_CTRLA_PRESCALER_DIV4_Val << TCC_CTRLA_PRESCALER_Pos) /**< (TCC_CTRLA) Divide by 4 Position  */
#define TCC_CTRLA_PRESCALER_DIV8              (TCC_CTRLA_PRESCALER_DIV8_Val << TCC_CTRLA_PRESCALER_Pos) /**< (TCC_CTRLA) Divide by 8 Position  */
#define TCC_CTRLA_PRESCALER_DIV16             (TCC_CTRLA_PRESCALER_DIV16_Val << TCC_CTRLA_PRESCALER_Pos) /**< (TCC_CTRLA) Divide by 16 Position  */
#define TCC_CTRLA_PRESCALER_DIV64             (TCC_CTRLA_PRESCALER_DIV64_Val << TCC_CTRLA_PRESCALER_Pos) /**< (TCC_CTRLA) Divide by 64 Position  */
#define TCC_CTRLA_PRESCALER_DIV256            (TCC_CTRLA_PRESCALER_DIV256_Val << TCC_CTRLA_PRESCALER_Pos) /**< (TCC_CTRLA) Divide by 256 Position  */
#define TCC_CTRLA_PRESCALER_DIV1024           (TCC_CTRLA_PRESCALER_DIV1024_Val << TCC_CTRLA_PRESCALER_Pos) /**< (TCC_CTRLA) Divide by 1024 Position  */
#define TCC_CTRLA_RUNSTDBY_Pos                _U_(11)                                              /**< (TCC_CTRLA) Run in Standby Position */
#define TCC_CTRLA_RUNSTDBY_Msk                (_U_(0x1) << TCC_CTRLA_RUNSTDBY_Pos)                 /**< (TCC_CTRLA) Run in Standby Mask */
#define TCC_CTRLA_RUNSTDBY(value)             (TCC_CTRLA_RUNSTDBY_Msk & ((value) << TCC_CTRLA_RUNSTDBY_Pos))
#define TCC_CTRLA_PRESCSYNC_Pos               _U_(12)                                              /**< (TCC_CTRLA) Prescaler and Counter Synchronization Selection Position */
#define TCC_CTRLA_PRESCSYNC_Msk               (_U_(0x3) << TCC_CTRLA_PRESCSYNC_Pos)                /**< (TCC_CTRLA) Prescaler and Counter Synchronization Selection Mask */
#define TCC_CTRLA_PRESCSYNC(value)            (TCC_CTRLA_PRESCSYNC_Msk & ((value) << TCC_CTRLA_PRESCSYNC_Pos))
#define   TCC_CTRLA_PRESCSYNC_GCLK_Val        _U_(0x0)                                             /**< (TCC_CTRLA) Reload or reset counter on next GCLK  */
#define   TCC_CTRLA_PRESCSYNC_PRESC_Val       _U_(0x1)                                             /**< (TCC_CTRLA) Reload or reset counter on next prescaler clock  */
#define   TCC_CTRLA_PRESCSYNC_RESYNC_Val      _U_(0x2)                                             /**< (TCC_CTRLA) Reload or reset counter on next GCLK and reset prescaler counter  */
#define TCC_CTRLA_PRESCSYNC_GCLK              (TCC_CTRLA_PRESCSYNC_GCLK_Val << TCC_CTRLA_PRESCSYNC_Pos) /**< (TCC_CTRLA) Reload or reset counter on next GCLK Position  */
#define TCC_CTRLA_PRESCSYNC_PRESC             (TCC_CTRLA_PRESCSYNC_PRESC_Val << TCC_CTRLA_PRESCSYNC_Pos) /**< (TCC_CTRLA) Reload or reset counter on next prescaler clock Position  */
#define TCC_CTRLA_PRESCSYNC_RESYNC            (TCC_CTRLA_PRESCSYNC_RESYNC_Val << TCC_CTRLA_PRESCSYNC_Pos) /**< (TCC_CTRLA) Reload or reset counter on next GCLK and reset prescaler counter Position  */
#define TCC_CTRLA_ALOCK_Pos                   _U_(14)                                              /**< (TCC_CTRLA) Auto Lock Position */
#define TCC_CTRLA_ALOCK_Msk                   (_U_(0x1) << TCC_CTRLA_ALOCK_Pos)                    /**< (TCC_CTRLA) Auto Lock Mask */
#define TCC_CTRLA_ALOCK(value)                (TCC_CTRLA_ALOCK_Msk & ((value) << TCC_CTRLA_ALOCK_Pos))
#define TCC_CTRLA_MSYNC_Pos                   _U_(15)                                              /**< (TCC_CTRLA) Master Synchronization (only for TCC Slave Instance) Position */
#define TCC_CTRLA_MSYNC_Msk                   (_U_(0x1) << TCC_CTRLA_MSYNC_Pos)                    /**< (TCC_CTRLA) Master Synchronization (only for TCC Slave Instance) Mask */
#define TCC_CTRLA_MSYNC(value)                (TCC_CTRLA_MSYNC_Msk & ((value) << TCC_CTRLA_MSYNC_Pos))
#define TCC_CTRLA_DMAOS_Pos                   _U_(23)                                              /**< (TCC_CTRLA) DMA One-shot Trigger Mode Position */
#define TCC_CTRLA_DMAOS_Msk                   (_U_(0x1) << TCC_CTRLA_DMAOS_Pos)                    /**< (TCC_CTRLA) DMA One-shot Trigger Mode Mask */
#define TCC_CTRLA_DMAOS(value)                (TCC_CTRLA_DMAOS_Msk & ((value) << TCC_CTRLA_DMAOS_Pos))
#define TCC_CTRLA_CPTEN0_Pos                  _U_(24)                                              /**< (TCC_CTRLA) Capture Channel 0 Enable Position */
#define TCC_CTRLA_CPTEN0_Msk                  (_U_(0x1) << TCC_CTRLA_CPTEN0_Pos)                   /**< (TCC_CTRLA) Capture Channel 0 Enable Mask */
#define TCC_CTRLA_CPTEN0(value)               (TCC_CTRLA_CPTEN0_Msk & ((value) << TCC_CTRLA_CPTEN0_Pos))
#define TCC_CTRLA_CPTEN1_Pos                  _U_(25)                                              /**< (TCC_CTRLA) Capture Channel 1 Enable Position */
#define TCC_CTRLA_CPTEN1_Msk                  (_U_(0x1) << TCC_CTRLA_CPTEN1_Pos)                   /**< (TCC_CTRLA) Capture Channel 1 Enable Mask */
#define TCC_CTRLA_CPTEN1(value)               (TCC_CTRLA_CPTEN1_Msk & ((value) << TCC_CTRLA_CPTEN1_Pos))
#define TCC_CTRLA_CPTEN2_Pos                  _U_(26)                                              /**< (TCC_CTRLA) Capture Channel 2 Enable Position */
#define TCC_CTRLA_CPTEN2_Msk                  (_U_(0x1) << TCC_CTRLA_CPTEN2_Pos)                   /**< (TCC_CTRLA) Capture Channel 2 Enable Mask */
#define TCC_CTRLA_CPTEN2(value)               (TCC_CTRLA_CPTEN2_Msk & ((value) << TCC_CTRLA_CPTEN2_Pos))
#define TCC_CTRLA_CPTEN3_Pos                  _U_(27)                                              /**< (TCC_CTRLA) Capture Channel 3 Enable Position */
#define TCC_CTRLA_CPTEN3_Msk                  (_U_(0x1) << TCC_CTRLA_CPTEN3_Pos)                   /**< (TCC_CTRLA) Capture Channel 3 Enable Mask */
#define TCC_CTRLA_CPTEN3(value)               (TCC_CTRLA_CPTEN3_Msk & ((value) << TCC_CTRLA_CPTEN3_Pos))
#define TCC_CTRLA_CPTEN4_Pos                  _U_(28)                                              /**< (TCC_CTRLA) Capture Channel 4 Enable Position */
#define TCC_CTRLA_CPTEN4_Msk                  (_U_(0x1) << TCC_CTRLA_CPTEN4_Pos)                   /**< (TCC_CTRLA) Capture Channel 4 Enable Mask */
#define TCC_CTRLA_CPTEN4(value)               (TCC_CTRLA_CPTEN4_Msk & ((value) << TCC_CTRLA_CPTEN4_Pos))
#define TCC_CTRLA_CPTEN5_Pos                  _U_(29)                                              /**< (TCC_CTRLA) Capture Channel 5 Enable Position */
#define TCC_CTRLA_CPTEN5_Msk                  (_U_(0x1) << TCC_CTRLA_CPTEN5_Pos)                   /**< (TCC_CTRLA) Capture Channel 5 Enable Mask */
#define TCC_CTRLA_CPTEN5(value)               (TCC_CTRLA_CPTEN5_Msk & ((value) << TCC_CTRLA_CPTEN5_Pos))
#define TCC_CTRLA_Msk                         _U_(0x3F80FF63)                                      /**< (TCC_CTRLA) Register Mask  */

#define TCC_CTRLA_CPTEN_Pos                   _U_(24)                                              /**< (TCC_CTRLA Position) Capture Channel 5 Enable */
#define TCC_CTRLA_CPTEN_Msk                   (_U_(0x3F) << TCC_CTRLA_CPTEN_Pos)                   /**< (TCC_CTRLA Mask) CPTEN */
#define TCC_CTRLA_CPTEN(value)                (TCC_CTRLA_CPTEN_Msk & ((value) << TCC_CTRLA_CPTEN_Pos)) 

/* -------- TCC_CTRLBCLR : (TCC Offset: 0x04) (R/W 8) Control B Clear -------- */
#define TCC_CTRLBCLR_RESETVALUE               _U_(0x00)                                            /**<  (TCC_CTRLBCLR) Control B Clear  Reset Value */

#define TCC_CTRLBCLR_DIR_Pos                  _U_(0)                                               /**< (TCC_CTRLBCLR) Counter Direction Position */
#define TCC_CTRLBCLR_DIR_Msk                  (_U_(0x1) << TCC_CTRLBCLR_DIR_Pos)                   /**< (TCC_CTRLBCLR) Counter Direction Mask */
#define TCC_CTRLBCLR_DIR(value)               (TCC_CTRLBCLR_DIR_Msk & ((value) << TCC_CTRLBCLR_DIR_Pos))
#define TCC_CTRLBCLR_LUPD_Pos                 _U_(1)                                               /**< (TCC_CTRLBCLR) Lock Update Position */
#define TCC_CTRLBCLR_LUPD_Msk                 (_U_(0x1) << TCC_CTRLBCLR_LUPD_Pos)                  /**< (TCC_CTRLBCLR) Lock Update Mask */
#define TCC_CTRLBCLR_LUPD(value)              (TCC_CTRLBCLR_LUPD_Msk & ((value) << TCC_CTRLBCLR_LUPD_Pos))
#define TCC_CTRLBCLR_ONESHOT_Pos              _U_(2)                                               /**< (TCC_CTRLBCLR) One-Shot Position */
#define TCC_CTRLBCLR_ONESHOT_Msk              (_U_(0x1) << TCC_CTRLBCLR_ONESHOT_Pos)               /**< (TCC_CTRLBCLR) One-Shot Mask */
#define TCC_CTRLBCLR_ONESHOT(value)           (TCC_CTRLBCLR_ONESHOT_Msk & ((value) << TCC_CTRLBCLR_ONESHOT_Pos))
#define TCC_CTRLBCLR_IDXCMD_Pos               _U_(3)                                               /**< (TCC_CTRLBCLR) Ramp Index Command Position */
#define TCC_CTRLBCLR_IDXCMD_Msk               (_U_(0x3) << TCC_CTRLBCLR_IDXCMD_Pos)                /**< (TCC_CTRLBCLR) Ramp Index Command Mask */
#define TCC_CTRLBCLR_IDXCMD(value)            (TCC_CTRLBCLR_IDXCMD_Msk & ((value) << TCC_CTRLBCLR_IDXCMD_Pos))
#define   TCC_CTRLBCLR_IDXCMD_DISABLE_Val     _U_(0x0)                                             /**< (TCC_CTRLBCLR) Command disabled: Index toggles between cycles A and B  */
#define   TCC_CTRLBCLR_IDXCMD_SET_Val         _U_(0x1)                                             /**< (TCC_CTRLBCLR) Set index: cycle B will be forced in the next cycle  */
#define   TCC_CTRLBCLR_IDXCMD_CLEAR_Val       _U_(0x2)                                             /**< (TCC_CTRLBCLR) Clear index: cycle A will be forced in the next cycle  */
#define   TCC_CTRLBCLR_IDXCMD_HOLD_Val        _U_(0x3)                                             /**< (TCC_CTRLBCLR) Hold index: the next cycle will be the same as the current cycle  */
#define TCC_CTRLBCLR_IDXCMD_DISABLE           (TCC_CTRLBCLR_IDXCMD_DISABLE_Val << TCC_CTRLBCLR_IDXCMD_Pos) /**< (TCC_CTRLBCLR) Command disabled: Index toggles between cycles A and B Position  */
#define TCC_CTRLBCLR_IDXCMD_SET               (TCC_CTRLBCLR_IDXCMD_SET_Val << TCC_CTRLBCLR_IDXCMD_Pos) /**< (TCC_CTRLBCLR) Set index: cycle B will be forced in the next cycle Position  */
#define TCC_CTRLBCLR_IDXCMD_CLEAR             (TCC_CTRLBCLR_IDXCMD_CLEAR_Val << TCC_CTRLBCLR_IDXCMD_Pos) /**< (TCC_CTRLBCLR) Clear index: cycle A will be forced in the next cycle Position  */
#define TCC_CTRLBCLR_IDXCMD_HOLD              (TCC_CTRLBCLR_IDXCMD_HOLD_Val << TCC_CTRLBCLR_IDXCMD_Pos) /**< (TCC_CTRLBCLR) Hold index: the next cycle will be the same as the current cycle Position  */
#define TCC_CTRLBCLR_CMD_Pos                  _U_(5)                                               /**< (TCC_CTRLBCLR) TCC Command Position */
#define TCC_CTRLBCLR_CMD_Msk                  (_U_(0x7) << TCC_CTRLBCLR_CMD_Pos)                   /**< (TCC_CTRLBCLR) TCC Command Mask */
#define TCC_CTRLBCLR_CMD(value)               (TCC_CTRLBCLR_CMD_Msk & ((value) << TCC_CTRLBCLR_CMD_Pos))
#define   TCC_CTRLBCLR_CMD_NONE_Val           _U_(0x0)                                             /**< (TCC_CTRLBCLR) No action  */
#define   TCC_CTRLBCLR_CMD_RETRIGGER_Val      _U_(0x1)                                             /**< (TCC_CTRLBCLR) Clear start, restart or retrigger  */
#define   TCC_CTRLBCLR_CMD_STOP_Val           _U_(0x2)                                             /**< (TCC_CTRLBCLR) Force stop  */
#define   TCC_CTRLBCLR_CMD_UPDATE_Val         _U_(0x3)                                             /**< (TCC_CTRLBCLR) Force update or double buffered registers  */
#define   TCC_CTRLBCLR_CMD_READSYNC_Val       _U_(0x4)                                             /**< (TCC_CTRLBCLR) Force COUNT read synchronization  */
#define   TCC_CTRLBCLR_CMD_DMAOS_Val          _U_(0x5)                                             /**< (TCC_CTRLBCLR) One-shot DMA trigger  */
#define TCC_CTRLBCLR_CMD_NONE                 (TCC_CTRLBCLR_CMD_NONE_Val << TCC_CTRLBCLR_CMD_Pos)  /**< (TCC_CTRLBCLR) No action Position  */
#define TCC_CTRLBCLR_CMD_RETRIGGER            (TCC_CTRLBCLR_CMD_RETRIGGER_Val << TCC_CTRLBCLR_CMD_Pos) /**< (TCC_CTRLBCLR) Clear start, restart or retrigger Position  */
#define TCC_CTRLBCLR_CMD_STOP                 (TCC_CTRLBCLR_CMD_STOP_Val << TCC_CTRLBCLR_CMD_Pos)  /**< (TCC_CTRLBCLR) Force stop Position  */
#define TCC_CTRLBCLR_CMD_UPDATE               (TCC_CTRLBCLR_CMD_UPDATE_Val << TCC_CTRLBCLR_CMD_Pos) /**< (TCC_CTRLBCLR) Force update or double buffered registers Position  */
#define TCC_CTRLBCLR_CMD_READSYNC             (TCC_CTRLBCLR_CMD_READSYNC_Val << TCC_CTRLBCLR_CMD_Pos) /**< (TCC_CTRLBCLR) Force COUNT read synchronization Position  */
#define TCC_CTRLBCLR_CMD_DMAOS                (TCC_CTRLBCLR_CMD_DMAOS_Val << TCC_CTRLBCLR_CMD_Pos) /**< (TCC_CTRLBCLR) One-shot DMA trigger Position  */
#define TCC_CTRLBCLR_Msk                      _U_(0xFF)                                            /**< (TCC_CTRLBCLR) Register Mask  */


/* -------- TCC_CTRLBSET : (TCC Offset: 0x05) (R/W 8) Control B Set -------- */
#define TCC_CTRLBSET_RESETVALUE               _U_(0x00)                                            /**<  (TCC_CTRLBSET) Control B Set  Reset Value */

#define TCC_CTRLBSET_DIR_Pos                  _U_(0)                                               /**< (TCC_CTRLBSET) Counter Direction Position */
#define TCC_CTRLBSET_DIR_Msk                  (_U_(0x1) << TCC_CTRLBSET_DIR_Pos)                   /**< (TCC_CTRLBSET) Counter Direction Mask */
#define TCC_CTRLBSET_DIR(value)               (TCC_CTRLBSET_DIR_Msk & ((value) << TCC_CTRLBSET_DIR_Pos))
#define TCC_CTRLBSET_LUPD_Pos                 _U_(1)                                               /**< (TCC_CTRLBSET) Lock Update Position */
#define TCC_CTRLBSET_LUPD_Msk                 (_U_(0x1) << TCC_CTRLBSET_LUPD_Pos)                  /**< (TCC_CTRLBSET) Lock Update Mask */
#define TCC_CTRLBSET_LUPD(value)              (TCC_CTRLBSET_LUPD_Msk & ((value) << TCC_CTRLBSET_LUPD_Pos))
#define TCC_CTRLBSET_ONESHOT_Pos              _U_(2)                                               /**< (TCC_CTRLBSET) One-Shot Position */
#define TCC_CTRLBSET_ONESHOT_Msk              (_U_(0x1) << TCC_CTRLBSET_ONESHOT_Pos)               /**< (TCC_CTRLBSET) One-Shot Mask */
#define TCC_CTRLBSET_ONESHOT(value)           (TCC_CTRLBSET_ONESHOT_Msk & ((value) << TCC_CTRLBSET_ONESHOT_Pos))
#define TCC_CTRLBSET_IDXCMD_Pos               _U_(3)                                               /**< (TCC_CTRLBSET) Ramp Index Command Position */
#define TCC_CTRLBSET_IDXCMD_Msk               (_U_(0x3) << TCC_CTRLBSET_IDXCMD_Pos)                /**< (TCC_CTRLBSET) Ramp Index Command Mask */
#define TCC_CTRLBSET_IDXCMD(value)            (TCC_CTRLBSET_IDXCMD_Msk & ((value) << TCC_CTRLBSET_IDXCMD_Pos))
#define   TCC_CTRLBSET_IDXCMD_DISABLE_Val     _U_(0x0)                                             /**< (TCC_CTRLBSET) Command disabled: Index toggles between cycles A and B  */
#define   TCC_CTRLBSET_IDXCMD_SET_Val         _U_(0x1)                                             /**< (TCC_CTRLBSET) Set index: cycle B will be forced in the next cycle  */
#define   TCC_CTRLBSET_IDXCMD_CLEAR_Val       _U_(0x2)                                             /**< (TCC_CTRLBSET) Clear index: cycle A will be forced in the next cycle  */
#define   TCC_CTRLBSET_IDXCMD_HOLD_Val        _U_(0x3)                                             /**< (TCC_CTRLBSET) Hold index: the next cycle will be the same as the current cycle  */
#define TCC_CTRLBSET_IDXCMD_DISABLE           (TCC_CTRLBSET_IDXCMD_DISABLE_Val << TCC_CTRLBSET_IDXCMD_Pos) /**< (TCC_CTRLBSET) Command disabled: Index toggles between cycles A and B Position  */
#define TCC_CTRLBSET_IDXCMD_SET               (TCC_CTRLBSET_IDXCMD_SET_Val << TCC_CTRLBSET_IDXCMD_Pos) /**< (TCC_CTRLBSET) Set index: cycle B will be forced in the next cycle Position  */
#define TCC_CTRLBSET_IDXCMD_CLEAR             (TCC_CTRLBSET_IDXCMD_CLEAR_Val << TCC_CTRLBSET_IDXCMD_Pos) /**< (TCC_CTRLBSET) Clear index: cycle A will be forced in the next cycle Position  */
#define TCC_CTRLBSET_IDXCMD_HOLD              (TCC_CTRLBSET_IDXCMD_HOLD_Val << TCC_CTRLBSET_IDXCMD_Pos) /**< (TCC_CTRLBSET) Hold index: the next cycle will be the same as the current cycle Position  */
#define TCC_CTRLBSET_CMD_Pos                  _U_(5)                                               /**< (TCC_CTRLBSET) TCC Command Position */
#define TCC_CTRLBSET_CMD_Msk                  (_U_(0x7) << TCC_CTRLBSET_CMD_Pos)                   /**< (TCC_CTRLBSET) TCC Command Mask */
#define TCC_CTRLBSET_CMD(value)               (TCC_CTRLBSET_CMD_Msk & ((value) << TCC_CTRLBSET_CMD_Pos))
#define   TCC_CTRLBSET_CMD_NONE_Val           _U_(0x0)                                             /**< (TCC_CTRLBSET) No action  */
#define   TCC_CTRLBSET_CMD_RETRIGGER_Val      _U_(0x1)                                             /**< (TCC_CTRLBSET) Clear start, restart or retrigger  */
#define   TCC_CTRLBSET_CMD_STOP_Val           _U_(0x2)                                             /**< (TCC_CTRLBSET) Force stop  */
#define   TCC_CTRLBSET_CMD_UPDATE_Val         _U_(0x3)                                             /**< (TCC_CTRLBSET) Force update or double buffered registers  */
#define   TCC_CTRLBSET_CMD_READSYNC_Val       _U_(0x4)                                             /**< (TCC_CTRLBSET) Force COUNT read synchronization  */
#define   TCC_CTRLBSET_CMD_DMAOS_Val          _U_(0x5)                                             /**< (TCC_CTRLBSET) One-shot DMA trigger  */
#define TCC_CTRLBSET_CMD_NONE                 (TCC_CTRLBSET_CMD_NONE_Val << TCC_CTRLBSET_CMD_Pos)  /**< (TCC_CTRLBSET) No action Position  */
#define TCC_CTRLBSET_CMD_RETRIGGER            (TCC_CTRLBSET_CMD_RETRIGGER_Val << TCC_CTRLBSET_CMD_Pos) /**< (TCC_CTRLBSET) Clear start, restart or retrigger Position  */
#define TCC_CTRLBSET_CMD_STOP                 (TCC_CTRLBSET_CMD_STOP_Val << TCC_CTRLBSET_CMD_Pos)  /**< (TCC_CTRLBSET) Force stop Position  */
#define TCC_CTRLBSET_CMD_UPDATE               (TCC_CTRLBSET_CMD_UPDATE_Val << TCC_CTRLBSET_CMD_Pos) /**< (TCC_CTRLBSET) Force update or double buffered registers Position  */
#define TCC_CTRLBSET_CMD_READSYNC             (TCC_CTRLBSET_CMD_READSYNC_Val << TCC_CTRLBSET_CMD_Pos) /**< (TCC_CTRLBSET) Force COUNT read synchronization Position  */
#define TCC_CTRLBSET_CMD_DMAOS                (TCC_CTRLBSET_CMD_DMAOS_Val << TCC_CTRLBSET_CMD_Pos) /**< (TCC_CTRLBSET) One-shot DMA trigger Position  */
#define TCC_CTRLBSET_Msk                      _U_(0xFF)                                            /**< (TCC_CTRLBSET) Register Mask  */


/* -------- TCC_SYNCBUSY : (TCC Offset: 0x08) ( R/ 32) Synchronization Busy -------- */
#define TCC_SYNCBUSY_RESETVALUE               _U_(0x00)                                            /**<  (TCC_SYNCBUSY) Synchronization Busy  Reset Value */

#define TCC_SYNCBUSY_SWRST_Pos                _U_(0)                                               /**< (TCC_SYNCBUSY) Swrst Busy Position */
#define TCC_SYNCBUSY_SWRST_Msk                (_U_(0x1) << TCC_SYNCBUSY_SWRST_Pos)                 /**< (TCC_SYNCBUSY) Swrst Busy Mask */
#define TCC_SYNCBUSY_SWRST(value)             (TCC_SYNCBUSY_SWRST_Msk & ((value) << TCC_SYNCBUSY_SWRST_Pos))
#define TCC_SYNCBUSY_ENABLE_Pos               _U_(1)                                               /**< (TCC_SYNCBUSY) Enable Busy Position */
#define TCC_SYNCBUSY_ENABLE_Msk               (_U_(0x1) << TCC_SYNCBUSY_ENABLE_Pos)                /**< (TCC_SYNCBUSY) Enable Busy Mask */
#define TCC_SYNCBUSY_ENABLE(value)            (TCC_SYNCBUSY_ENABLE_Msk & ((value) << TCC_SYNCBUSY_ENABLE_Pos))
#define TCC_SYNCBUSY_CTRLB_Pos                _U_(2)                                               /**< (TCC_SYNCBUSY) Ctrlb Busy Position */
#define TCC_SYNCBUSY_CTRLB_Msk                (_U_(0x1) << TCC_SYNCBUSY_CTRLB_Pos)                 /**< (TCC_SYNCBUSY) Ctrlb Busy Mask */
#define TCC_SYNCBUSY_CTRLB(value)             (TCC_SYNCBUSY_CTRLB_Msk & ((value) << TCC_SYNCBUSY_CTRLB_Pos))
#define TCC_SYNCBUSY_STATUS_Pos               _U_(3)                                               /**< (TCC_SYNCBUSY) Status Busy Position */
#define TCC_SYNCBUSY_STATUS_Msk               (_U_(0x1) << TCC_SYNCBUSY_STATUS_Pos)                /**< (TCC_SYNCBUSY) Status Busy Mask */
#define TCC_SYNCBUSY_STATUS(value)            (TCC_SYNCBUSY_STATUS_Msk & ((value) << TCC_SYNCBUSY_STATUS_Pos))
#define TCC_SYNCBUSY_COUNT_Pos                _U_(4)                                               /**< (TCC_SYNCBUSY) Count Busy Position */
#define TCC_SYNCBUSY_COUNT_Msk                (_U_(0x1) << TCC_SYNCBUSY_COUNT_Pos)                 /**< (TCC_SYNCBUSY) Count Busy Mask */
#define TCC_SYNCBUSY_COUNT(value)             (TCC_SYNCBUSY_COUNT_Msk & ((value) << TCC_SYNCBUSY_COUNT_Pos))
#define TCC_SYNCBUSY_PATT_Pos                 _U_(5)                                               /**< (TCC_SYNCBUSY) Pattern Busy Position */
#define TCC_SYNCBUSY_PATT_Msk                 (_U_(0x1) << TCC_SYNCBUSY_PATT_Pos)                  /**< (TCC_SYNCBUSY) Pattern Busy Mask */
#define TCC_SYNCBUSY_PATT(value)              (TCC_SYNCBUSY_PATT_Msk & ((value) << TCC_SYNCBUSY_PATT_Pos))
#define TCC_SYNCBUSY_WAVE_Pos                 _U_(6)                                               /**< (TCC_SYNCBUSY) Wave Busy Position */
#define TCC_SYNCBUSY_WAVE_Msk                 (_U_(0x1) << TCC_SYNCBUSY_WAVE_Pos)                  /**< (TCC_SYNCBUSY) Wave Busy Mask */
#define TCC_SYNCBUSY_WAVE(value)              (TCC_SYNCBUSY_WAVE_Msk & ((value) << TCC_SYNCBUSY_WAVE_Pos))
#define TCC_SYNCBUSY_PER_Pos                  _U_(7)                                               /**< (TCC_SYNCBUSY) Period Busy Position */
#define TCC_SYNCBUSY_PER_Msk                  (_U_(0x1) << TCC_SYNCBUSY_PER_Pos)                   /**< (TCC_SYNCBUSY) Period Busy Mask */
#define TCC_SYNCBUSY_PER(value)               (TCC_SYNCBUSY_PER_Msk & ((value) << TCC_SYNCBUSY_PER_Pos))
#define TCC_SYNCBUSY_CC0_Pos                  _U_(8)                                               /**< (TCC_SYNCBUSY) Compare Channel 0 Busy Position */
#define TCC_SYNCBUSY_CC0_Msk                  (_U_(0x1) << TCC_SYNCBUSY_CC0_Pos)                   /**< (TCC_SYNCBUSY) Compare Channel 0 Busy Mask */
#define TCC_SYNCBUSY_CC0(value)               (TCC_SYNCBUSY_CC0_Msk & ((value) << TCC_SYNCBUSY_CC0_Pos))
#define TCC_SYNCBUSY_CC1_Pos                  _U_(9)                                               /**< (TCC_SYNCBUSY) Compare Channel 1 Busy Position */
#define TCC_SYNCBUSY_CC1_Msk                  (_U_(0x1) << TCC_SYNCBUSY_CC1_Pos)                   /**< (TCC_SYNCBUSY) Compare Channel 1 Busy Mask */
#define TCC_SYNCBUSY_CC1(value)               (TCC_SYNCBUSY_CC1_Msk & ((value) << TCC_SYNCBUSY_CC1_Pos))
#define TCC_SYNCBUSY_CC2_Pos                  _U_(10)                                              /**< (TCC_SYNCBUSY) Compare Channel 2 Busy Position */
#define TCC_SYNCBUSY_CC2_Msk                  (_U_(0x1) << TCC_SYNCBUSY_CC2_Pos)                   /**< (TCC_SYNCBUSY) Compare Channel 2 Busy Mask */
#define TCC_SYNCBUSY_CC2(value)               (TCC_SYNCBUSY_CC2_Msk & ((value) << TCC_SYNCBUSY_CC2_Pos))
#define TCC_SYNCBUSY_CC3_Pos                  _U_(11)                                              /**< (TCC_SYNCBUSY) Compare Channel 3 Busy Position */
#define TCC_SYNCBUSY_CC3_Msk                  (_U_(0x1) << TCC_SYNCBUSY_CC3_Pos)                   /**< (TCC_SYNCBUSY) Compare Channel 3 Busy Mask */
#define TCC_SYNCBUSY_CC3(value)               (TCC_SYNCBUSY_CC3_Msk & ((value) << TCC_SYNCBUSY_CC3_Pos))
#define TCC_SYNCBUSY_CC4_Pos                  _U_(12)                                              /**< (TCC_SYNCBUSY) Compare Channel 4 Busy Position */
#define TCC_SYNCBUSY_CC4_Msk                  (_U_(0x1) << TCC_SYNCBUSY_CC4_Pos)                   /**< (TCC_SYNCBUSY) Compare Channel 4 Busy Mask */
#define TCC_SYNCBUSY_CC4(value)               (TCC_SYNCBUSY_CC4_Msk & ((value) << TCC_SYNCBUSY_CC4_Pos))
#define TCC_SYNCBUSY_CC5_Pos                  _U_(13)                                              /**< (TCC_SYNCBUSY) Compare Channel 5 Busy Position */
#define TCC_SYNCBUSY_CC5_Msk                  (_U_(0x1) << TCC_SYNCBUSY_CC5_Pos)                   /**< (TCC_SYNCBUSY) Compare Channel 5 Busy Mask */
#define TCC_SYNCBUSY_CC5(value)               (TCC_SYNCBUSY_CC5_Msk & ((value) << TCC_SYNCBUSY_CC5_Pos))
#define TCC_SYNCBUSY_Msk                      _U_(0x00003FFF)                                      /**< (TCC_SYNCBUSY) Register Mask  */

#define TCC_SYNCBUSY_CC_Pos                   _U_(8)                                               /**< (TCC_SYNCBUSY Position) Compare Channel 5 Busy */
#define TCC_SYNCBUSY_CC_Msk                   (_U_(0x3F) << TCC_SYNCBUSY_CC_Pos)                   /**< (TCC_SYNCBUSY Mask) CC */
#define TCC_SYNCBUSY_CC(value)                (TCC_SYNCBUSY_CC_Msk & ((value) << TCC_SYNCBUSY_CC_Pos)) 

/* -------- TCC_FCTRLA : (TCC Offset: 0x0C) (R/W 32) Recoverable Fault A Configuration -------- */
#define TCC_FCTRLA_RESETVALUE                 _U_(0x00)                                            /**<  (TCC_FCTRLA) Recoverable Fault A Configuration  Reset Value */

#define TCC_FCTRLA_SRC_Pos                    _U_(0)                                               /**< (TCC_FCTRLA) Fault A Source Position */
#define TCC_FCTRLA_SRC_Msk                    (_U_(0x3) << TCC_FCTRLA_SRC_Pos)                     /**< (TCC_FCTRLA) Fault A Source Mask */
#define TCC_FCTRLA_SRC(value)                 (TCC_FCTRLA_SRC_Msk & ((value) << TCC_FCTRLA_SRC_Pos))
#define   TCC_FCTRLA_SRC_DISABLE_Val          _U_(0x0)                                             /**< (TCC_FCTRLA) Fault input disabled  */
#define   TCC_FCTRLA_SRC_ENABLE_Val           _U_(0x1)                                             /**< (TCC_FCTRLA) MCEx (x=0,1) event input  */
#define   TCC_FCTRLA_SRC_INVERT_Val           _U_(0x2)                                             /**< (TCC_FCTRLA) Inverted MCEx (x=0,1) event input  */
#define   TCC_FCTRLA_SRC_ALTFAULT_Val         _U_(0x3)                                             /**< (TCC_FCTRLA) Alternate fault (A or B) state at the end of the previous period  */
#define TCC_FCTRLA_SRC_DISABLE                (TCC_FCTRLA_SRC_DISABLE_Val << TCC_FCTRLA_SRC_Pos)   /**< (TCC_FCTRLA) Fault input disabled Position  */
#define TCC_FCTRLA_SRC_ENABLE                 (TCC_FCTRLA_SRC_ENABLE_Val << TCC_FCTRLA_SRC_Pos)    /**< (TCC_FCTRLA) MCEx (x=0,1) event input Position  */
#define TCC_FCTRLA_SRC_INVERT                 (TCC_FCTRLA_SRC_INVERT_Val << TCC_FCTRLA_SRC_Pos)    /**< (TCC_FCTRLA) Inverted MCEx (x=0,1) event input Position  */
#define TCC_FCTRLA_SRC_ALTFAULT               (TCC_FCTRLA_SRC_ALTFAULT_Val << TCC_FCTRLA_SRC_Pos)  /**< (TCC_FCTRLA) Alternate fault (A or B) state at the end of the previous period Position  */
#define TCC_FCTRLA_KEEP_Pos                   _U_(3)                                               /**< (TCC_FCTRLA) Fault A Keeper Position */
#define TCC_FCTRLA_KEEP_Msk                   (_U_(0x1) << TCC_FCTRLA_KEEP_Pos)                    /**< (TCC_FCTRLA) Fault A Keeper Mask */
#define TCC_FCTRLA_KEEP(value)                (TCC_FCTRLA_KEEP_Msk & ((value) << TCC_FCTRLA_KEEP_Pos))
#define TCC_FCTRLA_QUAL_Pos                   _U_(4)                                               /**< (TCC_FCTRLA) Fault A Qualification Position */
#define TCC_FCTRLA_QUAL_Msk                   (_U_(0x1) << TCC_FCTRLA_QUAL_Pos)                    /**< (TCC_FCTRLA) Fault A Qualification Mask */
#define TCC_FCTRLA_QUAL(value)                (TCC_FCTRLA_QUAL_Msk & ((value) << TCC_FCTRLA_QUAL_Pos))
#define TCC_FCTRLA_BLANK_Pos                  _U_(5)                                               /**< (TCC_FCTRLA) Fault A Blanking Mode Position */
#define TCC_FCTRLA_BLANK_Msk                  (_U_(0x3) << TCC_FCTRLA_BLANK_Pos)                   /**< (TCC_FCTRLA) Fault A Blanking Mode Mask */
#define TCC_FCTRLA_BLANK(value)               (TCC_FCTRLA_BLANK_Msk & ((value) << TCC_FCTRLA_BLANK_Pos))
#define   TCC_FCTRLA_BLANK_START_Val          _U_(0x0)                                             /**< (TCC_FCTRLA) Blanking applied from start of the ramp  */
#define   TCC_FCTRLA_BLANK_RISE_Val           _U_(0x1)                                             /**< (TCC_FCTRLA) Blanking applied from rising edge of the output waveform  */
#define   TCC_FCTRLA_BLANK_FALL_Val           _U_(0x2)                                             /**< (TCC_FCTRLA) Blanking applied from falling edge of the output waveform  */
#define   TCC_FCTRLA_BLANK_BOTH_Val           _U_(0x3)                                             /**< (TCC_FCTRLA) Blanking applied from each toggle of the output waveform  */
#define TCC_FCTRLA_BLANK_START                (TCC_FCTRLA_BLANK_START_Val << TCC_FCTRLA_BLANK_Pos) /**< (TCC_FCTRLA) Blanking applied from start of the ramp Position  */
#define TCC_FCTRLA_BLANK_RISE                 (TCC_FCTRLA_BLANK_RISE_Val << TCC_FCTRLA_BLANK_Pos)  /**< (TCC_FCTRLA) Blanking applied from rising edge of the output waveform Position  */
#define TCC_FCTRLA_BLANK_FALL                 (TCC_FCTRLA_BLANK_FALL_Val << TCC_FCTRLA_BLANK_Pos)  /**< (TCC_FCTRLA) Blanking applied from falling edge of the output waveform Position  */
#define TCC_FCTRLA_BLANK_BOTH                 (TCC_FCTRLA_BLANK_BOTH_Val << TCC_FCTRLA_BLANK_Pos)  /**< (TCC_FCTRLA) Blanking applied from each toggle of the output waveform Position  */
#define TCC_FCTRLA_RESTART_Pos                _U_(7)                                               /**< (TCC_FCTRLA) Fault A Restart Position */
#define TCC_FCTRLA_RESTART_Msk                (_U_(0x1) << TCC_FCTRLA_RESTART_Pos)                 /**< (TCC_FCTRLA) Fault A Restart Mask */
#define TCC_FCTRLA_RESTART(value)             (TCC_FCTRLA_RESTART_Msk & ((value) << TCC_FCTRLA_RESTART_Pos))
#define TCC_FCTRLA_HALT_Pos                   _U_(8)                                               /**< (TCC_FCTRLA) Fault A Halt Mode Position */
#define TCC_FCTRLA_HALT_Msk                   (_U_(0x3) << TCC_FCTRLA_HALT_Pos)                    /**< (TCC_FCTRLA) Fault A Halt Mode Mask */
#define TCC_FCTRLA_HALT(value)                (TCC_FCTRLA_HALT_Msk & ((value) << TCC_FCTRLA_HALT_Pos))
#define   TCC_FCTRLA_HALT_DISABLE_Val         _U_(0x0)                                             /**< (TCC_FCTRLA) Halt action disabled  */
#define   TCC_FCTRLA_HALT_HW_Val              _U_(0x1)                                             /**< (TCC_FCTRLA) Hardware halt action  */
#define   TCC_FCTRLA_HALT_SW_Val              _U_(0x2)                                             /**< (TCC_FCTRLA) Software halt action  */
#define   TCC_FCTRLA_HALT_NR_Val              _U_(0x3)                                             /**< (TCC_FCTRLA) Non-recoverable fault  */
#define TCC_FCTRLA_HALT_DISABLE               (TCC_FCTRLA_HALT_DISABLE_Val << TCC_FCTRLA_HALT_Pos) /**< (TCC_FCTRLA) Halt action disabled Position  */
#define TCC_FCTRLA_HALT_HW                    (TCC_FCTRLA_HALT_HW_Val << TCC_FCTRLA_HALT_Pos)      /**< (TCC_FCTRLA) Hardware halt action Position  */
#define TCC_FCTRLA_HALT_SW                    (TCC_FCTRLA_HALT_SW_Val << TCC_FCTRLA_HALT_Pos)      /**< (TCC_FCTRLA) Software halt action Position  */
#define TCC_FCTRLA_HALT_NR                    (TCC_FCTRLA_HALT_NR_Val << TCC_FCTRLA_HALT_Pos)      /**< (TCC_FCTRLA) Non-recoverable fault Position  */
#define TCC_FCTRLA_CHSEL_Pos                  _U_(10)                                              /**< (TCC_FCTRLA) Fault A Capture Channel Position */
#define TCC_FCTRLA_CHSEL_Msk                  (_U_(0x3) << TCC_FCTRLA_CHSEL_Pos)                   /**< (TCC_FCTRLA) Fault A Capture Channel Mask */
#define TCC_FCTRLA_CHSEL(value)               (TCC_FCTRLA_CHSEL_Msk & ((value) << TCC_FCTRLA_CHSEL_Pos))
#define   TCC_FCTRLA_CHSEL_CC0_Val            _U_(0x0)                                             /**< (TCC_FCTRLA) Capture value stored in channel 0  */
#define   TCC_FCTRLA_CHSEL_CC1_Val            _U_(0x1)                                             /**< (TCC_FCTRLA) Capture value stored in channel 1  */
#define   TCC_FCTRLA_CHSEL_CC2_Val            _U_(0x2)                                             /**< (TCC_FCTRLA) Capture value stored in channel 2  */
#define   TCC_FCTRLA_CHSEL_CC3_Val            _U_(0x3)                                             /**< (TCC_FCTRLA) Capture value stored in channel 3  */
#define TCC_FCTRLA_CHSEL_CC0                  (TCC_FCTRLA_CHSEL_CC0_Val << TCC_FCTRLA_CHSEL_Pos)   /**< (TCC_FCTRLA) Capture value stored in channel 0 Position  */
#define TCC_FCTRLA_CHSEL_CC1                  (TCC_FCTRLA_CHSEL_CC1_Val << TCC_FCTRLA_CHSEL_Pos)   /**< (TCC_FCTRLA) Capture value stored in channel 1 Position  */
#define TCC_FCTRLA_CHSEL_CC2                  (TCC_FCTRLA_CHSEL_CC2_Val << TCC_FCTRLA_CHSEL_Pos)   /**< (TCC_FCTRLA) Capture value stored in channel 2 Position  */
#define TCC_FCTRLA_CHSEL_CC3                  (TCC_FCTRLA_CHSEL_CC3_Val << TCC_FCTRLA_CHSEL_Pos)   /**< (TCC_FCTRLA) Capture value stored in channel 3 Position  */
#define TCC_FCTRLA_CAPTURE_Pos                _U_(12)                                              /**< (TCC_FCTRLA) Fault A Capture Action Position */
#define TCC_FCTRLA_CAPTURE_Msk                (_U_(0x7) << TCC_FCTRLA_CAPTURE_Pos)                 /**< (TCC_FCTRLA) Fault A Capture Action Mask */
#define TCC_FCTRLA_CAPTURE(value)             (TCC_FCTRLA_CAPTURE_Msk & ((value) << TCC_FCTRLA_CAPTURE_Pos))
#define   TCC_FCTRLA_CAPTURE_DISABLE_Val      _U_(0x0)                                             /**< (TCC_FCTRLA) No capture  */
#define   TCC_FCTRLA_CAPTURE_CAPT_Val         _U_(0x1)                                             /**< (TCC_FCTRLA) Capture on fault  */
#define   TCC_FCTRLA_CAPTURE_CAPTMIN_Val      _U_(0x2)                                             /**< (TCC_FCTRLA) Minimum capture  */
#define   TCC_FCTRLA_CAPTURE_CAPTMAX_Val      _U_(0x3)                                             /**< (TCC_FCTRLA) Maximum capture  */
#define   TCC_FCTRLA_CAPTURE_LOCMIN_Val       _U_(0x4)                                             /**< (TCC_FCTRLA) Minimum local detection  */
#define   TCC_FCTRLA_CAPTURE_LOCMAX_Val       _U_(0x5)                                             /**< (TCC_FCTRLA) Maximum local detection  */
#define   TCC_FCTRLA_CAPTURE_DERIV0_Val       _U_(0x6)                                             /**< (TCC_FCTRLA) Minimum and maximum local detection  */
#define   TCC_FCTRLA_CAPTURE_CAPTMARK_Val     _U_(0x7)                                             /**< (TCC_FCTRLA) Capture with ramp index as MSB value  */
#define TCC_FCTRLA_CAPTURE_DISABLE            (TCC_FCTRLA_CAPTURE_DISABLE_Val << TCC_FCTRLA_CAPTURE_Pos) /**< (TCC_FCTRLA) No capture Position  */
#define TCC_FCTRLA_CAPTURE_CAPT               (TCC_FCTRLA_CAPTURE_CAPT_Val << TCC_FCTRLA_CAPTURE_Pos) /**< (TCC_FCTRLA) Capture on fault Position  */
#define TCC_FCTRLA_CAPTURE_CAPTMIN            (TCC_FCTRLA_CAPTURE_CAPTMIN_Val << TCC_FCTRLA_CAPTURE_Pos) /**< (TCC_FCTRLA) Minimum capture Position  */
#define TCC_FCTRLA_CAPTURE_CAPTMAX            (TCC_FCTRLA_CAPTURE_CAPTMAX_Val << TCC_FCTRLA_CAPTURE_Pos) /**< (TCC_FCTRLA) Maximum capture Position  */
#define TCC_FCTRLA_CAPTURE_LOCMIN             (TCC_FCTRLA_CAPTURE_LOCMIN_Val << TCC_FCTRLA_CAPTURE_Pos) /**< (TCC_FCTRLA) Minimum local detection Position  */
#define TCC_FCTRLA_CAPTURE_LOCMAX             (TCC_FCTRLA_CAPTURE_LOCMAX_Val << TCC_FCTRLA_CAPTURE_Pos) /**< (TCC_FCTRLA) Maximum local detection Position  */
#define TCC_FCTRLA_CAPTURE_DERIV0             (TCC_FCTRLA_CAPTURE_DERIV0_Val << TCC_FCTRLA_CAPTURE_Pos) /**< (TCC_FCTRLA) Minimum and maximum local detection Position  */
#define TCC_FCTRLA_CAPTURE_CAPTMARK           (TCC_FCTRLA_CAPTURE_CAPTMARK_Val << TCC_FCTRLA_CAPTURE_Pos) /**< (TCC_FCTRLA) Capture with ramp index as MSB value Position  */
#define TCC_FCTRLA_BLANKPRESC_Pos             _U_(15)                                              /**< (TCC_FCTRLA) Fault A Blanking Prescaler Position */
#define TCC_FCTRLA_BLANKPRESC_Msk             (_U_(0x1) << TCC_FCTRLA_BLANKPRESC_Pos)              /**< (TCC_FCTRLA) Fault A Blanking Prescaler Mask */
#define TCC_FCTRLA_BLANKPRESC(value)          (TCC_FCTRLA_BLANKPRESC_Msk & ((value) << TCC_FCTRLA_BLANKPRESC_Pos))
#define TCC_FCTRLA_BLANKVAL_Pos               _U_(16)                                              /**< (TCC_FCTRLA) Fault A Blanking Time Position */
#define TCC_FCTRLA_BLANKVAL_Msk               (_U_(0xFF) << TCC_FCTRLA_BLANKVAL_Pos)               /**< (TCC_FCTRLA) Fault A Blanking Time Mask */
#define TCC_FCTRLA_BLANKVAL(value)            (TCC_FCTRLA_BLANKVAL_Msk & ((value) << TCC_FCTRLA_BLANKVAL_Pos))
#define TCC_FCTRLA_FILTERVAL_Pos              _U_(24)                                              /**< (TCC_FCTRLA) Fault A Filter Value Position */
#define TCC_FCTRLA_FILTERVAL_Msk              (_U_(0xF) << TCC_FCTRLA_FILTERVAL_Pos)               /**< (TCC_FCTRLA) Fault A Filter Value Mask */
#define TCC_FCTRLA_FILTERVAL(value)           (TCC_FCTRLA_FILTERVAL_Msk & ((value) << TCC_FCTRLA_FILTERVAL_Pos))
#define TCC_FCTRLA_Msk                        _U_(0x0FFFFFFB)                                      /**< (TCC_FCTRLA) Register Mask  */


/* -------- TCC_FCTRLB : (TCC Offset: 0x10) (R/W 32) Recoverable Fault B Configuration -------- */
#define TCC_FCTRLB_RESETVALUE                 _U_(0x00)                                            /**<  (TCC_FCTRLB) Recoverable Fault B Configuration  Reset Value */

#define TCC_FCTRLB_SRC_Pos                    _U_(0)                                               /**< (TCC_FCTRLB) Fault B Source Position */
#define TCC_FCTRLB_SRC_Msk                    (_U_(0x3) << TCC_FCTRLB_SRC_Pos)                     /**< (TCC_FCTRLB) Fault B Source Mask */
#define TCC_FCTRLB_SRC(value)                 (TCC_FCTRLB_SRC_Msk & ((value) << TCC_FCTRLB_SRC_Pos))
#define   TCC_FCTRLB_SRC_DISABLE_Val          _U_(0x0)                                             /**< (TCC_FCTRLB) Fault input disabled  */
#define   TCC_FCTRLB_SRC_ENABLE_Val           _U_(0x1)                                             /**< (TCC_FCTRLB) MCEx (x=0,1) event input  */
#define   TCC_FCTRLB_SRC_INVERT_Val           _U_(0x2)                                             /**< (TCC_FCTRLB) Inverted MCEx (x=0,1) event input  */
#define   TCC_FCTRLB_SRC_ALTFAULT_Val         _U_(0x3)                                             /**< (TCC_FCTRLB) Alternate fault (A or B) state at the end of the previous period  */
#define TCC_FCTRLB_SRC_DISABLE                (TCC_FCTRLB_SRC_DISABLE_Val << TCC_FCTRLB_SRC_Pos)   /**< (TCC_FCTRLB) Fault input disabled Position  */
#define TCC_FCTRLB_SRC_ENABLE                 (TCC_FCTRLB_SRC_ENABLE_Val << TCC_FCTRLB_SRC_Pos)    /**< (TCC_FCTRLB) MCEx (x=0,1) event input Position  */
#define TCC_FCTRLB_SRC_INVERT                 (TCC_FCTRLB_SRC_INVERT_Val << TCC_FCTRLB_SRC_Pos)    /**< (TCC_FCTRLB) Inverted MCEx (x=0,1) event input Position  */
#define TCC_FCTRLB_SRC_ALTFAULT               (TCC_FCTRLB_SRC_ALTFAULT_Val << TCC_FCTRLB_SRC_Pos)  /**< (TCC_FCTRLB) Alternate fault (A or B) state at the end of the previous period Position  */
#define TCC_FCTRLB_KEEP_Pos                   _U_(3)                                               /**< (TCC_FCTRLB) Fault B Keeper Position */
#define TCC_FCTRLB_KEEP_Msk                   (_U_(0x1) << TCC_FCTRLB_KEEP_Pos)                    /**< (TCC_FCTRLB) Fault B Keeper Mask */
#define TCC_FCTRLB_KEEP(value)                (TCC_FCTRLB_KEEP_Msk & ((value) << TCC_FCTRLB_KEEP_Pos))
#define TCC_FCTRLB_QUAL_Pos                   _U_(4)                                               /**< (TCC_FCTRLB) Fault B Qualification Position */
#define TCC_FCTRLB_QUAL_Msk                   (_U_(0x1) << TCC_FCTRLB_QUAL_Pos)                    /**< (TCC_FCTRLB) Fault B Qualification Mask */
#define TCC_FCTRLB_QUAL(value)                (TCC_FCTRLB_QUAL_Msk & ((value) << TCC_FCTRLB_QUAL_Pos))
#define TCC_FCTRLB_BLANK_Pos                  _U_(5)                                               /**< (TCC_FCTRLB) Fault B Blanking Mode Position */
#define TCC_FCTRLB_BLANK_Msk                  (_U_(0x3) << TCC_FCTRLB_BLANK_Pos)                   /**< (TCC_FCTRLB) Fault B Blanking Mode Mask */
#define TCC_FCTRLB_BLANK(value)               (TCC_FCTRLB_BLANK_Msk & ((value) << TCC_FCTRLB_BLANK_Pos))
#define   TCC_FCTRLB_BLANK_START_Val          _U_(0x0)                                             /**< (TCC_FCTRLB) Blanking applied from start of the ramp  */
#define   TCC_FCTRLB_BLANK_RISE_Val           _U_(0x1)                                             /**< (TCC_FCTRLB) Blanking applied from rising edge of the output waveform  */
#define   TCC_FCTRLB_BLANK_FALL_Val           _U_(0x2)                                             /**< (TCC_FCTRLB) Blanking applied from falling edge of the output waveform  */
#define   TCC_FCTRLB_BLANK_BOTH_Val           _U_(0x3)                                             /**< (TCC_FCTRLB) Blanking applied from each toggle of the output waveform  */
#define TCC_FCTRLB_BLANK_START                (TCC_FCTRLB_BLANK_START_Val << TCC_FCTRLB_BLANK_Pos) /**< (TCC_FCTRLB) Blanking applied from start of the ramp Position  */
#define TCC_FCTRLB_BLANK_RISE                 (TCC_FCTRLB_BLANK_RISE_Val << TCC_FCTRLB_BLANK_Pos)  /**< (TCC_FCTRLB) Blanking applied from rising edge of the output waveform Position  */
#define TCC_FCTRLB_BLANK_FALL                 (TCC_FCTRLB_BLANK_FALL_Val << TCC_FCTRLB_BLANK_Pos)  /**< (TCC_FCTRLB) Blanking applied from falling edge of the output waveform Position  */
#define TCC_FCTRLB_BLANK_BOTH                 (TCC_FCTRLB_BLANK_BOTH_Val << TCC_FCTRLB_BLANK_Pos)  /**< (TCC_FCTRLB) Blanking applied from each toggle of the output waveform Position  */
#define TCC_FCTRLB_RESTART_Pos                _U_(7)                                               /**< (TCC_FCTRLB) Fault B Restart Position */
#define TCC_FCTRLB_RESTART_Msk                (_U_(0x1) << TCC_FCTRLB_RESTART_Pos)                 /**< (TCC_FCTRLB) Fault B Restart Mask */
#define TCC_FCTRLB_RESTART(value)             (TCC_FCTRLB_RESTART_Msk & ((value) << TCC_FCTRLB_RESTART_Pos))
#define TCC_FCTRLB_HALT_Pos                   _U_(8)                                               /**< (TCC_FCTRLB) Fault B Halt Mode Position */
#define TCC_FCTRLB_HALT_Msk                   (_U_(0x3) << TCC_FCTRLB_HALT_Pos)                    /**< (TCC_FCTRLB) Fault B Halt Mode Mask */
#define TCC_FCTRLB_HALT(value)                (TCC_FCTRLB_HALT_Msk & ((value) << TCC_FCTRLB_HALT_Pos))
#define   TCC_FCTRLB_HALT_DISABLE_Val         _U_(0x0)                                             /**< (TCC_FCTRLB) Halt action disabled  */
#define   TCC_FCTRLB_HALT_HW_Val              _U_(0x1)                                             /**< (TCC_FCTRLB) Hardware halt action  */
#define   TCC_FCTRLB_HALT_SW_Val              _U_(0x2)                                             /**< (TCC_FCTRLB) Software halt action  */
#define   TCC_FCTRLB_HALT_NR_Val              _U_(0x3)                                             /**< (TCC_FCTRLB) Non-recoverable fault  */
#define TCC_FCTRLB_HALT_DISABLE               (TCC_FCTRLB_HALT_DISABLE_Val << TCC_FCTRLB_HALT_Pos) /**< (TCC_FCTRLB) Halt action disabled Position  */
#define TCC_FCTRLB_HALT_HW                    (TCC_FCTRLB_HALT_HW_Val << TCC_FCTRLB_HALT_Pos)      /**< (TCC_FCTRLB) Hardware halt action Position  */
#define TCC_FCTRLB_HALT_SW                    (TCC_FCTRLB_HALT_SW_Val << TCC_FCTRLB_HALT_Pos)      /**< (TCC_FCTRLB) Software halt action Position  */
#define TCC_FCTRLB_HALT_NR                    (TCC_FCTRLB_HALT_NR_Val << TCC_FCTRLB_HALT_Pos)      /**< (TCC_FCTRLB) Non-recoverable fault Position  */
#define TCC_FCTRLB_CHSEL_Pos                  _U_(10)                                              /**< (TCC_FCTRLB) Fault B Capture Channel Position */
#define TCC_FCTRLB_CHSEL_Msk                  (_U_(0x3) << TCC_FCTRLB_CHSEL_Pos)                   /**< (TCC_FCTRLB) Fault B Capture Channel Mask */
#define TCC_FCTRLB_CHSEL(value)               (TCC_FCTRLB_CHSEL_Msk & ((value) << TCC_FCTRLB_CHSEL_Pos))
#define   TCC_FCTRLB_CHSEL_CC0_Val            _U_(0x0)                                             /**< (TCC_FCTRLB) Capture value stored in channel 0  */
#define   TCC_FCTRLB_CHSEL_CC1_Val            _U_(0x1)                                             /**< (TCC_FCTRLB) Capture value stored in channel 1  */
#define   TCC_FCTRLB_CHSEL_CC2_Val            _U_(0x2)                                             /**< (TCC_FCTRLB) Capture value stored in channel 2  */
#define   TCC_FCTRLB_CHSEL_CC3_Val            _U_(0x3)                                             /**< (TCC_FCTRLB) Capture value stored in channel 3  */
#define TCC_FCTRLB_CHSEL_CC0                  (TCC_FCTRLB_CHSEL_CC0_Val << TCC_FCTRLB_CHSEL_Pos)   /**< (TCC_FCTRLB) Capture value stored in channel 0 Position  */
#define TCC_FCTRLB_CHSEL_CC1                  (TCC_FCTRLB_CHSEL_CC1_Val << TCC_FCTRLB_CHSEL_Pos)   /**< (TCC_FCTRLB) Capture value stored in channel 1 Position  */
#define TCC_FCTRLB_CHSEL_CC2                  (TCC_FCTRLB_CHSEL_CC2_Val << TCC_FCTRLB_CHSEL_Pos)   /**< (TCC_FCTRLB) Capture value stored in channel 2 Position  */
#define TCC_FCTRLB_CHSEL_CC3                  (TCC_FCTRLB_CHSEL_CC3_Val << TCC_FCTRLB_CHSEL_Pos)   /**< (TCC_FCTRLB) Capture value stored in channel 3 Position  */
#define TCC_FCTRLB_CAPTURE_Pos                _U_(12)                                              /**< (TCC_FCTRLB) Fault B Capture Action Position */
#define TCC_FCTRLB_CAPTURE_Msk                (_U_(0x7) << TCC_FCTRLB_CAPTURE_Pos)                 /**< (TCC_FCTRLB) Fault B Capture Action Mask */
#define TCC_FCTRLB_CAPTURE(value)             (TCC_FCTRLB_CAPTURE_Msk & ((value) << TCC_FCTRLB_CAPTURE_Pos))
#define   TCC_FCTRLB_CAPTURE_DISABLE_Val      _U_(0x0)                                             /**< (TCC_FCTRLB) No capture  */
#define   TCC_FCTRLB_CAPTURE_CAPT_Val         _U_(0x1)                                             /**< (TCC_FCTRLB) Capture on fault  */
#define   TCC_FCTRLB_CAPTURE_CAPTMIN_Val      _U_(0x2)                                             /**< (TCC_FCTRLB) Minimum capture  */
#define   TCC_FCTRLB_CAPTURE_CAPTMAX_Val      _U_(0x3)                                             /**< (TCC_FCTRLB) Maximum capture  */
#define   TCC_FCTRLB_CAPTURE_LOCMIN_Val       _U_(0x4)                                             /**< (TCC_FCTRLB) Minimum local detection  */
#define   TCC_FCTRLB_CAPTURE_LOCMAX_Val       _U_(0x5)                                             /**< (TCC_FCTRLB) Maximum local detection  */
#define   TCC_FCTRLB_CAPTURE_DERIV0_Val       _U_(0x6)                                             /**< (TCC_FCTRLB) Minimum and maximum local detection  */
#define   TCC_FCTRLB_CAPTURE_CAPTMARK_Val     _U_(0x7)                                             /**< (TCC_FCTRLB) Capture with ramp index as MSB value  */
#define TCC_FCTRLB_CAPTURE_DISABLE            (TCC_FCTRLB_CAPTURE_DISABLE_Val << TCC_FCTRLB_CAPTURE_Pos) /**< (TCC_FCTRLB) No capture Position  */
#define TCC_FCTRLB_CAPTURE_CAPT               (TCC_FCTRLB_CAPTURE_CAPT_Val << TCC_FCTRLB_CAPTURE_Pos) /**< (TCC_FCTRLB) Capture on fault Position  */
#define TCC_FCTRLB_CAPTURE_CAPTMIN            (TCC_FCTRLB_CAPTURE_CAPTMIN_Val << TCC_FCTRLB_CAPTURE_Pos) /**< (TCC_FCTRLB) Minimum capture Position  */
#define TCC_FCTRLB_CAPTURE_CAPTMAX            (TCC_FCTRLB_CAPTURE_CAPTMAX_Val << TCC_FCTRLB_CAPTURE_Pos) /**< (TCC_FCTRLB) Maximum capture Position  */
#define TCC_FCTRLB_CAPTURE_LOCMIN             (TCC_FCTRLB_CAPTURE_LOCMIN_Val << TCC_FCTRLB_CAPTURE_Pos) /**< (TCC_FCTRLB) Minimum local detection Position  */
#define TCC_FCTRLB_CAPTURE_LOCMAX             (TCC_FCTRLB_CAPTURE_LOCMAX_Val << TCC_FCTRLB_CAPTURE_Pos) /**< (TCC_FCTRLB) Maximum local detection Position  */
#define TCC_FCTRLB_CAPTURE_DERIV0             (TCC_FCTRLB_CAPTURE_DERIV0_Val << TCC_FCTRLB_CAPTURE_Pos) /**< (TCC_FCTRLB) Minimum and maximum local detection Position  */
#define TCC_FCTRLB_CAPTURE_CAPTMARK           (TCC_FCTRLB_CAPTURE_CAPTMARK_Val << TCC_FCTRLB_CAPTURE_Pos) /**< (TCC_FCTRLB) Capture with ramp index as MSB value Position  */
#define TCC_FCTRLB_BLANKPRESC_Pos             _U_(15)                                              /**< (TCC_FCTRLB) Fault B Blanking Prescaler Position */
#define TCC_FCTRLB_BLANKPRESC_Msk             (_U_(0x1) << TCC_FCTRLB_BLANKPRESC_Pos)              /**< (TCC_FCTRLB) Fault B Blanking Prescaler Mask */
#define TCC_FCTRLB_BLANKPRESC(value)          (TCC_FCTRLB_BLANKPRESC_Msk & ((value) << TCC_FCTRLB_BLANKPRESC_Pos))
#define TCC_FCTRLB_BLANKVAL_Pos               _U_(16)                                              /**< (TCC_FCTRLB) Fault B Blanking Time Position */
#define TCC_FCTRLB_BLANKVAL_Msk               (_U_(0xFF) << TCC_FCTRLB_BLANKVAL_Pos)               /**< (TCC_FCTRLB) Fault B Blanking Time Mask */
#define TCC_FCTRLB_BLANKVAL(value)            (TCC_FCTRLB_BLANKVAL_Msk & ((value) << TCC_FCTRLB_BLANKVAL_Pos))
#define TCC_FCTRLB_FILTERVAL_Pos              _U_(24)                                              /**< (TCC_FCTRLB) Fault B Filter Value Position */
#define TCC_FCTRLB_FILTERVAL_Msk              (_U_(0xF) << TCC_FCTRLB_FILTERVAL_Pos)               /**< (TCC_FCTRLB) Fault B Filter Value Mask */
#define TCC_FCTRLB_FILTERVAL(value)           (TCC_FCTRLB_FILTERVAL_Msk & ((value) << TCC_FCTRLB_FILTERVAL_Pos))
#define TCC_FCTRLB_Msk                        _U_(0x0FFFFFFB)                                      /**< (TCC_FCTRLB) Register Mask  */


/* -------- TCC_WEXCTRL : (TCC Offset: 0x14) (R/W 32) Waveform Extension Configuration -------- */
#define TCC_WEXCTRL_RESETVALUE                _U_(0x00)                                            /**<  (TCC_WEXCTRL) Waveform Extension Configuration  Reset Value */

#define TCC_WEXCTRL_OTMX_Pos                  _U_(0)                                               /**< (TCC_WEXCTRL) Output Matrix Position */
#define TCC_WEXCTRL_OTMX_Msk                  (_U_(0x3) << TCC_WEXCTRL_OTMX_Pos)                   /**< (TCC_WEXCTRL) Output Matrix Mask */
#define TCC_WEXCTRL_OTMX(value)               (TCC_WEXCTRL_OTMX_Msk & ((value) << TCC_WEXCTRL_OTMX_Pos))
#define TCC_WEXCTRL_DTIEN0_Pos                _U_(8)                                               /**< (TCC_WEXCTRL) Dead-time Insertion Generator 0 Enable Position */
#define TCC_WEXCTRL_DTIEN0_Msk                (_U_(0x1) << TCC_WEXCTRL_DTIEN0_Pos)                 /**< (TCC_WEXCTRL) Dead-time Insertion Generator 0 Enable Mask */
#define TCC_WEXCTRL_DTIEN0(value)             (TCC_WEXCTRL_DTIEN0_Msk & ((value) << TCC_WEXCTRL_DTIEN0_Pos))
#define TCC_WEXCTRL_DTIEN1_Pos                _U_(9)                                               /**< (TCC_WEXCTRL) Dead-time Insertion Generator 1 Enable Position */
#define TCC_WEXCTRL_DTIEN1_Msk                (_U_(0x1) << TCC_WEXCTRL_DTIEN1_Pos)                 /**< (TCC_WEXCTRL) Dead-time Insertion Generator 1 Enable Mask */
#define TCC_WEXCTRL_DTIEN1(value)             (TCC_WEXCTRL_DTIEN1_Msk & ((value) << TCC_WEXCTRL_DTIEN1_Pos))
#define TCC_WEXCTRL_DTIEN2_Pos                _U_(10)                                              /**< (TCC_WEXCTRL) Dead-time Insertion Generator 2 Enable Position */
#define TCC_WEXCTRL_DTIEN2_Msk                (_U_(0x1) << TCC_WEXCTRL_DTIEN2_Pos)                 /**< (TCC_WEXCTRL) Dead-time Insertion Generator 2 Enable Mask */
#define TCC_WEXCTRL_DTIEN2(value)             (TCC_WEXCTRL_DTIEN2_Msk & ((value) << TCC_WEXCTRL_DTIEN2_Pos))
#define TCC_WEXCTRL_DTIEN3_Pos                _U_(11)                                              /**< (TCC_WEXCTRL) Dead-time Insertion Generator 3 Enable Position */
#define TCC_WEXCTRL_DTIEN3_Msk                (_U_(0x1) << TCC_WEXCTRL_DTIEN3_Pos)                 /**< (TCC_WEXCTRL) Dead-time Insertion Generator 3 Enable Mask */
#define TCC_WEXCTRL_DTIEN3(value)             (TCC_WEXCTRL_DTIEN3_Msk & ((value) << TCC_WEXCTRL_DTIEN3_Pos))
#define TCC_WEXCTRL_DTLS_Pos                  _U_(16)                                              /**< (TCC_WEXCTRL) Dead-time Low Side Outputs Value Position */
#define TCC_WEXCTRL_DTLS_Msk                  (_U_(0xFF) << TCC_WEXCTRL_DTLS_Pos)                  /**< (TCC_WEXCTRL) Dead-time Low Side Outputs Value Mask */
#define TCC_WEXCTRL_DTLS(value)               (TCC_WEXCTRL_DTLS_Msk & ((value) << TCC_WEXCTRL_DTLS_Pos))
#define TCC_WEXCTRL_DTHS_Pos                  _U_(24)                                              /**< (TCC_WEXCTRL) Dead-time High Side Outputs Value Position */
#define TCC_WEXCTRL_DTHS_Msk                  (_U_(0xFF) << TCC_WEXCTRL_DTHS_Pos)                  /**< (TCC_WEXCTRL) Dead-time High Side Outputs Value Mask */
#define TCC_WEXCTRL_DTHS(value)               (TCC_WEXCTRL_DTHS_Msk & ((value) << TCC_WEXCTRL_DTHS_Pos))
#define TCC_WEXCTRL_Msk                       _U_(0xFFFF0F03)                                      /**< (TCC_WEXCTRL) Register Mask  */

#define TCC_WEXCTRL_DTIEN_Pos                 _U_(8)                                               /**< (TCC_WEXCTRL Position) Dead-time Insertion Generator x Enable */
#define TCC_WEXCTRL_DTIEN_Msk                 (_U_(0xF) << TCC_WEXCTRL_DTIEN_Pos)                  /**< (TCC_WEXCTRL Mask) DTIEN */
#define TCC_WEXCTRL_DTIEN(value)              (TCC_WEXCTRL_DTIEN_Msk & ((value) << TCC_WEXCTRL_DTIEN_Pos)) 

/* -------- TCC_DRVCTRL : (TCC Offset: 0x18) (R/W 32) Driver Control -------- */
#define TCC_DRVCTRL_RESETVALUE                _U_(0x00)                                            /**<  (TCC_DRVCTRL) Driver Control  Reset Value */

#define TCC_DRVCTRL_NRE0_Pos                  _U_(0)                                               /**< (TCC_DRVCTRL) Non-Recoverable State 0 Output Enable Position */
#define TCC_DRVCTRL_NRE0_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRE0_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 0 Output Enable Mask */
#define TCC_DRVCTRL_NRE0(value)               (TCC_DRVCTRL_NRE0_Msk & ((value) << TCC_DRVCTRL_NRE0_Pos))
#define TCC_DRVCTRL_NRE1_Pos                  _U_(1)                                               /**< (TCC_DRVCTRL) Non-Recoverable State 1 Output Enable Position */
#define TCC_DRVCTRL_NRE1_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRE1_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 1 Output Enable Mask */
#define TCC_DRVCTRL_NRE1(value)               (TCC_DRVCTRL_NRE1_Msk & ((value) << TCC_DRVCTRL_NRE1_Pos))
#define TCC_DRVCTRL_NRE2_Pos                  _U_(2)                                               /**< (TCC_DRVCTRL) Non-Recoverable State 2 Output Enable Position */
#define TCC_DRVCTRL_NRE2_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRE2_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 2 Output Enable Mask */
#define TCC_DRVCTRL_NRE2(value)               (TCC_DRVCTRL_NRE2_Msk & ((value) << TCC_DRVCTRL_NRE2_Pos))
#define TCC_DRVCTRL_NRE3_Pos                  _U_(3)                                               /**< (TCC_DRVCTRL) Non-Recoverable State 3 Output Enable Position */
#define TCC_DRVCTRL_NRE3_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRE3_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 3 Output Enable Mask */
#define TCC_DRVCTRL_NRE3(value)               (TCC_DRVCTRL_NRE3_Msk & ((value) << TCC_DRVCTRL_NRE3_Pos))
#define TCC_DRVCTRL_NRE4_Pos                  _U_(4)                                               /**< (TCC_DRVCTRL) Non-Recoverable State 4 Output Enable Position */
#define TCC_DRVCTRL_NRE4_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRE4_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 4 Output Enable Mask */
#define TCC_DRVCTRL_NRE4(value)               (TCC_DRVCTRL_NRE4_Msk & ((value) << TCC_DRVCTRL_NRE4_Pos))
#define TCC_DRVCTRL_NRE5_Pos                  _U_(5)                                               /**< (TCC_DRVCTRL) Non-Recoverable State 5 Output Enable Position */
#define TCC_DRVCTRL_NRE5_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRE5_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 5 Output Enable Mask */
#define TCC_DRVCTRL_NRE5(value)               (TCC_DRVCTRL_NRE5_Msk & ((value) << TCC_DRVCTRL_NRE5_Pos))
#define TCC_DRVCTRL_NRE6_Pos                  _U_(6)                                               /**< (TCC_DRVCTRL) Non-Recoverable State 6 Output Enable Position */
#define TCC_DRVCTRL_NRE6_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRE6_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 6 Output Enable Mask */
#define TCC_DRVCTRL_NRE6(value)               (TCC_DRVCTRL_NRE6_Msk & ((value) << TCC_DRVCTRL_NRE6_Pos))
#define TCC_DRVCTRL_NRE7_Pos                  _U_(7)                                               /**< (TCC_DRVCTRL) Non-Recoverable State 7 Output Enable Position */
#define TCC_DRVCTRL_NRE7_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRE7_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 7 Output Enable Mask */
#define TCC_DRVCTRL_NRE7(value)               (TCC_DRVCTRL_NRE7_Msk & ((value) << TCC_DRVCTRL_NRE7_Pos))
#define TCC_DRVCTRL_NRV0_Pos                  _U_(8)                                               /**< (TCC_DRVCTRL) Non-Recoverable State 0 Output Value Position */
#define TCC_DRVCTRL_NRV0_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRV0_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 0 Output Value Mask */
#define TCC_DRVCTRL_NRV0(value)               (TCC_DRVCTRL_NRV0_Msk & ((value) << TCC_DRVCTRL_NRV0_Pos))
#define TCC_DRVCTRL_NRV1_Pos                  _U_(9)                                               /**< (TCC_DRVCTRL) Non-Recoverable State 1 Output Value Position */
#define TCC_DRVCTRL_NRV1_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRV1_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 1 Output Value Mask */
#define TCC_DRVCTRL_NRV1(value)               (TCC_DRVCTRL_NRV1_Msk & ((value) << TCC_DRVCTRL_NRV1_Pos))
#define TCC_DRVCTRL_NRV2_Pos                  _U_(10)                                              /**< (TCC_DRVCTRL) Non-Recoverable State 2 Output Value Position */
#define TCC_DRVCTRL_NRV2_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRV2_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 2 Output Value Mask */
#define TCC_DRVCTRL_NRV2(value)               (TCC_DRVCTRL_NRV2_Msk & ((value) << TCC_DRVCTRL_NRV2_Pos))
#define TCC_DRVCTRL_NRV3_Pos                  _U_(11)                                              /**< (TCC_DRVCTRL) Non-Recoverable State 3 Output Value Position */
#define TCC_DRVCTRL_NRV3_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRV3_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 3 Output Value Mask */
#define TCC_DRVCTRL_NRV3(value)               (TCC_DRVCTRL_NRV3_Msk & ((value) << TCC_DRVCTRL_NRV3_Pos))
#define TCC_DRVCTRL_NRV4_Pos                  _U_(12)                                              /**< (TCC_DRVCTRL) Non-Recoverable State 4 Output Value Position */
#define TCC_DRVCTRL_NRV4_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRV4_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 4 Output Value Mask */
#define TCC_DRVCTRL_NRV4(value)               (TCC_DRVCTRL_NRV4_Msk & ((value) << TCC_DRVCTRL_NRV4_Pos))
#define TCC_DRVCTRL_NRV5_Pos                  _U_(13)                                              /**< (TCC_DRVCTRL) Non-Recoverable State 5 Output Value Position */
#define TCC_DRVCTRL_NRV5_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRV5_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 5 Output Value Mask */
#define TCC_DRVCTRL_NRV5(value)               (TCC_DRVCTRL_NRV5_Msk & ((value) << TCC_DRVCTRL_NRV5_Pos))
#define TCC_DRVCTRL_NRV6_Pos                  _U_(14)                                              /**< (TCC_DRVCTRL) Non-Recoverable State 6 Output Value Position */
#define TCC_DRVCTRL_NRV6_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRV6_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 6 Output Value Mask */
#define TCC_DRVCTRL_NRV6(value)               (TCC_DRVCTRL_NRV6_Msk & ((value) << TCC_DRVCTRL_NRV6_Pos))
#define TCC_DRVCTRL_NRV7_Pos                  _U_(15)                                              /**< (TCC_DRVCTRL) Non-Recoverable State 7 Output Value Position */
#define TCC_DRVCTRL_NRV7_Msk                  (_U_(0x1) << TCC_DRVCTRL_NRV7_Pos)                   /**< (TCC_DRVCTRL) Non-Recoverable State 7 Output Value Mask */
#define TCC_DRVCTRL_NRV7(value)               (TCC_DRVCTRL_NRV7_Msk & ((value) << TCC_DRVCTRL_NRV7_Pos))
#define TCC_DRVCTRL_INVEN0_Pos                _U_(16)                                              /**< (TCC_DRVCTRL) Output Waveform 0 Inversion Position */
#define TCC_DRVCTRL_INVEN0_Msk                (_U_(0x1) << TCC_DRVCTRL_INVEN0_Pos)                 /**< (TCC_DRVCTRL) Output Waveform 0 Inversion Mask */
#define TCC_DRVCTRL_INVEN0(value)             (TCC_DRVCTRL_INVEN0_Msk & ((value) << TCC_DRVCTRL_INVEN0_Pos))
#define TCC_DRVCTRL_INVEN1_Pos                _U_(17)                                              /**< (TCC_DRVCTRL) Output Waveform 1 Inversion Position */
#define TCC_DRVCTRL_INVEN1_Msk                (_U_(0x1) << TCC_DRVCTRL_INVEN1_Pos)                 /**< (TCC_DRVCTRL) Output Waveform 1 Inversion Mask */
#define TCC_DRVCTRL_INVEN1(value)             (TCC_DRVCTRL_INVEN1_Msk & ((value) << TCC_DRVCTRL_INVEN1_Pos))
#define TCC_DRVCTRL_INVEN2_Pos                _U_(18)                                              /**< (TCC_DRVCTRL) Output Waveform 2 Inversion Position */
#define TCC_DRVCTRL_INVEN2_Msk                (_U_(0x1) << TCC_DRVCTRL_INVEN2_Pos)                 /**< (TCC_DRVCTRL) Output Waveform 2 Inversion Mask */
#define TCC_DRVCTRL_INVEN2(value)             (TCC_DRVCTRL_INVEN2_Msk & ((value) << TCC_DRVCTRL_INVEN2_Pos))
#define TCC_DRVCTRL_INVEN3_Pos                _U_(19)                                              /**< (TCC_DRVCTRL) Output Waveform 3 Inversion Position */
#define TCC_DRVCTRL_INVEN3_Msk                (_U_(0x1) << TCC_DRVCTRL_INVEN3_Pos)                 /**< (TCC_DRVCTRL) Output Waveform 3 Inversion Mask */
#define TCC_DRVCTRL_INVEN3(value)             (TCC_DRVCTRL_INVEN3_Msk & ((value) << TCC_DRVCTRL_INVEN3_Pos))
#define TCC_DRVCTRL_INVEN4_Pos                _U_(20)                                              /**< (TCC_DRVCTRL) Output Waveform 4 Inversion Position */
#define TCC_DRVCTRL_INVEN4_Msk                (_U_(0x1) << TCC_DRVCTRL_INVEN4_Pos)                 /**< (TCC_DRVCTRL) Output Waveform 4 Inversion Mask */
#define TCC_DRVCTRL_INVEN4(value)             (TCC_DRVCTRL_INVEN4_Msk & ((value) << TCC_DRVCTRL_INVEN4_Pos))
#define TCC_DRVCTRL_INVEN5_Pos                _U_(21)                                              /**< (TCC_DRVCTRL) Output Waveform 5 Inversion Position */
#define TCC_DRVCTRL_INVEN5_Msk                (_U_(0x1) << TCC_DRVCTRL_INVEN5_Pos)                 /**< (TCC_DRVCTRL) Output Waveform 5 Inversion Mask */
#define TCC_DRVCTRL_INVEN5(value)             (TCC_DRVCTRL_INVEN5_Msk & ((value) << TCC_DRVCTRL_INVEN5_Pos))
#define TCC_DRVCTRL_INVEN6_Pos                _U_(22)                                              /**< (TCC_DRVCTRL) Output Waveform 6 Inversion Position */
#define TCC_DRVCTRL_INVEN6_Msk                (_U_(0x1) << TCC_DRVCTRL_INVEN6_Pos)                 /**< (TCC_DRVCTRL) Output Waveform 6 Inversion Mask */
#define TCC_DRVCTRL_INVEN6(value)             (TCC_DRVCTRL_INVEN6_Msk & ((value) << TCC_DRVCTRL_INVEN6_Pos))
#define TCC_DRVCTRL_INVEN7_Pos                _U_(23)                                              /**< (TCC_DRVCTRL) Output Waveform 7 Inversion Position */
#define TCC_DRVCTRL_INVEN7_Msk                (_U_(0x1) << TCC_DRVCTRL_INVEN7_Pos)                 /**< (TCC_DRVCTRL) Output Waveform 7 Inversion Mask */
#define TCC_DRVCTRL_INVEN7(value)             (TCC_DRVCTRL_INVEN7_Msk & ((value) << TCC_DRVCTRL_INVEN7_Pos))
#define TCC_DRVCTRL_FILTERVAL0_Pos            _U_(24)                                              /**< (TCC_DRVCTRL) Non-Recoverable Fault Input 0 Filter Value Position */
#define TCC_DRVCTRL_FILTERVAL0_Msk            (_U_(0xF) << TCC_DRVCTRL_FILTERVAL0_Pos)             /**< (TCC_DRVCTRL) Non-Recoverable Fault Input 0 Filter Value Mask */
#define TCC_DRVCTRL_FILTERVAL0(value)         (TCC_DRVCTRL_FILTERVAL0_Msk & ((value) << TCC_DRVCTRL_FILTERVAL0_Pos))
#define TCC_DRVCTRL_FILTERVAL1_Pos            _U_(28)                                              /**< (TCC_DRVCTRL) Non-Recoverable Fault Input 1 Filter Value Position */
#define TCC_DRVCTRL_FILTERVAL1_Msk            (_U_(0xF) << TCC_DRVCTRL_FILTERVAL1_Pos)             /**< (TCC_DRVCTRL) Non-Recoverable Fault Input 1 Filter Value Mask */
#define TCC_DRVCTRL_FILTERVAL1(value)         (TCC_DRVCTRL_FILTERVAL1_Msk & ((value) << TCC_DRVCTRL_FILTERVAL1_Pos))
#define TCC_DRVCTRL_Msk                       _U_(0xFFFFFFFF)                                      /**< (TCC_DRVCTRL) Register Mask  */

#define TCC_DRVCTRL_NRE_Pos                   _U_(0)                                               /**< (TCC_DRVCTRL Position) Non-Recoverable State x Output Enable */
#define TCC_DRVCTRL_NRE_Msk                   (_U_(0xFF) << TCC_DRVCTRL_NRE_Pos)                   /**< (TCC_DRVCTRL Mask) NRE */
#define TCC_DRVCTRL_NRE(value)                (TCC_DRVCTRL_NRE_Msk & ((value) << TCC_DRVCTRL_NRE_Pos)) 
#define TCC_DRVCTRL_NRV_Pos                   _U_(8)                                               /**< (TCC_DRVCTRL Position) Non-Recoverable State x Output Value */
#define TCC_DRVCTRL_NRV_Msk                   (_U_(0xFF) << TCC_DRVCTRL_NRV_Pos)                   /**< (TCC_DRVCTRL Mask) NRV */
#define TCC_DRVCTRL_NRV(value)                (TCC_DRVCTRL_NRV_Msk & ((value) << TCC_DRVCTRL_NRV_Pos)) 
#define TCC_DRVCTRL_INVEN_Pos                 _U_(16)                                              /**< (TCC_DRVCTRL Position) Output Waveform x Inversion */
#define TCC_DRVCTRL_INVEN_Msk                 (_U_(0xFF) << TCC_DRVCTRL_INVEN_Pos)                 /**< (TCC_DRVCTRL Mask) INVEN */
#define TCC_DRVCTRL_INVEN(value)              (TCC_DRVCTRL_INVEN_Msk & ((value) << TCC_DRVCTRL_INVEN_Pos)) 

/* -------- TCC_DBGCTRL : (TCC Offset: 0x1E) (R/W 8) Debug Control -------- */
#define TCC_DBGCTRL_RESETVALUE                _U_(0x00)                                            /**<  (TCC_DBGCTRL) Debug Control  Reset Value */

#define TCC_DBGCTRL_DBGRUN_Pos                _U_(0)                                               /**< (TCC_DBGCTRL) Debug Running Mode Position */
#define TCC_DBGCTRL_DBGRUN_Msk                (_U_(0x1) << TCC_DBGCTRL_DBGRUN_Pos)                 /**< (TCC_DBGCTRL) Debug Running Mode Mask */
#define TCC_DBGCTRL_DBGRUN(value)             (TCC_DBGCTRL_DBGRUN_Msk & ((value) << TCC_DBGCTRL_DBGRUN_Pos))
#define TCC_DBGCTRL_FDDBD_Pos                 _U_(2)                                               /**< (TCC_DBGCTRL) Fault Detection on Debug Break Detection Position */
#define TCC_DBGCTRL_FDDBD_Msk                 (_U_(0x1) << TCC_DBGCTRL_FDDBD_Pos)                  /**< (TCC_DBGCTRL) Fault Detection on Debug Break Detection Mask */
#define TCC_DBGCTRL_FDDBD(value)              (TCC_DBGCTRL_FDDBD_Msk & ((value) << TCC_DBGCTRL_FDDBD_Pos))
#define TCC_DBGCTRL_Msk                       _U_(0x05)                                            /**< (TCC_DBGCTRL) Register Mask  */


/* -------- TCC_EVCTRL : (TCC Offset: 0x20) (R/W 32) Event Control -------- */
#define TCC_EVCTRL_RESETVALUE                 _U_(0x00)                                            /**<  (TCC_EVCTRL) Event Control  Reset Value */

#define TCC_EVCTRL_EVACT0_Pos                 _U_(0)                                               /**< (TCC_EVCTRL) Timer/counter Input Event0 Action Position */
#define TCC_EVCTRL_EVACT0_Msk                 (_U_(0x7) << TCC_EVCTRL_EVACT0_Pos)                  /**< (TCC_EVCTRL) Timer/counter Input Event0 Action Mask */
#define TCC_EVCTRL_EVACT0(value)              (TCC_EVCTRL_EVACT0_Msk & ((value) << TCC_EVCTRL_EVACT0_Pos))
#define   TCC_EVCTRL_EVACT0_OFF_Val           _U_(0x0)                                             /**< (TCC_EVCTRL) Event action disabled  */
#define   TCC_EVCTRL_EVACT0_RETRIGGER_Val     _U_(0x1)                                             /**< (TCC_EVCTRL) Start, restart or re-trigger counter on event  */
#define   TCC_EVCTRL_EVACT0_COUNTEV_Val       _U_(0x2)                                             /**< (TCC_EVCTRL) Count on event  */
#define   TCC_EVCTRL_EVACT0_START_Val         _U_(0x3)                                             /**< (TCC_EVCTRL) Start counter on event  */
#define   TCC_EVCTRL_EVACT0_INC_Val           _U_(0x4)                                             /**< (TCC_EVCTRL) Increment counter on event  */
#define   TCC_EVCTRL_EVACT0_COUNT_Val         _U_(0x5)                                             /**< (TCC_EVCTRL) Count on active state of asynchronous event  */
#define   TCC_EVCTRL_EVACT0_STAMP_Val         _U_(0x6)                                             /**< (TCC_EVCTRL) Stamp capture  */
#define   TCC_EVCTRL_EVACT0_FAULT_Val         _U_(0x7)                                             /**< (TCC_EVCTRL) Non-recoverable fault  */
#define TCC_EVCTRL_EVACT0_OFF                 (TCC_EVCTRL_EVACT0_OFF_Val << TCC_EVCTRL_EVACT0_Pos) /**< (TCC_EVCTRL) Event action disabled Position  */
#define TCC_EVCTRL_EVACT0_RETRIGGER           (TCC_EVCTRL_EVACT0_RETRIGGER_Val << TCC_EVCTRL_EVACT0_Pos) /**< (TCC_EVCTRL) Start, restart or re-trigger counter on event Position  */
#define TCC_EVCTRL_EVACT0_COUNTEV             (TCC_EVCTRL_EVACT0_COUNTEV_Val << TCC_EVCTRL_EVACT0_Pos) /**< (TCC_EVCTRL) Count on event Position  */
#define TCC_EVCTRL_EVACT0_START               (TCC_EVCTRL_EVACT0_START_Val << TCC_EVCTRL_EVACT0_Pos) /**< (TCC_EVCTRL) Start counter on event Position  */
#define TCC_EVCTRL_EVACT0_INC                 (TCC_EVCTRL_EVACT0_INC_Val << TCC_EVCTRL_EVACT0_Pos) /**< (TCC_EVCTRL) Increment counter on event Position  */
#define TCC_EVCTRL_EVACT0_COUNT               (TCC_EVCTRL_EVACT0_COUNT_Val << TCC_EVCTRL_EVACT0_Pos) /**< (TCC_EVCTRL) Count on active state of asynchronous event Position  */
#define TCC_EVCTRL_EVACT0_STAMP               (TCC_EVCTRL_EVACT0_STAMP_Val << TCC_EVCTRL_EVACT0_Pos) /**< (TCC_EVCTRL) Stamp capture Position  */
#define TCC_EVCTRL_EVACT0_FAULT               (TCC_EVCTRL_EVACT0_FAULT_Val << TCC_EVCTRL_EVACT0_Pos) /**< (TCC_EVCTRL) Non-recoverable fault Position  */
#define TCC_EVCTRL_EVACT1_Pos                 _U_(3)                                               /**< (TCC_EVCTRL) Timer/counter Input Event1 Action Position */
#define TCC_EVCTRL_EVACT1_Msk                 (_U_(0x7) << TCC_EVCTRL_EVACT1_Pos)                  /**< (TCC_EVCTRL) Timer/counter Input Event1 Action Mask */
#define TCC_EVCTRL_EVACT1(value)              (TCC_EVCTRL_EVACT1_Msk & ((value) << TCC_EVCTRL_EVACT1_Pos))
#define   TCC_EVCTRL_EVACT1_OFF_Val           _U_(0x0)                                             /**< (TCC_EVCTRL) Event action disabled  */
#define   TCC_EVCTRL_EVACT1_RETRIGGER_Val     _U_(0x1)                                             /**< (TCC_EVCTRL) Re-trigger counter on event  */
#define   TCC_EVCTRL_EVACT1_DIR_Val           _U_(0x2)                                             /**< (TCC_EVCTRL) Direction control  */
#define   TCC_EVCTRL_EVACT1_STOP_Val          _U_(0x3)                                             /**< (TCC_EVCTRL) Stop counter on event  */
#define   TCC_EVCTRL_EVACT1_DEC_Val           _U_(0x4)                                             /**< (TCC_EVCTRL) Decrement counter on event  */
#define   TCC_EVCTRL_EVACT1_PPW_Val           _U_(0x5)                                             /**< (TCC_EVCTRL) Period capture value in CC0 register, pulse width capture value in CC1 register  */
#define   TCC_EVCTRL_EVACT1_PWP_Val           _U_(0x6)                                             /**< (TCC_EVCTRL) Period capture value in CC1 register, pulse width capture value in CC0 register  */
#define   TCC_EVCTRL_EVACT1_FAULT_Val         _U_(0x7)                                             /**< (TCC_EVCTRL) Non-recoverable fault  */
#define TCC_EVCTRL_EVACT1_OFF                 (TCC_EVCTRL_EVACT1_OFF_Val << TCC_EVCTRL_EVACT1_Pos) /**< (TCC_EVCTRL) Event action disabled Position  */
#define TCC_EVCTRL_EVACT1_RETRIGGER           (TCC_EVCTRL_EVACT1_RETRIGGER_Val << TCC_EVCTRL_EVACT1_Pos) /**< (TCC_EVCTRL) Re-trigger counter on event Position  */
#define TCC_EVCTRL_EVACT1_DIR                 (TCC_EVCTRL_EVACT1_DIR_Val << TCC_EVCTRL_EVACT1_Pos) /**< (TCC_EVCTRL) Direction control Position  */
#define TCC_EVCTRL_EVACT1_STOP                (TCC_EVCTRL_EVACT1_STOP_Val << TCC_EVCTRL_EVACT1_Pos) /**< (TCC_EVCTRL) Stop counter on event Position  */
#define TCC_EVCTRL_EVACT1_DEC                 (TCC_EVCTRL_EVACT1_DEC_Val << TCC_EVCTRL_EVACT1_Pos) /**< (TCC_EVCTRL) Decrement counter on event Position  */
#define TCC_EVCTRL_EVACT1_PPW                 (TCC_EVCTRL_EVACT1_PPW_Val << TCC_EVCTRL_EVACT1_Pos) /**< (TCC_EVCTRL) Period capture value in CC0 register, pulse width capture value in CC1 register Position  */
#define TCC_EVCTRL_EVACT1_PWP                 (TCC_EVCTRL_EVACT1_PWP_Val << TCC_EVCTRL_EVACT1_Pos) /**< (TCC_EVCTRL) Period capture value in CC1 register, pulse width capture value in CC0 register Position  */
#define TCC_EVCTRL_EVACT1_FAULT               (TCC_EVCTRL_EVACT1_FAULT_Val << TCC_EVCTRL_EVACT1_Pos) /**< (TCC_EVCTRL) Non-recoverable fault Position  */
#define TCC_EVCTRL_CNTSEL_Pos                 _U_(6)                                               /**< (TCC_EVCTRL) Timer/counter Output Event Mode Position */
#define TCC_EVCTRL_CNTSEL_Msk                 (_U_(0x3) << TCC_EVCTRL_CNTSEL_Pos)                  /**< (TCC_EVCTRL) Timer/counter Output Event Mode Mask */
#define TCC_EVCTRL_CNTSEL(value)              (TCC_EVCTRL_CNTSEL_Msk & ((value) << TCC_EVCTRL_CNTSEL_Pos))
#define   TCC_EVCTRL_CNTSEL_START_Val         _U_(0x0)                                             /**< (TCC_EVCTRL) An interrupt/event is generated when a new counter cycle starts  */
#define   TCC_EVCTRL_CNTSEL_END_Val           _U_(0x1)                                             /**< (TCC_EVCTRL) An interrupt/event is generated when a counter cycle ends  */
#define   TCC_EVCTRL_CNTSEL_BETWEEN_Val       _U_(0x2)                                             /**< (TCC_EVCTRL) An interrupt/event is generated when a counter cycle ends, except for the first and last cycles  */
#define   TCC_EVCTRL_CNTSEL_BOUNDARY_Val      _U_(0x3)                                             /**< (TCC_EVCTRL) An interrupt/event is generated when a new counter cycle starts or a counter cycle ends  */
#define TCC_EVCTRL_CNTSEL_START               (TCC_EVCTRL_CNTSEL_START_Val << TCC_EVCTRL_CNTSEL_Pos) /**< (TCC_EVCTRL) An interrupt/event is generated when a new counter cycle starts Position  */
#define TCC_EVCTRL_CNTSEL_END                 (TCC_EVCTRL_CNTSEL_END_Val << TCC_EVCTRL_CNTSEL_Pos) /**< (TCC_EVCTRL) An interrupt/event is generated when a counter cycle ends Position  */
#define TCC_EVCTRL_CNTSEL_BETWEEN             (TCC_EVCTRL_CNTSEL_BETWEEN_Val << TCC_EVCTRL_CNTSEL_Pos) /**< (TCC_EVCTRL) An interrupt/event is generated when a counter cycle ends, except for the first and last cycles Position  */
#define TCC_EVCTRL_CNTSEL_BOUNDARY            (TCC_EVCTRL_CNTSEL_BOUNDARY_Val << TCC_EVCTRL_CNTSEL_Pos) /**< (TCC_EVCTRL) An interrupt/event is generated when a new counter cycle starts or a counter cycle ends Position  */
#define TCC_EVCTRL_OVFEO_Pos                  _U_(8)                                               /**< (TCC_EVCTRL) Overflow/Underflow Output Event Enable Position */
#define TCC_EVCTRL_OVFEO_Msk                  (_U_(0x1) << TCC_EVCTRL_OVFEO_Pos)                   /**< (TCC_EVCTRL) Overflow/Underflow Output Event Enable Mask */
#define TCC_EVCTRL_OVFEO(value)               (TCC_EVCTRL_OVFEO_Msk & ((value) << TCC_EVCTRL_OVFEO_Pos))
#define TCC_EVCTRL_TRGEO_Pos                  _U_(9)                                               /**< (TCC_EVCTRL) Retrigger Output Event Enable Position */
#define TCC_EVCTRL_TRGEO_Msk                  (_U_(0x1) << TCC_EVCTRL_TRGEO_Pos)                   /**< (TCC_EVCTRL) Retrigger Output Event Enable Mask */
#define TCC_EVCTRL_TRGEO(value)               (TCC_EVCTRL_TRGEO_Msk & ((value) << TCC_EVCTRL_TRGEO_Pos))
#define TCC_EVCTRL_CNTEO_Pos                  _U_(10)                                              /**< (TCC_EVCTRL) Timer/counter Output Event Enable Position */
#define TCC_EVCTRL_CNTEO_Msk                  (_U_(0x1) << TCC_EVCTRL_CNTEO_Pos)                   /**< (TCC_EVCTRL) Timer/counter Output Event Enable Mask */
#define TCC_EVCTRL_CNTEO(value)               (TCC_EVCTRL_CNTEO_Msk & ((value) << TCC_EVCTRL_CNTEO_Pos))
#define TCC_EVCTRL_TCINV0_Pos                 _U_(12)                                              /**< (TCC_EVCTRL) Inverted Event 0 Input Enable Position */
#define TCC_EVCTRL_TCINV0_Msk                 (_U_(0x1) << TCC_EVCTRL_TCINV0_Pos)                  /**< (TCC_EVCTRL) Inverted Event 0 Input Enable Mask */
#define TCC_EVCTRL_TCINV0(value)              (TCC_EVCTRL_TCINV0_Msk & ((value) << TCC_EVCTRL_TCINV0_Pos))
#define TCC_EVCTRL_TCINV1_Pos                 _U_(13)                                              /**< (TCC_EVCTRL) Inverted Event 1 Input Enable Position */
#define TCC_EVCTRL_TCINV1_Msk                 (_U_(0x1) << TCC_EVCTRL_TCINV1_Pos)                  /**< (TCC_EVCTRL) Inverted Event 1 Input Enable Mask */
#define TCC_EVCTRL_TCINV1(value)              (TCC_EVCTRL_TCINV1_Msk & ((value) << TCC_EVCTRL_TCINV1_Pos))
#define TCC_EVCTRL_TCEI0_Pos                  _U_(14)                                              /**< (TCC_EVCTRL) Timer/counter Event 0 Input Enable Position */
#define TCC_EVCTRL_TCEI0_Msk                  (_U_(0x1) << TCC_EVCTRL_TCEI0_Pos)                   /**< (TCC_EVCTRL) Timer/counter Event 0 Input Enable Mask */
#define TCC_EVCTRL_TCEI0(value)               (TCC_EVCTRL_TCEI0_Msk & ((value) << TCC_EVCTRL_TCEI0_Pos))
#define TCC_EVCTRL_TCEI1_Pos                  _U_(15)                                              /**< (TCC_EVCTRL) Timer/counter Event 1 Input Enable Position */
#define TCC_EVCTRL_TCEI1_Msk                  (_U_(0x1) << TCC_EVCTRL_TCEI1_Pos)                   /**< (TCC_EVCTRL) Timer/counter Event 1 Input Enable Mask */
#define TCC_EVCTRL_TCEI1(value)               (TCC_EVCTRL_TCEI1_Msk & ((value) << TCC_EVCTRL_TCEI1_Pos))
#define TCC_EVCTRL_MCEI0_Pos                  _U_(16)                                              /**< (TCC_EVCTRL) Match or Capture Channel 0 Event Input Enable Position */
#define TCC_EVCTRL_MCEI0_Msk                  (_U_(0x1) << TCC_EVCTRL_MCEI0_Pos)                   /**< (TCC_EVCTRL) Match or Capture Channel 0 Event Input Enable Mask */
#define TCC_EVCTRL_MCEI0(value)               (TCC_EVCTRL_MCEI0_Msk & ((value) << TCC_EVCTRL_MCEI0_Pos))
#define TCC_EVCTRL_MCEI1_Pos                  _U_(17)                                              /**< (TCC_EVCTRL) Match or Capture Channel 1 Event Input Enable Position */
#define TCC_EVCTRL_MCEI1_Msk                  (_U_(0x1) << TCC_EVCTRL_MCEI1_Pos)                   /**< (TCC_EVCTRL) Match or Capture Channel 1 Event Input Enable Mask */
#define TCC_EVCTRL_MCEI1(value)               (TCC_EVCTRL_MCEI1_Msk & ((value) << TCC_EVCTRL_MCEI1_Pos))
#define TCC_EVCTRL_MCEI2_Pos                  _U_(18)                                              /**< (TCC_EVCTRL) Match or Capture Channel 2 Event Input Enable Position */
#define TCC_EVCTRL_MCEI2_Msk                  (_U_(0x1) << TCC_EVCTRL_MCEI2_Pos)                   /**< (TCC_EVCTRL) Match or Capture Channel 2 Event Input Enable Mask */
#define TCC_EVCTRL_MCEI2(value)               (TCC_EVCTRL_MCEI2_Msk & ((value) << TCC_EVCTRL_MCEI2_Pos))
#define TCC_EVCTRL_MCEI3_Pos                  _U_(19)                                              /**< (TCC_EVCTRL) Match or Capture Channel 3 Event Input Enable Position */
#define TCC_EVCTRL_MCEI3_Msk                  (_U_(0x1) << TCC_EVCTRL_MCEI3_Pos)                   /**< (TCC_EVCTRL) Match or Capture Channel 3 Event Input Enable Mask */
#define TCC_EVCTRL_MCEI3(value)               (TCC_EVCTRL_MCEI3_Msk & ((value) << TCC_EVCTRL_MCEI3_Pos))
#define TCC_EVCTRL_MCEI4_Pos                  _U_(20)                                              /**< (TCC_EVCTRL) Match or Capture Channel 4 Event Input Enable Position */
#define TCC_EVCTRL_MCEI4_Msk                  (_U_(0x1) << TCC_EVCTRL_MCEI4_Pos)                   /**< (TCC_EVCTRL) Match or Capture Channel 4 Event Input Enable Mask */
#define TCC_EVCTRL_MCEI4(value)               (TCC_EVCTRL_MCEI4_Msk & ((value) << TCC_EVCTRL_MCEI4_Pos))
#define TCC_EVCTRL_MCEI5_Pos                  _U_(21)                                              /**< (TCC_EVCTRL) Match or Capture Channel 5 Event Input Enable Position */
#define TCC_EVCTRL_MCEI5_Msk                  (_U_(0x1) << TCC_EVCTRL_MCEI5_Pos)                   /**< (TCC_EVCTRL) Match or Capture Channel 5 Event Input Enable Mask */
#define TCC_EVCTRL_MCEI5(value)               (TCC_EVCTRL_MCEI5_Msk & ((value) << TCC_EVCTRL_MCEI5_Pos))
#define TCC_EVCTRL_MCEO0_Pos                  _U_(24)                                              /**< (TCC_EVCTRL) Match or Capture Channel 0 Event Output Enable Position */
#define TCC_EVCTRL_MCEO0_Msk                  (_U_(0x1) << TCC_EVCTRL_MCEO0_Pos)                   /**< (TCC_EVCTRL) Match or Capture Channel 0 Event Output Enable Mask */
#define TCC_EVCTRL_MCEO0(value)               (TCC_EVCTRL_MCEO0_Msk & ((value) << TCC_EVCTRL_MCEO0_Pos))
#define TCC_EVCTRL_MCEO1_Pos                  _U_(25)                                              /**< (TCC_EVCTRL) Match or Capture Channel 1 Event Output Enable Position */
#define TCC_EVCTRL_MCEO1_Msk                  (_U_(0x1) << TCC_EVCTRL_MCEO1_Pos)                   /**< (TCC_EVCTRL) Match or Capture Channel 1 Event Output Enable Mask */
#define TCC_EVCTRL_MCEO1(value)               (TCC_EVCTRL_MCEO1_Msk & ((value) << TCC_EVCTRL_MCEO1_Pos))
#define TCC_EVCTRL_MCEO2_Pos                  _U_(26)                                              /**< (TCC_EVCTRL) Match or Capture Channel 2 Event Output Enable Position */
#define TCC_EVCTRL_MCEO2_Msk                  (_U_(0x1) << TCC_EVCTRL_MCEO2_Pos)                   /**< (TCC_EVCTRL) Match or Capture Channel 2 Event Output Enable Mask */
#define TCC_EVCTRL_MCEO2(value)               (TCC_EVCTRL_MCEO2_Msk & ((value) << TCC_EVCTRL_MCEO2_Pos))
#define TCC_EVCTRL_MCEO3_Pos                  _U_(27)                                              /**< (TCC_EVCTRL) Match or Capture Channel 3 Event Output Enable Position */
#define TCC_EVCTRL_MCEO3_Msk                  (_U_(0x1) << TCC_EVCTRL_MCEO3_Pos)                   /**< (TCC_EVCTRL) Match or Capture Channel 3 Event Output Enable Mask */
#define TCC_EVCTRL_MCEO3(value)               (TCC_EVCTRL_MCEO3_Msk & ((value) << TCC_EVCTRL_MCEO3_Pos))
#define TCC_EVCTRL_MCEO4_Pos                  _U_(28)                                              /**< (TCC_EVCTRL) Match or Capture Channel 4 Event Output Enable Position */
#define TCC_EVCTRL_MCEO4_Msk                  (_U_(0x1) << TCC_EVCTRL_MCEO4_Pos)                   /**< (TCC_EVCTRL) Match or Capture Channel 4 Event Output Enable Mask */
#define TCC_EVCTRL_MCEO4(value)               (TCC_EVCTRL_MCEO4_Msk & ((value) << TCC_EVCTRL_MCEO4_Pos))
#define TCC_EVCTRL_MCEO5_Pos                  _U_(29)                                              /**< (TCC_EVCTRL) Match or Capture Channel 5 Event Output Enable Position */
#define TCC_EVCTRL_MCEO5_Msk                  (_U_(0x1) << TCC_EVCTRL_MCEO5_Pos)                   /**< (TCC_EVCTRL) Match or Capture Channel 5 Event Output Enable Mask */
#define TCC_EVCTRL_MCEO5(value)               (TCC_EVCTRL_MCEO5_Msk & ((value) << TCC_EVCTRL_MCEO5_Pos))
#define TCC_EVCTRL_Msk                        _U_(0x3F3FF7FF)                                      /**< (TCC_EVCTRL) Register Mask  */

#define TCC_EVCTRL_TCINV_Pos                  _U_(12)                                              /**< (TCC_EVCTRL Position) Inverted Event x Input Enable */
#define TCC_EVCTRL_TCINV_Msk                  (_U_(0x3) << TCC_EVCTRL_TCINV_Pos)                   /**< (TCC_EVCTRL Mask) TCINV */
#define TCC_EVCTRL_TCINV(value)               (TCC_EVCTRL_TCINV_Msk & ((value) << TCC_EVCTRL_TCINV_Pos)) 
#define TCC_EVCTRL_TCEI_Pos                   _U_(14)                                              /**< (TCC_EVCTRL Position) Timer/counter Event x Input Enable */
#define TCC_EVCTRL_TCEI_Msk                   (_U_(0x3) << TCC_EVCTRL_TCEI_Pos)                    /**< (TCC_EVCTRL Mask) TCEI */
#define TCC_EVCTRL_TCEI(value)                (TCC_EVCTRL_TCEI_Msk & ((value) << TCC_EVCTRL_TCEI_Pos)) 
#define TCC_EVCTRL_MCEI_Pos                   _U_(16)                                              /**< (TCC_EVCTRL Position) Match or Capture Channel x Event Input Enable */
#define TCC_EVCTRL_MCEI_Msk                   (_U_(0x3F) << TCC_EVCTRL_MCEI_Pos)                   /**< (TCC_EVCTRL Mask) MCEI */
#define TCC_EVCTRL_MCEI(value)                (TCC_EVCTRL_MCEI_Msk & ((value) << TCC_EVCTRL_MCEI_Pos)) 
#define TCC_EVCTRL_MCEO_Pos                   _U_(24)                                              /**< (TCC_EVCTRL Position) Match or Capture Channel 5 Event Output Enable */
#define TCC_EVCTRL_MCEO_Msk                   (_U_(0x3F) << TCC_EVCTRL_MCEO_Pos)                   /**< (TCC_EVCTRL Mask) MCEO */
#define TCC_EVCTRL_MCEO(value)                (TCC_EVCTRL_MCEO_Msk & ((value) << TCC_EVCTRL_MCEO_Pos)) 

/* -------- TCC_INTENCLR : (TCC Offset: 0x24) (R/W 32) Interrupt Enable Clear -------- */
#define TCC_INTENCLR_RESETVALUE               _U_(0x00)                                            /**<  (TCC_INTENCLR) Interrupt Enable Clear  Reset Value */

#define TCC_INTENCLR_OVF_Pos                  _U_(0)                                               /**< (TCC_INTENCLR) Overflow Interrupt Enable Position */
#define TCC_INTENCLR_OVF_Msk                  (_U_(0x1) << TCC_INTENCLR_OVF_Pos)                   /**< (TCC_INTENCLR) Overflow Interrupt Enable Mask */
#define TCC_INTENCLR_OVF(value)               (TCC_INTENCLR_OVF_Msk & ((value) << TCC_INTENCLR_OVF_Pos))
#define TCC_INTENCLR_TRG_Pos                  _U_(1)                                               /**< (TCC_INTENCLR) Retrigger Interrupt Enable Position */
#define TCC_INTENCLR_TRG_Msk                  (_U_(0x1) << TCC_INTENCLR_TRG_Pos)                   /**< (TCC_INTENCLR) Retrigger Interrupt Enable Mask */
#define TCC_INTENCLR_TRG(value)               (TCC_INTENCLR_TRG_Msk & ((value) << TCC_INTENCLR_TRG_Pos))
#define TCC_INTENCLR_CNT_Pos                  _U_(2)                                               /**< (TCC_INTENCLR) Counter Interrupt Enable Position */
#define TCC_INTENCLR_CNT_Msk                  (_U_(0x1) << TCC_INTENCLR_CNT_Pos)                   /**< (TCC_INTENCLR) Counter Interrupt Enable Mask */
#define TCC_INTENCLR_CNT(value)               (TCC_INTENCLR_CNT_Msk & ((value) << TCC_INTENCLR_CNT_Pos))
#define TCC_INTENCLR_ERR_Pos                  _U_(3)                                               /**< (TCC_INTENCLR) Error Interrupt Enable Position */
#define TCC_INTENCLR_ERR_Msk                  (_U_(0x1) << TCC_INTENCLR_ERR_Pos)                   /**< (TCC_INTENCLR) Error Interrupt Enable Mask */
#define TCC_INTENCLR_ERR(value)               (TCC_INTENCLR_ERR_Msk & ((value) << TCC_INTENCLR_ERR_Pos))
#define TCC_INTENCLR_UFS_Pos                  _U_(10)                                              /**< (TCC_INTENCLR) Non-Recoverable Update Fault Interrupt Enable Position */
#define TCC_INTENCLR_UFS_Msk                  (_U_(0x1) << TCC_INTENCLR_UFS_Pos)                   /**< (TCC_INTENCLR) Non-Recoverable Update Fault Interrupt Enable Mask */
#define TCC_INTENCLR_UFS(value)               (TCC_INTENCLR_UFS_Msk & ((value) << TCC_INTENCLR_UFS_Pos))
#define TCC_INTENCLR_DFS_Pos                  _U_(11)                                              /**< (TCC_INTENCLR) Non-Recoverable Debug Fault Interrupt Enable Position */
#define TCC_INTENCLR_DFS_Msk                  (_U_(0x1) << TCC_INTENCLR_DFS_Pos)                   /**< (TCC_INTENCLR) Non-Recoverable Debug Fault Interrupt Enable Mask */
#define TCC_INTENCLR_DFS(value)               (TCC_INTENCLR_DFS_Msk & ((value) << TCC_INTENCLR_DFS_Pos))
#define TCC_INTENCLR_FAULTA_Pos               _U_(12)                                              /**< (TCC_INTENCLR) Recoverable Fault A Interrupt Enable Position */
#define TCC_INTENCLR_FAULTA_Msk               (_U_(0x1) << TCC_INTENCLR_FAULTA_Pos)                /**< (TCC_INTENCLR) Recoverable Fault A Interrupt Enable Mask */
#define TCC_INTENCLR_FAULTA(value)            (TCC_INTENCLR_FAULTA_Msk & ((value) << TCC_INTENCLR_FAULTA_Pos))
#define TCC_INTENCLR_FAULTB_Pos               _U_(13)                                              /**< (TCC_INTENCLR) Recoverable Fault B Interrupt Enable Position */
#define TCC_INTENCLR_FAULTB_Msk               (_U_(0x1) << TCC_INTENCLR_FAULTB_Pos)                /**< (TCC_INTENCLR) Recoverable Fault B Interrupt Enable Mask */
#define TCC_INTENCLR_FAULTB(value)            (TCC_INTENCLR_FAULTB_Msk & ((value) << TCC_INTENCLR_FAULTB_Pos))
#define TCC_INTENCLR_FAULT0_Pos               _U_(14)                                              /**< (TCC_INTENCLR) Non-Recoverable Fault 0 Interrupt Enable Position */
#define TCC_INTENCLR_FAULT0_Msk               (_U_(0x1) << TCC_INTENCLR_FAULT0_Pos)                /**< (TCC_INTENCLR) Non-Recoverable Fault 0 Interrupt Enable Mask */
#define TCC_INTENCLR_FAULT0(value)            (TCC_INTENCLR_FAULT0_Msk & ((value) << TCC_INTENCLR_FAULT0_Pos))
#define TCC_INTENCLR_FAULT1_Pos               _U_(15)                                              /**< (TCC_INTENCLR) Non-Recoverable Fault 1 Interrupt Enable Position */
#define TCC_INTENCLR_FAULT1_Msk               (_U_(0x1) << TCC_INTENCLR_FAULT1_Pos)                /**< (TCC_INTENCLR) Non-Recoverable Fault 1 Interrupt Enable Mask */
#define TCC_INTENCLR_FAULT1(value)            (TCC_INTENCLR_FAULT1_Msk & ((value) << TCC_INTENCLR_FAULT1_Pos))
#define TCC_INTENCLR_MC0_Pos                  _U_(16)                                              /**< (TCC_INTENCLR) Match or Capture Channel 0 Interrupt Enable Position */
#define TCC_INTENCLR_MC0_Msk                  (_U_(0x1) << TCC_INTENCLR_MC0_Pos)                   /**< (TCC_INTENCLR) Match or Capture Channel 0 Interrupt Enable Mask */
#define TCC_INTENCLR_MC0(value)               (TCC_INTENCLR_MC0_Msk & ((value) << TCC_INTENCLR_MC0_Pos))
#define TCC_INTENCLR_MC1_Pos                  _U_(17)                                              /**< (TCC_INTENCLR) Match or Capture Channel 1 Interrupt Enable Position */
#define TCC_INTENCLR_MC1_Msk                  (_U_(0x1) << TCC_INTENCLR_MC1_Pos)                   /**< (TCC_INTENCLR) Match or Capture Channel 1 Interrupt Enable Mask */
#define TCC_INTENCLR_MC1(value)               (TCC_INTENCLR_MC1_Msk & ((value) << TCC_INTENCLR_MC1_Pos))
#define TCC_INTENCLR_MC2_Pos                  _U_(18)                                              /**< (TCC_INTENCLR) Match or Capture Channel 2 Interrupt Enable Position */
#define TCC_INTENCLR_MC2_Msk                  (_U_(0x1) << TCC_INTENCLR_MC2_Pos)                   /**< (TCC_INTENCLR) Match or Capture Channel 2 Interrupt Enable Mask */
#define TCC_INTENCLR_MC2(value)               (TCC_INTENCLR_MC2_Msk & ((value) << TCC_INTENCLR_MC2_Pos))
#define TCC_INTENCLR_MC3_Pos                  _U_(19)                                              /**< (TCC_INTENCLR) Match or Capture Channel 3 Interrupt Enable Position */
#define TCC_INTENCLR_MC3_Msk                  (_U_(0x1) << TCC_INTENCLR_MC3_Pos)                   /**< (TCC_INTENCLR) Match or Capture Channel 3 Interrupt Enable Mask */
#define TCC_INTENCLR_MC3(value)               (TCC_INTENCLR_MC3_Msk & ((value) << TCC_INTENCLR_MC3_Pos))
#define TCC_INTENCLR_MC4_Pos                  _U_(20)                                              /**< (TCC_INTENCLR) Match or Capture Channel 4 Interrupt Enable Position */
#define TCC_INTENCLR_MC4_Msk                  (_U_(0x1) << TCC_INTENCLR_MC4_Pos)                   /**< (TCC_INTENCLR) Match or Capture Channel 4 Interrupt Enable Mask */
#define TCC_INTENCLR_MC4(value)               (TCC_INTENCLR_MC4_Msk & ((value) << TCC_INTENCLR_MC4_Pos))
#define TCC_INTENCLR_MC5_Pos                  _U_(21)                                              /**< (TCC_INTENCLR) Match or Capture Channel 5 Interrupt Enable Position */
#define TCC_INTENCLR_MC5_Msk                  (_U_(0x1) << TCC_INTENCLR_MC5_Pos)                   /**< (TCC_INTENCLR) Match or Capture Channel 5 Interrupt Enable Mask */
#define TCC_INTENCLR_MC5(value)               (TCC_INTENCLR_MC5_Msk & ((value) << TCC_INTENCLR_MC5_Pos))
#define TCC_INTENCLR_Msk                      _U_(0x003FFC0F)                                      /**< (TCC_INTENCLR) Register Mask  */

#define TCC_INTENCLR_FAULT_Pos                _U_(14)                                              /**< (TCC_INTENCLR Position) Non-Recoverable Fault x Interrupt Enable */
#define TCC_INTENCLR_FAULT_Msk                (_U_(0x3) << TCC_INTENCLR_FAULT_Pos)                 /**< (TCC_INTENCLR Mask) FAULT */
#define TCC_INTENCLR_FAULT(value)             (TCC_INTENCLR_FAULT_Msk & ((value) << TCC_INTENCLR_FAULT_Pos)) 
#define TCC_INTENCLR_MC_Pos                   _U_(16)                                              /**< (TCC_INTENCLR Position) Match or Capture Channel 5 Interrupt Enable */
#define TCC_INTENCLR_MC_Msk                   (_U_(0x3F) << TCC_INTENCLR_MC_Pos)                   /**< (TCC_INTENCLR Mask) MC */
#define TCC_INTENCLR_MC(value)                (TCC_INTENCLR_MC_Msk & ((value) << TCC_INTENCLR_MC_Pos)) 

/* -------- TCC_INTENSET : (TCC Offset: 0x28) (R/W 32) Interrupt Enable Set -------- */
#define TCC_INTENSET_RESETVALUE               _U_(0x00)                                            /**<  (TCC_INTENSET) Interrupt Enable Set  Reset Value */

#define TCC_INTENSET_OVF_Pos                  _U_(0)                                               /**< (TCC_INTENSET) Overflow Interrupt Enable Position */
#define TCC_INTENSET_OVF_Msk                  (_U_(0x1) << TCC_INTENSET_OVF_Pos)                   /**< (TCC_INTENSET) Overflow Interrupt Enable Mask */
#define TCC_INTENSET_OVF(value)               (TCC_INTENSET_OVF_Msk & ((value) << TCC_INTENSET_OVF_Pos))
#define TCC_INTENSET_TRG_Pos                  _U_(1)                                               /**< (TCC_INTENSET) Retrigger Interrupt Enable Position */
#define TCC_INTENSET_TRG_Msk                  (_U_(0x1) << TCC_INTENSET_TRG_Pos)                   /**< (TCC_INTENSET) Retrigger Interrupt Enable Mask */
#define TCC_INTENSET_TRG(value)               (TCC_INTENSET_TRG_Msk & ((value) << TCC_INTENSET_TRG_Pos))
#define TCC_INTENSET_CNT_Pos                  _U_(2)                                               /**< (TCC_INTENSET) Counter Interrupt Enable Position */
#define TCC_INTENSET_CNT_Msk                  (_U_(0x1) << TCC_INTENSET_CNT_Pos)                   /**< (TCC_INTENSET) Counter Interrupt Enable Mask */
#define TCC_INTENSET_CNT(value)               (TCC_INTENSET_CNT_Msk & ((value) << TCC_INTENSET_CNT_Pos))
#define TCC_INTENSET_ERR_Pos                  _U_(3)                                               /**< (TCC_INTENSET) Error Interrupt Enable Position */
#define TCC_INTENSET_ERR_Msk                  (_U_(0x1) << TCC_INTENSET_ERR_Pos)                   /**< (TCC_INTENSET) Error Interrupt Enable Mask */
#define TCC_INTENSET_ERR(value)               (TCC_INTENSET_ERR_Msk & ((value) << TCC_INTENSET_ERR_Pos))
#define TCC_INTENSET_UFS_Pos                  _U_(10)                                              /**< (TCC_INTENSET) Non-Recoverable Update Fault Interrupt Enable Position */
#define TCC_INTENSET_UFS_Msk                  (_U_(0x1) << TCC_INTENSET_UFS_Pos)                   /**< (TCC_INTENSET) Non-Recoverable Update Fault Interrupt Enable Mask */
#define TCC_INTENSET_UFS(value)               (TCC_INTENSET_UFS_Msk & ((value) << TCC_INTENSET_UFS_Pos))
#define TCC_INTENSET_DFS_Pos                  _U_(11)                                              /**< (TCC_INTENSET) Non-Recoverable Debug Fault Interrupt Enable Position */
#define TCC_INTENSET_DFS_Msk                  (_U_(0x1) << TCC_INTENSET_DFS_Pos)                   /**< (TCC_INTENSET) Non-Recoverable Debug Fault Interrupt Enable Mask */
#define TCC_INTENSET_DFS(value)               (TCC_INTENSET_DFS_Msk & ((value) << TCC_INTENSET_DFS_Pos))
#define TCC_INTENSET_FAULTA_Pos               _U_(12)                                              /**< (TCC_INTENSET) Recoverable Fault A Interrupt Enable Position */
#define TCC_INTENSET_FAULTA_Msk               (_U_(0x1) << TCC_INTENSET_FAULTA_Pos)                /**< (TCC_INTENSET) Recoverable Fault A Interrupt Enable Mask */
#define TCC_INTENSET_FAULTA(value)            (TCC_INTENSET_FAULTA_Msk & ((value) << TCC_INTENSET_FAULTA_Pos))
#define TCC_INTENSET_FAULTB_Pos               _U_(13)                                              /**< (TCC_INTENSET) Recoverable Fault B Interrupt Enable Position */
#define TCC_INTENSET_FAULTB_Msk               (_U_(0x1) << TCC_INTENSET_FAULTB_Pos)                /**< (TCC_INTENSET) Recoverable Fault B Interrupt Enable Mask */
#define TCC_INTENSET_FAULTB(value)            (TCC_INTENSET_FAULTB_Msk & ((value) << TCC_INTENSET_FAULTB_Pos))
#define TCC_INTENSET_FAULT0_Pos               _U_(14)                                              /**< (TCC_INTENSET) Non-Recoverable Fault 0 Interrupt Enable Position */
#define TCC_INTENSET_FAULT0_Msk               (_U_(0x1) << TCC_INTENSET_FAULT0_Pos)                /**< (TCC_INTENSET) Non-Recoverable Fault 0 Interrupt Enable Mask */
#define TCC_INTENSET_FAULT0(value)            (TCC_INTENSET_FAULT0_Msk & ((value) << TCC_INTENSET_FAULT0_Pos))
#define TCC_INTENSET_FAULT1_Pos               _U_(15)                                              /**< (TCC_INTENSET) Non-Recoverable Fault 1 Interrupt Enable Position */
#define TCC_INTENSET_FAULT1_Msk               (_U_(0x1) << TCC_INTENSET_FAULT1_Pos)                /**< (TCC_INTENSET) Non-Recoverable Fault 1 Interrupt Enable Mask */
#define TCC_INTENSET_FAULT1(value)            (TCC_INTENSET_FAULT1_Msk & ((value) << TCC_INTENSET_FAULT1_Pos))
#define TCC_INTENSET_MC0_Pos                  _U_(16)                                              /**< (TCC_INTENSET) Match or Capture Channel 0 Interrupt Enable Position */
#define TCC_INTENSET_MC0_Msk                  (_U_(0x1) << TCC_INTENSET_MC0_Pos)                   /**< (TCC_INTENSET) Match or Capture Channel 0 Interrupt Enable Mask */
#define TCC_INTENSET_MC0(value)               (TCC_INTENSET_MC0_Msk & ((value) << TCC_INTENSET_MC0_Pos))
#define TCC_INTENSET_MC1_Pos                  _U_(17)                                              /**< (TCC_INTENSET) Match or Capture Channel 1 Interrupt Enable Position */
#define TCC_INTENSET_MC1_Msk                  (_U_(0x1) << TCC_INTENSET_MC1_Pos)                   /**< (TCC_INTENSET) Match or Capture Channel 1 Interrupt Enable Mask */
#define TCC_INTENSET_MC1(value)               (TCC_INTENSET_MC1_Msk & ((value) << TCC_INTENSET_MC1_Pos))
#define TCC_INTENSET_MC2_Pos                  _U_(18)                                              /**< (TCC_INTENSET) Match or Capture Channel 2 Interrupt Enable Position */
#define TCC_INTENSET_MC2_Msk                  (_U_(0x1) << TCC_INTENSET_MC2_Pos)                   /**< (TCC_INTENSET) Match or Capture Channel 2 Interrupt Enable Mask */
#define TCC_INTENSET_MC2(value)               (TCC_INTENSET_MC2_Msk & ((value) << TCC_INTENSET_MC2_Pos))
#define TCC_INTENSET_MC3_Pos                  _U_(19)                                              /**< (TCC_INTENSET) Match or Capture Channel 3 Interrupt Enable Position */
#define TCC_INTENSET_MC3_Msk                  (_U_(0x1) << TCC_INTENSET_MC3_Pos)                   /**< (TCC_INTENSET) Match or Capture Channel 3 Interrupt Enable Mask */
#define TCC_INTENSET_MC3(value)               (TCC_INTENSET_MC3_Msk & ((value) << TCC_INTENSET_MC3_Pos))
#define TCC_INTENSET_MC4_Pos                  _U_(20)                                              /**< (TCC_INTENSET) Match or Capture Channel 4 Interrupt Enable Position */
#define TCC_INTENSET_MC4_Msk                  (_U_(0x1) << TCC_INTENSET_MC4_Pos)                   /**< (TCC_INTENSET) Match or Capture Channel 4 Interrupt Enable Mask */
#define TCC_INTENSET_MC4(value)               (TCC_INTENSET_MC4_Msk & ((value) << TCC_INTENSET_MC4_Pos))
#define TCC_INTENSET_MC5_Pos                  _U_(21)                                              /**< (TCC_INTENSET) Match or Capture Channel 5 Interrupt Enable Position */
#define TCC_INTENSET_MC5_Msk                  (_U_(0x1) << TCC_INTENSET_MC5_Pos)                   /**< (TCC_INTENSET) Match or Capture Channel 5 Interrupt Enable Mask */
#define TCC_INTENSET_MC5(value)               (TCC_INTENSET_MC5_Msk & ((value) << TCC_INTENSET_MC5_Pos))
#define TCC_INTENSET_Msk                      _U_(0x003FFC0F)                                      /**< (TCC_INTENSET) Register Mask  */

#define TCC_INTENSET_FAULT_Pos                _U_(14)                                              /**< (TCC_INTENSET Position) Non-Recoverable Fault x Interrupt Enable */
#define TCC_INTENSET_FAULT_Msk                (_U_(0x3) << TCC_INTENSET_FAULT_Pos)                 /**< (TCC_INTENSET Mask) FAULT */
#define TCC_INTENSET_FAULT(value)             (TCC_INTENSET_FAULT_Msk & ((value) << TCC_INTENSET_FAULT_Pos)) 
#define TCC_INTENSET_MC_Pos                   _U_(16)                                              /**< (TCC_INTENSET Position) Match or Capture Channel 5 Interrupt Enable */
#define TCC_INTENSET_MC_Msk                   (_U_(0x3F) << TCC_INTENSET_MC_Pos)                   /**< (TCC_INTENSET Mask) MC */
#define TCC_INTENSET_MC(value)                (TCC_INTENSET_MC_Msk & ((value) << TCC_INTENSET_MC_Pos)) 

/* -------- TCC_INTFLAG : (TCC Offset: 0x2C) (R/W 32) Interrupt Flag Status and Clear -------- */
#define TCC_INTFLAG_RESETVALUE                _U_(0x00)                                            /**<  (TCC_INTFLAG) Interrupt Flag Status and Clear  Reset Value */

#define TCC_INTFLAG_OVF_Pos                   _U_(0)                                               /**< (TCC_INTFLAG) Overflow Position */
#define TCC_INTFLAG_OVF_Msk                   (_U_(0x1) << TCC_INTFLAG_OVF_Pos)                    /**< (TCC_INTFLAG) Overflow Mask */
#define TCC_INTFLAG_OVF(value)                (TCC_INTFLAG_OVF_Msk & ((value) << TCC_INTFLAG_OVF_Pos))
#define TCC_INTFLAG_TRG_Pos                   _U_(1)                                               /**< (TCC_INTFLAG) Retrigger Position */
#define TCC_INTFLAG_TRG_Msk                   (_U_(0x1) << TCC_INTFLAG_TRG_Pos)                    /**< (TCC_INTFLAG) Retrigger Mask */
#define TCC_INTFLAG_TRG(value)                (TCC_INTFLAG_TRG_Msk & ((value) << TCC_INTFLAG_TRG_Pos))
#define TCC_INTFLAG_CNT_Pos                   _U_(2)                                               /**< (TCC_INTFLAG) Counter Position */
#define TCC_INTFLAG_CNT_Msk                   (_U_(0x1) << TCC_INTFLAG_CNT_Pos)                    /**< (TCC_INTFLAG) Counter Mask */
#define TCC_INTFLAG_CNT(value)                (TCC_INTFLAG_CNT_Msk & ((value) << TCC_INTFLAG_CNT_Pos))
#define TCC_INTFLAG_ERR_Pos                   _U_(3)                                               /**< (TCC_INTFLAG) Error Position */
#define TCC_INTFLAG_ERR_Msk                   (_U_(0x1) << TCC_INTFLAG_ERR_Pos)                    /**< (TCC_INTFLAG) Error Mask */
#define TCC_INTFLAG_ERR(value)                (TCC_INTFLAG_ERR_Msk & ((value) << TCC_INTFLAG_ERR_Pos))
#define TCC_INTFLAG_UFS_Pos                   _U_(10)                                              /**< (TCC_INTFLAG) Non-Recoverable Update Fault Position */
#define TCC_INTFLAG_UFS_Msk                   (_U_(0x1) << TCC_INTFLAG_UFS_Pos)                    /**< (TCC_INTFLAG) Non-Recoverable Update Fault Mask */
#define TCC_INTFLAG_UFS(value)                (TCC_INTFLAG_UFS_Msk & ((value) << TCC_INTFLAG_UFS_Pos))
#define TCC_INTFLAG_DFS_Pos                   _U_(11)                                              /**< (TCC_INTFLAG) Non-Recoverable Debug Fault Position */
#define TCC_INTFLAG_DFS_Msk                   (_U_(0x1) << TCC_INTFLAG_DFS_Pos)                    /**< (TCC_INTFLAG) Non-Recoverable Debug Fault Mask */
#define TCC_INTFLAG_DFS(value)                (TCC_INTFLAG_DFS_Msk & ((value) << TCC_INTFLAG_DFS_Pos))
#define TCC_INTFLAG_FAULTA_Pos                _U_(12)                                              /**< (TCC_INTFLAG) Recoverable Fault A Position */
#define TCC_INTFLAG_FAULTA_Msk                (_U_(0x1) << TCC_INTFLAG_FAULTA_Pos)                 /**< (TCC_INTFLAG) Recoverable Fault A Mask */
#define TCC_INTFLAG_FAULTA(value)             (TCC_INTFLAG_FAULTA_Msk & ((value) << TCC_INTFLAG_FAULTA_Pos))
#define TCC_INTFLAG_FAULTB_Pos                _U_(13)                                              /**< (TCC_INTFLAG) Recoverable Fault B Position */
#define TCC_INTFLAG_FAULTB_Msk                (_U_(0x1) << TCC_INTFLAG_FAULTB_Pos)                 /**< (TCC_INTFLAG) Recoverable Fault B Mask */
#define TCC_INTFLAG_FAULTB(value)             (TCC_INTFLAG_FAULTB_Msk & ((value) << TCC_INTFLAG_FAULTB_Pos))
#define TCC_INTFLAG_FAULT0_Pos                _U_(14)                                              /**< (TCC_INTFLAG) Non-Recoverable Fault 0 Position */
#define TCC_INTFLAG_FAULT0_Msk                (_U_(0x1) << TCC_INTFLAG_FAULT0_Pos)                 /**< (TCC_INTFLAG) Non-Recoverable Fault 0 Mask */
#define TCC_INTFLAG_FAULT0(value)             (TCC_INTFLAG_FAULT0_Msk & ((value) << TCC_INTFLAG_FAULT0_Pos))
#define TCC_INTFLAG_FAULT1_Pos                _U_(15)                                              /**< (TCC_INTFLAG) Non-Recoverable Fault 1 Position */
#define TCC_INTFLAG_FAULT1_Msk                (_U_(0x1) << TCC_INTFLAG_FAULT1_Pos)                 /**< (TCC_INTFLAG) Non-Recoverable Fault 1 Mask */
#define TCC_INTFLAG_FAULT1(value)             (TCC_INTFLAG_FAULT1_Msk & ((value) << TCC_INTFLAG_FAULT1_Pos))
#define TCC_INTFLAG_MC0_Pos                   _U_(16)                                              /**< (TCC_INTFLAG) Match or Capture 0 Position */
#define TCC_INTFLAG_MC0_Msk                   (_U_(0x1) << TCC_INTFLAG_MC0_Pos)                    /**< (TCC_INTFLAG) Match or Capture 0 Mask */
#define TCC_INTFLAG_MC0(value)                (TCC_INTFLAG_MC0_Msk & ((value) << TCC_INTFLAG_MC0_Pos))
#define TCC_INTFLAG_MC1_Pos                   _U_(17)                                              /**< (TCC_INTFLAG) Match or Capture 1 Position */
#define TCC_INTFLAG_MC1_Msk                   (_U_(0x1) << TCC_INTFLAG_MC1_Pos)                    /**< (TCC_INTFLAG) Match or Capture 1 Mask */
#define TCC_INTFLAG_MC1(value)                (TCC_INTFLAG_MC1_Msk & ((value) << TCC_INTFLAG_MC1_Pos))
#define TCC_INTFLAG_MC2_Pos                   _U_(18)                                              /**< (TCC_INTFLAG) Match or Capture 2 Position */
#define TCC_INTFLAG_MC2_Msk                   (_U_(0x1) << TCC_INTFLAG_MC2_Pos)                    /**< (TCC_INTFLAG) Match or Capture 2 Mask */
#define TCC_INTFLAG_MC2(value)                (TCC_INTFLAG_MC2_Msk & ((value) << TCC_INTFLAG_MC2_Pos))
#define TCC_INTFLAG_MC3_Pos                   _U_(19)                                              /**< (TCC_INTFLAG) Match or Capture 3 Position */
#define TCC_INTFLAG_MC3_Msk                   (_U_(0x1) << TCC_INTFLAG_MC3_Pos)                    /**< (TCC_INTFLAG) Match or Capture 3 Mask */
#define TCC_INTFLAG_MC3(value)                (TCC_INTFLAG_MC3_Msk & ((value) << TCC_INTFLAG_MC3_Pos))
#define TCC_INTFLAG_MC4_Pos                   _U_(20)                                              /**< (TCC_INTFLAG) Match or Capture 4 Position */
#define TCC_INTFLAG_MC4_Msk                   (_U_(0x1) << TCC_INTFLAG_MC4_Pos)                    /**< (TCC_INTFLAG) Match or Capture 4 Mask */
#define TCC_INTFLAG_MC4(value)                (TCC_INTFLAG_MC4_Msk & ((value) << TCC_INTFLAG_MC4_Pos))
#define TCC_INTFLAG_MC5_Pos                   _U_(21)                                              /**< (TCC_INTFLAG) Match or Capture 5 Position */
#define TCC_INTFLAG_MC5_Msk                   (_U_(0x1) << TCC_INTFLAG_MC5_Pos)                    /**< (TCC_INTFLAG) Match or Capture 5 Mask */
#define TCC_INTFLAG_MC5(value)                (TCC_INTFLAG_MC5_Msk & ((value) << TCC_INTFLAG_MC5_Pos))
#define TCC_INTFLAG_Msk                       _U_(0x003FFC0F)                                      /**< (TCC_INTFLAG) Register Mask  */

#define TCC_INTFLAG_FAULT_Pos                 _U_(14)                                              /**< (TCC_INTFLAG Position) Non-Recoverable Fault x */
#define TCC_INTFLAG_FAULT_Msk                 (_U_(0x3) << TCC_INTFLAG_FAULT_Pos)                  /**< (TCC_INTFLAG Mask) FAULT */
#define TCC_INTFLAG_FAULT(value)              (TCC_INTFLAG_FAULT_Msk & ((value) << TCC_INTFLAG_FAULT_Pos)) 
#define TCC_INTFLAG_MC_Pos                    _U_(16)                                              /**< (TCC_INTFLAG Position) Match or Capture 5 */
#define TCC_INTFLAG_MC_Msk                    (_U_(0x3F) << TCC_INTFLAG_MC_Pos)                    /**< (TCC_INTFLAG Mask) MC */
#define TCC_INTFLAG_MC(value)                 (TCC_INTFLAG_MC_Msk & ((value) << TCC_INTFLAG_MC_Pos)) 

/* -------- TCC_STATUS : (TCC Offset: 0x30) (R/W 32) Status -------- */
#define TCC_STATUS_RESETVALUE                 _U_(0x01)                                            /**<  (TCC_STATUS) Status  Reset Value */

#define TCC_STATUS_STOP_Pos                   _U_(0)                                               /**< (TCC_STATUS) Stop Position */
#define TCC_STATUS_STOP_Msk                   (_U_(0x1) << TCC_STATUS_STOP_Pos)                    /**< (TCC_STATUS) Stop Mask */
#define TCC_STATUS_STOP(value)                (TCC_STATUS_STOP_Msk & ((value) << TCC_STATUS_STOP_Pos))
#define TCC_STATUS_IDX_Pos                    _U_(1)                                               /**< (TCC_STATUS) Ramp Position */
#define TCC_STATUS_IDX_Msk                    (_U_(0x1) << TCC_STATUS_IDX_Pos)                     /**< (TCC_STATUS) Ramp Mask */
#define TCC_STATUS_IDX(value)                 (TCC_STATUS_IDX_Msk & ((value) << TCC_STATUS_IDX_Pos))
#define TCC_STATUS_UFS_Pos                    _U_(2)                                               /**< (TCC_STATUS) Non-recoverable Update Fault State Position */
#define TCC_STATUS_UFS_Msk                    (_U_(0x1) << TCC_STATUS_UFS_Pos)                     /**< (TCC_STATUS) Non-recoverable Update Fault State Mask */
#define TCC_STATUS_UFS(value)                 (TCC_STATUS_UFS_Msk & ((value) << TCC_STATUS_UFS_Pos))
#define TCC_STATUS_DFS_Pos                    _U_(3)                                               /**< (TCC_STATUS) Non-Recoverable Debug Fault State Position */
#define TCC_STATUS_DFS_Msk                    (_U_(0x1) << TCC_STATUS_DFS_Pos)                     /**< (TCC_STATUS) Non-Recoverable Debug Fault State Mask */
#define TCC_STATUS_DFS(value)                 (TCC_STATUS_DFS_Msk & ((value) << TCC_STATUS_DFS_Pos))
#define TCC_STATUS_SLAVE_Pos                  _U_(4)                                               /**< (TCC_STATUS) Slave Position */
#define TCC_STATUS_SLAVE_Msk                  (_U_(0x1) << TCC_STATUS_SLAVE_Pos)                   /**< (TCC_STATUS) Slave Mask */
#define TCC_STATUS_SLAVE(value)               (TCC_STATUS_SLAVE_Msk & ((value) << TCC_STATUS_SLAVE_Pos))
#define TCC_STATUS_PATTBUFV_Pos               _U_(5)                                               /**< (TCC_STATUS) Pattern Buffer Valid Position */
#define TCC_STATUS_PATTBUFV_Msk               (_U_(0x1) << TCC_STATUS_PATTBUFV_Pos)                /**< (TCC_STATUS) Pattern Buffer Valid Mask */
#define TCC_STATUS_PATTBUFV(value)            (TCC_STATUS_PATTBUFV_Msk & ((value) << TCC_STATUS_PATTBUFV_Pos))
#define TCC_STATUS_PERBUFV_Pos                _U_(7)                                               /**< (TCC_STATUS) Period Buffer Valid Position */
#define TCC_STATUS_PERBUFV_Msk                (_U_(0x1) << TCC_STATUS_PERBUFV_Pos)                 /**< (TCC_STATUS) Period Buffer Valid Mask */
#define TCC_STATUS_PERBUFV(value)             (TCC_STATUS_PERBUFV_Msk & ((value) << TCC_STATUS_PERBUFV_Pos))
#define TCC_STATUS_FAULTAIN_Pos               _U_(8)                                               /**< (TCC_STATUS) Recoverable Fault A Input Position */
#define TCC_STATUS_FAULTAIN_Msk               (_U_(0x1) << TCC_STATUS_FAULTAIN_Pos)                /**< (TCC_STATUS) Recoverable Fault A Input Mask */
#define TCC_STATUS_FAULTAIN(value)            (TCC_STATUS_FAULTAIN_Msk & ((value) << TCC_STATUS_FAULTAIN_Pos))
#define TCC_STATUS_FAULTBIN_Pos               _U_(9)                                               /**< (TCC_STATUS) Recoverable Fault B Input Position */
#define TCC_STATUS_FAULTBIN_Msk               (_U_(0x1) << TCC_STATUS_FAULTBIN_Pos)                /**< (TCC_STATUS) Recoverable Fault B Input Mask */
#define TCC_STATUS_FAULTBIN(value)            (TCC_STATUS_FAULTBIN_Msk & ((value) << TCC_STATUS_FAULTBIN_Pos))
#define TCC_STATUS_FAULT0IN_Pos               _U_(10)                                              /**< (TCC_STATUS) Non-Recoverable Fault0 Input Position */
#define TCC_STATUS_FAULT0IN_Msk               (_U_(0x1) << TCC_STATUS_FAULT0IN_Pos)                /**< (TCC_STATUS) Non-Recoverable Fault0 Input Mask */
#define TCC_STATUS_FAULT0IN(value)            (TCC_STATUS_FAULT0IN_Msk & ((value) << TCC_STATUS_FAULT0IN_Pos))
#define TCC_STATUS_FAULT1IN_Pos               _U_(11)                                              /**< (TCC_STATUS) Non-Recoverable Fault1 Input Position */
#define TCC_STATUS_FAULT1IN_Msk               (_U_(0x1) << TCC_STATUS_FAULT1IN_Pos)                /**< (TCC_STATUS) Non-Recoverable Fault1 Input Mask */
#define TCC_STATUS_FAULT1IN(value)            (TCC_STATUS_FAULT1IN_Msk & ((value) << TCC_STATUS_FAULT1IN_Pos))
#define TCC_STATUS_FAULTA_Pos                 _U_(12)                                              /**< (TCC_STATUS) Recoverable Fault A State Position */
#define TCC_STATUS_FAULTA_Msk                 (_U_(0x1) << TCC_STATUS_FAULTA_Pos)                  /**< (TCC_STATUS) Recoverable Fault A State Mask */
#define TCC_STATUS_FAULTA(value)              (TCC_STATUS_FAULTA_Msk & ((value) << TCC_STATUS_FAULTA_Pos))
#define TCC_STATUS_FAULTB_Pos                 _U_(13)                                              /**< (TCC_STATUS) Recoverable Fault B State Position */
#define TCC_STATUS_FAULTB_Msk                 (_U_(0x1) << TCC_STATUS_FAULTB_Pos)                  /**< (TCC_STATUS) Recoverable Fault B State Mask */
#define TCC_STATUS_FAULTB(value)              (TCC_STATUS_FAULTB_Msk & ((value) << TCC_STATUS_FAULTB_Pos))
#define TCC_STATUS_FAULT0_Pos                 _U_(14)                                              /**< (TCC_STATUS) Non-Recoverable Fault 0 State Position */
#define TCC_STATUS_FAULT0_Msk                 (_U_(0x1) << TCC_STATUS_FAULT0_Pos)                  /**< (TCC_STATUS) Non-Recoverable Fault 0 State Mask */
#define TCC_STATUS_FAULT0(value)              (TCC_STATUS_FAULT0_Msk & ((value) << TCC_STATUS_FAULT0_Pos))
#define TCC_STATUS_FAULT1_Pos                 _U_(15)                                              /**< (TCC_STATUS) Non-Recoverable Fault 1 State Position */
#define TCC_STATUS_FAULT1_Msk                 (_U_(0x1) << TCC_STATUS_FAULT1_Pos)                  /**< (TCC_STATUS) Non-Recoverable Fault 1 State Mask */
#define TCC_STATUS_FAULT1(value)              (TCC_STATUS_FAULT1_Msk & ((value) << TCC_STATUS_FAULT1_Pos))
#define TCC_STATUS_CCBUFV0_Pos                _U_(16)                                              /**< (TCC_STATUS) Compare Channel 0 Buffer Valid Position */
#define TCC_STATUS_CCBUFV0_Msk                (_U_(0x1) << TCC_STATUS_CCBUFV0_Pos)                 /**< (TCC_STATUS) Compare Channel 0 Buffer Valid Mask */
#define TCC_STATUS_CCBUFV0(value)             (TCC_STATUS_CCBUFV0_Msk & ((value) << TCC_STATUS_CCBUFV0_Pos))
#define TCC_STATUS_CCBUFV1_Pos                _U_(17)                                              /**< (TCC_STATUS) Compare Channel 1 Buffer Valid Position */
#define TCC_STATUS_CCBUFV1_Msk                (_U_(0x1) << TCC_STATUS_CCBUFV1_Pos)                 /**< (TCC_STATUS) Compare Channel 1 Buffer Valid Mask */
#define TCC_STATUS_CCBUFV1(value)             (TCC_STATUS_CCBUFV1_Msk & ((value) << TCC_STATUS_CCBUFV1_Pos))
#define TCC_STATUS_CCBUFV2_Pos                _U_(18)                                              /**< (TCC_STATUS) Compare Channel 2 Buffer Valid Position */
#define TCC_STATUS_CCBUFV2_Msk                (_U_(0x1) << TCC_STATUS_CCBUFV2_Pos)                 /**< (TCC_STATUS) Compare Channel 2 Buffer Valid Mask */
#define TCC_STATUS_CCBUFV2(value)             (TCC_STATUS_CCBUFV2_Msk & ((value) << TCC_STATUS_CCBUFV2_Pos))
#define TCC_STATUS_CCBUFV3_Pos                _U_(19)                                              /**< (TCC_STATUS) Compare Channel 3 Buffer Valid Position */
#define TCC_STATUS_CCBUFV3_Msk                (_U_(0x1) << TCC_STATUS_CCBUFV3_Pos)                 /**< (TCC_STATUS) Compare Channel 3 Buffer Valid Mask */
#define TCC_STATUS_CCBUFV3(value)             (TCC_STATUS_CCBUFV3_Msk & ((value) << TCC_STATUS_CCBUFV3_Pos))
#define TCC_STATUS_CCBUFV4_Pos                _U_(20)                                              /**< (TCC_STATUS) Compare Channel 4 Buffer Valid Position */
#define TCC_STATUS_CCBUFV4_Msk                (_U_(0x1) << TCC_STATUS_CCBUFV4_Pos)                 /**< (TCC_STATUS) Compare Channel 4 Buffer Valid Mask */
#define TCC_STATUS_CCBUFV4(value)             (TCC_STATUS_CCBUFV4_Msk & ((value) << TCC_STATUS_CCBUFV4_Pos))
#define TCC_STATUS_CCBUFV5_Pos                _U_(21)                                              /**< (TCC_STATUS) Compare Channel 5 Buffer Valid Position */
#define TCC_STATUS_CCBUFV5_Msk                (_U_(0x1) << TCC_STATUS_CCBUFV5_Pos)                 /**< (TCC_STATUS) Compare Channel 5 Buffer Valid Mask */
#define TCC_STATUS_CCBUFV5(value)             (TCC_STATUS_CCBUFV5_Msk & ((value) << TCC_STATUS_CCBUFV5_Pos))
#define TCC_STATUS_CMP0_Pos                   _U_(24)                                              /**< (TCC_STATUS) Compare Channel 0 Value Position */
#define TCC_STATUS_CMP0_Msk                   (_U_(0x1) << TCC_STATUS_CMP0_Pos)                    /**< (TCC_STATUS) Compare Channel 0 Value Mask */
#define TCC_STATUS_CMP0(value)                (TCC_STATUS_CMP0_Msk & ((value) << TCC_STATUS_CMP0_Pos))
#define TCC_STATUS_CMP1_Pos                   _U_(25)                                              /**< (TCC_STATUS) Compare Channel 1 Value Position */
#define TCC_STATUS_CMP1_Msk                   (_U_(0x1) << TCC_STATUS_CMP1_Pos)                    /**< (TCC_STATUS) Compare Channel 1 Value Mask */
#define TCC_STATUS_CMP1(value)                (TCC_STATUS_CMP1_Msk & ((value) << TCC_STATUS_CMP1_Pos))
#define TCC_STATUS_CMP2_Pos                   _U_(26)                                              /**< (TCC_STATUS) Compare Channel 2 Value Position */
#define TCC_STATUS_CMP2_Msk                   (_U_(0x1) << TCC_STATUS_CMP2_Pos)                    /**< (TCC_STATUS) Compare Channel 2 Value Mask */
#define TCC_STATUS_CMP2(value)                (TCC_STATUS_CMP2_Msk & ((value) << TCC_STATUS_CMP2_Pos))
#define TCC_STATUS_CMP3_Pos                   _U_(27)                                              /**< (TCC_STATUS) Compare Channel 3 Value Position */
#define TCC_STATUS_CMP3_Msk                   (_U_(0x1) << TCC_STATUS_CMP3_Pos)                    /**< (TCC_STATUS) Compare Channel 3 Value Mask */
#define TCC_STATUS_CMP3(value)                (TCC_STATUS_CMP3_Msk & ((value) << TCC_STATUS_CMP3_Pos))
#define TCC_STATUS_CMP4_Pos                   _U_(28)                                              /**< (TCC_STATUS) Compare Channel 4 Value Position */
#define TCC_STATUS_CMP4_Msk                   (_U_(0x1) << TCC_STATUS_CMP4_Pos)                    /**< (TCC_STATUS) Compare Channel 4 Value Mask */
#define TCC_STATUS_CMP4(value)                (TCC_STATUS_CMP4_Msk & ((value) << TCC_STATUS_CMP4_Pos))
#define TCC_STATUS_CMP5_Pos                   _U_(29)                                              /**< (TCC_STATUS) Compare Channel 5 Value Position */
#define TCC_STATUS_CMP5_Msk                   (_U_(0x1) << TCC_STATUS_CMP5_Pos)                    /**< (TCC_STATUS) Compare Channel 5 Value Mask */
#define TCC_STATUS_CMP5(value)                (TCC_STATUS_CMP5_Msk & ((value) << TCC_STATUS_CMP5_Pos))
#define TCC_STATUS_Msk                        _U_(0x3F3FFFBF)                                      /**< (TCC_STATUS) Register Mask  */

#define TCC_STATUS_FAULT_Pos                  _U_(14)                                              /**< (TCC_STATUS Position) Non-Recoverable Fault x State */
#define TCC_STATUS_FAULT_Msk                  (_U_(0x3) << TCC_STATUS_FAULT_Pos)                   /**< (TCC_STATUS Mask) FAULT */
#define TCC_STATUS_FAULT(value)               (TCC_STATUS_FAULT_Msk & ((value) << TCC_STATUS_FAULT_Pos)) 
#define TCC_STATUS_CCBUFV_Pos                 _U_(16)                                              /**< (TCC_STATUS Position) Compare Channel x Buffer Valid */
#define TCC_STATUS_CCBUFV_Msk                 (_U_(0x3F) << TCC_STATUS_CCBUFV_Pos)                 /**< (TCC_STATUS Mask) CCBUFV */
#define TCC_STATUS_CCBUFV(value)              (TCC_STATUS_CCBUFV_Msk & ((value) << TCC_STATUS_CCBUFV_Pos)) 
#define TCC_STATUS_CMP_Pos                    _U_(24)                                              /**< (TCC_STATUS Position) Compare Channel 5 Value */
#define TCC_STATUS_CMP_Msk                    (_U_(0x3F) << TCC_STATUS_CMP_Pos)                    /**< (TCC_STATUS Mask) CMP */
#define TCC_STATUS_CMP(value)                 (TCC_STATUS_CMP_Msk & ((value) << TCC_STATUS_CMP_Pos)) 

/* -------- TCC_COUNT : (TCC Offset: 0x34) (R/W 32) Count -------- */
#define TCC_COUNT_RESETVALUE                  _U_(0x00)                                            /**<  (TCC_COUNT) Count  Reset Value */

#define TCC_COUNT_COUNT_Pos                   _U_(0)                                               /**< (TCC_COUNT) Counter Value Position */
#define TCC_COUNT_COUNT_Msk                   (_U_(0xFFFFFF) << TCC_COUNT_COUNT_Pos)               /**< (TCC_COUNT) Counter Value Mask */
#define TCC_COUNT_COUNT(value)                (TCC_COUNT_COUNT_Msk & ((value) << TCC_COUNT_COUNT_Pos))
#define TCC_COUNT_Msk                         _U_(0x00FFFFFF)                                      /**< (TCC_COUNT) Register Mask  */

/* DITH4 mode */
#define TCC_COUNT_DITH4_COUNT_Pos             _U_(4)                                               /**< (TCC_COUNT) Counter Value Position */
#define TCC_COUNT_DITH4_COUNT_Msk             (_U_(0xFFFFF) << TCC_COUNT_DITH4_COUNT_Pos)          /**< (TCC_COUNT) Counter Value Mask */
#define TCC_COUNT_DITH4_COUNT(value)          (TCC_COUNT_DITH4_COUNT_Msk & ((value) << TCC_COUNT_DITH4_COUNT_Pos))
#define TCC_COUNT_DITH4_Msk                   _U_(0x00FFFFF0)                                       /**< (TCC_COUNT_DITH4) Register Mask  */

/* DITH5 mode */
#define TCC_COUNT_DITH5_COUNT_Pos             _U_(5)                                               /**< (TCC_COUNT) Counter Value Position */
#define TCC_COUNT_DITH5_COUNT_Msk             (_U_(0x7FFFF) << TCC_COUNT_DITH5_COUNT_Pos)          /**< (TCC_COUNT) Counter Value Mask */
#define TCC_COUNT_DITH5_COUNT(value)          (TCC_COUNT_DITH5_COUNT_Msk & ((value) << TCC_COUNT_DITH5_COUNT_Pos))
#define TCC_COUNT_DITH5_Msk                   _U_(0x00FFFFE0)                                       /**< (TCC_COUNT_DITH5) Register Mask  */

/* DITH6 mode */
#define TCC_COUNT_DITH6_COUNT_Pos             _U_(6)                                               /**< (TCC_COUNT) Counter Value Position */
#define TCC_COUNT_DITH6_COUNT_Msk             (_U_(0x3FFFF) << TCC_COUNT_DITH6_COUNT_Pos)          /**< (TCC_COUNT) Counter Value Mask */
#define TCC_COUNT_DITH6_COUNT(value)          (TCC_COUNT_DITH6_COUNT_Msk & ((value) << TCC_COUNT_DITH6_COUNT_Pos))
#define TCC_COUNT_DITH6_Msk                   _U_(0x00FFFFC0)                                       /**< (TCC_COUNT_DITH6) Register Mask  */


/* -------- TCC_PATT : (TCC Offset: 0x38) (R/W 16) Pattern -------- */
#define TCC_PATT_RESETVALUE                   _U_(0x00)                                            /**<  (TCC_PATT) Pattern  Reset Value */

#define TCC_PATT_PGE0_Pos                     _U_(0)                                               /**< (TCC_PATT) Pattern Generator 0 Output Enable Position */
#define TCC_PATT_PGE0_Msk                     (_U_(0x1) << TCC_PATT_PGE0_Pos)                      /**< (TCC_PATT) Pattern Generator 0 Output Enable Mask */
#define TCC_PATT_PGE0(value)                  (TCC_PATT_PGE0_Msk & ((value) << TCC_PATT_PGE0_Pos))
#define TCC_PATT_PGE1_Pos                     _U_(1)                                               /**< (TCC_PATT) Pattern Generator 1 Output Enable Position */
#define TCC_PATT_PGE1_Msk                     (_U_(0x1) << TCC_PATT_PGE1_Pos)                      /**< (TCC_PATT) Pattern Generator 1 Output Enable Mask */
#define TCC_PATT_PGE1(value)                  (TCC_PATT_PGE1_Msk & ((value) << TCC_PATT_PGE1_Pos))
#define TCC_PATT_PGE2_Pos                     _U_(2)                                               /**< (TCC_PATT) Pattern Generator 2 Output Enable Position */
#define TCC_PATT_PGE2_Msk                     (_U_(0x1) << TCC_PATT_PGE2_Pos)                      /**< (TCC_PATT) Pattern Generator 2 Output Enable Mask */
#define TCC_PATT_PGE2(value)                  (TCC_PATT_PGE2_Msk & ((value) << TCC_PATT_PGE2_Pos))
#define TCC_PATT_PGE3_Pos                     _U_(3)                                               /**< (TCC_PATT) Pattern Generator 3 Output Enable Position */
#define TCC_PATT_PGE3_Msk                     (_U_(0x1) << TCC_PATT_PGE3_Pos)                      /**< (TCC_PATT) Pattern Generator 3 Output Enable Mask */
#define TCC_PATT_PGE3(value)                  (TCC_PATT_PGE3_Msk & ((value) << TCC_PATT_PGE3_Pos))
#define TCC_PATT_PGE4_Pos                     _U_(4)                                               /**< (TCC_PATT) Pattern Generator 4 Output Enable Position */
#define TCC_PATT_PGE4_Msk                     (_U_(0x1) << TCC_PATT_PGE4_Pos)                      /**< (TCC_PATT) Pattern Generator 4 Output Enable Mask */
#define TCC_PATT_PGE4(value)                  (TCC_PATT_PGE4_Msk & ((value) << TCC_PATT_PGE4_Pos))
#define TCC_PATT_PGE5_Pos                     _U_(5)                                               /**< (TCC_PATT) Pattern Generator 5 Output Enable Position */
#define TCC_PATT_PGE5_Msk                     (_U_(0x1) << TCC_PATT_PGE5_Pos)                      /**< (TCC_PATT) Pattern Generator 5 Output Enable Mask */
#define TCC_PATT_PGE5(value)                  (TCC_PATT_PGE5_Msk & ((value) << TCC_PATT_PGE5_Pos))
#define TCC_PATT_PGE6_Pos                     _U_(6)                                               /**< (TCC_PATT) Pattern Generator 6 Output Enable Position */
#define TCC_PATT_PGE6_Msk                     (_U_(0x1) << TCC_PATT_PGE6_Pos)                      /**< (TCC_PATT) Pattern Generator 6 Output Enable Mask */
#define TCC_PATT_PGE6(value)                  (TCC_PATT_PGE6_Msk & ((value) << TCC_PATT_PGE6_Pos))
#define TCC_PATT_PGE7_Pos                     _U_(7)                                               /**< (TCC_PATT) Pattern Generator 7 Output Enable Position */
#define TCC_PATT_PGE7_Msk                     (_U_(0x1) << TCC_PATT_PGE7_Pos)                      /**< (TCC_PATT) Pattern Generator 7 Output Enable Mask */
#define TCC_PATT_PGE7(value)                  (TCC_PATT_PGE7_Msk & ((value) << TCC_PATT_PGE7_Pos))
#define TCC_PATT_PGV0_Pos                     _U_(8)                                               /**< (TCC_PATT) Pattern Generator 0 Output Value Position */
#define TCC_PATT_PGV0_Msk                     (_U_(0x1) << TCC_PATT_PGV0_Pos)                      /**< (TCC_PATT) Pattern Generator 0 Output Value Mask */
#define TCC_PATT_PGV0(value)                  (TCC_PATT_PGV0_Msk & ((value) << TCC_PATT_PGV0_Pos))
#define TCC_PATT_PGV1_Pos                     _U_(9)                                               /**< (TCC_PATT) Pattern Generator 1 Output Value Position */
#define TCC_PATT_PGV1_Msk                     (_U_(0x1) << TCC_PATT_PGV1_Pos)                      /**< (TCC_PATT) Pattern Generator 1 Output Value Mask */
#define TCC_PATT_PGV1(value)                  (TCC_PATT_PGV1_Msk & ((value) << TCC_PATT_PGV1_Pos))
#define TCC_PATT_PGV2_Pos                     _U_(10)                                              /**< (TCC_PATT) Pattern Generator 2 Output Value Position */
#define TCC_PATT_PGV2_Msk                     (_U_(0x1) << TCC_PATT_PGV2_Pos)                      /**< (TCC_PATT) Pattern Generator 2 Output Value Mask */
#define TCC_PATT_PGV2(value)                  (TCC_PATT_PGV2_Msk & ((value) << TCC_PATT_PGV2_Pos))
#define TCC_PATT_PGV3_Pos                     _U_(11)                                              /**< (TCC_PATT) Pattern Generator 3 Output Value Position */
#define TCC_PATT_PGV3_Msk                     (_U_(0x1) << TCC_PATT_PGV3_Pos)                      /**< (TCC_PATT) Pattern Generator 3 Output Value Mask */
#define TCC_PATT_PGV3(value)                  (TCC_PATT_PGV3_Msk & ((value) << TCC_PATT_PGV3_Pos))
#define TCC_PATT_PGV4_Pos                     _U_(12)                                              /**< (TCC_PATT) Pattern Generator 4 Output Value Position */
#define TCC_PATT_PGV4_Msk                     (_U_(0x1) << TCC_PATT_PGV4_Pos)                      /**< (TCC_PATT) Pattern Generator 4 Output Value Mask */
#define TCC_PATT_PGV4(value)                  (TCC_PATT_PGV4_Msk & ((value) << TCC_PATT_PGV4_Pos))
#define TCC_PATT_PGV5_Pos                     _U_(13)                                              /**< (TCC_PATT) Pattern Generator 5 Output Value Position */
#define TCC_PATT_PGV5_Msk                     (_U_(0x1) << TCC_PATT_PGV5_Pos)                      /**< (TCC_PATT) Pattern Generator 5 Output Value Mask */
#define TCC_PATT_PGV5(value)                  (TCC_PATT_PGV5_Msk & ((value) << TCC_PATT_PGV5_Pos))
#define TCC_PATT_PGV6_Pos                     _U_(14)                                              /**< (TCC_PATT) Pattern Generator 6 Output Value Position */
#define TCC_PATT_PGV6_Msk                     (_U_(0x1) << TCC_PATT_PGV6_Pos)                      /**< (TCC_PATT) Pattern Generator 6 Output Value Mask */
#define TCC_PATT_PGV6(value)                  (TCC_PATT_PGV6_Msk & ((value) << TCC_PATT_PGV6_Pos))
#define TCC_PATT_PGV7_Pos                     _U_(15)                                              /**< (TCC_PATT) Pattern Generator 7 Output Value Position */
#define TCC_PATT_PGV7_Msk                     (_U_(0x1) << TCC_PATT_PGV7_Pos)                      /**< (TCC_PATT) Pattern Generator 7 Output Value Mask */
#define TCC_PATT_PGV7(value)                  (TCC_PATT_PGV7_Msk & ((value) << TCC_PATT_PGV7_Pos))
#define TCC_PATT_Msk                          _U_(0xFFFF)                                          /**< (TCC_PATT) Register Mask  */

#define TCC_PATT_PGE_Pos                      _U_(0)                                               /**< (TCC_PATT Position) Pattern Generator x Output Enable */
#define TCC_PATT_PGE_Msk                      (_U_(0xFF) << TCC_PATT_PGE_Pos)                      /**< (TCC_PATT Mask) PGE */
#define TCC_PATT_PGE(value)                   (TCC_PATT_PGE_Msk & ((value) << TCC_PATT_PGE_Pos))   
#define TCC_PATT_PGV_Pos                      _U_(8)                                               /**< (TCC_PATT Position) Pattern Generator 7 Output Value */
#define TCC_PATT_PGV_Msk                      (_U_(0xFF) << TCC_PATT_PGV_Pos)                      /**< (TCC_PATT Mask) PGV */
#define TCC_PATT_PGV(value)                   (TCC_PATT_PGV_Msk & ((value) << TCC_PATT_PGV_Pos))   

/* -------- TCC_WAVE : (TCC Offset: 0x3C) (R/W 32) Waveform Control -------- */
#define TCC_WAVE_RESETVALUE                   _U_(0x00)                                            /**<  (TCC_WAVE) Waveform Control  Reset Value */

#define TCC_WAVE_WAVEGEN_Pos                  _U_(0)                                               /**< (TCC_WAVE) Waveform Generation Position */
#define TCC_WAVE_WAVEGEN_Msk                  (_U_(0x7) << TCC_WAVE_WAVEGEN_Pos)                   /**< (TCC_WAVE) Waveform Generation Mask */
#define TCC_WAVE_WAVEGEN(value)               (TCC_WAVE_WAVEGEN_Msk & ((value) << TCC_WAVE_WAVEGEN_Pos))
#define   TCC_WAVE_WAVEGEN_NFRQ_Val           _U_(0x0)                                             /**< (TCC_WAVE) Normal frequency  */
#define   TCC_WAVE_WAVEGEN_MFRQ_Val           _U_(0x1)                                             /**< (TCC_WAVE) Match frequency  */
#define   TCC_WAVE_WAVEGEN_NPWM_Val           _U_(0x2)                                             /**< (TCC_WAVE) Normal PWM  */
#define   TCC_WAVE_WAVEGEN_DSCRITICAL_Val     _U_(0x4)                                             /**< (TCC_WAVE) Dual-slope critical  */
#define   TCC_WAVE_WAVEGEN_DSBOTTOM_Val       _U_(0x5)                                             /**< (TCC_WAVE) Dual-slope with interrupt/event condition when COUNT reaches ZERO  */
#define   TCC_WAVE_WAVEGEN_DSBOTH_Val         _U_(0x6)                                             /**< (TCC_WAVE) Dual-slope with interrupt/event condition when COUNT reaches ZERO or TOP  */
#define   TCC_WAVE_WAVEGEN_DSTOP_Val          _U_(0x7)                                             /**< (TCC_WAVE) Dual-slope with interrupt/event condition when COUNT reaches TOP  */
#define TCC_WAVE_WAVEGEN_NFRQ                 (TCC_WAVE_WAVEGEN_NFRQ_Val << TCC_WAVE_WAVEGEN_Pos)  /**< (TCC_WAVE) Normal frequency Position  */
#define TCC_WAVE_WAVEGEN_MFRQ                 (TCC_WAVE_WAVEGEN_MFRQ_Val << TCC_WAVE_WAVEGEN_Pos)  /**< (TCC_WAVE) Match frequency Position  */
#define TCC_WAVE_WAVEGEN_NPWM                 (TCC_WAVE_WAVEGEN_NPWM_Val << TCC_WAVE_WAVEGEN_Pos)  /**< (TCC_WAVE) Normal PWM Position  */
#define TCC_WAVE_WAVEGEN_DSCRITICAL           (TCC_WAVE_WAVEGEN_DSCRITICAL_Val << TCC_WAVE_WAVEGEN_Pos) /**< (TCC_WAVE) Dual-slope critical Position  */
#define TCC_WAVE_WAVEGEN_DSBOTTOM             (TCC_WAVE_WAVEGEN_DSBOTTOM_Val << TCC_WAVE_WAVEGEN_Pos) /**< (TCC_WAVE) Dual-slope with interrupt/event condition when COUNT reaches ZERO Position  */
#define TCC_WAVE_WAVEGEN_DSBOTH               (TCC_WAVE_WAVEGEN_DSBOTH_Val << TCC_WAVE_WAVEGEN_Pos) /**< (TCC_WAVE) Dual-slope with interrupt/event condition when COUNT reaches ZERO or TOP Position  */
#define TCC_WAVE_WAVEGEN_DSTOP                (TCC_WAVE_WAVEGEN_DSTOP_Val << TCC_WAVE_WAVEGEN_Pos) /**< (TCC_WAVE) Dual-slope with interrupt/event condition when COUNT reaches TOP Position  */
#define TCC_WAVE_RAMP_Pos                     _U_(4)                                               /**< (TCC_WAVE) Ramp Mode Position */
#define TCC_WAVE_RAMP_Msk                     (_U_(0x3) << TCC_WAVE_RAMP_Pos)                      /**< (TCC_WAVE) Ramp Mode Mask */
#define TCC_WAVE_RAMP(value)                  (TCC_WAVE_RAMP_Msk & ((value) << TCC_WAVE_RAMP_Pos))
#define   TCC_WAVE_RAMP_RAMP1_Val             _U_(0x0)                                             /**< (TCC_WAVE) RAMP1 operation  */
#define   TCC_WAVE_RAMP_RAMP2A_Val            _U_(0x1)                                             /**< (TCC_WAVE) Alternative RAMP2 operation  */
#define   TCC_WAVE_RAMP_RAMP2_Val             _U_(0x2)                                             /**< (TCC_WAVE) RAMP2 operation  */
#define   TCC_WAVE_RAMP_RAMP2C_Val            _U_(0x3)                                             /**< (TCC_WAVE) Critical RAMP2 operation  */
#define TCC_WAVE_RAMP_RAMP1                   (TCC_WAVE_RAMP_RAMP1_Val << TCC_WAVE_RAMP_Pos)       /**< (TCC_WAVE) RAMP1 operation Position  */
#define TCC_WAVE_RAMP_RAMP2A                  (TCC_WAVE_RAMP_RAMP2A_Val << TCC_WAVE_RAMP_Pos)      /**< (TCC_WAVE) Alternative RAMP2 operation Position  */
#define TCC_WAVE_RAMP_RAMP2                   (TCC_WAVE_RAMP_RAMP2_Val << TCC_WAVE_RAMP_Pos)       /**< (TCC_WAVE) RAMP2 operation Position  */
#define TCC_WAVE_RAMP_RAMP2C                  (TCC_WAVE_RAMP_RAMP2C_Val << TCC_WAVE_RAMP_Pos)      /**< (TCC_WAVE) Critical RAMP2 operation Position  */
#define TCC_WAVE_CIPEREN_Pos                  _U_(7)                                               /**< (TCC_WAVE) Circular period Enable Position */
#define TCC_WAVE_CIPEREN_Msk                  (_U_(0x1) << TCC_WAVE_CIPEREN_Pos)                   /**< (TCC_WAVE) Circular period Enable Mask */
#define TCC_WAVE_CIPEREN(value)               (TCC_WAVE_CIPEREN_Msk & ((value) << TCC_WAVE_CIPEREN_Pos))
#define TCC_WAVE_CICCEN0_Pos                  _U_(8)                                               /**< (TCC_WAVE) Circular Channel 0 Enable Position */
#define TCC_WAVE_CICCEN0_Msk                  (_U_(0x1) << TCC_WAVE_CICCEN0_Pos)                   /**< (TCC_WAVE) Circular Channel 0 Enable Mask */
#define TCC_WAVE_CICCEN0(value)               (TCC_WAVE_CICCEN0_Msk & ((value) << TCC_WAVE_CICCEN0_Pos))
#define TCC_WAVE_CICCEN1_Pos                  _U_(9)                                               /**< (TCC_WAVE) Circular Channel 1 Enable Position */
#define TCC_WAVE_CICCEN1_Msk                  (_U_(0x1) << TCC_WAVE_CICCEN1_Pos)                   /**< (TCC_WAVE) Circular Channel 1 Enable Mask */
#define TCC_WAVE_CICCEN1(value)               (TCC_WAVE_CICCEN1_Msk & ((value) << TCC_WAVE_CICCEN1_Pos))
#define TCC_WAVE_CICCEN2_Pos                  _U_(10)                                              /**< (TCC_WAVE) Circular Channel 2 Enable Position */
#define TCC_WAVE_CICCEN2_Msk                  (_U_(0x1) << TCC_WAVE_CICCEN2_Pos)                   /**< (TCC_WAVE) Circular Channel 2 Enable Mask */
#define TCC_WAVE_CICCEN2(value)               (TCC_WAVE_CICCEN2_Msk & ((value) << TCC_WAVE_CICCEN2_Pos))
#define TCC_WAVE_CICCEN3_Pos                  _U_(11)                                              /**< (TCC_WAVE) Circular Channel 3 Enable Position */
#define TCC_WAVE_CICCEN3_Msk                  (_U_(0x1) << TCC_WAVE_CICCEN3_Pos)                   /**< (TCC_WAVE) Circular Channel 3 Enable Mask */
#define TCC_WAVE_CICCEN3(value)               (TCC_WAVE_CICCEN3_Msk & ((value) << TCC_WAVE_CICCEN3_Pos))
#define TCC_WAVE_POL0_Pos                     _U_(16)                                              /**< (TCC_WAVE) Channel 0 Polarity Position */
#define TCC_WAVE_POL0_Msk                     (_U_(0x1) << TCC_WAVE_POL0_Pos)                      /**< (TCC_WAVE) Channel 0 Polarity Mask */
#define TCC_WAVE_POL0(value)                  (TCC_WAVE_POL0_Msk & ((value) << TCC_WAVE_POL0_Pos))
#define TCC_WAVE_POL1_Pos                     _U_(17)                                              /**< (TCC_WAVE) Channel 1 Polarity Position */
#define TCC_WAVE_POL1_Msk                     (_U_(0x1) << TCC_WAVE_POL1_Pos)                      /**< (TCC_WAVE) Channel 1 Polarity Mask */
#define TCC_WAVE_POL1(value)                  (TCC_WAVE_POL1_Msk & ((value) << TCC_WAVE_POL1_Pos))
#define TCC_WAVE_POL2_Pos                     _U_(18)                                              /**< (TCC_WAVE) Channel 2 Polarity Position */
#define TCC_WAVE_POL2_Msk                     (_U_(0x1) << TCC_WAVE_POL2_Pos)                      /**< (TCC_WAVE) Channel 2 Polarity Mask */
#define TCC_WAVE_POL2(value)                  (TCC_WAVE_POL2_Msk & ((value) << TCC_WAVE_POL2_Pos))
#define TCC_WAVE_POL3_Pos                     _U_(19)                                              /**< (TCC_WAVE) Channel 3 Polarity Position */
#define TCC_WAVE_POL3_Msk                     (_U_(0x1) << TCC_WAVE_POL3_Pos)                      /**< (TCC_WAVE) Channel 3 Polarity Mask */
#define TCC_WAVE_POL3(value)                  (TCC_WAVE_POL3_Msk & ((value) << TCC_WAVE_POL3_Pos))
#define TCC_WAVE_POL4_Pos                     _U_(20)                                              /**< (TCC_WAVE) Channel 4 Polarity Position */
#define TCC_WAVE_POL4_Msk                     (_U_(0x1) << TCC_WAVE_POL4_Pos)                      /**< (TCC_WAVE) Channel 4 Polarity Mask */
#define TCC_WAVE_POL4(value)                  (TCC_WAVE_POL4_Msk & ((value) << TCC_WAVE_POL4_Pos))
#define TCC_WAVE_POL5_Pos                     _U_(21)                                              /**< (TCC_WAVE) Channel 5 Polarity Position */
#define TCC_WAVE_POL5_Msk                     (_U_(0x1) << TCC_WAVE_POL5_Pos)                      /**< (TCC_WAVE) Channel 5 Polarity Mask */
#define TCC_WAVE_POL5(value)                  (TCC_WAVE_POL5_Msk & ((value) << TCC_WAVE_POL5_Pos))
#define TCC_WAVE_SWAP0_Pos                    _U_(24)                                              /**< (TCC_WAVE) Swap DTI Output Pair 0 Position */
#define TCC_WAVE_SWAP0_Msk                    (_U_(0x1) << TCC_WAVE_SWAP0_Pos)                     /**< (TCC_WAVE) Swap DTI Output Pair 0 Mask */
#define TCC_WAVE_SWAP0(value)                 (TCC_WAVE_SWAP0_Msk & ((value) << TCC_WAVE_SWAP0_Pos))
#define TCC_WAVE_SWAP1_Pos                    _U_(25)                                              /**< (TCC_WAVE) Swap DTI Output Pair 1 Position */
#define TCC_WAVE_SWAP1_Msk                    (_U_(0x1) << TCC_WAVE_SWAP1_Pos)                     /**< (TCC_WAVE) Swap DTI Output Pair 1 Mask */
#define TCC_WAVE_SWAP1(value)                 (TCC_WAVE_SWAP1_Msk & ((value) << TCC_WAVE_SWAP1_Pos))
#define TCC_WAVE_SWAP2_Pos                    _U_(26)                                              /**< (TCC_WAVE) Swap DTI Output Pair 2 Position */
#define TCC_WAVE_SWAP2_Msk                    (_U_(0x1) << TCC_WAVE_SWAP2_Pos)                     /**< (TCC_WAVE) Swap DTI Output Pair 2 Mask */
#define TCC_WAVE_SWAP2(value)                 (TCC_WAVE_SWAP2_Msk & ((value) << TCC_WAVE_SWAP2_Pos))
#define TCC_WAVE_SWAP3_Pos                    _U_(27)                                              /**< (TCC_WAVE) Swap DTI Output Pair 3 Position */
#define TCC_WAVE_SWAP3_Msk                    (_U_(0x1) << TCC_WAVE_SWAP3_Pos)                     /**< (TCC_WAVE) Swap DTI Output Pair 3 Mask */
#define TCC_WAVE_SWAP3(value)                 (TCC_WAVE_SWAP3_Msk & ((value) << TCC_WAVE_SWAP3_Pos))
#define TCC_WAVE_Msk                          _U_(0x0F3F0FB7)                                      /**< (TCC_WAVE) Register Mask  */

#define TCC_WAVE_CICCEN_Pos                   _U_(8)                                               /**< (TCC_WAVE Position) Circular Channel x Enable */
#define TCC_WAVE_CICCEN_Msk                   (_U_(0xF) << TCC_WAVE_CICCEN_Pos)                    /**< (TCC_WAVE Mask) CICCEN */
#define TCC_WAVE_CICCEN(value)                (TCC_WAVE_CICCEN_Msk & ((value) << TCC_WAVE_CICCEN_Pos)) 
#define TCC_WAVE_POL_Pos                      _U_(16)                                              /**< (TCC_WAVE Position) Channel x Polarity */
#define TCC_WAVE_POL_Msk                      (_U_(0x3F) << TCC_WAVE_POL_Pos)                      /**< (TCC_WAVE Mask) POL */
#define TCC_WAVE_POL(value)                   (TCC_WAVE_POL_Msk & ((value) << TCC_WAVE_POL_Pos))   
#define TCC_WAVE_SWAP_Pos                     _U_(24)                                              /**< (TCC_WAVE Position) Swap DTI Output Pair 3 */
#define TCC_WAVE_SWAP_Msk                     (_U_(0xF) << TCC_WAVE_SWAP_Pos)                      /**< (TCC_WAVE Mask) SWAP */
#define TCC_WAVE_SWAP(value)                  (TCC_WAVE_SWAP_Msk & ((value) << TCC_WAVE_SWAP_Pos)) 

/* -------- TCC_PER : (TCC Offset: 0x40) (R/W 32) Period -------- */
#define TCC_PER_RESETVALUE                    _U_(0xFFFFFFFF)                                      /**<  (TCC_PER) Period  Reset Value */

#define TCC_PER_PER_Pos                       _U_(0)                                               /**< (TCC_PER) Period Value Position */
#define TCC_PER_PER_Msk                       (_U_(0xFFFFFF) << TCC_PER_PER_Pos)                   /**< (TCC_PER) Period Value Mask */
#define TCC_PER_PER(value)                    (TCC_PER_PER_Msk & ((value) << TCC_PER_PER_Pos))    
#define TCC_PER_Msk                           _U_(0x00FFFFFF)                                      /**< (TCC_PER) Register Mask  */

/* DITH4 mode */
#define TCC_PER_DITH4_DITHER_Pos              _U_(0)                                               /**< (TCC_PER) Dithering Cycle Number Position */
#define TCC_PER_DITH4_DITHER_Msk              (_U_(0xF) << TCC_PER_DITH4_DITHER_Pos)               /**< (TCC_PER) Dithering Cycle Number Mask */
#define TCC_PER_DITH4_DITHER(value)           (TCC_PER_DITH4_DITHER_Msk & ((value) << TCC_PER_DITH4_DITHER_Pos))
#define TCC_PER_DITH4_PER_Pos                 _U_(4)                                               /**< (TCC_PER) Period Value Position */
#define TCC_PER_DITH4_PER_Msk                 (_U_(0xFFFFF) << TCC_PER_DITH4_PER_Pos)              /**< (TCC_PER) Period Value Mask */
#define TCC_PER_DITH4_PER(value)              (TCC_PER_DITH4_PER_Msk & ((value) << TCC_PER_DITH4_PER_Pos))
#define TCC_PER_DITH4_Msk                     _U_(0x00FFFFFF)                                       /**< (TCC_PER_DITH4) Register Mask  */

/* DITH5 mode */
#define TCC_PER_DITH5_DITHER_Pos              _U_(0)                                               /**< (TCC_PER) Dithering Cycle Number Position */
#define TCC_PER_DITH5_DITHER_Msk              (_U_(0x1F) << TCC_PER_DITH5_DITHER_Pos)              /**< (TCC_PER) Dithering Cycle Number Mask */
#define TCC_PER_DITH5_DITHER(value)           (TCC_PER_DITH5_DITHER_Msk & ((value) << TCC_PER_DITH5_DITHER_Pos))
#define TCC_PER_DITH5_PER_Pos                 _U_(5)                                               /**< (TCC_PER) Period Value Position */
#define TCC_PER_DITH5_PER_Msk                 (_U_(0x7FFFF) << TCC_PER_DITH5_PER_Pos)              /**< (TCC_PER) Period Value Mask */
#define TCC_PER_DITH5_PER(value)              (TCC_PER_DITH5_PER_Msk & ((value) << TCC_PER_DITH5_PER_Pos))
#define TCC_PER_DITH5_Msk                     _U_(0x00FFFFFF)                                       /**< (TCC_PER_DITH5) Register Mask  */

/* DITH6 mode */
#define TCC_PER_DITH6_DITHER_Pos              _U_(0)                                               /**< (TCC_PER) Dithering Cycle Number Position */
#define TCC_PER_DITH6_DITHER_Msk              (_U_(0x3F) << TCC_PER_DITH6_DITHER_Pos)              /**< (TCC_PER) Dithering Cycle Number Mask */
#define TCC_PER_DITH6_DITHER(value)           (TCC_PER_DITH6_DITHER_Msk & ((value) << TCC_PER_DITH6_DITHER_Pos))
#define TCC_PER_DITH6_PER_Pos                 _U_(6)                                               /**< (TCC_PER) Period Value Position */
#define TCC_PER_DITH6_PER_Msk                 (_U_(0x3FFFF) << TCC_PER_DITH6_PER_Pos)              /**< (TCC_PER) Period Value Mask */
#define TCC_PER_DITH6_PER(value)              (TCC_PER_DITH6_PER_Msk & ((value) << TCC_PER_DITH6_PER_Pos))
#define TCC_PER_DITH6_Msk                     _U_(0x00FFFFFF)                                       /**< (TCC_PER_DITH6) Register Mask  */


/* -------- TCC_CC : (TCC Offset: 0x44) (R/W 32) Compare and Capture -------- */
#define TCC_CC_RESETVALUE                     _U_(0x00)                                            /**<  (TCC_CC) Compare and Capture  Reset Value */

#define TCC_CC_CC_Pos                         _U_(0)                                               /**< (TCC_CC) Channel Compare/Capture Value Position */
#define TCC_CC_CC_Msk                         (_U_(0xFFFFFF) << TCC_CC_CC_Pos)                     /**< (TCC_CC) Channel Compare/Capture Value Mask */
#define TCC_CC_CC(value)                      (TCC_CC_CC_Msk & ((value) << TCC_CC_CC_Pos))        
#define TCC_CC_Msk                            _U_(0x00FFFFFF)                                      /**< (TCC_CC) Register Mask  */

/* DITH4 mode */
#define TCC_CC_DITH4_DITHER_Pos               _U_(0)                                               /**< (TCC_CC) Dithering Cycle Number Position */
#define TCC_CC_DITH4_DITHER_Msk               (_U_(0xF) << TCC_CC_DITH4_DITHER_Pos)                /**< (TCC_CC) Dithering Cycle Number Mask */
#define TCC_CC_DITH4_DITHER(value)            (TCC_CC_DITH4_DITHER_Msk & ((value) << TCC_CC_DITH4_DITHER_Pos))
#define TCC_CC_DITH4_CC_Pos                   _U_(4)                                               /**< (TCC_CC) Channel Compare/Capture Value Position */
#define TCC_CC_DITH4_CC_Msk                   (_U_(0xFFFFF) << TCC_CC_DITH4_CC_Pos)                /**< (TCC_CC) Channel Compare/Capture Value Mask */
#define TCC_CC_DITH4_CC(value)                (TCC_CC_DITH4_CC_Msk & ((value) << TCC_CC_DITH4_CC_Pos))
#define TCC_CC_DITH4_Msk                      _U_(0x00FFFFFF)                                       /**< (TCC_CC_DITH4) Register Mask  */

/* DITH5 mode */
#define TCC_CC_DITH5_DITHER_Pos               _U_(0)                                               /**< (TCC_CC) Dithering Cycle Number Position */
#define TCC_CC_DITH5_DITHER_Msk               (_U_(0x1F) << TCC_CC_DITH5_DITHER_Pos)               /**< (TCC_CC) Dithering Cycle Number Mask */
#define TCC_CC_DITH5_DITHER(value)            (TCC_CC_DITH5_DITHER_Msk & ((value) << TCC_CC_DITH5_DITHER_Pos))
#define TCC_CC_DITH5_CC_Pos                   _U_(5)                                               /**< (TCC_CC) Channel Compare/Capture Value Position */
#define TCC_CC_DITH5_CC_Msk                   (_U_(0x7FFFF) << TCC_CC_DITH5_CC_Pos)                /**< (TCC_CC) Channel Compare/Capture Value Mask */
#define TCC_CC_DITH5_CC(value)                (TCC_CC_DITH5_CC_Msk & ((value) << TCC_CC_DITH5_CC_Pos))
#define TCC_CC_DITH5_Msk                      _U_(0x00FFFFFF)                                       /**< (TCC_CC_DITH5) Register Mask  */

/* DITH6 mode */
#define TCC_CC_DITH6_DITHER_Pos               _U_(0)                                               /**< (TCC_CC) Dithering Cycle Number Position */
#define TCC_CC_DITH6_DITHER_Msk               (_U_(0x3F) << TCC_CC_DITH6_DITHER_Pos)               /**< (TCC_CC) Dithering Cycle Number Mask */
#define TCC_CC_DITH6_DITHER(value)            (TCC_CC_DITH6_DITHER_Msk & ((value) << TCC_CC_DITH6_DITHER_Pos))
#define TCC_CC_DITH6_CC_Pos                   _U_(6)                                               /**< (TCC_CC) Channel Compare/Capture Value Position */
#define TCC_CC_DITH6_CC_Msk                   (_U_(0x3FFFF) << TCC_CC_DITH6_CC_Pos)                /**< (TCC_CC) Channel Compare/Capture Value Mask */
#define TCC_CC_DITH6_CC(value)                (TCC_CC_DITH6_CC_Msk & ((value) << TCC_CC_DITH6_CC_Pos))
#define TCC_CC_DITH6_Msk                      _U_(0x00FFFFFF)                                       /**< (TCC_CC_DITH6) Register Mask  */


/* -------- TCC_PATTBUF : (TCC Offset: 0x64) (R/W 16) Pattern Buffer -------- */
#define TCC_PATTBUF_RESETVALUE                _U_(0x00)                                            /**<  (TCC_PATTBUF) Pattern Buffer  Reset Value */

#define TCC_PATTBUF_PGEB0_Pos                 _U_(0)                                               /**< (TCC_PATTBUF) Pattern Generator 0 Output Enable Buffer Position */
#define TCC_PATTBUF_PGEB0_Msk                 (_U_(0x1) << TCC_PATTBUF_PGEB0_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 0 Output Enable Buffer Mask */
#define TCC_PATTBUF_PGEB0(value)              (TCC_PATTBUF_PGEB0_Msk & ((value) << TCC_PATTBUF_PGEB0_Pos))
#define TCC_PATTBUF_PGEB1_Pos                 _U_(1)                                               /**< (TCC_PATTBUF) Pattern Generator 1 Output Enable Buffer Position */
#define TCC_PATTBUF_PGEB1_Msk                 (_U_(0x1) << TCC_PATTBUF_PGEB1_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 1 Output Enable Buffer Mask */
#define TCC_PATTBUF_PGEB1(value)              (TCC_PATTBUF_PGEB1_Msk & ((value) << TCC_PATTBUF_PGEB1_Pos))
#define TCC_PATTBUF_PGEB2_Pos                 _U_(2)                                               /**< (TCC_PATTBUF) Pattern Generator 2 Output Enable Buffer Position */
#define TCC_PATTBUF_PGEB2_Msk                 (_U_(0x1) << TCC_PATTBUF_PGEB2_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 2 Output Enable Buffer Mask */
#define TCC_PATTBUF_PGEB2(value)              (TCC_PATTBUF_PGEB2_Msk & ((value) << TCC_PATTBUF_PGEB2_Pos))
#define TCC_PATTBUF_PGEB3_Pos                 _U_(3)                                               /**< (TCC_PATTBUF) Pattern Generator 3 Output Enable Buffer Position */
#define TCC_PATTBUF_PGEB3_Msk                 (_U_(0x1) << TCC_PATTBUF_PGEB3_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 3 Output Enable Buffer Mask */
#define TCC_PATTBUF_PGEB3(value)              (TCC_PATTBUF_PGEB3_Msk & ((value) << TCC_PATTBUF_PGEB3_Pos))
#define TCC_PATTBUF_PGEB4_Pos                 _U_(4)                                               /**< (TCC_PATTBUF) Pattern Generator 4 Output Enable Buffer Position */
#define TCC_PATTBUF_PGEB4_Msk                 (_U_(0x1) << TCC_PATTBUF_PGEB4_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 4 Output Enable Buffer Mask */
#define TCC_PATTBUF_PGEB4(value)              (TCC_PATTBUF_PGEB4_Msk & ((value) << TCC_PATTBUF_PGEB4_Pos))
#define TCC_PATTBUF_PGEB5_Pos                 _U_(5)                                               /**< (TCC_PATTBUF) Pattern Generator 5 Output Enable Buffer Position */
#define TCC_PATTBUF_PGEB5_Msk                 (_U_(0x1) << TCC_PATTBUF_PGEB5_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 5 Output Enable Buffer Mask */
#define TCC_PATTBUF_PGEB5(value)              (TCC_PATTBUF_PGEB5_Msk & ((value) << TCC_PATTBUF_PGEB5_Pos))
#define TCC_PATTBUF_PGEB6_Pos                 _U_(6)                                               /**< (TCC_PATTBUF) Pattern Generator 6 Output Enable Buffer Position */
#define TCC_PATTBUF_PGEB6_Msk                 (_U_(0x1) << TCC_PATTBUF_PGEB6_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 6 Output Enable Buffer Mask */
#define TCC_PATTBUF_PGEB6(value)              (TCC_PATTBUF_PGEB6_Msk & ((value) << TCC_PATTBUF_PGEB6_Pos))
#define TCC_PATTBUF_PGEB7_Pos                 _U_(7)                                               /**< (TCC_PATTBUF) Pattern Generator 7 Output Enable Buffer Position */
#define TCC_PATTBUF_PGEB7_Msk                 (_U_(0x1) << TCC_PATTBUF_PGEB7_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 7 Output Enable Buffer Mask */
#define TCC_PATTBUF_PGEB7(value)              (TCC_PATTBUF_PGEB7_Msk & ((value) << TCC_PATTBUF_PGEB7_Pos))
#define TCC_PATTBUF_PGVB0_Pos                 _U_(8)                                               /**< (TCC_PATTBUF) Pattern Generator 0 Output Enable Position */
#define TCC_PATTBUF_PGVB0_Msk                 (_U_(0x1) << TCC_PATTBUF_PGVB0_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 0 Output Enable Mask */
#define TCC_PATTBUF_PGVB0(value)              (TCC_PATTBUF_PGVB0_Msk & ((value) << TCC_PATTBUF_PGVB0_Pos))
#define TCC_PATTBUF_PGVB1_Pos                 _U_(9)                                               /**< (TCC_PATTBUF) Pattern Generator 1 Output Enable Position */
#define TCC_PATTBUF_PGVB1_Msk                 (_U_(0x1) << TCC_PATTBUF_PGVB1_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 1 Output Enable Mask */
#define TCC_PATTBUF_PGVB1(value)              (TCC_PATTBUF_PGVB1_Msk & ((value) << TCC_PATTBUF_PGVB1_Pos))
#define TCC_PATTBUF_PGVB2_Pos                 _U_(10)                                              /**< (TCC_PATTBUF) Pattern Generator 2 Output Enable Position */
#define TCC_PATTBUF_PGVB2_Msk                 (_U_(0x1) << TCC_PATTBUF_PGVB2_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 2 Output Enable Mask */
#define TCC_PATTBUF_PGVB2(value)              (TCC_PATTBUF_PGVB2_Msk & ((value) << TCC_PATTBUF_PGVB2_Pos))
#define TCC_PATTBUF_PGVB3_Pos                 _U_(11)                                              /**< (TCC_PATTBUF) Pattern Generator 3 Output Enable Position */
#define TCC_PATTBUF_PGVB3_Msk                 (_U_(0x1) << TCC_PATTBUF_PGVB3_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 3 Output Enable Mask */
#define TCC_PATTBUF_PGVB3(value)              (TCC_PATTBUF_PGVB3_Msk & ((value) << TCC_PATTBUF_PGVB3_Pos))
#define TCC_PATTBUF_PGVB4_Pos                 _U_(12)                                              /**< (TCC_PATTBUF) Pattern Generator 4 Output Enable Position */
#define TCC_PATTBUF_PGVB4_Msk                 (_U_(0x1) << TCC_PATTBUF_PGVB4_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 4 Output Enable Mask */
#define TCC_PATTBUF_PGVB4(value)              (TCC_PATTBUF_PGVB4_Msk & ((value) << TCC_PATTBUF_PGVB4_Pos))
#define TCC_PATTBUF_PGVB5_Pos                 _U_(13)                                              /**< (TCC_PATTBUF) Pattern Generator 5 Output Enable Position */
#define TCC_PATTBUF_PGVB5_Msk                 (_U_(0x1) << TCC_PATTBUF_PGVB5_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 5 Output Enable Mask */
#define TCC_PATTBUF_PGVB5(value)              (TCC_PATTBUF_PGVB5_Msk & ((value) << TCC_PATTBUF_PGVB5_Pos))
#define TCC_PATTBUF_PGVB6_Pos                 _U_(14)                                              /**< (TCC_PATTBUF) Pattern Generator 6 Output Enable Position */
#define TCC_PATTBUF_PGVB6_Msk                 (_U_(0x1) << TCC_PATTBUF_PGVB6_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 6 Output Enable Mask */
#define TCC_PATTBUF_PGVB6(value)              (TCC_PATTBUF_PGVB6_Msk & ((value) << TCC_PATTBUF_PGVB6_Pos))
#define TCC_PATTBUF_PGVB7_Pos                 _U_(15)                                              /**< (TCC_PATTBUF) Pattern Generator 7 Output Enable Position */
#define TCC_PATTBUF_PGVB7_Msk                 (_U_(0x1) << TCC_PATTBUF_PGVB7_Pos)                  /**< (TCC_PATTBUF) Pattern Generator 7 Output Enable Mask */
#define TCC_PATTBUF_PGVB7(value)              (TCC_PATTBUF_PGVB7_Msk & ((value) << TCC_PATTBUF_PGVB7_Pos))
#define TCC_PATTBUF_Msk                       _U_(0xFFFF)                                          /**< (TCC_PATTBUF) Register Mask  */

#define TCC_PATTBUF_PGEB_Pos                  _U_(0)                                               /**< (TCC_PATTBUF Position) Pattern Generator x Output Enable Buffer */
#define TCC_PATTBUF_PGEB_Msk                  (_U_(0xFF) << TCC_PATTBUF_PGEB_Pos)                  /**< (TCC_PATTBUF Mask) PGEB */
#define TCC_PATTBUF_PGEB(value)               (TCC_PATTBUF_PGEB_Msk & ((value) << TCC_PATTBUF_PGEB_Pos)) 
#define TCC_PATTBUF_PGVB_Pos                  _U_(8)                                               /**< (TCC_PATTBUF Position) Pattern Generator 7 Output Enable */
#define TCC_PATTBUF_PGVB_Msk                  (_U_(0xFF) << TCC_PATTBUF_PGVB_Pos)                  /**< (TCC_PATTBUF Mask) PGVB */
#define TCC_PATTBUF_PGVB(value)               (TCC_PATTBUF_PGVB_Msk & ((value) << TCC_PATTBUF_PGVB_Pos)) 

/* -------- TCC_PERBUF : (TCC Offset: 0x6C) (R/W 32) Period Buffer -------- */
#define TCC_PERBUF_RESETVALUE                 _U_(0xFFFFFFFF)                                      /**<  (TCC_PERBUF) Period Buffer  Reset Value */

#define TCC_PERBUF_PERBUF_Pos                 _U_(0)                                               /**< (TCC_PERBUF) Period Buffer Value Position */
#define TCC_PERBUF_PERBUF_Msk                 (_U_(0xFFFFFF) << TCC_PERBUF_PERBUF_Pos)             /**< (TCC_PERBUF) Period Buffer Value Mask */
#define TCC_PERBUF_PERBUF(value)              (TCC_PERBUF_PERBUF_Msk & ((value) << TCC_PERBUF_PERBUF_Pos))
#define TCC_PERBUF_Msk                        _U_(0x00FFFFFF)                                      /**< (TCC_PERBUF) Register Mask  */

/* DITH4 mode */
#define TCC_PERBUF_DITH4_DITHERBUF_Pos        _U_(0)                                               /**< (TCC_PERBUF) Dithering Buffer Cycle Number Position */
#define TCC_PERBUF_DITH4_DITHERBUF_Msk        (_U_(0xF) << TCC_PERBUF_DITH4_DITHERBUF_Pos)         /**< (TCC_PERBUF) Dithering Buffer Cycle Number Mask */
#define TCC_PERBUF_DITH4_DITHERBUF(value)     (TCC_PERBUF_DITH4_DITHERBUF_Msk & ((value) << TCC_PERBUF_DITH4_DITHERBUF_Pos))
#define TCC_PERBUF_DITH4_PERBUF_Pos           _U_(4)                                               /**< (TCC_PERBUF) Period Buffer Value Position */
#define TCC_PERBUF_DITH4_PERBUF_Msk           (_U_(0xFFFFF) << TCC_PERBUF_DITH4_PERBUF_Pos)        /**< (TCC_PERBUF) Period Buffer Value Mask */
#define TCC_PERBUF_DITH4_PERBUF(value)        (TCC_PERBUF_DITH4_PERBUF_Msk & ((value) << TCC_PERBUF_DITH4_PERBUF_Pos))
#define TCC_PERBUF_DITH4_Msk                  _U_(0x00FFFFFF)                                       /**< (TCC_PERBUF_DITH4) Register Mask  */

/* DITH5 mode */
#define TCC_PERBUF_DITH5_DITHERBUF_Pos        _U_(0)                                               /**< (TCC_PERBUF) Dithering Buffer Cycle Number Position */
#define TCC_PERBUF_DITH5_DITHERBUF_Msk        (_U_(0x1F) << TCC_PERBUF_DITH5_DITHERBUF_Pos)        /**< (TCC_PERBUF) Dithering Buffer Cycle Number Mask */
#define TCC_PERBUF_DITH5_DITHERBUF(value)     (TCC_PERBUF_DITH5_DITHERBUF_Msk & ((value) << TCC_PERBUF_DITH5_DITHERBUF_Pos))
#define TCC_PERBUF_DITH5_PERBUF_Pos           _U_(5)                                               /**< (TCC_PERBUF) Period Buffer Value Position */
#define TCC_PERBUF_DITH5_PERBUF_Msk           (_U_(0x7FFFF) << TCC_PERBUF_DITH5_PERBUF_Pos)        /**< (TCC_PERBUF) Period Buffer Value Mask */
#define TCC_PERBUF_DITH5_PERBUF(value)        (TCC_PERBUF_DITH5_PERBUF_Msk & ((value) << TCC_PERBUF_DITH5_PERBUF_Pos))
#define TCC_PERBUF_DITH5_Msk                  _U_(0x00FFFFFF)                                       /**< (TCC_PERBUF_DITH5) Register Mask  */

/* DITH6 mode */
#define TCC_PERBUF_DITH6_DITHERBUF_Pos        _U_(0)                                               /**< (TCC_PERBUF) Dithering Buffer Cycle Number Position */
#define TCC_PERBUF_DITH6_DITHERBUF_Msk        (_U_(0x3F) << TCC_PERBUF_DITH6_DITHERBUF_Pos)        /**< (TCC_PERBUF) Dithering Buffer Cycle Number Mask */
#define TCC_PERBUF_DITH6_DITHERBUF(value)     (TCC_PERBUF_DITH6_DITHERBUF_Msk & ((value) << TCC_PERBUF_DITH6_DITHERBUF_Pos))
#define TCC_PERBUF_DITH6_PERBUF_Pos           _U_(6)                                               /**< (TCC_PERBUF) Period Buffer Value Position */
#define TCC_PERBUF_DITH6_PERBUF_Msk           (_U_(0x3FFFF) << TCC_PERBUF_DITH6_PERBUF_Pos)        /**< (TCC_PERBUF) Period Buffer Value Mask */
#define TCC_PERBUF_DITH6_PERBUF(value)        (TCC_PERBUF_DITH6_PERBUF_Msk & ((value) << TCC_PERBUF_DITH6_PERBUF_Pos))
#define TCC_PERBUF_DITH6_Msk                  _U_(0x00FFFFFF)                                       /**< (TCC_PERBUF_DITH6) Register Mask  */


/* -------- TCC_CCBUF : (TCC Offset: 0x70) (R/W 32) Compare and Capture Buffer -------- */
#define TCC_CCBUF_RESETVALUE                  _U_(0x00)                                            /**<  (TCC_CCBUF) Compare and Capture Buffer  Reset Value */

#define TCC_CCBUF_CCBUF_Pos                   _U_(0)                                               /**< (TCC_CCBUF) Channel Compare/Capture Buffer Value Position */
#define TCC_CCBUF_CCBUF_Msk                   (_U_(0xFFFFFF) << TCC_CCBUF_CCBUF_Pos)               /**< (TCC_CCBUF) Channel Compare/Capture Buffer Value Mask */
#define TCC_CCBUF_CCBUF(value)                (TCC_CCBUF_CCBUF_Msk & ((value) << TCC_CCBUF_CCBUF_Pos))
#define TCC_CCBUF_Msk                         _U_(0x00FFFFFF)                                      /**< (TCC_CCBUF) Register Mask  */

/* DITH4 mode */
#define TCC_CCBUF_DITH4_CCBUF_Pos             _U_(0)                                               /**< (TCC_CCBUF) Channel Compare/Capture Buffer Value Position */
#define TCC_CCBUF_DITH4_CCBUF_Msk             (_U_(0xF) << TCC_CCBUF_DITH4_CCBUF_Pos)              /**< (TCC_CCBUF) Channel Compare/Capture Buffer Value Mask */
#define TCC_CCBUF_DITH4_CCBUF(value)          (TCC_CCBUF_DITH4_CCBUF_Msk & ((value) << TCC_CCBUF_DITH4_CCBUF_Pos))
#define TCC_CCBUF_DITH4_DITHERBUF_Pos         _U_(4)                                               /**< (TCC_CCBUF) Dithering Buffer Cycle Number Position */
#define TCC_CCBUF_DITH4_DITHERBUF_Msk         (_U_(0xFFFFF) << TCC_CCBUF_DITH4_DITHERBUF_Pos)      /**< (TCC_CCBUF) Dithering Buffer Cycle Number Mask */
#define TCC_CCBUF_DITH4_DITHERBUF(value)      (TCC_CCBUF_DITH4_DITHERBUF_Msk & ((value) << TCC_CCBUF_DITH4_DITHERBUF_Pos))
#define TCC_CCBUF_DITH4_Msk                   _U_(0x00FFFFFF)                                       /**< (TCC_CCBUF_DITH4) Register Mask  */

/* DITH5 mode */
#define TCC_CCBUF_DITH5_DITHERBUF_Pos         _U_(0)                                               /**< (TCC_CCBUF) Dithering Buffer Cycle Number Position */
#define TCC_CCBUF_DITH5_DITHERBUF_Msk         (_U_(0x1F) << TCC_CCBUF_DITH5_DITHERBUF_Pos)         /**< (TCC_CCBUF) Dithering Buffer Cycle Number Mask */
#define TCC_CCBUF_DITH5_DITHERBUF(value)      (TCC_CCBUF_DITH5_DITHERBUF_Msk & ((value) << TCC_CCBUF_DITH5_DITHERBUF_Pos))
#define TCC_CCBUF_DITH5_CCBUF_Pos             _U_(5)                                               /**< (TCC_CCBUF) Channel Compare/Capture Buffer Value Position */
#define TCC_CCBUF_DITH5_CCBUF_Msk             (_U_(0x7FFFF) << TCC_CCBUF_DITH5_CCBUF_Pos)          /**< (TCC_CCBUF) Channel Compare/Capture Buffer Value Mask */
#define TCC_CCBUF_DITH5_CCBUF(value)          (TCC_CCBUF_DITH5_CCBUF_Msk & ((value) << TCC_CCBUF_DITH5_CCBUF_Pos))
#define TCC_CCBUF_DITH5_Msk                   _U_(0x00FFFFFF)                                       /**< (TCC_CCBUF_DITH5) Register Mask  */

/* DITH6 mode */
#define TCC_CCBUF_DITH6_DITHERBUF_Pos         _U_(0)                                               /**< (TCC_CCBUF) Dithering Buffer Cycle Number Position */
#define TCC_CCBUF_DITH6_DITHERBUF_Msk         (_U_(0x3F) << TCC_CCBUF_DITH6_DITHERBUF_Pos)         /**< (TCC_CCBUF) Dithering Buffer Cycle Number Mask */
#define TCC_CCBUF_DITH6_DITHERBUF(value)      (TCC_CCBUF_DITH6_DITHERBUF_Msk & ((value) << TCC_CCBUF_DITH6_DITHERBUF_Pos))
#define TCC_CCBUF_DITH6_CCBUF_Pos             _U_(6)                                               /**< (TCC_CCBUF) Channel Compare/Capture Buffer Value Position */
#define TCC_CCBUF_DITH6_CCBUF_Msk             (_U_(0x3FFFF) << TCC_CCBUF_DITH6_CCBUF_Pos)          /**< (TCC_CCBUF) Channel Compare/Capture Buffer Value Mask */
#define TCC_CCBUF_DITH6_CCBUF(value)          (TCC_CCBUF_DITH6_CCBUF_Msk & ((value) << TCC_CCBUF_DITH6_CCBUF_Pos))
#define TCC_CCBUF_DITH6_Msk                   _U_(0x00FFFFFF)                                       /**< (TCC_CCBUF_DITH6) Register Mask  */


/** \brief TCC register offsets definitions */
#define TCC_CTRLA_REG_OFST             (0x00)              /**< (TCC_CTRLA) Control A Offset */
#define TCC_CTRLBCLR_REG_OFST          (0x04)              /**< (TCC_CTRLBCLR) Control B Clear Offset */
#define TCC_CTRLBSET_REG_OFST          (0x05)              /**< (TCC_CTRLBSET) Control B Set Offset */
#define TCC_SYNCBUSY_REG_OFST          (0x08)              /**< (TCC_SYNCBUSY) Synchronization Busy Offset */
#define TCC_FCTRLA_REG_OFST            (0x0C)              /**< (TCC_FCTRLA) Recoverable Fault A Configuration Offset */
#define TCC_FCTRLB_REG_OFST            (0x10)              /**< (TCC_FCTRLB) Recoverable Fault B Configuration Offset */
#define TCC_WEXCTRL_REG_OFST           (0x14)              /**< (TCC_WEXCTRL) Waveform Extension Configuration Offset */
#define TCC_DRVCTRL_REG_OFST           (0x18)              /**< (TCC_DRVCTRL) Driver Control Offset */
#define TCC_DBGCTRL_REG_OFST           (0x1E)              /**< (TCC_DBGCTRL) Debug Control Offset */
#define TCC_EVCTRL_REG_OFST            (0x20)              /**< (TCC_EVCTRL) Event Control Offset */
#define TCC_INTENCLR_REG_OFST          (0x24)              /**< (TCC_INTENCLR) Interrupt Enable Clear Offset */
#define TCC_INTENSET_REG_OFST          (0x28)              /**< (TCC_INTENSET) Interrupt Enable Set Offset */
#define TCC_INTFLAG_REG_OFST           (0x2C)              /**< (TCC_INTFLAG) Interrupt Flag Status and Clear Offset */
#define TCC_STATUS_REG_OFST            (0x30)              /**< (TCC_STATUS) Status Offset */
#define TCC_COUNT_REG_OFST             (0x34)              /**< (TCC_COUNT) Count Offset */
#define TCC_PATT_REG_OFST              (0x38)              /**< (TCC_PATT) Pattern Offset */
#define TCC_WAVE_REG_OFST              (0x3C)              /**< (TCC_WAVE) Waveform Control Offset */
#define TCC_PER_REG_OFST               (0x40)              /**< (TCC_PER) Period Offset */
#define TCC_CC_REG_OFST                (0x44)              /**< (TCC_CC) Compare and Capture Offset */
#define TCC_PATTBUF_REG_OFST           (0x64)              /**< (TCC_PATTBUF) Pattern Buffer Offset */
#define TCC_PERBUF_REG_OFST            (0x6C)              /**< (TCC_PERBUF) Period Buffer Offset */
#define TCC_CCBUF_REG_OFST             (0x70)              /**< (TCC_CCBUF) Compare and Capture Buffer Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief TCC register API structure */
typedef struct
{  /* Timer Counter Control */
  __IO  uint32_t                       TCC_CTRLA;          /**< Offset: 0x00 (R/W  32) Control A */
  __IO  uint8_t                        TCC_CTRLBCLR;       /**< Offset: 0x04 (R/W  8) Control B Clear */
  __IO  uint8_t                        TCC_CTRLBSET;       /**< Offset: 0x05 (R/W  8) Control B Set */
  __I   uint8_t                        Reserved1[0x02];
  __I   uint32_t                       TCC_SYNCBUSY;       /**< Offset: 0x08 (R/   32) Synchronization Busy */
  __IO  uint32_t                       TCC_FCTRLA;         /**< Offset: 0x0C (R/W  32) Recoverable Fault A Configuration */
  __IO  uint32_t                       TCC_FCTRLB;         /**< Offset: 0x10 (R/W  32) Recoverable Fault B Configuration */
  __IO  uint32_t                       TCC_WEXCTRL;        /**< Offset: 0x14 (R/W  32) Waveform Extension Configuration */
  __IO  uint32_t                       TCC_DRVCTRL;        /**< Offset: 0x18 (R/W  32) Driver Control */
  __I   uint8_t                        Reserved2[0x02];
  __IO  uint8_t                        TCC_DBGCTRL;        /**< Offset: 0x1E (R/W  8) Debug Control */
  __I   uint8_t                        Reserved3[0x01];
  __IO  uint32_t                       TCC_EVCTRL;         /**< Offset: 0x20 (R/W  32) Event Control */
  __IO  uint32_t                       TCC_INTENCLR;       /**< Offset: 0x24 (R/W  32) Interrupt Enable Clear */
  __IO  uint32_t                       TCC_INTENSET;       /**< Offset: 0x28 (R/W  32) Interrupt Enable Set */
  __IO  uint32_t                       TCC_INTFLAG;        /**< Offset: 0x2C (R/W  32) Interrupt Flag Status and Clear */
  __IO  uint32_t                       TCC_STATUS;         /**< Offset: 0x30 (R/W  32) Status */
  __IO  uint32_t                       TCC_COUNT;          /**< Offset: 0x34 (R/W  32) Count */
  __IO  uint16_t                       TCC_PATT;           /**< Offset: 0x38 (R/W  16) Pattern */
  __I   uint8_t                        Reserved4[0x02];
  __IO  uint32_t                       TCC_WAVE;           /**< Offset: 0x3C (R/W  32) Waveform Control */
  __IO  uint32_t                       TCC_PER;            /**< Offset: 0x40 (R/W  32) Period */
  __IO  uint32_t                       TCC_CC[6];          /**< Offset: 0x44 (R/W  32) Compare and Capture */
  __I   uint8_t                        Reserved5[0x08];
  __IO  uint16_t                       TCC_PATTBUF;        /**< Offset: 0x64 (R/W  16) Pattern Buffer */
  __I   uint8_t                        Reserved6[0x06];
  __IO  uint32_t                       TCC_PERBUF;         /**< Offset: 0x6C (R/W  32) Period Buffer */
  __IO  uint32_t                       TCC_CCBUF[6];       /**< Offset: 0x70 (R/W  32) Compare and Capture Buffer */
} tcc_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_TCC_COMPONENT_H_ */
