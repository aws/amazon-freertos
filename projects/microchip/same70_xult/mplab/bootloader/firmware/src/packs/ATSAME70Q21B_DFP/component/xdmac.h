/**
 * \brief Component description for XDMAC
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
#ifndef _SAME70_XDMAC_COMPONENT_H_
#define _SAME70_XDMAC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR XDMAC                                        */
/* ************************************************************************** */

/* -------- XDMAC_CIE : (XDMAC Offset: 0x00) ( /W 32) Channel Interrupt Enable Register -------- */
#define XDMAC_CIE_BIE_Pos                     _U_(0)                                               /**< (XDMAC_CIE) End of Block Interrupt Enable Bit Position */
#define XDMAC_CIE_BIE_Msk                     (_U_(0x1) << XDMAC_CIE_BIE_Pos)                      /**< (XDMAC_CIE) End of Block Interrupt Enable Bit Mask */
#define XDMAC_CIE_BIE(value)                  (XDMAC_CIE_BIE_Msk & ((value) << XDMAC_CIE_BIE_Pos))
#define XDMAC_CIE_LIE_Pos                     _U_(1)                                               /**< (XDMAC_CIE) End of Linked List Interrupt Enable Bit Position */
#define XDMAC_CIE_LIE_Msk                     (_U_(0x1) << XDMAC_CIE_LIE_Pos)                      /**< (XDMAC_CIE) End of Linked List Interrupt Enable Bit Mask */
#define XDMAC_CIE_LIE(value)                  (XDMAC_CIE_LIE_Msk & ((value) << XDMAC_CIE_LIE_Pos))
#define XDMAC_CIE_DIE_Pos                     _U_(2)                                               /**< (XDMAC_CIE) End of Disable Interrupt Enable Bit Position */
#define XDMAC_CIE_DIE_Msk                     (_U_(0x1) << XDMAC_CIE_DIE_Pos)                      /**< (XDMAC_CIE) End of Disable Interrupt Enable Bit Mask */
#define XDMAC_CIE_DIE(value)                  (XDMAC_CIE_DIE_Msk & ((value) << XDMAC_CIE_DIE_Pos))
#define XDMAC_CIE_FIE_Pos                     _U_(3)                                               /**< (XDMAC_CIE) End of Flush Interrupt Enable Bit Position */
#define XDMAC_CIE_FIE_Msk                     (_U_(0x1) << XDMAC_CIE_FIE_Pos)                      /**< (XDMAC_CIE) End of Flush Interrupt Enable Bit Mask */
#define XDMAC_CIE_FIE(value)                  (XDMAC_CIE_FIE_Msk & ((value) << XDMAC_CIE_FIE_Pos))
#define XDMAC_CIE_RBIE_Pos                    _U_(4)                                               /**< (XDMAC_CIE) Read Bus Error Interrupt Enable Bit Position */
#define XDMAC_CIE_RBIE_Msk                    (_U_(0x1) << XDMAC_CIE_RBIE_Pos)                     /**< (XDMAC_CIE) Read Bus Error Interrupt Enable Bit Mask */
#define XDMAC_CIE_RBIE(value)                 (XDMAC_CIE_RBIE_Msk & ((value) << XDMAC_CIE_RBIE_Pos))
#define XDMAC_CIE_WBIE_Pos                    _U_(5)                                               /**< (XDMAC_CIE) Write Bus Error Interrupt Enable Bit Position */
#define XDMAC_CIE_WBIE_Msk                    (_U_(0x1) << XDMAC_CIE_WBIE_Pos)                     /**< (XDMAC_CIE) Write Bus Error Interrupt Enable Bit Mask */
#define XDMAC_CIE_WBIE(value)                 (XDMAC_CIE_WBIE_Msk & ((value) << XDMAC_CIE_WBIE_Pos))
#define XDMAC_CIE_ROIE_Pos                    _U_(6)                                               /**< (XDMAC_CIE) Request Overflow Error Interrupt Enable Bit Position */
#define XDMAC_CIE_ROIE_Msk                    (_U_(0x1) << XDMAC_CIE_ROIE_Pos)                     /**< (XDMAC_CIE) Request Overflow Error Interrupt Enable Bit Mask */
#define XDMAC_CIE_ROIE(value)                 (XDMAC_CIE_ROIE_Msk & ((value) << XDMAC_CIE_ROIE_Pos))
#define XDMAC_CIE_Msk                         _U_(0x0000007F)                                      /**< (XDMAC_CIE) Register Mask  */


/* -------- XDMAC_CID : (XDMAC Offset: 0x04) ( /W 32) Channel Interrupt Disable Register -------- */
#define XDMAC_CID_BID_Pos                     _U_(0)                                               /**< (XDMAC_CID) End of Block Interrupt Disable Bit Position */
#define XDMAC_CID_BID_Msk                     (_U_(0x1) << XDMAC_CID_BID_Pos)                      /**< (XDMAC_CID) End of Block Interrupt Disable Bit Mask */
#define XDMAC_CID_BID(value)                  (XDMAC_CID_BID_Msk & ((value) << XDMAC_CID_BID_Pos))
#define XDMAC_CID_LID_Pos                     _U_(1)                                               /**< (XDMAC_CID) End of Linked List Interrupt Disable Bit Position */
#define XDMAC_CID_LID_Msk                     (_U_(0x1) << XDMAC_CID_LID_Pos)                      /**< (XDMAC_CID) End of Linked List Interrupt Disable Bit Mask */
#define XDMAC_CID_LID(value)                  (XDMAC_CID_LID_Msk & ((value) << XDMAC_CID_LID_Pos))
#define XDMAC_CID_DID_Pos                     _U_(2)                                               /**< (XDMAC_CID) End of Disable Interrupt Disable Bit Position */
#define XDMAC_CID_DID_Msk                     (_U_(0x1) << XDMAC_CID_DID_Pos)                      /**< (XDMAC_CID) End of Disable Interrupt Disable Bit Mask */
#define XDMAC_CID_DID(value)                  (XDMAC_CID_DID_Msk & ((value) << XDMAC_CID_DID_Pos))
#define XDMAC_CID_FID_Pos                     _U_(3)                                               /**< (XDMAC_CID) End of Flush Interrupt Disable Bit Position */
#define XDMAC_CID_FID_Msk                     (_U_(0x1) << XDMAC_CID_FID_Pos)                      /**< (XDMAC_CID) End of Flush Interrupt Disable Bit Mask */
#define XDMAC_CID_FID(value)                  (XDMAC_CID_FID_Msk & ((value) << XDMAC_CID_FID_Pos))
#define XDMAC_CID_RBEID_Pos                   _U_(4)                                               /**< (XDMAC_CID) Read Bus Error Interrupt Disable Bit Position */
#define XDMAC_CID_RBEID_Msk                   (_U_(0x1) << XDMAC_CID_RBEID_Pos)                    /**< (XDMAC_CID) Read Bus Error Interrupt Disable Bit Mask */
#define XDMAC_CID_RBEID(value)                (XDMAC_CID_RBEID_Msk & ((value) << XDMAC_CID_RBEID_Pos))
#define XDMAC_CID_WBEID_Pos                   _U_(5)                                               /**< (XDMAC_CID) Write Bus Error Interrupt Disable Bit Position */
#define XDMAC_CID_WBEID_Msk                   (_U_(0x1) << XDMAC_CID_WBEID_Pos)                    /**< (XDMAC_CID) Write Bus Error Interrupt Disable Bit Mask */
#define XDMAC_CID_WBEID(value)                (XDMAC_CID_WBEID_Msk & ((value) << XDMAC_CID_WBEID_Pos))
#define XDMAC_CID_ROID_Pos                    _U_(6)                                               /**< (XDMAC_CID) Request Overflow Error Interrupt Disable Bit Position */
#define XDMAC_CID_ROID_Msk                    (_U_(0x1) << XDMAC_CID_ROID_Pos)                     /**< (XDMAC_CID) Request Overflow Error Interrupt Disable Bit Mask */
#define XDMAC_CID_ROID(value)                 (XDMAC_CID_ROID_Msk & ((value) << XDMAC_CID_ROID_Pos))
#define XDMAC_CID_Msk                         _U_(0x0000007F)                                      /**< (XDMAC_CID) Register Mask  */


/* -------- XDMAC_CIM : (XDMAC Offset: 0x08) ( R/ 32) Channel Interrupt Mask Register -------- */
#define XDMAC_CIM_BIM_Pos                     _U_(0)                                               /**< (XDMAC_CIM) End of Block Interrupt Mask Bit Position */
#define XDMAC_CIM_BIM_Msk                     (_U_(0x1) << XDMAC_CIM_BIM_Pos)                      /**< (XDMAC_CIM) End of Block Interrupt Mask Bit Mask */
#define XDMAC_CIM_BIM(value)                  (XDMAC_CIM_BIM_Msk & ((value) << XDMAC_CIM_BIM_Pos))
#define XDMAC_CIM_LIM_Pos                     _U_(1)                                               /**< (XDMAC_CIM) End of Linked List Interrupt Mask Bit Position */
#define XDMAC_CIM_LIM_Msk                     (_U_(0x1) << XDMAC_CIM_LIM_Pos)                      /**< (XDMAC_CIM) End of Linked List Interrupt Mask Bit Mask */
#define XDMAC_CIM_LIM(value)                  (XDMAC_CIM_LIM_Msk & ((value) << XDMAC_CIM_LIM_Pos))
#define XDMAC_CIM_DIM_Pos                     _U_(2)                                               /**< (XDMAC_CIM) End of Disable Interrupt Mask Bit Position */
#define XDMAC_CIM_DIM_Msk                     (_U_(0x1) << XDMAC_CIM_DIM_Pos)                      /**< (XDMAC_CIM) End of Disable Interrupt Mask Bit Mask */
#define XDMAC_CIM_DIM(value)                  (XDMAC_CIM_DIM_Msk & ((value) << XDMAC_CIM_DIM_Pos))
#define XDMAC_CIM_FIM_Pos                     _U_(3)                                               /**< (XDMAC_CIM) End of Flush Interrupt Mask Bit Position */
#define XDMAC_CIM_FIM_Msk                     (_U_(0x1) << XDMAC_CIM_FIM_Pos)                      /**< (XDMAC_CIM) End of Flush Interrupt Mask Bit Mask */
#define XDMAC_CIM_FIM(value)                  (XDMAC_CIM_FIM_Msk & ((value) << XDMAC_CIM_FIM_Pos))
#define XDMAC_CIM_RBEIM_Pos                   _U_(4)                                               /**< (XDMAC_CIM) Read Bus Error Interrupt Mask Bit Position */
#define XDMAC_CIM_RBEIM_Msk                   (_U_(0x1) << XDMAC_CIM_RBEIM_Pos)                    /**< (XDMAC_CIM) Read Bus Error Interrupt Mask Bit Mask */
#define XDMAC_CIM_RBEIM(value)                (XDMAC_CIM_RBEIM_Msk & ((value) << XDMAC_CIM_RBEIM_Pos))
#define XDMAC_CIM_WBEIM_Pos                   _U_(5)                                               /**< (XDMAC_CIM) Write Bus Error Interrupt Mask Bit Position */
#define XDMAC_CIM_WBEIM_Msk                   (_U_(0x1) << XDMAC_CIM_WBEIM_Pos)                    /**< (XDMAC_CIM) Write Bus Error Interrupt Mask Bit Mask */
#define XDMAC_CIM_WBEIM(value)                (XDMAC_CIM_WBEIM_Msk & ((value) << XDMAC_CIM_WBEIM_Pos))
#define XDMAC_CIM_ROIM_Pos                    _U_(6)                                               /**< (XDMAC_CIM) Request Overflow Error Interrupt Mask Bit Position */
#define XDMAC_CIM_ROIM_Msk                    (_U_(0x1) << XDMAC_CIM_ROIM_Pos)                     /**< (XDMAC_CIM) Request Overflow Error Interrupt Mask Bit Mask */
#define XDMAC_CIM_ROIM(value)                 (XDMAC_CIM_ROIM_Msk & ((value) << XDMAC_CIM_ROIM_Pos))
#define XDMAC_CIM_Msk                         _U_(0x0000007F)                                      /**< (XDMAC_CIM) Register Mask  */


/* -------- XDMAC_CIS : (XDMAC Offset: 0x0C) ( R/ 32) Channel Interrupt Status Register -------- */
#define XDMAC_CIS_BIS_Pos                     _U_(0)                                               /**< (XDMAC_CIS) End of Block Interrupt Status Bit Position */
#define XDMAC_CIS_BIS_Msk                     (_U_(0x1) << XDMAC_CIS_BIS_Pos)                      /**< (XDMAC_CIS) End of Block Interrupt Status Bit Mask */
#define XDMAC_CIS_BIS(value)                  (XDMAC_CIS_BIS_Msk & ((value) << XDMAC_CIS_BIS_Pos))
#define XDMAC_CIS_LIS_Pos                     _U_(1)                                               /**< (XDMAC_CIS) End of Linked List Interrupt Status Bit Position */
#define XDMAC_CIS_LIS_Msk                     (_U_(0x1) << XDMAC_CIS_LIS_Pos)                      /**< (XDMAC_CIS) End of Linked List Interrupt Status Bit Mask */
#define XDMAC_CIS_LIS(value)                  (XDMAC_CIS_LIS_Msk & ((value) << XDMAC_CIS_LIS_Pos))
#define XDMAC_CIS_DIS_Pos                     _U_(2)                                               /**< (XDMAC_CIS) End of Disable Interrupt Status Bit Position */
#define XDMAC_CIS_DIS_Msk                     (_U_(0x1) << XDMAC_CIS_DIS_Pos)                      /**< (XDMAC_CIS) End of Disable Interrupt Status Bit Mask */
#define XDMAC_CIS_DIS(value)                  (XDMAC_CIS_DIS_Msk & ((value) << XDMAC_CIS_DIS_Pos))
#define XDMAC_CIS_FIS_Pos                     _U_(3)                                               /**< (XDMAC_CIS) End of Flush Interrupt Status Bit Position */
#define XDMAC_CIS_FIS_Msk                     (_U_(0x1) << XDMAC_CIS_FIS_Pos)                      /**< (XDMAC_CIS) End of Flush Interrupt Status Bit Mask */
#define XDMAC_CIS_FIS(value)                  (XDMAC_CIS_FIS_Msk & ((value) << XDMAC_CIS_FIS_Pos))
#define XDMAC_CIS_RBEIS_Pos                   _U_(4)                                               /**< (XDMAC_CIS) Read Bus Error Interrupt Status Bit Position */
#define XDMAC_CIS_RBEIS_Msk                   (_U_(0x1) << XDMAC_CIS_RBEIS_Pos)                    /**< (XDMAC_CIS) Read Bus Error Interrupt Status Bit Mask */
#define XDMAC_CIS_RBEIS(value)                (XDMAC_CIS_RBEIS_Msk & ((value) << XDMAC_CIS_RBEIS_Pos))
#define XDMAC_CIS_WBEIS_Pos                   _U_(5)                                               /**< (XDMAC_CIS) Write Bus Error Interrupt Status Bit Position */
#define XDMAC_CIS_WBEIS_Msk                   (_U_(0x1) << XDMAC_CIS_WBEIS_Pos)                    /**< (XDMAC_CIS) Write Bus Error Interrupt Status Bit Mask */
#define XDMAC_CIS_WBEIS(value)                (XDMAC_CIS_WBEIS_Msk & ((value) << XDMAC_CIS_WBEIS_Pos))
#define XDMAC_CIS_ROIS_Pos                    _U_(6)                                               /**< (XDMAC_CIS) Request Overflow Error Interrupt Status Bit Position */
#define XDMAC_CIS_ROIS_Msk                    (_U_(0x1) << XDMAC_CIS_ROIS_Pos)                     /**< (XDMAC_CIS) Request Overflow Error Interrupt Status Bit Mask */
#define XDMAC_CIS_ROIS(value)                 (XDMAC_CIS_ROIS_Msk & ((value) << XDMAC_CIS_ROIS_Pos))
#define XDMAC_CIS_Msk                         _U_(0x0000007F)                                      /**< (XDMAC_CIS) Register Mask  */


/* -------- XDMAC_CSA : (XDMAC Offset: 0x10) (R/W 32) Channel Source Address Register -------- */
#define XDMAC_CSA_SA_Pos                      _U_(0)                                               /**< (XDMAC_CSA) Channel x Source Address Position */
#define XDMAC_CSA_SA_Msk                      (_U_(0xFFFFFFFF) << XDMAC_CSA_SA_Pos)                /**< (XDMAC_CSA) Channel x Source Address Mask */
#define XDMAC_CSA_SA(value)                   (XDMAC_CSA_SA_Msk & ((value) << XDMAC_CSA_SA_Pos))  
#define XDMAC_CSA_Msk                         _U_(0xFFFFFFFF)                                      /**< (XDMAC_CSA) Register Mask  */


/* -------- XDMAC_CDA : (XDMAC Offset: 0x14) (R/W 32) Channel Destination Address Register -------- */
#define XDMAC_CDA_DA_Pos                      _U_(0)                                               /**< (XDMAC_CDA) Channel x Destination Address Position */
#define XDMAC_CDA_DA_Msk                      (_U_(0xFFFFFFFF) << XDMAC_CDA_DA_Pos)                /**< (XDMAC_CDA) Channel x Destination Address Mask */
#define XDMAC_CDA_DA(value)                   (XDMAC_CDA_DA_Msk & ((value) << XDMAC_CDA_DA_Pos))  
#define XDMAC_CDA_Msk                         _U_(0xFFFFFFFF)                                      /**< (XDMAC_CDA) Register Mask  */


/* -------- XDMAC_CNDA : (XDMAC Offset: 0x18) (R/W 32) Channel Next Descriptor Address Register -------- */
#define XDMAC_CNDA_NDAIF_Pos                  _U_(0)                                               /**< (XDMAC_CNDA) Channel x Next Descriptor Interface Position */
#define XDMAC_CNDA_NDAIF_Msk                  (_U_(0x1) << XDMAC_CNDA_NDAIF_Pos)                   /**< (XDMAC_CNDA) Channel x Next Descriptor Interface Mask */
#define XDMAC_CNDA_NDAIF(value)               (XDMAC_CNDA_NDAIF_Msk & ((value) << XDMAC_CNDA_NDAIF_Pos))
#define XDMAC_CNDA_NDA_Pos                    _U_(2)                                               /**< (XDMAC_CNDA) Channel x Next Descriptor Address Position */
#define XDMAC_CNDA_NDA_Msk                    (_U_(0x3FFFFFFF) << XDMAC_CNDA_NDA_Pos)              /**< (XDMAC_CNDA) Channel x Next Descriptor Address Mask */
#define XDMAC_CNDA_NDA(value)                 (XDMAC_CNDA_NDA_Msk & ((value) << XDMAC_CNDA_NDA_Pos))
#define XDMAC_CNDA_Msk                        _U_(0xFFFFFFFD)                                      /**< (XDMAC_CNDA) Register Mask  */


/* -------- XDMAC_CNDC : (XDMAC Offset: 0x1C) (R/W 32) Channel Next Descriptor Control Register -------- */
#define XDMAC_CNDC_NDE_Pos                    _U_(0)                                               /**< (XDMAC_CNDC) Channel x Next Descriptor Enable Position */
#define XDMAC_CNDC_NDE_Msk                    (_U_(0x1) << XDMAC_CNDC_NDE_Pos)                     /**< (XDMAC_CNDC) Channel x Next Descriptor Enable Mask */
#define XDMAC_CNDC_NDE(value)                 (XDMAC_CNDC_NDE_Msk & ((value) << XDMAC_CNDC_NDE_Pos))
#define   XDMAC_CNDC_NDE_DSCR_FETCH_DIS_Val   _U_(0x0)                                             /**< (XDMAC_CNDC) Descriptor fetch is disabled.  */
#define   XDMAC_CNDC_NDE_DSCR_FETCH_EN_Val    _U_(0x1)                                             /**< (XDMAC_CNDC) Descriptor fetch is enabled.  */
#define XDMAC_CNDC_NDE_DSCR_FETCH_DIS         (XDMAC_CNDC_NDE_DSCR_FETCH_DIS_Val << XDMAC_CNDC_NDE_Pos) /**< (XDMAC_CNDC) Descriptor fetch is disabled. Position  */
#define XDMAC_CNDC_NDE_DSCR_FETCH_EN          (XDMAC_CNDC_NDE_DSCR_FETCH_EN_Val << XDMAC_CNDC_NDE_Pos) /**< (XDMAC_CNDC) Descriptor fetch is enabled. Position  */
#define XDMAC_CNDC_NDSUP_Pos                  _U_(1)                                               /**< (XDMAC_CNDC) Channel x Next Descriptor Source Update Position */
#define XDMAC_CNDC_NDSUP_Msk                  (_U_(0x1) << XDMAC_CNDC_NDSUP_Pos)                   /**< (XDMAC_CNDC) Channel x Next Descriptor Source Update Mask */
#define XDMAC_CNDC_NDSUP(value)               (XDMAC_CNDC_NDSUP_Msk & ((value) << XDMAC_CNDC_NDSUP_Pos))
#define   XDMAC_CNDC_NDSUP_SRC_PARAMS_UNCHANGED_Val _U_(0x0)                                             /**< (XDMAC_CNDC) Source parameters remain unchanged.  */
#define   XDMAC_CNDC_NDSUP_SRC_PARAMS_UPDATED_Val _U_(0x1)                                             /**< (XDMAC_CNDC) Source parameters are updated when the descriptor is retrieved.  */
#define XDMAC_CNDC_NDSUP_SRC_PARAMS_UNCHANGED (XDMAC_CNDC_NDSUP_SRC_PARAMS_UNCHANGED_Val << XDMAC_CNDC_NDSUP_Pos) /**< (XDMAC_CNDC) Source parameters remain unchanged. Position  */
#define XDMAC_CNDC_NDSUP_SRC_PARAMS_UPDATED   (XDMAC_CNDC_NDSUP_SRC_PARAMS_UPDATED_Val << XDMAC_CNDC_NDSUP_Pos) /**< (XDMAC_CNDC) Source parameters are updated when the descriptor is retrieved. Position  */
#define XDMAC_CNDC_NDDUP_Pos                  _U_(2)                                               /**< (XDMAC_CNDC) Channel x Next Descriptor Destination Update Position */
#define XDMAC_CNDC_NDDUP_Msk                  (_U_(0x1) << XDMAC_CNDC_NDDUP_Pos)                   /**< (XDMAC_CNDC) Channel x Next Descriptor Destination Update Mask */
#define XDMAC_CNDC_NDDUP(value)               (XDMAC_CNDC_NDDUP_Msk & ((value) << XDMAC_CNDC_NDDUP_Pos))
#define   XDMAC_CNDC_NDDUP_DST_PARAMS_UNCHANGED_Val _U_(0x0)                                             /**< (XDMAC_CNDC) Destination parameters remain unchanged.  */
#define   XDMAC_CNDC_NDDUP_DST_PARAMS_UPDATED_Val _U_(0x1)                                             /**< (XDMAC_CNDC) Destination parameters are updated when the descriptor is retrieved.  */
#define XDMAC_CNDC_NDDUP_DST_PARAMS_UNCHANGED (XDMAC_CNDC_NDDUP_DST_PARAMS_UNCHANGED_Val << XDMAC_CNDC_NDDUP_Pos) /**< (XDMAC_CNDC) Destination parameters remain unchanged. Position  */
#define XDMAC_CNDC_NDDUP_DST_PARAMS_UPDATED   (XDMAC_CNDC_NDDUP_DST_PARAMS_UPDATED_Val << XDMAC_CNDC_NDDUP_Pos) /**< (XDMAC_CNDC) Destination parameters are updated when the descriptor is retrieved. Position  */
#define XDMAC_CNDC_NDVIEW_Pos                 _U_(3)                                               /**< (XDMAC_CNDC) Channel x Next Descriptor View Position */
#define XDMAC_CNDC_NDVIEW_Msk                 (_U_(0x3) << XDMAC_CNDC_NDVIEW_Pos)                  /**< (XDMAC_CNDC) Channel x Next Descriptor View Mask */
#define XDMAC_CNDC_NDVIEW(value)              (XDMAC_CNDC_NDVIEW_Msk & ((value) << XDMAC_CNDC_NDVIEW_Pos))
#define   XDMAC_CNDC_NDVIEW_NDV0_Val          _U_(0x0)                                             /**< (XDMAC_CNDC) Next Descriptor View 0  */
#define   XDMAC_CNDC_NDVIEW_NDV1_Val          _U_(0x1)                                             /**< (XDMAC_CNDC) Next Descriptor View 1  */
#define   XDMAC_CNDC_NDVIEW_NDV2_Val          _U_(0x2)                                             /**< (XDMAC_CNDC) Next Descriptor View 2  */
#define   XDMAC_CNDC_NDVIEW_NDV3_Val          _U_(0x3)                                             /**< (XDMAC_CNDC) Next Descriptor View 3  */
#define XDMAC_CNDC_NDVIEW_NDV0                (XDMAC_CNDC_NDVIEW_NDV0_Val << XDMAC_CNDC_NDVIEW_Pos) /**< (XDMAC_CNDC) Next Descriptor View 0 Position  */
#define XDMAC_CNDC_NDVIEW_NDV1                (XDMAC_CNDC_NDVIEW_NDV1_Val << XDMAC_CNDC_NDVIEW_Pos) /**< (XDMAC_CNDC) Next Descriptor View 1 Position  */
#define XDMAC_CNDC_NDVIEW_NDV2                (XDMAC_CNDC_NDVIEW_NDV2_Val << XDMAC_CNDC_NDVIEW_Pos) /**< (XDMAC_CNDC) Next Descriptor View 2 Position  */
#define XDMAC_CNDC_NDVIEW_NDV3                (XDMAC_CNDC_NDVIEW_NDV3_Val << XDMAC_CNDC_NDVIEW_Pos) /**< (XDMAC_CNDC) Next Descriptor View 3 Position  */
#define XDMAC_CNDC_Msk                        _U_(0x0000001F)                                      /**< (XDMAC_CNDC) Register Mask  */


/* -------- XDMAC_CUBC : (XDMAC Offset: 0x20) (R/W 32) Channel Microblock Control Register -------- */
#define XDMAC_CUBC_UBLEN_Pos                  _U_(0)                                               /**< (XDMAC_CUBC) Channel x Microblock Length Position */
#define XDMAC_CUBC_UBLEN_Msk                  (_U_(0xFFFFFF) << XDMAC_CUBC_UBLEN_Pos)              /**< (XDMAC_CUBC) Channel x Microblock Length Mask */
#define XDMAC_CUBC_UBLEN(value)               (XDMAC_CUBC_UBLEN_Msk & ((value) << XDMAC_CUBC_UBLEN_Pos))
#define XDMAC_CUBC_Msk                        _U_(0x00FFFFFF)                                      /**< (XDMAC_CUBC) Register Mask  */


/* -------- XDMAC_CBC : (XDMAC Offset: 0x24) (R/W 32) Channel Block Control Register -------- */
#define XDMAC_CBC_BLEN_Pos                    _U_(0)                                               /**< (XDMAC_CBC) Channel x Block Length Position */
#define XDMAC_CBC_BLEN_Msk                    (_U_(0xFFF) << XDMAC_CBC_BLEN_Pos)                   /**< (XDMAC_CBC) Channel x Block Length Mask */
#define XDMAC_CBC_BLEN(value)                 (XDMAC_CBC_BLEN_Msk & ((value) << XDMAC_CBC_BLEN_Pos))
#define XDMAC_CBC_Msk                         _U_(0x00000FFF)                                      /**< (XDMAC_CBC) Register Mask  */


