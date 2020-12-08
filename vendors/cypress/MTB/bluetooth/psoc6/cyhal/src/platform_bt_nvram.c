/*****************************************************************************
**
**  Name:          bt_nvram_hal.c
**
**  Description:   NVRAM access functions for saving/retrieving link keys
**                 (see platform_bt_nvram_interace.h)
**
**  Copyright (c) 2019, Cypress semiconductor Corp, All Rights Reserved.
**  Cypress Proprietary and confidential.
******************************************************************************/
#include "bt_target.h"
#include "platform_bt_nvram.h"
#include "stdio.h"
#include "string.h"
#include "FreeRTOS.h"
#include "wiced_bt_dev.h"
#include "cyobjstore.h"

#define ENABLE_PLATFORM_NVRAM_LOG       0

#define PLATFORM_MAX_BONDED_DEVICES     5
#define MAX_BONIDNG_KEY_DATA_SIZE       sizeof(platform_bt_nvram_access_entry_t)+146 /* 146 is the max, size of Bluetooth key blob */

wiced_result_t platform_bt_nvram_get_bonded_devices(wiced_bt_device_address_t bonded_device_list[], uint16_t *p_num_devices);
wiced_result_t platform_bt_nvram_save_bonded_device_key(wiced_bt_device_address_t bd_addr, uint8_t *p_keyblobs, uint16_t key_len);
wiced_result_t plaform_bt_nvram_load_bonded_device_keys(wiced_bt_device_address_t bd_addr, uint8_t *p_key_entry, uint8_t entry_max_length);
wiced_result_t platform_bt_nvram_delete_bonded_device(wiced_bt_device_address_t bd_addr);
wiced_result_t platform_bt_nvram_load_local_identity_keys(wiced_bt_local_identity_keys_t *p_lkeys);
wiced_result_t platform_bt_nvram_save_local_identity_keys(wiced_bt_local_identity_keys_t *p_lkeys);
wiced_bool_t platform_bt_nvram_key_storage_available(wiced_bt_device_address_t bd_addr);

platform_bt_nvram_interface_t nvram_interface =
{
     .init = NULL, /* No initialization routine for KV storage interface */
     .get_bonded_devices = platform_bt_nvram_get_bonded_devices,
     .save_bonded_device_key = platform_bt_nvram_save_bonded_device_key,
     .load_bonded_device_keys = plaform_bt_nvram_load_bonded_device_keys,
     .delete_bonded_device = platform_bt_nvram_delete_bonded_device,
     .load_local_identity_keys = NULL,
     .save_local_identity_keys = NULL,
     .key_storage_available = NULL,
};

/*****************************************************************************
 * Utilities
 ****************************************************************************/

/**
 * Function     platform_bt_nvram_find_device
 *
 * Check if the KV store already has entry for this bd address
 *
 * @param[in]  bda
 *
 * @return  uint8_t
 */
static uint8_t platform_bt_nvram_find_device(const wiced_bt_device_address_t bda )
{
    uint8_t index = 0;
    uint8_t kv_data[MAX_BONIDNG_KEY_DATA_SIZE];
    uint32_t kv_entry_size;

    if (cy_objstore_is_initialized() == CY_OBJSTORE_NOT_INITIALIZED)
    {
        cy_objstore_initialize(false, 1);
    }

    for(index = 0; index < PLATFORM_MAX_BONDED_DEVICES; index++)
    {
        if (0 == cy_objstore_find_object(index+BT_BASE_KEY, (uint32_t *)kv_data, &kv_entry_size))
        {
#if ENABLE_PLATFORM_NVRAM_LOG
            printf("Found a valid KV store entry at index %d\r\n", index);
#endif
            break;
        }
    }

    return index+BT_BASE_KEY;
}

/**
 * Function     platform_bt_nvram_find_free_slot
 *
 * Check if any of the KV store slots for bonding data storage are free
 *
 * @param[in]  bda
 *
 * @return  uint8_t
 */
