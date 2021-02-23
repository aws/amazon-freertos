/*
 * FreeRTOS V1.4.8
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
#ifndef AWS_DEMO_LOGGING_H
#define AWS_DEMO_LOGGING_H

/*
 * Initialise a logging system that can be used from FreeRTOS tasks and Win32
 * threads.  Do not call printf() directly while the scheduler is running.
 *
 * Set xLogToStdout, xLogToFile and xLogToUDP to either pdTRUE or pdFALSE to
 * log to stdout, a disk file and a UDP port respectively.
 *
 * If xLogToUDP is pdTRUE then ulRemoteIPAddress and usRemotePort must be set
 * to the IP address and port number to which UDP log messages will be sent.
 */
void vLoggingInit( BaseType_t xLogToStdout,
                   BaseType_t xLogToFile,
                   BaseType_t xLogToUDP,
                   uint32_t ulRemoteIPAddress,
                   uint16_t usRemotePort );

/**
 * @brief Interface for logging strings in
 * from FreeRTOS tasks in the Windows platform.
 *
 * Depending on the configuration made through vLoggingInit(),
 * this function will print to stdout, log to disk file OR
 * transmit over a UDP port.
 */
void vLoggingPrint( const char * pcFormat );

/**
 * @brief Printf like logging interface to log messages from FreeRTOS
 * tasks in Windows platform.
 *
 * Depending on the configuration made through vLoggingInit(),
 * this function will print to stdout, log to disk file OR
 * transmit over a UDP port.
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
 * Depending on the configuration made through vLoggingInit(),
 * this function will print to stdout, log to disk file OR
 * transmit over a UDP port.
 */
void vLoggingPrintfError( const char * pcFormat,
                          ... );

/**
 * @brief Interface for logging message at Warn level.
 *
 * This function adds a "[WARN]" prefix to the
 * log message to label it as a warning.
 *
 * Depending on the configuration made through vLoggingInit(),
 * this function will print to stdout, log to disk file OR
 * transmit over a UDP port.
 */
void vLoggingPrintfWarn( const char * pcFormat,
                         ... );

/**
 * @brief Interface for logging message at Info level.
 *
 * This function adds a "[INFO]" prefix to the
 * log message to label it as an informational message.
 *
 * Depending on the configuration made through vLoggingInit(),
 * this function will print to stdout, log to disk file OR
 * transmit over a UDP port.
 */
void vLoggingPrintfInfo( const char * pcFormat,
                         ... );

/**
 * @brief Interface for logging message at Debug level.
 *
 * This function adds a "[DEBUG]" prefix to the
 * log message to label it as a debug level message.
 *
 * Depending on the configuration made through vLoggingInit(),
 * this function will print to stdout, log to disk file OR
 * transmit over a UDP port.
 */
void vLoggingPrintfDebug( const char * pcFormat,
                          ... );

#endif /* AWS_DEMO_LOGGING_H */
