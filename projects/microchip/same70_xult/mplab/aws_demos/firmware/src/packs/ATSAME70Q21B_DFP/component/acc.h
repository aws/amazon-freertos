/**
 * \brief Component description for ACC
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
#ifndef _SAME70_ACC_COMPONENT_H_
#define _SAME70_ACC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR ACC                                          */
/* ************************************************************************** */

/* -------- ACC_CR : (ACC Offset: 0x00) ( /W 32) Control Register -------- */
#define ACC_CR_SWRST_Pos                      _U_(0)                                               /**< (ACC_CR) Software Reset Position */
#define ACC_CR_SWRST_Msk                      (_U_(0x1) << ACC_CR_SWRST_Pos)                       /**< (ACC_CR) Software Reset Mask */
#define ACC_CR_SWRST(value)                   (ACC_CR_SWRST_Msk & ((value) << ACC_CR_SWRST_Pos))  
#define ACC_CR_Msk                            _U_(0x00000001)                                      /**< (ACC_CR) Register Mask  */


/* -------- ACC_MR : (ACC Offset: 0x04) (R/W 32) Mode Register -------- */
#define ACC_MR_SELMINUS_Pos                   _U_(0)                                               /**< (ACC_MR) Selection for Minus Comparator Input Position */
#define ACC_MR_SELMINUS_Msk                   (_U_(0x7) << ACC_MR_SELMINUS_Pos)                    /**< (ACC_MR) Selection for Minus Comparator Input Mask */
#define ACC_MR_SELMINUS(value)                (ACC_MR_SELMINUS_Msk & ((value) << ACC_MR_SELMINUS_Pos))
#define   ACC_MR_SELMINUS_TS_Val              _U_(0x0)                                             /**< (ACC_MR) Select TS  */
#define   ACC_MR_SELMINUS_VREFP_Val           _U_(0x1)                                             /**< (ACC_MR) Select VREFP  */
#define   ACC_MR_SELMINUS_DAC0_Val            _U_(0x2)                                             /**< (ACC_MR) Select DAC0  */
#define   ACC_MR_SELMINUS_DAC1_Val            _U_(0x3)                                             /**< (ACC_MR) Select DAC1  */
#define   ACC_MR_SELMINUS_AFE0_AD0_Val        _U_(0x4)                                             /**< (ACC_MR) Select AFE0_AD0  */
#define   ACC_MR_SELMINUS_AFE0_AD1_Val        _U_(0x5)                                             /**< (ACC_MR) Select AFE0_AD1  */
#define   ACC_MR_SELMINUS_AFE0_AD2_Val        _U_(0x6)                                             /**< (ACC_MR) Select AFE0_AD2  */
#define   ACC_MR_SELMINUS_AFE0_AD3_Val        _U_(0x7)                                             /**< (ACC_MR) Select AFE0_AD3  */
#define ACC_MR_SELMINUS_TS                    (ACC_MR_SELMINUS_TS_Val << ACC_MR_SELMINUS_Pos)      /**< (ACC_MR) Select TS Position  */
#define ACC_MR_SELMINUS_VREFP                 (ACC_MR_SELMINUS_VREFP_Val << ACC_MR_SELMINUS_Pos)   /**< (ACC_MR) Select VREFP Position  */
#define ACC_MR_SELMINUS_DAC0                  (ACC_MR_SELMINUS_DAC0_Val << ACC_MR_SELMINUS_Pos)    /**< (ACC_MR) Select DAC0 Position  */
#define ACC_MR_SELMINUS_DAC1                  (ACC_MR_SELMINUS_DAC1_Val << ACC_MR_SELMINUS_Pos)    /**< (ACC_MR) Select DAC1 Position  */
#define ACC_MR_SELMINUS_AFE0_AD0              (ACC_MR_SELMINUS_AFE0_AD0_Val << ACC_MR_SELMINUS_Pos) /**< (ACC_MR) Select AFE0_AD0 Position  */
#define ACC_MR_SELMINUS_AFE0_AD1              (ACC_MR_SELMINUS_AFE0_AD1_Val << ACC_MR_SELMINUS_Pos) /**< (ACC_MR) Select AFE0_AD1 Position  */
#define ACC_MR_SELMINUS_AFE0_AD2              (ACC_MR_SELMINUS_AFE0_AD2_Val << ACC_MR_SELMINUS_Pos) /**< (ACC_MR) Select AFE0_AD2 Position  */
#define ACC_MR_SELMINUS_AFE0_AD3              (ACC_MR_SELMINUS_AFE0_AD3_Val << ACC_MR_SELMINUS_Pos) /**< (ACC_MR) Select AFE0_AD3 Position  */
#define ACC_MR_SELPLUS_Pos                    _U_(4)                                               /**< (ACC_MR) Selection For Plus Comparator Input Position */
#define ACC_MR_SELPLUS_Msk                    (_U_(0x7) << ACC_MR_SELPLUS_Pos)                     /**< (ACC_MR) Selection For Plus Comparator Input Mask */
#define ACC_MR_SELPLUS(value)                 (ACC_MR_SELPLUS_Msk & ((value) << ACC_MR_SELPLUS_Pos))
#define   ACC_MR_SELPLUS_AFE0_AD0_Val         _U_(0x0)                                             /**< (ACC_MR) Select AFE0_AD0  */
#define   ACC_MR_SELPLUS_AFE0_AD1_Val         _U_(0x1)                                             /**< (ACC_MR) Select AFE0_AD1  */
#define   ACC_MR_SELPLUS_AFE0_AD2_Val         _U_(0x2)                                             /**< (ACC_MR) Select AFE0_AD2  */
#define   ACC_MR_SELPLUS_AFE0_AD3_Val         _U_(0x3)                                             /**< (ACC_MR) Select AFE0_AD3  */
#define   ACC_MR_SELPLUS_AFE0_AD4_Val         _U_(0x4)                                             /**< (ACC_MR) Select AFE0_AD4  */
#define   ACC_MR_SELPLUS_AFE0_AD5_Val         _U_(0x5)                                             /**< (ACC_MR) Select AFE0_AD5  */
#define   ACC_MR_SELPLUS_AFE1_AD0_Val         _U_(0x6)                                             /**< (ACC_MR) Select AFE1_AD0  */
#define   ACC_MR_SELPLUS_AFE1_AD1_Val         _U_(0x7)                                             /**< (ACC_MR) Select AFE1_AD1  */
#define ACC_MR_SELPLUS_AFE0_AD0               (ACC_MR_SELPLUS_AFE0_AD0_Val << ACC_MR_SELPLUS_Pos)  /**< (ACC_MR) Select AFE0_AD0 Position  */
#define ACC_MR_SELPLUS_AFE0_AD1               (ACC_MR_SELPLUS_AFE0_AD1_Val << ACC_MR_SELPLUS_Pos)  /**< (ACC_MR) Select AFE0_AD1 Position  */
#define ACC_MR_SELPLUS_AFE0_AD2               (ACC_MR_SELPLUS_AFE0_AD2_Val << ACC_MR_SELPLUS_Pos)  /**< (ACC_MR) Select AFE0_AD2 Position  */
#define ACC_MR_SELPLUS_AFE0_AD3               (ACC_MR_SELPLUS_AFE0_AD3_Val << ACC_MR_SELPLUS_Pos)  /**< (ACC_MR) Select AFE0_AD3 Position  */
#define ACC_MR_SELPLUS_AFE0_AD4               (ACC_MR_SELPLUS_AFE0_AD4_Val << ACC_MR_SELPLUS_Pos)  /**< (ACC_MR) Select AFE0_AD4 Position  */
#define ACC_MR_SELPLUS_AFE0_AD5               (ACC_MR_SELPLUS_AFE0_AD5_Val << ACC_MR_SELPLUS_Pos)  /**< (ACC_MR) Select AFE0_AD5 Position  */
#define ACC_MR_SELPLUS_AFE1_AD0               (ACC_MR_SELPLUS_AFE1_AD0_Val << ACC_MR_SELPLUS_Pos)  /**< (ACC_MR) Select AFE1_AD0 Position  */
#define ACC_MR_SELPLUS_AFE1_AD1               (ACC_MR_SELPLUS_AFE1_AD1_Val << ACC_MR_SELPLUS_Pos)  /**< (ACC_MR) Select AFE1_AD1 Position  */
#define ACC_MR_ACEN_Pos                       _U_(8)                                               /**< (ACC_MR) Analog Comparator Enable Position */
#define ACC_MR_ACEN_Msk                       (_U_(0x1) << ACC_MR_ACEN_Pos)                        /**< (ACC_MR) Analog Comparator Enable Mask */
#define ACC_MR_ACEN(value)                    (ACC_MR_ACEN_Msk & ((value) << ACC_MR_ACEN_Pos))    
#define   ACC_MR_ACEN_DIS_Val                 _U_(0x0)                                             /**< (ACC_MR) Analog comparator disabled.  */
#define   ACC_MR_ACEN_EN_Val                  _U_(0x1)                                             /**< (ACC_MR) Analog comparator enabled.  */
#define ACC_MR_ACEN_DIS                       (ACC_MR_ACEN_DIS_Val << ACC_MR_ACEN_Pos)             /**< (ACC_MR) Analog comparator disabled. Position  */
#define ACC_MR_ACEN_EN                        (ACC_MR_ACEN_EN_Val << ACC_MR_ACEN_Pos)              /**< (ACC_MR) Analog comparator enabled. Position  */
#define ACC_MR_EDGETYP_Pos                    _U_(9)                                               /**< (ACC_MR) Edge Type Position */
#define ACC_MR_EDGETYP_Msk                    (_U_(0x3) << ACC_MR_EDGETYP_Pos)                     /**< (ACC_MR) Edge Type Mask */
#define ACC_MR_EDGETYP(value)                 (ACC_MR_EDGETYP_Msk & ((value) << ACC_MR_EDGETYP_Pos))
#define   ACC_MR_EDGETYP_RISING_Val           _U_(0x0)                                             /**< (ACC_MR) Only rising edge of comparator output  */
#define   ACC_MR_EDGETYP_FALLING_Val          _U_(0x1)                                             /**< (ACC_MR) Falling edge of comparator output  */
#define   ACC_MR_EDGETYP_ANY_Val              _U_(0x2)                                             /**< (ACC_MR) Any edge of comparator output  */
#define ACC_MR_EDGETYP_RISING                 (ACC_MR_EDGETYP_RISING_Val << ACC_MR_EDGETYP_Pos)    /**< (ACC_MR) Only rising edge of comparator output Position  */
#define ACC_MR_EDGETYP_FALLING                (ACC_MR_EDGETYP_FALLING_Val << ACC_MR_EDGETYP_Pos)   /**< (ACC_MR) Falling edge of comparator output Position  */
#define ACC_MR_EDGETYP_ANY                    (ACC_MR_EDGETYP_ANY_Val << ACC_MR_EDGETYP_Pos)       /**< (ACC_MR) Any edge of comparator output Position  */
#define ACC_MR_INV_Pos                        _U_(12)                                              /**< (ACC_MR) Invert Comparator Output Position */
#define ACC_MR_INV_Msk                        (_U_(0x1) << ACC_MR_INV_Pos)                         /**< (ACC_MR) Invert Comparator Output Mask */
#define ACC_MR_INV(value)                     (ACC_MR_INV_Msk & ((value) << ACC_MR_INV_Pos))      
#define   ACC_MR_INV_DIS_Val                  _U_(0x0)                                             /**< (ACC_MR) Analog comparator output is directly processed.  */
#define   ACC_MR_INV_EN_Val                   _U_(0x1)                                             /**< (ACC_MR) Analog comparator output is inverted prior to being processed.  */
#define ACC_MR_INV_DIS                        (ACC_MR_INV_DIS_Val << ACC_MR_INV_Pos)               /**< (ACC_MR) Analog comparator output is directly processed. Position  */
#define ACC_MR_INV_EN                         (ACC_MR_INV_EN_Val << ACC_MR_INV_Pos)                /**< (ACC_MR) Analog comparator output is inverted prior to being processed. Position  */
#define ACC_MR_SELFS_Pos                      _U_(13)                                              /**< (ACC_MR) Selection Of Fault Source Position */
#define ACC_MR_SELFS_Msk                      (_U_(0x1) << ACC_MR_SELFS_Pos)                       /**< (ACC_MR) Selection Of Fault Source Mask */
#define ACC_MR_SELFS(value)                   (ACC_MR_SELFS_Msk & ((value) << ACC_MR_SELFS_Pos))  
#define   ACC_MR_SELFS_CE_Val                 _U_(0x0)                                             /**< (ACC_MR) The CE flag is used to drive the FAULT output.  */
#define   ACC_MR_SELFS_OUTPUT_Val             _U_(0x1)                                             /**< (ACC_MR) The output of the analog comparator flag is used to drive the FAULT output.  */
#define ACC_MR_SELFS_CE                       (ACC_MR_SELFS_CE_Val << ACC_MR_SELFS_Pos)            /**< (ACC_MR) The CE flag is used to drive the FAULT output. Position  */
#define ACC_MR_SELFS_OUTPUT                   (ACC_MR_SELFS_OUTPUT_Val << ACC_MR_SELFS_Pos)        /**< (ACC_MR) The output of the analog comparator flag is used to drive the FAULT output. Position  */
#define ACC_MR_FE_Pos                         _U_(14)                                              /**< (ACC_MR) Fault Enable Position */
#define ACC_MR_FE_Msk                         (_U_(0x1) << ACC_MR_FE_Pos)                          /**< (ACC_MR) Fault Enable Mask */
#define ACC_MR_FE(value)                      (ACC_MR_FE_Msk & ((value) << ACC_MR_FE_Pos))        
#define   ACC_MR_FE_DIS_Val                   _U_(0x0)                                             /**< (ACC_MR) The FAULT output is tied to 0.  */
#define   ACC_MR_FE_EN_Val                    _U_(0x1)                                             /**< (ACC_MR) The FAULT output is driven by the signal defined by SELFS.  */
#define ACC_MR_FE_DIS                         (ACC_MR_FE_DIS_Val << ACC_MR_FE_Pos)                 /**< (ACC_MR) The FAULT output is tied to 0. Position  */
#define ACC_MR_FE_EN                          (ACC_MR_FE_EN_Val << ACC_MR_FE_Pos)                  /**< (ACC_MR) The FAULT output is driven by the signal defined by SELFS. Position  */
#define ACC_MR_Msk                            _U_(0x00007777)                                      /**< (ACC_MR) Register Mask  */


