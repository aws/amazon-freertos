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
 *
 */

#include "wiced_time.h"
#include "wiced_utilities.h"
#include "string.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define UTC_TIME_TO_TIME( utc_time ) ( utc_time / 1000 )
#define BASE_UTC_YEAR (1970)
#define IS_LEAP_YEAR( year ) ( ( ( year ) % 400 == 0 ) || \
                             ( ( ( year ) % 100 != 0 ) && ( ( year ) % 4 == 0 ) ) )

/******************************************************
 *                    Constants
 ******************************************************/

#define SECONDS_IN_365_DAY_YEAR  (31536000)
#define SECONDS_IN_A_DAY         (86400)
#define SECONDS_IN_A_HOUR        (3600)
#define SECONDS_IN_A_MINUTE      (60)
static const uint32_t secondsPerMonth[ 12 ] =
{
    31*SECONDS_IN_A_DAY,
    28*SECONDS_IN_A_DAY,
    31*SECONDS_IN_A_DAY,
    30*SECONDS_IN_A_DAY,
    31*SECONDS_IN_A_DAY,
    30*SECONDS_IN_A_DAY,
    31*SECONDS_IN_A_DAY,
    31*SECONDS_IN_A_DAY,
    30*SECONDS_IN_A_DAY,
    31*SECONDS_IN_A_DAY,
    30*SECONDS_IN_A_DAY,
    31*SECONDS_IN_A_DAY,
};

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

static wiced_utc_time_ms_t current_utc_time = 0;
static wiced_time_t        last_utc_time_wiced_reference = 0;

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t wiced_time_get_utc_time( wiced_utc_time_t* utc_time )
{
    wiced_utc_time_ms_t utc_time_ms;

    wiced_time_get_utc_time_ms( &utc_time_ms );

    *utc_time = (uint32_t)( utc_time_ms / 1000 );
    return WICED_SUCCESS;
}

wiced_result_t wiced_time_get_utc_time_ms( wiced_utc_time_ms_t* utc_time_ms )
{
    wiced_time_t temp_wiced_time;
    uint32_t     time_since_last_reference;

    /* Update the UTC time by the time difference between now and the last update */
    wiced_time_get_time( &temp_wiced_time );
    time_since_last_reference = ( temp_wiced_time - last_utc_time_wiced_reference );

    if ( time_since_last_reference != 0 )
    {
        current_utc_time += time_since_last_reference;
        last_utc_time_wiced_reference = temp_wiced_time;
    }

    *utc_time_ms = current_utc_time;
    return WICED_SUCCESS;
}

wiced_result_t wiced_time_set_utc_time_ms( const wiced_utc_time_ms_t* utc_time_ms )
{
    wiced_time_get_time( &last_utc_time_wiced_reference );
    current_utc_time = *utc_time_ms;
    return WICED_SUCCESS;
}


wiced_result_t wiced_time_get_iso8601_time(wiced_iso8601_time_t* iso8601_time)
{
    wiced_utc_time_ms_t utc_time_ms;

    wiced_time_get_utc_time_ms( &utc_time_ms );
    return wiced_time_convert_utc_ms_to_iso8601( utc_time_ms, iso8601_time );
}

wiced_result_t wiced_time_convert_utc_ms_to_iso8601( wiced_utc_time_ms_t utc_time_ms, wiced_iso8601_time_t* iso8601_time )
{
    uint32_t            a;
    uint16_t            year;
    uint16_t            number_of_leap_years;
    uint8_t             month;
    uint8_t             day;
    uint8_t             hour;
    uint8_t             minute;
    uint64_t            second;
    uint32_t            sub_second;
    wiced_bool_t        is_a_leap_year;

    second     = utc_time_ms / 1000;               /* Time is in milliseconds. Convert to seconds */
    sub_second = (uint32_t) ( ( utc_time_ms % 1000 ) * 1000 ); /* Sub-second is in microseconds               */

    /* Calculate year */
    year = (uint16_t)( BASE_UTC_YEAR + second / SECONDS_IN_365_DAY_YEAR );
    number_of_leap_years = ( uint16_t )( ( ( year - ( BASE_UTC_YEAR - ( BASE_UTC_YEAR % 4 ) + 1 ) ) / 4 ) -
                           ( ( year - ( BASE_UTC_YEAR - ( BASE_UTC_YEAR % 100 ) + 1 ) ) / 100 ) +
                           ( ( year - ( BASE_UTC_YEAR - ( BASE_UTC_YEAR % 400 ) + 1 ) ) / 400 ) );
    second -= (uint64_t)( (uint64_t)( year - BASE_UTC_YEAR ) * SECONDS_IN_365_DAY_YEAR );

    if ( second >= ( uint64_t )( number_of_leap_years * SECONDS_IN_A_DAY ) )
    {
        second -= (uint64_t) ( ( number_of_leap_years * SECONDS_IN_A_DAY ) );
    }
    else
    {
        do
        {
            second += SECONDS_IN_365_DAY_YEAR;
            year--;
            if ( IS_LEAP_YEAR( year ) )
            {
                second += SECONDS_IN_A_DAY;
            }
        } while ( second < ( uint64_t )( number_of_leap_years * SECONDS_IN_A_DAY ) );

        second -= ( uint64_t )( number_of_leap_years * SECONDS_IN_A_DAY );
    }

    /* Remember if the current year is a leap year */
    is_a_leap_year = ( IS_LEAP_YEAR( year ) ) ? WICED_TRUE : WICED_FALSE;

    /* Calculate month */
    month = 1;

    for ( a = 0; a < 12; ++a )
    {
        uint32_t seconds_per_month = secondsPerMonth[a];
        /* Compensate for leap year */
        if ( ( a == 1 ) && is_a_leap_year )
        {
            seconds_per_month += SECONDS_IN_A_DAY;
        }
        if ( second >= seconds_per_month )
        {
            second -= seconds_per_month;
            month++;
        }
        else
        {
            break;
        }
    }

    /* Calculate day */
    day    = (uint8_t) ( second / SECONDS_IN_A_DAY );
    second -= (uint64_t) ( day * SECONDS_IN_A_DAY );
    ++day;

    /* Calculate hour */
    hour   = (uint8_t) ( second / SECONDS_IN_A_HOUR );
    second -= (uint64_t)  ( hour * SECONDS_IN_A_HOUR );

    /* Calculate minute */
    minute = (uint8_t) ( second / SECONDS_IN_A_MINUTE );
    second -= (uint64_t) ( minute * SECONDS_IN_A_MINUTE );

    /* Write iso8601 time (Note terminating nulls get overwritten intentionally) */
    unsigned_to_decimal_string( year,             iso8601_time->year,       4, 4 );
    unsigned_to_decimal_string( month,            iso8601_time->month,      2, 2 );
    unsigned_to_decimal_string( day,              iso8601_time->day,        2, 2 );
    unsigned_to_decimal_string( hour,             iso8601_time->hour,       2, 2 );
    unsigned_to_decimal_string( minute,           iso8601_time->minute,     2, 2 );
    unsigned_to_decimal_string( (uint8_t)second,  iso8601_time->second,     2, 2 );
    unsigned_to_decimal_string( sub_second,       iso8601_time->sub_second, 6, 6 );

    iso8601_time->T          = 'T';
    iso8601_time->Z          = 'Z';
    iso8601_time->colon1     = ':';
    iso8601_time->colon2     = ':';
    iso8601_time->dash1      = '-';
    iso8601_time->dash2      = '-';
    iso8601_time->decimal    = '.';

    return WICED_SUCCESS;
}
