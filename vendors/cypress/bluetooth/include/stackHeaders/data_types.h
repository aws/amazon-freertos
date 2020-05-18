/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#ifndef NULL
#define NULL     0
#endif

#ifndef FALSE
#define FALSE  0
#endif

#if defined( __IAR_SYSTEMS_ICC__ )
typedef __UINT8_T_TYPE__  UINT8;
typedef __UINT16_T_TYPE__ UINT16;
typedef __UINT32_T_TYPE__ UINT32;
typedef __UINT64_T_TYPE__ UINT64;
typedef __INT32_T_TYPE__  INT32;
typedef __INT8_T_TYPE__   INT8;
typedef __INT16_T_TYPE__  INT16;
#else
typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned long   UINT32;
typedef unsigned long long int UINT64;
typedef signed   long   INT32;
typedef signed   char   INT8;
typedef signed   short  INT16;
#endif /* defined( __IAR_SYSTEMS_ICC__ ) */

typedef unsigned int   BOOLEAN;
typedef UINT32          UINTPTR;
typedef UINT32          TIME_STAMP;


typedef UINT8           BOOL8;
typedef UINT32          BOOL32;

typedef unsigned long long __int64;

#ifndef TRUE
#define TRUE   (!FALSE)
#endif

typedef unsigned char   UBYTE;

#ifdef __arm
#define PACKED  __packed
#define INLINE  __inline
#else
#define PACKED
#ifndef INLINE
#define INLINE
#endif
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN FALSE
#endif

#define UINT16_LOW_BYTE(x)      ((x) & 0xff)
#define UINT16_HI_BYTE(x)       ((x) >> 8)

/* MACRO definitions for safe string functions */
/* Replace standard string functions with safe functions if available */
#define BCM_STRCAT_S(x1,x2,x3)      strcat((x1),(x3))
#define BCM_STRNCAT_S(x1,x2,x3,x4)  strncat((x1),(x3),(x4))
#define BCM_STRCPY_S(x1,x2,x3)      strcpy((x1),(x3))
#define BCM_STRNCPY_S(x1,x2,x3,x4)  strncpy((x1),(x3),(x4))
#define BCM_SPRINTF_S(x1,x2,x3,x4)  sprintf((x1),(x3),(x4))
#define BCM_VSPRINTF_S(x1,x2,x3,x4) vsprintf((x1),(x3),(x4))

#define BT_MEMCPY(a,b,c)            memcpy( (a), (b), (c) )
#define BT_MEMSET(a,b,c)            memset( (a), (b), (c) )
#define BT_STRLEN(a)                strlen( (a) )
#define BT_MEMCMP(a,b,c)            memcmp( (a), (b), (c) )
#define BT_MEMCMP_OK                0                       // memcmp success

/* Timer list entry callback type
*/
typedef void (TIMER_CBACK)(void *p_tle);
#ifndef TIMER_PARAM_TYPE
#define TIMER_PARAM_TYPE    UINT32
#endif

#endif

