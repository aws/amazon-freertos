/**
 * \brief Component description for SERCOM
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
#ifndef _SAME54_SERCOM_COMPONENT_H_
#define _SAME54_SERCOM_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR SERCOM                                       */
/* ************************************************************************** */

/* -------- SERCOM_I2CM_CTRLA : (SERCOM Offset: 0x00) (R/W 32) I2CM Control A -------- */
#define SERCOM_I2CM_CTRLA_RESETVALUE          _U_(0x00)                                            /**<  (SERCOM_I2CM_CTRLA) I2CM Control A  Reset Value */

#define SERCOM_I2CM_CTRLA_SWRST_Pos           _U_(0)                                               /**< (SERCOM_I2CM_CTRLA) Software Reset Position */
#define SERCOM_I2CM_CTRLA_SWRST_Msk           (_U_(0x1) << SERCOM_I2CM_CTRLA_SWRST_Pos)            /**< (SERCOM_I2CM_CTRLA) Software Reset Mask */
#define SERCOM_I2CM_CTRLA_SWRST(value)        (SERCOM_I2CM_CTRLA_SWRST_Msk & ((value) << SERCOM_I2CM_CTRLA_SWRST_Pos))
#define SERCOM_I2CM_CTRLA_ENABLE_Pos          _U_(1)                                               /**< (SERCOM_I2CM_CTRLA) Enable Position */
#define SERCOM_I2CM_CTRLA_ENABLE_Msk          (_U_(0x1) << SERCOM_I2CM_CTRLA_ENABLE_Pos)           /**< (SERCOM_I2CM_CTRLA) Enable Mask */
#define SERCOM_I2CM_CTRLA_ENABLE(value)       (SERCOM_I2CM_CTRLA_ENABLE_Msk & ((value) << SERCOM_I2CM_CTRLA_ENABLE_Pos))
#define SERCOM_I2CM_CTRLA_MODE_Pos            _U_(2)                                               /**< (SERCOM_I2CM_CTRLA) Operating Mode Position */
#define SERCOM_I2CM_CTRLA_MODE_Msk            (_U_(0x7) << SERCOM_I2CM_CTRLA_MODE_Pos)             /**< (SERCOM_I2CM_CTRLA) Operating Mode Mask */
#define SERCOM_I2CM_CTRLA_MODE(value)         (SERCOM_I2CM_CTRLA_MODE_Msk & ((value) << SERCOM_I2CM_CTRLA_MODE_Pos))
#define   SERCOM_I2CM_CTRLA_MODE_USART_EXT_CLK_Val _U_(0x0)                                             /**< (SERCOM_I2CM_CTRLA) USART with external clock  */
#define   SERCOM_I2CM_CTRLA_MODE_USART_INT_CLK_Val _U_(0x1)                                             /**< (SERCOM_I2CM_CTRLA) USART with internal clock  */
#define   SERCOM_I2CM_CTRLA_MODE_SPI_SLAVE_Val _U_(0x2)                                             /**< (SERCOM_I2CM_CTRLA) SPI in slave operation  */
#define   SERCOM_I2CM_CTRLA_MODE_SPI_MASTER_Val _U_(0x3)                                             /**< (SERCOM_I2CM_CTRLA) SPI in master operation  */
#define   SERCOM_I2CM_CTRLA_MODE_I2C_SLAVE_Val _U_(0x4)                                             /**< (SERCOM_I2CM_CTRLA) I2C slave operation  */
#define   SERCOM_I2CM_CTRLA_MODE_I2C_MASTER_Val _U_(0x5)                                             /**< (SERCOM_I2CM_CTRLA) I2C master operation  */
#define SERCOM_I2CM_CTRLA_MODE_USART_EXT_CLK  (SERCOM_I2CM_CTRLA_MODE_USART_EXT_CLK_Val << SERCOM_I2CM_CTRLA_MODE_Pos) /**< (SERCOM_I2CM_CTRLA) USART with external clock Position  */
#define SERCOM_I2CM_CTRLA_MODE_USART_INT_CLK  (SERCOM_I2CM_CTRLA_MODE_USART_INT_CLK_Val << SERCOM_I2CM_CTRLA_MODE_Pos) /**< (SERCOM_I2CM_CTRLA) USART with internal clock Position  */
#define SERCOM_I2CM_CTRLA_MODE_SPI_SLAVE      (SERCOM_I2CM_CTRLA_MODE_SPI_SLAVE_Val << SERCOM_I2CM_CTRLA_MODE_Pos) /**< (SERCOM_I2CM_CTRLA) SPI in slave operation Position  */
#define SERCOM_I2CM_CTRLA_MODE_SPI_MASTER     (SERCOM_I2CM_CTRLA_MODE_SPI_MASTER_Val << SERCOM_I2CM_CTRLA_MODE_Pos) /**< (SERCOM_I2CM_CTRLA) SPI in master operation Position  */
#define SERCOM_I2CM_CTRLA_MODE_I2C_SLAVE      (SERCOM_I2CM_CTRLA_MODE_I2C_SLAVE_Val << SERCOM_I2CM_CTRLA_MODE_Pos) /**< (SERCOM_I2CM_CTRLA) I2C slave operation Position  */
#define SERCOM_I2CM_CTRLA_MODE_I2C_MASTER     (SERCOM_I2CM_CTRLA_MODE_I2C_MASTER_Val << SERCOM_I2CM_CTRLA_MODE_Pos) /**< (SERCOM_I2CM_CTRLA) I2C master operation Position  */
#define SERCOM_I2CM_CTRLA_RUNSTDBY_Pos        _U_(7)                                               /**< (SERCOM_I2CM_CTRLA) Run in Standby Position */
#define SERCOM_I2CM_CTRLA_RUNSTDBY_Msk        (_U_(0x1) << SERCOM_I2CM_CTRLA_RUNSTDBY_Pos)         /**< (SERCOM_I2CM_CTRLA) Run in Standby Mask */
#define SERCOM_I2CM_CTRLA_RUNSTDBY(value)     (SERCOM_I2CM_CTRLA_RUNSTDBY_Msk & ((value) << SERCOM_I2CM_CTRLA_RUNSTDBY_Pos))
#define SERCOM_I2CM_CTRLA_PINOUT_Pos          _U_(16)                                              /**< (SERCOM_I2CM_CTRLA) Pin Usage Position */
#define SERCOM_I2CM_CTRLA_PINOUT_Msk          (_U_(0x1) << SERCOM_I2CM_CTRLA_PINOUT_Pos)           /**< (SERCOM_I2CM_CTRLA) Pin Usage Mask */
#define SERCOM_I2CM_CTRLA_PINOUT(value)       (SERCOM_I2CM_CTRLA_PINOUT_Msk & ((value) << SERCOM_I2CM_CTRLA_PINOUT_Pos))
#define SERCOM_I2CM_CTRLA_SDAHOLD_Pos         _U_(20)                                              /**< (SERCOM_I2CM_CTRLA) SDA Hold Time Position */
#define SERCOM_I2CM_CTRLA_SDAHOLD_Msk         (_U_(0x3) << SERCOM_I2CM_CTRLA_SDAHOLD_Pos)          /**< (SERCOM_I2CM_CTRLA) SDA Hold Time Mask */
#define SERCOM_I2CM_CTRLA_SDAHOLD(value)      (SERCOM_I2CM_CTRLA_SDAHOLD_Msk & ((value) << SERCOM_I2CM_CTRLA_SDAHOLD_Pos))
#define   SERCOM_I2CM_CTRLA_SDAHOLD_DISABLE_Val _U_(0x0)                                             /**< (SERCOM_I2CM_CTRLA) Disabled  */
#define   SERCOM_I2CM_CTRLA_SDAHOLD_75NS_Val  _U_(0x1)                                             /**< (SERCOM_I2CM_CTRLA) 50-100ns hold time  */
#define   SERCOM_I2CM_CTRLA_SDAHOLD_450NS_Val _U_(0x2)                                             /**< (SERCOM_I2CM_CTRLA) 300-600ns hold time  */
#define   SERCOM_I2CM_CTRLA_SDAHOLD_600NS_Val _U_(0x3)                                             /**< (SERCOM_I2CM_CTRLA) 400-800ns hold time  */
#define SERCOM_I2CM_CTRLA_SDAHOLD_DISABLE     (SERCOM_I2CM_CTRLA_SDAHOLD_DISABLE_Val << SERCOM_I2CM_CTRLA_SDAHOLD_Pos) /**< (SERCOM_I2CM_CTRLA) Disabled Position  */
#define SERCOM_I2CM_CTRLA_SDAHOLD_75NS        (SERCOM_I2CM_CTRLA_SDAHOLD_75NS_Val << SERCOM_I2CM_CTRLA_SDAHOLD_Pos) /**< (SERCOM_I2CM_CTRLA) 50-100ns hold time Position  */
#define SERCOM_I2CM_CTRLA_SDAHOLD_450NS       (SERCOM_I2CM_CTRLA_SDAHOLD_450NS_Val << SERCOM_I2CM_CTRLA_SDAHOLD_Pos) /**< (SERCOM_I2CM_CTRLA) 300-600ns hold time Position  */
#define SERCOM_I2CM_CTRLA_SDAHOLD_600NS       (SERCOM_I2CM_CTRLA_SDAHOLD_600NS_Val << SERCOM_I2CM_CTRLA_SDAHOLD_Pos) /**< (SERCOM_I2CM_CTRLA) 400-800ns hold time Position  */
#define SERCOM_I2CM_CTRLA_MEXTTOEN_Pos        _U_(22)                                              /**< (SERCOM_I2CM_CTRLA) Master SCL Low Extend Timeout Position */
#define SERCOM_I2CM_CTRLA_MEXTTOEN_Msk        (_U_(0x1) << SERCOM_I2CM_CTRLA_MEXTTOEN_Pos)         /**< (SERCOM_I2CM_CTRLA) Master SCL Low Extend Timeout Mask */
#define SERCOM_I2CM_CTRLA_MEXTTOEN(value)     (SERCOM_I2CM_CTRLA_MEXTTOEN_Msk & ((value) << SERCOM_I2CM_CTRLA_MEXTTOEN_Pos))
#define SERCOM_I2CM_CTRLA_SEXTTOEN_Pos        _U_(23)                                              /**< (SERCOM_I2CM_CTRLA) Slave SCL Low Extend Timeout Position */
#define SERCOM_I2CM_CTRLA_SEXTTOEN_Msk        (_U_(0x1) << SERCOM_I2CM_CTRLA_SEXTTOEN_Pos)         /**< (SERCOM_I2CM_CTRLA) Slave SCL Low Extend Timeout Mask */
#define SERCOM_I2CM_CTRLA_SEXTTOEN(value)     (SERCOM_I2CM_CTRLA_SEXTTOEN_Msk & ((value) << SERCOM_I2CM_CTRLA_SEXTTOEN_Pos))
#define SERCOM_I2CM_CTRLA_SPEED_Pos           _U_(24)                                              /**< (SERCOM_I2CM_CTRLA) Transfer Speed Position */
#define SERCOM_I2CM_CTRLA_SPEED_Msk           (_U_(0x3) << SERCOM_I2CM_CTRLA_SPEED_Pos)            /**< (SERCOM_I2CM_CTRLA) Transfer Speed Mask */
#define SERCOM_I2CM_CTRLA_SPEED(value)        (SERCOM_I2CM_CTRLA_SPEED_Msk & ((value) << SERCOM_I2CM_CTRLA_SPEED_Pos))
#define   SERCOM_I2CM_CTRLA_SPEED_STANDARD_AND_FAST_MODE_Val _U_(0x0)                                             /**< (SERCOM_I2CM_CTRLA) Standard Mode(Sm) Upto 100kHz and Fast Mode(Fm) Upto 400kHz   */
#define   SERCOM_I2CM_CTRLA_SPEED_FASTPLUS_MODE_Val _U_(0x1)                                             /**< (SERCOM_I2CM_CTRLA) Fast-mode Plus Upto 1MHz  */
#define   SERCOM_I2CM_CTRLA_SPEED_HIGH_SPEED_MODE_Val _U_(0x2)                                             /**< (SERCOM_I2CM_CTRLA) High-speed mode Upto 3.4MHz  */
#define SERCOM_I2CM_CTRLA_SPEED_STANDARD_AND_FAST_MODE (SERCOM_I2CM_CTRLA_SPEED_STANDARD_AND_FAST_MODE_Val << SERCOM_I2CM_CTRLA_SPEED_Pos) /**< (SERCOM_I2CM_CTRLA) Standard Mode(Sm) Upto 100kHz and Fast Mode(Fm) Upto 400kHz  Position  */
#define SERCOM_I2CM_CTRLA_SPEED_FASTPLUS_MODE (SERCOM_I2CM_CTRLA_SPEED_FASTPLUS_MODE_Val << SERCOM_I2CM_CTRLA_SPEED_Pos) /**< (SERCOM_I2CM_CTRLA) Fast-mode Plus Upto 1MHz Position  */
#define SERCOM_I2CM_CTRLA_SPEED_HIGH_SPEED_MODE (SERCOM_I2CM_CTRLA_SPEED_HIGH_SPEED_MODE_Val << SERCOM_I2CM_CTRLA_SPEED_Pos) /**< (SERCOM_I2CM_CTRLA) High-speed mode Upto 3.4MHz Position  */
#define SERCOM_I2CM_CTRLA_SCLSM_Pos           _U_(27)                                              /**< (SERCOM_I2CM_CTRLA) SCL Clock Stretch Mode Position */
#define SERCOM_I2CM_CTRLA_SCLSM_Msk           (_U_(0x1) << SERCOM_I2CM_CTRLA_SCLSM_Pos)            /**< (SERCOM_I2CM_CTRLA) SCL Clock Stretch Mode Mask */
#define SERCOM_I2CM_CTRLA_SCLSM(value)        (SERCOM_I2CM_CTRLA_SCLSM_Msk & ((value) << SERCOM_I2CM_CTRLA_SCLSM_Pos))
#define SERCOM_I2CM_CTRLA_INACTOUT_Pos        _U_(28)                                              /**< (SERCOM_I2CM_CTRLA) Inactive Time-Out Position */
#define SERCOM_I2CM_CTRLA_INACTOUT_Msk        (_U_(0x3) << SERCOM_I2CM_CTRLA_INACTOUT_Pos)         /**< (SERCOM_I2CM_CTRLA) Inactive Time-Out Mask */
#define SERCOM_I2CM_CTRLA_INACTOUT(value)     (SERCOM_I2CM_CTRLA_INACTOUT_Msk & ((value) << SERCOM_I2CM_CTRLA_INACTOUT_Pos))
#define   SERCOM_I2CM_CTRLA_INACTOUT_DISABLE_Val _U_(0x0)                                             /**< (SERCOM_I2CM_CTRLA) Disabled  */
#define   SERCOM_I2CM_CTRLA_INACTOUT_55US_Val _U_(0x1)                                             /**< (SERCOM_I2CM_CTRLA) 5-6 SCL Time-Out(50-60us)  */
#define   SERCOM_I2CM_CTRLA_INACTOUT_105US_Val _U_(0x2)                                             /**< (SERCOM_I2CM_CTRLA) 10-11 SCL Time-Out(100-110us)  */
#define   SERCOM_I2CM_CTRLA_INACTOUT_205US_Val _U_(0x3)                                             /**< (SERCOM_I2CM_CTRLA) 20-21 SCL Time-Out(200-210us)  */
#define SERCOM_I2CM_CTRLA_INACTOUT_DISABLE    (SERCOM_I2CM_CTRLA_INACTOUT_DISABLE_Val << SERCOM_I2CM_CTRLA_INACTOUT_Pos) /**< (SERCOM_I2CM_CTRLA) Disabled Position  */
#define SERCOM_I2CM_CTRLA_INACTOUT_55US       (SERCOM_I2CM_CTRLA_INACTOUT_55US_Val << SERCOM_I2CM_CTRLA_INACTOUT_Pos) /**< (SERCOM_I2CM_CTRLA) 5-6 SCL Time-Out(50-60us) Position  */
#define SERCOM_I2CM_CTRLA_INACTOUT_105US      (SERCOM_I2CM_CTRLA_INACTOUT_105US_Val << SERCOM_I2CM_CTRLA_INACTOUT_Pos) /**< (SERCOM_I2CM_CTRLA) 10-11 SCL Time-Out(100-110us) Position  */
#define SERCOM_I2CM_CTRLA_INACTOUT_205US      (SERCOM_I2CM_CTRLA_INACTOUT_205US_Val << SERCOM_I2CM_CTRLA_INACTOUT_Pos) /**< (SERCOM_I2CM_CTRLA) 20-21 SCL Time-Out(200-210us) Position  */
#define SERCOM_I2CM_CTRLA_LOWTOUTEN_Pos       _U_(30)                                              /**< (SERCOM_I2CM_CTRLA) SCL Low Timeout Enable Position */
#define SERCOM_I2CM_CTRLA_LOWTOUTEN_Msk       (_U_(0x1) << SERCOM_I2CM_CTRLA_LOWTOUTEN_Pos)        /**< (SERCOM_I2CM_CTRLA) SCL Low Timeout Enable Mask */
#define SERCOM_I2CM_CTRLA_LOWTOUTEN(value)    (SERCOM_I2CM_CTRLA_LOWTOUTEN_Msk & ((value) << SERCOM_I2CM_CTRLA_LOWTOUTEN_Pos))
#define SERCOM_I2CM_CTRLA_Msk                 _U_(0x7BF1009F)                                      /**< (SERCOM_I2CM_CTRLA) Register Mask  */


/* -------- SERCOM_I2CS_CTRLA : (SERCOM Offset: 0x00) (R/W 32) I2CS Control A -------- */
#define SERCOM_I2CS_CTRLA_RESETVALUE          _U_(0x00)                                            /**<  (SERCOM_I2CS_CTRLA) I2CS Control A  Reset Value */

#define SERCOM_I2CS_CTRLA_SWRST_Pos           _U_(0)                                               /**< (SERCOM_I2CS_CTRLA) Software Reset Position */
#define SERCOM_I2CS_CTRLA_SWRST_Msk           (_U_(0x1) << SERCOM_I2CS_CTRLA_SWRST_Pos)            /**< (SERCOM_I2CS_CTRLA) Software Reset Mask */
#define SERCOM_I2CS_CTRLA_SWRST(value)        (SERCOM_I2CS_CTRLA_SWRST_Msk & ((value) << SERCOM_I2CS_CTRLA_SWRST_Pos))
#define SERCOM_I2CS_CTRLA_ENABLE_Pos          _U_(1)                                               /**< (SERCOM_I2CS_CTRLA) Enable Position */
#define SERCOM_I2CS_CTRLA_ENABLE_Msk          (_U_(0x1) << SERCOM_I2CS_CTRLA_ENABLE_Pos)           /**< (SERCOM_I2CS_CTRLA) Enable Mask */
#define SERCOM_I2CS_CTRLA_ENABLE(value)       (SERCOM_I2CS_CTRLA_ENABLE_Msk & ((value) << SERCOM_I2CS_CTRLA_ENABLE_Pos))
#define SERCOM_I2CS_CTRLA_MODE_Pos            _U_(2)                                               /**< (SERCOM_I2CS_CTRLA) Operating Mode Position */
#define SERCOM_I2CS_CTRLA_MODE_Msk            (_U_(0x7) << SERCOM_I2CS_CTRLA_MODE_Pos)             /**< (SERCOM_I2CS_CTRLA) Operating Mode Mask */
#define SERCOM_I2CS_CTRLA_MODE(value)         (SERCOM_I2CS_CTRLA_MODE_Msk & ((value) << SERCOM_I2CS_CTRLA_MODE_Pos))
#define   SERCOM_I2CS_CTRLA_MODE_USART_EXT_CLK_Val _U_(0x0)                                             /**< (SERCOM_I2CS_CTRLA) USART with external clock  */
#define   SERCOM_I2CS_CTRLA_MODE_USART_INT_CLK_Val _U_(0x1)                                             /**< (SERCOM_I2CS_CTRLA) USART with internal clock  */
#define   SERCOM_I2CS_CTRLA_MODE_SPI_SLAVE_Val _U_(0x2)                                             /**< (SERCOM_I2CS_CTRLA) SPI in slave operation  */
#define   SERCOM_I2CS_CTRLA_MODE_SPI_MASTER_Val _U_(0x3)                                             /**< (SERCOM_I2CS_CTRLA) SPI in master operation  */
#define   SERCOM_I2CS_CTRLA_MODE_I2C_SLAVE_Val _U_(0x4)                                             /**< (SERCOM_I2CS_CTRLA) I2C slave operation  */
#define   SERCOM_I2CS_CTRLA_MODE_I2C_MASTER_Val _U_(0x5)                                             /**< (SERCOM_I2CS_CTRLA) I2C master operation  */
#define SERCOM_I2CS_CTRLA_MODE_USART_EXT_CLK  (SERCOM_I2CS_CTRLA_MODE_USART_EXT_CLK_Val << SERCOM_I2CS_CTRLA_MODE_Pos) /**< (SERCOM_I2CS_CTRLA) USART with external clock Position  */
#define SERCOM_I2CS_CTRLA_MODE_USART_INT_CLK  (SERCOM_I2CS_CTRLA_MODE_USART_INT_CLK_Val << SERCOM_I2CS_CTRLA_MODE_Pos) /**< (SERCOM_I2CS_CTRLA) USART with internal clock Position  */
#define SERCOM_I2CS_CTRLA_MODE_SPI_SLAVE      (SERCOM_I2CS_CTRLA_MODE_SPI_SLAVE_Val << SERCOM_I2CS_CTRLA_MODE_Pos) /**< (SERCOM_I2CS_CTRLA) SPI in slave operation Position  */
#define SERCOM_I2CS_CTRLA_MODE_SPI_MASTER     (SERCOM_I2CS_CTRLA_MODE_SPI_MASTER_Val << SERCOM_I2CS_CTRLA_MODE_Pos) /**< (SERCOM_I2CS_CTRLA) SPI in master operation Position  */
#define SERCOM_I2CS_CTRLA_MODE_I2C_SLAVE      (SERCOM_I2CS_CTRLA_MODE_I2C_SLAVE_Val << SERCOM_I2CS_CTRLA_MODE_Pos) /**< (SERCOM_I2CS_CTRLA) I2C slave operation Position  */
#define SERCOM_I2CS_CTRLA_MODE_I2C_MASTER     (SERCOM_I2CS_CTRLA_MODE_I2C_MASTER_Val << SERCOM_I2CS_CTRLA_MODE_Pos) /**< (SERCOM_I2CS_CTRLA) I2C master operation Position  */
#define SERCOM_I2CS_CTRLA_RUNSTDBY_Pos        _U_(7)                                               /**< (SERCOM_I2CS_CTRLA) Run during Standby Position */
#define SERCOM_I2CS_CTRLA_RUNSTDBY_Msk        (_U_(0x1) << SERCOM_I2CS_CTRLA_RUNSTDBY_Pos)         /**< (SERCOM_I2CS_CTRLA) Run during Standby Mask */
#define SERCOM_I2CS_CTRLA_RUNSTDBY(value)     (SERCOM_I2CS_CTRLA_RUNSTDBY_Msk & ((value) << SERCOM_I2CS_CTRLA_RUNSTDBY_Pos))
#define SERCOM_I2CS_CTRLA_PINOUT_Pos          _U_(16)                                              /**< (SERCOM_I2CS_CTRLA) Pin Usage Position */
#define SERCOM_I2CS_CTRLA_PINOUT_Msk          (_U_(0x1) << SERCOM_I2CS_CTRLA_PINOUT_Pos)           /**< (SERCOM_I2CS_CTRLA) Pin Usage Mask */
#define SERCOM_I2CS_CTRLA_PINOUT(value)       (SERCOM_I2CS_CTRLA_PINOUT_Msk & ((value) << SERCOM_I2CS_CTRLA_PINOUT_Pos))
#define SERCOM_I2CS_CTRLA_SDAHOLD_Pos         _U_(20)                                              /**< (SERCOM_I2CS_CTRLA) SDA Hold Time Position */
#define SERCOM_I2CS_CTRLA_SDAHOLD_Msk         (_U_(0x3) << SERCOM_I2CS_CTRLA_SDAHOLD_Pos)          /**< (SERCOM_I2CS_CTRLA) SDA Hold Time Mask */
#define SERCOM_I2CS_CTRLA_SDAHOLD(value)      (SERCOM_I2CS_CTRLA_SDAHOLD_Msk & ((value) << SERCOM_I2CS_CTRLA_SDAHOLD_Pos))
#define   SERCOM_I2CS_CTRLA_SDAHOLD_DISABLE_Val _U_(0x0)                                             /**< (SERCOM_I2CS_CTRLA) Disabled  */
#define   SERCOM_I2CS_CTRLA_SDAHOLD_75NS_Val  _U_(0x1)                                             /**< (SERCOM_I2CS_CTRLA) 50-100ns hold time  */
#define   SERCOM_I2CS_CTRLA_SDAHOLD_450NS_Val _U_(0x2)                                             /**< (SERCOM_I2CS_CTRLA) 300-600ns hold time  */
#define   SERCOM_I2CS_CTRLA_SDAHOLD_600NS_Val _U_(0x3)                                             /**< (SERCOM_I2CS_CTRLA) 400-800ns hold time  */
#define SERCOM_I2CS_CTRLA_SDAHOLD_DISABLE     (SERCOM_I2CS_CTRLA_SDAHOLD_DISABLE_Val << SERCOM_I2CS_CTRLA_SDAHOLD_Pos) /**< (SERCOM_I2CS_CTRLA) Disabled Position  */
#define SERCOM_I2CS_CTRLA_SDAHOLD_75NS        (SERCOM_I2CS_CTRLA_SDAHOLD_75NS_Val << SERCOM_I2CS_CTRLA_SDAHOLD_Pos) /**< (SERCOM_I2CS_CTRLA) 50-100ns hold time Position  */
#define SERCOM_I2CS_CTRLA_SDAHOLD_450NS       (SERCOM_I2CS_CTRLA_SDAHOLD_450NS_Val << SERCOM_I2CS_CTRLA_SDAHOLD_Pos) /**< (SERCOM_I2CS_CTRLA) 300-600ns hold time Position  */
#define SERCOM_I2CS_CTRLA_SDAHOLD_600NS       (SERCOM_I2CS_CTRLA_SDAHOLD_600NS_Val << SERCOM_I2CS_CTRLA_SDAHOLD_Pos) /**< (SERCOM_I2CS_CTRLA) 400-800ns hold time Position  */
#define SERCOM_I2CS_CTRLA_SEXTTOEN_Pos        _U_(23)                                              /**< (SERCOM_I2CS_CTRLA) Slave SCL Low Extend Timeout Position */
#define SERCOM_I2CS_CTRLA_SEXTTOEN_Msk        (_U_(0x1) << SERCOM_I2CS_CTRLA_SEXTTOEN_Pos)         /**< (SERCOM_I2CS_CTRLA) Slave SCL Low Extend Timeout Mask */
#define SERCOM_I2CS_CTRLA_SEXTTOEN(value)     (SERCOM_I2CS_CTRLA_SEXTTOEN_Msk & ((value) << SERCOM_I2CS_CTRLA_SEXTTOEN_Pos))
#define SERCOM_I2CS_CTRLA_SPEED_Pos           _U_(24)                                              /**< (SERCOM_I2CS_CTRLA) Transfer Speed Position */
#define SERCOM_I2CS_CTRLA_SPEED_Msk           (_U_(0x3) << SERCOM_I2CS_CTRLA_SPEED_Pos)            /**< (SERCOM_I2CS_CTRLA) Transfer Speed Mask */
#define SERCOM_I2CS_CTRLA_SPEED(value)        (SERCOM_I2CS_CTRLA_SPEED_Msk & ((value) << SERCOM_I2CS_CTRLA_SPEED_Pos))
#define   SERCOM_I2CS_CTRLA_SPEED_STANDARD_AND_FAST_MODE_Val _U_(0x0)                                             /**< (SERCOM_I2CS_CTRLA) Standard Mode(Sm) Upto 100kHz and Fast Mode(Fm) Upto 400kHz   */
#define   SERCOM_I2CS_CTRLA_SPEED_FASTPLUS_MODE_Val _U_(0x1)                                             /**< (SERCOM_I2CS_CTRLA) Fast-mode Plus Upto 1MHz  */
#define   SERCOM_I2CS_CTRLA_SPEED_HIGH_SPEED_MODE_Val _U_(0x2)                                             /**< (SERCOM_I2CS_CTRLA) High-speed mode Upto 3.4MHz  */
#define SERCOM_I2CS_CTRLA_SPEED_STANDARD_AND_FAST_MODE (SERCOM_I2CS_CTRLA_SPEED_STANDARD_AND_FAST_MODE_Val << SERCOM_I2CS_CTRLA_SPEED_Pos) /**< (SERCOM_I2CS_CTRLA) Standard Mode(Sm) Upto 100kHz and Fast Mode(Fm) Upto 400kHz  Position  */
#define SERCOM_I2CS_CTRLA_SPEED_FASTPLUS_MODE (SERCOM_I2CS_CTRLA_SPEED_FASTPLUS_MODE_Val << SERCOM_I2CS_CTRLA_SPEED_Pos) /**< (SERCOM_I2CS_CTRLA) Fast-mode Plus Upto 1MHz Position  */
#define SERCOM_I2CS_CTRLA_SPEED_HIGH_SPEED_MODE (SERCOM_I2CS_CTRLA_SPEED_HIGH_SPEED_MODE_Val << SERCOM_I2CS_CTRLA_SPEED_Pos) /**< (SERCOM_I2CS_CTRLA) High-speed mode Upto 3.4MHz Position  */
#define SERCOM_I2CS_CTRLA_SCLSM_Pos           _U_(27)                                              /**< (SERCOM_I2CS_CTRLA) SCL Clock Stretch Mode Position */
#define SERCOM_I2CS_CTRLA_SCLSM_Msk           (_U_(0x1) << SERCOM_I2CS_CTRLA_SCLSM_Pos)            /**< (SERCOM_I2CS_CTRLA) SCL Clock Stretch Mode Mask */
#define SERCOM_I2CS_CTRLA_SCLSM(value)        (SERCOM_I2CS_CTRLA_SCLSM_Msk & ((value) << SERCOM_I2CS_CTRLA_SCLSM_Pos))
#define SERCOM_I2CS_CTRLA_LOWTOUTEN_Pos       _U_(30)                                              /**< (SERCOM_I2CS_CTRLA) SCL Low Timeout Enable Position */
#define SERCOM_I2CS_CTRLA_LOWTOUTEN_Msk       (_U_(0x1) << SERCOM_I2CS_CTRLA_LOWTOUTEN_Pos)        /**< (SERCOM_I2CS_CTRLA) SCL Low Timeout Enable Mask */
#define SERCOM_I2CS_CTRLA_LOWTOUTEN(value)    (SERCOM_I2CS_CTRLA_LOWTOUTEN_Msk & ((value) << SERCOM_I2CS_CTRLA_LOWTOUTEN_Pos))
#define SERCOM_I2CS_CTRLA_Msk                 _U_(0x4BB1009F)                                      /**< (SERCOM_I2CS_CTRLA) Register Mask  */


/* -------- SERCOM_SPIM_CTRLA : (SERCOM Offset: 0x00) (R/W 32) SPIM Control A -------- */
#define SERCOM_SPIM_CTRLA_RESETVALUE          _U_(0x00)                                            /**<  (SERCOM_SPIM_CTRLA) SPIM Control A  Reset Value */

#define SERCOM_SPIM_CTRLA_SWRST_Pos           _U_(0)                                               /**< (SERCOM_SPIM_CTRLA) Software Reset Position */
#define SERCOM_SPIM_CTRLA_SWRST_Msk           (_U_(0x1) << SERCOM_SPIM_CTRLA_SWRST_Pos)            /**< (SERCOM_SPIM_CTRLA) Software Reset Mask */
#define SERCOM_SPIM_CTRLA_SWRST(value)        (SERCOM_SPIM_CTRLA_SWRST_Msk & ((value) << SERCOM_SPIM_CTRLA_SWRST_Pos))
#define SERCOM_SPIM_CTRLA_ENABLE_Pos          _U_(1)                                               /**< (SERCOM_SPIM_CTRLA) Enable Position */
#define SERCOM_SPIM_CTRLA_ENABLE_Msk          (_U_(0x1) << SERCOM_SPIM_CTRLA_ENABLE_Pos)           /**< (SERCOM_SPIM_CTRLA) Enable Mask */
#define SERCOM_SPIM_CTRLA_ENABLE(value)       (SERCOM_SPIM_CTRLA_ENABLE_Msk & ((value) << SERCOM_SPIM_CTRLA_ENABLE_Pos))
#define SERCOM_SPIM_CTRLA_MODE_Pos            _U_(2)                                               /**< (SERCOM_SPIM_CTRLA) Operating Mode Position */
#define SERCOM_SPIM_CTRLA_MODE_Msk            (_U_(0x7) << SERCOM_SPIM_CTRLA_MODE_Pos)             /**< (SERCOM_SPIM_CTRLA) Operating Mode Mask */
#define SERCOM_SPIM_CTRLA_MODE(value)         (SERCOM_SPIM_CTRLA_MODE_Msk & ((value) << SERCOM_SPIM_CTRLA_MODE_Pos))
#define   SERCOM_SPIM_CTRLA_MODE_USART_EXT_CLK_Val _U_(0x0)                                             /**< (SERCOM_SPIM_CTRLA) USART with external clock  */
#define   SERCOM_SPIM_CTRLA_MODE_USART_INT_CLK_Val _U_(0x1)                                             /**< (SERCOM_SPIM_CTRLA) USART with internal clock  */
#define   SERCOM_SPIM_CTRLA_MODE_SPI_SLAVE_Val _U_(0x2)                                             /**< (SERCOM_SPIM_CTRLA) SPI in slave operation  */
#define   SERCOM_SPIM_CTRLA_MODE_SPI_MASTER_Val _U_(0x3)                                             /**< (SERCOM_SPIM_CTRLA) SPI in master operation  */
#define   SERCOM_SPIM_CTRLA_MODE_I2C_SLAVE_Val _U_(0x4)                                             /**< (SERCOM_SPIM_CTRLA) I2C slave operation  */
#define   SERCOM_SPIM_CTRLA_MODE_I2C_MASTER_Val _U_(0x5)                                             /**< (SERCOM_SPIM_CTRLA) I2C master operation  */
#define SERCOM_SPIM_CTRLA_MODE_USART_EXT_CLK  (SERCOM_SPIM_CTRLA_MODE_USART_EXT_CLK_Val << SERCOM_SPIM_CTRLA_MODE_Pos) /**< (SERCOM_SPIM_CTRLA) USART with external clock Position  */
#define SERCOM_SPIM_CTRLA_MODE_USART_INT_CLK  (SERCOM_SPIM_CTRLA_MODE_USART_INT_CLK_Val << SERCOM_SPIM_CTRLA_MODE_Pos) /**< (SERCOM_SPIM_CTRLA) USART with internal clock Position  */
#define SERCOM_SPIM_CTRLA_MODE_SPI_SLAVE      (SERCOM_SPIM_CTRLA_MODE_SPI_SLAVE_Val << SERCOM_SPIM_CTRLA_MODE_Pos) /**< (SERCOM_SPIM_CTRLA) SPI in slave operation Position  */
#define SERCOM_SPIM_CTRLA_MODE_SPI_MASTER     (SERCOM_SPIM_CTRLA_MODE_SPI_MASTER_Val << SERCOM_SPIM_CTRLA_MODE_Pos) /**< (SERCOM_SPIM_CTRLA) SPI in master operation Position  */
#define SERCOM_SPIM_CTRLA_MODE_I2C_SLAVE      (SERCOM_SPIM_CTRLA_MODE_I2C_SLAVE_Val << SERCOM_SPIM_CTRLA_MODE_Pos) /**< (SERCOM_SPIM_CTRLA) I2C slave operation Position  */
#define SERCOM_SPIM_CTRLA_MODE_I2C_MASTER     (SERCOM_SPIM_CTRLA_MODE_I2C_MASTER_Val << SERCOM_SPIM_CTRLA_MODE_Pos) /**< (SERCOM_SPIM_CTRLA) I2C master operation Position  */
#define SERCOM_SPIM_CTRLA_RUNSTDBY_Pos        _U_(7)                                               /**< (SERCOM_SPIM_CTRLA) Run during Standby Position */
#define SERCOM_SPIM_CTRLA_RUNSTDBY_Msk        (_U_(0x1) << SERCOM_SPIM_CTRLA_RUNSTDBY_Pos)         /**< (SERCOM_SPIM_CTRLA) Run during Standby Mask */
#define SERCOM_SPIM_CTRLA_RUNSTDBY(value)     (SERCOM_SPIM_CTRLA_RUNSTDBY_Msk & ((value) << SERCOM_SPIM_CTRLA_RUNSTDBY_Pos))
#define SERCOM_SPIM_CTRLA_IBON_Pos            _U_(8)                                               /**< (SERCOM_SPIM_CTRLA) Immediate Buffer Overflow Notification Position */
#define SERCOM_SPIM_CTRLA_IBON_Msk            (_U_(0x1) << SERCOM_SPIM_CTRLA_IBON_Pos)             /**< (SERCOM_SPIM_CTRLA) Immediate Buffer Overflow Notification Mask */
#define SERCOM_SPIM_CTRLA_IBON(value)         (SERCOM_SPIM_CTRLA_IBON_Msk & ((value) << SERCOM_SPIM_CTRLA_IBON_Pos))
#define SERCOM_SPIM_CTRLA_DOPO_Pos            _U_(16)                                              /**< (SERCOM_SPIM_CTRLA) Data Out Pinout Position */
#define SERCOM_SPIM_CTRLA_DOPO_Msk            (_U_(0x3) << SERCOM_SPIM_CTRLA_DOPO_Pos)             /**< (SERCOM_SPIM_CTRLA) Data Out Pinout Mask */
#define SERCOM_SPIM_CTRLA_DOPO(value)         (SERCOM_SPIM_CTRLA_DOPO_Msk & ((value) << SERCOM_SPIM_CTRLA_DOPO_Pos))
#define   SERCOM_SPIM_CTRLA_DOPO_PAD0_Val     _U_(0x0)                                             /**< (SERCOM_SPIM_CTRLA) DO on PAD[0], SCK on PAD[1] and SS on PAD[2]  */
#define   SERCOM_SPIM_CTRLA_DOPO_PAD2_Val     _U_(0x2)                                             /**< (SERCOM_SPIM_CTRLA) DO on PAD[3], SCK on PAD[1] and SS on PAD[2]  */
#define SERCOM_SPIM_CTRLA_DOPO_PAD0           (SERCOM_SPIM_CTRLA_DOPO_PAD0_Val << SERCOM_SPIM_CTRLA_DOPO_Pos) /**< (SERCOM_SPIM_CTRLA) DO on PAD[0], SCK on PAD[1] and SS on PAD[2] Position  */
#define SERCOM_SPIM_CTRLA_DOPO_PAD2           (SERCOM_SPIM_CTRLA_DOPO_PAD2_Val << SERCOM_SPIM_CTRLA_DOPO_Pos) /**< (SERCOM_SPIM_CTRLA) DO on PAD[3], SCK on PAD[1] and SS on PAD[2] Position  */
#define SERCOM_SPIM_CTRLA_DIPO_Pos            _U_(20)                                              /**< (SERCOM_SPIM_CTRLA) Data In Pinout Position */
#define SERCOM_SPIM_CTRLA_DIPO_Msk            (_U_(0x3) << SERCOM_SPIM_CTRLA_DIPO_Pos)             /**< (SERCOM_SPIM_CTRLA) Data In Pinout Mask */
#define SERCOM_SPIM_CTRLA_DIPO(value)         (SERCOM_SPIM_CTRLA_DIPO_Msk & ((value) << SERCOM_SPIM_CTRLA_DIPO_Pos))
#define   SERCOM_SPIM_CTRLA_DIPO_PAD0_Val     _U_(0x0)                                             /**< (SERCOM_SPIM_CTRLA) SERCOM PAD[0] is used as data input  */
#define   SERCOM_SPIM_CTRLA_DIPO_PAD1_Val     _U_(0x1)                                             /**< (SERCOM_SPIM_CTRLA) SERCOM PAD[1] is used as data input  */
#define   SERCOM_SPIM_CTRLA_DIPO_PAD2_Val     _U_(0x2)                                             /**< (SERCOM_SPIM_CTRLA) SERCOM PAD[2] is used as data input  */
#define   SERCOM_SPIM_CTRLA_DIPO_PAD3_Val     _U_(0x3)                                             /**< (SERCOM_SPIM_CTRLA) SERCOM PAD[3] is used as data input  */
#define SERCOM_SPIM_CTRLA_DIPO_PAD0           (SERCOM_SPIM_CTRLA_DIPO_PAD0_Val << SERCOM_SPIM_CTRLA_DIPO_Pos) /**< (SERCOM_SPIM_CTRLA) SERCOM PAD[0] is used as data input Position  */
#define SERCOM_SPIM_CTRLA_DIPO_PAD1           (SERCOM_SPIM_CTRLA_DIPO_PAD1_Val << SERCOM_SPIM_CTRLA_DIPO_Pos) /**< (SERCOM_SPIM_CTRLA) SERCOM PAD[1] is used as data input Position  */
#define SERCOM_SPIM_CTRLA_DIPO_PAD2           (SERCOM_SPIM_CTRLA_DIPO_PAD2_Val << SERCOM_SPIM_CTRLA_DIPO_Pos) /**< (SERCOM_SPIM_CTRLA) SERCOM PAD[2] is used as data input Position  */
#define SERCOM_SPIM_CTRLA_DIPO_PAD3           (SERCOM_SPIM_CTRLA_DIPO_PAD3_Val << SERCOM_SPIM_CTRLA_DIPO_Pos) /**< (SERCOM_SPIM_CTRLA) SERCOM PAD[3] is used as data input Position  */
#define SERCOM_SPIM_CTRLA_FORM_Pos            _U_(24)                                              /**< (SERCOM_SPIM_CTRLA) Frame Format Position */
#define SERCOM_SPIM_CTRLA_FORM_Msk            (_U_(0xF) << SERCOM_SPIM_CTRLA_FORM_Pos)             /**< (SERCOM_SPIM_CTRLA) Frame Format Mask */
#define SERCOM_SPIM_CTRLA_FORM(value)         (SERCOM_SPIM_CTRLA_FORM_Msk & ((value) << SERCOM_SPIM_CTRLA_FORM_Pos))
#define   SERCOM_SPIM_CTRLA_FORM_SPI_FRAME_Val _U_(0x0)                                             /**< (SERCOM_SPIM_CTRLA) SPI Frame  */
#define   SERCOM_SPIM_CTRLA_FORM_SPI_FRAME_WITH_ADDR_Val _U_(0x2)                                             /**< (SERCOM_SPIM_CTRLA) SPI Frame with Addr  */
#define SERCOM_SPIM_CTRLA_FORM_SPI_FRAME      (SERCOM_SPIM_CTRLA_FORM_SPI_FRAME_Val << SERCOM_SPIM_CTRLA_FORM_Pos) /**< (SERCOM_SPIM_CTRLA) SPI Frame Position  */
#define SERCOM_SPIM_CTRLA_FORM_SPI_FRAME_WITH_ADDR (SERCOM_SPIM_CTRLA_FORM_SPI_FRAME_WITH_ADDR_Val << SERCOM_SPIM_CTRLA_FORM_Pos) /**< (SERCOM_SPIM_CTRLA) SPI Frame with Addr Position  */
#define SERCOM_SPIM_CTRLA_CPHA_Pos            _U_(28)                                              /**< (SERCOM_SPIM_CTRLA) Clock Phase Position */
#define SERCOM_SPIM_CTRLA_CPHA_Msk            (_U_(0x1) << SERCOM_SPIM_CTRLA_CPHA_Pos)             /**< (SERCOM_SPIM_CTRLA) Clock Phase Mask */
#define SERCOM_SPIM_CTRLA_CPHA(value)         (SERCOM_SPIM_CTRLA_CPHA_Msk & ((value) << SERCOM_SPIM_CTRLA_CPHA_Pos))
#define   SERCOM_SPIM_CTRLA_CPHA_LEADING_EDGE_Val _U_(0x0)                                             /**< (SERCOM_SPIM_CTRLA) The data is sampled on a leading SCK edge and changed on a trailing SCK edge  */
#define   SERCOM_SPIM_CTRLA_CPHA_TRAILING_EDGE_Val _U_(0x1)                                             /**< (SERCOM_SPIM_CTRLA) The data is sampled on a trailing SCK edge and changed on a leading SCK edge  */
#define SERCOM_SPIM_CTRLA_CPHA_LEADING_EDGE   (SERCOM_SPIM_CTRLA_CPHA_LEADING_EDGE_Val << SERCOM_SPIM_CTRLA_CPHA_Pos) /**< (SERCOM_SPIM_CTRLA) The data is sampled on a leading SCK edge and changed on a trailing SCK edge Position  */
#define SERCOM_SPIM_CTRLA_CPHA_TRAILING_EDGE  (SERCOM_SPIM_CTRLA_CPHA_TRAILING_EDGE_Val << SERCOM_SPIM_CTRLA_CPHA_Pos) /**< (SERCOM_SPIM_CTRLA) The data is sampled on a trailing SCK edge and changed on a leading SCK edge Position  */
#define SERCOM_SPIM_CTRLA_CPOL_Pos            _U_(29)                                              /**< (SERCOM_SPIM_CTRLA) Clock Polarity Position */
#define SERCOM_SPIM_CTRLA_CPOL_Msk            (_U_(0x1) << SERCOM_SPIM_CTRLA_CPOL_Pos)             /**< (SERCOM_SPIM_CTRLA) Clock Polarity Mask */
#define SERCOM_SPIM_CTRLA_CPOL(value)         (SERCOM_SPIM_CTRLA_CPOL_Msk & ((value) << SERCOM_SPIM_CTRLA_CPOL_Pos))
#define   SERCOM_SPIM_CTRLA_CPOL_IDLE_LOW_Val _U_(0x0)                                             /**< (SERCOM_SPIM_CTRLA) SCK is low when idle  */
#define   SERCOM_SPIM_CTRLA_CPOL_IDLE_HIGH_Val _U_(0x1)                                             /**< (SERCOM_SPIM_CTRLA) SCK is high when idle  */
#define SERCOM_SPIM_CTRLA_CPOL_IDLE_LOW       (SERCOM_SPIM_CTRLA_CPOL_IDLE_LOW_Val << SERCOM_SPIM_CTRLA_CPOL_Pos) /**< (SERCOM_SPIM_CTRLA) SCK is low when idle Position  */
#define SERCOM_SPIM_CTRLA_CPOL_IDLE_HIGH      (SERCOM_SPIM_CTRLA_CPOL_IDLE_HIGH_Val << SERCOM_SPIM_CTRLA_CPOL_Pos) /**< (SERCOM_SPIM_CTRLA) SCK is high when idle Position  */
#define SERCOM_SPIM_CTRLA_DORD_Pos            _U_(30)                                              /**< (SERCOM_SPIM_CTRLA) Data Order Position */
#define SERCOM_SPIM_CTRLA_DORD_Msk            (_U_(0x1) << SERCOM_SPIM_CTRLA_DORD_Pos)             /**< (SERCOM_SPIM_CTRLA) Data Order Mask */
#define SERCOM_SPIM_CTRLA_DORD(value)         (SERCOM_SPIM_CTRLA_DORD_Msk & ((value) << SERCOM_SPIM_CTRLA_DORD_Pos))
#define   SERCOM_SPIM_CTRLA_DORD_MSB_Val      _U_(0x0)                                             /**< (SERCOM_SPIM_CTRLA) MSB is transferred first  */
#define   SERCOM_SPIM_CTRLA_DORD_LSB_Val      _U_(0x1)                                             /**< (SERCOM_SPIM_CTRLA) LSB is transferred first  */
#define SERCOM_SPIM_CTRLA_DORD_MSB            (SERCOM_SPIM_CTRLA_DORD_MSB_Val << SERCOM_SPIM_CTRLA_DORD_Pos) /**< (SERCOM_SPIM_CTRLA) MSB is transferred first Position  */
#define SERCOM_SPIM_CTRLA_DORD_LSB            (SERCOM_SPIM_CTRLA_DORD_LSB_Val << SERCOM_SPIM_CTRLA_DORD_Pos) /**< (SERCOM_SPIM_CTRLA) LSB is transferred first Position  */
#define SERCOM_SPIM_CTRLA_Msk                 _U_(0x7F33019F)                                      /**< (SERCOM_SPIM_CTRLA) Register Mask  */


/* -------- SERCOM_SPIS_CTRLA : (SERCOM Offset: 0x00) (R/W 32) SPIS Control A -------- */
#define SERCOM_SPIS_CTRLA_RESETVALUE          _U_(0x00)                                            /**<  (SERCOM_SPIS_CTRLA) SPIS Control A  Reset Value */

#define SERCOM_SPIS_CTRLA_SWRST_Pos           _U_(0)                                               /**< (SERCOM_SPIS_CTRLA) Software Reset Position */
#define SERCOM_SPIS_CTRLA_SWRST_Msk           (_U_(0x1) << SERCOM_SPIS_CTRLA_SWRST_Pos)            /**< (SERCOM_SPIS_CTRLA) Software Reset Mask */
#define SERCOM_SPIS_CTRLA_SWRST(value)        (SERCOM_SPIS_CTRLA_SWRST_Msk & ((value) << SERCOM_SPIS_CTRLA_SWRST_Pos))
#define SERCOM_SPIS_CTRLA_ENABLE_Pos          _U_(1)                                               /**< (SERCOM_SPIS_CTRLA) Enable Position */
#define SERCOM_SPIS_CTRLA_ENABLE_Msk          (_U_(0x1) << SERCOM_SPIS_CTRLA_ENABLE_Pos)           /**< (SERCOM_SPIS_CTRLA) Enable Mask */
#define SERCOM_SPIS_CTRLA_ENABLE(value)       (SERCOM_SPIS_CTRLA_ENABLE_Msk & ((value) << SERCOM_SPIS_CTRLA_ENABLE_Pos))
#define SERCOM_SPIS_CTRLA_MODE_Pos            _U_(2)                                               /**< (SERCOM_SPIS_CTRLA) Operating Mode Position */
#define SERCOM_SPIS_CTRLA_MODE_Msk            (_U_(0x7) << SERCOM_SPIS_CTRLA_MODE_Pos)             /**< (SERCOM_SPIS_CTRLA) Operating Mode Mask */
#define SERCOM_SPIS_CTRLA_MODE(value)         (SERCOM_SPIS_CTRLA_MODE_Msk & ((value) << SERCOM_SPIS_CTRLA_MODE_Pos))
#define   SERCOM_SPIS_CTRLA_MODE_USART_EXT_CLK_Val _U_(0x0)                                             /**< (SERCOM_SPIS_CTRLA) USART with external clock  */
#define   SERCOM_SPIS_CTRLA_MODE_USART_INT_CLK_Val _U_(0x1)                                             /**< (SERCOM_SPIS_CTRLA) USART with internal clock  */
#define   SERCOM_SPIS_CTRLA_MODE_SPI_SLAVE_Val _U_(0x2)                                             /**< (SERCOM_SPIS_CTRLA) SPI in slave operation  */
#define   SERCOM_SPIS_CTRLA_MODE_SPI_MASTER_Val _U_(0x3)                                             /**< (SERCOM_SPIS_CTRLA) SPI in master operation  */
#define   SERCOM_SPIS_CTRLA_MODE_I2C_SLAVE_Val _U_(0x4)                                             /**< (SERCOM_SPIS_CTRLA) I2C slave operation  */
#define   SERCOM_SPIS_CTRLA_MODE_I2C_MASTER_Val _U_(0x5)                                             /**< (SERCOM_SPIS_CTRLA) I2C master operation  */
#define SERCOM_SPIS_CTRLA_MODE_USART_EXT_CLK  (SERCOM_SPIS_CTRLA_MODE_USART_EXT_CLK_Val << SERCOM_SPIS_CTRLA_MODE_Pos) /**< (SERCOM_SPIS_CTRLA) USART with external clock Position  */
#define SERCOM_SPIS_CTRLA_MODE_USART_INT_CLK  (SERCOM_SPIS_CTRLA_MODE_USART_INT_CLK_Val << SERCOM_SPIS_CTRLA_MODE_Pos) /**< (SERCOM_SPIS_CTRLA) USART with internal clock Position  */
#define SERCOM_SPIS_CTRLA_MODE_SPI_SLAVE      (SERCOM_SPIS_CTRLA_MODE_SPI_SLAVE_Val << SERCOM_SPIS_CTRLA_MODE_Pos) /**< (SERCOM_SPIS_CTRLA) SPI in slave operation Position  */
#define SERCOM_SPIS_CTRLA_MODE_SPI_MASTER     (SERCOM_SPIS_CTRLA_MODE_SPI_MASTER_Val << SERCOM_SPIS_CTRLA_MODE_Pos) /**< (SERCOM_SPIS_CTRLA) SPI in master operation Position  */
#define SERCOM_SPIS_CTRLA_MODE_I2C_SLAVE      (SERCOM_SPIS_CTRLA_MODE_I2C_SLAVE_Val << SERCOM_SPIS_CTRLA_MODE_Pos) /**< (SERCOM_SPIS_CTRLA) I2C slave operation Position  */
#define SERCOM_SPIS_CTRLA_MODE_I2C_MASTER     (SERCOM_SPIS_CTRLA_MODE_I2C_MASTER_Val << SERCOM_SPIS_CTRLA_MODE_Pos) /**< (SERCOM_SPIS_CTRLA) I2C master operation Position  */
#define SERCOM_SPIS_CTRLA_RUNSTDBY_Pos        _U_(7)                                               /**< (SERCOM_SPIS_CTRLA) Run during Standby Position */
#define SERCOM_SPIS_CTRLA_RUNSTDBY_Msk        (_U_(0x1) << SERCOM_SPIS_CTRLA_RUNSTDBY_Pos)         /**< (SERCOM_SPIS_CTRLA) Run during Standby Mask */
#define SERCOM_SPIS_CTRLA_RUNSTDBY(value)     (SERCOM_SPIS_CTRLA_RUNSTDBY_Msk & ((value) << SERCOM_SPIS_CTRLA_RUNSTDBY_Pos))
#define SERCOM_SPIS_CTRLA_IBON_Pos            _U_(8)                                               /**< (SERCOM_SPIS_CTRLA) Immediate Buffer Overflow Notification Position */
#define SERCOM_SPIS_CTRLA_IBON_Msk            (_U_(0x1) << SERCOM_SPIS_CTRLA_IBON_Pos)             /**< (SERCOM_SPIS_CTRLA) Immediate Buffer Overflow Notification Mask */
#define SERCOM_SPIS_CTRLA_IBON(value)         (SERCOM_SPIS_CTRLA_IBON_Msk & ((value) << SERCOM_SPIS_CTRLA_IBON_Pos))
#define SERCOM_SPIS_CTRLA_DOPO_Pos            _U_(16)                                              /**< (SERCOM_SPIS_CTRLA) Data Out Pinout Position */
#define SERCOM_SPIS_CTRLA_DOPO_Msk            (_U_(0x3) << SERCOM_SPIS_CTRLA_DOPO_Pos)             /**< (SERCOM_SPIS_CTRLA) Data Out Pinout Mask */
#define SERCOM_SPIS_CTRLA_DOPO(value)         (SERCOM_SPIS_CTRLA_DOPO_Msk & ((value) << SERCOM_SPIS_CTRLA_DOPO_Pos))
#define   SERCOM_SPIS_CTRLA_DOPO_PAD0_Val     _U_(0x0)                                             /**< (SERCOM_SPIS_CTRLA) DO on PAD[0], SCK on PAD[1] and SS on PAD[2]  */
#define   SERCOM_SPIS_CTRLA_DOPO_PAD2_Val     _U_(0x2)                                             /**< (SERCOM_SPIS_CTRLA) DO on PAD[3], SCK on PAD[1] and SS on PAD[2]  */
#define SERCOM_SPIS_CTRLA_DOPO_PAD0           (SERCOM_SPIS_CTRLA_DOPO_PAD0_Val << SERCOM_SPIS_CTRLA_DOPO_Pos) /**< (SERCOM_SPIS_CTRLA) DO on PAD[0], SCK on PAD[1] and SS on PAD[2] Position  */
#define SERCOM_SPIS_CTRLA_DOPO_PAD2           (SERCOM_SPIS_CTRLA_DOPO_PAD2_Val << SERCOM_SPIS_CTRLA_DOPO_Pos) /**< (SERCOM_SPIS_CTRLA) DO on PAD[3], SCK on PAD[1] and SS on PAD[2] Position  */
#define SERCOM_SPIS_CTRLA_DIPO_Pos            _U_(20)                                              /**< (SERCOM_SPIS_CTRLA) Data In Pinout Position */
#define SERCOM_SPIS_CTRLA_DIPO_Msk            (_U_(0x3) << SERCOM_SPIS_CTRLA_DIPO_Pos)             /**< (SERCOM_SPIS_CTRLA) Data In Pinout Mask */
#define SERCOM_SPIS_CTRLA_DIPO(value)         (SERCOM_SPIS_CTRLA_DIPO_Msk & ((value) << SERCOM_SPIS_CTRLA_DIPO_Pos))
#define   SERCOM_SPIS_CTRLA_DIPO_PAD0_Val     _U_(0x0)                                             /**< (SERCOM_SPIS_CTRLA) SERCOM PAD[0] is used as data input  */
#define   SERCOM_SPIS_CTRLA_DIPO_PAD1_Val     _U_(0x1)                                             /**< (SERCOM_SPIS_CTRLA) SERCOM PAD[1] is used as data input  */
#define   SERCOM_SPIS_CTRLA_DIPO_PAD2_Val     _U_(0x2)                                             /**< (SERCOM_SPIS_CTRLA) SERCOM PAD[2] is used as data input  */
#define   SERCOM_SPIS_CTRLA_DIPO_PAD3_Val     _U_(0x3)                                             /**< (SERCOM_SPIS_CTRLA) SERCOM PAD[3] is used as data input  */
#define SERCOM_SPIS_CTRLA_DIPO_PAD0           (SERCOM_SPIS_CTRLA_DIPO_PAD0_Val << SERCOM_SPIS_CTRLA_DIPO_Pos) /**< (SERCOM_SPIS_CTRLA) SERCOM PAD[0] is used as data input Position  */
#define SERCOM_SPIS_CTRLA_DIPO_PAD1           (SERCOM_SPIS_CTRLA_DIPO_PAD1_Val << SERCOM_SPIS_CTRLA_DIPO_Pos) /**< (SERCOM_SPIS_CTRLA) SERCOM PAD[1] is used as data input Position  */
#define SERCOM_SPIS_CTRLA_DIPO_PAD2           (SERCOM_SPIS_CTRLA_DIPO_PAD2_Val << SERCOM_SPIS_CTRLA_DIPO_Pos) /**< (SERCOM_SPIS_CTRLA) SERCOM PAD[2] is used as data input Position  */
#define SERCOM_SPIS_CTRLA_DIPO_PAD3           (SERCOM_SPIS_CTRLA_DIPO_PAD3_Val << SERCOM_SPIS_CTRLA_DIPO_Pos) /**< (SERCOM_SPIS_CTRLA) SERCOM PAD[3] is used as data input Position  */
#define SERCOM_SPIS_CTRLA_FORM_Pos            _U_(24)                                              /**< (SERCOM_SPIS_CTRLA) Frame Format Position */
#define SERCOM_SPIS_CTRLA_FORM_Msk            (_U_(0xF) << SERCOM_SPIS_CTRLA_FORM_Pos)             /**< (SERCOM_SPIS_CTRLA) Frame Format Mask */
#define SERCOM_SPIS_CTRLA_FORM(value)         (SERCOM_SPIS_CTRLA_FORM_Msk & ((value) << SERCOM_SPIS_CTRLA_FORM_Pos))
#define   SERCOM_SPIS_CTRLA_FORM_SPI_FRAME_Val _U_(0x0)                                             /**< (SERCOM_SPIS_CTRLA) SPI Frame  */
#define   SERCOM_SPIS_CTRLA_FORM_SPI_FRAME_WITH_ADDR_Val _U_(0x2)                                             /**< (SERCOM_SPIS_CTRLA) SPI Frame with Addr  */
#define SERCOM_SPIS_CTRLA_FORM_SPI_FRAME      (SERCOM_SPIS_CTRLA_FORM_SPI_FRAME_Val << SERCOM_SPIS_CTRLA_FORM_Pos) /**< (SERCOM_SPIS_CTRLA) SPI Frame Position  */
#define SERCOM_SPIS_CTRLA_FORM_SPI_FRAME_WITH_ADDR (SERCOM_SPIS_CTRLA_FORM_SPI_FRAME_WITH_ADDR_Val << SERCOM_SPIS_CTRLA_FORM_Pos) /**< (SERCOM_SPIS_CTRLA) SPI Frame with Addr Position  */
#define SERCOM_SPIS_CTRLA_CPHA_Pos            _U_(28)                                              /**< (SERCOM_SPIS_CTRLA) Clock Phase Position */
#define SERCOM_SPIS_CTRLA_CPHA_Msk            (_U_(0x1) << SERCOM_SPIS_CTRLA_CPHA_Pos)             /**< (SERCOM_SPIS_CTRLA) Clock Phase Mask */
#define SERCOM_SPIS_CTRLA_CPHA(value)         (SERCOM_SPIS_CTRLA_CPHA_Msk & ((value) << SERCOM_SPIS_CTRLA_CPHA_Pos))
#define   SERCOM_SPIS_CTRLA_CPHA_LEADING_EDGE_Val _U_(0x0)                                             /**< (SERCOM_SPIS_CTRLA) The data is sampled on a leading SCK edge and changed on a trailing SCK edge  */
#define   SERCOM_SPIS_CTRLA_CPHA_TRAILING_EDGE_Val _U_(0x1)                                             /**< (SERCOM_SPIS_CTRLA) The data is sampled on a trailing SCK edge and changed on a leading SCK edge  */
#define SERCOM_SPIS_CTRLA_CPHA_LEADING_EDGE   (SERCOM_SPIS_CTRLA_CPHA_LEADING_EDGE_Val << SERCOM_SPIS_CTRLA_CPHA_Pos) /**< (SERCOM_SPIS_CTRLA) The data is sampled on a leading SCK edge and changed on a trailing SCK edge Position  */
#define SERCOM_SPIS_CTRLA_CPHA_TRAILING_EDGE  (SERCOM_SPIS_CTRLA_CPHA_TRAILING_EDGE_Val << SERCOM_SPIS_CTRLA_CPHA_Pos) /**< (SERCOM_SPIS_CTRLA) The data is sampled on a trailing SCK edge and changed on a leading SCK edge Position  */
#define SERCOM_SPIS_CTRLA_CPOL_Pos            _U_(29)                                              /**< (SERCOM_SPIS_CTRLA) Clock Polarity Position */
#define SERCOM_SPIS_CTRLA_CPOL_Msk            (_U_(0x1) << SERCOM_SPIS_CTRLA_CPOL_Pos)             /**< (SERCOM_SPIS_CTRLA) Clock Polarity Mask */
#define SERCOM_SPIS_CTRLA_CPOL(value)         (SERCOM_SPIS_CTRLA_CPOL_Msk & ((value) << SERCOM_SPIS_CTRLA_CPOL_Pos))
#define   SERCOM_SPIS_CTRLA_CPOL_IDLE_LOW_Val _U_(0x0)                                             /**< (SERCOM_SPIS_CTRLA) SCK is low when idle  */
#define   SERCOM_SPIS_CTRLA_CPOL_IDLE_HIGH_Val _U_(0x1)                                             /**< (SERCOM_SPIS_CTRLA) SCK is high when idle  */
#define SERCOM_SPIS_CTRLA_CPOL_IDLE_LOW       (SERCOM_SPIS_CTRLA_CPOL_IDLE_LOW_Val << SERCOM_SPIS_CTRLA_CPOL_Pos) /**< (SERCOM_SPIS_CTRLA) SCK is low when idle Position  */
#define SERCOM_SPIS_CTRLA_CPOL_IDLE_HIGH      (SERCOM_SPIS_CTRLA_CPOL_IDLE_HIGH_Val << SERCOM_SPIS_CTRLA_CPOL_Pos) /**< (SERCOM_SPIS_CTRLA) SCK is high when idle Position  */
#define SERCOM_SPIS_CTRLA_DORD_Pos            _U_(30)                                              /**< (SERCOM_SPIS_CTRLA) Data Order Position */
#define SERCOM_SPIS_CTRLA_DORD_Msk            (_U_(0x1) << SERCOM_SPIS_CTRLA_DORD_Pos)             /**< (SERCOM_SPIS_CTRLA) Data Order Mask */
#define SERCOM_SPIS_CTRLA_DORD(value)         (SERCOM_SPIS_CTRLA_DORD_Msk & ((value) << SERCOM_SPIS_CTRLA_DORD_Pos))
#define   SERCOM_SPIS_CTRLA_DORD_MSB_Val      _U_(0x0)                                             /**< (SERCOM_SPIS_CTRLA) MSB is transferred first  */
#define   SERCOM_SPIS_CTRLA_DORD_LSB_Val      _U_(0x1)                                             /**< (SERCOM_SPIS_CTRLA) LSB is transferred first  */
#define SERCOM_SPIS_CTRLA_DORD_MSB            (SERCOM_SPIS_CTRLA_DORD_MSB_Val << SERCOM_SPIS_CTRLA_DORD_Pos) /**< (SERCOM_SPIS_CTRLA) MSB is transferred first Position  */
#define SERCOM_SPIS_CTRLA_DORD_LSB            (SERCOM_SPIS_CTRLA_DORD_LSB_Val << SERCOM_SPIS_CTRLA_DORD_Pos) /**< (SERCOM_SPIS_CTRLA) LSB is transferred first Position  */
#define SERCOM_SPIS_CTRLA_Msk                 _U_(0x7F33019F)                                      /**< (SERCOM_SPIS_CTRLA) Register Mask  */


/* -------- SERCOM_USART_EXT_CTRLA : (SERCOM Offset: 0x00) (R/W 32) USART_EXT Control A -------- */
#define SERCOM_USART_EXT_CTRLA_RESETVALUE     _U_(0x00)                                            /**<  (SERCOM_USART_EXT_CTRLA) USART_EXT Control A  Reset Value */

#define SERCOM_USART_EXT_CTRLA_SWRST_Pos      _U_(0)                                               /**< (SERCOM_USART_EXT_CTRLA) Software Reset Position */
#define SERCOM_USART_EXT_CTRLA_SWRST_Msk      (_U_(0x1) << SERCOM_USART_EXT_CTRLA_SWRST_Pos)       /**< (SERCOM_USART_EXT_CTRLA) Software Reset Mask */
#define SERCOM_USART_EXT_CTRLA_SWRST(value)   (SERCOM_USART_EXT_CTRLA_SWRST_Msk & ((value) << SERCOM_USART_EXT_CTRLA_SWRST_Pos))
#define SERCOM_USART_EXT_CTRLA_ENABLE_Pos     _U_(1)                                               /**< (SERCOM_USART_EXT_CTRLA) Enable Position */
#define SERCOM_USART_EXT_CTRLA_ENABLE_Msk     (_U_(0x1) << SERCOM_USART_EXT_CTRLA_ENABLE_Pos)      /**< (SERCOM_USART_EXT_CTRLA) Enable Mask */
#define SERCOM_USART_EXT_CTRLA_ENABLE(value)  (SERCOM_USART_EXT_CTRLA_ENABLE_Msk & ((value) << SERCOM_USART_EXT_CTRLA_ENABLE_Pos))
#define SERCOM_USART_EXT_CTRLA_MODE_Pos       _U_(2)                                               /**< (SERCOM_USART_EXT_CTRLA) Operating Mode Position */
#define SERCOM_USART_EXT_CTRLA_MODE_Msk       (_U_(0x7) << SERCOM_USART_EXT_CTRLA_MODE_Pos)        /**< (SERCOM_USART_EXT_CTRLA) Operating Mode Mask */
#define SERCOM_USART_EXT_CTRLA_MODE(value)    (SERCOM_USART_EXT_CTRLA_MODE_Msk & ((value) << SERCOM_USART_EXT_CTRLA_MODE_Pos))
#define   SERCOM_USART_EXT_CTRLA_MODE_USART_EXT_CLK_Val _U_(0x0)                                             /**< (SERCOM_USART_EXT_CTRLA) USART with external clock  */
#define   SERCOM_USART_EXT_CTRLA_MODE_USART_INT_CLK_Val _U_(0x1)                                             /**< (SERCOM_USART_EXT_CTRLA) USART with internal clock  */
#define   SERCOM_USART_EXT_CTRLA_MODE_SPI_SLAVE_Val _U_(0x2)                                             /**< (SERCOM_USART_EXT_CTRLA) SPI in slave operation  */
#define   SERCOM_USART_EXT_CTRLA_MODE_SPI_MASTER_Val _U_(0x3)                                             /**< (SERCOM_USART_EXT_CTRLA) SPI in master operation  */
#define   SERCOM_USART_EXT_CTRLA_MODE_I2C_SLAVE_Val _U_(0x4)                                             /**< (SERCOM_USART_EXT_CTRLA) I2C slave operation  */
#define   SERCOM_USART_EXT_CTRLA_MODE_I2C_MASTER_Val _U_(0x5)                                             /**< (SERCOM_USART_EXT_CTRLA) I2C master operation  */
#define SERCOM_USART_EXT_CTRLA_MODE_USART_EXT_CLK (SERCOM_USART_EXT_CTRLA_MODE_USART_EXT_CLK_Val << SERCOM_USART_EXT_CTRLA_MODE_Pos) /**< (SERCOM_USART_EXT_CTRLA) USART with external clock Position  */
#define SERCOM_USART_EXT_CTRLA_MODE_USART_INT_CLK (SERCOM_USART_EXT_CTRLA_MODE_USART_INT_CLK_Val << SERCOM_USART_EXT_CTRLA_MODE_Pos) /**< (SERCOM_USART_EXT_CTRLA) USART with internal clock Position  */
#define SERCOM_USART_EXT_CTRLA_MODE_SPI_SLAVE (SERCOM_USART_EXT_CTRLA_MODE_SPI_SLAVE_Val << SERCOM_USART_EXT_CTRLA_MODE_Pos) /**< (SERCOM_USART_EXT_CTRLA) SPI in slave operation Position  */
#define SERCOM_USART_EXT_CTRLA_MODE_SPI_MASTER (SERCOM_USART_EXT_CTRLA_MODE_SPI_MASTER_Val << SERCOM_USART_EXT_CTRLA_MODE_Pos) /**< (SERCOM_USART_EXT_CTRLA) SPI in master operation Position  */
#define SERCOM_USART_EXT_CTRLA_MODE_I2C_SLAVE (SERCOM_USART_EXT_CTRLA_MODE_I2C_SLAVE_Val << SERCOM_USART_EXT_CTRLA_MODE_Pos) /**< (SERCOM_USART_EXT_CTRLA) I2C slave operation Position  */
#define SERCOM_USART_EXT_CTRLA_MODE_I2C_MASTER (SERCOM_USART_EXT_CTRLA_MODE_I2C_MASTER_Val << SERCOM_USART_EXT_CTRLA_MODE_Pos) /**< (SERCOM_USART_EXT_CTRLA) I2C master operation Position  */
#define SERCOM_USART_EXT_CTRLA_RUNSTDBY_Pos   _U_(7)                                               /**< (SERCOM_USART_EXT_CTRLA) Run during Standby Position */
#define SERCOM_USART_EXT_CTRLA_RUNSTDBY_Msk   (_U_(0x1) << SERCOM_USART_EXT_CTRLA_RUNSTDBY_Pos)    /**< (SERCOM_USART_EXT_CTRLA) Run during Standby Mask */
#define SERCOM_USART_EXT_CTRLA_RUNSTDBY(value) (SERCOM_USART_EXT_CTRLA_RUNSTDBY_Msk & ((value) << SERCOM_USART_EXT_CTRLA_RUNSTDBY_Pos))
#define SERCOM_USART_EXT_CTRLA_IBON_Pos       _U_(8)                                               /**< (SERCOM_USART_EXT_CTRLA) Immediate Buffer Overflow Notification Position */
#define SERCOM_USART_EXT_CTRLA_IBON_Msk       (_U_(0x1) << SERCOM_USART_EXT_CTRLA_IBON_Pos)        /**< (SERCOM_USART_EXT_CTRLA) Immediate Buffer Overflow Notification Mask */
#define SERCOM_USART_EXT_CTRLA_IBON(value)    (SERCOM_USART_EXT_CTRLA_IBON_Msk & ((value) << SERCOM_USART_EXT_CTRLA_IBON_Pos))
#define SERCOM_USART_EXT_CTRLA_TXINV_Pos      _U_(9)                                               /**< (SERCOM_USART_EXT_CTRLA) Transmit Data Invert Position */
#define SERCOM_USART_EXT_CTRLA_TXINV_Msk      (_U_(0x1) << SERCOM_USART_EXT_CTRLA_TXINV_Pos)       /**< (SERCOM_USART_EXT_CTRLA) Transmit Data Invert Mask */
#define SERCOM_USART_EXT_CTRLA_TXINV(value)   (SERCOM_USART_EXT_CTRLA_TXINV_Msk & ((value) << SERCOM_USART_EXT_CTRLA_TXINV_Pos))
#define SERCOM_USART_EXT_CTRLA_RXINV_Pos      _U_(10)                                              /**< (SERCOM_USART_EXT_CTRLA) Receive Data Invert Position */
#define SERCOM_USART_EXT_CTRLA_RXINV_Msk      (_U_(0x1) << SERCOM_USART_EXT_CTRLA_RXINV_Pos)       /**< (SERCOM_USART_EXT_CTRLA) Receive Data Invert Mask */
#define SERCOM_USART_EXT_CTRLA_RXINV(value)   (SERCOM_USART_EXT_CTRLA_RXINV_Msk & ((value) << SERCOM_USART_EXT_CTRLA_RXINV_Pos))
#define SERCOM_USART_EXT_CTRLA_SAMPR_Pos      _U_(13)                                              /**< (SERCOM_USART_EXT_CTRLA) Sample Position */
#define SERCOM_USART_EXT_CTRLA_SAMPR_Msk      (_U_(0x7) << SERCOM_USART_EXT_CTRLA_SAMPR_Pos)       /**< (SERCOM_USART_EXT_CTRLA) Sample Mask */
#define SERCOM_USART_EXT_CTRLA_SAMPR(value)   (SERCOM_USART_EXT_CTRLA_SAMPR_Msk & ((value) << SERCOM_USART_EXT_CTRLA_SAMPR_Pos))
#define   SERCOM_USART_EXT_CTRLA_SAMPR_16X_ARITHMETIC_Val _U_(0x0)                                             /**< (SERCOM_USART_EXT_CTRLA) 16x over-sampling using arithmetic baudrate generation  */
#define   SERCOM_USART_EXT_CTRLA_SAMPR_16X_FRACTIONAL_Val _U_(0x1)                                             /**< (SERCOM_USART_EXT_CTRLA) 16x over-sampling using fractional baudrate generation  */
#define   SERCOM_USART_EXT_CTRLA_SAMPR_8X_ARITHMETIC_Val _U_(0x2)                                             /**< (SERCOM_USART_EXT_CTRLA) 8x over-sampling using arithmetic baudrate generation  */
#define   SERCOM_USART_EXT_CTRLA_SAMPR_8X_FRACTIONAL_Val _U_(0x3)                                             /**< (SERCOM_USART_EXT_CTRLA) 8x over-sampling using fractional baudrate generation  */
#define   SERCOM_USART_EXT_CTRLA_SAMPR_3X_ARITHMETIC_Val _U_(0x4)                                             /**< (SERCOM_USART_EXT_CTRLA) 3x over-sampling using arithmetic baudrate generation  */
#define SERCOM_USART_EXT_CTRLA_SAMPR_16X_ARITHMETIC (SERCOM_USART_EXT_CTRLA_SAMPR_16X_ARITHMETIC_Val << SERCOM_USART_EXT_CTRLA_SAMPR_Pos) /**< (SERCOM_USART_EXT_CTRLA) 16x over-sampling using arithmetic baudrate generation Position  */
#define SERCOM_USART_EXT_CTRLA_SAMPR_16X_FRACTIONAL (SERCOM_USART_EXT_CTRLA_SAMPR_16X_FRACTIONAL_Val << SERCOM_USART_EXT_CTRLA_SAMPR_Pos) /**< (SERCOM_USART_EXT_CTRLA) 16x over-sampling using fractional baudrate generation Position  */
#define SERCOM_USART_EXT_CTRLA_SAMPR_8X_ARITHMETIC (SERCOM_USART_EXT_CTRLA_SAMPR_8X_ARITHMETIC_Val << SERCOM_USART_EXT_CTRLA_SAMPR_Pos) /**< (SERCOM_USART_EXT_CTRLA) 8x over-sampling using arithmetic baudrate generation Position  */
#define SERCOM_USART_EXT_CTRLA_SAMPR_8X_FRACTIONAL (SERCOM_USART_EXT_CTRLA_SAMPR_8X_FRACTIONAL_Val << SERCOM_USART_EXT_CTRLA_SAMPR_Pos) /**< (SERCOM_USART_EXT_CTRLA) 8x over-sampling using fractional baudrate generation Position  */
#define SERCOM_USART_EXT_CTRLA_SAMPR_3X_ARITHMETIC (SERCOM_USART_EXT_CTRLA_SAMPR_3X_ARITHMETIC_Val << SERCOM_USART_EXT_CTRLA_SAMPR_Pos) /**< (SERCOM_USART_EXT_CTRLA) 3x over-sampling using arithmetic baudrate generation Position  */
#define SERCOM_USART_EXT_CTRLA_TXPO_Pos       _U_(16)                                              /**< (SERCOM_USART_EXT_CTRLA) Transmit Data Pinout Position */
#define SERCOM_USART_EXT_CTRLA_TXPO_Msk       (_U_(0x3) << SERCOM_USART_EXT_CTRLA_TXPO_Pos)        /**< (SERCOM_USART_EXT_CTRLA) Transmit Data Pinout Mask */
#define SERCOM_USART_EXT_CTRLA_TXPO(value)    (SERCOM_USART_EXT_CTRLA_TXPO_Msk & ((value) << SERCOM_USART_EXT_CTRLA_TXPO_Pos))
#define   SERCOM_USART_EXT_CTRLA_TXPO_PAD0_Val _U_(0x0)                                             /**< (SERCOM_USART_EXT_CTRLA) SERCOM PAD[0] is used for data transmission  */
#define   SERCOM_USART_EXT_CTRLA_TXPO_PAD3_Val _U_(0x3)                                             /**< (SERCOM_USART_EXT_CTRLA) SERCOM_PAD[0] is used for data transmission  */
#define SERCOM_USART_EXT_CTRLA_TXPO_PAD0      (SERCOM_USART_EXT_CTRLA_TXPO_PAD0_Val << SERCOM_USART_EXT_CTRLA_TXPO_Pos) /**< (SERCOM_USART_EXT_CTRLA) SERCOM PAD[0] is used for data transmission Position  */
#define SERCOM_USART_EXT_CTRLA_TXPO_PAD3      (SERCOM_USART_EXT_CTRLA_TXPO_PAD3_Val << SERCOM_USART_EXT_CTRLA_TXPO_Pos) /**< (SERCOM_USART_EXT_CTRLA) SERCOM_PAD[0] is used for data transmission Position  */
#define SERCOM_USART_EXT_CTRLA_RXPO_Pos       _U_(20)                                              /**< (SERCOM_USART_EXT_CTRLA) Receive Data Pinout Position */
#define SERCOM_USART_EXT_CTRLA_RXPO_Msk       (_U_(0x3) << SERCOM_USART_EXT_CTRLA_RXPO_Pos)        /**< (SERCOM_USART_EXT_CTRLA) Receive Data Pinout Mask */
#define SERCOM_USART_EXT_CTRLA_RXPO(value)    (SERCOM_USART_EXT_CTRLA_RXPO_Msk & ((value) << SERCOM_USART_EXT_CTRLA_RXPO_Pos))
#define   SERCOM_USART_EXT_CTRLA_RXPO_PAD0_Val _U_(0x0)                                             /**< (SERCOM_USART_EXT_CTRLA) SERCOM PAD[0] is used for data reception  */
#define   SERCOM_USART_EXT_CTRLA_RXPO_PAD1_Val _U_(0x1)                                             /**< (SERCOM_USART_EXT_CTRLA) SERCOM PAD[1] is used for data reception  */
#define   SERCOM_USART_EXT_CTRLA_RXPO_PAD2_Val _U_(0x2)                                             /**< (SERCOM_USART_EXT_CTRLA) SERCOM PAD[2] is used for data reception  */
#define   SERCOM_USART_EXT_CTRLA_RXPO_PAD3_Val _U_(0x3)                                             /**< (SERCOM_USART_EXT_CTRLA) SERCOM PAD[3] is used for data reception  */
#define SERCOM_USART_EXT_CTRLA_RXPO_PAD0      (SERCOM_USART_EXT_CTRLA_RXPO_PAD0_Val << SERCOM_USART_EXT_CTRLA_RXPO_Pos) /**< (SERCOM_USART_EXT_CTRLA) SERCOM PAD[0] is used for data reception Position  */
#define SERCOM_USART_EXT_CTRLA_RXPO_PAD1      (SERCOM_USART_EXT_CTRLA_RXPO_PAD1_Val << SERCOM_USART_EXT_CTRLA_RXPO_Pos) /**< (SERCOM_USART_EXT_CTRLA) SERCOM PAD[1] is used for data reception Position  */
#define SERCOM_USART_EXT_CTRLA_RXPO_PAD2      (SERCOM_USART_EXT_CTRLA_RXPO_PAD2_Val << SERCOM_USART_EXT_CTRLA_RXPO_Pos) /**< (SERCOM_USART_EXT_CTRLA) SERCOM PAD[2] is used for data reception Position  */
#define SERCOM_USART_EXT_CTRLA_RXPO_PAD3      (SERCOM_USART_EXT_CTRLA_RXPO_PAD3_Val << SERCOM_USART_EXT_CTRLA_RXPO_Pos) /**< (SERCOM_USART_EXT_CTRLA) SERCOM PAD[3] is used for data reception Position  */
#define SERCOM_USART_EXT_CTRLA_SAMPA_Pos      _U_(22)                                              /**< (SERCOM_USART_EXT_CTRLA) Sample Adjustment Position */
#define SERCOM_USART_EXT_CTRLA_SAMPA_Msk      (_U_(0x3) << SERCOM_USART_EXT_CTRLA_SAMPA_Pos)       /**< (SERCOM_USART_EXT_CTRLA) Sample Adjustment Mask */
#define SERCOM_USART_EXT_CTRLA_SAMPA(value)   (SERCOM_USART_EXT_CTRLA_SAMPA_Msk & ((value) << SERCOM_USART_EXT_CTRLA_SAMPA_Pos))
#define SERCOM_USART_EXT_CTRLA_FORM_Pos       _U_(24)                                              /**< (SERCOM_USART_EXT_CTRLA) Frame Format Position */
#define SERCOM_USART_EXT_CTRLA_FORM_Msk       (_U_(0xF) << SERCOM_USART_EXT_CTRLA_FORM_Pos)        /**< (SERCOM_USART_EXT_CTRLA) Frame Format Mask */
#define SERCOM_USART_EXT_CTRLA_FORM(value)    (SERCOM_USART_EXT_CTRLA_FORM_Msk & ((value) << SERCOM_USART_EXT_CTRLA_FORM_Pos))
#define   SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_NO_PARITY_Val _U_(0x0)                                             /**< (SERCOM_USART_EXT_CTRLA) USART frame  */
#define   SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_WITH_PARITY_Val _U_(0x1)                                             /**< (SERCOM_USART_EXT_CTRLA) USART frame with parity  */
#define   SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_LIN_MASTER_MODE_Val _U_(0x2)                                             /**< (SERCOM_USART_EXT_CTRLA) LIN Master - Break and sync generation  */
#define   SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_AUTO_BAUD_NO_PARITY_Val _U_(0x4)                                             /**< (SERCOM_USART_EXT_CTRLA) Auto-baud - break detection and auto-baud  */
#define   SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_AUTO_BAUD_WITH_PARITY_Val _U_(0x5)                                             /**< (SERCOM_USART_EXT_CTRLA) Auto-baud - break detection and auto-baud with parity  */
#define   SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_ISO_7816_Val _U_(0x7)                                             /**< (SERCOM_USART_EXT_CTRLA) ISO 7816  */
#define SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_NO_PARITY (SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_NO_PARITY_Val << SERCOM_USART_EXT_CTRLA_FORM_Pos) /**< (SERCOM_USART_EXT_CTRLA) USART frame Position  */
#define SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_WITH_PARITY (SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_WITH_PARITY_Val << SERCOM_USART_EXT_CTRLA_FORM_Pos) /**< (SERCOM_USART_EXT_CTRLA) USART frame with parity Position  */
#define SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_LIN_MASTER_MODE (SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_LIN_MASTER_MODE_Val << SERCOM_USART_EXT_CTRLA_FORM_Pos) /**< (SERCOM_USART_EXT_CTRLA) LIN Master - Break and sync generation Position  */
#define SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_AUTO_BAUD_NO_PARITY (SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_AUTO_BAUD_NO_PARITY_Val << SERCOM_USART_EXT_CTRLA_FORM_Pos) /**< (SERCOM_USART_EXT_CTRLA) Auto-baud - break detection and auto-baud Position  */
#define SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_AUTO_BAUD_WITH_PARITY (SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_AUTO_BAUD_WITH_PARITY_Val << SERCOM_USART_EXT_CTRLA_FORM_Pos) /**< (SERCOM_USART_EXT_CTRLA) Auto-baud - break detection and auto-baud with parity Position  */
#define SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_ISO_7816 (SERCOM_USART_EXT_CTRLA_FORM_USART_FRAME_ISO_7816_Val << SERCOM_USART_EXT_CTRLA_FORM_Pos) /**< (SERCOM_USART_EXT_CTRLA) ISO 7816 Position  */
#define SERCOM_USART_EXT_CTRLA_CMODE_Pos      _U_(28)                                              /**< (SERCOM_USART_EXT_CTRLA) Communication Mode Position */
#define SERCOM_USART_EXT_CTRLA_CMODE_Msk      (_U_(0x1) << SERCOM_USART_EXT_CTRLA_CMODE_Pos)       /**< (SERCOM_USART_EXT_CTRLA) Communication Mode Mask */
#define SERCOM_USART_EXT_CTRLA_CMODE(value)   (SERCOM_USART_EXT_CTRLA_CMODE_Msk & ((value) << SERCOM_USART_EXT_CTRLA_CMODE_Pos))
#define   SERCOM_USART_EXT_CTRLA_CMODE_ASYNC_Val _U_(0x0)                                             /**< (SERCOM_USART_EXT_CTRLA) Asynchronous Communication  */
#define   SERCOM_USART_EXT_CTRLA_CMODE_SYNC_Val _U_(0x1)                                             /**< (SERCOM_USART_EXT_CTRLA) Synchronous Communication  */
#define SERCOM_USART_EXT_CTRLA_CMODE_ASYNC    (SERCOM_USART_EXT_CTRLA_CMODE_ASYNC_Val << SERCOM_USART_EXT_CTRLA_CMODE_Pos) /**< (SERCOM_USART_EXT_CTRLA) Asynchronous Communication Position  */
#define SERCOM_USART_EXT_CTRLA_CMODE_SYNC     (SERCOM_USART_EXT_CTRLA_CMODE_SYNC_Val << SERCOM_USART_EXT_CTRLA_CMODE_Pos) /**< (SERCOM_USART_EXT_CTRLA) Synchronous Communication Position  */
#define SERCOM_USART_EXT_CTRLA_CPOL_Pos       _U_(29)                                              /**< (SERCOM_USART_EXT_CTRLA) Clock Polarity Position */
#define SERCOM_USART_EXT_CTRLA_CPOL_Msk       (_U_(0x1) << SERCOM_USART_EXT_CTRLA_CPOL_Pos)        /**< (SERCOM_USART_EXT_CTRLA) Clock Polarity Mask */
#define SERCOM_USART_EXT_CTRLA_CPOL(value)    (SERCOM_USART_EXT_CTRLA_CPOL_Msk & ((value) << SERCOM_USART_EXT_CTRLA_CPOL_Pos))
#define   SERCOM_USART_EXT_CTRLA_CPOL_IDLE_LOW_Val _U_(0x0)                                             /**< (SERCOM_USART_EXT_CTRLA) TxD Change:- Rising XCK edge, RxD Sample:- Falling XCK edge  */
#define   SERCOM_USART_EXT_CTRLA_CPOL_IDLE_HIGH_Val _U_(0x1)                                             /**< (SERCOM_USART_EXT_CTRLA) TxD Change:- Falling XCK edge, RxD Sample:- Rising XCK edge  */
#define SERCOM_USART_EXT_CTRLA_CPOL_IDLE_LOW  (SERCOM_USART_EXT_CTRLA_CPOL_IDLE_LOW_Val << SERCOM_USART_EXT_CTRLA_CPOL_Pos) /**< (SERCOM_USART_EXT_CTRLA) TxD Change:- Rising XCK edge, RxD Sample:- Falling XCK edge Position  */
#define SERCOM_USART_EXT_CTRLA_CPOL_IDLE_HIGH (SERCOM_USART_EXT_CTRLA_CPOL_IDLE_HIGH_Val << SERCOM_USART_EXT_CTRLA_CPOL_Pos) /**< (SERCOM_USART_EXT_CTRLA) TxD Change:- Falling XCK edge, RxD Sample:- Rising XCK edge Position  */
#define SERCOM_USART_EXT_CTRLA_DORD_Pos       _U_(30)                                              /**< (SERCOM_USART_EXT_CTRLA) Data Order Position */
#define SERCOM_USART_EXT_CTRLA_DORD_Msk       (_U_(0x1) << SERCOM_USART_EXT_CTRLA_DORD_Pos)        /**< (SERCOM_USART_EXT_CTRLA) Data Order Mask */
#define SERCOM_USART_EXT_CTRLA_DORD(value)    (SERCOM_USART_EXT_CTRLA_DORD_Msk & ((value) << SERCOM_USART_EXT_CTRLA_DORD_Pos))
#define   SERCOM_USART_EXT_CTRLA_DORD_MSB_Val _U_(0x0)                                             /**< (SERCOM_USART_EXT_CTRLA) MSB is transmitted first  */
#define   SERCOM_USART_EXT_CTRLA_DORD_LSB_Val _U_(0x1)                                             /**< (SERCOM_USART_EXT_CTRLA) LSB is transmitted first  */
#define SERCOM_USART_EXT_CTRLA_DORD_MSB       (SERCOM_USART_EXT_CTRLA_DORD_MSB_Val << SERCOM_USART_EXT_CTRLA_DORD_Pos) /**< (SERCOM_USART_EXT_CTRLA) MSB is transmitted first Position  */
#define SERCOM_USART_EXT_CTRLA_DORD_LSB       (SERCOM_USART_EXT_CTRLA_DORD_LSB_Val << SERCOM_USART_EXT_CTRLA_DORD_Pos) /**< (SERCOM_USART_EXT_CTRLA) LSB is transmitted first Position  */
#define SERCOM_USART_EXT_CTRLA_Msk            _U_(0x7FF3E79F)                                      /**< (SERCOM_USART_EXT_CTRLA) Register Mask  */


/* -------- SERCOM_USART_INT_CTRLA : (SERCOM Offset: 0x00) (R/W 32) USART_INT Control A -------- */
#define SERCOM_USART_INT_CTRLA_RESETVALUE     _U_(0x00)                                            /**<  (SERCOM_USART_INT_CTRLA) USART_INT Control A  Reset Value */

#define SERCOM_USART_INT_CTRLA_SWRST_Pos      _U_(0)                                               /**< (SERCOM_USART_INT_CTRLA) Software Reset Position */
#define SERCOM_USART_INT_CTRLA_SWRST_Msk      (_U_(0x1) << SERCOM_USART_INT_CTRLA_SWRST_Pos)       /**< (SERCOM_USART_INT_CTRLA) Software Reset Mask */
#define SERCOM_USART_INT_CTRLA_SWRST(value)   (SERCOM_USART_INT_CTRLA_SWRST_Msk & ((value) << SERCOM_USART_INT_CTRLA_SWRST_Pos))
#define SERCOM_USART_INT_CTRLA_ENABLE_Pos     _U_(1)                                               /**< (SERCOM_USART_INT_CTRLA) Enable Position */
#define SERCOM_USART_INT_CTRLA_ENABLE_Msk     (_U_(0x1) << SERCOM_USART_INT_CTRLA_ENABLE_Pos)      /**< (SERCOM_USART_INT_CTRLA) Enable Mask */
#define SERCOM_USART_INT_CTRLA_ENABLE(value)  (SERCOM_USART_INT_CTRLA_ENABLE_Msk & ((value) << SERCOM_USART_INT_CTRLA_ENABLE_Pos))
#define SERCOM_USART_INT_CTRLA_MODE_Pos       _U_(2)                                               /**< (SERCOM_USART_INT_CTRLA) Operating Mode Position */
#define SERCOM_USART_INT_CTRLA_MODE_Msk       (_U_(0x7) << SERCOM_USART_INT_CTRLA_MODE_Pos)        /**< (SERCOM_USART_INT_CTRLA) Operating Mode Mask */
#define SERCOM_USART_INT_CTRLA_MODE(value)    (SERCOM_USART_INT_CTRLA_MODE_Msk & ((value) << SERCOM_USART_INT_CTRLA_MODE_Pos))
#define   SERCOM_USART_INT_CTRLA_MODE_USART_EXT_CLK_Val _U_(0x0)                                             /**< (SERCOM_USART_INT_CTRLA) USART with external clock  */
#define   SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK_Val _U_(0x1)                                             /**< (SERCOM_USART_INT_CTRLA) USART with internal clock  */
#define   SERCOM_USART_INT_CTRLA_MODE_SPI_SLAVE_Val _U_(0x2)                                             /**< (SERCOM_USART_INT_CTRLA) SPI in slave operation  */
#define   SERCOM_USART_INT_CTRLA_MODE_SPI_MASTER_Val _U_(0x3)                                             /**< (SERCOM_USART_INT_CTRLA) SPI in master operation  */
#define   SERCOM_USART_INT_CTRLA_MODE_I2C_SLAVE_Val _U_(0x4)                                             /**< (SERCOM_USART_INT_CTRLA) I2C slave operation  */
#define   SERCOM_USART_INT_CTRLA_MODE_I2C_MASTER_Val _U_(0x5)                                             /**< (SERCOM_USART_INT_CTRLA) I2C master operation  */
#define SERCOM_USART_INT_CTRLA_MODE_USART_EXT_CLK (SERCOM_USART_INT_CTRLA_MODE_USART_EXT_CLK_Val << SERCOM_USART_INT_CTRLA_MODE_Pos) /**< (SERCOM_USART_INT_CTRLA) USART with external clock Position  */
#define SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK (SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK_Val << SERCOM_USART_INT_CTRLA_MODE_Pos) /**< (SERCOM_USART_INT_CTRLA) USART with internal clock Position  */
#define SERCOM_USART_INT_CTRLA_MODE_SPI_SLAVE (SERCOM_USART_INT_CTRLA_MODE_SPI_SLAVE_Val << SERCOM_USART_INT_CTRLA_MODE_Pos) /**< (SERCOM_USART_INT_CTRLA) SPI in slave operation Position  */
#define SERCOM_USART_INT_CTRLA_MODE_SPI_MASTER (SERCOM_USART_INT_CTRLA_MODE_SPI_MASTER_Val << SERCOM_USART_INT_CTRLA_MODE_Pos) /**< (SERCOM_USART_INT_CTRLA) SPI in master operation Position  */
#define SERCOM_USART_INT_CTRLA_MODE_I2C_SLAVE (SERCOM_USART_INT_CTRLA_MODE_I2C_SLAVE_Val << SERCOM_USART_INT_CTRLA_MODE_Pos) /**< (SERCOM_USART_INT_CTRLA) I2C slave operation Position  */
#define SERCOM_USART_INT_CTRLA_MODE_I2C_MASTER (SERCOM_USART_INT_CTRLA_MODE_I2C_MASTER_Val << SERCOM_USART_INT_CTRLA_MODE_Pos) /**< (SERCOM_USART_INT_CTRLA) I2C master operation Position  */
#define SERCOM_USART_INT_CTRLA_RUNSTDBY_Pos   _U_(7)                                               /**< (SERCOM_USART_INT_CTRLA) Run during Standby Position */
#define SERCOM_USART_INT_CTRLA_RUNSTDBY_Msk   (_U_(0x1) << SERCOM_USART_INT_CTRLA_RUNSTDBY_Pos)    /**< (SERCOM_USART_INT_CTRLA) Run during Standby Mask */
#define SERCOM_USART_INT_CTRLA_RUNSTDBY(value) (SERCOM_USART_INT_CTRLA_RUNSTDBY_Msk & ((value) << SERCOM_USART_INT_CTRLA_RUNSTDBY_Pos))
#define SERCOM_USART_INT_CTRLA_IBON_Pos       _U_(8)                                               /**< (SERCOM_USART_INT_CTRLA) Immediate Buffer Overflow Notification Position */
#define SERCOM_USART_INT_CTRLA_IBON_Msk       (_U_(0x1) << SERCOM_USART_INT_CTRLA_IBON_Pos)        /**< (SERCOM_USART_INT_CTRLA) Immediate Buffer Overflow Notification Mask */
#define SERCOM_USART_INT_CTRLA_IBON(value)    (SERCOM_USART_INT_CTRLA_IBON_Msk & ((value) << SERCOM_USART_INT_CTRLA_IBON_Pos))
#define SERCOM_USART_INT_CTRLA_TXINV_Pos      _U_(9)                                               /**< (SERCOM_USART_INT_CTRLA) Transmit Data Invert Position */
#define SERCOM_USART_INT_CTRLA_TXINV_Msk      (_U_(0x1) << SERCOM_USART_INT_CTRLA_TXINV_Pos)       /**< (SERCOM_USART_INT_CTRLA) Transmit Data Invert Mask */
#define SERCOM_USART_INT_CTRLA_TXINV(value)   (SERCOM_USART_INT_CTRLA_TXINV_Msk & ((value) << SERCOM_USART_INT_CTRLA_TXINV_Pos))
#define SERCOM_USART_INT_CTRLA_RXINV_Pos      _U_(10)                                              /**< (SERCOM_USART_INT_CTRLA) Receive Data Invert Position */
#define SERCOM_USART_INT_CTRLA_RXINV_Msk      (_U_(0x1) << SERCOM_USART_INT_CTRLA_RXINV_Pos)       /**< (SERCOM_USART_INT_CTRLA) Receive Data Invert Mask */
#define SERCOM_USART_INT_CTRLA_RXINV(value)   (SERCOM_USART_INT_CTRLA_RXINV_Msk & ((value) << SERCOM_USART_INT_CTRLA_RXINV_Pos))
#define SERCOM_USART_INT_CTRLA_SAMPR_Pos      _U_(13)                                              /**< (SERCOM_USART_INT_CTRLA) Sample Position */
#define SERCOM_USART_INT_CTRLA_SAMPR_Msk      (_U_(0x7) << SERCOM_USART_INT_CTRLA_SAMPR_Pos)       /**< (SERCOM_USART_INT_CTRLA) Sample Mask */
#define SERCOM_USART_INT_CTRLA_SAMPR(value)   (SERCOM_USART_INT_CTRLA_SAMPR_Msk & ((value) << SERCOM_USART_INT_CTRLA_SAMPR_Pos))
#define   SERCOM_USART_INT_CTRLA_SAMPR_16X_ARITHMETIC_Val _U_(0x0)                                             /**< (SERCOM_USART_INT_CTRLA) 16x over-sampling using arithmetic baudrate generation  */
#define   SERCOM_USART_INT_CTRLA_SAMPR_16X_FRACTIONAL_Val _U_(0x1)                                             /**< (SERCOM_USART_INT_CTRLA) 16x over-sampling using fractional baudrate generation  */
#define   SERCOM_USART_INT_CTRLA_SAMPR_8X_ARITHMETIC_Val _U_(0x2)                                             /**< (SERCOM_USART_INT_CTRLA) 8x over-sampling using arithmetic baudrate generation  */
#define   SERCOM_USART_INT_CTRLA_SAMPR_8X_FRACTIONAL_Val _U_(0x3)                                             /**< (SERCOM_USART_INT_CTRLA) 8x over-sampling using fractional baudrate generation  */
#define   SERCOM_USART_INT_CTRLA_SAMPR_3X_ARITHMETIC_Val _U_(0x4)                                             /**< (SERCOM_USART_INT_CTRLA) 3x over-sampling using arithmetic baudrate generation  */
#define SERCOM_USART_INT_CTRLA_SAMPR_16X_ARITHMETIC (SERCOM_USART_INT_CTRLA_SAMPR_16X_ARITHMETIC_Val << SERCOM_USART_INT_CTRLA_SAMPR_Pos) /**< (SERCOM_USART_INT_CTRLA) 16x over-sampling using arithmetic baudrate generation Position  */
#define SERCOM_USART_INT_CTRLA_SAMPR_16X_FRACTIONAL (SERCOM_USART_INT_CTRLA_SAMPR_16X_FRACTIONAL_Val << SERCOM_USART_INT_CTRLA_SAMPR_Pos) /**< (SERCOM_USART_INT_CTRLA) 16x over-sampling using fractional baudrate generation Position  */
#define SERCOM_USART_INT_CTRLA_SAMPR_8X_ARITHMETIC (SERCOM_USART_INT_CTRLA_SAMPR_8X_ARITHMETIC_Val << SERCOM_USART_INT_CTRLA_SAMPR_Pos) /**< (SERCOM_USART_INT_CTRLA) 8x over-sampling using arithmetic baudrate generation Position  */
#define SERCOM_USART_INT_CTRLA_SAMPR_8X_FRACTIONAL (SERCOM_USART_INT_CTRLA_SAMPR_8X_FRACTIONAL_Val << SERCOM_USART_INT_CTRLA_SAMPR_Pos) /**< (SERCOM_USART_INT_CTRLA) 8x over-sampling using fractional baudrate generation Position  */
#define SERCOM_USART_INT_CTRLA_SAMPR_3X_ARITHMETIC (SERCOM_USART_INT_CTRLA_SAMPR_3X_ARITHMETIC_Val << SERCOM_USART_INT_CTRLA_SAMPR_Pos) /**< (SERCOM_USART_INT_CTRLA) 3x over-sampling using arithmetic baudrate generation Position  */
#define SERCOM_USART_INT_CTRLA_TXPO_Pos       _U_(16)                                              /**< (SERCOM_USART_INT_CTRLA) Transmit Data Pinout Position */
#define SERCOM_USART_INT_CTRLA_TXPO_Msk       (_U_(0x3) << SERCOM_USART_INT_CTRLA_TXPO_Pos)        /**< (SERCOM_USART_INT_CTRLA) Transmit Data Pinout Mask */
#define SERCOM_USART_INT_CTRLA_TXPO(value)    (SERCOM_USART_INT_CTRLA_TXPO_Msk & ((value) << SERCOM_USART_INT_CTRLA_TXPO_Pos))
#define   SERCOM_USART_INT_CTRLA_TXPO_PAD0_Val _U_(0x0)                                             /**< (SERCOM_USART_INT_CTRLA) SERCOM PAD[0] is used for data transmission  */
#define   SERCOM_USART_INT_CTRLA_TXPO_PAD3_Val _U_(0x3)                                             /**< (SERCOM_USART_INT_CTRLA) SERCOM_PAD[0] is used for data transmission  */
#define SERCOM_USART_INT_CTRLA_TXPO_PAD0      (SERCOM_USART_INT_CTRLA_TXPO_PAD0_Val << SERCOM_USART_INT_CTRLA_TXPO_Pos) /**< (SERCOM_USART_INT_CTRLA) SERCOM PAD[0] is used for data transmission Position  */
#define SERCOM_USART_INT_CTRLA_TXPO_PAD3      (SERCOM_USART_INT_CTRLA_TXPO_PAD3_Val << SERCOM_USART_INT_CTRLA_TXPO_Pos) /**< (SERCOM_USART_INT_CTRLA) SERCOM_PAD[0] is used for data transmission Position  */
#define SERCOM_USART_INT_CTRLA_RXPO_Pos       _U_(20)                                              /**< (SERCOM_USART_INT_CTRLA) Receive Data Pinout Position */
#define SERCOM_USART_INT_CTRLA_RXPO_Msk       (_U_(0x3) << SERCOM_USART_INT_CTRLA_RXPO_Pos)        /**< (SERCOM_USART_INT_CTRLA) Receive Data Pinout Mask */
#define SERCOM_USART_INT_CTRLA_RXPO(value)    (SERCOM_USART_INT_CTRLA_RXPO_Msk & ((value) << SERCOM_USART_INT_CTRLA_RXPO_Pos))
#define   SERCOM_USART_INT_CTRLA_RXPO_PAD0_Val _U_(0x0)                                             /**< (SERCOM_USART_INT_CTRLA) SERCOM PAD[0] is used for data reception  */
#define   SERCOM_USART_INT_CTRLA_RXPO_PAD1_Val _U_(0x1)                                             /**< (SERCOM_USART_INT_CTRLA) SERCOM PAD[1] is used for data reception  */
#define   SERCOM_USART_INT_CTRLA_RXPO_PAD2_Val _U_(0x2)                                             /**< (SERCOM_USART_INT_CTRLA) SERCOM PAD[2] is used for data reception  */
#define   SERCOM_USART_INT_CTRLA_RXPO_PAD3_Val _U_(0x3)                                             /**< (SERCOM_USART_INT_CTRLA) SERCOM PAD[3] is used for data reception  */
#define SERCOM_USART_INT_CTRLA_RXPO_PAD0      (SERCOM_USART_INT_CTRLA_RXPO_PAD0_Val << SERCOM_USART_INT_CTRLA_RXPO_Pos) /**< (SERCOM_USART_INT_CTRLA) SERCOM PAD[0] is used for data reception Position  */
#define SERCOM_USART_INT_CTRLA_RXPO_PAD1      (SERCOM_USART_INT_CTRLA_RXPO_PAD1_Val << SERCOM_USART_INT_CTRLA_RXPO_Pos) /**< (SERCOM_USART_INT_CTRLA) SERCOM PAD[1] is used for data reception Position  */
#define SERCOM_USART_INT_CTRLA_RXPO_PAD2      (SERCOM_USART_INT_CTRLA_RXPO_PAD2_Val << SERCOM_USART_INT_CTRLA_RXPO_Pos) /**< (SERCOM_USART_INT_CTRLA) SERCOM PAD[2] is used for data reception Position  */
#define SERCOM_USART_INT_CTRLA_RXPO_PAD3      (SERCOM_USART_INT_CTRLA_RXPO_PAD3_Val << SERCOM_USART_INT_CTRLA_RXPO_Pos) /**< (SERCOM_USART_INT_CTRLA) SERCOM PAD[3] is used for data reception Position  */
#define SERCOM_USART_INT_CTRLA_SAMPA_Pos      _U_(22)                                              /**< (SERCOM_USART_INT_CTRLA) Sample Adjustment Position */
#define SERCOM_USART_INT_CTRLA_SAMPA_Msk      (_U_(0x3) << SERCOM_USART_INT_CTRLA_SAMPA_Pos)       /**< (SERCOM_USART_INT_CTRLA) Sample Adjustment Mask */
#define SERCOM_USART_INT_CTRLA_SAMPA(value)   (SERCOM_USART_INT_CTRLA_SAMPA_Msk & ((value) << SERCOM_USART_INT_CTRLA_SAMPA_Pos))
#define SERCOM_USART_INT_CTRLA_FORM_Pos       _U_(24)                                              /**< (SERCOM_USART_INT_CTRLA) Frame Format Position */
#define SERCOM_USART_INT_CTRLA_FORM_Msk       (_U_(0xF) << SERCOM_USART_INT_CTRLA_FORM_Pos)        /**< (SERCOM_USART_INT_CTRLA) Frame Format Mask */
#define SERCOM_USART_INT_CTRLA_FORM(value)    (SERCOM_USART_INT_CTRLA_FORM_Msk & ((value) << SERCOM_USART_INT_CTRLA_FORM_Pos))
#define   SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_NO_PARITY_Val _U_(0x0)                                             /**< (SERCOM_USART_INT_CTRLA) USART frame  */
#define   SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_WITH_PARITY_Val _U_(0x1)                                             /**< (SERCOM_USART_INT_CTRLA) USART frame with parity  */
#define   SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_LIN_MASTER_MODE_Val _U_(0x2)                                             /**< (SERCOM_USART_INT_CTRLA) LIN Master - Break and sync generation  */
#define   SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_AUTO_BAUD_NO_PARITY_Val _U_(0x4)                                             /**< (SERCOM_USART_INT_CTRLA) Auto-baud - break detection and auto-baud  */
#define   SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_AUTO_BAUD_WITH_PARITY_Val _U_(0x5)                                             /**< (SERCOM_USART_INT_CTRLA) Auto-baud - break detection and auto-baud with parity  */
#define   SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_ISO_7816_Val _U_(0x7)                                             /**< (SERCOM_USART_INT_CTRLA) ISO 7816  */
#define SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_NO_PARITY (SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_NO_PARITY_Val << SERCOM_USART_INT_CTRLA_FORM_Pos) /**< (SERCOM_USART_INT_CTRLA) USART frame Position  */
#define SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_WITH_PARITY (SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_WITH_PARITY_Val << SERCOM_USART_INT_CTRLA_FORM_Pos) /**< (SERCOM_USART_INT_CTRLA) USART frame with parity Position  */
#define SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_LIN_MASTER_MODE (SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_LIN_MASTER_MODE_Val << SERCOM_USART_INT_CTRLA_FORM_Pos) /**< (SERCOM_USART_INT_CTRLA) LIN Master - Break and sync generation Position  */
#define SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_AUTO_BAUD_NO_PARITY (SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_AUTO_BAUD_NO_PARITY_Val << SERCOM_USART_INT_CTRLA_FORM_Pos) /**< (SERCOM_USART_INT_CTRLA) Auto-baud - break detection and auto-baud Position  */
#define SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_AUTO_BAUD_WITH_PARITY (SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_AUTO_BAUD_WITH_PARITY_Val << SERCOM_USART_INT_CTRLA_FORM_Pos) /**< (SERCOM_USART_INT_CTRLA) Auto-baud - break detection and auto-baud with parity Position  */
#define SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_ISO_7816 (SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_ISO_7816_Val << SERCOM_USART_INT_CTRLA_FORM_Pos) /**< (SERCOM_USART_INT_CTRLA) ISO 7816 Position  */
#define SERCOM_USART_INT_CTRLA_CMODE_Pos      _U_(28)                                              /**< (SERCOM_USART_INT_CTRLA) Communication Mode Position */
#define SERCOM_USART_INT_CTRLA_CMODE_Msk      (_U_(0x1) << SERCOM_USART_INT_CTRLA_CMODE_Pos)       /**< (SERCOM_USART_INT_CTRLA) Communication Mode Mask */
#define SERCOM_USART_INT_CTRLA_CMODE(value)   (SERCOM_USART_INT_CTRLA_CMODE_Msk & ((value) << SERCOM_USART_INT_CTRLA_CMODE_Pos))
#define   SERCOM_USART_INT_CTRLA_CMODE_ASYNC_Val _U_(0x0)                                             /**< (SERCOM_USART_INT_CTRLA) Asynchronous Communication  */
#define   SERCOM_USART_INT_CTRLA_CMODE_SYNC_Val _U_(0x1)                                             /**< (SERCOM_USART_INT_CTRLA) Synchronous Communication  */
#define SERCOM_USART_INT_CTRLA_CMODE_ASYNC    (SERCOM_USART_INT_CTRLA_CMODE_ASYNC_Val << SERCOM_USART_INT_CTRLA_CMODE_Pos) /**< (SERCOM_USART_INT_CTRLA) Asynchronous Communication Position  */
#define SERCOM_USART_INT_CTRLA_CMODE_SYNC     (SERCOM_USART_INT_CTRLA_CMODE_SYNC_Val << SERCOM_USART_INT_CTRLA_CMODE_Pos) /**< (SERCOM_USART_INT_CTRLA) Synchronous Communication Position  */
#define SERCOM_USART_INT_CTRLA_CPOL_Pos       _U_(29)                                              /**< (SERCOM_USART_INT_CTRLA) Clock Polarity Position */
#define SERCOM_USART_INT_CTRLA_CPOL_Msk       (_U_(0x1) << SERCOM_USART_INT_CTRLA_CPOL_Pos)        /**< (SERCOM_USART_INT_CTRLA) Clock Polarity Mask */
#define SERCOM_USART_INT_CTRLA_CPOL(value)    (SERCOM_USART_INT_CTRLA_CPOL_Msk & ((value) << SERCOM_USART_INT_CTRLA_CPOL_Pos))
#define   SERCOM_USART_INT_CTRLA_CPOL_IDLE_LOW_Val _U_(0x0)                                             /**< (SERCOM_USART_INT_CTRLA) TxD Change:- Rising XCK edge, RxD Sample:- Falling XCK edge  */
#define   SERCOM_USART_INT_CTRLA_CPOL_IDLE_HIGH_Val _U_(0x1)                                             /**< (SERCOM_USART_INT_CTRLA) TxD Change:- Falling XCK edge, RxD Sample:- Rising XCK edge  */
#define SERCOM_USART_INT_CTRLA_CPOL_IDLE_LOW  (SERCOM_USART_INT_CTRLA_CPOL_IDLE_LOW_Val << SERCOM_USART_INT_CTRLA_CPOL_Pos) /**< (SERCOM_USART_INT_CTRLA) TxD Change:- Rising XCK edge, RxD Sample:- Falling XCK edge Position  */
#define SERCOM_USART_INT_CTRLA_CPOL_IDLE_HIGH (SERCOM_USART_INT_CTRLA_CPOL_IDLE_HIGH_Val << SERCOM_USART_INT_CTRLA_CPOL_Pos) /**< (SERCOM_USART_INT_CTRLA) TxD Change:- Falling XCK edge, RxD Sample:- Rising XCK edge Position  */
#define SERCOM_USART_INT_CTRLA_DORD_Pos       _U_(30)                                              /**< (SERCOM_USART_INT_CTRLA) Data Order Position */
#define SERCOM_USART_INT_CTRLA_DORD_Msk       (_U_(0x1) << SERCOM_USART_INT_CTRLA_DORD_Pos)        /**< (SERCOM_USART_INT_CTRLA) Data Order Mask */
#define SERCOM_USART_INT_CTRLA_DORD(value)    (SERCOM_USART_INT_CTRLA_DORD_Msk & ((value) << SERCOM_USART_INT_CTRLA_DORD_Pos))
#define   SERCOM_USART_INT_CTRLA_DORD_MSB_Val _U_(0x0)                                             /**< (SERCOM_USART_INT_CTRLA) MSB is transmitted first  */
#define   SERCOM_USART_INT_CTRLA_DORD_LSB_Val _U_(0x1)                                             /**< (SERCOM_USART_INT_CTRLA) LSB is transmitted first  */
#define SERCOM_USART_INT_CTRLA_DORD_MSB       (SERCOM_USART_INT_CTRLA_DORD_MSB_Val << SERCOM_USART_INT_CTRLA_DORD_Pos) /**< (SERCOM_USART_INT_CTRLA) MSB is transmitted first Position  */
#define SERCOM_USART_INT_CTRLA_DORD_LSB       (SERCOM_USART_INT_CTRLA_DORD_LSB_Val << SERCOM_USART_INT_CTRLA_DORD_Pos) /**< (SERCOM_USART_INT_CTRLA) LSB is transmitted first Position  */
#define SERCOM_USART_INT_CTRLA_Msk            _U_(0x7FF3E79F)                                      /**< (SERCOM_USART_INT_CTRLA) Register Mask  */


/* -------- SERCOM_I2CM_CTRLB : (SERCOM Offset: 0x04) (R/W 32) I2CM Control B -------- */
#define SERCOM_I2CM_CTRLB_RESETVALUE          _U_(0x00)                                            /**<  (SERCOM_I2CM_CTRLB) I2CM Control B  Reset Value */

#define SERCOM_I2CM_CTRLB_SMEN_Pos            _U_(8)                                               /**< (SERCOM_I2CM_CTRLB) Smart Mode Enable Position */
#define SERCOM_I2CM_CTRLB_SMEN_Msk            (_U_(0x1) << SERCOM_I2CM_CTRLB_SMEN_Pos)             /**< (SERCOM_I2CM_CTRLB) Smart Mode Enable Mask */
#define SERCOM_I2CM_CTRLB_SMEN(value)         (SERCOM_I2CM_CTRLB_SMEN_Msk & ((value) << SERCOM_I2CM_CTRLB_SMEN_Pos))
#define SERCOM_I2CM_CTRLB_QCEN_Pos            _U_(9)                                               /**< (SERCOM_I2CM_CTRLB) Quick Command Enable Position */
#define SERCOM_I2CM_CTRLB_QCEN_Msk            (_U_(0x1) << SERCOM_I2CM_CTRLB_QCEN_Pos)             /**< (SERCOM_I2CM_CTRLB) Quick Command Enable Mask */
#define SERCOM_I2CM_CTRLB_QCEN(value)         (SERCOM_I2CM_CTRLB_QCEN_Msk & ((value) << SERCOM_I2CM_CTRLB_QCEN_Pos))
#define SERCOM_I2CM_CTRLB_CMD_Pos             _U_(16)                                              /**< (SERCOM_I2CM_CTRLB) Command Position */
#define SERCOM_I2CM_CTRLB_CMD_Msk             (_U_(0x3) << SERCOM_I2CM_CTRLB_CMD_Pos)              /**< (SERCOM_I2CM_CTRLB) Command Mask */
#define SERCOM_I2CM_CTRLB_CMD(value)          (SERCOM_I2CM_CTRLB_CMD_Msk & ((value) << SERCOM_I2CM_CTRLB_CMD_Pos))
#define SERCOM_I2CM_CTRLB_ACKACT_Pos          _U_(18)                                              /**< (SERCOM_I2CM_CTRLB) Acknowledge Action Position */
#define SERCOM_I2CM_CTRLB_ACKACT_Msk          (_U_(0x1) << SERCOM_I2CM_CTRLB_ACKACT_Pos)           /**< (SERCOM_I2CM_CTRLB) Acknowledge Action Mask */
#define SERCOM_I2CM_CTRLB_ACKACT(value)       (SERCOM_I2CM_CTRLB_ACKACT_Msk & ((value) << SERCOM_I2CM_CTRLB_ACKACT_Pos))
#define SERCOM_I2CM_CTRLB_Msk                 _U_(0x00070300)                                      /**< (SERCOM_I2CM_CTRLB) Register Mask  */


/* -------- SERCOM_I2CS_CTRLB : (SERCOM Offset: 0x04) (R/W 32) I2CS Control B -------- */
#define SERCOM_I2CS_CTRLB_RESETVALUE          _U_(0x00)                                            /**<  (SERCOM_I2CS_CTRLB) I2CS Control B  Reset Value */

#define SERCOM_I2CS_CTRLB_SMEN_Pos            _U_(8)                                               /**< (SERCOM_I2CS_CTRLB) Smart Mode Enable Position */
#define SERCOM_I2CS_CTRLB_SMEN_Msk            (_U_(0x1) << SERCOM_I2CS_CTRLB_SMEN_Pos)             /**< (SERCOM_I2CS_CTRLB) Smart Mode Enable Mask */
#define SERCOM_I2CS_CTRLB_SMEN(value)         (SERCOM_I2CS_CTRLB_SMEN_Msk & ((value) << SERCOM_I2CS_CTRLB_SMEN_Pos))
#define SERCOM_I2CS_CTRLB_GCMD_Pos            _U_(9)                                               /**< (SERCOM_I2CS_CTRLB) PMBus Group Command Position */
#define SERCOM_I2CS_CTRLB_GCMD_Msk            (_U_(0x1) << SERCOM_I2CS_CTRLB_GCMD_Pos)             /**< (SERCOM_I2CS_CTRLB) PMBus Group Command Mask */
#define SERCOM_I2CS_CTRLB_GCMD(value)         (SERCOM_I2CS_CTRLB_GCMD_Msk & ((value) << SERCOM_I2CS_CTRLB_GCMD_Pos))
#define SERCOM_I2CS_CTRLB_AACKEN_Pos          _U_(10)                                              /**< (SERCOM_I2CS_CTRLB) Automatic Address Acknowledge Position */
#define SERCOM_I2CS_CTRLB_AACKEN_Msk          (_U_(0x1) << SERCOM_I2CS_CTRLB_AACKEN_Pos)           /**< (SERCOM_I2CS_CTRLB) Automatic Address Acknowledge Mask */
#define SERCOM_I2CS_CTRLB_AACKEN(value)       (SERCOM_I2CS_CTRLB_AACKEN_Msk & ((value) << SERCOM_I2CS_CTRLB_AACKEN_Pos))
#define SERCOM_I2CS_CTRLB_AMODE_Pos           _U_(14)                                              /**< (SERCOM_I2CS_CTRLB) Address Mode Position */
#define SERCOM_I2CS_CTRLB_AMODE_Msk           (_U_(0x3) << SERCOM_I2CS_CTRLB_AMODE_Pos)            /**< (SERCOM_I2CS_CTRLB) Address Mode Mask */
#define SERCOM_I2CS_CTRLB_AMODE(value)        (SERCOM_I2CS_CTRLB_AMODE_Msk & ((value) << SERCOM_I2CS_CTRLB_AMODE_Pos))
#define SERCOM_I2CS_CTRLB_CMD_Pos             _U_(16)                                              /**< (SERCOM_I2CS_CTRLB) Command Position */
#define SERCOM_I2CS_CTRLB_CMD_Msk             (_U_(0x3) << SERCOM_I2CS_CTRLB_CMD_Pos)              /**< (SERCOM_I2CS_CTRLB) Command Mask */
#define SERCOM_I2CS_CTRLB_CMD(value)          (SERCOM_I2CS_CTRLB_CMD_Msk & ((value) << SERCOM_I2CS_CTRLB_CMD_Pos))
#define SERCOM_I2CS_CTRLB_ACKACT_Pos          _U_(18)                                              /**< (SERCOM_I2CS_CTRLB) Acknowledge Action Position */
#define SERCOM_I2CS_CTRLB_ACKACT_Msk          (_U_(0x1) << SERCOM_I2CS_CTRLB_ACKACT_Pos)           /**< (SERCOM_I2CS_CTRLB) Acknowledge Action Mask */
#define SERCOM_I2CS_CTRLB_ACKACT(value)       (SERCOM_I2CS_CTRLB_ACKACT_Msk & ((value) << SERCOM_I2CS_CTRLB_ACKACT_Pos))
#define SERCOM_I2CS_CTRLB_Msk                 _U_(0x0007C700)                                      /**< (SERCOM_I2CS_CTRLB) Register Mask  */


/* -------- SERCOM_SPIM_CTRLB : (SERCOM Offset: 0x04) (R/W 32) SPIM Control B -------- */
#define SERCOM_SPIM_CTRLB_RESETVALUE          _U_(0x00)                                            /**<  (SERCOM_SPIM_CTRLB) SPIM Control B  Reset Value */

#define SERCOM_SPIM_CTRLB_CHSIZE_Pos          _U_(0)                                               /**< (SERCOM_SPIM_CTRLB) Character Size Position */
#define SERCOM_SPIM_CTRLB_CHSIZE_Msk          (_U_(0x7) << SERCOM_SPIM_CTRLB_CHSIZE_Pos)           /**< (SERCOM_SPIM_CTRLB) Character Size Mask */
#define SERCOM_SPIM_CTRLB_CHSIZE(value)       (SERCOM_SPIM_CTRLB_CHSIZE_Msk & ((value) << SERCOM_SPIM_CTRLB_CHSIZE_Pos))
#define   SERCOM_SPIM_CTRLB_CHSIZE_8_BIT_Val  _U_(0x0)                                             /**< (SERCOM_SPIM_CTRLB) 8 bits  */
#define   SERCOM_SPIM_CTRLB_CHSIZE_9_BIT_Val  _U_(0x1)                                             /**< (SERCOM_SPIM_CTRLB) 9 bits  */
#define SERCOM_SPIM_CTRLB_CHSIZE_8_BIT        (SERCOM_SPIM_CTRLB_CHSIZE_8_BIT_Val << SERCOM_SPIM_CTRLB_CHSIZE_Pos) /**< (SERCOM_SPIM_CTRLB) 8 bits Position  */
#define SERCOM_SPIM_CTRLB_CHSIZE_9_BIT        (SERCOM_SPIM_CTRLB_CHSIZE_9_BIT_Val << SERCOM_SPIM_CTRLB_CHSIZE_Pos) /**< (SERCOM_SPIM_CTRLB) 9 bits Position  */
#define SERCOM_SPIM_CTRLB_PLOADEN_Pos         _U_(6)                                               /**< (SERCOM_SPIM_CTRLB) Data Preload Enable Position */
#define SERCOM_SPIM_CTRLB_PLOADEN_Msk         (_U_(0x1) << SERCOM_SPIM_CTRLB_PLOADEN_Pos)          /**< (SERCOM_SPIM_CTRLB) Data Preload Enable Mask */
#define SERCOM_SPIM_CTRLB_PLOADEN(value)      (SERCOM_SPIM_CTRLB_PLOADEN_Msk & ((value) << SERCOM_SPIM_CTRLB_PLOADEN_Pos))
#define SERCOM_SPIM_CTRLB_SSDE_Pos            _U_(9)                                               /**< (SERCOM_SPIM_CTRLB) Slave Select Low Detect Enable Position */
#define SERCOM_SPIM_CTRLB_SSDE_Msk            (_U_(0x1) << SERCOM_SPIM_CTRLB_SSDE_Pos)             /**< (SERCOM_SPIM_CTRLB) Slave Select Low Detect Enable Mask */
#define SERCOM_SPIM_CTRLB_SSDE(value)         (SERCOM_SPIM_CTRLB_SSDE_Msk & ((value) << SERCOM_SPIM_CTRLB_SSDE_Pos))
#define SERCOM_SPIM_CTRLB_MSSEN_Pos           _U_(13)                                              /**< (SERCOM_SPIM_CTRLB) Master Slave Select Enable Position */
#define SERCOM_SPIM_CTRLB_MSSEN_Msk           (_U_(0x1) << SERCOM_SPIM_CTRLB_MSSEN_Pos)            /**< (SERCOM_SPIM_CTRLB) Master Slave Select Enable Mask */
#define SERCOM_SPIM_CTRLB_MSSEN(value)        (SERCOM_SPIM_CTRLB_MSSEN_Msk & ((value) << SERCOM_SPIM_CTRLB_MSSEN_Pos))
#define SERCOM_SPIM_CTRLB_AMODE_Pos           _U_(14)                                              /**< (SERCOM_SPIM_CTRLB) Address Mode Position */
#define SERCOM_SPIM_CTRLB_AMODE_Msk           (_U_(0x3) << SERCOM_SPIM_CTRLB_AMODE_Pos)            /**< (SERCOM_SPIM_CTRLB) Address Mode Mask */
#define SERCOM_SPIM_CTRLB_AMODE(value)        (SERCOM_SPIM_CTRLB_AMODE_Msk & ((value) << SERCOM_SPIM_CTRLB_AMODE_Pos))
#define   SERCOM_SPIM_CTRLB_AMODE_MASK_Val    _U_(0x0)                                             /**< (SERCOM_SPIM_CTRLB) SPI Address mask   */
#define   SERCOM_SPIM_CTRLB_AMODE_2_ADDRESSES_Val _U_(0x1)                                             /**< (SERCOM_SPIM_CTRLB) Two unique Addressess  */
#define   SERCOM_SPIM_CTRLB_AMODE_RANGE_Val   _U_(0x2)                                             /**< (SERCOM_SPIM_CTRLB) Address Range  */
#define SERCOM_SPIM_CTRLB_AMODE_MASK          (SERCOM_SPIM_CTRLB_AMODE_MASK_Val << SERCOM_SPIM_CTRLB_AMODE_Pos) /**< (SERCOM_SPIM_CTRLB) SPI Address mask  Position  */
#define SERCOM_SPIM_CTRLB_AMODE_2_ADDRESSES   (SERCOM_SPIM_CTRLB_AMODE_2_ADDRESSES_Val << SERCOM_SPIM_CTRLB_AMODE_Pos) /**< (SERCOM_SPIM_CTRLB) Two unique Addressess Position  */
#define SERCOM_SPIM_CTRLB_AMODE_RANGE         (SERCOM_SPIM_CTRLB_AMODE_RANGE_Val << SERCOM_SPIM_CTRLB_AMODE_Pos) /**< (SERCOM_SPIM_CTRLB) Address Range Position  */
#define SERCOM_SPIM_CTRLB_RXEN_Pos            _U_(17)                                              /**< (SERCOM_SPIM_CTRLB) Receiver Enable Position */
#define SERCOM_SPIM_CTRLB_RXEN_Msk            (_U_(0x1) << SERCOM_SPIM_CTRLB_RXEN_Pos)             /**< (SERCOM_SPIM_CTRLB) Receiver Enable Mask */
#define SERCOM_SPIM_CTRLB_RXEN(value)         (SERCOM_SPIM_CTRLB_RXEN_Msk & ((value) << SERCOM_SPIM_CTRLB_RXEN_Pos))
#define SERCOM_SPIM_CTRLB_Msk                 _U_(0x0002E247)                                      /**< (SERCOM_SPIM_CTRLB) Register Mask  */


/* -------- SERCOM_SPIS_CTRLB : (SERCOM Offset: 0x04) (R/W 32) SPIS Control B -------- */
#define SERCOM_SPIS_CTRLB_RESETVALUE          _U_(0x00)                                            /**<  (SERCOM_SPIS_CTRLB) SPIS Control B  Reset Value */

#define SERCOM_SPIS_CTRLB_CHSIZE_Pos          _U_(0)                                               /**< (SERCOM_SPIS_CTRLB) Character Size Position */
#define SERCOM_SPIS_CTRLB_CHSIZE_Msk          (_U_(0x7) << SERCOM_SPIS_CTRLB_CHSIZE_Pos)           /**< (SERCOM_SPIS_CTRLB) Character Size Mask */
#define SERCOM_SPIS_CTRLB_CHSIZE(value)       (SERCOM_SPIS_CTRLB_CHSIZE_Msk & ((value) << SERCOM_SPIS_CTRLB_CHSIZE_Pos))
#define   SERCOM_SPIS_CTRLB_CHSIZE_8_BIT_Val  _U_(0x0)                                             /**< (SERCOM_SPIS_CTRLB) 8 bits  */
#define   SERCOM_SPIS_CTRLB_CHSIZE_9_BIT_Val  _U_(0x1)                                             /**< (SERCOM_SPIS_CTRLB) 9 bits  */
#define SERCOM_SPIS_CTRLB_CHSIZE_8_BIT        (SERCOM_SPIS_CTRLB_CHSIZE_8_BIT_Val << SERCOM_SPIS_CTRLB_CHSIZE_Pos) /**< (SERCOM_SPIS_CTRLB) 8 bits Position  */
#define SERCOM_SPIS_CTRLB_CHSIZE_9_BIT        (SERCOM_SPIS_CTRLB_CHSIZE_9_BIT_Val << SERCOM_SPIS_CTRLB_CHSIZE_Pos) /**< (SERCOM_SPIS_CTRLB) 9 bits Position  */
#define SERCOM_SPIS_CTRLB_PLOADEN_Pos         _U_(6)                                               /**< (SERCOM_SPIS_CTRLB) Data Preload Enable Position */
#define SERCOM_SPIS_CTRLB_PLOADEN_Msk         (_U_(0x1) << SERCOM_SPIS_CTRLB_PLOADEN_Pos)          /**< (SERCOM_SPIS_CTRLB) Data Preload Enable Mask */
#define SERCOM_SPIS_CTRLB_PLOADEN(value)      (SERCOM_SPIS_CTRLB_PLOADEN_Msk & ((value) << SERCOM_SPIS_CTRLB_PLOADEN_Pos))
#define SERCOM_SPIS_CTRLB_SSDE_Pos            _U_(9)                                               /**< (SERCOM_SPIS_CTRLB) Slave Select Low Detect Enable Position */
#define SERCOM_SPIS_CTRLB_SSDE_Msk            (_U_(0x1) << SERCOM_SPIS_CTRLB_SSDE_Pos)             /**< (SERCOM_SPIS_CTRLB) Slave Select Low Detect Enable Mask */
#define SERCOM_SPIS_CTRLB_SSDE(value)         (SERCOM_SPIS_CTRLB_SSDE_Msk & ((value) << SERCOM_SPIS_CTRLB_SSDE_Pos))
#define SERCOM_SPIS_CTRLB_MSSEN_Pos           _U_(13)                                              /**< (SERCOM_SPIS_CTRLB) Master Slave Select Enable Position */
#define SERCOM_SPIS_CTRLB_MSSEN_Msk           (_U_(0x1) << SERCOM_SPIS_CTRLB_MSSEN_Pos)            /**< (SERCOM_SPIS_CTRLB) Master Slave Select Enable Mask */
#define SERCOM_SPIS_CTRLB_MSSEN(value)        (SERCOM_SPIS_CTRLB_MSSEN_Msk & ((value) << SERCOM_SPIS_CTRLB_MSSEN_Pos))
#define SERCOM_SPIS_CTRLB_AMODE_Pos           _U_(14)                                              /**< (SERCOM_SPIS_CTRLB) Address Mode Position */
#define SERCOM_SPIS_CTRLB_AMODE_Msk           (_U_(0x3) << SERCOM_SPIS_CTRLB_AMODE_Pos)            /**< (SERCOM_SPIS_CTRLB) Address Mode Mask */
#define SERCOM_SPIS_CTRLB_AMODE(value)        (SERCOM_SPIS_CTRLB_AMODE_Msk & ((value) << SERCOM_SPIS_CTRLB_AMODE_Pos))
#define   SERCOM_SPIS_CTRLB_AMODE_MASK_Val    _U_(0x0)                                             /**< (SERCOM_SPIS_CTRLB) SPI Address mask   */
#define   SERCOM_SPIS_CTRLB_AMODE_2_ADDRESSES_Val _U_(0x1)                                             /**< (SERCOM_SPIS_CTRLB) Two unique Addressess  */
#define   SERCOM_SPIS_CTRLB_AMODE_RANGE_Val   _U_(0x2)                                             /**< (SERCOM_SPIS_CTRLB) Address Range  */
#define SERCOM_SPIS_CTRLB_AMODE_MASK          (SERCOM_SPIS_CTRLB_AMODE_MASK_Val << SERCOM_SPIS_CTRLB_AMODE_Pos) /**< (SERCOM_SPIS_CTRLB) SPI Address mask  Position  */
#define SERCOM_SPIS_CTRLB_AMODE_2_ADDRESSES   (SERCOM_SPIS_CTRLB_AMODE_2_ADDRESSES_Val << SERCOM_SPIS_CTRLB_AMODE_Pos) /**< (SERCOM_SPIS_CTRLB) Two unique Addressess Position  */
#define SERCOM_SPIS_CTRLB_AMODE_RANGE         (SERCOM_SPIS_CTRLB_AMODE_RANGE_Val << SERCOM_SPIS_CTRLB_AMODE_Pos) /**< (SERCOM_SPIS_CTRLB) Address Range Position  */
#define SERCOM_SPIS_CTRLB_RXEN_Pos            _U_(17)                                              /**< (SERCOM_SPIS_CTRLB) Receiver Enable Position */
#define SERCOM_SPIS_CTRLB_RXEN_Msk            (_U_(0x1) << SERCOM_SPIS_CTRLB_RXEN_Pos)             /**< (SERCOM_SPIS_CTRLB) Receiver Enable Mask */
#define SERCOM_SPIS_CTRLB_RXEN(value)         (SERCOM_SPIS_CTRLB_RXEN_Msk & ((value) << SERCOM_SPIS_CTRLB_RXEN_Pos))
#define SERCOM_SPIS_CTRLB_Msk                 _U_(0x0002E247)                                      /**< (SERCOM_SPIS_CTRLB) Register Mask  */


/* -------- SERCOM_USART_EXT_CTRLB : (SERCOM Offset: 0x04) (R/W 32) USART_EXT Control B -------- */
#define SERCOM_USART_EXT_CTRLB_RESETVALUE     _U_(0x00)                                            /**<  (SERCOM_USART_EXT_CTRLB) USART_EXT Control B  Reset Value */

#define SERCOM_USART_EXT_CTRLB_CHSIZE_Pos     _U_(0)                                               /**< (SERCOM_USART_EXT_CTRLB) Character Size Position */
#define SERCOM_USART_EXT_CTRLB_CHSIZE_Msk     (_U_(0x7) << SERCOM_USART_EXT_CTRLB_CHSIZE_Pos)      /**< (SERCOM_USART_EXT_CTRLB) Character Size Mask */
#define SERCOM_USART_EXT_CTRLB_CHSIZE(value)  (SERCOM_USART_EXT_CTRLB_CHSIZE_Msk & ((value) << SERCOM_USART_EXT_CTRLB_CHSIZE_Pos))
#define   SERCOM_USART_EXT_CTRLB_CHSIZE_8_BIT_Val _U_(0x0)                                             /**< (SERCOM_USART_EXT_CTRLB) 8 Bits  */
#define   SERCOM_USART_EXT_CTRLB_CHSIZE_9_BIT_Val _U_(0x1)                                             /**< (SERCOM_USART_EXT_CTRLB) 9 Bits  */
#define   SERCOM_USART_EXT_CTRLB_CHSIZE_5_BIT_Val _U_(0x5)                                             /**< (SERCOM_USART_EXT_CTRLB) 5 Bits  */
#define   SERCOM_USART_EXT_CTRLB_CHSIZE_6_BIT_Val _U_(0x6)                                             /**< (SERCOM_USART_EXT_CTRLB) 6 Bits  */
#define   SERCOM_USART_EXT_CTRLB_CHSIZE_7_BIT_Val _U_(0x7)                                             /**< (SERCOM_USART_EXT_CTRLB) 7 Bits  */
#define SERCOM_USART_EXT_CTRLB_CHSIZE_8_BIT   (SERCOM_USART_EXT_CTRLB_CHSIZE_8_BIT_Val << SERCOM_USART_EXT_CTRLB_CHSIZE_Pos) /**< (SERCOM_USART_EXT_CTRLB) 8 Bits Position  */
#define SERCOM_USART_EXT_CTRLB_CHSIZE_9_BIT   (SERCOM_USART_EXT_CTRLB_CHSIZE_9_BIT_Val << SERCOM_USART_EXT_CTRLB_CHSIZE_Pos) /**< (SERCOM_USART_EXT_CTRLB) 9 Bits Position  */
#define SERCOM_USART_EXT_CTRLB_CHSIZE_5_BIT   (SERCOM_USART_EXT_CTRLB_CHSIZE_5_BIT_Val << SERCOM_USART_EXT_CTRLB_CHSIZE_Pos) /**< (SERCOM_USART_EXT_CTRLB) 5 Bits Position  */
#define SERCOM_USART_EXT_CTRLB_CHSIZE_6_BIT   (SERCOM_USART_EXT_CTRLB_CHSIZE_6_BIT_Val << SERCOM_USART_EXT_CTRLB_CHSIZE_Pos) /**< (SERCOM_USART_EXT_CTRLB) 6 Bits Position  */
#define SERCOM_USART_EXT_CTRLB_CHSIZE_7_BIT   (SERCOM_USART_EXT_CTRLB_CHSIZE_7_BIT_Val << SERCOM_USART_EXT_CTRLB_CHSIZE_Pos) /**< (SERCOM_USART_EXT_CTRLB) 7 Bits Position  */
#define SERCOM_USART_EXT_CTRLB_SBMODE_Pos     _U_(6)                                               /**< (SERCOM_USART_EXT_CTRLB) Stop Bit Mode Position */
#define SERCOM_USART_EXT_CTRLB_SBMODE_Msk     (_U_(0x1) << SERCOM_USART_EXT_CTRLB_SBMODE_Pos)      /**< (SERCOM_USART_EXT_CTRLB) Stop Bit Mode Mask */
#define SERCOM_USART_EXT_CTRLB_SBMODE(value)  (SERCOM_USART_EXT_CTRLB_SBMODE_Msk & ((value) << SERCOM_USART_EXT_CTRLB_SBMODE_Pos))
#define   SERCOM_USART_EXT_CTRLB_SBMODE_1_BIT_Val _U_(0x0)                                             /**< (SERCOM_USART_EXT_CTRLB) One Stop Bit  */
#define   SERCOM_USART_EXT_CTRLB_SBMODE_2_BIT_Val _U_(0x1)                                             /**< (SERCOM_USART_EXT_CTRLB) Two Stop Bits  */
#define SERCOM_USART_EXT_CTRLB_SBMODE_1_BIT   (SERCOM_USART_EXT_CTRLB_SBMODE_1_BIT_Val << SERCOM_USART_EXT_CTRLB_SBMODE_Pos) /**< (SERCOM_USART_EXT_CTRLB) One Stop Bit Position  */
#define SERCOM_USART_EXT_CTRLB_SBMODE_2_BIT   (SERCOM_USART_EXT_CTRLB_SBMODE_2_BIT_Val << SERCOM_USART_EXT_CTRLB_SBMODE_Pos) /**< (SERCOM_USART_EXT_CTRLB) Two Stop Bits Position  */
#define SERCOM_USART_EXT_CTRLB_COLDEN_Pos     _U_(8)                                               /**< (SERCOM_USART_EXT_CTRLB) Collision Detection Enable Position */
#define SERCOM_USART_EXT_CTRLB_COLDEN_Msk     (_U_(0x1) << SERCOM_USART_EXT_CTRLB_COLDEN_Pos)      /**< (SERCOM_USART_EXT_CTRLB) Collision Detection Enable Mask */
#define SERCOM_USART_EXT_CTRLB_COLDEN(value)  (SERCOM_USART_EXT_CTRLB_COLDEN_Msk & ((value) << SERCOM_USART_EXT_CTRLB_COLDEN_Pos))
#define SERCOM_USART_EXT_CTRLB_SFDE_Pos       _U_(9)                                               /**< (SERCOM_USART_EXT_CTRLB) Start of Frame Detection Enable Position */
#define SERCOM_USART_EXT_CTRLB_SFDE_Msk       (_U_(0x1) << SERCOM_USART_EXT_CTRLB_SFDE_Pos)        /**< (SERCOM_USART_EXT_CTRLB) Start of Frame Detection Enable Mask */
#define SERCOM_USART_EXT_CTRLB_SFDE(value)    (SERCOM_USART_EXT_CTRLB_SFDE_Msk & ((value) << SERCOM_USART_EXT_CTRLB_SFDE_Pos))
#define SERCOM_USART_EXT_CTRLB_ENC_Pos        _U_(10)                                              /**< (SERCOM_USART_EXT_CTRLB) Encoding Format Position */
#define SERCOM_USART_EXT_CTRLB_ENC_Msk        (_U_(0x1) << SERCOM_USART_EXT_CTRLB_ENC_Pos)         /**< (SERCOM_USART_EXT_CTRLB) Encoding Format Mask */
#define SERCOM_USART_EXT_CTRLB_ENC(value)     (SERCOM_USART_EXT_CTRLB_ENC_Msk & ((value) << SERCOM_USART_EXT_CTRLB_ENC_Pos))
#define SERCOM_USART_EXT_CTRLB_PMODE_Pos      _U_(13)                                              /**< (SERCOM_USART_EXT_CTRLB) Parity Mode Position */
#define SERCOM_USART_EXT_CTRLB_PMODE_Msk      (_U_(0x1) << SERCOM_USART_EXT_CTRLB_PMODE_Pos)       /**< (SERCOM_USART_EXT_CTRLB) Parity Mode Mask */
#define SERCOM_USART_EXT_CTRLB_PMODE(value)   (SERCOM_USART_EXT_CTRLB_PMODE_Msk & ((value) << SERCOM_USART_EXT_CTRLB_PMODE_Pos))
#define   SERCOM_USART_EXT_CTRLB_PMODE_EVEN_Val _U_(0x0)                                             /**< (SERCOM_USART_EXT_CTRLB) Even Parity  */
#define   SERCOM_USART_EXT_CTRLB_PMODE_ODD_Val _U_(0x1)                                             /**< (SERCOM_USART_EXT_CTRLB) Odd Parity  */
#define SERCOM_USART_EXT_CTRLB_PMODE_EVEN     (SERCOM_USART_EXT_CTRLB_PMODE_EVEN_Val << SERCOM_USART_EXT_CTRLB_PMODE_Pos) /**< (SERCOM_USART_EXT_CTRLB) Even Parity Position  */
#define SERCOM_USART_EXT_CTRLB_PMODE_ODD      (SERCOM_USART_EXT_CTRLB_PMODE_ODD_Val << SERCOM_USART_EXT_CTRLB_PMODE_Pos) /**< (SERCOM_USART_EXT_CTRLB) Odd Parity Position  */
#define SERCOM_USART_EXT_CTRLB_TXEN_Pos       _U_(16)                                              /**< (SERCOM_USART_EXT_CTRLB) Transmitter Enable Position */
#define SERCOM_USART_EXT_CTRLB_TXEN_Msk       (_U_(0x1) << SERCOM_USART_EXT_CTRLB_TXEN_Pos)        /**< (SERCOM_USART_EXT_CTRLB) Transmitter Enable Mask */
#define SERCOM_USART_EXT_CTRLB_TXEN(value)    (SERCOM_USART_EXT_CTRLB_TXEN_Msk & ((value) << SERCOM_USART_EXT_CTRLB_TXEN_Pos))
#define SERCOM_USART_EXT_CTRLB_RXEN_Pos       _U_(17)                                              /**< (SERCOM_USART_EXT_CTRLB) Receiver Enable Position */
#define SERCOM_USART_EXT_CTRLB_RXEN_Msk       (_U_(0x1) << SERCOM_USART_EXT_CTRLB_RXEN_Pos)        /**< (SERCOM_USART_EXT_CTRLB) Receiver Enable Mask */
#define SERCOM_USART_EXT_CTRLB_RXEN(value)    (SERCOM_USART_EXT_CTRLB_RXEN_Msk & ((value) << SERCOM_USART_EXT_CTRLB_RXEN_Pos))
#define SERCOM_USART_EXT_CTRLB_LINCMD_Pos     _U_(24)                                              /**< (SERCOM_USART_EXT_CTRLB) LIN Command Position */
#define SERCOM_USART_EXT_CTRLB_LINCMD_Msk     (_U_(0x3) << SERCOM_USART_EXT_CTRLB_LINCMD_Pos)      /**< (SERCOM_USART_EXT_CTRLB) LIN Command Mask */
#define SERCOM_USART_EXT_CTRLB_LINCMD(value)  (SERCOM_USART_EXT_CTRLB_LINCMD_Msk & ((value) << SERCOM_USART_EXT_CTRLB_LINCMD_Pos))
#define SERCOM_USART_EXT_CTRLB_Msk            _U_(0x03032747)                                      /**< (SERCOM_USART_EXT_CTRLB) Register Mask  */


/* -------- SERCOM_USART_INT_CTRLB : (SERCOM Offset: 0x04) (R/W 32) USART_INT Control B -------- */
#define SERCOM_USART_INT_CTRLB_RESETVALUE     _U_(0x00)                                            /**<  (SERCOM_USART_INT_CTRLB) USART_INT Control B  Reset Value */

#define SERCOM_USART_INT_CTRLB_CHSIZE_Pos     _U_(0)                                               /**< (SERCOM_USART_INT_CTRLB) Character Size Position */
#define SERCOM_USART_INT_CTRLB_CHSIZE_Msk     (_U_(0x7) << SERCOM_USART_INT_CTRLB_CHSIZE_Pos)      /**< (SERCOM_USART_INT_CTRLB) Character Size Mask */
#define SERCOM_USART_INT_CTRLB_CHSIZE(value)  (SERCOM_USART_INT_CTRLB_CHSIZE_Msk & ((value) << SERCOM_USART_INT_CTRLB_CHSIZE_Pos))
#define   SERCOM_USART_INT_CTRLB_CHSIZE_8_BIT_Val _U_(0x0)                                             /**< (SERCOM_USART_INT_CTRLB) 8 Bits  */
#define   SERCOM_USART_INT_CTRLB_CHSIZE_9_BIT_Val _U_(0x1)                                             /**< (SERCOM_USART_INT_CTRLB) 9 Bits  */
#define   SERCOM_USART_INT_CTRLB_CHSIZE_5_BIT_Val _U_(0x5)                                             /**< (SERCOM_USART_INT_CTRLB) 5 Bits  */
#define   SERCOM_USART_INT_CTRLB_CHSIZE_6_BIT_Val _U_(0x6)                                             /**< (SERCOM_USART_INT_CTRLB) 6 Bits  */
#define   SERCOM_USART_INT_CTRLB_CHSIZE_7_BIT_Val _U_(0x7)                                             /**< (SERCOM_USART_INT_CTRLB) 7 Bits  */
#define SERCOM_USART_INT_CTRLB_CHSIZE_8_BIT   (SERCOM_USART_INT_CTRLB_CHSIZE_8_BIT_Val << SERCOM_USART_INT_CTRLB_CHSIZE_Pos) /**< (SERCOM_USART_INT_CTRLB) 8 Bits Position  */
#define SERCOM_USART_INT_CTRLB_CHSIZE_9_BIT   (SERCOM_USART_INT_CTRLB_CHSIZE_9_BIT_Val << SERCOM_USART_INT_CTRLB_CHSIZE_Pos) /**< (SERCOM_USART_INT_CTRLB) 9 Bits Position  */
#define SERCOM_USART_INT_CTRLB_CHSIZE_5_BIT   (SERCOM_USART_INT_CTRLB_CHSIZE_5_BIT_Val << SERCOM_USART_INT_CTRLB_CHSIZE_Pos) /**< (SERCOM_USART_INT_CTRLB) 5 Bits Position  */
#define SERCOM_USART_INT_CTRLB_CHSIZE_6_BIT   (SERCOM_USART_INT_CTRLB_CHSIZE_6_BIT_Val << SERCOM_USART_INT_CTRLB_CHSIZE_Pos) /**< (SERCOM_USART_INT_CTRLB) 6 Bits Position  */
#define SERCOM_USART_INT_CTRLB_CHSIZE_7_BIT   (SERCOM_USART_INT_CTRLB_CHSIZE_7_BIT_Val << SERCOM_USART_INT_CTRLB_CHSIZE_Pos) /**< (SERCOM_USART_INT_CTRLB) 7 Bits Position  */
#define SERCOM_USART_INT_CTRLB_SBMODE_Pos     _U_(6)                                               /**< (SERCOM_USART_INT_CTRLB) Stop Bit Mode Position */
#define SERCOM_USART_INT_CTRLB_SBMODE_Msk     (_U_(0x1) << SERCOM_USART_INT_CTRLB_SBMODE_Pos)      /**< (SERCOM_USART_INT_CTRLB) Stop Bit Mode Mask */
#define SERCOM_USART_INT_CTRLB_SBMODE(value)  (SERCOM_USART_INT_CTRLB_SBMODE_Msk & ((value) << SERCOM_USART_INT_CTRLB_SBMODE_Pos))
#define   SERCOM_USART_INT_CTRLB_SBMODE_1_BIT_Val _U_(0x0)                                             /**< (SERCOM_USART_INT_CTRLB) One Stop Bit  */
#define   SERCOM_USART_INT_CTRLB_SBMODE_2_BIT_Val _U_(0x1)                                             /**< (SERCOM_USART_INT_CTRLB) Two Stop Bits  */
#define SERCOM_USART_INT_CTRLB_SBMODE_1_BIT   (SERCOM_USART_INT_CTRLB_SBMODE_1_BIT_Val << SERCOM_USART_INT_CTRLB_SBMODE_Pos) /**< (SERCOM_USART_INT_CTRLB) One Stop Bit Position  */
#define SERCOM_USART_INT_CTRLB_SBMODE_2_BIT   (SERCOM_USART_INT_CTRLB_SBMODE_2_BIT_Val << SERCOM_USART_INT_CTRLB_SBMODE_Pos) /**< (SERCOM_USART_INT_CTRLB) Two Stop Bits Position  */
#define SERCOM_USART_INT_CTRLB_COLDEN_Pos     _U_(8)                                               /**< (SERCOM_USART_INT_CTRLB) Collision Detection Enable Position */
#define SERCOM_USART_INT_CTRLB_COLDEN_Msk     (_U_(0x1) << SERCOM_USART_INT_CTRLB_COLDEN_Pos)      /**< (SERCOM_USART_INT_CTRLB) Collision Detection Enable Mask */
#define SERCOM_USART_INT_CTRLB_COLDEN(value)  (SERCOM_USART_INT_CTRLB_COLDEN_Msk & ((value) << SERCOM_USART_INT_CTRLB_COLDEN_Pos))
#define SERCOM_USART_INT_CTRLB_SFDE_Pos       _U_(9)                                               /**< (SERCOM_USART_INT_CTRLB) Start of Frame Detection Enable Position */
#define SERCOM_USART_INT_CTRLB_SFDE_Msk       (_U_(0x1) << SERCOM_USART_INT_CTRLB_SFDE_Pos)        /**< (SERCOM_USART_INT_CTRLB) Start of Frame Detection Enable Mask */
#define SERCOM_USART_INT_CTRLB_SFDE(value)    (SERCOM_USART_INT_CTRLB_SFDE_Msk & ((value) << SERCOM_USART_INT_CTRLB_SFDE_Pos))
#define SERCOM_USART_INT_CTRLB_ENC_Pos        _U_(10)                                              /**< (SERCOM_USART_INT_CTRLB) Encoding Format Position */
#define SERCOM_USART_INT_CTRLB_ENC_Msk        (_U_(0x1) << SERCOM_USART_INT_CTRLB_ENC_Pos)         /**< (SERCOM_USART_INT_CTRLB) Encoding Format Mask */
#define SERCOM_USART_INT_CTRLB_ENC(value)     (SERCOM_USART_INT_CTRLB_ENC_Msk & ((value) << SERCOM_USART_INT_CTRLB_ENC_Pos))
#define SERCOM_USART_INT_CTRLB_PMODE_Pos      _U_(13)                                              /**< (SERCOM_USART_INT_CTRLB) Parity Mode Position */
#define SERCOM_USART_INT_CTRLB_PMODE_Msk      (_U_(0x1) << SERCOM_USART_INT_CTRLB_PMODE_Pos)       /**< (SERCOM_USART_INT_CTRLB) Parity Mode Mask */
#define SERCOM_USART_INT_CTRLB_PMODE(value)   (SERCOM_USART_INT_CTRLB_PMODE_Msk & ((value) << SERCOM_USART_INT_CTRLB_PMODE_Pos))
#define   SERCOM_USART_INT_CTRLB_PMODE_EVEN_Val _U_(0x0)                                             /**< (SERCOM_USART_INT_CTRLB) Even Parity  */
#define   SERCOM_USART_INT_CTRLB_PMODE_ODD_Val _U_(0x1)                                             /**< (SERCOM_USART_INT_CTRLB) Odd Parity  */
#define SERCOM_USART_INT_CTRLB_PMODE_EVEN     (SERCOM_USART_INT_CTRLB_PMODE_EVEN_Val << SERCOM_USART_INT_CTRLB_PMODE_Pos) /**< (SERCOM_USART_INT_CTRLB) Even Parity Position  */
#define SERCOM_USART_INT_CTRLB_PMODE_ODD      (SERCOM_USART_INT_CTRLB_PMODE_ODD_Val << SERCOM_USART_INT_CTRLB_PMODE_Pos) /**< (SERCOM_USART_INT_CTRLB) Odd Parity Position  */
#define SERCOM_USART_INT_CTRLB_TXEN_Pos       _U_(16)                                              /**< (SERCOM_USART_INT_CTRLB) Transmitter Enable Position */
#define SERCOM_USART_INT_CTRLB_TXEN_Msk       (_U_(0x1) << SERCOM_USART_INT_CTRLB_TXEN_Pos)        /**< (SERCOM_USART_INT_CTRLB) Transmitter Enable Mask */
#define SERCOM_USART_INT_CTRLB_TXEN(value)    (SERCOM_USART_INT_CTRLB_TXEN_Msk & ((value) << SERCOM_USART_INT_CTRLB_TXEN_Pos))
#define SERCOM_USART_INT_CTRLB_RXEN_Pos       _U_(17)                                              /**< (SERCOM_USART_INT_CTRLB) Receiver Enable Position */
#define SERCOM_USART_INT_CTRLB_RXEN_Msk       (_U_(0x1) << SERCOM_USART_INT_CTRLB_RXEN_Pos)        /**< (SERCOM_USART_INT_CTRLB) Receiver Enable Mask */
#define SERCOM_USART_INT_CTRLB_RXEN(value)    (SERCOM_USART_INT_CTRLB_RXEN_Msk & ((value) << SERCOM_USART_INT_CTRLB_RXEN_Pos))
#define SERCOM_USART_INT_CTRLB_LINCMD_Pos     _U_(24)                                              /**< (SERCOM_USART_INT_CTRLB) LIN Command Position */
#define SERCOM_USART_INT_CTRLB_LINCMD_Msk     (_U_(0x3) << SERCOM_USART_INT_CTRLB_LINCMD_Pos)      /**< (SERCOM_USART_INT_CTRLB) LIN Command Mask */
#define SERCOM_USART_INT_CTRLB_LINCMD(value)  (SERCOM_USART_INT_CTRLB_LINCMD_Msk & ((value) << SERCOM_USART_INT_CTRLB_LINCMD_Pos))
#define SERCOM_USART_INT_CTRLB_Msk            _U_(0x03032747)                                      /**< (SERCOM_USART_INT_CTRLB) Register Mask  */


/* -------- SERCOM_I2CM_CTRLC : (SERCOM Offset: 0x08) (R/W 32) I2CM Control C -------- */
#define SERCOM_I2CM_CTRLC_RESETVALUE          _U_(0x00)                                            /**<  (SERCOM_I2CM_CTRLC) I2CM Control C  Reset Value */

#define SERCOM_I2CM_CTRLC_DATA32B_Pos         _U_(24)                                              /**< (SERCOM_I2CM_CTRLC) Data 32 Bit Position */
#define SERCOM_I2CM_CTRLC_DATA32B_Msk         (_U_(0x1) << SERCOM_I2CM_CTRLC_DATA32B_Pos)          /**< (SERCOM_I2CM_CTRLC) Data 32 Bit Mask */
#define SERCOM_I2CM_CTRLC_DATA32B(value)      (SERCOM_I2CM_CTRLC_DATA32B_Msk & ((value) << SERCOM_I2CM_CTRLC_DATA32B_Pos))
#define   SERCOM_I2CM_CTRLC_DATA32B_DATA_TRANS_8BIT_Val _U_(0x0)                                             /**< (SERCOM_I2CM_CTRLC) Data transaction from/to DATA register are 8-bit  */
#define   SERCOM_I2CM_CTRLC_DATA32B_DATA_TRANS_32BIT_Val _U_(0x1)                                             /**< (SERCOM_I2CM_CTRLC) Data transaction from/to DATA register are 32-bit  */
#define SERCOM_I2CM_CTRLC_DATA32B_DATA_TRANS_8BIT (SERCOM_I2CM_CTRLC_DATA32B_DATA_TRANS_8BIT_Val << SERCOM_I2CM_CTRLC_DATA32B_Pos) /**< (SERCOM_I2CM_CTRLC) Data transaction from/to DATA register are 8-bit Position  */
#define SERCOM_I2CM_CTRLC_DATA32B_DATA_TRANS_32BIT (SERCOM_I2CM_CTRLC_DATA32B_DATA_TRANS_32BIT_Val << SERCOM_I2CM_CTRLC_DATA32B_Pos) /**< (SERCOM_I2CM_CTRLC) Data transaction from/to DATA register are 32-bit Position  */
#define SERCOM_I2CM_CTRLC_Msk                 _U_(0x01000000)                                      /**< (SERCOM_I2CM_CTRLC) Register Mask  */


/* -------- SERCOM_I2CS_CTRLC : (SERCOM Offset: 0x08) (R/W 32) I2CS Control C -------- */
#define SERCOM_I2CS_CTRLC_RESETVALUE          _U_(0x00)                                            /**<  (SERCOM_I2CS_CTRLC) I2CS Control C  Reset Value */

#define SERCOM_I2CS_CTRLC_SDASETUP_Pos        _U_(0)                                               /**< (SERCOM_I2CS_CTRLC) SDA Setup Time Position */
#define SERCOM_I2CS_CTRLC_SDASETUP_Msk        (_U_(0xF) << SERCOM_I2CS_CTRLC_SDASETUP_Pos)         /**< (SERCOM_I2CS_CTRLC) SDA Setup Time Mask */
#define SERCOM_I2CS_CTRLC_SDASETUP(value)     (SERCOM_I2CS_CTRLC_SDASETUP_Msk & ((value) << SERCOM_I2CS_CTRLC_SDASETUP_Pos))
#define SERCOM_I2CS_CTRLC_DATA32B_Pos         _U_(24)                                              /**< (SERCOM_I2CS_CTRLC) Data 32 Bit Position */
#define SERCOM_I2CS_CTRLC_DATA32B_Msk         (_U_(0x1) << SERCOM_I2CS_CTRLC_DATA32B_Pos)          /**< (SERCOM_I2CS_CTRLC) Data 32 Bit Mask */
#define SERCOM_I2CS_CTRLC_DATA32B(value)      (SERCOM_I2CS_CTRLC_DATA32B_Msk & ((value) << SERCOM_I2CS_CTRLC_DATA32B_Pos))
#define   SERCOM_I2CS_CTRLC_DATA32B_DATA_TRANS_8BIT_Val _U_(0x0)                                             /**< (SERCOM_I2CS_CTRLC) Data transaction from/to DATA register are 8-bit  */
#define   SERCOM_I2CS_CTRLC_DATA32B_DATA_TRANS_32BIT_Val _U_(0x1)                                             /**< (SERCOM_I2CS_CTRLC) Data transaction from/to DATA register are 32-bit  */
#define SERCOM_I2CS_CTRLC_DATA32B_DATA_TRANS_8BIT (SERCOM_I2CS_CTRLC_DATA32B_DATA_TRANS_8BIT_Val << SERCOM_I2CS_CTRLC_DATA32B_Pos) /**< (SERCOM_I2CS_CTRLC) Data transaction from/to DATA register are 8-bit Position  */
#define SERCOM_I2CS_CTRLC_DATA32B_DATA_TRANS_32BIT (SERCOM_I2CS_CTRLC_DATA32B_DATA_TRANS_32BIT_Val << SERCOM_I2CS_CTRLC_DATA32B_Pos) /**< (SERCOM_I2CS_CTRLC) Data transaction from/to DATA register are 32-bit Position  */
#define SERCOM_I2CS_CTRLC_Msk                 _U_(0x0100000F)                                      /**< (SERCOM_I2CS_CTRLC) Register Mask  */


/* -------- SERCOM_SPIM_CTRLC : (SERCOM Offset: 0x08) (R/W 32) SPIM Control C -------- */
#define SERCOM_SPIM_CTRLC_RESETVALUE          _U_(0x00)                                            /**<  (SERCOM_SPIM_CTRLC) SPIM Control C  Reset Value */

#define SERCOM_SPIM_CTRLC_ICSPACE_Pos         _U_(0)                                               /**< (SERCOM_SPIM_CTRLC) Inter-Character Spacing Position */
#define SERCOM_SPIM_CTRLC_ICSPACE_Msk         (_U_(0x3F) << SERCOM_SPIM_CTRLC_ICSPACE_Pos)         /**< (SERCOM_SPIM_CTRLC) Inter-Character Spacing Mask */
#define SERCOM_SPIM_CTRLC_ICSPACE(value)      (SERCOM_SPIM_CTRLC_ICSPACE_Msk & ((value) << SERCOM_SPIM_CTRLC_ICSPACE_Pos))
#define SERCOM_SPIM_CTRLC_DATA32B_Pos         _U_(24)                                              /**< (SERCOM_SPIM_CTRLC) Data 32 Bit Position */
#define SERCOM_SPIM_CTRLC_DATA32B_Msk         (_U_(0x1) << SERCOM_SPIM_CTRLC_DATA32B_Pos)          /**< (SERCOM_SPIM_CTRLC) Data 32 Bit Mask */
#define SERCOM_SPIM_CTRLC_DATA32B(value)      (SERCOM_SPIM_CTRLC_DATA32B_Msk & ((value) << SERCOM_SPIM_CTRLC_DATA32B_Pos))
#define   SERCOM_SPIM_CTRLC_DATA32B_DATA_TRANS_8BIT_Val _U_(0x0)                                             /**< (SERCOM_SPIM_CTRLC) Transaction from and to DATA register are 8-bit  */
#define   SERCOM_SPIM_CTRLC_DATA32B_DATA_TRANS_32BIT_Val _U_(0x1)                                             /**< (SERCOM_SPIM_CTRLC) Transaction from and to DATA register are 32-bit  */
#define SERCOM_SPIM_CTRLC_DATA32B_DATA_TRANS_8BIT (SERCOM_SPIM_CTRLC_DATA32B_DATA_TRANS_8BIT_Val << SERCOM_SPIM_CTRLC_DATA32B_Pos) /**< (SERCOM_SPIM_CTRLC) Transaction from and to DATA register are 8-bit Position  */
#define SERCOM_SPIM_CTRLC_DATA32B_DATA_TRANS_32BIT (SERCOM_SPIM_CTRLC_DATA32B_DATA_TRANS_32BIT_Val << SERCOM_SPIM_CTRLC_DATA32B_Pos) /**< (SERCOM_SPIM_CTRLC) Transaction from and to DATA register are 32-bit Position  */
#define SERCOM_SPIM_CTRLC_Msk                 _U_(0x0100003F)                                      /**< (SERCOM_SPIM_CTRLC) Register Mask  */


/* -------- SERCOM_SPIS_CTRLC : (SERCOM Offset: 0x08) (R/W 32) SPIS Control C -------- */
#define SERCOM_SPIS_CTRLC_RESETVALUE          _U_(0x00)                                            /**<  (SERCOM_SPIS_CTRLC) SPIS Control C  Reset Value */

#define SERCOM_SPIS_CTRLC_ICSPACE_Pos         _U_(0)                                               /**< (SERCOM_SPIS_CTRLC) Inter-Character Spacing Position */
#define SERCOM_SPIS_CTRLC_ICSPACE_Msk         (_U_(0x3F) << SERCOM_SPIS_CTRLC_ICSPACE_Pos)         /**< (SERCOM_SPIS_CTRLC) Inter-Character Spacing Mask */
#define SERCOM_SPIS_CTRLC_ICSPACE(value)      (SERCOM_SPIS_CTRLC_ICSPACE_Msk & ((value) << SERCOM_SPIS_CTRLC_ICSPACE_Pos))
#define SERCOM_SPIS_CTRLC_DATA32B_Pos         _U_(24)                                              /**< (SERCOM_SPIS_CTRLC) Data 32 Bit Position */
#define SERCOM_SPIS_CTRLC_DATA32B_Msk         (_U_(0x1) << SERCOM_SPIS_CTRLC_DATA32B_Pos)          /**< (SERCOM_SPIS_CTRLC) Data 32 Bit Mask */
#define SERCOM_SPIS_CTRLC_DATA32B(value)      (SERCOM_SPIS_CTRLC_DATA32B_Msk & ((value) << SERCOM_SPIS_CTRLC_DATA32B_Pos))
#define   SERCOM_SPIS_CTRLC_DATA32B_DATA_TRANS_8BIT_Val _U_(0x0)                                             /**< (SERCOM_SPIS_CTRLC) Transaction from and to DATA register are 8-bit  */
#define   SERCOM_SPIS_CTRLC_DATA32B_DATA_TRANS_32BIT_Val _U_(0x1)                                             /**< (SERCOM_SPIS_CTRLC) Transaction from and to DATA register are 32-bit  */
#define SERCOM_SPIS_CTRLC_DATA32B_DATA_TRANS_8BIT (SERCOM_SPIS_CTRLC_DATA32B_DATA_TRANS_8BIT_Val << SERCOM_SPIS_CTRLC_DATA32B_Pos) /**< (SERCOM_SPIS_CTRLC) Transaction from and to DATA register are 8-bit Position  */
#define SERCOM_SPIS_CTRLC_DATA32B_DATA_TRANS_32BIT (SERCOM_SPIS_CTRLC_DATA32B_DATA_TRANS_32BIT_Val << SERCOM_SPIS_CTRLC_DATA32B_Pos) /**< (SERCOM_SPIS_CTRLC) Transaction from and to DATA register are 32-bit Position  */
#define SERCOM_SPIS_CTRLC_Msk                 _U_(0x0100003F)                                      /**< (SERCOM_SPIS_CTRLC) Register Mask  */


/* -------- SERCOM_USART_EXT_CTRLC : (SERCOM Offset: 0x08) (R/W 32) USART_EXT Control C -------- */
#define SERCOM_USART_EXT_CTRLC_RESETVALUE     _U_(0x00)                                            /**<  (SERCOM_USART_EXT_CTRLC) USART_EXT Control C  Reset Value */

#define SERCOM_USART_EXT_CTRLC_GTIME_Pos      _U_(0)                                               /**< (SERCOM_USART_EXT_CTRLC) Guard Time Position */
#define SERCOM_USART_EXT_CTRLC_GTIME_Msk      (_U_(0x7) << SERCOM_USART_EXT_CTRLC_GTIME_Pos)       /**< (SERCOM_USART_EXT_CTRLC) Guard Time Mask */
#define SERCOM_USART_EXT_CTRLC_GTIME(value)   (SERCOM_USART_EXT_CTRLC_GTIME_Msk & ((value) << SERCOM_USART_EXT_CTRLC_GTIME_Pos))
#define SERCOM_USART_EXT_CTRLC_BRKLEN_Pos     _U_(8)                                               /**< (SERCOM_USART_EXT_CTRLC) LIN Master Break Length Position */
#define SERCOM_USART_EXT_CTRLC_BRKLEN_Msk     (_U_(0x3) << SERCOM_USART_EXT_CTRLC_BRKLEN_Pos)      /**< (SERCOM_USART_EXT_CTRLC) LIN Master Break Length Mask */
#define SERCOM_USART_EXT_CTRLC_BRKLEN(value)  (SERCOM_USART_EXT_CTRLC_BRKLEN_Msk & ((value) << SERCOM_USART_EXT_CTRLC_BRKLEN_Pos))
#define SERCOM_USART_EXT_CTRLC_HDRDLY_Pos     _U_(10)                                              /**< (SERCOM_USART_EXT_CTRLC) LIN Master Header Delay Position */
#define SERCOM_USART_EXT_CTRLC_HDRDLY_Msk     (_U_(0x3) << SERCOM_USART_EXT_CTRLC_HDRDLY_Pos)      /**< (SERCOM_USART_EXT_CTRLC) LIN Master Header Delay Mask */
#define SERCOM_USART_EXT_CTRLC_HDRDLY(value)  (SERCOM_USART_EXT_CTRLC_HDRDLY_Msk & ((value) << SERCOM_USART_EXT_CTRLC_HDRDLY_Pos))
#define SERCOM_USART_EXT_CTRLC_INACK_Pos      _U_(16)                                              /**< (SERCOM_USART_EXT_CTRLC) Inhibit Not Acknowledge Position */
#define SERCOM_USART_EXT_CTRLC_INACK_Msk      (_U_(0x1) << SERCOM_USART_EXT_CTRLC_INACK_Pos)       /**< (SERCOM_USART_EXT_CTRLC) Inhibit Not Acknowledge Mask */
#define SERCOM_USART_EXT_CTRLC_INACK(value)   (SERCOM_USART_EXT_CTRLC_INACK_Msk & ((value) << SERCOM_USART_EXT_CTRLC_INACK_Pos))
#define SERCOM_USART_EXT_CTRLC_DSNACK_Pos     _U_(17)                                              /**< (SERCOM_USART_EXT_CTRLC) Disable Successive NACK Position */
#define SERCOM_USART_EXT_CTRLC_DSNACK_Msk     (_U_(0x1) << SERCOM_USART_EXT_CTRLC_DSNACK_Pos)      /**< (SERCOM_USART_EXT_CTRLC) Disable Successive NACK Mask */
#define SERCOM_USART_EXT_CTRLC_DSNACK(value)  (SERCOM_USART_EXT_CTRLC_DSNACK_Msk & ((value) << SERCOM_USART_EXT_CTRLC_DSNACK_Pos))
#define SERCOM_USART_EXT_CTRLC_MAXITER_Pos    _U_(20)                                              /**< (SERCOM_USART_EXT_CTRLC) Maximum Iterations Position */
#define SERCOM_USART_EXT_CTRLC_MAXITER_Msk    (_U_(0x7) << SERCOM_USART_EXT_CTRLC_MAXITER_Pos)     /**< (SERCOM_USART_EXT_CTRLC) Maximum Iterations Mask */
#define SERCOM_USART_EXT_CTRLC_MAXITER(value) (SERCOM_USART_EXT_CTRLC_MAXITER_Msk & ((value) << SERCOM_USART_EXT_CTRLC_MAXITER_Pos))
#define SERCOM_USART_EXT_CTRLC_DATA32B_Pos    _U_(24)                                              /**< (SERCOM_USART_EXT_CTRLC) Data 32 Bit Position */
#define SERCOM_USART_EXT_CTRLC_DATA32B_Msk    (_U_(0x3) << SERCOM_USART_EXT_CTRLC_DATA32B_Pos)     /**< (SERCOM_USART_EXT_CTRLC) Data 32 Bit Mask */
#define SERCOM_USART_EXT_CTRLC_DATA32B(value) (SERCOM_USART_EXT_CTRLC_DATA32B_Msk & ((value) << SERCOM_USART_EXT_CTRLC_DATA32B_Pos))
#define   SERCOM_USART_EXT_CTRLC_DATA32B_DATA_READ_WRITE_CHSIZE_Val _U_(0x0)                                             /**< (SERCOM_USART_EXT_CTRLC) Data reads and writes according CTRLB.CHSIZE  */
#define   SERCOM_USART_EXT_CTRLC_DATA32B_DATA_READ_CHSIZE_WRITE_32BIT_Val _U_(0x1)                                             /**< (SERCOM_USART_EXT_CTRLC) Data reads according CTRLB.CHSIZE and writes according 32-bit extension  */
#define   SERCOM_USART_EXT_CTRLC_DATA32B_DATA_READ_32BIT_WRITE_CHSIZE_Val _U_(0x2)                                             /**< (SERCOM_USART_EXT_CTRLC) Data reads according 32-bit extension and writes according CTRLB.CHSIZE  */
#define   SERCOM_USART_EXT_CTRLC_DATA32B_DATA_READ_WRITE_32BIT_Val _U_(0x3)                                             /**< (SERCOM_USART_EXT_CTRLC) Data reads and writes according 32-bit extension  */
#define SERCOM_USART_EXT_CTRLC_DATA32B_DATA_READ_WRITE_CHSIZE (SERCOM_USART_EXT_CTRLC_DATA32B_DATA_READ_WRITE_CHSIZE_Val << SERCOM_USART_EXT_CTRLC_DATA32B_Pos) /**< (SERCOM_USART_EXT_CTRLC) Data reads and writes according CTRLB.CHSIZE Position  */
#define SERCOM_USART_EXT_CTRLC_DATA32B_DATA_READ_CHSIZE_WRITE_32BIT (SERCOM_USART_EXT_CTRLC_DATA32B_DATA_READ_CHSIZE_WRITE_32BIT_Val << SERCOM_USART_EXT_CTRLC_DATA32B_Pos) /**< (SERCOM_USART_EXT_CTRLC) Data reads according CTRLB.CHSIZE and writes according 32-bit extension Position  */
#define SERCOM_USART_EXT_CTRLC_DATA32B_DATA_READ_32BIT_WRITE_CHSIZE (SERCOM_USART_EXT_CTRLC_DATA32B_DATA_READ_32BIT_WRITE_CHSIZE_Val << SERCOM_USART_EXT_CTRLC_DATA32B_Pos) /**< (SERCOM_USART_EXT_CTRLC) Data reads according 32-bit extension and writes according CTRLB.CHSIZE Position  */
#define SERCOM_USART_EXT_CTRLC_DATA32B_DATA_READ_WRITE_32BIT (SERCOM_USART_EXT_CTRLC_DATA32B_DATA_READ_WRITE_32BIT_Val << SERCOM_USART_EXT_CTRLC_DATA32B_Pos) /**< (SERCOM_USART_EXT_CTRLC) Data reads and writes according 32-bit extension Position  */
#define SERCOM_USART_EXT_CTRLC_Msk            _U_(0x03730F07)                                      /**< (SERCOM_USART_EXT_CTRLC) Register Mask  */


/* -------- SERCOM_USART_INT_CTRLC : (SERCOM Offset: 0x08) (R/W 32) USART_INT Control C -------- */
#define SERCOM_USART_INT_CTRLC_RESETVALUE     _U_(0x00)                                            /**<  (SERCOM_USART_INT_CTRLC) USART_INT Control C  Reset Value */

#define SERCOM_USART_INT_CTRLC_GTIME_Pos      _U_(0)                                               /**< (SERCOM_USART_INT_CTRLC) Guard Time Position */
#define SERCOM_USART_INT_CTRLC_GTIME_Msk      (_U_(0x7) << SERCOM_USART_INT_CTRLC_GTIME_Pos)       /**< (SERCOM_USART_INT_CTRLC) Guard Time Mask */
#define SERCOM_USART_INT_CTRLC_GTIME(value)   (SERCOM_USART_INT_CTRLC_GTIME_Msk & ((value) << SERCOM_USART_INT_CTRLC_GTIME_Pos))
#define SERCOM_USART_INT_CTRLC_BRKLEN_Pos     _U_(8)                                               /**< (SERCOM_USART_INT_CTRLC) LIN Master Break Length Position */
#define SERCOM_USART_INT_CTRLC_BRKLEN_Msk     (_U_(0x3) << SERCOM_USART_INT_CTRLC_BRKLEN_Pos)      /**< (SERCOM_USART_INT_CTRLC) LIN Master Break Length Mask */
#define SERCOM_USART_INT_CTRLC_BRKLEN(value)  (SERCOM_USART_INT_CTRLC_BRKLEN_Msk & ((value) << SERCOM_USART_INT_CTRLC_BRKLEN_Pos))
#define SERCOM_USART_INT_CTRLC_HDRDLY_Pos     _U_(10)                                              /**< (SERCOM_USART_INT_CTRLC) LIN Master Header Delay Position */
#define SERCOM_USART_INT_CTRLC_HDRDLY_Msk     (_U_(0x3) << SERCOM_USART_INT_CTRLC_HDRDLY_Pos)      /**< (SERCOM_USART_INT_CTRLC) LIN Master Header Delay Mask */
#define SERCOM_USART_INT_CTRLC_HDRDLY(value)  (SERCOM_USART_INT_CTRLC_HDRDLY_Msk & ((value) << SERCOM_USART_INT_CTRLC_HDRDLY_Pos))
#define SERCOM_USART_INT_CTRLC_INACK_Pos      _U_(16)                                              /**< (SERCOM_USART_INT_CTRLC) Inhibit Not Acknowledge Position */
#define SERCOM_USART_INT_CTRLC_INACK_Msk      (_U_(0x1) << SERCOM_USART_INT_CTRLC_INACK_Pos)       /**< (SERCOM_USART_INT_CTRLC) Inhibit Not Acknowledge Mask */
#define SERCOM_USART_INT_CTRLC_INACK(value)   (SERCOM_USART_INT_CTRLC_INACK_Msk & ((value) << SERCOM_USART_INT_CTRLC_INACK_Pos))
#define SERCOM_USART_INT_CTRLC_DSNACK_Pos     _U_(17)                                              /**< (SERCOM_USART_INT_CTRLC) Disable Successive NACK Position */
#define SERCOM_USART_INT_CTRLC_DSNACK_Msk     (_U_(0x1) << SERCOM_USART_INT_CTRLC_DSNACK_Pos)      /**< (SERCOM_USART_INT_CTRLC) Disable Successive NACK Mask */
#define SERCOM_USART_INT_CTRLC_DSNACK(value)  (SERCOM_USART_INT_CTRLC_DSNACK_Msk & ((value) << SERCOM_USART_INT_CTRLC_DSNACK_Pos))
#define SERCOM_USART_INT_CTRLC_MAXITER_Pos    _U_(20)                                              /**< (SERCOM_USART_INT_CTRLC) Maximum Iterations Position */
#define SERCOM_USART_INT_CTRLC_MAXITER_Msk    (_U_(0x7) << SERCOM_USART_INT_CTRLC_MAXITER_Pos)     /**< (SERCOM_USART_INT_CTRLC) Maximum Iterations Mask */
#define SERCOM_USART_INT_CTRLC_MAXITER(value) (SERCOM_USART_INT_CTRLC_MAXITER_Msk & ((value) << SERCOM_USART_INT_CTRLC_MAXITER_Pos))
#define SERCOM_USART_INT_CTRLC_DATA32B_Pos    _U_(24)                                              /**< (SERCOM_USART_INT_CTRLC) Data 32 Bit Position */
#define SERCOM_USART_INT_CTRLC_DATA32B_Msk    (_U_(0x3) << SERCOM_USART_INT_CTRLC_DATA32B_Pos)     /**< (SERCOM_USART_INT_CTRLC) Data 32 Bit Mask */
#define SERCOM_USART_INT_CTRLC_DATA32B(value) (SERCOM_USART_INT_CTRLC_DATA32B_Msk & ((value) << SERCOM_USART_INT_CTRLC_DATA32B_Pos))
#define   SERCOM_USART_INT_CTRLC_DATA32B_DATA_READ_WRITE_CHSIZE_Val _U_(0x0)                                             /**< (SERCOM_USART_INT_CTRLC) Data reads and writes according CTRLB.CHSIZE  */
#define   SERCOM_USART_INT_CTRLC_DATA32B_DATA_READ_CHSIZE_WRITE_32BIT_Val _U_(0x1)                                             /**< (SERCOM_USART_INT_CTRLC) Data reads according CTRLB.CHSIZE and writes according 32-bit extension  */
#define   SERCOM_USART_INT_CTRLC_DATA32B_DATA_READ_32BIT_WRITE_CHSIZE_Val _U_(0x2)                                             /**< (SERCOM_USART_INT_CTRLC) Data reads according 32-bit extension and writes according CTRLB.CHSIZE  */
#define   SERCOM_USART_INT_CTRLC_DATA32B_DATA_READ_WRITE_32BIT_Val _U_(0x3)                                             /**< (SERCOM_USART_INT_CTRLC) Data reads and writes according 32-bit extension  */
#define SERCOM_USART_INT_CTRLC_DATA32B_DATA_READ_WRITE_CHSIZE (SERCOM_USART_INT_CTRLC_DATA32B_DATA_READ_WRITE_CHSIZE_Val << SERCOM_USART_INT_CTRLC_DATA32B_Pos) /**< (SERCOM_USART_INT_CTRLC) Data reads and writes according CTRLB.CHSIZE Position  */
#define SERCOM_USART_INT_CTRLC_DATA32B_DATA_READ_CHSIZE_WRITE_32BIT (SERCOM_USART_INT_CTRLC_DATA32B_DATA_READ_CHSIZE_WRITE_32BIT_Val << SERCOM_USART_INT_CTRLC_DATA32B_Pos) /**< (SERCOM_USART_INT_CTRLC) Data reads according CTRLB.CHSIZE and writes according 32-bit extension Position  */
#define SERCOM_USART_INT_CTRLC_DATA32B_DATA_READ_32BIT_WRITE_CHSIZE (SERCOM_USART_INT_CTRLC_DATA32B_DATA_READ_32BIT_WRITE_CHSIZE_Val << SERCOM_USART_INT_CTRLC_DATA32B_Pos) /**< (SERCOM_USART_INT_CTRLC) Data reads according 32-bit extension and writes according CTRLB.CHSIZE Position  */
#define SERCOM_USART_INT_CTRLC_DATA32B_DATA_READ_WRITE_32BIT (SERCOM_USART_INT_CTRLC_DATA32B_DATA_READ_WRITE_32BIT_Val << SERCOM_USART_INT_CTRLC_DATA32B_Pos) /**< (SERCOM_USART_INT_CTRLC) Data reads and writes according 32-bit extension Position  */
#define SERCOM_USART_INT_CTRLC_Msk            _U_(0x03730F07)                                      /**< (SERCOM_USART_INT_CTRLC) Register Mask  */


/* -------- SERCOM_I2CM_BAUD : (SERCOM Offset: 0x0C) (R/W 32) I2CM Baud Rate -------- */
#define SERCOM_I2CM_BAUD_RESETVALUE           _U_(0x00)                                            /**<  (SERCOM_I2CM_BAUD) I2CM Baud Rate  Reset Value */

#define SERCOM_I2CM_BAUD_BAUD_Pos             _U_(0)                                               /**< (SERCOM_I2CM_BAUD) Baud Rate Value Position */
#define SERCOM_I2CM_BAUD_BAUD_Msk             (_U_(0xFF) << SERCOM_I2CM_BAUD_BAUD_Pos)             /**< (SERCOM_I2CM_BAUD) Baud Rate Value Mask */
#define SERCOM_I2CM_BAUD_BAUD(value)          (SERCOM_I2CM_BAUD_BAUD_Msk & ((value) << SERCOM_I2CM_BAUD_BAUD_Pos))
#define SERCOM_I2CM_BAUD_BAUDLOW_Pos          _U_(8)                                               /**< (SERCOM_I2CM_BAUD) Baud Rate Value Low Position */
#define SERCOM_I2CM_BAUD_BAUDLOW_Msk          (_U_(0xFF) << SERCOM_I2CM_BAUD_BAUDLOW_Pos)          /**< (SERCOM_I2CM_BAUD) Baud Rate Value Low Mask */
#define SERCOM_I2CM_BAUD_BAUDLOW(value)       (SERCOM_I2CM_BAUD_BAUDLOW_Msk & ((value) << SERCOM_I2CM_BAUD_BAUDLOW_Pos))
#define SERCOM_I2CM_BAUD_HSBAUD_Pos           _U_(16)                                              /**< (SERCOM_I2CM_BAUD) High Speed Baud Rate Value Position */
#define SERCOM_I2CM_BAUD_HSBAUD_Msk           (_U_(0xFF) << SERCOM_I2CM_BAUD_HSBAUD_Pos)           /**< (SERCOM_I2CM_BAUD) High Speed Baud Rate Value Mask */
#define SERCOM_I2CM_BAUD_HSBAUD(value)        (SERCOM_I2CM_BAUD_HSBAUD_Msk & ((value) << SERCOM_I2CM_BAUD_HSBAUD_Pos))
#define SERCOM_I2CM_BAUD_HSBAUDLOW_Pos        _U_(24)                                              /**< (SERCOM_I2CM_BAUD) High Speed Baud Rate Value Low Position */
#define SERCOM_I2CM_BAUD_HSBAUDLOW_Msk        (_U_(0xFF) << SERCOM_I2CM_BAUD_HSBAUDLOW_Pos)        /**< (SERCOM_I2CM_BAUD) High Speed Baud Rate Value Low Mask */
#define SERCOM_I2CM_BAUD_HSBAUDLOW(value)     (SERCOM_I2CM_BAUD_HSBAUDLOW_Msk & ((value) << SERCOM_I2CM_BAUD_HSBAUDLOW_Pos))
#define SERCOM_I2CM_BAUD_Msk                  _U_(0xFFFFFFFF)                                      /**< (SERCOM_I2CM_BAUD) Register Mask  */


/* -------- SERCOM_SPIM_BAUD : (SERCOM Offset: 0x0C) (R/W 8) SPIM Baud Rate -------- */
#define SERCOM_SPIM_BAUD_RESETVALUE           _U_(0x00)                                            /**<  (SERCOM_SPIM_BAUD) SPIM Baud Rate  Reset Value */

#define SERCOM_SPIM_BAUD_BAUD_Pos             _U_(0)                                               /**< (SERCOM_SPIM_BAUD) Baud Rate Value Position */
#define SERCOM_SPIM_BAUD_BAUD_Msk             (_U_(0xFF) << SERCOM_SPIM_BAUD_BAUD_Pos)             /**< (SERCOM_SPIM_BAUD) Baud Rate Value Mask */
#define SERCOM_SPIM_BAUD_BAUD(value)          (SERCOM_SPIM_BAUD_BAUD_Msk & ((value) << SERCOM_SPIM_BAUD_BAUD_Pos))
#define SERCOM_SPIM_BAUD_Msk                  _U_(0xFF)                                            /**< (SERCOM_SPIM_BAUD) Register Mask  */


/* -------- SERCOM_SPIS_BAUD : (SERCOM Offset: 0x0C) (R/W 8) SPIS Baud Rate -------- */
#define SERCOM_SPIS_BAUD_RESETVALUE           _U_(0x00)                                            /**<  (SERCOM_SPIS_BAUD) SPIS Baud Rate  Reset Value */

#define SERCOM_SPIS_BAUD_BAUD_Pos             _U_(0)                                               /**< (SERCOM_SPIS_BAUD) Baud Rate Value Position */
#define SERCOM_SPIS_BAUD_BAUD_Msk             (_U_(0xFF) << SERCOM_SPIS_BAUD_BAUD_Pos)             /**< (SERCOM_SPIS_BAUD) Baud Rate Value Mask */
#define SERCOM_SPIS_BAUD_BAUD(value)          (SERCOM_SPIS_BAUD_BAUD_Msk & ((value) << SERCOM_SPIS_BAUD_BAUD_Pos))
#define SERCOM_SPIS_BAUD_Msk                  _U_(0xFF)                                            /**< (SERCOM_SPIS_BAUD) Register Mask  */


/* -------- SERCOM_USART_EXT_BAUD : (SERCOM Offset: 0x0C) (R/W 16) USART_EXT Baud Rate -------- */
#define SERCOM_USART_EXT_BAUD_RESETVALUE      _U_(0x00)                                            /**<  (SERCOM_USART_EXT_BAUD) USART_EXT Baud Rate  Reset Value */

#define SERCOM_USART_EXT_BAUD_BAUD_Pos        _U_(0)                                               /**< (SERCOM_USART_EXT_BAUD) Baud Rate Value Position */
#define SERCOM_USART_EXT_BAUD_BAUD_Msk        (_U_(0xFFFF) << SERCOM_USART_EXT_BAUD_BAUD_Pos)      /**< (SERCOM_USART_EXT_BAUD) Baud Rate Value Mask */
#define SERCOM_USART_EXT_BAUD_BAUD(value)     (SERCOM_USART_EXT_BAUD_BAUD_Msk & ((value) << SERCOM_USART_EXT_BAUD_BAUD_Pos))
#define SERCOM_USART_EXT_BAUD_Msk             _U_(0xFFFF)                                          /**< (SERCOM_USART_EXT_BAUD) Register Mask  */

/* FRAC mode */
#define SERCOM_USART_EXT_BAUD_FRAC_BAUD_Pos   _U_(0)                                               /**< (SERCOM_USART_EXT_BAUD) Baud Rate Value Position */
#define SERCOM_USART_EXT_BAUD_FRAC_BAUD_Msk   (_U_(0x1FFF) << SERCOM_USART_EXT_BAUD_FRAC_BAUD_Pos) /**< (SERCOM_USART_EXT_BAUD) Baud Rate Value Mask */
#define SERCOM_USART_EXT_BAUD_FRAC_BAUD(value) (SERCOM_USART_EXT_BAUD_FRAC_BAUD_Msk & ((value) << SERCOM_USART_EXT_BAUD_FRAC_BAUD_Pos))
#define SERCOM_USART_EXT_BAUD_FRAC_FP_Pos     _U_(13)                                              /**< (SERCOM_USART_EXT_BAUD) Fractional Part Position */
#define SERCOM_USART_EXT_BAUD_FRAC_FP_Msk     (_U_(0x7) << SERCOM_USART_EXT_BAUD_FRAC_FP_Pos)      /**< (SERCOM_USART_EXT_BAUD) Fractional Part Mask */
#define SERCOM_USART_EXT_BAUD_FRAC_FP(value)  (SERCOM_USART_EXT_BAUD_FRAC_FP_Msk & ((value) << SERCOM_USART_EXT_BAUD_FRAC_FP_Pos))
#define SERCOM_USART_EXT_BAUD_FRAC_Msk        _U_(0xFFFF)                                           /**< (SERCOM_USART_EXT_BAUD_FRAC) Register Mask  */

/* FRACFP mode */
#define SERCOM_USART_EXT_BAUD_FRACFP_BAUD_Pos _U_(0)                                               /**< (SERCOM_USART_EXT_BAUD) Baud Rate Value Position */
#define SERCOM_USART_EXT_BAUD_FRACFP_BAUD_Msk (_U_(0x1FFF) << SERCOM_USART_EXT_BAUD_FRACFP_BAUD_Pos) /**< (SERCOM_USART_EXT_BAUD) Baud Rate Value Mask */
#define SERCOM_USART_EXT_BAUD_FRACFP_BAUD(value) (SERCOM_USART_EXT_BAUD_FRACFP_BAUD_Msk & ((value) << SERCOM_USART_EXT_BAUD_FRACFP_BAUD_Pos))
#define SERCOM_USART_EXT_BAUD_FRACFP_FP_Pos   _U_(13)                                              /**< (SERCOM_USART_EXT_BAUD) Fractional Part Position */
#define SERCOM_USART_EXT_BAUD_FRACFP_FP_Msk   (_U_(0x7) << SERCOM_USART_EXT_BAUD_FRACFP_FP_Pos)    /**< (SERCOM_USART_EXT_BAUD) Fractional Part Mask */
#define SERCOM_USART_EXT_BAUD_FRACFP_FP(value) (SERCOM_USART_EXT_BAUD_FRACFP_FP_Msk & ((value) << SERCOM_USART_EXT_BAUD_FRACFP_FP_Pos))
#define SERCOM_USART_EXT_BAUD_FRACFP_Msk      _U_(0xFFFF)                                           /**< (SERCOM_USART_EXT_BAUD_FRACFP) Register Mask  */

/* USARTFP mode */
#define SERCOM_USART_EXT_BAUD_USARTFP_BAUD_Pos _U_(0)                                               /**< (SERCOM_USART_EXT_BAUD) Baud Rate Value Position */
#define SERCOM_USART_EXT_BAUD_USARTFP_BAUD_Msk (_U_(0xFFFF) << SERCOM_USART_EXT_BAUD_USARTFP_BAUD_Pos) /**< (SERCOM_USART_EXT_BAUD) Baud Rate Value Mask */
#define SERCOM_USART_EXT_BAUD_USARTFP_BAUD(value) (SERCOM_USART_EXT_BAUD_USARTFP_BAUD_Msk & ((value) << SERCOM_USART_EXT_BAUD_USARTFP_BAUD_Pos))
#define SERCOM_USART_EXT_BAUD_USARTFP_Msk     _U_(0xFFFF)                                           /**< (SERCOM_USART_EXT_BAUD_USARTFP) Register Mask  */


/* -------- SERCOM_USART_INT_BAUD : (SERCOM Offset: 0x0C) (R/W 16) USART_INT Baud Rate -------- */
#define SERCOM_USART_INT_BAUD_RESETVALUE      _U_(0x00)                                            /**<  (SERCOM_USART_INT_BAUD) USART_INT Baud Rate  Reset Value */

#define SERCOM_USART_INT_BAUD_BAUD_Pos        _U_(0)                                               /**< (SERCOM_USART_INT_BAUD) Baud Rate Value Position */
#define SERCOM_USART_INT_BAUD_BAUD_Msk        (_U_(0xFFFF) << SERCOM_USART_INT_BAUD_BAUD_Pos)      /**< (SERCOM_USART_INT_BAUD) Baud Rate Value Mask */
#define SERCOM_USART_INT_BAUD_BAUD(value)     (SERCOM_USART_INT_BAUD_BAUD_Msk & ((value) << SERCOM_USART_INT_BAUD_BAUD_Pos))
#define SERCOM_USART_INT_BAUD_Msk             _U_(0xFFFF)                                          /**< (SERCOM_USART_INT_BAUD) Register Mask  */

/* FRAC mode */
#define SERCOM_USART_INT_BAUD_FRAC_BAUD_Pos   _U_(0)                                               /**< (SERCOM_USART_INT_BAUD) Baud Rate Value Position */
#define SERCOM_USART_INT_BAUD_FRAC_BAUD_Msk   (_U_(0x1FFF) << SERCOM_USART_INT_BAUD_FRAC_BAUD_Pos) /**< (SERCOM_USART_INT_BAUD) Baud Rate Value Mask */
#define SERCOM_USART_INT_BAUD_FRAC_BAUD(value) (SERCOM_USART_INT_BAUD_FRAC_BAUD_Msk & ((value) << SERCOM_USART_INT_BAUD_FRAC_BAUD_Pos))
#define SERCOM_USART_INT_BAUD_FRAC_FP_Pos     _U_(13)                                              /**< (SERCOM_USART_INT_BAUD) Fractional Part Position */
#define SERCOM_USART_INT_BAUD_FRAC_FP_Msk     (_U_(0x7) << SERCOM_USART_INT_BAUD_FRAC_FP_Pos)      /**< (SERCOM_USART_INT_BAUD) Fractional Part Mask */
#define SERCOM_USART_INT_BAUD_FRAC_FP(value)  (SERCOM_USART_INT_BAUD_FRAC_FP_Msk & ((value) << SERCOM_USART_INT_BAUD_FRAC_FP_Pos))
#define SERCOM_USART_INT_BAUD_FRAC_Msk        _U_(0xFFFF)                                           /**< (SERCOM_USART_INT_BAUD_FRAC) Register Mask  */

/* FRACFP mode */
#define SERCOM_USART_INT_BAUD_FRACFP_BAUD_Pos _U_(0)                                               /**< (SERCOM_USART_INT_BAUD) Baud Rate Value Position */
#define SERCOM_USART_INT_BAUD_FRACFP_BAUD_Msk (_U_(0x1FFF) << SERCOM_USART_INT_BAUD_FRACFP_BAUD_Pos) /**< (SERCOM_USART_INT_BAUD) Baud Rate Value Mask */
#define SERCOM_USART_INT_BAUD_FRACFP_BAUD(value) (SERCOM_USART_INT_BAUD_FRACFP_BAUD_Msk & ((value) << SERCOM_USART_INT_BAUD_FRACFP_BAUD_Pos))
#define SERCOM_USART_INT_BAUD_FRACFP_FP_Pos   _U_(13)                                              /**< (SERCOM_USART_INT_BAUD) Fractional Part Position */
#define SERCOM_USART_INT_BAUD_FRACFP_FP_Msk   (_U_(0x7) << SERCOM_USART_INT_BAUD_FRACFP_FP_Pos)    /**< (SERCOM_USART_INT_BAUD) Fractional Part Mask */
#define SERCOM_USART_INT_BAUD_FRACFP_FP(value) (SERCOM_USART_INT_BAUD_FRACFP_FP_Msk & ((value) << SERCOM_USART_INT_BAUD_FRACFP_FP_Pos))
#define SERCOM_USART_INT_BAUD_FRACFP_Msk      _U_(0xFFFF)                                           /**< (SERCOM_USART_INT_BAUD_FRACFP) Register Mask  */

/* USARTFP mode */
#define SERCOM_USART_INT_BAUD_USARTFP_BAUD_Pos _U_(0)                                               /**< (SERCOM_USART_INT_BAUD) Baud Rate Value Position */
#define SERCOM_USART_INT_BAUD_USARTFP_BAUD_Msk (_U_(0xFFFF) << SERCOM_USART_INT_BAUD_USARTFP_BAUD_Pos) /**< (SERCOM_USART_INT_BAUD) Baud Rate Value Mask */
#define SERCOM_USART_INT_BAUD_USARTFP_BAUD(value) (SERCOM_USART_INT_BAUD_USARTFP_BAUD_Msk & ((value) << SERCOM_USART_INT_BAUD_USARTFP_BAUD_Pos))
#define SERCOM_USART_INT_BAUD_USARTFP_Msk     _U_(0xFFFF)                                           /**< (SERCOM_USART_INT_BAUD_USARTFP) Register Mask  */


/* -------- SERCOM_USART_EXT_RXPL : (SERCOM Offset: 0x0E) (R/W 8) USART_EXT Receive Pulse Length -------- */
#define SERCOM_USART_EXT_RXPL_RESETVALUE      _U_(0x00)                                            /**<  (SERCOM_USART_EXT_RXPL) USART_EXT Receive Pulse Length  Reset Value */

#define SERCOM_USART_EXT_RXPL_RXPL_Pos        _U_(0)                                               /**< (SERCOM_USART_EXT_RXPL) Receive Pulse Length Position */
#define SERCOM_USART_EXT_RXPL_RXPL_Msk        (_U_(0xFF) << SERCOM_USART_EXT_RXPL_RXPL_Pos)        /**< (SERCOM_USART_EXT_RXPL) Receive Pulse Length Mask */
#define SERCOM_USART_EXT_RXPL_RXPL(value)     (SERCOM_USART_EXT_RXPL_RXPL_Msk & ((value) << SERCOM_USART_EXT_RXPL_RXPL_Pos))
#define SERCOM_USART_EXT_RXPL_Msk             _U_(0xFF)                                            /**< (SERCOM_USART_EXT_RXPL) Register Mask  */


/* -------- SERCOM_USART_INT_RXPL : (SERCOM Offset: 0x0E) (R/W 8) USART_INT Receive Pulse Length -------- */
#define SERCOM_USART_INT_RXPL_RESETVALUE      _U_(0x00)                                            /**<  (SERCOM_USART_INT_RXPL) USART_INT Receive Pulse Length  Reset Value */

#define SERCOM_USART_INT_RXPL_RXPL_Pos        _U_(0)                                               /**< (SERCOM_USART_INT_RXPL) Receive Pulse Length Position */
#define SERCOM_USART_INT_RXPL_RXPL_Msk        (_U_(0xFF) << SERCOM_USART_INT_RXPL_RXPL_Pos)        /**< (SERCOM_USART_INT_RXPL) Receive Pulse Length Mask */
#define SERCOM_USART_INT_RXPL_RXPL(value)     (SERCOM_USART_INT_RXPL_RXPL_Msk & ((value) << SERCOM_USART_INT_RXPL_RXPL_Pos))
#define SERCOM_USART_INT_RXPL_Msk             _U_(0xFF)                                            /**< (SERCOM_USART_INT_RXPL) Register Mask  */


/* -------- SERCOM_I2CM_INTENCLR : (SERCOM Offset: 0x14) (R/W 8) I2CM Interrupt Enable Clear -------- */
#define SERCOM_I2CM_INTENCLR_RESETVALUE       _U_(0x00)                                            /**<  (SERCOM_I2CM_INTENCLR) I2CM Interrupt Enable Clear  Reset Value */

#define SERCOM_I2CM_INTENCLR_MB_Pos           _U_(0)                                               /**< (SERCOM_I2CM_INTENCLR) Master On Bus Interrupt Disable Position */
#define SERCOM_I2CM_INTENCLR_MB_Msk           (_U_(0x1) << SERCOM_I2CM_INTENCLR_MB_Pos)            /**< (SERCOM_I2CM_INTENCLR) Master On Bus Interrupt Disable Mask */
#define SERCOM_I2CM_INTENCLR_MB(value)        (SERCOM_I2CM_INTENCLR_MB_Msk & ((value) << SERCOM_I2CM_INTENCLR_MB_Pos))
#define SERCOM_I2CM_INTENCLR_SB_Pos           _U_(1)                                               /**< (SERCOM_I2CM_INTENCLR) Slave On Bus Interrupt Disable Position */
#define SERCOM_I2CM_INTENCLR_SB_Msk           (_U_(0x1) << SERCOM_I2CM_INTENCLR_SB_Pos)            /**< (SERCOM_I2CM_INTENCLR) Slave On Bus Interrupt Disable Mask */
#define SERCOM_I2CM_INTENCLR_SB(value)        (SERCOM_I2CM_INTENCLR_SB_Msk & ((value) << SERCOM_I2CM_INTENCLR_SB_Pos))
#define SERCOM_I2CM_INTENCLR_ERROR_Pos        _U_(7)                                               /**< (SERCOM_I2CM_INTENCLR) Combined Error Interrupt Disable Position */
#define SERCOM_I2CM_INTENCLR_ERROR_Msk        (_U_(0x1) << SERCOM_I2CM_INTENCLR_ERROR_Pos)         /**< (SERCOM_I2CM_INTENCLR) Combined Error Interrupt Disable Mask */
#define SERCOM_I2CM_INTENCLR_ERROR(value)     (SERCOM_I2CM_INTENCLR_ERROR_Msk & ((value) << SERCOM_I2CM_INTENCLR_ERROR_Pos))
#define SERCOM_I2CM_INTENCLR_Msk              _U_(0x83)                                            /**< (SERCOM_I2CM_INTENCLR) Register Mask  */


/* -------- SERCOM_I2CS_INTENCLR : (SERCOM Offset: 0x14) (R/W 8) I2CS Interrupt Enable Clear -------- */
#define SERCOM_I2CS_INTENCLR_RESETVALUE       _U_(0x00)                                            /**<  (SERCOM_I2CS_INTENCLR) I2CS Interrupt Enable Clear  Reset Value */

#define SERCOM_I2CS_INTENCLR_PREC_Pos         _U_(0)                                               /**< (SERCOM_I2CS_INTENCLR) Stop Received Interrupt Disable Position */
#define SERCOM_I2CS_INTENCLR_PREC_Msk         (_U_(0x1) << SERCOM_I2CS_INTENCLR_PREC_Pos)          /**< (SERCOM_I2CS_INTENCLR) Stop Received Interrupt Disable Mask */
#define SERCOM_I2CS_INTENCLR_PREC(value)      (SERCOM_I2CS_INTENCLR_PREC_Msk & ((value) << SERCOM_I2CS_INTENCLR_PREC_Pos))
#define SERCOM_I2CS_INTENCLR_AMATCH_Pos       _U_(1)                                               /**< (SERCOM_I2CS_INTENCLR) Address Match Interrupt Disable Position */
#define SERCOM_I2CS_INTENCLR_AMATCH_Msk       (_U_(0x1) << SERCOM_I2CS_INTENCLR_AMATCH_Pos)        /**< (SERCOM_I2CS_INTENCLR) Address Match Interrupt Disable Mask */
#define SERCOM_I2CS_INTENCLR_AMATCH(value)    (SERCOM_I2CS_INTENCLR_AMATCH_Msk & ((value) << SERCOM_I2CS_INTENCLR_AMATCH_Pos))
#define SERCOM_I2CS_INTENCLR_DRDY_Pos         _U_(2)                                               /**< (SERCOM_I2CS_INTENCLR) Data Interrupt Disable Position */
#define SERCOM_I2CS_INTENCLR_DRDY_Msk         (_U_(0x1) << SERCOM_I2CS_INTENCLR_DRDY_Pos)          /**< (SERCOM_I2CS_INTENCLR) Data Interrupt Disable Mask */
#define SERCOM_I2CS_INTENCLR_DRDY(value)      (SERCOM_I2CS_INTENCLR_DRDY_Msk & ((value) << SERCOM_I2CS_INTENCLR_DRDY_Pos))
#define SERCOM_I2CS_INTENCLR_ERROR_Pos        _U_(7)                                               /**< (SERCOM_I2CS_INTENCLR) Combined Error Interrupt Disable Position */
#define SERCOM_I2CS_INTENCLR_ERROR_Msk        (_U_(0x1) << SERCOM_I2CS_INTENCLR_ERROR_Pos)         /**< (SERCOM_I2CS_INTENCLR) Combined Error Interrupt Disable Mask */
#define SERCOM_I2CS_INTENCLR_ERROR(value)     (SERCOM_I2CS_INTENCLR_ERROR_Msk & ((value) << SERCOM_I2CS_INTENCLR_ERROR_Pos))
#define SERCOM_I2CS_INTENCLR_Msk              _U_(0x87)                                            /**< (SERCOM_I2CS_INTENCLR) Register Mask  */


/* -------- SERCOM_SPIM_INTENCLR : (SERCOM Offset: 0x14) (R/W 8) SPIM Interrupt Enable Clear -------- */
#define SERCOM_SPIM_INTENCLR_RESETVALUE       _U_(0x00)                                            /**<  (SERCOM_SPIM_INTENCLR) SPIM Interrupt Enable Clear  Reset Value */

#define SERCOM_SPIM_INTENCLR_DRE_Pos          _U_(0)                                               /**< (SERCOM_SPIM_INTENCLR) Data Register Empty Interrupt Disable Position */
#define SERCOM_SPIM_INTENCLR_DRE_Msk          (_U_(0x1) << SERCOM_SPIM_INTENCLR_DRE_Pos)           /**< (SERCOM_SPIM_INTENCLR) Data Register Empty Interrupt Disable Mask */
#define SERCOM_SPIM_INTENCLR_DRE(value)       (SERCOM_SPIM_INTENCLR_DRE_Msk & ((value) << SERCOM_SPIM_INTENCLR_DRE_Pos))
#define SERCOM_SPIM_INTENCLR_TXC_Pos          _U_(1)                                               /**< (SERCOM_SPIM_INTENCLR) Transmit Complete Interrupt Disable Position */
#define SERCOM_SPIM_INTENCLR_TXC_Msk          (_U_(0x1) << SERCOM_SPIM_INTENCLR_TXC_Pos)           /**< (SERCOM_SPIM_INTENCLR) Transmit Complete Interrupt Disable Mask */
#define SERCOM_SPIM_INTENCLR_TXC(value)       (SERCOM_SPIM_INTENCLR_TXC_Msk & ((value) << SERCOM_SPIM_INTENCLR_TXC_Pos))
#define SERCOM_SPIM_INTENCLR_RXC_Pos          _U_(2)                                               /**< (SERCOM_SPIM_INTENCLR) Receive Complete Interrupt Disable Position */
#define SERCOM_SPIM_INTENCLR_RXC_Msk          (_U_(0x1) << SERCOM_SPIM_INTENCLR_RXC_Pos)           /**< (SERCOM_SPIM_INTENCLR) Receive Complete Interrupt Disable Mask */
#define SERCOM_SPIM_INTENCLR_RXC(value)       (SERCOM_SPIM_INTENCLR_RXC_Msk & ((value) << SERCOM_SPIM_INTENCLR_RXC_Pos))
#define SERCOM_SPIM_INTENCLR_SSL_Pos          _U_(3)                                               /**< (SERCOM_SPIM_INTENCLR) Slave Select Low Interrupt Disable Position */
#define SERCOM_SPIM_INTENCLR_SSL_Msk          (_U_(0x1) << SERCOM_SPIM_INTENCLR_SSL_Pos)           /**< (SERCOM_SPIM_INTENCLR) Slave Select Low Interrupt Disable Mask */
#define SERCOM_SPIM_INTENCLR_SSL(value)       (SERCOM_SPIM_INTENCLR_SSL_Msk & ((value) << SERCOM_SPIM_INTENCLR_SSL_Pos))
#define SERCOM_SPIM_INTENCLR_ERROR_Pos        _U_(7)                                               /**< (SERCOM_SPIM_INTENCLR) Combined Error Interrupt Disable Position */
#define SERCOM_SPIM_INTENCLR_ERROR_Msk        (_U_(0x1) << SERCOM_SPIM_INTENCLR_ERROR_Pos)         /**< (SERCOM_SPIM_INTENCLR) Combined Error Interrupt Disable Mask */
#define SERCOM_SPIM_INTENCLR_ERROR(value)     (SERCOM_SPIM_INTENCLR_ERROR_Msk & ((value) << SERCOM_SPIM_INTENCLR_ERROR_Pos))
#define SERCOM_SPIM_INTENCLR_Msk              _U_(0x8F)                                            /**< (SERCOM_SPIM_INTENCLR) Register Mask  */


/* -------- SERCOM_SPIS_INTENCLR : (SERCOM Offset: 0x14) (R/W 8) SPIS Interrupt Enable Clear -------- */
#define SERCOM_SPIS_INTENCLR_RESETVALUE       _U_(0x00)                                            /**<  (SERCOM_SPIS_INTENCLR) SPIS Interrupt Enable Clear  Reset Value */

#define SERCOM_SPIS_INTENCLR_DRE_Pos          _U_(0)                                               /**< (SERCOM_SPIS_INTENCLR) Data Register Empty Interrupt Disable Position */
#define SERCOM_SPIS_INTENCLR_DRE_Msk          (_U_(0x1) << SERCOM_SPIS_INTENCLR_DRE_Pos)           /**< (SERCOM_SPIS_INTENCLR) Data Register Empty Interrupt Disable Mask */
#define SERCOM_SPIS_INTENCLR_DRE(value)       (SERCOM_SPIS_INTENCLR_DRE_Msk & ((value) << SERCOM_SPIS_INTENCLR_DRE_Pos))
#define SERCOM_SPIS_INTENCLR_TXC_Pos          _U_(1)                                               /**< (SERCOM_SPIS_INTENCLR) Transmit Complete Interrupt Disable Position */
#define SERCOM_SPIS_INTENCLR_TXC_Msk          (_U_(0x1) << SERCOM_SPIS_INTENCLR_TXC_Pos)           /**< (SERCOM_SPIS_INTENCLR) Transmit Complete Interrupt Disable Mask */
#define SERCOM_SPIS_INTENCLR_TXC(value)       (SERCOM_SPIS_INTENCLR_TXC_Msk & ((value) << SERCOM_SPIS_INTENCLR_TXC_Pos))
#define SERCOM_SPIS_INTENCLR_RXC_Pos          _U_(2)                                               /**< (SERCOM_SPIS_INTENCLR) Receive Complete Interrupt Disable Position */
#define SERCOM_SPIS_INTENCLR_RXC_Msk          (_U_(0x1) << SERCOM_SPIS_INTENCLR_RXC_Pos)           /**< (SERCOM_SPIS_INTENCLR) Receive Complete Interrupt Disable Mask */
#define SERCOM_SPIS_INTENCLR_RXC(value)       (SERCOM_SPIS_INTENCLR_RXC_Msk & ((value) << SERCOM_SPIS_INTENCLR_RXC_Pos))
#define SERCOM_SPIS_INTENCLR_SSL_Pos          _U_(3)                                               /**< (SERCOM_SPIS_INTENCLR) Slave Select Low Interrupt Disable Position */
#define SERCOM_SPIS_INTENCLR_SSL_Msk          (_U_(0x1) << SERCOM_SPIS_INTENCLR_SSL_Pos)           /**< (SERCOM_SPIS_INTENCLR) Slave Select Low Interrupt Disable Mask */
#define SERCOM_SPIS_INTENCLR_SSL(value)       (SERCOM_SPIS_INTENCLR_SSL_Msk & ((value) << SERCOM_SPIS_INTENCLR_SSL_Pos))
#define SERCOM_SPIS_INTENCLR_ERROR_Pos        _U_(7)                                               /**< (SERCOM_SPIS_INTENCLR) Combined Error Interrupt Disable Position */
#define SERCOM_SPIS_INTENCLR_ERROR_Msk        (_U_(0x1) << SERCOM_SPIS_INTENCLR_ERROR_Pos)         /**< (SERCOM_SPIS_INTENCLR) Combined Error Interrupt Disable Mask */
#define SERCOM_SPIS_INTENCLR_ERROR(value)     (SERCOM_SPIS_INTENCLR_ERROR_Msk & ((value) << SERCOM_SPIS_INTENCLR_ERROR_Pos))
#define SERCOM_SPIS_INTENCLR_Msk              _U_(0x8F)                                            /**< (SERCOM_SPIS_INTENCLR) Register Mask  */


/* -------- SERCOM_USART_EXT_INTENCLR : (SERCOM Offset: 0x14) (R/W 8) USART_EXT Interrupt Enable Clear -------- */
#define SERCOM_USART_EXT_INTENCLR_RESETVALUE  _U_(0x00)                                            /**<  (SERCOM_USART_EXT_INTENCLR) USART_EXT Interrupt Enable Clear  Reset Value */

#define SERCOM_USART_EXT_INTENCLR_DRE_Pos     _U_(0)                                               /**< (SERCOM_USART_EXT_INTENCLR) Data Register Empty Interrupt Disable Position */
#define SERCOM_USART_EXT_INTENCLR_DRE_Msk     (_U_(0x1) << SERCOM_USART_EXT_INTENCLR_DRE_Pos)      /**< (SERCOM_USART_EXT_INTENCLR) Data Register Empty Interrupt Disable Mask */
#define SERCOM_USART_EXT_INTENCLR_DRE(value)  (SERCOM_USART_EXT_INTENCLR_DRE_Msk & ((value) << SERCOM_USART_EXT_INTENCLR_DRE_Pos))
#define SERCOM_USART_EXT_INTENCLR_TXC_Pos     _U_(1)                                               /**< (SERCOM_USART_EXT_INTENCLR) Transmit Complete Interrupt Disable Position */
#define SERCOM_USART_EXT_INTENCLR_TXC_Msk     (_U_(0x1) << SERCOM_USART_EXT_INTENCLR_TXC_Pos)      /**< (SERCOM_USART_EXT_INTENCLR) Transmit Complete Interrupt Disable Mask */
#define SERCOM_USART_EXT_INTENCLR_TXC(value)  (SERCOM_USART_EXT_INTENCLR_TXC_Msk & ((value) << SERCOM_USART_EXT_INTENCLR_TXC_Pos))
#define SERCOM_USART_EXT_INTENCLR_RXC_Pos     _U_(2)                                               /**< (SERCOM_USART_EXT_INTENCLR) Receive Complete Interrupt Disable Position */
#define SERCOM_USART_EXT_INTENCLR_RXC_Msk     (_U_(0x1) << SERCOM_USART_EXT_INTENCLR_RXC_Pos)      /**< (SERCOM_USART_EXT_INTENCLR) Receive Complete Interrupt Disable Mask */
#define SERCOM_USART_EXT_INTENCLR_RXC(value)  (SERCOM_USART_EXT_INTENCLR_RXC_Msk & ((value) << SERCOM_USART_EXT_INTENCLR_RXC_Pos))
#define SERCOM_USART_EXT_INTENCLR_RXS_Pos     _U_(3)                                               /**< (SERCOM_USART_EXT_INTENCLR) Receive Start Interrupt Disable Position */
#define SERCOM_USART_EXT_INTENCLR_RXS_Msk     (_U_(0x1) << SERCOM_USART_EXT_INTENCLR_RXS_Pos)      /**< (SERCOM_USART_EXT_INTENCLR) Receive Start Interrupt Disable Mask */
#define SERCOM_USART_EXT_INTENCLR_RXS(value)  (SERCOM_USART_EXT_INTENCLR_RXS_Msk & ((value) << SERCOM_USART_EXT_INTENCLR_RXS_Pos))
#define SERCOM_USART_EXT_INTENCLR_CTSIC_Pos   _U_(4)                                               /**< (SERCOM_USART_EXT_INTENCLR) Clear To Send Input Change Interrupt Disable Position */
#define SERCOM_USART_EXT_INTENCLR_CTSIC_Msk   (_U_(0x1) << SERCOM_USART_EXT_INTENCLR_CTSIC_Pos)    /**< (SERCOM_USART_EXT_INTENCLR) Clear To Send Input Change Interrupt Disable Mask */
#define SERCOM_USART_EXT_INTENCLR_CTSIC(value) (SERCOM_USART_EXT_INTENCLR_CTSIC_Msk & ((value) << SERCOM_USART_EXT_INTENCLR_CTSIC_Pos))
#define SERCOM_USART_EXT_INTENCLR_RXBRK_Pos   _U_(5)                                               /**< (SERCOM_USART_EXT_INTENCLR) Break Received Interrupt Disable Position */
#define SERCOM_USART_EXT_INTENCLR_RXBRK_Msk   (_U_(0x1) << SERCOM_USART_EXT_INTENCLR_RXBRK_Pos)    /**< (SERCOM_USART_EXT_INTENCLR) Break Received Interrupt Disable Mask */
#define SERCOM_USART_EXT_INTENCLR_RXBRK(value) (SERCOM_USART_EXT_INTENCLR_RXBRK_Msk & ((value) << SERCOM_USART_EXT_INTENCLR_RXBRK_Pos))
#define SERCOM_USART_EXT_INTENCLR_ERROR_Pos   _U_(7)                                               /**< (SERCOM_USART_EXT_INTENCLR) Combined Error Interrupt Disable Position */
#define SERCOM_USART_EXT_INTENCLR_ERROR_Msk   (_U_(0x1) << SERCOM_USART_EXT_INTENCLR_ERROR_Pos)    /**< (SERCOM_USART_EXT_INTENCLR) Combined Error Interrupt Disable Mask */
#define SERCOM_USART_EXT_INTENCLR_ERROR(value) (SERCOM_USART_EXT_INTENCLR_ERROR_Msk & ((value) << SERCOM_USART_EXT_INTENCLR_ERROR_Pos))
#define SERCOM_USART_EXT_INTENCLR_Msk         _U_(0xBF)                                            /**< (SERCOM_USART_EXT_INTENCLR) Register Mask  */


/* -------- SERCOM_USART_INT_INTENCLR : (SERCOM Offset: 0x14) (R/W 8) USART_INT Interrupt Enable Clear -------- */
#define SERCOM_USART_INT_INTENCLR_RESETVALUE  _U_(0x00)                                            /**<  (SERCOM_USART_INT_INTENCLR) USART_INT Interrupt Enable Clear  Reset Value */

#define SERCOM_USART_INT_INTENCLR_DRE_Pos     _U_(0)                                               /**< (SERCOM_USART_INT_INTENCLR) Data Register Empty Interrupt Disable Position */
#define SERCOM_USART_INT_INTENCLR_DRE_Msk     (_U_(0x1) << SERCOM_USART_INT_INTENCLR_DRE_Pos)      /**< (SERCOM_USART_INT_INTENCLR) Data Register Empty Interrupt Disable Mask */
#define SERCOM_USART_INT_INTENCLR_DRE(value)  (SERCOM_USART_INT_INTENCLR_DRE_Msk & ((value) << SERCOM_USART_INT_INTENCLR_DRE_Pos))
#define SERCOM_USART_INT_INTENCLR_TXC_Pos     _U_(1)                                               /**< (SERCOM_USART_INT_INTENCLR) Transmit Complete Interrupt Disable Position */
#define SERCOM_USART_INT_INTENCLR_TXC_Msk     (_U_(0x1) << SERCOM_USART_INT_INTENCLR_TXC_Pos)      /**< (SERCOM_USART_INT_INTENCLR) Transmit Complete Interrupt Disable Mask */
#define SERCOM_USART_INT_INTENCLR_TXC(value)  (SERCOM_USART_INT_INTENCLR_TXC_Msk & ((value) << SERCOM_USART_INT_INTENCLR_TXC_Pos))
#define SERCOM_USART_INT_INTENCLR_RXC_Pos     _U_(2)                                               /**< (SERCOM_USART_INT_INTENCLR) Receive Complete Interrupt Disable Position */
#define SERCOM_USART_INT_INTENCLR_RXC_Msk     (_U_(0x1) << SERCOM_USART_INT_INTENCLR_RXC_Pos)      /**< (SERCOM_USART_INT_INTENCLR) Receive Complete Interrupt Disable Mask */
#define SERCOM_USART_INT_INTENCLR_RXC(value)  (SERCOM_USART_INT_INTENCLR_RXC_Msk & ((value) << SERCOM_USART_INT_INTENCLR_RXC_Pos))
#define SERCOM_USART_INT_INTENCLR_RXS_Pos     _U_(3)                                               /**< (SERCOM_USART_INT_INTENCLR) Receive Start Interrupt Disable Position */
#define SERCOM_USART_INT_INTENCLR_RXS_Msk     (_U_(0x1) << SERCOM_USART_INT_INTENCLR_RXS_Pos)      /**< (SERCOM_USART_INT_INTENCLR) Receive Start Interrupt Disable Mask */
#define SERCOM_USART_INT_INTENCLR_RXS(value)  (SERCOM_USART_INT_INTENCLR_RXS_Msk & ((value) << SERCOM_USART_INT_INTENCLR_RXS_Pos))
#define SERCOM_USART_INT_INTENCLR_CTSIC_Pos   _U_(4)                                               /**< (SERCOM_USART_INT_INTENCLR) Clear To Send Input Change Interrupt Disable Position */
#define SERCOM_USART_INT_INTENCLR_CTSIC_Msk   (_U_(0x1) << SERCOM_USART_INT_INTENCLR_CTSIC_Pos)    /**< (SERCOM_USART_INT_INTENCLR) Clear To Send Input Change Interrupt Disable Mask */
#define SERCOM_USART_INT_INTENCLR_CTSIC(value) (SERCOM_USART_INT_INTENCLR_CTSIC_Msk & ((value) << SERCOM_USART_INT_INTENCLR_CTSIC_Pos))
#define SERCOM_USART_INT_INTENCLR_RXBRK_Pos   _U_(5)                                               /**< (SERCOM_USART_INT_INTENCLR) Break Received Interrupt Disable Position */
#define SERCOM_USART_INT_INTENCLR_RXBRK_Msk   (_U_(0x1) << SERCOM_USART_INT_INTENCLR_RXBRK_Pos)    /**< (SERCOM_USART_INT_INTENCLR) Break Received Interrupt Disable Mask */
#define SERCOM_USART_INT_INTENCLR_RXBRK(value) (SERCOM_USART_INT_INTENCLR_RXBRK_Msk & ((value) << SERCOM_USART_INT_INTENCLR_RXBRK_Pos))
#define SERCOM_USART_INT_INTENCLR_ERROR_Pos   _U_(7)                                               /**< (SERCOM_USART_INT_INTENCLR) Combined Error Interrupt Disable Position */
#define SERCOM_USART_INT_INTENCLR_ERROR_Msk   (_U_(0x1) << SERCOM_USART_INT_INTENCLR_ERROR_Pos)    /**< (SERCOM_USART_INT_INTENCLR) Combined Error Interrupt Disable Mask */
#define SERCOM_USART_INT_INTENCLR_ERROR(value) (SERCOM_USART_INT_INTENCLR_ERROR_Msk & ((value) << SERCOM_USART_INT_INTENCLR_ERROR_Pos))
#define SERCOM_USART_INT_INTENCLR_Msk         _U_(0xBF)                                            /**< (SERCOM_USART_INT_INTENCLR) Register Mask  */


/* -------- SERCOM_I2CM_INTENSET : (SERCOM Offset: 0x16) (R/W 8) I2CM Interrupt Enable Set -------- */
#define SERCOM_I2CM_INTENSET_RESETVALUE       _U_(0x00)                                            /**<  (SERCOM_I2CM_INTENSET) I2CM Interrupt Enable Set  Reset Value */

#define SERCOM_I2CM_INTENSET_MB_Pos           _U_(0)                                               /**< (SERCOM_I2CM_INTENSET) Master On Bus Interrupt Enable Position */
#define SERCOM_I2CM_INTENSET_MB_Msk           (_U_(0x1) << SERCOM_I2CM_INTENSET_MB_Pos)            /**< (SERCOM_I2CM_INTENSET) Master On Bus Interrupt Enable Mask */
#define SERCOM_I2CM_INTENSET_MB(value)        (SERCOM_I2CM_INTENSET_MB_Msk & ((value) << SERCOM_I2CM_INTENSET_MB_Pos))
#define SERCOM_I2CM_INTENSET_SB_Pos           _U_(1)                                               /**< (SERCOM_I2CM_INTENSET) Slave On Bus Interrupt Enable Position */
#define SERCOM_I2CM_INTENSET_SB_Msk           (_U_(0x1) << SERCOM_I2CM_INTENSET_SB_Pos)            /**< (SERCOM_I2CM_INTENSET) Slave On Bus Interrupt Enable Mask */
#define SERCOM_I2CM_INTENSET_SB(value)        (SERCOM_I2CM_INTENSET_SB_Msk & ((value) << SERCOM_I2CM_INTENSET_SB_Pos))
#define SERCOM_I2CM_INTENSET_ERROR_Pos        _U_(7)                                               /**< (SERCOM_I2CM_INTENSET) Combined Error Interrupt Enable Position */
#define SERCOM_I2CM_INTENSET_ERROR_Msk        (_U_(0x1) << SERCOM_I2CM_INTENSET_ERROR_Pos)         /**< (SERCOM_I2CM_INTENSET) Combined Error Interrupt Enable Mask */
#define SERCOM_I2CM_INTENSET_ERROR(value)     (SERCOM_I2CM_INTENSET_ERROR_Msk & ((value) << SERCOM_I2CM_INTENSET_ERROR_Pos))
#define SERCOM_I2CM_INTENSET_Msk              _U_(0x83)                                            /**< (SERCOM_I2CM_INTENSET) Register Mask  */


/* -------- SERCOM_I2CS_INTENSET : (SERCOM Offset: 0x16) (R/W 8) I2CS Interrupt Enable Set -------- */
#define SERCOM_I2CS_INTENSET_RESETVALUE       _U_(0x00)                                            /**<  (SERCOM_I2CS_INTENSET) I2CS Interrupt Enable Set  Reset Value */

#define SERCOM_I2CS_INTENSET_PREC_Pos         _U_(0)                                               /**< (SERCOM_I2CS_INTENSET) Stop Received Interrupt Enable Position */
#define SERCOM_I2CS_INTENSET_PREC_Msk         (_U_(0x1) << SERCOM_I2CS_INTENSET_PREC_Pos)          /**< (SERCOM_I2CS_INTENSET) Stop Received Interrupt Enable Mask */
#define SERCOM_I2CS_INTENSET_PREC(value)      (SERCOM_I2CS_INTENSET_PREC_Msk & ((value) << SERCOM_I2CS_INTENSET_PREC_Pos))
#define SERCOM_I2CS_INTENSET_AMATCH_Pos       _U_(1)                                               /**< (SERCOM_I2CS_INTENSET) Address Match Interrupt Enable Position */
#define SERCOM_I2CS_INTENSET_AMATCH_Msk       (_U_(0x1) << SERCOM_I2CS_INTENSET_AMATCH_Pos)        /**< (SERCOM_I2CS_INTENSET) Address Match Interrupt Enable Mask */
#define SERCOM_I2CS_INTENSET_AMATCH(value)    (SERCOM_I2CS_INTENSET_AMATCH_Msk & ((value) << SERCOM_I2CS_INTENSET_AMATCH_Pos))
#define SERCOM_I2CS_INTENSET_DRDY_Pos         _U_(2)                                               /**< (SERCOM_I2CS_INTENSET) Data Interrupt Enable Position */
#define SERCOM_I2CS_INTENSET_DRDY_Msk         (_U_(0x1) << SERCOM_I2CS_INTENSET_DRDY_Pos)          /**< (SERCOM_I2CS_INTENSET) Data Interrupt Enable Mask */
#define SERCOM_I2CS_INTENSET_DRDY(value)      (SERCOM_I2CS_INTENSET_DRDY_Msk & ((value) << SERCOM_I2CS_INTENSET_DRDY_Pos))
#define SERCOM_I2CS_INTENSET_ERROR_Pos        _U_(7)                                               /**< (SERCOM_I2CS_INTENSET) Combined Error Interrupt Enable Position */
#define SERCOM_I2CS_INTENSET_ERROR_Msk        (_U_(0x1) << SERCOM_I2CS_INTENSET_ERROR_Pos)         /**< (SERCOM_I2CS_INTENSET) Combined Error Interrupt Enable Mask */
#define SERCOM_I2CS_INTENSET_ERROR(value)     (SERCOM_I2CS_INTENSET_ERROR_Msk & ((value) << SERCOM_I2CS_INTENSET_ERROR_Pos))
#define SERCOM_I2CS_INTENSET_Msk              _U_(0x87)                                            /**< (SERCOM_I2CS_INTENSET) Register Mask  */


/* -------- SERCOM_SPIM_INTENSET : (SERCOM Offset: 0x16) (R/W 8) SPIM Interrupt Enable Set -------- */
#define SERCOM_SPIM_INTENSET_RESETVALUE       _U_(0x00)                                            /**<  (SERCOM_SPIM_INTENSET) SPIM Interrupt Enable Set  Reset Value */

#define SERCOM_SPIM_INTENSET_DRE_Pos          _U_(0)                                               /**< (SERCOM_SPIM_INTENSET) Data Register Empty Interrupt Enable Position */
#define SERCOM_SPIM_INTENSET_DRE_Msk          (_U_(0x1) << SERCOM_SPIM_INTENSET_DRE_Pos)           /**< (SERCOM_SPIM_INTENSET) Data Register Empty Interrupt Enable Mask */
#define SERCOM_SPIM_INTENSET_DRE(value)       (SERCOM_SPIM_INTENSET_DRE_Msk & ((value) << SERCOM_SPIM_INTENSET_DRE_Pos))
#define SERCOM_SPIM_INTENSET_TXC_Pos          _U_(1)                                               /**< (SERCOM_SPIM_INTENSET) Transmit Complete Interrupt Enable Position */
#define SERCOM_SPIM_INTENSET_TXC_Msk          (_U_(0x1) << SERCOM_SPIM_INTENSET_TXC_Pos)           /**< (SERCOM_SPIM_INTENSET) Transmit Complete Interrupt Enable Mask */
#define SERCOM_SPIM_INTENSET_TXC(value)       (SERCOM_SPIM_INTENSET_TXC_Msk & ((value) << SERCOM_SPIM_INTENSET_TXC_Pos))
#define SERCOM_SPIM_INTENSET_RXC_Pos          _U_(2)                                               /**< (SERCOM_SPIM_INTENSET) Receive Complete Interrupt Enable Position */
#define SERCOM_SPIM_INTENSET_RXC_Msk          (_U_(0x1) << SERCOM_SPIM_INTENSET_RXC_Pos)           /**< (SERCOM_SPIM_INTENSET) Receive Complete Interrupt Enable Mask */
#define SERCOM_SPIM_INTENSET_RXC(value)       (SERCOM_SPIM_INTENSET_RXC_Msk & ((value) << SERCOM_SPIM_INTENSET_RXC_Pos))
#define SERCOM_SPIM_INTENSET_SSL_Pos          _U_(3)                                               /**< (SERCOM_SPIM_INTENSET) Slave Select Low Interrupt Enable Position */
#define SERCOM_SPIM_INTENSET_SSL_Msk          (_U_(0x1) << SERCOM_SPIM_INTENSET_SSL_Pos)           /**< (SERCOM_SPIM_INTENSET) Slave Select Low Interrupt Enable Mask */
#define SERCOM_SPIM_INTENSET_SSL(value)       (SERCOM_SPIM_INTENSET_SSL_Msk & ((value) << SERCOM_SPIM_INTENSET_SSL_Pos))
#define SERCOM_SPIM_INTENSET_ERROR_Pos        _U_(7)                                               /**< (SERCOM_SPIM_INTENSET) Combined Error Interrupt Enable Position */
#define SERCOM_SPIM_INTENSET_ERROR_Msk        (_U_(0x1) << SERCOM_SPIM_INTENSET_ERROR_Pos)         /**< (SERCOM_SPIM_INTENSET) Combined Error Interrupt Enable Mask */
#define SERCOM_SPIM_INTENSET_ERROR(value)     (SERCOM_SPIM_INTENSET_ERROR_Msk & ((value) << SERCOM_SPIM_INTENSET_ERROR_Pos))
#define SERCOM_SPIM_INTENSET_Msk              _U_(0x8F)                                            /**< (SERCOM_SPIM_INTENSET) Register Mask  */


/* -------- SERCOM_SPIS_INTENSET : (SERCOM Offset: 0x16) (R/W 8) SPIS Interrupt Enable Set -------- */
#define SERCOM_SPIS_INTENSET_RESETVALUE       _U_(0x00)                                            /**<  (SERCOM_SPIS_INTENSET) SPIS Interrupt Enable Set  Reset Value */

#define SERCOM_SPIS_INTENSET_DRE_Pos          _U_(0)                                               /**< (SERCOM_SPIS_INTENSET) Data Register Empty Interrupt Enable Position */
#define SERCOM_SPIS_INTENSET_DRE_Msk          (_U_(0x1) << SERCOM_SPIS_INTENSET_DRE_Pos)           /**< (SERCOM_SPIS_INTENSET) Data Register Empty Interrupt Enable Mask */
#define SERCOM_SPIS_INTENSET_DRE(value)       (SERCOM_SPIS_INTENSET_DRE_Msk & ((value) << SERCOM_SPIS_INTENSET_DRE_Pos))
#define SERCOM_SPIS_INTENSET_TXC_Pos          _U_(1)                                               /**< (SERCOM_SPIS_INTENSET) Transmit Complete Interrupt Enable Position */
#define SERCOM_SPIS_INTENSET_TXC_Msk          (_U_(0x1) << SERCOM_SPIS_INTENSET_TXC_Pos)           /**< (SERCOM_SPIS_INTENSET) Transmit Complete Interrupt Enable Mask */
#define SERCOM_SPIS_INTENSET_TXC(value)       (SERCOM_SPIS_INTENSET_TXC_Msk & ((value) << SERCOM_SPIS_INTENSET_TXC_Pos))
#define SERCOM_SPIS_INTENSET_RXC_Pos          _U_(2)                                               /**< (SERCOM_SPIS_INTENSET) Receive Complete Interrupt Enable Position */
#define SERCOM_SPIS_INTENSET_RXC_Msk          (_U_(0x1) << SERCOM_SPIS_INTENSET_RXC_Pos)           /**< (SERCOM_SPIS_INTENSET) Receive Complete Interrupt Enable Mask */
#define SERCOM_SPIS_INTENSET_RXC(value)       (SERCOM_SPIS_INTENSET_RXC_Msk & ((value) << SERCOM_SPIS_INTENSET_RXC_Pos))
#define SERCOM_SPIS_INTENSET_SSL_Pos          _U_(3)                                               /**< (SERCOM_SPIS_INTENSET) Slave Select Low Interrupt Enable Position */
#define SERCOM_SPIS_INTENSET_SSL_Msk          (_U_(0x1) << SERCOM_SPIS_INTENSET_SSL_Pos)           /**< (SERCOM_SPIS_INTENSET) Slave Select Low Interrupt Enable Mask */
#define SERCOM_SPIS_INTENSET_SSL(value)       (SERCOM_SPIS_INTENSET_SSL_Msk & ((value) << SERCOM_SPIS_INTENSET_SSL_Pos))
#define SERCOM_SPIS_INTENSET_ERROR_Pos        _U_(7)                                               /**< (SERCOM_SPIS_INTENSET) Combined Error Interrupt Enable Position */
#define SERCOM_SPIS_INTENSET_ERROR_Msk        (_U_(0x1) << SERCOM_SPIS_INTENSET_ERROR_Pos)         /**< (SERCOM_SPIS_INTENSET) Combined Error Interrupt Enable Mask */
#define SERCOM_SPIS_INTENSET_ERROR(value)     (SERCOM_SPIS_INTENSET_ERROR_Msk & ((value) << SERCOM_SPIS_INTENSET_ERROR_Pos))
#define SERCOM_SPIS_INTENSET_Msk              _U_(0x8F)                                            /**< (SERCOM_SPIS_INTENSET) Register Mask  */


/* -------- SERCOM_USART_EXT_INTENSET : (SERCOM Offset: 0x16) (R/W 8) USART_EXT Interrupt Enable Set -------- */
#define SERCOM_USART_EXT_INTENSET_RESETVALUE  _U_(0x00)                                            /**<  (SERCOM_USART_EXT_INTENSET) USART_EXT Interrupt Enable Set  Reset Value */

#define SERCOM_USART_EXT_INTENSET_DRE_Pos     _U_(0)                                               /**< (SERCOM_USART_EXT_INTENSET) Data Register Empty Interrupt Enable Position */
#define SERCOM_USART_EXT_INTENSET_DRE_Msk     (_U_(0x1) << SERCOM_USART_EXT_INTENSET_DRE_Pos)      /**< (SERCOM_USART_EXT_INTENSET) Data Register Empty Interrupt Enable Mask */
#define SERCOM_USART_EXT_INTENSET_DRE(value)  (SERCOM_USART_EXT_INTENSET_DRE_Msk & ((value) << SERCOM_USART_EXT_INTENSET_DRE_Pos))
#define SERCOM_USART_EXT_INTENSET_TXC_Pos     _U_(1)                                               /**< (SERCOM_USART_EXT_INTENSET) Transmit Complete Interrupt Enable Position */
#define SERCOM_USART_EXT_INTENSET_TXC_Msk     (_U_(0x1) << SERCOM_USART_EXT_INTENSET_TXC_Pos)      /**< (SERCOM_USART_EXT_INTENSET) Transmit Complete Interrupt Enable Mask */
#define SERCOM_USART_EXT_INTENSET_TXC(value)  (SERCOM_USART_EXT_INTENSET_TXC_Msk & ((value) << SERCOM_USART_EXT_INTENSET_TXC_Pos))
#define SERCOM_USART_EXT_INTENSET_RXC_Pos     _U_(2)                                               /**< (SERCOM_USART_EXT_INTENSET) Receive Complete Interrupt Enable Position */
#define SERCOM_USART_EXT_INTENSET_RXC_Msk     (_U_(0x1) << SERCOM_USART_EXT_INTENSET_RXC_Pos)      /**< (SERCOM_USART_EXT_INTENSET) Receive Complete Interrupt Enable Mask */
#define SERCOM_USART_EXT_INTENSET_RXC(value)  (SERCOM_USART_EXT_INTENSET_RXC_Msk & ((value) << SERCOM_USART_EXT_INTENSET_RXC_Pos))
#define SERCOM_USART_EXT_INTENSET_RXS_Pos     _U_(3)                                               /**< (SERCOM_USART_EXT_INTENSET) Receive Start Interrupt Enable Position */
#define SERCOM_USART_EXT_INTENSET_RXS_Msk     (_U_(0x1) << SERCOM_USART_EXT_INTENSET_RXS_Pos)      /**< (SERCOM_USART_EXT_INTENSET) Receive Start Interrupt Enable Mask */
#define SERCOM_USART_EXT_INTENSET_RXS(value)  (SERCOM_USART_EXT_INTENSET_RXS_Msk & ((value) << SERCOM_USART_EXT_INTENSET_RXS_Pos))
#define SERCOM_USART_EXT_INTENSET_CTSIC_Pos   _U_(4)                                               /**< (SERCOM_USART_EXT_INTENSET) Clear To Send Input Change Interrupt Enable Position */
#define SERCOM_USART_EXT_INTENSET_CTSIC_Msk   (_U_(0x1) << SERCOM_USART_EXT_INTENSET_CTSIC_Pos)    /**< (SERCOM_USART_EXT_INTENSET) Clear To Send Input Change Interrupt Enable Mask */
#define SERCOM_USART_EXT_INTENSET_CTSIC(value) (SERCOM_USART_EXT_INTENSET_CTSIC_Msk & ((value) << SERCOM_USART_EXT_INTENSET_CTSIC_Pos))
#define SERCOM_USART_EXT_INTENSET_RXBRK_Pos   _U_(5)                                               /**< (SERCOM_USART_EXT_INTENSET) Break Received Interrupt Enable Position */
#define SERCOM_USART_EXT_INTENSET_RXBRK_Msk   (_U_(0x1) << SERCOM_USART_EXT_INTENSET_RXBRK_Pos)    /**< (SERCOM_USART_EXT_INTENSET) Break Received Interrupt Enable Mask */
#define SERCOM_USART_EXT_INTENSET_RXBRK(value) (SERCOM_USART_EXT_INTENSET_RXBRK_Msk & ((value) << SERCOM_USART_EXT_INTENSET_RXBRK_Pos))
#define SERCOM_USART_EXT_INTENSET_ERROR_Pos   _U_(7)                                               /**< (SERCOM_USART_EXT_INTENSET) Combined Error Interrupt Enable Position */
#define SERCOM_USART_EXT_INTENSET_ERROR_Msk   (_U_(0x1) << SERCOM_USART_EXT_INTENSET_ERROR_Pos)    /**< (SERCOM_USART_EXT_INTENSET) Combined Error Interrupt Enable Mask */
#define SERCOM_USART_EXT_INTENSET_ERROR(value) (SERCOM_USART_EXT_INTENSET_ERROR_Msk & ((value) << SERCOM_USART_EXT_INTENSET_ERROR_Pos))
#define SERCOM_USART_EXT_INTENSET_Msk         _U_(0xBF)                                            /**< (SERCOM_USART_EXT_INTENSET) Register Mask  */


/* -------- SERCOM_USART_INT_INTENSET : (SERCOM Offset: 0x16) (R/W 8) USART_INT Interrupt Enable Set -------- */
#define SERCOM_USART_INT_INTENSET_RESETVALUE  _U_(0x00)                                            /**<  (SERCOM_USART_INT_INTENSET) USART_INT Interrupt Enable Set  Reset Value */

#define SERCOM_USART_INT_INTENSET_DRE_Pos     _U_(0)                                               /**< (SERCOM_USART_INT_INTENSET) Data Register Empty Interrupt Enable Position */
#define SERCOM_USART_INT_INTENSET_DRE_Msk     (_U_(0x1) << SERCOM_USART_INT_INTENSET_DRE_Pos)      /**< (SERCOM_USART_INT_INTENSET) Data Register Empty Interrupt Enable Mask */
#define SERCOM_USART_INT_INTENSET_DRE(value)  (SERCOM_USART_INT_INTENSET_DRE_Msk & ((value) << SERCOM_USART_INT_INTENSET_DRE_Pos))
#define SERCOM_USART_INT_INTENSET_TXC_Pos     _U_(1)                                               /**< (SERCOM_USART_INT_INTENSET) Transmit Complete Interrupt Enable Position */
#define SERCOM_USART_INT_INTENSET_TXC_Msk     (_U_(0x1) << SERCOM_USART_INT_INTENSET_TXC_Pos)      /**< (SERCOM_USART_INT_INTENSET) Transmit Complete Interrupt Enable Mask */
#define SERCOM_USART_INT_INTENSET_TXC(value)  (SERCOM_USART_INT_INTENSET_TXC_Msk & ((value) << SERCOM_USART_INT_INTENSET_TXC_Pos))
#define SERCOM_USART_INT_INTENSET_RXC_Pos     _U_(2)                                               /**< (SERCOM_USART_INT_INTENSET) Receive Complete Interrupt Enable Position */
#define SERCOM_USART_INT_INTENSET_RXC_Msk     (_U_(0x1) << SERCOM_USART_INT_INTENSET_RXC_Pos)      /**< (SERCOM_USART_INT_INTENSET) Receive Complete Interrupt Enable Mask */
#define SERCOM_USART_INT_INTENSET_RXC(value)  (SERCOM_USART_INT_INTENSET_RXC_Msk & ((value) << SERCOM_USART_INT_INTENSET_RXC_Pos))
#define SERCOM_USART_INT_INTENSET_RXS_Pos     _U_(3)                                               /**< (SERCOM_USART_INT_INTENSET) Receive Start Interrupt Enable Position */
#define SERCOM_USART_INT_INTENSET_RXS_Msk     (_U_(0x1) << SERCOM_USART_INT_INTENSET_RXS_Pos)      /**< (SERCOM_USART_INT_INTENSET) Receive Start Interrupt Enable Mask */
#define SERCOM_USART_INT_INTENSET_RXS(value)  (SERCOM_USART_INT_INTENSET_RXS_Msk & ((value) << SERCOM_USART_INT_INTENSET_RXS_Pos))
#define SERCOM_USART_INT_INTENSET_CTSIC_Pos   _U_(4)                                               /**< (SERCOM_USART_INT_INTENSET) Clear To Send Input Change Interrupt Enable Position */
#define SERCOM_USART_INT_INTENSET_CTSIC_Msk   (_U_(0x1) << SERCOM_USART_INT_INTENSET_CTSIC_Pos)    /**< (SERCOM_USART_INT_INTENSET) Clear To Send Input Change Interrupt Enable Mask */
#define SERCOM_USART_INT_INTENSET_CTSIC(value) (SERCOM_USART_INT_INTENSET_CTSIC_Msk & ((value) << SERCOM_USART_INT_INTENSET_CTSIC_Pos))
#define SERCOM_USART_INT_INTENSET_RXBRK_Pos   _U_(5)                                               /**< (SERCOM_USART_INT_INTENSET) Break Received Interrupt Enable Position */
#define SERCOM_USART_INT_INTENSET_RXBRK_Msk   (_U_(0x1) << SERCOM_USART_INT_INTENSET_RXBRK_Pos)    /**< (SERCOM_USART_INT_INTENSET) Break Received Interrupt Enable Mask */
#define SERCOM_USART_INT_INTENSET_RXBRK(value) (SERCOM_USART_INT_INTENSET_RXBRK_Msk & ((value) << SERCOM_USART_INT_INTENSET_RXBRK_Pos))
#define SERCOM_USART_INT_INTENSET_ERROR_Pos   _U_(7)                                               /**< (SERCOM_USART_INT_INTENSET) Combined Error Interrupt Enable Position */
#define SERCOM_USART_INT_INTENSET_ERROR_Msk   (_U_(0x1) << SERCOM_USART_INT_INTENSET_ERROR_Pos)    /**< (SERCOM_USART_INT_INTENSET) Combined Error Interrupt Enable Mask */
#define SERCOM_USART_INT_INTENSET_ERROR(value) (SERCOM_USART_INT_INTENSET_ERROR_Msk & ((value) << SERCOM_USART_INT_INTENSET_ERROR_Pos))
#define SERCOM_USART_INT_INTENSET_Msk         _U_(0xBF)                                            /**< (SERCOM_USART_INT_INTENSET) Register Mask  */


/* -------- SERCOM_I2CM_INTFLAG : (SERCOM Offset: 0x18) (R/W 8) I2CM Interrupt Flag Status and Clear -------- */
#define SERCOM_I2CM_INTFLAG_RESETVALUE        _U_(0x00)                                            /**<  (SERCOM_I2CM_INTFLAG) I2CM Interrupt Flag Status and Clear  Reset Value */

#define SERCOM_I2CM_INTFLAG_MB_Pos            _U_(0)                                               /**< (SERCOM_I2CM_INTFLAG) Master On Bus Interrupt Position */
#define SERCOM_I2CM_INTFLAG_MB_Msk            (_U_(0x1) << SERCOM_I2CM_INTFLAG_MB_Pos)             /**< (SERCOM_I2CM_INTFLAG) Master On Bus Interrupt Mask */
#define SERCOM_I2CM_INTFLAG_MB(value)         (SERCOM_I2CM_INTFLAG_MB_Msk & ((value) << SERCOM_I2CM_INTFLAG_MB_Pos))
#define SERCOM_I2CM_INTFLAG_SB_Pos            _U_(1)                                               /**< (SERCOM_I2CM_INTFLAG) Slave On Bus Interrupt Position */
#define SERCOM_I2CM_INTFLAG_SB_Msk            (_U_(0x1) << SERCOM_I2CM_INTFLAG_SB_Pos)             /**< (SERCOM_I2CM_INTFLAG) Slave On Bus Interrupt Mask */
#define SERCOM_I2CM_INTFLAG_SB(value)         (SERCOM_I2CM_INTFLAG_SB_Msk & ((value) << SERCOM_I2CM_INTFLAG_SB_Pos))
#define SERCOM_I2CM_INTFLAG_ERROR_Pos         _U_(7)                                               /**< (SERCOM_I2CM_INTFLAG) Combined Error Interrupt Position */
#define SERCOM_I2CM_INTFLAG_ERROR_Msk         (_U_(0x1) << SERCOM_I2CM_INTFLAG_ERROR_Pos)          /**< (SERCOM_I2CM_INTFLAG) Combined Error Interrupt Mask */
#define SERCOM_I2CM_INTFLAG_ERROR(value)      (SERCOM_I2CM_INTFLAG_ERROR_Msk & ((value) << SERCOM_I2CM_INTFLAG_ERROR_Pos))
#define SERCOM_I2CM_INTFLAG_Msk               _U_(0x83)                                            /**< (SERCOM_I2CM_INTFLAG) Register Mask  */


/* -------- SERCOM_I2CS_INTFLAG : (SERCOM Offset: 0x18) (R/W 8) I2CS Interrupt Flag Status and Clear -------- */
#define SERCOM_I2CS_INTFLAG_RESETVALUE        _U_(0x00)                                            /**<  (SERCOM_I2CS_INTFLAG) I2CS Interrupt Flag Status and Clear  Reset Value */

#define SERCOM_I2CS_INTFLAG_PREC_Pos          _U_(0)                                               /**< (SERCOM_I2CS_INTFLAG) Stop Received Interrupt Position */
#define SERCOM_I2CS_INTFLAG_PREC_Msk          (_U_(0x1) << SERCOM_I2CS_INTFLAG_PREC_Pos)           /**< (SERCOM_I2CS_INTFLAG) Stop Received Interrupt Mask */
#define SERCOM_I2CS_INTFLAG_PREC(value)       (SERCOM_I2CS_INTFLAG_PREC_Msk & ((value) << SERCOM_I2CS_INTFLAG_PREC_Pos))
#define SERCOM_I2CS_INTFLAG_AMATCH_Pos        _U_(1)                                               /**< (SERCOM_I2CS_INTFLAG) Address Match Interrupt Position */
#define SERCOM_I2CS_INTFLAG_AMATCH_Msk        (_U_(0x1) << SERCOM_I2CS_INTFLAG_AMATCH_Pos)         /**< (SERCOM_I2CS_INTFLAG) Address Match Interrupt Mask */
#define SERCOM_I2CS_INTFLAG_AMATCH(value)     (SERCOM_I2CS_INTFLAG_AMATCH_Msk & ((value) << SERCOM_I2CS_INTFLAG_AMATCH_Pos))
#define SERCOM_I2CS_INTFLAG_DRDY_Pos          _U_(2)                                               /**< (SERCOM_I2CS_INTFLAG) Data Interrupt Position */
#define SERCOM_I2CS_INTFLAG_DRDY_Msk          (_U_(0x1) << SERCOM_I2CS_INTFLAG_DRDY_Pos)           /**< (SERCOM_I2CS_INTFLAG) Data Interrupt Mask */
#define SERCOM_I2CS_INTFLAG_DRDY(value)       (SERCOM_I2CS_INTFLAG_DRDY_Msk & ((value) << SERCOM_I2CS_INTFLAG_DRDY_Pos))
#define SERCOM_I2CS_INTFLAG_ERROR_Pos         _U_(7)                                               /**< (SERCOM_I2CS_INTFLAG) Combined Error Interrupt Position */
#define SERCOM_I2CS_INTFLAG_ERROR_Msk         (_U_(0x1) << SERCOM_I2CS_INTFLAG_ERROR_Pos)          /**< (SERCOM_I2CS_INTFLAG) Combined Error Interrupt Mask */
#define SERCOM_I2CS_INTFLAG_ERROR(value)      (SERCOM_I2CS_INTFLAG_ERROR_Msk & ((value) << SERCOM_I2CS_INTFLAG_ERROR_Pos))
#define SERCOM_I2CS_INTFLAG_Msk               _U_(0x87)                                            /**< (SERCOM_I2CS_INTFLAG) Register Mask  */


/* -------- SERCOM_SPIM_INTFLAG : (SERCOM Offset: 0x18) (R/W 8) SPIM Interrupt Flag Status and Clear -------- */
#define SERCOM_SPIM_INTFLAG_RESETVALUE        _U_(0x00)                                            /**<  (SERCOM_SPIM_INTFLAG) SPIM Interrupt Flag Status and Clear  Reset Value */

#define SERCOM_SPIM_INTFLAG_DRE_Pos           _U_(0)                                               /**< (SERCOM_SPIM_INTFLAG) Data Register Empty Interrupt Position */
#define SERCOM_SPIM_INTFLAG_DRE_Msk           (_U_(0x1) << SERCOM_SPIM_INTFLAG_DRE_Pos)            /**< (SERCOM_SPIM_INTFLAG) Data Register Empty Interrupt Mask */
#define SERCOM_SPIM_INTFLAG_DRE(value)        (SERCOM_SPIM_INTFLAG_DRE_Msk & ((value) << SERCOM_SPIM_INTFLAG_DRE_Pos))
#define SERCOM_SPIM_INTFLAG_TXC_Pos           _U_(1)                                               /**< (SERCOM_SPIM_INTFLAG) Transmit Complete Interrupt Position */
#define SERCOM_SPIM_INTFLAG_TXC_Msk           (_U_(0x1) << SERCOM_SPIM_INTFLAG_TXC_Pos)            /**< (SERCOM_SPIM_INTFLAG) Transmit Complete Interrupt Mask */
#define SERCOM_SPIM_INTFLAG_TXC(value)        (SERCOM_SPIM_INTFLAG_TXC_Msk & ((value) << SERCOM_SPIM_INTFLAG_TXC_Pos))
#define SERCOM_SPIM_INTFLAG_RXC_Pos           _U_(2)                                               /**< (SERCOM_SPIM_INTFLAG) Receive Complete Interrupt Position */
#define SERCOM_SPIM_INTFLAG_RXC_Msk           (_U_(0x1) << SERCOM_SPIM_INTFLAG_RXC_Pos)            /**< (SERCOM_SPIM_INTFLAG) Receive Complete Interrupt Mask */
#define SERCOM_SPIM_INTFLAG_RXC(value)        (SERCOM_SPIM_INTFLAG_RXC_Msk & ((value) << SERCOM_SPIM_INTFLAG_RXC_Pos))
#define SERCOM_SPIM_INTFLAG_SSL_Pos           _U_(3)                                               /**< (SERCOM_SPIM_INTFLAG) Slave Select Low Interrupt Flag Position */
#define SERCOM_SPIM_INTFLAG_SSL_Msk           (_U_(0x1) << SERCOM_SPIM_INTFLAG_SSL_Pos)            /**< (SERCOM_SPIM_INTFLAG) Slave Select Low Interrupt Flag Mask */
#define SERCOM_SPIM_INTFLAG_SSL(value)        (SERCOM_SPIM_INTFLAG_SSL_Msk & ((value) << SERCOM_SPIM_INTFLAG_SSL_Pos))
#define SERCOM_SPIM_INTFLAG_ERROR_Pos         _U_(7)                                               /**< (SERCOM_SPIM_INTFLAG) Combined Error Interrupt Position */
#define SERCOM_SPIM_INTFLAG_ERROR_Msk         (_U_(0x1) << SERCOM_SPIM_INTFLAG_ERROR_Pos)          /**< (SERCOM_SPIM_INTFLAG) Combined Error Interrupt Mask */
#define SERCOM_SPIM_INTFLAG_ERROR(value)      (SERCOM_SPIM_INTFLAG_ERROR_Msk & ((value) << SERCOM_SPIM_INTFLAG_ERROR_Pos))
#define SERCOM_SPIM_INTFLAG_Msk               _U_(0x8F)                                            /**< (SERCOM_SPIM_INTFLAG) Register Mask  */


/* -------- SERCOM_SPIS_INTFLAG : (SERCOM Offset: 0x18) (R/W 8) SPIS Interrupt Flag Status and Clear -------- */
#define SERCOM_SPIS_INTFLAG_RESETVALUE        _U_(0x00)                                            /**<  (SERCOM_SPIS_INTFLAG) SPIS Interrupt Flag Status and Clear  Reset Value */

#define SERCOM_SPIS_INTFLAG_DRE_Pos           _U_(0)                                               /**< (SERCOM_SPIS_INTFLAG) Data Register Empty Interrupt Position */
#define SERCOM_SPIS_INTFLAG_DRE_Msk           (_U_(0x1) << SERCOM_SPIS_INTFLAG_DRE_Pos)            /**< (SERCOM_SPIS_INTFLAG) Data Register Empty Interrupt Mask */
#define SERCOM_SPIS_INTFLAG_DRE(value)        (SERCOM_SPIS_INTFLAG_DRE_Msk & ((value) << SERCOM_SPIS_INTFLAG_DRE_Pos))
#define SERCOM_SPIS_INTFLAG_TXC_Pos           _U_(1)                                               /**< (SERCOM_SPIS_INTFLAG) Transmit Complete Interrupt Position */
#define SERCOM_SPIS_INTFLAG_TXC_Msk           (_U_(0x1) << SERCOM_SPIS_INTFLAG_TXC_Pos)            /**< (SERCOM_SPIS_INTFLAG) Transmit Complete Interrupt Mask */
#define SERCOM_SPIS_INTFLAG_TXC(value)        (SERCOM_SPIS_INTFLAG_TXC_Msk & ((value) << SERCOM_SPIS_INTFLAG_TXC_Pos))
#define SERCOM_SPIS_INTFLAG_RXC_Pos           _U_(2)                                               /**< (SERCOM_SPIS_INTFLAG) Receive Complete Interrupt Position */
#define SERCOM_SPIS_INTFLAG_RXC_Msk           (_U_(0x1) << SERCOM_SPIS_INTFLAG_RXC_Pos)            /**< (SERCOM_SPIS_INTFLAG) Receive Complete Interrupt Mask */
#define SERCOM_SPIS_INTFLAG_RXC(value)        (SERCOM_SPIS_INTFLAG_RXC_Msk & ((value) << SERCOM_SPIS_INTFLAG_RXC_Pos))
#define SERCOM_SPIS_INTFLAG_SSL_Pos           _U_(3)                                               /**< (SERCOM_SPIS_INTFLAG) Slave Select Low Interrupt Flag Position */
#define SERCOM_SPIS_INTFLAG_SSL_Msk           (_U_(0x1) << SERCOM_SPIS_INTFLAG_SSL_Pos)            /**< (SERCOM_SPIS_INTFLAG) Slave Select Low Interrupt Flag Mask */
#define SERCOM_SPIS_INTFLAG_SSL(value)        (SERCOM_SPIS_INTFLAG_SSL_Msk & ((value) << SERCOM_SPIS_INTFLAG_SSL_Pos))
#define SERCOM_SPIS_INTFLAG_ERROR_Pos         _U_(7)                                               /**< (SERCOM_SPIS_INTFLAG) Combined Error Interrupt Position */
#define SERCOM_SPIS_INTFLAG_ERROR_Msk         (_U_(0x1) << SERCOM_SPIS_INTFLAG_ERROR_Pos)          /**< (SERCOM_SPIS_INTFLAG) Combined Error Interrupt Mask */
#define SERCOM_SPIS_INTFLAG_ERROR(value)      (SERCOM_SPIS_INTFLAG_ERROR_Msk & ((value) << SERCOM_SPIS_INTFLAG_ERROR_Pos))
#define SERCOM_SPIS_INTFLAG_Msk               _U_(0x8F)                                            /**< (SERCOM_SPIS_INTFLAG) Register Mask  */


/* -------- SERCOM_USART_EXT_INTFLAG : (SERCOM Offset: 0x18) (R/W 8) USART_EXT Interrupt Flag Status and Clear -------- */
#define SERCOM_USART_EXT_INTFLAG_RESETVALUE   _U_(0x00)                                            /**<  (SERCOM_USART_EXT_INTFLAG) USART_EXT Interrupt Flag Status and Clear  Reset Value */

#define SERCOM_USART_EXT_INTFLAG_DRE_Pos      _U_(0)                                               /**< (SERCOM_USART_EXT_INTFLAG) Data Register Empty Interrupt Position */
#define SERCOM_USART_EXT_INTFLAG_DRE_Msk      (_U_(0x1) << SERCOM_USART_EXT_INTFLAG_DRE_Pos)       /**< (SERCOM_USART_EXT_INTFLAG) Data Register Empty Interrupt Mask */
#define SERCOM_USART_EXT_INTFLAG_DRE(value)   (SERCOM_USART_EXT_INTFLAG_DRE_Msk & ((value) << SERCOM_USART_EXT_INTFLAG_DRE_Pos))
#define SERCOM_USART_EXT_INTFLAG_TXC_Pos      _U_(1)                                               /**< (SERCOM_USART_EXT_INTFLAG) Transmit Complete Interrupt Position */
#define SERCOM_USART_EXT_INTFLAG_TXC_Msk      (_U_(0x1) << SERCOM_USART_EXT_INTFLAG_TXC_Pos)       /**< (SERCOM_USART_EXT_INTFLAG) Transmit Complete Interrupt Mask */
#define SERCOM_USART_EXT_INTFLAG_TXC(value)   (SERCOM_USART_EXT_INTFLAG_TXC_Msk & ((value) << SERCOM_USART_EXT_INTFLAG_TXC_Pos))
#define SERCOM_USART_EXT_INTFLAG_RXC_Pos      _U_(2)                                               /**< (SERCOM_USART_EXT_INTFLAG) Receive Complete Interrupt Position */
#define SERCOM_USART_EXT_INTFLAG_RXC_Msk      (_U_(0x1) << SERCOM_USART_EXT_INTFLAG_RXC_Pos)       /**< (SERCOM_USART_EXT_INTFLAG) Receive Complete Interrupt Mask */
#define SERCOM_USART_EXT_INTFLAG_RXC(value)   (SERCOM_USART_EXT_INTFLAG_RXC_Msk & ((value) << SERCOM_USART_EXT_INTFLAG_RXC_Pos))
#define SERCOM_USART_EXT_INTFLAG_RXS_Pos      _U_(3)                                               /**< (SERCOM_USART_EXT_INTFLAG) Receive Start Interrupt Position */
#define SERCOM_USART_EXT_INTFLAG_RXS_Msk      (_U_(0x1) << SERCOM_USART_EXT_INTFLAG_RXS_Pos)       /**< (SERCOM_USART_EXT_INTFLAG) Receive Start Interrupt Mask */
#define SERCOM_USART_EXT_INTFLAG_RXS(value)   (SERCOM_USART_EXT_INTFLAG_RXS_Msk & ((value) << SERCOM_USART_EXT_INTFLAG_RXS_Pos))
#define SERCOM_USART_EXT_INTFLAG_CTSIC_Pos    _U_(4)                                               /**< (SERCOM_USART_EXT_INTFLAG) Clear To Send Input Change Interrupt Position */
#define SERCOM_USART_EXT_INTFLAG_CTSIC_Msk    (_U_(0x1) << SERCOM_USART_EXT_INTFLAG_CTSIC_Pos)     /**< (SERCOM_USART_EXT_INTFLAG) Clear To Send Input Change Interrupt Mask */
#define SERCOM_USART_EXT_INTFLAG_CTSIC(value) (SERCOM_USART_EXT_INTFLAG_CTSIC_Msk & ((value) << SERCOM_USART_EXT_INTFLAG_CTSIC_Pos))
#define SERCOM_USART_EXT_INTFLAG_RXBRK_Pos    _U_(5)                                               /**< (SERCOM_USART_EXT_INTFLAG) Break Received Interrupt Position */
#define SERCOM_USART_EXT_INTFLAG_RXBRK_Msk    (_U_(0x1) << SERCOM_USART_EXT_INTFLAG_RXBRK_Pos)     /**< (SERCOM_USART_EXT_INTFLAG) Break Received Interrupt Mask */
#define SERCOM_USART_EXT_INTFLAG_RXBRK(value) (SERCOM_USART_EXT_INTFLAG_RXBRK_Msk & ((value) << SERCOM_USART_EXT_INTFLAG_RXBRK_Pos))
#define SERCOM_USART_EXT_INTFLAG_ERROR_Pos    _U_(7)                                               /**< (SERCOM_USART_EXT_INTFLAG) Combined Error Interrupt Position */
#define SERCOM_USART_EXT_INTFLAG_ERROR_Msk    (_U_(0x1) << SERCOM_USART_EXT_INTFLAG_ERROR_Pos)     /**< (SERCOM_USART_EXT_INTFLAG) Combined Error Interrupt Mask */
#define SERCOM_USART_EXT_INTFLAG_ERROR(value) (SERCOM_USART_EXT_INTFLAG_ERROR_Msk & ((value) << SERCOM_USART_EXT_INTFLAG_ERROR_Pos))
#define SERCOM_USART_EXT_INTFLAG_Msk          _U_(0xBF)                                            /**< (SERCOM_USART_EXT_INTFLAG) Register Mask  */


/* -------- SERCOM_USART_INT_INTFLAG : (SERCOM Offset: 0x18) (R/W 8) USART_INT Interrupt Flag Status and Clear -------- */
#define SERCOM_USART_INT_INTFLAG_RESETVALUE   _U_(0x00)                                            /**<  (SERCOM_USART_INT_INTFLAG) USART_INT Interrupt Flag Status and Clear  Reset Value */

#define SERCOM_USART_INT_INTFLAG_DRE_Pos      _U_(0)                                               /**< (SERCOM_USART_INT_INTFLAG) Data Register Empty Interrupt Position */
#define SERCOM_USART_INT_INTFLAG_DRE_Msk      (_U_(0x1) << SERCOM_USART_INT_INTFLAG_DRE_Pos)       /**< (SERCOM_USART_INT_INTFLAG) Data Register Empty Interrupt Mask */
#define SERCOM_USART_INT_INTFLAG_DRE(value)   (SERCOM_USART_INT_INTFLAG_DRE_Msk & ((value) << SERCOM_USART_INT_INTFLAG_DRE_Pos))
#define SERCOM_USART_INT_INTFLAG_TXC_Pos      _U_(1)                                               /**< (SERCOM_USART_INT_INTFLAG) Transmit Complete Interrupt Position */
#define SERCOM_USART_INT_INTFLAG_TXC_Msk      (_U_(0x1) << SERCOM_USART_INT_INTFLAG_TXC_Pos)       /**< (SERCOM_USART_INT_INTFLAG) Transmit Complete Interrupt Mask */
#define SERCOM_USART_INT_INTFLAG_TXC(value)   (SERCOM_USART_INT_INTFLAG_TXC_Msk & ((value) << SERCOM_USART_INT_INTFLAG_TXC_Pos))
#define SERCOM_USART_INT_INTFLAG_RXC_Pos      _U_(2)                                               /**< (SERCOM_USART_INT_INTFLAG) Receive Complete Interrupt Position */
#define SERCOM_USART_INT_INTFLAG_RXC_Msk      (_U_(0x1) << SERCOM_USART_INT_INTFLAG_RXC_Pos)       /**< (SERCOM_USART_INT_INTFLAG) Receive Complete Interrupt Mask */
#define SERCOM_USART_INT_INTFLAG_RXC(value)   (SERCOM_USART_INT_INTFLAG_RXC_Msk & ((value) << SERCOM_USART_INT_INTFLAG_RXC_Pos))
#define SERCOM_USART_INT_INTFLAG_RXS_Pos      _U_(3)                                               /**< (SERCOM_USART_INT_INTFLAG) Receive Start Interrupt Position */
#define SERCOM_USART_INT_INTFLAG_RXS_Msk      (_U_(0x1) << SERCOM_USART_INT_INTFLAG_RXS_Pos)       /**< (SERCOM_USART_INT_INTFLAG) Receive Start Interrupt Mask */
#define SERCOM_USART_INT_INTFLAG_RXS(value)   (SERCOM_USART_INT_INTFLAG_RXS_Msk & ((value) << SERCOM_USART_INT_INTFLAG_RXS_Pos))
#define SERCOM_USART_INT_INTFLAG_CTSIC_Pos    _U_(4)                                               /**< (SERCOM_USART_INT_INTFLAG) Clear To Send Input Change Interrupt Position */
#define SERCOM_USART_INT_INTFLAG_CTSIC_Msk    (_U_(0x1) << SERCOM_USART_INT_INTFLAG_CTSIC_Pos)     /**< (SERCOM_USART_INT_INTFLAG) Clear To Send Input Change Interrupt Mask */
#define SERCOM_USART_INT_INTFLAG_CTSIC(value) (SERCOM_USART_INT_INTFLAG_CTSIC_Msk & ((value) << SERCOM_USART_INT_INTFLAG_CTSIC_Pos))
#define SERCOM_USART_INT_INTFLAG_RXBRK_Pos    _U_(5)                                               /**< (SERCOM_USART_INT_INTFLAG) Break Received Interrupt Position */
#define SERCOM_USART_INT_INTFLAG_RXBRK_Msk    (_U_(0x1) << SERCOM_USART_INT_INTFLAG_RXBRK_Pos)     /**< (SERCOM_USART_INT_INTFLAG) Break Received Interrupt Mask */
#define SERCOM_USART_INT_INTFLAG_RXBRK(value) (SERCOM_USART_INT_INTFLAG_RXBRK_Msk & ((value) << SERCOM_USART_INT_INTFLAG_RXBRK_Pos))
#define SERCOM_USART_INT_INTFLAG_ERROR_Pos    _U_(7)                                               /**< (SERCOM_USART_INT_INTFLAG) Combined Error Interrupt Position */
#define SERCOM_USART_INT_INTFLAG_ERROR_Msk    (_U_(0x1) << SERCOM_USART_INT_INTFLAG_ERROR_Pos)     /**< (SERCOM_USART_INT_INTFLAG) Combined Error Interrupt Mask */
#define SERCOM_USART_INT_INTFLAG_ERROR(value) (SERCOM_USART_INT_INTFLAG_ERROR_Msk & ((value) << SERCOM_USART_INT_INTFLAG_ERROR_Pos))
#define SERCOM_USART_INT_INTFLAG_Msk          _U_(0xBF)                                            /**< (SERCOM_USART_INT_INTFLAG) Register Mask  */


/* -------- SERCOM_I2CM_STATUS : (SERCOM Offset: 0x1A) (R/W 16) I2CM Status -------- */
#define SERCOM_I2CM_STATUS_RESETVALUE         _U_(0x00)                                            /**<  (SERCOM_I2CM_STATUS) I2CM Status  Reset Value */

#define SERCOM_I2CM_STATUS_BUSERR_Pos         _U_(0)                                               /**< (SERCOM_I2CM_STATUS) Bus Error Position */
#define SERCOM_I2CM_STATUS_BUSERR_Msk         (_U_(0x1) << SERCOM_I2CM_STATUS_BUSERR_Pos)          /**< (SERCOM_I2CM_STATUS) Bus Error Mask */
#define SERCOM_I2CM_STATUS_BUSERR(value)      (SERCOM_I2CM_STATUS_BUSERR_Msk & ((value) << SERCOM_I2CM_STATUS_BUSERR_Pos))
#define SERCOM_I2CM_STATUS_ARBLOST_Pos        _U_(1)                                               /**< (SERCOM_I2CM_STATUS) Arbitration Lost Position */
#define SERCOM_I2CM_STATUS_ARBLOST_Msk        (_U_(0x1) << SERCOM_I2CM_STATUS_ARBLOST_Pos)         /**< (SERCOM_I2CM_STATUS) Arbitration Lost Mask */
#define SERCOM_I2CM_STATUS_ARBLOST(value)     (SERCOM_I2CM_STATUS_ARBLOST_Msk & ((value) << SERCOM_I2CM_STATUS_ARBLOST_Pos))
#define SERCOM_I2CM_STATUS_RXNACK_Pos         _U_(2)                                               /**< (SERCOM_I2CM_STATUS) Received Not Acknowledge Position */
#define SERCOM_I2CM_STATUS_RXNACK_Msk         (_U_(0x1) << SERCOM_I2CM_STATUS_RXNACK_Pos)          /**< (SERCOM_I2CM_STATUS) Received Not Acknowledge Mask */
#define SERCOM_I2CM_STATUS_RXNACK(value)      (SERCOM_I2CM_STATUS_RXNACK_Msk & ((value) << SERCOM_I2CM_STATUS_RXNACK_Pos))
#define SERCOM_I2CM_STATUS_BUSSTATE_Pos       _U_(4)                                               /**< (SERCOM_I2CM_STATUS) Bus State Position */
#define SERCOM_I2CM_STATUS_BUSSTATE_Msk       (_U_(0x3) << SERCOM_I2CM_STATUS_BUSSTATE_Pos)        /**< (SERCOM_I2CM_STATUS) Bus State Mask */
#define SERCOM_I2CM_STATUS_BUSSTATE(value)    (SERCOM_I2CM_STATUS_BUSSTATE_Msk & ((value) << SERCOM_I2CM_STATUS_BUSSTATE_Pos))
#define SERCOM_I2CM_STATUS_LOWTOUT_Pos        _U_(6)                                               /**< (SERCOM_I2CM_STATUS) SCL Low Timeout Position */
#define SERCOM_I2CM_STATUS_LOWTOUT_Msk        (_U_(0x1) << SERCOM_I2CM_STATUS_LOWTOUT_Pos)         /**< (SERCOM_I2CM_STATUS) SCL Low Timeout Mask */
#define SERCOM_I2CM_STATUS_LOWTOUT(value)     (SERCOM_I2CM_STATUS_LOWTOUT_Msk & ((value) << SERCOM_I2CM_STATUS_LOWTOUT_Pos))
#define SERCOM_I2CM_STATUS_CLKHOLD_Pos        _U_(7)                                               /**< (SERCOM_I2CM_STATUS) Clock Hold Position */
#define SERCOM_I2CM_STATUS_CLKHOLD_Msk        (_U_(0x1) << SERCOM_I2CM_STATUS_CLKHOLD_Pos)         /**< (SERCOM_I2CM_STATUS) Clock Hold Mask */
#define SERCOM_I2CM_STATUS_CLKHOLD(value)     (SERCOM_I2CM_STATUS_CLKHOLD_Msk & ((value) << SERCOM_I2CM_STATUS_CLKHOLD_Pos))
#define SERCOM_I2CM_STATUS_MEXTTOUT_Pos       _U_(8)                                               /**< (SERCOM_I2CM_STATUS) Master SCL Low Extend Timeout Position */
#define SERCOM_I2CM_STATUS_MEXTTOUT_Msk       (_U_(0x1) << SERCOM_I2CM_STATUS_MEXTTOUT_Pos)        /**< (SERCOM_I2CM_STATUS) Master SCL Low Extend Timeout Mask */
#define SERCOM_I2CM_STATUS_MEXTTOUT(value)    (SERCOM_I2CM_STATUS_MEXTTOUT_Msk & ((value) << SERCOM_I2CM_STATUS_MEXTTOUT_Pos))
#define SERCOM_I2CM_STATUS_SEXTTOUT_Pos       _U_(9)                                               /**< (SERCOM_I2CM_STATUS) Slave SCL Low Extend Timeout Position */
#define SERCOM_I2CM_STATUS_SEXTTOUT_Msk       (_U_(0x1) << SERCOM_I2CM_STATUS_SEXTTOUT_Pos)        /**< (SERCOM_I2CM_STATUS) Slave SCL Low Extend Timeout Mask */
#define SERCOM_I2CM_STATUS_SEXTTOUT(value)    (SERCOM_I2CM_STATUS_SEXTTOUT_Msk & ((value) << SERCOM_I2CM_STATUS_SEXTTOUT_Pos))
#define SERCOM_I2CM_STATUS_LENERR_Pos         _U_(10)                                              /**< (SERCOM_I2CM_STATUS) Length Error Position */
#define SERCOM_I2CM_STATUS_LENERR_Msk         (_U_(0x1) << SERCOM_I2CM_STATUS_LENERR_Pos)          /**< (SERCOM_I2CM_STATUS) Length Error Mask */
#define SERCOM_I2CM_STATUS_LENERR(value)      (SERCOM_I2CM_STATUS_LENERR_Msk & ((value) << SERCOM_I2CM_STATUS_LENERR_Pos))
#define SERCOM_I2CM_STATUS_Msk                _U_(0x07F7)                                          /**< (SERCOM_I2CM_STATUS) Register Mask  */


/* -------- SERCOM_I2CS_STATUS : (SERCOM Offset: 0x1A) (R/W 16) I2CS Status -------- */
#define SERCOM_I2CS_STATUS_RESETVALUE         _U_(0x00)                                            /**<  (SERCOM_I2CS_STATUS) I2CS Status  Reset Value */

#define SERCOM_I2CS_STATUS_BUSERR_Pos         _U_(0)                                               /**< (SERCOM_I2CS_STATUS) Bus Error Position */
#define SERCOM_I2CS_STATUS_BUSERR_Msk         (_U_(0x1) << SERCOM_I2CS_STATUS_BUSERR_Pos)          /**< (SERCOM_I2CS_STATUS) Bus Error Mask */
#define SERCOM_I2CS_STATUS_BUSERR(value)      (SERCOM_I2CS_STATUS_BUSERR_Msk & ((value) << SERCOM_I2CS_STATUS_BUSERR_Pos))
#define SERCOM_I2CS_STATUS_COLL_Pos           _U_(1)                                               /**< (SERCOM_I2CS_STATUS) Transmit Collision Position */
#define SERCOM_I2CS_STATUS_COLL_Msk           (_U_(0x1) << SERCOM_I2CS_STATUS_COLL_Pos)            /**< (SERCOM_I2CS_STATUS) Transmit Collision Mask */
#define SERCOM_I2CS_STATUS_COLL(value)        (SERCOM_I2CS_STATUS_COLL_Msk & ((value) << SERCOM_I2CS_STATUS_COLL_Pos))
#define SERCOM_I2CS_STATUS_RXNACK_Pos         _U_(2)                                               /**< (SERCOM_I2CS_STATUS) Received Not Acknowledge Position */
#define SERCOM_I2CS_STATUS_RXNACK_Msk         (_U_(0x1) << SERCOM_I2CS_STATUS_RXNACK_Pos)          /**< (SERCOM_I2CS_STATUS) Received Not Acknowledge Mask */
#define SERCOM_I2CS_STATUS_RXNACK(value)      (SERCOM_I2CS_STATUS_RXNACK_Msk & ((value) << SERCOM_I2CS_STATUS_RXNACK_Pos))
#define SERCOM_I2CS_STATUS_DIR_Pos            _U_(3)                                               /**< (SERCOM_I2CS_STATUS) Read/Write Direction Position */
#define SERCOM_I2CS_STATUS_DIR_Msk            (_U_(0x1) << SERCOM_I2CS_STATUS_DIR_Pos)             /**< (SERCOM_I2CS_STATUS) Read/Write Direction Mask */
#define SERCOM_I2CS_STATUS_DIR(value)         (SERCOM_I2CS_STATUS_DIR_Msk & ((value) << SERCOM_I2CS_STATUS_DIR_Pos))
#define SERCOM_I2CS_STATUS_SR_Pos             _U_(4)                                               /**< (SERCOM_I2CS_STATUS) Repeated Start Position */
#define SERCOM_I2CS_STATUS_SR_Msk             (_U_(0x1) << SERCOM_I2CS_STATUS_SR_Pos)              /**< (SERCOM_I2CS_STATUS) Repeated Start Mask */
#define SERCOM_I2CS_STATUS_SR(value)          (SERCOM_I2CS_STATUS_SR_Msk & ((value) << SERCOM_I2CS_STATUS_SR_Pos))
#define SERCOM_I2CS_STATUS_LOWTOUT_Pos        _U_(6)                                               /**< (SERCOM_I2CS_STATUS) SCL Low Timeout Position */
#define SERCOM_I2CS_STATUS_LOWTOUT_Msk        (_U_(0x1) << SERCOM_I2CS_STATUS_LOWTOUT_Pos)         /**< (SERCOM_I2CS_STATUS) SCL Low Timeout Mask */
#define SERCOM_I2CS_STATUS_LOWTOUT(value)     (SERCOM_I2CS_STATUS_LOWTOUT_Msk & ((value) << SERCOM_I2CS_STATUS_LOWTOUT_Pos))
#define SERCOM_I2CS_STATUS_CLKHOLD_Pos        _U_(7)                                               /**< (SERCOM_I2CS_STATUS) Clock Hold Position */
#define SERCOM_I2CS_STATUS_CLKHOLD_Msk        (_U_(0x1) << SERCOM_I2CS_STATUS_CLKHOLD_Pos)         /**< (SERCOM_I2CS_STATUS) Clock Hold Mask */
#define SERCOM_I2CS_STATUS_CLKHOLD(value)     (SERCOM_I2CS_STATUS_CLKHOLD_Msk & ((value) << SERCOM_I2CS_STATUS_CLKHOLD_Pos))
#define SERCOM_I2CS_STATUS_SEXTTOUT_Pos       _U_(9)                                               /**< (SERCOM_I2CS_STATUS) Slave SCL Low Extend Timeout Position */
#define SERCOM_I2CS_STATUS_SEXTTOUT_Msk       (_U_(0x1) << SERCOM_I2CS_STATUS_SEXTTOUT_Pos)        /**< (SERCOM_I2CS_STATUS) Slave SCL Low Extend Timeout Mask */
#define SERCOM_I2CS_STATUS_SEXTTOUT(value)    (SERCOM_I2CS_STATUS_SEXTTOUT_Msk & ((value) << SERCOM_I2CS_STATUS_SEXTTOUT_Pos))
#define SERCOM_I2CS_STATUS_HS_Pos             _U_(10)                                              /**< (SERCOM_I2CS_STATUS) High Speed Position */
#define SERCOM_I2CS_STATUS_HS_Msk             (_U_(0x1) << SERCOM_I2CS_STATUS_HS_Pos)              /**< (SERCOM_I2CS_STATUS) High Speed Mask */
#define SERCOM_I2CS_STATUS_HS(value)          (SERCOM_I2CS_STATUS_HS_Msk & ((value) << SERCOM_I2CS_STATUS_HS_Pos))
#define SERCOM_I2CS_STATUS_LENERR_Pos         _U_(11)                                              /**< (SERCOM_I2CS_STATUS) Transaction Length Error Position */
#define SERCOM_I2CS_STATUS_LENERR_Msk         (_U_(0x1) << SERCOM_I2CS_STATUS_LENERR_Pos)          /**< (SERCOM_I2CS_STATUS) Transaction Length Error Mask */
#define SERCOM_I2CS_STATUS_LENERR(value)      (SERCOM_I2CS_STATUS_LENERR_Msk & ((value) << SERCOM_I2CS_STATUS_LENERR_Pos))
#define SERCOM_I2CS_STATUS_Msk                _U_(0x0EDF)                                          /**< (SERCOM_I2CS_STATUS) Register Mask  */


/* -------- SERCOM_SPIM_STATUS : (SERCOM Offset: 0x1A) (R/W 16) SPIM Status -------- */
#define SERCOM_SPIM_STATUS_RESETVALUE         _U_(0x00)                                            /**<  (SERCOM_SPIM_STATUS) SPIM Status  Reset Value */

#define SERCOM_SPIM_STATUS_BUFOVF_Pos         _U_(2)                                               /**< (SERCOM_SPIM_STATUS) Buffer Overflow Position */
#define SERCOM_SPIM_STATUS_BUFOVF_Msk         (_U_(0x1) << SERCOM_SPIM_STATUS_BUFOVF_Pos)          /**< (SERCOM_SPIM_STATUS) Buffer Overflow Mask */
#define SERCOM_SPIM_STATUS_BUFOVF(value)      (SERCOM_SPIM_STATUS_BUFOVF_Msk & ((value) << SERCOM_SPIM_STATUS_BUFOVF_Pos))
#define SERCOM_SPIM_STATUS_LENERR_Pos         _U_(11)                                              /**< (SERCOM_SPIM_STATUS) Transaction Length Error Position */
#define SERCOM_SPIM_STATUS_LENERR_Msk         (_U_(0x1) << SERCOM_SPIM_STATUS_LENERR_Pos)          /**< (SERCOM_SPIM_STATUS) Transaction Length Error Mask */
#define SERCOM_SPIM_STATUS_LENERR(value)      (SERCOM_SPIM_STATUS_LENERR_Msk & ((value) << SERCOM_SPIM_STATUS_LENERR_Pos))
#define SERCOM_SPIM_STATUS_Msk                _U_(0x0804)                                          /**< (SERCOM_SPIM_STATUS) Register Mask  */


/* -------- SERCOM_SPIS_STATUS : (SERCOM Offset: 0x1A) (R/W 16) SPIS Status -------- */
#define SERCOM_SPIS_STATUS_RESETVALUE         _U_(0x00)                                            /**<  (SERCOM_SPIS_STATUS) SPIS Status  Reset Value */

#define SERCOM_SPIS_STATUS_BUFOVF_Pos         _U_(2)                                               /**< (SERCOM_SPIS_STATUS) Buffer Overflow Position */
#define SERCOM_SPIS_STATUS_BUFOVF_Msk         (_U_(0x1) << SERCOM_SPIS_STATUS_BUFOVF_Pos)          /**< (SERCOM_SPIS_STATUS) Buffer Overflow Mask */
#define SERCOM_SPIS_STATUS_BUFOVF(value)      (SERCOM_SPIS_STATUS_BUFOVF_Msk & ((value) << SERCOM_SPIS_STATUS_BUFOVF_Pos))
#define SERCOM_SPIS_STATUS_LENERR_Pos         _U_(11)                                              /**< (SERCOM_SPIS_STATUS) Transaction Length Error Position */
#define SERCOM_SPIS_STATUS_LENERR_Msk         (_U_(0x1) << SERCOM_SPIS_STATUS_LENERR_Pos)          /**< (SERCOM_SPIS_STATUS) Transaction Length Error Mask */
#define SERCOM_SPIS_STATUS_LENERR(value)      (SERCOM_SPIS_STATUS_LENERR_Msk & ((value) << SERCOM_SPIS_STATUS_LENERR_Pos))
#define SERCOM_SPIS_STATUS_Msk                _U_(0x0804)                                          /**< (SERCOM_SPIS_STATUS) Register Mask  */


/* -------- SERCOM_USART_EXT_STATUS : (SERCOM Offset: 0x1A) (R/W 16) USART_EXT Status -------- */
#define SERCOM_USART_EXT_STATUS_RESETVALUE    _U_(0x00)                                            /**<  (SERCOM_USART_EXT_STATUS) USART_EXT Status  Reset Value */

#define SERCOM_USART_EXT_STATUS_PERR_Pos      _U_(0)                                               /**< (SERCOM_USART_EXT_STATUS) Parity Error Position */
#define SERCOM_USART_EXT_STATUS_PERR_Msk      (_U_(0x1) << SERCOM_USART_EXT_STATUS_PERR_Pos)       /**< (SERCOM_USART_EXT_STATUS) Parity Error Mask */
#define SERCOM_USART_EXT_STATUS_PERR(value)   (SERCOM_USART_EXT_STATUS_PERR_Msk & ((value) << SERCOM_USART_EXT_STATUS_PERR_Pos))
#define SERCOM_USART_EXT_STATUS_FERR_Pos      _U_(1)                                               /**< (SERCOM_USART_EXT_STATUS) Frame Error Position */
#define SERCOM_USART_EXT_STATUS_FERR_Msk      (_U_(0x1) << SERCOM_USART_EXT_STATUS_FERR_Pos)       /**< (SERCOM_USART_EXT_STATUS) Frame Error Mask */
#define SERCOM_USART_EXT_STATUS_FERR(value)   (SERCOM_USART_EXT_STATUS_FERR_Msk & ((value) << SERCOM_USART_EXT_STATUS_FERR_Pos))
#define SERCOM_USART_EXT_STATUS_BUFOVF_Pos    _U_(2)                                               /**< (SERCOM_USART_EXT_STATUS) Buffer Overflow Position */
#define SERCOM_USART_EXT_STATUS_BUFOVF_Msk    (_U_(0x1) << SERCOM_USART_EXT_STATUS_BUFOVF_Pos)     /**< (SERCOM_USART_EXT_STATUS) Buffer Overflow Mask */
#define SERCOM_USART_EXT_STATUS_BUFOVF(value) (SERCOM_USART_EXT_STATUS_BUFOVF_Msk & ((value) << SERCOM_USART_EXT_STATUS_BUFOVF_Pos))
#define SERCOM_USART_EXT_STATUS_CTS_Pos       _U_(3)                                               /**< (SERCOM_USART_EXT_STATUS) Clear To Send Position */
#define SERCOM_USART_EXT_STATUS_CTS_Msk       (_U_(0x1) << SERCOM_USART_EXT_STATUS_CTS_Pos)        /**< (SERCOM_USART_EXT_STATUS) Clear To Send Mask */
#define SERCOM_USART_EXT_STATUS_CTS(value)    (SERCOM_USART_EXT_STATUS_CTS_Msk & ((value) << SERCOM_USART_EXT_STATUS_CTS_Pos))
#define SERCOM_USART_EXT_STATUS_ISF_Pos       _U_(4)                                               /**< (SERCOM_USART_EXT_STATUS) Inconsistent Sync Field Position */
#define SERCOM_USART_EXT_STATUS_ISF_Msk       (_U_(0x1) << SERCOM_USART_EXT_STATUS_ISF_Pos)        /**< (SERCOM_USART_EXT_STATUS) Inconsistent Sync Field Mask */
#define SERCOM_USART_EXT_STATUS_ISF(value)    (SERCOM_USART_EXT_STATUS_ISF_Msk & ((value) << SERCOM_USART_EXT_STATUS_ISF_Pos))
#define SERCOM_USART_EXT_STATUS_COLL_Pos      _U_(5)                                               /**< (SERCOM_USART_EXT_STATUS) Collision Detected Position */
#define SERCOM_USART_EXT_STATUS_COLL_Msk      (_U_(0x1) << SERCOM_USART_EXT_STATUS_COLL_Pos)       /**< (SERCOM_USART_EXT_STATUS) Collision Detected Mask */
#define SERCOM_USART_EXT_STATUS_COLL(value)   (SERCOM_USART_EXT_STATUS_COLL_Msk & ((value) << SERCOM_USART_EXT_STATUS_COLL_Pos))
#define SERCOM_USART_EXT_STATUS_TXE_Pos       _U_(6)                                               /**< (SERCOM_USART_EXT_STATUS) Transmitter Empty Position */
#define SERCOM_USART_EXT_STATUS_TXE_Msk       (_U_(0x1) << SERCOM_USART_EXT_STATUS_TXE_Pos)        /**< (SERCOM_USART_EXT_STATUS) Transmitter Empty Mask */
#define SERCOM_USART_EXT_STATUS_TXE(value)    (SERCOM_USART_EXT_STATUS_TXE_Msk & ((value) << SERCOM_USART_EXT_STATUS_TXE_Pos))
#define SERCOM_USART_EXT_STATUS_ITER_Pos      _U_(7)                                               /**< (SERCOM_USART_EXT_STATUS) Maximum Number of Repetitions Reached Position */
#define SERCOM_USART_EXT_STATUS_ITER_Msk      (_U_(0x1) << SERCOM_USART_EXT_STATUS_ITER_Pos)       /**< (SERCOM_USART_EXT_STATUS) Maximum Number of Repetitions Reached Mask */
#define SERCOM_USART_EXT_STATUS_ITER(value)   (SERCOM_USART_EXT_STATUS_ITER_Msk & ((value) << SERCOM_USART_EXT_STATUS_ITER_Pos))
#define SERCOM_USART_EXT_STATUS_Msk           _U_(0x00FF)                                          /**< (SERCOM_USART_EXT_STATUS) Register Mask  */


/* -------- SERCOM_USART_INT_STATUS : (SERCOM Offset: 0x1A) (R/W 16) USART_INT Status -------- */
#define SERCOM_USART_INT_STATUS_RESETVALUE    _U_(0x00)                                            /**<  (SERCOM_USART_INT_STATUS) USART_INT Status  Reset Value */

#define SERCOM_USART_INT_STATUS_PERR_Pos      _U_(0)                                               /**< (SERCOM_USART_INT_STATUS) Parity Error Position */
#define SERCOM_USART_INT_STATUS_PERR_Msk      (_U_(0x1) << SERCOM_USART_INT_STATUS_PERR_Pos)       /**< (SERCOM_USART_INT_STATUS) Parity Error Mask */
#define SERCOM_USART_INT_STATUS_PERR(value)   (SERCOM_USART_INT_STATUS_PERR_Msk & ((value) << SERCOM_USART_INT_STATUS_PERR_Pos))
#define SERCOM_USART_INT_STATUS_FERR_Pos      _U_(1)                                               /**< (SERCOM_USART_INT_STATUS) Frame Error Position */
#define SERCOM_USART_INT_STATUS_FERR_Msk      (_U_(0x1) << SERCOM_USART_INT_STATUS_FERR_Pos)       /**< (SERCOM_USART_INT_STATUS) Frame Error Mask */
#define SERCOM_USART_INT_STATUS_FERR(value)   (SERCOM_USART_INT_STATUS_FERR_Msk & ((value) << SERCOM_USART_INT_STATUS_FERR_Pos))
#define SERCOM_USART_INT_STATUS_BUFOVF_Pos    _U_(2)                                               /**< (SERCOM_USART_INT_STATUS) Buffer Overflow Position */
#define SERCOM_USART_INT_STATUS_BUFOVF_Msk    (_U_(0x1) << SERCOM_USART_INT_STATUS_BUFOVF_Pos)     /**< (SERCOM_USART_INT_STATUS) Buffer Overflow Mask */
#define SERCOM_USART_INT_STATUS_BUFOVF(value) (SERCOM_USART_INT_STATUS_BUFOVF_Msk & ((value) << SERCOM_USART_INT_STATUS_BUFOVF_Pos))
#define SERCOM_USART_INT_STATUS_CTS_Pos       _U_(3)                                               /**< (SERCOM_USART_INT_STATUS) Clear To Send Position */
#define SERCOM_USART_INT_STATUS_CTS_Msk       (_U_(0x1) << SERCOM_USART_INT_STATUS_CTS_Pos)        /**< (SERCOM_USART_INT_STATUS) Clear To Send Mask */
#define SERCOM_USART_INT_STATUS_CTS(value)    (SERCOM_USART_INT_STATUS_CTS_Msk & ((value) << SERCOM_USART_INT_STATUS_CTS_Pos))
#define SERCOM_USART_INT_STATUS_ISF_Pos       _U_(4)                                               /**< (SERCOM_USART_INT_STATUS) Inconsistent Sync Field Position */
#define SERCOM_USART_INT_STATUS_ISF_Msk       (_U_(0x1) << SERCOM_USART_INT_STATUS_ISF_Pos)        /**< (SERCOM_USART_INT_STATUS) Inconsistent Sync Field Mask */
#define SERCOM_USART_INT_STATUS_ISF(value)    (SERCOM_USART_INT_STATUS_ISF_Msk & ((value) << SERCOM_USART_INT_STATUS_ISF_Pos))
#define SERCOM_USART_INT_STATUS_COLL_Pos      _U_(5)                                               /**< (SERCOM_USART_INT_STATUS) Collision Detected Position */
#define SERCOM_USART_INT_STATUS_COLL_Msk      (_U_(0x1) << SERCOM_USART_INT_STATUS_COLL_Pos)       /**< (SERCOM_USART_INT_STATUS) Collision Detected Mask */
#define SERCOM_USART_INT_STATUS_COLL(value)   (SERCOM_USART_INT_STATUS_COLL_Msk & ((value) << SERCOM_USART_INT_STATUS_COLL_Pos))
#define SERCOM_USART_INT_STATUS_TXE_Pos       _U_(6)                                               /**< (SERCOM_USART_INT_STATUS) Transmitter Empty Position */
#define SERCOM_USART_INT_STATUS_TXE_Msk       (_U_(0x1) << SERCOM_USART_INT_STATUS_TXE_Pos)        /**< (SERCOM_USART_INT_STATUS) Transmitter Empty Mask */
#define SERCOM_USART_INT_STATUS_TXE(value)    (SERCOM_USART_INT_STATUS_TXE_Msk & ((value) << SERCOM_USART_INT_STATUS_TXE_Pos))
#define SERCOM_USART_INT_STATUS_ITER_Pos      _U_(7)                                               /**< (SERCOM_USART_INT_STATUS) Maximum Number of Repetitions Reached Position */
#define SERCOM_USART_INT_STATUS_ITER_Msk      (_U_(0x1) << SERCOM_USART_INT_STATUS_ITER_Pos)       /**< (SERCOM_USART_INT_STATUS) Maximum Number of Repetitions Reached Mask */
#define SERCOM_USART_INT_STATUS_ITER(value)   (SERCOM_USART_INT_STATUS_ITER_Msk & ((value) << SERCOM_USART_INT_STATUS_ITER_Pos))
#define SERCOM_USART_INT_STATUS_Msk           _U_(0x00FF)                                          /**< (SERCOM_USART_INT_STATUS) Register Mask  */


/* -------- SERCOM_I2CM_SYNCBUSY : (SERCOM Offset: 0x1C) ( R/ 32) I2CM Synchronization Busy -------- */
#define SERCOM_I2CM_SYNCBUSY_RESETVALUE       _U_(0x00)                                            /**<  (SERCOM_I2CM_SYNCBUSY) I2CM Synchronization Busy  Reset Value */

#define SERCOM_I2CM_SYNCBUSY_SWRST_Pos        _U_(0)                                               /**< (SERCOM_I2CM_SYNCBUSY) Software Reset Synchronization Busy Position */
#define SERCOM_I2CM_SYNCBUSY_SWRST_Msk        (_U_(0x1) << SERCOM_I2CM_SYNCBUSY_SWRST_Pos)         /**< (SERCOM_I2CM_SYNCBUSY) Software Reset Synchronization Busy Mask */
#define SERCOM_I2CM_SYNCBUSY_SWRST(value)     (SERCOM_I2CM_SYNCBUSY_SWRST_Msk & ((value) << SERCOM_I2CM_SYNCBUSY_SWRST_Pos))
#define SERCOM_I2CM_SYNCBUSY_ENABLE_Pos       _U_(1)                                               /**< (SERCOM_I2CM_SYNCBUSY) SERCOM Enable Synchronization Busy Position */
#define SERCOM_I2CM_SYNCBUSY_ENABLE_Msk       (_U_(0x1) << SERCOM_I2CM_SYNCBUSY_ENABLE_Pos)        /**< (SERCOM_I2CM_SYNCBUSY) SERCOM Enable Synchronization Busy Mask */
#define SERCOM_I2CM_SYNCBUSY_ENABLE(value)    (SERCOM_I2CM_SYNCBUSY_ENABLE_Msk & ((value) << SERCOM_I2CM_SYNCBUSY_ENABLE_Pos))
#define SERCOM_I2CM_SYNCBUSY_SYSOP_Pos        _U_(2)                                               /**< (SERCOM_I2CM_SYNCBUSY) System Operation Synchronization Busy Position */
#define SERCOM_I2CM_SYNCBUSY_SYSOP_Msk        (_U_(0x1) << SERCOM_I2CM_SYNCBUSY_SYSOP_Pos)         /**< (SERCOM_I2CM_SYNCBUSY) System Operation Synchronization Busy Mask */
#define SERCOM_I2CM_SYNCBUSY_SYSOP(value)     (SERCOM_I2CM_SYNCBUSY_SYSOP_Msk & ((value) << SERCOM_I2CM_SYNCBUSY_SYSOP_Pos))
#define SERCOM_I2CM_SYNCBUSY_LENGTH_Pos       _U_(4)                                               /**< (SERCOM_I2CM_SYNCBUSY) Length Synchronization Busy Position */
#define SERCOM_I2CM_SYNCBUSY_LENGTH_Msk       (_U_(0x1) << SERCOM_I2CM_SYNCBUSY_LENGTH_Pos)        /**< (SERCOM_I2CM_SYNCBUSY) Length Synchronization Busy Mask */
#define SERCOM_I2CM_SYNCBUSY_LENGTH(value)    (SERCOM_I2CM_SYNCBUSY_LENGTH_Msk & ((value) << SERCOM_I2CM_SYNCBUSY_LENGTH_Pos))
#define SERCOM_I2CM_SYNCBUSY_Msk              _U_(0x00000017)                                      /**< (SERCOM_I2CM_SYNCBUSY) Register Mask  */


/* -------- SERCOM_I2CS_SYNCBUSY : (SERCOM Offset: 0x1C) ( R/ 32) I2CS Synchronization Busy -------- */
#define SERCOM_I2CS_SYNCBUSY_RESETVALUE       _U_(0x00)                                            /**<  (SERCOM_I2CS_SYNCBUSY) I2CS Synchronization Busy  Reset Value */

#define SERCOM_I2CS_SYNCBUSY_SWRST_Pos        _U_(0)                                               /**< (SERCOM_I2CS_SYNCBUSY) Software Reset Synchronization Busy Position */
#define SERCOM_I2CS_SYNCBUSY_SWRST_Msk        (_U_(0x1) << SERCOM_I2CS_SYNCBUSY_SWRST_Pos)         /**< (SERCOM_I2CS_SYNCBUSY) Software Reset Synchronization Busy Mask */
#define SERCOM_I2CS_SYNCBUSY_SWRST(value)     (SERCOM_I2CS_SYNCBUSY_SWRST_Msk & ((value) << SERCOM_I2CS_SYNCBUSY_SWRST_Pos))
#define SERCOM_I2CS_SYNCBUSY_ENABLE_Pos       _U_(1)                                               /**< (SERCOM_I2CS_SYNCBUSY) SERCOM Enable Synchronization Busy Position */
#define SERCOM_I2CS_SYNCBUSY_ENABLE_Msk       (_U_(0x1) << SERCOM_I2CS_SYNCBUSY_ENABLE_Pos)        /**< (SERCOM_I2CS_SYNCBUSY) SERCOM Enable Synchronization Busy Mask */
#define SERCOM_I2CS_SYNCBUSY_ENABLE(value)    (SERCOM_I2CS_SYNCBUSY_ENABLE_Msk & ((value) << SERCOM_I2CS_SYNCBUSY_ENABLE_Pos))
#define SERCOM_I2CS_SYNCBUSY_LENGTH_Pos       _U_(4)                                               /**< (SERCOM_I2CS_SYNCBUSY) Length Synchronization Busy Position */
#define SERCOM_I2CS_SYNCBUSY_LENGTH_Msk       (_U_(0x1) << SERCOM_I2CS_SYNCBUSY_LENGTH_Pos)        /**< (SERCOM_I2CS_SYNCBUSY) Length Synchronization Busy Mask */
#define SERCOM_I2CS_SYNCBUSY_LENGTH(value)    (SERCOM_I2CS_SYNCBUSY_LENGTH_Msk & ((value) << SERCOM_I2CS_SYNCBUSY_LENGTH_Pos))
#define SERCOM_I2CS_SYNCBUSY_Msk              _U_(0x00000013)                                      /**< (SERCOM_I2CS_SYNCBUSY) Register Mask  */


/* -------- SERCOM_SPIM_SYNCBUSY : (SERCOM Offset: 0x1C) ( R/ 32) SPIM Synchronization Busy -------- */
#define SERCOM_SPIM_SYNCBUSY_RESETVALUE       _U_(0x00)                                            /**<  (SERCOM_SPIM_SYNCBUSY) SPIM Synchronization Busy  Reset Value */

#define SERCOM_SPIM_SYNCBUSY_SWRST_Pos        _U_(0)                                               /**< (SERCOM_SPIM_SYNCBUSY) Software Reset Synchronization Busy Position */
#define SERCOM_SPIM_SYNCBUSY_SWRST_Msk        (_U_(0x1) << SERCOM_SPIM_SYNCBUSY_SWRST_Pos)         /**< (SERCOM_SPIM_SYNCBUSY) Software Reset Synchronization Busy Mask */
#define SERCOM_SPIM_SYNCBUSY_SWRST(value)     (SERCOM_SPIM_SYNCBUSY_SWRST_Msk & ((value) << SERCOM_SPIM_SYNCBUSY_SWRST_Pos))
#define SERCOM_SPIM_SYNCBUSY_ENABLE_Pos       _U_(1)                                               /**< (SERCOM_SPIM_SYNCBUSY) SERCOM Enable Synchronization Busy Position */
#define SERCOM_SPIM_SYNCBUSY_ENABLE_Msk       (_U_(0x1) << SERCOM_SPIM_SYNCBUSY_ENABLE_Pos)        /**< (SERCOM_SPIM_SYNCBUSY) SERCOM Enable Synchronization Busy Mask */
#define SERCOM_SPIM_SYNCBUSY_ENABLE(value)    (SERCOM_SPIM_SYNCBUSY_ENABLE_Msk & ((value) << SERCOM_SPIM_SYNCBUSY_ENABLE_Pos))
#define SERCOM_SPIM_SYNCBUSY_CTRLB_Pos        _U_(2)                                               /**< (SERCOM_SPIM_SYNCBUSY) CTRLB Synchronization Busy Position */
#define SERCOM_SPIM_SYNCBUSY_CTRLB_Msk        (_U_(0x1) << SERCOM_SPIM_SYNCBUSY_CTRLB_Pos)         /**< (SERCOM_SPIM_SYNCBUSY) CTRLB Synchronization Busy Mask */
#define SERCOM_SPIM_SYNCBUSY_CTRLB(value)     (SERCOM_SPIM_SYNCBUSY_CTRLB_Msk & ((value) << SERCOM_SPIM_SYNCBUSY_CTRLB_Pos))
#define SERCOM_SPIM_SYNCBUSY_LENGTH_Pos       _U_(4)                                               /**< (SERCOM_SPIM_SYNCBUSY) LENGTH Synchronization Busy Position */
#define SERCOM_SPIM_SYNCBUSY_LENGTH_Msk       (_U_(0x1) << SERCOM_SPIM_SYNCBUSY_LENGTH_Pos)        /**< (SERCOM_SPIM_SYNCBUSY) LENGTH Synchronization Busy Mask */
#define SERCOM_SPIM_SYNCBUSY_LENGTH(value)    (SERCOM_SPIM_SYNCBUSY_LENGTH_Msk & ((value) << SERCOM_SPIM_SYNCBUSY_LENGTH_Pos))
#define SERCOM_SPIM_SYNCBUSY_Msk              _U_(0x00000017)                                      /**< (SERCOM_SPIM_SYNCBUSY) Register Mask  */


/* -------- SERCOM_SPIS_SYNCBUSY : (SERCOM Offset: 0x1C) ( R/ 32) SPIS Synchronization Busy -------- */
#define SERCOM_SPIS_SYNCBUSY_RESETVALUE       _U_(0x00)                                            /**<  (SERCOM_SPIS_SYNCBUSY) SPIS Synchronization Busy  Reset Value */

#define SERCOM_SPIS_SYNCBUSY_SWRST_Pos        _U_(0)                                               /**< (SERCOM_SPIS_SYNCBUSY) Software Reset Synchronization Busy Position */
#define SERCOM_SPIS_SYNCBUSY_SWRST_Msk        (_U_(0x1) << SERCOM_SPIS_SYNCBUSY_SWRST_Pos)         /**< (SERCOM_SPIS_SYNCBUSY) Software Reset Synchronization Busy Mask */
#define SERCOM_SPIS_SYNCBUSY_SWRST(value)     (SERCOM_SPIS_SYNCBUSY_SWRST_Msk & ((value) << SERCOM_SPIS_SYNCBUSY_SWRST_Pos))
#define SERCOM_SPIS_SYNCBUSY_ENABLE_Pos       _U_(1)                                               /**< (SERCOM_SPIS_SYNCBUSY) SERCOM Enable Synchronization Busy Position */
#define SERCOM_SPIS_SYNCBUSY_ENABLE_Msk       (_U_(0x1) << SERCOM_SPIS_SYNCBUSY_ENABLE_Pos)        /**< (SERCOM_SPIS_SYNCBUSY) SERCOM Enable Synchronization Busy Mask */
#define SERCOM_SPIS_SYNCBUSY_ENABLE(value)    (SERCOM_SPIS_SYNCBUSY_ENABLE_Msk & ((value) << SERCOM_SPIS_SYNCBUSY_ENABLE_Pos))
#define SERCOM_SPIS_SYNCBUSY_CTRLB_Pos        _U_(2)                                               /**< (SERCOM_SPIS_SYNCBUSY) CTRLB Synchronization Busy Position */
#define SERCOM_SPIS_SYNCBUSY_CTRLB_Msk        (_U_(0x1) << SERCOM_SPIS_SYNCBUSY_CTRLB_Pos)         /**< (SERCOM_SPIS_SYNCBUSY) CTRLB Synchronization Busy Mask */
#define SERCOM_SPIS_SYNCBUSY_CTRLB(value)     (SERCOM_SPIS_SYNCBUSY_CTRLB_Msk & ((value) << SERCOM_SPIS_SYNCBUSY_CTRLB_Pos))
#define SERCOM_SPIS_SYNCBUSY_LENGTH_Pos       _U_(4)                                               /**< (SERCOM_SPIS_SYNCBUSY) LENGTH Synchronization Busy Position */
#define SERCOM_SPIS_SYNCBUSY_LENGTH_Msk       (_U_(0x1) << SERCOM_SPIS_SYNCBUSY_LENGTH_Pos)        /**< (SERCOM_SPIS_SYNCBUSY) LENGTH Synchronization Busy Mask */
#define SERCOM_SPIS_SYNCBUSY_LENGTH(value)    (SERCOM_SPIS_SYNCBUSY_LENGTH_Msk & ((value) << SERCOM_SPIS_SYNCBUSY_LENGTH_Pos))
#define SERCOM_SPIS_SYNCBUSY_Msk              _U_(0x00000017)                                      /**< (SERCOM_SPIS_SYNCBUSY) Register Mask  */


/* -------- SERCOM_USART_EXT_SYNCBUSY : (SERCOM Offset: 0x1C) ( R/ 32) USART_EXT Synchronization Busy -------- */
#define SERCOM_USART_EXT_SYNCBUSY_RESETVALUE  _U_(0x00)                                            /**<  (SERCOM_USART_EXT_SYNCBUSY) USART_EXT Synchronization Busy  Reset Value */

#define SERCOM_USART_EXT_SYNCBUSY_SWRST_Pos   _U_(0)                                               /**< (SERCOM_USART_EXT_SYNCBUSY) Software Reset Synchronization Busy Position */
#define SERCOM_USART_EXT_SYNCBUSY_SWRST_Msk   (_U_(0x1) << SERCOM_USART_EXT_SYNCBUSY_SWRST_Pos)    /**< (SERCOM_USART_EXT_SYNCBUSY) Software Reset Synchronization Busy Mask */
#define SERCOM_USART_EXT_SYNCBUSY_SWRST(value) (SERCOM_USART_EXT_SYNCBUSY_SWRST_Msk & ((value) << SERCOM_USART_EXT_SYNCBUSY_SWRST_Pos))
#define SERCOM_USART_EXT_SYNCBUSY_ENABLE_Pos  _U_(1)                                               /**< (SERCOM_USART_EXT_SYNCBUSY) SERCOM Enable Synchronization Busy Position */
#define SERCOM_USART_EXT_SYNCBUSY_ENABLE_Msk  (_U_(0x1) << SERCOM_USART_EXT_SYNCBUSY_ENABLE_Pos)   /**< (SERCOM_USART_EXT_SYNCBUSY) SERCOM Enable Synchronization Busy Mask */
#define SERCOM_USART_EXT_SYNCBUSY_ENABLE(value) (SERCOM_USART_EXT_SYNCBUSY_ENABLE_Msk & ((value) << SERCOM_USART_EXT_SYNCBUSY_ENABLE_Pos))
#define SERCOM_USART_EXT_SYNCBUSY_CTRLB_Pos   _U_(2)                                               /**< (SERCOM_USART_EXT_SYNCBUSY) CTRLB Synchronization Busy Position */
#define SERCOM_USART_EXT_SYNCBUSY_CTRLB_Msk   (_U_(0x1) << SERCOM_USART_EXT_SYNCBUSY_CTRLB_Pos)    /**< (SERCOM_USART_EXT_SYNCBUSY) CTRLB Synchronization Busy Mask */
#define SERCOM_USART_EXT_SYNCBUSY_CTRLB(value) (SERCOM_USART_EXT_SYNCBUSY_CTRLB_Msk & ((value) << SERCOM_USART_EXT_SYNCBUSY_CTRLB_Pos))
#define SERCOM_USART_EXT_SYNCBUSY_RXERRCNT_Pos _U_(3)                                               /**< (SERCOM_USART_EXT_SYNCBUSY) RXERRCNT Synchronization Busy Position */
#define SERCOM_USART_EXT_SYNCBUSY_RXERRCNT_Msk (_U_(0x1) << SERCOM_USART_EXT_SYNCBUSY_RXERRCNT_Pos) /**< (SERCOM_USART_EXT_SYNCBUSY) RXERRCNT Synchronization Busy Mask */
#define SERCOM_USART_EXT_SYNCBUSY_RXERRCNT(value) (SERCOM_USART_EXT_SYNCBUSY_RXERRCNT_Msk & ((value) << SERCOM_USART_EXT_SYNCBUSY_RXERRCNT_Pos))
#define SERCOM_USART_EXT_SYNCBUSY_LENGTH_Pos  _U_(4)                                               /**< (SERCOM_USART_EXT_SYNCBUSY) LENGTH Synchronization Busy Position */
#define SERCOM_USART_EXT_SYNCBUSY_LENGTH_Msk  (_U_(0x1) << SERCOM_USART_EXT_SYNCBUSY_LENGTH_Pos)   /**< (SERCOM_USART_EXT_SYNCBUSY) LENGTH Synchronization Busy Mask */
#define SERCOM_USART_EXT_SYNCBUSY_LENGTH(value) (SERCOM_USART_EXT_SYNCBUSY_LENGTH_Msk & ((value) << SERCOM_USART_EXT_SYNCBUSY_LENGTH_Pos))
#define SERCOM_USART_EXT_SYNCBUSY_Msk         _U_(0x0000001F)                                      /**< (SERCOM_USART_EXT_SYNCBUSY) Register Mask  */


/* -------- SERCOM_USART_INT_SYNCBUSY : (SERCOM Offset: 0x1C) ( R/ 32) USART_INT Synchronization Busy -------- */
#define SERCOM_USART_INT_SYNCBUSY_RESETVALUE  _U_(0x00)                                            /**<  (SERCOM_USART_INT_SYNCBUSY) USART_INT Synchronization Busy  Reset Value */

#define SERCOM_USART_INT_SYNCBUSY_SWRST_Pos   _U_(0)                                               /**< (SERCOM_USART_INT_SYNCBUSY) Software Reset Synchronization Busy Position */
#define SERCOM_USART_INT_SYNCBUSY_SWRST_Msk   (_U_(0x1) << SERCOM_USART_INT_SYNCBUSY_SWRST_Pos)    /**< (SERCOM_USART_INT_SYNCBUSY) Software Reset Synchronization Busy Mask */
#define SERCOM_USART_INT_SYNCBUSY_SWRST(value) (SERCOM_USART_INT_SYNCBUSY_SWRST_Msk & ((value) << SERCOM_USART_INT_SYNCBUSY_SWRST_Pos))
#define SERCOM_USART_INT_SYNCBUSY_ENABLE_Pos  _U_(1)                                               /**< (SERCOM_USART_INT_SYNCBUSY) SERCOM Enable Synchronization Busy Position */
#define SERCOM_USART_INT_SYNCBUSY_ENABLE_Msk  (_U_(0x1) << SERCOM_USART_INT_SYNCBUSY_ENABLE_Pos)   /**< (SERCOM_USART_INT_SYNCBUSY) SERCOM Enable Synchronization Busy Mask */
#define SERCOM_USART_INT_SYNCBUSY_ENABLE(value) (SERCOM_USART_INT_SYNCBUSY_ENABLE_Msk & ((value) << SERCOM_USART_INT_SYNCBUSY_ENABLE_Pos))
#define SERCOM_USART_INT_SYNCBUSY_CTRLB_Pos   _U_(2)                                               /**< (SERCOM_USART_INT_SYNCBUSY) CTRLB Synchronization Busy Position */
#define SERCOM_USART_INT_SYNCBUSY_CTRLB_Msk   (_U_(0x1) << SERCOM_USART_INT_SYNCBUSY_CTRLB_Pos)    /**< (SERCOM_USART_INT_SYNCBUSY) CTRLB Synchronization Busy Mask */
#define SERCOM_USART_INT_SYNCBUSY_CTRLB(value) (SERCOM_USART_INT_SYNCBUSY_CTRLB_Msk & ((value) << SERCOM_USART_INT_SYNCBUSY_CTRLB_Pos))
#define SERCOM_USART_INT_SYNCBUSY_RXERRCNT_Pos _U_(3)                                               /**< (SERCOM_USART_INT_SYNCBUSY) RXERRCNT Synchronization Busy Position */
#define SERCOM_USART_INT_SYNCBUSY_RXERRCNT_Msk (_U_(0x1) << SERCOM_USART_INT_SYNCBUSY_RXERRCNT_Pos) /**< (SERCOM_USART_INT_SYNCBUSY) RXERRCNT Synchronization Busy Mask */
#define SERCOM_USART_INT_SYNCBUSY_RXERRCNT(value) (SERCOM_USART_INT_SYNCBUSY_RXERRCNT_Msk & ((value) << SERCOM_USART_INT_SYNCBUSY_RXERRCNT_Pos))
#define SERCOM_USART_INT_SYNCBUSY_LENGTH_Pos  _U_(4)                                               /**< (SERCOM_USART_INT_SYNCBUSY) LENGTH Synchronization Busy Position */
#define SERCOM_USART_INT_SYNCBUSY_LENGTH_Msk  (_U_(0x1) << SERCOM_USART_INT_SYNCBUSY_LENGTH_Pos)   /**< (SERCOM_USART_INT_SYNCBUSY) LENGTH Synchronization Busy Mask */
#define SERCOM_USART_INT_SYNCBUSY_LENGTH(value) (SERCOM_USART_INT_SYNCBUSY_LENGTH_Msk & ((value) << SERCOM_USART_INT_SYNCBUSY_LENGTH_Pos))
#define SERCOM_USART_INT_SYNCBUSY_Msk         _U_(0x0000001F)                                      /**< (SERCOM_USART_INT_SYNCBUSY) Register Mask  */


/* -------- SERCOM_USART_EXT_RXERRCNT : (SERCOM Offset: 0x20) ( R/ 8) USART_EXT Receive Error Count -------- */
#define SERCOM_USART_EXT_RXERRCNT_RESETVALUE  _U_(0x00)                                            /**<  (SERCOM_USART_EXT_RXERRCNT) USART_EXT Receive Error Count  Reset Value */

#define SERCOM_USART_EXT_RXERRCNT_Msk         _U_(0x00)                                            /**< (SERCOM_USART_EXT_RXERRCNT) Register Mask  */


/* -------- SERCOM_USART_INT_RXERRCNT : (SERCOM Offset: 0x20) ( R/ 8) USART_INT Receive Error Count -------- */
#define SERCOM_USART_INT_RXERRCNT_RESETVALUE  _U_(0x00)                                            /**<  (SERCOM_USART_INT_RXERRCNT) USART_INT Receive Error Count  Reset Value */

#define SERCOM_USART_INT_RXERRCNT_Msk         _U_(0x00)                                            /**< (SERCOM_USART_INT_RXERRCNT) Register Mask  */


/* -------- SERCOM_I2CS_LENGTH : (SERCOM Offset: 0x22) (R/W 16) I2CS Length -------- */
#define SERCOM_I2CS_LENGTH_RESETVALUE         _U_(0x00)                                            /**<  (SERCOM_I2CS_LENGTH) I2CS Length  Reset Value */

#define SERCOM_I2CS_LENGTH_LEN_Pos            _U_(0)                                               /**< (SERCOM_I2CS_LENGTH) Data Length Position */
#define SERCOM_I2CS_LENGTH_LEN_Msk            (_U_(0xFF) << SERCOM_I2CS_LENGTH_LEN_Pos)            /**< (SERCOM_I2CS_LENGTH) Data Length Mask */
#define SERCOM_I2CS_LENGTH_LEN(value)         (SERCOM_I2CS_LENGTH_LEN_Msk & ((value) << SERCOM_I2CS_LENGTH_LEN_Pos))
#define SERCOM_I2CS_LENGTH_LENEN_Pos          _U_(8)                                               /**< (SERCOM_I2CS_LENGTH) Data Length Enable Position */
#define SERCOM_I2CS_LENGTH_LENEN_Msk          (_U_(0x1) << SERCOM_I2CS_LENGTH_LENEN_Pos)           /**< (SERCOM_I2CS_LENGTH) Data Length Enable Mask */
#define SERCOM_I2CS_LENGTH_LENEN(value)       (SERCOM_I2CS_LENGTH_LENEN_Msk & ((value) << SERCOM_I2CS_LENGTH_LENEN_Pos))
#define SERCOM_I2CS_LENGTH_Msk                _U_(0x01FF)                                          /**< (SERCOM_I2CS_LENGTH) Register Mask  */


/* -------- SERCOM_SPIM_LENGTH : (SERCOM Offset: 0x22) (R/W 16) SPIM Length -------- */
#define SERCOM_SPIM_LENGTH_RESETVALUE         _U_(0x00)                                            /**<  (SERCOM_SPIM_LENGTH) SPIM Length  Reset Value */

#define SERCOM_SPIM_LENGTH_LEN_Pos            _U_(0)                                               /**< (SERCOM_SPIM_LENGTH) Data Length Position */
#define SERCOM_SPIM_LENGTH_LEN_Msk            (_U_(0xFF) << SERCOM_SPIM_LENGTH_LEN_Pos)            /**< (SERCOM_SPIM_LENGTH) Data Length Mask */
#define SERCOM_SPIM_LENGTH_LEN(value)         (SERCOM_SPIM_LENGTH_LEN_Msk & ((value) << SERCOM_SPIM_LENGTH_LEN_Pos))
#define SERCOM_SPIM_LENGTH_LENEN_Pos          _U_(8)                                               /**< (SERCOM_SPIM_LENGTH) Data Length Enable Position */
#define SERCOM_SPIM_LENGTH_LENEN_Msk          (_U_(0x1) << SERCOM_SPIM_LENGTH_LENEN_Pos)           /**< (SERCOM_SPIM_LENGTH) Data Length Enable Mask */
#define SERCOM_SPIM_LENGTH_LENEN(value)       (SERCOM_SPIM_LENGTH_LENEN_Msk & ((value) << SERCOM_SPIM_LENGTH_LENEN_Pos))
#define SERCOM_SPIM_LENGTH_Msk                _U_(0x01FF)                                          /**< (SERCOM_SPIM_LENGTH) Register Mask  */


/* -------- SERCOM_SPIS_LENGTH : (SERCOM Offset: 0x22) (R/W 16) SPIS Length -------- */
#define SERCOM_SPIS_LENGTH_RESETVALUE         _U_(0x00)                                            /**<  (SERCOM_SPIS_LENGTH) SPIS Length  Reset Value */

#define SERCOM_SPIS_LENGTH_LEN_Pos            _U_(0)                                               /**< (SERCOM_SPIS_LENGTH) Data Length Position */
#define SERCOM_SPIS_LENGTH_LEN_Msk            (_U_(0xFF) << SERCOM_SPIS_LENGTH_LEN_Pos)            /**< (SERCOM_SPIS_LENGTH) Data Length Mask */
#define SERCOM_SPIS_LENGTH_LEN(value)         (SERCOM_SPIS_LENGTH_LEN_Msk & ((value) << SERCOM_SPIS_LENGTH_LEN_Pos))
#define SERCOM_SPIS_LENGTH_LENEN_Pos          _U_(8)                                               /**< (SERCOM_SPIS_LENGTH) Data Length Enable Position */
#define SERCOM_SPIS_LENGTH_LENEN_Msk          (_U_(0x1) << SERCOM_SPIS_LENGTH_LENEN_Pos)           /**< (SERCOM_SPIS_LENGTH) Data Length Enable Mask */
#define SERCOM_SPIS_LENGTH_LENEN(value)       (SERCOM_SPIS_LENGTH_LENEN_Msk & ((value) << SERCOM_SPIS_LENGTH_LENEN_Pos))
#define SERCOM_SPIS_LENGTH_Msk                _U_(0x01FF)                                          /**< (SERCOM_SPIS_LENGTH) Register Mask  */


/* -------- SERCOM_USART_EXT_LENGTH : (SERCOM Offset: 0x22) (R/W 16) USART_EXT Length -------- */
#define SERCOM_USART_EXT_LENGTH_RESETVALUE    _U_(0x00)                                            /**<  (SERCOM_USART_EXT_LENGTH) USART_EXT Length  Reset Value */

#define SERCOM_USART_EXT_LENGTH_LEN_Pos       _U_(0)                                               /**< (SERCOM_USART_EXT_LENGTH) Data Length Position */
#define SERCOM_USART_EXT_LENGTH_LEN_Msk       (_U_(0xFF) << SERCOM_USART_EXT_LENGTH_LEN_Pos)       /**< (SERCOM_USART_EXT_LENGTH) Data Length Mask */
#define SERCOM_USART_EXT_LENGTH_LEN(value)    (SERCOM_USART_EXT_LENGTH_LEN_Msk & ((value) << SERCOM_USART_EXT_LENGTH_LEN_Pos))
#define SERCOM_USART_EXT_LENGTH_LENEN_Pos     _U_(8)                                               /**< (SERCOM_USART_EXT_LENGTH) Data Length Enable Position */
#define SERCOM_USART_EXT_LENGTH_LENEN_Msk     (_U_(0x3) << SERCOM_USART_EXT_LENGTH_LENEN_Pos)      /**< (SERCOM_USART_EXT_LENGTH) Data Length Enable Mask */
#define SERCOM_USART_EXT_LENGTH_LENEN(value)  (SERCOM_USART_EXT_LENGTH_LENEN_Msk & ((value) << SERCOM_USART_EXT_LENGTH_LENEN_Pos))
#define SERCOM_USART_EXT_LENGTH_Msk           _U_(0x03FF)                                          /**< (SERCOM_USART_EXT_LENGTH) Register Mask  */


/* -------- SERCOM_USART_INT_LENGTH : (SERCOM Offset: 0x22) (R/W 16) USART_INT Length -------- */
#define SERCOM_USART_INT_LENGTH_RESETVALUE    _U_(0x00)                                            /**<  (SERCOM_USART_INT_LENGTH) USART_INT Length  Reset Value */

#define SERCOM_USART_INT_LENGTH_LEN_Pos       _U_(0)                                               /**< (SERCOM_USART_INT_LENGTH) Data Length Position */
#define SERCOM_USART_INT_LENGTH_LEN_Msk       (_U_(0xFF) << SERCOM_USART_INT_LENGTH_LEN_Pos)       /**< (SERCOM_USART_INT_LENGTH) Data Length Mask */
#define SERCOM_USART_INT_LENGTH_LEN(value)    (SERCOM_USART_INT_LENGTH_LEN_Msk & ((value) << SERCOM_USART_INT_LENGTH_LEN_Pos))
#define SERCOM_USART_INT_LENGTH_LENEN_Pos     _U_(8)                                               /**< (SERCOM_USART_INT_LENGTH) Data Length Enable Position */
#define SERCOM_USART_INT_LENGTH_LENEN_Msk     (_U_(0x3) << SERCOM_USART_INT_LENGTH_LENEN_Pos)      /**< (SERCOM_USART_INT_LENGTH) Data Length Enable Mask */
#define SERCOM_USART_INT_LENGTH_LENEN(value)  (SERCOM_USART_INT_LENGTH_LENEN_Msk & ((value) << SERCOM_USART_INT_LENGTH_LENEN_Pos))
#define SERCOM_USART_INT_LENGTH_Msk           _U_(0x03FF)                                          /**< (SERCOM_USART_INT_LENGTH) Register Mask  */


/* -------- SERCOM_I2CM_ADDR : (SERCOM Offset: 0x24) (R/W 32) I2CM Address -------- */
#define SERCOM_I2CM_ADDR_RESETVALUE           _U_(0x00)                                            /**<  (SERCOM_I2CM_ADDR) I2CM Address  Reset Value */

#define SERCOM_I2CM_ADDR_ADDR_Pos             _U_(0)                                               /**< (SERCOM_I2CM_ADDR) Address Value Position */
#define SERCOM_I2CM_ADDR_ADDR_Msk             (_U_(0x7FF) << SERCOM_I2CM_ADDR_ADDR_Pos)            /**< (SERCOM_I2CM_ADDR) Address Value Mask */
#define SERCOM_I2CM_ADDR_ADDR(value)          (SERCOM_I2CM_ADDR_ADDR_Msk & ((value) << SERCOM_I2CM_ADDR_ADDR_Pos))
#define SERCOM_I2CM_ADDR_LENEN_Pos            _U_(13)                                              /**< (SERCOM_I2CM_ADDR) Length Enable Position */
#define SERCOM_I2CM_ADDR_LENEN_Msk            (_U_(0x1) << SERCOM_I2CM_ADDR_LENEN_Pos)             /**< (SERCOM_I2CM_ADDR) Length Enable Mask */
#define SERCOM_I2CM_ADDR_LENEN(value)         (SERCOM_I2CM_ADDR_LENEN_Msk & ((value) << SERCOM_I2CM_ADDR_LENEN_Pos))
#define SERCOM_I2CM_ADDR_HS_Pos               _U_(14)                                              /**< (SERCOM_I2CM_ADDR) High Speed Mode Position */
#define SERCOM_I2CM_ADDR_HS_Msk               (_U_(0x1) << SERCOM_I2CM_ADDR_HS_Pos)                /**< (SERCOM_I2CM_ADDR) High Speed Mode Mask */
#define SERCOM_I2CM_ADDR_HS(value)            (SERCOM_I2CM_ADDR_HS_Msk & ((value) << SERCOM_I2CM_ADDR_HS_Pos))
#define SERCOM_I2CM_ADDR_TENBITEN_Pos         _U_(15)                                              /**< (SERCOM_I2CM_ADDR) Ten Bit Addressing Enable Position */
#define SERCOM_I2CM_ADDR_TENBITEN_Msk         (_U_(0x1) << SERCOM_I2CM_ADDR_TENBITEN_Pos)          /**< (SERCOM_I2CM_ADDR) Ten Bit Addressing Enable Mask */
#define SERCOM_I2CM_ADDR_TENBITEN(value)      (SERCOM_I2CM_ADDR_TENBITEN_Msk & ((value) << SERCOM_I2CM_ADDR_TENBITEN_Pos))
#define SERCOM_I2CM_ADDR_LEN_Pos              _U_(16)                                              /**< (SERCOM_I2CM_ADDR) Length Position */
#define SERCOM_I2CM_ADDR_LEN_Msk              (_U_(0xFF) << SERCOM_I2CM_ADDR_LEN_Pos)              /**< (SERCOM_I2CM_ADDR) Length Mask */
#define SERCOM_I2CM_ADDR_LEN(value)           (SERCOM_I2CM_ADDR_LEN_Msk & ((value) << SERCOM_I2CM_ADDR_LEN_Pos))
#define SERCOM_I2CM_ADDR_Msk                  _U_(0x00FFE7FF)                                      /**< (SERCOM_I2CM_ADDR) Register Mask  */


/* -------- SERCOM_I2CS_ADDR : (SERCOM Offset: 0x24) (R/W 32) I2CS Address -------- */
#define SERCOM_I2CS_ADDR_RESETVALUE           _U_(0x00)                                            /**<  (SERCOM_I2CS_ADDR) I2CS Address  Reset Value */

#define SERCOM_I2CS_ADDR_GENCEN_Pos           _U_(0)                                               /**< (SERCOM_I2CS_ADDR) General Call Address Enable Position */
#define SERCOM_I2CS_ADDR_GENCEN_Msk           (_U_(0x1) << SERCOM_I2CS_ADDR_GENCEN_Pos)            /**< (SERCOM_I2CS_ADDR) General Call Address Enable Mask */
#define SERCOM_I2CS_ADDR_GENCEN(value)        (SERCOM_I2CS_ADDR_GENCEN_Msk & ((value) << SERCOM_I2CS_ADDR_GENCEN_Pos))
#define SERCOM_I2CS_ADDR_ADDR_Pos             _U_(1)                                               /**< (SERCOM_I2CS_ADDR) Address Value Position */
#define SERCOM_I2CS_ADDR_ADDR_Msk             (_U_(0x3FF) << SERCOM_I2CS_ADDR_ADDR_Pos)            /**< (SERCOM_I2CS_ADDR) Address Value Mask */
#define SERCOM_I2CS_ADDR_ADDR(value)          (SERCOM_I2CS_ADDR_ADDR_Msk & ((value) << SERCOM_I2CS_ADDR_ADDR_Pos))
#define SERCOM_I2CS_ADDR_TENBITEN_Pos         _U_(15)                                              /**< (SERCOM_I2CS_ADDR) Ten Bit Addressing Enable Position */
#define SERCOM_I2CS_ADDR_TENBITEN_Msk         (_U_(0x1) << SERCOM_I2CS_ADDR_TENBITEN_Pos)          /**< (SERCOM_I2CS_ADDR) Ten Bit Addressing Enable Mask */
#define SERCOM_I2CS_ADDR_TENBITEN(value)      (SERCOM_I2CS_ADDR_TENBITEN_Msk & ((value) << SERCOM_I2CS_ADDR_TENBITEN_Pos))
#define SERCOM_I2CS_ADDR_ADDRMASK_Pos         _U_(17)                                              /**< (SERCOM_I2CS_ADDR) Address Mask Position */
#define SERCOM_I2CS_ADDR_ADDRMASK_Msk         (_U_(0x3FF) << SERCOM_I2CS_ADDR_ADDRMASK_Pos)        /**< (SERCOM_I2CS_ADDR) Address Mask Mask */
#define SERCOM_I2CS_ADDR_ADDRMASK(value)      (SERCOM_I2CS_ADDR_ADDRMASK_Msk & ((value) << SERCOM_I2CS_ADDR_ADDRMASK_Pos))
#define SERCOM_I2CS_ADDR_Msk                  _U_(0x07FE87FF)                                      /**< (SERCOM_I2CS_ADDR) Register Mask  */


/* -------- SERCOM_SPIM_ADDR : (SERCOM Offset: 0x24) (R/W 32) SPIM Address -------- */
#define SERCOM_SPIM_ADDR_RESETVALUE           _U_(0x00)                                            /**<  (SERCOM_SPIM_ADDR) SPIM Address  Reset Value */

#define SERCOM_SPIM_ADDR_ADDR_Pos             _U_(0)                                               /**< (SERCOM_SPIM_ADDR) Address Value Position */
#define SERCOM_SPIM_ADDR_ADDR_Msk             (_U_(0xFF) << SERCOM_SPIM_ADDR_ADDR_Pos)             /**< (SERCOM_SPIM_ADDR) Address Value Mask */
#define SERCOM_SPIM_ADDR_ADDR(value)          (SERCOM_SPIM_ADDR_ADDR_Msk & ((value) << SERCOM_SPIM_ADDR_ADDR_Pos))
#define SERCOM_SPIM_ADDR_ADDRMASK_Pos         _U_(16)                                              /**< (SERCOM_SPIM_ADDR) Address Mask Position */
#define SERCOM_SPIM_ADDR_ADDRMASK_Msk         (_U_(0xFF) << SERCOM_SPIM_ADDR_ADDRMASK_Pos)         /**< (SERCOM_SPIM_ADDR) Address Mask Mask */
#define SERCOM_SPIM_ADDR_ADDRMASK(value)      (SERCOM_SPIM_ADDR_ADDRMASK_Msk & ((value) << SERCOM_SPIM_ADDR_ADDRMASK_Pos))
#define SERCOM_SPIM_ADDR_Msk                  _U_(0x00FF00FF)                                      /**< (SERCOM_SPIM_ADDR) Register Mask  */


/* -------- SERCOM_SPIS_ADDR : (SERCOM Offset: 0x24) (R/W 32) SPIS Address -------- */
#define SERCOM_SPIS_ADDR_RESETVALUE           _U_(0x00)                                            /**<  (SERCOM_SPIS_ADDR) SPIS Address  Reset Value */

#define SERCOM_SPIS_ADDR_ADDR_Pos             _U_(0)                                               /**< (SERCOM_SPIS_ADDR) Address Value Position */
#define SERCOM_SPIS_ADDR_ADDR_Msk             (_U_(0xFF) << SERCOM_SPIS_ADDR_ADDR_Pos)             /**< (SERCOM_SPIS_ADDR) Address Value Mask */
#define SERCOM_SPIS_ADDR_ADDR(value)          (SERCOM_SPIS_ADDR_ADDR_Msk & ((value) << SERCOM_SPIS_ADDR_ADDR_Pos))
#define SERCOM_SPIS_ADDR_ADDRMASK_Pos         _U_(16)                                              /**< (SERCOM_SPIS_ADDR) Address Mask Position */
#define SERCOM_SPIS_ADDR_ADDRMASK_Msk         (_U_(0xFF) << SERCOM_SPIS_ADDR_ADDRMASK_Pos)         /**< (SERCOM_SPIS_ADDR) Address Mask Mask */
#define SERCOM_SPIS_ADDR_ADDRMASK(value)      (SERCOM_SPIS_ADDR_ADDRMASK_Msk & ((value) << SERCOM_SPIS_ADDR_ADDRMASK_Pos))
#define SERCOM_SPIS_ADDR_Msk                  _U_(0x00FF00FF)                                      /**< (SERCOM_SPIS_ADDR) Register Mask  */


/* -------- SERCOM_I2CM_DATA : (SERCOM Offset: 0x28) (R/W 8) I2CM Data -------- */
#define SERCOM_I2CM_DATA_RESETVALUE           _U_(0x00)                                            /**<  (SERCOM_I2CM_DATA) I2CM Data  Reset Value */

#define SERCOM_I2CM_DATA_DATA_Pos             _U_(0)                                               /**< (SERCOM_I2CM_DATA) Data Value Position */
#define SERCOM_I2CM_DATA_DATA_Msk             (_U_(0xFF) << SERCOM_I2CM_DATA_DATA_Pos)             /**< (SERCOM_I2CM_DATA) Data Value Mask */
#define SERCOM_I2CM_DATA_DATA(value)          (SERCOM_I2CM_DATA_DATA_Msk & ((value) << SERCOM_I2CM_DATA_DATA_Pos))
#define SERCOM_I2CM_DATA_Msk                  _U_(0xFF)                                            /**< (SERCOM_I2CM_DATA) Register Mask  */


/* -------- SERCOM_I2CS_DATA : (SERCOM Offset: 0x28) (R/W 32) I2CS Data -------- */
#define SERCOM_I2CS_DATA_RESETVALUE           _U_(0x00)                                            /**<  (SERCOM_I2CS_DATA) I2CS Data  Reset Value */

#define SERCOM_I2CS_DATA_DATA_Pos             _U_(0)                                               /**< (SERCOM_I2CS_DATA) Data Value Position */
#define SERCOM_I2CS_DATA_DATA_Msk             (_U_(0xFFFFFFFF) << SERCOM_I2CS_DATA_DATA_Pos)       /**< (SERCOM_I2CS_DATA) Data Value Mask */
#define SERCOM_I2CS_DATA_DATA(value)          (SERCOM_I2CS_DATA_DATA_Msk & ((value) << SERCOM_I2CS_DATA_DATA_Pos))
#define SERCOM_I2CS_DATA_Msk                  _U_(0xFFFFFFFF)                                      /**< (SERCOM_I2CS_DATA) Register Mask  */


/* -------- SERCOM_SPIM_DATA : (SERCOM Offset: 0x28) (R/W 32) SPIM Data -------- */
#define SERCOM_SPIM_DATA_RESETVALUE           _U_(0x00)                                            /**<  (SERCOM_SPIM_DATA) SPIM Data  Reset Value */

#define SERCOM_SPIM_DATA_DATA_Pos             _U_(0)                                               /**< (SERCOM_SPIM_DATA) Data Value Position */
#define SERCOM_SPIM_DATA_DATA_Msk             (_U_(0xFFFFFFFF) << SERCOM_SPIM_DATA_DATA_Pos)       /**< (SERCOM_SPIM_DATA) Data Value Mask */
#define SERCOM_SPIM_DATA_DATA(value)          (SERCOM_SPIM_DATA_DATA_Msk & ((value) << SERCOM_SPIM_DATA_DATA_Pos))
#define SERCOM_SPIM_DATA_Msk                  _U_(0xFFFFFFFF)                                      /**< (SERCOM_SPIM_DATA) Register Mask  */


/* -------- SERCOM_SPIS_DATA : (SERCOM Offset: 0x28) (R/W 32) SPIS Data -------- */
#define SERCOM_SPIS_DATA_RESETVALUE           _U_(0x00)                                            /**<  (SERCOM_SPIS_DATA) SPIS Data  Reset Value */

#define SERCOM_SPIS_DATA_DATA_Pos             _U_(0)                                               /**< (SERCOM_SPIS_DATA) Data Value Position */
#define SERCOM_SPIS_DATA_DATA_Msk             (_U_(0xFFFFFFFF) << SERCOM_SPIS_DATA_DATA_Pos)       /**< (SERCOM_SPIS_DATA) Data Value Mask */
#define SERCOM_SPIS_DATA_DATA(value)          (SERCOM_SPIS_DATA_DATA_Msk & ((value) << SERCOM_SPIS_DATA_DATA_Pos))
#define SERCOM_SPIS_DATA_Msk                  _U_(0xFFFFFFFF)                                      /**< (SERCOM_SPIS_DATA) Register Mask  */


/* -------- SERCOM_USART_EXT_DATA : (SERCOM Offset: 0x28) (R/W 32) USART_EXT Data -------- */
#define SERCOM_USART_EXT_DATA_RESETVALUE      _U_(0x00)                                            /**<  (SERCOM_USART_EXT_DATA) USART_EXT Data  Reset Value */

#define SERCOM_USART_EXT_DATA_DATA_Pos        _U_(0)                                               /**< (SERCOM_USART_EXT_DATA) Data Value Position */
#define SERCOM_USART_EXT_DATA_DATA_Msk        (_U_(0xFFFFFFFF) << SERCOM_USART_EXT_DATA_DATA_Pos)  /**< (SERCOM_USART_EXT_DATA) Data Value Mask */
#define SERCOM_USART_EXT_DATA_DATA(value)     (SERCOM_USART_EXT_DATA_DATA_Msk & ((value) << SERCOM_USART_EXT_DATA_DATA_Pos))
#define SERCOM_USART_EXT_DATA_Msk             _U_(0xFFFFFFFF)                                      /**< (SERCOM_USART_EXT_DATA) Register Mask  */


/* -------- SERCOM_USART_INT_DATA : (SERCOM Offset: 0x28) (R/W 32) USART_INT Data -------- */
#define SERCOM_USART_INT_DATA_RESETVALUE      _U_(0x00)                                            /**<  (SERCOM_USART_INT_DATA) USART_INT Data  Reset Value */

#define SERCOM_USART_INT_DATA_DATA_Pos        _U_(0)                                               /**< (SERCOM_USART_INT_DATA) Data Value Position */
#define SERCOM_USART_INT_DATA_DATA_Msk        (_U_(0xFFFFFFFF) << SERCOM_USART_INT_DATA_DATA_Pos)  /**< (SERCOM_USART_INT_DATA) Data Value Mask */
#define SERCOM_USART_INT_DATA_DATA(value)     (SERCOM_USART_INT_DATA_DATA_Msk & ((value) << SERCOM_USART_INT_DATA_DATA_Pos))
#define SERCOM_USART_INT_DATA_Msk             _U_(0xFFFFFFFF)                                      /**< (SERCOM_USART_INT_DATA) Register Mask  */


/* -------- SERCOM_I2CM_DBGCTRL : (SERCOM Offset: 0x30) (R/W 8) I2CM Debug Control -------- */
#define SERCOM_I2CM_DBGCTRL_RESETVALUE        _U_(0x00)                                            /**<  (SERCOM_I2CM_DBGCTRL) I2CM Debug Control  Reset Value */

#define SERCOM_I2CM_DBGCTRL_DBGSTOP_Pos       _U_(0)                                               /**< (SERCOM_I2CM_DBGCTRL) Debug Mode Position */
#define SERCOM_I2CM_DBGCTRL_DBGSTOP_Msk       (_U_(0x1) << SERCOM_I2CM_DBGCTRL_DBGSTOP_Pos)        /**< (SERCOM_I2CM_DBGCTRL) Debug Mode Mask */
#define SERCOM_I2CM_DBGCTRL_DBGSTOP(value)    (SERCOM_I2CM_DBGCTRL_DBGSTOP_Msk & ((value) << SERCOM_I2CM_DBGCTRL_DBGSTOP_Pos))
#define SERCOM_I2CM_DBGCTRL_Msk               _U_(0x01)                                            /**< (SERCOM_I2CM_DBGCTRL) Register Mask  */


/* -------- SERCOM_SPIM_DBGCTRL : (SERCOM Offset: 0x30) (R/W 8) SPIM Debug Control -------- */
#define SERCOM_SPIM_DBGCTRL_RESETVALUE        _U_(0x00)                                            /**<  (SERCOM_SPIM_DBGCTRL) SPIM Debug Control  Reset Value */

#define SERCOM_SPIM_DBGCTRL_DBGSTOP_Pos       _U_(0)                                               /**< (SERCOM_SPIM_DBGCTRL) Debug Mode Position */
#define SERCOM_SPIM_DBGCTRL_DBGSTOP_Msk       (_U_(0x1) << SERCOM_SPIM_DBGCTRL_DBGSTOP_Pos)        /**< (SERCOM_SPIM_DBGCTRL) Debug Mode Mask */
#define SERCOM_SPIM_DBGCTRL_DBGSTOP(value)    (SERCOM_SPIM_DBGCTRL_DBGSTOP_Msk & ((value) << SERCOM_SPIM_DBGCTRL_DBGSTOP_Pos))
#define SERCOM_SPIM_DBGCTRL_Msk               _U_(0x01)                                            /**< (SERCOM_SPIM_DBGCTRL) Register Mask  */


/* -------- SERCOM_SPIS_DBGCTRL : (SERCOM Offset: 0x30) (R/W 8) SPIS Debug Control -------- */
#define SERCOM_SPIS_DBGCTRL_RESETVALUE        _U_(0x00)                                            /**<  (SERCOM_SPIS_DBGCTRL) SPIS Debug Control  Reset Value */

#define SERCOM_SPIS_DBGCTRL_DBGSTOP_Pos       _U_(0)                                               /**< (SERCOM_SPIS_DBGCTRL) Debug Mode Position */
#define SERCOM_SPIS_DBGCTRL_DBGSTOP_Msk       (_U_(0x1) << SERCOM_SPIS_DBGCTRL_DBGSTOP_Pos)        /**< (SERCOM_SPIS_DBGCTRL) Debug Mode Mask */
#define SERCOM_SPIS_DBGCTRL_DBGSTOP(value)    (SERCOM_SPIS_DBGCTRL_DBGSTOP_Msk & ((value) << SERCOM_SPIS_DBGCTRL_DBGSTOP_Pos))
#define SERCOM_SPIS_DBGCTRL_Msk               _U_(0x01)                                            /**< (SERCOM_SPIS_DBGCTRL) Register Mask  */


/* -------- SERCOM_USART_EXT_DBGCTRL : (SERCOM Offset: 0x30) (R/W 8) USART_EXT Debug Control -------- */
#define SERCOM_USART_EXT_DBGCTRL_RESETVALUE   _U_(0x00)                                            /**<  (SERCOM_USART_EXT_DBGCTRL) USART_EXT Debug Control  Reset Value */

#define SERCOM_USART_EXT_DBGCTRL_DBGSTOP_Pos  _U_(0)                                               /**< (SERCOM_USART_EXT_DBGCTRL) Debug Mode Position */
#define SERCOM_USART_EXT_DBGCTRL_DBGSTOP_Msk  (_U_(0x1) << SERCOM_USART_EXT_DBGCTRL_DBGSTOP_Pos)   /**< (SERCOM_USART_EXT_DBGCTRL) Debug Mode Mask */
#define SERCOM_USART_EXT_DBGCTRL_DBGSTOP(value) (SERCOM_USART_EXT_DBGCTRL_DBGSTOP_Msk & ((value) << SERCOM_USART_EXT_DBGCTRL_DBGSTOP_Pos))
#define SERCOM_USART_EXT_DBGCTRL_Msk          _U_(0x01)                                            /**< (SERCOM_USART_EXT_DBGCTRL) Register Mask  */


/* -------- SERCOM_USART_INT_DBGCTRL : (SERCOM Offset: 0x30) (R/W 8) USART_INT Debug Control -------- */
#define SERCOM_USART_INT_DBGCTRL_RESETVALUE   _U_(0x00)                                            /**<  (SERCOM_USART_INT_DBGCTRL) USART_INT Debug Control  Reset Value */

#define SERCOM_USART_INT_DBGCTRL_DBGSTOP_Pos  _U_(0)                                               /**< (SERCOM_USART_INT_DBGCTRL) Debug Mode Position */
#define SERCOM_USART_INT_DBGCTRL_DBGSTOP_Msk  (_U_(0x1) << SERCOM_USART_INT_DBGCTRL_DBGSTOP_Pos)   /**< (SERCOM_USART_INT_DBGCTRL) Debug Mode Mask */
#define SERCOM_USART_INT_DBGCTRL_DBGSTOP(value) (SERCOM_USART_INT_DBGCTRL_DBGSTOP_Msk & ((value) << SERCOM_USART_INT_DBGCTRL_DBGSTOP_Pos))
#define SERCOM_USART_INT_DBGCTRL_Msk          _U_(0x01)                                            /**< (SERCOM_USART_INT_DBGCTRL) Register Mask  */


/** \brief SERCOM register offsets definitions */
#define SERCOM_I2CM_CTRLA_REG_OFST     (0x00)              /**< (SERCOM_I2CM_CTRLA) I2CM Control A Offset */
#define SERCOM_I2CS_CTRLA_REG_OFST     (0x00)              /**< (SERCOM_I2CS_CTRLA) I2CS Control A Offset */
#define SERCOM_SPIM_CTRLA_REG_OFST     (0x00)              /**< (SERCOM_SPIM_CTRLA) SPIM Control A Offset */
#define SERCOM_SPIS_CTRLA_REG_OFST     (0x00)              /**< (SERCOM_SPIS_CTRLA) SPIS Control A Offset */
#define SERCOM_USART_EXT_CTRLA_REG_OFST (0x00)              /**< (SERCOM_USART_EXT_CTRLA) USART_EXT Control A Offset */
#define SERCOM_USART_INT_CTRLA_REG_OFST (0x00)              /**< (SERCOM_USART_INT_CTRLA) USART_INT Control A Offset */
#define SERCOM_I2CM_CTRLB_REG_OFST     (0x04)              /**< (SERCOM_I2CM_CTRLB) I2CM Control B Offset */
#define SERCOM_I2CS_CTRLB_REG_OFST     (0x04)              /**< (SERCOM_I2CS_CTRLB) I2CS Control B Offset */
#define SERCOM_SPIM_CTRLB_REG_OFST     (0x04)              /**< (SERCOM_SPIM_CTRLB) SPIM Control B Offset */
#define SERCOM_SPIS_CTRLB_REG_OFST     (0x04)              /**< (SERCOM_SPIS_CTRLB) SPIS Control B Offset */
#define SERCOM_USART_EXT_CTRLB_REG_OFST (0x04)              /**< (SERCOM_USART_EXT_CTRLB) USART_EXT Control B Offset */
#define SERCOM_USART_INT_CTRLB_REG_OFST (0x04)              /**< (SERCOM_USART_INT_CTRLB) USART_INT Control B Offset */
#define SERCOM_I2CM_CTRLC_REG_OFST     (0x08)              /**< (SERCOM_I2CM_CTRLC) I2CM Control C Offset */
#define SERCOM_I2CS_CTRLC_REG_OFST     (0x08)              /**< (SERCOM_I2CS_CTRLC) I2CS Control C Offset */
#define SERCOM_SPIM_CTRLC_REG_OFST     (0x08)              /**< (SERCOM_SPIM_CTRLC) SPIM Control C Offset */
#define SERCOM_SPIS_CTRLC_REG_OFST     (0x08)              /**< (SERCOM_SPIS_CTRLC) SPIS Control C Offset */
#define SERCOM_USART_EXT_CTRLC_REG_OFST (0x08)              /**< (SERCOM_USART_EXT_CTRLC) USART_EXT Control C Offset */
#define SERCOM_USART_INT_CTRLC_REG_OFST (0x08)              /**< (SERCOM_USART_INT_CTRLC) USART_INT Control C Offset */
#define SERCOM_I2CM_BAUD_REG_OFST      (0x0C)              /**< (SERCOM_I2CM_BAUD) I2CM Baud Rate Offset */
#define SERCOM_SPIM_BAUD_REG_OFST      (0x0C)              /**< (SERCOM_SPIM_BAUD) SPIM Baud Rate Offset */
#define SERCOM_SPIS_BAUD_REG_OFST      (0x0C)              /**< (SERCOM_SPIS_BAUD) SPIS Baud Rate Offset */
#define SERCOM_USART_EXT_BAUD_REG_OFST (0x0C)              /**< (SERCOM_USART_EXT_BAUD) USART_EXT Baud Rate Offset */
#define SERCOM_USART_INT_BAUD_REG_OFST (0x0C)              /**< (SERCOM_USART_INT_BAUD) USART_INT Baud Rate Offset */
#define SERCOM_USART_EXT_RXPL_REG_OFST (0x0E)              /**< (SERCOM_USART_EXT_RXPL) USART_EXT Receive Pulse Length Offset */
#define SERCOM_USART_INT_RXPL_REG_OFST (0x0E)              /**< (SERCOM_USART_INT_RXPL) USART_INT Receive Pulse Length Offset */
#define SERCOM_I2CM_INTENCLR_REG_OFST  (0x14)              /**< (SERCOM_I2CM_INTENCLR) I2CM Interrupt Enable Clear Offset */
#define SERCOM_I2CS_INTENCLR_REG_OFST  (0x14)              /**< (SERCOM_I2CS_INTENCLR) I2CS Interrupt Enable Clear Offset */
#define SERCOM_SPIM_INTENCLR_REG_OFST  (0x14)              /**< (SERCOM_SPIM_INTENCLR) SPIM Interrupt Enable Clear Offset */
#define SERCOM_SPIS_INTENCLR_REG_OFST  (0x14)              /**< (SERCOM_SPIS_INTENCLR) SPIS Interrupt Enable Clear Offset */
#define SERCOM_USART_EXT_INTENCLR_REG_OFST (0x14)              /**< (SERCOM_USART_EXT_INTENCLR) USART_EXT Interrupt Enable Clear Offset */
#define SERCOM_USART_INT_INTENCLR_REG_OFST (0x14)              /**< (SERCOM_USART_INT_INTENCLR) USART_INT Interrupt Enable Clear Offset */
#define SERCOM_I2CM_INTENSET_REG_OFST  (0x16)              /**< (SERCOM_I2CM_INTENSET) I2CM Interrupt Enable Set Offset */
#define SERCOM_I2CS_INTENSET_REG_OFST  (0x16)              /**< (SERCOM_I2CS_INTENSET) I2CS Interrupt Enable Set Offset */
#define SERCOM_SPIM_INTENSET_REG_OFST  (0x16)              /**< (SERCOM_SPIM_INTENSET) SPIM Interrupt Enable Set Offset */
#define SERCOM_SPIS_INTENSET_REG_OFST  (0x16)              /**< (SERCOM_SPIS_INTENSET) SPIS Interrupt Enable Set Offset */
#define SERCOM_USART_EXT_INTENSET_REG_OFST (0x16)              /**< (SERCOM_USART_EXT_INTENSET) USART_EXT Interrupt Enable Set Offset */
#define SERCOM_USART_INT_INTENSET_REG_OFST (0x16)              /**< (SERCOM_USART_INT_INTENSET) USART_INT Interrupt Enable Set Offset */
#define SERCOM_I2CM_INTFLAG_REG_OFST   (0x18)              /**< (SERCOM_I2CM_INTFLAG) I2CM Interrupt Flag Status and Clear Offset */
#define SERCOM_I2CS_INTFLAG_REG_OFST   (0x18)              /**< (SERCOM_I2CS_INTFLAG) I2CS Interrupt Flag Status and Clear Offset */
#define SERCOM_SPIM_INTFLAG_REG_OFST   (0x18)              /**< (SERCOM_SPIM_INTFLAG) SPIM Interrupt Flag Status and Clear Offset */
#define SERCOM_SPIS_INTFLAG_REG_OFST   (0x18)              /**< (SERCOM_SPIS_INTFLAG) SPIS Interrupt Flag Status and Clear Offset */
#define SERCOM_USART_EXT_INTFLAG_REG_OFST (0x18)              /**< (SERCOM_USART_EXT_INTFLAG) USART_EXT Interrupt Flag Status and Clear Offset */
#define SERCOM_USART_INT_INTFLAG_REG_OFST (0x18)              /**< (SERCOM_USART_INT_INTFLAG) USART_INT Interrupt Flag Status and Clear Offset */
#define SERCOM_I2CM_STATUS_REG_OFST    (0x1A)              /**< (SERCOM_I2CM_STATUS) I2CM Status Offset */
#define SERCOM_I2CS_STATUS_REG_OFST    (0x1A)              /**< (SERCOM_I2CS_STATUS) I2CS Status Offset */
#define SERCOM_SPIM_STATUS_REG_OFST    (0x1A)              /**< (SERCOM_SPIM_STATUS) SPIM Status Offset */
#define SERCOM_SPIS_STATUS_REG_OFST    (0x1A)              /**< (SERCOM_SPIS_STATUS) SPIS Status Offset */
#define SERCOM_USART_EXT_STATUS_REG_OFST (0x1A)              /**< (SERCOM_USART_EXT_STATUS) USART_EXT Status Offset */
#define SERCOM_USART_INT_STATUS_REG_OFST (0x1A)              /**< (SERCOM_USART_INT_STATUS) USART_INT Status Offset */
#define SERCOM_I2CM_SYNCBUSY_REG_OFST  (0x1C)              /**< (SERCOM_I2CM_SYNCBUSY) I2CM Synchronization Busy Offset */
#define SERCOM_I2CS_SYNCBUSY_REG_OFST  (0x1C)              /**< (SERCOM_I2CS_SYNCBUSY) I2CS Synchronization Busy Offset */
#define SERCOM_SPIM_SYNCBUSY_REG_OFST  (0x1C)              /**< (SERCOM_SPIM_SYNCBUSY) SPIM Synchronization Busy Offset */
#define SERCOM_SPIS_SYNCBUSY_REG_OFST  (0x1C)              /**< (SERCOM_SPIS_SYNCBUSY) SPIS Synchronization Busy Offset */
#define SERCOM_USART_EXT_SYNCBUSY_REG_OFST (0x1C)              /**< (SERCOM_USART_EXT_SYNCBUSY) USART_EXT Synchronization Busy Offset */
#define SERCOM_USART_INT_SYNCBUSY_REG_OFST (0x1C)              /**< (SERCOM_USART_INT_SYNCBUSY) USART_INT Synchronization Busy Offset */
#define SERCOM_USART_EXT_RXERRCNT_REG_OFST (0x20)              /**< (SERCOM_USART_EXT_RXERRCNT) USART_EXT Receive Error Count Offset */
#define SERCOM_USART_INT_RXERRCNT_REG_OFST (0x20)              /**< (SERCOM_USART_INT_RXERRCNT) USART_INT Receive Error Count Offset */
#define SERCOM_I2CS_LENGTH_REG_OFST    (0x22)              /**< (SERCOM_I2CS_LENGTH) I2CS Length Offset */
#define SERCOM_SPIM_LENGTH_REG_OFST    (0x22)              /**< (SERCOM_SPIM_LENGTH) SPIM Length Offset */
#define SERCOM_SPIS_LENGTH_REG_OFST    (0x22)              /**< (SERCOM_SPIS_LENGTH) SPIS Length Offset */
#define SERCOM_USART_EXT_LENGTH_REG_OFST (0x22)              /**< (SERCOM_USART_EXT_LENGTH) USART_EXT Length Offset */
#define SERCOM_USART_INT_LENGTH_REG_OFST (0x22)              /**< (SERCOM_USART_INT_LENGTH) USART_INT Length Offset */
#define SERCOM_I2CM_ADDR_REG_OFST      (0x24)              /**< (SERCOM_I2CM_ADDR) I2CM Address Offset */
#define SERCOM_I2CS_ADDR_REG_OFST      (0x24)              /**< (SERCOM_I2CS_ADDR) I2CS Address Offset */
#define SERCOM_SPIM_ADDR_REG_OFST      (0x24)              /**< (SERCOM_SPIM_ADDR) SPIM Address Offset */
#define SERCOM_SPIS_ADDR_REG_OFST      (0x24)              /**< (SERCOM_SPIS_ADDR) SPIS Address Offset */
#define SERCOM_I2CM_DATA_REG_OFST      (0x28)              /**< (SERCOM_I2CM_DATA) I2CM Data Offset */
#define SERCOM_I2CS_DATA_REG_OFST      (0x28)              /**< (SERCOM_I2CS_DATA) I2CS Data Offset */
#define SERCOM_SPIM_DATA_REG_OFST      (0x28)              /**< (SERCOM_SPIM_DATA) SPIM Data Offset */
#define SERCOM_SPIS_DATA_REG_OFST      (0x28)              /**< (SERCOM_SPIS_DATA) SPIS Data Offset */
#define SERCOM_USART_EXT_DATA_REG_OFST (0x28)              /**< (SERCOM_USART_EXT_DATA) USART_EXT Data Offset */
#define SERCOM_USART_INT_DATA_REG_OFST (0x28)              /**< (SERCOM_USART_INT_DATA) USART_INT Data Offset */
#define SERCOM_I2CM_DBGCTRL_REG_OFST   (0x30)              /**< (SERCOM_I2CM_DBGCTRL) I2CM Debug Control Offset */
#define SERCOM_SPIM_DBGCTRL_REG_OFST   (0x30)              /**< (SERCOM_SPIM_DBGCTRL) SPIM Debug Control Offset */
#define SERCOM_SPIS_DBGCTRL_REG_OFST   (0x30)              /**< (SERCOM_SPIS_DBGCTRL) SPIS Debug Control Offset */
#define SERCOM_USART_EXT_DBGCTRL_REG_OFST (0x30)              /**< (SERCOM_USART_EXT_DBGCTRL) USART_EXT Debug Control Offset */
#define SERCOM_USART_INT_DBGCTRL_REG_OFST (0x30)              /**< (SERCOM_USART_INT_DBGCTRL) USART_INT Debug Control Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief SERCOM register API structure */
typedef struct
{  /* Serial Communication Interface */
  __IO  uint32_t                       SERCOM_CTRLA;       /**< Offset: 0x00 (R/W  32) I2CM Control A */
  __IO  uint32_t                       SERCOM_CTRLB;       /**< Offset: 0x04 (R/W  32) I2CM Control B */
  __IO  uint32_t                       SERCOM_CTRLC;       /**< Offset: 0x08 (R/W  32) I2CM Control C */
  __IO  uint32_t                       SERCOM_BAUD;        /**< Offset: 0x0C (R/W  32) I2CM Baud Rate */
  __I   uint8_t                        Reserved1[0x04];
  __IO  uint8_t                        SERCOM_INTENCLR;    /**< Offset: 0x14 (R/W  8) I2CM Interrupt Enable Clear */
  __I   uint8_t                        Reserved2[0x01];
  __IO  uint8_t                        SERCOM_INTENSET;    /**< Offset: 0x16 (R/W  8) I2CM Interrupt Enable Set */
  __I   uint8_t                        Reserved3[0x01];
  __IO  uint8_t                        SERCOM_INTFLAG;     /**< Offset: 0x18 (R/W  8) I2CM Interrupt Flag Status and Clear */
  __I   uint8_t                        Reserved4[0x01];
  __IO  uint16_t                       SERCOM_STATUS;      /**< Offset: 0x1A (R/W  16) I2CM Status */
  __I   uint32_t                       SERCOM_SYNCBUSY;    /**< Offset: 0x1C (R/   32) I2CM Synchronization Busy */
  __I   uint8_t                        Reserved5[0x04];
  __IO  uint32_t                       SERCOM_ADDR;        /**< Offset: 0x24 (R/W  32) I2CM Address */
  __IO  uint8_t                        SERCOM_DATA;        /**< Offset: 0x28 (R/W  8) I2CM Data */
  __I   uint8_t                        Reserved6[0x07];
  __IO  uint8_t                        SERCOM_DBGCTRL;     /**< Offset: 0x30 (R/W  8) I2CM Debug Control */
} sercom_i2cm_registers_t;

/** \brief SERCOM register API structure */
typedef struct
{  /* Serial Communication Interface */
  __IO  uint32_t                       SERCOM_CTRLA;       /**< Offset: 0x00 (R/W  32) I2CS Control A */
  __IO  uint32_t                       SERCOM_CTRLB;       /**< Offset: 0x04 (R/W  32) I2CS Control B */
  __IO  uint32_t                       SERCOM_CTRLC;       /**< Offset: 0x08 (R/W  32) I2CS Control C */
  __I   uint8_t                        Reserved1[0x08];
  __IO  uint8_t                        SERCOM_INTENCLR;    /**< Offset: 0x14 (R/W  8) I2CS Interrupt Enable Clear */
  __I   uint8_t                        Reserved2[0x01];
  __IO  uint8_t                        SERCOM_INTENSET;    /**< Offset: 0x16 (R/W  8) I2CS Interrupt Enable Set */
  __I   uint8_t                        Reserved3[0x01];
  __IO  uint8_t                        SERCOM_INTFLAG;     /**< Offset: 0x18 (R/W  8) I2CS Interrupt Flag Status and Clear */
  __I   uint8_t                        Reserved4[0x01];
  __IO  uint16_t                       SERCOM_STATUS;      /**< Offset: 0x1A (R/W  16) I2CS Status */
  __I   uint32_t                       SERCOM_SYNCBUSY;    /**< Offset: 0x1C (R/   32) I2CS Synchronization Busy */
  __I   uint8_t                        Reserved5[0x02];
  __IO  uint16_t                       SERCOM_LENGTH;      /**< Offset: 0x22 (R/W  16) I2CS Length */
  __IO  uint32_t                       SERCOM_ADDR;        /**< Offset: 0x24 (R/W  32) I2CS Address */
  __IO  uint32_t                       SERCOM_DATA;        /**< Offset: 0x28 (R/W  32) I2CS Data */
} sercom_i2cs_registers_t;

/** \brief SERCOM register API structure */
typedef struct
{  /* Serial Communication Interface */
  __IO  uint32_t                       SERCOM_CTRLA;       /**< Offset: 0x00 (R/W  32) SPIS Control A */
  __IO  uint32_t                       SERCOM_CTRLB;       /**< Offset: 0x04 (R/W  32) SPIS Control B */
  __IO  uint32_t                       SERCOM_CTRLC;       /**< Offset: 0x08 (R/W  32) SPIS Control C */
  __IO  uint8_t                        SERCOM_BAUD;        /**< Offset: 0x0C (R/W  8) SPIS Baud Rate */
  __I   uint8_t                        Reserved1[0x07];
  __IO  uint8_t                        SERCOM_INTENCLR;    /**< Offset: 0x14 (R/W  8) SPIS Interrupt Enable Clear */
  __I   uint8_t                        Reserved2[0x01];
  __IO  uint8_t                        SERCOM_INTENSET;    /**< Offset: 0x16 (R/W  8) SPIS Interrupt Enable Set */
  __I   uint8_t                        Reserved3[0x01];
  __IO  uint8_t                        SERCOM_INTFLAG;     /**< Offset: 0x18 (R/W  8) SPIS Interrupt Flag Status and Clear */
  __I   uint8_t                        Reserved4[0x01];
  __IO  uint16_t                       SERCOM_STATUS;      /**< Offset: 0x1A (R/W  16) SPIS Status */
  __I   uint32_t                       SERCOM_SYNCBUSY;    /**< Offset: 0x1C (R/   32) SPIS Synchronization Busy */
  __I   uint8_t                        Reserved5[0x02];
  __IO  uint16_t                       SERCOM_LENGTH;      /**< Offset: 0x22 (R/W  16) SPIS Length */
  __IO  uint32_t                       SERCOM_ADDR;        /**< Offset: 0x24 (R/W  32) SPIS Address */
  __IO  uint32_t                       SERCOM_DATA;        /**< Offset: 0x28 (R/W  32) SPIS Data */
  __I   uint8_t                        Reserved6[0x04];
  __IO  uint8_t                        SERCOM_DBGCTRL;     /**< Offset: 0x30 (R/W  8) SPIS Debug Control */
} sercom_spis_registers_t;

/** \brief SERCOM register API structure */
typedef struct
{  /* Serial Communication Interface */
  __IO  uint32_t                       SERCOM_CTRLA;       /**< Offset: 0x00 (R/W  32) SPIM Control A */
  __IO  uint32_t                       SERCOM_CTRLB;       /**< Offset: 0x04 (R/W  32) SPIM Control B */
  __IO  uint32_t                       SERCOM_CTRLC;       /**< Offset: 0x08 (R/W  32) SPIM Control C */
  __IO  uint8_t                        SERCOM_BAUD;        /**< Offset: 0x0C (R/W  8) SPIM Baud Rate */
  __I   uint8_t                        Reserved1[0x07];
  __IO  uint8_t                        SERCOM_INTENCLR;    /**< Offset: 0x14 (R/W  8) SPIM Interrupt Enable Clear */
  __I   uint8_t                        Reserved2[0x01];
  __IO  uint8_t                        SERCOM_INTENSET;    /**< Offset: 0x16 (R/W  8) SPIM Interrupt Enable Set */
  __I   uint8_t                        Reserved3[0x01];
  __IO  uint8_t                        SERCOM_INTFLAG;     /**< Offset: 0x18 (R/W  8) SPIM Interrupt Flag Status and Clear */
  __I   uint8_t                        Reserved4[0x01];
  __IO  uint16_t                       SERCOM_STATUS;      /**< Offset: 0x1A (R/W  16) SPIM Status */
  __I   uint32_t                       SERCOM_SYNCBUSY;    /**< Offset: 0x1C (R/   32) SPIM Synchronization Busy */
  __I   uint8_t                        Reserved5[0x02];
  __IO  uint16_t                       SERCOM_LENGTH;      /**< Offset: 0x22 (R/W  16) SPIM Length */
  __IO  uint32_t                       SERCOM_ADDR;        /**< Offset: 0x24 (R/W  32) SPIM Address */
  __IO  uint32_t                       SERCOM_DATA;        /**< Offset: 0x28 (R/W  32) SPIM Data */
  __I   uint8_t                        Reserved6[0x04];
  __IO  uint8_t                        SERCOM_DBGCTRL;     /**< Offset: 0x30 (R/W  8) SPIM Debug Control */
} sercom_spim_registers_t;

/** \brief SERCOM register API structure */
typedef struct
{  /* Serial Communication Interface */
  __IO  uint32_t                       SERCOM_CTRLA;       /**< Offset: 0x00 (R/W  32) USART_EXT Control A */
  __IO  uint32_t                       SERCOM_CTRLB;       /**< Offset: 0x04 (R/W  32) USART_EXT Control B */
  __IO  uint32_t                       SERCOM_CTRLC;       /**< Offset: 0x08 (R/W  32) USART_EXT Control C */
  __IO  uint16_t                       SERCOM_BAUD;        /**< Offset: 0x0C (R/W  16) USART_EXT Baud Rate */
  __IO  uint8_t                        SERCOM_RXPL;        /**< Offset: 0x0E (R/W  8) USART_EXT Receive Pulse Length */
  __I   uint8_t                        Reserved1[0x05];
  __IO  uint8_t                        SERCOM_INTENCLR;    /**< Offset: 0x14 (R/W  8) USART_EXT Interrupt Enable Clear */
  __I   uint8_t                        Reserved2[0x01];
  __IO  uint8_t                        SERCOM_INTENSET;    /**< Offset: 0x16 (R/W  8) USART_EXT Interrupt Enable Set */
  __I   uint8_t                        Reserved3[0x01];
  __IO  uint8_t                        SERCOM_INTFLAG;     /**< Offset: 0x18 (R/W  8) USART_EXT Interrupt Flag Status and Clear */
  __I   uint8_t                        Reserved4[0x01];
  __IO  uint16_t                       SERCOM_STATUS;      /**< Offset: 0x1A (R/W  16) USART_EXT Status */
  __I   uint32_t                       SERCOM_SYNCBUSY;    /**< Offset: 0x1C (R/   32) USART_EXT Synchronization Busy */
  __I   uint8_t                        SERCOM_RXERRCNT;    /**< Offset: 0x20 (R/   8) USART_EXT Receive Error Count */
  __I   uint8_t                        Reserved5[0x01];
  __IO  uint16_t                       SERCOM_LENGTH;      /**< Offset: 0x22 (R/W  16) USART_EXT Length */
  __I   uint8_t                        Reserved6[0x04];
  __IO  uint32_t                       SERCOM_DATA;        /**< Offset: 0x28 (R/W  32) USART_EXT Data */
  __I   uint8_t                        Reserved7[0x04];
  __IO  uint8_t                        SERCOM_DBGCTRL;     /**< Offset: 0x30 (R/W  8) USART_EXT Debug Control */
} sercom_usart_ext_registers_t;

/** \brief SERCOM register API structure */
typedef struct
{  /* Serial Communication Interface */
  __IO  uint32_t                       SERCOM_CTRLA;       /**< Offset: 0x00 (R/W  32) USART_INT Control A */
  __IO  uint32_t                       SERCOM_CTRLB;       /**< Offset: 0x04 (R/W  32) USART_INT Control B */
  __IO  uint32_t                       SERCOM_CTRLC;       /**< Offset: 0x08 (R/W  32) USART_INT Control C */
  __IO  uint16_t                       SERCOM_BAUD;        /**< Offset: 0x0C (R/W  16) USART_INT Baud Rate */
  __IO  uint8_t                        SERCOM_RXPL;        /**< Offset: 0x0E (R/W  8) USART_INT Receive Pulse Length */
  __I   uint8_t                        Reserved1[0x05];
  __IO  uint8_t                        SERCOM_INTENCLR;    /**< Offset: 0x14 (R/W  8) USART_INT Interrupt Enable Clear */
  __I   uint8_t                        Reserved2[0x01];
  __IO  uint8_t                        SERCOM_INTENSET;    /**< Offset: 0x16 (R/W  8) USART_INT Interrupt Enable Set */
  __I   uint8_t                        Reserved3[0x01];
  __IO  uint8_t                        SERCOM_INTFLAG;     /**< Offset: 0x18 (R/W  8) USART_INT Interrupt Flag Status and Clear */
  __I   uint8_t                        Reserved4[0x01];
  __IO  uint16_t                       SERCOM_STATUS;      /**< Offset: 0x1A (R/W  16) USART_INT Status */
  __I   uint32_t                       SERCOM_SYNCBUSY;    /**< Offset: 0x1C (R/   32) USART_INT Synchronization Busy */
  __I   uint8_t                        SERCOM_RXERRCNT;    /**< Offset: 0x20 (R/   8) USART_INT Receive Error Count */
  __I   uint8_t                        Reserved5[0x01];
  __IO  uint16_t                       SERCOM_LENGTH;      /**< Offset: 0x22 (R/W  16) USART_INT Length */
  __I   uint8_t                        Reserved6[0x04];
  __IO  uint32_t                       SERCOM_DATA;        /**< Offset: 0x28 (R/W  32) USART_INT Data */
  __I   uint8_t                        Reserved7[0x04];
  __IO  uint8_t                        SERCOM_DBGCTRL;     /**< Offset: 0x30 (R/W  8) USART_INT Debug Control */
} sercom_usart_int_registers_t;

/** \brief SERCOM hardware registers */
typedef union
{  /* Serial Communication Interface */
       sercom_i2cm_registers_t        I2CM;           /**< I2C Master Mode */
       sercom_i2cs_registers_t        I2CS;           /**< I2C Slave Mode */
       sercom_spis_registers_t        SPIS;           /**< SPI Slave Mode */
       sercom_spim_registers_t        SPIM;           /**< SPI Master Mode */
       sercom_usart_ext_registers_t   USART_EXT;      /**< USART EXTERNAL CLOCK Mode */
       sercom_usart_int_registers_t   USART_INT;      /**< USART INTERNAL CLOCK Mode */
} sercom_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_SERCOM_COMPONENT_H_ */
