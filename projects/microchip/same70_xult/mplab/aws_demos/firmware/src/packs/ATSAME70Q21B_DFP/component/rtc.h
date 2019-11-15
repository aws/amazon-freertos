/**
 * \brief Component description for RTC
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
#ifndef _SAME70_RTC_COMPONENT_H_
#define _SAME70_RTC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR RTC                                          */
/* ************************************************************************** */

/* -------- RTC_CR : (RTC Offset: 0x00) (R/W 32) Control Register -------- */
#define RTC_CR_UPDTIM_Pos                     _U_(0)                                               /**< (RTC_CR) Update Request Time Register Position */
#define RTC_CR_UPDTIM_Msk                     (_U_(0x1) << RTC_CR_UPDTIM_Pos)                      /**< (RTC_CR) Update Request Time Register Mask */
#define RTC_CR_UPDTIM(value)                  (RTC_CR_UPDTIM_Msk & ((value) << RTC_CR_UPDTIM_Pos))
#define RTC_CR_UPDCAL_Pos                     _U_(1)                                               /**< (RTC_CR) Update Request Calendar Register Position */
#define RTC_CR_UPDCAL_Msk                     (_U_(0x1) << RTC_CR_UPDCAL_Pos)                      /**< (RTC_CR) Update Request Calendar Register Mask */
#define RTC_CR_UPDCAL(value)                  (RTC_CR_UPDCAL_Msk & ((value) << RTC_CR_UPDCAL_Pos))
#define RTC_CR_TIMEVSEL_Pos                   _U_(8)                                               /**< (RTC_CR) Time Event Selection Position */
#define RTC_CR_TIMEVSEL_Msk                   (_U_(0x3) << RTC_CR_TIMEVSEL_Pos)                    /**< (RTC_CR) Time Event Selection Mask */
#define RTC_CR_TIMEVSEL(value)                (RTC_CR_TIMEVSEL_Msk & ((value) << RTC_CR_TIMEVSEL_Pos))
#define   RTC_CR_TIMEVSEL_MINUTE_Val          _U_(0x0)                                             /**< (RTC_CR) Minute change  */
#define   RTC_CR_TIMEVSEL_HOUR_Val            _U_(0x1)                                             /**< (RTC_CR) Hour change  */
#define   RTC_CR_TIMEVSEL_MIDNIGHT_Val        _U_(0x2)                                             /**< (RTC_CR) Every day at midnight  */
#define   RTC_CR_TIMEVSEL_NOON_Val            _U_(0x3)                                             /**< (RTC_CR) Every day at noon  */
#define RTC_CR_TIMEVSEL_MINUTE                (RTC_CR_TIMEVSEL_MINUTE_Val << RTC_CR_TIMEVSEL_Pos)  /**< (RTC_CR) Minute change Position  */
#define RTC_CR_TIMEVSEL_HOUR                  (RTC_CR_TIMEVSEL_HOUR_Val << RTC_CR_TIMEVSEL_Pos)    /**< (RTC_CR) Hour change Position  */
#define RTC_CR_TIMEVSEL_MIDNIGHT              (RTC_CR_TIMEVSEL_MIDNIGHT_Val << RTC_CR_TIMEVSEL_Pos) /**< (RTC_CR) Every day at midnight Position  */
#define RTC_CR_TIMEVSEL_NOON                  (RTC_CR_TIMEVSEL_NOON_Val << RTC_CR_TIMEVSEL_Pos)    /**< (RTC_CR) Every day at noon Position  */
#define RTC_CR_CALEVSEL_Pos                   _U_(16)                                              /**< (RTC_CR) Calendar Event Selection Position */
#define RTC_CR_CALEVSEL_Msk                   (_U_(0x3) << RTC_CR_CALEVSEL_Pos)                    /**< (RTC_CR) Calendar Event Selection Mask */
#define RTC_CR_CALEVSEL(value)                (RTC_CR_CALEVSEL_Msk & ((value) << RTC_CR_CALEVSEL_Pos))
#define   RTC_CR_CALEVSEL_WEEK_Val            _U_(0x0)                                             /**< (RTC_CR) Week change (every Monday at time 00:00:00)  */
#define   RTC_CR_CALEVSEL_MONTH_Val           _U_(0x1)                                             /**< (RTC_CR) Month change (every 01 of each month at time 00:00:00)  */
#define   RTC_CR_CALEVSEL_YEAR_Val            _U_(0x2)                                             /**< (RTC_CR) Year change (every January 1 at time 00:00:00)  */
#define RTC_CR_CALEVSEL_WEEK                  (RTC_CR_CALEVSEL_WEEK_Val << RTC_CR_CALEVSEL_Pos)    /**< (RTC_CR) Week change (every Monday at time 00:00:00) Position  */
#define RTC_CR_CALEVSEL_MONTH                 (RTC_CR_CALEVSEL_MONTH_Val << RTC_CR_CALEVSEL_Pos)   /**< (RTC_CR) Month change (every 01 of each month at time 00:00:00) Position  */
#define RTC_CR_CALEVSEL_YEAR                  (RTC_CR_CALEVSEL_YEAR_Val << RTC_CR_CALEVSEL_Pos)    /**< (RTC_CR) Year change (every January 1 at time 00:00:00) Position  */
#define RTC_CR_Msk                            _U_(0x00030303)                                      /**< (RTC_CR) Register Mask  */


