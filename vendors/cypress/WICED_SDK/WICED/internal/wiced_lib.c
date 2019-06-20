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

/** @file
 *
 */

#include "wiced_utilities.h"
#include <string.h>
#include "wiced_resource.h"
#include "wwd_debug.h"
#include "platform_resource.h"
#include <limits.h>

/******************************************************
 *                      Macros
 ******************************************************/

#define IS_DIGIT(c) ((c >= '0') && (c <= '9'))
#define ULLONG_MIN_STR      "00000000000000000000"
#define ULLONG_MAX_STR      "18446744073709551615"
#define ULONG_MAX_STR       "4294967295"
#define ULONG_MIN_STR       "0000000000"

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

static uint8_t string_to_generic( const char* string, uint16_t str_length,  uint32_t* value_out, uint8_t is_unsigned, uint8_t is_hex )
{
    uint8_t nibble;
    uint8_t characters_processed = 0;

    if ( string == NULL )
    {
        return 0;
    }

    *value_out = 0;

    while ( ( characters_processed != str_length ) &&
            ( 0 == hexchar_to_nibble( *string, &nibble ) ) &&
            ( ( is_hex != 0 ) || ( nibble < 10 ) )
          )
    {
        if ( is_hex != 0 )
        {
            if ( ( ( *value_out > ( 0x7fffffff >> 4 ) ) && ( is_unsigned == 0 ) ) ||
                 ( *value_out > ( 0xffffffff >> 4 ) )
               )
            {
                break;
            }
            *value_out = ( *value_out << 4 ) + nibble;
        }
        else
        {
            if ( ( ( *value_out > ( 0x7fffffff / 10 ) ) && ( is_unsigned == 0 ) ) ||
                 ( *value_out > ( 0xffffffff / 10 ) )
               )
            {
                break;
            }
            *value_out = ( *value_out * 10 ) + nibble;
        }
        string++;
        characters_processed++;
    }

    return characters_processed;
}

/*!
 ******************************************************************************
 * Convert a decimal or hexidecimal string to an integer.
 *
 * @param[in] str  The string containing the value.
 *
 * @return    The value represented by the string.
 */
uint32_t generic_string_to_unsigned( const char* str )
{
    uint32_t val = 0;
    uint8_t is_hex = 0;

    if ( strncmp( str, "0x", 2 ) == 0 )
    {
        is_hex = 1;
        str += 2;
    }

    string_to_unsigned( str, (uint8_t)strlen(str), &val, is_hex );

    return val;
}

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
uint8_t string_to_signed( const char* string, uint16_t str_length, int32_t* value_out, uint8_t is_hex )
{
    uint8_t characters_processed = 0;
    uint8_t retval;
    char    first_char;

    if ( string == NULL )
    {
        return 0;
    }

    first_char = *string;

    if ( ( first_char == '-' ) || ( *string == '+' ) )
    {
        characters_processed++;
        string++;
        str_length--;
    }

    retval = string_to_generic( string, str_length, (uint32_t*)value_out, 0, is_hex );
    if ( retval == 0 )
    {
        return 0;
    }

    if ( first_char == '-' )
    {
        *value_out = -(*value_out);
    }
    return (uint8_t) ( characters_processed + retval );
}

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
uint8_t string_to_unsigned( const char* string, uint8_t str_length, uint32_t* value_out, uint8_t is_hex )
{
    return string_to_generic( string, str_length,  value_out, 1, is_hex );
}

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
uint8_t unsigned64_to_decimal_string( uint64_t value, char* output, uint8_t min_length, uint8_t max_length )
{
    uint8_t digits_left;
    char buffer[sizeof(ULLONG_MAX_STR) + 1] = ULLONG_MIN_STR;  /* Buffer for storing ULLONG_MAX with +1 for the sign */

    if ( output == NULL )
    {
        return 0;
    }

    max_length = (uint8_t) MIN( max_length, sizeof( buffer ) );
    digits_left = max_length;

    do
    {
        --digits_left;
        buffer[ digits_left ] = (char) (( value % 10 ) + '0');
        value = value / 10;
    } while ( ( value != 0 ) && ( digits_left != 0 ) );

    digits_left = (uint8_t) MIN( ( max_length - min_length ), digits_left );
    memcpy( output, &buffer[ digits_left ], (size_t)( max_length - digits_left ) );

    /* Add terminating null */
    output[( max_length - digits_left )] = '\x00';

    return (uint8_t) ( max_length - digits_left );
}

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
uint8_t signed64_to_decimal_string( int64_t value, char* output, uint8_t min_length, uint8_t max_length )
{
    uint8_t retval = 0;

    if ( ( value < 0 ) && ( max_length > 0 ) && ( output != NULL ) )
    {
        *output = '-';
        output++;
        max_length--;
        value = -value;
        retval++;
    }

    retval = (uint8_t) ( retval + unsigned64_to_decimal_string( (uint64_t)value, output, min_length, max_length ) );
    return retval;
}

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
uint8_t unsigned_to_decimal_string( uint32_t value, char* output, uint8_t min_length, uint8_t max_length )
{
    uint8_t digits_left;
    char buffer[sizeof(ULONG_MAX_STR) + 1] = ULONG_MIN_STR; /* Buffer for storing ULONG_MAX with +1 for the sign */

    if ( output == NULL )
    {
        return 0;
    }

    max_length = (uint8_t) MIN( max_length, sizeof( buffer ) );
    digits_left = max_length;
    do
    {
        --digits_left;
        buffer[ digits_left ] = (char) (( value % 10 ) + '0');
        value = value / 10;
    } while ( ( value != 0 ) && ( digits_left != 0 ) );

    digits_left = (uint8_t) MIN( ( max_length - min_length ), digits_left );
    memcpy( output, &buffer[ digits_left ], (size_t)( max_length - digits_left ) );

    /* Add terminating null */
    output[( max_length - digits_left )] = '\x00';

    return (uint8_t) ( max_length - digits_left );
}

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
uint8_t signed_to_decimal_string( int32_t value, char* output, uint8_t min_length, uint8_t max_length )
{
    uint8_t retval = 0;

    if ( ( value < 0 ) && ( max_length > 0 ) && ( output != NULL ) )
    {
        *output = '-';
        output++;
        max_length--;
        value = -value;
        retval++;
    }

    retval = (uint8_t) ( retval + unsigned_to_decimal_string( (uint32_t)value, output, min_length, max_length ) );
    return retval;
}

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
uint8_t unsigned_to_hex_string( uint32_t value, char* output, uint8_t min_length, uint8_t max_length )
{
    uint8_t digits_left;
    char buffer[sizeof(ULONG_MAX_STR) + 1] = ULONG_MIN_STR; /* Buffer for ULONG_MAX with +1 for storing the sign */
    max_length = (uint8_t) MIN( max_length, sizeof( buffer ) );
    digits_left = max_length;
    while ( ( value != 0 ) && ( digits_left != 0 ) )
    {
        --digits_left;
        buffer[ digits_left ] = nibble_to_hexchar( value & 0x0000000F );
        value = value >> 4;
    }

    digits_left = (uint8_t) MIN( ( max_length - min_length ), digits_left );
    memcpy( output, &buffer[ digits_left ], (size_t)( max_length - digits_left ) );

    /* Add terminating null */
    output[( max_length - digits_left )] = '\x00';

    return (uint8_t) ( max_length - digits_left );
}

