/**
 * \brief Component description for NVMCTRL
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
#ifndef _SAME54_NVMCTRL_COMPONENT_H_
#define _SAME54_NVMCTRL_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR NVMCTRL                                      */
/* ************************************************************************** */

/* -------- NVMCTRL_CTRLA : (NVMCTRL Offset: 0x00) (R/W 16) Control A -------- */
#define NVMCTRL_CTRLA_RESETVALUE              _U_(0x04)                                            /**<  (NVMCTRL_CTRLA) Control A  Reset Value */

#define NVMCTRL_CTRLA_AUTOWS_Pos              _U_(2)                                               /**< (NVMCTRL_CTRLA) Auto Wait State Enable Position */
#define NVMCTRL_CTRLA_AUTOWS_Msk              (_U_(0x1) << NVMCTRL_CTRLA_AUTOWS_Pos)               /**< (NVMCTRL_CTRLA) Auto Wait State Enable Mask */
#define NVMCTRL_CTRLA_AUTOWS(value)           (NVMCTRL_CTRLA_AUTOWS_Msk & ((value) << NVMCTRL_CTRLA_AUTOWS_Pos))
#define NVMCTRL_CTRLA_SUSPEN_Pos              _U_(3)                                               /**< (NVMCTRL_CTRLA) Suspend Enable Position */
#define NVMCTRL_CTRLA_SUSPEN_Msk              (_U_(0x1) << NVMCTRL_CTRLA_SUSPEN_Pos)               /**< (NVMCTRL_CTRLA) Suspend Enable Mask */
#define NVMCTRL_CTRLA_SUSPEN(value)           (NVMCTRL_CTRLA_SUSPEN_Msk & ((value) << NVMCTRL_CTRLA_SUSPEN_Pos))
#define NVMCTRL_CTRLA_WMODE_Pos               _U_(4)                                               /**< (NVMCTRL_CTRLA) Write Mode Position */
#define NVMCTRL_CTRLA_WMODE_Msk               (_U_(0x3) << NVMCTRL_CTRLA_WMODE_Pos)                /**< (NVMCTRL_CTRLA) Write Mode Mask */
#define NVMCTRL_CTRLA_WMODE(value)            (NVMCTRL_CTRLA_WMODE_Msk & ((value) << NVMCTRL_CTRLA_WMODE_Pos))
#define   NVMCTRL_CTRLA_WMODE_MAN_Val         _U_(0x0)                                             /**< (NVMCTRL_CTRLA) Manual Write  */
#define   NVMCTRL_CTRLA_WMODE_ADW_Val         _U_(0x1)                                             /**< (NVMCTRL_CTRLA) Automatic Double Word Write  */
#define   NVMCTRL_CTRLA_WMODE_AQW_Val         _U_(0x2)                                             /**< (NVMCTRL_CTRLA) Automatic Quad Word  */
#define   NVMCTRL_CTRLA_WMODE_AP_Val          _U_(0x3)                                             /**< (NVMCTRL_CTRLA) Automatic Page Write  */
#define NVMCTRL_CTRLA_WMODE_MAN               (NVMCTRL_CTRLA_WMODE_MAN_Val << NVMCTRL_CTRLA_WMODE_Pos) /**< (NVMCTRL_CTRLA) Manual Write Position  */
#define NVMCTRL_CTRLA_WMODE_ADW               (NVMCTRL_CTRLA_WMODE_ADW_Val << NVMCTRL_CTRLA_WMODE_Pos) /**< (NVMCTRL_CTRLA) Automatic Double Word Write Position  */
#define NVMCTRL_CTRLA_WMODE_AQW               (NVMCTRL_CTRLA_WMODE_AQW_Val << NVMCTRL_CTRLA_WMODE_Pos) /**< (NVMCTRL_CTRLA) Automatic Quad Word Position  */
#define NVMCTRL_CTRLA_WMODE_AP                (NVMCTRL_CTRLA_WMODE_AP_Val << NVMCTRL_CTRLA_WMODE_Pos) /**< (NVMCTRL_CTRLA) Automatic Page Write Position  */
#define NVMCTRL_CTRLA_PRM_Pos                 _U_(6)                                               /**< (NVMCTRL_CTRLA) Power Reduction Mode during Sleep Position */
#define NVMCTRL_CTRLA_PRM_Msk                 (_U_(0x3) << NVMCTRL_CTRLA_PRM_Pos)                  /**< (NVMCTRL_CTRLA) Power Reduction Mode during Sleep Mask */
#define NVMCTRL_CTRLA_PRM(value)              (NVMCTRL_CTRLA_PRM_Msk & ((value) << NVMCTRL_CTRLA_PRM_Pos))
#define   NVMCTRL_CTRLA_PRM_SEMIAUTO_Val      _U_(0x0)                                             /**< (NVMCTRL_CTRLA) NVM block enters low-power mode when entering standby mode. NVM block enters low-power mode when SPRM command is issued. NVM block exits low-power mode upon first access.  */
#define   NVMCTRL_CTRLA_PRM_FULLAUTO_Val      _U_(0x1)                                             /**< (NVMCTRL_CTRLA) NVM block enters low-power mode when entering standby mode. NVM block enters low-power mode when SPRM command is issued. NVM block exits low-power mode when system is not in standby mode.  */
#define   NVMCTRL_CTRLA_PRM_MANUAL_Val        _U_(0x3)                                             /**< (NVMCTRL_CTRLA) NVM block does not enter low-power mode when entering standby mode. NVM block enters low-power mode when SPRM command is issued. NVM block exits low-power mode upon first access.  */
#define NVMCTRL_CTRLA_PRM_SEMIAUTO            (NVMCTRL_CTRLA_PRM_SEMIAUTO_Val << NVMCTRL_CTRLA_PRM_Pos) /**< (NVMCTRL_CTRLA) NVM block enters low-power mode when entering standby mode. NVM block enters low-power mode when SPRM command is issued. NVM block exits low-power mode upon first access. Position  */
#define NVMCTRL_CTRLA_PRM_FULLAUTO            (NVMCTRL_CTRLA_PRM_FULLAUTO_Val << NVMCTRL_CTRLA_PRM_Pos) /**< (NVMCTRL_CTRLA) NVM block enters low-power mode when entering standby mode. NVM block enters low-power mode when SPRM command is issued. NVM block exits low-power mode when system is not in standby mode. Position  */
#define NVMCTRL_CTRLA_PRM_MANUAL              (NVMCTRL_CTRLA_PRM_MANUAL_Val << NVMCTRL_CTRLA_PRM_Pos) /**< (NVMCTRL_CTRLA) NVM block does not enter low-power mode when entering standby mode. NVM block enters low-power mode when SPRM command is issued. NVM block exits low-power mode upon first access. Position  */
#define NVMCTRL_CTRLA_RWS_Pos                 _U_(8)                                               /**< (NVMCTRL_CTRLA) NVM Read Wait States Position */
#define NVMCTRL_CTRLA_RWS_Msk                 (_U_(0xF) << NVMCTRL_CTRLA_RWS_Pos)                  /**< (NVMCTRL_CTRLA) NVM Read Wait States Mask */
#define NVMCTRL_CTRLA_RWS(value)              (NVMCTRL_CTRLA_RWS_Msk & ((value) << NVMCTRL_CTRLA_RWS_Pos))
#define NVMCTRL_CTRLA_AHBNS0_Pos              _U_(12)                                              /**< (NVMCTRL_CTRLA) Force AHB0 access to NONSEQ, burst transfers are continuously rearbitrated Position */
#define NVMCTRL_CTRLA_AHBNS0_Msk              (_U_(0x1) << NVMCTRL_CTRLA_AHBNS0_Pos)               /**< (NVMCTRL_CTRLA) Force AHB0 access to NONSEQ, burst transfers are continuously rearbitrated Mask */
#define NVMCTRL_CTRLA_AHBNS0(value)           (NVMCTRL_CTRLA_AHBNS0_Msk & ((value) << NVMCTRL_CTRLA_AHBNS0_Pos))
#define NVMCTRL_CTRLA_AHBNS1_Pos              _U_(13)                                              /**< (NVMCTRL_CTRLA) Force AHB1 access to NONSEQ, burst transfers are continuously rearbitrated Position */
#define NVMCTRL_CTRLA_AHBNS1_Msk              (_U_(0x1) << NVMCTRL_CTRLA_AHBNS1_Pos)               /**< (NVMCTRL_CTRLA) Force AHB1 access to NONSEQ, burst transfers are continuously rearbitrated Mask */
#define NVMCTRL_CTRLA_AHBNS1(value)           (NVMCTRL_CTRLA_AHBNS1_Msk & ((value) << NVMCTRL_CTRLA_AHBNS1_Pos))
#define NVMCTRL_CTRLA_CACHEDIS0_Pos           _U_(14)                                              /**< (NVMCTRL_CTRLA) AHB0 Cache Disable Position */
#define NVMCTRL_CTRLA_CACHEDIS0_Msk           (_U_(0x1) << NVMCTRL_CTRLA_CACHEDIS0_Pos)            /**< (NVMCTRL_CTRLA) AHB0 Cache Disable Mask */
#define NVMCTRL_CTRLA_CACHEDIS0(value)        (NVMCTRL_CTRLA_CACHEDIS0_Msk & ((value) << NVMCTRL_CTRLA_CACHEDIS0_Pos))
#define NVMCTRL_CTRLA_CACHEDIS1_Pos           _U_(15)                                              /**< (NVMCTRL_CTRLA) AHB1 Cache Disable Position */
#define NVMCTRL_CTRLA_CACHEDIS1_Msk           (_U_(0x1) << NVMCTRL_CTRLA_CACHEDIS1_Pos)            /**< (NVMCTRL_CTRLA) AHB1 Cache Disable Mask */
#define NVMCTRL_CTRLA_CACHEDIS1(value)        (NVMCTRL_CTRLA_CACHEDIS1_Msk & ((value) << NVMCTRL_CTRLA_CACHEDIS1_Pos))
#define NVMCTRL_CTRLA_Msk                     _U_(0xFFFC)                                          /**< (NVMCTRL_CTRLA) Register Mask  */

