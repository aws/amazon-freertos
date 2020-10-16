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
 */
#include <string.h>

#include "FreeRTOS.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "aws_test_runner.h"

#include "FreeRTOS_CLI.h"

#define USER_OUTPUT_BUFFER_SIZE    ( 32 )

static UBaseType_t xNumCmdInvocations = 0;
static UBaseType_t xExpectedCmdInvocations = 0;
static char * outputBuffer = NULL;
static size_t outputBufferLength = 0;
static char * pcCommand = NULL;


static BaseType_t prvCommandHandler( char * pcWriteBuffer,
                                     size_t xWriteBufferLen,
                                     const char * pcCommandString )
{
    BaseType_t xShouldInvokeAgain = pdTRUE;

    TEST_ASSERT_EQUAL( outputBuffer, pcWriteBuffer );
    TEST_ASSERT_EQUAL( outputBufferLength, xWriteBufferLen );
    TEST_ASSERT_NOT_NULL( pcCommandString );
    TEST_ASSERT_EQUAL( 0, strcmp( pcCommand, pcCommandString ) );

    xNumCmdInvocations++;

    if( xNumCmdInvocations == xExpectedCmdInvocations )
    {
        xShouldInvokeAgain = pdFALSE;
    }

    snprintf( pcWriteBuffer, xWriteBufferLen, "Command invocation %d output", xNumCmdInvocations );

    return xShouldInvokeAgain;
}

static const CLI_Command_Definition_t xTestCommandNoParams =
{
    "cmd_no_params",
    "\r\nDummy command with no parameters\r\n",
    prvCommandHandler,
    0
};

static const CLI_Command_Definition_t xTestCommandFixedParams =
{
    "cmd_two_params",
    "\r\nDummy command with two parameters\r\n",
    prvCommandHandler,
    2
};

static const CLI_Command_Definition_t xTestCommandVariableParams =
{
    "cmd_var_params",
    "\r\nDummy command with variable parameters\r\n",
    prvCommandHandler,
    -1
};

TEST_GROUP( FreeRTOS_CLI );

TEST_SETUP( FreeRTOS_CLI )
{
    static BaseType_t xCommandsRegistered = pdFALSE;


    /**
     * By default invoke the command only once.
     */
    xNumCmdInvocations = 0;
    xExpectedCmdInvocations = 1;
    pcCommand = NULL;

    /**
     * Defaults to FreeRTOS CLI provided output buffer.
     */
    outputBuffer = FreeRTOS_CLIGetOutputBuffer();
    TEST_ASSERT_NOT_NULL( outputBuffer );
    outputBufferLength = configCOMMAND_INT_MAX_OUTPUT_SIZE;
    memset( outputBuffer, 0x00, configCOMMAND_INT_MAX_OUTPUT_SIZE );

    /**
     * Register all commands at startup only once.
     */
    if( xCommandsRegistered == pdFALSE )
    {
        TEST_ASSERT_EQUAL( pdTRUE, FreeRTOS_CLIRegisterCommand( &xTestCommandNoParams ) );
        TEST_ASSERT_EQUAL( pdTRUE, FreeRTOS_CLIRegisterCommand( &xTestCommandFixedParams ) );
        TEST_ASSERT_EQUAL( pdTRUE, FreeRTOS_CLIRegisterCommand( &xTestCommandVariableParams ) );
        xCommandsRegistered = pdTRUE;
    }
}

TEST_TEAR_DOWN( FreeRTOS_CLI )
{
}

TEST_GROUP_RUNNER( FreeRTOS_CLI )
{
    RUN_TEST_CASE( FreeRTOS_CLI, ExecuteCommandSingleInvocation );
    RUN_TEST_CASE( FreeRTOS_CLI, ExecuteCommandMultipleInvocations );
    RUN_TEST_CASE( FreeRTOS_CLI, ExecuteCommandWithFixedParams );
    RUN_TEST_CASE( FreeRTOS_CLI, ExecuteCommandWithVariableParams );
    RUN_TEST_CASE( FreeRTOS_CLI, EmptyInput );
    RUN_TEST_CASE( FreeRTOS_CLI, InputWithNonRegisteredCommand );
    RUN_TEST_CASE( FreeRTOS_CLI, InputWithLesserThanExpectedParams )
    RUN_TEST_CASE( FreeRTOS_CLI, InputWithGreaterThanExpectedParams )
    RUN_TEST_CASE( FreeRTOS_CLI, InvalidInput )
    RUN_TEST_CASE( FreeRTOS_CLI, MultipleWhitespacesInInput )
    RUN_TEST_CASE( FreeRTOS_CLI, UserProvidedOutputBuffer )
    RUN_TEST_CASE( FreeRTOS_CLI, ExecuteHelpCommand )
}


