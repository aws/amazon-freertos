/**
 * \brief Component description for DACC
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
#ifndef _SAME70_DACC_COMPONENT_H_
#define _SAME70_DACC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR DACC                                         */
/* ************************************************************************** */

/* -------- DACC_CR : (DACC Offset: 0x00) ( /W 32) Control Register -------- */
#define DACC_CR_SWRST_Pos                     _U_(0)                                               /**< (DACC_CR) Software Reset Position */
#define DACC_CR_SWRST_Msk                     (_U_(0x1) << DACC_CR_SWRST_Pos)                      /**< (DACC_CR) Software Reset Mask */
#define DACC_CR_SWRST(value)                  (DACC_CR_SWRST_Msk & ((value) << DACC_CR_SWRST_Pos))
#define DACC_CR_Msk                           _U_(0x00000001)                                      /**< (DACC_CR) Register Mask  */


/* -------- DACC_MR : (DACC Offset: 0x04) (R/W 32) Mode Register -------- */
#define DACC_MR_MAXS0_Pos                     _U_(0)                                               /**< (DACC_MR) Max Speed Mode for Channel 0 Position */
#define DACC_MR_MAXS0_Msk                     (_U_(0x1) << DACC_MR_MAXS0_Pos)                      /**< (DACC_MR) Max Speed Mode for Channel 0 Mask */
#define DACC_MR_MAXS0(value)                  (DACC_MR_MAXS0_Msk & ((value) << DACC_MR_MAXS0_Pos))
#define   DACC_MR_MAXS0_TRIG_EVENT_Val        _U_(0x0)                                             /**< (DACC_MR) External trigger mode or Free-running mode enabled. (See TRGENx.DACC_TRIGR.)  */
#define   DACC_MR_MAXS0_MAXIMUM_Val           _U_(0x1)                                             /**< (DACC_MR) Max speed mode enabled.  */
#define DACC_MR_MAXS0_TRIG_EVENT              (DACC_MR_MAXS0_TRIG_EVENT_Val << DACC_MR_MAXS0_Pos)  /**< (DACC_MR) External trigger mode or Free-running mode enabled. (See TRGENx.DACC_TRIGR.) Position  */
#define DACC_MR_MAXS0_MAXIMUM                 (DACC_MR_MAXS0_MAXIMUM_Val << DACC_MR_MAXS0_Pos)     /**< (DACC_MR) Max speed mode enabled. Position  */
#define DACC_MR_MAXS1_Pos                     _U_(1)                                               /**< (DACC_MR) Max Speed Mode for Channel 1 Position */
#define DACC_MR_MAXS1_Msk                     (_U_(0x1) << DACC_MR_MAXS1_Pos)                      /**< (DACC_MR) Max Speed Mode for Channel 1 Mask */
#define DACC_MR_MAXS1(value)                  (DACC_MR_MAXS1_Msk & ((value) << DACC_MR_MAXS1_Pos))
#define   DACC_MR_MAXS1_TRIG_EVENT_Val        _U_(0x0)                                             /**< (DACC_MR) External trigger mode or Free-running mode enabled. (See TRGENx.DACC_TRIGR.)  */
#define   DACC_MR_MAXS1_MAXIMUM_Val           _U_(0x1)                                             /**< (DACC_MR) Max speed mode enabled.  */
#define DACC_MR_MAXS1_TRIG_EVENT              (DACC_MR_MAXS1_TRIG_EVENT_Val << DACC_MR_MAXS1_Pos)  /**< (DACC_MR) External trigger mode or Free-running mode enabled. (See TRGENx.DACC_TRIGR.) Position  */
#define DACC_MR_MAXS1_MAXIMUM                 (DACC_MR_MAXS1_MAXIMUM_Val << DACC_MR_MAXS1_Pos)     /**< (DACC_MR) Max speed mode enabled. Position  */
#define DACC_MR_WORD_Pos                      _U_(4)                                               /**< (DACC_MR) Word Transfer Mode Position */
#define DACC_MR_WORD_Msk                      (_U_(0x1) << DACC_MR_WORD_Pos)                       /**< (DACC_MR) Word Transfer Mode Mask */
#define DACC_MR_WORD(value)                   (DACC_MR_WORD_Msk & ((value) << DACC_MR_WORD_Pos))  
#define   DACC_MR_WORD_DISABLED_Val           _U_(0x0)                                             /**< (DACC_MR) One data to convert is written to the FIFO per access to DACC.  */
#define   DACC_MR_WORD_ENABLED_Val            _U_(0x1)                                             /**< (DACC_MR) Two data to convert are written to the FIFO per access to DACC (reduces the number of requests to DMA and the number of system bus accesses).  */
#define DACC_MR_WORD_DISABLED                 (DACC_MR_WORD_DISABLED_Val << DACC_MR_WORD_Pos)      /**< (DACC_MR) One data to convert is written to the FIFO per access to DACC. Position  */
#define DACC_MR_WORD_ENABLED                  (DACC_MR_WORD_ENABLED_Val << DACC_MR_WORD_Pos)       /**< (DACC_MR) Two data to convert are written to the FIFO per access to DACC (reduces the number of requests to DMA and the number of system bus accesses). Position  */
#define DACC_MR_ZERO_Pos                      _U_(5)                                               /**< (DACC_MR) Must always be written to 0. Position */
#define DACC_MR_ZERO_Msk                      (_U_(0x1) << DACC_MR_ZERO_Pos)                       /**< (DACC_MR) Must always be written to 0. Mask */
#define DACC_MR_ZERO(value)                   (DACC_MR_ZERO_Msk & ((value) << DACC_MR_ZERO_Pos))  
#define DACC_MR_DIFF_Pos                      _U_(23)                                              /**< (DACC_MR) Differential Mode Position */
#define DACC_MR_DIFF_Msk                      (_U_(0x1) << DACC_MR_DIFF_Pos)                       /**< (DACC_MR) Differential Mode Mask */
#define DACC_MR_DIFF(value)                   (DACC_MR_DIFF_Msk & ((value) << DACC_MR_DIFF_Pos))  
#define   DACC_MR_DIFF_DISABLED_Val           _U_(0x0)                                             /**< (DACC_MR) DAC0 and DAC1 are single-ended outputs.  */
#define   DACC_MR_DIFF_ENABLED_Val            _U_(0x1)                                             /**< (DACC_MR) DACP and DACN are differential outputs. The differential level is configured by the channel 0 value.  */
#define DACC_MR_DIFF_DISABLED                 (DACC_MR_DIFF_DISABLED_Val << DACC_MR_DIFF_Pos)      /**< (DACC_MR) DAC0 and DAC1 are single-ended outputs. Position  */
#define DACC_MR_DIFF_ENABLED                  (DACC_MR_DIFF_ENABLED_Val << DACC_MR_DIFF_Pos)       /**< (DACC_MR) DACP and DACN are differential outputs. The differential level is configured by the channel 0 value. Position  */
#define DACC_MR_PRESCALER_Pos                 _U_(24)                                              /**< (DACC_MR) Peripheral Clock to DAC Clock Ratio Position */
#define DACC_MR_PRESCALER_Msk                 (_U_(0xF) << DACC_MR_PRESCALER_Pos)                  /**< (DACC_MR) Peripheral Clock to DAC Clock Ratio Mask */
#define DACC_MR_PRESCALER(value)              (DACC_MR_PRESCALER_Msk & ((value) << DACC_MR_PRESCALER_Pos))
#define DACC_MR_Msk                           _U_(0x0F800033)                                      /**< (DACC_MR) Register Mask  */

