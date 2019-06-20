/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
#include <stdint.h>
#include "cr4.h"
#include "platform_stdio.h"
#include "platform_isr.h"
#include "platform_assert.h"
#include "platform_peripheral.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define CR4_FAULT_STATUS_MASK  (0x140f)

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    CR4_FAULT_STATUS_ALIGNMENT                             = 0x001,   /* Highest Priority */
    CR4_FAULT_STATUS_MPU_BACKGROUND                        = 0x000,
    CR4_FAULT_STATUS_MPU_PERMISSION                        = 0x00D,
    CR4_FAULT_STATUS_SYNC_EXTERNAL_ABORT_AXI_DECODE_ERROR  = 0x008,
    CR4_FAULT_STATUS_ASYNC_EXTERNAL_ABORT_AXI_DECODE_ERROR = 0x406,
    CR4_FAULT_STATUS_SYNC_EXTERNAL_ABORT_AXI_SLAVE_ERROR   = 0x1008,
    CR4_FAULT_STATUS_ASYNC_EXTERNAL_ABORT_AXI_SLAVE_ERROR  = 0x1406,
    CR4_FAULT_STATUS_SYNC_PARITY_ECC                       = 0x409,
    CR4_FAULT_STATUS_ASYNC_PARITY_ECC                      = 0x408,
    CR4_FAULT_STATUS_DEBUG_EVENT                           = 0x002,   /* Lowest Priority */
} cr4_fault_status_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

#ifndef DEBUG

static void UnhandledException( void );

PLATFORM_SET_DEFAULT_ISR( irq_vector_undefined_instruction, UnhandledException )
PLATFORM_SET_DEFAULT_ISR( irq_vector_prefetch_abort,        UnhandledException )
PLATFORM_SET_DEFAULT_ISR( irq_vector_data_abort,            UnhandledException )
PLATFORM_SET_DEFAULT_ISR( irq_vector_fast_interrupt,        UnhandledException )
PLATFORM_SET_DEFAULT_ISR( irq_vector_software_interrupt,    UnhandledException )
PLATFORM_SET_DEFAULT_ISR( irq_vector_reserved,              UnhandledException )

static void UnhandledException( void )
{
    platform_mcu_reset( );
}

#else /* DEBUG */

static volatile wiced_bool_t breakpoint_inside_exception = WICED_FALSE;

static void default_fast_interrupt_handler( void );
static void default_software_interrupt_handler( void );
static void default_reserved_interrupt_handler( void );
static void prefetch_abort_handler( void );
static void data_abort_handler( void );
static void undefined_instruction_handler( void );

#ifndef WICED_DISABLE_EXCEPTION_DUMP

static uint32_t* exception_handler_saved_registers;

#define EXCEPTION_HANDLER_SAVE_REGISTERS() \
    do \
    {  \
        __asm__ __volatile__ ( "PUSH {R0 - R12, LR}" );                         \
        __asm__ __volatile__ ( "LDR  R0, =exception_handler_saved_registers" ); \
        __asm__ __volatile__ ( "STR  SP, [R0]" );                               \
    } \
    while ( 0 )

static unsigned exception_handler_strlen( const char* name )
{
    unsigned result = 0;

    while ( *name++ )
    {
        result++;
    }

    return result;
}

static void exception_handler_dump_string( const char* str )
{
    platform_stdio_exception_write( str, exception_handler_strlen( str ) );
}

static void exception_handler_dump_newline( void )
{
    const char* new_line = "\r\n";
    platform_stdio_exception_write( new_line, exception_handler_strlen( new_line ) );
}

static void exception_handler_dump_uint32( uint32_t num )
{
    const char hex[] = "0123456789ABCDEF";
    char buf[11];
    unsigned i;

    buf[0] = '0';
    buf[1] = 'x';

    for ( i = 0; i < 8; ++i )
    {
        buf[ 9 - i ] = hex[ num & 0xF ];
        num = num >> 4;
    }

    buf[ 10 ] = '\0';

    exception_handler_dump_string( buf );
}

static void exception_handler_dump_one_register( const char* name, uint32_t value )
{
    exception_handler_dump_string( name );
    exception_handler_dump_uint32( value );
    exception_handler_dump_newline();
}