/* -------- XDMAC_CC : (XDMAC Offset: 0x28) (R/W 32) Channel Configuration Register -------- */
#define XDMAC_CC_TYPE_Pos                     _U_(0)                                               /**< (XDMAC_CC) Channel x Transfer Type Position */
#define XDMAC_CC_TYPE_Msk                     (_U_(0x1) << XDMAC_CC_TYPE_Pos)                      /**< (XDMAC_CC) Channel x Transfer Type Mask */
#define XDMAC_CC_TYPE(value)                  (XDMAC_CC_TYPE_Msk & ((value) << XDMAC_CC_TYPE_Pos))
#define   XDMAC_CC_TYPE_MEM_TRAN_Val          _U_(0x0)                                             /**< (XDMAC_CC) Self-triggered mode (memory-to-memory transfer).  */
#define   XDMAC_CC_TYPE_PER_TRAN_Val          _U_(0x1)                                             /**< (XDMAC_CC) Synchronized mode (peripheral-to-memory or memory-to-peripheral transfer).  */
#define XDMAC_CC_TYPE_MEM_TRAN                (XDMAC_CC_TYPE_MEM_TRAN_Val << XDMAC_CC_TYPE_Pos)    /**< (XDMAC_CC) Self-triggered mode (memory-to-memory transfer). Position  */
#define XDMAC_CC_TYPE_PER_TRAN                (XDMAC_CC_TYPE_PER_TRAN_Val << XDMAC_CC_TYPE_Pos)    /**< (XDMAC_CC) Synchronized mode (peripheral-to-memory or memory-to-peripheral transfer). Position  */
#define XDMAC_CC_MBSIZE_Pos                   _U_(1)                                               /**< (XDMAC_CC) Channel x Memory Burst Size Position */
#define XDMAC_CC_MBSIZE_Msk                   (_U_(0x3) << XDMAC_CC_MBSIZE_Pos)                    /**< (XDMAC_CC) Channel x Memory Burst Size Mask */
#define XDMAC_CC_MBSIZE(value)                (XDMAC_CC_MBSIZE_Msk & ((value) << XDMAC_CC_MBSIZE_Pos))
#define   XDMAC_CC_MBSIZE_SINGLE_Val          _U_(0x0)                                             /**< (XDMAC_CC) The memory burst size is set to one.  */
#define   XDMAC_CC_MBSIZE_FOUR_Val            _U_(0x1)                                             /**< (XDMAC_CC) The memory burst size is set to four.  */
#define   XDMAC_CC_MBSIZE_EIGHT_Val           _U_(0x2)                                             /**< (XDMAC_CC) The memory burst size is set to eight.  */
#define   XDMAC_CC_MBSIZE_SIXTEEN_Val         _U_(0x3)                                             /**< (XDMAC_CC) The memory burst size is set to sixteen.  */
#define XDMAC_CC_MBSIZE_SINGLE                (XDMAC_CC_MBSIZE_SINGLE_Val << XDMAC_CC_MBSIZE_Pos)  /**< (XDMAC_CC) The memory burst size is set to one. Position  */
#define XDMAC_CC_MBSIZE_FOUR                  (XDMAC_CC_MBSIZE_FOUR_Val << XDMAC_CC_MBSIZE_Pos)    /**< (XDMAC_CC) The memory burst size is set to four. Position  */
#define XDMAC_CC_MBSIZE_EIGHT                 (XDMAC_CC_MBSIZE_EIGHT_Val << XDMAC_CC_MBSIZE_Pos)   /**< (XDMAC_CC) The memory burst size is set to eight. Position  */
#define XDMAC_CC_MBSIZE_SIXTEEN               (XDMAC_CC_MBSIZE_SIXTEEN_Val << XDMAC_CC_MBSIZE_Pos) /**< (XDMAC_CC) The memory burst size is set to sixteen. Position  */
#define XDMAC_CC_DSYNC_Pos                    _U_(4)                                               /**< (XDMAC_CC) Channel x Synchronization Position */
#define XDMAC_CC_DSYNC_Msk                    (_U_(0x1) << XDMAC_CC_DSYNC_Pos)                     /**< (XDMAC_CC) Channel x Synchronization Mask */
#define XDMAC_CC_DSYNC(value)                 (XDMAC_CC_DSYNC_Msk & ((value) << XDMAC_CC_DSYNC_Pos))
#define   XDMAC_CC_DSYNC_PER2MEM_Val          _U_(0x0)                                             /**< (XDMAC_CC) Peripheral-to-memory transfer.  */
#define   XDMAC_CC_DSYNC_MEM2PER_Val          _U_(0x1)                                             /**< (XDMAC_CC) Memory-to-peripheral transfer.  */
#define XDMAC_CC_DSYNC_PER2MEM                (XDMAC_CC_DSYNC_PER2MEM_Val << XDMAC_CC_DSYNC_Pos)   /**< (XDMAC_CC) Peripheral-to-memory transfer. Position  */
#define XDMAC_CC_DSYNC_MEM2PER                (XDMAC_CC_DSYNC_MEM2PER_Val << XDMAC_CC_DSYNC_Pos)   /**< (XDMAC_CC) Memory-to-peripheral transfer. Position  */
#define XDMAC_CC_SWREQ_Pos                    _U_(6)                                               /**< (XDMAC_CC) Channel x Software Request Trigger Position */
#define XDMAC_CC_SWREQ_Msk                    (_U_(0x1) << XDMAC_CC_SWREQ_Pos)                     /**< (XDMAC_CC) Channel x Software Request Trigger Mask */
#define XDMAC_CC_SWREQ(value)                 (XDMAC_CC_SWREQ_Msk & ((value) << XDMAC_CC_SWREQ_Pos))
#define   XDMAC_CC_SWREQ_HWR_CONNECTED_Val    _U_(0x0)                                             /**< (XDMAC_CC) Hardware request line is connected to the peripheral request line.  */
#define   XDMAC_CC_SWREQ_SWR_CONNECTED_Val    _U_(0x1)                                             /**< (XDMAC_CC) Software request is connected to the peripheral request line.  */
#define XDMAC_CC_SWREQ_HWR_CONNECTED          (XDMAC_CC_SWREQ_HWR_CONNECTED_Val << XDMAC_CC_SWREQ_Pos) /**< (XDMAC_CC) Hardware request line is connected to the peripheral request line. Position  */
#define XDMAC_CC_SWREQ_SWR_CONNECTED          (XDMAC_CC_SWREQ_SWR_CONNECTED_Val << XDMAC_CC_SWREQ_Pos) /**< (XDMAC_CC) Software request is connected to the peripheral request line. Position  */
#define XDMAC_CC_MEMSET_Pos                   _U_(7)                                               /**< (XDMAC_CC) Channel x Fill Block of memory Position */
#define XDMAC_CC_MEMSET_Msk                   (_U_(0x1) << XDMAC_CC_MEMSET_Pos)                    /**< (XDMAC_CC) Channel x Fill Block of memory Mask */
#define XDMAC_CC_MEMSET(value)                (XDMAC_CC_MEMSET_Msk & ((value) << XDMAC_CC_MEMSET_Pos))
#define   XDMAC_CC_MEMSET_NORMAL_MODE_Val     _U_(0x0)                                             /**< (XDMAC_CC) Memset is not activated.  */
#define   XDMAC_CC_MEMSET_HW_MODE_Val         _U_(0x1)                                             /**< (XDMAC_CC) Sets the block of memory pointed by DA field to the specified value. This operation is performed on 8-, 16- or 32-bit basis.  */
#define XDMAC_CC_MEMSET_NORMAL_MODE           (XDMAC_CC_MEMSET_NORMAL_MODE_Val << XDMAC_CC_MEMSET_Pos) /**< (XDMAC_CC) Memset is not activated. Position  */
#define XDMAC_CC_MEMSET_HW_MODE               (XDMAC_CC_MEMSET_HW_MODE_Val << XDMAC_CC_MEMSET_Pos) /**< (XDMAC_CC) Sets the block of memory pointed by DA field to the specified value. This operation is performed on 8-, 16- or 32-bit basis. Position  */
#define XDMAC_CC_CSIZE_Pos                    _U_(8)                                               /**< (XDMAC_CC) Channel x Chunk Size Position */
#define XDMAC_CC_CSIZE_Msk                    (_U_(0x7) << XDMAC_CC_CSIZE_Pos)                     /**< (XDMAC_CC) Channel x Chunk Size Mask */
#define XDMAC_CC_CSIZE(value)                 (XDMAC_CC_CSIZE_Msk & ((value) << XDMAC_CC_CSIZE_Pos))
#define   XDMAC_CC_CSIZE_CHK_1_Val            _U_(0x0)                                             /**< (XDMAC_CC) 1 data transferred  */
#define   XDMAC_CC_CSIZE_CHK_2_Val            _U_(0x1)                                             /**< (XDMAC_CC) 2 data transferred  */
#define   XDMAC_CC_CSIZE_CHK_4_Val            _U_(0x2)                                             /**< (XDMAC_CC) 4 data transferred  */
#define   XDMAC_CC_CSIZE_CHK_8_Val            _U_(0x3)                                             /**< (XDMAC_CC) 8 data transferred  */
#define   XDMAC_CC_CSIZE_CHK_16_Val           _U_(0x4)                                             /**< (XDMAC_CC) 16 data transferred  */
#define XDMAC_CC_CSIZE_CHK_1                  (XDMAC_CC_CSIZE_CHK_1_Val << XDMAC_CC_CSIZE_Pos)     /**< (XDMAC_CC) 1 data transferred Position  */
#define XDMAC_CC_CSIZE_CHK_2                  (XDMAC_CC_CSIZE_CHK_2_Val << XDMAC_CC_CSIZE_Pos)     /**< (XDMAC_CC) 2 data transferred Position  */
#define XDMAC_CC_CSIZE_CHK_4                  (XDMAC_CC_CSIZE_CHK_4_Val << XDMAC_CC_CSIZE_Pos)     /**< (XDMAC_CC) 4 data transferred Position  */
#define XDMAC_CC_CSIZE_CHK_8                  (XDMAC_CC_CSIZE_CHK_8_Val << XDMAC_CC_CSIZE_Pos)     /**< (XDMAC_CC) 8 data transferred Position  */
#define XDMAC_CC_CSIZE_CHK_16                 (XDMAC_CC_CSIZE_CHK_16_Val << XDMAC_CC_CSIZE_Pos)    /**< (XDMAC_CC) 16 data transferred Position  */
#define XDMAC_CC_DWIDTH_Pos                   _U_(11)                                              /**< (XDMAC_CC) Channel x Data Width Position */
#define XDMAC_CC_DWIDTH_Msk                   (_U_(0x3) << XDMAC_CC_DWIDTH_Pos)                    /**< (XDMAC_CC) Channel x Data Width Mask */
#define XDMAC_CC_DWIDTH(value)                (XDMAC_CC_DWIDTH_Msk & ((value) << XDMAC_CC_DWIDTH_Pos))
#define   XDMAC_CC_DWIDTH_BYTE_Val            _U_(0x0)                                             /**< (XDMAC_CC) The data size is set to 8 bits  */
#define   XDMAC_CC_DWIDTH_HALFWORD_Val        _U_(0x1)                                             /**< (XDMAC_CC) The data size is set to 16 bits  */
#define   XDMAC_CC_DWIDTH_WORD_Val            _U_(0x2)                                             /**< (XDMAC_CC) The data size is set to 32 bits  */
#define XDMAC_CC_DWIDTH_BYTE                  (XDMAC_CC_DWIDTH_BYTE_Val << XDMAC_CC_DWIDTH_Pos)    /**< (XDMAC_CC) The data size is set to 8 bits Position  */
#define XDMAC_CC_DWIDTH_HALFWORD              (XDMAC_CC_DWIDTH_HALFWORD_Val << XDMAC_CC_DWIDTH_Pos) /**< (XDMAC_CC) The data size is set to 16 bits Position  */
#define XDMAC_CC_DWIDTH_WORD                  (XDMAC_CC_DWIDTH_WORD_Val << XDMAC_CC_DWIDTH_Pos)    /**< (XDMAC_CC) The data size is set to 32 bits Position  */
#define XDMAC_CC_SIF_Pos                      _U_(13)                                              /**< (XDMAC_CC) Channel x Source Interface Identifier Position */
#define XDMAC_CC_SIF_Msk                      (_U_(0x1) << XDMAC_CC_SIF_Pos)                       /**< (XDMAC_CC) Channel x Source Interface Identifier Mask */
#define XDMAC_CC_SIF(value)                   (XDMAC_CC_SIF_Msk & ((value) << XDMAC_CC_SIF_Pos))  
#define   XDMAC_CC_SIF_AHB_IF0_Val            _U_(0x0)                                             /**< (XDMAC_CC) The data is read through the system bus interface 0.  */
#define   XDMAC_CC_SIF_AHB_IF1_Val            _U_(0x1)                                             /**< (XDMAC_CC) The data is read through the system bus interface 1.  */
#define XDMAC_CC_SIF_AHB_IF0                  (XDMAC_CC_SIF_AHB_IF0_Val << XDMAC_CC_SIF_Pos)       /**< (XDMAC_CC) The data is read through the system bus interface 0. Position  */
#define XDMAC_CC_SIF_AHB_IF1                  (XDMAC_CC_SIF_AHB_IF1_Val << XDMAC_CC_SIF_Pos)       /**< (XDMAC_CC) The data is read through the system bus interface 1. Position  */
#define XDMAC_CC_DIF_Pos                      _U_(14)                                              /**< (XDMAC_CC) Channel x Destination Interface Identifier Position */
#define XDMAC_CC_DIF_Msk                      (_U_(0x1) << XDMAC_CC_DIF_Pos)                       /**< (XDMAC_CC) Channel x Destination Interface Identifier Mask */
#define XDMAC_CC_DIF(value)                   (XDMAC_CC_DIF_Msk & ((value) << XDMAC_CC_DIF_Pos))  
#define   XDMAC_CC_DIF_AHB_IF0_Val            _U_(0x0)                                             /**< (XDMAC_CC) The data is written through the system bus interface 0.  */
#define   XDMAC_CC_DIF_AHB_IF1_Val            _U_(0x1)                                             /**< (XDMAC_CC) The data is written though the system bus interface 1.  */
#define XDMAC_CC_DIF_AHB_IF0                  (XDMAC_CC_DIF_AHB_IF0_Val << XDMAC_CC_DIF_Pos)       /**< (XDMAC_CC) The data is written through the system bus interface 0. Position  */
#define XDMAC_CC_DIF_AHB_IF1                  (XDMAC_CC_DIF_AHB_IF1_Val << XDMAC_CC_DIF_Pos)       /**< (XDMAC_CC) The data is written though the system bus interface 1. Position  */
#define XDMAC_CC_SAM_Pos                      _U_(16)                                              /**< (XDMAC_CC) Channel x Source Addressing Mode Position */
#define XDMAC_CC_SAM_Msk                      (_U_(0x3) << XDMAC_CC_SAM_Pos)                       /**< (XDMAC_CC) Channel x Source Addressing Mode Mask */
#define XDMAC_CC_SAM(value)                   (XDMAC_CC_SAM_Msk & ((value) << XDMAC_CC_SAM_Pos))  
#define   XDMAC_CC_SAM_FIXED_AM_Val           _U_(0x0)                                             /**< (XDMAC_CC) The address remains unchanged.  */
#define   XDMAC_CC_SAM_INCREMENTED_AM_Val     _U_(0x1)                                             /**< (XDMAC_CC) The addressing mode is incremented (the increment size is set to the data size).  */
#define   XDMAC_CC_SAM_UBS_AM_Val             _U_(0x2)                                             /**< (XDMAC_CC) The microblock stride is added at the microblock boundary.  */
#define   XDMAC_CC_SAM_UBS_DS_AM_Val          _U_(0x3)                                             /**< (XDMAC_CC) The microblock stride is added at the microblock boundary, the data stride is added at the data boundary.  */
#define XDMAC_CC_SAM_FIXED_AM                 (XDMAC_CC_SAM_FIXED_AM_Val << XDMAC_CC_SAM_Pos)      /**< (XDMAC_CC) The address remains unchanged. Position  */
#define XDMAC_CC_SAM_INCREMENTED_AM           (XDMAC_CC_SAM_INCREMENTED_AM_Val << XDMAC_CC_SAM_Pos) /**< (XDMAC_CC) The addressing mode is incremented (the increment size is set to the data size). Position  */
#define XDMAC_CC_SAM_UBS_AM                   (XDMAC_CC_SAM_UBS_AM_Val << XDMAC_CC_SAM_Pos)        /**< (XDMAC_CC) The microblock stride is added at the microblock boundary. Position  */
#define XDMAC_CC_SAM_UBS_DS_AM                (XDMAC_CC_SAM_UBS_DS_AM_Val << XDMAC_CC_SAM_Pos)     /**< (XDMAC_CC) The microblock stride is added at the microblock boundary, the data stride is added at the data boundary. Position  */
#define XDMAC_CC_DAM_Pos                      _U_(18)                                              /**< (XDMAC_CC) Channel x Destination Addressing Mode Position */
#define XDMAC_CC_DAM_Msk                      (_U_(0x3) << XDMAC_CC_DAM_Pos)                       /**< (XDMAC_CC) Channel x Destination Addressing Mode Mask */
#define XDMAC_CC_DAM(value)                   (XDMAC_CC_DAM_Msk & ((value) << XDMAC_CC_DAM_Pos))  
#define   XDMAC_CC_DAM_FIXED_AM_Val           _U_(0x0)                                             /**< (XDMAC_CC) The address remains unchanged.  */
#define   XDMAC_CC_DAM_INCREMENTED_AM_Val     _U_(0x1)                                             /**< (XDMAC_CC) The addressing mode is incremented (the increment size is set to the data size).  */
#define   XDMAC_CC_DAM_UBS_AM_Val             _U_(0x2)                                             /**< (XDMAC_CC) The microblock stride is added at the microblock boundary.  */
#define   XDMAC_CC_DAM_UBS_DS_AM_Val          _U_(0x3)                                             /**< (XDMAC_CC) The microblock stride is added at the microblock boundary; the data stride is added at the data boundary.  */
#define XDMAC_CC_DAM_FIXED_AM                 (XDMAC_CC_DAM_FIXED_AM_Val << XDMAC_CC_DAM_Pos)      /**< (XDMAC_CC) The address remains unchanged. Position  */
#define XDMAC_CC_DAM_INCREMENTED_AM           (XDMAC_CC_DAM_INCREMENTED_AM_Val << XDMAC_CC_DAM_Pos) /**< (XDMAC_CC) The addressing mode is incremented (the increment size is set to the data size). Position  */
#define XDMAC_CC_DAM_UBS_AM                   (XDMAC_CC_DAM_UBS_AM_Val << XDMAC_CC_DAM_Pos)        /**< (XDMAC_CC) The microblock stride is added at the microblock boundary. Position  */
#define XDMAC_CC_DAM_UBS_DS_AM                (XDMAC_CC_DAM_UBS_DS_AM_Val << XDMAC_CC_DAM_Pos)     /**< (XDMAC_CC) The microblock stride is added at the microblock boundary; the data stride is added at the data boundary. Position  */
#define XDMAC_CC_INITD_Pos                    _U_(21)                                              /**< (XDMAC_CC) Channel Initialization Terminated (this bit is read-only) Position */
#define XDMAC_CC_INITD_Msk                    (_U_(0x1) << XDMAC_CC_INITD_Pos)                     /**< (XDMAC_CC) Channel Initialization Terminated (this bit is read-only) Mask */
#define XDMAC_CC_INITD(value)                 (XDMAC_CC_INITD_Msk & ((value) << XDMAC_CC_INITD_Pos))
#define   XDMAC_CC_INITD_IN_PROGRESS_Val      _U_(0x0)                                             /**< (XDMAC_CC) Channel initialization is in progress.  */
#define   XDMAC_CC_INITD_TERMINATED_Val       _U_(0x1)                                             /**< (XDMAC_CC) Channel initialization is completed.  */
#define XDMAC_CC_INITD_IN_PROGRESS            (XDMAC_CC_INITD_IN_PROGRESS_Val << XDMAC_CC_INITD_Pos) /**< (XDMAC_CC) Channel initialization is in progress. Position  */
#define XDMAC_CC_INITD_TERMINATED             (XDMAC_CC_INITD_TERMINATED_Val << XDMAC_CC_INITD_Pos) /**< (XDMAC_CC) Channel initialization is completed. Position  */
#define XDMAC_CC_RDIP_Pos                     _U_(22)                                              /**< (XDMAC_CC) Read in Progress (this bit is read-only) Position */
#define XDMAC_CC_RDIP_Msk                     (_U_(0x1) << XDMAC_CC_RDIP_Pos)                      /**< (XDMAC_CC) Read in Progress (this bit is read-only) Mask */
#define XDMAC_CC_RDIP(value)                  (XDMAC_CC_RDIP_Msk & ((value) << XDMAC_CC_RDIP_Pos))
#define   XDMAC_CC_RDIP_DONE_Val              _U_(0x0)                                             /**< (XDMAC_CC) No active read transaction on the bus.  */
#define   XDMAC_CC_RDIP_IN_PROGRESS_Val       _U_(0x1)                                             /**< (XDMAC_CC) A read transaction is in progress.  */
#define XDMAC_CC_RDIP_DONE                    (XDMAC_CC_RDIP_DONE_Val << XDMAC_CC_RDIP_Pos)        /**< (XDMAC_CC) No active read transaction on the bus. Position  */
#define XDMAC_CC_RDIP_IN_PROGRESS             (XDMAC_CC_RDIP_IN_PROGRESS_Val << XDMAC_CC_RDIP_Pos) /**< (XDMAC_CC) A read transaction is in progress. Position  */
#define XDMAC_CC_WRIP_Pos                     _U_(23)                                              /**< (XDMAC_CC) Write in Progress (this bit is read-only) Position */
#define XDMAC_CC_WRIP_Msk                     (_U_(0x1) << XDMAC_CC_WRIP_Pos)                      /**< (XDMAC_CC) Write in Progress (this bit is read-only) Mask */
#define XDMAC_CC_WRIP(value)                  (XDMAC_CC_WRIP_Msk & ((value) << XDMAC_CC_WRIP_Pos))
#define   XDMAC_CC_WRIP_DONE_Val              _U_(0x0)                                             /**< (XDMAC_CC) No active write transaction on the bus.  */
#define   XDMAC_CC_WRIP_IN_PROGRESS_Val       _U_(0x1)                                             /**< (XDMAC_CC) A write transaction is in progress.  */
#define XDMAC_CC_WRIP_DONE                    (XDMAC_CC_WRIP_DONE_Val << XDMAC_CC_WRIP_Pos)        /**< (XDMAC_CC) No active write transaction on the bus. Position  */
#define XDMAC_CC_WRIP_IN_PROGRESS             (XDMAC_CC_WRIP_IN_PROGRESS_Val << XDMAC_CC_WRIP_Pos) /**< (XDMAC_CC) A write transaction is in progress. Position  */
#define XDMAC_CC_PERID_Pos                    _U_(24)                                              /**< (XDMAC_CC) Channel x Peripheral Hardware Request Line Identifier Position */
#define XDMAC_CC_PERID_Msk                    (_U_(0x7F) << XDMAC_CC_PERID_Pos)                    /**< (XDMAC_CC) Channel x Peripheral Hardware Request Line Identifier Mask */
#define XDMAC_CC_PERID(value)                 (XDMAC_CC_PERID_Msk & ((value) << XDMAC_CC_PERID_Pos))
#define   XDMAC_CC_PERID_HSMCI_Val            _U_(0x0)                                             /**< (XDMAC_CC) HSMCI  */
#define   XDMAC_CC_PERID_SPI0_TX_Val          _U_(0x1)                                             /**< (XDMAC_CC) SPI0_TX  */
#define   XDMAC_CC_PERID_SPI0_RX_Val          _U_(0x2)                                             /**< (XDMAC_CC) SPI0_RX  */
#define   XDMAC_CC_PERID_SPI1_TX_Val          _U_(0x3)                                             /**< (XDMAC_CC) SPI1_TX  */
#define   XDMAC_CC_PERID_SPI1_RX_Val          _U_(0x4)                                             /**< (XDMAC_CC) SPI1_RX  */
#define   XDMAC_CC_PERID_QSPI_TX_Val          _U_(0x5)                                             /**< (XDMAC_CC) QSPI_TX  */
#define   XDMAC_CC_PERID_QSPI_RX_Val          _U_(0x6)                                             /**< (XDMAC_CC) QSPI_RX  */
#define   XDMAC_CC_PERID_USART0_TX_Val        _U_(0x7)                                             /**< (XDMAC_CC) USART0_TX  */
#define   XDMAC_CC_PERID_USART0_RX_Val        _U_(0x8)                                             /**< (XDMAC_CC) USART0_RX  */
#define   XDMAC_CC_PERID_USART1_TX_Val        _U_(0x9)                                             /**< (XDMAC_CC) USART1_TX  */
#define   XDMAC_CC_PERID_USART1_RX_Val        _U_(0xA)                                             /**< (XDMAC_CC) USART1_RX  */
#define   XDMAC_CC_PERID_USART2_TX_Val        _U_(0xB)                                             /**< (XDMAC_CC) USART2_TX  */
#define   XDMAC_CC_PERID_USART2_RX_Val        _U_(0xC)                                             /**< (XDMAC_CC) USART2_RX  */
#define   XDMAC_CC_PERID_PWM0_Val             _U_(0xD)                                             /**< (XDMAC_CC) PWM0  */
#define   XDMAC_CC_PERID_TWIHS0_TX_Val        _U_(0xE)                                             /**< (XDMAC_CC) TWIHS0_TX  */
#define   XDMAC_CC_PERID_TWIHS0_RX_Val        _U_(0xF)                                             /**< (XDMAC_CC) TWIHS0_RX  */
#define   XDMAC_CC_PERID_TWIHS1_TX_Val        _U_(0x10)                                            /**< (XDMAC_CC) TWIHS1_TX  */
#define   XDMAC_CC_PERID_TWIHS1_RX_Val        _U_(0x11)                                            /**< (XDMAC_CC) TWIHS1_RX  */
#define   XDMAC_CC_PERID_TWIHS2_TX_Val        _U_(0x12)                                            /**< (XDMAC_CC) TWIHS2_TX  */
#define   XDMAC_CC_PERID_TWIHS2_RX_Val        _U_(0x13)                                            /**< (XDMAC_CC) TWIHS2_RX  */
#define   XDMAC_CC_PERID_UART0_TX_Val         _U_(0x14)                                            /**< (XDMAC_CC) UART0_TX  */
#define   XDMAC_CC_PERID_UART0_RX_Val         _U_(0x15)                                            /**< (XDMAC_CC) UART0_RX  */
#define   XDMAC_CC_PERID_UART1_TX_Val         _U_(0x16)                                            /**< (XDMAC_CC) UART1_TX  */
#define   XDMAC_CC_PERID_UART1_RX_Val         _U_(0x17)                                            /**< (XDMAC_CC) UART1_RX  */
#define   XDMAC_CC_PERID_UART2_TX_Val         _U_(0x18)                                            /**< (XDMAC_CC) UART2_TX  */
#define   XDMAC_CC_PERID_UART2_RX_Val         _U_(0x19)                                            /**< (XDMAC_CC) UART2_RX  */
#define   XDMAC_CC_PERID_UART3_TX_Val         _U_(0x1A)                                            /**< (XDMAC_CC) UART3_TX  */
#define   XDMAC_CC_PERID_UART3_RX_Val         _U_(0x1B)                                            /**< (XDMAC_CC) UART3_RX  */
#define   XDMAC_CC_PERID_UART4_TX_Val         _U_(0x1C)                                            /**< (XDMAC_CC) UART4_TX  */
#define   XDMAC_CC_PERID_UART4_RX_Val         _U_(0x1D)                                            /**< (XDMAC_CC) UART4_RX  */
#define   XDMAC_CC_PERID_DACC0_Val            _U_(0x1E)                                            /**< (XDMAC_CC) DACC0  */
#define   XDMAC_CC_PERID_DACC1_Val            _U_(0x1F)                                            /**< (XDMAC_CC) DACC1  */
#define   XDMAC_CC_PERID_SSC_TX_Val           _U_(0x20)                                            /**< (XDMAC_CC) SSC_TX  */
#define   XDMAC_CC_PERID_SSC_RX_Val           _U_(0x21)                                            /**< (XDMAC_CC) SSC_RX  */
#define   XDMAC_CC_PERID_PIOA_Val             _U_(0x22)                                            /**< (XDMAC_CC) PIOA  */
#define   XDMAC_CC_PERID_AFEC0_Val            _U_(0x23)                                            /**< (XDMAC_CC) AFEC0  */
#define   XDMAC_CC_PERID_AFEC1_Val            _U_(0x24)                                            /**< (XDMAC_CC) AFEC1  */
#define   XDMAC_CC_PERID_AES_TX_Val           _U_(0x25)                                            /**< (XDMAC_CC) AES_TX  */
#define   XDMAC_CC_PERID_AES_RX_Val           _U_(0x26)                                            /**< (XDMAC_CC) AES_RX  */
#define   XDMAC_CC_PERID_PWM1_Val             _U_(0x27)                                            /**< (XDMAC_CC) PWM1  */
#define   XDMAC_CC_PERID_TC0_Val              _U_(0x28)                                            /**< (XDMAC_CC) TC0  */
#define   XDMAC_CC_PERID_TC3_Val              _U_(0x29)                                            /**< (XDMAC_CC) TC3  */
#define   XDMAC_CC_PERID_TC6_Val              _U_(0x2A)                                            /**< (XDMAC_CC) TC6  */
#define   XDMAC_CC_PERID_TC9_Val              _U_(0x2B)                                            /**< (XDMAC_CC) TC9  */
#define   XDMAC_CC_PERID_I2SC0_TX_LEFT_Val    _U_(0x2C)                                            /**< (XDMAC_CC) I2SC0_TX_LEFT  */
#define   XDMAC_CC_PERID_I2SC0_RX_LEFT_Val    _U_(0x2D)                                            /**< (XDMAC_CC) I2SC0_RX_LEFT  */
#define   XDMAC_CC_PERID_I2SC1_TX_LEFT_Val    _U_(0x2E)                                            /**< (XDMAC_CC) I2SC1_TX_LEFT  */
#define   XDMAC_CC_PERID_I2SC1_RX_LEFT_Val    _U_(0x2F)                                            /**< (XDMAC_CC) I2SC1_RX_LEFT  */
#define   XDMAC_CC_PERID_I2SC0_TX_RIGHT_Val   _U_(0x30)                                            /**< (XDMAC_CC) I2SC0_TX_RIGHT  */
#define   XDMAC_CC_PERID_I2SC0_RX_RIGHT_Val   _U_(0x31)                                            /**< (XDMAC_CC) I2SC0_RX_RIGHT  */
#define   XDMAC_CC_PERID_I2SC1_TX_RIGHT_Val   _U_(0x32)                                            /**< (XDMAC_CC) I2SC1_TX_RIGHT  */
#define   XDMAC_CC_PERID_I2SC1_RX_RIGHT_Val   _U_(0x33)                                            /**< (XDMAC_CC) I2SC1_RX_RIGHT  */
#define XDMAC_CC_PERID_HSMCI                  (XDMAC_CC_PERID_HSMCI_Val << XDMAC_CC_PERID_Pos)     /**< (XDMAC_CC) HSMCI Position  */
#define XDMAC_CC_PERID_SPI0_TX                (XDMAC_CC_PERID_SPI0_TX_Val << XDMAC_CC_PERID_Pos)   /**< (XDMAC_CC) SPI0_TX Position  */
#define XDMAC_CC_PERID_SPI0_RX                (XDMAC_CC_PERID_SPI0_RX_Val << XDMAC_CC_PERID_Pos)   /**< (XDMAC_CC) SPI0_RX Position  */
#define XDMAC_CC_PERID_SPI1_TX                (XDMAC_CC_PERID_SPI1_TX_Val << XDMAC_CC_PERID_Pos)   /**< (XDMAC_CC) SPI1_TX Position  */
#define XDMAC_CC_PERID_SPI1_RX                (XDMAC_CC_PERID_SPI1_RX_Val << XDMAC_CC_PERID_Pos)   /**< (XDMAC_CC) SPI1_RX Position  */
#define XDMAC_CC_PERID_QSPI_TX                (XDMAC_CC_PERID_QSPI_TX_Val << XDMAC_CC_PERID_Pos)   /**< (XDMAC_CC) QSPI_TX Position  */
#define XDMAC_CC_PERID_QSPI_RX                (XDMAC_CC_PERID_QSPI_RX_Val << XDMAC_CC_PERID_Pos)   /**< (XDMAC_CC) QSPI_RX Position  */
#define XDMAC_CC_PERID_USART0_TX              (XDMAC_CC_PERID_USART0_TX_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) USART0_TX Position  */
#define XDMAC_CC_PERID_USART0_RX              (XDMAC_CC_PERID_USART0_RX_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) USART0_RX Position  */
#define XDMAC_CC_PERID_USART1_TX              (XDMAC_CC_PERID_USART1_TX_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) USART1_TX Position  */
#define XDMAC_CC_PERID_USART1_RX              (XDMAC_CC_PERID_USART1_RX_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) USART1_RX Position  */
#define XDMAC_CC_PERID_USART2_TX              (XDMAC_CC_PERID_USART2_TX_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) USART2_TX Position  */
#define XDMAC_CC_PERID_USART2_RX              (XDMAC_CC_PERID_USART2_RX_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) USART2_RX Position  */
#define XDMAC_CC_PERID_PWM0                   (XDMAC_CC_PERID_PWM0_Val << XDMAC_CC_PERID_Pos)      /**< (XDMAC_CC) PWM0 Position  */
#define XDMAC_CC_PERID_TWIHS0_TX              (XDMAC_CC_PERID_TWIHS0_TX_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) TWIHS0_TX Position  */
#define XDMAC_CC_PERID_TWIHS0_RX              (XDMAC_CC_PERID_TWIHS0_RX_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) TWIHS0_RX Position  */
#define XDMAC_CC_PERID_TWIHS1_TX              (XDMAC_CC_PERID_TWIHS1_TX_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) TWIHS1_TX Position  */
#define XDMAC_CC_PERID_TWIHS1_RX              (XDMAC_CC_PERID_TWIHS1_RX_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) TWIHS1_RX Position  */
#define XDMAC_CC_PERID_TWIHS2_TX              (XDMAC_CC_PERID_TWIHS2_TX_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) TWIHS2_TX Position  */
#define XDMAC_CC_PERID_TWIHS2_RX              (XDMAC_CC_PERID_TWIHS2_RX_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) TWIHS2_RX Position  */
#define XDMAC_CC_PERID_UART0_TX               (XDMAC_CC_PERID_UART0_TX_Val << XDMAC_CC_PERID_Pos)  /**< (XDMAC_CC) UART0_TX Position  */
#define XDMAC_CC_PERID_UART0_RX               (XDMAC_CC_PERID_UART0_RX_Val << XDMAC_CC_PERID_Pos)  /**< (XDMAC_CC) UART0_RX Position  */
#define XDMAC_CC_PERID_UART1_TX               (XDMAC_CC_PERID_UART1_TX_Val << XDMAC_CC_PERID_Pos)  /**< (XDMAC_CC) UART1_TX Position  */
#define XDMAC_CC_PERID_UART1_RX               (XDMAC_CC_PERID_UART1_RX_Val << XDMAC_CC_PERID_Pos)  /**< (XDMAC_CC) UART1_RX Position  */
#define XDMAC_CC_PERID_UART2_TX               (XDMAC_CC_PERID_UART2_TX_Val << XDMAC_CC_PERID_Pos)  /**< (XDMAC_CC) UART2_TX Position  */
#define XDMAC_CC_PERID_UART2_RX               (XDMAC_CC_PERID_UART2_RX_Val << XDMAC_CC_PERID_Pos)  /**< (XDMAC_CC) UART2_RX Position  */
#define XDMAC_CC_PERID_UART3_TX               (XDMAC_CC_PERID_UART3_TX_Val << XDMAC_CC_PERID_Pos)  /**< (XDMAC_CC) UART3_TX Position  */
#define XDMAC_CC_PERID_UART3_RX               (XDMAC_CC_PERID_UART3_RX_Val << XDMAC_CC_PERID_Pos)  /**< (XDMAC_CC) UART3_RX Position  */
#define XDMAC_CC_PERID_UART4_TX               (XDMAC_CC_PERID_UART4_TX_Val << XDMAC_CC_PERID_Pos)  /**< (XDMAC_CC) UART4_TX Position  */
#define XDMAC_CC_PERID_UART4_RX               (XDMAC_CC_PERID_UART4_RX_Val << XDMAC_CC_PERID_Pos)  /**< (XDMAC_CC) UART4_RX Position  */
#define XDMAC_CC_PERID_DACC0                  (XDMAC_CC_PERID_DACC0_Val << XDMAC_CC_PERID_Pos)     /**< (XDMAC_CC) DACC0 Position  */
#define XDMAC_CC_PERID_DACC1                  (XDMAC_CC_PERID_DACC1_Val << XDMAC_CC_PERID_Pos)     /**< (XDMAC_CC) DACC1 Position  */
#define XDMAC_CC_PERID_SSC_TX                 (XDMAC_CC_PERID_SSC_TX_Val << XDMAC_CC_PERID_Pos)    /**< (XDMAC_CC) SSC_TX Position  */
#define XDMAC_CC_PERID_SSC_RX                 (XDMAC_CC_PERID_SSC_RX_Val << XDMAC_CC_PERID_Pos)    /**< (XDMAC_CC) SSC_RX Position  */
#define XDMAC_CC_PERID_PIOA                   (XDMAC_CC_PERID_PIOA_Val << XDMAC_CC_PERID_Pos)      /**< (XDMAC_CC) PIOA Position  */
#define XDMAC_CC_PERID_AFEC0                  (XDMAC_CC_PERID_AFEC0_Val << XDMAC_CC_PERID_Pos)     /**< (XDMAC_CC) AFEC0 Position  */
#define XDMAC_CC_PERID_AFEC1                  (XDMAC_CC_PERID_AFEC1_Val << XDMAC_CC_PERID_Pos)     /**< (XDMAC_CC) AFEC1 Position  */
#define XDMAC_CC_PERID_AES_TX                 (XDMAC_CC_PERID_AES_TX_Val << XDMAC_CC_PERID_Pos)    /**< (XDMAC_CC) AES_TX Position  */
#define XDMAC_CC_PERID_AES_RX                 (XDMAC_CC_PERID_AES_RX_Val << XDMAC_CC_PERID_Pos)    /**< (XDMAC_CC) AES_RX Position  */
#define XDMAC_CC_PERID_PWM1                   (XDMAC_CC_PERID_PWM1_Val << XDMAC_CC_PERID_Pos)      /**< (XDMAC_CC) PWM1 Position  */
#define XDMAC_CC_PERID_TC0                    (XDMAC_CC_PERID_TC0_Val << XDMAC_CC_PERID_Pos)       /**< (XDMAC_CC) TC0 Position  */
#define XDMAC_CC_PERID_TC3                    (XDMAC_CC_PERID_TC3_Val << XDMAC_CC_PERID_Pos)       /**< (XDMAC_CC) TC3 Position  */
#define XDMAC_CC_PERID_TC6                    (XDMAC_CC_PERID_TC6_Val << XDMAC_CC_PERID_Pos)       /**< (XDMAC_CC) TC6 Position  */
#define XDMAC_CC_PERID_TC9                    (XDMAC_CC_PERID_TC9_Val << XDMAC_CC_PERID_Pos)       /**< (XDMAC_CC) TC9 Position  */
#define XDMAC_CC_PERID_I2SC0_TX_LEFT          (XDMAC_CC_PERID_I2SC0_TX_LEFT_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) I2SC0_TX_LEFT Position  */
#define XDMAC_CC_PERID_I2SC0_RX_LEFT          (XDMAC_CC_PERID_I2SC0_RX_LEFT_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) I2SC0_RX_LEFT Position  */
#define XDMAC_CC_PERID_I2SC1_TX_LEFT          (XDMAC_CC_PERID_I2SC1_TX_LEFT_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) I2SC1_TX_LEFT Position  */
#define XDMAC_CC_PERID_I2SC1_RX_LEFT          (XDMAC_CC_PERID_I2SC1_RX_LEFT_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) I2SC1_RX_LEFT Position  */
#define XDMAC_CC_PERID_I2SC0_TX_RIGHT         (XDMAC_CC_PERID_I2SC0_TX_RIGHT_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) I2SC0_TX_RIGHT Position  */
#define XDMAC_CC_PERID_I2SC0_RX_RIGHT         (XDMAC_CC_PERID_I2SC0_RX_RIGHT_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) I2SC0_RX_RIGHT Position  */
#define XDMAC_CC_PERID_I2SC1_TX_RIGHT         (XDMAC_CC_PERID_I2SC1_TX_RIGHT_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) I2SC1_TX_RIGHT Position  */
#define XDMAC_CC_PERID_I2SC1_RX_RIGHT         (XDMAC_CC_PERID_I2SC1_RX_RIGHT_Val << XDMAC_CC_PERID_Pos) /**< (XDMAC_CC) I2SC1_RX_RIGHT Position  */
#define XDMAC_CC_Msk                          _U_(0x7FEF7FD7)                                      /**< (XDMAC_CC) Register Mask  */


