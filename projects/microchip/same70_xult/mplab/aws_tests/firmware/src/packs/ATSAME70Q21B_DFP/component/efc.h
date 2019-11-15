/**
 * \brief Component description for EFC
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
#ifndef _SAME70_EFC_COMPONENT_H_
#define _SAME70_EFC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR EFC                                          */
/* ************************************************************************** */

/* -------- EEFC_FMR : (EFC Offset: 0x00) (R/W 32) EEFC Flash Mode Register -------- */
#define EEFC_FMR_FRDY_Pos                     _U_(0)                                               /**< (EEFC_FMR) Flash Ready Interrupt Enable Position */
#define EEFC_FMR_FRDY_Msk                     (_U_(0x1) << EEFC_FMR_FRDY_Pos)                      /**< (EEFC_FMR) Flash Ready Interrupt Enable Mask */
#define EEFC_FMR_FRDY(value)                  (EEFC_FMR_FRDY_Msk & ((value) << EEFC_FMR_FRDY_Pos))
#define EEFC_FMR_FWS_Pos                      _U_(8)                                               /**< (EEFC_FMR) Flash Wait State Position */
#define EEFC_FMR_FWS_Msk                      (_U_(0xF) << EEFC_FMR_FWS_Pos)                       /**< (EEFC_FMR) Flash Wait State Mask */
#define EEFC_FMR_FWS(value)                   (EEFC_FMR_FWS_Msk & ((value) << EEFC_FMR_FWS_Pos))  
#define EEFC_FMR_SCOD_Pos                     _U_(16)                                              /**< (EEFC_FMR) Sequential Code Optimization Disable Position */
#define EEFC_FMR_SCOD_Msk                     (_U_(0x1) << EEFC_FMR_SCOD_Pos)                      /**< (EEFC_FMR) Sequential Code Optimization Disable Mask */
#define EEFC_FMR_SCOD(value)                  (EEFC_FMR_SCOD_Msk & ((value) << EEFC_FMR_SCOD_Pos))
#define EEFC_FMR_CLOE_Pos                     _U_(26)                                              /**< (EEFC_FMR) Code Loop Optimization Enable Position */
#define EEFC_FMR_CLOE_Msk                     (_U_(0x1) << EEFC_FMR_CLOE_Pos)                      /**< (EEFC_FMR) Code Loop Optimization Enable Mask */
#define EEFC_FMR_CLOE(value)                  (EEFC_FMR_CLOE_Msk & ((value) << EEFC_FMR_CLOE_Pos))
#define EEFC_FMR_Msk                          _U_(0x04010F01)                                      /**< (EEFC_FMR) Register Mask  */