static uint8_t platform_bt_nvram_find_free_slot(void)
{
    uint8_t index = 0;

    if (cy_objstore_is_initialized() == CY_OBJSTORE_NOT_INITIALIZED)
    {
        cy_objstore_initialize(false, 1);
    }

    for(index = 0; index < PLATFORM_MAX_BONDED_DEVICES; index++)
    {
        if (cy_objstore_find_object(index+BT_BASE_KEY, NULL, NULL) == CY_OBJSTORE_NO_SUCH_OBJECT)
        {
            /* Found an empty slot */
            return index+BT_BASE_KEY;
        }
    }

    //TODO: handle error case
    return index+BT_BASE_KEY;
}

/**
 * Function     platform_bt_nvram_write
 *
 * Write BT bonding info to a specific KV store bond key index
 *
 * @param[in]  kv_index - KV store bonding device index
 * @param[in]  p_entry - data pointer
 * @param[in]  entry_len - length of data to be written
 *
 * @return  wiced_result_t
 */
static wiced_result_t platform_bt_nvram_write(uint8_t kv_index, uint8_t* p_entry, uint8_t entry_len)
{
    wiced_result_t result = WICED_ERROR;

#if ENABLE_PLATFORM_NVRAM_LOG
    uint8_t index;
    printf("KV store write length = %d and value is:\r\n", entry_len);
    for(index = 0; index < entry_len; index++)
    {
        printf("%2x ", p_entry[index]);
    }
#endif

    if (cy_objstore_is_initialized() == CY_OBJSTORE_NOT_INITIALIZED)
    {
        cy_objstore_initialize(false, 1);
    }

    if (0 == cy_objstore_store_object(kv_index, p_entry, entry_len))
    {
#if ENABLE_PLATFORM_NVRAM_LOG
        printf("KV store write at index %d SUCCESS!\r\n", kv_index);
#endif
        result = WICED_SUCCESS;
    }
    else
    {
#if ENABLE_PLATFORM_NVRAM_LOG
        printf("KV store write at index %d FAILED!\r\n", kv_index);
#endif
    }
    return result;
}

/**
 * Function     platform_bt_nvram_read
 *
 * Read BT bonding info from a specific KV store bond key index location
 *
 * @param[in]  kv_index - KV store bonding device index
 * @param[in]  p_key_entry - data pointer
 * @param[in]  entry_max_length - max. length of data to be read
  *
 * @return  wiced_result_t
 */
static wiced_result_t platform_bt_nvram_read(uint8_t kv_index, uint8_t* p_key_entry, uint8_t entry_max_length)
{
    wiced_result_t result = WICED_ERROR;

    if (cy_objstore_is_initialized() == CY_OBJSTORE_NOT_INITIALIZED)
    {
        cy_objstore_initialize(false, 1);
    }

    if(0 == cy_objstore_read_object(kv_index, p_key_entry, entry_max_length))
    {
#if ENABLE_PLATFORM_NVRAM_LOG
        printf("KV store read at index %d SUCCESS!\r\n", kv_index);
#endif
        result = WICED_SUCCESS;
    }
    else
    {
#if ENABLE_PLATFORM_NVRAM_LOG
        printf("KV store read failed\r\n");
#endif
    }

    return result;
}

/**
 * Function     platform_bt_nvram_delete
 *
 * Delete BT bonding info from a specific KV store bond key index location
 *
 * @param[in]  kv_index - KV store bonding device index to be deleted
  *
 * @return  wiced_result_t
 */
static wiced_result_t platform_bt_nvram_delete(uint8_t kv_index)
{
    wiced_result_t result = WICED_ERROR;

    if (cy_objstore_is_initialized() == CY_OBJSTORE_NOT_INITIALIZED)
    {
        cy_objstore_initialize(false, 1);
    }

    if (0 == cy_objstore_delete_object(kv_index))
    {
#if ENABLE_PLATFORM_NVRAM_LOG
        printf("KV store delete at index %d SUCCESS!\r\n", kv_index);
#endif
        result = WICED_SUCCESS;
    }
    return result;
}

