/** @file wmtime.c
 *
 *  @brief This file provides  Functions for time management
 *
 *  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved.
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


/** time.c: Functions for time management
 */
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include <rtc.h>
#include <wmstdio.h>
#include <wmtime.h>
#include <wmerrno.h>

#define YEAR0           1900 /* The first year */
#define EPOCH_YR        1970 /* EPOCH = Jan 1 1970 00:00:00 */

uint32_t SEC_PER_YR[2] = { 31536000, 31622400 };

uint32_t SEC_PER_MT[2][12] = {
	{2678400, 2419200, 2678400, 2592000, 2678400, 2592000,
	 2678400, 2678400, 2592000, 2678400, 2592000, 2678400}
	,
	{2678400, 2505600, 2678400, 2592000, 2678400, 2592000,
	 2678400, 2678400, 2592000, 2678400, 2592000, 2678400}
	,
};

uint32_t SEC_PER_DY = 86400;
uint32_t SEC_PER_HR = 3600;

#define STD_ASCTIME_BUF_SIZE 30
static char asctime_buf[STD_ASCTIME_BUF_SIZE];
static bool init_done;

static const char wday_name[][4] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char mon_name[][4] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static int month_from_string_short(const char *month)
{
	if (strncmp(month, "Jan", 3) == 0)
		return 0;
	if (strncmp(month, "Feb", 3) == 0)
		return 1;
	if (strncmp(month, "Mar", 3) == 0)
		return 2;
	if (strncmp(month, "Apr", 3) == 0)
		return 3;
	if (strncmp(month, "May", 3) == 0)
		return 4;
	if (strncmp(month, "Jun", 3) == 0)
		return 5;
	if (strncmp(month, "Jul", 3) == 0)
		return 6;
	if (strncmp(month, "Aug", 3) == 0)
		return 7;
	if (strncmp(month, "Sep", 3) == 0)
		return 8;
	if (strncmp(month, "Oct", 3) == 0)
		return 9;
	if (strncmp(month, "Nov", 3) == 0)
		return 10;
	if (strncmp(month, "Dec", 3) == 0)
		return 11;
	/* not a valid date */
	return 12;
}

time_t time(time_t *c_time)
{
	time_t ltime;
	ltime = wmtime_time_get_posix();

	if (c_time)
		*c_time = ltime;

	return ltime;
}

