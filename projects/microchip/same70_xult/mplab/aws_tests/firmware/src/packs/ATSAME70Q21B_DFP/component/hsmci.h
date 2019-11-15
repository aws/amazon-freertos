/**
 * \brief Component description for HSMCI
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
#ifndef _SAME70_HSMCI_COMPONENT_H_
#define _SAME70_HSMCI_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR HSMCI                                        */
/* ************************************************************************** */

/* -------- HSMCI_CR : (HSMCI Offset: 0x00) ( /W 32) Control Register -------- */
#define HSMCI_CR_MCIEN_Pos                    _U_(0)                                               /**< (HSMCI_CR) Multi-Media Interface Enable Position */
#define HSMCI_CR_MCIEN_Msk                    (_U_(0x1) << HSMCI_CR_MCIEN_Pos)                     /**< (HSMCI_CR) Multi-Media Interface Enable Mask */
#define HSMCI_CR_MCIEN(value)                 (HSMCI_CR_MCIEN_Msk & ((value) << HSMCI_CR_MCIEN_Pos))
#define HSMCI_CR_MCIDIS_Pos                   _U_(1)                                               /**< (HSMCI_CR) Multi-Media Interface Disable Position */
#define HSMCI_CR_MCIDIS_Msk                   (_U_(0x1) << HSMCI_CR_MCIDIS_Pos)                    /**< (HSMCI_CR) Multi-Media Interface Disable Mask */
#define HSMCI_CR_MCIDIS(value)                (HSMCI_CR_MCIDIS_Msk & ((value) << HSMCI_CR_MCIDIS_Pos))
#define HSMCI_CR_PWSEN_Pos                    _U_(2)                                               /**< (HSMCI_CR) Power Save Mode Enable Position */
#define HSMCI_CR_PWSEN_Msk                    (_U_(0x1) << HSMCI_CR_PWSEN_Pos)                     /**< (HSMCI_CR) Power Save Mode Enable Mask */
#define HSMCI_CR_PWSEN(value)                 (HSMCI_CR_PWSEN_Msk & ((value) << HSMCI_CR_PWSEN_Pos))
#define HSMCI_CR_PWSDIS_Pos                   _U_(3)                                               /**< (HSMCI_CR) Power Save Mode Disable Position */
#define HSMCI_CR_PWSDIS_Msk                   (_U_(0x1) << HSMCI_CR_PWSDIS_Pos)                    /**< (HSMCI_CR) Power Save Mode Disable Mask */
#define HSMCI_CR_PWSDIS(value)                (HSMCI_CR_PWSDIS_Msk & ((value) << HSMCI_CR_PWSDIS_Pos))
#define HSMCI_CR_SWRST_Pos                    _U_(7)                                               /**< (HSMCI_CR) Software Reset Position */
#define HSMCI_CR_SWRST_Msk                    (_U_(0x1) << HSMCI_CR_SWRST_Pos)                     /**< (HSMCI_CR) Software Reset Mask */
#define HSMCI_CR_SWRST(value)                 (HSMCI_CR_SWRST_Msk & ((value) << HSMCI_CR_SWRST_Pos))
#define HSMCI_CR_Msk                          _U_(0x0000008F)                                      /**< (HSMCI_CR) Register Mask  */


/* -------- HSMCI_MR : (HSMCI Offset: 0x04) (R/W 32) Mode Register -------- */
#define HSMCI_MR_CLKDIV_Pos                   _U_(0)                                               /**< (HSMCI_MR) Clock Divider Position */
#define HSMCI_MR_CLKDIV_Msk                   (_U_(0xFF) << HSMCI_MR_CLKDIV_Pos)                   /**< (HSMCI_MR) Clock Divider Mask */
#define HSMCI_MR_CLKDIV(value)                (HSMCI_MR_CLKDIV_Msk & ((value) << HSMCI_MR_CLKDIV_Pos))
#define HSMCI_MR_PWSDIV_Pos                   _U_(8)                                               /**< (HSMCI_MR) Power Saving Divider Position */
#define HSMCI_MR_PWSDIV_Msk                   (_U_(0x7) << HSMCI_MR_PWSDIV_Pos)                    /**< (HSMCI_MR) Power Saving Divider Mask */
#define HSMCI_MR_PWSDIV(value)                (HSMCI_MR_PWSDIV_Msk & ((value) << HSMCI_MR_PWSDIV_Pos))
#define HSMCI_MR_RDPROOF_Pos                  _U_(11)                                              /**< (HSMCI_MR) Read Proof Enable Position */
#define HSMCI_MR_RDPROOF_Msk                  (_U_(0x1) << HSMCI_MR_RDPROOF_Pos)                   /**< (HSMCI_MR) Read Proof Enable Mask */
#define HSMCI_MR_RDPROOF(value)               (HSMCI_MR_RDPROOF_Msk & ((value) << HSMCI_MR_RDPROOF_Pos))
#define HSMCI_MR_WRPROOF_Pos                  _U_(12)                                              /**< (HSMCI_MR) Write Proof Enable Position */
#define HSMCI_MR_WRPROOF_Msk                  (_U_(0x1) << HSMCI_MR_WRPROOF_Pos)                   /**< (HSMCI_MR) Write Proof Enable Mask */
#define HSMCI_MR_WRPROOF(value)               (HSMCI_MR_WRPROOF_Msk & ((value) << HSMCI_MR_WRPROOF_Pos))
#define HSMCI_MR_FBYTE_Pos                    _U_(13)                                              /**< (HSMCI_MR) Force Byte Transfer Position */
#define HSMCI_MR_FBYTE_Msk                    (_U_(0x1) << HSMCI_MR_FBYTE_Pos)                     /**< (HSMCI_MR) Force Byte Transfer Mask */
#define HSMCI_MR_FBYTE(value)                 (HSMCI_MR_FBYTE_Msk & ((value) << HSMCI_MR_FBYTE_Pos))
#define HSMCI_MR_PADV_Pos                     _U_(14)                                              /**< (HSMCI_MR) Padding Value Position */
#define HSMCI_MR_PADV_Msk                     (_U_(0x1) << HSMCI_MR_PADV_Pos)                      /**< (HSMCI_MR) Padding Value Mask */
#define HSMCI_MR_PADV(value)                  (HSMCI_MR_PADV_Msk & ((value) << HSMCI_MR_PADV_Pos))
#define HSMCI_MR_CLKODD_Pos                   _U_(16)                                              /**< (HSMCI_MR) Clock divider is odd Position */
#define HSMCI_MR_CLKODD_Msk                   (_U_(0x1) << HSMCI_MR_CLKODD_Pos)                    /**< (HSMCI_MR) Clock divider is odd Mask */
#define HSMCI_MR_CLKODD(value)                (HSMCI_MR_CLKODD_Msk & ((value) << HSMCI_MR_CLKODD_Pos))
#define HSMCI_MR_Msk                          _U_(0x00017FFF)                                      /**< (HSMCI_MR) Register Mask  */


/* -------- HSMCI_DTOR : (HSMCI Offset: 0x08) (R/W 32) Data Timeout Register -------- */
#define HSMCI_DTOR_DTOCYC_Pos                 _U_(0)                                               /**< (HSMCI_DTOR) Data Timeout Cycle Number Position */
#define HSMCI_DTOR_DTOCYC_Msk                 (_U_(0xF) << HSMCI_DTOR_DTOCYC_Pos)                  /**< (HSMCI_DTOR) Data Timeout Cycle Number Mask */
#define HSMCI_DTOR_DTOCYC(value)              (HSMCI_DTOR_DTOCYC_Msk & ((value) << HSMCI_DTOR_DTOCYC_Pos))
#define HSMCI_DTOR_DTOMUL_Pos                 _U_(4)                                               /**< (HSMCI_DTOR) Data Timeout Multiplier Position */
#define HSMCI_DTOR_DTOMUL_Msk                 (_U_(0x7) << HSMCI_DTOR_DTOMUL_Pos)                  /**< (HSMCI_DTOR) Data Timeout Multiplier Mask */
#define HSMCI_DTOR_DTOMUL(value)              (HSMCI_DTOR_DTOMUL_Msk & ((value) << HSMCI_DTOR_DTOMUL_Pos))
#define   HSMCI_DTOR_DTOMUL_1_Val             _U_(0x0)                                             /**< (HSMCI_DTOR) DTOCYC  */
#define   HSMCI_DTOR_DTOMUL_16_Val            _U_(0x1)                                             /**< (HSMCI_DTOR) DTOCYC x 16  */
#define   HSMCI_DTOR_DTOMUL_128_Val           _U_(0x2)                                             /**< (HSMCI_DTOR) DTOCYC x 128  */
#define   HSMCI_DTOR_DTOMUL_256_Val           _U_(0x3)                                             /**< (HSMCI_DTOR) DTOCYC x 256  */
#define   HSMCI_DTOR_DTOMUL_1024_Val          _U_(0x4)                                             /**< (HSMCI_DTOR) DTOCYC x 1024  */
#define   HSMCI_DTOR_DTOMUL_4096_Val          _U_(0x5)                                             /**< (HSMCI_DTOR) DTOCYC x 4096  */
#define   HSMCI_DTOR_DTOMUL_65536_Val         _U_(0x6)                                             /**< (HSMCI_DTOR) DTOCYC x 65536  */
#define   HSMCI_DTOR_DTOMUL_1048576_Val       _U_(0x7)                                             /**< (HSMCI_DTOR) DTOCYC x 1048576  */
#define HSMCI_DTOR_DTOMUL_1                   (HSMCI_DTOR_DTOMUL_1_Val << HSMCI_DTOR_DTOMUL_Pos)   /**< (HSMCI_DTOR) DTOCYC Position  */
#define HSMCI_DTOR_DTOMUL_16                  (HSMCI_DTOR_DTOMUL_16_Val << HSMCI_DTOR_DTOMUL_Pos)  /**< (HSMCI_DTOR) DTOCYC x 16 Position  */
#define HSMCI_DTOR_DTOMUL_128                 (HSMCI_DTOR_DTOMUL_128_Val << HSMCI_DTOR_DTOMUL_Pos) /**< (HSMCI_DTOR) DTOCYC x 128 Position  */
#define HSMCI_DTOR_DTOMUL_256                 (HSMCI_DTOR_DTOMUL_256_Val << HSMCI_DTOR_DTOMUL_Pos) /**< (HSMCI_DTOR) DTOCYC x 256 Position  */
#define HSMCI_DTOR_DTOMUL_1024                (HSMCI_DTOR_DTOMUL_1024_Val << HSMCI_DTOR_DTOMUL_Pos) /**< (HSMCI_DTOR) DTOCYC x 1024 Position  */
#define HSMCI_DTOR_DTOMUL_4096                (HSMCI_DTOR_DTOMUL_4096_Val << HSMCI_DTOR_DTOMUL_Pos) /**< (HSMCI_DTOR) DTOCYC x 4096 Position  */
#define HSMCI_DTOR_DTOMUL_65536               (HSMCI_DTOR_DTOMUL_65536_Val << HSMCI_DTOR_DTOMUL_Pos) /**< (HSMCI_DTOR) DTOCYC x 65536 Position  */
#define HSMCI_DTOR_DTOMUL_1048576             (HSMCI_DTOR_DTOMUL_1048576_Val << HSMCI_DTOR_DTOMUL_Pos) /**< (HSMCI_DTOR) DTOCYC x 1048576 Position  */
#define HSMCI_DTOR_Msk                        _U_(0x0000007F)                                      /**< (HSMCI_DTOR) Register Mask  */


/* -------- HSMCI_SDCR : (HSMCI Offset: 0x0C) (R/W 32) SD/SDIO Card Register -------- */
#define HSMCI_SDCR_SDCSEL_Pos                 _U_(0)                                               /**< (HSMCI_SDCR) SDCard/SDIO Slot Position */
#define HSMCI_SDCR_SDCSEL_Msk                 (_U_(0x3) << HSMCI_SDCR_SDCSEL_Pos)                  /**< (HSMCI_SDCR) SDCard/SDIO Slot Mask */
#define HSMCI_SDCR_SDCSEL(value)              (HSMCI_SDCR_SDCSEL_Msk & ((value) << HSMCI_SDCR_SDCSEL_Pos))
#define   HSMCI_SDCR_SDCSEL_SLOTA_Val         _U_(0x0)                                             /**< (HSMCI_SDCR) Slot A is selected.  */
#define HSMCI_SDCR_SDCSEL_SLOTA               (HSMCI_SDCR_SDCSEL_SLOTA_Val << HSMCI_SDCR_SDCSEL_Pos) /**< (HSMCI_SDCR) Slot A is selected. Position  */
#define HSMCI_SDCR_SDCBUS_Pos                 _U_(6)                                               /**< (HSMCI_SDCR) SDCard/SDIO Bus Width Position */
#define HSMCI_SDCR_SDCBUS_Msk                 (_U_(0x3) << HSMCI_SDCR_SDCBUS_Pos)                  /**< (HSMCI_SDCR) SDCard/SDIO Bus Width Mask */
#define HSMCI_SDCR_SDCBUS(value)              (HSMCI_SDCR_SDCBUS_Msk & ((value) << HSMCI_SDCR_SDCBUS_Pos))
#define   HSMCI_SDCR_SDCBUS_1_Val             _U_(0x0)                                             /**< (HSMCI_SDCR) 1 bit  */
#define   HSMCI_SDCR_SDCBUS_4_Val             _U_(0x2)                                             /**< (HSMCI_SDCR) 4 bits  */
#define   HSMCI_SDCR_SDCBUS_8_Val             _U_(0x3)                                             /**< (HSMCI_SDCR) 8 bits  */
#define HSMCI_SDCR_SDCBUS_1                   (HSMCI_SDCR_SDCBUS_1_Val << HSMCI_SDCR_SDCBUS_Pos)   /**< (HSMCI_SDCR) 1 bit Position  */
#define HSMCI_SDCR_SDCBUS_4                   (HSMCI_SDCR_SDCBUS_4_Val << HSMCI_SDCR_SDCBUS_Pos)   /**< (HSMCI_SDCR) 4 bits Position  */
#define HSMCI_SDCR_SDCBUS_8                   (HSMCI_SDCR_SDCBUS_8_Val << HSMCI_SDCR_SDCBUS_Pos)   /**< (HSMCI_SDCR) 8 bits Position  */
#define HSMCI_SDCR_Msk                        _U_(0x000000C3)                                      /**< (HSMCI_SDCR) Register Mask  */


