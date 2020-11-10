/*******************************************************************************
 * Copyright (c)  2015  Dipl.-Ing. Tobias Rohde, http://www.lobaro.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/

#ifndef USER_LOBARO_COAP_INTERFACE_COAP_INTERFACE_H_
#define USER_LOBARO_COAP_INTERFACE_COAP_INTERFACE_H_

//-------------------------------------------------------------------------
// lobaro-coap needs some std C language functions/headers
//-------------------------------------------------------------------------
	#include <stdlib.h>
	#include <stdarg.h>
	#include <stddef.h>
	#include <stdio.h>
	#include <string.h>

#ifdef LOBARO_HAL_ESP8266 //can be defined as "-D" compiler option
	//ESP8266 specific glue (because the sdk has no complete std c lib)
	//ESP8266 with partial libC from SDK
	#include <osapi.h>
	#include <os_type.h>

	#define coap_sprintf os_sprintf
	#define coap_printf  ets_uart_printf
	#define coap_memcpy  os_memcpy
	#define coap_memset  os_memset
	#define coap_memmove os_memmove
	#define coap_strlen  os_strlen
	#define	coap_strstr  os_strstr
	#define coap_strcpy  os_strcpy
	#define coap_memcmp  os_memcmp

	#define _rom ICACHE_FLASH_ATTR
	#define _ram
#else //in most cases a full standard C lib will be available and used by the stack
	#include <stdint.h>
	#include <stdbool.h>

	#define coap_sprintf sprintf
	#define coap_printf printf
	#define coap_memcpy memcpy
	#define coap_memset memset
	#define coap_memmove memmove
	#define coap_strlen strlen
	#define	coap_strstr strstr
	#define coap_strcpy strcpy
	#define coap_memcmp memcmp

	#define _rom
	#define _ram
#endif

//Memory Allocator
//if "DONT_USE_LOBARO_COAP_MEMORY_ALLOCATOR" is not defined via global "-D" compiler option
//then the following included functions must be supplied externally...
#include "mem/coap_mem.h"

//Debug/Uart/Terminal Output
//#include "debug/coap_debug.h"

//----------------------------------------------------------------------------------
//Interface "glue" to surrounding project/software
//Lobaro CoAP manages external network send and receive function in a pool of
//interfaces addressed by a socketHandle. To port the lib you must do the following:
//1) Get a free interface buffer from the CoAP stack by calling "CoAP_Socket_t* CoAP_NewSocket(Socket_t handle)"
//   This will give you a prefilled structure to work with.
//2) Attach your Send/TX data function/callback following the "NetTransmit_fn" function signature
//In the rx function you must determinate (or remember) your socket handle, because the stack uses only this as key
//3) Listen to socket
//----------------------------------------------------------------------------------
#include "network/net_Endpoint.h"
#include "network/net_Packet.h"
#include "network/net_Socket.h"

//-------------------------------------------------------------------------
//Implementation for these function prototypes must be provided externally:
//-------------------------------------------------------------------------
//Uart/Display function to print debug/status messages to
void hal_debug_puts(char *s);

#endif