/* -------- XDMAC_CDS_MSP : (XDMAC Offset: 0x2C) (R/W 32) Channel Data Stride Memory Set Pattern -------- */
#define XDMAC_CDS_MSP_SDS_MSP_Pos             _U_(0)                                               /**< (XDMAC_CDS_MSP) Channel x Source Data stride or Memory Set Pattern Position */
#define XDMAC_CDS_MSP_SDS_MSP_Msk             (_U_(0xFFFF) << XDMAC_CDS_MSP_SDS_MSP_Pos)           /**< (XDMAC_CDS_MSP) Channel x Source Data stride or Memory Set Pattern Mask */
#define XDMAC_CDS_MSP_SDS_MSP(value)          (XDMAC_CDS_MSP_SDS_MSP_Msk & ((value) << XDMAC_CDS_MSP_SDS_MSP_Pos))
#define XDMAC_CDS_MSP_DDS_MSP_Pos             _U_(16)                                              /**< (XDMAC_CDS_MSP) Channel x Destination Data Stride or Memory Set Pattern Position */
#define XDMAC_CDS_MSP_DDS_MSP_Msk             (_U_(0xFFFF) << XDMAC_CDS_MSP_DDS_MSP_Pos)           /**< (XDMAC_CDS_MSP) Channel x Destination Data Stride or Memory Set Pattern Mask */
#define XDMAC_CDS_MSP_DDS_MSP(value)          (XDMAC_CDS_MSP_DDS_MSP_Msk & ((value) << XDMAC_CDS_MSP_DDS_MSP_Pos))
#define XDMAC_CDS_MSP_Msk                     _U_(0xFFFFFFFF)                                      /**< (XDMAC_CDS_MSP) Register Mask  */


/* -------- XDMAC_CSUS : (XDMAC Offset: 0x30) (R/W 32) Channel Source Microblock Stride -------- */
#define XDMAC_CSUS_SUBS_Pos                   _U_(0)                                               /**< (XDMAC_CSUS) Channel x Source Microblock Stride Position */
#define XDMAC_CSUS_SUBS_Msk                   (_U_(0xFFFFFF) << XDMAC_CSUS_SUBS_Pos)               /**< (XDMAC_CSUS) Channel x Source Microblock Stride Mask */
#define XDMAC_CSUS_SUBS(value)                (XDMAC_CSUS_SUBS_Msk & ((value) << XDMAC_CSUS_SUBS_Pos))
#define XDMAC_CSUS_Msk                        _U_(0x00FFFFFF)                                      /**< (XDMAC_CSUS) Register Mask  */


/* -------- XDMAC_CDUS : (XDMAC Offset: 0x34) (R/W 32) Channel Destination Microblock Stride -------- */
#define XDMAC_CDUS_DUBS_Pos                   _U_(0)                                               /**< (XDMAC_CDUS) Channel x Destination Microblock Stride Position */
#define XDMAC_CDUS_DUBS_Msk                   (_U_(0xFFFFFF) << XDMAC_CDUS_DUBS_Pos)               /**< (XDMAC_CDUS) Channel x Destination Microblock Stride Mask */
#define XDMAC_CDUS_DUBS(value)                (XDMAC_CDUS_DUBS_Msk & ((value) << XDMAC_CDUS_DUBS_Pos))
#define XDMAC_CDUS_Msk                        _U_(0x00FFFFFF)                                      /**< (XDMAC_CDUS) Register Mask  */


/* -------- XDMAC_GTYPE : (XDMAC Offset: 0x00) ( R/ 32) Global Type Register -------- */
#define XDMAC_GTYPE_NB_CH_Pos                 _U_(0)                                               /**< (XDMAC_GTYPE) Number of Channels Minus One Position */
#define XDMAC_GTYPE_NB_CH_Msk                 (_U_(0x1F) << XDMAC_GTYPE_NB_CH_Pos)                 /**< (XDMAC_GTYPE) Number of Channels Minus One Mask */
#define XDMAC_GTYPE_NB_CH(value)              (XDMAC_GTYPE_NB_CH_Msk & ((value) << XDMAC_GTYPE_NB_CH_Pos))
#define XDMAC_GTYPE_FIFO_SZ_Pos               _U_(5)                                               /**< (XDMAC_GTYPE) Number of Bytes Position */
#define XDMAC_GTYPE_FIFO_SZ_Msk               (_U_(0x7FF) << XDMAC_GTYPE_FIFO_SZ_Pos)              /**< (XDMAC_GTYPE) Number of Bytes Mask */
#define XDMAC_GTYPE_FIFO_SZ(value)            (XDMAC_GTYPE_FIFO_SZ_Msk & ((value) << XDMAC_GTYPE_FIFO_SZ_Pos))
#define XDMAC_GTYPE_NB_REQ_Pos                _U_(16)                                              /**< (XDMAC_GTYPE) Number of Peripheral Requests Minus One Position */
#define XDMAC_GTYPE_NB_REQ_Msk                (_U_(0x7F) << XDMAC_GTYPE_NB_REQ_Pos)                /**< (XDMAC_GTYPE) Number of Peripheral Requests Minus One Mask */
#define XDMAC_GTYPE_NB_REQ(value)             (XDMAC_GTYPE_NB_REQ_Msk & ((value) << XDMAC_GTYPE_NB_REQ_Pos))
#define XDMAC_GTYPE_Msk                       _U_(0x007FFFFF)                                      /**< (XDMAC_GTYPE) Register Mask  */


/* -------- XDMAC_GCFG : (XDMAC Offset: 0x04) (R/W 32) Global Configuration Register -------- */
#define XDMAC_GCFG_CGDISREG_Pos               _U_(0)                                               /**< (XDMAC_GCFG) Configuration Registers Clock Gating Disable Position */
#define XDMAC_GCFG_CGDISREG_Msk               (_U_(0x1) << XDMAC_GCFG_CGDISREG_Pos)                /**< (XDMAC_GCFG) Configuration Registers Clock Gating Disable Mask */
#define XDMAC_GCFG_CGDISREG(value)            (XDMAC_GCFG_CGDISREG_Msk & ((value) << XDMAC_GCFG_CGDISREG_Pos))
#define XDMAC_GCFG_CGDISPIPE_Pos              _U_(1)                                               /**< (XDMAC_GCFG) Pipeline Clock Gating Disable Position */
#define XDMAC_GCFG_CGDISPIPE_Msk              (_U_(0x1) << XDMAC_GCFG_CGDISPIPE_Pos)               /**< (XDMAC_GCFG) Pipeline Clock Gating Disable Mask */
#define XDMAC_GCFG_CGDISPIPE(value)           (XDMAC_GCFG_CGDISPIPE_Msk & ((value) << XDMAC_GCFG_CGDISPIPE_Pos))
#define XDMAC_GCFG_CGDISFIFO_Pos              _U_(2)                                               /**< (XDMAC_GCFG) FIFO Clock Gating Disable Position */
#define XDMAC_GCFG_CGDISFIFO_Msk              (_U_(0x1) << XDMAC_GCFG_CGDISFIFO_Pos)               /**< (XDMAC_GCFG) FIFO Clock Gating Disable Mask */
#define XDMAC_GCFG_CGDISFIFO(value)           (XDMAC_GCFG_CGDISFIFO_Msk & ((value) << XDMAC_GCFG_CGDISFIFO_Pos))
#define XDMAC_GCFG_CGDISIF_Pos                _U_(3)                                               /**< (XDMAC_GCFG) Bus Interface Clock Gating Disable Position */
#define XDMAC_GCFG_CGDISIF_Msk                (_U_(0x1) << XDMAC_GCFG_CGDISIF_Pos)                 /**< (XDMAC_GCFG) Bus Interface Clock Gating Disable Mask */
#define XDMAC_GCFG_CGDISIF(value)             (XDMAC_GCFG_CGDISIF_Msk & ((value) << XDMAC_GCFG_CGDISIF_Pos))
#define XDMAC_GCFG_BXKBEN_Pos                 _U_(8)                                               /**< (XDMAC_GCFG) Boundary X Kilobyte Enable Position */
#define XDMAC_GCFG_BXKBEN_Msk                 (_U_(0x1) << XDMAC_GCFG_BXKBEN_Pos)                  /**< (XDMAC_GCFG) Boundary X Kilobyte Enable Mask */
#define XDMAC_GCFG_BXKBEN(value)              (XDMAC_GCFG_BXKBEN_Msk & ((value) << XDMAC_GCFG_BXKBEN_Pos))
#define XDMAC_GCFG_Msk                        _U_(0x0000010F)                                      /**< (XDMAC_GCFG) Register Mask  */


/* -------- XDMAC_GWAC : (XDMAC Offset: 0x08) (R/W 32) Global Weighted Arbiter Configuration Register -------- */
#define XDMAC_GWAC_PW0_Pos                    _U_(0)                                               /**< (XDMAC_GWAC) Pool Weight 0 Position */
#define XDMAC_GWAC_PW0_Msk                    (_U_(0xF) << XDMAC_GWAC_PW0_Pos)                     /**< (XDMAC_GWAC) Pool Weight 0 Mask */
#define XDMAC_GWAC_PW0(value)                 (XDMAC_GWAC_PW0_Msk & ((value) << XDMAC_GWAC_PW0_Pos))
#define XDMAC_GWAC_PW1_Pos                    _U_(4)                                               /**< (XDMAC_GWAC) Pool Weight 1 Position */
#define XDMAC_GWAC_PW1_Msk                    (_U_(0xF) << XDMAC_GWAC_PW1_Pos)                     /**< (XDMAC_GWAC) Pool Weight 1 Mask */
#define XDMAC_GWAC_PW1(value)                 (XDMAC_GWAC_PW1_Msk & ((value) << XDMAC_GWAC_PW1_Pos))
#define XDMAC_GWAC_PW2_Pos                    _U_(8)                                               /**< (XDMAC_GWAC) Pool Weight 2 Position */
#define XDMAC_GWAC_PW2_Msk                    (_U_(0xF) << XDMAC_GWAC_PW2_Pos)                     /**< (XDMAC_GWAC) Pool Weight 2 Mask */
#define XDMAC_GWAC_PW2(value)                 (XDMAC_GWAC_PW2_Msk & ((value) << XDMAC_GWAC_PW2_Pos))
#define XDMAC_GWAC_PW3_Pos                    _U_(12)                                              /**< (XDMAC_GWAC) Pool Weight 3 Position */
#define XDMAC_GWAC_PW3_Msk                    (_U_(0xF) << XDMAC_GWAC_PW3_Pos)                     /**< (XDMAC_GWAC) Pool Weight 3 Mask */
#define XDMAC_GWAC_PW3(value)                 (XDMAC_GWAC_PW3_Msk & ((value) << XDMAC_GWAC_PW3_Pos))
#define XDMAC_GWAC_Msk                        _U_(0x0000FFFF)                                      /**< (XDMAC_GWAC) Register Mask  */


/* -------- XDMAC_GIE : (XDMAC Offset: 0x0C) ( /W 32) Global Interrupt Enable Register -------- */
#define XDMAC_GIE_IE0_Pos                     _U_(0)                                               /**< (XDMAC_GIE) XDMAC Channel 0 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE0_Msk                     (_U_(0x1) << XDMAC_GIE_IE0_Pos)                      /**< (XDMAC_GIE) XDMAC Channel 0 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE0(value)                  (XDMAC_GIE_IE0_Msk & ((value) << XDMAC_GIE_IE0_Pos))
#define XDMAC_GIE_IE1_Pos                     _U_(1)                                               /**< (XDMAC_GIE) XDMAC Channel 1 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE1_Msk                     (_U_(0x1) << XDMAC_GIE_IE1_Pos)                      /**< (XDMAC_GIE) XDMAC Channel 1 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE1(value)                  (XDMAC_GIE_IE1_Msk & ((value) << XDMAC_GIE_IE1_Pos))
#define XDMAC_GIE_IE2_Pos                     _U_(2)                                               /**< (XDMAC_GIE) XDMAC Channel 2 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE2_Msk                     (_U_(0x1) << XDMAC_GIE_IE2_Pos)                      /**< (XDMAC_GIE) XDMAC Channel 2 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE2(value)                  (XDMAC_GIE_IE2_Msk & ((value) << XDMAC_GIE_IE2_Pos))
#define XDMAC_GIE_IE3_Pos                     _U_(3)                                               /**< (XDMAC_GIE) XDMAC Channel 3 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE3_Msk                     (_U_(0x1) << XDMAC_GIE_IE3_Pos)                      /**< (XDMAC_GIE) XDMAC Channel 3 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE3(value)                  (XDMAC_GIE_IE3_Msk & ((value) << XDMAC_GIE_IE3_Pos))
#define XDMAC_GIE_IE4_Pos                     _U_(4)                                               /**< (XDMAC_GIE) XDMAC Channel 4 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE4_Msk                     (_U_(0x1) << XDMAC_GIE_IE4_Pos)                      /**< (XDMAC_GIE) XDMAC Channel 4 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE4(value)                  (XDMAC_GIE_IE4_Msk & ((value) << XDMAC_GIE_IE4_Pos))
#define XDMAC_GIE_IE5_Pos                     _U_(5)                                               /**< (XDMAC_GIE) XDMAC Channel 5 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE5_Msk                     (_U_(0x1) << XDMAC_GIE_IE5_Pos)                      /**< (XDMAC_GIE) XDMAC Channel 5 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE5(value)                  (XDMAC_GIE_IE5_Msk & ((value) << XDMAC_GIE_IE5_Pos))
#define XDMAC_GIE_IE6_Pos                     _U_(6)                                               /**< (XDMAC_GIE) XDMAC Channel 6 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE6_Msk                     (_U_(0x1) << XDMAC_GIE_IE6_Pos)                      /**< (XDMAC_GIE) XDMAC Channel 6 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE6(value)                  (XDMAC_GIE_IE6_Msk & ((value) << XDMAC_GIE_IE6_Pos))
#define XDMAC_GIE_IE7_Pos                     _U_(7)                                               /**< (XDMAC_GIE) XDMAC Channel 7 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE7_Msk                     (_U_(0x1) << XDMAC_GIE_IE7_Pos)                      /**< (XDMAC_GIE) XDMAC Channel 7 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE7(value)                  (XDMAC_GIE_IE7_Msk & ((value) << XDMAC_GIE_IE7_Pos))
#define XDMAC_GIE_IE8_Pos                     _U_(8)                                               /**< (XDMAC_GIE) XDMAC Channel 8 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE8_Msk                     (_U_(0x1) << XDMAC_GIE_IE8_Pos)                      /**< (XDMAC_GIE) XDMAC Channel 8 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE8(value)                  (XDMAC_GIE_IE8_Msk & ((value) << XDMAC_GIE_IE8_Pos))
#define XDMAC_GIE_IE9_Pos                     _U_(9)                                               /**< (XDMAC_GIE) XDMAC Channel 9 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE9_Msk                     (_U_(0x1) << XDMAC_GIE_IE9_Pos)                      /**< (XDMAC_GIE) XDMAC Channel 9 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE9(value)                  (XDMAC_GIE_IE9_Msk & ((value) << XDMAC_GIE_IE9_Pos))
#define XDMAC_GIE_IE10_Pos                    _U_(10)                                              /**< (XDMAC_GIE) XDMAC Channel 10 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE10_Msk                    (_U_(0x1) << XDMAC_GIE_IE10_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 10 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE10(value)                 (XDMAC_GIE_IE10_Msk & ((value) << XDMAC_GIE_IE10_Pos))
#define XDMAC_GIE_IE11_Pos                    _U_(11)                                              /**< (XDMAC_GIE) XDMAC Channel 11 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE11_Msk                    (_U_(0x1) << XDMAC_GIE_IE11_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 11 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE11(value)                 (XDMAC_GIE_IE11_Msk & ((value) << XDMAC_GIE_IE11_Pos))
#define XDMAC_GIE_IE12_Pos                    _U_(12)                                              /**< (XDMAC_GIE) XDMAC Channel 12 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE12_Msk                    (_U_(0x1) << XDMAC_GIE_IE12_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 12 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE12(value)                 (XDMAC_GIE_IE12_Msk & ((value) << XDMAC_GIE_IE12_Pos))
#define XDMAC_GIE_IE13_Pos                    _U_(13)                                              /**< (XDMAC_GIE) XDMAC Channel 13 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE13_Msk                    (_U_(0x1) << XDMAC_GIE_IE13_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 13 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE13(value)                 (XDMAC_GIE_IE13_Msk & ((value) << XDMAC_GIE_IE13_Pos))
#define XDMAC_GIE_IE14_Pos                    _U_(14)                                              /**< (XDMAC_GIE) XDMAC Channel 14 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE14_Msk                    (_U_(0x1) << XDMAC_GIE_IE14_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 14 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE14(value)                 (XDMAC_GIE_IE14_Msk & ((value) << XDMAC_GIE_IE14_Pos))
#define XDMAC_GIE_IE15_Pos                    _U_(15)                                              /**< (XDMAC_GIE) XDMAC Channel 15 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE15_Msk                    (_U_(0x1) << XDMAC_GIE_IE15_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 15 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE15(value)                 (XDMAC_GIE_IE15_Msk & ((value) << XDMAC_GIE_IE15_Pos))
#define XDMAC_GIE_IE16_Pos                    _U_(16)                                              /**< (XDMAC_GIE) XDMAC Channel 16 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE16_Msk                    (_U_(0x1) << XDMAC_GIE_IE16_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 16 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE16(value)                 (XDMAC_GIE_IE16_Msk & ((value) << XDMAC_GIE_IE16_Pos))
#define XDMAC_GIE_IE17_Pos                    _U_(17)                                              /**< (XDMAC_GIE) XDMAC Channel 17 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE17_Msk                    (_U_(0x1) << XDMAC_GIE_IE17_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 17 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE17(value)                 (XDMAC_GIE_IE17_Msk & ((value) << XDMAC_GIE_IE17_Pos))
#define XDMAC_GIE_IE18_Pos                    _U_(18)                                              /**< (XDMAC_GIE) XDMAC Channel 18 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE18_Msk                    (_U_(0x1) << XDMAC_GIE_IE18_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 18 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE18(value)                 (XDMAC_GIE_IE18_Msk & ((value) << XDMAC_GIE_IE18_Pos))
#define XDMAC_GIE_IE19_Pos                    _U_(19)                                              /**< (XDMAC_GIE) XDMAC Channel 19 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE19_Msk                    (_U_(0x1) << XDMAC_GIE_IE19_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 19 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE19(value)                 (XDMAC_GIE_IE19_Msk & ((value) << XDMAC_GIE_IE19_Pos))
#define XDMAC_GIE_IE20_Pos                    _U_(20)                                              /**< (XDMAC_GIE) XDMAC Channel 20 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE20_Msk                    (_U_(0x1) << XDMAC_GIE_IE20_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 20 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE20(value)                 (XDMAC_GIE_IE20_Msk & ((value) << XDMAC_GIE_IE20_Pos))
#define XDMAC_GIE_IE21_Pos                    _U_(21)                                              /**< (XDMAC_GIE) XDMAC Channel 21 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE21_Msk                    (_U_(0x1) << XDMAC_GIE_IE21_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 21 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE21(value)                 (XDMAC_GIE_IE21_Msk & ((value) << XDMAC_GIE_IE21_Pos))
#define XDMAC_GIE_IE22_Pos                    _U_(22)                                              /**< (XDMAC_GIE) XDMAC Channel 22 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE22_Msk                    (_U_(0x1) << XDMAC_GIE_IE22_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 22 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE22(value)                 (XDMAC_GIE_IE22_Msk & ((value) << XDMAC_GIE_IE22_Pos))
#define XDMAC_GIE_IE23_Pos                    _U_(23)                                              /**< (XDMAC_GIE) XDMAC Channel 23 Interrupt Enable Bit Position */
#define XDMAC_GIE_IE23_Msk                    (_U_(0x1) << XDMAC_GIE_IE23_Pos)                     /**< (XDMAC_GIE) XDMAC Channel 23 Interrupt Enable Bit Mask */
#define XDMAC_GIE_IE23(value)                 (XDMAC_GIE_IE23_Msk & ((value) << XDMAC_GIE_IE23_Pos))
#define XDMAC_GIE_Msk                         _U_(0x00FFFFFF)                                      /**< (XDMAC_GIE) Register Mask  */

#define XDMAC_GIE_IE_Pos                      _U_(0)                                               /**< (XDMAC_GIE Position) XDMAC Channel 23 Interrupt Enable Bit */
#define XDMAC_GIE_IE_Msk                      (_U_(0xFFFFFF) << XDMAC_GIE_IE_Pos)                  /**< (XDMAC_GIE Mask) IE */
#define XDMAC_GIE_IE(value)                   (XDMAC_GIE_IE_Msk & ((value) << XDMAC_GIE_IE_Pos))   

