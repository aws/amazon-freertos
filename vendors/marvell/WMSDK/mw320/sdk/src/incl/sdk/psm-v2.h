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

/** @file psm-v2.h
*
*  @brief Persistent storage module version 2
*
*  PSM (Persistent Storage Module) is a module which provides a PSM object
*  abstraction layer over low level flash sector read/write operations. This is
*  similar to how a filesystem provides a file abstraction over a block
*  device driver.
*
*  The PSM is specifically developed to work on flash memory devices which
*  can be read/written on byte level but erases are possible only on block
*  level. PSM helps reduce the flash usage thus helping in wear
*  leveling. The working is described in brief as follows. A PSM object is
*  an atomic structure on the flash and contains metadata and
*  data (value). Metadata contains object name, length, etc. When an object
*  value is changed a new object with same name is appended to the flash
*  block instead of:
*               - Backing up other objects
*               - Erasing flash
*               - Restoring objects
*               - Writing new object.
*
*  The earlier object with obsolete value will be inactivated and it is
*  practically deleted from the PSM. It will be physically deleted during
*  the compaction process.
*
*  \section psm-v2_compaction Compaction process
*
*  As new objects are appended to current objects a
*  time will come when the flash space is close to getting full. At this
*  point all the objects (excluding the inactivated ones) are copied to a
*  special variable sized reserved area of the same flash partition. The
*  data area i.e. the area of the partition which contained the active and
*  inactive objects uptill now, is erased and the objects are copied back
*  to data area. In effect, all obsolete objects are now physically deleted.
*
*  During the process of compaction, or during new object write a power
*  failure could occur. PSM is designed to handle this situation and has
*  error recovery algorithms. It will ensure that no existing objects are
*  corrupted and corrupted objects are safely removed during psm module
*  initialization.
*
*  The PSM module is designed to be able to read/write to arbitrary sized
*  objects spanning multiple flash blocks. The only limitation is flash
*  size and space reserved for compaction process.
*
*  \section psm-v2_terminology Terminology
*  Block
*  -----
*  This is the storage unit for the PSM. The size of a block has to
*  be in multiples of sector size of the NOR flash chip used.
*
*  \section psm-v2_mem_usage Memory usage of PSM module
*
*  Flash space usage
*  ------------------
*  PSM has 12 bytes of overhead per PSM object. Thus,
*  every PSM object size is: Data size (user defined) + name length (user
*  defined) + 12 bytes (PSM defined).
*
*  Heap usage
*  ----------
*  PSM init (psm_module_init()) allocates about 34 bytes for (internal) PSM
*  singleton object. This object will be present on heap till
*  psm_module_deinit() is called.
*  psm_object_open() allocates about 46 bytes internally and remains
*  allocated till psm_object_close() is called. If instead
*  psm_set_variable() or psm_get_variable() is used no heap is used.
*
*  \section psm-v2_thread Thread safety
*  PSM module is designed to be completely thread safe. Even across
*  multiple instances of PSM module (i.e. multiple psm_module_init()
*  calls) on separate sections/partitions of flash. Only caveat is that low
*  level flash driver also needs take care of thread safety if multiple PSM
*  partitions are on the _same_ flash chip.
*
*  \section psm-v2_readonly Read only usage
*  In certain use cases it may be desired to use PSM in a read only
*  mode. One of the benefits of this is that reserved space is not
*  required and PSM can fit in lesser number of sectors. When the read only
*  flag is passed to the psm_module_init(), PSM will run in read only mode
*  for that partition and not allow any writes.
*
*  \section psm-v2_usage Usage
*
*  PSM provides two types of API's for separate use cases. First type
*  (Simple case) is when the caller has entire value in a buffer for write
*  or has a buffer size equal to entire data to be read. Second type (
*  Advanced case) is when caller needs  to read/write huge files in
*  multiple KB's or MB's. In this case the caller needs an API which helps
*  him read/write across multiple calls. Both the cases: are illustrated
*  below.
*
*  PSM initialization
*  ------------------
*  This needs to be done for both the simple and advanced case. It is
*  generally done only once after boot.
*
*  Statically/dynamically allocate and populate \ref flash_desc_t
*  object. Note that this object needs to persist until psm_module_deinit()
*  returns. PSM does _not_ make an internal copy of this object to avoid
*  wasting space.
*
*  \code
*  psm_hnd_t phandle;
*  int rv = psm_module_init(&ppart, &phandle, NULL);
*  if (rv != WM_SUCCESS) {
*    wmprintf("PSM init failed");
*    return rv;
*  }
*  \endcode
*
*  This handle 'phandle' can now be used for other API which require
*  it. The third parameter is passed as NULL here. If any special flags
*  need to be passed the \ref psm_cfg_t param can be passed accordingly.
*
*  PSM simple usage case
*  ---------------------
*  Write and read back a small variable.
*
*  \code
*  char var[] = "name";
*  char value[] = "Tim";
*  int rv = psm_set_variable(phandle, var, value, strlen(value));
*  if (rv != WM_SUCCESS) {
* 	wmprintf("Variable write failed");
* 	return rv;
*  }
*
*  char read_value[10];
*  rv = psm_get_variable(phandle, var, read_value, strlen(value));
*  if (rv != WM_SUCCESS) {
* 	wmprintf("Variable read failed");
* 	return rv;
*  }
*
*  \endcode
*
*  PSM advanced use case
*  ---------------------
*  Write a huge valued object
*
*  \code
*  char var[] = "hello.mp3";
*  char value[1024]; //Assume data is already in this buffer
*
*  psm_object_handle_t ohandle;
*  int rv = psm_object_open(phandle, var, PSM_MODE_WRITE, sizeof(value),
*  NULL, &ohandle);
*  if (rv != WM_SUCCESS) {
* 	//Failed
* 	return rv;
*  }
*  //Write the object in two chunks just to show valid usage pattern
*  rv = psm_object_write(ohandle, value, 512);
*  if (rv != WM_SUCCESS) {
* 	//Failed
* 	return rv;
*  }
*
*  rv = psm_object_write(ohandle, &value[512], 512);
*  if (rv != WM_SUCCESS) {
* 	//Failed *
* 	return rv;
*  }
*
*  rv = psm_object_close(&ohandle);
*  if (rv != WM_SUCCESS) {
* 	//Failed
* 	return rv;
*  }
*
*  \endcode
*
*  De initialize PSM
*  -----------------
*  This would generally be done once only during system shutdown.
*
*  \code
*  int rv = psm_module_deinit(&phandle);
*  if (rv != WM_SUCCESS) {
* 	//Failed
* 	return rv;
*  }
*
*  \endcode
*
*  \section psm-v2_index_caching Indexing and caching
*
*  Indexing and Caching are additional features to be used when fast
*  access to a PSM object is required. Both are to be enabled
*  separately per object. When indexing is enabled the address of the PSM
*  object is saved in RAM. This avoids a search for the object in the
*  flash. When caching is enabled the object name and value are cached in
*  RAM. This gives instant access to the values and NO flash is
*  accessed. When caching is enabled indexing is also automatically enabled
*  for that object.
*
*  Predictably caching consumes more RAM and amount of RAM consumed
*  depends on the size of the name and value of object. Indexing uses 14
*  bytes per object.
*
*  \section psm-v2_secure PSM Encryption (Secure PSM)
*
*  User may choose to store the data in PSM in encrypted format. If
*  encryption is enabled, data is encrypted before storing it into PSM
*  and decrypted when data is retrieved. PSMv2 currently uses AES-CTR for
*  encryption. The keys are retrieved from the secure keystore. If
*  encryption is enabled and secure keystore does not have the keys psm
*  initialization will fail. Each PSM object is encrypted separately
*  before storing to flash. The metadata is not encrypted. The name
*  field and data field is encrypted.
*
*  Note that, even if Secure PSM is enabled, users can still use PSM in
*  non-encrypted mode. This is useful when there are more than one
*  PSM partitions and only some of them are encrypted.
*
*/

