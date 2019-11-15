/**
 * \brief Component description for AFEC
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
#ifndef _SAME70_AFEC_COMPONENT_H_
#define _SAME70_AFEC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR AFEC                                         */
/* ************************************************************************** */

/* -------- AFEC_CR : (AFEC Offset: 0x00) ( /W 32) AFEC Control Register -------- */
#define AFEC_CR_SWRST_Pos                     _U_(0)                                               /**< (AFEC_CR) Software Reset Position */
#define AFEC_CR_SWRST_Msk                     (_U_(0x1) << AFEC_CR_SWRST_Pos)                      /**< (AFEC_CR) Software Reset Mask */
#define AFEC_CR_SWRST(value)                  (AFEC_CR_SWRST_Msk & ((value) << AFEC_CR_SWRST_Pos))
#define AFEC_CR_START_Pos                     _U_(1)                                               /**< (AFEC_CR) Start Conversion Position */
#define AFEC_CR_START_Msk                     (_U_(0x1) << AFEC_CR_START_Pos)                      /**< (AFEC_CR) Start Conversion Mask */
#define AFEC_CR_START(value)                  (AFEC_CR_START_Msk & ((value) << AFEC_CR_START_Pos))
#define AFEC_CR_Msk                           _U_(0x00000003)                                      /**< (AFEC_CR) Register Mask  */


/* -------- AFEC_MR : (AFEC Offset: 0x04) (R/W 32) AFEC Mode Register -------- */
#define AFEC_MR_TRGEN_Pos                     _U_(0)                                               /**< (AFEC_MR) Trigger Enable Position */
#define AFEC_MR_TRGEN_Msk                     (_U_(0x1) << AFEC_MR_TRGEN_Pos)                      /**< (AFEC_MR) Trigger Enable Mask */
#define AFEC_MR_TRGEN(value)                  (AFEC_MR_TRGEN_Msk & ((value) << AFEC_MR_TRGEN_Pos))
#define   AFEC_MR_TRGEN_DIS_Val               _U_(0x0)                                             /**< (AFEC_MR) Hardware triggers are disabled. Starting a conversion is only possible by software.  */
#define   AFEC_MR_TRGEN_EN_Val                _U_(0x1)                                             /**< (AFEC_MR) Hardware trigger selected by TRGSEL field is enabled.  */
#define AFEC_MR_TRGEN_DIS                     (AFEC_MR_TRGEN_DIS_Val << AFEC_MR_TRGEN_Pos)         /**< (AFEC_MR) Hardware triggers are disabled. Starting a conversion is only possible by software. Position  */
#define AFEC_MR_TRGEN_EN                      (AFEC_MR_TRGEN_EN_Val << AFEC_MR_TRGEN_Pos)          /**< (AFEC_MR) Hardware trigger selected by TRGSEL field is enabled. Position  */
#define AFEC_MR_TRGSEL_Pos                    _U_(1)                                               /**< (AFEC_MR) Trigger Selection Position */
#define AFEC_MR_TRGSEL_Msk                    (_U_(0x7) << AFEC_MR_TRGSEL_Pos)                     /**< (AFEC_MR) Trigger Selection Mask */
#define AFEC_MR_TRGSEL(value)                 (AFEC_MR_TRGSEL_Msk & ((value) << AFEC_MR_TRGSEL_Pos))
#define   AFEC_MR_TRGSEL_AFEC_TRIG0_Val       _U_(0x0)                                             /**< (AFEC_MR) AFE0_ADTRG for AFEC0 / AFE1_ADTRG for AFEC1  */
#define   AFEC_MR_TRGSEL_AFEC_TRIG1_Val       _U_(0x1)                                             /**< (AFEC_MR) TIOA Output of the Timer Counter Channel 0 for AFEC0/TIOA Output of the Timer Counter Channel 3 for AFEC1  */
#define   AFEC_MR_TRGSEL_AFEC_TRIG2_Val       _U_(0x2)                                             /**< (AFEC_MR) TIOA Output of the Timer Counter Channel 1 for AFEC0/TIOA Output of the Timer Counter Channel 4 for AFEC1  */
#define   AFEC_MR_TRGSEL_AFEC_TRIG3_Val       _U_(0x3)                                             /**< (AFEC_MR) TIOA Output of the Timer Counter Channel 2 for AFEC0/TIOA Output of the Timer Counter Channel 5 for AFEC1  */
#define   AFEC_MR_TRGSEL_AFEC_TRIG4_Val       _U_(0x4)                                             /**< (AFEC_MR) PWM0 event line 0 for AFEC0 / PWM1 event line 0 for AFEC1  */
#define   AFEC_MR_TRGSEL_AFEC_TRIG5_Val       _U_(0x5)                                             /**< (AFEC_MR) PWM0 event line 1 for AFEC0 / PWM1 event line 1 for AFEC1  */
#define   AFEC_MR_TRGSEL_AFEC_TRIG6_Val       _U_(0x6)                                             /**< (AFEC_MR) Analog Comparator  */
#define AFEC_MR_TRGSEL_AFEC_TRIG0             (AFEC_MR_TRGSEL_AFEC_TRIG0_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) AFE0_ADTRG for AFEC0 / AFE1_ADTRG for AFEC1 Position  */
#define AFEC_MR_TRGSEL_AFEC_TRIG1             (AFEC_MR_TRGSEL_AFEC_TRIG1_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) TIOA Output of the Timer Counter Channel 0 for AFEC0/TIOA Output of the Timer Counter Channel 3 for AFEC1 Position  */
#define AFEC_MR_TRGSEL_AFEC_TRIG2             (AFEC_MR_TRGSEL_AFEC_TRIG2_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) TIOA Output of the Timer Counter Channel 1 for AFEC0/TIOA Output of the Timer Counter Channel 4 for AFEC1 Position  */
#define AFEC_MR_TRGSEL_AFEC_TRIG3             (AFEC_MR_TRGSEL_AFEC_TRIG3_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) TIOA Output of the Timer Counter Channel 2 for AFEC0/TIOA Output of the Timer Counter Channel 5 for AFEC1 Position  */
#define AFEC_MR_TRGSEL_AFEC_TRIG4             (AFEC_MR_TRGSEL_AFEC_TRIG4_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) PWM0 event line 0 for AFEC0 / PWM1 event line 0 for AFEC1 Position  */
#define AFEC_MR_TRGSEL_AFEC_TRIG5             (AFEC_MR_TRGSEL_AFEC_TRIG5_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) PWM0 event line 1 for AFEC0 / PWM1 event line 1 for AFEC1 Position  */
#define AFEC_MR_TRGSEL_AFEC_TRIG6             (AFEC_MR_TRGSEL_AFEC_TRIG6_Val << AFEC_MR_TRGSEL_Pos) /**< (AFEC_MR) Analog Comparator Position  */
#define AFEC_MR_SLEEP_Pos                     _U_(5)                                               /**< (AFEC_MR) Sleep Mode Position */
#define AFEC_MR_SLEEP_Msk                     (_U_(0x1) << AFEC_MR_SLEEP_Pos)                      /**< (AFEC_MR) Sleep Mode Mask */
#define AFEC_MR_SLEEP(value)                  (AFEC_MR_SLEEP_Msk & ((value) << AFEC_MR_SLEEP_Pos))
#define   AFEC_MR_SLEEP_NORMAL_Val            _U_(0x0)                                             /**< (AFEC_MR) Normal mode: The AFE and reference voltage circuitry are kept ON between conversions.  */
#define   AFEC_MR_SLEEP_SLEEP_Val             _U_(0x1)                                             /**< (AFEC_MR) Sleep mode: The AFE and reference voltage circuitry are OFF between conversions.  */
#define AFEC_MR_SLEEP_NORMAL                  (AFEC_MR_SLEEP_NORMAL_Val << AFEC_MR_SLEEP_Pos)      /**< (AFEC_MR) Normal mode: The AFE and reference voltage circuitry are kept ON between conversions. Position  */
#define AFEC_MR_SLEEP_SLEEP                   (AFEC_MR_SLEEP_SLEEP_Val << AFEC_MR_SLEEP_Pos)       /**< (AFEC_MR) Sleep mode: The AFE and reference voltage circuitry are OFF between conversions. Position  */
#define AFEC_MR_FWUP_Pos                      _U_(6)                                               /**< (AFEC_MR) Fast Wake-up Position */
#define AFEC_MR_FWUP_Msk                      (_U_(0x1) << AFEC_MR_FWUP_Pos)                       /**< (AFEC_MR) Fast Wake-up Mask */
#define AFEC_MR_FWUP(value)                   (AFEC_MR_FWUP_Msk & ((value) << AFEC_MR_FWUP_Pos))  
#define   AFEC_MR_FWUP_OFF_Val                _U_(0x0)                                             /**< (AFEC_MR) Normal Sleep mode: The sleep mode is defined by the SLEEP bit.  */
#define   AFEC_MR_FWUP_ON_Val                 _U_(0x1)                                             /**< (AFEC_MR) Fast wake-up Sleep mode: The voltage reference is ON between conversions and AFE is OFF.  */
#define AFEC_MR_FWUP_OFF                      (AFEC_MR_FWUP_OFF_Val << AFEC_MR_FWUP_Pos)           /**< (AFEC_MR) Normal Sleep mode: The sleep mode is defined by the SLEEP bit. Position  */
#define AFEC_MR_FWUP_ON                       (AFEC_MR_FWUP_ON_Val << AFEC_MR_FWUP_Pos)            /**< (AFEC_MR) Fast wake-up Sleep mode: The voltage reference is ON between conversions and AFE is OFF. Position  */
#define AFEC_MR_FREERUN_Pos                   _U_(7)                                               /**< (AFEC_MR) Free Run Mode Position */
#define AFEC_MR_FREERUN_Msk                   (_U_(0x1) << AFEC_MR_FREERUN_Pos)                    /**< (AFEC_MR) Free Run Mode Mask */
#define AFEC_MR_FREERUN(value)                (AFEC_MR_FREERUN_Msk & ((value) << AFEC_MR_FREERUN_Pos))
#define   AFEC_MR_FREERUN_OFF_Val             _U_(0x0)                                             /**< (AFEC_MR) Normal mode  */
#define   AFEC_MR_FREERUN_ON_Val              _U_(0x1)                                             /**< (AFEC_MR) Free Run mode: Never wait for any trigger.  */
#define AFEC_MR_FREERUN_OFF                   (AFEC_MR_FREERUN_OFF_Val << AFEC_MR_FREERUN_Pos)     /**< (AFEC_MR) Normal mode Position  */
#define AFEC_MR_FREERUN_ON                    (AFEC_MR_FREERUN_ON_Val << AFEC_MR_FREERUN_Pos)      /**< (AFEC_MR) Free Run mode: Never wait for any trigger. Position  */
#define AFEC_MR_PRESCAL_Pos                   _U_(8)                                               /**< (AFEC_MR) Prescaler Rate Selection Position */
#define AFEC_MR_PRESCAL_Msk                   (_U_(0xFF) << AFEC_MR_PRESCAL_Pos)                   /**< (AFEC_MR) Prescaler Rate Selection Mask */
#define AFEC_MR_PRESCAL(value)                (AFEC_MR_PRESCAL_Msk & ((value) << AFEC_MR_PRESCAL_Pos))
#define AFEC_MR_STARTUP_Pos                   _U_(16)                                              /**< (AFEC_MR) Start-up Time Position */
#define AFEC_MR_STARTUP_Msk                   (_U_(0xF) << AFEC_MR_STARTUP_Pos)                    /**< (AFEC_MR) Start-up Time Mask */
#define AFEC_MR_STARTUP(value)                (AFEC_MR_STARTUP_Msk & ((value) << AFEC_MR_STARTUP_Pos))
#define   AFEC_MR_STARTUP_SUT0_Val            _U_(0x0)                                             /**< (AFEC_MR) 0 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT8_Val            _U_(0x1)                                             /**< (AFEC_MR) 8 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT16_Val           _U_(0x2)                                             /**< (AFEC_MR) 16 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT24_Val           _U_(0x3)                                             /**< (AFEC_MR) 24 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT64_Val           _U_(0x4)                                             /**< (AFEC_MR) 64 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT80_Val           _U_(0x5)                                             /**< (AFEC_MR) 80 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT96_Val           _U_(0x6)                                             /**< (AFEC_MR) 96 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT112_Val          _U_(0x7)                                             /**< (AFEC_MR) 112 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT512_Val          _U_(0x8)                                             /**< (AFEC_MR) 512 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT576_Val          _U_(0x9)                                             /**< (AFEC_MR) 576 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT640_Val          _U_(0xA)                                             /**< (AFEC_MR) 640 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT704_Val          _U_(0xB)                                             /**< (AFEC_MR) 704 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT768_Val          _U_(0xC)                                             /**< (AFEC_MR) 768 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT832_Val          _U_(0xD)                                             /**< (AFEC_MR) 832 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT896_Val          _U_(0xE)                                             /**< (AFEC_MR) 896 periods of AFE clock  */
#define   AFEC_MR_STARTUP_SUT960_Val          _U_(0xF)                                             /**< (AFEC_MR) 960 periods of AFE clock  */
#define AFEC_MR_STARTUP_SUT0                  (AFEC_MR_STARTUP_SUT0_Val << AFEC_MR_STARTUP_Pos)    /**< (AFEC_MR) 0 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT8                  (AFEC_MR_STARTUP_SUT8_Val << AFEC_MR_STARTUP_Pos)    /**< (AFEC_MR) 8 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT16                 (AFEC_MR_STARTUP_SUT16_Val << AFEC_MR_STARTUP_Pos)   /**< (AFEC_MR) 16 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT24                 (AFEC_MR_STARTUP_SUT24_Val << AFEC_MR_STARTUP_Pos)   /**< (AFEC_MR) 24 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT64                 (AFEC_MR_STARTUP_SUT64_Val << AFEC_MR_STARTUP_Pos)   /**< (AFEC_MR) 64 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT80                 (AFEC_MR_STARTUP_SUT80_Val << AFEC_MR_STARTUP_Pos)   /**< (AFEC_MR) 80 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT96                 (AFEC_MR_STARTUP_SUT96_Val << AFEC_MR_STARTUP_Pos)   /**< (AFEC_MR) 96 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT112                (AFEC_MR_STARTUP_SUT112_Val << AFEC_MR_STARTUP_Pos)  /**< (AFEC_MR) 112 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT512                (AFEC_MR_STARTUP_SUT512_Val << AFEC_MR_STARTUP_Pos)  /**< (AFEC_MR) 512 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT576                (AFEC_MR_STARTUP_SUT576_Val << AFEC_MR_STARTUP_Pos)  /**< (AFEC_MR) 576 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT640                (AFEC_MR_STARTUP_SUT640_Val << AFEC_MR_STARTUP_Pos)  /**< (AFEC_MR) 640 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT704                (AFEC_MR_STARTUP_SUT704_Val << AFEC_MR_STARTUP_Pos)  /**< (AFEC_MR) 704 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT768                (AFEC_MR_STARTUP_SUT768_Val << AFEC_MR_STARTUP_Pos)  /**< (AFEC_MR) 768 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT832                (AFEC_MR_STARTUP_SUT832_Val << AFEC_MR_STARTUP_Pos)  /**< (AFEC_MR) 832 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT896                (AFEC_MR_STARTUP_SUT896_Val << AFEC_MR_STARTUP_Pos)  /**< (AFEC_MR) 896 periods of AFE clock Position  */
#define AFEC_MR_STARTUP_SUT960                (AFEC_MR_STARTUP_SUT960_Val << AFEC_MR_STARTUP_Pos)  /**< (AFEC_MR) 960 periods of AFE clock Position  */
#define AFEC_MR_ONE_Pos                       _U_(23)                                              /**< (AFEC_MR) One Position */
#define AFEC_MR_ONE_Msk                       (_U_(0x1) << AFEC_MR_ONE_Pos)                        /**< (AFEC_MR) One Mask */
#define AFEC_MR_ONE(value)                    (AFEC_MR_ONE_Msk & ((value) << AFEC_MR_ONE_Pos))    
#define AFEC_MR_TRACKTIM_Pos                  _U_(24)                                              /**< (AFEC_MR) Tracking Time Position */
#define AFEC_MR_TRACKTIM_Msk                  (_U_(0xF) << AFEC_MR_TRACKTIM_Pos)                   /**< (AFEC_MR) Tracking Time Mask */
#define AFEC_MR_TRACKTIM(value)               (AFEC_MR_TRACKTIM_Msk & ((value) << AFEC_MR_TRACKTIM_Pos))
#define AFEC_MR_TRANSFER_Pos                  _U_(28)                                              /**< (AFEC_MR) Transfer Period Position */
#define AFEC_MR_TRANSFER_Msk                  (_U_(0x3) << AFEC_MR_TRANSFER_Pos)                   /**< (AFEC_MR) Transfer Period Mask */
#define AFEC_MR_TRANSFER(value)               (AFEC_MR_TRANSFER_Msk & ((value) << AFEC_MR_TRANSFER_Pos))
#define AFEC_MR_USEQ_Pos                      _U_(31)                                              /**< (AFEC_MR) User Sequence Enable Position */
#define AFEC_MR_USEQ_Msk                      (_U_(0x1) << AFEC_MR_USEQ_Pos)                       /**< (AFEC_MR) User Sequence Enable Mask */
#define AFEC_MR_USEQ(value)                   (AFEC_MR_USEQ_Msk & ((value) << AFEC_MR_USEQ_Pos))  
#define   AFEC_MR_USEQ_NUM_ORDER_Val          _U_(0x0)                                             /**< (AFEC_MR) Normal mode: The controller converts channels in a simple numeric order.  */
#define   AFEC_MR_USEQ_REG_ORDER_Val          _U_(0x1)                                             /**< (AFEC_MR) User Sequence mode: The sequence respects what is defined in AFEC_SEQ1R and AFEC_SEQ1R.  */
#define AFEC_MR_USEQ_NUM_ORDER                (AFEC_MR_USEQ_NUM_ORDER_Val << AFEC_MR_USEQ_Pos)     /**< (AFEC_MR) Normal mode: The controller converts channels in a simple numeric order. Position  */
#define AFEC_MR_USEQ_REG_ORDER                (AFEC_MR_USEQ_REG_ORDER_Val << AFEC_MR_USEQ_Pos)     /**< (AFEC_MR) User Sequence mode: The sequence respects what is defined in AFEC_SEQ1R and AFEC_SEQ1R. Position  */
#define AFEC_MR_Msk                           _U_(0xBF8FFFEF)                                      /**< (AFEC_MR) Register Mask  */