#define DACC_MR_MAXS_Pos                      _U_(0)                                               /**< (DACC_MR Position) Max Speed Mode for Channel x */
#define DACC_MR_MAXS_Msk                      (_U_(0x3) << DACC_MR_MAXS_Pos)                       /**< (DACC_MR Mask) MAXS */
#define DACC_MR_MAXS(value)                   (DACC_MR_MAXS_Msk & ((value) << DACC_MR_MAXS_Pos))   

/* -------- DACC_TRIGR : (DACC Offset: 0x08) (R/W 32) Trigger Register -------- */
#define DACC_TRIGR_TRGEN0_Pos                 _U_(0)                                               /**< (DACC_TRIGR) Trigger Enable of Channel 0 Position */
#define DACC_TRIGR_TRGEN0_Msk                 (_U_(0x1) << DACC_TRIGR_TRGEN0_Pos)                  /**< (DACC_TRIGR) Trigger Enable of Channel 0 Mask */
#define DACC_TRIGR_TRGEN0(value)              (DACC_TRIGR_TRGEN0_Msk & ((value) << DACC_TRIGR_TRGEN0_Pos))
#define   DACC_TRIGR_TRGEN0_DIS_Val           _U_(0x0)                                             /**< (DACC_TRIGR) External trigger mode disabled. DACC is in Free-running mode or Max speed mode.  */
#define   DACC_TRIGR_TRGEN0_EN_Val            _U_(0x1)                                             /**< (DACC_TRIGR) External trigger mode enabled.  */
#define DACC_TRIGR_TRGEN0_DIS                 (DACC_TRIGR_TRGEN0_DIS_Val << DACC_TRIGR_TRGEN0_Pos) /**< (DACC_TRIGR) External trigger mode disabled. DACC is in Free-running mode or Max speed mode. Position  */
#define DACC_TRIGR_TRGEN0_EN                  (DACC_TRIGR_TRGEN0_EN_Val << DACC_TRIGR_TRGEN0_Pos)  /**< (DACC_TRIGR) External trigger mode enabled. Position  */
#define DACC_TRIGR_TRGEN1_Pos                 _U_(1)                                               /**< (DACC_TRIGR) Trigger Enable of Channel 1 Position */
#define DACC_TRIGR_TRGEN1_Msk                 (_U_(0x1) << DACC_TRIGR_TRGEN1_Pos)                  /**< (DACC_TRIGR) Trigger Enable of Channel 1 Mask */
#define DACC_TRIGR_TRGEN1(value)              (DACC_TRIGR_TRGEN1_Msk & ((value) << DACC_TRIGR_TRGEN1_Pos))
#define   DACC_TRIGR_TRGEN1_DIS_Val           _U_(0x0)                                             /**< (DACC_TRIGR) External trigger mode disabled. DACC is in Free-running mode or Max speed mode.  */
#define   DACC_TRIGR_TRGEN1_EN_Val            _U_(0x1)                                             /**< (DACC_TRIGR) External trigger mode enabled.  */
#define DACC_TRIGR_TRGEN1_DIS                 (DACC_TRIGR_TRGEN1_DIS_Val << DACC_TRIGR_TRGEN1_Pos) /**< (DACC_TRIGR) External trigger mode disabled. DACC is in Free-running mode or Max speed mode. Position  */
#define DACC_TRIGR_TRGEN1_EN                  (DACC_TRIGR_TRGEN1_EN_Val << DACC_TRIGR_TRGEN1_Pos)  /**< (DACC_TRIGR) External trigger mode enabled. Position  */
#define DACC_TRIGR_TRGSEL0_Pos                _U_(4)                                               /**< (DACC_TRIGR) Trigger Selection of Channel 0 Position */
#define DACC_TRIGR_TRGSEL0_Msk                (_U_(0x7) << DACC_TRIGR_TRGSEL0_Pos)                 /**< (DACC_TRIGR) Trigger Selection of Channel 0 Mask */
#define DACC_TRIGR_TRGSEL0(value)             (DACC_TRIGR_TRGSEL0_Msk & ((value) << DACC_TRIGR_TRGSEL0_Pos))
#define   DACC_TRIGR_TRGSEL0_TRGSEL0_Val      _U_(0x0)                                             /**< (DACC_TRIGR) DAC External Trigger Input (DATRG)  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL1_Val      _U_(0x1)                                             /**< (DACC_TRIGR) TC0 Channel 0 Output (TIOA0)  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL2_Val      _U_(0x2)                                             /**< (DACC_TRIGR) TC0 Channel 1 Output (TIOA1)  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL3_Val      _U_(0x3)                                             /**< (DACC_TRIGR) TC0 Channel 2 Output (TIOA2)  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL4_Val      _U_(0x4)                                             /**< (DACC_TRIGR) PWM0 Event Line 0  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL5_Val      _U_(0x5)                                             /**< (DACC_TRIGR) PWM0 Event Line 1  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL6_Val      _U_(0x6)                                             /**< (DACC_TRIGR) PWM1 Event Line 0  */
#define   DACC_TRIGR_TRGSEL0_TRGSEL7_Val      _U_(0x7)                                             /**< (DACC_TRIGR) PWM1 Event Line 1  */
#define DACC_TRIGR_TRGSEL0_TRGSEL0            (DACC_TRIGR_TRGSEL0_TRGSEL0_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) DAC External Trigger Input (DATRG) Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL1            (DACC_TRIGR_TRGSEL0_TRGSEL1_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) TC0 Channel 0 Output (TIOA0) Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL2            (DACC_TRIGR_TRGSEL0_TRGSEL2_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) TC0 Channel 1 Output (TIOA1) Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL3            (DACC_TRIGR_TRGSEL0_TRGSEL3_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) TC0 Channel 2 Output (TIOA2) Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL4            (DACC_TRIGR_TRGSEL0_TRGSEL4_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) PWM0 Event Line 0 Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL5            (DACC_TRIGR_TRGSEL0_TRGSEL5_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) PWM0 Event Line 1 Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL6            (DACC_TRIGR_TRGSEL0_TRGSEL6_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) PWM1 Event Line 0 Position  */
#define DACC_TRIGR_TRGSEL0_TRGSEL7            (DACC_TRIGR_TRGSEL0_TRGSEL7_Val << DACC_TRIGR_TRGSEL0_Pos) /**< (DACC_TRIGR) PWM1 Event Line 1 Position  */
#define DACC_TRIGR_TRGSEL1_Pos                _U_(8)                                               /**< (DACC_TRIGR) Trigger Selection of Channel 1 Position */
#define DACC_TRIGR_TRGSEL1_Msk                (_U_(0x7) << DACC_TRIGR_TRGSEL1_Pos)                 /**< (DACC_TRIGR) Trigger Selection of Channel 1 Mask */
#define DACC_TRIGR_TRGSEL1(value)             (DACC_TRIGR_TRGSEL1_Msk & ((value) << DACC_TRIGR_TRGSEL1_Pos))
#define   DACC_TRIGR_TRGSEL1_TRGSEL0_Val      _U_(0x0)                                             /**< (DACC_TRIGR) DAC External Trigger Input (DATRG)  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL1_Val      _U_(0x1)                                             /**< (DACC_TRIGR) TC0 Channel 0 Output (TIOA0)  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL2_Val      _U_(0x2)                                             /**< (DACC_TRIGR) TC0 Channel 1 Output (TIOA1)  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL3_Val      _U_(0x3)                                             /**< (DACC_TRIGR) TC0 Channel 2 Output (TIOA2)  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL4_Val      _U_(0x4)                                             /**< (DACC_TRIGR) PWM0 Event Line 0  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL5_Val      _U_(0x5)                                             /**< (DACC_TRIGR) PWM0 Event Line 1  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL6_Val      _U_(0x6)                                             /**< (DACC_TRIGR) PWM1 Event Line 0  */
#define   DACC_TRIGR_TRGSEL1_TRGSEL7_Val      _U_(0x7)                                             /**< (DACC_TRIGR) PWM1 Event Line 1  */
#define DACC_TRIGR_TRGSEL1_TRGSEL0            (DACC_TRIGR_TRGSEL1_TRGSEL0_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) DAC External Trigger Input (DATRG) Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL1            (DACC_TRIGR_TRGSEL1_TRGSEL1_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) TC0 Channel 0 Output (TIOA0) Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL2            (DACC_TRIGR_TRGSEL1_TRGSEL2_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) TC0 Channel 1 Output (TIOA1) Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL3            (DACC_TRIGR_TRGSEL1_TRGSEL3_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) TC0 Channel 2 Output (TIOA2) Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL4            (DACC_TRIGR_TRGSEL1_TRGSEL4_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) PWM0 Event Line 0 Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL5            (DACC_TRIGR_TRGSEL1_TRGSEL5_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) PWM0 Event Line 1 Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL6            (DACC_TRIGR_TRGSEL1_TRGSEL6_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) PWM1 Event Line 0 Position  */
#define DACC_TRIGR_TRGSEL1_TRGSEL7            (DACC_TRIGR_TRGSEL1_TRGSEL7_Val << DACC_TRIGR_TRGSEL1_Pos) /**< (DACC_TRIGR) PWM1 Event Line 1 Position  */
#define DACC_TRIGR_OSR0_Pos                   _U_(16)                                              /**< (DACC_TRIGR) Over Sampling Ratio of Channel 0 Position */
#define DACC_TRIGR_OSR0_Msk                   (_U_(0x7) << DACC_TRIGR_OSR0_Pos)                    /**< (DACC_TRIGR) Over Sampling Ratio of Channel 0 Mask */
#define DACC_TRIGR_OSR0(value)                (DACC_TRIGR_OSR0_Msk & ((value) << DACC_TRIGR_OSR0_Pos))
#define   DACC_TRIGR_OSR0_OSR_1_Val           _U_(0x0)                                             /**< (DACC_TRIGR) OSR = 1  */
#define   DACC_TRIGR_OSR0_OSR_2_Val           _U_(0x1)                                             /**< (DACC_TRIGR) OSR = 2  */
#define   DACC_TRIGR_OSR0_OSR_4_Val           _U_(0x2)                                             /**< (DACC_TRIGR) OSR = 4  */
#define   DACC_TRIGR_OSR0_OSR_8_Val           _U_(0x3)                                             /**< (DACC_TRIGR) OSR = 8  */
#define   DACC_TRIGR_OSR0_OSR_16_Val          _U_(0x4)                                             /**< (DACC_TRIGR) OSR = 16  */
#define   DACC_TRIGR_OSR0_OSR_32_Val          _U_(0x5)                                             /**< (DACC_TRIGR) OSR = 32  */
#define DACC_TRIGR_OSR0_OSR_1                 (DACC_TRIGR_OSR0_OSR_1_Val << DACC_TRIGR_OSR0_Pos)   /**< (DACC_TRIGR) OSR = 1 Position  */
#define DACC_TRIGR_OSR0_OSR_2                 (DACC_TRIGR_OSR0_OSR_2_Val << DACC_TRIGR_OSR0_Pos)   /**< (DACC_TRIGR) OSR = 2 Position  */
#define DACC_TRIGR_OSR0_OSR_4                 (DACC_TRIGR_OSR0_OSR_4_Val << DACC_TRIGR_OSR0_Pos)   /**< (DACC_TRIGR) OSR = 4 Position  */
#define DACC_TRIGR_OSR0_OSR_8                 (DACC_TRIGR_OSR0_OSR_8_Val << DACC_TRIGR_OSR0_Pos)   /**< (DACC_TRIGR) OSR = 8 Position  */
#define DACC_TRIGR_OSR0_OSR_16                (DACC_TRIGR_OSR0_OSR_16_Val << DACC_TRIGR_OSR0_Pos)  /**< (DACC_TRIGR) OSR = 16 Position  */
#define DACC_TRIGR_OSR0_OSR_32                (DACC_TRIGR_OSR0_OSR_32_Val << DACC_TRIGR_OSR0_Pos)  /**< (DACC_TRIGR) OSR = 32 Position  */
#define DACC_TRIGR_OSR1_Pos                   _U_(20)                                              /**< (DACC_TRIGR) Over Sampling Ratio of Channel 1 Position */
#define DACC_TRIGR_OSR1_Msk                   (_U_(0x7) << DACC_TRIGR_OSR1_Pos)                    /**< (DACC_TRIGR) Over Sampling Ratio of Channel 1 Mask */
#define DACC_TRIGR_OSR1(value)                (DACC_TRIGR_OSR1_Msk & ((value) << DACC_TRIGR_OSR1_Pos))
#define   DACC_TRIGR_OSR1_OSR_1_Val           _U_(0x0)                                             /**< (DACC_TRIGR) OSR = 1  */
#define   DACC_TRIGR_OSR1_OSR_2_Val           _U_(0x1)                                             /**< (DACC_TRIGR) OSR = 2  */
#define   DACC_TRIGR_OSR1_OSR_4_Val           _U_(0x2)                                             /**< (DACC_TRIGR) OSR = 4  */
#define   DACC_TRIGR_OSR1_OSR_8_Val           _U_(0x3)                                             /**< (DACC_TRIGR) OSR = 8  */
#define   DACC_TRIGR_OSR1_OSR_16_Val          _U_(0x4)                                             /**< (DACC_TRIGR) OSR = 16  */
#define   DACC_TRIGR_OSR1_OSR_32_Val          _U_(0x5)                                             /**< (DACC_TRIGR) OSR = 32  */
#define DACC_TRIGR_OSR1_OSR_1                 (DACC_TRIGR_OSR1_OSR_1_Val << DACC_TRIGR_OSR1_Pos)   /**< (DACC_TRIGR) OSR = 1 Position  */
#define DACC_TRIGR_OSR1_OSR_2                 (DACC_TRIGR_OSR1_OSR_2_Val << DACC_TRIGR_OSR1_Pos)   /**< (DACC_TRIGR) OSR = 2 Position  */
#define DACC_TRIGR_OSR1_OSR_4                 (DACC_TRIGR_OSR1_OSR_4_Val << DACC_TRIGR_OSR1_Pos)   /**< (DACC_TRIGR) OSR = 4 Position  */
#define DACC_TRIGR_OSR1_OSR_8                 (DACC_TRIGR_OSR1_OSR_8_Val << DACC_TRIGR_OSR1_Pos)   /**< (DACC_TRIGR) OSR = 8 Position  */
#define DACC_TRIGR_OSR1_OSR_16                (DACC_TRIGR_OSR1_OSR_16_Val << DACC_TRIGR_OSR1_Pos)  /**< (DACC_TRIGR) OSR = 16 Position  */
#define DACC_TRIGR_OSR1_OSR_32                (DACC_TRIGR_OSR1_OSR_32_Val << DACC_TRIGR_OSR1_Pos)  /**< (DACC_TRIGR) OSR = 32 Position  */
#define DACC_TRIGR_Msk                        _U_(0x00770773)                                      /**< (DACC_TRIGR) Register Mask  */

