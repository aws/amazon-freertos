/*
 * FreeRTOS+CLI V1.0.4
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "FreeRTOS_CLI_Console.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "aws_test_runner.h"

#define COMMAND_BUFFER_SIZE              ( 10 )

#define OUTPUT_BUFFER_SIZE               ( 100 )

#define MAX_INPUT_STREAM_SIZE            ( 100 )

#define MAX_OUTPUT_STREAM_SIZE           ( 200 )

#define MAX_OUTPUT_WAIT_TIME_MS          ( 5000 )

#define MAX_INPUT_STREAM_WAIT_TIME_MS    ( 500 )

static BaseType_t prvEchoCommandHandler( char * pcWriteBuffer,
                                         size_t xWriteBufferLen,
                                         const char * pcCommandString );

static int32_t prvReadFromInputStream( char * const buffer,
                                       uint32_t length );

static void prvWriteToOutputStream( const char * const buffer,
                                    uint32_t length );

static void prvConsoleLoopTask( void * parameters );

static char cCommandBuffer[ COMMAND_BUFFER_SIZE ];
static char cOutputBuffer[ OUTPUT_BUFFER_SIZE ];

static TaskHandle_t xConsoleTask;
static StreamBufferHandle_t xInputStream;
static StreamBufferHandle_t xOutputStream;
static const char * pcEndOfOutputMessage = "\r\n[Press Enter to run the previous command]\r\n>";
static const char * const pcStartMessage = "\r\n[Input a command or type help then press ENTER to get started]\r\n>";
static size_t xOutputAtOnceLength;
static char cOutput[ MAX_OUTPUT_STREAM_SIZE ] = "";

static xConsoleIO_t xConsole =
{
    prvReadFromInputStream,
    prvWriteToOutputStream
};

static const CLI_Command_Definition_t xEchoCommand =
{
    "echo",
    "\r\nDummy command with no parameters\r\n",
    prvEchoCommandHandler,
    0
};

static BaseType_t prvEchoCommandHandler( char * pcWriteBuffer,
                                         size_t xWriteBufferLen,
                                         const char * pcCommandString )
{
    static size_t xLengthWritten = 0;
    BaseType_t xShouldInvokeAgain = pdTRUE;
    size_t xLengthToWrite;

    xLengthToWrite = strlen( pcCommandString ) - xLengthWritten;

    if( xLengthToWrite > xOutputAtOnceLength )
    {
        xLengthToWrite = xOutputAtOnceLength;
    }

    strncpy( pcWriteBuffer, ( pcCommandString + xLengthWritten ), xLengthToWrite );
    pcWriteBuffer[ xLengthToWrite ] = '\0';

    xLengthWritten += xLengthToWrite;

    if( xLengthWritten == strlen( pcCommandString ) )
    {
        xLengthWritten = 0;
        xShouldInvokeAgain = pdFALSE;
    }

    return xShouldInvokeAgain;
}

static void prvConsoleLoopTask( void * parameters )
{
    ( void ) parameters;

    /*
     * This function never returns, if valid parameters
     * are passed into the function.
     */
    FreeRTOS_CLIEnterConsoleLoop( xConsole,
                                  cCommandBuffer,
                                  COMMAND_BUFFER_SIZE,
                                  cOutputBuffer,
                                  OUTPUT_BUFFER_SIZE );
}

static int32_t prvReadFromInputStream( char * const buffer,
                                       uint32_t length )
{
    return ( int32_t ) xStreamBufferReceive( xInputStream,
                                             buffer,
                                             length,
                                             pdMS_TO_TICKS( MAX_INPUT_STREAM_WAIT_TIME_MS ) );
}

static void prvWriteToOutputStream( const char * const buffer,
                                    uint32_t length )
{
    size_t index, actualLength;

    /*
     * Console loop output buffer should always include a null termination.
     * Length does not include the null termination.
     */
    TEST_ASSERT_EQUAL( 0x00, buffer[ length ] );

    actualLength = strlen( buffer );

    /*
     * Ensure remaining length of output buffer is all filled with zeros.
     */
    for( index = actualLength; index < length; index++ )
    {
        TEST_ASSERT_EQUAL( 0x00, buffer[ index ] );
    }

    xStreamBufferSend( xOutputStream, buffer, actualLength, portMAX_DELAY );
}

static void prvValidateStartupMessage( void )
{
    size_t xOutputLength;

    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          strlen( pcStartMessage ),
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( strlen( pcStartMessage ), xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( pcStartMessage, cOutput, strlen( pcStartMessage ) ) );
    TEST_ASSERT_EQUAL( pdTRUE, xStreamBufferIsEmpty( xOutputStream ) );
}

