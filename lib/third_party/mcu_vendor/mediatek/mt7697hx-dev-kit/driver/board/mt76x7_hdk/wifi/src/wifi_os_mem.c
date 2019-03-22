/* Copyright Statement:
 *
 * (C) 2018-2018  MediaTek Inc. All rights reserved.
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
 * @file This file contains 3 versions of memory-management APIs for WPA
 *       Supplicant and Wi-Fi's connsys driver.
 *
 * The first one is a very simple memory allocation algorithm, enclosed by
 * the definition of MTK_WIFI_OS_ISOLATED_MM_ENABLE.
 *
 * The second one is a wrapper only API layer, it augments heap_4.c of
 * FreeRTOS with realloc API. Enclosed by the definition of
 * MTK_FREERTOS_MM_ENABLE.
 *
 * The third one is the second version with debug capability, enclosed by
 * MTK_FREERTOS_MM_DEBUG_ENABLE.
 *
 * Here is more explanation about the first memory-management algorithm:
 *
 * The required memory of WPA Supplicant and connsys is dynamically allocated
 * mostly. The advantage of this approach is apparently:
 *
 * 1. Memory can be used by requirements from different phases.
 * 2. Not need to predetermine the needed memory.
 *
 * The issues that came along with the approach above:
 *
 * 1. The needed memory is not known.
 * 2. The actual consumed memory size is memory-management-algorithm
 *    dependent. This fails memory-leak detectors in API unit tests.
 * 3. The memory may allocate fail, and the behavior is often undefined at
 *    design phase.
 *
 * To address issues 1 and 2, this algorithm is implemented to let developer
 * able to replace memory-management algorithm for WPA Supplicant and connsys
 * at compile time.
 *
 * Issue 3 is quality controlled by SQA.
 *
 * USAGE:
 *
 *      Use this algorithm by defining MTK_WIFI_OS_ISOLATED_MM_ENABLE.
 *
 *      *** DO NOT DEFINE IT IF YOU DO NOT KNOW WHAT YOU ARE DOING ***
 */


#include <assert.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef UT
    #include "FreeRTOS.h"
    #include "os.h"
#else
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
#endif


/****************************************************************************
 *
 * DEBUG
 *
 ****************************************************************************/


#ifdef MTK_WIFI_OS_ISOLATED_MM_ENABLE

    #if 0
        #define should_not_reach assert
    #else
        void should_not_reach(int n)
        {
            if (n)
                return;

            printf("ASSERT: wifi_os_mem.c\n");

            while (1)
                ;
        }
    #endif

#endif


/****************************************************************************
 *
 * CONSTANT
 *
 ****************************************************************************/


#ifdef MTK_WIFI_OS_ISOLATED_MM_ENABLE

    #define BLOCK_128_COUNT     (32)
    #define BLOCK_128_SIZE      (128  * BLOCK_128_COUNT)   // 4k
    #define BLOCK_256_COUNT     (10)
    #define BLOCK_256_SIZE      (256  * BLOCK_256_COUNT)   // 2.5k
    #define BLOCK_512_COUNT     (48)
    #define BLOCK_512_SIZE      (512  * BLOCK_512_COUNT)   // 24k
    #define BLOCK_1024_COUNT    (6)
    #define BLOCK_1024_SIZE     (1024 * BLOCK_1024_COUNT)  // 6k
    #define BLOCK_2048_COUNT    (3)
    #define BLOCK_2048_SIZE     (2048 * BLOCK_2048_COUNT)  // 6k

    #define TOTAL_COUNT         (BLOCK_128_COUNT  + BLOCK_256_COUNT  + \
                                 BLOCK_512_COUNT  + BLOCK_1024_COUNT + \
                                 BLOCK_2048_COUNT )


    #define TOTAL_SIZE          (BLOCK_128_SIZE   + BLOCK_256_SIZE  + \
                                 BLOCK_512_SIZE   + BLOCK_1024_SIZE + \
                                 BLOCK_2048_SIZE )

    #define BLOCK_128_ADDR      ((char *)pool.p)
    #define BLOCK_128_USED      (0)

    #define BLOCK_256_ADDR      (BLOCK_128_ADDR + BLOCK_128_SIZE)
    #define BLOCK_256_USED      (BLOCK_128_COUNT)

    #define BLOCK_512_ADDR      (BLOCK_256_ADDR  + BLOCK_256_SIZE)
    #define BLOCK_512_USED      (BLOCK_128_COUNT + BLOCK_256_COUNT)

    #define BLOCK_1024_ADDR     (BLOCK_512_ADDR  + BLOCK_512_SIZE)
    #define BLOCK_1024_USED     (BLOCK_512_USED  + BLOCK_512_COUNT)

    #define BLOCK_2048_ADDR     (BLOCK_1024_ADDR + BLOCK_1024_SIZE)
    #define BLOCK_2048_USED     (BLOCK_1024_USED + BLOCK_1024_COUNT)

    #define IS_128_BLOCK(ptr)   ((char *)ptr < BLOCK_256_ADDR)
    #define IS_256_BLOCK(ptr)   ((char *)ptr < BLOCK_512_ADDR)
    #define IS_512_BLOCK(ptr)   ((char *)ptr < BLOCK_1024_ADDR)
    #define IS_1024_BLOCK(ptr)  ((char *)ptr < BLOCK_2048_ADDR)
    #define IS_2048_BLOCK(ptr)  ((char *)ptr < ((char *)pool.p + TOTAL_SIZE))

