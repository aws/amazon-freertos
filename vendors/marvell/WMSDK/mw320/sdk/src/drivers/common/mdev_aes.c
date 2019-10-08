/** @file mdev_aes.c
 *
 *  @brief This file provides  mdev driver for AES
 *
 *  (C) Copyright 2014-2019 Marvell International Ltd. All Rights Reserved.
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


/* mdev_aes.c: mdev driver for AES
 */
#include <wmtypes.h>
#include <wm_os.h>
#include <lowlevel_drivers.h>
#include <mdev_aes.h>
#ifdef CONFIG_ENABLE_TLS
#include <ccm.h>
#else
#define MBEDTLS_ERR_CCM_AUTH_FAILED -0x000F
#endif /* CONFIG_ENABLE_TLS */

static bool aes_init_done;
/*#define AES_DEBUG */

#ifdef AES_DEBUG

/* #define AES_DUMP_INIT_DATA */
/* #define AES_DUMP_DATA */

#define ADEBUG(...)				\
	do {					\
		wmprintf("AES: ");	\
		wmprintf(__VA_ARGS__);	\
		wmprintf("\n\r");		\
	} while (0);

#define CHECK_AES_INIT_DONE						\
	if (!aes_init_done) {						\
		wmprintf("AES init not done. Please do it first.\n\r"); \
		return -WM_FAIL;					\
	}

#define ENTER_F					\
	wmprintf("Enter: %s\n\r", __func__);

#define EXIT_F					\
	wmprintf("Exit: %s\n\r", __func__);
#else
#define ADEBUG(...)
#define ENTER_F
#define EXIT_F
#define CHECK_AES_INIT_DONE
#endif /* AES_DEBUG */

#define ADEBUG_E(...)							\
	do {								\
		wmlog_e("AES", " R: 0x%x\n\r",\
		__builtin_return_address(0), __VA_ARGS__); \
	} while (0);

#define ADEBUG_W(...)				\
	do {					\
		wmlog("AES", "Warn: R: 0x%x\n\r",\
		__builtin_return_address(0), __VA_ARGS__); \
	} while (0);

#define LOCK_AES				\
	os_mutex_get(&aes_mutex, OS_WAIT_FOREVER)

#define UNLOCK_AES				\
	os_mutex_put(&aes_mutex)

#ifdef AES_DUMP_DATA
#define ADUMP wmprintf
#else
#define ADUMP(...)
#endif /* AES_DUMP_DATA */

/*
 *
 * This driver fully support AES hardware usage in parallel threads with
 * different AES configurations. Following method is used to support it.
 *
 * The principle user of AES is CyaSSL library. It does not support a
 * session based usage of the AES API's. So, the mdev layer in this file is
 * a dummy one to keep API consistency across various drivers. A different
 * way compatible to CyaSSL is used internally to enable parallel
 * operation.
 *
 * The below pointer current_aes_ptr will be used to remember the last AES
 * configuration loaded in AES hardware. Whenever, encrypt/decrypt is
 * called with the aes_t parameter which same as below saved one, this
 * pointer will be updated with new aes_t and  and h/w will be programmed
 * again with this new aes_t.
 */
static const aes_t *current_aes_ptr;
static os_mutex_t aes_mutex;

/* The device objects */
static mdev_t mdev_aes;
/* Device name */
static const char *mdev_aes_name = MDEV_AES_0;

int aes_drv_init()
{
	int status;

	if (aes_init_done)
		return WM_SUCCESS;

	status = os_mutex_create(&aes_mutex, "aes-hw", OS_MUTEX_INHERIT);
	if (status != WM_SUCCESS)
		return -WM_FAIL;

	/* Mask all Interrupts */
	AES_IntMask(AES_INT_ALL, MASK);

	current_aes_ptr = NULL;

	aes_init_done = true;

	return WM_SUCCESS;
}

/**
 * This cache is kept to reconfigure the hardware in case the hardware is
 * hijacked with some other configuration.
 *
 * Note: No actual hardware operation is to be performed in this function.
 */
static void update_cache_members(aes_t *aes, const AES_Config_Type *aesConfig)
{
	aes->mode = aesConfig->mode;
	aes->dir = aesConfig->encDecSel;
	/* Already processed as required by h/w */
	memcpy(aes->initVect, aesConfig->initVect, sizeof(aesConfig->initVect));
	aes->keySize = aesConfig->keySize;
	/* Already processed as required by h/w */
	memcpy(aes->saved_key, aesConfig->key, sizeof(aesConfig->key));
	aes->micLen = aesConfig->micLen;
	aes->micEn = aesConfig->micEn;

	if (aes->mode == AES_MODE_CTR)
		aes->ctr_mod = AES_GetCTRCntMod();
}

/*
 * At the end of any operation the function will save the context so that
 * any other thread/user can use the AES hardware without disturbing the
 * current operation.
 *
 * Ensure that this function is called inside a critical section.
 */
static void save_context_to_cache(aes_t *aes)
{
	/* Save the output vector to be used next time as Init Vector */
	if (aes->mode == AES_MODE_CBC || aes->mode == AES_MODE_CTR)
		AES_ReadOutputVector((uint32_t *) aes->initVect);

	/*
	 * All other members in aes_t are known and can be restored during
	 * context restore later.
	 */
}

/**
 * This cache is kept to reconfigure the hardware in case the hardware is
 * hijacked with some other configuration.
 *
 * assumes AES locked
 */
