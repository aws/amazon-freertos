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
 * BCM4390x pin mux related definitions and APIs
 */

#ifdef __cplusplus
extern "C"
{
#endif

platform_result_t platform_pinmux_init( platform_pin_t pin, platform_pin_function_t function );
platform_result_t platform_pinmux_deinit( platform_pin_t pin, platform_pin_function_t function );
platform_result_t platform_pinmux_get_function_config( platform_pin_t pin, const platform_pin_internal_config_t **pin_conf_pp, uint32_t *pin_function_index_p );
platform_result_t platform_pinmux_function_get( const platform_pin_internal_config_t *pin_conf, uint32_t *pin_function_index_ptr );
const platform_pin_internal_config_t *platform_pinmux_get_internal_config( platform_pin_t pin );
platform_result_t platform_pinmux_function_deinit( const platform_pin_internal_config_t *pin_conf, uint32_t pin_function_index );
platform_result_t platform_pinmux_function_init( const platform_pin_internal_config_t *pin_conf, uint32_t pin_function_index, platform_pin_config_t config );
platform_result_t platform_chipcommon_gpio_init( const platform_pin_internal_config_t *pin_conf, uint32_t pin_function_index, platform_pin_config_t config );
platform_result_t platform_chipcommon_gpio_deinit( const platform_pin_internal_config_t *pin_conf, uint32_t pin_function_index );

#ifdef __cplusplus
} /* extern "C" */
#endif