/* -------- RTC_MR : (RTC Offset: 0x04) (R/W 32) Mode Register -------- */
#define RTC_MR_HRMOD_Pos                      _U_(0)                                               /**< (RTC_MR) 12-/24-hour Mode Position */
#define RTC_MR_HRMOD_Msk                      (_U_(0x1) << RTC_MR_HRMOD_Pos)                       /**< (RTC_MR) 12-/24-hour Mode Mask */
#define RTC_MR_HRMOD(value)                   (RTC_MR_HRMOD_Msk & ((value) << RTC_MR_HRMOD_Pos))  
#define RTC_MR_PERSIAN_Pos                    _U_(1)                                               /**< (RTC_MR) PERSIAN Calendar Position */
#define RTC_MR_PERSIAN_Msk                    (_U_(0x1) << RTC_MR_PERSIAN_Pos)                     /**< (RTC_MR) PERSIAN Calendar Mask */
#define RTC_MR_PERSIAN(value)                 (RTC_MR_PERSIAN_Msk & ((value) << RTC_MR_PERSIAN_Pos))
#define RTC_MR_NEGPPM_Pos                     _U_(4)                                               /**< (RTC_MR) NEGative PPM Correction Position */
#define RTC_MR_NEGPPM_Msk                     (_U_(0x1) << RTC_MR_NEGPPM_Pos)                      /**< (RTC_MR) NEGative PPM Correction Mask */
#define RTC_MR_NEGPPM(value)                  (RTC_MR_NEGPPM_Msk & ((value) << RTC_MR_NEGPPM_Pos))
#define RTC_MR_CORRECTION_Pos                 _U_(8)                                               /**< (RTC_MR) Slow Clock Correction Position */
#define RTC_MR_CORRECTION_Msk                 (_U_(0x7F) << RTC_MR_CORRECTION_Pos)                 /**< (RTC_MR) Slow Clock Correction Mask */
#define RTC_MR_CORRECTION(value)              (RTC_MR_CORRECTION_Msk & ((value) << RTC_MR_CORRECTION_Pos))
#define RTC_MR_HIGHPPM_Pos                    _U_(15)                                              /**< (RTC_MR) HIGH PPM Correction Position */
#define RTC_MR_HIGHPPM_Msk                    (_U_(0x1) << RTC_MR_HIGHPPM_Pos)                     /**< (RTC_MR) HIGH PPM Correction Mask */
#define RTC_MR_HIGHPPM(value)                 (RTC_MR_HIGHPPM_Msk & ((value) << RTC_MR_HIGHPPM_Pos))
#define RTC_MR_OUT0_Pos                       _U_(16)                                              /**< (RTC_MR) RTCOUT0 OutputSource Selection Position */
#define RTC_MR_OUT0_Msk                       (_U_(0x7) << RTC_MR_OUT0_Pos)                        /**< (RTC_MR) RTCOUT0 OutputSource Selection Mask */
#define RTC_MR_OUT0(value)                    (RTC_MR_OUT0_Msk & ((value) << RTC_MR_OUT0_Pos))    
#define   RTC_MR_OUT0_NO_WAVE_Val             _U_(0x0)                                             /**< (RTC_MR) No waveform, stuck at '0'  */
#define   RTC_MR_OUT0_FREQ1HZ_Val             _U_(0x1)                                             /**< (RTC_MR) 1 Hz square wave  */
#define   RTC_MR_OUT0_FREQ32HZ_Val            _U_(0x2)                                             /**< (RTC_MR) 32 Hz square wave  */
#define   RTC_MR_OUT0_FREQ64HZ_Val            _U_(0x3)                                             /**< (RTC_MR) 64 Hz square wave  */
#define   RTC_MR_OUT0_FREQ512HZ_Val           _U_(0x4)                                             /**< (RTC_MR) 512 Hz square wave  */
#define   RTC_MR_OUT0_ALARM_TOGGLE_Val        _U_(0x5)                                             /**< (RTC_MR) Output toggles when alarm flag rises  */
#define   RTC_MR_OUT0_ALARM_FLAG_Val          _U_(0x6)                                             /**< (RTC_MR) Output is a copy of the alarm flag  */
#define   RTC_MR_OUT0_PROG_PULSE_Val          _U_(0x7)                                             /**< (RTC_MR) Duty cycle programmable pulse  */
#define RTC_MR_OUT0_NO_WAVE                   (RTC_MR_OUT0_NO_WAVE_Val << RTC_MR_OUT0_Pos)         /**< (RTC_MR) No waveform, stuck at '0' Position  */
#define RTC_MR_OUT0_FREQ1HZ                   (RTC_MR_OUT0_FREQ1HZ_Val << RTC_MR_OUT0_Pos)         /**< (RTC_MR) 1 Hz square wave Position  */
#define RTC_MR_OUT0_FREQ32HZ                  (RTC_MR_OUT0_FREQ32HZ_Val << RTC_MR_OUT0_Pos)        /**< (RTC_MR) 32 Hz square wave Position  */
#define RTC_MR_OUT0_FREQ64HZ                  (RTC_MR_OUT0_FREQ64HZ_Val << RTC_MR_OUT0_Pos)        /**< (RTC_MR) 64 Hz square wave Position  */
#define RTC_MR_OUT0_FREQ512HZ                 (RTC_MR_OUT0_FREQ512HZ_Val << RTC_MR_OUT0_Pos)       /**< (RTC_MR) 512 Hz square wave Position  */
#define RTC_MR_OUT0_ALARM_TOGGLE              (RTC_MR_OUT0_ALARM_TOGGLE_Val << RTC_MR_OUT0_Pos)    /**< (RTC_MR) Output toggles when alarm flag rises Position  */
#define RTC_MR_OUT0_ALARM_FLAG                (RTC_MR_OUT0_ALARM_FLAG_Val << RTC_MR_OUT0_Pos)      /**< (RTC_MR) Output is a copy of the alarm flag Position  */
#define RTC_MR_OUT0_PROG_PULSE                (RTC_MR_OUT0_PROG_PULSE_Val << RTC_MR_OUT0_Pos)      /**< (RTC_MR) Duty cycle programmable pulse Position  */
#define RTC_MR_OUT1_Pos                       _U_(20)                                              /**< (RTC_MR) RTCOUT1 Output Source Selection Position */
#define RTC_MR_OUT1_Msk                       (_U_(0x7) << RTC_MR_OUT1_Pos)                        /**< (RTC_MR) RTCOUT1 Output Source Selection Mask */
#define RTC_MR_OUT1(value)                    (RTC_MR_OUT1_Msk & ((value) << RTC_MR_OUT1_Pos))    
#define   RTC_MR_OUT1_NO_WAVE_Val             _U_(0x0)                                             /**< (RTC_MR) No waveform, stuck at '0'  */
#define   RTC_MR_OUT1_FREQ1HZ_Val             _U_(0x1)                                             /**< (RTC_MR) 1 Hz square wave  */
#define   RTC_MR_OUT1_FREQ32HZ_Val            _U_(0x2)                                             /**< (RTC_MR) 32 Hz square wave  */
#define   RTC_MR_OUT1_FREQ64HZ_Val            _U_(0x3)                                             /**< (RTC_MR) 64 Hz square wave  */
#define   RTC_MR_OUT1_FREQ512HZ_Val           _U_(0x4)                                             /**< (RTC_MR) 512 Hz square wave  */
#define   RTC_MR_OUT1_ALARM_TOGGLE_Val        _U_(0x5)                                             /**< (RTC_MR) Output toggles when alarm flag rises  */
#define   RTC_MR_OUT1_ALARM_FLAG_Val          _U_(0x6)                                             /**< (RTC_MR) Output is a copy of the alarm flag  */
#define   RTC_MR_OUT1_PROG_PULSE_Val          _U_(0x7)                                             /**< (RTC_MR) Duty cycle programmable pulse  */
#define RTC_MR_OUT1_NO_WAVE                   (RTC_MR_OUT1_NO_WAVE_Val << RTC_MR_OUT1_Pos)         /**< (RTC_MR) No waveform, stuck at '0' Position  */
#define RTC_MR_OUT1_FREQ1HZ                   (RTC_MR_OUT1_FREQ1HZ_Val << RTC_MR_OUT1_Pos)         /**< (RTC_MR) 1 Hz square wave Position  */
#define RTC_MR_OUT1_FREQ32HZ                  (RTC_MR_OUT1_FREQ32HZ_Val << RTC_MR_OUT1_Pos)        /**< (RTC_MR) 32 Hz square wave Position  */
#define RTC_MR_OUT1_FREQ64HZ                  (RTC_MR_OUT1_FREQ64HZ_Val << RTC_MR_OUT1_Pos)        /**< (RTC_MR) 64 Hz square wave Position  */
#define RTC_MR_OUT1_FREQ512HZ                 (RTC_MR_OUT1_FREQ512HZ_Val << RTC_MR_OUT1_Pos)       /**< (RTC_MR) 512 Hz square wave Position  */
#define RTC_MR_OUT1_ALARM_TOGGLE              (RTC_MR_OUT1_ALARM_TOGGLE_Val << RTC_MR_OUT1_Pos)    /**< (RTC_MR) Output toggles when alarm flag rises Position  */
#define RTC_MR_OUT1_ALARM_FLAG                (RTC_MR_OUT1_ALARM_FLAG_Val << RTC_MR_OUT1_Pos)      /**< (RTC_MR) Output is a copy of the alarm flag Position  */
#define RTC_MR_OUT1_PROG_PULSE                (RTC_MR_OUT1_PROG_PULSE_Val << RTC_MR_OUT1_Pos)      /**< (RTC_MR) Duty cycle programmable pulse Position  */
#define RTC_MR_THIGH_Pos                      _U_(24)                                              /**< (RTC_MR) High Duration of the Output Pulse Position */
#define RTC_MR_THIGH_Msk                      (_U_(0x7) << RTC_MR_THIGH_Pos)                       /**< (RTC_MR) High Duration of the Output Pulse Mask */
#define RTC_MR_THIGH(value)                   (RTC_MR_THIGH_Msk & ((value) << RTC_MR_THIGH_Pos))  
#define   RTC_MR_THIGH_H_31MS_Val             _U_(0x0)                                             /**< (RTC_MR) 31.2 ms  */
#define   RTC_MR_THIGH_H_16MS_Val             _U_(0x1)                                             /**< (RTC_MR) 15.6 ms  */
#define   RTC_MR_THIGH_H_4MS_Val              _U_(0x2)                                             /**< (RTC_MR) 3.91 ms  */
#define   RTC_MR_THIGH_H_976US_Val            _U_(0x3)                                             /**< (RTC_MR) 976 us  */
#define   RTC_MR_THIGH_H_488US_Val            _U_(0x4)                                             /**< (RTC_MR) 488 us  */
#define   RTC_MR_THIGH_H_122US_Val            _U_(0x5)                                             /**< (RTC_MR) 122 us  */
#define   RTC_MR_THIGH_H_30US_Val             _U_(0x6)                                             /**< (RTC_MR) 30.5 us  */
#define   RTC_MR_THIGH_H_15US_Val             _U_(0x7)                                             /**< (RTC_MR) 15.2 us  */
#define RTC_MR_THIGH_H_31MS                   (RTC_MR_THIGH_H_31MS_Val << RTC_MR_THIGH_Pos)        /**< (RTC_MR) 31.2 ms Position  */
#define RTC_MR_THIGH_H_16MS                   (RTC_MR_THIGH_H_16MS_Val << RTC_MR_THIGH_Pos)        /**< (RTC_MR) 15.6 ms Position  */
#define RTC_MR_THIGH_H_4MS                    (RTC_MR_THIGH_H_4MS_Val << RTC_MR_THIGH_Pos)         /**< (RTC_MR) 3.91 ms Position  */
#define RTC_MR_THIGH_H_976US                  (RTC_MR_THIGH_H_976US_Val << RTC_MR_THIGH_Pos)       /**< (RTC_MR) 976 us Position  */
#define RTC_MR_THIGH_H_488US                  (RTC_MR_THIGH_H_488US_Val << RTC_MR_THIGH_Pos)       /**< (RTC_MR) 488 us Position  */
#define RTC_MR_THIGH_H_122US                  (RTC_MR_THIGH_H_122US_Val << RTC_MR_THIGH_Pos)       /**< (RTC_MR) 122 us Position  */
#define RTC_MR_THIGH_H_30US                   (RTC_MR_THIGH_H_30US_Val << RTC_MR_THIGH_Pos)        /**< (RTC_MR) 30.5 us Position  */
#define RTC_MR_THIGH_H_15US                   (RTC_MR_THIGH_H_15US_Val << RTC_MR_THIGH_Pos)        /**< (RTC_MR) 15.2 us Position  */
#define RTC_MR_TPERIOD_Pos                    _U_(28)                                              /**< (RTC_MR) Period of the Output Pulse Position */
#define RTC_MR_TPERIOD_Msk                    (_U_(0x3) << RTC_MR_TPERIOD_Pos)                     /**< (RTC_MR) Period of the Output Pulse Mask */
#define RTC_MR_TPERIOD(value)                 (RTC_MR_TPERIOD_Msk & ((value) << RTC_MR_TPERIOD_Pos))
#define   RTC_MR_TPERIOD_P_1S_Val             _U_(0x0)                                             /**< (RTC_MR) 1 second  */
#define   RTC_MR_TPERIOD_P_500MS_Val          _U_(0x1)                                             /**< (RTC_MR) 500 ms  */
#define   RTC_MR_TPERIOD_P_250MS_Val          _U_(0x2)                                             /**< (RTC_MR) 250 ms  */
#define   RTC_MR_TPERIOD_P_125MS_Val          _U_(0x3)                                             /**< (RTC_MR) 125 ms  */
#define RTC_MR_TPERIOD_P_1S                   (RTC_MR_TPERIOD_P_1S_Val << RTC_MR_TPERIOD_Pos)      /**< (RTC_MR) 1 second Position  */
#define RTC_MR_TPERIOD_P_500MS                (RTC_MR_TPERIOD_P_500MS_Val << RTC_MR_TPERIOD_Pos)   /**< (RTC_MR) 500 ms Position  */
#define RTC_MR_TPERIOD_P_250MS                (RTC_MR_TPERIOD_P_250MS_Val << RTC_MR_TPERIOD_Pos)   /**< (RTC_MR) 250 ms Position  */
#define RTC_MR_TPERIOD_P_125MS                (RTC_MR_TPERIOD_P_125MS_Val << RTC_MR_TPERIOD_Pos)   /**< (RTC_MR) 125 ms Position  */
#define RTC_MR_Msk                            _U_(0x3777FF13)                                      /**< (RTC_MR) Register Mask  */