#ifndef __PSMV2_H__
#define __PSMV2_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <flash.h>
#include <wmerrno.h>
#define PSM_MODULE_VER "2.0"

/** PSM Handle */
typedef void *psm_hnd_t;

/** PSM Object handle */
typedef void *psm_object_handle_t;

/** PSM opening mode */
typedef enum {
	/** Read mode */
	PSM_MODE_READ = 1,
	/** Write mode */
	PSM_MODE_WRITE = 2,
} psm_mode_t;


/** PSM Flags
 *
 * Required in \ref psm_objattrib_t. Please refer \ref psm-v2_index_caching
 * for details
 */
typedef enum {
	/** Indexing enabled */
	PSM_INDEXING_ENABLED = 0x01,
	/** Caching enabled */
	PSM_CACHING_ENABLED = 0x02,
} psm_flags_t;


/** PSM Object attributes
 *
 * This structure is accepted by psm_object_open()
 */
typedef struct {
	/** OR of the flags defined in \ref psm_flags_t */
	uint32_t flags;
} psm_objattrib_t;

/** PSM Configuration
 *
 * This structure is passed to psm_module_init(). PSMv2 won't need the user
 * copy of this structure after call to psm_module_init() returns.
 */
typedef struct {
	/** If read-only is enabled no writes will be possible to the
	partition */
	bool read_only;
	/**
	 * If 'secure' is set, the object will be encrypted before
	 * writing, and object will be decrypted before reading. If secure
	 * PSM is enabled KEY_PSM_ENCRYPT_KEY and KEY_PSM_NONCE needs to
	 * be present in the keystore or must be provided through following
	 * members. Without these parameters the PSM initialization will fail.
	 * This variable should not be set in case of mc200.
	 */
	bool secure;

#ifdef CONFIG_SECURE_PSM
	/**
	 * If 'keystore_override' is set, and secure is required, then it must
	 * use below key/iv field so that the keystore will be ignored.
	 * If 'keystore_override' is not set, and secure is required, then keystore
	 * key will be checked first before checking below key/iv field, whichever
	 * has valid key/iv will be used.
	 */
	bool keystore_override;

	/**
	 * 'key' from app to be used for encryption/decryption when applicable.
	 */
	const uint8_t *key;

	/**
	 * 'iv' from app to be used for encryption/decryption when applicable.
	 */
	const uint8_t *iv;

	/**
	 * 'key_sz' from app for the 'key' size in byte. typical 16.
	 */
	uint16_t key_sz;

	/**
	 * 'iv_sz' from app for the 'iv' size in byte. typical 16.
	 */
	uint16_t iv_sz;
#endif /* CONFIG_SECURE_PSM */
} psm_cfg_t;

