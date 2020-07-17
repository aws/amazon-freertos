/*
 * Amazon FreeRTOS Wi-Fi V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_wifi_lwip.c
 * @brief Wi-Fi Interface functions specifi to lwIP.
 */

/* Socket and Wi-Fi interface includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <iot_wifi.h>
#include <iot_wifi_common.h>

/* Wi-Fi configuration includes. */
#include "aws_wifi_config.h"

/* Board and Abstraction layer includes */
#include <cy_result.h>
#include <cybsp_wifi.h>
#include <cyabs_rtos.h>
#include <cy_utils.h>
#include <cy_network_buffer.h>

#include "emac_eapol.h"
/* Wi-Fi Host driver includes. */
#include "whd.h"
#include "whd_wifi_api.h"
#include "whd_network_types.h"
#include "whd_buffer_api.h"

#if defined(CY_USE_LWIP)

/* lwIP stack includes */
#include <lwipopts.h>
#include <lwip/dhcp.h>
#include <lwip/dns.h>
#include <lwip/inet.h>
#include <lwip/netdb.h>
#include <lwip/netif.h>
#include <lwip/netifapi.h>
#include <lwip/init.h>
#include <lwip/dhcp.h>
#include <lwip/etharp.h>
#include <lwip/tcpip.h>
#include <lwip/ethip6.h>
#include <lwip/igmp.h>
#include <netif/ethernet.h>

#define MULTICAST_IP_TO_MAC(ip)       { (uint8_t) 0x01,             \
                                        (uint8_t) 0x00,             \
                                        (uint8_t) 0x5e,             \
                                        (uint8_t) ((ip)[1] & 0x7F), \
                                        (uint8_t) (ip)[2],          \
                                        (uint8_t) (ip)[3]           \
                                      }

#define CY_MAX_DHCP_ITERATION_COUNT        (1000)
#define CY_DHCP_NOT_COMPLETE               (3)

/** Static IP address structure */
typedef struct
{
    ip_addr_t addr ;
    ip_addr_t netmask ;
    ip_addr_t gateway ;
} ip_static_addr_t ;

static ip_static_addr_t staticAddr = 
{
    IPADDR4_INIT_BYTES(configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3),
    IPADDR4_INIT_BYTES(configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3),
    IPADDR4_INIT_BYTES(configGATEWAY_ADDR0, configGATEWAY_ADDR1, configGATEWAY_ADDR2, configGATEWAY_ADDR3)
};

static struct netif *netInterface = NULL;
static bool isNetworkUp = false;
static bool isNetifAdded = false;
static bool isApStarted = false;
static bool isDhcpEnabled = false;
eapol_packet_handler_t emac_eapol_packet_handler = NULL;

/*----------------------- lwIP Helpers -------------------------------*/
static void emac_receive_eapol_packet(whd_interface_t interface, whd_buffer_t buffer)
{
    if (buffer != NULL)
    {
        if (emac_eapol_packet_handler != NULL) 
        {
            emac_eapol_packet_handler(interface, buffer);
        } else
        {
            whd_buffer_release(interface->whd_driver, buffer, WHD_NETWORK_RX);
        }
    }
}

whd_result_t cy_emac_register_eapol_packet_handler(eapol_packet_handler_t eapol_packet_handler)
{
    if (emac_eapol_packet_handler == NULL)
    {
        emac_eapol_packet_handler = eapol_packet_handler;
        return WHD_SUCCESS;
    }

    return WHD_HANDLER_ALREADY_REGISTERED;
}

void cy_emac_unregister_eapol_packet_handler(void)
{
    emac_eapol_packet_handler = NULL;
}