/**
 * Function     platform_bt_nvram_get_bonded_devices
 *
 * Get list of bonded device addresses stored in NVRAM
 *
 * @param[in]  paired_device_list : array for getting bd address of bonded devices
 * @param[in/out] p_num_devices :  list size of paired_device_list/total number of bonded devices stored
 *
 * @return      WICED_SUCCESS or ERROR
 */
wiced_result_t platform_bt_nvram_get_bonded_devices(
    wiced_bt_device_address_t bonded_device_list[], uint16_t *p_num_devices)
{
    wiced_result_t result = WICED_ERROR;
    uint8_t index = 0;
    uint8_t kv_data[MAX_BONIDNG_KEY_DATA_SIZE];

    *p_num_devices = 0;

    if (cy_objstore_is_initialized() == CY_OBJSTORE_NOT_INITIALIZED)
    {
        cy_objstore_initialize(false, 1);
    }
    
    for(index = 0; index < PLATFORM_MAX_BONDED_DEVICES; index++)
    {
        if (0 == cy_objstore_find_object(index+BT_BASE_KEY, NULL, NULL))
        {
#if ENABLE_PLATFORM_NVRAM_LOG
            printf("Found a valid KV store entry at index %d\r\n", index);
#endif
            cy_objstore_read_object(index + BT_BASE_KEY, kv_data, sizeof(wiced_bt_device_address_t));
            /* BD address is the first 6 bytes of the key blob */
            memcpy(&bonded_device_list[*p_num_devices], kv_data, sizeof(wiced_bt_device_address_t));
            *p_num_devices = *p_num_devices + 1;
            result = WICED_SUCCESS;
        }
    }
    return result;
}

/**
 * Function     platform_bt_nvram_save_bonded_device_key
 *
 * Save link key information to KV Store
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[in]  p_keyblobs : key blobs including key header, link keys and key length
 * @param[in]  key_len :  total length of p_keyblobs
 *
 * @return      WICED_SUCCESS or ERROR
 */
wiced_result_t platform_bt_nvram_save_bonded_device_key(
    wiced_bt_device_address_t bd_addr, uint8_t *p_keyblobs, uint16_t key_len)
{
    uint8_t kv_index;
    wiced_result_t result = WICED_SUCCESS;
    uint8_t entry_len = 0;
    platform_bt_nvram_access_entry_t *p_entry = NULL;

#if ENABLE_PLATFORM_NVRAM_LOG
    uint8_t index;
    printf("BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT - platform_bt_nvram_save_bonded_device_key %d bytes for BD address - ", key_len);
    for(index = 0; index < 6; index++)
    {
        printf("%2x ", bd_addr[index]);
    }
    printf("\r\n Size of platform_bt_nvram_access_entry_t = %d\r\n", sizeof(platform_bt_nvram_access_entry_t));
#endif

    if( !key_len || !p_keyblobs)
        return WICED_ERROR;

    entry_len = (uint8_t)(sizeof(platform_bt_nvram_access_entry_t) -1 + key_len);
    p_entry = (platform_bt_nvram_access_entry_t *) pvPortMalloc( entry_len );
    if(!p_entry)
        return WICED_ERROR;

    if (cy_objstore_is_initialized() == CY_OBJSTORE_NOT_INITIALIZED)
    {
        cy_objstore_initialize(false, 1);
    }

    /* Check if an entry already exists or a slot is free, if yes, then store the bonding key in the KV store */
    if (PLATFORM_MAX_BONDED_DEVICES + BT_BASE_KEY != (kv_index = platform_bt_nvram_find_device(bd_addr)) ||
        PLATFORM_MAX_BONDED_DEVICES + BT_BASE_KEY != (kv_index = platform_bt_nvram_find_free_slot()))
    {
        /* Updating the storage structure */
        memcpy(&p_entry->bd_addr, bd_addr, sizeof(wiced_bt_device_address_t));
        p_entry->length  = key_len;
        memcpy(p_entry->key_blobs, p_keyblobs, key_len);
        result = platform_bt_nvram_write(kv_index, (uint8_t*)p_entry, entry_len);
    }
    else
    {
#if ENABLE_PLATFORM_NVRAM_LOG
        printf("save_key: Max number of devices to NVRAM reached. No space to save\r\n");
#endif
        result = WICED_ERROR;
    }

    vPortFree(p_entry);
    return result;
}