/** Part Info types */
enum part_info_type {
	/** part_info.name is valid */
	PART_INFO_NAME = 0,
	/** part_info.fl is valid */
	PART_INFO_FL_DESC = 1,
};

/** PSM init partition info
 *
 * This structure is passed to psm_module_init_ex() to specify partition
 * details. It allows app to specify an arbitrary flash space
 * or an existing partition name in addition to the PSM configuration for
 * this partition. PSMv2 won't need the user copy of this structure or
 * any pointer within this structure after call to psm_module_init_ex()
 * returns.
 */
typedef struct {
	/** part info type below  */
	enum part_info_type pitype;
	union {
		/** 'name' is valid if 'pitype'= PART_INFO_NAME */
		const char *name;
		/** 'fl' is valid if 'pitype'= PART_INFO_FL_DESC */
		const flash_desc_t *fl;
	} part_info;

	/** PSM Configuration settings for this init partition */
	psm_cfg_t *psm_cfg;
} psm_init_part_t;

/**
 * Initialize the PSM module
 *
 * This API initializes the PSM module. The given flash space will
 * be accessed and initialized if necessary. Since this function is the
 * first one called after a power cycle, if a flash operation was in
 * progress recovery actions will be performed.
 *
 * @note If secure PSM is enabled KEY_PSM_ENCRYPT_KEY and KEY_PSM_NONCE
 * needs to be present in the keystore or can optionally be provided
 * through \t ref psm_cfg_t structure.
 *
 * @param[in] fdesc Populated flash descriptor \ref flash_desc_t having
 * partition details. Please refer to \ref partition.h for relevant APIs.
 * @param[out] phandle Pointer to the handle \ref psm_hnd_t. Populated by PSM.
 * @param[in] psm_cfg \ref psm_cfg_t PSM configuration structure. If NULL
 * is passed default configuration i.e. read/write mode is used.
 *
 * @return WM_SUCCESS Init success.
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_E_NOMEM Memory allocation failure.
 * @return -WM_FAIL If any other error occurs.
 */
