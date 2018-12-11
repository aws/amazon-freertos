/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_compiler.h
* Description  : Replace different functions for each compiler.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : xx.xx.xxxx 1.00     First Release
***********************************************************************************************************************/

#ifndef R_GCOMPILER_H
#define R_GCOMPILER_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/
/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/

/* ========== Check Compiler ========== */
#if defined(__CCRX__)
    /* supported */
#elif defined(__GNUC__)
    /* supported */
#elif defined(__ICCRX__)
    /* supported */
#else
    #error "Unrecognized compiler"
#endif


/* ========== Macros ========== */
#if defined(__CCRX__)

/* #define __RX 1 */ /* This is already defined by CCRX. */
/* #define __LIT 1 */ /* This is automatically defined by CCRX. */
/* #define __BIG 1 */ /* This is automatically defined by CCRX. */
/* #define __FPU 1 */ /* This is automatically defined by CCRX. */
/* #define __RXV2 1 */ /* This is automatically defined by CCRX. */

#elif defined(__GNUC__)

#if !defined(__RX)
#define __RX 1
#endif

#if defined(__RX_LITTLE_ENDIAN__)
#if !defined(__LIT)
#define __LIT 1
#endif
#elif defined(__RX_BIG_ENDIAN__)
#if !defined(__BIG)
#define __BIG 1
#endif
#endif

#if defined(__RX_FPU_INSNS__)
#if !defined(__FPU)
#define __FPU 1
#endif
#endif

#if defined(__RXv2__)
#if !defined(__RXV2)
#define __RXV2 1
#endif
#endif

#elif defined(__ICCRX__)

#if !defined(__RX)
#define __RX 1
#endif

/* #define __LIT 1 */ /* This is automatically defined by ICCRX. */
/* #define __BIG 1 */ /* This is automatically defined by ICCRX. */
/* #define __FPU 1 */ /* This is automatically defined by ICCRX. */
/* #define __RXV2 1 */ /* This is automatically defined by ICCRX. */

#endif


/* ========== Keywords ========== */
#if !(defined(__CCRX__) && defined(__cplusplus))
#define R_PRAGMA(...)    _Pragma(#__VA_ARGS__)
#else
/* CC-RX' C++ mode does not support Pragma operator and variadic macros */
#define R_PRAGMA(x)
#endif

#if defined(__CCRX__)

/* #define volatile        volatile __evenaccess */

#elif defined(__GNUC__)

#define __evenaccess    /* none */

#elif defined(__ICCRX__)

#define __evenaccess    __sfr

#endif


/* ========== Sections ========== */

/* ---------- Operators ---------- */
#if defined(__CCRX__)

#define R_SECTOP(name)        __sectop(#name)
#define R_SECEND(name)        __secend(#name)
#define R_SECSIZE(name)       __secsize(#name)

#define R_EXTERN_SEC(name)    /* none */

#elif defined(__GNUC__)

#define R_SECTOP(name)        ((void *)name##_start)
#define R_SECEND(name)        ((void *)name##_end)
#define R_SECSIZE(name)       ((size_t)((uint8_t *)R_SECEND(name) - (uint8_t *)R_SECTOP(name)))

#define R_EXTERN_SEC(name)    extern uint8_t name##_start[], name##_end[];

#elif defined(__ICCRX__)

#define R_SECTOP(name)        __section_begin(#name)
#define R_SECEND(name)        __section_end(#name)
#define R_SECSIZE(name)       __section_size(#name)

#define R_EXTERN_SEC(name)    /* none */

#endif

/* ---------- Names ---------- */
#if defined(__CCRX__)

#define R_SECNAME_INTVECTTBL       "C$VECT"
#if defined(__RXV2)
#define R_SECNAME_EXCEPTVECTTBL    "EXCEPTVECT"
#define R_SECNAME_RESETVECT        "RESETVECT"
#else
#define R_SECNAME_FIXEDVECTTBL     "FIXEDVECT"
#endif
#define R_SECNAME_UBSETTINGS       "UBSETTINGS"

#elif defined(__GNUC__)

#define R_SECNAME_INTVECTTBL       ".rvectors"
#if defined(__RXV2)
#define R_SECNAME_EXCEPTVECTTBL    ".exvectors"
#define R_SECNAME_RESETVECT        ".fvectors"
#else
#define R_SECNAME_FIXEDVECTTBL     ".fvectors"
#endif
#define R_SECNAME_UBSETTINGS       ".ubsettings"

#elif defined(__ICCRX__)

#define R_SECNAME_INTVECTTBL       ".inttable"
#if defined(__RXV2)
#define R_SECNAME_EXCEPTVECTTBL    ".nmivec"
#define R_SECNAME_RESETVECT        //FIXME: Is the section defined? If so, what is the name?
#else
#define R_SECNAME_FIXEDVECTTBL     ".nmivec"
#endif
#define R_SECNAME_UBSETTINGS       ".ubsettings"

#endif

/* ---------- Addresses ---------- */
#if defined(__CCRX__)

#define R_SECTOP_INTVECTTBL       __sectop(R_SECNAME_INTVECTTBL)
#if defined(__RXV2)
#define R_SECTOP_EXCEPTVECTTBL    __sectop(R_SECNAME_EXCEPTVECTTBL)
#endif

#define R_SECEND_ISTACK           R_SECEND(SI)
#define R_SECEND_USTACK           R_SECEND(SU)

#elif defined(__GNUC__)

#define R_SECTOP_INTVECTTBL       ((void *)rvectors_start)
extern void * const               rvectors_start[];
#if defined(__RXV2)
#if 1 /* for backward compatibility */
#define R_SECTOP_EXCEPTVECTTBL    ((void *)Except_Vectors)
extern void * const               Except_Vectors[];
#else
#define R_SECTOP_EXCEPTVECTTBL    ((void *)exvectors_start)
extern void * const               exvectors_start[];
#endif
#endif

