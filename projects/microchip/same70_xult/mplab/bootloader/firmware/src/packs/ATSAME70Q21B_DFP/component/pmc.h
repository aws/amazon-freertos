/**
 * \brief Component description for PMC
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
#ifndef _SAME70_PMC_COMPONENT_H_
#define _SAME70_PMC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR PMC                                          */
/* ************************************************************************** */

/* -------- PMC_SCER : (PMC Offset: 0x00) ( /W 32) System Clock Enable Register -------- */
#define PMC_SCER_USBCLK_Pos                   _U_(5)                                               /**< (PMC_SCER) Enable USB FS Clock Position */
#define PMC_SCER_USBCLK_Msk                   (_U_(0x1) << PMC_SCER_USBCLK_Pos)                    /**< (PMC_SCER) Enable USB FS Clock Mask */
#define PMC_SCER_USBCLK(value)                (PMC_SCER_USBCLK_Msk & ((value) << PMC_SCER_USBCLK_Pos))
#define PMC_SCER_PCK0_Pos                     _U_(8)                                               /**< (PMC_SCER) Programmable Clock 0 Output Enable Position */
#define PMC_SCER_PCK0_Msk                     (_U_(0x1) << PMC_SCER_PCK0_Pos)                      /**< (PMC_SCER) Programmable Clock 0 Output Enable Mask */
#define PMC_SCER_PCK0(value)                  (PMC_SCER_PCK0_Msk & ((value) << PMC_SCER_PCK0_Pos))
#define PMC_SCER_PCK1_Pos                     _U_(9)                                               /**< (PMC_SCER) Programmable Clock 1 Output Enable Position */
#define PMC_SCER_PCK1_Msk                     (_U_(0x1) << PMC_SCER_PCK1_Pos)                      /**< (PMC_SCER) Programmable Clock 1 Output Enable Mask */
#define PMC_SCER_PCK1(value)                  (PMC_SCER_PCK1_Msk & ((value) << PMC_SCER_PCK1_Pos))
#define PMC_SCER_PCK2_Pos                     _U_(10)                                              /**< (PMC_SCER) Programmable Clock 2 Output Enable Position */
#define PMC_SCER_PCK2_Msk                     (_U_(0x1) << PMC_SCER_PCK2_Pos)                      /**< (PMC_SCER) Programmable Clock 2 Output Enable Mask */
#define PMC_SCER_PCK2(value)                  (PMC_SCER_PCK2_Msk & ((value) << PMC_SCER_PCK2_Pos))
#define PMC_SCER_PCK3_Pos                     _U_(11)                                              /**< (PMC_SCER) Programmable Clock 3 Output Enable Position */
#define PMC_SCER_PCK3_Msk                     (_U_(0x1) << PMC_SCER_PCK3_Pos)                      /**< (PMC_SCER) Programmable Clock 3 Output Enable Mask */
#define PMC_SCER_PCK3(value)                  (PMC_SCER_PCK3_Msk & ((value) << PMC_SCER_PCK3_Pos))
#define PMC_SCER_PCK4_Pos                     _U_(12)                                              /**< (PMC_SCER) Programmable Clock 4 Output Enable Position */
#define PMC_SCER_PCK4_Msk                     (_U_(0x1) << PMC_SCER_PCK4_Pos)                      /**< (PMC_SCER) Programmable Clock 4 Output Enable Mask */
#define PMC_SCER_PCK4(value)                  (PMC_SCER_PCK4_Msk & ((value) << PMC_SCER_PCK4_Pos))
#define PMC_SCER_PCK5_Pos                     _U_(13)                                              /**< (PMC_SCER) Programmable Clock 5 Output Enable Position */
#define PMC_SCER_PCK5_Msk                     (_U_(0x1) << PMC_SCER_PCK5_Pos)                      /**< (PMC_SCER) Programmable Clock 5 Output Enable Mask */
#define PMC_SCER_PCK5(value)                  (PMC_SCER_PCK5_Msk & ((value) << PMC_SCER_PCK5_Pos))
#define PMC_SCER_PCK6_Pos                     _U_(14)                                              /**< (PMC_SCER) Programmable Clock 6 Output Enable Position */
#define PMC_SCER_PCK6_Msk                     (_U_(0x1) << PMC_SCER_PCK6_Pos)                      /**< (PMC_SCER) Programmable Clock 6 Output Enable Mask */
#define PMC_SCER_PCK6(value)                  (PMC_SCER_PCK6_Msk & ((value) << PMC_SCER_PCK6_Pos))
#define PMC_SCER_PCK7_Pos                     _U_(15)                                              /**< (PMC_SCER) Programmable Clock 7 Output Enable Position */
#define PMC_SCER_PCK7_Msk                     (_U_(0x1) << PMC_SCER_PCK7_Pos)                      /**< (PMC_SCER) Programmable Clock 7 Output Enable Mask */
#define PMC_SCER_PCK7(value)                  (PMC_SCER_PCK7_Msk & ((value) << PMC_SCER_PCK7_Pos))
#define PMC_SCER_Msk                          _U_(0x0000FF20)                                      /**< (PMC_SCER) Register Mask  */

#define PMC_SCER_PCK_Pos                      _U_(8)                                               /**< (PMC_SCER Position) Programmable Clock 7 Output Enable */
#define PMC_SCER_PCK_Msk                      (_U_(0xFF) << PMC_SCER_PCK_Pos)                      /**< (PMC_SCER Mask) PCK */
#define PMC_SCER_PCK(value)                   (PMC_SCER_PCK_Msk & ((value) << PMC_SCER_PCK_Pos))   

/* -------- PMC_SCDR : (PMC Offset: 0x04) ( /W 32) System Clock Disable Register -------- */
#define PMC_SCDR_USBCLK_Pos                   _U_(5)                                               /**< (PMC_SCDR) Disable USB FS Clock Position */
#define PMC_SCDR_USBCLK_Msk                   (_U_(0x1) << PMC_SCDR_USBCLK_Pos)                    /**< (PMC_SCDR) Disable USB FS Clock Mask */
#define PMC_SCDR_USBCLK(value)                (PMC_SCDR_USBCLK_Msk & ((value) << PMC_SCDR_USBCLK_Pos))
#define PMC_SCDR_PCK0_Pos                     _U_(8)                                               /**< (PMC_SCDR) Programmable Clock 0 Output Disable Position */
#define PMC_SCDR_PCK0_Msk                     (_U_(0x1) << PMC_SCDR_PCK0_Pos)                      /**< (PMC_SCDR) Programmable Clock 0 Output Disable Mask */
#define PMC_SCDR_PCK0(value)                  (PMC_SCDR_PCK0_Msk & ((value) << PMC_SCDR_PCK0_Pos))
#define PMC_SCDR_PCK1_Pos                     _U_(9)                                               /**< (PMC_SCDR) Programmable Clock 1 Output Disable Position */
#define PMC_SCDR_PCK1_Msk                     (_U_(0x1) << PMC_SCDR_PCK1_Pos)                      /**< (PMC_SCDR) Programmable Clock 1 Output Disable Mask */
#define PMC_SCDR_PCK1(value)                  (PMC_SCDR_PCK1_Msk & ((value) << PMC_SCDR_PCK1_Pos))
#define PMC_SCDR_PCK2_Pos                     _U_(10)                                              /**< (PMC_SCDR) Programmable Clock 2 Output Disable Position */
#define PMC_SCDR_PCK2_Msk                     (_U_(0x1) << PMC_SCDR_PCK2_Pos)                      /**< (PMC_SCDR) Programmable Clock 2 Output Disable Mask */
#define PMC_SCDR_PCK2(value)                  (PMC_SCDR_PCK2_Msk & ((value) << PMC_SCDR_PCK2_Pos))
#define PMC_SCDR_PCK3_Pos                     _U_(11)                                              /**< (PMC_SCDR) Programmable Clock 3 Output Disable Position */
#define PMC_SCDR_PCK3_Msk                     (_U_(0x1) << PMC_SCDR_PCK3_Pos)                      /**< (PMC_SCDR) Programmable Clock 3 Output Disable Mask */
#define PMC_SCDR_PCK3(value)                  (PMC_SCDR_PCK3_Msk & ((value) << PMC_SCDR_PCK3_Pos))
#define PMC_SCDR_PCK4_Pos                     _U_(12)                                              /**< (PMC_SCDR) Programmable Clock 4 Output Disable Position */
#define PMC_SCDR_PCK4_Msk                     (_U_(0x1) << PMC_SCDR_PCK4_Pos)                      /**< (PMC_SCDR) Programmable Clock 4 Output Disable Mask */
#define PMC_SCDR_PCK4(value)                  (PMC_SCDR_PCK4_Msk & ((value) << PMC_SCDR_PCK4_Pos))
#define PMC_SCDR_PCK5_Pos                     _U_(13)                                              /**< (PMC_SCDR) Programmable Clock 5 Output Disable Position */
#define PMC_SCDR_PCK5_Msk                     (_U_(0x1) << PMC_SCDR_PCK5_Pos)                      /**< (PMC_SCDR) Programmable Clock 5 Output Disable Mask */
#define PMC_SCDR_PCK5(value)                  (PMC_SCDR_PCK5_Msk & ((value) << PMC_SCDR_PCK5_Pos))
#define PMC_SCDR_PCK6_Pos                     _U_(14)                                              /**< (PMC_SCDR) Programmable Clock 6 Output Disable Position */
#define PMC_SCDR_PCK6_Msk                     (_U_(0x1) << PMC_SCDR_PCK6_Pos)                      /**< (PMC_SCDR) Programmable Clock 6 Output Disable Mask */
#define PMC_SCDR_PCK6(value)                  (PMC_SCDR_PCK6_Msk & ((value) << PMC_SCDR_PCK6_Pos))
#define PMC_SCDR_PCK7_Pos                     _U_(15)                                              /**< (PMC_SCDR) Programmable Clock 7 Output Disable Position */
#define PMC_SCDR_PCK7_Msk                     (_U_(0x1) << PMC_SCDR_PCK7_Pos)                      /**< (PMC_SCDR) Programmable Clock 7 Output Disable Mask */
#define PMC_SCDR_PCK7(value)                  (PMC_SCDR_PCK7_Msk & ((value) << PMC_SCDR_PCK7_Pos))
#define PMC_SCDR_Msk                          _U_(0x0000FF20)                                      /**< (PMC_SCDR) Register Mask  */

#define PMC_SCDR_PCK_Pos                      _U_(8)                                               /**< (PMC_SCDR Position) Programmable Clock 7 Output Disable */
#define PMC_SCDR_PCK_Msk                      (_U_(0xFF) << PMC_SCDR_PCK_Pos)                      /**< (PMC_SCDR Mask) PCK */
#define PMC_SCDR_PCK(value)                   (PMC_SCDR_PCK_Msk & ((value) << PMC_SCDR_PCK_Pos))   

/* -------- PMC_SCSR : (PMC Offset: 0x08) ( R/ 32) System Clock Status Register -------- */
#define PMC_SCSR_HCLKS_Pos                    _U_(0)                                               /**< (PMC_SCSR) HCLK Status Position */
#define PMC_SCSR_HCLKS_Msk                    (_U_(0x1) << PMC_SCSR_HCLKS_Pos)                     /**< (PMC_SCSR) HCLK Status Mask */
#define PMC_SCSR_HCLKS(value)                 (PMC_SCSR_HCLKS_Msk & ((value) << PMC_SCSR_HCLKS_Pos))
#define PMC_SCSR_USBCLK_Pos                   _U_(5)                                               /**< (PMC_SCSR) USB FS Clock Status Position */
#define PMC_SCSR_USBCLK_Msk                   (_U_(0x1) << PMC_SCSR_USBCLK_Pos)                    /**< (PMC_SCSR) USB FS Clock Status Mask */
#define PMC_SCSR_USBCLK(value)                (PMC_SCSR_USBCLK_Msk & ((value) << PMC_SCSR_USBCLK_Pos))
#define PMC_SCSR_PCK0_Pos                     _U_(8)                                               /**< (PMC_SCSR) Programmable Clock 0 Output Status Position */
#define PMC_SCSR_PCK0_Msk                     (_U_(0x1) << PMC_SCSR_PCK0_Pos)                      /**< (PMC_SCSR) Programmable Clock 0 Output Status Mask */
#define PMC_SCSR_PCK0(value)                  (PMC_SCSR_PCK0_Msk & ((value) << PMC_SCSR_PCK0_Pos))
#define PMC_SCSR_PCK1_Pos                     _U_(9)                                               /**< (PMC_SCSR) Programmable Clock 1 Output Status Position */
#define PMC_SCSR_PCK1_Msk                     (_U_(0x1) << PMC_SCSR_PCK1_Pos)                      /**< (PMC_SCSR) Programmable Clock 1 Output Status Mask */
#define PMC_SCSR_PCK1(value)                  (PMC_SCSR_PCK1_Msk & ((value) << PMC_SCSR_PCK1_Pos))
#define PMC_SCSR_PCK2_Pos                     _U_(10)                                              /**< (PMC_SCSR) Programmable Clock 2 Output Status Position */
#define PMC_SCSR_PCK2_Msk                     (_U_(0x1) << PMC_SCSR_PCK2_Pos)                      /**< (PMC_SCSR) Programmable Clock 2 Output Status Mask */
#define PMC_SCSR_PCK2(value)                  (PMC_SCSR_PCK2_Msk & ((value) << PMC_SCSR_PCK2_Pos))
#define PMC_SCSR_PCK3_Pos                     _U_(11)                                              /**< (PMC_SCSR) Programmable Clock 3 Output Status Position */
#define PMC_SCSR_PCK3_Msk                     (_U_(0x1) << PMC_SCSR_PCK3_Pos)                      /**< (PMC_SCSR) Programmable Clock 3 Output Status Mask */
#define PMC_SCSR_PCK3(value)                  (PMC_SCSR_PCK3_Msk & ((value) << PMC_SCSR_PCK3_Pos))
#define PMC_SCSR_PCK4_Pos                     _U_(12)                                              /**< (PMC_SCSR) Programmable Clock 4 Output Status Position */
#define PMC_SCSR_PCK4_Msk                     (_U_(0x1) << PMC_SCSR_PCK4_Pos)                      /**< (PMC_SCSR) Programmable Clock 4 Output Status Mask */
#define PMC_SCSR_PCK4(value)                  (PMC_SCSR_PCK4_Msk & ((value) << PMC_SCSR_PCK4_Pos))
#define PMC_SCSR_PCK5_Pos                     _U_(13)                                              /**< (PMC_SCSR) Programmable Clock 5 Output Status Position */
#define PMC_SCSR_PCK5_Msk                     (_U_(0x1) << PMC_SCSR_PCK5_Pos)                      /**< (PMC_SCSR) Programmable Clock 5 Output Status Mask */
#define PMC_SCSR_PCK5(value)                  (PMC_SCSR_PCK5_Msk & ((value) << PMC_SCSR_PCK5_Pos))
#define PMC_SCSR_PCK6_Pos                     _U_(14)                                              /**< (PMC_SCSR) Programmable Clock 6 Output Status Position */
#define PMC_SCSR_PCK6_Msk                     (_U_(0x1) << PMC_SCSR_PCK6_Pos)                      /**< (PMC_SCSR) Programmable Clock 6 Output Status Mask */
#define PMC_SCSR_PCK6(value)                  (PMC_SCSR_PCK6_Msk & ((value) << PMC_SCSR_PCK6_Pos))
#define PMC_SCSR_PCK7_Pos                     _U_(15)                                              /**< (PMC_SCSR) Programmable Clock 7 Output Status Position */
#define PMC_SCSR_PCK7_Msk                     (_U_(0x1) << PMC_SCSR_PCK7_Pos)                      /**< (PMC_SCSR) Programmable Clock 7 Output Status Mask */
#define PMC_SCSR_PCK7(value)                  (PMC_SCSR_PCK7_Msk & ((value) << PMC_SCSR_PCK7_Pos))
#define PMC_SCSR_Msk                          _U_(0x0000FF21)                                      /**< (PMC_SCSR) Register Mask  */

#define PMC_SCSR_PCK_Pos                      _U_(8)                                               /**< (PMC_SCSR Position) Programmable Clock 7 Output Status */
#define PMC_SCSR_PCK_Msk                      (_U_(0xFF) << PMC_SCSR_PCK_Pos)                      /**< (PMC_SCSR Mask) PCK */
#define PMC_SCSR_PCK(value)                   (PMC_SCSR_PCK_Msk & ((value) << PMC_SCSR_PCK_Pos))   

/* -------- PMC_PCER0 : (PMC Offset: 0x10) ( /W 32) Peripheral Clock Enable Register 0 -------- */
#define PMC_PCER0_PID7_Pos                    _U_(7)                                               /**< (PMC_PCER0) Peripheral Clock 7 Enable Position */
#define PMC_PCER0_PID7_Msk                    (_U_(0x1) << PMC_PCER0_PID7_Pos)                     /**< (PMC_PCER0) Peripheral Clock 7 Enable Mask */
#define PMC_PCER0_PID7(value)                 (PMC_PCER0_PID7_Msk & ((value) << PMC_PCER0_PID7_Pos))
#define PMC_PCER0_PID8_Pos                    _U_(8)                                               /**< (PMC_PCER0) Peripheral Clock 8 Enable Position */
#define PMC_PCER0_PID8_Msk                    (_U_(0x1) << PMC_PCER0_PID8_Pos)                     /**< (PMC_PCER0) Peripheral Clock 8 Enable Mask */
#define PMC_PCER0_PID8(value)                 (PMC_PCER0_PID8_Msk & ((value) << PMC_PCER0_PID8_Pos))
#define PMC_PCER0_PID9_Pos                    _U_(9)                                               /**< (PMC_PCER0) Peripheral Clock 9 Enable Position */
#define PMC_PCER0_PID9_Msk                    (_U_(0x1) << PMC_PCER0_PID9_Pos)                     /**< (PMC_PCER0) Peripheral Clock 9 Enable Mask */
#define PMC_PCER0_PID9(value)                 (PMC_PCER0_PID9_Msk & ((value) << PMC_PCER0_PID9_Pos))
#define PMC_PCER0_PID10_Pos                   _U_(10)                                              /**< (PMC_PCER0) Peripheral Clock 10 Enable Position */
#define PMC_PCER0_PID10_Msk                   (_U_(0x1) << PMC_PCER0_PID10_Pos)                    /**< (PMC_PCER0) Peripheral Clock 10 Enable Mask */
#define PMC_PCER0_PID10(value)                (PMC_PCER0_PID10_Msk & ((value) << PMC_PCER0_PID10_Pos))
#define PMC_PCER0_PID11_Pos                   _U_(11)                                              /**< (PMC_PCER0) Peripheral Clock 11 Enable Position */
#define PMC_PCER0_PID11_Msk                   (_U_(0x1) << PMC_PCER0_PID11_Pos)                    /**< (PMC_PCER0) Peripheral Clock 11 Enable Mask */
#define PMC_PCER0_PID11(value)                (PMC_PCER0_PID11_Msk & ((value) << PMC_PCER0_PID11_Pos))
#define PMC_PCER0_PID12_Pos                   _U_(12)                                              /**< (PMC_PCER0) Peripheral Clock 12 Enable Position */
#define PMC_PCER0_PID12_Msk                   (_U_(0x1) << PMC_PCER0_PID12_Pos)                    /**< (PMC_PCER0) Peripheral Clock 12 Enable Mask */
#define PMC_PCER0_PID12(value)                (PMC_PCER0_PID12_Msk & ((value) << PMC_PCER0_PID12_Pos))
#define PMC_PCER0_PID13_Pos                   _U_(13)                                              /**< (PMC_PCER0) Peripheral Clock 13 Enable Position */
#define PMC_PCER0_PID13_Msk                   (_U_(0x1) << PMC_PCER0_PID13_Pos)                    /**< (PMC_PCER0) Peripheral Clock 13 Enable Mask */
#define PMC_PCER0_PID13(value)                (PMC_PCER0_PID13_Msk & ((value) << PMC_PCER0_PID13_Pos))
#define PMC_PCER0_PID14_Pos                   _U_(14)                                              /**< (PMC_PCER0) Peripheral Clock 14 Enable Position */
#define PMC_PCER0_PID14_Msk                   (_U_(0x1) << PMC_PCER0_PID14_Pos)                    /**< (PMC_PCER0) Peripheral Clock 14 Enable Mask */
#define PMC_PCER0_PID14(value)                (PMC_PCER0_PID14_Msk & ((value) << PMC_PCER0_PID14_Pos))
#define PMC_PCER0_PID15_Pos                   _U_(15)                                              /**< (PMC_PCER0) Peripheral Clock 15 Enable Position */
#define PMC_PCER0_PID15_Msk                   (_U_(0x1) << PMC_PCER0_PID15_Pos)                    /**< (PMC_PCER0) Peripheral Clock 15 Enable Mask */
#define PMC_PCER0_PID15(value)                (PMC_PCER0_PID15_Msk & ((value) << PMC_PCER0_PID15_Pos))
#define PMC_PCER0_PID16_Pos                   _U_(16)                                              /**< (PMC_PCER0) Peripheral Clock 16 Enable Position */
#define PMC_PCER0_PID16_Msk                   (_U_(0x1) << PMC_PCER0_PID16_Pos)                    /**< (PMC_PCER0) Peripheral Clock 16 Enable Mask */
#define PMC_PCER0_PID16(value)                (PMC_PCER0_PID16_Msk & ((value) << PMC_PCER0_PID16_Pos))
#define PMC_PCER0_PID17_Pos                   _U_(17)                                              /**< (PMC_PCER0) Peripheral Clock 17 Enable Position */
#define PMC_PCER0_PID17_Msk                   (_U_(0x1) << PMC_PCER0_PID17_Pos)                    /**< (PMC_PCER0) Peripheral Clock 17 Enable Mask */
#define PMC_PCER0_PID17(value)                (PMC_PCER0_PID17_Msk & ((value) << PMC_PCER0_PID17_Pos))
#define PMC_PCER0_PID18_Pos                   _U_(18)                                              /**< (PMC_PCER0) Peripheral Clock 18 Enable Position */
#define PMC_PCER0_PID18_Msk                   (_U_(0x1) << PMC_PCER0_PID18_Pos)                    /**< (PMC_PCER0) Peripheral Clock 18 Enable Mask */
#define PMC_PCER0_PID18(value)                (PMC_PCER0_PID18_Msk & ((value) << PMC_PCER0_PID18_Pos))
#define PMC_PCER0_PID19_Pos                   _U_(19)                                              /**< (PMC_PCER0) Peripheral Clock 19 Enable Position */
#define PMC_PCER0_PID19_Msk                   (_U_(0x1) << PMC_PCER0_PID19_Pos)                    /**< (PMC_PCER0) Peripheral Clock 19 Enable Mask */
#define PMC_PCER0_PID19(value)                (PMC_PCER0_PID19_Msk & ((value) << PMC_PCER0_PID19_Pos))
#define PMC_PCER0_PID20_Pos                   _U_(20)                                              /**< (PMC_PCER0) Peripheral Clock 20 Enable Position */
#define PMC_PCER0_PID20_Msk                   (_U_(0x1) << PMC_PCER0_PID20_Pos)                    /**< (PMC_PCER0) Peripheral Clock 20 Enable Mask */
#define PMC_PCER0_PID20(value)                (PMC_PCER0_PID20_Msk & ((value) << PMC_PCER0_PID20_Pos))
#define PMC_PCER0_PID21_Pos                   _U_(21)                                              /**< (PMC_PCER0) Peripheral Clock 21 Enable Position */
#define PMC_PCER0_PID21_Msk                   (_U_(0x1) << PMC_PCER0_PID21_Pos)                    /**< (PMC_PCER0) Peripheral Clock 21 Enable Mask */
#define PMC_PCER0_PID21(value)                (PMC_PCER0_PID21_Msk & ((value) << PMC_PCER0_PID21_Pos))
#define PMC_PCER0_PID22_Pos                   _U_(22)                                              /**< (PMC_PCER0) Peripheral Clock 22 Enable Position */
#define PMC_PCER0_PID22_Msk                   (_U_(0x1) << PMC_PCER0_PID22_Pos)                    /**< (PMC_PCER0) Peripheral Clock 22 Enable Mask */
#define PMC_PCER0_PID22(value)                (PMC_PCER0_PID22_Msk & ((value) << PMC_PCER0_PID22_Pos))
#define PMC_PCER0_PID23_Pos                   _U_(23)                                              /**< (PMC_PCER0) Peripheral Clock 23 Enable Position */
#define PMC_PCER0_PID23_Msk                   (_U_(0x1) << PMC_PCER0_PID23_Pos)                    /**< (PMC_PCER0) Peripheral Clock 23 Enable Mask */
#define PMC_PCER0_PID23(value)                (PMC_PCER0_PID23_Msk & ((value) << PMC_PCER0_PID23_Pos))
#define PMC_PCER0_PID24_Pos                   _U_(24)                                              /**< (PMC_PCER0) Peripheral Clock 24 Enable Position */
#define PMC_PCER0_PID24_Msk                   (_U_(0x1) << PMC_PCER0_PID24_Pos)                    /**< (PMC_PCER0) Peripheral Clock 24 Enable Mask */
#define PMC_PCER0_PID24(value)                (PMC_PCER0_PID24_Msk & ((value) << PMC_PCER0_PID24_Pos))
#define PMC_PCER0_PID25_Pos                   _U_(25)                                              /**< (PMC_PCER0) Peripheral Clock 25 Enable Position */
#define PMC_PCER0_PID25_Msk                   (_U_(0x1) << PMC_PCER0_PID25_Pos)                    /**< (PMC_PCER0) Peripheral Clock 25 Enable Mask */
#define PMC_PCER0_PID25(value)                (PMC_PCER0_PID25_Msk & ((value) << PMC_PCER0_PID25_Pos))
#define PMC_PCER0_PID26_Pos                   _U_(26)                                              /**< (PMC_PCER0) Peripheral Clock 26 Enable Position */
#define PMC_PCER0_PID26_Msk                   (_U_(0x1) << PMC_PCER0_PID26_Pos)                    /**< (PMC_PCER0) Peripheral Clock 26 Enable Mask */
#define PMC_PCER0_PID26(value)                (PMC_PCER0_PID26_Msk & ((value) << PMC_PCER0_PID26_Pos))
#define PMC_PCER0_PID27_Pos                   _U_(27)                                              /**< (PMC_PCER0) Peripheral Clock 27 Enable Position */
#define PMC_PCER0_PID27_Msk                   (_U_(0x1) << PMC_PCER0_PID27_Pos)                    /**< (PMC_PCER0) Peripheral Clock 27 Enable Mask */
#define PMC_PCER0_PID27(value)                (PMC_PCER0_PID27_Msk & ((value) << PMC_PCER0_PID27_Pos))
#define PMC_PCER0_PID28_Pos                   _U_(28)                                              /**< (PMC_PCER0) Peripheral Clock 28 Enable Position */
#define PMC_PCER0_PID28_Msk                   (_U_(0x1) << PMC_PCER0_PID28_Pos)                    /**< (PMC_PCER0) Peripheral Clock 28 Enable Mask */
#define PMC_PCER0_PID28(value)                (PMC_PCER0_PID28_Msk & ((value) << PMC_PCER0_PID28_Pos))
#define PMC_PCER0_PID29_Pos                   _U_(29)                                              /**< (PMC_PCER0) Peripheral Clock 29 Enable Position */
#define PMC_PCER0_PID29_Msk                   (_U_(0x1) << PMC_PCER0_PID29_Pos)                    /**< (PMC_PCER0) Peripheral Clock 29 Enable Mask */
#define PMC_PCER0_PID29(value)                (PMC_PCER0_PID29_Msk & ((value) << PMC_PCER0_PID29_Pos))
#define PMC_PCER0_PID30_Pos                   _U_(30)                                              /**< (PMC_PCER0) Peripheral Clock 30 Enable Position */
#define PMC_PCER0_PID30_Msk                   (_U_(0x1) << PMC_PCER0_PID30_Pos)                    /**< (PMC_PCER0) Peripheral Clock 30 Enable Mask */
#define PMC_PCER0_PID30(value)                (PMC_PCER0_PID30_Msk & ((value) << PMC_PCER0_PID30_Pos))
#define PMC_PCER0_PID31_Pos                   _U_(31)                                              /**< (PMC_PCER0) Peripheral Clock 31 Enable Position */
#define PMC_PCER0_PID31_Msk                   (_U_(0x1) << PMC_PCER0_PID31_Pos)                    /**< (PMC_PCER0) Peripheral Clock 31 Enable Mask */
#define PMC_PCER0_PID31(value)                (PMC_PCER0_PID31_Msk & ((value) << PMC_PCER0_PID31_Pos))
#define PMC_PCER0_Msk                         _U_(0xFFFFFF80)                                      /**< (PMC_PCER0) Register Mask  */

#define PMC_PCER0_PID_Pos                     _U_(7)                                               /**< (PMC_PCER0 Position) Peripheral Clock 3x Enable */
#define PMC_PCER0_PID_Msk                     (_U_(0x1FFFFFF) << PMC_PCER0_PID_Pos)                /**< (PMC_PCER0 Mask) PID */
#define PMC_PCER0_PID(value)                  (PMC_PCER0_PID_Msk & ((value) << PMC_PCER0_PID_Pos)) 

