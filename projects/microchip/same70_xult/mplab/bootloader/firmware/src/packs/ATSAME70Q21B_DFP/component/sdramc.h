/**
 * \brief Component description for SDRAMC
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
#ifndef _SAME70_SDRAMC_COMPONENT_H_
#define _SAME70_SDRAMC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR SDRAMC                                       */
/* ************************************************************************** */

/* -------- SDRAMC_MR : (SDRAMC Offset: 0x00) (R/W 32) SDRAMC Mode Register -------- */
#define SDRAMC_MR_MODE_Pos                    _U_(0)                                               /**< (SDRAMC_MR) SDRAMC Command Mode Position */
#define SDRAMC_MR_MODE_Msk                    (_U_(0x7) << SDRAMC_MR_MODE_Pos)                     /**< (SDRAMC_MR) SDRAMC Command Mode Mask */
#define SDRAMC_MR_MODE(value)                 (SDRAMC_MR_MODE_Msk & ((value) << SDRAMC_MR_MODE_Pos))
#define   SDRAMC_MR_MODE_NORMAL_Val           _U_(0x0)                                             /**< (SDRAMC_MR) Normal mode. Any access to the SDRAM is decoded normally. To activate this mode, the command must be followed by a write to the SDRAM.  */
#define   SDRAMC_MR_MODE_NOP_Val              _U_(0x1)                                             /**< (SDRAMC_MR) The SDRAMC issues a NOP command when the SDRAM device is accessed regardless of the cycle. To activate this mode, the command must be followed by a write to the SDRAM.  */
#define   SDRAMC_MR_MODE_ALLBANKS_PRECHARGE_Val _U_(0x2)                                             /**< (SDRAMC_MR) The SDRAMC issues an "All Banks Precharge" command when the SDRAM device is accessed regardless of the cycle. To activate this mode, the command must be followed by a write to the SDRAM.  */
#define   SDRAMC_MR_MODE_LOAD_MODEREG_Val     _U_(0x3)                                             /**< (SDRAMC_MR) The SDRAMC issues a "Load Mode Register" command when the SDRAM device is accessed regardless of the cycle. To activate this mode, the command must be followed by a write to the SDRAM.  */
#define   SDRAMC_MR_MODE_AUTO_REFRESH_Val     _U_(0x4)                                             /**< (SDRAMC_MR) The SDRAMC issues an "Auto-Refresh" Command when the SDRAM device is accessed regardless of the cycle. Previously, an "All Banks Precharge" command must be issued. To activate this mode, the command must be followed by a write to the SDRAM.  */
#define   SDRAMC_MR_MODE_EXT_LOAD_MODEREG_Val _U_(0x5)                                             /**< (SDRAMC_MR) The SDRAMC issues an "Extended Load Mode Register" command when the SDRAM device is accessed regardless of the cycle. To activate this mode, the "Extended Load Mode Register" command must be followed by a write to the SDRAM. The write in the SDRAM must be done in the appropriate bank; most low-power SDRAM devices use the bank 1.  */
#define   SDRAMC_MR_MODE_DEEP_POWERDOWN_Val   _U_(0x6)                                             /**< (SDRAMC_MR) Deep Power-down mode. Enters Deep Power-down mode.  */
#define SDRAMC_MR_MODE_NORMAL                 (SDRAMC_MR_MODE_NORMAL_Val << SDRAMC_MR_MODE_Pos)    /**< (SDRAMC_MR) Normal mode. Any access to the SDRAM is decoded normally. To activate this mode, the command must be followed by a write to the SDRAM. Position  */
#define SDRAMC_MR_MODE_NOP                    (SDRAMC_MR_MODE_NOP_Val << SDRAMC_MR_MODE_Pos)       /**< (SDRAMC_MR) The SDRAMC issues a NOP command when the SDRAM device is accessed regardless of the cycle. To activate this mode, the command must be followed by a write to the SDRAM. Position  */
#define SDRAMC_MR_MODE_ALLBANKS_PRECHARGE     (SDRAMC_MR_MODE_ALLBANKS_PRECHARGE_Val << SDRAMC_MR_MODE_Pos) /**< (SDRAMC_MR) The SDRAMC issues an "All Banks Precharge" command when the SDRAM device is accessed regardless of the cycle. To activate this mode, the command must be followed by a write to the SDRAM. Position  */
#define SDRAMC_MR_MODE_LOAD_MODEREG           (SDRAMC_MR_MODE_LOAD_MODEREG_Val << SDRAMC_MR_MODE_Pos) /**< (SDRAMC_MR) The SDRAMC issues a "Load Mode Register" command when the SDRAM device is accessed regardless of the cycle. To activate this mode, the command must be followed by a write to the SDRAM. Position  */
#define SDRAMC_MR_MODE_AUTO_REFRESH           (SDRAMC_MR_MODE_AUTO_REFRESH_Val << SDRAMC_MR_MODE_Pos) /**< (SDRAMC_MR) The SDRAMC issues an "Auto-Refresh" Command when the SDRAM device is accessed regardless of the cycle. Previously, an "All Banks Precharge" command must be issued. To activate this mode, the command must be followed by a write to the SDRAM. Position  */
#define SDRAMC_MR_MODE_EXT_LOAD_MODEREG       (SDRAMC_MR_MODE_EXT_LOAD_MODEREG_Val << SDRAMC_MR_MODE_Pos) /**< (SDRAMC_MR) The SDRAMC issues an "Extended Load Mode Register" command when the SDRAM device is accessed regardless of the cycle. To activate this mode, the "Extended Load Mode Register" command must be followed by a write to the SDRAM. The write in the SDRAM must be done in the appropriate bank; most low-power SDRAM devices use the bank 1. Position  */
#define SDRAMC_MR_MODE_DEEP_POWERDOWN         (SDRAMC_MR_MODE_DEEP_POWERDOWN_Val << SDRAMC_MR_MODE_Pos) /**< (SDRAMC_MR) Deep Power-down mode. Enters Deep Power-down mode. Position  */
#define SDRAMC_MR_Msk                         _U_(0x00000007)                                      /**< (SDRAMC_MR) Register Mask  */


