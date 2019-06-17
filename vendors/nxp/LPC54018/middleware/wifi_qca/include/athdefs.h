//------------------------------------------------------------------------------
// Copyright (c) Qualcomm Atheros, Inc.
// All rights reserved.
// Redistribution and use in source and binary forms, with or without modification, are permitted (subject to
// the limitations in the disclaimer below) provided that the following conditions are met:
//
// · Redistributions of source code must retain the above copyright notice, this list of conditions and the
//   following disclaimer.
// · Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
//   following disclaimer in the documentation and/or other materials provided with the distribution.
// · Neither the name of nor the names of its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE. THIS SOFTWARE IS
// PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------
//==============================================================================
// Author(s): ="Atheros"
//==============================================================================
#ifndef __ATHDEFS_H__
#define __ATHDEFS_H__

/*
 * This file contains definitions that may be used across both
 * Host and Target software.  Nothing here is module-dependent
 * or platform-dependent.
 */

#include <stdint.h>
typedef uint32_t boolean;

/*
 * Generic error codes that can be used by hw, sta, ap, sim, dk
 * and any other environments. Since these are enums, feel free to
 * add any more codes that you need.
 */

typedef enum
{
    A_RESOURCES = -5,     /*TODO: fix later*/
    A_DEADLOCK = -4,     /*TODO: fix later*/
    A_TIMEOUT = -3,      /*TODO: fix later*/
    A_SOCK_INVALID = -2,
    A_ERROR = -1,        /* Generic error return */
    A_OK = 0,            /* success */
                         /* Following values start at 1 */
    A_DEVICE_NOT_FOUND,  /* not able to find PCI device */
    A_NO_MEMORY,         /* not able to allocate memory, not available */
    A_MEMORY_NOT_AVAIL,  /* memory region is not free for mapping */
    A_NO_FREE_DESC,      /* no free descriptors available */
    A_BAD_ADDRESS,       /* address does not match descriptor */
    A_WIN_DRIVER_ERROR,  /* used in NT_HW version, if problem at init */
    A_REGS_NOT_MAPPED,   /* registers not correctly mapped */
    A_EPERM,             /* Not superuser */
    A_EACCES,            /* Access denied */
    A_ENOENT,            /* No such entry, search failed, etc. */
    A_EEXIST,            /* The object already exists (can't create) */
    A_EFAULT,            /* Bad address fault */
    A_EBUSY,             /* Object is busy */
    A_EINVAL,            /* Invalid parameter */
    A_EMSGSIZE,          /* Inappropriate message buffer length */
    A_ECANCELED,         /* Operation canceled */
    A_ENOTSUP,           /* Operation not supported */
    A_ECOMM,             /* Communication error on send */
    A_EPROTO,            /* Protocol error */
    A_ENODEV,            /* No such device */
    A_EDEVNOTUP,         /* device is not UP */
    A_NO_RESOURCE,       /* No resources for requested operation */
    A_HARDWARE,          /* Hardware failure */
    A_PENDING,           /* Asynchronous routine; will send up results later (typically in callback) */
    A_EBADCHANNEL,       /* The channel cannot be used */
    A_DECRYPT_ERROR,     /* Decryption error */
    A_PHY_ERROR,         /* RX PHY error */
    A_CONSUMED,          /* Object was consumed */
    A_SOCKCXT_NOT_FOUND, /*Socket context not found*/
    A_UNKNOWN_CMD,       /*Unknown socket ocmmand*/
    A_SOCK_UNAVAILABLE,  /*Socket limit reached*/
    A_MEMFREE_ERROR      /*Error while freeing resource*/
} A_STATUS;

#define A_SUCCESS(x) (x == A_OK)
#define A_FAILED(x) (!A_SUCCESS(x))

typedef enum
{
    USER, /* Max Perf set by user */
    WPS,  /* Max Perf set by WPS  */
    P2P,  /* Max Perf set by P2P  */
    SOFTAP,
    SRCALL,
    DHCP,
    TCPL1,
    TCPL2,
    TKIP,
    RAWMODE,
    PWR_MAX = 32,
} A_PWR_MODULE;

#define EXPECTED_REF_CLK_AR4100 26000000
#define EXPECTED_REF_CLK_AR400X 40000000

#endif /* __ATHDEFS_H__ */
