/**
 * \brief Component description for PM
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
#ifndef _SAME54_PM_COMPONENT_H_
#define _SAME54_PM_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR PM                                           */
/* ************************************************************************** */

/* -------- PM_CTRLA : (PM Offset: 0x00) (R/W 8) Control A -------- */
#define PM_CTRLA_RESETVALUE                   _U_(0x00)                                            /**<  (PM_CTRLA) Control A  Reset Value */

#define PM_CTRLA_IORET_Pos                    _U_(2)                                               /**< (PM_CTRLA) I/O Retention Position */
#define PM_CTRLA_IORET_Msk                    (_U_(0x1) << PM_CTRLA_IORET_Pos)                     /**< (PM_CTRLA) I/O Retention Mask */
#define PM_CTRLA_IORET(value)                 (PM_CTRLA_IORET_Msk & ((value) << PM_CTRLA_IORET_Pos))
#define PM_CTRLA_Msk                          _U_(0x04)                                            /**< (PM_CTRLA) Register Mask  */


/* -------- PM_SLEEPCFG : (PM Offset: 0x01) (R/W 8) Sleep Configuration -------- */
#define PM_SLEEPCFG_RESETVALUE                _U_(0x02)                                            /**<  (PM_SLEEPCFG) Sleep Configuration  Reset Value */

#define PM_SLEEPCFG_SLEEPMODE_Pos             _U_(0)                                               /**< (PM_SLEEPCFG) Sleep Mode Position */
#define PM_SLEEPCFG_SLEEPMODE_Msk             (_U_(0x7) << PM_SLEEPCFG_SLEEPMODE_Pos)              /**< (PM_SLEEPCFG) Sleep Mode Mask */
#define PM_SLEEPCFG_SLEEPMODE(value)          (PM_SLEEPCFG_SLEEPMODE_Msk & ((value) << PM_SLEEPCFG_SLEEPMODE_Pos))
#define   PM_SLEEPCFG_SLEEPMODE_IDLE_Val      _U_(0x2)                                             /**< (PM_SLEEPCFG) CPU, AHBx, and APBx clocks are OFF  */
#define   PM_SLEEPCFG_SLEEPMODE_STANDBY_Val   _U_(0x4)                                             /**< (PM_SLEEPCFG) All Clocks are OFF  */
#define   PM_SLEEPCFG_SLEEPMODE_HIBERNATE_Val _U_(0x5)                                             /**< (PM_SLEEPCFG) Backup domain is ON as well as some PDRAMs  */
#define   PM_SLEEPCFG_SLEEPMODE_BACKUP_Val    _U_(0x6)                                             /**< (PM_SLEEPCFG) Only Backup domain is powered ON  */
#define   PM_SLEEPCFG_SLEEPMODE_OFF_Val       _U_(0x7)                                             /**< (PM_SLEEPCFG) All power domains are powered OFF  */
#define PM_SLEEPCFG_SLEEPMODE_IDLE            (PM_SLEEPCFG_SLEEPMODE_IDLE_Val << PM_SLEEPCFG_SLEEPMODE_Pos) /**< (PM_SLEEPCFG) CPU, AHBx, and APBx clocks are OFF Position  */
#define PM_SLEEPCFG_SLEEPMODE_STANDBY         (PM_SLEEPCFG_SLEEPMODE_STANDBY_Val << PM_SLEEPCFG_SLEEPMODE_Pos) /**< (PM_SLEEPCFG) All Clocks are OFF Position  */
#define PM_SLEEPCFG_SLEEPMODE_HIBERNATE       (PM_SLEEPCFG_SLEEPMODE_HIBERNATE_Val << PM_SLEEPCFG_SLEEPMODE_Pos) /**< (PM_SLEEPCFG) Backup domain is ON as well as some PDRAMs Position  */
#define PM_SLEEPCFG_SLEEPMODE_BACKUP          (PM_SLEEPCFG_SLEEPMODE_BACKUP_Val << PM_SLEEPCFG_SLEEPMODE_Pos) /**< (PM_SLEEPCFG) Only Backup domain is powered ON Position  */
#define PM_SLEEPCFG_SLEEPMODE_OFF             (PM_SLEEPCFG_SLEEPMODE_OFF_Val << PM_SLEEPCFG_SLEEPMODE_Pos) /**< (PM_SLEEPCFG) All power domains are powered OFF Position  */
#define PM_SLEEPCFG_Msk                       _U_(0x07)                                            /**< (PM_SLEEPCFG) Register Mask  */


