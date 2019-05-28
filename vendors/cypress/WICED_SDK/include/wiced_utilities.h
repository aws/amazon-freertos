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
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "platform_toolchain.h"
#include "wiced_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file Wiced helper functions
 *
 *  WICED Utility libraries and helper functions for commonly used tasks.
 */

/******************************************************
 *                      Macros
 ******************************************************/

#ifdef LINT
/* Lint does not know about inline functions */
extern uint16_t htobe16(uint16_t v);
extern uint32_t htobe32(uint32_t v);

#else /* ifdef LINT */

#if defined(WIN32) && !defined(ALWAYS_INLINE)
#define ALWAYS_INLINE_PRE
#define ALWAYS_INLINE
#endif

#ifndef htobe16   /* This is defined in POSIX platforms */
ALWAYS_INLINE_PRE static inline ALWAYS_INLINE uint16_t htobe16(uint16_t v)
{
    return (uint16_t)(((v&0x00FF) << 8) | ((v&0xFF00)>>8));
}
#endif /* ifndef htobe16 */

#ifndef htobe32   /* This is defined in POSIX platforms */
ALWAYS_INLINE_PRE static inline ALWAYS_INLINE uint32_t htobe32(uint32_t v)
{
    return (uint32_t)(((v&0x000000FF) << 24) | ((v&0x0000FF00) << 8) | ((v&0x00FF0000) >> 8) | ((v&0xFF000000) >> 24));
}
#endif /* ifndef htobe32 */

#endif /* ifdef LINT */

#ifndef MIN
#define MIN(x,y)  ((x) < (y) ? (x) : (y))
#endif /* ifndef MIN */

#ifndef MAX
#define MAX(x,y)  ((x) > (y) ? (x) : (y))
#endif /* ifndef MAX */

#ifndef ROUND_UP
#define ROUND_UP(x,y)    ((x) % (y) ? (x) + (y)-((x)%(y)) : (x))
#endif /* ifndef ROUND_UP */

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(m, n)    (((m) + (n) - 1) / (n))
#endif /* ifndef DIV_ROUND_UP */

/**
 * Macro to verify that result value of function x is WICED_SUCCESS
 * if not, return with result value;
 */
#define WICED_VERIFY(x)                               {wiced_result_t res = (wiced_result_t)(x); if (res != WICED_SUCCESS){return res;}}

/**
 * Verify the expression, if the result is not WICED_SUCCESS, jump to the label
 */
#define WICED_VERIFY_GOTO( expr, res_var, label )     {res_var = (expr); if (res_var != WICED_SUCCESS){goto label;}}

/**
 * Memory concatenation
 *  Copy source to destination, return pointer to the end of the copy
 */
#define MEMCAT(destination, source, source_length)    (void*)((uint8_t*)memcpy((destination),(source),(source_length)) + (source_length))

/**
 * Macro to allocate named memory of sizeof(object_type)
 */
#define MALLOC_OBJECT(name,object_type)               ((object_type*)malloc_named(name,sizeof(object_type)))

/**
 * Get the offset (in bytes) of a member within a structure
 */
#define OFFSET(type, member)                          ((uint32_t)&((type *)0)->member)

/**
 * determine size (number of elements) in an array
 */
#define ARRAY_SIZE(a)                                ( sizeof(a) / sizeof(a[0]) )

/**
 * Macro to determine the element index in an array from the element address
 */
#define ARRAY_POSITION( array, element_pointer )     ( ((uint32_t)element_pointer - (uint32_t)array) / sizeof(array[0]) )

/**
 *  Macro for comparing MAC addresses
 */
#define CMP_MAC( a, b )  (((((unsigned char*)a)[0])==(((unsigned char*)b)[0]))&& \
                          ((((unsigned char*)a)[1])==(((unsigned char*)b)[1]))&& \
                          ((((unsigned char*)a)[2])==(((unsigned char*)b)[2]))&& \
                          ((((unsigned char*)a)[3])==(((unsigned char*)b)[3]))&& \
                          ((((unsigned char*)a)[4])==(((unsigned char*)b)[4]))&& \
                          ((((unsigned char*)a)[5])==(((unsigned char*)b)[5])))

