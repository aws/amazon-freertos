/**
 * \brief Component description for SDHC
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
#ifndef _SAME54_SDHC_COMPONENT_H_
#define _SAME54_SDHC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR SDHC                                         */
/* ************************************************************************** */

/* -------- SDHC_SSAR : (SDHC Offset: 0x00) (R/W 32) SDMA System Address / Argument 2 -------- */
#define SDHC_SSAR_RESETVALUE                  _U_(0x00)                                            /**<  (SDHC_SSAR) SDMA System Address / Argument 2  Reset Value */

#define SDHC_SSAR_ADDR_Pos                    _U_(0)                                               /**< (SDHC_SSAR) SDMA System Address Position */
#define SDHC_SSAR_ADDR_Msk                    (_U_(0xFFFFFFFF) << SDHC_SSAR_ADDR_Pos)              /**< (SDHC_SSAR) SDMA System Address Mask */
#define SDHC_SSAR_ADDR(value)                 (SDHC_SSAR_ADDR_Msk & ((value) << SDHC_SSAR_ADDR_Pos))
#define SDHC_SSAR_Msk                         _U_(0xFFFFFFFF)                                      /**< (SDHC_SSAR) Register Mask  */

/* CMD23 mode */
#define SDHC_SSAR_CMD23_ARG2_Pos              _U_(0)                                               /**< (SDHC_SSAR) Argument 2 Position */
#define SDHC_SSAR_CMD23_ARG2_Msk              (_U_(0xFFFFFFFF) << SDHC_SSAR_CMD23_ARG2_Pos)        /**< (SDHC_SSAR) Argument 2 Mask */
#define SDHC_SSAR_CMD23_ARG2(value)           (SDHC_SSAR_CMD23_ARG2_Msk & ((value) << SDHC_SSAR_CMD23_ARG2_Pos))
#define SDHC_SSAR_CMD23_Msk                   _U_(0xFFFFFFFF)                                       /**< (SDHC_SSAR_CMD23) Register Mask  */


/* -------- SDHC_BSR : (SDHC Offset: 0x04) (R/W 16) Block Size -------- */
#define SDHC_BSR_RESETVALUE                   _U_(0x00)                                            /**<  (SDHC_BSR) Block Size  Reset Value */

#define SDHC_BSR_BLOCKSIZE_Pos                _U_(0)                                               /**< (SDHC_BSR) Transfer Block Size Position */
#define SDHC_BSR_BLOCKSIZE_Msk                (_U_(0x3FF) << SDHC_BSR_BLOCKSIZE_Pos)               /**< (SDHC_BSR) Transfer Block Size Mask */
#define SDHC_BSR_BLOCKSIZE(value)             (SDHC_BSR_BLOCKSIZE_Msk & ((value) << SDHC_BSR_BLOCKSIZE_Pos))
#define SDHC_BSR_BOUNDARY_Pos                 _U_(12)                                              /**< (SDHC_BSR) SDMA Buffer Boundary Position */
#define SDHC_BSR_BOUNDARY_Msk                 (_U_(0x7) << SDHC_BSR_BOUNDARY_Pos)                  /**< (SDHC_BSR) SDMA Buffer Boundary Mask */
#define SDHC_BSR_BOUNDARY(value)              (SDHC_BSR_BOUNDARY_Msk & ((value) << SDHC_BSR_BOUNDARY_Pos))
#define   SDHC_BSR_BOUNDARY_4K_Val            _U_(0x0)                                             /**< (SDHC_BSR) 4k bytes  */
#define   SDHC_BSR_BOUNDARY_8K_Val            _U_(0x1)                                             /**< (SDHC_BSR) 8k bytes  */
#define   SDHC_BSR_BOUNDARY_16K_Val           _U_(0x2)                                             /**< (SDHC_BSR) 16k bytes  */
#define   SDHC_BSR_BOUNDARY_32K_Val           _U_(0x3)                                             /**< (SDHC_BSR) 32k bytes  */
#define   SDHC_BSR_BOUNDARY_64K_Val           _U_(0x4)                                             /**< (SDHC_BSR) 64k bytes  */
#define   SDHC_BSR_BOUNDARY_128K_Val          _U_(0x5)                                             /**< (SDHC_BSR) 128k bytes  */
#define   SDHC_BSR_BOUNDARY_256K_Val          _U_(0x6)                                             /**< (SDHC_BSR) 256k bytes  */
#define   SDHC_BSR_BOUNDARY_512K_Val          _U_(0x7)                                             /**< (SDHC_BSR) 512k bytes  */
#define SDHC_BSR_BOUNDARY_4K                  (SDHC_BSR_BOUNDARY_4K_Val << SDHC_BSR_BOUNDARY_Pos)  /**< (SDHC_BSR) 4k bytes Position  */
#define SDHC_BSR_BOUNDARY_8K                  (SDHC_BSR_BOUNDARY_8K_Val << SDHC_BSR_BOUNDARY_Pos)  /**< (SDHC_BSR) 8k bytes Position  */
#define SDHC_BSR_BOUNDARY_16K                 (SDHC_BSR_BOUNDARY_16K_Val << SDHC_BSR_BOUNDARY_Pos) /**< (SDHC_BSR) 16k bytes Position  */
#define SDHC_BSR_BOUNDARY_32K                 (SDHC_BSR_BOUNDARY_32K_Val << SDHC_BSR_BOUNDARY_Pos) /**< (SDHC_BSR) 32k bytes Position  */
#define SDHC_BSR_BOUNDARY_64K                 (SDHC_BSR_BOUNDARY_64K_Val << SDHC_BSR_BOUNDARY_Pos) /**< (SDHC_BSR) 64k bytes Position  */
#define SDHC_BSR_BOUNDARY_128K                (SDHC_BSR_BOUNDARY_128K_Val << SDHC_BSR_BOUNDARY_Pos) /**< (SDHC_BSR) 128k bytes Position  */
#define SDHC_BSR_BOUNDARY_256K                (SDHC_BSR_BOUNDARY_256K_Val << SDHC_BSR_BOUNDARY_Pos) /**< (SDHC_BSR) 256k bytes Position  */
#define SDHC_BSR_BOUNDARY_512K                (SDHC_BSR_BOUNDARY_512K_Val << SDHC_BSR_BOUNDARY_Pos) /**< (SDHC_BSR) 512k bytes Position  */
#define SDHC_BSR_Msk                          _U_(0x73FF)                                          /**< (SDHC_BSR) Register Mask  */


/* -------- SDHC_BCR : (SDHC Offset: 0x06) (R/W 16) Block Count -------- */
#define SDHC_BCR_RESETVALUE                   _U_(0x00)                                            /**<  (SDHC_BCR) Block Count  Reset Value */

#define SDHC_BCR_BCNT_Pos                     _U_(0)                                               /**< (SDHC_BCR) Blocks Count for Current Transfer Position */
#define SDHC_BCR_BCNT_Msk                     (_U_(0xFFFF) << SDHC_BCR_BCNT_Pos)                   /**< (SDHC_BCR) Blocks Count for Current Transfer Mask */
#define SDHC_BCR_BCNT(value)                  (SDHC_BCR_BCNT_Msk & ((value) << SDHC_BCR_BCNT_Pos))
#define SDHC_BCR_Msk                          _U_(0xFFFF)                                          /**< (SDHC_BCR) Register Mask  */


/* -------- SDHC_ARG1R : (SDHC Offset: 0x08) (R/W 32) Argument 1 -------- */
#define SDHC_ARG1R_RESETVALUE                 _U_(0x00)                                            /**<  (SDHC_ARG1R) Argument 1  Reset Value */

#define SDHC_ARG1R_ARG_Pos                    _U_(0)                                               /**< (SDHC_ARG1R) Argument 1 Position */
#define SDHC_ARG1R_ARG_Msk                    (_U_(0xFFFFFFFF) << SDHC_ARG1R_ARG_Pos)              /**< (SDHC_ARG1R) Argument 1 Mask */
#define SDHC_ARG1R_ARG(value)                 (SDHC_ARG1R_ARG_Msk & ((value) << SDHC_ARG1R_ARG_Pos))
#define SDHC_ARG1R_Msk                        _U_(0xFFFFFFFF)                                      /**< (SDHC_ARG1R) Register Mask  */


/* -------- SDHC_TMR : (SDHC Offset: 0x0C) (R/W 16) Transfer Mode -------- */
#define SDHC_TMR_RESETVALUE                   _U_(0x00)                                            /**<  (SDHC_TMR) Transfer Mode  Reset Value */

#define SDHC_TMR_DMAEN_Pos                    _U_(0)                                               /**< (SDHC_TMR) DMA Enable Position */
#define SDHC_TMR_DMAEN_Msk                    (_U_(0x1) << SDHC_TMR_DMAEN_Pos)                     /**< (SDHC_TMR) DMA Enable Mask */
#define SDHC_TMR_DMAEN(value)                 (SDHC_TMR_DMAEN_Msk & ((value) << SDHC_TMR_DMAEN_Pos))
#define   SDHC_TMR_DMAEN_DISABLE_Val          _U_(0x0)                                             /**< (SDHC_TMR) No data transfer or Non DMA data transfer  */
#define   SDHC_TMR_DMAEN_ENABLE_Val           _U_(0x1)                                             /**< (SDHC_TMR) DMA data transfer  */
#define SDHC_TMR_DMAEN_DISABLE                (SDHC_TMR_DMAEN_DISABLE_Val << SDHC_TMR_DMAEN_Pos)   /**< (SDHC_TMR) No data transfer or Non DMA data transfer Position  */
#define SDHC_TMR_DMAEN_ENABLE                 (SDHC_TMR_DMAEN_ENABLE_Val << SDHC_TMR_DMAEN_Pos)    /**< (SDHC_TMR) DMA data transfer Position  */
#define SDHC_TMR_BCEN_Pos                     _U_(1)                                               /**< (SDHC_TMR) Block Count Enable Position */
#define SDHC_TMR_BCEN_Msk                     (_U_(0x1) << SDHC_TMR_BCEN_Pos)                      /**< (SDHC_TMR) Block Count Enable Mask */
#define SDHC_TMR_BCEN(value)                  (SDHC_TMR_BCEN_Msk & ((value) << SDHC_TMR_BCEN_Pos))
#define   SDHC_TMR_BCEN_DISABLE_Val           _U_(0x0)                                             /**< (SDHC_TMR) Disable  */
#define   SDHC_TMR_BCEN_ENABLE_Val            _U_(0x1)                                             /**< (SDHC_TMR) Enable  */
#define SDHC_TMR_BCEN_DISABLE                 (SDHC_TMR_BCEN_DISABLE_Val << SDHC_TMR_BCEN_Pos)     /**< (SDHC_TMR) Disable Position  */
#define SDHC_TMR_BCEN_ENABLE                  (SDHC_TMR_BCEN_ENABLE_Val << SDHC_TMR_BCEN_Pos)      /**< (SDHC_TMR) Enable Position  */
#define SDHC_TMR_ACMDEN_Pos                   _U_(2)                                               /**< (SDHC_TMR) Auto Command Enable Position */
#define SDHC_TMR_ACMDEN_Msk                   (_U_(0x3) << SDHC_TMR_ACMDEN_Pos)                    /**< (SDHC_TMR) Auto Command Enable Mask */
#define SDHC_TMR_ACMDEN(value)                (SDHC_TMR_ACMDEN_Msk & ((value) << SDHC_TMR_ACMDEN_Pos))
#define   SDHC_TMR_ACMDEN_DISABLED_Val        _U_(0x0)                                             /**< (SDHC_TMR) Auto Command Disabled  */
#define   SDHC_TMR_ACMDEN_CMD12_Val           _U_(0x1)                                             /**< (SDHC_TMR) Auto CMD12 Enable  */
#define   SDHC_TMR_ACMDEN_CMD23_Val           _U_(0x2)                                             /**< (SDHC_TMR) Auto CMD23 Enable  */
#define SDHC_TMR_ACMDEN_DISABLED              (SDHC_TMR_ACMDEN_DISABLED_Val << SDHC_TMR_ACMDEN_Pos) /**< (SDHC_TMR) Auto Command Disabled Position  */
#define SDHC_TMR_ACMDEN_CMD12                 (SDHC_TMR_ACMDEN_CMD12_Val << SDHC_TMR_ACMDEN_Pos)   /**< (SDHC_TMR) Auto CMD12 Enable Position  */
#define SDHC_TMR_ACMDEN_CMD23                 (SDHC_TMR_ACMDEN_CMD23_Val << SDHC_TMR_ACMDEN_Pos)   /**< (SDHC_TMR) Auto CMD23 Enable Position  */
#define SDHC_TMR_DTDSEL_Pos                   _U_(4)                                               /**< (SDHC_TMR) Data Transfer Direction Selection Position */
#define SDHC_TMR_DTDSEL_Msk                   (_U_(0x1) << SDHC_TMR_DTDSEL_Pos)                    /**< (SDHC_TMR) Data Transfer Direction Selection Mask */
#define SDHC_TMR_DTDSEL(value)                (SDHC_TMR_DTDSEL_Msk & ((value) << SDHC_TMR_DTDSEL_Pos))
#define   SDHC_TMR_DTDSEL_WRITE_Val           _U_(0x0)                                             /**< (SDHC_TMR) Write (Host to Card)  */
#define   SDHC_TMR_DTDSEL_READ_Val            _U_(0x1)                                             /**< (SDHC_TMR) Read (Card to Host)  */
#define SDHC_TMR_DTDSEL_WRITE                 (SDHC_TMR_DTDSEL_WRITE_Val << SDHC_TMR_DTDSEL_Pos)   /**< (SDHC_TMR) Write (Host to Card) Position  */
#define SDHC_TMR_DTDSEL_READ                  (SDHC_TMR_DTDSEL_READ_Val << SDHC_TMR_DTDSEL_Pos)    /**< (SDHC_TMR) Read (Card to Host) Position  */
#define SDHC_TMR_MSBSEL_Pos                   _U_(5)                                               /**< (SDHC_TMR) Multi/Single Block Selection Position */
#define SDHC_TMR_MSBSEL_Msk                   (_U_(0x1) << SDHC_TMR_MSBSEL_Pos)                    /**< (SDHC_TMR) Multi/Single Block Selection Mask */
#define SDHC_TMR_MSBSEL(value)                (SDHC_TMR_MSBSEL_Msk & ((value) << SDHC_TMR_MSBSEL_Pos))
#define   SDHC_TMR_MSBSEL_SINGLE_Val          _U_(0x0)                                             /**< (SDHC_TMR) Single Block  */
#define   SDHC_TMR_MSBSEL_MULTIPLE_Val        _U_(0x1)                                             /**< (SDHC_TMR) Multiple Block  */
#define SDHC_TMR_MSBSEL_SINGLE                (SDHC_TMR_MSBSEL_SINGLE_Val << SDHC_TMR_MSBSEL_Pos)  /**< (SDHC_TMR) Single Block Position  */
#define SDHC_TMR_MSBSEL_MULTIPLE              (SDHC_TMR_MSBSEL_MULTIPLE_Val << SDHC_TMR_MSBSEL_Pos) /**< (SDHC_TMR) Multiple Block Position  */
#define SDHC_TMR_Msk                          _U_(0x003F)                                          /**< (SDHC_TMR) Register Mask  */


/* -------- SDHC_CR : (SDHC Offset: 0x0E) (R/W 16) Command -------- */
#define SDHC_CR_RESETVALUE                    _U_(0x00)                                            /**<  (SDHC_CR) Command  Reset Value */

#define SDHC_CR_RESPTYP_Pos                   _U_(0)                                               /**< (SDHC_CR) Response Type Position */
#define SDHC_CR_RESPTYP_Msk                   (_U_(0x3) << SDHC_CR_RESPTYP_Pos)                    /**< (SDHC_CR) Response Type Mask */
#define SDHC_CR_RESPTYP(value)                (SDHC_CR_RESPTYP_Msk & ((value) << SDHC_CR_RESPTYP_Pos))
#define   SDHC_CR_RESPTYP_NONE_Val            _U_(0x0)                                             /**< (SDHC_CR) No response  */
#define   SDHC_CR_RESPTYP_136_BIT_Val         _U_(0x1)                                             /**< (SDHC_CR) 136-bit response  */
#define   SDHC_CR_RESPTYP_48_BIT_Val          _U_(0x2)                                             /**< (SDHC_CR) 48-bit response  */
#define   SDHC_CR_RESPTYP_48_BIT_BUSY_Val     _U_(0x3)                                             /**< (SDHC_CR) 48-bit response check busy after response  */
#define SDHC_CR_RESPTYP_NONE                  (SDHC_CR_RESPTYP_NONE_Val << SDHC_CR_RESPTYP_Pos)    /**< (SDHC_CR) No response Position  */
#define SDHC_CR_RESPTYP_136_BIT               (SDHC_CR_RESPTYP_136_BIT_Val << SDHC_CR_RESPTYP_Pos) /**< (SDHC_CR) 136-bit response Position  */
#define SDHC_CR_RESPTYP_48_BIT                (SDHC_CR_RESPTYP_48_BIT_Val << SDHC_CR_RESPTYP_Pos)  /**< (SDHC_CR) 48-bit response Position  */
#define SDHC_CR_RESPTYP_48_BIT_BUSY           (SDHC_CR_RESPTYP_48_BIT_BUSY_Val << SDHC_CR_RESPTYP_Pos) /**< (SDHC_CR) 48-bit response check busy after response Position  */
#define SDHC_CR_CMDCCEN_Pos                   _U_(3)                                               /**< (SDHC_CR) Command CRC Check Enable Position */
#define SDHC_CR_CMDCCEN_Msk                   (_U_(0x1) << SDHC_CR_CMDCCEN_Pos)                    /**< (SDHC_CR) Command CRC Check Enable Mask */
#define SDHC_CR_CMDCCEN(value)                (SDHC_CR_CMDCCEN_Msk & ((value) << SDHC_CR_CMDCCEN_Pos))
#define   SDHC_CR_CMDCCEN_DISABLE_Val         _U_(0x0)                                             /**< (SDHC_CR) Disable  */
#define   SDHC_CR_CMDCCEN_ENABLE_Val          _U_(0x1)                                             /**< (SDHC_CR) Enable  */
#define SDHC_CR_CMDCCEN_DISABLE               (SDHC_CR_CMDCCEN_DISABLE_Val << SDHC_CR_CMDCCEN_Pos) /**< (SDHC_CR) Disable Position  */
#define SDHC_CR_CMDCCEN_ENABLE                (SDHC_CR_CMDCCEN_ENABLE_Val << SDHC_CR_CMDCCEN_Pos)  /**< (SDHC_CR) Enable Position  */
#define SDHC_CR_CMDICEN_Pos                   _U_(4)                                               /**< (SDHC_CR) Command Index Check Enable Position */
#define SDHC_CR_CMDICEN_Msk                   (_U_(0x1) << SDHC_CR_CMDICEN_Pos)                    /**< (SDHC_CR) Command Index Check Enable Mask */
#define SDHC_CR_CMDICEN(value)                (SDHC_CR_CMDICEN_Msk & ((value) << SDHC_CR_CMDICEN_Pos))
#define   SDHC_CR_CMDICEN_DISABLE_Val         _U_(0x0)                                             /**< (SDHC_CR) Disable  */
#define   SDHC_CR_CMDICEN_ENABLE_Val          _U_(0x1)                                             /**< (SDHC_CR) Enable  */
#define SDHC_CR_CMDICEN_DISABLE               (SDHC_CR_CMDICEN_DISABLE_Val << SDHC_CR_CMDICEN_Pos) /**< (SDHC_CR) Disable Position  */
#define SDHC_CR_CMDICEN_ENABLE                (SDHC_CR_CMDICEN_ENABLE_Val << SDHC_CR_CMDICEN_Pos)  /**< (SDHC_CR) Enable Position  */
#define SDHC_CR_DPSEL_Pos                     _U_(5)                                               /**< (SDHC_CR) Data Present Select Position */
#define SDHC_CR_DPSEL_Msk                     (_U_(0x1) << SDHC_CR_DPSEL_Pos)                      /**< (SDHC_CR) Data Present Select Mask */
#define SDHC_CR_DPSEL(value)                  (SDHC_CR_DPSEL_Msk & ((value) << SDHC_CR_DPSEL_Pos))
#define   SDHC_CR_DPSEL_NO_DATA_Val           _U_(0x0)                                             /**< (SDHC_CR) No Data Present  */
#define   SDHC_CR_DPSEL_DATA_Val              _U_(0x1)                                             /**< (SDHC_CR) Data Present  */
#define SDHC_CR_DPSEL_NO_DATA                 (SDHC_CR_DPSEL_NO_DATA_Val << SDHC_CR_DPSEL_Pos)     /**< (SDHC_CR) No Data Present Position  */
#define SDHC_CR_DPSEL_DATA                    (SDHC_CR_DPSEL_DATA_Val << SDHC_CR_DPSEL_Pos)        /**< (SDHC_CR) Data Present Position  */
#define SDHC_CR_CMDTYP_Pos                    _U_(6)                                               /**< (SDHC_CR) Command Type Position */
#define SDHC_CR_CMDTYP_Msk                    (_U_(0x3) << SDHC_CR_CMDTYP_Pos)                     /**< (SDHC_CR) Command Type Mask */
#define SDHC_CR_CMDTYP(value)                 (SDHC_CR_CMDTYP_Msk & ((value) << SDHC_CR_CMDTYP_Pos))
#define   SDHC_CR_CMDTYP_NORMAL_Val           _U_(0x0)                                             /**< (SDHC_CR) Other commands  */
#define   SDHC_CR_CMDTYP_SUSPEND_Val          _U_(0x1)                                             /**< (SDHC_CR) CMD52 for writing Bus Suspend in CCCR  */
#define   SDHC_CR_CMDTYP_RESUME_Val           _U_(0x2)                                             /**< (SDHC_CR) CMD52 for writing Function Select in CCCR  */
#define   SDHC_CR_CMDTYP_ABORT_Val            _U_(0x3)                                             /**< (SDHC_CR) CMD12, CMD52 for writing I/O Abort in CCCR  */
#define SDHC_CR_CMDTYP_NORMAL                 (SDHC_CR_CMDTYP_NORMAL_Val << SDHC_CR_CMDTYP_Pos)    /**< (SDHC_CR) Other commands Position  */
#define SDHC_CR_CMDTYP_SUSPEND                (SDHC_CR_CMDTYP_SUSPEND_Val << SDHC_CR_CMDTYP_Pos)   /**< (SDHC_CR) CMD52 for writing Bus Suspend in CCCR Position  */
#define SDHC_CR_CMDTYP_RESUME                 (SDHC_CR_CMDTYP_RESUME_Val << SDHC_CR_CMDTYP_Pos)    /**< (SDHC_CR) CMD52 for writing Function Select in CCCR Position  */
#define SDHC_CR_CMDTYP_ABORT                  (SDHC_CR_CMDTYP_ABORT_Val << SDHC_CR_CMDTYP_Pos)     /**< (SDHC_CR) CMD12, CMD52 for writing I/O Abort in CCCR Position  */
#define SDHC_CR_CMDIDX_Pos                    _U_(8)                                               /**< (SDHC_CR) Command Index Position */
#define SDHC_CR_CMDIDX_Msk                    (_U_(0x3F) << SDHC_CR_CMDIDX_Pos)                    /**< (SDHC_CR) Command Index Mask */
#define SDHC_CR_CMDIDX(value)                 (SDHC_CR_CMDIDX_Msk & ((value) << SDHC_CR_CMDIDX_Pos))
#define SDHC_CR_Msk                           _U_(0x3FFB)                                          /**< (SDHC_CR) Register Mask  */


/* -------- SDHC_RR : (SDHC Offset: 0x10) ( R/ 32) Response -------- */
#define SDHC_RR_RESETVALUE                    _U_(0x00)                                            /**<  (SDHC_RR) Response  Reset Value */

#define SDHC_RR_CMDRESP_Pos                   _U_(0)                                               /**< (SDHC_RR) Command Response Position */
#define SDHC_RR_CMDRESP_Msk                   (_U_(0xFFFFFFFF) << SDHC_RR_CMDRESP_Pos)             /**< (SDHC_RR) Command Response Mask */
#define SDHC_RR_CMDRESP(value)                (SDHC_RR_CMDRESP_Msk & ((value) << SDHC_RR_CMDRESP_Pos))
#define SDHC_RR_Msk                           _U_(0xFFFFFFFF)                                      /**< (SDHC_RR) Register Mask  */


/* -------- SDHC_BDPR : (SDHC Offset: 0x20) (R/W 32) Buffer Data Port -------- */
#define SDHC_BDPR_RESETVALUE                  _U_(0x00)                                            /**<  (SDHC_BDPR) Buffer Data Port  Reset Value */

#define SDHC_BDPR_BUFDATA_Pos                 _U_(0)                                               /**< (SDHC_BDPR) Buffer Data Position */
#define SDHC_BDPR_BUFDATA_Msk                 (_U_(0xFFFFFFFF) << SDHC_BDPR_BUFDATA_Pos)           /**< (SDHC_BDPR) Buffer Data Mask */
#define SDHC_BDPR_BUFDATA(value)              (SDHC_BDPR_BUFDATA_Msk & ((value) << SDHC_BDPR_BUFDATA_Pos))
#define SDHC_BDPR_Msk                         _U_(0xFFFFFFFF)                                      /**< (SDHC_BDPR) Register Mask  */


/* -------- SDHC_PSR : (SDHC Offset: 0x24) ( R/ 32) Present State -------- */
#define SDHC_PSR_RESETVALUE                   _U_(0xF80000)                                        /**<  (SDHC_PSR) Present State  Reset Value */

#define SDHC_PSR_CMDINHC_Pos                  _U_(0)                                               /**< (SDHC_PSR) Command Inhibit (CMD) Position */
#define SDHC_PSR_CMDINHC_Msk                  (_U_(0x1) << SDHC_PSR_CMDINHC_Pos)                   /**< (SDHC_PSR) Command Inhibit (CMD) Mask */
#define SDHC_PSR_CMDINHC(value)               (SDHC_PSR_CMDINHC_Msk & ((value) << SDHC_PSR_CMDINHC_Pos))
#define   SDHC_PSR_CMDINHC_CAN_Val            _U_(0x0)                                             /**< (SDHC_PSR) Can issue command using only CMD line  */
#define   SDHC_PSR_CMDINHC_CANNOT_Val         _U_(0x1)                                             /**< (SDHC_PSR) Cannot issue command  */
#define SDHC_PSR_CMDINHC_CAN                  (SDHC_PSR_CMDINHC_CAN_Val << SDHC_PSR_CMDINHC_Pos)   /**< (SDHC_PSR) Can issue command using only CMD line Position  */
#define SDHC_PSR_CMDINHC_CANNOT               (SDHC_PSR_CMDINHC_CANNOT_Val << SDHC_PSR_CMDINHC_Pos) /**< (SDHC_PSR) Cannot issue command Position  */
#define SDHC_PSR_CMDINHD_Pos                  _U_(1)                                               /**< (SDHC_PSR) Command Inhibit (DAT) Position */
#define SDHC_PSR_CMDINHD_Msk                  (_U_(0x1) << SDHC_PSR_CMDINHD_Pos)                   /**< (SDHC_PSR) Command Inhibit (DAT) Mask */
#define SDHC_PSR_CMDINHD(value)               (SDHC_PSR_CMDINHD_Msk & ((value) << SDHC_PSR_CMDINHD_Pos))
#define   SDHC_PSR_CMDINHD_CAN_Val            _U_(0x0)                                             /**< (SDHC_PSR) Can issue command which uses the DAT line  */
#define   SDHC_PSR_CMDINHD_CANNOT_Val         _U_(0x1)                                             /**< (SDHC_PSR) Cannot issue command which uses the DAT line  */
#define SDHC_PSR_CMDINHD_CAN                  (SDHC_PSR_CMDINHD_CAN_Val << SDHC_PSR_CMDINHD_Pos)   /**< (SDHC_PSR) Can issue command which uses the DAT line Position  */
#define SDHC_PSR_CMDINHD_CANNOT               (SDHC_PSR_CMDINHD_CANNOT_Val << SDHC_PSR_CMDINHD_Pos) /**< (SDHC_PSR) Cannot issue command which uses the DAT line Position  */
#define SDHC_PSR_DLACT_Pos                    _U_(2)                                               /**< (SDHC_PSR) DAT Line Active Position */
#define SDHC_PSR_DLACT_Msk                    (_U_(0x1) << SDHC_PSR_DLACT_Pos)                     /**< (SDHC_PSR) DAT Line Active Mask */
#define SDHC_PSR_DLACT(value)                 (SDHC_PSR_DLACT_Msk & ((value) << SDHC_PSR_DLACT_Pos))
#define   SDHC_PSR_DLACT_INACTIVE_Val         _U_(0x0)                                             /**< (SDHC_PSR) DAT Line Inactive  */
#define   SDHC_PSR_DLACT_ACTIVE_Val           _U_(0x1)                                             /**< (SDHC_PSR) DAT Line Active  */
#define SDHC_PSR_DLACT_INACTIVE               (SDHC_PSR_DLACT_INACTIVE_Val << SDHC_PSR_DLACT_Pos)  /**< (SDHC_PSR) DAT Line Inactive Position  */
#define SDHC_PSR_DLACT_ACTIVE                 (SDHC_PSR_DLACT_ACTIVE_Val << SDHC_PSR_DLACT_Pos)    /**< (SDHC_PSR) DAT Line Active Position  */
#define SDHC_PSR_RTREQ_Pos                    _U_(3)                                               /**< (SDHC_PSR) Re-Tuning Request Position */
#define SDHC_PSR_RTREQ_Msk                    (_U_(0x1) << SDHC_PSR_RTREQ_Pos)                     /**< (SDHC_PSR) Re-Tuning Request Mask */
#define SDHC_PSR_RTREQ(value)                 (SDHC_PSR_RTREQ_Msk & ((value) << SDHC_PSR_RTREQ_Pos))
#define   SDHC_PSR_RTREQ_OK_Val               _U_(0x0)                                             /**< (SDHC_PSR) Fixed or well-tuned sampling clock  */
#define   SDHC_PSR_RTREQ_REQUIRED_Val         _U_(0x1)                                             /**< (SDHC_PSR) Sampling clock needs re-tuning  */
#define SDHC_PSR_RTREQ_OK                     (SDHC_PSR_RTREQ_OK_Val << SDHC_PSR_RTREQ_Pos)        /**< (SDHC_PSR) Fixed or well-tuned sampling clock Position  */
#define SDHC_PSR_RTREQ_REQUIRED               (SDHC_PSR_RTREQ_REQUIRED_Val << SDHC_PSR_RTREQ_Pos)  /**< (SDHC_PSR) Sampling clock needs re-tuning Position  */
#define SDHC_PSR_WTACT_Pos                    _U_(8)                                               /**< (SDHC_PSR) Write Transfer Active Position */
#define SDHC_PSR_WTACT_Msk                    (_U_(0x1) << SDHC_PSR_WTACT_Pos)                     /**< (SDHC_PSR) Write Transfer Active Mask */
#define SDHC_PSR_WTACT(value)                 (SDHC_PSR_WTACT_Msk & ((value) << SDHC_PSR_WTACT_Pos))
#define   SDHC_PSR_WTACT_NO_Val               _U_(0x0)                                             /**< (SDHC_PSR) No valid data  */
#define   SDHC_PSR_WTACT_YES_Val              _U_(0x1)                                             /**< (SDHC_PSR) Transferring data  */
#define SDHC_PSR_WTACT_NO                     (SDHC_PSR_WTACT_NO_Val << SDHC_PSR_WTACT_Pos)        /**< (SDHC_PSR) No valid data Position  */
#define SDHC_PSR_WTACT_YES                    (SDHC_PSR_WTACT_YES_Val << SDHC_PSR_WTACT_Pos)       /**< (SDHC_PSR) Transferring data Position  */
#define SDHC_PSR_RTACT_Pos                    _U_(9)                                               /**< (SDHC_PSR) Read Transfer Active Position */
#define SDHC_PSR_RTACT_Msk                    (_U_(0x1) << SDHC_PSR_RTACT_Pos)                     /**< (SDHC_PSR) Read Transfer Active Mask */
#define SDHC_PSR_RTACT(value)                 (SDHC_PSR_RTACT_Msk & ((value) << SDHC_PSR_RTACT_Pos))
#define   SDHC_PSR_RTACT_NO_Val               _U_(0x0)                                             /**< (SDHC_PSR) No valid data  */
#define   SDHC_PSR_RTACT_YES_Val              _U_(0x1)                                             /**< (SDHC_PSR) Transferring data  */
#define SDHC_PSR_RTACT_NO                     (SDHC_PSR_RTACT_NO_Val << SDHC_PSR_RTACT_Pos)        /**< (SDHC_PSR) No valid data Position  */
#define SDHC_PSR_RTACT_YES                    (SDHC_PSR_RTACT_YES_Val << SDHC_PSR_RTACT_Pos)       /**< (SDHC_PSR) Transferring data Position  */
#define SDHC_PSR_BUFWREN_Pos                  _U_(10)                                              /**< (SDHC_PSR) Buffer Write Enable Position */
#define SDHC_PSR_BUFWREN_Msk                  (_U_(0x1) << SDHC_PSR_BUFWREN_Pos)                   /**< (SDHC_PSR) Buffer Write Enable Mask */
#define SDHC_PSR_BUFWREN(value)               (SDHC_PSR_BUFWREN_Msk & ((value) << SDHC_PSR_BUFWREN_Pos))
#define   SDHC_PSR_BUFWREN_DISABLE_Val        _U_(0x0)                                             /**< (SDHC_PSR) Write disable  */
#define   SDHC_PSR_BUFWREN_ENABLE_Val         _U_(0x1)                                             /**< (SDHC_PSR) Write enable  */
#define SDHC_PSR_BUFWREN_DISABLE              (SDHC_PSR_BUFWREN_DISABLE_Val << SDHC_PSR_BUFWREN_Pos) /**< (SDHC_PSR) Write disable Position  */
#define SDHC_PSR_BUFWREN_ENABLE               (SDHC_PSR_BUFWREN_ENABLE_Val << SDHC_PSR_BUFWREN_Pos) /**< (SDHC_PSR) Write enable Position  */
#define SDHC_PSR_BUFRDEN_Pos                  _U_(11)                                              /**< (SDHC_PSR) Buffer Read Enable Position */
#define SDHC_PSR_BUFRDEN_Msk                  (_U_(0x1) << SDHC_PSR_BUFRDEN_Pos)                   /**< (SDHC_PSR) Buffer Read Enable Mask */
#define SDHC_PSR_BUFRDEN(value)               (SDHC_PSR_BUFRDEN_Msk & ((value) << SDHC_PSR_BUFRDEN_Pos))
#define   SDHC_PSR_BUFRDEN_DISABLE_Val        _U_(0x0)                                             /**< (SDHC_PSR) Read disable  */
#define   SDHC_PSR_BUFRDEN_ENABLE_Val         _U_(0x1)                                             /**< (SDHC_PSR) Read enable  */
#define SDHC_PSR_BUFRDEN_DISABLE              (SDHC_PSR_BUFRDEN_DISABLE_Val << SDHC_PSR_BUFRDEN_Pos) /**< (SDHC_PSR) Read disable Position  */
#define SDHC_PSR_BUFRDEN_ENABLE               (SDHC_PSR_BUFRDEN_ENABLE_Val << SDHC_PSR_BUFRDEN_Pos) /**< (SDHC_PSR) Read enable Position  */
#define SDHC_PSR_CARDINS_Pos                  _U_(16)                                              /**< (SDHC_PSR) Card Inserted Position */
#define SDHC_PSR_CARDINS_Msk                  (_U_(0x1) << SDHC_PSR_CARDINS_Pos)                   /**< (SDHC_PSR) Card Inserted Mask */
#define SDHC_PSR_CARDINS(value)               (SDHC_PSR_CARDINS_Msk & ((value) << SDHC_PSR_CARDINS_Pos))
#define   SDHC_PSR_CARDINS_NO_Val             _U_(0x0)                                             /**< (SDHC_PSR) Reset or Debouncing or No Card  */
#define   SDHC_PSR_CARDINS_YES_Val            _U_(0x1)                                             /**< (SDHC_PSR) Card inserted  */
#define SDHC_PSR_CARDINS_NO                   (SDHC_PSR_CARDINS_NO_Val << SDHC_PSR_CARDINS_Pos)    /**< (SDHC_PSR) Reset or Debouncing or No Card Position  */
#define SDHC_PSR_CARDINS_YES                  (SDHC_PSR_CARDINS_YES_Val << SDHC_PSR_CARDINS_Pos)   /**< (SDHC_PSR) Card inserted Position  */
#define SDHC_PSR_CARDSS_Pos                   _U_(17)                                              /**< (SDHC_PSR) Card State Stable Position */
#define SDHC_PSR_CARDSS_Msk                   (_U_(0x1) << SDHC_PSR_CARDSS_Pos)                    /**< (SDHC_PSR) Card State Stable Mask */
#define SDHC_PSR_CARDSS(value)                (SDHC_PSR_CARDSS_Msk & ((value) << SDHC_PSR_CARDSS_Pos))
#define   SDHC_PSR_CARDSS_NO_Val              _U_(0x0)                                             /**< (SDHC_PSR) Reset or Debouncing  */
#define   SDHC_PSR_CARDSS_YES_Val             _U_(0x1)                                             /**< (SDHC_PSR) No Card or Insered  */
#define SDHC_PSR_CARDSS_NO                    (SDHC_PSR_CARDSS_NO_Val << SDHC_PSR_CARDSS_Pos)      /**< (SDHC_PSR) Reset or Debouncing Position  */
#define SDHC_PSR_CARDSS_YES                   (SDHC_PSR_CARDSS_YES_Val << SDHC_PSR_CARDSS_Pos)     /**< (SDHC_PSR) No Card or Insered Position  */
#define SDHC_PSR_CARDDPL_Pos                  _U_(18)                                              /**< (SDHC_PSR) Card Detect Pin Level Position */
#define SDHC_PSR_CARDDPL_Msk                  (_U_(0x1) << SDHC_PSR_CARDDPL_Pos)                   /**< (SDHC_PSR) Card Detect Pin Level Mask */
#define SDHC_PSR_CARDDPL(value)               (SDHC_PSR_CARDDPL_Msk & ((value) << SDHC_PSR_CARDDPL_Pos))
#define   SDHC_PSR_CARDDPL_NO_Val             _U_(0x0)                                             /**< (SDHC_PSR) No card present (SDCD#=1)  */
#define   SDHC_PSR_CARDDPL_YES_Val            _U_(0x1)                                             /**< (SDHC_PSR) Card present (SDCD#=0)  */
#define SDHC_PSR_CARDDPL_NO                   (SDHC_PSR_CARDDPL_NO_Val << SDHC_PSR_CARDDPL_Pos)    /**< (SDHC_PSR) No card present (SDCD#=1) Position  */
#define SDHC_PSR_CARDDPL_YES                  (SDHC_PSR_CARDDPL_YES_Val << SDHC_PSR_CARDDPL_Pos)   /**< (SDHC_PSR) Card present (SDCD#=0) Position  */
#define SDHC_PSR_WRPPL_Pos                    _U_(19)                                              /**< (SDHC_PSR) Write Protect Pin Level Position */
#define SDHC_PSR_WRPPL_Msk                    (_U_(0x1) << SDHC_PSR_WRPPL_Pos)                     /**< (SDHC_PSR) Write Protect Pin Level Mask */
#define SDHC_PSR_WRPPL(value)                 (SDHC_PSR_WRPPL_Msk & ((value) << SDHC_PSR_WRPPL_Pos))
#define   SDHC_PSR_WRPPL_PROTECTED_Val        _U_(0x0)                                             /**< (SDHC_PSR) Write protected (SDWP#=0)  */
#define   SDHC_PSR_WRPPL_ENABLED_Val          _U_(0x1)                                             /**< (SDHC_PSR) Write enabled (SDWP#=1)  */
#define SDHC_PSR_WRPPL_PROTECTED              (SDHC_PSR_WRPPL_PROTECTED_Val << SDHC_PSR_WRPPL_Pos) /**< (SDHC_PSR) Write protected (SDWP#=0) Position  */
#define SDHC_PSR_WRPPL_ENABLED                (SDHC_PSR_WRPPL_ENABLED_Val << SDHC_PSR_WRPPL_Pos)   /**< (SDHC_PSR) Write enabled (SDWP#=1) Position  */
#define SDHC_PSR_DATLL_Pos                    _U_(20)                                              /**< (SDHC_PSR) DAT[3:0] Line Level Position */
#define SDHC_PSR_DATLL_Msk                    (_U_(0xF) << SDHC_PSR_DATLL_Pos)                     /**< (SDHC_PSR) DAT[3:0] Line Level Mask */
#define SDHC_PSR_DATLL(value)                 (SDHC_PSR_DATLL_Msk & ((value) << SDHC_PSR_DATLL_Pos))
#define SDHC_PSR_CMDLL_Pos                    _U_(24)                                              /**< (SDHC_PSR) CMD Line Level Position */
#define SDHC_PSR_CMDLL_Msk                    (_U_(0x1) << SDHC_PSR_CMDLL_Pos)                     /**< (SDHC_PSR) CMD Line Level Mask */
#define SDHC_PSR_CMDLL(value)                 (SDHC_PSR_CMDLL_Msk & ((value) << SDHC_PSR_CMDLL_Pos))
#define SDHC_PSR_Msk                          _U_(0x01FF0F0F)                                      /**< (SDHC_PSR) Register Mask  */