/* -------- PM_INTENCLR : (PM Offset: 0x04) (R/W 8) Interrupt Enable Clear -------- */
#define PM_INTENCLR_RESETVALUE                _U_(0x00)                                            /**<  (PM_INTENCLR) Interrupt Enable Clear  Reset Value */

#define PM_INTENCLR_SLEEPRDY_Pos              _U_(0)                                               /**< (PM_INTENCLR) Sleep Mode Entry Ready Enable Position */
#define PM_INTENCLR_SLEEPRDY_Msk              (_U_(0x1) << PM_INTENCLR_SLEEPRDY_Pos)               /**< (PM_INTENCLR) Sleep Mode Entry Ready Enable Mask */
#define PM_INTENCLR_SLEEPRDY(value)           (PM_INTENCLR_SLEEPRDY_Msk & ((value) << PM_INTENCLR_SLEEPRDY_Pos))
#define PM_INTENCLR_Msk                       _U_(0x01)                                            /**< (PM_INTENCLR) Register Mask  */


/* -------- PM_INTENSET : (PM Offset: 0x05) (R/W 8) Interrupt Enable Set -------- */
#define PM_INTENSET_RESETVALUE                _U_(0x00)                                            /**<  (PM_INTENSET) Interrupt Enable Set  Reset Value */

#define PM_INTENSET_SLEEPRDY_Pos              _U_(0)                                               /**< (PM_INTENSET) Sleep Mode Entry Ready Enable Position */
#define PM_INTENSET_SLEEPRDY_Msk              (_U_(0x1) << PM_INTENSET_SLEEPRDY_Pos)               /**< (PM_INTENSET) Sleep Mode Entry Ready Enable Mask */
#define PM_INTENSET_SLEEPRDY(value)           (PM_INTENSET_SLEEPRDY_Msk & ((value) << PM_INTENSET_SLEEPRDY_Pos))
#define PM_INTENSET_Msk                       _U_(0x01)                                            /**< (PM_INTENSET) Register Mask  */


/* -------- PM_INTFLAG : (PM Offset: 0x06) (R/W 8) Interrupt Flag Status and Clear -------- */
#define PM_INTFLAG_RESETVALUE                 _U_(0x00)                                            /**<  (PM_INTFLAG) Interrupt Flag Status and Clear  Reset Value */

#define PM_INTFLAG_SLEEPRDY_Pos               _U_(0)                                               /**< (PM_INTFLAG) Sleep Mode Entry Ready Position */
#define PM_INTFLAG_SLEEPRDY_Msk               (_U_(0x1) << PM_INTFLAG_SLEEPRDY_Pos)                /**< (PM_INTFLAG) Sleep Mode Entry Ready Mask */
#define PM_INTFLAG_SLEEPRDY(value)            (PM_INTFLAG_SLEEPRDY_Msk & ((value) << PM_INTFLAG_SLEEPRDY_Pos))
#define PM_INTFLAG_Msk                        _U_(0x01)                                            /**< (PM_INTFLAG) Register Mask  */


/* -------- PM_STDBYCFG : (PM Offset: 0x08) (R/W 8) Standby Configuration -------- */
#define PM_STDBYCFG_RESETVALUE                _U_(0x00)                                            /**<  (PM_STDBYCFG) Standby Configuration  Reset Value */