#define DACC_TRIGR_TRGEN_Pos                  _U_(0)                                               /**< (DACC_TRIGR Position) Trigger Enable of Channel x */
#define DACC_TRIGR_TRGEN_Msk                  (_U_(0x3) << DACC_TRIGR_TRGEN_Pos)                   /**< (DACC_TRIGR Mask) TRGEN */
#define DACC_TRIGR_TRGEN(value)               (DACC_TRIGR_TRGEN_Msk & ((value) << DACC_TRIGR_TRGEN_Pos)) 

/* -------- DACC_CHER : (DACC Offset: 0x10) ( /W 32) Channel Enable Register -------- */
#define DACC_CHER_CH0_Pos                     _U_(0)                                               /**< (DACC_CHER) Channel 0 Enable Position */
#define DACC_CHER_CH0_Msk                     (_U_(0x1) << DACC_CHER_CH0_Pos)                      /**< (DACC_CHER) Channel 0 Enable Mask */
#define DACC_CHER_CH0(value)                  (DACC_CHER_CH0_Msk & ((value) << DACC_CHER_CH0_Pos))
#define DACC_CHER_CH1_Pos                     _U_(1)                                               /**< (DACC_CHER) Channel 1 Enable Position */
#define DACC_CHER_CH1_Msk                     (_U_(0x1) << DACC_CHER_CH1_Pos)                      /**< (DACC_CHER) Channel 1 Enable Mask */
#define DACC_CHER_CH1(value)                  (DACC_CHER_CH1_Msk & ((value) << DACC_CHER_CH1_Pos))
#define DACC_CHER_Msk                         _U_(0x00000003)                                      /**< (DACC_CHER) Register Mask  */