/* -------- SDHC_HC1R : (SDHC Offset: 0x28) (R/W 8) Host Control 1 -------- */
#define SDHC_HC1R_RESETVALUE                  _U_(0xE00)                                           /**<  (SDHC_HC1R) Host Control 1  Reset Value */

#define SDHC_HC1R_LEDCTRL_Pos                 _U_(0)                                               /**< (SDHC_HC1R) LED Control Position */
#define SDHC_HC1R_LEDCTRL_Msk                 (_U_(0x1) << SDHC_HC1R_LEDCTRL_Pos)                  /**< (SDHC_HC1R) LED Control Mask */
#define SDHC_HC1R_LEDCTRL(value)              (SDHC_HC1R_LEDCTRL_Msk & ((value) << SDHC_HC1R_LEDCTRL_Pos))
#define   SDHC_HC1R_LEDCTRL_OFF_Val           _U_(0x0)                                             /**< (SDHC_HC1R) LED off  */
#define   SDHC_HC1R_LEDCTRL_ON_Val            _U_(0x1)                                             /**< (SDHC_HC1R) LED on  */
#define SDHC_HC1R_LEDCTRL_OFF                 (SDHC_HC1R_LEDCTRL_OFF_Val << SDHC_HC1R_LEDCTRL_Pos) /**< (SDHC_HC1R) LED off Position  */
#define SDHC_HC1R_LEDCTRL_ON                  (SDHC_HC1R_LEDCTRL_ON_Val << SDHC_HC1R_LEDCTRL_Pos)  /**< (SDHC_HC1R) LED on Position  */
#define SDHC_HC1R_DW_Pos                      _U_(1)                                               /**< (SDHC_HC1R) Data Width Position */
#define SDHC_HC1R_DW_Msk                      (_U_(0x1) << SDHC_HC1R_DW_Pos)                       /**< (SDHC_HC1R) Data Width Mask */
#define SDHC_HC1R_DW(value)                   (SDHC_HC1R_DW_Msk & ((value) << SDHC_HC1R_DW_Pos))  
#define   SDHC_HC1R_DW_1BIT_Val               _U_(0x0)                                             /**< (SDHC_HC1R) 1-bit mode  */
#define   SDHC_HC1R_DW_4BIT_Val               _U_(0x1)                                             /**< (SDHC_HC1R) 4-bit mode  */
#define SDHC_HC1R_DW_1BIT                     (SDHC_HC1R_DW_1BIT_Val << SDHC_HC1R_DW_Pos)          /**< (SDHC_HC1R) 1-bit mode Position  */
#define SDHC_HC1R_DW_4BIT                     (SDHC_HC1R_DW_4BIT_Val << SDHC_HC1R_DW_Pos)          /**< (SDHC_HC1R) 4-bit mode Position  */
#define SDHC_HC1R_HSEN_Pos                    _U_(2)                                               /**< (SDHC_HC1R) High Speed Enable Position */
#define SDHC_HC1R_HSEN_Msk                    (_U_(0x1) << SDHC_HC1R_HSEN_Pos)                     /**< (SDHC_HC1R) High Speed Enable Mask */
#define SDHC_HC1R_HSEN(value)                 (SDHC_HC1R_HSEN_Msk & ((value) << SDHC_HC1R_HSEN_Pos))
#define   SDHC_HC1R_HSEN_NORMAL_Val           _U_(0x0)                                             /**< (SDHC_HC1R) Normal Speed mode  */
#define   SDHC_HC1R_HSEN_HIGH_Val             _U_(0x1)                                             /**< (SDHC_HC1R) High Speed mode  */
#define SDHC_HC1R_HSEN_NORMAL                 (SDHC_HC1R_HSEN_NORMAL_Val << SDHC_HC1R_HSEN_Pos)    /**< (SDHC_HC1R) Normal Speed mode Position  */
#define SDHC_HC1R_HSEN_HIGH                   (SDHC_HC1R_HSEN_HIGH_Val << SDHC_HC1R_HSEN_Pos)      /**< (SDHC_HC1R) High Speed mode Position  */
#define SDHC_HC1R_DMASEL_Pos                  _U_(3)                                               /**< (SDHC_HC1R) DMA Select Position */
#define SDHC_HC1R_DMASEL_Msk                  (_U_(0x3) << SDHC_HC1R_DMASEL_Pos)                   /**< (SDHC_HC1R) DMA Select Mask */
#define SDHC_HC1R_DMASEL(value)               (SDHC_HC1R_DMASEL_Msk & ((value) << SDHC_HC1R_DMASEL_Pos))
#define   SDHC_HC1R_DMASEL_SDMA_Val           _U_(0x0)                                             /**< (SDHC_HC1R) SDMA is selected  */
#define   SDHC_HC1R_DMASEL_32BIT_Val          _U_(0x2)                                             /**< (SDHC_HC1R) 32-bit Address ADMA2 is selected  */
#define SDHC_HC1R_DMASEL_SDMA                 (SDHC_HC1R_DMASEL_SDMA_Val << SDHC_HC1R_DMASEL_Pos)  /**< (SDHC_HC1R) SDMA is selected Position  */
#define SDHC_HC1R_DMASEL_32BIT                (SDHC_HC1R_DMASEL_32BIT_Val << SDHC_HC1R_DMASEL_Pos) /**< (SDHC_HC1R) 32-bit Address ADMA2 is selected Position  */
#define SDHC_HC1R_CARDDTL_Pos                 _U_(6)                                               /**< (SDHC_HC1R) Card Detect Test Level Position */
#define SDHC_HC1R_CARDDTL_Msk                 (_U_(0x1) << SDHC_HC1R_CARDDTL_Pos)                  /**< (SDHC_HC1R) Card Detect Test Level Mask */
#define SDHC_HC1R_CARDDTL(value)              (SDHC_HC1R_CARDDTL_Msk & ((value) << SDHC_HC1R_CARDDTL_Pos))
#define   SDHC_HC1R_CARDDTL_NO_Val            _U_(0x0)                                             /**< (SDHC_HC1R) No Card  */
#define   SDHC_HC1R_CARDDTL_YES_Val           _U_(0x1)                                             /**< (SDHC_HC1R) Card Inserted  */
#define SDHC_HC1R_CARDDTL_NO                  (SDHC_HC1R_CARDDTL_NO_Val << SDHC_HC1R_CARDDTL_Pos)  /**< (SDHC_HC1R) No Card Position  */
#define SDHC_HC1R_CARDDTL_YES                 (SDHC_HC1R_CARDDTL_YES_Val << SDHC_HC1R_CARDDTL_Pos) /**< (SDHC_HC1R) Card Inserted Position  */
#define SDHC_HC1R_CARDDSEL_Pos                _U_(7)                                               /**< (SDHC_HC1R) Card Detect Signal Selection Position */
#define SDHC_HC1R_CARDDSEL_Msk                (_U_(0x1) << SDHC_HC1R_CARDDSEL_Pos)                 /**< (SDHC_HC1R) Card Detect Signal Selection Mask */
#define SDHC_HC1R_CARDDSEL(value)             (SDHC_HC1R_CARDDSEL_Msk & ((value) << SDHC_HC1R_CARDDSEL_Pos))
#define   SDHC_HC1R_CARDDSEL_NORMAL_Val       _U_(0x0)                                             /**< (SDHC_HC1R) SDCD# is selected (for normal use)  */
#define   SDHC_HC1R_CARDDSEL_TEST_Val         _U_(0x1)                                             /**< (SDHC_HC1R) The Card Select Test Level is selected (for test purpose)  */
#define SDHC_HC1R_CARDDSEL_NORMAL             (SDHC_HC1R_CARDDSEL_NORMAL_Val << SDHC_HC1R_CARDDSEL_Pos) /**< (SDHC_HC1R) SDCD# is selected (for normal use) Position  */
#define SDHC_HC1R_CARDDSEL_TEST               (SDHC_HC1R_CARDDSEL_TEST_Val << SDHC_HC1R_CARDDSEL_Pos) /**< (SDHC_HC1R) The Card Select Test Level is selected (for test purpose) Position  */
#define SDHC_HC1R_Msk                         _U_(0xDF)                                            /**< (SDHC_HC1R) Register Mask  */

/* EMMC mode */
#define SDHC_HC1R_EMMC_Msk                    _U_(0x00)                                             /**< (SDHC_HC1R_EMMC) Register Mask  */


/* -------- SDHC_PCR : (SDHC Offset: 0x29) (R/W 8) Power Control -------- */
#define SDHC_PCR_RESETVALUE                   _U_(0x0E)                                            /**<  (SDHC_PCR) Power Control  Reset Value */

#define SDHC_PCR_SDBPWR_Pos                   _U_(0)                                               /**< (SDHC_PCR) SD Bus Power Position */
#define SDHC_PCR_SDBPWR_Msk                   (_U_(0x1) << SDHC_PCR_SDBPWR_Pos)                    /**< (SDHC_PCR) SD Bus Power Mask */
#define SDHC_PCR_SDBPWR(value)                (SDHC_PCR_SDBPWR_Msk & ((value) << SDHC_PCR_SDBPWR_Pos))
#define   SDHC_PCR_SDBPWR_OFF_Val             _U_(0x0)                                             /**< (SDHC_PCR) Power off  */
#define   SDHC_PCR_SDBPWR_ON_Val              _U_(0x1)                                             /**< (SDHC_PCR) Power on  */
#define SDHC_PCR_SDBPWR_OFF                   (SDHC_PCR_SDBPWR_OFF_Val << SDHC_PCR_SDBPWR_Pos)     /**< (SDHC_PCR) Power off Position  */
#define SDHC_PCR_SDBPWR_ON                    (SDHC_PCR_SDBPWR_ON_Val << SDHC_PCR_SDBPWR_Pos)      /**< (SDHC_PCR) Power on Position  */
#define SDHC_PCR_SDBVSEL_Pos                  _U_(1)                                               /**< (SDHC_PCR) SD Bus Voltage Select Position */
#define SDHC_PCR_SDBVSEL_Msk                  (_U_(0x7) << SDHC_PCR_SDBVSEL_Pos)                   /**< (SDHC_PCR) SD Bus Voltage Select Mask */
#define SDHC_PCR_SDBVSEL(value)               (SDHC_PCR_SDBVSEL_Msk & ((value) << SDHC_PCR_SDBVSEL_Pos))
#define   SDHC_PCR_SDBVSEL_1V8_Val            _U_(0x5)                                             /**< (SDHC_PCR) 1.8V (Typ.)  */
#define   SDHC_PCR_SDBVSEL_3V0_Val            _U_(0x6)                                             /**< (SDHC_PCR) 3.0V (Typ.)  */
#define   SDHC_PCR_SDBVSEL_3V3_Val            _U_(0x7)                                             /**< (SDHC_PCR) 3.3V (Typ.)  */
#define SDHC_PCR_SDBVSEL_1V8                  (SDHC_PCR_SDBVSEL_1V8_Val << SDHC_PCR_SDBVSEL_Pos)   /**< (SDHC_PCR) 1.8V (Typ.) Position  */
#define SDHC_PCR_SDBVSEL_3V0                  (SDHC_PCR_SDBVSEL_3V0_Val << SDHC_PCR_SDBVSEL_Pos)   /**< (SDHC_PCR) 3.0V (Typ.) Position  */
#define SDHC_PCR_SDBVSEL_3V3                  (SDHC_PCR_SDBVSEL_3V3_Val << SDHC_PCR_SDBVSEL_Pos)   /**< (SDHC_PCR) 3.3V (Typ.) Position  */
#define SDHC_PCR_Msk                          _U_(0x0F)                                            /**< (SDHC_PCR) Register Mask  */


/* -------- SDHC_BGCR : (SDHC Offset: 0x2A) (R/W 8) Block Gap Control -------- */
#define SDHC_BGCR_RESETVALUE                  _U_(0x00)                                            /**<  (SDHC_BGCR) Block Gap Control  Reset Value */

#define SDHC_BGCR_STPBGR_Pos                  _U_(0)                                               /**< (SDHC_BGCR) Stop at Block Gap Request Position */
#define SDHC_BGCR_STPBGR_Msk                  (_U_(0x1) << SDHC_BGCR_STPBGR_Pos)                   /**< (SDHC_BGCR) Stop at Block Gap Request Mask */
#define SDHC_BGCR_STPBGR(value)               (SDHC_BGCR_STPBGR_Msk & ((value) << SDHC_BGCR_STPBGR_Pos))
#define   SDHC_BGCR_STPBGR_TRANSFER_Val       _U_(0x0)                                             /**< (SDHC_BGCR) Transfer  */
#define   SDHC_BGCR_STPBGR_STOP_Val           _U_(0x1)                                             /**< (SDHC_BGCR) Stop  */
#define SDHC_BGCR_STPBGR_TRANSFER             (SDHC_BGCR_STPBGR_TRANSFER_Val << SDHC_BGCR_STPBGR_Pos) /**< (SDHC_BGCR) Transfer Position  */
#define SDHC_BGCR_STPBGR_STOP                 (SDHC_BGCR_STPBGR_STOP_Val << SDHC_BGCR_STPBGR_Pos)  /**< (SDHC_BGCR) Stop Position  */
#define SDHC_BGCR_CONTR_Pos                   _U_(1)                                               /**< (SDHC_BGCR) Continue Request Position */
#define SDHC_BGCR_CONTR_Msk                   (_U_(0x1) << SDHC_BGCR_CONTR_Pos)                    /**< (SDHC_BGCR) Continue Request Mask */
#define SDHC_BGCR_CONTR(value)                (SDHC_BGCR_CONTR_Msk & ((value) << SDHC_BGCR_CONTR_Pos))
#define   SDHC_BGCR_CONTR_GO_ON_Val           _U_(0x0)                                             /**< (SDHC_BGCR) Not affected  */
#define   SDHC_BGCR_CONTR_RESTART_Val         _U_(0x1)                                             /**< (SDHC_BGCR) Restart  */
#define SDHC_BGCR_CONTR_GO_ON                 (SDHC_BGCR_CONTR_GO_ON_Val << SDHC_BGCR_CONTR_Pos)   /**< (SDHC_BGCR) Not affected Position  */
#define SDHC_BGCR_CONTR_RESTART               (SDHC_BGCR_CONTR_RESTART_Val << SDHC_BGCR_CONTR_Pos) /**< (SDHC_BGCR) Restart Position  */
#define SDHC_BGCR_RWCTRL_Pos                  _U_(2)                                               /**< (SDHC_BGCR) Read Wait Control Position */
#define SDHC_BGCR_RWCTRL_Msk                  (_U_(0x1) << SDHC_BGCR_RWCTRL_Pos)                   /**< (SDHC_BGCR) Read Wait Control Mask */
#define SDHC_BGCR_RWCTRL(value)               (SDHC_BGCR_RWCTRL_Msk & ((value) << SDHC_BGCR_RWCTRL_Pos))
#define   SDHC_BGCR_RWCTRL_DISABLE_Val        _U_(0x0)                                             /**< (SDHC_BGCR) Disable Read Wait Control  */
#define   SDHC_BGCR_RWCTRL_ENABLE_Val         _U_(0x1)                                             /**< (SDHC_BGCR) Enable Read Wait Control  */
#define SDHC_BGCR_RWCTRL_DISABLE              (SDHC_BGCR_RWCTRL_DISABLE_Val << SDHC_BGCR_RWCTRL_Pos) /**< (SDHC_BGCR) Disable Read Wait Control Position  */
#define SDHC_BGCR_RWCTRL_ENABLE               (SDHC_BGCR_RWCTRL_ENABLE_Val << SDHC_BGCR_RWCTRL_Pos) /**< (SDHC_BGCR) Enable Read Wait Control Position  */
#define SDHC_BGCR_INTBG_Pos                   _U_(3)                                               /**< (SDHC_BGCR) Interrupt at Block Gap Position */
#define SDHC_BGCR_INTBG_Msk                   (_U_(0x1) << SDHC_BGCR_INTBG_Pos)                    /**< (SDHC_BGCR) Interrupt at Block Gap Mask */
#define SDHC_BGCR_INTBG(value)                (SDHC_BGCR_INTBG_Msk & ((value) << SDHC_BGCR_INTBG_Pos))
#define   SDHC_BGCR_INTBG_DISABLED_Val        _U_(0x0)                                             /**< (SDHC_BGCR) Disabled  */
#define   SDHC_BGCR_INTBG_ENABLED_Val         _U_(0x1)                                             /**< (SDHC_BGCR) Enabled  */
#define SDHC_BGCR_INTBG_DISABLED              (SDHC_BGCR_INTBG_DISABLED_Val << SDHC_BGCR_INTBG_Pos) /**< (SDHC_BGCR) Disabled Position  */
#define SDHC_BGCR_INTBG_ENABLED               (SDHC_BGCR_INTBG_ENABLED_Val << SDHC_BGCR_INTBG_Pos) /**< (SDHC_BGCR) Enabled Position  */
#define SDHC_BGCR_Msk                         _U_(0x0F)                                            /**< (SDHC_BGCR) Register Mask  */

/* EMMC mode */
#define SDHC_BGCR_EMMC_Msk                    _U_(0x00)                                             /**< (SDHC_BGCR_EMMC) Register Mask  */


/* -------- SDHC_WCR : (SDHC Offset: 0x2B) (R/W 8) Wakeup Control -------- */
#define SDHC_WCR_RESETVALUE                   _U_(0x00)                                            /**<  (SDHC_WCR) Wakeup Control  Reset Value */

#define SDHC_WCR_WKENCINT_Pos                 _U_(0)                                               /**< (SDHC_WCR) Wakeup Event Enable on Card Interrupt Position */
#define SDHC_WCR_WKENCINT_Msk                 (_U_(0x1) << SDHC_WCR_WKENCINT_Pos)                  /**< (SDHC_WCR) Wakeup Event Enable on Card Interrupt Mask */
#define SDHC_WCR_WKENCINT(value)              (SDHC_WCR_WKENCINT_Msk & ((value) << SDHC_WCR_WKENCINT_Pos))
#define   SDHC_WCR_WKENCINT_DISABLE_Val       _U_(0x0)                                             /**< (SDHC_WCR) Disable  */
#define   SDHC_WCR_WKENCINT_ENABLE_Val        _U_(0x1)                                             /**< (SDHC_WCR) Enable  */
#define SDHC_WCR_WKENCINT_DISABLE             (SDHC_WCR_WKENCINT_DISABLE_Val << SDHC_WCR_WKENCINT_Pos) /**< (SDHC_WCR) Disable Position  */
#define SDHC_WCR_WKENCINT_ENABLE              (SDHC_WCR_WKENCINT_ENABLE_Val << SDHC_WCR_WKENCINT_Pos) /**< (SDHC_WCR) Enable Position  */
#define SDHC_WCR_WKENCINS_Pos                 _U_(1)                                               /**< (SDHC_WCR) Wakeup Event Enable on Card Insertion Position */
#define SDHC_WCR_WKENCINS_Msk                 (_U_(0x1) << SDHC_WCR_WKENCINS_Pos)                  /**< (SDHC_WCR) Wakeup Event Enable on Card Insertion Mask */
#define SDHC_WCR_WKENCINS(value)              (SDHC_WCR_WKENCINS_Msk & ((value) << SDHC_WCR_WKENCINS_Pos))
#define   SDHC_WCR_WKENCINS_DISABLE_Val       _U_(0x0)                                             /**< (SDHC_WCR) Disable  */
#define   SDHC_WCR_WKENCINS_ENABLE_Val        _U_(0x1)                                             /**< (SDHC_WCR) Enable  */
#define SDHC_WCR_WKENCINS_DISABLE             (SDHC_WCR_WKENCINS_DISABLE_Val << SDHC_WCR_WKENCINS_Pos) /**< (SDHC_WCR) Disable Position  */
#define SDHC_WCR_WKENCINS_ENABLE              (SDHC_WCR_WKENCINS_ENABLE_Val << SDHC_WCR_WKENCINS_Pos) /**< (SDHC_WCR) Enable Position  */
#define SDHC_WCR_WKENCREM_Pos                 _U_(2)                                               /**< (SDHC_WCR) Wakeup Event Enable on Card Removal Position */
#define SDHC_WCR_WKENCREM_Msk                 (_U_(0x1) << SDHC_WCR_WKENCREM_Pos)                  /**< (SDHC_WCR) Wakeup Event Enable on Card Removal Mask */
#define SDHC_WCR_WKENCREM(value)              (SDHC_WCR_WKENCREM_Msk & ((value) << SDHC_WCR_WKENCREM_Pos))
#define   SDHC_WCR_WKENCREM_DISABLE_Val       _U_(0x0)                                             /**< (SDHC_WCR) Disable  */
#define   SDHC_WCR_WKENCREM_ENABLE_Val        _U_(0x1)                                             /**< (SDHC_WCR) Enable  */
#define SDHC_WCR_WKENCREM_DISABLE             (SDHC_WCR_WKENCREM_DISABLE_Val << SDHC_WCR_WKENCREM_Pos) /**< (SDHC_WCR) Disable Position  */
#define SDHC_WCR_WKENCREM_ENABLE              (SDHC_WCR_WKENCREM_ENABLE_Val << SDHC_WCR_WKENCREM_Pos) /**< (SDHC_WCR) Enable Position  */
#define SDHC_WCR_Msk                          _U_(0x07)                                            /**< (SDHC_WCR) Register Mask  */


/* -------- SDHC_CCR : (SDHC Offset: 0x2C) (R/W 16) Clock Control -------- */
#define SDHC_CCR_RESETVALUE                   _U_(0x00)                                            /**<  (SDHC_CCR) Clock Control  Reset Value */

#define SDHC_CCR_INTCLKEN_Pos                 _U_(0)                                               /**< (SDHC_CCR) Internal Clock Enable Position */
#define SDHC_CCR_INTCLKEN_Msk                 (_U_(0x1) << SDHC_CCR_INTCLKEN_Pos)                  /**< (SDHC_CCR) Internal Clock Enable Mask */
#define SDHC_CCR_INTCLKEN(value)              (SDHC_CCR_INTCLKEN_Msk & ((value) << SDHC_CCR_INTCLKEN_Pos))
#define   SDHC_CCR_INTCLKEN_OFF_Val           _U_(0x0)                                             /**< (SDHC_CCR) Stop  */
#define   SDHC_CCR_INTCLKEN_ON_Val            _U_(0x1)                                             /**< (SDHC_CCR) Oscillate  */
#define SDHC_CCR_INTCLKEN_OFF                 (SDHC_CCR_INTCLKEN_OFF_Val << SDHC_CCR_INTCLKEN_Pos) /**< (SDHC_CCR) Stop Position  */
#define SDHC_CCR_INTCLKEN_ON                  (SDHC_CCR_INTCLKEN_ON_Val << SDHC_CCR_INTCLKEN_Pos)  /**< (SDHC_CCR) Oscillate Position  */
#define SDHC_CCR_INTCLKS_Pos                  _U_(1)                                               /**< (SDHC_CCR) Internal Clock Stable Position */
#define SDHC_CCR_INTCLKS_Msk                  (_U_(0x1) << SDHC_CCR_INTCLKS_Pos)                   /**< (SDHC_CCR) Internal Clock Stable Mask */
#define SDHC_CCR_INTCLKS(value)               (SDHC_CCR_INTCLKS_Msk & ((value) << SDHC_CCR_INTCLKS_Pos))
#define   SDHC_CCR_INTCLKS_NOT_READY_Val      _U_(0x0)                                             /**< (SDHC_CCR) Not Ready  */
#define   SDHC_CCR_INTCLKS_READY_Val          _U_(0x1)                                             /**< (SDHC_CCR) Ready  */
#define SDHC_CCR_INTCLKS_NOT_READY            (SDHC_CCR_INTCLKS_NOT_READY_Val << SDHC_CCR_INTCLKS_Pos) /**< (SDHC_CCR) Not Ready Position  */
#define SDHC_CCR_INTCLKS_READY                (SDHC_CCR_INTCLKS_READY_Val << SDHC_CCR_INTCLKS_Pos) /**< (SDHC_CCR) Ready Position  */
#define SDHC_CCR_SDCLKEN_Pos                  _U_(2)                                               /**< (SDHC_CCR) SD Clock Enable Position */
#define SDHC_CCR_SDCLKEN_Msk                  (_U_(0x1) << SDHC_CCR_SDCLKEN_Pos)                   /**< (SDHC_CCR) SD Clock Enable Mask */
#define SDHC_CCR_SDCLKEN(value)               (SDHC_CCR_SDCLKEN_Msk & ((value) << SDHC_CCR_SDCLKEN_Pos))
#define   SDHC_CCR_SDCLKEN_DISABLE_Val        _U_(0x0)                                             /**< (SDHC_CCR) Disable  */
#define   SDHC_CCR_SDCLKEN_ENABLE_Val         _U_(0x1)                                             /**< (SDHC_CCR) Enable  */
#define SDHC_CCR_SDCLKEN_DISABLE              (SDHC_CCR_SDCLKEN_DISABLE_Val << SDHC_CCR_SDCLKEN_Pos) /**< (SDHC_CCR) Disable Position  */
#define SDHC_CCR_SDCLKEN_ENABLE               (SDHC_CCR_SDCLKEN_ENABLE_Val << SDHC_CCR_SDCLKEN_Pos) /**< (SDHC_CCR) Enable Position  */
#define SDHC_CCR_CLKGSEL_Pos                  _U_(5)                                               /**< (SDHC_CCR) Clock Generator Select Position */
#define SDHC_CCR_CLKGSEL_Msk                  (_U_(0x1) << SDHC_CCR_CLKGSEL_Pos)                   /**< (SDHC_CCR) Clock Generator Select Mask */
#define SDHC_CCR_CLKGSEL(value)               (SDHC_CCR_CLKGSEL_Msk & ((value) << SDHC_CCR_CLKGSEL_Pos))
#define   SDHC_CCR_CLKGSEL_DIV_Val            _U_(0x0)                                             /**< (SDHC_CCR) Divided Clock Mode  */
#define   SDHC_CCR_CLKGSEL_PROG_Val           _U_(0x1)                                             /**< (SDHC_CCR) Programmable Clock Mode  */
#define SDHC_CCR_CLKGSEL_DIV                  (SDHC_CCR_CLKGSEL_DIV_Val << SDHC_CCR_CLKGSEL_Pos)   /**< (SDHC_CCR) Divided Clock Mode Position  */
#define SDHC_CCR_CLKGSEL_PROG                 (SDHC_CCR_CLKGSEL_PROG_Val << SDHC_CCR_CLKGSEL_Pos)  /**< (SDHC_CCR) Programmable Clock Mode Position  */
#define SDHC_CCR_USDCLKFSEL_Pos               _U_(6)                                               /**< (SDHC_CCR) Upper Bits of SDCLK Frequency Select Position */
#define SDHC_CCR_USDCLKFSEL_Msk               (_U_(0x3) << SDHC_CCR_USDCLKFSEL_Pos)                /**< (SDHC_CCR) Upper Bits of SDCLK Frequency Select Mask */
#define SDHC_CCR_USDCLKFSEL(value)            (SDHC_CCR_USDCLKFSEL_Msk & ((value) << SDHC_CCR_USDCLKFSEL_Pos))
#define SDHC_CCR_SDCLKFSEL_Pos                _U_(8)                                               /**< (SDHC_CCR) SDCLK Frequency Select Position */
#define SDHC_CCR_SDCLKFSEL_Msk                (_U_(0xFF) << SDHC_CCR_SDCLKFSEL_Pos)                /**< (SDHC_CCR) SDCLK Frequency Select Mask */
#define SDHC_CCR_SDCLKFSEL(value)             (SDHC_CCR_SDCLKFSEL_Msk & ((value) << SDHC_CCR_SDCLKFSEL_Pos))
#define SDHC_CCR_Msk                          _U_(0xFFE7)                                          /**< (SDHC_CCR) Register Mask  */


