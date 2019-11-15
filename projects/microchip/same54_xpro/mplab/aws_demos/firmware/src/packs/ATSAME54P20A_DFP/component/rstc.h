/**
 * \brief Component description for RSTC
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
#ifndef _SAME54_RSTC_COMPONENT_H_
#define _SAME54_RSTC_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR RSTC                                         */
/* ************************************************************************** */

/* -------- RSTC_RCAUSE : (RSTC Offset: 0x00) ( R/ 8) Reset Cause -------- */
#define RSTC_RCAUSE_POR_Pos                   _U_(0)                                               /**< (RSTC_RCAUSE) Power On Reset Position */
#define RSTC_RCAUSE_POR_Msk                   (_U_(0x1) << RSTC_RCAUSE_POR_Pos)                    /**< (RSTC_RCAUSE) Power On Reset Mask */
#define RSTC_RCAUSE_POR(value)                (RSTC_RCAUSE_POR_Msk & ((value) << RSTC_RCAUSE_POR_Pos))
#define RSTC_RCAUSE_BODCORE_Pos               _U_(1)                                               /**< (RSTC_RCAUSE) Brown Out CORE Detector Reset Position */
#define RSTC_RCAUSE_BODCORE_Msk               (_U_(0x1) << RSTC_RCAUSE_BODCORE_Pos)                /**< (RSTC_RCAUSE) Brown Out CORE Detector Reset Mask */
#define RSTC_RCAUSE_BODCORE(value)            (RSTC_RCAUSE_BODCORE_Msk & ((value) << RSTC_RCAUSE_BODCORE_Pos))
#define RSTC_RCAUSE_BODVDD_Pos                _U_(2)                                               /**< (RSTC_RCAUSE) Brown Out VDD Detector Reset Position */
#define RSTC_RCAUSE_BODVDD_Msk                (_U_(0x1) << RSTC_RCAUSE_BODVDD_Pos)                 /**< (RSTC_RCAUSE) Brown Out VDD Detector Reset Mask */
#define RSTC_RCAUSE_BODVDD(value)             (RSTC_RCAUSE_BODVDD_Msk & ((value) << RSTC_RCAUSE_BODVDD_Pos))
#define RSTC_RCAUSE_NVM_Pos                   _U_(3)                                               /**< (RSTC_RCAUSE) NVM Reset Position */
#define RSTC_RCAUSE_NVM_Msk                   (_U_(0x1) << RSTC_RCAUSE_NVM_Pos)                    /**< (RSTC_RCAUSE) NVM Reset Mask */
#define RSTC_RCAUSE_NVM(value)                (RSTC_RCAUSE_NVM_Msk & ((value) << RSTC_RCAUSE_NVM_Pos))
#define RSTC_RCAUSE_EXT_Pos                   _U_(4)                                               /**< (RSTC_RCAUSE) External Reset Position */
#define RSTC_RCAUSE_EXT_Msk                   (_U_(0x1) << RSTC_RCAUSE_EXT_Pos)                    /**< (RSTC_RCAUSE) External Reset Mask */
#define RSTC_RCAUSE_EXT(value)                (RSTC_RCAUSE_EXT_Msk & ((value) << RSTC_RCAUSE_EXT_Pos))
#define RSTC_RCAUSE_WDT_Pos                   _U_(5)                                               /**< (RSTC_RCAUSE) Watchdog Reset Position */
#define RSTC_RCAUSE_WDT_Msk                   (_U_(0x1) << RSTC_RCAUSE_WDT_Pos)                    /**< (RSTC_RCAUSE) Watchdog Reset Mask */
#define RSTC_RCAUSE_WDT(value)                (RSTC_RCAUSE_WDT_Msk & ((value) << RSTC_RCAUSE_WDT_Pos))
#define RSTC_RCAUSE_SYST_Pos                  _U_(6)                                               /**< (RSTC_RCAUSE) System Reset Request Position */
#define RSTC_RCAUSE_SYST_Msk                  (_U_(0x1) << RSTC_RCAUSE_SYST_Pos)                   /**< (RSTC_RCAUSE) System Reset Request Mask */
#define RSTC_RCAUSE_SYST(value)               (RSTC_RCAUSE_SYST_Msk & ((value) << RSTC_RCAUSE_SYST_Pos))
#define RSTC_RCAUSE_BACKUP_Pos                _U_(7)                                               /**< (RSTC_RCAUSE) Backup Reset Position */
#define RSTC_RCAUSE_BACKUP_Msk                (_U_(0x1) << RSTC_RCAUSE_BACKUP_Pos)                 /**< (RSTC_RCAUSE) Backup Reset Mask */
#define RSTC_RCAUSE_BACKUP(value)             (RSTC_RCAUSE_BACKUP_Msk & ((value) << RSTC_RCAUSE_BACKUP_Pos))
#define RSTC_RCAUSE_Msk                       _U_(0xFF)                                            /**< (RSTC_RCAUSE) Register Mask  */


