/**
 * \brief Component description for RAMECC
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
#ifndef _SAME54_RAMECC_COMPONENT_H_
#define _SAME54_RAMECC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR RAMECC                                       */
/* ************************************************************************** */

/* -------- RAMECC_INTENCLR : (RAMECC Offset: 0x00) (R/W 8) Interrupt Enable Clear -------- */
#define RAMECC_INTENCLR_RESETVALUE            _U_(0x00)                                            /**<  (RAMECC_INTENCLR) Interrupt Enable Clear  Reset Value */

#define RAMECC_INTENCLR_SINGLEE_Pos           _U_(0)                                               /**< (RAMECC_INTENCLR) Single Bit ECC Error Interrupt Enable Clear Position */
#define RAMECC_INTENCLR_SINGLEE_Msk           (_U_(0x1) << RAMECC_INTENCLR_SINGLEE_Pos)            /**< (RAMECC_INTENCLR) Single Bit ECC Error Interrupt Enable Clear Mask */
#define RAMECC_INTENCLR_SINGLEE(value)        (RAMECC_INTENCLR_SINGLEE_Msk & ((value) << RAMECC_INTENCLR_SINGLEE_Pos))
#define RAMECC_INTENCLR_DUALE_Pos             _U_(1)                                               /**< (RAMECC_INTENCLR) Dual Bit ECC Error Interrupt Enable Clear Position */
#define RAMECC_INTENCLR_DUALE_Msk             (_U_(0x1) << RAMECC_INTENCLR_DUALE_Pos)              /**< (RAMECC_INTENCLR) Dual Bit ECC Error Interrupt Enable Clear Mask */
#define RAMECC_INTENCLR_DUALE(value)          (RAMECC_INTENCLR_DUALE_Msk & ((value) << RAMECC_INTENCLR_DUALE_Pos))
#define RAMECC_INTENCLR_Msk                   _U_(0x03)                                            /**< (RAMECC_INTENCLR) Register Mask  */


/* -------- RAMECC_INTENSET : (RAMECC Offset: 0x01) (R/W 8) Interrupt Enable Set -------- */
#define RAMECC_INTENSET_RESETVALUE            _U_(0x00)                                            /**<  (RAMECC_INTENSET) Interrupt Enable Set  Reset Value */

#define RAMECC_INTENSET_SINGLEE_Pos           _U_(0)                                               /**< (RAMECC_INTENSET) Single Bit ECC Error Interrupt Enable Set Position */
#define RAMECC_INTENSET_SINGLEE_Msk           (_U_(0x1) << RAMECC_INTENSET_SINGLEE_Pos)            /**< (RAMECC_INTENSET) Single Bit ECC Error Interrupt Enable Set Mask */
#define RAMECC_INTENSET_SINGLEE(value)        (RAMECC_INTENSET_SINGLEE_Msk & ((value) << RAMECC_INTENSET_SINGLEE_Pos))
#define RAMECC_INTENSET_DUALE_Pos             _U_(1)                                               /**< (RAMECC_INTENSET) Dual Bit ECC Error Interrupt Enable Set Position */
#define RAMECC_INTENSET_DUALE_Msk             (_U_(0x1) << RAMECC_INTENSET_DUALE_Pos)              /**< (RAMECC_INTENSET) Dual Bit ECC Error Interrupt Enable Set Mask */
#define RAMECC_INTENSET_DUALE(value)          (RAMECC_INTENSET_DUALE_Msk & ((value) << RAMECC_INTENSET_DUALE_Pos))
#define RAMECC_INTENSET_Msk                   _U_(0x03)                                            /**< (RAMECC_INTENSET) Register Mask  */


/* -------- RAMECC_INTFLAG : (RAMECC Offset: 0x02) (R/W 8) Interrupt Flag -------- */
#define RAMECC_INTFLAG_RESETVALUE             _U_(0x00)                                            /**<  (RAMECC_INTFLAG) Interrupt Flag  Reset Value */

#define RAMECC_INTFLAG_SINGLEE_Pos            _U_(0)                                               /**< (RAMECC_INTFLAG) Single Bit ECC Error Interrupt Position */
#define RAMECC_INTFLAG_SINGLEE_Msk            (_U_(0x1) << RAMECC_INTFLAG_SINGLEE_Pos)             /**< (RAMECC_INTFLAG) Single Bit ECC Error Interrupt Mask */
#define RAMECC_INTFLAG_SINGLEE(value)         (RAMECC_INTFLAG_SINGLEE_Msk & ((value) << RAMECC_INTFLAG_SINGLEE_Pos))
#define RAMECC_INTFLAG_DUALE_Pos              _U_(1)                                               /**< (RAMECC_INTFLAG) Dual Bit ECC Error Interrupt Position */
#define RAMECC_INTFLAG_DUALE_Msk              (_U_(0x1) << RAMECC_INTFLAG_DUALE_Pos)               /**< (RAMECC_INTFLAG) Dual Bit ECC Error Interrupt Mask */
#define RAMECC_INTFLAG_DUALE(value)           (RAMECC_INTFLAG_DUALE_Msk & ((value) << RAMECC_INTFLAG_DUALE_Pos))
#define RAMECC_INTFLAG_Msk                    _U_(0x03)                                            /**< (RAMECC_INTFLAG) Register Mask  */


