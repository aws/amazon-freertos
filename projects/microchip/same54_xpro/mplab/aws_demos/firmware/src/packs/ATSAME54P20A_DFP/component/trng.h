/**
 * \brief Component description for TRNG
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
#ifndef _SAME54_TRNG_COMPONENT_H_
#define _SAME54_TRNG_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR TRNG                                         */
/* ************************************************************************** */

/* -------- TRNG_CTRLA : (TRNG Offset: 0x00) (R/W 8) Control A -------- */
#define TRNG_CTRLA_RESETVALUE                 _U_(0x00)                                            /**<  (TRNG_CTRLA) Control A  Reset Value */

#define TRNG_CTRLA_ENABLE_Pos                 _U_(1)                                               /**< (TRNG_CTRLA) Enable Position */
#define TRNG_CTRLA_ENABLE_Msk                 (_U_(0x1) << TRNG_CTRLA_ENABLE_Pos)                  /**< (TRNG_CTRLA) Enable Mask */
#define TRNG_CTRLA_ENABLE(value)              (TRNG_CTRLA_ENABLE_Msk & ((value) << TRNG_CTRLA_ENABLE_Pos))
#define TRNG_CTRLA_RUNSTDBY_Pos               _U_(6)                                               /**< (TRNG_CTRLA) Run in Standby Position */
#define TRNG_CTRLA_RUNSTDBY_Msk               (_U_(0x1) << TRNG_CTRLA_RUNSTDBY_Pos)                /**< (TRNG_CTRLA) Run in Standby Mask */
#define TRNG_CTRLA_RUNSTDBY(value)            (TRNG_CTRLA_RUNSTDBY_Msk & ((value) << TRNG_CTRLA_RUNSTDBY_Pos))
#define TRNG_CTRLA_Msk                        _U_(0x42)                                            /**< (TRNG_CTRLA) Register Mask  */


/* -------- TRNG_EVCTRL : (TRNG Offset: 0x04) (R/W 8) Event Control -------- */
#define TRNG_EVCTRL_RESETVALUE                _U_(0x00)                                            /**<  (TRNG_EVCTRL) Event Control  Reset Value */

#define TRNG_EVCTRL_DATARDYEO_Pos             _U_(0)                                               /**< (TRNG_EVCTRL) Data Ready Event Output Position */
#define TRNG_EVCTRL_DATARDYEO_Msk             (_U_(0x1) << TRNG_EVCTRL_DATARDYEO_Pos)              /**< (TRNG_EVCTRL) Data Ready Event Output Mask */
#define TRNG_EVCTRL_DATARDYEO(value)          (TRNG_EVCTRL_DATARDYEO_Msk & ((value) << TRNG_EVCTRL_DATARDYEO_Pos))
#define TRNG_EVCTRL_Msk                       _U_(0x01)                                            /**< (TRNG_EVCTRL) Register Mask  */


/* -------- TRNG_INTENCLR : (TRNG Offset: 0x08) (R/W 8) Interrupt Enable Clear -------- */
#define TRNG_INTENCLR_RESETVALUE              _U_(0x00)                                            /**<  (TRNG_INTENCLR) Interrupt Enable Clear  Reset Value */

#define TRNG_INTENCLR_DATARDY_Pos             _U_(0)                                               /**< (TRNG_INTENCLR) Data Ready Interrupt Enable Position */
#define TRNG_INTENCLR_DATARDY_Msk             (_U_(0x1) << TRNG_INTENCLR_DATARDY_Pos)              /**< (TRNG_INTENCLR) Data Ready Interrupt Enable Mask */
#define TRNG_INTENCLR_DATARDY(value)          (TRNG_INTENCLR_DATARDY_Msk & ((value) << TRNG_INTENCLR_DATARDY_Pos))
#define TRNG_INTENCLR_Msk                     _U_(0x01)                                            /**< (TRNG_INTENCLR) Register Mask  */


/* -------- TRNG_INTENSET : (TRNG Offset: 0x09) (R/W 8) Interrupt Enable Set -------- */
#define TRNG_INTENSET_RESETVALUE              _U_(0x00)                                            /**<  (TRNG_INTENSET) Interrupt Enable Set  Reset Value */