/**
 *  Macro for checking for NULL MAC addresses
 */
#define NULL_MAC( a )  (((((unsigned char*)a)[0])==0)&& \
                        ((((unsigned char*)a)[1])==0)&& \
                        ((((unsigned char*)a)[2])==0)&& \
                        ((((unsigned char*)a)[3])==0)&& \
                        ((((unsigned char*)a)[4])==0)&& \
                        ((((unsigned char*)a)[5])==0))


#define MEMORY_BARRIER_AGAINST_COMPILER_REORDERING()  __asm__ __volatile__ ("" : : : "memory") /* assume registers are Device memory, so have implicit CPU memory barriers */

#define REGISTER_WRITE_WITH_BARRIER( type, address, value ) do {*(volatile type *)(address) = (type)(value); MEMORY_BARRIER_AGAINST_COMPILER_REORDERING();} while (0)
#define REGISTER_READ( type, address )                      (*(volatile type *)(address))

/**
 * If condition is not true, jump to label
 */
#define wiced_jump_when_not_true( condition, label ) \
    do \
    { \
        if( ( condition ) == 0 ) \
        { \
            goto label; \
        } \
    } while(0)

/**
 * If condition is not true, perform action, then jump to label
 */
#define wiced_action_jump_when_not_true( condition, jump_label, action ) \
    do \
    { \
        if( ( condition ) == 0 ) \
        { \
            { action; } \
            goto jump_label; \
        } \
    } while(0)


#define wiced_htol16_ua_store( val, bytes ) \
    do \
    { \
        uint16_t _val = (val); \
        uint8_t* _bytes = (uint8_t* )(bytes); \
        _bytes[0] = (uint8_t)((uint16_t)_val & 0xff); \
        _bytes[1] = (uint8_t)((uint16_t)_val >> 8); \
    } while(0)

/**
 * Macro to reverse bytes in a 16-bit value
 */
#define WICED_SWAP16(val) \
    ((uint16_t)((((uint16_t)(val) & (uint16_t)0x00ffU) << 8) | \
                (((uint16_t)(val) & (uint16_t)0xff00U) >> 8)))

/**
 * Macro to reverse bytes in a 32-bit value
 */
#define WICED_SWAP32(val) \
    ((uint32_t)((((uint32_t)(val) & (uint32_t)0x000000ffU) << 24) | \
                (((uint32_t)(val) & (uint32_t)0x0000ff00U) <<  8) | \
                (((uint32_t)(val) & (uint32_t)0x00ff0000U) >>  8) | \
                (((uint32_t)(val) & (uint32_t)0xff000000U) >> 24)))

/**
 * Macro to reverse the two 16-bit halves of a 32-bit value
 */
#define WICED_SWAP32BY16(val) \
        ((uint32_t)((((uint32_t)(val) & (uint32_t)0x0000ffffU) << 16) | \
                    (((uint32_t)(val) & (uint32_t)0xffff0000U) >> 16)))

/**
 * Macro to change a nibble to a hex ascii character
 */
#define wiced_hex_nibble_to_ascii_char( cur_char ) (uint8_t)( (cur_char) < 10  ? ( (uint8_t)'0' +  (cur_char) ) : ( (uint8_t)'a' + ( (cur_char) - 10 ) ) )

/**
 * Macro to perform XOR Swap
 */
#define WICED_XOR_SWAP( val1, val2 )               do { val1 = val1 ^ val2; val2 = val1 ^ val2; val1 = val2 ^ val1; } while ( 0 )


/**
 * Memory Leak Check types
 */
typedef enum
{
    LEAK_CHECK_THREAD,  /**< Thread scope */
    LEAK_CHECK_GLOBAL,  /**< Global scope */
} leak_check_scope_t;

/**
 * Memory use debugging
 */
