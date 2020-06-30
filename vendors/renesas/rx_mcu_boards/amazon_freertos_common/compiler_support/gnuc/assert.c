#if 0 /* This file will be removed later because assert() macro in the tinycbor
was replaced by modifying assert_p.h and cbor.h to call vAssertCalled() when
GNURX's __RX__ is defined. */
#include <assert.h>
#include "FreeRTOS.h"

/* Messages by __assert_func() and __assert() are output to stderr
 * but this functionality is not implemented. */

void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
    (void)file;
    (void)line;
    (void)func;
    (void)failedexpr;

    vAssertCalled();

    for(;;) {}
}

void __assert(const char *file, int line, const char *failedexpr)
{
    (void)file;
    (void)line;
    (void)failedexpr;

    vAssertCalled();

    for(;;) {}
}
#endif
