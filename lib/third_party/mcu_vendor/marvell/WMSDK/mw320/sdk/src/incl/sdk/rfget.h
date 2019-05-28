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


/*! \file rfget.h
 *  \brief Remote Firmware Upgrade
 *
 * This module provides APIs for getting an upgrade image over the network
 * and updating on-flash firmware image. WMSDK supports redundant image
 * based firmware upgrades whereby there are two partitions in the flash to
 * hold the firmware: active and passive. Firmware upgrade process always
 * writes new image on passive partition and finally modifies partition
 * table to mark it as the new active partition. This then results into boot
 * from new firmware image when device reboots.
 *
 * This module provides APIs at different levels.
 *
 * The simplest APIs to use are the \ref rfget_high. Applications can directly
 * download and upgrade the firmware over a secure TLS connection by using
 * rfget_client_mode_update() and passing appropriate \ref httpc_cfg_t
 * structure. HTTP server mode upgrade can be performed using the
 * rfget_server_mode_update() API.
 *
 * If there is not enough memory for TLS, APIs that use other security
 * mechanisms like ED-CHACHA (\ref fw_upgrade_ed_chacha.h) or
 * RSA-AESCTR (\ref fw_upgrade_rsa_aes.h) are also available.
 * These APIs ensure that the upgrade images are encrypted and
 * signed, thus ensuring comparable security as TLS.
 * If applications want to use some other crypto combination besides
 * ED/CHACHA and AES/RSA, they can add it by using the generic secure upgrade
 * mechanism specified in \ref secure_upgrade.h
 *
 * If an application does not want to use HTTP, but some other deta fetch
 * mechanism, the \ref rfget_middle can be used. These are available for MCU
 * Firmware (update_and_validate_firmware()), Wi-Fi firmware
 * (update_and_validate_wifi_firmware())and File System
 * (update_and_validate_fs())
 *
 * For an even granular control, the \ref rfget_low should be used. These will
 * be especially useful if some application specific partition is to be
 * upgraded.
 *
 * Other RFGET APIs can be found in \ref rfget_other section.
 *
 */


#ifndef _RFGET_H_
#define _RFGET_H_

#include <wmtypes.h>
#include <wmerrno.h>
#include <wmstdio.h>
#include <mdev.h>
#include <flash.h>
#include <wmlog.h>
#include <ftfs.h>
#include <partition.h>
//#include <httpc.h>