#define NVMCTRL_CTRLA_AHBNS_Pos               _U_(12)                                              /**< (NVMCTRL_CTRLA Position) Force AHBx access to NONSEQ, burst transfers are continuously rearbitrated */
#define NVMCTRL_CTRLA_AHBNS_Msk               (_U_(0x3) << NVMCTRL_CTRLA_AHBNS_Pos)                /**< (NVMCTRL_CTRLA Mask) AHBNS */
#define NVMCTRL_CTRLA_AHBNS(value)            (NVMCTRL_CTRLA_AHBNS_Msk & ((value) << NVMCTRL_CTRLA_AHBNS_Pos)) 
#define NVMCTRL_CTRLA_CACHEDIS_Pos            _U_(14)                                              /**< (NVMCTRL_CTRLA Position) AHBx Cache Disable */
#define NVMCTRL_CTRLA_CACHEDIS_Msk            (_U_(0x3) << NVMCTRL_CTRLA_CACHEDIS_Pos)             /**< (NVMCTRL_CTRLA Mask) CACHEDIS */
#define NVMCTRL_CTRLA_CACHEDIS(value)         (NVMCTRL_CTRLA_CACHEDIS_Msk & ((value) << NVMCTRL_CTRLA_CACHEDIS_Pos)) 

/* -------- NVMCTRL_CTRLB : (NVMCTRL Offset: 0x04) ( /W 16) Control B -------- */
#define NVMCTRL_CTRLB_RESETVALUE              _U_(0x00)                                            /**<  (NVMCTRL_CTRLB) Control B  Reset Value */

#define NVMCTRL_CTRLB_CMD_Pos                 _U_(0)                                               /**< (NVMCTRL_CTRLB) Command Position */
#define NVMCTRL_CTRLB_CMD_Msk                 (_U_(0x7F) << NVMCTRL_CTRLB_CMD_Pos)                 /**< (NVMCTRL_CTRLB) Command Mask */
#define NVMCTRL_CTRLB_CMD(value)              (NVMCTRL_CTRLB_CMD_Msk & ((value) << NVMCTRL_CTRLB_CMD_Pos))
#define   NVMCTRL_CTRLB_CMD_EP_Val            _U_(0x0)                                             /**< (NVMCTRL_CTRLB) Erase Page - Only supported in the USER and AUX pages.  */
#define   NVMCTRL_CTRLB_CMD_EB_Val            _U_(0x1)                                             /**< (NVMCTRL_CTRLB) Erase Block - Erases the block addressed by the ADDR register, not supported in the user page  */
#define   NVMCTRL_CTRLB_CMD_WP_Val            _U_(0x3)                                             /**< (NVMCTRL_CTRLB) Write Page - Writes the contents of the page buffer to the page addressed by the ADDR register, not supported in the user page  */
#define   NVMCTRL_CTRLB_CMD_WQW_Val           _U_(0x4)                                             /**< (NVMCTRL_CTRLB) Write Quad Word - Writes a 128-bit word at the location addressed by the ADDR register.  */
#define   NVMCTRL_CTRLB_CMD_SWRST_Val         _U_(0x10)                                            /**< (NVMCTRL_CTRLB) Software Reset - Power-Cycle the NVM memory and replay the device automatic calibration procedure and resets the module configuration registers  */
#define   NVMCTRL_CTRLB_CMD_LR_Val            _U_(0x11)                                            /**< (NVMCTRL_CTRLB) Lock Region - Locks the region containing the address location in the ADDR register.  */
#define   NVMCTRL_CTRLB_CMD_UR_Val            _U_(0x12)                                            /**< (NVMCTRL_CTRLB) Unlock Region - Unlocks the region containing the address location in the ADDR register.  */
#define   NVMCTRL_CTRLB_CMD_SPRM_Val          _U_(0x13)                                            /**< (NVMCTRL_CTRLB) Sets the power reduction mode.  */
#define   NVMCTRL_CTRLB_CMD_CPRM_Val          _U_(0x14)                                            /**< (NVMCTRL_CTRLB) Clears the power reduction mode.  */
#define   NVMCTRL_CTRLB_CMD_PBC_Val           _U_(0x15)                                            /**< (NVMCTRL_CTRLB) Page Buffer Clear - Clears the page buffer.  */
#define   NVMCTRL_CTRLB_CMD_SSB_Val           _U_(0x16)                                            /**< (NVMCTRL_CTRLB) Set Security Bit  */
#define   NVMCTRL_CTRLB_CMD_BKSWRST_Val       _U_(0x17)                                            /**< (NVMCTRL_CTRLB) Bank swap and system reset, if SMEE is used also reallocate SMEE data into the opposite BANK  */
#define   NVMCTRL_CTRLB_CMD_CELCK_Val         _U_(0x18)                                            /**< (NVMCTRL_CTRLB) Chip Erase Lock - DSU.CE command is not available  */
#define   NVMCTRL_CTRLB_CMD_CEULCK_Val        _U_(0x19)                                            /**< (NVMCTRL_CTRLB) Chip Erase Unlock - DSU.CE command is available  */
#define   NVMCTRL_CTRLB_CMD_SBPDIS_Val        _U_(0x1A)                                            /**< (NVMCTRL_CTRLB) Sets STATUS.BPDIS, Boot loader protection is discarded until CBPDIS is issued or next start-up sequence  */
#define   NVMCTRL_CTRLB_CMD_CBPDIS_Val        _U_(0x1B)                                            /**< (NVMCTRL_CTRLB) Clears STATUS.BPDIS, Boot loader protection is not discarded  */
#define   NVMCTRL_CTRLB_CMD_ASEES0_Val        _U_(0x30)                                            /**< (NVMCTRL_CTRLB) Activate SmartEEPROM Sector 0, deactivate Sector 1  */
#define   NVMCTRL_CTRLB_CMD_ASEES1_Val        _U_(0x31)                                            /**< (NVMCTRL_CTRLB) Activate SmartEEPROM Sector 1, deactivate Sector 0  */
#define   NVMCTRL_CTRLB_CMD_SEERALOC_Val      _U_(0x32)                                            /**< (NVMCTRL_CTRLB) Starts SmartEEPROM sector reallocation algorithm  */
#define   NVMCTRL_CTRLB_CMD_SEEFLUSH_Val      _U_(0x33)                                            /**< (NVMCTRL_CTRLB) Flush SMEE data when in buffered mode  */
#define   NVMCTRL_CTRLB_CMD_LSEE_Val          _U_(0x34)                                            /**< (NVMCTRL_CTRLB) Lock access to SmartEEPROM data from any mean  */
#define   NVMCTRL_CTRLB_CMD_USEE_Val          _U_(0x35)                                            /**< (NVMCTRL_CTRLB) Unlock access to SmartEEPROM data  */
#define   NVMCTRL_CTRLB_CMD_LSEER_Val         _U_(0x36)                                            /**< (NVMCTRL_CTRLB) Lock access to the SmartEEPROM Register Address Space (above 64KB)  */
#define   NVMCTRL_CTRLB_CMD_USEER_Val         _U_(0x37)                                            /**< (NVMCTRL_CTRLB) Unlock access to the SmartEEPROM Register Address Space (above 64KB)  */
#define NVMCTRL_CTRLB_CMD_EP                  (NVMCTRL_CTRLB_CMD_EP_Val << NVMCTRL_CTRLB_CMD_Pos)  /**< (NVMCTRL_CTRLB) Erase Page - Only supported in the USER and AUX pages. Position  */
#define NVMCTRL_CTRLB_CMD_EB                  (NVMCTRL_CTRLB_CMD_EB_Val << NVMCTRL_CTRLB_CMD_Pos)  /**< (NVMCTRL_CTRLB) Erase Block - Erases the block addressed by the ADDR register, not supported in the user page Position  */
#define NVMCTRL_CTRLB_CMD_WP                  (NVMCTRL_CTRLB_CMD_WP_Val << NVMCTRL_CTRLB_CMD_Pos)  /**< (NVMCTRL_CTRLB) Write Page - Writes the contents of the page buffer to the page addressed by the ADDR register, not supported in the user page Position  */
#define NVMCTRL_CTRLB_CMD_WQW                 (NVMCTRL_CTRLB_CMD_WQW_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Write Quad Word - Writes a 128-bit word at the location addressed by the ADDR register. Position  */
#define NVMCTRL_CTRLB_CMD_SWRST               (NVMCTRL_CTRLB_CMD_SWRST_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Software Reset - Power-Cycle the NVM memory and replay the device automatic calibration procedure and resets the module configuration registers Position  */
#define NVMCTRL_CTRLB_CMD_LR                  (NVMCTRL_CTRLB_CMD_LR_Val << NVMCTRL_CTRLB_CMD_Pos)  /**< (NVMCTRL_CTRLB) Lock Region - Locks the region containing the address location in the ADDR register. Position  */
#define NVMCTRL_CTRLB_CMD_UR                  (NVMCTRL_CTRLB_CMD_UR_Val << NVMCTRL_CTRLB_CMD_Pos)  /**< (NVMCTRL_CTRLB) Unlock Region - Unlocks the region containing the address location in the ADDR register. Position  */
#define NVMCTRL_CTRLB_CMD_SPRM                (NVMCTRL_CTRLB_CMD_SPRM_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Sets the power reduction mode. Position  */
#define NVMCTRL_CTRLB_CMD_CPRM                (NVMCTRL_CTRLB_CMD_CPRM_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Clears the power reduction mode. Position  */
#define NVMCTRL_CTRLB_CMD_PBC                 (NVMCTRL_CTRLB_CMD_PBC_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Page Buffer Clear - Clears the page buffer. Position  */
#define NVMCTRL_CTRLB_CMD_SSB                 (NVMCTRL_CTRLB_CMD_SSB_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Set Security Bit Position  */
#define NVMCTRL_CTRLB_CMD_BKSWRST             (NVMCTRL_CTRLB_CMD_BKSWRST_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Bank swap and system reset, if SMEE is used also reallocate SMEE data into the opposite BANK Position  */
#define NVMCTRL_CTRLB_CMD_CELCK               (NVMCTRL_CTRLB_CMD_CELCK_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Chip Erase Lock - DSU.CE command is not available Position  */
#define NVMCTRL_CTRLB_CMD_CEULCK              (NVMCTRL_CTRLB_CMD_CEULCK_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Chip Erase Unlock - DSU.CE command is available Position  */
#define NVMCTRL_CTRLB_CMD_SBPDIS              (NVMCTRL_CTRLB_CMD_SBPDIS_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Sets STATUS.BPDIS, Boot loader protection is discarded until CBPDIS is issued or next start-up sequence Position  */
#define NVMCTRL_CTRLB_CMD_CBPDIS              (NVMCTRL_CTRLB_CMD_CBPDIS_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Clears STATUS.BPDIS, Boot loader protection is not discarded Position  */
#define NVMCTRL_CTRLB_CMD_ASEES0              (NVMCTRL_CTRLB_CMD_ASEES0_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Activate SmartEEPROM Sector 0, deactivate Sector 1 Position  */
#define NVMCTRL_CTRLB_CMD_ASEES1              (NVMCTRL_CTRLB_CMD_ASEES1_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Activate SmartEEPROM Sector 1, deactivate Sector 0 Position  */
#define NVMCTRL_CTRLB_CMD_SEERALOC            (NVMCTRL_CTRLB_CMD_SEERALOC_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Starts SmartEEPROM sector reallocation algorithm Position  */
#define NVMCTRL_CTRLB_CMD_SEEFLUSH            (NVMCTRL_CTRLB_CMD_SEEFLUSH_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Flush SMEE data when in buffered mode Position  */
#define NVMCTRL_CTRLB_CMD_LSEE                (NVMCTRL_CTRLB_CMD_LSEE_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Lock access to SmartEEPROM data from any mean Position  */
#define NVMCTRL_CTRLB_CMD_USEE                (NVMCTRL_CTRLB_CMD_USEE_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Unlock access to SmartEEPROM data Position  */
#define NVMCTRL_CTRLB_CMD_LSEER               (NVMCTRL_CTRLB_CMD_LSEER_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Lock access to the SmartEEPROM Register Address Space (above 64KB) Position  */
#define NVMCTRL_CTRLB_CMD_USEER               (NVMCTRL_CTRLB_CMD_USEER_Val << NVMCTRL_CTRLB_CMD_Pos) /**< (NVMCTRL_CTRLB) Unlock access to the SmartEEPROM Register Address Space (above 64KB) Position  */
#define NVMCTRL_CTRLB_CMDEX_Pos               _U_(8)                                               /**< (NVMCTRL_CTRLB) Command Execution Position */
#define NVMCTRL_CTRLB_CMDEX_Msk               (_U_(0xFF) << NVMCTRL_CTRLB_CMDEX_Pos)               /**< (NVMCTRL_CTRLB) Command Execution Mask */
#define NVMCTRL_CTRLB_CMDEX(value)            (NVMCTRL_CTRLB_CMDEX_Msk & ((value) << NVMCTRL_CTRLB_CMDEX_Pos))
#define   NVMCTRL_CTRLB_CMDEX_KEY_Val         _U_(0xA5)                                            /**< (NVMCTRL_CTRLB) Execution Key  */
#define NVMCTRL_CTRLB_CMDEX_KEY               (NVMCTRL_CTRLB_CMDEX_KEY_Val << NVMCTRL_CTRLB_CMDEX_Pos) /**< (NVMCTRL_CTRLB) Execution Key Position  */
#define NVMCTRL_CTRLB_Msk                     _U_(0xFF7F)                                          /**< (NVMCTRL_CTRLB) Register Mask  */


