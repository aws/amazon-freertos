/*
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


/*! @file keystore.h
 * @brief Keystore access module (This module is available only for MW30X
 * devices)
 *
 * Keystore is a component of bootloader that is passed by it to application
 * firmware while booting. Usually, bootloader will be encrypted and hence
 * keystore acts as read-only secure storage on flash to store important
 * information such as encryption/decryption keys for secure PSM or any
 * application specific secure information. Bootloader passes on this
 * keystore to application firmware in decrypted form in RAM.
 *
 * Keystore is basically a packed array of TLV (Type-Length-Value) triplets.
 * There is a header at the start of keystore which is used to validate and
 * traverse keystore. Header is followed by one or more TLV triplet, each of
 * different type and different lengths. TLVs of keystore are divided into two
 * categories, TLV Types 0-95 are reserved for secure boot process and TLV Types
 * 96-254 are reserved for applications for custom use, including that for
 * secure PSM.
 *
 * This module provides a simple interface to access keystore from application
 * firmware.
 *
 * @note Keystore is a read-only storage and cannot be updated in memory or
 * on flash by application firmware. It can only be done by flashing new
 * bootloader with updated keystore.
 *
 * @section keystore_usage Usage
 *
 * -# Use keystore_get to get a copy of TLV value stored in keystore
 * -# Use keystore_get_ref to get in-place pointer to TLV value in keystore
 * -# To iterate through all TLV's stored in keystore
 * @code
 * uint8_t val[256];
 * uint8_t type;
 * uint16_t len;
 * int ret;
 *
 * keystore_iterate_start();
 * while(1) {
 *     ret = keystore_iterate(&type, sizeof(val), val);
 *     if (ret == ERR_TLV_NOT_FOUND)
 *         break;
 *     if (ret == ERR_TLV_BAD_BUF_LEN) {
 *         log("Use_larger_val_buffer");
 *         break;
 *     }
 *     len = ret;
 *     use_tlv(type, len, val);
 * }
 * @endcode
 */


#ifndef __KEYSTORE_H__
#define __KEYSTORE_H__

#include <wmtlv.h>

/** Predefined TLV key types for keystore. Note that TLV types must be unique
 * throughout the system to avoid any confusion.
 * Applications must use a unique integer in the range KEY_APP_DATA_START
 * and KEY_APP_DATA_END as TLV type for its use.
 */
enum ks_tlv_type {
	/* Start of Reserved TLV types. Applications must not modify them */
	/* Boot2 key types */
	KEY_BOOT2_SIGNING_ALGO = 0,
	KEY_BOOT2_HASH_ALGO,
	KEY_BOOT2_ENCRYPT_ALGO,
	KEY_BOOT2_PRIVATE_KEY,
	KEY_BOOT2_PUBLIC_KEY,
	KEY_BOOT2_ENCRYPT_KEY,
	KEY_BOOT2_DECRYPT_KEY,
	KEY_BOOT2_NONCE,
	KEY_BOOT2_LOAD_ADDR,
	KEY_BOOT2_ENTRY_ADDR,

	/* OTP key types */
	KEY_OTP_JTAG_ENABLE = 24,
	KEY_OTP_UART_USB_BOOT_ENABLE,
	KEY_OTP_USER_DATA,

	/* Firmware key types */
	KEY_FW_SIGNING_ALGO = 32,
	KEY_FW_HASH_ALGO,
	KEY_FW_ENCRYPT_ALGO,
	KEY_FW_PRIVATE_KEY,
	KEY_FW_PUBLIC_KEY,
	KEY_FW_ENCRYPT_KEY,
	KEY_FW_DECRYPT_KEY,
	KEY_FW_NONCE,
	KEY_FW_IMAGE_LEN,

	/* Firmware Image Header key types */
	KEY_FW_MIC = 64,
	KEY_FW_SIGNATURE,

	/* End of Reserved TLV types */

	/** Application specific TLV type must be greater than or equal to this
	 * value (= 96) */
	KEY_APP_DATA_START = 96,

	/* PSMv2 */
	KEY_PSM_ENCRYPT_KEY = 97,
	KEY_PSM_NONCE = 98,
	/** Application specific TLV type must be less than or equal to this
	 * value (= 254)*/
	KEY_APP_DATA_END = 254,

	/* Filler or padding data */
	KEY_FILLER = 255
};


/** This function retrives a TLV from keystore.
 *
 * @param[in] type TLV type to retrieve. Refer to enum \ref ks_tlv_type
 * for pre-defined values
 * @param[in] len Length of the buffer in which to copy TLV value
 * @param[out] value Buffer in which to copy TLV value. Can be NULL
 * to retrive the the length of TLV value
 *
 * @return Length of TLV value if TLV type found (can be 0)
 * @return ERR_TLV_BAD_BUF_LEN if buffer length is smaller than TLV value size
 * @return ERR_TLV_NOT_FOUND if TLV type not found
 */
int keystore_get(uint8_t type, uint16_t len, uint8_t *value);

/** This function retrives a TLV from keystore. Unlike keystore_get()
 * this function returns reference of TLV value, rather than copy into
 * a separate buffer.
 *
 * @note Application must not modify this TLV value.
 *
 * @param[in] type TLV type to retrieve. Refer to enum \ref ks_tlv_type
 * for pre-defined values
 * @param[out] plen Length of TLV value (can be 0)
 * @param[out] pvalue Pointer within keystore to TLV value of size *plen.
 * Can be NULL to retrive the the length of TLV value
 *
 * @return Length of TLV value if TLV type found
 * @return ERR_TLV_NOT_FOUND if TLV type not found
 */
int keystore_get_ref(uint8_t type, uint16_t *plen, const uint8_t **pvalue);

/** This function enables to iterate over each TLV stored in keystore from
 * beginning. This must be called once before keystore_iterate() or
 * keystore_iterate_ref().
 *
 * @note This function also enables to restart iteration from beginning.
 */
void keystore_iterate_start();

/** This function retrives each TLV from keystore in the sequence in which
 * they are stored. TLV is copied into the buffer supplied.
 *
 * @pre keystore_iterate_start()
 *
 * @param[out] ptype TLV type retrived. Refer to enum \ref ks_tlv_type
 * for pre-defined values
 * @param[in] len Length of the buffer in which to copy TLV value
 * @param[out] value Buffer in which to copy TLV value
 *
 * @return Length of TLV value if TLV type found
 * @return ERR_TLV_BAD_BUF_LEN if buffer length is smaller than TLV value size
 * @return ERR_TLV_NOT_FOUND if end of keystore reached
 */
int keystore_iterate(uint8_t *ptype, uint16_t len, uint8_t *value);

/** This function is keystore_get_ref() equivalent of keystore_iterate().
 * TLV value is returned as reference into keystore rather than separate buffer
 * copy.
 *
 * @note Application must not modify this TLV value.
 *
 * @pre keystore_iterate_start()
 *
 * @param[out] ptype TLV type retrieved. Refer to enum \ref ks_tlv_type
 * for pre-defined values
 * @param[out] plen Length of TLV value (can be 0)
 * @param[out] pvalue Pointer within keystore to TLV value of size *plen
 *
 * @return Length of TLV value if TLV type found
 * @return ERR_TLV_NOT_FOUND if end of keystore reached
 */
int keystore_iterate_ref(uint8_t *ptype, uint16_t *plen,
			const uint8_t **pvalue);

#endif /* __KEYSTORE_H__ */