/* -------- ACC_IER : (ACC Offset: 0x24) ( /W 32) Interrupt Enable Register -------- */
#define ACC_IER_CE_Pos                        _U_(0)                                               /**< (ACC_IER) Comparison Edge Position */
#define ACC_IER_CE_Msk                        (_U_(0x1) << ACC_IER_CE_Pos)                         /**< (ACC_IER) Comparison Edge Mask */
#define ACC_IER_CE(value)                     (ACC_IER_CE_Msk & ((value) << ACC_IER_CE_Pos))      
#define ACC_IER_Msk                           _U_(0x00000001)                                      /**< (ACC_IER) Register Mask  */


/* -------- ACC_IDR : (ACC Offset: 0x28) ( /W 32) Interrupt Disable Register -------- */
#define ACC_IDR_CE_Pos                        _U_(0)                                               /**< (ACC_IDR) Comparison Edge Position */
#define ACC_IDR_CE_Msk                        (_U_(0x1) << ACC_IDR_CE_Pos)                         /**< (ACC_IDR) Comparison Edge Mask */
#define ACC_IDR_CE(value)                     (ACC_IDR_CE_Msk & ((value) << ACC_IDR_CE_Pos))      
#define ACC_IDR_Msk                           _U_(0x00000001)                                      /**< (ACC_IDR) Register Mask  */


