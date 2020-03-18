/*
* $ Copyright Cypress Semiconductor Apache2 $
*/

#ifndef IPERF_DEBUG_H_
#define IPERF_DEBUG_H_

#include "iperf_util.h"

/******************************************************
 *             Debug configuration
 ******************************************************/

/*
 * IPERF_DBG_MIN_LEVEL: After masking, the value of the debug is compared
 * against this value. If it is smaller, then debugging messages are written.
 */
#ifndef IPERF_DBG_MIN_LEVEL
#define IPERF_DBG_MIN_LEVEL             IPERF_DBG_LEVEL_ALL
#endif /* IPERF_DBG_MIN_LEVEL */

/*
 * IPERF_DBG_TYPES_ON: A mask that can be used to globally enable/disable debug
 * messages of certain types.
 */
#ifndef IPERF_DBG_TYPES_ON
#define IPERF_DBG_TYPES_ON               IPERF_DBG_ON
#endif /* IPERF_DBG_TYPES_ON */

/*
 * CLIENT_DEBUG: Enable debugging in Client.cpp.
 */
#ifndef CLIENT_DEBUG
#define CLIENT_DEBUG                    IPERF_DBG_ON
#endif /* CLIENT_DEBUG */

/*
 * CONDITION_DEBUG: Enable debugging of conditions.
 */
#ifndef CONDITION_DEBUG
#define CONDITION_DEBUG                 IPERF_DBG_ON
#endif /* CONDITION_DEBUG */

/*
 * EXTRACTOR_DEBUG: Enable debugging in Extractor.c.
 */
#ifndef EXTRACTOR_DEBUG
#define EXTRACTOR_DEBUG                 IPERF_DBG_ON
#endif /* EXTRACTOR_DEBUG */

/*
 * LISTENER_DEBUG: Enable debugging in Listener.cpp.
 */
#ifndef LISTENER_DEBUG
#define LISTENER_DEBUG                  IPERF_DBG_ON
#endif /* LISTENER_DEBUG */

/*
 * MEMALLOC_DEBUG: Enable debugging of memory allocation.
 */
#ifndef MEMALLOC_DEBUG
#define MEMALLOC_DEBUG                  IPERF_DBG_ON
#endif /* MEMALLOC_DEBUG */

/*
 * MEMFREE_DEBUG: Enable debugging of memory freeing.
 */
#ifndef MEMFREE_DEBUG
#define MEMFREE_DEBUG                   IPERF_DBG_ON
#endif /* MEMFREE_DEBUG */

/*
 * MUTEX_DEBUG: Enable debugging of mutexes.
 */
#ifndef MUTEX_DEBUG
#define MUTEX_DEBUG                     IPERF_DBG_ON
#endif /* MUTEX_DEBUG */

/*
 * REPORTER_DEBUG: Enable debugging in Reporter.cpp.
 */
#ifndef REPORTER_DEBUG
#define REPORTER_DEBUG                  IPERF_DBG_ON
#endif /* REPORTER_DEBUG */

/*
 * SERVER_DEBUG: Enable debugging in Server.cpp.
 */
#ifndef SERVER_DEBUG
#define SERVER_DEBUG                    IPERF_DBG_ON
#endif /* SERVER_DEBUG */

/*
 * SOCKET_DEBUG: Enable debugging in sockets.
 */
#ifndef SOCKET_DEBUG
#define SOCKET_DEBUG                    IPERF_DBG_ON
#endif /* SOCKET_DEBUG */

/*
 * THREAD_DEBUG: Enable debugging in threading functions.
 */
#ifndef THREAD_DEBUG
#define THREAD_DEBUG                    IPERF_DBG_ON
#endif /* THREAD_DEBUG */

/*
 * TIME_DEBUG: Enable debugging in time related functions.
 */
#ifndef TIME_DEBUG
#define TIME_DEBUG                      IPERF_DBG_ON
#endif /* TIME_DEBUG */