/**
 * Function     plaform_bt_nvram_load_bonded_device_keys
 *
 * Load link key information from KV Store
 *
 * @param[in]  bd_addr : bd_addr of bonded device
 * @param[out]  p_key_entry :  key information stored
 * @param[in]    entry_max_length : max length can be filled into p_key_entry
 *
 * @return      WICED_SUCCESS or ERROR
 */
wiced_result_t plaform_bt_nvram_load_bonded_device_keys(
    wiced_bt_device_address_t bd_addr, uint8_t *p_key_entry, uint8_t entry_max_length)
{
    uint16_t kv_index;
    wiced_result_t result = WICED_SUCCESS;
    platform_bt_nvram_access_entry_t *p_entry = NULL;
    uint8_t entry_len;

#if ENABLE_PLATFORM_NVRAM_LOG
    uint8_t index;
    printf("BTM_PAIRED_DEVICE_LINK_KEYS_REQUEST_EVT - plaform_bt_nvram_load_bonded_device_keys %d bytes for BD address - ", entry_max_length);
    for(index = 0; index < 6; index++)
    {
        printf("%2x ", bd_addr[index]);
    }
    printf("\r\n");
#endif

    if( p_key_entry == NULL || entry_max_length == 0)
        return WICED_ERROR;

    if (cy_objstore_is_initialized() == CY_OBJSTORE_NOT_INITIALIZED)
    {
        cy_objstore_initialize(false, 1);
    }

    if( PLATFORM_MAX_BONDED_DEVICES != (kv_index = platform_bt_nvram_find_device(bd_addr)) )
    {
        entry_len = (uint8_t)(sizeof(platform_bt_nvram_access_entry_t) -1 + entry_max_length);
        p_entry = (platform_bt_nvram_access_entry_t *) pvPortMalloc( entry_len );
        if(!p_entry)
        return WICED_ERROR;

        result = platform_bt_nvram_read(kv_index, (uint8_t*)p_entry, entry_len);
        if(result == WICED_SUCCESS)
        {
            memcpy(p_key_entry, p_entry->key_blobs, entry_max_length);
#if ENABLE_PLATFORM_NVRAM_LOG
            printf("load_key - entry_max_length = %d, key_len: %d key: \n", entry_max_length, p_entry->length);
            for(index = 0; index < p_entry->length; index++)
            {
                printf("%2x ", p_key_entry[index]);
            }
            printf("\r\n");
#endif
        }
        vPortFree(p_entry);
    }
    else
    {
#if ENABLE_PLATFORM_NVRAM_LOG
        printf("load_key: Device doesn't exist in bonded list\r\n");
#endif
        result = WICED_ERROR;
    }
    return result;
}

/**
 * Function     platform_bt_nvram_delete_bonded_device
 *
 * Delete link key information from KV store
 *
 * @param[in]  bd_addr : bd_addr of bonded device to be removed

 * @return      WICED_SUCCESS or ERROR
 */
wiced_result_t platform_bt_nvram_delete_bonded_device(wiced_bt_device_address_t bd_addr)
{
    uint8_t kv_index;
    wiced_result_t result = WICED_ERROR;

#if ENABLE_PLATFORM_NVRAM_LOG
    uint8_t index;
    printf("platform_bt_nvram_delete_bonded_device for BD address - ");
    for(index = 0; index < 6; index++)
    {
        printf("%2x ", bd_addr[index]);
    }
    printf("\r\n");
#endif

    kv_index = platform_bt_nvram_find_device(bd_addr);

    if(kv_index != PLATFORM_MAX_BONDED_DEVICES)
    {
        result = platform_bt_nvram_delete(kv_index);
    }
    return result;
}

