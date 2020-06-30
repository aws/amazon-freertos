/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 */

#include <stddef.h>
#include <string.h>
#include "JSON.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define MAX_BACKUP_SIZE 500
#define MAX_PARENTS 4

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

cy_json_callback_t      internal_json_callback;
void                    *internal_json_callback_arg;
static cy_rslt_t validate_array_value( char* start, char* stop, uint8_t len );

/******************************************************
 *               Variable Definitions
 ******************************************************/

static char*               previous_token   = NULL;
static cy_json_object_t json_object =
{
    .object_string        = NULL,
    .object_string_length =                 0,
    .value_type           = UNKNOWN_JSON_TYPE,
    .value                = NULL,
    .value_length         =                 0,
    .parent_object        = NULL
};

static int32_t             parent_counter = 0;
static cy_json_object_t parent_json_object[ MAX_PARENTS ];

static int                  incomplete_response       = 0;

static int32_t             object_counter            = 0;

static char*               string_start              = NULL;
static char*               string_end                = NULL;

static char*               value_start               = NULL;
static char*               value_end                 = NULL;

static cy_JSON_types_t  type                      = UNKNOWN_JSON_TYPE;

static char*               current_input_token;
static char*               end_of_input;

static char*               most_recent_object_marker = NULL;

char                       packet_backup[ MAX_BACKUP_SIZE ];

uint32_t                   number_of_bytes_backed_up;

static int                 escape_token              = 0;
static int32_t             array_counter            = 0;

/******************************************************
 *               Function Definitions
 ******************************************************/
static cy_rslt_t validate_array_value( char* start, char* stop, uint8_t len )
{
    char*   temp = NULL;
    uint8_t e_count = 0;
    uint8_t dot = 0;
    uint8_t minus = 0;
    uint8_t plus = 0;

    /* TODO : Cases to be handled for example : e+-  e.+1  .e-+2  -1.234e-3  */

    if ( ( *start == 'f' ) && ( len == 5 ) )
    {
        if ( !strncmp( (const char*) start, "false", ( sizeof( "false" ) - 1 ) ) )
        {
            return CY_RSLT_SUCCESS;
        }
        else
        {
            return CY_RSLT_TYPE_ERROR;
        }
    }
    if ( ( *start == 't' ) && ( len == 4 ) )
    {
        if ( !strncmp( (const char*) start, "true", ( sizeof( "true" ) - 1 ) ) )
        {
            return CY_RSLT_SUCCESS;
        }
        else
        {
            return CY_RSLT_TYPE_ERROR;
        }
    }
    if ( ( *start == 'n' ) && ( len == 4 ) )
    {
        if ( !strncmp( (const char*) start, "null", ( sizeof( "null" ) - 1 ) ) )
        {
            return CY_RSLT_SUCCESS;
        }
        else
        {
            return CY_RSLT_TYPE_ERROR;
        }
    }

    temp = start;
    while ( temp <= stop )
    {
        if( *temp == ' ')
        {
            temp++;
            continue;
        }
        if( *temp == '\n')
        {
            temp++;
            continue;
        }
        if( *temp == '\r')
        {
            temp++;
            continue;
        }

        switch ( *temp )
        {
            case 'e':
                e_count++;
                break;

            case '+':
                plus++;
                break;

            case '-':
                minus++;
                break;

            case '.':
                dot++;
                break;

            default:
                break;
        }

        if ( !( ( ( *temp >= '0' ) && ( *temp <= '9' ) ) || ( *temp == 'e' ) || ( *temp == '-' ) || ( *temp == '.' ) || ( *temp == '+' ) ) )
        {
            return CY_RSLT_TYPE_ERROR;
        }

        if ( ( e_count > 1 ) || ( minus > 1 ) || ( plus > 1 ) || ( dot > 1 ) )
        {
            return CY_RSLT_TYPE_ERROR;
        }
        temp++;
    }

    return CY_RSLT_SUCCESS;
}


