/*
 * Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
 *
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

#ifndef __ESP_NIMBLE_HCI_H__
#define __ESP_NIMBLE_HCI_H__

#include "nimble/ble_hci_trans.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_HCI_UART_H4_NONE        0x00
#define BLE_HCI_UART_H4_CMD         0x01
#define BLE_HCI_UART_H4_ACL         0x02
#define BLE_HCI_UART_H4_SCO         0x03
#define BLE_HCI_UART_H4_EVT         0x04

/**
 * @brief Initialise VHCI transport layer.
 *
 * This function allocates memory for transport buffers to be
 * exchanged between NimBLE host and ESP controller. It also
 * registers required host callbacks with the controller.
 *
 * @return
 *    - ESP_OK if the initialisation is successful.
 *    - Other error codes from controller APIs
 */

esp_err_t esp_nimble_hci_init(void);

/**
 * @brief Initialise ESP controller(link layer) and VHCI transport layer.
 *
 * This function initialises ESP controller in BLE only mode and allocates
 * memory for transport buffers to be exchanged between NimBLE host and
 * ESP controller.  It also registers required host callbacks with the
 * controller.
 *
 * @return
 *    - ESP_OK if the initialisation is successful.
 *    - Other error codes from NVS APIs or controller APIs
 */

esp_err_t esp_nimble_hci_and_controller_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_NIMBLE_HCI_H__ */
