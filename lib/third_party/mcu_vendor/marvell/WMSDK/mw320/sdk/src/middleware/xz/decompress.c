/** @file decompress.c
*
*  @brief Uncompress API
*
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



#include "xz.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wmerrno.h>
#include <wmstdio.h>

static struct xz_dec *s;

int xz_uncompress_init(struct xz_buf *stream, uint8_t *sbuf, uint8_t *dbuf)
{
	xz_crc32_init();

        /*
         * Support up to 32 KiB dictionary. The actually needed memory
         * is allocated once the headers have been parsed.
         */
        s = xz_dec_init(XZ_DYNALLOC, 1 << 15);
        if (s == NULL) {
                return -WM_FAIL;
        }

	stream->in = sbuf;
	stream->in_pos = 0;
	stream->in_size = 0;
	stream->out = dbuf;
	stream->out_pos = 0;
	stream->out_size = 0;

	return WM_SUCCESS;
}

int xz_uncompress_stream(struct xz_buf *stream, uint8_t *sbuf, uint32_t slen,
		uint8_t *dbuf, uint32_t dlen, uint32_t *decomp_len)
{
	int status;
	*decomp_len = 0;

	if (stream->in_pos == stream->in_size) {
		stream->in_size = slen;
	        stream->in_pos = 0;
	}

	if (stream->out_pos == stream->out_size) {
	        stream->out_size = dlen;
		stream->out_pos = 0;
	}

	status = xz_dec_run(s, stream);

	if ((status == XZ_STREAM_END) || (stream->out_pos == stream->out_size))
		*decomp_len = stream->out_pos;

	return status;
}

void xz_uncompress_end()
{
	xz_dec_end(s);
}