int psm_module_init(flash_desc_t *fdesc,
		    psm_hnd_t *phandle, psm_cfg_t *psm_cfg);

/**
 * Initialize the PSM module with specified partition name or flash space.
 *
 * This API initializes the PSM module with the enhancement that
 * partition details can be specified either as a partition name
 * from partition table or as flash descriptor itself.
 *
 * The given flash spaces will be accessed and initialized if necessary.
 * Since this function is the first one called after a power cycle,
 * if a flash operation was in progress recovery actions will be performed.
 *
 * @note If secure PSM is enabled KEY_PSM_ENCRYPT_KEY and KEY_PSM_NONCE
 * needs to be present in the keystore or can optionally be provided
 * through \t ref psm_cfg_t structure that is part of \ref
 * psm_init_part_t structure.
 *
 * @note there is no corresponding psm_module_deinit_ex() API required.
 * Use existing \ref psm_module_deinit() instead.
 *
 * @param[in] psm_init_part Pointer to \ref psm_init_part_t structure having
 * partition details. PSMv2 won't need the user copy of any field in
 * psm_init_part after call returns.
 * @param[out] phandle Pointer to the handle \ref psm_hnd_t. Populated by PSM.
 *
 * @return WM_SUCCESS Init success.
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_E_NOMEM Memory allocation failure.
 * @return -WM_FAIL If any other error occurs.
 */
int psm_module_init_ex(const psm_init_part_t *psm_init_part,
			psm_hnd_t *phandle);

/**
 * Open an existing or a new PSM object
 *
 * This API allows the caller to open a new or existing PSM object.
 * This call is essential before called psm_object_read() or
 * psm_object_write().
 *
 * \note If write mode is specified and if an object with the same
 * name is already present, the existing object will be deleted
 * and replaced with the new one.
 *
 * @param[in] phandle Handle returned in earlier call to psm_module_init()
 * @param[in] name NULL terminated ASCII name of PSM object.
 * @param[in] mode Specify read mode (\ref PSM_MODE_READ) or a write
 * mode(\ref PSM_MODE_WRITE).
 * @param[in] max_data_len Valid only for write mode. This specifies the
 * maximum length of the data the caller expect to write across single or
 * multiple calls of psm_object_write(). It is okay if the caller later
 * actually writes less number of bytes to be object than max_data_len
 * given here. For e.g. Giving 'max_data_len' 20 but writing only 4 bytes
 * is perfectly alright. The remaining  16 bytes will be released to the
 * free space pool once this object is closed.
 * @param[in] attribs Additional attributes as per \ref psm_objattrib_t.
 * Pass NULL if not required.
 * @param[out] ohandle Pointer to Object handle. Will be populated by
 * psm_object_open() if open operation was successful. This needs to be
 * passed to other psm read/write API's.
 *
 * @return For read mode: If open is successful size of the object is returned.
 *         For write mode: If open is successful WM_SUCCESS is returned.
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_E_NOMEM Memory allocation failure.
 * @return -WM_E_NOSPC If enough space is unavailable for the new
 * object. This error can only be returned in write mode.
 * @return -WM_FAIL If any other error occurs.
 */