#define PM_STDBYCFG_RAMCFG_Pos                _U_(0)                                               /**< (PM_STDBYCFG) Ram Configuration Position */
#define PM_STDBYCFG_RAMCFG_Msk                (_U_(0x3) << PM_STDBYCFG_RAMCFG_Pos)                 /**< (PM_STDBYCFG) Ram Configuration Mask */
#define PM_STDBYCFG_RAMCFG(value)             (PM_STDBYCFG_RAMCFG_Msk & ((value) << PM_STDBYCFG_RAMCFG_Pos))
#define   PM_STDBYCFG_RAMCFG_RET_Val          _U_(0x0)                                             /**< (PM_STDBYCFG) All the system RAM is retained  */
#define   PM_STDBYCFG_RAMCFG_PARTIAL_Val      _U_(0x1)                                             /**< (PM_STDBYCFG) Only the first 32Kbytes of the system RAM is retained  */
#define   PM_STDBYCFG_RAMCFG_OFF_Val          _U_(0x2)                                             /**< (PM_STDBYCFG) All the system RAM is turned OFF  */
#define PM_STDBYCFG_RAMCFG_RET                (PM_STDBYCFG_RAMCFG_RET_Val << PM_STDBYCFG_RAMCFG_Pos) /**< (PM_STDBYCFG) All the system RAM is retained Position  */
#define PM_STDBYCFG_RAMCFG_PARTIAL            (PM_STDBYCFG_RAMCFG_PARTIAL_Val << PM_STDBYCFG_RAMCFG_Pos) /**< (PM_STDBYCFG) Only the first 32Kbytes of the system RAM is retained Position  */
#define PM_STDBYCFG_RAMCFG_OFF                (PM_STDBYCFG_RAMCFG_OFF_Val << PM_STDBYCFG_RAMCFG_Pos) /**< (PM_STDBYCFG) All the system RAM is turned OFF Position  */
#define PM_STDBYCFG_FASTWKUP_Pos              _U_(4)                                               /**< (PM_STDBYCFG) Fast Wakeup Position */
#define PM_STDBYCFG_FASTWKUP_Msk              (_U_(0x3) << PM_STDBYCFG_FASTWKUP_Pos)               /**< (PM_STDBYCFG) Fast Wakeup Mask */
#define PM_STDBYCFG_FASTWKUP(value)           (PM_STDBYCFG_FASTWKUP_Msk & ((value) << PM_STDBYCFG_FASTWKUP_Pos))
#define   PM_STDBYCFG_FASTWKUP_NO_Val         _U_(0x0)                                             /**< (PM_STDBYCFG) Fast Wakeup is disabled  */
#define   PM_STDBYCFG_FASTWKUP_NVM_Val        _U_(0x1)                                             /**< (PM_STDBYCFG) Fast Wakeup is enabled on NVM  */
#define   PM_STDBYCFG_FASTWKUP_MAINVREG_Val   _U_(0x2)                                             /**< (PM_STDBYCFG) Fast Wakeup is enabled on the main voltage regulator (MAINVREG)  */
#define   PM_STDBYCFG_FASTWKUP_BOTH_Val       _U_(0x3)                                             /**< (PM_STDBYCFG) Fast Wakeup is enabled on both NVM and MAINVREG  */
#define PM_STDBYCFG_FASTWKUP_NO               (PM_STDBYCFG_FASTWKUP_NO_Val << PM_STDBYCFG_FASTWKUP_Pos) /**< (PM_STDBYCFG) Fast Wakeup is disabled Position  */
#define PM_STDBYCFG_FASTWKUP_NVM              (PM_STDBYCFG_FASTWKUP_NVM_Val << PM_STDBYCFG_FASTWKUP_Pos) /**< (PM_STDBYCFG) Fast Wakeup is enabled on NVM Position  */
#define PM_STDBYCFG_FASTWKUP_MAINVREG         (PM_STDBYCFG_FASTWKUP_MAINVREG_Val << PM_STDBYCFG_FASTWKUP_Pos) /**< (PM_STDBYCFG) Fast Wakeup is enabled on the main voltage regulator (MAINVREG) Position  */
#define PM_STDBYCFG_FASTWKUP_BOTH             (PM_STDBYCFG_FASTWKUP_BOTH_Val << PM_STDBYCFG_FASTWKUP_Pos) /**< (PM_STDBYCFG) Fast Wakeup is enabled on both NVM and MAINVREG Position  */
#define PM_STDBYCFG_Msk                       _U_(0x33)                                            /**< (PM_STDBYCFG) Register Mask  */