/* -------- RAMECC_STATUS : (RAMECC Offset: 0x03) ( R/ 8) Status -------- */
#define RAMECC_STATUS_RESETVALUE              _U_(0x00)                                            /**<  (RAMECC_STATUS) Status  Reset Value */

#define RAMECC_STATUS_ECCDIS_Pos              _U_(0)                                               /**< (RAMECC_STATUS) ECC Disable Position */
#define RAMECC_STATUS_ECCDIS_Msk              (_U_(0x1) << RAMECC_STATUS_ECCDIS_Pos)               /**< (RAMECC_STATUS) ECC Disable Mask */
#define RAMECC_STATUS_ECCDIS(value)           (RAMECC_STATUS_ECCDIS_Msk & ((value) << RAMECC_STATUS_ECCDIS_Pos))
#define RAMECC_STATUS_Msk                     _U_(0x01)                                            /**< (RAMECC_STATUS) Register Mask  */


/* -------- RAMECC_ERRADDR : (RAMECC Offset: 0x04) ( R/ 32) Error Address -------- */
#define RAMECC_ERRADDR_RESETVALUE             _U_(0x00)                                            /**<  (RAMECC_ERRADDR) Error Address  Reset Value */

#define RAMECC_ERRADDR_ERRADDR_Pos            _U_(0)                                               /**< (RAMECC_ERRADDR) Error Address Position */
#define RAMECC_ERRADDR_ERRADDR_Msk            (_U_(0x1FFFF) << RAMECC_ERRADDR_ERRADDR_Pos)         /**< (RAMECC_ERRADDR) Error Address Mask */
#define RAMECC_ERRADDR_ERRADDR(value)         (RAMECC_ERRADDR_ERRADDR_Msk & ((value) << RAMECC_ERRADDR_ERRADDR_Pos))
#define RAMECC_ERRADDR_Msk                    _U_(0x0001FFFF)                                      /**< (RAMECC_ERRADDR) Register Mask  */


/* -------- RAMECC_DBGCTRL : (RAMECC Offset: 0x0F) (R/W 8) Debug Control -------- */
#define RAMECC_DBGCTRL_RESETVALUE             _U_(0x00)                                            /**<  (RAMECC_DBGCTRL) Debug Control  Reset Value */

#define RAMECC_DBGCTRL_ECCDIS_Pos             _U_(0)                                               /**< (RAMECC_DBGCTRL) ECC Disable Position */
#define RAMECC_DBGCTRL_ECCDIS_Msk             (_U_(0x1) << RAMECC_DBGCTRL_ECCDIS_Pos)              /**< (RAMECC_DBGCTRL) ECC Disable Mask */
#define RAMECC_DBGCTRL_ECCDIS(value)          (RAMECC_DBGCTRL_ECCDIS_Msk & ((value) << RAMECC_DBGCTRL_ECCDIS_Pos))
#define RAMECC_DBGCTRL_ECCELOG_Pos            _U_(1)                                               /**< (RAMECC_DBGCTRL) ECC Error Log Position */
#define RAMECC_DBGCTRL_ECCELOG_Msk            (_U_(0x1) << RAMECC_DBGCTRL_ECCELOG_Pos)             /**< (RAMECC_DBGCTRL) ECC Error Log Mask */
#define RAMECC_DBGCTRL_ECCELOG(value)         (RAMECC_DBGCTRL_ECCELOG_Msk & ((value) << RAMECC_DBGCTRL_ECCELOG_Pos))
#define RAMECC_DBGCTRL_Msk                    _U_(0x03)                                            /**< (RAMECC_DBGCTRL) Register Mask  */


/** \brief RAMECC register offsets definitions */
#define RAMECC_INTENCLR_REG_OFST       (0x00)              /**< (RAMECC_INTENCLR) Interrupt Enable Clear Offset */
#define RAMECC_INTENSET_REG_OFST       (0x01)              /**< (RAMECC_INTENSET) Interrupt Enable Set Offset */
#define RAMECC_INTFLAG_REG_OFST        (0x02)              /**< (RAMECC_INTFLAG) Interrupt Flag Offset */
#define RAMECC_STATUS_REG_OFST         (0x03)              /**< (RAMECC_STATUS) Status Offset */
#define RAMECC_ERRADDR_REG_OFST        (0x04)              /**< (RAMECC_ERRADDR) Error Address Offset */
#define RAMECC_DBGCTRL_REG_OFST        (0x0F)              /**< (RAMECC_DBGCTRL) Debug Control Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief RAMECC register API structure */
typedef struct
{  /* RAM ECC */
  __IO  uint8_t                        RAMECC_INTENCLR;    /**< Offset: 0x00 (R/W  8) Interrupt Enable Clear */
  __IO  uint8_t                        RAMECC_INTENSET;    /**< Offset: 0x01 (R/W  8) Interrupt Enable Set */
  __IO  uint8_t                        RAMECC_INTFLAG;     /**< Offset: 0x02 (R/W  8) Interrupt Flag */
  __I   uint8_t                        RAMECC_STATUS;      /**< Offset: 0x03 (R/   8) Status */
  __I   uint32_t                       RAMECC_ERRADDR;     /**< Offset: 0x04 (R/   32) Error Address */
  __I   uint8_t                        Reserved1[0x07];
  __IO  uint8_t                        RAMECC_DBGCTRL;     /**< Offset: 0x0F (R/W  8) Debug Control */
} ramecc_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_RAMECC_COMPONENT_H_ */
