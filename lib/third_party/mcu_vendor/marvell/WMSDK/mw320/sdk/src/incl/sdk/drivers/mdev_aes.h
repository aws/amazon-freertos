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


/*! \file mdev_aes.h
 *  \brief AES Driver
 *
 * The AES driver handles the AES hardware accelerator and provides an API
 * to the user for AES encryption and decryption.
 *
 * \note This AES driver does not support interrupt based AES operations.
 *
 * Follow the steps below to use AES driver from applications
 *
 * 1. AES driver initialization
 *
 * Call \ref aes_drv_init  from your application initialization function.
 * This will initialize and register aes driver with the mdev
 * interface.
 *
 * 2. Open AES device to use
 *
 * \ref aes_drv_open should be called once before using AES hardware for
 * AES operations. Here you will specify the name of the AES
 * module. There is only one hardware engine. Hence, you should pass
 * MDEV_AES_0 here.
 *
 * Typical example:
 *
 * Opening AES device
 * \code
 * {
 *
 *     mdev_t *aes_dev;
 *
 *	aes_dev = aes_drv_open("MDEV_AES_0");
 *
 * }
 * \endcode
 *
 * 3. Perform AES operations.
 *
 * AES API can is used for encryption and decryption of plain text and
 * cipher text respectively. Before any operation first a key, relevant to
 * that operation needs to be set.
 *
 * \note Please ensure that the size of the plain/cipher text sent to the
 * encryption/decryption functions are aligned to AES_BLOCK_SIZE
 *
 * Typical example:
 *
 * Encryption:
 * \code
 * {
 *     unsigned char const key[] = {
 *      0xc2, 0x86, 0x69, 0x6d,
 *      0x88, 0x7c, 0x9a, 0xa0,
 *	 0x61, 0x1b, 0xbb, 0x3e,
 *	 0x20, 0x25, 0xa4, 0x5a
 *   };
 *
 * unsigned char const initial_vector[] = {
 *     0x56, 0x2e, 0x17, 0x99,
 *	0x6d, 0x09, 0x3d, 0x28,
 *     0xdd, 0xb3, 0xba, 0x69,
 *     0x5a, 0x2e, 0x6f, 0x58
 *    };
 *
 * char plain[1024];
 * char cipher[1024];
 *
 *      aes_t enc_aes;
 *      aes_drv_init();
 *      mdev_t *aes_dev = aes_drv_open(MDEV_AES_0);
 *      aes_drv_setkey(aes_dev, &enc_aes, &key, sizeof(key),
 *            &initial_vector, AES_ENCRYPTION, HW_AES_MODE_CBC);
 *
 *      aes_drv_encrypt(aes_dev, &enc_aes, plain, cipher, sizeof(plain));
 *      aes_drv_close(aes_dev);
 * }
 * \endcode
 *
 * 4. Closing a device.
 *
 * A call to \ref aes_drv_close informs the AES driver to release the
 * resources that it is holding. It is always good practice to free the
 * resources after use.
 *
 * Typical usecase:
 * \code
 * aes_drv_close(aes_dev);
 * \endcode
 */

#ifndef _MDEV_AES_H_
#define _MDEV_AES_H_

#include <mdev.h>

#define MDEV_AES_0          "MDEV_AES_0"

#ifndef AES_MACROS_DEFINED
enum {
	AES_ENC_TYPE   = 1,   /* cipher unique type */
	AES_ENCRYPTION = 0,
	AES_DECRYPTION = 1,
	AES_BLOCK_SIZE = 16
};

#define AES_MACROS_DEFINED
#endif

/** Mode in which HW AES is used */
typedef enum {
	/** AES mode: ECB */
	HW_AES_MODE_ECB = 0,
	/** AES mode: CBC */
	HW_AES_MODE_CBC = 1,
	/** AES mode: CTR */
	HW_AES_MODE_CTR = 2,
	/** AES mode: CCM */
	HW_AES_MODE_CCM = 5,
	/** AES mode: MMO */
	HW_AES_MODE_MMO = 6,
	/** AES mode: Bypass */
	HW_AES_MODE_BYPASS = 7,
} hw_aes_mode_t;