int psm_object_open(psm_hnd_t phandle, const char *name,
		    psm_mode_t mode, uint32_t max_data_len,
		    psm_objattrib_t *attribs,
		    psm_object_handle_t *ohandle);

/**
 * Read data of an existing and opened object
 *
 * This API allows the caller to read data from an object. This
 * call can be invoked as many times as the caller wants till the data read
 * finishes. Offset from where data is read is automatically incremented
 * depending on the size read. When data read is over, zero is returned.
 *
 * @pre psm_object_open()
 *
 * @note To avoid double pass over flash data area, CRC calculations are done
 * on-the-go as data is being read progressively by the caller. CRC
 * verification is done only in the last call to psm_object_read(). The
 * downside to this is that whole object needs to be read by caller before
 * object integrity can be confirmed.
 *
 * @note This API will block if other writers are currently using
 * the PSM in write mode.
 *
 * @param[in] ohandle Handle returned from call to psm_object_open().
 * @param[out] buf Destination buffer of data read from flash. Ensure that
 * it is atleast equal to max_len
 * @param[in] max_len Length of the buffer passed as second parameter. Data
 * read can be less than this value is end of object is reached.
 *
 * @return Non-zero positive value equal to number of bytes written to user
 * given buffer
 * @return 0 No more data available.
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_E_CRC If CRC check failed.
 * @return -WM_FAIL If any other error occurs.
 */
int psm_object_read(psm_object_handle_t ohandle, void *buf, uint32_t max_len);

/**
 * Write data to an opened PSM object.
 *
 * This API allows the caller to write data to an object. This
 * call can be invoked as many times as the caller wants as long as total
 * size written is less that 'max_len' specified during
 * psm_object_open(). Data passed in every invocation is appended to the
 * existing data already written.
 *
 * @pre psm_object_open()
 *
 * @note This API will block if other readers/writers are currently using
 * the PSM.
 *
 * @param[in] ohandle Handle returned from call to psm_object_open().
 * @param[in] buf Pointer to buffer containing data to be written to flash.
 * @param[in] len Length of the data in the buffer passed.
 *
 * @return WM_SUCCESS Data was written successfully.
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_E_PERM If object was opened in read mode.
 * @return -WM_E_NOSPC If caller tries to write more data than specified in
 * call to psm_object_open().
 * @return -WM_FAIL If any other error occurs.
 */
int psm_object_write(psm_object_handle_t ohandle,
		     const void *buf, uint32_t len);

/**
 * Close the PSM object
 *
 * This API will close the earlier opened PSM object. The object
 * is finalized in the flash only after this call.
 *
 * @pre psm_object_open()
 *
 * @param[in] ohandle Handle returned from earlier call to
 * psm_object_open()
 *
 * @return WM_SUCCESS Object was closed successfully.
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_FAIL If any other error occurs.
 */
int psm_object_close(psm_object_handle_t *ohandle);


/**
 * Delete a PSM object
 *
 * This API deletes an object from the PSM.
 *
 * @param[in] phandle Handle returned from earlier call to
 * psm_module_init()
 * @param[in] name NULL terminated ASCII name of the PSM object to be
 * deleted.
 *
 * @note This API will block if other readers/writers are currently using
 * the PSM.
 *
 * @return WM_SUCCESS Object was deleted successfully.
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_E_NOSPC If caller tries to write more data than specified in
 * call to psm_object_open().
 * @return -WM_FAIL If any other error occurs.
 */
int psm_object_delete(psm_hnd_t phandle, const char *name);

/**
 * Format the PSM partition
 *
 * This API allows the caller to erase and initialize the PSM
 * partition. All the existing objects will be erased.
 *
 * @note This API will block if other readers/writers are currently using
 * the PSM.
 *
 * @param[in] phandle Handle to PSM returned earlier from the call to
 * psm_module_init()
 *
 * @return WM_SUCCESS Format operation was successful.
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_FAIL If any other error occurs.
 */
