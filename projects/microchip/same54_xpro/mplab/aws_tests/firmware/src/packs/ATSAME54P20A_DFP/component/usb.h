/**
 * \brief Component description for USB
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
#ifndef _SAME54_USB_COMPONENT_H_
#define _SAME54_USB_COMPONENT_H_

/* ************************************************************************** */
/*   SOFTWARE API DEFINITION FOR USB                                          */
/* ************************************************************************** */

/* -------- USB_DEVICE_ADDR : (USB Offset: 0x00) (R/W 32) DEVICE_DESC_BANK Endpoint Bank, Adress of Data Buffer -------- */
#define USB_DEVICE_ADDR_ADDR_Pos              _U_(0)                                               /**< (USB_DEVICE_ADDR) Adress of data buffer Position */
#define USB_DEVICE_ADDR_ADDR_Msk              (_U_(0xFFFFFFFF) << USB_DEVICE_ADDR_ADDR_Pos)        /**< (USB_DEVICE_ADDR) Adress of data buffer Mask */
#define USB_DEVICE_ADDR_ADDR(value)           (USB_DEVICE_ADDR_ADDR_Msk & ((value) << USB_DEVICE_ADDR_ADDR_Pos))
#define USB_DEVICE_ADDR_Msk                   _U_(0xFFFFFFFF)                                      /**< (USB_DEVICE_ADDR) Register Mask  */


/* -------- USB_DEVICE_PCKSIZE : (USB Offset: 0x04) (R/W 32) DEVICE_DESC_BANK Endpoint Bank, Packet Size -------- */
#define USB_DEVICE_PCKSIZE_BYTE_COUNT_Pos     _U_(0)                                               /**< (USB_DEVICE_PCKSIZE) Byte Count Position */
#define USB_DEVICE_PCKSIZE_BYTE_COUNT_Msk     (_U_(0x3FFF) << USB_DEVICE_PCKSIZE_BYTE_COUNT_Pos)   /**< (USB_DEVICE_PCKSIZE) Byte Count Mask */
#define USB_DEVICE_PCKSIZE_BYTE_COUNT(value)  (USB_DEVICE_PCKSIZE_BYTE_COUNT_Msk & ((value) << USB_DEVICE_PCKSIZE_BYTE_COUNT_Pos))
#define USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Pos _U_(14)                                              /**< (USB_DEVICE_PCKSIZE) Multi Packet In or Out size Position */
#define USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Msk (_U_(0x3FFF) << USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Pos) /**< (USB_DEVICE_PCKSIZE) Multi Packet In or Out size Mask */
#define USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(value) (USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Msk & ((value) << USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Pos))
#define USB_DEVICE_PCKSIZE_SIZE_Pos           _U_(28)                                              /**< (USB_DEVICE_PCKSIZE) Enpoint size Position */
#define USB_DEVICE_PCKSIZE_SIZE_Msk           (_U_(0x7) << USB_DEVICE_PCKSIZE_SIZE_Pos)            /**< (USB_DEVICE_PCKSIZE) Enpoint size Mask */
#define USB_DEVICE_PCKSIZE_SIZE(value)        (USB_DEVICE_PCKSIZE_SIZE_Msk & ((value) << USB_DEVICE_PCKSIZE_SIZE_Pos))
#define USB_DEVICE_PCKSIZE_AUTO_ZLP_Pos       _U_(31)                                              /**< (USB_DEVICE_PCKSIZE) Automatic Zero Length Packet Position */
#define USB_DEVICE_PCKSIZE_AUTO_ZLP_Msk       (_U_(0x1) << USB_DEVICE_PCKSIZE_AUTO_ZLP_Pos)        /**< (USB_DEVICE_PCKSIZE) Automatic Zero Length Packet Mask */
#define USB_DEVICE_PCKSIZE_AUTO_ZLP(value)    (USB_DEVICE_PCKSIZE_AUTO_ZLP_Msk & ((value) << USB_DEVICE_PCKSIZE_AUTO_ZLP_Pos))
#define USB_DEVICE_PCKSIZE_Msk                _U_(0xFFFFFFFF)                                      /**< (USB_DEVICE_PCKSIZE) Register Mask  */


/* -------- USB_DEVICE_EXTREG : (USB Offset: 0x08) (R/W 16) DEVICE_DESC_BANK Endpoint Bank, Extended -------- */
#define USB_DEVICE_EXTREG_SUBPID_Pos          _U_(0)                                               /**< (USB_DEVICE_EXTREG) SUBPID field send with extended token Position */
#define USB_DEVICE_EXTREG_SUBPID_Msk          (_U_(0xF) << USB_DEVICE_EXTREG_SUBPID_Pos)           /**< (USB_DEVICE_EXTREG) SUBPID field send with extended token Mask */
#define USB_DEVICE_EXTREG_SUBPID(value)       (USB_DEVICE_EXTREG_SUBPID_Msk & ((value) << USB_DEVICE_EXTREG_SUBPID_Pos))
#define USB_DEVICE_EXTREG_VARIABLE_Pos        _U_(4)                                               /**< (USB_DEVICE_EXTREG) Variable field send with extended token Position */
#define USB_DEVICE_EXTREG_VARIABLE_Msk        (_U_(0x7FF) << USB_DEVICE_EXTREG_VARIABLE_Pos)       /**< (USB_DEVICE_EXTREG) Variable field send with extended token Mask */
#define USB_DEVICE_EXTREG_VARIABLE(value)     (USB_DEVICE_EXTREG_VARIABLE_Msk & ((value) << USB_DEVICE_EXTREG_VARIABLE_Pos))
#define USB_DEVICE_EXTREG_Msk                 _U_(0x7FFF)                                          /**< (USB_DEVICE_EXTREG) Register Mask  */


/* -------- USB_DEVICE_STATUS_BK : (USB Offset: 0x0A) (R/W 8) DEVICE_DESC_BANK Enpoint Bank, Status of Bank -------- */
#define USB_DEVICE_STATUS_BK_CRCERR_Pos       _U_(0)                                               /**< (USB_DEVICE_STATUS_BK) CRC Error Status Position */
#define USB_DEVICE_STATUS_BK_CRCERR_Msk       (_U_(0x1) << USB_DEVICE_STATUS_BK_CRCERR_Pos)        /**< (USB_DEVICE_STATUS_BK) CRC Error Status Mask */
#define USB_DEVICE_STATUS_BK_CRCERR(value)    (USB_DEVICE_STATUS_BK_CRCERR_Msk & ((value) << USB_DEVICE_STATUS_BK_CRCERR_Pos))
#define USB_DEVICE_STATUS_BK_ERRORFLOW_Pos    _U_(1)                                               /**< (USB_DEVICE_STATUS_BK) Error Flow Status Position */
#define USB_DEVICE_STATUS_BK_ERRORFLOW_Msk    (_U_(0x1) << USB_DEVICE_STATUS_BK_ERRORFLOW_Pos)     /**< (USB_DEVICE_STATUS_BK) Error Flow Status Mask */
#define USB_DEVICE_STATUS_BK_ERRORFLOW(value) (USB_DEVICE_STATUS_BK_ERRORFLOW_Msk & ((value) << USB_DEVICE_STATUS_BK_ERRORFLOW_Pos))
#define USB_DEVICE_STATUS_BK_Msk              _U_(0x03)                                            /**< (USB_DEVICE_STATUS_BK) Register Mask  */


/* -------- USB_HOST_ADDR : (USB Offset: 0x00) (R/W 32) HOST_DESC_BANK Host Bank, Adress of Data Buffer -------- */
#define USB_HOST_ADDR_ADDR_Pos                _U_(0)                                               /**< (USB_HOST_ADDR) Adress of data buffer Position */
#define USB_HOST_ADDR_ADDR_Msk                (_U_(0xFFFFFFFF) << USB_HOST_ADDR_ADDR_Pos)          /**< (USB_HOST_ADDR) Adress of data buffer Mask */
#define USB_HOST_ADDR_ADDR(value)             (USB_HOST_ADDR_ADDR_Msk & ((value) << USB_HOST_ADDR_ADDR_Pos))
#define USB_HOST_ADDR_Msk                     _U_(0xFFFFFFFF)                                      /**< (USB_HOST_ADDR) Register Mask  */


/* -------- USB_HOST_PCKSIZE : (USB Offset: 0x04) (R/W 32) HOST_DESC_BANK Host Bank, Packet Size -------- */
#define USB_HOST_PCKSIZE_BYTE_COUNT_Pos       _U_(0)                                               /**< (USB_HOST_PCKSIZE) Byte Count Position */
#define USB_HOST_PCKSIZE_BYTE_COUNT_Msk       (_U_(0x3FFF) << USB_HOST_PCKSIZE_BYTE_COUNT_Pos)     /**< (USB_HOST_PCKSIZE) Byte Count Mask */
#define USB_HOST_PCKSIZE_BYTE_COUNT(value)    (USB_HOST_PCKSIZE_BYTE_COUNT_Msk & ((value) << USB_HOST_PCKSIZE_BYTE_COUNT_Pos))
#define USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Pos _U_(14)                                              /**< (USB_HOST_PCKSIZE) Multi Packet In or Out size Position */
#define USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Msk (_U_(0x3FFF) << USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Pos) /**< (USB_HOST_PCKSIZE) Multi Packet In or Out size Mask */
#define USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(value) (USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Msk & ((value) << USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Pos))
#define USB_HOST_PCKSIZE_SIZE_Pos             _U_(28)                                              /**< (USB_HOST_PCKSIZE) Pipe size Position */
#define USB_HOST_PCKSIZE_SIZE_Msk             (_U_(0x7) << USB_HOST_PCKSIZE_SIZE_Pos)              /**< (USB_HOST_PCKSIZE) Pipe size Mask */
#define USB_HOST_PCKSIZE_SIZE(value)          (USB_HOST_PCKSIZE_SIZE_Msk & ((value) << USB_HOST_PCKSIZE_SIZE_Pos))
#define USB_HOST_PCKSIZE_AUTO_ZLP_Pos         _U_(31)                                              /**< (USB_HOST_PCKSIZE) Automatic Zero Length Packet Position */
#define USB_HOST_PCKSIZE_AUTO_ZLP_Msk         (_U_(0x1) << USB_HOST_PCKSIZE_AUTO_ZLP_Pos)          /**< (USB_HOST_PCKSIZE) Automatic Zero Length Packet Mask */
#define USB_HOST_PCKSIZE_AUTO_ZLP(value)      (USB_HOST_PCKSIZE_AUTO_ZLP_Msk & ((value) << USB_HOST_PCKSIZE_AUTO_ZLP_Pos))
#define USB_HOST_PCKSIZE_Msk                  _U_(0xFFFFFFFF)                                      /**< (USB_HOST_PCKSIZE) Register Mask  */


/* -------- USB_HOST_EXTREG : (USB Offset: 0x08) (R/W 16) HOST_DESC_BANK Host Bank, Extended -------- */
#define USB_HOST_EXTREG_SUBPID_Pos            _U_(0)                                               /**< (USB_HOST_EXTREG) SUBPID field send with extended token Position */
#define USB_HOST_EXTREG_SUBPID_Msk            (_U_(0xF) << USB_HOST_EXTREG_SUBPID_Pos)             /**< (USB_HOST_EXTREG) SUBPID field send with extended token Mask */
#define USB_HOST_EXTREG_SUBPID(value)         (USB_HOST_EXTREG_SUBPID_Msk & ((value) << USB_HOST_EXTREG_SUBPID_Pos))
#define USB_HOST_EXTREG_VARIABLE_Pos          _U_(4)                                               /**< (USB_HOST_EXTREG) Variable field send with extended token Position */
#define USB_HOST_EXTREG_VARIABLE_Msk          (_U_(0x7FF) << USB_HOST_EXTREG_VARIABLE_Pos)         /**< (USB_HOST_EXTREG) Variable field send with extended token Mask */
#define USB_HOST_EXTREG_VARIABLE(value)       (USB_HOST_EXTREG_VARIABLE_Msk & ((value) << USB_HOST_EXTREG_VARIABLE_Pos))
#define USB_HOST_EXTREG_Msk                   _U_(0x7FFF)                                          /**< (USB_HOST_EXTREG) Register Mask  */


/* -------- USB_HOST_STATUS_BK : (USB Offset: 0x0A) (R/W 8) HOST_DESC_BANK Host Bank, Status of Bank -------- */
#define USB_HOST_STATUS_BK_CRCERR_Pos         _U_(0)                                               /**< (USB_HOST_STATUS_BK) CRC Error Status Position */
#define USB_HOST_STATUS_BK_CRCERR_Msk         (_U_(0x1) << USB_HOST_STATUS_BK_CRCERR_Pos)          /**< (USB_HOST_STATUS_BK) CRC Error Status Mask */
#define USB_HOST_STATUS_BK_CRCERR(value)      (USB_HOST_STATUS_BK_CRCERR_Msk & ((value) << USB_HOST_STATUS_BK_CRCERR_Pos))
#define USB_HOST_STATUS_BK_ERRORFLOW_Pos      _U_(1)                                               /**< (USB_HOST_STATUS_BK) Error Flow Status Position */
#define USB_HOST_STATUS_BK_ERRORFLOW_Msk      (_U_(0x1) << USB_HOST_STATUS_BK_ERRORFLOW_Pos)       /**< (USB_HOST_STATUS_BK) Error Flow Status Mask */
#define USB_HOST_STATUS_BK_ERRORFLOW(value)   (USB_HOST_STATUS_BK_ERRORFLOW_Msk & ((value) << USB_HOST_STATUS_BK_ERRORFLOW_Pos))
#define USB_HOST_STATUS_BK_Msk                _U_(0x03)                                            /**< (USB_HOST_STATUS_BK) Register Mask  */


/* -------- USB_HOST_CTRL_PIPE : (USB Offset: 0x0C) (R/W 16) HOST_DESC_BANK Host Bank, Host Control Pipe -------- */
#define USB_HOST_CTRL_PIPE_RESETVALUE         _U_(0x00)                                            /**<  (USB_HOST_CTRL_PIPE) HOST_DESC_BANK Host Bank, Host Control Pipe  Reset Value */

#define USB_HOST_CTRL_PIPE_PDADDR_Pos         _U_(0)                                               /**< (USB_HOST_CTRL_PIPE) Pipe Device Adress Position */
#define USB_HOST_CTRL_PIPE_PDADDR_Msk         (_U_(0x7F) << USB_HOST_CTRL_PIPE_PDADDR_Pos)         /**< (USB_HOST_CTRL_PIPE) Pipe Device Adress Mask */
#define USB_HOST_CTRL_PIPE_PDADDR(value)      (USB_HOST_CTRL_PIPE_PDADDR_Msk & ((value) << USB_HOST_CTRL_PIPE_PDADDR_Pos))
#define USB_HOST_CTRL_PIPE_PEPNUM_Pos         _U_(8)                                               /**< (USB_HOST_CTRL_PIPE) Pipe Endpoint Number Position */
#define USB_HOST_CTRL_PIPE_PEPNUM_Msk         (_U_(0xF) << USB_HOST_CTRL_PIPE_PEPNUM_Pos)          /**< (USB_HOST_CTRL_PIPE) Pipe Endpoint Number Mask */
#define USB_HOST_CTRL_PIPE_PEPNUM(value)      (USB_HOST_CTRL_PIPE_PEPNUM_Msk & ((value) << USB_HOST_CTRL_PIPE_PEPNUM_Pos))
#define USB_HOST_CTRL_PIPE_PERMAX_Pos         _U_(12)                                              /**< (USB_HOST_CTRL_PIPE) Pipe Error Max Number Position */
#define USB_HOST_CTRL_PIPE_PERMAX_Msk         (_U_(0xF) << USB_HOST_CTRL_PIPE_PERMAX_Pos)          /**< (USB_HOST_CTRL_PIPE) Pipe Error Max Number Mask */
#define USB_HOST_CTRL_PIPE_PERMAX(value)      (USB_HOST_CTRL_PIPE_PERMAX_Msk & ((value) << USB_HOST_CTRL_PIPE_PERMAX_Pos))
#define USB_HOST_CTRL_PIPE_Msk                _U_(0xFF7F)                                          /**< (USB_HOST_CTRL_PIPE) Register Mask  */


/* -------- USB_HOST_STATUS_PIPE : (USB Offset: 0x0E) (R/W 16) HOST_DESC_BANK Host Bank, Host Status Pipe -------- */
#define USB_HOST_STATUS_PIPE_DTGLER_Pos       _U_(0)                                               /**< (USB_HOST_STATUS_PIPE) Data Toggle Error Position */
#define USB_HOST_STATUS_PIPE_DTGLER_Msk       (_U_(0x1) << USB_HOST_STATUS_PIPE_DTGLER_Pos)        /**< (USB_HOST_STATUS_PIPE) Data Toggle Error Mask */
#define USB_HOST_STATUS_PIPE_DTGLER(value)    (USB_HOST_STATUS_PIPE_DTGLER_Msk & ((value) << USB_HOST_STATUS_PIPE_DTGLER_Pos))
#define USB_HOST_STATUS_PIPE_DAPIDER_Pos      _U_(1)                                               /**< (USB_HOST_STATUS_PIPE) Data PID Error Position */
#define USB_HOST_STATUS_PIPE_DAPIDER_Msk      (_U_(0x1) << USB_HOST_STATUS_PIPE_DAPIDER_Pos)       /**< (USB_HOST_STATUS_PIPE) Data PID Error Mask */
#define USB_HOST_STATUS_PIPE_DAPIDER(value)   (USB_HOST_STATUS_PIPE_DAPIDER_Msk & ((value) << USB_HOST_STATUS_PIPE_DAPIDER_Pos))
#define USB_HOST_STATUS_PIPE_PIDER_Pos        _U_(2)                                               /**< (USB_HOST_STATUS_PIPE) PID Error Position */
#define USB_HOST_STATUS_PIPE_PIDER_Msk        (_U_(0x1) << USB_HOST_STATUS_PIPE_PIDER_Pos)         /**< (USB_HOST_STATUS_PIPE) PID Error Mask */
#define USB_HOST_STATUS_PIPE_PIDER(value)     (USB_HOST_STATUS_PIPE_PIDER_Msk & ((value) << USB_HOST_STATUS_PIPE_PIDER_Pos))
#define USB_HOST_STATUS_PIPE_TOUTER_Pos       _U_(3)                                               /**< (USB_HOST_STATUS_PIPE) Time Out Error Position */
#define USB_HOST_STATUS_PIPE_TOUTER_Msk       (_U_(0x1) << USB_HOST_STATUS_PIPE_TOUTER_Pos)        /**< (USB_HOST_STATUS_PIPE) Time Out Error Mask */
#define USB_HOST_STATUS_PIPE_TOUTER(value)    (USB_HOST_STATUS_PIPE_TOUTER_Msk & ((value) << USB_HOST_STATUS_PIPE_TOUTER_Pos))
#define USB_HOST_STATUS_PIPE_CRC16ER_Pos      _U_(4)                                               /**< (USB_HOST_STATUS_PIPE) CRC16 Error Position */
#define USB_HOST_STATUS_PIPE_CRC16ER_Msk      (_U_(0x1) << USB_HOST_STATUS_PIPE_CRC16ER_Pos)       /**< (USB_HOST_STATUS_PIPE) CRC16 Error Mask */
#define USB_HOST_STATUS_PIPE_CRC16ER(value)   (USB_HOST_STATUS_PIPE_CRC16ER_Msk & ((value) << USB_HOST_STATUS_PIPE_CRC16ER_Pos))
#define USB_HOST_STATUS_PIPE_ERCNT_Pos        _U_(5)                                               /**< (USB_HOST_STATUS_PIPE) Pipe Error Count Position */
#define USB_HOST_STATUS_PIPE_ERCNT_Msk        (_U_(0x7) << USB_HOST_STATUS_PIPE_ERCNT_Pos)         /**< (USB_HOST_STATUS_PIPE) Pipe Error Count Mask */
#define USB_HOST_STATUS_PIPE_ERCNT(value)     (USB_HOST_STATUS_PIPE_ERCNT_Msk & ((value) << USB_HOST_STATUS_PIPE_ERCNT_Pos))
#define USB_HOST_STATUS_PIPE_Msk              _U_(0x00FF)                                          /**< (USB_HOST_STATUS_PIPE) Register Mask  */


/* -------- USB_DEVICE_EPCFG : (USB Offset: 0x00) (R/W 8) DEVICE_ENDPOINT End Point Configuration -------- */
#define USB_DEVICE_EPCFG_RESETVALUE           _U_(0x00)                                            /**<  (USB_DEVICE_EPCFG) DEVICE_ENDPOINT End Point Configuration  Reset Value */

#define USB_DEVICE_EPCFG_EPTYPE0_Pos          _U_(0)                                               /**< (USB_DEVICE_EPCFG) End Point Type0 Position */
#define USB_DEVICE_EPCFG_EPTYPE0_Msk          (_U_(0x7) << USB_DEVICE_EPCFG_EPTYPE0_Pos)           /**< (USB_DEVICE_EPCFG) End Point Type0 Mask */
#define USB_DEVICE_EPCFG_EPTYPE0(value)       (USB_DEVICE_EPCFG_EPTYPE0_Msk & ((value) << USB_DEVICE_EPCFG_EPTYPE0_Pos))
#define USB_DEVICE_EPCFG_EPTYPE1_Pos          _U_(4)                                               /**< (USB_DEVICE_EPCFG) End Point Type1 Position */
#define USB_DEVICE_EPCFG_EPTYPE1_Msk          (_U_(0x7) << USB_DEVICE_EPCFG_EPTYPE1_Pos)           /**< (USB_DEVICE_EPCFG) End Point Type1 Mask */
#define USB_DEVICE_EPCFG_EPTYPE1(value)       (USB_DEVICE_EPCFG_EPTYPE1_Msk & ((value) << USB_DEVICE_EPCFG_EPTYPE1_Pos))
#define USB_DEVICE_EPCFG_NYETDIS_Pos          _U_(7)                                               /**< (USB_DEVICE_EPCFG) NYET Token Disable Position */
#define USB_DEVICE_EPCFG_NYETDIS_Msk          (_U_(0x1) << USB_DEVICE_EPCFG_NYETDIS_Pos)           /**< (USB_DEVICE_EPCFG) NYET Token Disable Mask */
#define USB_DEVICE_EPCFG_NYETDIS(value)       (USB_DEVICE_EPCFG_NYETDIS_Msk & ((value) << USB_DEVICE_EPCFG_NYETDIS_Pos))
#define USB_DEVICE_EPCFG_Msk                  _U_(0xF7)                                            /**< (USB_DEVICE_EPCFG) Register Mask  */


/* -------- USB_DEVICE_EPSTATUSCLR : (USB Offset: 0x04) ( /W 8) DEVICE_ENDPOINT End Point Pipe Status Clear -------- */
#define USB_DEVICE_EPSTATUSCLR_RESETVALUE     _U_(0x00)                                            /**<  (USB_DEVICE_EPSTATUSCLR) DEVICE_ENDPOINT End Point Pipe Status Clear  Reset Value */

