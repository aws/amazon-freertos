/** @file wmtime_cli.c
 *
 *  @brief This file provides  CLI for Time Manager
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


#include <stdlib.h>

#include <wmstdio.h>
#include <wmtime.h>
#include <cli.h>

static void cmd_time_usage(const char *str)
{
	wmprintf("Usage: time-set <year> <month> <day> <hour> <min> <sec> (all numeric)\r\n");
	wmprintf("Error: %s\r\n", str);
}


const char *month_names[] = { "Jan", "Feb", "Mar", "Apr", "May",
		      "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
const char *day_names[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static void cmd_time_get(int argc, char **argv)
{
	struct tm c_time;
	wmtime_time_get(&c_time);
	wmprintf("%s %d %s %.2d %.2d:%.2d:%.2d\r\n",
		       day_names[c_time.tm_wday],
		       c_time.tm_year + 1900, month_names[c_time.tm_mon],
		       c_time.tm_mday, c_time.tm_hour, c_time.tm_min, c_time.tm_sec);
}

static void cmd_time_get_posix(int argc, char **argv)
{
	wmprintf("%u\r\n", wmtime_time_get_posix());
}

static void cmd_time_set(int argc, char **argv)
{
	int val;
	struct tm c_time;
	const char *err_str;

	if (argc != 7) {
		err_str = "invalid number of arguments";
		goto error_usage;
	}

	/* Year */
	val = atoi(argv[1]);
	if (val < 1970 || val > 2111) {
		err_str = "year should be > 1970";
		goto error_usage;
	}
	c_time.tm_year = val - 1900;

	/* Month */
	val = atoi(argv[2]);
	if (val < 1 || val > 12) {
		err_str = "month should be from 1 to 12";
		goto error_usage;
	}
	/* tm struct containing month value form 0-11*/
	c_time.tm_mon = val - 1;

	/* Day */
	val = atoi(argv[3]);
	if (val < 1 || val > 31) {
		err_str = "date should be from 1 to 31";
		goto error_usage;
	}
	c_time.tm_mday = val;

	/* Hour */
	val = atoi(argv[4]);
	if (val < 0 || val > 23) {
		err_str = "hour should be from 0 to 23";
		goto error_usage;
	}
	c_time.tm_hour = val;

	/* Minute */
	val = atoi(argv[5]);
	if (val < 0 || val > 59) {
		err_str = "minute should be from 0 to 59";
		goto error_usage;
	}
	c_time.tm_min = val;

	/* Second */
	val = atoi(argv[6]);
	if (val < 0 || val > 59) {
		err_str = "second should be from 0 to 59";
		goto error_usage;
	}
	c_time.tm_sec = val;

	if (wmtime_time_set(&c_time)) {
		wmprintf("Error: Validation failed. Current time retained to \r\n");
	} else {
		wmprintf("New time set:\r\n");
	}
	cmd_time_get(1, NULL);

	return;
error_usage:
		cmd_time_usage(err_str);
		return;
}

static void cmd_time_set_posix(int argc, char **argv)
{
	time_t val;

	if (argc != 2) {
		return;
	}
	if ( (*argv[1]) == '-') {
		/* Error message from python test script */
		wmprintf("Usage: time-set-posix <sec_since_epoch>\r\n");
		wmprintf("Error: sec_since_epoch should be positive\r\n");
		return;
	}
	val = strtoull(argv[1], NULL, 10);
	wmtime_time_set_posix(val);
}

static struct cli_command time_cmds[] = {
	{"time-get", "", cmd_time_get},
	{"time-set", "<year> <month> <day> <hour> <min> <sec> (all numeric)", cmd_time_set},
	{"time-get-posix", "", cmd_time_get_posix},
	{"time-set-posix", "<sec_since_epoch>", cmd_time_set_posix},
};

int wmtime_cli_init(void)
{
	int i;

	for (i = 0; i < sizeof(time_cmds)/sizeof(struct cli_command); i++)
		if (cli_register_command(&time_cmds[i]))
			return 1;
	return 0;
}