/* -------- EEFC_FCR : (EFC Offset: 0x04) ( /W 32) EEFC Flash Command Register -------- */
#define EEFC_FCR_FCMD_Pos                     _U_(0)                                               /**< (EEFC_FCR) Flash Command Position */
#define EEFC_FCR_FCMD_Msk                     (_U_(0xFF) << EEFC_FCR_FCMD_Pos)                     /**< (EEFC_FCR) Flash Command Mask */
#define EEFC_FCR_FCMD(value)                  (EEFC_FCR_FCMD_Msk & ((value) << EEFC_FCR_FCMD_Pos))
#define   EEFC_FCR_FCMD_GETD_Val              _U_(0x0)                                             /**< (EEFC_FCR) Get Flash descriptor  */
#define   EEFC_FCR_FCMD_WP_Val                _U_(0x1)                                             /**< (EEFC_FCR) Write page  */
#define   EEFC_FCR_FCMD_WPL_Val               _U_(0x2)                                             /**< (EEFC_FCR) Write page and lock  */
#define   EEFC_FCR_FCMD_EWP_Val               _U_(0x3)                                             /**< (EEFC_FCR) Erase page and write page  */
#define   EEFC_FCR_FCMD_EWPL_Val              _U_(0x4)                                             /**< (EEFC_FCR) Erase page and write page then lock  */
#define   EEFC_FCR_FCMD_EA_Val                _U_(0x5)                                             /**< (EEFC_FCR) Erase all  */
#define   EEFC_FCR_FCMD_EPA_Val               _U_(0x7)                                             /**< (EEFC_FCR) Erase pages  */
#define   EEFC_FCR_FCMD_SLB_Val               _U_(0x8)                                             /**< (EEFC_FCR) Set lock bit  */
#define   EEFC_FCR_FCMD_CLB_Val               _U_(0x9)                                             /**< (EEFC_FCR) Clear lock bit  */
#define   EEFC_FCR_FCMD_GLB_Val               _U_(0xA)                                             /**< (EEFC_FCR) Get lock bit  */
#define   EEFC_FCR_FCMD_SGPB_Val              _U_(0xB)                                             /**< (EEFC_FCR) Set GPNVM bit  */
#define   EEFC_FCR_FCMD_CGPB_Val              _U_(0xC)                                             /**< (EEFC_FCR) Clear GPNVM bit  */
#define   EEFC_FCR_FCMD_GGPB_Val              _U_(0xD)                                             /**< (EEFC_FCR) Get GPNVM bit  */
#define   EEFC_FCR_FCMD_STUI_Val              _U_(0xE)                                             /**< (EEFC_FCR) Start read unique identifier  */
#define   EEFC_FCR_FCMD_SPUI_Val              _U_(0xF)                                             /**< (EEFC_FCR) Stop read unique identifier  */
#define   EEFC_FCR_FCMD_GCALB_Val             _U_(0x10)                                            /**< (EEFC_FCR) Get CALIB bit  */
#define   EEFC_FCR_FCMD_ES_Val                _U_(0x11)                                            /**< (EEFC_FCR) Erase sector  */
#define   EEFC_FCR_FCMD_WUS_Val               _U_(0x12)                                            /**< (EEFC_FCR) Write user signature  */
#define   EEFC_FCR_FCMD_EUS_Val               _U_(0x13)                                            /**< (EEFC_FCR) Erase user signature  */
#define   EEFC_FCR_FCMD_STUS_Val              _U_(0x14)                                            /**< (EEFC_FCR) Start read user signature  */
#define   EEFC_FCR_FCMD_SPUS_Val              _U_(0x15)                                            /**< (EEFC_FCR) Stop read user signature  */
#define EEFC_FCR_FCMD_GETD                    (EEFC_FCR_FCMD_GETD_Val << EEFC_FCR_FCMD_Pos)        /**< (EEFC_FCR) Get Flash descriptor Position  */
#define EEFC_FCR_FCMD_WP                      (EEFC_FCR_FCMD_WP_Val << EEFC_FCR_FCMD_Pos)          /**< (EEFC_FCR) Write page Position  */
#define EEFC_FCR_FCMD_WPL                     (EEFC_FCR_FCMD_WPL_Val << EEFC_FCR_FCMD_Pos)         /**< (EEFC_FCR) Write page and lock Position  */
#define EEFC_FCR_FCMD_EWP                     (EEFC_FCR_FCMD_EWP_Val << EEFC_FCR_FCMD_Pos)         /**< (EEFC_FCR) Erase page and write page Position  */
#define EEFC_FCR_FCMD_EWPL                    (EEFC_FCR_FCMD_EWPL_Val << EEFC_FCR_FCMD_Pos)        /**< (EEFC_FCR) Erase page and write page then lock Position  */
#define EEFC_FCR_FCMD_EA                      (EEFC_FCR_FCMD_EA_Val << EEFC_FCR_FCMD_Pos)          /**< (EEFC_FCR) Erase all Position  */
#define EEFC_FCR_FCMD_EPA                     (EEFC_FCR_FCMD_EPA_Val << EEFC_FCR_FCMD_Pos)         /**< (EEFC_FCR) Erase pages Position  */
#define EEFC_FCR_FCMD_SLB                     (EEFC_FCR_FCMD_SLB_Val << EEFC_FCR_FCMD_Pos)         /**< (EEFC_FCR) Set lock bit Position  */
#define EEFC_FCR_FCMD_CLB                     (EEFC_FCR_FCMD_CLB_Val << EEFC_FCR_FCMD_Pos)         /**< (EEFC_FCR) Clear lock bit Position  */
#define EEFC_FCR_FCMD_GLB                     (EEFC_FCR_FCMD_GLB_Val << EEFC_FCR_FCMD_Pos)         /**< (EEFC_FCR) Get lock bit Position  */
#define EEFC_FCR_FCMD_SGPB                    (EEFC_FCR_FCMD_SGPB_Val << EEFC_FCR_FCMD_Pos)        /**< (EEFC_FCR) Set GPNVM bit Position  */
#define EEFC_FCR_FCMD_CGPB                    (EEFC_FCR_FCMD_CGPB_Val << EEFC_FCR_FCMD_Pos)        /**< (EEFC_FCR) Clear GPNVM bit Position  */
#define EEFC_FCR_FCMD_GGPB                    (EEFC_FCR_FCMD_GGPB_Val << EEFC_FCR_FCMD_Pos)        /**< (EEFC_FCR) Get GPNVM bit Position  */
#define EEFC_FCR_FCMD_STUI                    (EEFC_FCR_FCMD_STUI_Val << EEFC_FCR_FCMD_Pos)        /**< (EEFC_FCR) Start read unique identifier Position  */
#define EEFC_FCR_FCMD_SPUI                    (EEFC_FCR_FCMD_SPUI_Val << EEFC_FCR_FCMD_Pos)        /**< (EEFC_FCR) Stop read unique identifier Position  */
#define EEFC_FCR_FCMD_GCALB                   (EEFC_FCR_FCMD_GCALB_Val << EEFC_FCR_FCMD_Pos)       /**< (EEFC_FCR) Get CALIB bit Position  */
#define EEFC_FCR_FCMD_ES                      (EEFC_FCR_FCMD_ES_Val << EEFC_FCR_FCMD_Pos)          /**< (EEFC_FCR) Erase sector Position  */
#define EEFC_FCR_FCMD_WUS                     (EEFC_FCR_FCMD_WUS_Val << EEFC_FCR_FCMD_Pos)         /**< (EEFC_FCR) Write user signature Position  */
#define EEFC_FCR_FCMD_EUS                     (EEFC_FCR_FCMD_EUS_Val << EEFC_FCR_FCMD_Pos)         /**< (EEFC_FCR) Erase user signature Position  */
#define EEFC_FCR_FCMD_STUS                    (EEFC_FCR_FCMD_STUS_Val << EEFC_FCR_FCMD_Pos)        /**< (EEFC_FCR) Start read user signature Position  */
#define EEFC_FCR_FCMD_SPUS                    (EEFC_FCR_FCMD_SPUS_Val << EEFC_FCR_FCMD_Pos)        /**< (EEFC_FCR) Stop read user signature Position  */
#define EEFC_FCR_FARG_Pos                     _U_(8)                                               /**< (EEFC_FCR) Flash Command Argument Position */
#define EEFC_FCR_FARG_Msk                     (_U_(0xFFFF) << EEFC_FCR_FARG_Pos)                   /**< (EEFC_FCR) Flash Command Argument Mask */
#define EEFC_FCR_FARG(value)                  (EEFC_FCR_FARG_Msk & ((value) << EEFC_FCR_FARG_Pos))
#define EEFC_FCR_FKEY_Pos                     _U_(24)                                              /**< (EEFC_FCR) Flash Writing Protection Key Position */
#define EEFC_FCR_FKEY_Msk                     (_U_(0xFF) << EEFC_FCR_FKEY_Pos)                     /**< (EEFC_FCR) Flash Writing Protection Key Mask */
#define EEFC_FCR_FKEY(value)                  (EEFC_FCR_FKEY_Msk & ((value) << EEFC_FCR_FKEY_Pos))
#define   EEFC_FCR_FKEY_PASSWD_Val            _U_(0x5A)                                            /**< (EEFC_FCR) The 0x5A value enables the command defined by the bits of the register. If the field is written with a different value, the write is not performed and no action is started.  */
#define EEFC_FCR_FKEY_PASSWD                  (EEFC_FCR_FKEY_PASSWD_Val << EEFC_FCR_FKEY_Pos)      /**< (EEFC_FCR) The 0x5A value enables the command defined by the bits of the register. If the field is written with a different value, the write is not performed and no action is started. Position  */
#define EEFC_FCR_Msk                          _U_(0xFFFFFFFF)                                      /**< (EEFC_FCR) Register Mask  */


