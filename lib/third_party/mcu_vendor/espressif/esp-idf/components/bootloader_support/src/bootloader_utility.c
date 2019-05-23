// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_log.h"

#include "rom/cache.h"
#include "rom/efuse.h"
#include "rom/ets_sys.h"
#include "rom/spi_flash.h"
#include "rom/crc.h"
#include "rom/rtc.h"
#include "rom/uart.h"
#include "rom/gpio.h"
#include "rom/secure_boot.h"

#include "soc/soc.h"
#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/efuse_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"

#include "sdkconfig.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"
#include "esp_flash_encrypt.h"
#include "esp_flash_partitions.h"
#include "bootloader_flash.h"
#include "bootloader_random.h"
#include "bootloader_config.h"
#include "bootloader_common.h"

static const char* TAG = "boot";

/* Reduce literal size for some generic string literals */
#define MAP_ERR_MSG "Image contains multiple %s segments. Only the last one will be mapped."

static void load_image(bootloader_state_t *bs, const esp_image_metadata_t* image_data);
static void unpack_load_app(const esp_image_metadata_t *data);
static void set_cache_and_start_app(uint32_t drom_addr,
    uint32_t drom_load_addr,
    uint32_t drom_size,
    uint32_t irom_addr,
    uint32_t irom_load_addr,
    uint32_t irom_size,
    uint32_t entry_addr);

bool bootloader_utility_load_partition_table(bootloader_state_t* bs)
{
    const esp_partition_info_t *partitions;
    const char *partition_usage;
    esp_err_t err;
    int num_partitions;

    partitions = bootloader_mmap(ESP_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_MAX_LEN);
    if (!partitions) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", ESP_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_MAX_LEN);
        return false;
    }
    ESP_LOGD(TAG, "mapped partition table 0x%x at 0x%x", ESP_PARTITION_TABLE_OFFSET, (intptr_t)partitions);

    err = esp_partition_table_verify(partitions, true, &num_partitions);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to verify partition table");
        return false;
    }

    ESP_LOGI(TAG, "Partition Table:");
    ESP_LOGI(TAG, "## Label            Usage          Type ST Offset   Length");

    for(int i = 0; i < num_partitions; i++) {
        const esp_partition_info_t *partition = &partitions[i];
        ESP_LOGD(TAG, "load partition table entry 0x%x", (intptr_t)partition);
        ESP_LOGD(TAG, "type=%x subtype=%x", partition->type, partition->subtype);
        partition_usage = "unknown";

        /* valid partition table */
        switch(partition->type) {
        case PART_TYPE_APP: /* app partition */
            switch(partition->subtype) {
            case PART_SUBTYPE_FACTORY: /* factory binary */
                bs->factory = partition->pos;
                partition_usage = "factory app";
                break;
            case PART_SUBTYPE_TEST: /* test binary */
                bs->test = partition->pos;
                partition_usage = "test app";
                break;
            default:
                /* OTA binary */
                if ((partition->subtype & ~PART_SUBTYPE_OTA_MASK) == PART_SUBTYPE_OTA_FLAG) {
                    bs->ota[partition->subtype & PART_SUBTYPE_OTA_MASK] = partition->pos;
                    ++bs->app_count;
                    partition_usage = "OTA app";
                }
                else {
                    partition_usage = "Unknown app";
                }
                break;
            }
            break; /* PART_TYPE_APP */
        case PART_TYPE_DATA: /* data partition */
            switch(partition->subtype) {
            case PART_SUBTYPE_DATA_OTA: /* ota data */
                bs->ota_info = partition->pos;
                partition_usage = "OTA data";
                break;
            case PART_SUBTYPE_DATA_RF:
                partition_usage = "RF data";
                break;
            case PART_SUBTYPE_DATA_WIFI:
                partition_usage = "WiFi data";
                break;
            case PART_SUBTYPE_DATA_NVS_KEYS:
                partition_usage = "NVS keys";
                break;
            default:
                partition_usage = "Unknown data";
                break;
            }
            break; /* PARTITION_USAGE_DATA */
        default: /* other partition type */
            break;
        }

        /* print partition type info */
        ESP_LOGI(TAG, "%2d %-16s %-16s %02x %02x %08x %08x", i, partition->label, partition_usage,
                 partition->type, partition->subtype,
                 partition->pos.offset, partition->pos.size);
    }

    bootloader_munmap(partitions);

    ESP_LOGI(TAG,"End of partition table");
    return true;
}