void cy_network_process_ethernet_data(whd_interface_t ifp, whd_buffer_t buf)
{
    uint8_t *data = whd_buffer_get_current_piece_data_pointer(ifp->whd_driver, buf);
    uint16_t size = whd_buffer_get_current_piece_size(ifp->whd_driver, buf);
    uint16_t ethertype;

    if (size > WHD_ETHERNET_SIZE)
    {
        ethertype = (uint16_t)(data[12] << 8 | data[13]);
        if (ethertype == EAPOL_PACKET_TYPE) {
            /* pass it to the EAP layer, but do not release the packet */
            emac_receive_eapol_packet(ifp, buf);
        }
        else
        {
            if (netInterface != NULL)
            {
                if (netInterface->input(buf, netInterface) != ERR_OK)
                {
                    cy_buffer_release(buf, WHD_NETWORK_RX);
                }
            }
            else
            {
                cy_buffer_release(buf, WHD_NETWORK_RX);
            }
        }
    }
}

static struct pbuf *pbuf_dup(const struct pbuf *orig)
{
    struct pbuf *p = pbuf_alloc(PBUF_LINK, orig->tot_len, PBUF_RAM);
    if (p != NULL)
    {
        pbuf_copy(p, orig);
        p->flags = orig->flags;
    }
    return p;
}

static err_t low_level_output(struct netif *iface, struct pbuf *p)
{
    if (whd_wifi_is_ready_to_transceive((whd_interface_t)iface->state) != WHD_SUCCESS)
    {
        printf("wifi is not ready, packet not sent\n");
        return ERR_INPROGRESS;
    }

    struct pbuf *whd_buf = pbuf_dup(p);
    if (whd_buf == NULL)
    {
        printf("failed to allocate buffer for outgoing packet\n");
        return ERR_MEM;
    }
    whd_network_send_ethernet_data((whd_interface_t)iface->state, whd_buf);
    return ERR_OK;
}

static err_t igmp_filter(struct netif *iface, const ip4_addr_t *group, enum netif_mac_filter_action action)
{
    whd_mac_t mac = { MULTICAST_IP_TO_MAC((uint8_t*)group) };

    switch ( action )
    {
        case NETIF_ADD_MAC_FILTER:
            if ( whd_wifi_register_multicast_address( (whd_interface_t)iface->state, &mac ) != CY_RSLT_SUCCESS )
            {
                return ERR_VAL;
            }
            break;

        case NETIF_DEL_MAC_FILTER:
            if ( whd_wifi_unregister_multicast_address( (whd_interface_t)iface->state, &mac ) != CY_RSLT_SUCCESS )
            {
                return ERR_VAL;
            }
            break;

        default:
            return ERR_VAL;
    }
    return ERR_OK;
}

static err_t ethernetif_init(struct netif *iface)
{
    cy_rslt_t res;
    whd_mac_t macaddr;    

    iface->output = etharp_output;
    iface->linkoutput = low_level_output;
    iface->name[0] = 'w';
    iface->name[1] = 'l';

    res = whd_wifi_get_mac_address((whd_interface_t)iface->state, &macaddr);
    if (res != CY_RSLT_SUCCESS)
    {
        printf("initLWIP: whd_wifi_get_mac_address call failed, err = %lx", (unsigned long)res);
        return res;
    }  

    memcpy(&iface->hwaddr, &macaddr, sizeof(macaddr));
    iface->hwaddr_len = sizeof(macaddr);

    iface->mtu = WHD_LINK_MTU;
    iface->flags |= (NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP);

#if LWIP_IPV6 == 1
    iface->output_ip6 = ethip6_output;

    iface->ip6_autoconfig_enabled = 1;
    netif_create_ip6_linklocal_address(iface, 1);
    netif_set_igmp_mac_filter(iface, igmp_filter);
    netif_set_mld_mac_filter(iface, NULL);

    macaddr.octet[0] = 0x33;
    macaddr.octet[1] = 0x33;
    macaddr.octet[2] = 0xff;  
    whd_wifi_register_multicast_address((whd_interface_t)iface->state, &macaddr);

    memset(&macaddr, 0, sizeof(macaddr));
    macaddr.octet[0] = 0x33;
    macaddr.octet[1] = 0x33;
    macaddr.octet[5] = 0x01;
    whd_wifi_register_multicast_address((whd_interface_t)iface->state, &macaddr);

    memset(&macaddr, 0, sizeof(macaddr));
    macaddr.octet[0] = 0x33;
    macaddr.octet[1] = 0x33;
    macaddr.octet[5] = 0x02;
    whd_wifi_register_multicast_address((whd_interface_t)iface->state, &macaddr);
#endif

    return ERR_OK;
}