/* Register callbacks parser will use to populate fields*/
cy_rslt_t cy_JSON_parser_register_callback( cy_json_callback_t json_callback, void *json_cb_arg )
{
    internal_json_callback = json_callback;
    internal_json_callback_arg = json_cb_arg;

    return CY_RSLT_SUCCESS;
}


/* Get current callback */
cy_json_callback_t cy_JSON_parser_get_callback( void )
{
    return internal_json_callback;
}


cy_rslt_t cy_JSON_parser( const char* json_input, uint32_t input_length )
{
    cy_rslt_t valid_json_string         = CY_RSLT_SUCCESS;
    int            space = 0;

    if ( incomplete_response )
    {
        // If there is enough room on backup buffer to hold new data and old backed up
        // data, parse the backup buffer
        if ( ( input_length + number_of_bytes_backed_up ) < MAX_BACKUP_SIZE )
        {
            memcpy( packet_backup + number_of_bytes_backed_up, json_input, input_length );
            current_input_token = packet_backup;
            end_of_input = current_input_token + number_of_bytes_backed_up + input_length;
        }
        else
        {
            // We must drop entire json object from last buffer. Find start of new json object
            // in new packet, and being parsing from there
            while ( *json_input != '{' )
            {
                json_input++;
                input_length--;
            }
            current_input_token = (char*) json_input;
            end_of_input        = current_input_token + input_length;
        }

        incomplete_response = 0;
    }
    else
    {
        current_input_token = (char*) json_input;
        end_of_input        = current_input_token + input_length;
        previous_token = current_input_token;
    }

    while ( *( current_input_token ) == ' ' )
    {
        current_input_token++;
    }
    previous_token = current_input_token;

    if ( ( *( current_input_token ) == OBJECT_START_TOKEN ) || ( *( current_input_token ) == ARRAY_START_TOKEN ) )
    {
        /* Parse through entire input */
        while ( current_input_token < end_of_input )
        {
            switch ( *current_input_token )
            {
                /* This is a start of object token */
                case OBJECT_START_TOKEN:

                    if ( escape_token )
                    {
                        escape_token = 0;
                        break;
                    }

                    /* Skip in case we're parsing a string value */
                    if ( ( string_start != NULL ) && ( string_end == NULL ) )
                    {
                        break;
                    }

                    /* In case the json is split across packets, record the most recent object market
                     * and copy from this point forward
                     */
                    most_recent_object_marker = current_input_token;

                    /* Keep track of the number of objects open */
                    object_counter++;

                    type = JSON_OBJECT_TYPE;
                    /* If we have already captured some string value, then this string must represent the name of this object */
                    if ( string_end )
                    {
                        /* prepare JSON object. The object string was already piced up by the start of value token */
                        json_object.value_type = type;
                        json_object.value = NULL;
                        json_object.value_length = 0;

                        /* Reset the string and value pointers */
                        string_start = NULL;
                        string_end = NULL;
                        value_start = NULL;
                        value_end = NULL;
                    }

                    if ( ( ( json_object.value_type == JSON_ARRAY_TYPE ) || ( json_object.value_type == JSON_OBJECT_TYPE ) ) && ( parent_counter < MAX_PARENTS ) )
                    {
                        parent_json_object[ parent_counter ] = json_object;
                        json_object.parent_object = &parent_json_object[ parent_counter ];
                        parent_counter++;
                    }
                    else if ( *previous_token == COMMA_SEPARATOR )
                    {
                        json_object.parent_object = &parent_json_object[ parent_counter ];
                        parent_counter++;
                    }
                    previous_token = current_input_token;

                    break;

                /* This is an end of object token */
                case OBJECT_END_TOKEN:

                    if ( escape_token )
                    {
                        escape_token = 0;
                        break;
                    }

                    /* Skip in case we're parsing a string value */
                    if ( ( string_start != NULL ) && ( string_end == NULL ) )
                    {
                        break;
                    }

                    object_counter--;

                    if ( *( previous_token ) == COMMA_SEPARATOR )
                    {
                        if ( *( current_input_token ) == ( OBJECT_END_TOKEN ) )
                        {
                            valid_json_string = CY_RSLT_TYPE_ERROR;
                            object_counter = 0;
                            array_counter = 0;
                            return valid_json_string;
                        }
                    }
                    /* Extract final value in object list. If we have already marked the beginning of a value, than this must be final value in object list */
                    if ( value_start )
                    {
                        value_end = current_input_token;
                        /* If previous was a string token, then this must be a string value */
                        if ( *previous_token == STRING_TOKEN )
                        {
                            type = JSON_STRING_TYPE;

                            /* Move value token to point prior to string token and to last character of string value*/
                            value_end = previous_token - 1;
                            value_start = string_start + 1;
                        }
                        else if ( *previous_token == TRUE_TOKEN )
                        {
                            type = JSON_BOOLEAN_TYPE;

                            value_end = previous_token + sizeof( "true" ) - 2;
                            value_start = previous_token;
                        }
                        else if ( *previous_token == FALSE_TOKEN )
                        {
                            type = JSON_BOOLEAN_TYPE;

                            value_end = previous_token + sizeof( "false" ) - 2;
                            value_start = previous_token;
                        }
                        else if ( *previous_token == NULL_TOKEN )
                        {
                            type = JSON_NULL_TYPE;
                            value_end = previous_token + sizeof( "null" ) - 2;
                            value_start = previous_token;
                        }
                        else
                        {
                            char*   start = NULL;
                            char*   end = NULL;
                            uint8_t len = 0;
                            /* This must be a number value if not string. Arrays would have been picked up already by the end of array token */
                            type = JSON_NUMBER_TYPE;

                            end = value_end;

                            start = previous_token;
                            start++;
                            end--;
                            while ( *end == ' ' )
                            {
                                end--;
                            }
                            while ( *start == ' ' )
                            {
                                start++;
                            }

                            len = (uint8_t)(end - start + 1);

                            if ( validate_array_value( start, end, len ) != CY_RSLT_SUCCESS )
                            {
                                valid_json_string = CY_RSLT_TYPE_ERROR;
                                object_counter = 0;
                                array_counter = 0;
                                return valid_json_string;
                            }

                            /* Keep moving the value end token back till you encounter a digit */
                            while ( ( *value_end < '0' ) || ( *value_end > '9' ) )
                            {
                                value_end--;
                            }

                            /* Initialise the value_start token with value_end */
                            value_start = value_end;

                            /* Move value_start token until we encounter a non-digit value */
                            while ( ( ( *value_start >= '0' ) && ( *value_start <= '9' ) ) || ( *value_start == '.' ) || ( *value_start == '-' ) || ( *value_start == '+' ) || ( *value_start == 'e' ) )
                            {
                                value_start--;
                            }

                            /*Point to first number */
                            value_start++;
                        }

                        /* Prepare JSON object */
                        json_object.value_type = type;
                        json_object.value = value_start;
                        json_object.value_length = (uint16_t)(value_end - value_start + 1);

                        if ( internal_json_callback != NULL )
                        {
                            internal_json_callback( &json_object, internal_json_callback_arg );
                        }

                        /* Reset the value pointers */
                        value_start = NULL;
                        value_end = NULL;
                        string_start = NULL;
                        string_end = NULL;
                        type = UNKNOWN_JSON_TYPE;
                    }
                    if ( parent_counter )
                    {
                        parent_counter--;

                        if ( parent_counter )
                        {
                            json_object.parent_object = &parent_json_object[ parent_counter - 1 ];
                        }
                        else
                        {
                            json_object.parent_object = &parent_json_object[ parent_counter ];
                        }
                    }
                    else
                    {
                        json_object.parent_object = NULL;
                    }
                    previous_token = current_input_token;

                    break;

                case STRING_TOKEN:

                    if ( escape_token )
                    {
                        escape_token = 0;
                        break;
                    }
                    /* This indicates this must be closing token for object name */
                    if ( *previous_token == STRING_TOKEN )
                    {
                        /* Get the last character of the string name */
                        string_end = current_input_token;
                    }
                    else
                    {
                        /* Find start and end of of object name */
                        string_start = current_input_token;
                        string_end = NULL;
                    }
                    previous_token = current_input_token;

                    break;

                case TRUE_TOKEN:
                    if ( escape_token )
                    {
                        escape_token = 0;
                        break;
                    }
                    if ( *previous_token == OBJECT_START_TOKEN )
                    {
                        valid_json_string = CY_RSLT_TYPE_ERROR;
                        object_counter = 0;

                        return valid_json_string;
                    }
                    if ( ( *previous_token == START_OF_VALUE ) && ( string_end ) )
                    {
                        previous_token = current_input_token;
                        current_input_token = current_input_token + sizeof( "true" ) - 2;
                    }

                    break;

                case FALSE_TOKEN:
                    if ( escape_token )
                    {
                        escape_token = 0;
                        break;
                    }
                    if ( *previous_token == OBJECT_START_TOKEN )
                    {
                        valid_json_string = CY_RSLT_TYPE_ERROR;
                        object_counter = 0;
                        array_counter = 0;
                        return valid_json_string;
                    }
                    if ( ( *previous_token == START_OF_VALUE ) && ( string_end ) )
                    {
                        /* Skip ahead as this must be boolean false */
                        previous_token = current_input_token;
                        current_input_token = current_input_token + sizeof( "false" ) - 2;
                    }
                    break;

                case NULL_TOKEN:
                    if ( escape_token )
                    {
                        escape_token = 0;
                        break;
                    }
                    if ( ( *previous_token == START_OF_VALUE ) && ( string_end ) )
                    {
                        previous_token = current_input_token;
                        current_input_token = current_input_token + sizeof( "null" ) - 2;
                    }
                    break;

                case ARRAY_START_TOKEN:

                    if ( escape_token )
                    {
                        escape_token = 0;
                        break;
                    }

                    /* Skip in case we're parsing a string value */
                    if ( ( string_start != NULL ) && ( string_end == NULL ) )
                    {
                        break;
                    }
                    else if (string_start == NULL)
                    {
                        json_object.object_string = NULL;
                        json_object.object_string_length = 0;
                    }

                    array_counter++;
                    /*This means the last object name must have an array value type*/
                    type = JSON_ARRAY_TYPE;

                    json_object.value_type = type;
                    json_object.value = NULL;
                    json_object.value_length = 0;

                    if ( internal_json_callback != NULL )
                    {
                        internal_json_callback( &json_object, internal_json_callback_arg );
                    }

                    /* Reset object string start/end tokens */
                    string_start = NULL;
                    string_end = NULL;

                    previous_token = current_input_token;

                    break;

                case ARRAY_END_TOKEN:

                    if ( escape_token )
                    {
                        escape_token = 0;
                        break;
                    }

                    /* Skip in case we're parsing a string value */
                    if ( ( string_start != NULL ) && ( string_end == NULL ) )
                    {
                        break;
                    }

                    array_counter--;


                    if ( *( previous_token ) == START_OF_VALUE )
                    {
                        valid_json_string = CY_RSLT_TYPE_ERROR;
                        object_counter = 0;
                        return valid_json_string;
                    }

                    if ( *( current_input_token - space - 1 ) == COMMA_SEPARATOR )
                    {
                        valid_json_string = CY_RSLT_TYPE_ERROR;
                        object_counter = 0;
                        array_counter = 0;
                        return valid_json_string;
                    }

                    /* Ignore comma separators in values */
                    if ( ( string_start ) && ( string_end == NULL ) )
                    {
                        break;
                    }
                    /* If this comma is within an array, it must be delimiting values, so extract the comma delimited value */
                    else if ( type == JSON_ARRAY_TYPE )
                    {
                        /* If the token prior to the comma was a string token, then the delimited value must be a string */
                        if ( *previous_token == STRING_TOKEN )
                        {
                            type = JSON_STRING_TYPE;

                            /* Move token to point prior to string token and to last character of string value*/
                            value_end = previous_token - 1;
                            value_start = string_start + 1;
                        }
                        else if ( *previous_token == TRUE_TOKEN )
                        {
                            type = JSON_BOOLEAN_TYPE;

                            value_end = previous_token + sizeof( "true" ) - 2;
                            value_start = previous_token;
                        }
                        else if ( *previous_token == FALSE_TOKEN )
                        {
                            type = JSON_BOOLEAN_TYPE;

                            value_end = previous_token + sizeof( "false" ) - 2;
                            value_start = previous_token;
                        }
                        else if ( *previous_token == NULL_TOKEN )
                        {
                            type = JSON_NULL_TYPE;
                            value_end = previous_token + sizeof( "null" ) - 2;
                            value_start = previous_token;
                        }
                        else
                        {
                            char*   start = NULL;
                            char*   end = NULL;
                            uint8_t len = 0;
                            /* Delimited values must be a NUMBER if they are not a string */
                            type = JSON_NUMBER_TYPE;

                            /* Set value_end to point to current location */
                            value_end = current_input_token;

                            /* This must be a number value if not string. Arrays would have been picked up already by the end of array token */
                            type = JSON_NUMBER_TYPE;

                            end = value_end;

                            start = previous_token;
                            start++;
                            end--;
                            while ( *end == ' ' )
                            {
                                end--;
                            }
                            while ( *start == ' ' )
                            {
                                start++;
                            }

                            len = (uint8_t)(end - start + 1);

                            if ( validate_array_value( start, end, len ) != CY_RSLT_SUCCESS )
                            {
                                valid_json_string = CY_RSLT_TYPE_ERROR;
                                object_counter = 0;
                                array_counter = 0;
                                return valid_json_string;
                            }

                            /* Point to last number. Keep moving the value end token back till you encounter a digit */
                            while ( ( *value_end < '0' ) || ( *value_end > '9' ) )
                            {
                                value_end--;
                            }

                            /* Initialise the value_start pointer to point to last digit */
                            value_start = value_end;

                            /* Increment value_start until you reach first number */
                            while ( ( ( *value_start >= '0' ) && ( *value_start <= '9' ) ) || ( *value_start == '.' ) || ( *value_start == '-' ) || ( *value_start == '+' ) || ( *value_start == 'e' ) )
                            {
                                value_start--;
                            }

                            /*Point to first number */
                            value_start++;
                        }

                        /* prepare JSON object */
                        json_object.object_string = NULL;
                        json_object.object_string_length = 0;
                        json_object.value_type = type;
                        json_object.value = value_start;
                        json_object.value_length = (uint16_t)(value_end - value_start + 1);

                        if ( internal_json_callback != NULL )
                        {
                            internal_json_callback( &json_object, internal_json_callback_arg );
                        }

                        string_start = NULL;
                        string_end = NULL;
                        value_start = NULL;
                        value_end = NULL;
                        type = JSON_ARRAY_TYPE;
                    }

                    type = UNKNOWN_JSON_TYPE;
                    previous_token = current_input_token;

                    break;

                case START_OF_VALUE:

                    if ( escape_token )
                    {
                        escape_token = 0;
                        break;
                    }

                    if ( ( *( previous_token  ) == OBJECT_START_TOKEN ) || ( *( previous_token  ) == ARRAY_START_TOKEN ) )
                    {
                        valid_json_string = CY_RSLT_TYPE_ERROR;
                        object_counter = 0;
                        array_counter = 0;
                        return valid_json_string;
                    }
                    if ( string_end )
                    {
                        /* prepare JSON object */
                        json_object.object_string = string_start + 1;
                        json_object.object_string_length = (uint8_t)(string_end - string_start - 1);
                        type = UNKNOWN_JSON_TYPE;
                        previous_token = current_input_token;
                    }
                    if ( value_start == NULL )
                    {
                        value_start = current_input_token;
                    }

                    break;

                case COMMA_SEPARATOR:

                    if ( escape_token )
                    {
                        escape_token = 0;
                        break;
                    }

                    /* Ignore comma separators in values */
                    if ( ( string_start ) && ( string_end == NULL ) )
                    {
                        break;
                    }
                    /* If this comma is within an array, it must be delimiting values, so extract the comma delimited value */
                    else if ( type == JSON_ARRAY_TYPE )
                    {
                        /* If the token prior to the comma was a string token, then the delimited value must be a string */
                        if ( *previous_token == STRING_TOKEN )
                        {
                            type = JSON_STRING_TYPE;

                            /* Move token to point prior to string token and to last character of string value*/
                            value_end = previous_token - 1;
                            value_start = string_start + 1;
                        }
                        else if ( *previous_token == TRUE_TOKEN )
                        {
                            type = JSON_BOOLEAN_TYPE;

                            value_end = previous_token + sizeof( "true" ) - 2;
                            value_start = previous_token;
                        }
                        else if ( *previous_token == FALSE_TOKEN )
                        {
                            type = JSON_BOOLEAN_TYPE;

                            value_end = previous_token + sizeof( "false" ) - 2;
                            value_start = previous_token;
                        }
                        else if ( *previous_token == NULL_TOKEN )
                        {
                            type = JSON_NULL_TYPE;
                            value_end = previous_token + sizeof( "null" ) - 2;
                            value_start = previous_token;
                        }
                        else
                        {
                            /* Delimited values must be a NUMBER if they are not a string */
                            type = JSON_NUMBER_TYPE;

                            /* Set value_end to point to current location */
                            value_end = current_input_token;

                            /* Point to last number. Keep moving the value end token back till you encounter a digit */
                            while ( ( *value_end < '0' ) || ( *value_end > '9' ) )
                            {
                                value_end--;
                            }

                            /* Initialise the value_start pointer to point to last digit */
                            value_start = value_end;

                            /* Increment value_start until you reach first number */
                            while ( ( ( ( *value_start >= '0' ) && ( *value_start <= '9' ) ) || ( *value_start == '.' ) || ( *value_start == '-' ) || ( *value_start == '+' ) || ( *value_start == 'e' ) ) && ( *previous_token != *value_start ) )
                            {
                                value_start--;
                            }

                            /*Point to first number */
                            value_start++;
                        }

                        /* prepare JSON object */
                        json_object.object_string = NULL;
                        json_object.object_string_length = 0;
                        json_object.value_type = type;
                        json_object.value = value_start;
                        json_object.value_length = (uint16_t)(value_end - value_start + 1);

                        if ( internal_json_callback != NULL )
                        {
                            internal_json_callback( &json_object, internal_json_callback_arg );
                        }
                        value_start = NULL;
                        value_end = NULL;
                        string_start = NULL;
                        string_end = NULL;
                        type = JSON_ARRAY_TYPE;
                    }
                    else if ( value_start )
                    {
                        value_end = current_input_token;

                        /* Commas are only used to seperate values so this must indicate an end of value, which means last object information is for us */
                        if ( *previous_token == STRING_TOKEN )
                        {
                            type = JSON_STRING_TYPE;

                            value_end = previous_token - 1;
                            value_start = string_start + 1;
                        }
                        else if ( *previous_token == ARRAY_END_TOKEN )
                        {
                            if ( string_start )
                            {
                                type = JSON_STRING_TYPE;
                                value_start = string_start + 1;
                                value_end = string_end - 1;
                            }
                            else
                            {
                                type = JSON_NUMBER_TYPE;

                                /* Keep moving the value end token back till you encounter a digit */
                                while ( ( *value_end < '0' ) || ( *value_end > '9' ) )
                                {
                                    value_end--;
                                }

                                value_start = value_end;

                                while ( ( *value_start >= '0' ) && ( *value_start <= '9' ) )
                                {
                                    value_start--;
                                }

                                value_start++;
                            }
                        }
                        else if ( *previous_token == TRUE_TOKEN )
                        {
                            type = JSON_BOOLEAN_TYPE;

                            value_end = previous_token + sizeof( "true" ) - 2;
                            value_start = previous_token;
                        }
                        else if ( *previous_token == FALSE_TOKEN )
                        {
                            type = JSON_BOOLEAN_TYPE;

                            value_end = previous_token + sizeof( "false" ) - 2;
                            value_start = previous_token;
                        }
                        else if ( *previous_token == NULL_TOKEN )
                        {
                            type = JSON_NULL_TYPE;
                            value_end = previous_token + sizeof( "null" ) - 2;
                            value_start = previous_token;
                        }
                        else
                        {
                            type = JSON_NUMBER_TYPE;

                            /* Keep moving the value end token back till you encounter a digit */
                            while ( ( ( *value_end < '0' ) || ( *value_end > '9' ) ) )
                            {
                                value_end--;
                            }

                            value_start = value_end;

                            while ( ( ( *value_start >= '0' ) && ( *value_start <= '9' ) ) || ( *value_start == '.' ) || ( *value_start == '-' ) || ( *value_start == '+' ) || ( *value_start == 'e' ) )
                            {
                                value_start--;
                            }

                            value_start++;
                        }

                        json_object.value_type = type;
                        json_object.value = value_start;
                        json_object.value_length = (uint16_t)(value_end - value_start + 1);

                        if ( internal_json_callback != NULL )
                        {
                            internal_json_callback( &json_object, internal_json_callback_arg );
                        }

                        string_start = NULL;
                        string_end = NULL;
                        value_start = NULL;
                        value_end = NULL;
                        type = UNKNOWN_JSON_TYPE;
                    }

                    previous_token = current_input_token;

                    break;

                case ESCAPE_TOKEN:
                    /* Clear escape token flag, if the previous token is an escape token. Else set it */
                    escape_token = ( escape_token == 1 ) ? 0 : 1;

                    break;

                default:
                    /* Reset escape token flag */
                    escape_token = 0;
                    if ( ( object_counter == 0 ) && ( array_counter == 0 ) && ( *current_input_token  != ' ' ) &&
                         ( JSON_IS_NOT_ESC_CHAR ( *current_input_token ) ) )
                    {
                        valid_json_string = CY_RSLT_TYPE_ERROR;
                        object_counter = 0;
                        array_counter = 0;
                        return valid_json_string;
                    }
                    break;
            } // switch

            /* Counting spaces*/
            if ( *( current_input_token ) == ' ' )
            {
                space++;
            }
            else
            {
                space = 0;
            }
            current_input_token++;
            if ( ( *( current_input_token ) == '\0' ) && ( ( ( *( previous_token ) == COMMA_SEPARATOR ) || ( *( previous_token ) == STRING_TOKEN ) || ( *( previous_token ) == START_OF_VALUE ) || ( *( previous_token ) == ARRAY_START_TOKEN ) ) ) )
            {
                valid_json_string = CY_RSLT_TYPE_ERROR;
                object_counter = 0;
                array_counter = 0;
                return valid_json_string;
            }
        } // while
    }     // if
    else
    {
        valid_json_string = CY_RSLT_TYPE_ERROR;
        return valid_json_string;
    }

    /* This means that a closing brace has not been found for an object. This data is split across packets */
    if ( object_counter || array_counter )
    {
        memset( packet_backup, 0x0, sizeof( packet_backup ) );

        // Copy everything from the most recent unfinished object onwards

        number_of_bytes_backed_up = (uint32_t)(end_of_input - most_recent_object_marker);

        memcpy( packet_backup, most_recent_object_marker, number_of_bytes_backed_up );


        incomplete_response = 1;
        valid_json_string = CY_RSLT_TYPE_ERROR;
        object_counter      = 0;

        return valid_json_string;
    }


    memset( &parent_json_object, 0x0, sizeof( parent_json_object ) );

    incomplete_response = 0;

    object_counter      = 0;

    string_start        = NULL;
    string_end          = NULL;

    value_start         = NULL;
    value_end           = NULL;

    type                = UNKNOWN_JSON_TYPE;

    escape_token        = 0;

    previous_token      = NULL;

    return valid_json_string;
}
