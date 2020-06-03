/*****************************************************************************
**
**  Name:          wiced_bt_nvram_access.h
**
**  Description:   Internal definitions wiced bt nv storage call out functions
**
**
**  Copyright (c) 2015, Broadcom Corp, All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
******************************************************************************/
#pragma once
#include "wiced_bt_dev.h"

/** NVRAM entry for bonded device */
typedef PACKED struct {
    wiced_bt_device_address_t   bd_addr;        /**< Device address */
    uint8_t                     addr_type;      /**< BLE_ADDR_PUBLIC or BLE_ADDR_RANDOM */
    uint8_t                     device_type;    /**< BT_DEVICE_TYPE_BREDR or BT_DEVICE_TYPE_BLE */
    uint16_t                    length;         /**< Length of key_blobs (link key information) */
    uint8_t                     key_blobs[1];   /**< Link keys (actual length specified by 'length' field) */
} wiced_bt_nvram_access_entry_t;


/**
 * Function     wiced_bt_nvram_access_init_t
 *
 * Initialize NVRAM subsystem
 *
 * @param[in]  void
 *
 * @return  void
 */
typedef void (wiced_bt_nvram_access_init_t)(void);

/**
 * Function     wiced_bt_nvram_access_get_bonded_devices_t
 *
 * Get list of bonded devices stored in NVRAM
 *
 * @param[in]  paired_device_list : array for getting bd address of bonded devices
 * @param[in/out] p_num_devices :  list size of paired_device_list/total number of bonded devices stored
 *
 * @return      WICED_SUCCESS or ERROR
 */
typedef wiced_result_t (wiced_bt_nvram_access_get_bonded_devices_t)(wiced_bt_dev_bonded_device_info_t paired_device_list[], uint16_t *p_num_devices);

/**
 * Function     wiced_bt_nvram_access_save_bonded_device_key_t
 *
 * Save link key information to NVRAM
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[in]  p_keyblobs : key blobs including key header, link keys and key length
 * @param[in]  key_len :  total length of p_keyblobs
 *
 * @return      WICED_SUCCESS or ERROR
 */
typedef wiced_result_t (wiced_bt_nvram_access_save_bonded_device_key_t)(wiced_bt_device_address_t bd_addr, wiced_bt_ble_address_type_t addr_type, uint8_t device_type, uint8_t *p_keyblobs, uint16_t key_len);

/**
 * Function     wiced_bt_nvram_access_load_bonded_device_keys_t
 *
 * Load link key information from NVRAM
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[out] p_key_entry :  key information stored
 * @param[in]  entry_max_length : max length can be filled into p_key_entry
 *
 * @return      WICED_SUCCESS or ERROR
 */
typedef wiced_result_t (wiced_bt_nvram_access_load_bonded_device_keys_t)(wiced_bt_device_address_t bd_addr, wiced_bt_nvram_access_entry_t *p_key_entry, uint8_t entry_max_length);


/**
 * Function     wiced_bt_nvram_access_delete_bonded_device_t
 *
 * Delete link key information from NVRAM
 *
 * @param[in]  bd_addr : bd_addr of bonded device to be removed

 * @return      WICED_SUCCESS or ERROR
 */
typedef wiced_result_t (wiced_bt_nvram_access_delete_bonded_device_t)(wiced_bt_device_address_t bd_addr);


/**
 * Function     wiced_bt_nvram_access_load_local_identity_keys_t
 *
 * Load local identity keys including ir/irk/dhk stored in NVRAM
 *
 * @param[out]  p_lkeys: local identity key information
 *
 * @return      WICED_SUCCESS or ERROR
 */
typedef wiced_result_t (wiced_bt_nvram_access_load_local_identity_keys_t)(wiced_bt_local_identity_keys_t *p_lkeys);

/**
 * Function     wiced_bt_nvram_access_save_local_identity_keys_t
 *
 * Save local identity keys including ir/irk/dhk stored to NVRAM
 *
 * @param[in]  p_lkeys : local identity key information
 *
 * @return      WICED_SUCCESS or ERROR
 */
typedef wiced_result_t (wiced_bt_nvram_access_save_local_identity_keys_t)(wiced_bt_local_identity_keys_t *p_lkeys);


/**
 * Function     wiced_bt_nvram_access_key_storage_available_t
 *
 * Check if there is space available to store link keys for specified bd_addr
 * (May overwrite existing keys for bd_addr, if device was previously bonded)
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[in]  req_size : requested size to be stored
 *
 * @return      TRUE if there is available space or FALSE
 */
typedef wiced_bool_t (wiced_bt_nvram_access_key_storage_available_t)(wiced_bt_device_address_t bd_addr,int req_size);


/**
 * Function     wiced_bt_nvram_access_enum_bonded_device_keys_t
 *
 * Load link key information from NVRAM by index
 *
 * @param[out]  p_index : index of stored key
 * @param[out]  p_key_entry : key information stored
 * @param[in]   entry_max_length : max length can be filled into p_key_entry
 * 
 * @return      WICED_SUCCESS or ERROR
 */
typedef wiced_result_t (wiced_bt_nvram_access_enum_bonded_device_keys_t)(int8_t *p_index, wiced_bt_nvram_access_entry_t * p_key_entry, uint8_t entry_max_length);


/** NV access functions for saving/retrieving link keys */
typedef struct {
    wiced_bt_nvram_access_init_t                        *init;                          /**< Initialize NVRAM subsystem */
    wiced_bt_nvram_access_get_bonded_devices_t          *get_bonded_devices;            /**< Get list of bonded devices stored in NVRAM */
    wiced_bt_nvram_access_save_bonded_device_key_t      *save_bonded_device_key;        /**< Save link key information to NVRAM */
    wiced_bt_nvram_access_load_bonded_device_keys_t     *load_bonded_device_keys;       /**< Load link key information from NVRAM */
    wiced_bt_nvram_access_delete_bonded_device_t        *delete_bonded_device;          /**< Delete link key information from NVRAM */
    wiced_bt_nvram_access_load_local_identity_keys_t    *load_local_identity_keys;      /**< Load local identity keys including ir/irk/dhk stored in NVRAM */
    wiced_bt_nvram_access_save_local_identity_keys_t    *save_local_identity_keys;      /**< Save local identity keys including ir/irk/dhk stored to NVRAM */
    wiced_bt_nvram_access_key_storage_available_t       *key_storage_available;         /**< Check if there is space available to store link keys for specified bd_addr */
    wiced_bt_nvram_access_enum_bonded_device_keys_t     *enum_bonded_device_keys;       /**< Load link key information from NVRAM by index */
} wiced_bt_nvram_access_t;

/******************************************************
 *               Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Function         wiced_bt_nvram_access_register
 *
 *                  Register/over-ride default NVRAM access functions for saving/retrieving link keys.
 *                  Must be called preior to calling wiced_bt_stack_init
 *
 * @param[in]       p_nvram_access  : NVRAM access functions
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_nvram_access_register(wiced_bt_nvram_access_t *p_nvram_access);

#ifdef __cplusplus
}
#endif
