/**
 * \brief Component description for RSWDT
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
#ifndef _SAME70_RSWDT_COMPONENT_H_
#define _SAME70_RSWDT_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR RSWDT                                        */
/* ************************************************************************** */

/* -------- RSWDT_CR : (RSWDT Offset: 0x00) ( /W 32) Control Register -------- */
#define RSWDT_CR_WDRSTT_Pos                   _U_(0)                                               /**< (RSWDT_CR) Watchdog Restart Position */
#define RSWDT_CR_WDRSTT_Msk                   (_U_(0x1) << RSWDT_CR_WDRSTT_Pos)                    /**< (RSWDT_CR) Watchdog Restart Mask */
#define RSWDT_CR_WDRSTT(value)                (RSWDT_CR_WDRSTT_Msk & ((value) << RSWDT_CR_WDRSTT_Pos))
#define RSWDT_CR_KEY_Pos                      _U_(24)                                              /**< (RSWDT_CR) Password Position */
#define RSWDT_CR_KEY_Msk                      (_U_(0xFF) << RSWDT_CR_KEY_Pos)                      /**< (RSWDT_CR) Password Mask */
#define RSWDT_CR_KEY(value)                   (RSWDT_CR_KEY_Msk & ((value) << RSWDT_CR_KEY_Pos))  
#define   RSWDT_CR_KEY_PASSWD_Val             _U_(0xC4)                                            /**< (RSWDT_CR) Writing any other value in this field aborts the write operation.  */
#define RSWDT_CR_KEY_PASSWD                   (RSWDT_CR_KEY_PASSWD_Val << RSWDT_CR_KEY_Pos)        /**< (RSWDT_CR) Writing any other value in this field aborts the write operation. Position  */
#define RSWDT_CR_Msk                          _U_(0xFF000001)                                      /**< (RSWDT_CR) Register Mask  */


/* -------- RSWDT_MR : (RSWDT Offset: 0x04) (R/W 32) Mode Register -------- */
#define RSWDT_MR_WDV_Pos                      _U_(0)                                               /**< (RSWDT_MR) Watchdog Counter Value Position */
#define RSWDT_MR_WDV_Msk                      (_U_(0xFFF) << RSWDT_MR_WDV_Pos)                     /**< (RSWDT_MR) Watchdog Counter Value Mask */
#define RSWDT_MR_WDV(value)                   (RSWDT_MR_WDV_Msk & ((value) << RSWDT_MR_WDV_Pos))  
#define RSWDT_MR_WDFIEN_Pos                   _U_(12)                                              /**< (RSWDT_MR) Watchdog Fault Interrupt Enable Position */
#define RSWDT_MR_WDFIEN_Msk                   (_U_(0x1) << RSWDT_MR_WDFIEN_Pos)                    /**< (RSWDT_MR) Watchdog Fault Interrupt Enable Mask */
#define RSWDT_MR_WDFIEN(value)                (RSWDT_MR_WDFIEN_Msk & ((value) << RSWDT_MR_WDFIEN_Pos))
#define RSWDT_MR_WDRSTEN_Pos                  _U_(13)                                              /**< (RSWDT_MR) Watchdog Reset Enable Position */
#define RSWDT_MR_WDRSTEN_Msk                  (_U_(0x1) << RSWDT_MR_WDRSTEN_Pos)                   /**< (RSWDT_MR) Watchdog Reset Enable Mask */
#define RSWDT_MR_WDRSTEN(value)               (RSWDT_MR_WDRSTEN_Msk & ((value) << RSWDT_MR_WDRSTEN_Pos))
#define RSWDT_MR_WDDIS_Pos                    _U_(15)                                              /**< (RSWDT_MR) Watchdog Disable Position */
#define RSWDT_MR_WDDIS_Msk                    (_U_(0x1) << RSWDT_MR_WDDIS_Pos)                     /**< (RSWDT_MR) Watchdog Disable Mask */
#define RSWDT_MR_WDDIS(value)                 (RSWDT_MR_WDDIS_Msk & ((value) << RSWDT_MR_WDDIS_Pos))
#define RSWDT_MR_ALLONES_Pos                  _U_(16)                                              /**< (RSWDT_MR) Must Always Be Written with 0xFFF Position */
#define RSWDT_MR_ALLONES_Msk                  (_U_(0xFFF) << RSWDT_MR_ALLONES_Pos)                 /**< (RSWDT_MR) Must Always Be Written with 0xFFF Mask */
#define RSWDT_MR_ALLONES(value)               (RSWDT_MR_ALLONES_Msk & ((value) << RSWDT_MR_ALLONES_Pos))
#define RSWDT_MR_WDDBGHLT_Pos                 _U_(28)                                              /**< (RSWDT_MR) Watchdog Debug Halt Position */
#define RSWDT_MR_WDDBGHLT_Msk                 (_U_(0x1) << RSWDT_MR_WDDBGHLT_Pos)                  /**< (RSWDT_MR) Watchdog Debug Halt Mask */
#define RSWDT_MR_WDDBGHLT(value)              (RSWDT_MR_WDDBGHLT_Msk & ((value) << RSWDT_MR_WDDBGHLT_Pos))
#define RSWDT_MR_WDIDLEHLT_Pos                _U_(29)                                              /**< (RSWDT_MR) Watchdog Idle Halt Position */
#define RSWDT_MR_WDIDLEHLT_Msk                (_U_(0x1) << RSWDT_MR_WDIDLEHLT_Pos)                 /**< (RSWDT_MR) Watchdog Idle Halt Mask */
#define RSWDT_MR_WDIDLEHLT(value)             (RSWDT_MR_WDIDLEHLT_Msk & ((value) << RSWDT_MR_WDIDLEHLT_Pos))
#define RSWDT_MR_Msk                          _U_(0x3FFFBFFF)                                      /**< (RSWDT_MR) Register Mask  */


/* -------- RSWDT_SR : (RSWDT Offset: 0x08) ( R/ 32) Status Register -------- */
#define RSWDT_SR_WDUNF_Pos                    _U_(0)                                               /**< (RSWDT_SR) Watchdog Underflow Position */
#define RSWDT_SR_WDUNF_Msk                    (_U_(0x1) << RSWDT_SR_WDUNF_Pos)                     /**< (RSWDT_SR) Watchdog Underflow Mask */
#define RSWDT_SR_WDUNF(value)                 (RSWDT_SR_WDUNF_Msk & ((value) << RSWDT_SR_WDUNF_Pos))
#define RSWDT_SR_Msk                          _U_(0x00000001)                                      /**< (RSWDT_SR) Register Mask  */


/** \brief RSWDT register offsets definitions */
#define RSWDT_CR_REG_OFST              (0x00)              /**< (RSWDT_CR) Control Register Offset */
#define RSWDT_MR_REG_OFST              (0x04)              /**< (RSWDT_MR) Mode Register Offset */
#define RSWDT_SR_REG_OFST              (0x08)              /**< (RSWDT_SR) Status Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief RSWDT register API structure */
typedef struct
{
  __O   uint32_t                       RSWDT_CR;           /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       RSWDT_MR;           /**< Offset: 0x04 (R/W  32) Mode Register */
  __I   uint32_t                       RSWDT_SR;           /**< Offset: 0x08 (R/   32) Status Register */
} rswdt_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_RSWDT_COMPONENT_H_ */
