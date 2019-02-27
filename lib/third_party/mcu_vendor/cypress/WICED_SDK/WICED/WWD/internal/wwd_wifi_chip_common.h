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
 *  Prototypes of functions for controlling the Wi-Fi system chip specific features
 */

#ifndef INCLUDED_WWD_WIFI_CHIP_COMMON_H
#define INCLUDED_WWD_WIFI_CHIP_COMMON_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *  Function prototypes
 *
 */

/** Gets the current value of all beacon listen interval variables
 *
 * @param listen_interval_beacon : The current value of the listen interval set as a multiple of the beacon period
 * @param listen_interval_dtim   : The current value of the listen interval set as a multiple of the DTIM period
 * @param listen_interval_assoc  : The current value of the listen interval sent to access points in an association request frame
 *
 * @return  WWD_SUCCESS : If all listen interval values are read successfully
 *          Error code   : If at least one of the listen interval values are NOT read successfully
 */
extern wwd_result_t wwd_wlan_bus_complete_ds_wake( wiced_bool_t wake_from_firmware, uint32_t wake_event_indication_addr, uint32_t wake_indication_addr, uint32_t sdio_control_addr );

/** Gets the current value of a WLAN register
 *
 * @param addr   : absolute address to read
 * @param size    : size of read to do
 * @param value  : value of successful read
 *
 * @return  WWD_SUCCESS : if read is successful
 *          else, assert fires and returns WWD_TIMEOUT
 */
extern wwd_result_t wwd_wlan_register_read( uint32_t addr, uint32_t size, uint32_t *value );

/** Writes a value to a WLAN register
 *
 * @param addr   : absolute address to write
 * @param value  : value to write
 * @param size    : size of write
 *
 * @return  WWD_SUCCESS : if write is successful
 *          else, assert fires and returns WWD_TIMEOUT
 */
extern wwd_result_t wwd_wlan_register_write( uint32_t addr, uint32_t size, uint32_t value );

/** Gets the current value of a WLAN shared memory location
 *
 * @param addr   : absolute address to read
 * @param size    : size of read to do
 * @param value  : value of successful read
 *
 * @return  WWD_SUCCESS : if read is successful
 *          else, assert fires and returns WWD_TIMEOUT
 */
extern wwd_result_t wwd_wlan_shared_memory_read( uint32_t addr, uint8_t size, uint32_t *value );

/**
 *  Definitions
 */
#define WWD_WLAN_WAKE_TIMEOUT           10000
#define WWD_SHARED_MEMORY_POLLING_DELAY 10

#define WWD_VERIFY_WLAN_ACCESS( expr ) \
    do \
    { \
        wwd_result_t result = ( expr ); \
        if ( WWD_SUCCESS != result )    \
        {                               \
            WPRINT_WWD_DEBUG(("Failed wlan mem access: result=%d\n", (int)result)); \
            return result;           \
        }                            \
    } \
    while ( 0 )

#define WWD_WLAN_SHARED_MEMORY_READ( addr, size, value ) WWD_VERIFY_WLAN_ACCESS( wwd_wlan_shared_memory_read( ( addr ), ( size ), ( value ) ) )
#define WWD_WLAN_REGISTER_READ( addr, size, value ) WWD_VERIFY_WLAN_ACCESS( wwd_wlan_register_read( ( addr ), ( size ), ( value ) ) )
#define WWD_WLAN_REGISTER_WRITE( addr, size, value ) WWD_VERIFY_WLAN_ACCESS( wwd_wlan_register_write( ( addr ), ( size ), ( value ) ) )

#define WWD_WLAN_WAKE_FROM_HOST( ) \
        do { \
                uint32_t val32; \
                \
                WWD_WLAN_REGISTER_READ( D11_MACCONTROL_REG, D11_MACCONTROL_REG_SIZE, &val32 ); \
                WPRINT_WWD_DEBUG(("%s: %d:before: maccontrol: 0x%08x\n", __FUNCTION__, __LINE__, (unsigned int)val32 )); \
                \
                val32 = val32 | D11_MACCONTROL_REG_WAKE; \
                WWD_WLAN_REGISTER_WRITE( D11_MACCONTROL_REG, D11_MACCONTROL_REG_SIZE, val32 ); \
                WWD_WLAN_REGISTER_READ( D11_MACCONTROL_REG, D11_MACCONTROL_REG_SIZE, &val32 ); \
                WPRINT_WWD_DEBUG(( "%s: %d:after: maccontrol: 0x%08x\n", __FUNCTION__, __LINE__, (unsigned int)val32 )); \
        } while (0);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* INCLUDED_WWD_WIFI_CHIP_COMMON_H */