static void restore_context_from_cache(const aes_t *aes)
{
	AES_Config_Type aesConfig;
	ADEBUG("Configuration hijacked. Restoring\n\r");
	aesConfig.mode = aes->mode;
	aesConfig.encDecSel = aes->dir;

	/* Already processed as required by h/w */
	memcpy(aesConfig.initVect, aes->initVect, sizeof(aesConfig.initVect));

	aesConfig.keySize = aes->keySize;
	/* Already processed as required by h/w */
	memcpy(aesConfig.key, aes->saved_key, sizeof(aesConfig.key));
	aesConfig.micLen = aes->micLen;
	aesConfig.micEn = aes->micEn;

	aesConfig.aStrLen = aes->aStrLen;

	/* Configure hardware */
	current_aes_ptr = aes;

#ifdef AES_DUMP_INIT_DATA
	wmprintf("aesConfig.mode = %d\n\r", aesConfig.mode);
	wmprintf("encDecSel = %d\n\r", aesConfig.encDecSel);
	dump_hex(&aesConfig.initVect[0], sizeof(aesConfig.initVect));
	wmprintf("keySize: %d\n\r", aesConfig.keySize);
	dump_hex(&aesConfig.key[0], sizeof(aesConfig.key));
	wmprintf("aStrLen: %d\n\r", aesConfig.aStrLen);
	wmprintf("mStrLen: %d\n\r", aesConfig.mStrLen);
	wmprintf("micLen: %d\n\r", aesConfig.micLen);
	wmprintf("micEn: %d\n\r", aesConfig.micEn);
#endif /* AES_DUMP_INIT_DATA */

	AES_Init(&aesConfig);

	/* Clear AES input and output FIFO */
	AES_ClrInputFIFO();
	AES_ClrOutputFIFO();

	if (aes->mode == AES_MODE_CTR)
		AES_SetCTRCntMod(aes->ctr_mod);
}

static int hwmode_to_mode(hw_aes_mode_t hw_mode)
{
	int mode = -1;
	switch (hw_mode) {
	case HW_AES_MODE_ECB:
		mode = AES_MODE_ECB;
		break;
	case HW_AES_MODE_CBC:
		mode = AES_MODE_CBC;
		break;
	case HW_AES_MODE_CTR:
		mode = AES_MODE_CTR;
		break;
	case HW_AES_MODE_CCM:
		mode = AES_MODE_CCM;
		break;
	case HW_AES_MODE_MMO:
		mode = AES_MODE_MMO;
		break;
	case HW_AES_MODE_BYPASS:
		mode = AES_MODE_BYPASS;
		break;
	default:
		ADEBUG_E("Unknown mode.");
	}
	return mode;
}

static inline int aes_set_dir_and_iv(const uint8_t *pIV,
				     int dir, AES_Config_Type *aesConfig)
{
	int i, j;
	switch (dir) {
	case AES_ENCRYPTION:
		ADEBUG("Encrypt\n\r");
		aesConfig->encDecSel = AES_MODE_ENCRYPTION;
		break;
	case AES_DECRYPTION:
		ADEBUG("Decrypt\n\r");
		aesConfig->encDecSel = AES_MODE_DECRYPTION;
		break;
	default:
		ADEBUG_E("Unknown direction");
		return -WM_FAIL;
	}

	/*
	 * Initial input vector. This is also called as the nonce
	 * in case of AES counter mode.
	 */
	for (i = 0, j = 0; i < 4; i++, j += 4)
		aesConfig->initVect[3 - i] = pIV[j+3] << 24 | pIV[j+2] << 16 |
						pIV[j+1] << 8 | pIV[j];

	return WM_SUCCESS;
}

/**
 * Size of IV should be equal to AES block size i.e. 16 bytes (128 bits)
 */
static inline int aes_setkey(aes_t *aes, const uint8_t *userKey,
			     uint32_t key_len, const uint8_t *IV, int dir,
			     hw_aes_mode_t hw_mode)
{
	int count, i, j, rv;
	AES_Config_Type aesConfig;

	ENTER_F;
	CHECK_AES_INIT_DONE;

	if (!aes || !userKey || !IV) {
		ADEBUG_E("Null pointer");
		return -WM_E_INVAL;
	}

	/* Set default mode. Can be changed later by function aes_setmode */
	aesConfig.mode = hwmode_to_mode(hw_mode);

	rv = aes_set_dir_and_iv(IV, dir, &aesConfig);
	if (rv != WM_SUCCESS)
		return rv;

	ADEBUG("keysize: %d", key_len);
	switch (key_len) {
	case 16:
		aesConfig.keySize = AES_KEY_BYTES_16;
		count = 4;
		break;
	case 24:
		aesConfig.keySize = AES_KEY_BYTES_24;
		count = 6;
		break;
	case 32:
		aesConfig.keySize = AES_KEY_BYTES_32;
		count = 8;
		break;
	default:
		ADEBUG_E("Key length %d wrong", key_len);
		return -WM_E_INVAL;
	}

	/* AES key */
	for (i = 0, j = 0; i < 8; i++, j += 4) {
		if (i < count)
			aesConfig.key[7 - i] = userKey[j+3] << 24 |
						userKey[j+2] << 16 |
						userKey[j+1] << 8 |
						userKey[j];
		else
			aesConfig.key[7 - i] = 0x0;
	}

	aesConfig.micLen = 0;
	aesConfig.micEn = DISABLE;

	LOCK_AES;
	if (aes->mode == AES_MODE_CTR) {
		AES_SetCTRCntMod(AES_CTR_MOD_2_POW_128);
		aes->data_buffered = false;
		aes->bufferCnt = 0;
		aes->buf_idx = 0;
	}

	update_cache_members(aes, &aesConfig);
	/*
	 * It is a possibility that the same aes pointer which was cached
	 * before in current_aes_ptr is being reused with different
	 * parameters. We need to reset the current_aes_ptr in this case to
	 * force loading new parameters into the hardware.
	 */
	if (current_aes_ptr == aes)
		current_aes_ptr = NULL;
	UNLOCK_AES;

	/*
	 * Note: We have not performed any hardware operation in this
	 * function. This will happen automatically during the first
	 * operation on the AES.
	 */

	return WM_SUCCESS;
}

/**
 * Size of IV should be equal to AES block size i.e. 16 bytes (128 bits)
 */