#define R_SECEND_ISTACK           istack
extern uint8_t                    istack[]; /* This symbol means the end address of the istack_area[] */
#define R_SECEND_USTACK           ustack
extern uint8_t                    ustack[]; /* This symbol means the end address of the ustack_area[] */

#elif defined(__ICCRX__)

#define R_SECTOP_INTVECTTBL       __section_begin(R_SECNAME_INTVECTTBL)
#if defined(__RXV2)
#define R_SECTOP_EXCEPTVECTTBL    __section_begin(R_SECNAME_EXCEPTVECTTBL)
#endif

#define R_SECEND_ISTACK           R_SECEND(ISTACK)
#define R_SECEND_USTACK           R_SECEND(USTACK)

#endif


/* ========== #pragma Directive ========== */

/* ---------- Stack Size ---------- */
#if defined(__CCRX__)

#define R_PRAGMA_STACKSIZE_SI(_size)     _R_PRAGMA_STACKSIZE_SI(_size) /* _size means '(size)' */
#define _R_PRAGMA_STACKSIZE_SI(_size)    __R_PRAGMA_STACKSIZE_SI##_size
#define __R_PRAGMA_STACKSIZE_SI(size)     R_PRAGMA(stacksize si=size)
#define R_PRAGMA_STACKSIZE_SU(_size)     _R_PRAGMA_STACKSIZE_SU(_size) /* _size means '(size)' */
#define _R_PRAGMA_STACKSIZE_SU(_size)    __R_PRAGMA_STACKSIZE_SU##_size
#define __R_PRAGMA_STACKSIZE_SU(size)    R_PRAGMA(stacksize su=size)

#elif defined(__GNUC__)

#define R_PRAGMA_STACKSIZE_SI(size)      static uint8_t istack_area[size] __attribute__((section(".r_bsp_istack"), used));
#define R_PRAGMA_STACKSIZE_SU(size)      static uint8_t ustack_area[size] __attribute__((section(".r_bsp_ustack"), used));

#elif defined(__ICCRX__)

//#define R_PRAGMA_STACKSIZE_SI(size)    FIXME: Is there any way? If so, what to do?
//#define R_PRAGMA_STACKSIZE_SU(size)    FIXME: Is there any way? If so, what to do?

#endif

/* ---------- Section Switch (part1) ---------- */
#if defined(__CCRX__)

#define R_ATTRIB_SECTION_CHANGE_UBSETTINGS                R_PRAGMA(section C UBSETTINGS)
#if defined(__RXV2)
#define R_ATTRIB_SECTION_CHANGE_EXCEPTVECT                R_PRAGMA(section C EXCEPTVECT)
#define R_ATTRIB_SECTION_CHANGE_RESETVECT                 R_PRAGMA(section C RESETVECT)
#else
#define R_ATTRIB_SECTION_CHANGE_FIXEDVECT                 R_PRAGMA(section C FIXEDVECT)
#endif

#elif defined(__GNUC__)

#define R_ATTRIB_SECTION_CHANGE_UBSETTINGS                __attribute__((section(R_SECNAME_UBSETTINGS)))
#if defined (__RXV2)
#define R_ATTRIB_SECTION_CHANGE_EXCEPTVECT                __attribute__((section(R_SECNAME_EXCEPTVECTTBL)))
#define R_ATTRIB_SECTION_CHANGE_RESETVECT                 __attribute__((section(R_SECNAME_RESETVECT)))
#else
#define R_ATTRIB_SECTION_CHANGE_FIXEDVECT                 __attribute__((section(R_SECNAME_FIXEDVECTTBL)))
#endif

#elif defined(__ICCRX__)

#define R_ATTRIB_SECTION_CHANGE_UBSETTINGS                R_PRAGMA(location=R_SECNAME_UBSETTINGS)

#endif

/* ---------- Section Switch (part2) ---------- */
#if defined(__CCRX__)

#define __R_ATTRIB_SECTION_CHANGE_V(type, section_name)    R_PRAGMA(section type section_name)
#define __R_ATTRIB_SECTION_CHANGE_F(type, section_name)    R_PRAGMA(section type section_name)

#define _R_ATTRIB_SECTION_CHANGE_B1(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(B, B##section_tag) /* The CC-RX adds postfix '_1' automatically */
#define _R_ATTRIB_SECTION_CHANGE_B2(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(B, B##section_tag) /* The CC-RX adds postfix '_2' automatically */
#define _R_ATTRIB_SECTION_CHANGE_B4(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(B, B##section_tag) /* The CC-RX does not add postfix '_4' */
#define _R_ATTRIB_SECTION_CHANGE_C1(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(C, C##section_tag) /* The CC-RX adds postfix '_1' automatically */
#define _R_ATTRIB_SECTION_CHANGE_C2(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(C, C##section_tag) /* The CC-RX adds postfix '_2' automatically */
#define _R_ATTRIB_SECTION_CHANGE_C4(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(C, C##section_tag) /* The CC-RX does not add postfix '_4' */
#define _R_ATTRIB_SECTION_CHANGE_P(section_tag)            __R_ATTRIB_SECTION_CHANGE_F(P, P##section_tag)

#if !defined(__cplusplus)
#define R_ATTRIB_SECTION_CHANGE(type, section_tag, ...)    _R_ATTRIB_SECTION_CHANGE_##type##__VA_ARGS__(section_tag)
#else
/* CC-RX' C++ mode does not support variadic macros */
#endif
#define R_ATTRIB_SECTION_CHANGE_END                        R_PRAGMA(section)

#elif defined(__GNUC__)

#define __R_ATTRIB_SECTION_CHANGE_V(section_name)          __attribute__((section(#section_name)))
#define __R_ATTRIB_SECTION_CHANGE_F(section_name)          __attribute__((section(#section_name)))