#define USB_DEVICE_EPSTATUSCLR_DTGLOUT_Pos    _U_(0)                                               /**< (USB_DEVICE_EPSTATUSCLR) Data Toggle OUT Clear Position */
#define USB_DEVICE_EPSTATUSCLR_DTGLOUT_Msk    (_U_(0x1) << USB_DEVICE_EPSTATUSCLR_DTGLOUT_Pos)     /**< (USB_DEVICE_EPSTATUSCLR) Data Toggle OUT Clear Mask */
#define USB_DEVICE_EPSTATUSCLR_DTGLOUT(value) (USB_DEVICE_EPSTATUSCLR_DTGLOUT_Msk & ((value) << USB_DEVICE_EPSTATUSCLR_DTGLOUT_Pos))
#define USB_DEVICE_EPSTATUSCLR_DTGLIN_Pos     _U_(1)                                               /**< (USB_DEVICE_EPSTATUSCLR) Data Toggle IN Clear Position */
#define USB_DEVICE_EPSTATUSCLR_DTGLIN_Msk     (_U_(0x1) << USB_DEVICE_EPSTATUSCLR_DTGLIN_Pos)      /**< (USB_DEVICE_EPSTATUSCLR) Data Toggle IN Clear Mask */
#define USB_DEVICE_EPSTATUSCLR_DTGLIN(value)  (USB_DEVICE_EPSTATUSCLR_DTGLIN_Msk & ((value) << USB_DEVICE_EPSTATUSCLR_DTGLIN_Pos))
#define USB_DEVICE_EPSTATUSCLR_CURBK_Pos      _U_(2)                                               /**< (USB_DEVICE_EPSTATUSCLR) Current Bank Clear Position */
#define USB_DEVICE_EPSTATUSCLR_CURBK_Msk      (_U_(0x1) << USB_DEVICE_EPSTATUSCLR_CURBK_Pos)       /**< (USB_DEVICE_EPSTATUSCLR) Current Bank Clear Mask */
#define USB_DEVICE_EPSTATUSCLR_CURBK(value)   (USB_DEVICE_EPSTATUSCLR_CURBK_Msk & ((value) << USB_DEVICE_EPSTATUSCLR_CURBK_Pos))
#define USB_DEVICE_EPSTATUSCLR_STALLRQ0_Pos   _U_(4)                                               /**< (USB_DEVICE_EPSTATUSCLR) Stall 0 Request Clear Position */
#define USB_DEVICE_EPSTATUSCLR_STALLRQ0_Msk   (_U_(0x1) << USB_DEVICE_EPSTATUSCLR_STALLRQ0_Pos)    /**< (USB_DEVICE_EPSTATUSCLR) Stall 0 Request Clear Mask */
#define USB_DEVICE_EPSTATUSCLR_STALLRQ0(value) (USB_DEVICE_EPSTATUSCLR_STALLRQ0_Msk & ((value) << USB_DEVICE_EPSTATUSCLR_STALLRQ0_Pos))
#define USB_DEVICE_EPSTATUSCLR_STALLRQ1_Pos   _U_(5)                                               /**< (USB_DEVICE_EPSTATUSCLR) Stall 1 Request Clear Position */
#define USB_DEVICE_EPSTATUSCLR_STALLRQ1_Msk   (_U_(0x1) << USB_DEVICE_EPSTATUSCLR_STALLRQ1_Pos)    /**< (USB_DEVICE_EPSTATUSCLR) Stall 1 Request Clear Mask */
#define USB_DEVICE_EPSTATUSCLR_STALLRQ1(value) (USB_DEVICE_EPSTATUSCLR_STALLRQ1_Msk & ((value) << USB_DEVICE_EPSTATUSCLR_STALLRQ1_Pos))
#define USB_DEVICE_EPSTATUSCLR_BK0RDY_Pos     _U_(6)                                               /**< (USB_DEVICE_EPSTATUSCLR) Bank 0 Ready Clear Position */
#define USB_DEVICE_EPSTATUSCLR_BK0RDY_Msk     (_U_(0x1) << USB_DEVICE_EPSTATUSCLR_BK0RDY_Pos)      /**< (USB_DEVICE_EPSTATUSCLR) Bank 0 Ready Clear Mask */
#define USB_DEVICE_EPSTATUSCLR_BK0RDY(value)  (USB_DEVICE_EPSTATUSCLR_BK0RDY_Msk & ((value) << USB_DEVICE_EPSTATUSCLR_BK0RDY_Pos))
#define USB_DEVICE_EPSTATUSCLR_BK1RDY_Pos     _U_(7)                                               /**< (USB_DEVICE_EPSTATUSCLR) Bank 1 Ready Clear Position */
#define USB_DEVICE_EPSTATUSCLR_BK1RDY_Msk     (_U_(0x1) << USB_DEVICE_EPSTATUSCLR_BK1RDY_Pos)      /**< (USB_DEVICE_EPSTATUSCLR) Bank 1 Ready Clear Mask */
#define USB_DEVICE_EPSTATUSCLR_BK1RDY(value)  (USB_DEVICE_EPSTATUSCLR_BK1RDY_Msk & ((value) << USB_DEVICE_EPSTATUSCLR_BK1RDY_Pos))
#define USB_DEVICE_EPSTATUSCLR_Msk            _U_(0xF7)                                            /**< (USB_DEVICE_EPSTATUSCLR) Register Mask  */

#define USB_DEVICE_EPSTATUSCLR_STALLRQ_Pos    _U_(4)                                               /**< (USB_DEVICE_EPSTATUSCLR Position) Stall x Request Clear */
#define USB_DEVICE_EPSTATUSCLR_STALLRQ_Msk    (_U_(0x3) << USB_DEVICE_EPSTATUSCLR_STALLRQ_Pos)     /**< (USB_DEVICE_EPSTATUSCLR Mask) STALLRQ */
#define USB_DEVICE_EPSTATUSCLR_STALLRQ(value) (USB_DEVICE_EPSTATUSCLR_STALLRQ_Msk & ((value) << USB_DEVICE_EPSTATUSCLR_STALLRQ_Pos)) 

/* -------- USB_DEVICE_EPSTATUSSET : (USB Offset: 0x05) ( /W 8) DEVICE_ENDPOINT End Point Pipe Status Set -------- */
#define USB_DEVICE_EPSTATUSSET_RESETVALUE     _U_(0x00)                                            /**<  (USB_DEVICE_EPSTATUSSET) DEVICE_ENDPOINT End Point Pipe Status Set  Reset Value */

#define USB_DEVICE_EPSTATUSSET_DTGLOUT_Pos    _U_(0)                                               /**< (USB_DEVICE_EPSTATUSSET) Data Toggle OUT Set Position */
#define USB_DEVICE_EPSTATUSSET_DTGLOUT_Msk    (_U_(0x1) << USB_DEVICE_EPSTATUSSET_DTGLOUT_Pos)     /**< (USB_DEVICE_EPSTATUSSET) Data Toggle OUT Set Mask */
#define USB_DEVICE_EPSTATUSSET_DTGLOUT(value) (USB_DEVICE_EPSTATUSSET_DTGLOUT_Msk & ((value) << USB_DEVICE_EPSTATUSSET_DTGLOUT_Pos))
#define USB_DEVICE_EPSTATUSSET_DTGLIN_Pos     _U_(1)                                               /**< (USB_DEVICE_EPSTATUSSET) Data Toggle IN Set Position */
#define USB_DEVICE_EPSTATUSSET_DTGLIN_Msk     (_U_(0x1) << USB_DEVICE_EPSTATUSSET_DTGLIN_Pos)      /**< (USB_DEVICE_EPSTATUSSET) Data Toggle IN Set Mask */
#define USB_DEVICE_EPSTATUSSET_DTGLIN(value)  (USB_DEVICE_EPSTATUSSET_DTGLIN_Msk & ((value) << USB_DEVICE_EPSTATUSSET_DTGLIN_Pos))
#define USB_DEVICE_EPSTATUSSET_CURBK_Pos      _U_(2)                                               /**< (USB_DEVICE_EPSTATUSSET) Current Bank Set Position */
#define USB_DEVICE_EPSTATUSSET_CURBK_Msk      (_U_(0x1) << USB_DEVICE_EPSTATUSSET_CURBK_Pos)       /**< (USB_DEVICE_EPSTATUSSET) Current Bank Set Mask */
#define USB_DEVICE_EPSTATUSSET_CURBK(value)   (USB_DEVICE_EPSTATUSSET_CURBK_Msk & ((value) << USB_DEVICE_EPSTATUSSET_CURBK_Pos))
#define USB_DEVICE_EPSTATUSSET_STALLRQ0_Pos   _U_(4)                                               /**< (USB_DEVICE_EPSTATUSSET) Stall 0 Request Set Position */
#define USB_DEVICE_EPSTATUSSET_STALLRQ0_Msk   (_U_(0x1) << USB_DEVICE_EPSTATUSSET_STALLRQ0_Pos)    /**< (USB_DEVICE_EPSTATUSSET) Stall 0 Request Set Mask */
#define USB_DEVICE_EPSTATUSSET_STALLRQ0(value) (USB_DEVICE_EPSTATUSSET_STALLRQ0_Msk & ((value) << USB_DEVICE_EPSTATUSSET_STALLRQ0_Pos))
#define USB_DEVICE_EPSTATUSSET_STALLRQ1_Pos   _U_(5)                                               /**< (USB_DEVICE_EPSTATUSSET) Stall 1 Request Set Position */
#define USB_DEVICE_EPSTATUSSET_STALLRQ1_Msk   (_U_(0x1) << USB_DEVICE_EPSTATUSSET_STALLRQ1_Pos)    /**< (USB_DEVICE_EPSTATUSSET) Stall 1 Request Set Mask */
#define USB_DEVICE_EPSTATUSSET_STALLRQ1(value) (USB_DEVICE_EPSTATUSSET_STALLRQ1_Msk & ((value) << USB_DEVICE_EPSTATUSSET_STALLRQ1_Pos))
#define USB_DEVICE_EPSTATUSSET_BK0RDY_Pos     _U_(6)                                               /**< (USB_DEVICE_EPSTATUSSET) Bank 0 Ready Set Position */
#define USB_DEVICE_EPSTATUSSET_BK0RDY_Msk     (_U_(0x1) << USB_DEVICE_EPSTATUSSET_BK0RDY_Pos)      /**< (USB_DEVICE_EPSTATUSSET) Bank 0 Ready Set Mask */
#define USB_DEVICE_EPSTATUSSET_BK0RDY(value)  (USB_DEVICE_EPSTATUSSET_BK0RDY_Msk & ((value) << USB_DEVICE_EPSTATUSSET_BK0RDY_Pos))
#define USB_DEVICE_EPSTATUSSET_BK1RDY_Pos     _U_(7)                                               /**< (USB_DEVICE_EPSTATUSSET) Bank 1 Ready Set Position */
#define USB_DEVICE_EPSTATUSSET_BK1RDY_Msk     (_U_(0x1) << USB_DEVICE_EPSTATUSSET_BK1RDY_Pos)      /**< (USB_DEVICE_EPSTATUSSET) Bank 1 Ready Set Mask */
#define USB_DEVICE_EPSTATUSSET_BK1RDY(value)  (USB_DEVICE_EPSTATUSSET_BK1RDY_Msk & ((value) << USB_DEVICE_EPSTATUSSET_BK1RDY_Pos))
#define USB_DEVICE_EPSTATUSSET_Msk            _U_(0xF7)                                            /**< (USB_DEVICE_EPSTATUSSET) Register Mask  */

#define USB_DEVICE_EPSTATUSSET_STALLRQ_Pos    _U_(4)                                               /**< (USB_DEVICE_EPSTATUSSET Position) Stall x Request Set */
#define USB_DEVICE_EPSTATUSSET_STALLRQ_Msk    (_U_(0x3) << USB_DEVICE_EPSTATUSSET_STALLRQ_Pos)     /**< (USB_DEVICE_EPSTATUSSET Mask) STALLRQ */
#define USB_DEVICE_EPSTATUSSET_STALLRQ(value) (USB_DEVICE_EPSTATUSSET_STALLRQ_Msk & ((value) << USB_DEVICE_EPSTATUSSET_STALLRQ_Pos)) 

/* -------- USB_DEVICE_EPSTATUS : (USB Offset: 0x06) ( R/ 8) DEVICE_ENDPOINT End Point Pipe Status -------- */
#define USB_DEVICE_EPSTATUS_RESETVALUE        _U_(0x00)                                            /**<  (USB_DEVICE_EPSTATUS) DEVICE_ENDPOINT End Point Pipe Status  Reset Value */

#define USB_DEVICE_EPSTATUS_DTGLOUT_Pos       _U_(0)                                               /**< (USB_DEVICE_EPSTATUS) Data Toggle Out Position */
#define USB_DEVICE_EPSTATUS_DTGLOUT_Msk       (_U_(0x1) << USB_DEVICE_EPSTATUS_DTGLOUT_Pos)        /**< (USB_DEVICE_EPSTATUS) Data Toggle Out Mask */
#define USB_DEVICE_EPSTATUS_DTGLOUT(value)    (USB_DEVICE_EPSTATUS_DTGLOUT_Msk & ((value) << USB_DEVICE_EPSTATUS_DTGLOUT_Pos))
#define USB_DEVICE_EPSTATUS_DTGLIN_Pos        _U_(1)                                               /**< (USB_DEVICE_EPSTATUS) Data Toggle In Position */
#define USB_DEVICE_EPSTATUS_DTGLIN_Msk        (_U_(0x1) << USB_DEVICE_EPSTATUS_DTGLIN_Pos)         /**< (USB_DEVICE_EPSTATUS) Data Toggle In Mask */
#define USB_DEVICE_EPSTATUS_DTGLIN(value)     (USB_DEVICE_EPSTATUS_DTGLIN_Msk & ((value) << USB_DEVICE_EPSTATUS_DTGLIN_Pos))
#define USB_DEVICE_EPSTATUS_CURBK_Pos         _U_(2)                                               /**< (USB_DEVICE_EPSTATUS) Current Bank Position */
#define USB_DEVICE_EPSTATUS_CURBK_Msk         (_U_(0x1) << USB_DEVICE_EPSTATUS_CURBK_Pos)          /**< (USB_DEVICE_EPSTATUS) Current Bank Mask */
#define USB_DEVICE_EPSTATUS_CURBK(value)      (USB_DEVICE_EPSTATUS_CURBK_Msk & ((value) << USB_DEVICE_EPSTATUS_CURBK_Pos))
#define USB_DEVICE_EPSTATUS_STALLRQ0_Pos      _U_(4)                                               /**< (USB_DEVICE_EPSTATUS) Stall 0 Request Position */
#define USB_DEVICE_EPSTATUS_STALLRQ0_Msk      (_U_(0x1) << USB_DEVICE_EPSTATUS_STALLRQ0_Pos)       /**< (USB_DEVICE_EPSTATUS) Stall 0 Request Mask */
#define USB_DEVICE_EPSTATUS_STALLRQ0(value)   (USB_DEVICE_EPSTATUS_STALLRQ0_Msk & ((value) << USB_DEVICE_EPSTATUS_STALLRQ0_Pos))
#define USB_DEVICE_EPSTATUS_STALLRQ1_Pos      _U_(5)                                               /**< (USB_DEVICE_EPSTATUS) Stall 1 Request Position */
#define USB_DEVICE_EPSTATUS_STALLRQ1_Msk      (_U_(0x1) << USB_DEVICE_EPSTATUS_STALLRQ1_Pos)       /**< (USB_DEVICE_EPSTATUS) Stall 1 Request Mask */
#define USB_DEVICE_EPSTATUS_STALLRQ1(value)   (USB_DEVICE_EPSTATUS_STALLRQ1_Msk & ((value) << USB_DEVICE_EPSTATUS_STALLRQ1_Pos))
#define USB_DEVICE_EPSTATUS_BK0RDY_Pos        _U_(6)                                               /**< (USB_DEVICE_EPSTATUS) Bank 0 ready Position */
#define USB_DEVICE_EPSTATUS_BK0RDY_Msk        (_U_(0x1) << USB_DEVICE_EPSTATUS_BK0RDY_Pos)         /**< (USB_DEVICE_EPSTATUS) Bank 0 ready Mask */
#define USB_DEVICE_EPSTATUS_BK0RDY(value)     (USB_DEVICE_EPSTATUS_BK0RDY_Msk & ((value) << USB_DEVICE_EPSTATUS_BK0RDY_Pos))
#define USB_DEVICE_EPSTATUS_BK1RDY_Pos        _U_(7)                                               /**< (USB_DEVICE_EPSTATUS) Bank 1 ready Position */
#define USB_DEVICE_EPSTATUS_BK1RDY_Msk        (_U_(0x1) << USB_DEVICE_EPSTATUS_BK1RDY_Pos)         /**< (USB_DEVICE_EPSTATUS) Bank 1 ready Mask */
#define USB_DEVICE_EPSTATUS_BK1RDY(value)     (USB_DEVICE_EPSTATUS_BK1RDY_Msk & ((value) << USB_DEVICE_EPSTATUS_BK1RDY_Pos))
#define USB_DEVICE_EPSTATUS_Msk               _U_(0xF7)                                            /**< (USB_DEVICE_EPSTATUS) Register Mask  */

#define USB_DEVICE_EPSTATUS_STALLRQ_Pos       _U_(4)                                               /**< (USB_DEVICE_EPSTATUS Position) Stall x Request */
#define USB_DEVICE_EPSTATUS_STALLRQ_Msk       (_U_(0x3) << USB_DEVICE_EPSTATUS_STALLRQ_Pos)        /**< (USB_DEVICE_EPSTATUS Mask) STALLRQ */
#define USB_DEVICE_EPSTATUS_STALLRQ(value)    (USB_DEVICE_EPSTATUS_STALLRQ_Msk & ((value) << USB_DEVICE_EPSTATUS_STALLRQ_Pos)) 

/* -------- USB_DEVICE_EPINTFLAG : (USB Offset: 0x07) (R/W 8) DEVICE_ENDPOINT End Point Interrupt Flag -------- */
#define USB_DEVICE_EPINTFLAG_RESETVALUE       _U_(0x00)                                            /**<  (USB_DEVICE_EPINTFLAG) DEVICE_ENDPOINT End Point Interrupt Flag  Reset Value */

#define USB_DEVICE_EPINTFLAG_TRCPT0_Pos       _U_(0)                                               /**< (USB_DEVICE_EPINTFLAG) Transfer Complete 0 Position */
#define USB_DEVICE_EPINTFLAG_TRCPT0_Msk       (_U_(0x1) << USB_DEVICE_EPINTFLAG_TRCPT0_Pos)        /**< (USB_DEVICE_EPINTFLAG) Transfer Complete 0 Mask */
#define USB_DEVICE_EPINTFLAG_TRCPT0(value)    (USB_DEVICE_EPINTFLAG_TRCPT0_Msk & ((value) << USB_DEVICE_EPINTFLAG_TRCPT0_Pos))
#define USB_DEVICE_EPINTFLAG_TRCPT1_Pos       _U_(1)                                               /**< (USB_DEVICE_EPINTFLAG) Transfer Complete 1 Position */
#define USB_DEVICE_EPINTFLAG_TRCPT1_Msk       (_U_(0x1) << USB_DEVICE_EPINTFLAG_TRCPT1_Pos)        /**< (USB_DEVICE_EPINTFLAG) Transfer Complete 1 Mask */
#define USB_DEVICE_EPINTFLAG_TRCPT1(value)    (USB_DEVICE_EPINTFLAG_TRCPT1_Msk & ((value) << USB_DEVICE_EPINTFLAG_TRCPT1_Pos))
#define USB_DEVICE_EPINTFLAG_TRFAIL0_Pos      _U_(2)                                               /**< (USB_DEVICE_EPINTFLAG) Error Flow 0 Position */
#define USB_DEVICE_EPINTFLAG_TRFAIL0_Msk      (_U_(0x1) << USB_DEVICE_EPINTFLAG_TRFAIL0_Pos)       /**< (USB_DEVICE_EPINTFLAG) Error Flow 0 Mask */
#define USB_DEVICE_EPINTFLAG_TRFAIL0(value)   (USB_DEVICE_EPINTFLAG_TRFAIL0_Msk & ((value) << USB_DEVICE_EPINTFLAG_TRFAIL0_Pos))
#define USB_DEVICE_EPINTFLAG_TRFAIL1_Pos      _U_(3)                                               /**< (USB_DEVICE_EPINTFLAG) Error Flow 1 Position */
#define USB_DEVICE_EPINTFLAG_TRFAIL1_Msk      (_U_(0x1) << USB_DEVICE_EPINTFLAG_TRFAIL1_Pos)       /**< (USB_DEVICE_EPINTFLAG) Error Flow 1 Mask */
#define USB_DEVICE_EPINTFLAG_TRFAIL1(value)   (USB_DEVICE_EPINTFLAG_TRFAIL1_Msk & ((value) << USB_DEVICE_EPINTFLAG_TRFAIL1_Pos))
#define USB_DEVICE_EPINTFLAG_RXSTP_Pos        _U_(4)                                               /**< (USB_DEVICE_EPINTFLAG) Received Setup Position */
#define USB_DEVICE_EPINTFLAG_RXSTP_Msk        (_U_(0x1) << USB_DEVICE_EPINTFLAG_RXSTP_Pos)         /**< (USB_DEVICE_EPINTFLAG) Received Setup Mask */
#define USB_DEVICE_EPINTFLAG_RXSTP(value)     (USB_DEVICE_EPINTFLAG_RXSTP_Msk & ((value) << USB_DEVICE_EPINTFLAG_RXSTP_Pos))
#define USB_DEVICE_EPINTFLAG_STALL0_Pos       _U_(5)                                               /**< (USB_DEVICE_EPINTFLAG) Stall 0 In/out Position */
#define USB_DEVICE_EPINTFLAG_STALL0_Msk       (_U_(0x1) << USB_DEVICE_EPINTFLAG_STALL0_Pos)        /**< (USB_DEVICE_EPINTFLAG) Stall 0 In/out Mask */
#define USB_DEVICE_EPINTFLAG_STALL0(value)    (USB_DEVICE_EPINTFLAG_STALL0_Msk & ((value) << USB_DEVICE_EPINTFLAG_STALL0_Pos))
#define USB_DEVICE_EPINTFLAG_STALL1_Pos       _U_(6)                                               /**< (USB_DEVICE_EPINTFLAG) Stall 1 In/out Position */
#define USB_DEVICE_EPINTFLAG_STALL1_Msk       (_U_(0x1) << USB_DEVICE_EPINTFLAG_STALL1_Pos)        /**< (USB_DEVICE_EPINTFLAG) Stall 1 In/out Mask */
#define USB_DEVICE_EPINTFLAG_STALL1(value)    (USB_DEVICE_EPINTFLAG_STALL1_Msk & ((value) << USB_DEVICE_EPINTFLAG_STALL1_Pos))
#define USB_DEVICE_EPINTFLAG_Msk              _U_(0x7F)                                            /**< (USB_DEVICE_EPINTFLAG) Register Mask  */

#define USB_DEVICE_EPINTFLAG_TRCPT_Pos        _U_(0)                                               /**< (USB_DEVICE_EPINTFLAG Position) Transfer Complete x */
#define USB_DEVICE_EPINTFLAG_TRCPT_Msk        (_U_(0x3) << USB_DEVICE_EPINTFLAG_TRCPT_Pos)         /**< (USB_DEVICE_EPINTFLAG Mask) TRCPT */
#define USB_DEVICE_EPINTFLAG_TRCPT(value)     (USB_DEVICE_EPINTFLAG_TRCPT_Msk & ((value) << USB_DEVICE_EPINTFLAG_TRCPT_Pos)) 
#define USB_DEVICE_EPINTFLAG_TRFAIL_Pos       _U_(2)                                               /**< (USB_DEVICE_EPINTFLAG Position) Error Flow x */
#define USB_DEVICE_EPINTFLAG_TRFAIL_Msk       (_U_(0x3) << USB_DEVICE_EPINTFLAG_TRFAIL_Pos)        /**< (USB_DEVICE_EPINTFLAG Mask) TRFAIL */
#define USB_DEVICE_EPINTFLAG_TRFAIL(value)    (USB_DEVICE_EPINTFLAG_TRFAIL_Msk & ((value) << USB_DEVICE_EPINTFLAG_TRFAIL_Pos)) 
#define USB_DEVICE_EPINTFLAG_STALL_Pos        _U_(5)                                               /**< (USB_DEVICE_EPINTFLAG Position) Stall x In/out */
#define USB_DEVICE_EPINTFLAG_STALL_Msk        (_U_(0x3) << USB_DEVICE_EPINTFLAG_STALL_Pos)         /**< (USB_DEVICE_EPINTFLAG Mask) STALL */
#define USB_DEVICE_EPINTFLAG_STALL(value)     (USB_DEVICE_EPINTFLAG_STALL_Msk & ((value) << USB_DEVICE_EPINTFLAG_STALL_Pos)) 

/* -------- USB_DEVICE_EPINTENCLR : (USB Offset: 0x08) (R/W 8) DEVICE_ENDPOINT End Point Interrupt Clear Flag -------- */
#define USB_DEVICE_EPINTENCLR_RESETVALUE      _U_(0x00)                                            /**<  (USB_DEVICE_EPINTENCLR) DEVICE_ENDPOINT End Point Interrupt Clear Flag  Reset Value */