/* -------- RTC_TIMR : (RTC Offset: 0x08) (R/W 32) Time Register -------- */
#define RTC_TIMR_SEC_Pos                      _U_(0)                                               /**< (RTC_TIMR) Current Second Position */
#define RTC_TIMR_SEC_Msk                      (_U_(0x7F) << RTC_TIMR_SEC_Pos)                      /**< (RTC_TIMR) Current Second Mask */
#define RTC_TIMR_SEC(value)                   (RTC_TIMR_SEC_Msk & ((value) << RTC_TIMR_SEC_Pos))  
#define RTC_TIMR_MIN_Pos                      _U_(8)                                               /**< (RTC_TIMR) Current Minute Position */
#define RTC_TIMR_MIN_Msk                      (_U_(0x7F) << RTC_TIMR_MIN_Pos)                      /**< (RTC_TIMR) Current Minute Mask */
#define RTC_TIMR_MIN(value)                   (RTC_TIMR_MIN_Msk & ((value) << RTC_TIMR_MIN_Pos))  
#define RTC_TIMR_HOUR_Pos                     _U_(16)                                              /**< (RTC_TIMR) Current Hour Position */
#define RTC_TIMR_HOUR_Msk                     (_U_(0x3F) << RTC_TIMR_HOUR_Pos)                     /**< (RTC_TIMR) Current Hour Mask */
#define RTC_TIMR_HOUR(value)                  (RTC_TIMR_HOUR_Msk & ((value) << RTC_TIMR_HOUR_Pos))
#define RTC_TIMR_AMPM_Pos                     _U_(22)                                              /**< (RTC_TIMR) Ante Meridiem Post Meridiem Indicator Position */
#define RTC_TIMR_AMPM_Msk                     (_U_(0x1) << RTC_TIMR_AMPM_Pos)                      /**< (RTC_TIMR) Ante Meridiem Post Meridiem Indicator Mask */
#define RTC_TIMR_AMPM(value)                  (RTC_TIMR_AMPM_Msk & ((value) << RTC_TIMR_AMPM_Pos))
#define RTC_TIMR_Msk                          _U_(0x007F7F7F)                                      /**< (RTC_TIMR) Register Mask  */