/* -------- SDRAMC_TR : (SDRAMC Offset: 0x04) (R/W 32) SDRAMC Refresh Timer Register -------- */
#define SDRAMC_TR_COUNT_Pos                   _U_(0)                                               /**< (SDRAMC_TR) SDRAMC Refresh Timer Count Position */
#define SDRAMC_TR_COUNT_Msk                   (_U_(0xFFF) << SDRAMC_TR_COUNT_Pos)                  /**< (SDRAMC_TR) SDRAMC Refresh Timer Count Mask */
#define SDRAMC_TR_COUNT(value)                (SDRAMC_TR_COUNT_Msk & ((value) << SDRAMC_TR_COUNT_Pos))
#define SDRAMC_TR_Msk                         _U_(0x00000FFF)                                      /**< (SDRAMC_TR) Register Mask  */


/* -------- SDRAMC_CR : (SDRAMC Offset: 0x08) (R/W 32) SDRAMC Configuration Register -------- */
#define SDRAMC_CR_NC_Pos                      _U_(0)                                               /**< (SDRAMC_CR) Number of Column Bits Position */
#define SDRAMC_CR_NC_Msk                      (_U_(0x3) << SDRAMC_CR_NC_Pos)                       /**< (SDRAMC_CR) Number of Column Bits Mask */
#define SDRAMC_CR_NC(value)                   (SDRAMC_CR_NC_Msk & ((value) << SDRAMC_CR_NC_Pos))  
#define   SDRAMC_CR_NC_COL8_Val               _U_(0x0)                                             /**< (SDRAMC_CR) 8 column bits  */
#define   SDRAMC_CR_NC_COL9_Val               _U_(0x1)                                             /**< (SDRAMC_CR) 9 column bits  */
#define   SDRAMC_CR_NC_COL10_Val              _U_(0x2)                                             /**< (SDRAMC_CR) 10 column bits  */
#define   SDRAMC_CR_NC_COL11_Val              _U_(0x3)                                             /**< (SDRAMC_CR) 11 column bits  */
#define SDRAMC_CR_NC_COL8                     (SDRAMC_CR_NC_COL8_Val << SDRAMC_CR_NC_Pos)          /**< (SDRAMC_CR) 8 column bits Position  */
#define SDRAMC_CR_NC_COL9                     (SDRAMC_CR_NC_COL9_Val << SDRAMC_CR_NC_Pos)          /**< (SDRAMC_CR) 9 column bits Position  */
#define SDRAMC_CR_NC_COL10                    (SDRAMC_CR_NC_COL10_Val << SDRAMC_CR_NC_Pos)         /**< (SDRAMC_CR) 10 column bits Position  */
#define SDRAMC_CR_NC_COL11                    (SDRAMC_CR_NC_COL11_Val << SDRAMC_CR_NC_Pos)         /**< (SDRAMC_CR) 11 column bits Position  */
#define SDRAMC_CR_NR_Pos                      _U_(2)                                               /**< (SDRAMC_CR) Number of Row Bits Position */
#define SDRAMC_CR_NR_Msk                      (_U_(0x3) << SDRAMC_CR_NR_Pos)                       /**< (SDRAMC_CR) Number of Row Bits Mask */
#define SDRAMC_CR_NR(value)                   (SDRAMC_CR_NR_Msk & ((value) << SDRAMC_CR_NR_Pos))  
#define   SDRAMC_CR_NR_ROW11_Val              _U_(0x0)                                             /**< (SDRAMC_CR) 11 row bits  */
#define   SDRAMC_CR_NR_ROW12_Val              _U_(0x1)                                             /**< (SDRAMC_CR) 12 row bits  */
#define   SDRAMC_CR_NR_ROW13_Val              _U_(0x2)                                             /**< (SDRAMC_CR) 13 row bits  */
#define SDRAMC_CR_NR_ROW11                    (SDRAMC_CR_NR_ROW11_Val << SDRAMC_CR_NR_Pos)         /**< (SDRAMC_CR) 11 row bits Position  */
#define SDRAMC_CR_NR_ROW12                    (SDRAMC_CR_NR_ROW12_Val << SDRAMC_CR_NR_Pos)         /**< (SDRAMC_CR) 12 row bits Position  */
#define SDRAMC_CR_NR_ROW13                    (SDRAMC_CR_NR_ROW13_Val << SDRAMC_CR_NR_Pos)         /**< (SDRAMC_CR) 13 row bits Position  */
#define SDRAMC_CR_NB_Pos                      _U_(4)                                               /**< (SDRAMC_CR) Number of Banks Position */
#define SDRAMC_CR_NB_Msk                      (_U_(0x1) << SDRAMC_CR_NB_Pos)                       /**< (SDRAMC_CR) Number of Banks Mask */
#define SDRAMC_CR_NB(value)                   (SDRAMC_CR_NB_Msk & ((value) << SDRAMC_CR_NB_Pos))  
#define   SDRAMC_CR_NB_BANK2_Val              _U_(0x0)                                             /**< (SDRAMC_CR) 2 banks  */
#define   SDRAMC_CR_NB_BANK4_Val              _U_(0x1)                                             /**< (SDRAMC_CR) 4 banks  */
#define SDRAMC_CR_NB_BANK2                    (SDRAMC_CR_NB_BANK2_Val << SDRAMC_CR_NB_Pos)         /**< (SDRAMC_CR) 2 banks Position  */
#define SDRAMC_CR_NB_BANK4                    (SDRAMC_CR_NB_BANK4_Val << SDRAMC_CR_NB_Pos)         /**< (SDRAMC_CR) 4 banks Position  */
#define SDRAMC_CR_CAS_Pos                     _U_(5)                                               /**< (SDRAMC_CR) CAS Latency Position */
#define SDRAMC_CR_CAS_Msk                     (_U_(0x3) << SDRAMC_CR_CAS_Pos)                      /**< (SDRAMC_CR) CAS Latency Mask */
#define SDRAMC_CR_CAS(value)                  (SDRAMC_CR_CAS_Msk & ((value) << SDRAMC_CR_CAS_Pos))
#define   SDRAMC_CR_CAS_LATENCY1_Val          _U_(0x1)                                             /**< (SDRAMC_CR) 1 cycle CAS latency  */
#define   SDRAMC_CR_CAS_LATENCY2_Val          _U_(0x2)                                             /**< (SDRAMC_CR) 2 cycle CAS latency  */
#define   SDRAMC_CR_CAS_LATENCY3_Val          _U_(0x3)                                             /**< (SDRAMC_CR) 3 cycle CAS latency  */
#define SDRAMC_CR_CAS_LATENCY1                (SDRAMC_CR_CAS_LATENCY1_Val << SDRAMC_CR_CAS_Pos)    /**< (SDRAMC_CR) 1 cycle CAS latency Position  */
#define SDRAMC_CR_CAS_LATENCY2                (SDRAMC_CR_CAS_LATENCY2_Val << SDRAMC_CR_CAS_Pos)    /**< (SDRAMC_CR) 2 cycle CAS latency Position  */
#define SDRAMC_CR_CAS_LATENCY3                (SDRAMC_CR_CAS_LATENCY3_Val << SDRAMC_CR_CAS_Pos)    /**< (SDRAMC_CR) 3 cycle CAS latency Position  */
#define SDRAMC_CR_DBW_Pos                     _U_(7)                                               /**< (SDRAMC_CR) Data Bus Width Position */
#define SDRAMC_CR_DBW_Msk                     (_U_(0x1) << SDRAMC_CR_DBW_Pos)                      /**< (SDRAMC_CR) Data Bus Width Mask */
#define SDRAMC_CR_DBW(value)                  (SDRAMC_CR_DBW_Msk & ((value) << SDRAMC_CR_DBW_Pos))
#define SDRAMC_CR_TWR_Pos                     _U_(8)                                               /**< (SDRAMC_CR) Write Recovery Delay Position */
#define SDRAMC_CR_TWR_Msk                     (_U_(0xF) << SDRAMC_CR_TWR_Pos)                      /**< (SDRAMC_CR) Write Recovery Delay Mask */
#define SDRAMC_CR_TWR(value)                  (SDRAMC_CR_TWR_Msk & ((value) << SDRAMC_CR_TWR_Pos))
#define SDRAMC_CR_TRC_TRFC_Pos                _U_(12)                                              /**< (SDRAMC_CR) Row Cycle Delay and Row Refresh Cycle Position */
#define SDRAMC_CR_TRC_TRFC_Msk                (_U_(0xF) << SDRAMC_CR_TRC_TRFC_Pos)                 /**< (SDRAMC_CR) Row Cycle Delay and Row Refresh Cycle Mask */
#define SDRAMC_CR_TRC_TRFC(value)             (SDRAMC_CR_TRC_TRFC_Msk & ((value) << SDRAMC_CR_TRC_TRFC_Pos))
#define SDRAMC_CR_TRP_Pos                     _U_(16)                                              /**< (SDRAMC_CR) Row Precharge Delay Position */
#define SDRAMC_CR_TRP_Msk                     (_U_(0xF) << SDRAMC_CR_TRP_Pos)                      /**< (SDRAMC_CR) Row Precharge Delay Mask */
#define SDRAMC_CR_TRP(value)                  (SDRAMC_CR_TRP_Msk & ((value) << SDRAMC_CR_TRP_Pos))
#define SDRAMC_CR_TRCD_Pos                    _U_(20)                                              /**< (SDRAMC_CR) Row to Column Delay Position */
#define SDRAMC_CR_TRCD_Msk                    (_U_(0xF) << SDRAMC_CR_TRCD_Pos)                     /**< (SDRAMC_CR) Row to Column Delay Mask */
#define SDRAMC_CR_TRCD(value)                 (SDRAMC_CR_TRCD_Msk & ((value) << SDRAMC_CR_TRCD_Pos))
#define SDRAMC_CR_TRAS_Pos                    _U_(24)                                              /**< (SDRAMC_CR) Active to Precharge Delay Position */
#define SDRAMC_CR_TRAS_Msk                    (_U_(0xF) << SDRAMC_CR_TRAS_Pos)                     /**< (SDRAMC_CR) Active to Precharge Delay Mask */
#define SDRAMC_CR_TRAS(value)                 (SDRAMC_CR_TRAS_Msk & ((value) << SDRAMC_CR_TRAS_Pos))
#define SDRAMC_CR_TXSR_Pos                    _U_(28)                                              /**< (SDRAMC_CR) Exit Self-Refresh to Active Delay Position */
#define SDRAMC_CR_TXSR_Msk                    (_U_(0xF) << SDRAMC_CR_TXSR_Pos)                     /**< (SDRAMC_CR) Exit Self-Refresh to Active Delay Mask */
#define SDRAMC_CR_TXSR(value)                 (SDRAMC_CR_TXSR_Msk & ((value) << SDRAMC_CR_TXSR_Pos))
#define SDRAMC_CR_Msk                         _U_(0xFFFFFFFF)                                      /**< (SDRAMC_CR) Register Mask  */