/* -------- AFEC_EMR : (AFEC Offset: 0x08) (R/W 32) AFEC Extended Mode Register -------- */
#define AFEC_EMR_CMPMODE_Pos                  _U_(0)                                               /**< (AFEC_EMR) Comparison Mode Position */
#define AFEC_EMR_CMPMODE_Msk                  (_U_(0x3) << AFEC_EMR_CMPMODE_Pos)                   /**< (AFEC_EMR) Comparison Mode Mask */
#define AFEC_EMR_CMPMODE(value)               (AFEC_EMR_CMPMODE_Msk & ((value) << AFEC_EMR_CMPMODE_Pos))
#define   AFEC_EMR_CMPMODE_LOW_Val            _U_(0x0)                                             /**< (AFEC_EMR) Generates an event when the converted data is lower than the low threshold of the window.  */
#define   AFEC_EMR_CMPMODE_HIGH_Val           _U_(0x1)                                             /**< (AFEC_EMR) Generates an event when the converted data is higher than the high threshold of the window.  */
#define   AFEC_EMR_CMPMODE_IN_Val             _U_(0x2)                                             /**< (AFEC_EMR) Generates an event when the converted data is in the comparison window.  */
#define   AFEC_EMR_CMPMODE_OUT_Val            _U_(0x3)                                             /**< (AFEC_EMR) Generates an event when the converted data is out of the comparison window.  */
#define AFEC_EMR_CMPMODE_LOW                  (AFEC_EMR_CMPMODE_LOW_Val << AFEC_EMR_CMPMODE_Pos)   /**< (AFEC_EMR) Generates an event when the converted data is lower than the low threshold of the window. Position  */
#define AFEC_EMR_CMPMODE_HIGH                 (AFEC_EMR_CMPMODE_HIGH_Val << AFEC_EMR_CMPMODE_Pos)  /**< (AFEC_EMR) Generates an event when the converted data is higher than the high threshold of the window. Position  */
#define AFEC_EMR_CMPMODE_IN                   (AFEC_EMR_CMPMODE_IN_Val << AFEC_EMR_CMPMODE_Pos)    /**< (AFEC_EMR) Generates an event when the converted data is in the comparison window. Position  */
#define AFEC_EMR_CMPMODE_OUT                  (AFEC_EMR_CMPMODE_OUT_Val << AFEC_EMR_CMPMODE_Pos)   /**< (AFEC_EMR) Generates an event when the converted data is out of the comparison window. Position  */
#define AFEC_EMR_CMPSEL_Pos                   _U_(3)                                               /**< (AFEC_EMR) Comparison Selected Channel Position */
#define AFEC_EMR_CMPSEL_Msk                   (_U_(0x1F) << AFEC_EMR_CMPSEL_Pos)                   /**< (AFEC_EMR) Comparison Selected Channel Mask */
#define AFEC_EMR_CMPSEL(value)                (AFEC_EMR_CMPSEL_Msk & ((value) << AFEC_EMR_CMPSEL_Pos))
#define AFEC_EMR_CMPALL_Pos                   _U_(9)                                               /**< (AFEC_EMR) Compare All Channels Position */
#define AFEC_EMR_CMPALL_Msk                   (_U_(0x1) << AFEC_EMR_CMPALL_Pos)                    /**< (AFEC_EMR) Compare All Channels Mask */
#define AFEC_EMR_CMPALL(value)                (AFEC_EMR_CMPALL_Msk & ((value) << AFEC_EMR_CMPALL_Pos))
#define AFEC_EMR_CMPFILTER_Pos                _U_(12)                                              /**< (AFEC_EMR) Compare Event Filtering Position */
#define AFEC_EMR_CMPFILTER_Msk                (_U_(0x3) << AFEC_EMR_CMPFILTER_Pos)                 /**< (AFEC_EMR) Compare Event Filtering Mask */
#define AFEC_EMR_CMPFILTER(value)             (AFEC_EMR_CMPFILTER_Msk & ((value) << AFEC_EMR_CMPFILTER_Pos))
#define AFEC_EMR_RES_Pos                      _U_(16)                                              /**< (AFEC_EMR) Resolution Position */
#define AFEC_EMR_RES_Msk                      (_U_(0x7) << AFEC_EMR_RES_Pos)                       /**< (AFEC_EMR) Resolution Mask */
#define AFEC_EMR_RES(value)                   (AFEC_EMR_RES_Msk & ((value) << AFEC_EMR_RES_Pos))  
#define   AFEC_EMR_RES_NO_AVERAGE_Val         _U_(0x0)                                             /**< (AFEC_EMR) 12-bit resolution, AFE sample rate is maximum (no averaging).  */
#define   AFEC_EMR_RES_OSR4_Val               _U_(0x2)                                             /**< (AFEC_EMR) 13-bit resolution, AFE sample rate divided by 4 (averaging).  */
#define   AFEC_EMR_RES_OSR16_Val              _U_(0x3)                                             /**< (AFEC_EMR) 14-bit resolution, AFE sample rate divided by 16 (averaging).  */
#define   AFEC_EMR_RES_OSR64_Val              _U_(0x4)                                             /**< (AFEC_EMR) 15-bit resolution, AFE sample rate divided by 64 (averaging).  */
#define   AFEC_EMR_RES_OSR256_Val             _U_(0x5)                                             /**< (AFEC_EMR) 16-bit resolution, AFE sample rate divided by 256 (averaging).  */
#define AFEC_EMR_RES_NO_AVERAGE               (AFEC_EMR_RES_NO_AVERAGE_Val << AFEC_EMR_RES_Pos)    /**< (AFEC_EMR) 12-bit resolution, AFE sample rate is maximum (no averaging). Position  */
#define AFEC_EMR_RES_OSR4                     (AFEC_EMR_RES_OSR4_Val << AFEC_EMR_RES_Pos)          /**< (AFEC_EMR) 13-bit resolution, AFE sample rate divided by 4 (averaging). Position  */
#define AFEC_EMR_RES_OSR16                    (AFEC_EMR_RES_OSR16_Val << AFEC_EMR_RES_Pos)         /**< (AFEC_EMR) 14-bit resolution, AFE sample rate divided by 16 (averaging). Position  */
#define AFEC_EMR_RES_OSR64                    (AFEC_EMR_RES_OSR64_Val << AFEC_EMR_RES_Pos)         /**< (AFEC_EMR) 15-bit resolution, AFE sample rate divided by 64 (averaging). Position  */
#define AFEC_EMR_RES_OSR256                   (AFEC_EMR_RES_OSR256_Val << AFEC_EMR_RES_Pos)        /**< (AFEC_EMR) 16-bit resolution, AFE sample rate divided by 256 (averaging). Position  */
#define AFEC_EMR_TAG_Pos                      _U_(24)                                              /**< (AFEC_EMR) TAG of the AFEC_LDCR Position */
#define AFEC_EMR_TAG_Msk                      (_U_(0x1) << AFEC_EMR_TAG_Pos)                       /**< (AFEC_EMR) TAG of the AFEC_LDCR Mask */
#define AFEC_EMR_TAG(value)                   (AFEC_EMR_TAG_Msk & ((value) << AFEC_EMR_TAG_Pos))  
#define AFEC_EMR_STM_Pos                      _U_(25)                                              /**< (AFEC_EMR) Single Trigger Mode Position */
#define AFEC_EMR_STM_Msk                      (_U_(0x1) << AFEC_EMR_STM_Pos)                       /**< (AFEC_EMR) Single Trigger Mode Mask */
#define AFEC_EMR_STM(value)                   (AFEC_EMR_STM_Msk & ((value) << AFEC_EMR_STM_Pos))  
#define AFEC_EMR_SIGNMODE_Pos                 _U_(28)                                              /**< (AFEC_EMR) Sign Mode Position */
#define AFEC_EMR_SIGNMODE_Msk                 (_U_(0x3) << AFEC_EMR_SIGNMODE_Pos)                  /**< (AFEC_EMR) Sign Mode Mask */
#define AFEC_EMR_SIGNMODE(value)              (AFEC_EMR_SIGNMODE_Msk & ((value) << AFEC_EMR_SIGNMODE_Pos))
#define   AFEC_EMR_SIGNMODE_SE_UNSG_DF_SIGN_Val _U_(0x0)                                             /**< (AFEC_EMR) Single-Ended channels: Unsigned conversions.Differential channels: Signed conversions.  */
#define   AFEC_EMR_SIGNMODE_SE_SIGN_DF_UNSG_Val _U_(0x1)                                             /**< (AFEC_EMR) Single-Ended channels: Signed conversions.Differential channels: Unsigned conversions.  */
#define   AFEC_EMR_SIGNMODE_ALL_UNSIGNED_Val  _U_(0x2)                                             /**< (AFEC_EMR) All channels: Unsigned conversions.  */
#define   AFEC_EMR_SIGNMODE_ALL_SIGNED_Val    _U_(0x3)                                             /**< (AFEC_EMR) All channels: Signed conversions.  */
#define AFEC_EMR_SIGNMODE_SE_UNSG_DF_SIGN     (AFEC_EMR_SIGNMODE_SE_UNSG_DF_SIGN_Val << AFEC_EMR_SIGNMODE_Pos) /**< (AFEC_EMR) Single-Ended channels: Unsigned conversions.Differential channels: Signed conversions. Position  */
#define AFEC_EMR_SIGNMODE_SE_SIGN_DF_UNSG     (AFEC_EMR_SIGNMODE_SE_SIGN_DF_UNSG_Val << AFEC_EMR_SIGNMODE_Pos) /**< (AFEC_EMR) Single-Ended channels: Signed conversions.Differential channels: Unsigned conversions. Position  */
#define AFEC_EMR_SIGNMODE_ALL_UNSIGNED        (AFEC_EMR_SIGNMODE_ALL_UNSIGNED_Val << AFEC_EMR_SIGNMODE_Pos) /**< (AFEC_EMR) All channels: Unsigned conversions. Position  */
#define AFEC_EMR_SIGNMODE_ALL_SIGNED          (AFEC_EMR_SIGNMODE_ALL_SIGNED_Val << AFEC_EMR_SIGNMODE_Pos) /**< (AFEC_EMR) All channels: Signed conversions. Position  */
#define AFEC_EMR_Msk                          _U_(0x330732FB)                                      /**< (AFEC_EMR) Register Mask  */


/* -------- AFEC_SEQ1R : (AFEC Offset: 0x0C) (R/W 32) AFEC Channel Sequence 1 Register -------- */
#define AFEC_SEQ1R_USCH0_Pos                  _U_(0)                                               /**< (AFEC_SEQ1R) User Sequence Number 0 Position */
#define AFEC_SEQ1R_USCH0_Msk                  (_U_(0xF) << AFEC_SEQ1R_USCH0_Pos)                   /**< (AFEC_SEQ1R) User Sequence Number 0 Mask */
#define AFEC_SEQ1R_USCH0(value)               (AFEC_SEQ1R_USCH0_Msk & ((value) << AFEC_SEQ1R_USCH0_Pos))
#define AFEC_SEQ1R_USCH1_Pos                  _U_(4)                                               /**< (AFEC_SEQ1R) User Sequence Number 1 Position */
#define AFEC_SEQ1R_USCH1_Msk                  (_U_(0xF) << AFEC_SEQ1R_USCH1_Pos)                   /**< (AFEC_SEQ1R) User Sequence Number 1 Mask */
#define AFEC_SEQ1R_USCH1(value)               (AFEC_SEQ1R_USCH1_Msk & ((value) << AFEC_SEQ1R_USCH1_Pos))
#define AFEC_SEQ1R_USCH2_Pos                  _U_(8)                                               /**< (AFEC_SEQ1R) User Sequence Number 2 Position */
#define AFEC_SEQ1R_USCH2_Msk                  (_U_(0xF) << AFEC_SEQ1R_USCH2_Pos)                   /**< (AFEC_SEQ1R) User Sequence Number 2 Mask */
#define AFEC_SEQ1R_USCH2(value)               (AFEC_SEQ1R_USCH2_Msk & ((value) << AFEC_SEQ1R_USCH2_Pos))
#define AFEC_SEQ1R_USCH3_Pos                  _U_(12)                                              /**< (AFEC_SEQ1R) User Sequence Number 3 Position */
#define AFEC_SEQ1R_USCH3_Msk                  (_U_(0xF) << AFEC_SEQ1R_USCH3_Pos)                   /**< (AFEC_SEQ1R) User Sequence Number 3 Mask */
#define AFEC_SEQ1R_USCH3(value)               (AFEC_SEQ1R_USCH3_Msk & ((value) << AFEC_SEQ1R_USCH3_Pos))
#define AFEC_SEQ1R_USCH4_Pos                  _U_(16)                                              /**< (AFEC_SEQ1R) User Sequence Number 4 Position */
#define AFEC_SEQ1R_USCH4_Msk                  (_U_(0xF) << AFEC_SEQ1R_USCH4_Pos)                   /**< (AFEC_SEQ1R) User Sequence Number 4 Mask */
#define AFEC_SEQ1R_USCH4(value)               (AFEC_SEQ1R_USCH4_Msk & ((value) << AFEC_SEQ1R_USCH4_Pos))
#define AFEC_SEQ1R_USCH5_Pos                  _U_(20)                                              /**< (AFEC_SEQ1R) User Sequence Number 5 Position */
#define AFEC_SEQ1R_USCH5_Msk                  (_U_(0xF) << AFEC_SEQ1R_USCH5_Pos)                   /**< (AFEC_SEQ1R) User Sequence Number 5 Mask */
#define AFEC_SEQ1R_USCH5(value)               (AFEC_SEQ1R_USCH5_Msk & ((value) << AFEC_SEQ1R_USCH5_Pos))
#define AFEC_SEQ1R_USCH6_Pos                  _U_(24)                                              /**< (AFEC_SEQ1R) User Sequence Number 6 Position */
#define AFEC_SEQ1R_USCH6_Msk                  (_U_(0xF) << AFEC_SEQ1R_USCH6_Pos)                   /**< (AFEC_SEQ1R) User Sequence Number 6 Mask */
#define AFEC_SEQ1R_USCH6(value)               (AFEC_SEQ1R_USCH6_Msk & ((value) << AFEC_SEQ1R_USCH6_Pos))
#define AFEC_SEQ1R_USCH7_Pos                  _U_(28)                                              /**< (AFEC_SEQ1R) User Sequence Number 7 Position */
#define AFEC_SEQ1R_USCH7_Msk                  (_U_(0xF) << AFEC_SEQ1R_USCH7_Pos)                   /**< (AFEC_SEQ1R) User Sequence Number 7 Mask */
#define AFEC_SEQ1R_USCH7(value)               (AFEC_SEQ1R_USCH7_Msk & ((value) << AFEC_SEQ1R_USCH7_Pos))
#define AFEC_SEQ1R_Msk                        _U_(0xFFFFFFFF)                                      /**< (AFEC_SEQ1R) Register Mask  */


/* -------- AFEC_SEQ2R : (AFEC Offset: 0x10) (R/W 32) AFEC Channel Sequence 2 Register -------- */
#define AFEC_SEQ2R_USCH8_Pos                  _U_(0)                                               /**< (AFEC_SEQ2R) User Sequence Number 8 Position */
#define AFEC_SEQ2R_USCH8_Msk                  (_U_(0xF) << AFEC_SEQ2R_USCH8_Pos)                   /**< (AFEC_SEQ2R) User Sequence Number 8 Mask */
#define AFEC_SEQ2R_USCH8(value)               (AFEC_SEQ2R_USCH8_Msk & ((value) << AFEC_SEQ2R_USCH8_Pos))
#define AFEC_SEQ2R_USCH9_Pos                  _U_(4)                                               /**< (AFEC_SEQ2R) User Sequence Number 9 Position */
#define AFEC_SEQ2R_USCH9_Msk                  (_U_(0xF) << AFEC_SEQ2R_USCH9_Pos)                   /**< (AFEC_SEQ2R) User Sequence Number 9 Mask */
#define AFEC_SEQ2R_USCH9(value)               (AFEC_SEQ2R_USCH9_Msk & ((value) << AFEC_SEQ2R_USCH9_Pos))
#define AFEC_SEQ2R_USCH10_Pos                 _U_(8)                                               /**< (AFEC_SEQ2R) User Sequence Number 10 Position */
#define AFEC_SEQ2R_USCH10_Msk                 (_U_(0xF) << AFEC_SEQ2R_USCH10_Pos)                  /**< (AFEC_SEQ2R) User Sequence Number 10 Mask */
#define AFEC_SEQ2R_USCH10(value)              (AFEC_SEQ2R_USCH10_Msk & ((value) << AFEC_SEQ2R_USCH10_Pos))
#define AFEC_SEQ2R_USCH11_Pos                 _U_(12)                                              /**< (AFEC_SEQ2R) User Sequence Number 11 Position */
#define AFEC_SEQ2R_USCH11_Msk                 (_U_(0xF) << AFEC_SEQ2R_USCH11_Pos)                  /**< (AFEC_SEQ2R) User Sequence Number 11 Mask */
#define AFEC_SEQ2R_USCH11(value)              (AFEC_SEQ2R_USCH11_Msk & ((value) << AFEC_SEQ2R_USCH11_Pos))
#define AFEC_SEQ2R_Msk                        _U_(0x0000FFFF)                                      /**< (AFEC_SEQ2R) Register Mask  */


