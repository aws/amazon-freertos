/*
 * Copyright (c) 2016-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== mqueue.h ========
 */

#ifndef ti_posix_ccs_mqueue__include
#define ti_posix_ccs_mqueue__include

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

#include <stddef.h>
#include <stdint.h>

#include "time.h"
#include "sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Message queue descriptor */
#ifndef _MQD_T_DECLARED
typedef void *mqd_t;    /* defined in TI ARM 18.1.0.LTS */
#define _MQD_T_DECLARED
#endif

/*
 *  ======== mq_attr ========
 */
struct mq_attr {
    long    mq_flags;    /* Message queue description flags: 0 or O_NONBLOCK.
                            Initialized from oflag argument of mq_open(). */
    long    mq_maxmsg;   /* Maximum number of messages on queue.  */
    long    mq_msgsize;  /* Maximum message size. */
    long    mq_curmsgs;  /* Number of messages currently queued. */
};

/*  Deprecated. This typedef is for compatibility with old SDKs. It is
 *  not part of the POSIX standard. It will be removed in a future
 *  release. TIRTOS-1317
 */
typedef struct mq_attr mq_attr;

/* For mq_open() */
#define O_CREAT         0x200   /* TODO: sys/fcntl.h? */
#define O_EXCL          0x0800  /* Error on open if queue exists */
#define O_RDONLY        0
#define O_WRONLY        1
#define O_RDWR          2
#define O_NONBLOCK      0x4000  /* Fail with EAGAIN if resources unavailable */

extern int mq_close(mqd_t mqdes);
extern int mq_getattr(mqd_t mqdes, struct mq_attr *mqstat);
extern mqd_t mq_open(const char *name, int oflags, ...);
extern ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len,
        unsigned int *msg_prio);
extern int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len,
        unsigned int msg_prio);
extern int mq_setattr(mqd_t mqdes, const struct mq_attr *mqstat,
        struct mq_attr *omqstat);
extern ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len,
        unsigned int *msg_prio, const struct timespec *abstime);
extern int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len,
        unsigned int msg_prio, const struct timespec *abstime);
extern int mq_unlink(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* ti_posix_ccs_mqueue__include */