/* -------- SDRAMC_LPR : (SDRAMC Offset: 0x10) (R/W 32) SDRAMC Low Power Register -------- */
#define SDRAMC_LPR_LPCB_Pos                   _U_(0)                                               /**< (SDRAMC_LPR) Low-power Configuration Bits Position */
#define SDRAMC_LPR_LPCB_Msk                   (_U_(0x3) << SDRAMC_LPR_LPCB_Pos)                    /**< (SDRAMC_LPR) Low-power Configuration Bits Mask */
#define SDRAMC_LPR_LPCB(value)                (SDRAMC_LPR_LPCB_Msk & ((value) << SDRAMC_LPR_LPCB_Pos))
#define   SDRAMC_LPR_LPCB_DISABLED_Val        _U_(0x0)                                             /**< (SDRAMC_LPR) The low-power feature is inhibited: no Power-down, Self-refresh or Deep Power-down command is issued to the SDRAM device.  */
#define   SDRAMC_LPR_LPCB_SELF_REFRESH_Val    _U_(0x1)                                             /**< (SDRAMC_LPR) The SDRAMC issues a Self-refresh command to the SDRAM device, the SDCK clock is deactivated and the SDCKE signal is set low. The SDRAM device leaves the Self-refresh mode when accessed and enters it after the access.  */
#define   SDRAMC_LPR_LPCB_POWER_DOWN_Val      _U_(0x2)                                             /**< (SDRAMC_LPR) The SDRAMC issues a Power-down Command to the SDRAM device after each access, the SDCKE signal is set to low. The SDRAM device leaves the Power-down mode when accessed and enters it after the access.  */
#define   SDRAMC_LPR_LPCB_DEEP_POWER_DOWN_Val _U_(0x3)                                             /**< (SDRAMC_LPR) The SDRAMC issues a Deep Power-down command to the SDRAM device. This mode is unique to low-power SDRAM.  */
#define SDRAMC_LPR_LPCB_DISABLED              (SDRAMC_LPR_LPCB_DISABLED_Val << SDRAMC_LPR_LPCB_Pos) /**< (SDRAMC_LPR) The low-power feature is inhibited: no Power-down, Self-refresh or Deep Power-down command is issued to the SDRAM device. Position  */
#define SDRAMC_LPR_LPCB_SELF_REFRESH          (SDRAMC_LPR_LPCB_SELF_REFRESH_Val << SDRAMC_LPR_LPCB_Pos) /**< (SDRAMC_LPR) The SDRAMC issues a Self-refresh command to the SDRAM device, the SDCK clock is deactivated and the SDCKE signal is set low. The SDRAM device leaves the Self-refresh mode when accessed and enters it after the access. Position  */
#define SDRAMC_LPR_LPCB_POWER_DOWN            (SDRAMC_LPR_LPCB_POWER_DOWN_Val << SDRAMC_LPR_LPCB_Pos) /**< (SDRAMC_LPR) The SDRAMC issues a Power-down Command to the SDRAM device after each access, the SDCKE signal is set to low. The SDRAM device leaves the Power-down mode when accessed and enters it after the access. Position  */
#define SDRAMC_LPR_LPCB_DEEP_POWER_DOWN       (SDRAMC_LPR_LPCB_DEEP_POWER_DOWN_Val << SDRAMC_LPR_LPCB_Pos) /**< (SDRAMC_LPR) The SDRAMC issues a Deep Power-down command to the SDRAM device. This mode is unique to low-power SDRAM. Position  */
#define SDRAMC_LPR_PASR_Pos                   _U_(4)                                               /**< (SDRAMC_LPR) Partial Array Self-refresh (only for low-power SDRAM) Position */
#define SDRAMC_LPR_PASR_Msk                   (_U_(0x7) << SDRAMC_LPR_PASR_Pos)                    /**< (SDRAMC_LPR) Partial Array Self-refresh (only for low-power SDRAM) Mask */
#define SDRAMC_LPR_PASR(value)                (SDRAMC_LPR_PASR_Msk & ((value) << SDRAMC_LPR_PASR_Pos))
#define SDRAMC_LPR_TCSR_Pos                   _U_(8)                                               /**< (SDRAMC_LPR) Temperature Compensated Self-Refresh (only for low-power SDRAM) Position */
#define SDRAMC_LPR_TCSR_Msk                   (_U_(0x3) << SDRAMC_LPR_TCSR_Pos)                    /**< (SDRAMC_LPR) Temperature Compensated Self-Refresh (only for low-power SDRAM) Mask */
#define SDRAMC_LPR_TCSR(value)                (SDRAMC_LPR_TCSR_Msk & ((value) << SDRAMC_LPR_TCSR_Pos))
#define SDRAMC_LPR_DS_Pos                     _U_(10)                                              /**< (SDRAMC_LPR) Drive Strength (only for low-power SDRAM) Position */
#define SDRAMC_LPR_DS_Msk                     (_U_(0x3) << SDRAMC_LPR_DS_Pos)                      /**< (SDRAMC_LPR) Drive Strength (only for low-power SDRAM) Mask */
#define SDRAMC_LPR_DS(value)                  (SDRAMC_LPR_DS_Msk & ((value) << SDRAMC_LPR_DS_Pos))
#define SDRAMC_LPR_TIMEOUT_Pos                _U_(12)                                              /**< (SDRAMC_LPR) Time to Define When Low-power Mode Is Enabled Position */
#define SDRAMC_LPR_TIMEOUT_Msk                (_U_(0x3) << SDRAMC_LPR_TIMEOUT_Pos)                 /**< (SDRAMC_LPR) Time to Define When Low-power Mode Is Enabled Mask */
#define SDRAMC_LPR_TIMEOUT(value)             (SDRAMC_LPR_TIMEOUT_Msk & ((value) << SDRAMC_LPR_TIMEOUT_Pos))
#define   SDRAMC_LPR_TIMEOUT_LP_LAST_XFER_Val _U_(0x0)                                             /**< (SDRAMC_LPR) The SDRAMC activates the SDRAM Low-power mode immediately after the end of the last transfer.  */
#define   SDRAMC_LPR_TIMEOUT_LP_LAST_XFER_64_Val _U_(0x1)                                             /**< (SDRAMC_LPR) The SDRAMC activates the SDRAM Low-power mode 64 clock cycles after the end of the last transfer.  */
#define   SDRAMC_LPR_TIMEOUT_LP_LAST_XFER_128_Val _U_(0x2)                                             /**< (SDRAMC_LPR) The SDRAMC activates the SDRAM Low-power mode 128 clock cycles after the end of the last transfer.  */
#define SDRAMC_LPR_TIMEOUT_LP_LAST_XFER       (SDRAMC_LPR_TIMEOUT_LP_LAST_XFER_Val << SDRAMC_LPR_TIMEOUT_Pos) /**< (SDRAMC_LPR) The SDRAMC activates the SDRAM Low-power mode immediately after the end of the last transfer. Position  */
#define SDRAMC_LPR_TIMEOUT_LP_LAST_XFER_64    (SDRAMC_LPR_TIMEOUT_LP_LAST_XFER_64_Val << SDRAMC_LPR_TIMEOUT_Pos) /**< (SDRAMC_LPR) The SDRAMC activates the SDRAM Low-power mode 64 clock cycles after the end of the last transfer. Position  */
#define SDRAMC_LPR_TIMEOUT_LP_LAST_XFER_128   (SDRAMC_LPR_TIMEOUT_LP_LAST_XFER_128_Val << SDRAMC_LPR_TIMEOUT_Pos) /**< (SDRAMC_LPR) The SDRAMC activates the SDRAM Low-power mode 128 clock cycles after the end of the last transfer. Position  */
#define SDRAMC_LPR_Msk                        _U_(0x00003F73)                                      /**< (SDRAMC_LPR) Register Mask  */


