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
 * @file type_def.h
 *
 *  TYPE definition.
 *
 */

#ifndef __TYPE_DEF_H__
#define __TYPE_DEF_H__

#include <stdint.h>
#include "memory_attribute.h"

#if (defined(__CC_ARM) || defined(__ICCARM__))
#ifndef SSIZE_MAX
typedef long int ssize_t;
#define SSIZE_MAX INT_MAX
#endif
#else
typedef unsigned int size_t;
#endif

typedef char                INT8;
typedef short               INT16;
typedef int                 INT32;

typedef long long           INT64;
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned short      UINT16LE;
typedef unsigned int        UINT32;
typedef unsigned int        UINT32LE;
typedef unsigned long long  UINT64;
typedef unsigned char       UCHAR;
typedef unsigned short      USHORT;
typedef unsigned int        UINT;
typedef unsigned long       ULONG;

typedef signed char         CHAR;
typedef signed short        SHORT;
typedef signed int          INT;
typedef signed long         LONG;
typedef signed long long    LONGLONG;

typedef unsigned long long  ULONGLONG;

/* Void Type */
typedef void                VOID;
typedef void                *PVOID;

typedef CHAR               	*PCHAR;
typedef UCHAR               *PUCHAR;
typedef unsigned char       *PUINT8;
typedef unsigned short      *PUINT16;
typedef unsigned int        *PUINT32;
typedef unsigned long long  *PUINT64;
typedef int                 *PINT32;
typedef long long           *PINT64;

typedef UINT8               BYTE;
typedef UINT16              WORD;
typedef UINT32            DWORD;

#ifndef NULL
#define NULL        (0)
#endif

#ifndef TRUE
#define TRUE        (1)
#endif

#ifndef FALSE
#define FALSE       (0)
#endif

/* MTK kal definitions */
typedef unsigned char           kal_uint8;
typedef signed char             kal_int8;
typedef char                    kal_char;
typedef unsigned short          kal_wchar;

typedef unsigned short int      kal_uint16;
typedef signed short int        kal_int16;

typedef unsigned int            kal_uint32;
typedef signed int              kal_int32;

typedef unsigned char           BOOLEAN;

typedef enum {
    KAL_FALSE	= 0,
    KAL_TRUE
} kal_bool;

typedef void (*kal_func_ptr)(void);

/*
 * Unsigned types
 */
typedef unsigned char u_int8;
typedef unsigned short u_int16;
typedef unsigned int u_int32;

/*
 * Signed types
 */
typedef signed char s_int8;
typedef signed short s_int16;
typedef signed int s_int32;

/*
 * Other types
 */
typedef u_int8 boolean;
typedef u_int32 t_clock;

#if defined(__ICCARM__)
#define TEXT_IN_RAM ATTR_TEXT_IN_TCM
#define BSS_IN_TCM ATTR_ZIDATA_IN_TCM
#else
#define TEXT_IN_RAM ATTR_TEXT_IN_TCM
#define BSS_IN_TCM ATTR_ZIDATA_IN_TCM
#endif


/*
 * MTK HW register definitions
 */

#define u_intHW             u_int32
#define SWAP_ENDIAN(Value)  SWAP_ENDIAN32(Value)

#define ADDR(Field)         ((u_intHW volatile *) (Field##_ADDR  ))
#define ADDR32(Field)       ((u_int32 *) (Field##_ADDR  ))
#define MASK(Field)         (Field##_MASK)
#define SHFT(Field)         (Field##_SHFT)