#ifdef WICED_ENABLE_MALLOC_DEBUG
#include "malloc_debug.h"
extern void malloc_print_mallocs           ( void );
#else
#define calloc_named( name, nelems, elemsize) calloc ( nelems, elemsize )
#define calloc_named_hideleak( name, nelems, elemsize )  calloc ( nelems, elemsize )
#define realloc_named( name, ptr, size )      realloc( ptr, size )
#define malloc_named( name, size )            malloc ( size )
#define malloc_named_hideleak( name, size )   malloc ( size )
#define malloc_set_name( name )
#define malloc_leak_set_ignored( global_flag )
#define malloc_leak_set_base( global_flag )
#define malloc_leak_check( thread, global_flag )
#define malloc_transfer_to_curr_thread( block )
#define malloc_transfer_to_thread( block, thread )
#define malloc_print_mallocs( void )
#define malloc_debug_startup_finished( )
#endif /* ifdef WICED_ENABLE_MALLOC_DEBUG */

/**
 *  Define macros to assist operation on host MCUs that require aligned memory access
 */
#ifndef WICED_HOST_REQUIRES_ALIGNED_MEMORY_ACCESS

#define WICED_MEMCPY(destination, source, size)   memcpy(destination, source, size)

#define WICED_WRITE_16( pointer, value )      (*((uint16_t*)pointer) = value)
#define WICED_WRITE_32( pointer, value )      (*((uint32_t*)pointer) = value)
#define WICED_READ_16( pointer )              *((uint16_t*)pointer)
#define WICED_READ_32( pointer )              *((uint32_t*)pointer)

#else /* WICED_HOST_REQUIRES_ALIGNED_MEMORY_ACCESS */

#define WICED_MEMCPY( destination, source, size )   mem_byte_cpy( destination, source, size )

#define WICED_WRITE_16( pointer, value )      do { ((uint8_t*)pointer)[0] = (uint8_t)value; ((uint8_t*)pointer)[1]=(uint8_t)(value>>8); } while(0)
#define WICED_WRITE_32( pointer, value )      do { ((uint8_t*)pointer)[0] = (uint8_t)value; ((uint8_t*)pointer)[1]=(uint8_t)(value>>8); ((uint8_t*)pointer)[2]=(uint8_t)(value>>16); ((uint8_t*)pointer)[3]=(uint8_t)(value>>24); } while(0)
#define WICED_READ_16( pointer )              (((uint8_t*)pointer)[0] + (((uint8_t*)pointer)[1] << 8))
#define WICED_READ_32( pointer )              (((uint8_t*)pointer)[0] + ((((uint8_t*)pointer)[1] << 8)) + (((uint8_t*)pointer)[2] << 16) + (((uint8_t*)pointer)[3] << 24))

#endif /* WICED_HOST_REQUIRES_ALIGNED_MEMORY_ACCESS */

/******************************************************
 *                    Constants
 ******************************************************/

/**
 * Semaphore wait time constants
 */
#define WICED_NEVER_TIMEOUT                         (0xFFFFFFFF)
#define WICED_WAIT_FOREVER                          (0xFFFFFFFF)
#define WICED_NO_WAIT                               (0)

/**
 * Maximum digits after decimal point for float output to string
 */
#define FLOAT_TO_STRING_MAX_FRACTION_SUPPORTED      (6)

/**
 * Size of ascii printable string for a 6 octet ethernet address (including ending NULL)
 */
#define WICED_ETHER_ADDR_STR_LEN                    (18)
#define WICED_ETHER_ADDR_LEN                        ( 6)

/******************************************************
 *                   Enumerations
 ******************************************************/

/**
 * WEP key format
 */
typedef enum
{
    WEP_KEY_ASCII_FORMAT,   /**< ASCII format */
    WEP_KEY_HEX_FORMAT,     /**< HEX Format    */
} wep_key_format_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/
/*****************************************************************************/
/**
 *
 *  @addtogroup helper     Helper functions
 *  @ingroup utilities
 *
 * This library implements helper functions for commonly used tasks.
 *
 *  @{
 */
/*****************************************************************************/

/******************************************************
 *                 Function Declarations
 ******************************************************/

/**
 * Converts a decimal/hexidecimal string to an unsigned long int
 * Better than strtol or atol or atoi because the return value indicates if an error occurred
 *
 * @param string[in]     : The string buffer to be converted
 * @param str_length[in] : The maximum number of characters to process in the string buffer
 * @param value_out[out] : The unsigned in that will receive value of the the decimal string
 * @param is_hex[in]     : 0 = Decimal string, 1 = Hexidecimal string
 *
 * @return the number of characters successfully converted.  i.e. 0 = error
 *
 */
