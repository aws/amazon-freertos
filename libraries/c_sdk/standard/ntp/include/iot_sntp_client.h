/*
 * Amazon FreeRTOS SNTP Client V1.1.4
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

#ifndef _IOT_SNTP_CLIENT_H_
#define _IOT_SNTP_CLIENT_H_

/**
 * @brief return codes IOT SNTP library
 */
typedef enum IotSntpError
{
    IOT_SNTP_ERROR_NONE = 0,
    IOT_SNTP_GENERIC_FAILURE = -9,
    IOT_SNTP_TASKPOOL_FAILED,
    IOT_SNTP_INIT_FAILED,
    IOT_SNTP_INVALID_TIME,
    IOT_SNTP_RESPONSE_ERROR,
} IotSntpError_t;

/**
 * @brief structure to store time of day in seconds and micro seconds.
 */
typedef struct IotSntpTimeval
{
    uint64_t tv_sec;
    uint64_t tv_usec;
} IotSntpTimeval_t;

/* Max NTP servers */
#define IOT_MAX_SNTP_SERVERS    ( ( uint8_t ) 5 )

/**
 * @brief structure to store SNTP configuration
 */
typedef struct IotSntpConfig
{
    /* List of NTP servers currently active */
    const char * pActiveNTPServers[ IOT_MAX_SNTP_SERVERS ];
    /* Total number of NTP servers configured */
    uint8_t sntpServerCount;
    /* Socket send/recv timeout in milliseconds */
    uint16_t sntpSocketTimeout;
    /* Polling period */
    uint16_t sntpPollingPeriod;
    /* If true, use async receive network abstraction */
    bool sntpUseRecvCallback;
} IotSntpConfig_t;

/**
 * @brief signature for SNTP status callback function.
 *
 * This is an optional callback function the application can pass to @ref
 * Iot_SntpStart().
 * @param [in] status: A boolean indicating if SNTP module was successfully started
 *
 */
typedef void (* IotSntpStatusCallback)( bool status );


/**
 * @brief Start SNTP service.
 *
 * This function call will trigger NTP state machine. This function should be called
 * once after A:FR reboots. It will return error if NTP server request creation fails.
 * If IotNTPPollingInterval is set to value other than zero, then this function will
 * start SNTP state machine to fetch wallclock time from NTP server every polling interval.
 * If polling interval is set to zero, this function will fetch wallclock time
 * exactly once and then it application's responsiblity to update the time by calling
 * the function again.
 *
 * @param[in] sntpStatusCallback A pointer to callback function to be executed when SNTP
 * successfully fetches wallclock time for the first time. It can be set to NULL if no
 * callback is desired
 *
 * @param[in] pSntpConfig A pointer to IotSntpConfig_t structure. If application wants to
 * setup SNTP configuration, it should pass this structure. If set to NULL, the library will
 * use default configration from iot_sntp_config.h
 * @note SNTP library will not copy server addresses ( pActiveNTPServers from #IotSntpConfig_t)
 * therefore server address strings should not go out of scope during lifetime of NTP client.
 *
 * @return
 * returns
 * * On Success, IOT_SNTP_ERROR_NONE.
 * * On Failure, IOT_SNTP_INIT_FAILED.
 *
 * @warning This SNTP implementation does not handle server authentication.
 * @warning This SNTP implementation is not secure. 
 */
IotSntpError_t Iot_SntpStart( IotSntpStatusCallback sntpStatusCallback,
                              IotSntpConfig_t * pSntpConfig );

/**
 * @brief This function returns current UTC time ( seconds since Jan 1 1970 )
 *
 * This function can be used by an application to get the wallclock time.
 * This function should not be used to measure time or when monotonically increasing
 * time is required, as SNTP time may move backwards to compensate for system clock error.
 * This function will return failure when there is no valid wallclock time available.
 * The failure can happen after system restart when SNTP service has not yet fetched
 * the wallclock time from the server or there no RTC on board.
 * In case of both success and failure, tv will be populated with current available time.
 *
 * @param[out] tv A pointer to a IotSntptimeval_t structure that contains the time in seconds
 * and microseconds since 1 January 1970 (UTC)
 *
 * @return
 * On Success, IOT_SNTP_ERROR_NONE.
 * On Failure, IOT_SNTP_INVALID_TIME.
 *
 */
IotSntpError_t Iot_SntpGetTimeOfDay( IotSntpTimeval_t * tv );

#endif /* ifndef _IOT_SNTP_CLIENT_H_ */