/* -------- SDRAMC_IER : (SDRAMC Offset: 0x14) ( /W 32) SDRAMC Interrupt Enable Register -------- */
#define SDRAMC_IER_RES_Pos                    _U_(0)                                               /**< (SDRAMC_IER) Refresh Error Interrupt Enable Position */
#define SDRAMC_IER_RES_Msk                    (_U_(0x1) << SDRAMC_IER_RES_Pos)                     /**< (SDRAMC_IER) Refresh Error Interrupt Enable Mask */
#define SDRAMC_IER_RES(value)                 (SDRAMC_IER_RES_Msk & ((value) << SDRAMC_IER_RES_Pos))
#define SDRAMC_IER_Msk                        _U_(0x00000001)                                      /**< (SDRAMC_IER) Register Mask  */


/* -------- SDRAMC_IDR : (SDRAMC Offset: 0x18) ( /W 32) SDRAMC Interrupt Disable Register -------- */
#define SDRAMC_IDR_RES_Pos                    _U_(0)                                               /**< (SDRAMC_IDR) Refresh Error Interrupt Disable Position */
#define SDRAMC_IDR_RES_Msk                    (_U_(0x1) << SDRAMC_IDR_RES_Pos)                     /**< (SDRAMC_IDR) Refresh Error Interrupt Disable Mask */
#define SDRAMC_IDR_RES(value)                 (SDRAMC_IDR_RES_Msk & ((value) << SDRAMC_IDR_RES_Pos))
#define SDRAMC_IDR_Msk                        _U_(0x00000001)                                      /**< (SDRAMC_IDR) Register Mask  */