time_t http_date_to_time(const unsigned char *date)
{
	struct tm tm_time;
	time_t ret = 0;
	char buf[12];
	const unsigned char *start_date = NULL;
	int i = 0;

	/* make sure we can use it */
	if (!date)
		return 0;
	memset(&tm_time, 0, sizeof(struct tm));
	memset(buf, 0, 12);
	/* try to figure out which format it's in */
	/* rfc 1123 */
	if (date[3] == ',') {
		if (strlen((char *)date) != 29)
			return 0;
		/* make sure that everything is legal */
		if (date[4] != ' ')
			return 0;
		/* 06 */
		if ((isdigit(date[5]) == 0) || (isdigit(date[6]) == 0))
			return 0;
		/* Nov */
		tm_time.tm_mon = month_from_string_short((char *)&date[8]);
		if (tm_time.tm_mon >= 12)
			return 0;
		/* 1994 */
		if ((isdigit(date[12]) == 0) ||
		    (isdigit(date[13]) == 0) ||
		    (isdigit(date[14]) == 0) || (isdigit(date[15]) == 0))
			return 0;
		if (date[16] != ' ')
			return 0;
		/* 08:49:37 */
		if ((isdigit(date[17]) == 0) ||
		    (isdigit(date[18]) == 0) ||
		    (date[19] != ':') ||
		    (isdigit(date[20]) == 0) ||
		    (isdigit(date[21]) == 0) ||
		    (date[22] != ':') ||
		    (isdigit(date[23]) == 0) || (isdigit(date[24]) == 0))
			return 0;
		if (date[25] != ' ')
			return 0;
		/* GMT */
		if (strncmp((char *)&date[26], "GMT", 3) != 0)
			return 0;
		/* ok, it's valid.  Do it */
		/* parse out the day of the month */
		tm_time.tm_mday += (date[5] - 0x30) * 10;
		tm_time.tm_mday += (date[6] - 0x30);
		/* already got the month from above */
		/* parse out the year */
		tm_time.tm_year += (date[12] - 0x30) * 1000;
		tm_time.tm_year += (date[13] - 0x30) * 100;
		tm_time.tm_year += (date[14] - 0x30) * 10;
		tm_time.tm_year += (date[15] - 0x30);
		tm_time.tm_year -= YEAR0;
		/* parse out the time */
		tm_time.tm_hour += (date[17] - 0x30) * 10;
		tm_time.tm_hour += (date[18] - 0x30);
		tm_time.tm_min += (date[20] - 0x30) * 10;
		tm_time.tm_min += (date[21] - 0x30);
		tm_time.tm_sec += (date[23] - 0x30) * 10;
		tm_time.tm_sec += (date[24] - 0x30);
		/* ok, generate the result */
		ret = mktime(&tm_time);
	}
	/* ansi C */
	else if (date[3] == ' ') {
		if (strlen((char *)date) != 24)
			return 0;
		/* Nov */
		tm_time.tm_mon = month_from_string_short((char *)&date[4]);
		if (tm_time.tm_mon >= 12)
			return 0;
		if (date[7] != ' ')
			return 0;
		/* "10" or " 6" */
		if (((date[8] != ' ') && (isdigit(date[8]) == 0)) ||
		    (isdigit(date[9]) == 0))
			return 0;
		if (date[10] != ' ')
			return 0;
		/* 08:49:37 */
		if ((isdigit(date[11]) == 0) ||
		    (isdigit(date[12]) == 0) ||
		    (date[13] != ':') ||
		    (isdigit(date[14]) == 0) ||
		    (isdigit(date[15]) == 0) ||
		    (date[16] != ':') ||
		    (isdigit(date[17]) == 0) || (isdigit(date[18]) == 0))
			return 0;
		if (date[19] != ' ')
			return 0;
		/* 1994 */
		if ((isdigit(date[20]) == 0) ||
		    (isdigit(date[21]) == 0) ||
		    (isdigit(date[22]) == 0) || (isdigit(date[23]) == 0))
			return 0;
		/* looks good */
		/* got the month from above */
		/* parse out the day of the month */
		if (date[8] != ' ')
			tm_time.tm_mday += (date[8] - 0x30) * 10;
		tm_time.tm_mday += (date[9] - 0x30);
		/* parse out the time */
		tm_time.tm_hour += (date[11] - 0x30) * 10;
		tm_time.tm_hour += (date[12] - 0x30);
		tm_time.tm_min += (date[14] - 0x30) * 10;
		tm_time.tm_min += (date[15] - 0x30);
		tm_time.tm_sec += (date[17] - 0x30) * 10;
		tm_time.tm_sec += (date[18] - 0x30);
		/* parse out the year */
		tm_time.tm_year += (date[20] - 0x30) * 1000;
		tm_time.tm_year += (date[21] - 0x30) * 100;
		tm_time.tm_year += (date[22] - 0x30) * 10;
		tm_time.tm_year += (date[23] - 0x30);
		tm_time.tm_year -= YEAR0;
		/* generate the result */
		ret = mktime(&tm_time);
	}
	/* must be the 1036... */
	else {
		/* check to make sure we won't run out of any bounds */
		if (strlen((char *)date) < 11)
			return 0;
		while (date[i]) {
			if (date[i] == ' ') {
				start_date = &date[i + 1];
				break;
			}
			i++;
		}
		/* check to make sure there was a space found */
		if (start_date == NULL)
			return 0;
		/* check to make sure that we don't overrun anything */
		if (strlen((char *)start_date) != 22)
			return 0;
		/* make sure that the rest of the date was valid */
		/* 06- */
		if ((isdigit(start_date[0]) == 0) ||
		    (isdigit(start_date[1]) == 0) || (start_date[2] != '-'))
			return 0;
		/* Nov */
		tm_time.tm_mon = month_from_string_short((char *)
							 &start_date[3]);
		if (tm_time.tm_mon >= 12)
			return 0;
		/* -94 */
		if ((start_date[6] != '-') ||
		    (isdigit(start_date[7]) == 0) ||
		    (isdigit(start_date[8]) == 0))
			return 0;
		if (start_date[9] != ' ')
			return 0;
		/* 08:49:37 */
		if ((isdigit(start_date[10]) == 0) ||
		    (isdigit(start_date[11]) == 0) ||
		    (start_date[12] != ':') ||
		    (isdigit(start_date[13]) == 0) ||
		    (isdigit(start_date[14]) == 0) ||
		    (start_date[15] != ':') ||
		    (isdigit(start_date[16]) == 0) ||
		    (isdigit(start_date[17]) == 0))
			return 0;
		if (start_date[18] != ' ')
			return 0;
		if (strncmp((char *)&start_date[19], "GMT", 3) != 0)
			return 0;
		/* looks ok to parse */
		/* parse out the day of the month */
		tm_time.tm_mday += (start_date[0] - 0x30) * 10;
		tm_time.tm_mday += (start_date[1] - 0x30);
		/* have the month from above */
		/* parse out the year */
		tm_time.tm_year += (start_date[7] - 0x30) * 10;
		tm_time.tm_year += (start_date[8] - 0x30);
		/* check for y2k */
		if (tm_time.tm_year < 20)
			tm_time.tm_year += 100;
		/* parse out the time */
		tm_time.tm_hour += (start_date[10] - 0x30) * 10;
		tm_time.tm_hour += (start_date[11] - 0x30);
		tm_time.tm_min += (start_date[13] - 0x30) * 10;
		tm_time.tm_min += (start_date[14] - 0x30);
		tm_time.tm_sec += (start_date[16] - 0x30) * 10;
		tm_time.tm_sec += (start_date[17] - 0x30);
		/* generate the result */
		ret = mktime(&tm_time);
	}
	return ret;
}

