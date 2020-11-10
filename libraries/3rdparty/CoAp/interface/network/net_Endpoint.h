/*******************************************************************************
 * Copyright (c)  2015  Dipl.-Ing. Tobias Rohde, http://www.lobaro.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/
#ifndef COM_NET_EP_H
#define COM_NET_EP_H

#include <stdint.h>
#include <stdbool.h>
#include "../../liblobaro_coap.h"

#define  IPv6_IP(A3, A2, A1, A0)   \
        { (A3 >> 24 & 0xff), (A3 >> 16 & 0xff), (A3 >> 8 & 0xff), (A3 >> 0 & 0xff), \
          (A2 >> 24 & 0xff), (A2 >> 16 & 0xff), (A2 >> 8 & 0xff), (A2 >> 0 & 0xff), \
          (A1 >> 24 & 0xff), (A1 >> 16 & 0xff), (A1 >> 8 & 0xff), (A1 >> 0 & 0xff), \
          (A0 >> 24 & 0xff), (A0 >> 16 & 0xff), (A0 >> 8 & 0xff), (A0 >> 0 & 0xff)}

extern const NetAddr_IPv6_t NetAddr_IPv6_unspecified;
extern const NetAddr_IPv6_t NetAddr_IPv6_mulitcast;
extern const NetAddr_IPv4_t NetAddr_IPv4_unspecified;
extern const NetAddr_IPv4_t NetAddr_IPv4_mulitcast;

extern const NetEp_t NetEp_IPv6_mulitcast;
extern const NetEp_t NetEp_IPv4_mulitcast;

bool EpAreEqual(const NetEp_t* ep_A, const NetEp_t* ep_B);
void CopyEndpoints(NetEp_t* Destination, const NetEp_t* Source);

#endif
