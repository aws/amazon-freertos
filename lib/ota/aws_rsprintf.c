/*
Amazon FreeRTOS OTA Agent V0.9.4
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/


#include <stdarg.h>
#include "ctype.h"
#include "aws_rsprintf.h"
#include "FreeRTOS.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef USE_EFFICIENT_PRINT_SPOOLER
#include "printTask.h"
#endif


static char* prvMemPrint(u8* ptr, u32 len, bool_t bUseIsPrint, bool_t bConsoleOut, char* acSmallBuf, char* dptr);

#ifdef USE_EFFICIENT_PRINT_SPOOLER
/* This macro will check if console output is selected and spool the data
* in the specified buffer (arg1) to the print queue and then reset the
* rvsprintf destination pointer (arg2) to the start of that buffer. The
* purpose of this is to build and spool the formatted string in small
* chunks to minimize memory usage. The result of the queue function call
* is returned in arg3.
*/
#define SPOOL_TO_PRINT_QUEUE_IF_CONSOLE( pcBuffer, dptr, xRes ) do \
{ \
	if ( bConsoleOut == pdTRUE ) \
	{ \
		xRes = PRINT_QueueChars( pcBuffer, ( uint32_t) ( dptr - pcBuffer ) ); \
		/* Reset destination pointer to start of debug buffer so \
		 * we can output small chunks at a time. */ \
		dptr = pcBuffer; \
	} \
} while (0);

/* This macro will spool the data in the specified buffer (arg1) to the print
* queue and then reset the rvsprintf destination pointer (arg2) to the start
* of that buffer. The purpose of this is to build and spool the formatted
* string in small chunks to minimize memory usage.
* The result of the queue function call is returned in arg3.
*/
#define SPOOL_TO_PRINT_QUEUE( pcBuffer, dptr, xRes ) do\
{ \
	xRes = PRINT_QueueChars( pcBuffer, ( uint32_t) ( dptr - pcBuffer ) ); \
	/* Reset destination pointer to start of debug buffer so \
	 * we can output small chunks at a time. */ \
	dptr = pcBuffer; \
} while (0);
#else
#define SPOOL_TO_PRINT_QUEUE_IF_CONSOLE(...)
#define SPOOL_TO_PRINT_QUEUE(...)
#endif

/* For spooling to the console, the rsprintf function requires a tiny buffer
* on the stack just big enough to hold the minimum sized atomic unit created
* by the print formatter. It happens to be the size of the 32 bit signed
* decimal output. */
#define kSmallPrintBufferSize kS32_MaxTextSize


char *stpcpy(register char *dest, register const char *src)
{
	register char       *d = dest;
	register const char *s = src;

	while ((*d++ = *s++));
	return d - 1;
}



/******************************************************************************
Function: szU32()

Construct an ASCII zero terminated string of an unsigned 32 bit value in
decimal.

Args: dest = pointer to destination string.
val  = 32 bit value to convert.
iFieldWidth = fixed width of string or 0 to suppress leading zero's.

Returns: pointer to the end of the string (the terminating zero).
*****************************************************************************/
char *szU32(char* dest, u32 val, u8 iFieldWidth) {

	char buf[kU32_MaxPlaces];
	char *dptr;
	u32 d, n;

	if (iFieldWidth > kU32_MaxPlaces) {

		iFieldWidth = kU32_MaxPlaces;
	}
	dptr = buf;
	for (d = 1000000000; d > 1; d /= 10) {

		n = val / d;
		*dptr++ = '0' + n;
		val -= (n * d);
	}
	*dptr++ = '0' + val;

	if (iFieldWidth == 0) {

		/* Suppress leading zeros... */
		for (d = 0; d < (kU32_MaxPlaces - 1); d++) {

			if (buf[d] != '0') break;
		}
	}
	else {

		/* Use a specific field width... */
		d = kU32_MaxPlaces - iFieldWidth;
	}
	do {

		*dest++ = buf[d];
	} while (d++ < (kU32_MaxPlaces - 1));	/* Write out up to the end of 10 digits. */
	*dest = 0;                  			/* Zero terminate the string. */
	return dest;                			/* Return pointer to the zero terminator for easy concatenation. */
}





/******************************************************************************
Function: szS32()

Construct an ASCII zero terminated string of a signed 32 bit value in
decimal. This will prepend a minus sign if the value is negative.

Args: dest = pointer to destination string.
val  = 32 bit value to convert.
iFieldWidth = fixed width of string or 0 to suppress leading zero's.

Returns: pointer to the end of the string (the terminating zero).
*****************************************************************************/
char *szS32(char* dest, s32 val, u8 iFieldWidth) {

	if (val < 0) {

		*dest++ = '-';
		val = -val;
	}
	return szU32(dest, val, iFieldWidth);
}



