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

#ifndef _AWS_ESP_OTA_OPS_H
#define _AWS_ESP_OTA_OPS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_spi_flash.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Set firmware image flags, `active_part` if true then sets current running firmware flags, else passive (non-executing) firmware flags */
esp_err_t aws_esp_ota_set_boot_flags(uint32_t flags, bool active_part);

/* Get firmware image flags, `active_part` if true then gets current running firmware flags, else passive (non-executing) firmware flags */
esp_err_t aws_esp_ota_get_boot_flags(uint32_t *flags, bool active_part);

#ifdef __cplusplus
}
#endif

#endif /* AWS_ESP_OTA_OPS_H */
