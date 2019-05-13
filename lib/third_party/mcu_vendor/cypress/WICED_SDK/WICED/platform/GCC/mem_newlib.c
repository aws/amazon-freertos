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

/*
 * @file
 * Interface functions for Newlib libC implementation
 */


#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <malloc.h>
#include "platform_toolchain.h"
#include <wwd_assert.h>
#include <wwd_constants.h>

#ifdef LINT  /* Some defines to keep lint happy in the absense of newlib */
typedef char *  caddr_t;
struct mallinfo { int x; };
/*@-declundef@*/ /*@-exportheader@*/ extern struct mallinfo mallinfo(void); /*@+declundef@*/ /*@+exportheader@*/
#endif /* ifdef LINT */

/*@-declundef@*/ /*@-exportheadervar@*/ /* Lint: These variables are defined by the linker or by newlib */

#undef errno
extern int errno;


/* sbrk
 * Increase program data space.
 * Malloc and related functions depend on this
 */
extern unsigned char _heap[];
extern unsigned char _eheap[];
/*@+declundef@*/ /*@+exportheadervar@*/

/*static*/ /*@shared@*/ unsigned char *sbrk_heap_top = _heap;

WEAK NEVER_INLINE void platform_toolchain_sbrk_prepare( void* ptr, int incr )
{
}

/*@-exportheader@*/ /* Lint: These functions are used by newlib, but it does not provide a header */

/*@shared@*/ caddr_t _sbrk( int incr )
{
    unsigned char *prev_heap;

    if ( sbrk_heap_top + incr > _eheap )
    {
        /* Out of dynamic memory heap space */

        volatile struct mallinfo mi = mallinfo();

        // See variable mi for malloc information:
        // Total allocated :  mi.uordblks
        // Total free :       mi.fordblks

        wiced_assert("Out of dynamic memory heap space", 0 != 0 );

        UNUSED_VARIABLE( mi );

        errno = ENOMEM;
        return (caddr_t) -1;
    }
    prev_heap = sbrk_heap_top;

    sbrk_heap_top += incr;

    platform_toolchain_sbrk_prepare( prev_heap, incr );

    return (caddr_t) prev_heap;
}

/* Override the default Newlib assert, since it tries to do printf stuff */

void __assert_func( const char * file, int line, const char * func, const char * failedexpr )
{
    /* Assertion failed!
     *
     * To find out where this assert was triggered, either look up the call stack,
     * or inspect the file, line and function parameters
     */
    wiced_assert("newlib assert", 0 != 0 );

    UNUSED_PARAMETER( file );
    UNUSED_PARAMETER( line );
    UNUSED_PARAMETER( func );
    UNUSED_PARAMETER( failedexpr );
}

/*
 * These are needed for C++ programs. They shouldn't really be here, so let's just
 * hit a breakpoint when these functions are called.
 */

#if 1

int _kill( int pid, int sig )
{
    wiced_assert("", 0 != 0 );

    UNUSED_PARAMETER( pid );
    UNUSED_PARAMETER( sig );
    return 0;
}

int _getpid( void )
{
    wiced_assert("", 0 != 0 );
    return 0;
}

/*@+exportheader@*/

#endif

/* Search memory in reverse for the last instance of a character in a buffer*/
void *memrchr( const void *source_buffer, int search_character, size_t buffer_length )
{
    unsigned char * read_pos = ((unsigned char *)source_buffer + buffer_length);
    while ( ( *read_pos != (unsigned char) search_character ) &&
            ( read_pos >= (unsigned char*) source_buffer ) )
    {
        read_pos--;
    }
    return ( read_pos >= (unsigned char*) source_buffer )?read_pos:NULL;
}

