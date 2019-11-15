/**
 * \brief Component description for AES
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
#ifndef _SAME54_AES_COMPONENT_H_
#define _SAME54_AES_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR AES                                          */
/* ************************************************************************** */

/* -------- AES_CTRLA : (AES Offset: 0x00) (R/W 32) Control A -------- */
#define AES_CTRLA_RESETVALUE                  _U_(0x00)                                            /**<  (AES_CTRLA) Control A  Reset Value */

#define AES_CTRLA_SWRST_Pos                   _U_(0)                                               /**< (AES_CTRLA) Software Reset Position */
#define AES_CTRLA_SWRST_Msk                   (_U_(0x1) << AES_CTRLA_SWRST_Pos)                    /**< (AES_CTRLA) Software Reset Mask */
#define AES_CTRLA_SWRST(value)                (AES_CTRLA_SWRST_Msk & ((value) << AES_CTRLA_SWRST_Pos))
#define AES_CTRLA_ENABLE_Pos                  _U_(1)                                               /**< (AES_CTRLA) Enable Position */
#define AES_CTRLA_ENABLE_Msk                  (_U_(0x1) << AES_CTRLA_ENABLE_Pos)                   /**< (AES_CTRLA) Enable Mask */
#define AES_CTRLA_ENABLE(value)               (AES_CTRLA_ENABLE_Msk & ((value) << AES_CTRLA_ENABLE_Pos))
#define AES_CTRLA_AESMODE_Pos                 _U_(2)                                               /**< (AES_CTRLA) AES Modes of operation Position */
#define AES_CTRLA_AESMODE_Msk                 (_U_(0x7) << AES_CTRLA_AESMODE_Pos)                  /**< (AES_CTRLA) AES Modes of operation Mask */
#define AES_CTRLA_AESMODE(value)              (AES_CTRLA_AESMODE_Msk & ((value) << AES_CTRLA_AESMODE_Pos))
#define   AES_CTRLA_AESMODE_ECB_Val           _U_(0x0)                                             /**< (AES_CTRLA) Electronic code book mode  */
#define   AES_CTRLA_AESMODE_CBC_Val           _U_(0x1)                                             /**< (AES_CTRLA) Cipher block chaining mode  */
#define   AES_CTRLA_AESMODE_OFB_Val           _U_(0x2)                                             /**< (AES_CTRLA) Output feedback mode  */
#define   AES_CTRLA_AESMODE_CFB_Val           _U_(0x3)                                             /**< (AES_CTRLA) Cipher feedback mode  */
#define   AES_CTRLA_AESMODE_COUNTER_Val       _U_(0x4)                                             /**< (AES_CTRLA) Counter mode  */
#define   AES_CTRLA_AESMODE_CCM_Val           _U_(0x5)                                             /**< (AES_CTRLA) CCM mode  */
#define   AES_CTRLA_AESMODE_GCM_Val           _U_(0x6)                                             /**< (AES_CTRLA) Galois counter mode  */
#define AES_CTRLA_AESMODE_ECB                 (AES_CTRLA_AESMODE_ECB_Val << AES_CTRLA_AESMODE_Pos) /**< (AES_CTRLA) Electronic code book mode Position  */
#define AES_CTRLA_AESMODE_CBC                 (AES_CTRLA_AESMODE_CBC_Val << AES_CTRLA_AESMODE_Pos) /**< (AES_CTRLA) Cipher block chaining mode Position  */
#define AES_CTRLA_AESMODE_OFB                 (AES_CTRLA_AESMODE_OFB_Val << AES_CTRLA_AESMODE_Pos) /**< (AES_CTRLA) Output feedback mode Position  */
#define AES_CTRLA_AESMODE_CFB                 (AES_CTRLA_AESMODE_CFB_Val << AES_CTRLA_AESMODE_Pos) /**< (AES_CTRLA) Cipher feedback mode Position  */
#define AES_CTRLA_AESMODE_COUNTER             (AES_CTRLA_AESMODE_COUNTER_Val << AES_CTRLA_AESMODE_Pos) /**< (AES_CTRLA) Counter mode Position  */
#define AES_CTRLA_AESMODE_CCM                 (AES_CTRLA_AESMODE_CCM_Val << AES_CTRLA_AESMODE_Pos) /**< (AES_CTRLA) CCM mode Position  */
#define AES_CTRLA_AESMODE_GCM                 (AES_CTRLA_AESMODE_GCM_Val << AES_CTRLA_AESMODE_Pos) /**< (AES_CTRLA) Galois counter mode Position  */
#define AES_CTRLA_CFBS_Pos                    _U_(5)                                               /**< (AES_CTRLA) Cipher Feedback Block Size Position */
#define AES_CTRLA_CFBS_Msk                    (_U_(0x7) << AES_CTRLA_CFBS_Pos)                     /**< (AES_CTRLA) Cipher Feedback Block Size Mask */
#define AES_CTRLA_CFBS(value)                 (AES_CTRLA_CFBS_Msk & ((value) << AES_CTRLA_CFBS_Pos))
#define   AES_CTRLA_CFBS_128BIT_Val           _U_(0x0)                                             /**< (AES_CTRLA) 128-bit Input data block for Encryption/Decryption in Cipher Feedback mode  */
#define   AES_CTRLA_CFBS_64BIT_Val            _U_(0x1)                                             /**< (AES_CTRLA) 64-bit Input data block for Encryption/Decryption in Cipher Feedback mode  */
#define   AES_CTRLA_CFBS_32BIT_Val            _U_(0x2)                                             /**< (AES_CTRLA) 32-bit Input data block for Encryption/Decryption in Cipher Feedback mode  */
#define   AES_CTRLA_CFBS_16BIT_Val            _U_(0x3)                                             /**< (AES_CTRLA) 16-bit Input data block for Encryption/Decryption in Cipher Feedback mode  */
#define   AES_CTRLA_CFBS_8BIT_Val             _U_(0x4)                                             /**< (AES_CTRLA) 8-bit Input data block for Encryption/Decryption in Cipher Feedback mode  */
#define AES_CTRLA_CFBS_128BIT                 (AES_CTRLA_CFBS_128BIT_Val << AES_CTRLA_CFBS_Pos)    /**< (AES_CTRLA) 128-bit Input data block for Encryption/Decryption in Cipher Feedback mode Position  */
#define AES_CTRLA_CFBS_64BIT                  (AES_CTRLA_CFBS_64BIT_Val << AES_CTRLA_CFBS_Pos)     /**< (AES_CTRLA) 64-bit Input data block for Encryption/Decryption in Cipher Feedback mode Position  */
#define AES_CTRLA_CFBS_32BIT                  (AES_CTRLA_CFBS_32BIT_Val << AES_CTRLA_CFBS_Pos)     /**< (AES_CTRLA) 32-bit Input data block for Encryption/Decryption in Cipher Feedback mode Position  */
#define AES_CTRLA_CFBS_16BIT                  (AES_CTRLA_CFBS_16BIT_Val << AES_CTRLA_CFBS_Pos)     /**< (AES_CTRLA) 16-bit Input data block for Encryption/Decryption in Cipher Feedback mode Position  */
#define AES_CTRLA_CFBS_8BIT                   (AES_CTRLA_CFBS_8BIT_Val << AES_CTRLA_CFBS_Pos)      /**< (AES_CTRLA) 8-bit Input data block for Encryption/Decryption in Cipher Feedback mode Position  */
#define AES_CTRLA_KEYSIZE_Pos                 _U_(8)                                               /**< (AES_CTRLA) Encryption Key Size Position */
#define AES_CTRLA_KEYSIZE_Msk                 (_U_(0x3) << AES_CTRLA_KEYSIZE_Pos)                  /**< (AES_CTRLA) Encryption Key Size Mask */
#define AES_CTRLA_KEYSIZE(value)              (AES_CTRLA_KEYSIZE_Msk & ((value) << AES_CTRLA_KEYSIZE_Pos))
#define   AES_CTRLA_KEYSIZE_128BIT_Val        _U_(0x0)                                             /**< (AES_CTRLA) 128-bit Key for Encryption / Decryption  */
#define   AES_CTRLA_KEYSIZE_192BIT_Val        _U_(0x1)                                             /**< (AES_CTRLA) 192-bit Key for Encryption / Decryption  */
#define   AES_CTRLA_KEYSIZE_256BIT_Val        _U_(0x2)                                             /**< (AES_CTRLA) 256-bit Key for Encryption / Decryption  */
#define AES_CTRLA_KEYSIZE_128BIT              (AES_CTRLA_KEYSIZE_128BIT_Val << AES_CTRLA_KEYSIZE_Pos) /**< (AES_CTRLA) 128-bit Key for Encryption / Decryption Position  */
#define AES_CTRLA_KEYSIZE_192BIT              (AES_CTRLA_KEYSIZE_192BIT_Val << AES_CTRLA_KEYSIZE_Pos) /**< (AES_CTRLA) 192-bit Key for Encryption / Decryption Position  */
#define AES_CTRLA_KEYSIZE_256BIT              (AES_CTRLA_KEYSIZE_256BIT_Val << AES_CTRLA_KEYSIZE_Pos) /**< (AES_CTRLA) 256-bit Key for Encryption / Decryption Position  */
#define AES_CTRLA_CIPHER_Pos                  _U_(10)                                              /**< (AES_CTRLA) Cipher Mode Position */
#define AES_CTRLA_CIPHER_Msk                  (_U_(0x1) << AES_CTRLA_CIPHER_Pos)                   /**< (AES_CTRLA) Cipher Mode Mask */
#define AES_CTRLA_CIPHER(value)               (AES_CTRLA_CIPHER_Msk & ((value) << AES_CTRLA_CIPHER_Pos))
#define   AES_CTRLA_CIPHER_DEC_Val            _U_(0x0)                                             /**< (AES_CTRLA) Decryption  */
#define   AES_CTRLA_CIPHER_ENC_Val            _U_(0x1)                                             /**< (AES_CTRLA) Encryption  */
#define AES_CTRLA_CIPHER_DEC                  (AES_CTRLA_CIPHER_DEC_Val << AES_CTRLA_CIPHER_Pos)   /**< (AES_CTRLA) Decryption Position  */
#define AES_CTRLA_CIPHER_ENC                  (AES_CTRLA_CIPHER_ENC_Val << AES_CTRLA_CIPHER_Pos)   /**< (AES_CTRLA) Encryption Position  */
#define AES_CTRLA_STARTMODE_Pos               _U_(11)                                              /**< (AES_CTRLA) Start Mode Select Position */
#define AES_CTRLA_STARTMODE_Msk               (_U_(0x1) << AES_CTRLA_STARTMODE_Pos)                /**< (AES_CTRLA) Start Mode Select Mask */
#define AES_CTRLA_STARTMODE(value)            (AES_CTRLA_STARTMODE_Msk & ((value) << AES_CTRLA_STARTMODE_Pos))
#define   AES_CTRLA_STARTMODE_MANUAL_Val      _U_(0x0)                                             /**< (AES_CTRLA) Start Encryption / Decryption in Manual mode  */
#define   AES_CTRLA_STARTMODE_AUTO_Val        _U_(0x1)                                             /**< (AES_CTRLA) Start Encryption / Decryption in Auto mode  */
#define AES_CTRLA_STARTMODE_MANUAL            (AES_CTRLA_STARTMODE_MANUAL_Val << AES_CTRLA_STARTMODE_Pos) /**< (AES_CTRLA) Start Encryption / Decryption in Manual mode Position  */
#define AES_CTRLA_STARTMODE_AUTO              (AES_CTRLA_STARTMODE_AUTO_Val << AES_CTRLA_STARTMODE_Pos) /**< (AES_CTRLA) Start Encryption / Decryption in Auto mode Position  */
#define AES_CTRLA_LOD_Pos                     _U_(12)                                              /**< (AES_CTRLA) Last Output Data Mode Position */
#define AES_CTRLA_LOD_Msk                     (_U_(0x1) << AES_CTRLA_LOD_Pos)                      /**< (AES_CTRLA) Last Output Data Mode Mask */
#define AES_CTRLA_LOD(value)                  (AES_CTRLA_LOD_Msk & ((value) << AES_CTRLA_LOD_Pos))
#define   AES_CTRLA_LOD_NONE_Val              _U_(0x0)                                             /**< (AES_CTRLA) No effect  */
#define   AES_CTRLA_LOD_LAST_Val              _U_(0x1)                                             /**< (AES_CTRLA) Start encryption in Last Output Data mode  */
#define AES_CTRLA_LOD_NONE                    (AES_CTRLA_LOD_NONE_Val << AES_CTRLA_LOD_Pos)        /**< (AES_CTRLA) No effect Position  */
#define AES_CTRLA_LOD_LAST                    (AES_CTRLA_LOD_LAST_Val << AES_CTRLA_LOD_Pos)        /**< (AES_CTRLA) Start encryption in Last Output Data mode Position  */
#define AES_CTRLA_KEYGEN_Pos                  _U_(13)                                              /**< (AES_CTRLA) Last Key Generation Position */
#define AES_CTRLA_KEYGEN_Msk                  (_U_(0x1) << AES_CTRLA_KEYGEN_Pos)                   /**< (AES_CTRLA) Last Key Generation Mask */
#define AES_CTRLA_KEYGEN(value)               (AES_CTRLA_KEYGEN_Msk & ((value) << AES_CTRLA_KEYGEN_Pos))
#define   AES_CTRLA_KEYGEN_NONE_Val           _U_(0x0)                                             /**< (AES_CTRLA) No effect  */
#define   AES_CTRLA_KEYGEN_LAST_Val           _U_(0x1)                                             /**< (AES_CTRLA) Start Computation of the last NK words of the expanded key  */
#define AES_CTRLA_KEYGEN_NONE                 (AES_CTRLA_KEYGEN_NONE_Val << AES_CTRLA_KEYGEN_Pos)  /**< (AES_CTRLA) No effect Position  */
#define AES_CTRLA_KEYGEN_LAST                 (AES_CTRLA_KEYGEN_LAST_Val << AES_CTRLA_KEYGEN_Pos)  /**< (AES_CTRLA) Start Computation of the last NK words of the expanded key Position  */
#define AES_CTRLA_XORKEY_Pos                  _U_(14)                                              /**< (AES_CTRLA) XOR Key Operation Position */
#define AES_CTRLA_XORKEY_Msk                  (_U_(0x1) << AES_CTRLA_XORKEY_Pos)                   /**< (AES_CTRLA) XOR Key Operation Mask */
#define AES_CTRLA_XORKEY(value)               (AES_CTRLA_XORKEY_Msk & ((value) << AES_CTRLA_XORKEY_Pos))
#define   AES_CTRLA_XORKEY_NONE_Val           _U_(0x0)                                             /**< (AES_CTRLA) No effect  */
#define   AES_CTRLA_XORKEY_XOR_Val            _U_(0x1)                                             /**< (AES_CTRLA) The user keyword gets XORed with the previous keyword register content.  */
#define AES_CTRLA_XORKEY_NONE                 (AES_CTRLA_XORKEY_NONE_Val << AES_CTRLA_XORKEY_Pos)  /**< (AES_CTRLA) No effect Position  */
#define AES_CTRLA_XORKEY_XOR                  (AES_CTRLA_XORKEY_XOR_Val << AES_CTRLA_XORKEY_Pos)   /**< (AES_CTRLA) The user keyword gets XORed with the previous keyword register content. Position  */
#define AES_CTRLA_CTYPE_Pos                   _U_(16)                                              /**< (AES_CTRLA) Counter Measure Type Position */
#define AES_CTRLA_CTYPE_Msk                   (_U_(0xF) << AES_CTRLA_CTYPE_Pos)                    /**< (AES_CTRLA) Counter Measure Type Mask */
#define AES_CTRLA_CTYPE(value)                (AES_CTRLA_CTYPE_Msk & ((value) << AES_CTRLA_CTYPE_Pos))
#define AES_CTRLA_Msk                         _U_(0x000F7FFF)                                      /**< (AES_CTRLA) Register Mask  */


