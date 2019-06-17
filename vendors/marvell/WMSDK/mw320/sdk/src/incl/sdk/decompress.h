/** @file decompress.h
*
*  @brief Decompress Header
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


#ifndef __DECOMPRESS_H__
#define __DECOMPRESS_H__

#include <wmtypes.h>
#include <xz.h>

int xz_uncompress_init(struct xz_buf *stream, uint8_t *sbuf, uint8_t *dbuf);
int xz_uncompress_stream(struct xz_buf *stream, uint8_t *sbuf, uint32_t slen,
		uint8_t *dbuf, uint32_t dlen, uint32_t *decomp_len);
void xz_uncompress_end();

#endif /* __DECOMPRESS_H__ */
