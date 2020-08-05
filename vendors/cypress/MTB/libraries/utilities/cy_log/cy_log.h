/*
 * Copyright 2019-2020 Cypress Semiconductor Corporation
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file
 * A logging subsystem that allows run time control for the logging level.
 * Log messages are passed back to the application for output.
 * Log messages are given sequence numbers.
 * A time callback can be provided by the application for the timestamp for each output line.
 * Log messages are mutex protected across threads so that log messages do not interrupt each other.
 */
/*
 * in Main application file:
 *
 *  Log output callback function - The App decides what and how logging is to be output
 *
 *  int app_log_output_callback(CY_LOG_FACILITY_T facility, CY_LOG_LEVEL_T level, char *logmsg)
 *  {
 *      (void)facility;     // Can be used to decide to reduce output or send output to remote logging
 *      (void)level;        // Can be used to decide to reduce output, although the output has already been
 *                          // limited by the log routines
 *
 *      return printf( "%s\n", logmsg);   // print directly to console
 *  }
 *
 *
 *  Log time callback - get the current time for the log message timestamp in millseconds
 *
 *  cy_rslt_t app_log_time(uint32_t* time)
 *  {
 *      if (time != NULL)
 *      {
 *          *time = get_time_ms(); // get system time (in milliseconds)
 *      }
 *      return CY_RSLT_SUCCESS;
 *  }
 *
 *
 *  Log initialization - default os OFF, no output from any facility
 *
 *  result = cy_log_init(CY_LOG_OFF, app_log_output_callback, app_log_time);
 *  if (result != CY_RSLT_SUCCESS)
 *  {
 *      printf("cy_log_init() FAILED %ld\n", result);
 *  }
 *
 *
 *  Example using TEST facility
 *
 *  cy_log_set_facility_level(CYLF_TEST, CY_LOG_WARNING);           // set log message level to WARNING
 *
 *  cy_log_printf("TEST message: always print.");                   // Bypass facility/level check and always print message
 *                                                                  // calls app_log_output_callback(CYLF_DEF, CY_LOG_PRINTF, logmsg)
 *
 *  cy_log_msg(CYLF_TEST, CY_LOG_ERR,     "TEST message: ERR");     // Print if CYLF_TEST level is CY_LOG_ERR or higher
 *  cy_log_msg(CYLF_TEST, CY_LOG_WARNING, "TEST message: WARNING"); // Print if CYLF_TEST level is CY_LOG_WARNING or higher
 *  cy_log_msg(CYLF_TEST, CY_LOG_NOTICE,  "TEST message: NOTICE");  // Print if CYLF_TEST level is CY_LOG_NOTICE or higher
 *
 *  cy_log_msg(CYLF_DRIVER, CY_LOG_ERR,   "DRIVER message: ERR");   // Print if CYLF_DRIVER level is CY_LOG_ERR or higher
 *
 *  OUTPUT:
 *
 *  TEST message: always print.
 *  TEST message: ERR
 *  TEST message: WARNING
 *
 *  - No other CYLF_TEST output due to level set as CY_LOG_WARNING
 *  - No CYLF_DRIVER output due to level set as CY_LOG_OFF
 */

#pragma once

#include <stdarg.h>
#include "cy_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************************************/
/** \addtogroup group_logging_enums *//** \{ */
/******************************************************************************/

/** Logging levels. NOTE: Default value for all facilities is passed in to init call */
typedef enum
{
    CY_LOG_OFF = 0,     /**< Do not print log messages */
    CY_LOG_ERR,         /**< Print log message if run-time level is <= CY_LOG_ERR       */
    CY_LOG_WARNING,     /**< Print log message if run-time level is <= CY_LOG_WARNING   */
    CY_LOG_NOTICE,      /**< Print log message if run-time level is <= CY_LOG_NOTICE    */
    CY_LOG_INFO,        /**< Print log message if run-time level is <= CY_LOG_INFO      */
    CY_LOG_DEBUG,       /**< Print log message if run-time level is <= CY_LOG_DEBUG     */
    CY_LOG_DEBUG1,      /**< Print log message if run-time level is <= CY_LOG_DEBUG1    */
    CY_LOG_DEBUG2,      /**< Print log message if run-time level is <= CY_LOG_DEBUG2    */
    CY_LOG_DEBUG3,      /**< Print log message if run-time level is <= CY_LOG_DEBUG3    */
    CY_LOG_DEBUG4,      /**< Print log message if run-time level is <= CY_LOG_DEBUG4    */

    CY_LOG_PRINTF, /* Identifies log messages generated by cy_log_printf calls */

    CY_LOG_MAX
} CY_LOG_LEVEL_T;

/** Log Facility type
 * Log facilities allow for separate subsystems to have different run-time log levels for output.
 *  This allows for someone working in the Driver subsystem to turn on DEBUG level without turning DEBUG
 *  level for middleware - makes for less unwanted output during debugging / testing.
 */
