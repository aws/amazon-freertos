// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_err.h"
#include "esp_partition.h"
#include "esp_spi_flash.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"
#include "esp_flash_encrypt.h"
#include "sdkconfig.h"

#include "esp_ota_ops.h"
#include "aws_esp_ota_ops.h"
#include "sys/queue.h"
#include "esp32/rom/crc.h"
#include "soc/dport_reg.h"
#include "esp_log.h"
#include "esp_flash_partitions.h"
#include "esp_efuse.h"
#include "bootloader_common.h"

typedef esp_ota_select_entry_t ota_select;

const static char *TAG = "esp_ota_ops";

static bool ota_select_valid(const ota_select *s)
{
    return bootloader_common_ota_select_valid(s);
}

static const esp_partition_t *_esp_get_otadata_partition(uint32_t *offset, ota_select *entry, bool active_part)
{
    esp_err_t ret;
    const esp_partition_t *find_partition = NULL;
    spi_flash_mmap_memory_t ota_data_map;
    const void *result = NULL;
    ota_select s_ota_select[2];

    find_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
    if (find_partition != NULL) {
        ret = esp_partition_mmap(find_partition, 0, find_partition->size, SPI_FLASH_MMAP_DATA, &result, &ota_data_map);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "mmap failed %d", ret);
            return NULL;
        } else {
            memcpy(&s_ota_select[0], result, sizeof(ota_select));
            memcpy(&s_ota_select[1], result + SPI_FLASH_SEC_SIZE, sizeof(ota_select));
            spi_flash_munmap(ota_data_map);
        }
        uint32_t gen_0_seq = ota_select_valid(&s_ota_select[0]) ? s_ota_select[0].ota_seq : 0;
        uint32_t gen_1_seq = ota_select_valid(&s_ota_select[1]) ? s_ota_select[1].ota_seq : 0;
        if (gen_0_seq == 0 && gen_1_seq == 0) {
            ESP_LOGW(TAG, "otadata partition is invalid, factory/ota_0 is boot partition");
            memcpy(entry, &s_ota_select[0], sizeof(ota_select));
            *offset = 0;
        } else if ((gen_0_seq >= gen_1_seq && active_part) || (gen_1_seq > gen_0_seq && !active_part)) {
            memcpy(entry, &s_ota_select[0], sizeof(ota_select));
            *offset = 0;
            ESP_LOGI(TAG, "[0] aflags/seq:0x%x/0x%x, pflags/seq:0x%x/0x%x",
                            s_ota_select[0].ota_state, gen_0_seq, s_ota_select[1].ota_state, gen_1_seq);
        } else {
            memcpy(entry, &s_ota_select[1], sizeof(ota_select));
            *offset = SPI_FLASH_SEC_SIZE;
            ESP_LOGI(TAG, "[1] aflags/seq:0x%x/0x%x, pflags/seq:0x%x/0x%x",
                            s_ota_select[1].ota_state, gen_1_seq, s_ota_select[0].ota_state, gen_0_seq);
        }
    } else {
        ESP_LOGE(TAG, "no otadata partition found");
    }
    return find_partition;
}

#ifdef CONFIG_APP_ANTI_ROLLBACK
static esp_err_t esp_ota_set_anti_rollback(void) {
    const esp_app_desc_t *app_desc = esp_ota_get_app_description();
    return esp_efuse_update_secure_version(app_desc->secure_version);
}
#endif

esp_err_t aws_esp_ota_set_boot_flags(uint32_t flags, bool active_part)
{
    const esp_partition_t *part = NULL;
    uint32_t offset;
    ota_select entry;

    ESP_LOGI(TAG, "%s: %d %d", __func__, flags, active_part);
    part = _esp_get_otadata_partition(&offset, &entry, active_part);
    if (part == NULL) {
        return ESP_FAIL;
    }
    entry.ota_state = flags;
    esp_err_t ret = esp_partition_erase_range(part, offset, SPI_FLASH_SEC_SIZE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to erase partition %d %d", offset, ret);
        return ret;
    }
    ret = esp_partition_write(part, offset, &entry, sizeof(ota_select));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to write partition %d %d", offset, ret);
        return ret;
    }
#ifdef CONFIG_APP_ANTI_ROLLBACK
    if (flags == ESP_OTA_IMG_VALID) {
        return esp_ota_set_anti_rollback();
    }
#endif
    return ret;
}

esp_err_t aws_esp_ota_get_boot_flags(uint32_t *flags, bool active_part)
{
    const esp_partition_t *part = NULL;
    uint32_t offset;
    ota_select entry;

    ESP_LOGI(TAG, "%s: %d", __func__, active_part);
    *flags = ESP_OTA_IMG_INVALID;
    part = _esp_get_otadata_partition(&offset, &entry, active_part);
    if (part == NULL) {
        return ESP_FAIL;
    }
    *flags = entry.ota_state;
    return ESP_OK;
}