/* -------- AFEC_CHER : (AFEC Offset: 0x14) ( /W 32) AFEC Channel Enable Register -------- */
#define AFEC_CHER_CH0_Pos                     _U_(0)                                               /**< (AFEC_CHER) Channel 0 Enable Position */
#define AFEC_CHER_CH0_Msk                     (_U_(0x1) << AFEC_CHER_CH0_Pos)                      /**< (AFEC_CHER) Channel 0 Enable Mask */
#define AFEC_CHER_CH0(value)                  (AFEC_CHER_CH0_Msk & ((value) << AFEC_CHER_CH0_Pos))
#define AFEC_CHER_CH1_Pos                     _U_(1)                                               /**< (AFEC_CHER) Channel 1 Enable Position */
#define AFEC_CHER_CH1_Msk                     (_U_(0x1) << AFEC_CHER_CH1_Pos)                      /**< (AFEC_CHER) Channel 1 Enable Mask */
#define AFEC_CHER_CH1(value)                  (AFEC_CHER_CH1_Msk & ((value) << AFEC_CHER_CH1_Pos))
#define AFEC_CHER_CH2_Pos                     _U_(2)                                               /**< (AFEC_CHER) Channel 2 Enable Position */
#define AFEC_CHER_CH2_Msk                     (_U_(0x1) << AFEC_CHER_CH2_Pos)                      /**< (AFEC_CHER) Channel 2 Enable Mask */
#define AFEC_CHER_CH2(value)                  (AFEC_CHER_CH2_Msk & ((value) << AFEC_CHER_CH2_Pos))
#define AFEC_CHER_CH3_Pos                     _U_(3)                                               /**< (AFEC_CHER) Channel 3 Enable Position */
#define AFEC_CHER_CH3_Msk                     (_U_(0x1) << AFEC_CHER_CH3_Pos)                      /**< (AFEC_CHER) Channel 3 Enable Mask */
#define AFEC_CHER_CH3(value)                  (AFEC_CHER_CH3_Msk & ((value) << AFEC_CHER_CH3_Pos))
#define AFEC_CHER_CH4_Pos                     _U_(4)                                               /**< (AFEC_CHER) Channel 4 Enable Position */
#define AFEC_CHER_CH4_Msk                     (_U_(0x1) << AFEC_CHER_CH4_Pos)                      /**< (AFEC_CHER) Channel 4 Enable Mask */
#define AFEC_CHER_CH4(value)                  (AFEC_CHER_CH4_Msk & ((value) << AFEC_CHER_CH4_Pos))
#define AFEC_CHER_CH5_Pos                     _U_(5)                                               /**< (AFEC_CHER) Channel 5 Enable Position */
#define AFEC_CHER_CH5_Msk                     (_U_(0x1) << AFEC_CHER_CH5_Pos)                      /**< (AFEC_CHER) Channel 5 Enable Mask */
#define AFEC_CHER_CH5(value)                  (AFEC_CHER_CH5_Msk & ((value) << AFEC_CHER_CH5_Pos))
#define AFEC_CHER_CH6_Pos                     _U_(6)                                               /**< (AFEC_CHER) Channel 6 Enable Position */
#define AFEC_CHER_CH6_Msk                     (_U_(0x1) << AFEC_CHER_CH6_Pos)                      /**< (AFEC_CHER) Channel 6 Enable Mask */
#define AFEC_CHER_CH6(value)                  (AFEC_CHER_CH6_Msk & ((value) << AFEC_CHER_CH6_Pos))
#define AFEC_CHER_CH7_Pos                     _U_(7)                                               /**< (AFEC_CHER) Channel 7 Enable Position */
#define AFEC_CHER_CH7_Msk                     (_U_(0x1) << AFEC_CHER_CH7_Pos)                      /**< (AFEC_CHER) Channel 7 Enable Mask */
#define AFEC_CHER_CH7(value)                  (AFEC_CHER_CH7_Msk & ((value) << AFEC_CHER_CH7_Pos))
#define AFEC_CHER_CH8_Pos                     _U_(8)                                               /**< (AFEC_CHER) Channel 8 Enable Position */
#define AFEC_CHER_CH8_Msk                     (_U_(0x1) << AFEC_CHER_CH8_Pos)                      /**< (AFEC_CHER) Channel 8 Enable Mask */
#define AFEC_CHER_CH8(value)                  (AFEC_CHER_CH8_Msk & ((value) << AFEC_CHER_CH8_Pos))
#define AFEC_CHER_CH9_Pos                     _U_(9)                                               /**< (AFEC_CHER) Channel 9 Enable Position */
#define AFEC_CHER_CH9_Msk                     (_U_(0x1) << AFEC_CHER_CH9_Pos)                      /**< (AFEC_CHER) Channel 9 Enable Mask */
#define AFEC_CHER_CH9(value)                  (AFEC_CHER_CH9_Msk & ((value) << AFEC_CHER_CH9_Pos))
#define AFEC_CHER_CH10_Pos                    _U_(10)                                              /**< (AFEC_CHER) Channel 10 Enable Position */
#define AFEC_CHER_CH10_Msk                    (_U_(0x1) << AFEC_CHER_CH10_Pos)                     /**< (AFEC_CHER) Channel 10 Enable Mask */
#define AFEC_CHER_CH10(value)                 (AFEC_CHER_CH10_Msk & ((value) << AFEC_CHER_CH10_Pos))
#define AFEC_CHER_CH11_Pos                    _U_(11)                                              /**< (AFEC_CHER) Channel 11 Enable Position */
#define AFEC_CHER_CH11_Msk                    (_U_(0x1) << AFEC_CHER_CH11_Pos)                     /**< (AFEC_CHER) Channel 11 Enable Mask */
#define AFEC_CHER_CH11(value)                 (AFEC_CHER_CH11_Msk & ((value) << AFEC_CHER_CH11_Pos))
#define AFEC_CHER_Msk                         _U_(0x00000FFF)                                      /**< (AFEC_CHER) Register Mask  */

#define AFEC_CHER_CH_Pos                      _U_(0)                                               /**< (AFEC_CHER Position) Channel xx Enable */
#define AFEC_CHER_CH_Msk                      (_U_(0xFFF) << AFEC_CHER_CH_Pos)                     /**< (AFEC_CHER Mask) CH */
#define AFEC_CHER_CH(value)                   (AFEC_CHER_CH_Msk & ((value) << AFEC_CHER_CH_Pos))   

/* -------- AFEC_CHDR : (AFEC Offset: 0x18) ( /W 32) AFEC Channel Disable Register -------- */
#define AFEC_CHDR_CH0_Pos                     _U_(0)                                               /**< (AFEC_CHDR) Channel 0 Disable Position */
#define AFEC_CHDR_CH0_Msk                     (_U_(0x1) << AFEC_CHDR_CH0_Pos)                      /**< (AFEC_CHDR) Channel 0 Disable Mask */
#define AFEC_CHDR_CH0(value)                  (AFEC_CHDR_CH0_Msk & ((value) << AFEC_CHDR_CH0_Pos))
#define AFEC_CHDR_CH1_Pos                     _U_(1)                                               /**< (AFEC_CHDR) Channel 1 Disable Position */
#define AFEC_CHDR_CH1_Msk                     (_U_(0x1) << AFEC_CHDR_CH1_Pos)                      /**< (AFEC_CHDR) Channel 1 Disable Mask */
#define AFEC_CHDR_CH1(value)                  (AFEC_CHDR_CH1_Msk & ((value) << AFEC_CHDR_CH1_Pos))
#define AFEC_CHDR_CH2_Pos                     _U_(2)                                               /**< (AFEC_CHDR) Channel 2 Disable Position */
#define AFEC_CHDR_CH2_Msk                     (_U_(0x1) << AFEC_CHDR_CH2_Pos)                      /**< (AFEC_CHDR) Channel 2 Disable Mask */
#define AFEC_CHDR_CH2(value)                  (AFEC_CHDR_CH2_Msk & ((value) << AFEC_CHDR_CH2_Pos))
#define AFEC_CHDR_CH3_Pos                     _U_(3)                                               /**< (AFEC_CHDR) Channel 3 Disable Position */
#define AFEC_CHDR_CH3_Msk                     (_U_(0x1) << AFEC_CHDR_CH3_Pos)                      /**< (AFEC_CHDR) Channel 3 Disable Mask */
#define AFEC_CHDR_CH3(value)                  (AFEC_CHDR_CH3_Msk & ((value) << AFEC_CHDR_CH3_Pos))
#define AFEC_CHDR_CH4_Pos                     _U_(4)                                               /**< (AFEC_CHDR) Channel 4 Disable Position */
#define AFEC_CHDR_CH4_Msk                     (_U_(0x1) << AFEC_CHDR_CH4_Pos)                      /**< (AFEC_CHDR) Channel 4 Disable Mask */
#define AFEC_CHDR_CH4(value)                  (AFEC_CHDR_CH4_Msk & ((value) << AFEC_CHDR_CH4_Pos))
#define AFEC_CHDR_CH5_Pos                     _U_(5)                                               /**< (AFEC_CHDR) Channel 5 Disable Position */
#define AFEC_CHDR_CH5_Msk                     (_U_(0x1) << AFEC_CHDR_CH5_Pos)                      /**< (AFEC_CHDR) Channel 5 Disable Mask */
#define AFEC_CHDR_CH5(value)                  (AFEC_CHDR_CH5_Msk & ((value) << AFEC_CHDR_CH5_Pos))
#define AFEC_CHDR_CH6_Pos                     _U_(6)                                               /**< (AFEC_CHDR) Channel 6 Disable Position */
#define AFEC_CHDR_CH6_Msk                     (_U_(0x1) << AFEC_CHDR_CH6_Pos)                      /**< (AFEC_CHDR) Channel 6 Disable Mask */
#define AFEC_CHDR_CH6(value)                  (AFEC_CHDR_CH6_Msk & ((value) << AFEC_CHDR_CH6_Pos))
#define AFEC_CHDR_CH7_Pos                     _U_(7)                                               /**< (AFEC_CHDR) Channel 7 Disable Position */
#define AFEC_CHDR_CH7_Msk                     (_U_(0x1) << AFEC_CHDR_CH7_Pos)                      /**< (AFEC_CHDR) Channel 7 Disable Mask */
#define AFEC_CHDR_CH7(value)                  (AFEC_CHDR_CH7_Msk & ((value) << AFEC_CHDR_CH7_Pos))
#define AFEC_CHDR_CH8_Pos                     _U_(8)                                               /**< (AFEC_CHDR) Channel 8 Disable Position */
#define AFEC_CHDR_CH8_Msk                     (_U_(0x1) << AFEC_CHDR_CH8_Pos)                      /**< (AFEC_CHDR) Channel 8 Disable Mask */
#define AFEC_CHDR_CH8(value)                  (AFEC_CHDR_CH8_Msk & ((value) << AFEC_CHDR_CH8_Pos))
#define AFEC_CHDR_CH9_Pos                     _U_(9)                                               /**< (AFEC_CHDR) Channel 9 Disable Position */
#define AFEC_CHDR_CH9_Msk                     (_U_(0x1) << AFEC_CHDR_CH9_Pos)                      /**< (AFEC_CHDR) Channel 9 Disable Mask */
#define AFEC_CHDR_CH9(value)                  (AFEC_CHDR_CH9_Msk & ((value) << AFEC_CHDR_CH9_Pos))
#define AFEC_CHDR_CH10_Pos                    _U_(10)                                              /**< (AFEC_CHDR) Channel 10 Disable Position */
#define AFEC_CHDR_CH10_Msk                    (_U_(0x1) << AFEC_CHDR_CH10_Pos)                     /**< (AFEC_CHDR) Channel 10 Disable Mask */
#define AFEC_CHDR_CH10(value)                 (AFEC_CHDR_CH10_Msk & ((value) << AFEC_CHDR_CH10_Pos))
#define AFEC_CHDR_CH11_Pos                    _U_(11)                                              /**< (AFEC_CHDR) Channel 11 Disable Position */
#define AFEC_CHDR_CH11_Msk                    (_U_(0x1) << AFEC_CHDR_CH11_Pos)                     /**< (AFEC_CHDR) Channel 11 Disable Mask */
#define AFEC_CHDR_CH11(value)                 (AFEC_CHDR_CH11_Msk & ((value) << AFEC_CHDR_CH11_Pos))
#define AFEC_CHDR_Msk                         _U_(0x00000FFF)                                      /**< (AFEC_CHDR) Register Mask  */

#define AFEC_CHDR_CH_Pos                      _U_(0)                                               /**< (AFEC_CHDR Position) Channel xx Disable */
#define AFEC_CHDR_CH_Msk                      (_U_(0xFFF) << AFEC_CHDR_CH_Pos)                     /**< (AFEC_CHDR Mask) CH */
#define AFEC_CHDR_CH(value)                   (AFEC_CHDR_CH_Msk & ((value) << AFEC_CHDR_CH_Pos))   

/* -------- AFEC_CHSR : (AFEC Offset: 0x1C) ( R/ 32) AFEC Channel Status Register -------- */
#define AFEC_CHSR_CH0_Pos                     _U_(0)                                               /**< (AFEC_CHSR) Channel 0 Status Position */
#define AFEC_CHSR_CH0_Msk                     (_U_(0x1) << AFEC_CHSR_CH0_Pos)                      /**< (AFEC_CHSR) Channel 0 Status Mask */
#define AFEC_CHSR_CH0(value)                  (AFEC_CHSR_CH0_Msk & ((value) << AFEC_CHSR_CH0_Pos))
#define AFEC_CHSR_CH1_Pos                     _U_(1)                                               /**< (AFEC_CHSR) Channel 1 Status Position */
#define AFEC_CHSR_CH1_Msk                     (_U_(0x1) << AFEC_CHSR_CH1_Pos)                      /**< (AFEC_CHSR) Channel 1 Status Mask */
#define AFEC_CHSR_CH1(value)                  (AFEC_CHSR_CH1_Msk & ((value) << AFEC_CHSR_CH1_Pos))
#define AFEC_CHSR_CH2_Pos                     _U_(2)                                               /**< (AFEC_CHSR) Channel 2 Status Position */
#define AFEC_CHSR_CH2_Msk                     (_U_(0x1) << AFEC_CHSR_CH2_Pos)                      /**< (AFEC_CHSR) Channel 2 Status Mask */
#define AFEC_CHSR_CH2(value)                  (AFEC_CHSR_CH2_Msk & ((value) << AFEC_CHSR_CH2_Pos))
#define AFEC_CHSR_CH3_Pos                     _U_(3)                                               /**< (AFEC_CHSR) Channel 3 Status Position */
#define AFEC_CHSR_CH3_Msk                     (_U_(0x1) << AFEC_CHSR_CH3_Pos)                      /**< (AFEC_CHSR) Channel 3 Status Mask */
#define AFEC_CHSR_CH3(value)                  (AFEC_CHSR_CH3_Msk & ((value) << AFEC_CHSR_CH3_Pos))
#define AFEC_CHSR_CH4_Pos                     _U_(4)                                               /**< (AFEC_CHSR) Channel 4 Status Position */
#define AFEC_CHSR_CH4_Msk                     (_U_(0x1) << AFEC_CHSR_CH4_Pos)                      /**< (AFEC_CHSR) Channel 4 Status Mask */
#define AFEC_CHSR_CH4(value)                  (AFEC_CHSR_CH4_Msk & ((value) << AFEC_CHSR_CH4_Pos))
#define AFEC_CHSR_CH5_Pos                     _U_(5)                                               /**< (AFEC_CHSR) Channel 5 Status Position */
#define AFEC_CHSR_CH5_Msk                     (_U_(0x1) << AFEC_CHSR_CH5_Pos)                      /**< (AFEC_CHSR) Channel 5 Status Mask */
#define AFEC_CHSR_CH5(value)                  (AFEC_CHSR_CH5_Msk & ((value) << AFEC_CHSR_CH5_Pos))
#define AFEC_CHSR_CH6_Pos                     _U_(6)                                               /**< (AFEC_CHSR) Channel 6 Status Position */
#define AFEC_CHSR_CH6_Msk                     (_U_(0x1) << AFEC_CHSR_CH6_Pos)                      /**< (AFEC_CHSR) Channel 6 Status Mask */
#define AFEC_CHSR_CH6(value)                  (AFEC_CHSR_CH6_Msk & ((value) << AFEC_CHSR_CH6_Pos))
#define AFEC_CHSR_CH7_Pos                     _U_(7)                                               /**< (AFEC_CHSR) Channel 7 Status Position */
#define AFEC_CHSR_CH7_Msk                     (_U_(0x1) << AFEC_CHSR_CH7_Pos)                      /**< (AFEC_CHSR) Channel 7 Status Mask */
#define AFEC_CHSR_CH7(value)                  (AFEC_CHSR_CH7_Msk & ((value) << AFEC_CHSR_CH7_Pos))
#define AFEC_CHSR_CH8_Pos                     _U_(8)                                               /**< (AFEC_CHSR) Channel 8 Status Position */
#define AFEC_CHSR_CH8_Msk                     (_U_(0x1) << AFEC_CHSR_CH8_Pos)                      /**< (AFEC_CHSR) Channel 8 Status Mask */
#define AFEC_CHSR_CH8(value)                  (AFEC_CHSR_CH8_Msk & ((value) << AFEC_CHSR_CH8_Pos))
#define AFEC_CHSR_CH9_Pos                     _U_(9)                                               /**< (AFEC_CHSR) Channel 9 Status Position */
#define AFEC_CHSR_CH9_Msk                     (_U_(0x1) << AFEC_CHSR_CH9_Pos)                      /**< (AFEC_CHSR) Channel 9 Status Mask */
#define AFEC_CHSR_CH9(value)                  (AFEC_CHSR_CH9_Msk & ((value) << AFEC_CHSR_CH9_Pos))
#define AFEC_CHSR_CH10_Pos                    _U_(10)                                              /**< (AFEC_CHSR) Channel 10 Status Position */
#define AFEC_CHSR_CH10_Msk                    (_U_(0x1) << AFEC_CHSR_CH10_Pos)                     /**< (AFEC_CHSR) Channel 10 Status Mask */
#define AFEC_CHSR_CH10(value)                 (AFEC_CHSR_CH10_Msk & ((value) << AFEC_CHSR_CH10_Pos))
#define AFEC_CHSR_CH11_Pos                    _U_(11)                                              /**< (AFEC_CHSR) Channel 11 Status Position */
#define AFEC_CHSR_CH11_Msk                    (_U_(0x1) << AFEC_CHSR_CH11_Pos)                     /**< (AFEC_CHSR) Channel 11 Status Mask */
#define AFEC_CHSR_CH11(value)                 (AFEC_CHSR_CH11_Msk & ((value) << AFEC_CHSR_CH11_Pos))
#define AFEC_CHSR_Msk                         _U_(0x00000FFF)                                      /**< (AFEC_CHSR) Register Mask  */

#define AFEC_CHSR_CH_Pos                      _U_(0)                                               /**< (AFEC_CHSR Position) Channel xx Status */
#define AFEC_CHSR_CH_Msk                      (_U_(0xFFF) << AFEC_CHSR_CH_Pos)                     /**< (AFEC_CHSR Mask) CH */
#define AFEC_CHSR_CH(value)                   (AFEC_CHSR_CH_Msk & ((value) << AFEC_CHSR_CH_Pos))   

/* -------- AFEC_LCDR : (AFEC Offset: 0x20) ( R/ 32) AFEC Last Converted Data Register -------- */
#define AFEC_LCDR_LDATA_Pos                   _U_(0)                                               /**< (AFEC_LCDR) Last Data Converted Position */
#define AFEC_LCDR_LDATA_Msk                   (_U_(0xFFFF) << AFEC_LCDR_LDATA_Pos)                 /**< (AFEC_LCDR) Last Data Converted Mask */
#define AFEC_LCDR_LDATA(value)                (AFEC_LCDR_LDATA_Msk & ((value) << AFEC_LCDR_LDATA_Pos))
#define AFEC_LCDR_CHNB_Pos                    _U_(24)                                              /**< (AFEC_LCDR) Channel Number Position */
#define AFEC_LCDR_CHNB_Msk                    (_U_(0xF) << AFEC_LCDR_CHNB_Pos)                     /**< (AFEC_LCDR) Channel Number Mask */
#define AFEC_LCDR_CHNB(value)                 (AFEC_LCDR_CHNB_Msk & ((value) << AFEC_LCDR_CHNB_Pos))
#define AFEC_LCDR_Msk                         _U_(0x0F00FFFF)                                      /**< (AFEC_LCDR) Register Mask  */