#define DACC_CHER_CH_Pos                      _U_(0)                                               /**< (DACC_CHER Position) Channel x Enable */
#define DACC_CHER_CH_Msk                      (_U_(0x3) << DACC_CHER_CH_Pos)                       /**< (DACC_CHER Mask) CH */
#define DACC_CHER_CH(value)                   (DACC_CHER_CH_Msk & ((value) << DACC_CHER_CH_Pos))   

/* -------- DACC_CHDR : (DACC Offset: 0x14) ( /W 32) Channel Disable Register -------- */
#define DACC_CHDR_CH0_Pos                     _U_(0)                                               /**< (DACC_CHDR) Channel 0 Disable Position */
#define DACC_CHDR_CH0_Msk                     (_U_(0x1) << DACC_CHDR_CH0_Pos)                      /**< (DACC_CHDR) Channel 0 Disable Mask */
#define DACC_CHDR_CH0(value)                  (DACC_CHDR_CH0_Msk & ((value) << DACC_CHDR_CH0_Pos))
#define DACC_CHDR_CH1_Pos                     _U_(1)                                               /**< (DACC_CHDR) Channel 1 Disable Position */
#define DACC_CHDR_CH1_Msk                     (_U_(0x1) << DACC_CHDR_CH1_Pos)                      /**< (DACC_CHDR) Channel 1 Disable Mask */
#define DACC_CHDR_CH1(value)                  (DACC_CHDR_CH1_Msk & ((value) << DACC_CHDR_CH1_Pos))
#define DACC_CHDR_Msk                         _U_(0x00000003)                                      /**< (DACC_CHDR) Register Mask  */