/* CTR mode's counter modular */
#define AES_CTR_MOD_2_POW_128 0x00

/*
 * Note that though this structure is open for all to see, it should not be
 * edited by the caller. It is solely for the use of the AES driver. As
 * shown in above example, it needs to be allocated by the caller.
 */
typedef struct {
	/*
	 * The following parameters will be cached in the call to *setkey*.
	 * Ensure that this remains in sync with AES_Config_Type. Only
	 * required members are cached. Add if necessary in future.
	 */
	int mode;
	/* of the type AES_EncDecSel_Type. Use directly */
	int dir;
	/* Already processed as required by h/w */
	uint32_t initVect[4];
	uint32_t keySize;
	/* Already processed as required by h/w */
	uint32_t saved_key[8];
	int micLen;
	int micEn;

	/* For ctr mode */
	uint32_t ctr_mod; /* counter modular */
	bool data_buffered;
	uint8_t buffered_data[AES_BLOCK_SIZE];
	uint8_t bufferCnt;
	uint8_t buf_idx;

	/* for CCM mode */
	uint32_t aStrLen; /* associated auth. data len */
} aes_t;

/**
 * Init AES Driver
 *
 * This function registers the AES driver with the mdev interface.
 *
 * \return WM_SUCCESS on success, error code otherwise.
 *
 */
int aes_drv_init(void);

/**
 *  Open AES Device
 *
 * This function opens the device for use and returns a handle. This handle
 * should be used for other calls to the AES driver.
 *
 * \param[in] name Name of the driver to be opened. It should be MDEV_AES_0.
 *
 * \return NULL if error, mdev_t handle otherwise.
 */
mdev_t *aes_drv_open(const char *name);

/**
 * Set the AES key
 *
 * \param[in] dev pointer returned in earlier call to \ref aes_drv_open
 * \param[in] aes Pointer to the structure allocated by \b caller. Please
 * do not touch this structure after return from \ref aes_drv_setkey. Pass it
 * unchanged to \ref aes_drv_encrypt or \ref aes_drv_decrypt
 * \param[in] userKey  AES key. Can either is 16 bytes, 24 bytes or 32
 * bytes. Any other value is invalid.
 * \param[in] key_len The length of the key in bytes.
 * \param[in] IV The initialization vector.
 * \param[in] dir Is either AES_ENCRYPTION or AES_DECRYPTION
 * \param[in] hw_mode One of the enum \ref hw_aes_mode_t
 *
 * \return Standard SDK return codes.
 */
int aes_drv_setkey(mdev_t *dev, aes_t *aes, const uint8_t *userKey,
		   uint32_t key_len, const uint8_t *IV, int dir,
		   hw_aes_mode_t hw_mode);

/**
 * Set the AES key for CCM.
 *
 * \param[in] dev pointer returned in earlier call to \ref aes_drv_open
 * \param[in] aes Pointer to the structure allocated by \b caller. Please
 * do not touch this structure after return from \ref aes_drv_ccm_setkey.
 * Pass it unchanged to \ref aes_drv_ccm_encrypt or \ref aes_drv_ccm_decrypt
 * \param[in] userKey  AES key. Can either is 16 bytes, 24 bytes or 32
 * bytes. Any other value is invalid.
 * \param[in] key_len The length of the key in bytes.
 *
 * \return Standard SDK return codes.
 */
int aes_drv_ccm_setkey(mdev_t *dev, aes_t *aes, const uint8_t *userKey,
		       uint32_t key_len);

/**
 * Encrypt the given plain text based on the key set earlier.
 *
 * \param[in] dev The pointer returned in earlier call to \ref aes_drv_open
 * \param[in] aes The same object sent to the function \ref aes_drv_setkey
 * unchanged
 * \param[in] plain Plain text
 * \param[out] cipher Cipher text returned by this function.
 * \param[in] sz The total length of the plain text
 *
 * \return Standard SDK return codes.
 */