static inline int aes_ccm_setkey(aes_t *aes, const uint8_t *userKey,
				 uint32_t key_len)
{
	int count, i;
	uint32_t *pKey = (uint32_t *) userKey;

	ENTER_F;
	CHECK_AES_INIT_DONE;

	if (!aes || !userKey) {
		ADEBUG_E("Null pointer");
		return -WM_E_INVAL;
	}

	aes->mode = AES_MODE_CCM;

	ADEBUG("keysize: %d", key_len);
	switch (key_len) {
	case 16:
		aes->keySize = AES_KEY_BYTES_16;
		count = 4;
		break;
	case 24:
		aes->keySize = AES_KEY_BYTES_24;
		count = 6;
		break;
	case 32:
		aes->keySize = AES_KEY_BYTES_32;
		count = 8;
		break;
	default:
		ADEBUG_E("Key length %d wrong", key_len);
		return -WM_E_INVAL;
	}

	/* AES key */
	for (i = 0; i < 8; i++) {
		if (i < count)
			aes->saved_key[7 - i] = pKey[i];
		else
			aes->saved_key[7 - i] = 0x0;
	}

	/*
	 * Note: We have not performed any hardware operation in this
	 * function. This will happen automatically during the first
	 * operation on the AES.
	 */

	return WM_SUCCESS;
}

void AesCTRIncrementIV(aes_t *aes, uint32_t len)
{
	int8_t i, block_cnt, row;
	uint8_t index;
	uint8_t *IV = (uint8_t *)aes->initVect;

	/* Increment IV for the next block.
	 * Order of IV index to be increased is as follow
	 *
	 * 3  2  1  0  <- row1
	 * 7  6  5  4  <- row2
	 * 11 10 9  8  <- row3
	 * 15 14 13 12 <- row4
	 */
	for (block_cnt = 0; block_cnt < len; block_cnt++) {
		for (row = 0; row < AES_BLOCK_SIZE/4; row--) {
			for (i = 3; i >= 0; i--) {
				index = (row * 4) + i;
				if (++IV[index])
					break;
			}
			if (i)
				break;
		}
	}

}

/*
 * Assumes AES_LOCK is taken.
 */
static void aes_operation(aes_t *aes, uint8_t *indata,
			  uint8_t *outdata, uint32_t len)
{
	uint32_t inputCnt = 0;
	uint32_t outputCnt = 0;
	uint32_t block_data_len = 0;
	uint32_t orig_len = len;
	uint32_t plain = 0, tmp_var = 0, unaligned_word = 0;
	uint8_t extra_bytes = 0, plain_borrow_sz = 0;
	uint8_t size_32 = sizeof(uint32_t);

	/*
	 * Warning: The below logic (current_aes_ptr != aes) is extremely
	 * important for multithreaded operation of AES. Please be
	 * absolutely sure before changing anything related to this
	 * logic.
	 */
	if (current_aes_ptr != aes) {
		/* Somebody has hijacked our configuration or this is the
		   first call to aes_operation after setkey */
		restore_context_from_cache(aes);
	} else {
		/* Restore the last saved Output vector. */
		if (aes->mode == AES_MODE_CBC || aes->mode == AES_MODE_CTR)
			AES_SetIV((const uint32_t *)aes->initVect);
	}

	/* Use any buffered key material first for CTR mode */
	if ((aes->mode == AES_MODE_CTR) &&
	    (aes->data_buffered == true)) {
		for (; (inputCnt < aes->bufferCnt) && (inputCnt < len);
		     outputCnt++, inputCnt++, aes->buf_idx++) {
			outdata[outputCnt] =
				aes->buffered_data[aes->buf_idx] ^
				indata[inputCnt];
		}

		if (len < aes->bufferCnt) {
			aes->bufferCnt -= len;
			goto end;
		} else {
			len -= aes->bufferCnt;
			aes->bufferCnt = 0;
			aes->buf_idx = 0;
			aes->data_buffered = false;
		}
	}

	if ((aes->mode == AES_MODE_CTR) && (len & (AES_BLOCK_SIZE - 1))) {
		if ((extra_bytes = len & (size_32 - 1))) {
			ADEBUG("* Data unaligned");
			/*
			 * The len of data could be 4 byte unaligned. This means
			 * we need to add some data from the plain text to
			 * remaining bytes of the last word of data.
			 */

			/* Copy the unaligned part of data */
			memcpy(&unaligned_word, indata + orig_len - extra_bytes,
					extra_bytes);
			/* Fill remaining bytes from data */
			plain_borrow_sz = size_32 - extra_bytes;
			memcpy(((char *)&unaligned_word + extra_bytes),
					(char *)&plain, plain_borrow_sz);

			/*
			 * Modify pointers to original data now that we have
			 * taken away some bytes from it.
			 */
			len -= extra_bytes;

		}
		/* Select block_data_len as next higher multiple of
		 * AES_BLOCK_SIZE */
		block_data_len = (len / AES_BLOCK_SIZE) + 1;
		block_data_len *= AES_BLOCK_SIZE;
	}

	else
		block_data_len = len;

	/* If block_data_len equals 0, no hardware AES engine is used for
	 * encryption. Thus, do not wait for AES interrupts & return without
	 * feeding data to AES engine.
	 */
	if (block_data_len == 0)
		goto end;

	AES_SetMsgLen(block_data_len);

	ADEBUG("Len: %d", block_data_len);
	AES_Enable();
	ADEBUG("Operation started");

	/* In case of unaligned transfer in CTR make sure next whole block is
	 * processed */
	if ((aes->mode == AES_MODE_CTR)) {
		block_data_len += inputCnt;
		len += inputCnt;
	}

	while ((inputCnt < block_data_len) || (outputCnt < block_data_len)) {
		/* Proceed only if input buffer is not full */
		if (AES_GetStatus(AES_STATUS_INFIFO_FULL))
			continue;

		/* IN data */
		if (inputCnt < len) {
			memcpy((uint8_t *)&tmp_var, indata + inputCnt, size_32);
			AES_FeedData(tmp_var);
			inputCnt += size_32;
		}

		/* IN misaligned word with padded zeroes*/
		if (extra_bytes && inputCnt == len) {
			AES_FeedData((uint32_t)unaligned_word);
			inputCnt += size_32;
		}

		/* IN padded zeroes */
		if ((inputCnt >= len) && (inputCnt < block_data_len)) {
			AES_FeedData(0);
			inputCnt += size_32;
		}

		/* Proceed only if out buffer is not empty */
		if (AES_GetStatus(AES_STATUS_OUTFIFO_EMPTY))
			continue;

		/* OUT data*/
		if (outputCnt < len) {
			tmp_var = AES_ReadData();
			memcpy(outdata + outputCnt,
				(uint8_t *)&tmp_var, size_32);
			outputCnt += size_32;
		}


		/* OUT get misaligned bytes and buffer rest of the output */
		if (extra_bytes && (outputCnt == len)) {
			tmp_var = AES_ReadData();
			memcpy(outdata + outputCnt,
					(uint8_t *)&tmp_var,
					extra_bytes);
			memcpy(aes->buffered_data + aes->bufferCnt,
					(uint8_t *)&tmp_var + extra_bytes,
					plain_borrow_sz);
			outputCnt += size_32;
			aes->bufferCnt += plain_borrow_sz;
		}

		/* Process block with zero input */
		if ((outputCnt >= len) && (outputCnt < block_data_len)) {
			tmp_var = AES_ReadData();
			memcpy(aes->buffered_data + aes->bufferCnt,
					(uint8_t *)&tmp_var, size_32);
			outputCnt += size_32;
			aes->bufferCnt += size_32;
		}


	}

	if ((aes->mode == AES_MODE_CTR)) {
		if (aes->bufferCnt)
			aes->data_buffered = true;
	}
	ADEBUG("Operation done");
	AES_Disable();
	save_context_to_cache(aes);
end:
	return;
}

