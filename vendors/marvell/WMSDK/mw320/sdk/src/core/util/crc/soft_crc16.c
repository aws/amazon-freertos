/** @file soft_crc16.c
 *
 *  @brief This file provides  CRC 16 API
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


#include <wm_utils.h>
#include <wmassert.h>

#define CRC_16_CCITT 0x8408	/*!< CRC mode: CRC-16-CCITT */
#define TABLE_SIZE 16

static uint32_t crc_table[TABLE_SIZE];
static bool init_done;

void soft_crc16_init(void)
{
	uint16_t crc = 0;
	uint8_t i;
	uint8_t j;

	if (init_done)
		return;

	for (j = 0; j < TABLE_SIZE; j++) {
		crc = 0;
		for (i = 0x01; i != 0x10; i <<= 1) {
			if ((crc & 0x0001) != 0) {
				crc >>= 1;
				crc ^= CRC_16_CCITT;
			} else {
				crc >>= 1;
			}
			if ((j & i) != 0) {
				crc ^= CRC_16_CCITT;
			}
		}
		crc_table[j] = crc;
	}
	init_done = true;
}

uint32_t soft_crc16(const void *__buf, uint16_t len, uint32_t crc_init)
{
	const uint8_t *buf = __buf;
	uint8_t crc_H8;
	uint16_t crc = (uint16_t) crc_init;

	ASSERT(init_done == 1);

	while (len--) {
		crc_H8 = (uint8_t)(crc & 0x000F);
		crc >>= 4;
		crc ^= crc_table[crc_H8 ^ (*buf & 0x0F)];
		crc_H8 = (uint8_t)(crc & 0x000F);
		crc >>= 4;
		crc ^= crc_table[crc_H8 ^ (*buf >> 4)];
		buf++;
	}

	return (uint32_t)crc;
}