/* -------- AES_CTRLB : (AES Offset: 0x04) (R/W 8) Control B -------- */
#define AES_CTRLB_RESETVALUE                  _U_(0x00)                                            /**<  (AES_CTRLB) Control B  Reset Value */

#define AES_CTRLB_START_Pos                   _U_(0)                                               /**< (AES_CTRLB) Start Encryption/Decryption Position */
#define AES_CTRLB_START_Msk                   (_U_(0x1) << AES_CTRLB_START_Pos)                    /**< (AES_CTRLB) Start Encryption/Decryption Mask */
#define AES_CTRLB_START(value)                (AES_CTRLB_START_Msk & ((value) << AES_CTRLB_START_Pos))
#define AES_CTRLB_NEWMSG_Pos                  _U_(1)                                               /**< (AES_CTRLB) New message Position */
#define AES_CTRLB_NEWMSG_Msk                  (_U_(0x1) << AES_CTRLB_NEWMSG_Pos)                   /**< (AES_CTRLB) New message Mask */
#define AES_CTRLB_NEWMSG(value)               (AES_CTRLB_NEWMSG_Msk & ((value) << AES_CTRLB_NEWMSG_Pos))
#define AES_CTRLB_EOM_Pos                     _U_(2)                                               /**< (AES_CTRLB) End of message Position */
#define AES_CTRLB_EOM_Msk                     (_U_(0x1) << AES_CTRLB_EOM_Pos)                      /**< (AES_CTRLB) End of message Mask */
#define AES_CTRLB_EOM(value)                  (AES_CTRLB_EOM_Msk & ((value) << AES_CTRLB_EOM_Pos))
#define AES_CTRLB_GFMUL_Pos                   _U_(3)                                               /**< (AES_CTRLB) GF Multiplication Position */
#define AES_CTRLB_GFMUL_Msk                   (_U_(0x1) << AES_CTRLB_GFMUL_Pos)                    /**< (AES_CTRLB) GF Multiplication Mask */
#define AES_CTRLB_GFMUL(value)                (AES_CTRLB_GFMUL_Msk & ((value) << AES_CTRLB_GFMUL_Pos))
#define AES_CTRLB_Msk                         _U_(0x0F)                                            /**< (AES_CTRLB) Register Mask  */


