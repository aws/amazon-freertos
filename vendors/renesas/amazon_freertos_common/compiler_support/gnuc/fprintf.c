#if 0 /* This file will be removed later because fprintf() in the tinycbor
was removed by modifying cbor.h and cborpretty.c to check GNURX's __RX__. */
#include <stdio.h>

/* Messages by fprintf() are output to stdout, stderr, file, etc
 * but these functionalities are not implemented. */

int fprintf(FILE *__restrict fp, const char *__restrict fmt, ...)
{
    (void)fp;
    (void)fmt;

    return EOF;
}
#endif
