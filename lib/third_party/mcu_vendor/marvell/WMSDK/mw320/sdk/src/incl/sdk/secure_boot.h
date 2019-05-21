/** @file secure_boot.h
*
*  @brief Secure Algo Enums
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

#ifndef __SECURE_BOOT_H__
#define __SECURE_BOOT_H__

#include <wmtlv.h>
#include <keystore.h>

/* End of SRAM0 address where boot-loader will copy keystore
 * Note: Boot loader refers to this macro and same address space is reserved
 * in linker script of application fw as well.
 * Important point to remember here is, this should note overlap with
 * boot-loader CODE/DATA/BSS region, otherwise that may result in undefined
 * behaviour.
 * Please do not modify.
 */
#define SB_KEYSTORE_ADDR ((volatile uint32_t *)(0x15F000))
#define SB_KEYSTORE_SIZE (4 * 1024)
#define SB_SEC_IMG_HDR_SIZE (512)

typedef struct tlv_hdr img_sec_hdr, boot2_ks_hdr;

enum signing_algo_t {
	NO_SIGN = 0,
	RSA_SIGN,
};

enum hash_algo_t {
	NO_HASH = 0,
	SHA256_HASH,
};

enum encrypt_algo_t {
	NO_ENCRYPT = 0,
	AES_CCM_256_ENCRYPT,
	AES_CTR_128_ENCRYPT,
};

#endif /* __SECURE_BOOT_H__ */
