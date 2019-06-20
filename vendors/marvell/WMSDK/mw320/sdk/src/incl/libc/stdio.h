/*
 * stdio.h
 */

#ifndef _STDIO_H
#define _STDIO_H

#include <extern.h>
#include <stdarg.h>
#include <stddef.h>

/* The actual IO functions are not included. */
/* For float values, supported set of flags are : -, +, 0, (space), 0.
 * In addition to this, width specifier and precision upto 6 digits after
 * decimal point is supported. By default, precision for float values is 6.
 * Below are some of the example formats:
 * - For printing float with preceding blanks = %15f
 * - For printing float with preceding zeros = %015f
 * - For printing float with sign = %+15f
 * - For printing float with left justification = %-15f
 * - For printing float with decimal point precision of 3 = %015.3f
 * - For printing float with zero decimal point precision = %015.0f
 */

__extern int sprintf(char *, const char *, ...);
__extern int vsprintf(char *, const char *, va_list);
__extern int snprintf(char *, size_t n, const char *, ...);
__extern int vsnprintf(char *, size_t n, const char *, va_list);
__extern int sscanf(const char *, const char *, ...);
__extern int vsscanf(const char *, const char *, va_list);

#endif				/* _STDIO_H */
