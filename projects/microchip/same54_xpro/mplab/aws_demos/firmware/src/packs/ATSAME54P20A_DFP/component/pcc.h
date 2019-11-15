/**
 * \brief Component description for PCC
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
#ifndef _SAME54_PCC_COMPONENT_H_
#define _SAME54_PCC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR PCC                                          */
/* ************************************************************************** */

/* -------- PCC_MR : (PCC Offset: 0x00) (R/W 32) Mode Register -------- */
#define PCC_MR_RESETVALUE                     _U_(0x00)                                            /**<  (PCC_MR) Mode Register  Reset Value */

#define PCC_MR_PCEN_Pos                       _U_(0)                                               /**< (PCC_MR) Parallel Capture Enable Position */
#define PCC_MR_PCEN_Msk                       (_U_(0x1) << PCC_MR_PCEN_Pos)                        /**< (PCC_MR) Parallel Capture Enable Mask */
#define PCC_MR_PCEN(value)                    (PCC_MR_PCEN_Msk & ((value) << PCC_MR_PCEN_Pos))    
#define PCC_MR_DSIZE_Pos                      _U_(4)                                               /**< (PCC_MR) Data size Position */
#define PCC_MR_DSIZE_Msk                      (_U_(0x3) << PCC_MR_DSIZE_Pos)                       /**< (PCC_MR) Data size Mask */
#define PCC_MR_DSIZE(value)                   (PCC_MR_DSIZE_Msk & ((value) << PCC_MR_DSIZE_Pos))  
#define PCC_MR_SCALE_Pos                      _U_(8)                                               /**< (PCC_MR) Scale data Position */
#define PCC_MR_SCALE_Msk                      (_U_(0x1) << PCC_MR_SCALE_Pos)                       /**< (PCC_MR) Scale data Mask */
#define PCC_MR_SCALE(value)                   (PCC_MR_SCALE_Msk & ((value) << PCC_MR_SCALE_Pos))  
#define PCC_MR_ALWYS_Pos                      _U_(9)                                               /**< (PCC_MR) Always Sampling Position */
#define PCC_MR_ALWYS_Msk                      (_U_(0x1) << PCC_MR_ALWYS_Pos)                       /**< (PCC_MR) Always Sampling Mask */
#define PCC_MR_ALWYS(value)                   (PCC_MR_ALWYS_Msk & ((value) << PCC_MR_ALWYS_Pos))  
#define PCC_MR_HALFS_Pos                      _U_(10)                                              /**< (PCC_MR) Half Sampling Position */
#define PCC_MR_HALFS_Msk                      (_U_(0x1) << PCC_MR_HALFS_Pos)                       /**< (PCC_MR) Half Sampling Mask */
#define PCC_MR_HALFS(value)                   (PCC_MR_HALFS_Msk & ((value) << PCC_MR_HALFS_Pos))  
#define PCC_MR_FRSTS_Pos                      _U_(11)                                              /**< (PCC_MR) First sample Position */
#define PCC_MR_FRSTS_Msk                      (_U_(0x1) << PCC_MR_FRSTS_Pos)                       /**< (PCC_MR) First sample Mask */
#define PCC_MR_FRSTS(value)                   (PCC_MR_FRSTS_Msk & ((value) << PCC_MR_FRSTS_Pos))  
#define PCC_MR_ISIZE_Pos                      _U_(16)                                              /**< (PCC_MR) Input Data Size Position */
#define PCC_MR_ISIZE_Msk                      (_U_(0x7) << PCC_MR_ISIZE_Pos)                       /**< (PCC_MR) Input Data Size Mask */
#define PCC_MR_ISIZE(value)                   (PCC_MR_ISIZE_Msk & ((value) << PCC_MR_ISIZE_Pos))  
#define PCC_MR_CID_Pos                        _U_(30)                                              /**< (PCC_MR) Clear If Disabled Position */
#define PCC_MR_CID_Msk                        (_U_(0x3) << PCC_MR_CID_Pos)                         /**< (PCC_MR) Clear If Disabled Mask */
#define PCC_MR_CID(value)                     (PCC_MR_CID_Msk & ((value) << PCC_MR_CID_Pos))      
#define PCC_MR_Msk                            _U_(0xC0070F31)                                      /**< (PCC_MR) Register Mask  */