#endif


#ifdef MTK_FREERTOS_MM_DEBUG_ENABLE

    #define MAX_RECS    (100)

#endif


/****************************************************************************
 *
 * DATA STRUCTURE
 *
 ****************************************************************************/


#ifdef MTK_WIFI_OS_ISOLATED_MM_ENABLE

    typedef struct pool_s
    {
        void    *p;
        bool    used[ TOTAL_COUNT ];
    } pool_t;

#endif


#ifdef MTK_FREERTOS_MM_DEBUG_ENABLE

    ssize_t  size_requested;

    typedef struct _mem_rec
    {
        void        *addr;
        const char  *file;
        int         line;
        size_t      size;
    } mem_rec;

#endif /* MTK_FREERTOS_MM_DEBUG_ENABLE */



/****************************************************************************
 *
 * LOCAL VARIABLE
 *
 ****************************************************************************/


#ifdef MTK_WIFI_OS_ISOLATED_MM_ENABLE

    static pool_t   pool;

    static char     watermark_128;
    static char     watermark_256;
    static char     watermark_512;
    static char     watermark_1024;
    static char     watermark_2048;

    static char     waterlevel_128;
    static char     waterlevel_256;
    static char     waterlevel_512;
    static char     waterlevel_1024;
    static char     waterlevel_2048;

#endif


#ifdef MTK_FREERTOS_MM_DEBUG_ENABLE

    int             os_malloc_d_flag;
    static mem_rec  rec[ MAX_RECS ];

#endif /* MTK_FREERTOS_MM_DEBUG_ENABLE */



/****************************************************************************
 *
 * LOCAL FUNCTIONS
 *
 ****************************************************************************/


