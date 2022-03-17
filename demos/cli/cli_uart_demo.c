/*
 * FreeRTOS V202203.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI_UART.h"
#include "platform/iot_network.h"


/* Dimensions of the buffer into which input command is placed. */
#define cmdMAX_INPUT_SIZE     60

/* Dimensions of the buffer into which string output is placed. */
#define cmdMAX_OUTPUT_SIZE    1024


/*
 * Defines a command that expects exactly three parameters.  Each of the three
 * parameter are echoed back one at a time.
 */
static BaseType_t prvThreeParameterEchoCommand( char * pcWriteBuffer,
                                                size_t xWriteBufferLen,
                                                const char * pcCommandString );


/*
 * Defines a command that can take a variable number of parameters.  Each
 * parameter is echoes back one at a time.
 */
static BaseType_t prvParameterEchoCommand( char * pcWriteBuffer,
                                           size_t xWriteBufferLen,
                                           const char * pcCommandString );

/*
 * Defines a command that returns a table showing the state of each task at the
 * time the command is called.
 */
static BaseType_t prvTaskStatsCommand( char * pcWriteBuffer,
                                       size_t xWriteBufferLen,
                                       const char * pcCommandString );

/**
 * Registers the CLI commands along with handlers required for the demo.
 */
static void prvRegisterCLICommands( void );

/* Structure that defines the "echo_3_parameters" command line command.  This
 * takes exactly three parameters that the command simply echos back one at a
 * time. */
static const CLI_Command_Definition_t xThreeParameterEcho =
{
    "echo_3_parameters",
    "\r\necho_3_parameters <param1> <param2> <param3>:\r\n Expects three parameters, echos each in turn\r\n\r\n",
    prvThreeParameterEchoCommand, /* The function to run. */
    3                             /* Three parameters are expected, which can take any value. */
};

/* Structure that defines the "echo_parameters" command line command.  This
 * takes a variable number of parameters that the command simply echos back one at
 * a time. */
static const CLI_Command_Definition_t xParameterEcho =
{
    "echo_parameters",
    "\r\necho_parameters <...>:\r\n Take variable number of parameters, echos each in turn\r\n\r\n",
    prvParameterEchoCommand, /* The function to run. */
    -1                       /* The user can enter any number of commands. */
};

/* Structure that defines the "task-stats" command line command. */
static const CLI_Command_Definition_t xTaskStats =
{
    "task-stats",        /* The command string to type. */
    "\r\ntask-stats:\r\n Displays a table showing the state of each FreeRTOS task\r\n\r\n",
    prvTaskStatsCommand, /* The function to run. */
    0                    /* No parameters are expected. */
};

/**
 * Static array into which the commands will be placed from the console.
 */
static char cCommandBuffer[ cmdMAX_INPUT_SIZE ];

/**
 * Static array into which the output of the command will be stored to be written to the console.
 */
static char cOutputBuffer[ cmdMAX_OUTPUT_SIZE ];


static BaseType_t prvThreeParameterEchoCommand( char * pcWriteBuffer,
                                                size_t xWriteBufferLen,
                                                const char * pcCommandString )
{
    const char * pcParameter;
    BaseType_t lParameterStringLength, xReturn;
    static BaseType_t lParameterNumber = 0;
    size_t xStatus;

    /* Check the write buffer is not NULL. */
    configASSERT( pcWriteBuffer );

    if( lParameterNumber == 0 )
    {
        /* The first time the function is called after the command has been
         * entered just a header string is returned. */
        xStatus = snprintf( pcWriteBuffer, xWriteBufferLen, "The three parameters were:\r\n" );

        /* Assert condition that the buffer provided is enough to write the formatted string. If the buffer is not
         * adequate, snprintf returns the total length required (without the null termination) for
         * writing the formatted string to the buffer. If this assert condition is hit,
         * adjust the output buffer to the required length as returned from snprintf.
         */
        configASSERT( ( xStatus >= 0 ) && ( xStatus < xWriteBufferLen ) );


        /* Next time the function is called the first parameter will be echoed
         * back. */
        lParameterNumber = 1L;

        /* There is more data to be returned as no parameters have been echoed
         * back yet. */
        xReturn = pdPASS;
    }
    else
    {
        /* Obtain the parameter string. */
        pcParameter = FreeRTOS_CLIGetParameter
                      (
            pcCommandString,        /* The command string itself. */
            lParameterNumber,       /* Return the next parameter. */
            &lParameterStringLength /* Store the parameter string length. */
                      );

        /* Sanity check something was returned. */
        configASSERT( pcParameter );

        /* Return the parameter string. */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );
        xStatus = snprintf( pcWriteBuffer, xWriteBufferLen, "%d: %.*s\r\n",
                            lParameterNumber,
                            lParameterStringLength,
                            pcParameter );

        /* Assert condition that the buffer provided is enough to write the formatted string. If the buffer is not
         * adequate, snprintf returns the total length required (without the null termination) for
         * writing the formatted string to the buffer. If this assert condition is hit,
         * adjust the output buffer to the required length as returned from snprintf.
         */
        configASSERT( ( xStatus >= 0 ) && ( xStatus < xWriteBufferLen ) );

        /* If this is the last of the three parameters then there are no more
         * strings to return after this one. */
        if( lParameterNumber == 3L )
        {
            /* If this is the last of the three parameters then there are no more
             * strings to return after this one. */
            xReturn = pdFALSE;
            lParameterNumber = 0L;
        }
        else
        {
            /* There are more parameters to return after this one. */
            xReturn = pdTRUE;
            lParameterNumber++;
        }
    }

    return xReturn;
}