/* -------- SDRAMC_IMR : (SDRAMC Offset: 0x1C) ( R/ 32) SDRAMC Interrupt Mask Register -------- */
#define SDRAMC_IMR_RES_Pos                    _U_(0)                                               /**< (SDRAMC_IMR) Refresh Error Interrupt Mask Position */
#define SDRAMC_IMR_RES_Msk                    (_U_(0x1) << SDRAMC_IMR_RES_Pos)                     /**< (SDRAMC_IMR) Refresh Error Interrupt Mask Mask */
#define SDRAMC_IMR_RES(value)                 (SDRAMC_IMR_RES_Msk & ((value) << SDRAMC_IMR_RES_Pos))
#define SDRAMC_IMR_Msk                        _U_(0x00000001)                                      /**< (SDRAMC_IMR) Register Mask  */


/* -------- SDRAMC_ISR : (SDRAMC Offset: 0x20) ( R/ 32) SDRAMC Interrupt Status Register -------- */
#define SDRAMC_ISR_RES_Pos                    _U_(0)                                               /**< (SDRAMC_ISR) Refresh Error Status (cleared on read) Position */
#define SDRAMC_ISR_RES_Msk                    (_U_(0x1) << SDRAMC_ISR_RES_Pos)                     /**< (SDRAMC_ISR) Refresh Error Status (cleared on read) Mask */
#define SDRAMC_ISR_RES(value)                 (SDRAMC_ISR_RES_Msk & ((value) << SDRAMC_ISR_RES_Pos))
#define SDRAMC_ISR_Msk                        _U_(0x00000001)                                      /**< (SDRAMC_ISR) Register Mask  */