/* -------- RSTC_BKUPEXIT : (RSTC Offset: 0x02) ( R/ 8) Backup Exit Source -------- */
#define RSTC_BKUPEXIT_RESETVALUE              _U_(0x00)                                            /**<  (RSTC_BKUPEXIT) Backup Exit Source  Reset Value */

#define RSTC_BKUPEXIT_RTC_Pos                 _U_(1)                                               /**< (RSTC_BKUPEXIT) Real Timer Counter Interrupt Position */
#define RSTC_BKUPEXIT_RTC_Msk                 (_U_(0x1) << RSTC_BKUPEXIT_RTC_Pos)                  /**< (RSTC_BKUPEXIT) Real Timer Counter Interrupt Mask */
#define RSTC_BKUPEXIT_RTC(value)              (RSTC_BKUPEXIT_RTC_Msk & ((value) << RSTC_BKUPEXIT_RTC_Pos))
#define RSTC_BKUPEXIT_BBPS_Pos                _U_(2)                                               /**< (RSTC_BKUPEXIT) Battery Backup Power Switch Position */
#define RSTC_BKUPEXIT_BBPS_Msk                (_U_(0x1) << RSTC_BKUPEXIT_BBPS_Pos)                 /**< (RSTC_BKUPEXIT) Battery Backup Power Switch Mask */
#define RSTC_BKUPEXIT_BBPS(value)             (RSTC_BKUPEXIT_BBPS_Msk & ((value) << RSTC_BKUPEXIT_BBPS_Pos))
#define RSTC_BKUPEXIT_HIB_Pos                 _U_(7)                                               /**< (RSTC_BKUPEXIT) Hibernate Position */
#define RSTC_BKUPEXIT_HIB_Msk                 (_U_(0x1) << RSTC_BKUPEXIT_HIB_Pos)                  /**< (RSTC_BKUPEXIT) Hibernate Mask */
#define RSTC_BKUPEXIT_HIB(value)              (RSTC_BKUPEXIT_HIB_Msk & ((value) << RSTC_BKUPEXIT_HIB_Pos))
#define RSTC_BKUPEXIT_Msk                     _U_(0x86)                                            /**< (RSTC_BKUPEXIT) Register Mask  */


/** \brief RSTC register offsets definitions */
#define RSTC_RCAUSE_REG_OFST           (0x00)              /**< (RSTC_RCAUSE) Reset Cause Offset */
#define RSTC_BKUPEXIT_REG_OFST         (0x02)              /**< (RSTC_BKUPEXIT) Backup Exit Source Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief RSTC register API structure */
typedef struct
{  /* Reset Controller */
  __I   uint8_t                        RSTC_RCAUSE;        /**< Offset: 0x00 (R/   8) Reset Cause */
  __I   uint8_t                        Reserved1[0x01];
  __I   uint8_t                        RSTC_BKUPEXIT;      /**< Offset: 0x02 (R/   8) Backup Exit Source */
} rstc_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_RSTC_COMPONENT_H_ */