/* -------- AFEC_IER : (AFEC Offset: 0x24) ( /W 32) AFEC Interrupt Enable Register -------- */
#define AFEC_IER_EOC0_Pos                     _U_(0)                                               /**< (AFEC_IER) End of Conversion Interrupt Enable 0 Position */
#define AFEC_IER_EOC0_Msk                     (_U_(0x1) << AFEC_IER_EOC0_Pos)                      /**< (AFEC_IER) End of Conversion Interrupt Enable 0 Mask */
#define AFEC_IER_EOC0(value)                  (AFEC_IER_EOC0_Msk & ((value) << AFEC_IER_EOC0_Pos))
#define AFEC_IER_EOC1_Pos                     _U_(1)                                               /**< (AFEC_IER) End of Conversion Interrupt Enable 1 Position */
#define AFEC_IER_EOC1_Msk                     (_U_(0x1) << AFEC_IER_EOC1_Pos)                      /**< (AFEC_IER) End of Conversion Interrupt Enable 1 Mask */
#define AFEC_IER_EOC1(value)                  (AFEC_IER_EOC1_Msk & ((value) << AFEC_IER_EOC1_Pos))
#define AFEC_IER_EOC2_Pos                     _U_(2)                                               /**< (AFEC_IER) End of Conversion Interrupt Enable 2 Position */
#define AFEC_IER_EOC2_Msk                     (_U_(0x1) << AFEC_IER_EOC2_Pos)                      /**< (AFEC_IER) End of Conversion Interrupt Enable 2 Mask */
#define AFEC_IER_EOC2(value)                  (AFEC_IER_EOC2_Msk & ((value) << AFEC_IER_EOC2_Pos))
#define AFEC_IER_EOC3_Pos                     _U_(3)                                               /**< (AFEC_IER) End of Conversion Interrupt Enable 3 Position */
#define AFEC_IER_EOC3_Msk                     (_U_(0x1) << AFEC_IER_EOC3_Pos)                      /**< (AFEC_IER) End of Conversion Interrupt Enable 3 Mask */
#define AFEC_IER_EOC3(value)                  (AFEC_IER_EOC3_Msk & ((value) << AFEC_IER_EOC3_Pos))
#define AFEC_IER_EOC4_Pos                     _U_(4)                                               /**< (AFEC_IER) End of Conversion Interrupt Enable 4 Position */
#define AFEC_IER_EOC4_Msk                     (_U_(0x1) << AFEC_IER_EOC4_Pos)                      /**< (AFEC_IER) End of Conversion Interrupt Enable 4 Mask */
#define AFEC_IER_EOC4(value)                  (AFEC_IER_EOC4_Msk & ((value) << AFEC_IER_EOC4_Pos))
#define AFEC_IER_EOC5_Pos                     _U_(5)                                               /**< (AFEC_IER) End of Conversion Interrupt Enable 5 Position */
#define AFEC_IER_EOC5_Msk                     (_U_(0x1) << AFEC_IER_EOC5_Pos)                      /**< (AFEC_IER) End of Conversion Interrupt Enable 5 Mask */
#define AFEC_IER_EOC5(value)                  (AFEC_IER_EOC5_Msk & ((value) << AFEC_IER_EOC5_Pos))
#define AFEC_IER_EOC6_Pos                     _U_(6)                                               /**< (AFEC_IER) End of Conversion Interrupt Enable 6 Position */
#define AFEC_IER_EOC6_Msk                     (_U_(0x1) << AFEC_IER_EOC6_Pos)                      /**< (AFEC_IER) End of Conversion Interrupt Enable 6 Mask */
#define AFEC_IER_EOC6(value)                  (AFEC_IER_EOC6_Msk & ((value) << AFEC_IER_EOC6_Pos))
#define AFEC_IER_EOC7_Pos                     _U_(7)                                               /**< (AFEC_IER) End of Conversion Interrupt Enable 7 Position */
#define AFEC_IER_EOC7_Msk                     (_U_(0x1) << AFEC_IER_EOC7_Pos)                      /**< (AFEC_IER) End of Conversion Interrupt Enable 7 Mask */
#define AFEC_IER_EOC7(value)                  (AFEC_IER_EOC7_Msk & ((value) << AFEC_IER_EOC7_Pos))
#define AFEC_IER_EOC8_Pos                     _U_(8)                                               /**< (AFEC_IER) End of Conversion Interrupt Enable 8 Position */
#define AFEC_IER_EOC8_Msk                     (_U_(0x1) << AFEC_IER_EOC8_Pos)                      /**< (AFEC_IER) End of Conversion Interrupt Enable 8 Mask */
#define AFEC_IER_EOC8(value)                  (AFEC_IER_EOC8_Msk & ((value) << AFEC_IER_EOC8_Pos))
#define AFEC_IER_EOC9_Pos                     _U_(9)                                               /**< (AFEC_IER) End of Conversion Interrupt Enable 9 Position */
#define AFEC_IER_EOC9_Msk                     (_U_(0x1) << AFEC_IER_EOC9_Pos)                      /**< (AFEC_IER) End of Conversion Interrupt Enable 9 Mask */
#define AFEC_IER_EOC9(value)                  (AFEC_IER_EOC9_Msk & ((value) << AFEC_IER_EOC9_Pos))
#define AFEC_IER_EOC10_Pos                    _U_(10)                                              /**< (AFEC_IER) End of Conversion Interrupt Enable 10 Position */
#define AFEC_IER_EOC10_Msk                    (_U_(0x1) << AFEC_IER_EOC10_Pos)                     /**< (AFEC_IER) End of Conversion Interrupt Enable 10 Mask */
#define AFEC_IER_EOC10(value)                 (AFEC_IER_EOC10_Msk & ((value) << AFEC_IER_EOC10_Pos))
#define AFEC_IER_EOC11_Pos                    _U_(11)                                              /**< (AFEC_IER) End of Conversion Interrupt Enable 11 Position */
#define AFEC_IER_EOC11_Msk                    (_U_(0x1) << AFEC_IER_EOC11_Pos)                     /**< (AFEC_IER) End of Conversion Interrupt Enable 11 Mask */
#define AFEC_IER_EOC11(value)                 (AFEC_IER_EOC11_Msk & ((value) << AFEC_IER_EOC11_Pos))
#define AFEC_IER_DRDY_Pos                     _U_(24)                                              /**< (AFEC_IER) Data Ready Interrupt Enable Position */
#define AFEC_IER_DRDY_Msk                     (_U_(0x1) << AFEC_IER_DRDY_Pos)                      /**< (AFEC_IER) Data Ready Interrupt Enable Mask */
#define AFEC_IER_DRDY(value)                  (AFEC_IER_DRDY_Msk & ((value) << AFEC_IER_DRDY_Pos))
#define AFEC_IER_GOVRE_Pos                    _U_(25)                                              /**< (AFEC_IER) General Overrun Error Interrupt Enable Position */
#define AFEC_IER_GOVRE_Msk                    (_U_(0x1) << AFEC_IER_GOVRE_Pos)                     /**< (AFEC_IER) General Overrun Error Interrupt Enable Mask */
#define AFEC_IER_GOVRE(value)                 (AFEC_IER_GOVRE_Msk & ((value) << AFEC_IER_GOVRE_Pos))
#define AFEC_IER_COMPE_Pos                    _U_(26)                                              /**< (AFEC_IER) Comparison Event Interrupt Enable Position */
#define AFEC_IER_COMPE_Msk                    (_U_(0x1) << AFEC_IER_COMPE_Pos)                     /**< (AFEC_IER) Comparison Event Interrupt Enable Mask */
#define AFEC_IER_COMPE(value)                 (AFEC_IER_COMPE_Msk & ((value) << AFEC_IER_COMPE_Pos))
#define AFEC_IER_TEMPCHG_Pos                  _U_(30)                                              /**< (AFEC_IER) Temperature Change Interrupt Enable Position */
#define AFEC_IER_TEMPCHG_Msk                  (_U_(0x1) << AFEC_IER_TEMPCHG_Pos)                   /**< (AFEC_IER) Temperature Change Interrupt Enable Mask */
#define AFEC_IER_TEMPCHG(value)               (AFEC_IER_TEMPCHG_Msk & ((value) << AFEC_IER_TEMPCHG_Pos))
#define AFEC_IER_Msk                          _U_(0x47000FFF)                                      /**< (AFEC_IER) Register Mask  */

#define AFEC_IER_EOC_Pos                      _U_(0)                                               /**< (AFEC_IER Position) End of Conversion Interrupt Enable x */
#define AFEC_IER_EOC_Msk                      (_U_(0xFFF) << AFEC_IER_EOC_Pos)                     /**< (AFEC_IER Mask) EOC */
#define AFEC_IER_EOC(value)                   (AFEC_IER_EOC_Msk & ((value) << AFEC_IER_EOC_Pos))   

/* -------- AFEC_IDR : (AFEC Offset: 0x28) ( /W 32) AFEC Interrupt Disable Register -------- */
#define AFEC_IDR_EOC0_Pos                     _U_(0)                                               /**< (AFEC_IDR) End of Conversion Interrupt Disable 0 Position */
#define AFEC_IDR_EOC0_Msk                     (_U_(0x1) << AFEC_IDR_EOC0_Pos)                      /**< (AFEC_IDR) End of Conversion Interrupt Disable 0 Mask */
#define AFEC_IDR_EOC0(value)                  (AFEC_IDR_EOC0_Msk & ((value) << AFEC_IDR_EOC0_Pos))
#define AFEC_IDR_EOC1_Pos                     _U_(1)                                               /**< (AFEC_IDR) End of Conversion Interrupt Disable 1 Position */
#define AFEC_IDR_EOC1_Msk                     (_U_(0x1) << AFEC_IDR_EOC1_Pos)                      /**< (AFEC_IDR) End of Conversion Interrupt Disable 1 Mask */
#define AFEC_IDR_EOC1(value)                  (AFEC_IDR_EOC1_Msk & ((value) << AFEC_IDR_EOC1_Pos))
#define AFEC_IDR_EOC2_Pos                     _U_(2)                                               /**< (AFEC_IDR) End of Conversion Interrupt Disable 2 Position */
#define AFEC_IDR_EOC2_Msk                     (_U_(0x1) << AFEC_IDR_EOC2_Pos)                      /**< (AFEC_IDR) End of Conversion Interrupt Disable 2 Mask */
#define AFEC_IDR_EOC2(value)                  (AFEC_IDR_EOC2_Msk & ((value) << AFEC_IDR_EOC2_Pos))
#define AFEC_IDR_EOC3_Pos                     _U_(3)                                               /**< (AFEC_IDR) End of Conversion Interrupt Disable 3 Position */
#define AFEC_IDR_EOC3_Msk                     (_U_(0x1) << AFEC_IDR_EOC3_Pos)                      /**< (AFEC_IDR) End of Conversion Interrupt Disable 3 Mask */
#define AFEC_IDR_EOC3(value)                  (AFEC_IDR_EOC3_Msk & ((value) << AFEC_IDR_EOC3_Pos))
#define AFEC_IDR_EOC4_Pos                     _U_(4)                                               /**< (AFEC_IDR) End of Conversion Interrupt Disable 4 Position */
#define AFEC_IDR_EOC4_Msk                     (_U_(0x1) << AFEC_IDR_EOC4_Pos)                      /**< (AFEC_IDR) End of Conversion Interrupt Disable 4 Mask */
#define AFEC_IDR_EOC4(value)                  (AFEC_IDR_EOC4_Msk & ((value) << AFEC_IDR_EOC4_Pos))
#define AFEC_IDR_EOC5_Pos                     _U_(5)                                               /**< (AFEC_IDR) End of Conversion Interrupt Disable 5 Position */
#define AFEC_IDR_EOC5_Msk                     (_U_(0x1) << AFEC_IDR_EOC5_Pos)                      /**< (AFEC_IDR) End of Conversion Interrupt Disable 5 Mask */
#define AFEC_IDR_EOC5(value)                  (AFEC_IDR_EOC5_Msk & ((value) << AFEC_IDR_EOC5_Pos))
#define AFEC_IDR_EOC6_Pos                     _U_(6)                                               /**< (AFEC_IDR) End of Conversion Interrupt Disable 6 Position */
#define AFEC_IDR_EOC6_Msk                     (_U_(0x1) << AFEC_IDR_EOC6_Pos)                      /**< (AFEC_IDR) End of Conversion Interrupt Disable 6 Mask */
#define AFEC_IDR_EOC6(value)                  (AFEC_IDR_EOC6_Msk & ((value) << AFEC_IDR_EOC6_Pos))
#define AFEC_IDR_EOC7_Pos                     _U_(7)                                               /**< (AFEC_IDR) End of Conversion Interrupt Disable 7 Position */
#define AFEC_IDR_EOC7_Msk                     (_U_(0x1) << AFEC_IDR_EOC7_Pos)                      /**< (AFEC_IDR) End of Conversion Interrupt Disable 7 Mask */
#define AFEC_IDR_EOC7(value)                  (AFEC_IDR_EOC7_Msk & ((value) << AFEC_IDR_EOC7_Pos))
#define AFEC_IDR_EOC8_Pos                     _U_(8)                                               /**< (AFEC_IDR) End of Conversion Interrupt Disable 8 Position */
#define AFEC_IDR_EOC8_Msk                     (_U_(0x1) << AFEC_IDR_EOC8_Pos)                      /**< (AFEC_IDR) End of Conversion Interrupt Disable 8 Mask */
#define AFEC_IDR_EOC8(value)                  (AFEC_IDR_EOC8_Msk & ((value) << AFEC_IDR_EOC8_Pos))
#define AFEC_IDR_EOC9_Pos                     _U_(9)                                               /**< (AFEC_IDR) End of Conversion Interrupt Disable 9 Position */
#define AFEC_IDR_EOC9_Msk                     (_U_(0x1) << AFEC_IDR_EOC9_Pos)                      /**< (AFEC_IDR) End of Conversion Interrupt Disable 9 Mask */
#define AFEC_IDR_EOC9(value)                  (AFEC_IDR_EOC9_Msk & ((value) << AFEC_IDR_EOC9_Pos))
#define AFEC_IDR_EOC10_Pos                    _U_(10)                                              /**< (AFEC_IDR) End of Conversion Interrupt Disable 10 Position */
#define AFEC_IDR_EOC10_Msk                    (_U_(0x1) << AFEC_IDR_EOC10_Pos)                     /**< (AFEC_IDR) End of Conversion Interrupt Disable 10 Mask */
#define AFEC_IDR_EOC10(value)                 (AFEC_IDR_EOC10_Msk & ((value) << AFEC_IDR_EOC10_Pos))
#define AFEC_IDR_EOC11_Pos                    _U_(11)                                              /**< (AFEC_IDR) End of Conversion Interrupt Disable 11 Position */
#define AFEC_IDR_EOC11_Msk                    (_U_(0x1) << AFEC_IDR_EOC11_Pos)                     /**< (AFEC_IDR) End of Conversion Interrupt Disable 11 Mask */
#define AFEC_IDR_EOC11(value)                 (AFEC_IDR_EOC11_Msk & ((value) << AFEC_IDR_EOC11_Pos))
#define AFEC_IDR_DRDY_Pos                     _U_(24)                                              /**< (AFEC_IDR) Data Ready Interrupt Disable Position */
#define AFEC_IDR_DRDY_Msk                     (_U_(0x1) << AFEC_IDR_DRDY_Pos)                      /**< (AFEC_IDR) Data Ready Interrupt Disable Mask */
#define AFEC_IDR_DRDY(value)                  (AFEC_IDR_DRDY_Msk & ((value) << AFEC_IDR_DRDY_Pos))
#define AFEC_IDR_GOVRE_Pos                    _U_(25)                                              /**< (AFEC_IDR) General Overrun Error Interrupt Disable Position */
#define AFEC_IDR_GOVRE_Msk                    (_U_(0x1) << AFEC_IDR_GOVRE_Pos)                     /**< (AFEC_IDR) General Overrun Error Interrupt Disable Mask */
#define AFEC_IDR_GOVRE(value)                 (AFEC_IDR_GOVRE_Msk & ((value) << AFEC_IDR_GOVRE_Pos))
#define AFEC_IDR_COMPE_Pos                    _U_(26)                                              /**< (AFEC_IDR) Comparison Event Interrupt Disable Position */
#define AFEC_IDR_COMPE_Msk                    (_U_(0x1) << AFEC_IDR_COMPE_Pos)                     /**< (AFEC_IDR) Comparison Event Interrupt Disable Mask */
#define AFEC_IDR_COMPE(value)                 (AFEC_IDR_COMPE_Msk & ((value) << AFEC_IDR_COMPE_Pos))
#define AFEC_IDR_TEMPCHG_Pos                  _U_(30)                                              /**< (AFEC_IDR) Temperature Change Interrupt Disable Position */
#define AFEC_IDR_TEMPCHG_Msk                  (_U_(0x1) << AFEC_IDR_TEMPCHG_Pos)                   /**< (AFEC_IDR) Temperature Change Interrupt Disable Mask */
#define AFEC_IDR_TEMPCHG(value)               (AFEC_IDR_TEMPCHG_Msk & ((value) << AFEC_IDR_TEMPCHG_Pos))
#define AFEC_IDR_Msk                          _U_(0x47000FFF)                                      /**< (AFEC_IDR) Register Mask  */

#define AFEC_IDR_EOC_Pos                      _U_(0)                                               /**< (AFEC_IDR Position) End of Conversion Interrupt Disable x */
#define AFEC_IDR_EOC_Msk                      (_U_(0xFFF) << AFEC_IDR_EOC_Pos)                     /**< (AFEC_IDR Mask) EOC */
#define AFEC_IDR_EOC(value)                   (AFEC_IDR_EOC_Msk & ((value) << AFEC_IDR_EOC_Pos))   