/* Given a partition index, return the partition position data from the bootloader_state_t structure */
static esp_partition_pos_t index_to_partition(const bootloader_state_t *bs, int index)
{
    if (index == FACTORY_INDEX) {
        return bs->factory;
    }

    if (index == TEST_APP_INDEX) {
        return bs->test;
    }

    if (index >= 0 && index < MAX_OTA_SLOTS && index < bs->app_count) {
        return bs->ota[index];
    }

    esp_partition_pos_t invalid = { 0 };
    return invalid;
}

static void log_invalid_app_partition(int index)
{
    const char *not_bootable = " is not bootable"; /* save a few string literal bytes */
    switch(index) {
    case FACTORY_INDEX:
        ESP_LOGE(TAG, "Factory app partition%s", not_bootable);
        break;
    case TEST_APP_INDEX:
        ESP_LOGE(TAG, "Factory test app partition%s", not_bootable);
        break;
    default:
        ESP_LOGE(TAG, "OTA app partition slot %d%s", index, not_bootable);
        break;
    }
}

int bootloader_utility_get_selected_boot_partition(bootloader_state_t *bs)
{
    esp_ota_select_entry_t *sa, *sb;
    const esp_ota_select_entry_t *ota_select_map;

    sa = &bs->se[0];
    sb = &bs->se[1];

    if (bs->ota_info.offset != 0) {
        // partition table has OTA data partition
        if (bs->ota_info.size < 2 * SPI_SEC_SIZE) {
            ESP_LOGE(TAG, "ota_info partition size %d is too small (minimum %d bytes)", bs->ota_info.size, sizeof(esp_ota_select_entry_t));
            return INVALID_INDEX; // can't proceed
        }

        ESP_LOGD(TAG, "OTA data offset 0x%x", bs->ota_info.offset);
        ota_select_map = bootloader_mmap(bs->ota_info.offset, bs->ota_info.size);
        if (!ota_select_map) {
            ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", bs->ota_info.offset, bs->ota_info.size);
            return INVALID_INDEX; // can't proceed
        }
        memcpy(sa, ota_select_map, sizeof(esp_ota_select_entry_t));
        memcpy(sb, (uint8_t *)ota_select_map + SPI_SEC_SIZE, sizeof(esp_ota_select_entry_t));
        bootloader_munmap(ota_select_map);

        ESP_LOGD(TAG, "OTA sequence values A 0x%08x B 0x%08x", sa->ota_seq, sb->ota_seq);
        if(sa->ota_seq == UINT32_MAX && sb->ota_seq == UINT32_MAX) {
            ESP_LOGD(TAG, "OTA sequence numbers both empty (all-0xFF)");
            if (bs->factory.offset != 0) {
                ESP_LOGI(TAG, "Defaulting to factory image");
                bs->active_sec = 0;
                return FACTORY_INDEX;
            } else {
                ESP_LOGI(TAG, "No factory image, trying OTA 0");
#ifdef CONFIG_BOOTLOADER_OTA_ROLLBACK
                /* Write valid sequence number, such that on successful OTA this partition can be wiped out */
                esp_err_t ret;
                ret = bootloader_flash_erase_sector(bs->ota_info.offset / FLASH_SECTOR_SIZE);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "failed to erase ota sector");
                }
                sa->ota_seq = 0x1;
                sa->ota_flags = ESP_OTA_IMG_VALID;
                sa->crc = bootloader_common_ota_select_crc(sa);
                ret = bootloader_flash_write(bs->ota_info.offset, sa, sizeof(esp_ota_select_entry_t),
                                        esp_flash_encryption_enabled());
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "flash write failed");
                }