/* -------- SDHC_TCR : (SDHC Offset: 0x2E) (R/W 8) Timeout Control -------- */
#define SDHC_TCR_RESETVALUE                   _U_(0x00)                                            /**<  (SDHC_TCR) Timeout Control  Reset Value */

#define SDHC_TCR_DTCVAL_Pos                   _U_(0)                                               /**< (SDHC_TCR) Data Timeout Counter Value Position */
#define SDHC_TCR_DTCVAL_Msk                   (_U_(0xF) << SDHC_TCR_DTCVAL_Pos)                    /**< (SDHC_TCR) Data Timeout Counter Value Mask */
#define SDHC_TCR_DTCVAL(value)                (SDHC_TCR_DTCVAL_Msk & ((value) << SDHC_TCR_DTCVAL_Pos))
#define SDHC_TCR_Msk                          _U_(0x0F)                                            /**< (SDHC_TCR) Register Mask  */


/* -------- SDHC_SRR : (SDHC Offset: 0x2F) (R/W 8) Software Reset -------- */
#define SDHC_SRR_RESETVALUE                   _U_(0x00)                                            /**<  (SDHC_SRR) Software Reset  Reset Value */

#define SDHC_SRR_SWRSTALL_Pos                 _U_(0)                                               /**< (SDHC_SRR) Software Reset For All Position */
#define SDHC_SRR_SWRSTALL_Msk                 (_U_(0x1) << SDHC_SRR_SWRSTALL_Pos)                  /**< (SDHC_SRR) Software Reset For All Mask */
#define SDHC_SRR_SWRSTALL(value)              (SDHC_SRR_SWRSTALL_Msk & ((value) << SDHC_SRR_SWRSTALL_Pos))
#define   SDHC_SRR_SWRSTALL_WORK_Val          _U_(0x0)                                             /**< (SDHC_SRR) Work  */
#define   SDHC_SRR_SWRSTALL_RESET_Val         _U_(0x1)                                             /**< (SDHC_SRR) Reset  */
#define SDHC_SRR_SWRSTALL_WORK                (SDHC_SRR_SWRSTALL_WORK_Val << SDHC_SRR_SWRSTALL_Pos) /**< (SDHC_SRR) Work Position  */
#define SDHC_SRR_SWRSTALL_RESET               (SDHC_SRR_SWRSTALL_RESET_Val << SDHC_SRR_SWRSTALL_Pos) /**< (SDHC_SRR) Reset Position  */
#define SDHC_SRR_SWRSTCMD_Pos                 _U_(1)                                               /**< (SDHC_SRR) Software Reset For CMD Line Position */
#define SDHC_SRR_SWRSTCMD_Msk                 (_U_(0x1) << SDHC_SRR_SWRSTCMD_Pos)                  /**< (SDHC_SRR) Software Reset For CMD Line Mask */
#define SDHC_SRR_SWRSTCMD(value)              (SDHC_SRR_SWRSTCMD_Msk & ((value) << SDHC_SRR_SWRSTCMD_Pos))
#define   SDHC_SRR_SWRSTCMD_WORK_Val          _U_(0x0)                                             /**< (SDHC_SRR) Work  */
#define   SDHC_SRR_SWRSTCMD_RESET_Val         _U_(0x1)                                             /**< (SDHC_SRR) Reset  */
#define SDHC_SRR_SWRSTCMD_WORK                (SDHC_SRR_SWRSTCMD_WORK_Val << SDHC_SRR_SWRSTCMD_Pos) /**< (SDHC_SRR) Work Position  */
#define SDHC_SRR_SWRSTCMD_RESET               (SDHC_SRR_SWRSTCMD_RESET_Val << SDHC_SRR_SWRSTCMD_Pos) /**< (SDHC_SRR) Reset Position  */
#define SDHC_SRR_SWRSTDAT_Pos                 _U_(2)                                               /**< (SDHC_SRR) Software Reset For DAT Line Position */
#define SDHC_SRR_SWRSTDAT_Msk                 (_U_(0x1) << SDHC_SRR_SWRSTDAT_Pos)                  /**< (SDHC_SRR) Software Reset For DAT Line Mask */
#define SDHC_SRR_SWRSTDAT(value)              (SDHC_SRR_SWRSTDAT_Msk & ((value) << SDHC_SRR_SWRSTDAT_Pos))
#define   SDHC_SRR_SWRSTDAT_WORK_Val          _U_(0x0)                                             /**< (SDHC_SRR) Work  */
#define   SDHC_SRR_SWRSTDAT_RESET_Val         _U_(0x1)                                             /**< (SDHC_SRR) Reset  */
#define SDHC_SRR_SWRSTDAT_WORK                (SDHC_SRR_SWRSTDAT_WORK_Val << SDHC_SRR_SWRSTDAT_Pos) /**< (SDHC_SRR) Work Position  */
#define SDHC_SRR_SWRSTDAT_RESET               (SDHC_SRR_SWRSTDAT_RESET_Val << SDHC_SRR_SWRSTDAT_Pos) /**< (SDHC_SRR) Reset Position  */
#define SDHC_SRR_Msk                          _U_(0x07)                                            /**< (SDHC_SRR) Register Mask  */


/* -------- SDHC_NISTR : (SDHC Offset: 0x30) (R/W 16) Normal Interrupt Status -------- */
#define SDHC_NISTR_RESETVALUE                 _U_(0x00)                                            /**<  (SDHC_NISTR) Normal Interrupt Status  Reset Value */

#define SDHC_NISTR_CMDC_Pos                   _U_(0)                                               /**< (SDHC_NISTR) Command Complete Position */
#define SDHC_NISTR_CMDC_Msk                   (_U_(0x1) << SDHC_NISTR_CMDC_Pos)                    /**< (SDHC_NISTR) Command Complete Mask */
#define SDHC_NISTR_CMDC(value)                (SDHC_NISTR_CMDC_Msk & ((value) << SDHC_NISTR_CMDC_Pos))
#define   SDHC_NISTR_CMDC_NO_Val              _U_(0x0)                                             /**< (SDHC_NISTR) No command complete  */
#define   SDHC_NISTR_CMDC_YES_Val             _U_(0x1)                                             /**< (SDHC_NISTR) Command complete  */
#define SDHC_NISTR_CMDC_NO                    (SDHC_NISTR_CMDC_NO_Val << SDHC_NISTR_CMDC_Pos)      /**< (SDHC_NISTR) No command complete Position  */
#define SDHC_NISTR_CMDC_YES                   (SDHC_NISTR_CMDC_YES_Val << SDHC_NISTR_CMDC_Pos)     /**< (SDHC_NISTR) Command complete Position  */
#define SDHC_NISTR_TRFC_Pos                   _U_(1)                                               /**< (SDHC_NISTR) Transfer Complete Position */
#define SDHC_NISTR_TRFC_Msk                   (_U_(0x1) << SDHC_NISTR_TRFC_Pos)                    /**< (SDHC_NISTR) Transfer Complete Mask */
#define SDHC_NISTR_TRFC(value)                (SDHC_NISTR_TRFC_Msk & ((value) << SDHC_NISTR_TRFC_Pos))
#define   SDHC_NISTR_TRFC_NO_Val              _U_(0x0)                                             /**< (SDHC_NISTR) Not complete  */
#define   SDHC_NISTR_TRFC_YES_Val             _U_(0x1)                                             /**< (SDHC_NISTR) Command execution is completed  */
#define SDHC_NISTR_TRFC_NO                    (SDHC_NISTR_TRFC_NO_Val << SDHC_NISTR_TRFC_Pos)      /**< (SDHC_NISTR) Not complete Position  */
#define SDHC_NISTR_TRFC_YES                   (SDHC_NISTR_TRFC_YES_Val << SDHC_NISTR_TRFC_Pos)     /**< (SDHC_NISTR) Command execution is completed Position  */
#define SDHC_NISTR_BLKGE_Pos                  _U_(2)                                               /**< (SDHC_NISTR) Block Gap Event Position */
#define SDHC_NISTR_BLKGE_Msk                  (_U_(0x1) << SDHC_NISTR_BLKGE_Pos)                   /**< (SDHC_NISTR) Block Gap Event Mask */
#define SDHC_NISTR_BLKGE(value)               (SDHC_NISTR_BLKGE_Msk & ((value) << SDHC_NISTR_BLKGE_Pos))
#define   SDHC_NISTR_BLKGE_NO_Val             _U_(0x0)                                             /**< (SDHC_NISTR) No Block Gap Event  */
#define   SDHC_NISTR_BLKGE_STOP_Val           _U_(0x1)                                             /**< (SDHC_NISTR) Transaction stopped at block gap  */
#define SDHC_NISTR_BLKGE_NO                   (SDHC_NISTR_BLKGE_NO_Val << SDHC_NISTR_BLKGE_Pos)    /**< (SDHC_NISTR) No Block Gap Event Position  */
#define SDHC_NISTR_BLKGE_STOP                 (SDHC_NISTR_BLKGE_STOP_Val << SDHC_NISTR_BLKGE_Pos)  /**< (SDHC_NISTR) Transaction stopped at block gap Position  */
#define SDHC_NISTR_DMAINT_Pos                 _U_(3)                                               /**< (SDHC_NISTR) DMA Interrupt Position */
#define SDHC_NISTR_DMAINT_Msk                 (_U_(0x1) << SDHC_NISTR_DMAINT_Pos)                  /**< (SDHC_NISTR) DMA Interrupt Mask */
#define SDHC_NISTR_DMAINT(value)              (SDHC_NISTR_DMAINT_Msk & ((value) << SDHC_NISTR_DMAINT_Pos))
#define   SDHC_NISTR_DMAINT_NO_Val            _U_(0x0)                                             /**< (SDHC_NISTR) No DMA Interrupt  */
#define   SDHC_NISTR_DMAINT_YES_Val           _U_(0x1)                                             /**< (SDHC_NISTR) DMA Interrupt is generated  */
#define SDHC_NISTR_DMAINT_NO                  (SDHC_NISTR_DMAINT_NO_Val << SDHC_NISTR_DMAINT_Pos)  /**< (SDHC_NISTR) No DMA Interrupt Position  */
#define SDHC_NISTR_DMAINT_YES                 (SDHC_NISTR_DMAINT_YES_Val << SDHC_NISTR_DMAINT_Pos) /**< (SDHC_NISTR) DMA Interrupt is generated Position  */
#define SDHC_NISTR_BWRRDY_Pos                 _U_(4)                                               /**< (SDHC_NISTR) Buffer Write Ready Position */
#define SDHC_NISTR_BWRRDY_Msk                 (_U_(0x1) << SDHC_NISTR_BWRRDY_Pos)                  /**< (SDHC_NISTR) Buffer Write Ready Mask */
#define SDHC_NISTR_BWRRDY(value)              (SDHC_NISTR_BWRRDY_Msk & ((value) << SDHC_NISTR_BWRRDY_Pos))
#define   SDHC_NISTR_BWRRDY_NO_Val            _U_(0x0)                                             /**< (SDHC_NISTR) Not ready to write buffer  */
#define   SDHC_NISTR_BWRRDY_YES_Val           _U_(0x1)                                             /**< (SDHC_NISTR) Ready to write buffer  */
#define SDHC_NISTR_BWRRDY_NO                  (SDHC_NISTR_BWRRDY_NO_Val << SDHC_NISTR_BWRRDY_Pos)  /**< (SDHC_NISTR) Not ready to write buffer Position  */
#define SDHC_NISTR_BWRRDY_YES                 (SDHC_NISTR_BWRRDY_YES_Val << SDHC_NISTR_BWRRDY_Pos) /**< (SDHC_NISTR) Ready to write buffer Position  */
#define SDHC_NISTR_BRDRDY_Pos                 _U_(5)                                               /**< (SDHC_NISTR) Buffer Read Ready Position */
#define SDHC_NISTR_BRDRDY_Msk                 (_U_(0x1) << SDHC_NISTR_BRDRDY_Pos)                  /**< (SDHC_NISTR) Buffer Read Ready Mask */
#define SDHC_NISTR_BRDRDY(value)              (SDHC_NISTR_BRDRDY_Msk & ((value) << SDHC_NISTR_BRDRDY_Pos))
#define   SDHC_NISTR_BRDRDY_NO_Val            _U_(0x0)                                             /**< (SDHC_NISTR) Not ready to read buffer  */
#define   SDHC_NISTR_BRDRDY_YES_Val           _U_(0x1)                                             /**< (SDHC_NISTR) Ready to read buffer  */
#define SDHC_NISTR_BRDRDY_NO                  (SDHC_NISTR_BRDRDY_NO_Val << SDHC_NISTR_BRDRDY_Pos)  /**< (SDHC_NISTR) Not ready to read buffer Position  */
#define SDHC_NISTR_BRDRDY_YES                 (SDHC_NISTR_BRDRDY_YES_Val << SDHC_NISTR_BRDRDY_Pos) /**< (SDHC_NISTR) Ready to read buffer Position  */
#define SDHC_NISTR_CINS_Pos                   _U_(6)                                               /**< (SDHC_NISTR) Card Insertion Position */
#define SDHC_NISTR_CINS_Msk                   (_U_(0x1) << SDHC_NISTR_CINS_Pos)                    /**< (SDHC_NISTR) Card Insertion Mask */
#define SDHC_NISTR_CINS(value)                (SDHC_NISTR_CINS_Msk & ((value) << SDHC_NISTR_CINS_Pos))
#define   SDHC_NISTR_CINS_NO_Val              _U_(0x0)                                             /**< (SDHC_NISTR) Card state stable or Debouncing  */
#define   SDHC_NISTR_CINS_YES_Val             _U_(0x1)                                             /**< (SDHC_NISTR) Card inserted  */
#define SDHC_NISTR_CINS_NO                    (SDHC_NISTR_CINS_NO_Val << SDHC_NISTR_CINS_Pos)      /**< (SDHC_NISTR) Card state stable or Debouncing Position  */
#define SDHC_NISTR_CINS_YES                   (SDHC_NISTR_CINS_YES_Val << SDHC_NISTR_CINS_Pos)     /**< (SDHC_NISTR) Card inserted Position  */
#define SDHC_NISTR_CREM_Pos                   _U_(7)                                               /**< (SDHC_NISTR) Card Removal Position */
#define SDHC_NISTR_CREM_Msk                   (_U_(0x1) << SDHC_NISTR_CREM_Pos)                    /**< (SDHC_NISTR) Card Removal Mask */
#define SDHC_NISTR_CREM(value)                (SDHC_NISTR_CREM_Msk & ((value) << SDHC_NISTR_CREM_Pos))
#define   SDHC_NISTR_CREM_NO_Val              _U_(0x0)                                             /**< (SDHC_NISTR) Card state stable or Debouncing  */
#define   SDHC_NISTR_CREM_YES_Val             _U_(0x1)                                             /**< (SDHC_NISTR) Card Removed  */
#define SDHC_NISTR_CREM_NO                    (SDHC_NISTR_CREM_NO_Val << SDHC_NISTR_CREM_Pos)      /**< (SDHC_NISTR) Card state stable or Debouncing Position  */
#define SDHC_NISTR_CREM_YES                   (SDHC_NISTR_CREM_YES_Val << SDHC_NISTR_CREM_Pos)     /**< (SDHC_NISTR) Card Removed Position  */
#define SDHC_NISTR_CINT_Pos                   _U_(8)                                               /**< (SDHC_NISTR) Card Interrupt Position */
#define SDHC_NISTR_CINT_Msk                   (_U_(0x1) << SDHC_NISTR_CINT_Pos)                    /**< (SDHC_NISTR) Card Interrupt Mask */
#define SDHC_NISTR_CINT(value)                (SDHC_NISTR_CINT_Msk & ((value) << SDHC_NISTR_CINT_Pos))
#define   SDHC_NISTR_CINT_NO_Val              _U_(0x0)                                             /**< (SDHC_NISTR) No Card Interrupt  */
#define   SDHC_NISTR_CINT_YES_Val             _U_(0x1)                                             /**< (SDHC_NISTR) Generate Card Interrupt  */
#define SDHC_NISTR_CINT_NO                    (SDHC_NISTR_CINT_NO_Val << SDHC_NISTR_CINT_Pos)      /**< (SDHC_NISTR) No Card Interrupt Position  */
#define SDHC_NISTR_CINT_YES                   (SDHC_NISTR_CINT_YES_Val << SDHC_NISTR_CINT_Pos)     /**< (SDHC_NISTR) Generate Card Interrupt Position  */
#define SDHC_NISTR_ERRINT_Pos                 _U_(15)                                              /**< (SDHC_NISTR) Error Interrupt Position */
#define SDHC_NISTR_ERRINT_Msk                 (_U_(0x1) << SDHC_NISTR_ERRINT_Pos)                  /**< (SDHC_NISTR) Error Interrupt Mask */
#define SDHC_NISTR_ERRINT(value)              (SDHC_NISTR_ERRINT_Msk & ((value) << SDHC_NISTR_ERRINT_Pos))
#define   SDHC_NISTR_ERRINT_NO_Val            _U_(0x0)                                             /**< (SDHC_NISTR) No Error  */
#define   SDHC_NISTR_ERRINT_YES_Val           _U_(0x1)                                             /**< (SDHC_NISTR) Error  */
#define SDHC_NISTR_ERRINT_NO                  (SDHC_NISTR_ERRINT_NO_Val << SDHC_NISTR_ERRINT_Pos)  /**< (SDHC_NISTR) No Error Position  */
#define SDHC_NISTR_ERRINT_YES                 (SDHC_NISTR_ERRINT_YES_Val << SDHC_NISTR_ERRINT_Pos) /**< (SDHC_NISTR) Error Position  */
#define SDHC_NISTR_Msk                        _U_(0x81FF)                                          /**< (SDHC_NISTR) Register Mask  */

/* EMMC mode */
#define SDHC_NISTR_EMMC_BOOTAR_Pos            _U_(14)                                              /**< (SDHC_NISTR) Boot Acknowledge Received Position */
#define SDHC_NISTR_EMMC_BOOTAR_Msk            (_U_(0x1) << SDHC_NISTR_EMMC_BOOTAR_Pos)             /**< (SDHC_NISTR) Boot Acknowledge Received Mask */
#define SDHC_NISTR_EMMC_BOOTAR(value)         (SDHC_NISTR_EMMC_BOOTAR_Msk & ((value) << SDHC_NISTR_EMMC_BOOTAR_Pos))
#define SDHC_NISTR_EMMC_Msk                   _U_(0x4000)                                           /**< (SDHC_NISTR_EMMC) Register Mask  */


/* -------- SDHC_EISTR : (SDHC Offset: 0x32) (R/W 16) Error Interrupt Status -------- */
#define SDHC_EISTR_RESETVALUE                 _U_(0x00)                                            /**<  (SDHC_EISTR) Error Interrupt Status  Reset Value */

#define SDHC_EISTR_CMDTEO_Pos                 _U_(0)                                               /**< (SDHC_EISTR) Command Timeout Error Position */
#define SDHC_EISTR_CMDTEO_Msk                 (_U_(0x1) << SDHC_EISTR_CMDTEO_Pos)                  /**< (SDHC_EISTR) Command Timeout Error Mask */
#define SDHC_EISTR_CMDTEO(value)              (SDHC_EISTR_CMDTEO_Msk & ((value) << SDHC_EISTR_CMDTEO_Pos))
#define   SDHC_EISTR_CMDTEO_NO_Val            _U_(0x0)                                             /**< (SDHC_EISTR) No Error  */
#define   SDHC_EISTR_CMDTEO_YES_Val           _U_(0x1)                                             /**< (SDHC_EISTR) Timeout  */
#define SDHC_EISTR_CMDTEO_NO                  (SDHC_EISTR_CMDTEO_NO_Val << SDHC_EISTR_CMDTEO_Pos)  /**< (SDHC_EISTR) No Error Position  */
#define SDHC_EISTR_CMDTEO_YES                 (SDHC_EISTR_CMDTEO_YES_Val << SDHC_EISTR_CMDTEO_Pos) /**< (SDHC_EISTR) Timeout Position  */
#define SDHC_EISTR_CMDCRC_Pos                 _U_(1)                                               /**< (SDHC_EISTR) Command CRC Error Position */
#define SDHC_EISTR_CMDCRC_Msk                 (_U_(0x1) << SDHC_EISTR_CMDCRC_Pos)                  /**< (SDHC_EISTR) Command CRC Error Mask */
#define SDHC_EISTR_CMDCRC(value)              (SDHC_EISTR_CMDCRC_Msk & ((value) << SDHC_EISTR_CMDCRC_Pos))
#define   SDHC_EISTR_CMDCRC_NO_Val            _U_(0x0)                                             /**< (SDHC_EISTR) No Error  */
#define   SDHC_EISTR_CMDCRC_YES_Val           _U_(0x1)                                             /**< (SDHC_EISTR) CRC Error Generated  */
#define SDHC_EISTR_CMDCRC_NO                  (SDHC_EISTR_CMDCRC_NO_Val << SDHC_EISTR_CMDCRC_Pos)  /**< (SDHC_EISTR) No Error Position  */
#define SDHC_EISTR_CMDCRC_YES                 (SDHC_EISTR_CMDCRC_YES_Val << SDHC_EISTR_CMDCRC_Pos) /**< (SDHC_EISTR) CRC Error Generated Position  */
#define SDHC_EISTR_CMDEND_Pos                 _U_(2)                                               /**< (SDHC_EISTR) Command End Bit Error Position */
#define SDHC_EISTR_CMDEND_Msk                 (_U_(0x1) << SDHC_EISTR_CMDEND_Pos)                  /**< (SDHC_EISTR) Command End Bit Error Mask */
#define SDHC_EISTR_CMDEND(value)              (SDHC_EISTR_CMDEND_Msk & ((value) << SDHC_EISTR_CMDEND_Pos))
#define   SDHC_EISTR_CMDEND_NO_Val            _U_(0x0)                                             /**< (SDHC_EISTR) No error  */
#define   SDHC_EISTR_CMDEND_YES_Val           _U_(0x1)                                             /**< (SDHC_EISTR) End Bit Error Generated  */
#define SDHC_EISTR_CMDEND_NO                  (SDHC_EISTR_CMDEND_NO_Val << SDHC_EISTR_CMDEND_Pos)  /**< (SDHC_EISTR) No error Position  */
#define SDHC_EISTR_CMDEND_YES                 (SDHC_EISTR_CMDEND_YES_Val << SDHC_EISTR_CMDEND_Pos) /**< (SDHC_EISTR) End Bit Error Generated Position  */
#define SDHC_EISTR_CMDIDX_Pos                 _U_(3)                                               /**< (SDHC_EISTR) Command Index Error Position */
#define SDHC_EISTR_CMDIDX_Msk                 (_U_(0x1) << SDHC_EISTR_CMDIDX_Pos)                  /**< (SDHC_EISTR) Command Index Error Mask */
#define SDHC_EISTR_CMDIDX(value)              (SDHC_EISTR_CMDIDX_Msk & ((value) << SDHC_EISTR_CMDIDX_Pos))
#define   SDHC_EISTR_CMDIDX_NO_Val            _U_(0x0)                                             /**< (SDHC_EISTR) No Error  */
#define   SDHC_EISTR_CMDIDX_YES_Val           _U_(0x1)                                             /**< (SDHC_EISTR) Error  */
#define SDHC_EISTR_CMDIDX_NO                  (SDHC_EISTR_CMDIDX_NO_Val << SDHC_EISTR_CMDIDX_Pos)  /**< (SDHC_EISTR) No Error Position  */
#define SDHC_EISTR_CMDIDX_YES                 (SDHC_EISTR_CMDIDX_YES_Val << SDHC_EISTR_CMDIDX_Pos) /**< (SDHC_EISTR) Error Position  */
#define SDHC_EISTR_DATTEO_Pos                 _U_(4)                                               /**< (SDHC_EISTR) Data Timeout Error Position */
#define SDHC_EISTR_DATTEO_Msk                 (_U_(0x1) << SDHC_EISTR_DATTEO_Pos)                  /**< (SDHC_EISTR) Data Timeout Error Mask */
#define SDHC_EISTR_DATTEO(value)              (SDHC_EISTR_DATTEO_Msk & ((value) << SDHC_EISTR_DATTEO_Pos))
#define   SDHC_EISTR_DATTEO_NO_Val            _U_(0x0)                                             /**< (SDHC_EISTR) No Error  */
#define   SDHC_EISTR_DATTEO_YES_Val           _U_(0x1)                                             /**< (SDHC_EISTR) Timeout  */
#define SDHC_EISTR_DATTEO_NO                  (SDHC_EISTR_DATTEO_NO_Val << SDHC_EISTR_DATTEO_Pos)  /**< (SDHC_EISTR) No Error Position  */
#define SDHC_EISTR_DATTEO_YES                 (SDHC_EISTR_DATTEO_YES_Val << SDHC_EISTR_DATTEO_Pos) /**< (SDHC_EISTR) Timeout Position  */
#define SDHC_EISTR_DATCRC_Pos                 _U_(5)                                               /**< (SDHC_EISTR) Data CRC Error Position */
#define SDHC_EISTR_DATCRC_Msk                 (_U_(0x1) << SDHC_EISTR_DATCRC_Pos)                  /**< (SDHC_EISTR) Data CRC Error Mask */
#define SDHC_EISTR_DATCRC(value)              (SDHC_EISTR_DATCRC_Msk & ((value) << SDHC_EISTR_DATCRC_Pos))
#define   SDHC_EISTR_DATCRC_NO_Val            _U_(0x0)                                             /**< (SDHC_EISTR) No Error  */
#define   SDHC_EISTR_DATCRC_YES_Val           _U_(0x1)                                             /**< (SDHC_EISTR) Error  */
#define SDHC_EISTR_DATCRC_NO                  (SDHC_EISTR_DATCRC_NO_Val << SDHC_EISTR_DATCRC_Pos)  /**< (SDHC_EISTR) No Error Position  */
#define SDHC_EISTR_DATCRC_YES                 (SDHC_EISTR_DATCRC_YES_Val << SDHC_EISTR_DATCRC_Pos) /**< (SDHC_EISTR) Error Position  */
#define SDHC_EISTR_DATEND_Pos                 _U_(6)                                               /**< (SDHC_EISTR) Data End Bit Error Position */
#define SDHC_EISTR_DATEND_Msk                 (_U_(0x1) << SDHC_EISTR_DATEND_Pos)                  /**< (SDHC_EISTR) Data End Bit Error Mask */
#define SDHC_EISTR_DATEND(value)              (SDHC_EISTR_DATEND_Msk & ((value) << SDHC_EISTR_DATEND_Pos))
#define   SDHC_EISTR_DATEND_NO_Val            _U_(0x0)                                             /**< (SDHC_EISTR) No Error  */
#define   SDHC_EISTR_DATEND_YES_Val           _U_(0x1)                                             /**< (SDHC_EISTR) Error  */
#define SDHC_EISTR_DATEND_NO                  (SDHC_EISTR_DATEND_NO_Val << SDHC_EISTR_DATEND_Pos)  /**< (SDHC_EISTR) No Error Position  */
#define SDHC_EISTR_DATEND_YES                 (SDHC_EISTR_DATEND_YES_Val << SDHC_EISTR_DATEND_Pos) /**< (SDHC_EISTR) Error Position  */
#define SDHC_EISTR_CURLIM_Pos                 _U_(7)                                               /**< (SDHC_EISTR) Current Limit Error Position */
#define SDHC_EISTR_CURLIM_Msk                 (_U_(0x1) << SDHC_EISTR_CURLIM_Pos)                  /**< (SDHC_EISTR) Current Limit Error Mask */
#define SDHC_EISTR_CURLIM(value)              (SDHC_EISTR_CURLIM_Msk & ((value) << SDHC_EISTR_CURLIM_Pos))
#define   SDHC_EISTR_CURLIM_NO_Val            _U_(0x0)                                             /**< (SDHC_EISTR) No Error  */
#define   SDHC_EISTR_CURLIM_YES_Val           _U_(0x1)                                             /**< (SDHC_EISTR) Power Fail  */
#define SDHC_EISTR_CURLIM_NO                  (SDHC_EISTR_CURLIM_NO_Val << SDHC_EISTR_CURLIM_Pos)  /**< (SDHC_EISTR) No Error Position  */
#define SDHC_EISTR_CURLIM_YES                 (SDHC_EISTR_CURLIM_YES_Val << SDHC_EISTR_CURLIM_Pos) /**< (SDHC_EISTR) Power Fail Position  */
#define SDHC_EISTR_ACMD_Pos                   _U_(8)                                               /**< (SDHC_EISTR) Auto CMD Error Position */
#define SDHC_EISTR_ACMD_Msk                   (_U_(0x1) << SDHC_EISTR_ACMD_Pos)                    /**< (SDHC_EISTR) Auto CMD Error Mask */
#define SDHC_EISTR_ACMD(value)                (SDHC_EISTR_ACMD_Msk & ((value) << SDHC_EISTR_ACMD_Pos))
#define   SDHC_EISTR_ACMD_NO_Val              _U_(0x0)                                             /**< (SDHC_EISTR) No Error  */
#define   SDHC_EISTR_ACMD_YES_Val             _U_(0x1)                                             /**< (SDHC_EISTR) Error  */
#define SDHC_EISTR_ACMD_NO                    (SDHC_EISTR_ACMD_NO_Val << SDHC_EISTR_ACMD_Pos)      /**< (SDHC_EISTR) No Error Position  */
#define SDHC_EISTR_ACMD_YES                   (SDHC_EISTR_ACMD_YES_Val << SDHC_EISTR_ACMD_Pos)     /**< (SDHC_EISTR) Error Position  */
#define SDHC_EISTR_ADMA_Pos                   _U_(9)                                               /**< (SDHC_EISTR) ADMA Error Position */
#define SDHC_EISTR_ADMA_Msk                   (_U_(0x1) << SDHC_EISTR_ADMA_Pos)                    /**< (SDHC_EISTR) ADMA Error Mask */
#define SDHC_EISTR_ADMA(value)                (SDHC_EISTR_ADMA_Msk & ((value) << SDHC_EISTR_ADMA_Pos))
#define   SDHC_EISTR_ADMA_NO_Val              _U_(0x0)                                             /**< (SDHC_EISTR) No Error  */
#define   SDHC_EISTR_ADMA_YES_Val             _U_(0x1)                                             /**< (SDHC_EISTR) Error  */
#define SDHC_EISTR_ADMA_NO                    (SDHC_EISTR_ADMA_NO_Val << SDHC_EISTR_ADMA_Pos)      /**< (SDHC_EISTR) No Error Position  */
#define SDHC_EISTR_ADMA_YES                   (SDHC_EISTR_ADMA_YES_Val << SDHC_EISTR_ADMA_Pos)     /**< (SDHC_EISTR) Error Position  */
#define SDHC_EISTR_Msk                        _U_(0x03FF)                                          /**< (SDHC_EISTR) Register Mask  */

/* EMMC mode */
#define SDHC_EISTR_EMMC_BOOTAE_Pos            _U_(12)                                              /**< (SDHC_EISTR) Boot Acknowledge Error Position */
#define SDHC_EISTR_EMMC_BOOTAE_Msk            (_U_(0x1) << SDHC_EISTR_EMMC_BOOTAE_Pos)             /**< (SDHC_EISTR) Boot Acknowledge Error Mask */
#define SDHC_EISTR_EMMC_BOOTAE(value)         (SDHC_EISTR_EMMC_BOOTAE_Msk & ((value) << SDHC_EISTR_EMMC_BOOTAE_Pos))
#define   SDHC_EISTR_EMMC_BOOTAE_0_Val        _U_(0x0)                                             /**< (SDHC_EISTR) FIFO contains at least one byte  */
#define   SDHC_EISTR_EMMC_BOOTAE_1_Val        _U_(0x1)                                             /**< (SDHC_EISTR) FIFO is empty  */
#define SDHC_EISTR_EMMC_BOOTAE_0              (SDHC_EISTR_EMMC_BOOTAE_0_Val << SDHC_EISTR_EMMC_BOOTAE_Pos) /**< (SDHC_EISTR) FIFO contains at least one byte Position  */
#define SDHC_EISTR_EMMC_BOOTAE_1              (SDHC_EISTR_EMMC_BOOTAE_1_Val << SDHC_EISTR_EMMC_BOOTAE_Pos) /**< (SDHC_EISTR) FIFO is empty Position  */
#define SDHC_EISTR_EMMC_Msk                   _U_(0x1000)                                           /**< (SDHC_EISTR_EMMC) Register Mask  */


/* -------- SDHC_NISTER : (SDHC Offset: 0x34) (R/W 16) Normal Interrupt Status Enable -------- */
#define SDHC_NISTER_RESETVALUE                _U_(0x00)                                            /**<  (SDHC_NISTER) Normal Interrupt Status Enable  Reset Value */

