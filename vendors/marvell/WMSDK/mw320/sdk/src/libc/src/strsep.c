/*
 * Copyright 2008-2017, Marvell International Ltd.
 * All Rights Reserved.
 *
 * Derived from:
 * http://www.kernel.org/pub/linux/libs/klibc/
 */
/*
 * strsep.c
 */

#include <string.h>

char *strsep(char **stringp, const char *delim)
{
	char *s = *stringp;
	char *e;

	if (!s)
		return NULL;

	e = strpbrk(s, delim);
	if (e)
		*e++ = '\0';

	*stringp = e;
	return s;
}