#define USB_DEVICE_EPINTENCLR_TRCPT0_Pos      _U_(0)                                               /**< (USB_DEVICE_EPINTENCLR) Transfer Complete 0 Interrupt Disable Position */
#define USB_DEVICE_EPINTENCLR_TRCPT0_Msk      (_U_(0x1) << USB_DEVICE_EPINTENCLR_TRCPT0_Pos)       /**< (USB_DEVICE_EPINTENCLR) Transfer Complete 0 Interrupt Disable Mask */
#define USB_DEVICE_EPINTENCLR_TRCPT0(value)   (USB_DEVICE_EPINTENCLR_TRCPT0_Msk & ((value) << USB_DEVICE_EPINTENCLR_TRCPT0_Pos))
#define USB_DEVICE_EPINTENCLR_TRCPT1_Pos      _U_(1)                                               /**< (USB_DEVICE_EPINTENCLR) Transfer Complete 1 Interrupt Disable Position */
#define USB_DEVICE_EPINTENCLR_TRCPT1_Msk      (_U_(0x1) << USB_DEVICE_EPINTENCLR_TRCPT1_Pos)       /**< (USB_DEVICE_EPINTENCLR) Transfer Complete 1 Interrupt Disable Mask */
#define USB_DEVICE_EPINTENCLR_TRCPT1(value)   (USB_DEVICE_EPINTENCLR_TRCPT1_Msk & ((value) << USB_DEVICE_EPINTENCLR_TRCPT1_Pos))
#define USB_DEVICE_EPINTENCLR_TRFAIL0_Pos     _U_(2)                                               /**< (USB_DEVICE_EPINTENCLR) Error Flow 0 Interrupt Disable Position */
#define USB_DEVICE_EPINTENCLR_TRFAIL0_Msk     (_U_(0x1) << USB_DEVICE_EPINTENCLR_TRFAIL0_Pos)      /**< (USB_DEVICE_EPINTENCLR) Error Flow 0 Interrupt Disable Mask */
#define USB_DEVICE_EPINTENCLR_TRFAIL0(value)  (USB_DEVICE_EPINTENCLR_TRFAIL0_Msk & ((value) << USB_DEVICE_EPINTENCLR_TRFAIL0_Pos))
#define USB_DEVICE_EPINTENCLR_TRFAIL1_Pos     _U_(3)                                               /**< (USB_DEVICE_EPINTENCLR) Error Flow 1 Interrupt Disable Position */
#define USB_DEVICE_EPINTENCLR_TRFAIL1_Msk     (_U_(0x1) << USB_DEVICE_EPINTENCLR_TRFAIL1_Pos)      /**< (USB_DEVICE_EPINTENCLR) Error Flow 1 Interrupt Disable Mask */
#define USB_DEVICE_EPINTENCLR_TRFAIL1(value)  (USB_DEVICE_EPINTENCLR_TRFAIL1_Msk & ((value) << USB_DEVICE_EPINTENCLR_TRFAIL1_Pos))
#define USB_DEVICE_EPINTENCLR_RXSTP_Pos       _U_(4)                                               /**< (USB_DEVICE_EPINTENCLR) Received Setup Interrupt Disable Position */
#define USB_DEVICE_EPINTENCLR_RXSTP_Msk       (_U_(0x1) << USB_DEVICE_EPINTENCLR_RXSTP_Pos)        /**< (USB_DEVICE_EPINTENCLR) Received Setup Interrupt Disable Mask */
#define USB_DEVICE_EPINTENCLR_RXSTP(value)    (USB_DEVICE_EPINTENCLR_RXSTP_Msk & ((value) << USB_DEVICE_EPINTENCLR_RXSTP_Pos))
#define USB_DEVICE_EPINTENCLR_STALL0_Pos      _U_(5)                                               /**< (USB_DEVICE_EPINTENCLR) Stall 0 In/Out Interrupt Disable Position */
#define USB_DEVICE_EPINTENCLR_STALL0_Msk      (_U_(0x1) << USB_DEVICE_EPINTENCLR_STALL0_Pos)       /**< (USB_DEVICE_EPINTENCLR) Stall 0 In/Out Interrupt Disable Mask */
#define USB_DEVICE_EPINTENCLR_STALL0(value)   (USB_DEVICE_EPINTENCLR_STALL0_Msk & ((value) << USB_DEVICE_EPINTENCLR_STALL0_Pos))
#define USB_DEVICE_EPINTENCLR_STALL1_Pos      _U_(6)                                               /**< (USB_DEVICE_EPINTENCLR) Stall 1 In/Out Interrupt Disable Position */
#define USB_DEVICE_EPINTENCLR_STALL1_Msk      (_U_(0x1) << USB_DEVICE_EPINTENCLR_STALL1_Pos)       /**< (USB_DEVICE_EPINTENCLR) Stall 1 In/Out Interrupt Disable Mask */
#define USB_DEVICE_EPINTENCLR_STALL1(value)   (USB_DEVICE_EPINTENCLR_STALL1_Msk & ((value) << USB_DEVICE_EPINTENCLR_STALL1_Pos))
#define USB_DEVICE_EPINTENCLR_Msk             _U_(0x7F)                                            /**< (USB_DEVICE_EPINTENCLR) Register Mask  */

#define USB_DEVICE_EPINTENCLR_TRCPT_Pos       _U_(0)                                               /**< (USB_DEVICE_EPINTENCLR Position) Transfer Complete x Interrupt Disable */
#define USB_DEVICE_EPINTENCLR_TRCPT_Msk       (_U_(0x3) << USB_DEVICE_EPINTENCLR_TRCPT_Pos)        /**< (USB_DEVICE_EPINTENCLR Mask) TRCPT */
#define USB_DEVICE_EPINTENCLR_TRCPT(value)    (USB_DEVICE_EPINTENCLR_TRCPT_Msk & ((value) << USB_DEVICE_EPINTENCLR_TRCPT_Pos)) 
#define USB_DEVICE_EPINTENCLR_TRFAIL_Pos      _U_(2)                                               /**< (USB_DEVICE_EPINTENCLR Position) Error Flow x Interrupt Disable */
#define USB_DEVICE_EPINTENCLR_TRFAIL_Msk      (_U_(0x3) << USB_DEVICE_EPINTENCLR_TRFAIL_Pos)       /**< (USB_DEVICE_EPINTENCLR Mask) TRFAIL */
#define USB_DEVICE_EPINTENCLR_TRFAIL(value)   (USB_DEVICE_EPINTENCLR_TRFAIL_Msk & ((value) << USB_DEVICE_EPINTENCLR_TRFAIL_Pos)) 
#define USB_DEVICE_EPINTENCLR_STALL_Pos       _U_(5)                                               /**< (USB_DEVICE_EPINTENCLR Position) Stall x In/Out Interrupt Disable */
#define USB_DEVICE_EPINTENCLR_STALL_Msk       (_U_(0x3) << USB_DEVICE_EPINTENCLR_STALL_Pos)        /**< (USB_DEVICE_EPINTENCLR Mask) STALL */
#define USB_DEVICE_EPINTENCLR_STALL(value)    (USB_DEVICE_EPINTENCLR_STALL_Msk & ((value) << USB_DEVICE_EPINTENCLR_STALL_Pos)) 

/* -------- USB_DEVICE_EPINTENSET : (USB Offset: 0x09) (R/W 8) DEVICE_ENDPOINT End Point Interrupt Set Flag -------- */
#define USB_DEVICE_EPINTENSET_RESETVALUE      _U_(0x00)                                            /**<  (USB_DEVICE_EPINTENSET) DEVICE_ENDPOINT End Point Interrupt Set Flag  Reset Value */

#define USB_DEVICE_EPINTENSET_TRCPT0_Pos      _U_(0)                                               /**< (USB_DEVICE_EPINTENSET) Transfer Complete 0 Interrupt Enable Position */
#define USB_DEVICE_EPINTENSET_TRCPT0_Msk      (_U_(0x1) << USB_DEVICE_EPINTENSET_TRCPT0_Pos)       /**< (USB_DEVICE_EPINTENSET) Transfer Complete 0 Interrupt Enable Mask */
#define USB_DEVICE_EPINTENSET_TRCPT0(value)   (USB_DEVICE_EPINTENSET_TRCPT0_Msk & ((value) << USB_DEVICE_EPINTENSET_TRCPT0_Pos))
#define USB_DEVICE_EPINTENSET_TRCPT1_Pos      _U_(1)                                               /**< (USB_DEVICE_EPINTENSET) Transfer Complete 1 Interrupt Enable Position */
#define USB_DEVICE_EPINTENSET_TRCPT1_Msk      (_U_(0x1) << USB_DEVICE_EPINTENSET_TRCPT1_Pos)       /**< (USB_DEVICE_EPINTENSET) Transfer Complete 1 Interrupt Enable Mask */
#define USB_DEVICE_EPINTENSET_TRCPT1(value)   (USB_DEVICE_EPINTENSET_TRCPT1_Msk & ((value) << USB_DEVICE_EPINTENSET_TRCPT1_Pos))
#define USB_DEVICE_EPINTENSET_TRFAIL0_Pos     _U_(2)                                               /**< (USB_DEVICE_EPINTENSET) Error Flow 0 Interrupt Enable Position */
#define USB_DEVICE_EPINTENSET_TRFAIL0_Msk     (_U_(0x1) << USB_DEVICE_EPINTENSET_TRFAIL0_Pos)      /**< (USB_DEVICE_EPINTENSET) Error Flow 0 Interrupt Enable Mask */
#define USB_DEVICE_EPINTENSET_TRFAIL0(value)  (USB_DEVICE_EPINTENSET_TRFAIL0_Msk & ((value) << USB_DEVICE_EPINTENSET_TRFAIL0_Pos))
#define USB_DEVICE_EPINTENSET_TRFAIL1_Pos     _U_(3)                                               /**< (USB_DEVICE_EPINTENSET) Error Flow 1 Interrupt Enable Position */
#define USB_DEVICE_EPINTENSET_TRFAIL1_Msk     (_U_(0x1) << USB_DEVICE_EPINTENSET_TRFAIL1_Pos)      /**< (USB_DEVICE_EPINTENSET) Error Flow 1 Interrupt Enable Mask */
#define USB_DEVICE_EPINTENSET_TRFAIL1(value)  (USB_DEVICE_EPINTENSET_TRFAIL1_Msk & ((value) << USB_DEVICE_EPINTENSET_TRFAIL1_Pos))
#define USB_DEVICE_EPINTENSET_RXSTP_Pos       _U_(4)                                               /**< (USB_DEVICE_EPINTENSET) Received Setup Interrupt Enable Position */
#define USB_DEVICE_EPINTENSET_RXSTP_Msk       (_U_(0x1) << USB_DEVICE_EPINTENSET_RXSTP_Pos)        /**< (USB_DEVICE_EPINTENSET) Received Setup Interrupt Enable Mask */
#define USB_DEVICE_EPINTENSET_RXSTP(value)    (USB_DEVICE_EPINTENSET_RXSTP_Msk & ((value) << USB_DEVICE_EPINTENSET_RXSTP_Pos))
#define USB_DEVICE_EPINTENSET_STALL0_Pos      _U_(5)                                               /**< (USB_DEVICE_EPINTENSET) Stall 0 In/out Interrupt enable Position */
#define USB_DEVICE_EPINTENSET_STALL0_Msk      (_U_(0x1) << USB_DEVICE_EPINTENSET_STALL0_Pos)       /**< (USB_DEVICE_EPINTENSET) Stall 0 In/out Interrupt enable Mask */
#define USB_DEVICE_EPINTENSET_STALL0(value)   (USB_DEVICE_EPINTENSET_STALL0_Msk & ((value) << USB_DEVICE_EPINTENSET_STALL0_Pos))
#define USB_DEVICE_EPINTENSET_STALL1_Pos      _U_(6)                                               /**< (USB_DEVICE_EPINTENSET) Stall 1 In/out Interrupt enable Position */
#define USB_DEVICE_EPINTENSET_STALL1_Msk      (_U_(0x1) << USB_DEVICE_EPINTENSET_STALL1_Pos)       /**< (USB_DEVICE_EPINTENSET) Stall 1 In/out Interrupt enable Mask */
#define USB_DEVICE_EPINTENSET_STALL1(value)   (USB_DEVICE_EPINTENSET_STALL1_Msk & ((value) << USB_DEVICE_EPINTENSET_STALL1_Pos))
#define USB_DEVICE_EPINTENSET_Msk             _U_(0x7F)                                            /**< (USB_DEVICE_EPINTENSET) Register Mask  */

#define USB_DEVICE_EPINTENSET_TRCPT_Pos       _U_(0)                                               /**< (USB_DEVICE_EPINTENSET Position) Transfer Complete x Interrupt Enable */
#define USB_DEVICE_EPINTENSET_TRCPT_Msk       (_U_(0x3) << USB_DEVICE_EPINTENSET_TRCPT_Pos)        /**< (USB_DEVICE_EPINTENSET Mask) TRCPT */
#define USB_DEVICE_EPINTENSET_TRCPT(value)    (USB_DEVICE_EPINTENSET_TRCPT_Msk & ((value) << USB_DEVICE_EPINTENSET_TRCPT_Pos)) 
#define USB_DEVICE_EPINTENSET_TRFAIL_Pos      _U_(2)                                               /**< (USB_DEVICE_EPINTENSET Position) Error Flow x Interrupt Enable */
#define USB_DEVICE_EPINTENSET_TRFAIL_Msk      (_U_(0x3) << USB_DEVICE_EPINTENSET_TRFAIL_Pos)       /**< (USB_DEVICE_EPINTENSET Mask) TRFAIL */
#define USB_DEVICE_EPINTENSET_TRFAIL(value)   (USB_DEVICE_EPINTENSET_TRFAIL_Msk & ((value) << USB_DEVICE_EPINTENSET_TRFAIL_Pos)) 
#define USB_DEVICE_EPINTENSET_STALL_Pos       _U_(5)                                               /**< (USB_DEVICE_EPINTENSET Position) Stall x In/out Interrupt enable */
#define USB_DEVICE_EPINTENSET_STALL_Msk       (_U_(0x3) << USB_DEVICE_EPINTENSET_STALL_Pos)        /**< (USB_DEVICE_EPINTENSET Mask) STALL */
#define USB_DEVICE_EPINTENSET_STALL(value)    (USB_DEVICE_EPINTENSET_STALL_Msk & ((value) << USB_DEVICE_EPINTENSET_STALL_Pos)) 

/* -------- USB_HOST_PCFG : (USB Offset: 0x00) (R/W 8) HOST_PIPE End Point Configuration -------- */
#define USB_HOST_PCFG_RESETVALUE              _U_(0x00)                                            /**<  (USB_HOST_PCFG) HOST_PIPE End Point Configuration  Reset Value */

#define USB_HOST_PCFG_PTOKEN_Pos              _U_(0)                                               /**< (USB_HOST_PCFG) Pipe Token Position */
#define USB_HOST_PCFG_PTOKEN_Msk              (_U_(0x3) << USB_HOST_PCFG_PTOKEN_Pos)               /**< (USB_HOST_PCFG) Pipe Token Mask */
#define USB_HOST_PCFG_PTOKEN(value)           (USB_HOST_PCFG_PTOKEN_Msk & ((value) << USB_HOST_PCFG_PTOKEN_Pos))
#define USB_HOST_PCFG_BK_Pos                  _U_(2)                                               /**< (USB_HOST_PCFG) Pipe Bank Position */
#define USB_HOST_PCFG_BK_Msk                  (_U_(0x1) << USB_HOST_PCFG_BK_Pos)                   /**< (USB_HOST_PCFG) Pipe Bank Mask */
#define USB_HOST_PCFG_BK(value)               (USB_HOST_PCFG_BK_Msk & ((value) << USB_HOST_PCFG_BK_Pos))
#define USB_HOST_PCFG_PTYPE_Pos               _U_(3)                                               /**< (USB_HOST_PCFG) Pipe Type Position */
#define USB_HOST_PCFG_PTYPE_Msk               (_U_(0x7) << USB_HOST_PCFG_PTYPE_Pos)                /**< (USB_HOST_PCFG) Pipe Type Mask */
#define USB_HOST_PCFG_PTYPE(value)            (USB_HOST_PCFG_PTYPE_Msk & ((value) << USB_HOST_PCFG_PTYPE_Pos))
#define USB_HOST_PCFG_Msk                     _U_(0x3F)                                            /**< (USB_HOST_PCFG) Register Mask  */


/* -------- USB_HOST_BINTERVAL : (USB Offset: 0x03) (R/W 8) HOST_PIPE Bus Access Period of Pipe -------- */
#define USB_HOST_BINTERVAL_RESETVALUE         _U_(0x00)                                            /**<  (USB_HOST_BINTERVAL) HOST_PIPE Bus Access Period of Pipe  Reset Value */

#define USB_HOST_BINTERVAL_BITINTERVAL_Pos    _U_(0)                                               /**< (USB_HOST_BINTERVAL) Bit Interval Position */
#define USB_HOST_BINTERVAL_BITINTERVAL_Msk    (_U_(0xFF) << USB_HOST_BINTERVAL_BITINTERVAL_Pos)    /**< (USB_HOST_BINTERVAL) Bit Interval Mask */
#define USB_HOST_BINTERVAL_BITINTERVAL(value) (USB_HOST_BINTERVAL_BITINTERVAL_Msk & ((value) << USB_HOST_BINTERVAL_BITINTERVAL_Pos))
#define USB_HOST_BINTERVAL_Msk                _U_(0xFF)                                            /**< (USB_HOST_BINTERVAL) Register Mask  */


/* -------- USB_HOST_PSTATUSCLR : (USB Offset: 0x04) ( /W 8) HOST_PIPE End Point Pipe Status Clear -------- */
#define USB_HOST_PSTATUSCLR_RESETVALUE        _U_(0x00)                                            /**<  (USB_HOST_PSTATUSCLR) HOST_PIPE End Point Pipe Status Clear  Reset Value */

#define USB_HOST_PSTATUSCLR_DTGL_Pos          _U_(0)                                               /**< (USB_HOST_PSTATUSCLR) Data Toggle clear Position */
#define USB_HOST_PSTATUSCLR_DTGL_Msk          (_U_(0x1) << USB_HOST_PSTATUSCLR_DTGL_Pos)           /**< (USB_HOST_PSTATUSCLR) Data Toggle clear Mask */
#define USB_HOST_PSTATUSCLR_DTGL(value)       (USB_HOST_PSTATUSCLR_DTGL_Msk & ((value) << USB_HOST_PSTATUSCLR_DTGL_Pos))
#define USB_HOST_PSTATUSCLR_CURBK_Pos         _U_(2)                                               /**< (USB_HOST_PSTATUSCLR) Curren Bank clear Position */
#define USB_HOST_PSTATUSCLR_CURBK_Msk         (_U_(0x1) << USB_HOST_PSTATUSCLR_CURBK_Pos)          /**< (USB_HOST_PSTATUSCLR) Curren Bank clear Mask */
#define USB_HOST_PSTATUSCLR_CURBK(value)      (USB_HOST_PSTATUSCLR_CURBK_Msk & ((value) << USB_HOST_PSTATUSCLR_CURBK_Pos))
#define USB_HOST_PSTATUSCLR_PFREEZE_Pos       _U_(4)                                               /**< (USB_HOST_PSTATUSCLR) Pipe Freeze Clear Position */
#define USB_HOST_PSTATUSCLR_PFREEZE_Msk       (_U_(0x1) << USB_HOST_PSTATUSCLR_PFREEZE_Pos)        /**< (USB_HOST_PSTATUSCLR) Pipe Freeze Clear Mask */
#define USB_HOST_PSTATUSCLR_PFREEZE(value)    (USB_HOST_PSTATUSCLR_PFREEZE_Msk & ((value) << USB_HOST_PSTATUSCLR_PFREEZE_Pos))
#define USB_HOST_PSTATUSCLR_BK0RDY_Pos        _U_(6)                                               /**< (USB_HOST_PSTATUSCLR) Bank 0 Ready Clear Position */
#define USB_HOST_PSTATUSCLR_BK0RDY_Msk        (_U_(0x1) << USB_HOST_PSTATUSCLR_BK0RDY_Pos)         /**< (USB_HOST_PSTATUSCLR) Bank 0 Ready Clear Mask */
#define USB_HOST_PSTATUSCLR_BK0RDY(value)     (USB_HOST_PSTATUSCLR_BK0RDY_Msk & ((value) << USB_HOST_PSTATUSCLR_BK0RDY_Pos))
#define USB_HOST_PSTATUSCLR_BK1RDY_Pos        _U_(7)                                               /**< (USB_HOST_PSTATUSCLR) Bank 1 Ready Clear Position */
#define USB_HOST_PSTATUSCLR_BK1RDY_Msk        (_U_(0x1) << USB_HOST_PSTATUSCLR_BK1RDY_Pos)         /**< (USB_HOST_PSTATUSCLR) Bank 1 Ready Clear Mask */
#define USB_HOST_PSTATUSCLR_BK1RDY(value)     (USB_HOST_PSTATUSCLR_BK1RDY_Msk & ((value) << USB_HOST_PSTATUSCLR_BK1RDY_Pos))
#define USB_HOST_PSTATUSCLR_Msk               _U_(0xD5)                                            /**< (USB_HOST_PSTATUSCLR) Register Mask  */


/* -------- USB_HOST_PSTATUSSET : (USB Offset: 0x05) ( /W 8) HOST_PIPE End Point Pipe Status Set -------- */
#define USB_HOST_PSTATUSSET_RESETVALUE        _U_(0x00)                                            /**<  (USB_HOST_PSTATUSSET) HOST_PIPE End Point Pipe Status Set  Reset Value */

#define USB_HOST_PSTATUSSET_DTGL_Pos          _U_(0)                                               /**< (USB_HOST_PSTATUSSET) Data Toggle Set Position */
#define USB_HOST_PSTATUSSET_DTGL_Msk          (_U_(0x1) << USB_HOST_PSTATUSSET_DTGL_Pos)           /**< (USB_HOST_PSTATUSSET) Data Toggle Set Mask */
#define USB_HOST_PSTATUSSET_DTGL(value)       (USB_HOST_PSTATUSSET_DTGL_Msk & ((value) << USB_HOST_PSTATUSSET_DTGL_Pos))
#define USB_HOST_PSTATUSSET_CURBK_Pos         _U_(2)                                               /**< (USB_HOST_PSTATUSSET) Current Bank Set Position */
#define USB_HOST_PSTATUSSET_CURBK_Msk         (_U_(0x1) << USB_HOST_PSTATUSSET_CURBK_Pos)          /**< (USB_HOST_PSTATUSSET) Current Bank Set Mask */
#define USB_HOST_PSTATUSSET_CURBK(value)      (USB_HOST_PSTATUSSET_CURBK_Msk & ((value) << USB_HOST_PSTATUSSET_CURBK_Pos))
#define USB_HOST_PSTATUSSET_PFREEZE_Pos       _U_(4)                                               /**< (USB_HOST_PSTATUSSET) Pipe Freeze Set Position */
#define USB_HOST_PSTATUSSET_PFREEZE_Msk       (_U_(0x1) << USB_HOST_PSTATUSSET_PFREEZE_Pos)        /**< (USB_HOST_PSTATUSSET) Pipe Freeze Set Mask */
#define USB_HOST_PSTATUSSET_PFREEZE(value)    (USB_HOST_PSTATUSSET_PFREEZE_Msk & ((value) << USB_HOST_PSTATUSSET_PFREEZE_Pos))
#define USB_HOST_PSTATUSSET_BK0RDY_Pos        _U_(6)                                               /**< (USB_HOST_PSTATUSSET) Bank 0 Ready Set Position */
#define USB_HOST_PSTATUSSET_BK0RDY_Msk        (_U_(0x1) << USB_HOST_PSTATUSSET_BK0RDY_Pos)         /**< (USB_HOST_PSTATUSSET) Bank 0 Ready Set Mask */
#define USB_HOST_PSTATUSSET_BK0RDY(value)     (USB_HOST_PSTATUSSET_BK0RDY_Msk & ((value) << USB_HOST_PSTATUSSET_BK0RDY_Pos))
#define USB_HOST_PSTATUSSET_BK1RDY_Pos        _U_(7)                                               /**< (USB_HOST_PSTATUSSET) Bank 1 Ready Set Position */
#define USB_HOST_PSTATUSSET_BK1RDY_Msk        (_U_(0x1) << USB_HOST_PSTATUSSET_BK1RDY_Pos)         /**< (USB_HOST_PSTATUSSET) Bank 1 Ready Set Mask */
#define USB_HOST_PSTATUSSET_BK1RDY(value)     (USB_HOST_PSTATUSSET_BK1RDY_Msk & ((value) << USB_HOST_PSTATUSSET_BK1RDY_Pos))
#define USB_HOST_PSTATUSSET_Msk               _U_(0xD5)                                            /**< (USB_HOST_PSTATUSSET) Register Mask  */


/* -------- USB_HOST_PSTATUS : (USB Offset: 0x06) ( R/ 8) HOST_PIPE End Point Pipe Status -------- */
#define USB_HOST_PSTATUS_RESETVALUE           _U_(0x00)                                            /**<  (USB_HOST_PSTATUS) HOST_PIPE End Point Pipe Status  Reset Value */

#define USB_HOST_PSTATUS_DTGL_Pos             _U_(0)                                               /**< (USB_HOST_PSTATUS) Data Toggle Position */
#define USB_HOST_PSTATUS_DTGL_Msk             (_U_(0x1) << USB_HOST_PSTATUS_DTGL_Pos)              /**< (USB_HOST_PSTATUS) Data Toggle Mask */
#define USB_HOST_PSTATUS_DTGL(value)          (USB_HOST_PSTATUS_DTGL_Msk & ((value) << USB_HOST_PSTATUS_DTGL_Pos))
#define USB_HOST_PSTATUS_CURBK_Pos            _U_(2)                                               /**< (USB_HOST_PSTATUS) Current Bank Position */
#define USB_HOST_PSTATUS_CURBK_Msk            (_U_(0x1) << USB_HOST_PSTATUS_CURBK_Pos)             /**< (USB_HOST_PSTATUS) Current Bank Mask */
#define USB_HOST_PSTATUS_CURBK(value)         (USB_HOST_PSTATUS_CURBK_Msk & ((value) << USB_HOST_PSTATUS_CURBK_Pos))
#define USB_HOST_PSTATUS_PFREEZE_Pos          _U_(4)                                               /**< (USB_HOST_PSTATUS) Pipe Freeze Position */
#define USB_HOST_PSTATUS_PFREEZE_Msk          (_U_(0x1) << USB_HOST_PSTATUS_PFREEZE_Pos)           /**< (USB_HOST_PSTATUS) Pipe Freeze Mask */
#define USB_HOST_PSTATUS_PFREEZE(value)       (USB_HOST_PSTATUS_PFREEZE_Msk & ((value) << USB_HOST_PSTATUS_PFREEZE_Pos))
#define USB_HOST_PSTATUS_BK0RDY_Pos           _U_(6)                                               /**< (USB_HOST_PSTATUS) Bank 0 ready Position */
#define USB_HOST_PSTATUS_BK0RDY_Msk           (_U_(0x1) << USB_HOST_PSTATUS_BK0RDY_Pos)            /**< (USB_HOST_PSTATUS) Bank 0 ready Mask */
#define USB_HOST_PSTATUS_BK0RDY(value)        (USB_HOST_PSTATUS_BK0RDY_Msk & ((value) << USB_HOST_PSTATUS_BK0RDY_Pos))
#define USB_HOST_PSTATUS_BK1RDY_Pos           _U_(7)                                               /**< (USB_HOST_PSTATUS) Bank 1 ready Position */
#define USB_HOST_PSTATUS_BK1RDY_Msk           (_U_(0x1) << USB_HOST_PSTATUS_BK1RDY_Pos)            /**< (USB_HOST_PSTATUS) Bank 1 ready Mask */
#define USB_HOST_PSTATUS_BK1RDY(value)        (USB_HOST_PSTATUS_BK1RDY_Msk & ((value) << USB_HOST_PSTATUS_BK1RDY_Pos))
#define USB_HOST_PSTATUS_Msk                  _U_(0xD5)                                            /**< (USB_HOST_PSTATUS) Register Mask  */


