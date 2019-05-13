/** @file dumphex.c
 *
 *  @brief This file provides  Hex Dump API
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

#include <ctype.h>
#include <wmstdio.h>

#ifdef CONFIG_DEBUG_BUILD
#define DUMP_WRAPAROUND 16
void dump_hex(const void *data, unsigned len)
{
	wmprintf("**** Dump @ %p Len: %d ****\n\r", data, len);

	int i;
	const char *data8 = (const char *)data;
	for (i = 0; i < len;) {
		wmprintf("%02x ", data8[i++]);
		if (!(i % DUMP_WRAPAROUND))
			wmprintf("\n\r");
	}

	wmprintf("\n\r******** End Dump *******\n\r");
}

void dump_hex_ascii(const void *data, unsigned len)
{
	wmprintf("**** Dump @ %p Len: %d ****\n\r", data, len);

	int i;
	const unsigned char *data8 = (const unsigned char *)data;
	for (i = 0; i < len;) {
		wmprintf("%02x", data8[i]);
		if (!iscntrl(data8[i]))
			wmprintf(" == %c  ", data8[i]);
		else
			wmprintf(" ==    ");

		i++;
		if (!(i % DUMP_WRAPAROUND / 2))
			wmprintf("\n\r");
	}

	wmprintf("\n\r******** End Dump *******\n\r");
}

void dump_ascii(const void *data, unsigned len)
{
	wmprintf("**** Dump @ %p Len: %d ****\n\r", data, len);

	int i;
	const unsigned char *data8 = (const unsigned char *)data;
	for (i = 0; i < len;) {
		if (isprint(data8[i]))
			wmprintf("%c", data8[i++]);
		else
			wmprintf("(%02x)", data8[i++]);
		if (!(i % DUMP_WRAPAROUND))
			wmprintf("\n\r");
	}

	wmprintf("\n\r******** End Dump *******\n\r");
}

void print_ascii(const void *data, unsigned len)
{
	wmprintf("**** Print @ %p Len: %d ****\n\r", data, len);

	int i;
	const char *data8 = (const char *)data;
	for (i = 0; i < len;) {
		wmprintf("%c", data8[i++]);
	}

	wmprintf("\n\r******** End Print *******\n\r");
}


void dump_json(const void *buffer, unsigned len)
{
	wmprintf("**** Print @ %p Len: %d ****\n\r", buffer, len);

	int i;
	int indentation_level = 0;
	const char *data8 = (const char *)buffer;
	for (i = 0; i < len; i++) {
		if (data8[i] == '{') {
			wmprintf("\r\n");
			int l_indent_level = indentation_level;
			while (l_indent_level--)
				wmprintf(" ");
			wmprintf("{\r\n");
			indentation_level += 2;
			continue;
		}

		if (data8[i] == '}') {
			wmprintf("\r\n");
			indentation_level -= 2;
			int l_indent_level = indentation_level;
			while (l_indent_level--)
				wmprintf(" ");
			wmprintf("}\r\n");
			continue;
		}

		if (data8[i] == ',') {
			wmprintf(",\r\n");
			continue;
		}

		wmprintf("%c", data8[i]);
	}

	wmprintf("\n\r******** End Print *******\n\r");
}
#endif /* CONFIG_DEBUG_BUILD */