#endif
                bs->active_sec = 0;
                return 0;
            }
        } else  {
            bool ota_valid = false;
            const char *ota_msg;
            int ota_seq; // Raw OTA sequence number. May be more than # of OTA slots
            if(bootloader_common_ota_select_valid(sa) && bootloader_common_ota_select_valid(sb)) {
                ota_valid = true;
                ota_msg = "Both OTA values";
                ota_seq = MAX(sa->ota_seq, sb->ota_seq) - 1;
                bs->active_sec = (sa->ota_seq > sb->ota_seq) ? 0: 1;
            } else if(bootloader_common_ota_select_valid(sa)) {
                ota_valid = true;
                ota_msg = "Only OTA sequence A is";
                ota_seq = sa->ota_seq - 1;
                bs->active_sec = 0;
            } else if(bootloader_common_ota_select_valid(sb)) {
                ota_valid = true;
                ota_msg = "Only OTA sequence B is";
                ota_seq = sb->ota_seq - 1;
                bs->active_sec = 1;
            }

            if (ota_valid) {
                int ota_slot = ota_seq % bs->app_count; // Actual OTA partition selection
                ESP_LOGD(TAG, "%s valid. Mapping seq %d -> OTA slot %d", ota_msg, ota_seq, ota_slot);
                return ota_slot;
            } else if (bs->factory.offset != 0) {
                ESP_LOGE(TAG, "ota data partition invalid, falling back to factory");
                return FACTORY_INDEX;
            } else {
                ESP_LOGE(TAG, "ota data partition invalid and no factory, will try all partitions");
                return FACTORY_INDEX;
            }
        }
    }

    // otherwise, start from factory app partition and let the search logic
    // proceed from there
    return FACTORY_INDEX;
}

/* Return true if a partition has a valid app image that was successfully loaded */
static bool try_load_partition(const esp_partition_pos_t *partition, esp_image_metadata_t *data)
{
    if (partition->size == 0) {
        ESP_LOGD(TAG, "Can't boot from zero-length partition");
        return false;
    }
#ifdef BOOTLOADER_BUILD
    if (esp_image_load(ESP_IMAGE_LOAD, partition, data) == ESP_OK) {
        ESP_LOGI(TAG, "Loaded app from partition at offset 0x%x",
                 partition->offset);
        return true;
    }
#endif

    return false;
}

#define TRY_LOG_FORMAT "Trying partition index %d offs 0x%x size 0x%x"

void bootloader_utility_load_boot_image(bootloader_state_t *bs, int start_index)
{
    int index = start_index;
    esp_partition_pos_t part;
    esp_image_metadata_t image_data;

    if(start_index == TEST_APP_INDEX) {
        if (try_load_partition(&bs->test, &image_data)) {
            load_image(bs, &image_data);
        } else {
            ESP_LOGE(TAG, "No bootable test partition in the partition table");
            return;
        }
    }

    /* work backwards from start_index, down to the factory app */
    for(index = start_index; index >= FACTORY_INDEX; index--) {
        part = index_to_partition(bs, index);
        if (part.size == 0) {
            continue;
        }
        ESP_LOGD(TAG, TRY_LOG_FORMAT, index, part.offset, part.size);
        if (try_load_partition(&part, &image_data)) {
            load_image(bs, &image_data);
        }
        log_invalid_app_partition(index);
    }

    /* failing that work forwards from start_index, try valid OTA slots */
    for(index = start_index + 1; index < bs->app_count; index++) {
        part = index_to_partition(bs, index);
        if (part.size == 0) {
            continue;
        }
        ESP_LOGD(TAG, TRY_LOG_FORMAT, index, part.offset, part.size);
        if (try_load_partition(&part, &image_data)) {
            load_image(bs, &image_data);
        }
        log_invalid_app_partition(index);
    }

    if (try_load_partition(&bs->test, &image_data)) {
        ESP_LOGW(TAG, "Falling back to test app as only bootable partition");
        load_image(bs, &image_data);
    }

    ESP_LOGE(TAG, "No bootable app partitions in the partition table");
    bzero(&image_data, sizeof(esp_image_metadata_t));
}