/* -------- AFEC_IMR : (AFEC Offset: 0x2C) ( R/ 32) AFEC Interrupt Mask Register -------- */
#define AFEC_IMR_EOC0_Pos                     _U_(0)                                               /**< (AFEC_IMR) End of Conversion Interrupt Mask 0 Position */
#define AFEC_IMR_EOC0_Msk                     (_U_(0x1) << AFEC_IMR_EOC0_Pos)                      /**< (AFEC_IMR) End of Conversion Interrupt Mask 0 Mask */
#define AFEC_IMR_EOC0(value)                  (AFEC_IMR_EOC0_Msk & ((value) << AFEC_IMR_EOC0_Pos))
#define AFEC_IMR_EOC1_Pos                     _U_(1)                                               /**< (AFEC_IMR) End of Conversion Interrupt Mask 1 Position */
#define AFEC_IMR_EOC1_Msk                     (_U_(0x1) << AFEC_IMR_EOC1_Pos)                      /**< (AFEC_IMR) End of Conversion Interrupt Mask 1 Mask */
#define AFEC_IMR_EOC1(value)                  (AFEC_IMR_EOC1_Msk & ((value) << AFEC_IMR_EOC1_Pos))
#define AFEC_IMR_EOC2_Pos                     _U_(2)                                               /**< (AFEC_IMR) End of Conversion Interrupt Mask 2 Position */
#define AFEC_IMR_EOC2_Msk                     (_U_(0x1) << AFEC_IMR_EOC2_Pos)                      /**< (AFEC_IMR) End of Conversion Interrupt Mask 2 Mask */
#define AFEC_IMR_EOC2(value)                  (AFEC_IMR_EOC2_Msk & ((value) << AFEC_IMR_EOC2_Pos))
#define AFEC_IMR_EOC3_Pos                     _U_(3)                                               /**< (AFEC_IMR) End of Conversion Interrupt Mask 3 Position */
#define AFEC_IMR_EOC3_Msk                     (_U_(0x1) << AFEC_IMR_EOC3_Pos)                      /**< (AFEC_IMR) End of Conversion Interrupt Mask 3 Mask */
#define AFEC_IMR_EOC3(value)                  (AFEC_IMR_EOC3_Msk & ((value) << AFEC_IMR_EOC3_Pos))
#define AFEC_IMR_EOC4_Pos                     _U_(4)                                               /**< (AFEC_IMR) End of Conversion Interrupt Mask 4 Position */
#define AFEC_IMR_EOC4_Msk                     (_U_(0x1) << AFEC_IMR_EOC4_Pos)                      /**< (AFEC_IMR) End of Conversion Interrupt Mask 4 Mask */
#define AFEC_IMR_EOC4(value)                  (AFEC_IMR_EOC4_Msk & ((value) << AFEC_IMR_EOC4_Pos))
#define AFEC_IMR_EOC5_Pos                     _U_(5)                                               /**< (AFEC_IMR) End of Conversion Interrupt Mask 5 Position */
#define AFEC_IMR_EOC5_Msk                     (_U_(0x1) << AFEC_IMR_EOC5_Pos)                      /**< (AFEC_IMR) End of Conversion Interrupt Mask 5 Mask */
#define AFEC_IMR_EOC5(value)                  (AFEC_IMR_EOC5_Msk & ((value) << AFEC_IMR_EOC5_Pos))
#define AFEC_IMR_EOC6_Pos                     _U_(6)                                               /**< (AFEC_IMR) End of Conversion Interrupt Mask 6 Position */
#define AFEC_IMR_EOC6_Msk                     (_U_(0x1) << AFEC_IMR_EOC6_Pos)                      /**< (AFEC_IMR) End of Conversion Interrupt Mask 6 Mask */
#define AFEC_IMR_EOC6(value)                  (AFEC_IMR_EOC6_Msk & ((value) << AFEC_IMR_EOC6_Pos))
#define AFEC_IMR_EOC7_Pos                     _U_(7)                                               /**< (AFEC_IMR) End of Conversion Interrupt Mask 7 Position */
#define AFEC_IMR_EOC7_Msk                     (_U_(0x1) << AFEC_IMR_EOC7_Pos)                      /**< (AFEC_IMR) End of Conversion Interrupt Mask 7 Mask */
#define AFEC_IMR_EOC7(value)                  (AFEC_IMR_EOC7_Msk & ((value) << AFEC_IMR_EOC7_Pos))
#define AFEC_IMR_EOC8_Pos                     _U_(8)                                               /**< (AFEC_IMR) End of Conversion Interrupt Mask 8 Position */
#define AFEC_IMR_EOC8_Msk                     (_U_(0x1) << AFEC_IMR_EOC8_Pos)                      /**< (AFEC_IMR) End of Conversion Interrupt Mask 8 Mask */
#define AFEC_IMR_EOC8(value)                  (AFEC_IMR_EOC8_Msk & ((value) << AFEC_IMR_EOC8_Pos))
#define AFEC_IMR_EOC9_Pos                     _U_(9)                                               /**< (AFEC_IMR) End of Conversion Interrupt Mask 9 Position */
#define AFEC_IMR_EOC9_Msk                     (_U_(0x1) << AFEC_IMR_EOC9_Pos)                      /**< (AFEC_IMR) End of Conversion Interrupt Mask 9 Mask */
#define AFEC_IMR_EOC9(value)                  (AFEC_IMR_EOC9_Msk & ((value) << AFEC_IMR_EOC9_Pos))
#define AFEC_IMR_EOC10_Pos                    _U_(10)                                              /**< (AFEC_IMR) End of Conversion Interrupt Mask 10 Position */
#define AFEC_IMR_EOC10_Msk                    (_U_(0x1) << AFEC_IMR_EOC10_Pos)                     /**< (AFEC_IMR) End of Conversion Interrupt Mask 10 Mask */
#define AFEC_IMR_EOC10(value)                 (AFEC_IMR_EOC10_Msk & ((value) << AFEC_IMR_EOC10_Pos))
#define AFEC_IMR_EOC11_Pos                    _U_(11)                                              /**< (AFEC_IMR) End of Conversion Interrupt Mask 11 Position */
#define AFEC_IMR_EOC11_Msk                    (_U_(0x1) << AFEC_IMR_EOC11_Pos)                     /**< (AFEC_IMR) End of Conversion Interrupt Mask 11 Mask */
#define AFEC_IMR_EOC11(value)                 (AFEC_IMR_EOC11_Msk & ((value) << AFEC_IMR_EOC11_Pos))
#define AFEC_IMR_DRDY_Pos                     _U_(24)                                              /**< (AFEC_IMR) Data Ready Interrupt Mask Position */
#define AFEC_IMR_DRDY_Msk                     (_U_(0x1) << AFEC_IMR_DRDY_Pos)                      /**< (AFEC_IMR) Data Ready Interrupt Mask Mask */
#define AFEC_IMR_DRDY(value)                  (AFEC_IMR_DRDY_Msk & ((value) << AFEC_IMR_DRDY_Pos))
#define AFEC_IMR_GOVRE_Pos                    _U_(25)                                              /**< (AFEC_IMR) General Overrun Error Interrupt Mask Position */
#define AFEC_IMR_GOVRE_Msk                    (_U_(0x1) << AFEC_IMR_GOVRE_Pos)                     /**< (AFEC_IMR) General Overrun Error Interrupt Mask Mask */
#define AFEC_IMR_GOVRE(value)                 (AFEC_IMR_GOVRE_Msk & ((value) << AFEC_IMR_GOVRE_Pos))
#define AFEC_IMR_COMPE_Pos                    _U_(26)                                              /**< (AFEC_IMR) Comparison Event Interrupt Mask Position */
#define AFEC_IMR_COMPE_Msk                    (_U_(0x1) << AFEC_IMR_COMPE_Pos)                     /**< (AFEC_IMR) Comparison Event Interrupt Mask Mask */
#define AFEC_IMR_COMPE(value)                 (AFEC_IMR_COMPE_Msk & ((value) << AFEC_IMR_COMPE_Pos))
#define AFEC_IMR_TEMPCHG_Pos                  _U_(30)                                              /**< (AFEC_IMR) Temperature Change Interrupt Mask Position */
#define AFEC_IMR_TEMPCHG_Msk                  (_U_(0x1) << AFEC_IMR_TEMPCHG_Pos)                   /**< (AFEC_IMR) Temperature Change Interrupt Mask Mask */
#define AFEC_IMR_TEMPCHG(value)               (AFEC_IMR_TEMPCHG_Msk & ((value) << AFEC_IMR_TEMPCHG_Pos))
#define AFEC_IMR_Msk                          _U_(0x47000FFF)                                      /**< (AFEC_IMR) Register Mask  */

#define AFEC_IMR_EOC_Pos                      _U_(0)                                               /**< (AFEC_IMR Position) End of Conversion Interrupt Mask x */
#define AFEC_IMR_EOC_Msk                      (_U_(0xFFF) << AFEC_IMR_EOC_Pos)                     /**< (AFEC_IMR Mask) EOC */
#define AFEC_IMR_EOC(value)                   (AFEC_IMR_EOC_Msk & ((value) << AFEC_IMR_EOC_Pos))   

/* -------- AFEC_ISR : (AFEC Offset: 0x30) ( R/ 32) AFEC Interrupt Status Register -------- */
#define AFEC_ISR_EOC0_Pos                     _U_(0)                                               /**< (AFEC_ISR) End of Conversion 0 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC0_Msk                     (_U_(0x1) << AFEC_ISR_EOC0_Pos)                      /**< (AFEC_ISR) End of Conversion 0 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC0(value)                  (AFEC_ISR_EOC0_Msk & ((value) << AFEC_ISR_EOC0_Pos))
#define AFEC_ISR_EOC1_Pos                     _U_(1)                                               /**< (AFEC_ISR) End of Conversion 1 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC1_Msk                     (_U_(0x1) << AFEC_ISR_EOC1_Pos)                      /**< (AFEC_ISR) End of Conversion 1 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC1(value)                  (AFEC_ISR_EOC1_Msk & ((value) << AFEC_ISR_EOC1_Pos))
#define AFEC_ISR_EOC2_Pos                     _U_(2)                                               /**< (AFEC_ISR) End of Conversion 2 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC2_Msk                     (_U_(0x1) << AFEC_ISR_EOC2_Pos)                      /**< (AFEC_ISR) End of Conversion 2 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC2(value)                  (AFEC_ISR_EOC2_Msk & ((value) << AFEC_ISR_EOC2_Pos))
#define AFEC_ISR_EOC3_Pos                     _U_(3)                                               /**< (AFEC_ISR) End of Conversion 3 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC3_Msk                     (_U_(0x1) << AFEC_ISR_EOC3_Pos)                      /**< (AFEC_ISR) End of Conversion 3 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC3(value)                  (AFEC_ISR_EOC3_Msk & ((value) << AFEC_ISR_EOC3_Pos))
#define AFEC_ISR_EOC4_Pos                     _U_(4)                                               /**< (AFEC_ISR) End of Conversion 4 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC4_Msk                     (_U_(0x1) << AFEC_ISR_EOC4_Pos)                      /**< (AFEC_ISR) End of Conversion 4 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC4(value)                  (AFEC_ISR_EOC4_Msk & ((value) << AFEC_ISR_EOC4_Pos))
#define AFEC_ISR_EOC5_Pos                     _U_(5)                                               /**< (AFEC_ISR) End of Conversion 5 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC5_Msk                     (_U_(0x1) << AFEC_ISR_EOC5_Pos)                      /**< (AFEC_ISR) End of Conversion 5 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC5(value)                  (AFEC_ISR_EOC5_Msk & ((value) << AFEC_ISR_EOC5_Pos))
#define AFEC_ISR_EOC6_Pos                     _U_(6)                                               /**< (AFEC_ISR) End of Conversion 6 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC6_Msk                     (_U_(0x1) << AFEC_ISR_EOC6_Pos)                      /**< (AFEC_ISR) End of Conversion 6 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC6(value)                  (AFEC_ISR_EOC6_Msk & ((value) << AFEC_ISR_EOC6_Pos))
#define AFEC_ISR_EOC7_Pos                     _U_(7)                                               /**< (AFEC_ISR) End of Conversion 7 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC7_Msk                     (_U_(0x1) << AFEC_ISR_EOC7_Pos)                      /**< (AFEC_ISR) End of Conversion 7 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC7(value)                  (AFEC_ISR_EOC7_Msk & ((value) << AFEC_ISR_EOC7_Pos))
#define AFEC_ISR_EOC8_Pos                     _U_(8)                                               /**< (AFEC_ISR) End of Conversion 8 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC8_Msk                     (_U_(0x1) << AFEC_ISR_EOC8_Pos)                      /**< (AFEC_ISR) End of Conversion 8 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC8(value)                  (AFEC_ISR_EOC8_Msk & ((value) << AFEC_ISR_EOC8_Pos))
#define AFEC_ISR_EOC9_Pos                     _U_(9)                                               /**< (AFEC_ISR) End of Conversion 9 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC9_Msk                     (_U_(0x1) << AFEC_ISR_EOC9_Pos)                      /**< (AFEC_ISR) End of Conversion 9 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC9(value)                  (AFEC_ISR_EOC9_Msk & ((value) << AFEC_ISR_EOC9_Pos))
#define AFEC_ISR_EOC10_Pos                    _U_(10)                                              /**< (AFEC_ISR) End of Conversion 10 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC10_Msk                    (_U_(0x1) << AFEC_ISR_EOC10_Pos)                     /**< (AFEC_ISR) End of Conversion 10 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC10(value)                 (AFEC_ISR_EOC10_Msk & ((value) << AFEC_ISR_EOC10_Pos))
#define AFEC_ISR_EOC11_Pos                    _U_(11)                                              /**< (AFEC_ISR) End of Conversion 11 (cleared by reading AFEC_CDRx) Position */
#define AFEC_ISR_EOC11_Msk                    (_U_(0x1) << AFEC_ISR_EOC11_Pos)                     /**< (AFEC_ISR) End of Conversion 11 (cleared by reading AFEC_CDRx) Mask */
#define AFEC_ISR_EOC11(value)                 (AFEC_ISR_EOC11_Msk & ((value) << AFEC_ISR_EOC11_Pos))
#define AFEC_ISR_DRDY_Pos                     _U_(24)                                              /**< (AFEC_ISR) Data Ready (cleared by reading AFEC_LCDR) Position */
#define AFEC_ISR_DRDY_Msk                     (_U_(0x1) << AFEC_ISR_DRDY_Pos)                      /**< (AFEC_ISR) Data Ready (cleared by reading AFEC_LCDR) Mask */
#define AFEC_ISR_DRDY(value)                  (AFEC_ISR_DRDY_Msk & ((value) << AFEC_ISR_DRDY_Pos))
#define AFEC_ISR_GOVRE_Pos                    _U_(25)                                              /**< (AFEC_ISR) General Overrun Error (cleared by reading AFEC_ISR) Position */
#define AFEC_ISR_GOVRE_Msk                    (_U_(0x1) << AFEC_ISR_GOVRE_Pos)                     /**< (AFEC_ISR) General Overrun Error (cleared by reading AFEC_ISR) Mask */
#define AFEC_ISR_GOVRE(value)                 (AFEC_ISR_GOVRE_Msk & ((value) << AFEC_ISR_GOVRE_Pos))
#define AFEC_ISR_COMPE_Pos                    _U_(26)                                              /**< (AFEC_ISR) Comparison Error (cleared by reading AFEC_ISR) Position */
#define AFEC_ISR_COMPE_Msk                    (_U_(0x1) << AFEC_ISR_COMPE_Pos)                     /**< (AFEC_ISR) Comparison Error (cleared by reading AFEC_ISR) Mask */
#define AFEC_ISR_COMPE(value)                 (AFEC_ISR_COMPE_Msk & ((value) << AFEC_ISR_COMPE_Pos))
#define AFEC_ISR_TEMPCHG_Pos                  _U_(30)                                              /**< (AFEC_ISR) Temperature Change (cleared on read) Position */
#define AFEC_ISR_TEMPCHG_Msk                  (_U_(0x1) << AFEC_ISR_TEMPCHG_Pos)                   /**< (AFEC_ISR) Temperature Change (cleared on read) Mask */
#define AFEC_ISR_TEMPCHG(value)               (AFEC_ISR_TEMPCHG_Msk & ((value) << AFEC_ISR_TEMPCHG_Pos))
#define AFEC_ISR_Msk                          _U_(0x47000FFF)                                      /**< (AFEC_ISR) Register Mask  */

#define AFEC_ISR_EOC_Pos                      _U_(0)                                               /**< (AFEC_ISR Position) End of Conversion x (cleared by reading AFEC_CDRx) */
#define AFEC_ISR_EOC_Msk                      (_U_(0xFFF) << AFEC_ISR_EOC_Pos)                     /**< (AFEC_ISR Mask) EOC */
#define AFEC_ISR_EOC(value)                   (AFEC_ISR_EOC_Msk & ((value) << AFEC_ISR_EOC_Pos))   