/* -------- SDRAMC_MDR : (SDRAMC Offset: 0x24) (R/W 32) SDRAMC Memory Device Register -------- */
#define SDRAMC_MDR_MD_Pos                     _U_(0)                                               /**< (SDRAMC_MDR) Memory Device Type Position */
#define SDRAMC_MDR_MD_Msk                     (_U_(0x3) << SDRAMC_MDR_MD_Pos)                      /**< (SDRAMC_MDR) Memory Device Type Mask */
#define SDRAMC_MDR_MD(value)                  (SDRAMC_MDR_MD_Msk & ((value) << SDRAMC_MDR_MD_Pos))
#define   SDRAMC_MDR_MD_SDRAM_Val             _U_(0x0)                                             /**< (SDRAMC_MDR) SDRAM  */
#define   SDRAMC_MDR_MD_LPSDRAM_Val           _U_(0x1)                                             /**< (SDRAMC_MDR) Low-power SDRAM  */
#define SDRAMC_MDR_MD_SDRAM                   (SDRAMC_MDR_MD_SDRAM_Val << SDRAMC_MDR_MD_Pos)       /**< (SDRAMC_MDR) SDRAM Position  */
#define SDRAMC_MDR_MD_LPSDRAM                 (SDRAMC_MDR_MD_LPSDRAM_Val << SDRAMC_MDR_MD_Pos)     /**< (SDRAMC_MDR) Low-power SDRAM Position  */
#define SDRAMC_MDR_Msk                        _U_(0x00000003)                                      /**< (SDRAMC_MDR) Register Mask  */


