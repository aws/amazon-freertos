/*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/

/** @file wmerrno.h
*
*  @brief This file contains Error Management APIs
*/

#ifndef WM_ERRNO_H
#define WM_ERRNO_H

/* Get the module index number from error code (4th byte from LSB)*/
#define get_module_base(code) ((code&0xF000)>>12)

/* Get notifier message type i.e Error, Warning or Info (3rd byte from LSB)*/
#define get_notifier_msg_type(code) ((code&0x0F00)>>8)

/* Get module notifier code (2nd and 1st byte from LSB)*/
#define get_code(code) (code&0xFF)

#define MOD_ERROR_START(x)  (x << 12 | 0)
#define MOD_WARN_START(x)   (x << 12 | 1)
#define MOD_INFO_START(x)   (x << 12 | 2)

/* Create Module index */
#define MOD_GENERIC    0
/** Unused */
#define MOD_UNUSED_3   2
/** HTTPD module index */
#define MOD_HTTPD      3
/** Application framework module index */
#define MOD_AF         4
/** FTFS module index */
#define MOD_FTFS       5
/** RFGET module index */
#define MOD_RFGET      6
/** JSON module index  */
#define MOD_JSON       7
/** TELNETD module index */
#define MOD_TELNETD    8
/** SIMPLE MDNS module index */
#define MOD_SMDNS      9
/** EXML module index */
#define MOD_EXML       10
/** DHCPD module index */
#define MOD_DHCPD      11
/** MDNS module index */
#define MOD_MDNS       12
/** SYSINFO module index */
#define MOD_SYSINFO   13
/** Unused module index */
#define MOD_UNUSED_1     14
/** CRYPTO module index */
#define MOD_CRYPTO     15
/** HTTP-CLIENT module index */
#define MOD_HTTPC      16
/** PROVISIONING module index */
#define MOD_PROV       17
/** SPI module index */
#define MOD_SPI        18
/** PSM module index */
#define MOD_PSM        19
/** TTCP module index */
#define MOD_TTCP       20
/** DIAGNOSTICS module index */
#define MOD_DIAG       21
/** Unused module index */
#define MOD_UNUSED_2    22
/** WPS module index */
#define MOD_WPS        23
/** WLAN module index */
#define MOD_WLAN        24
/** USB module index */
#define MOD_USB        25
/** WIFI driver module index */
#define MOD_WIFI        26
/** Critical error module index */
#define MOD_CRIT_ERR    27
/** Last module index .Applications can define their own modules beyond this */
#define MOD_ERR_LAST	50

/* Globally unique success code */
#define WM_SUCCESS 0

enum wm_errno {
	/* First Generic Error codes */
	WM_GEN_E_BASE = MOD_ERROR_START(MOD_GENERIC),
	WM_FAIL,     /* 1 */
	WM_E_PERM,   /* 2: Operation not permitted */
	WM_E_NOENT,  /* 3: No such file or directory */
	WM_E_SRCH,   /* 4: No such process */
	WM_E_INTR,   /* 5: Interrupted system call */
	WM_E_IO,     /* 6: I/O error */
	WM_E_NXIO,   /* 7: No such device or address */
	WM_E_2BIG,   /* 8: Argument list too long */
	WM_E_NOEXEC, /* 9: Exec format error */
	WM_E_BADF,   /* 10: Bad file number */
	WM_E_CHILD,  /* 11: No child processes */
	WM_E_AGAIN,  /* 12: Try again */
	WM_E_NOMEM,  /* 13: Out of memory */
	WM_E_ACCES,  /* 14: Permission denied */
	WM_E_FAULT,  /* 15: Bad address */
	WM_E_NOTBLK, /* 16: Block device required */
	WM_E_BUSY,   /* 17: Device or resource busy */
	WM_E_EXIST,  /* 18: File exists */
	WM_E_XDEV,   /* 19: Cross-device link */
	WM_E_NODEV,  /* 20: No such device */
	WM_E_NOTDIR, /* 21: Not a directory */
	WM_E_ISDIR,  /* 22: Is a directory */
	WM_E_INVAL,  /* 23: Invalid argument */
	WM_E_NFILE,  /* 24: File table overflow */
	WM_E_MFILE,  /* 25: Too many open files */
	WM_E_NOTTY,  /* 26: Not a typewriter */
	WM_E_TXTBSY, /* 27: Text file busy */
	WM_E_FBIG,   /* 28: File too large */
	WM_E_NOSPC,  /* 29: No space left on device */
	WM_E_SPIPE,  /* 30: Illegal seek */
	WM_E_ROFS,   /* 31: Read-only file system */
	WM_E_MLINK,  /* 32: Too many links */
	WM_E_PIPE,   /* 33: Broken pipe */
	WM_E_DOM,    /* 34: Math argument out of domain of func */
	WM_E_RANGE,  /* 35: Math result not representable */

	/* WMSDK generic error codes */
	WM_E_CRC,    /* 36: Error in CRC check */
	WM_E_UNINIT,  /* 37: Module is not yet initialized */
	WM_E_TIMEOUT, /* 38: Timeout occurred during operation */
};

#endif /* ! WM_ERRNO_H */
