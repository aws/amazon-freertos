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


/*! \file mdev_usb_host_port.c
 *   Stubs for USB Host
 */


#include <mdev_usb_host_port.h>

void usb_uvc_print_descriptors()
{
        /* Provide USB-Stack specific implementation here  */
}
void usb_uac_print_descriptors()
{
        /* Provide USB-Stack specific implementation here  */
}
void usb_msc_print_descriptors()
{
        /* Provide USB-Stack specific implementation here  */
}
void usb_cdc_print_descriptors()
{
        /* Provide USB-Stack specific implementation here  */
}
int usb_cdc_drv_init(void *data)
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_cdc_drv_read(uint8_t * buf, uint32_t count)
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_cdc_drv_write(uint8_t * buf, uint32_t count)
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_cdc_drv_start(void)
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_cdc_drv_stop(void)
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_uvc_drv_init(void *data)
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_uvc_drv_read(uint8_t * buf, uint32_t count)
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_uvc_drv_start()
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_uvc_drv_stop()
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_uac_drv_init(void *data)
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_uac_drv_read(uint8_t * buf, uint32_t count)
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_msc_drv_init(void *data)
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_msc_drv_read(uint8_t * buf, uint32_t sector, uint32_t sectors)
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_msc_drv_write(uint8_t * buf, uint32_t sector, uint32_t sectors)
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}
int usb_msc_drv_info(uint8_t * buf)
{
        /* Provide USB-Stack specific implementation here  */
        return 0;
}

void USB_HOST_ShutDown()
{
        /* Provide USB-Stack specific implementation here  */
}
void USB_HOST_HWInit()
{
        /* Provide USB-Stack specific implementation here  */
}

void USB_HOST_RegisterEHCI(void * MemBase, unsigned char IRQ)
{
        /* Provide USB-Stack specific implementation here  */
}

void USBAudio(void *pvt)
{
        /* Provide USB-Stack specific implementation here  */
}
void USBVideo(void *pvt)
{
        /* Provide USB-Stack specific implementation here  */
}
void USBComm(void *pvt)
{
        /* Provide USB-Stack specific implementation here  */
}
void USBDisk(void *pvt)
{
        /* Provide USB-Stack specific implementation here  */
}

void USB_HOST_RegisterCallback(USB_HOST_CALLBACK Callback)
{
        /* Provide USB-Stack specific implementation here  */
}

