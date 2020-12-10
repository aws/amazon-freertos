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

#include <stdio.h>
#include <string.h>

#include "tcpip_adapter.h"
#include "esp_event.h"
#include "esp_log.h"

const char ethbroadcast[] = {0xff,0xff,0xff,0xff,0xff,0xff};

static struct netif *esp_netif[TCPIP_ADAPTER_IF_MAX];
static tcpip_adapter_ip_info_t esp_ip_old[TCPIP_ADAPTER_IF_MAX];

static tcpip_adapter_dhcp_status_t dhcps_status = TCPIP_ADAPTER_DHCP_INIT;
static tcpip_adapter_dhcp_status_t dhcpc_status[TCPIP_ADAPTER_IF_MAX] = {TCPIP_ADAPTER_DHCP_INIT};
static bool tcpip_inited = false;

extern esp_err_t wlanif_input(void *netif, void *buffer, uint16_t len, void *eb);
extern void vNetworkNotifyIFUp();
extern void vNetworkNotifyIFDown();

#define TAG "tcpip_adapter"

void tcpip_adapter_init(void)
{
    if (tcpip_inited == false) {
        tcpip_inited = true;
    }
}

esp_err_t tcpip_adapter_start(tcpip_adapter_if_t tcpip_if, uint8_t *mac, tcpip_adapter_ip_info_t *ip_info)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_sta_start(uint8_t *mac, tcpip_adapter_ip_info_t *ip_info)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_ap_start(uint8_t *mac, tcpip_adapter_ip_info_t *ip_info)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_stop(tcpip_adapter_if_t tcpip_if)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_up(tcpip_adapter_if_t tcpip_if)
{
    vNetworkNotifyIFUp();
    return ESP_OK;
}

esp_err_t tcpip_adapter_down(tcpip_adapter_if_t tcpip_if)
{
    vNetworkNotifyIFDown();
    return ESP_OK;
}

esp_err_t tcpip_adapter_set_old_ip_info_api(tcpip_adapter_api_msg_t *msg)
{
    memcpy(&esp_ip_old[msg->tcpip_if], msg->ip_info, sizeof(tcpip_adapter_ip_info_t));
    return ESP_OK;
}

esp_err_t tcpip_adapter_set_old_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info)
{
    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || ip_info == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_get_old_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info)
{
    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || ip_info == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    memcpy(ip_info, &esp_ip_old[tcpip_if], sizeof(tcpip_adapter_ip_info_t));
    return ESP_OK;
}

esp_err_t tcpip_adapter_get_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcps_option(tcpip_adapter_option_mode_t opt_op, tcpip_adapter_option_id_t opt_id, void *opt_val, uint32_t opt_len)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_set_dns_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dns_type_t type, tcpip_adapter_dns_info_t *dns)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_get_dns_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dns_type_t type, tcpip_adapter_dns_info_t *dns)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcps_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status)
{
    *status = dhcps_status;

    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcpc_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status)
{
    *status = dhcpc_status[tcpip_if];

    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcpc_start(tcpip_adapter_if_t tcpip_if)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcpc_stop(tcpip_adapter_if_t tcpip_if)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_sta_input(void *buffer, uint16_t len, void *eb)
{
    wlanif_input(esp_netif[TCPIP_ADAPTER_IF_STA], buffer, len, eb);
    return ESP_OK;
}

esp_err_t tcpip_adapter_ap_input(void *buffer, uint16_t len, void *eb)
{
    wlanif_input(esp_netif[TCPIP_ADAPTER_IF_AP], buffer, len, eb);
    return ESP_OK;
}

esp_interface_t tcpip_adapter_get_esp_if(void *dev)
{
    struct netif *p_netif = (struct netif *)dev;

    if (p_netif == esp_netif[TCPIP_ADAPTER_IF_STA]) {
        return ESP_IF_WIFI_STA;
    } else if (p_netif == esp_netif[TCPIP_ADAPTER_IF_AP]) {
        return ESP_IF_WIFI_AP;
    } else if (p_netif == esp_netif[TCPIP_ADAPTER_IF_ETH]) {
        return ESP_IF_ETH;
    }

    return ESP_IF_MAX;
}

esp_err_t tcpip_adapter_get_sta_list(wifi_sta_list_t *wifi_sta_list, tcpip_adapter_sta_list_t *tcpip_sta_list)
{
    int i;

    if ((wifi_sta_list == NULL) || (tcpip_sta_list == NULL)) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    memset(tcpip_sta_list, 0, sizeof(tcpip_adapter_sta_list_t));
    tcpip_sta_list->num = wifi_sta_list->num;
    for (i = 0; i < wifi_sta_list->num; i++) {
        memcpy(tcpip_sta_list->sta[i].mac, wifi_sta_list->sta[i].mac, 6);
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_set_hostname(tcpip_adapter_if_t tcpip_if, const char *hostname)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_get_hostname(tcpip_adapter_if_t tcpip_if, const char **hostname)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_get_netif(tcpip_adapter_if_t tcpip_if, void **netif)
{
    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    *netif = esp_netif[tcpip_if];

    if (*netif == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
    }
    return ESP_OK;
}
