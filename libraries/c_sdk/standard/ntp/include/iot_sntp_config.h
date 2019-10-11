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

#ifndef _IOT_SNTP_CCONFIG_H_
#define _IOT_SNTP_CONFIG_H_

/* NTP server port */
#define IOT_SNTP_PORT                  ( ( uint16_t ) 123 )

/* Polling period in seconds */
#define IOT_SNTP_POLLING_PERIOD        ( ( uint16_t ) 60 )

/* SNTP client socket timeout in milliseconds */
#define IOT_SNTP_SOCKET_TIMEOUT        ( ( uint16_t ) 500 )

/* Difference between NTP and UNIX epoch  1-1-1900 - 1-1-1970 */
#define IOT_SNTP1900_TO_UNIX1970       ( ( uint32_t ) 2208988800UL )

/* Minimum polling interval in seconds */
#define IOT_SNTP_MIN_POLLING_PERIOD    ( ( uint16_t ) 60 )

/* SNTP Failure Retry period in seconds */
#define IOT_SNTP_RETRY_PERIOD          ( ( uint16_t ) 10 )

/* SNTP Max retries */
#define IOT_SNTP_MAX_RETRIES           ( ( uint16_t ) 10 )

/* Use Async receive (recommened), set to false if sync receive is preferred */
#define SNTP_USE_RECEIVE_CALLBACK      true

/* List of sntp servers  */
static const char * pNTPServers[] =
{
    /*
     * Add List of NTP servers here ( up to IOT_MAX_SNTP_SERVERS defined in iot_sntp_client.h)
     * For example:
     * "1.test.ntp.org",
     * "2.test.ntp.org"
     * (Note: Replace empty sting ("") with actual NTP server.)
     */
    ""
};

#endif /* ifndef _IOT_SNTP_CCONFIG_H_ */
