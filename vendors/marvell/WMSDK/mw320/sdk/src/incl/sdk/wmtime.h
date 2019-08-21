/*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/

/** @file wmtime.h
 *
 *  @brief Time Management Subsystem
 *
 * This module provides the time management APIs. Internally wireless
 * microcontroller manages time in terms of seconds since epoch (1 Jan 1970
 * 00:00). This representation is known as "posix time". These routines can
 * be used to do conversions between human readable time and posix time as well
 * as to set the system time to desired value.
 *
 */

#ifndef _WMTIME_H_
#define _WMTIME_H_

#include <compat_time.h>
#include <wmtypes.h>

typedef long suseconds_t;

/** Convert HTTP date format to POSIX time format
 *
 * \param[in] date HTTP date format
 *
 * \return success or failure as:
 *     -WM_FAIL: Conversion failed. Invalid format/data
 *     else valid time_t value
 */
time_t http_date_to_time(const unsigned char *date);

/** Set the date and time
 *
 * \param[in] tm The rtc value is updated with the values in tm structure
 * \return success or failure as:
 *     0: Success
 *     -1: Failed validation of tm structure
 */
extern int wmtime_time_set(const struct tm *tm);

/**
 * Get date and time
 *
 * \param[out] tm tm structure is updated to get the current value in rtc
 * \return success or failure as:
 *     0: Success
 *     non-zero: Internal error
 */
extern int wmtime_time_get(struct tm *tm);

/** Set the date and time using posix time
*
*  \param[in] time The rtc value is updated with the value present in time
*  \return success or failure as:
*      0: Success
*      non-zero: Internal error
*/
extern int wmtime_time_set_posix(time_t time);

/**
 * Get date and time in posix format
 *
*  \return time_t value from RTC
 */
extern time_t wmtime_time_get_posix(void);

/**
 * Convert to tm structure from POSIX/Unix time (Seconds since epoch)
 *
 * \param[in] time This is POSIX time that is to be converted into \ref tm
 * \param[out] result This should point to pre-allocated \ref tm instance
 * \return pointer to struct tm; NULL in case of error
 */
struct tm *gmtime_r(const time_t *time, struct tm *result);

/**
 * Converts to POSIX/Unix time from tm structure
 *
 * \param[in] tm This is \ref tm instance that is to be converted into
 * time_t format
 * \return time_t POSIX/Unix time equivalent
 */
extern time_t mktime(struct tm *tm);

/**
 * Converts the broken-down time value tm into a null-terminated string.
 *
 * \param[in] tm This is \ref tm instance that is to be converted string.
 *
 * @return Pointer to a statically allocated string which contains the date
 * and time format as follows "Tue Mar 24 09:20:14 2015". The statically
 * allocated string might be overwritten by subsequent calls to any of the
 * date and time functions.
 */
char *asctime(const struct tm *tm);

/**
 * Initialize time subsystem including RTC. Sets system time to 1/1/1970 00:00
 * (i.e. epoch 0)
 *
 * \return WM_SUCCESS on success, zero otherwise
 */
extern int wmtime_init(void);

/** Get current wmtime initialization status
 *
 *\return true if initialized, false if not.
 */
bool is_wmtime_init_done();

/**
 * Register wmtime cli commands for the application use
 *
 * \note This function can be called by the application
 * after time subsystem is initialized.
 *
 * \return success or failure as:
 *     0: Success
 *     non-zero: Failure
 */
int wmtime_cli_init(void);

#endif
