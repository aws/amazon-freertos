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
 *  BCM43909 Platform RTC Driver
 */

#include <stdint.h>
#include "typedefs.h"

#include "osl.h"
#include "hndsoc.h"

#include "platform_peripheral.h"
#include "platform_appscr4.h"
#include "platform_toolchain.h"

#include "wwd_assert.h"
#include "wwd_rtos.h"

/******************************************************
 *                      Macros
 ******************************************************/

/*
 * RTC registers are latched on the always-on ILP domain, and that is slow.
 * Since we do register shadowing from backplane domain to 32kHz ILP domain,
 * we need to make sure RTC writes are latched correctly after a 32kHz cycle.
 * This can be achieved by polling RTCWriteInProgress bit of the PMUStatus
 * register until the current RTC register write transaction has completed.
 */
#define RTC_REG_WRITE_SYNC(rtc_register, value)                                         \
    do                                                                                  \
    {                                                                                   \
        rtc_register = value;                                                           \
        while ( (PLATFORM_PMU->pmustatus & PMU_STATUS_RTC_WRITE_IN_PROGRESS) != 0 );    \
    }                                                                                   \
    while( 0 )

/*
 * NOTE:
 * To ensure the coherent observation and setting of time and calendar,
 * both counters are stopped whenever the values are read or written.
 * As only one increment is remembered while the counters are stopped,
 * the duration of stoppage should be limited to be less than 1 second.
 */

/* Stop the RTC time and calendar */
#define RTC_STOP_TIME_CALENDAR                                                          \
    do                                                                                  \
    {                                                                                   \
        rtc_control_t rtc_control;                                                      \
        rtc_control.raw = PLATFORM_PMU->real_time_clock.control.raw;                    \
        rtc_control.bits.time = 1;                                                      \
        rtc_control.bits.calendar = 1;                                                  \
        RTC_REG_WRITE_SYNC(PLATFORM_PMU->real_time_clock.control.raw, rtc_control.raw); \
    }                                                                                   \
    while( 0 )

/* Start the RTC time and calendar */
#define RTC_START_TIME_CALENDAR                                                         \
    do                                                                                  \
    {                                                                                   \
        rtc_control_t rtc_control;                                                      \
        rtc_control.raw = PLATFORM_PMU->real_time_clock.control.raw;                    \
        rtc_control.bits.time = 0;                                                      \
        rtc_control.bits.calendar = 0;                                                  \
        RTC_REG_WRITE_SYNC(PLATFORM_PMU->real_time_clock.control.raw, rtc_control.raw); \
    }                                                                                   \
    while( 0 )

/* Check to identify a leap year */
#define IS_LEAP_YEAR(year) ( (((((year) % 4) == 0) && (((year) % 100) != 0)) || (((year) % 400) == 0)) ? (1) : (0) )

/* BCD units and tens digit representation */
#define NUM_TO_BCD_UNITS(num)               ( num % 10 )
#define NUM_TO_BCD_TENS(num)                ( (num / 10) % 10 )
#define BCD_UNITS_TO_NUM(units)             ( units )
#define BCD_TENS_TO_NUM(tens)               ( tens * 10 )
#define BCD_UNITS_TENS_TO_NUM(units, tens)  ( BCD_TENS_TO_NUM(tens) + BCD_UNITS_TO_NUM(units) )

/*
 * NOTE:
 * The 43909 RTC block uses a 1 Hz counter clock, which means a time counter accuracy of 1 second.
 * So hundredth of a second fields are not applicable. The reason is that we only have an accurate
 * 32.768kHz clock source, so it's not possible to generate an accurate 100Hz reference from that.
 */
#define NUM_SECONDS_IN_MINUTE       (60)
#define NUM_MINUTES_IN_HOUR         (60)
#define NUM_HOURS_IN_DAY            (24)
#define NUM_DAYS_IN_WEEK            (7)
#define NUM_MONTHS_IN_YEAR          (12)
#define NUM_YEARS_IN_CENTURY        (100)

#define RTC_CURRENT_CENTURY         (20)

/******************************************************
 *                    Constants
 ******************************************************/