/* -------- XDMAC_GID : (XDMAC Offset: 0x10) ( /W 32) Global Interrupt Disable Register -------- */
#define XDMAC_GID_ID0_Pos                     _U_(0)                                               /**< (XDMAC_GID) XDMAC Channel 0 Interrupt Disable Bit Position */
#define XDMAC_GID_ID0_Msk                     (_U_(0x1) << XDMAC_GID_ID0_Pos)                      /**< (XDMAC_GID) XDMAC Channel 0 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID0(value)                  (XDMAC_GID_ID0_Msk & ((value) << XDMAC_GID_ID0_Pos))
#define XDMAC_GID_ID1_Pos                     _U_(1)                                               /**< (XDMAC_GID) XDMAC Channel 1 Interrupt Disable Bit Position */
#define XDMAC_GID_ID1_Msk                     (_U_(0x1) << XDMAC_GID_ID1_Pos)                      /**< (XDMAC_GID) XDMAC Channel 1 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID1(value)                  (XDMAC_GID_ID1_Msk & ((value) << XDMAC_GID_ID1_Pos))
#define XDMAC_GID_ID2_Pos                     _U_(2)                                               /**< (XDMAC_GID) XDMAC Channel 2 Interrupt Disable Bit Position */
#define XDMAC_GID_ID2_Msk                     (_U_(0x1) << XDMAC_GID_ID2_Pos)                      /**< (XDMAC_GID) XDMAC Channel 2 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID2(value)                  (XDMAC_GID_ID2_Msk & ((value) << XDMAC_GID_ID2_Pos))
#define XDMAC_GID_ID3_Pos                     _U_(3)                                               /**< (XDMAC_GID) XDMAC Channel 3 Interrupt Disable Bit Position */
#define XDMAC_GID_ID3_Msk                     (_U_(0x1) << XDMAC_GID_ID3_Pos)                      /**< (XDMAC_GID) XDMAC Channel 3 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID3(value)                  (XDMAC_GID_ID3_Msk & ((value) << XDMAC_GID_ID3_Pos))
#define XDMAC_GID_ID4_Pos                     _U_(4)                                               /**< (XDMAC_GID) XDMAC Channel 4 Interrupt Disable Bit Position */
#define XDMAC_GID_ID4_Msk                     (_U_(0x1) << XDMAC_GID_ID4_Pos)                      /**< (XDMAC_GID) XDMAC Channel 4 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID4(value)                  (XDMAC_GID_ID4_Msk & ((value) << XDMAC_GID_ID4_Pos))
#define XDMAC_GID_ID5_Pos                     _U_(5)                                               /**< (XDMAC_GID) XDMAC Channel 5 Interrupt Disable Bit Position */
#define XDMAC_GID_ID5_Msk                     (_U_(0x1) << XDMAC_GID_ID5_Pos)                      /**< (XDMAC_GID) XDMAC Channel 5 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID5(value)                  (XDMAC_GID_ID5_Msk & ((value) << XDMAC_GID_ID5_Pos))
#define XDMAC_GID_ID6_Pos                     _U_(6)                                               /**< (XDMAC_GID) XDMAC Channel 6 Interrupt Disable Bit Position */
#define XDMAC_GID_ID6_Msk                     (_U_(0x1) << XDMAC_GID_ID6_Pos)                      /**< (XDMAC_GID) XDMAC Channel 6 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID6(value)                  (XDMAC_GID_ID6_Msk & ((value) << XDMAC_GID_ID6_Pos))
#define XDMAC_GID_ID7_Pos                     _U_(7)                                               /**< (XDMAC_GID) XDMAC Channel 7 Interrupt Disable Bit Position */
#define XDMAC_GID_ID7_Msk                     (_U_(0x1) << XDMAC_GID_ID7_Pos)                      /**< (XDMAC_GID) XDMAC Channel 7 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID7(value)                  (XDMAC_GID_ID7_Msk & ((value) << XDMAC_GID_ID7_Pos))
#define XDMAC_GID_ID8_Pos                     _U_(8)                                               /**< (XDMAC_GID) XDMAC Channel 8 Interrupt Disable Bit Position */
#define XDMAC_GID_ID8_Msk                     (_U_(0x1) << XDMAC_GID_ID8_Pos)                      /**< (XDMAC_GID) XDMAC Channel 8 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID8(value)                  (XDMAC_GID_ID8_Msk & ((value) << XDMAC_GID_ID8_Pos))
#define XDMAC_GID_ID9_Pos                     _U_(9)                                               /**< (XDMAC_GID) XDMAC Channel 9 Interrupt Disable Bit Position */
#define XDMAC_GID_ID9_Msk                     (_U_(0x1) << XDMAC_GID_ID9_Pos)                      /**< (XDMAC_GID) XDMAC Channel 9 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID9(value)                  (XDMAC_GID_ID9_Msk & ((value) << XDMAC_GID_ID9_Pos))
#define XDMAC_GID_ID10_Pos                    _U_(10)                                              /**< (XDMAC_GID) XDMAC Channel 10 Interrupt Disable Bit Position */
#define XDMAC_GID_ID10_Msk                    (_U_(0x1) << XDMAC_GID_ID10_Pos)                     /**< (XDMAC_GID) XDMAC Channel 10 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID10(value)                 (XDMAC_GID_ID10_Msk & ((value) << XDMAC_GID_ID10_Pos))
#define XDMAC_GID_ID11_Pos                    _U_(11)                                              /**< (XDMAC_GID) XDMAC Channel 11 Interrupt Disable Bit Position */
#define XDMAC_GID_ID11_Msk                    (_U_(0x1) << XDMAC_GID_ID11_Pos)                     /**< (XDMAC_GID) XDMAC Channel 11 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID11(value)                 (XDMAC_GID_ID11_Msk & ((value) << XDMAC_GID_ID11_Pos))
#define XDMAC_GID_ID12_Pos                    _U_(12)                                              /**< (XDMAC_GID) XDMAC Channel 12 Interrupt Disable Bit Position */
#define XDMAC_GID_ID12_Msk                    (_U_(0x1) << XDMAC_GID_ID12_Pos)                     /**< (XDMAC_GID) XDMAC Channel 12 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID12(value)                 (XDMAC_GID_ID12_Msk & ((value) << XDMAC_GID_ID12_Pos))
#define XDMAC_GID_ID13_Pos                    _U_(13)                                              /**< (XDMAC_GID) XDMAC Channel 13 Interrupt Disable Bit Position */
#define XDMAC_GID_ID13_Msk                    (_U_(0x1) << XDMAC_GID_ID13_Pos)                     /**< (XDMAC_GID) XDMAC Channel 13 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID13(value)                 (XDMAC_GID_ID13_Msk & ((value) << XDMAC_GID_ID13_Pos))
#define XDMAC_GID_ID14_Pos                    _U_(14)                                              /**< (XDMAC_GID) XDMAC Channel 14 Interrupt Disable Bit Position */
#define XDMAC_GID_ID14_Msk                    (_U_(0x1) << XDMAC_GID_ID14_Pos)                     /**< (XDMAC_GID) XDMAC Channel 14 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID14(value)                 (XDMAC_GID_ID14_Msk & ((value) << XDMAC_GID_ID14_Pos))
#define XDMAC_GID_ID15_Pos                    _U_(15)                                              /**< (XDMAC_GID) XDMAC Channel 15 Interrupt Disable Bit Position */
#define XDMAC_GID_ID15_Msk                    (_U_(0x1) << XDMAC_GID_ID15_Pos)                     /**< (XDMAC_GID) XDMAC Channel 15 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID15(value)                 (XDMAC_GID_ID15_Msk & ((value) << XDMAC_GID_ID15_Pos))
#define XDMAC_GID_ID16_Pos                    _U_(16)                                              /**< (XDMAC_GID) XDMAC Channel 16 Interrupt Disable Bit Position */
#define XDMAC_GID_ID16_Msk                    (_U_(0x1) << XDMAC_GID_ID16_Pos)                     /**< (XDMAC_GID) XDMAC Channel 16 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID16(value)                 (XDMAC_GID_ID16_Msk & ((value) << XDMAC_GID_ID16_Pos))
#define XDMAC_GID_ID17_Pos                    _U_(17)                                              /**< (XDMAC_GID) XDMAC Channel 17 Interrupt Disable Bit Position */
#define XDMAC_GID_ID17_Msk                    (_U_(0x1) << XDMAC_GID_ID17_Pos)                     /**< (XDMAC_GID) XDMAC Channel 17 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID17(value)                 (XDMAC_GID_ID17_Msk & ((value) << XDMAC_GID_ID17_Pos))
#define XDMAC_GID_ID18_Pos                    _U_(18)                                              /**< (XDMAC_GID) XDMAC Channel 18 Interrupt Disable Bit Position */
#define XDMAC_GID_ID18_Msk                    (_U_(0x1) << XDMAC_GID_ID18_Pos)                     /**< (XDMAC_GID) XDMAC Channel 18 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID18(value)                 (XDMAC_GID_ID18_Msk & ((value) << XDMAC_GID_ID18_Pos))
#define XDMAC_GID_ID19_Pos                    _U_(19)                                              /**< (XDMAC_GID) XDMAC Channel 19 Interrupt Disable Bit Position */
#define XDMAC_GID_ID19_Msk                    (_U_(0x1) << XDMAC_GID_ID19_Pos)                     /**< (XDMAC_GID) XDMAC Channel 19 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID19(value)                 (XDMAC_GID_ID19_Msk & ((value) << XDMAC_GID_ID19_Pos))
#define XDMAC_GID_ID20_Pos                    _U_(20)                                              /**< (XDMAC_GID) XDMAC Channel 20 Interrupt Disable Bit Position */
#define XDMAC_GID_ID20_Msk                    (_U_(0x1) << XDMAC_GID_ID20_Pos)                     /**< (XDMAC_GID) XDMAC Channel 20 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID20(value)                 (XDMAC_GID_ID20_Msk & ((value) << XDMAC_GID_ID20_Pos))
#define XDMAC_GID_ID21_Pos                    _U_(21)                                              /**< (XDMAC_GID) XDMAC Channel 21 Interrupt Disable Bit Position */
#define XDMAC_GID_ID21_Msk                    (_U_(0x1) << XDMAC_GID_ID21_Pos)                     /**< (XDMAC_GID) XDMAC Channel 21 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID21(value)                 (XDMAC_GID_ID21_Msk & ((value) << XDMAC_GID_ID21_Pos))
#define XDMAC_GID_ID22_Pos                    _U_(22)                                              /**< (XDMAC_GID) XDMAC Channel 22 Interrupt Disable Bit Position */
#define XDMAC_GID_ID22_Msk                    (_U_(0x1) << XDMAC_GID_ID22_Pos)                     /**< (XDMAC_GID) XDMAC Channel 22 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID22(value)                 (XDMAC_GID_ID22_Msk & ((value) << XDMAC_GID_ID22_Pos))
#define XDMAC_GID_ID23_Pos                    _U_(23)                                              /**< (XDMAC_GID) XDMAC Channel 23 Interrupt Disable Bit Position */
#define XDMAC_GID_ID23_Msk                    (_U_(0x1) << XDMAC_GID_ID23_Pos)                     /**< (XDMAC_GID) XDMAC Channel 23 Interrupt Disable Bit Mask */
#define XDMAC_GID_ID23(value)                 (XDMAC_GID_ID23_Msk & ((value) << XDMAC_GID_ID23_Pos))
#define XDMAC_GID_Msk                         _U_(0x00FFFFFF)                                      /**< (XDMAC_GID) Register Mask  */

#define XDMAC_GID_ID_Pos                      _U_(0)                                               /**< (XDMAC_GID Position) XDMAC Channel 23 Interrupt Disable Bit */
#define XDMAC_GID_ID_Msk                      (_U_(0xFFFFFF) << XDMAC_GID_ID_Pos)                  /**< (XDMAC_GID Mask) ID */
#define XDMAC_GID_ID(value)                   (XDMAC_GID_ID_Msk & ((value) << XDMAC_GID_ID_Pos))   

/* -------- XDMAC_GIM : (XDMAC Offset: 0x14) ( R/ 32) Global Interrupt Mask Register -------- */
#define XDMAC_GIM_IM0_Pos                     _U_(0)                                               /**< (XDMAC_GIM) XDMAC Channel 0 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM0_Msk                     (_U_(0x1) << XDMAC_GIM_IM0_Pos)                      /**< (XDMAC_GIM) XDMAC Channel 0 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM0(value)                  (XDMAC_GIM_IM0_Msk & ((value) << XDMAC_GIM_IM0_Pos))
#define XDMAC_GIM_IM1_Pos                     _U_(1)                                               /**< (XDMAC_GIM) XDMAC Channel 1 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM1_Msk                     (_U_(0x1) << XDMAC_GIM_IM1_Pos)                      /**< (XDMAC_GIM) XDMAC Channel 1 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM1(value)                  (XDMAC_GIM_IM1_Msk & ((value) << XDMAC_GIM_IM1_Pos))
#define XDMAC_GIM_IM2_Pos                     _U_(2)                                               /**< (XDMAC_GIM) XDMAC Channel 2 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM2_Msk                     (_U_(0x1) << XDMAC_GIM_IM2_Pos)                      /**< (XDMAC_GIM) XDMAC Channel 2 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM2(value)                  (XDMAC_GIM_IM2_Msk & ((value) << XDMAC_GIM_IM2_Pos))
#define XDMAC_GIM_IM3_Pos                     _U_(3)                                               /**< (XDMAC_GIM) XDMAC Channel 3 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM3_Msk                     (_U_(0x1) << XDMAC_GIM_IM3_Pos)                      /**< (XDMAC_GIM) XDMAC Channel 3 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM3(value)                  (XDMAC_GIM_IM3_Msk & ((value) << XDMAC_GIM_IM3_Pos))
#define XDMAC_GIM_IM4_Pos                     _U_(4)                                               /**< (XDMAC_GIM) XDMAC Channel 4 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM4_Msk                     (_U_(0x1) << XDMAC_GIM_IM4_Pos)                      /**< (XDMAC_GIM) XDMAC Channel 4 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM4(value)                  (XDMAC_GIM_IM4_Msk & ((value) << XDMAC_GIM_IM4_Pos))
#define XDMAC_GIM_IM5_Pos                     _U_(5)                                               /**< (XDMAC_GIM) XDMAC Channel 5 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM5_Msk                     (_U_(0x1) << XDMAC_GIM_IM5_Pos)                      /**< (XDMAC_GIM) XDMAC Channel 5 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM5(value)                  (XDMAC_GIM_IM5_Msk & ((value) << XDMAC_GIM_IM5_Pos))
#define XDMAC_GIM_IM6_Pos                     _U_(6)                                               /**< (XDMAC_GIM) XDMAC Channel 6 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM6_Msk                     (_U_(0x1) << XDMAC_GIM_IM6_Pos)                      /**< (XDMAC_GIM) XDMAC Channel 6 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM6(value)                  (XDMAC_GIM_IM6_Msk & ((value) << XDMAC_GIM_IM6_Pos))
#define XDMAC_GIM_IM7_Pos                     _U_(7)                                               /**< (XDMAC_GIM) XDMAC Channel 7 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM7_Msk                     (_U_(0x1) << XDMAC_GIM_IM7_Pos)                      /**< (XDMAC_GIM) XDMAC Channel 7 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM7(value)                  (XDMAC_GIM_IM7_Msk & ((value) << XDMAC_GIM_IM7_Pos))
#define XDMAC_GIM_IM8_Pos                     _U_(8)                                               /**< (XDMAC_GIM) XDMAC Channel 8 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM8_Msk                     (_U_(0x1) << XDMAC_GIM_IM8_Pos)                      /**< (XDMAC_GIM) XDMAC Channel 8 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM8(value)                  (XDMAC_GIM_IM8_Msk & ((value) << XDMAC_GIM_IM8_Pos))
#define XDMAC_GIM_IM9_Pos                     _U_(9)                                               /**< (XDMAC_GIM) XDMAC Channel 9 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM9_Msk                     (_U_(0x1) << XDMAC_GIM_IM9_Pos)                      /**< (XDMAC_GIM) XDMAC Channel 9 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM9(value)                  (XDMAC_GIM_IM9_Msk & ((value) << XDMAC_GIM_IM9_Pos))
#define XDMAC_GIM_IM10_Pos                    _U_(10)                                              /**< (XDMAC_GIM) XDMAC Channel 10 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM10_Msk                    (_U_(0x1) << XDMAC_GIM_IM10_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 10 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM10(value)                 (XDMAC_GIM_IM10_Msk & ((value) << XDMAC_GIM_IM10_Pos))
#define XDMAC_GIM_IM11_Pos                    _U_(11)                                              /**< (XDMAC_GIM) XDMAC Channel 11 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM11_Msk                    (_U_(0x1) << XDMAC_GIM_IM11_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 11 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM11(value)                 (XDMAC_GIM_IM11_Msk & ((value) << XDMAC_GIM_IM11_Pos))
#define XDMAC_GIM_IM12_Pos                    _U_(12)                                              /**< (XDMAC_GIM) XDMAC Channel 12 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM12_Msk                    (_U_(0x1) << XDMAC_GIM_IM12_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 12 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM12(value)                 (XDMAC_GIM_IM12_Msk & ((value) << XDMAC_GIM_IM12_Pos))
#define XDMAC_GIM_IM13_Pos                    _U_(13)                                              /**< (XDMAC_GIM) XDMAC Channel 13 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM13_Msk                    (_U_(0x1) << XDMAC_GIM_IM13_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 13 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM13(value)                 (XDMAC_GIM_IM13_Msk & ((value) << XDMAC_GIM_IM13_Pos))
#define XDMAC_GIM_IM14_Pos                    _U_(14)                                              /**< (XDMAC_GIM) XDMAC Channel 14 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM14_Msk                    (_U_(0x1) << XDMAC_GIM_IM14_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 14 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM14(value)                 (XDMAC_GIM_IM14_Msk & ((value) << XDMAC_GIM_IM14_Pos))
#define XDMAC_GIM_IM15_Pos                    _U_(15)                                              /**< (XDMAC_GIM) XDMAC Channel 15 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM15_Msk                    (_U_(0x1) << XDMAC_GIM_IM15_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 15 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM15(value)                 (XDMAC_GIM_IM15_Msk & ((value) << XDMAC_GIM_IM15_Pos))
#define XDMAC_GIM_IM16_Pos                    _U_(16)                                              /**< (XDMAC_GIM) XDMAC Channel 16 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM16_Msk                    (_U_(0x1) << XDMAC_GIM_IM16_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 16 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM16(value)                 (XDMAC_GIM_IM16_Msk & ((value) << XDMAC_GIM_IM16_Pos))
#define XDMAC_GIM_IM17_Pos                    _U_(17)                                              /**< (XDMAC_GIM) XDMAC Channel 17 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM17_Msk                    (_U_(0x1) << XDMAC_GIM_IM17_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 17 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM17(value)                 (XDMAC_GIM_IM17_Msk & ((value) << XDMAC_GIM_IM17_Pos))
#define XDMAC_GIM_IM18_Pos                    _U_(18)                                              /**< (XDMAC_GIM) XDMAC Channel 18 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM18_Msk                    (_U_(0x1) << XDMAC_GIM_IM18_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 18 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM18(value)                 (XDMAC_GIM_IM18_Msk & ((value) << XDMAC_GIM_IM18_Pos))
#define XDMAC_GIM_IM19_Pos                    _U_(19)                                              /**< (XDMAC_GIM) XDMAC Channel 19 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM19_Msk                    (_U_(0x1) << XDMAC_GIM_IM19_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 19 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM19(value)                 (XDMAC_GIM_IM19_Msk & ((value) << XDMAC_GIM_IM19_Pos))
#define XDMAC_GIM_IM20_Pos                    _U_(20)                                              /**< (XDMAC_GIM) XDMAC Channel 20 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM20_Msk                    (_U_(0x1) << XDMAC_GIM_IM20_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 20 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM20(value)                 (XDMAC_GIM_IM20_Msk & ((value) << XDMAC_GIM_IM20_Pos))
#define XDMAC_GIM_IM21_Pos                    _U_(21)                                              /**< (XDMAC_GIM) XDMAC Channel 21 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM21_Msk                    (_U_(0x1) << XDMAC_GIM_IM21_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 21 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM21(value)                 (XDMAC_GIM_IM21_Msk & ((value) << XDMAC_GIM_IM21_Pos))
#define XDMAC_GIM_IM22_Pos                    _U_(22)                                              /**< (XDMAC_GIM) XDMAC Channel 22 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM22_Msk                    (_U_(0x1) << XDMAC_GIM_IM22_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 22 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM22(value)                 (XDMAC_GIM_IM22_Msk & ((value) << XDMAC_GIM_IM22_Pos))
#define XDMAC_GIM_IM23_Pos                    _U_(23)                                              /**< (XDMAC_GIM) XDMAC Channel 23 Interrupt Mask Bit Position */
#define XDMAC_GIM_IM23_Msk                    (_U_(0x1) << XDMAC_GIM_IM23_Pos)                     /**< (XDMAC_GIM) XDMAC Channel 23 Interrupt Mask Bit Mask */
#define XDMAC_GIM_IM23(value)                 (XDMAC_GIM_IM23_Msk & ((value) << XDMAC_GIM_IM23_Pos))
#define XDMAC_GIM_Msk                         _U_(0x00FFFFFF)                                      /**< (XDMAC_GIM) Register Mask  */

#define XDMAC_GIM_IM_Pos                      _U_(0)                                               /**< (XDMAC_GIM Position) XDMAC Channel 23 Interrupt Mask Bit */
#define XDMAC_GIM_IM_Msk                      (_U_(0xFFFFFF) << XDMAC_GIM_IM_Pos)                  /**< (XDMAC_GIM Mask) IM */
#define XDMAC_GIM_IM(value)                   (XDMAC_GIM_IM_Msk & ((value) << XDMAC_GIM_IM_Pos))   

/* -------- XDMAC_GIS : (XDMAC Offset: 0x18) ( R/ 32) Global Interrupt Status Register -------- */
#define XDMAC_GIS_IS0_Pos                     _U_(0)                                               /**< (XDMAC_GIS) XDMAC Channel 0 Interrupt Status Bit Position */
#define XDMAC_GIS_IS0_Msk                     (_U_(0x1) << XDMAC_GIS_IS0_Pos)                      /**< (XDMAC_GIS) XDMAC Channel 0 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS0(value)                  (XDMAC_GIS_IS0_Msk & ((value) << XDMAC_GIS_IS0_Pos))
#define XDMAC_GIS_IS1_Pos                     _U_(1)                                               /**< (XDMAC_GIS) XDMAC Channel 1 Interrupt Status Bit Position */
#define XDMAC_GIS_IS1_Msk                     (_U_(0x1) << XDMAC_GIS_IS1_Pos)                      /**< (XDMAC_GIS) XDMAC Channel 1 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS1(value)                  (XDMAC_GIS_IS1_Msk & ((value) << XDMAC_GIS_IS1_Pos))
#define XDMAC_GIS_IS2_Pos                     _U_(2)                                               /**< (XDMAC_GIS) XDMAC Channel 2 Interrupt Status Bit Position */
#define XDMAC_GIS_IS2_Msk                     (_U_(0x1) << XDMAC_GIS_IS2_Pos)                      /**< (XDMAC_GIS) XDMAC Channel 2 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS2(value)                  (XDMAC_GIS_IS2_Msk & ((value) << XDMAC_GIS_IS2_Pos))
#define XDMAC_GIS_IS3_Pos                     _U_(3)                                               /**< (XDMAC_GIS) XDMAC Channel 3 Interrupt Status Bit Position */
#define XDMAC_GIS_IS3_Msk                     (_U_(0x1) << XDMAC_GIS_IS3_Pos)                      /**< (XDMAC_GIS) XDMAC Channel 3 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS3(value)                  (XDMAC_GIS_IS3_Msk & ((value) << XDMAC_GIS_IS3_Pos))
#define XDMAC_GIS_IS4_Pos                     _U_(4)                                               /**< (XDMAC_GIS) XDMAC Channel 4 Interrupt Status Bit Position */
#define XDMAC_GIS_IS4_Msk                     (_U_(0x1) << XDMAC_GIS_IS4_Pos)                      /**< (XDMAC_GIS) XDMAC Channel 4 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS4(value)                  (XDMAC_GIS_IS4_Msk & ((value) << XDMAC_GIS_IS4_Pos))
#define XDMAC_GIS_IS5_Pos                     _U_(5)                                               /**< (XDMAC_GIS) XDMAC Channel 5 Interrupt Status Bit Position */
#define XDMAC_GIS_IS5_Msk                     (_U_(0x1) << XDMAC_GIS_IS5_Pos)                      /**< (XDMAC_GIS) XDMAC Channel 5 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS5(value)                  (XDMAC_GIS_IS5_Msk & ((value) << XDMAC_GIS_IS5_Pos))
#define XDMAC_GIS_IS6_Pos                     _U_(6)                                               /**< (XDMAC_GIS) XDMAC Channel 6 Interrupt Status Bit Position */
#define XDMAC_GIS_IS6_Msk                     (_U_(0x1) << XDMAC_GIS_IS6_Pos)                      /**< (XDMAC_GIS) XDMAC Channel 6 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS6(value)                  (XDMAC_GIS_IS6_Msk & ((value) << XDMAC_GIS_IS6_Pos))
#define XDMAC_GIS_IS7_Pos                     _U_(7)                                               /**< (XDMAC_GIS) XDMAC Channel 7 Interrupt Status Bit Position */
#define XDMAC_GIS_IS7_Msk                     (_U_(0x1) << XDMAC_GIS_IS7_Pos)                      /**< (XDMAC_GIS) XDMAC Channel 7 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS7(value)                  (XDMAC_GIS_IS7_Msk & ((value) << XDMAC_GIS_IS7_Pos))
#define XDMAC_GIS_IS8_Pos                     _U_(8)                                               /**< (XDMAC_GIS) XDMAC Channel 8 Interrupt Status Bit Position */
#define XDMAC_GIS_IS8_Msk                     (_U_(0x1) << XDMAC_GIS_IS8_Pos)                      /**< (XDMAC_GIS) XDMAC Channel 8 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS8(value)                  (XDMAC_GIS_IS8_Msk & ((value) << XDMAC_GIS_IS8_Pos))
#define XDMAC_GIS_IS9_Pos                     _U_(9)                                               /**< (XDMAC_GIS) XDMAC Channel 9 Interrupt Status Bit Position */
#define XDMAC_GIS_IS9_Msk                     (_U_(0x1) << XDMAC_GIS_IS9_Pos)                      /**< (XDMAC_GIS) XDMAC Channel 9 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS9(value)                  (XDMAC_GIS_IS9_Msk & ((value) << XDMAC_GIS_IS9_Pos))
#define XDMAC_GIS_IS10_Pos                    _U_(10)                                              /**< (XDMAC_GIS) XDMAC Channel 10 Interrupt Status Bit Position */
#define XDMAC_GIS_IS10_Msk                    (_U_(0x1) << XDMAC_GIS_IS10_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 10 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS10(value)                 (XDMAC_GIS_IS10_Msk & ((value) << XDMAC_GIS_IS10_Pos))
#define XDMAC_GIS_IS11_Pos                    _U_(11)                                              /**< (XDMAC_GIS) XDMAC Channel 11 Interrupt Status Bit Position */
#define XDMAC_GIS_IS11_Msk                    (_U_(0x1) << XDMAC_GIS_IS11_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 11 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS11(value)                 (XDMAC_GIS_IS11_Msk & ((value) << XDMAC_GIS_IS11_Pos))
#define XDMAC_GIS_IS12_Pos                    _U_(12)                                              /**< (XDMAC_GIS) XDMAC Channel 12 Interrupt Status Bit Position */
#define XDMAC_GIS_IS12_Msk                    (_U_(0x1) << XDMAC_GIS_IS12_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 12 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS12(value)                 (XDMAC_GIS_IS12_Msk & ((value) << XDMAC_GIS_IS12_Pos))
#define XDMAC_GIS_IS13_Pos                    _U_(13)                                              /**< (XDMAC_GIS) XDMAC Channel 13 Interrupt Status Bit Position */
#define XDMAC_GIS_IS13_Msk                    (_U_(0x1) << XDMAC_GIS_IS13_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 13 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS13(value)                 (XDMAC_GIS_IS13_Msk & ((value) << XDMAC_GIS_IS13_Pos))
#define XDMAC_GIS_IS14_Pos                    _U_(14)                                              /**< (XDMAC_GIS) XDMAC Channel 14 Interrupt Status Bit Position */
#define XDMAC_GIS_IS14_Msk                    (_U_(0x1) << XDMAC_GIS_IS14_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 14 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS14(value)                 (XDMAC_GIS_IS14_Msk & ((value) << XDMAC_GIS_IS14_Pos))
#define XDMAC_GIS_IS15_Pos                    _U_(15)                                              /**< (XDMAC_GIS) XDMAC Channel 15 Interrupt Status Bit Position */
#define XDMAC_GIS_IS15_Msk                    (_U_(0x1) << XDMAC_GIS_IS15_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 15 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS15(value)                 (XDMAC_GIS_IS15_Msk & ((value) << XDMAC_GIS_IS15_Pos))
#define XDMAC_GIS_IS16_Pos                    _U_(16)                                              /**< (XDMAC_GIS) XDMAC Channel 16 Interrupt Status Bit Position */
#define XDMAC_GIS_IS16_Msk                    (_U_(0x1) << XDMAC_GIS_IS16_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 16 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS16(value)                 (XDMAC_GIS_IS16_Msk & ((value) << XDMAC_GIS_IS16_Pos))
#define XDMAC_GIS_IS17_Pos                    _U_(17)                                              /**< (XDMAC_GIS) XDMAC Channel 17 Interrupt Status Bit Position */
#define XDMAC_GIS_IS17_Msk                    (_U_(0x1) << XDMAC_GIS_IS17_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 17 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS17(value)                 (XDMAC_GIS_IS17_Msk & ((value) << XDMAC_GIS_IS17_Pos))
#define XDMAC_GIS_IS18_Pos                    _U_(18)                                              /**< (XDMAC_GIS) XDMAC Channel 18 Interrupt Status Bit Position */
#define XDMAC_GIS_IS18_Msk                    (_U_(0x1) << XDMAC_GIS_IS18_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 18 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS18(value)                 (XDMAC_GIS_IS18_Msk & ((value) << XDMAC_GIS_IS18_Pos))
#define XDMAC_GIS_IS19_Pos                    _U_(19)                                              /**< (XDMAC_GIS) XDMAC Channel 19 Interrupt Status Bit Position */
#define XDMAC_GIS_IS19_Msk                    (_U_(0x1) << XDMAC_GIS_IS19_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 19 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS19(value)                 (XDMAC_GIS_IS19_Msk & ((value) << XDMAC_GIS_IS19_Pos))
#define XDMAC_GIS_IS20_Pos                    _U_(20)                                              /**< (XDMAC_GIS) XDMAC Channel 20 Interrupt Status Bit Position */
#define XDMAC_GIS_IS20_Msk                    (_U_(0x1) << XDMAC_GIS_IS20_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 20 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS20(value)                 (XDMAC_GIS_IS20_Msk & ((value) << XDMAC_GIS_IS20_Pos))
#define XDMAC_GIS_IS21_Pos                    _U_(21)                                              /**< (XDMAC_GIS) XDMAC Channel 21 Interrupt Status Bit Position */
#define XDMAC_GIS_IS21_Msk                    (_U_(0x1) << XDMAC_GIS_IS21_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 21 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS21(value)                 (XDMAC_GIS_IS21_Msk & ((value) << XDMAC_GIS_IS21_Pos))
#define XDMAC_GIS_IS22_Pos                    _U_(22)                                              /**< (XDMAC_GIS) XDMAC Channel 22 Interrupt Status Bit Position */
#define XDMAC_GIS_IS22_Msk                    (_U_(0x1) << XDMAC_GIS_IS22_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 22 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS22(value)                 (XDMAC_GIS_IS22_Msk & ((value) << XDMAC_GIS_IS22_Pos))
#define XDMAC_GIS_IS23_Pos                    _U_(23)                                              /**< (XDMAC_GIS) XDMAC Channel 23 Interrupt Status Bit Position */
#define XDMAC_GIS_IS23_Msk                    (_U_(0x1) << XDMAC_GIS_IS23_Pos)                     /**< (XDMAC_GIS) XDMAC Channel 23 Interrupt Status Bit Mask */
#define XDMAC_GIS_IS23(value)                 (XDMAC_GIS_IS23_Msk & ((value) << XDMAC_GIS_IS23_Pos))
#define XDMAC_GIS_Msk                         _U_(0x00FFFFFF)                                      /**< (XDMAC_GIS) Register Mask  */