#if 0
/**
 * Function     platform_bt_nvram_load_local_identity_keys
 *
 * Load local identity keys including ir/irk/dhk stored in NVRAM
 *
 * @param[out]  p_lkeys: local identity key information
 *
 * @return      WICED_SUCCESS or ERROR
 */
wiced_result_t platform_bt_nvram_load_local_identity_keys(wiced_bt_local_identity_keys_t *p_lkeys)
{
    wiced_result_t result = WICED_ERROR;

#if ENABLE_PLATFORM_NVRAM_LOG
    printf("BTM_LOCAL_IDENTITY_KEYS_REQUEST_EVT - platform_bt_nvram_load_local_identity_keys\r\n");
#endif

    if(p_lkeys != NULL)
    {
        int idkeySize;
        idkeySize = aceKeyValueDsHal_get(PLATFORM_IDENTITY_KEY, p_lkeys, sizeof(wiced_bt_local_identity_keys_t));
        if(idkeySize == sizeof(wiced_bt_local_identity_keys_t))
        {
            result = WICED_SUCCESS;
        }
#if ENABLE_PLATFORM_NVRAM_LOG
        printf("load local_keys: result: %d \n", idkeySize);
#endif
    }
    return result;
}

/**
 * Function     platform_bt_nvram_save_local_identity_keys
 *
 * Save local identity keys including ir/irk/dhk stored to NVRAM
 *
 * @param[in]  p_lkeys : local identity key information
 *
 * @return      WICED_SUCCESS or ERROR
 */
wiced_result_t platform_bt_nvram_save_local_identity_keys(wiced_bt_local_identity_keys_t *p_lkeys)
{
    wiced_result_t result = WICED_ERROR;

#if ENABLE_PLATFORM_NVRAM_LOG
    printf("BTM_LOCAL_IDENTITY_KEYS_UPDATE_EVT - platform_bt_nvram_save_local_identity_keys\r\n");
#endif

    if(p_lkeys != NULL)
    {
        int storeResult;
        storeResult = aceKeyValueDsHal_set(PLATFORM_IDENTITY_KEY, p_lkeys, sizeof(wiced_bt_local_identity_keys_t));

        if(storeResult == 0)
        {
            result = WICED_SUCCESS;
        }

#if ENABLE_PLATFORM_NVRAM_LOG
        printf("save local_keys: result: %d \n", storeResult);
#endif
    }
    return result;
}
#endif

/**
 * Function     display_blob_element
 *
 * Print the content of a specific element in the key data blob
 *
 * @param[in]  element_name. Element name string
 * @param[in]  data_ptr. Element data pointer
 * @param[in]  length. Element data length
 *
 * @return      None
 */
void display_blob_element(char* element_name, uint8_t* data_ptr, uint8_t* length)
{
    uint8_t loop_index;
    printf("%s: ", element_name);
    for(loop_index = 0; loop_index < *length; loop_index++)
    {
        printf("%02X ", data_ptr[loop_index]);
    }
    printf("\r\n");
}

/**
 * Function     print_bt_key_info
 *
 * Print all the valid bluetooth bonding device keys stored in the KV store
 *
 * @param[in]  None. Keeping the standard CLI parameters for compliance
 *
 * @return      TRUE if there is available space or FALSE
 */