/* -------- HSMCI_ARGR : (HSMCI Offset: 0x10) (R/W 32) Argument Register -------- */
#define HSMCI_ARGR_ARG_Pos                    _U_(0)                                               /**< (HSMCI_ARGR) Command Argument Position */
#define HSMCI_ARGR_ARG_Msk                    (_U_(0xFFFFFFFF) << HSMCI_ARGR_ARG_Pos)              /**< (HSMCI_ARGR) Command Argument Mask */
#define HSMCI_ARGR_ARG(value)                 (HSMCI_ARGR_ARG_Msk & ((value) << HSMCI_ARGR_ARG_Pos))
#define HSMCI_ARGR_Msk                        _U_(0xFFFFFFFF)                                      /**< (HSMCI_ARGR) Register Mask  */


/* -------- HSMCI_CMDR : (HSMCI Offset: 0x14) ( /W 32) Command Register -------- */
#define HSMCI_CMDR_CMDNB_Pos                  _U_(0)                                               /**< (HSMCI_CMDR) Command Number Position */
#define HSMCI_CMDR_CMDNB_Msk                  (_U_(0x3F) << HSMCI_CMDR_CMDNB_Pos)                  /**< (HSMCI_CMDR) Command Number Mask */
#define HSMCI_CMDR_CMDNB(value)               (HSMCI_CMDR_CMDNB_Msk & ((value) << HSMCI_CMDR_CMDNB_Pos))
#define HSMCI_CMDR_RSPTYP_Pos                 _U_(6)                                               /**< (HSMCI_CMDR) Response Type Position */
#define HSMCI_CMDR_RSPTYP_Msk                 (_U_(0x3) << HSMCI_CMDR_RSPTYP_Pos)                  /**< (HSMCI_CMDR) Response Type Mask */
#define HSMCI_CMDR_RSPTYP(value)              (HSMCI_CMDR_RSPTYP_Msk & ((value) << HSMCI_CMDR_RSPTYP_Pos))
#define   HSMCI_CMDR_RSPTYP_NORESP_Val        _U_(0x0)                                             /**< (HSMCI_CMDR) No response  */
#define   HSMCI_CMDR_RSPTYP_48_BIT_Val        _U_(0x1)                                             /**< (HSMCI_CMDR) 48-bit response  */
#define   HSMCI_CMDR_RSPTYP_136_BIT_Val       _U_(0x2)                                             /**< (HSMCI_CMDR) 136-bit response  */
#define   HSMCI_CMDR_RSPTYP_R1B_Val           _U_(0x3)                                             /**< (HSMCI_CMDR) R1b response type  */
#define HSMCI_CMDR_RSPTYP_NORESP              (HSMCI_CMDR_RSPTYP_NORESP_Val << HSMCI_CMDR_RSPTYP_Pos) /**< (HSMCI_CMDR) No response Position  */
#define HSMCI_CMDR_RSPTYP_48_BIT              (HSMCI_CMDR_RSPTYP_48_BIT_Val << HSMCI_CMDR_RSPTYP_Pos) /**< (HSMCI_CMDR) 48-bit response Position  */
#define HSMCI_CMDR_RSPTYP_136_BIT             (HSMCI_CMDR_RSPTYP_136_BIT_Val << HSMCI_CMDR_RSPTYP_Pos) /**< (HSMCI_CMDR) 136-bit response Position  */
#define HSMCI_CMDR_RSPTYP_R1B                 (HSMCI_CMDR_RSPTYP_R1B_Val << HSMCI_CMDR_RSPTYP_Pos) /**< (HSMCI_CMDR) R1b response type Position  */
#define HSMCI_CMDR_SPCMD_Pos                  _U_(8)                                               /**< (HSMCI_CMDR) Special Command Position */
#define HSMCI_CMDR_SPCMD_Msk                  (_U_(0x7) << HSMCI_CMDR_SPCMD_Pos)                   /**< (HSMCI_CMDR) Special Command Mask */
#define HSMCI_CMDR_SPCMD(value)               (HSMCI_CMDR_SPCMD_Msk & ((value) << HSMCI_CMDR_SPCMD_Pos))
#define   HSMCI_CMDR_SPCMD_STD_Val            _U_(0x0)                                             /**< (HSMCI_CMDR) Not a special CMD.  */
#define   HSMCI_CMDR_SPCMD_INIT_Val           _U_(0x1)                                             /**< (HSMCI_CMDR) Initialization CMD: 74 clock cycles for initialization sequence.  */
#define   HSMCI_CMDR_SPCMD_SYNC_Val           _U_(0x2)                                             /**< (HSMCI_CMDR) Synchronized CMD: Wait for the end of the current data block transfer before sending the pending command.  */
#define   HSMCI_CMDR_SPCMD_CE_ATA_Val         _U_(0x3)                                             /**< (HSMCI_CMDR) CE-ATA Completion Signal disable Command. The host cancels the ability for the device to return a command completion signal on the command line.  */
#define   HSMCI_CMDR_SPCMD_IT_CMD_Val         _U_(0x4)                                             /**< (HSMCI_CMDR) Interrupt command: Corresponds to the Interrupt Mode (CMD40).  */
#define   HSMCI_CMDR_SPCMD_IT_RESP_Val        _U_(0x5)                                             /**< (HSMCI_CMDR) Interrupt response: Corresponds to the Interrupt Mode (CMD40).  */
#define   HSMCI_CMDR_SPCMD_BOR_Val            _U_(0x6)                                             /**< (HSMCI_CMDR) Boot Operation Request. Start a boot operation mode, the host processor can read boot data from the MMC device directly.  */
#define   HSMCI_CMDR_SPCMD_EBO_Val            _U_(0x7)                                             /**< (HSMCI_CMDR) End Boot Operation. This command allows the host processor to terminate the boot operation mode.  */
#define HSMCI_CMDR_SPCMD_STD                  (HSMCI_CMDR_SPCMD_STD_Val << HSMCI_CMDR_SPCMD_Pos)   /**< (HSMCI_CMDR) Not a special CMD. Position  */
#define HSMCI_CMDR_SPCMD_INIT                 (HSMCI_CMDR_SPCMD_INIT_Val << HSMCI_CMDR_SPCMD_Pos)  /**< (HSMCI_CMDR) Initialization CMD: 74 clock cycles for initialization sequence. Position  */
#define HSMCI_CMDR_SPCMD_SYNC                 (HSMCI_CMDR_SPCMD_SYNC_Val << HSMCI_CMDR_SPCMD_Pos)  /**< (HSMCI_CMDR) Synchronized CMD: Wait for the end of the current data block transfer before sending the pending command. Position  */
#define HSMCI_CMDR_SPCMD_CE_ATA               (HSMCI_CMDR_SPCMD_CE_ATA_Val << HSMCI_CMDR_SPCMD_Pos) /**< (HSMCI_CMDR) CE-ATA Completion Signal disable Command. The host cancels the ability for the device to return a command completion signal on the command line. Position  */
#define HSMCI_CMDR_SPCMD_IT_CMD               (HSMCI_CMDR_SPCMD_IT_CMD_Val << HSMCI_CMDR_SPCMD_Pos) /**< (HSMCI_CMDR) Interrupt command: Corresponds to the Interrupt Mode (CMD40). Position  */
#define HSMCI_CMDR_SPCMD_IT_RESP              (HSMCI_CMDR_SPCMD_IT_RESP_Val << HSMCI_CMDR_SPCMD_Pos) /**< (HSMCI_CMDR) Interrupt response: Corresponds to the Interrupt Mode (CMD40). Position  */
#define HSMCI_CMDR_SPCMD_BOR                  (HSMCI_CMDR_SPCMD_BOR_Val << HSMCI_CMDR_SPCMD_Pos)   /**< (HSMCI_CMDR) Boot Operation Request. Start a boot operation mode, the host processor can read boot data from the MMC device directly. Position  */
#define HSMCI_CMDR_SPCMD_EBO                  (HSMCI_CMDR_SPCMD_EBO_Val << HSMCI_CMDR_SPCMD_Pos)   /**< (HSMCI_CMDR) End Boot Operation. This command allows the host processor to terminate the boot operation mode. Position  */
#define HSMCI_CMDR_OPDCMD_Pos                 _U_(11)                                              /**< (HSMCI_CMDR) Open Drain Command Position */
#define HSMCI_CMDR_OPDCMD_Msk                 (_U_(0x1) << HSMCI_CMDR_OPDCMD_Pos)                  /**< (HSMCI_CMDR) Open Drain Command Mask */
#define HSMCI_CMDR_OPDCMD(value)              (HSMCI_CMDR_OPDCMD_Msk & ((value) << HSMCI_CMDR_OPDCMD_Pos))
#define   HSMCI_CMDR_OPDCMD_PUSHPULL_Val      _U_(0x0)                                             /**< (HSMCI_CMDR) Push pull command.  */
#define   HSMCI_CMDR_OPDCMD_OPENDRAIN_Val     _U_(0x1)                                             /**< (HSMCI_CMDR) Open drain command.  */
#define HSMCI_CMDR_OPDCMD_PUSHPULL            (HSMCI_CMDR_OPDCMD_PUSHPULL_Val << HSMCI_CMDR_OPDCMD_Pos) /**< (HSMCI_CMDR) Push pull command. Position  */
#define HSMCI_CMDR_OPDCMD_OPENDRAIN           (HSMCI_CMDR_OPDCMD_OPENDRAIN_Val << HSMCI_CMDR_OPDCMD_Pos) /**< (HSMCI_CMDR) Open drain command. Position  */
#define HSMCI_CMDR_MAXLAT_Pos                 _U_(12)                                              /**< (HSMCI_CMDR) Max Latency for Command to Response Position */
#define HSMCI_CMDR_MAXLAT_Msk                 (_U_(0x1) << HSMCI_CMDR_MAXLAT_Pos)                  /**< (HSMCI_CMDR) Max Latency for Command to Response Mask */
#define HSMCI_CMDR_MAXLAT(value)              (HSMCI_CMDR_MAXLAT_Msk & ((value) << HSMCI_CMDR_MAXLAT_Pos))
#define   HSMCI_CMDR_MAXLAT_5_Val             _U_(0x0)                                             /**< (HSMCI_CMDR) 5-cycle max latency.  */
#define   HSMCI_CMDR_MAXLAT_64_Val            _U_(0x1)                                             /**< (HSMCI_CMDR) 64-cycle max latency.  */
#define HSMCI_CMDR_MAXLAT_5                   (HSMCI_CMDR_MAXLAT_5_Val << HSMCI_CMDR_MAXLAT_Pos)   /**< (HSMCI_CMDR) 5-cycle max latency. Position  */
#define HSMCI_CMDR_MAXLAT_64                  (HSMCI_CMDR_MAXLAT_64_Val << HSMCI_CMDR_MAXLAT_Pos)  /**< (HSMCI_CMDR) 64-cycle max latency. Position  */
#define HSMCI_CMDR_TRCMD_Pos                  _U_(16)                                              /**< (HSMCI_CMDR) Transfer Command Position */
#define HSMCI_CMDR_TRCMD_Msk                  (_U_(0x3) << HSMCI_CMDR_TRCMD_Pos)                   /**< (HSMCI_CMDR) Transfer Command Mask */
#define HSMCI_CMDR_TRCMD(value)               (HSMCI_CMDR_TRCMD_Msk & ((value) << HSMCI_CMDR_TRCMD_Pos))
#define   HSMCI_CMDR_TRCMD_NO_DATA_Val        _U_(0x0)                                             /**< (HSMCI_CMDR) No data transfer  */
#define   HSMCI_CMDR_TRCMD_START_DATA_Val     _U_(0x1)                                             /**< (HSMCI_CMDR) Start data transfer  */
#define   HSMCI_CMDR_TRCMD_STOP_DATA_Val      _U_(0x2)                                             /**< (HSMCI_CMDR) Stop data transfer  */
#define HSMCI_CMDR_TRCMD_NO_DATA              (HSMCI_CMDR_TRCMD_NO_DATA_Val << HSMCI_CMDR_TRCMD_Pos) /**< (HSMCI_CMDR) No data transfer Position  */
#define HSMCI_CMDR_TRCMD_START_DATA           (HSMCI_CMDR_TRCMD_START_DATA_Val << HSMCI_CMDR_TRCMD_Pos) /**< (HSMCI_CMDR) Start data transfer Position  */
#define HSMCI_CMDR_TRCMD_STOP_DATA            (HSMCI_CMDR_TRCMD_STOP_DATA_Val << HSMCI_CMDR_TRCMD_Pos) /**< (HSMCI_CMDR) Stop data transfer Position  */
#define HSMCI_CMDR_TRDIR_Pos                  _U_(18)                                              /**< (HSMCI_CMDR) Transfer Direction Position */
#define HSMCI_CMDR_TRDIR_Msk                  (_U_(0x1) << HSMCI_CMDR_TRDIR_Pos)                   /**< (HSMCI_CMDR) Transfer Direction Mask */
#define HSMCI_CMDR_TRDIR(value)               (HSMCI_CMDR_TRDIR_Msk & ((value) << HSMCI_CMDR_TRDIR_Pos))
#define   HSMCI_CMDR_TRDIR_WRITE_Val          _U_(0x0)                                             /**< (HSMCI_CMDR) Write.  */
#define   HSMCI_CMDR_TRDIR_READ_Val           _U_(0x1)                                             /**< (HSMCI_CMDR) Read.  */
#define HSMCI_CMDR_TRDIR_WRITE                (HSMCI_CMDR_TRDIR_WRITE_Val << HSMCI_CMDR_TRDIR_Pos) /**< (HSMCI_CMDR) Write. Position  */
#define HSMCI_CMDR_TRDIR_READ                 (HSMCI_CMDR_TRDIR_READ_Val << HSMCI_CMDR_TRDIR_Pos)  /**< (HSMCI_CMDR) Read. Position  */
#define HSMCI_CMDR_TRTYP_Pos                  _U_(19)                                              /**< (HSMCI_CMDR) Transfer Type Position */
#define HSMCI_CMDR_TRTYP_Msk                  (_U_(0x7) << HSMCI_CMDR_TRTYP_Pos)                   /**< (HSMCI_CMDR) Transfer Type Mask */
#define HSMCI_CMDR_TRTYP(value)               (HSMCI_CMDR_TRTYP_Msk & ((value) << HSMCI_CMDR_TRTYP_Pos))
#define   HSMCI_CMDR_TRTYP_SINGLE_Val         _U_(0x0)                                             /**< (HSMCI_CMDR) MMC/SD Card Single Block  */
#define   HSMCI_CMDR_TRTYP_MULTIPLE_Val       _U_(0x1)                                             /**< (HSMCI_CMDR) MMC/SD Card Multiple Block  */
#define   HSMCI_CMDR_TRTYP_STREAM_Val         _U_(0x2)                                             /**< (HSMCI_CMDR) MMC Stream  */
#define   HSMCI_CMDR_TRTYP_BYTE_Val           _U_(0x4)                                             /**< (HSMCI_CMDR) SDIO Byte  */
#define   HSMCI_CMDR_TRTYP_BLOCK_Val          _U_(0x5)                                             /**< (HSMCI_CMDR) SDIO Block  */
#define HSMCI_CMDR_TRTYP_SINGLE               (HSMCI_CMDR_TRTYP_SINGLE_Val << HSMCI_CMDR_TRTYP_Pos) /**< (HSMCI_CMDR) MMC/SD Card Single Block Position  */
#define HSMCI_CMDR_TRTYP_MULTIPLE             (HSMCI_CMDR_TRTYP_MULTIPLE_Val << HSMCI_CMDR_TRTYP_Pos) /**< (HSMCI_CMDR) MMC/SD Card Multiple Block Position  */
#define HSMCI_CMDR_TRTYP_STREAM               (HSMCI_CMDR_TRTYP_STREAM_Val << HSMCI_CMDR_TRTYP_Pos) /**< (HSMCI_CMDR) MMC Stream Position  */
#define HSMCI_CMDR_TRTYP_BYTE                 (HSMCI_CMDR_TRTYP_BYTE_Val << HSMCI_CMDR_TRTYP_Pos)  /**< (HSMCI_CMDR) SDIO Byte Position  */
#define HSMCI_CMDR_TRTYP_BLOCK                (HSMCI_CMDR_TRTYP_BLOCK_Val << HSMCI_CMDR_TRTYP_Pos) /**< (HSMCI_CMDR) SDIO Block Position  */
#define HSMCI_CMDR_IOSPCMD_Pos                _U_(24)                                              /**< (HSMCI_CMDR) SDIO Special Command Position */
#define HSMCI_CMDR_IOSPCMD_Msk                (_U_(0x3) << HSMCI_CMDR_IOSPCMD_Pos)                 /**< (HSMCI_CMDR) SDIO Special Command Mask */
#define HSMCI_CMDR_IOSPCMD(value)             (HSMCI_CMDR_IOSPCMD_Msk & ((value) << HSMCI_CMDR_IOSPCMD_Pos))
#define   HSMCI_CMDR_IOSPCMD_STD_Val          _U_(0x0)                                             /**< (HSMCI_CMDR) Not an SDIO Special Command  */
#define   HSMCI_CMDR_IOSPCMD_SUSPEND_Val      _U_(0x1)                                             /**< (HSMCI_CMDR) SDIO Suspend Command  */
#define   HSMCI_CMDR_IOSPCMD_RESUME_Val       _U_(0x2)                                             /**< (HSMCI_CMDR) SDIO Resume Command  */
#define HSMCI_CMDR_IOSPCMD_STD                (HSMCI_CMDR_IOSPCMD_STD_Val << HSMCI_CMDR_IOSPCMD_Pos) /**< (HSMCI_CMDR) Not an SDIO Special Command Position  */
#define HSMCI_CMDR_IOSPCMD_SUSPEND            (HSMCI_CMDR_IOSPCMD_SUSPEND_Val << HSMCI_CMDR_IOSPCMD_Pos) /**< (HSMCI_CMDR) SDIO Suspend Command Position  */
#define HSMCI_CMDR_IOSPCMD_RESUME             (HSMCI_CMDR_IOSPCMD_RESUME_Val << HSMCI_CMDR_IOSPCMD_Pos) /**< (HSMCI_CMDR) SDIO Resume Command Position  */
#define HSMCI_CMDR_ATACS_Pos                  _U_(26)                                              /**< (HSMCI_CMDR) ATA with Command Completion Signal Position */
#define HSMCI_CMDR_ATACS_Msk                  (_U_(0x1) << HSMCI_CMDR_ATACS_Pos)                   /**< (HSMCI_CMDR) ATA with Command Completion Signal Mask */
#define HSMCI_CMDR_ATACS(value)               (HSMCI_CMDR_ATACS_Msk & ((value) << HSMCI_CMDR_ATACS_Pos))
#define   HSMCI_CMDR_ATACS_NORMAL_Val         _U_(0x0)                                             /**< (HSMCI_CMDR) Normal operation mode.  */
#define   HSMCI_CMDR_ATACS_COMPLETION_Val     _U_(0x1)                                             /**< (HSMCI_CMDR) This bit indicates that a completion signal is expected within a programmed amount of time (HSMCI_CSTOR).  */
#define HSMCI_CMDR_ATACS_NORMAL               (HSMCI_CMDR_ATACS_NORMAL_Val << HSMCI_CMDR_ATACS_Pos) /**< (HSMCI_CMDR) Normal operation mode. Position  */
#define HSMCI_CMDR_ATACS_COMPLETION           (HSMCI_CMDR_ATACS_COMPLETION_Val << HSMCI_CMDR_ATACS_Pos) /**< (HSMCI_CMDR) This bit indicates that a completion signal is expected within a programmed amount of time (HSMCI_CSTOR). Position  */
#define HSMCI_CMDR_BOOT_ACK_Pos               _U_(27)                                              /**< (HSMCI_CMDR) Boot Operation Acknowledge Position */
#define HSMCI_CMDR_BOOT_ACK_Msk               (_U_(0x1) << HSMCI_CMDR_BOOT_ACK_Pos)                /**< (HSMCI_CMDR) Boot Operation Acknowledge Mask */
#define HSMCI_CMDR_BOOT_ACK(value)            (HSMCI_CMDR_BOOT_ACK_Msk & ((value) << HSMCI_CMDR_BOOT_ACK_Pos))
#define HSMCI_CMDR_Msk                        _U_(0x0F3F1FFF)                                      /**< (HSMCI_CMDR) Register Mask  */