#ifdef CONFIG_BOOTLOADER_WDT_ENABLE
static rtc_slow_freq_t bootloader_rtc_clk_slow_freq_get()
{
	return REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_ANA_CLK_RTC_SEL);
}

static uint32_t bootloader_rtc_clk_slow_freq_get_hz()
{
	switch (bootloader_rtc_clk_slow_freq_get()) {
		case RTC_SLOW_FREQ_RTC: return 150000;
		case RTC_SLOW_FREQ_32K_XTAL: return 32768;
		case RTC_SLOW_FREQ_8MD256: return (8500000 / 256);
	}
	return 0;
}

static void enable_rtc_wdt(uint32_t time_ms)
{
    uint32_t wdt_cycles = (uint32_t) ((uint64_t) bootloader_rtc_clk_slow_freq_get_hz() * time_ms / 1000);

    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);
    WRITE_PERI_REG(RTC_CNTL_WDTFEED_REG, 1);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_SYS_RESET_LENGTH, 7);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_CPU_RESET_LENGTH, 7);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_STG0, RTC_WDT_STG_SEL_RESET_RTC);
    WRITE_PERI_REG(RTC_CNTL_WDTCONFIG1_REG, wdt_cycles);
    SET_PERI_REG_MASK(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_EN | RTC_CNTL_WDT_PAUSE_IN_SLP);
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, 0);
}
#endif

#ifdef CONFIG_BOOTLOADER_OTA_ROLLBACK
static void esp_ota_rollback(bootloader_state_t *bs, const esp_image_metadata_t *image_data)
{
    esp_err_t ret;
    bool active_sec = bs->active_sec;
    bool passive_sec = !bs->active_sec;

    int a_ota_slot = (bs->se[active_sec].ota_seq - 1) % bs->app_count;
    int p_ota_slot = (bs->se[passive_sec].ota_seq - 1) % bs->app_count;

    /* Make sure that we are booting from correct partition */
    if (image_data->start_addr == bs->ota[a_ota_slot].offset) {
        /* Check if this is first OTA boot of firmware */
        if (bootloader_common_ota_select_valid(&bs->se[active_sec]) && (bs->se[active_sec].ota_flags == ESP_OTA_IMG_NEW)) {
            ESP_LOGI(TAG, "Set fw to pending validation");
#ifdef CONFIG_BOOTLOADER_WDT_ENABLE
            /* Enable RTC WDT as this is first boot of OTA image */
            const uint32_t time_ms = CONFIG_BOOTLOADER_WDT_TIME_MS;
            enable_rtc_wdt(time_ms);
#endif
            bs->se[active_sec].ota_flags = ESP_OTA_IMG_PENDING_VERIFY;
            ret = bootloader_flash_erase_sector((bs->ota_info.offset / FLASH_SECTOR_SIZE) + active_sec);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "failed to erase ota sector");
                return;
            }
            ret = bootloader_flash_write(bs->ota_info.offset + (active_sec * FLASH_SECTOR_SIZE), &(bs->se[active_sec]),
                                            sizeof(esp_ota_select_entry_t), esp_flash_encryption_enabled());
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "flash write failed");
            }
        } else if (bs->se[active_sec].ota_flags == ESP_OTA_IMG_VALID) {
#ifdef CONFIG_BOOTLOADER_OTA_NO_FORCE_ROLLBACK
            /* Firmware image is valid, check if, passive partition should be completely erased */
            if (bs->se[passive_sec].crc == bootloader_common_ota_select_crc(&bs->se[passive_sec])) {
                esp_partition_pos_t part;
                part = index_to_partition(bs, p_ota_slot);
                ESP_LOGW(TAG, "Deleting passive firmware @0x%x len:0x%x", part.offset, part.size);

                /* Delete contents of other partition and its entry from `otadata` */
                ret = bootloader_flash_erase_range(part.offset, part.size);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "flash partition erase failed");
                }
                ret = bootloader_flash_erase_sector((bs->ota_info.offset / FLASH_SECTOR_SIZE) + passive_sec);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "failed to erase ota sector");
                }