/* -------- NVMCTRL_PARAM : (NVMCTRL Offset: 0x08) ( R/ 32) NVM Parameter -------- */
#define NVMCTRL_PARAM_RESETVALUE              _U_(0x60000)                                         /**<  (NVMCTRL_PARAM) NVM Parameter  Reset Value */

#define NVMCTRL_PARAM_NVMP_Pos                _U_(0)                                               /**< (NVMCTRL_PARAM) NVM Pages Position */
#define NVMCTRL_PARAM_NVMP_Msk                (_U_(0xFFFF) << NVMCTRL_PARAM_NVMP_Pos)              /**< (NVMCTRL_PARAM) NVM Pages Mask */
#define NVMCTRL_PARAM_NVMP(value)             (NVMCTRL_PARAM_NVMP_Msk & ((value) << NVMCTRL_PARAM_NVMP_Pos))
#define NVMCTRL_PARAM_PSZ_Pos                 _U_(16)                                              /**< (NVMCTRL_PARAM) Page Size Position */
#define NVMCTRL_PARAM_PSZ_Msk                 (_U_(0x7) << NVMCTRL_PARAM_PSZ_Pos)                  /**< (NVMCTRL_PARAM) Page Size Mask */
#define NVMCTRL_PARAM_PSZ(value)              (NVMCTRL_PARAM_PSZ_Msk & ((value) << NVMCTRL_PARAM_PSZ_Pos))
#define   NVMCTRL_PARAM_PSZ_8_Val             _U_(0x0)                                             /**< (NVMCTRL_PARAM) 8 bytes  */
#define   NVMCTRL_PARAM_PSZ_16_Val            _U_(0x1)                                             /**< (NVMCTRL_PARAM) 16 bytes  */
#define   NVMCTRL_PARAM_PSZ_32_Val            _U_(0x2)                                             /**< (NVMCTRL_PARAM) 32 bytes  */
#define   NVMCTRL_PARAM_PSZ_64_Val            _U_(0x3)                                             /**< (NVMCTRL_PARAM) 64 bytes  */
#define   NVMCTRL_PARAM_PSZ_128_Val           _U_(0x4)                                             /**< (NVMCTRL_PARAM) 128 bytes  */
#define   NVMCTRL_PARAM_PSZ_256_Val           _U_(0x5)                                             /**< (NVMCTRL_PARAM) 256 bytes  */
#define   NVMCTRL_PARAM_PSZ_512_Val           _U_(0x6)                                             /**< (NVMCTRL_PARAM) 512 bytes  */
#define   NVMCTRL_PARAM_PSZ_1024_Val          _U_(0x7)                                             /**< (NVMCTRL_PARAM) 1024 bytes  */
#define NVMCTRL_PARAM_PSZ_8                   (NVMCTRL_PARAM_PSZ_8_Val << NVMCTRL_PARAM_PSZ_Pos)   /**< (NVMCTRL_PARAM) 8 bytes Position  */
#define NVMCTRL_PARAM_PSZ_16                  (NVMCTRL_PARAM_PSZ_16_Val << NVMCTRL_PARAM_PSZ_Pos)  /**< (NVMCTRL_PARAM) 16 bytes Position  */
#define NVMCTRL_PARAM_PSZ_32                  (NVMCTRL_PARAM_PSZ_32_Val << NVMCTRL_PARAM_PSZ_Pos)  /**< (NVMCTRL_PARAM) 32 bytes Position  */
#define NVMCTRL_PARAM_PSZ_64                  (NVMCTRL_PARAM_PSZ_64_Val << NVMCTRL_PARAM_PSZ_Pos)  /**< (NVMCTRL_PARAM) 64 bytes Position  */
#define NVMCTRL_PARAM_PSZ_128                 (NVMCTRL_PARAM_PSZ_128_Val << NVMCTRL_PARAM_PSZ_Pos) /**< (NVMCTRL_PARAM) 128 bytes Position  */
#define NVMCTRL_PARAM_PSZ_256                 (NVMCTRL_PARAM_PSZ_256_Val << NVMCTRL_PARAM_PSZ_Pos) /**< (NVMCTRL_PARAM) 256 bytes Position  */
#define NVMCTRL_PARAM_PSZ_512                 (NVMCTRL_PARAM_PSZ_512_Val << NVMCTRL_PARAM_PSZ_Pos) /**< (NVMCTRL_PARAM) 512 bytes Position  */
#define NVMCTRL_PARAM_PSZ_1024                (NVMCTRL_PARAM_PSZ_1024_Val << NVMCTRL_PARAM_PSZ_Pos) /**< (NVMCTRL_PARAM) 1024 bytes Position  */
#define NVMCTRL_PARAM_SEE_Pos                 _U_(31)                                              /**< (NVMCTRL_PARAM) SmartEEPROM Supported Position */
#define NVMCTRL_PARAM_SEE_Msk                 (_U_(0x1) << NVMCTRL_PARAM_SEE_Pos)                  /**< (NVMCTRL_PARAM) SmartEEPROM Supported Mask */
#define NVMCTRL_PARAM_SEE(value)              (NVMCTRL_PARAM_SEE_Msk & ((value) << NVMCTRL_PARAM_SEE_Pos))
#define   NVMCTRL_PARAM_SEE_A_Val             _U_(0xA)                                             /**< (NVMCTRL_PARAM) 163840 bytes  */
#define   NVMCTRL_PARAM_SEE_9_Val             _U_(0x9)                                             /**< (NVMCTRL_PARAM) 147456 bytes  */
#define   NVMCTRL_PARAM_SEE_8_Val             _U_(0x8)                                             /**< (NVMCTRL_PARAM) 131072 bytes  */
#define   NVMCTRL_PARAM_SEE_7_Val             _U_(0x7)                                             /**< (NVMCTRL_PARAM) 114688 bytes  */
#define   NVMCTRL_PARAM_SEE_6_Val             _U_(0x6)                                             /**< (NVMCTRL_PARAM) 98304 bytes  */
#define   NVMCTRL_PARAM_SEE_5_Val             _U_(0x5)                                             /**< (NVMCTRL_PARAM) 81920 bytes  */
#define   NVMCTRL_PARAM_SEE_4_Val             _U_(0x4)                                             /**< (NVMCTRL_PARAM) 65536 bytes  */
#define   NVMCTRL_PARAM_SEE_3_Val             _U_(0x3)                                             /**< (NVMCTRL_PARAM) 49152 bytes  */
#define   NVMCTRL_PARAM_SEE_2_Val             _U_(0x2)                                             /**< (NVMCTRL_PARAM) 32768 bytes  */
#define   NVMCTRL_PARAM_SEE_1_Val             _U_(0x1)                                             /**< (NVMCTRL_PARAM) 16384 bytes  */
#define   NVMCTRL_PARAM_SEE_0_Val             _U_(0x0)                                             /**< (NVMCTRL_PARAM) 0 bytes  */
#define NVMCTRL_PARAM_SEE_A                   (NVMCTRL_PARAM_SEE_A_Val << NVMCTRL_PARAM_SEE_Pos)   /**< (NVMCTRL_PARAM) 163840 bytes Position  */
#define NVMCTRL_PARAM_SEE_9                   (NVMCTRL_PARAM_SEE_9_Val << NVMCTRL_PARAM_SEE_Pos)   /**< (NVMCTRL_PARAM) 147456 bytes Position  */
#define NVMCTRL_PARAM_SEE_8                   (NVMCTRL_PARAM_SEE_8_Val << NVMCTRL_PARAM_SEE_Pos)   /**< (NVMCTRL_PARAM) 131072 bytes Position  */
#define NVMCTRL_PARAM_SEE_7                   (NVMCTRL_PARAM_SEE_7_Val << NVMCTRL_PARAM_SEE_Pos)   /**< (NVMCTRL_PARAM) 114688 bytes Position  */
#define NVMCTRL_PARAM_SEE_6                   (NVMCTRL_PARAM_SEE_6_Val << NVMCTRL_PARAM_SEE_Pos)   /**< (NVMCTRL_PARAM) 98304 bytes Position  */
#define NVMCTRL_PARAM_SEE_5                   (NVMCTRL_PARAM_SEE_5_Val << NVMCTRL_PARAM_SEE_Pos)   /**< (NVMCTRL_PARAM) 81920 bytes Position  */
#define NVMCTRL_PARAM_SEE_4                   (NVMCTRL_PARAM_SEE_4_Val << NVMCTRL_PARAM_SEE_Pos)   /**< (NVMCTRL_PARAM) 65536 bytes Position  */
#define NVMCTRL_PARAM_SEE_3                   (NVMCTRL_PARAM_SEE_3_Val << NVMCTRL_PARAM_SEE_Pos)   /**< (NVMCTRL_PARAM) 49152 bytes Position  */
#define NVMCTRL_PARAM_SEE_2                   (NVMCTRL_PARAM_SEE_2_Val << NVMCTRL_PARAM_SEE_Pos)   /**< (NVMCTRL_PARAM) 32768 bytes Position  */
#define NVMCTRL_PARAM_SEE_1                   (NVMCTRL_PARAM_SEE_1_Val << NVMCTRL_PARAM_SEE_Pos)   /**< (NVMCTRL_PARAM) 16384 bytes Position  */
#define NVMCTRL_PARAM_SEE_0                   (NVMCTRL_PARAM_SEE_0_Val << NVMCTRL_PARAM_SEE_Pos)   /**< (NVMCTRL_PARAM) 0 bytes Position  */
#define NVMCTRL_PARAM_Msk                     _U_(0x8007FFFF)                                      /**< (NVMCTRL_PARAM) Register Mask  */


