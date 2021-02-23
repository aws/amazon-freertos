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
 */

/**
 * @file logging_stack.h
 * @brief Reference implementation of Logging stack as a header-only library.
 */

#ifndef LOGGING_STACK_H_
#define LOGGING_STACK_H_

/* Include header for logging level macros. */
#include "logging_levels.h"

/* FreeRTOS Include. */
#include "FreeRTOS.h"
#include "iot_logging_task.h"

/* Standard Include. */
#include <stdint.h>

/**
 * @brief This config enables extra verbosity in log messages with metadata information
 * about the source library and location of the log message.
 * This requires that the toolchain support C99 (for variadic macros) and the GNU
 * extension for comma elision in variadic macros (with ##__VA_ARGS__).
 * If this flag is enabled, you can change the metadata information from the logging_stack.h
 * file.
 *
 * @note By default, this configuration is disabled by
 */
#ifndef LOGGING_ENABLE_METADATA_WITH_C99_AND_GNU_EXTENSION
    #define LOGGING_ENABLE_METADATA_WITH_C99_AND_GNU_EXTENSION    0
#endif

#if LOGGING_ENABLE_METADATA_WITH_C99_AND_GNU_EXTENSION == 1

    #define SdkLogError( message )           SdkLogErrorC99 message
    #define SdkLogErrorC99( format, ... )    vLoggingPrintfWithFileAndLine( __FILE__, __LINE__, "[ERROR] [%s] " format "\r\n", LIBRARY_LOG_NAME, ## __VA_ARGS__ )
    #define SdkLogWarn( message )            SdkLogWarnC99 message
    #define SdkLogWarnC99( format, ... )     vLoggingPrintfWithFileAndLine( __FILE__, __LINE__, "[WARN] [%s] " format "\r\n", LIBRARY_LOG_NAME, ## __VA_ARGS__ )
    #define SdkLogInfo( message )            SdkLogInfoC99 message
    #define SdkLogInfoC99( format, ... )     vLoggingPrintfWithFileAndLine( __FILE__, __LINE__, "[INFO] [%s] " format "\r\n", LIBRARY_LOG_NAME, ## __VA_ARGS__ )
    #define SdkLogDebug( message )           SdkLogDebugC99 message
    #define SdkLogDebugC99( format, ... )    vLoggingPrintfWithFileAndLine( __FILE__, __LINE__, "[DEBUG] [%s] " format "\r\n", LIBRARY_LOG_NAME, ## __VA_ARGS__ )
#else /* if LOGGING_ENABLE_METADATA_WITH_C99_AND_GNU_EXTENSION == 1 */
    #define SdkLogError( message )           vLoggingPrintfError message
    #define SdkLogWarn( message )            vLoggingPrintfWarn message
    #define SdkLogInfo( message )            vLoggingPrintfInfo message
    #define SdkLogDebug( message )           vLoggingPrintfDebug message
#endif /* if defined( LOGGING_METADATA_WITH_C99_SUPPORT ) && ( LOGGING_METADATA_WITH_C99_SUPPORT == 1 ) */

/* Check that LIBRARY_LOG_LEVEL is defined and has a valid value. */
#if !defined( LIBRARY_LOG_LEVEL ) ||       \
    ( ( LIBRARY_LOG_LEVEL != LOG_NONE ) && \
    ( LIBRARY_LOG_LEVEL != LOG_ERROR ) &&  \
    ( LIBRARY_LOG_LEVEL != LOG_WARN ) &&   \
    ( LIBRARY_LOG_LEVEL != LOG_INFO ) &&   \
    ( LIBRARY_LOG_LEVEL != LOG_DEBUG ) )
    #error "Please define LIBRARY_LOG_LEVEL as either LOG_NONE, LOG_ERROR, LOG_WARN, LOG_INFO, or LOG_DEBUG."
#elif !defined( LIBRARY_LOG_NAME )
    #error "Please define LIBRARY_LOG_NAME for the library."
#else
    #if LIBRARY_LOG_LEVEL == LOG_DEBUG
        /* All log level messages will logged. */
        #define LogError( message )    SdkLogError( message )
        #define LogWarn( message )     SdkLogWarn( message )
        #define LogInfo( message )     SdkLogInfo( message )
        #define LogDebug( message )    SdkLogDebug( message )

    #elif LIBRARY_LOG_LEVEL == LOG_INFO
        /* Only INFO, WARNING and ERROR messages will be logged. */
        #define LogError( message )    SdkLogError( message )
        #define LogWarn( message )     SdkLogWarn( message )
        #define LogInfo( message )     SdkLogInfo( message )
        #define LogDebug( message )

    #elif LIBRARY_LOG_LEVEL == LOG_WARN
        /* Only WARNING and ERROR messages will be logged.*/
        #define LogError( message )    SdkLogError( message )
        #define LogWarn( message )     SdkLogWarn( message )
        #define LogInfo( message )
        #define LogDebug( message )

    #elif LIBRARY_LOG_LEVEL == LOG_ERROR
        /* Only ERROR messages will be logged. */
        #define LogError( message )    SdkLogError( message )
        #define LogWarn( message )
        #define LogInfo( message )
        #define LogDebug( message )

    #else /* if LIBRARY_LOG_LEVEL == LOG_ERROR */

        #define LogError( message )
        #define LogWarn( message )
        #define LogInfo( message )
        #define LogDebug( message )

    #endif /* if LIBRARY_LOG_LEVEL == LOG_ERROR */
#endif /* if !defined( LIBRARY_LOG_LEVEL ) || ( ( LIBRARY_LOG_LEVEL != LOG_NONE ) && ( LIBRARY_LOG_LEVEL != LOG_ERROR ) && ( LIBRARY_LOG_LEVEL != LOG_WARN ) && ( LIBRARY_LOG_LEVEL != LOG_INFO ) && ( LIBRARY_LOG_LEVEL != LOG_DEBUG ) ) */

#endif /* ifndef LOGGING_STACK_H_ */
