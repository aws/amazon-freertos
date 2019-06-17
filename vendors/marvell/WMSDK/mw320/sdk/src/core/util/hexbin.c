/** @file hexbin.c
 *
 *  @brief This file provides  Hex Bin utility API
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


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wmstdio.h>
#include <wmlog.h>

unsigned int hex2bin(const uint8_t *ibuf, uint8_t *obuf,
		     unsigned max_olen)
{
	unsigned int i;		/* loop iteration variable */
	unsigned int j = 0;	/* current character */
	unsigned int by = 0;	/* byte value for conversion */
	unsigned char ch;	/* current character */
	unsigned int len = strlen((char *)ibuf);
	/* process the list of characters */
	for (i = 0; i < len; i++) {
		if (i == (2 * max_olen)) {
			wmlog("hexbin", "Destination full. "
				"Truncating to avoid overflow.\r\n");
			return j + 1;
		}
		ch = toupper(*ibuf++);	/* get next uppercase character */

		/* do the conversion */
		if (ch >= '0' && ch <= '9')
			by = (by << 4) + ch - '0';
		else if (ch >= 'A' && ch <= 'F')
			by = (by << 4) + ch - 'A' + 10;
		else {		/* error if not hexadecimal */
			return 0;
		}

		/* store a byte for each pair of hexadecimal digits */
		if (i & 1) {
			j = ((i + 1) / 2) - 1;
			obuf[j] = by & 0xff;
		}
	}
	return j + 1;
}

void bin2hex(uint8_t *src, char *dest, unsigned int src_len,
	     unsigned int dest_len)
{
	int i;
	for (i = 0; i < src_len; i++) {
		if (snprintf(dest, dest_len, "%02x", src[i]) >= dest_len) {
			wmlog("hexbin", "Destination full. "
				"Truncating to avoid overflow.\r\n");
			return;
		}
		dest_len -= 2;
		dest += 2;
	}
}