/* -------- NVMCTRL_INTENCLR : (NVMCTRL Offset: 0x0C) (R/W 16) Interrupt Enable Clear -------- */
#define NVMCTRL_INTENCLR_RESETVALUE           _U_(0x00)                                            /**<  (NVMCTRL_INTENCLR) Interrupt Enable Clear  Reset Value */

#define NVMCTRL_INTENCLR_DONE_Pos             _U_(0)                                               /**< (NVMCTRL_INTENCLR) Command Done Interrupt Clear Position */
#define NVMCTRL_INTENCLR_DONE_Msk             (_U_(0x1) << NVMCTRL_INTENCLR_DONE_Pos)              /**< (NVMCTRL_INTENCLR) Command Done Interrupt Clear Mask */
#define NVMCTRL_INTENCLR_DONE(value)          (NVMCTRL_INTENCLR_DONE_Msk & ((value) << NVMCTRL_INTENCLR_DONE_Pos))
#define NVMCTRL_INTENCLR_ADDRE_Pos            _U_(1)                                               /**< (NVMCTRL_INTENCLR) Address Error Position */
#define NVMCTRL_INTENCLR_ADDRE_Msk            (_U_(0x1) << NVMCTRL_INTENCLR_ADDRE_Pos)             /**< (NVMCTRL_INTENCLR) Address Error Mask */
#define NVMCTRL_INTENCLR_ADDRE(value)         (NVMCTRL_INTENCLR_ADDRE_Msk & ((value) << NVMCTRL_INTENCLR_ADDRE_Pos))
#define NVMCTRL_INTENCLR_PROGE_Pos            _U_(2)                                               /**< (NVMCTRL_INTENCLR) Programming Error Interrupt Clear Position */
#define NVMCTRL_INTENCLR_PROGE_Msk            (_U_(0x1) << NVMCTRL_INTENCLR_PROGE_Pos)             /**< (NVMCTRL_INTENCLR) Programming Error Interrupt Clear Mask */
#define NVMCTRL_INTENCLR_PROGE(value)         (NVMCTRL_INTENCLR_PROGE_Msk & ((value) << NVMCTRL_INTENCLR_PROGE_Pos))
#define NVMCTRL_INTENCLR_LOCKE_Pos            _U_(3)                                               /**< (NVMCTRL_INTENCLR) Lock Error Interrupt Clear Position */
#define NVMCTRL_INTENCLR_LOCKE_Msk            (_U_(0x1) << NVMCTRL_INTENCLR_LOCKE_Pos)             /**< (NVMCTRL_INTENCLR) Lock Error Interrupt Clear Mask */
#define NVMCTRL_INTENCLR_LOCKE(value)         (NVMCTRL_INTENCLR_LOCKE_Msk & ((value) << NVMCTRL_INTENCLR_LOCKE_Pos))
#define NVMCTRL_INTENCLR_ECCSE_Pos            _U_(4)                                               /**< (NVMCTRL_INTENCLR) ECC Single Error Interrupt Clear Position */
#define NVMCTRL_INTENCLR_ECCSE_Msk            (_U_(0x1) << NVMCTRL_INTENCLR_ECCSE_Pos)             /**< (NVMCTRL_INTENCLR) ECC Single Error Interrupt Clear Mask */
#define NVMCTRL_INTENCLR_ECCSE(value)         (NVMCTRL_INTENCLR_ECCSE_Msk & ((value) << NVMCTRL_INTENCLR_ECCSE_Pos))
#define NVMCTRL_INTENCLR_ECCDE_Pos            _U_(5)                                               /**< (NVMCTRL_INTENCLR) ECC Dual Error Interrupt Clear Position */
#define NVMCTRL_INTENCLR_ECCDE_Msk            (_U_(0x1) << NVMCTRL_INTENCLR_ECCDE_Pos)             /**< (NVMCTRL_INTENCLR) ECC Dual Error Interrupt Clear Mask */
#define NVMCTRL_INTENCLR_ECCDE(value)         (NVMCTRL_INTENCLR_ECCDE_Msk & ((value) << NVMCTRL_INTENCLR_ECCDE_Pos))
#define NVMCTRL_INTENCLR_NVME_Pos             _U_(6)                                               /**< (NVMCTRL_INTENCLR) NVM Error Interrupt Clear Position */
#define NVMCTRL_INTENCLR_NVME_Msk             (_U_(0x1) << NVMCTRL_INTENCLR_NVME_Pos)              /**< (NVMCTRL_INTENCLR) NVM Error Interrupt Clear Mask */
#define NVMCTRL_INTENCLR_NVME(value)          (NVMCTRL_INTENCLR_NVME_Msk & ((value) << NVMCTRL_INTENCLR_NVME_Pos))
#define NVMCTRL_INTENCLR_SUSP_Pos             _U_(7)                                               /**< (NVMCTRL_INTENCLR) Suspended Write Or Erase Interrupt Clear Position */
#define NVMCTRL_INTENCLR_SUSP_Msk             (_U_(0x1) << NVMCTRL_INTENCLR_SUSP_Pos)              /**< (NVMCTRL_INTENCLR) Suspended Write Or Erase Interrupt Clear Mask */
#define NVMCTRL_INTENCLR_SUSP(value)          (NVMCTRL_INTENCLR_SUSP_Msk & ((value) << NVMCTRL_INTENCLR_SUSP_Pos))
#define NVMCTRL_INTENCLR_SEESFULL_Pos         _U_(8)                                               /**< (NVMCTRL_INTENCLR) Active SEES Full Interrupt Clear Position */
#define NVMCTRL_INTENCLR_SEESFULL_Msk         (_U_(0x1) << NVMCTRL_INTENCLR_SEESFULL_Pos)          /**< (NVMCTRL_INTENCLR) Active SEES Full Interrupt Clear Mask */
#define NVMCTRL_INTENCLR_SEESFULL(value)      (NVMCTRL_INTENCLR_SEESFULL_Msk & ((value) << NVMCTRL_INTENCLR_SEESFULL_Pos))
#define NVMCTRL_INTENCLR_SEESOVF_Pos          _U_(9)                                               /**< (NVMCTRL_INTENCLR) Active SEES Overflow Interrupt Clear Position */
#define NVMCTRL_INTENCLR_SEESOVF_Msk          (_U_(0x1) << NVMCTRL_INTENCLR_SEESOVF_Pos)           /**< (NVMCTRL_INTENCLR) Active SEES Overflow Interrupt Clear Mask */
#define NVMCTRL_INTENCLR_SEESOVF(value)       (NVMCTRL_INTENCLR_SEESOVF_Msk & ((value) << NVMCTRL_INTENCLR_SEESOVF_Pos))
#define NVMCTRL_INTENCLR_SEEWRC_Pos           _U_(10)                                              /**< (NVMCTRL_INTENCLR) SEE Write Completed Interrupt Clear Position */
#define NVMCTRL_INTENCLR_SEEWRC_Msk           (_U_(0x1) << NVMCTRL_INTENCLR_SEEWRC_Pos)            /**< (NVMCTRL_INTENCLR) SEE Write Completed Interrupt Clear Mask */
#define NVMCTRL_INTENCLR_SEEWRC(value)        (NVMCTRL_INTENCLR_SEEWRC_Msk & ((value) << NVMCTRL_INTENCLR_SEEWRC_Pos))
#define NVMCTRL_INTENCLR_Msk                  _U_(0x07FF)                                          /**< (NVMCTRL_INTENCLR) Register Mask  */


/* -------- NVMCTRL_INTENSET : (NVMCTRL Offset: 0x0E) (R/W 16) Interrupt Enable Set -------- */
#define NVMCTRL_INTENSET_RESETVALUE           _U_(0x00)                                            /**<  (NVMCTRL_INTENSET) Interrupt Enable Set  Reset Value */