TEST( FreeRTOS_CLI, ExecuteCommandSingleInvocation )
{
    /**
     * Execute the command without any params.
     * Verify the command handler is invoked only once and CLI returns pdFALSE to inform there are
     * no more invocations.
     */

    pcCommand = "cmd_no_params";
    TEST_ASSERT_EQUAL( pdFALSE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, strncmp( outputBuffer, "Command invocation 1 output", outputBufferLength ) );
    TEST_ASSERT_EQUAL( 1, xNumCmdInvocations );
}

TEST( FreeRTOS_CLI, ExecuteCommandMultipleInvocations )
{
    /**
     * Execute the command without any params. Command handler needs to be invoked twice.
     * CLI returns pdTRUE then pdFALSE to inform that there are no more invocations.
     */
    pcCommand = "cmd_no_params";
    xExpectedCmdInvocations = 2;

    TEST_ASSERT_EQUAL( pdTRUE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, strncmp( outputBuffer, "Command invocation 1 output", outputBufferLength ) );
    TEST_ASSERT_EQUAL( pdFALSE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, strncmp( outputBuffer, "Command invocation 2 output", outputBufferLength ) );
    TEST_ASSERT_EQUAL( xExpectedCmdInvocations, xNumCmdInvocations );
}

TEST( FreeRTOS_CLI, ExecuteCommandWithFixedParams )
{
    pcCommand = "cmd_two_params 1 two";
    const char * param = NULL;
    BaseType_t xParamLength;
    TEST_ASSERT_EQUAL( pdFALSE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, strncmp( outputBuffer, "Command invocation 1 output", outputBufferLength ) );
    TEST_ASSERT_EQUAL( 1, xNumCmdInvocations );

    param = FreeRTOS_CLIGetParameter( pcCommand, 1, &xParamLength );
    TEST_ASSERT_NOT_NULL( param );
    TEST_ASSERT_EQUAL( 1, xParamLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "1", param, xParamLength ) );

    param = FreeRTOS_CLIGetParameter( pcCommand, 2, &xParamLength );
    TEST_ASSERT_NOT_NULL( param );
    TEST_ASSERT_EQUAL( 3, xParamLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "two", param, xParamLength ) );

    /**
     * Out of bound fetch should return NULL.
     */
    param = FreeRTOS_CLIGetParameter( pcCommand, 3, &xParamLength );
    TEST_ASSERT_NULL( param );
}

TEST( FreeRTOS_CLI, ExecuteCommandWithVariableParams )
{
    pcCommand = "cmd_var_params  one two three";
    const char * param = NULL;
    BaseType_t xParamLength;
    TEST_ASSERT_EQUAL( pdFALSE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, strncmp( outputBuffer, "Command invocation 1 output", outputBufferLength ) );
    TEST_ASSERT_EQUAL( 1, xNumCmdInvocations );

    param = FreeRTOS_CLIGetParameter( pcCommand, 1, &xParamLength );
    TEST_ASSERT_NOT_NULL( param );
    TEST_ASSERT_EQUAL( 3, xParamLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "one", param, xParamLength ) );

    param = FreeRTOS_CLIGetParameter( pcCommand, 2, &xParamLength );
    TEST_ASSERT_NOT_NULL( param );
    TEST_ASSERT_EQUAL( 3, xParamLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "two", param, xParamLength ) );

    param = FreeRTOS_CLIGetParameter( pcCommand, 3, &xParamLength );
    TEST_ASSERT_NOT_NULL( param );
    TEST_ASSERT_EQUAL( 5, xParamLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "three", param, xParamLength ) );

    /**
     * Out of bound fetch should return NULL.
     */
    param = FreeRTOS_CLIGetParameter( pcCommand, 4, &xParamLength );
    TEST_ASSERT_NULL( param );
}

TEST( FreeRTOS_CLI, EmptyInput )
{
    /**
     * Passing an empty input should return pdFALSE, with an error log copied in the output buffer.
     * There should not be any command invocations.
     */
    pcCommand = "";
    BaseType_t xParamLength;
    TEST_ASSERT_EQUAL( pdFALSE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, xNumCmdInvocations );
    TEST_ASSERT_EQUAL( 0, strncmp( "Command not recognised.  Enter 'help' to view a list of available commands.\r\n\r\n", outputBuffer, outputBufferLength ) );
    TEST_ASSERT_NULL( FreeRTOS_CLIGetParameter( pcCommand, 1, &xParamLength ) );
}

TEST( FreeRTOS_CLI, InputWithNonRegisteredCommand )
{
    /**
     * Any command not registered should return pdFALSE, with an error log copied in the output buffer.
     * There should not be any command invocations.
     */
    pcCommand = "nop";
    BaseType_t xParamLength;
    TEST_ASSERT_EQUAL( pdFALSE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, xNumCmdInvocations );
    TEST_ASSERT_EQUAL( 0, strncmp( "Command not recognised.  Enter 'help' to view a list of available commands.\r\n\r\n", outputBuffer, outputBufferLength ) );
    TEST_ASSERT_NULL( FreeRTOS_CLIGetParameter( pcCommand, 1, &xParamLength ) );
}