uint8_t string_to_unsigned( const char* string, uint8_t str_length, uint32_t* value_out, uint8_t is_hex );

/**
 * Converts a unsigned 64-bit integer (long long) to a decimal string
 *
 * @param value[in]      : The unsigned 64-bit integer (long long) to be converted.
 * @param output[out]    : The buffer which will receive the decimal string. A terminating 'null' is added. Ensure that there is space in the buffer for this.
 * @param min_length[in] : The minimum number of characters to output (zero padding will apply if required).
 * @param max_length[in] : The maximum number of characters to output. The max number of characters it can have is of the length of (ULLONG_MAX + 1).
 *
 * @return the number of characters returned (excluding terminating null)
 *
 */
uint8_t unsigned64_to_decimal_string( uint64_t value, char* output, uint8_t min_length, uint8_t max_length );

/**
 * Converts a unsigned 32-bit long int to a decimal string
 *
 * @param value[in]      : The unsigned long to be converted.
 * @param output[out]    : The buffer which will receive the decimal string. A terminating 'null' is added. Ensure that there is space in the buffer for this.
 * @param min_length[in] : The minimum number of characters to output (zero padding will apply if required).
 * @param max_length[in] : The maximum number of characters to output. The max number of characters it can have is of the length of (ULONG_MAX + 1).
 *
 * @return the number of characters returned (excluding terminating null)
 *
 */
uint8_t unsigned_to_decimal_string( uint32_t value, char* output, uint8_t min_length, uint8_t max_length );

/*!
 ******************************************************************************
 * Convert a decimal or hexidecimal string to an integer.
 *
 * @param[in] str  The string containing the value.
 *
 * @return    The value represented by the string.
 */

uint32_t generic_string_to_unsigned( const char* str );

/**
 * Converts a decimal/hexidecimal string (with optional sign) to a signed long int
 * Better than strtol or atol or atoi because the return value indicates if an error occurred
 *
 * @param string[in]     : The string buffer to be converted
 * @param str_length[in] : The maximum number of characters to process in the string buffer
 * @param value_out[out] : The unsigned in that will receive value of the the decimal string
 * @param is_hex[in]     : 0 = Decimal string, 1 = Hexidecimal string
 *
 * @return the number of characters successfully converted (including sign).  i.e. 0 = error
 *
 */
uint8_t string_to_signed( const char* string, uint16_t str_length, int32_t* value_out, uint8_t is_hex );

/**
 * Converts a signed 64-bit integer (long long) to a decimal string
 *
 * @param value[in]      : The signed 64-bit integer (long long) to be converted.
 * @param output[out]    : The buffer which will receive the decimal string. A terminating 'null' is added. Ensure that there is space in the buffer for this.
 * @param min_length[in] : The minimum number of characters to output (zero padding will apply if required).
 * @param max_length[in] : The maximum number of characters to output. The max number of characters it can have is of the length of (ULLONG_MAX + 1).
 *
 * @return the number of characters returned (excluding terminating null)
 *
 */
uint8_t signed64_to_decimal_string( int64_t value, char* output, uint8_t min_length, uint8_t max_length );

/**
 * Converts a signed 32-bit long int to a decimal string
 *
 * @param value[in]      : The signed 32-bit long int to be converted
 * @param output[out]    : The buffer which will receive the decimal string. A terminating 'null' is added. Ensure that there is space in the buffer for this.
 * @param min_length[in] : The minimum number of characters to output (zero padding will apply if required)
 * @param max_length[in] : The maximum number of characters to output. The max number of characters it can have is of the length of (ULONG_MAX + 1).
 *
 * @return the number of characters returned (excluding terminating null)
 *
 */
uint8_t signed_to_decimal_string( int32_t value, char* output, uint8_t min_length, uint8_t max_length );