/* -------- AFEC_OVER : (AFEC Offset: 0x4C) ( R/ 32) AFEC Overrun Status Register -------- */
#define AFEC_OVER_OVRE0_Pos                   _U_(0)                                               /**< (AFEC_OVER) Overrun Error 0 Position */
#define AFEC_OVER_OVRE0_Msk                   (_U_(0x1) << AFEC_OVER_OVRE0_Pos)                    /**< (AFEC_OVER) Overrun Error 0 Mask */
#define AFEC_OVER_OVRE0(value)                (AFEC_OVER_OVRE0_Msk & ((value) << AFEC_OVER_OVRE0_Pos))
#define AFEC_OVER_OVRE1_Pos                   _U_(1)                                               /**< (AFEC_OVER) Overrun Error 1 Position */
#define AFEC_OVER_OVRE1_Msk                   (_U_(0x1) << AFEC_OVER_OVRE1_Pos)                    /**< (AFEC_OVER) Overrun Error 1 Mask */
#define AFEC_OVER_OVRE1(value)                (AFEC_OVER_OVRE1_Msk & ((value) << AFEC_OVER_OVRE1_Pos))
#define AFEC_OVER_OVRE2_Pos                   _U_(2)                                               /**< (AFEC_OVER) Overrun Error 2 Position */
#define AFEC_OVER_OVRE2_Msk                   (_U_(0x1) << AFEC_OVER_OVRE2_Pos)                    /**< (AFEC_OVER) Overrun Error 2 Mask */
#define AFEC_OVER_OVRE2(value)                (AFEC_OVER_OVRE2_Msk & ((value) << AFEC_OVER_OVRE2_Pos))
#define AFEC_OVER_OVRE3_Pos                   _U_(3)                                               /**< (AFEC_OVER) Overrun Error 3 Position */
#define AFEC_OVER_OVRE3_Msk                   (_U_(0x1) << AFEC_OVER_OVRE3_Pos)                    /**< (AFEC_OVER) Overrun Error 3 Mask */
#define AFEC_OVER_OVRE3(value)                (AFEC_OVER_OVRE3_Msk & ((value) << AFEC_OVER_OVRE3_Pos))
#define AFEC_OVER_OVRE4_Pos                   _U_(4)                                               /**< (AFEC_OVER) Overrun Error 4 Position */
#define AFEC_OVER_OVRE4_Msk                   (_U_(0x1) << AFEC_OVER_OVRE4_Pos)                    /**< (AFEC_OVER) Overrun Error 4 Mask */
#define AFEC_OVER_OVRE4(value)                (AFEC_OVER_OVRE4_Msk & ((value) << AFEC_OVER_OVRE4_Pos))
#define AFEC_OVER_OVRE5_Pos                   _U_(5)                                               /**< (AFEC_OVER) Overrun Error 5 Position */
#define AFEC_OVER_OVRE5_Msk                   (_U_(0x1) << AFEC_OVER_OVRE5_Pos)                    /**< (AFEC_OVER) Overrun Error 5 Mask */
#define AFEC_OVER_OVRE5(value)                (AFEC_OVER_OVRE5_Msk & ((value) << AFEC_OVER_OVRE5_Pos))
#define AFEC_OVER_OVRE6_Pos                   _U_(6)                                               /**< (AFEC_OVER) Overrun Error 6 Position */
#define AFEC_OVER_OVRE6_Msk                   (_U_(0x1) << AFEC_OVER_OVRE6_Pos)                    /**< (AFEC_OVER) Overrun Error 6 Mask */
#define AFEC_OVER_OVRE6(value)                (AFEC_OVER_OVRE6_Msk & ((value) << AFEC_OVER_OVRE6_Pos))
#define AFEC_OVER_OVRE7_Pos                   _U_(7)                                               /**< (AFEC_OVER) Overrun Error 7 Position */
#define AFEC_OVER_OVRE7_Msk                   (_U_(0x1) << AFEC_OVER_OVRE7_Pos)                    /**< (AFEC_OVER) Overrun Error 7 Mask */
#define AFEC_OVER_OVRE7(value)                (AFEC_OVER_OVRE7_Msk & ((value) << AFEC_OVER_OVRE7_Pos))
#define AFEC_OVER_OVRE8_Pos                   _U_(8)                                               /**< (AFEC_OVER) Overrun Error 8 Position */
#define AFEC_OVER_OVRE8_Msk                   (_U_(0x1) << AFEC_OVER_OVRE8_Pos)                    /**< (AFEC_OVER) Overrun Error 8 Mask */
#define AFEC_OVER_OVRE8(value)                (AFEC_OVER_OVRE8_Msk & ((value) << AFEC_OVER_OVRE8_Pos))
#define AFEC_OVER_OVRE9_Pos                   _U_(9)                                               /**< (AFEC_OVER) Overrun Error 9 Position */
#define AFEC_OVER_OVRE9_Msk                   (_U_(0x1) << AFEC_OVER_OVRE9_Pos)                    /**< (AFEC_OVER) Overrun Error 9 Mask */
#define AFEC_OVER_OVRE9(value)                (AFEC_OVER_OVRE9_Msk & ((value) << AFEC_OVER_OVRE9_Pos))
#define AFEC_OVER_OVRE10_Pos                  _U_(10)                                              /**< (AFEC_OVER) Overrun Error 10 Position */
#define AFEC_OVER_OVRE10_Msk                  (_U_(0x1) << AFEC_OVER_OVRE10_Pos)                   /**< (AFEC_OVER) Overrun Error 10 Mask */
#define AFEC_OVER_OVRE10(value)               (AFEC_OVER_OVRE10_Msk & ((value) << AFEC_OVER_OVRE10_Pos))
#define AFEC_OVER_OVRE11_Pos                  _U_(11)                                              /**< (AFEC_OVER) Overrun Error 11 Position */
#define AFEC_OVER_OVRE11_Msk                  (_U_(0x1) << AFEC_OVER_OVRE11_Pos)                   /**< (AFEC_OVER) Overrun Error 11 Mask */
#define AFEC_OVER_OVRE11(value)               (AFEC_OVER_OVRE11_Msk & ((value) << AFEC_OVER_OVRE11_Pos))
#define AFEC_OVER_Msk                         _U_(0x00000FFF)                                      /**< (AFEC_OVER) Register Mask  */

#define AFEC_OVER_OVRE_Pos                    _U_(0)                                               /**< (AFEC_OVER Position) Overrun Error xx */
#define AFEC_OVER_OVRE_Msk                    (_U_(0xFFF) << AFEC_OVER_OVRE_Pos)                   /**< (AFEC_OVER Mask) OVRE */
#define AFEC_OVER_OVRE(value)                 (AFEC_OVER_OVRE_Msk & ((value) << AFEC_OVER_OVRE_Pos)) 

/* -------- AFEC_CWR : (AFEC Offset: 0x50) (R/W 32) AFEC Compare Window Register -------- */
#define AFEC_CWR_LOWTHRES_Pos                 _U_(0)                                               /**< (AFEC_CWR) Low Threshold Position */
#define AFEC_CWR_LOWTHRES_Msk                 (_U_(0xFFFF) << AFEC_CWR_LOWTHRES_Pos)               /**< (AFEC_CWR) Low Threshold Mask */
#define AFEC_CWR_LOWTHRES(value)              (AFEC_CWR_LOWTHRES_Msk & ((value) << AFEC_CWR_LOWTHRES_Pos))
#define AFEC_CWR_HIGHTHRES_Pos                _U_(16)                                              /**< (AFEC_CWR) High Threshold Position */
#define AFEC_CWR_HIGHTHRES_Msk                (_U_(0xFFFF) << AFEC_CWR_HIGHTHRES_Pos)              /**< (AFEC_CWR) High Threshold Mask */
#define AFEC_CWR_HIGHTHRES(value)             (AFEC_CWR_HIGHTHRES_Msk & ((value) << AFEC_CWR_HIGHTHRES_Pos))
#define AFEC_CWR_Msk                          _U_(0xFFFFFFFF)                                      /**< (AFEC_CWR) Register Mask  */


/* -------- AFEC_CGR : (AFEC Offset: 0x54) (R/W 32) AFEC Channel Gain Register -------- */
#define AFEC_CGR_GAIN0_Pos                    _U_(0)                                               /**< (AFEC_CGR) Gain for Channel 0 Position */
#define AFEC_CGR_GAIN0_Msk                    (_U_(0x3) << AFEC_CGR_GAIN0_Pos)                     /**< (AFEC_CGR) Gain for Channel 0 Mask */
#define AFEC_CGR_GAIN0(value)                 (AFEC_CGR_GAIN0_Msk & ((value) << AFEC_CGR_GAIN0_Pos))
#define AFEC_CGR_GAIN1_Pos                    _U_(2)                                               /**< (AFEC_CGR) Gain for Channel 1 Position */
#define AFEC_CGR_GAIN1_Msk                    (_U_(0x3) << AFEC_CGR_GAIN1_Pos)                     /**< (AFEC_CGR) Gain for Channel 1 Mask */
#define AFEC_CGR_GAIN1(value)                 (AFEC_CGR_GAIN1_Msk & ((value) << AFEC_CGR_GAIN1_Pos))
#define AFEC_CGR_GAIN2_Pos                    _U_(4)                                               /**< (AFEC_CGR) Gain for Channel 2 Position */
#define AFEC_CGR_GAIN2_Msk                    (_U_(0x3) << AFEC_CGR_GAIN2_Pos)                     /**< (AFEC_CGR) Gain for Channel 2 Mask */
#define AFEC_CGR_GAIN2(value)                 (AFEC_CGR_GAIN2_Msk & ((value) << AFEC_CGR_GAIN2_Pos))
#define AFEC_CGR_GAIN3_Pos                    _U_(6)                                               /**< (AFEC_CGR) Gain for Channel 3 Position */
#define AFEC_CGR_GAIN3_Msk                    (_U_(0x3) << AFEC_CGR_GAIN3_Pos)                     /**< (AFEC_CGR) Gain for Channel 3 Mask */
#define AFEC_CGR_GAIN3(value)                 (AFEC_CGR_GAIN3_Msk & ((value) << AFEC_CGR_GAIN3_Pos))
#define AFEC_CGR_GAIN4_Pos                    _U_(8)                                               /**< (AFEC_CGR) Gain for Channel 4 Position */
#define AFEC_CGR_GAIN4_Msk                    (_U_(0x3) << AFEC_CGR_GAIN4_Pos)                     /**< (AFEC_CGR) Gain for Channel 4 Mask */
#define AFEC_CGR_GAIN4(value)                 (AFEC_CGR_GAIN4_Msk & ((value) << AFEC_CGR_GAIN4_Pos))
#define AFEC_CGR_GAIN5_Pos                    _U_(10)                                              /**< (AFEC_CGR) Gain for Channel 5 Position */
#define AFEC_CGR_GAIN5_Msk                    (_U_(0x3) << AFEC_CGR_GAIN5_Pos)                     /**< (AFEC_CGR) Gain for Channel 5 Mask */
#define AFEC_CGR_GAIN5(value)                 (AFEC_CGR_GAIN5_Msk & ((value) << AFEC_CGR_GAIN5_Pos))
#define AFEC_CGR_GAIN6_Pos                    _U_(12)                                              /**< (AFEC_CGR) Gain for Channel 6 Position */
#define AFEC_CGR_GAIN6_Msk                    (_U_(0x3) << AFEC_CGR_GAIN6_Pos)                     /**< (AFEC_CGR) Gain for Channel 6 Mask */
#define AFEC_CGR_GAIN6(value)                 (AFEC_CGR_GAIN6_Msk & ((value) << AFEC_CGR_GAIN6_Pos))
#define AFEC_CGR_GAIN7_Pos                    _U_(14)                                              /**< (AFEC_CGR) Gain for Channel 7 Position */
#define AFEC_CGR_GAIN7_Msk                    (_U_(0x3) << AFEC_CGR_GAIN7_Pos)                     /**< (AFEC_CGR) Gain for Channel 7 Mask */
#define AFEC_CGR_GAIN7(value)                 (AFEC_CGR_GAIN7_Msk & ((value) << AFEC_CGR_GAIN7_Pos))
#define AFEC_CGR_GAIN8_Pos                    _U_(16)                                              /**< (AFEC_CGR) Gain for Channel 8 Position */
#define AFEC_CGR_GAIN8_Msk                    (_U_(0x3) << AFEC_CGR_GAIN8_Pos)                     /**< (AFEC_CGR) Gain for Channel 8 Mask */
#define AFEC_CGR_GAIN8(value)                 (AFEC_CGR_GAIN8_Msk & ((value) << AFEC_CGR_GAIN8_Pos))
#define AFEC_CGR_GAIN9_Pos                    _U_(18)                                              /**< (AFEC_CGR) Gain for Channel 9 Position */
#define AFEC_CGR_GAIN9_Msk                    (_U_(0x3) << AFEC_CGR_GAIN9_Pos)                     /**< (AFEC_CGR) Gain for Channel 9 Mask */
#define AFEC_CGR_GAIN9(value)                 (AFEC_CGR_GAIN9_Msk & ((value) << AFEC_CGR_GAIN9_Pos))
#define AFEC_CGR_GAIN10_Pos                   _U_(20)                                              /**< (AFEC_CGR) Gain for Channel 10 Position */
#define AFEC_CGR_GAIN10_Msk                   (_U_(0x3) << AFEC_CGR_GAIN10_Pos)                    /**< (AFEC_CGR) Gain for Channel 10 Mask */
#define AFEC_CGR_GAIN10(value)                (AFEC_CGR_GAIN10_Msk & ((value) << AFEC_CGR_GAIN10_Pos))
#define AFEC_CGR_GAIN11_Pos                   _U_(22)                                              /**< (AFEC_CGR) Gain for Channel 11 Position */
#define AFEC_CGR_GAIN11_Msk                   (_U_(0x3) << AFEC_CGR_GAIN11_Pos)                    /**< (AFEC_CGR) Gain for Channel 11 Mask */
#define AFEC_CGR_GAIN11(value)                (AFEC_CGR_GAIN11_Msk & ((value) << AFEC_CGR_GAIN11_Pos))
#define AFEC_CGR_Msk                          _U_(0x00FFFFFF)                                      /**< (AFEC_CGR) Register Mask  */


/* -------- AFEC_DIFFR : (AFEC Offset: 0x60) (R/W 32) AFEC Channel Differential Register -------- */
#define AFEC_DIFFR_DIFF0_Pos                  _U_(0)                                               /**< (AFEC_DIFFR) Differential inputs for channel 0 Position */
#define AFEC_DIFFR_DIFF0_Msk                  (_U_(0x1) << AFEC_DIFFR_DIFF0_Pos)                   /**< (AFEC_DIFFR) Differential inputs for channel 0 Mask */
#define AFEC_DIFFR_DIFF0(value)               (AFEC_DIFFR_DIFF0_Msk & ((value) << AFEC_DIFFR_DIFF0_Pos))
#define AFEC_DIFFR_DIFF1_Pos                  _U_(1)                                               /**< (AFEC_DIFFR) Differential inputs for channel 1 Position */
#define AFEC_DIFFR_DIFF1_Msk                  (_U_(0x1) << AFEC_DIFFR_DIFF1_Pos)                   /**< (AFEC_DIFFR) Differential inputs for channel 1 Mask */
#define AFEC_DIFFR_DIFF1(value)               (AFEC_DIFFR_DIFF1_Msk & ((value) << AFEC_DIFFR_DIFF1_Pos))
#define AFEC_DIFFR_DIFF2_Pos                  _U_(2)                                               /**< (AFEC_DIFFR) Differential inputs for channel 2 Position */
#define AFEC_DIFFR_DIFF2_Msk                  (_U_(0x1) << AFEC_DIFFR_DIFF2_Pos)                   /**< (AFEC_DIFFR) Differential inputs for channel 2 Mask */
#define AFEC_DIFFR_DIFF2(value)               (AFEC_DIFFR_DIFF2_Msk & ((value) << AFEC_DIFFR_DIFF2_Pos))
#define AFEC_DIFFR_DIFF3_Pos                  _U_(3)                                               /**< (AFEC_DIFFR) Differential inputs for channel 3 Position */
#define AFEC_DIFFR_DIFF3_Msk                  (_U_(0x1) << AFEC_DIFFR_DIFF3_Pos)                   /**< (AFEC_DIFFR) Differential inputs for channel 3 Mask */
#define AFEC_DIFFR_DIFF3(value)               (AFEC_DIFFR_DIFF3_Msk & ((value) << AFEC_DIFFR_DIFF3_Pos))
#define AFEC_DIFFR_DIFF4_Pos                  _U_(4)                                               /**< (AFEC_DIFFR) Differential inputs for channel 4 Position */
#define AFEC_DIFFR_DIFF4_Msk                  (_U_(0x1) << AFEC_DIFFR_DIFF4_Pos)                   /**< (AFEC_DIFFR) Differential inputs for channel 4 Mask */
#define AFEC_DIFFR_DIFF4(value)               (AFEC_DIFFR_DIFF4_Msk & ((value) << AFEC_DIFFR_DIFF4_Pos))
#define AFEC_DIFFR_DIFF5_Pos                  _U_(5)                                               /**< (AFEC_DIFFR) Differential inputs for channel 5 Position */
#define AFEC_DIFFR_DIFF5_Msk                  (_U_(0x1) << AFEC_DIFFR_DIFF5_Pos)                   /**< (AFEC_DIFFR) Differential inputs for channel 5 Mask */
#define AFEC_DIFFR_DIFF5(value)               (AFEC_DIFFR_DIFF5_Msk & ((value) << AFEC_DIFFR_DIFF5_Pos))
#define AFEC_DIFFR_DIFF6_Pos                  _U_(6)                                               /**< (AFEC_DIFFR) Differential inputs for channel 6 Position */
#define AFEC_DIFFR_DIFF6_Msk                  (_U_(0x1) << AFEC_DIFFR_DIFF6_Pos)                   /**< (AFEC_DIFFR) Differential inputs for channel 6 Mask */
#define AFEC_DIFFR_DIFF6(value)               (AFEC_DIFFR_DIFF6_Msk & ((value) << AFEC_DIFFR_DIFF6_Pos))
#define AFEC_DIFFR_DIFF7_Pos                  _U_(7)                                               /**< (AFEC_DIFFR) Differential inputs for channel 7 Position */
#define AFEC_DIFFR_DIFF7_Msk                  (_U_(0x1) << AFEC_DIFFR_DIFF7_Pos)                   /**< (AFEC_DIFFR) Differential inputs for channel 7 Mask */
#define AFEC_DIFFR_DIFF7(value)               (AFEC_DIFFR_DIFF7_Msk & ((value) << AFEC_DIFFR_DIFF7_Pos))
#define AFEC_DIFFR_DIFF8_Pos                  _U_(8)                                               /**< (AFEC_DIFFR) Differential inputs for channel 8 Position */
#define AFEC_DIFFR_DIFF8_Msk                  (_U_(0x1) << AFEC_DIFFR_DIFF8_Pos)                   /**< (AFEC_DIFFR) Differential inputs for channel 8 Mask */
#define AFEC_DIFFR_DIFF8(value)               (AFEC_DIFFR_DIFF8_Msk & ((value) << AFEC_DIFFR_DIFF8_Pos))
#define AFEC_DIFFR_DIFF9_Pos                  _U_(9)                                               /**< (AFEC_DIFFR) Differential inputs for channel 9 Position */
#define AFEC_DIFFR_DIFF9_Msk                  (_U_(0x1) << AFEC_DIFFR_DIFF9_Pos)                   /**< (AFEC_DIFFR) Differential inputs for channel 9 Mask */
#define AFEC_DIFFR_DIFF9(value)               (AFEC_DIFFR_DIFF9_Msk & ((value) << AFEC_DIFFR_DIFF9_Pos))
#define AFEC_DIFFR_DIFF10_Pos                 _U_(10)                                              /**< (AFEC_DIFFR) Differential inputs for channel 10 Position */
#define AFEC_DIFFR_DIFF10_Msk                 (_U_(0x1) << AFEC_DIFFR_DIFF10_Pos)                  /**< (AFEC_DIFFR) Differential inputs for channel 10 Mask */
#define AFEC_DIFFR_DIFF10(value)              (AFEC_DIFFR_DIFF10_Msk & ((value) << AFEC_DIFFR_DIFF10_Pos))
#define AFEC_DIFFR_DIFF11_Pos                 _U_(11)                                              /**< (AFEC_DIFFR) Differential inputs for channel 11 Position */
#define AFEC_DIFFR_DIFF11_Msk                 (_U_(0x1) << AFEC_DIFFR_DIFF11_Pos)                  /**< (AFEC_DIFFR) Differential inputs for channel 11 Mask */
#define AFEC_DIFFR_DIFF11(value)              (AFEC_DIFFR_DIFF11_Msk & ((value) << AFEC_DIFFR_DIFF11_Pos))
#define AFEC_DIFFR_Msk                        _U_(0x00000FFF)                                      /**< (AFEC_DIFFR) Register Mask  */