static void prvSetupTaskAndStreamBuffer( void )
{
    xInputStream = xStreamBufferCreate( MAX_INPUT_STREAM_SIZE, 1 );
    TEST_ASSERT_NOT_NULL( xInputStream );

    xOutputStream = xStreamBufferCreate( MAX_OUTPUT_STREAM_SIZE, 1 );
    TEST_ASSERT_NOT_NULL( xOutputStream );

    TEST_ASSERT_EQUAL( pdTRUE,
                       xTaskCreate( prvConsoleLoopTask, "CONSOLE_TASK",
                                    configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY,
                                    &xConsoleTask ) );

    prvValidateStartupMessage();
}

static void prvTeardownTaskAndStreamBUffer( void )
{
    vTaskDelete( xConsoleTask );
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    vStreamBufferDelete( xInputStream );
    vStreamBufferDelete( xOutputStream );
}

TEST_GROUP( FreeRTOS_CLI_Console );

TEST_SETUP( FreeRTOS_CLI_Console )
{
    static BaseType_t xCommandsRegistered = pdFALSE;

    /**
     * Register all commands only once.
     */
    if( xCommandsRegistered == pdFALSE )
    {
        TEST_ASSERT_EQUAL( pdTRUE, FreeRTOS_CLIRegisterCommand( &xEchoCommand ) );
        xCommandsRegistered = pdTRUE;
    }

    xOutputAtOnceLength = strlen( "echo" );

    memset( cCommandBuffer, 0x00, COMMAND_BUFFER_SIZE );
    memset( cOutputBuffer, 0x00, OUTPUT_BUFFER_SIZE );
}
TEST_TEAR_DOWN( FreeRTOS_CLI_Console )
{
}

TEST_GROUP_RUNNER( FreeRTOS_CLI_Console )
{
    prvSetupTaskAndStreamBuffer();
    RUN_TEST_CASE( FreeRTOS_CLI_Console, ExecuteEchoCommandSingleInvocation );
    RUN_TEST_CASE( FreeRTOS_CLI_Console, ExecuteEchoCommandMultipleReads );
    RUN_TEST_CASE( FreeRTOS_CLI_Console, InputWithBackspace );
    RUN_TEST_CASE( FreeRTOS_CLI_Console, ExecuteEchoCommandMultipleInvocation );
    RUN_TEST_CASE( FreeRTOS_CLI_Console, ExecutePreviousCommand );
    prvTeardownTaskAndStreamBUffer();
}

TEST( FreeRTOS_CLI_Console, ExecuteEchoCommandSingleInvocation )
{
    const char * pcCommand = "echo\r\n";
    size_t commandLength = strlen( pcCommand );
    size_t xOutputLength;

    /*
     * Input the command along with newline characters.
     */
    TEST_ASSERT_EQUAL( commandLength,
                       xStreamBufferSend( xInputStream,
                                          pcCommand,
                                          commandLength,
                                          portMAX_DELAY ) );

    /*
     * The command along with new line characters should be echoed back to the console.
     */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          commandLength,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( commandLength, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( pcCommand, cOutput, commandLength ) );

    /**
     * Newline characters are again written to the console to space the input and output.
     */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          2,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 2, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "\r\n", cOutput, 2 ) );

    /**
     * Output of the echo command is written to the console. This is the command itself followed by zeros for remaining
     * length of output buffer.
     */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          4,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 4, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "echo", cOutput, 4 ) );

    /**
     * Last message written to the console is the end of output message.
     */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          strlen( pcEndOfOutputMessage ),
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( strlen( pcEndOfOutputMessage ), xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( pcEndOfOutputMessage,
                                   cOutput,
                                   strlen( pcEndOfOutputMessage ) ) );

    TEST_ASSERT_EQUAL( pdTRUE, xStreamBufferIsEmpty( xOutputStream ) );
}

TEST( FreeRTOS_CLI_Console, ExecuteEchoCommandMultipleReads )
{
    const char * pcCommand = "echo";
    size_t commandLength = strlen( pcCommand );
    size_t xOutputLength;

    /* Input the command without newline characters. */
    TEST_ASSERT_EQUAL( commandLength,
                       xStreamBufferSend( xInputStream,
                                          pcCommand,
                                          commandLength,
                                          portMAX_DELAY ) );

    /*
     * The command should be echoed back to the console.
     */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          commandLength,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( commandLength, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( pcCommand, cOutput, commandLength ) );

    /*
     * Input the newline characters.
     */
    TEST_ASSERT_EQUAL( 2, xStreamBufferSend( xInputStream, "\r\n", 2, portMAX_DELAY ) );

    /* The input newline characters are echoed back to console. */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          2,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 2, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "\r\n", cOutput, 2 ) );

    /* New line characters are written to space input and output. */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          2,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 2, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "\r\n", cOutput, 2 ) );


    /*
     * Command is processed and the output of the command is written to console, followed
     * by all zeros for length of buffer. Finally an end of output message is written to
     * console.
     */

    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          4,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 4, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "echo", cOutput, 4 ) );

    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          strlen( pcEndOfOutputMessage ),
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( strlen( pcEndOfOutputMessage ), xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( pcEndOfOutputMessage, cOutput, strlen( pcEndOfOutputMessage ) ) );
    TEST_ASSERT_EQUAL( pdTRUE, xStreamBufferIsEmpty( xOutputStream ) );
}