/* -------- PM_HIBCFG : (PM Offset: 0x09) (R/W 8) Hibernate Configuration -------- */
#define PM_HIBCFG_RESETVALUE                  _U_(0x00)                                            /**<  (PM_HIBCFG) Hibernate Configuration  Reset Value */

#define PM_HIBCFG_RAMCFG_Pos                  _U_(0)                                               /**< (PM_HIBCFG) Ram Configuration Position */
#define PM_HIBCFG_RAMCFG_Msk                  (_U_(0x3) << PM_HIBCFG_RAMCFG_Pos)                   /**< (PM_HIBCFG) Ram Configuration Mask */
#define PM_HIBCFG_RAMCFG(value)               (PM_HIBCFG_RAMCFG_Msk & ((value) << PM_HIBCFG_RAMCFG_Pos))
#define   PM_HIBCFG_RAMCFG_RET_Val            _U_(0x0)                                             /**< (PM_HIBCFG) All the system RAM is retained  */
#define   PM_HIBCFG_RAMCFG_PARTIAL_Val        _U_(0x1)                                             /**< (PM_HIBCFG) Only the first 32Kbytes of the system RAM is retained  */
#define   PM_HIBCFG_RAMCFG_OFF_Val            _U_(0x2)                                             /**< (PM_HIBCFG) All the system RAM is turned OFF  */
#define PM_HIBCFG_RAMCFG_RET                  (PM_HIBCFG_RAMCFG_RET_Val << PM_HIBCFG_RAMCFG_Pos)   /**< (PM_HIBCFG) All the system RAM is retained Position  */
#define PM_HIBCFG_RAMCFG_PARTIAL              (PM_HIBCFG_RAMCFG_PARTIAL_Val << PM_HIBCFG_RAMCFG_Pos) /**< (PM_HIBCFG) Only the first 32Kbytes of the system RAM is retained Position  */
#define PM_HIBCFG_RAMCFG_OFF                  (PM_HIBCFG_RAMCFG_OFF_Val << PM_HIBCFG_RAMCFG_Pos)   /**< (PM_HIBCFG) All the system RAM is turned OFF Position  */
#define PM_HIBCFG_BRAMCFG_Pos                 _U_(2)                                               /**< (PM_HIBCFG) Backup Ram Configuration Position */
#define PM_HIBCFG_BRAMCFG_Msk                 (_U_(0x3) << PM_HIBCFG_BRAMCFG_Pos)                  /**< (PM_HIBCFG) Backup Ram Configuration Mask */
#define PM_HIBCFG_BRAMCFG(value)              (PM_HIBCFG_BRAMCFG_Msk & ((value) << PM_HIBCFG_BRAMCFG_Pos))
#define   PM_HIBCFG_BRAMCFG_RET_Val           _U_(0x0)                                             /**< (PM_HIBCFG) All the backup RAM is retained  */
#define   PM_HIBCFG_BRAMCFG_PARTIAL_Val       _U_(0x1)                                             /**< (PM_HIBCFG) Only the first 4Kbytes of the backup RAM is retained  */
#define   PM_HIBCFG_BRAMCFG_OFF_Val           _U_(0x2)                                             /**< (PM_HIBCFG) All the backup RAM is turned OFF  */
#define PM_HIBCFG_BRAMCFG_RET                 (PM_HIBCFG_BRAMCFG_RET_Val << PM_HIBCFG_BRAMCFG_Pos) /**< (PM_HIBCFG) All the backup RAM is retained Position  */
#define PM_HIBCFG_BRAMCFG_PARTIAL             (PM_HIBCFG_BRAMCFG_PARTIAL_Val << PM_HIBCFG_BRAMCFG_Pos) /**< (PM_HIBCFG) Only the first 4Kbytes of the backup RAM is retained Position  */
#define PM_HIBCFG_BRAMCFG_OFF                 (PM_HIBCFG_BRAMCFG_OFF_Val << PM_HIBCFG_BRAMCFG_Pos) /**< (PM_HIBCFG) All the backup RAM is turned OFF Position  */
#define PM_HIBCFG_Msk                         _U_(0x0F)                                            /**< (PM_HIBCFG) Register Mask  */