/* -------- PMC_PCDR0 : (PMC Offset: 0x14) ( /W 32) Peripheral Clock Disable Register 0 -------- */
#define PMC_PCDR0_PID7_Pos                    _U_(7)                                               /**< (PMC_PCDR0) Peripheral Clock 7 Disable Position */
#define PMC_PCDR0_PID7_Msk                    (_U_(0x1) << PMC_PCDR0_PID7_Pos)                     /**< (PMC_PCDR0) Peripheral Clock 7 Disable Mask */
#define PMC_PCDR0_PID7(value)                 (PMC_PCDR0_PID7_Msk & ((value) << PMC_PCDR0_PID7_Pos))
#define PMC_PCDR0_PID8_Pos                    _U_(8)                                               /**< (PMC_PCDR0) Peripheral Clock 8 Disable Position */
#define PMC_PCDR0_PID8_Msk                    (_U_(0x1) << PMC_PCDR0_PID8_Pos)                     /**< (PMC_PCDR0) Peripheral Clock 8 Disable Mask */
#define PMC_PCDR0_PID8(value)                 (PMC_PCDR0_PID8_Msk & ((value) << PMC_PCDR0_PID8_Pos))
#define PMC_PCDR0_PID9_Pos                    _U_(9)                                               /**< (PMC_PCDR0) Peripheral Clock 9 Disable Position */
#define PMC_PCDR0_PID9_Msk                    (_U_(0x1) << PMC_PCDR0_PID9_Pos)                     /**< (PMC_PCDR0) Peripheral Clock 9 Disable Mask */
#define PMC_PCDR0_PID9(value)                 (PMC_PCDR0_PID9_Msk & ((value) << PMC_PCDR0_PID9_Pos))
#define PMC_PCDR0_PID10_Pos                   _U_(10)                                              /**< (PMC_PCDR0) Peripheral Clock 10 Disable Position */
#define PMC_PCDR0_PID10_Msk                   (_U_(0x1) << PMC_PCDR0_PID10_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 10 Disable Mask */
#define PMC_PCDR0_PID10(value)                (PMC_PCDR0_PID10_Msk & ((value) << PMC_PCDR0_PID10_Pos))
#define PMC_PCDR0_PID11_Pos                   _U_(11)                                              /**< (PMC_PCDR0) Peripheral Clock 11 Disable Position */
#define PMC_PCDR0_PID11_Msk                   (_U_(0x1) << PMC_PCDR0_PID11_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 11 Disable Mask */
#define PMC_PCDR0_PID11(value)                (PMC_PCDR0_PID11_Msk & ((value) << PMC_PCDR0_PID11_Pos))
#define PMC_PCDR0_PID12_Pos                   _U_(12)                                              /**< (PMC_PCDR0) Peripheral Clock 12 Disable Position */
#define PMC_PCDR0_PID12_Msk                   (_U_(0x1) << PMC_PCDR0_PID12_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 12 Disable Mask */
#define PMC_PCDR0_PID12(value)                (PMC_PCDR0_PID12_Msk & ((value) << PMC_PCDR0_PID12_Pos))
#define PMC_PCDR0_PID13_Pos                   _U_(13)                                              /**< (PMC_PCDR0) Peripheral Clock 13 Disable Position */
#define PMC_PCDR0_PID13_Msk                   (_U_(0x1) << PMC_PCDR0_PID13_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 13 Disable Mask */
#define PMC_PCDR0_PID13(value)                (PMC_PCDR0_PID13_Msk & ((value) << PMC_PCDR0_PID13_Pos))
#define PMC_PCDR0_PID14_Pos                   _U_(14)                                              /**< (PMC_PCDR0) Peripheral Clock 14 Disable Position */
#define PMC_PCDR0_PID14_Msk                   (_U_(0x1) << PMC_PCDR0_PID14_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 14 Disable Mask */
#define PMC_PCDR0_PID14(value)                (PMC_PCDR0_PID14_Msk & ((value) << PMC_PCDR0_PID14_Pos))
#define PMC_PCDR0_PID15_Pos                   _U_(15)                                              /**< (PMC_PCDR0) Peripheral Clock 15 Disable Position */
#define PMC_PCDR0_PID15_Msk                   (_U_(0x1) << PMC_PCDR0_PID15_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 15 Disable Mask */
#define PMC_PCDR0_PID15(value)                (PMC_PCDR0_PID15_Msk & ((value) << PMC_PCDR0_PID15_Pos))
#define PMC_PCDR0_PID16_Pos                   _U_(16)                                              /**< (PMC_PCDR0) Peripheral Clock 16 Disable Position */
#define PMC_PCDR0_PID16_Msk                   (_U_(0x1) << PMC_PCDR0_PID16_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 16 Disable Mask */
#define PMC_PCDR0_PID16(value)                (PMC_PCDR0_PID16_Msk & ((value) << PMC_PCDR0_PID16_Pos))
#define PMC_PCDR0_PID17_Pos                   _U_(17)                                              /**< (PMC_PCDR0) Peripheral Clock 17 Disable Position */
#define PMC_PCDR0_PID17_Msk                   (_U_(0x1) << PMC_PCDR0_PID17_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 17 Disable Mask */
#define PMC_PCDR0_PID17(value)                (PMC_PCDR0_PID17_Msk & ((value) << PMC_PCDR0_PID17_Pos))
#define PMC_PCDR0_PID18_Pos                   _U_(18)                                              /**< (PMC_PCDR0) Peripheral Clock 18 Disable Position */
#define PMC_PCDR0_PID18_Msk                   (_U_(0x1) << PMC_PCDR0_PID18_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 18 Disable Mask */
#define PMC_PCDR0_PID18(value)                (PMC_PCDR0_PID18_Msk & ((value) << PMC_PCDR0_PID18_Pos))
#define PMC_PCDR0_PID19_Pos                   _U_(19)                                              /**< (PMC_PCDR0) Peripheral Clock 19 Disable Position */
#define PMC_PCDR0_PID19_Msk                   (_U_(0x1) << PMC_PCDR0_PID19_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 19 Disable Mask */
#define PMC_PCDR0_PID19(value)                (PMC_PCDR0_PID19_Msk & ((value) << PMC_PCDR0_PID19_Pos))
#define PMC_PCDR0_PID20_Pos                   _U_(20)                                              /**< (PMC_PCDR0) Peripheral Clock 20 Disable Position */
#define PMC_PCDR0_PID20_Msk                   (_U_(0x1) << PMC_PCDR0_PID20_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 20 Disable Mask */
#define PMC_PCDR0_PID20(value)                (PMC_PCDR0_PID20_Msk & ((value) << PMC_PCDR0_PID20_Pos))
#define PMC_PCDR0_PID21_Pos                   _U_(21)                                              /**< (PMC_PCDR0) Peripheral Clock 21 Disable Position */
#define PMC_PCDR0_PID21_Msk                   (_U_(0x1) << PMC_PCDR0_PID21_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 21 Disable Mask */
#define PMC_PCDR0_PID21(value)                (PMC_PCDR0_PID21_Msk & ((value) << PMC_PCDR0_PID21_Pos))
#define PMC_PCDR0_PID22_Pos                   _U_(22)                                              /**< (PMC_PCDR0) Peripheral Clock 22 Disable Position */
#define PMC_PCDR0_PID22_Msk                   (_U_(0x1) << PMC_PCDR0_PID22_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 22 Disable Mask */
#define PMC_PCDR0_PID22(value)                (PMC_PCDR0_PID22_Msk & ((value) << PMC_PCDR0_PID22_Pos))
#define PMC_PCDR0_PID23_Pos                   _U_(23)                                              /**< (PMC_PCDR0) Peripheral Clock 23 Disable Position */
#define PMC_PCDR0_PID23_Msk                   (_U_(0x1) << PMC_PCDR0_PID23_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 23 Disable Mask */
#define PMC_PCDR0_PID23(value)                (PMC_PCDR0_PID23_Msk & ((value) << PMC_PCDR0_PID23_Pos))
#define PMC_PCDR0_PID24_Pos                   _U_(24)                                              /**< (PMC_PCDR0) Peripheral Clock 24 Disable Position */
#define PMC_PCDR0_PID24_Msk                   (_U_(0x1) << PMC_PCDR0_PID24_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 24 Disable Mask */
#define PMC_PCDR0_PID24(value)                (PMC_PCDR0_PID24_Msk & ((value) << PMC_PCDR0_PID24_Pos))
#define PMC_PCDR0_PID25_Pos                   _U_(25)                                              /**< (PMC_PCDR0) Peripheral Clock 25 Disable Position */
#define PMC_PCDR0_PID25_Msk                   (_U_(0x1) << PMC_PCDR0_PID25_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 25 Disable Mask */
#define PMC_PCDR0_PID25(value)                (PMC_PCDR0_PID25_Msk & ((value) << PMC_PCDR0_PID25_Pos))
#define PMC_PCDR0_PID26_Pos                   _U_(26)                                              /**< (PMC_PCDR0) Peripheral Clock 26 Disable Position */
#define PMC_PCDR0_PID26_Msk                   (_U_(0x1) << PMC_PCDR0_PID26_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 26 Disable Mask */
#define PMC_PCDR0_PID26(value)                (PMC_PCDR0_PID26_Msk & ((value) << PMC_PCDR0_PID26_Pos))
#define PMC_PCDR0_PID27_Pos                   _U_(27)                                              /**< (PMC_PCDR0) Peripheral Clock 27 Disable Position */
#define PMC_PCDR0_PID27_Msk                   (_U_(0x1) << PMC_PCDR0_PID27_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 27 Disable Mask */
#define PMC_PCDR0_PID27(value)                (PMC_PCDR0_PID27_Msk & ((value) << PMC_PCDR0_PID27_Pos))
#define PMC_PCDR0_PID28_Pos                   _U_(28)                                              /**< (PMC_PCDR0) Peripheral Clock 28 Disable Position */
#define PMC_PCDR0_PID28_Msk                   (_U_(0x1) << PMC_PCDR0_PID28_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 28 Disable Mask */
#define PMC_PCDR0_PID28(value)                (PMC_PCDR0_PID28_Msk & ((value) << PMC_PCDR0_PID28_Pos))
#define PMC_PCDR0_PID29_Pos                   _U_(29)                                              /**< (PMC_PCDR0) Peripheral Clock 29 Disable Position */
#define PMC_PCDR0_PID29_Msk                   (_U_(0x1) << PMC_PCDR0_PID29_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 29 Disable Mask */
#define PMC_PCDR0_PID29(value)                (PMC_PCDR0_PID29_Msk & ((value) << PMC_PCDR0_PID29_Pos))
#define PMC_PCDR0_PID30_Pos                   _U_(30)                                              /**< (PMC_PCDR0) Peripheral Clock 30 Disable Position */
#define PMC_PCDR0_PID30_Msk                   (_U_(0x1) << PMC_PCDR0_PID30_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 30 Disable Mask */
#define PMC_PCDR0_PID30(value)                (PMC_PCDR0_PID30_Msk & ((value) << PMC_PCDR0_PID30_Pos))
#define PMC_PCDR0_PID31_Pos                   _U_(31)                                              /**< (PMC_PCDR0) Peripheral Clock 31 Disable Position */
#define PMC_PCDR0_PID31_Msk                   (_U_(0x1) << PMC_PCDR0_PID31_Pos)                    /**< (PMC_PCDR0) Peripheral Clock 31 Disable Mask */
#define PMC_PCDR0_PID31(value)                (PMC_PCDR0_PID31_Msk & ((value) << PMC_PCDR0_PID31_Pos))
#define PMC_PCDR0_Msk                         _U_(0xFFFFFF80)                                      /**< (PMC_PCDR0) Register Mask  */

#define PMC_PCDR0_PID_Pos                     _U_(7)                                               /**< (PMC_PCDR0 Position) Peripheral Clock 3x Disable */
#define PMC_PCDR0_PID_Msk                     (_U_(0x1FFFFFF) << PMC_PCDR0_PID_Pos)                /**< (PMC_PCDR0 Mask) PID */
#define PMC_PCDR0_PID(value)                  (PMC_PCDR0_PID_Msk & ((value) << PMC_PCDR0_PID_Pos)) 

/* -------- PMC_PCSR0 : (PMC Offset: 0x18) ( R/ 32) Peripheral Clock Status Register 0 -------- */
#define PMC_PCSR0_PID7_Pos                    _U_(7)                                               /**< (PMC_PCSR0) Peripheral Clock 7 Status Position */
#define PMC_PCSR0_PID7_Msk                    (_U_(0x1) << PMC_PCSR0_PID7_Pos)                     /**< (PMC_PCSR0) Peripheral Clock 7 Status Mask */
#define PMC_PCSR0_PID7(value)                 (PMC_PCSR0_PID7_Msk & ((value) << PMC_PCSR0_PID7_Pos))
#define PMC_PCSR0_PID8_Pos                    _U_(8)                                               /**< (PMC_PCSR0) Peripheral Clock 8 Status Position */
#define PMC_PCSR0_PID8_Msk                    (_U_(0x1) << PMC_PCSR0_PID8_Pos)                     /**< (PMC_PCSR0) Peripheral Clock 8 Status Mask */
#define PMC_PCSR0_PID8(value)                 (PMC_PCSR0_PID8_Msk & ((value) << PMC_PCSR0_PID8_Pos))
#define PMC_PCSR0_PID9_Pos                    _U_(9)                                               /**< (PMC_PCSR0) Peripheral Clock 9 Status Position */
#define PMC_PCSR0_PID9_Msk                    (_U_(0x1) << PMC_PCSR0_PID9_Pos)                     /**< (PMC_PCSR0) Peripheral Clock 9 Status Mask */
#define PMC_PCSR0_PID9(value)                 (PMC_PCSR0_PID9_Msk & ((value) << PMC_PCSR0_PID9_Pos))
#define PMC_PCSR0_PID10_Pos                   _U_(10)                                              /**< (PMC_PCSR0) Peripheral Clock 10 Status Position */
#define PMC_PCSR0_PID10_Msk                   (_U_(0x1) << PMC_PCSR0_PID10_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 10 Status Mask */
#define PMC_PCSR0_PID10(value)                (PMC_PCSR0_PID10_Msk & ((value) << PMC_PCSR0_PID10_Pos))
#define PMC_PCSR0_PID11_Pos                   _U_(11)                                              /**< (PMC_PCSR0) Peripheral Clock 11 Status Position */
#define PMC_PCSR0_PID11_Msk                   (_U_(0x1) << PMC_PCSR0_PID11_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 11 Status Mask */
#define PMC_PCSR0_PID11(value)                (PMC_PCSR0_PID11_Msk & ((value) << PMC_PCSR0_PID11_Pos))
#define PMC_PCSR0_PID12_Pos                   _U_(12)                                              /**< (PMC_PCSR0) Peripheral Clock 12 Status Position */
#define PMC_PCSR0_PID12_Msk                   (_U_(0x1) << PMC_PCSR0_PID12_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 12 Status Mask */
#define PMC_PCSR0_PID12(value)                (PMC_PCSR0_PID12_Msk & ((value) << PMC_PCSR0_PID12_Pos))
#define PMC_PCSR0_PID13_Pos                   _U_(13)                                              /**< (PMC_PCSR0) Peripheral Clock 13 Status Position */
#define PMC_PCSR0_PID13_Msk                   (_U_(0x1) << PMC_PCSR0_PID13_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 13 Status Mask */
#define PMC_PCSR0_PID13(value)                (PMC_PCSR0_PID13_Msk & ((value) << PMC_PCSR0_PID13_Pos))
#define PMC_PCSR0_PID14_Pos                   _U_(14)                                              /**< (PMC_PCSR0) Peripheral Clock 14 Status Position */
#define PMC_PCSR0_PID14_Msk                   (_U_(0x1) << PMC_PCSR0_PID14_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 14 Status Mask */
#define PMC_PCSR0_PID14(value)                (PMC_PCSR0_PID14_Msk & ((value) << PMC_PCSR0_PID14_Pos))
#define PMC_PCSR0_PID15_Pos                   _U_(15)                                              /**< (PMC_PCSR0) Peripheral Clock 15 Status Position */
#define PMC_PCSR0_PID15_Msk                   (_U_(0x1) << PMC_PCSR0_PID15_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 15 Status Mask */
#define PMC_PCSR0_PID15(value)                (PMC_PCSR0_PID15_Msk & ((value) << PMC_PCSR0_PID15_Pos))
#define PMC_PCSR0_PID16_Pos                   _U_(16)                                              /**< (PMC_PCSR0) Peripheral Clock 16 Status Position */
#define PMC_PCSR0_PID16_Msk                   (_U_(0x1) << PMC_PCSR0_PID16_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 16 Status Mask */
#define PMC_PCSR0_PID16(value)                (PMC_PCSR0_PID16_Msk & ((value) << PMC_PCSR0_PID16_Pos))
#define PMC_PCSR0_PID17_Pos                   _U_(17)                                              /**< (PMC_PCSR0) Peripheral Clock 17 Status Position */
#define PMC_PCSR0_PID17_Msk                   (_U_(0x1) << PMC_PCSR0_PID17_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 17 Status Mask */
#define PMC_PCSR0_PID17(value)                (PMC_PCSR0_PID17_Msk & ((value) << PMC_PCSR0_PID17_Pos))
#define PMC_PCSR0_PID18_Pos                   _U_(18)                                              /**< (PMC_PCSR0) Peripheral Clock 18 Status Position */
#define PMC_PCSR0_PID18_Msk                   (_U_(0x1) << PMC_PCSR0_PID18_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 18 Status Mask */
#define PMC_PCSR0_PID18(value)                (PMC_PCSR0_PID18_Msk & ((value) << PMC_PCSR0_PID18_Pos))
#define PMC_PCSR0_PID19_Pos                   _U_(19)                                              /**< (PMC_PCSR0) Peripheral Clock 19 Status Position */
#define PMC_PCSR0_PID19_Msk                   (_U_(0x1) << PMC_PCSR0_PID19_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 19 Status Mask */
#define PMC_PCSR0_PID19(value)                (PMC_PCSR0_PID19_Msk & ((value) << PMC_PCSR0_PID19_Pos))
#define PMC_PCSR0_PID20_Pos                   _U_(20)                                              /**< (PMC_PCSR0) Peripheral Clock 20 Status Position */
#define PMC_PCSR0_PID20_Msk                   (_U_(0x1) << PMC_PCSR0_PID20_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 20 Status Mask */
#define PMC_PCSR0_PID20(value)                (PMC_PCSR0_PID20_Msk & ((value) << PMC_PCSR0_PID20_Pos))
#define PMC_PCSR0_PID21_Pos                   _U_(21)                                              /**< (PMC_PCSR0) Peripheral Clock 21 Status Position */
#define PMC_PCSR0_PID21_Msk                   (_U_(0x1) << PMC_PCSR0_PID21_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 21 Status Mask */
#define PMC_PCSR0_PID21(value)                (PMC_PCSR0_PID21_Msk & ((value) << PMC_PCSR0_PID21_Pos))
#define PMC_PCSR0_PID22_Pos                   _U_(22)                                              /**< (PMC_PCSR0) Peripheral Clock 22 Status Position */
#define PMC_PCSR0_PID22_Msk                   (_U_(0x1) << PMC_PCSR0_PID22_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 22 Status Mask */
#define PMC_PCSR0_PID22(value)                (PMC_PCSR0_PID22_Msk & ((value) << PMC_PCSR0_PID22_Pos))
#define PMC_PCSR0_PID23_Pos                   _U_(23)                                              /**< (PMC_PCSR0) Peripheral Clock 23 Status Position */
#define PMC_PCSR0_PID23_Msk                   (_U_(0x1) << PMC_PCSR0_PID23_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 23 Status Mask */
#define PMC_PCSR0_PID23(value)                (PMC_PCSR0_PID23_Msk & ((value) << PMC_PCSR0_PID23_Pos))
#define PMC_PCSR0_PID24_Pos                   _U_(24)                                              /**< (PMC_PCSR0) Peripheral Clock 24 Status Position */
#define PMC_PCSR0_PID24_Msk                   (_U_(0x1) << PMC_PCSR0_PID24_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 24 Status Mask */
#define PMC_PCSR0_PID24(value)                (PMC_PCSR0_PID24_Msk & ((value) << PMC_PCSR0_PID24_Pos))
#define PMC_PCSR0_PID25_Pos                   _U_(25)                                              /**< (PMC_PCSR0) Peripheral Clock 25 Status Position */
#define PMC_PCSR0_PID25_Msk                   (_U_(0x1) << PMC_PCSR0_PID25_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 25 Status Mask */
#define PMC_PCSR0_PID25(value)                (PMC_PCSR0_PID25_Msk & ((value) << PMC_PCSR0_PID25_Pos))
#define PMC_PCSR0_PID26_Pos                   _U_(26)                                              /**< (PMC_PCSR0) Peripheral Clock 26 Status Position */
#define PMC_PCSR0_PID26_Msk                   (_U_(0x1) << PMC_PCSR0_PID26_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 26 Status Mask */
#define PMC_PCSR0_PID26(value)                (PMC_PCSR0_PID26_Msk & ((value) << PMC_PCSR0_PID26_Pos))
#define PMC_PCSR0_PID27_Pos                   _U_(27)                                              /**< (PMC_PCSR0) Peripheral Clock 27 Status Position */
#define PMC_PCSR0_PID27_Msk                   (_U_(0x1) << PMC_PCSR0_PID27_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 27 Status Mask */
#define PMC_PCSR0_PID27(value)                (PMC_PCSR0_PID27_Msk & ((value) << PMC_PCSR0_PID27_Pos))
#define PMC_PCSR0_PID28_Pos                   _U_(28)                                              /**< (PMC_PCSR0) Peripheral Clock 28 Status Position */
#define PMC_PCSR0_PID28_Msk                   (_U_(0x1) << PMC_PCSR0_PID28_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 28 Status Mask */
#define PMC_PCSR0_PID28(value)                (PMC_PCSR0_PID28_Msk & ((value) << PMC_PCSR0_PID28_Pos))
#define PMC_PCSR0_PID29_Pos                   _U_(29)                                              /**< (PMC_PCSR0) Peripheral Clock 29 Status Position */
#define PMC_PCSR0_PID29_Msk                   (_U_(0x1) << PMC_PCSR0_PID29_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 29 Status Mask */
#define PMC_PCSR0_PID29(value)                (PMC_PCSR0_PID29_Msk & ((value) << PMC_PCSR0_PID29_Pos))
#define PMC_PCSR0_PID30_Pos                   _U_(30)                                              /**< (PMC_PCSR0) Peripheral Clock 30 Status Position */
#define PMC_PCSR0_PID30_Msk                   (_U_(0x1) << PMC_PCSR0_PID30_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 30 Status Mask */
#define PMC_PCSR0_PID30(value)                (PMC_PCSR0_PID30_Msk & ((value) << PMC_PCSR0_PID30_Pos))
#define PMC_PCSR0_PID31_Pos                   _U_(31)                                              /**< (PMC_PCSR0) Peripheral Clock 31 Status Position */
#define PMC_PCSR0_PID31_Msk                   (_U_(0x1) << PMC_PCSR0_PID31_Pos)                    /**< (PMC_PCSR0) Peripheral Clock 31 Status Mask */
#define PMC_PCSR0_PID31(value)                (PMC_PCSR0_PID31_Msk & ((value) << PMC_PCSR0_PID31_Pos))
#define PMC_PCSR0_Msk                         _U_(0xFFFFFF80)                                      /**< (PMC_PCSR0) Register Mask  */

#define PMC_PCSR0_PID_Pos                     _U_(7)                                               /**< (PMC_PCSR0 Position) Peripheral Clock 3x Status */
#define PMC_PCSR0_PID_Msk                     (_U_(0x1FFFFFF) << PMC_PCSR0_PID_Pos)                /**< (PMC_PCSR0 Mask) PID */
#define PMC_PCSR0_PID(value)                  (PMC_PCSR0_PID_Msk & ((value) << PMC_PCSR0_PID_Pos)) 

/* -------- CKGR_UCKR : (PMC Offset: 0x1C) (R/W 32) UTMI Clock Register -------- */
#define CKGR_UCKR_UPLLEN_Pos                  _U_(16)                                              /**< (CKGR_UCKR) UTMI PLL Enable Position */
#define CKGR_UCKR_UPLLEN_Msk                  (_U_(0x1) << CKGR_UCKR_UPLLEN_Pos)                   /**< (CKGR_UCKR) UTMI PLL Enable Mask */
#define CKGR_UCKR_UPLLEN(value)               (CKGR_UCKR_UPLLEN_Msk & ((value) << CKGR_UCKR_UPLLEN_Pos))
#define CKGR_UCKR_UPLLCOUNT_Pos               _U_(20)                                              /**< (CKGR_UCKR) UTMI PLL Start-up Time Position */
#define CKGR_UCKR_UPLLCOUNT_Msk               (_U_(0xF) << CKGR_UCKR_UPLLCOUNT_Pos)                /**< (CKGR_UCKR) UTMI PLL Start-up Time Mask */
#define CKGR_UCKR_UPLLCOUNT(value)            (CKGR_UCKR_UPLLCOUNT_Msk & ((value) << CKGR_UCKR_UPLLCOUNT_Pos))
#define CKGR_UCKR_Msk                         _U_(0x00F10000)                                      /**< (CKGR_UCKR) Register Mask  */


/* -------- CKGR_MOR : (PMC Offset: 0x20) (R/W 32) Main Oscillator Register -------- */
#define CKGR_MOR_MOSCXTEN_Pos                 _U_(0)                                               /**< (CKGR_MOR) Main Crystal Oscillator Enable Position */
#define CKGR_MOR_MOSCXTEN_Msk                 (_U_(0x1) << CKGR_MOR_MOSCXTEN_Pos)                  /**< (CKGR_MOR) Main Crystal Oscillator Enable Mask */
#define CKGR_MOR_MOSCXTEN(value)              (CKGR_MOR_MOSCXTEN_Msk & ((value) << CKGR_MOR_MOSCXTEN_Pos))
#define CKGR_MOR_MOSCXTBY_Pos                 _U_(1)                                               /**< (CKGR_MOR) Main Crystal Oscillator Bypass Position */
#define CKGR_MOR_MOSCXTBY_Msk                 (_U_(0x1) << CKGR_MOR_MOSCXTBY_Pos)                  /**< (CKGR_MOR) Main Crystal Oscillator Bypass Mask */
#define CKGR_MOR_MOSCXTBY(value)              (CKGR_MOR_MOSCXTBY_Msk & ((value) << CKGR_MOR_MOSCXTBY_Pos))
#define CKGR_MOR_WAITMODE_Pos                 _U_(2)                                               /**< (CKGR_MOR) Wait Mode Command (Write-only) Position */
#define CKGR_MOR_WAITMODE_Msk                 (_U_(0x1) << CKGR_MOR_WAITMODE_Pos)                  /**< (CKGR_MOR) Wait Mode Command (Write-only) Mask */
#define CKGR_MOR_WAITMODE(value)              (CKGR_MOR_WAITMODE_Msk & ((value) << CKGR_MOR_WAITMODE_Pos))
#define CKGR_MOR_MOSCRCEN_Pos                 _U_(3)                                               /**< (CKGR_MOR) Main RC Oscillator Enable Position */
#define CKGR_MOR_MOSCRCEN_Msk                 (_U_(0x1) << CKGR_MOR_MOSCRCEN_Pos)                  /**< (CKGR_MOR) Main RC Oscillator Enable Mask */
#define CKGR_MOR_MOSCRCEN(value)              (CKGR_MOR_MOSCRCEN_Msk & ((value) << CKGR_MOR_MOSCRCEN_Pos))
#define CKGR_MOR_MOSCRCF_Pos                  _U_(4)                                               /**< (CKGR_MOR) Main RC Oscillator Frequency Selection Position */
#define CKGR_MOR_MOSCRCF_Msk                  (_U_(0x7) << CKGR_MOR_MOSCRCF_Pos)                   /**< (CKGR_MOR) Main RC Oscillator Frequency Selection Mask */
#define CKGR_MOR_MOSCRCF(value)               (CKGR_MOR_MOSCRCF_Msk & ((value) << CKGR_MOR_MOSCRCF_Pos))
#define   CKGR_MOR_MOSCRCF_4_MHz_Val          _U_(0x0)                                             /**< (CKGR_MOR) The RC oscillator frequency is at 4 MHz  */
#define   CKGR_MOR_MOSCRCF_8_MHz_Val          _U_(0x1)                                             /**< (CKGR_MOR) The RC oscillator frequency is at 8 MHz  */
#define   CKGR_MOR_MOSCRCF_12_MHz_Val         _U_(0x2)                                             /**< (CKGR_MOR) The RC oscillator frequency is at 12 MHz  */
#define CKGR_MOR_MOSCRCF_4_MHz                (CKGR_MOR_MOSCRCF_4_MHz_Val << CKGR_MOR_MOSCRCF_Pos) /**< (CKGR_MOR) The RC oscillator frequency is at 4 MHz Position  */
#define CKGR_MOR_MOSCRCF_8_MHz                (CKGR_MOR_MOSCRCF_8_MHz_Val << CKGR_MOR_MOSCRCF_Pos) /**< (CKGR_MOR) The RC oscillator frequency is at 8 MHz Position  */
#define CKGR_MOR_MOSCRCF_12_MHz               (CKGR_MOR_MOSCRCF_12_MHz_Val << CKGR_MOR_MOSCRCF_Pos) /**< (CKGR_MOR) The RC oscillator frequency is at 12 MHz Position  */
#define CKGR_MOR_MOSCXTST_Pos                 _U_(8)                                               /**< (CKGR_MOR) Main Crystal Oscillator Startup Time Position */
#define CKGR_MOR_MOSCXTST_Msk                 (_U_(0xFF) << CKGR_MOR_MOSCXTST_Pos)                 /**< (CKGR_MOR) Main Crystal Oscillator Startup Time Mask */
#define CKGR_MOR_MOSCXTST(value)              (CKGR_MOR_MOSCXTST_Msk & ((value) << CKGR_MOR_MOSCXTST_Pos))
#define CKGR_MOR_KEY_Pos                      _U_(16)                                              /**< (CKGR_MOR) Write Access Password Position */
#define CKGR_MOR_KEY_Msk                      (_U_(0xFF) << CKGR_MOR_KEY_Pos)                      /**< (CKGR_MOR) Write Access Password Mask */
#define CKGR_MOR_KEY(value)                   (CKGR_MOR_KEY_Msk & ((value) << CKGR_MOR_KEY_Pos))  
#define   CKGR_MOR_KEY_PASSWD_Val             _U_(0x37)                                            /**< (CKGR_MOR) Writing any other value in this field aborts the write operation.Always reads as 0.  */
#define CKGR_MOR_KEY_PASSWD                   (CKGR_MOR_KEY_PASSWD_Val << CKGR_MOR_KEY_Pos)        /**< (CKGR_MOR) Writing any other value in this field aborts the write operation.Always reads as 0. Position  */
#define CKGR_MOR_MOSCSEL_Pos                  _U_(24)                                              /**< (CKGR_MOR) Main Clock Oscillator Selection Position */
#define CKGR_MOR_MOSCSEL_Msk                  (_U_(0x1) << CKGR_MOR_MOSCSEL_Pos)                   /**< (CKGR_MOR) Main Clock Oscillator Selection Mask */
#define CKGR_MOR_MOSCSEL(value)               (CKGR_MOR_MOSCSEL_Msk & ((value) << CKGR_MOR_MOSCSEL_Pos))
#define CKGR_MOR_CFDEN_Pos                    _U_(25)                                              /**< (CKGR_MOR) Clock Failure Detector Enable Position */
#define CKGR_MOR_CFDEN_Msk                    (_U_(0x1) << CKGR_MOR_CFDEN_Pos)                     /**< (CKGR_MOR) Clock Failure Detector Enable Mask */
#define CKGR_MOR_CFDEN(value)                 (CKGR_MOR_CFDEN_Msk & ((value) << CKGR_MOR_CFDEN_Pos))
#define CKGR_MOR_XT32KFME_Pos                 _U_(26)                                              /**< (CKGR_MOR) 32.768 kHz Crystal Oscillator Frequency Monitoring Enable Position */
#define CKGR_MOR_XT32KFME_Msk                 (_U_(0x1) << CKGR_MOR_XT32KFME_Pos)                  /**< (CKGR_MOR) 32.768 kHz Crystal Oscillator Frequency Monitoring Enable Mask */
#define CKGR_MOR_XT32KFME(value)              (CKGR_MOR_XT32KFME_Msk & ((value) << CKGR_MOR_XT32KFME_Pos))
#define CKGR_MOR_Msk                          _U_(0x07FFFF7F)                                      /**< (CKGR_MOR) Register Mask  */


/* -------- CKGR_MCFR : (PMC Offset: 0x24) (R/W 32) Main Clock Frequency Register -------- */
#define CKGR_MCFR_MAINF_Pos                   _U_(0)                                               /**< (CKGR_MCFR) Main Clock Frequency Position */
#define CKGR_MCFR_MAINF_Msk                   (_U_(0xFFFF) << CKGR_MCFR_MAINF_Pos)                 /**< (CKGR_MCFR) Main Clock Frequency Mask */
#define CKGR_MCFR_MAINF(value)                (CKGR_MCFR_MAINF_Msk & ((value) << CKGR_MCFR_MAINF_Pos))
#define CKGR_MCFR_MAINFRDY_Pos                _U_(16)                                              /**< (CKGR_MCFR) Main Clock Frequency Measure Ready Position */
#define CKGR_MCFR_MAINFRDY_Msk                (_U_(0x1) << CKGR_MCFR_MAINFRDY_Pos)                 /**< (CKGR_MCFR) Main Clock Frequency Measure Ready Mask */
#define CKGR_MCFR_MAINFRDY(value)             (CKGR_MCFR_MAINFRDY_Msk & ((value) << CKGR_MCFR_MAINFRDY_Pos))
#define CKGR_MCFR_RCMEAS_Pos                  _U_(20)                                              /**< (CKGR_MCFR) RC Oscillator Frequency Measure (write-only) Position */
#define CKGR_MCFR_RCMEAS_Msk                  (_U_(0x1) << CKGR_MCFR_RCMEAS_Pos)                   /**< (CKGR_MCFR) RC Oscillator Frequency Measure (write-only) Mask */
#define CKGR_MCFR_RCMEAS(value)               (CKGR_MCFR_RCMEAS_Msk & ((value) << CKGR_MCFR_RCMEAS_Pos))
#define CKGR_MCFR_CCSS_Pos                    _U_(24)                                              /**< (CKGR_MCFR) Counter Clock Source Selection Position */
#define CKGR_MCFR_CCSS_Msk                    (_U_(0x1) << CKGR_MCFR_CCSS_Pos)                     /**< (CKGR_MCFR) Counter Clock Source Selection Mask */
#define CKGR_MCFR_CCSS(value)                 (CKGR_MCFR_CCSS_Msk & ((value) << CKGR_MCFR_CCSS_Pos))
#define CKGR_MCFR_Msk                         _U_(0x0111FFFF)                                      /**< (CKGR_MCFR) Register Mask  */


/* -------- CKGR_PLLAR : (PMC Offset: 0x28) (R/W 32) PLLA Register -------- */
#define CKGR_PLLAR_DIVA_Pos                   _U_(0)                                               /**< (CKGR_PLLAR) PLLA Front End Divider Position */
#define CKGR_PLLAR_DIVA_Msk                   (_U_(0xFF) << CKGR_PLLAR_DIVA_Pos)                   /**< (CKGR_PLLAR) PLLA Front End Divider Mask */
#define CKGR_PLLAR_DIVA(value)                (CKGR_PLLAR_DIVA_Msk & ((value) << CKGR_PLLAR_DIVA_Pos))
#define   CKGR_PLLAR_DIVA_0_Val               _U_(0x0)                                             /**< (CKGR_PLLAR) Divider output is 0 and PLLA is disabled.  */
#define   CKGR_PLLAR_DIVA_BYPASS_Val          _U_(0x1)                                             /**< (CKGR_PLLAR) Divider is bypassed (divide by 1) and PLLA is enabled.  */
#define CKGR_PLLAR_DIVA_0                     (CKGR_PLLAR_DIVA_0_Val << CKGR_PLLAR_DIVA_Pos)       /**< (CKGR_PLLAR) Divider output is 0 and PLLA is disabled. Position  */
#define CKGR_PLLAR_DIVA_BYPASS                (CKGR_PLLAR_DIVA_BYPASS_Val << CKGR_PLLAR_DIVA_Pos)  /**< (CKGR_PLLAR) Divider is bypassed (divide by 1) and PLLA is enabled. Position  */
#define CKGR_PLLAR_PLLACOUNT_Pos              _U_(8)                                               /**< (CKGR_PLLAR) PLLA Counter Position */
#define CKGR_PLLAR_PLLACOUNT_Msk              (_U_(0x3F) << CKGR_PLLAR_PLLACOUNT_Pos)              /**< (CKGR_PLLAR) PLLA Counter Mask */
#define CKGR_PLLAR_PLLACOUNT(value)           (CKGR_PLLAR_PLLACOUNT_Msk & ((value) << CKGR_PLLAR_PLLACOUNT_Pos))
#define CKGR_PLLAR_MULA_Pos                   _U_(16)                                              /**< (CKGR_PLLAR) PLLA Multiplier Position */
#define CKGR_PLLAR_MULA_Msk                   (_U_(0x7FF) << CKGR_PLLAR_MULA_Pos)                  /**< (CKGR_PLLAR) PLLA Multiplier Mask */
#define CKGR_PLLAR_MULA(value)                (CKGR_PLLAR_MULA_Msk & ((value) << CKGR_PLLAR_MULA_Pos))
#define CKGR_PLLAR_ONE_Pos                    _U_(29)                                              /**< (CKGR_PLLAR) Must Be Set to 1 Position */
#define CKGR_PLLAR_ONE_Msk                    (_U_(0x1) << CKGR_PLLAR_ONE_Pos)                     /**< (CKGR_PLLAR) Must Be Set to 1 Mask */
#define CKGR_PLLAR_ONE(value)                 (CKGR_PLLAR_ONE_Msk & ((value) << CKGR_PLLAR_ONE_Pos))
#define CKGR_PLLAR_Msk                        _U_(0x27FF3FFF)                                      /**< (CKGR_PLLAR) Register Mask  */


