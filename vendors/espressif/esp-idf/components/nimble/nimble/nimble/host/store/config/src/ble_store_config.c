/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <inttypes.h>
#include <string.h>

#include "sysinit/sysinit.h"
#include "syscfg/syscfg.h"
#include "host/ble_hs.h"
#include "store/config/ble_store_config.h"
#include "ble_store_config_priv.h"
#include "nvs.h"

#define NVS_STRING_NAME_MAX_LEN           16
#define NVS_PEER_SEC_KEY                  "peer_sec"
#define NVS_OUR_SEC_KEY                   "our_sec"
#define NVS_CCCD_SEC_KEY                  "cccd_sec"
#define NVS_NAMESPACE                     "NimBLE_BOND"

/*****************************************************************************
 * $sec                                                                      *
 *****************************************************************************/

static void
ble_store_config_print_value_sec(const struct ble_store_value_sec *sec)
{
    if (sec->ltk_present) {
        BLE_HS_LOG(DEBUG, "ediv=%u rand=%llu authenticated=%d ltk=",
                   sec->ediv, sec->rand_num, sec->authenticated);
        ble_hs_log_flat_buf(sec->ltk, 16);
        BLE_HS_LOG(DEBUG, " ");
    }
    if (sec->irk_present) {
        BLE_HS_LOG(DEBUG, "irk=");
        ble_hs_log_flat_buf(sec->irk, 16);
        BLE_HS_LOG(DEBUG, " ");
    }
    if (sec->csrk_present) {
        BLE_HS_LOG(DEBUG, "csrk=");
        ble_hs_log_flat_buf(sec->csrk, 16);
        BLE_HS_LOG(DEBUG, " ");
    }

    BLE_HS_LOG(DEBUG, "\n");
}

static void
ble_store_config_print_key_sec(const struct ble_store_key_sec *key_sec)
{
    if (ble_addr_cmp(&key_sec->peer_addr, BLE_ADDR_ANY)) {
        BLE_HS_LOG(DEBUG, "peer_addr_type=%d peer_addr=",
                   key_sec->peer_addr.type);
        ble_hs_log_flat_buf(key_sec->peer_addr.val, 6);
        BLE_HS_LOG(DEBUG, " ");
    }
    if (key_sec->ediv_rand_present) {
        BLE_HS_LOG(DEBUG, "ediv=0x%02x rand=0x%llx ",
                   key_sec->ediv, key_sec->rand_num);
    }
}

/*****************************************************************************
 * $ MISC                                                                    *
 *****************************************************************************/

static void
get_nvs_key_string(char *key_string, int obj_type, int index)
{
    if (obj_type == BLE_STORE_OBJ_TYPE_PEER_SEC) {
        sprintf(key_string, "%s_%d", NVS_PEER_SEC_KEY, index);
    } else if (obj_type == BLE_STORE_OBJ_TYPE_OUR_SEC) {
        sprintf(key_string, "%s_%d", NVS_OUR_SEC_KEY, index);
    } else {
        sprintf(key_string, "%s_%d", NVS_CCCD_SEC_KEY, index);
    }

}

/*****************************************************************************
 * $ NVS storage                                                             *
 *****************************************************************************/

/**
 * Searches the database for an object matching the specified criteria.
 *
 * @return                      0 if a key was found; else BLE_HS_ENOENT.
 */

