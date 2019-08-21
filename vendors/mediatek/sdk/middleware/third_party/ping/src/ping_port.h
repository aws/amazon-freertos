#ifndef __PING_PORT_H__
#define __PING_PORT_H__

#include "lwip/ip.h"

#define IPADDR_TYPE_V4                0
#define IPADDR_TYPE_V6                1

#if LWIP_IPV4 && LWIP_IPV6
#define IP_GET_TYPE(ipaddr)           ((ipaddr)->type)
#elif LWIP_IPV4
#define IP_GET_TYPE(ipaddr)           IPADDR_TYPE_V4
#else
#define IP_GET_TYPE(ipaddr)           IPADDR_TYPE_V6
#endif

#if LWIP_IPV4 && LWIP_IPV6
#define IP_IS_V4(ipaddr)              ((ipaddr)->type == IPADDR_TYPE_V4)
#elif LWIP_IPV4
#define IP_IS_V4(ipaddr)              1
#else
#define IP_IS_V4(ipaddr)              0
#endif

#if LWIP_IPV4

#define inet_addr_from_ip4addr(target_inaddr, source_ipaddr) ((target_inaddr)->s_addr = ip4_addr_get_u32(source_ipaddr))
#define inet_addr_to_ip4addr(target_ipaddr, source_inaddr)   (ip4_addr_set_u32(target_ipaddr, (source_inaddr)->s_addr))

/* directly map this to the lwip internal functions */
#define inet_addr(cp)                   ipaddr_addr(cp)
#define inet_aton(cp, addr)             ip4addr_aton(cp, (ip4_addr_t*)addr)
#define inet_ntoa(addr)                 ip4addr_ntoa((const ip4_addr_t*)&(addr))
#define inet_ntoa_r(addr, buf, buflen)  ip4addr_ntoa_r((const ip4_addr_t*)&(addr), buf, buflen)

#endif /* LWIP_IPV4 */

#endif /*  __PING_PORT_H__ */

