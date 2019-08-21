/** @file psm-v2-secure.h
*
*  @brief Persistent storage manager version 2 (secure)
*
*  This file is used in case of Secure PSM. The idea is to have easily
*  replacable security function. The security functions declared in this
*  file need to be implemented. There can be mupliple versions. One of them
*  can be selected at compile time.

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


#include <psm-v2.h>

typedef void *psm_sec_hnd_t;

/** Enum values which will be passed back in callback function 'resetkey' */
typedef enum {
	SET_ENCRYPTION_KEY = 0,
	SET_DECRYPTION_KEY = 1,
} psm_resetkey_mode_t;

/**
 * Security Init.
 *
 * This function should  perform the initialization required for psm
 * encryption and decryption operations. This should do everything that is
 * required to enable encryption and decryption operations later. This
 * function is invoked when psm_module_init() is invoked.
 *
 * @param[out] sec_handle Pointer to handle. Will be populated by
 * psm_security_init()
 *
 * @return User defined value typically WM_SUCCESS or -WM_FAIL. Any
 * failure here will abort the ongoing PSM operation.
 */
int psm_security_init(psm_sec_hnd_t *sec_handle);

/**
 * Security Init with key from caller.
 *
 * This function is similar to psm_security_init() with key from caller.
 * This function should  perform the initialization required for psm
 * encryption and decryption operations. This should do everything that is
 * required to enable encryption and decryption operations later. This
 * function is invoked when psm_module_init() is invoked.
 *
 * @param[out] sec_handle Pointer to handle. Will be populated by
 * psm_security_init_with_key()
 * @param[in] psm_cfg Pointer to psm_cfg_t where it contains key/iv to be
 * used by security encryption/decryption operations. PSMv2 won't need user
 * copy of any field in psm_cfg after call returns.
 *
 * @return User defined value typically WM_SUCCESS or -WM_FAIL. Any
 * failure here will abort the ongoing PSM operation.
 */
int psm_security_init_with_key(psm_sec_hnd_t *sec_handle, psm_cfg_t *psm_cfg);

/**
 * Specify if encryption/decryption key is present in keystore or not.
 *
 * This function returns true if the key is present in
 * keystore, otherwise returns false.
 *
 * @return True if encryption/decryption key is present in keystore,
 * otherwise false.
 */
bool psm_is_key_present_in_keystore();

/**
 * Reset PSM key.
 *
 * This function should reset the encryption/decryption key and/or
 * nonce. This function will be called at the start of operation on every
 * PSM object.
 *
 * Every PSM object is regarded as a independant encryption/decryption unit
 * by the PSM. Hence, this function will be called at the start of
 * processing of any new object.
 *
 * @param[in] sec_handle The same handle returned by psm_security_init().
 * @param[in] mode The exact operation that will happen later for this
 * key.
 *
 * @return User defined value typically WM_SUCCESS or -WM_FAIL. Any
 * failure here will abort the ongoing PSM operation.
 */
int psm_key_reset(psm_sec_hnd_t sec_handle, psm_resetkey_mode_t mode);

/**
 * Encrypt PSM operation.
 *
 * This function should perform the encryption of psm data passed as a
 * parameter.
 *
 * @param[in] sec_handle The same handle returned by psm_security_init().
 * @param[in] plain Pointer to data to be encrypted (plain text)
 * @param[out] cipher Pointer to encrypted data (cipher text).
 * @param[in] len Length of data to be encrypted.
 *
 * @return User defined value typically WM_SUCCESS or -WM_FAIL. Any
 * failure here will abort the ongoing PSM operation.
 */
int psm_encrypt(psm_sec_hnd_t sec_handle, const void *plain, void *cipher,
		uint32_t len);

/**
 * Decrypt PSM operation.
 *
 * This function should perform the decryption of psm data passed as a
 * parameter.
 *
 * @param[in] sec_handle The same handle returned by psm_security_init().
 * @param[in] cipher Pointer to data to be decrypted (cipher text)
 * @param[out] plain Pointer to buffer where decrypted data (plain
 * text) should be stored. Note that cipher and plain point to
 * same buffer i.e. PSM-v2 expects in place decryption.
 * @param[in] len Length of data to be decrypted.
 *
 * @return User defined value typically WM_SUCCESS or -WM_FAIL. Any
 * failure here will abort the ongoing PSM operation.
 */
int psm_decrypt(psm_sec_hnd_t sec_handle, const void *cipher, void *plain,
		uint32_t len);

/**
 * Security de-init
 *
 * This function should perform the de-initialization required to
 * free up the resources used for psm encryption and decryption
 * operations. After this API no other security related function will be
 * invoked. The implementation can free up any resources it is using. This
 * function is invoked when psm_module_deinit() is invoked.
 *
 * @param[in] sec_handle Pointer to the same handle returned by
 * psm_security_init().
 */
void psm_security_deinit(psm_sec_hnd_t *sec_handle);
