/*****************************************************************************
**
**  Name:          platform_bt_nvram_interface.h
**
**  Description:   Internal definitions platform bt nv storage call out functions
**
**
**  Copyright (c) 2019, Cypress Semiconductor Corp, All Rights Reserved.
**  Cypress Proprietary and confidential.
******************************************************************************/
#pragma once
#include "wiced_bt_dev.h"

#define BT_BASE_KEY 140

/** NVRAM entry for bonded device */
typedef struct PACKED {
    wiced_bt_device_address_t   bd_addr;        /**< Device address */
    uint16_t                    length;         /**< Length of key_blobs (link key information) */
    uint8_t                     key_blobs[1];   /**< Link keys (actual length specified by 'length' field) */
} platform_bt_nvram_access_entry_t;


/**
 * Function     platform_bt_nvram_access_init_t
 *
 * Initialize NVRAM subsystem
 *
 * @param[in]  void
 *
 * @return  void
 */
typedef void (platform_bt_nvram_access_init_t)(void);

/**
 * Function     platform_bt_nvram_access_get_bonded_devices_t
 *
 * Get list of bonded devices stored in NVRAM
 *
 * @param[in]  paired_device_list : array for getting bd address of bonded devices
 * @param[in/out] p_num_devices :  list size of paired_device_list/total number of bonded devices stored
 *
 * @return      WICED_SUCCESS or ERROR
 */
typedef wiced_result_t (platform_bt_nvram_access_get_bonded_devices_t)(wiced_bt_device_address_t paired_device_list[], uint16_t *p_num_devices);

/**
 * Function     platform_bt_nvram_access_save_bonded_device_key_t
 *
 * Save link key information to NVRAM
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[in]  p_keyblobs : key blobs including key header, link keys and key length
 * @param[in]  key_len :  total length of p_keyblobs
 *
 * @return      WICED_SUCCESS or ERROR
 */
typedef wiced_result_t (platform_bt_nvram_access_save_bonded_device_key_t)(wiced_bt_device_address_t bd_addr, uint8_t *p_keyblobs, uint16_t key_len);

/**
 * Function     platform_bt_nvram_access_load_bonded_device_keys_t
 *
 * Load link key information from NVRAM
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[out] p_key_entry :  key information stored
 * @param[in]  entry_max_length : max length can be filled into p_key_entry
 *
 * @return      WICED_SUCCESS or ERROR
 */
typedef wiced_result_t (platform_bt_nvram_access_load_bonded_device_keys_t)(wiced_bt_device_address_t bd_addr, uint8_t *p_key_entry, uint8_t entry_max_length);


/**
 * Function     platform_bt_nvram_access_delete_bonded_device_t
 *
 * Delete link key information from NVRAM
 *
 * @param[in]  bd_addr : bd_addr of bonded device to be removed

 * @return      WICED_SUCCESS or ERROR
 */
typedef wiced_result_t (platform_bt_nvram_access_delete_bonded_device_t)(wiced_bt_device_address_t bd_addr);


/**
 * Function     platform_bt_nvram_access_load_local_identity_keys_t
 *
 * Load local identity keys including ir/irk/dhk stored in NVRAM
 *
 * @param[out]  p_lkeys: local identity key information
 *
 * @return      WICED_SUCCESS or ERROR
 */
typedef wiced_result_t (platform_bt_nvram_access_load_local_identity_keys_t)(wiced_bt_local_identity_keys_t *p_lkeys);

/**
 * Function     platform_bt_nvram_access_save_local_identity_keys_t
 *
 * Save local identity keys including ir/irk/dhk stored to NVRAM
 *
 * @param[in]  p_lkeys : local identity key information
 *
 * @return      WICED_SUCCESS or ERROR
 */
typedef wiced_result_t (platform_bt_nvram_access_save_local_identity_keys_t)(wiced_bt_local_identity_keys_t *p_lkeys);


/**
 * Function     platform_bt_nvram_access_key_storage_available_t
 *
 * Check if there is space available to store link keys for specified bd_addr
 * (May overwrite existing keys for bd_addr, if device was previously bonded)
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 *
 * @return      TRUE if there is available space or FALSE
 */
typedef wiced_bool_t (platform_bt_nvram_access_key_storage_available_t)(wiced_bt_device_address_t bd_addr);

/** NV access functions for saving/retrieving link keys */
typedef struct {
    platform_bt_nvram_access_init_t                        *init;                          /**< Initialize NVRAM subsystem */
    platform_bt_nvram_access_get_bonded_devices_t          *get_bonded_devices;            /**< Get list of bonded devices stored in NVRAM */
    platform_bt_nvram_access_save_bonded_device_key_t      *save_bonded_device_key;        /**< Save link key information to NVRAM */
    platform_bt_nvram_access_load_bonded_device_keys_t     *load_bonded_device_keys;       /**< Load link key information from NVRAM */
    platform_bt_nvram_access_delete_bonded_device_t        *delete_bonded_device;          /**< Delete link key information from NVRAM */
    platform_bt_nvram_access_load_local_identity_keys_t    *load_local_identity_keys;      /**< Load local identity keys including ir/irk/dhk stored in NVRAM */
    platform_bt_nvram_access_save_local_identity_keys_t    *save_local_identity_keys;      /**< Save local identity keys including ir/irk/dhk stored to NVRAM */
    platform_bt_nvram_access_key_storage_available_t       *key_storage_available;         /**< Check if there is space available to store link keys for specified bd_addr */
} platform_bt_nvram_interface_t;

/******************************************************
 *               Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Function         platform_bt_nvram_access_register
 *
 *                  Register/over-ride default NVRAM access functions for saving/retrieving link keys.
 *                  Must be called prior to stack initialization
 *
 * @param[in]       p_nvram_access  : NVRAM access functions
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t platform_bt_nvram_interface_register(platform_bt_nvram_interface_t *p_nvram_access);

#ifdef __cplusplus
}
#endif