/* -------- RTC_CALR : (RTC Offset: 0x0C) (R/W 32) Calendar Register -------- */
#define RTC_CALR_CENT_Pos                     _U_(0)                                               /**< (RTC_CALR) Current Century Position */
#define RTC_CALR_CENT_Msk                     (_U_(0x7F) << RTC_CALR_CENT_Pos)                     /**< (RTC_CALR) Current Century Mask */
#define RTC_CALR_CENT(value)                  (RTC_CALR_CENT_Msk & ((value) << RTC_CALR_CENT_Pos))
#define RTC_CALR_YEAR_Pos                     _U_(8)                                               /**< (RTC_CALR) Current Year Position */
#define RTC_CALR_YEAR_Msk                     (_U_(0xFF) << RTC_CALR_YEAR_Pos)                     /**< (RTC_CALR) Current Year Mask */
#define RTC_CALR_YEAR(value)                  (RTC_CALR_YEAR_Msk & ((value) << RTC_CALR_YEAR_Pos))
#define RTC_CALR_MONTH_Pos                    _U_(16)                                              /**< (RTC_CALR) Current Month Position */
#define RTC_CALR_MONTH_Msk                    (_U_(0x1F) << RTC_CALR_MONTH_Pos)                    /**< (RTC_CALR) Current Month Mask */
#define RTC_CALR_MONTH(value)                 (RTC_CALR_MONTH_Msk & ((value) << RTC_CALR_MONTH_Pos))
#define RTC_CALR_DAY_Pos                      _U_(21)                                              /**< (RTC_CALR) Current Day in Current Week Position */
#define RTC_CALR_DAY_Msk                      (_U_(0x7) << RTC_CALR_DAY_Pos)                       /**< (RTC_CALR) Current Day in Current Week Mask */
#define RTC_CALR_DAY(value)                   (RTC_CALR_DAY_Msk & ((value) << RTC_CALR_DAY_Pos))  
#define RTC_CALR_DATE_Pos                     _U_(24)                                              /**< (RTC_CALR) Current Day in Current Month Position */
#define RTC_CALR_DATE_Msk                     (_U_(0x3F) << RTC_CALR_DATE_Pos)                     /**< (RTC_CALR) Current Day in Current Month Mask */
#define RTC_CALR_DATE(value)                  (RTC_CALR_DATE_Msk & ((value) << RTC_CALR_DATE_Pos))
#define RTC_CALR_Msk                          _U_(0x3FFFFF7F)                                      /**< (RTC_CALR) Register Mask  */


/* -------- RTC_TIMALR : (RTC Offset: 0x10) (R/W 32) Time Alarm Register -------- */
#define RTC_TIMALR_SEC_Pos                    _U_(0)                                               /**< (RTC_TIMALR) Second Alarm Position */
#define RTC_TIMALR_SEC_Msk                    (_U_(0x7F) << RTC_TIMALR_SEC_Pos)                    /**< (RTC_TIMALR) Second Alarm Mask */
#define RTC_TIMALR_SEC(value)                 (RTC_TIMALR_SEC_Msk & ((value) << RTC_TIMALR_SEC_Pos))
#define RTC_TIMALR_SECEN_Pos                  _U_(7)                                               /**< (RTC_TIMALR) Second Alarm Enable Position */
#define RTC_TIMALR_SECEN_Msk                  (_U_(0x1) << RTC_TIMALR_SECEN_Pos)                   /**< (RTC_TIMALR) Second Alarm Enable Mask */
#define RTC_TIMALR_SECEN(value)               (RTC_TIMALR_SECEN_Msk & ((value) << RTC_TIMALR_SECEN_Pos))
#define RTC_TIMALR_MIN_Pos                    _U_(8)                                               /**< (RTC_TIMALR) Minute Alarm Position */
#define RTC_TIMALR_MIN_Msk                    (_U_(0x7F) << RTC_TIMALR_MIN_Pos)                    /**< (RTC_TIMALR) Minute Alarm Mask */
#define RTC_TIMALR_MIN(value)                 (RTC_TIMALR_MIN_Msk & ((value) << RTC_TIMALR_MIN_Pos))
#define RTC_TIMALR_MINEN_Pos                  _U_(15)                                              /**< (RTC_TIMALR) Minute Alarm Enable Position */
#define RTC_TIMALR_MINEN_Msk                  (_U_(0x1) << RTC_TIMALR_MINEN_Pos)                   /**< (RTC_TIMALR) Minute Alarm Enable Mask */
#define RTC_TIMALR_MINEN(value)               (RTC_TIMALR_MINEN_Msk & ((value) << RTC_TIMALR_MINEN_Pos))
#define RTC_TIMALR_HOUR_Pos                   _U_(16)                                              /**< (RTC_TIMALR) Hour Alarm Position */
#define RTC_TIMALR_HOUR_Msk                   (_U_(0x3F) << RTC_TIMALR_HOUR_Pos)                   /**< (RTC_TIMALR) Hour Alarm Mask */
#define RTC_TIMALR_HOUR(value)                (RTC_TIMALR_HOUR_Msk & ((value) << RTC_TIMALR_HOUR_Pos))
#define RTC_TIMALR_AMPM_Pos                   _U_(22)                                              /**< (RTC_TIMALR) AM/PM Indicator Position */
#define RTC_TIMALR_AMPM_Msk                   (_U_(0x1) << RTC_TIMALR_AMPM_Pos)                    /**< (RTC_TIMALR) AM/PM Indicator Mask */
#define RTC_TIMALR_AMPM(value)                (RTC_TIMALR_AMPM_Msk & ((value) << RTC_TIMALR_AMPM_Pos))
#define RTC_TIMALR_HOUREN_Pos                 _U_(23)                                              /**< (RTC_TIMALR) Hour Alarm Enable Position */
#define RTC_TIMALR_HOUREN_Msk                 (_U_(0x1) << RTC_TIMALR_HOUREN_Pos)                  /**< (RTC_TIMALR) Hour Alarm Enable Mask */
#define RTC_TIMALR_HOUREN(value)              (RTC_TIMALR_HOUREN_Msk & ((value) << RTC_TIMALR_HOUREN_Pos))
#define RTC_TIMALR_Msk                        _U_(0x00FFFFFF)                                      /**< (RTC_TIMALR) Register Mask  */