int is_digit_str( const char* str )
{
    int res = 0;
    int i = 0;

    if ( str != NULL )
    {
        i = (int)strlen(str);
        res = 1;
        while ( i > 0 )
        {
            if ( !IS_DIGIT(*str) )
            {
                res = 0;
                break;
            }
            str++;
            i--;
        }
    }

    return res;
}

uint8_t match_string_with_wildcard_pattern( const char* string, uint32_t length, const char* pattern )
{
    uint32_t current_string_length = length;
    uint32_t temp_string_length    = 0;
    char*    current_string        = (char*)string;
    char*    current_pattern       = (char*)pattern;
    char*    temp_string           = NULL;
    char*    temp_pattern          = NULL;

    /* Iterate through string and pattern until '*' is found */
    while ( ( current_string_length != 0 ) && ( *current_pattern != '*' ) )
    {
        /* Current pattern is not equal current string and current pattern isn't a wildcard character */
        if ( ( *current_pattern != *current_string ) && ( *current_pattern != '?' ) )
        {
            return 0;
        }
        current_pattern++;
        current_string++;
        current_string_length--;
    }

    /* '*' is detected in pattern. Consume string until matching pattern is found */
    while ( current_string_length != 0 )
    {
        switch ( *current_pattern )
        {
            case '*':
                if ( *(++current_pattern) == '\0' )
                {
                    /* Last character in the pattern is '*'. Return successful */
                    return 1;
                }

                /* Store temp variables for starting another matching iteration when non-matching character is found. */
                temp_pattern       = current_pattern;
                temp_string_length = current_string_length - 1;
                temp_string        = current_string + 1;
                break;

            case '?':
                current_pattern++;
                current_string++;
                current_string_length--;
                break;

            default:
                if ( *current_pattern == *current_string )
                {
                    current_pattern++;
                    current_string++;
                    current_string_length--;
                }
                else
                {
                    current_pattern       = temp_pattern;
                    current_string        = temp_string++;
                    current_string_length = temp_string_length--;
                }
                break;
        }
    }

    while ( *current_pattern == '*' )
    {
        current_pattern++;
    }

    return ( *current_pattern == '\0' );
}