/* -------- PCC_IER : (PCC Offset: 0x04) ( /W 32) Interrupt Enable Register -------- */
#define PCC_IER_RESETVALUE                    _U_(0x00)                                            /**<  (PCC_IER) Interrupt Enable Register  Reset Value */

#define PCC_IER_DRDY_Pos                      _U_(0)                                               /**< (PCC_IER) Data Ready Interrupt Enable Position */
#define PCC_IER_DRDY_Msk                      (_U_(0x1) << PCC_IER_DRDY_Pos)                       /**< (PCC_IER) Data Ready Interrupt Enable Mask */
#define PCC_IER_DRDY(value)                   (PCC_IER_DRDY_Msk & ((value) << PCC_IER_DRDY_Pos))  
#define PCC_IER_OVRE_Pos                      _U_(1)                                               /**< (PCC_IER) Overrun Error Interrupt Enable Position */
#define PCC_IER_OVRE_Msk                      (_U_(0x1) << PCC_IER_OVRE_Pos)                       /**< (PCC_IER) Overrun Error Interrupt Enable Mask */
#define PCC_IER_OVRE(value)                   (PCC_IER_OVRE_Msk & ((value) << PCC_IER_OVRE_Pos))  
#define PCC_IER_Msk                           _U_(0x00000003)                                      /**< (PCC_IER) Register Mask  */


/* -------- PCC_IDR : (PCC Offset: 0x08) ( /W 32) Interrupt Disable Register -------- */
#define PCC_IDR_RESETVALUE                    _U_(0x00)                                            /**<  (PCC_IDR) Interrupt Disable Register  Reset Value */

#define PCC_IDR_DRDY_Pos                      _U_(0)                                               /**< (PCC_IDR) Data Ready Interrupt Disable Position */
#define PCC_IDR_DRDY_Msk                      (_U_(0x1) << PCC_IDR_DRDY_Pos)                       /**< (PCC_IDR) Data Ready Interrupt Disable Mask */
#define PCC_IDR_DRDY(value)                   (PCC_IDR_DRDY_Msk & ((value) << PCC_IDR_DRDY_Pos))  
#define PCC_IDR_OVRE_Pos                      _U_(1)                                               /**< (PCC_IDR) Overrun Error Interrupt Disable Position */
#define PCC_IDR_OVRE_Msk                      (_U_(0x1) << PCC_IDR_OVRE_Pos)                       /**< (PCC_IDR) Overrun Error Interrupt Disable Mask */
#define PCC_IDR_OVRE(value)                   (PCC_IDR_OVRE_Msk & ((value) << PCC_IDR_OVRE_Pos))  
#define PCC_IDR_Msk                           _U_(0x00000003)                                      /**< (PCC_IDR) Register Mask  */


/* -------- PCC_IMR : (PCC Offset: 0x0C) ( R/ 32) Interrupt Mask Register -------- */
#define PCC_IMR_RESETVALUE                    _U_(0x00)                                            /**<  (PCC_IMR) Interrupt Mask Register  Reset Value */