/******************************************************************************
Function: szH32()

Construct an ASCII zero terminated string of a 32 bit value in hex.

Args: dest = pointer to destination string.
val  = 32 bit value to convert.
iFieldWidth = fixed width of string or 0 to suppress leading zero's.

Returns: pointer to the end of the string (the terminating zero).
*****************************************************************************/
#if 0
char *szH32(char* dest, u32 val, u8 iFieldWidth) {

	char buf[kH32_MaxPlaces];
	char *dptr;
	u32 d, n;

	if (iFieldWidth > kH32_MaxPlaces) {

		iFieldWidth = kH32_MaxPlaces;
	}
	dptr = &buf[kH32_MaxPlaces - 1];
	for (d = 0; d < kH32_MaxPlaces; d++) {

		n = val & 0xf;
		if (n > 9) {

			*dptr-- = 'a' - 10 + n;
		}
		else {

			*dptr-- = '0' + n;
		}
		val >>= 4;
	}
	if (iFieldWidth == 0) {

		/* Suppress leading zeros... */
		for (d = 0; d < (kH32_MaxPlaces - 1); d++) {

			if (buf[d] != '0') break;
		}
	}
	else {

		/* Use a specific field width... */
		d = kH32_MaxPlaces - iFieldWidth;
	}
	do {

		*dest++ = buf[d];
	} while (d++ < (kH32_MaxPlaces - 1));	/* Write out up to the end of all digit places. */
	*dest = 0;                  			/* Zero terminate the string. */
	return dest;                			/* Return pointer to the zero terminator for easy concatenation. */
}
#else

char *szH32(char* dest, u32 val, u8 iFieldWidth) {

	char buf[kH32_MaxPlaces];
	char *dptr;
	u32 d, n;

	if (iFieldWidth > kH32_MaxPlaces) {

		iFieldWidth = kH32_MaxPlaces;
	}
	dptr = buf;
	for (d = 0x10000000; d > 0; d >>= 4) {

		n = val / d;
		if (n > 9) {

			*dptr++ = 'a' - 10 + n;
		}
		else {

			*dptr++ = '0' + n;
		}
		val -= (n * d);
	}

	if (iFieldWidth == 0) {

		/* Suppress leading zeros... */
		for (d = 0; d < (kH32_MaxPlaces - 1); d++) {

			if (buf[d] != '0') break;
		}
	}
	else {

		/* Use a specific field width... */
		d = kH32_MaxPlaces - iFieldWidth;
	}
	do {

		*dest++ = buf[d];
	} while (d++ < (kH32_MaxPlaces - 1));	/* Write out up to the end of 8 digits. */
	*dest = 0;                  			/* Zero terminate the string. */
	return dest;                			/* Return pointer to the zero terminator for easy concatenation. */
}
#endif



/******************************************************************************
Function: rsprintf() "reduced sprintf"

Custom limited version of sprintf that doesn't require lots of stack space
per thread and only supports the functionality we need plus a few bonus
features. There is no floating point support! This is NOT meant to be a
full replacement for sprintf(). If you really need that, then use sprintf()
but be aware of the stack space requirement. If you are also supporting
USE_EFFICIENT_PRINT_SPOOLER, this implementation will automatically output
to the console UART if the destination buffer pointer is NULL. If that
support is not included then it will simply do nothing and return with a
zero length result if the destination pointer is NULL.

Args: dest = pointer to destination buffer.
fmt  = pointer to format string similar to sprintf.

Returns: u32 = the number of characters written to the buffer not
including the terminating zero.
*****************************************************************************/
u32 rsprintf(char *dest, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);
	u32 iLen = rvsprintf(dest, fmt, args);
	va_end(args);
	return iLen;
}

/* Formatted print to the Console. Prepare the rsprintf to print to the
* console UART instead of a user destination buffer.
*/
u32 cprintf(const char *fmt, ...) {

#ifdef	USE_EFFICIENT_PRINT_SPOOLER
	va_list args;
	va_start(args, fmt);
	u32 iLen = rvsprintf(NULL, fmt, args);
	va_end(args);
	return iLen;
#else
	(void)fmt;
	return 0;
#endif
}