#define SDHC_NISTER_CMDC_Pos                  _U_(0)                                               /**< (SDHC_NISTER) Command Complete Status Enable Position */
#define SDHC_NISTER_CMDC_Msk                  (_U_(0x1) << SDHC_NISTER_CMDC_Pos)                   /**< (SDHC_NISTER) Command Complete Status Enable Mask */
#define SDHC_NISTER_CMDC(value)               (SDHC_NISTER_CMDC_Msk & ((value) << SDHC_NISTER_CMDC_Pos))
#define   SDHC_NISTER_CMDC_MASKED_Val         _U_(0x0)                                             /**< (SDHC_NISTER) Masked  */
#define   SDHC_NISTER_CMDC_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_NISTER) Enabled  */
#define SDHC_NISTER_CMDC_MASKED               (SDHC_NISTER_CMDC_MASKED_Val << SDHC_NISTER_CMDC_Pos) /**< (SDHC_NISTER) Masked Position  */
#define SDHC_NISTER_CMDC_ENABLED              (SDHC_NISTER_CMDC_ENABLED_Val << SDHC_NISTER_CMDC_Pos) /**< (SDHC_NISTER) Enabled Position  */
#define SDHC_NISTER_TRFC_Pos                  _U_(1)                                               /**< (SDHC_NISTER) Transfer Complete Status Enable Position */
#define SDHC_NISTER_TRFC_Msk                  (_U_(0x1) << SDHC_NISTER_TRFC_Pos)                   /**< (SDHC_NISTER) Transfer Complete Status Enable Mask */
#define SDHC_NISTER_TRFC(value)               (SDHC_NISTER_TRFC_Msk & ((value) << SDHC_NISTER_TRFC_Pos))
#define   SDHC_NISTER_TRFC_MASKED_Val         _U_(0x0)                                             /**< (SDHC_NISTER) Masked  */
#define   SDHC_NISTER_TRFC_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_NISTER) Enabled  */
#define SDHC_NISTER_TRFC_MASKED               (SDHC_NISTER_TRFC_MASKED_Val << SDHC_NISTER_TRFC_Pos) /**< (SDHC_NISTER) Masked Position  */
#define SDHC_NISTER_TRFC_ENABLED              (SDHC_NISTER_TRFC_ENABLED_Val << SDHC_NISTER_TRFC_Pos) /**< (SDHC_NISTER) Enabled Position  */
#define SDHC_NISTER_BLKGE_Pos                 _U_(2)                                               /**< (SDHC_NISTER) Block Gap Event Status Enable Position */
#define SDHC_NISTER_BLKGE_Msk                 (_U_(0x1) << SDHC_NISTER_BLKGE_Pos)                  /**< (SDHC_NISTER) Block Gap Event Status Enable Mask */
#define SDHC_NISTER_BLKGE(value)              (SDHC_NISTER_BLKGE_Msk & ((value) << SDHC_NISTER_BLKGE_Pos))
#define   SDHC_NISTER_BLKGE_MASKED_Val        _U_(0x0)                                             /**< (SDHC_NISTER) Masked  */
#define   SDHC_NISTER_BLKGE_ENABLED_Val       _U_(0x1)                                             /**< (SDHC_NISTER) Enabled  */
#define SDHC_NISTER_BLKGE_MASKED              (SDHC_NISTER_BLKGE_MASKED_Val << SDHC_NISTER_BLKGE_Pos) /**< (SDHC_NISTER) Masked Position  */
#define SDHC_NISTER_BLKGE_ENABLED             (SDHC_NISTER_BLKGE_ENABLED_Val << SDHC_NISTER_BLKGE_Pos) /**< (SDHC_NISTER) Enabled Position  */
#define SDHC_NISTER_DMAINT_Pos                _U_(3)                                               /**< (SDHC_NISTER) DMA Interrupt Status Enable Position */
#define SDHC_NISTER_DMAINT_Msk                (_U_(0x1) << SDHC_NISTER_DMAINT_Pos)                 /**< (SDHC_NISTER) DMA Interrupt Status Enable Mask */
#define SDHC_NISTER_DMAINT(value)             (SDHC_NISTER_DMAINT_Msk & ((value) << SDHC_NISTER_DMAINT_Pos))
#define   SDHC_NISTER_DMAINT_MASKED_Val       _U_(0x0)                                             /**< (SDHC_NISTER) Masked  */
#define   SDHC_NISTER_DMAINT_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_NISTER) Enabled  */
#define SDHC_NISTER_DMAINT_MASKED             (SDHC_NISTER_DMAINT_MASKED_Val << SDHC_NISTER_DMAINT_Pos) /**< (SDHC_NISTER) Masked Position  */
#define SDHC_NISTER_DMAINT_ENABLED            (SDHC_NISTER_DMAINT_ENABLED_Val << SDHC_NISTER_DMAINT_Pos) /**< (SDHC_NISTER) Enabled Position  */
#define SDHC_NISTER_BWRRDY_Pos                _U_(4)                                               /**< (SDHC_NISTER) Buffer Write Ready Status Enable Position */
#define SDHC_NISTER_BWRRDY_Msk                (_U_(0x1) << SDHC_NISTER_BWRRDY_Pos)                 /**< (SDHC_NISTER) Buffer Write Ready Status Enable Mask */
#define SDHC_NISTER_BWRRDY(value)             (SDHC_NISTER_BWRRDY_Msk & ((value) << SDHC_NISTER_BWRRDY_Pos))
#define   SDHC_NISTER_BWRRDY_MASKED_Val       _U_(0x0)                                             /**< (SDHC_NISTER) Masked  */
#define   SDHC_NISTER_BWRRDY_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_NISTER) Enabled  */
#define SDHC_NISTER_BWRRDY_MASKED             (SDHC_NISTER_BWRRDY_MASKED_Val << SDHC_NISTER_BWRRDY_Pos) /**< (SDHC_NISTER) Masked Position  */
#define SDHC_NISTER_BWRRDY_ENABLED            (SDHC_NISTER_BWRRDY_ENABLED_Val << SDHC_NISTER_BWRRDY_Pos) /**< (SDHC_NISTER) Enabled Position  */
#define SDHC_NISTER_BRDRDY_Pos                _U_(5)                                               /**< (SDHC_NISTER) Buffer Read Ready Status Enable Position */
#define SDHC_NISTER_BRDRDY_Msk                (_U_(0x1) << SDHC_NISTER_BRDRDY_Pos)                 /**< (SDHC_NISTER) Buffer Read Ready Status Enable Mask */
#define SDHC_NISTER_BRDRDY(value)             (SDHC_NISTER_BRDRDY_Msk & ((value) << SDHC_NISTER_BRDRDY_Pos))
#define   SDHC_NISTER_BRDRDY_MASKED_Val       _U_(0x0)                                             /**< (SDHC_NISTER) Masked  */
#define   SDHC_NISTER_BRDRDY_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_NISTER) Enabled  */
#define SDHC_NISTER_BRDRDY_MASKED             (SDHC_NISTER_BRDRDY_MASKED_Val << SDHC_NISTER_BRDRDY_Pos) /**< (SDHC_NISTER) Masked Position  */
#define SDHC_NISTER_BRDRDY_ENABLED            (SDHC_NISTER_BRDRDY_ENABLED_Val << SDHC_NISTER_BRDRDY_Pos) /**< (SDHC_NISTER) Enabled Position  */
#define SDHC_NISTER_CINS_Pos                  _U_(6)                                               /**< (SDHC_NISTER) Card Insertion Status Enable Position */
#define SDHC_NISTER_CINS_Msk                  (_U_(0x1) << SDHC_NISTER_CINS_Pos)                   /**< (SDHC_NISTER) Card Insertion Status Enable Mask */
#define SDHC_NISTER_CINS(value)               (SDHC_NISTER_CINS_Msk & ((value) << SDHC_NISTER_CINS_Pos))
#define   SDHC_NISTER_CINS_MASKED_Val         _U_(0x0)                                             /**< (SDHC_NISTER) Masked  */
#define   SDHC_NISTER_CINS_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_NISTER) Enabled  */
#define SDHC_NISTER_CINS_MASKED               (SDHC_NISTER_CINS_MASKED_Val << SDHC_NISTER_CINS_Pos) /**< (SDHC_NISTER) Masked Position  */
#define SDHC_NISTER_CINS_ENABLED              (SDHC_NISTER_CINS_ENABLED_Val << SDHC_NISTER_CINS_Pos) /**< (SDHC_NISTER) Enabled Position  */
#define SDHC_NISTER_CREM_Pos                  _U_(7)                                               /**< (SDHC_NISTER) Card Removal Status Enable Position */
#define SDHC_NISTER_CREM_Msk                  (_U_(0x1) << SDHC_NISTER_CREM_Pos)                   /**< (SDHC_NISTER) Card Removal Status Enable Mask */
#define SDHC_NISTER_CREM(value)               (SDHC_NISTER_CREM_Msk & ((value) << SDHC_NISTER_CREM_Pos))
#define   SDHC_NISTER_CREM_MASKED_Val         _U_(0x0)                                             /**< (SDHC_NISTER) Masked  */
#define   SDHC_NISTER_CREM_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_NISTER) Enabled  */
#define SDHC_NISTER_CREM_MASKED               (SDHC_NISTER_CREM_MASKED_Val << SDHC_NISTER_CREM_Pos) /**< (SDHC_NISTER) Masked Position  */
#define SDHC_NISTER_CREM_ENABLED              (SDHC_NISTER_CREM_ENABLED_Val << SDHC_NISTER_CREM_Pos) /**< (SDHC_NISTER) Enabled Position  */
#define SDHC_NISTER_CINT_Pos                  _U_(8)                                               /**< (SDHC_NISTER) Card Interrupt Status Enable Position */
#define SDHC_NISTER_CINT_Msk                  (_U_(0x1) << SDHC_NISTER_CINT_Pos)                   /**< (SDHC_NISTER) Card Interrupt Status Enable Mask */
#define SDHC_NISTER_CINT(value)               (SDHC_NISTER_CINT_Msk & ((value) << SDHC_NISTER_CINT_Pos))
#define   SDHC_NISTER_CINT_MASKED_Val         _U_(0x0)                                             /**< (SDHC_NISTER) Masked  */
#define   SDHC_NISTER_CINT_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_NISTER) Enabled  */
#define SDHC_NISTER_CINT_MASKED               (SDHC_NISTER_CINT_MASKED_Val << SDHC_NISTER_CINT_Pos) /**< (SDHC_NISTER) Masked Position  */
#define SDHC_NISTER_CINT_ENABLED              (SDHC_NISTER_CINT_ENABLED_Val << SDHC_NISTER_CINT_Pos) /**< (SDHC_NISTER) Enabled Position  */
#define SDHC_NISTER_Msk                       _U_(0x01FF)                                          /**< (SDHC_NISTER) Register Mask  */

/* EMMC mode */
#define SDHC_NISTER_EMMC_BOOTAR_Pos           _U_(14)                                              /**< (SDHC_NISTER) Boot Acknowledge Received Status Enable Position */
#define SDHC_NISTER_EMMC_BOOTAR_Msk           (_U_(0x1) << SDHC_NISTER_EMMC_BOOTAR_Pos)            /**< (SDHC_NISTER) Boot Acknowledge Received Status Enable Mask */
#define SDHC_NISTER_EMMC_BOOTAR(value)        (SDHC_NISTER_EMMC_BOOTAR_Msk & ((value) << SDHC_NISTER_EMMC_BOOTAR_Pos))
#define SDHC_NISTER_EMMC_Msk                  _U_(0x4000)                                           /**< (SDHC_NISTER_EMMC) Register Mask  */


/* -------- SDHC_EISTER : (SDHC Offset: 0x36) (R/W 16) Error Interrupt Status Enable -------- */
#define SDHC_EISTER_RESETVALUE                _U_(0x00)                                            /**<  (SDHC_EISTER) Error Interrupt Status Enable  Reset Value */

#define SDHC_EISTER_CMDTEO_Pos                _U_(0)                                               /**< (SDHC_EISTER) Command Timeout Error Status Enable Position */
#define SDHC_EISTER_CMDTEO_Msk                (_U_(0x1) << SDHC_EISTER_CMDTEO_Pos)                 /**< (SDHC_EISTER) Command Timeout Error Status Enable Mask */
#define SDHC_EISTER_CMDTEO(value)             (SDHC_EISTER_CMDTEO_Msk & ((value) << SDHC_EISTER_CMDTEO_Pos))
#define   SDHC_EISTER_CMDTEO_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISTER) Masked  */
#define   SDHC_EISTER_CMDTEO_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISTER) Enabled  */
#define SDHC_EISTER_CMDTEO_MASKED             (SDHC_EISTER_CMDTEO_MASKED_Val << SDHC_EISTER_CMDTEO_Pos) /**< (SDHC_EISTER) Masked Position  */
#define SDHC_EISTER_CMDTEO_ENABLED            (SDHC_EISTER_CMDTEO_ENABLED_Val << SDHC_EISTER_CMDTEO_Pos) /**< (SDHC_EISTER) Enabled Position  */
#define SDHC_EISTER_CMDCRC_Pos                _U_(1)                                               /**< (SDHC_EISTER) Command CRC Error Status Enable Position */
#define SDHC_EISTER_CMDCRC_Msk                (_U_(0x1) << SDHC_EISTER_CMDCRC_Pos)                 /**< (SDHC_EISTER) Command CRC Error Status Enable Mask */
#define SDHC_EISTER_CMDCRC(value)             (SDHC_EISTER_CMDCRC_Msk & ((value) << SDHC_EISTER_CMDCRC_Pos))
#define   SDHC_EISTER_CMDCRC_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISTER) Masked  */
#define   SDHC_EISTER_CMDCRC_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISTER) Enabled  */
#define SDHC_EISTER_CMDCRC_MASKED             (SDHC_EISTER_CMDCRC_MASKED_Val << SDHC_EISTER_CMDCRC_Pos) /**< (SDHC_EISTER) Masked Position  */
#define SDHC_EISTER_CMDCRC_ENABLED            (SDHC_EISTER_CMDCRC_ENABLED_Val << SDHC_EISTER_CMDCRC_Pos) /**< (SDHC_EISTER) Enabled Position  */
#define SDHC_EISTER_CMDEND_Pos                _U_(2)                                               /**< (SDHC_EISTER) Command End Bit Error Status Enable Position */
#define SDHC_EISTER_CMDEND_Msk                (_U_(0x1) << SDHC_EISTER_CMDEND_Pos)                 /**< (SDHC_EISTER) Command End Bit Error Status Enable Mask */
#define SDHC_EISTER_CMDEND(value)             (SDHC_EISTER_CMDEND_Msk & ((value) << SDHC_EISTER_CMDEND_Pos))
#define   SDHC_EISTER_CMDEND_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISTER) Masked  */
#define   SDHC_EISTER_CMDEND_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISTER) Enabled  */
#define SDHC_EISTER_CMDEND_MASKED             (SDHC_EISTER_CMDEND_MASKED_Val << SDHC_EISTER_CMDEND_Pos) /**< (SDHC_EISTER) Masked Position  */
#define SDHC_EISTER_CMDEND_ENABLED            (SDHC_EISTER_CMDEND_ENABLED_Val << SDHC_EISTER_CMDEND_Pos) /**< (SDHC_EISTER) Enabled Position  */
#define SDHC_EISTER_CMDIDX_Pos                _U_(3)                                               /**< (SDHC_EISTER) Command Index Error Status Enable Position */
#define SDHC_EISTER_CMDIDX_Msk                (_U_(0x1) << SDHC_EISTER_CMDIDX_Pos)                 /**< (SDHC_EISTER) Command Index Error Status Enable Mask */
#define SDHC_EISTER_CMDIDX(value)             (SDHC_EISTER_CMDIDX_Msk & ((value) << SDHC_EISTER_CMDIDX_Pos))
#define   SDHC_EISTER_CMDIDX_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISTER) Masked  */
#define   SDHC_EISTER_CMDIDX_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISTER) Enabled  */
#define SDHC_EISTER_CMDIDX_MASKED             (SDHC_EISTER_CMDIDX_MASKED_Val << SDHC_EISTER_CMDIDX_Pos) /**< (SDHC_EISTER) Masked Position  */
#define SDHC_EISTER_CMDIDX_ENABLED            (SDHC_EISTER_CMDIDX_ENABLED_Val << SDHC_EISTER_CMDIDX_Pos) /**< (SDHC_EISTER) Enabled Position  */
#define SDHC_EISTER_DATTEO_Pos                _U_(4)                                               /**< (SDHC_EISTER) Data Timeout Error Status Enable Position */
#define SDHC_EISTER_DATTEO_Msk                (_U_(0x1) << SDHC_EISTER_DATTEO_Pos)                 /**< (SDHC_EISTER) Data Timeout Error Status Enable Mask */
#define SDHC_EISTER_DATTEO(value)             (SDHC_EISTER_DATTEO_Msk & ((value) << SDHC_EISTER_DATTEO_Pos))
#define   SDHC_EISTER_DATTEO_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISTER) Masked  */
#define   SDHC_EISTER_DATTEO_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISTER) Enabled  */
#define SDHC_EISTER_DATTEO_MASKED             (SDHC_EISTER_DATTEO_MASKED_Val << SDHC_EISTER_DATTEO_Pos) /**< (SDHC_EISTER) Masked Position  */
#define SDHC_EISTER_DATTEO_ENABLED            (SDHC_EISTER_DATTEO_ENABLED_Val << SDHC_EISTER_DATTEO_Pos) /**< (SDHC_EISTER) Enabled Position  */
#define SDHC_EISTER_DATCRC_Pos                _U_(5)                                               /**< (SDHC_EISTER) Data CRC Error Status Enable Position */
#define SDHC_EISTER_DATCRC_Msk                (_U_(0x1) << SDHC_EISTER_DATCRC_Pos)                 /**< (SDHC_EISTER) Data CRC Error Status Enable Mask */
#define SDHC_EISTER_DATCRC(value)             (SDHC_EISTER_DATCRC_Msk & ((value) << SDHC_EISTER_DATCRC_Pos))
#define   SDHC_EISTER_DATCRC_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISTER) Masked  */
#define   SDHC_EISTER_DATCRC_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISTER) Enabled  */
#define SDHC_EISTER_DATCRC_MASKED             (SDHC_EISTER_DATCRC_MASKED_Val << SDHC_EISTER_DATCRC_Pos) /**< (SDHC_EISTER) Masked Position  */
#define SDHC_EISTER_DATCRC_ENABLED            (SDHC_EISTER_DATCRC_ENABLED_Val << SDHC_EISTER_DATCRC_Pos) /**< (SDHC_EISTER) Enabled Position  */
#define SDHC_EISTER_DATEND_Pos                _U_(6)                                               /**< (SDHC_EISTER) Data End Bit Error Status Enable Position */
#define SDHC_EISTER_DATEND_Msk                (_U_(0x1) << SDHC_EISTER_DATEND_Pos)                 /**< (SDHC_EISTER) Data End Bit Error Status Enable Mask */
#define SDHC_EISTER_DATEND(value)             (SDHC_EISTER_DATEND_Msk & ((value) << SDHC_EISTER_DATEND_Pos))
#define   SDHC_EISTER_DATEND_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISTER) Masked  */
#define   SDHC_EISTER_DATEND_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISTER) Enabled  */
#define SDHC_EISTER_DATEND_MASKED             (SDHC_EISTER_DATEND_MASKED_Val << SDHC_EISTER_DATEND_Pos) /**< (SDHC_EISTER) Masked Position  */
#define SDHC_EISTER_DATEND_ENABLED            (SDHC_EISTER_DATEND_ENABLED_Val << SDHC_EISTER_DATEND_Pos) /**< (SDHC_EISTER) Enabled Position  */
#define SDHC_EISTER_CURLIM_Pos                _U_(7)                                               /**< (SDHC_EISTER) Current Limit Error Status Enable Position */
#define SDHC_EISTER_CURLIM_Msk                (_U_(0x1) << SDHC_EISTER_CURLIM_Pos)                 /**< (SDHC_EISTER) Current Limit Error Status Enable Mask */
#define SDHC_EISTER_CURLIM(value)             (SDHC_EISTER_CURLIM_Msk & ((value) << SDHC_EISTER_CURLIM_Pos))
#define   SDHC_EISTER_CURLIM_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISTER) Masked  */
#define   SDHC_EISTER_CURLIM_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISTER) Enabled  */
#define SDHC_EISTER_CURLIM_MASKED             (SDHC_EISTER_CURLIM_MASKED_Val << SDHC_EISTER_CURLIM_Pos) /**< (SDHC_EISTER) Masked Position  */
#define SDHC_EISTER_CURLIM_ENABLED            (SDHC_EISTER_CURLIM_ENABLED_Val << SDHC_EISTER_CURLIM_Pos) /**< (SDHC_EISTER) Enabled Position  */
#define SDHC_EISTER_ACMD_Pos                  _U_(8)                                               /**< (SDHC_EISTER) Auto CMD Error Status Enable Position */
#define SDHC_EISTER_ACMD_Msk                  (_U_(0x1) << SDHC_EISTER_ACMD_Pos)                   /**< (SDHC_EISTER) Auto CMD Error Status Enable Mask */
#define SDHC_EISTER_ACMD(value)               (SDHC_EISTER_ACMD_Msk & ((value) << SDHC_EISTER_ACMD_Pos))
#define   SDHC_EISTER_ACMD_MASKED_Val         _U_(0x0)                                             /**< (SDHC_EISTER) Masked  */
#define   SDHC_EISTER_ACMD_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_EISTER) Enabled  */
#define SDHC_EISTER_ACMD_MASKED               (SDHC_EISTER_ACMD_MASKED_Val << SDHC_EISTER_ACMD_Pos) /**< (SDHC_EISTER) Masked Position  */
#define SDHC_EISTER_ACMD_ENABLED              (SDHC_EISTER_ACMD_ENABLED_Val << SDHC_EISTER_ACMD_Pos) /**< (SDHC_EISTER) Enabled Position  */
#define SDHC_EISTER_ADMA_Pos                  _U_(9)                                               /**< (SDHC_EISTER) ADMA Error Status Enable Position */
#define SDHC_EISTER_ADMA_Msk                  (_U_(0x1) << SDHC_EISTER_ADMA_Pos)                   /**< (SDHC_EISTER) ADMA Error Status Enable Mask */
#define SDHC_EISTER_ADMA(value)               (SDHC_EISTER_ADMA_Msk & ((value) << SDHC_EISTER_ADMA_Pos))
#define   SDHC_EISTER_ADMA_MASKED_Val         _U_(0x0)                                             /**< (SDHC_EISTER) Masked  */
#define   SDHC_EISTER_ADMA_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_EISTER) Enabled  */
#define SDHC_EISTER_ADMA_MASKED               (SDHC_EISTER_ADMA_MASKED_Val << SDHC_EISTER_ADMA_Pos) /**< (SDHC_EISTER) Masked Position  */
#define SDHC_EISTER_ADMA_ENABLED              (SDHC_EISTER_ADMA_ENABLED_Val << SDHC_EISTER_ADMA_Pos) /**< (SDHC_EISTER) Enabled Position  */
#define SDHC_EISTER_Msk                       _U_(0x03FF)                                          /**< (SDHC_EISTER) Register Mask  */

/* EMMC mode */
#define SDHC_EISTER_EMMC_BOOTAE_Pos           _U_(12)                                              /**< (SDHC_EISTER) Boot Acknowledge Error Status Enable Position */
#define SDHC_EISTER_EMMC_BOOTAE_Msk           (_U_(0x1) << SDHC_EISTER_EMMC_BOOTAE_Pos)            /**< (SDHC_EISTER) Boot Acknowledge Error Status Enable Mask */
#define SDHC_EISTER_EMMC_BOOTAE(value)        (SDHC_EISTER_EMMC_BOOTAE_Msk & ((value) << SDHC_EISTER_EMMC_BOOTAE_Pos))
#define SDHC_EISTER_EMMC_Msk                  _U_(0x1000)                                           /**< (SDHC_EISTER_EMMC) Register Mask  */


/* -------- SDHC_NISIER : (SDHC Offset: 0x38) (R/W 16) Normal Interrupt Signal Enable -------- */
#define SDHC_NISIER_RESETVALUE                _U_(0x00)                                            /**<  (SDHC_NISIER) Normal Interrupt Signal Enable  Reset Value */

#define SDHC_NISIER_CMDC_Pos                  _U_(0)                                               /**< (SDHC_NISIER) Command Complete Signal Enable Position */
#define SDHC_NISIER_CMDC_Msk                  (_U_(0x1) << SDHC_NISIER_CMDC_Pos)                   /**< (SDHC_NISIER) Command Complete Signal Enable Mask */
#define SDHC_NISIER_CMDC(value)               (SDHC_NISIER_CMDC_Msk & ((value) << SDHC_NISIER_CMDC_Pos))
#define   SDHC_NISIER_CMDC_MASKED_Val         _U_(0x0)                                             /**< (SDHC_NISIER) Masked  */
#define   SDHC_NISIER_CMDC_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_NISIER) Enabled  */
#define SDHC_NISIER_CMDC_MASKED               (SDHC_NISIER_CMDC_MASKED_Val << SDHC_NISIER_CMDC_Pos) /**< (SDHC_NISIER) Masked Position  */
#define SDHC_NISIER_CMDC_ENABLED              (SDHC_NISIER_CMDC_ENABLED_Val << SDHC_NISIER_CMDC_Pos) /**< (SDHC_NISIER) Enabled Position  */
#define SDHC_NISIER_TRFC_Pos                  _U_(1)                                               /**< (SDHC_NISIER) Transfer Complete Signal Enable Position */
#define SDHC_NISIER_TRFC_Msk                  (_U_(0x1) << SDHC_NISIER_TRFC_Pos)                   /**< (SDHC_NISIER) Transfer Complete Signal Enable Mask */
#define SDHC_NISIER_TRFC(value)               (SDHC_NISIER_TRFC_Msk & ((value) << SDHC_NISIER_TRFC_Pos))
#define   SDHC_NISIER_TRFC_MASKED_Val         _U_(0x0)                                             /**< (SDHC_NISIER) Masked  */
#define   SDHC_NISIER_TRFC_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_NISIER) Enabled  */
#define SDHC_NISIER_TRFC_MASKED               (SDHC_NISIER_TRFC_MASKED_Val << SDHC_NISIER_TRFC_Pos) /**< (SDHC_NISIER) Masked Position  */
#define SDHC_NISIER_TRFC_ENABLED              (SDHC_NISIER_TRFC_ENABLED_Val << SDHC_NISIER_TRFC_Pos) /**< (SDHC_NISIER) Enabled Position  */
#define SDHC_NISIER_BLKGE_Pos                 _U_(2)                                               /**< (SDHC_NISIER) Block Gap Event Signal Enable Position */
#define SDHC_NISIER_BLKGE_Msk                 (_U_(0x1) << SDHC_NISIER_BLKGE_Pos)                  /**< (SDHC_NISIER) Block Gap Event Signal Enable Mask */
#define SDHC_NISIER_BLKGE(value)              (SDHC_NISIER_BLKGE_Msk & ((value) << SDHC_NISIER_BLKGE_Pos))
#define   SDHC_NISIER_BLKGE_MASKED_Val        _U_(0x0)                                             /**< (SDHC_NISIER) Masked  */
#define   SDHC_NISIER_BLKGE_ENABLED_Val       _U_(0x1)                                             /**< (SDHC_NISIER) Enabled  */
#define SDHC_NISIER_BLKGE_MASKED              (SDHC_NISIER_BLKGE_MASKED_Val << SDHC_NISIER_BLKGE_Pos) /**< (SDHC_NISIER) Masked Position  */
#define SDHC_NISIER_BLKGE_ENABLED             (SDHC_NISIER_BLKGE_ENABLED_Val << SDHC_NISIER_BLKGE_Pos) /**< (SDHC_NISIER) Enabled Position  */
#define SDHC_NISIER_DMAINT_Pos                _U_(3)                                               /**< (SDHC_NISIER) DMA Interrupt Signal Enable Position */
#define SDHC_NISIER_DMAINT_Msk                (_U_(0x1) << SDHC_NISIER_DMAINT_Pos)                 /**< (SDHC_NISIER) DMA Interrupt Signal Enable Mask */
#define SDHC_NISIER_DMAINT(value)             (SDHC_NISIER_DMAINT_Msk & ((value) << SDHC_NISIER_DMAINT_Pos))
#define   SDHC_NISIER_DMAINT_MASKED_Val       _U_(0x0)                                             /**< (SDHC_NISIER) Masked  */
#define   SDHC_NISIER_DMAINT_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_NISIER) Enabled  */
#define SDHC_NISIER_DMAINT_MASKED             (SDHC_NISIER_DMAINT_MASKED_Val << SDHC_NISIER_DMAINT_Pos) /**< (SDHC_NISIER) Masked Position  */
#define SDHC_NISIER_DMAINT_ENABLED            (SDHC_NISIER_DMAINT_ENABLED_Val << SDHC_NISIER_DMAINT_Pos) /**< (SDHC_NISIER) Enabled Position  */
#define SDHC_NISIER_BWRRDY_Pos                _U_(4)                                               /**< (SDHC_NISIER) Buffer Write Ready Signal Enable Position */
#define SDHC_NISIER_BWRRDY_Msk                (_U_(0x1) << SDHC_NISIER_BWRRDY_Pos)                 /**< (SDHC_NISIER) Buffer Write Ready Signal Enable Mask */
#define SDHC_NISIER_BWRRDY(value)             (SDHC_NISIER_BWRRDY_Msk & ((value) << SDHC_NISIER_BWRRDY_Pos))
#define   SDHC_NISIER_BWRRDY_MASKED_Val       _U_(0x0)                                             /**< (SDHC_NISIER) Masked  */
#define   SDHC_NISIER_BWRRDY_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_NISIER) Enabled  */
#define SDHC_NISIER_BWRRDY_MASKED             (SDHC_NISIER_BWRRDY_MASKED_Val << SDHC_NISIER_BWRRDY_Pos) /**< (SDHC_NISIER) Masked Position  */
#define SDHC_NISIER_BWRRDY_ENABLED            (SDHC_NISIER_BWRRDY_ENABLED_Val << SDHC_NISIER_BWRRDY_Pos) /**< (SDHC_NISIER) Enabled Position  */
#define SDHC_NISIER_BRDRDY_Pos                _U_(5)                                               /**< (SDHC_NISIER) Buffer Read Ready Signal Enable Position */
#define SDHC_NISIER_BRDRDY_Msk                (_U_(0x1) << SDHC_NISIER_BRDRDY_Pos)                 /**< (SDHC_NISIER) Buffer Read Ready Signal Enable Mask */
#define SDHC_NISIER_BRDRDY(value)             (SDHC_NISIER_BRDRDY_Msk & ((value) << SDHC_NISIER_BRDRDY_Pos))
#define   SDHC_NISIER_BRDRDY_MASKED_Val       _U_(0x0)                                             /**< (SDHC_NISIER) Masked  */
#define   SDHC_NISIER_BRDRDY_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_NISIER) Enabled  */
#define SDHC_NISIER_BRDRDY_MASKED             (SDHC_NISIER_BRDRDY_MASKED_Val << SDHC_NISIER_BRDRDY_Pos) /**< (SDHC_NISIER) Masked Position  */
#define SDHC_NISIER_BRDRDY_ENABLED            (SDHC_NISIER_BRDRDY_ENABLED_Val << SDHC_NISIER_BRDRDY_Pos) /**< (SDHC_NISIER) Enabled Position  */
#define SDHC_NISIER_CINS_Pos                  _U_(6)                                               /**< (SDHC_NISIER) Card Insertion Signal Enable Position */
#define SDHC_NISIER_CINS_Msk                  (_U_(0x1) << SDHC_NISIER_CINS_Pos)                   /**< (SDHC_NISIER) Card Insertion Signal Enable Mask */
#define SDHC_NISIER_CINS(value)               (SDHC_NISIER_CINS_Msk & ((value) << SDHC_NISIER_CINS_Pos))
#define   SDHC_NISIER_CINS_MASKED_Val         _U_(0x0)                                             /**< (SDHC_NISIER) Masked  */
#define   SDHC_NISIER_CINS_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_NISIER) Enabled  */
#define SDHC_NISIER_CINS_MASKED               (SDHC_NISIER_CINS_MASKED_Val << SDHC_NISIER_CINS_Pos) /**< (SDHC_NISIER) Masked Position  */
#define SDHC_NISIER_CINS_ENABLED              (SDHC_NISIER_CINS_ENABLED_Val << SDHC_NISIER_CINS_Pos) /**< (SDHC_NISIER) Enabled Position  */
#define SDHC_NISIER_CREM_Pos                  _U_(7)                                               /**< (SDHC_NISIER) Card Removal Signal Enable Position */
#define SDHC_NISIER_CREM_Msk                  (_U_(0x1) << SDHC_NISIER_CREM_Pos)                   /**< (SDHC_NISIER) Card Removal Signal Enable Mask */
#define SDHC_NISIER_CREM(value)               (SDHC_NISIER_CREM_Msk & ((value) << SDHC_NISIER_CREM_Pos))
#define   SDHC_NISIER_CREM_MASKED_Val         _U_(0x0)                                             /**< (SDHC_NISIER) Masked  */
#define   SDHC_NISIER_CREM_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_NISIER) Enabled  */
#define SDHC_NISIER_CREM_MASKED               (SDHC_NISIER_CREM_MASKED_Val << SDHC_NISIER_CREM_Pos) /**< (SDHC_NISIER) Masked Position  */
#define SDHC_NISIER_CREM_ENABLED              (SDHC_NISIER_CREM_ENABLED_Val << SDHC_NISIER_CREM_Pos) /**< (SDHC_NISIER) Enabled Position  */
#define SDHC_NISIER_CINT_Pos                  _U_(8)                                               /**< (SDHC_NISIER) Card Interrupt Signal Enable Position */
#define SDHC_NISIER_CINT_Msk                  (_U_(0x1) << SDHC_NISIER_CINT_Pos)                   /**< (SDHC_NISIER) Card Interrupt Signal Enable Mask */
#define SDHC_NISIER_CINT(value)               (SDHC_NISIER_CINT_Msk & ((value) << SDHC_NISIER_CINT_Pos))
#define   SDHC_NISIER_CINT_MASKED_Val         _U_(0x0)                                             /**< (SDHC_NISIER) Masked  */
#define   SDHC_NISIER_CINT_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_NISIER) Enabled  */
#define SDHC_NISIER_CINT_MASKED               (SDHC_NISIER_CINT_MASKED_Val << SDHC_NISIER_CINT_Pos) /**< (SDHC_NISIER) Masked Position  */
#define SDHC_NISIER_CINT_ENABLED              (SDHC_NISIER_CINT_ENABLED_Val << SDHC_NISIER_CINT_Pos) /**< (SDHC_NISIER) Enabled Position  */
#define SDHC_NISIER_Msk                       _U_(0x01FF)                                          /**< (SDHC_NISIER) Register Mask  */

/* EMMC mode */
#define SDHC_NISIER_EMMC_BOOTAR_Pos           _U_(14)                                              /**< (SDHC_NISIER) Boot Acknowledge Received Signal Enable Position */
#define SDHC_NISIER_EMMC_BOOTAR_Msk           (_U_(0x1) << SDHC_NISIER_EMMC_BOOTAR_Pos)            /**< (SDHC_NISIER) Boot Acknowledge Received Signal Enable Mask */
#define SDHC_NISIER_EMMC_BOOTAR(value)        (SDHC_NISIER_EMMC_BOOTAR_Msk & ((value) << SDHC_NISIER_EMMC_BOOTAR_Pos))
#define SDHC_NISIER_EMMC_Msk                  _U_(0x4000)                                           /**< (SDHC_NISIER_EMMC) Register Mask  */