/* -------- HSMCI_BLKR : (HSMCI Offset: 0x18) (R/W 32) Block Register -------- */
#define HSMCI_BLKR_BCNT_Pos                   _U_(0)                                               /**< (HSMCI_BLKR) MMC/SDIO Block Count - SDIO Byte Count Position */
#define HSMCI_BLKR_BCNT_Msk                   (_U_(0xFFFF) << HSMCI_BLKR_BCNT_Pos)                 /**< (HSMCI_BLKR) MMC/SDIO Block Count - SDIO Byte Count Mask */
#define HSMCI_BLKR_BCNT(value)                (HSMCI_BLKR_BCNT_Msk & ((value) << HSMCI_BLKR_BCNT_Pos))
#define HSMCI_BLKR_BLKLEN_Pos                 _U_(16)                                              /**< (HSMCI_BLKR) Data Block Length Position */
#define HSMCI_BLKR_BLKLEN_Msk                 (_U_(0xFFFF) << HSMCI_BLKR_BLKLEN_Pos)               /**< (HSMCI_BLKR) Data Block Length Mask */
#define HSMCI_BLKR_BLKLEN(value)              (HSMCI_BLKR_BLKLEN_Msk & ((value) << HSMCI_BLKR_BLKLEN_Pos))
#define HSMCI_BLKR_Msk                        _U_(0xFFFFFFFF)                                      /**< (HSMCI_BLKR) Register Mask  */


/* -------- HSMCI_CSTOR : (HSMCI Offset: 0x1C) (R/W 32) Completion Signal Timeout Register -------- */
#define HSMCI_CSTOR_CSTOCYC_Pos               _U_(0)                                               /**< (HSMCI_CSTOR) Completion Signal Timeout Cycle Number Position */
#define HSMCI_CSTOR_CSTOCYC_Msk               (_U_(0xF) << HSMCI_CSTOR_CSTOCYC_Pos)                /**< (HSMCI_CSTOR) Completion Signal Timeout Cycle Number Mask */
#define HSMCI_CSTOR_CSTOCYC(value)            (HSMCI_CSTOR_CSTOCYC_Msk & ((value) << HSMCI_CSTOR_CSTOCYC_Pos))
#define HSMCI_CSTOR_CSTOMUL_Pos               _U_(4)                                               /**< (HSMCI_CSTOR) Completion Signal Timeout Multiplier Position */
#define HSMCI_CSTOR_CSTOMUL_Msk               (_U_(0x7) << HSMCI_CSTOR_CSTOMUL_Pos)                /**< (HSMCI_CSTOR) Completion Signal Timeout Multiplier Mask */
#define HSMCI_CSTOR_CSTOMUL(value)            (HSMCI_CSTOR_CSTOMUL_Msk & ((value) << HSMCI_CSTOR_CSTOMUL_Pos))
#define   HSMCI_CSTOR_CSTOMUL_1_Val           _U_(0x0)                                             /**< (HSMCI_CSTOR) CSTOCYC x 1  */
#define   HSMCI_CSTOR_CSTOMUL_16_Val          _U_(0x1)                                             /**< (HSMCI_CSTOR) CSTOCYC x 16  */
#define   HSMCI_CSTOR_CSTOMUL_128_Val         _U_(0x2)                                             /**< (HSMCI_CSTOR) CSTOCYC x 128  */
#define   HSMCI_CSTOR_CSTOMUL_256_Val         _U_(0x3)                                             /**< (HSMCI_CSTOR) CSTOCYC x 256  */
#define   HSMCI_CSTOR_CSTOMUL_1024_Val        _U_(0x4)                                             /**< (HSMCI_CSTOR) CSTOCYC x 1024  */
#define   HSMCI_CSTOR_CSTOMUL_4096_Val        _U_(0x5)                                             /**< (HSMCI_CSTOR) CSTOCYC x 4096  */
#define   HSMCI_CSTOR_CSTOMUL_65536_Val       _U_(0x6)                                             /**< (HSMCI_CSTOR) CSTOCYC x 65536  */
#define   HSMCI_CSTOR_CSTOMUL_1048576_Val     _U_(0x7)                                             /**< (HSMCI_CSTOR) CSTOCYC x 1048576  */
#define HSMCI_CSTOR_CSTOMUL_1                 (HSMCI_CSTOR_CSTOMUL_1_Val << HSMCI_CSTOR_CSTOMUL_Pos) /**< (HSMCI_CSTOR) CSTOCYC x 1 Position  */
#define HSMCI_CSTOR_CSTOMUL_16                (HSMCI_CSTOR_CSTOMUL_16_Val << HSMCI_CSTOR_CSTOMUL_Pos) /**< (HSMCI_CSTOR) CSTOCYC x 16 Position  */
#define HSMCI_CSTOR_CSTOMUL_128               (HSMCI_CSTOR_CSTOMUL_128_Val << HSMCI_CSTOR_CSTOMUL_Pos) /**< (HSMCI_CSTOR) CSTOCYC x 128 Position  */
#define HSMCI_CSTOR_CSTOMUL_256               (HSMCI_CSTOR_CSTOMUL_256_Val << HSMCI_CSTOR_CSTOMUL_Pos) /**< (HSMCI_CSTOR) CSTOCYC x 256 Position  */
#define HSMCI_CSTOR_CSTOMUL_1024              (HSMCI_CSTOR_CSTOMUL_1024_Val << HSMCI_CSTOR_CSTOMUL_Pos) /**< (HSMCI_CSTOR) CSTOCYC x 1024 Position  */
#define HSMCI_CSTOR_CSTOMUL_4096              (HSMCI_CSTOR_CSTOMUL_4096_Val << HSMCI_CSTOR_CSTOMUL_Pos) /**< (HSMCI_CSTOR) CSTOCYC x 4096 Position  */
#define HSMCI_CSTOR_CSTOMUL_65536             (HSMCI_CSTOR_CSTOMUL_65536_Val << HSMCI_CSTOR_CSTOMUL_Pos) /**< (HSMCI_CSTOR) CSTOCYC x 65536 Position  */
#define HSMCI_CSTOR_CSTOMUL_1048576           (HSMCI_CSTOR_CSTOMUL_1048576_Val << HSMCI_CSTOR_CSTOMUL_Pos) /**< (HSMCI_CSTOR) CSTOCYC x 1048576 Position  */
#define HSMCI_CSTOR_Msk                       _U_(0x0000007F)                                      /**< (HSMCI_CSTOR) Register Mask  */


/* -------- HSMCI_RSPR : (HSMCI Offset: 0x20) ( R/ 32) Response Register 0 -------- */
#define HSMCI_RSPR_RSP_Pos                    _U_(0)                                               /**< (HSMCI_RSPR) Response Position */
#define HSMCI_RSPR_RSP_Msk                    (_U_(0xFFFFFFFF) << HSMCI_RSPR_RSP_Pos)              /**< (HSMCI_RSPR) Response Mask */
#define HSMCI_RSPR_RSP(value)                 (HSMCI_RSPR_RSP_Msk & ((value) << HSMCI_RSPR_RSP_Pos))
#define HSMCI_RSPR_Msk                        _U_(0xFFFFFFFF)                                      /**< (HSMCI_RSPR) Register Mask  */


