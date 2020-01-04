/**
 * \file
 * \brief PKCS11 Library Debugging
 *
 * \copyright (c) 2017 Microchip Technology Inc. and its subsidiaries.
 *            You may use this software and any derivatives exclusively with
 *            Microchip products.
 *
 * \page License
 *
 * (c) 2017 Microchip Technology Inc. and its subsidiaries. You may use this
 * software and any derivatives exclusively with Microchip products.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIPS TOTAL LIABILITY ON ALL CLAIMS IN
 * ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
 * TERMS.
 */

#ifndef PKCS11_DEBUG_H_
#define PKCS11_DEBUG_H_

#include "pkcs11_config.h"

#if PKCS11_DEBUG_ENABLE
const char * pkcs11_debug_get_ckr_name(CK_RV rv);
void pkcs11_debug_attributes(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);

#if defined(__linux__) || defined(__CYGWIN__)
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#define PKCS11_DEBUG_NOFILE(fmt, ...) { fflush(stdout); fprintf(stderr, "%d:%d:" fmt, getpid(), (int)syscall(SYS_gettid), ## __VA_ARGS__); fflush(stderr); }
#else
#define PKCS11_DEBUG_NOFILE(fmt, ...) { printf(fmt, ## __VA_ARGS__); }
#endif

#define PKCS11_DEBUG(fmt, ...)  PKCS11_DEBUG_NOFILE("%s:%d:" fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define PKCS11_DEBUG_RETURN(x)     { CK_RV __y = x; PKCS11_DEBUG("%s(%x)\n", pkcs11_debug_get_ckr_name(__y), (unsigned int)__y); return __y; }

#else

#define PKCS11_DEBUG_NOFILE(...)
#define PKCS11_DEBUG(...)
#define PKCS11_DEBUG_RETURN(x)     { return x; }
#define pkcs11_debug_attributes(x, y)

#endif


#endif /* PKCS11_DEBUG_H_ */
