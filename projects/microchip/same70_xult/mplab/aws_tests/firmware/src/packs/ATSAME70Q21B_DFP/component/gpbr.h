/**
 * \brief Component description for GPBR
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
#ifndef _SAME70_GPBR_COMPONENT_H_
#define _SAME70_GPBR_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR GPBR                                         */
/* ************************************************************************** */

/* -------- SYS_GPBR : (GPBR Offset: 0x00) (R/W 32) General Purpose Backup Register 0 -------- */
#define SYS_GPBR_GPBR_VALUE_Pos               _U_(0)                                               /**< (SYS_GPBR) Value of GPBR x Position */
#define SYS_GPBR_GPBR_VALUE_Msk               (_U_(0xFFFFFFFF) << SYS_GPBR_GPBR_VALUE_Pos)         /**< (SYS_GPBR) Value of GPBR x Mask */
#define SYS_GPBR_GPBR_VALUE(value)            (SYS_GPBR_GPBR_VALUE_Msk & ((value) << SYS_GPBR_GPBR_VALUE_Pos))
#define SYS_GPBR_Msk                          _U_(0xFFFFFFFF)                                      /**< (SYS_GPBR) Register Mask  */


/** \brief GPBR register offsets definitions */
#define SYS_GPBR_REG_OFST              (0x00)              /**< (SYS_GPBR) General Purpose Backup Register 0 Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief GPBR register API structure */
typedef struct
{
  __IO  uint32_t                       SYS_GPBR[8];        /**< Offset: 0x00 (R/W  32) General Purpose Backup Register 0 */
} gpbr_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_GPBR_COMPONENT_H_ */
