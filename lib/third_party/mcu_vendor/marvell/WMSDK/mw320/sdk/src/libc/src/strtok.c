/*
 * Copyright 2008-2017, Marvell International Ltd.
 * All Rights Reserved.
 *
 * Derived from:
 * http://www.kernel.org/pub/linux/libs/klibc/
 */
/*
 * strtok.c
 */

#include <string.h>

char *strtok(char *s, const char *delim)
{
	static char *holder;

	return strtok_r(s, delim, &holder);
}
