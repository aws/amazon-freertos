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

#include "wwd_constants.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    WWD_RESOURCE_WLAN_FIRMWARE,
    WWD_RESOURCE_WLAN_NVRAM,
} wwd_resource_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

#if defined( WWD_DYNAMIC_NVRAM )
extern uint32_t dynamic_nvram_size;
extern void*    dynamic_nvram_image;
#endif

/******************************************************
 *               Function Declarations
 ******************************************************/

extern wwd_result_t host_platform_resource_size( wwd_resource_t resource, uint32_t* size_out );

/*
 * This function is used when WWD_DIRECT_RESOURCES is defined
 */
extern wwd_result_t host_platform_resource_read_direct( wwd_resource_t resource, const void** ptr_out );

/*
 * This function is used when WWD_DIRECT_RESOURCES is not defined
 */
extern wwd_result_t host_platform_resource_read_indirect( wwd_resource_t resource, uint32_t offset, void* buffer, uint32_t buffer_size, uint32_t* size_out );

#ifdef __cplusplus
} /* extern "C" */
#endif