/* -------- SDHC_EISIER : (SDHC Offset: 0x3A) (R/W 16) Error Interrupt Signal Enable -------- */
#define SDHC_EISIER_RESETVALUE                _U_(0x00)                                            /**<  (SDHC_EISIER) Error Interrupt Signal Enable  Reset Value */

#define SDHC_EISIER_CMDTEO_Pos                _U_(0)                                               /**< (SDHC_EISIER) Command Timeout Error Signal Enable Position */
#define SDHC_EISIER_CMDTEO_Msk                (_U_(0x1) << SDHC_EISIER_CMDTEO_Pos)                 /**< (SDHC_EISIER) Command Timeout Error Signal Enable Mask */
#define SDHC_EISIER_CMDTEO(value)             (SDHC_EISIER_CMDTEO_Msk & ((value) << SDHC_EISIER_CMDTEO_Pos))
#define   SDHC_EISIER_CMDTEO_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISIER) Masked  */
#define   SDHC_EISIER_CMDTEO_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISIER) Enabled  */
#define SDHC_EISIER_CMDTEO_MASKED             (SDHC_EISIER_CMDTEO_MASKED_Val << SDHC_EISIER_CMDTEO_Pos) /**< (SDHC_EISIER) Masked Position  */
#define SDHC_EISIER_CMDTEO_ENABLED            (SDHC_EISIER_CMDTEO_ENABLED_Val << SDHC_EISIER_CMDTEO_Pos) /**< (SDHC_EISIER) Enabled Position  */
#define SDHC_EISIER_CMDCRC_Pos                _U_(1)                                               /**< (SDHC_EISIER) Command CRC Error Signal Enable Position */
#define SDHC_EISIER_CMDCRC_Msk                (_U_(0x1) << SDHC_EISIER_CMDCRC_Pos)                 /**< (SDHC_EISIER) Command CRC Error Signal Enable Mask */
#define SDHC_EISIER_CMDCRC(value)             (SDHC_EISIER_CMDCRC_Msk & ((value) << SDHC_EISIER_CMDCRC_Pos))
#define   SDHC_EISIER_CMDCRC_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISIER) Masked  */
#define   SDHC_EISIER_CMDCRC_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISIER) Enabled  */
#define SDHC_EISIER_CMDCRC_MASKED             (SDHC_EISIER_CMDCRC_MASKED_Val << SDHC_EISIER_CMDCRC_Pos) /**< (SDHC_EISIER) Masked Position  */
#define SDHC_EISIER_CMDCRC_ENABLED            (SDHC_EISIER_CMDCRC_ENABLED_Val << SDHC_EISIER_CMDCRC_Pos) /**< (SDHC_EISIER) Enabled Position  */
#define SDHC_EISIER_CMDEND_Pos                _U_(2)                                               /**< (SDHC_EISIER) Command End Bit Error Signal Enable Position */
#define SDHC_EISIER_CMDEND_Msk                (_U_(0x1) << SDHC_EISIER_CMDEND_Pos)                 /**< (SDHC_EISIER) Command End Bit Error Signal Enable Mask */
#define SDHC_EISIER_CMDEND(value)             (SDHC_EISIER_CMDEND_Msk & ((value) << SDHC_EISIER_CMDEND_Pos))
#define   SDHC_EISIER_CMDEND_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISIER) Masked  */
#define   SDHC_EISIER_CMDEND_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISIER) Enabled  */
#define SDHC_EISIER_CMDEND_MASKED             (SDHC_EISIER_CMDEND_MASKED_Val << SDHC_EISIER_CMDEND_Pos) /**< (SDHC_EISIER) Masked Position  */
#define SDHC_EISIER_CMDEND_ENABLED            (SDHC_EISIER_CMDEND_ENABLED_Val << SDHC_EISIER_CMDEND_Pos) /**< (SDHC_EISIER) Enabled Position  */
#define SDHC_EISIER_CMDIDX_Pos                _U_(3)                                               /**< (SDHC_EISIER) Command Index Error Signal Enable Position */
#define SDHC_EISIER_CMDIDX_Msk                (_U_(0x1) << SDHC_EISIER_CMDIDX_Pos)                 /**< (SDHC_EISIER) Command Index Error Signal Enable Mask */
#define SDHC_EISIER_CMDIDX(value)             (SDHC_EISIER_CMDIDX_Msk & ((value) << SDHC_EISIER_CMDIDX_Pos))
#define   SDHC_EISIER_CMDIDX_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISIER) Masked  */
#define   SDHC_EISIER_CMDIDX_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISIER) Enabled  */
#define SDHC_EISIER_CMDIDX_MASKED             (SDHC_EISIER_CMDIDX_MASKED_Val << SDHC_EISIER_CMDIDX_Pos) /**< (SDHC_EISIER) Masked Position  */
#define SDHC_EISIER_CMDIDX_ENABLED            (SDHC_EISIER_CMDIDX_ENABLED_Val << SDHC_EISIER_CMDIDX_Pos) /**< (SDHC_EISIER) Enabled Position  */
#define SDHC_EISIER_DATTEO_Pos                _U_(4)                                               /**< (SDHC_EISIER) Data Timeout Error Signal Enable Position */
#define SDHC_EISIER_DATTEO_Msk                (_U_(0x1) << SDHC_EISIER_DATTEO_Pos)                 /**< (SDHC_EISIER) Data Timeout Error Signal Enable Mask */
#define SDHC_EISIER_DATTEO(value)             (SDHC_EISIER_DATTEO_Msk & ((value) << SDHC_EISIER_DATTEO_Pos))
#define   SDHC_EISIER_DATTEO_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISIER) Masked  */
#define   SDHC_EISIER_DATTEO_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISIER) Enabled  */
#define SDHC_EISIER_DATTEO_MASKED             (SDHC_EISIER_DATTEO_MASKED_Val << SDHC_EISIER_DATTEO_Pos) /**< (SDHC_EISIER) Masked Position  */
#define SDHC_EISIER_DATTEO_ENABLED            (SDHC_EISIER_DATTEO_ENABLED_Val << SDHC_EISIER_DATTEO_Pos) /**< (SDHC_EISIER) Enabled Position  */
#define SDHC_EISIER_DATCRC_Pos                _U_(5)                                               /**< (SDHC_EISIER) Data CRC Error Signal Enable Position */
#define SDHC_EISIER_DATCRC_Msk                (_U_(0x1) << SDHC_EISIER_DATCRC_Pos)                 /**< (SDHC_EISIER) Data CRC Error Signal Enable Mask */
#define SDHC_EISIER_DATCRC(value)             (SDHC_EISIER_DATCRC_Msk & ((value) << SDHC_EISIER_DATCRC_Pos))
#define   SDHC_EISIER_DATCRC_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISIER) Masked  */
#define   SDHC_EISIER_DATCRC_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISIER) Enabled  */
#define SDHC_EISIER_DATCRC_MASKED             (SDHC_EISIER_DATCRC_MASKED_Val << SDHC_EISIER_DATCRC_Pos) /**< (SDHC_EISIER) Masked Position  */
#define SDHC_EISIER_DATCRC_ENABLED            (SDHC_EISIER_DATCRC_ENABLED_Val << SDHC_EISIER_DATCRC_Pos) /**< (SDHC_EISIER) Enabled Position  */
#define SDHC_EISIER_DATEND_Pos                _U_(6)                                               /**< (SDHC_EISIER) Data End Bit Error Signal Enable Position */
#define SDHC_EISIER_DATEND_Msk                (_U_(0x1) << SDHC_EISIER_DATEND_Pos)                 /**< (SDHC_EISIER) Data End Bit Error Signal Enable Mask */
#define SDHC_EISIER_DATEND(value)             (SDHC_EISIER_DATEND_Msk & ((value) << SDHC_EISIER_DATEND_Pos))
#define   SDHC_EISIER_DATEND_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISIER) Masked  */
#define   SDHC_EISIER_DATEND_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISIER) Enabled  */
#define SDHC_EISIER_DATEND_MASKED             (SDHC_EISIER_DATEND_MASKED_Val << SDHC_EISIER_DATEND_Pos) /**< (SDHC_EISIER) Masked Position  */
#define SDHC_EISIER_DATEND_ENABLED            (SDHC_EISIER_DATEND_ENABLED_Val << SDHC_EISIER_DATEND_Pos) /**< (SDHC_EISIER) Enabled Position  */
#define SDHC_EISIER_CURLIM_Pos                _U_(7)                                               /**< (SDHC_EISIER) Current Limit Error Signal Enable Position */
#define SDHC_EISIER_CURLIM_Msk                (_U_(0x1) << SDHC_EISIER_CURLIM_Pos)                 /**< (SDHC_EISIER) Current Limit Error Signal Enable Mask */
#define SDHC_EISIER_CURLIM(value)             (SDHC_EISIER_CURLIM_Msk & ((value) << SDHC_EISIER_CURLIM_Pos))
#define   SDHC_EISIER_CURLIM_MASKED_Val       _U_(0x0)                                             /**< (SDHC_EISIER) Masked  */
#define   SDHC_EISIER_CURLIM_ENABLED_Val      _U_(0x1)                                             /**< (SDHC_EISIER) Enabled  */
#define SDHC_EISIER_CURLIM_MASKED             (SDHC_EISIER_CURLIM_MASKED_Val << SDHC_EISIER_CURLIM_Pos) /**< (SDHC_EISIER) Masked Position  */
#define SDHC_EISIER_CURLIM_ENABLED            (SDHC_EISIER_CURLIM_ENABLED_Val << SDHC_EISIER_CURLIM_Pos) /**< (SDHC_EISIER) Enabled Position  */
#define SDHC_EISIER_ACMD_Pos                  _U_(8)                                               /**< (SDHC_EISIER) Auto CMD Error Signal Enable Position */
#define SDHC_EISIER_ACMD_Msk                  (_U_(0x1) << SDHC_EISIER_ACMD_Pos)                   /**< (SDHC_EISIER) Auto CMD Error Signal Enable Mask */
#define SDHC_EISIER_ACMD(value)               (SDHC_EISIER_ACMD_Msk & ((value) << SDHC_EISIER_ACMD_Pos))
#define   SDHC_EISIER_ACMD_MASKED_Val         _U_(0x0)                                             /**< (SDHC_EISIER) Masked  */
#define   SDHC_EISIER_ACMD_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_EISIER) Enabled  */
#define SDHC_EISIER_ACMD_MASKED               (SDHC_EISIER_ACMD_MASKED_Val << SDHC_EISIER_ACMD_Pos) /**< (SDHC_EISIER) Masked Position  */
#define SDHC_EISIER_ACMD_ENABLED              (SDHC_EISIER_ACMD_ENABLED_Val << SDHC_EISIER_ACMD_Pos) /**< (SDHC_EISIER) Enabled Position  */
#define SDHC_EISIER_ADMA_Pos                  _U_(9)                                               /**< (SDHC_EISIER) ADMA Error Signal Enable Position */
#define SDHC_EISIER_ADMA_Msk                  (_U_(0x1) << SDHC_EISIER_ADMA_Pos)                   /**< (SDHC_EISIER) ADMA Error Signal Enable Mask */
#define SDHC_EISIER_ADMA(value)               (SDHC_EISIER_ADMA_Msk & ((value) << SDHC_EISIER_ADMA_Pos))
#define   SDHC_EISIER_ADMA_MASKED_Val         _U_(0x0)                                             /**< (SDHC_EISIER) Masked  */
#define   SDHC_EISIER_ADMA_ENABLED_Val        _U_(0x1)                                             /**< (SDHC_EISIER) Enabled  */
#define SDHC_EISIER_ADMA_MASKED               (SDHC_EISIER_ADMA_MASKED_Val << SDHC_EISIER_ADMA_Pos) /**< (SDHC_EISIER) Masked Position  */
#define SDHC_EISIER_ADMA_ENABLED              (SDHC_EISIER_ADMA_ENABLED_Val << SDHC_EISIER_ADMA_Pos) /**< (SDHC_EISIER) Enabled Position  */
#define SDHC_EISIER_Msk                       _U_(0x03FF)                                          /**< (SDHC_EISIER) Register Mask  */

/* EMMC mode */
#define SDHC_EISIER_EMMC_BOOTAE_Pos           _U_(12)                                              /**< (SDHC_EISIER) Boot Acknowledge Error Signal Enable Position */
#define SDHC_EISIER_EMMC_BOOTAE_Msk           (_U_(0x1) << SDHC_EISIER_EMMC_BOOTAE_Pos)            /**< (SDHC_EISIER) Boot Acknowledge Error Signal Enable Mask */
#define SDHC_EISIER_EMMC_BOOTAE(value)        (SDHC_EISIER_EMMC_BOOTAE_Msk & ((value) << SDHC_EISIER_EMMC_BOOTAE_Pos))
#define SDHC_EISIER_EMMC_Msk                  _U_(0x1000)                                           /**< (SDHC_EISIER_EMMC) Register Mask  */


/* -------- SDHC_ACESR : (SDHC Offset: 0x3C) ( R/ 16) Auto CMD Error Status -------- */
#define SDHC_ACESR_RESETVALUE                 _U_(0x00)                                            /**<  (SDHC_ACESR) Auto CMD Error Status  Reset Value */

#define SDHC_ACESR_ACMD12NE_Pos               _U_(0)                                               /**< (SDHC_ACESR) Auto CMD12 Not Executed Position */
#define SDHC_ACESR_ACMD12NE_Msk               (_U_(0x1) << SDHC_ACESR_ACMD12NE_Pos)                /**< (SDHC_ACESR) Auto CMD12 Not Executed Mask */
#define SDHC_ACESR_ACMD12NE(value)            (SDHC_ACESR_ACMD12NE_Msk & ((value) << SDHC_ACESR_ACMD12NE_Pos))
#define   SDHC_ACESR_ACMD12NE_EXEC_Val        _U_(0x0)                                             /**< (SDHC_ACESR) Executed  */
#define   SDHC_ACESR_ACMD12NE_NOT_EXEC_Val    _U_(0x1)                                             /**< (SDHC_ACESR) Not executed  */
#define SDHC_ACESR_ACMD12NE_EXEC              (SDHC_ACESR_ACMD12NE_EXEC_Val << SDHC_ACESR_ACMD12NE_Pos) /**< (SDHC_ACESR) Executed Position  */
#define SDHC_ACESR_ACMD12NE_NOT_EXEC          (SDHC_ACESR_ACMD12NE_NOT_EXEC_Val << SDHC_ACESR_ACMD12NE_Pos) /**< (SDHC_ACESR) Not executed Position  */
#define SDHC_ACESR_ACMDTEO_Pos                _U_(1)                                               /**< (SDHC_ACESR) Auto CMD Timeout Error Position */
#define SDHC_ACESR_ACMDTEO_Msk                (_U_(0x1) << SDHC_ACESR_ACMDTEO_Pos)                 /**< (SDHC_ACESR) Auto CMD Timeout Error Mask */
#define SDHC_ACESR_ACMDTEO(value)             (SDHC_ACESR_ACMDTEO_Msk & ((value) << SDHC_ACESR_ACMDTEO_Pos))
#define   SDHC_ACESR_ACMDTEO_NO_Val           _U_(0x0)                                             /**< (SDHC_ACESR) No error  */
#define   SDHC_ACESR_ACMDTEO_YES_Val          _U_(0x1)                                             /**< (SDHC_ACESR) Timeout  */
#define SDHC_ACESR_ACMDTEO_NO                 (SDHC_ACESR_ACMDTEO_NO_Val << SDHC_ACESR_ACMDTEO_Pos) /**< (SDHC_ACESR) No error Position  */
#define SDHC_ACESR_ACMDTEO_YES                (SDHC_ACESR_ACMDTEO_YES_Val << SDHC_ACESR_ACMDTEO_Pos) /**< (SDHC_ACESR) Timeout Position  */
#define SDHC_ACESR_ACMDCRC_Pos                _U_(2)                                               /**< (SDHC_ACESR) Auto CMD CRC Error Position */
#define SDHC_ACESR_ACMDCRC_Msk                (_U_(0x1) << SDHC_ACESR_ACMDCRC_Pos)                 /**< (SDHC_ACESR) Auto CMD CRC Error Mask */
#define SDHC_ACESR_ACMDCRC(value)             (SDHC_ACESR_ACMDCRC_Msk & ((value) << SDHC_ACESR_ACMDCRC_Pos))
#define   SDHC_ACESR_ACMDCRC_NO_Val           _U_(0x0)                                             /**< (SDHC_ACESR) No error  */
#define   SDHC_ACESR_ACMDCRC_YES_Val          _U_(0x1)                                             /**< (SDHC_ACESR) CRC Error Generated  */
#define SDHC_ACESR_ACMDCRC_NO                 (SDHC_ACESR_ACMDCRC_NO_Val << SDHC_ACESR_ACMDCRC_Pos) /**< (SDHC_ACESR) No error Position  */
#define SDHC_ACESR_ACMDCRC_YES                (SDHC_ACESR_ACMDCRC_YES_Val << SDHC_ACESR_ACMDCRC_Pos) /**< (SDHC_ACESR) CRC Error Generated Position  */
#define SDHC_ACESR_ACMDEND_Pos                _U_(3)                                               /**< (SDHC_ACESR) Auto CMD End Bit Error Position */
#define SDHC_ACESR_ACMDEND_Msk                (_U_(0x1) << SDHC_ACESR_ACMDEND_Pos)                 /**< (SDHC_ACESR) Auto CMD End Bit Error Mask */
#define SDHC_ACESR_ACMDEND(value)             (SDHC_ACESR_ACMDEND_Msk & ((value) << SDHC_ACESR_ACMDEND_Pos))
#define   SDHC_ACESR_ACMDEND_NO_Val           _U_(0x0)                                             /**< (SDHC_ACESR) No error  */
#define   SDHC_ACESR_ACMDEND_YES_Val          _U_(0x1)                                             /**< (SDHC_ACESR) End Bit Error Generated  */
#define SDHC_ACESR_ACMDEND_NO                 (SDHC_ACESR_ACMDEND_NO_Val << SDHC_ACESR_ACMDEND_Pos) /**< (SDHC_ACESR) No error Position  */
#define SDHC_ACESR_ACMDEND_YES                (SDHC_ACESR_ACMDEND_YES_Val << SDHC_ACESR_ACMDEND_Pos) /**< (SDHC_ACESR) End Bit Error Generated Position  */
#define SDHC_ACESR_ACMDIDX_Pos                _U_(4)                                               /**< (SDHC_ACESR) Auto CMD Index Error Position */
#define SDHC_ACESR_ACMDIDX_Msk                (_U_(0x1) << SDHC_ACESR_ACMDIDX_Pos)                 /**< (SDHC_ACESR) Auto CMD Index Error Mask */
#define SDHC_ACESR_ACMDIDX(value)             (SDHC_ACESR_ACMDIDX_Msk & ((value) << SDHC_ACESR_ACMDIDX_Pos))
#define   SDHC_ACESR_ACMDIDX_NO_Val           _U_(0x0)                                             /**< (SDHC_ACESR) No error  */
#define   SDHC_ACESR_ACMDIDX_YES_Val          _U_(0x1)                                             /**< (SDHC_ACESR) Error  */
#define SDHC_ACESR_ACMDIDX_NO                 (SDHC_ACESR_ACMDIDX_NO_Val << SDHC_ACESR_ACMDIDX_Pos) /**< (SDHC_ACESR) No error Position  */
#define SDHC_ACESR_ACMDIDX_YES                (SDHC_ACESR_ACMDIDX_YES_Val << SDHC_ACESR_ACMDIDX_Pos) /**< (SDHC_ACESR) Error Position  */
#define SDHC_ACESR_CMDNI_Pos                  _U_(7)                                               /**< (SDHC_ACESR) Command not Issued By Auto CMD12 Error Position */
#define SDHC_ACESR_CMDNI_Msk                  (_U_(0x1) << SDHC_ACESR_CMDNI_Pos)                   /**< (SDHC_ACESR) Command not Issued By Auto CMD12 Error Mask */
#define SDHC_ACESR_CMDNI(value)               (SDHC_ACESR_CMDNI_Msk & ((value) << SDHC_ACESR_CMDNI_Pos))
#define   SDHC_ACESR_CMDNI_OK_Val             _U_(0x0)                                             /**< (SDHC_ACESR) No error  */
#define   SDHC_ACESR_CMDNI_NOT_ISSUED_Val     _U_(0x1)                                             /**< (SDHC_ACESR) Not Issued  */
#define SDHC_ACESR_CMDNI_OK                   (SDHC_ACESR_CMDNI_OK_Val << SDHC_ACESR_CMDNI_Pos)    /**< (SDHC_ACESR) No error Position  */
#define SDHC_ACESR_CMDNI_NOT_ISSUED           (SDHC_ACESR_CMDNI_NOT_ISSUED_Val << SDHC_ACESR_CMDNI_Pos) /**< (SDHC_ACESR) Not Issued Position  */
#define SDHC_ACESR_Msk                        _U_(0x009F)                                          /**< (SDHC_ACESR) Register Mask  */


/* -------- SDHC_HC2R : (SDHC Offset: 0x3E) (R/W 16) Host Control 2 -------- */
#define SDHC_HC2R_RESETVALUE                  _U_(0x00)                                            /**<  (SDHC_HC2R) Host Control 2  Reset Value */

#define SDHC_HC2R_UHSMS_Pos                   _U_(0)                                               /**< (SDHC_HC2R) UHS Mode Select Position */
#define SDHC_HC2R_UHSMS_Msk                   (_U_(0x7) << SDHC_HC2R_UHSMS_Pos)                    /**< (SDHC_HC2R) UHS Mode Select Mask */
#define SDHC_HC2R_UHSMS(value)                (SDHC_HC2R_UHSMS_Msk & ((value) << SDHC_HC2R_UHSMS_Pos))
#define   SDHC_HC2R_UHSMS_SDR12_Val           _U_(0x0)                                             /**< (SDHC_HC2R) SDR12  */
#define   SDHC_HC2R_UHSMS_SDR25_Val           _U_(0x1)                                             /**< (SDHC_HC2R) SDR25  */
#define   SDHC_HC2R_UHSMS_SDR50_Val           _U_(0x2)                                             /**< (SDHC_HC2R) SDR50  */
#define   SDHC_HC2R_UHSMS_SDR104_Val          _U_(0x3)                                             /**< (SDHC_HC2R) SDR104  */
#define   SDHC_HC2R_UHSMS_DDR50_Val           _U_(0x4)                                             /**< (SDHC_HC2R) DDR50  */
#define SDHC_HC2R_UHSMS_SDR12                 (SDHC_HC2R_UHSMS_SDR12_Val << SDHC_HC2R_UHSMS_Pos)   /**< (SDHC_HC2R) SDR12 Position  */
#define SDHC_HC2R_UHSMS_SDR25                 (SDHC_HC2R_UHSMS_SDR25_Val << SDHC_HC2R_UHSMS_Pos)   /**< (SDHC_HC2R) SDR25 Position  */
#define SDHC_HC2R_UHSMS_SDR50                 (SDHC_HC2R_UHSMS_SDR50_Val << SDHC_HC2R_UHSMS_Pos)   /**< (SDHC_HC2R) SDR50 Position  */
#define SDHC_HC2R_UHSMS_SDR104                (SDHC_HC2R_UHSMS_SDR104_Val << SDHC_HC2R_UHSMS_Pos)  /**< (SDHC_HC2R) SDR104 Position  */
#define SDHC_HC2R_UHSMS_DDR50                 (SDHC_HC2R_UHSMS_DDR50_Val << SDHC_HC2R_UHSMS_Pos)   /**< (SDHC_HC2R) DDR50 Position  */
#define SDHC_HC2R_VS18EN_Pos                  _U_(3)                                               /**< (SDHC_HC2R) 1.8V Signaling Enable Position */
#define SDHC_HC2R_VS18EN_Msk                  (_U_(0x1) << SDHC_HC2R_VS18EN_Pos)                   /**< (SDHC_HC2R) 1.8V Signaling Enable Mask */
#define SDHC_HC2R_VS18EN(value)               (SDHC_HC2R_VS18EN_Msk & ((value) << SDHC_HC2R_VS18EN_Pos))
#define   SDHC_HC2R_VS18EN_S33V_Val           _U_(0x0)                                             /**< (SDHC_HC2R) 3.3V Signaling  */
#define   SDHC_HC2R_VS18EN_S18V_Val           _U_(0x1)                                             /**< (SDHC_HC2R) 1.8V Signaling  */
#define SDHC_HC2R_VS18EN_S33V                 (SDHC_HC2R_VS18EN_S33V_Val << SDHC_HC2R_VS18EN_Pos)  /**< (SDHC_HC2R) 3.3V Signaling Position  */
#define SDHC_HC2R_VS18EN_S18V                 (SDHC_HC2R_VS18EN_S18V_Val << SDHC_HC2R_VS18EN_Pos)  /**< (SDHC_HC2R) 1.8V Signaling Position  */
#define SDHC_HC2R_DRVSEL_Pos                  _U_(4)                                               /**< (SDHC_HC2R) Driver Strength Select Position */
#define SDHC_HC2R_DRVSEL_Msk                  (_U_(0x3) << SDHC_HC2R_DRVSEL_Pos)                   /**< (SDHC_HC2R) Driver Strength Select Mask */
#define SDHC_HC2R_DRVSEL(value)               (SDHC_HC2R_DRVSEL_Msk & ((value) << SDHC_HC2R_DRVSEL_Pos))
#define   SDHC_HC2R_DRVSEL_B_Val              _U_(0x0)                                             /**< (SDHC_HC2R) Driver Type B is Selected (Default)  */
#define   SDHC_HC2R_DRVSEL_A_Val              _U_(0x1)                                             /**< (SDHC_HC2R) Driver Type A is Selected  */
#define   SDHC_HC2R_DRVSEL_C_Val              _U_(0x2)                                             /**< (SDHC_HC2R) Driver Type C is Selected  */
#define   SDHC_HC2R_DRVSEL_D_Val              _U_(0x3)                                             /**< (SDHC_HC2R) Driver Type D is Selected  */
#define SDHC_HC2R_DRVSEL_B                    (SDHC_HC2R_DRVSEL_B_Val << SDHC_HC2R_DRVSEL_Pos)     /**< (SDHC_HC2R) Driver Type B is Selected (Default) Position  */
#define SDHC_HC2R_DRVSEL_A                    (SDHC_HC2R_DRVSEL_A_Val << SDHC_HC2R_DRVSEL_Pos)     /**< (SDHC_HC2R) Driver Type A is Selected Position  */
#define SDHC_HC2R_DRVSEL_C                    (SDHC_HC2R_DRVSEL_C_Val << SDHC_HC2R_DRVSEL_Pos)     /**< (SDHC_HC2R) Driver Type C is Selected Position  */
#define SDHC_HC2R_DRVSEL_D                    (SDHC_HC2R_DRVSEL_D_Val << SDHC_HC2R_DRVSEL_Pos)     /**< (SDHC_HC2R) Driver Type D is Selected Position  */
#define SDHC_HC2R_EXTUN_Pos                   _U_(6)                                               /**< (SDHC_HC2R) Execute Tuning Position */
#define SDHC_HC2R_EXTUN_Msk                   (_U_(0x1) << SDHC_HC2R_EXTUN_Pos)                    /**< (SDHC_HC2R) Execute Tuning Mask */
#define SDHC_HC2R_EXTUN(value)                (SDHC_HC2R_EXTUN_Msk & ((value) << SDHC_HC2R_EXTUN_Pos))
#define   SDHC_HC2R_EXTUN_NO_Val              _U_(0x0)                                             /**< (SDHC_HC2R) Not Tuned or Tuning Completed  */
#define   SDHC_HC2R_EXTUN_REQUESTED_Val       _U_(0x1)                                             /**< (SDHC_HC2R) Execute Tuning  */
#define SDHC_HC2R_EXTUN_NO                    (SDHC_HC2R_EXTUN_NO_Val << SDHC_HC2R_EXTUN_Pos)      /**< (SDHC_HC2R) Not Tuned or Tuning Completed Position  */
#define SDHC_HC2R_EXTUN_REQUESTED             (SDHC_HC2R_EXTUN_REQUESTED_Val << SDHC_HC2R_EXTUN_Pos) /**< (SDHC_HC2R) Execute Tuning Position  */
#define SDHC_HC2R_SLCKSEL_Pos                 _U_(7)                                               /**< (SDHC_HC2R) Sampling Clock Select Position */
#define SDHC_HC2R_SLCKSEL_Msk                 (_U_(0x1) << SDHC_HC2R_SLCKSEL_Pos)                  /**< (SDHC_HC2R) Sampling Clock Select Mask */
#define SDHC_HC2R_SLCKSEL(value)              (SDHC_HC2R_SLCKSEL_Msk & ((value) << SDHC_HC2R_SLCKSEL_Pos))
#define   SDHC_HC2R_SLCKSEL_FIXED_Val         _U_(0x0)                                             /**< (SDHC_HC2R) Fixed clock is used to sample data  */
#define   SDHC_HC2R_SLCKSEL_TUNED_Val         _U_(0x1)                                             /**< (SDHC_HC2R) Tuned clock is used to sample data  */
#define SDHC_HC2R_SLCKSEL_FIXED               (SDHC_HC2R_SLCKSEL_FIXED_Val << SDHC_HC2R_SLCKSEL_Pos) /**< (SDHC_HC2R) Fixed clock is used to sample data Position  */
#define SDHC_HC2R_SLCKSEL_TUNED               (SDHC_HC2R_SLCKSEL_TUNED_Val << SDHC_HC2R_SLCKSEL_Pos) /**< (SDHC_HC2R) Tuned clock is used to sample data Position  */
#define SDHC_HC2R_ASINTEN_Pos                 _U_(14)                                              /**< (SDHC_HC2R) Asynchronous Interrupt Enable Position */
#define SDHC_HC2R_ASINTEN_Msk                 (_U_(0x1) << SDHC_HC2R_ASINTEN_Pos)                  /**< (SDHC_HC2R) Asynchronous Interrupt Enable Mask */
#define SDHC_HC2R_ASINTEN(value)              (SDHC_HC2R_ASINTEN_Msk & ((value) << SDHC_HC2R_ASINTEN_Pos))
#define   SDHC_HC2R_ASINTEN_DISABLED_Val      _U_(0x0)                                             /**< (SDHC_HC2R) Disabled  */
#define   SDHC_HC2R_ASINTEN_ENABLED_Val       _U_(0x1)                                             /**< (SDHC_HC2R) Enabled  */
#define SDHC_HC2R_ASINTEN_DISABLED            (SDHC_HC2R_ASINTEN_DISABLED_Val << SDHC_HC2R_ASINTEN_Pos) /**< (SDHC_HC2R) Disabled Position  */
#define SDHC_HC2R_ASINTEN_ENABLED             (SDHC_HC2R_ASINTEN_ENABLED_Val << SDHC_HC2R_ASINTEN_Pos) /**< (SDHC_HC2R) Enabled Position  */
#define SDHC_HC2R_PVALEN_Pos                  _U_(15)                                              /**< (SDHC_HC2R) Preset Value Enable Position */
#define SDHC_HC2R_PVALEN_Msk                  (_U_(0x1) << SDHC_HC2R_PVALEN_Pos)                   /**< (SDHC_HC2R) Preset Value Enable Mask */
#define SDHC_HC2R_PVALEN(value)               (SDHC_HC2R_PVALEN_Msk & ((value) << SDHC_HC2R_PVALEN_Pos))
#define   SDHC_HC2R_PVALEN_HOST_Val           _U_(0x0)                                             /**< (SDHC_HC2R) SDCLK and Driver Strength are controlled by Host Controller  */
#define   SDHC_HC2R_PVALEN_AUTO_Val           _U_(0x1)                                             /**< (SDHC_HC2R) Automatic Selection by Preset Value is Enabled  */
#define SDHC_HC2R_PVALEN_HOST                 (SDHC_HC2R_PVALEN_HOST_Val << SDHC_HC2R_PVALEN_Pos)  /**< (SDHC_HC2R) SDCLK and Driver Strength are controlled by Host Controller Position  */
#define SDHC_HC2R_PVALEN_AUTO                 (SDHC_HC2R_PVALEN_AUTO_Val << SDHC_HC2R_PVALEN_Pos)  /**< (SDHC_HC2R) Automatic Selection by Preset Value is Enabled Position  */
#define SDHC_HC2R_Msk                         _U_(0xC0FF)                                          /**< (SDHC_HC2R) Register Mask  */

/* EMMC mode */
#define SDHC_HC2R_EMMC_HS200EN_Pos            _U_(0)                                               /**< (SDHC_HC2R) HS200 Mode Enable Position */
#define SDHC_HC2R_EMMC_HS200EN_Msk            (_U_(0xF) << SDHC_HC2R_EMMC_HS200EN_Pos)             /**< (SDHC_HC2R) HS200 Mode Enable Mask */
#define SDHC_HC2R_EMMC_HS200EN(value)         (SDHC_HC2R_EMMC_HS200EN_Msk & ((value) << SDHC_HC2R_EMMC_HS200EN_Pos))
#define   SDHC_HC2R_EMMC_HS200EN_SDR12_Val    _U_(0x0)                                             /**< (SDHC_HC2R) SDR12  */
#define   SDHC_HC2R_EMMC_HS200EN_SDR25_Val    _U_(0x1)                                             /**< (SDHC_HC2R) SDR25  */
#define   SDHC_HC2R_EMMC_HS200EN_SDR50_Val    _U_(0x2)                                             /**< (SDHC_HC2R) SDR50  */
#define   SDHC_HC2R_EMMC_HS200EN_SDR104_Val   _U_(0x3)                                             /**< (SDHC_HC2R) SDR104  */
#define   SDHC_HC2R_EMMC_HS200EN_DDR50_Val    _U_(0x4)                                             /**< (SDHC_HC2R) DDR50  */
#define SDHC_HC2R_EMMC_HS200EN_SDR12          (SDHC_HC2R_EMMC_HS200EN_SDR12_Val << SDHC_HC2R_EMMC_HS200EN_Pos) /**< (SDHC_HC2R) SDR12 Position  */
#define SDHC_HC2R_EMMC_HS200EN_SDR25          (SDHC_HC2R_EMMC_HS200EN_SDR25_Val << SDHC_HC2R_EMMC_HS200EN_Pos) /**< (SDHC_HC2R) SDR25 Position  */
#define SDHC_HC2R_EMMC_HS200EN_SDR50          (SDHC_HC2R_EMMC_HS200EN_SDR50_Val << SDHC_HC2R_EMMC_HS200EN_Pos) /**< (SDHC_HC2R) SDR50 Position  */
#define SDHC_HC2R_EMMC_HS200EN_SDR104         (SDHC_HC2R_EMMC_HS200EN_SDR104_Val << SDHC_HC2R_EMMC_HS200EN_Pos) /**< (SDHC_HC2R) SDR104 Position  */
#define SDHC_HC2R_EMMC_HS200EN_DDR50          (SDHC_HC2R_EMMC_HS200EN_DDR50_Val << SDHC_HC2R_EMMC_HS200EN_Pos) /**< (SDHC_HC2R) DDR50 Position  */
#define SDHC_HC2R_EMMC_Msk                    _U_(0x000F)                                           /**< (SDHC_HC2R_EMMC) Register Mask  */