/* -------- RTC_CALALR : (RTC Offset: 0x14) (R/W 32) Calendar Alarm Register -------- */
#define RTC_CALALR_MONTH_Pos                  _U_(16)                                              /**< (RTC_CALALR) Month Alarm Position */
#define RTC_CALALR_MONTH_Msk                  (_U_(0x1F) << RTC_CALALR_MONTH_Pos)                  /**< (RTC_CALALR) Month Alarm Mask */
#define RTC_CALALR_MONTH(value)               (RTC_CALALR_MONTH_Msk & ((value) << RTC_CALALR_MONTH_Pos))
#define RTC_CALALR_MTHEN_Pos                  _U_(23)                                              /**< (RTC_CALALR) Month Alarm Enable Position */
#define RTC_CALALR_MTHEN_Msk                  (_U_(0x1) << RTC_CALALR_MTHEN_Pos)                   /**< (RTC_CALALR) Month Alarm Enable Mask */
#define RTC_CALALR_MTHEN(value)               (RTC_CALALR_MTHEN_Msk & ((value) << RTC_CALALR_MTHEN_Pos))
#define RTC_CALALR_DATE_Pos                   _U_(24)                                              /**< (RTC_CALALR) Date Alarm Position */
#define RTC_CALALR_DATE_Msk                   (_U_(0x3F) << RTC_CALALR_DATE_Pos)                   /**< (RTC_CALALR) Date Alarm Mask */
#define RTC_CALALR_DATE(value)                (RTC_CALALR_DATE_Msk & ((value) << RTC_CALALR_DATE_Pos))
#define RTC_CALALR_DATEEN_Pos                 _U_(31)                                              /**< (RTC_CALALR) Date Alarm Enable Position */
#define RTC_CALALR_DATEEN_Msk                 (_U_(0x1) << RTC_CALALR_DATEEN_Pos)                  /**< (RTC_CALALR) Date Alarm Enable Mask */
#define RTC_CALALR_DATEEN(value)              (RTC_CALALR_DATEEN_Msk & ((value) << RTC_CALALR_DATEEN_Pos))
#define RTC_CALALR_Msk                        _U_(0xBF9F0000)                                      /**< (RTC_CALALR) Register Mask  */


/* -------- RTC_SR : (RTC Offset: 0x18) ( R/ 32) Status Register -------- */
#define RTC_SR_ACKUPD_Pos                     _U_(0)                                               /**< (RTC_SR) Acknowledge for Update Position */
#define RTC_SR_ACKUPD_Msk                     (_U_(0x1) << RTC_SR_ACKUPD_Pos)                      /**< (RTC_SR) Acknowledge for Update Mask */
#define RTC_SR_ACKUPD(value)                  (RTC_SR_ACKUPD_Msk & ((value) << RTC_SR_ACKUPD_Pos))
#define   RTC_SR_ACKUPD_FREERUN_Val           _U_(0x0)                                             /**< (RTC_SR) Time and calendar registers cannot be updated.  */
#define   RTC_SR_ACKUPD_UPDATE_Val            _U_(0x1)                                             /**< (RTC_SR) Time and calendar registers can be updated.  */
#define RTC_SR_ACKUPD_FREERUN                 (RTC_SR_ACKUPD_FREERUN_Val << RTC_SR_ACKUPD_Pos)     /**< (RTC_SR) Time and calendar registers cannot be updated. Position  */
#define RTC_SR_ACKUPD_UPDATE                  (RTC_SR_ACKUPD_UPDATE_Val << RTC_SR_ACKUPD_Pos)      /**< (RTC_SR) Time and calendar registers can be updated. Position  */
#define RTC_SR_ALARM_Pos                      _U_(1)                                               /**< (RTC_SR) Alarm Flag Position */
#define RTC_SR_ALARM_Msk                      (_U_(0x1) << RTC_SR_ALARM_Pos)                       /**< (RTC_SR) Alarm Flag Mask */
#define RTC_SR_ALARM(value)                   (RTC_SR_ALARM_Msk & ((value) << RTC_SR_ALARM_Pos))  
#define   RTC_SR_ALARM_NO_ALARMEVENT_Val      _U_(0x0)                                             /**< (RTC_SR) No alarm matching condition occurred.  */
#define   RTC_SR_ALARM_ALARMEVENT_Val         _U_(0x1)                                             /**< (RTC_SR) An alarm matching condition has occurred.  */
#define RTC_SR_ALARM_NO_ALARMEVENT            (RTC_SR_ALARM_NO_ALARMEVENT_Val << RTC_SR_ALARM_Pos) /**< (RTC_SR) No alarm matching condition occurred. Position  */
#define RTC_SR_ALARM_ALARMEVENT               (RTC_SR_ALARM_ALARMEVENT_Val << RTC_SR_ALARM_Pos)    /**< (RTC_SR) An alarm matching condition has occurred. Position  */
#define RTC_SR_SEC_Pos                        _U_(2)                                               /**< (RTC_SR) Second Event Position */
#define RTC_SR_SEC_Msk                        (_U_(0x1) << RTC_SR_SEC_Pos)                         /**< (RTC_SR) Second Event Mask */
#define RTC_SR_SEC(value)                     (RTC_SR_SEC_Msk & ((value) << RTC_SR_SEC_Pos))      
#define   RTC_SR_SEC_NO_SECEVENT_Val          _U_(0x0)                                             /**< (RTC_SR) No second event has occurred since the last clear.  */
#define   RTC_SR_SEC_SECEVENT_Val             _U_(0x1)                                             /**< (RTC_SR) At least one second event has occurred since the last clear.  */
#define RTC_SR_SEC_NO_SECEVENT                (RTC_SR_SEC_NO_SECEVENT_Val << RTC_SR_SEC_Pos)       /**< (RTC_SR) No second event has occurred since the last clear. Position  */
#define RTC_SR_SEC_SECEVENT                   (RTC_SR_SEC_SECEVENT_Val << RTC_SR_SEC_Pos)          /**< (RTC_SR) At least one second event has occurred since the last clear. Position  */
#define RTC_SR_TIMEV_Pos                      _U_(3)                                               /**< (RTC_SR) Time Event Position */
#define RTC_SR_TIMEV_Msk                      (_U_(0x1) << RTC_SR_TIMEV_Pos)                       /**< (RTC_SR) Time Event Mask */
#define RTC_SR_TIMEV(value)                   (RTC_SR_TIMEV_Msk & ((value) << RTC_SR_TIMEV_Pos))  
#define   RTC_SR_TIMEV_NO_TIMEVENT_Val        _U_(0x0)                                             /**< (RTC_SR) No time event has occurred since the last clear.  */
#define   RTC_SR_TIMEV_TIMEVENT_Val           _U_(0x1)                                             /**< (RTC_SR) At least one time event has occurred since the last clear.  */
#define RTC_SR_TIMEV_NO_TIMEVENT              (RTC_SR_TIMEV_NO_TIMEVENT_Val << RTC_SR_TIMEV_Pos)   /**< (RTC_SR) No time event has occurred since the last clear. Position  */
#define RTC_SR_TIMEV_TIMEVENT                 (RTC_SR_TIMEV_TIMEVENT_Val << RTC_SR_TIMEV_Pos)      /**< (RTC_SR) At least one time event has occurred since the last clear. Position  */
#define RTC_SR_CALEV_Pos                      _U_(4)                                               /**< (RTC_SR) Calendar Event Position */
#define RTC_SR_CALEV_Msk                      (_U_(0x1) << RTC_SR_CALEV_Pos)                       /**< (RTC_SR) Calendar Event Mask */
#define RTC_SR_CALEV(value)                   (RTC_SR_CALEV_Msk & ((value) << RTC_SR_CALEV_Pos))  
#define   RTC_SR_CALEV_NO_CALEVENT_Val        _U_(0x0)                                             /**< (RTC_SR) No calendar event has occurred since the last clear.  */
#define   RTC_SR_CALEV_CALEVENT_Val           _U_(0x1)                                             /**< (RTC_SR) At least one calendar event has occurred since the last clear.  */
#define RTC_SR_CALEV_NO_CALEVENT              (RTC_SR_CALEV_NO_CALEVENT_Val << RTC_SR_CALEV_Pos)   /**< (RTC_SR) No calendar event has occurred since the last clear. Position  */
#define RTC_SR_CALEV_CALEVENT                 (RTC_SR_CALEV_CALEVENT_Val << RTC_SR_CALEV_Pos)      /**< (RTC_SR) At least one calendar event has occurred since the last clear. Position  */
#define RTC_SR_TDERR_Pos                      _U_(5)                                               /**< (RTC_SR) Time and/or Date Free Running Error Position */
#define RTC_SR_TDERR_Msk                      (_U_(0x1) << RTC_SR_TDERR_Pos)                       /**< (RTC_SR) Time and/or Date Free Running Error Mask */
#define RTC_SR_TDERR(value)                   (RTC_SR_TDERR_Msk & ((value) << RTC_SR_TDERR_Pos))  
#define   RTC_SR_TDERR_CORRECT_Val            _U_(0x0)                                             /**< (RTC_SR) The internal free running counters are carrying valid values since the last read of the Status Register (RTC_SR).  */
#define   RTC_SR_TDERR_ERR_TIMEDATE_Val       _U_(0x1)                                             /**< (RTC_SR) The internal free running counters have been corrupted (invalid date or time, non-BCD values) since the last read and/or they are still invalid.  */
#define RTC_SR_TDERR_CORRECT                  (RTC_SR_TDERR_CORRECT_Val << RTC_SR_TDERR_Pos)       /**< (RTC_SR) The internal free running counters are carrying valid values since the last read of the Status Register (RTC_SR). Position  */
#define RTC_SR_TDERR_ERR_TIMEDATE             (RTC_SR_TDERR_ERR_TIMEDATE_Val << RTC_SR_TDERR_Pos)  /**< (RTC_SR) The internal free running counters have been corrupted (invalid date or time, non-BCD values) since the last read and/or they are still invalid. Position  */
#define RTC_SR_Msk                            _U_(0x0000003F)                                      /**< (RTC_SR) Register Mask  */