#define XDMAC_GIS_IS_Pos                      _U_(0)                                               /**< (XDMAC_GIS Position) XDMAC Channel 23 Interrupt Status Bit */
#define XDMAC_GIS_IS_Msk                      (_U_(0xFFFFFF) << XDMAC_GIS_IS_Pos)                  /**< (XDMAC_GIS Mask) IS */
#define XDMAC_GIS_IS(value)                   (XDMAC_GIS_IS_Msk & ((value) << XDMAC_GIS_IS_Pos))   

/* -------- XDMAC_GE : (XDMAC Offset: 0x1C) ( /W 32) Global Channel Enable Register -------- */
#define XDMAC_GE_EN0_Pos                      _U_(0)                                               /**< (XDMAC_GE) XDMAC Channel 0 Enable Bit Position */
#define XDMAC_GE_EN0_Msk                      (_U_(0x1) << XDMAC_GE_EN0_Pos)                       /**< (XDMAC_GE) XDMAC Channel 0 Enable Bit Mask */
#define XDMAC_GE_EN0(value)                   (XDMAC_GE_EN0_Msk & ((value) << XDMAC_GE_EN0_Pos))  
#define XDMAC_GE_EN1_Pos                      _U_(1)                                               /**< (XDMAC_GE) XDMAC Channel 1 Enable Bit Position */
#define XDMAC_GE_EN1_Msk                      (_U_(0x1) << XDMAC_GE_EN1_Pos)                       /**< (XDMAC_GE) XDMAC Channel 1 Enable Bit Mask */
#define XDMAC_GE_EN1(value)                   (XDMAC_GE_EN1_Msk & ((value) << XDMAC_GE_EN1_Pos))  
#define XDMAC_GE_EN2_Pos                      _U_(2)                                               /**< (XDMAC_GE) XDMAC Channel 2 Enable Bit Position */
#define XDMAC_GE_EN2_Msk                      (_U_(0x1) << XDMAC_GE_EN2_Pos)                       /**< (XDMAC_GE) XDMAC Channel 2 Enable Bit Mask */
#define XDMAC_GE_EN2(value)                   (XDMAC_GE_EN2_Msk & ((value) << XDMAC_GE_EN2_Pos))  
#define XDMAC_GE_EN3_Pos                      _U_(3)                                               /**< (XDMAC_GE) XDMAC Channel 3 Enable Bit Position */
#define XDMAC_GE_EN3_Msk                      (_U_(0x1) << XDMAC_GE_EN3_Pos)                       /**< (XDMAC_GE) XDMAC Channel 3 Enable Bit Mask */
#define XDMAC_GE_EN3(value)                   (XDMAC_GE_EN3_Msk & ((value) << XDMAC_GE_EN3_Pos))  
#define XDMAC_GE_EN4_Pos                      _U_(4)                                               /**< (XDMAC_GE) XDMAC Channel 4 Enable Bit Position */
#define XDMAC_GE_EN4_Msk                      (_U_(0x1) << XDMAC_GE_EN4_Pos)                       /**< (XDMAC_GE) XDMAC Channel 4 Enable Bit Mask */
#define XDMAC_GE_EN4(value)                   (XDMAC_GE_EN4_Msk & ((value) << XDMAC_GE_EN4_Pos))  
#define XDMAC_GE_EN5_Pos                      _U_(5)                                               /**< (XDMAC_GE) XDMAC Channel 5 Enable Bit Position */
#define XDMAC_GE_EN5_Msk                      (_U_(0x1) << XDMAC_GE_EN5_Pos)                       /**< (XDMAC_GE) XDMAC Channel 5 Enable Bit Mask */
#define XDMAC_GE_EN5(value)                   (XDMAC_GE_EN5_Msk & ((value) << XDMAC_GE_EN5_Pos))  
#define XDMAC_GE_EN6_Pos                      _U_(6)                                               /**< (XDMAC_GE) XDMAC Channel 6 Enable Bit Position */
#define XDMAC_GE_EN6_Msk                      (_U_(0x1) << XDMAC_GE_EN6_Pos)                       /**< (XDMAC_GE) XDMAC Channel 6 Enable Bit Mask */
#define XDMAC_GE_EN6(value)                   (XDMAC_GE_EN6_Msk & ((value) << XDMAC_GE_EN6_Pos))  
#define XDMAC_GE_EN7_Pos                      _U_(7)                                               /**< (XDMAC_GE) XDMAC Channel 7 Enable Bit Position */
#define XDMAC_GE_EN7_Msk                      (_U_(0x1) << XDMAC_GE_EN7_Pos)                       /**< (XDMAC_GE) XDMAC Channel 7 Enable Bit Mask */
#define XDMAC_GE_EN7(value)                   (XDMAC_GE_EN7_Msk & ((value) << XDMAC_GE_EN7_Pos))  
#define XDMAC_GE_EN8_Pos                      _U_(8)                                               /**< (XDMAC_GE) XDMAC Channel 8 Enable Bit Position */
#define XDMAC_GE_EN8_Msk                      (_U_(0x1) << XDMAC_GE_EN8_Pos)                       /**< (XDMAC_GE) XDMAC Channel 8 Enable Bit Mask */
#define XDMAC_GE_EN8(value)                   (XDMAC_GE_EN8_Msk & ((value) << XDMAC_GE_EN8_Pos))  
#define XDMAC_GE_EN9_Pos                      _U_(9)                                               /**< (XDMAC_GE) XDMAC Channel 9 Enable Bit Position */
#define XDMAC_GE_EN9_Msk                      (_U_(0x1) << XDMAC_GE_EN9_Pos)                       /**< (XDMAC_GE) XDMAC Channel 9 Enable Bit Mask */
#define XDMAC_GE_EN9(value)                   (XDMAC_GE_EN9_Msk & ((value) << XDMAC_GE_EN9_Pos))  
#define XDMAC_GE_EN10_Pos                     _U_(10)                                              /**< (XDMAC_GE) XDMAC Channel 10 Enable Bit Position */
#define XDMAC_GE_EN10_Msk                     (_U_(0x1) << XDMAC_GE_EN10_Pos)                      /**< (XDMAC_GE) XDMAC Channel 10 Enable Bit Mask */
#define XDMAC_GE_EN10(value)                  (XDMAC_GE_EN10_Msk & ((value) << XDMAC_GE_EN10_Pos))
#define XDMAC_GE_EN11_Pos                     _U_(11)                                              /**< (XDMAC_GE) XDMAC Channel 11 Enable Bit Position */
#define XDMAC_GE_EN11_Msk                     (_U_(0x1) << XDMAC_GE_EN11_Pos)                      /**< (XDMAC_GE) XDMAC Channel 11 Enable Bit Mask */
#define XDMAC_GE_EN11(value)                  (XDMAC_GE_EN11_Msk & ((value) << XDMAC_GE_EN11_Pos))
#define XDMAC_GE_EN12_Pos                     _U_(12)                                              /**< (XDMAC_GE) XDMAC Channel 12 Enable Bit Position */
#define XDMAC_GE_EN12_Msk                     (_U_(0x1) << XDMAC_GE_EN12_Pos)                      /**< (XDMAC_GE) XDMAC Channel 12 Enable Bit Mask */
#define XDMAC_GE_EN12(value)                  (XDMAC_GE_EN12_Msk & ((value) << XDMAC_GE_EN12_Pos))
#define XDMAC_GE_EN13_Pos                     _U_(13)                                              /**< (XDMAC_GE) XDMAC Channel 13 Enable Bit Position */
#define XDMAC_GE_EN13_Msk                     (_U_(0x1) << XDMAC_GE_EN13_Pos)                      /**< (XDMAC_GE) XDMAC Channel 13 Enable Bit Mask */
#define XDMAC_GE_EN13(value)                  (XDMAC_GE_EN13_Msk & ((value) << XDMAC_GE_EN13_Pos))
#define XDMAC_GE_EN14_Pos                     _U_(14)                                              /**< (XDMAC_GE) XDMAC Channel 14 Enable Bit Position */
#define XDMAC_GE_EN14_Msk                     (_U_(0x1) << XDMAC_GE_EN14_Pos)                      /**< (XDMAC_GE) XDMAC Channel 14 Enable Bit Mask */
#define XDMAC_GE_EN14(value)                  (XDMAC_GE_EN14_Msk & ((value) << XDMAC_GE_EN14_Pos))
#define XDMAC_GE_EN15_Pos                     _U_(15)                                              /**< (XDMAC_GE) XDMAC Channel 15 Enable Bit Position */
#define XDMAC_GE_EN15_Msk                     (_U_(0x1) << XDMAC_GE_EN15_Pos)                      /**< (XDMAC_GE) XDMAC Channel 15 Enable Bit Mask */
#define XDMAC_GE_EN15(value)                  (XDMAC_GE_EN15_Msk & ((value) << XDMAC_GE_EN15_Pos))
#define XDMAC_GE_EN16_Pos                     _U_(16)                                              /**< (XDMAC_GE) XDMAC Channel 16 Enable Bit Position */
#define XDMAC_GE_EN16_Msk                     (_U_(0x1) << XDMAC_GE_EN16_Pos)                      /**< (XDMAC_GE) XDMAC Channel 16 Enable Bit Mask */
#define XDMAC_GE_EN16(value)                  (XDMAC_GE_EN16_Msk & ((value) << XDMAC_GE_EN16_Pos))
#define XDMAC_GE_EN17_Pos                     _U_(17)                                              /**< (XDMAC_GE) XDMAC Channel 17 Enable Bit Position */
#define XDMAC_GE_EN17_Msk                     (_U_(0x1) << XDMAC_GE_EN17_Pos)                      /**< (XDMAC_GE) XDMAC Channel 17 Enable Bit Mask */
#define XDMAC_GE_EN17(value)                  (XDMAC_GE_EN17_Msk & ((value) << XDMAC_GE_EN17_Pos))
#define XDMAC_GE_EN18_Pos                     _U_(18)                                              /**< (XDMAC_GE) XDMAC Channel 18 Enable Bit Position */
#define XDMAC_GE_EN18_Msk                     (_U_(0x1) << XDMAC_GE_EN18_Pos)                      /**< (XDMAC_GE) XDMAC Channel 18 Enable Bit Mask */
#define XDMAC_GE_EN18(value)                  (XDMAC_GE_EN18_Msk & ((value) << XDMAC_GE_EN18_Pos))
#define XDMAC_GE_EN19_Pos                     _U_(19)                                              /**< (XDMAC_GE) XDMAC Channel 19 Enable Bit Position */
#define XDMAC_GE_EN19_Msk                     (_U_(0x1) << XDMAC_GE_EN19_Pos)                      /**< (XDMAC_GE) XDMAC Channel 19 Enable Bit Mask */
#define XDMAC_GE_EN19(value)                  (XDMAC_GE_EN19_Msk & ((value) << XDMAC_GE_EN19_Pos))
#define XDMAC_GE_EN20_Pos                     _U_(20)                                              /**< (XDMAC_GE) XDMAC Channel 20 Enable Bit Position */
#define XDMAC_GE_EN20_Msk                     (_U_(0x1) << XDMAC_GE_EN20_Pos)                      /**< (XDMAC_GE) XDMAC Channel 20 Enable Bit Mask */
#define XDMAC_GE_EN20(value)                  (XDMAC_GE_EN20_Msk & ((value) << XDMAC_GE_EN20_Pos))
#define XDMAC_GE_EN21_Pos                     _U_(21)                                              /**< (XDMAC_GE) XDMAC Channel 21 Enable Bit Position */
#define XDMAC_GE_EN21_Msk                     (_U_(0x1) << XDMAC_GE_EN21_Pos)                      /**< (XDMAC_GE) XDMAC Channel 21 Enable Bit Mask */
#define XDMAC_GE_EN21(value)                  (XDMAC_GE_EN21_Msk & ((value) << XDMAC_GE_EN21_Pos))
#define XDMAC_GE_EN22_Pos                     _U_(22)                                              /**< (XDMAC_GE) XDMAC Channel 22 Enable Bit Position */
#define XDMAC_GE_EN22_Msk                     (_U_(0x1) << XDMAC_GE_EN22_Pos)                      /**< (XDMAC_GE) XDMAC Channel 22 Enable Bit Mask */
#define XDMAC_GE_EN22(value)                  (XDMAC_GE_EN22_Msk & ((value) << XDMAC_GE_EN22_Pos))
#define XDMAC_GE_EN23_Pos                     _U_(23)                                              /**< (XDMAC_GE) XDMAC Channel 23 Enable Bit Position */
#define XDMAC_GE_EN23_Msk                     (_U_(0x1) << XDMAC_GE_EN23_Pos)                      /**< (XDMAC_GE) XDMAC Channel 23 Enable Bit Mask */
#define XDMAC_GE_EN23(value)                  (XDMAC_GE_EN23_Msk & ((value) << XDMAC_GE_EN23_Pos))
#define XDMAC_GE_Msk                          _U_(0x00FFFFFF)                                      /**< (XDMAC_GE) Register Mask  */

#define XDMAC_GE_EN_Pos                       _U_(0)                                               /**< (XDMAC_GE Position) XDMAC Channel 23 Enable Bit */
#define XDMAC_GE_EN_Msk                       (_U_(0xFFFFFF) << XDMAC_GE_EN_Pos)                   /**< (XDMAC_GE Mask) EN */
#define XDMAC_GE_EN(value)                    (XDMAC_GE_EN_Msk & ((value) << XDMAC_GE_EN_Pos))     

/* -------- XDMAC_GD : (XDMAC Offset: 0x20) ( /W 32) Global Channel Disable Register -------- */
#define XDMAC_GD_DI0_Pos                      _U_(0)                                               /**< (XDMAC_GD) XDMAC Channel 0 Disable Bit Position */
#define XDMAC_GD_DI0_Msk                      (_U_(0x1) << XDMAC_GD_DI0_Pos)                       /**< (XDMAC_GD) XDMAC Channel 0 Disable Bit Mask */
#define XDMAC_GD_DI0(value)                   (XDMAC_GD_DI0_Msk & ((value) << XDMAC_GD_DI0_Pos))  
#define XDMAC_GD_DI1_Pos                      _U_(1)                                               /**< (XDMAC_GD) XDMAC Channel 1 Disable Bit Position */
#define XDMAC_GD_DI1_Msk                      (_U_(0x1) << XDMAC_GD_DI1_Pos)                       /**< (XDMAC_GD) XDMAC Channel 1 Disable Bit Mask */
#define XDMAC_GD_DI1(value)                   (XDMAC_GD_DI1_Msk & ((value) << XDMAC_GD_DI1_Pos))  
#define XDMAC_GD_DI2_Pos                      _U_(2)                                               /**< (XDMAC_GD) XDMAC Channel 2 Disable Bit Position */
#define XDMAC_GD_DI2_Msk                      (_U_(0x1) << XDMAC_GD_DI2_Pos)                       /**< (XDMAC_GD) XDMAC Channel 2 Disable Bit Mask */
#define XDMAC_GD_DI2(value)                   (XDMAC_GD_DI2_Msk & ((value) << XDMAC_GD_DI2_Pos))  
#define XDMAC_GD_DI3_Pos                      _U_(3)                                               /**< (XDMAC_GD) XDMAC Channel 3 Disable Bit Position */
#define XDMAC_GD_DI3_Msk                      (_U_(0x1) << XDMAC_GD_DI3_Pos)                       /**< (XDMAC_GD) XDMAC Channel 3 Disable Bit Mask */
#define XDMAC_GD_DI3(value)                   (XDMAC_GD_DI3_Msk & ((value) << XDMAC_GD_DI3_Pos))  
#define XDMAC_GD_DI4_Pos                      _U_(4)                                               /**< (XDMAC_GD) XDMAC Channel 4 Disable Bit Position */
#define XDMAC_GD_DI4_Msk                      (_U_(0x1) << XDMAC_GD_DI4_Pos)                       /**< (XDMAC_GD) XDMAC Channel 4 Disable Bit Mask */
#define XDMAC_GD_DI4(value)                   (XDMAC_GD_DI4_Msk & ((value) << XDMAC_GD_DI4_Pos))  
#define XDMAC_GD_DI5_Pos                      _U_(5)                                               /**< (XDMAC_GD) XDMAC Channel 5 Disable Bit Position */
#define XDMAC_GD_DI5_Msk                      (_U_(0x1) << XDMAC_GD_DI5_Pos)                       /**< (XDMAC_GD) XDMAC Channel 5 Disable Bit Mask */
#define XDMAC_GD_DI5(value)                   (XDMAC_GD_DI5_Msk & ((value) << XDMAC_GD_DI5_Pos))  
#define XDMAC_GD_DI6_Pos                      _U_(6)                                               /**< (XDMAC_GD) XDMAC Channel 6 Disable Bit Position */
#define XDMAC_GD_DI6_Msk                      (_U_(0x1) << XDMAC_GD_DI6_Pos)                       /**< (XDMAC_GD) XDMAC Channel 6 Disable Bit Mask */
#define XDMAC_GD_DI6(value)                   (XDMAC_GD_DI6_Msk & ((value) << XDMAC_GD_DI6_Pos))  
#define XDMAC_GD_DI7_Pos                      _U_(7)                                               /**< (XDMAC_GD) XDMAC Channel 7 Disable Bit Position */
#define XDMAC_GD_DI7_Msk                      (_U_(0x1) << XDMAC_GD_DI7_Pos)                       /**< (XDMAC_GD) XDMAC Channel 7 Disable Bit Mask */
#define XDMAC_GD_DI7(value)                   (XDMAC_GD_DI7_Msk & ((value) << XDMAC_GD_DI7_Pos))  
#define XDMAC_GD_DI8_Pos                      _U_(8)                                               /**< (XDMAC_GD) XDMAC Channel 8 Disable Bit Position */
#define XDMAC_GD_DI8_Msk                      (_U_(0x1) << XDMAC_GD_DI8_Pos)                       /**< (XDMAC_GD) XDMAC Channel 8 Disable Bit Mask */
#define XDMAC_GD_DI8(value)                   (XDMAC_GD_DI8_Msk & ((value) << XDMAC_GD_DI8_Pos))  
#define XDMAC_GD_DI9_Pos                      _U_(9)                                               /**< (XDMAC_GD) XDMAC Channel 9 Disable Bit Position */
#define XDMAC_GD_DI9_Msk                      (_U_(0x1) << XDMAC_GD_DI9_Pos)                       /**< (XDMAC_GD) XDMAC Channel 9 Disable Bit Mask */
#define XDMAC_GD_DI9(value)                   (XDMAC_GD_DI9_Msk & ((value) << XDMAC_GD_DI9_Pos))  
#define XDMAC_GD_DI10_Pos                     _U_(10)                                              /**< (XDMAC_GD) XDMAC Channel 10 Disable Bit Position */
#define XDMAC_GD_DI10_Msk                     (_U_(0x1) << XDMAC_GD_DI10_Pos)                      /**< (XDMAC_GD) XDMAC Channel 10 Disable Bit Mask */
#define XDMAC_GD_DI10(value)                  (XDMAC_GD_DI10_Msk & ((value) << XDMAC_GD_DI10_Pos))
#define XDMAC_GD_DI11_Pos                     _U_(11)                                              /**< (XDMAC_GD) XDMAC Channel 11 Disable Bit Position */
#define XDMAC_GD_DI11_Msk                     (_U_(0x1) << XDMAC_GD_DI11_Pos)                      /**< (XDMAC_GD) XDMAC Channel 11 Disable Bit Mask */
#define XDMAC_GD_DI11(value)                  (XDMAC_GD_DI11_Msk & ((value) << XDMAC_GD_DI11_Pos))
#define XDMAC_GD_DI12_Pos                     _U_(12)                                              /**< (XDMAC_GD) XDMAC Channel 12 Disable Bit Position */
#define XDMAC_GD_DI12_Msk                     (_U_(0x1) << XDMAC_GD_DI12_Pos)                      /**< (XDMAC_GD) XDMAC Channel 12 Disable Bit Mask */
#define XDMAC_GD_DI12(value)                  (XDMAC_GD_DI12_Msk & ((value) << XDMAC_GD_DI12_Pos))
#define XDMAC_GD_DI13_Pos                     _U_(13)                                              /**< (XDMAC_GD) XDMAC Channel 13 Disable Bit Position */
#define XDMAC_GD_DI13_Msk                     (_U_(0x1) << XDMAC_GD_DI13_Pos)                      /**< (XDMAC_GD) XDMAC Channel 13 Disable Bit Mask */
#define XDMAC_GD_DI13(value)                  (XDMAC_GD_DI13_Msk & ((value) << XDMAC_GD_DI13_Pos))
#define XDMAC_GD_DI14_Pos                     _U_(14)                                              /**< (XDMAC_GD) XDMAC Channel 14 Disable Bit Position */
#define XDMAC_GD_DI14_Msk                     (_U_(0x1) << XDMAC_GD_DI14_Pos)                      /**< (XDMAC_GD) XDMAC Channel 14 Disable Bit Mask */
#define XDMAC_GD_DI14(value)                  (XDMAC_GD_DI14_Msk & ((value) << XDMAC_GD_DI14_Pos))
#define XDMAC_GD_DI15_Pos                     _U_(15)                                              /**< (XDMAC_GD) XDMAC Channel 15 Disable Bit Position */
#define XDMAC_GD_DI15_Msk                     (_U_(0x1) << XDMAC_GD_DI15_Pos)                      /**< (XDMAC_GD) XDMAC Channel 15 Disable Bit Mask */
#define XDMAC_GD_DI15(value)                  (XDMAC_GD_DI15_Msk & ((value) << XDMAC_GD_DI15_Pos))
#define XDMAC_GD_DI16_Pos                     _U_(16)                                              /**< (XDMAC_GD) XDMAC Channel 16 Disable Bit Position */
#define XDMAC_GD_DI16_Msk                     (_U_(0x1) << XDMAC_GD_DI16_Pos)                      /**< (XDMAC_GD) XDMAC Channel 16 Disable Bit Mask */
#define XDMAC_GD_DI16(value)                  (XDMAC_GD_DI16_Msk & ((value) << XDMAC_GD_DI16_Pos))
#define XDMAC_GD_DI17_Pos                     _U_(17)                                              /**< (XDMAC_GD) XDMAC Channel 17 Disable Bit Position */
#define XDMAC_GD_DI17_Msk                     (_U_(0x1) << XDMAC_GD_DI17_Pos)                      /**< (XDMAC_GD) XDMAC Channel 17 Disable Bit Mask */
#define XDMAC_GD_DI17(value)                  (XDMAC_GD_DI17_Msk & ((value) << XDMAC_GD_DI17_Pos))
#define XDMAC_GD_DI18_Pos                     _U_(18)                                              /**< (XDMAC_GD) XDMAC Channel 18 Disable Bit Position */
#define XDMAC_GD_DI18_Msk                     (_U_(0x1) << XDMAC_GD_DI18_Pos)                      /**< (XDMAC_GD) XDMAC Channel 18 Disable Bit Mask */
#define XDMAC_GD_DI18(value)                  (XDMAC_GD_DI18_Msk & ((value) << XDMAC_GD_DI18_Pos))
#define XDMAC_GD_DI19_Pos                     _U_(19)                                              /**< (XDMAC_GD) XDMAC Channel 19 Disable Bit Position */
#define XDMAC_GD_DI19_Msk                     (_U_(0x1) << XDMAC_GD_DI19_Pos)                      /**< (XDMAC_GD) XDMAC Channel 19 Disable Bit Mask */
#define XDMAC_GD_DI19(value)                  (XDMAC_GD_DI19_Msk & ((value) << XDMAC_GD_DI19_Pos))
#define XDMAC_GD_DI20_Pos                     _U_(20)                                              /**< (XDMAC_GD) XDMAC Channel 20 Disable Bit Position */
#define XDMAC_GD_DI20_Msk                     (_U_(0x1) << XDMAC_GD_DI20_Pos)                      /**< (XDMAC_GD) XDMAC Channel 20 Disable Bit Mask */
#define XDMAC_GD_DI20(value)                  (XDMAC_GD_DI20_Msk & ((value) << XDMAC_GD_DI20_Pos))
#define XDMAC_GD_DI21_Pos                     _U_(21)                                              /**< (XDMAC_GD) XDMAC Channel 21 Disable Bit Position */
#define XDMAC_GD_DI21_Msk                     (_U_(0x1) << XDMAC_GD_DI21_Pos)                      /**< (XDMAC_GD) XDMAC Channel 21 Disable Bit Mask */
#define XDMAC_GD_DI21(value)                  (XDMAC_GD_DI21_Msk & ((value) << XDMAC_GD_DI21_Pos))
#define XDMAC_GD_DI22_Pos                     _U_(22)                                              /**< (XDMAC_GD) XDMAC Channel 22 Disable Bit Position */
#define XDMAC_GD_DI22_Msk                     (_U_(0x1) << XDMAC_GD_DI22_Pos)                      /**< (XDMAC_GD) XDMAC Channel 22 Disable Bit Mask */
#define XDMAC_GD_DI22(value)                  (XDMAC_GD_DI22_Msk & ((value) << XDMAC_GD_DI22_Pos))
#define XDMAC_GD_DI23_Pos                     _U_(23)                                              /**< (XDMAC_GD) XDMAC Channel 23 Disable Bit Position */
#define XDMAC_GD_DI23_Msk                     (_U_(0x1) << XDMAC_GD_DI23_Pos)                      /**< (XDMAC_GD) XDMAC Channel 23 Disable Bit Mask */
#define XDMAC_GD_DI23(value)                  (XDMAC_GD_DI23_Msk & ((value) << XDMAC_GD_DI23_Pos))
#define XDMAC_GD_Msk                          _U_(0x00FFFFFF)                                      /**< (XDMAC_GD) Register Mask  */

#define XDMAC_GD_DI_Pos                       _U_(0)                                               /**< (XDMAC_GD Position) XDMAC Channel 23 Disable Bit */
#define XDMAC_GD_DI_Msk                       (_U_(0xFFFFFF) << XDMAC_GD_DI_Pos)                   /**< (XDMAC_GD Mask) DI */
#define XDMAC_GD_DI(value)                    (XDMAC_GD_DI_Msk & ((value) << XDMAC_GD_DI_Pos))     