/* -------- PMC_MCKR : (PMC Offset: 0x30) (R/W 32) Master Clock Register -------- */
#define PMC_MCKR_CSS_Pos                      _U_(0)                                               /**< (PMC_MCKR) Master Clock Source Selection Position */
#define PMC_MCKR_CSS_Msk                      (_U_(0x3) << PMC_MCKR_CSS_Pos)                       /**< (PMC_MCKR) Master Clock Source Selection Mask */
#define PMC_MCKR_CSS(value)                   (PMC_MCKR_CSS_Msk & ((value) << PMC_MCKR_CSS_Pos))  
#define   PMC_MCKR_CSS_SLOW_CLK_Val           _U_(0x0)                                             /**< (PMC_MCKR) Slow Clock is selected  */
#define   PMC_MCKR_CSS_MAIN_CLK_Val           _U_(0x1)                                             /**< (PMC_MCKR) Main Clock is selected  */
#define   PMC_MCKR_CSS_PLLA_CLK_Val           _U_(0x2)                                             /**< (PMC_MCKR) PLLA Clock is selected  */
#define   PMC_MCKR_CSS_UPLL_CLK_Val           _U_(0x3)                                             /**< (PMC_MCKR) Divided UPLL Clock is selected  */
#define PMC_MCKR_CSS_SLOW_CLK                 (PMC_MCKR_CSS_SLOW_CLK_Val << PMC_MCKR_CSS_Pos)      /**< (PMC_MCKR) Slow Clock is selected Position  */
#define PMC_MCKR_CSS_MAIN_CLK                 (PMC_MCKR_CSS_MAIN_CLK_Val << PMC_MCKR_CSS_Pos)      /**< (PMC_MCKR) Main Clock is selected Position  */
#define PMC_MCKR_CSS_PLLA_CLK                 (PMC_MCKR_CSS_PLLA_CLK_Val << PMC_MCKR_CSS_Pos)      /**< (PMC_MCKR) PLLA Clock is selected Position  */
#define PMC_MCKR_CSS_UPLL_CLK                 (PMC_MCKR_CSS_UPLL_CLK_Val << PMC_MCKR_CSS_Pos)      /**< (PMC_MCKR) Divided UPLL Clock is selected Position  */
#define PMC_MCKR_PRES_Pos                     _U_(4)                                               /**< (PMC_MCKR) Processor Clock Prescaler Position */
#define PMC_MCKR_PRES_Msk                     (_U_(0x7) << PMC_MCKR_PRES_Pos)                      /**< (PMC_MCKR) Processor Clock Prescaler Mask */
#define PMC_MCKR_PRES(value)                  (PMC_MCKR_PRES_Msk & ((value) << PMC_MCKR_PRES_Pos))
#define   PMC_MCKR_PRES_CLK_1_Val             _U_(0x0)                                             /**< (PMC_MCKR) Selected clock  */
#define   PMC_MCKR_PRES_CLK_2_Val             _U_(0x1)                                             /**< (PMC_MCKR) Selected clock divided by 2  */
#define   PMC_MCKR_PRES_CLK_4_Val             _U_(0x2)                                             /**< (PMC_MCKR) Selected clock divided by 4  */
#define   PMC_MCKR_PRES_CLK_8_Val             _U_(0x3)                                             /**< (PMC_MCKR) Selected clock divided by 8  */
#define   PMC_MCKR_PRES_CLK_16_Val            _U_(0x4)                                             /**< (PMC_MCKR) Selected clock divided by 16  */
#define   PMC_MCKR_PRES_CLK_32_Val            _U_(0x5)                                             /**< (PMC_MCKR) Selected clock divided by 32  */
#define   PMC_MCKR_PRES_CLK_64_Val            _U_(0x6)                                             /**< (PMC_MCKR) Selected clock divided by 64  */
#define   PMC_MCKR_PRES_CLK_3_Val             _U_(0x7)                                             /**< (PMC_MCKR) Selected clock divided by 3  */
#define PMC_MCKR_PRES_CLK_1                   (PMC_MCKR_PRES_CLK_1_Val << PMC_MCKR_PRES_Pos)       /**< (PMC_MCKR) Selected clock Position  */
#define PMC_MCKR_PRES_CLK_2                   (PMC_MCKR_PRES_CLK_2_Val << PMC_MCKR_PRES_Pos)       /**< (PMC_MCKR) Selected clock divided by 2 Position  */
#define PMC_MCKR_PRES_CLK_4                   (PMC_MCKR_PRES_CLK_4_Val << PMC_MCKR_PRES_Pos)       /**< (PMC_MCKR) Selected clock divided by 4 Position  */
#define PMC_MCKR_PRES_CLK_8                   (PMC_MCKR_PRES_CLK_8_Val << PMC_MCKR_PRES_Pos)       /**< (PMC_MCKR) Selected clock divided by 8 Position  */
#define PMC_MCKR_PRES_CLK_16                  (PMC_MCKR_PRES_CLK_16_Val << PMC_MCKR_PRES_Pos)      /**< (PMC_MCKR) Selected clock divided by 16 Position  */
#define PMC_MCKR_PRES_CLK_32                  (PMC_MCKR_PRES_CLK_32_Val << PMC_MCKR_PRES_Pos)      /**< (PMC_MCKR) Selected clock divided by 32 Position  */
#define PMC_MCKR_PRES_CLK_64                  (PMC_MCKR_PRES_CLK_64_Val << PMC_MCKR_PRES_Pos)      /**< (PMC_MCKR) Selected clock divided by 64 Position  */
#define PMC_MCKR_PRES_CLK_3                   (PMC_MCKR_PRES_CLK_3_Val << PMC_MCKR_PRES_Pos)       /**< (PMC_MCKR) Selected clock divided by 3 Position  */
#define PMC_MCKR_MDIV_Pos                     _U_(8)                                               /**< (PMC_MCKR) Master Clock Division Position */
#define PMC_MCKR_MDIV_Msk                     (_U_(0x3) << PMC_MCKR_MDIV_Pos)                      /**< (PMC_MCKR) Master Clock Division Mask */
#define PMC_MCKR_MDIV(value)                  (PMC_MCKR_MDIV_Msk & ((value) << PMC_MCKR_MDIV_Pos))
#define   PMC_MCKR_MDIV_EQ_PCK_Val            _U_(0x0)                                             /**< (PMC_MCKR) Master Clock is Prescaler Output Clock divided by 1.  */
#define   PMC_MCKR_MDIV_PCK_DIV2_Val          _U_(0x1)                                             /**< (PMC_MCKR) Master Clock is Prescaler Output Clock divided by 2.  */
#define   PMC_MCKR_MDIV_PCK_DIV4_Val          _U_(0x2)                                             /**< (PMC_MCKR) Master Clock is Prescaler Output Clock divided by 4.  */
#define   PMC_MCKR_MDIV_PCK_DIV3_Val          _U_(0x3)                                             /**< (PMC_MCKR) Master Clock is Prescaler Output Clock divided by 3.  */
#define PMC_MCKR_MDIV_EQ_PCK                  (PMC_MCKR_MDIV_EQ_PCK_Val << PMC_MCKR_MDIV_Pos)      /**< (PMC_MCKR) Master Clock is Prescaler Output Clock divided by 1. Position  */
#define PMC_MCKR_MDIV_PCK_DIV2                (PMC_MCKR_MDIV_PCK_DIV2_Val << PMC_MCKR_MDIV_Pos)    /**< (PMC_MCKR) Master Clock is Prescaler Output Clock divided by 2. Position  */
#define PMC_MCKR_MDIV_PCK_DIV4                (PMC_MCKR_MDIV_PCK_DIV4_Val << PMC_MCKR_MDIV_Pos)    /**< (PMC_MCKR) Master Clock is Prescaler Output Clock divided by 4. Position  */
#define PMC_MCKR_MDIV_PCK_DIV3                (PMC_MCKR_MDIV_PCK_DIV3_Val << PMC_MCKR_MDIV_Pos)    /**< (PMC_MCKR) Master Clock is Prescaler Output Clock divided by 3. Position  */
#define PMC_MCKR_UPLLDIV2_Pos                 _U_(13)                                              /**< (PMC_MCKR) UPLL Divider by 2 Position */
#define PMC_MCKR_UPLLDIV2_Msk                 (_U_(0x1) << PMC_MCKR_UPLLDIV2_Pos)                  /**< (PMC_MCKR) UPLL Divider by 2 Mask */
#define PMC_MCKR_UPLLDIV2(value)              (PMC_MCKR_UPLLDIV2_Msk & ((value) << PMC_MCKR_UPLLDIV2_Pos))
#define PMC_MCKR_Msk                          _U_(0x00002373)                                      /**< (PMC_MCKR) Register Mask  */

#define PMC_MCKR_UPLLDIV_Pos                  _U_(13)                                              /**< (PMC_MCKR Position) UPLL Divider by 2 */
#define PMC_MCKR_UPLLDIV_Msk                  (_U_(0x1) << PMC_MCKR_UPLLDIV_Pos)                   /**< (PMC_MCKR Mask) UPLLDIV */
#define PMC_MCKR_UPLLDIV(value)               (PMC_MCKR_UPLLDIV_Msk & ((value) << PMC_MCKR_UPLLDIV_Pos)) 

/* -------- PMC_USB : (PMC Offset: 0x38) (R/W 32) USB Clock Register -------- */
#define PMC_USB_USBS_Pos                      _U_(0)                                               /**< (PMC_USB) USB Input Clock Selection Position */
#define PMC_USB_USBS_Msk                      (_U_(0x1) << PMC_USB_USBS_Pos)                       /**< (PMC_USB) USB Input Clock Selection Mask */
#define PMC_USB_USBS(value)                   (PMC_USB_USBS_Msk & ((value) << PMC_USB_USBS_Pos))  
#define PMC_USB_USBDIV_Pos                    _U_(8)                                               /**< (PMC_USB) Divider for USB_48M Position */
#define PMC_USB_USBDIV_Msk                    (_U_(0xF) << PMC_USB_USBDIV_Pos)                     /**< (PMC_USB) Divider for USB_48M Mask */
#define PMC_USB_USBDIV(value)                 (PMC_USB_USBDIV_Msk & ((value) << PMC_USB_USBDIV_Pos))
#define PMC_USB_Msk                           _U_(0x00000F01)                                      /**< (PMC_USB) Register Mask  */


/* -------- PMC_PCK : (PMC Offset: 0x40) (R/W 32) Programmable Clock Register -------- */
#define PMC_PCK_CSS_Pos                       _U_(0)                                               /**< (PMC_PCK) Programmable Clock Source Selection Position */
#define PMC_PCK_CSS_Msk                       (_U_(0x7) << PMC_PCK_CSS_Pos)                        /**< (PMC_PCK) Programmable Clock Source Selection Mask */
#define PMC_PCK_CSS(value)                    (PMC_PCK_CSS_Msk & ((value) << PMC_PCK_CSS_Pos))    
#define   PMC_PCK_CSS_SLOW_CLK_Val            _U_(0x0)                                             /**< (PMC_PCK) SLCK is selected  */
#define   PMC_PCK_CSS_MAIN_CLK_Val            _U_(0x1)                                             /**< (PMC_PCK) MAINCK is selected  */
#define   PMC_PCK_CSS_PLLA_CLK_Val            _U_(0x2)                                             /**< (PMC_PCK) PLLACK is selected  */
#define   PMC_PCK_CSS_UPLL_CLK_Val            _U_(0x3)                                             /**< (PMC_PCK) UPLLCKDIV is selected  */
#define   PMC_PCK_CSS_MCK_Val                 _U_(0x4)                                             /**< (PMC_PCK) MCK is selected  */
#define PMC_PCK_CSS_SLOW_CLK                  (PMC_PCK_CSS_SLOW_CLK_Val << PMC_PCK_CSS_Pos)        /**< (PMC_PCK) SLCK is selected Position  */
#define PMC_PCK_CSS_MAIN_CLK                  (PMC_PCK_CSS_MAIN_CLK_Val << PMC_PCK_CSS_Pos)        /**< (PMC_PCK) MAINCK is selected Position  */
#define PMC_PCK_CSS_PLLA_CLK                  (PMC_PCK_CSS_PLLA_CLK_Val << PMC_PCK_CSS_Pos)        /**< (PMC_PCK) PLLACK is selected Position  */
#define PMC_PCK_CSS_UPLL_CLK                  (PMC_PCK_CSS_UPLL_CLK_Val << PMC_PCK_CSS_Pos)        /**< (PMC_PCK) UPLLCKDIV is selected Position  */
#define PMC_PCK_CSS_MCK                       (PMC_PCK_CSS_MCK_Val << PMC_PCK_CSS_Pos)             /**< (PMC_PCK) MCK is selected Position  */
#define PMC_PCK_PRES_Pos                      _U_(4)                                               /**< (PMC_PCK) Programmable Clock Prescaler Position */
#define PMC_PCK_PRES_Msk                      (_U_(0xFF) << PMC_PCK_PRES_Pos)                      /**< (PMC_PCK) Programmable Clock Prescaler Mask */
#define PMC_PCK_PRES(value)                   (PMC_PCK_PRES_Msk & ((value) << PMC_PCK_PRES_Pos))  
#define PMC_PCK_Msk                           _U_(0x00000FF7)                                      /**< (PMC_PCK) Register Mask  */


/* -------- PMC_IER : (PMC Offset: 0x60) ( /W 32) Interrupt Enable Register -------- */
#define PMC_IER_MOSCXTS_Pos                   _U_(0)                                               /**< (PMC_IER) Main Crystal Oscillator Status Interrupt Enable Position */
#define PMC_IER_MOSCXTS_Msk                   (_U_(0x1) << PMC_IER_MOSCXTS_Pos)                    /**< (PMC_IER) Main Crystal Oscillator Status Interrupt Enable Mask */
#define PMC_IER_MOSCXTS(value)                (PMC_IER_MOSCXTS_Msk & ((value) << PMC_IER_MOSCXTS_Pos))
#define PMC_IER_LOCKA_Pos                     _U_(1)                                               /**< (PMC_IER) PLLA Lock Interrupt Enable Position */
#define PMC_IER_LOCKA_Msk                     (_U_(0x1) << PMC_IER_LOCKA_Pos)                      /**< (PMC_IER) PLLA Lock Interrupt Enable Mask */
#define PMC_IER_LOCKA(value)                  (PMC_IER_LOCKA_Msk & ((value) << PMC_IER_LOCKA_Pos))
#define PMC_IER_MCKRDY_Pos                    _U_(3)                                               /**< (PMC_IER) Master Clock Ready Interrupt Enable Position */
#define PMC_IER_MCKRDY_Msk                    (_U_(0x1) << PMC_IER_MCKRDY_Pos)                     /**< (PMC_IER) Master Clock Ready Interrupt Enable Mask */
#define PMC_IER_MCKRDY(value)                 (PMC_IER_MCKRDY_Msk & ((value) << PMC_IER_MCKRDY_Pos))
#define PMC_IER_LOCKU_Pos                     _U_(6)                                               /**< (PMC_IER) UTMI PLL Lock Interrupt Enable Position */
#define PMC_IER_LOCKU_Msk                     (_U_(0x1) << PMC_IER_LOCKU_Pos)                      /**< (PMC_IER) UTMI PLL Lock Interrupt Enable Mask */
#define PMC_IER_LOCKU(value)                  (PMC_IER_LOCKU_Msk & ((value) << PMC_IER_LOCKU_Pos))
#define PMC_IER_PCKRDY0_Pos                   _U_(8)                                               /**< (PMC_IER) Programmable Clock Ready 0 Interrupt Enable Position */
#define PMC_IER_PCKRDY0_Msk                   (_U_(0x1) << PMC_IER_PCKRDY0_Pos)                    /**< (PMC_IER) Programmable Clock Ready 0 Interrupt Enable Mask */
#define PMC_IER_PCKRDY0(value)                (PMC_IER_PCKRDY0_Msk & ((value) << PMC_IER_PCKRDY0_Pos))
#define PMC_IER_PCKRDY1_Pos                   _U_(9)                                               /**< (PMC_IER) Programmable Clock Ready 1 Interrupt Enable Position */
#define PMC_IER_PCKRDY1_Msk                   (_U_(0x1) << PMC_IER_PCKRDY1_Pos)                    /**< (PMC_IER) Programmable Clock Ready 1 Interrupt Enable Mask */
#define PMC_IER_PCKRDY1(value)                (PMC_IER_PCKRDY1_Msk & ((value) << PMC_IER_PCKRDY1_Pos))
#define PMC_IER_PCKRDY2_Pos                   _U_(10)                                              /**< (PMC_IER) Programmable Clock Ready 2 Interrupt Enable Position */
#define PMC_IER_PCKRDY2_Msk                   (_U_(0x1) << PMC_IER_PCKRDY2_Pos)                    /**< (PMC_IER) Programmable Clock Ready 2 Interrupt Enable Mask */
#define PMC_IER_PCKRDY2(value)                (PMC_IER_PCKRDY2_Msk & ((value) << PMC_IER_PCKRDY2_Pos))
#define PMC_IER_PCKRDY3_Pos                   _U_(11)                                              /**< (PMC_IER) Programmable Clock Ready 3 Interrupt Enable Position */
#define PMC_IER_PCKRDY3_Msk                   (_U_(0x1) << PMC_IER_PCKRDY3_Pos)                    /**< (PMC_IER) Programmable Clock Ready 3 Interrupt Enable Mask */
#define PMC_IER_PCKRDY3(value)                (PMC_IER_PCKRDY3_Msk & ((value) << PMC_IER_PCKRDY3_Pos))
#define PMC_IER_PCKRDY4_Pos                   _U_(12)                                              /**< (PMC_IER) Programmable Clock Ready 4 Interrupt Enable Position */
#define PMC_IER_PCKRDY4_Msk                   (_U_(0x1) << PMC_IER_PCKRDY4_Pos)                    /**< (PMC_IER) Programmable Clock Ready 4 Interrupt Enable Mask */
#define PMC_IER_PCKRDY4(value)                (PMC_IER_PCKRDY4_Msk & ((value) << PMC_IER_PCKRDY4_Pos))
#define PMC_IER_PCKRDY5_Pos                   _U_(13)                                              /**< (PMC_IER) Programmable Clock Ready 5 Interrupt Enable Position */
#define PMC_IER_PCKRDY5_Msk                   (_U_(0x1) << PMC_IER_PCKRDY5_Pos)                    /**< (PMC_IER) Programmable Clock Ready 5 Interrupt Enable Mask */
#define PMC_IER_PCKRDY5(value)                (PMC_IER_PCKRDY5_Msk & ((value) << PMC_IER_PCKRDY5_Pos))
#define PMC_IER_PCKRDY6_Pos                   _U_(14)                                              /**< (PMC_IER) Programmable Clock Ready 6 Interrupt Enable Position */
#define PMC_IER_PCKRDY6_Msk                   (_U_(0x1) << PMC_IER_PCKRDY6_Pos)                    /**< (PMC_IER) Programmable Clock Ready 6 Interrupt Enable Mask */
#define PMC_IER_PCKRDY6(value)                (PMC_IER_PCKRDY6_Msk & ((value) << PMC_IER_PCKRDY6_Pos))
#define PMC_IER_PCKRDY7_Pos                   _U_(15)                                              /**< (PMC_IER) Programmable Clock Ready 7 Interrupt Enable Position */
#define PMC_IER_PCKRDY7_Msk                   (_U_(0x1) << PMC_IER_PCKRDY7_Pos)                    /**< (PMC_IER) Programmable Clock Ready 7 Interrupt Enable Mask */
#define PMC_IER_PCKRDY7(value)                (PMC_IER_PCKRDY7_Msk & ((value) << PMC_IER_PCKRDY7_Pos))
#define PMC_IER_MOSCSELS_Pos                  _U_(16)                                              /**< (PMC_IER) Main Clock Source Oscillator Selection Status Interrupt Enable Position */
#define PMC_IER_MOSCSELS_Msk                  (_U_(0x1) << PMC_IER_MOSCSELS_Pos)                   /**< (PMC_IER) Main Clock Source Oscillator Selection Status Interrupt Enable Mask */
#define PMC_IER_MOSCSELS(value)               (PMC_IER_MOSCSELS_Msk & ((value) << PMC_IER_MOSCSELS_Pos))
#define PMC_IER_MOSCRCS_Pos                   _U_(17)                                              /**< (PMC_IER) Main RC Oscillator Status Interrupt Enable Position */
#define PMC_IER_MOSCRCS_Msk                   (_U_(0x1) << PMC_IER_MOSCRCS_Pos)                    /**< (PMC_IER) Main RC Oscillator Status Interrupt Enable Mask */
#define PMC_IER_MOSCRCS(value)                (PMC_IER_MOSCRCS_Msk & ((value) << PMC_IER_MOSCRCS_Pos))
#define PMC_IER_CFDEV_Pos                     _U_(18)                                              /**< (PMC_IER) Clock Failure Detector Event Interrupt Enable Position */
#define PMC_IER_CFDEV_Msk                     (_U_(0x1) << PMC_IER_CFDEV_Pos)                      /**< (PMC_IER) Clock Failure Detector Event Interrupt Enable Mask */
#define PMC_IER_CFDEV(value)                  (PMC_IER_CFDEV_Msk & ((value) << PMC_IER_CFDEV_Pos))
#define PMC_IER_XT32KERR_Pos                  _U_(21)                                              /**< (PMC_IER) 32.768 kHz Crystal Oscillator Error Interrupt Enable Position */
#define PMC_IER_XT32KERR_Msk                  (_U_(0x1) << PMC_IER_XT32KERR_Pos)                   /**< (PMC_IER) 32.768 kHz Crystal Oscillator Error Interrupt Enable Mask */
#define PMC_IER_XT32KERR(value)               (PMC_IER_XT32KERR_Msk & ((value) << PMC_IER_XT32KERR_Pos))
#define PMC_IER_Msk                           _U_(0x0027FF4B)                                      /**< (PMC_IER) Register Mask  */

#define PMC_IER_PCKRDY_Pos                    _U_(8)                                               /**< (PMC_IER Position) Programmable Clock Ready x Interrupt Enable */
#define PMC_IER_PCKRDY_Msk                    (_U_(0xFF) << PMC_IER_PCKRDY_Pos)                    /**< (PMC_IER Mask) PCKRDY */
#define PMC_IER_PCKRDY(value)                 (PMC_IER_PCKRDY_Msk & ((value) << PMC_IER_PCKRDY_Pos)) 

/* -------- PMC_IDR : (PMC Offset: 0x64) ( /W 32) Interrupt Disable Register -------- */
#define PMC_IDR_MOSCXTS_Pos                   _U_(0)                                               /**< (PMC_IDR) Main Crystal Oscillator Status Interrupt Disable Position */
#define PMC_IDR_MOSCXTS_Msk                   (_U_(0x1) << PMC_IDR_MOSCXTS_Pos)                    /**< (PMC_IDR) Main Crystal Oscillator Status Interrupt Disable Mask */
#define PMC_IDR_MOSCXTS(value)                (PMC_IDR_MOSCXTS_Msk & ((value) << PMC_IDR_MOSCXTS_Pos))
#define PMC_IDR_LOCKA_Pos                     _U_(1)                                               /**< (PMC_IDR) PLLA Lock Interrupt Disable Position */
#define PMC_IDR_LOCKA_Msk                     (_U_(0x1) << PMC_IDR_LOCKA_Pos)                      /**< (PMC_IDR) PLLA Lock Interrupt Disable Mask */
#define PMC_IDR_LOCKA(value)                  (PMC_IDR_LOCKA_Msk & ((value) << PMC_IDR_LOCKA_Pos))
#define PMC_IDR_MCKRDY_Pos                    _U_(3)                                               /**< (PMC_IDR) Master Clock Ready Interrupt Disable Position */
#define PMC_IDR_MCKRDY_Msk                    (_U_(0x1) << PMC_IDR_MCKRDY_Pos)                     /**< (PMC_IDR) Master Clock Ready Interrupt Disable Mask */
#define PMC_IDR_MCKRDY(value)                 (PMC_IDR_MCKRDY_Msk & ((value) << PMC_IDR_MCKRDY_Pos))
#define PMC_IDR_LOCKU_Pos                     _U_(6)                                               /**< (PMC_IDR) UTMI PLL Lock Interrupt Disable Position */
#define PMC_IDR_LOCKU_Msk                     (_U_(0x1) << PMC_IDR_LOCKU_Pos)                      /**< (PMC_IDR) UTMI PLL Lock Interrupt Disable Mask */
#define PMC_IDR_LOCKU(value)                  (PMC_IDR_LOCKU_Msk & ((value) << PMC_IDR_LOCKU_Pos))
#define PMC_IDR_PCKRDY0_Pos                   _U_(8)                                               /**< (PMC_IDR) Programmable Clock Ready 0 Interrupt Disable Position */
#define PMC_IDR_PCKRDY0_Msk                   (_U_(0x1) << PMC_IDR_PCKRDY0_Pos)                    /**< (PMC_IDR) Programmable Clock Ready 0 Interrupt Disable Mask */
#define PMC_IDR_PCKRDY0(value)                (PMC_IDR_PCKRDY0_Msk & ((value) << PMC_IDR_PCKRDY0_Pos))
#define PMC_IDR_PCKRDY1_Pos                   _U_(9)                                               /**< (PMC_IDR) Programmable Clock Ready 1 Interrupt Disable Position */
#define PMC_IDR_PCKRDY1_Msk                   (_U_(0x1) << PMC_IDR_PCKRDY1_Pos)                    /**< (PMC_IDR) Programmable Clock Ready 1 Interrupt Disable Mask */
#define PMC_IDR_PCKRDY1(value)                (PMC_IDR_PCKRDY1_Msk & ((value) << PMC_IDR_PCKRDY1_Pos))
#define PMC_IDR_PCKRDY2_Pos                   _U_(10)                                              /**< (PMC_IDR) Programmable Clock Ready 2 Interrupt Disable Position */
#define PMC_IDR_PCKRDY2_Msk                   (_U_(0x1) << PMC_IDR_PCKRDY2_Pos)                    /**< (PMC_IDR) Programmable Clock Ready 2 Interrupt Disable Mask */
#define PMC_IDR_PCKRDY2(value)                (PMC_IDR_PCKRDY2_Msk & ((value) << PMC_IDR_PCKRDY2_Pos))
#define PMC_IDR_PCKRDY3_Pos                   _U_(11)                                              /**< (PMC_IDR) Programmable Clock Ready 3 Interrupt Disable Position */
#define PMC_IDR_PCKRDY3_Msk                   (_U_(0x1) << PMC_IDR_PCKRDY3_Pos)                    /**< (PMC_IDR) Programmable Clock Ready 3 Interrupt Disable Mask */
#define PMC_IDR_PCKRDY3(value)                (PMC_IDR_PCKRDY3_Msk & ((value) << PMC_IDR_PCKRDY3_Pos))
#define PMC_IDR_PCKRDY4_Pos                   _U_(12)                                              /**< (PMC_IDR) Programmable Clock Ready 4 Interrupt Disable Position */
#define PMC_IDR_PCKRDY4_Msk                   (_U_(0x1) << PMC_IDR_PCKRDY4_Pos)                    /**< (PMC_IDR) Programmable Clock Ready 4 Interrupt Disable Mask */
#define PMC_IDR_PCKRDY4(value)                (PMC_IDR_PCKRDY4_Msk & ((value) << PMC_IDR_PCKRDY4_Pos))
#define PMC_IDR_PCKRDY5_Pos                   _U_(13)                                              /**< (PMC_IDR) Programmable Clock Ready 5 Interrupt Disable Position */
#define PMC_IDR_PCKRDY5_Msk                   (_U_(0x1) << PMC_IDR_PCKRDY5_Pos)                    /**< (PMC_IDR) Programmable Clock Ready 5 Interrupt Disable Mask */
#define PMC_IDR_PCKRDY5(value)                (PMC_IDR_PCKRDY5_Msk & ((value) << PMC_IDR_PCKRDY5_Pos))
#define PMC_IDR_PCKRDY6_Pos                   _U_(14)                                              /**< (PMC_IDR) Programmable Clock Ready 6 Interrupt Disable Position */
#define PMC_IDR_PCKRDY6_Msk                   (_U_(0x1) << PMC_IDR_PCKRDY6_Pos)                    /**< (PMC_IDR) Programmable Clock Ready 6 Interrupt Disable Mask */
#define PMC_IDR_PCKRDY6(value)                (PMC_IDR_PCKRDY6_Msk & ((value) << PMC_IDR_PCKRDY6_Pos))
#define PMC_IDR_PCKRDY7_Pos                   _U_(15)                                              /**< (PMC_IDR) Programmable Clock Ready 7 Interrupt Disable Position */
#define PMC_IDR_PCKRDY7_Msk                   (_U_(0x1) << PMC_IDR_PCKRDY7_Pos)                    /**< (PMC_IDR) Programmable Clock Ready 7 Interrupt Disable Mask */
#define PMC_IDR_PCKRDY7(value)                (PMC_IDR_PCKRDY7_Msk & ((value) << PMC_IDR_PCKRDY7_Pos))
#define PMC_IDR_MOSCSELS_Pos                  _U_(16)                                              /**< (PMC_IDR) Main Clock Source Oscillator Selection Status Interrupt Disable Position */
#define PMC_IDR_MOSCSELS_Msk                  (_U_(0x1) << PMC_IDR_MOSCSELS_Pos)                   /**< (PMC_IDR) Main Clock Source Oscillator Selection Status Interrupt Disable Mask */
#define PMC_IDR_MOSCSELS(value)               (PMC_IDR_MOSCSELS_Msk & ((value) << PMC_IDR_MOSCSELS_Pos))
#define PMC_IDR_MOSCRCS_Pos                   _U_(17)                                              /**< (PMC_IDR) Main RC Status Interrupt Disable Position */
#define PMC_IDR_MOSCRCS_Msk                   (_U_(0x1) << PMC_IDR_MOSCRCS_Pos)                    /**< (PMC_IDR) Main RC Status Interrupt Disable Mask */
#define PMC_IDR_MOSCRCS(value)                (PMC_IDR_MOSCRCS_Msk & ((value) << PMC_IDR_MOSCRCS_Pos))
#define PMC_IDR_CFDEV_Pos                     _U_(18)                                              /**< (PMC_IDR) Clock Failure Detector Event Interrupt Disable Position */
#define PMC_IDR_CFDEV_Msk                     (_U_(0x1) << PMC_IDR_CFDEV_Pos)                      /**< (PMC_IDR) Clock Failure Detector Event Interrupt Disable Mask */
#define PMC_IDR_CFDEV(value)                  (PMC_IDR_CFDEV_Msk & ((value) << PMC_IDR_CFDEV_Pos))
#define PMC_IDR_XT32KERR_Pos                  _U_(21)                                              /**< (PMC_IDR) 32.768 kHz Crystal Oscillator Error Interrupt Disable Position */
#define PMC_IDR_XT32KERR_Msk                  (_U_(0x1) << PMC_IDR_XT32KERR_Pos)                   /**< (PMC_IDR) 32.768 kHz Crystal Oscillator Error Interrupt Disable Mask */
#define PMC_IDR_XT32KERR(value)               (PMC_IDR_XT32KERR_Msk & ((value) << PMC_IDR_XT32KERR_Pos))
#define PMC_IDR_Msk                           _U_(0x0027FF4B)                                      /**< (PMC_IDR) Register Mask  */

#define PMC_IDR_PCKRDY_Pos                    _U_(8)                                               /**< (PMC_IDR Position) Programmable Clock Ready x Interrupt Disable */
#define PMC_IDR_PCKRDY_Msk                    (_U_(0xFF) << PMC_IDR_PCKRDY_Pos)                    /**< (PMC_IDR Mask) PCKRDY */
#define PMC_IDR_PCKRDY(value)                 (PMC_IDR_PCKRDY_Msk & ((value) << PMC_IDR_PCKRDY_Pos)) 