void aes_ctr_final(aes_t *aes)
{
	/* Clear all buffered data */
	aes->bufferCnt = 0;
	aes->data_buffered = false;
	aes->buf_idx = 0;
	memset(aes->buffered_data , 0, sizeof(aes->buffered_data));
}

/*
 * The input data is : AAD + Plain
 * The output data is: AAD + Cipher + MIC (optional)
 *
 * Some corner cases which are handled by this function:
 * - The AAD can be unaligned to 4 bytes.
 *           In this case some bytes from plain text are borrowed and
 *           appended to AAD.
 * - The Data can be unaligned to 4 bytes.
 *           This is handled by _not_ dereferencing the pointer pointing to
 * last word directly. The unaligned part is removed and handled
 * separately.
 *
 * To avoid increasing the code complexity and reduce the "ifs" we will
 * assume that the length is always unaligned. We will keep a 32 bit
 * variable each for AAD and data  to hold the last 4 bytes. If actually
 * unaligned the four bytes will contain the trailing unaligned bytes from
 * the main buffer and zero pad for remaining. If originally aligned the
 * the last four bytes of the buffer will be as is in the variable.
 *
 * Assumes AES_LOCK is taken.
 */
static void aes_ccm_enc_operation(aes_t *aes,
				  const char *plain, char *out_data,
				  uint32_t len, char *out_mic, uint32_t mic_sz,
				  const char *in_aad, uint32_t aad_sz)
{
	uint32_t ori_len = len;
	int extra_aad, extra_data;

	/*
	 * Note that we have done some magic below to avoid allocating new
	 * buffers to store the AAD + data together. If the AAD is of
	 * unaligned size then we need to add bytes from data to the last
	 * word of AAD. This introduces some complexities of handling the
	 * data. Allocating buffers and storing AAD and data together is
	 * the easiest solution but we are doing the below magic to save
	 * heap and memcpys.
	 */

	uint32_t aad_in_aligned = 0;
	if ((extra_aad = aad_sz % sizeof(uint32_t))) {
		ADEBUG("* AAD unaligned");
		/*
		 * The len of AAD is 4 byte unaligned. This means we need
		 * to add some data from the plain text to remaining bytes
		 * of the last word of AAD.
		 */

		/* Copy the unaligned part of AAD */
		memcpy(&aad_in_aligned, in_aad + aad_sz - extra_aad, extra_aad);
		/* Fill remaining bytes from data */
		unsigned plain_borrow_sz = sizeof(uint32_t) - extra_aad;
		memcpy(((char *)&aad_in_aligned + extra_aad),
		       plain, plain_borrow_sz);

		/*
		 * Modify pointers to original data now that we have taken
		 * away some bytes from it.
		 */
		plain += plain_borrow_sz;
		len -= plain_borrow_sz;

		/* Fake that auth size is more than it actually is. Now it
		   also includes (4 - extra_aad) bytes of the data */
		aad_sz = (aad_sz - extra_aad) + sizeof(uint32_t);
	} else {
		/* The AAD length is word aligned */
		ADEBUG("* AAD aligned");

		/* Copy the complete last word */
		memcpy(&aad_in_aligned, in_aad + aad_sz - sizeof(uint32_t),
		       sizeof(uint32_t));
	}

	uint32_t data_in_aligned = 0;
	if ((extra_data = len % sizeof(uint32_t))) {
		ADEBUG("* Data unaligned");
		/*
		 * The len of plain is 4 byte unaligned.
		 */

		/* Copy the unaligned part */
		memcpy(&data_in_aligned, plain + len - extra_data, extra_data);
		/* fake that the data size is more than it actually is */
		len = (len - extra_data) + sizeof(uint32_t);
	} else {
		/* The data length is word aligned */

		/* Copy the complete last word */
		memcpy(&data_in_aligned, plain + len - sizeof(uint32_t),
		       sizeof(uint32_t));
	}

	AES_Reset();
	restore_context_from_cache(aes);
	AES_SetMsgLen(ori_len);

	ADEBUG("Len: %d", len);

	const uint32_t *plain32 = (const uint32_t *)plain;
	uint32_t *out_data32 = (uint32_t *) out_data;
	const uint32_t *in_aad32 = (const uint32_t *)in_aad;
	uint32_t *out_mic32 = (uint32_t *) out_mic;

	/*
	 * - Now AAD and data i.e. aad_sz and len are atleast word
	 *   aligned.
	 * - The *_buf variable below stores the amount to data (uint32_t)
	 *   to be read from the buffers. The remaining one word will be
	 *   read from the result of processing done above.
	 *
	 * - FIFO port size is 4 bytes.
	 */
	unsigned len32 = len / sizeof(uint32_t),
	    aad_sz32 = aad_sz / sizeof(uint32_t);

	const uint32_t len_buf32 = len32 - 1;
	const uint32_t aad_sz_buf32 = aad_sz32 - 1;

	const uint32_t total_out32 = aad_sz32 + len32 +
	    mic_sz / sizeof(uint32_t), total_in32 = aad_sz32 + len32;
	uint32_t aad_in_idx = 0, aad_out_idx = 0,
	    data_in_idx = 0, data_out_idx = 0, mic_out_idx = 0;
	uint32_t in32 = 0, out32 = 0;

	/*
	 * Note: The below variables are needed for 4-byte unalignment
	 * management and thus to avoid user buffer overrun. For e.g. if
	 * the data in size is 23 bytes, the out size will be also 23
	 * bytes. The user buffer is likely to be only 23 bytes. If we read
	 * from the 32 bit FIFO and assign it directly to the user buffer
	 * we will corrupt the 24th byte which is scandalous.
	 */
	uint32_t aad_out_aligned, data_out_aligned;

	ADEBUG("CCM encryption Operation started");
	AES_Enable();

	while (out32 < total_out32) {
		/* IN */
		if (in32 < total_in32 &&
		    !AES_GetStatus(AES_STATUS_INFIFO_FULL)) {
			/* AAD */
			if (aad_in_idx < aad_sz32) {
				if (aad_in_idx == aad_sz_buf32) {
					AES_FeedData(aad_in_aligned);
					ADUMP("-- AW: L: %p\n\r",
					      aad_in_aligned);
				} else {
					AES_FeedData(in_aad32[aad_in_idx]);
					ADUMP("-- AW: %p\n\r",
					      in_aad32[aad_in_idx]);
				}
				aad_in_idx++;
			} else {	/* DATA */
				if (data_in_idx == len_buf32) {
					AES_FeedData(data_in_aligned);
					ADUMP("-- DW: L: %p\n\r",
					      data_in_aligned);
				} else {
					AES_FeedData(plain32[data_in_idx]);
					ADUMP("-- DW: %p\n\r",
					      plain32[data_in_idx]);
				}
				data_in_idx++;
			}
			in32++;
		}

		/* OUT */
		if (!AES_GetStatus(AES_STATUS_OUTFIFO_EMPTY)) {
			/* AAD */
			if (aad_out_idx < aad_sz32) {
				/* Read and discard */
				uint32_t v = AES_ReadData();
				if (aad_out_idx == aad_sz_buf32) {
					aad_out_aligned = v;
					ADUMP("-- AR: L: %p\n\r", v);
				} else {
					ADUMP("-- AR: %p\n\r", v);
				}
				aad_out_idx++;
			} else if (data_out_idx < len32) {	/* DATA */
				uint32_t v = AES_ReadData();
				if (data_out_idx == len_buf32) {
					data_out_aligned = v;
					ADUMP("-- DR: L: %p\n\r", v);
				} else {
					out_data32[data_out_idx] = v;
					ADUMP("-- DR: %p\n\r",
					      out_data32[data_out_idx]);
				}
				data_out_idx++;
			} else {	/* MIC */
				out_mic32[mic_out_idx] = AES_ReadData();
				ADUMP("-- MR: %p\n\r", out_mic32[mic_out_idx]);
				/*
				 * Note: We do not need the alignment
				 * management here as mic will always be
				 * aligned to 4 bytes and we have no risk
				 * of overrunning the user buffer.
				 */
				mic_out_idx++;
			}
			out32++;
			ADUMP("%d: %d\n\r", out32, total_out32);
		}
	}

	/* Post processing */

	/* Now do read data alignment management */
	unsigned out_data_borrow_sz = 0;
	if (extra_aad) {
		ADEBUG("AAD alignment management needed (ori unaligned)");
		/*
		 * Since the input AAD was misaligned we will have few data
		 * out bytes in last word of read AAD.
		 */
		out_data_borrow_sz = sizeof(uint32_t) - extra_aad;
		memmove(out_data + out_data_borrow_sz, out_data,
			len_buf32 * sizeof(uint32_t));
		memcpy(out_data, (char *)&aad_out_aligned + extra_aad,
		       out_data_borrow_sz);
	}

	if (mic_sz && extra_data) {	/* MIC len can be zero */
		ADEBUG("Data alignment management needed (ori unaligned)");
		/*
		 * Since the input len was misaligned we will have few MIC
		 * bytes in last word of read data.
		 */
		unsigned out_mic_borrow_sz = sizeof(uint32_t) - extra_data;

		/* Push MIC by a few bytes */
		memmove(out_mic + out_mic_borrow_sz, out_mic,
			mic_sz - out_mic_borrow_sz);

		/* Prepend MIC */
		memcpy(out_mic, (char *)&data_out_aligned + extra_data,
		       out_mic_borrow_sz);

		/*
		 * Append data.
		 *
		 * Note that the AAD data also may have been
		 * misaligned. If that was the case the data has already
		 * been pushed forward by 'out_data_borrow_sz' bytes. Lets
		 * also consider that here. 'out_data_borrow_sz' has been
		 * init'ed to zero above. So, unconditionally adding it
		 * here is harmless.
		 */
		memcpy(out_data + (len_buf32 * 4) + out_data_borrow_sz,
		       &data_out_aligned, extra_data);
	} else {
		ADEBUG("Data alignment management needed (ori aligned)");
		/* Append data */
		memcpy(out_data + (len_buf32 * 4),
		       &data_out_aligned, sizeof(uint32_t));
	}

	ADEBUG("Operation done");
	AES_Disable();

	save_context_to_cache(aes);
}