/* -------- USB_HOST_PINTFLAG : (USB Offset: 0x07) (R/W 8) HOST_PIPE Pipe Interrupt Flag -------- */
#define USB_HOST_PINTFLAG_RESETVALUE          _U_(0x00)                                            /**<  (USB_HOST_PINTFLAG) HOST_PIPE Pipe Interrupt Flag  Reset Value */

#define USB_HOST_PINTFLAG_TRCPT0_Pos          _U_(0)                                               /**< (USB_HOST_PINTFLAG) Transfer Complete 0 Interrupt Flag Position */
#define USB_HOST_PINTFLAG_TRCPT0_Msk          (_U_(0x1) << USB_HOST_PINTFLAG_TRCPT0_Pos)           /**< (USB_HOST_PINTFLAG) Transfer Complete 0 Interrupt Flag Mask */
#define USB_HOST_PINTFLAG_TRCPT0(value)       (USB_HOST_PINTFLAG_TRCPT0_Msk & ((value) << USB_HOST_PINTFLAG_TRCPT0_Pos))
#define USB_HOST_PINTFLAG_TRCPT1_Pos          _U_(1)                                               /**< (USB_HOST_PINTFLAG) Transfer Complete 1 Interrupt Flag Position */
#define USB_HOST_PINTFLAG_TRCPT1_Msk          (_U_(0x1) << USB_HOST_PINTFLAG_TRCPT1_Pos)           /**< (USB_HOST_PINTFLAG) Transfer Complete 1 Interrupt Flag Mask */
#define USB_HOST_PINTFLAG_TRCPT1(value)       (USB_HOST_PINTFLAG_TRCPT1_Msk & ((value) << USB_HOST_PINTFLAG_TRCPT1_Pos))
#define USB_HOST_PINTFLAG_TRFAIL_Pos          _U_(2)                                               /**< (USB_HOST_PINTFLAG) Error Flow Interrupt Flag Position */
#define USB_HOST_PINTFLAG_TRFAIL_Msk          (_U_(0x1) << USB_HOST_PINTFLAG_TRFAIL_Pos)           /**< (USB_HOST_PINTFLAG) Error Flow Interrupt Flag Mask */
#define USB_HOST_PINTFLAG_TRFAIL(value)       (USB_HOST_PINTFLAG_TRFAIL_Msk & ((value) << USB_HOST_PINTFLAG_TRFAIL_Pos))
#define USB_HOST_PINTFLAG_PERR_Pos            _U_(3)                                               /**< (USB_HOST_PINTFLAG) Pipe Error Interrupt Flag Position */
#define USB_HOST_PINTFLAG_PERR_Msk            (_U_(0x1) << USB_HOST_PINTFLAG_PERR_Pos)             /**< (USB_HOST_PINTFLAG) Pipe Error Interrupt Flag Mask */
#define USB_HOST_PINTFLAG_PERR(value)         (USB_HOST_PINTFLAG_PERR_Msk & ((value) << USB_HOST_PINTFLAG_PERR_Pos))
#define USB_HOST_PINTFLAG_TXSTP_Pos           _U_(4)                                               /**< (USB_HOST_PINTFLAG) Transmit  Setup Interrupt Flag Position */
#define USB_HOST_PINTFLAG_TXSTP_Msk           (_U_(0x1) << USB_HOST_PINTFLAG_TXSTP_Pos)            /**< (USB_HOST_PINTFLAG) Transmit  Setup Interrupt Flag Mask */
#define USB_HOST_PINTFLAG_TXSTP(value)        (USB_HOST_PINTFLAG_TXSTP_Msk & ((value) << USB_HOST_PINTFLAG_TXSTP_Pos))
#define USB_HOST_PINTFLAG_STALL_Pos           _U_(5)                                               /**< (USB_HOST_PINTFLAG) Stall Interrupt Flag Position */
#define USB_HOST_PINTFLAG_STALL_Msk           (_U_(0x1) << USB_HOST_PINTFLAG_STALL_Pos)            /**< (USB_HOST_PINTFLAG) Stall Interrupt Flag Mask */
#define USB_HOST_PINTFLAG_STALL(value)        (USB_HOST_PINTFLAG_STALL_Msk & ((value) << USB_HOST_PINTFLAG_STALL_Pos))
#define USB_HOST_PINTFLAG_Msk                 _U_(0x3F)                                            /**< (USB_HOST_PINTFLAG) Register Mask  */

#define USB_HOST_PINTFLAG_TRCPT_Pos           _U_(0)                                               /**< (USB_HOST_PINTFLAG Position) Transfer Complete x Interrupt Flag */
#define USB_HOST_PINTFLAG_TRCPT_Msk           (_U_(0x3) << USB_HOST_PINTFLAG_TRCPT_Pos)            /**< (USB_HOST_PINTFLAG Mask) TRCPT */
#define USB_HOST_PINTFLAG_TRCPT(value)        (USB_HOST_PINTFLAG_TRCPT_Msk & ((value) << USB_HOST_PINTFLAG_TRCPT_Pos)) 

/* -------- USB_HOST_PINTENCLR : (USB Offset: 0x08) (R/W 8) HOST_PIPE Pipe Interrupt Flag Clear -------- */
#define USB_HOST_PINTENCLR_RESETVALUE         _U_(0x00)                                            /**<  (USB_HOST_PINTENCLR) HOST_PIPE Pipe Interrupt Flag Clear  Reset Value */

#define USB_HOST_PINTENCLR_TRCPT0_Pos         _U_(0)                                               /**< (USB_HOST_PINTENCLR) Transfer Complete 0 Disable Position */
#define USB_HOST_PINTENCLR_TRCPT0_Msk         (_U_(0x1) << USB_HOST_PINTENCLR_TRCPT0_Pos)          /**< (USB_HOST_PINTENCLR) Transfer Complete 0 Disable Mask */
#define USB_HOST_PINTENCLR_TRCPT0(value)      (USB_HOST_PINTENCLR_TRCPT0_Msk & ((value) << USB_HOST_PINTENCLR_TRCPT0_Pos))
#define USB_HOST_PINTENCLR_TRCPT1_Pos         _U_(1)                                               /**< (USB_HOST_PINTENCLR) Transfer Complete 1 Disable Position */
#define USB_HOST_PINTENCLR_TRCPT1_Msk         (_U_(0x1) << USB_HOST_PINTENCLR_TRCPT1_Pos)          /**< (USB_HOST_PINTENCLR) Transfer Complete 1 Disable Mask */
#define USB_HOST_PINTENCLR_TRCPT1(value)      (USB_HOST_PINTENCLR_TRCPT1_Msk & ((value) << USB_HOST_PINTENCLR_TRCPT1_Pos))
#define USB_HOST_PINTENCLR_TRFAIL_Pos         _U_(2)                                               /**< (USB_HOST_PINTENCLR) Error Flow Interrupt Disable Position */
#define USB_HOST_PINTENCLR_TRFAIL_Msk         (_U_(0x1) << USB_HOST_PINTENCLR_TRFAIL_Pos)          /**< (USB_HOST_PINTENCLR) Error Flow Interrupt Disable Mask */
#define USB_HOST_PINTENCLR_TRFAIL(value)      (USB_HOST_PINTENCLR_TRFAIL_Msk & ((value) << USB_HOST_PINTENCLR_TRFAIL_Pos))
#define USB_HOST_PINTENCLR_PERR_Pos           _U_(3)                                               /**< (USB_HOST_PINTENCLR) Pipe Error Interrupt Disable Position */
#define USB_HOST_PINTENCLR_PERR_Msk           (_U_(0x1) << USB_HOST_PINTENCLR_PERR_Pos)            /**< (USB_HOST_PINTENCLR) Pipe Error Interrupt Disable Mask */
#define USB_HOST_PINTENCLR_PERR(value)        (USB_HOST_PINTENCLR_PERR_Msk & ((value) << USB_HOST_PINTENCLR_PERR_Pos))
#define USB_HOST_PINTENCLR_TXSTP_Pos          _U_(4)                                               /**< (USB_HOST_PINTENCLR) Transmit Setup Interrupt Disable Position */
#define USB_HOST_PINTENCLR_TXSTP_Msk          (_U_(0x1) << USB_HOST_PINTENCLR_TXSTP_Pos)           /**< (USB_HOST_PINTENCLR) Transmit Setup Interrupt Disable Mask */
#define USB_HOST_PINTENCLR_TXSTP(value)       (USB_HOST_PINTENCLR_TXSTP_Msk & ((value) << USB_HOST_PINTENCLR_TXSTP_Pos))
#define USB_HOST_PINTENCLR_STALL_Pos          _U_(5)                                               /**< (USB_HOST_PINTENCLR) Stall Inetrrupt Disable Position */
#define USB_HOST_PINTENCLR_STALL_Msk          (_U_(0x1) << USB_HOST_PINTENCLR_STALL_Pos)           /**< (USB_HOST_PINTENCLR) Stall Inetrrupt Disable Mask */
#define USB_HOST_PINTENCLR_STALL(value)       (USB_HOST_PINTENCLR_STALL_Msk & ((value) << USB_HOST_PINTENCLR_STALL_Pos))
#define USB_HOST_PINTENCLR_Msk                _U_(0x3F)                                            /**< (USB_HOST_PINTENCLR) Register Mask  */

#define USB_HOST_PINTENCLR_TRCPT_Pos          _U_(0)                                               /**< (USB_HOST_PINTENCLR Position) Transfer Complete x Disable */
#define USB_HOST_PINTENCLR_TRCPT_Msk          (_U_(0x3) << USB_HOST_PINTENCLR_TRCPT_Pos)           /**< (USB_HOST_PINTENCLR Mask) TRCPT */
#define USB_HOST_PINTENCLR_TRCPT(value)       (USB_HOST_PINTENCLR_TRCPT_Msk & ((value) << USB_HOST_PINTENCLR_TRCPT_Pos)) 

/* -------- USB_HOST_PINTENSET : (USB Offset: 0x09) (R/W 8) HOST_PIPE Pipe Interrupt Flag Set -------- */
#define USB_HOST_PINTENSET_RESETVALUE         _U_(0x00)                                            /**<  (USB_HOST_PINTENSET) HOST_PIPE Pipe Interrupt Flag Set  Reset Value */

#define USB_HOST_PINTENSET_TRCPT0_Pos         _U_(0)                                               /**< (USB_HOST_PINTENSET) Transfer Complete 0 Interrupt Enable Position */
#define USB_HOST_PINTENSET_TRCPT0_Msk         (_U_(0x1) << USB_HOST_PINTENSET_TRCPT0_Pos)          /**< (USB_HOST_PINTENSET) Transfer Complete 0 Interrupt Enable Mask */
#define USB_HOST_PINTENSET_TRCPT0(value)      (USB_HOST_PINTENSET_TRCPT0_Msk & ((value) << USB_HOST_PINTENSET_TRCPT0_Pos))
#define USB_HOST_PINTENSET_TRCPT1_Pos         _U_(1)                                               /**< (USB_HOST_PINTENSET) Transfer Complete 1 Interrupt Enable Position */
#define USB_HOST_PINTENSET_TRCPT1_Msk         (_U_(0x1) << USB_HOST_PINTENSET_TRCPT1_Pos)          /**< (USB_HOST_PINTENSET) Transfer Complete 1 Interrupt Enable Mask */
#define USB_HOST_PINTENSET_TRCPT1(value)      (USB_HOST_PINTENSET_TRCPT1_Msk & ((value) << USB_HOST_PINTENSET_TRCPT1_Pos))
#define USB_HOST_PINTENSET_TRFAIL_Pos         _U_(2)                                               /**< (USB_HOST_PINTENSET) Error Flow Interrupt Enable Position */
#define USB_HOST_PINTENSET_TRFAIL_Msk         (_U_(0x1) << USB_HOST_PINTENSET_TRFAIL_Pos)          /**< (USB_HOST_PINTENSET) Error Flow Interrupt Enable Mask */
#define USB_HOST_PINTENSET_TRFAIL(value)      (USB_HOST_PINTENSET_TRFAIL_Msk & ((value) << USB_HOST_PINTENSET_TRFAIL_Pos))
#define USB_HOST_PINTENSET_PERR_Pos           _U_(3)                                               /**< (USB_HOST_PINTENSET) Pipe Error Interrupt Enable Position */
#define USB_HOST_PINTENSET_PERR_Msk           (_U_(0x1) << USB_HOST_PINTENSET_PERR_Pos)            /**< (USB_HOST_PINTENSET) Pipe Error Interrupt Enable Mask */
#define USB_HOST_PINTENSET_PERR(value)        (USB_HOST_PINTENSET_PERR_Msk & ((value) << USB_HOST_PINTENSET_PERR_Pos))
#define USB_HOST_PINTENSET_TXSTP_Pos          _U_(4)                                               /**< (USB_HOST_PINTENSET) Transmit  Setup Interrupt Enable Position */
#define USB_HOST_PINTENSET_TXSTP_Msk          (_U_(0x1) << USB_HOST_PINTENSET_TXSTP_Pos)           /**< (USB_HOST_PINTENSET) Transmit  Setup Interrupt Enable Mask */
#define USB_HOST_PINTENSET_TXSTP(value)       (USB_HOST_PINTENSET_TXSTP_Msk & ((value) << USB_HOST_PINTENSET_TXSTP_Pos))
#define USB_HOST_PINTENSET_STALL_Pos          _U_(5)                                               /**< (USB_HOST_PINTENSET) Stall Interrupt Enable Position */
#define USB_HOST_PINTENSET_STALL_Msk          (_U_(0x1) << USB_HOST_PINTENSET_STALL_Pos)           /**< (USB_HOST_PINTENSET) Stall Interrupt Enable Mask */
#define USB_HOST_PINTENSET_STALL(value)       (USB_HOST_PINTENSET_STALL_Msk & ((value) << USB_HOST_PINTENSET_STALL_Pos))
#define USB_HOST_PINTENSET_Msk                _U_(0x3F)                                            /**< (USB_HOST_PINTENSET) Register Mask  */

#define USB_HOST_PINTENSET_TRCPT_Pos          _U_(0)                                               /**< (USB_HOST_PINTENSET Position) Transfer Complete x Interrupt Enable */
#define USB_HOST_PINTENSET_TRCPT_Msk          (_U_(0x3) << USB_HOST_PINTENSET_TRCPT_Pos)           /**< (USB_HOST_PINTENSET Mask) TRCPT */
#define USB_HOST_PINTENSET_TRCPT(value)       (USB_HOST_PINTENSET_TRCPT_Msk & ((value) << USB_HOST_PINTENSET_TRCPT_Pos)) 

/* -------- USB_CTRLA : (USB Offset: 0x00) (R/W 8) Control A -------- */
#define USB_CTRLA_RESETVALUE                  _U_(0x00)                                            /**<  (USB_CTRLA) Control A  Reset Value */

#define USB_CTRLA_SWRST_Pos                   _U_(0)                                               /**< (USB_CTRLA) Software Reset Position */
#define USB_CTRLA_SWRST_Msk                   (_U_(0x1) << USB_CTRLA_SWRST_Pos)                    /**< (USB_CTRLA) Software Reset Mask */
#define USB_CTRLA_SWRST(value)                (USB_CTRLA_SWRST_Msk & ((value) << USB_CTRLA_SWRST_Pos))
#define USB_CTRLA_ENABLE_Pos                  _U_(1)                                               /**< (USB_CTRLA) Enable Position */
#define USB_CTRLA_ENABLE_Msk                  (_U_(0x1) << USB_CTRLA_ENABLE_Pos)                   /**< (USB_CTRLA) Enable Mask */
#define USB_CTRLA_ENABLE(value)               (USB_CTRLA_ENABLE_Msk & ((value) << USB_CTRLA_ENABLE_Pos))
#define USB_CTRLA_RUNSTDBY_Pos                _U_(2)                                               /**< (USB_CTRLA) Run in Standby Mode Position */
#define USB_CTRLA_RUNSTDBY_Msk                (_U_(0x1) << USB_CTRLA_RUNSTDBY_Pos)                 /**< (USB_CTRLA) Run in Standby Mode Mask */
#define USB_CTRLA_RUNSTDBY(value)             (USB_CTRLA_RUNSTDBY_Msk & ((value) << USB_CTRLA_RUNSTDBY_Pos))
#define USB_CTRLA_MODE_Pos                    _U_(7)                                               /**< (USB_CTRLA) Operating Mode Position */
#define USB_CTRLA_MODE_Msk                    (_U_(0x1) << USB_CTRLA_MODE_Pos)                     /**< (USB_CTRLA) Operating Mode Mask */
#define USB_CTRLA_MODE(value)                 (USB_CTRLA_MODE_Msk & ((value) << USB_CTRLA_MODE_Pos))
#define   USB_CTRLA_MODE_DEVICE_Val           _U_(0x0)                                             /**< (USB_CTRLA) Device Mode  */
#define   USB_CTRLA_MODE_HOST_Val             _U_(0x1)                                             /**< (USB_CTRLA) Host Mode  */
#define USB_CTRLA_MODE_DEVICE                 (USB_CTRLA_MODE_DEVICE_Val << USB_CTRLA_MODE_Pos)    /**< (USB_CTRLA) Device Mode Position  */
#define USB_CTRLA_MODE_HOST                   (USB_CTRLA_MODE_HOST_Val << USB_CTRLA_MODE_Pos)      /**< (USB_CTRLA) Host Mode Position  */
#define USB_CTRLA_Msk                         _U_(0x87)                                            /**< (USB_CTRLA) Register Mask  */


/* -------- USB_SYNCBUSY : (USB Offset: 0x02) ( R/ 8) Synchronization Busy -------- */
#define USB_SYNCBUSY_RESETVALUE               _U_(0x00)                                            /**<  (USB_SYNCBUSY) Synchronization Busy  Reset Value */

#define USB_SYNCBUSY_SWRST_Pos                _U_(0)                                               /**< (USB_SYNCBUSY) Software Reset Synchronization Busy Position */
#define USB_SYNCBUSY_SWRST_Msk                (_U_(0x1) << USB_SYNCBUSY_SWRST_Pos)                 /**< (USB_SYNCBUSY) Software Reset Synchronization Busy Mask */
#define USB_SYNCBUSY_SWRST(value)             (USB_SYNCBUSY_SWRST_Msk & ((value) << USB_SYNCBUSY_SWRST_Pos))
#define USB_SYNCBUSY_ENABLE_Pos               _U_(1)                                               /**< (USB_SYNCBUSY) Enable Synchronization Busy Position */
#define USB_SYNCBUSY_ENABLE_Msk               (_U_(0x1) << USB_SYNCBUSY_ENABLE_Pos)                /**< (USB_SYNCBUSY) Enable Synchronization Busy Mask */
#define USB_SYNCBUSY_ENABLE(value)            (USB_SYNCBUSY_ENABLE_Msk & ((value) << USB_SYNCBUSY_ENABLE_Pos))
#define USB_SYNCBUSY_Msk                      _U_(0x03)                                            /**< (USB_SYNCBUSY) Register Mask  */


/* -------- USB_QOSCTRL : (USB Offset: 0x03) (R/W 8) USB Quality Of Service -------- */
#define USB_QOSCTRL_RESETVALUE                _U_(0x0F)                                            /**<  (USB_QOSCTRL) USB Quality Of Service  Reset Value */

#define USB_QOSCTRL_CQOS_Pos                  _U_(0)                                               /**< (USB_QOSCTRL) Configuration Quality of Service Position */
#define USB_QOSCTRL_CQOS_Msk                  (_U_(0x3) << USB_QOSCTRL_CQOS_Pos)                   /**< (USB_QOSCTRL) Configuration Quality of Service Mask */
#define USB_QOSCTRL_CQOS(value)               (USB_QOSCTRL_CQOS_Msk & ((value) << USB_QOSCTRL_CQOS_Pos))
#define USB_QOSCTRL_DQOS_Pos                  _U_(2)                                               /**< (USB_QOSCTRL) Data Quality of Service Position */
#define USB_QOSCTRL_DQOS_Msk                  (_U_(0x3) << USB_QOSCTRL_DQOS_Pos)                   /**< (USB_QOSCTRL) Data Quality of Service Mask */
#define USB_QOSCTRL_DQOS(value)               (USB_QOSCTRL_DQOS_Msk & ((value) << USB_QOSCTRL_DQOS_Pos))
#define USB_QOSCTRL_Msk                       _U_(0x0F)                                            /**< (USB_QOSCTRL) Register Mask  */


/* -------- USB_DEVICE_CTRLB : (USB Offset: 0x08) (R/W 16) DEVICE Control B -------- */
#define USB_DEVICE_CTRLB_RESETVALUE           _U_(0x01)                                            /**<  (USB_DEVICE_CTRLB) DEVICE Control B  Reset Value */