/**
 * Converts a unsigned 32-bit long int to a hexidecimal string
 *
 * @param value[in]      : The unsigned 32-bit long int to be converted
 * @param output[out]    : The buffer which will receive the hexidecimal string. A terminating 'null' is added. Ensure that there is space in the buffer for this.
 * @param min_length[in] : The minimum number of characters to output (zero padding will apply if required)
 * @param max_length[in] : The maximum number of characters to output. The max number of characters it can have is of the length of (ULONG_MAX + 1).
 *
 * @note: No leading '0x' is added.
 *
 * @return the number of characters returned (excluding terminating null)
 *
 */
uint8_t unsigned_to_hex_string( uint32_t value, char* output, uint8_t min_length, uint8_t max_length );


/**
 * Verifies the provided string is a collection of digits.
 *
 * @param str[in]        : The string to verify
 *
 * @return 1 if string is valid digits, 0 otherwise
 *
 */
int is_digit_str( const char* str );

/**
 ******************************************************************************
 * Convert a nibble into a hex character
 *
 * @param[in] nibble  The value of the nibble in the lower 4 bits
 *
 * @return    The hex character corresponding to the nibble
 */
ALWAYS_INLINE_PRE static inline ALWAYS_INLINE char nibble_to_hexchar( uint8_t nibble )
{
    if (nibble > 9)
    {
        return (char)('A' + (nibble - 10));
    }
    else
    {
        return (char) ('0' + nibble);
    }
}


/**
 ******************************************************************************
 * Convert an ASCII hex character into a nibble
 *
 * @param[in]  char     : The single hex character to convert to a nibble
 * @param[out] nibble   : Pointer to store The value of the nibble in the lower 4 bits
 *
 * @return    0 = SUCCESS
 *           -1 = not a hex character
 */
ALWAYS_INLINE_PRE static inline ALWAYS_INLINE char hexchar_to_nibble( char hexchar, uint8_t* nibble )
{
    if ( ( hexchar >= '0' ) && ( hexchar <= '9' ) )
    {
        *nibble = (uint8_t)( hexchar - '0' );
        return 0;
    }
    else if ( ( hexchar >= 'A' ) && ( hexchar <= 'F' ) )
    {
        *nibble = (uint8_t) ( hexchar - 'A' + 10 );
        return 0;
    }
    else if ( ( hexchar >= 'a' ) && ( hexchar <= 'f' ) )
    {
        *nibble = (uint8_t) ( hexchar - 'a' + 10 );
        return 0;
    }
    return -1;
}

/**
 ******************************************************************************
 * Append the two character hex value of a byte to a string
 *
 * @note: no terminating null is added
 *
 * @param[out] string  The buffer which will receive the two bytes
 * @param[in]  byte    The byte which will be converted to hex
 *
 * @return    A pointer to the character after the two hex characters added
 */
ALWAYS_INLINE_PRE static inline ALWAYS_INLINE char* string_append_two_digit_hex_byte( char* string, uint8_t byte )
{
    *string = nibble_to_hexchar( ( byte & 0xf0 ) >> 4 );
    string++;
    *string = nibble_to_hexchar( ( byte & 0x0f ) >> 0 );
    string++;
    return string;
}

/**
 ******************************************************************************
 * Convert WEP security key to the format used by WICED
 *
 * @param[out]    wep_key_ouput   The converted key
 * @param[in]     wep_key_data    The WEP key to convert
 * @param[in,out] wep_key_length  The length of the WEP key data. Upon return, the length of the converted WEP key
 * @param[in]     wep_key_format  The current format of the WEP key
 */
void format_wep_keys( char* wep_key_output, const char* wep_key_data, uint8_t* wep_key_length, wep_key_format_t wep_key_format );

/**
 * Length limited version of strstr. Ported from bcmutils.c
 *
 * @param s[in]             : The string to be searched.
 * @param s_len[in]         : The length of the string to be searched.
 * @param substr[in]        : The string to be found.
 * @param substr_len[in]    : The length of the string to be found.
 *
 * @return    pointer to the found string if search successful, otherwise NULL
 */
char* strnstrn( const char* s, uint16_t s_len, const char* substr, uint16_t substr_len );