/* -------- SDHC_CA0R : (SDHC Offset: 0x40) ( R/ 32) Capabilities 0 -------- */
#define SDHC_CA0R_RESETVALUE                  _U_(0x27E80080)                                      /**<  (SDHC_CA0R) Capabilities 0  Reset Value */

#define SDHC_CA0R_TEOCLKF_Pos                 _U_(0)                                               /**< (SDHC_CA0R) Timeout Clock Frequency Position */
#define SDHC_CA0R_TEOCLKF_Msk                 (_U_(0x3F) << SDHC_CA0R_TEOCLKF_Pos)                 /**< (SDHC_CA0R) Timeout Clock Frequency Mask */
#define SDHC_CA0R_TEOCLKF(value)              (SDHC_CA0R_TEOCLKF_Msk & ((value) << SDHC_CA0R_TEOCLKF_Pos))
#define   SDHC_CA0R_TEOCLKF_OTHER_Val         _U_(0x0)                                             /**< (SDHC_CA0R) Get information via another method  */
#define SDHC_CA0R_TEOCLKF_OTHER               (SDHC_CA0R_TEOCLKF_OTHER_Val << SDHC_CA0R_TEOCLKF_Pos) /**< (SDHC_CA0R) Get information via another method Position  */
#define SDHC_CA0R_TEOCLKU_Pos                 _U_(7)                                               /**< (SDHC_CA0R) Timeout Clock Unit Position */
#define SDHC_CA0R_TEOCLKU_Msk                 (_U_(0x1) << SDHC_CA0R_TEOCLKU_Pos)                  /**< (SDHC_CA0R) Timeout Clock Unit Mask */
#define SDHC_CA0R_TEOCLKU(value)              (SDHC_CA0R_TEOCLKU_Msk & ((value) << SDHC_CA0R_TEOCLKU_Pos))
#define   SDHC_CA0R_TEOCLKU_KHZ_Val           _U_(0x0)                                             /**< (SDHC_CA0R) KHz  */
#define   SDHC_CA0R_TEOCLKU_MHZ_Val           _U_(0x1)                                             /**< (SDHC_CA0R) MHz  */
#define SDHC_CA0R_TEOCLKU_KHZ                 (SDHC_CA0R_TEOCLKU_KHZ_Val << SDHC_CA0R_TEOCLKU_Pos) /**< (SDHC_CA0R) KHz Position  */
#define SDHC_CA0R_TEOCLKU_MHZ                 (SDHC_CA0R_TEOCLKU_MHZ_Val << SDHC_CA0R_TEOCLKU_Pos) /**< (SDHC_CA0R) MHz Position  */
#define SDHC_CA0R_BASECLKF_Pos                _U_(8)                                               /**< (SDHC_CA0R) Base Clock Frequency Position */
#define SDHC_CA0R_BASECLKF_Msk                (_U_(0xFF) << SDHC_CA0R_BASECLKF_Pos)                /**< (SDHC_CA0R) Base Clock Frequency Mask */
#define SDHC_CA0R_BASECLKF(value)             (SDHC_CA0R_BASECLKF_Msk & ((value) << SDHC_CA0R_BASECLKF_Pos))
#define   SDHC_CA0R_BASECLKF_OTHER_Val        _U_(0x0)                                             /**< (SDHC_CA0R) Get information via another method  */
#define SDHC_CA0R_BASECLKF_OTHER              (SDHC_CA0R_BASECLKF_OTHER_Val << SDHC_CA0R_BASECLKF_Pos) /**< (SDHC_CA0R) Get information via another method Position  */
#define SDHC_CA0R_MAXBLKL_Pos                 _U_(16)                                              /**< (SDHC_CA0R) Max Block Length Position */
#define SDHC_CA0R_MAXBLKL_Msk                 (_U_(0x3) << SDHC_CA0R_MAXBLKL_Pos)                  /**< (SDHC_CA0R) Max Block Length Mask */
#define SDHC_CA0R_MAXBLKL(value)              (SDHC_CA0R_MAXBLKL_Msk & ((value) << SDHC_CA0R_MAXBLKL_Pos))
#define   SDHC_CA0R_MAXBLKL_512_Val           _U_(0x0)                                             /**< (SDHC_CA0R) 512 bytes  */
#define   SDHC_CA0R_MAXBLKL_1024_Val          _U_(0x1)                                             /**< (SDHC_CA0R) 1024 bytes  */
#define   SDHC_CA0R_MAXBLKL_2048_Val          _U_(0x2)                                             /**< (SDHC_CA0R) 2048 bytes  */
#define SDHC_CA0R_MAXBLKL_512                 (SDHC_CA0R_MAXBLKL_512_Val << SDHC_CA0R_MAXBLKL_Pos) /**< (SDHC_CA0R) 512 bytes Position  */
#define SDHC_CA0R_MAXBLKL_1024                (SDHC_CA0R_MAXBLKL_1024_Val << SDHC_CA0R_MAXBLKL_Pos) /**< (SDHC_CA0R) 1024 bytes Position  */
#define SDHC_CA0R_MAXBLKL_2048                (SDHC_CA0R_MAXBLKL_2048_Val << SDHC_CA0R_MAXBLKL_Pos) /**< (SDHC_CA0R) 2048 bytes Position  */
#define SDHC_CA0R_ED8SUP_Pos                  _U_(18)                                              /**< (SDHC_CA0R) 8-bit Support for Embedded Device Position */
#define SDHC_CA0R_ED8SUP_Msk                  (_U_(0x1) << SDHC_CA0R_ED8SUP_Pos)                   /**< (SDHC_CA0R) 8-bit Support for Embedded Device Mask */
#define SDHC_CA0R_ED8SUP(value)               (SDHC_CA0R_ED8SUP_Msk & ((value) << SDHC_CA0R_ED8SUP_Pos))
#define   SDHC_CA0R_ED8SUP_NO_Val             _U_(0x0)                                             /**< (SDHC_CA0R) 8-bit Bus Width not Supported  */
#define   SDHC_CA0R_ED8SUP_YES_Val            _U_(0x1)                                             /**< (SDHC_CA0R) 8-bit Bus Width Supported  */
#define SDHC_CA0R_ED8SUP_NO                   (SDHC_CA0R_ED8SUP_NO_Val << SDHC_CA0R_ED8SUP_Pos)    /**< (SDHC_CA0R) 8-bit Bus Width not Supported Position  */
#define SDHC_CA0R_ED8SUP_YES                  (SDHC_CA0R_ED8SUP_YES_Val << SDHC_CA0R_ED8SUP_Pos)   /**< (SDHC_CA0R) 8-bit Bus Width Supported Position  */
#define SDHC_CA0R_ADMA2SUP_Pos                _U_(19)                                              /**< (SDHC_CA0R) ADMA2 Support Position */
#define SDHC_CA0R_ADMA2SUP_Msk                (_U_(0x1) << SDHC_CA0R_ADMA2SUP_Pos)                 /**< (SDHC_CA0R) ADMA2 Support Mask */
#define SDHC_CA0R_ADMA2SUP(value)             (SDHC_CA0R_ADMA2SUP_Msk & ((value) << SDHC_CA0R_ADMA2SUP_Pos))
#define   SDHC_CA0R_ADMA2SUP_NO_Val           _U_(0x0)                                             /**< (SDHC_CA0R) ADMA2 not Supported  */
#define   SDHC_CA0R_ADMA2SUP_YES_Val          _U_(0x1)                                             /**< (SDHC_CA0R) ADMA2 Supported  */
#define SDHC_CA0R_ADMA2SUP_NO                 (SDHC_CA0R_ADMA2SUP_NO_Val << SDHC_CA0R_ADMA2SUP_Pos) /**< (SDHC_CA0R) ADMA2 not Supported Position  */
#define SDHC_CA0R_ADMA2SUP_YES                (SDHC_CA0R_ADMA2SUP_YES_Val << SDHC_CA0R_ADMA2SUP_Pos) /**< (SDHC_CA0R) ADMA2 Supported Position  */
#define SDHC_CA0R_HSSUP_Pos                   _U_(21)                                              /**< (SDHC_CA0R) High Speed Support Position */
#define SDHC_CA0R_HSSUP_Msk                   (_U_(0x1) << SDHC_CA0R_HSSUP_Pos)                    /**< (SDHC_CA0R) High Speed Support Mask */
#define SDHC_CA0R_HSSUP(value)                (SDHC_CA0R_HSSUP_Msk & ((value) << SDHC_CA0R_HSSUP_Pos))
#define   SDHC_CA0R_HSSUP_NO_Val              _U_(0x0)                                             /**< (SDHC_CA0R) High Speed not Supported  */
#define   SDHC_CA0R_HSSUP_YES_Val             _U_(0x1)                                             /**< (SDHC_CA0R) High Speed Supported  */
#define SDHC_CA0R_HSSUP_NO                    (SDHC_CA0R_HSSUP_NO_Val << SDHC_CA0R_HSSUP_Pos)      /**< (SDHC_CA0R) High Speed not Supported Position  */
#define SDHC_CA0R_HSSUP_YES                   (SDHC_CA0R_HSSUP_YES_Val << SDHC_CA0R_HSSUP_Pos)     /**< (SDHC_CA0R) High Speed Supported Position  */
#define SDHC_CA0R_SDMASUP_Pos                 _U_(22)                                              /**< (SDHC_CA0R) SDMA Support Position */
#define SDHC_CA0R_SDMASUP_Msk                 (_U_(0x1) << SDHC_CA0R_SDMASUP_Pos)                  /**< (SDHC_CA0R) SDMA Support Mask */
#define SDHC_CA0R_SDMASUP(value)              (SDHC_CA0R_SDMASUP_Msk & ((value) << SDHC_CA0R_SDMASUP_Pos))
#define   SDHC_CA0R_SDMASUP_NO_Val            _U_(0x0)                                             /**< (SDHC_CA0R) SDMA not Supported  */
#define   SDHC_CA0R_SDMASUP_YES_Val           _U_(0x1)                                             /**< (SDHC_CA0R) SDMA Supported  */
#define SDHC_CA0R_SDMASUP_NO                  (SDHC_CA0R_SDMASUP_NO_Val << SDHC_CA0R_SDMASUP_Pos)  /**< (SDHC_CA0R) SDMA not Supported Position  */
#define SDHC_CA0R_SDMASUP_YES                 (SDHC_CA0R_SDMASUP_YES_Val << SDHC_CA0R_SDMASUP_Pos) /**< (SDHC_CA0R) SDMA Supported Position  */
#define SDHC_CA0R_SRSUP_Pos                   _U_(23)                                              /**< (SDHC_CA0R) Suspend/Resume Support Position */
#define SDHC_CA0R_SRSUP_Msk                   (_U_(0x1) << SDHC_CA0R_SRSUP_Pos)                    /**< (SDHC_CA0R) Suspend/Resume Support Mask */
#define SDHC_CA0R_SRSUP(value)                (SDHC_CA0R_SRSUP_Msk & ((value) << SDHC_CA0R_SRSUP_Pos))
#define   SDHC_CA0R_SRSUP_NO_Val              _U_(0x0)                                             /**< (SDHC_CA0R) Suspend/Resume not Supported  */
#define   SDHC_CA0R_SRSUP_YES_Val             _U_(0x1)                                             /**< (SDHC_CA0R) Suspend/Resume Supported  */
#define SDHC_CA0R_SRSUP_NO                    (SDHC_CA0R_SRSUP_NO_Val << SDHC_CA0R_SRSUP_Pos)      /**< (SDHC_CA0R) Suspend/Resume not Supported Position  */
#define SDHC_CA0R_SRSUP_YES                   (SDHC_CA0R_SRSUP_YES_Val << SDHC_CA0R_SRSUP_Pos)     /**< (SDHC_CA0R) Suspend/Resume Supported Position  */
#define SDHC_CA0R_V33VSUP_Pos                 _U_(24)                                              /**< (SDHC_CA0R) Voltage Support 3.3V Position */
#define SDHC_CA0R_V33VSUP_Msk                 (_U_(0x1) << SDHC_CA0R_V33VSUP_Pos)                  /**< (SDHC_CA0R) Voltage Support 3.3V Mask */
#define SDHC_CA0R_V33VSUP(value)              (SDHC_CA0R_V33VSUP_Msk & ((value) << SDHC_CA0R_V33VSUP_Pos))
#define   SDHC_CA0R_V33VSUP_NO_Val            _U_(0x0)                                             /**< (SDHC_CA0R) 3.3V Not Supported  */
#define   SDHC_CA0R_V33VSUP_YES_Val           _U_(0x1)                                             /**< (SDHC_CA0R) 3.3V Supported  */
#define SDHC_CA0R_V33VSUP_NO                  (SDHC_CA0R_V33VSUP_NO_Val << SDHC_CA0R_V33VSUP_Pos)  /**< (SDHC_CA0R) 3.3V Not Supported Position  */
#define SDHC_CA0R_V33VSUP_YES                 (SDHC_CA0R_V33VSUP_YES_Val << SDHC_CA0R_V33VSUP_Pos) /**< (SDHC_CA0R) 3.3V Supported Position  */
#define SDHC_CA0R_V30VSUP_Pos                 _U_(25)                                              /**< (SDHC_CA0R) Voltage Support 3.0V Position */
#define SDHC_CA0R_V30VSUP_Msk                 (_U_(0x1) << SDHC_CA0R_V30VSUP_Pos)                  /**< (SDHC_CA0R) Voltage Support 3.0V Mask */
#define SDHC_CA0R_V30VSUP(value)              (SDHC_CA0R_V30VSUP_Msk & ((value) << SDHC_CA0R_V30VSUP_Pos))
#define   SDHC_CA0R_V30VSUP_NO_Val            _U_(0x0)                                             /**< (SDHC_CA0R) 3.0V Not Supported  */
#define   SDHC_CA0R_V30VSUP_YES_Val           _U_(0x1)                                             /**< (SDHC_CA0R) 3.0V Supported  */
#define SDHC_CA0R_V30VSUP_NO                  (SDHC_CA0R_V30VSUP_NO_Val << SDHC_CA0R_V30VSUP_Pos)  /**< (SDHC_CA0R) 3.0V Not Supported Position  */
#define SDHC_CA0R_V30VSUP_YES                 (SDHC_CA0R_V30VSUP_YES_Val << SDHC_CA0R_V30VSUP_Pos) /**< (SDHC_CA0R) 3.0V Supported Position  */
#define SDHC_CA0R_V18VSUP_Pos                 _U_(26)                                              /**< (SDHC_CA0R) Voltage Support 1.8V Position */
#define SDHC_CA0R_V18VSUP_Msk                 (_U_(0x1) << SDHC_CA0R_V18VSUP_Pos)                  /**< (SDHC_CA0R) Voltage Support 1.8V Mask */
#define SDHC_CA0R_V18VSUP(value)              (SDHC_CA0R_V18VSUP_Msk & ((value) << SDHC_CA0R_V18VSUP_Pos))
#define   SDHC_CA0R_V18VSUP_NO_Val            _U_(0x0)                                             /**< (SDHC_CA0R) 1.8V Not Supported  */
#define   SDHC_CA0R_V18VSUP_YES_Val           _U_(0x1)                                             /**< (SDHC_CA0R) 1.8V Supported  */
#define SDHC_CA0R_V18VSUP_NO                  (SDHC_CA0R_V18VSUP_NO_Val << SDHC_CA0R_V18VSUP_Pos)  /**< (SDHC_CA0R) 1.8V Not Supported Position  */
#define SDHC_CA0R_V18VSUP_YES                 (SDHC_CA0R_V18VSUP_YES_Val << SDHC_CA0R_V18VSUP_Pos) /**< (SDHC_CA0R) 1.8V Supported Position  */
#define SDHC_CA0R_SB64SUP_Pos                 _U_(28)                                              /**< (SDHC_CA0R) 64-Bit System Bus Support Position */
#define SDHC_CA0R_SB64SUP_Msk                 (_U_(0x1) << SDHC_CA0R_SB64SUP_Pos)                  /**< (SDHC_CA0R) 64-Bit System Bus Support Mask */
#define SDHC_CA0R_SB64SUP(value)              (SDHC_CA0R_SB64SUP_Msk & ((value) << SDHC_CA0R_SB64SUP_Pos))
#define   SDHC_CA0R_SB64SUP_NO_Val            _U_(0x0)                                             /**< (SDHC_CA0R) 32-bit Address Descriptors and System Bus  */
#define   SDHC_CA0R_SB64SUP_YES_Val           _U_(0x1)                                             /**< (SDHC_CA0R) 64-bit Address Descriptors and System Bus  */
#define SDHC_CA0R_SB64SUP_NO                  (SDHC_CA0R_SB64SUP_NO_Val << SDHC_CA0R_SB64SUP_Pos)  /**< (SDHC_CA0R) 32-bit Address Descriptors and System Bus Position  */
#define SDHC_CA0R_SB64SUP_YES                 (SDHC_CA0R_SB64SUP_YES_Val << SDHC_CA0R_SB64SUP_Pos) /**< (SDHC_CA0R) 64-bit Address Descriptors and System Bus Position  */
#define SDHC_CA0R_ASINTSUP_Pos                _U_(29)                                              /**< (SDHC_CA0R) Asynchronous Interrupt Support Position */
#define SDHC_CA0R_ASINTSUP_Msk                (_U_(0x1) << SDHC_CA0R_ASINTSUP_Pos)                 /**< (SDHC_CA0R) Asynchronous Interrupt Support Mask */
#define SDHC_CA0R_ASINTSUP(value)             (SDHC_CA0R_ASINTSUP_Msk & ((value) << SDHC_CA0R_ASINTSUP_Pos))
#define   SDHC_CA0R_ASINTSUP_NO_Val           _U_(0x0)                                             /**< (SDHC_CA0R) Asynchronous Interrupt not Supported  */
#define   SDHC_CA0R_ASINTSUP_YES_Val          _U_(0x1)                                             /**< (SDHC_CA0R) Asynchronous Interrupt supported  */
#define SDHC_CA0R_ASINTSUP_NO                 (SDHC_CA0R_ASINTSUP_NO_Val << SDHC_CA0R_ASINTSUP_Pos) /**< (SDHC_CA0R) Asynchronous Interrupt not Supported Position  */
#define SDHC_CA0R_ASINTSUP_YES                (SDHC_CA0R_ASINTSUP_YES_Val << SDHC_CA0R_ASINTSUP_Pos) /**< (SDHC_CA0R) Asynchronous Interrupt supported Position  */
#define SDHC_CA0R_SLTYPE_Pos                  _U_(30)                                              /**< (SDHC_CA0R) Slot Type Position */
#define SDHC_CA0R_SLTYPE_Msk                  (_U_(0x3) << SDHC_CA0R_SLTYPE_Pos)                   /**< (SDHC_CA0R) Slot Type Mask */
#define SDHC_CA0R_SLTYPE(value)               (SDHC_CA0R_SLTYPE_Msk & ((value) << SDHC_CA0R_SLTYPE_Pos))
#define   SDHC_CA0R_SLTYPE_REMOVABLE_Val      _U_(0x0)                                             /**< (SDHC_CA0R) Removable Card Slot  */
#define   SDHC_CA0R_SLTYPE_EMBEDDED_Val       _U_(0x1)                                             /**< (SDHC_CA0R) Embedded Slot for One Device  */
#define SDHC_CA0R_SLTYPE_REMOVABLE            (SDHC_CA0R_SLTYPE_REMOVABLE_Val << SDHC_CA0R_SLTYPE_Pos) /**< (SDHC_CA0R) Removable Card Slot Position  */
#define SDHC_CA0R_SLTYPE_EMBEDDED             (SDHC_CA0R_SLTYPE_EMBEDDED_Val << SDHC_CA0R_SLTYPE_Pos) /**< (SDHC_CA0R) Embedded Slot for One Device Position  */
#define SDHC_CA0R_Msk                         _U_(0xF7EFFFBF)                                      /**< (SDHC_CA0R) Register Mask  */


/* -------- SDHC_CA1R : (SDHC Offset: 0x44) ( R/ 32) Capabilities 1 -------- */
#define SDHC_CA1R_RESETVALUE                  _U_(0x70)                                            /**<  (SDHC_CA1R) Capabilities 1  Reset Value */

#define SDHC_CA1R_SDR50SUP_Pos                _U_(0)                                               /**< (SDHC_CA1R) SDR50 Support Position */
#define SDHC_CA1R_SDR50SUP_Msk                (_U_(0x1) << SDHC_CA1R_SDR50SUP_Pos)                 /**< (SDHC_CA1R) SDR50 Support Mask */
#define SDHC_CA1R_SDR50SUP(value)             (SDHC_CA1R_SDR50SUP_Msk & ((value) << SDHC_CA1R_SDR50SUP_Pos))
#define   SDHC_CA1R_SDR50SUP_NO_Val           _U_(0x0)                                             /**< (SDHC_CA1R) SDR50 is Not Supported  */
#define   SDHC_CA1R_SDR50SUP_YES_Val          _U_(0x1)                                             /**< (SDHC_CA1R) SDR50 is Supported  */
#define SDHC_CA1R_SDR50SUP_NO                 (SDHC_CA1R_SDR50SUP_NO_Val << SDHC_CA1R_SDR50SUP_Pos) /**< (SDHC_CA1R) SDR50 is Not Supported Position  */
#define SDHC_CA1R_SDR50SUP_YES                (SDHC_CA1R_SDR50SUP_YES_Val << SDHC_CA1R_SDR50SUP_Pos) /**< (SDHC_CA1R) SDR50 is Supported Position  */
#define SDHC_CA1R_SDR104SUP_Pos               _U_(1)                                               /**< (SDHC_CA1R) SDR104 Support Position */
#define SDHC_CA1R_SDR104SUP_Msk               (_U_(0x1) << SDHC_CA1R_SDR104SUP_Pos)                /**< (SDHC_CA1R) SDR104 Support Mask */
#define SDHC_CA1R_SDR104SUP(value)            (SDHC_CA1R_SDR104SUP_Msk & ((value) << SDHC_CA1R_SDR104SUP_Pos))
#define   SDHC_CA1R_SDR104SUP_NO_Val          _U_(0x0)                                             /**< (SDHC_CA1R) SDR104 is Not Supported  */
#define   SDHC_CA1R_SDR104SUP_YES_Val         _U_(0x1)                                             /**< (SDHC_CA1R) SDR104 is Supported  */
#define SDHC_CA1R_SDR104SUP_NO                (SDHC_CA1R_SDR104SUP_NO_Val << SDHC_CA1R_SDR104SUP_Pos) /**< (SDHC_CA1R) SDR104 is Not Supported Position  */
#define SDHC_CA1R_SDR104SUP_YES               (SDHC_CA1R_SDR104SUP_YES_Val << SDHC_CA1R_SDR104SUP_Pos) /**< (SDHC_CA1R) SDR104 is Supported Position  */
#define SDHC_CA1R_DDR50SUP_Pos                _U_(2)                                               /**< (SDHC_CA1R) DDR50 Support Position */
#define SDHC_CA1R_DDR50SUP_Msk                (_U_(0x1) << SDHC_CA1R_DDR50SUP_Pos)                 /**< (SDHC_CA1R) DDR50 Support Mask */
#define SDHC_CA1R_DDR50SUP(value)             (SDHC_CA1R_DDR50SUP_Msk & ((value) << SDHC_CA1R_DDR50SUP_Pos))
#define   SDHC_CA1R_DDR50SUP_NO_Val           _U_(0x0)                                             /**< (SDHC_CA1R) DDR50 is Not Supported  */
#define   SDHC_CA1R_DDR50SUP_YES_Val          _U_(0x1)                                             /**< (SDHC_CA1R) DDR50 is Supported  */
#define SDHC_CA1R_DDR50SUP_NO                 (SDHC_CA1R_DDR50SUP_NO_Val << SDHC_CA1R_DDR50SUP_Pos) /**< (SDHC_CA1R) DDR50 is Not Supported Position  */
#define SDHC_CA1R_DDR50SUP_YES                (SDHC_CA1R_DDR50SUP_YES_Val << SDHC_CA1R_DDR50SUP_Pos) /**< (SDHC_CA1R) DDR50 is Supported Position  */
#define SDHC_CA1R_DRVASUP_Pos                 _U_(4)                                               /**< (SDHC_CA1R) Driver Type A Support Position */
#define SDHC_CA1R_DRVASUP_Msk                 (_U_(0x1) << SDHC_CA1R_DRVASUP_Pos)                  /**< (SDHC_CA1R) Driver Type A Support Mask */
#define SDHC_CA1R_DRVASUP(value)              (SDHC_CA1R_DRVASUP_Msk & ((value) << SDHC_CA1R_DRVASUP_Pos))
#define   SDHC_CA1R_DRVASUP_NO_Val            _U_(0x0)                                             /**< (SDHC_CA1R) Driver Type A is Not Supported  */
#define   SDHC_CA1R_DRVASUP_YES_Val           _U_(0x1)                                             /**< (SDHC_CA1R) Driver Type A is Supported  */
#define SDHC_CA1R_DRVASUP_NO                  (SDHC_CA1R_DRVASUP_NO_Val << SDHC_CA1R_DRVASUP_Pos)  /**< (SDHC_CA1R) Driver Type A is Not Supported Position  */
#define SDHC_CA1R_DRVASUP_YES                 (SDHC_CA1R_DRVASUP_YES_Val << SDHC_CA1R_DRVASUP_Pos) /**< (SDHC_CA1R) Driver Type A is Supported Position  */
#define SDHC_CA1R_DRVCSUP_Pos                 _U_(5)                                               /**< (SDHC_CA1R) Driver Type C Support Position */
#define SDHC_CA1R_DRVCSUP_Msk                 (_U_(0x1) << SDHC_CA1R_DRVCSUP_Pos)                  /**< (SDHC_CA1R) Driver Type C Support Mask */
#define SDHC_CA1R_DRVCSUP(value)              (SDHC_CA1R_DRVCSUP_Msk & ((value) << SDHC_CA1R_DRVCSUP_Pos))
#define   SDHC_CA1R_DRVCSUP_NO_Val            _U_(0x0)                                             /**< (SDHC_CA1R) Driver Type C is Not Supported  */
#define   SDHC_CA1R_DRVCSUP_YES_Val           _U_(0x1)                                             /**< (SDHC_CA1R) Driver Type C is Supported  */
#define SDHC_CA1R_DRVCSUP_NO                  (SDHC_CA1R_DRVCSUP_NO_Val << SDHC_CA1R_DRVCSUP_Pos)  /**< (SDHC_CA1R) Driver Type C is Not Supported Position  */
#define SDHC_CA1R_DRVCSUP_YES                 (SDHC_CA1R_DRVCSUP_YES_Val << SDHC_CA1R_DRVCSUP_Pos) /**< (SDHC_CA1R) Driver Type C is Supported Position  */
#define SDHC_CA1R_DRVDSUP_Pos                 _U_(6)                                               /**< (SDHC_CA1R) Driver Type D Support Position */
#define SDHC_CA1R_DRVDSUP_Msk                 (_U_(0x1) << SDHC_CA1R_DRVDSUP_Pos)                  /**< (SDHC_CA1R) Driver Type D Support Mask */
#define SDHC_CA1R_DRVDSUP(value)              (SDHC_CA1R_DRVDSUP_Msk & ((value) << SDHC_CA1R_DRVDSUP_Pos))
#define   SDHC_CA1R_DRVDSUP_NO_Val            _U_(0x0)                                             /**< (SDHC_CA1R) Driver Type D is Not Supported  */
#define   SDHC_CA1R_DRVDSUP_YES_Val           _U_(0x1)                                             /**< (SDHC_CA1R) Driver Type D is Supported  */
#define SDHC_CA1R_DRVDSUP_NO                  (SDHC_CA1R_DRVDSUP_NO_Val << SDHC_CA1R_DRVDSUP_Pos)  /**< (SDHC_CA1R) Driver Type D is Not Supported Position  */
#define SDHC_CA1R_DRVDSUP_YES                 (SDHC_CA1R_DRVDSUP_YES_Val << SDHC_CA1R_DRVDSUP_Pos) /**< (SDHC_CA1R) Driver Type D is Supported Position  */
#define SDHC_CA1R_TCNTRT_Pos                  _U_(8)                                               /**< (SDHC_CA1R) Timer Count for Re-Tuning Position */
#define SDHC_CA1R_TCNTRT_Msk                  (_U_(0xF) << SDHC_CA1R_TCNTRT_Pos)                   /**< (SDHC_CA1R) Timer Count for Re-Tuning Mask */
#define SDHC_CA1R_TCNTRT(value)               (SDHC_CA1R_TCNTRT_Msk & ((value) << SDHC_CA1R_TCNTRT_Pos))
#define   SDHC_CA1R_TCNTRT_DISABLED_Val       _U_(0x0)                                             /**< (SDHC_CA1R) Re-Tuning Timer disabled  */
#define   SDHC_CA1R_TCNTRT_1S_Val             _U_(0x1)                                             /**< (SDHC_CA1R) 1 second  */
#define   SDHC_CA1R_TCNTRT_2S_Val             _U_(0x2)                                             /**< (SDHC_CA1R) 2 seconds  */
#define   SDHC_CA1R_TCNTRT_4S_Val             _U_(0x3)                                             /**< (SDHC_CA1R) 4 seconds  */
#define   SDHC_CA1R_TCNTRT_8S_Val             _U_(0x4)                                             /**< (SDHC_CA1R) 8 seconds  */
#define   SDHC_CA1R_TCNTRT_16S_Val            _U_(0x5)                                             /**< (SDHC_CA1R) 16 seconds  */
#define   SDHC_CA1R_TCNTRT_32S_Val            _U_(0x6)                                             /**< (SDHC_CA1R) 32 seconds  */
#define   SDHC_CA1R_TCNTRT_64S_Val            _U_(0x7)                                             /**< (SDHC_CA1R) 64 seconds  */
#define   SDHC_CA1R_TCNTRT_128S_Val           _U_(0x8)                                             /**< (SDHC_CA1R) 128 seconds  */
#define   SDHC_CA1R_TCNTRT_256S_Val           _U_(0x9)                                             /**< (SDHC_CA1R) 256 seconds  */
#define   SDHC_CA1R_TCNTRT_512S_Val           _U_(0xA)                                             /**< (SDHC_CA1R) 512 seconds  */
#define   SDHC_CA1R_TCNTRT_1024S_Val          _U_(0xB)                                             /**< (SDHC_CA1R) 1024 seconds  */
#define   SDHC_CA1R_TCNTRT_OTHER_Val          _U_(0xF)                                             /**< (SDHC_CA1R) Get information from other source  */
#define SDHC_CA1R_TCNTRT_DISABLED             (SDHC_CA1R_TCNTRT_DISABLED_Val << SDHC_CA1R_TCNTRT_Pos) /**< (SDHC_CA1R) Re-Tuning Timer disabled Position  */
#define SDHC_CA1R_TCNTRT_1S                   (SDHC_CA1R_TCNTRT_1S_Val << SDHC_CA1R_TCNTRT_Pos)    /**< (SDHC_CA1R) 1 second Position  */
#define SDHC_CA1R_TCNTRT_2S                   (SDHC_CA1R_TCNTRT_2S_Val << SDHC_CA1R_TCNTRT_Pos)    /**< (SDHC_CA1R) 2 seconds Position  */
#define SDHC_CA1R_TCNTRT_4S                   (SDHC_CA1R_TCNTRT_4S_Val << SDHC_CA1R_TCNTRT_Pos)    /**< (SDHC_CA1R) 4 seconds Position  */
#define SDHC_CA1R_TCNTRT_8S                   (SDHC_CA1R_TCNTRT_8S_Val << SDHC_CA1R_TCNTRT_Pos)    /**< (SDHC_CA1R) 8 seconds Position  */
#define SDHC_CA1R_TCNTRT_16S                  (SDHC_CA1R_TCNTRT_16S_Val << SDHC_CA1R_TCNTRT_Pos)   /**< (SDHC_CA1R) 16 seconds Position  */
#define SDHC_CA1R_TCNTRT_32S                  (SDHC_CA1R_TCNTRT_32S_Val << SDHC_CA1R_TCNTRT_Pos)   /**< (SDHC_CA1R) 32 seconds Position  */
#define SDHC_CA1R_TCNTRT_64S                  (SDHC_CA1R_TCNTRT_64S_Val << SDHC_CA1R_TCNTRT_Pos)   /**< (SDHC_CA1R) 64 seconds Position  */
#define SDHC_CA1R_TCNTRT_128S                 (SDHC_CA1R_TCNTRT_128S_Val << SDHC_CA1R_TCNTRT_Pos)  /**< (SDHC_CA1R) 128 seconds Position  */
#define SDHC_CA1R_TCNTRT_256S                 (SDHC_CA1R_TCNTRT_256S_Val << SDHC_CA1R_TCNTRT_Pos)  /**< (SDHC_CA1R) 256 seconds Position  */
#define SDHC_CA1R_TCNTRT_512S                 (SDHC_CA1R_TCNTRT_512S_Val << SDHC_CA1R_TCNTRT_Pos)  /**< (SDHC_CA1R) 512 seconds Position  */
#define SDHC_CA1R_TCNTRT_1024S                (SDHC_CA1R_TCNTRT_1024S_Val << SDHC_CA1R_TCNTRT_Pos) /**< (SDHC_CA1R) 1024 seconds Position  */
#define SDHC_CA1R_TCNTRT_OTHER                (SDHC_CA1R_TCNTRT_OTHER_Val << SDHC_CA1R_TCNTRT_Pos) /**< (SDHC_CA1R) Get information from other source Position  */
#define SDHC_CA1R_TSDR50_Pos                  _U_(13)                                              /**< (SDHC_CA1R) Use Tuning for SDR50 Position */
#define SDHC_CA1R_TSDR50_Msk                  (_U_(0x1) << SDHC_CA1R_TSDR50_Pos)                   /**< (SDHC_CA1R) Use Tuning for SDR50 Mask */
#define SDHC_CA1R_TSDR50(value)               (SDHC_CA1R_TSDR50_Msk & ((value) << SDHC_CA1R_TSDR50_Pos))
#define   SDHC_CA1R_TSDR50_NO_Val             _U_(0x0)                                             /**< (SDHC_CA1R) SDR50 does not require tuning  */
#define   SDHC_CA1R_TSDR50_YES_Val            _U_(0x1)                                             /**< (SDHC_CA1R) SDR50 requires tuning  */
#define SDHC_CA1R_TSDR50_NO                   (SDHC_CA1R_TSDR50_NO_Val << SDHC_CA1R_TSDR50_Pos)    /**< (SDHC_CA1R) SDR50 does not require tuning Position  */
#define SDHC_CA1R_TSDR50_YES                  (SDHC_CA1R_TSDR50_YES_Val << SDHC_CA1R_TSDR50_Pos)   /**< (SDHC_CA1R) SDR50 requires tuning Position  */
#define SDHC_CA1R_CLKMULT_Pos                 _U_(16)                                              /**< (SDHC_CA1R) Clock Multiplier Position */
#define SDHC_CA1R_CLKMULT_Msk                 (_U_(0xFF) << SDHC_CA1R_CLKMULT_Pos)                 /**< (SDHC_CA1R) Clock Multiplier Mask */
#define SDHC_CA1R_CLKMULT(value)              (SDHC_CA1R_CLKMULT_Msk & ((value) << SDHC_CA1R_CLKMULT_Pos))
#define   SDHC_CA1R_CLKMULT_NO_Val            _U_(0x0)                                             /**< (SDHC_CA1R) Clock Multiplier is Not Supported  */
#define SDHC_CA1R_CLKMULT_NO                  (SDHC_CA1R_CLKMULT_NO_Val << SDHC_CA1R_CLKMULT_Pos)  /**< (SDHC_CA1R) Clock Multiplier is Not Supported Position  */
#define SDHC_CA1R_Msk                         _U_(0x00FF2F77)                                      /**< (SDHC_CA1R) Register Mask  */