/*
 * DEBUG_DIVISOR: Only debug every nth packet operations. This can be used to
 * suppress the (otherwise very verbose) output.
 */
#ifndef DEBUG_DIVISOR
#define DEBUG_DIVISOR                   1000
#endif /* DEBUG_DIVISOR */

/*
 * Lower two bits indicate debug level
 * - 0 all
 * - 1 warning
 * - 2 serious
 * - 3 severe
 */
#define IPERF_DBG_LEVEL_ALL     0x00
#define IPERF_DBG_LEVEL_OFF     IPERF_DBG_LEVEL_ALL /* compatibility define only */
#define IPERF_DBG_LEVEL_WARNING 0x01
#define IPERF_DBG_LEVEL_SERIOUS 0x02
#define IPERF_DBG_LEVEL_SEVERE  0x03
#define IPERF_DBG_MASK_LEVEL    0x03

/*
 * Flag for IPERF_DEBUGF to enable that debug message.
 */
#define IPERF_DBG_ON            0x80U
/*
 * Flag for IPERF_DEBUGF to disable that debug message.
 */
#define IPERF_DBG_OFF           0x00U

/*
 * Flag for IPERF_DEBUGF indicating a tracing message (to follow program flow).
 */
#define IPERF_DBG_TRACE         0x40U
/*
 * Flag for IPERF_DEBUGF indicating a state debug message (to follow module
 * states).
 */
#define IPERF_DBG_STATE         0x20U
/*
 * Flag for IPERF_DEBUGF indicating newly added code, not thoroughly tested yet.
 */
#define IPERF_DBG_FRESH         0x10U
/*
 * Flag for IPERF_DEBUGF to halt after printing this debug message.
 */
#define IPERF_DBG_HALT          0x08U

/* Memory allocation messages */
#define IPERF_MEMALLOC_MSG(var, size) \
    ("Allocated %lu bytes for " TOSTRING(var) " at address %p.\n", (long unsigned) size, var)
#define IPERF_MEMFREE_MSG(var) \
    ("Freeing memory for " TOSTRING(var) " at address %p.\n", var)

#if IPERF_DEBUG

/* Counter for suppressing verbose debug output. Defined in `src/main.cpp' */
#ifdef __cplusplus
extern "C"
#else
extern
#endif /* __cplusplus */
max_size_t counter;

/*
 * print debug message only if debug message type is enabled... AND is of
 * correct type AND is at least IPERF_DBG_LEVEL.
 */
#define IPERF_DEBUGF(debug, message)          \
    do {                                      \
        if (((debug) & IPERF_DBG_ON) &&       \
            ((debug) & IPERF_DBG_TYPES_ON) && \
            ((signed short) ((debug) & IPERF_DBG_MASK_LEVEL) >= IPERF_DBG_MIN_LEVEL)) { \
            debug_get_mutex();                \
            IPERF_DETAILED_MSG(message);      \
            debug_release_mutex();            \
            if ((debug) & IPERF_DBG_HALT)     \
                while (1);                    \
        }                                     \
    } while (0)

/*
 * To reduce the amount of output from this debug message, set DEBUG_DIVISOR
 * accordingly.
 */
#define IPERF_DEBUGF_COUNTER(debug, message) \
    do {                                     \
        if (counter % DEBUG_DIVISOR == 0)    \
            IPERF_DEBUGF(debug, message);    \
    } while (0)

#define IPERF_DEBUGF_COUNTER_INCREMENT()  counter++
#define IPERF_DEBUGF_COUNTER_INITIALISE() max_size_t counter = 0

#else

#define IPERF_DEBUGF(debug, message)
#define IPERF_DEBUGF_COUNTER(debug, message)
#define IPERF_DEBUGF_COUNTER_INCREMENT()
#define IPERF_DEBUGF_COUNTER_INITIALISE()

#endif /* IPERF_DEBUG */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    void debug_init(void);
    void debug_destroy(void);
    void debug_get_mutex(void);
    void debug_release_mutex(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* IPERF_DEBUG_H_ */