int aes_drv_encrypt(mdev_t *dev, aes_t *aes, const uint8_t *plain,
		    uint8_t *cipher, uint32_t sz);

/**
 * Encrypt the given plain text based on the key set earlier.
 *
 * \param[in] dev The pointer returned in earlier call to \ref aes_drv_open
 * \param[in] aes The same object sent to the function \ref aes_drv_ccm_setkey
 * unchanged
 * \param[in] plain Plain text to be encrypted
 * \param[out] cipher Cipher text.
 * \param[in] sz The total length of the plain text in bytes
 * \param[in] nonce The initialization vector
 * \param[in] nonceSz The size in bytes of the init vector. In CCM mode,
 * length of nonce ranges from 11 bytes to 13 bytes.
 * \param[out] authTag The MIC will be returned in this buffer
 * \param[in] authTagSz Size in bytes of MIC required.
 * \param[in] authIn The additional authenticated data (AAD).
 * \param[in] authInSz Size in bytes of the AAD.
 *
 * \return Standard SDK return codes.
 * \note Iterative calls for encrypting data in chunked fashion is not
 * supported by this function, i.e. entire input (AAD + Plain Text) should be
 * provided in single call.
 */
int aes_drv_ccm_encrypt(mdev_t *dev, aes_t *aes, const char *plain,
			char *cipher, uint32_t sz,
			const char *nonce, uint32_t nonceSz,
			char *authTag, uint32_t authTagSz,
			const char *authIn, uint32_t authInSz);

/**
 * Decrypt the given cipher text based on the key set earlier.
 *
 * \param[in] dev The pointer returned in earlier call to \ref aes_drv_open
 * \param[in] aes The same object sent to the function \ref aes_drv_setkey
 * unchanged
 * \param[in] cipher Cipher text.
 * \param[out] plain Plain text returned by this function.
 * \param[in] sz The total length of the plain text
 *
 * \return Standard SDK return codes.
 */
int aes_drv_decrypt(mdev_t *dev, aes_t *aes, const uint8_t *cipher,
		    uint8_t *plain, uint32_t sz);

/**
 * Decrypt the given cipher text based on the key set earlier.
 *
 * \param[in] dev The pointer returned in earlier call to \ref aes_drv_open
 * \param[in] aes The same object sent to the function \ref aes_drv_ccm_setkey
 * unchanged
 * \param[in] cipher Cipher text.
 * \param[out] plain Plain text returned by this function.
 * \param[in] sz The total length of the cipher text in bytes
 * \param[in] nonce The initialization vector
 * \param[in] nonceSz The size in bytes of the init vector. In CCM mode,
 * length of nonce ranges from 11 bytes to 13 bytes.
 * \param[in] authTag The input MIC for verification
 * \param[in] authTagSz Size in bytes of input MIC
 * \param[in] authIn The additional authenticated data (AAD).
 * \param[in] authInSz Size in bytes of the AAD.
 *
 * \return Standard SDK return codes.
 * \note Iterative calls for decrypting data in chunked fashion is not
 * supported by this function, i.e. entire input (AAD + Cipher Text + MIC)
 * should be provided in single call.
 */
int aes_drv_ccm_decrypt(mdev_t *dev, aes_t *aes, const char *cipher,
			char *plain, uint32_t sz,
			const char *nonce, uint32_t nonceSz,
			const char *authTag, uint32_t authTagSz,
			const char *authIn, uint32_t authInSz);

/**
 * Function clears any buffered data for unaligned block encryption to start
 * encryption of an entirely new block.
 *
 * \param[in] aes The pointer to the aes_t structure.
 */
void aes_ctr_final(aes_t *aes);

/**
 * Close the device instance.
 *
 * \param[in] dev The pointer returned in earlier call to \ref aes_drv_open
 *
 * \note Calling this function (after your work with AES engine is over) is
 * essential. Failing to do so will result in failure of the AES driver.
 *
 * \return void
 */
void aes_drv_close(mdev_t *dev);

#endif /* !_MDEV_AES_H_ */
