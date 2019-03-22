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
 *  Defines functions to set and get the current time
 */

#pragma once

#include "wiced_result.h"
#include "wiced_utilities.h"
#include "RTOS/wwd_rtos_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                    Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/** @cond !ADDTHIS*/
#define MILLISECONDS      (1)
#define SECONDS           (1000)
#define MINUTES           (60 * SECONDS)
#define HOURS             (60 * MINUTES)
#define DAYS              (24 * HOURS)
/** @endcond */

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/


typedef uint32_t  wiced_time_t;        /**< Time value in milliseconds */
typedef uint32_t  wiced_utc_time_t;    /**< UTC Time in seconds        */
typedef uint64_t  wiced_utc_time_ms_t; /**< UTC Time in milliseconds   */

/******************************************************
 *                    Structures
 ******************************************************/

/** ISO8601 Time Structure
 */
#pragma pack(1)

typedef struct
{
    char year[4];        /**< Year         */
    char dash1;          /**< Dash1        */
    char month[2];       /**< Month        */
    char dash2;          /**< Dash2        */
    char day[2];         /**< Day          */
    char T;              /**< T            */
    char hour[2];        /**< Hour         */
    char colon1;         /**< Colon1       */
    char minute[2];      /**< Minute       */
    char colon2;         /**< Colon2       */
    char second[2];      /**< Second       */
    char decimal;        /**< Decimal      */
    char sub_second[6];  /**< Sub-second   */
    char Z;              /**< UTC timezone */
} wiced_iso8601_time_t;

#pragma pack()

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/*****************************************************************************/
/** @addtogroup time       Time management functions
 *
 * Functions to get and set the system time.
 *  System time is derived from timer ticks implemented
 *  by the underlying operating system.
 *
 *      wiced_time_get_time
 *      wiced_time_set_time
 *      wiced_time_get_utc_time
 *      wiced_time_get_utc_time_ms
 *      wiced_time_set_utc_time_ms
 *      wiced_time_get_iso8601_time
 *      wiced_time_convert_utc_ms_to_iso8601
 *
 * Nano-Second precision timer related functions
 *
 *      wiced_init_nanosecond_clock
 *      wiced_get_nanosecond_clock_value
 *      wiced_reset_nanosecond_clock
 *      wiced_deinit_nanosecond_clock
 *
 *  @{
 */
/*****************************************************************************/


/** Get the current system tick time in milliseconds
 *
 * @note The time will roll over every 49.7 days
 *
 * @param[out] time : A pointer to the variable which will receive the time value
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_time_get_time( wiced_time_t* time );


/** Set the current system tick time in milliseconds
 *
 * @param[in] time : The time value to set
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_time_set_time( const wiced_time_t* time );


/** Get the current UTC time in seconds
 *
 * This will only be accurate if the time has previously been set by using @ref wiced_time_set_utc_time_ms
 *
 * @param[out] utc_time : A pointer to the variable which will receive the time value
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_time_get_utc_time( wiced_utc_time_t* utc_time );


/** Get the current UTC time in milliseconds
 *
 * This will only be accurate if the time has previously been set by using @ref wiced_time_set_utc_time_ms
 *
 * @param[out] utc_time_ms : A pointer to the variable which will receive the time value
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_time_get_utc_time_ms( wiced_utc_time_ms_t* utc_time_ms );


/** Set the current UTC time in milliseconds
 *
 * @param[in] utc_time_ms : The time value to set
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_time_set_utc_time_ms( const wiced_utc_time_ms_t* utc_time_ms );


/** Get the current UTC time in iso 8601 format e.g. "2012-07-02T17:12:34.567890Z"
 *
 * @note The time will roll over every 49.7 days
 *
 * @param[out] iso8601_time : A pointer to the structure variable that
 *                            will receive the time value
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_time_get_iso8601_time( wiced_iso8601_time_t* iso8601_time );


/** Convert a time from UTC milliseconds to iso 8601 format e.g. "2012-07-02T17:12:34.567890Z"
 *
 * @param[in] utc_time_ms   : The time value to convert
 * @param[out] iso8601_time : A pointer to the structure variable that
 *                            will receive the time value
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_time_convert_utc_ms_to_iso8601( wiced_utc_time_ms_t utc_time_ms, wiced_iso8601_time_t* iso8601_time );

/** @} */

/**
 * This function will return the value of time read from the nanosecond clock.
 *
 * @return : number of nanoseconds passed since the function wiced_init_nanosecond_clock or wiced_reset_nanosecond_clock was called
 */
uint64_t wiced_get_nanosecond_clock_value( void );


/**
 * This function will deinitialize the nanosecond clock.
 */
void wiced_deinit_nanosecond_clock( void );


/**
 * This function will reset the nanosecond clock.
 */
void wiced_reset_nanosecond_clock( void );


/**
 * This function will initialize the nanosecond clock.
*/
void wiced_init_nanosecond_clock( void );

#ifdef __cplusplus
} /*extern "C" */
#endif