/* -------- RTC_SCCR : (RTC Offset: 0x1C) ( /W 32) Status Clear Command Register -------- */
#define RTC_SCCR_ACKCLR_Pos                   _U_(0)                                               /**< (RTC_SCCR) Acknowledge Clear Position */
#define RTC_SCCR_ACKCLR_Msk                   (_U_(0x1) << RTC_SCCR_ACKCLR_Pos)                    /**< (RTC_SCCR) Acknowledge Clear Mask */
#define RTC_SCCR_ACKCLR(value)                (RTC_SCCR_ACKCLR_Msk & ((value) << RTC_SCCR_ACKCLR_Pos))
#define RTC_SCCR_ALRCLR_Pos                   _U_(1)                                               /**< (RTC_SCCR) Alarm Clear Position */
#define RTC_SCCR_ALRCLR_Msk                   (_U_(0x1) << RTC_SCCR_ALRCLR_Pos)                    /**< (RTC_SCCR) Alarm Clear Mask */
#define RTC_SCCR_ALRCLR(value)                (RTC_SCCR_ALRCLR_Msk & ((value) << RTC_SCCR_ALRCLR_Pos))
#define RTC_SCCR_SECCLR_Pos                   _U_(2)                                               /**< (RTC_SCCR) Second Clear Position */
#define RTC_SCCR_SECCLR_Msk                   (_U_(0x1) << RTC_SCCR_SECCLR_Pos)                    /**< (RTC_SCCR) Second Clear Mask */
#define RTC_SCCR_SECCLR(value)                (RTC_SCCR_SECCLR_Msk & ((value) << RTC_SCCR_SECCLR_Pos))
#define RTC_SCCR_TIMCLR_Pos                   _U_(3)                                               /**< (RTC_SCCR) Time Clear Position */
#define RTC_SCCR_TIMCLR_Msk                   (_U_(0x1) << RTC_SCCR_TIMCLR_Pos)                    /**< (RTC_SCCR) Time Clear Mask */
#define RTC_SCCR_TIMCLR(value)                (RTC_SCCR_TIMCLR_Msk & ((value) << RTC_SCCR_TIMCLR_Pos))
#define RTC_SCCR_CALCLR_Pos                   _U_(4)                                               /**< (RTC_SCCR) Calendar Clear Position */
#define RTC_SCCR_CALCLR_Msk                   (_U_(0x1) << RTC_SCCR_CALCLR_Pos)                    /**< (RTC_SCCR) Calendar Clear Mask */
#define RTC_SCCR_CALCLR(value)                (RTC_SCCR_CALCLR_Msk & ((value) << RTC_SCCR_CALCLR_Pos))
#define RTC_SCCR_TDERRCLR_Pos                 _U_(5)                                               /**< (RTC_SCCR) Time and/or Date Free Running Error Clear Position */
#define RTC_SCCR_TDERRCLR_Msk                 (_U_(0x1) << RTC_SCCR_TDERRCLR_Pos)                  /**< (RTC_SCCR) Time and/or Date Free Running Error Clear Mask */
#define RTC_SCCR_TDERRCLR(value)              (RTC_SCCR_TDERRCLR_Msk & ((value) << RTC_SCCR_TDERRCLR_Pos))
#define RTC_SCCR_Msk                          _U_(0x0000003F)                                      /**< (RTC_SCCR) Register Mask  */