/* -------- PMC_SR : (PMC Offset: 0x68) ( R/ 32) Status Register -------- */
#define PMC_SR_MOSCXTS_Pos                    _U_(0)                                               /**< (PMC_SR) Main Crystal Oscillator Status Position */
#define PMC_SR_MOSCXTS_Msk                    (_U_(0x1) << PMC_SR_MOSCXTS_Pos)                     /**< (PMC_SR) Main Crystal Oscillator Status Mask */
#define PMC_SR_MOSCXTS(value)                 (PMC_SR_MOSCXTS_Msk & ((value) << PMC_SR_MOSCXTS_Pos))
#define PMC_SR_LOCKA_Pos                      _U_(1)                                               /**< (PMC_SR) PLLA Lock Status Position */
#define PMC_SR_LOCKA_Msk                      (_U_(0x1) << PMC_SR_LOCKA_Pos)                       /**< (PMC_SR) PLLA Lock Status Mask */
#define PMC_SR_LOCKA(value)                   (PMC_SR_LOCKA_Msk & ((value) << PMC_SR_LOCKA_Pos))  
#define PMC_SR_MCKRDY_Pos                     _U_(3)                                               /**< (PMC_SR) Master Clock Status Position */
#define PMC_SR_MCKRDY_Msk                     (_U_(0x1) << PMC_SR_MCKRDY_Pos)                      /**< (PMC_SR) Master Clock Status Mask */
#define PMC_SR_MCKRDY(value)                  (PMC_SR_MCKRDY_Msk & ((value) << PMC_SR_MCKRDY_Pos))
#define PMC_SR_LOCKU_Pos                      _U_(6)                                               /**< (PMC_SR) UTMI PLL Lock Status Position */
#define PMC_SR_LOCKU_Msk                      (_U_(0x1) << PMC_SR_LOCKU_Pos)                       /**< (PMC_SR) UTMI PLL Lock Status Mask */
#define PMC_SR_LOCKU(value)                   (PMC_SR_LOCKU_Msk & ((value) << PMC_SR_LOCKU_Pos))  
#define PMC_SR_OSCSELS_Pos                    _U_(7)                                               /**< (PMC_SR) Slow Clock Source Oscillator Selection Position */
#define PMC_SR_OSCSELS_Msk                    (_U_(0x1) << PMC_SR_OSCSELS_Pos)                     /**< (PMC_SR) Slow Clock Source Oscillator Selection Mask */
#define PMC_SR_OSCSELS(value)                 (PMC_SR_OSCSELS_Msk & ((value) << PMC_SR_OSCSELS_Pos))
#define PMC_SR_PCKRDY0_Pos                    _U_(8)                                               /**< (PMC_SR) Programmable Clock Ready 0 Status Position */
#define PMC_SR_PCKRDY0_Msk                    (_U_(0x1) << PMC_SR_PCKRDY0_Pos)                     /**< (PMC_SR) Programmable Clock Ready 0 Status Mask */
#define PMC_SR_PCKRDY0(value)                 (PMC_SR_PCKRDY0_Msk & ((value) << PMC_SR_PCKRDY0_Pos))
#define PMC_SR_PCKRDY1_Pos                    _U_(9)                                               /**< (PMC_SR) Programmable Clock Ready 1 Status Position */
#define PMC_SR_PCKRDY1_Msk                    (_U_(0x1) << PMC_SR_PCKRDY1_Pos)                     /**< (PMC_SR) Programmable Clock Ready 1 Status Mask */
#define PMC_SR_PCKRDY1(value)                 (PMC_SR_PCKRDY1_Msk & ((value) << PMC_SR_PCKRDY1_Pos))
#define PMC_SR_PCKRDY2_Pos                    _U_(10)                                              /**< (PMC_SR) Programmable Clock Ready 2 Status Position */
#define PMC_SR_PCKRDY2_Msk                    (_U_(0x1) << PMC_SR_PCKRDY2_Pos)                     /**< (PMC_SR) Programmable Clock Ready 2 Status Mask */
#define PMC_SR_PCKRDY2(value)                 (PMC_SR_PCKRDY2_Msk & ((value) << PMC_SR_PCKRDY2_Pos))
#define PMC_SR_PCKRDY3_Pos                    _U_(11)                                              /**< (PMC_SR) Programmable Clock Ready 3 Status Position */
#define PMC_SR_PCKRDY3_Msk                    (_U_(0x1) << PMC_SR_PCKRDY3_Pos)                     /**< (PMC_SR) Programmable Clock Ready 3 Status Mask */
#define PMC_SR_PCKRDY3(value)                 (PMC_SR_PCKRDY3_Msk & ((value) << PMC_SR_PCKRDY3_Pos))
#define PMC_SR_PCKRDY4_Pos                    _U_(12)                                              /**< (PMC_SR) Programmable Clock Ready 4 Status Position */
#define PMC_SR_PCKRDY4_Msk                    (_U_(0x1) << PMC_SR_PCKRDY4_Pos)                     /**< (PMC_SR) Programmable Clock Ready 4 Status Mask */
#define PMC_SR_PCKRDY4(value)                 (PMC_SR_PCKRDY4_Msk & ((value) << PMC_SR_PCKRDY4_Pos))
#define PMC_SR_PCKRDY5_Pos                    _U_(13)                                              /**< (PMC_SR) Programmable Clock Ready 5 Status Position */
#define PMC_SR_PCKRDY5_Msk                    (_U_(0x1) << PMC_SR_PCKRDY5_Pos)                     /**< (PMC_SR) Programmable Clock Ready 5 Status Mask */
#define PMC_SR_PCKRDY5(value)                 (PMC_SR_PCKRDY5_Msk & ((value) << PMC_SR_PCKRDY5_Pos))
#define PMC_SR_PCKRDY6_Pos                    _U_(14)                                              /**< (PMC_SR) Programmable Clock Ready 6 Status Position */
#define PMC_SR_PCKRDY6_Msk                    (_U_(0x1) << PMC_SR_PCKRDY6_Pos)                     /**< (PMC_SR) Programmable Clock Ready 6 Status Mask */
#define PMC_SR_PCKRDY6(value)                 (PMC_SR_PCKRDY6_Msk & ((value) << PMC_SR_PCKRDY6_Pos))
#define PMC_SR_PCKRDY7_Pos                    _U_(15)                                              /**< (PMC_SR) Programmable Clock Ready 7 Status Position */
#define PMC_SR_PCKRDY7_Msk                    (_U_(0x1) << PMC_SR_PCKRDY7_Pos)                     /**< (PMC_SR) Programmable Clock Ready 7 Status Mask */
#define PMC_SR_PCKRDY7(value)                 (PMC_SR_PCKRDY7_Msk & ((value) << PMC_SR_PCKRDY7_Pos))
#define PMC_SR_MOSCSELS_Pos                   _U_(16)                                              /**< (PMC_SR) Main Clock Source Oscillator Selection Status Position */
#define PMC_SR_MOSCSELS_Msk                   (_U_(0x1) << PMC_SR_MOSCSELS_Pos)                    /**< (PMC_SR) Main Clock Source Oscillator Selection Status Mask */
#define PMC_SR_MOSCSELS(value)                (PMC_SR_MOSCSELS_Msk & ((value) << PMC_SR_MOSCSELS_Pos))
#define PMC_SR_MOSCRCS_Pos                    _U_(17)                                              /**< (PMC_SR) Main RC Oscillator Status Position */
#define PMC_SR_MOSCRCS_Msk                    (_U_(0x1) << PMC_SR_MOSCRCS_Pos)                     /**< (PMC_SR) Main RC Oscillator Status Mask */
#define PMC_SR_MOSCRCS(value)                 (PMC_SR_MOSCRCS_Msk & ((value) << PMC_SR_MOSCRCS_Pos))
#define PMC_SR_CFDEV_Pos                      _U_(18)                                              /**< (PMC_SR) Clock Failure Detector Event Position */
#define PMC_SR_CFDEV_Msk                      (_U_(0x1) << PMC_SR_CFDEV_Pos)                       /**< (PMC_SR) Clock Failure Detector Event Mask */
#define PMC_SR_CFDEV(value)                   (PMC_SR_CFDEV_Msk & ((value) << PMC_SR_CFDEV_Pos))  
#define PMC_SR_CFDS_Pos                       _U_(19)                                              /**< (PMC_SR) Clock Failure Detector Status Position */
#define PMC_SR_CFDS_Msk                       (_U_(0x1) << PMC_SR_CFDS_Pos)                        /**< (PMC_SR) Clock Failure Detector Status Mask */
#define PMC_SR_CFDS(value)                    (PMC_SR_CFDS_Msk & ((value) << PMC_SR_CFDS_Pos))    
#define PMC_SR_FOS_Pos                        _U_(20)                                              /**< (PMC_SR) Clock Failure Detector Fault Output Status Position */
#define PMC_SR_FOS_Msk                        (_U_(0x1) << PMC_SR_FOS_Pos)                         /**< (PMC_SR) Clock Failure Detector Fault Output Status Mask */
#define PMC_SR_FOS(value)                     (PMC_SR_FOS_Msk & ((value) << PMC_SR_FOS_Pos))      
#define PMC_SR_XT32KERR_Pos                   _U_(21)                                              /**< (PMC_SR) Slow Crystal Oscillator Error Position */
#define PMC_SR_XT32KERR_Msk                   (_U_(0x1) << PMC_SR_XT32KERR_Pos)                    /**< (PMC_SR) Slow Crystal Oscillator Error Mask */
#define PMC_SR_XT32KERR(value)                (PMC_SR_XT32KERR_Msk & ((value) << PMC_SR_XT32KERR_Pos))
#define PMC_SR_Msk                            _U_(0x003FFFCB)                                      /**< (PMC_SR) Register Mask  */

#define PMC_SR_PCKRDY_Pos                     _U_(8)                                               /**< (PMC_SR Position) Programmable Clock Ready x Status */
#define PMC_SR_PCKRDY_Msk                     (_U_(0xFF) << PMC_SR_PCKRDY_Pos)                     /**< (PMC_SR Mask) PCKRDY */
#define PMC_SR_PCKRDY(value)                  (PMC_SR_PCKRDY_Msk & ((value) << PMC_SR_PCKRDY_Pos)) 

/* -------- PMC_IMR : (PMC Offset: 0x6C) ( R/ 32) Interrupt Mask Register -------- */
#define PMC_IMR_MOSCXTS_Pos                   _U_(0)                                               /**< (PMC_IMR) Main Crystal Oscillator Status Interrupt Mask Position */
#define PMC_IMR_MOSCXTS_Msk                   (_U_(0x1) << PMC_IMR_MOSCXTS_Pos)                    /**< (PMC_IMR) Main Crystal Oscillator Status Interrupt Mask Mask */
#define PMC_IMR_MOSCXTS(value)                (PMC_IMR_MOSCXTS_Msk & ((value) << PMC_IMR_MOSCXTS_Pos))
#define PMC_IMR_LOCKA_Pos                     _U_(1)                                               /**< (PMC_IMR) PLLA Lock Interrupt Mask Position */
#define PMC_IMR_LOCKA_Msk                     (_U_(0x1) << PMC_IMR_LOCKA_Pos)                      /**< (PMC_IMR) PLLA Lock Interrupt Mask Mask */
#define PMC_IMR_LOCKA(value)                  (PMC_IMR_LOCKA_Msk & ((value) << PMC_IMR_LOCKA_Pos))
#define PMC_IMR_MCKRDY_Pos                    _U_(3)                                               /**< (PMC_IMR) Master Clock Ready Interrupt Mask Position */
#define PMC_IMR_MCKRDY_Msk                    (_U_(0x1) << PMC_IMR_MCKRDY_Pos)                     /**< (PMC_IMR) Master Clock Ready Interrupt Mask Mask */
#define PMC_IMR_MCKRDY(value)                 (PMC_IMR_MCKRDY_Msk & ((value) << PMC_IMR_MCKRDY_Pos))
#define PMC_IMR_LOCKU_Pos                     _U_(6)                                               /**< (PMC_IMR) UTMI PLL Lock Interrupt Mask Position */
#define PMC_IMR_LOCKU_Msk                     (_U_(0x1) << PMC_IMR_LOCKU_Pos)                      /**< (PMC_IMR) UTMI PLL Lock Interrupt Mask Mask */
#define PMC_IMR_LOCKU(value)                  (PMC_IMR_LOCKU_Msk & ((value) << PMC_IMR_LOCKU_Pos))
#define PMC_IMR_PCKRDY0_Pos                   _U_(8)                                               /**< (PMC_IMR) Programmable Clock Ready 0 Interrupt Mask Position */
#define PMC_IMR_PCKRDY0_Msk                   (_U_(0x1) << PMC_IMR_PCKRDY0_Pos)                    /**< (PMC_IMR) Programmable Clock Ready 0 Interrupt Mask Mask */
#define PMC_IMR_PCKRDY0(value)                (PMC_IMR_PCKRDY0_Msk & ((value) << PMC_IMR_PCKRDY0_Pos))
#define PMC_IMR_PCKRDY1_Pos                   _U_(9)                                               /**< (PMC_IMR) Programmable Clock Ready 1 Interrupt Mask Position */
#define PMC_IMR_PCKRDY1_Msk                   (_U_(0x1) << PMC_IMR_PCKRDY1_Pos)                    /**< (PMC_IMR) Programmable Clock Ready 1 Interrupt Mask Mask */
#define PMC_IMR_PCKRDY1(value)                (PMC_IMR_PCKRDY1_Msk & ((value) << PMC_IMR_PCKRDY1_Pos))
#define PMC_IMR_PCKRDY2_Pos                   _U_(10)                                              /**< (PMC_IMR) Programmable Clock Ready 2 Interrupt Mask Position */
#define PMC_IMR_PCKRDY2_Msk                   (_U_(0x1) << PMC_IMR_PCKRDY2_Pos)                    /**< (PMC_IMR) Programmable Clock Ready 2 Interrupt Mask Mask */
#define PMC_IMR_PCKRDY2(value)                (PMC_IMR_PCKRDY2_Msk & ((value) << PMC_IMR_PCKRDY2_Pos))
#define PMC_IMR_PCKRDY3_Pos                   _U_(11)                                              /**< (PMC_IMR) Programmable Clock Ready 3 Interrupt Mask Position */
#define PMC_IMR_PCKRDY3_Msk                   (_U_(0x1) << PMC_IMR_PCKRDY3_Pos)                    /**< (PMC_IMR) Programmable Clock Ready 3 Interrupt Mask Mask */
#define PMC_IMR_PCKRDY3(value)                (PMC_IMR_PCKRDY3_Msk & ((value) << PMC_IMR_PCKRDY3_Pos))
#define PMC_IMR_PCKRDY4_Pos                   _U_(12)                                              /**< (PMC_IMR) Programmable Clock Ready 4 Interrupt Mask Position */
#define PMC_IMR_PCKRDY4_Msk                   (_U_(0x1) << PMC_IMR_PCKRDY4_Pos)                    /**< (PMC_IMR) Programmable Clock Ready 4 Interrupt Mask Mask */
#define PMC_IMR_PCKRDY4(value)                (PMC_IMR_PCKRDY4_Msk & ((value) << PMC_IMR_PCKRDY4_Pos))
#define PMC_IMR_PCKRDY5_Pos                   _U_(13)                                              /**< (PMC_IMR) Programmable Clock Ready 5 Interrupt Mask Position */
#define PMC_IMR_PCKRDY5_Msk                   (_U_(0x1) << PMC_IMR_PCKRDY5_Pos)                    /**< (PMC_IMR) Programmable Clock Ready 5 Interrupt Mask Mask */
#define PMC_IMR_PCKRDY5(value)                (PMC_IMR_PCKRDY5_Msk & ((value) << PMC_IMR_PCKRDY5_Pos))
#define PMC_IMR_PCKRDY6_Pos                   _U_(14)                                              /**< (PMC_IMR) Programmable Clock Ready 6 Interrupt Mask Position */
#define PMC_IMR_PCKRDY6_Msk                   (_U_(0x1) << PMC_IMR_PCKRDY6_Pos)                    /**< (PMC_IMR) Programmable Clock Ready 6 Interrupt Mask Mask */
#define PMC_IMR_PCKRDY6(value)                (PMC_IMR_PCKRDY6_Msk & ((value) << PMC_IMR_PCKRDY6_Pos))
#define PMC_IMR_PCKRDY7_Pos                   _U_(15)                                              /**< (PMC_IMR) Programmable Clock Ready 7 Interrupt Mask Position */
#define PMC_IMR_PCKRDY7_Msk                   (_U_(0x1) << PMC_IMR_PCKRDY7_Pos)                    /**< (PMC_IMR) Programmable Clock Ready 7 Interrupt Mask Mask */
#define PMC_IMR_PCKRDY7(value)                (PMC_IMR_PCKRDY7_Msk & ((value) << PMC_IMR_PCKRDY7_Pos))
#define PMC_IMR_MOSCSELS_Pos                  _U_(16)                                              /**< (PMC_IMR) Main Clock Source Oscillator Selection Status Interrupt Mask Position */
#define PMC_IMR_MOSCSELS_Msk                  (_U_(0x1) << PMC_IMR_MOSCSELS_Pos)                   /**< (PMC_IMR) Main Clock Source Oscillator Selection Status Interrupt Mask Mask */
#define PMC_IMR_MOSCSELS(value)               (PMC_IMR_MOSCSELS_Msk & ((value) << PMC_IMR_MOSCSELS_Pos))
#define PMC_IMR_MOSCRCS_Pos                   _U_(17)                                              /**< (PMC_IMR) Main RC Status Interrupt Mask Position */
#define PMC_IMR_MOSCRCS_Msk                   (_U_(0x1) << PMC_IMR_MOSCRCS_Pos)                    /**< (PMC_IMR) Main RC Status Interrupt Mask Mask */
#define PMC_IMR_MOSCRCS(value)                (PMC_IMR_MOSCRCS_Msk & ((value) << PMC_IMR_MOSCRCS_Pos))
#define PMC_IMR_CFDEV_Pos                     _U_(18)                                              /**< (PMC_IMR) Clock Failure Detector Event Interrupt Mask Position */
#define PMC_IMR_CFDEV_Msk                     (_U_(0x1) << PMC_IMR_CFDEV_Pos)                      /**< (PMC_IMR) Clock Failure Detector Event Interrupt Mask Mask */
#define PMC_IMR_CFDEV(value)                  (PMC_IMR_CFDEV_Msk & ((value) << PMC_IMR_CFDEV_Pos))
#define PMC_IMR_XT32KERR_Pos                  _U_(21)                                              /**< (PMC_IMR) 32.768 kHz Crystal Oscillator Error Interrupt Mask Position */
#define PMC_IMR_XT32KERR_Msk                  (_U_(0x1) << PMC_IMR_XT32KERR_Pos)                   /**< (PMC_IMR) 32.768 kHz Crystal Oscillator Error Interrupt Mask Mask */
#define PMC_IMR_XT32KERR(value)               (PMC_IMR_XT32KERR_Msk & ((value) << PMC_IMR_XT32KERR_Pos))
#define PMC_IMR_Msk                           _U_(0x0027FF4B)                                      /**< (PMC_IMR) Register Mask  */

#define PMC_IMR_PCKRDY_Pos                    _U_(8)                                               /**< (PMC_IMR Position) Programmable Clock Ready x Interrupt Mask */
#define PMC_IMR_PCKRDY_Msk                    (_U_(0xFF) << PMC_IMR_PCKRDY_Pos)                    /**< (PMC_IMR Mask) PCKRDY */
#define PMC_IMR_PCKRDY(value)                 (PMC_IMR_PCKRDY_Msk & ((value) << PMC_IMR_PCKRDY_Pos)) 

/* -------- PMC_FSMR : (PMC Offset: 0x70) (R/W 32) Fast Startup Mode Register -------- */
#define PMC_FSMR_FSTT0_Pos                    _U_(0)                                               /**< (PMC_FSMR) Fast Startup Input Enable 0 Position */
#define PMC_FSMR_FSTT0_Msk                    (_U_(0x1) << PMC_FSMR_FSTT0_Pos)                     /**< (PMC_FSMR) Fast Startup Input Enable 0 Mask */
#define PMC_FSMR_FSTT0(value)                 (PMC_FSMR_FSTT0_Msk & ((value) << PMC_FSMR_FSTT0_Pos))
#define PMC_FSMR_FSTT1_Pos                    _U_(1)                                               /**< (PMC_FSMR) Fast Startup Input Enable 1 Position */
#define PMC_FSMR_FSTT1_Msk                    (_U_(0x1) << PMC_FSMR_FSTT1_Pos)                     /**< (PMC_FSMR) Fast Startup Input Enable 1 Mask */
#define PMC_FSMR_FSTT1(value)                 (PMC_FSMR_FSTT1_Msk & ((value) << PMC_FSMR_FSTT1_Pos))
#define PMC_FSMR_FSTT2_Pos                    _U_(2)                                               /**< (PMC_FSMR) Fast Startup Input Enable 2 Position */
#define PMC_FSMR_FSTT2_Msk                    (_U_(0x1) << PMC_FSMR_FSTT2_Pos)                     /**< (PMC_FSMR) Fast Startup Input Enable 2 Mask */
#define PMC_FSMR_FSTT2(value)                 (PMC_FSMR_FSTT2_Msk & ((value) << PMC_FSMR_FSTT2_Pos))
#define PMC_FSMR_FSTT3_Pos                    _U_(3)                                               /**< (PMC_FSMR) Fast Startup Input Enable 3 Position */
#define PMC_FSMR_FSTT3_Msk                    (_U_(0x1) << PMC_FSMR_FSTT3_Pos)                     /**< (PMC_FSMR) Fast Startup Input Enable 3 Mask */
#define PMC_FSMR_FSTT3(value)                 (PMC_FSMR_FSTT3_Msk & ((value) << PMC_FSMR_FSTT3_Pos))
#define PMC_FSMR_FSTT4_Pos                    _U_(4)                                               /**< (PMC_FSMR) Fast Startup Input Enable 4 Position */
#define PMC_FSMR_FSTT4_Msk                    (_U_(0x1) << PMC_FSMR_FSTT4_Pos)                     /**< (PMC_FSMR) Fast Startup Input Enable 4 Mask */
#define PMC_FSMR_FSTT4(value)                 (PMC_FSMR_FSTT4_Msk & ((value) << PMC_FSMR_FSTT4_Pos))
#define PMC_FSMR_FSTT5_Pos                    _U_(5)                                               /**< (PMC_FSMR) Fast Startup Input Enable 5 Position */
#define PMC_FSMR_FSTT5_Msk                    (_U_(0x1) << PMC_FSMR_FSTT5_Pos)                     /**< (PMC_FSMR) Fast Startup Input Enable 5 Mask */
#define PMC_FSMR_FSTT5(value)                 (PMC_FSMR_FSTT5_Msk & ((value) << PMC_FSMR_FSTT5_Pos))
#define PMC_FSMR_FSTT6_Pos                    _U_(6)                                               /**< (PMC_FSMR) Fast Startup Input Enable 6 Position */
#define PMC_FSMR_FSTT6_Msk                    (_U_(0x1) << PMC_FSMR_FSTT6_Pos)                     /**< (PMC_FSMR) Fast Startup Input Enable 6 Mask */
#define PMC_FSMR_FSTT6(value)                 (PMC_FSMR_FSTT6_Msk & ((value) << PMC_FSMR_FSTT6_Pos))
#define PMC_FSMR_FSTT7_Pos                    _U_(7)                                               /**< (PMC_FSMR) Fast Startup Input Enable 7 Position */
#define PMC_FSMR_FSTT7_Msk                    (_U_(0x1) << PMC_FSMR_FSTT7_Pos)                     /**< (PMC_FSMR) Fast Startup Input Enable 7 Mask */
#define PMC_FSMR_FSTT7(value)                 (PMC_FSMR_FSTT7_Msk & ((value) << PMC_FSMR_FSTT7_Pos))
#define PMC_FSMR_FSTT8_Pos                    _U_(8)                                               /**< (PMC_FSMR) Fast Startup Input Enable 8 Position */
#define PMC_FSMR_FSTT8_Msk                    (_U_(0x1) << PMC_FSMR_FSTT8_Pos)                     /**< (PMC_FSMR) Fast Startup Input Enable 8 Mask */
#define PMC_FSMR_FSTT8(value)                 (PMC_FSMR_FSTT8_Msk & ((value) << PMC_FSMR_FSTT8_Pos))
#define PMC_FSMR_FSTT9_Pos                    _U_(9)                                               /**< (PMC_FSMR) Fast Startup Input Enable 9 Position */
#define PMC_FSMR_FSTT9_Msk                    (_U_(0x1) << PMC_FSMR_FSTT9_Pos)                     /**< (PMC_FSMR) Fast Startup Input Enable 9 Mask */
#define PMC_FSMR_FSTT9(value)                 (PMC_FSMR_FSTT9_Msk & ((value) << PMC_FSMR_FSTT9_Pos))
#define PMC_FSMR_FSTT10_Pos                   _U_(10)                                              /**< (PMC_FSMR) Fast Startup Input Enable 10 Position */
#define PMC_FSMR_FSTT10_Msk                   (_U_(0x1) << PMC_FSMR_FSTT10_Pos)                    /**< (PMC_FSMR) Fast Startup Input Enable 10 Mask */
#define PMC_FSMR_FSTT10(value)                (PMC_FSMR_FSTT10_Msk & ((value) << PMC_FSMR_FSTT10_Pos))
#define PMC_FSMR_FSTT11_Pos                   _U_(11)                                              /**< (PMC_FSMR) Fast Startup Input Enable 11 Position */
#define PMC_FSMR_FSTT11_Msk                   (_U_(0x1) << PMC_FSMR_FSTT11_Pos)                    /**< (PMC_FSMR) Fast Startup Input Enable 11 Mask */
#define PMC_FSMR_FSTT11(value)                (PMC_FSMR_FSTT11_Msk & ((value) << PMC_FSMR_FSTT11_Pos))
#define PMC_FSMR_FSTT12_Pos                   _U_(12)                                              /**< (PMC_FSMR) Fast Startup Input Enable 12 Position */
#define PMC_FSMR_FSTT12_Msk                   (_U_(0x1) << PMC_FSMR_FSTT12_Pos)                    /**< (PMC_FSMR) Fast Startup Input Enable 12 Mask */
#define PMC_FSMR_FSTT12(value)                (PMC_FSMR_FSTT12_Msk & ((value) << PMC_FSMR_FSTT12_Pos))
#define PMC_FSMR_FSTT13_Pos                   _U_(13)                                              /**< (PMC_FSMR) Fast Startup Input Enable 13 Position */
#define PMC_FSMR_FSTT13_Msk                   (_U_(0x1) << PMC_FSMR_FSTT13_Pos)                    /**< (PMC_FSMR) Fast Startup Input Enable 13 Mask */
#define PMC_FSMR_FSTT13(value)                (PMC_FSMR_FSTT13_Msk & ((value) << PMC_FSMR_FSTT13_Pos))
#define PMC_FSMR_FSTT14_Pos                   _U_(14)                                              /**< (PMC_FSMR) Fast Startup Input Enable 14 Position */
#define PMC_FSMR_FSTT14_Msk                   (_U_(0x1) << PMC_FSMR_FSTT14_Pos)                    /**< (PMC_FSMR) Fast Startup Input Enable 14 Mask */
#define PMC_FSMR_FSTT14(value)                (PMC_FSMR_FSTT14_Msk & ((value) << PMC_FSMR_FSTT14_Pos))
#define PMC_FSMR_FSTT15_Pos                   _U_(15)                                              /**< (PMC_FSMR) Fast Startup Input Enable 15 Position */
#define PMC_FSMR_FSTT15_Msk                   (_U_(0x1) << PMC_FSMR_FSTT15_Pos)                    /**< (PMC_FSMR) Fast Startup Input Enable 15 Mask */
#define PMC_FSMR_FSTT15(value)                (PMC_FSMR_FSTT15_Msk & ((value) << PMC_FSMR_FSTT15_Pos))
#define PMC_FSMR_RTTAL_Pos                    _U_(16)                                              /**< (PMC_FSMR) RTT Alarm Enable Position */
#define PMC_FSMR_RTTAL_Msk                    (_U_(0x1) << PMC_FSMR_RTTAL_Pos)                     /**< (PMC_FSMR) RTT Alarm Enable Mask */
#define PMC_FSMR_RTTAL(value)                 (PMC_FSMR_RTTAL_Msk & ((value) << PMC_FSMR_RTTAL_Pos))
#define PMC_FSMR_RTCAL_Pos                    _U_(17)                                              /**< (PMC_FSMR) RTC Alarm Enable Position */
#define PMC_FSMR_RTCAL_Msk                    (_U_(0x1) << PMC_FSMR_RTCAL_Pos)                     /**< (PMC_FSMR) RTC Alarm Enable Mask */
#define PMC_FSMR_RTCAL(value)                 (PMC_FSMR_RTCAL_Msk & ((value) << PMC_FSMR_RTCAL_Pos))
#define PMC_FSMR_USBAL_Pos                    _U_(18)                                              /**< (PMC_FSMR) USB Alarm Enable Position */
#define PMC_FSMR_USBAL_Msk                    (_U_(0x1) << PMC_FSMR_USBAL_Pos)                     /**< (PMC_FSMR) USB Alarm Enable Mask */
#define PMC_FSMR_USBAL(value)                 (PMC_FSMR_USBAL_Msk & ((value) << PMC_FSMR_USBAL_Pos))
#define PMC_FSMR_LPM_Pos                      _U_(20)                                              /**< (PMC_FSMR) Low-power Mode Position */
#define PMC_FSMR_LPM_Msk                      (_U_(0x1) << PMC_FSMR_LPM_Pos)                       /**< (PMC_FSMR) Low-power Mode Mask */
#define PMC_FSMR_LPM(value)                   (PMC_FSMR_LPM_Msk & ((value) << PMC_FSMR_LPM_Pos))  
#define PMC_FSMR_FLPM_Pos                     _U_(21)                                              /**< (PMC_FSMR) Flash Low-power Mode Position */
#define PMC_FSMR_FLPM_Msk                     (_U_(0x3) << PMC_FSMR_FLPM_Pos)                      /**< (PMC_FSMR) Flash Low-power Mode Mask */
#define PMC_FSMR_FLPM(value)                  (PMC_FSMR_FLPM_Msk & ((value) << PMC_FSMR_FLPM_Pos))
#define   PMC_FSMR_FLPM_FLASH_STANDBY_Val     _U_(0x0)                                             /**< (PMC_FSMR) Flash is in Standby Mode when system enters Wait Mode  */
#define   PMC_FSMR_FLPM_FLASH_DEEP_POWERDOWN_Val _U_(0x1)                                             /**< (PMC_FSMR) Flash is in Deep-power-down mode when system enters Wait Mode  */
#define   PMC_FSMR_FLPM_FLASH_IDLE_Val        _U_(0x2)                                             /**< (PMC_FSMR) Idle mode  */
#define PMC_FSMR_FLPM_FLASH_STANDBY           (PMC_FSMR_FLPM_FLASH_STANDBY_Val << PMC_FSMR_FLPM_Pos) /**< (PMC_FSMR) Flash is in Standby Mode when system enters Wait Mode Position  */
#define PMC_FSMR_FLPM_FLASH_DEEP_POWERDOWN    (PMC_FSMR_FLPM_FLASH_DEEP_POWERDOWN_Val << PMC_FSMR_FLPM_Pos) /**< (PMC_FSMR) Flash is in Deep-power-down mode when system enters Wait Mode Position  */
#define PMC_FSMR_FLPM_FLASH_IDLE              (PMC_FSMR_FLPM_FLASH_IDLE_Val << PMC_FSMR_FLPM_Pos)  /**< (PMC_FSMR) Idle mode Position  */
#define PMC_FSMR_FFLPM_Pos                    _U_(23)                                              /**< (PMC_FSMR) Force Flash Low-power Mode Position */
#define PMC_FSMR_FFLPM_Msk                    (_U_(0x1) << PMC_FSMR_FFLPM_Pos)                     /**< (PMC_FSMR) Force Flash Low-power Mode Mask */
#define PMC_FSMR_FFLPM(value)                 (PMC_FSMR_FFLPM_Msk & ((value) << PMC_FSMR_FFLPM_Pos))
#define PMC_FSMR_Msk                          _U_(0x00F7FFFF)                                      /**< (PMC_FSMR) Register Mask  */

#define PMC_FSMR_FSTT_Pos                     _U_(0)                                               /**< (PMC_FSMR Position) Fast Startup Input Enable x */
#define PMC_FSMR_FSTT_Msk                     (_U_(0xFFFF) << PMC_FSMR_FSTT_Pos)                   /**< (PMC_FSMR Mask) FSTT */
#define PMC_FSMR_FSTT(value)                  (PMC_FSMR_FSTT_Msk & ((value) << PMC_FSMR_FSTT_Pos)) 

/* -------- PMC_FSPR : (PMC Offset: 0x74) (R/W 32) Fast Startup Polarity Register -------- */
#define PMC_FSPR_FSTP0_Pos                    _U_(0)                                               /**< (PMC_FSPR) Fast Startup Input Polarity 0 Position */
#define PMC_FSPR_FSTP0_Msk                    (_U_(0x1) << PMC_FSPR_FSTP0_Pos)                     /**< (PMC_FSPR) Fast Startup Input Polarity 0 Mask */
#define PMC_FSPR_FSTP0(value)                 (PMC_FSPR_FSTP0_Msk & ((value) << PMC_FSPR_FSTP0_Pos))
#define PMC_FSPR_FSTP1_Pos                    _U_(1)                                               /**< (PMC_FSPR) Fast Startup Input Polarity 1 Position */
#define PMC_FSPR_FSTP1_Msk                    (_U_(0x1) << PMC_FSPR_FSTP1_Pos)                     /**< (PMC_FSPR) Fast Startup Input Polarity 1 Mask */
#define PMC_FSPR_FSTP1(value)                 (PMC_FSPR_FSTP1_Msk & ((value) << PMC_FSPR_FSTP1_Pos))
#define PMC_FSPR_FSTP2_Pos                    _U_(2)                                               /**< (PMC_FSPR) Fast Startup Input Polarity 2 Position */
#define PMC_FSPR_FSTP2_Msk                    (_U_(0x1) << PMC_FSPR_FSTP2_Pos)                     /**< (PMC_FSPR) Fast Startup Input Polarity 2 Mask */
#define PMC_FSPR_FSTP2(value)                 (PMC_FSPR_FSTP2_Msk & ((value) << PMC_FSPR_FSTP2_Pos))
#define PMC_FSPR_FSTP3_Pos                    _U_(3)                                               /**< (PMC_FSPR) Fast Startup Input Polarity 3 Position */
#define PMC_FSPR_FSTP3_Msk                    (_U_(0x1) << PMC_FSPR_FSTP3_Pos)                     /**< (PMC_FSPR) Fast Startup Input Polarity 3 Mask */
#define PMC_FSPR_FSTP3(value)                 (PMC_FSPR_FSTP3_Msk & ((value) << PMC_FSPR_FSTP3_Pos))
#define PMC_FSPR_FSTP4_Pos                    _U_(4)                                               /**< (PMC_FSPR) Fast Startup Input Polarity 4 Position */
#define PMC_FSPR_FSTP4_Msk                    (_U_(0x1) << PMC_FSPR_FSTP4_Pos)                     /**< (PMC_FSPR) Fast Startup Input Polarity 4 Mask */
#define PMC_FSPR_FSTP4(value)                 (PMC_FSPR_FSTP4_Msk & ((value) << PMC_FSPR_FSTP4_Pos))
#define PMC_FSPR_FSTP5_Pos                    _U_(5)                                               /**< (PMC_FSPR) Fast Startup Input Polarity 5 Position */
#define PMC_FSPR_FSTP5_Msk                    (_U_(0x1) << PMC_FSPR_FSTP5_Pos)                     /**< (PMC_FSPR) Fast Startup Input Polarity 5 Mask */
#define PMC_FSPR_FSTP5(value)                 (PMC_FSPR_FSTP5_Msk & ((value) << PMC_FSPR_FSTP5_Pos))
#define PMC_FSPR_FSTP6_Pos                    _U_(6)                                               /**< (PMC_FSPR) Fast Startup Input Polarity 6 Position */
#define PMC_FSPR_FSTP6_Msk                    (_U_(0x1) << PMC_FSPR_FSTP6_Pos)                     /**< (PMC_FSPR) Fast Startup Input Polarity 6 Mask */
#define PMC_FSPR_FSTP6(value)                 (PMC_FSPR_FSTP6_Msk & ((value) << PMC_FSPR_FSTP6_Pos))
#define PMC_FSPR_FSTP7_Pos                    _U_(7)                                               /**< (PMC_FSPR) Fast Startup Input Polarity 7 Position */
#define PMC_FSPR_FSTP7_Msk                    (_U_(0x1) << PMC_FSPR_FSTP7_Pos)                     /**< (PMC_FSPR) Fast Startup Input Polarity 7 Mask */
#define PMC_FSPR_FSTP7(value)                 (PMC_FSPR_FSTP7_Msk & ((value) << PMC_FSPR_FSTP7_Pos))
#define PMC_FSPR_FSTP8_Pos                    _U_(8)                                               /**< (PMC_FSPR) Fast Startup Input Polarity 8 Position */
#define PMC_FSPR_FSTP8_Msk                    (_U_(0x1) << PMC_FSPR_FSTP8_Pos)                     /**< (PMC_FSPR) Fast Startup Input Polarity 8 Mask */
#define PMC_FSPR_FSTP8(value)                 (PMC_FSPR_FSTP8_Msk & ((value) << PMC_FSPR_FSTP8_Pos))
#define PMC_FSPR_FSTP9_Pos                    _U_(9)                                               /**< (PMC_FSPR) Fast Startup Input Polarity 9 Position */
#define PMC_FSPR_FSTP9_Msk                    (_U_(0x1) << PMC_FSPR_FSTP9_Pos)                     /**< (PMC_FSPR) Fast Startup Input Polarity 9 Mask */
#define PMC_FSPR_FSTP9(value)                 (PMC_FSPR_FSTP9_Msk & ((value) << PMC_FSPR_FSTP9_Pos))
#define PMC_FSPR_FSTP10_Pos                   _U_(10)                                              /**< (PMC_FSPR) Fast Startup Input Polarity 10 Position */
#define PMC_FSPR_FSTP10_Msk                   (_U_(0x1) << PMC_FSPR_FSTP10_Pos)                    /**< (PMC_FSPR) Fast Startup Input Polarity 10 Mask */
#define PMC_FSPR_FSTP10(value)                (PMC_FSPR_FSTP10_Msk & ((value) << PMC_FSPR_FSTP10_Pos))
#define PMC_FSPR_FSTP11_Pos                   _U_(11)                                              /**< (PMC_FSPR) Fast Startup Input Polarity 11 Position */
#define PMC_FSPR_FSTP11_Msk                   (_U_(0x1) << PMC_FSPR_FSTP11_Pos)                    /**< (PMC_FSPR) Fast Startup Input Polarity 11 Mask */
#define PMC_FSPR_FSTP11(value)                (PMC_FSPR_FSTP11_Msk & ((value) << PMC_FSPR_FSTP11_Pos))
#define PMC_FSPR_FSTP12_Pos                   _U_(12)                                              /**< (PMC_FSPR) Fast Startup Input Polarity 12 Position */
#define PMC_FSPR_FSTP12_Msk                   (_U_(0x1) << PMC_FSPR_FSTP12_Pos)                    /**< (PMC_FSPR) Fast Startup Input Polarity 12 Mask */
#define PMC_FSPR_FSTP12(value)                (PMC_FSPR_FSTP12_Msk & ((value) << PMC_FSPR_FSTP12_Pos))
#define PMC_FSPR_FSTP13_Pos                   _U_(13)                                              /**< (PMC_FSPR) Fast Startup Input Polarity 13 Position */
#define PMC_FSPR_FSTP13_Msk                   (_U_(0x1) << PMC_FSPR_FSTP13_Pos)                    /**< (PMC_FSPR) Fast Startup Input Polarity 13 Mask */
#define PMC_FSPR_FSTP13(value)                (PMC_FSPR_FSTP13_Msk & ((value) << PMC_FSPR_FSTP13_Pos))
#define PMC_FSPR_FSTP14_Pos                   _U_(14)                                              /**< (PMC_FSPR) Fast Startup Input Polarity 14 Position */
#define PMC_FSPR_FSTP14_Msk                   (_U_(0x1) << PMC_FSPR_FSTP14_Pos)                    /**< (PMC_FSPR) Fast Startup Input Polarity 14 Mask */
#define PMC_FSPR_FSTP14(value)                (PMC_FSPR_FSTP14_Msk & ((value) << PMC_FSPR_FSTP14_Pos))
#define PMC_FSPR_FSTP15_Pos                   _U_(15)                                              /**< (PMC_FSPR) Fast Startup Input Polarity 15 Position */
#define PMC_FSPR_FSTP15_Msk                   (_U_(0x1) << PMC_FSPR_FSTP15_Pos)                    /**< (PMC_FSPR) Fast Startup Input Polarity 15 Mask */
#define PMC_FSPR_FSTP15(value)                (PMC_FSPR_FSTP15_Msk & ((value) << PMC_FSPR_FSTP15_Pos))
#define PMC_FSPR_Msk                          _U_(0x0000FFFF)                                      /**< (PMC_FSPR) Register Mask  */