/* -------- XDMAC_GS : (XDMAC Offset: 0x24) ( R/ 32) Global Channel Status Register -------- */
#define XDMAC_GS_ST0_Pos                      _U_(0)                                               /**< (XDMAC_GS) XDMAC Channel 0 Status Bit Position */
#define XDMAC_GS_ST0_Msk                      (_U_(0x1) << XDMAC_GS_ST0_Pos)                       /**< (XDMAC_GS) XDMAC Channel 0 Status Bit Mask */
#define XDMAC_GS_ST0(value)                   (XDMAC_GS_ST0_Msk & ((value) << XDMAC_GS_ST0_Pos))  
#define XDMAC_GS_ST1_Pos                      _U_(1)                                               /**< (XDMAC_GS) XDMAC Channel 1 Status Bit Position */
#define XDMAC_GS_ST1_Msk                      (_U_(0x1) << XDMAC_GS_ST1_Pos)                       /**< (XDMAC_GS) XDMAC Channel 1 Status Bit Mask */
#define XDMAC_GS_ST1(value)                   (XDMAC_GS_ST1_Msk & ((value) << XDMAC_GS_ST1_Pos))  
#define XDMAC_GS_ST2_Pos                      _U_(2)                                               /**< (XDMAC_GS) XDMAC Channel 2 Status Bit Position */
#define XDMAC_GS_ST2_Msk                      (_U_(0x1) << XDMAC_GS_ST2_Pos)                       /**< (XDMAC_GS) XDMAC Channel 2 Status Bit Mask */
#define XDMAC_GS_ST2(value)                   (XDMAC_GS_ST2_Msk & ((value) << XDMAC_GS_ST2_Pos))  
#define XDMAC_GS_ST3_Pos                      _U_(3)                                               /**< (XDMAC_GS) XDMAC Channel 3 Status Bit Position */
#define XDMAC_GS_ST3_Msk                      (_U_(0x1) << XDMAC_GS_ST3_Pos)                       /**< (XDMAC_GS) XDMAC Channel 3 Status Bit Mask */
#define XDMAC_GS_ST3(value)                   (XDMAC_GS_ST3_Msk & ((value) << XDMAC_GS_ST3_Pos))  
#define XDMAC_GS_ST4_Pos                      _U_(4)                                               /**< (XDMAC_GS) XDMAC Channel 4 Status Bit Position */
#define XDMAC_GS_ST4_Msk                      (_U_(0x1) << XDMAC_GS_ST4_Pos)                       /**< (XDMAC_GS) XDMAC Channel 4 Status Bit Mask */
#define XDMAC_GS_ST4(value)                   (XDMAC_GS_ST4_Msk & ((value) << XDMAC_GS_ST4_Pos))  
#define XDMAC_GS_ST5_Pos                      _U_(5)                                               /**< (XDMAC_GS) XDMAC Channel 5 Status Bit Position */
#define XDMAC_GS_ST5_Msk                      (_U_(0x1) << XDMAC_GS_ST5_Pos)                       /**< (XDMAC_GS) XDMAC Channel 5 Status Bit Mask */
#define XDMAC_GS_ST5(value)                   (XDMAC_GS_ST5_Msk & ((value) << XDMAC_GS_ST5_Pos))  
#define XDMAC_GS_ST6_Pos                      _U_(6)                                               /**< (XDMAC_GS) XDMAC Channel 6 Status Bit Position */
#define XDMAC_GS_ST6_Msk                      (_U_(0x1) << XDMAC_GS_ST6_Pos)                       /**< (XDMAC_GS) XDMAC Channel 6 Status Bit Mask */
#define XDMAC_GS_ST6(value)                   (XDMAC_GS_ST6_Msk & ((value) << XDMAC_GS_ST6_Pos))  
#define XDMAC_GS_ST7_Pos                      _U_(7)                                               /**< (XDMAC_GS) XDMAC Channel 7 Status Bit Position */
#define XDMAC_GS_ST7_Msk                      (_U_(0x1) << XDMAC_GS_ST7_Pos)                       /**< (XDMAC_GS) XDMAC Channel 7 Status Bit Mask */
#define XDMAC_GS_ST7(value)                   (XDMAC_GS_ST7_Msk & ((value) << XDMAC_GS_ST7_Pos))  
#define XDMAC_GS_ST8_Pos                      _U_(8)                                               /**< (XDMAC_GS) XDMAC Channel 8 Status Bit Position */
#define XDMAC_GS_ST8_Msk                      (_U_(0x1) << XDMAC_GS_ST8_Pos)                       /**< (XDMAC_GS) XDMAC Channel 8 Status Bit Mask */
#define XDMAC_GS_ST8(value)                   (XDMAC_GS_ST8_Msk & ((value) << XDMAC_GS_ST8_Pos))  
#define XDMAC_GS_ST9_Pos                      _U_(9)                                               /**< (XDMAC_GS) XDMAC Channel 9 Status Bit Position */
#define XDMAC_GS_ST9_Msk                      (_U_(0x1) << XDMAC_GS_ST9_Pos)                       /**< (XDMAC_GS) XDMAC Channel 9 Status Bit Mask */
#define XDMAC_GS_ST9(value)                   (XDMAC_GS_ST9_Msk & ((value) << XDMAC_GS_ST9_Pos))  
#define XDMAC_GS_ST10_Pos                     _U_(10)                                              /**< (XDMAC_GS) XDMAC Channel 10 Status Bit Position */
#define XDMAC_GS_ST10_Msk                     (_U_(0x1) << XDMAC_GS_ST10_Pos)                      /**< (XDMAC_GS) XDMAC Channel 10 Status Bit Mask */
#define XDMAC_GS_ST10(value)                  (XDMAC_GS_ST10_Msk & ((value) << XDMAC_GS_ST10_Pos))
#define XDMAC_GS_ST11_Pos                     _U_(11)                                              /**< (XDMAC_GS) XDMAC Channel 11 Status Bit Position */
#define XDMAC_GS_ST11_Msk                     (_U_(0x1) << XDMAC_GS_ST11_Pos)                      /**< (XDMAC_GS) XDMAC Channel 11 Status Bit Mask */
#define XDMAC_GS_ST11(value)                  (XDMAC_GS_ST11_Msk & ((value) << XDMAC_GS_ST11_Pos))
#define XDMAC_GS_ST12_Pos                     _U_(12)                                              /**< (XDMAC_GS) XDMAC Channel 12 Status Bit Position */
#define XDMAC_GS_ST12_Msk                     (_U_(0x1) << XDMAC_GS_ST12_Pos)                      /**< (XDMAC_GS) XDMAC Channel 12 Status Bit Mask */
#define XDMAC_GS_ST12(value)                  (XDMAC_GS_ST12_Msk & ((value) << XDMAC_GS_ST12_Pos))
#define XDMAC_GS_ST13_Pos                     _U_(13)                                              /**< (XDMAC_GS) XDMAC Channel 13 Status Bit Position */
#define XDMAC_GS_ST13_Msk                     (_U_(0x1) << XDMAC_GS_ST13_Pos)                      /**< (XDMAC_GS) XDMAC Channel 13 Status Bit Mask */
#define XDMAC_GS_ST13(value)                  (XDMAC_GS_ST13_Msk & ((value) << XDMAC_GS_ST13_Pos))
#define XDMAC_GS_ST14_Pos                     _U_(14)                                              /**< (XDMAC_GS) XDMAC Channel 14 Status Bit Position */
#define XDMAC_GS_ST14_Msk                     (_U_(0x1) << XDMAC_GS_ST14_Pos)                      /**< (XDMAC_GS) XDMAC Channel 14 Status Bit Mask */
#define XDMAC_GS_ST14(value)                  (XDMAC_GS_ST14_Msk & ((value) << XDMAC_GS_ST14_Pos))
#define XDMAC_GS_ST15_Pos                     _U_(15)                                              /**< (XDMAC_GS) XDMAC Channel 15 Status Bit Position */
#define XDMAC_GS_ST15_Msk                     (_U_(0x1) << XDMAC_GS_ST15_Pos)                      /**< (XDMAC_GS) XDMAC Channel 15 Status Bit Mask */
#define XDMAC_GS_ST15(value)                  (XDMAC_GS_ST15_Msk & ((value) << XDMAC_GS_ST15_Pos))
#define XDMAC_GS_ST16_Pos                     _U_(16)                                              /**< (XDMAC_GS) XDMAC Channel 16 Status Bit Position */
#define XDMAC_GS_ST16_Msk                     (_U_(0x1) << XDMAC_GS_ST16_Pos)                      /**< (XDMAC_GS) XDMAC Channel 16 Status Bit Mask */
#define XDMAC_GS_ST16(value)                  (XDMAC_GS_ST16_Msk & ((value) << XDMAC_GS_ST16_Pos))
#define XDMAC_GS_ST17_Pos                     _U_(17)                                              /**< (XDMAC_GS) XDMAC Channel 17 Status Bit Position */
#define XDMAC_GS_ST17_Msk                     (_U_(0x1) << XDMAC_GS_ST17_Pos)                      /**< (XDMAC_GS) XDMAC Channel 17 Status Bit Mask */
#define XDMAC_GS_ST17(value)                  (XDMAC_GS_ST17_Msk & ((value) << XDMAC_GS_ST17_Pos))
#define XDMAC_GS_ST18_Pos                     _U_(18)                                              /**< (XDMAC_GS) XDMAC Channel 18 Status Bit Position */
#define XDMAC_GS_ST18_Msk                     (_U_(0x1) << XDMAC_GS_ST18_Pos)                      /**< (XDMAC_GS) XDMAC Channel 18 Status Bit Mask */
#define XDMAC_GS_ST18(value)                  (XDMAC_GS_ST18_Msk & ((value) << XDMAC_GS_ST18_Pos))
#define XDMAC_GS_ST19_Pos                     _U_(19)                                              /**< (XDMAC_GS) XDMAC Channel 19 Status Bit Position */
#define XDMAC_GS_ST19_Msk                     (_U_(0x1) << XDMAC_GS_ST19_Pos)                      /**< (XDMAC_GS) XDMAC Channel 19 Status Bit Mask */
#define XDMAC_GS_ST19(value)                  (XDMAC_GS_ST19_Msk & ((value) << XDMAC_GS_ST19_Pos))
#define XDMAC_GS_ST20_Pos                     _U_(20)                                              /**< (XDMAC_GS) XDMAC Channel 20 Status Bit Position */
#define XDMAC_GS_ST20_Msk                     (_U_(0x1) << XDMAC_GS_ST20_Pos)                      /**< (XDMAC_GS) XDMAC Channel 20 Status Bit Mask */
#define XDMAC_GS_ST20(value)                  (XDMAC_GS_ST20_Msk & ((value) << XDMAC_GS_ST20_Pos))
#define XDMAC_GS_ST21_Pos                     _U_(21)                                              /**< (XDMAC_GS) XDMAC Channel 21 Status Bit Position */
#define XDMAC_GS_ST21_Msk                     (_U_(0x1) << XDMAC_GS_ST21_Pos)                      /**< (XDMAC_GS) XDMAC Channel 21 Status Bit Mask */
#define XDMAC_GS_ST21(value)                  (XDMAC_GS_ST21_Msk & ((value) << XDMAC_GS_ST21_Pos))
#define XDMAC_GS_ST22_Pos                     _U_(22)                                              /**< (XDMAC_GS) XDMAC Channel 22 Status Bit Position */
#define XDMAC_GS_ST22_Msk                     (_U_(0x1) << XDMAC_GS_ST22_Pos)                      /**< (XDMAC_GS) XDMAC Channel 22 Status Bit Mask */
#define XDMAC_GS_ST22(value)                  (XDMAC_GS_ST22_Msk & ((value) << XDMAC_GS_ST22_Pos))
#define XDMAC_GS_ST23_Pos                     _U_(23)                                              /**< (XDMAC_GS) XDMAC Channel 23 Status Bit Position */
#define XDMAC_GS_ST23_Msk                     (_U_(0x1) << XDMAC_GS_ST23_Pos)                      /**< (XDMAC_GS) XDMAC Channel 23 Status Bit Mask */
#define XDMAC_GS_ST23(value)                  (XDMAC_GS_ST23_Msk & ((value) << XDMAC_GS_ST23_Pos))
#define XDMAC_GS_Msk                          _U_(0x00FFFFFF)                                      /**< (XDMAC_GS) Register Mask  */

#define XDMAC_GS_ST_Pos                       _U_(0)                                               /**< (XDMAC_GS Position) XDMAC Channel 23 Status Bit */
#define XDMAC_GS_ST_Msk                       (_U_(0xFFFFFF) << XDMAC_GS_ST_Pos)                   /**< (XDMAC_GS Mask) ST */
#define XDMAC_GS_ST(value)                    (XDMAC_GS_ST_Msk & ((value) << XDMAC_GS_ST_Pos))     

/* -------- XDMAC_GRS : (XDMAC Offset: 0x28) (R/W 32) Global Channel Read Suspend Register -------- */
#define XDMAC_GRS_RS0_Pos                     _U_(0)                                               /**< (XDMAC_GRS) XDMAC Channel 0 Read Suspend Bit Position */
#define XDMAC_GRS_RS0_Msk                     (_U_(0x1) << XDMAC_GRS_RS0_Pos)                      /**< (XDMAC_GRS) XDMAC Channel 0 Read Suspend Bit Mask */
#define XDMAC_GRS_RS0(value)                  (XDMAC_GRS_RS0_Msk & ((value) << XDMAC_GRS_RS0_Pos))
#define XDMAC_GRS_RS1_Pos                     _U_(1)                                               /**< (XDMAC_GRS) XDMAC Channel 1 Read Suspend Bit Position */
#define XDMAC_GRS_RS1_Msk                     (_U_(0x1) << XDMAC_GRS_RS1_Pos)                      /**< (XDMAC_GRS) XDMAC Channel 1 Read Suspend Bit Mask */
#define XDMAC_GRS_RS1(value)                  (XDMAC_GRS_RS1_Msk & ((value) << XDMAC_GRS_RS1_Pos))
#define XDMAC_GRS_RS2_Pos                     _U_(2)                                               /**< (XDMAC_GRS) XDMAC Channel 2 Read Suspend Bit Position */
#define XDMAC_GRS_RS2_Msk                     (_U_(0x1) << XDMAC_GRS_RS2_Pos)                      /**< (XDMAC_GRS) XDMAC Channel 2 Read Suspend Bit Mask */
#define XDMAC_GRS_RS2(value)                  (XDMAC_GRS_RS2_Msk & ((value) << XDMAC_GRS_RS2_Pos))
#define XDMAC_GRS_RS3_Pos                     _U_(3)                                               /**< (XDMAC_GRS) XDMAC Channel 3 Read Suspend Bit Position */
#define XDMAC_GRS_RS3_Msk                     (_U_(0x1) << XDMAC_GRS_RS3_Pos)                      /**< (XDMAC_GRS) XDMAC Channel 3 Read Suspend Bit Mask */
#define XDMAC_GRS_RS3(value)                  (XDMAC_GRS_RS3_Msk & ((value) << XDMAC_GRS_RS3_Pos))
#define XDMAC_GRS_RS4_Pos                     _U_(4)                                               /**< (XDMAC_GRS) XDMAC Channel 4 Read Suspend Bit Position */
#define XDMAC_GRS_RS4_Msk                     (_U_(0x1) << XDMAC_GRS_RS4_Pos)                      /**< (XDMAC_GRS) XDMAC Channel 4 Read Suspend Bit Mask */
#define XDMAC_GRS_RS4(value)                  (XDMAC_GRS_RS4_Msk & ((value) << XDMAC_GRS_RS4_Pos))
#define XDMAC_GRS_RS5_Pos                     _U_(5)                                               /**< (XDMAC_GRS) XDMAC Channel 5 Read Suspend Bit Position */
#define XDMAC_GRS_RS5_Msk                     (_U_(0x1) << XDMAC_GRS_RS5_Pos)                      /**< (XDMAC_GRS) XDMAC Channel 5 Read Suspend Bit Mask */
#define XDMAC_GRS_RS5(value)                  (XDMAC_GRS_RS5_Msk & ((value) << XDMAC_GRS_RS5_Pos))
#define XDMAC_GRS_RS6_Pos                     _U_(6)                                               /**< (XDMAC_GRS) XDMAC Channel 6 Read Suspend Bit Position */
#define XDMAC_GRS_RS6_Msk                     (_U_(0x1) << XDMAC_GRS_RS6_Pos)                      /**< (XDMAC_GRS) XDMAC Channel 6 Read Suspend Bit Mask */
#define XDMAC_GRS_RS6(value)                  (XDMAC_GRS_RS6_Msk & ((value) << XDMAC_GRS_RS6_Pos))
#define XDMAC_GRS_RS7_Pos                     _U_(7)                                               /**< (XDMAC_GRS) XDMAC Channel 7 Read Suspend Bit Position */
#define XDMAC_GRS_RS7_Msk                     (_U_(0x1) << XDMAC_GRS_RS7_Pos)                      /**< (XDMAC_GRS) XDMAC Channel 7 Read Suspend Bit Mask */
#define XDMAC_GRS_RS7(value)                  (XDMAC_GRS_RS7_Msk & ((value) << XDMAC_GRS_RS7_Pos))
#define XDMAC_GRS_RS8_Pos                     _U_(8)                                               /**< (XDMAC_GRS) XDMAC Channel 8 Read Suspend Bit Position */
#define XDMAC_GRS_RS8_Msk                     (_U_(0x1) << XDMAC_GRS_RS8_Pos)                      /**< (XDMAC_GRS) XDMAC Channel 8 Read Suspend Bit Mask */
#define XDMAC_GRS_RS8(value)                  (XDMAC_GRS_RS8_Msk & ((value) << XDMAC_GRS_RS8_Pos))
#define XDMAC_GRS_RS9_Pos                     _U_(9)                                               /**< (XDMAC_GRS) XDMAC Channel 9 Read Suspend Bit Position */
#define XDMAC_GRS_RS9_Msk                     (_U_(0x1) << XDMAC_GRS_RS9_Pos)                      /**< (XDMAC_GRS) XDMAC Channel 9 Read Suspend Bit Mask */
#define XDMAC_GRS_RS9(value)                  (XDMAC_GRS_RS9_Msk & ((value) << XDMAC_GRS_RS9_Pos))
#define XDMAC_GRS_RS10_Pos                    _U_(10)                                              /**< (XDMAC_GRS) XDMAC Channel 10 Read Suspend Bit Position */
#define XDMAC_GRS_RS10_Msk                    (_U_(0x1) << XDMAC_GRS_RS10_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 10 Read Suspend Bit Mask */
#define XDMAC_GRS_RS10(value)                 (XDMAC_GRS_RS10_Msk & ((value) << XDMAC_GRS_RS10_Pos))
#define XDMAC_GRS_RS11_Pos                    _U_(11)                                              /**< (XDMAC_GRS) XDMAC Channel 11 Read Suspend Bit Position */
#define XDMAC_GRS_RS11_Msk                    (_U_(0x1) << XDMAC_GRS_RS11_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 11 Read Suspend Bit Mask */
#define XDMAC_GRS_RS11(value)                 (XDMAC_GRS_RS11_Msk & ((value) << XDMAC_GRS_RS11_Pos))
#define XDMAC_GRS_RS12_Pos                    _U_(12)                                              /**< (XDMAC_GRS) XDMAC Channel 12 Read Suspend Bit Position */
#define XDMAC_GRS_RS12_Msk                    (_U_(0x1) << XDMAC_GRS_RS12_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 12 Read Suspend Bit Mask */
#define XDMAC_GRS_RS12(value)                 (XDMAC_GRS_RS12_Msk & ((value) << XDMAC_GRS_RS12_Pos))
#define XDMAC_GRS_RS13_Pos                    _U_(13)                                              /**< (XDMAC_GRS) XDMAC Channel 13 Read Suspend Bit Position */
#define XDMAC_GRS_RS13_Msk                    (_U_(0x1) << XDMAC_GRS_RS13_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 13 Read Suspend Bit Mask */
#define XDMAC_GRS_RS13(value)                 (XDMAC_GRS_RS13_Msk & ((value) << XDMAC_GRS_RS13_Pos))
#define XDMAC_GRS_RS14_Pos                    _U_(14)                                              /**< (XDMAC_GRS) XDMAC Channel 14 Read Suspend Bit Position */
#define XDMAC_GRS_RS14_Msk                    (_U_(0x1) << XDMAC_GRS_RS14_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 14 Read Suspend Bit Mask */
#define XDMAC_GRS_RS14(value)                 (XDMAC_GRS_RS14_Msk & ((value) << XDMAC_GRS_RS14_Pos))
#define XDMAC_GRS_RS15_Pos                    _U_(15)                                              /**< (XDMAC_GRS) XDMAC Channel 15 Read Suspend Bit Position */
#define XDMAC_GRS_RS15_Msk                    (_U_(0x1) << XDMAC_GRS_RS15_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 15 Read Suspend Bit Mask */
#define XDMAC_GRS_RS15(value)                 (XDMAC_GRS_RS15_Msk & ((value) << XDMAC_GRS_RS15_Pos))
#define XDMAC_GRS_RS16_Pos                    _U_(16)                                              /**< (XDMAC_GRS) XDMAC Channel 16 Read Suspend Bit Position */
#define XDMAC_GRS_RS16_Msk                    (_U_(0x1) << XDMAC_GRS_RS16_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 16 Read Suspend Bit Mask */
#define XDMAC_GRS_RS16(value)                 (XDMAC_GRS_RS16_Msk & ((value) << XDMAC_GRS_RS16_Pos))
#define XDMAC_GRS_RS17_Pos                    _U_(17)                                              /**< (XDMAC_GRS) XDMAC Channel 17 Read Suspend Bit Position */
#define XDMAC_GRS_RS17_Msk                    (_U_(0x1) << XDMAC_GRS_RS17_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 17 Read Suspend Bit Mask */
#define XDMAC_GRS_RS17(value)                 (XDMAC_GRS_RS17_Msk & ((value) << XDMAC_GRS_RS17_Pos))
#define XDMAC_GRS_RS18_Pos                    _U_(18)                                              /**< (XDMAC_GRS) XDMAC Channel 18 Read Suspend Bit Position */
#define XDMAC_GRS_RS18_Msk                    (_U_(0x1) << XDMAC_GRS_RS18_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 18 Read Suspend Bit Mask */
#define XDMAC_GRS_RS18(value)                 (XDMAC_GRS_RS18_Msk & ((value) << XDMAC_GRS_RS18_Pos))
#define XDMAC_GRS_RS19_Pos                    _U_(19)                                              /**< (XDMAC_GRS) XDMAC Channel 19 Read Suspend Bit Position */
#define XDMAC_GRS_RS19_Msk                    (_U_(0x1) << XDMAC_GRS_RS19_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 19 Read Suspend Bit Mask */
#define XDMAC_GRS_RS19(value)                 (XDMAC_GRS_RS19_Msk & ((value) << XDMAC_GRS_RS19_Pos))
#define XDMAC_GRS_RS20_Pos                    _U_(20)                                              /**< (XDMAC_GRS) XDMAC Channel 20 Read Suspend Bit Position */
#define XDMAC_GRS_RS20_Msk                    (_U_(0x1) << XDMAC_GRS_RS20_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 20 Read Suspend Bit Mask */
#define XDMAC_GRS_RS20(value)                 (XDMAC_GRS_RS20_Msk & ((value) << XDMAC_GRS_RS20_Pos))
#define XDMAC_GRS_RS21_Pos                    _U_(21)                                              /**< (XDMAC_GRS) XDMAC Channel 21 Read Suspend Bit Position */
#define XDMAC_GRS_RS21_Msk                    (_U_(0x1) << XDMAC_GRS_RS21_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 21 Read Suspend Bit Mask */
#define XDMAC_GRS_RS21(value)                 (XDMAC_GRS_RS21_Msk & ((value) << XDMAC_GRS_RS21_Pos))
#define XDMAC_GRS_RS22_Pos                    _U_(22)                                              /**< (XDMAC_GRS) XDMAC Channel 22 Read Suspend Bit Position */
#define XDMAC_GRS_RS22_Msk                    (_U_(0x1) << XDMAC_GRS_RS22_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 22 Read Suspend Bit Mask */
#define XDMAC_GRS_RS22(value)                 (XDMAC_GRS_RS22_Msk & ((value) << XDMAC_GRS_RS22_Pos))
#define XDMAC_GRS_RS23_Pos                    _U_(23)                                              /**< (XDMAC_GRS) XDMAC Channel 23 Read Suspend Bit Position */
#define XDMAC_GRS_RS23_Msk                    (_U_(0x1) << XDMAC_GRS_RS23_Pos)                     /**< (XDMAC_GRS) XDMAC Channel 23 Read Suspend Bit Mask */
#define XDMAC_GRS_RS23(value)                 (XDMAC_GRS_RS23_Msk & ((value) << XDMAC_GRS_RS23_Pos))
#define XDMAC_GRS_Msk                         _U_(0x00FFFFFF)                                      /**< (XDMAC_GRS) Register Mask  */

#define XDMAC_GRS_RS_Pos                      _U_(0)                                               /**< (XDMAC_GRS Position) XDMAC Channel 23 Read Suspend Bit */
#define XDMAC_GRS_RS_Msk                      (_U_(0xFFFFFF) << XDMAC_GRS_RS_Pos)                  /**< (XDMAC_GRS Mask) RS */
#define XDMAC_GRS_RS(value)                   (XDMAC_GRS_RS_Msk & ((value) << XDMAC_GRS_RS_Pos))   

/* -------- XDMAC_GWS : (XDMAC Offset: 0x2C) (R/W 32) Global Channel Write Suspend Register -------- */
#define XDMAC_GWS_WS0_Pos                     _U_(0)                                               /**< (XDMAC_GWS) XDMAC Channel 0 Write Suspend Bit Position */
#define XDMAC_GWS_WS0_Msk                     (_U_(0x1) << XDMAC_GWS_WS0_Pos)                      /**< (XDMAC_GWS) XDMAC Channel 0 Write Suspend Bit Mask */
#define XDMAC_GWS_WS0(value)                  (XDMAC_GWS_WS0_Msk & ((value) << XDMAC_GWS_WS0_Pos))
#define XDMAC_GWS_WS1_Pos                     _U_(1)                                               /**< (XDMAC_GWS) XDMAC Channel 1 Write Suspend Bit Position */
#define XDMAC_GWS_WS1_Msk                     (_U_(0x1) << XDMAC_GWS_WS1_Pos)                      /**< (XDMAC_GWS) XDMAC Channel 1 Write Suspend Bit Mask */
#define XDMAC_GWS_WS1(value)                  (XDMAC_GWS_WS1_Msk & ((value) << XDMAC_GWS_WS1_Pos))
#define XDMAC_GWS_WS2_Pos                     _U_(2)                                               /**< (XDMAC_GWS) XDMAC Channel 2 Write Suspend Bit Position */
#define XDMAC_GWS_WS2_Msk                     (_U_(0x1) << XDMAC_GWS_WS2_Pos)                      /**< (XDMAC_GWS) XDMAC Channel 2 Write Suspend Bit Mask */
#define XDMAC_GWS_WS2(value)                  (XDMAC_GWS_WS2_Msk & ((value) << XDMAC_GWS_WS2_Pos))
#define XDMAC_GWS_WS3_Pos                     _U_(3)                                               /**< (XDMAC_GWS) XDMAC Channel 3 Write Suspend Bit Position */
#define XDMAC_GWS_WS3_Msk                     (_U_(0x1) << XDMAC_GWS_WS3_Pos)                      /**< (XDMAC_GWS) XDMAC Channel 3 Write Suspend Bit Mask */
#define XDMAC_GWS_WS3(value)                  (XDMAC_GWS_WS3_Msk & ((value) << XDMAC_GWS_WS3_Pos))
#define XDMAC_GWS_WS4_Pos                     _U_(4)                                               /**< (XDMAC_GWS) XDMAC Channel 4 Write Suspend Bit Position */
#define XDMAC_GWS_WS4_Msk                     (_U_(0x1) << XDMAC_GWS_WS4_Pos)                      /**< (XDMAC_GWS) XDMAC Channel 4 Write Suspend Bit Mask */
#define XDMAC_GWS_WS4(value)                  (XDMAC_GWS_WS4_Msk & ((value) << XDMAC_GWS_WS4_Pos))
#define XDMAC_GWS_WS5_Pos                     _U_(5)                                               /**< (XDMAC_GWS) XDMAC Channel 5 Write Suspend Bit Position */
#define XDMAC_GWS_WS5_Msk                     (_U_(0x1) << XDMAC_GWS_WS5_Pos)                      /**< (XDMAC_GWS) XDMAC Channel 5 Write Suspend Bit Mask */
#define XDMAC_GWS_WS5(value)                  (XDMAC_GWS_WS5_Msk & ((value) << XDMAC_GWS_WS5_Pos))
#define XDMAC_GWS_WS6_Pos                     _U_(6)                                               /**< (XDMAC_GWS) XDMAC Channel 6 Write Suspend Bit Position */
#define XDMAC_GWS_WS6_Msk                     (_U_(0x1) << XDMAC_GWS_WS6_Pos)                      /**< (XDMAC_GWS) XDMAC Channel 6 Write Suspend Bit Mask */
#define XDMAC_GWS_WS6(value)                  (XDMAC_GWS_WS6_Msk & ((value) << XDMAC_GWS_WS6_Pos))
#define XDMAC_GWS_WS7_Pos                     _U_(7)                                               /**< (XDMAC_GWS) XDMAC Channel 7 Write Suspend Bit Position */
#define XDMAC_GWS_WS7_Msk                     (_U_(0x1) << XDMAC_GWS_WS7_Pos)                      /**< (XDMAC_GWS) XDMAC Channel 7 Write Suspend Bit Mask */
#define XDMAC_GWS_WS7(value)                  (XDMAC_GWS_WS7_Msk & ((value) << XDMAC_GWS_WS7_Pos))
#define XDMAC_GWS_WS8_Pos                     _U_(8)                                               /**< (XDMAC_GWS) XDMAC Channel 8 Write Suspend Bit Position */
#define XDMAC_GWS_WS8_Msk                     (_U_(0x1) << XDMAC_GWS_WS8_Pos)                      /**< (XDMAC_GWS) XDMAC Channel 8 Write Suspend Bit Mask */
#define XDMAC_GWS_WS8(value)                  (XDMAC_GWS_WS8_Msk & ((value) << XDMAC_GWS_WS8_Pos))
#define XDMAC_GWS_WS9_Pos                     _U_(9)                                               /**< (XDMAC_GWS) XDMAC Channel 9 Write Suspend Bit Position */
#define XDMAC_GWS_WS9_Msk                     (_U_(0x1) << XDMAC_GWS_WS9_Pos)                      /**< (XDMAC_GWS) XDMAC Channel 9 Write Suspend Bit Mask */
#define XDMAC_GWS_WS9(value)                  (XDMAC_GWS_WS9_Msk & ((value) << XDMAC_GWS_WS9_Pos))
#define XDMAC_GWS_WS10_Pos                    _U_(10)                                              /**< (XDMAC_GWS) XDMAC Channel 10 Write Suspend Bit Position */
#define XDMAC_GWS_WS10_Msk                    (_U_(0x1) << XDMAC_GWS_WS10_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 10 Write Suspend Bit Mask */
#define XDMAC_GWS_WS10(value)                 (XDMAC_GWS_WS10_Msk & ((value) << XDMAC_GWS_WS10_Pos))
#define XDMAC_GWS_WS11_Pos                    _U_(11)                                              /**< (XDMAC_GWS) XDMAC Channel 11 Write Suspend Bit Position */
#define XDMAC_GWS_WS11_Msk                    (_U_(0x1) << XDMAC_GWS_WS11_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 11 Write Suspend Bit Mask */
#define XDMAC_GWS_WS11(value)                 (XDMAC_GWS_WS11_Msk & ((value) << XDMAC_GWS_WS11_Pos))
#define XDMAC_GWS_WS12_Pos                    _U_(12)                                              /**< (XDMAC_GWS) XDMAC Channel 12 Write Suspend Bit Position */
#define XDMAC_GWS_WS12_Msk                    (_U_(0x1) << XDMAC_GWS_WS12_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 12 Write Suspend Bit Mask */
#define XDMAC_GWS_WS12(value)                 (XDMAC_GWS_WS12_Msk & ((value) << XDMAC_GWS_WS12_Pos))
#define XDMAC_GWS_WS13_Pos                    _U_(13)                                              /**< (XDMAC_GWS) XDMAC Channel 13 Write Suspend Bit Position */
#define XDMAC_GWS_WS13_Msk                    (_U_(0x1) << XDMAC_GWS_WS13_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 13 Write Suspend Bit Mask */
#define XDMAC_GWS_WS13(value)                 (XDMAC_GWS_WS13_Msk & ((value) << XDMAC_GWS_WS13_Pos))
#define XDMAC_GWS_WS14_Pos                    _U_(14)                                              /**< (XDMAC_GWS) XDMAC Channel 14 Write Suspend Bit Position */
#define XDMAC_GWS_WS14_Msk                    (_U_(0x1) << XDMAC_GWS_WS14_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 14 Write Suspend Bit Mask */
#define XDMAC_GWS_WS14(value)                 (XDMAC_GWS_WS14_Msk & ((value) << XDMAC_GWS_WS14_Pos))
#define XDMAC_GWS_WS15_Pos                    _U_(15)                                              /**< (XDMAC_GWS) XDMAC Channel 15 Write Suspend Bit Position */
#define XDMAC_GWS_WS15_Msk                    (_U_(0x1) << XDMAC_GWS_WS15_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 15 Write Suspend Bit Mask */
#define XDMAC_GWS_WS15(value)                 (XDMAC_GWS_WS15_Msk & ((value) << XDMAC_GWS_WS15_Pos))
#define XDMAC_GWS_WS16_Pos                    _U_(16)                                              /**< (XDMAC_GWS) XDMAC Channel 16 Write Suspend Bit Position */
#define XDMAC_GWS_WS16_Msk                    (_U_(0x1) << XDMAC_GWS_WS16_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 16 Write Suspend Bit Mask */
#define XDMAC_GWS_WS16(value)                 (XDMAC_GWS_WS16_Msk & ((value) << XDMAC_GWS_WS16_Pos))
#define XDMAC_GWS_WS17_Pos                    _U_(17)                                              /**< (XDMAC_GWS) XDMAC Channel 17 Write Suspend Bit Position */
#define XDMAC_GWS_WS17_Msk                    (_U_(0x1) << XDMAC_GWS_WS17_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 17 Write Suspend Bit Mask */
#define XDMAC_GWS_WS17(value)                 (XDMAC_GWS_WS17_Msk & ((value) << XDMAC_GWS_WS17_Pos))
#define XDMAC_GWS_WS18_Pos                    _U_(18)                                              /**< (XDMAC_GWS) XDMAC Channel 18 Write Suspend Bit Position */
#define XDMAC_GWS_WS18_Msk                    (_U_(0x1) << XDMAC_GWS_WS18_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 18 Write Suspend Bit Mask */
#define XDMAC_GWS_WS18(value)                 (XDMAC_GWS_WS18_Msk & ((value) << XDMAC_GWS_WS18_Pos))
#define XDMAC_GWS_WS19_Pos                    _U_(19)                                              /**< (XDMAC_GWS) XDMAC Channel 19 Write Suspend Bit Position */
#define XDMAC_GWS_WS19_Msk                    (_U_(0x1) << XDMAC_GWS_WS19_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 19 Write Suspend Bit Mask */
#define XDMAC_GWS_WS19(value)                 (XDMAC_GWS_WS19_Msk & ((value) << XDMAC_GWS_WS19_Pos))
#define XDMAC_GWS_WS20_Pos                    _U_(20)                                              /**< (XDMAC_GWS) XDMAC Channel 20 Write Suspend Bit Position */
#define XDMAC_GWS_WS20_Msk                    (_U_(0x1) << XDMAC_GWS_WS20_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 20 Write Suspend Bit Mask */
#define XDMAC_GWS_WS20(value)                 (XDMAC_GWS_WS20_Msk & ((value) << XDMAC_GWS_WS20_Pos))
#define XDMAC_GWS_WS21_Pos                    _U_(21)                                              /**< (XDMAC_GWS) XDMAC Channel 21 Write Suspend Bit Position */
#define XDMAC_GWS_WS21_Msk                    (_U_(0x1) << XDMAC_GWS_WS21_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 21 Write Suspend Bit Mask */
#define XDMAC_GWS_WS21(value)                 (XDMAC_GWS_WS21_Msk & ((value) << XDMAC_GWS_WS21_Pos))
#define XDMAC_GWS_WS22_Pos                    _U_(22)                                              /**< (XDMAC_GWS) XDMAC Channel 22 Write Suspend Bit Position */
#define XDMAC_GWS_WS22_Msk                    (_U_(0x1) << XDMAC_GWS_WS22_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 22 Write Suspend Bit Mask */
#define XDMAC_GWS_WS22(value)                 (XDMAC_GWS_WS22_Msk & ((value) << XDMAC_GWS_WS22_Pos))
#define XDMAC_GWS_WS23_Pos                    _U_(23)                                              /**< (XDMAC_GWS) XDMAC Channel 23 Write Suspend Bit Position */
#define XDMAC_GWS_WS23_Msk                    (_U_(0x1) << XDMAC_GWS_WS23_Pos)                     /**< (XDMAC_GWS) XDMAC Channel 23 Write Suspend Bit Mask */
#define XDMAC_GWS_WS23(value)                 (XDMAC_GWS_WS23_Msk & ((value) << XDMAC_GWS_WS23_Pos))
#define XDMAC_GWS_Msk                         _U_(0x00FFFFFF)                                      /**< (XDMAC_GWS) Register Mask  */

