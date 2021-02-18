/*
 * Copyright (c) 2017-2020, Texas Instruments Incorporated
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

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include <errno.h>

#include <ti/net/bsd/sys/socket.h>
#include <ti/net/bsd/arpa/inet.h>
#include <ti/net/bsd/errnoutil.h>
#include <ti/net/slnetsock.h>
#include <ti/net/slnetutils.h>
#include <ti/net/slneterr.h>

/* The implementation assumes the various SLNETSOCK_MSG_ flags passed
 * to SlNetSock send/recv (and friends) are equivalent to the BSD
 * definitions.
 *
 * Here we defensively ensure (at compile time) that this assumption
 * is valid.  If these #errors ever fire, it means there is likely
 * flag-translation work needed.
 */
#if (SLNETSOCK_MSG_OOB != MSG_OOB)
#error Conflicting definitions of MSG_OOB
#endif
#if (SLNETSOCK_MSG_PEEK != MSG_PEEK)
#error Conflicting definitions of MSG_PEEK
#endif
#if (SLNETSOCK_MSG_WAITALL != MSG_WAITALL)
#error Conflicting definitions of MSG_WAITALL
#endif
#if (SLNETSOCK_MSG_DONTWAIT != MSG_DONTWAIT)
#error Conflicting definitions of MSG_DONTWAIT
#endif
#if (SLNETSOCK_MSG_DONTROUTE != MSG_DONTROUTE)
#error Conflicting definitions of MSG_DONTROUTE
#endif
#if (SLNETSOCK_MSG_NOSIGNAL != MSG_NOSIGNAL)
#error Conflicting definitions of MSG_NOSIGNAL
#endif

/*******************************************************************************/
/*  socket */
/*******************************************************************************/

int socket(int Domain, int Type, int Protocol)
{
    int RetVal = (int)SlNetSock_create(Domain, Type, Protocol, 0, 0);
    return ErrnoUtil_set(RetVal);
}


/*******************************************************************************/
/*  shutdown */
/*******************************************************************************/

int shutdown(int fd, int how)
{
    int RetVal = (int)SlNetSock_shutdown((uint16_t)fd, (uint16_t)how);
    return ErrnoUtil_set(RetVal);
}


/*******************************************************************************/
/*  accept */
/*******************************************************************************/

int accept(int fd, struct sockaddr *addr, socklen_t *addrlen)
{
    int RetVal = (int)SlNetSock_accept((int16_t)fd, (SlNetSock_Addr_t *)addr, addrlen);
    return ErrnoUtil_set(RetVal);
}


/*******************************************************************************/
/*  bind */
/*******************************************************************************/

int bind(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
    int RetVal = (int)SlNetSock_bind((int16_t)fd, (const SlNetSock_Addr_t *)addr, addrlen);
    return ErrnoUtil_set(RetVal);
}


/*******************************************************************************/
/*  listen */
/*******************************************************************************/

int listen(int fd, int backlog)
{
    int RetVal = (int)SlNetSock_listen((int16_t)fd, backlog);
    return ErrnoUtil_set(RetVal);
}


/*******************************************************************************/
/*  connect */
/*******************************************************************************/

int connect(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
    int RetVal = (int)SlNetSock_connect((int16_t)fd, (const SlNetSock_Addr_t *)addr, addrlen);
    return ErrnoUtil_set(RetVal);
}


/*******************************************************************************/
/*  getpeername */
/*******************************************************************************/

int getpeername(int fd, struct sockaddr *addr, socklen_t *addrlen)
{
    int RetVal = (int)SlNetSock_getPeerName((int16_t)fd, (SlNetSock_Addr_t *)addr, addrlen);
    return ErrnoUtil_set(RetVal);
}

/*******************************************************************************/
/*  getsockname */
/*******************************************************************************/

int getsockname(int fd, struct sockaddr *addr, socklen_t *addrlen)
{
    int RetVal = (int)SlNetSock_getSockName((int16_t)fd, (SlNetSock_Addr_t *)addr, addrlen);
    return ErrnoUtil_set(RetVal);
}

/*******************************************************************************/
/*  setsockopt */
/*******************************************************************************/

int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
    const struct linger *tempLinger;

    /* Negative l_linger values are not allowed */
    if (optname == SO_LINGER && level == SOL_SOCKET)
    {
        tempLinger = optval;
        if (tempLinger->l_linger < 0)
        {
            return ErrnoUtil_set(SLNETERR_BSD_EDOM);
        }
    }

    int RetVal = (int)SlNetSock_setOpt((int16_t)fd, level, optname, (void *)optval, optlen);
    return ErrnoUtil_set(RetVal);
}


/*******************************************************************************/
/*  getsockopt */
/*******************************************************************************/

int getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen)
{
    int RetVal;
    if ( (optname == SLNETSOCK_OPSOCK_SLNETSOCKSD) && (level == SOL_SOCKET) )
    {
        if ( (NULL != optlen) && (*optlen >= sizeof((int16_t)fd)) )
        {
            *(uint16_t *)optval = (int16_t)fd;
            *optlen = sizeof(uint16_t);
            RetVal = SLNETERR_RET_CODE_OK;
        }
        else
        {
            RetVal = EINVAL;
        }
    }
    else
    {
        RetVal = (int)SlNetSock_getOpt((int16_t)fd, level, optname, optval, optlen);
    }
    return ErrnoUtil_set(RetVal);
}


/*******************************************************************************/
/*  recv */
/*******************************************************************************/