/* -------- AES_INTENCLR : (AES Offset: 0x05) (R/W 8) Interrupt Enable Clear -------- */
#define AES_INTENCLR_RESETVALUE               _U_(0x00)                                            /**<  (AES_INTENCLR) Interrupt Enable Clear  Reset Value */

#define AES_INTENCLR_ENCCMP_Pos               _U_(0)                                               /**< (AES_INTENCLR) Encryption Complete Interrupt Enable Position */
#define AES_INTENCLR_ENCCMP_Msk               (_U_(0x1) << AES_INTENCLR_ENCCMP_Pos)                /**< (AES_INTENCLR) Encryption Complete Interrupt Enable Mask */
#define AES_INTENCLR_ENCCMP(value)            (AES_INTENCLR_ENCCMP_Msk & ((value) << AES_INTENCLR_ENCCMP_Pos))
#define AES_INTENCLR_GFMCMP_Pos               _U_(1)                                               /**< (AES_INTENCLR) GF Multiplication Complete Interrupt Enable Position */
#define AES_INTENCLR_GFMCMP_Msk               (_U_(0x1) << AES_INTENCLR_GFMCMP_Pos)                /**< (AES_INTENCLR) GF Multiplication Complete Interrupt Enable Mask */
#define AES_INTENCLR_GFMCMP(value)            (AES_INTENCLR_GFMCMP_Msk & ((value) << AES_INTENCLR_GFMCMP_Pos))
#define AES_INTENCLR_Msk                      _U_(0x03)                                            /**< (AES_INTENCLR) Register Mask  */