static int
ble_nvs_find_cccd_index(const struct ble_store_key_cccd *key_cccd, struct ble_store_value_cccd *value, nvs_handle nimble_handle)
{
    struct ble_store_value_cccd cccd = {0};
    size_t required_size;
    esp_err_t err;
    int skipped;
    int8_t i;
    char key_string[NVS_STRING_NAME_MAX_LEN];
    skipped = 0;

    for (i = 1; i <= MYNEWT_VAL(BLE_STORE_MAX_CCCDS); i++) {
        get_nvs_key_string(key_string, BLE_STORE_OBJ_TYPE_CCCD, i);

        err = nvs_get_blob(nimble_handle, key_string, NULL, &required_size);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
            BLE_HS_LOG(DEBUG, "NVS read operation failed for CCCD while fetching size !!\n");
            return -1;
            /* Check if entry is empty*/
        } else if (err == ESP_ERR_NVS_NOT_FOUND) {
            continue;
        }

        err = nvs_get_blob(nimble_handle, key_string, &cccd, &required_size);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
            BLE_HS_LOG(DEBUG, "NVS read operation failed while getting CCCD data!!\n");
            return -1;
        }

        if (ble_addr_cmp(&key_cccd->peer_addr, BLE_ADDR_ANY)) {
            if (ble_addr_cmp(&cccd.peer_addr, &key_cccd->peer_addr)) {
                continue;
            }
        }

        if (key_cccd->chr_val_handle != 0) {
            if (cccd.chr_val_handle != key_cccd->chr_val_handle) {
                continue;
            }
        }

        if (key_cccd->idx > skipped) {
            skipped++;
            continue;
        }

        /* Populate the value if present */
        if (value != NULL) {
            memcpy(value, &cccd, required_size);
        }

        return i;
    }

    return -1;
}

static int
ble_nvs_find_sec_index(int obj_type, const struct ble_store_key_sec *key_sec,
                       struct ble_store_value_sec *value, nvs_handle nimble_handle)
{
    size_t required_size;
    esp_err_t err;
    struct ble_store_value_sec cur = {0};
    int8_t skipped;
    int8_t i;
    char key_string[NVS_STRING_NAME_MAX_LEN];
    skipped = 0;

    for (i = 1; i <= MYNEWT_VAL(BLE_STORE_MAX_BONDS); i++) {
        get_nvs_key_string(key_string, obj_type, i);

        err = nvs_get_blob(nimble_handle, key_string, NULL, &required_size);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
            BLE_HS_LOG(DEBUG, "NVS read operation failed while fetching size !!\n");
            return -1;
            /* Check if the entry is empty, if so skip */
        } else if (err == ESP_ERR_NVS_NOT_FOUND) {
            continue;
        }

        err = nvs_get_blob(nimble_handle, key_string, &cur, &required_size);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
            BLE_HS_LOG(DEBUG, "NVS read operation failed while fetching data!!\n");
            return -1;
        }

        if (ble_addr_cmp(&key_sec->peer_addr, BLE_ADDR_ANY)) {
            if (ble_addr_cmp(&cur.peer_addr, &key_sec->peer_addr)) {
                continue;
            }
        }

        if (key_sec->ediv_rand_present) {
            if (cur.ediv != key_sec->ediv) {
                continue;
            }
            if (cur.rand_num != key_sec->rand_num) {
                continue;
            }
        }

        if (key_sec->idx > skipped) {
            skipped++;
            continue;
        }

        /* Populate the value if present */
        if (value != NULL) {
            memcpy(value, &cur, required_size);
        }

        return i;
    }
    return -1;
}

static int
ble_nvs_find_empty_index(int obj_type)
{
    size_t required_size;
    esp_err_t err;
    int8_t i;
    char key_string[NVS_STRING_NAME_MAX_LEN];
    nvs_handle nimble_handle;
    int max_limit = 0;

    if (obj_type != BLE_STORE_OBJ_TYPE_CCCD) {
        max_limit = MYNEWT_VAL(BLE_STORE_MAX_BONDS);
    } else {
        max_limit = MYNEWT_VAL(BLE_STORE_MAX_CCCDS);
    }

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nimble_handle);
    if (err != ESP_OK) {
        return -1;
    }

    for (i = 1; i <= max_limit; i++) {
        get_nvs_key_string(key_string, obj_type, i);

        err = nvs_get_blob(nimble_handle, key_string, NULL, &required_size);
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            nvs_close(nimble_handle);
            return i;
        } else if (err != ESP_OK) {
            nvs_close(nimble_handle);
            return -1;
        }
    }

    nvs_close(nimble_handle);
    return i;
}

