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
#ifndef _IOT_SNTP_DATETIME_H_
#define _IOT_SNTP_DATETIME_H_
/* structure to store parsed time received from server */
typedef struct _IotSntpDateTime
{
    int32_t tm_sec;   /* Seconds */
    int32_t tm_min;   /* Minutes */
    int32_t tm_hour;  /* Hour (0--23) */
    int32_t tm_mday;  /* Day of month (1--31) */
    int32_t tm_mon;   /* Month (0--11) */
    int32_t tm_year;  /* Year (calendar year minus 1900) */
    int32_t tm_wday;  /* Weekday (0--6; Sunday = 0) */
    int32_t tm_yday;  /* Day of year (0--365) */
    int32_t tm_isdst; /* 0 if daylight savings time is not in effect) */
} IotSntpDateTime_t;

/**
 *
 * @brief This function parses time given in time_t (as number of seconds from
 * Unix Epoch into #IotSntpDateTime_t
 *
 * @param[in] pTime seconds since 1 January 1970 (UTC)
 *
 * @param[out] pTimeBuf parsed time in #IotSntpDateTime_t format. Caller must supply
 * #IotSntpDateTime_t, this function does not allocate any memory.
 *
 * @return
 * Pointer #IotSntpDateTime_t  (same as second argument)
 *
 */
IotSntpDateTime_t * Iot_sntpParseTime( const time_t * pxTime,
                                       IotSntpDateTime_t * pxTimeBuf );

/**
 * @brief This function prints time in pretty format
 *
 * @param[in] logmsg - message to be printed before actual time
 * @param[in] currentSeconds seconds since 1 January 1970 (UTC)
 * @param[in] currentMS milliseconds
 */
void Iot_SntpPrintDateTime( char * logmsg,
                            time_t currentSeconds,
                            time_t currentMS );
#endif /* ifndef _IOT_SNTP_DATETIME_H_ */