/* -------- AES_INTENSET : (AES Offset: 0x06) (R/W 8) Interrupt Enable Set -------- */
#define AES_INTENSET_RESETVALUE               _U_(0x00)                                            /**<  (AES_INTENSET) Interrupt Enable Set  Reset Value */

#define AES_INTENSET_ENCCMP_Pos               _U_(0)                                               /**< (AES_INTENSET) Encryption Complete Interrupt Enable Position */
#define AES_INTENSET_ENCCMP_Msk               (_U_(0x1) << AES_INTENSET_ENCCMP_Pos)                /**< (AES_INTENSET) Encryption Complete Interrupt Enable Mask */
#define AES_INTENSET_ENCCMP(value)            (AES_INTENSET_ENCCMP_Msk & ((value) << AES_INTENSET_ENCCMP_Pos))
#define AES_INTENSET_GFMCMP_Pos               _U_(1)                                               /**< (AES_INTENSET) GF Multiplication Complete Interrupt Enable Position */
#define AES_INTENSET_GFMCMP_Msk               (_U_(0x1) << AES_INTENSET_GFMCMP_Pos)                /**< (AES_INTENSET) GF Multiplication Complete Interrupt Enable Mask */
#define AES_INTENSET_GFMCMP(value)            (AES_INTENSET_GFMCMP_Msk & ((value) << AES_INTENSET_GFMCMP_Pos))
#define AES_INTENSET_Msk                      _U_(0x03)                                            /**< (AES_INTENSET) Register Mask  */