#ifdef CONFIG_FLASH_ENCRYPTION_ENABLED
                /*
                 * When Flash Encryption is enabled, the erased ota entry is written with 0xFF
                 */
                memset(&bs->se[passive_sec], 0xff, sizeof(esp_ota_select_entry_t));
                ret = bootloader_flash_write(bs->ota_info.offset + (passive_sec * FLASH_SECTOR_SIZE), &(bs->se[passive_sec]),
                                                sizeof(esp_ota_select_entry_t), esp_flash_encryption_enabled());
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "flash write failed");
                }
#endif
            }
#endif
        }

        ESP_LOGI(TAG, "ota rollback check done");
    }
}
#endif

// Copy loaded segments to RAM, set up caches for mapped segments, and start application.
static void load_image(bootloader_state_t *bs, const esp_image_metadata_t* image_data)
{
    /**
     * Rough steps for a first boot, when encryption and secure boot are both disabled:
     *   1) Generate secure boot key and write to EFUSE.
     *   2) Write plaintext digest based on plaintext bootloader
     *   3) Generate flash encryption key and write to EFUSE.
     *   4) Encrypt flash in-place including bootloader, then digest,
     *      then app partitions and other encrypted partitions
     *   5) Burn EFUSE to enable flash encryption (FLASH_CRYPT_CNT)
     *   6) Burn EFUSE to enable secure boot (ABS_DONE_0)
     *
     * If power failure happens during Step 1, probably the next boot will continue from Step 2.
     * There is some small chance that EFUSEs will be part-way through being written so will be
     * somehow corrupted here. Thankfully this window of time is very small, but if that's the
     * case, one has to use the espefuse tool to manually set the remaining bits and enable R/W
     * protection. Once the relevant EFUSE bits are set and R/W protected, Step 1 will be skipped
     * successfully on further reboots.
     *
     * If power failure happens during Step 2, Step 1 will be skipped and Step 2 repeated:
     * the digest will get re-written on the next boot.
     *
     * If power failure happens during Step 3, it's possible that EFUSE was partially written
     * with the generated flash encryption key, though the time window for that would again
     * be very small. On reboot, Step 1 will be skipped and Step 2 repeated, though, Step 3
     * may fail due to the above mentioned reason, in which case, one has to use the espefuse
     * tool to manually set the remaining bits and enable R/W protection. Once the relevant EFUSE
     * bits are set and R/W protected, Step 3 will be skipped successfully on further reboots.
     *
     * If power failure happens after start of 4 and before end of 5, the next boot will fail
     * (bootloader header is encrypted and flash encryption isn't enabled yet, so it looks like
     * noise to the ROM bootloader). The check in the ROM is pretty basic so if the first byte of
     * ciphertext happens to be the magic byte E9 then it may try to boot, but it will definitely
     * crash (no chance that the remaining ciphertext will look like a valid bootloader image).
     * Only solution is to reflash with all plaintext and the whole process starts again: skips
     * Step 1, repeats Step 2, skips Step 3, etc.
     *
     * If power failure happens after 5 but before 6, the device will reboot with flash
     * encryption on and will regenerate an encrypted digest in Step 2. This should still
     * be valid as the input data for the digest is read via flash cache (so will be decrypted)
     * and the code in secure_boot_generate() tells bootloader_flash_write() to encrypt the data
     * on write if flash encryption is enabled. Steps 3 - 5 are skipped (encryption already on),
     * then Step 6 enables secure boot.
     */

#if defined(CONFIG_SECURE_BOOT_ENABLED) || defined(CONFIG_FLASH_ENCRYPTION_ENABLED)
    esp_err_t err;
#endif

#ifdef CONFIG_SECURE_BOOT_ENABLED
    /* Steps 1 & 2 (see above for full description):
     *   1) Generate secure boot EFUSE key
     *   2) Compute digest of plaintext bootloader
     */
    err = esp_secure_boot_generate_digest();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Bootloader digest generation for secure boot failed (%d).", err);
        return;
    }