#define PMC_FSPR_FSTP_Pos                     _U_(0)                                               /**< (PMC_FSPR Position) Fast Startup Input Polarity x5 */
#define PMC_FSPR_FSTP_Msk                     (_U_(0xFFFF) << PMC_FSPR_FSTP_Pos)                   /**< (PMC_FSPR Mask) FSTP */
#define PMC_FSPR_FSTP(value)                  (PMC_FSPR_FSTP_Msk & ((value) << PMC_FSPR_FSTP_Pos)) 

/* -------- PMC_FOCR : (PMC Offset: 0x78) ( /W 32) Fault Output Clear Register -------- */
#define PMC_FOCR_FOCLR_Pos                    _U_(0)                                               /**< (PMC_FOCR) Fault Output Clear Position */
#define PMC_FOCR_FOCLR_Msk                    (_U_(0x1) << PMC_FOCR_FOCLR_Pos)                     /**< (PMC_FOCR) Fault Output Clear Mask */
#define PMC_FOCR_FOCLR(value)                 (PMC_FOCR_FOCLR_Msk & ((value) << PMC_FOCR_FOCLR_Pos))
#define PMC_FOCR_Msk                          _U_(0x00000001)                                      /**< (PMC_FOCR) Register Mask  */


/* -------- PMC_WPMR : (PMC Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define PMC_WPMR_WPEN_Pos                     _U_(0)                                               /**< (PMC_WPMR) Write Protection Enable Position */
#define PMC_WPMR_WPEN_Msk                     (_U_(0x1) << PMC_WPMR_WPEN_Pos)                      /**< (PMC_WPMR) Write Protection Enable Mask */
#define PMC_WPMR_WPEN(value)                  (PMC_WPMR_WPEN_Msk & ((value) << PMC_WPMR_WPEN_Pos))
#define PMC_WPMR_WPKEY_Pos                    _U_(8)                                               /**< (PMC_WPMR) Write Protection Key Position */
#define PMC_WPMR_WPKEY_Msk                    (_U_(0xFFFFFF) << PMC_WPMR_WPKEY_Pos)                /**< (PMC_WPMR) Write Protection Key Mask */
#define PMC_WPMR_WPKEY(value)                 (PMC_WPMR_WPKEY_Msk & ((value) << PMC_WPMR_WPKEY_Pos))
#define   PMC_WPMR_WPKEY_PASSWD_Val           _U_(0x504D43)                                        /**< (PMC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0.  */
#define PMC_WPMR_WPKEY_PASSWD                 (PMC_WPMR_WPKEY_PASSWD_Val << PMC_WPMR_WPKEY_Pos)    /**< (PMC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0. Position  */
#define PMC_WPMR_Msk                          _U_(0xFFFFFF01)                                      /**< (PMC_WPMR) Register Mask  */


/* -------- PMC_WPSR : (PMC Offset: 0xE8) ( R/ 32) Write Protection Status Register -------- */
#define PMC_WPSR_WPVS_Pos                     _U_(0)                                               /**< (PMC_WPSR) Write Protection Violation Status Position */
#define PMC_WPSR_WPVS_Msk                     (_U_(0x1) << PMC_WPSR_WPVS_Pos)                      /**< (PMC_WPSR) Write Protection Violation Status Mask */
#define PMC_WPSR_WPVS(value)                  (PMC_WPSR_WPVS_Msk & ((value) << PMC_WPSR_WPVS_Pos))
#define PMC_WPSR_WPVSRC_Pos                   _U_(8)                                               /**< (PMC_WPSR) Write Protection Violation Source Position */
#define PMC_WPSR_WPVSRC_Msk                   (_U_(0xFFFF) << PMC_WPSR_WPVSRC_Pos)                 /**< (PMC_WPSR) Write Protection Violation Source Mask */
#define PMC_WPSR_WPVSRC(value)                (PMC_WPSR_WPVSRC_Msk & ((value) << PMC_WPSR_WPVSRC_Pos))
#define PMC_WPSR_Msk                          _U_(0x00FFFF01)                                      /**< (PMC_WPSR) Register Mask  */


/* -------- PMC_PCER1 : (PMC Offset: 0x100) ( /W 32) Peripheral Clock Enable Register 1 -------- */
#define PMC_PCER1_PID32_Pos                   _U_(0)                                               /**< (PMC_PCER1) Peripheral Clock 32 Enable Position */
#define PMC_PCER1_PID32_Msk                   (_U_(0x1) << PMC_PCER1_PID32_Pos)                    /**< (PMC_PCER1) Peripheral Clock 32 Enable Mask */
#define PMC_PCER1_PID32(value)                (PMC_PCER1_PID32_Msk & ((value) << PMC_PCER1_PID32_Pos))
#define PMC_PCER1_PID33_Pos                   _U_(1)                                               /**< (PMC_PCER1) Peripheral Clock 33 Enable Position */
#define PMC_PCER1_PID33_Msk                   (_U_(0x1) << PMC_PCER1_PID33_Pos)                    /**< (PMC_PCER1) Peripheral Clock 33 Enable Mask */
#define PMC_PCER1_PID33(value)                (PMC_PCER1_PID33_Msk & ((value) << PMC_PCER1_PID33_Pos))
#define PMC_PCER1_PID34_Pos                   _U_(2)                                               /**< (PMC_PCER1) Peripheral Clock 34 Enable Position */
#define PMC_PCER1_PID34_Msk                   (_U_(0x1) << PMC_PCER1_PID34_Pos)                    /**< (PMC_PCER1) Peripheral Clock 34 Enable Mask */
#define PMC_PCER1_PID34(value)                (PMC_PCER1_PID34_Msk & ((value) << PMC_PCER1_PID34_Pos))
#define PMC_PCER1_PID35_Pos                   _U_(3)                                               /**< (PMC_PCER1) Peripheral Clock 35 Enable Position */
#define PMC_PCER1_PID35_Msk                   (_U_(0x1) << PMC_PCER1_PID35_Pos)                    /**< (PMC_PCER1) Peripheral Clock 35 Enable Mask */
#define PMC_PCER1_PID35(value)                (PMC_PCER1_PID35_Msk & ((value) << PMC_PCER1_PID35_Pos))
#define PMC_PCER1_PID37_Pos                   _U_(5)                                               /**< (PMC_PCER1) Peripheral Clock 37 Enable Position */
#define PMC_PCER1_PID37_Msk                   (_U_(0x1) << PMC_PCER1_PID37_Pos)                    /**< (PMC_PCER1) Peripheral Clock 37 Enable Mask */
#define PMC_PCER1_PID37(value)                (PMC_PCER1_PID37_Msk & ((value) << PMC_PCER1_PID37_Pos))
#define PMC_PCER1_PID39_Pos                   _U_(7)                                               /**< (PMC_PCER1) Peripheral Clock 39 Enable Position */
#define PMC_PCER1_PID39_Msk                   (_U_(0x1) << PMC_PCER1_PID39_Pos)                    /**< (PMC_PCER1) Peripheral Clock 39 Enable Mask */
#define PMC_PCER1_PID39(value)                (PMC_PCER1_PID39_Msk & ((value) << PMC_PCER1_PID39_Pos))
#define PMC_PCER1_PID40_Pos                   _U_(8)                                               /**< (PMC_PCER1) Peripheral Clock 40 Enable Position */
#define PMC_PCER1_PID40_Msk                   (_U_(0x1) << PMC_PCER1_PID40_Pos)                    /**< (PMC_PCER1) Peripheral Clock 40 Enable Mask */
#define PMC_PCER1_PID40(value)                (PMC_PCER1_PID40_Msk & ((value) << PMC_PCER1_PID40_Pos))
#define PMC_PCER1_PID41_Pos                   _U_(9)                                               /**< (PMC_PCER1) Peripheral Clock 41 Enable Position */
#define PMC_PCER1_PID41_Msk                   (_U_(0x1) << PMC_PCER1_PID41_Pos)                    /**< (PMC_PCER1) Peripheral Clock 41 Enable Mask */
#define PMC_PCER1_PID41(value)                (PMC_PCER1_PID41_Msk & ((value) << PMC_PCER1_PID41_Pos))
#define PMC_PCER1_PID42_Pos                   _U_(10)                                              /**< (PMC_PCER1) Peripheral Clock 42 Enable Position */
#define PMC_PCER1_PID42_Msk                   (_U_(0x1) << PMC_PCER1_PID42_Pos)                    /**< (PMC_PCER1) Peripheral Clock 42 Enable Mask */
#define PMC_PCER1_PID42(value)                (PMC_PCER1_PID42_Msk & ((value) << PMC_PCER1_PID42_Pos))
#define PMC_PCER1_PID43_Pos                   _U_(11)                                              /**< (PMC_PCER1) Peripheral Clock 43 Enable Position */
#define PMC_PCER1_PID43_Msk                   (_U_(0x1) << PMC_PCER1_PID43_Pos)                    /**< (PMC_PCER1) Peripheral Clock 43 Enable Mask */
#define PMC_PCER1_PID43(value)                (PMC_PCER1_PID43_Msk & ((value) << PMC_PCER1_PID43_Pos))
#define PMC_PCER1_PID44_Pos                   _U_(12)                                              /**< (PMC_PCER1) Peripheral Clock 44 Enable Position */
#define PMC_PCER1_PID44_Msk                   (_U_(0x1) << PMC_PCER1_PID44_Pos)                    /**< (PMC_PCER1) Peripheral Clock 44 Enable Mask */
#define PMC_PCER1_PID44(value)                (PMC_PCER1_PID44_Msk & ((value) << PMC_PCER1_PID44_Pos))
#define PMC_PCER1_PID45_Pos                   _U_(13)                                              /**< (PMC_PCER1) Peripheral Clock 45 Enable Position */
#define PMC_PCER1_PID45_Msk                   (_U_(0x1) << PMC_PCER1_PID45_Pos)                    /**< (PMC_PCER1) Peripheral Clock 45 Enable Mask */
#define PMC_PCER1_PID45(value)                (PMC_PCER1_PID45_Msk & ((value) << PMC_PCER1_PID45_Pos))
#define PMC_PCER1_PID46_Pos                   _U_(14)                                              /**< (PMC_PCER1) Peripheral Clock 46 Enable Position */
#define PMC_PCER1_PID46_Msk                   (_U_(0x1) << PMC_PCER1_PID46_Pos)                    /**< (PMC_PCER1) Peripheral Clock 46 Enable Mask */
#define PMC_PCER1_PID46(value)                (PMC_PCER1_PID46_Msk & ((value) << PMC_PCER1_PID46_Pos))
#define PMC_PCER1_PID47_Pos                   _U_(15)                                              /**< (PMC_PCER1) Peripheral Clock 47 Enable Position */
#define PMC_PCER1_PID47_Msk                   (_U_(0x1) << PMC_PCER1_PID47_Pos)                    /**< (PMC_PCER1) Peripheral Clock 47 Enable Mask */
#define PMC_PCER1_PID47(value)                (PMC_PCER1_PID47_Msk & ((value) << PMC_PCER1_PID47_Pos))
#define PMC_PCER1_PID48_Pos                   _U_(16)                                              /**< (PMC_PCER1) Peripheral Clock 48 Enable Position */
#define PMC_PCER1_PID48_Msk                   (_U_(0x1) << PMC_PCER1_PID48_Pos)                    /**< (PMC_PCER1) Peripheral Clock 48 Enable Mask */
#define PMC_PCER1_PID48(value)                (PMC_PCER1_PID48_Msk & ((value) << PMC_PCER1_PID48_Pos))
#define PMC_PCER1_PID49_Pos                   _U_(17)                                              /**< (PMC_PCER1) Peripheral Clock 49 Enable Position */
#define PMC_PCER1_PID49_Msk                   (_U_(0x1) << PMC_PCER1_PID49_Pos)                    /**< (PMC_PCER1) Peripheral Clock 49 Enable Mask */
#define PMC_PCER1_PID49(value)                (PMC_PCER1_PID49_Msk & ((value) << PMC_PCER1_PID49_Pos))
#define PMC_PCER1_PID50_Pos                   _U_(18)                                              /**< (PMC_PCER1) Peripheral Clock 50 Enable Position */
#define PMC_PCER1_PID50_Msk                   (_U_(0x1) << PMC_PCER1_PID50_Pos)                    /**< (PMC_PCER1) Peripheral Clock 50 Enable Mask */
#define PMC_PCER1_PID50(value)                (PMC_PCER1_PID50_Msk & ((value) << PMC_PCER1_PID50_Pos))
#define PMC_PCER1_PID51_Pos                   _U_(19)                                              /**< (PMC_PCER1) Peripheral Clock 51 Enable Position */
#define PMC_PCER1_PID51_Msk                   (_U_(0x1) << PMC_PCER1_PID51_Pos)                    /**< (PMC_PCER1) Peripheral Clock 51 Enable Mask */
#define PMC_PCER1_PID51(value)                (PMC_PCER1_PID51_Msk & ((value) << PMC_PCER1_PID51_Pos))
#define PMC_PCER1_PID52_Pos                   _U_(20)                                              /**< (PMC_PCER1) Peripheral Clock 52 Enable Position */
#define PMC_PCER1_PID52_Msk                   (_U_(0x1) << PMC_PCER1_PID52_Pos)                    /**< (PMC_PCER1) Peripheral Clock 52 Enable Mask */
#define PMC_PCER1_PID52(value)                (PMC_PCER1_PID52_Msk & ((value) << PMC_PCER1_PID52_Pos))
#define PMC_PCER1_PID56_Pos                   _U_(24)                                              /**< (PMC_PCER1) Peripheral Clock 56 Enable Position */
#define PMC_PCER1_PID56_Msk                   (_U_(0x1) << PMC_PCER1_PID56_Pos)                    /**< (PMC_PCER1) Peripheral Clock 56 Enable Mask */
#define PMC_PCER1_PID56(value)                (PMC_PCER1_PID56_Msk & ((value) << PMC_PCER1_PID56_Pos))
#define PMC_PCER1_PID57_Pos                   _U_(25)                                              /**< (PMC_PCER1) Peripheral Clock 57 Enable Position */
#define PMC_PCER1_PID57_Msk                   (_U_(0x1) << PMC_PCER1_PID57_Pos)                    /**< (PMC_PCER1) Peripheral Clock 57 Enable Mask */
#define PMC_PCER1_PID57(value)                (PMC_PCER1_PID57_Msk & ((value) << PMC_PCER1_PID57_Pos))
#define PMC_PCER1_PID58_Pos                   _U_(26)                                              /**< (PMC_PCER1) Peripheral Clock 58 Enable Position */
#define PMC_PCER1_PID58_Msk                   (_U_(0x1) << PMC_PCER1_PID58_Pos)                    /**< (PMC_PCER1) Peripheral Clock 58 Enable Mask */
#define PMC_PCER1_PID58(value)                (PMC_PCER1_PID58_Msk & ((value) << PMC_PCER1_PID58_Pos))
#define PMC_PCER1_PID59_Pos                   _U_(27)                                              /**< (PMC_PCER1) Peripheral Clock 59 Enable Position */
#define PMC_PCER1_PID59_Msk                   (_U_(0x1) << PMC_PCER1_PID59_Pos)                    /**< (PMC_PCER1) Peripheral Clock 59 Enable Mask */
#define PMC_PCER1_PID59(value)                (PMC_PCER1_PID59_Msk & ((value) << PMC_PCER1_PID59_Pos))
#define PMC_PCER1_PID60_Pos                   _U_(28)                                              /**< (PMC_PCER1) Peripheral Clock 60 Enable Position */
#define PMC_PCER1_PID60_Msk                   (_U_(0x1) << PMC_PCER1_PID60_Pos)                    /**< (PMC_PCER1) Peripheral Clock 60 Enable Mask */
#define PMC_PCER1_PID60(value)                (PMC_PCER1_PID60_Msk & ((value) << PMC_PCER1_PID60_Pos))
#define PMC_PCER1_Msk                         _U_(0x1F1FFFAF)                                      /**< (PMC_PCER1) Register Mask  */

#define PMC_PCER1_PID_Pos                     _U_(0)                                               /**< (PMC_PCER1 Position) Peripheral Clock 6x Enable */
#define PMC_PCER1_PID_Msk                     (_U_(0xFFFFFF) << PMC_PCER1_PID_Pos)                 /**< (PMC_PCER1 Mask) PID */
#define PMC_PCER1_PID(value)                  (PMC_PCER1_PID_Msk & ((value) << PMC_PCER1_PID_Pos)) 

/* -------- PMC_PCDR1 : (PMC Offset: 0x104) ( /W 32) Peripheral Clock Disable Register 1 -------- */
#define PMC_PCDR1_PID32_Pos                   _U_(0)                                               /**< (PMC_PCDR1) Peripheral Clock 32 Disable Position */
#define PMC_PCDR1_PID32_Msk                   (_U_(0x1) << PMC_PCDR1_PID32_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 32 Disable Mask */
#define PMC_PCDR1_PID32(value)                (PMC_PCDR1_PID32_Msk & ((value) << PMC_PCDR1_PID32_Pos))
#define PMC_PCDR1_PID33_Pos                   _U_(1)                                               /**< (PMC_PCDR1) Peripheral Clock 33 Disable Position */
#define PMC_PCDR1_PID33_Msk                   (_U_(0x1) << PMC_PCDR1_PID33_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 33 Disable Mask */
#define PMC_PCDR1_PID33(value)                (PMC_PCDR1_PID33_Msk & ((value) << PMC_PCDR1_PID33_Pos))
#define PMC_PCDR1_PID34_Pos                   _U_(2)                                               /**< (PMC_PCDR1) Peripheral Clock 34 Disable Position */
#define PMC_PCDR1_PID34_Msk                   (_U_(0x1) << PMC_PCDR1_PID34_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 34 Disable Mask */
#define PMC_PCDR1_PID34(value)                (PMC_PCDR1_PID34_Msk & ((value) << PMC_PCDR1_PID34_Pos))
#define PMC_PCDR1_PID35_Pos                   _U_(3)                                               /**< (PMC_PCDR1) Peripheral Clock 35 Disable Position */
#define PMC_PCDR1_PID35_Msk                   (_U_(0x1) << PMC_PCDR1_PID35_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 35 Disable Mask */
#define PMC_PCDR1_PID35(value)                (PMC_PCDR1_PID35_Msk & ((value) << PMC_PCDR1_PID35_Pos))
#define PMC_PCDR1_PID37_Pos                   _U_(5)                                               /**< (PMC_PCDR1) Peripheral Clock 37 Disable Position */
#define PMC_PCDR1_PID37_Msk                   (_U_(0x1) << PMC_PCDR1_PID37_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 37 Disable Mask */
#define PMC_PCDR1_PID37(value)                (PMC_PCDR1_PID37_Msk & ((value) << PMC_PCDR1_PID37_Pos))
#define PMC_PCDR1_PID39_Pos                   _U_(7)                                               /**< (PMC_PCDR1) Peripheral Clock 39 Disable Position */
#define PMC_PCDR1_PID39_Msk                   (_U_(0x1) << PMC_PCDR1_PID39_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 39 Disable Mask */
#define PMC_PCDR1_PID39(value)                (PMC_PCDR1_PID39_Msk & ((value) << PMC_PCDR1_PID39_Pos))
#define PMC_PCDR1_PID40_Pos                   _U_(8)                                               /**< (PMC_PCDR1) Peripheral Clock 40 Disable Position */
#define PMC_PCDR1_PID40_Msk                   (_U_(0x1) << PMC_PCDR1_PID40_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 40 Disable Mask */
#define PMC_PCDR1_PID40(value)                (PMC_PCDR1_PID40_Msk & ((value) << PMC_PCDR1_PID40_Pos))
#define PMC_PCDR1_PID41_Pos                   _U_(9)                                               /**< (PMC_PCDR1) Peripheral Clock 41 Disable Position */
#define PMC_PCDR1_PID41_Msk                   (_U_(0x1) << PMC_PCDR1_PID41_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 41 Disable Mask */
#define PMC_PCDR1_PID41(value)                (PMC_PCDR1_PID41_Msk & ((value) << PMC_PCDR1_PID41_Pos))
#define PMC_PCDR1_PID42_Pos                   _U_(10)                                              /**< (PMC_PCDR1) Peripheral Clock 42 Disable Position */
#define PMC_PCDR1_PID42_Msk                   (_U_(0x1) << PMC_PCDR1_PID42_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 42 Disable Mask */
#define PMC_PCDR1_PID42(value)                (PMC_PCDR1_PID42_Msk & ((value) << PMC_PCDR1_PID42_Pos))
#define PMC_PCDR1_PID43_Pos                   _U_(11)                                              /**< (PMC_PCDR1) Peripheral Clock 43 Disable Position */
#define PMC_PCDR1_PID43_Msk                   (_U_(0x1) << PMC_PCDR1_PID43_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 43 Disable Mask */
#define PMC_PCDR1_PID43(value)                (PMC_PCDR1_PID43_Msk & ((value) << PMC_PCDR1_PID43_Pos))
#define PMC_PCDR1_PID44_Pos                   _U_(12)                                              /**< (PMC_PCDR1) Peripheral Clock 44 Disable Position */
#define PMC_PCDR1_PID44_Msk                   (_U_(0x1) << PMC_PCDR1_PID44_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 44 Disable Mask */
#define PMC_PCDR1_PID44(value)                (PMC_PCDR1_PID44_Msk & ((value) << PMC_PCDR1_PID44_Pos))
#define PMC_PCDR1_PID45_Pos                   _U_(13)                                              /**< (PMC_PCDR1) Peripheral Clock 45 Disable Position */
#define PMC_PCDR1_PID45_Msk                   (_U_(0x1) << PMC_PCDR1_PID45_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 45 Disable Mask */
#define PMC_PCDR1_PID45(value)                (PMC_PCDR1_PID45_Msk & ((value) << PMC_PCDR1_PID45_Pos))
#define PMC_PCDR1_PID46_Pos                   _U_(14)                                              /**< (PMC_PCDR1) Peripheral Clock 46 Disable Position */
#define PMC_PCDR1_PID46_Msk                   (_U_(0x1) << PMC_PCDR1_PID46_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 46 Disable Mask */
#define PMC_PCDR1_PID46(value)                (PMC_PCDR1_PID46_Msk & ((value) << PMC_PCDR1_PID46_Pos))
#define PMC_PCDR1_PID47_Pos                   _U_(15)                                              /**< (PMC_PCDR1) Peripheral Clock 47 Disable Position */
#define PMC_PCDR1_PID47_Msk                   (_U_(0x1) << PMC_PCDR1_PID47_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 47 Disable Mask */
#define PMC_PCDR1_PID47(value)                (PMC_PCDR1_PID47_Msk & ((value) << PMC_PCDR1_PID47_Pos))
#define PMC_PCDR1_PID48_Pos                   _U_(16)                                              /**< (PMC_PCDR1) Peripheral Clock 48 Disable Position */
#define PMC_PCDR1_PID48_Msk                   (_U_(0x1) << PMC_PCDR1_PID48_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 48 Disable Mask */
#define PMC_PCDR1_PID48(value)                (PMC_PCDR1_PID48_Msk & ((value) << PMC_PCDR1_PID48_Pos))
#define PMC_PCDR1_PID49_Pos                   _U_(17)                                              /**< (PMC_PCDR1) Peripheral Clock 49 Disable Position */
#define PMC_PCDR1_PID49_Msk                   (_U_(0x1) << PMC_PCDR1_PID49_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 49 Disable Mask */
#define PMC_PCDR1_PID49(value)                (PMC_PCDR1_PID49_Msk & ((value) << PMC_PCDR1_PID49_Pos))
#define PMC_PCDR1_PID50_Pos                   _U_(18)                                              /**< (PMC_PCDR1) Peripheral Clock 50 Disable Position */
#define PMC_PCDR1_PID50_Msk                   (_U_(0x1) << PMC_PCDR1_PID50_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 50 Disable Mask */
#define PMC_PCDR1_PID50(value)                (PMC_PCDR1_PID50_Msk & ((value) << PMC_PCDR1_PID50_Pos))
#define PMC_PCDR1_PID51_Pos                   _U_(19)                                              /**< (PMC_PCDR1) Peripheral Clock 51 Disable Position */
#define PMC_PCDR1_PID51_Msk                   (_U_(0x1) << PMC_PCDR1_PID51_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 51 Disable Mask */
#define PMC_PCDR1_PID51(value)                (PMC_PCDR1_PID51_Msk & ((value) << PMC_PCDR1_PID51_Pos))
#define PMC_PCDR1_PID52_Pos                   _U_(20)                                              /**< (PMC_PCDR1) Peripheral Clock 52 Disable Position */
#define PMC_PCDR1_PID52_Msk                   (_U_(0x1) << PMC_PCDR1_PID52_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 52 Disable Mask */
#define PMC_PCDR1_PID52(value)                (PMC_PCDR1_PID52_Msk & ((value) << PMC_PCDR1_PID52_Pos))
#define PMC_PCDR1_PID56_Pos                   _U_(24)                                              /**< (PMC_PCDR1) Peripheral Clock 56 Disable Position */
#define PMC_PCDR1_PID56_Msk                   (_U_(0x1) << PMC_PCDR1_PID56_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 56 Disable Mask */
#define PMC_PCDR1_PID56(value)                (PMC_PCDR1_PID56_Msk & ((value) << PMC_PCDR1_PID56_Pos))
#define PMC_PCDR1_PID57_Pos                   _U_(25)                                              /**< (PMC_PCDR1) Peripheral Clock 57 Disable Position */
#define PMC_PCDR1_PID57_Msk                   (_U_(0x1) << PMC_PCDR1_PID57_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 57 Disable Mask */
#define PMC_PCDR1_PID57(value)                (PMC_PCDR1_PID57_Msk & ((value) << PMC_PCDR1_PID57_Pos))
#define PMC_PCDR1_PID58_Pos                   _U_(26)                                              /**< (PMC_PCDR1) Peripheral Clock 58 Disable Position */
#define PMC_PCDR1_PID58_Msk                   (_U_(0x1) << PMC_PCDR1_PID58_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 58 Disable Mask */
#define PMC_PCDR1_PID58(value)                (PMC_PCDR1_PID58_Msk & ((value) << PMC_PCDR1_PID58_Pos))
#define PMC_PCDR1_PID59_Pos                   _U_(27)                                              /**< (PMC_PCDR1) Peripheral Clock 59 Disable Position */
#define PMC_PCDR1_PID59_Msk                   (_U_(0x1) << PMC_PCDR1_PID59_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 59 Disable Mask */
#define PMC_PCDR1_PID59(value)                (PMC_PCDR1_PID59_Msk & ((value) << PMC_PCDR1_PID59_Pos))
#define PMC_PCDR1_PID60_Pos                   _U_(28)                                              /**< (PMC_PCDR1) Peripheral Clock 60 Disable Position */
#define PMC_PCDR1_PID60_Msk                   (_U_(0x1) << PMC_PCDR1_PID60_Pos)                    /**< (PMC_PCDR1) Peripheral Clock 60 Disable Mask */
#define PMC_PCDR1_PID60(value)                (PMC_PCDR1_PID60_Msk & ((value) << PMC_PCDR1_PID60_Pos))
#define PMC_PCDR1_Msk                         _U_(0x1F1FFFAF)                                      /**< (PMC_PCDR1) Register Mask  */

#define PMC_PCDR1_PID_Pos                     _U_(0)                                               /**< (PMC_PCDR1 Position) Peripheral Clock 6x Disable */
#define PMC_PCDR1_PID_Msk                     (_U_(0xFFFFFF) << PMC_PCDR1_PID_Pos)                 /**< (PMC_PCDR1 Mask) PID */
#define PMC_PCDR1_PID(value)                  (PMC_PCDR1_PID_Msk & ((value) << PMC_PCDR1_PID_Pos)) 