static int
ble_nvs_read_key_value_cccd(const struct ble_store_key_cccd *key, struct ble_store_value_cccd *value)
{
    esp_err_t err;
    int8_t index;
    nvs_handle nimble_handle;

    if (key->idx < MYNEWT_VAL(BLE_STORE_MAX_CCCDS)) {
        err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nimble_handle);
        if (err != ESP_OK) {
            BLE_HS_LOG(DEBUG, "NVS open operation failed !!\n");
            return BLE_HS_ESTORE_FAIL;
        }

        index = ble_nvs_find_cccd_index(key, value, nimble_handle);
        if (index == -1) {
            nvs_close(nimble_handle);
            BLE_HS_LOG(DEBUG, "Key did not match with any entry in Database !! \n");
            return BLE_HS_ENOENT;
        }

        /* NVS Close */
        nvs_close(nimble_handle);
        return 0;

    } else {
        return BLE_HS_ENOENT;
    }
}

static int
ble_nvs_read_key_value_sec(int obj_type, const struct ble_store_key_sec *key, struct ble_store_value_sec *value)
{
    esp_err_t err;
    int8_t index;
    nvs_handle nimble_handle;

    if (key->idx < MYNEWT_VAL(BLE_STORE_MAX_BONDS)) {
        err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nimble_handle);
        if (err != ESP_OK) {
            BLE_HS_LOG(DEBUG, "NVS open operation failed !!\n");
            return BLE_HS_ESTORE_FAIL;
        }

        index = ble_nvs_find_sec_index(obj_type, key, value, nimble_handle);
        if (index == -1) {
            nvs_close(nimble_handle);
            BLE_HS_LOG(DEBUG, "Key did not match with any entry in Database !! \n");
            return BLE_HS_ENOENT;
        }

        /* NVS Close */
        nvs_close(nimble_handle);
        return 0;

    } else {
        return BLE_HS_ENOENT;
    }
}

static int
ble_nvs_write_key_value(char *key, const void *value_generic, size_t required_size)
{
    nvs_handle nimble_handle;
    esp_err_t err;
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nimble_handle);
    if (err != ESP_OK) {
        BLE_HS_LOG(DEBUG, "NVS open operation failed !!\n");
        return BLE_HS_ESTORE_FAIL;
    }

    err = nvs_set_blob(nimble_handle, key, value_generic, required_size);
    if (err != ESP_OK) {
        BLE_HS_LOG(DEBUG, "NVS write operation failed !!\n");
        nvs_close(nimble_handle);
        return BLE_HS_ESTORE_FAIL;
    }

    /* NVS commit and close */
    err = nvs_commit(nimble_handle);
    if (err != ESP_OK) {
        BLE_HS_LOG(DEBUG, "NVS commit operation failed !!\n");
        nvs_close(nimble_handle);
        return BLE_HS_ESTORE_FAIL;
    }

    nvs_close(nimble_handle);
    return 0;
}

static int
ble_nvs_delete_value(int obj_type, int8_t index, nvs_handle nimble_handle)
{
    esp_err_t err;
    char key_string[NVS_STRING_NAME_MAX_LEN];

    get_nvs_key_string(key_string, obj_type, index);

    /* Erase the key with given index */
    err = nvs_erase_key(nimble_handle, key_string);
    if (err != ESP_OK) {
        nvs_close(nimble_handle);
        return -1;
    }

    err = nvs_commit(nimble_handle);
    if (err != ESP_OK) {
        nvs_close(nimble_handle);
        return -1;
    }

    nvs_close(nimble_handle);
    return 0;
}

/**
 * Searches the database for an object matching the specified criteria.
 *
 * @return                      0 if a key was found; else BLE_HS_ENOENT.
 */

int
ble_store_nvs_read(int obj_type, const union ble_store_key *key,
                   union ble_store_value *value)
{
    switch (obj_type) {
    /* An encryption procedure (bonding) is being attempted.  The nimble
     * stack is asking us to look in our key database for a long-term key
     * corresponding to the specified ediv and random number.
     *
     * Perform a key lookup and populate the context object with the
     * result.  The nimble stack will use this key if this function returns
     * success.
     */
    case BLE_STORE_OBJ_TYPE_PEER_SEC:
    case BLE_STORE_OBJ_TYPE_OUR_SEC:
        ble_store_config_print_key_sec(&key->sec);
        return ble_nvs_read_key_value_sec(obj_type, &key->sec, &value->sec);

    case BLE_STORE_OBJ_TYPE_CCCD:
        return ble_nvs_read_key_value_cccd(&key->cccd, &value->cccd);

    default:
        return BLE_HS_ENOTSUP;
    }
}