/* -------- HSMCI_RDR : (HSMCI Offset: 0x30) ( R/ 32) Receive Data Register -------- */
#define HSMCI_RDR_DATA_Pos                    _U_(0)                                               /**< (HSMCI_RDR) Data to Read Position */
#define HSMCI_RDR_DATA_Msk                    (_U_(0xFFFFFFFF) << HSMCI_RDR_DATA_Pos)              /**< (HSMCI_RDR) Data to Read Mask */
#define HSMCI_RDR_DATA(value)                 (HSMCI_RDR_DATA_Msk & ((value) << HSMCI_RDR_DATA_Pos))
#define HSMCI_RDR_Msk                         _U_(0xFFFFFFFF)                                      /**< (HSMCI_RDR) Register Mask  */


/* -------- HSMCI_TDR : (HSMCI Offset: 0x34) ( /W 32) Transmit Data Register -------- */
#define HSMCI_TDR_DATA_Pos                    _U_(0)                                               /**< (HSMCI_TDR) Data to Write Position */
#define HSMCI_TDR_DATA_Msk                    (_U_(0xFFFFFFFF) << HSMCI_TDR_DATA_Pos)              /**< (HSMCI_TDR) Data to Write Mask */
#define HSMCI_TDR_DATA(value)                 (HSMCI_TDR_DATA_Msk & ((value) << HSMCI_TDR_DATA_Pos))
#define HSMCI_TDR_Msk                         _U_(0xFFFFFFFF)                                      /**< (HSMCI_TDR) Register Mask  */


/* -------- HSMCI_SR : (HSMCI Offset: 0x40) ( R/ 32) Status Register -------- */
#define HSMCI_SR_CMDRDY_Pos                   _U_(0)                                               /**< (HSMCI_SR) Command Ready (cleared by writing in HSMCI_CMDR) Position */
#define HSMCI_SR_CMDRDY_Msk                   (_U_(0x1) << HSMCI_SR_CMDRDY_Pos)                    /**< (HSMCI_SR) Command Ready (cleared by writing in HSMCI_CMDR) Mask */
#define HSMCI_SR_CMDRDY(value)                (HSMCI_SR_CMDRDY_Msk & ((value) << HSMCI_SR_CMDRDY_Pos))
#define HSMCI_SR_RXRDY_Pos                    _U_(1)                                               /**< (HSMCI_SR) Receiver Ready (cleared by reading HSMCI_RDR) Position */
#define HSMCI_SR_RXRDY_Msk                    (_U_(0x1) << HSMCI_SR_RXRDY_Pos)                     /**< (HSMCI_SR) Receiver Ready (cleared by reading HSMCI_RDR) Mask */
#define HSMCI_SR_RXRDY(value)                 (HSMCI_SR_RXRDY_Msk & ((value) << HSMCI_SR_RXRDY_Pos))
#define HSMCI_SR_TXRDY_Pos                    _U_(2)                                               /**< (HSMCI_SR) Transmit Ready (cleared by writing in HSMCI_TDR) Position */
#define HSMCI_SR_TXRDY_Msk                    (_U_(0x1) << HSMCI_SR_TXRDY_Pos)                     /**< (HSMCI_SR) Transmit Ready (cleared by writing in HSMCI_TDR) Mask */
#define HSMCI_SR_TXRDY(value)                 (HSMCI_SR_TXRDY_Msk & ((value) << HSMCI_SR_TXRDY_Pos))
#define HSMCI_SR_BLKE_Pos                     _U_(3)                                               /**< (HSMCI_SR) Data Block Ended (cleared on read) Position */
#define HSMCI_SR_BLKE_Msk                     (_U_(0x1) << HSMCI_SR_BLKE_Pos)                      /**< (HSMCI_SR) Data Block Ended (cleared on read) Mask */
#define HSMCI_SR_BLKE(value)                  (HSMCI_SR_BLKE_Msk & ((value) << HSMCI_SR_BLKE_Pos))
#define HSMCI_SR_DTIP_Pos                     _U_(4)                                               /**< (HSMCI_SR) Data Transfer in Progress (cleared at the end of CRC16 calculation) Position */
#define HSMCI_SR_DTIP_Msk                     (_U_(0x1) << HSMCI_SR_DTIP_Pos)                      /**< (HSMCI_SR) Data Transfer in Progress (cleared at the end of CRC16 calculation) Mask */
#define HSMCI_SR_DTIP(value)                  (HSMCI_SR_DTIP_Msk & ((value) << HSMCI_SR_DTIP_Pos))
#define HSMCI_SR_NOTBUSY_Pos                  _U_(5)                                               /**< (HSMCI_SR) HSMCI Not Busy Position */
#define HSMCI_SR_NOTBUSY_Msk                  (_U_(0x1) << HSMCI_SR_NOTBUSY_Pos)                   /**< (HSMCI_SR) HSMCI Not Busy Mask */
#define HSMCI_SR_NOTBUSY(value)               (HSMCI_SR_NOTBUSY_Msk & ((value) << HSMCI_SR_NOTBUSY_Pos))
#define HSMCI_SR_SDIOIRQA_Pos                 _U_(8)                                               /**< (HSMCI_SR) SDIO Interrupt for Slot A (cleared on read) Position */
#define HSMCI_SR_SDIOIRQA_Msk                 (_U_(0x1) << HSMCI_SR_SDIOIRQA_Pos)                  /**< (HSMCI_SR) SDIO Interrupt for Slot A (cleared on read) Mask */
#define HSMCI_SR_SDIOIRQA(value)              (HSMCI_SR_SDIOIRQA_Msk & ((value) << HSMCI_SR_SDIOIRQA_Pos))
#define HSMCI_SR_SDIOWAIT_Pos                 _U_(12)                                              /**< (HSMCI_SR) SDIO Read Wait Operation Status Position */
#define HSMCI_SR_SDIOWAIT_Msk                 (_U_(0x1) << HSMCI_SR_SDIOWAIT_Pos)                  /**< (HSMCI_SR) SDIO Read Wait Operation Status Mask */
#define HSMCI_SR_SDIOWAIT(value)              (HSMCI_SR_SDIOWAIT_Msk & ((value) << HSMCI_SR_SDIOWAIT_Pos))
#define HSMCI_SR_CSRCV_Pos                    _U_(13)                                              /**< (HSMCI_SR) CE-ATA Completion Signal Received (cleared on read) Position */
#define HSMCI_SR_CSRCV_Msk                    (_U_(0x1) << HSMCI_SR_CSRCV_Pos)                     /**< (HSMCI_SR) CE-ATA Completion Signal Received (cleared on read) Mask */
#define HSMCI_SR_CSRCV(value)                 (HSMCI_SR_CSRCV_Msk & ((value) << HSMCI_SR_CSRCV_Pos))
#define HSMCI_SR_RINDE_Pos                    _U_(16)                                              /**< (HSMCI_SR) Response Index Error (cleared by writing in HSMCI_CMDR) Position */
#define HSMCI_SR_RINDE_Msk                    (_U_(0x1) << HSMCI_SR_RINDE_Pos)                     /**< (HSMCI_SR) Response Index Error (cleared by writing in HSMCI_CMDR) Mask */
#define HSMCI_SR_RINDE(value)                 (HSMCI_SR_RINDE_Msk & ((value) << HSMCI_SR_RINDE_Pos))
#define HSMCI_SR_RDIRE_Pos                    _U_(17)                                              /**< (HSMCI_SR) Response Direction Error (cleared by writing in HSMCI_CMDR) Position */
#define HSMCI_SR_RDIRE_Msk                    (_U_(0x1) << HSMCI_SR_RDIRE_Pos)                     /**< (HSMCI_SR) Response Direction Error (cleared by writing in HSMCI_CMDR) Mask */
#define HSMCI_SR_RDIRE(value)                 (HSMCI_SR_RDIRE_Msk & ((value) << HSMCI_SR_RDIRE_Pos))
#define HSMCI_SR_RCRCE_Pos                    _U_(18)                                              /**< (HSMCI_SR) Response CRC Error (cleared by writing in HSMCI_CMDR) Position */
#define HSMCI_SR_RCRCE_Msk                    (_U_(0x1) << HSMCI_SR_RCRCE_Pos)                     /**< (HSMCI_SR) Response CRC Error (cleared by writing in HSMCI_CMDR) Mask */
#define HSMCI_SR_RCRCE(value)                 (HSMCI_SR_RCRCE_Msk & ((value) << HSMCI_SR_RCRCE_Pos))
#define HSMCI_SR_RENDE_Pos                    _U_(19)                                              /**< (HSMCI_SR) Response End Bit Error (cleared by writing in HSMCI_CMDR) Position */
#define HSMCI_SR_RENDE_Msk                    (_U_(0x1) << HSMCI_SR_RENDE_Pos)                     /**< (HSMCI_SR) Response End Bit Error (cleared by writing in HSMCI_CMDR) Mask */
#define HSMCI_SR_RENDE(value)                 (HSMCI_SR_RENDE_Msk & ((value) << HSMCI_SR_RENDE_Pos))
#define HSMCI_SR_RTOE_Pos                     _U_(20)                                              /**< (HSMCI_SR) Response Time-out Error (cleared by writing in HSMCI_CMDR) Position */
#define HSMCI_SR_RTOE_Msk                     (_U_(0x1) << HSMCI_SR_RTOE_Pos)                      /**< (HSMCI_SR) Response Time-out Error (cleared by writing in HSMCI_CMDR) Mask */
#define HSMCI_SR_RTOE(value)                  (HSMCI_SR_RTOE_Msk & ((value) << HSMCI_SR_RTOE_Pos))
#define HSMCI_SR_DCRCE_Pos                    _U_(21)                                              /**< (HSMCI_SR) Data CRC Error (cleared on read) Position */
#define HSMCI_SR_DCRCE_Msk                    (_U_(0x1) << HSMCI_SR_DCRCE_Pos)                     /**< (HSMCI_SR) Data CRC Error (cleared on read) Mask */
#define HSMCI_SR_DCRCE(value)                 (HSMCI_SR_DCRCE_Msk & ((value) << HSMCI_SR_DCRCE_Pos))
#define HSMCI_SR_DTOE_Pos                     _U_(22)                                              /**< (HSMCI_SR) Data Time-out Error (cleared on read) Position */
#define HSMCI_SR_DTOE_Msk                     (_U_(0x1) << HSMCI_SR_DTOE_Pos)                      /**< (HSMCI_SR) Data Time-out Error (cleared on read) Mask */
#define HSMCI_SR_DTOE(value)                  (HSMCI_SR_DTOE_Msk & ((value) << HSMCI_SR_DTOE_Pos))
#define HSMCI_SR_CSTOE_Pos                    _U_(23)                                              /**< (HSMCI_SR) Completion Signal Time-out Error (cleared on read) Position */
#define HSMCI_SR_CSTOE_Msk                    (_U_(0x1) << HSMCI_SR_CSTOE_Pos)                     /**< (HSMCI_SR) Completion Signal Time-out Error (cleared on read) Mask */
#define HSMCI_SR_CSTOE(value)                 (HSMCI_SR_CSTOE_Msk & ((value) << HSMCI_SR_CSTOE_Pos))
#define HSMCI_SR_BLKOVRE_Pos                  _U_(24)                                              /**< (HSMCI_SR) DMA Block Overrun Error (cleared on read) Position */
#define HSMCI_SR_BLKOVRE_Msk                  (_U_(0x1) << HSMCI_SR_BLKOVRE_Pos)                   /**< (HSMCI_SR) DMA Block Overrun Error (cleared on read) Mask */
#define HSMCI_SR_BLKOVRE(value)               (HSMCI_SR_BLKOVRE_Msk & ((value) << HSMCI_SR_BLKOVRE_Pos))
#define HSMCI_SR_FIFOEMPTY_Pos                _U_(26)                                              /**< (HSMCI_SR) FIFO empty flag Position */
#define HSMCI_SR_FIFOEMPTY_Msk                (_U_(0x1) << HSMCI_SR_FIFOEMPTY_Pos)                 /**< (HSMCI_SR) FIFO empty flag Mask */
#define HSMCI_SR_FIFOEMPTY(value)             (HSMCI_SR_FIFOEMPTY_Msk & ((value) << HSMCI_SR_FIFOEMPTY_Pos))
#define HSMCI_SR_XFRDONE_Pos                  _U_(27)                                              /**< (HSMCI_SR) Transfer Done flag Position */
#define HSMCI_SR_XFRDONE_Msk                  (_U_(0x1) << HSMCI_SR_XFRDONE_Pos)                   /**< (HSMCI_SR) Transfer Done flag Mask */
#define HSMCI_SR_XFRDONE(value)               (HSMCI_SR_XFRDONE_Msk & ((value) << HSMCI_SR_XFRDONE_Pos))
#define HSMCI_SR_ACKRCV_Pos                   _U_(28)                                              /**< (HSMCI_SR) Boot Operation Acknowledge Received (cleared on read) Position */
#define HSMCI_SR_ACKRCV_Msk                   (_U_(0x1) << HSMCI_SR_ACKRCV_Pos)                    /**< (HSMCI_SR) Boot Operation Acknowledge Received (cleared on read) Mask */
#define HSMCI_SR_ACKRCV(value)                (HSMCI_SR_ACKRCV_Msk & ((value) << HSMCI_SR_ACKRCV_Pos))
#define HSMCI_SR_ACKRCVE_Pos                  _U_(29)                                              /**< (HSMCI_SR) Boot Operation Acknowledge Error (cleared on read) Position */
#define HSMCI_SR_ACKRCVE_Msk                  (_U_(0x1) << HSMCI_SR_ACKRCVE_Pos)                   /**< (HSMCI_SR) Boot Operation Acknowledge Error (cleared on read) Mask */
#define HSMCI_SR_ACKRCVE(value)               (HSMCI_SR_ACKRCVE_Msk & ((value) << HSMCI_SR_ACKRCVE_Pos))
#define HSMCI_SR_OVRE_Pos                     _U_(30)                                              /**< (HSMCI_SR) Overrun (if FERRCTRL = 1, cleared by writing in HSMCI_CMDR or cleared on read if FERRCTRL = 0) Position */
#define HSMCI_SR_OVRE_Msk                     (_U_(0x1) << HSMCI_SR_OVRE_Pos)                      /**< (HSMCI_SR) Overrun (if FERRCTRL = 1, cleared by writing in HSMCI_CMDR or cleared on read if FERRCTRL = 0) Mask */
#define HSMCI_SR_OVRE(value)                  (HSMCI_SR_OVRE_Msk & ((value) << HSMCI_SR_OVRE_Pos))
#define HSMCI_SR_UNRE_Pos                     _U_(31)                                              /**< (HSMCI_SR) Underrun (if FERRCTRL = 1, cleared by writing in HSMCI_CMDR or cleared on read if FERRCTRL = 0) Position */
#define HSMCI_SR_UNRE_Msk                     (_U_(0x1) << HSMCI_SR_UNRE_Pos)                      /**< (HSMCI_SR) Underrun (if FERRCTRL = 1, cleared by writing in HSMCI_CMDR or cleared on read if FERRCTRL = 0) Mask */
#define HSMCI_SR_UNRE(value)                  (HSMCI_SR_UNRE_Msk & ((value) << HSMCI_SR_UNRE_Pos))
#define HSMCI_SR_Msk                          _U_(0xFDFF313F)                                      /**< (HSMCI_SR) Register Mask  */