#endif

#ifdef CONFIG_FLASH_ENCRYPTION_ENABLED
    /* Steps 3, 4 & 5 (see above for full description):
     *   3) Generate flash encryption EFUSE key
     *   4) Encrypt flash contents
     *   5) Burn EFUSE to enable flash encryption
     */
    ESP_LOGI(TAG, "Checking flash encryption...");
    bool flash_encryption_enabled = esp_flash_encryption_enabled();
    err = esp_flash_encrypt_check_and_update();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Flash encryption check failed (%d).", err);
        return;
    }
#endif

#ifdef CONFIG_SECURE_BOOT_ENABLED
    /* Step 6 (see above for full description):
     *   6) Burn EFUSE to enable secure boot
     */
    ESP_LOGI(TAG, "Checking secure boot...");
    err = esp_secure_boot_permanently_enable();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "FAILED TO ENABLE SECURE BOOT (%d).", err);
        /* Allow booting to continue, as the failure is probably
           due to user-configured EFUSEs for testing...
        */
    }
#endif

#ifdef CONFIG_FLASH_ENCRYPTION_ENABLED
    if (!flash_encryption_enabled && esp_flash_encryption_enabled()) {
        /* Flash encryption was just enabled for the first time,
           so issue a system reset to ensure flash encryption
           cache resets properly */
        ESP_LOGI(TAG, "Resetting with flash encryption enabled...");
        REG_WRITE(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_SYS_RST);
        return;
    }
#endif

#ifdef CONFIG_BOOTLOADER_OTA_ROLLBACK
    if (bs->ota_info.offset != 0) {
        /* otadata partition exists, check for rollback */
        if (bs->app_count >= 2) {
            esp_ota_rollback(bs, image_data);
        }
    }
#endif

    ESP_LOGI(TAG, "Disabling RNG early entropy source...");
    bootloader_random_disable();

    // copy loaded segments to RAM, set up caches for mapped segments, and start application
    unpack_load_app(image_data);
}

static void unpack_load_app(const esp_image_metadata_t* data)
{
    uint32_t drom_addr = 0;
    uint32_t drom_load_addr = 0;
    uint32_t drom_size = 0;
    uint32_t irom_addr = 0;
    uint32_t irom_load_addr = 0;
    uint32_t irom_size = 0;

    // Find DROM & IROM addresses, to configure cache mappings
    for (int i = 0; i < data->image.segment_count; i++) {
        const esp_image_segment_header_t *header = &data->segments[i];
        if (header->load_addr >= SOC_DROM_LOW && header->load_addr < SOC_DROM_HIGH) {
            if (drom_addr != 0) {
                ESP_LOGE(TAG, MAP_ERR_MSG, "DROM");
            } else {
                ESP_LOGD(TAG, "Mapping segment %d as %s", i, "DROM");
            }
            drom_addr = data->segment_data[i];
            drom_load_addr = header->load_addr;
            drom_size = header->data_len;
        }
        if (header->load_addr >= SOC_IROM_LOW && header->load_addr < SOC_IROM_HIGH) {
            if (irom_addr != 0) {
                ESP_LOGE(TAG, MAP_ERR_MSG, "IROM");
            } else {
                ESP_LOGD(TAG, "Mapping segment %d as %s", i, "IROM");
            }
            irom_addr = data->segment_data[i];
            irom_load_addr = header->load_addr;
            irom_size = header->data_len;
        }
    }

    ESP_LOGD(TAG, "calling set_cache_and_start_app");
    set_cache_and_start_app(drom_addr,
        drom_load_addr,
        drom_size,
        irom_addr,
        irom_load_addr,
        irom_size,
        data->image.entry_addr);
}