#define USB_DEVICE_CTRLB_DETACH_Pos           _U_(0)                                               /**< (USB_DEVICE_CTRLB) Detach Position */
#define USB_DEVICE_CTRLB_DETACH_Msk           (_U_(0x1) << USB_DEVICE_CTRLB_DETACH_Pos)            /**< (USB_DEVICE_CTRLB) Detach Mask */
#define USB_DEVICE_CTRLB_DETACH(value)        (USB_DEVICE_CTRLB_DETACH_Msk & ((value) << USB_DEVICE_CTRLB_DETACH_Pos))
#define USB_DEVICE_CTRLB_UPRSM_Pos            _U_(1)                                               /**< (USB_DEVICE_CTRLB) Upstream Resume Position */
#define USB_DEVICE_CTRLB_UPRSM_Msk            (_U_(0x1) << USB_DEVICE_CTRLB_UPRSM_Pos)             /**< (USB_DEVICE_CTRLB) Upstream Resume Mask */
#define USB_DEVICE_CTRLB_UPRSM(value)         (USB_DEVICE_CTRLB_UPRSM_Msk & ((value) << USB_DEVICE_CTRLB_UPRSM_Pos))
#define USB_DEVICE_CTRLB_SPDCONF_Pos          _U_(2)                                               /**< (USB_DEVICE_CTRLB) Speed Configuration Position */
#define USB_DEVICE_CTRLB_SPDCONF_Msk          (_U_(0x3) << USB_DEVICE_CTRLB_SPDCONF_Pos)           /**< (USB_DEVICE_CTRLB) Speed Configuration Mask */
#define USB_DEVICE_CTRLB_SPDCONF(value)       (USB_DEVICE_CTRLB_SPDCONF_Msk & ((value) << USB_DEVICE_CTRLB_SPDCONF_Pos))
#define   USB_DEVICE_CTRLB_SPDCONF_FS_Val     _U_(0x0)                                             /**< (USB_DEVICE_CTRLB) FS : Full Speed  */
#define   USB_DEVICE_CTRLB_SPDCONF_LS_Val     _U_(0x1)                                             /**< (USB_DEVICE_CTRLB) LS : Low Speed  */
#define   USB_DEVICE_CTRLB_SPDCONF_HS_Val     _U_(0x2)                                             /**< (USB_DEVICE_CTRLB) HS : High Speed capable  */
#define   USB_DEVICE_CTRLB_SPDCONF_HSTM_Val   _U_(0x3)                                             /**< (USB_DEVICE_CTRLB) HSTM: High Speed Test Mode (force high-speed mode for test mode)  */
#define USB_DEVICE_CTRLB_SPDCONF_FS           (USB_DEVICE_CTRLB_SPDCONF_FS_Val << USB_DEVICE_CTRLB_SPDCONF_Pos) /**< (USB_DEVICE_CTRLB) FS : Full Speed Position  */
#define USB_DEVICE_CTRLB_SPDCONF_LS           (USB_DEVICE_CTRLB_SPDCONF_LS_Val << USB_DEVICE_CTRLB_SPDCONF_Pos) /**< (USB_DEVICE_CTRLB) LS : Low Speed Position  */
#define USB_DEVICE_CTRLB_SPDCONF_HS           (USB_DEVICE_CTRLB_SPDCONF_HS_Val << USB_DEVICE_CTRLB_SPDCONF_Pos) /**< (USB_DEVICE_CTRLB) HS : High Speed capable Position  */
#define USB_DEVICE_CTRLB_SPDCONF_HSTM         (USB_DEVICE_CTRLB_SPDCONF_HSTM_Val << USB_DEVICE_CTRLB_SPDCONF_Pos) /**< (USB_DEVICE_CTRLB) HSTM: High Speed Test Mode (force high-speed mode for test mode) Position  */
#define USB_DEVICE_CTRLB_NREPLY_Pos           _U_(4)                                               /**< (USB_DEVICE_CTRLB) No Reply Position */
#define USB_DEVICE_CTRLB_NREPLY_Msk           (_U_(0x1) << USB_DEVICE_CTRLB_NREPLY_Pos)            /**< (USB_DEVICE_CTRLB) No Reply Mask */
#define USB_DEVICE_CTRLB_NREPLY(value)        (USB_DEVICE_CTRLB_NREPLY_Msk & ((value) << USB_DEVICE_CTRLB_NREPLY_Pos))
#define USB_DEVICE_CTRLB_TSTJ_Pos             _U_(5)                                               /**< (USB_DEVICE_CTRLB) Test mode J Position */
#define USB_DEVICE_CTRLB_TSTJ_Msk             (_U_(0x1) << USB_DEVICE_CTRLB_TSTJ_Pos)              /**< (USB_DEVICE_CTRLB) Test mode J Mask */
#define USB_DEVICE_CTRLB_TSTJ(value)          (USB_DEVICE_CTRLB_TSTJ_Msk & ((value) << USB_DEVICE_CTRLB_TSTJ_Pos))
#define USB_DEVICE_CTRLB_TSTK_Pos             _U_(6)                                               /**< (USB_DEVICE_CTRLB) Test mode K Position */
#define USB_DEVICE_CTRLB_TSTK_Msk             (_U_(0x1) << USB_DEVICE_CTRLB_TSTK_Pos)              /**< (USB_DEVICE_CTRLB) Test mode K Mask */
#define USB_DEVICE_CTRLB_TSTK(value)          (USB_DEVICE_CTRLB_TSTK_Msk & ((value) << USB_DEVICE_CTRLB_TSTK_Pos))
#define USB_DEVICE_CTRLB_TSTPCKT_Pos          _U_(7)                                               /**< (USB_DEVICE_CTRLB) Test packet mode Position */
#define USB_DEVICE_CTRLB_TSTPCKT_Msk          (_U_(0x1) << USB_DEVICE_CTRLB_TSTPCKT_Pos)           /**< (USB_DEVICE_CTRLB) Test packet mode Mask */
#define USB_DEVICE_CTRLB_TSTPCKT(value)       (USB_DEVICE_CTRLB_TSTPCKT_Msk & ((value) << USB_DEVICE_CTRLB_TSTPCKT_Pos))
#define USB_DEVICE_CTRLB_OPMODE2_Pos          _U_(8)                                               /**< (USB_DEVICE_CTRLB) Specific Operational Mode Position */
#define USB_DEVICE_CTRLB_OPMODE2_Msk          (_U_(0x1) << USB_DEVICE_CTRLB_OPMODE2_Pos)           /**< (USB_DEVICE_CTRLB) Specific Operational Mode Mask */
#define USB_DEVICE_CTRLB_OPMODE2(value)       (USB_DEVICE_CTRLB_OPMODE2_Msk & ((value) << USB_DEVICE_CTRLB_OPMODE2_Pos))
#define USB_DEVICE_CTRLB_GNAK_Pos             _U_(9)                                               /**< (USB_DEVICE_CTRLB) Global NAK Position */
#define USB_DEVICE_CTRLB_GNAK_Msk             (_U_(0x1) << USB_DEVICE_CTRLB_GNAK_Pos)              /**< (USB_DEVICE_CTRLB) Global NAK Mask */
#define USB_DEVICE_CTRLB_GNAK(value)          (USB_DEVICE_CTRLB_GNAK_Msk & ((value) << USB_DEVICE_CTRLB_GNAK_Pos))
#define USB_DEVICE_CTRLB_LPMHDSK_Pos          _U_(10)                                              /**< (USB_DEVICE_CTRLB) Link Power Management Handshake Position */
#define USB_DEVICE_CTRLB_LPMHDSK_Msk          (_U_(0x3) << USB_DEVICE_CTRLB_LPMHDSK_Pos)           /**< (USB_DEVICE_CTRLB) Link Power Management Handshake Mask */
#define USB_DEVICE_CTRLB_LPMHDSK(value)       (USB_DEVICE_CTRLB_LPMHDSK_Msk & ((value) << USB_DEVICE_CTRLB_LPMHDSK_Pos))
#define   USB_DEVICE_CTRLB_LPMHDSK_NO_Val     _U_(0x0)                                             /**< (USB_DEVICE_CTRLB) No handshake. LPM is not supported  */
#define   USB_DEVICE_CTRLB_LPMHDSK_ACK_Val    _U_(0x1)                                             /**< (USB_DEVICE_CTRLB) ACK  */
#define   USB_DEVICE_CTRLB_LPMHDSK_NYET_Val   _U_(0x2)                                             /**< (USB_DEVICE_CTRLB) NYET  */
#define   USB_DEVICE_CTRLB_LPMHDSK_STALL_Val  _U_(0x3)                                             /**< (USB_DEVICE_CTRLB) STALL  */
#define USB_DEVICE_CTRLB_LPMHDSK_NO           (USB_DEVICE_CTRLB_LPMHDSK_NO_Val << USB_DEVICE_CTRLB_LPMHDSK_Pos) /**< (USB_DEVICE_CTRLB) No handshake. LPM is not supported Position  */
#define USB_DEVICE_CTRLB_LPMHDSK_ACK          (USB_DEVICE_CTRLB_LPMHDSK_ACK_Val << USB_DEVICE_CTRLB_LPMHDSK_Pos) /**< (USB_DEVICE_CTRLB) ACK Position  */
#define USB_DEVICE_CTRLB_LPMHDSK_NYET         (USB_DEVICE_CTRLB_LPMHDSK_NYET_Val << USB_DEVICE_CTRLB_LPMHDSK_Pos) /**< (USB_DEVICE_CTRLB) NYET Position  */
#define USB_DEVICE_CTRLB_LPMHDSK_STALL        (USB_DEVICE_CTRLB_LPMHDSK_STALL_Val << USB_DEVICE_CTRLB_LPMHDSK_Pos) /**< (USB_DEVICE_CTRLB) STALL Position  */
#define USB_DEVICE_CTRLB_Msk                  _U_(0x0FFF)                                          /**< (USB_DEVICE_CTRLB) Register Mask  */

#define USB_DEVICE_CTRLB_OPMODE_Pos           _U_(8)                                               /**< (USB_DEVICE_CTRLB Position) Specific Operational Mode */
#define USB_DEVICE_CTRLB_OPMODE_Msk           (_U_(0x1) << USB_DEVICE_CTRLB_OPMODE_Pos)            /**< (USB_DEVICE_CTRLB Mask) OPMODE */
#define USB_DEVICE_CTRLB_OPMODE(value)        (USB_DEVICE_CTRLB_OPMODE_Msk & ((value) << USB_DEVICE_CTRLB_OPMODE_Pos)) 

/* -------- USB_HOST_CTRLB : (USB Offset: 0x08) (R/W 16) HOST Control B -------- */
#define USB_HOST_CTRLB_RESETVALUE             _U_(0x00)                                            /**<  (USB_HOST_CTRLB) HOST Control B  Reset Value */

#define USB_HOST_CTRLB_RESUME_Pos             _U_(1)                                               /**< (USB_HOST_CTRLB) Send USB Resume Position */
#define USB_HOST_CTRLB_RESUME_Msk             (_U_(0x1) << USB_HOST_CTRLB_RESUME_Pos)              /**< (USB_HOST_CTRLB) Send USB Resume Mask */
#define USB_HOST_CTRLB_RESUME(value)          (USB_HOST_CTRLB_RESUME_Msk & ((value) << USB_HOST_CTRLB_RESUME_Pos))
#define USB_HOST_CTRLB_SPDCONF_Pos            _U_(2)                                               /**< (USB_HOST_CTRLB) Speed Configuration for Host Position */
#define USB_HOST_CTRLB_SPDCONF_Msk            (_U_(0x3) << USB_HOST_CTRLB_SPDCONF_Pos)             /**< (USB_HOST_CTRLB) Speed Configuration for Host Mask */
#define USB_HOST_CTRLB_SPDCONF(value)         (USB_HOST_CTRLB_SPDCONF_Msk & ((value) << USB_HOST_CTRLB_SPDCONF_Pos))
#define   USB_HOST_CTRLB_SPDCONF_NORMAL_Val   _U_(0x0)                                             /**< (USB_HOST_CTRLB) Normal mode: the host starts in full-speed mode and performs a high-speed reset to switch to the high speed mode if the downstream peripheral is high-speed capable.  */
#define   USB_HOST_CTRLB_SPDCONF_FS_Val       _U_(0x3)                                             /**< (USB_HOST_CTRLB) Full-speed: the host remains in full-speed mode whatever is the peripheral speed capability. Relevant in UTMI mode only.  */
#define USB_HOST_CTRLB_SPDCONF_NORMAL         (USB_HOST_CTRLB_SPDCONF_NORMAL_Val << USB_HOST_CTRLB_SPDCONF_Pos) /**< (USB_HOST_CTRLB) Normal mode: the host starts in full-speed mode and performs a high-speed reset to switch to the high speed mode if the downstream peripheral is high-speed capable. Position  */
#define USB_HOST_CTRLB_SPDCONF_FS             (USB_HOST_CTRLB_SPDCONF_FS_Val << USB_HOST_CTRLB_SPDCONF_Pos) /**< (USB_HOST_CTRLB) Full-speed: the host remains in full-speed mode whatever is the peripheral speed capability. Relevant in UTMI mode only. Position  */
#define USB_HOST_CTRLB_AUTORESUME_Pos         _U_(4)                                               /**< (USB_HOST_CTRLB) Auto Resume Enable Position */
#define USB_HOST_CTRLB_AUTORESUME_Msk         (_U_(0x1) << USB_HOST_CTRLB_AUTORESUME_Pos)          /**< (USB_HOST_CTRLB) Auto Resume Enable Mask */
#define USB_HOST_CTRLB_AUTORESUME(value)      (USB_HOST_CTRLB_AUTORESUME_Msk & ((value) << USB_HOST_CTRLB_AUTORESUME_Pos))
#define USB_HOST_CTRLB_TSTJ_Pos               _U_(5)                                               /**< (USB_HOST_CTRLB) Test mode J Position */
#define USB_HOST_CTRLB_TSTJ_Msk               (_U_(0x1) << USB_HOST_CTRLB_TSTJ_Pos)                /**< (USB_HOST_CTRLB) Test mode J Mask */
#define USB_HOST_CTRLB_TSTJ(value)            (USB_HOST_CTRLB_TSTJ_Msk & ((value) << USB_HOST_CTRLB_TSTJ_Pos))
#define USB_HOST_CTRLB_TSTK_Pos               _U_(6)                                               /**< (USB_HOST_CTRLB) Test mode K Position */
#define USB_HOST_CTRLB_TSTK_Msk               (_U_(0x1) << USB_HOST_CTRLB_TSTK_Pos)                /**< (USB_HOST_CTRLB) Test mode K Mask */
#define USB_HOST_CTRLB_TSTK(value)            (USB_HOST_CTRLB_TSTK_Msk & ((value) << USB_HOST_CTRLB_TSTK_Pos))
#define USB_HOST_CTRLB_SOFE_Pos               _U_(8)                                               /**< (USB_HOST_CTRLB) Start of Frame Generation Enable Position */
#define USB_HOST_CTRLB_SOFE_Msk               (_U_(0x1) << USB_HOST_CTRLB_SOFE_Pos)                /**< (USB_HOST_CTRLB) Start of Frame Generation Enable Mask */
#define USB_HOST_CTRLB_SOFE(value)            (USB_HOST_CTRLB_SOFE_Msk & ((value) << USB_HOST_CTRLB_SOFE_Pos))
#define USB_HOST_CTRLB_BUSRESET_Pos           _U_(9)                                               /**< (USB_HOST_CTRLB) Send USB Reset Position */
#define USB_HOST_CTRLB_BUSRESET_Msk           (_U_(0x1) << USB_HOST_CTRLB_BUSRESET_Pos)            /**< (USB_HOST_CTRLB) Send USB Reset Mask */
#define USB_HOST_CTRLB_BUSRESET(value)        (USB_HOST_CTRLB_BUSRESET_Msk & ((value) << USB_HOST_CTRLB_BUSRESET_Pos))
#define USB_HOST_CTRLB_VBUSOK_Pos             _U_(10)                                              /**< (USB_HOST_CTRLB) VBUS is OK Position */
#define USB_HOST_CTRLB_VBUSOK_Msk             (_U_(0x1) << USB_HOST_CTRLB_VBUSOK_Pos)              /**< (USB_HOST_CTRLB) VBUS is OK Mask */
#define USB_HOST_CTRLB_VBUSOK(value)          (USB_HOST_CTRLB_VBUSOK_Msk & ((value) << USB_HOST_CTRLB_VBUSOK_Pos))
#define USB_HOST_CTRLB_L1RESUME_Pos           _U_(11)                                              /**< (USB_HOST_CTRLB) Send L1 Resume Position */
#define USB_HOST_CTRLB_L1RESUME_Msk           (_U_(0x1) << USB_HOST_CTRLB_L1RESUME_Pos)            /**< (USB_HOST_CTRLB) Send L1 Resume Mask */
#define USB_HOST_CTRLB_L1RESUME(value)        (USB_HOST_CTRLB_L1RESUME_Msk & ((value) << USB_HOST_CTRLB_L1RESUME_Pos))
#define USB_HOST_CTRLB_Msk                    _U_(0x0F7E)                                          /**< (USB_HOST_CTRLB) Register Mask  */


/* -------- USB_DEVICE_DADD : (USB Offset: 0x0A) (R/W 8) DEVICE Device Address -------- */
#define USB_DEVICE_DADD_RESETVALUE            _U_(0x00)                                            /**<  (USB_DEVICE_DADD) DEVICE Device Address  Reset Value */

#define USB_DEVICE_DADD_DADD_Pos              _U_(0)                                               /**< (USB_DEVICE_DADD) Device Address Position */
#define USB_DEVICE_DADD_DADD_Msk              (_U_(0x7F) << USB_DEVICE_DADD_DADD_Pos)              /**< (USB_DEVICE_DADD) Device Address Mask */
#define USB_DEVICE_DADD_DADD(value)           (USB_DEVICE_DADD_DADD_Msk & ((value) << USB_DEVICE_DADD_DADD_Pos))
#define USB_DEVICE_DADD_ADDEN_Pos             _U_(7)                                               /**< (USB_DEVICE_DADD) Device Address Enable Position */
#define USB_DEVICE_DADD_ADDEN_Msk             (_U_(0x1) << USB_DEVICE_DADD_ADDEN_Pos)              /**< (USB_DEVICE_DADD) Device Address Enable Mask */
#define USB_DEVICE_DADD_ADDEN(value)          (USB_DEVICE_DADD_ADDEN_Msk & ((value) << USB_DEVICE_DADD_ADDEN_Pos))
#define USB_DEVICE_DADD_Msk                   _U_(0xFF)                                            /**< (USB_DEVICE_DADD) Register Mask  */


/* -------- USB_HOST_HSOFC : (USB Offset: 0x0A) (R/W 8) HOST Host Start Of Frame Control -------- */
#define USB_HOST_HSOFC_RESETVALUE             _U_(0x00)                                            /**<  (USB_HOST_HSOFC) HOST Host Start Of Frame Control  Reset Value */

#define USB_HOST_HSOFC_FLENC_Pos              _U_(0)                                               /**< (USB_HOST_HSOFC) Frame Length Control Position */
#define USB_HOST_HSOFC_FLENC_Msk              (_U_(0xF) << USB_HOST_HSOFC_FLENC_Pos)               /**< (USB_HOST_HSOFC) Frame Length Control Mask */
#define USB_HOST_HSOFC_FLENC(value)           (USB_HOST_HSOFC_FLENC_Msk & ((value) << USB_HOST_HSOFC_FLENC_Pos))
#define USB_HOST_HSOFC_FLENCE_Pos             _U_(7)                                               /**< (USB_HOST_HSOFC) Frame Length Control Enable Position */
#define USB_HOST_HSOFC_FLENCE_Msk             (_U_(0x1) << USB_HOST_HSOFC_FLENCE_Pos)              /**< (USB_HOST_HSOFC) Frame Length Control Enable Mask */
#define USB_HOST_HSOFC_FLENCE(value)          (USB_HOST_HSOFC_FLENCE_Msk & ((value) << USB_HOST_HSOFC_FLENCE_Pos))
#define USB_HOST_HSOFC_Msk                    _U_(0x8F)                                            /**< (USB_HOST_HSOFC) Register Mask  */


/* -------- USB_DEVICE_STATUS : (USB Offset: 0x0C) ( R/ 8) DEVICE Status -------- */
#define USB_DEVICE_STATUS_RESETVALUE          _U_(0x40)                                            /**<  (USB_DEVICE_STATUS) DEVICE Status  Reset Value */

#define USB_DEVICE_STATUS_SPEED_Pos           _U_(2)                                               /**< (USB_DEVICE_STATUS) Speed Status Position */
#define USB_DEVICE_STATUS_SPEED_Msk           (_U_(0x3) << USB_DEVICE_STATUS_SPEED_Pos)            /**< (USB_DEVICE_STATUS) Speed Status Mask */
#define USB_DEVICE_STATUS_SPEED(value)        (USB_DEVICE_STATUS_SPEED_Msk & ((value) << USB_DEVICE_STATUS_SPEED_Pos))
#define   USB_DEVICE_STATUS_SPEED_FS_Val      _U_(0x0)                                             /**< (USB_DEVICE_STATUS) Full-speed mode  */
#define   USB_DEVICE_STATUS_SPEED_LS_Val      _U_(0x1)                                             /**< (USB_DEVICE_STATUS) Low-speed mode  */
#define   USB_DEVICE_STATUS_SPEED_HS_Val      _U_(0x2)                                             /**< (USB_DEVICE_STATUS) High-speed mode  */
#define USB_DEVICE_STATUS_SPEED_FS            (USB_DEVICE_STATUS_SPEED_FS_Val << USB_DEVICE_STATUS_SPEED_Pos) /**< (USB_DEVICE_STATUS) Full-speed mode Position  */
#define USB_DEVICE_STATUS_SPEED_LS            (USB_DEVICE_STATUS_SPEED_LS_Val << USB_DEVICE_STATUS_SPEED_Pos) /**< (USB_DEVICE_STATUS) Low-speed mode Position  */
#define USB_DEVICE_STATUS_SPEED_HS            (USB_DEVICE_STATUS_SPEED_HS_Val << USB_DEVICE_STATUS_SPEED_Pos) /**< (USB_DEVICE_STATUS) High-speed mode Position  */
#define USB_DEVICE_STATUS_LINESTATE_Pos       _U_(6)                                               /**< (USB_DEVICE_STATUS) USB Line State Status Position */
#define USB_DEVICE_STATUS_LINESTATE_Msk       (_U_(0x3) << USB_DEVICE_STATUS_LINESTATE_Pos)        /**< (USB_DEVICE_STATUS) USB Line State Status Mask */
#define USB_DEVICE_STATUS_LINESTATE(value)    (USB_DEVICE_STATUS_LINESTATE_Msk & ((value) << USB_DEVICE_STATUS_LINESTATE_Pos))
#define   USB_DEVICE_STATUS_LINESTATE_0_Val   _U_(0x0)                                             /**< (USB_DEVICE_STATUS) SE0/RESET  */
#define   USB_DEVICE_STATUS_LINESTATE_1_Val   _U_(0x1)                                             /**< (USB_DEVICE_STATUS) FS-J or LS-K State  */
#define   USB_DEVICE_STATUS_LINESTATE_2_Val   _U_(0x2)                                             /**< (USB_DEVICE_STATUS) FS-K or LS-J State  */
#define USB_DEVICE_STATUS_LINESTATE_0         (USB_DEVICE_STATUS_LINESTATE_0_Val << USB_DEVICE_STATUS_LINESTATE_Pos) /**< (USB_DEVICE_STATUS) SE0/RESET Position  */
#define USB_DEVICE_STATUS_LINESTATE_1         (USB_DEVICE_STATUS_LINESTATE_1_Val << USB_DEVICE_STATUS_LINESTATE_Pos) /**< (USB_DEVICE_STATUS) FS-J or LS-K State Position  */
#define USB_DEVICE_STATUS_LINESTATE_2         (USB_DEVICE_STATUS_LINESTATE_2_Val << USB_DEVICE_STATUS_LINESTATE_Pos) /**< (USB_DEVICE_STATUS) FS-K or LS-J State Position  */
#define USB_DEVICE_STATUS_Msk                 _U_(0xCC)                                            /**< (USB_DEVICE_STATUS) Register Mask  */


/* -------- USB_HOST_STATUS : (USB Offset: 0x0C) (R/W 8) HOST Status -------- */
#define USB_HOST_STATUS_RESETVALUE            _U_(0x00)                                            /**<  (USB_HOST_STATUS) HOST Status  Reset Value */

#define USB_HOST_STATUS_SPEED_Pos             _U_(2)                                               /**< (USB_HOST_STATUS) Speed Status Position */
#define USB_HOST_STATUS_SPEED_Msk             (_U_(0x3) << USB_HOST_STATUS_SPEED_Pos)              /**< (USB_HOST_STATUS) Speed Status Mask */
#define USB_HOST_STATUS_SPEED(value)          (USB_HOST_STATUS_SPEED_Msk & ((value) << USB_HOST_STATUS_SPEED_Pos))
#define USB_HOST_STATUS_LINESTATE_Pos         _U_(6)                                               /**< (USB_HOST_STATUS) USB Line State Status Position */
#define USB_HOST_STATUS_LINESTATE_Msk         (_U_(0x3) << USB_HOST_STATUS_LINESTATE_Pos)          /**< (USB_HOST_STATUS) USB Line State Status Mask */
#define USB_HOST_STATUS_LINESTATE(value)      (USB_HOST_STATUS_LINESTATE_Msk & ((value) << USB_HOST_STATUS_LINESTATE_Pos))
#define USB_HOST_STATUS_Msk                   _U_(0xCC)                                            /**< (USB_HOST_STATUS) Register Mask  */


/* -------- USB_FSMSTATUS : (USB Offset: 0x0D) ( R/ 8) Finite State Machine Status -------- */
#define USB_FSMSTATUS_RESETVALUE              _U_(0x01)                                            /**<  (USB_FSMSTATUS) Finite State Machine Status  Reset Value */

#define USB_FSMSTATUS_FSMSTATE_Pos            _U_(0)                                               /**< (USB_FSMSTATUS) Fine State Machine Status Position */
#define USB_FSMSTATUS_FSMSTATE_Msk            (_U_(0x7F) << USB_FSMSTATUS_FSMSTATE_Pos)            /**< (USB_FSMSTATUS) Fine State Machine Status Mask */
#define USB_FSMSTATUS_FSMSTATE(value)         (USB_FSMSTATUS_FSMSTATE_Msk & ((value) << USB_FSMSTATUS_FSMSTATE_Pos))
#define   USB_FSMSTATUS_FSMSTATE_OFF_Val      _U_(0x1)                                             /**< (USB_FSMSTATUS) OFF (L3). It corresponds to the powered-off, disconnected, and disabled state  */
#define   USB_FSMSTATUS_FSMSTATE_ON_Val       _U_(0x2)                                             /**< (USB_FSMSTATUS) ON (L0). It corresponds to the Idle and Active states  */
#define   USB_FSMSTATUS_FSMSTATE_SUSPEND_Val  _U_(0x4)                                             /**< (USB_FSMSTATUS) SUSPEND (L2)  */
#define   USB_FSMSTATUS_FSMSTATE_SLEEP_Val    _U_(0x8)                                             /**< (USB_FSMSTATUS) SLEEP (L1)  */
#define   USB_FSMSTATUS_FSMSTATE_DNRESUME_Val _U_(0x10)                                            /**< (USB_FSMSTATUS) DNRESUME. Down Stream Resume.  */
#define   USB_FSMSTATUS_FSMSTATE_UPRESUME_Val _U_(0x20)                                            /**< (USB_FSMSTATUS) UPRESUME. Up Stream Resume.  */
#define   USB_FSMSTATUS_FSMSTATE_RESET_Val    _U_(0x40)                                            /**< (USB_FSMSTATUS) RESET. USB lines Reset.  */
#define USB_FSMSTATUS_FSMSTATE_OFF            (USB_FSMSTATUS_FSMSTATE_OFF_Val << USB_FSMSTATUS_FSMSTATE_Pos) /**< (USB_FSMSTATUS) OFF (L3). It corresponds to the powered-off, disconnected, and disabled state Position  */
#define USB_FSMSTATUS_FSMSTATE_ON             (USB_FSMSTATUS_FSMSTATE_ON_Val << USB_FSMSTATUS_FSMSTATE_Pos) /**< (USB_FSMSTATUS) ON (L0). It corresponds to the Idle and Active states Position  */
#define USB_FSMSTATUS_FSMSTATE_SUSPEND        (USB_FSMSTATUS_FSMSTATE_SUSPEND_Val << USB_FSMSTATUS_FSMSTATE_Pos) /**< (USB_FSMSTATUS) SUSPEND (L2) Position  */
#define USB_FSMSTATUS_FSMSTATE_SLEEP          (USB_FSMSTATUS_FSMSTATE_SLEEP_Val << USB_FSMSTATUS_FSMSTATE_Pos) /**< (USB_FSMSTATUS) SLEEP (L1) Position  */
#define USB_FSMSTATUS_FSMSTATE_DNRESUME       (USB_FSMSTATUS_FSMSTATE_DNRESUME_Val << USB_FSMSTATUS_FSMSTATE_Pos) /**< (USB_FSMSTATUS) DNRESUME. Down Stream Resume. Position  */
#define USB_FSMSTATUS_FSMSTATE_UPRESUME       (USB_FSMSTATUS_FSMSTATE_UPRESUME_Val << USB_FSMSTATUS_FSMSTATE_Pos) /**< (USB_FSMSTATUS) UPRESUME. Up Stream Resume. Position  */
#define USB_FSMSTATUS_FSMSTATE_RESET          (USB_FSMSTATUS_FSMSTATE_RESET_Val << USB_FSMSTATUS_FSMSTATE_Pos) /**< (USB_FSMSTATUS) RESET. USB lines Reset. Position  */
#define USB_FSMSTATUS_Msk                     _U_(0x7F)                                            /**< (USB_FSMSTATUS) Register Mask  */


