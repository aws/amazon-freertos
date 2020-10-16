/*
 * AWS IoT Device SDK for Embedded C V202009.00
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

/* Standard Include. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* The macro definition for LIBRARY_LOG_NAME is for Doxygen
 * documentation only. This macro is typically defined in only the
 * <library>_config.h file or the demo_config.h file. */

/**
 * @brief The name of the library or demo to add as metadata in log messages
 * from the library or demo.
 *
 * This metadata aids in identifying the module source of log messages.
 * The metadata is logged in the format `[ <LIBRARY-NAME> ]` as a prefix to the
 * log messages.
 * Refer to #LOG_METADATA_FORMAT for the complete format of the metadata prefix in
 * log messages.
 */
#ifdef DOXYGEN
    #define LIBRARY_LOG_NAME    "<LIBRARY_NAME>"
#endif

/* Check if LIBRARY_LOG_NAME macro has been defined. */
#if !defined( LIBRARY_LOG_NAME )
    #error "Please define LIBRARY_LOG_NAME for the library."
#endif

/**
 * @brief Macro to extract only the file name from file path to use for metadata in
 * log messages.
 */
#define FILENAME               ( strrchr( __FILE__, '/' ) ? strrchr( __FILE__, '/' ) + 1 : __FILE__ )

/* Metadata information to prepend to every log message. */
#define LOG_METADATA_FORMAT    "[%s] [%s:%d] "                      /**< @brief Format of metadata prefix in log messages as `[<Logging-Level>] [<Library-Name>] [<File-Name>:<Line-Number>]` */
#define LOG_METADATA_ARGS      LIBRARY_LOG_NAME, FILENAME, __LINE__ /**< @brief Arguments into the metadata logging prefix format. */

#if !defined( DISABLE_LOGGING )

/**
 * @brief Common macro that maps all the logging interfaces,
 * (#LogDebug, #LogInfo, #LogWarn, #LogError) to the platform-specific logging
 * function.
 *
 * `printf` from the standard C library is the POSIX platform implementation used
 * for logging functionality.
 */
    #define SdkLog( string )    printf string
#else
    #define SdkLog( string )
#endif

/**
 * Disable definition of logging interface macros when generating doxygen output,
 * to avoid conflict with documentation of macros at the end of the file.
 * @cond DOXYGEN_IGNORE
 */
/* Check that LIBRARY_LOG_LEVEL is defined and has a valid value. */
#if !defined( LIBRARY_LOG_LEVEL ) ||       \
    ( ( LIBRARY_LOG_LEVEL != LOG_NONE ) && \
    ( LIBRARY_LOG_LEVEL != LOG_ERROR ) &&  \
    ( LIBRARY_LOG_LEVEL != LOG_WARN ) &&   \
    ( LIBRARY_LOG_LEVEL != LOG_INFO ) &&   \
    ( LIBRARY_LOG_LEVEL != LOG_DEBUG )     \
    )
    #error "Please define LIBRARY_LOG_LEVEL as either LOG_NONE, LOG_ERROR, LOG_WARN, LOG_INFO, or LOG_DEBUG."
#else
    #if LIBRARY_LOG_LEVEL == LOG_DEBUG
        /* All log level messages will logged. */
        #define LogError( message )    SdkLog( ( "[ERROR] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
        #define LogWarn( message )     SdkLog( ( "[WARN] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
        #define LogInfo( message )     SdkLog( ( "[INFO] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
        #define LogDebug( message )    SdkLog( ( "[DEBUG] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )

    #elif LIBRARY_LOG_LEVEL == LOG_INFO
        /* Only INFO, WARNING and ERROR messages will be logged. */
        #define LogError( message )    SdkLog( ( "[ERROR] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
        #define LogWarn( message )     SdkLog( ( "[WARN] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
        #define LogInfo( message )     SdkLog( ( "[INFO] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
        #define LogDebug( message )

    #elif LIBRARY_LOG_LEVEL == LOG_WARN
        /* Only WARNING and ERROR messages will be logged.*/
        #define LogError( message )    SdkLog( ( "[ERROR] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
        #define LogWarn( message )     SdkLog( ( "[WARN] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
        #define LogInfo( message )
        #define LogDebug( message )

    #elif LIBRARY_LOG_LEVEL == LOG_ERROR
        /* Only ERROR messages will be logged. */
        #define LogError( message )    SdkLog( ( "[ERROR] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )
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
/** @endcond */

/* Doxygen documentation of logging interface macro definitions for Doxygen. */
#ifdef DOXYGEN

/**
 * @brief Definition of logging interface macro that logs messages at the "Debug"
 * level, when debug level logging is enabled.
 *
 * This macro is only enabled for #LOG_DEBUG level configuration in this
 * logging stack implementation.
 */
    #define LogDebug( message )    SdkLog( ( "[DEBUG] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )

/**
 * @brief Definition of logging interface macro that logs messages at the "Info"
 * level, when info level logging is enabled.
 *
 * This macro is only enabled for #LOG_DEBUG and #LOG_INFO level configurations
 * in this logging stack implementation.
 */
    #define LogInfo( message )     SdkLog( ( "[INFO] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )

/**
 * @brief Definition of logging interface macro that logs messages at the "Warning"
 * level, when warning level logging is enabled.
 *
 * This macro is only enabled for #LOG_DEBUG, #LOG_INFO and #LOG_WARN level
 * configurations in this logging stack implementation.
 */
    #define LogWarn( message )     SdkLog( ( "[WARN] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )

/**
 * @brief Definition of logging interface macro that logs messages at the "Error"
 * level, when error level logging is enabled.
 *
 * This macro is only enabled for all logging level configurations
 * unless except the #LOG_NONE configuration.
 */
    #define LogError( message )    SdkLog( ( "[ERROR] "LOG_METADATA_FORMAT, LOG_METADATA_ARGS ) ); SdkLog( message ); SdkLog( ( "\r\n" ) )

#endif /* ifdef DOXYGEN */

#endif /* ifndef LOGGING_STACK_H_ */