char* wiced_ether_ntoa( const uint8_t *ea, char *buf, uint8_t buf_len )
{
    const char hex[] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };
    char *output = buf;
    const uint8_t *octet = ea;

    if ( buf_len < WICED_ETHER_ADDR_STR_LEN )
    {
        if ( buf_len > 0 )
        {
            /* buffer too short */
            buf[0] = '\0';
        }
        return buf;
    }

    for ( ; octet != &ea[WICED_ETHER_ADDR_LEN] ; octet++) {
        *output++ = hex[(*octet >> 4) & 0xf];
        *output++ = hex[*octet & 0xf];
        *output++ = ':';
    }

    *(output-1) = '\0';

    return buf;
}

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
#ifndef WAC_AIRPLAY_NUTTX
char* strnstrn(const char *s, uint16_t s_len, const char *substr, uint16_t substr_len)
{
    for (; s_len >= substr_len; s++, s_len--)
    {
        if (strncmp(s, substr, substr_len) == 0)
        {
            return (char*)s;
        }
    }

    return NULL;
}

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
char* strncasestr(const char *s, uint16_t s_len, const char *substr, uint16_t substr_len)
{
    for (; s_len >= substr_len; s++, s_len--)
    {
        if (strncasecmp(s, substr, substr_len) == 0)
        {
            return (char*)s;
        }
    }

    return NULL;
}
#endif /* WAC_AIRPLAY_NUTTX */

/*
 ******************************************************************************
 * Translate string in hex format E.g. "0xfffffffffffffaa" or "ffffedfdaa" to equivalent binary format
 *
 * @param     ascii_buffer                String with hex value.
 * @param     buffer                        Write the hex value represented in the string into this buffer in binary format.
 *                                                   Note the value on the far right of the string ('aa') will end up in the lower byte of the buffer (buffer[0]).
 * @param     buffer_length              Max length of the buffer.
 *
* @return    Number of uint8_t filled out in buffer. On error, returns 0.
 */
uint32_t wiced_ascii_to_hex( const char *ascii_buffer, uint8_t *buffer, uint32_t buffer_length )
{
    /* define start and end positions of input pointer */
    const char *in_start = &ascii_buffer[strlen(ascii_buffer) - 1];
    const char *in_end   = ascii_buffer;
    /* define start and end (via tracking bytes written) of output buffer */
    uint32_t   out_chars = 0;

    /* adjust end input if needed for '0x' */
    if ( strlen(ascii_buffer) > 2 && strncmp( ascii_buffer, "0x", strlen("0x") ) == 0 )
    {
        in_end += 2;
    }

    /* while more input and more output */
    while ( in_start >= in_end && out_chars < buffer_length )
    {
        /* translate input chars to output byte */
        {
            char tmp[3] = { 0 };
            char *end   = NULL;
            tmp[0] = *in_start;
            in_start--;
            if ( in_start >= in_end )
            {
                /* shift in */
                tmp[1] = tmp[0];
                tmp[0] = *in_start;
                in_start--;
            }
            buffer[out_chars++] = (uint8_t)(strtoul( tmp, &end, 16));
            if ( end == NULL )
            {
                return 0;
            }
        }
    }

    /* if input not done and output done, return 0 */
    if ( in_start >= in_end && out_chars == buffer_length )
    {
        return 0;
    }

    /* return bytes written */
    return out_chars;
}

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
uint8_t double_to_string ( char* buffer, uint8_t buffer_len, double value, uint8_t resolution  )
{
    long double fraction;
    long double input = value;
    char fraction_buf[FLOAT_TO_STRING_MAX_FRACTION_SUPPORTED + 1];
    int32_t decimal;
    uint8_t ret_value, i;

    if ( ( buffer == NULL ) || (resolution > FLOAT_TO_STRING_MAX_FRACTION_SUPPORTED) )
    {
        return 0;
    }

    /* Extract integer part of the float value */
    decimal = (int32_t)input;

    /* Extract float part of the value */
    fraction = input - (int)input;
    for ( i = 0; i <= resolution; i++ )
    {
        fraction *= 10;
        fraction_buf[i] = (char)((int)fraction);
        fraction = fraction - (int)fraction;
    }

    /* Check if round off required */
    if (fraction_buf[resolution] > 4)
    {
        /* Do round off */
        for ( i = resolution; i > 0; i--)
        {
            fraction_buf[i - 1] = (char)(fraction_buf[i - 1] + 1);
            if ( fraction_buf[i - 1] <= 9)
            {
                break;
            }
        }

        /* All the fractional digits are rounded. Hence increment decimal by 1 */
        if ( i == 0 )
        {
            decimal += 1;
        }
    }

    /* Convert the fractions to characters */
    for ( i = 0; i < resolution; i++ )
    {
        if (fraction_buf[i] == 10)
        {
            fraction_buf[i] = '0';
        }
        else
        {
            fraction_buf[i] = (char)(fraction_buf[i] + '0');
        }
    }
    fraction_buf[i] = '\0';

    /* Prepare the decimal string */
    ret_value =  signed_to_decimal_string( decimal, buffer, 1, buffer_len );
    if ((ret_value  + 1 + resolution + 1) > buffer_len)
    {
        return 0;
    }
    buffer [ret_value] = '.';
    ret_value++;

    /* Prepare the fractional string */
    strcpy ( (buffer + ret_value), (char*)fraction_buf );

    return (uint8_t)(ret_value + resolution);
}

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
uint8_t float_to_string ( char* buffer, uint8_t buffer_len, float value, uint8_t resolution  )
{
    return double_to_string(buffer, buffer_len, value, resolution);
}
