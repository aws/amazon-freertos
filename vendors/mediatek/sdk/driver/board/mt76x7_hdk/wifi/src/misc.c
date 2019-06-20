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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "misc.h"

static char *__rstrtok;

// TODO:  NOT Thread Safe (__rstrtok static variable) so far ...Michael
char *rstrtok(char *s, const char *ct)
{
    char *sbegin, *send;

    sbegin  = s ? s : __rstrtok;
    if (!sbegin) {
        return NULL;
    }

    sbegin += strspn(sbegin, ct);
    if (*sbegin == '\0') {
        __rstrtok = NULL;
        return (NULL);
    }

    send = strpbrk(sbegin, ct);
    if (send && *send != '\0') {
        *send++ = '\0';
    }

    __rstrtok = send;

    return (sbegin);
}

void convert_string_to_hex_array(char *string, unsigned char *hex)
{
    char *ptr, *keys = string;
    int index = 0;
    unsigned char hb, lb;
    ptr = rstrtok((char *)keys, ",");
    if (NULL == ptr) {
        return;
    }
    printf("[%d] token=%s\n", index, ptr);
    for (int i = 0; i < strlen(ptr); i++) {
        hb = ptr[i];
        if (hb >= 'A' && hb <= 'F') {
            hb = hb - 'A' + 10;
        } else if (hb >= '0' && hb <= '9') {
            hb = hb - '0';
        }
        i++;
        lb = ptr[i];
        if (lb >= 'A' && lb <= 'F') {
            lb = lb - 'A' + 10;
        } else if (lb >= '0' && lb <= '9') {
            lb = lb - '0';
        }
        hex[index++] = (hb << 4) | (lb);
    }
}

void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen)
{
    unsigned char *pt;
    int x;

    pt = pSrcBufVA;
    printf("%s: %p, len = %d\n\r", str, pSrcBufVA, SrcBufLen);
    for (x = 0; x < SrcBufLen; x++) {
        if (x % 16 == 0) {
            printf("0x%04x : ", x);
        }
        printf("%02x ", ((unsigned char)pt[x]));
        if (x % 16 == 15) {
            printf("\n\r");
        }
    }
    printf("\n\r");
}

void hex_dump_limit_len(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen, unsigned int limit_len)
{
    unsigned char *pt;
    int x;
    unsigned int print_len = SrcBufLen;

    pt = pSrcBufVA;

    if (print_len > limit_len) {
        print_len = limit_len;
    }

    printf("%s: %p, len = %d, dump lenght is limited to %u\n\r",
           str,
           pSrcBufVA,
           SrcBufLen,
           limit_len);

    for (x = 0; x < print_len; x++) {
        if (x % 16 == 0) {
            printf("0x%04x : ", x);
        }
        printf("%02x ", ((unsigned char)pt[x]));
        if (x % 16 == 15) {
            printf("\n\r");
        }
    }

    printf("\n\r");
}


static char strbuf[20 + 1];

char *uint64ToDecStr(uint64_t value)
{

    char *p = strbuf + sizeof(strbuf);
    bool first;

    *(--p) = '\0';

    for (first = true; value || first; first = false) {
        const uint32_t digit = value % 10;
        const char c = '0' + digit;
        *(--p) = c;
        value = value / 10;
    }

    return p;
}

void ignore_spaces_dash(char *pin_src, char *pin_dst)
{
    int i, j = 0;
    int len = 0;

    if (pin_src) {
        len = strlen(pin_src);
        for (i = 0; i < len; i++) {
            if (pin_src[i] == 0x20) { // ' '
                continue;
            }
            if (pin_src[i] == 0x2d) { // '-'
                continue;
            }
            pin_dst[j++] = pin_src[i];
        }
    }
}