/* -------- PMC_PCSR1 : (PMC Offset: 0x108) ( R/ 32) Peripheral Clock Status Register 1 -------- */
#define PMC_PCSR1_PID32_Pos                   _U_(0)                                               /**< (PMC_PCSR1) Peripheral Clock 32 Status Position */
#define PMC_PCSR1_PID32_Msk                   (_U_(0x1) << PMC_PCSR1_PID32_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 32 Status Mask */
#define PMC_PCSR1_PID32(value)                (PMC_PCSR1_PID32_Msk & ((value) << PMC_PCSR1_PID32_Pos))
#define PMC_PCSR1_PID33_Pos                   _U_(1)                                               /**< (PMC_PCSR1) Peripheral Clock 33 Status Position */
#define PMC_PCSR1_PID33_Msk                   (_U_(0x1) << PMC_PCSR1_PID33_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 33 Status Mask */
#define PMC_PCSR1_PID33(value)                (PMC_PCSR1_PID33_Msk & ((value) << PMC_PCSR1_PID33_Pos))
#define PMC_PCSR1_PID34_Pos                   _U_(2)                                               /**< (PMC_PCSR1) Peripheral Clock 34 Status Position */
#define PMC_PCSR1_PID34_Msk                   (_U_(0x1) << PMC_PCSR1_PID34_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 34 Status Mask */
#define PMC_PCSR1_PID34(value)                (PMC_PCSR1_PID34_Msk & ((value) << PMC_PCSR1_PID34_Pos))
#define PMC_PCSR1_PID35_Pos                   _U_(3)                                               /**< (PMC_PCSR1) Peripheral Clock 35 Status Position */
#define PMC_PCSR1_PID35_Msk                   (_U_(0x1) << PMC_PCSR1_PID35_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 35 Status Mask */
#define PMC_PCSR1_PID35(value)                (PMC_PCSR1_PID35_Msk & ((value) << PMC_PCSR1_PID35_Pos))
#define PMC_PCSR1_PID37_Pos                   _U_(5)                                               /**< (PMC_PCSR1) Peripheral Clock 37 Status Position */
#define PMC_PCSR1_PID37_Msk                   (_U_(0x1) << PMC_PCSR1_PID37_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 37 Status Mask */
#define PMC_PCSR1_PID37(value)                (PMC_PCSR1_PID37_Msk & ((value) << PMC_PCSR1_PID37_Pos))
#define PMC_PCSR1_PID39_Pos                   _U_(7)                                               /**< (PMC_PCSR1) Peripheral Clock 39 Status Position */
#define PMC_PCSR1_PID39_Msk                   (_U_(0x1) << PMC_PCSR1_PID39_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 39 Status Mask */
#define PMC_PCSR1_PID39(value)                (PMC_PCSR1_PID39_Msk & ((value) << PMC_PCSR1_PID39_Pos))
#define PMC_PCSR1_PID40_Pos                   _U_(8)                                               /**< (PMC_PCSR1) Peripheral Clock 40 Status Position */
#define PMC_PCSR1_PID40_Msk                   (_U_(0x1) << PMC_PCSR1_PID40_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 40 Status Mask */
#define PMC_PCSR1_PID40(value)                (PMC_PCSR1_PID40_Msk & ((value) << PMC_PCSR1_PID40_Pos))
#define PMC_PCSR1_PID41_Pos                   _U_(9)                                               /**< (PMC_PCSR1) Peripheral Clock 41 Status Position */
#define PMC_PCSR1_PID41_Msk                   (_U_(0x1) << PMC_PCSR1_PID41_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 41 Status Mask */
#define PMC_PCSR1_PID41(value)                (PMC_PCSR1_PID41_Msk & ((value) << PMC_PCSR1_PID41_Pos))
#define PMC_PCSR1_PID42_Pos                   _U_(10)                                              /**< (PMC_PCSR1) Peripheral Clock 42 Status Position */
#define PMC_PCSR1_PID42_Msk                   (_U_(0x1) << PMC_PCSR1_PID42_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 42 Status Mask */
#define PMC_PCSR1_PID42(value)                (PMC_PCSR1_PID42_Msk & ((value) << PMC_PCSR1_PID42_Pos))
#define PMC_PCSR1_PID43_Pos                   _U_(11)                                              /**< (PMC_PCSR1) Peripheral Clock 43 Status Position */
#define PMC_PCSR1_PID43_Msk                   (_U_(0x1) << PMC_PCSR1_PID43_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 43 Status Mask */
#define PMC_PCSR1_PID43(value)                (PMC_PCSR1_PID43_Msk & ((value) << PMC_PCSR1_PID43_Pos))
#define PMC_PCSR1_PID44_Pos                   _U_(12)                                              /**< (PMC_PCSR1) Peripheral Clock 44 Status Position */
#define PMC_PCSR1_PID44_Msk                   (_U_(0x1) << PMC_PCSR1_PID44_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 44 Status Mask */
#define PMC_PCSR1_PID44(value)                (PMC_PCSR1_PID44_Msk & ((value) << PMC_PCSR1_PID44_Pos))
#define PMC_PCSR1_PID45_Pos                   _U_(13)                                              /**< (PMC_PCSR1) Peripheral Clock 45 Status Position */
#define PMC_PCSR1_PID45_Msk                   (_U_(0x1) << PMC_PCSR1_PID45_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 45 Status Mask */
#define PMC_PCSR1_PID45(value)                (PMC_PCSR1_PID45_Msk & ((value) << PMC_PCSR1_PID45_Pos))
#define PMC_PCSR1_PID46_Pos                   _U_(14)                                              /**< (PMC_PCSR1) Peripheral Clock 46 Status Position */
#define PMC_PCSR1_PID46_Msk                   (_U_(0x1) << PMC_PCSR1_PID46_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 46 Status Mask */
#define PMC_PCSR1_PID46(value)                (PMC_PCSR1_PID46_Msk & ((value) << PMC_PCSR1_PID46_Pos))
#define PMC_PCSR1_PID47_Pos                   _U_(15)                                              /**< (PMC_PCSR1) Peripheral Clock 47 Status Position */
#define PMC_PCSR1_PID47_Msk                   (_U_(0x1) << PMC_PCSR1_PID47_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 47 Status Mask */
#define PMC_PCSR1_PID47(value)                (PMC_PCSR1_PID47_Msk & ((value) << PMC_PCSR1_PID47_Pos))
#define PMC_PCSR1_PID48_Pos                   _U_(16)                                              /**< (PMC_PCSR1) Peripheral Clock 48 Status Position */
#define PMC_PCSR1_PID48_Msk                   (_U_(0x1) << PMC_PCSR1_PID48_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 48 Status Mask */
#define PMC_PCSR1_PID48(value)                (PMC_PCSR1_PID48_Msk & ((value) << PMC_PCSR1_PID48_Pos))
#define PMC_PCSR1_PID49_Pos                   _U_(17)                                              /**< (PMC_PCSR1) Peripheral Clock 49 Status Position */
#define PMC_PCSR1_PID49_Msk                   (_U_(0x1) << PMC_PCSR1_PID49_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 49 Status Mask */
#define PMC_PCSR1_PID49(value)                (PMC_PCSR1_PID49_Msk & ((value) << PMC_PCSR1_PID49_Pos))
#define PMC_PCSR1_PID50_Pos                   _U_(18)                                              /**< (PMC_PCSR1) Peripheral Clock 50 Status Position */
#define PMC_PCSR1_PID50_Msk                   (_U_(0x1) << PMC_PCSR1_PID50_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 50 Status Mask */
#define PMC_PCSR1_PID50(value)                (PMC_PCSR1_PID50_Msk & ((value) << PMC_PCSR1_PID50_Pos))
#define PMC_PCSR1_PID51_Pos                   _U_(19)                                              /**< (PMC_PCSR1) Peripheral Clock 51 Status Position */
#define PMC_PCSR1_PID51_Msk                   (_U_(0x1) << PMC_PCSR1_PID51_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 51 Status Mask */
#define PMC_PCSR1_PID51(value)                (PMC_PCSR1_PID51_Msk & ((value) << PMC_PCSR1_PID51_Pos))
#define PMC_PCSR1_PID52_Pos                   _U_(20)                                              /**< (PMC_PCSR1) Peripheral Clock 52 Status Position */
#define PMC_PCSR1_PID52_Msk                   (_U_(0x1) << PMC_PCSR1_PID52_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 52 Status Mask */
#define PMC_PCSR1_PID52(value)                (PMC_PCSR1_PID52_Msk & ((value) << PMC_PCSR1_PID52_Pos))
#define PMC_PCSR1_PID56_Pos                   _U_(24)                                              /**< (PMC_PCSR1) Peripheral Clock 56 Status Position */
#define PMC_PCSR1_PID56_Msk                   (_U_(0x1) << PMC_PCSR1_PID56_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 56 Status Mask */
#define PMC_PCSR1_PID56(value)                (PMC_PCSR1_PID56_Msk & ((value) << PMC_PCSR1_PID56_Pos))
#define PMC_PCSR1_PID57_Pos                   _U_(25)                                              /**< (PMC_PCSR1) Peripheral Clock 57 Status Position */
#define PMC_PCSR1_PID57_Msk                   (_U_(0x1) << PMC_PCSR1_PID57_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 57 Status Mask */
#define PMC_PCSR1_PID57(value)                (PMC_PCSR1_PID57_Msk & ((value) << PMC_PCSR1_PID57_Pos))
#define PMC_PCSR1_PID58_Pos                   _U_(26)                                              /**< (PMC_PCSR1) Peripheral Clock 58 Status Position */
#define PMC_PCSR1_PID58_Msk                   (_U_(0x1) << PMC_PCSR1_PID58_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 58 Status Mask */
#define PMC_PCSR1_PID58(value)                (PMC_PCSR1_PID58_Msk & ((value) << PMC_PCSR1_PID58_Pos))
#define PMC_PCSR1_PID59_Pos                   _U_(27)                                              /**< (PMC_PCSR1) Peripheral Clock 59 Status Position */
#define PMC_PCSR1_PID59_Msk                   (_U_(0x1) << PMC_PCSR1_PID59_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 59 Status Mask */
#define PMC_PCSR1_PID59(value)                (PMC_PCSR1_PID59_Msk & ((value) << PMC_PCSR1_PID59_Pos))
#define PMC_PCSR1_PID60_Pos                   _U_(28)                                              /**< (PMC_PCSR1) Peripheral Clock 60 Status Position */
#define PMC_PCSR1_PID60_Msk                   (_U_(0x1) << PMC_PCSR1_PID60_Pos)                    /**< (PMC_PCSR1) Peripheral Clock 60 Status Mask */
#define PMC_PCSR1_PID60(value)                (PMC_PCSR1_PID60_Msk & ((value) << PMC_PCSR1_PID60_Pos))
#define PMC_PCSR1_Msk                         _U_(0x1F1FFFAF)                                      /**< (PMC_PCSR1) Register Mask  */

#define PMC_PCSR1_PID_Pos                     _U_(0)                                               /**< (PMC_PCSR1 Position) Peripheral Clock 6x Status */
#define PMC_PCSR1_PID_Msk                     (_U_(0xFFFFFF) << PMC_PCSR1_PID_Pos)                 /**< (PMC_PCSR1 Mask) PID */
#define PMC_PCSR1_PID(value)                  (PMC_PCSR1_PID_Msk & ((value) << PMC_PCSR1_PID_Pos)) 

/* -------- PMC_PCR : (PMC Offset: 0x10C) (R/W 32) Peripheral Control Register -------- */
#define PMC_PCR_PID_Pos                       _U_(0)                                               /**< (PMC_PCR) Peripheral ID Position */
#define PMC_PCR_PID_Msk                       (_U_(0x7F) << PMC_PCR_PID_Pos)                       /**< (PMC_PCR) Peripheral ID Mask */
#define PMC_PCR_PID(value)                    (PMC_PCR_PID_Msk & ((value) << PMC_PCR_PID_Pos))    
#define PMC_PCR_GCLKCSS_Pos                   _U_(8)                                               /**< (PMC_PCR) Generic Clock Source Selection Position */
#define PMC_PCR_GCLKCSS_Msk                   (_U_(0x7) << PMC_PCR_GCLKCSS_Pos)                    /**< (PMC_PCR) Generic Clock Source Selection Mask */
#define PMC_PCR_GCLKCSS(value)                (PMC_PCR_GCLKCSS_Msk & ((value) << PMC_PCR_GCLKCSS_Pos))
#define   PMC_PCR_GCLKCSS_SLOW_CLK_Val        _U_(0x0)                                             /**< (PMC_PCR) Slow clock is selected  */
#define   PMC_PCR_GCLKCSS_MAIN_CLK_Val        _U_(0x1)                                             /**< (PMC_PCR) Main clock is selected  */
#define   PMC_PCR_GCLKCSS_PLLA_CLK_Val        _U_(0x2)                                             /**< (PMC_PCR) PLLACK is selected  */
#define   PMC_PCR_GCLKCSS_UPLL_CLK_Val        _U_(0x3)                                             /**< (PMC_PCR) UPLL Clock is selected  */
#define   PMC_PCR_GCLKCSS_MCK_CLK_Val         _U_(0x4)                                             /**< (PMC_PCR) Master Clock is selected  */
#define PMC_PCR_GCLKCSS_SLOW_CLK              (PMC_PCR_GCLKCSS_SLOW_CLK_Val << PMC_PCR_GCLKCSS_Pos) /**< (PMC_PCR) Slow clock is selected Position  */
#define PMC_PCR_GCLKCSS_MAIN_CLK              (PMC_PCR_GCLKCSS_MAIN_CLK_Val << PMC_PCR_GCLKCSS_Pos) /**< (PMC_PCR) Main clock is selected Position  */
#define PMC_PCR_GCLKCSS_PLLA_CLK              (PMC_PCR_GCLKCSS_PLLA_CLK_Val << PMC_PCR_GCLKCSS_Pos) /**< (PMC_PCR) PLLACK is selected Position  */
#define PMC_PCR_GCLKCSS_UPLL_CLK              (PMC_PCR_GCLKCSS_UPLL_CLK_Val << PMC_PCR_GCLKCSS_Pos) /**< (PMC_PCR) UPLL Clock is selected Position  */
#define PMC_PCR_GCLKCSS_MCK_CLK               (PMC_PCR_GCLKCSS_MCK_CLK_Val << PMC_PCR_GCLKCSS_Pos) /**< (PMC_PCR) Master Clock is selected Position  */
#define PMC_PCR_CMD_Pos                       _U_(12)                                              /**< (PMC_PCR) Command Position */
#define PMC_PCR_CMD_Msk                       (_U_(0x1) << PMC_PCR_CMD_Pos)                        /**< (PMC_PCR) Command Mask */
#define PMC_PCR_CMD(value)                    (PMC_PCR_CMD_Msk & ((value) << PMC_PCR_CMD_Pos))    
#define PMC_PCR_GCLKDIV_Pos                   _U_(20)                                              /**< (PMC_PCR) Generic Clock Division Ratio Position */
#define PMC_PCR_GCLKDIV_Msk                   (_U_(0xFF) << PMC_PCR_GCLKDIV_Pos)                   /**< (PMC_PCR) Generic Clock Division Ratio Mask */
#define PMC_PCR_GCLKDIV(value)                (PMC_PCR_GCLKDIV_Msk & ((value) << PMC_PCR_GCLKDIV_Pos))
#define PMC_PCR_EN_Pos                        _U_(28)                                              /**< (PMC_PCR) Enable Position */
#define PMC_PCR_EN_Msk                        (_U_(0x1) << PMC_PCR_EN_Pos)                         /**< (PMC_PCR) Enable Mask */
#define PMC_PCR_EN(value)                     (PMC_PCR_EN_Msk & ((value) << PMC_PCR_EN_Pos))      
#define PMC_PCR_GCLKEN_Pos                    _U_(29)                                              /**< (PMC_PCR) Generic Clock Enable Position */
#define PMC_PCR_GCLKEN_Msk                    (_U_(0x1) << PMC_PCR_GCLKEN_Pos)                     /**< (PMC_PCR) Generic Clock Enable Mask */
#define PMC_PCR_GCLKEN(value)                 (PMC_PCR_GCLKEN_Msk & ((value) << PMC_PCR_GCLKEN_Pos))
#define PMC_PCR_Msk                           _U_(0x3FF0177F)                                      /**< (PMC_PCR) Register Mask  */


/* -------- PMC_OCR : (PMC Offset: 0x110) (R/W 32) Oscillator Calibration Register -------- */
#define PMC_OCR_CAL4_Pos                      _U_(0)                                               /**< (PMC_OCR) Main RC Oscillator Calibration Bits for 4 MHz Position */
#define PMC_OCR_CAL4_Msk                      (_U_(0x7F) << PMC_OCR_CAL4_Pos)                      /**< (PMC_OCR) Main RC Oscillator Calibration Bits for 4 MHz Mask */
#define PMC_OCR_CAL4(value)                   (PMC_OCR_CAL4_Msk & ((value) << PMC_OCR_CAL4_Pos))  
#define PMC_OCR_SEL4_Pos                      _U_(7)                                               /**< (PMC_OCR) Selection of Main RC Oscillator Calibration Bits for 4 MHz Position */
#define PMC_OCR_SEL4_Msk                      (_U_(0x1) << PMC_OCR_SEL4_Pos)                       /**< (PMC_OCR) Selection of Main RC Oscillator Calibration Bits for 4 MHz Mask */
#define PMC_OCR_SEL4(value)                   (PMC_OCR_SEL4_Msk & ((value) << PMC_OCR_SEL4_Pos))  
#define PMC_OCR_CAL8_Pos                      _U_(8)                                               /**< (PMC_OCR) Main RC Oscillator Calibration Bits for 8 MHz Position */
#define PMC_OCR_CAL8_Msk                      (_U_(0x7F) << PMC_OCR_CAL8_Pos)                      /**< (PMC_OCR) Main RC Oscillator Calibration Bits for 8 MHz Mask */
#define PMC_OCR_CAL8(value)                   (PMC_OCR_CAL8_Msk & ((value) << PMC_OCR_CAL8_Pos))  
#define PMC_OCR_SEL8_Pos                      _U_(15)                                              /**< (PMC_OCR) Selection of Main RC Oscillator Calibration Bits for 8 MHz Position */
#define PMC_OCR_SEL8_Msk                      (_U_(0x1) << PMC_OCR_SEL8_Pos)                       /**< (PMC_OCR) Selection of Main RC Oscillator Calibration Bits for 8 MHz Mask */
#define PMC_OCR_SEL8(value)                   (PMC_OCR_SEL8_Msk & ((value) << PMC_OCR_SEL8_Pos))  
#define PMC_OCR_CAL12_Pos                     _U_(16)                                              /**< (PMC_OCR) Main RC Oscillator Calibration Bits for 12 MHz Position */
#define PMC_OCR_CAL12_Msk                     (_U_(0x7F) << PMC_OCR_CAL12_Pos)                     /**< (PMC_OCR) Main RC Oscillator Calibration Bits for 12 MHz Mask */
#define PMC_OCR_CAL12(value)                  (PMC_OCR_CAL12_Msk & ((value) << PMC_OCR_CAL12_Pos))
#define PMC_OCR_SEL12_Pos                     _U_(23)                                              /**< (PMC_OCR) Selection of Main RC Oscillator Calibration Bits for 12 MHz Position */
#define PMC_OCR_SEL12_Msk                     (_U_(0x1) << PMC_OCR_SEL12_Pos)                      /**< (PMC_OCR) Selection of Main RC Oscillator Calibration Bits for 12 MHz Mask */
#define PMC_OCR_SEL12(value)                  (PMC_OCR_SEL12_Msk & ((value) << PMC_OCR_SEL12_Pos))
#define PMC_OCR_Msk                           _U_(0x00FFFFFF)                                      /**< (PMC_OCR) Register Mask  */


/* -------- PMC_SLPWK_ER0 : (PMC Offset: 0x114) ( /W 32) SleepWalking Enable Register 0 -------- */
#define PMC_SLPWK_ER0_PID7_Pos                _U_(7)                                               /**< (PMC_SLPWK_ER0) Peripheral 7 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID7_Msk                (_U_(0x1) << PMC_SLPWK_ER0_PID7_Pos)                 /**< (PMC_SLPWK_ER0) Peripheral 7 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID7(value)             (PMC_SLPWK_ER0_PID7_Msk & ((value) << PMC_SLPWK_ER0_PID7_Pos))
#define PMC_SLPWK_ER0_PID8_Pos                _U_(8)                                               /**< (PMC_SLPWK_ER0) Peripheral 8 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID8_Msk                (_U_(0x1) << PMC_SLPWK_ER0_PID8_Pos)                 /**< (PMC_SLPWK_ER0) Peripheral 8 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID8(value)             (PMC_SLPWK_ER0_PID8_Msk & ((value) << PMC_SLPWK_ER0_PID8_Pos))
#define PMC_SLPWK_ER0_PID9_Pos                _U_(9)                                               /**< (PMC_SLPWK_ER0) Peripheral 9 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID9_Msk                (_U_(0x1) << PMC_SLPWK_ER0_PID9_Pos)                 /**< (PMC_SLPWK_ER0) Peripheral 9 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID9(value)             (PMC_SLPWK_ER0_PID9_Msk & ((value) << PMC_SLPWK_ER0_PID9_Pos))
#define PMC_SLPWK_ER0_PID10_Pos               _U_(10)                                              /**< (PMC_SLPWK_ER0) Peripheral 10 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID10_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID10_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 10 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID10(value)            (PMC_SLPWK_ER0_PID10_Msk & ((value) << PMC_SLPWK_ER0_PID10_Pos))
#define PMC_SLPWK_ER0_PID11_Pos               _U_(11)                                              /**< (PMC_SLPWK_ER0) Peripheral 11 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID11_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID11_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 11 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID11(value)            (PMC_SLPWK_ER0_PID11_Msk & ((value) << PMC_SLPWK_ER0_PID11_Pos))
#define PMC_SLPWK_ER0_PID12_Pos               _U_(12)                                              /**< (PMC_SLPWK_ER0) Peripheral 12 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID12_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID12_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 12 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID12(value)            (PMC_SLPWK_ER0_PID12_Msk & ((value) << PMC_SLPWK_ER0_PID12_Pos))
#define PMC_SLPWK_ER0_PID13_Pos               _U_(13)                                              /**< (PMC_SLPWK_ER0) Peripheral 13 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID13_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID13_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 13 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID13(value)            (PMC_SLPWK_ER0_PID13_Msk & ((value) << PMC_SLPWK_ER0_PID13_Pos))
#define PMC_SLPWK_ER0_PID14_Pos               _U_(14)                                              /**< (PMC_SLPWK_ER0) Peripheral 14 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID14_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID14_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 14 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID14(value)            (PMC_SLPWK_ER0_PID14_Msk & ((value) << PMC_SLPWK_ER0_PID14_Pos))
#define PMC_SLPWK_ER0_PID15_Pos               _U_(15)                                              /**< (PMC_SLPWK_ER0) Peripheral 15 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID15_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID15_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 15 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID15(value)            (PMC_SLPWK_ER0_PID15_Msk & ((value) << PMC_SLPWK_ER0_PID15_Pos))
#define PMC_SLPWK_ER0_PID16_Pos               _U_(16)                                              /**< (PMC_SLPWK_ER0) Peripheral 16 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID16_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID16_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 16 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID16(value)            (PMC_SLPWK_ER0_PID16_Msk & ((value) << PMC_SLPWK_ER0_PID16_Pos))
#define PMC_SLPWK_ER0_PID17_Pos               _U_(17)                                              /**< (PMC_SLPWK_ER0) Peripheral 17 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID17_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID17_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 17 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID17(value)            (PMC_SLPWK_ER0_PID17_Msk & ((value) << PMC_SLPWK_ER0_PID17_Pos))
#define PMC_SLPWK_ER0_PID18_Pos               _U_(18)                                              /**< (PMC_SLPWK_ER0) Peripheral 18 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID18_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID18_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 18 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID18(value)            (PMC_SLPWK_ER0_PID18_Msk & ((value) << PMC_SLPWK_ER0_PID18_Pos))
#define PMC_SLPWK_ER0_PID19_Pos               _U_(19)                                              /**< (PMC_SLPWK_ER0) Peripheral 19 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID19_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID19_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 19 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID19(value)            (PMC_SLPWK_ER0_PID19_Msk & ((value) << PMC_SLPWK_ER0_PID19_Pos))
#define PMC_SLPWK_ER0_PID20_Pos               _U_(20)                                              /**< (PMC_SLPWK_ER0) Peripheral 20 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID20_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID20_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 20 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID20(value)            (PMC_SLPWK_ER0_PID20_Msk & ((value) << PMC_SLPWK_ER0_PID20_Pos))
#define PMC_SLPWK_ER0_PID21_Pos               _U_(21)                                              /**< (PMC_SLPWK_ER0) Peripheral 21 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID21_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID21_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 21 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID21(value)            (PMC_SLPWK_ER0_PID21_Msk & ((value) << PMC_SLPWK_ER0_PID21_Pos))
#define PMC_SLPWK_ER0_PID22_Pos               _U_(22)                                              /**< (PMC_SLPWK_ER0) Peripheral 22 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID22_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID22_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 22 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID22(value)            (PMC_SLPWK_ER0_PID22_Msk & ((value) << PMC_SLPWK_ER0_PID22_Pos))
#define PMC_SLPWK_ER0_PID23_Pos               _U_(23)                                              /**< (PMC_SLPWK_ER0) Peripheral 23 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID23_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID23_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 23 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID23(value)            (PMC_SLPWK_ER0_PID23_Msk & ((value) << PMC_SLPWK_ER0_PID23_Pos))
#define PMC_SLPWK_ER0_PID24_Pos               _U_(24)                                              /**< (PMC_SLPWK_ER0) Peripheral 24 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID24_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID24_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 24 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID24(value)            (PMC_SLPWK_ER0_PID24_Msk & ((value) << PMC_SLPWK_ER0_PID24_Pos))
#define PMC_SLPWK_ER0_PID25_Pos               _U_(25)                                              /**< (PMC_SLPWK_ER0) Peripheral 25 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID25_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID25_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 25 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID25(value)            (PMC_SLPWK_ER0_PID25_Msk & ((value) << PMC_SLPWK_ER0_PID25_Pos))
#define PMC_SLPWK_ER0_PID26_Pos               _U_(26)                                              /**< (PMC_SLPWK_ER0) Peripheral 26 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID26_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID26_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 26 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID26(value)            (PMC_SLPWK_ER0_PID26_Msk & ((value) << PMC_SLPWK_ER0_PID26_Pos))
#define PMC_SLPWK_ER0_PID27_Pos               _U_(27)                                              /**< (PMC_SLPWK_ER0) Peripheral 27 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID27_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID27_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 27 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID27(value)            (PMC_SLPWK_ER0_PID27_Msk & ((value) << PMC_SLPWK_ER0_PID27_Pos))
#define PMC_SLPWK_ER0_PID28_Pos               _U_(28)                                              /**< (PMC_SLPWK_ER0) Peripheral 28 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID28_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID28_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 28 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID28(value)            (PMC_SLPWK_ER0_PID28_Msk & ((value) << PMC_SLPWK_ER0_PID28_Pos))
#define PMC_SLPWK_ER0_PID29_Pos               _U_(29)                                              /**< (PMC_SLPWK_ER0) Peripheral 29 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID29_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID29_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 29 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID29(value)            (PMC_SLPWK_ER0_PID29_Msk & ((value) << PMC_SLPWK_ER0_PID29_Pos))
#define PMC_SLPWK_ER0_PID30_Pos               _U_(30)                                              /**< (PMC_SLPWK_ER0) Peripheral 30 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID30_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID30_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 30 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID30(value)            (PMC_SLPWK_ER0_PID30_Msk & ((value) << PMC_SLPWK_ER0_PID30_Pos))
#define PMC_SLPWK_ER0_PID31_Pos               _U_(31)                                              /**< (PMC_SLPWK_ER0) Peripheral 31 SleepWalking Enable Position */
#define PMC_SLPWK_ER0_PID31_Msk               (_U_(0x1) << PMC_SLPWK_ER0_PID31_Pos)                /**< (PMC_SLPWK_ER0) Peripheral 31 SleepWalking Enable Mask */
#define PMC_SLPWK_ER0_PID31(value)            (PMC_SLPWK_ER0_PID31_Msk & ((value) << PMC_SLPWK_ER0_PID31_Pos))
#define PMC_SLPWK_ER0_Msk                     _U_(0xFFFFFF80)                                      /**< (PMC_SLPWK_ER0) Register Mask  */

#define PMC_SLPWK_ER0_PID_Pos                 _U_(7)                                               /**< (PMC_SLPWK_ER0 Position) Peripheral 3x SleepWalking Enable */
#define PMC_SLPWK_ER0_PID_Msk                 (_U_(0x1FFFFFF) << PMC_SLPWK_ER0_PID_Pos)            /**< (PMC_SLPWK_ER0 Mask) PID */
#define PMC_SLPWK_ER0_PID(value)              (PMC_SLPWK_ER0_PID_Msk & ((value) << PMC_SLPWK_ER0_PID_Pos)) 

/* -------- PMC_SLPWK_DR0 : (PMC Offset: 0x118) ( /W 32) SleepWalking Disable Register 0 -------- */
#define PMC_SLPWK_DR0_PID7_Pos                _U_(7)                                               /**< (PMC_SLPWK_DR0) Peripheral 7 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID7_Msk                (_U_(0x1) << PMC_SLPWK_DR0_PID7_Pos)                 /**< (PMC_SLPWK_DR0) Peripheral 7 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID7(value)             (PMC_SLPWK_DR0_PID7_Msk & ((value) << PMC_SLPWK_DR0_PID7_Pos))
#define PMC_SLPWK_DR0_PID8_Pos                _U_(8)                                               /**< (PMC_SLPWK_DR0) Peripheral 8 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID8_Msk                (_U_(0x1) << PMC_SLPWK_DR0_PID8_Pos)                 /**< (PMC_SLPWK_DR0) Peripheral 8 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID8(value)             (PMC_SLPWK_DR0_PID8_Msk & ((value) << PMC_SLPWK_DR0_PID8_Pos))
#define PMC_SLPWK_DR0_PID9_Pos                _U_(9)                                               /**< (PMC_SLPWK_DR0) Peripheral 9 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID9_Msk                (_U_(0x1) << PMC_SLPWK_DR0_PID9_Pos)                 /**< (PMC_SLPWK_DR0) Peripheral 9 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID9(value)             (PMC_SLPWK_DR0_PID9_Msk & ((value) << PMC_SLPWK_DR0_PID9_Pos))
#define PMC_SLPWK_DR0_PID10_Pos               _U_(10)                                              /**< (PMC_SLPWK_DR0) Peripheral 10 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID10_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID10_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 10 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID10(value)            (PMC_SLPWK_DR0_PID10_Msk & ((value) << PMC_SLPWK_DR0_PID10_Pos))
#define PMC_SLPWK_DR0_PID11_Pos               _U_(11)                                              /**< (PMC_SLPWK_DR0) Peripheral 11 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID11_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID11_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 11 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID11(value)            (PMC_SLPWK_DR0_PID11_Msk & ((value) << PMC_SLPWK_DR0_PID11_Pos))
#define PMC_SLPWK_DR0_PID12_Pos               _U_(12)                                              /**< (PMC_SLPWK_DR0) Peripheral 12 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID12_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID12_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 12 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID12(value)            (PMC_SLPWK_DR0_PID12_Msk & ((value) << PMC_SLPWK_DR0_PID12_Pos))
#define PMC_SLPWK_DR0_PID13_Pos               _U_(13)                                              /**< (PMC_SLPWK_DR0) Peripheral 13 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID13_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID13_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 13 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID13(value)            (PMC_SLPWK_DR0_PID13_Msk & ((value) << PMC_SLPWK_DR0_PID13_Pos))
#define PMC_SLPWK_DR0_PID14_Pos               _U_(14)                                              /**< (PMC_SLPWK_DR0) Peripheral 14 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID14_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID14_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 14 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID14(value)            (PMC_SLPWK_DR0_PID14_Msk & ((value) << PMC_SLPWK_DR0_PID14_Pos))
#define PMC_SLPWK_DR0_PID15_Pos               _U_(15)                                              /**< (PMC_SLPWK_DR0) Peripheral 15 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID15_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID15_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 15 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID15(value)            (PMC_SLPWK_DR0_PID15_Msk & ((value) << PMC_SLPWK_DR0_PID15_Pos))
#define PMC_SLPWK_DR0_PID16_Pos               _U_(16)                                              /**< (PMC_SLPWK_DR0) Peripheral 16 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID16_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID16_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 16 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID16(value)            (PMC_SLPWK_DR0_PID16_Msk & ((value) << PMC_SLPWK_DR0_PID16_Pos))
#define PMC_SLPWK_DR0_PID17_Pos               _U_(17)                                              /**< (PMC_SLPWK_DR0) Peripheral 17 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID17_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID17_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 17 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID17(value)            (PMC_SLPWK_DR0_PID17_Msk & ((value) << PMC_SLPWK_DR0_PID17_Pos))
#define PMC_SLPWK_DR0_PID18_Pos               _U_(18)                                              /**< (PMC_SLPWK_DR0) Peripheral 18 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID18_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID18_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 18 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID18(value)            (PMC_SLPWK_DR0_PID18_Msk & ((value) << PMC_SLPWK_DR0_PID18_Pos))
#define PMC_SLPWK_DR0_PID19_Pos               _U_(19)                                              /**< (PMC_SLPWK_DR0) Peripheral 19 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID19_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID19_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 19 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID19(value)            (PMC_SLPWK_DR0_PID19_Msk & ((value) << PMC_SLPWK_DR0_PID19_Pos))
#define PMC_SLPWK_DR0_PID20_Pos               _U_(20)                                              /**< (PMC_SLPWK_DR0) Peripheral 20 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID20_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID20_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 20 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID20(value)            (PMC_SLPWK_DR0_PID20_Msk & ((value) << PMC_SLPWK_DR0_PID20_Pos))
#define PMC_SLPWK_DR0_PID21_Pos               _U_(21)                                              /**< (PMC_SLPWK_DR0) Peripheral 21 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID21_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID21_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 21 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID21(value)            (PMC_SLPWK_DR0_PID21_Msk & ((value) << PMC_SLPWK_DR0_PID21_Pos))
#define PMC_SLPWK_DR0_PID22_Pos               _U_(22)                                              /**< (PMC_SLPWK_DR0) Peripheral 22 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID22_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID22_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 22 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID22(value)            (PMC_SLPWK_DR0_PID22_Msk & ((value) << PMC_SLPWK_DR0_PID22_Pos))
#define PMC_SLPWK_DR0_PID23_Pos               _U_(23)                                              /**< (PMC_SLPWK_DR0) Peripheral 23 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID23_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID23_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 23 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID23(value)            (PMC_SLPWK_DR0_PID23_Msk & ((value) << PMC_SLPWK_DR0_PID23_Pos))
#define PMC_SLPWK_DR0_PID24_Pos               _U_(24)                                              /**< (PMC_SLPWK_DR0) Peripheral 24 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID24_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID24_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 24 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID24(value)            (PMC_SLPWK_DR0_PID24_Msk & ((value) << PMC_SLPWK_DR0_PID24_Pos))
#define PMC_SLPWK_DR0_PID25_Pos               _U_(25)                                              /**< (PMC_SLPWK_DR0) Peripheral 25 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID25_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID25_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 25 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID25(value)            (PMC_SLPWK_DR0_PID25_Msk & ((value) << PMC_SLPWK_DR0_PID25_Pos))
#define PMC_SLPWK_DR0_PID26_Pos               _U_(26)                                              /**< (PMC_SLPWK_DR0) Peripheral 26 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID26_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID26_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 26 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID26(value)            (PMC_SLPWK_DR0_PID26_Msk & ((value) << PMC_SLPWK_DR0_PID26_Pos))
#define PMC_SLPWK_DR0_PID27_Pos               _U_(27)                                              /**< (PMC_SLPWK_DR0) Peripheral 27 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID27_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID27_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 27 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID27(value)            (PMC_SLPWK_DR0_PID27_Msk & ((value) << PMC_SLPWK_DR0_PID27_Pos))
#define PMC_SLPWK_DR0_PID28_Pos               _U_(28)                                              /**< (PMC_SLPWK_DR0) Peripheral 28 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID28_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID28_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 28 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID28(value)            (PMC_SLPWK_DR0_PID28_Msk & ((value) << PMC_SLPWK_DR0_PID28_Pos))
#define PMC_SLPWK_DR0_PID29_Pos               _U_(29)                                              /**< (PMC_SLPWK_DR0) Peripheral 29 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID29_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID29_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 29 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID29(value)            (PMC_SLPWK_DR0_PID29_Msk & ((value) << PMC_SLPWK_DR0_PID29_Pos))
#define PMC_SLPWK_DR0_PID30_Pos               _U_(30)                                              /**< (PMC_SLPWK_DR0) Peripheral 30 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID30_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID30_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 30 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID30(value)            (PMC_SLPWK_DR0_PID30_Msk & ((value) << PMC_SLPWK_DR0_PID30_Pos))
#define PMC_SLPWK_DR0_PID31_Pos               _U_(31)                                              /**< (PMC_SLPWK_DR0) Peripheral 31 SleepWalking Disable Position */
#define PMC_SLPWK_DR0_PID31_Msk               (_U_(0x1) << PMC_SLPWK_DR0_PID31_Pos)                /**< (PMC_SLPWK_DR0) Peripheral 31 SleepWalking Disable Mask */
#define PMC_SLPWK_DR0_PID31(value)            (PMC_SLPWK_DR0_PID31_Msk & ((value) << PMC_SLPWK_DR0_PID31_Pos))
#define PMC_SLPWK_DR0_Msk                     _U_(0xFFFFFF80)                                      /**< (PMC_SLPWK_DR0) Register Mask  */

#define PMC_SLPWK_DR0_PID_Pos                 _U_(7)                                               /**< (PMC_SLPWK_DR0 Position) Peripheral 3x SleepWalking Disable */
#define PMC_SLPWK_DR0_PID_Msk                 (_U_(0x1FFFFFF) << PMC_SLPWK_DR0_PID_Pos)            /**< (PMC_SLPWK_DR0 Mask) PID */
#define PMC_SLPWK_DR0_PID(value)              (PMC_SLPWK_DR0_PID_Msk & ((value) << PMC_SLPWK_DR0_PID_Pos)) 

