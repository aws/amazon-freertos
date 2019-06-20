
#ifndef __SYSLOG_H__
#define __SYSLOG_H__

#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"

#include "portmacro.h"

#include "aws_logging_task.h"

#ifndef NO_LOG

#include "FreeRTOSIPConfig.h"

#define _HEXDUMP(_message, ptr, len)                    \
do {                                                    \
    int  __i;                                           \
    char str[50];                                       \
    vLoggingPrintf("%s (%d bytes)\n", _message, len);   \
    for (__i = 0; __i < len; __i++)                     \
    {                                                   \
        char __c = ((char *)ptr)[ __i ];                \
        int __j = __i & 15;                             \
        sprintf(str + __j * 3, "%02x ", __c);           \
        if (((__i & 15) == 15) || (__i == len - 1))     \
        {                                               \
            str[ __j * 3 + 2 ] = '\n';                  \
            str[ __j * 3 + 3 ] = '\0';                  \
            vLoggingPrintf(str);                        \
        }                                               \
    }                                                   \
} while (0)

#define LOG_INFO    1
#define LOG_WARN    2
#define LOG_ERRR    4

#define CURRENT_LEVEL   (LOG_ERRR)
#define CHECK_LEVEL(L)  (CURRENT_LEVEL & (L))
#define ONCE(s)         do { s; } while (0)

#define LOG_E(_module, _msg...)         ONCE(if (CHECK_LEVEL(LOG_ERRR)) vLoggingPrintf(_msg))

#define LOG_I(_module, _msg...)         ONCE(if (CHECK_LEVEL(LOG_INFO)) vLoggingPrintf(_msg))

#define LOG_W(_module, _msg...)         ONCE(if (CHECK_LEVEL(LOG_WARN)) vLoggingPrintf(_msg))

#define LOG_HEXDUMP_E(_m, _msg, _p, _l) ONCE(if (CHECK_LEVEL(LOG_ERRR)) _HEXDUMP(_msg, _p, _l))

#define LOG_HEXDUMP_I(_m, _msg, _p, _l) ONCE(if (CHECK_LEVEL(LOG_INFO)) _HEXDUMP(_msg, _p, _l))

#define LOG_HEXDUMP_W(_m, _msg, _p, _l) ONCE(if (CHECK_LEVEL(LOG_WARN)) _HEXDUMP(_msg, _p, _l))

#else

#define DO_NOTHING  do { } while (0)

#define LOG_E(_module, _message...)        DO_NOTHING

#define LOG_I(_module, _message...)        DO_NOTHING

#define LOG_W(_module, _message...)        DO_NOTHING

#define LOG_HEXDUMP_E(_module, _message, ptr, len) DO_NOTHING

#define LOG_HEXDUMP_I(_module, _message, ptr, len) DO_NOTHING

#define LOG_HEXDUMP_W(_module, _message, ptr, len) DO_NOTHING

#endif

#endif /* __SYSLOG_H__ */