#define rf_e(...)				\
	wmlog_e("rf", ##__VA_ARGS__)
#define rf_w(...)				\
	wmlog_w("rf", ##__VA_ARGS__)

#ifdef CONFIG_RFGET_DEBUG
#define rf_d(...)				\
	wmlog("rf", ##__VA_ARGS__)
#else
#define rf_d(...)
#endif /* ! CONFIG_RFGET_DEBUG */

#define ftfs_l(...) wmlog("ftfs", ##__VA_ARGS__)
#define RFGET_PROTO_DEFAULT RFGET_HTTP_GET
/** RFGET Error Code */
enum wm_rfget_code {
	WM_RFGET_BASE = MOD_ERROR_START(MOD_RFGET),
	/** Protocol Not Supported */
	WM_E_RFGET_INVPRO,
	/** Failed to set 2msl time */
	WM_E_RFGET_F2MSL,
	/** Failed to send/recv http request */
	WM_E_RFGET_FHTTP_REQ,
	/** Failed to parse http header */
	WM_E_RFGET_FHTTP_PARSE,
	/** Invalid http status */
	WM_E_RFGET_INVHTTP_STATUS,
	/**  Error in rfget_sock_init */
	WM_E_RFGET_FSOCK,
	/** rfget length unknown or invalid*/
	WM_E_RFGET_INVLEN,
	/** Failed to init rfget*/
	WM_E_RFGET_FINIT,
	/** Firmware or ftfs Update failed */
	WM_E_RFGET_FUPDATE,
	/** Firmware or ftfs read fail */
	WM_E_RFGET_FRD,
	/** Firmware or ftfs write fail */
	WM_E_RFGET_FWR,
	/** Failed to load firmware */
	WM_E_RFGET_FFW_LOAD,
	/** Invalid file format */
	WM_E_RFGET_INV_IMG,
};

/**  rfget_low Low level RFGET APIs
 *
 * These APIs can be used for granular control on the upgrades or to
 * upgrade any application specific partition.
 * A simple flow will be in this order:
 * \code
 * rfget_init();
 * struct partition_entry *p = get_passive_partition(); //Write own function
 * update_desc_t ud;
 * rfget_update_begin(&ud, p);
 * rfget_update_data(&ud, data, data_len); //Loop to write all data
 * rfget_update_complete(&ud);
 * \endcode
 * Applications should use appropriate mechanism to verify that the written
 * image is valid. Various convenience APIs are also provided by this module
 * to help in upgrading the MCU Firmware or Wi-Fi firmware.
 * @{
 */

/** Update Description Structure
 *
 * This is referenced when using the rfget_update_ APIs. The contents of this
 * structure are populated and used by the rfget_update_ APIs.
 */
typedef struct update_desc {
	/** Pointer to partition entry structure
	 *  this is the partition being updated
	 */
	struct partition_entry *pe;
	/** MDEV handle to flash driver */
	mdev_t *dev;
	/** Address in flash where update occurs */
	uint32_t addr;
	/** Remaining space in partition being updated */
	uint32_t remaining;
} update_desc_t;

/** Initialize the RFGET module
 *
 * This API should be called before using any other low level
 * upgrade APIs
 *
 * \return WM_SUCCESS on success, error otherwise
 */
int rfget_init();

/** Begin an update
 *
 * This function erases the associated flash sector for the partition entry that
 * is passed to it. The ud is used to maintain information across the various
 * calls to the rfget_update_ functions.
 *
 * \param[out] ud the update descriptor structure
 * \param[in] p the partition entry of the passive partition
 * \return WM_SUCCESS on success, error otherwise
 */
int rfget_update_begin(update_desc_t *ud, struct partition_entry *p);

/** Write data to partition
 *
 * This should be called after a call to the rfget_update_begin(). Successive
 * calls to this function write data to the flash.
 *
 * \param[in] ud the update descriptor
 * \param[in] data the data to be written
 * \param[in] datalen the length of the data to be written
 * \return WM_SUCCESS on success, error otherwise
 */
int rfget_update_data(update_desc_t *ud, const char *data, size_t datalen);

/** Finish update
 *
 * Carrying out subsequent reboot is not handled by this API and is left
 * to the application.
 *
 * Mark this partition as the active partition and finish the update process.
 * \param[in] ud the update descriptor
 * \return WM_SUCCESS on success, error otherwise
 */
int rfget_update_complete(update_desc_t *ud);

/** Abort update in case of some error
 *
 * This function does not mark this partition as the active partition but
 * only cleans up the update process.
 * \param[in] ud the update descriptor
 * \return WM_SUCCESS on success, error otherwise
 */
int rfget_update_abort(update_desc_t *ud);
/**
 * This function tells whether system image update is in progress.
 *
 * @return If update is in progress returns TRUE else returns FALSE.
 */
bool rfget_is_update_in_progress(void);

/**
 * Verify if the MCU Firmware written to flash is valid
 *
 * \param[in] start The start address from which the firmware data starts
 * \param[in] size of the firmware image on flash
 *
 * \return WM_SUCCESS on success
 * \return -WM_E_RFGET_FFW_LOAD if an error is encountered
 */
int verify_load_firmware(uint32_t start, int size);
/** Convenience function to get the passive firmware partition
 *
 * \return Pointer of type \ref partition_entry for the
 * passive firmware partition.
 */
struct partition_entry *rfget_get_passive_firmware(void);
/** Convenience function to get the passive wifi firmware partition
 *
 * \return Pointer of type \ref partition_entry for the
 * passive wifi firmware partition.
 */
struct partition_entry *rfget_get_passive_wifi_firmware(void);
/** Convenience function to get the passive BT firmware partition
 *
 * \return Pointer of type \ref partition_entry for the
 * passive BT firmware partition.
 */
struct partition_entry *rfget_get_passive_bt_firmware(void);
/** Convenience function to get the passive ftfs partition
 *
 * \return Pointer of type \ref partition_entry for the
 * passive ftfs partition.
 */
struct partition_entry *rfget_get_passive_ftfs(void);
/** @} */

/**  rfget_middle Middle level RFGET APIs
 *
 * The middle level APIs provide APIs for MCU firmware, Wi-Fi firmware
 * and File system upgrades by abstracting out the low level details.
 * These APIs do not put any restriction on the method/protocol used to
 * fetch the data.
 *
 * The convenience APIs rfget_get_passive_firmware() and
 * rfget_get_passive_wifi_firmware() can be used to get the passive partitions.
 *
 * \note Carrying out subsequent reboot is not handled by any of the APIs
 * and is left to the application.
 * @{
 */
/**
 * Data Fetch callback function
 *
 * Callback function pointer passed to the update_and_validate_firmware(),
 * update_and_validate_wifi_firmware() or update_and_validate_fs()
 * functions.
 * This is needed because the update file may be too big to fit in a buffer
 * at a time. The rfget module will call this API repeatedly to fetch the
 * data. Applications should write appropriate function for this.
 *
 * \param[in,out] priv A private data pointer that will be used/modified by
 * this function to maintain a context. This will be initialized with
 * the pointer passed to update_and_validate_firmware(),
 * update_and_validate_wifi_firmware() or update_and_validate_fs()
 * \param[out] buf Pointer to a buffer that should be used by this function
 * to read the data
 * \param[in] len Length of the data requested
 *
 * \return Length of the data actually read
 * \return -WM_FAIL if any error is encountered
 */
typedef size_t(*data_fetch) (void *priv, void *buf, size_t len);
/**
 * Data validate callback function
 *
 * Optional callback function pointer passed to the
 * update_and_validate_firmware(), update_and_validate_wifi_firmware()
 * or update_and_validate_fs() function.
 * This may be required as a security measure to validate the received data.
 * Applications should write appropriate function for this.
 *
 * \param[in] priv The private data pointer used/modified by data_fetch
 * callback
 *
 * \return WM_SUCCESS if validation succeeds
 * \return -WM_FAIL if validation fails.
 */
typedef int(*data_validate) (void *priv);

/** Update and validate firmware image
 *
 * This API can be used to update and validate the MCU firmware without
 * worrying about any lower level details.
 *
 * \param[in] data_fetch_cb A callback of type \ref data_fetch that
 * will internally be used by this API to fetch the firmware data.
 * \param[in] priv Pointer to private data that will be passed to
 * the \ref data_fetch and data_validate functions.
 * \param[in] firmware_length Length of the firmware image that needs to
 * be written to flash.
 * \param[in] passive Pointer of type \ref partition_entry to the
 * passive partition to be updated
 * \param[in] data_validate_cb An optional pointer to a function of type
 * \ref data_validate that will be called at the end, to validate the
 * written data. This can be kept NULL if validation is not required.
 *
 * \return WM_SUCCESS on success
 * \return -WM_E_RFGET_INVLEN if the passed firmware_length is invalid
 * (either 0 or more than the partition size)
 * \return -WM_E_RFGET_INV_IMG if the received image is not valid firmware
 * image
 * \return -WM_E_RFGET_FWR if the passive partition write/erase failed
 * \return -WM_E_RFGET_FFW_LOAD if firmware verification failed
 * \return -WM_FAIL if any other error encountered
 * \return any other error returned by the data_validate function
 */
int update_and_validate_firmware(data_fetch data_fetch_cb, void *priv,
		    size_t firmware_length, struct partition_entry *passive,
		    data_validate data_validate_cb);

/** Update and validate Wi-Fi firmware image
 *
 * This API can be used to update and validate the Wi-Fi firmware without
 * worrying about any lower level details.
 *
 * \param[in] data_fetch_cb A callback of type \ref data_fetch that
 * will internally be used by this API to fetch the wifi firmware data.
 * \param[in] priv Pointer to private data that will be passed to
 * the \ref data_fetch and data_validate functions.
 * \param[in] firmware_length Length of the wifi firmware image that needs to
 * be written to flash.
 * \param[in] passive Pointer of type \ref partition_entry to the
 * passive wifi partition to be updated
 * \param[in] data_validate_cb An optional pointer to a function of type
 * \ref data_validate that will be called at the end, to validate the
 * written data. This can be kept NULL if validation is not required.
 *
 * \return WM_SUCCESS on success
 * \return -WM_E_RFGET_INVLEN if the passed firmware_length is invalid
 * (either 0 or more than the partition size)
 * \return -WM_E_RFGET_FWR if the passive partition write/erase failed
 * \return -WM_FAIL if any other error encountered
 * \return any other error returned by the data_validate function
 */
int update_and_validate_wifi_firmware(data_fetch data_fetch_cb, void *priv,
		    size_t firmware_length, struct partition_entry *passive,
		    data_validate data_validate_cb);

/** Update and validate filesystem image
 *
 * This API can be used to update and validate the Filesystem without
 * worrying about any lower level details.
 *
 * \param[in] data_fetch_cb A callback of type \ref data_fetch that
 * will internally be used by this API to fetch the filesystem data.
 * \param[in] priv Pointer to private data that will be passed to
 * the \ref data_fetch and data_validate functions.
 * \param[in] fs_length Length of the filesystem image that needs to
 * be written to flash.
 * \param[in] passive Pointer of type \ref partition_entry to the
 * passive partition to be updated
 * \param[in] data_validate_cb An optional pointer to a function of type
 * \ref data_validate that will be called at the end, to validate the
 * written data. This can be kept NULL if validation is not required.
 *
 * \return WM_SUCCESS on success
 * \return -WM_E_RFGET_INVLEN if the passed fs_length is invalid
 * (either 0 or more than the partition size)
 * \return -WM_E_RFGET_INV_IMG if the received image is not valid filesystem
 * image
 * \return -WM_E_RFGET_FWR if the passive partition write/erase failed
 * \return -WM_FAIL if any other error encountered
 * \return any other error returned by the data_validate function
 */
int update_and_validate_fs(data_fetch data_fetch_cb, void *priv,
		    size_t fs_length, struct partition_entry *passive,
		    data_validate data_validate_cb);

/** Update and validate BT firmware image
 *
 * This API can be used to update and validate the BT firmware without
 * worrying about any lower level details.
 *
 * \param[in] data_fetch_cb A callback of type \ref data_fetch that
 * will internally be used by this API to fetch the BT firmware data.
 * \param[in] priv Pointer to private data that will be passed to
 * the \ref data_fetch and data_validate functions.
 * \param[in] firmware_length Length of the BT firmware image that needs to
 * be written to flash.
 * \param[in] passive Pointer of type \ref partition_entry to the
 * passive wifi partition to be updated
 * \param[in] data_validate_cb An optional pointer to a function of type
 * \ref data_validate that will be called at the end, to validate the
 * written data. This can be kept NULL if validation is not required.
 *
 * \return WM_SUCCESS on success
 * \return -WM_E_RFGET_INVLEN if the passed firmware_length is invalid
 * (either 0 or more than the partition size)
 * \return -WM_E_RFGET_FWR if the passive partition write/erase failed
 * \return -WM_FAIL if any other error encountered
 * \return any other error returned by the data_validate function
 */
int update_and_validate_bt_firmware(data_fetch data_fetch_cb, void *priv,
		    size_t firmware_length, struct partition_entry *passive,
		    data_validate data_validate_cb);

/** \deprecated Update firmware image
 *
 * Please use update_and_validate_firmware()
 *
 * \param[in] data_fetch_cb A callback of type \ref data_fetch that
 * will internally be used by this API to fetch the firmware data.
 * \param[in] priv Pointer to private data that will be passed to
 * the \ref data_fetch and data_validate functions.
 * \param[in] firmware_length Length of the firmware image that needs to
 * be written to flash.
 * \param[in] passive Pointer of type \ref partition_entry to the
 * passive partition to be updated
 *
 * \return Please refer update_and_validate_firmware()
 *
 */
int update_firmware(data_fetch data_fetch_cb, void *priv,
		    size_t firmware_length, struct partition_entry *passive);

/** \deprecated Update Wi-Fi firmware image
 *
 * Please use update_and_validate_wifi_firmware()
 *
 * \param[in] data_fetch_cb A callback of type \ref data_fetch that
 * will internally be used by this API to fetch the wifi firmware data.
 * \param[in] priv Pointer to private data that will be passed to
 * the \ref data_fetch and data_validate functions.
 * \param[in] firmware_length Length of the wifi firmware image that needs to
 * be written to flash.
 * \param[in] passive Pointer of type \ref partition_entry to the
 * passive wifi partition to be updated
 *
 * \return Please refer update_and_validate_wifi_firmware()
 */
int update_wifi_firmware(data_fetch data_fetch_cb, void *priv,
		    size_t firmware_length, struct partition_entry *passive);

/** @} */
/**  rfget_high High level RFGET APIs
 *
 * There are two different High level APIs provided by this module:
 * \li Server mode upgrade: In this method rfget module receives the
 * image data through an already opened socket connection with a peer. This
 * mode is agnostic to image delivery protocol. This is covered by
 * \ref rfget_server_mode_update API.
 * \li Client mode upgrade: In this method wireless microcontroller device
 * acts as a HTTP client to receive the image from other web server using
 * specified URL. \ref rfget_client_mode_update API covers this functionality.
 *
 * In both the cases the image is written to flash using following
 * process:
 * -# Receive firmware image header and verify signature
 * -# Identify passive partition using partition table and erase it.
 *  Convenience APIs rfget_get_passive_firmware() and
 *  rfget_get_passive_wifi_firmware() have been provided to get the passive
 *  MCU Firmware and Wi-Fi firmware partitions.
 * -# Receive image data and write it as received in parts
 * -# Update partition table
 *
 * As of now these functions support the MCU firmware, Wi-Fi firmware and
 * filesystem upgrades. If an application specific component needs to be
 * upgraded, the lower level APIs given in \ref rfget_low should be used.
 *
 * \note Carrying out subsequent reboot is not handled by any of the APIs and
 * is left to the application.
 * @{
 */
/**
 * Update a system image in server mode using http server.
 *
 * This function updates a system image. i.e. a firmware or filesystem
 * image. This function reads the file from the HTTP server side socket
 * 'sockfd'
 *
 * @param[in] sockfd Server side socket file descriptor. This function will
 * read 'filelength' bytes from this socket and write it to appropriate
 * location depending on the partition
 * @param[in] filelength Number of bytes to be read from socket and written
 * to flash.
 * @param[in] passive the passive partition that should be upgraded.
 *
 * @return On success returns WM_SUCCESS. On error returns appropriate
 * error code.
 * \note This API should be called only after stripping off HTTP header from
 * received packets. The sockfd should contain the data starting with image
 * header.
 */
int rfget_server_mode_update(int sockfd, size_t filelength,
			     struct partition_entry *passive);

/**
 * Update a system image in client mode using http client.
 *
 * This function updates a system image. for e.g. a firmware
 * image. This function reads the resource given as a part of the URL.
 *
 * @param[in] url_str URL to the image.
 * @param[in] passive the passive partition that should be upgraded.
 * @param[in] cfg is a pointer of the type \ref httpc_cfg_t. Please refer
 * to file \link httpc.h \endlink for more information.
 *
 * @return On success returns WM_SUCCESS. On error returns appropriate
 * error code.
 *
int rfget_client_mode_update(const char *url_str,
			     struct partition_entry *passive,
			     const httpc_cfg_t *cfg);
 */
/**  rfget_other Other RFGET APIs
 *
 * This section provides any additional convenience APIs related to RFGET.
 * @{
 */

/**
 * This function registers the CLI for the rfget application.
 *
 * @return On success returns WM_SUCCESS. On error returns appropriate
 * error code.
 */
int rfget_cli_init(void);

/** Initialize Upgradeable FTFS
 *
 * If you wish to use upgradeable FTFS you should use this function. If
 * you have FTFS partitions that do not require upgradability, please use
 * ftfs_init() instead.
 *
 * Given the backend API version this function will mount the appropriate FTFS
 * from the active-passive pair. The FTFS with the highest generation number
 * that matches the be_ver is mounted.
 *
 * \param[out] sb pointer to a ftfs_super structure
 * \param[in] be_ver backend API version
 * \param[in] name name of the FTFS component in the layout
 * \param[out] passive a pointer to the passive component
 *
 * \return a pointer to struct fs. The pointer points to an entry within the
 * 'sb' parameter that is passed as an argument.
 * \return NULL in case of an error
 *
 */
struct fs *rfget_ftfs_init(struct ftfs_super *sb, int be_ver, const char *name,
			   struct partition_entry **passive);

/** @} */
#endif
