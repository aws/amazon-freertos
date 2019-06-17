/*
 * Copyright 2008-2017, Marvell International Ltd.
 * All Rights Reserved.
 *
 * Derived from:
 * http://www.kernel.org/pub/linux/libs/klibc/
 */
/*
 * memccpy.c
 *
 * memccpy()
 */

#include <stddef.h>
#include <string.h>

void *memccpy(void *dst, const void *src, int c, size_t n)
{
	char *q = dst;
	const char *p = src;
	char ch;

	while (n--) {
		*q++ = ch = *p++;
		if (ch == (char)c)
			return q;
	}

	return NULL;		/* No instance of "c" found */
}
