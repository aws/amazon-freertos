/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/**
 * @file        linux_errno.h
 * @brief       the linux error return code
 */
 
#ifndef __LINUX_ERRNO_H__
#define __LINUX_ERRNO_H__

#define RET_EPERM            1      /* Operation not permitted */
#define RET_ENOENT           2      /* No such file or directory */
#define RET_ESRCH            3      /* No such process */
#define RET_EINTR            4      /* Interrupted system call */
#define RET_EIO              5      /* I/O error */
#define RET_ENXIO            6      /* No such device or address */
#define RET_E2BIG            7      /* Arg list too long */
#define RET_ENOEXEC          8      /* Exec format error */
#define RET_EBADF            9      /* Bad file number */
#define RET_ECHILD          10      /* No child processes */
#define RET_EAGAIN          11      /* Try again */
#define RET_ENOMEM          12      /* Out of memory */
#define RET_EACCES          13      /* Permission denied */
#define RET_EFAULT          14      /* Bad address */
#define RET_ENOTBLK         15      /* Block device required */
#define RET_EBUSY           16      /* Device or resource busy */
#define RET_EEXIST          17      /* File exists */
#define RET_EXDEV           18      /* Cross-device link */
#define RET_ENODEV          19      /* No such device */
#define RET_ENOTDIR         20      /* Not a directory */
#define RET_EISDIR          21      /* Is a directory */
#define RET_EINVAL          22      /* Invalid argument */
#define RET_ENFILE          23      /* File table overflow */
#define RET_EMFILE          24      /* Too many open files */
#define RET_ENOTTY          25      /* Not a typewriter */
#define RET_ETXTBSY         26      /* Text file busy */
#define RET_EFBIG           27      /* File too large */
#define RET_ENOSPC          28      /* No space left on device */
#define RET_ESPIPE          29      /* Illegal seek */
#define RET_EROFS           30      /* Read-only file system */
#define RET_EMLINK          31      /* Too many links */
#define RET_EPIPE           32      /* Broken pipe */
#define RET_EDOM            33      /* Math argument out of domain of func */
#define RET_ERANGE          34      /* Math result not representable */


#define RET_EDEADLK         35      /* Resource deadlock would occur */
#define RET_ENAMETOOLONG    36      /* File name too long */
#define RET_ENOLCK          37      /* No record locks available */
#define RET_ENOSYS          38      /* Function not implemented */
#define RET_ENOTEMPTY       39      /* Directory not empty */
#define RET_ELOOP           40      /* Too many symbolic links encountered */
#define RET_EWOULDBLOCK     RET_EAGAIN  /* Operation would block */
#define RET_ENOMSG          42      /* No message of desired type */
#define RET_EIDRM           43      /* Identifier removed */
#define RET_ECHRNG          44      /* Channel number out of range */
#define RET_EL2NSYNC        45      /* Level 2 not synchronized */
#define RET_EL3HLT          46      /* Level 3 halted */
#define RET_EL3RST          47      /* Level 3 reset */
#define RET_ELNRNG          48      /* Link number out of range */
#define RET_EUNATCH         49      /* Protocol driver not attached */
#define RET_ENOCSI          50      /* No CSI structure available */
#define RET_EL2HLT          51      /* Level 2 halted */
#define RET_EBADE           52      /* Invalid exchange */
#define RET_EBADR           53      /* Invalid request descriptor */
#define RET_EXFULL          54      /* Exchange full */
#define RET_ENOANO          55      /* No anode */
#define RET_EBADRQC         56      /* Invalid request code */
#define RET_EBADSLT         57      /* Invalid slot */

#define RET_EDEADLOCK       RET_EDEADLK

