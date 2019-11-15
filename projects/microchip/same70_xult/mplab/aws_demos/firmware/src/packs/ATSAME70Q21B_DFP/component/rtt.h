/**
 * \brief Component description for RTT
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
#ifndef _SAME70_RTT_COMPONENT_H_
#define _SAME70_RTT_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR RTT                                          */
/* ************************************************************************** */

/* -------- RTT_MR : (RTT Offset: 0x00) (R/W 32) Mode Register -------- */
#define RTT_MR_RTPRES_Pos                     _U_(0)                                               /**< (RTT_MR) Real-time Timer Prescaler Value Position */
#define RTT_MR_RTPRES_Msk                     (_U_(0xFFFF) << RTT_MR_RTPRES_Pos)                   /**< (RTT_MR) Real-time Timer Prescaler Value Mask */
#define RTT_MR_RTPRES(value)                  (RTT_MR_RTPRES_Msk & ((value) << RTT_MR_RTPRES_Pos))
#define RTT_MR_ALMIEN_Pos                     _U_(16)                                              /**< (RTT_MR) Alarm Interrupt Enable Position */
#define RTT_MR_ALMIEN_Msk                     (_U_(0x1) << RTT_MR_ALMIEN_Pos)                      /**< (RTT_MR) Alarm Interrupt Enable Mask */
#define RTT_MR_ALMIEN(value)                  (RTT_MR_ALMIEN_Msk & ((value) << RTT_MR_ALMIEN_Pos))
#define RTT_MR_RTTINCIEN_Pos                  _U_(17)                                              /**< (RTT_MR) Real-time Timer Increment Interrupt Enable Position */
#define RTT_MR_RTTINCIEN_Msk                  (_U_(0x1) << RTT_MR_RTTINCIEN_Pos)                   /**< (RTT_MR) Real-time Timer Increment Interrupt Enable Mask */
#define RTT_MR_RTTINCIEN(value)               (RTT_MR_RTTINCIEN_Msk & ((value) << RTT_MR_RTTINCIEN_Pos))
#define RTT_MR_RTTRST_Pos                     _U_(18)                                              /**< (RTT_MR) Real-time Timer Restart Position */
#define RTT_MR_RTTRST_Msk                     (_U_(0x1) << RTT_MR_RTTRST_Pos)                      /**< (RTT_MR) Real-time Timer Restart Mask */
#define RTT_MR_RTTRST(value)                  (RTT_MR_RTTRST_Msk & ((value) << RTT_MR_RTTRST_Pos))
#define RTT_MR_RTTDIS_Pos                     _U_(20)                                              /**< (RTT_MR) Real-time Timer Disable Position */
#define RTT_MR_RTTDIS_Msk                     (_U_(0x1) << RTT_MR_RTTDIS_Pos)                      /**< (RTT_MR) Real-time Timer Disable Mask */
#define RTT_MR_RTTDIS(value)                  (RTT_MR_RTTDIS_Msk & ((value) << RTT_MR_RTTDIS_Pos))
#define RTT_MR_RTC1HZ_Pos                     _U_(24)                                              /**< (RTT_MR) Real-Time Clock 1Hz Clock Selection Position */
#define RTT_MR_RTC1HZ_Msk                     (_U_(0x1) << RTT_MR_RTC1HZ_Pos)                      /**< (RTT_MR) Real-Time Clock 1Hz Clock Selection Mask */
#define RTT_MR_RTC1HZ(value)                  (RTT_MR_RTC1HZ_Msk & ((value) << RTT_MR_RTC1HZ_Pos))
#define RTT_MR_Msk                            _U_(0x0117FFFF)                                      /**< (RTT_MR) Register Mask  */


/* -------- RTT_AR : (RTT Offset: 0x04) (R/W 32) Alarm Register -------- */
#define RTT_AR_ALMV_Pos                       _U_(0)                                               /**< (RTT_AR) Alarm Value Position */
#define RTT_AR_ALMV_Msk                       (_U_(0xFFFFFFFF) << RTT_AR_ALMV_Pos)                 /**< (RTT_AR) Alarm Value Mask */
#define RTT_AR_ALMV(value)                    (RTT_AR_ALMV_Msk & ((value) << RTT_AR_ALMV_Pos))    
#define RTT_AR_Msk                            _U_(0xFFFFFFFF)                                      /**< (RTT_AR) Register Mask  */


/* -------- RTT_VR : (RTT Offset: 0x08) ( R/ 32) Value Register -------- */
#define RTT_VR_CRTV_Pos                       _U_(0)                                               /**< (RTT_VR) Current Real-time Value Position */
#define RTT_VR_CRTV_Msk                       (_U_(0xFFFFFFFF) << RTT_VR_CRTV_Pos)                 /**< (RTT_VR) Current Real-time Value Mask */
#define RTT_VR_CRTV(value)                    (RTT_VR_CRTV_Msk & ((value) << RTT_VR_CRTV_Pos))    
#define RTT_VR_Msk                            _U_(0xFFFFFFFF)                                      /**< (RTT_VR) Register Mask  */


/* -------- RTT_SR : (RTT Offset: 0x0C) ( R/ 32) Status Register -------- */
#define RTT_SR_ALMS_Pos                       _U_(0)                                               /**< (RTT_SR) Real-time Alarm Status (cleared on read) Position */
#define RTT_SR_ALMS_Msk                       (_U_(0x1) << RTT_SR_ALMS_Pos)                        /**< (RTT_SR) Real-time Alarm Status (cleared on read) Mask */
#define RTT_SR_ALMS(value)                    (RTT_SR_ALMS_Msk & ((value) << RTT_SR_ALMS_Pos))    
#define RTT_SR_RTTINC_Pos                     _U_(1)                                               /**< (RTT_SR) Prescaler Roll-over Status (cleared on read) Position */
#define RTT_SR_RTTINC_Msk                     (_U_(0x1) << RTT_SR_RTTINC_Pos)                      /**< (RTT_SR) Prescaler Roll-over Status (cleared on read) Mask */
#define RTT_SR_RTTINC(value)                  (RTT_SR_RTTINC_Msk & ((value) << RTT_SR_RTTINC_Pos))
#define RTT_SR_Msk                            _U_(0x00000003)                                      /**< (RTT_SR) Register Mask  */


/** \brief RTT register offsets definitions */
#define RTT_MR_REG_OFST                (0x00)              /**< (RTT_MR) Mode Register Offset */
#define RTT_AR_REG_OFST                (0x04)              /**< (RTT_AR) Alarm Register Offset */
#define RTT_VR_REG_OFST                (0x08)              /**< (RTT_VR) Value Register Offset */
#define RTT_SR_REG_OFST                (0x0C)              /**< (RTT_SR) Status Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief RTT register API structure */
typedef struct
{
  __IO  uint32_t                       RTT_MR;             /**< Offset: 0x00 (R/W  32) Mode Register */
  __IO  uint32_t                       RTT_AR;             /**< Offset: 0x04 (R/W  32) Alarm Register */
  __I   uint32_t                       RTT_VR;             /**< Offset: 0x08 (R/   32) Value Register */
  __I   uint32_t                       RTT_SR;             /**< Offset: 0x0C (R/   32) Status Register */
} rtt_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_RTT_COMPONENT_H_ */
