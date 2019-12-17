/**
 * @file lwip_cli.h
 *
 *  lwip_cli command
 *
 */

#ifndef __LWIP_CLI_H__
#define __LWIP_CLI_H__

#if defined(MTK_MINICLI_ENABLE)

/**
 * @ingroup LwIP
 * @addtogroup CLI
 * @{
 */


#include <stdint.h>

#include <cli.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * LwIP CLI declaration.
 *
 * Provides commands to debug
 */
extern cmd_t    lwip_cli[];


/**
 * IP configuration command handler.
 *
 * Provides commands to config IP address mode (DHCP/STATIC) and IP address,
 * netmask, and gateway.
 */
uint8_t lwip_ip_cli(uint8_t len, char *param[]);


#if defined(MTK_LWIP_CLI_ENABLE)
#define IP_CLI_ENTRY       { "ip",   "ip config",  lwip_ip_cli, NULL     },
#else
#define IP_CLI_ENTRY
#endif


#ifdef __cplusplus
}
#endif


/** }@ */

#endif

#endif /* __LWIP_CLI_H__ */