#define PCC_IMR_DRDY_Pos                      _U_(0)                                               /**< (PCC_IMR) Data Ready Interrupt Mask Position */
#define PCC_IMR_DRDY_Msk                      (_U_(0x1) << PCC_IMR_DRDY_Pos)                       /**< (PCC_IMR) Data Ready Interrupt Mask Mask */
#define PCC_IMR_DRDY(value)                   (PCC_IMR_DRDY_Msk & ((value) << PCC_IMR_DRDY_Pos))  
#define PCC_IMR_OVRE_Pos                      _U_(1)                                               /**< (PCC_IMR) Overrun Error Interrupt Mask Position */
#define PCC_IMR_OVRE_Msk                      (_U_(0x1) << PCC_IMR_OVRE_Pos)                       /**< (PCC_IMR) Overrun Error Interrupt Mask Mask */
#define PCC_IMR_OVRE(value)                   (PCC_IMR_OVRE_Msk & ((value) << PCC_IMR_OVRE_Pos))  
#define PCC_IMR_Msk                           _U_(0x00000003)                                      /**< (PCC_IMR) Register Mask  */


/* -------- PCC_ISR : (PCC Offset: 0x10) ( R/ 32) Interrupt Status Register -------- */
#define PCC_ISR_RESETVALUE                    _U_(0x00)                                            /**<  (PCC_ISR) Interrupt Status Register  Reset Value */

#define PCC_ISR_DRDY_Pos                      _U_(0)                                               /**< (PCC_ISR) Data Ready Interrupt Status Position */
#define PCC_ISR_DRDY_Msk                      (_U_(0x1) << PCC_ISR_DRDY_Pos)                       /**< (PCC_ISR) Data Ready Interrupt Status Mask */
#define PCC_ISR_DRDY(value)                   (PCC_ISR_DRDY_Msk & ((value) << PCC_ISR_DRDY_Pos))  
#define PCC_ISR_OVRE_Pos                      _U_(1)                                               /**< (PCC_ISR) Overrun Error Interrupt Status Position */
#define PCC_ISR_OVRE_Msk                      (_U_(0x1) << PCC_ISR_OVRE_Pos)                       /**< (PCC_ISR) Overrun Error Interrupt Status Mask */
#define PCC_ISR_OVRE(value)                   (PCC_ISR_OVRE_Msk & ((value) << PCC_ISR_OVRE_Pos))  
#define PCC_ISR_Msk                           _U_(0x00000003)                                      /**< (PCC_ISR) Register Mask  */


/* -------- PCC_RHR : (PCC Offset: 0x14) ( R/ 32) Reception Holding Register -------- */
#define PCC_RHR_RESETVALUE                    _U_(0x00)                                            /**<  (PCC_RHR) Reception Holding Register  Reset Value */

#define PCC_RHR_RDATA_Pos                     _U_(0)                                               /**< (PCC_RHR) Reception Data Position */
#define PCC_RHR_RDATA_Msk                     (_U_(0xFFFFFFFF) << PCC_RHR_RDATA_Pos)               /**< (PCC_RHR) Reception Data Mask */
#define PCC_RHR_RDATA(value)                  (PCC_RHR_RDATA_Msk & ((value) << PCC_RHR_RDATA_Pos))
#define PCC_RHR_Msk                           _U_(0xFFFFFFFF)                                      /**< (PCC_RHR) Register Mask  */


/* -------- PCC_WPMR : (PCC Offset: 0xE0) (R/W 32) Write Protection Mode Register -------- */
#define PCC_WPMR_RESETVALUE                   _U_(0x00)                                            /**<  (PCC_WPMR) Write Protection Mode Register  Reset Value */

#define PCC_WPMR_WPEN_Pos                     _U_(0)                                               /**< (PCC_WPMR) Write Protection Enable Position */
#define PCC_WPMR_WPEN_Msk                     (_U_(0x1) << PCC_WPMR_WPEN_Pos)                      /**< (PCC_WPMR) Write Protection Enable Mask */
#define PCC_WPMR_WPEN(value)                  (PCC_WPMR_WPEN_Msk & ((value) << PCC_WPMR_WPEN_Pos))
#define PCC_WPMR_WPKEY_Pos                    _U_(8)                                               /**< (PCC_WPMR) Write Protection Key Position */
#define PCC_WPMR_WPKEY_Msk                    (_U_(0xFFFFFF) << PCC_WPMR_WPKEY_Pos)                /**< (PCC_WPMR) Write Protection Key Mask */
#define PCC_WPMR_WPKEY(value)                 (PCC_WPMR_WPKEY_Msk & ((value) << PCC_WPMR_WPKEY_Pos))
#define PCC_WPMR_Msk                          _U_(0xFFFFFF01)                                      /**< (PCC_WPMR) Register Mask  */


