/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : compiler.h
* Author             : AMS - VMA RF Application team 
* Version            : V1.0.0
* Date               : 14-September-2015
* Description        : Compiler-dependent macros.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#ifndef __COMPILER_H__
#define __COMPILER_H__

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define QUOTEME(a) #a

/** @addtogroup compiler_macros compiler macros
  * @{
  */


/** @addtogroup IAR_toolchain_macros    IAR toolchain macros
  * @{
  */

/**
  * @brief  This is the section dedicated to IAR toolchain
  */
#if defined(__ICCARM__) || defined(__IAR_SYSTEMS_ASM__)
    
/**
  * @brief  PACKED
  *         Use the PACKED macro for variables that needs to be packed.
  *         Usage:  PACKED(struct) myStruct_s
  *                 PACKED(union) myStruct_s
  */
#define PACKED(decl)                   __packed decl
    
/**
  * @brief  REQUIRED
  *         Use the REQUIRED macro for variables that must be always included.
  *         Usage:  REQUIRED(static uint8_t my_array[16])
  *                 REQUIRED(static int my_int)
  */
#define REQUIRED(decl)                 __root decl

/**
  * @brief  NORETURN_FUNCTION
  *         Use the NORETURN_FUNCTION macro to declare a no return function.
  *         Usage:  NORETURN_FUNCTION(void my_noretrun_function(void))
  */
#define NORETURN_FUNCTION(function)                 __noreturn function

/**
  * @brief  SECTION
  *         Use the SECTION macro to assign data or code in a specific section.
  *         Usage:  SECTION(".my_section")
  */
#define SECTION(name)                  _Pragma(QUOTEME(location=name))

/**
  * @brief  ALIGN
  *         Use the ALIGN macro to specify the alignment of a variable.
  *         Usage:  ALIGN(4)
  */
#define ALIGN(v)                       _Pragma(QUOTEME(data_alignment=v))

/**
  * @brief  WEAK_FUNCTION
  *         Use the WEAK_FUNCTION macro to declare a weak function.
  *         Usage:  WEAK_FUNCTION(int my_weak_function(void))
  */
#define WEAK_FUNCTION(function)        __weak function

/**
  * @brief  NO_INIT
  *         Use the NO_INIT macro to declare a not initialized variable.
  *         Usage:  NO_INIT(int my_no_init_var)
  *         Usage:  NO_INIT(uint16_t my_no_init_array[10])
  */
#define NO_INIT(var)                   __no_init var

/**
  * @brief  NOLOAD
  *         Use the NOLOAD macro to declare a not initialized variable that 
  *             must be placed in a specific section in Flash.
  *             Before the NOLOAD declaration, the SECTION declaration must be used.
  *
  *         SECTION(".noinit.ro_section_my_noload_var")
  *         Usage:  NOLOAD(int my_noload_var)
  */
#define NOLOAD(var)                    NO_INIT(var)

#define VARIABLE_SIZE 0
#pragma segment = "CSTACK"
#define _INITIAL_SP                    { .__ptr = __sfe( "CSTACK" ) }  /* Stack address */
extern void __iar_program_start(void);
#define RESET_HANDLER                  __iar_program_start

/**
 * @}
 */
    
/** @addtogroup Atollic_toolchain_macros    Atollic toolchain macros
  * @{
  */

/**
  * @brief  This is the section dedicated to Atollic toolchain
  */
#else
#ifdef __GNUC__

/**
  * @brief  PACKED
  *         Use the PACKED macro for variables that needs to be packed.
  *         Usage:  PACKED(struct) myStruct_s
  *                 PACKED(union) myStruct_s
  */
#define PACKED(decl)                    decl __attribute__((packed))

/**
  * @brief  REQUIRED
  *         Use the REQUIRED macro for variables that must be always included.
  *         Usage:  REQUIRED(static uint8_t my_array[16])
  *                 REQUIRED(static int my_int)
  */
#define REQUIRED(var)                   var __attribute__((used))

/**
  * @brief  SECTION
  *         Use the SECTION macro to assign data or code in a specific section.
  *         Usage:  SECTION(".my_section")
  */
#define SECTION(name)                   __attribute__((section(name)))

/**
  * @brief  ALIGN
  *         Use the ALIGN macro to specify the alignment of a variable.
  *         Usage:  ALIGN(4)
  */
#define ALIGN(N)                        __attribute__((aligned(N)))