/* -------- SDRAMC_CFR1 : (SDRAMC Offset: 0x28) (R/W 32) SDRAMC Configuration Register 1 -------- */
#define SDRAMC_CFR1_TMRD_Pos                  _U_(0)                                               /**< (SDRAMC_CFR1) Load Mode Register Command to Active or Refresh Command Position */
#define SDRAMC_CFR1_TMRD_Msk                  (_U_(0xF) << SDRAMC_CFR1_TMRD_Pos)                   /**< (SDRAMC_CFR1) Load Mode Register Command to Active or Refresh Command Mask */
#define SDRAMC_CFR1_TMRD(value)               (SDRAMC_CFR1_TMRD_Msk & ((value) << SDRAMC_CFR1_TMRD_Pos))
#define SDRAMC_CFR1_UNAL_Pos                  _U_(8)                                               /**< (SDRAMC_CFR1) Support Unaligned Access Position */
#define SDRAMC_CFR1_UNAL_Msk                  (_U_(0x1) << SDRAMC_CFR1_UNAL_Pos)                   /**< (SDRAMC_CFR1) Support Unaligned Access Mask */
#define SDRAMC_CFR1_UNAL(value)               (SDRAMC_CFR1_UNAL_Msk & ((value) << SDRAMC_CFR1_UNAL_Pos))
#define   SDRAMC_CFR1_UNAL_UNSUPPORTED_Val    _U_(0x0)                                             /**< (SDRAMC_CFR1) Unaligned access is not supported.  */
#define   SDRAMC_CFR1_UNAL_SUPPORTED_Val      _U_(0x1)                                             /**< (SDRAMC_CFR1) Unaligned access is supported.  */
#define SDRAMC_CFR1_UNAL_UNSUPPORTED          (SDRAMC_CFR1_UNAL_UNSUPPORTED_Val << SDRAMC_CFR1_UNAL_Pos) /**< (SDRAMC_CFR1) Unaligned access is not supported. Position  */
#define SDRAMC_CFR1_UNAL_SUPPORTED            (SDRAMC_CFR1_UNAL_SUPPORTED_Val << SDRAMC_CFR1_UNAL_Pos) /**< (SDRAMC_CFR1) Unaligned access is supported. Position  */
#define SDRAMC_CFR1_Msk                       _U_(0x0000010F)                                      /**< (SDRAMC_CFR1) Register Mask  */


/* -------- SDRAMC_OCMS : (SDRAMC Offset: 0x2C) (R/W 32) SDRAMC OCMS Register -------- */
#define SDRAMC_OCMS_SDR_SE_Pos                _U_(0)                                               /**< (SDRAMC_OCMS) SDRAM Memory Controller Scrambling Enable Position */
#define SDRAMC_OCMS_SDR_SE_Msk                (_U_(0x1) << SDRAMC_OCMS_SDR_SE_Pos)                 /**< (SDRAMC_OCMS) SDRAM Memory Controller Scrambling Enable Mask */
#define SDRAMC_OCMS_SDR_SE(value)             (SDRAMC_OCMS_SDR_SE_Msk & ((value) << SDRAMC_OCMS_SDR_SE_Pos))
#define SDRAMC_OCMS_Msk                       _U_(0x00000001)                                      /**< (SDRAMC_OCMS) Register Mask  */


/* -------- SDRAMC_OCMS_KEY1 : (SDRAMC Offset: 0x30) ( /W 32) SDRAMC OCMS KEY1 Register -------- */
#define SDRAMC_OCMS_KEY1_KEY1_Pos             _U_(0)                                               /**< (SDRAMC_OCMS_KEY1) Off-chip Memory Scrambling (OCMS) Key Part 1 Position */
#define SDRAMC_OCMS_KEY1_KEY1_Msk             (_U_(0xFFFFFFFF) << SDRAMC_OCMS_KEY1_KEY1_Pos)       /**< (SDRAMC_OCMS_KEY1) Off-chip Memory Scrambling (OCMS) Key Part 1 Mask */
#define SDRAMC_OCMS_KEY1_KEY1(value)          (SDRAMC_OCMS_KEY1_KEY1_Msk & ((value) << SDRAMC_OCMS_KEY1_KEY1_Pos))
#define SDRAMC_OCMS_KEY1_Msk                  _U_(0xFFFFFFFF)                                      /**< (SDRAMC_OCMS_KEY1) Register Mask  */


