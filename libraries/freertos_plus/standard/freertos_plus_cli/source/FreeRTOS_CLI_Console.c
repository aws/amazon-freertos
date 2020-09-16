/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI_Console.h"

#define cmdMAX_INPUT_BUFFER_SIZE    50

#define cmdMAX_ERROR_SIZE           50

/* DEL acts as a backspace. */
#define cmdASCII_DEL                ( 0x7F )

#define cmdERROR_DELAY              ( 1000 )

/* Const messages output by the command console. */
static const char * const pcStartMessage = "\r\n[Input a command or type help then press ENTER to get started]\r\n>";
static const char * const pcEndOfOutputMessage = "\r\n[Press Enter to run the previous command]\r\n>";
static const char * const pcNewLine = "\r\n";

static char cInputBuffer[ cmdMAX_INPUT_BUFFER_SIZE ] = "";
static char cErrorString[ cmdMAX_ERROR_SIZE ] = "";

static uint8_t ucCommandIndex = 0;

static void processInputBuffer( xConsoleIO_t consoleIO,
                                int32_t inputSize,
                                char * pCommandBuffer,
                                size_t commandBufferLength,
                                char * pOutputBuffer,
                                size_t outpuBufferLength );

void FreeRTOS_CLIEnterConsoleLoop( xConsoleIO_t consoleIO,
                                   char * pCommandBuffer,
                                   size_t commandBufferLength,
                                   char * pOutputBuffer,
                                   size_t outputBufferLength )
{
    BaseType_t status = pdTRUE;
    int32_t bytesRead;

    if( ( consoleIO.read == NULL ) || ( consoleIO.write == NULL ) )
    {
        status = pdFALSE;
    }

    if( status == pdTRUE )
    {
        if( ( pCommandBuffer == NULL ) || ( commandBufferLength == 0 ) )
        {
            status = pdFALSE;
        }
    }

    if( status == pdTRUE )
    {
        if( ( pOutputBuffer == NULL ) || ( outputBufferLength == 0 ) )
        {
            status = pdFALSE;
        }
    }

    if( status == pdTRUE )
    {
        memset( pCommandBuffer, 0x00, commandBufferLength );
        memset( pOutputBuffer, 0x00, outputBufferLength );

        consoleIO.write( pcStartMessage, strlen( pcStartMessage ) );

        for( ; ; )
        {
            /* Read characters to input buffer. */
            bytesRead = consoleIO.read( cInputBuffer, cmdMAX_INPUT_BUFFER_SIZE - 1 );

            if( bytesRead > 0 )
            {
                /* Echo back. */
                consoleIO.write( cInputBuffer, bytesRead );

                processInputBuffer( consoleIO,
                                    bytesRead,
                                    pCommandBuffer,
                                    commandBufferLength,
                                    pOutputBuffer,
                                    outputBufferLength );

                /* Reset input buffer for next iteration. */
                memset( cInputBuffer, 0x00, cmdMAX_INPUT_BUFFER_SIZE );
            }
            else if( bytesRead < 0 )
            {
                snprintf( cErrorString, cmdMAX_ERROR_SIZE, "Read failed with error %d\n", ( int ) status );
                consoleIO.write( cErrorString, sizeof( cErrorString ) );
                memset( cErrorString, 0x00, cmdMAX_ERROR_SIZE );

                vTaskDelay( pdMS_TO_TICKS( cmdERROR_DELAY ) );
            }
        }
    }
}

static void processInputBuffer( xConsoleIO_t consoleIO,
                                int32_t inputSize,
                                char * pCommandBuffer,
                                size_t commandBufferLength,
                                char * pOutputBuffer,
                                size_t outpuBufferLength )
{
    BaseType_t xReturned;
    uint8_t i;
    char cRxedChar;

    for( i = 0; i < inputSize; i++ )
    {
        cRxedChar = cInputBuffer[ i ];

        /* A character was entered.  Add it to the string entered so
         * far. Add a null termination to the end of the string.
         * When a \n is entered the complete string will be
         * passed to the command interpreter. */
        if( ( cRxedChar >= ' ' ) && ( cRxedChar <= '~' ) )
        {
            if( ucCommandIndex < ( commandBufferLength - 1UL ) )
            {
                pCommandBuffer[ ucCommandIndex ] = cRxedChar;
                ucCommandIndex++;
                pCommandBuffer[ ucCommandIndex ] = '\0';
            }
        }
        else if( ( cRxedChar == '\b' ) || ( cRxedChar == cmdASCII_DEL ) )
        {
            /* Backspace was pressed.  Erase the last character in the string - if any. */
            if( ucCommandIndex > 0 )
            {
                ucCommandIndex--;
                pCommandBuffer[ ucCommandIndex ] = '\0';
            }
        }
        /* Was it the end of the line? */
        else if( ( cRxedChar == '\n' ) || ( cRxedChar == '\r' ) )
        {
            /* Skip subsequent '\n', '\r' or '\n' of CRLF. */
            if( ( i > 0 ) &&
                ( ( cInputBuffer[ i - 1 ] == '\r' ) || ( cInputBuffer[ i - 1 ] == '\n' ) ) )
            {
                continue;
            }

            /* Just to space the output from the input. */
            consoleIO.write( pcNewLine, strlen( pcNewLine ) );

            /* Pass the received command to the command interpreter.  The
             * command interpreter is called repeatedly until it returns
             * pdFALSE	(indicating there is no more output) as it might
             * generate more than one string. */
            do
            {
                /* Get the next output string from the command interpreter. */
                xReturned = FreeRTOS_CLIProcessCommand( pCommandBuffer, pOutputBuffer, outpuBufferLength - 1 );

                /* Write the generated string to the output. */
                consoleIO.write( pOutputBuffer, outpuBufferLength - 1 );
                memset( pOutputBuffer, 0x00, outpuBufferLength );
            } while( xReturned != pdFALSE );

            /* All the strings generated by the input command have been
             * sent.  Clear the command index to receive a new command.
             * Remember the command that was just processed first in case it is
             * to be processed again. */
            ucCommandIndex = 0;

            consoleIO.write( pcEndOfOutputMessage, strlen( pcEndOfOutputMessage ) );
        }
    }
}