/* -------- USB_DEVICE_FNUM : (USB Offset: 0x10) ( R/ 16) DEVICE Device Frame Number -------- */
#define USB_DEVICE_FNUM_RESETVALUE            _U_(0x00)                                            /**<  (USB_DEVICE_FNUM) DEVICE Device Frame Number  Reset Value */

#define USB_DEVICE_FNUM_MFNUM_Pos             _U_(0)                                               /**< (USB_DEVICE_FNUM) Micro Frame Number Position */
#define USB_DEVICE_FNUM_MFNUM_Msk             (_U_(0x7) << USB_DEVICE_FNUM_MFNUM_Pos)              /**< (USB_DEVICE_FNUM) Micro Frame Number Mask */
#define USB_DEVICE_FNUM_MFNUM(value)          (USB_DEVICE_FNUM_MFNUM_Msk & ((value) << USB_DEVICE_FNUM_MFNUM_Pos))
#define USB_DEVICE_FNUM_FNUM_Pos              _U_(3)                                               /**< (USB_DEVICE_FNUM) Frame Number Position */
#define USB_DEVICE_FNUM_FNUM_Msk              (_U_(0x7FF) << USB_DEVICE_FNUM_FNUM_Pos)             /**< (USB_DEVICE_FNUM) Frame Number Mask */
#define USB_DEVICE_FNUM_FNUM(value)           (USB_DEVICE_FNUM_FNUM_Msk & ((value) << USB_DEVICE_FNUM_FNUM_Pos))
#define USB_DEVICE_FNUM_FNCERR_Pos            _U_(15)                                              /**< (USB_DEVICE_FNUM) Frame Number CRC Error Position */
#define USB_DEVICE_FNUM_FNCERR_Msk            (_U_(0x1) << USB_DEVICE_FNUM_FNCERR_Pos)             /**< (USB_DEVICE_FNUM) Frame Number CRC Error Mask */
#define USB_DEVICE_FNUM_FNCERR(value)         (USB_DEVICE_FNUM_FNCERR_Msk & ((value) << USB_DEVICE_FNUM_FNCERR_Pos))
#define USB_DEVICE_FNUM_Msk                   _U_(0xBFFF)                                          /**< (USB_DEVICE_FNUM) Register Mask  */


/* -------- USB_HOST_FNUM : (USB Offset: 0x10) (R/W 16) HOST Host Frame Number -------- */
#define USB_HOST_FNUM_RESETVALUE              _U_(0x00)                                            /**<  (USB_HOST_FNUM) HOST Host Frame Number  Reset Value */

#define USB_HOST_FNUM_MFNUM_Pos               _U_(0)                                               /**< (USB_HOST_FNUM) Micro Frame Number Position */
#define USB_HOST_FNUM_MFNUM_Msk               (_U_(0x7) << USB_HOST_FNUM_MFNUM_Pos)                /**< (USB_HOST_FNUM) Micro Frame Number Mask */
#define USB_HOST_FNUM_MFNUM(value)            (USB_HOST_FNUM_MFNUM_Msk & ((value) << USB_HOST_FNUM_MFNUM_Pos))
#define USB_HOST_FNUM_FNUM_Pos                _U_(3)                                               /**< (USB_HOST_FNUM) Frame Number Position */
#define USB_HOST_FNUM_FNUM_Msk                (_U_(0x7FF) << USB_HOST_FNUM_FNUM_Pos)               /**< (USB_HOST_FNUM) Frame Number Mask */
#define USB_HOST_FNUM_FNUM(value)             (USB_HOST_FNUM_FNUM_Msk & ((value) << USB_HOST_FNUM_FNUM_Pos))
#define USB_HOST_FNUM_Msk                     _U_(0x3FFF)                                          /**< (USB_HOST_FNUM) Register Mask  */


/* -------- USB_HOST_FLENHIGH : (USB Offset: 0x12) ( R/ 8) HOST Host Frame Length -------- */
#define USB_HOST_FLENHIGH_RESETVALUE          _U_(0x00)                                            /**<  (USB_HOST_FLENHIGH) HOST Host Frame Length  Reset Value */

#define USB_HOST_FLENHIGH_FLENHIGH_Pos        _U_(0)                                               /**< (USB_HOST_FLENHIGH) Frame Length Position */
#define USB_HOST_FLENHIGH_FLENHIGH_Msk        (_U_(0xFF) << USB_HOST_FLENHIGH_FLENHIGH_Pos)        /**< (USB_HOST_FLENHIGH) Frame Length Mask */
#define USB_HOST_FLENHIGH_FLENHIGH(value)     (USB_HOST_FLENHIGH_FLENHIGH_Msk & ((value) << USB_HOST_FLENHIGH_FLENHIGH_Pos))
#define USB_HOST_FLENHIGH_Msk                 _U_(0xFF)                                            /**< (USB_HOST_FLENHIGH) Register Mask  */


/* -------- USB_DEVICE_INTENCLR : (USB Offset: 0x14) (R/W 16) DEVICE Device Interrupt Enable Clear -------- */
#define USB_DEVICE_INTENCLR_RESETVALUE        _U_(0x00)                                            /**<  (USB_DEVICE_INTENCLR) DEVICE Device Interrupt Enable Clear  Reset Value */

#define USB_DEVICE_INTENCLR_SUSPEND_Pos       _U_(0)                                               /**< (USB_DEVICE_INTENCLR) Suspend Interrupt Enable Position */
#define USB_DEVICE_INTENCLR_SUSPEND_Msk       (_U_(0x1) << USB_DEVICE_INTENCLR_SUSPEND_Pos)        /**< (USB_DEVICE_INTENCLR) Suspend Interrupt Enable Mask */
#define USB_DEVICE_INTENCLR_SUSPEND(value)    (USB_DEVICE_INTENCLR_SUSPEND_Msk & ((value) << USB_DEVICE_INTENCLR_SUSPEND_Pos))
#define USB_DEVICE_INTENCLR_MSOF_Pos          _U_(1)                                               /**< (USB_DEVICE_INTENCLR) Micro Start of Frame Interrupt Enable in High Speed Mode Position */
#define USB_DEVICE_INTENCLR_MSOF_Msk          (_U_(0x1) << USB_DEVICE_INTENCLR_MSOF_Pos)           /**< (USB_DEVICE_INTENCLR) Micro Start of Frame Interrupt Enable in High Speed Mode Mask */
#define USB_DEVICE_INTENCLR_MSOF(value)       (USB_DEVICE_INTENCLR_MSOF_Msk & ((value) << USB_DEVICE_INTENCLR_MSOF_Pos))
#define USB_DEVICE_INTENCLR_SOF_Pos           _U_(2)                                               /**< (USB_DEVICE_INTENCLR) Start Of Frame Interrupt Enable Position */
#define USB_DEVICE_INTENCLR_SOF_Msk           (_U_(0x1) << USB_DEVICE_INTENCLR_SOF_Pos)            /**< (USB_DEVICE_INTENCLR) Start Of Frame Interrupt Enable Mask */
#define USB_DEVICE_INTENCLR_SOF(value)        (USB_DEVICE_INTENCLR_SOF_Msk & ((value) << USB_DEVICE_INTENCLR_SOF_Pos))
#define USB_DEVICE_INTENCLR_EORST_Pos         _U_(3)                                               /**< (USB_DEVICE_INTENCLR) End of Reset Interrupt Enable Position */
#define USB_DEVICE_INTENCLR_EORST_Msk         (_U_(0x1) << USB_DEVICE_INTENCLR_EORST_Pos)          /**< (USB_DEVICE_INTENCLR) End of Reset Interrupt Enable Mask */
#define USB_DEVICE_INTENCLR_EORST(value)      (USB_DEVICE_INTENCLR_EORST_Msk & ((value) << USB_DEVICE_INTENCLR_EORST_Pos))
#define USB_DEVICE_INTENCLR_WAKEUP_Pos        _U_(4)                                               /**< (USB_DEVICE_INTENCLR) Wake Up Interrupt Enable Position */
#define USB_DEVICE_INTENCLR_WAKEUP_Msk        (_U_(0x1) << USB_DEVICE_INTENCLR_WAKEUP_Pos)         /**< (USB_DEVICE_INTENCLR) Wake Up Interrupt Enable Mask */
#define USB_DEVICE_INTENCLR_WAKEUP(value)     (USB_DEVICE_INTENCLR_WAKEUP_Msk & ((value) << USB_DEVICE_INTENCLR_WAKEUP_Pos))
#define USB_DEVICE_INTENCLR_EORSM_Pos         _U_(5)                                               /**< (USB_DEVICE_INTENCLR) End Of Resume Interrupt Enable Position */
#define USB_DEVICE_INTENCLR_EORSM_Msk         (_U_(0x1) << USB_DEVICE_INTENCLR_EORSM_Pos)          /**< (USB_DEVICE_INTENCLR) End Of Resume Interrupt Enable Mask */
#define USB_DEVICE_INTENCLR_EORSM(value)      (USB_DEVICE_INTENCLR_EORSM_Msk & ((value) << USB_DEVICE_INTENCLR_EORSM_Pos))
#define USB_DEVICE_INTENCLR_UPRSM_Pos         _U_(6)                                               /**< (USB_DEVICE_INTENCLR) Upstream Resume Interrupt Enable Position */
#define USB_DEVICE_INTENCLR_UPRSM_Msk         (_U_(0x1) << USB_DEVICE_INTENCLR_UPRSM_Pos)          /**< (USB_DEVICE_INTENCLR) Upstream Resume Interrupt Enable Mask */
#define USB_DEVICE_INTENCLR_UPRSM(value)      (USB_DEVICE_INTENCLR_UPRSM_Msk & ((value) << USB_DEVICE_INTENCLR_UPRSM_Pos))
#define USB_DEVICE_INTENCLR_RAMACER_Pos       _U_(7)                                               /**< (USB_DEVICE_INTENCLR) Ram Access Interrupt Enable Position */
#define USB_DEVICE_INTENCLR_RAMACER_Msk       (_U_(0x1) << USB_DEVICE_INTENCLR_RAMACER_Pos)        /**< (USB_DEVICE_INTENCLR) Ram Access Interrupt Enable Mask */
#define USB_DEVICE_INTENCLR_RAMACER(value)    (USB_DEVICE_INTENCLR_RAMACER_Msk & ((value) << USB_DEVICE_INTENCLR_RAMACER_Pos))
#define USB_DEVICE_INTENCLR_LPMNYET_Pos       _U_(8)                                               /**< (USB_DEVICE_INTENCLR) Link Power Management Not Yet Interrupt Enable Position */
#define USB_DEVICE_INTENCLR_LPMNYET_Msk       (_U_(0x1) << USB_DEVICE_INTENCLR_LPMNYET_Pos)        /**< (USB_DEVICE_INTENCLR) Link Power Management Not Yet Interrupt Enable Mask */
#define USB_DEVICE_INTENCLR_LPMNYET(value)    (USB_DEVICE_INTENCLR_LPMNYET_Msk & ((value) << USB_DEVICE_INTENCLR_LPMNYET_Pos))
#define USB_DEVICE_INTENCLR_LPMSUSP_Pos       _U_(9)                                               /**< (USB_DEVICE_INTENCLR) Link Power Management Suspend Interrupt Enable Position */
#define USB_DEVICE_INTENCLR_LPMSUSP_Msk       (_U_(0x1) << USB_DEVICE_INTENCLR_LPMSUSP_Pos)        /**< (USB_DEVICE_INTENCLR) Link Power Management Suspend Interrupt Enable Mask */
#define USB_DEVICE_INTENCLR_LPMSUSP(value)    (USB_DEVICE_INTENCLR_LPMSUSP_Msk & ((value) << USB_DEVICE_INTENCLR_LPMSUSP_Pos))
#define USB_DEVICE_INTENCLR_Msk               _U_(0x03FF)                                          /**< (USB_DEVICE_INTENCLR) Register Mask  */


/* -------- USB_HOST_INTENCLR : (USB Offset: 0x14) (R/W 16) HOST Host Interrupt Enable Clear -------- */
#define USB_HOST_INTENCLR_RESETVALUE          _U_(0x00)                                            /**<  (USB_HOST_INTENCLR) HOST Host Interrupt Enable Clear  Reset Value */

#define USB_HOST_INTENCLR_HSOF_Pos            _U_(2)                                               /**< (USB_HOST_INTENCLR) Host Start Of Frame Interrupt Disable Position */
#define USB_HOST_INTENCLR_HSOF_Msk            (_U_(0x1) << USB_HOST_INTENCLR_HSOF_Pos)             /**< (USB_HOST_INTENCLR) Host Start Of Frame Interrupt Disable Mask */
#define USB_HOST_INTENCLR_HSOF(value)         (USB_HOST_INTENCLR_HSOF_Msk & ((value) << USB_HOST_INTENCLR_HSOF_Pos))
#define USB_HOST_INTENCLR_RST_Pos             _U_(3)                                               /**< (USB_HOST_INTENCLR) BUS Reset Interrupt Disable Position */
#define USB_HOST_INTENCLR_RST_Msk             (_U_(0x1) << USB_HOST_INTENCLR_RST_Pos)              /**< (USB_HOST_INTENCLR) BUS Reset Interrupt Disable Mask */
#define USB_HOST_INTENCLR_RST(value)          (USB_HOST_INTENCLR_RST_Msk & ((value) << USB_HOST_INTENCLR_RST_Pos))
#define USB_HOST_INTENCLR_WAKEUP_Pos          _U_(4)                                               /**< (USB_HOST_INTENCLR) Wake Up Interrupt Disable Position */
#define USB_HOST_INTENCLR_WAKEUP_Msk          (_U_(0x1) << USB_HOST_INTENCLR_WAKEUP_Pos)           /**< (USB_HOST_INTENCLR) Wake Up Interrupt Disable Mask */
#define USB_HOST_INTENCLR_WAKEUP(value)       (USB_HOST_INTENCLR_WAKEUP_Msk & ((value) << USB_HOST_INTENCLR_WAKEUP_Pos))
#define USB_HOST_INTENCLR_DNRSM_Pos           _U_(5)                                               /**< (USB_HOST_INTENCLR) DownStream to Device Interrupt Disable Position */
#define USB_HOST_INTENCLR_DNRSM_Msk           (_U_(0x1) << USB_HOST_INTENCLR_DNRSM_Pos)            /**< (USB_HOST_INTENCLR) DownStream to Device Interrupt Disable Mask */
#define USB_HOST_INTENCLR_DNRSM(value)        (USB_HOST_INTENCLR_DNRSM_Msk & ((value) << USB_HOST_INTENCLR_DNRSM_Pos))
#define USB_HOST_INTENCLR_UPRSM_Pos           _U_(6)                                               /**< (USB_HOST_INTENCLR) Upstream Resume from Device Interrupt Disable Position */
#define USB_HOST_INTENCLR_UPRSM_Msk           (_U_(0x1) << USB_HOST_INTENCLR_UPRSM_Pos)            /**< (USB_HOST_INTENCLR) Upstream Resume from Device Interrupt Disable Mask */
#define USB_HOST_INTENCLR_UPRSM(value)        (USB_HOST_INTENCLR_UPRSM_Msk & ((value) << USB_HOST_INTENCLR_UPRSM_Pos))
#define USB_HOST_INTENCLR_RAMACER_Pos         _U_(7)                                               /**< (USB_HOST_INTENCLR) Ram Access Interrupt Disable Position */
#define USB_HOST_INTENCLR_RAMACER_Msk         (_U_(0x1) << USB_HOST_INTENCLR_RAMACER_Pos)          /**< (USB_HOST_INTENCLR) Ram Access Interrupt Disable Mask */
#define USB_HOST_INTENCLR_RAMACER(value)      (USB_HOST_INTENCLR_RAMACER_Msk & ((value) << USB_HOST_INTENCLR_RAMACER_Pos))
#define USB_HOST_INTENCLR_DCONN_Pos           _U_(8)                                               /**< (USB_HOST_INTENCLR) Device Connection Interrupt Disable Position */
#define USB_HOST_INTENCLR_DCONN_Msk           (_U_(0x1) << USB_HOST_INTENCLR_DCONN_Pos)            /**< (USB_HOST_INTENCLR) Device Connection Interrupt Disable Mask */
#define USB_HOST_INTENCLR_DCONN(value)        (USB_HOST_INTENCLR_DCONN_Msk & ((value) << USB_HOST_INTENCLR_DCONN_Pos))
#define USB_HOST_INTENCLR_DDISC_Pos           _U_(9)                                               /**< (USB_HOST_INTENCLR) Device Disconnection Interrupt Disable Position */
#define USB_HOST_INTENCLR_DDISC_Msk           (_U_(0x1) << USB_HOST_INTENCLR_DDISC_Pos)            /**< (USB_HOST_INTENCLR) Device Disconnection Interrupt Disable Mask */
#define USB_HOST_INTENCLR_DDISC(value)        (USB_HOST_INTENCLR_DDISC_Msk & ((value) << USB_HOST_INTENCLR_DDISC_Pos))
#define USB_HOST_INTENCLR_Msk                 _U_(0x03FC)                                          /**< (USB_HOST_INTENCLR) Register Mask  */


/* -------- USB_DEVICE_INTENSET : (USB Offset: 0x18) (R/W 16) DEVICE Device Interrupt Enable Set -------- */
#define USB_DEVICE_INTENSET_RESETVALUE        _U_(0x00)                                            /**<  (USB_DEVICE_INTENSET) DEVICE Device Interrupt Enable Set  Reset Value */

#define USB_DEVICE_INTENSET_SUSPEND_Pos       _U_(0)                                               /**< (USB_DEVICE_INTENSET) Suspend Interrupt Enable Position */
#define USB_DEVICE_INTENSET_SUSPEND_Msk       (_U_(0x1) << USB_DEVICE_INTENSET_SUSPEND_Pos)        /**< (USB_DEVICE_INTENSET) Suspend Interrupt Enable Mask */
#define USB_DEVICE_INTENSET_SUSPEND(value)    (USB_DEVICE_INTENSET_SUSPEND_Msk & ((value) << USB_DEVICE_INTENSET_SUSPEND_Pos))
#define USB_DEVICE_INTENSET_MSOF_Pos          _U_(1)                                               /**< (USB_DEVICE_INTENSET) Micro Start of Frame Interrupt Enable in High Speed Mode Position */
#define USB_DEVICE_INTENSET_MSOF_Msk          (_U_(0x1) << USB_DEVICE_INTENSET_MSOF_Pos)           /**< (USB_DEVICE_INTENSET) Micro Start of Frame Interrupt Enable in High Speed Mode Mask */
#define USB_DEVICE_INTENSET_MSOF(value)       (USB_DEVICE_INTENSET_MSOF_Msk & ((value) << USB_DEVICE_INTENSET_MSOF_Pos))
#define USB_DEVICE_INTENSET_SOF_Pos           _U_(2)                                               /**< (USB_DEVICE_INTENSET) Start Of Frame Interrupt Enable Position */
#define USB_DEVICE_INTENSET_SOF_Msk           (_U_(0x1) << USB_DEVICE_INTENSET_SOF_Pos)            /**< (USB_DEVICE_INTENSET) Start Of Frame Interrupt Enable Mask */
#define USB_DEVICE_INTENSET_SOF(value)        (USB_DEVICE_INTENSET_SOF_Msk & ((value) << USB_DEVICE_INTENSET_SOF_Pos))
#define USB_DEVICE_INTENSET_EORST_Pos         _U_(3)                                               /**< (USB_DEVICE_INTENSET) End of Reset Interrupt Enable Position */
#define USB_DEVICE_INTENSET_EORST_Msk         (_U_(0x1) << USB_DEVICE_INTENSET_EORST_Pos)          /**< (USB_DEVICE_INTENSET) End of Reset Interrupt Enable Mask */
#define USB_DEVICE_INTENSET_EORST(value)      (USB_DEVICE_INTENSET_EORST_Msk & ((value) << USB_DEVICE_INTENSET_EORST_Pos))
#define USB_DEVICE_INTENSET_WAKEUP_Pos        _U_(4)                                               /**< (USB_DEVICE_INTENSET) Wake Up Interrupt Enable Position */
#define USB_DEVICE_INTENSET_WAKEUP_Msk        (_U_(0x1) << USB_DEVICE_INTENSET_WAKEUP_Pos)         /**< (USB_DEVICE_INTENSET) Wake Up Interrupt Enable Mask */
#define USB_DEVICE_INTENSET_WAKEUP(value)     (USB_DEVICE_INTENSET_WAKEUP_Msk & ((value) << USB_DEVICE_INTENSET_WAKEUP_Pos))
#define USB_DEVICE_INTENSET_EORSM_Pos         _U_(5)                                               /**< (USB_DEVICE_INTENSET) End Of Resume Interrupt Enable Position */
#define USB_DEVICE_INTENSET_EORSM_Msk         (_U_(0x1) << USB_DEVICE_INTENSET_EORSM_Pos)          /**< (USB_DEVICE_INTENSET) End Of Resume Interrupt Enable Mask */
#define USB_DEVICE_INTENSET_EORSM(value)      (USB_DEVICE_INTENSET_EORSM_Msk & ((value) << USB_DEVICE_INTENSET_EORSM_Pos))
#define USB_DEVICE_INTENSET_UPRSM_Pos         _U_(6)                                               /**< (USB_DEVICE_INTENSET) Upstream Resume Interrupt Enable Position */
#define USB_DEVICE_INTENSET_UPRSM_Msk         (_U_(0x1) << USB_DEVICE_INTENSET_UPRSM_Pos)          /**< (USB_DEVICE_INTENSET) Upstream Resume Interrupt Enable Mask */
#define USB_DEVICE_INTENSET_UPRSM(value)      (USB_DEVICE_INTENSET_UPRSM_Msk & ((value) << USB_DEVICE_INTENSET_UPRSM_Pos))
#define USB_DEVICE_INTENSET_RAMACER_Pos       _U_(7)                                               /**< (USB_DEVICE_INTENSET) Ram Access Interrupt Enable Position */
#define USB_DEVICE_INTENSET_RAMACER_Msk       (_U_(0x1) << USB_DEVICE_INTENSET_RAMACER_Pos)        /**< (USB_DEVICE_INTENSET) Ram Access Interrupt Enable Mask */
#define USB_DEVICE_INTENSET_RAMACER(value)    (USB_DEVICE_INTENSET_RAMACER_Msk & ((value) << USB_DEVICE_INTENSET_RAMACER_Pos))
#define USB_DEVICE_INTENSET_LPMNYET_Pos       _U_(8)                                               /**< (USB_DEVICE_INTENSET) Link Power Management Not Yet Interrupt Enable Position */
#define USB_DEVICE_INTENSET_LPMNYET_Msk       (_U_(0x1) << USB_DEVICE_INTENSET_LPMNYET_Pos)        /**< (USB_DEVICE_INTENSET) Link Power Management Not Yet Interrupt Enable Mask */
#define USB_DEVICE_INTENSET_LPMNYET(value)    (USB_DEVICE_INTENSET_LPMNYET_Msk & ((value) << USB_DEVICE_INTENSET_LPMNYET_Pos))
#define USB_DEVICE_INTENSET_LPMSUSP_Pos       _U_(9)                                               /**< (USB_DEVICE_INTENSET) Link Power Management Suspend Interrupt Enable Position */
#define USB_DEVICE_INTENSET_LPMSUSP_Msk       (_U_(0x1) << USB_DEVICE_INTENSET_LPMSUSP_Pos)        /**< (USB_DEVICE_INTENSET) Link Power Management Suspend Interrupt Enable Mask */
#define USB_DEVICE_INTENSET_LPMSUSP(value)    (USB_DEVICE_INTENSET_LPMSUSP_Msk & ((value) << USB_DEVICE_INTENSET_LPMSUSP_Pos))
#define USB_DEVICE_INTENSET_Msk               _U_(0x03FF)                                          /**< (USB_DEVICE_INTENSET) Register Mask  */


/* -------- USB_HOST_INTENSET : (USB Offset: 0x18) (R/W 16) HOST Host Interrupt Enable Set -------- */
#define USB_HOST_INTENSET_RESETVALUE          _U_(0x00)                                            /**<  (USB_HOST_INTENSET) HOST Host Interrupt Enable Set  Reset Value */