#define _R_ATTRIB_SECTION_CHANGE_B1(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(B##section_tag##_1)
#define _R_ATTRIB_SECTION_CHANGE_B2(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(B##section_tag##_2)
#define _R_ATTRIB_SECTION_CHANGE_B4(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(B##section_tag) /* No postfix '_4' because the CC-RX does not add it */
#define _R_ATTRIB_SECTION_CHANGE_C1(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(C##section_tag##_1)
#define _R_ATTRIB_SECTION_CHANGE_C2(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(C##section_tag##_2)
#define _R_ATTRIB_SECTION_CHANGE_C4(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(C##section_tag) /* No postfix '_4' because the CC-RX does not add it */
#define _R_ATTRIB_SECTION_CHANGE_P(section_tag)            __R_ATTRIB_SECTION_CHANGE_F(P##section_tag)

#define R_ATTRIB_SECTION_CHANGE(type, section_tag, ...)    _R_ATTRIB_SECTION_CHANGE_##type##__VA_ARGS__(section_tag)
#define R_ATTRIB_SECTION_CHANGE_END                        /* none */

#elif defined(__ICCRX__)

#define __R_ATTRIB_SECTION_CHANGE_V(section_name)          R_PRAGMA(location=#section_name)\
                                                           __no_init
#define __R_ATTRIB_SECTION_CHANGE_F(section_name)          R_PRAGMA(location=#section_name)

#define _R_ATTRIB_SECTION_CHANGE_B1(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(B##section_tag##_1)
#define _R_ATTRIB_SECTION_CHANGE_B2(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(B##section_tag##_2)
#define _R_ATTRIB_SECTION_CHANGE_B4(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(B##section_tag) /* No postfix '_4' because the CC-RX does not add it */
#define _R_ATTRIB_SECTION_CHANGE_P(section_tag)            __R_ATTRIB_SECTION_CHANGE_F(P##section_tag)

#define R_ATTRIB_SECTION_CHANGE(type, section_tag, ...)    _R_ATTRIB_SECTION_CHANGE_##type##__VA_ARGS__(section_tag)
#define R_ATTRIB_SECTION_CHANGE_END                        /* none */

#endif

/* ---------- Interrupt Function Creation ---------- */
#if defined(__CCRX__)

#define R_PRAGMA_INTERRUPT(function_name, vector)            R_PRAGMA(interrupt function_name(vect=vector))\
                                                             extern void function_name(void);
#define R_PRAGMA_STATIC_INTERRUPT(function_name, vector)     R_PRAGMA(interrupt function_name(vect=vector))\
                                                             static void function_name(void);

#define R_PRAGMA_INTERRUPT_DEFAULT(function_name)            R_PRAGMA(interrupt function_name)\
                                                             extern void function_name(void);
#define R_PRAGMA_STATIC_INTERRUPT_DEFAULT(function_name)     R_PRAGMA(interrupt function_name)\
                                                             static void function_name(void);

#define R_PRAGMA_INTERRUPT_FUNCTION(function_name)           R_PRAGMA(interrupt function_name)\
                                                             extern void function_name(void);
#define R_PRAGMA_STATIC_INTERRUPT_FUNCTION(function_name)    R_PRAGMA(interrupt function_name)\
                                                             static void function_name(void);

#define R_PRAGMA_FAST_INTERRUPT(function_name)               R_PRAGMA(interrupt function_name(fast))\
                                                             extern void function_name(void);
#define R_PRAGMA_STATIC_FAST_INTERRUPT(function_name)        R_PRAGMA(interrupt function_name(fast))\
                                                             static void function_name(void);

#define R_ATTRIB_INTERRUPT                                   extern /* only this one because of no corresponding keyword */
#define R_ATTRIB_STATIC_INTERRUPT                            static /* only this one because of no corresponding keyword */

#define R_ATTRIB_FAST_INTERRUPT                              extern /* only this one because of no corresponding keyword */
#define R_ATTRIB_STATIC_FAST_INTERRUPT                       static /* only this one because of no corresponding keyword */

#elif defined(__GNUC__)

#define R_PRAGMA_INTERRUPT(function_name, vector)            extern void function_name(void) __attribute__((interrupt(R_SECNAME_INTVECTTBL, vector)));
#define R_PRAGMA_STATIC_INTERRUPT(function_name, vector)     static void function_name(void) __attribute__((interrupt(R_SECNAME_INTVECTTBL, vector), used));

#define R_PRAGMA_INTERRUPT_DEFAULT(function_name)            extern void function_name(void) __attribute__((interrupt(R_SECNAME_INTVECTTBL, "$default")));
#define R_PRAGMA_STATIC_INTERRUPT_DEFAULT(function_name)     static void function_name(void) __attribute__((interrupt(R_SECNAME_INTVECTTBL, "$default"), used));

#define R_PRAGMA_INTERRUPT_FUNCTION(function_name)           extern void function_name(void) __attribute__((interrupt));
#define R_PRAGMA_STATIC_INTERRUPT_FUNCTION(function_name)    static void function_name(void) __attribute__((interrupt, used));

#define R_PRAGMA_FAST_INTERRUPT(function_name)               extern void function_name(void) __attribute__((interrupt(fast)));
#define R_PRAGMA_STATIC_FAST_INTERRUPT(function_name)        static void function_name(void) __attribute__((interrupt(fast)), used);

#define R_ATTRIB_INTERRUPT                                   extern /* only this one because __attribute__((interrupt)) prevents GNURX from generating vector */
#define R_ATTRIB_STATIC_INTERRUPT                            static /* only this one because __attribute__((interrupt, used)) prevents GNURX from generating vector */

#define R_ATTRIB_FAST_INTERRUPT                              extern /* __attribute__((interrupt(fast))) Not necessary, but Don't forget a R_PRAGMA_FAST_INTERRUPT() declaration */
#define R_ATTRIB_STATIC_FAST_INTERRUPT                       static /* __attribute__((interrupt(fast)), used) Not necessary, but Don't forget a R_PRAGMA_STATIC_FAST_INTERRUPT() declaration */

