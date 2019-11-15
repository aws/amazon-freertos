/**
 * \brief Component description for HMATRIXB
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
#ifndef _SAME54_HMATRIXB_COMPONENT_H_
#define _SAME54_HMATRIXB_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR HMATRIXB                                     */
/* ************************************************************************** */

/* -------- HMATRIXB_PRAS : (HMATRIXB Offset: 0x00) (R/W 32) Priority A for Slave -------- */
#define HMATRIXB_PRAS_RESETVALUE              _U_(0x00)                                            /**<  (HMATRIXB_PRAS) Priority A for Slave  Reset Value */

#define HMATRIXB_PRAS_Msk                     _U_(0x00000000)                                      /**< (HMATRIXB_PRAS) Register Mask  */


/* -------- HMATRIXB_PRBS : (HMATRIXB Offset: 0x04) (R/W 32) Priority B for Slave -------- */
#define HMATRIXB_PRBS_RESETVALUE              _U_(0x00)                                            /**<  (HMATRIXB_PRBS) Priority B for Slave  Reset Value */

#define HMATRIXB_PRBS_Msk                     _U_(0x00000000)                                      /**< (HMATRIXB_PRBS) Register Mask  */


/** \brief HMATRIXB register offsets definitions */
#define HMATRIXB_PRAS_REG_OFST         (0x00)              /**< (HMATRIXB_PRAS) Priority A for Slave Offset */
#define HMATRIXB_PRBS_REG_OFST         (0x04)              /**< (HMATRIXB_PRBS) Priority B for Slave Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief PRS register API structure */
typedef struct
{
  __IO  uint32_t                       HMATRIXB_PRAS;      /**< Offset: 0x00 (R/W  32) Priority A for Slave */
  __IO  uint32_t                       HMATRIXB_PRBS;      /**< Offset: 0x04 (R/W  32) Priority B for Slave */
} hmatrixb_prs_registers_t;

#define PRS_NUMBER _U_(16)

/** \brief HMATRIXB register API structure */
typedef struct
{  /* HSB Matrix */
  __I   uint8_t                        Reserved1[0x80];
        hmatrixb_prs_registers_t       PRS[PRS_NUMBER]; /**< Offset: 0x80  */
} hmatrixb_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_HMATRIXB_COMPONENT_H_ */
