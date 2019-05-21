/** @file wmtlv.h
*
*  @brief TLV (Type-Length-Value) module
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

/*! \file wmtlv.h
 *  \brief TLV (Type-Length-Value) module
 *
 * TLV module is the low-level module on top of which keystore is based.
 * (For details of keystore, refer to keystore.h). This module manages
 * the pre-defined TLV types.
 */


#ifndef __TLV_H__
#define __TLV_H__

#include <stdint.h>
#include <compat_attribute.h>

#define IS_VALID_TLV_TYPE(tlv_type) (((tlv_type) >= 0 && (tlv_type) <= 255) ? \
					1 : 0)
#define XIP_ALIGN_SIZE 4

__PACKED_START__
struct tlv_hdr {
	uint32_t magic;
	uint32_t len;
	uint32_t crc;
}  __PACKED_END__;

__PACKED_START__
struct tlv_entry {
	uint8_t type;
	uint16_t len;
	uint8_t value[];
} __PACKED_END__;

enum tlv_store_type {
	BOOT2_KEYSTORE = 0,
	IMAGE_SECURE_HEADER,
	CONFIG_FILE_STORE
};

#define SEC_KEYSTORE_MAGIC_SIG (('S' << 0)|('B' << 8)|('K' << 16)|('S' << 24))
#define SEC_FW_MAGIC_SIG (('S' << 0)|('B' << 8)|('F' << 16)|('H' << 24))
#define SEC_DEFAULT_MAGIC_SIG 0x00

#define TLV_SUCCESS			0
#define ERR_TLV_MAGIC_FAILURE		-1
#define ERR_TLV_CRC_FAILURE		-2
#define ERR_TLV_BAD_BUF_LEN		-3
#define ERR_TLV_NOT_FOUND		-4
#define ERR_TLV_BAD_LENGTH		-5
#define ERR_TLV_INSUFFICIENT_MEMORY	-6
#define ERR_TLV_INVALID_TYPE		-7
#define ERR_TLV_INVALID_VALUE		-7

/* Following apis should not be used directly for accessing keystore.
 * Use keystore APIs instead. Refer to keystore.h for details.
 */
typedef uint32_t (*crc_func_t) (const void *data, int data_size,
			uint32_t crc);

int tlv_store_init(uint8_t *tlv_buf, uint32_t max_size, uint8_t store_type);
int tlv_store_add(uint8_t *tlv_buf, uint8_t type, uint16_t len,
			const uint8_t *value);
int tlv_store_close(uint8_t *tlv_buf, crc_func_t crc_func);
int tlv_store_length(uint8_t *tlv_buf);
int tlv_store_validate(uint8_t *tlv_buf, uint8_t store_type,
			crc_func_t crc_func);
int tlv_store_get(uint8_t *tlv_buf, uint8_t type, uint16_t len, uint8_t *value);
int tlv_store_get_ref(uint8_t *tlv_buf, uint8_t type, uint16_t *plen,
			const uint8_t **pvalue);
int tlv_store_iterate(uint8_t *tlv_buf, uint8_t *ptype, uint16_t len,
			uint8_t *value);
int tlv_store_iterate_ref(uint8_t *tlv_buf, uint8_t *ptype, uint16_t *plen,
			const uint8_t **pvalue);
int tlv_store_xip_padding(uint8_t *tlv_buf, uint8_t *pad_value);

#endif /* __TLV_H__ */