/* -------- AES_INTFLAG : (AES Offset: 0x07) (R/W 8) Interrupt Flag Status -------- */
#define AES_INTFLAG_RESETVALUE                _U_(0x00)                                            /**<  (AES_INTFLAG) Interrupt Flag Status  Reset Value */

#define AES_INTFLAG_ENCCMP_Pos                _U_(0)                                               /**< (AES_INTFLAG) Encryption Complete Position */
#define AES_INTFLAG_ENCCMP_Msk                (_U_(0x1) << AES_INTFLAG_ENCCMP_Pos)                 /**< (AES_INTFLAG) Encryption Complete Mask */
#define AES_INTFLAG_ENCCMP(value)             (AES_INTFLAG_ENCCMP_Msk & ((value) << AES_INTFLAG_ENCCMP_Pos))
#define AES_INTFLAG_GFMCMP_Pos                _U_(1)                                               /**< (AES_INTFLAG) GF Multiplication Complete Position */
#define AES_INTFLAG_GFMCMP_Msk                (_U_(0x1) << AES_INTFLAG_GFMCMP_Pos)                 /**< (AES_INTFLAG) GF Multiplication Complete Mask */
#define AES_INTFLAG_GFMCMP(value)             (AES_INTFLAG_GFMCMP_Msk & ((value) << AES_INTFLAG_GFMCMP_Pos))
#define AES_INTFLAG_Msk                       _U_(0x03)                                            /**< (AES_INTFLAG) Register Mask  */