/* -------- PM_BKUPCFG : (PM Offset: 0x0A) (R/W 8) Backup Configuration -------- */
#define PM_BKUPCFG_RESETVALUE                 _U_(0x00)                                            /**<  (PM_BKUPCFG) Backup Configuration  Reset Value */

#define PM_BKUPCFG_BRAMCFG_Pos                _U_(0)                                               /**< (PM_BKUPCFG) Ram Configuration Position */
#define PM_BKUPCFG_BRAMCFG_Msk                (_U_(0x3) << PM_BKUPCFG_BRAMCFG_Pos)                 /**< (PM_BKUPCFG) Ram Configuration Mask */
#define PM_BKUPCFG_BRAMCFG(value)             (PM_BKUPCFG_BRAMCFG_Msk & ((value) << PM_BKUPCFG_BRAMCFG_Pos))
#define   PM_BKUPCFG_BRAMCFG_RET_Val          _U_(0x0)                                             /**< (PM_BKUPCFG) All the backup RAM is retained  */
#define   PM_BKUPCFG_BRAMCFG_PARTIAL_Val      _U_(0x1)                                             /**< (PM_BKUPCFG) Only the first 4Kbytes of the backup RAM is retained  */
#define   PM_BKUPCFG_BRAMCFG_OFF_Val          _U_(0x2)                                             /**< (PM_BKUPCFG) All the backup RAM is turned OFF  */
#define PM_BKUPCFG_BRAMCFG_RET                (PM_BKUPCFG_BRAMCFG_RET_Val << PM_BKUPCFG_BRAMCFG_Pos) /**< (PM_BKUPCFG) All the backup RAM is retained Position  */
#define PM_BKUPCFG_BRAMCFG_PARTIAL            (PM_BKUPCFG_BRAMCFG_PARTIAL_Val << PM_BKUPCFG_BRAMCFG_Pos) /**< (PM_BKUPCFG) Only the first 4Kbytes of the backup RAM is retained Position  */
#define PM_BKUPCFG_BRAMCFG_OFF                (PM_BKUPCFG_BRAMCFG_OFF_Val << PM_BKUPCFG_BRAMCFG_Pos) /**< (PM_BKUPCFG) All the backup RAM is turned OFF Position  */
#define PM_BKUPCFG_Msk                        _U_(0x03)                                            /**< (PM_BKUPCFG) Register Mask  */


/* -------- PM_PWSAKDLY : (PM Offset: 0x12) (R/W 8) Power Switch Acknowledge Delay -------- */
#define PM_PWSAKDLY_RESETVALUE                _U_(0x00)                                            /**<  (PM_PWSAKDLY) Power Switch Acknowledge Delay  Reset Value */

