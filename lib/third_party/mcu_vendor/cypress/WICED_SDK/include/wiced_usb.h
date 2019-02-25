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
#pragma once

#include "wiced_result.h"
#include "wwd_assert.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
/* USB Host Event */
#define USB_HOST_EVENT_UNDEFINED                    0
#define USB_HOST_EVENT_HID_DEVICE_INSERTION         1
#define USB_HOST_EVENT_HID_DEVICE_REMOVAL           2
#define USB_HOST_EVENT_STORAGE_DEVICE_INSERTION     11
#define USB_HOST_EVENT_STORAGE_DEVICE_REMOVAL       12
#define USB_HOST_EVENT_AUDIO_DEVICE_INSERTION       21
#define USB_HOST_EVENT_AUDIO_DEVICE_REMOVAL         22
#define USB_HOST_EVENT_CDC_ACM_DEVICE_INSERTION     31
#define USB_HOST_EVENT_CDC_ACM_DEVICE_REMOVAL       32
#define USB_HOST_EVENT_PROLIFIC_DEVICE_INSERTION    41
#define USB_HOST_EVENT_PROLIFIC_DEVICE_REMOVAL      42

/* USB Disk storage name for filesystem mount */
#define USB_DISK_STORAGE_MOUNT_NAME                 "usb0"

/* USB maximum number of audio device handle */
#define USB_AUDIO_DEVICE_HANDLE_MAX                 5

/* USB maximum number of cdc-acm device handle */
#define USB_CDC_ACM_DEVICE_HANDLE_MAX               20

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/
/* USB Host Event Callback function */
typedef wiced_result_t (*wiced_usb_host_event_callback_t)( uint32_t evt, void *param1, void *param2 );

/******************************************************
 *                    Structures
 ******************************************************/
/* USB User Config */
typedef struct
{
    uint32_t                        host_max_class;
    uint32_t                        host_max_hcd;
    uint32_t                        host_max_devices;
    uint32_t                        host_max_ed;
    uint32_t                        host_max_td;
    uint32_t                        host_max_iso_td;
    wiced_usb_host_event_callback_t host_event_callback;
    uint32_t                        host_thread_stack_size;
} wiced_usb_user_config_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

/** Initialises USB Host stack
 *
 * @param[in] config : Pointer to a USB config structure.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_usb_host_init( wiced_usb_user_config_t *config );


/** De-initialises USB Host stack
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_usb_host_deinit( void );



#ifdef __cplusplus
} /*extern "C" */
#endif