#define DACC_CHDR_CH_Pos                      _U_(0)                                               /**< (DACC_CHDR Position) Channel x Disable */
#define DACC_CHDR_CH_Msk                      (_U_(0x3) << DACC_CHDR_CH_Pos)                       /**< (DACC_CHDR Mask) CH */
#define DACC_CHDR_CH(value)                   (DACC_CHDR_CH_Msk & ((value) << DACC_CHDR_CH_Pos))   

/* -------- DACC_CHSR : (DACC Offset: 0x18) ( R/ 32) Channel Status Register -------- */
#define DACC_CHSR_CH0_Pos                     _U_(0)                                               /**< (DACC_CHSR) Channel 0 Status Position */
#define DACC_CHSR_CH0_Msk                     (_U_(0x1) << DACC_CHSR_CH0_Pos)                      /**< (DACC_CHSR) Channel 0 Status Mask */
#define DACC_CHSR_CH0(value)                  (DACC_CHSR_CH0_Msk & ((value) << DACC_CHSR_CH0_Pos))
#define DACC_CHSR_CH1_Pos                     _U_(1)                                               /**< (DACC_CHSR) Channel 1 Status Position */
#define DACC_CHSR_CH1_Msk                     (_U_(0x1) << DACC_CHSR_CH1_Pos)                      /**< (DACC_CHSR) Channel 1 Status Mask */
#define DACC_CHSR_CH1(value)                  (DACC_CHSR_CH1_Msk & ((value) << DACC_CHSR_CH1_Pos))
#define DACC_CHSR_DACRDY0_Pos                 _U_(8)                                               /**< (DACC_CHSR) DAC Ready Flag Position */
#define DACC_CHSR_DACRDY0_Msk                 (_U_(0x1) << DACC_CHSR_DACRDY0_Pos)                  /**< (DACC_CHSR) DAC Ready Flag Mask */
#define DACC_CHSR_DACRDY0(value)              (DACC_CHSR_DACRDY0_Msk & ((value) << DACC_CHSR_DACRDY0_Pos))
#define DACC_CHSR_DACRDY1_Pos                 _U_(9)                                               /**< (DACC_CHSR) DAC Ready Flag Position */
#define DACC_CHSR_DACRDY1_Msk                 (_U_(0x1) << DACC_CHSR_DACRDY1_Pos)                  /**< (DACC_CHSR) DAC Ready Flag Mask */
#define DACC_CHSR_DACRDY1(value)              (DACC_CHSR_DACRDY1_Msk & ((value) << DACC_CHSR_DACRDY1_Pos))
#define DACC_CHSR_Msk                         _U_(0x00000303)                                      /**< (DACC_CHSR) Register Mask  */

#define DACC_CHSR_CH_Pos                      _U_(0)                                               /**< (DACC_CHSR Position) Channel x Status */
#define DACC_CHSR_CH_Msk                      (_U_(0x3) << DACC_CHSR_CH_Pos)                       /**< (DACC_CHSR Mask) CH */
#define DACC_CHSR_CH(value)                   (DACC_CHSR_CH_Msk & ((value) << DACC_CHSR_CH_Pos))   
#define DACC_CHSR_DACRDY_Pos                  _U_(8)                                               /**< (DACC_CHSR Position) DAC Ready Flag */
#define DACC_CHSR_DACRDY_Msk                  (_U_(0x3) << DACC_CHSR_DACRDY_Pos)                   /**< (DACC_CHSR Mask) DACRDY */
#define DACC_CHSR_DACRDY(value)               (DACC_CHSR_DACRDY_Msk & ((value) << DACC_CHSR_DACRDY_Pos)) 

/* -------- DACC_CDR : (DACC Offset: 0x1C) ( /W 32) Conversion Data Register 0 -------- */
#define DACC_CDR_DATA0_Pos                    _U_(0)                                               /**< (DACC_CDR) Data to Convert for channel 0 Position */
#define DACC_CDR_DATA0_Msk                    (_U_(0xFFFF) << DACC_CDR_DATA0_Pos)                  /**< (DACC_CDR) Data to Convert for channel 0 Mask */
#define DACC_CDR_DATA0(value)                 (DACC_CDR_DATA0_Msk & ((value) << DACC_CDR_DATA0_Pos))
#define DACC_CDR_DATA1_Pos                    _U_(16)                                              /**< (DACC_CDR) Data to Convert for channel 1 Position */
#define DACC_CDR_DATA1_Msk                    (_U_(0xFFFF) << DACC_CDR_DATA1_Pos)                  /**< (DACC_CDR) Data to Convert for channel 1 Mask */
#define DACC_CDR_DATA1(value)                 (DACC_CDR_DATA1_Msk & ((value) << DACC_CDR_DATA1_Pos))
#define DACC_CDR_Msk                          _U_(0xFFFFFFFF)                                      /**< (DACC_CDR) Register Mask  */


/* -------- DACC_IER : (DACC Offset: 0x24) ( /W 32) Interrupt Enable Register -------- */
#define DACC_IER_TXRDY0_Pos                   _U_(0)                                               /**< (DACC_IER) Transmit Ready Interrupt Enable of channel 0 Position */
#define DACC_IER_TXRDY0_Msk                   (_U_(0x1) << DACC_IER_TXRDY0_Pos)                    /**< (DACC_IER) Transmit Ready Interrupt Enable of channel 0 Mask */
#define DACC_IER_TXRDY0(value)                (DACC_IER_TXRDY0_Msk & ((value) << DACC_IER_TXRDY0_Pos))
#define DACC_IER_TXRDY1_Pos                   _U_(1)                                               /**< (DACC_IER) Transmit Ready Interrupt Enable of channel 1 Position */
#define DACC_IER_TXRDY1_Msk                   (_U_(0x1) << DACC_IER_TXRDY1_Pos)                    /**< (DACC_IER) Transmit Ready Interrupt Enable of channel 1 Mask */
#define DACC_IER_TXRDY1(value)                (DACC_IER_TXRDY1_Msk & ((value) << DACC_IER_TXRDY1_Pos))
#define DACC_IER_EOC0_Pos                     _U_(4)                                               /**< (DACC_IER) End of Conversion Interrupt Enable of channel 0 Position */
#define DACC_IER_EOC0_Msk                     (_U_(0x1) << DACC_IER_EOC0_Pos)                      /**< (DACC_IER) End of Conversion Interrupt Enable of channel 0 Mask */
#define DACC_IER_EOC0(value)                  (DACC_IER_EOC0_Msk & ((value) << DACC_IER_EOC0_Pos))
#define DACC_IER_EOC1_Pos                     _U_(5)                                               /**< (DACC_IER) End of Conversion Interrupt Enable of channel 1 Position */
#define DACC_IER_EOC1_Msk                     (_U_(0x1) << DACC_IER_EOC1_Pos)                      /**< (DACC_IER) End of Conversion Interrupt Enable of channel 1 Mask */
#define DACC_IER_EOC1(value)                  (DACC_IER_EOC1_Msk & ((value) << DACC_IER_EOC1_Pos))
#define DACC_IER_Msk                          _U_(0x00000033)                                      /**< (DACC_IER) Register Mask  */

