// Copyright 2010-2018 Espressif Systems (Shanghai) PTE LTD
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

/*
 * After station connects to AP and gets IP address by smartconfig,
 * it will use UDP to send 'ACK' to cellphone.
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "FreeRTOS_Sockets.h"
#include "tcpip_adapter.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_smartconfig.h"
#include "smartconfig_ack.h"

static const char *TAG = "smartconfig";

/* Flag to indicate sending smartconfig ACK or not. */
static bool s_sc_ack_send = false;

static void sc_ack_send_task(void *pvParameters)
{
    sc_ack_t *ack = (sc_ack_t *)pvParameters;
    uint8_t remote_ip[4];
    int remote_port = (ack->type == SC_ACK_TYPE_ESPTOUCH) ? SC_ACK_TOUCH_SERVER_PORT : SC_ACK_AIRKISS_SERVER_PORT;
    struct freertos_sockaddr server_addr;
    Socket_t xSocket = FREERTOS_INVALID_SOCKET;
    socklen_t sin_size = sizeof(server_addr);
    int sendlen;
    int ack_len = (ack->type == SC_ACK_TYPE_ESPTOUCH) ? SC_ACK_TOUCH_LEN : SC_ACK_AIRKISS_LEN;
    uint8_t packet_count = 1;

    bzero(&server_addr, sizeof(struct freertos_sockaddr));
    memcpy(remote_ip, ack->ctx.ip, sizeof(remote_ip));
    server_addr.sin_family = FREERTOS_AF_INET;
    server_addr.sin_addr = FreeRTOS_inet_addr_quick(remote_ip[0], remote_ip[1], remote_ip[2], remote_ip[3]);
    server_addr.sin_port = FreeRTOS_htons(remote_port);

    esp_wifi_get_mac(WIFI_IF_STA, ack->ctx.mac);

    vTaskDelay(200 / portTICK_RATE_MS);

    while (s_sc_ack_send) {
        uint32_t ulIPAddress;
        /* Get local IP address of station */
        FreeRTOS_GetAddressConfiguration(&ulIPAddress, NULL, NULL, NULL);
        if (ulIPAddress != 0UL) {
            /* If ESP touch, smartconfig ACK contains local IP address. */
            if (ack->type == SC_ACK_TYPE_ESPTOUCH) {
                memcpy(ack->ctx.ip, &ulIPAddress, 4);
            }

            /* Create UDP socket. */
            xSocket = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP);
            if (xSocket == FREERTOS_INVALID_SOCKET) {
                ESP_LOGE(TAG, "Creat udp socket failed");
                goto _end;
            }

            while (s_sc_ack_send) {
                /* Send smartconfig ACK every 100ms. */
                vTaskDelay(100 / portTICK_RATE_MS);

                sendlen = FreeRTOS_sendto(xSocket, &ack->ctx, ack_len, 0, &server_addr, sin_size);
                if (sendlen > 0) {
                    /* Totally send 30 smartconfig ACKs. Then smartconfig is successful. */
                    if (packet_count++ >= SC_ACK_MAX_COUNT) {
                        if (ack->link_flag) {
                            *ack->link_flag = 1;
                        }
                        if (ack->cb) {
                            ack->cb(SC_STATUS_LINK_OVER, remote_ip);
                        }
                        goto _end;
                    }
                }
                else {
                    /* FreeRTOS socket interface does not implement errno */
                    ESP_LOGE(TAG, "send failed");
                    goto _end;
                }
            }
        }
        else {
            vTaskDelay((portTickType)(100 / portTICK_RATE_MS));
        }
    }

_end:
    if (xSocket != FREERTOS_INVALID_SOCKET) {
        FreeRTOS_closesocket(xSocket);
    }
    free(ack);
    vTaskDelete(NULL);
}

void sc_ack_send(sc_ack_t *param)
{
    sc_ack_t *ack = NULL;

    if (param == NULL) {
        ESP_LOGE(TAG, "Smart config ack parameter error");
        return;
    }

    ack = malloc(sizeof(sc_ack_t));
    if (ack == NULL) {
        ESP_LOGE(TAG, "Smart config ack parameter malloc fail");
        return;
    }
    memcpy(ack, param, sizeof(sc_ack_t));

    s_sc_ack_send = true;

    if (xTaskCreate(sc_ack_send_task, "sc_ack_send_task", SC_ACK_TASK_STACK_SIZE, ack, SC_ACK_TASK_PRIORITY, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Create sending smartconfig ACK task fail");
        free(ack);
    }
}

void sc_ack_send_stop(void)
{
    s_sc_ack_send = false;
}