#define NVMCTRL_INTENSET_DONE_Pos             _U_(0)                                               /**< (NVMCTRL_INTENSET) Command Done Interrupt Enable Position */
#define NVMCTRL_INTENSET_DONE_Msk             (_U_(0x1) << NVMCTRL_INTENSET_DONE_Pos)              /**< (NVMCTRL_INTENSET) Command Done Interrupt Enable Mask */
#define NVMCTRL_INTENSET_DONE(value)          (NVMCTRL_INTENSET_DONE_Msk & ((value) << NVMCTRL_INTENSET_DONE_Pos))
#define NVMCTRL_INTENSET_ADDRE_Pos            _U_(1)                                               /**< (NVMCTRL_INTENSET) Address Error Interrupt Enable Position */
#define NVMCTRL_INTENSET_ADDRE_Msk            (_U_(0x1) << NVMCTRL_INTENSET_ADDRE_Pos)             /**< (NVMCTRL_INTENSET) Address Error Interrupt Enable Mask */
#define NVMCTRL_INTENSET_ADDRE(value)         (NVMCTRL_INTENSET_ADDRE_Msk & ((value) << NVMCTRL_INTENSET_ADDRE_Pos))
#define NVMCTRL_INTENSET_PROGE_Pos            _U_(2)                                               /**< (NVMCTRL_INTENSET) Programming Error Interrupt Enable Position */
#define NVMCTRL_INTENSET_PROGE_Msk            (_U_(0x1) << NVMCTRL_INTENSET_PROGE_Pos)             /**< (NVMCTRL_INTENSET) Programming Error Interrupt Enable Mask */
#define NVMCTRL_INTENSET_PROGE(value)         (NVMCTRL_INTENSET_PROGE_Msk & ((value) << NVMCTRL_INTENSET_PROGE_Pos))
#define NVMCTRL_INTENSET_LOCKE_Pos            _U_(3)                                               /**< (NVMCTRL_INTENSET) Lock Error Interrupt Enable Position */
#define NVMCTRL_INTENSET_LOCKE_Msk            (_U_(0x1) << NVMCTRL_INTENSET_LOCKE_Pos)             /**< (NVMCTRL_INTENSET) Lock Error Interrupt Enable Mask */
#define NVMCTRL_INTENSET_LOCKE(value)         (NVMCTRL_INTENSET_LOCKE_Msk & ((value) << NVMCTRL_INTENSET_LOCKE_Pos))
#define NVMCTRL_INTENSET_ECCSE_Pos            _U_(4)                                               /**< (NVMCTRL_INTENSET) ECC Single Error Interrupt Enable Position */
#define NVMCTRL_INTENSET_ECCSE_Msk            (_U_(0x1) << NVMCTRL_INTENSET_ECCSE_Pos)             /**< (NVMCTRL_INTENSET) ECC Single Error Interrupt Enable Mask */
#define NVMCTRL_INTENSET_ECCSE(value)         (NVMCTRL_INTENSET_ECCSE_Msk & ((value) << NVMCTRL_INTENSET_ECCSE_Pos))
#define NVMCTRL_INTENSET_ECCDE_Pos            _U_(5)                                               /**< (NVMCTRL_INTENSET) ECC Dual Error Interrupt Enable Position */
#define NVMCTRL_INTENSET_ECCDE_Msk            (_U_(0x1) << NVMCTRL_INTENSET_ECCDE_Pos)             /**< (NVMCTRL_INTENSET) ECC Dual Error Interrupt Enable Mask */
#define NVMCTRL_INTENSET_ECCDE(value)         (NVMCTRL_INTENSET_ECCDE_Msk & ((value) << NVMCTRL_INTENSET_ECCDE_Pos))
#define NVMCTRL_INTENSET_NVME_Pos             _U_(6)                                               /**< (NVMCTRL_INTENSET) NVM Error Interrupt Enable Position */
#define NVMCTRL_INTENSET_NVME_Msk             (_U_(0x1) << NVMCTRL_INTENSET_NVME_Pos)              /**< (NVMCTRL_INTENSET) NVM Error Interrupt Enable Mask */
#define NVMCTRL_INTENSET_NVME(value)          (NVMCTRL_INTENSET_NVME_Msk & ((value) << NVMCTRL_INTENSET_NVME_Pos))
#define NVMCTRL_INTENSET_SUSP_Pos             _U_(7)                                               /**< (NVMCTRL_INTENSET) Suspended Write Or Erase  Interrupt Enable Position */
#define NVMCTRL_INTENSET_SUSP_Msk             (_U_(0x1) << NVMCTRL_INTENSET_SUSP_Pos)              /**< (NVMCTRL_INTENSET) Suspended Write Or Erase  Interrupt Enable Mask */
#define NVMCTRL_INTENSET_SUSP(value)          (NVMCTRL_INTENSET_SUSP_Msk & ((value) << NVMCTRL_INTENSET_SUSP_Pos))
#define NVMCTRL_INTENSET_SEESFULL_Pos         _U_(8)                                               /**< (NVMCTRL_INTENSET) Active SEES Full Interrupt Enable Position */
#define NVMCTRL_INTENSET_SEESFULL_Msk         (_U_(0x1) << NVMCTRL_INTENSET_SEESFULL_Pos)          /**< (NVMCTRL_INTENSET) Active SEES Full Interrupt Enable Mask */
#define NVMCTRL_INTENSET_SEESFULL(value)      (NVMCTRL_INTENSET_SEESFULL_Msk & ((value) << NVMCTRL_INTENSET_SEESFULL_Pos))
#define NVMCTRL_INTENSET_SEESOVF_Pos          _U_(9)                                               /**< (NVMCTRL_INTENSET) Active SEES Overflow Interrupt Enable Position */
#define NVMCTRL_INTENSET_SEESOVF_Msk          (_U_(0x1) << NVMCTRL_INTENSET_SEESOVF_Pos)           /**< (NVMCTRL_INTENSET) Active SEES Overflow Interrupt Enable Mask */
#define NVMCTRL_INTENSET_SEESOVF(value)       (NVMCTRL_INTENSET_SEESOVF_Msk & ((value) << NVMCTRL_INTENSET_SEESOVF_Pos))
#define NVMCTRL_INTENSET_SEEWRC_Pos           _U_(10)                                              /**< (NVMCTRL_INTENSET) SEE Write Completed Interrupt Enable Position */
#define NVMCTRL_INTENSET_SEEWRC_Msk           (_U_(0x1) << NVMCTRL_INTENSET_SEEWRC_Pos)            /**< (NVMCTRL_INTENSET) SEE Write Completed Interrupt Enable Mask */
#define NVMCTRL_INTENSET_SEEWRC(value)        (NVMCTRL_INTENSET_SEEWRC_Msk & ((value) << NVMCTRL_INTENSET_SEEWRC_Pos))
#define NVMCTRL_INTENSET_Msk                  _U_(0x07FF)                                          /**< (NVMCTRL_INTENSET) Register Mask  */


/* -------- NVMCTRL_INTFLAG : (NVMCTRL Offset: 0x10) (R/W 16) Interrupt Flag Status and Clear -------- */
#define NVMCTRL_INTFLAG_RESETVALUE            _U_(0x00)                                            /**<  (NVMCTRL_INTFLAG) Interrupt Flag Status and Clear  Reset Value */

#define NVMCTRL_INTFLAG_DONE_Pos              _U_(0)                                               /**< (NVMCTRL_INTFLAG) Command Done Position */
#define NVMCTRL_INTFLAG_DONE_Msk              (_U_(0x1) << NVMCTRL_INTFLAG_DONE_Pos)               /**< (NVMCTRL_INTFLAG) Command Done Mask */
#define NVMCTRL_INTFLAG_DONE(value)           (NVMCTRL_INTFLAG_DONE_Msk & ((value) << NVMCTRL_INTFLAG_DONE_Pos))
#define NVMCTRL_INTFLAG_ADDRE_Pos             _U_(1)                                               /**< (NVMCTRL_INTFLAG) Address Error Position */
#define NVMCTRL_INTFLAG_ADDRE_Msk             (_U_(0x1) << NVMCTRL_INTFLAG_ADDRE_Pos)              /**< (NVMCTRL_INTFLAG) Address Error Mask */
#define NVMCTRL_INTFLAG_ADDRE(value)          (NVMCTRL_INTFLAG_ADDRE_Msk & ((value) << NVMCTRL_INTFLAG_ADDRE_Pos))
#define NVMCTRL_INTFLAG_PROGE_Pos             _U_(2)                                               /**< (NVMCTRL_INTFLAG) Programming Error Position */
#define NVMCTRL_INTFLAG_PROGE_Msk             (_U_(0x1) << NVMCTRL_INTFLAG_PROGE_Pos)              /**< (NVMCTRL_INTFLAG) Programming Error Mask */
#define NVMCTRL_INTFLAG_PROGE(value)          (NVMCTRL_INTFLAG_PROGE_Msk & ((value) << NVMCTRL_INTFLAG_PROGE_Pos))
#define NVMCTRL_INTFLAG_LOCKE_Pos             _U_(3)                                               /**< (NVMCTRL_INTFLAG) Lock Error Position */
#define NVMCTRL_INTFLAG_LOCKE_Msk             (_U_(0x1) << NVMCTRL_INTFLAG_LOCKE_Pos)              /**< (NVMCTRL_INTFLAG) Lock Error Mask */
#define NVMCTRL_INTFLAG_LOCKE(value)          (NVMCTRL_INTFLAG_LOCKE_Msk & ((value) << NVMCTRL_INTFLAG_LOCKE_Pos))
#define NVMCTRL_INTFLAG_ECCSE_Pos             _U_(4)                                               /**< (NVMCTRL_INTFLAG) ECC Single Error Position */
#define NVMCTRL_INTFLAG_ECCSE_Msk             (_U_(0x1) << NVMCTRL_INTFLAG_ECCSE_Pos)              /**< (NVMCTRL_INTFLAG) ECC Single Error Mask */
#define NVMCTRL_INTFLAG_ECCSE(value)          (NVMCTRL_INTFLAG_ECCSE_Msk & ((value) << NVMCTRL_INTFLAG_ECCSE_Pos))
#define NVMCTRL_INTFLAG_ECCDE_Pos             _U_(5)                                               /**< (NVMCTRL_INTFLAG) ECC Dual Error Position */
#define NVMCTRL_INTFLAG_ECCDE_Msk             (_U_(0x1) << NVMCTRL_INTFLAG_ECCDE_Pos)              /**< (NVMCTRL_INTFLAG) ECC Dual Error Mask */
#define NVMCTRL_INTFLAG_ECCDE(value)          (NVMCTRL_INTFLAG_ECCDE_Msk & ((value) << NVMCTRL_INTFLAG_ECCDE_Pos))
#define NVMCTRL_INTFLAG_NVME_Pos              _U_(6)                                               /**< (NVMCTRL_INTFLAG) NVM Error Position */
#define NVMCTRL_INTFLAG_NVME_Msk              (_U_(0x1) << NVMCTRL_INTFLAG_NVME_Pos)               /**< (NVMCTRL_INTFLAG) NVM Error Mask */
#define NVMCTRL_INTFLAG_NVME(value)           (NVMCTRL_INTFLAG_NVME_Msk & ((value) << NVMCTRL_INTFLAG_NVME_Pos))
#define NVMCTRL_INTFLAG_SUSP_Pos              _U_(7)                                               /**< (NVMCTRL_INTFLAG) Suspended Write Or Erase Operation Position */
#define NVMCTRL_INTFLAG_SUSP_Msk              (_U_(0x1) << NVMCTRL_INTFLAG_SUSP_Pos)               /**< (NVMCTRL_INTFLAG) Suspended Write Or Erase Operation Mask */
#define NVMCTRL_INTFLAG_SUSP(value)           (NVMCTRL_INTFLAG_SUSP_Msk & ((value) << NVMCTRL_INTFLAG_SUSP_Pos))
#define NVMCTRL_INTFLAG_SEESFULL_Pos          _U_(8)                                               /**< (NVMCTRL_INTFLAG) Active SEES Full Position */
#define NVMCTRL_INTFLAG_SEESFULL_Msk          (_U_(0x1) << NVMCTRL_INTFLAG_SEESFULL_Pos)           /**< (NVMCTRL_INTFLAG) Active SEES Full Mask */
#define NVMCTRL_INTFLAG_SEESFULL(value)       (NVMCTRL_INTFLAG_SEESFULL_Msk & ((value) << NVMCTRL_INTFLAG_SEESFULL_Pos))
#define NVMCTRL_INTFLAG_SEESOVF_Pos           _U_(9)                                               /**< (NVMCTRL_INTFLAG) Active SEES Overflow Position */
#define NVMCTRL_INTFLAG_SEESOVF_Msk           (_U_(0x1) << NVMCTRL_INTFLAG_SEESOVF_Pos)            /**< (NVMCTRL_INTFLAG) Active SEES Overflow Mask */
#define NVMCTRL_INTFLAG_SEESOVF(value)        (NVMCTRL_INTFLAG_SEESOVF_Msk & ((value) << NVMCTRL_INTFLAG_SEESOVF_Pos))
#define NVMCTRL_INTFLAG_SEEWRC_Pos            _U_(10)                                              /**< (NVMCTRL_INTFLAG) SEE Write Completed Position */
#define NVMCTRL_INTFLAG_SEEWRC_Msk            (_U_(0x1) << NVMCTRL_INTFLAG_SEEWRC_Pos)             /**< (NVMCTRL_INTFLAG) SEE Write Completed Mask */
#define NVMCTRL_INTFLAG_SEEWRC(value)         (NVMCTRL_INTFLAG_SEEWRC_Msk & ((value) << NVMCTRL_INTFLAG_SEEWRC_Pos))
#define NVMCTRL_INTFLAG_Msk                   _U_(0x07FF)                                          /**< (NVMCTRL_INTFLAG) Register Mask  */