#define DACC_IER_TXRDY_Pos                    _U_(0)                                               /**< (DACC_IER Position) Transmit Ready Interrupt Enable of channel x */
#define DACC_IER_TXRDY_Msk                    (_U_(0x3) << DACC_IER_TXRDY_Pos)                     /**< (DACC_IER Mask) TXRDY */
#define DACC_IER_TXRDY(value)                 (DACC_IER_TXRDY_Msk & ((value) << DACC_IER_TXRDY_Pos)) 
#define DACC_IER_EOC_Pos                      _U_(4)                                               /**< (DACC_IER Position) End of Conversion Interrupt Enable of channel x */
#define DACC_IER_EOC_Msk                      (_U_(0x3) << DACC_IER_EOC_Pos)                       /**< (DACC_IER Mask) EOC */
#define DACC_IER_EOC(value)                   (DACC_IER_EOC_Msk & ((value) << DACC_IER_EOC_Pos))   

/* -------- DACC_IDR : (DACC Offset: 0x28) ( /W 32) Interrupt Disable Register -------- */
#define DACC_IDR_TXRDY0_Pos                   _U_(0)                                               /**< (DACC_IDR) Transmit Ready Interrupt Disable of channel 0 Position */
#define DACC_IDR_TXRDY0_Msk                   (_U_(0x1) << DACC_IDR_TXRDY0_Pos)                    /**< (DACC_IDR) Transmit Ready Interrupt Disable of channel 0 Mask */
#define DACC_IDR_TXRDY0(value)                (DACC_IDR_TXRDY0_Msk & ((value) << DACC_IDR_TXRDY0_Pos))
#define DACC_IDR_TXRDY1_Pos                   _U_(1)                                               /**< (DACC_IDR) Transmit Ready Interrupt Disable of channel 1 Position */
#define DACC_IDR_TXRDY1_Msk                   (_U_(0x1) << DACC_IDR_TXRDY1_Pos)                    /**< (DACC_IDR) Transmit Ready Interrupt Disable of channel 1 Mask */
#define DACC_IDR_TXRDY1(value)                (DACC_IDR_TXRDY1_Msk & ((value) << DACC_IDR_TXRDY1_Pos))
#define DACC_IDR_EOC0_Pos                     _U_(4)                                               /**< (DACC_IDR) End of Conversion Interrupt Disable of channel 0 Position */
#define DACC_IDR_EOC0_Msk                     (_U_(0x1) << DACC_IDR_EOC0_Pos)                      /**< (DACC_IDR) End of Conversion Interrupt Disable of channel 0 Mask */
#define DACC_IDR_EOC0(value)                  (DACC_IDR_EOC0_Msk & ((value) << DACC_IDR_EOC0_Pos))
#define DACC_IDR_EOC1_Pos                     _U_(5)                                               /**< (DACC_IDR) End of Conversion Interrupt Disable of channel 1 Position */
#define DACC_IDR_EOC1_Msk                     (_U_(0x1) << DACC_IDR_EOC1_Pos)                      /**< (DACC_IDR) End of Conversion Interrupt Disable of channel 1 Mask */
#define DACC_IDR_EOC1(value)                  (DACC_IDR_EOC1_Msk & ((value) << DACC_IDR_EOC1_Pos))
#define DACC_IDR_Msk                          _U_(0x00000033)                                      /**< (DACC_IDR) Register Mask  */

#define DACC_IDR_TXRDY_Pos                    _U_(0)                                               /**< (DACC_IDR Position) Transmit Ready Interrupt Disable of channel x */
#define DACC_IDR_TXRDY_Msk                    (_U_(0x3) << DACC_IDR_TXRDY_Pos)                     /**< (DACC_IDR Mask) TXRDY */
#define DACC_IDR_TXRDY(value)                 (DACC_IDR_TXRDY_Msk & ((value) << DACC_IDR_TXRDY_Pos)) 
#define DACC_IDR_EOC_Pos                      _U_(4)                                               /**< (DACC_IDR Position) End of Conversion Interrupt Disable of channel x */
#define DACC_IDR_EOC_Msk                      (_U_(0x3) << DACC_IDR_EOC_Pos)                       /**< (DACC_IDR Mask) EOC */
#define DACC_IDR_EOC(value)                   (DACC_IDR_EOC_Msk & ((value) << DACC_IDR_EOC_Pos))   

/* -------- DACC_IMR : (DACC Offset: 0x2C) ( R/ 32) Interrupt Mask Register -------- */
#define DACC_IMR_TXRDY0_Pos                   _U_(0)                                               /**< (DACC_IMR) Transmit Ready Interrupt Mask of channel 0 Position */
#define DACC_IMR_TXRDY0_Msk                   (_U_(0x1) << DACC_IMR_TXRDY0_Pos)                    /**< (DACC_IMR) Transmit Ready Interrupt Mask of channel 0 Mask */
#define DACC_IMR_TXRDY0(value)                (DACC_IMR_TXRDY0_Msk & ((value) << DACC_IMR_TXRDY0_Pos))
#define DACC_IMR_TXRDY1_Pos                   _U_(1)                                               /**< (DACC_IMR) Transmit Ready Interrupt Mask of channel 1 Position */
#define DACC_IMR_TXRDY1_Msk                   (_U_(0x1) << DACC_IMR_TXRDY1_Pos)                    /**< (DACC_IMR) Transmit Ready Interrupt Mask of channel 1 Mask */
#define DACC_IMR_TXRDY1(value)                (DACC_IMR_TXRDY1_Msk & ((value) << DACC_IMR_TXRDY1_Pos))
#define DACC_IMR_EOC0_Pos                     _U_(4)                                               /**< (DACC_IMR) End of Conversion Interrupt Mask of channel 0 Position */
#define DACC_IMR_EOC0_Msk                     (_U_(0x1) << DACC_IMR_EOC0_Pos)                      /**< (DACC_IMR) End of Conversion Interrupt Mask of channel 0 Mask */
#define DACC_IMR_EOC0(value)                  (DACC_IMR_EOC0_Msk & ((value) << DACC_IMR_EOC0_Pos))
#define DACC_IMR_EOC1_Pos                     _U_(5)                                               /**< (DACC_IMR) End of Conversion Interrupt Mask of channel 1 Position */
#define DACC_IMR_EOC1_Msk                     (_U_(0x1) << DACC_IMR_EOC1_Pos)                      /**< (DACC_IMR) End of Conversion Interrupt Mask of channel 1 Mask */
#define DACC_IMR_EOC1(value)                  (DACC_IMR_EOC1_Msk & ((value) << DACC_IMR_EOC1_Pos))
#define DACC_IMR_Msk                          _U_(0x00000033)                                      /**< (DACC_IMR) Register Mask  */