static void exception_handler_dump_registers( const char* exception_name )
{
    exception_handler_dump_string( "=== EXCEPTION ===" );
    exception_handler_dump_newline();

    exception_handler_dump_string( exception_name );
    exception_handler_dump_newline();

    exception_handler_dump_one_register( "DFSR : ", get_DFSR() ); /* Valid for data exceptions */
    exception_handler_dump_one_register( "DFAR : ", get_DFAR() ); /* Valid for data exceptions */
    exception_handler_dump_one_register( "IFSR : ", get_IFSR() ); /* Valid for instruction exceptions */
    exception_handler_dump_one_register( "IFAR : ", get_IFAR() ); /* Valid for instruction exceptions */
    exception_handler_dump_one_register( "CPSR : ", get_CPSR() );

    if ( exception_handler_saved_registers )
    {
        exception_handler_dump_one_register( "R0   : ", exception_handler_saved_registers[0] );
        exception_handler_dump_one_register( "R1   : ", exception_handler_saved_registers[1] );
        exception_handler_dump_one_register( "R2   : ", exception_handler_saved_registers[2] );
        exception_handler_dump_one_register( "R3   : ", exception_handler_saved_registers[3] );
        exception_handler_dump_one_register( "R4   : ", exception_handler_saved_registers[4] );
        exception_handler_dump_one_register( "R5   : ", exception_handler_saved_registers[5] );
        exception_handler_dump_one_register( "R6   : ", exception_handler_saved_registers[6] );
        exception_handler_dump_one_register( "R7   : ", exception_handler_saved_registers[7] );
        exception_handler_dump_one_register( "R8   : ", exception_handler_saved_registers[8] );
        exception_handler_dump_one_register( "R9   : ", exception_handler_saved_registers[9] );
        exception_handler_dump_one_register( "R10  : ", exception_handler_saved_registers[10] );
        exception_handler_dump_one_register( "R11  : ", exception_handler_saved_registers[11] );
        exception_handler_dump_one_register( "R12  : ", exception_handler_saved_registers[12] );
        exception_handler_dump_one_register( "LR   : ", exception_handler_saved_registers[13] ); /* Where exception happened. Please check end of exception handler function which converts LR into address (offset of -4 or -8 may be applied). */
    }

    exception_handler_dump_string( "=================" );
    exception_handler_dump_newline();
}

static void __attribute__((naked)) default_fast_interrupt_handler_with_dump( void )
{
    (void)default_fast_interrupt_handler;
    EXCEPTION_HANDLER_SAVE_REGISTERS();
    __asm__ __volatile__ ("LDR R0, =default_fast_interrupt_handler; BX R0" );
}

static void __attribute__((naked)) default_software_interrupt_handler_with_dump( void )
{
    (void)default_software_interrupt_handler;
    EXCEPTION_HANDLER_SAVE_REGISTERS();
    __asm__ __volatile__ ("LDR R0, =default_software_interrupt_handler; BX R0" );
}

static void __attribute__((naked)) default_reserved_interrupt_handler_with_dump( void )
{
    (void)default_reserved_interrupt_handler;
    EXCEPTION_HANDLER_SAVE_REGISTERS();
    __asm__ __volatile__ ("LDR R0, =default_reserved_interrupt_handler; BX R0" );
}

static void __attribute__((naked)) prefetch_abort_handler_with_dump( void )
{
    (void)prefetch_abort_handler;
    EXCEPTION_HANDLER_SAVE_REGISTERS();
    __asm__ __volatile__ ("LDR R0, =prefetch_abort_handler; BX R0" );
}

static void __attribute__((naked)) data_abort_handler_with_dump( void )
{
    (void)data_abort_handler;
    EXCEPTION_HANDLER_SAVE_REGISTERS();
    __asm__ __volatile__ ("LDR R0, =data_abort_handler; BX R0" );
}

static void __attribute__((naked)) undefined_instruction_handler_with_dump( void )
{
    (void)undefined_instruction_handler;
    EXCEPTION_HANDLER_SAVE_REGISTERS();
    __asm__ __volatile__ ("LDR R0, =undefined_instruction_handler; BX R0" );
}

PLATFORM_SET_DEFAULT_ISR( irq_vector_undefined_instruction, undefined_instruction_handler_with_dump )
PLATFORM_SET_DEFAULT_ISR( irq_vector_prefetch_abort,        prefetch_abort_handler_with_dump )
PLATFORM_SET_DEFAULT_ISR( irq_vector_data_abort,            data_abort_handler_with_dump )
PLATFORM_SET_DEFAULT_ISR( irq_vector_fast_interrupt,        default_fast_interrupt_handler_with_dump )
PLATFORM_SET_DEFAULT_ISR( irq_vector_software_interrupt,    default_software_interrupt_handler_with_dump )
PLATFORM_SET_DEFAULT_ISR( irq_vector_reserved,              default_reserved_interrupt_handler_with_dump )

#define EXCEPTION_HANDLER_DUMP( str ) \
    do \
    {  \
        exception_handler_dump_registers( str ); \
    }  \
    while ( 0 )

#else

PLATFORM_SET_DEFAULT_ISR( irq_vector_undefined_instruction, undefined_instruction_handler )
PLATFORM_SET_DEFAULT_ISR( irq_vector_prefetch_abort,        prefetch_abort_handler )
PLATFORM_SET_DEFAULT_ISR( irq_vector_data_abort,            data_abort_handler )
PLATFORM_SET_DEFAULT_ISR( irq_vector_fast_interrupt,        default_fast_interrupt_handler )
PLATFORM_SET_DEFAULT_ISR( irq_vector_software_interrupt,    default_software_interrupt_handler )
PLATFORM_SET_DEFAULT_ISR( irq_vector_reserved,              default_reserved_interrupt_handler )

#define EXCEPTION_HANDLER_DUMP( str )

#endif /* WICED_DISABLE_EXCEPTION_DUMP */