/**
 * Returns 1 if current year id a leap year
 */
static inline int is_leap(int yr)
{
	if (!(yr % 100))
		return (yr % 400 == 0) ? 1 : 0;
	else
		return (yr % 4 == 0) ? 1 : 0;
}

static unsigned char day_of_week_get(unsigned char month, unsigned char day,
				     unsigned short year)
{
	/* Month should be a number 0 to 11, Day should be a number 1 to 31 */

	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	year -= month < 3;
	return (year + year / 4 - year / 100 + year / 400 + t[month - 1] +
		day) % 7;
}

time_t mktime(struct tm *tm)
{
	int i;
	int leapyr = 0;

	time_t ret = 0;

	for (i = EPOCH_YR; i < (tm->tm_year + YEAR0); i++)
		ret += SEC_PER_YR[is_leap(i)];

	if (is_leap(tm->tm_year + YEAR0))
		leapyr = 1;

	for (i = 0; i < (tm->tm_mon); i++) {
		ret += SEC_PER_MT[leapyr][i];
	}

	ret += ((tm->tm_mday) - 1) * SEC_PER_DY;
	ret += (tm->tm_hour) * SEC_PER_HR;
	ret += (tm->tm_min) * 60;
	ret += tm->tm_sec;

	return ret;
}

struct tm *gmtime_r(const time_t * time, struct tm *result)
{
	int leapyr = 0;
	time_t ltime = *time;

	memset(result, 0, sizeof(struct tm));
	result->tm_year = EPOCH_YR;

	while (1) {
		if (ltime < SEC_PER_YR[is_leap(result->tm_year)]) {
			break;
		}
		ltime -= SEC_PER_YR[is_leap(result->tm_year)];
		result->tm_year++;
	}

	result->tm_yday = ltime / SEC_PER_DY;

	leapyr = is_leap(result->tm_year);

	while (1) {
		if (ltime < SEC_PER_MT[leapyr][result->tm_mon])
			break;
		ltime -= SEC_PER_MT[leapyr][result->tm_mon];
		result->tm_mon++;
	}

	result->tm_mday = ltime / SEC_PER_DY;
	result->tm_mday++;
	ltime = ltime % SEC_PER_DY;

	result->tm_hour = ltime / SEC_PER_HR;
	ltime = ltime % SEC_PER_HR;