#define XDMAC_GWS_WS_Pos                      _U_(0)                                               /**< (XDMAC_GWS Position) XDMAC Channel 23 Write Suspend Bit */
#define XDMAC_GWS_WS_Msk                      (_U_(0xFFFFFF) << XDMAC_GWS_WS_Pos)                  /**< (XDMAC_GWS Mask) WS */
#define XDMAC_GWS_WS(value)                   (XDMAC_GWS_WS_Msk & ((value) << XDMAC_GWS_WS_Pos))   

/* -------- XDMAC_GRWS : (XDMAC Offset: 0x30) ( /W 32) Global Channel Read Write Suspend Register -------- */
#define XDMAC_GRWS_RWS0_Pos                   _U_(0)                                               /**< (XDMAC_GRWS) XDMAC Channel 0 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS0_Msk                   (_U_(0x1) << XDMAC_GRWS_RWS0_Pos)                    /**< (XDMAC_GRWS) XDMAC Channel 0 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS0(value)                (XDMAC_GRWS_RWS0_Msk & ((value) << XDMAC_GRWS_RWS0_Pos))
#define XDMAC_GRWS_RWS1_Pos                   _U_(1)                                               /**< (XDMAC_GRWS) XDMAC Channel 1 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS1_Msk                   (_U_(0x1) << XDMAC_GRWS_RWS1_Pos)                    /**< (XDMAC_GRWS) XDMAC Channel 1 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS1(value)                (XDMAC_GRWS_RWS1_Msk & ((value) << XDMAC_GRWS_RWS1_Pos))
#define XDMAC_GRWS_RWS2_Pos                   _U_(2)                                               /**< (XDMAC_GRWS) XDMAC Channel 2 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS2_Msk                   (_U_(0x1) << XDMAC_GRWS_RWS2_Pos)                    /**< (XDMAC_GRWS) XDMAC Channel 2 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS2(value)                (XDMAC_GRWS_RWS2_Msk & ((value) << XDMAC_GRWS_RWS2_Pos))
#define XDMAC_GRWS_RWS3_Pos                   _U_(3)                                               /**< (XDMAC_GRWS) XDMAC Channel 3 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS3_Msk                   (_U_(0x1) << XDMAC_GRWS_RWS3_Pos)                    /**< (XDMAC_GRWS) XDMAC Channel 3 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS3(value)                (XDMAC_GRWS_RWS3_Msk & ((value) << XDMAC_GRWS_RWS3_Pos))
#define XDMAC_GRWS_RWS4_Pos                   _U_(4)                                               /**< (XDMAC_GRWS) XDMAC Channel 4 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS4_Msk                   (_U_(0x1) << XDMAC_GRWS_RWS4_Pos)                    /**< (XDMAC_GRWS) XDMAC Channel 4 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS4(value)                (XDMAC_GRWS_RWS4_Msk & ((value) << XDMAC_GRWS_RWS4_Pos))
#define XDMAC_GRWS_RWS5_Pos                   _U_(5)                                               /**< (XDMAC_GRWS) XDMAC Channel 5 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS5_Msk                   (_U_(0x1) << XDMAC_GRWS_RWS5_Pos)                    /**< (XDMAC_GRWS) XDMAC Channel 5 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS5(value)                (XDMAC_GRWS_RWS5_Msk & ((value) << XDMAC_GRWS_RWS5_Pos))
#define XDMAC_GRWS_RWS6_Pos                   _U_(6)                                               /**< (XDMAC_GRWS) XDMAC Channel 6 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS6_Msk                   (_U_(0x1) << XDMAC_GRWS_RWS6_Pos)                    /**< (XDMAC_GRWS) XDMAC Channel 6 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS6(value)                (XDMAC_GRWS_RWS6_Msk & ((value) << XDMAC_GRWS_RWS6_Pos))
#define XDMAC_GRWS_RWS7_Pos                   _U_(7)                                               /**< (XDMAC_GRWS) XDMAC Channel 7 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS7_Msk                   (_U_(0x1) << XDMAC_GRWS_RWS7_Pos)                    /**< (XDMAC_GRWS) XDMAC Channel 7 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS7(value)                (XDMAC_GRWS_RWS7_Msk & ((value) << XDMAC_GRWS_RWS7_Pos))
#define XDMAC_GRWS_RWS8_Pos                   _U_(8)                                               /**< (XDMAC_GRWS) XDMAC Channel 8 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS8_Msk                   (_U_(0x1) << XDMAC_GRWS_RWS8_Pos)                    /**< (XDMAC_GRWS) XDMAC Channel 8 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS8(value)                (XDMAC_GRWS_RWS8_Msk & ((value) << XDMAC_GRWS_RWS8_Pos))
#define XDMAC_GRWS_RWS9_Pos                   _U_(9)                                               /**< (XDMAC_GRWS) XDMAC Channel 9 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS9_Msk                   (_U_(0x1) << XDMAC_GRWS_RWS9_Pos)                    /**< (XDMAC_GRWS) XDMAC Channel 9 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS9(value)                (XDMAC_GRWS_RWS9_Msk & ((value) << XDMAC_GRWS_RWS9_Pos))
#define XDMAC_GRWS_RWS10_Pos                  _U_(10)                                              /**< (XDMAC_GRWS) XDMAC Channel 10 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS10_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS10_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 10 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS10(value)               (XDMAC_GRWS_RWS10_Msk & ((value) << XDMAC_GRWS_RWS10_Pos))
#define XDMAC_GRWS_RWS11_Pos                  _U_(11)                                              /**< (XDMAC_GRWS) XDMAC Channel 11 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS11_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS11_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 11 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS11(value)               (XDMAC_GRWS_RWS11_Msk & ((value) << XDMAC_GRWS_RWS11_Pos))
#define XDMAC_GRWS_RWS12_Pos                  _U_(12)                                              /**< (XDMAC_GRWS) XDMAC Channel 12 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS12_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS12_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 12 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS12(value)               (XDMAC_GRWS_RWS12_Msk & ((value) << XDMAC_GRWS_RWS12_Pos))
#define XDMAC_GRWS_RWS13_Pos                  _U_(13)                                              /**< (XDMAC_GRWS) XDMAC Channel 13 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS13_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS13_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 13 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS13(value)               (XDMAC_GRWS_RWS13_Msk & ((value) << XDMAC_GRWS_RWS13_Pos))
#define XDMAC_GRWS_RWS14_Pos                  _U_(14)                                              /**< (XDMAC_GRWS) XDMAC Channel 14 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS14_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS14_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 14 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS14(value)               (XDMAC_GRWS_RWS14_Msk & ((value) << XDMAC_GRWS_RWS14_Pos))
#define XDMAC_GRWS_RWS15_Pos                  _U_(15)                                              /**< (XDMAC_GRWS) XDMAC Channel 15 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS15_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS15_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 15 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS15(value)               (XDMAC_GRWS_RWS15_Msk & ((value) << XDMAC_GRWS_RWS15_Pos))
#define XDMAC_GRWS_RWS16_Pos                  _U_(16)                                              /**< (XDMAC_GRWS) XDMAC Channel 16 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS16_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS16_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 16 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS16(value)               (XDMAC_GRWS_RWS16_Msk & ((value) << XDMAC_GRWS_RWS16_Pos))
#define XDMAC_GRWS_RWS17_Pos                  _U_(17)                                              /**< (XDMAC_GRWS) XDMAC Channel 17 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS17_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS17_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 17 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS17(value)               (XDMAC_GRWS_RWS17_Msk & ((value) << XDMAC_GRWS_RWS17_Pos))
#define XDMAC_GRWS_RWS18_Pos                  _U_(18)                                              /**< (XDMAC_GRWS) XDMAC Channel 18 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS18_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS18_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 18 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS18(value)               (XDMAC_GRWS_RWS18_Msk & ((value) << XDMAC_GRWS_RWS18_Pos))
#define XDMAC_GRWS_RWS19_Pos                  _U_(19)                                              /**< (XDMAC_GRWS) XDMAC Channel 19 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS19_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS19_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 19 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS19(value)               (XDMAC_GRWS_RWS19_Msk & ((value) << XDMAC_GRWS_RWS19_Pos))
#define XDMAC_GRWS_RWS20_Pos                  _U_(20)                                              /**< (XDMAC_GRWS) XDMAC Channel 20 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS20_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS20_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 20 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS20(value)               (XDMAC_GRWS_RWS20_Msk & ((value) << XDMAC_GRWS_RWS20_Pos))
#define XDMAC_GRWS_RWS21_Pos                  _U_(21)                                              /**< (XDMAC_GRWS) XDMAC Channel 21 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS21_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS21_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 21 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS21(value)               (XDMAC_GRWS_RWS21_Msk & ((value) << XDMAC_GRWS_RWS21_Pos))
#define XDMAC_GRWS_RWS22_Pos                  _U_(22)                                              /**< (XDMAC_GRWS) XDMAC Channel 22 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS22_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS22_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 22 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS22(value)               (XDMAC_GRWS_RWS22_Msk & ((value) << XDMAC_GRWS_RWS22_Pos))
#define XDMAC_GRWS_RWS23_Pos                  _U_(23)                                              /**< (XDMAC_GRWS) XDMAC Channel 23 Read Write Suspend Bit Position */
#define XDMAC_GRWS_RWS23_Msk                  (_U_(0x1) << XDMAC_GRWS_RWS23_Pos)                   /**< (XDMAC_GRWS) XDMAC Channel 23 Read Write Suspend Bit Mask */
#define XDMAC_GRWS_RWS23(value)               (XDMAC_GRWS_RWS23_Msk & ((value) << XDMAC_GRWS_RWS23_Pos))
#define XDMAC_GRWS_Msk                        _U_(0x00FFFFFF)                                      /**< (XDMAC_GRWS) Register Mask  */

#define XDMAC_GRWS_RWS_Pos                    _U_(0)                                               /**< (XDMAC_GRWS Position) XDMAC Channel 23 Read Write Suspend Bit */
#define XDMAC_GRWS_RWS_Msk                    (_U_(0xFFFFFF) << XDMAC_GRWS_RWS_Pos)                /**< (XDMAC_GRWS Mask) RWS */
#define XDMAC_GRWS_RWS(value)                 (XDMAC_GRWS_RWS_Msk & ((value) << XDMAC_GRWS_RWS_Pos)) 

/* -------- XDMAC_GRWR : (XDMAC Offset: 0x34) ( /W 32) Global Channel Read Write Resume Register -------- */
#define XDMAC_GRWR_RWR0_Pos                   _U_(0)                                               /**< (XDMAC_GRWR) XDMAC Channel 0 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR0_Msk                   (_U_(0x1) << XDMAC_GRWR_RWR0_Pos)                    /**< (XDMAC_GRWR) XDMAC Channel 0 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR0(value)                (XDMAC_GRWR_RWR0_Msk & ((value) << XDMAC_GRWR_RWR0_Pos))
#define XDMAC_GRWR_RWR1_Pos                   _U_(1)                                               /**< (XDMAC_GRWR) XDMAC Channel 1 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR1_Msk                   (_U_(0x1) << XDMAC_GRWR_RWR1_Pos)                    /**< (XDMAC_GRWR) XDMAC Channel 1 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR1(value)                (XDMAC_GRWR_RWR1_Msk & ((value) << XDMAC_GRWR_RWR1_Pos))
#define XDMAC_GRWR_RWR2_Pos                   _U_(2)                                               /**< (XDMAC_GRWR) XDMAC Channel 2 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR2_Msk                   (_U_(0x1) << XDMAC_GRWR_RWR2_Pos)                    /**< (XDMAC_GRWR) XDMAC Channel 2 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR2(value)                (XDMAC_GRWR_RWR2_Msk & ((value) << XDMAC_GRWR_RWR2_Pos))
#define XDMAC_GRWR_RWR3_Pos                   _U_(3)                                               /**< (XDMAC_GRWR) XDMAC Channel 3 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR3_Msk                   (_U_(0x1) << XDMAC_GRWR_RWR3_Pos)                    /**< (XDMAC_GRWR) XDMAC Channel 3 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR3(value)                (XDMAC_GRWR_RWR3_Msk & ((value) << XDMAC_GRWR_RWR3_Pos))
#define XDMAC_GRWR_RWR4_Pos                   _U_(4)                                               /**< (XDMAC_GRWR) XDMAC Channel 4 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR4_Msk                   (_U_(0x1) << XDMAC_GRWR_RWR4_Pos)                    /**< (XDMAC_GRWR) XDMAC Channel 4 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR4(value)                (XDMAC_GRWR_RWR4_Msk & ((value) << XDMAC_GRWR_RWR4_Pos))
#define XDMAC_GRWR_RWR5_Pos                   _U_(5)                                               /**< (XDMAC_GRWR) XDMAC Channel 5 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR5_Msk                   (_U_(0x1) << XDMAC_GRWR_RWR5_Pos)                    /**< (XDMAC_GRWR) XDMAC Channel 5 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR5(value)                (XDMAC_GRWR_RWR5_Msk & ((value) << XDMAC_GRWR_RWR5_Pos))
#define XDMAC_GRWR_RWR6_Pos                   _U_(6)                                               /**< (XDMAC_GRWR) XDMAC Channel 6 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR6_Msk                   (_U_(0x1) << XDMAC_GRWR_RWR6_Pos)                    /**< (XDMAC_GRWR) XDMAC Channel 6 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR6(value)                (XDMAC_GRWR_RWR6_Msk & ((value) << XDMAC_GRWR_RWR6_Pos))
#define XDMAC_GRWR_RWR7_Pos                   _U_(7)                                               /**< (XDMAC_GRWR) XDMAC Channel 7 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR7_Msk                   (_U_(0x1) << XDMAC_GRWR_RWR7_Pos)                    /**< (XDMAC_GRWR) XDMAC Channel 7 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR7(value)                (XDMAC_GRWR_RWR7_Msk & ((value) << XDMAC_GRWR_RWR7_Pos))
#define XDMAC_GRWR_RWR8_Pos                   _U_(8)                                               /**< (XDMAC_GRWR) XDMAC Channel 8 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR8_Msk                   (_U_(0x1) << XDMAC_GRWR_RWR8_Pos)                    /**< (XDMAC_GRWR) XDMAC Channel 8 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR8(value)                (XDMAC_GRWR_RWR8_Msk & ((value) << XDMAC_GRWR_RWR8_Pos))
#define XDMAC_GRWR_RWR9_Pos                   _U_(9)                                               /**< (XDMAC_GRWR) XDMAC Channel 9 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR9_Msk                   (_U_(0x1) << XDMAC_GRWR_RWR9_Pos)                    /**< (XDMAC_GRWR) XDMAC Channel 9 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR9(value)                (XDMAC_GRWR_RWR9_Msk & ((value) << XDMAC_GRWR_RWR9_Pos))
#define XDMAC_GRWR_RWR10_Pos                  _U_(10)                                              /**< (XDMAC_GRWR) XDMAC Channel 10 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR10_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR10_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 10 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR10(value)               (XDMAC_GRWR_RWR10_Msk & ((value) << XDMAC_GRWR_RWR10_Pos))
#define XDMAC_GRWR_RWR11_Pos                  _U_(11)                                              /**< (XDMAC_GRWR) XDMAC Channel 11 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR11_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR11_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 11 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR11(value)               (XDMAC_GRWR_RWR11_Msk & ((value) << XDMAC_GRWR_RWR11_Pos))
#define XDMAC_GRWR_RWR12_Pos                  _U_(12)                                              /**< (XDMAC_GRWR) XDMAC Channel 12 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR12_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR12_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 12 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR12(value)               (XDMAC_GRWR_RWR12_Msk & ((value) << XDMAC_GRWR_RWR12_Pos))
#define XDMAC_GRWR_RWR13_Pos                  _U_(13)                                              /**< (XDMAC_GRWR) XDMAC Channel 13 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR13_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR13_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 13 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR13(value)               (XDMAC_GRWR_RWR13_Msk & ((value) << XDMAC_GRWR_RWR13_Pos))
#define XDMAC_GRWR_RWR14_Pos                  _U_(14)                                              /**< (XDMAC_GRWR) XDMAC Channel 14 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR14_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR14_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 14 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR14(value)               (XDMAC_GRWR_RWR14_Msk & ((value) << XDMAC_GRWR_RWR14_Pos))
#define XDMAC_GRWR_RWR15_Pos                  _U_(15)                                              /**< (XDMAC_GRWR) XDMAC Channel 15 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR15_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR15_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 15 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR15(value)               (XDMAC_GRWR_RWR15_Msk & ((value) << XDMAC_GRWR_RWR15_Pos))
#define XDMAC_GRWR_RWR16_Pos                  _U_(16)                                              /**< (XDMAC_GRWR) XDMAC Channel 16 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR16_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR16_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 16 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR16(value)               (XDMAC_GRWR_RWR16_Msk & ((value) << XDMAC_GRWR_RWR16_Pos))
#define XDMAC_GRWR_RWR17_Pos                  _U_(17)                                              /**< (XDMAC_GRWR) XDMAC Channel 17 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR17_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR17_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 17 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR17(value)               (XDMAC_GRWR_RWR17_Msk & ((value) << XDMAC_GRWR_RWR17_Pos))
#define XDMAC_GRWR_RWR18_Pos                  _U_(18)                                              /**< (XDMAC_GRWR) XDMAC Channel 18 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR18_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR18_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 18 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR18(value)               (XDMAC_GRWR_RWR18_Msk & ((value) << XDMAC_GRWR_RWR18_Pos))
#define XDMAC_GRWR_RWR19_Pos                  _U_(19)                                              /**< (XDMAC_GRWR) XDMAC Channel 19 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR19_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR19_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 19 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR19(value)               (XDMAC_GRWR_RWR19_Msk & ((value) << XDMAC_GRWR_RWR19_Pos))
#define XDMAC_GRWR_RWR20_Pos                  _U_(20)                                              /**< (XDMAC_GRWR) XDMAC Channel 20 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR20_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR20_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 20 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR20(value)               (XDMAC_GRWR_RWR20_Msk & ((value) << XDMAC_GRWR_RWR20_Pos))
#define XDMAC_GRWR_RWR21_Pos                  _U_(21)                                              /**< (XDMAC_GRWR) XDMAC Channel 21 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR21_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR21_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 21 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR21(value)               (XDMAC_GRWR_RWR21_Msk & ((value) << XDMAC_GRWR_RWR21_Pos))
#define XDMAC_GRWR_RWR22_Pos                  _U_(22)                                              /**< (XDMAC_GRWR) XDMAC Channel 22 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR22_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR22_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 22 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR22(value)               (XDMAC_GRWR_RWR22_Msk & ((value) << XDMAC_GRWR_RWR22_Pos))
#define XDMAC_GRWR_RWR23_Pos                  _U_(23)                                              /**< (XDMAC_GRWR) XDMAC Channel 23 Read Write Resume Bit Position */
#define XDMAC_GRWR_RWR23_Msk                  (_U_(0x1) << XDMAC_GRWR_RWR23_Pos)                   /**< (XDMAC_GRWR) XDMAC Channel 23 Read Write Resume Bit Mask */
#define XDMAC_GRWR_RWR23(value)               (XDMAC_GRWR_RWR23_Msk & ((value) << XDMAC_GRWR_RWR23_Pos))
#define XDMAC_GRWR_Msk                        _U_(0x00FFFFFF)                                      /**< (XDMAC_GRWR) Register Mask  */

#define XDMAC_GRWR_RWR_Pos                    _U_(0)                                               /**< (XDMAC_GRWR Position) XDMAC Channel 23 Read Write Resume Bit */
#define XDMAC_GRWR_RWR_Msk                    (_U_(0xFFFFFF) << XDMAC_GRWR_RWR_Pos)                /**< (XDMAC_GRWR Mask) RWR */
#define XDMAC_GRWR_RWR(value)                 (XDMAC_GRWR_RWR_Msk & ((value) << XDMAC_GRWR_RWR_Pos)) 

/* -------- XDMAC_GSWR : (XDMAC Offset: 0x38) ( /W 32) Global Channel Software Request Register -------- */
#define XDMAC_GSWR_SWREQ0_Pos                 _U_(0)                                               /**< (XDMAC_GSWR) XDMAC Channel 0 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ0_Msk                 (_U_(0x1) << XDMAC_GSWR_SWREQ0_Pos)                  /**< (XDMAC_GSWR) XDMAC Channel 0 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ0(value)              (XDMAC_GSWR_SWREQ0_Msk & ((value) << XDMAC_GSWR_SWREQ0_Pos))
#define XDMAC_GSWR_SWREQ1_Pos                 _U_(1)                                               /**< (XDMAC_GSWR) XDMAC Channel 1 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ1_Msk                 (_U_(0x1) << XDMAC_GSWR_SWREQ1_Pos)                  /**< (XDMAC_GSWR) XDMAC Channel 1 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ1(value)              (XDMAC_GSWR_SWREQ1_Msk & ((value) << XDMAC_GSWR_SWREQ1_Pos))
#define XDMAC_GSWR_SWREQ2_Pos                 _U_(2)                                               /**< (XDMAC_GSWR) XDMAC Channel 2 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ2_Msk                 (_U_(0x1) << XDMAC_GSWR_SWREQ2_Pos)                  /**< (XDMAC_GSWR) XDMAC Channel 2 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ2(value)              (XDMAC_GSWR_SWREQ2_Msk & ((value) << XDMAC_GSWR_SWREQ2_Pos))
#define XDMAC_GSWR_SWREQ3_Pos                 _U_(3)                                               /**< (XDMAC_GSWR) XDMAC Channel 3 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ3_Msk                 (_U_(0x1) << XDMAC_GSWR_SWREQ3_Pos)                  /**< (XDMAC_GSWR) XDMAC Channel 3 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ3(value)              (XDMAC_GSWR_SWREQ3_Msk & ((value) << XDMAC_GSWR_SWREQ3_Pos))
#define XDMAC_GSWR_SWREQ4_Pos                 _U_(4)                                               /**< (XDMAC_GSWR) XDMAC Channel 4 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ4_Msk                 (_U_(0x1) << XDMAC_GSWR_SWREQ4_Pos)                  /**< (XDMAC_GSWR) XDMAC Channel 4 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ4(value)              (XDMAC_GSWR_SWREQ4_Msk & ((value) << XDMAC_GSWR_SWREQ4_Pos))
#define XDMAC_GSWR_SWREQ5_Pos                 _U_(5)                                               /**< (XDMAC_GSWR) XDMAC Channel 5 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ5_Msk                 (_U_(0x1) << XDMAC_GSWR_SWREQ5_Pos)                  /**< (XDMAC_GSWR) XDMAC Channel 5 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ5(value)              (XDMAC_GSWR_SWREQ5_Msk & ((value) << XDMAC_GSWR_SWREQ5_Pos))
#define XDMAC_GSWR_SWREQ6_Pos                 _U_(6)                                               /**< (XDMAC_GSWR) XDMAC Channel 6 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ6_Msk                 (_U_(0x1) << XDMAC_GSWR_SWREQ6_Pos)                  /**< (XDMAC_GSWR) XDMAC Channel 6 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ6(value)              (XDMAC_GSWR_SWREQ6_Msk & ((value) << XDMAC_GSWR_SWREQ6_Pos))
#define XDMAC_GSWR_SWREQ7_Pos                 _U_(7)                                               /**< (XDMAC_GSWR) XDMAC Channel 7 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ7_Msk                 (_U_(0x1) << XDMAC_GSWR_SWREQ7_Pos)                  /**< (XDMAC_GSWR) XDMAC Channel 7 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ7(value)              (XDMAC_GSWR_SWREQ7_Msk & ((value) << XDMAC_GSWR_SWREQ7_Pos))
#define XDMAC_GSWR_SWREQ8_Pos                 _U_(8)                                               /**< (XDMAC_GSWR) XDMAC Channel 8 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ8_Msk                 (_U_(0x1) << XDMAC_GSWR_SWREQ8_Pos)                  /**< (XDMAC_GSWR) XDMAC Channel 8 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ8(value)              (XDMAC_GSWR_SWREQ8_Msk & ((value) << XDMAC_GSWR_SWREQ8_Pos))
#define XDMAC_GSWR_SWREQ9_Pos                 _U_(9)                                               /**< (XDMAC_GSWR) XDMAC Channel 9 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ9_Msk                 (_U_(0x1) << XDMAC_GSWR_SWREQ9_Pos)                  /**< (XDMAC_GSWR) XDMAC Channel 9 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ9(value)              (XDMAC_GSWR_SWREQ9_Msk & ((value) << XDMAC_GSWR_SWREQ9_Pos))
#define XDMAC_GSWR_SWREQ10_Pos                _U_(10)                                              /**< (XDMAC_GSWR) XDMAC Channel 10 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ10_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ10_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 10 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ10(value)             (XDMAC_GSWR_SWREQ10_Msk & ((value) << XDMAC_GSWR_SWREQ10_Pos))
#define XDMAC_GSWR_SWREQ11_Pos                _U_(11)                                              /**< (XDMAC_GSWR) XDMAC Channel 11 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ11_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ11_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 11 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ11(value)             (XDMAC_GSWR_SWREQ11_Msk & ((value) << XDMAC_GSWR_SWREQ11_Pos))
#define XDMAC_GSWR_SWREQ12_Pos                _U_(12)                                              /**< (XDMAC_GSWR) XDMAC Channel 12 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ12_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ12_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 12 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ12(value)             (XDMAC_GSWR_SWREQ12_Msk & ((value) << XDMAC_GSWR_SWREQ12_Pos))
#define XDMAC_GSWR_SWREQ13_Pos                _U_(13)                                              /**< (XDMAC_GSWR) XDMAC Channel 13 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ13_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ13_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 13 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ13(value)             (XDMAC_GSWR_SWREQ13_Msk & ((value) << XDMAC_GSWR_SWREQ13_Pos))
#define XDMAC_GSWR_SWREQ14_Pos                _U_(14)                                              /**< (XDMAC_GSWR) XDMAC Channel 14 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ14_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ14_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 14 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ14(value)             (XDMAC_GSWR_SWREQ14_Msk & ((value) << XDMAC_GSWR_SWREQ14_Pos))
#define XDMAC_GSWR_SWREQ15_Pos                _U_(15)                                              /**< (XDMAC_GSWR) XDMAC Channel 15 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ15_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ15_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 15 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ15(value)             (XDMAC_GSWR_SWREQ15_Msk & ((value) << XDMAC_GSWR_SWREQ15_Pos))
#define XDMAC_GSWR_SWREQ16_Pos                _U_(16)                                              /**< (XDMAC_GSWR) XDMAC Channel 16 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ16_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ16_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 16 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ16(value)             (XDMAC_GSWR_SWREQ16_Msk & ((value) << XDMAC_GSWR_SWREQ16_Pos))
#define XDMAC_GSWR_SWREQ17_Pos                _U_(17)                                              /**< (XDMAC_GSWR) XDMAC Channel 17 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ17_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ17_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 17 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ17(value)             (XDMAC_GSWR_SWREQ17_Msk & ((value) << XDMAC_GSWR_SWREQ17_Pos))
#define XDMAC_GSWR_SWREQ18_Pos                _U_(18)                                              /**< (XDMAC_GSWR) XDMAC Channel 18 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ18_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ18_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 18 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ18(value)             (XDMAC_GSWR_SWREQ18_Msk & ((value) << XDMAC_GSWR_SWREQ18_Pos))
#define XDMAC_GSWR_SWREQ19_Pos                _U_(19)                                              /**< (XDMAC_GSWR) XDMAC Channel 19 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ19_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ19_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 19 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ19(value)             (XDMAC_GSWR_SWREQ19_Msk & ((value) << XDMAC_GSWR_SWREQ19_Pos))
#define XDMAC_GSWR_SWREQ20_Pos                _U_(20)                                              /**< (XDMAC_GSWR) XDMAC Channel 20 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ20_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ20_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 20 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ20(value)             (XDMAC_GSWR_SWREQ20_Msk & ((value) << XDMAC_GSWR_SWREQ20_Pos))
#define XDMAC_GSWR_SWREQ21_Pos                _U_(21)                                              /**< (XDMAC_GSWR) XDMAC Channel 21 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ21_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ21_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 21 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ21(value)             (XDMAC_GSWR_SWREQ21_Msk & ((value) << XDMAC_GSWR_SWREQ21_Pos))
#define XDMAC_GSWR_SWREQ22_Pos                _U_(22)                                              /**< (XDMAC_GSWR) XDMAC Channel 22 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ22_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ22_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 22 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ22(value)             (XDMAC_GSWR_SWREQ22_Msk & ((value) << XDMAC_GSWR_SWREQ22_Pos))
#define XDMAC_GSWR_SWREQ23_Pos                _U_(23)                                              /**< (XDMAC_GSWR) XDMAC Channel 23 Software Request Bit Position */
#define XDMAC_GSWR_SWREQ23_Msk                (_U_(0x1) << XDMAC_GSWR_SWREQ23_Pos)                 /**< (XDMAC_GSWR) XDMAC Channel 23 Software Request Bit Mask */
#define XDMAC_GSWR_SWREQ23(value)             (XDMAC_GSWR_SWREQ23_Msk & ((value) << XDMAC_GSWR_SWREQ23_Pos))
#define XDMAC_GSWR_Msk                        _U_(0x00FFFFFF)                                      /**< (XDMAC_GSWR) Register Mask  */