#define SDHC_CA1R_TSDR_Pos                    _U_(13)                                              /**< (SDHC_CA1R Position) Use Tuning for SDR5x */
#define SDHC_CA1R_TSDR_Msk                    (_U_(0x1) << SDHC_CA1R_TSDR_Pos)                     /**< (SDHC_CA1R Mask) TSDR */
#define SDHC_CA1R_TSDR(value)                 (SDHC_CA1R_TSDR_Msk & ((value) << SDHC_CA1R_TSDR_Pos)) 

/* -------- SDHC_MCCAR : (SDHC Offset: 0x48) ( R/ 32) Maximum Current Capabilities -------- */
#define SDHC_MCCAR_RESETVALUE                 _U_(0x00)                                            /**<  (SDHC_MCCAR) Maximum Current Capabilities  Reset Value */

#define SDHC_MCCAR_MAXCUR33V_Pos              _U_(0)                                               /**< (SDHC_MCCAR) Maximum Current for 3.3V Position */
#define SDHC_MCCAR_MAXCUR33V_Msk              (_U_(0xFF) << SDHC_MCCAR_MAXCUR33V_Pos)              /**< (SDHC_MCCAR) Maximum Current for 3.3V Mask */
#define SDHC_MCCAR_MAXCUR33V(value)           (SDHC_MCCAR_MAXCUR33V_Msk & ((value) << SDHC_MCCAR_MAXCUR33V_Pos))
#define   SDHC_MCCAR_MAXCUR33V_OTHER_Val      _U_(0x0)                                             /**< (SDHC_MCCAR) Get information via another method  */
#define   SDHC_MCCAR_MAXCUR33V_4MA_Val        _U_(0x1)                                             /**< (SDHC_MCCAR) 4mA  */
#define   SDHC_MCCAR_MAXCUR33V_8MA_Val        _U_(0x2)                                             /**< (SDHC_MCCAR) 8mA  */
#define   SDHC_MCCAR_MAXCUR33V_12MA_Val       _U_(0x3)                                             /**< (SDHC_MCCAR) 12mA  */
#define SDHC_MCCAR_MAXCUR33V_OTHER            (SDHC_MCCAR_MAXCUR33V_OTHER_Val << SDHC_MCCAR_MAXCUR33V_Pos) /**< (SDHC_MCCAR) Get information via another method Position  */
#define SDHC_MCCAR_MAXCUR33V_4MA              (SDHC_MCCAR_MAXCUR33V_4MA_Val << SDHC_MCCAR_MAXCUR33V_Pos) /**< (SDHC_MCCAR) 4mA Position  */
#define SDHC_MCCAR_MAXCUR33V_8MA              (SDHC_MCCAR_MAXCUR33V_8MA_Val << SDHC_MCCAR_MAXCUR33V_Pos) /**< (SDHC_MCCAR) 8mA Position  */
#define SDHC_MCCAR_MAXCUR33V_12MA             (SDHC_MCCAR_MAXCUR33V_12MA_Val << SDHC_MCCAR_MAXCUR33V_Pos) /**< (SDHC_MCCAR) 12mA Position  */
#define SDHC_MCCAR_MAXCUR30V_Pos              _U_(8)                                               /**< (SDHC_MCCAR) Maximum Current for 3.0V Position */
#define SDHC_MCCAR_MAXCUR30V_Msk              (_U_(0xFF) << SDHC_MCCAR_MAXCUR30V_Pos)              /**< (SDHC_MCCAR) Maximum Current for 3.0V Mask */
#define SDHC_MCCAR_MAXCUR30V(value)           (SDHC_MCCAR_MAXCUR30V_Msk & ((value) << SDHC_MCCAR_MAXCUR30V_Pos))
#define   SDHC_MCCAR_MAXCUR30V_OTHER_Val      _U_(0x0)                                             /**< (SDHC_MCCAR) Get information via another method  */
#define   SDHC_MCCAR_MAXCUR30V_4MA_Val        _U_(0x1)                                             /**< (SDHC_MCCAR) 4mA  */
#define   SDHC_MCCAR_MAXCUR30V_8MA_Val        _U_(0x2)                                             /**< (SDHC_MCCAR) 8mA  */
#define   SDHC_MCCAR_MAXCUR30V_12MA_Val       _U_(0x3)                                             /**< (SDHC_MCCAR) 12mA  */
#define SDHC_MCCAR_MAXCUR30V_OTHER            (SDHC_MCCAR_MAXCUR30V_OTHER_Val << SDHC_MCCAR_MAXCUR30V_Pos) /**< (SDHC_MCCAR) Get information via another method Position  */
#define SDHC_MCCAR_MAXCUR30V_4MA              (SDHC_MCCAR_MAXCUR30V_4MA_Val << SDHC_MCCAR_MAXCUR30V_Pos) /**< (SDHC_MCCAR) 4mA Position  */
#define SDHC_MCCAR_MAXCUR30V_8MA              (SDHC_MCCAR_MAXCUR30V_8MA_Val << SDHC_MCCAR_MAXCUR30V_Pos) /**< (SDHC_MCCAR) 8mA Position  */
#define SDHC_MCCAR_MAXCUR30V_12MA             (SDHC_MCCAR_MAXCUR30V_12MA_Val << SDHC_MCCAR_MAXCUR30V_Pos) /**< (SDHC_MCCAR) 12mA Position  */
#define SDHC_MCCAR_MAXCUR18V_Pos              _U_(16)                                              /**< (SDHC_MCCAR) Maximum Current for 1.8V Position */
#define SDHC_MCCAR_MAXCUR18V_Msk              (_U_(0xFF) << SDHC_MCCAR_MAXCUR18V_Pos)              /**< (SDHC_MCCAR) Maximum Current for 1.8V Mask */
#define SDHC_MCCAR_MAXCUR18V(value)           (SDHC_MCCAR_MAXCUR18V_Msk & ((value) << SDHC_MCCAR_MAXCUR18V_Pos))
#define   SDHC_MCCAR_MAXCUR18V_OTHER_Val      _U_(0x0)                                             /**< (SDHC_MCCAR) Get information via another method  */
#define   SDHC_MCCAR_MAXCUR18V_4MA_Val        _U_(0x1)                                             /**< (SDHC_MCCAR) 4mA  */
#define   SDHC_MCCAR_MAXCUR18V_8MA_Val        _U_(0x2)                                             /**< (SDHC_MCCAR) 8mA  */
#define   SDHC_MCCAR_MAXCUR18V_12MA_Val       _U_(0x3)                                             /**< (SDHC_MCCAR) 12mA  */
#define SDHC_MCCAR_MAXCUR18V_OTHER            (SDHC_MCCAR_MAXCUR18V_OTHER_Val << SDHC_MCCAR_MAXCUR18V_Pos) /**< (SDHC_MCCAR) Get information via another method Position  */
#define SDHC_MCCAR_MAXCUR18V_4MA              (SDHC_MCCAR_MAXCUR18V_4MA_Val << SDHC_MCCAR_MAXCUR18V_Pos) /**< (SDHC_MCCAR) 4mA Position  */
#define SDHC_MCCAR_MAXCUR18V_8MA              (SDHC_MCCAR_MAXCUR18V_8MA_Val << SDHC_MCCAR_MAXCUR18V_Pos) /**< (SDHC_MCCAR) 8mA Position  */
#define SDHC_MCCAR_MAXCUR18V_12MA             (SDHC_MCCAR_MAXCUR18V_12MA_Val << SDHC_MCCAR_MAXCUR18V_Pos) /**< (SDHC_MCCAR) 12mA Position  */
#define SDHC_MCCAR_Msk                        _U_(0x00FFFFFF)                                      /**< (SDHC_MCCAR) Register Mask  */


/* -------- SDHC_FERACES : (SDHC Offset: 0x50) ( /W 16) Force Event for Auto CMD Error Status -------- */
#define SDHC_FERACES_RESETVALUE               _U_(0x00)                                            /**<  (SDHC_FERACES) Force Event for Auto CMD Error Status  Reset Value */

#define SDHC_FERACES_ACMD12NE_Pos             _U_(0)                                               /**< (SDHC_FERACES) Force Event for Auto CMD12 Not Executed Position */
#define SDHC_FERACES_ACMD12NE_Msk             (_U_(0x1) << SDHC_FERACES_ACMD12NE_Pos)              /**< (SDHC_FERACES) Force Event for Auto CMD12 Not Executed Mask */
#define SDHC_FERACES_ACMD12NE(value)          (SDHC_FERACES_ACMD12NE_Msk & ((value) << SDHC_FERACES_ACMD12NE_Pos))
#define   SDHC_FERACES_ACMD12NE_NO_Val        _U_(0x0)                                             /**< (SDHC_FERACES) No Interrupt  */
#define   SDHC_FERACES_ACMD12NE_YES_Val       _U_(0x1)                                             /**< (SDHC_FERACES) Interrupt is generated  */
#define SDHC_FERACES_ACMD12NE_NO              (SDHC_FERACES_ACMD12NE_NO_Val << SDHC_FERACES_ACMD12NE_Pos) /**< (SDHC_FERACES) No Interrupt Position  */
#define SDHC_FERACES_ACMD12NE_YES             (SDHC_FERACES_ACMD12NE_YES_Val << SDHC_FERACES_ACMD12NE_Pos) /**< (SDHC_FERACES) Interrupt is generated Position  */
#define SDHC_FERACES_ACMDTEO_Pos              _U_(1)                                               /**< (SDHC_FERACES) Force Event for Auto CMD Timeout Error Position */
#define SDHC_FERACES_ACMDTEO_Msk              (_U_(0x1) << SDHC_FERACES_ACMDTEO_Pos)               /**< (SDHC_FERACES) Force Event for Auto CMD Timeout Error Mask */
#define SDHC_FERACES_ACMDTEO(value)           (SDHC_FERACES_ACMDTEO_Msk & ((value) << SDHC_FERACES_ACMDTEO_Pos))
#define   SDHC_FERACES_ACMDTEO_NO_Val         _U_(0x0)                                             /**< (SDHC_FERACES) No Interrupt  */
#define   SDHC_FERACES_ACMDTEO_YES_Val        _U_(0x1)                                             /**< (SDHC_FERACES) Interrupt is generated  */
#define SDHC_FERACES_ACMDTEO_NO               (SDHC_FERACES_ACMDTEO_NO_Val << SDHC_FERACES_ACMDTEO_Pos) /**< (SDHC_FERACES) No Interrupt Position  */
#define SDHC_FERACES_ACMDTEO_YES              (SDHC_FERACES_ACMDTEO_YES_Val << SDHC_FERACES_ACMDTEO_Pos) /**< (SDHC_FERACES) Interrupt is generated Position  */
#define SDHC_FERACES_ACMDCRC_Pos              _U_(2)                                               /**< (SDHC_FERACES) Force Event for Auto CMD CRC Error Position */
#define SDHC_FERACES_ACMDCRC_Msk              (_U_(0x1) << SDHC_FERACES_ACMDCRC_Pos)               /**< (SDHC_FERACES) Force Event for Auto CMD CRC Error Mask */
#define SDHC_FERACES_ACMDCRC(value)           (SDHC_FERACES_ACMDCRC_Msk & ((value) << SDHC_FERACES_ACMDCRC_Pos))
#define   SDHC_FERACES_ACMDCRC_NO_Val         _U_(0x0)                                             /**< (SDHC_FERACES) No Interrupt  */
#define   SDHC_FERACES_ACMDCRC_YES_Val        _U_(0x1)                                             /**< (SDHC_FERACES) Interrupt is generated  */
#define SDHC_FERACES_ACMDCRC_NO               (SDHC_FERACES_ACMDCRC_NO_Val << SDHC_FERACES_ACMDCRC_Pos) /**< (SDHC_FERACES) No Interrupt Position  */
#define SDHC_FERACES_ACMDCRC_YES              (SDHC_FERACES_ACMDCRC_YES_Val << SDHC_FERACES_ACMDCRC_Pos) /**< (SDHC_FERACES) Interrupt is generated Position  */
#define SDHC_FERACES_ACMDEND_Pos              _U_(3)                                               /**< (SDHC_FERACES) Force Event for Auto CMD End Bit Error Position */
#define SDHC_FERACES_ACMDEND_Msk              (_U_(0x1) << SDHC_FERACES_ACMDEND_Pos)               /**< (SDHC_FERACES) Force Event for Auto CMD End Bit Error Mask */
#define SDHC_FERACES_ACMDEND(value)           (SDHC_FERACES_ACMDEND_Msk & ((value) << SDHC_FERACES_ACMDEND_Pos))
#define   SDHC_FERACES_ACMDEND_NO_Val         _U_(0x0)                                             /**< (SDHC_FERACES) No Interrupt  */
#define   SDHC_FERACES_ACMDEND_YES_Val        _U_(0x1)                                             /**< (SDHC_FERACES) Interrupt is generated  */
#define SDHC_FERACES_ACMDEND_NO               (SDHC_FERACES_ACMDEND_NO_Val << SDHC_FERACES_ACMDEND_Pos) /**< (SDHC_FERACES) No Interrupt Position  */
#define SDHC_FERACES_ACMDEND_YES              (SDHC_FERACES_ACMDEND_YES_Val << SDHC_FERACES_ACMDEND_Pos) /**< (SDHC_FERACES) Interrupt is generated Position  */
#define SDHC_FERACES_ACMDIDX_Pos              _U_(4)                                               /**< (SDHC_FERACES) Force Event for Auto CMD Index Error Position */
#define SDHC_FERACES_ACMDIDX_Msk              (_U_(0x1) << SDHC_FERACES_ACMDIDX_Pos)               /**< (SDHC_FERACES) Force Event for Auto CMD Index Error Mask */
#define SDHC_FERACES_ACMDIDX(value)           (SDHC_FERACES_ACMDIDX_Msk & ((value) << SDHC_FERACES_ACMDIDX_Pos))
#define   SDHC_FERACES_ACMDIDX_NO_Val         _U_(0x0)                                             /**< (SDHC_FERACES) No Interrupt  */
#define   SDHC_FERACES_ACMDIDX_YES_Val        _U_(0x1)                                             /**< (SDHC_FERACES) Interrupt is generated  */
#define SDHC_FERACES_ACMDIDX_NO               (SDHC_FERACES_ACMDIDX_NO_Val << SDHC_FERACES_ACMDIDX_Pos) /**< (SDHC_FERACES) No Interrupt Position  */
#define SDHC_FERACES_ACMDIDX_YES              (SDHC_FERACES_ACMDIDX_YES_Val << SDHC_FERACES_ACMDIDX_Pos) /**< (SDHC_FERACES) Interrupt is generated Position  */
#define SDHC_FERACES_CMDNI_Pos                _U_(7)                                               /**< (SDHC_FERACES) Force Event for Command Not Issued By Auto CMD12 Error Position */
#define SDHC_FERACES_CMDNI_Msk                (_U_(0x1) << SDHC_FERACES_CMDNI_Pos)                 /**< (SDHC_FERACES) Force Event for Command Not Issued By Auto CMD12 Error Mask */
#define SDHC_FERACES_CMDNI(value)             (SDHC_FERACES_CMDNI_Msk & ((value) << SDHC_FERACES_CMDNI_Pos))
#define   SDHC_FERACES_CMDNI_NO_Val           _U_(0x0)                                             /**< (SDHC_FERACES) No Interrupt  */
#define   SDHC_FERACES_CMDNI_YES_Val          _U_(0x1)                                             /**< (SDHC_FERACES) Interrupt is generated  */
#define SDHC_FERACES_CMDNI_NO                 (SDHC_FERACES_CMDNI_NO_Val << SDHC_FERACES_CMDNI_Pos) /**< (SDHC_FERACES) No Interrupt Position  */
#define SDHC_FERACES_CMDNI_YES                (SDHC_FERACES_CMDNI_YES_Val << SDHC_FERACES_CMDNI_Pos) /**< (SDHC_FERACES) Interrupt is generated Position  */
#define SDHC_FERACES_Msk                      _U_(0x009F)                                          /**< (SDHC_FERACES) Register Mask  */


/* -------- SDHC_FEREIS : (SDHC Offset: 0x52) ( /W 16) Force Event for Error Interrupt Status -------- */
#define SDHC_FEREIS_RESETVALUE                _U_(0x00)                                            /**<  (SDHC_FEREIS) Force Event for Error Interrupt Status  Reset Value */

#define SDHC_FEREIS_CMDTEO_Pos                _U_(0)                                               /**< (SDHC_FEREIS) Force Event for Command Timeout Error Position */
#define SDHC_FEREIS_CMDTEO_Msk                (_U_(0x1) << SDHC_FEREIS_CMDTEO_Pos)                 /**< (SDHC_FEREIS) Force Event for Command Timeout Error Mask */
#define SDHC_FEREIS_CMDTEO(value)             (SDHC_FEREIS_CMDTEO_Msk & ((value) << SDHC_FEREIS_CMDTEO_Pos))
#define   SDHC_FEREIS_CMDTEO_NO_Val           _U_(0x0)                                             /**< (SDHC_FEREIS) No Interrupt  */
#define   SDHC_FEREIS_CMDTEO_YES_Val          _U_(0x1)                                             /**< (SDHC_FEREIS) Interrupt is generated  */
#define SDHC_FEREIS_CMDTEO_NO                 (SDHC_FEREIS_CMDTEO_NO_Val << SDHC_FEREIS_CMDTEO_Pos) /**< (SDHC_FEREIS) No Interrupt Position  */
#define SDHC_FEREIS_CMDTEO_YES                (SDHC_FEREIS_CMDTEO_YES_Val << SDHC_FEREIS_CMDTEO_Pos) /**< (SDHC_FEREIS) Interrupt is generated Position  */
#define SDHC_FEREIS_CMDCRC_Pos                _U_(1)                                               /**< (SDHC_FEREIS) Force Event for Command CRC Error Position */
#define SDHC_FEREIS_CMDCRC_Msk                (_U_(0x1) << SDHC_FEREIS_CMDCRC_Pos)                 /**< (SDHC_FEREIS) Force Event for Command CRC Error Mask */
#define SDHC_FEREIS_CMDCRC(value)             (SDHC_FEREIS_CMDCRC_Msk & ((value) << SDHC_FEREIS_CMDCRC_Pos))
#define   SDHC_FEREIS_CMDCRC_NO_Val           _U_(0x0)                                             /**< (SDHC_FEREIS) No Interrupt  */
#define   SDHC_FEREIS_CMDCRC_YES_Val          _U_(0x1)                                             /**< (SDHC_FEREIS) Interrupt is generated  */
#define SDHC_FEREIS_CMDCRC_NO                 (SDHC_FEREIS_CMDCRC_NO_Val << SDHC_FEREIS_CMDCRC_Pos) /**< (SDHC_FEREIS) No Interrupt Position  */
#define SDHC_FEREIS_CMDCRC_YES                (SDHC_FEREIS_CMDCRC_YES_Val << SDHC_FEREIS_CMDCRC_Pos) /**< (SDHC_FEREIS) Interrupt is generated Position  */
#define SDHC_FEREIS_CMDEND_Pos                _U_(2)                                               /**< (SDHC_FEREIS) Force Event for Command End Bit Error Position */
#define SDHC_FEREIS_CMDEND_Msk                (_U_(0x1) << SDHC_FEREIS_CMDEND_Pos)                 /**< (SDHC_FEREIS) Force Event for Command End Bit Error Mask */
#define SDHC_FEREIS_CMDEND(value)             (SDHC_FEREIS_CMDEND_Msk & ((value) << SDHC_FEREIS_CMDEND_Pos))
#define   SDHC_FEREIS_CMDEND_NO_Val           _U_(0x0)                                             /**< (SDHC_FEREIS) No Interrupt  */
#define   SDHC_FEREIS_CMDEND_YES_Val          _U_(0x1)                                             /**< (SDHC_FEREIS) Interrupt is generated  */
#define SDHC_FEREIS_CMDEND_NO                 (SDHC_FEREIS_CMDEND_NO_Val << SDHC_FEREIS_CMDEND_Pos) /**< (SDHC_FEREIS) No Interrupt Position  */
#define SDHC_FEREIS_CMDEND_YES                (SDHC_FEREIS_CMDEND_YES_Val << SDHC_FEREIS_CMDEND_Pos) /**< (SDHC_FEREIS) Interrupt is generated Position  */
#define SDHC_FEREIS_CMDIDX_Pos                _U_(3)                                               /**< (SDHC_FEREIS) Force Event for Command Index Error Position */
#define SDHC_FEREIS_CMDIDX_Msk                (_U_(0x1) << SDHC_FEREIS_CMDIDX_Pos)                 /**< (SDHC_FEREIS) Force Event for Command Index Error Mask */
#define SDHC_FEREIS_CMDIDX(value)             (SDHC_FEREIS_CMDIDX_Msk & ((value) << SDHC_FEREIS_CMDIDX_Pos))
#define   SDHC_FEREIS_CMDIDX_NO_Val           _U_(0x0)                                             /**< (SDHC_FEREIS) No Interrupt  */
#define   SDHC_FEREIS_CMDIDX_YES_Val          _U_(0x1)                                             /**< (SDHC_FEREIS) Interrupt is generated  */
#define SDHC_FEREIS_CMDIDX_NO                 (SDHC_FEREIS_CMDIDX_NO_Val << SDHC_FEREIS_CMDIDX_Pos) /**< (SDHC_FEREIS) No Interrupt Position  */
#define SDHC_FEREIS_CMDIDX_YES                (SDHC_FEREIS_CMDIDX_YES_Val << SDHC_FEREIS_CMDIDX_Pos) /**< (SDHC_FEREIS) Interrupt is generated Position  */
#define SDHC_FEREIS_DATTEO_Pos                _U_(4)                                               /**< (SDHC_FEREIS) Force Event for Data Timeout Error Position */
#define SDHC_FEREIS_DATTEO_Msk                (_U_(0x1) << SDHC_FEREIS_DATTEO_Pos)                 /**< (SDHC_FEREIS) Force Event for Data Timeout Error Mask */
#define SDHC_FEREIS_DATTEO(value)             (SDHC_FEREIS_DATTEO_Msk & ((value) << SDHC_FEREIS_DATTEO_Pos))
#define   SDHC_FEREIS_DATTEO_NO_Val           _U_(0x0)                                             /**< (SDHC_FEREIS) No Interrupt  */
#define   SDHC_FEREIS_DATTEO_YES_Val          _U_(0x1)                                             /**< (SDHC_FEREIS) Interrupt is generated  */
#define SDHC_FEREIS_DATTEO_NO                 (SDHC_FEREIS_DATTEO_NO_Val << SDHC_FEREIS_DATTEO_Pos) /**< (SDHC_FEREIS) No Interrupt Position  */
#define SDHC_FEREIS_DATTEO_YES                (SDHC_FEREIS_DATTEO_YES_Val << SDHC_FEREIS_DATTEO_Pos) /**< (SDHC_FEREIS) Interrupt is generated Position  */
#define SDHC_FEREIS_DATCRC_Pos                _U_(5)                                               /**< (SDHC_FEREIS) Force Event for Data CRC Error Position */
#define SDHC_FEREIS_DATCRC_Msk                (_U_(0x1) << SDHC_FEREIS_DATCRC_Pos)                 /**< (SDHC_FEREIS) Force Event for Data CRC Error Mask */
#define SDHC_FEREIS_DATCRC(value)             (SDHC_FEREIS_DATCRC_Msk & ((value) << SDHC_FEREIS_DATCRC_Pos))
#define   SDHC_FEREIS_DATCRC_NO_Val           _U_(0x0)                                             /**< (SDHC_FEREIS) No Interrupt  */
#define   SDHC_FEREIS_DATCRC_YES_Val          _U_(0x1)                                             /**< (SDHC_FEREIS) Interrupt is generated  */
#define SDHC_FEREIS_DATCRC_NO                 (SDHC_FEREIS_DATCRC_NO_Val << SDHC_FEREIS_DATCRC_Pos) /**< (SDHC_FEREIS) No Interrupt Position  */
#define SDHC_FEREIS_DATCRC_YES                (SDHC_FEREIS_DATCRC_YES_Val << SDHC_FEREIS_DATCRC_Pos) /**< (SDHC_FEREIS) Interrupt is generated Position  */
#define SDHC_FEREIS_DATEND_Pos                _U_(6)                                               /**< (SDHC_FEREIS) Force Event for Data End Bit Error Position */
#define SDHC_FEREIS_DATEND_Msk                (_U_(0x1) << SDHC_FEREIS_DATEND_Pos)                 /**< (SDHC_FEREIS) Force Event for Data End Bit Error Mask */
#define SDHC_FEREIS_DATEND(value)             (SDHC_FEREIS_DATEND_Msk & ((value) << SDHC_FEREIS_DATEND_Pos))
#define   SDHC_FEREIS_DATEND_NO_Val           _U_(0x0)                                             /**< (SDHC_FEREIS) No Interrupt  */
#define   SDHC_FEREIS_DATEND_YES_Val          _U_(0x1)                                             /**< (SDHC_FEREIS) Interrupt is generated  */
#define SDHC_FEREIS_DATEND_NO                 (SDHC_FEREIS_DATEND_NO_Val << SDHC_FEREIS_DATEND_Pos) /**< (SDHC_FEREIS) No Interrupt Position  */
#define SDHC_FEREIS_DATEND_YES                (SDHC_FEREIS_DATEND_YES_Val << SDHC_FEREIS_DATEND_Pos) /**< (SDHC_FEREIS) Interrupt is generated Position  */
#define SDHC_FEREIS_CURLIM_Pos                _U_(7)                                               /**< (SDHC_FEREIS) Force Event for Current Limit Error Position */
#define SDHC_FEREIS_CURLIM_Msk                (_U_(0x1) << SDHC_FEREIS_CURLIM_Pos)                 /**< (SDHC_FEREIS) Force Event for Current Limit Error Mask */
#define SDHC_FEREIS_CURLIM(value)             (SDHC_FEREIS_CURLIM_Msk & ((value) << SDHC_FEREIS_CURLIM_Pos))
#define   SDHC_FEREIS_CURLIM_NO_Val           _U_(0x0)                                             /**< (SDHC_FEREIS) No Interrupt  */
#define   SDHC_FEREIS_CURLIM_YES_Val          _U_(0x1)                                             /**< (SDHC_FEREIS) Interrupt is generated  */
#define SDHC_FEREIS_CURLIM_NO                 (SDHC_FEREIS_CURLIM_NO_Val << SDHC_FEREIS_CURLIM_Pos) /**< (SDHC_FEREIS) No Interrupt Position  */
#define SDHC_FEREIS_CURLIM_YES                (SDHC_FEREIS_CURLIM_YES_Val << SDHC_FEREIS_CURLIM_Pos) /**< (SDHC_FEREIS) Interrupt is generated Position  */
#define SDHC_FEREIS_ACMD_Pos                  _U_(8)                                               /**< (SDHC_FEREIS) Force Event for Auto CMD Error Position */
#define SDHC_FEREIS_ACMD_Msk                  (_U_(0x1) << SDHC_FEREIS_ACMD_Pos)                   /**< (SDHC_FEREIS) Force Event for Auto CMD Error Mask */
#define SDHC_FEREIS_ACMD(value)               (SDHC_FEREIS_ACMD_Msk & ((value) << SDHC_FEREIS_ACMD_Pos))
#define   SDHC_FEREIS_ACMD_NO_Val             _U_(0x0)                                             /**< (SDHC_FEREIS) No Interrupt  */
#define   SDHC_FEREIS_ACMD_YES_Val            _U_(0x1)                                             /**< (SDHC_FEREIS) Interrupt is generated  */
#define SDHC_FEREIS_ACMD_NO                   (SDHC_FEREIS_ACMD_NO_Val << SDHC_FEREIS_ACMD_Pos)    /**< (SDHC_FEREIS) No Interrupt Position  */
#define SDHC_FEREIS_ACMD_YES                  (SDHC_FEREIS_ACMD_YES_Val << SDHC_FEREIS_ACMD_Pos)   /**< (SDHC_FEREIS) Interrupt is generated Position  */
#define SDHC_FEREIS_ADMA_Pos                  _U_(9)                                               /**< (SDHC_FEREIS) Force Event for ADMA Error Position */
#define SDHC_FEREIS_ADMA_Msk                  (_U_(0x1) << SDHC_FEREIS_ADMA_Pos)                   /**< (SDHC_FEREIS) Force Event for ADMA Error Mask */
#define SDHC_FEREIS_ADMA(value)               (SDHC_FEREIS_ADMA_Msk & ((value) << SDHC_FEREIS_ADMA_Pos))
#define   SDHC_FEREIS_ADMA_NO_Val             _U_(0x0)                                             /**< (SDHC_FEREIS) No Interrupt  */
#define   SDHC_FEREIS_ADMA_YES_Val            _U_(0x1)                                             /**< (SDHC_FEREIS) Interrupt is generated  */
#define SDHC_FEREIS_ADMA_NO                   (SDHC_FEREIS_ADMA_NO_Val << SDHC_FEREIS_ADMA_Pos)    /**< (SDHC_FEREIS) No Interrupt Position  */
#define SDHC_FEREIS_ADMA_YES                  (SDHC_FEREIS_ADMA_YES_Val << SDHC_FEREIS_ADMA_Pos)   /**< (SDHC_FEREIS) Interrupt is generated Position  */
#define SDHC_FEREIS_BOOTAE_Pos                _U_(12)                                              /**< (SDHC_FEREIS) Force Event for Boot Acknowledge Error Position */
#define SDHC_FEREIS_BOOTAE_Msk                (_U_(0x1) << SDHC_FEREIS_BOOTAE_Pos)                 /**< (SDHC_FEREIS) Force Event for Boot Acknowledge Error Mask */
#define SDHC_FEREIS_BOOTAE(value)             (SDHC_FEREIS_BOOTAE_Msk & ((value) << SDHC_FEREIS_BOOTAE_Pos))
#define   SDHC_FEREIS_BOOTAE_NO_Val           _U_(0x0)                                             /**< (SDHC_FEREIS) No Interrupt  */
#define   SDHC_FEREIS_BOOTAE_YES_Val          _U_(0x1)                                             /**< (SDHC_FEREIS) Interrupt is generated  */
#define SDHC_FEREIS_BOOTAE_NO                 (SDHC_FEREIS_BOOTAE_NO_Val << SDHC_FEREIS_BOOTAE_Pos) /**< (SDHC_FEREIS) No Interrupt Position  */
#define SDHC_FEREIS_BOOTAE_YES                (SDHC_FEREIS_BOOTAE_YES_Val << SDHC_FEREIS_BOOTAE_Pos) /**< (SDHC_FEREIS) Interrupt is generated Position  */
#define SDHC_FEREIS_Msk                       _U_(0x13FF)                                          /**< (SDHC_FEREIS) Register Mask  */


/* -------- SDHC_AESR : (SDHC Offset: 0x54) ( R/ 8) ADMA Error Status -------- */
#define SDHC_AESR_RESETVALUE                  _U_(0x00)                                            /**<  (SDHC_AESR) ADMA Error Status  Reset Value */