int psm_format(psm_hnd_t phandle);

/**
 * This callback will be invoked 'n' times if there are 'n' total variables
 * in the PSM. The user can call any API to read the PSM variables in the
 * callback. Any write operation to PSM will cause a deadlock if done in
 * the callback implementation.
 *
 * If the callback implementation returns a non-zero value no futher
 * callbacks will be given. This can be useful when the callback has found
 * what it was searching for and does not need remaining variables. The
 * callback is invoked till all variables are done or callback returns
 * WM_SUCCESS, whichever comes first.
 */
typedef int (*psm_list_cb)(const uint8_t *name);

/**
 * Print all the objects and their data to console.
 *
 * @param[in] phandle Handle to PSM returned earlier from the call to
 * psm_module_init()
 * @param[in] list_cb Callback invoked by this API to pass on variable
 * names one by one to the user.
 *
 * @return void
 */
void psm_objects_list(psm_hnd_t phandle, psm_list_cb list_cb);

/*
 * Check if PSM module is encrypted.
 *
 * @param[in] phandle Handle to PSM returned earlier from the call to
 * psm_module_init()
 *
 * @return 'true' if PSM is encrypted or 'false' otherwise.
 */
bool is_psm_encrypted(psm_hnd_t phandle);

/**
 * Set a variable value pair
 *
 * This API is built on top of existing
 * psm_object_open(), psm_object_write() and psm_object_close()
 * APIs. This API is preferable when small sized objects
 * are being written.
 *
 * @note psm_object_open() and psm_object_close() are not required to be
 * called if this API is used.
 *
 * @param[in] phandle Handle to PSM returned earlier from the call to
 * psm_module_init()
 * @param[in] variable Name of the object.
 * @param[in] value Pointer to buffer having the data to be written
 * @param[in] len Length of the data to be written.
 *
 * @return WM_SUCCESS Variable-value pair was set successfully.
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_FAIL If any other error occurs.
 */
int psm_set_variable(psm_hnd_t phandle, const char *variable,
		     const void *value, uint32_t len);

/**
 * Helper function to simplify set of NULL terminated value.
 *
 * psm_set_variable_str() will self calculate the length of the NULL
 * terminated string sent by caller.
 *
 * @param[in] phandle See documentation of psm_set_variable()
 * @param[in] variable See documentation of psm_set_variable()
 * @param[in] value See documentation of psm_set_variable()
 *
 * @return See documentation of psm_set_variable()
 */
static inline int psm_set_variable_str(psm_hnd_t phandle, const char *variable,
		     const void *value)
{
	return psm_set_variable(phandle, variable, value,
				strlen((const char *)value));
}

/**
 * Helper function to set integer value to PSM
 *
 * This function will convert given int to string representation and store
 * it to PSM. psm_get_variable_int() can be used to read the value back
 * again in int format.
 *
 * for e.g. 1234 given by the user will be stored as "1234"
 *
 * @param[in] phandle See documentation of psm_get_variable()
 * @param[in] variable See documentation of psm_get_variable()
 * @param[out] value The value to be stored to PSM
 *
 * @return See documentation of psm_set_variable()
 */
static inline int psm_set_variable_int(psm_hnd_t phandle, const char *variable,
		     int value)
{
	char int_s[14];

	snprintf(int_s, sizeof(int_s), "%d", value);
	return psm_set_variable_str(phandle, variable, int_s);
}