/* -------- ACC_IMR : (ACC Offset: 0x2C) ( R/ 32) Interrupt Mask Register -------- */
#define ACC_IMR_CE_Pos                        _U_(0)                                               /**< (ACC_IMR) Comparison Edge Position */
#define ACC_IMR_CE_Msk                        (_U_(0x1) << ACC_IMR_CE_Pos)                         /**< (ACC_IMR) Comparison Edge Mask */
#define ACC_IMR_CE(value)                     (ACC_IMR_CE_Msk & ((value) << ACC_IMR_CE_Pos))      
#define ACC_IMR_Msk                           _U_(0x00000001)                                      /**< (ACC_IMR) Register Mask  */


/* -------- ACC_ISR : (ACC Offset: 0x30) ( R/ 32) Interrupt Status Register -------- */
#define ACC_ISR_CE_Pos                        _U_(0)                                               /**< (ACC_ISR) Comparison Edge (cleared on read) Position */
#define ACC_ISR_CE_Msk                        (_U_(0x1) << ACC_ISR_CE_Pos)                         /**< (ACC_ISR) Comparison Edge (cleared on read) Mask */
#define ACC_ISR_CE(value)                     (ACC_ISR_CE_Msk & ((value) << ACC_ISR_CE_Pos))      
#define ACC_ISR_SCO_Pos                       _U_(1)                                               /**< (ACC_ISR) Synchronized Comparator Output Position */
#define ACC_ISR_SCO_Msk                       (_U_(0x1) << ACC_ISR_SCO_Pos)                        /**< (ACC_ISR) Synchronized Comparator Output Mask */
#define ACC_ISR_SCO(value)                    (ACC_ISR_SCO_Msk & ((value) << ACC_ISR_SCO_Pos))    
#define ACC_ISR_MASK_Pos                      _U_(31)                                              /**< (ACC_ISR) Flag Mask Position */
#define ACC_ISR_MASK_Msk                      (_U_(0x1) << ACC_ISR_MASK_Pos)                       /**< (ACC_ISR) Flag Mask Mask */
#define ACC_ISR_MASK(value)                   (ACC_ISR_MASK_Msk & ((value) << ACC_ISR_MASK_Pos))  
#define ACC_ISR_Msk                           _U_(0x80000003)                                      /**< (ACC_ISR) Register Mask  */


