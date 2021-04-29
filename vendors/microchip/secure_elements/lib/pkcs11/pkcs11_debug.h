/**
 * \file
 * \brief PKCS11 Library Debugging
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
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
#define PKCS11_DEBUG_NOFILE(fmt, ...) { fflush(stderr); fprintf(stderr, "%d:%d:" fmt, getpid(), (int)syscall(SYS_gettid), ## __VA_ARGS__); fflush(stderr); }
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
