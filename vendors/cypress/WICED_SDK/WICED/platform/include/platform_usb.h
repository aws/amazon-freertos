/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 * Declares platform USB Host/Device functions
 */
#pragma once

#include "wiced_result.h"

#include "platform_peripheral.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/
/* USB20 Host HCI (host controller interface) type */
typedef enum
{
    USB_HOST_CONTROLLER_INTERFACE_OHCI,         /* Generic 1.1 USB host controller */
    USB_HOST_CONTROLLER_INTERFACE_EHCI,         /* Generic 2.0 USB host controller */
    USB_HOST_CONTROLLER_INTERFACE_ISP1161,      /* NXP 1.1 embedded discrete host controller */
    USB_HOST_CONTROLLER_INTERFACE_ISP1362,      /* NXP 2.0 embedded discrete host controller */
    USB_HOST_CONTROLLER_INTERFACE_PIC32,        /* Microchip embedded 1.1 host controller */
    USB_HOST_CONTROLLER_INTERFACE_RX,           /* Renesas RX62 embedded host controller */
    USB_HOST_CONTROLLER_INTERFACE_RZ,           /* Renesas RZ embedded host controller */
    USB_HOST_CONTROLLER_INTERFACE_SH2A,         /* Renesas SH2A based embedded host controller */
    USB_HOST_CONTROLLER_INTERFACE_STM32,        /* ST Micro STM32 F1/F2/F4 embedded host controller */
    USB_HOST_CONTROLLER_INTERFACE_MUSB,         /* Mentor generic host controller */
    USB_HOST_CONTROLLER_INTERFACE_MAX
} platform_usb_host_controller_interface_t;

/* USB20 Device DCI (device controller interface) type */
typedef enum
{
    USB_DEVICE_CONTROLLER_INTERFACE_BRCM,       /* BRCM USB20 device controller */
    USB_DEVICE_CONTROLLER_INTERFACE_RZ,         /* Renesas RZ embedded device controller */
    USB_DEVICE_CONTROLLER_INTERFACE_SH2A,       /* Renesas SH2A based embedded device controller */
    USB_DEVICE_CONTROLLER_INTERFACE_MUSB,       /* Mentor generic device controller */
    USB_DEVICE_CONTROLLER_INTERFACE_MAX
} platform_usb_device_controller_interface_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/
/* USB20 Host interrupt handler */
typedef void (*platform_usb_host_irq_handler_t)( void );

/* USB20 Device interrupt handler */
typedef void (*platform_usb_device_irq_handler_t)( void );

/******************************************************
 *                    Structures
 ******************************************************/
/* USB20 Host HCI (host controller interface) resource */
typedef struct
{
    platform_usb_host_controller_interface_t    usb_host_hci_type;
    uint32_t                                    usb_host_hci_ioaddress;
    uint32_t                                    usb_host_hci_irq_number;
} platform_usb_host_hci_resource_t;

/* USB20 Device DCI (device controller interface) resource */
typedef struct
{
    platform_usb_device_controller_interface_t  usb_device_dci_type;
    uint32_t                                    usb_device_dci_ioaddress;
    uint32_t                                    usb_device_dci_irq_number;
    void*                                       usb_device_dci_private_data;
} platform_usb_device_dci_resource_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
/* Board USB20 mode check */
wiced_bool_t        platform_is_board_in_usb_phy_mode( void );
wiced_bool_t        platform_is_board_in_usb_host_mode( void );


/* USB20 Host init/deinit */
platform_result_t   platform_usb_host_init( void );
platform_result_t   platform_usb_host_post_init( void );
void                platform_usb_host_deinit( void );

/* USB20 Host init IRQ */
platform_result_t   platform_usb_host_init_irq( platform_usb_host_irq_handler_t irq_handler );

platform_result_t   platform_usb_host_enable_irq( void );
platform_result_t   platform_usb_host_disable_irq( void );

/* USB20 Host get HCI (host controller interface) resource */
platform_result_t   platform_usb_host_get_hci_resource( platform_usb_host_hci_resource_t *resource_list_buf, uint32_t buf_size, uint32_t *resource_total_num );


/* USB20 Device init/deinit */
platform_result_t   platform_usb_device_init( void );
void                platform_usb_device_deinit( void );

/* USB20 Device init IRQ */
platform_result_t   platform_usb_device_init_irq( platform_usb_device_irq_handler_t irq_handler );

platform_result_t   platform_usb_device_enable_irq( void );
platform_result_t   platform_usb_device_disable_irq( void );

/* USB20 Device get DCI (device controller interface) resource */
platform_result_t   platform_usb_device_get_dci_resource( platform_usb_device_dci_resource_t *resource_list_buf, uint32_t buf_size, uint32_t *resource_total_num );



#ifdef __cplusplus
} /*extern "C" */
#endif