/**
 * Get a variable value pair
 *
 * This API is built on top of existing
 * psm_object_open(), psm_object_read() and psm_object_close()
 * APIs. This API is preferable when small sized objects
 * are being read.
 *
 * @note psm_object_open() and psm_object_close() are not required to be
 * called if this API is used.
 *
 * @param[in] phandle Handle to PSM returned earlier from the call to
 * psm_module_init()
 * @param[in] variable Name of the object.
 * @param[out] value Pointer to buffer where value will be written by PSM.
 * @param[in] max_len Length of the buffer passed as third parameter. If
 * the buffer is not enough to fit the whole object error will be
 * returned. Partial read will not be done.
 *
 * @note Note that this function returns the length of the value. It will
 * \b not< NULL terminate the returned data.
 *
 * @return If operation is a success then length of the value field will be
 * returned.
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_E_NOSPC If variable value exceeds buffer length given.
 * @return -WM_FAIL If any other error occurs.
 */
int psm_get_variable(psm_hnd_t phandle, const char *variable,
		     void *value, uint32_t max_len);

/**
 * Get size of a PSM variable
 *
 * No changes are done to the PSM.
 *
 * @param[in] phandle Handle to PSM returned earlier from the call to
 * psm_module_init()
 * @param[in] variable Name of the variable.
 *
 * @return Zero or positive value of size of variable if variable is present.
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_FAIL if object not found or any error occurred.
 */
int psm_get_variable_size(psm_hnd_t phandle, const char *variable);

/**
 * Check if a variable is present
 *
 * No changes are done to the PSM.
 *
 * @param[in] phandle Handle to PSM returned earlier from the call to
 * psm_module_init()
 * @param[in] variable Name of the variable.
 *
 * @return 'true' if variable is present.
 * @return 'false' if variable is absent or some other error.
 */
static inline bool psm_is_variable_present(psm_hnd_t phandle,
					   const char *variable)
{
	return (psm_get_variable_size(phandle, variable) >= 0);
}

/**
 * Helper function to simplify get of NULL terminated value.
 *
 * This function will automatically NULL terminate the byte string
 * retrieved from PSM. One additional byte should be kept reserved by the
 * caller for this NULL terminator.
 *
 * @param[in] phandle See documentation of psm_get_variable()
 * @param[in] variable See documentation of psm_get_variable()
 * @param[out] value See documentation of psm_set_gariable()
 * @param[in] max_len See documentation of psm_get_variable()
 *
 * @return See documentation of psm_get_variable()
 */
static inline int psm_get_variable_str(psm_hnd_t phandle, const char *variable,
		     void *value, uint32_t max_len)
{
	if (!max_len)
		return -WM_E_INVAL;

	uint8_t *valp = (uint8_t *)value;

	int ret = psm_get_variable(phandle, variable, valp, max_len - 1);
	if (ret >= 0) /* NULL terminate */
		valp[ret] = 0;

	return ret;
}

/**
 * Helper function to get integer value from PSM
 *
 * This function will automatically read string representation of a number
 * from PSM and convert it to int format before returning.
 *
 * for e.g. "1234" stored in PSM will be returned as 1234 in the user given
 * int variable.
 *
 * @param[in] phandle See documentation of psm_get_variable()
 * @param[in] variable See documentation of psm_get_variable()
 * @param[out] value The value returned.
 *
 * @return WM_SUCCESS if retrieval was successful
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_FAIL If any other error occurs.
 */
static inline int psm_get_variable_int(psm_hnd_t phandle, const char *variable,
		     int *value)
{
	char int_s[14];

	int ret = psm_get_variable_str(phandle, variable, int_s, sizeof(int_s));
	if (ret < 0)
		return ret;

	*value = strtol(int_s, NULL, 0);
	return WM_SUCCESS;
}

/**
 * De-initialize the PSM module
 *
 * This API unloads the PSM module from handling the flash partition.
 *
 * @note This API will block till other readers/writers are currently using
 * the PSM.
 *
 * @param[in] phandle Handle to the PSM obtained earlier through
 * psm_module_init()
 *
 * @return WM_SUCCESS Module de-init success.
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_FAIL If any other error occurs.
 */
int psm_module_deinit(psm_hnd_t *phandle);

#endif /* __PSMV2_H__ */