/*
 * Input data : AAD + Cipher + MIC
 * Output data: AAD + Plain
 */
static void aes_ccm_dec_operation(aes_t *aes,
				  const char *cipher,
				  char *plain, uint32_t len,
				  const char *mic, uint32_t mic_sz,
				  const char *in_aad, uint32_t aad_sz)
{
	uint32_t ori_len = len;
	int extra_aad, extra_data, extra_mic;

	/*
	 * Note that we have done some magic below to avoid allocating new
	 * buffers to store the AAD + data + MIC together. If the AAD is of
	 * unaligned size then we need to add bytes from data to the last
	 * word of AAD. Similar for Data and MIC part. This introduces some
	 * complexities of handling the data. Allocating buffers and
	 * storing AAD and data together is the easiest solution but we are
	 * doing the below magic to save heap and memcpys.
	 */

	uint32_t aad_in_aligned = 0;
	if ((extra_aad = aad_sz % sizeof(uint32_t))) {
		ADEBUG("* AAD unaligned");
		/*
		 * The len of AAD is 4 byte unaligned. This means that we
		 * need to add some bytes from the data to the remaining
		 * bytes of last word of AAD.
		 */

		/* Copy the unaligned part of AAD */
		memcpy(&aad_in_aligned, in_aad + aad_sz - extra_aad, extra_aad);
		/* Fill remaining bytes from data */
		unsigned cipher_borrow_sz = sizeof(uint32_t) - extra_aad;
		memcpy((char *)&aad_in_aligned + extra_aad,
		       cipher, cipher_borrow_sz);

		/*
		 * Modify pointers to original data now that we have taken
		 * away some bytes from it.
		 */
		cipher += cipher_borrow_sz;
		len -= cipher_borrow_sz;

		/* Fake that auth size is more than it actually is. Now, it
		   also includes (4 - extra_aad) bytes of the data */
		aad_sz = (aad_sz - extra_aad) + sizeof(uint32_t);
	} else {
		/* The AAD length is word aligned */
		ADEBUG("* AAD aligned");

		/* Copy the complete last word */
		if (in_aad)
			memcpy(&aad_in_aligned,
				in_aad + aad_sz - sizeof(uint32_t),
				sizeof(uint32_t));
	}

	uint32_t data_in_aligned = 0;
	if ((extra_data = len % sizeof(uint32_t))) {
		ADEBUG("* Data unaligned");
		/*
		 * The len of cipher is 4 byte unaligned. We need to add
		 * some bytes from the MIC to the last word of len
		 */

		/* Copy the unaligned part */
		memcpy(&data_in_aligned, cipher + len - extra_data, extra_data);

		if (mic_sz) {
			/* Take some bytes from MIC */
			unsigned mic_borrow_sz = sizeof(uint32_t) - extra_data;
			memcpy((char *)&data_in_aligned + extra_data,
			       mic, mic_borrow_sz);

			mic += mic_borrow_sz;
			mic_sz -= mic_borrow_sz;
		}

		/* fake that the data size is more than it actually is */
		len = (len - extra_data) + sizeof(uint32_t);
	} else {
		/* The data length is word aligned */

		/* Copy the complete last word */
		memcpy(&data_in_aligned, cipher + len - sizeof(uint32_t),
		       sizeof(uint32_t));
	}

	/*
	 * Note: MIC by default is always aligned to 4 bytes. But, it may
	 * be a possible that some bytes from the MIC are borrowed by the
	 * data part to match 4 byte alignment. Hence, below code is
	 * necessary.
	 */
	uint32_t mic_aligned = 0;
	if (mic_sz) {
		if ((extra_mic = mic_sz % sizeof(uint32_t))) {
			/* MIC is not aligned */
			memcpy(&mic_aligned, mic + mic_sz - extra_mic,
			       extra_mic);

			mic_sz = mic_sz - extra_mic + sizeof(uint32_t);
			ADEBUG("MIC aligned: %p", mic_aligned);
		} else {
			/* Copy the complete last word */
			memcpy(&mic_aligned, mic + mic_sz - sizeof(uint32_t),
			       sizeof(uint32_t));
		}
	}

	AES_Reset();
	restore_context_from_cache(aes);
	AES_SetMsgLen(ori_len);

	ADEBUG("Len: %d", len);

	const uint32_t *in_aad32 = (const uint32_t *)in_aad;
	const uint32_t *cipher32 = (const uint32_t *)cipher;
	uint32_t *plain32 = (uint32_t *) plain;

	const uint32_t *mic32 = (const uint32_t *)mic;

	/*
	 * - Now AAD, cipher and mic i.e. aad_sz, len and mic_sz  are
	 *   atleast word aligned.
	 * - The *_buf variable below store the amount to data (uint32_t)
	 *   to be read from the buffers. The remaining one word will be
	 *   read from the result of processing done above.
	 *
	 * - FIFO port size is 4 bytes.
	 */
	unsigned len32 = len / sizeof(uint32_t),
	    aad_sz32 = aad_sz / sizeof(uint32_t),
	    mic_sz32 = mic_sz / sizeof(uint32_t);

	const uint32_t len_buf32 = len32 - 1;
	const uint32_t aad_sz_buf32 = aad_sz32 - 1;
	const uint32_t mic_sz_buf32 = mic_sz32 - 1;

	const uint32_t total_out32 = aad_sz32 + len32,
	    total_in32 = aad_sz32 + len32 + mic_sz32;
	uint32_t aad_in_idx = 0, aad_out_idx = 0,
	    data_in_idx = 0, data_out_idx = 0, mic_in_idx = 0;
	uint32_t in32 = 0, out32 = 0;

	/*
	 * Note: The below variables are needed for 4-byte unalignment
	 * management and thus to avoid user buffer overrun. For e.g. if
	 * the data in size is 23 bytes, the out size will be also 23
	 * bytes. The user buffer is likely to be only 23 bytes. If we read
	 * from the 32 bit FIFO and assign it directly to the user buffer
	 * we will corrupt the 24th byte which is scandalous.
	 */
	uint32_t aad_out_aligned, data_out_aligned;

	ADEBUG("CCM decryption Operation started");
	AES_Enable();

	while (out32 < total_out32) {
		/* IN */
		if (in32 < total_in32 &&
		    !AES_GetStatus(AES_STATUS_INFIFO_FULL)) {
			/* AAD */
			if (aad_in_idx < aad_sz32) {
				if (aad_in_idx == aad_sz_buf32) {
					AES_FeedData(aad_in_aligned);
					ADUMP("-- AW: L: %p\n\r",
					      aad_in_aligned);
				} else {
					AES_FeedData(in_aad32[aad_in_idx]);
					ADUMP("-- AW: %p\n\r",
					      in_aad32[aad_in_idx]);
				}
				aad_in_idx++;
			} else if (data_in_idx < len32) {	/* DATA */
				if (data_in_idx == len_buf32) {
					AES_FeedData(data_in_aligned);
					ADUMP("-- DW: L: %p\n\r",
					      data_in_aligned);
				} else {
					AES_FeedData(cipher32[data_in_idx]);
					ADUMP("-- DW: %p\n\r",
					      cipher32[data_in_idx]);
				}
				data_in_idx++;
			} else {	/* MIC */
				if (mic_in_idx == mic_sz_buf32) {
					AES_FeedData(mic_aligned);
					ADUMP("-- MW: L: %p\n\r", mic_aligned);
				} else {
					AES_FeedData(mic32[mic_in_idx]);
					ADUMP("-- MW: %p\n\r",
					      mic32[mic_in_idx]);
				}
				mic_in_idx++;
			}

			in32++;
		}

		/* OUT */
		if (!AES_GetStatus(AES_STATUS_OUTFIFO_EMPTY)) {
			/* AAD */
			if (aad_out_idx < aad_sz32) {
				/* Read and discard */
				uint32_t v = AES_ReadData();
				if (aad_out_idx == aad_sz_buf32) {
					/*
					 * This needs to be saved as it may
					 * contain some data bytes.
					 */
					aad_out_aligned = v;
					ADUMP("-- AR: L: %p\n\r", v);
				} else {
					ADUMP("-- AR: %p\n\r", v);
				}
				aad_out_idx++;
			} else {	/* DATA */
				uint32_t v = AES_ReadData();
				if (data_out_idx == len_buf32) {
					data_out_aligned = v;
					ADUMP("-- DR: L: %p\n\r", v);
				} else {
					plain32[data_out_idx] = v;
					ADUMP("-- DR: %p\n\r",
					      plain32[data_out_idx]);
				}
				data_out_idx++;
			}

			out32++;
			ADUMP("%d: %d\n\r", out32, total_out32);
		}
	}

	/* Post processing */

	/* Now do read data alignment management */
	unsigned plain_borrow_sz = 0;
	if (extra_aad) {
		ADEBUG("AAD alignment management needed (ori unaligned)");
		/*
		 * Since the input AAD was misaligned we will have few data
		 * out bytes in last word of read AAD.
		 */
		plain_borrow_sz = sizeof(uint32_t) - extra_aad;
		memmove(plain + plain_borrow_sz, plain,
			len_buf32 * sizeof(uint32_t));
		memcpy(plain, (char *)&aad_out_aligned + extra_aad,
		       plain_borrow_sz);
	}

	if (extra_data) {
		ADEBUG("Data alignment management needed (ori unaligned)");
		/*
		 * Append data.
		 *
		 * Note that the AAD data also may have been
		 * misaligned. If that was the case the data has already
		 * been pushed forward by 'plain_borrow_sz' bytes. Lets
		 * also consider that here. 'plain_borrow_sz' has been
		 * init'ed to zero above. So, unconditionally adding it
		 * here is harmless.
		 */
		memcpy(plain + (len_buf32 * 4) + plain_borrow_sz,
		       &data_out_aligned, extra_data);
	} else {
		ADEBUG("Data alignment management needed (ori aligned)");
		/* Append data */
		memcpy(plain + (len_buf32 * 4),
		       &data_out_aligned, sizeof(uint32_t));
	}

	ADEBUG("Operation done");
	AES_Disable();

	save_context_to_cache(aes);
}