int print_bt_key_info( int32_t argc, const char* argv[]  )
{
#if 0
    int status = 0;
    uint8_t index = 0;
    uint8_t bdIndex = 0;
    uint32_t dataLength = 0;
    char key[8];
    uint8_t kv_data[MAX_BONIDNG_KEY_DATA_SIZE];
    uint8_t deviceFound = 0;

    for(index = 0; index < PLATFORM_MAX_BONDED_DEVICES; index++)
    {
        cy_objstore_read_object(index + BT_BASE_KEY, kv_data, &dataLength);
        if (dataLength > 0)
        {
            if(dataLength == (uint8_t)(sizeof(platform_bt_nvram_access_entry_t) -1 + sizeof(wiced_bt_device_sec_keys_t)))
            {
                wiced_bt_device_sec_keys_t *keys_ptr;
                display_blob_element("Device BD address", &kv_data[0], sizeof(wiced_bt_device_address_t));
                display_blob_element("Key blob length", &kv_data[6], sizeof(uint16_t));

                keys_ptr = (wiced_bt_device_sec_keys_t *)&kv_data[8];
                display_blob_element("br_edr_key_type", &keys_ptr->br_edr_key_type, sizeof(keys_ptr->br_edr_key_type));
                display_blob_element("br_edr_key", &keys_ptr->br_edr_key, sizeof(keys_ptr->br_edr_key));
                display_blob_element("le_keys_available_mask", &keys_ptr->le_keys_available_mask, sizeof(keys_ptr->le_keys_available_mask));
                display_blob_element("ble_addr_type", &keys_ptr->ble_addr_type, sizeof(keys_ptr->ble_addr_type));
                display_blob_element("static_addr_type", &keys_ptr->static_addr_type, sizeof(keys_ptr->static_addr_type));
                display_blob_element("static_addr", &keys_ptr->static_addr, sizeof(keys_ptr->static_addr));
                display_blob_element("le_keys irk", &keys_ptr->le_keys.irk, sizeof(keys_ptr->le_keys.irk));
                display_blob_element("le_keys pltk", &keys_ptr->le_keys.pltk, sizeof(keys_ptr->le_keys.pltk));
                display_blob_element("le_keys pcsrk", &keys_ptr->le_keys.pcsrk, sizeof(keys_ptr->le_keys.pcsrk));
                display_blob_element("le_keys lltk", &keys_ptr->le_keys.lltk, sizeof(keys_ptr->le_keys.lltk));
                display_blob_element("le_keys lcsrk", &keys_ptr->le_keys.lcsrk, sizeof(keys_ptr->le_keys.lcsrk));
                display_blob_element("le_keys rand", &keys_ptr->le_keys.rand, sizeof(keys_ptr->le_keys.rand));
                display_blob_element("le_keys ediv", &keys_ptr->le_keys.ediv, sizeof(keys_ptr->le_keys.ediv));
                display_blob_element("le_keys div", &keys_ptr->le_keys.div, sizeof(keys_ptr->le_keys.div));
                display_blob_element("le_keys sec_level", &keys_ptr->le_keys.sec_level, sizeof(keys_ptr->le_keys.sec_level));
                display_blob_element("le_keys key_size", &keys_ptr->le_keys.key_size, sizeof(keys_ptr->le_keys.key_size));
                display_blob_element("le_keys srk_sec_level", &keys_ptr->le_keys.srk_sec_level, sizeof(keys_ptr->le_keys.srk_sec_level));
                display_blob_element("le_keys local_csrk_sec_level", &keys_ptr->le_keys.local_csrk_sec_level, sizeof(keys_ptr->le_keys.local_csrk_sec_level));
                display_blob_element("le_keys counter", &keys_ptr->le_keys.counter, sizeof(keys_ptr->le_keys.counter));
                display_blob_element("le_keys local_counter", &keys_ptr->le_keys.local_counter, sizeof(keys_ptr->le_keys.local_counter));
                printf("\r\n\nDevice raw key blob dump - ");
            }
            else
            {
                printf("Device key blob corupted, blob entries are - ");
            }

            for(bdIndex = 0; bdIndex <dataLength ; bdIndex++)
            {
                printf ("%02X ", kv_data[bdIndex]);
            }
            printf("\r\n\n");
            deviceFound = 1;
        }
    }

    if(deviceFound == 0)
    {
        printf("Bluetooth bonding key data not found\r\n");
    }
    return status;
#endif

    return 0;
}

