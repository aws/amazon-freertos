#ifndef __PING_API_H__
#define __PING_API_H__

#include <stdint.h>

#include "ping.h"

int
ping_custom_sync(const ip_addr_t* ping_addr, uint32_t count, uint32_t delay_ms);

#endif /* __PING_API_H__ */
