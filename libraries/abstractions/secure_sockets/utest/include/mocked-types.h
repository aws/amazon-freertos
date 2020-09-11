#ifndef MOCKED_TYPES_H
#define MOCKED_TYPES_H
#include "lwip/ip_addr.h"
typedef void (* dns_found_callback)( const char * name,
                                     const ip_addr_t * ipaddr,
                                     void * callback_arg );
#endif /* MOCKED_TYPES_H */