/* -------- HSMCI_IER : (HSMCI Offset: 0x44) ( /W 32) Interrupt Enable Register -------- */
#define HSMCI_IER_CMDRDY_Pos                  _U_(0)                                               /**< (HSMCI_IER) Command Ready Interrupt Enable Position */
#define HSMCI_IER_CMDRDY_Msk                  (_U_(0x1) << HSMCI_IER_CMDRDY_Pos)                   /**< (HSMCI_IER) Command Ready Interrupt Enable Mask */
#define HSMCI_IER_CMDRDY(value)               (HSMCI_IER_CMDRDY_Msk & ((value) << HSMCI_IER_CMDRDY_Pos))
#define HSMCI_IER_RXRDY_Pos                   _U_(1)                                               /**< (HSMCI_IER) Receiver Ready Interrupt Enable Position */
#define HSMCI_IER_RXRDY_Msk                   (_U_(0x1) << HSMCI_IER_RXRDY_Pos)                    /**< (HSMCI_IER) Receiver Ready Interrupt Enable Mask */
#define HSMCI_IER_RXRDY(value)                (HSMCI_IER_RXRDY_Msk & ((value) << HSMCI_IER_RXRDY_Pos))
#define HSMCI_IER_TXRDY_Pos                   _U_(2)                                               /**< (HSMCI_IER) Transmit Ready Interrupt Enable Position */
#define HSMCI_IER_TXRDY_Msk                   (_U_(0x1) << HSMCI_IER_TXRDY_Pos)                    /**< (HSMCI_IER) Transmit Ready Interrupt Enable Mask */
#define HSMCI_IER_TXRDY(value)                (HSMCI_IER_TXRDY_Msk & ((value) << HSMCI_IER_TXRDY_Pos))
#define HSMCI_IER_BLKE_Pos                    _U_(3)                                               /**< (HSMCI_IER) Data Block Ended Interrupt Enable Position */
#define HSMCI_IER_BLKE_Msk                    (_U_(0x1) << HSMCI_IER_BLKE_Pos)                     /**< (HSMCI_IER) Data Block Ended Interrupt Enable Mask */
#define HSMCI_IER_BLKE(value)                 (HSMCI_IER_BLKE_Msk & ((value) << HSMCI_IER_BLKE_Pos))
#define HSMCI_IER_DTIP_Pos                    _U_(4)                                               /**< (HSMCI_IER) Data Transfer in Progress Interrupt Enable Position */
#define HSMCI_IER_DTIP_Msk                    (_U_(0x1) << HSMCI_IER_DTIP_Pos)                     /**< (HSMCI_IER) Data Transfer in Progress Interrupt Enable Mask */
#define HSMCI_IER_DTIP(value)                 (HSMCI_IER_DTIP_Msk & ((value) << HSMCI_IER_DTIP_Pos))
#define HSMCI_IER_NOTBUSY_Pos                 _U_(5)                                               /**< (HSMCI_IER) Data Not Busy Interrupt Enable Position */
#define HSMCI_IER_NOTBUSY_Msk                 (_U_(0x1) << HSMCI_IER_NOTBUSY_Pos)                  /**< (HSMCI_IER) Data Not Busy Interrupt Enable Mask */
#define HSMCI_IER_NOTBUSY(value)              (HSMCI_IER_NOTBUSY_Msk & ((value) << HSMCI_IER_NOTBUSY_Pos))
#define HSMCI_IER_SDIOIRQA_Pos                _U_(8)                                               /**< (HSMCI_IER) SDIO Interrupt for Slot A Interrupt Enable Position */
#define HSMCI_IER_SDIOIRQA_Msk                (_U_(0x1) << HSMCI_IER_SDIOIRQA_Pos)                 /**< (HSMCI_IER) SDIO Interrupt for Slot A Interrupt Enable Mask */
#define HSMCI_IER_SDIOIRQA(value)             (HSMCI_IER_SDIOIRQA_Msk & ((value) << HSMCI_IER_SDIOIRQA_Pos))
#define HSMCI_IER_SDIOWAIT_Pos                _U_(12)                                              /**< (HSMCI_IER) SDIO Read Wait Operation Status Interrupt Enable Position */
#define HSMCI_IER_SDIOWAIT_Msk                (_U_(0x1) << HSMCI_IER_SDIOWAIT_Pos)                 /**< (HSMCI_IER) SDIO Read Wait Operation Status Interrupt Enable Mask */
#define HSMCI_IER_SDIOWAIT(value)             (HSMCI_IER_SDIOWAIT_Msk & ((value) << HSMCI_IER_SDIOWAIT_Pos))
#define HSMCI_IER_CSRCV_Pos                   _U_(13)                                              /**< (HSMCI_IER) Completion Signal Received Interrupt Enable Position */
#define HSMCI_IER_CSRCV_Msk                   (_U_(0x1) << HSMCI_IER_CSRCV_Pos)                    /**< (HSMCI_IER) Completion Signal Received Interrupt Enable Mask */
#define HSMCI_IER_CSRCV(value)                (HSMCI_IER_CSRCV_Msk & ((value) << HSMCI_IER_CSRCV_Pos))
#define HSMCI_IER_RINDE_Pos                   _U_(16)                                              /**< (HSMCI_IER) Response Index Error Interrupt Enable Position */
#define HSMCI_IER_RINDE_Msk                   (_U_(0x1) << HSMCI_IER_RINDE_Pos)                    /**< (HSMCI_IER) Response Index Error Interrupt Enable Mask */
#define HSMCI_IER_RINDE(value)                (HSMCI_IER_RINDE_Msk & ((value) << HSMCI_IER_RINDE_Pos))
#define HSMCI_IER_RDIRE_Pos                   _U_(17)                                              /**< (HSMCI_IER) Response Direction Error Interrupt Enable Position */
#define HSMCI_IER_RDIRE_Msk                   (_U_(0x1) << HSMCI_IER_RDIRE_Pos)                    /**< (HSMCI_IER) Response Direction Error Interrupt Enable Mask */
#define HSMCI_IER_RDIRE(value)                (HSMCI_IER_RDIRE_Msk & ((value) << HSMCI_IER_RDIRE_Pos))
#define HSMCI_IER_RCRCE_Pos                   _U_(18)                                              /**< (HSMCI_IER) Response CRC Error Interrupt Enable Position */
#define HSMCI_IER_RCRCE_Msk                   (_U_(0x1) << HSMCI_IER_RCRCE_Pos)                    /**< (HSMCI_IER) Response CRC Error Interrupt Enable Mask */
#define HSMCI_IER_RCRCE(value)                (HSMCI_IER_RCRCE_Msk & ((value) << HSMCI_IER_RCRCE_Pos))
#define HSMCI_IER_RENDE_Pos                   _U_(19)                                              /**< (HSMCI_IER) Response End Bit Error Interrupt Enable Position */
#define HSMCI_IER_RENDE_Msk                   (_U_(0x1) << HSMCI_IER_RENDE_Pos)                    /**< (HSMCI_IER) Response End Bit Error Interrupt Enable Mask */
#define HSMCI_IER_RENDE(value)                (HSMCI_IER_RENDE_Msk & ((value) << HSMCI_IER_RENDE_Pos))
#define HSMCI_IER_RTOE_Pos                    _U_(20)                                              /**< (HSMCI_IER) Response Time-out Error Interrupt Enable Position */
#define HSMCI_IER_RTOE_Msk                    (_U_(0x1) << HSMCI_IER_RTOE_Pos)                     /**< (HSMCI_IER) Response Time-out Error Interrupt Enable Mask */
#define HSMCI_IER_RTOE(value)                 (HSMCI_IER_RTOE_Msk & ((value) << HSMCI_IER_RTOE_Pos))
#define HSMCI_IER_DCRCE_Pos                   _U_(21)                                              /**< (HSMCI_IER) Data CRC Error Interrupt Enable Position */
#define HSMCI_IER_DCRCE_Msk                   (_U_(0x1) << HSMCI_IER_DCRCE_Pos)                    /**< (HSMCI_IER) Data CRC Error Interrupt Enable Mask */
#define HSMCI_IER_DCRCE(value)                (HSMCI_IER_DCRCE_Msk & ((value) << HSMCI_IER_DCRCE_Pos))
#define HSMCI_IER_DTOE_Pos                    _U_(22)                                              /**< (HSMCI_IER) Data Time-out Error Interrupt Enable Position */
#define HSMCI_IER_DTOE_Msk                    (_U_(0x1) << HSMCI_IER_DTOE_Pos)                     /**< (HSMCI_IER) Data Time-out Error Interrupt Enable Mask */
#define HSMCI_IER_DTOE(value)                 (HSMCI_IER_DTOE_Msk & ((value) << HSMCI_IER_DTOE_Pos))
#define HSMCI_IER_CSTOE_Pos                   _U_(23)                                              /**< (HSMCI_IER) Completion Signal Timeout Error Interrupt Enable Position */
#define HSMCI_IER_CSTOE_Msk                   (_U_(0x1) << HSMCI_IER_CSTOE_Pos)                    /**< (HSMCI_IER) Completion Signal Timeout Error Interrupt Enable Mask */
#define HSMCI_IER_CSTOE(value)                (HSMCI_IER_CSTOE_Msk & ((value) << HSMCI_IER_CSTOE_Pos))
#define HSMCI_IER_BLKOVRE_Pos                 _U_(24)                                              /**< (HSMCI_IER) DMA Block Overrun Error Interrupt Enable Position */
#define HSMCI_IER_BLKOVRE_Msk                 (_U_(0x1) << HSMCI_IER_BLKOVRE_Pos)                  /**< (HSMCI_IER) DMA Block Overrun Error Interrupt Enable Mask */
#define HSMCI_IER_BLKOVRE(value)              (HSMCI_IER_BLKOVRE_Msk & ((value) << HSMCI_IER_BLKOVRE_Pos))
#define HSMCI_IER_FIFOEMPTY_Pos               _U_(26)                                              /**< (HSMCI_IER) FIFO empty Interrupt enable Position */
#define HSMCI_IER_FIFOEMPTY_Msk               (_U_(0x1) << HSMCI_IER_FIFOEMPTY_Pos)                /**< (HSMCI_IER) FIFO empty Interrupt enable Mask */
#define HSMCI_IER_FIFOEMPTY(value)            (HSMCI_IER_FIFOEMPTY_Msk & ((value) << HSMCI_IER_FIFOEMPTY_Pos))
#define HSMCI_IER_XFRDONE_Pos                 _U_(27)                                              /**< (HSMCI_IER) Transfer Done Interrupt enable Position */
#define HSMCI_IER_XFRDONE_Msk                 (_U_(0x1) << HSMCI_IER_XFRDONE_Pos)                  /**< (HSMCI_IER) Transfer Done Interrupt enable Mask */
#define HSMCI_IER_XFRDONE(value)              (HSMCI_IER_XFRDONE_Msk & ((value) << HSMCI_IER_XFRDONE_Pos))
#define HSMCI_IER_ACKRCV_Pos                  _U_(28)                                              /**< (HSMCI_IER) Boot Acknowledge Interrupt Enable Position */
#define HSMCI_IER_ACKRCV_Msk                  (_U_(0x1) << HSMCI_IER_ACKRCV_Pos)                   /**< (HSMCI_IER) Boot Acknowledge Interrupt Enable Mask */
#define HSMCI_IER_ACKRCV(value)               (HSMCI_IER_ACKRCV_Msk & ((value) << HSMCI_IER_ACKRCV_Pos))
#define HSMCI_IER_ACKRCVE_Pos                 _U_(29)                                              /**< (HSMCI_IER) Boot Acknowledge Error Interrupt Enable Position */
#define HSMCI_IER_ACKRCVE_Msk                 (_U_(0x1) << HSMCI_IER_ACKRCVE_Pos)                  /**< (HSMCI_IER) Boot Acknowledge Error Interrupt Enable Mask */
#define HSMCI_IER_ACKRCVE(value)              (HSMCI_IER_ACKRCVE_Msk & ((value) << HSMCI_IER_ACKRCVE_Pos))
#define HSMCI_IER_OVRE_Pos                    _U_(30)                                              /**< (HSMCI_IER) Overrun Interrupt Enable Position */
#define HSMCI_IER_OVRE_Msk                    (_U_(0x1) << HSMCI_IER_OVRE_Pos)                     /**< (HSMCI_IER) Overrun Interrupt Enable Mask */
#define HSMCI_IER_OVRE(value)                 (HSMCI_IER_OVRE_Msk & ((value) << HSMCI_IER_OVRE_Pos))
#define HSMCI_IER_UNRE_Pos                    _U_(31)                                              /**< (HSMCI_IER) Underrun Interrupt Enable Position */
#define HSMCI_IER_UNRE_Msk                    (_U_(0x1) << HSMCI_IER_UNRE_Pos)                     /**< (HSMCI_IER) Underrun Interrupt Enable Mask */
#define HSMCI_IER_UNRE(value)                 (HSMCI_IER_UNRE_Msk & ((value) << HSMCI_IER_UNRE_Pos))
#define HSMCI_IER_Msk                         _U_(0xFDFF313F)                                      /**< (HSMCI_IER) Register Mask  */