#elif defined(__ICCRX__)

#define R_PRAGMA_INTERRUPT(function_name, vect)              R_PRAGMA(vector=vect)\
                                                             extern __interrupt void function_name(void);
#define R_PRAGMA_STATIC_INTERRUPT(function_name, vect)       R_PRAGMA(vector=vect)\
                                                             static __interrupt void function_name(void);

#define R_PRAGMA_INTERRUPT_DEFAULT(function_name)            extern __interrupt void function_name(void);
#define R_PRAGMA_STATIC_INTERRUPT_DEFAULT(function_name)     static __interrupt void function_name(void);

#define R_PRAGMA_INTERRUPT_FUNCTION(function_name)           extern __interrupt void function_name(void);
#define R_PRAGMA_STATIC_INTERRUPT_FUNCTION(function_name)    static __interrupt void function_name(void);

#define R_PRAGMA_FAST_INTERRUPT(function_name)               extern __fast_interrupt void function_name(void);
#define R_PRAGMA_STATIC_FAST_INTERRUPT(function_name)        static __fast_interrupt void function_name(void);

#define R_ATTRIB_INTERRUPT                                   extern __interrupt /* ICCRX requires __interrupt not only at a function declaration but also at a function definition */
#define R_ATTRIB_STATIC_INTERRUPT                            static __interrupt /* ICCRX requires __interrupt not only at a function declaration but also at a function definition */

#define R_ATTRIB_FAST_INTERRUPT                              extern __fast_interrupt /* ICCRX requires __interrupt not only at a function declaration but also at a function definition */
#define R_ATTRIB_STATIC_FAST_INTERRUPT                       static __fast_interrupt /* ICCRX requires __interrupt not only at a function declaration but also at a function definition */

#endif

/* ---------- Inline Expansion of Function ---------- */
#if defined(__CCRX__)

#define R_PRAGMA_INLINE(function_name)           R_PRAGMA(inline function_name)\
                                                 extern
#define R_PRAGMA_STATIC_INLINE(function_name)    R_PRAGMA(inline function_name)\
                                                 static

#elif defined(__GNUC__)

#define R_PRAGMA_INLINE(function_name)           inline extern __attribute__((always_inline))
#define R_PRAGMA_STATIC_INLINE(function_name)    inline static __attribute__((always_inline))

#elif defined(__ICCRX__)

#define R_PRAGMA_INLINE(function_name)           R_PRAGMA(inline=forced)\
                                                 extern
#define R_PRAGMA_STATIC_INLINE(function_name)    R_PRAGMA(inline=forced)\
                                                 static

#endif

/* ---------- Inline Expansion of Assembly-Language Function (part1) ---------- */
#if defined(__CCRX__)

#define R_PRAGMA_INLINE_ASM(function_name)           R_PRAGMA(inline_asm function_name)\
                                                     extern
#define R_PRAGMA_STATIC_INLINE_ASM(function_name)    R_PRAGMA(inline_asm function_name)\
                                                     static

#elif defined(__GNUC__)

/* Using inline assembler without operands and clobbered resources is dangerous but using it with them is too difficult. */

#define R_PRAGMA_INLINE_ASM(function_name)           extern __attribute__((naked, noinline))
#define R_PRAGMA_STATIC_INLINE_ASM(function_name)    static __attribute__((naked, noinline))

#elif defined(__ICCRX__)

/* Using inline assembler without operands and clobbered resources is dangerous but using it with them is too difficult. */

#define R_PRAGMA_INLINE_ASM(function_name)           R_PRAGMA(inline=never)\
                                                     extern __task
#define R_PRAGMA_STATIC_INLINE_ASM(function_name)    R_PRAGMA(inline=never)\
                                                     static __task

#endif

/* ---------- Inline Expansion of Assembly-Language Function (part2) ---------- */
#if defined(__CDT_PARSER__)

#define R_ASM(...)        /* none */
#define R_LAB_NEXT(n)     /* none */
#define R_LAB_PREV(n)     /* none */
#define R_LAB(n_colon)    /* none */
#define R_ASM_BEGIN       /* none */
#define R_ASM_END         /* none */

#else

#if defined(__CCRX__)

#if !defined(__cplusplus)
#define R_ASM(...)        __VA_ARGS__
#else
/* CC-RX' C++ mode does not support variadic macros */
#endif
#define R_LAB_NEXT(n)     ?+
#define R_LAB_PREV(n)     ?-
#define R_LAB(n_colon)    R_ASM(?:)
#define R_ASM_BEGIN       /* none */
#define R_ASM_END         /* none */

#elif defined(__GNUC__)

#define _R_ASM(...)       #__VA_ARGS__
#define R_ASM(...)        _R_ASM(__VA_ARGS__\n)
#define R_LAB_NEXT(n)     ?+
#define R_LAB_PREV(n)     ?-
#define R_LAB(n_colon)    R_ASM(?:)
#define R_ASM_BEGIN       __asm__ volatile (
#define R_ASM_END         R_ASM(rts));

#elif defined(__ICCRX__)

#define _R_ASM(...)       #__VA_ARGS__
#define R_ASM(...)        _R_ASM(__VA_ARGS__\n)
#define R_LAB_NEXT(n)     _lab##n
#define R_LAB_PREV(n)     _lab##n
#define R_LAB(n_colon)    R_ASM(_lab##n_colon)
#define R_ASM_BEGIN       asm(
#define R_ASM_END         );

#endif

#endif

/* ---------- Inline Expansion of Assembly-Language Function (part3) ---------- */
#if defined(__CCRX__)

#define R_ASM_INTERNAL_USED(p)        /* no way */
#define R_ASM_INTERNAL_NOT_USED(p)    /* no way */

#elif defined(__GNUC__)

#define R_ASM_INTERNAL_USED(p)        ((void)(p));
#define R_ASM_INTERNAL_NOT_USED(p)    ((void)(p));

#elif defined(__ICCRX__)