ssize_t recv(int fd, void *pBuf, size_t Len, int flags)
{
    ssize_t RetVal;

    /* Note, we assume here (and validated above!) that BSD flag
     * values matched the SLNETSOCK flag values.  So we can blindly
     * pass BSD flags into this SlNetSock call.
     *
     * Note on cast from size_t to uint32_t - this cast is OK for the ILP32 data
     * model, as both size_t and uint32_t are 32 bit unsigned integers in that
     * case. However, for 64 bit data models (e.g. LP64), size_t will be a
     * 64 bit unsigned integer, while uint32_t remains 32 bits in size. This
     * difference will result in data loss due to truncation, if the user calls
     * recv() with a value of Len greater than the max value for uint32_t.
     * (tracked by NS-308)
     */
    RetVal = (ssize_t)SlNetSock_recv((int16_t)fd, pBuf, (uint32_t)Len, flags);
    return (ssize_t)(ErrnoUtil_set(RetVal));
}


/*******************************************************************************/
/*  recvfrom */
/*******************************************************************************/

ssize_t recvfrom(int fd, void *buf, size_t Len, int flags, struct sockaddr *from, socklen_t *fromlen)
{
    ssize_t RetVal;

    /* Note, we assume here (and validated above!) that BSD flag
     * values matched the SLNETSOCK flag values.  So we can blindly
     * pass BSD flags into this SlNetSock call.
     *
     * Note: there is an implicit cast from size_t to uint32_t here for the Len
     * parameter - this case is OK for the ILP32 data
     * model, as both size_t and uint32_t are 32 bit unsigned integers in that
     * case. However, for 64 bit data models (e.g. LP64), size_t will be a
     * 64 bit unsigned integer, while uint32_t remains 32 bits in size. This
     * difference will result in data loss due to truncation, if the user calls
     * recvfrom() with a value of Len greater than the max value for uint32_t.
     * (tracked by NS-308)
     */
    RetVal = (ssize_t)SlNetSock_recvFrom((int16_t)fd, buf, Len, flags,
            (SlNetSock_Addr_t *)from, fromlen);
    return (ssize_t)(ErrnoUtil_set(RetVal));
}


/*******************************************************************************/
/*  send */
/*******************************************************************************/

ssize_t send(int fd, const void *pBuf, size_t Len, int flags)
{
    ssize_t RetVal;

    /* Note, we assume here (and validated above!) that BSD flag
     * values matched the SLNETSOCK flag values.  So we can blindly
     * pass BSD flags into this SlNetSock call.
     *
     * Note: there is an implicit cast from size_t to uint32_t here for the Len
     * parameter - this case is OK for the ILP32 data
     * model, as both size_t and uint32_t are 32 bit unsigned integers in that
     * case. However, for 64 bit data models (e.g. LP64), size_t will be a
     * 64 bit unsigned integer, while uint32_t remains 32 bits in size. This
     * difference will result in data loss due to truncation, if the user calls
     * send() with a value of Len greater than the max value for uint32_t.
     * (tracked by NS-308)
     */
    RetVal = (ssize_t)SlNetSock_send((int16_t)fd, pBuf, Len, flags);
    return (ssize_t)(ErrnoUtil_set(RetVal));
}


/*******************************************************************************/
/*  sendto */
/*******************************************************************************/

ssize_t sendto(int fd, const void *pBuf, size_t Len, int flags, const struct sockaddr *to, socklen_t tolen)
{
    ssize_t RetVal;

    /* Note, we assume here (and validated above!) that BSD flag
     * values matched the SLNETSOCK flag values.  So we can blindly
     * pass BSD flags into this SlNetSock call.
     *
     * Note on cast from size_t to uint32_t - this cast is OK for the ILP32 data
     * model, as both size_t and uint32_t are 32 bit unsigned integers in that
     * case. However, for 64 bit data models (e.g. LP64), size_t will be a
     * 64 bit unsigned integer, while uint32_t remains 32 bits in size. This
     * difference will result in data loss due to truncation, if the user calls
     * sendto() with a value of Len greater than the max value for uint32_t.
     * (tracked by NS-308)
     */
    RetVal = (ssize_t)SlNetSock_sendTo((int16_t)fd, pBuf, (uint32_t)Len, flags,
            (const SlNetSock_Addr_t *)to, tolen);
    return (ssize_t)(ErrnoUtil_set(RetVal));
}

/*******************************************************************************/
/*  inet_pton */
/*******************************************************************************/

int inet_pton(int af, const char *src, void *dst)
{
    int ret;

    /* This check is already performed in SlNetUtil_inetPton(), but we do it
     * here because af is an int16_t argument there. This way we can ensure af
     * will not be truncated once it gets to SlNetUtil_inetPton().
     */
    if(af != AF_INET && af != AF_INET6)
    {
        ret = ErrnoUtil_set(SLNETERR_BSD_EAFNOSUPPORT);
    }
    else
    {
        ret = SlNetUtil_inetPton(af, src, dst);
    }

    return ret;
}

/*******************************************************************************/
/*  inet_ntop */
/*******************************************************************************/

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    const char *ret;

    if(af != AF_INET && af != AF_INET6)
    {
        ErrnoUtil_set(SLNETERR_BSD_EAFNOSUPPORT);
        ret = NULL;
    }
    else
    {
        ret = SlNetUtil_inetNtop(af, src, dst, size);
        if(ret == NULL)
        {
            ErrnoUtil_set(SLNETERR_BSD_ENOSPC);
        }
    }

    return ret;
}
