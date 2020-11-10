/*
 * Copyright (c) 2019 Amazon.com, Inc. or its affiliates.  All rights reserved.
 *
 * PROPRIETARY/CONFIDENTIAL.  USE IS SUBJECT TO LICENSE TERMS.
 */

#include "wifi_nvdm_config.h"
#include "iot_wifi.h"

/** @brief This enum defines the display type of a data item. */
typedef enum {
    NVDM_DATA_ITEM_TYPE_RAW_DATA = 0x01,   /**< Defines the display type with raw data. */
    NVDM_DATA_ITEM_TYPE_STRING = 0x02,     /**< Defines the display type with string. */
} nvdm_data_item_type_t;

/** @brief This enum defines return type of NVDM APIs. */
typedef enum {
    NVDM_STATUS_INVALID_PARAMETER = -5,  /**< The user parameter is invalid. */
    NVDM_STATUS_ITEM_NOT_FOUND = -4,     /**< The data item wasn't found by the NVDM. */
    NVDM_STATUS_INSUFFICIENT_SPACE = -3, /**< No space is available in the flash. */
    NVDM_STATUS_INCORRECT_CHECKSUM = -2, /**< The NVDM found a checksum error when reading the data item. */
    NVDM_STATUS_ERROR = -1,              /**< An unknown error occurred. */
    NVDM_STATUS_OK = 0,                  /**< The operation was successful. */
} nvdm_status_t;

int32_t wifi_config_init(wifi_cfg_t *wifi_config)
{
    return eWiFiSuccess;
}

int32_t dhcp_config_init(void)
{
    return STA_IP_MODE_DHCP;
}

int32_t tcpip_config_init(lwip_tcpip_config_t *tcpip_config)
{
    if (tcpip_config) {
        memset( tcpip_config, 0, sizeof(*tcpip_config) );

        IP4_ADDR( &tcpip_config->sta_addr,    192, 168,   1,   1 );
        IP4_ADDR( &tcpip_config->sta_mask,    255, 255, 255,   0 );
        IP4_ADDR( &tcpip_config->sta_gateway, 192, 168,   1, 254 );

        IP4_ADDR( &tcpip_config->ap_addr,     192, 168,   2,   1 );
        IP4_ADDR( &tcpip_config->ap_mask,     255, 255, 255,   0 );
        IP4_ADDR( &tcpip_config->ap_gateway,  192, 168,   2,   1 );
    }
    return 0;
}

nvdm_status_t nvdm_write_data_item(const char *group_name,
        const char *data_item_name,
        nvdm_data_item_type_t type,
        const uint8_t *buffer,
        uint32_t size)
{
    return NVDM_STATUS_ERROR;
}

static uint32_t ip_number_to_big_endian(uint32_t ip_number)
{
    uint8_t *byte = (uint8_t *)&ip_number;
    return (uint32_t)((byte[0] << 24) | (byte[1] << 16) | (byte[2] << 8) | byte[3]);
}

static void ip_number_to_string(uint32_t ip_number, char ip_string[IP4ADDR_STRLEN_MAX])
{
    snprintf(ip_string,
                IP4ADDR_STRLEN_MAX,
                "%d.%d.%d.%d",
                (uint8_t)((ip_number >> 24) & 0xFF),
                (uint8_t)((ip_number >> 16) & 0xFF),
                (uint8_t)((ip_number >> 8) & 0xFF),
                (uint8_t)((ip_number >> 0) & 0xFF));
}

static void dhcpd_set_ip_pool(const ip4_addr_t *ap_ip_addr,
                              const ip4_addr_t *ap_net_mask,
                              char ip_pool_start[IP4ADDR_STRLEN_MAX],
                              char ip_pool_end[IP4ADDR_STRLEN_MAX])
{
    uint32_t ap_ip_number = ip_number_to_big_endian(ip4_addr_get_u32(ap_ip_addr));
    uint32_t ap_mask_number = ip_number_to_big_endian(ip4_addr_get_u32(ap_net_mask));
    uint32_t ip_range_min = ap_ip_number & ap_mask_number;
    uint32_t ip_range_max = ip_range_min | (~ap_mask_number);

    if ((ip_range_max - ap_ip_number) > (ap_ip_number - ip_range_min)) {
        ip_number_to_string(ap_ip_number + 1, ip_pool_start);
        ip_number_to_string(ip_range_max - 1, ip_pool_end);
    } else {
        ip_number_to_string(ip_range_min + 1, ip_pool_start);
        ip_number_to_string(ap_ip_number - 1, ip_pool_end);
    }
}

#define STRCPY strncpy

void dhcpd_settings_init(const lwip_tcpip_config_t *tcpip_config,
                                dhcpd_settings_t *dhcpd_settings)
{
    STRCPY(dhcpd_settings->dhcpd_server_address,
               ip4addr_ntoa(&tcpip_config->ap_addr),
               IP4ADDR_STRLEN_MAX);

    STRCPY(dhcpd_settings->dhcpd_netmask,
               ip4addr_ntoa(&tcpip_config->ap_mask),
               IP4ADDR_STRLEN_MAX);

    STRCPY(dhcpd_settings->dhcpd_gateway,
               (char *)dhcpd_settings->dhcpd_server_address,
               IP4ADDR_STRLEN_MAX);

    STRCPY(dhcpd_settings->dhcpd_primary_dns,
               (char *)dhcpd_settings->dhcpd_server_address,
               IP4ADDR_STRLEN_MAX);

    /* secondary DNS is not defined by default */
    STRCPY(dhcpd_settings->dhcpd_secondary_dns,
               "0.0.0.0",
               IP4ADDR_STRLEN_MAX);

    dhcpd_set_ip_pool(&tcpip_config->ap_addr,
                      &tcpip_config->ap_mask,
                      dhcpd_settings->dhcpd_ip_pool_start,
                      dhcpd_settings->dhcpd_ip_pool_end);
}



