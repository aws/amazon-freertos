/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef MTK_LWIP_HDR_DEBUG_H
#define MTK_LWIP_HDR_DEBUG_H

#include "lwip/debug.h"


/**
 * LWIP_NOASSERT: Disable LWIP_ASSERT checks.
 * -- To disable assertions define LWIP_NOASSERT in arch/cc.h.
 */
#ifndef LWIP_NOASSERT

#ifdef MTK_DEBUG_LEVEL_NONE
#undef LWIP_ASSERT
#define LWIP_ASSERT(message, assertion)
#endif

#else  /* LWIP_NOASSERT */
#define LWIP_ASSERT(message, assertion)
#endif /* LWIP_NOASSERT */


#ifdef MTK_DEBUG_LEVEL_NONE
#undef LWIP_ERROR
#define LWIP_ERROR(message, expression, handler)
#endif

#endif /* LWIP_ERROR */

#ifdef LWIP_DEBUG
#ifndef LWIP_PLATFORM_DIAG
#error "If you want to use LWIP_DEBUG, LWIP_PLATFORM_DIAG(message) needs to be defined in your arch/cc.h"
#endif

#ifndef MTK_DEBUG_LEVEL_NONE
#if defined(MTK_LWIP_DYNAMIC_DEBUG_ENABLE)
#undef LWIP_DEBUGF
#define LWIP_DEBUGF(debug, message) do { \
                               if ( \
							     ((lwip_debug_flags[LWIP_DEBUG_IDX(debug)].debug_flag) & LWIP_DBG_ON) && \
                                 ((lwip_debug_flags[LWIP_DEBUG_IDX(debug)].debug_flag) & LWIP_DBG_TYPES_ON) && \
                                 ((s16_t)((debug) & LWIP_DBG_MASK_LEVEL) >= LWIP_DBG_MIN_LEVEL)) { \
                                 LWIP_PLATFORM_DIAG(message); \
                                 if ((debug) & LWIP_DBG_HALT) { \
                                   while(1); \
                                 } \
                               } \
                             } while(0)
#endif  /* MTK_LWIP_DYNAMIC_DEBUG_ENABLE */

#else /* MTK_DEBUG_LEVEL_NONE */
#define LWIP_DEBUGF(debug, message)
#endif /* MTK_DEBUG_LEVEL_NONE */



#ifdef MTK_BSP_LOOPBACK_ENABLE
unsigned int loopback_start();
void loopback_start_set(unsigned int start);
#endif /* MTK_BSP_LOOPBACK_ENABLE */

#endif /* MTK_LWIP_HDR_DEBUG_H */