/**
 * Adds the specified object to the database.
 *
 * @return                      0 on success;
 *                              BLE_HS_ESTORE_CAP if the database is full.
 */

int
ble_store_nvs_write(int obj_type, const union ble_store_value *val)
{
    char key_string[NVS_STRING_NAME_MAX_LEN];
    BLE_HS_LOG(DEBUG, "BLE_STORE_MAX_BONDS = %d \n", MYNEWT_VAL(BLE_STORE_MAX_BONDS));
    int8_t write_key_index = 0;

    write_key_index = ble_nvs_find_empty_index(obj_type);
    if (write_key_index == -1) {
        BLE_HS_LOG(DEBUG, "NVS operation failed !! \n");
        return BLE_HS_ESTORE_FAIL;
    } else if (write_key_index > MYNEWT_VAL(BLE_STORE_MAX_BONDS) && (obj_type != BLE_STORE_OBJ_TYPE_CCCD)) {
        if (obj_type == BLE_STORE_OBJ_TYPE_PEER_SEC) {
            BLE_HS_LOG(DEBUG, "Max Bonds limit exceeded for PEER_SEC !! \n");
        } else {
            BLE_HS_LOG(DEBUG, "Max Bonds limit exceeded for OUR_SEC !! \n");
        }
        return BLE_HS_ESTORE_CAP;
    } else if (write_key_index > MYNEWT_VAL(BLE_STORE_MAX_CCCDS)) {
        BLE_HS_LOG(DEBUG, "Max CCCDs storage limit exceeded for CCCD !! \n");
        return BLE_HS_ESTORE_CAP;
    }

    get_nvs_key_string(key_string, obj_type, write_key_index);

    if (obj_type != BLE_STORE_OBJ_TYPE_CCCD) {
        ble_store_config_print_value_sec(&val->sec);
        return ble_nvs_write_key_value(key_string, &val->sec, sizeof(struct
                                       ble_store_value_sec));
    } else {
        return ble_nvs_write_key_value(key_string, &val->cccd, sizeof(struct
                                       ble_store_value_cccd));
    }
}

int
ble_store_nvs_delete(int obj_type, const union ble_store_key *key)
{
    int rc;
    int8_t idx;
    esp_err_t err;
    nvs_handle nimble_handle;

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nimble_handle);
    if (err != ESP_OK) {
        return BLE_HS_ESTORE_FAIL;
    }

    if (obj_type != BLE_STORE_OBJ_TYPE_CCCD) {
        idx = ble_nvs_find_sec_index(obj_type, &key->sec, NULL, nimble_handle);
    } else {
        idx = ble_nvs_find_cccd_index(&key->cccd, NULL, nimble_handle);
    }

    if (idx == -1) {
        nvs_close(nimble_handle);
        BLE_HS_LOG(DEBUG, "Key did not match with any entry in Database !! \n");
        return BLE_HS_ENOENT;
    }

    rc = ble_nvs_delete_value(obj_type, idx, nimble_handle);
    if (rc == -1) {
        nvs_close(nimble_handle);
        return BLE_HS_ESTORE_FAIL;
    }

    return 0;
}

void
ble_store_config_init(void)
{
    /* Ensure this function only gets called by sysinit. */
    SYSINIT_ASSERT_ACTIVE();

    ble_hs_cfg.store_read_cb = ble_store_nvs_read;
    ble_hs_cfg.store_write_cb = ble_store_nvs_write;
    ble_hs_cfg.store_delete_cb = ble_store_nvs_delete;

    /* Re-initialize BSS values in case of unit tests. */

    ble_store_config_conf_init();
}
