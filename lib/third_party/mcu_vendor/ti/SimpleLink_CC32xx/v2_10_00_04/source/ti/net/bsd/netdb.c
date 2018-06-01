/*
 * Copyright (c) 2017-2018, Texas Instruments Incorporated
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

#include <stdint.h>
#include <string.h>

#include <ti/net/bsd/sys/socket.h>

#include <ti/net/slnetutils.h>
#include <ti/net/bsd/netdb.h>

#define IPv4_ADDR_LEN                (4)
#define IPv6_ADDR_LEN                (16)

#define GET_HOST_BY_NAME_MAX_ANSWERS (5)

/*******************************************************************************/
/*  gethostbyname */
/*******************************************************************************/
struct hostent* gethostbyname(const char *name)
{
    static struct  hostent Hostent;
    static char    Addr[GET_HOST_BY_NAME_MAX_ANSWERS][IPv6_ADDR_LEN] = { {0} };
    uint16_t       AddrLen = GET_HOST_BY_NAME_MAX_ANSWERS;

    int RetVal = 0;

    /* Clear the reused buffer */
    memset(&Hostent, 0, sizeof(struct hostent));
    memset(&Addr, 0, sizeof(Addr));

    /* Set the host name */
    Hostent.h_name = (char *)name;
    /* Query DNS for IPv4 address. */
    RetVal = SlNetUtil_getHostByName(0, (char *)name, strlen(name), (uint32_t *)Addr, &AddrLen, AF_INET);

    if(RetVal < 0)
    {
        /* If call fails, try again for IPv6. */
        RetVal = SlNetUtil_getHostByName(0, (char *)name, strlen(name), (uint32_t *)Addr, &AddrLen, AF_INET6);
        if(RetVal < 0)
        {
            /* if the request failed twice, there's an error - return NULL. */
            return NULL;
        }
        else
        {
            /* fill the answer fields */
            Hostent.h_addrtype = AF_INET6;
            Hostent.h_length = IPv6_ADDR_LEN;
        }
    }
    else
    {
        /* fill the answer fields */
        Hostent.h_addrtype = AF_INET;
        Hostent.h_length = IPv4_ADDR_LEN;
    }

    Hostent.h_addr_list = (char **)Addr;

    /* Return the address of the reused buffer */
    return (&Hostent);
}