/* -------- PMC_SLPWK_SR0 : (PMC Offset: 0x11C) ( R/ 32) SleepWalking Status Register 0 -------- */
#define PMC_SLPWK_SR0_PID7_Pos                _U_(7)                                               /**< (PMC_SLPWK_SR0) Peripheral 7 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID7_Msk                (_U_(0x1) << PMC_SLPWK_SR0_PID7_Pos)                 /**< (PMC_SLPWK_SR0) Peripheral 7 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID7(value)             (PMC_SLPWK_SR0_PID7_Msk & ((value) << PMC_SLPWK_SR0_PID7_Pos))
#define PMC_SLPWK_SR0_PID8_Pos                _U_(8)                                               /**< (PMC_SLPWK_SR0) Peripheral 8 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID8_Msk                (_U_(0x1) << PMC_SLPWK_SR0_PID8_Pos)                 /**< (PMC_SLPWK_SR0) Peripheral 8 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID8(value)             (PMC_SLPWK_SR0_PID8_Msk & ((value) << PMC_SLPWK_SR0_PID8_Pos))
#define PMC_SLPWK_SR0_PID9_Pos                _U_(9)                                               /**< (PMC_SLPWK_SR0) Peripheral 9 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID9_Msk                (_U_(0x1) << PMC_SLPWK_SR0_PID9_Pos)                 /**< (PMC_SLPWK_SR0) Peripheral 9 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID9(value)             (PMC_SLPWK_SR0_PID9_Msk & ((value) << PMC_SLPWK_SR0_PID9_Pos))
#define PMC_SLPWK_SR0_PID10_Pos               _U_(10)                                              /**< (PMC_SLPWK_SR0) Peripheral 10 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID10_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID10_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 10 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID10(value)            (PMC_SLPWK_SR0_PID10_Msk & ((value) << PMC_SLPWK_SR0_PID10_Pos))
#define PMC_SLPWK_SR0_PID11_Pos               _U_(11)                                              /**< (PMC_SLPWK_SR0) Peripheral 11 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID11_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID11_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 11 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID11(value)            (PMC_SLPWK_SR0_PID11_Msk & ((value) << PMC_SLPWK_SR0_PID11_Pos))
#define PMC_SLPWK_SR0_PID12_Pos               _U_(12)                                              /**< (PMC_SLPWK_SR0) Peripheral 12 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID12_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID12_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 12 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID12(value)            (PMC_SLPWK_SR0_PID12_Msk & ((value) << PMC_SLPWK_SR0_PID12_Pos))
#define PMC_SLPWK_SR0_PID13_Pos               _U_(13)                                              /**< (PMC_SLPWK_SR0) Peripheral 13 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID13_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID13_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 13 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID13(value)            (PMC_SLPWK_SR0_PID13_Msk & ((value) << PMC_SLPWK_SR0_PID13_Pos))
#define PMC_SLPWK_SR0_PID14_Pos               _U_(14)                                              /**< (PMC_SLPWK_SR0) Peripheral 14 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID14_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID14_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 14 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID14(value)            (PMC_SLPWK_SR0_PID14_Msk & ((value) << PMC_SLPWK_SR0_PID14_Pos))
#define PMC_SLPWK_SR0_PID15_Pos               _U_(15)                                              /**< (PMC_SLPWK_SR0) Peripheral 15 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID15_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID15_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 15 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID15(value)            (PMC_SLPWK_SR0_PID15_Msk & ((value) << PMC_SLPWK_SR0_PID15_Pos))
#define PMC_SLPWK_SR0_PID16_Pos               _U_(16)                                              /**< (PMC_SLPWK_SR0) Peripheral 16 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID16_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID16_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 16 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID16(value)            (PMC_SLPWK_SR0_PID16_Msk & ((value) << PMC_SLPWK_SR0_PID16_Pos))
#define PMC_SLPWK_SR0_PID17_Pos               _U_(17)                                              /**< (PMC_SLPWK_SR0) Peripheral 17 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID17_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID17_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 17 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID17(value)            (PMC_SLPWK_SR0_PID17_Msk & ((value) << PMC_SLPWK_SR0_PID17_Pos))
#define PMC_SLPWK_SR0_PID18_Pos               _U_(18)                                              /**< (PMC_SLPWK_SR0) Peripheral 18 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID18_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID18_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 18 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID18(value)            (PMC_SLPWK_SR0_PID18_Msk & ((value) << PMC_SLPWK_SR0_PID18_Pos))
#define PMC_SLPWK_SR0_PID19_Pos               _U_(19)                                              /**< (PMC_SLPWK_SR0) Peripheral 19 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID19_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID19_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 19 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID19(value)            (PMC_SLPWK_SR0_PID19_Msk & ((value) << PMC_SLPWK_SR0_PID19_Pos))
#define PMC_SLPWK_SR0_PID20_Pos               _U_(20)                                              /**< (PMC_SLPWK_SR0) Peripheral 20 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID20_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID20_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 20 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID20(value)            (PMC_SLPWK_SR0_PID20_Msk & ((value) << PMC_SLPWK_SR0_PID20_Pos))
#define PMC_SLPWK_SR0_PID21_Pos               _U_(21)                                              /**< (PMC_SLPWK_SR0) Peripheral 21 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID21_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID21_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 21 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID21(value)            (PMC_SLPWK_SR0_PID21_Msk & ((value) << PMC_SLPWK_SR0_PID21_Pos))
#define PMC_SLPWK_SR0_PID22_Pos               _U_(22)                                              /**< (PMC_SLPWK_SR0) Peripheral 22 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID22_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID22_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 22 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID22(value)            (PMC_SLPWK_SR0_PID22_Msk & ((value) << PMC_SLPWK_SR0_PID22_Pos))
#define PMC_SLPWK_SR0_PID23_Pos               _U_(23)                                              /**< (PMC_SLPWK_SR0) Peripheral 23 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID23_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID23_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 23 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID23(value)            (PMC_SLPWK_SR0_PID23_Msk & ((value) << PMC_SLPWK_SR0_PID23_Pos))
#define PMC_SLPWK_SR0_PID24_Pos               _U_(24)                                              /**< (PMC_SLPWK_SR0) Peripheral 24 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID24_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID24_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 24 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID24(value)            (PMC_SLPWK_SR0_PID24_Msk & ((value) << PMC_SLPWK_SR0_PID24_Pos))
#define PMC_SLPWK_SR0_PID25_Pos               _U_(25)                                              /**< (PMC_SLPWK_SR0) Peripheral 25 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID25_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID25_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 25 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID25(value)            (PMC_SLPWK_SR0_PID25_Msk & ((value) << PMC_SLPWK_SR0_PID25_Pos))
#define PMC_SLPWK_SR0_PID26_Pos               _U_(26)                                              /**< (PMC_SLPWK_SR0) Peripheral 26 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID26_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID26_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 26 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID26(value)            (PMC_SLPWK_SR0_PID26_Msk & ((value) << PMC_SLPWK_SR0_PID26_Pos))
#define PMC_SLPWK_SR0_PID27_Pos               _U_(27)                                              /**< (PMC_SLPWK_SR0) Peripheral 27 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID27_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID27_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 27 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID27(value)            (PMC_SLPWK_SR0_PID27_Msk & ((value) << PMC_SLPWK_SR0_PID27_Pos))
#define PMC_SLPWK_SR0_PID28_Pos               _U_(28)                                              /**< (PMC_SLPWK_SR0) Peripheral 28 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID28_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID28_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 28 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID28(value)            (PMC_SLPWK_SR0_PID28_Msk & ((value) << PMC_SLPWK_SR0_PID28_Pos))
#define PMC_SLPWK_SR0_PID29_Pos               _U_(29)                                              /**< (PMC_SLPWK_SR0) Peripheral 29 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID29_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID29_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 29 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID29(value)            (PMC_SLPWK_SR0_PID29_Msk & ((value) << PMC_SLPWK_SR0_PID29_Pos))
#define PMC_SLPWK_SR0_PID30_Pos               _U_(30)                                              /**< (PMC_SLPWK_SR0) Peripheral 30 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID30_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID30_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 30 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID30(value)            (PMC_SLPWK_SR0_PID30_Msk & ((value) << PMC_SLPWK_SR0_PID30_Pos))
#define PMC_SLPWK_SR0_PID31_Pos               _U_(31)                                              /**< (PMC_SLPWK_SR0) Peripheral 31 SleepWalking Status Position */
#define PMC_SLPWK_SR0_PID31_Msk               (_U_(0x1) << PMC_SLPWK_SR0_PID31_Pos)                /**< (PMC_SLPWK_SR0) Peripheral 31 SleepWalking Status Mask */
#define PMC_SLPWK_SR0_PID31(value)            (PMC_SLPWK_SR0_PID31_Msk & ((value) << PMC_SLPWK_SR0_PID31_Pos))
#define PMC_SLPWK_SR0_Msk                     _U_(0xFFFFFF80)                                      /**< (PMC_SLPWK_SR0) Register Mask  */

#define PMC_SLPWK_SR0_PID_Pos                 _U_(7)                                               /**< (PMC_SLPWK_SR0 Position) Peripheral 3x SleepWalking Status */
#define PMC_SLPWK_SR0_PID_Msk                 (_U_(0x1FFFFFF) << PMC_SLPWK_SR0_PID_Pos)            /**< (PMC_SLPWK_SR0 Mask) PID */
#define PMC_SLPWK_SR0_PID(value)              (PMC_SLPWK_SR0_PID_Msk & ((value) << PMC_SLPWK_SR0_PID_Pos)) 

/* -------- PMC_SLPWK_ASR0 : (PMC Offset: 0x120) ( R/ 32) SleepWalking Activity Status Register 0 -------- */
#define PMC_SLPWK_ASR0_PID7_Pos               _U_(7)                                               /**< (PMC_SLPWK_ASR0) Peripheral 7 Activity Status Position */
#define PMC_SLPWK_ASR0_PID7_Msk               (_U_(0x1) << PMC_SLPWK_ASR0_PID7_Pos)                /**< (PMC_SLPWK_ASR0) Peripheral 7 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID7(value)            (PMC_SLPWK_ASR0_PID7_Msk & ((value) << PMC_SLPWK_ASR0_PID7_Pos))
#define PMC_SLPWK_ASR0_PID8_Pos               _U_(8)                                               /**< (PMC_SLPWK_ASR0) Peripheral 8 Activity Status Position */
#define PMC_SLPWK_ASR0_PID8_Msk               (_U_(0x1) << PMC_SLPWK_ASR0_PID8_Pos)                /**< (PMC_SLPWK_ASR0) Peripheral 8 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID8(value)            (PMC_SLPWK_ASR0_PID8_Msk & ((value) << PMC_SLPWK_ASR0_PID8_Pos))
#define PMC_SLPWK_ASR0_PID9_Pos               _U_(9)                                               /**< (PMC_SLPWK_ASR0) Peripheral 9 Activity Status Position */
#define PMC_SLPWK_ASR0_PID9_Msk               (_U_(0x1) << PMC_SLPWK_ASR0_PID9_Pos)                /**< (PMC_SLPWK_ASR0) Peripheral 9 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID9(value)            (PMC_SLPWK_ASR0_PID9_Msk & ((value) << PMC_SLPWK_ASR0_PID9_Pos))
#define PMC_SLPWK_ASR0_PID10_Pos              _U_(10)                                              /**< (PMC_SLPWK_ASR0) Peripheral 10 Activity Status Position */
#define PMC_SLPWK_ASR0_PID10_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID10_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 10 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID10(value)           (PMC_SLPWK_ASR0_PID10_Msk & ((value) << PMC_SLPWK_ASR0_PID10_Pos))
#define PMC_SLPWK_ASR0_PID11_Pos              _U_(11)                                              /**< (PMC_SLPWK_ASR0) Peripheral 11 Activity Status Position */
#define PMC_SLPWK_ASR0_PID11_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID11_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 11 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID11(value)           (PMC_SLPWK_ASR0_PID11_Msk & ((value) << PMC_SLPWK_ASR0_PID11_Pos))
#define PMC_SLPWK_ASR0_PID12_Pos              _U_(12)                                              /**< (PMC_SLPWK_ASR0) Peripheral 12 Activity Status Position */
#define PMC_SLPWK_ASR0_PID12_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID12_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 12 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID12(value)           (PMC_SLPWK_ASR0_PID12_Msk & ((value) << PMC_SLPWK_ASR0_PID12_Pos))
#define PMC_SLPWK_ASR0_PID13_Pos              _U_(13)                                              /**< (PMC_SLPWK_ASR0) Peripheral 13 Activity Status Position */
#define PMC_SLPWK_ASR0_PID13_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID13_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 13 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID13(value)           (PMC_SLPWK_ASR0_PID13_Msk & ((value) << PMC_SLPWK_ASR0_PID13_Pos))
#define PMC_SLPWK_ASR0_PID14_Pos              _U_(14)                                              /**< (PMC_SLPWK_ASR0) Peripheral 14 Activity Status Position */
#define PMC_SLPWK_ASR0_PID14_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID14_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 14 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID14(value)           (PMC_SLPWK_ASR0_PID14_Msk & ((value) << PMC_SLPWK_ASR0_PID14_Pos))
#define PMC_SLPWK_ASR0_PID15_Pos              _U_(15)                                              /**< (PMC_SLPWK_ASR0) Peripheral 15 Activity Status Position */
#define PMC_SLPWK_ASR0_PID15_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID15_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 15 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID15(value)           (PMC_SLPWK_ASR0_PID15_Msk & ((value) << PMC_SLPWK_ASR0_PID15_Pos))
#define PMC_SLPWK_ASR0_PID16_Pos              _U_(16)                                              /**< (PMC_SLPWK_ASR0) Peripheral 16 Activity Status Position */
#define PMC_SLPWK_ASR0_PID16_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID16_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 16 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID16(value)           (PMC_SLPWK_ASR0_PID16_Msk & ((value) << PMC_SLPWK_ASR0_PID16_Pos))
#define PMC_SLPWK_ASR0_PID17_Pos              _U_(17)                                              /**< (PMC_SLPWK_ASR0) Peripheral 17 Activity Status Position */
#define PMC_SLPWK_ASR0_PID17_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID17_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 17 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID17(value)           (PMC_SLPWK_ASR0_PID17_Msk & ((value) << PMC_SLPWK_ASR0_PID17_Pos))
#define PMC_SLPWK_ASR0_PID18_Pos              _U_(18)                                              /**< (PMC_SLPWK_ASR0) Peripheral 18 Activity Status Position */
#define PMC_SLPWK_ASR0_PID18_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID18_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 18 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID18(value)           (PMC_SLPWK_ASR0_PID18_Msk & ((value) << PMC_SLPWK_ASR0_PID18_Pos))
#define PMC_SLPWK_ASR0_PID19_Pos              _U_(19)                                              /**< (PMC_SLPWK_ASR0) Peripheral 19 Activity Status Position */
#define PMC_SLPWK_ASR0_PID19_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID19_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 19 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID19(value)           (PMC_SLPWK_ASR0_PID19_Msk & ((value) << PMC_SLPWK_ASR0_PID19_Pos))
#define PMC_SLPWK_ASR0_PID20_Pos              _U_(20)                                              /**< (PMC_SLPWK_ASR0) Peripheral 20 Activity Status Position */
#define PMC_SLPWK_ASR0_PID20_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID20_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 20 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID20(value)           (PMC_SLPWK_ASR0_PID20_Msk & ((value) << PMC_SLPWK_ASR0_PID20_Pos))
#define PMC_SLPWK_ASR0_PID21_Pos              _U_(21)                                              /**< (PMC_SLPWK_ASR0) Peripheral 21 Activity Status Position */
#define PMC_SLPWK_ASR0_PID21_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID21_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 21 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID21(value)           (PMC_SLPWK_ASR0_PID21_Msk & ((value) << PMC_SLPWK_ASR0_PID21_Pos))
#define PMC_SLPWK_ASR0_PID22_Pos              _U_(22)                                              /**< (PMC_SLPWK_ASR0) Peripheral 22 Activity Status Position */
#define PMC_SLPWK_ASR0_PID22_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID22_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 22 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID22(value)           (PMC_SLPWK_ASR0_PID22_Msk & ((value) << PMC_SLPWK_ASR0_PID22_Pos))
#define PMC_SLPWK_ASR0_PID23_Pos              _U_(23)                                              /**< (PMC_SLPWK_ASR0) Peripheral 23 Activity Status Position */
#define PMC_SLPWK_ASR0_PID23_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID23_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 23 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID23(value)           (PMC_SLPWK_ASR0_PID23_Msk & ((value) << PMC_SLPWK_ASR0_PID23_Pos))
#define PMC_SLPWK_ASR0_PID24_Pos              _U_(24)                                              /**< (PMC_SLPWK_ASR0) Peripheral 24 Activity Status Position */
#define PMC_SLPWK_ASR0_PID24_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID24_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 24 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID24(value)           (PMC_SLPWK_ASR0_PID24_Msk & ((value) << PMC_SLPWK_ASR0_PID24_Pos))
#define PMC_SLPWK_ASR0_PID25_Pos              _U_(25)                                              /**< (PMC_SLPWK_ASR0) Peripheral 25 Activity Status Position */
#define PMC_SLPWK_ASR0_PID25_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID25_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 25 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID25(value)           (PMC_SLPWK_ASR0_PID25_Msk & ((value) << PMC_SLPWK_ASR0_PID25_Pos))
#define PMC_SLPWK_ASR0_PID26_Pos              _U_(26)                                              /**< (PMC_SLPWK_ASR0) Peripheral 26 Activity Status Position */
#define PMC_SLPWK_ASR0_PID26_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID26_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 26 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID26(value)           (PMC_SLPWK_ASR0_PID26_Msk & ((value) << PMC_SLPWK_ASR0_PID26_Pos))
#define PMC_SLPWK_ASR0_PID27_Pos              _U_(27)                                              /**< (PMC_SLPWK_ASR0) Peripheral 27 Activity Status Position */
#define PMC_SLPWK_ASR0_PID27_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID27_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 27 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID27(value)           (PMC_SLPWK_ASR0_PID27_Msk & ((value) << PMC_SLPWK_ASR0_PID27_Pos))
#define PMC_SLPWK_ASR0_PID28_Pos              _U_(28)                                              /**< (PMC_SLPWK_ASR0) Peripheral 28 Activity Status Position */
#define PMC_SLPWK_ASR0_PID28_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID28_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 28 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID28(value)           (PMC_SLPWK_ASR0_PID28_Msk & ((value) << PMC_SLPWK_ASR0_PID28_Pos))
#define PMC_SLPWK_ASR0_PID29_Pos              _U_(29)                                              /**< (PMC_SLPWK_ASR0) Peripheral 29 Activity Status Position */
#define PMC_SLPWK_ASR0_PID29_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID29_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 29 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID29(value)           (PMC_SLPWK_ASR0_PID29_Msk & ((value) << PMC_SLPWK_ASR0_PID29_Pos))
#define PMC_SLPWK_ASR0_PID30_Pos              _U_(30)                                              /**< (PMC_SLPWK_ASR0) Peripheral 30 Activity Status Position */
#define PMC_SLPWK_ASR0_PID30_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID30_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 30 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID30(value)           (PMC_SLPWK_ASR0_PID30_Msk & ((value) << PMC_SLPWK_ASR0_PID30_Pos))
#define PMC_SLPWK_ASR0_PID31_Pos              _U_(31)                                              /**< (PMC_SLPWK_ASR0) Peripheral 31 Activity Status Position */
#define PMC_SLPWK_ASR0_PID31_Msk              (_U_(0x1) << PMC_SLPWK_ASR0_PID31_Pos)               /**< (PMC_SLPWK_ASR0) Peripheral 31 Activity Status Mask */
#define PMC_SLPWK_ASR0_PID31(value)           (PMC_SLPWK_ASR0_PID31_Msk & ((value) << PMC_SLPWK_ASR0_PID31_Pos))
#define PMC_SLPWK_ASR0_Msk                    _U_(0xFFFFFF80)                                      /**< (PMC_SLPWK_ASR0) Register Mask  */

#define PMC_SLPWK_ASR0_PID_Pos                _U_(7)                                               /**< (PMC_SLPWK_ASR0 Position) Peripheral 3x Activity Status */
#define PMC_SLPWK_ASR0_PID_Msk                (_U_(0x1FFFFFF) << PMC_SLPWK_ASR0_PID_Pos)           /**< (PMC_SLPWK_ASR0 Mask) PID */
#define PMC_SLPWK_ASR0_PID(value)             (PMC_SLPWK_ASR0_PID_Msk & ((value) << PMC_SLPWK_ASR0_PID_Pos)) 

/* -------- PMC_PMMR : (PMC Offset: 0x130) (R/W 32) PLL Maximum Multiplier Value Register -------- */
#define PMC_PMMR_PLLA_MMAX_Pos                _U_(0)                                               /**< (PMC_PMMR) PLLA Maximum Allowed Multiplier Value Position */
#define PMC_PMMR_PLLA_MMAX_Msk                (_U_(0x7FF) << PMC_PMMR_PLLA_MMAX_Pos)               /**< (PMC_PMMR) PLLA Maximum Allowed Multiplier Value Mask */
#define PMC_PMMR_PLLA_MMAX(value)             (PMC_PMMR_PLLA_MMAX_Msk & ((value) << PMC_PMMR_PLLA_MMAX_Pos))
#define PMC_PMMR_Msk                          _U_(0x000007FF)                                      /**< (PMC_PMMR) Register Mask  */


/* -------- PMC_SLPWK_ER1 : (PMC Offset: 0x134) ( /W 32) SleepWalking Enable Register 1 -------- */
#define PMC_SLPWK_ER1_PID32_Pos               _U_(0)                                               /**< (PMC_SLPWK_ER1) Peripheral 32 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID32_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID32_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 32 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID32(value)            (PMC_SLPWK_ER1_PID32_Msk & ((value) << PMC_SLPWK_ER1_PID32_Pos))
#define PMC_SLPWK_ER1_PID33_Pos               _U_(1)                                               /**< (PMC_SLPWK_ER1) Peripheral 33 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID33_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID33_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 33 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID33(value)            (PMC_SLPWK_ER1_PID33_Msk & ((value) << PMC_SLPWK_ER1_PID33_Pos))
#define PMC_SLPWK_ER1_PID34_Pos               _U_(2)                                               /**< (PMC_SLPWK_ER1) Peripheral 34 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID34_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID34_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 34 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID34(value)            (PMC_SLPWK_ER1_PID34_Msk & ((value) << PMC_SLPWK_ER1_PID34_Pos))
#define PMC_SLPWK_ER1_PID35_Pos               _U_(3)                                               /**< (PMC_SLPWK_ER1) Peripheral 35 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID35_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID35_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 35 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID35(value)            (PMC_SLPWK_ER1_PID35_Msk & ((value) << PMC_SLPWK_ER1_PID35_Pos))
#define PMC_SLPWK_ER1_PID37_Pos               _U_(5)                                               /**< (PMC_SLPWK_ER1) Peripheral 37 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID37_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID37_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 37 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID37(value)            (PMC_SLPWK_ER1_PID37_Msk & ((value) << PMC_SLPWK_ER1_PID37_Pos))
#define PMC_SLPWK_ER1_PID39_Pos               _U_(7)                                               /**< (PMC_SLPWK_ER1) Peripheral 39 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID39_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID39_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 39 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID39(value)            (PMC_SLPWK_ER1_PID39_Msk & ((value) << PMC_SLPWK_ER1_PID39_Pos))
#define PMC_SLPWK_ER1_PID40_Pos               _U_(8)                                               /**< (PMC_SLPWK_ER1) Peripheral 40 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID40_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID40_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 40 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID40(value)            (PMC_SLPWK_ER1_PID40_Msk & ((value) << PMC_SLPWK_ER1_PID40_Pos))
#define PMC_SLPWK_ER1_PID41_Pos               _U_(9)                                               /**< (PMC_SLPWK_ER1) Peripheral 41 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID41_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID41_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 41 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID41(value)            (PMC_SLPWK_ER1_PID41_Msk & ((value) << PMC_SLPWK_ER1_PID41_Pos))
#define PMC_SLPWK_ER1_PID42_Pos               _U_(10)                                              /**< (PMC_SLPWK_ER1) Peripheral 42 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID42_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID42_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 42 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID42(value)            (PMC_SLPWK_ER1_PID42_Msk & ((value) << PMC_SLPWK_ER1_PID42_Pos))
#define PMC_SLPWK_ER1_PID43_Pos               _U_(11)                                              /**< (PMC_SLPWK_ER1) Peripheral 43 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID43_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID43_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 43 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID43(value)            (PMC_SLPWK_ER1_PID43_Msk & ((value) << PMC_SLPWK_ER1_PID43_Pos))
#define PMC_SLPWK_ER1_PID44_Pos               _U_(12)                                              /**< (PMC_SLPWK_ER1) Peripheral 44 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID44_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID44_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 44 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID44(value)            (PMC_SLPWK_ER1_PID44_Msk & ((value) << PMC_SLPWK_ER1_PID44_Pos))
#define PMC_SLPWK_ER1_PID45_Pos               _U_(13)                                              /**< (PMC_SLPWK_ER1) Peripheral 45 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID45_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID45_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 45 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID45(value)            (PMC_SLPWK_ER1_PID45_Msk & ((value) << PMC_SLPWK_ER1_PID45_Pos))
#define PMC_SLPWK_ER1_PID46_Pos               _U_(14)                                              /**< (PMC_SLPWK_ER1) Peripheral 46 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID46_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID46_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 46 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID46(value)            (PMC_SLPWK_ER1_PID46_Msk & ((value) << PMC_SLPWK_ER1_PID46_Pos))
#define PMC_SLPWK_ER1_PID47_Pos               _U_(15)                                              /**< (PMC_SLPWK_ER1) Peripheral 47 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID47_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID47_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 47 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID47(value)            (PMC_SLPWK_ER1_PID47_Msk & ((value) << PMC_SLPWK_ER1_PID47_Pos))
#define PMC_SLPWK_ER1_PID48_Pos               _U_(16)                                              /**< (PMC_SLPWK_ER1) Peripheral 48 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID48_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID48_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 48 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID48(value)            (PMC_SLPWK_ER1_PID48_Msk & ((value) << PMC_SLPWK_ER1_PID48_Pos))
#define PMC_SLPWK_ER1_PID49_Pos               _U_(17)                                              /**< (PMC_SLPWK_ER1) Peripheral 49 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID49_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID49_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 49 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID49(value)            (PMC_SLPWK_ER1_PID49_Msk & ((value) << PMC_SLPWK_ER1_PID49_Pos))
#define PMC_SLPWK_ER1_PID50_Pos               _U_(18)                                              /**< (PMC_SLPWK_ER1) Peripheral 50 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID50_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID50_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 50 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID50(value)            (PMC_SLPWK_ER1_PID50_Msk & ((value) << PMC_SLPWK_ER1_PID50_Pos))
#define PMC_SLPWK_ER1_PID51_Pos               _U_(19)                                              /**< (PMC_SLPWK_ER1) Peripheral 51 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID51_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID51_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 51 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID51(value)            (PMC_SLPWK_ER1_PID51_Msk & ((value) << PMC_SLPWK_ER1_PID51_Pos))
#define PMC_SLPWK_ER1_PID52_Pos               _U_(20)                                              /**< (PMC_SLPWK_ER1) Peripheral 52 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID52_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID52_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 52 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID52(value)            (PMC_SLPWK_ER1_PID52_Msk & ((value) << PMC_SLPWK_ER1_PID52_Pos))
#define PMC_SLPWK_ER1_PID56_Pos               _U_(24)                                              /**< (PMC_SLPWK_ER1) Peripheral 56 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID56_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID56_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 56 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID56(value)            (PMC_SLPWK_ER1_PID56_Msk & ((value) << PMC_SLPWK_ER1_PID56_Pos))
#define PMC_SLPWK_ER1_PID57_Pos               _U_(25)                                              /**< (PMC_SLPWK_ER1) Peripheral 57 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID57_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID57_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 57 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID57(value)            (PMC_SLPWK_ER1_PID57_Msk & ((value) << PMC_SLPWK_ER1_PID57_Pos))
#define PMC_SLPWK_ER1_PID58_Pos               _U_(26)                                              /**< (PMC_SLPWK_ER1) Peripheral 58 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID58_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID58_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 58 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID58(value)            (PMC_SLPWK_ER1_PID58_Msk & ((value) << PMC_SLPWK_ER1_PID58_Pos))
#define PMC_SLPWK_ER1_PID59_Pos               _U_(27)                                              /**< (PMC_SLPWK_ER1) Peripheral 59 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID59_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID59_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 59 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID59(value)            (PMC_SLPWK_ER1_PID59_Msk & ((value) << PMC_SLPWK_ER1_PID59_Pos))
#define PMC_SLPWK_ER1_PID60_Pos               _U_(28)                                              /**< (PMC_SLPWK_ER1) Peripheral 60 SleepWalking Enable Position */
#define PMC_SLPWK_ER1_PID60_Msk               (_U_(0x1) << PMC_SLPWK_ER1_PID60_Pos)                /**< (PMC_SLPWK_ER1) Peripheral 60 SleepWalking Enable Mask */
#define PMC_SLPWK_ER1_PID60(value)            (PMC_SLPWK_ER1_PID60_Msk & ((value) << PMC_SLPWK_ER1_PID60_Pos))
#define PMC_SLPWK_ER1_Msk                     _U_(0x1F1FFFAF)                                      /**< (PMC_SLPWK_ER1) Register Mask  */

#define PMC_SLPWK_ER1_PID_Pos                 _U_(0)                                               /**< (PMC_SLPWK_ER1 Position) Peripheral 6x SleepWalking Enable */
#define PMC_SLPWK_ER1_PID_Msk                 (_U_(0xFFFFFF) << PMC_SLPWK_ER1_PID_Pos)             /**< (PMC_SLPWK_ER1 Mask) PID */
#define PMC_SLPWK_ER1_PID(value)              (PMC_SLPWK_ER1_PID_Msk & ((value) << PMC_SLPWK_ER1_PID_Pos)) 

/* -------- PMC_SLPWK_DR1 : (PMC Offset: 0x138) ( /W 32) SleepWalking Disable Register 1 -------- */
#define PMC_SLPWK_DR1_PID32_Pos               _U_(0)                                               /**< (PMC_SLPWK_DR1) Peripheral 32 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID32_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID32_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 32 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID32(value)            (PMC_SLPWK_DR1_PID32_Msk & ((value) << PMC_SLPWK_DR1_PID32_Pos))
#define PMC_SLPWK_DR1_PID33_Pos               _U_(1)                                               /**< (PMC_SLPWK_DR1) Peripheral 33 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID33_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID33_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 33 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID33(value)            (PMC_SLPWK_DR1_PID33_Msk & ((value) << PMC_SLPWK_DR1_PID33_Pos))
#define PMC_SLPWK_DR1_PID34_Pos               _U_(2)                                               /**< (PMC_SLPWK_DR1) Peripheral 34 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID34_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID34_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 34 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID34(value)            (PMC_SLPWK_DR1_PID34_Msk & ((value) << PMC_SLPWK_DR1_PID34_Pos))
#define PMC_SLPWK_DR1_PID35_Pos               _U_(3)                                               /**< (PMC_SLPWK_DR1) Peripheral 35 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID35_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID35_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 35 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID35(value)            (PMC_SLPWK_DR1_PID35_Msk & ((value) << PMC_SLPWK_DR1_PID35_Pos))
#define PMC_SLPWK_DR1_PID37_Pos               _U_(5)                                               /**< (PMC_SLPWK_DR1) Peripheral 37 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID37_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID37_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 37 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID37(value)            (PMC_SLPWK_DR1_PID37_Msk & ((value) << PMC_SLPWK_DR1_PID37_Pos))
#define PMC_SLPWK_DR1_PID39_Pos               _U_(7)                                               /**< (PMC_SLPWK_DR1) Peripheral 39 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID39_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID39_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 39 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID39(value)            (PMC_SLPWK_DR1_PID39_Msk & ((value) << PMC_SLPWK_DR1_PID39_Pos))
#define PMC_SLPWK_DR1_PID40_Pos               _U_(8)                                               /**< (PMC_SLPWK_DR1) Peripheral 40 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID40_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID40_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 40 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID40(value)            (PMC_SLPWK_DR1_PID40_Msk & ((value) << PMC_SLPWK_DR1_PID40_Pos))
#define PMC_SLPWK_DR1_PID41_Pos               _U_(9)                                               /**< (PMC_SLPWK_DR1) Peripheral 41 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID41_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID41_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 41 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID41(value)            (PMC_SLPWK_DR1_PID41_Msk & ((value) << PMC_SLPWK_DR1_PID41_Pos))
#define PMC_SLPWK_DR1_PID42_Pos               _U_(10)                                              /**< (PMC_SLPWK_DR1) Peripheral 42 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID42_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID42_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 42 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID42(value)            (PMC_SLPWK_DR1_PID42_Msk & ((value) << PMC_SLPWK_DR1_PID42_Pos))
#define PMC_SLPWK_DR1_PID43_Pos               _U_(11)                                              /**< (PMC_SLPWK_DR1) Peripheral 43 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID43_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID43_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 43 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID43(value)            (PMC_SLPWK_DR1_PID43_Msk & ((value) << PMC_SLPWK_DR1_PID43_Pos))
#define PMC_SLPWK_DR1_PID44_Pos               _U_(12)                                              /**< (PMC_SLPWK_DR1) Peripheral 44 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID44_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID44_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 44 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID44(value)            (PMC_SLPWK_DR1_PID44_Msk & ((value) << PMC_SLPWK_DR1_PID44_Pos))
#define PMC_SLPWK_DR1_PID45_Pos               _U_(13)                                              /**< (PMC_SLPWK_DR1) Peripheral 45 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID45_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID45_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 45 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID45(value)            (PMC_SLPWK_DR1_PID45_Msk & ((value) << PMC_SLPWK_DR1_PID45_Pos))
#define PMC_SLPWK_DR1_PID46_Pos               _U_(14)                                              /**< (PMC_SLPWK_DR1) Peripheral 46 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID46_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID46_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 46 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID46(value)            (PMC_SLPWK_DR1_PID46_Msk & ((value) << PMC_SLPWK_DR1_PID46_Pos))
#define PMC_SLPWK_DR1_PID47_Pos               _U_(15)                                              /**< (PMC_SLPWK_DR1) Peripheral 47 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID47_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID47_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 47 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID47(value)            (PMC_SLPWK_DR1_PID47_Msk & ((value) << PMC_SLPWK_DR1_PID47_Pos))
#define PMC_SLPWK_DR1_PID48_Pos               _U_(16)                                              /**< (PMC_SLPWK_DR1) Peripheral 48 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID48_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID48_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 48 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID48(value)            (PMC_SLPWK_DR1_PID48_Msk & ((value) << PMC_SLPWK_DR1_PID48_Pos))
#define PMC_SLPWK_DR1_PID49_Pos               _U_(17)                                              /**< (PMC_SLPWK_DR1) Peripheral 49 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID49_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID49_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 49 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID49(value)            (PMC_SLPWK_DR1_PID49_Msk & ((value) << PMC_SLPWK_DR1_PID49_Pos))
#define PMC_SLPWK_DR1_PID50_Pos               _U_(18)                                              /**< (PMC_SLPWK_DR1) Peripheral 50 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID50_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID50_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 50 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID50(value)            (PMC_SLPWK_DR1_PID50_Msk & ((value) << PMC_SLPWK_DR1_PID50_Pos))
#define PMC_SLPWK_DR1_PID51_Pos               _U_(19)                                              /**< (PMC_SLPWK_DR1) Peripheral 51 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID51_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID51_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 51 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID51(value)            (PMC_SLPWK_DR1_PID51_Msk & ((value) << PMC_SLPWK_DR1_PID51_Pos))
#define PMC_SLPWK_DR1_PID52_Pos               _U_(20)                                              /**< (PMC_SLPWK_DR1) Peripheral 52 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID52_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID52_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 52 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID52(value)            (PMC_SLPWK_DR1_PID52_Msk & ((value) << PMC_SLPWK_DR1_PID52_Pos))
#define PMC_SLPWK_DR1_PID56_Pos               _U_(24)                                              /**< (PMC_SLPWK_DR1) Peripheral 56 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID56_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID56_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 56 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID56(value)            (PMC_SLPWK_DR1_PID56_Msk & ((value) << PMC_SLPWK_DR1_PID56_Pos))
#define PMC_SLPWK_DR1_PID57_Pos               _U_(25)                                              /**< (PMC_SLPWK_DR1) Peripheral 57 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID57_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID57_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 57 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID57(value)            (PMC_SLPWK_DR1_PID57_Msk & ((value) << PMC_SLPWK_DR1_PID57_Pos))
#define PMC_SLPWK_DR1_PID58_Pos               _U_(26)                                              /**< (PMC_SLPWK_DR1) Peripheral 58 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID58_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID58_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 58 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID58(value)            (PMC_SLPWK_DR1_PID58_Msk & ((value) << PMC_SLPWK_DR1_PID58_Pos))
#define PMC_SLPWK_DR1_PID59_Pos               _U_(27)                                              /**< (PMC_SLPWK_DR1) Peripheral 59 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID59_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID59_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 59 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID59(value)            (PMC_SLPWK_DR1_PID59_Msk & ((value) << PMC_SLPWK_DR1_PID59_Pos))
#define PMC_SLPWK_DR1_PID60_Pos               _U_(28)                                              /**< (PMC_SLPWK_DR1) Peripheral 60 SleepWalking Disable Position */
#define PMC_SLPWK_DR1_PID60_Msk               (_U_(0x1) << PMC_SLPWK_DR1_PID60_Pos)                /**< (PMC_SLPWK_DR1) Peripheral 60 SleepWalking Disable Mask */
#define PMC_SLPWK_DR1_PID60(value)            (PMC_SLPWK_DR1_PID60_Msk & ((value) << PMC_SLPWK_DR1_PID60_Pos))
#define PMC_SLPWK_DR1_Msk                     _U_(0x1F1FFFAF)                                      /**< (PMC_SLPWK_DR1) Register Mask  */