#define SDHC_AESR_ERRST_Pos                   _U_(0)                                               /**< (SDHC_AESR) ADMA Error State Position */
#define SDHC_AESR_ERRST_Msk                   (_U_(0x3) << SDHC_AESR_ERRST_Pos)                    /**< (SDHC_AESR) ADMA Error State Mask */
#define SDHC_AESR_ERRST(value)                (SDHC_AESR_ERRST_Msk & ((value) << SDHC_AESR_ERRST_Pos))
#define   SDHC_AESR_ERRST_STOP_Val            _U_(0x0)                                             /**< (SDHC_AESR) ST_STOP (Stop DMA)  */
#define   SDHC_AESR_ERRST_FDS_Val             _U_(0x1)                                             /**< (SDHC_AESR) ST_FDS (Fetch Descriptor)  */
#define   SDHC_AESR_ERRST_TFR_Val             _U_(0x3)                                             /**< (SDHC_AESR) ST_TFR (Transfer Data)  */
#define SDHC_AESR_ERRST_STOP                  (SDHC_AESR_ERRST_STOP_Val << SDHC_AESR_ERRST_Pos)    /**< (SDHC_AESR) ST_STOP (Stop DMA) Position  */
#define SDHC_AESR_ERRST_FDS                   (SDHC_AESR_ERRST_FDS_Val << SDHC_AESR_ERRST_Pos)     /**< (SDHC_AESR) ST_FDS (Fetch Descriptor) Position  */
#define SDHC_AESR_ERRST_TFR                   (SDHC_AESR_ERRST_TFR_Val << SDHC_AESR_ERRST_Pos)     /**< (SDHC_AESR) ST_TFR (Transfer Data) Position  */
#define SDHC_AESR_LMIS_Pos                    _U_(2)                                               /**< (SDHC_AESR) ADMA Length Mismatch Error Position */
#define SDHC_AESR_LMIS_Msk                    (_U_(0x1) << SDHC_AESR_LMIS_Pos)                     /**< (SDHC_AESR) ADMA Length Mismatch Error Mask */
#define SDHC_AESR_LMIS(value)                 (SDHC_AESR_LMIS_Msk & ((value) << SDHC_AESR_LMIS_Pos))
#define   SDHC_AESR_LMIS_NO_Val               _U_(0x0)                                             /**< (SDHC_AESR) No Error  */
#define   SDHC_AESR_LMIS_YES_Val              _U_(0x1)                                             /**< (SDHC_AESR) Error  */
#define SDHC_AESR_LMIS_NO                     (SDHC_AESR_LMIS_NO_Val << SDHC_AESR_LMIS_Pos)        /**< (SDHC_AESR) No Error Position  */
#define SDHC_AESR_LMIS_YES                    (SDHC_AESR_LMIS_YES_Val << SDHC_AESR_LMIS_Pos)       /**< (SDHC_AESR) Error Position  */
#define SDHC_AESR_Msk                         _U_(0x07)                                            /**< (SDHC_AESR) Register Mask  */


/* -------- SDHC_ASAR : (SDHC Offset: 0x58) (R/W 32) ADMA System Address n -------- */
#define SDHC_ASAR_RESETVALUE                  _U_(0x00)                                            /**<  (SDHC_ASAR) ADMA System Address n  Reset Value */

#define SDHC_ASAR_ADMASA_Pos                  _U_(0)                                               /**< (SDHC_ASAR) ADMA System Address Position */
#define SDHC_ASAR_ADMASA_Msk                  (_U_(0xFFFFFFFF) << SDHC_ASAR_ADMASA_Pos)            /**< (SDHC_ASAR) ADMA System Address Mask */
#define SDHC_ASAR_ADMASA(value)               (SDHC_ASAR_ADMASA_Msk & ((value) << SDHC_ASAR_ADMASA_Pos))
#define SDHC_ASAR_Msk                         _U_(0xFFFFFFFF)                                      /**< (SDHC_ASAR) Register Mask  */


/* -------- SDHC_PVR : (SDHC Offset: 0x60) (R/W 16) Preset Value n -------- */
#define SDHC_PVR_RESETVALUE                   _U_(0x00)                                            /**<  (SDHC_PVR) Preset Value n  Reset Value */

#define SDHC_PVR_SDCLKFSEL_Pos                _U_(0)                                               /**< (SDHC_PVR) SDCLK Frequency Select Value for Initialization Position */
#define SDHC_PVR_SDCLKFSEL_Msk                (_U_(0x3FF) << SDHC_PVR_SDCLKFSEL_Pos)               /**< (SDHC_PVR) SDCLK Frequency Select Value for Initialization Mask */
#define SDHC_PVR_SDCLKFSEL(value)             (SDHC_PVR_SDCLKFSEL_Msk & ((value) << SDHC_PVR_SDCLKFSEL_Pos))
#define SDHC_PVR_CLKGSEL_Pos                  _U_(10)                                              /**< (SDHC_PVR) Clock Generator Select Value for Initialization Position */
#define SDHC_PVR_CLKGSEL_Msk                  (_U_(0x1) << SDHC_PVR_CLKGSEL_Pos)                   /**< (SDHC_PVR) Clock Generator Select Value for Initialization Mask */
#define SDHC_PVR_CLKGSEL(value)               (SDHC_PVR_CLKGSEL_Msk & ((value) << SDHC_PVR_CLKGSEL_Pos))
#define   SDHC_PVR_CLKGSEL_DIV_Val            _U_(0x0)                                             /**< (SDHC_PVR) Host Controller Ver2.00 Compatible Clock Generator (Divider)  */
#define   SDHC_PVR_CLKGSEL_PROG_Val           _U_(0x1)                                             /**< (SDHC_PVR) Programmable Clock Generator  */
#define SDHC_PVR_CLKGSEL_DIV                  (SDHC_PVR_CLKGSEL_DIV_Val << SDHC_PVR_CLKGSEL_Pos)   /**< (SDHC_PVR) Host Controller Ver2.00 Compatible Clock Generator (Divider) Position  */
#define SDHC_PVR_CLKGSEL_PROG                 (SDHC_PVR_CLKGSEL_PROG_Val << SDHC_PVR_CLKGSEL_Pos)  /**< (SDHC_PVR) Programmable Clock Generator Position  */
#define SDHC_PVR_DRVSEL_Pos                   _U_(14)                                              /**< (SDHC_PVR) Driver Strength Select Value for Initialization Position */
#define SDHC_PVR_DRVSEL_Msk                   (_U_(0x3) << SDHC_PVR_DRVSEL_Pos)                    /**< (SDHC_PVR) Driver Strength Select Value for Initialization Mask */
#define SDHC_PVR_DRVSEL(value)                (SDHC_PVR_DRVSEL_Msk & ((value) << SDHC_PVR_DRVSEL_Pos))
#define   SDHC_PVR_DRVSEL_B_Val               _U_(0x0)                                             /**< (SDHC_PVR) Driver Type B is Selected  */
#define   SDHC_PVR_DRVSEL_A_Val               _U_(0x1)                                             /**< (SDHC_PVR) Driver Type A is Selected  */
#define   SDHC_PVR_DRVSEL_C_Val               _U_(0x2)                                             /**< (SDHC_PVR) Driver Type C is Selected  */
#define   SDHC_PVR_DRVSEL_D_Val               _U_(0x3)                                             /**< (SDHC_PVR) Driver Type D is Selected  */
#define SDHC_PVR_DRVSEL_B                     (SDHC_PVR_DRVSEL_B_Val << SDHC_PVR_DRVSEL_Pos)       /**< (SDHC_PVR) Driver Type B is Selected Position  */
#define SDHC_PVR_DRVSEL_A                     (SDHC_PVR_DRVSEL_A_Val << SDHC_PVR_DRVSEL_Pos)       /**< (SDHC_PVR) Driver Type A is Selected Position  */
#define SDHC_PVR_DRVSEL_C                     (SDHC_PVR_DRVSEL_C_Val << SDHC_PVR_DRVSEL_Pos)       /**< (SDHC_PVR) Driver Type C is Selected Position  */
#define SDHC_PVR_DRVSEL_D                     (SDHC_PVR_DRVSEL_D_Val << SDHC_PVR_DRVSEL_Pos)       /**< (SDHC_PVR) Driver Type D is Selected Position  */
#define SDHC_PVR_Msk                          _U_(0xC7FF)                                          /**< (SDHC_PVR) Register Mask  */


/* -------- SDHC_SISR : (SDHC Offset: 0xFC) ( R/ 16) Slot Interrupt Status -------- */
#define SDHC_SISR_RESETVALUE                  _U_(0x20000)                                         /**<  (SDHC_SISR) Slot Interrupt Status  Reset Value */

#define SDHC_SISR_INTSSL_Pos                  _U_(0)                                               /**< (SDHC_SISR) Interrupt Signal for Each Slot Position */
#define SDHC_SISR_INTSSL_Msk                  (_U_(0x1) << SDHC_SISR_INTSSL_Pos)                   /**< (SDHC_SISR) Interrupt Signal for Each Slot Mask */
#define SDHC_SISR_INTSSL(value)               (SDHC_SISR_INTSSL_Msk & ((value) << SDHC_SISR_INTSSL_Pos))
#define SDHC_SISR_Msk                         _U_(0x0001)                                          /**< (SDHC_SISR) Register Mask  */


/* -------- SDHC_HCVR : (SDHC Offset: 0xFE) ( R/ 16) Host Controller Version -------- */
#define SDHC_HCVR_RESETVALUE                  _U_(0x1802)                                          /**<  (SDHC_HCVR) Host Controller Version  Reset Value */

#define SDHC_HCVR_SVER_Pos                    _U_(0)                                               /**< (SDHC_HCVR) Spec Version Position */
#define SDHC_HCVR_SVER_Msk                    (_U_(0xFF) << SDHC_HCVR_SVER_Pos)                    /**< (SDHC_HCVR) Spec Version Mask */
#define SDHC_HCVR_SVER(value)                 (SDHC_HCVR_SVER_Msk & ((value) << SDHC_HCVR_SVER_Pos))
#define SDHC_HCVR_VVER_Pos                    _U_(8)                                               /**< (SDHC_HCVR) Vendor Version Position */
#define SDHC_HCVR_VVER_Msk                    (_U_(0xFF) << SDHC_HCVR_VVER_Pos)                    /**< (SDHC_HCVR) Vendor Version Mask */
#define SDHC_HCVR_VVER(value)                 (SDHC_HCVR_VVER_Msk & ((value) << SDHC_HCVR_VVER_Pos))
#define SDHC_HCVR_Msk                         _U_(0xFFFF)                                          /**< (SDHC_HCVR) Register Mask  */


/* -------- SDHC_MC1R : (SDHC Offset: 0x204) (R/W 8) MMC Control 1 -------- */
#define SDHC_MC1R_RESETVALUE                  _U_(0x00)                                            /**<  (SDHC_MC1R) MMC Control 1  Reset Value */

#define SDHC_MC1R_CMDTYP_Pos                  _U_(0)                                               /**< (SDHC_MC1R) e.MMC Command Type Position */
#define SDHC_MC1R_CMDTYP_Msk                  (_U_(0x3) << SDHC_MC1R_CMDTYP_Pos)                   /**< (SDHC_MC1R) e.MMC Command Type Mask */
#define SDHC_MC1R_CMDTYP(value)               (SDHC_MC1R_CMDTYP_Msk & ((value) << SDHC_MC1R_CMDTYP_Pos))
#define   SDHC_MC1R_CMDTYP_NORMAL_Val         _U_(0x0)                                             /**< (SDHC_MC1R) Not a MMC specific command  */
#define   SDHC_MC1R_CMDTYP_WAITIRQ_Val        _U_(0x1)                                             /**< (SDHC_MC1R) Wait IRQ Command  */
#define   SDHC_MC1R_CMDTYP_STREAM_Val         _U_(0x2)                                             /**< (SDHC_MC1R) Stream Command  */
#define   SDHC_MC1R_CMDTYP_BOOT_Val           _U_(0x3)                                             /**< (SDHC_MC1R) Boot Command  */
#define SDHC_MC1R_CMDTYP_NORMAL               (SDHC_MC1R_CMDTYP_NORMAL_Val << SDHC_MC1R_CMDTYP_Pos) /**< (SDHC_MC1R) Not a MMC specific command Position  */
#define SDHC_MC1R_CMDTYP_WAITIRQ              (SDHC_MC1R_CMDTYP_WAITIRQ_Val << SDHC_MC1R_CMDTYP_Pos) /**< (SDHC_MC1R) Wait IRQ Command Position  */
#define SDHC_MC1R_CMDTYP_STREAM               (SDHC_MC1R_CMDTYP_STREAM_Val << SDHC_MC1R_CMDTYP_Pos) /**< (SDHC_MC1R) Stream Command Position  */
#define SDHC_MC1R_CMDTYP_BOOT                 (SDHC_MC1R_CMDTYP_BOOT_Val << SDHC_MC1R_CMDTYP_Pos)  /**< (SDHC_MC1R) Boot Command Position  */
#define SDHC_MC1R_DDR_Pos                     _U_(3)                                               /**< (SDHC_MC1R) e.MMC HSDDR Mode Position */
#define SDHC_MC1R_DDR_Msk                     (_U_(0x1) << SDHC_MC1R_DDR_Pos)                      /**< (SDHC_MC1R) e.MMC HSDDR Mode Mask */
#define SDHC_MC1R_DDR(value)                  (SDHC_MC1R_DDR_Msk & ((value) << SDHC_MC1R_DDR_Pos))
#define SDHC_MC1R_OPD_Pos                     _U_(4)                                               /**< (SDHC_MC1R) e.MMC Open Drain Mode Position */
#define SDHC_MC1R_OPD_Msk                     (_U_(0x1) << SDHC_MC1R_OPD_Pos)                      /**< (SDHC_MC1R) e.MMC Open Drain Mode Mask */
#define SDHC_MC1R_OPD(value)                  (SDHC_MC1R_OPD_Msk & ((value) << SDHC_MC1R_OPD_Pos))
#define SDHC_MC1R_BOOTA_Pos                   _U_(5)                                               /**< (SDHC_MC1R) e.MMC Boot Acknowledge Enable Position */
#define SDHC_MC1R_BOOTA_Msk                   (_U_(0x1) << SDHC_MC1R_BOOTA_Pos)                    /**< (SDHC_MC1R) e.MMC Boot Acknowledge Enable Mask */
#define SDHC_MC1R_BOOTA(value)                (SDHC_MC1R_BOOTA_Msk & ((value) << SDHC_MC1R_BOOTA_Pos))
#define SDHC_MC1R_RSTN_Pos                    _U_(6)                                               /**< (SDHC_MC1R) e.MMC Reset Signal Position */
#define SDHC_MC1R_RSTN_Msk                    (_U_(0x1) << SDHC_MC1R_RSTN_Pos)                     /**< (SDHC_MC1R) e.MMC Reset Signal Mask */
#define SDHC_MC1R_RSTN(value)                 (SDHC_MC1R_RSTN_Msk & ((value) << SDHC_MC1R_RSTN_Pos))
#define SDHC_MC1R_FCD_Pos                     _U_(7)                                               /**< (SDHC_MC1R) e.MMC Force Card Detect Position */
#define SDHC_MC1R_FCD_Msk                     (_U_(0x1) << SDHC_MC1R_FCD_Pos)                      /**< (SDHC_MC1R) e.MMC Force Card Detect Mask */
#define SDHC_MC1R_FCD(value)                  (SDHC_MC1R_FCD_Msk & ((value) << SDHC_MC1R_FCD_Pos))
#define SDHC_MC1R_Msk                         _U_(0xFB)                                            /**< (SDHC_MC1R) Register Mask  */


/* -------- SDHC_MC2R : (SDHC Offset: 0x205) ( /W 8) MMC Control 2 -------- */
#define SDHC_MC2R_RESETVALUE                  _U_(0x00)                                            /**<  (SDHC_MC2R) MMC Control 2  Reset Value */

#define SDHC_MC2R_SRESP_Pos                   _U_(0)                                               /**< (SDHC_MC2R) e.MMC Abort Wait IRQ Position */
#define SDHC_MC2R_SRESP_Msk                   (_U_(0x1) << SDHC_MC2R_SRESP_Pos)                    /**< (SDHC_MC2R) e.MMC Abort Wait IRQ Mask */
#define SDHC_MC2R_SRESP(value)                (SDHC_MC2R_SRESP_Msk & ((value) << SDHC_MC2R_SRESP_Pos))
#define SDHC_MC2R_ABOOT_Pos                   _U_(1)                                               /**< (SDHC_MC2R) e.MMC Abort Boot Position */
#define SDHC_MC2R_ABOOT_Msk                   (_U_(0x1) << SDHC_MC2R_ABOOT_Pos)                    /**< (SDHC_MC2R) e.MMC Abort Boot Mask */
#define SDHC_MC2R_ABOOT(value)                (SDHC_MC2R_ABOOT_Msk & ((value) << SDHC_MC2R_ABOOT_Pos))
#define SDHC_MC2R_Msk                         _U_(0x03)                                            /**< (SDHC_MC2R) Register Mask  */


/* -------- SDHC_ACR : (SDHC Offset: 0x208) (R/W 32) AHB Control -------- */
#define SDHC_ACR_RESETVALUE                   _U_(0x00)                                            /**<  (SDHC_ACR) AHB Control  Reset Value */

#define SDHC_ACR_BMAX_Pos                     _U_(0)                                               /**< (SDHC_ACR) AHB Maximum Burst Position */
#define SDHC_ACR_BMAX_Msk                     (_U_(0x3) << SDHC_ACR_BMAX_Pos)                      /**< (SDHC_ACR) AHB Maximum Burst Mask */
#define SDHC_ACR_BMAX(value)                  (SDHC_ACR_BMAX_Msk & ((value) << SDHC_ACR_BMAX_Pos))
#define   SDHC_ACR_BMAX_INCR16_Val            _U_(0x0)                                             /**< (SDHC_ACR)   */
#define   SDHC_ACR_BMAX_INCR8_Val             _U_(0x1)                                             /**< (SDHC_ACR)   */
#define   SDHC_ACR_BMAX_INCR4_Val             _U_(0x2)                                             /**< (SDHC_ACR)   */
#define   SDHC_ACR_BMAX_SINGLE_Val            _U_(0x3)                                             /**< (SDHC_ACR)   */
#define SDHC_ACR_BMAX_INCR16                  (SDHC_ACR_BMAX_INCR16_Val << SDHC_ACR_BMAX_Pos)      /**< (SDHC_ACR)  Position  */
#define SDHC_ACR_BMAX_INCR8                   (SDHC_ACR_BMAX_INCR8_Val << SDHC_ACR_BMAX_Pos)       /**< (SDHC_ACR)  Position  */
#define SDHC_ACR_BMAX_INCR4                   (SDHC_ACR_BMAX_INCR4_Val << SDHC_ACR_BMAX_Pos)       /**< (SDHC_ACR)  Position  */
#define SDHC_ACR_BMAX_SINGLE                  (SDHC_ACR_BMAX_SINGLE_Val << SDHC_ACR_BMAX_Pos)      /**< (SDHC_ACR)  Position  */
#define SDHC_ACR_Msk                          _U_(0x00000003)                                      /**< (SDHC_ACR) Register Mask  */


/* -------- SDHC_CC2R : (SDHC Offset: 0x20C) (R/W 32) Clock Control 2 -------- */
#define SDHC_CC2R_RESETVALUE                  _U_(0x00)                                            /**<  (SDHC_CC2R) Clock Control 2  Reset Value */

#define SDHC_CC2R_FSDCLKD_Pos                 _U_(0)                                               /**< (SDHC_CC2R) Force SDCK Disabled Position */
#define SDHC_CC2R_FSDCLKD_Msk                 (_U_(0x1) << SDHC_CC2R_FSDCLKD_Pos)                  /**< (SDHC_CC2R) Force SDCK Disabled Mask */
#define SDHC_CC2R_FSDCLKD(value)              (SDHC_CC2R_FSDCLKD_Msk & ((value) << SDHC_CC2R_FSDCLKD_Pos))
#define   SDHC_CC2R_FSDCLKD_NOEFFECT_Val      _U_(0x0)                                             /**< (SDHC_CC2R) No effect  */
#define   SDHC_CC2R_FSDCLKD_DISABLE_Val       _U_(0x1)                                             /**< (SDHC_CC2R) SDCLK can be stopped at any time after DATA transfer.SDCLK enable forcing for 8 SDCLK cycles is disabled  */
#define SDHC_CC2R_FSDCLKD_NOEFFECT            (SDHC_CC2R_FSDCLKD_NOEFFECT_Val << SDHC_CC2R_FSDCLKD_Pos) /**< (SDHC_CC2R) No effect Position  */
#define SDHC_CC2R_FSDCLKD_DISABLE             (SDHC_CC2R_FSDCLKD_DISABLE_Val << SDHC_CC2R_FSDCLKD_Pos) /**< (SDHC_CC2R) SDCLK can be stopped at any time after DATA transfer.SDCLK enable forcing for 8 SDCLK cycles is disabled Position  */
#define SDHC_CC2R_Msk                         _U_(0x00000001)                                      /**< (SDHC_CC2R) Register Mask  */


/* -------- SDHC_CACR : (SDHC Offset: 0x230) (R/W 32) Capabilities Control -------- */
#define SDHC_CACR_RESETVALUE                  _U_(0x00)                                            /**<  (SDHC_CACR) Capabilities Control  Reset Value */

#define SDHC_CACR_CAPWREN_Pos                 _U_(0)                                               /**< (SDHC_CACR) Capabilities Registers Write Enable (Required to write the correct frequencies in the Capabilities Registers) Position */
#define SDHC_CACR_CAPWREN_Msk                 (_U_(0x1) << SDHC_CACR_CAPWREN_Pos)                  /**< (SDHC_CACR) Capabilities Registers Write Enable (Required to write the correct frequencies in the Capabilities Registers) Mask */
#define SDHC_CACR_CAPWREN(value)              (SDHC_CACR_CAPWREN_Msk & ((value) << SDHC_CACR_CAPWREN_Pos))
#define SDHC_CACR_KEY_Pos                     _U_(8)                                               /**< (SDHC_CACR) Key (0x46) Position */
#define SDHC_CACR_KEY_Msk                     (_U_(0xFF) << SDHC_CACR_KEY_Pos)                     /**< (SDHC_CACR) Key (0x46) Mask */
#define SDHC_CACR_KEY(value)                  (SDHC_CACR_KEY_Msk & ((value) << SDHC_CACR_KEY_Pos))
#define SDHC_CACR_Msk                         _U_(0x0000FF01)                                      /**< (SDHC_CACR) Register Mask  */


/* -------- SDHC_DBGR : (SDHC Offset: 0x234) (R/W 8) Debug -------- */
#define SDHC_DBGR_RESETVALUE                  _U_(0x00)                                            /**<  (SDHC_DBGR) Debug  Reset Value */

#define SDHC_DBGR_NIDBG_Pos                   _U_(0)                                               /**< (SDHC_DBGR) Non-intrusive debug enable Position */
#define SDHC_DBGR_NIDBG_Msk                   (_U_(0x1) << SDHC_DBGR_NIDBG_Pos)                    /**< (SDHC_DBGR) Non-intrusive debug enable Mask */
#define SDHC_DBGR_NIDBG(value)                (SDHC_DBGR_NIDBG_Msk & ((value) << SDHC_DBGR_NIDBG_Pos))
#define   SDHC_DBGR_NIDBG_IDBG_Val            _U_(0x0)                                             /**< (SDHC_DBGR) Debugging is intrusive (reads of BDPR from debugger are considered and increment the internal buffer pointer)  */
#define   SDHC_DBGR_NIDBG_NIDBG_Val           _U_(0x1)                                             /**< (SDHC_DBGR) Debugging is not intrusive (reads of BDPR from debugger are discarded and do not increment the internal buffer pointer)  */
#define SDHC_DBGR_NIDBG_IDBG                  (SDHC_DBGR_NIDBG_IDBG_Val << SDHC_DBGR_NIDBG_Pos)    /**< (SDHC_DBGR) Debugging is intrusive (reads of BDPR from debugger are considered and increment the internal buffer pointer) Position  */
#define SDHC_DBGR_NIDBG_NIDBG                 (SDHC_DBGR_NIDBG_NIDBG_Val << SDHC_DBGR_NIDBG_Pos)   /**< (SDHC_DBGR) Debugging is not intrusive (reads of BDPR from debugger are discarded and do not increment the internal buffer pointer) Position  */
#define SDHC_DBGR_Msk                         _U_(0x01)                                            /**< (SDHC_DBGR) Register Mask  */


/** \brief SDHC register offsets definitions */
#define SDHC_SSAR_REG_OFST             (0x00)              /**< (SDHC_SSAR) SDMA System Address / Argument 2 Offset */
#define SDHC_BSR_REG_OFST              (0x04)              /**< (SDHC_BSR) Block Size Offset */
#define SDHC_BCR_REG_OFST              (0x06)              /**< (SDHC_BCR) Block Count Offset */
#define SDHC_ARG1R_REG_OFST            (0x08)              /**< (SDHC_ARG1R) Argument 1 Offset */
#define SDHC_TMR_REG_OFST              (0x0C)              /**< (SDHC_TMR) Transfer Mode Offset */
#define SDHC_CR_REG_OFST               (0x0E)              /**< (SDHC_CR) Command Offset */
#define SDHC_RR_REG_OFST               (0x10)              /**< (SDHC_RR) Response Offset */
#define SDHC_BDPR_REG_OFST             (0x20)              /**< (SDHC_BDPR) Buffer Data Port Offset */
#define SDHC_PSR_REG_OFST              (0x24)              /**< (SDHC_PSR) Present State Offset */
#define SDHC_HC1R_REG_OFST             (0x28)              /**< (SDHC_HC1R) Host Control 1 Offset */
#define SDHC_PCR_REG_OFST              (0x29)              /**< (SDHC_PCR) Power Control Offset */
#define SDHC_BGCR_REG_OFST             (0x2A)              /**< (SDHC_BGCR) Block Gap Control Offset */
#define SDHC_WCR_REG_OFST              (0x2B)              /**< (SDHC_WCR) Wakeup Control Offset */
#define SDHC_CCR_REG_OFST              (0x2C)              /**< (SDHC_CCR) Clock Control Offset */
#define SDHC_TCR_REG_OFST              (0x2E)              /**< (SDHC_TCR) Timeout Control Offset */
#define SDHC_SRR_REG_OFST              (0x2F)              /**< (SDHC_SRR) Software Reset Offset */
#define SDHC_NISTR_REG_OFST            (0x30)              /**< (SDHC_NISTR) Normal Interrupt Status Offset */
#define SDHC_EISTR_REG_OFST            (0x32)              /**< (SDHC_EISTR) Error Interrupt Status Offset */
#define SDHC_NISTER_REG_OFST           (0x34)              /**< (SDHC_NISTER) Normal Interrupt Status Enable Offset */
#define SDHC_EISTER_REG_OFST           (0x36)              /**< (SDHC_EISTER) Error Interrupt Status Enable Offset */
#define SDHC_NISIER_REG_OFST           (0x38)              /**< (SDHC_NISIER) Normal Interrupt Signal Enable Offset */
#define SDHC_EISIER_REG_OFST           (0x3A)              /**< (SDHC_EISIER) Error Interrupt Signal Enable Offset */
#define SDHC_ACESR_REG_OFST            (0x3C)              /**< (SDHC_ACESR) Auto CMD Error Status Offset */
#define SDHC_HC2R_REG_OFST             (0x3E)              /**< (SDHC_HC2R) Host Control 2 Offset */
#define SDHC_CA0R_REG_OFST             (0x40)              /**< (SDHC_CA0R) Capabilities 0 Offset */
#define SDHC_CA1R_REG_OFST             (0x44)              /**< (SDHC_CA1R) Capabilities 1 Offset */
#define SDHC_MCCAR_REG_OFST            (0x48)              /**< (SDHC_MCCAR) Maximum Current Capabilities Offset */
#define SDHC_FERACES_REG_OFST          (0x50)              /**< (SDHC_FERACES) Force Event for Auto CMD Error Status Offset */
#define SDHC_FEREIS_REG_OFST           (0x52)              /**< (SDHC_FEREIS) Force Event for Error Interrupt Status Offset */
#define SDHC_AESR_REG_OFST             (0x54)              /**< (SDHC_AESR) ADMA Error Status Offset */
#define SDHC_ASAR_REG_OFST             (0x58)              /**< (SDHC_ASAR) ADMA System Address n Offset */
#define SDHC_PVR_REG_OFST              (0x60)              /**< (SDHC_PVR) Preset Value n Offset */
#define SDHC_SISR_REG_OFST             (0xFC)              /**< (SDHC_SISR) Slot Interrupt Status Offset */
#define SDHC_HCVR_REG_OFST             (0xFE)              /**< (SDHC_HCVR) Host Controller Version Offset */
#define SDHC_MC1R_REG_OFST             (0x204)             /**< (SDHC_MC1R) MMC Control 1 Offset */
#define SDHC_MC2R_REG_OFST             (0x205)             /**< (SDHC_MC2R) MMC Control 2 Offset */
#define SDHC_ACR_REG_OFST              (0x208)             /**< (SDHC_ACR) AHB Control Offset */
#define SDHC_CC2R_REG_OFST             (0x20C)             /**< (SDHC_CC2R) Clock Control 2 Offset */
#define SDHC_CACR_REG_OFST             (0x230)             /**< (SDHC_CACR) Capabilities Control Offset */
#define SDHC_DBGR_REG_OFST             (0x234)             /**< (SDHC_DBGR) Debug Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief SDHC register API structure */
typedef struct
{  /* SD/MMC Host Controller */
  __IO  uint32_t                       SDHC_SSAR;          /**< Offset: 0x00 (R/W  32) SDMA System Address / Argument 2 */
  __IO  uint16_t                       SDHC_BSR;           /**< Offset: 0x04 (R/W  16) Block Size */
  __IO  uint16_t                       SDHC_BCR;           /**< Offset: 0x06 (R/W  16) Block Count */
  __IO  uint32_t                       SDHC_ARG1R;         /**< Offset: 0x08 (R/W  32) Argument 1 */
  __IO  uint16_t                       SDHC_TMR;           /**< Offset: 0x0C (R/W  16) Transfer Mode */
  __IO  uint16_t                       SDHC_CR;            /**< Offset: 0x0E (R/W  16) Command */
  __I   uint32_t                       SDHC_RR[4];         /**< Offset: 0x10 (R/   32) Response */
  __IO  uint32_t                       SDHC_BDPR;          /**< Offset: 0x20 (R/W  32) Buffer Data Port */
  __I   uint32_t                       SDHC_PSR;           /**< Offset: 0x24 (R/   32) Present State */
  __IO  uint8_t                        SDHC_HC1R;          /**< Offset: 0x28 (R/W  8) Host Control 1 */
  __IO  uint8_t                        SDHC_PCR;           /**< Offset: 0x29 (R/W  8) Power Control */
  __IO  uint8_t                        SDHC_BGCR;          /**< Offset: 0x2A (R/W  8) Block Gap Control */
  __IO  uint8_t                        SDHC_WCR;           /**< Offset: 0x2B (R/W  8) Wakeup Control */
  __IO  uint16_t                       SDHC_CCR;           /**< Offset: 0x2C (R/W  16) Clock Control */
  __IO  uint8_t                        SDHC_TCR;           /**< Offset: 0x2E (R/W  8) Timeout Control */
  __IO  uint8_t                        SDHC_SRR;           /**< Offset: 0x2F (R/W  8) Software Reset */
  __IO  uint16_t                       SDHC_NISTR;         /**< Offset: 0x30 (R/W  16) Normal Interrupt Status */
  __IO  uint16_t                       SDHC_EISTR;         /**< Offset: 0x32 (R/W  16) Error Interrupt Status */
  __IO  uint16_t                       SDHC_NISTER;        /**< Offset: 0x34 (R/W  16) Normal Interrupt Status Enable */
  __IO  uint16_t                       SDHC_EISTER;        /**< Offset: 0x36 (R/W  16) Error Interrupt Status Enable */
  __IO  uint16_t                       SDHC_NISIER;        /**< Offset: 0x38 (R/W  16) Normal Interrupt Signal Enable */
  __IO  uint16_t                       SDHC_EISIER;        /**< Offset: 0x3A (R/W  16) Error Interrupt Signal Enable */
  __I   uint16_t                       SDHC_ACESR;         /**< Offset: 0x3C (R/   16) Auto CMD Error Status */
  __IO  uint16_t                       SDHC_HC2R;          /**< Offset: 0x3E (R/W  16) Host Control 2 */
  __I   uint32_t                       SDHC_CA0R;          /**< Offset: 0x40 (R/   32) Capabilities 0 */
  __I   uint32_t                       SDHC_CA1R;          /**< Offset: 0x44 (R/   32) Capabilities 1 */
  __I   uint32_t                       SDHC_MCCAR;         /**< Offset: 0x48 (R/   32) Maximum Current Capabilities */
  __I   uint8_t                        Reserved1[0x04];
  __O   uint16_t                       SDHC_FERACES;       /**< Offset: 0x50 ( /W  16) Force Event for Auto CMD Error Status */
  __O   uint16_t                       SDHC_FEREIS;        /**< Offset: 0x52 ( /W  16) Force Event for Error Interrupt Status */
  __I   uint8_t                        SDHC_AESR;          /**< Offset: 0x54 (R/   8) ADMA Error Status */
  __I   uint8_t                        Reserved2[0x03];
  __IO  uint32_t                       SDHC_ASAR[1];       /**< Offset: 0x58 (R/W  32) ADMA System Address n */
  __I   uint8_t                        Reserved3[0x04];
  __IO  uint16_t                       SDHC_PVR[8];        /**< Offset: 0x60 (R/W  16) Preset Value n */
  __I   uint8_t                        Reserved4[0x8C];
  __I   uint16_t                       SDHC_SISR;          /**< Offset: 0xFC (R/   16) Slot Interrupt Status */
  __I   uint16_t                       SDHC_HCVR;          /**< Offset: 0xFE (R/   16) Host Controller Version */
  __I   uint8_t                        Reserved5[0x104];
  __IO  uint8_t                        SDHC_MC1R;          /**< Offset: 0x204 (R/W  8) MMC Control 1 */
  __O   uint8_t                        SDHC_MC2R;          /**< Offset: 0x205 ( /W  8) MMC Control 2 */
  __I   uint8_t                        Reserved6[0x02];
  __IO  uint32_t                       SDHC_ACR;           /**< Offset: 0x208 (R/W  32) AHB Control */
  __IO  uint32_t                       SDHC_CC2R;          /**< Offset: 0x20C (R/W  32) Clock Control 2 */
  __I   uint8_t                        Reserved7[0x20];
  __IO  uint32_t                       SDHC_CACR;          /**< Offset: 0x230 (R/W  32) Capabilities Control */
  __IO  uint8_t                        SDHC_DBGR;          /**< Offset: 0x234 (R/W  8) Debug */
} sdhc_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_SDHC_COMPONENT_H_ */
