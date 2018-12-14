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
#include <string.h>
#ifndef __CC_ARM
#include <strings.h> /* strings.h is part of string.h in ARM CC */
#endif
#include <stdarg.h>

#include "os.h"

#include <FreeRTOSConfig.h>
#include "FreeRTOS.h"

#include "task.h"

#include "type_def.h"

#define WIFI_ONLY_POOL

static TickType_t gSysBootTick = 0;

TEXT_IN_RAM void * os_memcpy(void *dest, const void *src, size_t n)
{
    char *d = dest;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

void *os_memmove(void *dest, const void *src, size_t n)
{
    if (dest < src) {
        os_memcpy(dest, src, n);
    } else {
        /* overlapping areas */
        char *d = (char *) dest + n;
        const char *s = (const char *) src + n;
        while (n--) {
            *--d = *--s;
        }
    }
    return dest;
}

TEXT_IN_RAM void * os_memset(void *s, int c, size_t n)
{
    char *p = s;
    while (n--) {
        *p++ = c;
    }
    return s;
}

int os_memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = s1, *p2 = s2;

    if (n == 0) {
        return 0;
    }

    while (*p1 == *p2) {
        p1++;
        p2++;
        n--;
        if (n == 0) {
            return 0;
        }
    }

    return *p1 - *p2;
}

size_t os_strlen(const char *s)
{
    const char *p = s;
    while (*p) {
        p++;
    }
    return p - s;
}


int os_strcasecmp(const char *s1, const char *s2)
{
    return strcasecmp(s1, s2);
}

int os_strncasecmp(const char *s1, const char *s2, size_t n)
{
    return strncasecmp(s1, s2, n);
}

char *os_strchr(const char *s, int c)
{
    return strchr(s, c);
}

char *os_strrchr(const char *s, int c)
{
    return strrchr(s, c);
}

int os_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

int os_strncmp(const char *s1, const char *s2, size_t n)
{
    return strncmp(s1, s2, n);
}

char *os_strncpy(char *dest, const char *src, size_t n)
{
    return strncpy(dest, src, n);
}

char *os_strstr(const char *haystack, const char *needle)
{
    return strstr(haystack, needle);
}

int os_snprintf(char *str, size_t size, const char *format, ...)
{
    int sz;

    va_list ap;

    va_start(ap, format);
    sz = vsnprintf(str, size, format, ap);
    va_end(ap);

    return sz;
}


int os_daemonize(const char *pid_file)
{
    return -1;
}


void os_daemonize_terminate(const char *pid_file)
{
}


char *os_rel2abs_path(const char *rel_path)
{
    return NULL; /* strdup(rel_path) can be used here */
}


int os_program_init(void)
{
    return 0;
}


void os_program_deinit(void)
{
}


int os_setenv(const char *name, const char *value, int overwrite)
{
    return -1;
}


int os_unsetenv(const char *name)
{
    return -1;
}


char *os_readfile(const char *name, size_t *len)
{
    return NULL;
}


size_t os_strlcpy(char *dest, const char *src, size_t size)
{
    const char *s = src;
    size_t left = size;

    if (left) {
        /* Copy string up to the maximum size of the dest buffer */
        while (--left != 0) {
            if ((*dest++ = *s++) == '\0') {
                break;
            }
        }
    }

    if (left == 0) {
        /* Not enough room for the string; force NUL-termination */
        if (size != 0) {
            *dest = '\0';
        }
        while (*s++)
            ; /* determine total src string length */
    }

    return s - src - 1;
}

size_t os_memrlen(const void *in_src, size_t in_max_len)
{
    const unsigned char *const ptr = (const unsigned char *) in_src;
    size_t                      i;

    for (i = in_max_len; (i > 0) && (ptr[ i - 1 ] == 0); --i) {}
    return (i);
}


int os_get_time(struct os_time *t)
{
    TickType_t  cur;

    if (!gSysBootTick) {
        gSysBootTick = xTaskGetTickCount();
    }

    cur = xTaskGetTickCount();

    t->sec = (cur - gSysBootTick) / 1000 / portTICK_PERIOD_MS;
    t->usec = ((cur - gSysBootTick) % 1000) / portTICK_PERIOD_MS * 1000;

    return (cur - gSysBootTick);
}

int os_get_random(unsigned char *buf, size_t len)
{
    int            idx = 0;
    unsigned long  rd = 0;
    unsigned char *pc = (unsigned char *)&rd;

    while (len) {
        rd = os_random();
        os_memcpy(buf + idx, pc, (len > sizeof(unsigned long)) ? sizeof(unsigned long) : len);
        idx += (len > sizeof(unsigned long)) ? sizeof(unsigned long) : len;
        len = (len > sizeof(unsigned long)) ? len - sizeof(unsigned long) : 0;
    }

    return 0;
}

void os_sleep(os_time_t sec, os_time_t usec)
{
    TickType_t xDelay;

    xDelay = sec * 1000 * portTICK_PERIOD_MS + usec * portTICK_PERIOD_MS / 1000;

    vTaskDelay(xDelay);
}

int os_mktime(int year, int month, int day, int hour, int min, int sec,
              os_time_t *t)
{
    return -1;
}


int os_gmtime(os_time_t t, struct os_tm *tm)
{
    return -1;
}

unsigned long os_random(void)
{
    TickType_t  cur;

    cur = xTaskGetTickCount();

    cur = cur * 1103515245 + 12345 ;

    /* for return type "unsigned long" */
    return (unsigned long)(cur / 131072);

    //return (unsigned long)(cur/131072) % 65536 ;
}