#define XDMAC_GSWR_SWREQ_Pos                  _U_(0)                                               /**< (XDMAC_GSWR Position) XDMAC Channel 23 Software Request Bit */
#define XDMAC_GSWR_SWREQ_Msk                  (_U_(0xFFFFFF) << XDMAC_GSWR_SWREQ_Pos)              /**< (XDMAC_GSWR Mask) SWREQ */
#define XDMAC_GSWR_SWREQ(value)               (XDMAC_GSWR_SWREQ_Msk & ((value) << XDMAC_GSWR_SWREQ_Pos)) 

/* -------- XDMAC_GSWS : (XDMAC Offset: 0x3C) ( R/ 32) Global Channel Software Request Status Register -------- */
#define XDMAC_GSWS_SWRS0_Pos                  _U_(0)                                               /**< (XDMAC_GSWS) XDMAC Channel 0 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS0_Msk                  (_U_(0x1) << XDMAC_GSWS_SWRS0_Pos)                   /**< (XDMAC_GSWS) XDMAC Channel 0 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS0(value)               (XDMAC_GSWS_SWRS0_Msk & ((value) << XDMAC_GSWS_SWRS0_Pos))
#define XDMAC_GSWS_SWRS1_Pos                  _U_(1)                                               /**< (XDMAC_GSWS) XDMAC Channel 1 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS1_Msk                  (_U_(0x1) << XDMAC_GSWS_SWRS1_Pos)                   /**< (XDMAC_GSWS) XDMAC Channel 1 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS1(value)               (XDMAC_GSWS_SWRS1_Msk & ((value) << XDMAC_GSWS_SWRS1_Pos))
#define XDMAC_GSWS_SWRS2_Pos                  _U_(2)                                               /**< (XDMAC_GSWS) XDMAC Channel 2 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS2_Msk                  (_U_(0x1) << XDMAC_GSWS_SWRS2_Pos)                   /**< (XDMAC_GSWS) XDMAC Channel 2 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS2(value)               (XDMAC_GSWS_SWRS2_Msk & ((value) << XDMAC_GSWS_SWRS2_Pos))
#define XDMAC_GSWS_SWRS3_Pos                  _U_(3)                                               /**< (XDMAC_GSWS) XDMAC Channel 3 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS3_Msk                  (_U_(0x1) << XDMAC_GSWS_SWRS3_Pos)                   /**< (XDMAC_GSWS) XDMAC Channel 3 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS3(value)               (XDMAC_GSWS_SWRS3_Msk & ((value) << XDMAC_GSWS_SWRS3_Pos))
#define XDMAC_GSWS_SWRS4_Pos                  _U_(4)                                               /**< (XDMAC_GSWS) XDMAC Channel 4 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS4_Msk                  (_U_(0x1) << XDMAC_GSWS_SWRS4_Pos)                   /**< (XDMAC_GSWS) XDMAC Channel 4 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS4(value)               (XDMAC_GSWS_SWRS4_Msk & ((value) << XDMAC_GSWS_SWRS4_Pos))
#define XDMAC_GSWS_SWRS5_Pos                  _U_(5)                                               /**< (XDMAC_GSWS) XDMAC Channel 5 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS5_Msk                  (_U_(0x1) << XDMAC_GSWS_SWRS5_Pos)                   /**< (XDMAC_GSWS) XDMAC Channel 5 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS5(value)               (XDMAC_GSWS_SWRS5_Msk & ((value) << XDMAC_GSWS_SWRS5_Pos))
#define XDMAC_GSWS_SWRS6_Pos                  _U_(6)                                               /**< (XDMAC_GSWS) XDMAC Channel 6 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS6_Msk                  (_U_(0x1) << XDMAC_GSWS_SWRS6_Pos)                   /**< (XDMAC_GSWS) XDMAC Channel 6 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS6(value)               (XDMAC_GSWS_SWRS6_Msk & ((value) << XDMAC_GSWS_SWRS6_Pos))
#define XDMAC_GSWS_SWRS7_Pos                  _U_(7)                                               /**< (XDMAC_GSWS) XDMAC Channel 7 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS7_Msk                  (_U_(0x1) << XDMAC_GSWS_SWRS7_Pos)                   /**< (XDMAC_GSWS) XDMAC Channel 7 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS7(value)               (XDMAC_GSWS_SWRS7_Msk & ((value) << XDMAC_GSWS_SWRS7_Pos))
#define XDMAC_GSWS_SWRS8_Pos                  _U_(8)                                               /**< (XDMAC_GSWS) XDMAC Channel 8 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS8_Msk                  (_U_(0x1) << XDMAC_GSWS_SWRS8_Pos)                   /**< (XDMAC_GSWS) XDMAC Channel 8 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS8(value)               (XDMAC_GSWS_SWRS8_Msk & ((value) << XDMAC_GSWS_SWRS8_Pos))
#define XDMAC_GSWS_SWRS9_Pos                  _U_(9)                                               /**< (XDMAC_GSWS) XDMAC Channel 9 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS9_Msk                  (_U_(0x1) << XDMAC_GSWS_SWRS9_Pos)                   /**< (XDMAC_GSWS) XDMAC Channel 9 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS9(value)               (XDMAC_GSWS_SWRS9_Msk & ((value) << XDMAC_GSWS_SWRS9_Pos))
#define XDMAC_GSWS_SWRS10_Pos                 _U_(10)                                              /**< (XDMAC_GSWS) XDMAC Channel 10 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS10_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS10_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 10 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS10(value)              (XDMAC_GSWS_SWRS10_Msk & ((value) << XDMAC_GSWS_SWRS10_Pos))
#define XDMAC_GSWS_SWRS11_Pos                 _U_(11)                                              /**< (XDMAC_GSWS) XDMAC Channel 11 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS11_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS11_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 11 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS11(value)              (XDMAC_GSWS_SWRS11_Msk & ((value) << XDMAC_GSWS_SWRS11_Pos))
#define XDMAC_GSWS_SWRS12_Pos                 _U_(12)                                              /**< (XDMAC_GSWS) XDMAC Channel 12 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS12_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS12_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 12 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS12(value)              (XDMAC_GSWS_SWRS12_Msk & ((value) << XDMAC_GSWS_SWRS12_Pos))
#define XDMAC_GSWS_SWRS13_Pos                 _U_(13)                                              /**< (XDMAC_GSWS) XDMAC Channel 13 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS13_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS13_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 13 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS13(value)              (XDMAC_GSWS_SWRS13_Msk & ((value) << XDMAC_GSWS_SWRS13_Pos))
#define XDMAC_GSWS_SWRS14_Pos                 _U_(14)                                              /**< (XDMAC_GSWS) XDMAC Channel 14 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS14_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS14_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 14 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS14(value)              (XDMAC_GSWS_SWRS14_Msk & ((value) << XDMAC_GSWS_SWRS14_Pos))
#define XDMAC_GSWS_SWRS15_Pos                 _U_(15)                                              /**< (XDMAC_GSWS) XDMAC Channel 15 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS15_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS15_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 15 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS15(value)              (XDMAC_GSWS_SWRS15_Msk & ((value) << XDMAC_GSWS_SWRS15_Pos))
#define XDMAC_GSWS_SWRS16_Pos                 _U_(16)                                              /**< (XDMAC_GSWS) XDMAC Channel 16 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS16_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS16_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 16 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS16(value)              (XDMAC_GSWS_SWRS16_Msk & ((value) << XDMAC_GSWS_SWRS16_Pos))
#define XDMAC_GSWS_SWRS17_Pos                 _U_(17)                                              /**< (XDMAC_GSWS) XDMAC Channel 17 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS17_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS17_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 17 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS17(value)              (XDMAC_GSWS_SWRS17_Msk & ((value) << XDMAC_GSWS_SWRS17_Pos))
#define XDMAC_GSWS_SWRS18_Pos                 _U_(18)                                              /**< (XDMAC_GSWS) XDMAC Channel 18 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS18_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS18_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 18 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS18(value)              (XDMAC_GSWS_SWRS18_Msk & ((value) << XDMAC_GSWS_SWRS18_Pos))
#define XDMAC_GSWS_SWRS19_Pos                 _U_(19)                                              /**< (XDMAC_GSWS) XDMAC Channel 19 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS19_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS19_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 19 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS19(value)              (XDMAC_GSWS_SWRS19_Msk & ((value) << XDMAC_GSWS_SWRS19_Pos))
#define XDMAC_GSWS_SWRS20_Pos                 _U_(20)                                              /**< (XDMAC_GSWS) XDMAC Channel 20 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS20_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS20_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 20 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS20(value)              (XDMAC_GSWS_SWRS20_Msk & ((value) << XDMAC_GSWS_SWRS20_Pos))
#define XDMAC_GSWS_SWRS21_Pos                 _U_(21)                                              /**< (XDMAC_GSWS) XDMAC Channel 21 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS21_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS21_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 21 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS21(value)              (XDMAC_GSWS_SWRS21_Msk & ((value) << XDMAC_GSWS_SWRS21_Pos))
#define XDMAC_GSWS_SWRS22_Pos                 _U_(22)                                              /**< (XDMAC_GSWS) XDMAC Channel 22 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS22_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS22_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 22 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS22(value)              (XDMAC_GSWS_SWRS22_Msk & ((value) << XDMAC_GSWS_SWRS22_Pos))
#define XDMAC_GSWS_SWRS23_Pos                 _U_(23)                                              /**< (XDMAC_GSWS) XDMAC Channel 23 Software Request Status Bit Position */
#define XDMAC_GSWS_SWRS23_Msk                 (_U_(0x1) << XDMAC_GSWS_SWRS23_Pos)                  /**< (XDMAC_GSWS) XDMAC Channel 23 Software Request Status Bit Mask */
#define XDMAC_GSWS_SWRS23(value)              (XDMAC_GSWS_SWRS23_Msk & ((value) << XDMAC_GSWS_SWRS23_Pos))
#define XDMAC_GSWS_Msk                        _U_(0x00FFFFFF)                                      /**< (XDMAC_GSWS) Register Mask  */

#define XDMAC_GSWS_SWRS_Pos                   _U_(0)                                               /**< (XDMAC_GSWS Position) XDMAC Channel 23 Software Request Status Bit */
#define XDMAC_GSWS_SWRS_Msk                   (_U_(0xFFFFFF) << XDMAC_GSWS_SWRS_Pos)               /**< (XDMAC_GSWS Mask) SWRS */
#define XDMAC_GSWS_SWRS(value)                (XDMAC_GSWS_SWRS_Msk & ((value) << XDMAC_GSWS_SWRS_Pos)) 

/* -------- XDMAC_GSWF : (XDMAC Offset: 0x40) ( /W 32) Global Channel Software Flush Request Register -------- */
#define XDMAC_GSWF_SWF0_Pos                   _U_(0)                                               /**< (XDMAC_GSWF) XDMAC Channel 0 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF0_Msk                   (_U_(0x1) << XDMAC_GSWF_SWF0_Pos)                    /**< (XDMAC_GSWF) XDMAC Channel 0 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF0(value)                (XDMAC_GSWF_SWF0_Msk & ((value) << XDMAC_GSWF_SWF0_Pos))
#define XDMAC_GSWF_SWF1_Pos                   _U_(1)                                               /**< (XDMAC_GSWF) XDMAC Channel 1 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF1_Msk                   (_U_(0x1) << XDMAC_GSWF_SWF1_Pos)                    /**< (XDMAC_GSWF) XDMAC Channel 1 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF1(value)                (XDMAC_GSWF_SWF1_Msk & ((value) << XDMAC_GSWF_SWF1_Pos))
#define XDMAC_GSWF_SWF2_Pos                   _U_(2)                                               /**< (XDMAC_GSWF) XDMAC Channel 2 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF2_Msk                   (_U_(0x1) << XDMAC_GSWF_SWF2_Pos)                    /**< (XDMAC_GSWF) XDMAC Channel 2 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF2(value)                (XDMAC_GSWF_SWF2_Msk & ((value) << XDMAC_GSWF_SWF2_Pos))
#define XDMAC_GSWF_SWF3_Pos                   _U_(3)                                               /**< (XDMAC_GSWF) XDMAC Channel 3 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF3_Msk                   (_U_(0x1) << XDMAC_GSWF_SWF3_Pos)                    /**< (XDMAC_GSWF) XDMAC Channel 3 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF3(value)                (XDMAC_GSWF_SWF3_Msk & ((value) << XDMAC_GSWF_SWF3_Pos))
#define XDMAC_GSWF_SWF4_Pos                   _U_(4)                                               /**< (XDMAC_GSWF) XDMAC Channel 4 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF4_Msk                   (_U_(0x1) << XDMAC_GSWF_SWF4_Pos)                    /**< (XDMAC_GSWF) XDMAC Channel 4 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF4(value)                (XDMAC_GSWF_SWF4_Msk & ((value) << XDMAC_GSWF_SWF4_Pos))
#define XDMAC_GSWF_SWF5_Pos                   _U_(5)                                               /**< (XDMAC_GSWF) XDMAC Channel 5 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF5_Msk                   (_U_(0x1) << XDMAC_GSWF_SWF5_Pos)                    /**< (XDMAC_GSWF) XDMAC Channel 5 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF5(value)                (XDMAC_GSWF_SWF5_Msk & ((value) << XDMAC_GSWF_SWF5_Pos))
#define XDMAC_GSWF_SWF6_Pos                   _U_(6)                                               /**< (XDMAC_GSWF) XDMAC Channel 6 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF6_Msk                   (_U_(0x1) << XDMAC_GSWF_SWF6_Pos)                    /**< (XDMAC_GSWF) XDMAC Channel 6 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF6(value)                (XDMAC_GSWF_SWF6_Msk & ((value) << XDMAC_GSWF_SWF6_Pos))
#define XDMAC_GSWF_SWF7_Pos                   _U_(7)                                               /**< (XDMAC_GSWF) XDMAC Channel 7 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF7_Msk                   (_U_(0x1) << XDMAC_GSWF_SWF7_Pos)                    /**< (XDMAC_GSWF) XDMAC Channel 7 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF7(value)                (XDMAC_GSWF_SWF7_Msk & ((value) << XDMAC_GSWF_SWF7_Pos))
#define XDMAC_GSWF_SWF8_Pos                   _U_(8)                                               /**< (XDMAC_GSWF) XDMAC Channel 8 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF8_Msk                   (_U_(0x1) << XDMAC_GSWF_SWF8_Pos)                    /**< (XDMAC_GSWF) XDMAC Channel 8 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF8(value)                (XDMAC_GSWF_SWF8_Msk & ((value) << XDMAC_GSWF_SWF8_Pos))
#define XDMAC_GSWF_SWF9_Pos                   _U_(9)                                               /**< (XDMAC_GSWF) XDMAC Channel 9 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF9_Msk                   (_U_(0x1) << XDMAC_GSWF_SWF9_Pos)                    /**< (XDMAC_GSWF) XDMAC Channel 9 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF9(value)                (XDMAC_GSWF_SWF9_Msk & ((value) << XDMAC_GSWF_SWF9_Pos))
#define XDMAC_GSWF_SWF10_Pos                  _U_(10)                                              /**< (XDMAC_GSWF) XDMAC Channel 10 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF10_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF10_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 10 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF10(value)               (XDMAC_GSWF_SWF10_Msk & ((value) << XDMAC_GSWF_SWF10_Pos))
#define XDMAC_GSWF_SWF11_Pos                  _U_(11)                                              /**< (XDMAC_GSWF) XDMAC Channel 11 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF11_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF11_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 11 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF11(value)               (XDMAC_GSWF_SWF11_Msk & ((value) << XDMAC_GSWF_SWF11_Pos))
#define XDMAC_GSWF_SWF12_Pos                  _U_(12)                                              /**< (XDMAC_GSWF) XDMAC Channel 12 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF12_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF12_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 12 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF12(value)               (XDMAC_GSWF_SWF12_Msk & ((value) << XDMAC_GSWF_SWF12_Pos))
#define XDMAC_GSWF_SWF13_Pos                  _U_(13)                                              /**< (XDMAC_GSWF) XDMAC Channel 13 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF13_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF13_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 13 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF13(value)               (XDMAC_GSWF_SWF13_Msk & ((value) << XDMAC_GSWF_SWF13_Pos))
#define XDMAC_GSWF_SWF14_Pos                  _U_(14)                                              /**< (XDMAC_GSWF) XDMAC Channel 14 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF14_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF14_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 14 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF14(value)               (XDMAC_GSWF_SWF14_Msk & ((value) << XDMAC_GSWF_SWF14_Pos))
#define XDMAC_GSWF_SWF15_Pos                  _U_(15)                                              /**< (XDMAC_GSWF) XDMAC Channel 15 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF15_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF15_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 15 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF15(value)               (XDMAC_GSWF_SWF15_Msk & ((value) << XDMAC_GSWF_SWF15_Pos))
#define XDMAC_GSWF_SWF16_Pos                  _U_(16)                                              /**< (XDMAC_GSWF) XDMAC Channel 16 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF16_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF16_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 16 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF16(value)               (XDMAC_GSWF_SWF16_Msk & ((value) << XDMAC_GSWF_SWF16_Pos))
#define XDMAC_GSWF_SWF17_Pos                  _U_(17)                                              /**< (XDMAC_GSWF) XDMAC Channel 17 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF17_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF17_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 17 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF17(value)               (XDMAC_GSWF_SWF17_Msk & ((value) << XDMAC_GSWF_SWF17_Pos))
#define XDMAC_GSWF_SWF18_Pos                  _U_(18)                                              /**< (XDMAC_GSWF) XDMAC Channel 18 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF18_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF18_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 18 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF18(value)               (XDMAC_GSWF_SWF18_Msk & ((value) << XDMAC_GSWF_SWF18_Pos))
#define XDMAC_GSWF_SWF19_Pos                  _U_(19)                                              /**< (XDMAC_GSWF) XDMAC Channel 19 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF19_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF19_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 19 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF19(value)               (XDMAC_GSWF_SWF19_Msk & ((value) << XDMAC_GSWF_SWF19_Pos))
#define XDMAC_GSWF_SWF20_Pos                  _U_(20)                                              /**< (XDMAC_GSWF) XDMAC Channel 20 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF20_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF20_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 20 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF20(value)               (XDMAC_GSWF_SWF20_Msk & ((value) << XDMAC_GSWF_SWF20_Pos))
#define XDMAC_GSWF_SWF21_Pos                  _U_(21)                                              /**< (XDMAC_GSWF) XDMAC Channel 21 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF21_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF21_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 21 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF21(value)               (XDMAC_GSWF_SWF21_Msk & ((value) << XDMAC_GSWF_SWF21_Pos))
#define XDMAC_GSWF_SWF22_Pos                  _U_(22)                                              /**< (XDMAC_GSWF) XDMAC Channel 22 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF22_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF22_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 22 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF22(value)               (XDMAC_GSWF_SWF22_Msk & ((value) << XDMAC_GSWF_SWF22_Pos))
#define XDMAC_GSWF_SWF23_Pos                  _U_(23)                                              /**< (XDMAC_GSWF) XDMAC Channel 23 Software Flush Request Bit Position */
#define XDMAC_GSWF_SWF23_Msk                  (_U_(0x1) << XDMAC_GSWF_SWF23_Pos)                   /**< (XDMAC_GSWF) XDMAC Channel 23 Software Flush Request Bit Mask */
#define XDMAC_GSWF_SWF23(value)               (XDMAC_GSWF_SWF23_Msk & ((value) << XDMAC_GSWF_SWF23_Pos))
#define XDMAC_GSWF_Msk                        _U_(0x00FFFFFF)                                      /**< (XDMAC_GSWF) Register Mask  */

#define XDMAC_GSWF_SWF_Pos                    _U_(0)                                               /**< (XDMAC_GSWF Position) XDMAC Channel 23 Software Flush Request Bit */
#define XDMAC_GSWF_SWF_Msk                    (_U_(0xFFFFFF) << XDMAC_GSWF_SWF_Pos)                /**< (XDMAC_GSWF Mask) SWF */
#define XDMAC_GSWF_SWF(value)                 (XDMAC_GSWF_SWF_Msk & ((value) << XDMAC_GSWF_SWF_Pos)) 

/** \brief XDMAC register offsets definitions */
#define XDMAC_CIE_REG_OFST             (0x00)              /**< (XDMAC_CIE) Channel Interrupt Enable Register Offset */
#define XDMAC_CID_REG_OFST             (0x04)              /**< (XDMAC_CID) Channel Interrupt Disable Register Offset */
#define XDMAC_CIM_REG_OFST             (0x08)              /**< (XDMAC_CIM) Channel Interrupt Mask Register Offset */
#define XDMAC_CIS_REG_OFST             (0x0C)              /**< (XDMAC_CIS) Channel Interrupt Status Register Offset */
#define XDMAC_CSA_REG_OFST             (0x10)              /**< (XDMAC_CSA) Channel Source Address Register Offset */
#define XDMAC_CDA_REG_OFST             (0x14)              /**< (XDMAC_CDA) Channel Destination Address Register Offset */
#define XDMAC_CNDA_REG_OFST            (0x18)              /**< (XDMAC_CNDA) Channel Next Descriptor Address Register Offset */
#define XDMAC_CNDC_REG_OFST            (0x1C)              /**< (XDMAC_CNDC) Channel Next Descriptor Control Register Offset */
#define XDMAC_CUBC_REG_OFST            (0x20)              /**< (XDMAC_CUBC) Channel Microblock Control Register Offset */
#define XDMAC_CBC_REG_OFST             (0x24)              /**< (XDMAC_CBC) Channel Block Control Register Offset */
#define XDMAC_CC_REG_OFST              (0x28)              /**< (XDMAC_CC) Channel Configuration Register Offset */
#define XDMAC_CDS_MSP_REG_OFST         (0x2C)              /**< (XDMAC_CDS_MSP) Channel Data Stride Memory Set Pattern Offset */
#define XDMAC_CSUS_REG_OFST            (0x30)              /**< (XDMAC_CSUS) Channel Source Microblock Stride Offset */
#define XDMAC_CDUS_REG_OFST            (0x34)              /**< (XDMAC_CDUS) Channel Destination Microblock Stride Offset */
#define XDMAC_GTYPE_REG_OFST           (0x00)              /**< (XDMAC_GTYPE) Global Type Register Offset */
#define XDMAC_GCFG_REG_OFST            (0x04)              /**< (XDMAC_GCFG) Global Configuration Register Offset */
#define XDMAC_GWAC_REG_OFST            (0x08)              /**< (XDMAC_GWAC) Global Weighted Arbiter Configuration Register Offset */
#define XDMAC_GIE_REG_OFST             (0x0C)              /**< (XDMAC_GIE) Global Interrupt Enable Register Offset */
#define XDMAC_GID_REG_OFST             (0x10)              /**< (XDMAC_GID) Global Interrupt Disable Register Offset */
#define XDMAC_GIM_REG_OFST             (0x14)              /**< (XDMAC_GIM) Global Interrupt Mask Register Offset */
#define XDMAC_GIS_REG_OFST             (0x18)              /**< (XDMAC_GIS) Global Interrupt Status Register Offset */
#define XDMAC_GE_REG_OFST              (0x1C)              /**< (XDMAC_GE) Global Channel Enable Register Offset */
#define XDMAC_GD_REG_OFST              (0x20)              /**< (XDMAC_GD) Global Channel Disable Register Offset */
#define XDMAC_GS_REG_OFST              (0x24)              /**< (XDMAC_GS) Global Channel Status Register Offset */
#define XDMAC_GRS_REG_OFST             (0x28)              /**< (XDMAC_GRS) Global Channel Read Suspend Register Offset */
#define XDMAC_GWS_REG_OFST             (0x2C)              /**< (XDMAC_GWS) Global Channel Write Suspend Register Offset */
#define XDMAC_GRWS_REG_OFST            (0x30)              /**< (XDMAC_GRWS) Global Channel Read Write Suspend Register Offset */
#define XDMAC_GRWR_REG_OFST            (0x34)              /**< (XDMAC_GRWR) Global Channel Read Write Resume Register Offset */
#define XDMAC_GSWR_REG_OFST            (0x38)              /**< (XDMAC_GSWR) Global Channel Software Request Register Offset */
#define XDMAC_GSWS_REG_OFST            (0x3C)              /**< (XDMAC_GSWS) Global Channel Software Request Status Register Offset */
#define XDMAC_GSWF_REG_OFST            (0x40)              /**< (XDMAC_GSWF) Global Channel Software Flush Request Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief XDMAC_CHID register API structure */
typedef struct
{
  __O   uint32_t                       XDMAC_CIE;          /**< Offset: 0x00 ( /W  32) Channel Interrupt Enable Register */
  __O   uint32_t                       XDMAC_CID;          /**< Offset: 0x04 ( /W  32) Channel Interrupt Disable Register */
  __I   uint32_t                       XDMAC_CIM;          /**< Offset: 0x08 (R/   32) Channel Interrupt Mask Register */
  __I   uint32_t                       XDMAC_CIS;          /**< Offset: 0x0C (R/   32) Channel Interrupt Status Register */
  __IO  uint32_t                       XDMAC_CSA;          /**< Offset: 0x10 (R/W  32) Channel Source Address Register */
  __IO  uint32_t                       XDMAC_CDA;          /**< Offset: 0x14 (R/W  32) Channel Destination Address Register */
  __IO  uint32_t                       XDMAC_CNDA;         /**< Offset: 0x18 (R/W  32) Channel Next Descriptor Address Register */
  __IO  uint32_t                       XDMAC_CNDC;         /**< Offset: 0x1C (R/W  32) Channel Next Descriptor Control Register */
  __IO  uint32_t                       XDMAC_CUBC;         /**< Offset: 0x20 (R/W  32) Channel Microblock Control Register */
  __IO  uint32_t                       XDMAC_CBC;          /**< Offset: 0x24 (R/W  32) Channel Block Control Register */
  __IO  uint32_t                       XDMAC_CC;           /**< Offset: 0x28 (R/W  32) Channel Configuration Register */
  __IO  uint32_t                       XDMAC_CDS_MSP;      /**< Offset: 0x2C (R/W  32) Channel Data Stride Memory Set Pattern */
  __IO  uint32_t                       XDMAC_CSUS;         /**< Offset: 0x30 (R/W  32) Channel Source Microblock Stride */
  __IO  uint32_t                       XDMAC_CDUS;         /**< Offset: 0x34 (R/W  32) Channel Destination Microblock Stride */
  __I   uint8_t                        Reserved1[0x08];
} xdmac_chid_registers_t;

#define XDMAC_CHID_NUMBER _U_(24)

/** \brief XDMAC register API structure */
typedef struct
{
  __I   uint32_t                       XDMAC_GTYPE;        /**< Offset: 0x00 (R/   32) Global Type Register */
  __IO  uint32_t                       XDMAC_GCFG;         /**< Offset: 0x04 (R/W  32) Global Configuration Register */
  __IO  uint32_t                       XDMAC_GWAC;         /**< Offset: 0x08 (R/W  32) Global Weighted Arbiter Configuration Register */
  __O   uint32_t                       XDMAC_GIE;          /**< Offset: 0x0C ( /W  32) Global Interrupt Enable Register */
  __O   uint32_t                       XDMAC_GID;          /**< Offset: 0x10 ( /W  32) Global Interrupt Disable Register */
  __I   uint32_t                       XDMAC_GIM;          /**< Offset: 0x14 (R/   32) Global Interrupt Mask Register */
  __I   uint32_t                       XDMAC_GIS;          /**< Offset: 0x18 (R/   32) Global Interrupt Status Register */
  __O   uint32_t                       XDMAC_GE;           /**< Offset: 0x1C ( /W  32) Global Channel Enable Register */
  __O   uint32_t                       XDMAC_GD;           /**< Offset: 0x20 ( /W  32) Global Channel Disable Register */
  __I   uint32_t                       XDMAC_GS;           /**< Offset: 0x24 (R/   32) Global Channel Status Register */
  __IO  uint32_t                       XDMAC_GRS;          /**< Offset: 0x28 (R/W  32) Global Channel Read Suspend Register */
  __IO  uint32_t                       XDMAC_GWS;          /**< Offset: 0x2C (R/W  32) Global Channel Write Suspend Register */
  __O   uint32_t                       XDMAC_GRWS;         /**< Offset: 0x30 ( /W  32) Global Channel Read Write Suspend Register */
  __O   uint32_t                       XDMAC_GRWR;         /**< Offset: 0x34 ( /W  32) Global Channel Read Write Resume Register */
  __O   uint32_t                       XDMAC_GSWR;         /**< Offset: 0x38 ( /W  32) Global Channel Software Request Register */
  __I   uint32_t                       XDMAC_GSWS;         /**< Offset: 0x3C (R/   32) Global Channel Software Request Status Register */
  __O   uint32_t                       XDMAC_GSWF;         /**< Offset: 0x40 ( /W  32) Global Channel Software Flush Request Register */
  __I   uint8_t                        Reserved1[0x0C];
        xdmac_chid_registers_t         XDMAC_CHID[XDMAC_CHID_NUMBER]; /**< Offset: 0x50 Channel Interrupt Enable Register */
} xdmac_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_XDMAC_COMPONENT_H_ */
