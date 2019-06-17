/** @file  wifi_common.h
 *
 *  @brief Header file for wifi common functions
 *
 * (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
 *
 * MARVELL CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Marvell International Ltd or its
 * suppliers or licensors. Title to the Material remains with Marvell
 * International Ltd or its suppliers and licensors. The Material contains
 * trade secrets and proprietary and confidential information of Marvell or
 * its suppliers and licensors. The Material is protected by worldwide
 * copyright and trade secret laws and treaty provisions. No part of the
 * Material may be used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed, or disclosed in any way without
 * Marvell's prior express written permission.
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Marvell in writing.
 *
 */
#ifndef _WIFI_COMMON_H
#define _WIFI_COMMON_H
#include <wifi.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

#define wfd_e(...)                             \
	wmlog_e("p2p", ##__VA_ARGS__)
#define wfd_w(...)                             \
	wmlog_w("p2p", ##__VA_ARGS__)

#ifdef CONFIG_P2P_DEBUG
#define wfd_d(...)                             \
	wmlog("p2p", ##__VA_ARGS__)
/**
 *  @brief Dump hex data
 *
 *  @param p        A pointer to data buffer
 *  @param len      The len of data buffer
 *  @param delim    Deliminator character
 *  @return         Hex integer
 */
static void
hexdump(void *p, t_s32 len, t_s8 delim)
{
	t_s32 i;
	t_u8 *s = p;
	wmprintf("HexDump: len=%d\r\n", (int) len);
	for (i = 0; i < len; i++) {
		if (i != len - 1)
			wmprintf("%02x%c", *s++, delim);
		else
			wmprintf("%02x\r\n", *s);
		if ((i + 1) % 16 == 0)
			wmprintf("\r\n");
	}
}

#else
#define wfd_d(...)
#endif /* ! CONFIG_P2P_DEBUG */

/** Success */
#define SUCCESS     1
/** Failure */
#define FAILURE     0
/** MAC BROADCAST */
#define WIFIDIRECT_RET_MAC_BROADCAST   0x1FF
/** MAC MULTICAST */
#define WIFIDIRECT_RET_MAC_MULTICAST   0x1FE

/* fixme: remove this after mlan integration complete
 */
/* Definition of firmware host command */
/** HostCmd_DS_GEN */
typedef PACK_START struct
{
	/** Command */
	t_u16 command;
	/** Size */
	t_u16 size;
	/** Sequence number */
	t_u16 seq_num;
	/** Result */
	t_u16 result;
} PACK_END HostCmd_DS_GEN;



/**
 *  @brief  Detects duplicates channel in array of strings
 *
 *  @param  argc    Number of elements
 *  @param  argv    Array of strings
 *  @return UAP_FAILURE or UAP_SUCCESS
 */
static inline int
has_dup_channel(int argc, char *argv[])
{
	int i, j;
	/* Check for duplicate */
	for (i = 0; i < (argc - 1); i++) {
		for (j = i + 1; j < argc; j++) {
			if (atoi(argv[i]) == atoi(argv[j]))
				return FAILURE;
		}
	}
	return SUCCESS;
}

/**
 *  @brief Converts colon separated MAC address to hex value
 *
 *  @param mac      A pointer to the colon separated MAC string
 *  @param raw      A pointer to the hex data buffer
 *  @return         SUCCESS or FAILURE
 *                  WIFIDIRECT_RET_MAC_BROADCAST  - if broadcast mac
 *                  WIFIDIRECT_RET_MAC_MULTICAST - if multicast mac
 */
static int
mac2raw(char *mac, t_u8 * raw)
{
	unsigned int temp_raw[ETH_ALEN];
	int num_tokens = 0;
	int i;
	if (strlen(mac) != ((2 * ETH_ALEN) + (ETH_ALEN - 1)))
		return FAILURE;
	num_tokens = sscanf(mac, "%2x:%2x:%2x:%2x:%2x:%2x",
			temp_raw + 0, temp_raw + 1, temp_raw + 2, temp_raw + 3,
			temp_raw + 4, temp_raw + 5);
	if (num_tokens != ETH_ALEN)
		return FAILURE;

	for (i = 0; i < num_tokens; i++)
		raw[i] = (t_u8) temp_raw[i];

	if (memcmp(raw, "\xff\xff\xff\xff\xff\xff", ETH_ALEN) == 0)
		return WIFIDIRECT_RET_MAC_BROADCAST;
	else if (raw[0] & 0x01)
		return WIFIDIRECT_RET_MAC_MULTICAST;
	return SUCCESS;
}


/**
 *  @brief Parses a command line
 *
 *  @param line     The line to parse
 *  @param args     Pointer to the argument buffer to be filled in
 *  @return         Number of arguments in the line or EOF
 */
static int
parse_line(char *line, char *args[])
{
	int arg_num = 0;
	int is_start = 0;
	int is_quote = 0;
	int length = 0;
	int i = 0;

	arg_num = 0;
	length = strlen(line);
	/* Process line */

	/* Find number of arguments */
	is_start = 0;
	is_quote = 0;
	for (i = 0; i < length; i++) {
		/* Ignore leading spaces */
		if (is_start == 0) {
			if (line[i] == ' ') {
				continue;
			} else if (line[i] == '\t') {
				continue;
			} else if (line[i] == '\n') {
				break;
			} else {
				is_start = 1;
				args[arg_num] = &line[i];
				arg_num++;
			}
		}
		if (is_start == 1) {
			/* Ignore comments */
			if (line[i] == '#') {
				if (is_quote == 0) {
					line[i] = '\0';
					arg_num--;
				}
				break;
			}
			/* Separate by '=' */
			if (line[i] == '=') {
				line[i] = '\0';
				is_start = 0;
				continue;
			}
			/* Separate by ',' */
			if (line[i] == ',') {
				line[i] = '\0';
				is_start = 0;
				continue;
			}
			/* Change ',' to ' ', but not inside quotes */
			if ((line[i] == ',') && (is_quote == 0)) {
				line[i] = ' ';
				continue;
			}
		}
		/* Remove newlines */
		if (line[i] == '\n')
			line[i] = '\0';
		/* Check for quotes */
		if (line[i] == '"') {
			is_quote = (is_quote == 1) ? 0 : 1;
			continue;
		}
		if (((line[i] == ' ') || (line[i] == '\t')) &&
					(is_quote == 0)) {
			line[i] = '\0';
			is_start = 0;
			continue;
		}
	}
	return arg_num;
}


#endif /* _WIFI_COMMON_H */