TEST( FreeRTOS_CLI_Console, InputWithBackspace )
{
    const char * pcCommand = "\becx\bho\r\n";
    size_t commandLength = strlen( pcCommand );
    size_t xOutputLength;

    /*
     * Input the command with back space.
     */
    TEST_ASSERT_EQUAL( commandLength,
                       xStreamBufferSend( xInputStream,
                                          pcCommand,
                                          commandLength,
                                          portMAX_DELAY ) );

    /*
     * The command should be echoed back to the console along with the back space.
     */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          commandLength,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( commandLength, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( pcCommand, cOutput, commandLength ) );

    /*
     * New line characters are written to space input and output.
     */

    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          2,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 2, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "\r\n", cOutput, 2 ) );

    /*
     * Output of the command should not contain the backspace.
     */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          4,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 4, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "echo", cOutput, 4 ) );

    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          strlen( pcEndOfOutputMessage ),
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( strlen( pcEndOfOutputMessage ), xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( pcEndOfOutputMessage, cOutput, strlen( pcEndOfOutputMessage ) ) );
    TEST_ASSERT_EQUAL( pdTRUE, xStreamBufferIsEmpty( xOutputStream ) );
}

TEST( FreeRTOS_CLI_Console, ExecuteEchoCommandMultipleInvocation )
{
    const char * pcCommand = "echo\n";
    size_t commandLength = strlen( pcCommand );
    size_t xOutputLength;

    xOutputAtOnceLength = 2;

    /*
     * Input the command with back space.
     */
    TEST_ASSERT_EQUAL( commandLength,
                       xStreamBufferSend( xInputStream,
                                          pcCommand,
                                          commandLength,
                                          portMAX_DELAY ) );

    /*
     * The command should be echoed back to the console.
     */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          commandLength,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( commandLength, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( pcCommand, cOutput, commandLength ) );

    /*
     * New line characters are written to space input and output.
     */

    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          2,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 2, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "\r\n", cOutput, 2 ) );

    /*
     * Output of the command should be written into stream in two steps.
     */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          2,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 2, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "ec", cOutput, 2 ) );

    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          2,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 2, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "ho", cOutput, 2 ) );

    /* End of output message. */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          strlen( pcEndOfOutputMessage ),
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( strlen( pcEndOfOutputMessage ), xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( pcEndOfOutputMessage, cOutput, strlen( pcEndOfOutputMessage ) ) );
    TEST_ASSERT_EQUAL( pdTRUE, xStreamBufferIsEmpty( xOutputStream ) );
}

TEST( FreeRTOS_CLI_Console, ExecutePreviousCommand )
{
    const char * pcCommand = "echo\n";
    size_t commandLength = strlen( pcCommand );
    size_t xOutputLength;

    /*
     * Input the command with back space.
     */
    TEST_ASSERT_EQUAL( commandLength,
                       xStreamBufferSend( xInputStream,
                                          pcCommand,
                                          commandLength,
                                          portMAX_DELAY ) );

    /*
     * The command should be echoed back to the console.
     */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          commandLength,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( commandLength, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( pcCommand, cOutput, commandLength ) );

    /*
     * New line characters are written to space input and output.
     */

    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          2,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 2, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "\r\n", cOutput, 2 ) );

    /* Output of the command is written to console. */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          4,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 4, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "echo", cOutput, 4 ) );

    /* End of output message. */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          strlen( pcEndOfOutputMessage ),
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( strlen( pcEndOfOutputMessage ), xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( pcEndOfOutputMessage, cOutput, strlen( pcEndOfOutputMessage ) ) );
    TEST_ASSERT_EQUAL( pdTRUE, xStreamBufferIsEmpty( xOutputStream ) );


    /*
     * Press Enter to rerun the previous command.
     */
    TEST_ASSERT_EQUAL( 2,
                       xStreamBufferSend( xInputStream,
                                          "\r\n",
                                          2,
                                          portMAX_DELAY ) );

    /*
     * Command echoed back.
     */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          2,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 2, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "\r\n", cOutput, 2 ) );


    /*
     * Newline to space input and output.
     */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          2,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 2, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "\r\n", cOutput, 2 ) );

    /* Output of the command is written to console. */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          4,
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( 4, xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "echo", cOutput, 4 ) );

    /* End of output message. */
    xOutputLength = xStreamBufferReceive( xOutputStream,
                                          cOutput,
                                          strlen( pcEndOfOutputMessage ),
                                          pdMS_TO_TICKS( MAX_OUTPUT_WAIT_TIME_MS ) );

    TEST_ASSERT_EQUAL( strlen( pcEndOfOutputMessage ), xOutputLength );
    TEST_ASSERT_EQUAL( 0, strncmp( pcEndOfOutputMessage, cOutput, strlen( pcEndOfOutputMessage ) ) );
    TEST_ASSERT_EQUAL( pdTRUE, xStreamBufferIsEmpty( xOutputStream ) );
}