static BaseType_t prvParameterEchoCommand( char * pcWriteBuffer,
                                           size_t xWriteBufferLen,
                                           const char * pcCommandString )
{
    const char * pcParameter;
    BaseType_t lParameterStringLength, xReturn;
    static BaseType_t lParameterNumber = 0;
    size_t xStatus;

    /* Check the write buffer is not NULL. */
    configASSERT( pcWriteBuffer );

    if( lParameterNumber == 0 )
    {
        /* The first time the function is called after the command has been
         * entered just a header string is returned. */
        xStatus = snprintf( pcWriteBuffer, xWriteBufferLen, "The parameters were:\r\n" );

        /* Assert condition that the buffer provided is enough to write the formatted string. If the buffer is not
         * adequate, snprintf returns the total length required (without the null termination) for
         * writing the formatted string to the buffer. If this assert condition is hit,
         * adjust the output buffer to the required length as returned from snprintf.
         */
        configASSERT( ( xStatus >= 0 ) && ( xStatus < xWriteBufferLen ) );

        /* Next time the function is called the first parameter will be echoed
         * back. */
        lParameterNumber = 1L;

        /* There is more data to be returned as no parameters have been echoed
         * back yet. */
        xReturn = pdPASS;
    }
    else
    {
        /* Obtain the parameter string. */
        pcParameter = FreeRTOS_CLIGetParameter
                      (
            pcCommandString,        /* The command string itself. */
            lParameterNumber,       /* Return the next parameter. */
            &lParameterStringLength /* Store the parameter string length. */
                      );

        if( pcParameter != NULL )
        {
            /* Return the parameter string. */
            memset( pcWriteBuffer, 0x00, xWriteBufferLen );
            xStatus = snprintf( pcWriteBuffer, xWriteBufferLen, "%d: %.*s\r\n",
                                lParameterNumber,
                                lParameterStringLength,
                                pcParameter );

            /* Assert condition that the buffer provided is enough to write the formatted string. If the buffer is not
             * adequate, snprintf returns the total length required (without the null termination) for
             * writing the formatted string to the buffer. If this assert condition is hit,
             * adjust the output buffer to the required length as returned from snprintf.
             */
            configASSERT( ( xStatus >= 0 ) && ( xStatus < xWriteBufferLen ) );



            /* There might be more parameters to return after this one. */
            xReturn = pdTRUE;
            lParameterNumber++;
        }
        else
        {
            /* No more parameters were found.  Make sure the write buffer does
             * not contain a valid string. */
            pcWriteBuffer[ 0 ] = 0x00;

            /* No more data to return. */
            xReturn = pdFALSE;

            /* Start over the next time this command is executed. */
            lParameterNumber = 0;
        }
    }

    return xReturn;
}

static BaseType_t prvTaskStatsCommand( char * pcWriteBuffer,
                                       size_t xWriteBufferLen,
                                       const char * pcCommandString )
{
    const char * const pcHeader = "Task          State  Priority  Stack	#\r\n************************************************\r\n";
    size_t xStatus;

    /* Remove compile time warnings about unused parameters, and check the
     * write buffer is not NULL. */
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    configASSERT( pcWriteBuffer );

    /* Generate a table of task stats. */
    xStatus = snprintf( pcWriteBuffer, xWriteBufferLen, pcHeader );

    /* Assert condition that the buffer provided is enough to write the formatted string. If the buffer is not
     * adequate, snprintf returns the total length required (without the null termination) for
     * writing the formatted string to the buffer. If this assert condition is hit,
     * adjust the output buffer to the required length as returned from snprintf.
     */
    configASSERT( ( xStatus >= 0 ) && ( xStatus < xWriteBufferLen ) );

    vTaskList( pcWriteBuffer + strlen( pcHeader ) );

    /* There is no more data to return after this single string, so return
     * pdFALSE. */
    return pdFALSE;
}


static void prvRegisterCLICommands( void )
{
    /* Register all the command line commands defined immediately above. */
    FreeRTOS_CLIRegisterCommand( &xThreeParameterEcho );
    FreeRTOS_CLIRegisterCommand( &xParameterEcho );
    FreeRTOS_CLIRegisterCommand( &xTaskStats );
}


int vRunCLIUartDemo( bool awsIotMqttMode,
                     const char * pIdentifier,
                     void * pNetworkServerInfo,
                     void * pNetworkCredentialInfo,
                     const IotNetworkInterface_t * pNetworkInterface )
{
    /* These parameters are unused for the demo. */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;

    /* Register all CLI commands at startup. */
    prvRegisterCLICommands();

    configPRINTF( ( "Welcome to FreeRTOS CLI demo\r\n " ) );

    /* Run a non-terminating loop on the demo task to wait for commands from the UART console. */
    FreeRTOS_CLIEnterConsoleLoop( uartConsoleIO, cCommandBuffer, cmdMAX_INPUT_SIZE, cOutputBuffer, cmdMAX_OUTPUT_SIZE );

    return 0;
}