#define R_ASM_INTERNAL_USED(p)        ((void)(p));
#define R_ASM_INTERNAL_NOT_USED(p)    ((void)(p));

#endif

/* ---------- Bit Field Order Specification ---------- */
#if defined(__CCRX__)

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_2(bf1, bf2)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_3(bf1, bf2, bf3)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_4(bf1, bf2, bf3, bf4)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_5(bf1, bf2, bf3, bf4, bf5)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_6(bf1, bf2, bf3, bf4, bf5, bf6)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_7(bf1, bf2, bf3, bf4, bf5, bf6, bf7)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_8(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_9(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_10(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_11(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_12(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_13(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_14(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_15(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_16(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_17(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_18(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_19(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_20(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_21(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_22(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_23(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_24(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_25(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_26(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_27(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_28(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_29(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_30(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_31(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
        bf31;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_32(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31, bf32)\
struct {\
R_PRAGMA(bit_order left)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
        bf31;\
        bf32;\
    };\
R_PRAGMA(bit_order)\
}

#elif defined(__GNUC__)

#if defined(__RX_LITTLE_ENDIAN__)

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_2(bf1, bf2)\
struct {\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_3(bf1, bf2, bf3)\
struct {\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_4(bf1, bf2, bf3, bf4)\
struct {\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_5(bf1, bf2, bf3, bf4, bf5)\
struct {\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_6(bf1, bf2, bf3, bf4, bf5, bf6)\
struct {\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_7(bf1, bf2, bf3, bf4, bf5, bf6, bf7)\
struct {\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_8(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8)\
struct {\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_9(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9)\
struct {\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_10(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10)\
struct {\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_11(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11)\
struct {\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_12(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12)\
struct {\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_13(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13)\
struct {\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_14(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14)\
struct {\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_15(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15)\
struct {\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_16(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16)\
struct {\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_17(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17)\
struct {\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_18(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18)\
struct {\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_19(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19)\
struct {\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_20(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20)\
struct {\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_21(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21)\
struct {\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_22(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22)\
struct {\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_23(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23)\
struct {\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_24(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24)\
struct {\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_25(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25)\
struct {\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_26(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26)\
struct {\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_27(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27)\
struct {\
    bf27;\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_28(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28)\
struct {\
    bf28;\
    bf27;\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_29(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29)\
struct {\
    bf29;\
    bf28;\
    bf27;\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_30(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30)\
struct {\
    bf30;\
    bf29;\
    bf28;\
    bf27;\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_31(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31)\
struct {\
    bf31;\
    bf30;\
    bf29;\
    bf28;\
    bf27;\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_32(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31, bf32)\
struct {\
    bf32;\
    bf31;\
    bf30;\
    bf29;\
    bf28;\
    bf27;\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#else

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_2(bf1, bf2)\
struct {\
    bf1;\
    bf2;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_3(bf1, bf2, bf3)\
struct {\
    bf1;\
    bf2;\
    bf3;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_4(bf1, bf2, bf3, bf4)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_5(bf1, bf2, bf3, bf4, bf5)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_6(bf1, bf2, bf3, bf4, bf5, bf6)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_7(bf1, bf2, bf3, bf4, bf5, bf6, bf7)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_8(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_9(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_10(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_11(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_12(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_13(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_14(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_15(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_16(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_17(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_18(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_19(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_20(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_21(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_22(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_23(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_24(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_25(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_26(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_27(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
    bf27;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_28(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
    bf27;\
    bf28;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_29(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
    bf27;\
    bf28;\
    bf29;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_30(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
    bf27;\
    bf28;\
    bf29;\
    bf30;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_31(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
    bf27;\
    bf28;\
    bf29;\
    bf30;\
    bf31;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_32(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31, bf32)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
    bf27;\
    bf28;\
    bf29;\
    bf30;\
    bf31;\
    bf32;\
}

#endif


#elif defined(__ICCRX__)

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_2(bf1, bf2)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_3(bf1, bf2, bf3)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_4(bf1, bf2, bf3, bf4)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_5(bf1, bf2, bf3, bf4, bf5)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_6(bf1, bf2, bf3, bf4, bf5, bf6)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_7(bf1, bf2, bf3, bf4, bf5, bf6, bf7)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_8(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_9(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_10(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_11(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_12(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_13(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_14(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_15(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_16(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_17(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_18(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_19(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_20(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_21(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_22(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_23(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_24(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_25(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_26(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_27(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_28(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_29(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_30(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_31(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
        bf31;\
    };\
R_PRAGMA(bitfields=default)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_32(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31, bf32)\
struct {\
R_PRAGMA(bitfields=reversed_disjoint_types)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
        bf31;\
        bf32;\
    };\
R_PRAGMA(bitfields=default)\
}

#endif

#if defined(__CCRX__)

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_2(bf1, bf2)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_3(bf1, bf2, bf3)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_4(bf1, bf2, bf3, bf4)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_5(bf1, bf2, bf3, bf4, bf5)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_6(bf1, bf2, bf3, bf4, bf5, bf6)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_7(bf1, bf2, bf3, bf4, bf5, bf6, bf7)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_8(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_9(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_10(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_11(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_12(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_13(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_14(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_15(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_16(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_17(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_18(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_19(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_20(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_21(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                             bf21)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_22(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                             bf21, bf22)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_23(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                             bf21, bf22, bf23)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_24(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                             bf21, bf22, bf23, bf24)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_25(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                             bf21, bf22, bf23, bf24, bf25)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_26(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                             bf21, bf22, bf23, bf24, bf25, bf26)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_27(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                             bf21, bf22, bf23, bf24, bf25, bf26, bf27)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_28(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                             bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_29(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                             bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_30(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                             bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_31(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                             bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                             bf31)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
        bf31;\
    };\
R_PRAGMA(bit_order)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_32(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                             bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                             bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                             bf31, bf32)\
struct {\
R_PRAGMA(bit_order right)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
        bf31;\
        bf32;\
    };\
R_PRAGMA(bit_order)\
}

#elif defined(__GNUC__)

#if defined(__RX_LITTLE_ENDIAN__)

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_2(bf1, bf2)\
struct {\
    bf1;\
    bf2;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_3(bf1, bf2, bf3)\
struct {\
    bf1;\
    bf2;\
    bf3;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_4(bf1, bf2, bf3, bf4)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_5(bf1, bf2, bf3, bf4, bf5)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_6(bf1, bf2, bf3, bf4, bf5, bf6)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_7(bf1, bf2, bf3, bf4, bf5, bf6, bf7)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_8(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_9(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_10(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_11(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_12(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_13(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_14(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_15(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_16(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_17(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_18(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_19(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_20(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_21(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_22(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_23(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_24(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_25(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_26(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_27(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
    bf27;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_28(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
    bf27;\
    bf28;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_29(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
    bf27;\
    bf28;\
    bf29;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_30(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
    bf27;\
    bf28;\
    bf29;\
    bf30;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_31(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
    bf27;\
    bf28;\
    bf29;\
    bf30;\
    bf31;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_32(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31, bf32)\
struct {\
    bf1;\
    bf2;\
    bf3;\
    bf4;\
    bf5;\
    bf6;\
    bf7;\
    bf8;\
    bf9;\
    bf10;\
    bf11;\
    bf12;\
    bf13;\
    bf14;\
    bf15;\
    bf16;\
    bf17;\
    bf18;\
    bf19;\
    bf20;\
    bf21;\
    bf22;\
    bf23;\
    bf24;\
    bf25;\
    bf26;\
    bf27;\
    bf28;\
    bf29;\
    bf30;\
    bf31;\
    bf32;\
}

#else

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_2(bf1, bf2)\
struct {\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_3(bf1, bf2, bf3)\
struct {\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_4(bf1, bf2, bf3, bf4)\
struct {\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_5(bf1, bf2, bf3, bf4, bf5)\
struct {\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_6(bf1, bf2, bf3, bf4, bf5, bf6)\
struct {\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_7(bf1, bf2, bf3, bf4, bf5, bf6, bf7)\
struct {\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_8(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8)\
struct {\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_9(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9)\
struct {\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_10(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10)\
struct {\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_11(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11)\
struct {\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_12(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12)\
struct {\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_13(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13)\
struct {\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_14(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14)\
struct {\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_15(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15)\
struct {\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_16(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16)\
struct {\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_17(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17)\
struct {\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_18(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18)\
struct {\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_19(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19)\
struct {\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_20(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20)\
struct {\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_21(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21)\
struct {\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_22(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22)\
struct {\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_23(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23)\
struct {\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_24(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24)\
struct {\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_25(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25)\
struct {\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_26(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26)\
struct {\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_27(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27)\
struct {\
    bf27;\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_28(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28)\
struct {\
    bf28;\
    bf27;\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_29(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29)\
struct {\
    bf29;\
    bf28;\
    bf27;\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_30(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30)\
struct {\
    bf30;\
    bf29;\
    bf28;\
    bf27;\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_31(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31)\
struct {\
    bf31;\
    bf30;\
    bf29;\
    bf28;\
    bf27;\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_32(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31, bf32)\
struct {\
    bf32;\
    bf31;\
    bf30;\
    bf29;\
    bf28;\
    bf27;\
    bf26;\
    bf25;\
    bf24;\
    bf23;\
    bf22;\
    bf21;\
    bf20;\
    bf19;\
    bf18;\
    bf17;\
    bf16;\
    bf15;\
    bf14;\
    bf13;\
    bf12;\
    bf11;\
    bf10;\
    bf9;\
    bf8;\
    bf7;\
    bf6;\
    bf5;\
    bf4;\
    bf3;\
    bf2;\
    bf1;\
}

#endif

#elif defined(__ICCRX__)

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_2(bf1, bf2)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_3(bf1, bf2, bf3)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_4(bf1, bf2, bf3, bf4)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_5(bf1, bf2, bf3, bf4, bf5)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_6(bf1, bf2, bf3, bf4, bf5, bf6)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_7(bf1, bf2, bf3, bf4, bf5, bf6, bf7)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_8(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_9(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_10(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_11(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_12(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_13(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_14(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_15(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_16(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_17(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_18(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_19(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_20(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_21(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_22(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_23(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_24(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_25(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_26(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_27(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_28(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_29(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_30(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_31(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
        bf31;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#define R_ATTRIB_BITFIELD_BIT_ORDER_RIGHT_32(bf1, bf2, bf3, bf4, bf5, bf6, bf7, bf8, bf9, bf10, \
                                            bf11, bf12, bf13, bf14, bf15, bf16, bf17, bf18, bf19, bf20, \
                                            bf21, bf22, bf23, bf24, bf25, bf26, bf27, bf28, bf29, bf30, \
                                            bf31, bf32)\
struct {\
R_PRAGMA(bitfields=default)\
    struct {\
        bf1;\
        bf2;\
        bf3;\
        bf4;\
        bf5;\
        bf6;\
        bf7;\
        bf8;\
        bf9;\
        bf10;\
        bf11;\
        bf12;\
        bf13;\
        bf14;\
        bf15;\
        bf16;\
        bf17;\
        bf18;\
        bf19;\
        bf20;\
        bf21;\
        bf22;\
        bf23;\
        bf24;\
        bf25;\
        bf26;\
        bf27;\
        bf28;\
        bf29;\
        bf30;\
        bf31;\
        bf32;\
    };\
R_PRAGMA(bitfields=reversed_disjoint_types)\
}

#endif

/* ---------- Alignment Value Specification for Structure Members and Class Members ---------- */
#if defined(__CCRX__)

#define R_PRAGMA_PACK          R_PRAGMA(pack)
#define R_PRAGMA_UNPACK        R_PRAGMA(unpack)
#define R_PRAGMA_PACKOPTION    R_PRAGMA(packoption)

#elif defined(__GNUC__)

#define R_PRAGMA_PACK          R_PRAGMA(pack(1))
#define R_PRAGMA_UNPACK        R_PRAGMA(pack(4))
#define R_PRAGMA_PACKOPTION    R_PRAGMA(pack())

#elif defined(__ICCRX__)

#define R_PRAGMA_PACK          R_PRAGMA(pack(1))
#define R_PRAGMA_UNPACK        R_PRAGMA(pack(4))
#define R_PRAGMA_PACKOPTION    R_PRAGMA(pack())

#endif


/* ========== Warning supression macro ========== */
/* This macro is used to suppress compiler messages about not only a parameter but also a auto variable not being used
 * in a function. The nice thing about using this implementation is that it does not take any extra RAM or ROM.
 * This macro is available for the followings:
 * CC-RX's 'M0520826:Parameter "XXXX" was never referenced'
 * CC-RX's 'W0520550:Variable "XXXX" was set but never used'
 * GNURX's 'unused parameter 'XXXX' [-Wunused-parameter]'
 * GNURX's 'variable 'XXXX' set but not used [-Wunused-but-set-variable]'
 * When the variable is declared as volatile, the '&' can be applied like 'R_INTERNAL_NOT_USED(&volatile_variable);'.
 */
#define R_INTERNAL_NOT_USED(p)    ((void)(p))


/* ========== Intrinsic Functions ========== */
#if defined(__CCRX__)

/* ---------- Data Exchange ---------- */
#define R_EXCHANGE(x, y)    xchg((signed long *)(x), (signed long *)(y))    /* void xchg(signed long *data1, signed long *data2) */

/* ---------- Special Instructions ---------- */
#define R_BRK()     brk()     /* void brk(void) */
#define R_WAIT()    wait()    /* void wait(void) */
#define R_NOP()     nop()     /* void nop(void) */

/* ---------- Processor interrupt priority level (IPL) ---------- */
//#define R_SET_IPL(x)    set_ipl((signed long)(x))    /* void set_ipl(signed long level) */
//#define R_GET_IPL()     get_ipl()                    /* unsigned char get_ipl(void) */

/* ---------- Processor status word (PSW) ---------- */
#define R_SET_PSW(x)    set_psw((unsigned long)(x))    /* void set_psw(unsigned long data) */
#define R_GET_PSW()     get_psw()                      /* unsigned long get_psw(void) */

/* ---------- Floating-point status word (FPSW) ---------- */
#define R_SET_FPSW(x)    set_fpsw((unsigned long)(x))    /* void set_fpsw(unsigned long data) */
#define R_GET_FPSW()     get_fpsw()                      /* unsigned long get_fpsw(void) */

/* ---------- User Stack Pointer (USP) ---------- */
#define R_SET_USP(x)    set_usp((void *)(x))    /* void set_usp(void *data) */
#define R_GET_USP()     get_usp()               /* void *get_usp(void) */

/* ---------- Interrupt Stack Pointer (ISP) ---------- */
#define R_SET_ISP(x)    set_isp((void *)(x))    /* void set_fpsw(void *data) */
#define R_GET_ISP()     get_isp()               /* void *get_fpsw(void) */

/* ---------- Fast Interrupt Vector Register (FINTV) ---------- */
#define R_SET_FINTV(x)    set_fintv((void *)(x))    /* void set_fintv(void *data) */
#define R_GET_FINTV()     get_fintv()               /* void *get_fintv(void) */

/* ---------- Interrupt Table Register (INTB) ---------- */
#define R_SET_INTB(x)    set_intb((void *)(x))    /* void set_intb(void *data) */
#define R_GET_INTB()     get_intb()               /* void *get_intb(void) */

/* ---------- Exception Table Register (EXTB) ---------- */
#define R_SET_EXTB(x)    set_extb((void *)(x))    /* void set_extb(void *data) */
#define R_GET_EXTB()     get_extb()               /* void *get_extb(void) */

/* ---------- Control of the interrupt enable bits ---------- */
#define R_SETPSW_I()    setpsw_i()    /* void setpsw_i(void) */
#define R_CLRPSW_I()    clrpsw_i()    /* void clrpsw_i(void) */

#elif defined(__GNUC__)

/* ---------- Data Exchange ---------- */
#define R_EXCHANGE(x, y)    __builtin_rx_xchg((int *)(x), (int *)(y))    /* void __builtin_rx_xchg (int *, int *) */

/* ---------- Special Instructions ---------- */
#define R_BRK()     __asm("brk")
#define R_WAIT()    __asm("wait")
#define R_NOP()     __asm("nop")

/* It is useful to define the following commonly used functions.
 * These are the same as CG source code for GNURX.
 */
#if !defined(brk)
#define brk()       asm("brk;")
#endif
#if !defined(wait)
#define wait()      asm("wait;")
#endif
#if !defined(nop)
#define nop()       asm("nop;")
#endif

/* ---------- Processor interrupt priority level (IPL) ---------- */
//#define R_SET_IPL(x)    /* none */
//#define R_GET_IPL()     /* none */

/* ---------- Processor status word (PSW) ---------- */
#define R_SET_PSW(x)    __builtin_rx_mvtc(0x0, (int)(x))    /* void __builtin_rx_mvtc (int reg, int val) */
#define R_GET_PSW()     __builtin_rx_mvfc(0x0)              /* int __builtin_rx_mvfc (int) */

/* ---------- Floating-point status word (FPSW) ---------- */
#define R_SET_FPSW(x)    __builtin_rx_mvtc(0x3, (int)(x))    /* void __builtin_rx_mvtc (int reg, int val) */
#define R_GET_FPSW()     __builtin_rx_mvfc(0x3)              /* int __builtin_rx_mvfc (int) */

/* ---------- User Stack Pointer (USP) ---------- */
#define R_SET_USP(x)    __builtin_rx_mvtc(0x2, (int)(x))    /* void __builtin_rx_mvtc (int reg, int val) */
#define R_GET_USP()     __builtin_rx_mvfc(0x2)              /* int __builtin_rx_mvfc (int) */

/* ---------- Interrupt Stack Pointer (ISP) ---------- */
#define R_SET_ISP(x)    __builtin_rx_mvtc(0xA, (int)(x))    /* void __builtin_rx_mvtc (int reg, int val) */
#define R_GET_ISP()     __builtin_rx_mvfc(0xA)              /* int __builtin_rx_mvfc (int) */

/* ---------- Fast Interrupt Vector Register (FINTV) ---------- */
#define R_SET_FINTV(x)    __builtin_rx_mvtc(0xB, (int)(x))    /* void __builtin_rx_mvtc (int reg, int val) */
#define R_GET_FINTV()     __builtin_rx_mvfc(0xB)              /* int __builtin_rx_mvfc (int) */

/* ---------- Interrupt Table Register (INTB) ---------- */
#define R_SET_INTB(x)    __builtin_rx_mvtc(0xC, (int)(x))    /* void __builtin_rx_mvtc (int reg, int val) */
#define R_GET_INTB()     __builtin_rx_mvfc(0xC)              /* int __builtin_rx_mvfc (int) */

/* ---------- Exception Table Register (EXTB) ---------- */
#define R_SET_EXTB(x)    __builtin_rx_mvtc(0xD, (int)(x))    /* void __builtin_rx_mvtc (int reg, int val) */
#define R_GET_EXTB()     __builtin_rx_mvfc(0xD)              /* int __builtin_rx_mvfc (int) */

/* ---------- Control of the interrupt enable bits ---------- */
#define R_SETPSW_I()    __builtin_rx_setpsw('I')    /* void __builtin_rx_setpsw (int) */
#define R_CLRPSW_I()    __builtin_rx_clrpsw('I')    /* void __builtin_rx_setpsw (int) */

#elif defined(__ICCRX__)

/* ---------- Data Exchange ---------- */
#define R_EXCHANGE(x, y)    xchg((signed long *)(x), (signed long *)(y))    /* void xchg(signed long *data1, signed long *data2) */

/* ---------- Special Instructions ---------- */
#define R_BRK()     __break()                 /* void __break(void) */
#define R_WAIT()    __wait_for_interrupt()    /* void __wait_for_interrupt(void) */
#define R_NOP()     __no_operation()          /* void __no_operation(void) */

/* It is useful to define the following commonly used functions.
 * TODO: Are these the same as CG source code for ICCRX?
 */
#if !defined(brk)
#define brk()       __break()
#endif
#if !defined(wait)
#define wait()      __wait_for_interrupt()
#endif
#if !defined(nop)
#define nop()       __no_operation()
#endif

/* ---------- Processor interrupt priority level (IPL) ---------- */
//#define R_SET_IPL(x)    __set_interrupt_level((__ilevel_t)(x))    /* void __set_interrupt_level(__ilevel_t) */
//#define R_GET_IPL()     __get_interrupt_level()                   /* __ilevel_t __get_interrupt_level(void) */

/* ---------- Processor status word (PSW) ---------- */
#define R_SET_PSW(x)    set_psw((unsigned long)(x))    /* void set_psw(unsigned long data) */
#define R_GET_PSW()     get_psw()                      /* unsigned long get_psw(void) */

/* ---------- Floating-point status word (FPSW) ---------- */
#define R_SET_FPSW(x)    set_fpsw((unsigned long)(x))    /* void set_fpsw(unsigned long data) */
#define R_GET_FPSW()     get_fpsw()                      /* unsigned long get_fpsw(void) */

/* ---------- User Stack Pointer (USP) ---------- */
#define R_SET_USP(x)    __set_USP_register((unsigned long)(x))    /* void __set_USP_register(unsigned long) */
#define R_GET_USP()     __get_USP_register()                      /* unsigned long __get_USP_register(void) */

/* ---------- Interrupt Stack Pointer (ISP) ---------- */
#define R_SET_ISP(x)    __set_ISP_register((unsigned long)(x))    /* void __set_ISP_register(unsigned long) */
#define R_GET_ISP()     __get_ISP_register()                      /* unsigned long __get_ISP_register(void) */

/* ---------- Fast Interrupt Vector Register (FINTV) ---------- */
#define R_SET_FINTV(x)    __get_FINTV_register((__fast_int_f)(x))    /* void __set_FINTV_register(__fast_int_f) */
#define R_GET_FINTV()     __get_FINTV_register()                     /* __fast_int_f __get_FINTV_register(void) */

/* ---------- Interrupt Table Register (INTB) ---------- */
#define R_SET_INTB(x)    __set_interrupt_table((unsigned long)(x))    /* void __set_interrupt_table(unsigned long) */
#define R_GET_INTB()     __get_interrupt_table()                      /* unsigned long __get_interrupt_table(void) */

/* ---------- Exception Table Register (EXTB) ---------- */
#define R_SET_EXTB(x)    set_EXTB_register((unsigned long)(x))
void set_EXTB_register(unsigned long value);
R_PRAGMA_STATIC_INLINE(set_EXTB_register)
void set_EXTB_register(unsigned long value){asm("mvtc %0,EXTB" : : "r"(value));}
#define R_GET_EXTB()     get_EXTB_register()
unsigned long get_EXTB_register(void);
R_PRAGMA_STATIC_INLINE(get_EXTB_register)
unsigned long get_EXTB_register(void){unsigned long value; asm("mvfc EXTB,%0" : "=r"(value)); return value;}

/* ---------- Control of the interrupt enable bits ---------- */
#define R_SETPSW_I()    __enable_interrupt()     /* void __enable_interrupt(void) */
#define R_CLRPSW_I()    __disable_interrupt()    /* void __disable_interrupt(void) */

#endif

#endif /* R_GCOMPILER_H */