/* -------- AES_DATABUFPTR : (AES Offset: 0x08) (R/W 8) Data buffer pointer -------- */
#define AES_DATABUFPTR_RESETVALUE             _U_(0x00)                                            /**<  (AES_DATABUFPTR) Data buffer pointer  Reset Value */

#define AES_DATABUFPTR_INDATAPTR_Pos          _U_(0)                                               /**< (AES_DATABUFPTR) Input Data Pointer Position */
#define AES_DATABUFPTR_INDATAPTR_Msk          (_U_(0x3) << AES_DATABUFPTR_INDATAPTR_Pos)           /**< (AES_DATABUFPTR) Input Data Pointer Mask */
#define AES_DATABUFPTR_INDATAPTR(value)       (AES_DATABUFPTR_INDATAPTR_Msk & ((value) << AES_DATABUFPTR_INDATAPTR_Pos))
#define AES_DATABUFPTR_Msk                    _U_(0x03)                                            /**< (AES_DATABUFPTR) Register Mask  */


/* -------- AES_DBGCTRL : (AES Offset: 0x09) (R/W 8) Debug control -------- */
#define AES_DBGCTRL_RESETVALUE                _U_(0x00)                                            /**<  (AES_DBGCTRL) Debug control  Reset Value */

#define AES_DBGCTRL_DBGRUN_Pos                _U_(0)                                               /**< (AES_DBGCTRL) Debug Run Position */
#define AES_DBGCTRL_DBGRUN_Msk                (_U_(0x1) << AES_DBGCTRL_DBGRUN_Pos)                 /**< (AES_DBGCTRL) Debug Run Mask */
#define AES_DBGCTRL_DBGRUN(value)             (AES_DBGCTRL_DBGRUN_Msk & ((value) << AES_DBGCTRL_DBGRUN_Pos))
#define AES_DBGCTRL_Msk                       _U_(0x01)                                            /**< (AES_DBGCTRL) Register Mask  */