#ifdef MTK_WIFI_OS_ISOLATED_MM_ENABLE

    static void _wifi_os_mem_init(void)
    {
        if (pool.p)
            return;

    #ifdef UT
        pool.p = malloc(TOTAL_SIZE);
    #else
        static char _wifi_os_mem_heap[TOTAL_SIZE] __attribute__ ((aligned (4)));
        pool.p = &_wifi_os_mem_heap[0];
    #endif
    }


    static int ptr_to_index(void *ptr)
    {
        if ((char *)ptr < (char *)pool.p || (char *)ptr >= ((char *)pool.p + TOTAL_SIZE))
            return -1;

    #define PTR_TO_INDEX_HANDLE(num)                                                  \
        do {                                                                          \
            if (IS_ ## num ## _BLOCK(((char *)ptr))) {                                          \
                if ((((char *)ptr) - BLOCK_ ## num ## _ADDR) % num)                             \
                    return -1;                                                        \
                return (((char *)ptr) - BLOCK_ ## num ## _ADDR) / num + BLOCK_ ## num ## _USED; \
            }                                                                         \
        } while (0)

        PTR_TO_INDEX_HANDLE(128);
        PTR_TO_INDEX_HANDLE(256);
        PTR_TO_INDEX_HANDLE(512);
        PTR_TO_INDEX_HANDLE(1024);
        PTR_TO_INDEX_HANDLE(2048);

        return -1;
    }

#endif


#ifdef MTK_FREERTOS_MM_DEBUG_ENABLE

    static void rec_add(void *addr, const char *file, int line, size_t size)
    {
        if (!addr) return;

        size_requested += size;

        for (int i = 0; i < MAX_RECS; i++) {
            if (rec[i].addr == 0) {
                rec[i].addr = addr;
                rec[i].file = file;
                rec[i].line = line;
                rec[i].size = size;
                return;
            } if (rec[i].addr == addr) {
                printf("! free not found: %s %u\n", rec[i].file, rec[i].line);
            }
        }

        printf("table full\n");

        for (int i = 0; i < MAX_RECS; i++)
            printf("%s %u\n", rec[i].file, rec[i].line);
    }

    static void rec_del(void *addr, const char *file, int line)
    {
        if (!addr) return;

        for (int i = 0; i < MAX_RECS; i++) {
            if (rec[i].addr == addr) {
                rec[i].addr = 0;
                size_requested -= rec[i].size;
                return;
            }
        }

        //printf("! malloc not found: %s %u\n", file ? file : "n/a", line);
    }

    static const char *basename(const char *path)
    {
        if (path)
        {
            while (*path != '\0')
                path++;
            while (*path != '/')
                path--;
            path++;
        }
        else
            path = "n/a";

        return path;
    }

#endif


/****************************************************************************
 *
 * PUBLIC FUNCTIONS
 *
 ****************************************************************************/


#ifdef MTK_WIFI_OS_ISOLATED_MM_ENABLE

    void *wifi_os_mem_malloc(size_t size)
    {
        _wifi_os_mem_init();

        if (size == 0)
            return NULL;

    #define OS_MALLOC_HANDLE(sz)                                           \
        do {                                                               \
            if (size <= sz)                                                \
            {                                                              \
                int i;                                                     \
                for (i =  BLOCK_ ## sz ## _USED;                           \
                     i < (BLOCK_ ## sz ## _USED + BLOCK_ ## sz ## _COUNT); \
                     i ++)                                                 \
                    if (!pool.used[i]) {                                   \
                        pool.used[i] = true;                               \
                        waterlevel_ ## sz ++;                              \
                        if (waterlevel_ ## sz > watermark_ ## sz)          \
                            watermark_ ## sz = waterlevel_ ## sz;          \
                        return BLOCK_ ## sz ## _ADDR +                     \
                               (i - BLOCK_ ## sz ## _USED) * sz;           \
                    }                                                      \
                should_not_reach(0);                                       \
            }                                                              \
        } while (0)

        OS_MALLOC_HANDLE(128);
        OS_MALLOC_HANDLE(256);
        OS_MALLOC_HANDLE(512);
        OS_MALLOC_HANDLE(1024);
        OS_MALLOC_HANDLE(2048);

        should_not_reach(0);

        return NULL;
    }

    void wifi_os_mem_free(void *ptr)
    {
        int index;

        _wifi_os_mem_init();

        if (!ptr)
            return;

        index = ptr_to_index(ptr);

        should_not_reach(index >= 0);

        if (IS_128_BLOCK(ptr))  waterlevel_128 --; else
        if (IS_256_BLOCK(ptr))  waterlevel_256 --; else
        if (IS_512_BLOCK(ptr))  waterlevel_512 --; else
        if (IS_1024_BLOCK(ptr)) waterlevel_1024--; else
        if (IS_2048_BLOCK(ptr)) waterlevel_2048--;

        pool.used[index] = false;
    }

    void *wifi_os_mem_realloc(void *ptr, size_t size)
    {
        int     index;
        void    *rtp;
        size_t  ezis;

        _wifi_os_mem_init();

        if (!ptr)
        {
            rtp = wifi_os_mem_malloc(size);
        }
        else
        {
            index = ptr_to_index(ptr);

            should_not_reach(index >= 0);

            if (IS_128_BLOCK(ptr))  ezis =  128; else
            if (IS_256_BLOCK(ptr))  ezis =  256; else
            if (IS_512_BLOCK(ptr))  ezis =  512; else
            if (IS_1024_BLOCK(ptr)) ezis = 1024; else
            if (IS_2048_BLOCK(ptr)) ezis = 2048; else
                                    ezis =    0; // ptr is NULL

            rtp = wifi_os_mem_malloc(size);

            os_memcpy(rtp, ptr, size > ezis ? ezis : size);

            wifi_os_mem_free(ptr);
        }

        return rtp;
    }

    void *wifi_os_mem_zalloc(size_t size)
    {
        void *p = wifi_os_mem_malloc(size);

        if (p)
            os_memset(p, 0, size);

        return p;
    }

    char *wifi_os_mem_strdup(const char *s)
    {
        _wifi_os_mem_init();

        size_t len = os_strlen(s) + 1;
        char   *t = wifi_os_mem_malloc(len);

        os_memcpy(t, s, len);

        return t;
    }

    void wifi_os_mem_dump(void)
    {
        printf("used: ");

        /* dump the occupation flags */
        for (int i = 0; i < TOTAL_COUNT; i++) {
            printf("%d%s", pool.used[i], ((i % 8) == 7) ? " " : "");
        }

        printf("128 maximum %d\n",  watermark_128);
        printf("256 maximum %d\n",  watermark_256);
        printf("512 maximum %d\n",  watermark_512);
        printf("1024 maximum %d\n", watermark_1024);
        printf("2048 maximum %d\n", watermark_2048);
    }

#endif /* ifdef MTK_WIFI_OS_ISOLATED_MM_ENABLE */


#ifdef MTK_FREERTOS_MM_ENABLE

    void *os_mem_malloc(size_t size)
    {
        void *p = pvPortMalloc(size);

        configASSERT(p);

        return p;
    }

    void os_mem_free(void *ptr)
    {
        vPortFree(ptr);
    }

    void *os_mem_realloc(void *ptr, size_t size)
    {
        extern void *pvPortRealloc( void *pv, size_t xWantedSize );
        return pvPortRealloc(ptr, size);
    }

    void *os_mem_zalloc(size_t size)
    {
        void *p = os_malloc(size);

        configASSERT(p);

        if (p)
            os_memset(p, 0, size);

        return p;
    }

    char *os_mem_strdup(const char *s)
    {
        int  l;
        char *r;

        if (s == NULL) {
            return NULL;
        }

        l = os_strlen(s);
        r = os_malloc(l + 1);

        if (r != NULL) {
            os_memcpy(r, s, l);
            r[l] = '\0';
        }

        return r;
    }

#endif


#ifdef MTK_FREERTOS_MM_DEBUG_ENABLE

    void *os_mem_d_malloc(const char *file, const int line, size_t size)
    {
        void *p = pvPortMalloc(size);

        if (os_malloc_d_flag & 1)
            printf("+ %x %x %s %u\n", (unsigned int)p,
                                      (unsigned int)size,
                                      basename(file),
                                      line);
        if (os_malloc_d_flag & 2)
            rec_add(p, file, line, size);

        return p;
    }

    void os_mem_d_free(const char *file, const int line, void *ptr)
    {
        if (os_malloc_d_flag & 1)
            printf("- %x %s %u\n", (unsigned int)ptr,
                                   basename(file),
                                   line);
        if (os_malloc_d_flag & 2)
            rec_del(ptr, file, line);

        vPortFree(ptr);
    }

    void *os_mem_d_realloc(const char *file, const int line, void *ptr, size_t size)
    {
        extern void *pvPortRealloc( void *pv, size_t xWantedSize );
        void *new_ptr = pvPortRealloc(ptr, size);

        if (os_malloc_d_flag & 1)
            printf("+ %x => %x %u %s %u\n", (unsigned int)ptr,
                                            (unsigned int)new_ptr,
                                            (unsigned int)size,
                                            basename(file),
                                            line);

        if (os_malloc_d_flag & 2) {
            rec_del(ptr, NULL, 0);
            rec_add(new_ptr, file, line, size);
        }

        return new_ptr;
    }

    void *os_mem_d_zalloc(const char *file, const int line, size_t size)
    {
        void *p = malloc(size);

        if (os_malloc_d_flag & 1)
            printf("+ %x %x %s %u\n", (unsigned int)p,
                                      (unsigned int)size,
                                      basename(file),
                                      line);
        if (os_malloc_d_flag & 2)
            rec_add(p, file, line, size);

        if (p)
            os_memset(p, 0, size);

        return p;
    }

    char *os_mem_d_strdup(const char *s)
    {
        int  l;
        char *r;

        if (s == NULL) {
            return NULL;
        }

        l = os_strlen(s);
        r = os_malloc(l + 1);

        if (r != NULL) {
            os_memcpy(r, s, l);
            r[l] = '\0';
        }

        return r;
    }

    void os_mem_d_dump(void)
    {
        for (int i = 0; i < MAX_RECS; i++) {
            if (!rec[i].addr)
                continue;
            printf("~ %s %u %u\n", rec[i].file, rec[i].line, rec[i].size);
        }

        printf("~ size_requested %d\n", size_requested);
    }

#endif /* MTK_FREERTOS_MM_DEBUG_ENABLE */


/****************************************************************************
 *
 * UNIT TEST FUNCTIONS
 *
 ****************************************************************************/


#ifdef MTK_WIFI_OS_ISOLATED_MM_ENABLE

    #ifdef UT
        void dump_used(void)
        {
            printf("used: ");

            for (int i = 0; i < TOTAL_COUNT; i++) {
                printf("%d%s", pool.used[i], ((i % 8) == 7) ? " " : "");
            }

            printf("\n");
        }

        void use_128(void)
        {
            void *ptrs[BLOCK_128_COUNT];

            for (int i = 0; i < BLOCK_128_COUNT; i++)
                ptrs[i] = wifi_os_mem_malloc(128);

            dump_used();

            for (int i = 0; i < BLOCK_128_COUNT; i++)
                wifi_os_mem_free(ptrs[i]);

            dump_used();
        }

        void out_128(void)
        {
            for (int i = 0; i <= BLOCK_128_COUNT; i++)
                printf("ptr %x\n", wifi_os_mem_malloc(128));
            dump_used();
        }

        void full(void)
        {
            for (int i = 0; i < BLOCK_128_COUNT; i++)
                wifi_os_mem_malloc(128);
            for (int i = 0; i < BLOCK_256_COUNT; i++)
                wifi_os_mem_malloc(256);
            for (int i = 0; i < BLOCK_512_COUNT; i++)
                wifi_os_mem_malloc(512);
            for (int i = 0; i < BLOCK_1024_COUNT; i++)
                wifi_os_mem_malloc(1024);
            for (int i = 0; i < BLOCK_2048_COUNT; i++)
                wifi_os_mem_malloc(2048);
            dump_used();
        }

        void all_size(void)
        {
            void *ptr1 = NULL;
            void *ptr2 = NULL;

            for (int i = 1; i < 2048; i++)
            {
                ptr2 = wifi_os_mem_malloc(i);

                if (ptr1 != ptr2) {
                    printf("allocate %d bytes ptr = %x\n", i, ptr2);
                    dump_used();
                    ptr1 = ptr2;
                }

                wifi_os_mem_free(ptr2);
            }
        }

        typedef struct test_case_s
        {
            char *name;
            void (*func)(void);
        } test_case_t;

        static test_case_t cases[] =
        {
            { "use_128", use_128 },
            { "out_128", out_128 },
            { "full", full },
            { "all_size", all_size },
        };
        #define num_cases   (sizeof(cases) / sizeof(test_case_t))

        int main(int argc, char *argv[])
        {
            while (--argc > 0)
            {
                argv++;

                for (int i = 0; i < num_cases; i++)
                    if (!strcmp(argv[0], cases[i].name))
                        cases[i].func();
            }
        }
    #endif

#endif /* ifdef MTK_WIFI_OS_ISOLATED_MM_ENABLE */