/* -------- HSMCI_IDR : (HSMCI Offset: 0x48) ( /W 32) Interrupt Disable Register -------- */
#define HSMCI_IDR_CMDRDY_Pos                  _U_(0)                                               /**< (HSMCI_IDR) Command Ready Interrupt Disable Position */
#define HSMCI_IDR_CMDRDY_Msk                  (_U_(0x1) << HSMCI_IDR_CMDRDY_Pos)                   /**< (HSMCI_IDR) Command Ready Interrupt Disable Mask */
#define HSMCI_IDR_CMDRDY(value)               (HSMCI_IDR_CMDRDY_Msk & ((value) << HSMCI_IDR_CMDRDY_Pos))
#define HSMCI_IDR_RXRDY_Pos                   _U_(1)                                               /**< (HSMCI_IDR) Receiver Ready Interrupt Disable Position */
#define HSMCI_IDR_RXRDY_Msk                   (_U_(0x1) << HSMCI_IDR_RXRDY_Pos)                    /**< (HSMCI_IDR) Receiver Ready Interrupt Disable Mask */
#define HSMCI_IDR_RXRDY(value)                (HSMCI_IDR_RXRDY_Msk & ((value) << HSMCI_IDR_RXRDY_Pos))
#define HSMCI_IDR_TXRDY_Pos                   _U_(2)                                               /**< (HSMCI_IDR) Transmit Ready Interrupt Disable Position */
#define HSMCI_IDR_TXRDY_Msk                   (_U_(0x1) << HSMCI_IDR_TXRDY_Pos)                    /**< (HSMCI_IDR) Transmit Ready Interrupt Disable Mask */
#define HSMCI_IDR_TXRDY(value)                (HSMCI_IDR_TXRDY_Msk & ((value) << HSMCI_IDR_TXRDY_Pos))
#define HSMCI_IDR_BLKE_Pos                    _U_(3)                                               /**< (HSMCI_IDR) Data Block Ended Interrupt Disable Position */
#define HSMCI_IDR_BLKE_Msk                    (_U_(0x1) << HSMCI_IDR_BLKE_Pos)                     /**< (HSMCI_IDR) Data Block Ended Interrupt Disable Mask */
#define HSMCI_IDR_BLKE(value)                 (HSMCI_IDR_BLKE_Msk & ((value) << HSMCI_IDR_BLKE_Pos))
#define HSMCI_IDR_DTIP_Pos                    _U_(4)                                               /**< (HSMCI_IDR) Data Transfer in Progress Interrupt Disable Position */
#define HSMCI_IDR_DTIP_Msk                    (_U_(0x1) << HSMCI_IDR_DTIP_Pos)                     /**< (HSMCI_IDR) Data Transfer in Progress Interrupt Disable Mask */
#define HSMCI_IDR_DTIP(value)                 (HSMCI_IDR_DTIP_Msk & ((value) << HSMCI_IDR_DTIP_Pos))
#define HSMCI_IDR_NOTBUSY_Pos                 _U_(5)                                               /**< (HSMCI_IDR) Data Not Busy Interrupt Disable Position */
#define HSMCI_IDR_NOTBUSY_Msk                 (_U_(0x1) << HSMCI_IDR_NOTBUSY_Pos)                  /**< (HSMCI_IDR) Data Not Busy Interrupt Disable Mask */
#define HSMCI_IDR_NOTBUSY(value)              (HSMCI_IDR_NOTBUSY_Msk & ((value) << HSMCI_IDR_NOTBUSY_Pos))
#define HSMCI_IDR_SDIOIRQA_Pos                _U_(8)                                               /**< (HSMCI_IDR) SDIO Interrupt for Slot A Interrupt Disable Position */
#define HSMCI_IDR_SDIOIRQA_Msk                (_U_(0x1) << HSMCI_IDR_SDIOIRQA_Pos)                 /**< (HSMCI_IDR) SDIO Interrupt for Slot A Interrupt Disable Mask */
#define HSMCI_IDR_SDIOIRQA(value)             (HSMCI_IDR_SDIOIRQA_Msk & ((value) << HSMCI_IDR_SDIOIRQA_Pos))
#define HSMCI_IDR_SDIOWAIT_Pos                _U_(12)                                              /**< (HSMCI_IDR) SDIO Read Wait Operation Status Interrupt Disable Position */
#define HSMCI_IDR_SDIOWAIT_Msk                (_U_(0x1) << HSMCI_IDR_SDIOWAIT_Pos)                 /**< (HSMCI_IDR) SDIO Read Wait Operation Status Interrupt Disable Mask */
#define HSMCI_IDR_SDIOWAIT(value)             (HSMCI_IDR_SDIOWAIT_Msk & ((value) << HSMCI_IDR_SDIOWAIT_Pos))
#define HSMCI_IDR_CSRCV_Pos                   _U_(13)                                              /**< (HSMCI_IDR) Completion Signal received interrupt Disable Position */
#define HSMCI_IDR_CSRCV_Msk                   (_U_(0x1) << HSMCI_IDR_CSRCV_Pos)                    /**< (HSMCI_IDR) Completion Signal received interrupt Disable Mask */
#define HSMCI_IDR_CSRCV(value)                (HSMCI_IDR_CSRCV_Msk & ((value) << HSMCI_IDR_CSRCV_Pos))
#define HSMCI_IDR_RINDE_Pos                   _U_(16)                                              /**< (HSMCI_IDR) Response Index Error Interrupt Disable Position */
#define HSMCI_IDR_RINDE_Msk                   (_U_(0x1) << HSMCI_IDR_RINDE_Pos)                    /**< (HSMCI_IDR) Response Index Error Interrupt Disable Mask */
#define HSMCI_IDR_RINDE(value)                (HSMCI_IDR_RINDE_Msk & ((value) << HSMCI_IDR_RINDE_Pos))
#define HSMCI_IDR_RDIRE_Pos                   _U_(17)                                              /**< (HSMCI_IDR) Response Direction Error Interrupt Disable Position */
#define HSMCI_IDR_RDIRE_Msk                   (_U_(0x1) << HSMCI_IDR_RDIRE_Pos)                    /**< (HSMCI_IDR) Response Direction Error Interrupt Disable Mask */
#define HSMCI_IDR_RDIRE(value)                (HSMCI_IDR_RDIRE_Msk & ((value) << HSMCI_IDR_RDIRE_Pos))
#define HSMCI_IDR_RCRCE_Pos                   _U_(18)                                              /**< (HSMCI_IDR) Response CRC Error Interrupt Disable Position */
#define HSMCI_IDR_RCRCE_Msk                   (_U_(0x1) << HSMCI_IDR_RCRCE_Pos)                    /**< (HSMCI_IDR) Response CRC Error Interrupt Disable Mask */
#define HSMCI_IDR_RCRCE(value)                (HSMCI_IDR_RCRCE_Msk & ((value) << HSMCI_IDR_RCRCE_Pos))
#define HSMCI_IDR_RENDE_Pos                   _U_(19)                                              /**< (HSMCI_IDR) Response End Bit Error Interrupt Disable Position */
#define HSMCI_IDR_RENDE_Msk                   (_U_(0x1) << HSMCI_IDR_RENDE_Pos)                    /**< (HSMCI_IDR) Response End Bit Error Interrupt Disable Mask */
#define HSMCI_IDR_RENDE(value)                (HSMCI_IDR_RENDE_Msk & ((value) << HSMCI_IDR_RENDE_Pos))
#define HSMCI_IDR_RTOE_Pos                    _U_(20)                                              /**< (HSMCI_IDR) Response Time-out Error Interrupt Disable Position */
#define HSMCI_IDR_RTOE_Msk                    (_U_(0x1) << HSMCI_IDR_RTOE_Pos)                     /**< (HSMCI_IDR) Response Time-out Error Interrupt Disable Mask */
#define HSMCI_IDR_RTOE(value)                 (HSMCI_IDR_RTOE_Msk & ((value) << HSMCI_IDR_RTOE_Pos))
#define HSMCI_IDR_DCRCE_Pos                   _U_(21)                                              /**< (HSMCI_IDR) Data CRC Error Interrupt Disable Position */
#define HSMCI_IDR_DCRCE_Msk                   (_U_(0x1) << HSMCI_IDR_DCRCE_Pos)                    /**< (HSMCI_IDR) Data CRC Error Interrupt Disable Mask */
#define HSMCI_IDR_DCRCE(value)                (HSMCI_IDR_DCRCE_Msk & ((value) << HSMCI_IDR_DCRCE_Pos))
#define HSMCI_IDR_DTOE_Pos                    _U_(22)                                              /**< (HSMCI_IDR) Data Time-out Error Interrupt Disable Position */
#define HSMCI_IDR_DTOE_Msk                    (_U_(0x1) << HSMCI_IDR_DTOE_Pos)                     /**< (HSMCI_IDR) Data Time-out Error Interrupt Disable Mask */
#define HSMCI_IDR_DTOE(value)                 (HSMCI_IDR_DTOE_Msk & ((value) << HSMCI_IDR_DTOE_Pos))
#define HSMCI_IDR_CSTOE_Pos                   _U_(23)                                              /**< (HSMCI_IDR) Completion Signal Time out Error Interrupt Disable Position */
#define HSMCI_IDR_CSTOE_Msk                   (_U_(0x1) << HSMCI_IDR_CSTOE_Pos)                    /**< (HSMCI_IDR) Completion Signal Time out Error Interrupt Disable Mask */
#define HSMCI_IDR_CSTOE(value)                (HSMCI_IDR_CSTOE_Msk & ((value) << HSMCI_IDR_CSTOE_Pos))
#define HSMCI_IDR_BLKOVRE_Pos                 _U_(24)                                              /**< (HSMCI_IDR) DMA Block Overrun Error Interrupt Disable Position */
#define HSMCI_IDR_BLKOVRE_Msk                 (_U_(0x1) << HSMCI_IDR_BLKOVRE_Pos)                  /**< (HSMCI_IDR) DMA Block Overrun Error Interrupt Disable Mask */
#define HSMCI_IDR_BLKOVRE(value)              (HSMCI_IDR_BLKOVRE_Msk & ((value) << HSMCI_IDR_BLKOVRE_Pos))
#define HSMCI_IDR_FIFOEMPTY_Pos               _U_(26)                                              /**< (HSMCI_IDR) FIFO empty Interrupt Disable Position */
#define HSMCI_IDR_FIFOEMPTY_Msk               (_U_(0x1) << HSMCI_IDR_FIFOEMPTY_Pos)                /**< (HSMCI_IDR) FIFO empty Interrupt Disable Mask */
#define HSMCI_IDR_FIFOEMPTY(value)            (HSMCI_IDR_FIFOEMPTY_Msk & ((value) << HSMCI_IDR_FIFOEMPTY_Pos))
#define HSMCI_IDR_XFRDONE_Pos                 _U_(27)                                              /**< (HSMCI_IDR) Transfer Done Interrupt Disable Position */
#define HSMCI_IDR_XFRDONE_Msk                 (_U_(0x1) << HSMCI_IDR_XFRDONE_Pos)                  /**< (HSMCI_IDR) Transfer Done Interrupt Disable Mask */
#define HSMCI_IDR_XFRDONE(value)              (HSMCI_IDR_XFRDONE_Msk & ((value) << HSMCI_IDR_XFRDONE_Pos))
#define HSMCI_IDR_ACKRCV_Pos                  _U_(28)                                              /**< (HSMCI_IDR) Boot Acknowledge Interrupt Disable Position */
#define HSMCI_IDR_ACKRCV_Msk                  (_U_(0x1) << HSMCI_IDR_ACKRCV_Pos)                   /**< (HSMCI_IDR) Boot Acknowledge Interrupt Disable Mask */
#define HSMCI_IDR_ACKRCV(value)               (HSMCI_IDR_ACKRCV_Msk & ((value) << HSMCI_IDR_ACKRCV_Pos))
#define HSMCI_IDR_ACKRCVE_Pos                 _U_(29)                                              /**< (HSMCI_IDR) Boot Acknowledge Error Interrupt Disable Position */
#define HSMCI_IDR_ACKRCVE_Msk                 (_U_(0x1) << HSMCI_IDR_ACKRCVE_Pos)                  /**< (HSMCI_IDR) Boot Acknowledge Error Interrupt Disable Mask */
#define HSMCI_IDR_ACKRCVE(value)              (HSMCI_IDR_ACKRCVE_Msk & ((value) << HSMCI_IDR_ACKRCVE_Pos))
#define HSMCI_IDR_OVRE_Pos                    _U_(30)                                              /**< (HSMCI_IDR) Overrun Interrupt Disable Position */
#define HSMCI_IDR_OVRE_Msk                    (_U_(0x1) << HSMCI_IDR_OVRE_Pos)                     /**< (HSMCI_IDR) Overrun Interrupt Disable Mask */
#define HSMCI_IDR_OVRE(value)                 (HSMCI_IDR_OVRE_Msk & ((value) << HSMCI_IDR_OVRE_Pos))
#define HSMCI_IDR_UNRE_Pos                    _U_(31)                                              /**< (HSMCI_IDR) Underrun Interrupt Disable Position */
#define HSMCI_IDR_UNRE_Msk                    (_U_(0x1) << HSMCI_IDR_UNRE_Pos)                     /**< (HSMCI_IDR) Underrun Interrupt Disable Mask */
#define HSMCI_IDR_UNRE(value)                 (HSMCI_IDR_UNRE_Msk & ((value) << HSMCI_IDR_UNRE_Pos))
#define HSMCI_IDR_Msk                         _U_(0xFDFF313F)                                      /**< (HSMCI_IDR) Register Mask  */