#define EXCEPTION_HANDLER_BREAKPOINT() \
    do \
    {  \
        EXCEPTION_HANDLER_DUMP( __func__ );   \
        breakpoint_inside_exception = WICED_TRUE; \
        WICED_TRIGGER_BREAKPOINT();               \
    }  \
    while ( 0 )

void WEAK NEVER_INLINE platform_exception_debug( void )
{
    platform_watchdog_deinit();
}

static void default_fast_interrupt_handler( void )
{
    uint32_t lr = get_LR(); /* <----- CHECK THIS FOR WHERE EXCEPTION HAPPENED */

    UNUSED_VARIABLE( lr );

    /* Fast interrupt triggered without any configured handler */
    EXCEPTION_HANDLER_BREAKPOINT();
    while (1)
    {
        /* Loop forever */
    }
}

static void default_software_interrupt_handler( void )
{
    uint32_t lr = get_LR(); /* <----- CHECK THIS FOR WHERE EXCEPTION HAPPENED */

    UNUSED_VARIABLE( lr );

    /* Software interrupt triggered without any configured handler */
    EXCEPTION_HANDLER_BREAKPOINT();
    while (1)
    {
        /* Loop forever */
    }
}

static void default_reserved_interrupt_handler( void )
{
    uint32_t lr = get_LR(); /* <----- CHECK THIS FOR WHERE EXCEPTION HAPPENED */

    UNUSED_VARIABLE( lr );

    /* Reserved interrupt triggered - This should never happen! */
    EXCEPTION_HANDLER_BREAKPOINT();
    while (1)
    {
        /* Loop forever */
    }
}

static void prefetch_abort_handler( void )
{
    uint32_t lr               = get_LR(); /* <----- CHECK THIS FOR WHERE EXCEPTION HAPPENED */
    uint32_t ifar             = get_IFAR(); /* <----- CHECK THIS FOR ADDRESS OF INSTRUCTION */
    cr4_fault_status_t status = (cr4_fault_status_t) (get_IFSR( ) & CR4_FAULT_STATUS_MASK);  /* <----- CHECK THIS FOR STATUS */

    UNUSED_VARIABLE( lr );
    UNUSED_VARIABLE( ifar );

    /* Prefetch abort occurred */

    /* This means debug event, like breakpoint instruction and no JTAG debugger attached. */
    if ( status == CR4_FAULT_STATUS_DEBUG_EVENT )
    {
        if ( !breakpoint_inside_exception )
        {
            EXCEPTION_HANDLER_DUMP( "Debug event (e.g. breakpoint)" );
        }
        while ( 1 )
        {
            platform_exception_debug();
        }
    }

    /* This means that the processor attempted to execute an instruction
     * which was marked invalid due to a memory access failure (i.e. an abort)
     *
     *  - permission fault indicated by the MPU
     *  - an error response to a transaction on the AXI memory bus
     *  - an error detected in the data by the ECC checking logic.
     *
     */
    EXCEPTION_HANDLER_BREAKPOINT();

    platform_backplane_debug(); /* Step here to check backplane wrappers if timed out because of bus hang */

    __asm__ __volatile__ ( "MOV LR, %0; SUBS PC, LR, #4" : : "r"(lr) );  /* Step here to return to the instruction which caused the prefetch abort */
}

static void data_abort_handler( void )
{
    uint32_t lr               = get_LR(); /* <----- CHECK THIS FOR WHERE EXCEPTION HAPPENED */
    uint32_t dfar             = get_DFAR(); /* <----- CHECK THIS FOR ADDRESS OF DATA ACCESS */
    cr4_fault_status_t status = (cr4_fault_status_t) (get_DFSR( ) & CR4_FAULT_STATUS_MASK);  /* <----- CHECK THIS FOR STATUS */

    UNUSED_VARIABLE( lr );
    UNUSED_VARIABLE( dfar );
    UNUSED_VARIABLE( status );

    /* Data abort occurred */

    /* This means that the processor attempted a data access which
     * caused a memory access failure (i.e. an abort)
     *
     *  - permission fault indicated by the MPU
     *  - an error response to a transaction on the AXI memory bus
     *  - an error detected in the data by the ECC checking logic.
     *
     */
    EXCEPTION_HANDLER_BREAKPOINT();

    platform_backplane_debug(); /* Step here to check backplane wrappers if timed out because of bus hang */

    __asm__ __volatile__ ( "MOV LR, %0; SUBS PC, LR, #8" : : "r"(lr) );  /* If synchronous abort, Step here to return to the instruction which caused the data abort */
}

static void undefined_instruction_handler( void )
{
    uint32_t lr = get_LR(); /* <----- CHECK THIS FOR WHERE EXCEPTION HAPPENED */

    UNUSED_VARIABLE( lr );

    /* Undefined instruction exception occurred */
    EXCEPTION_HANDLER_BREAKPOINT();

    __asm__ __volatile__ ( "MOV LR, %0; MOVS PC, LR" : : "r"(lr) );  /* Step here to return to the instruction which caused the undefined instruction exception */
}

#endif /* DEBUG */