/* -------- EEFC_FSR : (EFC Offset: 0x08) ( R/ 32) EEFC Flash Status Register -------- */
#define EEFC_FSR_FRDY_Pos                     _U_(0)                                               /**< (EEFC_FSR) Flash Ready Status (cleared when Flash is busy) Position */
#define EEFC_FSR_FRDY_Msk                     (_U_(0x1) << EEFC_FSR_FRDY_Pos)                      /**< (EEFC_FSR) Flash Ready Status (cleared when Flash is busy) Mask */
#define EEFC_FSR_FRDY(value)                  (EEFC_FSR_FRDY_Msk & ((value) << EEFC_FSR_FRDY_Pos))
#define EEFC_FSR_FCMDE_Pos                    _U_(1)                                               /**< (EEFC_FSR) Flash Command Error Status (cleared on read or by writing EEFC_FCR) Position */
#define EEFC_FSR_FCMDE_Msk                    (_U_(0x1) << EEFC_FSR_FCMDE_Pos)                     /**< (EEFC_FSR) Flash Command Error Status (cleared on read or by writing EEFC_FCR) Mask */
#define EEFC_FSR_FCMDE(value)                 (EEFC_FSR_FCMDE_Msk & ((value) << EEFC_FSR_FCMDE_Pos))
#define EEFC_FSR_FLOCKE_Pos                   _U_(2)                                               /**< (EEFC_FSR) Flash Lock Error Status (cleared on read) Position */
#define EEFC_FSR_FLOCKE_Msk                   (_U_(0x1) << EEFC_FSR_FLOCKE_Pos)                    /**< (EEFC_FSR) Flash Lock Error Status (cleared on read) Mask */
#define EEFC_FSR_FLOCKE(value)                (EEFC_FSR_FLOCKE_Msk & ((value) << EEFC_FSR_FLOCKE_Pos))
#define EEFC_FSR_FLERR_Pos                    _U_(3)                                               /**< (EEFC_FSR) Flash Error Status (cleared when a programming operation starts) Position */
#define EEFC_FSR_FLERR_Msk                    (_U_(0x1) << EEFC_FSR_FLERR_Pos)                     /**< (EEFC_FSR) Flash Error Status (cleared when a programming operation starts) Mask */
#define EEFC_FSR_FLERR(value)                 (EEFC_FSR_FLERR_Msk & ((value) << EEFC_FSR_FLERR_Pos))
#define EEFC_FSR_UECCELSB_Pos                 _U_(16)                                              /**< (EEFC_FSR) Unique ECC Error on LSB Part of the Memory Flash Data Bus (cleared on read) Position */
#define EEFC_FSR_UECCELSB_Msk                 (_U_(0x1) << EEFC_FSR_UECCELSB_Pos)                  /**< (EEFC_FSR) Unique ECC Error on LSB Part of the Memory Flash Data Bus (cleared on read) Mask */
#define EEFC_FSR_UECCELSB(value)              (EEFC_FSR_UECCELSB_Msk & ((value) << EEFC_FSR_UECCELSB_Pos))
#define EEFC_FSR_MECCELSB_Pos                 _U_(17)                                              /**< (EEFC_FSR) Multiple ECC Error on LSB Part of the Memory Flash Data Bus (cleared on read) Position */
#define EEFC_FSR_MECCELSB_Msk                 (_U_(0x1) << EEFC_FSR_MECCELSB_Pos)                  /**< (EEFC_FSR) Multiple ECC Error on LSB Part of the Memory Flash Data Bus (cleared on read) Mask */
#define EEFC_FSR_MECCELSB(value)              (EEFC_FSR_MECCELSB_Msk & ((value) << EEFC_FSR_MECCELSB_Pos))
#define EEFC_FSR_UECCEMSB_Pos                 _U_(18)                                              /**< (EEFC_FSR) Unique ECC Error on MSB Part of the Memory Flash Data Bus (cleared on read) Position */
#define EEFC_FSR_UECCEMSB_Msk                 (_U_(0x1) << EEFC_FSR_UECCEMSB_Pos)                  /**< (EEFC_FSR) Unique ECC Error on MSB Part of the Memory Flash Data Bus (cleared on read) Mask */
#define EEFC_FSR_UECCEMSB(value)              (EEFC_FSR_UECCEMSB_Msk & ((value) << EEFC_FSR_UECCEMSB_Pos))
#define EEFC_FSR_MECCEMSB_Pos                 _U_(19)                                              /**< (EEFC_FSR) Multiple ECC Error on MSB Part of the Memory Flash Data Bus (cleared on read) Position */
#define EEFC_FSR_MECCEMSB_Msk                 (_U_(0x1) << EEFC_FSR_MECCEMSB_Pos)                  /**< (EEFC_FSR) Multiple ECC Error on MSB Part of the Memory Flash Data Bus (cleared on read) Mask */
#define EEFC_FSR_MECCEMSB(value)              (EEFC_FSR_MECCEMSB_Msk & ((value) << EEFC_FSR_MECCEMSB_Pos))
#define EEFC_FSR_Msk                          _U_(0x000F000F)                                      /**< (EEFC_FSR) Register Mask  */