typedef enum
{
    CYLF_DEF = 0,                   /**< General log message not associated with any specific Facility */
    CYLF_TEST,                      /**< Test Facility */
    CYLF_DRIVER,                    /**< Driver Facility */
    CYLF_LP,                        /**< Low Power Facility */
    CYLF_MIDDLEWARE,                /**< Middleware Facility */
    CYLF_AUDIO,                     /**< Audio Facility */

    CYLF_MAX                        /**< Must be last, not an actual index */
} CY_LOG_FACILITY_T;

/** \} */

/******************************************************************************/
/** \addtogroup group_logging_structures *//** \{ */
/******************************************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/** Prototype for application callback to use the logging message */
typedef int (*log_output)(CY_LOG_FACILITY_T facility, CY_LOG_LEVEL_T level, char *logmsg);

/** Prototype for application callback to get current time in milliseconds
 * */
typedef cy_rslt_t (*platform_get_time)(uint32_t* time);

/** \} */

/*****************************************************************************/
/**
 *
 *  @addtogroup group_logging_func
 *
 * A logging subsystem that allows run time control for the logging level.
 *
 *  @{
 */
/*****************************************************************************/
/******************************************************
 *               Function Declarations
 ******************************************************/
/** Initialize the logging subsystem.
 *
 * @param[in] level           : The initial logging level to use for all facilities.
 * @param[in] platform_output : Pointer to the function invoked by the library to output the log messages.
 *                              If this argument is passed as NULL, the library prints the log output on the standard output stream (stdio).
 * @param[in] platform_time   : Pointer to the function invoked by the library to get the time. If this argument is passed as NULL, the library calls
 *                              [cy_rtos_get_time](https://cypresssemiconductorco.github.io/abstraction-rtos/html/group__group__abstraction__rtos__time.html)
 *                              function to get the time required for logging.
 *
 * @return cy_rslt_t
 */
cy_rslt_t cy_log_init(CY_LOG_LEVEL_T level, log_output platform_output, platform_get_time platform_time);

/** Shutdown the logging subsystem.
 *
 * @return cy_rslt_t
 */
cy_rslt_t cy_log_shutdown(void);

/** Set the platform output routine for log messages.
 *
 * @note If platform_output is NULL, log messages will be discarded.
 *
 * @param[in] platform_output : Pointer to the platform output routine for log messages.
 *
 * @return cy_rslt_t
 */
cy_rslt_t cy_log_set_platform_output(log_output platform_output);

/** Set the platform routine for getting time stamps for log messages.
 *
 * @note If platform_time is NULL, cy_time_get_time() is used for time stamps.
 *
 * @param[in] platform_time   : Pointer to a platform time routine for log message time stamps.
 *
 * @return cy_rslt_t
 */
cy_rslt_t cy_log_set_platform_time(platform_get_time platform_time);

/** Set the logging level for a facility.
 *
 * @param[in] facility  : The facility for which to set the log level.
 * @param[in] level     : The new log level to use.
 *
 * @return cy_rslt_t
 */
cy_rslt_t cy_log_set_facility_level(CY_LOG_FACILITY_T facility, CY_LOG_LEVEL_T level);

/** Set the logging level for all facilities.
 *
 * @param[in] level  : The new log level to use.
 *
 * @return cy_rslt_t
 */
cy_rslt_t cy_log_set_all_levels(CY_LOG_LEVEL_T level);

/** Get the logging level for a facility.
 *
 * @param[in] facility  : The facility for which to return the log level.
 *
 * @return The current log level.
 */
CY_LOG_LEVEL_T cy_log_get_facility_level(CY_LOG_FACILITY_T facility);

/** Write a log message.
 *
 * @note The format arguments are the same as for printf.
 *
 * @param[in] facility  : The facility for the log message.
 * @param[in] level     : Log level of the message.
 * @param[in] fmt       : Format control string followed by any optional arguments.
 *
 * @return cy_rslt_t
 */
cy_rslt_t cy_log_msg(CY_LOG_FACILITY_T facility, CY_LOG_LEVEL_T level, const char *fmt, ...);

/** Write a log message bypassing the log level check.
 *
 * @note The format arguments are the same as for printf.
 *
 * @param[in] fmt    : Format control string followed by any optional arguments.
 *
 * @return cy_rslt_t
 */
cy_rslt_t cy_log_printf(const char *fmt, ...);

/** Write a log message bypassing the log level check using va_list
 *
 * @note The format arguments are the same as for vprintf.
 *
 * @param[in] fmt   : Format control string.
 * @param[in] varg  : va_list of arguments.
 *
 * @return cy_rslt_t
 */
cy_rslt_t cy_log_vprintf(const char *fmt, va_list varg);

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif
