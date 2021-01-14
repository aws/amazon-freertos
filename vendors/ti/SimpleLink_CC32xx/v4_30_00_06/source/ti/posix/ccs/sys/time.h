/*
 * Copyright (c) 2017-2019 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== sys/time.h ========
 */

#ifndef ti_posix_ccs_sys_time__include
#define ti_posix_ccs_sys_time__include

/* compiler vendor check */
/* short-term migration warning; ticlang users should migrate away */
#if defined(__clang__)
#warning ticlang users should migrate to the ti/posix/ticlang include path
#endif

#if !defined(__TI_COMPILER_VERSION__) && !defined(__clang__)
#error Incompatible compiler: use this include path (.../ti/posix/ccs) only \
with a Texas Instruments compiler. You appear to be using a different compiler.
#endif

/* CODEGEN-6425 work-around; remove when bug is fixed */
#if defined(__clang__) && defined(__ti_version__)
#pragma clang system_header
#endif

#if !defined(__TMS470__) || (__TI_COMPILER_VERSION__ < 18001000)
#include <stdint.h>
#include <stddef.h>

/* include compiler time.h */
#if defined(__clang__)
/* TI ARM CLang Compiler */
#include <../../include/c/time.h>
#else
#include <../include/time.h>
#endif

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct timeval {
  time_t      tv_sec;
  suseconds_t tv_usec;
};

#ifdef __cplusplus
}
#endif

#else
#include <../include/sys/_timeval.h>
#endif

#endif /* ti_posix_ccs_sys_time__include */