#define PM_PWSAKDLY_DLYVAL_Pos                _U_(0)                                               /**< (PM_PWSAKDLY) Delay Value Position */
#define PM_PWSAKDLY_DLYVAL_Msk                (_U_(0x7F) << PM_PWSAKDLY_DLYVAL_Pos)                /**< (PM_PWSAKDLY) Delay Value Mask */
#define PM_PWSAKDLY_DLYVAL(value)             (PM_PWSAKDLY_DLYVAL_Msk & ((value) << PM_PWSAKDLY_DLYVAL_Pos))
#define PM_PWSAKDLY_IGNACK_Pos                _U_(7)                                               /**< (PM_PWSAKDLY) Ignore Acknowledge Position */
#define PM_PWSAKDLY_IGNACK_Msk                (_U_(0x1) << PM_PWSAKDLY_IGNACK_Pos)                 /**< (PM_PWSAKDLY) Ignore Acknowledge Mask */
#define PM_PWSAKDLY_IGNACK(value)             (PM_PWSAKDLY_IGNACK_Msk & ((value) << PM_PWSAKDLY_IGNACK_Pos))
#define PM_PWSAKDLY_Msk                       _U_(0xFF)                                            /**< (PM_PWSAKDLY) Register Mask  */


/** \brief PM register offsets definitions */
#define PM_CTRLA_REG_OFST              (0x00)              /**< (PM_CTRLA) Control A Offset */
#define PM_SLEEPCFG_REG_OFST           (0x01)              /**< (PM_SLEEPCFG) Sleep Configuration Offset */
#define PM_INTENCLR_REG_OFST           (0x04)              /**< (PM_INTENCLR) Interrupt Enable Clear Offset */
#define PM_INTENSET_REG_OFST           (0x05)              /**< (PM_INTENSET) Interrupt Enable Set Offset */
#define PM_INTFLAG_REG_OFST            (0x06)              /**< (PM_INTFLAG) Interrupt Flag Status and Clear Offset */
#define PM_STDBYCFG_REG_OFST           (0x08)              /**< (PM_STDBYCFG) Standby Configuration Offset */
#define PM_HIBCFG_REG_OFST             (0x09)              /**< (PM_HIBCFG) Hibernate Configuration Offset */
#define PM_BKUPCFG_REG_OFST            (0x0A)              /**< (PM_BKUPCFG) Backup Configuration Offset */
#define PM_PWSAKDLY_REG_OFST           (0x12)              /**< (PM_PWSAKDLY) Power Switch Acknowledge Delay Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief PM register API structure */
typedef struct
{  /* Power Manager */
  __IO  uint8_t                        PM_CTRLA;           /**< Offset: 0x00 (R/W  8) Control A */
  __IO  uint8_t                        PM_SLEEPCFG;        /**< Offset: 0x01 (R/W  8) Sleep Configuration */
  __I   uint8_t                        Reserved1[0x02];
  __IO  uint8_t                        PM_INTENCLR;        /**< Offset: 0x04 (R/W  8) Interrupt Enable Clear */
  __IO  uint8_t                        PM_INTENSET;        /**< Offset: 0x05 (R/W  8) Interrupt Enable Set */
  __IO  uint8_t                        PM_INTFLAG;         /**< Offset: 0x06 (R/W  8) Interrupt Flag Status and Clear */
  __I   uint8_t                        Reserved2[0x01];
  __IO  uint8_t                        PM_STDBYCFG;        /**< Offset: 0x08 (R/W  8) Standby Configuration */
  __IO  uint8_t                        PM_HIBCFG;          /**< Offset: 0x09 (R/W  8) Hibernate Configuration */
  __IO  uint8_t                        PM_BKUPCFG;         /**< Offset: 0x0A (R/W  8) Backup Configuration */
  __I   uint8_t                        Reserved3[0x07];
  __IO  uint8_t                        PM_PWSAKDLY;        /**< Offset: 0x12 (R/W  8) Power Switch Acknowledge Delay */
} pm_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_PM_COMPONENT_H_ */
