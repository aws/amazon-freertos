
#ifndef CC_H_INCLUDED
#define CC_H_INCLUDED

#include <stdio.h>

/* Define platform endianness */
#define BYTE_ORDER LITTLE_ENDIAN
#include "wm_type_def.h"
#if 0 // remove by kevin 140612
/* The unsigned data types */
typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;

/* The signed counterparts */
typedef signed char s8_t;
typedef signed short s16_t;
typedef signed int s32_t;

/* A generic pointer type */
typedef u32_t mem_ptr_t;
#endif
/* Display name of types */
#define U16_F           "hu"
#define S16_F           "hd"
#define X16_F           "hx"
#define U32_F           "u"
#define S32_F           "d"
#define X32_F           "x"

/* Compiler hints for packing lwip's structures */
#if defined(__CC_ARM)
    /* Setup PACKing macros for MDK Tools */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((packed))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#elif defined (__ICCARM__)
    /* Setup PACKing macros for EWARM Tools */
#define PACK_STRUCT_BEGIN __packed
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#elif defined (__GNUC__)
    /* Setup PACKing macros for GCC Tools */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((packed))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#else
#error "This compiler does not support."
#endif

/* define LWIP_COMPAT_MUTEX
    to let sys.h use binary semaphores instead of mutexes - as before in 1.3.2
    Refer CHANGELOG
*/
#define  LWIP_COMPAT_MUTEX  1

#ifndef LWIP_PLATFORM_ASSERT
#define LWIP_PLATFORM_ASSERT(x) \
    do \
    {   printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); \
    } while(0)
#else
#define LWIP_PLATFORM_ASSERT(x)
#endif

#ifdef LWIP_PLATFORM_DIAG
#define LWIP_PLATFORM_DIAG(x) do { printf x;} while(0)
#else
#define LWIP_PLATFORM_DIAG(x) do { printf x;} while(0)
#endif

#define LWIP_PROVIDE_ERRNO /* Make lwip/arch.h define the codes which are used throughout */

#endif /* CC_H_INCLUDED */