/* -------- EEFC_FRR : (EFC Offset: 0x0C) ( R/ 32) EEFC Flash Result Register -------- */
#define EEFC_FRR_FVALUE_Pos                   _U_(0)                                               /**< (EEFC_FRR) Flash Result Value Position */
#define EEFC_FRR_FVALUE_Msk                   (_U_(0xFFFFFFFF) << EEFC_FRR_FVALUE_Pos)             /**< (EEFC_FRR) Flash Result Value Mask */
#define EEFC_FRR_FVALUE(value)                (EEFC_FRR_FVALUE_Msk & ((value) << EEFC_FRR_FVALUE_Pos))
#define EEFC_FRR_Msk                          _U_(0xFFFFFFFF)                                      /**< (EEFC_FRR) Register Mask  */


/* -------- EEFC_WPMR : (EFC Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define EEFC_WPMR_WPEN_Pos                    _U_(0)                                               /**< (EEFC_WPMR) Write Protection Enable Position */
#define EEFC_WPMR_WPEN_Msk                    (_U_(0x1) << EEFC_WPMR_WPEN_Pos)                     /**< (EEFC_WPMR) Write Protection Enable Mask */
#define EEFC_WPMR_WPEN(value)                 (EEFC_WPMR_WPEN_Msk & ((value) << EEFC_WPMR_WPEN_Pos))
#define EEFC_WPMR_WPKEY_Pos                   _U_(8)                                               /**< (EEFC_WPMR) Write Protection Key Position */
#define EEFC_WPMR_WPKEY_Msk                   (_U_(0xFFFFFF) << EEFC_WPMR_WPKEY_Pos)               /**< (EEFC_WPMR) Write Protection Key Mask */
#define EEFC_WPMR_WPKEY(value)                (EEFC_WPMR_WPKEY_Msk & ((value) << EEFC_WPMR_WPKEY_Pos))
#define   EEFC_WPMR_WPKEY_PASSWD_Val          _U_(0x454643)                                        /**< (EEFC_WPMR) Writing any other value in this field aborts the write operation.Always reads as 0.  */
#define EEFC_WPMR_WPKEY_PASSWD                (EEFC_WPMR_WPKEY_PASSWD_Val << EEFC_WPMR_WPKEY_Pos)  /**< (EEFC_WPMR) Writing any other value in this field aborts the write operation.Always reads as 0. Position  */
#define EEFC_WPMR_Msk                         _U_(0xFFFFFF01)                                      /**< (EEFC_WPMR) Register Mask  */


