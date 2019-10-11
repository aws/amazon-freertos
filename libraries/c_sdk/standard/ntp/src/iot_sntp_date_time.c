/*
 * Amazon FreeRTOS SNTP Client V1.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* iot config file. */
#include "iot_config.h"

/* standard includes */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "iot_sntp_date_time.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_NETWORK
    #define LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_NETWORK
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
    #endif
#endif

#define LIBRARY_LOG_NAME    ( "NTP" )
#include "iot_logging_setup.h"


/* Year 1 for Unix UTC time */
#define GMTIME_FIRST_YEAR     1970
#define SECONDS_PER_MINUTE    ( 60 )
#define MINUTES_PER_HOUR      ( 60 )
#define HOURS_PER_DAY         ( 24 )
#define SECONDS_PER_HOUR      ( MINUTES_PER_HOUR * SECONDS_PER_MINUTE )
#define SECONDS_PER_DAY       ( HOURS_PER_DAY * SECONDS_PER_HOUR )
/* Make a bitmask with a '1' for each 31-day month. */
#define MONTH_MASK( month )    ( 1u << ( month - 1 ) )
#define LONG_MONTHS_MASK      ( MONTH_MASK( 1 ) | MONTH_MASK( 3 ) | MONTH_MASK( 5 ) | MONTH_MASK( 7 ) | MONTH_MASK( 8 ) | MONTH_MASK( 10 ) | MONTH_MASK( 12 ) )

static bool _isLeapYear( uint32_t year )
{
    bool isLeapYear = false;

    if( ( year % 4 ) != 0 )
    {
        /* Not a multiple of 4 years. */
        isLeapYear = false;
    }
    else if( ( year % 400 ) == 0 )
    {
        /* Every 4 centuries there is a leap year */
        isLeapYear = true;
    }
    else if( ( year % 100 ) == 0 )
    {
        /* Other centuries are not a leap year */
        isLeapYear = false;
    }
    else
    {
        /* Otherwise every fourth year. */
        isLeapYear = true;
    }

    return isLeapYear;
}
/*-----------------------------------------------------------*/

static uint32_t _daysPerYear( uint32_t year )
{
    uint32_t days;

    if( _isLeapYear( year ) )
    {
        days = 366;
    }
    else
    {
        days = 365;
    }

    return days;
}
/*-----------------------------------------------------------*/

static uint32_t _daysPerMonth( uint32_t year,
                               uint32_t month )
{
    uint32_t days;

    /* Month is zero-based, 1 is February. */
    if( month != 1 )
    {
        /* 30 or 31 days? */
        if( ( LONG_MONTHS_MASK & ( 1u << month ) ) != 0 )
        {
            days = 31;
        }
        else
        {
            days = 30;
        }
    }
    else if( _isLeapYear( year ) == false )
    {
        /* February, non leap year. */
        days = 28;
    }
    else
    {
        /* February, leap year. */
        days = 29;
    }

    return days;
}
/*-----------------------------------------------------------*/


IotSntpDateTime_t * Iot_sntpParseTime( const time_t * pTime,
                                       IotSntpDateTime_t * pTimeBuf )
{
    time_t xTime = *pTime;
    uint32_t daySeconds, dayNumber;
    uint16_t year = GMTIME_FIRST_YEAR;
    uint16_t month = 0;

    /* Clear all fields, some might not get set here. */
    memset( ( void * ) pTimeBuf, '\0', sizeof( *pTimeBuf ) );

    /* Seconds since last midnight. */
    daySeconds = ( unsigned long ) ( xTime % SECONDS_PER_DAY );

    /* Days since 1 Jan 1970. */
    dayNumber = ( unsigned long ) ( xTime / SECONDS_PER_DAY );

    /* Today's HH:MM:SS */
    pTimeBuf->tm_hour = daySeconds / SECONDS_PER_HOUR;
    pTimeBuf->tm_min = ( daySeconds % SECONDS_PER_HOUR ) / 60;
    pTimeBuf->tm_sec = daySeconds % 60;

    /* Today's week day, knowing that 1-1-1970 was a Thursday (4). */
    pTimeBuf->tm_wday = ( dayNumber + 4 ) % 7;

    for( ; ; )
    {
        /* Keep subtracting 365 (or 366) days while possible. */
        unsigned long ulDays = _daysPerYear( year );

        if( dayNumber < ulDays )
        {
            break;
        }

        dayNumber -= ulDays;
        year++;
    }

    /* Set Year */
    pTimeBuf->tm_year = year;

    /* The day within this year. */
    pTimeBuf->tm_yday = dayNumber;

    /* Month are counted as 0..11 */
    month = 0;

    for( ; ; )
    {
        uint32_t days = _daysPerMonth( year, month );

        /* Keep subtracting 30 (or 28, 29, or 31) days while possible. */
        if( dayNumber < days )
        {
            break;
        }

        dayNumber -= days;
        month++;
    }

    pTimeBuf->tm_mon = month;

    /* Month days are counted as 1..31 */
    pTimeBuf->tm_mday = dayNumber + 1;

    return pTimeBuf;
}
/*-----------------------------------------------------------*/

/*
 * @brief Utility function to pretty print date. Prints time and date in UTC
 *
 * @param logmsg a string to be printed before formated time.
 * @param currentSeconds seconds since unix eponch Jan 1st 1970 00:00
 * @param currentMS fraction of seconds in milliseconds
 */
void Iot_SntpPrintDateTime( char * logmsg,
                            time_t currentSeconds,
                            time_t currentMS )
{
    IotSntpDateTime_t sntpTime = { 0 };

    Iot_sntpParseTime( &currentSeconds, &sntpTime );

    IotLogInfo( "%s: %d/%d/%02d %2d:%02d:%02d.%03u",
                logmsg,
                sntpTime.tm_mon + 1,
                sntpTime.tm_mday,
                sntpTime.tm_year,
                sntpTime.tm_hour,
                sntpTime.tm_min,
                sntpTime.tm_sec,
                ( unsigned ) currentMS );
}