u32 rvsprintf(char *dest, const char *fmt, va_list va) {

	char c;
	u32 iWidth = 0;
	static const char acNullMsg[] = "{NULL}";
	char acSmallBuf[kSmallPrintBufferSize];
	bool_t bConsoleOut = pdFALSE;
	BaseType_t xResult;

	char *dptr;
#ifdef USE_EFFICIENT_PRINT_SPOOLER
	if (dest == NULL) {

		bConsoleOut = pdTRUE;
		dptr = acSmallBuf;
	}
#else
	if (dest == NULL)
	{
		return 0;
	}
#endif
	else {
		dptr = dest;
	}

	while ((c = *fmt++) != 0) {

		switch (c) {

		case '%':
		{
			c = *fmt++;
			/* Ignore unsupported options. */
			if ((c == 'l') || (c == 'L') || (c == '.'))
			{
				c = *fmt++;
			}
			if (c == '*')
			{
				/* Pull the field width parameter from the stack. */
				iWidth = va_arg(va, u32);
				c = *fmt++;
			}
			else {
				/* Check for field width. */
				if ((c >= '0') && (c <= '9')) {

					iWidth = strtoul(fmt - 1, (char**)&fmt, 10);
					c = *fmt++;
				}
				else {
					iWidth = 0;		/* Default to suppress leading zero's of numeric values. */
				}
			}
			switch (c)
			{
				/* Ignore next parameter. */
			case '~':
			{
				va_arg(va, u32);
				break;
			}
			/* Unsigned 32 bit decimal output. */
			case 'd':
			case 'D':
			{
				s32 v = va_arg(va, s32);
				dptr = szS32(dptr, v, iWidth);
				break;
			}
			case 'u':
			case 'U':
			{
				u32 v = va_arg(va, u32);
				dptr = szU32(dptr, v, iWidth);
				break;
			}

			/* 32 bit hexadecimal output. */
			case 'x':
			case 'X':
			{
				u32 v = va_arg(va, u32);
				dptr = szH32(dptr, v, iWidth);
				break;
			}
#ifdef SUPPORT_HEX_CHAR_DEBUG_PRINT
			case 'C':
			{
				char c2 = va_arg(va, int);
				if (isprint((int)c2) == 0)
				{
					*dptr++ = '[';
					dptr = szH32(dptr, c2, 2);
					*dptr++ = ']';
				}
				else
				{
					*dptr++ = c2;
				}
				break;
			}
#else
			case 'C':
#endif
			case 'c':
			{
				char c2 = va_arg(va, int);
				*dptr++ = c2;
				break;
			}
			/* String insertion (nested format specifiers are not handled). */
			case 's':
			{
				char *p = va_arg(va, char*);
				if (p != NULL)
				{
					/* If we already have the field width, print with width limit. */
					if (iWidth != 0)
					{
						dptr = prvMemPrint((u8*)p, iWidth, pdFALSE, bConsoleOut, acSmallBuf, dptr);
						break;
					}
#ifdef USE_EFFICIENT_PRINT_SPOOLER
					/* We don't know the width so print to zero terminator. */
					else if (bConsoleOut == pdTRUE)
					{
						char* eptr;
						do
						{
							/* Suppress warning of memccpy name change in Windows since the
							   embedded targets only support the original name. */
							#pragma warning(suppress : 4996)
							eptr = (char*) memccpy(dptr, p, '\0', kSmallPrintBufferSize);
							/* eptr either points to the 0 or NULL if 0 wasn't found within the range limit.
							* If 0 wasn't found, we want to spool the entire small buffer so we can free it up.
							*/
							if (eptr == NULL)
							{
								p += kSmallPrintBufferSize;
								/* Set the destination pointer to the end of the buffer for spooling. */
								dptr = acSmallBuf + kSmallPrintBufferSize;
							}
							else
							{
								dptr = eptr;
							}
							SPOOL_TO_PRINT_QUEUE(acSmallBuf, dptr, xResult);
						} while ((xResult == pdPASS) && (eptr == NULL));
					}
#endif
					else
					{
						dptr = stpcpy(dptr, p);
					}
				}
				else {

					dptr = stpcpy(dptr, acNullMsg);
				}
				break;
			}
			/* Special hexdump. Takes length followed by pointer to data. */
			case 'H':
			{
				iWidth = va_arg(va, u32);
				u8 *p = va_arg(va, u8*);
				xResult = pdPASS;
				while ((xResult == pdPASS) && iWidth--) {

					dptr = szH32(dptr, *p++, 2);
					SPOOL_TO_PRINT_QUEUE_IF_CONSOLE(acSmallBuf, dptr, xResult);
				}
				break;
			}
			/* Mem dump with non-printable character substitutions. */
			case '^':
			{
				iWidth = va_arg(va, u32);
				u8 *p = va_arg(va, u8*);
				dptr = prvMemPrint(p, iWidth, pdTRUE, bConsoleOut, acSmallBuf, dptr);
				break;
			}
			default:
			{
				*dptr++ = c;
				break;
			}
			}
			break;
		}
		default:
		{
			*dptr++ = c;
			break;
		}
		}
		SPOOL_TO_PRINT_QUEUE_IF_CONSOLE(acSmallBuf, dptr, xResult);
	}
	*dptr = 0;                                  /* Zero terminate the string. */
	return (u32)(dptr - dest);           		/* Return number of characters written. */
}


static char* prvMemPrint(u8* ptr, u32 len, bool_t bUseIsPrint, bool_t bConsoleOut, char* acSmallBuf, char* dptr)
{
#ifndef USE_EFFICIENT_PRINT_SPOOLER
	(void) acSmallBuf;
#else
	BaseType_t xResult;
#endif

	u32 lim = kSmallPrintBufferSize;
	while (len--)
	{
		u8 c3 = *ptr++;
		if ((bUseIsPrint == pdTRUE) && isprint((int)c3) == 0)
		{
			c3 = '.';
		}
		*dptr++ = c3;
		if (bConsoleOut == pdTRUE)
		{
			if (--lim == 0)
			{
				SPOOL_TO_PRINT_QUEUE(acSmallBuf, dptr, xResult);
				lim = kSmallPrintBufferSize;
			}
		}
	}
	return dptr;
}