/* -------- ACC_ACR : (ACC Offset: 0x94) (R/W 32) Analog Control Register -------- */
#define ACC_ACR_ISEL_Pos                      _U_(0)                                               /**< (ACC_ACR) Current Selection Position */
#define ACC_ACR_ISEL_Msk                      (_U_(0x1) << ACC_ACR_ISEL_Pos)                       /**< (ACC_ACR) Current Selection Mask */
#define ACC_ACR_ISEL(value)                   (ACC_ACR_ISEL_Msk & ((value) << ACC_ACR_ISEL_Pos))  
#define   ACC_ACR_ISEL_LOPW_Val               _U_(0x0)                                             /**< (ACC_ACR) Low-power option.  */
#define   ACC_ACR_ISEL_HISP_Val               _U_(0x1)                                             /**< (ACC_ACR) High-speed option.  */
#define ACC_ACR_ISEL_LOPW                     (ACC_ACR_ISEL_LOPW_Val << ACC_ACR_ISEL_Pos)          /**< (ACC_ACR) Low-power option. Position  */
#define ACC_ACR_ISEL_HISP                     (ACC_ACR_ISEL_HISP_Val << ACC_ACR_ISEL_Pos)          /**< (ACC_ACR) High-speed option. Position  */
#define ACC_ACR_HYST_Pos                      _U_(1)                                               /**< (ACC_ACR) Hysteresis Selection Position */
#define ACC_ACR_HYST_Msk                      (_U_(0x3) << ACC_ACR_HYST_Pos)                       /**< (ACC_ACR) Hysteresis Selection Mask */
#define ACC_ACR_HYST(value)                   (ACC_ACR_HYST_Msk & ((value) << ACC_ACR_HYST_Pos))  
#define ACC_ACR_Msk                           _U_(0x00000007)                                      /**< (ACC_ACR) Register Mask  */


