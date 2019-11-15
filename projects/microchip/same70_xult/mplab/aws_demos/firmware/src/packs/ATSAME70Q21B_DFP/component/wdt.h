/**
 * \brief Component description for WDT
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
#ifndef _SAME70_WDT_COMPONENT_H_
#define _SAME70_WDT_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR WDT                                          */
/* ************************************************************************** */

/* -------- WDT_CR : (WDT Offset: 0x00) ( /W 32) Control Register -------- */
#define WDT_CR_WDRSTT_Pos                     _U_(0)                                               /**< (WDT_CR) Watchdog Restart Position */
#define WDT_CR_WDRSTT_Msk                     (_U_(0x1) << WDT_CR_WDRSTT_Pos)                      /**< (WDT_CR) Watchdog Restart Mask */
#define WDT_CR_WDRSTT(value)                  (WDT_CR_WDRSTT_Msk & ((value) << WDT_CR_WDRSTT_Pos))
#define WDT_CR_KEY_Pos                        _U_(24)                                              /**< (WDT_CR) Password Position */
#define WDT_CR_KEY_Msk                        (_U_(0xFF) << WDT_CR_KEY_Pos)                        /**< (WDT_CR) Password Mask */
#define WDT_CR_KEY(value)                     (WDT_CR_KEY_Msk & ((value) << WDT_CR_KEY_Pos))      
#define   WDT_CR_KEY_PASSWD_Val               _U_(0xA5)                                            /**< (WDT_CR) Writing any other value in this field aborts the write operation.  */
#define WDT_CR_KEY_PASSWD                     (WDT_CR_KEY_PASSWD_Val << WDT_CR_KEY_Pos)            /**< (WDT_CR) Writing any other value in this field aborts the write operation. Position  */
#define WDT_CR_Msk                            _U_(0xFF000001)                                      /**< (WDT_CR) Register Mask  */


/* -------- WDT_MR : (WDT Offset: 0x04) (R/W 32) Mode Register -------- */
#define WDT_MR_WDV_Pos                        _U_(0)                                               /**< (WDT_MR) Watchdog Counter Value Position */
#define WDT_MR_WDV_Msk                        (_U_(0xFFF) << WDT_MR_WDV_Pos)                       /**< (WDT_MR) Watchdog Counter Value Mask */
#define WDT_MR_WDV(value)                     (WDT_MR_WDV_Msk & ((value) << WDT_MR_WDV_Pos))      
#define WDT_MR_WDFIEN_Pos                     _U_(12)                                              /**< (WDT_MR) Watchdog Fault Interrupt Enable Position */
#define WDT_MR_WDFIEN_Msk                     (_U_(0x1) << WDT_MR_WDFIEN_Pos)                      /**< (WDT_MR) Watchdog Fault Interrupt Enable Mask */
#define WDT_MR_WDFIEN(value)                  (WDT_MR_WDFIEN_Msk & ((value) << WDT_MR_WDFIEN_Pos))
#define WDT_MR_WDRSTEN_Pos                    _U_(13)                                              /**< (WDT_MR) Watchdog Reset Enable Position */
#define WDT_MR_WDRSTEN_Msk                    (_U_(0x1) << WDT_MR_WDRSTEN_Pos)                     /**< (WDT_MR) Watchdog Reset Enable Mask */
#define WDT_MR_WDRSTEN(value)                 (WDT_MR_WDRSTEN_Msk & ((value) << WDT_MR_WDRSTEN_Pos))
#define WDT_MR_WDDIS_Pos                      _U_(15)                                              /**< (WDT_MR) Watchdog Disable Position */
#define WDT_MR_WDDIS_Msk                      (_U_(0x1) << WDT_MR_WDDIS_Pos)                       /**< (WDT_MR) Watchdog Disable Mask */
#define WDT_MR_WDDIS(value)                   (WDT_MR_WDDIS_Msk & ((value) << WDT_MR_WDDIS_Pos))  
#define WDT_MR_WDD_Pos                        _U_(16)                                              /**< (WDT_MR) Watchdog Delta Value Position */
#define WDT_MR_WDD_Msk                        (_U_(0xFFF) << WDT_MR_WDD_Pos)                       /**< (WDT_MR) Watchdog Delta Value Mask */
#define WDT_MR_WDD(value)                     (WDT_MR_WDD_Msk & ((value) << WDT_MR_WDD_Pos))      
#define WDT_MR_WDDBGHLT_Pos                   _U_(28)                                              /**< (WDT_MR) Watchdog Debug Halt Position */
#define WDT_MR_WDDBGHLT_Msk                   (_U_(0x1) << WDT_MR_WDDBGHLT_Pos)                    /**< (WDT_MR) Watchdog Debug Halt Mask */
#define WDT_MR_WDDBGHLT(value)                (WDT_MR_WDDBGHLT_Msk & ((value) << WDT_MR_WDDBGHLT_Pos))
#define WDT_MR_WDIDLEHLT_Pos                  _U_(29)                                              /**< (WDT_MR) Watchdog Idle Halt Position */
#define WDT_MR_WDIDLEHLT_Msk                  (_U_(0x1) << WDT_MR_WDIDLEHLT_Pos)                   /**< (WDT_MR) Watchdog Idle Halt Mask */
#define WDT_MR_WDIDLEHLT(value)               (WDT_MR_WDIDLEHLT_Msk & ((value) << WDT_MR_WDIDLEHLT_Pos))
#define WDT_MR_Msk                            _U_(0x3FFFBFFF)                                      /**< (WDT_MR) Register Mask  */


/* -------- WDT_SR : (WDT Offset: 0x08) ( R/ 32) Status Register -------- */
#define WDT_SR_WDUNF_Pos                      _U_(0)                                               /**< (WDT_SR) Watchdog Underflow (cleared on read) Position */
#define WDT_SR_WDUNF_Msk                      (_U_(0x1) << WDT_SR_WDUNF_Pos)                       /**< (WDT_SR) Watchdog Underflow (cleared on read) Mask */
#define WDT_SR_WDUNF(value)                   (WDT_SR_WDUNF_Msk & ((value) << WDT_SR_WDUNF_Pos))  
#define WDT_SR_WDERR_Pos                      _U_(1)                                               /**< (WDT_SR) Watchdog Error (cleared on read) Position */
#define WDT_SR_WDERR_Msk                      (_U_(0x1) << WDT_SR_WDERR_Pos)                       /**< (WDT_SR) Watchdog Error (cleared on read) Mask */
#define WDT_SR_WDERR(value)                   (WDT_SR_WDERR_Msk & ((value) << WDT_SR_WDERR_Pos))  
#define WDT_SR_Msk                            _U_(0x00000003)                                      /**< (WDT_SR) Register Mask  */


/** \brief WDT register offsets definitions */
#define WDT_CR_REG_OFST                (0x00)              /**< (WDT_CR) Control Register Offset */
#define WDT_MR_REG_OFST                (0x04)              /**< (WDT_MR) Mode Register Offset */
#define WDT_SR_REG_OFST                (0x08)              /**< (WDT_SR) Status Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief WDT register API structure */
typedef struct
{
  __O   uint32_t                       WDT_CR;             /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       WDT_MR;             /**< Offset: 0x04 (R/W  32) Mode Register */
  __I   uint32_t                       WDT_SR;             /**< Offset: 0x08 (R/   32) Status Register */
} wdt_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_WDT_COMPONENT_H_ */