/* -------- HSMCI_IMR : (HSMCI Offset: 0x4C) ( R/ 32) Interrupt Mask Register -------- */
#define HSMCI_IMR_CMDRDY_Pos                  _U_(0)                                               /**< (HSMCI_IMR) Command Ready Interrupt Mask Position */
#define HSMCI_IMR_CMDRDY_Msk                  (_U_(0x1) << HSMCI_IMR_CMDRDY_Pos)                   /**< (HSMCI_IMR) Command Ready Interrupt Mask Mask */
#define HSMCI_IMR_CMDRDY(value)               (HSMCI_IMR_CMDRDY_Msk & ((value) << HSMCI_IMR_CMDRDY_Pos))
#define HSMCI_IMR_RXRDY_Pos                   _U_(1)                                               /**< (HSMCI_IMR) Receiver Ready Interrupt Mask Position */
#define HSMCI_IMR_RXRDY_Msk                   (_U_(0x1) << HSMCI_IMR_RXRDY_Pos)                    /**< (HSMCI_IMR) Receiver Ready Interrupt Mask Mask */
#define HSMCI_IMR_RXRDY(value)                (HSMCI_IMR_RXRDY_Msk & ((value) << HSMCI_IMR_RXRDY_Pos))
#define HSMCI_IMR_TXRDY_Pos                   _U_(2)                                               /**< (HSMCI_IMR) Transmit Ready Interrupt Mask Position */
#define HSMCI_IMR_TXRDY_Msk                   (_U_(0x1) << HSMCI_IMR_TXRDY_Pos)                    /**< (HSMCI_IMR) Transmit Ready Interrupt Mask Mask */
#define HSMCI_IMR_TXRDY(value)                (HSMCI_IMR_TXRDY_Msk & ((value) << HSMCI_IMR_TXRDY_Pos))
#define HSMCI_IMR_BLKE_Pos                    _U_(3)                                               /**< (HSMCI_IMR) Data Block Ended Interrupt Mask Position */
#define HSMCI_IMR_BLKE_Msk                    (_U_(0x1) << HSMCI_IMR_BLKE_Pos)                     /**< (HSMCI_IMR) Data Block Ended Interrupt Mask Mask */
#define HSMCI_IMR_BLKE(value)                 (HSMCI_IMR_BLKE_Msk & ((value) << HSMCI_IMR_BLKE_Pos))
#define HSMCI_IMR_DTIP_Pos                    _U_(4)                                               /**< (HSMCI_IMR) Data Transfer in Progress Interrupt Mask Position */
#define HSMCI_IMR_DTIP_Msk                    (_U_(0x1) << HSMCI_IMR_DTIP_Pos)                     /**< (HSMCI_IMR) Data Transfer in Progress Interrupt Mask Mask */
#define HSMCI_IMR_DTIP(value)                 (HSMCI_IMR_DTIP_Msk & ((value) << HSMCI_IMR_DTIP_Pos))
#define HSMCI_IMR_NOTBUSY_Pos                 _U_(5)                                               /**< (HSMCI_IMR) Data Not Busy Interrupt Mask Position */
#define HSMCI_IMR_NOTBUSY_Msk                 (_U_(0x1) << HSMCI_IMR_NOTBUSY_Pos)                  /**< (HSMCI_IMR) Data Not Busy Interrupt Mask Mask */
#define HSMCI_IMR_NOTBUSY(value)              (HSMCI_IMR_NOTBUSY_Msk & ((value) << HSMCI_IMR_NOTBUSY_Pos))
#define HSMCI_IMR_SDIOIRQA_Pos                _U_(8)                                               /**< (HSMCI_IMR) SDIO Interrupt for Slot A Interrupt Mask Position */
#define HSMCI_IMR_SDIOIRQA_Msk                (_U_(0x1) << HSMCI_IMR_SDIOIRQA_Pos)                 /**< (HSMCI_IMR) SDIO Interrupt for Slot A Interrupt Mask Mask */
#define HSMCI_IMR_SDIOIRQA(value)             (HSMCI_IMR_SDIOIRQA_Msk & ((value) << HSMCI_IMR_SDIOIRQA_Pos))
#define HSMCI_IMR_SDIOWAIT_Pos                _U_(12)                                              /**< (HSMCI_IMR) SDIO Read Wait Operation Status Interrupt Mask Position */
#define HSMCI_IMR_SDIOWAIT_Msk                (_U_(0x1) << HSMCI_IMR_SDIOWAIT_Pos)                 /**< (HSMCI_IMR) SDIO Read Wait Operation Status Interrupt Mask Mask */
#define HSMCI_IMR_SDIOWAIT(value)             (HSMCI_IMR_SDIOWAIT_Msk & ((value) << HSMCI_IMR_SDIOWAIT_Pos))
#define HSMCI_IMR_CSRCV_Pos                   _U_(13)                                              /**< (HSMCI_IMR) Completion Signal Received Interrupt Mask Position */
#define HSMCI_IMR_CSRCV_Msk                   (_U_(0x1) << HSMCI_IMR_CSRCV_Pos)                    /**< (HSMCI_IMR) Completion Signal Received Interrupt Mask Mask */
#define HSMCI_IMR_CSRCV(value)                (HSMCI_IMR_CSRCV_Msk & ((value) << HSMCI_IMR_CSRCV_Pos))
#define HSMCI_IMR_RINDE_Pos                   _U_(16)                                              /**< (HSMCI_IMR) Response Index Error Interrupt Mask Position */
#define HSMCI_IMR_RINDE_Msk                   (_U_(0x1) << HSMCI_IMR_RINDE_Pos)                    /**< (HSMCI_IMR) Response Index Error Interrupt Mask Mask */
#define HSMCI_IMR_RINDE(value)                (HSMCI_IMR_RINDE_Msk & ((value) << HSMCI_IMR_RINDE_Pos))
#define HSMCI_IMR_RDIRE_Pos                   _U_(17)                                              /**< (HSMCI_IMR) Response Direction Error Interrupt Mask Position */
#define HSMCI_IMR_RDIRE_Msk                   (_U_(0x1) << HSMCI_IMR_RDIRE_Pos)                    /**< (HSMCI_IMR) Response Direction Error Interrupt Mask Mask */
#define HSMCI_IMR_RDIRE(value)                (HSMCI_IMR_RDIRE_Msk & ((value) << HSMCI_IMR_RDIRE_Pos))
#define HSMCI_IMR_RCRCE_Pos                   _U_(18)                                              /**< (HSMCI_IMR) Response CRC Error Interrupt Mask Position */
#define HSMCI_IMR_RCRCE_Msk                   (_U_(0x1) << HSMCI_IMR_RCRCE_Pos)                    /**< (HSMCI_IMR) Response CRC Error Interrupt Mask Mask */
#define HSMCI_IMR_RCRCE(value)                (HSMCI_IMR_RCRCE_Msk & ((value) << HSMCI_IMR_RCRCE_Pos))
#define HSMCI_IMR_RENDE_Pos                   _U_(19)                                              /**< (HSMCI_IMR) Response End Bit Error Interrupt Mask Position */
#define HSMCI_IMR_RENDE_Msk                   (_U_(0x1) << HSMCI_IMR_RENDE_Pos)                    /**< (HSMCI_IMR) Response End Bit Error Interrupt Mask Mask */
#define HSMCI_IMR_RENDE(value)                (HSMCI_IMR_RENDE_Msk & ((value) << HSMCI_IMR_RENDE_Pos))
#define HSMCI_IMR_RTOE_Pos                    _U_(20)                                              /**< (HSMCI_IMR) Response Time-out Error Interrupt Mask Position */
#define HSMCI_IMR_RTOE_Msk                    (_U_(0x1) << HSMCI_IMR_RTOE_Pos)                     /**< (HSMCI_IMR) Response Time-out Error Interrupt Mask Mask */
#define HSMCI_IMR_RTOE(value)                 (HSMCI_IMR_RTOE_Msk & ((value) << HSMCI_IMR_RTOE_Pos))
#define HSMCI_IMR_DCRCE_Pos                   _U_(21)                                              /**< (HSMCI_IMR) Data CRC Error Interrupt Mask Position */
#define HSMCI_IMR_DCRCE_Msk                   (_U_(0x1) << HSMCI_IMR_DCRCE_Pos)                    /**< (HSMCI_IMR) Data CRC Error Interrupt Mask Mask */
#define HSMCI_IMR_DCRCE(value)                (HSMCI_IMR_DCRCE_Msk & ((value) << HSMCI_IMR_DCRCE_Pos))
#define HSMCI_IMR_DTOE_Pos                    _U_(22)                                              /**< (HSMCI_IMR) Data Time-out Error Interrupt Mask Position */
#define HSMCI_IMR_DTOE_Msk                    (_U_(0x1) << HSMCI_IMR_DTOE_Pos)                     /**< (HSMCI_IMR) Data Time-out Error Interrupt Mask Mask */
#define HSMCI_IMR_DTOE(value)                 (HSMCI_IMR_DTOE_Msk & ((value) << HSMCI_IMR_DTOE_Pos))
#define HSMCI_IMR_CSTOE_Pos                   _U_(23)                                              /**< (HSMCI_IMR) Completion Signal Time-out Error Interrupt Mask Position */
#define HSMCI_IMR_CSTOE_Msk                   (_U_(0x1) << HSMCI_IMR_CSTOE_Pos)                    /**< (HSMCI_IMR) Completion Signal Time-out Error Interrupt Mask Mask */
#define HSMCI_IMR_CSTOE(value)                (HSMCI_IMR_CSTOE_Msk & ((value) << HSMCI_IMR_CSTOE_Pos))
#define HSMCI_IMR_BLKOVRE_Pos                 _U_(24)                                              /**< (HSMCI_IMR) DMA Block Overrun Error Interrupt Mask Position */
#define HSMCI_IMR_BLKOVRE_Msk                 (_U_(0x1) << HSMCI_IMR_BLKOVRE_Pos)                  /**< (HSMCI_IMR) DMA Block Overrun Error Interrupt Mask Mask */
#define HSMCI_IMR_BLKOVRE(value)              (HSMCI_IMR_BLKOVRE_Msk & ((value) << HSMCI_IMR_BLKOVRE_Pos))
#define HSMCI_IMR_FIFOEMPTY_Pos               _U_(26)                                              /**< (HSMCI_IMR) FIFO Empty Interrupt Mask Position */
#define HSMCI_IMR_FIFOEMPTY_Msk               (_U_(0x1) << HSMCI_IMR_FIFOEMPTY_Pos)                /**< (HSMCI_IMR) FIFO Empty Interrupt Mask Mask */
#define HSMCI_IMR_FIFOEMPTY(value)            (HSMCI_IMR_FIFOEMPTY_Msk & ((value) << HSMCI_IMR_FIFOEMPTY_Pos))
#define HSMCI_IMR_XFRDONE_Pos                 _U_(27)                                              /**< (HSMCI_IMR) Transfer Done Interrupt Mask Position */
#define HSMCI_IMR_XFRDONE_Msk                 (_U_(0x1) << HSMCI_IMR_XFRDONE_Pos)                  /**< (HSMCI_IMR) Transfer Done Interrupt Mask Mask */
#define HSMCI_IMR_XFRDONE(value)              (HSMCI_IMR_XFRDONE_Msk & ((value) << HSMCI_IMR_XFRDONE_Pos))
#define HSMCI_IMR_ACKRCV_Pos                  _U_(28)                                              /**< (HSMCI_IMR) Boot Operation Acknowledge Received Interrupt Mask Position */
#define HSMCI_IMR_ACKRCV_Msk                  (_U_(0x1) << HSMCI_IMR_ACKRCV_Pos)                   /**< (HSMCI_IMR) Boot Operation Acknowledge Received Interrupt Mask Mask */
#define HSMCI_IMR_ACKRCV(value)               (HSMCI_IMR_ACKRCV_Msk & ((value) << HSMCI_IMR_ACKRCV_Pos))
#define HSMCI_IMR_ACKRCVE_Pos                 _U_(29)                                              /**< (HSMCI_IMR) Boot Operation Acknowledge Error Interrupt Mask Position */
#define HSMCI_IMR_ACKRCVE_Msk                 (_U_(0x1) << HSMCI_IMR_ACKRCVE_Pos)                  /**< (HSMCI_IMR) Boot Operation Acknowledge Error Interrupt Mask Mask */
#define HSMCI_IMR_ACKRCVE(value)              (HSMCI_IMR_ACKRCVE_Msk & ((value) << HSMCI_IMR_ACKRCVE_Pos))
#define HSMCI_IMR_OVRE_Pos                    _U_(30)                                              /**< (HSMCI_IMR) Overrun Interrupt Mask Position */
#define HSMCI_IMR_OVRE_Msk                    (_U_(0x1) << HSMCI_IMR_OVRE_Pos)                     /**< (HSMCI_IMR) Overrun Interrupt Mask Mask */
#define HSMCI_IMR_OVRE(value)                 (HSMCI_IMR_OVRE_Msk & ((value) << HSMCI_IMR_OVRE_Pos))
#define HSMCI_IMR_UNRE_Pos                    _U_(31)                                              /**< (HSMCI_IMR) Underrun Interrupt Mask Position */
#define HSMCI_IMR_UNRE_Msk                    (_U_(0x1) << HSMCI_IMR_UNRE_Pos)                     /**< (HSMCI_IMR) Underrun Interrupt Mask Mask */
#define HSMCI_IMR_UNRE(value)                 (HSMCI_IMR_UNRE_Msk & ((value) << HSMCI_IMR_UNRE_Pos))
#define HSMCI_IMR_Msk                         _U_(0xFDFF313F)                                      /**< (HSMCI_IMR) Register Mask  */


