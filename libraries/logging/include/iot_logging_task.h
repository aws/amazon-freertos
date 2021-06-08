/*
 * FreeRTOS Common V1.1.3
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

/**
 * @file iot_logging_task.h
 * @brief Logging task interface.
 */

#ifndef AWS_LOGGING_TASK_H
#define AWS_LOGGING_TASK_H

#ifndef INC_FREERTOS_H
    #error "include FreeRTOS.h must appear in source files before include iot_logging_task.h"
#endif

/**
 * @brief Initialization function for logging task.
 *
 * Called once to create the logging task and queue.  Must be called before any
 * calls to vLoggingPrintf().
 */
BaseType_t xLoggingTaskInitialize( uint16_t usStackSize,
                                   UBaseType_t uxPriority,
                                   UBaseType_t uxQueueLength );

/**
 * @brief Interface to print via the logging interface.
 *
 * Uses the same semantics as printf().  How the print is performed depends on
 * which files are being built.  Some vLoggingPrintf() implementations will
 * output directly, others will use a logging task to allow log message to be
 * output in the background should the output device be too slow for output to
 * be performed inline.
 *
 * @param[in] pcFormat The format string of the log message.
 * @param[in] ... The variadic list of parameters for the format
 * specifiers in the @p pcFormat.
 */
void vLoggingPrintf( const char * pcFormat,
                     ... );

/**
 * @brief Same as vLoggingPrintf but additionally takes parameters
 * of source file location of log to add as metadata in message.
 *
 * @param[in] pcFile The file name or file path containing the log
 * message. If a file path is passed, only the filename is added to
 * the log message.
 * @param[in] fileLineNo The line number in the @p pcFile containing
 * the message being logged.
 * @param[in] pcFormat The format string of the log message.
 * @param[in] ... The variadic list of parameters for the format
 * specifiers in the @p pcFormat.
 */
void vLoggingPrintfWithFileAndLine( const char * pcFile,
                                    size_t fileLineNo,
                                    const char * pcFormat,
                                    ... );

/**
 * @brief Interface for logging message at Error level.
 *
 * This function adds a "[ERROR]" prefix to the
 * log message to label it as an error.
 *
 * @param[in] pcFormat The format string of the log message.
 * @param[in] ... The variadic list of parameters for the format
 * specifiers in the @p pcFormat.
 */
void vLoggingPrintfError( const char * pcFormat,
                          ... );

/**
 * @brief Interface for logging message at Warn level.
 *
 * This function adds a "[WARN]" prefix to the
 * log message to label it as a warning.
 *
 * @param[in] pcFormat The format string of the log message.
 * @param[in] ... The variadic list of parameters for the format
 * specifiers in the @p pcFormat.
 */
void vLoggingPrintfWarn( const char * pcFormat,
                         ... );

/**
 * @brief Interface for logging message at Info level.
 *
 * This function adds a "[INFO]" prefix to the
 * log message to label it as an informational message.
 *
 * @param[in] pcFormat The format string of the log message.
 * @param[in] ... The variadic list of parameters for the format
 * specifiers in the @p pcFormat.
 */
void vLoggingPrintfInfo( const char * pcFormat,
                         ... );

/**
 * @brief Interface for logging message at Debug level.
 *
 * This function adds a "[DEBUG]" prefix to the
 * log message to label it as a debug level message.
 *
 * @param[in] pcFormat The format string of the log message.
 * @param[in] ... The variadic list of parameters for the format
 * specifiers in the @p pcFormat.
 */
void vLoggingPrintfDebug( const char * pcFormat,
                          ... );

#endif /* AWS_LOGGING_TASK_H */
