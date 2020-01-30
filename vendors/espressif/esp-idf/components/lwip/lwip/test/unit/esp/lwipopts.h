/*
Copyright 2018 Espressif Systems (Shanghai) PTE LTD

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifndef _ESP_LWIP_HDR_LWIPOPTS_H
#define _ESP_LWIP_HDR_LWIPOPTS_H

/* esp-lwip branch src and tests compilable */
#define SNTP_SERVER_DNS                 0
#define LWIP_HAVE_LOOPIF                1
#define LWIP_NETIF_HOSTNAME             1

/* ESP specific cofiguration */
#define ESP_LWIP                                1
#define ESP_DHCP                                1 
#define ESP_DHCP_TIMER                          1 
#define ESP_DHCPS_TIMER                         0
#define ESP_TCP_KEEP_CONNECTION_WHEN_IP_CHANGES 1 
#define ESP_IP4_ATON                            1
#define ESP_AUTO_RECV                           1
#define ESP_STATS_DROP                          0
#define ESP_STATS_TCP                           0
#define ESP_IRAM_ATTR                   
#define ESP_RANDOM_TCP_PORT                     0 
#define ESP_GRATUITOUS_ARP                      1

#include "../lwipopts.h"

#endif /* _ESP_LWIP_HDR_LWIPOPTS_H */