/* -------- NVMCTRL_STATUS : (NVMCTRL Offset: 0x12) ( R/ 16) Status -------- */
#define NVMCTRL_STATUS_RESETVALUE             _U_(0x00)                                            /**<  (NVMCTRL_STATUS) Status  Reset Value */

#define NVMCTRL_STATUS_READY_Pos              _U_(0)                                               /**< (NVMCTRL_STATUS) Ready to accept a command Position */
#define NVMCTRL_STATUS_READY_Msk              (_U_(0x1) << NVMCTRL_STATUS_READY_Pos)               /**< (NVMCTRL_STATUS) Ready to accept a command Mask */
#define NVMCTRL_STATUS_READY(value)           (NVMCTRL_STATUS_READY_Msk & ((value) << NVMCTRL_STATUS_READY_Pos))
#define NVMCTRL_STATUS_PRM_Pos                _U_(1)                                               /**< (NVMCTRL_STATUS) Power Reduction Mode Position */
#define NVMCTRL_STATUS_PRM_Msk                (_U_(0x1) << NVMCTRL_STATUS_PRM_Pos)                 /**< (NVMCTRL_STATUS) Power Reduction Mode Mask */
#define NVMCTRL_STATUS_PRM(value)             (NVMCTRL_STATUS_PRM_Msk & ((value) << NVMCTRL_STATUS_PRM_Pos))
#define NVMCTRL_STATUS_LOAD_Pos               _U_(2)                                               /**< (NVMCTRL_STATUS) NVM Page Buffer Active Loading Position */
#define NVMCTRL_STATUS_LOAD_Msk               (_U_(0x1) << NVMCTRL_STATUS_LOAD_Pos)                /**< (NVMCTRL_STATUS) NVM Page Buffer Active Loading Mask */
#define NVMCTRL_STATUS_LOAD(value)            (NVMCTRL_STATUS_LOAD_Msk & ((value) << NVMCTRL_STATUS_LOAD_Pos))
#define NVMCTRL_STATUS_SUSP_Pos               _U_(3)                                               /**< (NVMCTRL_STATUS) NVM Write Or Erase Operation Is Suspended Position */
#define NVMCTRL_STATUS_SUSP_Msk               (_U_(0x1) << NVMCTRL_STATUS_SUSP_Pos)                /**< (NVMCTRL_STATUS) NVM Write Or Erase Operation Is Suspended Mask */
#define NVMCTRL_STATUS_SUSP(value)            (NVMCTRL_STATUS_SUSP_Msk & ((value) << NVMCTRL_STATUS_SUSP_Pos))
#define NVMCTRL_STATUS_AFIRST_Pos             _U_(4)                                               /**< (NVMCTRL_STATUS) BANKA First Position */
#define NVMCTRL_STATUS_AFIRST_Msk             (_U_(0x1) << NVMCTRL_STATUS_AFIRST_Pos)              /**< (NVMCTRL_STATUS) BANKA First Mask */
#define NVMCTRL_STATUS_AFIRST(value)          (NVMCTRL_STATUS_AFIRST_Msk & ((value) << NVMCTRL_STATUS_AFIRST_Pos))
#define NVMCTRL_STATUS_BPDIS_Pos              _U_(5)                                               /**< (NVMCTRL_STATUS) Boot Loader Protection Disable Position */
#define NVMCTRL_STATUS_BPDIS_Msk              (_U_(0x1) << NVMCTRL_STATUS_BPDIS_Pos)               /**< (NVMCTRL_STATUS) Boot Loader Protection Disable Mask */
#define NVMCTRL_STATUS_BPDIS(value)           (NVMCTRL_STATUS_BPDIS_Msk & ((value) << NVMCTRL_STATUS_BPDIS_Pos))
#define NVMCTRL_STATUS_BOOTPROT_Pos           _U_(8)                                               /**< (NVMCTRL_STATUS) Boot Loader Protection Size Position */
#define NVMCTRL_STATUS_BOOTPROT_Msk           (_U_(0xF) << NVMCTRL_STATUS_BOOTPROT_Pos)            /**< (NVMCTRL_STATUS) Boot Loader Protection Size Mask */
#define NVMCTRL_STATUS_BOOTPROT(value)        (NVMCTRL_STATUS_BOOTPROT_Msk & ((value) << NVMCTRL_STATUS_BOOTPROT_Pos))
#define   NVMCTRL_STATUS_BOOTPROT_0_Val       _U_(0xF)                                             /**< (NVMCTRL_STATUS) 0 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_8_Val       _U_(0xE)                                             /**< (NVMCTRL_STATUS) 8 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_16_Val      _U_(0xD)                                             /**< (NVMCTRL_STATUS) 16 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_24_Val      _U_(0xC)                                             /**< (NVMCTRL_STATUS) 24 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_32_Val      _U_(0xB)                                             /**< (NVMCTRL_STATUS) 32 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_40_Val      _U_(0xA)                                             /**< (NVMCTRL_STATUS) 40 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_48_Val      _U_(0x9)                                             /**< (NVMCTRL_STATUS) 48 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_56_Val      _U_(0x8)                                             /**< (NVMCTRL_STATUS) 56 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_64_Val      _U_(0x7)                                             /**< (NVMCTRL_STATUS) 64 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_72_Val      _U_(0x6)                                             /**< (NVMCTRL_STATUS) 72 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_80_Val      _U_(0x5)                                             /**< (NVMCTRL_STATUS) 80 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_88_Val      _U_(0x4)                                             /**< (NVMCTRL_STATUS) 88 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_96_Val      _U_(0x3)                                             /**< (NVMCTRL_STATUS) 96 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_104_Val     _U_(0x2)                                             /**< (NVMCTRL_STATUS) 104 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_112_Val     _U_(0x1)                                             /**< (NVMCTRL_STATUS) 112 kbytes  */
#define   NVMCTRL_STATUS_BOOTPROT_120_Val     _U_(0x0)                                             /**< (NVMCTRL_STATUS) 120 kbytes  */
#define NVMCTRL_STATUS_BOOTPROT_0             (NVMCTRL_STATUS_BOOTPROT_0_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 0 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_8             (NVMCTRL_STATUS_BOOTPROT_8_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 8 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_16            (NVMCTRL_STATUS_BOOTPROT_16_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 16 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_24            (NVMCTRL_STATUS_BOOTPROT_24_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 24 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_32            (NVMCTRL_STATUS_BOOTPROT_32_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 32 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_40            (NVMCTRL_STATUS_BOOTPROT_40_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 40 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_48            (NVMCTRL_STATUS_BOOTPROT_48_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 48 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_56            (NVMCTRL_STATUS_BOOTPROT_56_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 56 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_64            (NVMCTRL_STATUS_BOOTPROT_64_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 64 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_72            (NVMCTRL_STATUS_BOOTPROT_72_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 72 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_80            (NVMCTRL_STATUS_BOOTPROT_80_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 80 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_88            (NVMCTRL_STATUS_BOOTPROT_88_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 88 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_96            (NVMCTRL_STATUS_BOOTPROT_96_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 96 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_104           (NVMCTRL_STATUS_BOOTPROT_104_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 104 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_112           (NVMCTRL_STATUS_BOOTPROT_112_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 112 kbytes Position  */
#define NVMCTRL_STATUS_BOOTPROT_120           (NVMCTRL_STATUS_BOOTPROT_120_Val << NVMCTRL_STATUS_BOOTPROT_Pos) /**< (NVMCTRL_STATUS) 120 kbytes Position  */
#define NVMCTRL_STATUS_Msk                    _U_(0x0F3F)                                          /**< (NVMCTRL_STATUS) Register Mask  */


/* -------- NVMCTRL_ADDR : (NVMCTRL Offset: 0x14) (R/W 32) Address -------- */
#define NVMCTRL_ADDR_RESETVALUE               _U_(0x00)                                            /**<  (NVMCTRL_ADDR) Address  Reset Value */

#define NVMCTRL_ADDR_ADDR_Pos                 _U_(0)                                               /**< (NVMCTRL_ADDR) NVM Address Position */
#define NVMCTRL_ADDR_ADDR_Msk                 (_U_(0xFFFFFF) << NVMCTRL_ADDR_ADDR_Pos)             /**< (NVMCTRL_ADDR) NVM Address Mask */
#define NVMCTRL_ADDR_ADDR(value)              (NVMCTRL_ADDR_ADDR_Msk & ((value) << NVMCTRL_ADDR_ADDR_Pos))
#define NVMCTRL_ADDR_Msk                      _U_(0x00FFFFFF)                                      /**< (NVMCTRL_ADDR) Register Mask  */