/* -------- SDRAMC_OCMS_KEY2 : (SDRAMC Offset: 0x34) ( /W 32) SDRAMC OCMS KEY2 Register -------- */
#define SDRAMC_OCMS_KEY2_KEY2_Pos             _U_(0)                                               /**< (SDRAMC_OCMS_KEY2) Off-chip Memory Scrambling (OCMS) Key Part 2 Position */
#define SDRAMC_OCMS_KEY2_KEY2_Msk             (_U_(0xFFFFFFFF) << SDRAMC_OCMS_KEY2_KEY2_Pos)       /**< (SDRAMC_OCMS_KEY2) Off-chip Memory Scrambling (OCMS) Key Part 2 Mask */
#define SDRAMC_OCMS_KEY2_KEY2(value)          (SDRAMC_OCMS_KEY2_KEY2_Msk & ((value) << SDRAMC_OCMS_KEY2_KEY2_Pos))
#define SDRAMC_OCMS_KEY2_Msk                  _U_(0xFFFFFFFF)                                      /**< (SDRAMC_OCMS_KEY2) Register Mask  */


/** \brief SDRAMC register offsets definitions */
#define SDRAMC_MR_REG_OFST             (0x00)              /**< (SDRAMC_MR) SDRAMC Mode Register Offset */
#define SDRAMC_TR_REG_OFST             (0x04)              /**< (SDRAMC_TR) SDRAMC Refresh Timer Register Offset */
#define SDRAMC_CR_REG_OFST             (0x08)              /**< (SDRAMC_CR) SDRAMC Configuration Register Offset */
#define SDRAMC_LPR_REG_OFST            (0x10)              /**< (SDRAMC_LPR) SDRAMC Low Power Register Offset */
#define SDRAMC_IER_REG_OFST            (0x14)              /**< (SDRAMC_IER) SDRAMC Interrupt Enable Register Offset */
#define SDRAMC_IDR_REG_OFST            (0x18)              /**< (SDRAMC_IDR) SDRAMC Interrupt Disable Register Offset */
#define SDRAMC_IMR_REG_OFST            (0x1C)              /**< (SDRAMC_IMR) SDRAMC Interrupt Mask Register Offset */
#define SDRAMC_ISR_REG_OFST            (0x20)              /**< (SDRAMC_ISR) SDRAMC Interrupt Status Register Offset */
#define SDRAMC_MDR_REG_OFST            (0x24)              /**< (SDRAMC_MDR) SDRAMC Memory Device Register Offset */
#define SDRAMC_CFR1_REG_OFST           (0x28)              /**< (SDRAMC_CFR1) SDRAMC Configuration Register 1 Offset */
#define SDRAMC_OCMS_REG_OFST           (0x2C)              /**< (SDRAMC_OCMS) SDRAMC OCMS Register Offset */
#define SDRAMC_OCMS_KEY1_REG_OFST      (0x30)              /**< (SDRAMC_OCMS_KEY1) SDRAMC OCMS KEY1 Register Offset */
#define SDRAMC_OCMS_KEY2_REG_OFST      (0x34)              /**< (SDRAMC_OCMS_KEY2) SDRAMC OCMS KEY2 Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief SDRAMC register API structure */
typedef struct
{
  __IO  uint32_t                       SDRAMC_MR;          /**< Offset: 0x00 (R/W  32) SDRAMC Mode Register */
  __IO  uint32_t                       SDRAMC_TR;          /**< Offset: 0x04 (R/W  32) SDRAMC Refresh Timer Register */
  __IO  uint32_t                       SDRAMC_CR;          /**< Offset: 0x08 (R/W  32) SDRAMC Configuration Register */
  __I   uint8_t                        Reserved1[0x04];
  __IO  uint32_t                       SDRAMC_LPR;         /**< Offset: 0x10 (R/W  32) SDRAMC Low Power Register */
  __O   uint32_t                       SDRAMC_IER;         /**< Offset: 0x14 ( /W  32) SDRAMC Interrupt Enable Register */
  __O   uint32_t                       SDRAMC_IDR;         /**< Offset: 0x18 ( /W  32) SDRAMC Interrupt Disable Register */
  __I   uint32_t                       SDRAMC_IMR;         /**< Offset: 0x1C (R/   32) SDRAMC Interrupt Mask Register */
  __I   uint32_t                       SDRAMC_ISR;         /**< Offset: 0x20 (R/   32) SDRAMC Interrupt Status Register */
  __IO  uint32_t                       SDRAMC_MDR;         /**< Offset: 0x24 (R/W  32) SDRAMC Memory Device Register */
  __IO  uint32_t                       SDRAMC_CFR1;        /**< Offset: 0x28 (R/W  32) SDRAMC Configuration Register 1 */
  __IO  uint32_t                       SDRAMC_OCMS;        /**< Offset: 0x2C (R/W  32) SDRAMC OCMS Register */
  __O   uint32_t                       SDRAMC_OCMS_KEY1;   /**< Offset: 0x30 ( /W  32) SDRAMC OCMS KEY1 Register */
  __O   uint32_t                       SDRAMC_OCMS_KEY2;   /**< Offset: 0x34 ( /W  32) SDRAMC OCMS KEY2 Register */
} sdramc_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_SDRAMC_COMPONENT_H_ */