/* -------- RTC_IER : (RTC Offset: 0x20) ( /W 32) Interrupt Enable Register -------- */
#define RTC_IER_ACKEN_Pos                     _U_(0)                                               /**< (RTC_IER) Acknowledge Update Interrupt Enable Position */
#define RTC_IER_ACKEN_Msk                     (_U_(0x1) << RTC_IER_ACKEN_Pos)                      /**< (RTC_IER) Acknowledge Update Interrupt Enable Mask */
#define RTC_IER_ACKEN(value)                  (RTC_IER_ACKEN_Msk & ((value) << RTC_IER_ACKEN_Pos))
#define RTC_IER_ALREN_Pos                     _U_(1)                                               /**< (RTC_IER) Alarm Interrupt Enable Position */
#define RTC_IER_ALREN_Msk                     (_U_(0x1) << RTC_IER_ALREN_Pos)                      /**< (RTC_IER) Alarm Interrupt Enable Mask */
#define RTC_IER_ALREN(value)                  (RTC_IER_ALREN_Msk & ((value) << RTC_IER_ALREN_Pos))
#define RTC_IER_SECEN_Pos                     _U_(2)                                               /**< (RTC_IER) Second Event Interrupt Enable Position */
#define RTC_IER_SECEN_Msk                     (_U_(0x1) << RTC_IER_SECEN_Pos)                      /**< (RTC_IER) Second Event Interrupt Enable Mask */
#define RTC_IER_SECEN(value)                  (RTC_IER_SECEN_Msk & ((value) << RTC_IER_SECEN_Pos))
#define RTC_IER_TIMEN_Pos                     _U_(3)                                               /**< (RTC_IER) Time Event Interrupt Enable Position */
#define RTC_IER_TIMEN_Msk                     (_U_(0x1) << RTC_IER_TIMEN_Pos)                      /**< (RTC_IER) Time Event Interrupt Enable Mask */
#define RTC_IER_TIMEN(value)                  (RTC_IER_TIMEN_Msk & ((value) << RTC_IER_TIMEN_Pos))
#define RTC_IER_CALEN_Pos                     _U_(4)                                               /**< (RTC_IER) Calendar Event Interrupt Enable Position */
#define RTC_IER_CALEN_Msk                     (_U_(0x1) << RTC_IER_CALEN_Pos)                      /**< (RTC_IER) Calendar Event Interrupt Enable Mask */
#define RTC_IER_CALEN(value)                  (RTC_IER_CALEN_Msk & ((value) << RTC_IER_CALEN_Pos))
#define RTC_IER_TDERREN_Pos                   _U_(5)                                               /**< (RTC_IER) Time and/or Date Error Interrupt Enable Position */
#define RTC_IER_TDERREN_Msk                   (_U_(0x1) << RTC_IER_TDERREN_Pos)                    /**< (RTC_IER) Time and/or Date Error Interrupt Enable Mask */
#define RTC_IER_TDERREN(value)                (RTC_IER_TDERREN_Msk & ((value) << RTC_IER_TDERREN_Pos))
#define RTC_IER_Msk                           _U_(0x0000003F)                                      /**< (RTC_IER) Register Mask  */


/* -------- RTC_IDR : (RTC Offset: 0x24) ( /W 32) Interrupt Disable Register -------- */
#define RTC_IDR_ACKDIS_Pos                    _U_(0)                                               /**< (RTC_IDR) Acknowledge Update Interrupt Disable Position */
#define RTC_IDR_ACKDIS_Msk                    (_U_(0x1) << RTC_IDR_ACKDIS_Pos)                     /**< (RTC_IDR) Acknowledge Update Interrupt Disable Mask */
#define RTC_IDR_ACKDIS(value)                 (RTC_IDR_ACKDIS_Msk & ((value) << RTC_IDR_ACKDIS_Pos))
#define RTC_IDR_ALRDIS_Pos                    _U_(1)                                               /**< (RTC_IDR) Alarm Interrupt Disable Position */
#define RTC_IDR_ALRDIS_Msk                    (_U_(0x1) << RTC_IDR_ALRDIS_Pos)                     /**< (RTC_IDR) Alarm Interrupt Disable Mask */
#define RTC_IDR_ALRDIS(value)                 (RTC_IDR_ALRDIS_Msk & ((value) << RTC_IDR_ALRDIS_Pos))
#define RTC_IDR_SECDIS_Pos                    _U_(2)                                               /**< (RTC_IDR) Second Event Interrupt Disable Position */
#define RTC_IDR_SECDIS_Msk                    (_U_(0x1) << RTC_IDR_SECDIS_Pos)                     /**< (RTC_IDR) Second Event Interrupt Disable Mask */
#define RTC_IDR_SECDIS(value)                 (RTC_IDR_SECDIS_Msk & ((value) << RTC_IDR_SECDIS_Pos))
#define RTC_IDR_TIMDIS_Pos                    _U_(3)                                               /**< (RTC_IDR) Time Event Interrupt Disable Position */
#define RTC_IDR_TIMDIS_Msk                    (_U_(0x1) << RTC_IDR_TIMDIS_Pos)                     /**< (RTC_IDR) Time Event Interrupt Disable Mask */
#define RTC_IDR_TIMDIS(value)                 (RTC_IDR_TIMDIS_Msk & ((value) << RTC_IDR_TIMDIS_Pos))
#define RTC_IDR_CALDIS_Pos                    _U_(4)                                               /**< (RTC_IDR) Calendar Event Interrupt Disable Position */
#define RTC_IDR_CALDIS_Msk                    (_U_(0x1) << RTC_IDR_CALDIS_Pos)                     /**< (RTC_IDR) Calendar Event Interrupt Disable Mask */
#define RTC_IDR_CALDIS(value)                 (RTC_IDR_CALDIS_Msk & ((value) << RTC_IDR_CALDIS_Pos))
#define RTC_IDR_TDERRDIS_Pos                  _U_(5)                                               /**< (RTC_IDR) Time and/or Date Error Interrupt Disable Position */
#define RTC_IDR_TDERRDIS_Msk                  (_U_(0x1) << RTC_IDR_TDERRDIS_Pos)                   /**< (RTC_IDR) Time and/or Date Error Interrupt Disable Mask */
#define RTC_IDR_TDERRDIS(value)               (RTC_IDR_TDERRDIS_Msk & ((value) << RTC_IDR_TDERRDIS_Pos))
#define RTC_IDR_Msk                           _U_(0x0000003F)                                      /**< (RTC_IDR) Register Mask  */