/* -------- ACC_WPMR : (ACC Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define ACC_WPMR_WPEN_Pos                     _U_(0)                                               /**< (ACC_WPMR) Write Protection Enable Position */
#define ACC_WPMR_WPEN_Msk                     (_U_(0x1) << ACC_WPMR_WPEN_Pos)                      /**< (ACC_WPMR) Write Protection Enable Mask */
#define ACC_WPMR_WPEN(value)                  (ACC_WPMR_WPEN_Msk & ((value) << ACC_WPMR_WPEN_Pos))
#define ACC_WPMR_WPKEY_Pos                    _U_(8)                                               /**< (ACC_WPMR) Write Protection Key Position */
#define ACC_WPMR_WPKEY_Msk                    (_U_(0xFFFFFF) << ACC_WPMR_WPKEY_Pos)                /**< (ACC_WPMR) Write Protection Key Mask */
#define ACC_WPMR_WPKEY(value)                 (ACC_WPMR_WPKEY_Msk & ((value) << ACC_WPMR_WPKEY_Pos))
#define   ACC_WPMR_WPKEY_PASSWD_Val           _U_(0x414343)                                        /**< (ACC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0.  */
#define ACC_WPMR_WPKEY_PASSWD                 (ACC_WPMR_WPKEY_PASSWD_Val << ACC_WPMR_WPKEY_Pos)    /**< (ACC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit.Always reads as 0. Position  */
#define ACC_WPMR_Msk                          _U_(0xFFFFFF01)                                      /**< (ACC_WPMR) Register Mask  */