#define DACC_IMR_TXRDY_Pos                    _U_(0)                                               /**< (DACC_IMR Position) Transmit Ready Interrupt Mask of channel x */
#define DACC_IMR_TXRDY_Msk                    (_U_(0x3) << DACC_IMR_TXRDY_Pos)                     /**< (DACC_IMR Mask) TXRDY */
#define DACC_IMR_TXRDY(value)                 (DACC_IMR_TXRDY_Msk & ((value) << DACC_IMR_TXRDY_Pos)) 
#define DACC_IMR_EOC_Pos                      _U_(4)                                               /**< (DACC_IMR Position) End of Conversion Interrupt Mask of channel x */
#define DACC_IMR_EOC_Msk                      (_U_(0x3) << DACC_IMR_EOC_Pos)                       /**< (DACC_IMR Mask) EOC */
#define DACC_IMR_EOC(value)                   (DACC_IMR_EOC_Msk & ((value) << DACC_IMR_EOC_Pos))   

/* -------- DACC_ISR : (DACC Offset: 0x30) ( R/ 32) Interrupt Status Register -------- */
#define DACC_ISR_TXRDY0_Pos                   _U_(0)                                               /**< (DACC_ISR) Transmit Ready Interrupt Flag of channel 0 Position */
#define DACC_ISR_TXRDY0_Msk                   (_U_(0x1) << DACC_ISR_TXRDY0_Pos)                    /**< (DACC_ISR) Transmit Ready Interrupt Flag of channel 0 Mask */
#define DACC_ISR_TXRDY0(value)                (DACC_ISR_TXRDY0_Msk & ((value) << DACC_ISR_TXRDY0_Pos))
#define DACC_ISR_TXRDY1_Pos                   _U_(1)                                               /**< (DACC_ISR) Transmit Ready Interrupt Flag of channel 1 Position */
#define DACC_ISR_TXRDY1_Msk                   (_U_(0x1) << DACC_ISR_TXRDY1_Pos)                    /**< (DACC_ISR) Transmit Ready Interrupt Flag of channel 1 Mask */
#define DACC_ISR_TXRDY1(value)                (DACC_ISR_TXRDY1_Msk & ((value) << DACC_ISR_TXRDY1_Pos))
#define DACC_ISR_EOC0_Pos                     _U_(4)                                               /**< (DACC_ISR) End of Conversion Interrupt Flag of channel 0 Position */
#define DACC_ISR_EOC0_Msk                     (_U_(0x1) << DACC_ISR_EOC0_Pos)                      /**< (DACC_ISR) End of Conversion Interrupt Flag of channel 0 Mask */
#define DACC_ISR_EOC0(value)                  (DACC_ISR_EOC0_Msk & ((value) << DACC_ISR_EOC0_Pos))
#define DACC_ISR_EOC1_Pos                     _U_(5)                                               /**< (DACC_ISR) End of Conversion Interrupt Flag of channel 1 Position */
#define DACC_ISR_EOC1_Msk                     (_U_(0x1) << DACC_ISR_EOC1_Pos)                      /**< (DACC_ISR) End of Conversion Interrupt Flag of channel 1 Mask */
#define DACC_ISR_EOC1(value)                  (DACC_ISR_EOC1_Msk & ((value) << DACC_ISR_EOC1_Pos))
#define DACC_ISR_Msk                          _U_(0x00000033)                                      /**< (DACC_ISR) Register Mask  */

#define DACC_ISR_TXRDY_Pos                    _U_(0)                                               /**< (DACC_ISR Position) Transmit Ready Interrupt Flag of channel x */
#define DACC_ISR_TXRDY_Msk                    (_U_(0x3) << DACC_ISR_TXRDY_Pos)                     /**< (DACC_ISR Mask) TXRDY */
#define DACC_ISR_TXRDY(value)                 (DACC_ISR_TXRDY_Msk & ((value) << DACC_ISR_TXRDY_Pos)) 
#define DACC_ISR_EOC_Pos                      _U_(4)                                               /**< (DACC_ISR Position) End of Conversion Interrupt Flag of channel x */
#define DACC_ISR_EOC_Msk                      (_U_(0x3) << DACC_ISR_EOC_Pos)                       /**< (DACC_ISR Mask) EOC */
#define DACC_ISR_EOC(value)                   (DACC_ISR_EOC_Msk & ((value) << DACC_ISR_EOC_Pos))   

/* -------- DACC_ACR : (DACC Offset: 0x94) (R/W 32) Analog Current Register -------- */
#define DACC_ACR_IBCTLCH0_Pos                 _U_(0)                                               /**< (DACC_ACR) Analog Output Current Control Position */
#define DACC_ACR_IBCTLCH0_Msk                 (_U_(0x3) << DACC_ACR_IBCTLCH0_Pos)                  /**< (DACC_ACR) Analog Output Current Control Mask */
#define DACC_ACR_IBCTLCH0(value)              (DACC_ACR_IBCTLCH0_Msk & ((value) << DACC_ACR_IBCTLCH0_Pos))
#define DACC_ACR_IBCTLCH1_Pos                 _U_(2)                                               /**< (DACC_ACR) Analog Output Current Control Position */
#define DACC_ACR_IBCTLCH1_Msk                 (_U_(0x3) << DACC_ACR_IBCTLCH1_Pos)                  /**< (DACC_ACR) Analog Output Current Control Mask */
#define DACC_ACR_IBCTLCH1(value)              (DACC_ACR_IBCTLCH1_Msk & ((value) << DACC_ACR_IBCTLCH1_Pos))
#define DACC_ACR_Msk                          _U_(0x0000000F)                                      /**< (DACC_ACR) Register Mask  */