#define PMC_SLPWK_DR1_PID_Pos                 _U_(0)                                               /**< (PMC_SLPWK_DR1 Position) Peripheral 6x SleepWalking Disable */
#define PMC_SLPWK_DR1_PID_Msk                 (_U_(0xFFFFFF) << PMC_SLPWK_DR1_PID_Pos)             /**< (PMC_SLPWK_DR1 Mask) PID */
#define PMC_SLPWK_DR1_PID(value)              (PMC_SLPWK_DR1_PID_Msk & ((value) << PMC_SLPWK_DR1_PID_Pos)) 

/* -------- PMC_SLPWK_SR1 : (PMC Offset: 0x13C) ( R/ 32) SleepWalking Status Register 1 -------- */
#define PMC_SLPWK_SR1_PID32_Pos               _U_(0)                                               /**< (PMC_SLPWK_SR1) Peripheral 32 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID32_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID32_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 32 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID32(value)            (PMC_SLPWK_SR1_PID32_Msk & ((value) << PMC_SLPWK_SR1_PID32_Pos))
#define PMC_SLPWK_SR1_PID33_Pos               _U_(1)                                               /**< (PMC_SLPWK_SR1) Peripheral 33 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID33_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID33_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 33 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID33(value)            (PMC_SLPWK_SR1_PID33_Msk & ((value) << PMC_SLPWK_SR1_PID33_Pos))
#define PMC_SLPWK_SR1_PID34_Pos               _U_(2)                                               /**< (PMC_SLPWK_SR1) Peripheral 34 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID34_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID34_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 34 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID34(value)            (PMC_SLPWK_SR1_PID34_Msk & ((value) << PMC_SLPWK_SR1_PID34_Pos))
#define PMC_SLPWK_SR1_PID35_Pos               _U_(3)                                               /**< (PMC_SLPWK_SR1) Peripheral 35 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID35_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID35_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 35 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID35(value)            (PMC_SLPWK_SR1_PID35_Msk & ((value) << PMC_SLPWK_SR1_PID35_Pos))
#define PMC_SLPWK_SR1_PID37_Pos               _U_(5)                                               /**< (PMC_SLPWK_SR1) Peripheral 37 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID37_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID37_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 37 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID37(value)            (PMC_SLPWK_SR1_PID37_Msk & ((value) << PMC_SLPWK_SR1_PID37_Pos))
#define PMC_SLPWK_SR1_PID39_Pos               _U_(7)                                               /**< (PMC_SLPWK_SR1) Peripheral 39 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID39_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID39_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 39 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID39(value)            (PMC_SLPWK_SR1_PID39_Msk & ((value) << PMC_SLPWK_SR1_PID39_Pos))
#define PMC_SLPWK_SR1_PID40_Pos               _U_(8)                                               /**< (PMC_SLPWK_SR1) Peripheral 40 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID40_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID40_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 40 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID40(value)            (PMC_SLPWK_SR1_PID40_Msk & ((value) << PMC_SLPWK_SR1_PID40_Pos))
#define PMC_SLPWK_SR1_PID41_Pos               _U_(9)                                               /**< (PMC_SLPWK_SR1) Peripheral 41 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID41_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID41_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 41 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID41(value)            (PMC_SLPWK_SR1_PID41_Msk & ((value) << PMC_SLPWK_SR1_PID41_Pos))
#define PMC_SLPWK_SR1_PID42_Pos               _U_(10)                                              /**< (PMC_SLPWK_SR1) Peripheral 42 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID42_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID42_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 42 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID42(value)            (PMC_SLPWK_SR1_PID42_Msk & ((value) << PMC_SLPWK_SR1_PID42_Pos))
#define PMC_SLPWK_SR1_PID43_Pos               _U_(11)                                              /**< (PMC_SLPWK_SR1) Peripheral 43 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID43_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID43_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 43 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID43(value)            (PMC_SLPWK_SR1_PID43_Msk & ((value) << PMC_SLPWK_SR1_PID43_Pos))
#define PMC_SLPWK_SR1_PID44_Pos               _U_(12)                                              /**< (PMC_SLPWK_SR1) Peripheral 44 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID44_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID44_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 44 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID44(value)            (PMC_SLPWK_SR1_PID44_Msk & ((value) << PMC_SLPWK_SR1_PID44_Pos))
#define PMC_SLPWK_SR1_PID45_Pos               _U_(13)                                              /**< (PMC_SLPWK_SR1) Peripheral 45 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID45_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID45_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 45 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID45(value)            (PMC_SLPWK_SR1_PID45_Msk & ((value) << PMC_SLPWK_SR1_PID45_Pos))
#define PMC_SLPWK_SR1_PID46_Pos               _U_(14)                                              /**< (PMC_SLPWK_SR1) Peripheral 46 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID46_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID46_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 46 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID46(value)            (PMC_SLPWK_SR1_PID46_Msk & ((value) << PMC_SLPWK_SR1_PID46_Pos))
#define PMC_SLPWK_SR1_PID47_Pos               _U_(15)                                              /**< (PMC_SLPWK_SR1) Peripheral 47 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID47_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID47_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 47 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID47(value)            (PMC_SLPWK_SR1_PID47_Msk & ((value) << PMC_SLPWK_SR1_PID47_Pos))
#define PMC_SLPWK_SR1_PID48_Pos               _U_(16)                                              /**< (PMC_SLPWK_SR1) Peripheral 48 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID48_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID48_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 48 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID48(value)            (PMC_SLPWK_SR1_PID48_Msk & ((value) << PMC_SLPWK_SR1_PID48_Pos))
#define PMC_SLPWK_SR1_PID49_Pos               _U_(17)                                              /**< (PMC_SLPWK_SR1) Peripheral 49 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID49_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID49_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 49 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID49(value)            (PMC_SLPWK_SR1_PID49_Msk & ((value) << PMC_SLPWK_SR1_PID49_Pos))
#define PMC_SLPWK_SR1_PID50_Pos               _U_(18)                                              /**< (PMC_SLPWK_SR1) Peripheral 50 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID50_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID50_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 50 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID50(value)            (PMC_SLPWK_SR1_PID50_Msk & ((value) << PMC_SLPWK_SR1_PID50_Pos))
#define PMC_SLPWK_SR1_PID51_Pos               _U_(19)                                              /**< (PMC_SLPWK_SR1) Peripheral 51 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID51_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID51_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 51 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID51(value)            (PMC_SLPWK_SR1_PID51_Msk & ((value) << PMC_SLPWK_SR1_PID51_Pos))
#define PMC_SLPWK_SR1_PID52_Pos               _U_(20)                                              /**< (PMC_SLPWK_SR1) Peripheral 52 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID52_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID52_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 52 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID52(value)            (PMC_SLPWK_SR1_PID52_Msk & ((value) << PMC_SLPWK_SR1_PID52_Pos))
#define PMC_SLPWK_SR1_PID56_Pos               _U_(24)                                              /**< (PMC_SLPWK_SR1) Peripheral 56 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID56_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID56_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 56 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID56(value)            (PMC_SLPWK_SR1_PID56_Msk & ((value) << PMC_SLPWK_SR1_PID56_Pos))
#define PMC_SLPWK_SR1_PID57_Pos               _U_(25)                                              /**< (PMC_SLPWK_SR1) Peripheral 57 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID57_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID57_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 57 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID57(value)            (PMC_SLPWK_SR1_PID57_Msk & ((value) << PMC_SLPWK_SR1_PID57_Pos))
#define PMC_SLPWK_SR1_PID58_Pos               _U_(26)                                              /**< (PMC_SLPWK_SR1) Peripheral 58 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID58_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID58_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 58 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID58(value)            (PMC_SLPWK_SR1_PID58_Msk & ((value) << PMC_SLPWK_SR1_PID58_Pos))
#define PMC_SLPWK_SR1_PID59_Pos               _U_(27)                                              /**< (PMC_SLPWK_SR1) Peripheral 59 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID59_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID59_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 59 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID59(value)            (PMC_SLPWK_SR1_PID59_Msk & ((value) << PMC_SLPWK_SR1_PID59_Pos))
#define PMC_SLPWK_SR1_PID60_Pos               _U_(28)                                              /**< (PMC_SLPWK_SR1) Peripheral 60 SleepWalking Status Position */
#define PMC_SLPWK_SR1_PID60_Msk               (_U_(0x1) << PMC_SLPWK_SR1_PID60_Pos)                /**< (PMC_SLPWK_SR1) Peripheral 60 SleepWalking Status Mask */
#define PMC_SLPWK_SR1_PID60(value)            (PMC_SLPWK_SR1_PID60_Msk & ((value) << PMC_SLPWK_SR1_PID60_Pos))
#define PMC_SLPWK_SR1_Msk                     _U_(0x1F1FFFAF)                                      /**< (PMC_SLPWK_SR1) Register Mask  */

#define PMC_SLPWK_SR1_PID_Pos                 _U_(0)                                               /**< (PMC_SLPWK_SR1 Position) Peripheral 6x SleepWalking Status */
#define PMC_SLPWK_SR1_PID_Msk                 (_U_(0xFFFFFF) << PMC_SLPWK_SR1_PID_Pos)             /**< (PMC_SLPWK_SR1 Mask) PID */
#define PMC_SLPWK_SR1_PID(value)              (PMC_SLPWK_SR1_PID_Msk & ((value) << PMC_SLPWK_SR1_PID_Pos)) 

/* -------- PMC_SLPWK_ASR1 : (PMC Offset: 0x140) ( R/ 32) SleepWalking Activity Status Register 1 -------- */
#define PMC_SLPWK_ASR1_PID32_Pos              _U_(0)                                               /**< (PMC_SLPWK_ASR1) Peripheral 32 Activity Status Position */
#define PMC_SLPWK_ASR1_PID32_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID32_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 32 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID32(value)           (PMC_SLPWK_ASR1_PID32_Msk & ((value) << PMC_SLPWK_ASR1_PID32_Pos))
#define PMC_SLPWK_ASR1_PID33_Pos              _U_(1)                                               /**< (PMC_SLPWK_ASR1) Peripheral 33 Activity Status Position */
#define PMC_SLPWK_ASR1_PID33_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID33_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 33 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID33(value)           (PMC_SLPWK_ASR1_PID33_Msk & ((value) << PMC_SLPWK_ASR1_PID33_Pos))
#define PMC_SLPWK_ASR1_PID34_Pos              _U_(2)                                               /**< (PMC_SLPWK_ASR1) Peripheral 34 Activity Status Position */
#define PMC_SLPWK_ASR1_PID34_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID34_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 34 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID34(value)           (PMC_SLPWK_ASR1_PID34_Msk & ((value) << PMC_SLPWK_ASR1_PID34_Pos))
#define PMC_SLPWK_ASR1_PID35_Pos              _U_(3)                                               /**< (PMC_SLPWK_ASR1) Peripheral 35 Activity Status Position */
#define PMC_SLPWK_ASR1_PID35_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID35_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 35 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID35(value)           (PMC_SLPWK_ASR1_PID35_Msk & ((value) << PMC_SLPWK_ASR1_PID35_Pos))
#define PMC_SLPWK_ASR1_PID37_Pos              _U_(5)                                               /**< (PMC_SLPWK_ASR1) Peripheral 37 Activity Status Position */
#define PMC_SLPWK_ASR1_PID37_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID37_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 37 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID37(value)           (PMC_SLPWK_ASR1_PID37_Msk & ((value) << PMC_SLPWK_ASR1_PID37_Pos))
#define PMC_SLPWK_ASR1_PID39_Pos              _U_(7)                                               /**< (PMC_SLPWK_ASR1) Peripheral 39 Activity Status Position */
#define PMC_SLPWK_ASR1_PID39_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID39_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 39 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID39(value)           (PMC_SLPWK_ASR1_PID39_Msk & ((value) << PMC_SLPWK_ASR1_PID39_Pos))
#define PMC_SLPWK_ASR1_PID40_Pos              _U_(8)                                               /**< (PMC_SLPWK_ASR1) Peripheral 40 Activity Status Position */
#define PMC_SLPWK_ASR1_PID40_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID40_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 40 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID40(value)           (PMC_SLPWK_ASR1_PID40_Msk & ((value) << PMC_SLPWK_ASR1_PID40_Pos))
#define PMC_SLPWK_ASR1_PID41_Pos              _U_(9)                                               /**< (PMC_SLPWK_ASR1) Peripheral 41 Activity Status Position */
#define PMC_SLPWK_ASR1_PID41_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID41_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 41 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID41(value)           (PMC_SLPWK_ASR1_PID41_Msk & ((value) << PMC_SLPWK_ASR1_PID41_Pos))
#define PMC_SLPWK_ASR1_PID42_Pos              _U_(10)                                              /**< (PMC_SLPWK_ASR1) Peripheral 42 Activity Status Position */
#define PMC_SLPWK_ASR1_PID42_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID42_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 42 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID42(value)           (PMC_SLPWK_ASR1_PID42_Msk & ((value) << PMC_SLPWK_ASR1_PID42_Pos))
#define PMC_SLPWK_ASR1_PID43_Pos              _U_(11)                                              /**< (PMC_SLPWK_ASR1) Peripheral 43 Activity Status Position */
#define PMC_SLPWK_ASR1_PID43_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID43_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 43 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID43(value)           (PMC_SLPWK_ASR1_PID43_Msk & ((value) << PMC_SLPWK_ASR1_PID43_Pos))
#define PMC_SLPWK_ASR1_PID44_Pos              _U_(12)                                              /**< (PMC_SLPWK_ASR1) Peripheral 44 Activity Status Position */
#define PMC_SLPWK_ASR1_PID44_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID44_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 44 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID44(value)           (PMC_SLPWK_ASR1_PID44_Msk & ((value) << PMC_SLPWK_ASR1_PID44_Pos))
#define PMC_SLPWK_ASR1_PID45_Pos              _U_(13)                                              /**< (PMC_SLPWK_ASR1) Peripheral 45 Activity Status Position */
#define PMC_SLPWK_ASR1_PID45_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID45_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 45 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID45(value)           (PMC_SLPWK_ASR1_PID45_Msk & ((value) << PMC_SLPWK_ASR1_PID45_Pos))
#define PMC_SLPWK_ASR1_PID46_Pos              _U_(14)                                              /**< (PMC_SLPWK_ASR1) Peripheral 46 Activity Status Position */
#define PMC_SLPWK_ASR1_PID46_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID46_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 46 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID46(value)           (PMC_SLPWK_ASR1_PID46_Msk & ((value) << PMC_SLPWK_ASR1_PID46_Pos))
#define PMC_SLPWK_ASR1_PID47_Pos              _U_(15)                                              /**< (PMC_SLPWK_ASR1) Peripheral 47 Activity Status Position */
#define PMC_SLPWK_ASR1_PID47_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID47_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 47 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID47(value)           (PMC_SLPWK_ASR1_PID47_Msk & ((value) << PMC_SLPWK_ASR1_PID47_Pos))
#define PMC_SLPWK_ASR1_PID48_Pos              _U_(16)                                              /**< (PMC_SLPWK_ASR1) Peripheral 48 Activity Status Position */
#define PMC_SLPWK_ASR1_PID48_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID48_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 48 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID48(value)           (PMC_SLPWK_ASR1_PID48_Msk & ((value) << PMC_SLPWK_ASR1_PID48_Pos))
#define PMC_SLPWK_ASR1_PID49_Pos              _U_(17)                                              /**< (PMC_SLPWK_ASR1) Peripheral 49 Activity Status Position */
#define PMC_SLPWK_ASR1_PID49_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID49_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 49 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID49(value)           (PMC_SLPWK_ASR1_PID49_Msk & ((value) << PMC_SLPWK_ASR1_PID49_Pos))
#define PMC_SLPWK_ASR1_PID50_Pos              _U_(18)                                              /**< (PMC_SLPWK_ASR1) Peripheral 50 Activity Status Position */
#define PMC_SLPWK_ASR1_PID50_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID50_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 50 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID50(value)           (PMC_SLPWK_ASR1_PID50_Msk & ((value) << PMC_SLPWK_ASR1_PID50_Pos))
#define PMC_SLPWK_ASR1_PID51_Pos              _U_(19)                                              /**< (PMC_SLPWK_ASR1) Peripheral 51 Activity Status Position */
#define PMC_SLPWK_ASR1_PID51_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID51_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 51 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID51(value)           (PMC_SLPWK_ASR1_PID51_Msk & ((value) << PMC_SLPWK_ASR1_PID51_Pos))
#define PMC_SLPWK_ASR1_PID52_Pos              _U_(20)                                              /**< (PMC_SLPWK_ASR1) Peripheral 52 Activity Status Position */
#define PMC_SLPWK_ASR1_PID52_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID52_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 52 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID52(value)           (PMC_SLPWK_ASR1_PID52_Msk & ((value) << PMC_SLPWK_ASR1_PID52_Pos))
#define PMC_SLPWK_ASR1_PID56_Pos              _U_(24)                                              /**< (PMC_SLPWK_ASR1) Peripheral 56 Activity Status Position */
#define PMC_SLPWK_ASR1_PID56_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID56_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 56 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID56(value)           (PMC_SLPWK_ASR1_PID56_Msk & ((value) << PMC_SLPWK_ASR1_PID56_Pos))
#define PMC_SLPWK_ASR1_PID57_Pos              _U_(25)                                              /**< (PMC_SLPWK_ASR1) Peripheral 57 Activity Status Position */
#define PMC_SLPWK_ASR1_PID57_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID57_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 57 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID57(value)           (PMC_SLPWK_ASR1_PID57_Msk & ((value) << PMC_SLPWK_ASR1_PID57_Pos))
#define PMC_SLPWK_ASR1_PID58_Pos              _U_(26)                                              /**< (PMC_SLPWK_ASR1) Peripheral 58 Activity Status Position */
#define PMC_SLPWK_ASR1_PID58_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID58_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 58 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID58(value)           (PMC_SLPWK_ASR1_PID58_Msk & ((value) << PMC_SLPWK_ASR1_PID58_Pos))
#define PMC_SLPWK_ASR1_PID59_Pos              _U_(27)                                              /**< (PMC_SLPWK_ASR1) Peripheral 59 Activity Status Position */
#define PMC_SLPWK_ASR1_PID59_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID59_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 59 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID59(value)           (PMC_SLPWK_ASR1_PID59_Msk & ((value) << PMC_SLPWK_ASR1_PID59_Pos))
#define PMC_SLPWK_ASR1_PID60_Pos              _U_(28)                                              /**< (PMC_SLPWK_ASR1) Peripheral 60 Activity Status Position */
#define PMC_SLPWK_ASR1_PID60_Msk              (_U_(0x1) << PMC_SLPWK_ASR1_PID60_Pos)               /**< (PMC_SLPWK_ASR1) Peripheral 60 Activity Status Mask */
#define PMC_SLPWK_ASR1_PID60(value)           (PMC_SLPWK_ASR1_PID60_Msk & ((value) << PMC_SLPWK_ASR1_PID60_Pos))
#define PMC_SLPWK_ASR1_Msk                    _U_(0x1F1FFFAF)                                      /**< (PMC_SLPWK_ASR1) Register Mask  */

#define PMC_SLPWK_ASR1_PID_Pos                _U_(0)                                               /**< (PMC_SLPWK_ASR1 Position) Peripheral 6x Activity Status */
#define PMC_SLPWK_ASR1_PID_Msk                (_U_(0xFFFFFF) << PMC_SLPWK_ASR1_PID_Pos)            /**< (PMC_SLPWK_ASR1 Mask) PID */
#define PMC_SLPWK_ASR1_PID(value)             (PMC_SLPWK_ASR1_PID_Msk & ((value) << PMC_SLPWK_ASR1_PID_Pos)) 

/* -------- PMC_SLPWK_AIPR : (PMC Offset: 0x144) ( R/ 32) SleepWalking Activity In Progress Register -------- */
#define PMC_SLPWK_AIPR_AIP_Pos                _U_(0)                                               /**< (PMC_SLPWK_AIPR) Activity In Progress Position */
#define PMC_SLPWK_AIPR_AIP_Msk                (_U_(0x1) << PMC_SLPWK_AIPR_AIP_Pos)                 /**< (PMC_SLPWK_AIPR) Activity In Progress Mask */
#define PMC_SLPWK_AIPR_AIP(value)             (PMC_SLPWK_AIPR_AIP_Msk & ((value) << PMC_SLPWK_AIPR_AIP_Pos))
#define PMC_SLPWK_AIPR_Msk                    _U_(0x00000001)                                      /**< (PMC_SLPWK_AIPR) Register Mask  */


/** \brief PMC register offsets definitions */
#define PMC_SCER_REG_OFST              (0x00)              /**< (PMC_SCER) System Clock Enable Register Offset */
#define PMC_SCDR_REG_OFST              (0x04)              /**< (PMC_SCDR) System Clock Disable Register Offset */
#define PMC_SCSR_REG_OFST              (0x08)              /**< (PMC_SCSR) System Clock Status Register Offset */
#define PMC_PCER0_REG_OFST             (0x10)              /**< (PMC_PCER0) Peripheral Clock Enable Register 0 Offset */
#define PMC_PCDR0_REG_OFST             (0x14)              /**< (PMC_PCDR0) Peripheral Clock Disable Register 0 Offset */
#define PMC_PCSR0_REG_OFST             (0x18)              /**< (PMC_PCSR0) Peripheral Clock Status Register 0 Offset */
#define CKGR_UCKR_REG_OFST             (0x1C)              /**< (CKGR_UCKR) UTMI Clock Register Offset */
#define CKGR_MOR_REG_OFST              (0x20)              /**< (CKGR_MOR) Main Oscillator Register Offset */
#define CKGR_MCFR_REG_OFST             (0x24)              /**< (CKGR_MCFR) Main Clock Frequency Register Offset */
#define CKGR_PLLAR_REG_OFST            (0x28)              /**< (CKGR_PLLAR) PLLA Register Offset */
#define PMC_MCKR_REG_OFST              (0x30)              /**< (PMC_MCKR) Master Clock Register Offset */
#define PMC_USB_REG_OFST               (0x38)              /**< (PMC_USB) USB Clock Register Offset */
#define PMC_PCK_REG_OFST               (0x40)              /**< (PMC_PCK) Programmable Clock Register Offset */
#define PMC_IER_REG_OFST               (0x60)              /**< (PMC_IER) Interrupt Enable Register Offset */
#define PMC_IDR_REG_OFST               (0x64)              /**< (PMC_IDR) Interrupt Disable Register Offset */
#define PMC_SR_REG_OFST                (0x68)              /**< (PMC_SR) Status Register Offset */
#define PMC_IMR_REG_OFST               (0x6C)              /**< (PMC_IMR) Interrupt Mask Register Offset */
#define PMC_FSMR_REG_OFST              (0x70)              /**< (PMC_FSMR) Fast Startup Mode Register Offset */
#define PMC_FSPR_REG_OFST              (0x74)              /**< (PMC_FSPR) Fast Startup Polarity Register Offset */
#define PMC_FOCR_REG_OFST              (0x78)              /**< (PMC_FOCR) Fault Output Clear Register Offset */
#define PMC_WPMR_REG_OFST              (0xE4)              /**< (PMC_WPMR) Write Protection Mode Register Offset */
#define PMC_WPSR_REG_OFST              (0xE8)              /**< (PMC_WPSR) Write Protection Status Register Offset */
#define PMC_PCER1_REG_OFST             (0x100)             /**< (PMC_PCER1) Peripheral Clock Enable Register 1 Offset */
#define PMC_PCDR1_REG_OFST             (0x104)             /**< (PMC_PCDR1) Peripheral Clock Disable Register 1 Offset */
#define PMC_PCSR1_REG_OFST             (0x108)             /**< (PMC_PCSR1) Peripheral Clock Status Register 1 Offset */
#define PMC_PCR_REG_OFST               (0x10C)             /**< (PMC_PCR) Peripheral Control Register Offset */
#define PMC_OCR_REG_OFST               (0x110)             /**< (PMC_OCR) Oscillator Calibration Register Offset */
#define PMC_SLPWK_ER0_REG_OFST         (0x114)             /**< (PMC_SLPWK_ER0) SleepWalking Enable Register 0 Offset */
#define PMC_SLPWK_DR0_REG_OFST         (0x118)             /**< (PMC_SLPWK_DR0) SleepWalking Disable Register 0 Offset */
#define PMC_SLPWK_SR0_REG_OFST         (0x11C)             /**< (PMC_SLPWK_SR0) SleepWalking Status Register 0 Offset */
#define PMC_SLPWK_ASR0_REG_OFST        (0x120)             /**< (PMC_SLPWK_ASR0) SleepWalking Activity Status Register 0 Offset */
#define PMC_PMMR_REG_OFST              (0x130)             /**< (PMC_PMMR) PLL Maximum Multiplier Value Register Offset */
#define PMC_SLPWK_ER1_REG_OFST         (0x134)             /**< (PMC_SLPWK_ER1) SleepWalking Enable Register 1 Offset */
#define PMC_SLPWK_DR1_REG_OFST         (0x138)             /**< (PMC_SLPWK_DR1) SleepWalking Disable Register 1 Offset */
#define PMC_SLPWK_SR1_REG_OFST         (0x13C)             /**< (PMC_SLPWK_SR1) SleepWalking Status Register 1 Offset */
#define PMC_SLPWK_ASR1_REG_OFST        (0x140)             /**< (PMC_SLPWK_ASR1) SleepWalking Activity Status Register 1 Offset */
#define PMC_SLPWK_AIPR_REG_OFST        (0x144)             /**< (PMC_SLPWK_AIPR) SleepWalking Activity In Progress Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief PMC register API structure */
typedef struct
{
  __O   uint32_t                       PMC_SCER;           /**< Offset: 0x00 ( /W  32) System Clock Enable Register */
  __O   uint32_t                       PMC_SCDR;           /**< Offset: 0x04 ( /W  32) System Clock Disable Register */
  __I   uint32_t                       PMC_SCSR;           /**< Offset: 0x08 (R/   32) System Clock Status Register */
  __I   uint8_t                        Reserved1[0x04];
  __O   uint32_t                       PMC_PCER0;          /**< Offset: 0x10 ( /W  32) Peripheral Clock Enable Register 0 */
  __O   uint32_t                       PMC_PCDR0;          /**< Offset: 0x14 ( /W  32) Peripheral Clock Disable Register 0 */
  __I   uint32_t                       PMC_PCSR0;          /**< Offset: 0x18 (R/   32) Peripheral Clock Status Register 0 */
  __IO  uint32_t                       CKGR_UCKR;          /**< Offset: 0x1C (R/W  32) UTMI Clock Register */
  __IO  uint32_t                       CKGR_MOR;           /**< Offset: 0x20 (R/W  32) Main Oscillator Register */
  __IO  uint32_t                       CKGR_MCFR;          /**< Offset: 0x24 (R/W  32) Main Clock Frequency Register */
  __IO  uint32_t                       CKGR_PLLAR;         /**< Offset: 0x28 (R/W  32) PLLA Register */
  __I   uint8_t                        Reserved2[0x04];
  __IO  uint32_t                       PMC_MCKR;           /**< Offset: 0x30 (R/W  32) Master Clock Register */
  __I   uint8_t                        Reserved3[0x04];
  __IO  uint32_t                       PMC_USB;            /**< Offset: 0x38 (R/W  32) USB Clock Register */
  __I   uint8_t                        Reserved4[0x04];
  __IO  uint32_t                       PMC_PCK[8];         /**< Offset: 0x40 (R/W  32) Programmable Clock Register */
  __O   uint32_t                       PMC_IER;            /**< Offset: 0x60 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       PMC_IDR;            /**< Offset: 0x64 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       PMC_SR;             /**< Offset: 0x68 (R/   32) Status Register */
  __I   uint32_t                       PMC_IMR;            /**< Offset: 0x6C (R/   32) Interrupt Mask Register */
  __IO  uint32_t                       PMC_FSMR;           /**< Offset: 0x70 (R/W  32) Fast Startup Mode Register */
  __IO  uint32_t                       PMC_FSPR;           /**< Offset: 0x74 (R/W  32) Fast Startup Polarity Register */
  __O   uint32_t                       PMC_FOCR;           /**< Offset: 0x78 ( /W  32) Fault Output Clear Register */
  __I   uint8_t                        Reserved5[0x68];
  __IO  uint32_t                       PMC_WPMR;           /**< Offset: 0xE4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       PMC_WPSR;           /**< Offset: 0xE8 (R/   32) Write Protection Status Register */
  __I   uint8_t                        Reserved6[0x14];
  __O   uint32_t                       PMC_PCER1;          /**< Offset: 0x100 ( /W  32) Peripheral Clock Enable Register 1 */
  __O   uint32_t                       PMC_PCDR1;          /**< Offset: 0x104 ( /W  32) Peripheral Clock Disable Register 1 */
  __I   uint32_t                       PMC_PCSR1;          /**< Offset: 0x108 (R/   32) Peripheral Clock Status Register 1 */
  __IO  uint32_t                       PMC_PCR;            /**< Offset: 0x10C (R/W  32) Peripheral Control Register */
  __IO  uint32_t                       PMC_OCR;            /**< Offset: 0x110 (R/W  32) Oscillator Calibration Register */
  __O   uint32_t                       PMC_SLPWK_ER0;      /**< Offset: 0x114 ( /W  32) SleepWalking Enable Register 0 */
  __O   uint32_t                       PMC_SLPWK_DR0;      /**< Offset: 0x118 ( /W  32) SleepWalking Disable Register 0 */
  __I   uint32_t                       PMC_SLPWK_SR0;      /**< Offset: 0x11C (R/   32) SleepWalking Status Register 0 */
  __I   uint32_t                       PMC_SLPWK_ASR0;     /**< Offset: 0x120 (R/   32) SleepWalking Activity Status Register 0 */
  __I   uint8_t                        Reserved7[0x0C];
  __IO  uint32_t                       PMC_PMMR;           /**< Offset: 0x130 (R/W  32) PLL Maximum Multiplier Value Register */
  __O   uint32_t                       PMC_SLPWK_ER1;      /**< Offset: 0x134 ( /W  32) SleepWalking Enable Register 1 */
  __O   uint32_t                       PMC_SLPWK_DR1;      /**< Offset: 0x138 ( /W  32) SleepWalking Disable Register 1 */
  __I   uint32_t                       PMC_SLPWK_SR1;      /**< Offset: 0x13C (R/   32) SleepWalking Status Register 1 */
  __I   uint32_t                       PMC_SLPWK_ASR1;     /**< Offset: 0x140 (R/   32) SleepWalking Activity Status Register 1 */
  __I   uint32_t                       PMC_SLPWK_AIPR;     /**< Offset: 0x144 (R/   32) SleepWalking Activity In Progress Register */
} pmc_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_PMC_COMPONENT_H_ */