/** \brief EFC register offsets definitions */
#define EEFC_FMR_REG_OFST              (0x00)              /**< (EEFC_FMR) EEFC Flash Mode Register Offset */
#define EEFC_FCR_REG_OFST              (0x04)              /**< (EEFC_FCR) EEFC Flash Command Register Offset */
#define EEFC_FSR_REG_OFST              (0x08)              /**< (EEFC_FSR) EEFC Flash Status Register Offset */
#define EEFC_FRR_REG_OFST              (0x0C)              /**< (EEFC_FRR) EEFC Flash Result Register Offset */
#define EEFC_WPMR_REG_OFST             (0xE4)              /**< (EEFC_WPMR) Write Protection Mode Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief EFC register API structure */
typedef struct
{
  __IO  uint32_t                       EEFC_FMR;           /**< Offset: 0x00 (R/W  32) EEFC Flash Mode Register */
  __O   uint32_t                       EEFC_FCR;           /**< Offset: 0x04 ( /W  32) EEFC Flash Command Register */
  __I   uint32_t                       EEFC_FSR;           /**< Offset: 0x08 (R/   32) EEFC Flash Status Register */
  __I   uint32_t                       EEFC_FRR;           /**< Offset: 0x0C (R/   32) EEFC Flash Result Register */
  __I   uint8_t                        Reserved1[0xD4];
  __IO  uint32_t                       EEFC_WPMR;          /**< Offset: 0xE4 (R/W  32) Write Protection Mode Register */
} efc_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_EFC_COMPONENT_H_ */