#define ADDR16(Field)       ((u_intHW volatile *) \
                             (Field##_ADDR + device_addr + ((Field##_SHFT) < 16 ? (0) : (2)) ) )
#define MASK16(Field) \
                            ((Field##_SHFT) < 16 ? (Field##_MASK) : ((Field##_MASK) >> 16) )
#define SHFT16(Field) \
	((Field##_SHFT) < 16 ? (Field##_SHFT) : ((Field##_SHFT) - 16 ) )

#define mGetHWEntry(Field) \
	( ( *ADDR(Field) & (MASK(Field)) ) >> SHFT(Field) )

#define mSetHWEntry(Field, Value) \
	{ u_intHW volatile *addr = ADDR(Field); \
		*addr = ((((u_intHW)(Value) << SHFT(Field)) \
		& MASK(Field)) | (*addr & ~MASK(Field))) ; }

#define mGetHWEntry32(Field) ( *ADDR(Field) )

#define mSetHWEntry32(Field, Source) ( *ADDR(Field) = Source )

#define mSetSWEntry(p_Data, Field, Value) \
	{ /*u_intHW volatile *addr = ADDR(Field)*/; \
		*p_Data = ((((u_intHW)(Value) << SHFT(Field)) \
		& MASK(Field)) | (*p_Data & ~MASK(Field))) ; }

#define DRV_WriteReg16(addr,data)     ((*(volatile kal_uint16 *)(addr)) = (kal_uint16)(data))
#define DRV_Reg16(addr)               (*(volatile kal_uint16 *)(addr))
#define DRV_WriteReg32(addr,data)     ((*(volatile kal_uint32 *)(addr)) = (kal_uint32)(data))
#define DRV_Reg32(addr)               (*(volatile kal_uint32 *)(addr))
#define DRV_WriteReg8(addr,data)     ((*(volatile kal_uint8 *)(addr)) = (kal_uint8)(data))
#define DRV_Reg8(addr)               (*(volatile kal_uint8 *)(addr))

#define HAL_REG_32(reg)         (*((volatile unsigned int *)(reg)))
#define REG32(x)		        (*(volatile unsigned int *)(x))
#define REG16(x)                (*(volatile unsigned short *)(x))
#define REG8(x)                 (*(volatile unsigned char *)(x))

#ifndef BIT
#define BIT(n)                  ((UINT32) 1 << (n))

/*
 * bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m,n)                       (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* BIT */

/* Endian byte swapping codes */
#define SWAP16(x) \
    ((uint16_t) (\
	       (((uint16_t) (x) & (uint16_t) 0x00ffU) << 8) | \
	       (((uint16_t) (x) & (uint16_t) 0xff00U) >> 8)))

#define SWAP32(x) \
    ((uint32_t) (\
	       (((uint32_t) (x) & (uint32_t) 0x000000ffUL) << 24) | \
	       (((uint32_t) (x) & (uint32_t) 0x0000ff00UL) << 8) | \
	       (((uint32_t) (x) & (uint32_t) 0x00ff0000UL) >> 8) | \
	       (((uint32_t) (x) & (uint32_t) 0xff000000UL) >> 24)))

#define SWAP64(x) \
    ((uint64_t)( \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x00000000000000ffULL) << 56) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x000000000000ff00ULL) << 40) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x0000000000ff0000ULL) << 24) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x00000000ff000000ULL) <<  8) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x000000ff00000000ULL) >>  8) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x0000ff0000000000ULL) >> 24) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0x00ff000000000000ULL) >> 40) | \
    (uint64_t)(((uint64_t)(x) & (uint64_t) 0xff00000000000000ULL) >> 56) ))

#ifdef RT_BIG_ENDIAN
#define cpu2le64(x) SWAP64((x))
#define le2cpu64(x) SWAP64((x))
#define cpu2le32(x) SWAP32((x))
#define le2cpu32(x) SWAP32((x))
#define cpu2le16(x) SWAP16((x))
#define le2cpu16(x) SWAP16((x))
#define cpu2be64(x) ((uint64_t)(x))
#define be2cpu64(x) ((uint64_t)(x))
#define cpu2be32(x) ((uint32_t)(x))
#define be2cpu32(x) ((uint32_t)(x))
#define cpu2be16(x) ((uint16_t)(x))
#define be2cpu16(x) ((uint16_t)(x))
#else /* Little_Endian */
#define cpu2le64(x) ((uint64_t)(x))
#define le2cpu64(x) ((uint64_t)(x))
#define cpu2le32(x) ((uint32_t)(x))
#define le2cpu32(x) ((uint32_t)(x))
#define cpu2le16(x) ((uint16_t)(x))
#define le2cpu16(x) ((uint16_t)(x))
#define cpu2be64(x) SWAP64((x))
#define be2cpu64(x) SWAP64((x))
#define cpu2be32(x) SWAP32((x))
#define be2cpu32(x) SWAP32((x))
#define cpu2be16(x) SWAP16((x))
#define be2cpu16(x) SWAP16((x))
#endif /* !RT_BIG_ENDIAN */

#endif /* __TYPE_DEF_H__ */