static void set_cache_and_start_app(
    uint32_t drom_addr,
    uint32_t drom_load_addr,
    uint32_t drom_size,
    uint32_t irom_addr,
    uint32_t irom_load_addr,
    uint32_t irom_size,
    uint32_t entry_addr)
{
    int rc;
    ESP_LOGD(TAG, "configure drom and irom and start");
    Cache_Read_Disable( 0 );
    Cache_Flush( 0 );

    /* Clear the MMU entries that are already set up,
       so the new app only has the mappings it creates.
    */
    for (int i = 0; i < DPORT_FLASH_MMU_TABLE_SIZE; i++) {
        DPORT_PRO_FLASH_MMU_TABLE[i] = DPORT_FLASH_MMU_TABLE_INVALID_VAL;
    }

    uint32_t drom_load_addr_aligned = drom_load_addr & MMU_FLASH_MASK;
    uint32_t drom_page_count = bootloader_cache_pages_to_map(drom_size, drom_load_addr);
    ESP_LOGV(TAG, "d mmu set paddr=%08x vaddr=%08x size=%d n=%d",
            drom_addr & MMU_FLASH_MASK, drom_load_addr_aligned, drom_size, drom_page_count);
    rc = cache_flash_mmu_set(0, 0, drom_load_addr_aligned, drom_addr & MMU_FLASH_MASK, 64, drom_page_count);
    ESP_LOGV(TAG, "rc=%d", rc);
    rc = cache_flash_mmu_set(1, 0, drom_load_addr_aligned, drom_addr & MMU_FLASH_MASK, 64, drom_page_count);
    ESP_LOGV(TAG, "rc=%d", rc);

    uint32_t irom_load_addr_aligned = irom_load_addr & MMU_FLASH_MASK;
    uint32_t irom_page_count = bootloader_cache_pages_to_map(irom_size, irom_load_addr);
    ESP_LOGV(TAG, "i mmu set paddr=%08x vaddr=%08x size=%d n=%d",
            irom_addr & MMU_FLASH_MASK, irom_load_addr_aligned, irom_size, irom_page_count);
    rc = cache_flash_mmu_set(0, 0, irom_load_addr_aligned, irom_addr & MMU_FLASH_MASK, 64, irom_page_count);
    ESP_LOGV(TAG, "rc=%d", rc);
    rc = cache_flash_mmu_set(1, 0, irom_load_addr_aligned, irom_addr & MMU_FLASH_MASK, 64, irom_page_count);
    ESP_LOGV(TAG, "rc=%d", rc);

    DPORT_REG_CLR_BIT( DPORT_PRO_CACHE_CTRL1_REG,
            (DPORT_PRO_CACHE_MASK_IRAM0) | (DPORT_PRO_CACHE_MASK_IRAM1 & 0) |
            (DPORT_PRO_CACHE_MASK_IROM0 & 0) | DPORT_PRO_CACHE_MASK_DROM0 |
            DPORT_PRO_CACHE_MASK_DRAM1 );

    DPORT_REG_CLR_BIT( DPORT_APP_CACHE_CTRL1_REG,
            (DPORT_APP_CACHE_MASK_IRAM0) | (DPORT_APP_CACHE_MASK_IRAM1 & 0) |
            (DPORT_APP_CACHE_MASK_IROM0 & 0) | DPORT_APP_CACHE_MASK_DROM0 |
            DPORT_APP_CACHE_MASK_DRAM1 );

    Cache_Read_Enable( 0 );

    // Application will need to do Cache_Flush(1) and Cache_Read_Enable(1)

    ESP_LOGD(TAG, "start: 0x%08x", entry_addr);
    typedef void (*entry_t)(void) __attribute__((noreturn));
    entry_t entry = ((entry_t) entry_addr);

    // TODO: we have used quite a bit of stack at this point.
    // use "movsp" instruction to reset stack back to where ROM stack starts.
    (*entry)();
}