/* -------- RTC_IMR : (RTC Offset: 0x28) ( R/ 32) Interrupt Mask Register -------- */
#define RTC_IMR_ACK_Pos                       _U_(0)                                               /**< (RTC_IMR) Acknowledge Update Interrupt Mask Position */
#define RTC_IMR_ACK_Msk                       (_U_(0x1) << RTC_IMR_ACK_Pos)                        /**< (RTC_IMR) Acknowledge Update Interrupt Mask Mask */
#define RTC_IMR_ACK(value)                    (RTC_IMR_ACK_Msk & ((value) << RTC_IMR_ACK_Pos))    
#define RTC_IMR_ALR_Pos                       _U_(1)                                               /**< (RTC_IMR) Alarm Interrupt Mask Position */
#define RTC_IMR_ALR_Msk                       (_U_(0x1) << RTC_IMR_ALR_Pos)                        /**< (RTC_IMR) Alarm Interrupt Mask Mask */
#define RTC_IMR_ALR(value)                    (RTC_IMR_ALR_Msk & ((value) << RTC_IMR_ALR_Pos))    
#define RTC_IMR_SEC_Pos                       _U_(2)                                               /**< (RTC_IMR) Second Event Interrupt Mask Position */
#define RTC_IMR_SEC_Msk                       (_U_(0x1) << RTC_IMR_SEC_Pos)                        /**< (RTC_IMR) Second Event Interrupt Mask Mask */
#define RTC_IMR_SEC(value)                    (RTC_IMR_SEC_Msk & ((value) << RTC_IMR_SEC_Pos))    
#define RTC_IMR_TIM_Pos                       _U_(3)                                               /**< (RTC_IMR) Time Event Interrupt Mask Position */
#define RTC_IMR_TIM_Msk                       (_U_(0x1) << RTC_IMR_TIM_Pos)                        /**< (RTC_IMR) Time Event Interrupt Mask Mask */
#define RTC_IMR_TIM(value)                    (RTC_IMR_TIM_Msk & ((value) << RTC_IMR_TIM_Pos))    
#define RTC_IMR_CAL_Pos                       _U_(4)                                               /**< (RTC_IMR) Calendar Event Interrupt Mask Position */
#define RTC_IMR_CAL_Msk                       (_U_(0x1) << RTC_IMR_CAL_Pos)                        /**< (RTC_IMR) Calendar Event Interrupt Mask Mask */
#define RTC_IMR_CAL(value)                    (RTC_IMR_CAL_Msk & ((value) << RTC_IMR_CAL_Pos))    
#define RTC_IMR_TDERR_Pos                     _U_(5)                                               /**< (RTC_IMR) Time and/or Date Error Mask Position */
#define RTC_IMR_TDERR_Msk                     (_U_(0x1) << RTC_IMR_TDERR_Pos)                      /**< (RTC_IMR) Time and/or Date Error Mask Mask */
#define RTC_IMR_TDERR(value)                  (RTC_IMR_TDERR_Msk & ((value) << RTC_IMR_TDERR_Pos))
#define RTC_IMR_Msk                           _U_(0x0000003F)                                      /**< (RTC_IMR) Register Mask  */


/* -------- RTC_VER : (RTC Offset: 0x2C) ( R/ 32) Valid Entry Register -------- */
#define RTC_VER_NVTIM_Pos                     _U_(0)                                               /**< (RTC_VER) Non-valid Time Position */
#define RTC_VER_NVTIM_Msk                     (_U_(0x1) << RTC_VER_NVTIM_Pos)                      /**< (RTC_VER) Non-valid Time Mask */
#define RTC_VER_NVTIM(value)                  (RTC_VER_NVTIM_Msk & ((value) << RTC_VER_NVTIM_Pos))
#define RTC_VER_NVCAL_Pos                     _U_(1)                                               /**< (RTC_VER) Non-valid Calendar Position */
#define RTC_VER_NVCAL_Msk                     (_U_(0x1) << RTC_VER_NVCAL_Pos)                      /**< (RTC_VER) Non-valid Calendar Mask */
#define RTC_VER_NVCAL(value)                  (RTC_VER_NVCAL_Msk & ((value) << RTC_VER_NVCAL_Pos))
#define RTC_VER_NVTIMALR_Pos                  _U_(2)                                               /**< (RTC_VER) Non-valid Time Alarm Position */
#define RTC_VER_NVTIMALR_Msk                  (_U_(0x1) << RTC_VER_NVTIMALR_Pos)                   /**< (RTC_VER) Non-valid Time Alarm Mask */
#define RTC_VER_NVTIMALR(value)               (RTC_VER_NVTIMALR_Msk & ((value) << RTC_VER_NVTIMALR_Pos))
#define RTC_VER_NVCALALR_Pos                  _U_(3)                                               /**< (RTC_VER) Non-valid Calendar Alarm Position */
#define RTC_VER_NVCALALR_Msk                  (_U_(0x1) << RTC_VER_NVCALALR_Pos)                   /**< (RTC_VER) Non-valid Calendar Alarm Mask */
#define RTC_VER_NVCALALR(value)               (RTC_VER_NVCALALR_Msk & ((value) << RTC_VER_NVCALALR_Pos))
#define RTC_VER_Msk                           _U_(0x0000000F)                                      /**< (RTC_VER) Register Mask  */


/** \brief RTC register offsets definitions */
#define RTC_CR_REG_OFST                (0x00)              /**< (RTC_CR) Control Register Offset */
#define RTC_MR_REG_OFST                (0x04)              /**< (RTC_MR) Mode Register Offset */
#define RTC_TIMR_REG_OFST              (0x08)              /**< (RTC_TIMR) Time Register Offset */
#define RTC_CALR_REG_OFST              (0x0C)              /**< (RTC_CALR) Calendar Register Offset */
#define RTC_TIMALR_REG_OFST            (0x10)              /**< (RTC_TIMALR) Time Alarm Register Offset */
#define RTC_CALALR_REG_OFST            (0x14)              /**< (RTC_CALALR) Calendar Alarm Register Offset */
#define RTC_SR_REG_OFST                (0x18)              /**< (RTC_SR) Status Register Offset */
#define RTC_SCCR_REG_OFST              (0x1C)              /**< (RTC_SCCR) Status Clear Command Register Offset */
#define RTC_IER_REG_OFST               (0x20)              /**< (RTC_IER) Interrupt Enable Register Offset */
#define RTC_IDR_REG_OFST               (0x24)              /**< (RTC_IDR) Interrupt Disable Register Offset */
#define RTC_IMR_REG_OFST               (0x28)              /**< (RTC_IMR) Interrupt Mask Register Offset */
#define RTC_VER_REG_OFST               (0x2C)              /**< (RTC_VER) Valid Entry Register Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief RTC register API structure */
typedef struct
{
  __IO  uint32_t                       RTC_CR;             /**< Offset: 0x00 (R/W  32) Control Register */
  __IO  uint32_t                       RTC_MR;             /**< Offset: 0x04 (R/W  32) Mode Register */
  __IO  uint32_t                       RTC_TIMR;           /**< Offset: 0x08 (R/W  32) Time Register */
  __IO  uint32_t                       RTC_CALR;           /**< Offset: 0x0C (R/W  32) Calendar Register */
  __IO  uint32_t                       RTC_TIMALR;         /**< Offset: 0x10 (R/W  32) Time Alarm Register */
  __IO  uint32_t                       RTC_CALALR;         /**< Offset: 0x14 (R/W  32) Calendar Alarm Register */
  __I   uint32_t                       RTC_SR;             /**< Offset: 0x18 (R/   32) Status Register */
  __O   uint32_t                       RTC_SCCR;           /**< Offset: 0x1C ( /W  32) Status Clear Command Register */
  __O   uint32_t                       RTC_IER;            /**< Offset: 0x20 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       RTC_IDR;            /**< Offset: 0x24 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       RTC_IMR;            /**< Offset: 0x28 (R/   32) Interrupt Mask Register */
  __I   uint32_t                       RTC_VER;            /**< Offset: 0x2C (R/   32) Valid Entry Register */
} rtc_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME70_RTC_COMPONENT_H_ */