	result->tm_min = ltime / 60;
	result->tm_sec = ltime % 60;

	result->tm_wday =
	    day_of_week_get(result->tm_mon + 1, result->tm_mday,
			    result->tm_year);

	/*
	 * The number of years since YEAR0"
	 */
	result->tm_year -= YEAR0;

	/* fixme: how to handle daylight saving ? */
	result->tm_isdst = 0;

	return result;
}

static int validate_date_time(const struct tm *tm)
{
	if (tm->tm_sec > 59)
		return -1;
	if (tm->tm_min > 59)
		return -1;
	if (tm->tm_hour > 23)
		return -1;
	if (((tm->tm_year + YEAR0) < EPOCH_YR) || ((tm->tm_year + YEAR0) > 2111))
		return -1;
	if (tm->tm_mon >= 12)
		return -1;
	if (tm->tm_mon == 1) {
		if (!is_leap(tm->tm_year + YEAR0)) {
			if (tm->tm_mday > 28)
				return -1;
		} else {
			if (tm->tm_mday > 29)
				return -1;
		}
	}
	switch (tm->tm_mon) {
	case 3:
	case 5:
	case 8:
	case 10:
		if (tm->tm_mday > 30)
			return -1;
	}

	if ((tm->tm_mday < 1) || (tm->tm_mday > 31))
		return -1;

	return 0;
}

/**
 * Set date and time
 */
int wmtime_time_set(const struct tm *tm)
{
	time_t time;

	if (validate_date_time(tm) == 0) {
		time = mktime((struct tm *)tm);
		return wmtime_time_set_posix(time);
	} else {
		return -1;
	}
}

/**
 * Get date and time
 */
int wmtime_time_get(struct tm *tm)
{
	time_t curtime;
	curtime = wmtime_time_get_posix();

	if (gmtime_r((const time_t *)&curtime, tm) == NULL) {
		return -1;
	}

	return 0;
}

int wmtime_time_set_posix(time_t time)
{
	return rtc_time_set(time);
}

time_t wmtime_time_get_posix(void)
{
	return rtc_time_get();
}

int wmtime_init()
{
	int rc = 0;

	if (init_done)
		return WM_SUCCESS;

	rtc_init();
	init_done = true;
	return rc;
}

bool is_wmtime_init_done()
{
	return init_done;
}

char *asctime(const struct tm *tm)
{
	if (!tm)
		return NULL;

	memset(asctime_buf, 0x00, sizeof(asctime_buf));
	snprintf(asctime_buf, STD_ASCTIME_BUF_SIZE,
		 "%s %s %d %d.%d.%d %d", wday_name[tm->tm_wday],
		 mon_name[tm->tm_mon], tm->tm_mday, tm->tm_hour, tm->tm_min,
		 tm->tm_sec, tm->tm_year + YEAR0);

	return asctime_buf;
}

int gettimeofday(struct timeval *tv, void *tz)
{
	(void) tz;

	if (tv == NULL)
		return -1;

	tv->tv_sec = wmtime_time_get_posix();
	/* fixme; This could be improved */
	tv->tv_usec = 0;

	/* Unix value for successful */
	return 0;
}

#if 0
/*
 * @internal: Debug function.
 */
void dump_struct_tm(const struct tm *tm)
{
	wmprintf("*-*-*-*-*-*-*-*-*-*\r\n");
	wmprintf("tm->tm_sec: %d\r\n", tm->tm_sec);
	wmprintf("tm->tm_min: %d\r\n", tm->tm_min);
	wmprintf("tm->tm_hour: %d\r\n", tm->tm_hour);
	wmprintf("tm->tm_mday: %d\r\n", tm->tm_mday);
	wmprintf("tm->tm_mon: %d\r\n", tm->tm_mon);
	wmprintf("tm->tm_year: %d\r\n", tm->tm_year);
	wmprintf("tm->tm_wday: %d\r\n", tm->tm_wday);
	wmprintf("tm->tm_yday: %d\r\n", tm->tm_yday);
	wmprintf("tm->tm_isdst: %d\r\n", tm->tm_isdst);
	wmprintf("*-*-*-*-*-*-*-*-*-*\r\n");
}
#endif