#define TRNG_INTENSET_DATARDY_Pos             _U_(0)                                               /**< (TRNG_INTENSET) Data Ready Interrupt Enable Position */
#define TRNG_INTENSET_DATARDY_Msk             (_U_(0x1) << TRNG_INTENSET_DATARDY_Pos)              /**< (TRNG_INTENSET) Data Ready Interrupt Enable Mask */
#define TRNG_INTENSET_DATARDY(value)          (TRNG_INTENSET_DATARDY_Msk & ((value) << TRNG_INTENSET_DATARDY_Pos))
#define TRNG_INTENSET_Msk                     _U_(0x01)                                            /**< (TRNG_INTENSET) Register Mask  */


/* -------- TRNG_INTFLAG : (TRNG Offset: 0x0A) (R/W 8) Interrupt Flag Status and Clear -------- */
#define TRNG_INTFLAG_RESETVALUE               _U_(0x00)                                            /**<  (TRNG_INTFLAG) Interrupt Flag Status and Clear  Reset Value */

#define TRNG_INTFLAG_DATARDY_Pos              _U_(0)                                               /**< (TRNG_INTFLAG) Data Ready Interrupt Flag Position */
#define TRNG_INTFLAG_DATARDY_Msk              (_U_(0x1) << TRNG_INTFLAG_DATARDY_Pos)               /**< (TRNG_INTFLAG) Data Ready Interrupt Flag Mask */
#define TRNG_INTFLAG_DATARDY(value)           (TRNG_INTFLAG_DATARDY_Msk & ((value) << TRNG_INTFLAG_DATARDY_Pos))
#define TRNG_INTFLAG_Msk                      _U_(0x01)                                            /**< (TRNG_INTFLAG) Register Mask  */


/* -------- TRNG_DATA : (TRNG Offset: 0x20) ( R/ 32) Output Data -------- */
#define TRNG_DATA_RESETVALUE                  _U_(0x00)                                            /**<  (TRNG_DATA) Output Data  Reset Value */

#define TRNG_DATA_DATA_Pos                    _U_(0)                                               /**< (TRNG_DATA) Output Data Position */
#define TRNG_DATA_DATA_Msk                    (_U_(0xFFFFFFFF) << TRNG_DATA_DATA_Pos)              /**< (TRNG_DATA) Output Data Mask */
#define TRNG_DATA_DATA(value)                 (TRNG_DATA_DATA_Msk & ((value) << TRNG_DATA_DATA_Pos))
#define TRNG_DATA_Msk                         _U_(0xFFFFFFFF)                                      /**< (TRNG_DATA) Register Mask  */


/** \brief TRNG register offsets definitions */
#define TRNG_CTRLA_REG_OFST            (0x00)              /**< (TRNG_CTRLA) Control A Offset */
#define TRNG_EVCTRL_REG_OFST           (0x04)              /**< (TRNG_EVCTRL) Event Control Offset */
#define TRNG_INTENCLR_REG_OFST         (0x08)              /**< (TRNG_INTENCLR) Interrupt Enable Clear Offset */
#define TRNG_INTENSET_REG_OFST         (0x09)              /**< (TRNG_INTENSET) Interrupt Enable Set Offset */
#define TRNG_INTFLAG_REG_OFST          (0x0A)              /**< (TRNG_INTFLAG) Interrupt Flag Status and Clear Offset */
#define TRNG_DATA_REG_OFST             (0x20)              /**< (TRNG_DATA) Output Data Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief TRNG register API structure */
typedef struct
{  /* True Random Generator */
  __IO  uint8_t                        TRNG_CTRLA;         /**< Offset: 0x00 (R/W  8) Control A */
  __I   uint8_t                        Reserved1[0x03];
  __IO  uint8_t                        TRNG_EVCTRL;        /**< Offset: 0x04 (R/W  8) Event Control */
  __I   uint8_t                        Reserved2[0x03];
  __IO  uint8_t                        TRNG_INTENCLR;      /**< Offset: 0x08 (R/W  8) Interrupt Enable Clear */
  __IO  uint8_t                        TRNG_INTENSET;      /**< Offset: 0x09 (R/W  8) Interrupt Enable Set */
  __IO  uint8_t                        TRNG_INTFLAG;       /**< Offset: 0x0A (R/W  8) Interrupt Flag Status and Clear */
  __I   uint8_t                        Reserved3[0x15];
  __I   uint32_t                       TRNG_DATA;          /**< Offset: 0x20 (R/   32) Output Data */
} trng_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_TRNG_COMPONENT_H_ */