#define USB_HOST_INTENSET_HSOF_Pos            _U_(2)                                               /**< (USB_HOST_INTENSET) Host Start Of Frame Interrupt Enable Position */
#define USB_HOST_INTENSET_HSOF_Msk            (_U_(0x1) << USB_HOST_INTENSET_HSOF_Pos)             /**< (USB_HOST_INTENSET) Host Start Of Frame Interrupt Enable Mask */
#define USB_HOST_INTENSET_HSOF(value)         (USB_HOST_INTENSET_HSOF_Msk & ((value) << USB_HOST_INTENSET_HSOF_Pos))
#define USB_HOST_INTENSET_RST_Pos             _U_(3)                                               /**< (USB_HOST_INTENSET) Bus Reset Interrupt Enable Position */
#define USB_HOST_INTENSET_RST_Msk             (_U_(0x1) << USB_HOST_INTENSET_RST_Pos)              /**< (USB_HOST_INTENSET) Bus Reset Interrupt Enable Mask */
#define USB_HOST_INTENSET_RST(value)          (USB_HOST_INTENSET_RST_Msk & ((value) << USB_HOST_INTENSET_RST_Pos))
#define USB_HOST_INTENSET_WAKEUP_Pos          _U_(4)                                               /**< (USB_HOST_INTENSET) Wake Up Interrupt Enable Position */
#define USB_HOST_INTENSET_WAKEUP_Msk          (_U_(0x1) << USB_HOST_INTENSET_WAKEUP_Pos)           /**< (USB_HOST_INTENSET) Wake Up Interrupt Enable Mask */
#define USB_HOST_INTENSET_WAKEUP(value)       (USB_HOST_INTENSET_WAKEUP_Msk & ((value) << USB_HOST_INTENSET_WAKEUP_Pos))
#define USB_HOST_INTENSET_DNRSM_Pos           _U_(5)                                               /**< (USB_HOST_INTENSET) DownStream to the Device Interrupt Enable Position */
#define USB_HOST_INTENSET_DNRSM_Msk           (_U_(0x1) << USB_HOST_INTENSET_DNRSM_Pos)            /**< (USB_HOST_INTENSET) DownStream to the Device Interrupt Enable Mask */
#define USB_HOST_INTENSET_DNRSM(value)        (USB_HOST_INTENSET_DNRSM_Msk & ((value) << USB_HOST_INTENSET_DNRSM_Pos))
#define USB_HOST_INTENSET_UPRSM_Pos           _U_(6)                                               /**< (USB_HOST_INTENSET) Upstream Resume fromthe device Interrupt Enable Position */
#define USB_HOST_INTENSET_UPRSM_Msk           (_U_(0x1) << USB_HOST_INTENSET_UPRSM_Pos)            /**< (USB_HOST_INTENSET) Upstream Resume fromthe device Interrupt Enable Mask */
#define USB_HOST_INTENSET_UPRSM(value)        (USB_HOST_INTENSET_UPRSM_Msk & ((value) << USB_HOST_INTENSET_UPRSM_Pos))
#define USB_HOST_INTENSET_RAMACER_Pos         _U_(7)                                               /**< (USB_HOST_INTENSET) Ram Access Interrupt Enable Position */
#define USB_HOST_INTENSET_RAMACER_Msk         (_U_(0x1) << USB_HOST_INTENSET_RAMACER_Pos)          /**< (USB_HOST_INTENSET) Ram Access Interrupt Enable Mask */
#define USB_HOST_INTENSET_RAMACER(value)      (USB_HOST_INTENSET_RAMACER_Msk & ((value) << USB_HOST_INTENSET_RAMACER_Pos))
#define USB_HOST_INTENSET_DCONN_Pos           _U_(8)                                               /**< (USB_HOST_INTENSET) Link Power Management Interrupt Enable Position */
#define USB_HOST_INTENSET_DCONN_Msk           (_U_(0x1) << USB_HOST_INTENSET_DCONN_Pos)            /**< (USB_HOST_INTENSET) Link Power Management Interrupt Enable Mask */
#define USB_HOST_INTENSET_DCONN(value)        (USB_HOST_INTENSET_DCONN_Msk & ((value) << USB_HOST_INTENSET_DCONN_Pos))
#define USB_HOST_INTENSET_DDISC_Pos           _U_(9)                                               /**< (USB_HOST_INTENSET) Device Disconnection Interrupt Enable Position */
#define USB_HOST_INTENSET_DDISC_Msk           (_U_(0x1) << USB_HOST_INTENSET_DDISC_Pos)            /**< (USB_HOST_INTENSET) Device Disconnection Interrupt Enable Mask */
#define USB_HOST_INTENSET_DDISC(value)        (USB_HOST_INTENSET_DDISC_Msk & ((value) << USB_HOST_INTENSET_DDISC_Pos))
#define USB_HOST_INTENSET_Msk                 _U_(0x03FC)                                          /**< (USB_HOST_INTENSET) Register Mask  */


/* -------- USB_DEVICE_INTFLAG : (USB Offset: 0x1C) (R/W 16) DEVICE Device Interrupt Flag -------- */
#define USB_DEVICE_INTFLAG_RESETVALUE         _U_(0x00)                                            /**<  (USB_DEVICE_INTFLAG) DEVICE Device Interrupt Flag  Reset Value */

#define USB_DEVICE_INTFLAG_SUSPEND_Pos        _U_(0)                                               /**< (USB_DEVICE_INTFLAG) Suspend Position */
#define USB_DEVICE_INTFLAG_SUSPEND_Msk        (_U_(0x1) << USB_DEVICE_INTFLAG_SUSPEND_Pos)         /**< (USB_DEVICE_INTFLAG) Suspend Mask */
#define USB_DEVICE_INTFLAG_SUSPEND(value)     (USB_DEVICE_INTFLAG_SUSPEND_Msk & ((value) << USB_DEVICE_INTFLAG_SUSPEND_Pos))
#define USB_DEVICE_INTFLAG_MSOF_Pos           _U_(1)                                               /**< (USB_DEVICE_INTFLAG) Micro Start of Frame in High Speed Mode Position */
#define USB_DEVICE_INTFLAG_MSOF_Msk           (_U_(0x1) << USB_DEVICE_INTFLAG_MSOF_Pos)            /**< (USB_DEVICE_INTFLAG) Micro Start of Frame in High Speed Mode Mask */
#define USB_DEVICE_INTFLAG_MSOF(value)        (USB_DEVICE_INTFLAG_MSOF_Msk & ((value) << USB_DEVICE_INTFLAG_MSOF_Pos))
#define USB_DEVICE_INTFLAG_SOF_Pos            _U_(2)                                               /**< (USB_DEVICE_INTFLAG) Start Of Frame Position */
#define USB_DEVICE_INTFLAG_SOF_Msk            (_U_(0x1) << USB_DEVICE_INTFLAG_SOF_Pos)             /**< (USB_DEVICE_INTFLAG) Start Of Frame Mask */
#define USB_DEVICE_INTFLAG_SOF(value)         (USB_DEVICE_INTFLAG_SOF_Msk & ((value) << USB_DEVICE_INTFLAG_SOF_Pos))
#define USB_DEVICE_INTFLAG_EORST_Pos          _U_(3)                                               /**< (USB_DEVICE_INTFLAG) End of Reset Position */
#define USB_DEVICE_INTFLAG_EORST_Msk          (_U_(0x1) << USB_DEVICE_INTFLAG_EORST_Pos)           /**< (USB_DEVICE_INTFLAG) End of Reset Mask */
#define USB_DEVICE_INTFLAG_EORST(value)       (USB_DEVICE_INTFLAG_EORST_Msk & ((value) << USB_DEVICE_INTFLAG_EORST_Pos))
#define USB_DEVICE_INTFLAG_WAKEUP_Pos         _U_(4)                                               /**< (USB_DEVICE_INTFLAG) Wake Up Position */
#define USB_DEVICE_INTFLAG_WAKEUP_Msk         (_U_(0x1) << USB_DEVICE_INTFLAG_WAKEUP_Pos)          /**< (USB_DEVICE_INTFLAG) Wake Up Mask */
#define USB_DEVICE_INTFLAG_WAKEUP(value)      (USB_DEVICE_INTFLAG_WAKEUP_Msk & ((value) << USB_DEVICE_INTFLAG_WAKEUP_Pos))
#define USB_DEVICE_INTFLAG_EORSM_Pos          _U_(5)                                               /**< (USB_DEVICE_INTFLAG) End Of Resume Position */
#define USB_DEVICE_INTFLAG_EORSM_Msk          (_U_(0x1) << USB_DEVICE_INTFLAG_EORSM_Pos)           /**< (USB_DEVICE_INTFLAG) End Of Resume Mask */
#define USB_DEVICE_INTFLAG_EORSM(value)       (USB_DEVICE_INTFLAG_EORSM_Msk & ((value) << USB_DEVICE_INTFLAG_EORSM_Pos))
#define USB_DEVICE_INTFLAG_UPRSM_Pos          _U_(6)                                               /**< (USB_DEVICE_INTFLAG) Upstream Resume Position */
#define USB_DEVICE_INTFLAG_UPRSM_Msk          (_U_(0x1) << USB_DEVICE_INTFLAG_UPRSM_Pos)           /**< (USB_DEVICE_INTFLAG) Upstream Resume Mask */
#define USB_DEVICE_INTFLAG_UPRSM(value)       (USB_DEVICE_INTFLAG_UPRSM_Msk & ((value) << USB_DEVICE_INTFLAG_UPRSM_Pos))
#define USB_DEVICE_INTFLAG_RAMACER_Pos        _U_(7)                                               /**< (USB_DEVICE_INTFLAG) Ram Access Position */
#define USB_DEVICE_INTFLAG_RAMACER_Msk        (_U_(0x1) << USB_DEVICE_INTFLAG_RAMACER_Pos)         /**< (USB_DEVICE_INTFLAG) Ram Access Mask */
#define USB_DEVICE_INTFLAG_RAMACER(value)     (USB_DEVICE_INTFLAG_RAMACER_Msk & ((value) << USB_DEVICE_INTFLAG_RAMACER_Pos))
#define USB_DEVICE_INTFLAG_LPMNYET_Pos        _U_(8)                                               /**< (USB_DEVICE_INTFLAG) Link Power Management Not Yet Position */
#define USB_DEVICE_INTFLAG_LPMNYET_Msk        (_U_(0x1) << USB_DEVICE_INTFLAG_LPMNYET_Pos)         /**< (USB_DEVICE_INTFLAG) Link Power Management Not Yet Mask */
#define USB_DEVICE_INTFLAG_LPMNYET(value)     (USB_DEVICE_INTFLAG_LPMNYET_Msk & ((value) << USB_DEVICE_INTFLAG_LPMNYET_Pos))
#define USB_DEVICE_INTFLAG_LPMSUSP_Pos        _U_(9)                                               /**< (USB_DEVICE_INTFLAG) Link Power Management Suspend Position */
#define USB_DEVICE_INTFLAG_LPMSUSP_Msk        (_U_(0x1) << USB_DEVICE_INTFLAG_LPMSUSP_Pos)         /**< (USB_DEVICE_INTFLAG) Link Power Management Suspend Mask */
#define USB_DEVICE_INTFLAG_LPMSUSP(value)     (USB_DEVICE_INTFLAG_LPMSUSP_Msk & ((value) << USB_DEVICE_INTFLAG_LPMSUSP_Pos))
#define USB_DEVICE_INTFLAG_Msk                _U_(0x03FF)                                          /**< (USB_DEVICE_INTFLAG) Register Mask  */


/* -------- USB_HOST_INTFLAG : (USB Offset: 0x1C) (R/W 16) HOST Host Interrupt Flag -------- */
#define USB_HOST_INTFLAG_RESETVALUE           _U_(0x00)                                            /**<  (USB_HOST_INTFLAG) HOST Host Interrupt Flag  Reset Value */

#define USB_HOST_INTFLAG_HSOF_Pos             _U_(2)                                               /**< (USB_HOST_INTFLAG) Host Start Of Frame Position */
#define USB_HOST_INTFLAG_HSOF_Msk             (_U_(0x1) << USB_HOST_INTFLAG_HSOF_Pos)              /**< (USB_HOST_INTFLAG) Host Start Of Frame Mask */
#define USB_HOST_INTFLAG_HSOF(value)          (USB_HOST_INTFLAG_HSOF_Msk & ((value) << USB_HOST_INTFLAG_HSOF_Pos))
#define USB_HOST_INTFLAG_RST_Pos              _U_(3)                                               /**< (USB_HOST_INTFLAG) Bus Reset Position */
#define USB_HOST_INTFLAG_RST_Msk              (_U_(0x1) << USB_HOST_INTFLAG_RST_Pos)               /**< (USB_HOST_INTFLAG) Bus Reset Mask */
#define USB_HOST_INTFLAG_RST(value)           (USB_HOST_INTFLAG_RST_Msk & ((value) << USB_HOST_INTFLAG_RST_Pos))
#define USB_HOST_INTFLAG_WAKEUP_Pos           _U_(4)                                               /**< (USB_HOST_INTFLAG) Wake Up Position */
#define USB_HOST_INTFLAG_WAKEUP_Msk           (_U_(0x1) << USB_HOST_INTFLAG_WAKEUP_Pos)            /**< (USB_HOST_INTFLAG) Wake Up Mask */
#define USB_HOST_INTFLAG_WAKEUP(value)        (USB_HOST_INTFLAG_WAKEUP_Msk & ((value) << USB_HOST_INTFLAG_WAKEUP_Pos))
#define USB_HOST_INTFLAG_DNRSM_Pos            _U_(5)                                               /**< (USB_HOST_INTFLAG) Downstream Position */
#define USB_HOST_INTFLAG_DNRSM_Msk            (_U_(0x1) << USB_HOST_INTFLAG_DNRSM_Pos)             /**< (USB_HOST_INTFLAG) Downstream Mask */
#define USB_HOST_INTFLAG_DNRSM(value)         (USB_HOST_INTFLAG_DNRSM_Msk & ((value) << USB_HOST_INTFLAG_DNRSM_Pos))
#define USB_HOST_INTFLAG_UPRSM_Pos            _U_(6)                                               /**< (USB_HOST_INTFLAG) Upstream Resume from the Device Position */
#define USB_HOST_INTFLAG_UPRSM_Msk            (_U_(0x1) << USB_HOST_INTFLAG_UPRSM_Pos)             /**< (USB_HOST_INTFLAG) Upstream Resume from the Device Mask */
#define USB_HOST_INTFLAG_UPRSM(value)         (USB_HOST_INTFLAG_UPRSM_Msk & ((value) << USB_HOST_INTFLAG_UPRSM_Pos))
#define USB_HOST_INTFLAG_RAMACER_Pos          _U_(7)                                               /**< (USB_HOST_INTFLAG) Ram Access Position */
#define USB_HOST_INTFLAG_RAMACER_Msk          (_U_(0x1) << USB_HOST_INTFLAG_RAMACER_Pos)           /**< (USB_HOST_INTFLAG) Ram Access Mask */
#define USB_HOST_INTFLAG_RAMACER(value)       (USB_HOST_INTFLAG_RAMACER_Msk & ((value) << USB_HOST_INTFLAG_RAMACER_Pos))
#define USB_HOST_INTFLAG_DCONN_Pos            _U_(8)                                               /**< (USB_HOST_INTFLAG) Device Connection Position */
#define USB_HOST_INTFLAG_DCONN_Msk            (_U_(0x1) << USB_HOST_INTFLAG_DCONN_Pos)             /**< (USB_HOST_INTFLAG) Device Connection Mask */
#define USB_HOST_INTFLAG_DCONN(value)         (USB_HOST_INTFLAG_DCONN_Msk & ((value) << USB_HOST_INTFLAG_DCONN_Pos))
#define USB_HOST_INTFLAG_DDISC_Pos            _U_(9)                                               /**< (USB_HOST_INTFLAG) Device Disconnection Position */
#define USB_HOST_INTFLAG_DDISC_Msk            (_U_(0x1) << USB_HOST_INTFLAG_DDISC_Pos)             /**< (USB_HOST_INTFLAG) Device Disconnection Mask */
#define USB_HOST_INTFLAG_DDISC(value)         (USB_HOST_INTFLAG_DDISC_Msk & ((value) << USB_HOST_INTFLAG_DDISC_Pos))
#define USB_HOST_INTFLAG_Msk                  _U_(0x03FC)                                          /**< (USB_HOST_INTFLAG) Register Mask  */


/* -------- USB_DEVICE_EPINTSMRY : (USB Offset: 0x20) ( R/ 16) DEVICE End Point Interrupt Summary -------- */
#define USB_DEVICE_EPINTSMRY_RESETVALUE       _U_(0x00)                                            /**<  (USB_DEVICE_EPINTSMRY) DEVICE End Point Interrupt Summary  Reset Value */

#define USB_DEVICE_EPINTSMRY_EPINT0_Pos       _U_(0)                                               /**< (USB_DEVICE_EPINTSMRY) End Point 0 Interrupt Position */
#define USB_DEVICE_EPINTSMRY_EPINT0_Msk       (_U_(0x1) << USB_DEVICE_EPINTSMRY_EPINT0_Pos)        /**< (USB_DEVICE_EPINTSMRY) End Point 0 Interrupt Mask */
#define USB_DEVICE_EPINTSMRY_EPINT0(value)    (USB_DEVICE_EPINTSMRY_EPINT0_Msk & ((value) << USB_DEVICE_EPINTSMRY_EPINT0_Pos))
#define USB_DEVICE_EPINTSMRY_EPINT1_Pos       _U_(1)                                               /**< (USB_DEVICE_EPINTSMRY) End Point 1 Interrupt Position */
#define USB_DEVICE_EPINTSMRY_EPINT1_Msk       (_U_(0x1) << USB_DEVICE_EPINTSMRY_EPINT1_Pos)        /**< (USB_DEVICE_EPINTSMRY) End Point 1 Interrupt Mask */
#define USB_DEVICE_EPINTSMRY_EPINT1(value)    (USB_DEVICE_EPINTSMRY_EPINT1_Msk & ((value) << USB_DEVICE_EPINTSMRY_EPINT1_Pos))
#define USB_DEVICE_EPINTSMRY_EPINT2_Pos       _U_(2)                                               /**< (USB_DEVICE_EPINTSMRY) End Point 2 Interrupt Position */
#define USB_DEVICE_EPINTSMRY_EPINT2_Msk       (_U_(0x1) << USB_DEVICE_EPINTSMRY_EPINT2_Pos)        /**< (USB_DEVICE_EPINTSMRY) End Point 2 Interrupt Mask */
#define USB_DEVICE_EPINTSMRY_EPINT2(value)    (USB_DEVICE_EPINTSMRY_EPINT2_Msk & ((value) << USB_DEVICE_EPINTSMRY_EPINT2_Pos))
#define USB_DEVICE_EPINTSMRY_EPINT3_Pos       _U_(3)                                               /**< (USB_DEVICE_EPINTSMRY) End Point 3 Interrupt Position */
#define USB_DEVICE_EPINTSMRY_EPINT3_Msk       (_U_(0x1) << USB_DEVICE_EPINTSMRY_EPINT3_Pos)        /**< (USB_DEVICE_EPINTSMRY) End Point 3 Interrupt Mask */
#define USB_DEVICE_EPINTSMRY_EPINT3(value)    (USB_DEVICE_EPINTSMRY_EPINT3_Msk & ((value) << USB_DEVICE_EPINTSMRY_EPINT3_Pos))
#define USB_DEVICE_EPINTSMRY_EPINT4_Pos       _U_(4)                                               /**< (USB_DEVICE_EPINTSMRY) End Point 4 Interrupt Position */
#define USB_DEVICE_EPINTSMRY_EPINT4_Msk       (_U_(0x1) << USB_DEVICE_EPINTSMRY_EPINT4_Pos)        /**< (USB_DEVICE_EPINTSMRY) End Point 4 Interrupt Mask */
#define USB_DEVICE_EPINTSMRY_EPINT4(value)    (USB_DEVICE_EPINTSMRY_EPINT4_Msk & ((value) << USB_DEVICE_EPINTSMRY_EPINT4_Pos))
#define USB_DEVICE_EPINTSMRY_EPINT5_Pos       _U_(5)                                               /**< (USB_DEVICE_EPINTSMRY) End Point 5 Interrupt Position */
#define USB_DEVICE_EPINTSMRY_EPINT5_Msk       (_U_(0x1) << USB_DEVICE_EPINTSMRY_EPINT5_Pos)        /**< (USB_DEVICE_EPINTSMRY) End Point 5 Interrupt Mask */
#define USB_DEVICE_EPINTSMRY_EPINT5(value)    (USB_DEVICE_EPINTSMRY_EPINT5_Msk & ((value) << USB_DEVICE_EPINTSMRY_EPINT5_Pos))
#define USB_DEVICE_EPINTSMRY_EPINT6_Pos       _U_(6)                                               /**< (USB_DEVICE_EPINTSMRY) End Point 6 Interrupt Position */
#define USB_DEVICE_EPINTSMRY_EPINT6_Msk       (_U_(0x1) << USB_DEVICE_EPINTSMRY_EPINT6_Pos)        /**< (USB_DEVICE_EPINTSMRY) End Point 6 Interrupt Mask */
#define USB_DEVICE_EPINTSMRY_EPINT6(value)    (USB_DEVICE_EPINTSMRY_EPINT6_Msk & ((value) << USB_DEVICE_EPINTSMRY_EPINT6_Pos))
#define USB_DEVICE_EPINTSMRY_EPINT7_Pos       _U_(7)                                               /**< (USB_DEVICE_EPINTSMRY) End Point 7 Interrupt Position */
#define USB_DEVICE_EPINTSMRY_EPINT7_Msk       (_U_(0x1) << USB_DEVICE_EPINTSMRY_EPINT7_Pos)        /**< (USB_DEVICE_EPINTSMRY) End Point 7 Interrupt Mask */
#define USB_DEVICE_EPINTSMRY_EPINT7(value)    (USB_DEVICE_EPINTSMRY_EPINT7_Msk & ((value) << USB_DEVICE_EPINTSMRY_EPINT7_Pos))
#define USB_DEVICE_EPINTSMRY_Msk              _U_(0x00FF)                                          /**< (USB_DEVICE_EPINTSMRY) Register Mask  */

#define USB_DEVICE_EPINTSMRY_EPINT_Pos        _U_(0)                                               /**< (USB_DEVICE_EPINTSMRY Position) End Point 7 Interrupt */
#define USB_DEVICE_EPINTSMRY_EPINT_Msk        (_U_(0xFF) << USB_DEVICE_EPINTSMRY_EPINT_Pos)        /**< (USB_DEVICE_EPINTSMRY Mask) EPINT */
#define USB_DEVICE_EPINTSMRY_EPINT(value)     (USB_DEVICE_EPINTSMRY_EPINT_Msk & ((value) << USB_DEVICE_EPINTSMRY_EPINT_Pos)) 

/* -------- USB_HOST_PINTSMRY : (USB Offset: 0x20) ( R/ 16) HOST Pipe Interrupt Summary -------- */
#define USB_HOST_PINTSMRY_RESETVALUE          _U_(0x00)                                            /**<  (USB_HOST_PINTSMRY) HOST Pipe Interrupt Summary  Reset Value */

#define USB_HOST_PINTSMRY_EPINT0_Pos          _U_(0)                                               /**< (USB_HOST_PINTSMRY) Pipe 0 Interrupt Position */
#define USB_HOST_PINTSMRY_EPINT0_Msk          (_U_(0x1) << USB_HOST_PINTSMRY_EPINT0_Pos)           /**< (USB_HOST_PINTSMRY) Pipe 0 Interrupt Mask */
#define USB_HOST_PINTSMRY_EPINT0(value)       (USB_HOST_PINTSMRY_EPINT0_Msk & ((value) << USB_HOST_PINTSMRY_EPINT0_Pos))
#define USB_HOST_PINTSMRY_EPINT1_Pos          _U_(1)                                               /**< (USB_HOST_PINTSMRY) Pipe 1 Interrupt Position */
#define USB_HOST_PINTSMRY_EPINT1_Msk          (_U_(0x1) << USB_HOST_PINTSMRY_EPINT1_Pos)           /**< (USB_HOST_PINTSMRY) Pipe 1 Interrupt Mask */
#define USB_HOST_PINTSMRY_EPINT1(value)       (USB_HOST_PINTSMRY_EPINT1_Msk & ((value) << USB_HOST_PINTSMRY_EPINT1_Pos))
#define USB_HOST_PINTSMRY_EPINT2_Pos          _U_(2)                                               /**< (USB_HOST_PINTSMRY) Pipe 2 Interrupt Position */
#define USB_HOST_PINTSMRY_EPINT2_Msk          (_U_(0x1) << USB_HOST_PINTSMRY_EPINT2_Pos)           /**< (USB_HOST_PINTSMRY) Pipe 2 Interrupt Mask */
#define USB_HOST_PINTSMRY_EPINT2(value)       (USB_HOST_PINTSMRY_EPINT2_Msk & ((value) << USB_HOST_PINTSMRY_EPINT2_Pos))
#define USB_HOST_PINTSMRY_EPINT3_Pos          _U_(3)                                               /**< (USB_HOST_PINTSMRY) Pipe 3 Interrupt Position */
#define USB_HOST_PINTSMRY_EPINT3_Msk          (_U_(0x1) << USB_HOST_PINTSMRY_EPINT3_Pos)           /**< (USB_HOST_PINTSMRY) Pipe 3 Interrupt Mask */
#define USB_HOST_PINTSMRY_EPINT3(value)       (USB_HOST_PINTSMRY_EPINT3_Msk & ((value) << USB_HOST_PINTSMRY_EPINT3_Pos))
#define USB_HOST_PINTSMRY_EPINT4_Pos          _U_(4)                                               /**< (USB_HOST_PINTSMRY) Pipe 4 Interrupt Position */
#define USB_HOST_PINTSMRY_EPINT4_Msk          (_U_(0x1) << USB_HOST_PINTSMRY_EPINT4_Pos)           /**< (USB_HOST_PINTSMRY) Pipe 4 Interrupt Mask */
#define USB_HOST_PINTSMRY_EPINT4(value)       (USB_HOST_PINTSMRY_EPINT4_Msk & ((value) << USB_HOST_PINTSMRY_EPINT4_Pos))
#define USB_HOST_PINTSMRY_EPINT5_Pos          _U_(5)                                               /**< (USB_HOST_PINTSMRY) Pipe 5 Interrupt Position */
#define USB_HOST_PINTSMRY_EPINT5_Msk          (_U_(0x1) << USB_HOST_PINTSMRY_EPINT5_Pos)           /**< (USB_HOST_PINTSMRY) Pipe 5 Interrupt Mask */
#define USB_HOST_PINTSMRY_EPINT5(value)       (USB_HOST_PINTSMRY_EPINT5_Msk & ((value) << USB_HOST_PINTSMRY_EPINT5_Pos))
#define USB_HOST_PINTSMRY_EPINT6_Pos          _U_(6)                                               /**< (USB_HOST_PINTSMRY) Pipe 6 Interrupt Position */
#define USB_HOST_PINTSMRY_EPINT6_Msk          (_U_(0x1) << USB_HOST_PINTSMRY_EPINT6_Pos)           /**< (USB_HOST_PINTSMRY) Pipe 6 Interrupt Mask */
#define USB_HOST_PINTSMRY_EPINT6(value)       (USB_HOST_PINTSMRY_EPINT6_Msk & ((value) << USB_HOST_PINTSMRY_EPINT6_Pos))
#define USB_HOST_PINTSMRY_EPINT7_Pos          _U_(7)                                               /**< (USB_HOST_PINTSMRY) Pipe 7 Interrupt Position */
#define USB_HOST_PINTSMRY_EPINT7_Msk          (_U_(0x1) << USB_HOST_PINTSMRY_EPINT7_Pos)           /**< (USB_HOST_PINTSMRY) Pipe 7 Interrupt Mask */
#define USB_HOST_PINTSMRY_EPINT7(value)       (USB_HOST_PINTSMRY_EPINT7_Msk & ((value) << USB_HOST_PINTSMRY_EPINT7_Pos))
#define USB_HOST_PINTSMRY_Msk                 _U_(0x00FF)                                          /**< (USB_HOST_PINTSMRY) Register Mask  */