/* -------- AES_KEYWORD : (AES Offset: 0x0C) ( /W 32) Keyword n -------- */
#define AES_KEYWORD_RESETVALUE                _U_(0x00)                                            /**<  (AES_KEYWORD) Keyword n  Reset Value */

#define AES_KEYWORD_Msk                       _U_(0x00000000)                                      /**< (AES_KEYWORD) Register Mask  */


/* -------- AES_INDATA : (AES Offset: 0x38) (R/W 32) Indata -------- */
#define AES_INDATA_RESETVALUE                 _U_(0x00)                                            /**<  (AES_INDATA) Indata  Reset Value */

#define AES_INDATA_Msk                        _U_(0x00000000)                                      /**< (AES_INDATA) Register Mask  */


/* -------- AES_INTVECTV : (AES Offset: 0x3C) ( /W 32) Initialisation Vector n -------- */
#define AES_INTVECTV_RESETVALUE               _U_(0x00)                                            /**<  (AES_INTVECTV) Initialisation Vector n  Reset Value */

#define AES_INTVECTV_Msk                      _U_(0x00000000)                                      /**< (AES_INTVECTV) Register Mask  */


/* -------- AES_HASHKEY : (AES Offset: 0x5C) (R/W 32) Hash key n -------- */
#define AES_HASHKEY_RESETVALUE                _U_(0x00)                                            /**<  (AES_HASHKEY) Hash key n  Reset Value */

#define AES_HASHKEY_Msk                       _U_(0x00000000)                                      /**< (AES_HASHKEY) Register Mask  */


/* -------- AES_GHASH : (AES Offset: 0x6C) (R/W 32) Galois Hash n -------- */
#define AES_GHASH_RESETVALUE                  _U_(0x00)                                            /**<  (AES_GHASH) Galois Hash n  Reset Value */

#define AES_GHASH_Msk                         _U_(0x00000000)                                      /**< (AES_GHASH) Register Mask  */


/* -------- AES_CIPLEN : (AES Offset: 0x80) (R/W 32) Cipher Length -------- */
#define AES_CIPLEN_RESETVALUE                 _U_(0x00)                                            /**<  (AES_CIPLEN) Cipher Length  Reset Value */

#define AES_CIPLEN_Msk                        _U_(0x00000000)                                      /**< (AES_CIPLEN) Register Mask  */


/* -------- AES_RANDSEED : (AES Offset: 0x84) (R/W 32) Random Seed -------- */
#define AES_RANDSEED_RESETVALUE               _U_(0x00)                                            /**<  (AES_RANDSEED) Random Seed  Reset Value */

#define AES_RANDSEED_Msk                      _U_(0x00000000)                                      /**< (AES_RANDSEED) Register Mask  */