/**
 * Length limited version of strcasestr. Ported from bcmutils.c
 *
 * @param s[in]             : The string to be searched.
 * @param s_len[in]         : The length of the string to be searched.
 * @param substr[in]        : The string to be found.
 * @param substr_len[in]    : The length of the string to be found.
 *
 * @return    pointer to the found string if search successful, otherwise NULL
 */
char* strncasestr( const char* s, uint16_t s_len, const char* substr, uint16_t substr_len );

/**
 ******************************************************************************
 * Compare a string to a pattern containing wildcard character(s).
 *
 * @note: The following wildcard characters are supported:
 *        \li '*' for matching zero or more characters
 *        \li '?' for matching exactly one character
 *
 * @param[in] string   The target string to compare with with the pattern
 * @param[in] length   The length of the target string
 * @param[in] pattern  The NUL-terminated string pattern which contains wildcard character(s)
 *
 * @return    1 if the string matches the pattern; 0 otherwise.
 */
uint8_t match_string_with_wildcard_pattern( const char* string, uint32_t length, const char* pattern );

/**
 ******************************************************************************
 * Convert ether address to a printable string
 *
 * @param[in] ea         Ethernet address to convert
 * @param[in] buf        Buffer to write the ascii string into
 * @param[in] buf_len  Length of the memory buf points to
 *
 * @return                   Pointer to buf if successful; "" if not successful due to buffer too short
 */
char* wiced_ether_ntoa( const uint8_t *ea, char *buf, uint8_t buf_len );

/**
 * Converts double precision floating point value to string
 *
 * @param buffer[in/out]       : Pointer to buffer where the output string to be stored.
 * @param buffer_len[in]       : Length of the buffer used for storing the output.
 * @param value[in]            : Double floating point value to be converted to a string.
 * @param resolution[in]       : Floating point resolution for the conversion. Supported max upto FLOAT_TO_STRING_MAX_FRACTION_SUPPORTED.
 *
 * @return    Number of char printed in buffer. On error, returns 0.
 *
 */
uint8_t double_to_string ( char* buffer, uint8_t buffer_len, double value, uint8_t resolution  );

/**
 * Converts floating point value to string
 *
 * @param buffer[in/out]       : Pointer to buffer where the output string to be stored.
 * @param buffer_len[in]       : Length of the buffer used for storing the output.
 * @param value[in]            : Floating point value to be converted to a string.
 * @param resolution[in]       : Floating point resolution for the conversion. Supported max upto FLOAT_TO_STRING_MAX_FRACTION_SUPPORTED.
 *
 * @return    Number of char printed in buffer. On error, returns 0.
 *
 */
uint8_t float_to_string ( char* buffer, uint8_t buffer_len, float value, uint8_t resolution  );

/*
 ******************************************************************************
 * Translate string in hex format E.g. "0xfffffffffffffff" or "ffffedfdfe" to equivalent binary format
 *
 * @param     ascii_buffer                String with hex value.
 * @param     buffer                        Write the hex value represented in the string into this buffer in binary format.
 * @param     buffer_length              Max length of the buffer.
 *
* @return    Number of uint8_t filled out in buffer. On error, returns 0.
 */
uint32_t wiced_ascii_to_hex( const char *ascii_buffer, uint8_t *buffer, uint32_t max_buffer_length );


/*
 ******************************************************************************
 * Wiced API to set iovar value
 *
 * @param     iovar                      iovar string
 * @param     value                      uint32 value to be set iovar.
 * @param     interface                  interface type
 *
 * @return @ref wiced_result_t
 */

extern wiced_result_t wiced_wifi_set_iovar_value ( const char* iovar, uint32_t value, wiced_interface_t interface );

/*
 ******************************************************************************
 * Wiced API to get iovar value
 *
 * @param     iovar                      iovar string
 * @param     buffer                     uint32_t buffer to get iovar value from firmware
 * @param     interface                  interface type
 *
 * @return @ref wiced_result_t
 */

extern wiced_result_t wiced_wifi_get_iovar_value ( const char* iovar, uint32_t* value, wiced_interface_t interface );


/** @} */

#ifdef __cplusplus
} /*extern "C" */
#endif