TEST( FreeRTOS_CLI, InputWithLesserThanExpectedParams )
{
    /**
     * Input with lesser number of params should return pdFALSE, with an error log copied in the output buffer.
     * There should not be any command invocations.
     */
    pcCommand = "cmd_two_params one";
    TEST_ASSERT_EQUAL( pdFALSE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, xNumCmdInvocations );
    TEST_ASSERT_EQUAL( 0, strncmp( "Incorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n", outputBuffer, outputBufferLength ) );
}

TEST( FreeRTOS_CLI, InputWithGreaterThanExpectedParams )
{
    /**
     * Input with non expected number of params should return pdFALSE, with an error log copied in the output buffer.
     * There should not be any command invocations.
     */
    pcCommand = "cmd_two_params one two three";
    TEST_ASSERT_EQUAL( pdFALSE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, xNumCmdInvocations );
    TEST_ASSERT_EQUAL( 0, strncmp( "Incorrect command parameter(s).  Enter \"help\" to view a list of available commands.\r\n\r\n", outputBuffer, outputBufferLength ) );
}

TEST( FreeRTOS_CLI, InvalidInput )
{
    /**
     * Input with a null character in between. CLI should log error saying
     */
    pcCommand = "cmd_\0two_params one two three";
    BaseType_t xParamLength;
    TEST_ASSERT_EQUAL( pdFALSE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, xNumCmdInvocations );
    TEST_ASSERT_EQUAL( 0, strncmp( "Command not recognised.  Enter 'help' to view a list of available commands.\r\n\r\n", outputBuffer, outputBufferLength ) );
    TEST_ASSERT_NULL( FreeRTOS_CLIGetParameter( pcCommand, 1, &xParamLength ) );
}

TEST( FreeRTOS_CLI, MultipleWhitespacesInInput )
{
    pcCommand = "cmd_two_params    one   two";
    const char * param = NULL;
    BaseType_t xParamLength;
    TEST_ASSERT_EQUAL( pdFALSE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, strncmp( outputBuffer, "Command invocation 1 output", outputBufferLength ) );
    TEST_ASSERT_EQUAL( 1, xNumCmdInvocations );

    param = FreeRTOS_CLIGetParameter( pcCommand, 1, &xParamLength );
    TEST_ASSERT_NOT_NULL( param );
    TEST_ASSERT_EQUAL( 3, xParamLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "one", param, xParamLength ) );

    param = FreeRTOS_CLIGetParameter( pcCommand, 2, &xParamLength );
    TEST_ASSERT_NOT_NULL( param );
    TEST_ASSERT_EQUAL( 3, xParamLength );
    TEST_ASSERT_EQUAL( 0, strncmp( "two", param, xParamLength ) );

    /**
     * Out of bound fetch should return NULL.
     */
    param = FreeRTOS_CLIGetParameter( pcCommand, 3, &xParamLength );
    TEST_ASSERT_NULL( param );
}

TEST( FreeRTOS_CLI, UserProvidedOutputBuffer )
{
    pcCommand = "cmd_no_params";
    char userOutputBuffer[ USER_OUTPUT_BUFFER_SIZE ] = { 0 };
    outputBuffer = userOutputBuffer;
    outputBufferLength = USER_OUTPUT_BUFFER_SIZE;

    TEST_ASSERT_EQUAL( pdFALSE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, strncmp( outputBuffer, "Command invocation 1 output", outputBufferLength ) );
    TEST_ASSERT_EQUAL( 1, xNumCmdInvocations );
}

TEST( FreeRTOS_CLI, ExecuteHelpCommand )
{
    pcCommand = "help";
    TEST_ASSERT_EQUAL( pdTRUE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, strncmp( outputBuffer, "\r\nhelp:\r\n Lists all the registered commands\r\n\r\n", outputBufferLength ) );
    TEST_ASSERT_EQUAL( pdTRUE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, strncmp( outputBuffer, "\r\nDummy command with no parameters\r\n", outputBufferLength ) );
    TEST_ASSERT_EQUAL( pdTRUE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, strncmp( outputBuffer, "\r\nDummy command with two parameters\r\n", outputBufferLength ) );
    TEST_ASSERT_EQUAL( pdFALSE, FreeRTOS_CLIProcessCommand( pcCommand, outputBuffer, outputBufferLength ) );
    TEST_ASSERT_EQUAL( 0, strncmp( outputBuffer, "\r\nDummy command with variable parameters\r\n", outputBufferLength ) );
}