/* -------- NVMCTRL_RUNLOCK : (NVMCTRL Offset: 0x18) ( R/ 32) Lock Section -------- */
#define NVMCTRL_RUNLOCK_RESETVALUE            _U_(0x00)                                            /**<  (NVMCTRL_RUNLOCK) Lock Section  Reset Value */

#define NVMCTRL_RUNLOCK_RUNLOCK_Pos           _U_(0)                                               /**< (NVMCTRL_RUNLOCK) Region Un-Lock Bits Position */
#define NVMCTRL_RUNLOCK_RUNLOCK_Msk           (_U_(0xFFFFFFFF) << NVMCTRL_RUNLOCK_RUNLOCK_Pos)     /**< (NVMCTRL_RUNLOCK) Region Un-Lock Bits Mask */
#define NVMCTRL_RUNLOCK_RUNLOCK(value)        (NVMCTRL_RUNLOCK_RUNLOCK_Msk & ((value) << NVMCTRL_RUNLOCK_RUNLOCK_Pos))
#define NVMCTRL_RUNLOCK_Msk                   _U_(0xFFFFFFFF)                                      /**< (NVMCTRL_RUNLOCK) Register Mask  */


/* -------- NVMCTRL_PBLDATA : (NVMCTRL Offset: 0x1C) ( R/ 32) Page Buffer Load Data x -------- */
#define NVMCTRL_PBLDATA_RESETVALUE            _U_(0xFFFFFFFF)                                      /**<  (NVMCTRL_PBLDATA) Page Buffer Load Data x  Reset Value */

#define NVMCTRL_PBLDATA_DATA_Pos              _U_(0)                                               /**< (NVMCTRL_PBLDATA) Page Buffer Data Position */
#define NVMCTRL_PBLDATA_DATA_Msk              (_U_(0xFFFFFFFF) << NVMCTRL_PBLDATA_DATA_Pos)        /**< (NVMCTRL_PBLDATA) Page Buffer Data Mask */
#define NVMCTRL_PBLDATA_DATA(value)           (NVMCTRL_PBLDATA_DATA_Msk & ((value) << NVMCTRL_PBLDATA_DATA_Pos))
#define NVMCTRL_PBLDATA_Msk                   _U_(0xFFFFFFFF)                                      /**< (NVMCTRL_PBLDATA) Register Mask  */


/* -------- NVMCTRL_ECCERR : (NVMCTRL Offset: 0x24) ( R/ 32) ECC Error Status Register -------- */
#define NVMCTRL_ECCERR_RESETVALUE             _U_(0x00)                                            /**<  (NVMCTRL_ECCERR) ECC Error Status Register  Reset Value */

#define NVMCTRL_ECCERR_ADDR_Pos               _U_(0)                                               /**< (NVMCTRL_ECCERR) Error Address Position */
#define NVMCTRL_ECCERR_ADDR_Msk               (_U_(0xFFFFFF) << NVMCTRL_ECCERR_ADDR_Pos)           /**< (NVMCTRL_ECCERR) Error Address Mask */
#define NVMCTRL_ECCERR_ADDR(value)            (NVMCTRL_ECCERR_ADDR_Msk & ((value) << NVMCTRL_ECCERR_ADDR_Pos))
#define NVMCTRL_ECCERR_TYPEL_Pos              _U_(28)                                              /**< (NVMCTRL_ECCERR) Low Double-Word Error Type Position */
#define NVMCTRL_ECCERR_TYPEL_Msk              (_U_(0x3) << NVMCTRL_ECCERR_TYPEL_Pos)               /**< (NVMCTRL_ECCERR) Low Double-Word Error Type Mask */
#define NVMCTRL_ECCERR_TYPEL(value)           (NVMCTRL_ECCERR_TYPEL_Msk & ((value) << NVMCTRL_ECCERR_TYPEL_Pos))
#define   NVMCTRL_ECCERR_TYPEL_None_Val       _U_(0x0)                                             /**< (NVMCTRL_ECCERR) No Error Detected Since Last Read  */
#define   NVMCTRL_ECCERR_TYPEL_Single_Val     _U_(0x1)                                             /**< (NVMCTRL_ECCERR) At Least One Single Error Detected Since last Read  */
#define   NVMCTRL_ECCERR_TYPEL_Dual_Val       _U_(0x2)                                             /**< (NVMCTRL_ECCERR) At Least One Dual Error Detected Since Last Read  */
#define NVMCTRL_ECCERR_TYPEL_None             (NVMCTRL_ECCERR_TYPEL_None_Val << NVMCTRL_ECCERR_TYPEL_Pos) /**< (NVMCTRL_ECCERR) No Error Detected Since Last Read Position  */
#define NVMCTRL_ECCERR_TYPEL_Single           (NVMCTRL_ECCERR_TYPEL_Single_Val << NVMCTRL_ECCERR_TYPEL_Pos) /**< (NVMCTRL_ECCERR) At Least One Single Error Detected Since last Read Position  */
#define NVMCTRL_ECCERR_TYPEL_Dual             (NVMCTRL_ECCERR_TYPEL_Dual_Val << NVMCTRL_ECCERR_TYPEL_Pos) /**< (NVMCTRL_ECCERR) At Least One Dual Error Detected Since Last Read Position  */
#define NVMCTRL_ECCERR_TYPEH_Pos              _U_(30)                                              /**< (NVMCTRL_ECCERR) High Double-Word Error Type Position */
#define NVMCTRL_ECCERR_TYPEH_Msk              (_U_(0x3) << NVMCTRL_ECCERR_TYPEH_Pos)               /**< (NVMCTRL_ECCERR) High Double-Word Error Type Mask */
#define NVMCTRL_ECCERR_TYPEH(value)           (NVMCTRL_ECCERR_TYPEH_Msk & ((value) << NVMCTRL_ECCERR_TYPEH_Pos))
#define   NVMCTRL_ECCERR_TYPEH_None_Val       _U_(0x0)                                             /**< (NVMCTRL_ECCERR) No Error Detected Since Last Read  */
#define   NVMCTRL_ECCERR_TYPEH_Single_Val     _U_(0x1)                                             /**< (NVMCTRL_ECCERR) At Least One Single Error Detected Since last Read  */
#define   NVMCTRL_ECCERR_TYPEH_Dual_Val       _U_(0x2)                                             /**< (NVMCTRL_ECCERR) At Least One Dual Error Detected Since Last Read  */
#define NVMCTRL_ECCERR_TYPEH_None             (NVMCTRL_ECCERR_TYPEH_None_Val << NVMCTRL_ECCERR_TYPEH_Pos) /**< (NVMCTRL_ECCERR) No Error Detected Since Last Read Position  */
#define NVMCTRL_ECCERR_TYPEH_Single           (NVMCTRL_ECCERR_TYPEH_Single_Val << NVMCTRL_ECCERR_TYPEH_Pos) /**< (NVMCTRL_ECCERR) At Least One Single Error Detected Since last Read Position  */
#define NVMCTRL_ECCERR_TYPEH_Dual             (NVMCTRL_ECCERR_TYPEH_Dual_Val << NVMCTRL_ECCERR_TYPEH_Pos) /**< (NVMCTRL_ECCERR) At Least One Dual Error Detected Since Last Read Position  */
#define NVMCTRL_ECCERR_Msk                    _U_(0xF0FFFFFF)                                      /**< (NVMCTRL_ECCERR) Register Mask  */


/* -------- NVMCTRL_DBGCTRL : (NVMCTRL Offset: 0x28) (R/W 8) Debug Control -------- */
#define NVMCTRL_DBGCTRL_RESETVALUE            _U_(0x00)                                            /**<  (NVMCTRL_DBGCTRL) Debug Control  Reset Value */

#define NVMCTRL_DBGCTRL_ECCDIS_Pos            _U_(0)                                               /**< (NVMCTRL_DBGCTRL) Debugger ECC Read Disable Position */
#define NVMCTRL_DBGCTRL_ECCDIS_Msk            (_U_(0x1) << NVMCTRL_DBGCTRL_ECCDIS_Pos)             /**< (NVMCTRL_DBGCTRL) Debugger ECC Read Disable Mask */
#define NVMCTRL_DBGCTRL_ECCDIS(value)         (NVMCTRL_DBGCTRL_ECCDIS_Msk & ((value) << NVMCTRL_DBGCTRL_ECCDIS_Pos))
#define NVMCTRL_DBGCTRL_ECCELOG_Pos           _U_(1)                                               /**< (NVMCTRL_DBGCTRL) Debugger ECC Error Tracking Mode Position */
#define NVMCTRL_DBGCTRL_ECCELOG_Msk           (_U_(0x1) << NVMCTRL_DBGCTRL_ECCELOG_Pos)            /**< (NVMCTRL_DBGCTRL) Debugger ECC Error Tracking Mode Mask */
#define NVMCTRL_DBGCTRL_ECCELOG(value)        (NVMCTRL_DBGCTRL_ECCELOG_Msk & ((value) << NVMCTRL_DBGCTRL_ECCELOG_Pos))
#define NVMCTRL_DBGCTRL_Msk                   _U_(0x03)                                            /**< (NVMCTRL_DBGCTRL) Register Mask  */


/* -------- NVMCTRL_SEECFG : (NVMCTRL Offset: 0x2A) (R/W 8) SmartEEPROM Configuration Register -------- */
#define NVMCTRL_SEECFG_RESETVALUE             _U_(0x00)                                            /**<  (NVMCTRL_SEECFG) SmartEEPROM Configuration Register  Reset Value */