/* PMU Core Capabilities Extension register bits */
#define PMU_CAP_EXT_RTC_PRESENT             (1 << 16)

/* PMU Status register bits */
#define PMU_STATUS_RTC_WRITE_IN_PROGRESS    (1 << 15)

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
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/* Default RTC time. Set to 00:00:00 01/01/2000 Saturday */
static const platform_rtc_time_t default_rtc_time =
{
   .sec     = 00,
   .min     = 00,
   .hr      = 00,
   .weekday = 7,
   .date    = 1,
   .month   = 1,
   .year    = 00,
};

static const char month_days_not_leap_year[] =
{
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const char month_days_in_leap_year[] =
{
    0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

/******************************************************
 *               Function Definitions
 ******************************************************/

static wiced_bool_t validate_input_time_calendar( const platform_rtc_time_t* time )
{
    wiced_bool_t valid_time_calendar = WICED_FALSE;

    if ( time->sec < NUM_SECONDS_IN_MINUTE )
    {
        if ( time->min < NUM_MINUTES_IN_HOUR )
        {
            if ( time->hr < NUM_HOURS_IN_DAY )
            {
                if ( (time->weekday > 0) && (time->weekday <= NUM_DAYS_IN_WEEK) )
                {
                    if ( (time->month > 0) && (time->month <= NUM_MONTHS_IN_YEAR) )
                    {
                        if ( time->year < NUM_YEARS_IN_CENTURY )
                        {
                            if ( IS_LEAP_YEAR((RTC_CURRENT_CENTURY * 100) + time->year) == 1 )
                            {
                                if ( (time->date > 0) && (time->date <= month_days_in_leap_year[time->month]) )
                                {
                                    valid_time_calendar = WICED_TRUE;
                                }
                            }
                            else
                            {
                                if ( (time->date > 0) && (time->date <= month_days_not_leap_year[time->month]) )
                                {
                                    valid_time_calendar = WICED_TRUE;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return valid_time_calendar;
}

static wiced_bool_t platform_rtc_valid_time_calendar( const platform_rtc_time_t* time )
{
    rtc_status_t rtc_status;

    rtc_status.raw = PLATFORM_PMU->real_time_clock.status.raw;

    return ( ((rtc_status.bits.time_valid == 1) && (rtc_status.bits.calendar_valid == 1)) ? WICED_TRUE : WICED_FALSE );
}

static void platform_rtc_read_time( platform_rtc_time_t* time )
{
    rtc_time_t rtc_time;

    /* Read the time value from RTC */
    rtc_time.raw = PLATFORM_PMU->real_time_clock.time.raw;

    time->sec = BCD_UNITS_TENS_TO_NUM(rtc_time.bits.seconds_units, rtc_time.bits.seconds_tens);
    time->min = BCD_UNITS_TENS_TO_NUM(rtc_time.bits.minutes_units, rtc_time.bits.minutes_tens);
    time->hr  = BCD_UNITS_TENS_TO_NUM(rtc_time.bits.hours_units, rtc_time.bits.hours_tens);
}

static void platform_rtc_read_calendar( platform_rtc_time_t* time )
{
    rtc_calendar_t rtc_calendar;

    /* Read the calendar value from RTC */
    rtc_calendar.raw = PLATFORM_PMU->real_time_clock.calendar.raw;

    time->weekday = BCD_UNITS_TO_NUM(rtc_calendar.bits.day_of_week);
    time->date    = BCD_UNITS_TENS_TO_NUM(rtc_calendar.bits.date_units, rtc_calendar.bits.date_tens);
    time->month   = BCD_UNITS_TENS_TO_NUM(rtc_calendar.bits.month_units, rtc_calendar.bits.month_tens);
    time->year    = BCD_UNITS_TENS_TO_NUM(rtc_calendar.bits.year_units, rtc_calendar.bits.year_tens);
}

static void platform_rtc_write_time( const platform_rtc_time_t* time )
{
    rtc_time_t rtc_time;

    rtc_time.bits.seconds_units = NUM_TO_BCD_UNITS(time->sec);
    rtc_time.bits.seconds_tens  = NUM_TO_BCD_TENS(time->sec);
    rtc_time.bits.minutes_units = NUM_TO_BCD_UNITS(time->min);
    rtc_time.bits.minutes_tens  = NUM_TO_BCD_TENS(time->min);
    rtc_time.bits.hours_units   = NUM_TO_BCD_UNITS(time->hr);
    rtc_time.bits.hours_tens    = NUM_TO_BCD_TENS(time->hr);

    /* Write the time value to RTC */
    RTC_REG_WRITE_SYNC(PLATFORM_PMU->real_time_clock.time.raw, rtc_time.raw);
}

static void platform_rtc_write_calendar( const platform_rtc_time_t* time )
{
    rtc_calendar_t rtc_calendar;

    rtc_calendar.bits.day_of_week   = NUM_TO_BCD_UNITS(time->weekday);
    rtc_calendar.bits.date_units    = NUM_TO_BCD_UNITS(time->date);
    rtc_calendar.bits.date_tens     = NUM_TO_BCD_TENS(time->date);
    rtc_calendar.bits.month_units   = NUM_TO_BCD_UNITS(time->month);
    rtc_calendar.bits.month_tens    = NUM_TO_BCD_TENS(time->month);
    rtc_calendar.bits.year_units    = NUM_TO_BCD_UNITS(time->year);
    rtc_calendar.bits.year_tens     = NUM_TO_BCD_TENS(time->year);
    rtc_calendar.bits.century_units = NUM_TO_BCD_UNITS(RTC_CURRENT_CENTURY);
    rtc_calendar.bits.century_tens  = NUM_TO_BCD_TENS(RTC_CURRENT_CENTURY);

    /* Write the calendar value to RTC */
    RTC_REG_WRITE_SYNC(PLATFORM_PMU->real_time_clock.calendar.raw, rtc_calendar.raw);
}

static platform_result_t platform_rtc_read_time_calendar( platform_rtc_time_t* time )
{
    /* Stop RTC time and calendar */
    RTC_STOP_TIME_CALENDAR;

    if ( platform_rtc_valid_time_calendar(time) != WICED_TRUE )
    {
        return PLATFORM_ERROR;
    }

    /* Read the time and calendar from RTC */
    platform_rtc_read_time(time);
    platform_rtc_read_calendar(time);

    /* Start RTC time and calendar */
    RTC_START_TIME_CALENDAR;

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_rtc_write_time_calendar( const platform_rtc_time_t* time )
{
    rtc_hour_mode_t rtc_hour_mode;

    /* Validate the input time and calendar settings */
    if ( validate_input_time_calendar(time) != WICED_TRUE )
    {
        return PLATFORM_ERROR;
    }

    /* Stop RTC time and calendar */
    RTC_STOP_TIME_CALENDAR;

    /* Operate in 24 Hour Format */
    rtc_hour_mode.raw = PLATFORM_PMU->real_time_clock.hour_mode.raw;
    rtc_hour_mode.bits.hour_mode = 0;
    RTC_REG_WRITE_SYNC(PLATFORM_PMU->real_time_clock.hour_mode.raw, rtc_hour_mode.raw);

    /* Write the time and calendar to RTC */
    platform_rtc_write_time(time);
    platform_rtc_write_calendar(time);

    if ( platform_rtc_valid_time_calendar(time) != WICED_TRUE )
    {
        return PLATFORM_ERROR;
    }

    /* Start RTC time and calendar */
    RTC_START_TIME_CALENDAR;

    return PLATFORM_SUCCESS;
}

platform_result_t platform_rtc_get_time( platform_rtc_time_t* time )
{
    wiced_assert( "bad argument", ( time != NULL ) );

    return platform_rtc_read_time_calendar(time);
}

platform_result_t platform_rtc_set_time( const platform_rtc_time_t* time )
{
    wiced_assert( "bad argument", ( time != NULL ) );

    return platform_rtc_write_time_calendar(time);
}

platform_result_t platform_rtc_init ( void )
{
    /* Make sure RTC is present */
    if ( (PLATFORM_PMU->pmucapabilities_ext & PMU_CAP_EXT_RTC_PRESENT) == 0 )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Set default RTC time and calendar */
    if ( platform_rtc_write_time_calendar(&default_rtc_time) != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}