/**
  * @brief  WEAK_FUNCTION
  *         Use the WEAK_FUNCTION macro to declare a weak function.
  *         Usage:  WEAK_FUNCTION(int my_weak_function(void))
  */
#define WEAK_FUNCTION(function)         __attribute__((weak)) function 

/**
  * @brief  NORETURN_FUNCTION
  *         Use the NORETURN_FUNCTION macro to declare a no return function.
  *         Usage:  NORETURN_FUNCTION(void my_noretrun_function(void))
  */
#define NORETURN_FUNCTION(function)     __attribute__((noreturn)) function 

/**
  * @brief  NO_INIT
  *         Use the NO_INIT macro to declare a not initialized variable.
  *         Usage:  NO_INIT(int my_no_init_var)
  *         Usage:  NO_INIT(uint16_t my_no_init_array[10])
  */
#define NO_INIT(var)                    var  __attribute__((section(".noinit")))

/**
  * @brief  NOLOAD
  *         Use the NOLOAD macro to declare a not initialized variable that 
  *             must be placed in a specific section in Flash.
  *             Before the NOLOAD declaration, the SECTION declaration must be used.
  *             Then, this section must be placed correctly in the linker file.
  *
  *         SECTION(".noinit.ro_section_my_noload_var")
  *         Usage:  NOLOAD(int my_noload_var)
  */
#define NOLOAD(var)                    var

#define _INITIAL_SP                     (void(*)(void))(&_estack)
#define VARIABLE_SIZE 0


/** @addtogroup Keil_toolchain_macros    Keil toolchain macros
  * @{
  */

/**
  * @brief  This is the section dedicated to Keil toolchain
  */
#else
#ifdef __CC_ARM	

/**
  * @brief  PACKED
  *         Use the PACKED macro for variables that needs to be packed.
  *         Usage:  PACKED(struct) myStruct_s
  *                 PACKED(union) myStruct_s
  */
#define PACKED(decl)                        decl __attribute__((packed))

/**
  * @brief  REQUIRED
  *         Use the REQUIRED macro for variables that must be always included.
  *         Usage:  REQUIRED(static uint8_t my_array[16])
  *                 REQUIRED(static int my_int)
  */
#define REQUIRED(decl)                      decl __attribute__((used))

/**
  * @brief  SECTION
  *         Use the SECTION macro to assign data or code in a specific section.
  *         Usage:  SECTION(".my_section")
  */
#define SECTION(name)                       __attribute__((section(name)))

/**
  * @brief  ALIGN
  *         Use the ALIGN macro to specify the alignment of a variable.
  *         Usage:  ALIGN(4)
  */
#define ALIGN(N)                            __attribute__((aligned(N)))

/**
  * @brief  WEAK_FUNCTION
  *         Use the WEAK_FUNCTION macro to declare a weak function.
  *         Usage:  WEAK_FUNCTION(int my_weak_function(void))
  */
#define WEAK_FUNCTION(function)             __weak function

/**
  * @brief  NORETURN_FUNCTION
  *         Use the NORETURN_FUNCTION macro to declare a no return function.
  *         Usage:  NORETURN_FUNCTION(void my_noretrun_function(void))
  */
#define NORETURN_FUNCTION(function)     __attribute__((noreturn)) function 

/**
  * @brief  NO_INIT
  *         Use the NO_INIT macro to declare a not initialized variable.
  *         Usage:  NO_INIT(int my_no_init_var)
  *         Usage:  NO_INIT(uint16_t my_no_init_array[10])
  */
#define NO_INIT(var)                        var __attribute__((section("NoInit")))

/**
  * @brief  NOLOAD
  *         Use the NOLOAD macro to declare a not initialized variable that 
  *             must be placed in a specific section in Flash.
  *             Before the NOLOAD declaration, the SECTION declaration must be used.
  *             Then, this section must be placed correctly in the linker file.
  *
  *         SECTION(".noinit.ro_section_my_noload_var")
  *         Usage:  NOLOAD(int my_noload_var)
  */
#define NOLOAD(var)                         var

extern void __main(void);
extern int main(void);
extern unsigned int Image$$ARM_LIB_STACKHEAP$$ZI$$Limit;
#define _INITIAL_SP                         (void(*)(void))&Image$$ARM_LIB_STACKHEAP$$ZI$$Limit  /* Stack address */
#define VARIABLE_SIZE 1

/**
 * @}
 */

#else

#error Neither ICCARM, CC ARM nor GNUC C detected. Define your macros.

#endif
#endif
#endif

/**
 * @}
 */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
#endif /* __COMPILER_H__ */
