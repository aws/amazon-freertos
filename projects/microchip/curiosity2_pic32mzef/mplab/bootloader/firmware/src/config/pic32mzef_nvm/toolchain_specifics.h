/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef TOOLCHAIN_SPECIFICS_H
#define TOOLCHAIN_SPECIFICS_H

#include <sys/types.h>

#define NO_INIT                        __attribute__((section(".no_init")))
#define SECTION(a)                     __attribute__((__section__(a)))

#ifndef   __ASM
    #define __ASM                      __asm__
#endif
#ifndef   __INLINE
    #define __INLINE                   __inline__
#endif
#ifndef   __STATIC_INLINE
    #define __STATIC_INLINE            static __inline__
#endif
#ifndef   __STATIC_FORCEINLINE
    #define __STATIC_FORCEINLINE       __attribute__((always_inline)) static __inline__
#endif
#ifndef   __NO_RETURN
    #define __NO_RETURN                __attribute__((__noreturn__))
#endif
#ifndef   __USED
    #define __USED                     __attribute__((used))
#endif
#ifndef   __WEAK
    #define __WEAK                     __attribute__((weak))
#endif
#ifndef   __PACKED
    #define __PACKED                   __attribute__((packed, aligned(1)))
#endif
#ifndef   __PACKED_STRUCT
    #define __PACKED_STRUCT            struct __attribute__((packed, aligned(1)))
#endif
#ifndef   __PACKED_UNION
    #define __PACKED_UNION             union __attribute__((packed, aligned(1)))
#endif
#ifndef   __COHERENT
    #define __COHERENT                 __attribute__((coherent))
#endif
#ifndef   __ALIGNED
    #define __ALIGNED(x)               __attribute__((aligned(x)))
#endif
#ifndef   __RESTRICT
    #define __RESTRICT                 __restrict__
#endif

#define CACHE_ALIGN                    __COHERENT

#endif // end of header