#define NVMCTRL_SEECFG_WMODE_Pos              _U_(0)                                               /**< (NVMCTRL_SEECFG) Write Mode Position */
#define NVMCTRL_SEECFG_WMODE_Msk              (_U_(0x1) << NVMCTRL_SEECFG_WMODE_Pos)               /**< (NVMCTRL_SEECFG) Write Mode Mask */
#define NVMCTRL_SEECFG_WMODE(value)           (NVMCTRL_SEECFG_WMODE_Msk & ((value) << NVMCTRL_SEECFG_WMODE_Pos))
#define   NVMCTRL_SEECFG_WMODE_UNBUFFERED_Val _U_(0x0)                                             /**< (NVMCTRL_SEECFG) A NVM write command is issued after each write in the pagebuffer  */
#define   NVMCTRL_SEECFG_WMODE_BUFFERED_Val   _U_(0x1)                                             /**< (NVMCTRL_SEECFG) A NVM write command is issued when a write to a new page is requested  */
#define NVMCTRL_SEECFG_WMODE_UNBUFFERED       (NVMCTRL_SEECFG_WMODE_UNBUFFERED_Val << NVMCTRL_SEECFG_WMODE_Pos) /**< (NVMCTRL_SEECFG) A NVM write command is issued after each write in the pagebuffer Position  */
#define NVMCTRL_SEECFG_WMODE_BUFFERED         (NVMCTRL_SEECFG_WMODE_BUFFERED_Val << NVMCTRL_SEECFG_WMODE_Pos) /**< (NVMCTRL_SEECFG) A NVM write command is issued when a write to a new page is requested Position  */
#define NVMCTRL_SEECFG_APRDIS_Pos             _U_(1)                                               /**< (NVMCTRL_SEECFG) Automatic Page Reallocation Disable Position */
#define NVMCTRL_SEECFG_APRDIS_Msk             (_U_(0x1) << NVMCTRL_SEECFG_APRDIS_Pos)              /**< (NVMCTRL_SEECFG) Automatic Page Reallocation Disable Mask */
#define NVMCTRL_SEECFG_APRDIS(value)          (NVMCTRL_SEECFG_APRDIS_Msk & ((value) << NVMCTRL_SEECFG_APRDIS_Pos))
#define NVMCTRL_SEECFG_Msk                    _U_(0x03)                                            /**< (NVMCTRL_SEECFG) Register Mask  */


/* -------- NVMCTRL_SEESTAT : (NVMCTRL Offset: 0x2C) ( R/ 32) SmartEEPROM Status Register -------- */
#define NVMCTRL_SEESTAT_RESETVALUE            _U_(0x00)                                            /**<  (NVMCTRL_SEESTAT) SmartEEPROM Status Register  Reset Value */

#define NVMCTRL_SEESTAT_ASEES_Pos             _U_(0)                                               /**< (NVMCTRL_SEESTAT) Active SmartEEPROM Sector Position */
#define NVMCTRL_SEESTAT_ASEES_Msk             (_U_(0x1) << NVMCTRL_SEESTAT_ASEES_Pos)              /**< (NVMCTRL_SEESTAT) Active SmartEEPROM Sector Mask */
#define NVMCTRL_SEESTAT_ASEES(value)          (NVMCTRL_SEESTAT_ASEES_Msk & ((value) << NVMCTRL_SEESTAT_ASEES_Pos))
#define NVMCTRL_SEESTAT_LOAD_Pos              _U_(1)                                               /**< (NVMCTRL_SEESTAT) Page Buffer Loaded Position */
#define NVMCTRL_SEESTAT_LOAD_Msk              (_U_(0x1) << NVMCTRL_SEESTAT_LOAD_Pos)               /**< (NVMCTRL_SEESTAT) Page Buffer Loaded Mask */
#define NVMCTRL_SEESTAT_LOAD(value)           (NVMCTRL_SEESTAT_LOAD_Msk & ((value) << NVMCTRL_SEESTAT_LOAD_Pos))
#define NVMCTRL_SEESTAT_BUSY_Pos              _U_(2)                                               /**< (NVMCTRL_SEESTAT) Busy Position */
#define NVMCTRL_SEESTAT_BUSY_Msk              (_U_(0x1) << NVMCTRL_SEESTAT_BUSY_Pos)               /**< (NVMCTRL_SEESTAT) Busy Mask */
#define NVMCTRL_SEESTAT_BUSY(value)           (NVMCTRL_SEESTAT_BUSY_Msk & ((value) << NVMCTRL_SEESTAT_BUSY_Pos))
#define NVMCTRL_SEESTAT_LOCK_Pos              _U_(3)                                               /**< (NVMCTRL_SEESTAT) SmartEEPROM Write Access Is Locked Position */
#define NVMCTRL_SEESTAT_LOCK_Msk              (_U_(0x1) << NVMCTRL_SEESTAT_LOCK_Pos)               /**< (NVMCTRL_SEESTAT) SmartEEPROM Write Access Is Locked Mask */
#define NVMCTRL_SEESTAT_LOCK(value)           (NVMCTRL_SEESTAT_LOCK_Msk & ((value) << NVMCTRL_SEESTAT_LOCK_Pos))
#define NVMCTRL_SEESTAT_RLOCK_Pos             _U_(4)                                               /**< (NVMCTRL_SEESTAT) SmartEEPROM Write Access To Register Address Space Is Locked Position */
#define NVMCTRL_SEESTAT_RLOCK_Msk             (_U_(0x1) << NVMCTRL_SEESTAT_RLOCK_Pos)              /**< (NVMCTRL_SEESTAT) SmartEEPROM Write Access To Register Address Space Is Locked Mask */
#define NVMCTRL_SEESTAT_RLOCK(value)          (NVMCTRL_SEESTAT_RLOCK_Msk & ((value) << NVMCTRL_SEESTAT_RLOCK_Pos))
#define NVMCTRL_SEESTAT_SBLK_Pos              _U_(8)                                               /**< (NVMCTRL_SEESTAT) Blocks Number In a Sector Position */
#define NVMCTRL_SEESTAT_SBLK_Msk              (_U_(0xF) << NVMCTRL_SEESTAT_SBLK_Pos)               /**< (NVMCTRL_SEESTAT) Blocks Number In a Sector Mask */
#define NVMCTRL_SEESTAT_SBLK(value)           (NVMCTRL_SEESTAT_SBLK_Msk & ((value) << NVMCTRL_SEESTAT_SBLK_Pos))
#define NVMCTRL_SEESTAT_PSZ_Pos               _U_(16)                                              /**< (NVMCTRL_SEESTAT) SmartEEPROM Page Size Position */
#define NVMCTRL_SEESTAT_PSZ_Msk               (_U_(0x7) << NVMCTRL_SEESTAT_PSZ_Pos)                /**< (NVMCTRL_SEESTAT) SmartEEPROM Page Size Mask */
#define NVMCTRL_SEESTAT_PSZ(value)            (NVMCTRL_SEESTAT_PSZ_Msk & ((value) << NVMCTRL_SEESTAT_PSZ_Pos))
#define NVMCTRL_SEESTAT_Msk                   _U_(0x00070F1F)                                      /**< (NVMCTRL_SEESTAT) Register Mask  */


/** \brief NVMCTRL register offsets definitions */
#define NVMCTRL_CTRLA_REG_OFST         (0x00)              /**< (NVMCTRL_CTRLA) Control A Offset */
#define NVMCTRL_CTRLB_REG_OFST         (0x04)              /**< (NVMCTRL_CTRLB) Control B Offset */
#define NVMCTRL_PARAM_REG_OFST         (0x08)              /**< (NVMCTRL_PARAM) NVM Parameter Offset */
#define NVMCTRL_INTENCLR_REG_OFST      (0x0C)              /**< (NVMCTRL_INTENCLR) Interrupt Enable Clear Offset */
#define NVMCTRL_INTENSET_REG_OFST      (0x0E)              /**< (NVMCTRL_INTENSET) Interrupt Enable Set Offset */
#define NVMCTRL_INTFLAG_REG_OFST       (0x10)              /**< (NVMCTRL_INTFLAG) Interrupt Flag Status and Clear Offset */
#define NVMCTRL_STATUS_REG_OFST        (0x12)              /**< (NVMCTRL_STATUS) Status Offset */
#define NVMCTRL_ADDR_REG_OFST          (0x14)              /**< (NVMCTRL_ADDR) Address Offset */
#define NVMCTRL_RUNLOCK_REG_OFST       (0x18)              /**< (NVMCTRL_RUNLOCK) Lock Section Offset */
#define NVMCTRL_PBLDATA_REG_OFST       (0x1C)              /**< (NVMCTRL_PBLDATA) Page Buffer Load Data x Offset */
#define NVMCTRL_ECCERR_REG_OFST        (0x24)              /**< (NVMCTRL_ECCERR) ECC Error Status Register Offset */
#define NVMCTRL_DBGCTRL_REG_OFST       (0x28)              /**< (NVMCTRL_DBGCTRL) Debug Control Offset */
#define NVMCTRL_SEECFG_REG_OFST        (0x2A)              /**< (NVMCTRL_SEECFG) SmartEEPROM Configuration Register Offset */
#define NVMCTRL_SEESTAT_REG_OFST       (0x2C)              /**< (NVMCTRL_SEESTAT) SmartEEPROM Status Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief NVMCTRL register API structure */
typedef struct
{  /* Non-Volatile Memory Controller */
  __IO  uint16_t                       NVMCTRL_CTRLA;      /**< Offset: 0x00 (R/W  16) Control A */
  __I   uint8_t                        Reserved1[0x02];
  __O   uint16_t                       NVMCTRL_CTRLB;      /**< Offset: 0x04 ( /W  16) Control B */
  __I   uint8_t                        Reserved2[0x02];
  __I   uint32_t                       NVMCTRL_PARAM;      /**< Offset: 0x08 (R/   32) NVM Parameter */
  __IO  uint16_t                       NVMCTRL_INTENCLR;   /**< Offset: 0x0C (R/W  16) Interrupt Enable Clear */
  __IO  uint16_t                       NVMCTRL_INTENSET;   /**< Offset: 0x0E (R/W  16) Interrupt Enable Set */
  __IO  uint16_t                       NVMCTRL_INTFLAG;    /**< Offset: 0x10 (R/W  16) Interrupt Flag Status and Clear */
  __I   uint16_t                       NVMCTRL_STATUS;     /**< Offset: 0x12 (R/   16) Status */
  __IO  uint32_t                       NVMCTRL_ADDR;       /**< Offset: 0x14 (R/W  32) Address */
  __I   uint32_t                       NVMCTRL_RUNLOCK;    /**< Offset: 0x18 (R/   32) Lock Section */
  __I   uint32_t                       NVMCTRL_PBLDATA[2]; /**< Offset: 0x1C (R/   32) Page Buffer Load Data x */
  __I   uint32_t                       NVMCTRL_ECCERR;     /**< Offset: 0x24 (R/   32) ECC Error Status Register */
  __IO  uint8_t                        NVMCTRL_DBGCTRL;    /**< Offset: 0x28 (R/W  8) Debug Control */
  __I   uint8_t                        Reserved3[0x01];
  __IO  uint8_t                        NVMCTRL_SEECFG;     /**< Offset: 0x2A (R/W  8) SmartEEPROM Configuration Register */
  __I   uint8_t                        Reserved4[0x01];
  __I   uint32_t                       NVMCTRL_SEESTAT;    /**< Offset: 0x2C (R/   32) SmartEEPROM Status Register */
} nvmctrl_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_NVMCTRL_COMPONENT_H_ */