void checkDhcpStatus(void * arg)
{
    cy_rslt_t* result = (cy_rslt_t *)arg;
    *result = dhcp_supplied_address(netInterface) ? CY_RSLT_SUCCESS : CY_DHCP_NOT_COMPLETE;
}

static cy_rslt_t cywifi_lwip_bringup(void)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    int itr_count = 0;
    netifapi_netif_set_link_up(netInterface);
    netifapi_netif_set_up(netInterface);
    if (isDhcpEnabled)
    {
        netifapi_dhcp_start(netInterface);
        result = CY_DHCP_NOT_COMPLETE;
        while(result != CY_RSLT_SUCCESS && itr_count < CY_MAX_DHCP_ITERATION_COUNT)
        {
            tcpip_callback(checkDhcpStatus, &result);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    return result;
}

static cy_rslt_t cywifi_lwip_bringdown(void)
{
    /* Bring down DHCP */
    if (isDhcpEnabled) {
        netifapi_dhcp_release(netInterface);
        netifapi_dhcp_stop(netInterface);
    }
    netifapi_netif_set_down(netInterface);
    netifapi_netif_set_link_down(netInterface);

    return CY_RSLT_SUCCESS;
}

static WIFIReturnCode_t network_stack_bringup(whd_interface_t iface, const ip_static_addr_t* ip_addr)
{
    if (!isNetifAdded)
    {
        configASSERT(netInterface == NULL);
        netInterface = (struct netif *)calloc(sizeof(struct netif), 1);
        netifapi_netif_add(
            netInterface,
            0,
            0,
            0,
            iface,
            ethernetif_init,
            tcpip_input);
        netifapi_netif_set_default(netInterface);
        isNetifAdded = true;
    }

    if (ip_addr != NULL)
    {
        netifapi_netif_set_addr(
            netInterface, 
            &ip_addr->addr.u_addr.ip4,
            &ip_addr->netmask.u_addr.ip4,
            &ip_addr->gateway.u_addr.ip4);
        isDhcpEnabled = false;
    }
    else
    {
        isDhcpEnabled = true;
    }

    if (cywifi_lwip_bringup() != CY_RSLT_SUCCESS)
    {
        return eWiFiFailure;
    }

    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{
    if (isPoweredUp)
    {
        if ((devMode == eWiFiModeStation && WIFI_Disconnect() != eWiFiSuccess) ||
            (devMode == eWiFiModeAP && WIFI_StopAP() != eWiFiSuccess))
        {
            return eWiFiFailure;
        }

        if (wifiMutex != NULL && cy_rtos_deinit_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }

        if(isNetifAdded)
        {
            netifapi_netif_remove(netInterface);
            free(netInterface);
            netInterface = NULL;
            isNetifAdded = false;
        }

        if (cybsp_wifi_deinit(primaryInterface) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }

        isPoweredUp = false;
        devMode = eWiFiModeNotSupported;
    }
    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    cy_rslt_t res = CY_RSLT_SUCCESS;
    whd_ssid_t ssid;
    uint8_t *key;
    uint8_t keylen;
    whd_security_t security;
    uint8_t channel;

    cy_check_network_params(pxNetworkParams);
    
    if (isConnected && WIFI_Disconnect() != eWiFiSuccess)
    {
        return eWiFiFailure;
    }

    if (devMode != eWiFiModeStation &&
        devMode != eWiFiModeNotSupported &&
         WIFI_SetMode(eWiFiModeStation) != eWiFiSuccess)
    {
        return eWiFiFailure;
    }

    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        cy_convert_network_params(pxNetworkParams, &ssid, &key, &keylen, &security, &channel);

        res = whd_wifi_join(primaryInterface, &ssid, security, key, keylen);
        if (res != CY_RSLT_SUCCESS)
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }

        isConnected = true;
        devMode = eWiFiModeStation;

        if (!isNetworkUp)
        {
            if (network_stack_bringup(primaryInterface, (LWIP_DHCP == 1) ? NULL : &staticAddr) != eWiFiSuccess)
            {
                configPRINTF(("Failed to bring up network stack\n"));
                cy_rtos_set_mutex(&wifiMutex);
                return eWiFiFailure;
            }
            isNetworkUp = true;
        }
        if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
    }
    else
    {
        return eWiFiTimeout;
    }
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
    if (isConnected)
    {
        if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
        {
            if (cywifi_lwip_bringdown() != CY_RSLT_SUCCESS)
            {
                cy_rtos_set_mutex(&wifiMutex);
                return eWiFiFailure;
            }
            isNetworkUp = false;
            if (whd_wifi_leave(primaryInterface) != CY_RSLT_SUCCESS)
            {
                cy_rtos_set_mutex(&wifiMutex);
                return eWiFiFailure;
            }
            isConnected = false;
            if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
            {
                return eWiFiFailure;
            }
        }
        else
        {
            return eWiFiTimeout;
        }
    }
    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr )
{
    configASSERT(pucIPAddr != NULL);
    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        if (pucIPAddr == NULL && netInterface == NULL)
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }
        memcpy(pucIPAddr, &netInterface->ip_addr.u_addr.ip4, sizeof(netInterface->ip_addr.u_addr.ip4));
        if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
    }
    else
    {
        return eWiFiTimeout;
    }
    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr )
{
    configASSERT(pcHost != NULL && pucIPAddr != NULL);

    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        int                 err;
        struct addrinfo     hints;
        struct addrinfo     *result;
        struct sockaddr_in  *addr_in;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family     = AF_INET;
        hints.ai_socktype   = SOCK_DGRAM;
        hints.ai_flags      = 0;
        hints.ai_protocol   = 0;

        err = getaddrinfo(pcHost, NULL, &hints, &result);
        if (err != 0)
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }

        if (result)
        {
            /*
            * ai_addr is of struct sockaddr type.
            *
                struct sockaddr {
                u8_t        sa_len;
                sa_family_t sa_family;
                char        sa_data[14];
                };
            */
            addr_in = (struct sockaddr_in *)result->ai_addr;

            memcpy(pucIPAddr, &addr_in->sin_addr.s_addr,
                    sizeof(addr_in->sin_addr.s_addr));
        }
        else
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }

        freeaddrinfo(result);
        if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
    }
    else
    {
        return eWiFiTimeout;
    }
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartAP( void )
{
    if (!isApStarted)
    {
        if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
        {
            cy_rslt_t res = whd_wifi_start_ap(primaryInterface);
            if (res != CY_RSLT_SUCCESS)
            {
                configPRINTF(("Failed to start AP.\n"));
                cy_rtos_set_mutex(&wifiMutex);
                return eWiFiFailure;
            }
            isApStarted = true;

            if (!isNetworkUp)
            {
                if (network_stack_bringup(primaryInterface, &staticAddr) !=  eWiFiSuccess)
                {
                    configPRINTF(("Failed to bring up network stack\n"));
                    cy_rtos_set_mutex(&wifiMutex);
                    return eWiFiFailure;
                }
                isNetworkUp = true;
            }
            if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
            {
                return eWiFiFailure;
            }
        }
        else
        {
            return eWiFiTimeout;
        }
    }
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StopAP( void )
{
    if (isApStarted)
    {
        if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
        {
            if (cywifi_lwip_bringdown() != CY_RSLT_SUCCESS)
            {
                cy_rtos_set_mutex(&wifiMutex);
                return eWiFiFailure;
            }
            isNetworkUp = false;
            if (whd_wifi_stop_ap(primaryInterface) != CY_RSLT_SUCCESS)
            {
                cy_rtos_set_mutex(&wifiMutex);
                return eWiFiFailure;
            }
            isApStarted = false;
            if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
            {
                return eWiFiFailure;
            }
        }
        else
        {
            return eWiFiTimeout;
        }
    }
    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/
#endif /* CY_USE_LWIP */