#define USB_HOST_PINTSMRY_EPINT_Pos           _U_(0)                                               /**< (USB_HOST_PINTSMRY Position) Pipe 7 Interrupt */
#define USB_HOST_PINTSMRY_EPINT_Msk           (_U_(0xFF) << USB_HOST_PINTSMRY_EPINT_Pos)           /**< (USB_HOST_PINTSMRY Mask) EPINT */
#define USB_HOST_PINTSMRY_EPINT(value)        (USB_HOST_PINTSMRY_EPINT_Msk & ((value) << USB_HOST_PINTSMRY_EPINT_Pos)) 

/* -------- USB_DESCADD : (USB Offset: 0x24) (R/W 32) Descriptor Address -------- */
#define USB_DESCADD_RESETVALUE                _U_(0x00)                                            /**<  (USB_DESCADD) Descriptor Address  Reset Value */

#define USB_DESCADD_DESCADD_Pos               _U_(0)                                               /**< (USB_DESCADD) Descriptor Address Value Position */
#define USB_DESCADD_DESCADD_Msk               (_U_(0xFFFFFFFF) << USB_DESCADD_DESCADD_Pos)         /**< (USB_DESCADD) Descriptor Address Value Mask */
#define USB_DESCADD_DESCADD(value)            (USB_DESCADD_DESCADD_Msk & ((value) << USB_DESCADD_DESCADD_Pos))
#define USB_DESCADD_Msk                       _U_(0xFFFFFFFF)                                      /**< (USB_DESCADD) Register Mask  */


/* -------- USB_PADCAL : (USB Offset: 0x28) (R/W 16) USB PAD Calibration -------- */
#define USB_PADCAL_RESETVALUE                 _U_(0x00)                                            /**<  (USB_PADCAL) USB PAD Calibration  Reset Value */

#define USB_PADCAL_TRANSP_Pos                 _U_(0)                                               /**< (USB_PADCAL) USB Pad Transp calibration Position */
#define USB_PADCAL_TRANSP_Msk                 (_U_(0x1F) << USB_PADCAL_TRANSP_Pos)                 /**< (USB_PADCAL) USB Pad Transp calibration Mask */
#define USB_PADCAL_TRANSP(value)              (USB_PADCAL_TRANSP_Msk & ((value) << USB_PADCAL_TRANSP_Pos))
#define USB_PADCAL_TRANSN_Pos                 _U_(6)                                               /**< (USB_PADCAL) USB Pad Transn calibration Position */
#define USB_PADCAL_TRANSN_Msk                 (_U_(0x1F) << USB_PADCAL_TRANSN_Pos)                 /**< (USB_PADCAL) USB Pad Transn calibration Mask */
#define USB_PADCAL_TRANSN(value)              (USB_PADCAL_TRANSN_Msk & ((value) << USB_PADCAL_TRANSN_Pos))
#define USB_PADCAL_TRIM_Pos                   _U_(12)                                              /**< (USB_PADCAL) USB Pad Trim calibration Position */
#define USB_PADCAL_TRIM_Msk                   (_U_(0x7) << USB_PADCAL_TRIM_Pos)                    /**< (USB_PADCAL) USB Pad Trim calibration Mask */
#define USB_PADCAL_TRIM(value)                (USB_PADCAL_TRIM_Msk & ((value) << USB_PADCAL_TRIM_Pos))
#define USB_PADCAL_Msk                        _U_(0x77DF)                                          /**< (USB_PADCAL) Register Mask  */


/** \brief USB register offsets definitions */
#define USB_DEVICE_ADDR_REG_OFST       (0x00)              /**< (USB_DEVICE_ADDR) DEVICE_DESC_BANK Endpoint Bank, Adress of Data Buffer Offset */
#define USB_DEVICE_PCKSIZE_REG_OFST    (0x04)              /**< (USB_DEVICE_PCKSIZE) DEVICE_DESC_BANK Endpoint Bank, Packet Size Offset */
#define USB_DEVICE_EXTREG_REG_OFST     (0x08)              /**< (USB_DEVICE_EXTREG) DEVICE_DESC_BANK Endpoint Bank, Extended Offset */
#define USB_DEVICE_STATUS_BK_REG_OFST  (0x0A)              /**< (USB_DEVICE_STATUS_BK) DEVICE_DESC_BANK Enpoint Bank, Status of Bank Offset */
#define USB_HOST_ADDR_REG_OFST         (0x00)              /**< (USB_HOST_ADDR) HOST_DESC_BANK Host Bank, Adress of Data Buffer Offset */
#define USB_HOST_PCKSIZE_REG_OFST      (0x04)              /**< (USB_HOST_PCKSIZE) HOST_DESC_BANK Host Bank, Packet Size Offset */
#define USB_HOST_EXTREG_REG_OFST       (0x08)              /**< (USB_HOST_EXTREG) HOST_DESC_BANK Host Bank, Extended Offset */
#define USB_HOST_STATUS_BK_REG_OFST    (0x0A)              /**< (USB_HOST_STATUS_BK) HOST_DESC_BANK Host Bank, Status of Bank Offset */
#define USB_HOST_CTRL_PIPE_REG_OFST    (0x0C)              /**< (USB_HOST_CTRL_PIPE) HOST_DESC_BANK Host Bank, Host Control Pipe Offset */
#define USB_HOST_STATUS_PIPE_REG_OFST  (0x0E)              /**< (USB_HOST_STATUS_PIPE) HOST_DESC_BANK Host Bank, Host Status Pipe Offset */
#define USB_DEVICE_EPCFG_REG_OFST      (0x00)              /**< (USB_DEVICE_EPCFG) DEVICE_ENDPOINT End Point Configuration Offset */
#define USB_DEVICE_EPSTATUSCLR_REG_OFST (0x04)              /**< (USB_DEVICE_EPSTATUSCLR) DEVICE_ENDPOINT End Point Pipe Status Clear Offset */
#define USB_DEVICE_EPSTATUSSET_REG_OFST (0x05)              /**< (USB_DEVICE_EPSTATUSSET) DEVICE_ENDPOINT End Point Pipe Status Set Offset */
#define USB_DEVICE_EPSTATUS_REG_OFST   (0x06)              /**< (USB_DEVICE_EPSTATUS) DEVICE_ENDPOINT End Point Pipe Status Offset */
#define USB_DEVICE_EPINTFLAG_REG_OFST  (0x07)              /**< (USB_DEVICE_EPINTFLAG) DEVICE_ENDPOINT End Point Interrupt Flag Offset */
#define USB_DEVICE_EPINTENCLR_REG_OFST (0x08)              /**< (USB_DEVICE_EPINTENCLR) DEVICE_ENDPOINT End Point Interrupt Clear Flag Offset */
#define USB_DEVICE_EPINTENSET_REG_OFST (0x09)              /**< (USB_DEVICE_EPINTENSET) DEVICE_ENDPOINT End Point Interrupt Set Flag Offset */
#define USB_HOST_PCFG_REG_OFST         (0x00)              /**< (USB_HOST_PCFG) HOST_PIPE End Point Configuration Offset */
#define USB_HOST_BINTERVAL_REG_OFST    (0x03)              /**< (USB_HOST_BINTERVAL) HOST_PIPE Bus Access Period of Pipe Offset */
#define USB_HOST_PSTATUSCLR_REG_OFST   (0x04)              /**< (USB_HOST_PSTATUSCLR) HOST_PIPE End Point Pipe Status Clear Offset */
#define USB_HOST_PSTATUSSET_REG_OFST   (0x05)              /**< (USB_HOST_PSTATUSSET) HOST_PIPE End Point Pipe Status Set Offset */
#define USB_HOST_PSTATUS_REG_OFST      (0x06)              /**< (USB_HOST_PSTATUS) HOST_PIPE End Point Pipe Status Offset */
#define USB_HOST_PINTFLAG_REG_OFST     (0x07)              /**< (USB_HOST_PINTFLAG) HOST_PIPE Pipe Interrupt Flag Offset */
#define USB_HOST_PINTENCLR_REG_OFST    (0x08)              /**< (USB_HOST_PINTENCLR) HOST_PIPE Pipe Interrupt Flag Clear Offset */
#define USB_HOST_PINTENSET_REG_OFST    (0x09)              /**< (USB_HOST_PINTENSET) HOST_PIPE Pipe Interrupt Flag Set Offset */
#define USB_CTRLA_REG_OFST             (0x00)              /**< (USB_CTRLA) Control A Offset */
#define USB_SYNCBUSY_REG_OFST          (0x02)              /**< (USB_SYNCBUSY) Synchronization Busy Offset */
#define USB_QOSCTRL_REG_OFST           (0x03)              /**< (USB_QOSCTRL) USB Quality Of Service Offset */
#define USB_DEVICE_CTRLB_REG_OFST      (0x08)              /**< (USB_DEVICE_CTRLB) DEVICE Control B Offset */
#define USB_HOST_CTRLB_REG_OFST        (0x08)              /**< (USB_HOST_CTRLB) HOST Control B Offset */
#define USB_DEVICE_DADD_REG_OFST       (0x0A)              /**< (USB_DEVICE_DADD) DEVICE Device Address Offset */
#define USB_HOST_HSOFC_REG_OFST        (0x0A)              /**< (USB_HOST_HSOFC) HOST Host Start Of Frame Control Offset */
#define USB_DEVICE_STATUS_REG_OFST     (0x0C)              /**< (USB_DEVICE_STATUS) DEVICE Status Offset */
#define USB_HOST_STATUS_REG_OFST       (0x0C)              /**< (USB_HOST_STATUS) HOST Status Offset */
#define USB_FSMSTATUS_REG_OFST         (0x0D)              /**< (USB_FSMSTATUS) Finite State Machine Status Offset */
#define USB_DEVICE_FNUM_REG_OFST       (0x10)              /**< (USB_DEVICE_FNUM) DEVICE Device Frame Number Offset */
#define USB_HOST_FNUM_REG_OFST         (0x10)              /**< (USB_HOST_FNUM) HOST Host Frame Number Offset */
#define USB_HOST_FLENHIGH_REG_OFST     (0x12)              /**< (USB_HOST_FLENHIGH) HOST Host Frame Length Offset */
#define USB_DEVICE_INTENCLR_REG_OFST   (0x14)              /**< (USB_DEVICE_INTENCLR) DEVICE Device Interrupt Enable Clear Offset */
#define USB_HOST_INTENCLR_REG_OFST     (0x14)              /**< (USB_HOST_INTENCLR) HOST Host Interrupt Enable Clear Offset */
#define USB_DEVICE_INTENSET_REG_OFST   (0x18)              /**< (USB_DEVICE_INTENSET) DEVICE Device Interrupt Enable Set Offset */
#define USB_HOST_INTENSET_REG_OFST     (0x18)              /**< (USB_HOST_INTENSET) HOST Host Interrupt Enable Set Offset */
#define USB_DEVICE_INTFLAG_REG_OFST    (0x1C)              /**< (USB_DEVICE_INTFLAG) DEVICE Device Interrupt Flag Offset */
#define USB_HOST_INTFLAG_REG_OFST      (0x1C)              /**< (USB_HOST_INTFLAG) HOST Host Interrupt Flag Offset */
#define USB_DEVICE_EPINTSMRY_REG_OFST  (0x20)              /**< (USB_DEVICE_EPINTSMRY) DEVICE End Point Interrupt Summary Offset */
#define USB_HOST_PINTSMRY_REG_OFST     (0x20)              /**< (USB_HOST_PINTSMRY) HOST Pipe Interrupt Summary Offset */
#define USB_DESCADD_REG_OFST           (0x24)              /**< (USB_DESCADD) Descriptor Address Offset */
#define USB_PADCAL_REG_OFST            (0x28)              /**< (USB_PADCAL) USB PAD Calibration Offset */

#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief DEVICE_DESC_BANK register API structure */
typedef struct
{
  __IO  uint32_t                       USB_ADDR;           /**< Offset: 0x00 (R/W  32) DEVICE_DESC_BANK Endpoint Bank, Adress of Data Buffer */
  __IO  uint32_t                       USB_PCKSIZE;        /**< Offset: 0x04 (R/W  32) DEVICE_DESC_BANK Endpoint Bank, Packet Size */
  __IO  uint16_t                       USB_EXTREG;         /**< Offset: 0x08 (R/W  16) DEVICE_DESC_BANK Endpoint Bank, Extended */
  __IO  uint8_t                        USB_STATUS_BK;      /**< Offset: 0x0A (R/W  8) DEVICE_DESC_BANK Enpoint Bank, Status of Bank */
  __I   uint8_t                        Reserved1[0x05];
} usb_device_desc_bank_registers_t;

/** \brief HOST_DESC_BANK register API structure */
typedef struct
{
  __IO  uint32_t                       USB_ADDR;           /**< Offset: 0x00 (R/W  32) HOST_DESC_BANK Host Bank, Adress of Data Buffer */
  __IO  uint32_t                       USB_PCKSIZE;        /**< Offset: 0x04 (R/W  32) HOST_DESC_BANK Host Bank, Packet Size */
  __IO  uint16_t                       USB_EXTREG;         /**< Offset: 0x08 (R/W  16) HOST_DESC_BANK Host Bank, Extended */
  __IO  uint8_t                        USB_STATUS_BK;      /**< Offset: 0x0A (R/W  8) HOST_DESC_BANK Host Bank, Status of Bank */
  __I   uint8_t                        Reserved1[0x01];
  __IO  uint16_t                       USB_CTRL_PIPE;      /**< Offset: 0x0C (R/W  16) HOST_DESC_BANK Host Bank, Host Control Pipe */
  __IO  uint16_t                       USB_STATUS_PIPE;    /**< Offset: 0x0E (R/W  16) HOST_DESC_BANK Host Bank, Host Status Pipe */
} usb_host_desc_bank_registers_t;

/** \brief DEVICE_ENDPOINT register API structure */
typedef struct
{
  __IO  uint8_t                        USB_EPCFG;          /**< Offset: 0x00 (R/W  8) DEVICE_ENDPOINT End Point Configuration */
  __I   uint8_t                        Reserved1[0x03];
  __O   uint8_t                        USB_EPSTATUSCLR;    /**< Offset: 0x04 ( /W  8) DEVICE_ENDPOINT End Point Pipe Status Clear */
  __O   uint8_t                        USB_EPSTATUSSET;    /**< Offset: 0x05 ( /W  8) DEVICE_ENDPOINT End Point Pipe Status Set */
  __I   uint8_t                        USB_EPSTATUS;       /**< Offset: 0x06 (R/   8) DEVICE_ENDPOINT End Point Pipe Status */
  __IO  uint8_t                        USB_EPINTFLAG;      /**< Offset: 0x07 (R/W  8) DEVICE_ENDPOINT End Point Interrupt Flag */
  __IO  uint8_t                        USB_EPINTENCLR;     /**< Offset: 0x08 (R/W  8) DEVICE_ENDPOINT End Point Interrupt Clear Flag */
  __IO  uint8_t                        USB_EPINTENSET;     /**< Offset: 0x09 (R/W  8) DEVICE_ENDPOINT End Point Interrupt Set Flag */
  __I   uint8_t                        Reserved2[0x16];
} usb_device_endpoint_registers_t;

/** \brief HOST_PIPE register API structure */
typedef struct
{
  __IO  uint8_t                        USB_PCFG;           /**< Offset: 0x00 (R/W  8) HOST_PIPE End Point Configuration */
  __I   uint8_t                        Reserved1[0x02];
  __IO  uint8_t                        USB_BINTERVAL;      /**< Offset: 0x03 (R/W  8) HOST_PIPE Bus Access Period of Pipe */
  __O   uint8_t                        USB_PSTATUSCLR;     /**< Offset: 0x04 ( /W  8) HOST_PIPE End Point Pipe Status Clear */
  __O   uint8_t                        USB_PSTATUSSET;     /**< Offset: 0x05 ( /W  8) HOST_PIPE End Point Pipe Status Set */
  __I   uint8_t                        USB_PSTATUS;        /**< Offset: 0x06 (R/   8) HOST_PIPE End Point Pipe Status */
  __IO  uint8_t                        USB_PINTFLAG;       /**< Offset: 0x07 (R/W  8) HOST_PIPE Pipe Interrupt Flag */
  __IO  uint8_t                        USB_PINTENCLR;      /**< Offset: 0x08 (R/W  8) HOST_PIPE Pipe Interrupt Flag Clear */
  __IO  uint8_t                        USB_PINTENSET;      /**< Offset: 0x09 (R/W  8) HOST_PIPE Pipe Interrupt Flag Set */
  __I   uint8_t                        Reserved2[0x16];
} usb_host_pipe_registers_t;

#define DEVICE_DESC_BANK_NUMBER _U_(2)

/** \brief USB_DESCRIPTOR register API structure */
typedef struct
{  /* Universal Serial Bus */
        usb_device_desc_bank_registers_t DEVICE_DESC_BANK[DEVICE_DESC_BANK_NUMBER]; /**< Offset: 0x00  */
} usb_descriptor_device_registers_t;

#define HOST_DESC_BANK_NUMBER _U_(2)

/** \brief USB_DESCRIPTOR register API structure */
typedef struct
{  /* Universal Serial Bus */
        usb_host_desc_bank_registers_t HOST_DESC_BANK[HOST_DESC_BANK_NUMBER]; /**< Offset: 0x00  */
} usb_descriptor_host_registers_t;

/** \brief USB_DESCRIPTOR hardware registers */
typedef union
{  /* Universal Serial Bus */
       usb_descriptor_device_registers_t DEVICE;         /**< USB is Device */
       usb_descriptor_host_registers_t HOST;           /**< USB is Host */
} usb_descriptor_registers_t;

#define DEVICE_ENDPOINT_NUMBER _U_(8)

/** \brief USB register API structure */
typedef struct
{  /* Universal Serial Bus */
  __IO  uint8_t                        USB_CTRLA;          /**< Offset: 0x00 (R/W  8) Control A */
  __I   uint8_t                        Reserved1[0x01];
  __I   uint8_t                        USB_SYNCBUSY;       /**< Offset: 0x02 (R/   8) Synchronization Busy */
  __IO  uint8_t                        USB_QOSCTRL;        /**< Offset: 0x03 (R/W  8) USB Quality Of Service */
  __I   uint8_t                        Reserved2[0x04];
  __IO  uint16_t                       USB_CTRLB;          /**< Offset: 0x08 (R/W  16) DEVICE Control B */
  __IO  uint8_t                        USB_DADD;           /**< Offset: 0x0A (R/W  8) DEVICE Device Address */
  __I   uint8_t                        Reserved3[0x01];
  __I   uint8_t                        USB_STATUS;         /**< Offset: 0x0C (R/   8) DEVICE Status */
  __I   uint8_t                        USB_FSMSTATUS;      /**< Offset: 0x0D (R/   8) Finite State Machine Status */
  __I   uint8_t                        Reserved4[0x02];
  __I   uint16_t                       USB_FNUM;           /**< Offset: 0x10 (R/   16) DEVICE Device Frame Number */
  __I   uint8_t                        Reserved5[0x02];
  __IO  uint16_t                       USB_INTENCLR;       /**< Offset: 0x14 (R/W  16) DEVICE Device Interrupt Enable Clear */
  __I   uint8_t                        Reserved6[0x02];
  __IO  uint16_t                       USB_INTENSET;       /**< Offset: 0x18 (R/W  16) DEVICE Device Interrupt Enable Set */
  __I   uint8_t                        Reserved7[0x02];
  __IO  uint16_t                       USB_INTFLAG;        /**< Offset: 0x1C (R/W  16) DEVICE Device Interrupt Flag */
  __I   uint8_t                        Reserved8[0x02];
  __I   uint16_t                       USB_EPINTSMRY;      /**< Offset: 0x20 (R/   16) DEVICE End Point Interrupt Summary */
  __I   uint8_t                        Reserved9[0x02];
  __IO  uint32_t                       USB_DESCADD;        /**< Offset: 0x24 (R/W  32) Descriptor Address */
  __IO  uint16_t                       USB_PADCAL;         /**< Offset: 0x28 (R/W  16) USB PAD Calibration */
  __I   uint8_t                        Reserved10[0xD6];
        usb_device_endpoint_registers_t DEVICE_ENDPOINT[DEVICE_ENDPOINT_NUMBER]; /**< Offset: 0x100  */
} usb_device_registers_t;

#define HOST_PIPE_NUMBER _U_(8)

/** \brief USB register API structure */
typedef struct
{  /* Universal Serial Bus */
  __IO  uint8_t                        USB_CTRLA;          /**< Offset: 0x00 (R/W  8) Control A */
  __I   uint8_t                        Reserved1[0x01];
  __I   uint8_t                        USB_SYNCBUSY;       /**< Offset: 0x02 (R/   8) Synchronization Busy */
  __IO  uint8_t                        USB_QOSCTRL;        /**< Offset: 0x03 (R/W  8) USB Quality Of Service */
  __I   uint8_t                        Reserved2[0x04];
  __IO  uint16_t                       USB_CTRLB;          /**< Offset: 0x08 (R/W  16) HOST Control B */
  __IO  uint8_t                        USB_HSOFC;          /**< Offset: 0x0A (R/W  8) HOST Host Start Of Frame Control */
  __I   uint8_t                        Reserved3[0x01];
  __IO  uint8_t                        USB_STATUS;         /**< Offset: 0x0C (R/W  8) HOST Status */
  __I   uint8_t                        USB_FSMSTATUS;      /**< Offset: 0x0D (R/   8) Finite State Machine Status */
  __I   uint8_t                        Reserved4[0x02];
  __IO  uint16_t                       USB_FNUM;           /**< Offset: 0x10 (R/W  16) HOST Host Frame Number */
  __I   uint8_t                        USB_FLENHIGH;       /**< Offset: 0x12 (R/   8) HOST Host Frame Length */
  __I   uint8_t                        Reserved5[0x01];
  __IO  uint16_t                       USB_INTENCLR;       /**< Offset: 0x14 (R/W  16) HOST Host Interrupt Enable Clear */
  __I   uint8_t                        Reserved6[0x02];
  __IO  uint16_t                       USB_INTENSET;       /**< Offset: 0x18 (R/W  16) HOST Host Interrupt Enable Set */
  __I   uint8_t                        Reserved7[0x02];
  __IO  uint16_t                       USB_INTFLAG;        /**< Offset: 0x1C (R/W  16) HOST Host Interrupt Flag */
  __I   uint8_t                        Reserved8[0x02];
  __I   uint16_t                       USB_PINTSMRY;       /**< Offset: 0x20 (R/   16) HOST Pipe Interrupt Summary */
  __I   uint8_t                        Reserved9[0x02];
  __IO  uint32_t                       USB_DESCADD;        /**< Offset: 0x24 (R/W  32) Descriptor Address */
  __IO  uint16_t                       USB_PADCAL;         /**< Offset: 0x28 (R/W  16) USB PAD Calibration */
  __I   uint8_t                        Reserved10[0xD6];
        usb_host_pipe_registers_t      HOST_PIPE[HOST_PIPE_NUMBER]; /**< Offset: 0x100  */
} usb_host_registers_t;

/** \brief USB hardware registers */
typedef union
{  /* Universal Serial Bus */
       usb_device_registers_t         DEVICE;         /**< USB is Device */
       usb_host_registers_t           HOST;           /**< USB is Host */
} usb_registers_t;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
/** \brief USB_DESCRIPTOR memory section attribute */
#define SECTION_USB_DESCRIPTOR

#endif /* _SAME54_USB_COMPONENT_H_ */
