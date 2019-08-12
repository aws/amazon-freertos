#
# Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 # Cypress Semiconductor Corporation. All Rights Reserved.
 # 
 # This software, associated documentation and materials ("Software")
 # is owned by Cypress Semiconductor Corporation,
 # or one of its subsidiaries ("Cypress") and is protected by and subject to
 # worldwide patent protection (United States and foreign),
 # United States copyright laws and international treaty provisions.
 # Therefore, you may use this Software only as provided in the license
 # agreement accompanying the software package from which you
 # obtained this Software ("EULA").
 # If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 # non-transferable license to copy, modify, and compile the Software
 # source code solely for use in connection with Cypress's
 # integrated circuit products. Any reproduction, modification, translation,
 # compilation, or representation of this Software except as specified
 # above is prohibited without the express written permission of Cypress.
 #
 # Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 # EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 # reserves the right to make changes to the Software without notice. Cypress
 # does not assume any liability arising out of the application or use of the
 # Software or any product or circuit described in the Software. Cypress does
 # not authorize its products for use in any products where a malfunction or
 # failure of the Cypress product may reasonably be expected to result in
 # significant property damage, injury or death ("High Risk Product"). By
 # including Cypress's product in a High Risk Product, the manufacturer
 # of such system or application assumes all risk of such use and in doing
 # so agrees to indemnify Cypress against all liability.
#

NAME := LwIP

VERSION := 2.0.3

$(NAME)_COMPONENTS += WICED/network/LwIP/WWD
ifeq (,$(APP_WWD_ONLY)$(NS_WWD_ONLY)$(RTOS_WWD_ONLY))
$(NAME)_COMPONENTS += WICED/network/LwIP/WICED
endif

ifeq ($(BUILD_TYPE),debug)
GLOBAL_DEFINES := WICED_LWIP_DEBUG
endif

VALID_RTOS_LIST:= FreeRTOS

# Ethernet driver is not yet implemented for LwIP
PLATFORM_NO_GMAC := 1

# Define some macros to allow for some network-specific checks
GLOBAL_DEFINES += NETWORK_$(NAME)=1
GLOBAL_DEFINES += $(NAME)_VERSION=$$(SLASH_QUOTE_START)v$(VERSION)$$(SLASH_QUOTE_END)

# See GCC bug 48778. Some LwIP IP address macros fall over this
ifeq ($(TOOLCHAIN_TYPE),gcc)
GLOBAL_CFLAGS += -Wno-address
endif
export AFR_THIRDPARTY_PATH := ../../../../../../libraries/3rdparty/

GLOBAL_INCLUDES := $(AFR_THIRDPARTY_PATH)lwip/src/include \
                   $(AFR_THIRDPARTY_PATH)lwip/src/portable \
                   $(AFR_THIRDPARTY_PATH)lwip/src/portable/cypress/$(PLATFORM)/include \
                   WICED

$(NAME)_SOURCES :=  $(AFR_THIRDPARTY_PATH)lwip/src/api/api_lib.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/api/api_msg.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/api/err.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/api/netbuf.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/api/netdb.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/api/netifapi.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/api/sockets.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/api/tcpip.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv4/dhcp.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/dns.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/init.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ip.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv4/autoip.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv4/icmp.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv4/igmp.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/inet_chksum.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv4/ip4.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv4/ip4_addr.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv4/ip4_frag.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/def.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/timeouts.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/mem.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/memp.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/netif.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/pbuf.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/raw.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/apps/snmp/snmp_asn1.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/apps/snmp/snmp_mib2.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/apps/snmp/snmp_msg.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/stats.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/sys.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/tcp.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/tcp_in.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/tcp_out.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/udp.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv4/etharp.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/netif/ethernet.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv6/ethip6.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv6/dhcp6.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv6/icmp6.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv6/inet6.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv6/ip6.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv6/ip6_addr.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv6/ip6_frag.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv6/mld6.c \
                    $(AFR_THIRDPARTY_PATH)lwip/src/core/ipv6/nd6.c