mdev_t *aes_drv_open(const char *name)
{
	if (!aes_init_done) {
		ADEBUG_W("driver open called without registering device"
		      " (%s)", name);
		return NULL;
	}

	if (!name || strcmp(name, mdev_aes_name)) {
		ADEBUG_E("Open failed. Name invalid");
		return NULL;
	}
	return &mdev_aes;
}

int aes_drv_setkey(mdev_t *dev, aes_t *aes, const uint8_t *userKey,
		   uint32_t key_len, const uint8_t *IV, int dir,
		   hw_aes_mode_t hw_mode)
{
	return aes_setkey(aes, userKey, key_len, IV, dir, hw_mode);
}

int aes_drv_ccm_setkey(mdev_t *dev, aes_t *aes, const uint8_t *userKey,
		       uint32_t key_len)
{
	return aes_ccm_setkey(aes, userKey, key_len);
}

int aes_drv_encrypt(mdev_t *dev, aes_t *aes, const uint8_t *plain,
		    uint8_t *cipher, uint32_t sz)
{
	ENTER_F;
	CHECK_AES_INIT_DONE;

	if (aes->mode == AES_MODE_CBC && sz % 4) {
		ADEBUG_E("AES block size wrong. Cannot encrypt");
		return -WM_E_INVAL;
	}

	LOCK_AES;
	aes_operation(aes, (uint8_t *)plain, cipher, sz);
	UNLOCK_AES;
	return WM_SUCCESS;
}

