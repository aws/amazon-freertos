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
 */

/**
 * @file logging_levels.h
 * @brief Defines the logging level macros.
 */

#ifndef LOGGING_LEVELS_H_
#define LOGGING_LEVELS_H_

/**
 * @brief No log messages.
 *
 * When @ref LIBRARY_LOG_LEVEL is #LOG_NONE, logging is disabled and no
 * logging messages are printed.
 */
#define LOG_NONE     0

/**
 * @brief Represents erroneous application state or event.
 *
 * These messages describe the situations when a library encounters an error from
 * which it cannot recover.
 *
 * These messages are printed when @ref LIBRARY_LOG_LEVEL is defined as either
 * of #LOG_ERROR, #LOG_WARN, #LOG_INFO or #LOG_DEBUG.
 */
#define LOG_ERROR    1

/**
 * @brief Message about an abnormal event.
 *
 * These messages describe the situations when a library encounters
 * abnormal event that may be indicative of an error. Libraries continue
 * execution after logging a warning.
 *
 * These messages are printed when @ref LIBRARY_LOG_LEVEL is defined as either
 * of #LOG_WARN, #LOG_INFO or #LOG_DEBUG.
 */
#define LOG_WARN     2

/**
 * @brief A helpful, informational message.
 *
 * These messages describe normal execution of a library. They provide
 * the progress of the program at a coarse-grained level.
 *
 * These messages are printed when @ref LIBRARY_LOG_LEVEL is defined as either
 * of #LOG_INFO or #LOG_DEBUG.
 */
#define LOG_INFO     3

/**
 * @brief Detailed and excessive debug information.
 *
 * Debug log messages are used to provide the
 * progress of the program at a fine-grained level. These are mostly used
 * for debugging and may contain excessive information such as internal
 * variables, buffers, or other specific information.
 *
 * These messages are only printed when @ref LIBRARY_LOG_LEVEL is defined as
 * #LOG_DEBUG.
 */
#define LOG_DEBUG    4

/* The macro definition for LIBRARY_LOG_LEVEL is for Doxygen
 * documentation only. This macro is typically defined in only the
 * <library>_config.h file or the demo_config.h file. */

/**
 * @brief The logging level verbosity configuration of log messages from library.
 *
 * The logging verbosity levels are one of #LOG_DEBUG, #LOG_INFO, #LOG_WARN,
 * and #LOG_ERROR.
 * - With level #LOG_NONE, logging will be disabled.
 * - With level #LOG_DEBUG, all log messages will print.
 * - With level #LOG_INFO, all log messages, except level #LOG_DEBUG, will print.
 * - With level #LOG_WARN, only messages this level and #LOG_ERROR level will print.
 * - With level #LOG_ERROR, only messages at this level will print.
 */
#ifdef DOXYGEN
    #define LIBRARY_LOG_LEVEL    LOG_ERROR
#endif

#endif /* ifndef LOGGING_LEVELS_H_ */
