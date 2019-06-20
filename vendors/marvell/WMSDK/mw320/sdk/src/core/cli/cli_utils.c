/** @file cli_utils.c
 *
 *  @brief This file provides  Convenience functions for the CLI
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


/* cli_utils.c: Convenience functions for the CLI
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <wmstdio.h>
#include <cli.h>
#include <ctype.h>

int string_equal(const char *s1, const char *s2)
{
	size_t len = strlen(s1);

	if (len == strlen(s2) && !strncmp(s1, s2, len))
		return 1;
	return 0;
}

/**
 *@brief convert char to hex integer
 *
 *@param chr          char
 *@return             hex integer
 **/
uint8_t hexc2bin(char chr)
{
	if (chr >= '0' && chr <= '9')
		chr -= '0';
	else if (chr >= 'A' && chr <= 'F')
		chr -= ('A' - 10);
	else if (chr >= 'a' && chr <= 'f')
		chr -= ('a' - 10);
	return chr;
}

/**
 *@brief convert string to hex integer
 *
 *@param s            A pointer string buffer
 *@return             hex integer
 **/
uint32_t a2hex(const char *s)
{
	uint32_t val = 0;

	if (!strncasecmp("0x", s, 2)) {
		s += 2;
	}

	while (*s && isxdigit((unsigned char)*s)) {
		val = (val << 4) + hexc2bin(*s++);
	}
	return val;
}

/**
 *    @brief isdigit for String.
 *
 *    @param x            Char string
 *    @return             0 for non-digit.
 *                        1 for digit
 */
int ISDIGIT(char *x)
{
	unsigned int i;
	for (i = 0; i < strlen(x); i++)
		if (isdigit((unsigned char)x[i]) == 0)
			return 0;
	return 1;
}

/**
 *  @brief      Hex to number
 *
 *  @param c    Hex value
 *  @return     Integer value or -1
 */
static int hex2num(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;

	return -1;
}

/**
 *  @brief              Check hex string
 *
 *  @param hex          A pointer to hex string
 *  @return             1 or 0
 */
int ishexstring(void *hex)
{
	int i, a;
	char *p = hex;
	int len = strlen(p);
	if (!strncasecmp("0x", p, 2)) {
		p += 2;
		len -= 2;
	}
	for (i = 0; i < len; i++) {
		a = hex2num(*p);
		if (a < 0)
			return 0;
		p++;
	}
	return 1;
}


/*
 * @brief convert String to integer
 *
 *@param value        A pointer to string
 *@return             integer
 **/
uint32_t a2hex_or_atoi(char *value)
{
	if (value[0] == '0' && (value[1] == 'X' || value[1] == 'x')) {
		return a2hex(value + 2);
	} else if (isdigit((unsigned char)*value)) {
		return atoi(value);
	} else {
		return *value;
	}
}

int get_uint(const char *arg, unsigned int *dest, unsigned int len)
{
	int i;
	unsigned int val = 0;

	for (i = 0; i < len; i++) {
		if (arg[i] < '0' || arg[i] > '9')
			return 1;
		val *= 10;
		val += arg[i] - '0';
	}

	*dest = val;
	return 0;
}

/* Parse string 'arg' formatted "AA:BB:CC:DD:EE:FF" (assuming 'sep' is ':')
 * into a 6-byte array 'dest' such that dest = {0xAA,0xBB,0xCC,0xDD,0xEE,0xFF} 
 * set 'sep' accordingly. */
int get_mac(const char *arg, char *dest, char sep)
{
	unsigned char n;
	int i, j, k;

	if (strlen(arg) < 17)
		return 1;

	memset(dest, 0, 6);

	for (i = 0, k = 0; i < 17; i += 3, k++) {
		for (j = 0; j < 2; j++) {
			if (arg[i + j] >= '0' && arg[i + j] <= '9')
				n = arg[i + j] - '0';
			else if (arg[i + j] >= 'A' && arg[i + j] <= 'F')
				n = arg[i + j] - 'A' + 10;
			else if (arg[i + j] >= 'a' && arg[i + j] <= 'f')
				n = arg[i + j] - 'a' + 10;
			else
				return 1;

			n <<= 4 * (1 - j);
			dest[k] += n;
		}
		if (i < 15 && arg[i + 2] != sep)
			return 1;
	}

	return 0;
}

void print_mac(const char *mac)
{
	wmprintf("%02X:%02X:%02X:%02X:%02X:%02X ", mac[0], mac[1], mac[2],
		       mac[3], mac[4], mac[5]);
}

/* Non-reentrant getopt implementation */
int cli_optind = 0;
char *cli_optarg = NULL;
int cli_getopt(int argc, char **argv, const char *fmt)
{
	char *opt, *c;

	if (cli_optind == argc)
		return -1;
	cli_optarg = NULL;
	opt = argv[cli_optind];
	if (opt[0] != '-')
		return -1;
	if (opt[0] == 0 || opt[1] == 0)
		return '?';
	cli_optind++;
	c = strchr(fmt, opt[1]);
	if (c == NULL)
		return opt[1];
	if (c[1] == ':') {
		if (cli_optind < argc)
			cli_optarg = argv[cli_optind++];
	}
	return c[0];
}