static int aes_ccm_configure_options(aes_t *aes,
				     const char *nonce, uint32_t nonce_sz,
				     uint32_t mic_sz, uint32_t aad_sz)
{
	uint32_t *pIV = (uint32_t *) nonce;
	int i;

	switch (mic_sz) {
	case 0:
		aes->micLen = AES_MIC_BYTES_0;
		break;
	case 4:
		aes->micLen = AES_MIC_BYTES_4;
		break;
	case 8:
		aes->micLen = AES_MIC_BYTES_8;
		break;
	case 16:
		aes->micLen = AES_MIC_BYTES_16;
		break;
	default:
		ADEBUG_E("AES MIC wrong size: %d. Cannot encrypt", mic_sz);
		return -WM_E_INVAL;
	}

	if (aes->micLen != AES_MIC_BYTES_0)
		aes->micEn = ENABLE;
	else
		aes->micEn = DISABLE;

	if (nonce_sz > 13)
		return -WM_E_INVAL;

	/* Set IV
	 *
	 * In CCM mode the init vector is received from the caller at every
	 * encrypt/decrypt call. We thus cannot use rely on setkey API to
	 * set the IV for us. We need to initialize the cache IV member
	 * directly here.
	 */
	for (i = 0; i < 4; i++)
		aes->initVect[i] = pIV[i];

	/* Set the value as expected from datasheet */
	aes->initVect[3] &= 0x000000FF;
	aes->initVect[3] |= (15 - nonce_sz) << 8;

	aes->aStrLen = aad_sz;

	return WM_SUCCESS;
}