/* -------- HSMCI_DMA : (HSMCI Offset: 0x50) (R/W 32) DMA Configuration Register -------- */
#define HSMCI_DMA_CHKSIZE_Pos                 _U_(4)                                               /**< (HSMCI_DMA) DMA Channel Read and Write Chunk Size Position */
#define HSMCI_DMA_CHKSIZE_Msk                 (_U_(0x7) << HSMCI_DMA_CHKSIZE_Pos)                  /**< (HSMCI_DMA) DMA Channel Read and Write Chunk Size Mask */
#define HSMCI_DMA_CHKSIZE(value)              (HSMCI_DMA_CHKSIZE_Msk & ((value) << HSMCI_DMA_CHKSIZE_Pos))
#define   HSMCI_DMA_CHKSIZE_1_Val             _U_(0x0)                                             /**< (HSMCI_DMA) 1 data available  */
#define   HSMCI_DMA_CHKSIZE_2_Val             _U_(0x1)                                             /**< (HSMCI_DMA) 2 data available  */
#define   HSMCI_DMA_CHKSIZE_4_Val             _U_(0x2)                                             /**< (HSMCI_DMA) 4 data available  */
#define   HSMCI_DMA_CHKSIZE_8_Val             _U_(0x3)                                             /**< (HSMCI_DMA) 8 data available  */
#define   HSMCI_DMA_CHKSIZE_16_Val            _U_(0x4)                                             /**< (HSMCI_DMA) 16 data available  */
#define HSMCI_DMA_CHKSIZE_1                   (HSMCI_DMA_CHKSIZE_1_Val << HSMCI_DMA_CHKSIZE_Pos)   /**< (HSMCI_DMA) 1 data available Position  */
#define HSMCI_DMA_CHKSIZE_2                   (HSMCI_DMA_CHKSIZE_2_Val << HSMCI_DMA_CHKSIZE_Pos)   /**< (HSMCI_DMA) 2 data available Position  */
#define HSMCI_DMA_CHKSIZE_4                   (HSMCI_DMA_CHKSIZE_4_Val << HSMCI_DMA_CHKSIZE_Pos)   /**< (HSMCI_DMA) 4 data available Position  */
#define HSMCI_DMA_CHKSIZE_8                   (HSMCI_DMA_CHKSIZE_8_Val << HSMCI_DMA_CHKSIZE_Pos)   /**< (HSMCI_DMA) 8 data available Position  */
#define HSMCI_DMA_CHKSIZE_16                  (HSMCI_DMA_CHKSIZE_16_Val << HSMCI_DMA_CHKSIZE_Pos)  /**< (HSMCI_DMA) 16 data available Position  */
#define HSMCI_DMA_DMAEN_Pos                   _U_(8)                                               /**< (HSMCI_DMA) DMA Hardware Handshaking Enable Position */
#define HSMCI_DMA_DMAEN_Msk                   (_U_(0x1) << HSMCI_DMA_DMAEN_Pos)                    /**< (HSMCI_DMA) DMA Hardware Handshaking Enable Mask */
#define HSMCI_DMA_DMAEN(value)                (HSMCI_DMA_DMAEN_Msk & ((value) << HSMCI_DMA_DMAEN_Pos))
#define HSMCI_DMA_Msk                         _U_(0x00000170)                                      /**< (HSMCI_DMA) Register Mask  */


/* -------- HSMCI_CFG : (HSMCI Offset: 0x54) (R/W 32) Configuration Register -------- */
#define HSMCI_CFG_FIFOMODE_Pos                _U_(0)                                               /**< (HSMCI_CFG) HSMCI Internal FIFO control mode Position */
#define HSMCI_CFG_FIFOMODE_Msk                (_U_(0x1) << HSMCI_CFG_FIFOMODE_Pos)                 /**< (HSMCI_CFG) HSMCI Internal FIFO control mode Mask */
#define HSMCI_CFG_FIFOMODE(value)             (HSMCI_CFG_FIFOMODE_Msk & ((value) << HSMCI_CFG_FIFOMODE_Pos))
#define HSMCI_CFG_FERRCTRL_Pos                _U_(4)                                               /**< (HSMCI_CFG) Flow Error flag reset control mode Position */
#define HSMCI_CFG_FERRCTRL_Msk                (_U_(0x1) << HSMCI_CFG_FERRCTRL_Pos)                 /**< (HSMCI_CFG) Flow Error flag reset control mode Mask */
#define HSMCI_CFG_FERRCTRL(value)             (HSMCI_CFG_FERRCTRL_Msk & ((value) << HSMCI_CFG_FERRCTRL_Pos))
#define HSMCI_CFG_HSMODE_Pos                  _U_(8)                                               /**< (HSMCI_CFG) High Speed Mode Position */
#define HSMCI_CFG_HSMODE_Msk                  (_U_(0x1) << HSMCI_CFG_HSMODE_Pos)                   /**< (HSMCI_CFG) High Speed Mode Mask */
#define HSMCI_CFG_HSMODE(value)               (HSMCI_CFG_HSMODE_Msk & ((value) << HSMCI_CFG_HSMODE_Pos))
#define HSMCI_CFG_LSYNC_Pos                   _U_(12)                                              /**< (HSMCI_CFG) Synchronize on the last block Position */
#define HSMCI_CFG_LSYNC_Msk                   (_U_(0x1) << HSMCI_CFG_LSYNC_Pos)                    /**< (HSMCI_CFG) Synchronize on the last block Mask */
#define HSMCI_CFG_LSYNC(value)                (HSMCI_CFG_LSYNC_Msk & ((value) << HSMCI_CFG_LSYNC_Pos))
#define HSMCI_CFG_Msk                         _U_(0x00001111)                                      /**< (HSMCI_CFG) Register Mask  */


/* -------- HSMCI_WPMR : (HSMCI Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define HSMCI_WPMR_WPEN_Pos                   _U_(0)                                               /**< (HSMCI_WPMR) Write Protect Enable Position */
#define HSMCI_WPMR_WPEN_Msk                   (_U_(0x1) << HSMCI_WPMR_WPEN_Pos)                    /**< (HSMCI_WPMR) Write Protect Enable Mask */
#define HSMCI_WPMR_WPEN(value)                (HSMCI_WPMR_WPEN_Msk & ((value) << HSMCI_WPMR_WPEN_Pos))
#define HSMCI_WPMR_WPKEY_Pos                  _U_(8)                                               /**< (HSMCI_WPMR) Write Protect Key Position */
#define HSMCI_WPMR_WPKEY_Msk                  (_U_(0xFFFFFF) << HSMCI_WPMR_WPKEY_Pos)              /**< (HSMCI_WPMR) Write Protect Key Mask */
#define HSMCI_WPMR_WPKEY(value)               (HSMCI_WPMR_WPKEY_Msk & ((value) << HSMCI_WPMR_WPKEY_Pos))
#define   HSMCI_WPMR_WPKEY_PASSWD_Val         _U_(0x4D4349)                                        /**< (HSMCI_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0.  */
#define HSMCI_WPMR_WPKEY_PASSWD               (HSMCI_WPMR_WPKEY_PASSWD_Val << HSMCI_WPMR_WPKEY_Pos) /**< (HSMCI_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0. Position  */
#define HSMCI_WPMR_Msk                        _U_(0xFFFFFF01)                                      /**< (HSMCI_WPMR) Register Mask  */


/* -------- HSMCI_WPSR : (HSMCI Offset: 0xE8) ( R/ 32) Write Protection Status Register -------- */
#define HSMCI_WPSR_WPVS_Pos                   _U_(0)                                               /**< (HSMCI_WPSR) Write Protection Violation Status Position */
#define HSMCI_WPSR_WPVS_Msk                   (_U_(0x1) << HSMCI_WPSR_WPVS_Pos)                    /**< (HSMCI_WPSR) Write Protection Violation Status Mask */
#define HSMCI_WPSR_WPVS(value)                (HSMCI_WPSR_WPVS_Msk & ((value) << HSMCI_WPSR_WPVS_Pos))
#define HSMCI_WPSR_WPVSRC_Pos                 _U_(8)                                               /**< (HSMCI_WPSR) Write Protection Violation Source Position */
#define HSMCI_WPSR_WPVSRC_Msk                 (_U_(0xFFFF) << HSMCI_WPSR_WPVSRC_Pos)               /**< (HSMCI_WPSR) Write Protection Violation Source Mask */
#define HSMCI_WPSR_WPVSRC(value)              (HSMCI_WPSR_WPVSRC_Msk & ((value) << HSMCI_WPSR_WPVSRC_Pos))
#define HSMCI_WPSR_Msk                        _U_(0x00FFFF01)                                      /**< (HSMCI_WPSR) Register Mask  */


/* -------- HSMCI_FIFO : (HSMCI Offset: 0x200) (R/W 32) FIFO Memory Aperture0 0 -------- */
#define HSMCI_FIFO_DATA_Pos                   _U_(0)                                               /**< (HSMCI_FIFO) Data to Read or Data to Write Position */
#define HSMCI_FIFO_DATA_Msk                   (_U_(0xFFFFFFFF) << HSMCI_FIFO_DATA_Pos)             /**< (HSMCI_FIFO) Data to Read or Data to Write Mask */
#define HSMCI_FIFO_DATA(value)                (HSMCI_FIFO_DATA_Msk & ((value) << HSMCI_FIFO_DATA_Pos))
#define HSMCI_FIFO_Msk                        _U_(0xFFFFFFFF)                                      /**< (HSMCI_FIFO) Register Mask  */


/** \brief HSMCI register offsets definitions */
#define HSMCI_CR_REG_OFST              (0x00)              /**< (HSMCI_CR) Control Register Offset */
#define HSMCI_MR_REG_OFST              (0x04)              /**< (HSMCI_MR) Mode Register Offset */
#define HSMCI_DTOR_REG_OFST            (0x08)              /**< (HSMCI_DTOR) Data Timeout Register Offset */
#define HSMCI_SDCR_REG_OFST            (0x0C)              /**< (HSMCI_SDCR) SD/SDIO Card Register Offset */
#define HSMCI_ARGR_REG_OFST            (0x10)              /**< (HSMCI_ARGR) Argument Register Offset */
#define HSMCI_CMDR_REG_OFST            (0x14)              /**< (HSMCI_CMDR) Command Register Offset */
#define HSMCI_BLKR_REG_OFST            (0x18)              /**< (HSMCI_BLKR) Block Register Offset */
#define HSMCI_CSTOR_REG_OFST           (0x1C)              /**< (HSMCI_CSTOR) Completion Signal Timeout Register Offset */
#define HSMCI_RSPR_REG_OFST            (0x20)              /**< (HSMCI_RSPR) Response Register 0 Offset */
#define HSMCI_RDR_REG_OFST             (0x30)              /**< (HSMCI_RDR) Receive Data Register Offset */
#define HSMCI_TDR_REG_OFST             (0x34)              /**< (HSMCI_TDR) Transmit Data Register Offset */
#define HSMCI_SR_REG_OFST              (0x40)              /**< (HSMCI_SR) Status Register Offset */
#define HSMCI_IER_REG_OFST             (0x44)              /**< (HSMCI_IER) Interrupt Enable Register Offset */
#define HSMCI_IDR_REG_OFST             (0x48)              /**< (HSMCI_IDR) Interrupt Disable Register Offset */
#define HSMCI_IMR_REG_OFST             (0x4C)              /**< (HSMCI_IMR) Interrupt Mask Register Offset */
#define HSMCI_DMA_REG_OFST             (0x50)              /**< (HSMCI_DMA) DMA Configuration Register Offset */
#define HSMCI_CFG_REG_OFST             (0x54)              /**< (HSMCI_CFG) Configuration Register Offset */
#define HSMCI_WPMR_REG_OFST            (0xE4)              /**< (HSMCI_WPMR) Write Protection Mode Register Offset */
#define HSMCI_WPSR_REG_OFST            (0xE8)              /**< (HSMCI_WPSR) Write Protection Status Register Offset */
#define HSMCI_FIFO_REG_OFST            (0x200)             /**< (HSMCI_FIFO) FIFO Memory Aperture0 0 Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief HSMCI register API structure */
typedef struct
{
  __O   uint32_t                       HSMCI_CR;           /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       HSMCI_MR;           /**< Offset: 0x04 (R/W  32) Mode Register */
  __IO  uint32_t                       HSMCI_DTOR;         /**< Offset: 0x08 (R/W  32) Data Timeout Register */
  __IO  uint32_t                       HSMCI_SDCR;         /**< Offset: 0x0C (R/W  32) SD/SDIO Card Register */
  __IO  uint32_t                       HSMCI_ARGR;         /**< Offset: 0x10 (R/W  32) Argument Register */
  __O   uint32_t                       HSMCI_CMDR;         /**< Offset: 0x14 ( /W  32) Command Register */
  __IO  uint32_t                       HSMCI_BLKR;         /**< Offset: 0x18 (R/W  32) Block Register */
  __IO  uint32_t                       HSMCI_CSTOR;        /**< Offset: 0x1C (R/W  32) Completion Signal Timeout Register */
  __I   uint32_t                       HSMCI_RSPR[4];      /**< Offset: 0x20 (R/   32) Response Register 0 */
  __I   uint32_t                       HSMCI_RDR;          /**< Offset: 0x30 (R/   32) Receive Data Register */
  __O   uint32_t                       HSMCI_TDR;          /**< Offset: 0x34 ( /W  32) Transmit Data Register */
  __I   uint8_t                        Reserved1[0x08];
  __I   uint32_t                       HSMCI_SR;           /**< Offset: 0x40 (R/   32) Status Register */
  __O   uint32_t                       HSMCI_IER;          /**< Offset: 0x44 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       HSMCI_IDR;          /**< Offset: 0x48 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       HSMCI_IMR;          /**< Offset: 0x4C (R/   32) Interrupt Mask Register */
  __IO  uint32_t                       HSMCI_DMA;          /**< Offset: 0x50 (R/W  32) DMA Configuration Register */
  __IO  uint32_t                       HSMCI_CFG;          /**< Offset: 0x54 (R/W  32) Configuration Register */
  __I   uint8_t                        Reserved2[0x8C];
  __IO  uint32_t                       HSMCI_WPMR;         /**< Offset: 0xE4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       HSMCI_WPSR;         /**< Offset: 0xE8 (R/   32) Write Protection Status Register */
  __I   uint8_t                        Reserved3[0x114];
  __IO  uint32_t                       HSMCI_FIFO[256];    /**< Offset: 0x200 (R/W  32) FIFO Memory Aperture0 0 */
} hsmci_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_HSMCI_COMPONENT_H_ */