#define AFEC_DIFFR_DIFF_Pos                   _U_(0)                                               /**< (AFEC_DIFFR Position) Differential inputs for channel xx */
#define AFEC_DIFFR_DIFF_Msk                   (_U_(0xFFF) << AFEC_DIFFR_DIFF_Pos)                  /**< (AFEC_DIFFR Mask) DIFF */
#define AFEC_DIFFR_DIFF(value)                (AFEC_DIFFR_DIFF_Msk & ((value) << AFEC_DIFFR_DIFF_Pos)) 

/* -------- AFEC_CSELR : (AFEC Offset: 0x64) (R/W 32) AFEC Channel Selection Register -------- */
#define AFEC_CSELR_CSEL_Pos                   _U_(0)                                               /**< (AFEC_CSELR) Channel Selection Position */
#define AFEC_CSELR_CSEL_Msk                   (_U_(0xF) << AFEC_CSELR_CSEL_Pos)                    /**< (AFEC_CSELR) Channel Selection Mask */
#define AFEC_CSELR_CSEL(value)                (AFEC_CSELR_CSEL_Msk & ((value) << AFEC_CSELR_CSEL_Pos))
#define AFEC_CSELR_Msk                        _U_(0x0000000F)                                      /**< (AFEC_CSELR) Register Mask  */


/* -------- AFEC_CDR : (AFEC Offset: 0x68) ( R/ 32) AFEC Channel Data Register -------- */
#define AFEC_CDR_DATA_Pos                     _U_(0)                                               /**< (AFEC_CDR) Converted Data Position */
#define AFEC_CDR_DATA_Msk                     (_U_(0xFFFF) << AFEC_CDR_DATA_Pos)                   /**< (AFEC_CDR) Converted Data Mask */
#define AFEC_CDR_DATA(value)                  (AFEC_CDR_DATA_Msk & ((value) << AFEC_CDR_DATA_Pos))
#define AFEC_CDR_Msk                          _U_(0x0000FFFF)                                      /**< (AFEC_CDR) Register Mask  */


/* -------- AFEC_COCR : (AFEC Offset: 0x6C) (R/W 32) AFEC Channel Offset Compensation Register -------- */
#define AFEC_COCR_AOFF_Pos                    _U_(0)                                               /**< (AFEC_COCR) Analog Offset Position */
#define AFEC_COCR_AOFF_Msk                    (_U_(0x3FF) << AFEC_COCR_AOFF_Pos)                   /**< (AFEC_COCR) Analog Offset Mask */
#define AFEC_COCR_AOFF(value)                 (AFEC_COCR_AOFF_Msk & ((value) << AFEC_COCR_AOFF_Pos))
#define AFEC_COCR_Msk                         _U_(0x000003FF)                                      /**< (AFEC_COCR) Register Mask  */


/* -------- AFEC_TEMPMR : (AFEC Offset: 0x70) (R/W 32) AFEC Temperature Sensor Mode Register -------- */
#define AFEC_TEMPMR_RTCT_Pos                  _U_(0)                                               /**< (AFEC_TEMPMR) Temperature Sensor RTC Trigger Mode Position */
#define AFEC_TEMPMR_RTCT_Msk                  (_U_(0x1) << AFEC_TEMPMR_RTCT_Pos)                   /**< (AFEC_TEMPMR) Temperature Sensor RTC Trigger Mode Mask */
#define AFEC_TEMPMR_RTCT(value)               (AFEC_TEMPMR_RTCT_Msk & ((value) << AFEC_TEMPMR_RTCT_Pos))
#define AFEC_TEMPMR_TEMPCMPMOD_Pos            _U_(4)                                               /**< (AFEC_TEMPMR) Temperature Comparison Mode Position */
#define AFEC_TEMPMR_TEMPCMPMOD_Msk            (_U_(0x3) << AFEC_TEMPMR_TEMPCMPMOD_Pos)             /**< (AFEC_TEMPMR) Temperature Comparison Mode Mask */
#define AFEC_TEMPMR_TEMPCMPMOD(value)         (AFEC_TEMPMR_TEMPCMPMOD_Msk & ((value) << AFEC_TEMPMR_TEMPCMPMOD_Pos))
#define   AFEC_TEMPMR_TEMPCMPMOD_LOW_Val      _U_(0x0)                                             /**< (AFEC_TEMPMR) Generates an event when the converted data is lower than the low threshold of the window.  */
#define   AFEC_TEMPMR_TEMPCMPMOD_HIGH_Val     _U_(0x1)                                             /**< (AFEC_TEMPMR) Generates an event when the converted data is higher than the high threshold of the window.  */
#define   AFEC_TEMPMR_TEMPCMPMOD_IN_Val       _U_(0x2)                                             /**< (AFEC_TEMPMR) Generates an event when the converted data is in the comparison window.  */
#define   AFEC_TEMPMR_TEMPCMPMOD_OUT_Val      _U_(0x3)                                             /**< (AFEC_TEMPMR) Generates an event when the converted data is out of the comparison window.  */
#define AFEC_TEMPMR_TEMPCMPMOD_LOW            (AFEC_TEMPMR_TEMPCMPMOD_LOW_Val << AFEC_TEMPMR_TEMPCMPMOD_Pos) /**< (AFEC_TEMPMR) Generates an event when the converted data is lower than the low threshold of the window. Position  */
#define AFEC_TEMPMR_TEMPCMPMOD_HIGH           (AFEC_TEMPMR_TEMPCMPMOD_HIGH_Val << AFEC_TEMPMR_TEMPCMPMOD_Pos) /**< (AFEC_TEMPMR) Generates an event when the converted data is higher than the high threshold of the window. Position  */
#define AFEC_TEMPMR_TEMPCMPMOD_IN             (AFEC_TEMPMR_TEMPCMPMOD_IN_Val << AFEC_TEMPMR_TEMPCMPMOD_Pos) /**< (AFEC_TEMPMR) Generates an event when the converted data is in the comparison window. Position  */
#define AFEC_TEMPMR_TEMPCMPMOD_OUT            (AFEC_TEMPMR_TEMPCMPMOD_OUT_Val << AFEC_TEMPMR_TEMPCMPMOD_Pos) /**< (AFEC_TEMPMR) Generates an event when the converted data is out of the comparison window. Position  */
#define AFEC_TEMPMR_Msk                       _U_(0x00000031)                                      /**< (AFEC_TEMPMR) Register Mask  */


/* -------- AFEC_TEMPCWR : (AFEC Offset: 0x74) (R/W 32) AFEC Temperature Compare Window Register -------- */
#define AFEC_TEMPCWR_TLOWTHRES_Pos            _U_(0)                                               /**< (AFEC_TEMPCWR) Temperature Low Threshold Position */
#define AFEC_TEMPCWR_TLOWTHRES_Msk            (_U_(0xFFFF) << AFEC_TEMPCWR_TLOWTHRES_Pos)          /**< (AFEC_TEMPCWR) Temperature Low Threshold Mask */
#define AFEC_TEMPCWR_TLOWTHRES(value)         (AFEC_TEMPCWR_TLOWTHRES_Msk & ((value) << AFEC_TEMPCWR_TLOWTHRES_Pos))
#define AFEC_TEMPCWR_THIGHTHRES_Pos           _U_(16)                                              /**< (AFEC_TEMPCWR) Temperature High Threshold Position */
#define AFEC_TEMPCWR_THIGHTHRES_Msk           (_U_(0xFFFF) << AFEC_TEMPCWR_THIGHTHRES_Pos)         /**< (AFEC_TEMPCWR) Temperature High Threshold Mask */
#define AFEC_TEMPCWR_THIGHTHRES(value)        (AFEC_TEMPCWR_THIGHTHRES_Msk & ((value) << AFEC_TEMPCWR_THIGHTHRES_Pos))
#define AFEC_TEMPCWR_Msk                      _U_(0xFFFFFFFF)                                      /**< (AFEC_TEMPCWR) Register Mask  */


/* -------- AFEC_ACR : (AFEC Offset: 0x94) (R/W 32) AFEC Analog Control Register -------- */
#define AFEC_ACR_PGA0EN_Pos                   _U_(2)                                               /**< (AFEC_ACR) PGA0 Enable Position */
#define AFEC_ACR_PGA0EN_Msk                   (_U_(0x1) << AFEC_ACR_PGA0EN_Pos)                    /**< (AFEC_ACR) PGA0 Enable Mask */
#define AFEC_ACR_PGA0EN(value)                (AFEC_ACR_PGA0EN_Msk & ((value) << AFEC_ACR_PGA0EN_Pos))
#define AFEC_ACR_PGA1EN_Pos                   _U_(3)                                               /**< (AFEC_ACR) PGA1 Enable Position */
#define AFEC_ACR_PGA1EN_Msk                   (_U_(0x1) << AFEC_ACR_PGA1EN_Pos)                    /**< (AFEC_ACR) PGA1 Enable Mask */
#define AFEC_ACR_PGA1EN(value)                (AFEC_ACR_PGA1EN_Msk & ((value) << AFEC_ACR_PGA1EN_Pos))
#define AFEC_ACR_IBCTL_Pos                    _U_(8)                                               /**< (AFEC_ACR) AFE Bias Current Control Position */
#define AFEC_ACR_IBCTL_Msk                    (_U_(0x3) << AFEC_ACR_IBCTL_Pos)                     /**< (AFEC_ACR) AFE Bias Current Control Mask */
#define AFEC_ACR_IBCTL(value)                 (AFEC_ACR_IBCTL_Msk & ((value) << AFEC_ACR_IBCTL_Pos))
#define AFEC_ACR_Msk                          _U_(0x0000030C)                                      /**< (AFEC_ACR) Register Mask  */


/* -------- AFEC_SHMR : (AFEC Offset: 0xA0) (R/W 32) AFEC Sample & Hold Mode Register -------- */
#define AFEC_SHMR_DUAL0_Pos                   _U_(0)                                               /**< (AFEC_SHMR) Dual Sample & Hold for channel 0 Position */
#define AFEC_SHMR_DUAL0_Msk                   (_U_(0x1) << AFEC_SHMR_DUAL0_Pos)                    /**< (AFEC_SHMR) Dual Sample & Hold for channel 0 Mask */
#define AFEC_SHMR_DUAL0(value)                (AFEC_SHMR_DUAL0_Msk & ((value) << AFEC_SHMR_DUAL0_Pos))
#define AFEC_SHMR_DUAL1_Pos                   _U_(1)                                               /**< (AFEC_SHMR) Dual Sample & Hold for channel 1 Position */
#define AFEC_SHMR_DUAL1_Msk                   (_U_(0x1) << AFEC_SHMR_DUAL1_Pos)                    /**< (AFEC_SHMR) Dual Sample & Hold for channel 1 Mask */
#define AFEC_SHMR_DUAL1(value)                (AFEC_SHMR_DUAL1_Msk & ((value) << AFEC_SHMR_DUAL1_Pos))
#define AFEC_SHMR_DUAL2_Pos                   _U_(2)                                               /**< (AFEC_SHMR) Dual Sample & Hold for channel 2 Position */
#define AFEC_SHMR_DUAL2_Msk                   (_U_(0x1) << AFEC_SHMR_DUAL2_Pos)                    /**< (AFEC_SHMR) Dual Sample & Hold for channel 2 Mask */
#define AFEC_SHMR_DUAL2(value)                (AFEC_SHMR_DUAL2_Msk & ((value) << AFEC_SHMR_DUAL2_Pos))
#define AFEC_SHMR_DUAL3_Pos                   _U_(3)                                               /**< (AFEC_SHMR) Dual Sample & Hold for channel 3 Position */
#define AFEC_SHMR_DUAL3_Msk                   (_U_(0x1) << AFEC_SHMR_DUAL3_Pos)                    /**< (AFEC_SHMR) Dual Sample & Hold for channel 3 Mask */
#define AFEC_SHMR_DUAL3(value)                (AFEC_SHMR_DUAL3_Msk & ((value) << AFEC_SHMR_DUAL3_Pos))
#define AFEC_SHMR_DUAL4_Pos                   _U_(4)                                               /**< (AFEC_SHMR) Dual Sample & Hold for channel 4 Position */
#define AFEC_SHMR_DUAL4_Msk                   (_U_(0x1) << AFEC_SHMR_DUAL4_Pos)                    /**< (AFEC_SHMR) Dual Sample & Hold for channel 4 Mask */
#define AFEC_SHMR_DUAL4(value)                (AFEC_SHMR_DUAL4_Msk & ((value) << AFEC_SHMR_DUAL4_Pos))
#define AFEC_SHMR_DUAL5_Pos                   _U_(5)                                               /**< (AFEC_SHMR) Dual Sample & Hold for channel 5 Position */
#define AFEC_SHMR_DUAL5_Msk                   (_U_(0x1) << AFEC_SHMR_DUAL5_Pos)                    /**< (AFEC_SHMR) Dual Sample & Hold for channel 5 Mask */
#define AFEC_SHMR_DUAL5(value)                (AFEC_SHMR_DUAL5_Msk & ((value) << AFEC_SHMR_DUAL5_Pos))
#define AFEC_SHMR_DUAL6_Pos                   _U_(6)                                               /**< (AFEC_SHMR) Dual Sample & Hold for channel 6 Position */
#define AFEC_SHMR_DUAL6_Msk                   (_U_(0x1) << AFEC_SHMR_DUAL6_Pos)                    /**< (AFEC_SHMR) Dual Sample & Hold for channel 6 Mask */
#define AFEC_SHMR_DUAL6(value)                (AFEC_SHMR_DUAL6_Msk & ((value) << AFEC_SHMR_DUAL6_Pos))
#define AFEC_SHMR_DUAL7_Pos                   _U_(7)                                               /**< (AFEC_SHMR) Dual Sample & Hold for channel 7 Position */
#define AFEC_SHMR_DUAL7_Msk                   (_U_(0x1) << AFEC_SHMR_DUAL7_Pos)                    /**< (AFEC_SHMR) Dual Sample & Hold for channel 7 Mask */
#define AFEC_SHMR_DUAL7(value)                (AFEC_SHMR_DUAL7_Msk & ((value) << AFEC_SHMR_DUAL7_Pos))
#define AFEC_SHMR_DUAL8_Pos                   _U_(8)                                               /**< (AFEC_SHMR) Dual Sample & Hold for channel 8 Position */
#define AFEC_SHMR_DUAL8_Msk                   (_U_(0x1) << AFEC_SHMR_DUAL8_Pos)                    /**< (AFEC_SHMR) Dual Sample & Hold for channel 8 Mask */
#define AFEC_SHMR_DUAL8(value)                (AFEC_SHMR_DUAL8_Msk & ((value) << AFEC_SHMR_DUAL8_Pos))
#define AFEC_SHMR_DUAL9_Pos                   _U_(9)                                               /**< (AFEC_SHMR) Dual Sample & Hold for channel 9 Position */
#define AFEC_SHMR_DUAL9_Msk                   (_U_(0x1) << AFEC_SHMR_DUAL9_Pos)                    /**< (AFEC_SHMR) Dual Sample & Hold for channel 9 Mask */
#define AFEC_SHMR_DUAL9(value)                (AFEC_SHMR_DUAL9_Msk & ((value) << AFEC_SHMR_DUAL9_Pos))
#define AFEC_SHMR_DUAL10_Pos                  _U_(10)                                              /**< (AFEC_SHMR) Dual Sample & Hold for channel 10 Position */
#define AFEC_SHMR_DUAL10_Msk                  (_U_(0x1) << AFEC_SHMR_DUAL10_Pos)                   /**< (AFEC_SHMR) Dual Sample & Hold for channel 10 Mask */
#define AFEC_SHMR_DUAL10(value)               (AFEC_SHMR_DUAL10_Msk & ((value) << AFEC_SHMR_DUAL10_Pos))
#define AFEC_SHMR_DUAL11_Pos                  _U_(11)                                              /**< (AFEC_SHMR) Dual Sample & Hold for channel 11 Position */
#define AFEC_SHMR_DUAL11_Msk                  (_U_(0x1) << AFEC_SHMR_DUAL11_Pos)                   /**< (AFEC_SHMR) Dual Sample & Hold for channel 11 Mask */
#define AFEC_SHMR_DUAL11(value)               (AFEC_SHMR_DUAL11_Msk & ((value) << AFEC_SHMR_DUAL11_Pos))
#define AFEC_SHMR_Msk                         _U_(0x00000FFF)                                      /**< (AFEC_SHMR) Register Mask  */

#define AFEC_SHMR_DUAL_Pos                    _U_(0)                                               /**< (AFEC_SHMR Position) Dual Sample & Hold for channel xx */
#define AFEC_SHMR_DUAL_Msk                    (_U_(0xFFF) << AFEC_SHMR_DUAL_Pos)                   /**< (AFEC_SHMR Mask) DUAL */
#define AFEC_SHMR_DUAL(value)                 (AFEC_SHMR_DUAL_Msk & ((value) << AFEC_SHMR_DUAL_Pos)) 

/* -------- AFEC_COSR : (AFEC Offset: 0xD0) (R/W 32) AFEC Correction Select Register -------- */
#define AFEC_COSR_CSEL_Pos                    _U_(0)                                               /**< (AFEC_COSR) Sample & Hold unit Correction Select Position */
#define AFEC_COSR_CSEL_Msk                    (_U_(0x1) << AFEC_COSR_CSEL_Pos)                     /**< (AFEC_COSR) Sample & Hold unit Correction Select Mask */
#define AFEC_COSR_CSEL(value)                 (AFEC_COSR_CSEL_Msk & ((value) << AFEC_COSR_CSEL_Pos))
#define AFEC_COSR_Msk                         _U_(0x00000001)                                      /**< (AFEC_COSR) Register Mask  */


/* -------- AFEC_CVR : (AFEC Offset: 0xD4) (R/W 32) AFEC Correction Values Register -------- */
#define AFEC_CVR_OFFSETCORR_Pos               _U_(0)                                               /**< (AFEC_CVR) Offset Correction Position */
#define AFEC_CVR_OFFSETCORR_Msk               (_U_(0xFFFF) << AFEC_CVR_OFFSETCORR_Pos)             /**< (AFEC_CVR) Offset Correction Mask */
#define AFEC_CVR_OFFSETCORR(value)            (AFEC_CVR_OFFSETCORR_Msk & ((value) << AFEC_CVR_OFFSETCORR_Pos))
#define AFEC_CVR_GAINCORR_Pos                 _U_(16)                                              /**< (AFEC_CVR) Gain Correction Position */
#define AFEC_CVR_GAINCORR_Msk                 (_U_(0xFFFF) << AFEC_CVR_GAINCORR_Pos)               /**< (AFEC_CVR) Gain Correction Mask */
#define AFEC_CVR_GAINCORR(value)              (AFEC_CVR_GAINCORR_Msk & ((value) << AFEC_CVR_GAINCORR_Pos))
#define AFEC_CVR_Msk                          _U_(0xFFFFFFFF)                                      /**< (AFEC_CVR) Register Mask  */