/*
 * How does the CCM mode work in the context of our AES h/w accl.?
 *
 * The CCM mode has 4 inputs: plain text, nonce (also called as IV), key
 * and the optional associated authenticated data (AAD). We give back the
 * cipher text and MIC (authTag) to the caller. Note that AAD is never
 * given back to the user even though our hardware echoes it back. It is
 * only used to calculate the MIC. Note that our hardware supports inbuilt
 * alignment. We will only take case of 4 byte alignment when necessary.
 *
 * Hardware does not need aligned AAD data. The plain text starts
 * immediately after AAD with no padding bytes in between.
 *
 * In AES CCM mode the IV (nonce) is not set during key add phase. We need
 * to do it for every call to encrypt/decrypt.
 */
int aes_drv_ccm_encrypt(mdev_t *dev, aes_t *aes, const char *plain,
			char *cipher, uint32_t sz,
			const char *nonce, uint32_t nonceSz,
			char *authTag, uint32_t authTagSz,
			const char *authIn, uint32_t authInSz)
{
	int rv;
	ENTER_F;

	ADUMP("sz: %d nonceSz: %d authTagSz: %d authInSz: %d\n\r",
	      sz, nonceSz, authTagSz, authInSz);

	rv = aes_ccm_configure_options(aes, nonce, nonceSz,
				       authTagSz, authInSz);
	if (rv != WM_SUCCESS) {
		return rv;
	}

	aes->dir = AES_MODE_ENCRYPTION;

	if (!authTagSz)
		authTag = NULL;

	LOCK_AES;
	aes_ccm_enc_operation(aes, plain, cipher, sz, authTag, authTagSz,
			      authIn, authInSz);
	UNLOCK_AES;

	EXIT_F;
	return WM_SUCCESS;
}

int aes_drv_decrypt(mdev_t *dev, aes_t *aes, const uint8_t *cipher,
		    uint8_t *plain, uint32_t sz)
{
	ENTER_F;
	CHECK_AES_INIT_DONE;

	if (aes->mode == AES_MODE_CBC && sz % 4) {
		ADEBUG_E("AES block size wrong. Cannot decrypt");
		return -WM_E_INVAL;
	}

	LOCK_AES;
	aes_operation(aes, (uint8_t *)cipher, plain, sz);
	UNLOCK_AES;
	return WM_SUCCESS;
}

int aes_drv_ccm_decrypt(mdev_t *dev, aes_t *aes, const char *cipher,
			char *plain, uint32_t sz,
			const char *nonce, uint32_t nonceSz,
			const char *authTag, uint32_t authTagSz,
			const char *authIn, uint32_t authInSz)
{

	int rv = WM_SUCCESS;
	ENTER_F;

	ADUMP("sz: %d nonceSz: %d authTagSz: %d authInSz: %d\n\r",
	      sz, nonceSz, authTagSz, authInSz);

	rv = aes_ccm_configure_options(aes, nonce, nonceSz,
				       authTagSz, authInSz);
	if (rv != WM_SUCCESS) {
		return rv;
	}

	aes->dir = AES_MODE_DECRYPTION;
	if (!authTagSz)
		authTag = NULL;

	LOCK_AES;
	aes_ccm_dec_operation(aes, cipher, plain, sz, authTag, authTagSz,
			      authIn, authInSz);

	if (AES_GetStatus(AES_STATUS_ERROR_2)) {
		/* MIC error has occurred */
		memset(plain, 0x00, sz);
		rv = MBEDTLS_ERR_CCM_AUTH_FAILED;
	}
	UNLOCK_AES;

	EXIT_F;
	return rv;
}

void aes_drv_close(mdev_t *mdev)
{
	ENTER_F;
}