/* -------- DACC_WPMR : (DACC Offset: 0xE4) (R/W 32) Write Protection Mode Register -------- */
#define DACC_WPMR_WPEN_Pos                    _U_(0)                                               /**< (DACC_WPMR) Write Protection Enable Position */
#define DACC_WPMR_WPEN_Msk                    (_U_(0x1) << DACC_WPMR_WPEN_Pos)                     /**< (DACC_WPMR) Write Protection Enable Mask */
#define DACC_WPMR_WPEN(value)                 (DACC_WPMR_WPEN_Msk & ((value) << DACC_WPMR_WPEN_Pos))
#define DACC_WPMR_WPKEY_Pos                   _U_(8)                                               /**< (DACC_WPMR) Write Protect Key Position */
#define DACC_WPMR_WPKEY_Msk                   (_U_(0xFFFFFF) << DACC_WPMR_WPKEY_Pos)               /**< (DACC_WPMR) Write Protect Key Mask */
#define DACC_WPMR_WPKEY(value)                (DACC_WPMR_WPKEY_Msk & ((value) << DACC_WPMR_WPKEY_Pos))
#define   DACC_WPMR_WPKEY_PASSWD_Val          _U_(0x444143)                                        /**< (DACC_WPMR) Writing any other value in this field aborts the write operation of bit WPEN.Always reads as 0.  */
#define DACC_WPMR_WPKEY_PASSWD                (DACC_WPMR_WPKEY_PASSWD_Val << DACC_WPMR_WPKEY_Pos)  /**< (DACC_WPMR) Writing any other value in this field aborts the write operation of bit WPEN.Always reads as 0. Position  */
#define DACC_WPMR_Msk                         _U_(0xFFFFFF01)                                      /**< (DACC_WPMR) Register Mask  */


/* -------- DACC_WPSR : (DACC Offset: 0xE8) ( R/ 32) Write Protection Status Register -------- */
#define DACC_WPSR_WPVS_Pos                    _U_(0)                                               /**< (DACC_WPSR) Write Protection Violation Status Position */
#define DACC_WPSR_WPVS_Msk                    (_U_(0x1) << DACC_WPSR_WPVS_Pos)                     /**< (DACC_WPSR) Write Protection Violation Status Mask */
#define DACC_WPSR_WPVS(value)                 (DACC_WPSR_WPVS_Msk & ((value) << DACC_WPSR_WPVS_Pos))
#define DACC_WPSR_WPVSRC_Pos                  _U_(8)                                               /**< (DACC_WPSR) Write Protection Violation Source Position */
#define DACC_WPSR_WPVSRC_Msk                  (_U_(0xFF) << DACC_WPSR_WPVSRC_Pos)                  /**< (DACC_WPSR) Write Protection Violation Source Mask */
#define DACC_WPSR_WPVSRC(value)               (DACC_WPSR_WPVSRC_Msk & ((value) << DACC_WPSR_WPVSRC_Pos))
#define DACC_WPSR_Msk                         _U_(0x0000FF01)                                      /**< (DACC_WPSR) Register Mask  */


/** \brief DACC register offsets definitions */
#define DACC_CR_REG_OFST               (0x00)              /**< (DACC_CR) Control Register Offset */
#define DACC_MR_REG_OFST               (0x04)              /**< (DACC_MR) Mode Register Offset */
#define DACC_TRIGR_REG_OFST            (0x08)              /**< (DACC_TRIGR) Trigger Register Offset */
#define DACC_CHER_REG_OFST             (0x10)              /**< (DACC_CHER) Channel Enable Register Offset */
#define DACC_CHDR_REG_OFST             (0x14)              /**< (DACC_CHDR) Channel Disable Register Offset */
#define DACC_CHSR_REG_OFST             (0x18)              /**< (DACC_CHSR) Channel Status Register Offset */
#define DACC_CDR_REG_OFST              (0x1C)              /**< (DACC_CDR) Conversion Data Register 0 Offset */
#define DACC_IER_REG_OFST              (0x24)              /**< (DACC_IER) Interrupt Enable Register Offset */
#define DACC_IDR_REG_OFST              (0x28)              /**< (DACC_IDR) Interrupt Disable Register Offset */
#define DACC_IMR_REG_OFST              (0x2C)              /**< (DACC_IMR) Interrupt Mask Register Offset */
#define DACC_ISR_REG_OFST              (0x30)              /**< (DACC_ISR) Interrupt Status Register Offset */
#define DACC_ACR_REG_OFST              (0x94)              /**< (DACC_ACR) Analog Current Register Offset */
#define DACC_WPMR_REG_OFST             (0xE4)              /**< (DACC_WPMR) Write Protection Mode Register Offset */
#define DACC_WPSR_REG_OFST             (0xE8)              /**< (DACC_WPSR) Write Protection Status Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief DACC register API structure */
typedef struct
{
  __O   uint32_t                       DACC_CR;            /**< Offset: 0x00 ( /W  32) Control Register */
  __IO  uint32_t                       DACC_MR;            /**< Offset: 0x04 (R/W  32) Mode Register */
  __IO  uint32_t                       DACC_TRIGR;         /**< Offset: 0x08 (R/W  32) Trigger Register */
  __I   uint8_t                        Reserved1[0x04];
  __O   uint32_t                       DACC_CHER;          /**< Offset: 0x10 ( /W  32) Channel Enable Register */
  __O   uint32_t                       DACC_CHDR;          /**< Offset: 0x14 ( /W  32) Channel Disable Register */
  __I   uint32_t                       DACC_CHSR;          /**< Offset: 0x18 (R/   32) Channel Status Register */
  __O   uint32_t                       DACC_CDR[2];        /**< Offset: 0x1C ( /W  32) Conversion Data Register 0 */
  __O   uint32_t                       DACC_IER;           /**< Offset: 0x24 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       DACC_IDR;           /**< Offset: 0x28 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       DACC_IMR;           /**< Offset: 0x2C (R/   32) Interrupt Mask Register */
  __I   uint32_t                       DACC_ISR;           /**< Offset: 0x30 (R/   32) Interrupt Status Register */
  __I   uint8_t                        Reserved2[0x60];
  __IO  uint32_t                       DACC_ACR;           /**< Offset: 0x94 (R/W  32) Analog Current Register */
  __I   uint8_t                        Reserved3[0x4C];
  __IO  uint32_t                       DACC_WPMR;          /**< Offset: 0xE4 (R/W  32) Write Protection Mode Register */
  __I   uint32_t                       DACC_WPSR;          /**< Offset: 0xE8 (R/   32) Write Protection Status Register */
} dacc_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_DACC_COMPONENT_H_ */