/* -------- PCC_WPSR : (PCC Offset: 0xE4) ( R/ 32) Write Protection Status Register -------- */
#define PCC_WPSR_RESETVALUE                   _U_(0x00)                                            /**<  (PCC_WPSR) Write Protection Status Register  Reset Value */

#define PCC_WPSR_WPVS_Pos                     _U_(0)                                               /**< (PCC_WPSR) Write Protection Violation Source Position */
#define PCC_WPSR_WPVS_Msk                     (_U_(0x1) << PCC_WPSR_WPVS_Pos)                      /**< (PCC_WPSR) Write Protection Violation Source Mask */
#define PCC_WPSR_WPVS(value)                  (PCC_WPSR_WPVS_Msk & ((value) << PCC_WPSR_WPVS_Pos))
#define PCC_WPSR_WPVSRC_Pos                   _U_(8)                                               /**< (PCC_WPSR) Write Protection Violation Status Position */
#define PCC_WPSR_WPVSRC_Msk                   (_U_(0xFFFF) << PCC_WPSR_WPVSRC_Pos)                 /**< (PCC_WPSR) Write Protection Violation Status Mask */
#define PCC_WPSR_WPVSRC(value)                (PCC_WPSR_WPVSRC_Msk & ((value) << PCC_WPSR_WPVSRC_Pos))
#define PCC_WPSR_Msk                          _U_(0x00FFFF01)                                      /**< (PCC_WPSR) Register Mask  */


/** \brief PCC register offsets definitions */
#define PCC_MR_REG_OFST                (0x00)              /**< (PCC_MR) Mode Register Offset */
#define PCC_IER_REG_OFST               (0x04)              /**< (PCC_IER) Interrupt Enable Register Offset */
#define PCC_IDR_REG_OFST               (0x08)              /**< (PCC_IDR) Interrupt Disable Register Offset */
#define PCC_IMR_REG_OFST               (0x0C)              /**< (PCC_IMR) Interrupt Mask Register Offset */
#define PCC_ISR_REG_OFST               (0x10)              /**< (PCC_ISR) Interrupt Status Register Offset */
#define PCC_RHR_REG_OFST               (0x14)              /**< (PCC_RHR) Reception Holding Register Offset */
#define PCC_WPMR_REG_OFST              (0xE0)              /**< (PCC_WPMR) Write Protection Mode Register Offset */
#define PCC_WPSR_REG_OFST              (0xE4)              /**< (PCC_WPSR) Write Protection Status Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief PCC register API structure */
typedef struct
{  /* Parallel Capture Controller */
  __IO  uint32_t                       PCC_MR;             /**< Offset: 0x00 (R/W  32) Mode Register */
  __O   uint32_t                       PCC_IER;            /**< Offset: 0x04 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       PCC_IDR;            /**< Offset: 0x08 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       PCC_IMR;            /**< Offset: 0x0C (R/   32) Interrupt Mask Register */
  __I   uint32_t                       PCC_ISR;            /**< Offset: 0x10 (R/   32) Interrupt Status Register */
  __I   uint32_t                       PCC_RHR;            /**< Offset: 0x14 (R/   32) Reception Holding Register */
  __I   uint8_t                        Reserved1[0xC8];
  __IO  uint32_t                       PCC_WPMR;           /**< Offset: 0xE0 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       PCC_WPSR;           /**< Offset: 0xE4 (R/   32) Write Protection Status Register */
} pcc_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_PCC_COMPONENT_H_ */