/* -------- AFEC_CECR : (AFEC Offset: 0xD8) (R/W 32) AFEC Channel Error Correction Register -------- */
#define AFEC_CECR_ECORR0_Pos                  _U_(0)                                               /**< (AFEC_CECR) Error Correction Enable for channel 0 Position */
#define AFEC_CECR_ECORR0_Msk                  (_U_(0x1) << AFEC_CECR_ECORR0_Pos)                   /**< (AFEC_CECR) Error Correction Enable for channel 0 Mask */
#define AFEC_CECR_ECORR0(value)               (AFEC_CECR_ECORR0_Msk & ((value) << AFEC_CECR_ECORR0_Pos))
#define AFEC_CECR_ECORR1_Pos                  _U_(1)                                               /**< (AFEC_CECR) Error Correction Enable for channel 1 Position */
#define AFEC_CECR_ECORR1_Msk                  (_U_(0x1) << AFEC_CECR_ECORR1_Pos)                   /**< (AFEC_CECR) Error Correction Enable for channel 1 Mask */
#define AFEC_CECR_ECORR1(value)               (AFEC_CECR_ECORR1_Msk & ((value) << AFEC_CECR_ECORR1_Pos))
#define AFEC_CECR_ECORR2_Pos                  _U_(2)                                               /**< (AFEC_CECR) Error Correction Enable for channel 2 Position */
#define AFEC_CECR_ECORR2_Msk                  (_U_(0x1) << AFEC_CECR_ECORR2_Pos)                   /**< (AFEC_CECR) Error Correction Enable for channel 2 Mask */
#define AFEC_CECR_ECORR2(value)               (AFEC_CECR_ECORR2_Msk & ((value) << AFEC_CECR_ECORR2_Pos))
#define AFEC_CECR_ECORR3_Pos                  _U_(3)                                               /**< (AFEC_CECR) Error Correction Enable for channel 3 Position */
#define AFEC_CECR_ECORR3_Msk                  (_U_(0x1) << AFEC_CECR_ECORR3_Pos)                   /**< (AFEC_CECR) Error Correction Enable for channel 3 Mask */
#define AFEC_CECR_ECORR3(value)               (AFEC_CECR_ECORR3_Msk & ((value) << AFEC_CECR_ECORR3_Pos))
#define AFEC_CECR_ECORR4_Pos                  _U_(4)                                               /**< (AFEC_CECR) Error Correction Enable for channel 4 Position */
#define AFEC_CECR_ECORR4_Msk                  (_U_(0x1) << AFEC_CECR_ECORR4_Pos)                   /**< (AFEC_CECR) Error Correction Enable for channel 4 Mask */
#define AFEC_CECR_ECORR4(value)               (AFEC_CECR_ECORR4_Msk & ((value) << AFEC_CECR_ECORR4_Pos))
#define AFEC_CECR_ECORR5_Pos                  _U_(5)                                               /**< (AFEC_CECR) Error Correction Enable for channel 5 Position */
#define AFEC_CECR_ECORR5_Msk                  (_U_(0x1) << AFEC_CECR_ECORR5_Pos)                   /**< (AFEC_CECR) Error Correction Enable for channel 5 Mask */
#define AFEC_CECR_ECORR5(value)               (AFEC_CECR_ECORR5_Msk & ((value) << AFEC_CECR_ECORR5_Pos))
#define AFEC_CECR_ECORR6_Pos                  _U_(6)                                               /**< (AFEC_CECR) Error Correction Enable for channel 6 Position */
#define AFEC_CECR_ECORR6_Msk                  (_U_(0x1) << AFEC_CECR_ECORR6_Pos)                   /**< (AFEC_CECR) Error Correction Enable for channel 6 Mask */
#define AFEC_CECR_ECORR6(value)               (AFEC_CECR_ECORR6_Msk & ((value) << AFEC_CECR_ECORR6_Pos))
#define AFEC_CECR_ECORR7_Pos                  _U_(7)                                               /**< (AFEC_CECR) Error Correction Enable for channel 7 Position */
#define AFEC_CECR_ECORR7_Msk                  (_U_(0x1) << AFEC_CECR_ECORR7_Pos)                   /**< (AFEC_CECR) Error Correction Enable for channel 7 Mask */
#define AFEC_CECR_ECORR7(value)               (AFEC_CECR_ECORR7_Msk & ((value) << AFEC_CECR_ECORR7_Pos))
#define AFEC_CECR_ECORR8_Pos                  _U_(8)                                               /**< (AFEC_CECR) Error Correction Enable for channel 8 Position */
#define AFEC_CECR_ECORR8_Msk                  (_U_(0x1) << AFEC_CECR_ECORR8_Pos)                   /**< (AFEC_CECR) Error Correction Enable for channel 8 Mask */
#define AFEC_CECR_ECORR8(value)               (AFEC_CECR_ECORR8_Msk & ((value) << AFEC_CECR_ECORR8_Pos))
#define AFEC_CECR_ECORR9_Pos                  _U_(9)                                               /**< (AFEC_CECR) Error Correction Enable for channel 9 Position */
#define AFEC_CECR_ECORR9_Msk                  (_U_(0x1) << AFEC_CECR_ECORR9_Pos)                   /**< (AFEC_CECR) Error Correction Enable for channel 9 Mask */
#define AFEC_CECR_ECORR9(value)               (AFEC_CECR_ECORR9_Msk & ((value) << AFEC_CECR_ECORR9_Pos))
#define AFEC_CECR_ECORR10_Pos                 _U_(10)                                              /**< (AFEC_CECR) Error Correction Enable for channel 10 Position */
#define AFEC_CECR_ECORR10_Msk                 (_U_(0x1) << AFEC_CECR_ECORR10_Pos)                  /**< (AFEC_CECR) Error Correction Enable for channel 10 Mask */
#define AFEC_CECR_ECORR10(value)              (AFEC_CECR_ECORR10_Msk & ((value) << AFEC_CECR_ECORR10_Pos))
#define AFEC_CECR_ECORR11_Pos                 _U_(11)                                              /**< (AFEC_CECR) Error Correction Enable for channel 11 Position */
#define AFEC_CECR_ECORR11_Msk                 (_U_(0x1) << AFEC_CECR_ECORR11_Pos)                  /**< (AFEC_CECR) Error Correction Enable for channel 11 Mask */
#define AFEC_CECR_ECORR11(value)              (AFEC_CECR_ECORR11_Msk & ((value) << AFEC_CECR_ECORR11_Pos))
#define AFEC_CECR_Msk                         _U_(0x00000FFF)                                      /**< (AFEC_CECR) Register Mask  */

#define AFEC_CECR_ECORR_Pos                   _U_(0)                                               /**< (AFEC_CECR Position) Error Correction Enable for channel xx */
#define AFEC_CECR_ECORR_Msk                   (_U_(0xFFF) << AFEC_CECR_ECORR_Pos)                  /**< (AFEC_CECR Mask) ECORR */
#define AFEC_CECR_ECORR(value)                (AFEC_CECR_ECORR_Msk & ((value) << AFEC_CECR_ECORR_Pos)) 

/* -------- AFEC_WPMR : (AFEC Offset: 0xE4) (R/W 32) AFEC Write Protection Mode Register -------- */
#define AFEC_WPMR_WPEN_Pos                    _U_(0)                                               /**< (AFEC_WPMR) Write Protection Enable Position */
#define AFEC_WPMR_WPEN_Msk                    (_U_(0x1) << AFEC_WPMR_WPEN_Pos)                     /**< (AFEC_WPMR) Write Protection Enable Mask */
#define AFEC_WPMR_WPEN(value)                 (AFEC_WPMR_WPEN_Msk & ((value) << AFEC_WPMR_WPEN_Pos))
#define AFEC_WPMR_WPKEY_Pos                   _U_(8)                                               /**< (AFEC_WPMR) Write Protect KEY Position */
#define AFEC_WPMR_WPKEY_Msk                   (_U_(0xFFFFFF) << AFEC_WPMR_WPKEY_Pos)               /**< (AFEC_WPMR) Write Protect KEY Mask */
#define AFEC_WPMR_WPKEY(value)                (AFEC_WPMR_WPKEY_Msk & ((value) << AFEC_WPMR_WPKEY_Pos))
#define   AFEC_WPMR_WPKEY_PASSWD_Val          _U_(0x414443)                                        /**< (AFEC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0.  */
#define AFEC_WPMR_WPKEY_PASSWD                (AFEC_WPMR_WPKEY_PASSWD_Val << AFEC_WPMR_WPKEY_Pos)  /**< (AFEC_WPMR) Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0. Position  */
#define AFEC_WPMR_Msk                         _U_(0xFFFFFF01)                                      /**< (AFEC_WPMR) Register Mask  */


/* -------- AFEC_WPSR : (AFEC Offset: 0xE8) ( R/ 32) AFEC Write Protection Status Register -------- */
#define AFEC_WPSR_WPVS_Pos                    _U_(0)                                               /**< (AFEC_WPSR) Write Protect Violation Status Position */
#define AFEC_WPSR_WPVS_Msk                    (_U_(0x1) << AFEC_WPSR_WPVS_Pos)                     /**< (AFEC_WPSR) Write Protect Violation Status Mask */
#define AFEC_WPSR_WPVS(value)                 (AFEC_WPSR_WPVS_Msk & ((value) << AFEC_WPSR_WPVS_Pos))
#define AFEC_WPSR_WPVSRC_Pos                  _U_(8)                                               /**< (AFEC_WPSR) Write Protect Violation Source Position */
#define AFEC_WPSR_WPVSRC_Msk                  (_U_(0xFFFF) << AFEC_WPSR_WPVSRC_Pos)                /**< (AFEC_WPSR) Write Protect Violation Source Mask */
#define AFEC_WPSR_WPVSRC(value)               (AFEC_WPSR_WPVSRC_Msk & ((value) << AFEC_WPSR_WPVSRC_Pos))
#define AFEC_WPSR_Msk                         _U_(0x00FFFF01)                                      /**< (AFEC_WPSR) Register Mask  */


/** \brief AFEC register offsets definitions */
#define AFEC_CR_REG_OFST               (0x00)              /**< (AFEC_CR) AFEC Control Register Offset */
#define AFEC_MR_REG_OFST               (0x04)              /**< (AFEC_MR) AFEC Mode Register Offset */
#define AFEC_EMR_REG_OFST              (0x08)              /**< (AFEC_EMR) AFEC Extended Mode Register Offset */
#define AFEC_SEQ1R_REG_OFST            (0x0C)              /**< (AFEC_SEQ1R) AFEC Channel Sequence 1 Register Offset */
#define AFEC_SEQ2R_REG_OFST            (0x10)              /**< (AFEC_SEQ2R) AFEC Channel Sequence 2 Register Offset */
#define AFEC_CHER_REG_OFST             (0x14)              /**< (AFEC_CHER) AFEC Channel Enable Register Offset */
#define AFEC_CHDR_REG_OFST             (0x18)              /**< (AFEC_CHDR) AFEC Channel Disable Register Offset */
#define AFEC_CHSR_REG_OFST             (0x1C)              /**< (AFEC_CHSR) AFEC Channel Status Register Offset */
#define AFEC_LCDR_REG_OFST             (0x20)              /**< (AFEC_LCDR) AFEC Last Converted Data Register Offset */
#define AFEC_IER_REG_OFST              (0x24)              /**< (AFEC_IER) AFEC Interrupt Enable Register Offset */
#define AFEC_IDR_REG_OFST              (0x28)              /**< (AFEC_IDR) AFEC Interrupt Disable Register Offset */
#define AFEC_IMR_REG_OFST              (0x2C)              /**< (AFEC_IMR) AFEC Interrupt Mask Register Offset */
#define AFEC_ISR_REG_OFST              (0x30)              /**< (AFEC_ISR) AFEC Interrupt Status Register Offset */
#define AFEC_OVER_REG_OFST             (0x4C)              /**< (AFEC_OVER) AFEC Overrun Status Register Offset */
#define AFEC_CWR_REG_OFST              (0x50)              /**< (AFEC_CWR) AFEC Compare Window Register Offset */
#define AFEC_CGR_REG_OFST              (0x54)              /**< (AFEC_CGR) AFEC Channel Gain Register Offset */
#define AFEC_DIFFR_REG_OFST            (0x60)              /**< (AFEC_DIFFR) AFEC Channel Differential Register Offset */
#define AFEC_CSELR_REG_OFST            (0x64)              /**< (AFEC_CSELR) AFEC Channel Selection Register Offset */
#define AFEC_CDR_REG_OFST              (0x68)              /**< (AFEC_CDR) AFEC Channel Data Register Offset */
#define AFEC_COCR_REG_OFST             (0x6C)              /**< (AFEC_COCR) AFEC Channel Offset Compensation Register Offset */
#define AFEC_TEMPMR_REG_OFST           (0x70)              /**< (AFEC_TEMPMR) AFEC Temperature Sensor Mode Register Offset */
#define AFEC_TEMPCWR_REG_OFST          (0x74)              /**< (AFEC_TEMPCWR) AFEC Temperature Compare Window Register Offset */
#define AFEC_ACR_REG_OFST              (0x94)              /**< (AFEC_ACR) AFEC Analog Control Register Offset */
#define AFEC_SHMR_REG_OFST             (0xA0)              /**< (AFEC_SHMR) AFEC Sample & Hold Mode Register Offset */
#define AFEC_COSR_REG_OFST             (0xD0)              /**< (AFEC_COSR) AFEC Correction Select Register Offset */
#define AFEC_CVR_REG_OFST              (0xD4)              /**< (AFEC_CVR) AFEC Correction Values Register Offset */
#define AFEC_CECR_REG_OFST             (0xD8)              /**< (AFEC_CECR) AFEC Channel Error Correction Register Offset */
#define AFEC_WPMR_REG_OFST             (0xE4)              /**< (AFEC_WPMR) AFEC Write Protection Mode Register Offset */
#define AFEC_WPSR_REG_OFST             (0xE8)              /**< (AFEC_WPSR) AFEC Write Protection Status Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief AFEC register API structure */
typedef struct
{
  __O   uint32_t                       AFEC_CR;            /**< Offset: 0x00 ( /W  32) AFEC Control Register */
  __IO  uint32_t                       AFEC_MR;            /**< Offset: 0x04 (R/W  32) AFEC Mode Register */
  __IO  uint32_t                       AFEC_EMR;           /**< Offset: 0x08 (R/W  32) AFEC Extended Mode Register */
  __IO  uint32_t                       AFEC_SEQ1R;         /**< Offset: 0x0C (R/W  32) AFEC Channel Sequence 1 Register */
  __IO  uint32_t                       AFEC_SEQ2R;         /**< Offset: 0x10 (R/W  32) AFEC Channel Sequence 2 Register */
  __O   uint32_t                       AFEC_CHER;          /**< Offset: 0x14 ( /W  32) AFEC Channel Enable Register */
  __O   uint32_t                       AFEC_CHDR;          /**< Offset: 0x18 ( /W  32) AFEC Channel Disable Register */
  __I   uint32_t                       AFEC_CHSR;          /**< Offset: 0x1C (R/   32) AFEC Channel Status Register */
  __I   uint32_t                       AFEC_LCDR;          /**< Offset: 0x20 (R/   32) AFEC Last Converted Data Register */
  __O   uint32_t                       AFEC_IER;           /**< Offset: 0x24 ( /W  32) AFEC Interrupt Enable Register */
  __O   uint32_t                       AFEC_IDR;           /**< Offset: 0x28 ( /W  32) AFEC Interrupt Disable Register */
  __I   uint32_t                       AFEC_IMR;           /**< Offset: 0x2C (R/   32) AFEC Interrupt Mask Register */
  __I   uint32_t                       AFEC_ISR;           /**< Offset: 0x30 (R/   32) AFEC Interrupt Status Register */
  __I   uint8_t                        Reserved1[0x18];
  __I   uint32_t                       AFEC_OVER;          /**< Offset: 0x4C (R/   32) AFEC Overrun Status Register */
  __IO  uint32_t                       AFEC_CWR;           /**< Offset: 0x50 (R/W  32) AFEC Compare Window Register */
  __IO  uint32_t                       AFEC_CGR;           /**< Offset: 0x54 (R/W  32) AFEC Channel Gain Register */
  __I   uint8_t                        Reserved2[0x08];
  __IO  uint32_t                       AFEC_DIFFR;         /**< Offset: 0x60 (R/W  32) AFEC Channel Differential Register */
  __IO  uint32_t                       AFEC_CSELR;         /**< Offset: 0x64 (R/W  32) AFEC Channel Selection Register */
  __I   uint32_t                       AFEC_CDR;           /**< Offset: 0x68 (R/   32) AFEC Channel Data Register */
  __IO  uint32_t                       AFEC_COCR;          /**< Offset: 0x6C (R/W  32) AFEC Channel Offset Compensation Register */
  __IO  uint32_t                       AFEC_TEMPMR;        /**< Offset: 0x70 (R/W  32) AFEC Temperature Sensor Mode Register */
  __IO  uint32_t                       AFEC_TEMPCWR;       /**< Offset: 0x74 (R/W  32) AFEC Temperature Compare Window Register */
  __I   uint8_t                        Reserved3[0x1C];
  __IO  uint32_t                       AFEC_ACR;           /**< Offset: 0x94 (R/W  32) AFEC Analog Control Register */
  __I   uint8_t                        Reserved4[0x08];
  __IO  uint32_t                       AFEC_SHMR;          /**< Offset: 0xA0 (R/W  32) AFEC Sample & Hold Mode Register */
  __I   uint8_t                        Reserved5[0x2C];
  __IO  uint32_t                       AFEC_COSR;          /**< Offset: 0xD0 (R/W  32) AFEC Correction Select Register */
  __IO  uint32_t                       AFEC_CVR;           /**< Offset: 0xD4 (R/W  32) AFEC Correction Values Register */
  __IO  uint32_t                       AFEC_CECR;          /**< Offset: 0xD8 (R/W  32) AFEC Channel Error Correction Register */
  __I   uint8_t                        Reserved6[0x08];
  __IO  uint32_t                       AFEC_WPMR;          /**< Offset: 0xE4 (R/W  32) AFEC Write Protection Mode Register */
  __I   uint32_t                       AFEC_WPSR;          /**< Offset: 0xE8 (R/   32) AFEC Write Protection Status Register */
} afec_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_AFEC_COMPONENT_H_ */