/* -------- ACC_WPSR : (ACC Offset: 0xE8) ( R/ 32) Write Protection Status Register -------- */
#define ACC_WPSR_WPVS_Pos                     _U_(0)                                               /**< (ACC_WPSR) Write Protection Violation Status Position */
#define ACC_WPSR_WPVS_Msk                     (_U_(0x1) << ACC_WPSR_WPVS_Pos)                      /**< (ACC_WPSR) Write Protection Violation Status Mask */
#define ACC_WPSR_WPVS(value)                  (ACC_WPSR_WPVS_Msk & ((value) << ACC_WPSR_WPVS_Pos))
#define ACC_WPSR_Msk                          _U_(0x00000001)                                      /**< (ACC_WPSR) Register Mask  */


/** \brief ACC register offsets definitions */
#define ACC_CR_REG_OFST                (0x00)              /**< (ACC_CR) Control Register Offset */
#define ACC_MR_REG_OFST                (0x04)              /**< (ACC_MR) Mode Register Offset */
#define ACC_IER_REG_OFST               (0x24)              /**< (ACC_IER) Interrupt Enable Register Offset */
#define ACC_IDR_REG_OFST               (0x28)              /**< (ACC_IDR) Interrupt Disable Register Offset */
#define ACC_IMR_REG_OFST               (0x2C)              /**< (ACC_IMR) Interrupt Mask Register Offset */
#define ACC_ISR_REG_OFST               (0x30)              /**< (ACC_ISR) Interrupt Status Register Offset */
#define ACC_ACR_REG_OFST               (0x94)              /**< (ACC_ACR) Analog Control Register Offset */
#define ACC_WPMR_REG_OFST              (0xE4)              /**< (ACC_WPMR) Write Protection Mode Register Offset */
#define ACC_WPSR_REG_OFST              (0xE8)              /**< (ACC_WPSR) Write Protection Status Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief ACC register API structure */
typedef struct
{
  __O   uint32_t                       ACC_CR;             /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       ACC_MR;             /**< Offset: 0x04 (R/W  32) Mode Register */
  __I   uint8_t                        Reserved1[0x1C];
  __O   uint32_t                       ACC_IER;            /**< Offset: 0x24 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       ACC_IDR;            /**< Offset: 0x28 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       ACC_IMR;            /**< Offset: 0x2C (R/   32) Interrupt Mask Register */
  __I   uint32_t                       ACC_ISR;            /**< Offset: 0x30 (R/   32) Interrupt Status Register */
  __I   uint8_t                        Reserved2[0x60];
  __IO  uint32_t                       ACC_ACR;            /**< Offset: 0x94 (R/W  32) Analog Control Register */
  __I   uint8_t                        Reserved3[0x4C];
  __IO  uint32_t                       ACC_WPMR;           /**< Offset: 0xE4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       ACC_WPSR;           /**< Offset: 0xE8 (R/   32) Write Protection Status Register */
} acc_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_ACC_COMPONENT_H_ */