#define RET_EBFONT          59      /* Bad font file format */
#define RET_ENOSTR          60      /* Device not a stream */
#define RET_ENODATA         61      /* No data available */
#define RET_ETIME           62      /* Timer expired */
#define RET_ENOSR           63      /* Out of streams resources */
#define RET_ENONET          64      /* Machine is not on the network */
#define RET_ENOPKG          65      /* Package not installed */
#define RET_EREMOTE         66      /* Object is remote */
#define RET_ENOLINK         67      /* Link has been severed */
#define RET_EADV            68      /* Advertise error */
#define RET_ESRMNT          69      /* Srmount error */
#define RET_ECOMM           70      /* Communication error on send */
#define RET_EPROTO          71      /* Protocol error */
#define RET_EMULTIHOP       72      /* Multihop attempted */
#define RET_EDOTDOT         73      /* RFS specific error */
#define RET_EBADMSG         74      /* Not a data message */
#define RET_EOVERFLOW       75      /* Value too large for defined data type */
#define RET_ENOTUNIQ        76      /* Name not unique on network */
#define RET_EBADFD          77      /* File descriptor in bad state */
#define RET_EREMCHG         78      /* Remote address changed */
#define RET_ELIBACC         79      /* Can not access a needed shared library */
#define RET_ELIBBAD         80      /* Accessing a corrupted shared library */
#define RET_ELIBSCN         81      /* .lib section in a.out corrupted */
#define RET_ELIBMAX         82      /* Attempting to link in too many shared libraries */
#define RET_ELIBEXEC        83      /* Cannot exec a shared library directly */
#define RET_EILSEQ          84      /* Illegal byte sequence */
#define RET_ERESTART        85      /* Interrupted system call should be restarted */
#define RET_ESTRPIPE        86      /* Streams pipe error */
#define RET_EUSERS          87      /* Too many users */
#define RET_ENOTSOCK        88      /* Socket operation on non-socket */
#define RET_EDESTADDRREQ    89      /* Destination address required */
#define RET_EMSGSIZE        90      /* Message too long */
#define RET_EPROTOTYPE      91      /* Protocol wrong type for socket */
#define RET_ENOPROTOOPT     92      /* Protocol not available */
#define RET_EPROTONOSUPPORT 93      /* Protocol not supported */
#define RET_ESOCKTNOSUPPORT 94      /* Socket type not supported */
#define RET_EOPNOTSUPP      95      /* Operation not supported on transport endpoint */
#define RET_EPFNOSUPPORT    96      /* Protocol family not supported */
#define RET_EAFNOSUPPORT    97      /* Address family not supported by protocol */
#define RET_EADDRINUSE      98      /* Address already in use */
#define RET_EADDRNOTAVAIL   99      /* Cannot assign requested address */
#define RET_ENETDOWN        100     /* Network is down */
#define RET_ENETUNREACH     101     /* Network is unreachable */
#define RET_ENETRESET       102     /* Network dropped connection because of reset */
#define RET_ECONNABORTED    103     /* Software caused connection abort */
#define RET_ECONNRESET      104     /* Connection reset by peer */
#define RET_ENOBUFS         105     /* No buffer space available */
#define RET_EISCONN         106     /* Transport endpoint is already connected */
#define RET_ENOTCONN        107     /* Transport endpoint is not connected */
#define RET_ESHUTDOWN       108     /* Cannot send after transport endpoint shutdown */
#define RET_ETOOMANYREFS    109     /* Too many references: cannot splice */
#define RET_ETIMEDOUT       110     /* Connection timed out */
#define RET_ECONNREFUSED    111     /* Connection refused */
#define RET_EHOSTDOWN       112     /* Host is down */
#define RET_EHOSTUNREACH    113     /* No route to host */
#define RET_EALREADY        114     /* Operation already in progress */
#define RET_EINPROGRESS     115     /* Operation now in progress */
#define RET_ESTALE          116     /* Stale NFS file handle */
#define RET_EUCLEAN         117     /* Structure needs cleaning */
#define RET_ENOTNAM         118     /* Not a XENIX named type file */
#define RET_ENAVAIL         119     /* No XENIX semaphores available */
#define RET_EISNAM          120     /* Is a named type file */
#define RET_EREMOTEIO       121     /* Remote I/O error */
#define RET_EDQUOT          122     /* Quota exceeded */

#define RET_ENOMEDIUM       123     /* No medium found */
#define RET_EMEDIUMTYPE     124     /* Wrong medium type */

#endif  /*__LINUX_ERRNO_H__*/