/** \brief AES register offsets definitions */
#define AES_CTRLA_REG_OFST             (0x00)              /**< (AES_CTRLA) Control A Offset */
#define AES_CTRLB_REG_OFST             (0x04)              /**< (AES_CTRLB) Control B Offset */
#define AES_INTENCLR_REG_OFST          (0x05)              /**< (AES_INTENCLR) Interrupt Enable Clear Offset */
#define AES_INTENSET_REG_OFST          (0x06)              /**< (AES_INTENSET) Interrupt Enable Set Offset */
#define AES_INTFLAG_REG_OFST           (0x07)              /**< (AES_INTFLAG) Interrupt Flag Status Offset */
#define AES_DATABUFPTR_REG_OFST        (0x08)              /**< (AES_DATABUFPTR) Data buffer pointer Offset */
#define AES_DBGCTRL_REG_OFST           (0x09)              /**< (AES_DBGCTRL) Debug control Offset */
#define AES_KEYWORD_REG_OFST           (0x0C)              /**< (AES_KEYWORD) Keyword n Offset */
#define AES_INDATA_REG_OFST            (0x38)              /**< (AES_INDATA) Indata Offset */
#define AES_INTVECTV_REG_OFST          (0x3C)              /**< (AES_INTVECTV) Initialisation Vector n Offset */
#define AES_HASHKEY_REG_OFST           (0x5C)              /**< (AES_HASHKEY) Hash key n Offset */
#define AES_GHASH_REG_OFST             (0x6C)              /**< (AES_GHASH) Galois Hash n Offset */
#define AES_CIPLEN_REG_OFST            (0x80)              /**< (AES_CIPLEN) Cipher Length Offset */
#define AES_RANDSEED_REG_OFST          (0x84)              /**< (AES_RANDSEED) Random Seed Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief AES register API structure */
typedef struct
{  /* Advanced Encryption Standard */
  __IO  uint32_t                       AES_CTRLA;          /**< Offset: 0x00 (R/W  32) Control A */
  __IO  uint8_t                        AES_CTRLB;          /**< Offset: 0x04 (R/W  8) Control B */
  __IO  uint8_t                        AES_INTENCLR;       /**< Offset: 0x05 (R/W  8) Interrupt Enable Clear */
  __IO  uint8_t                        AES_INTENSET;       /**< Offset: 0x06 (R/W  8) Interrupt Enable Set */
  __IO  uint8_t                        AES_INTFLAG;        /**< Offset: 0x07 (R/W  8) Interrupt Flag Status */
  __IO  uint8_t                        AES_DATABUFPTR;     /**< Offset: 0x08 (R/W  8) Data buffer pointer */
  __IO  uint8_t                        AES_DBGCTRL;        /**< Offset: 0x09 (R/W  8) Debug control */
  __I   uint8_t                        Reserved1[0x02];
  __O   uint32_t                       AES_KEYWORD[8];     /**< Offset: 0x0C ( /W  32) Keyword n */
  __I   uint8_t                        Reserved2[0x0C];
  __IO  uint32_t                       AES_INDATA;         /**< Offset: 0x38 (R/W  32) Indata */
  __O   uint32_t                       AES_INTVECTV[4];    /**< Offset: 0x3C ( /W  32) Initialisation Vector n */
  __I   uint8_t                        Reserved3[0x10];
  __IO  uint32_t                       AES_HASHKEY[4];     /**< Offset: 0x5C (R/W  32) Hash key n */
  __IO  uint32_t                       AES_GHASH[4];       /**< Offset: 0x6C (R/W  32) Galois Hash n */
  __I   uint8_t                        Reserved4[0x04];
  __IO  uint32_t                       AES_CIPLEN;         /**< Offset: 0x80 (R/W  32) Cipher Length */
  __IO  uint32_t                       AES_RANDSEED;       /**< Offset: 0x84 (R/W  32) Random Seed */
} aes_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
#endif /* _SAME54_AES_COMPONENT_H_ */
