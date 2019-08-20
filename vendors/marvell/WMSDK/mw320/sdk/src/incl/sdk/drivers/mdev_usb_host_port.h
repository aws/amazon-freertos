/*
*  (C) Copyright 2008-2019 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/


/*! \file mdev_usb_host_port.h
 *   
 */

#ifndef _USB_STACK_SPECIFIC_FN_
#define _USB_STACK_SPECIFIC_FN_ 

#include <mdev.h>

typedef void (*USB_HOST_CALLBACK)(void *);
void USB_HOST_RegisterCallback(USB_HOST_CALLBACK Callback);
void usb_uvc_print_descriptors();
void USB_HOST_ShutDown();
void USB_HOST_HWInit();
void USB_HOST_RegisterEHCI(void * MemBase, unsigned char IRQ);
void usb_uac_print_descriptors();
void USBAudio(void *pvt);
void USBVideo(void *pvt);
void USBComm(void *pvt);
void USBDisk(void *pvt);
void usb_msc_print_descriptors();
void usb_cdc_print_descriptors();
int usb_cdc_drv_init(void *data);
int usb_cdc_drv_read(uint8_t * buf, uint32_t count);
int usb_cdc_drv_write(uint8_t * buf, uint32_t count);
int usb_cdc_drv_start(void);
int usb_cdc_drv_stop(void);
int usb_uvc_drv_init(void *data);
int usb_uvc_drv_read(uint8_t * buf, uint32_t count);
int usb_uvc_drv_start();
int usb_uvc_drv_stop();
int usb_uac_drv_init(void *data);
int usb_uac_drv_read(uint8_t * buf, uint32_t count);
int usb_msc_drv_init(void *data);
int usb_msc_drv_read(uint8_t * buf, uint32_t sector, uint32_t sectors);
int usb_msc_drv_write(uint8_t * buf, uint32_t sector, uint32_t sectors);
int usb_msc_drv_info(uint8_t * buf);
#endif
