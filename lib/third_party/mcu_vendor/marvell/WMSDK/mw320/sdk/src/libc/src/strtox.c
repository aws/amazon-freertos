/*
 * Copyright 2008-2017, Marvell International Ltd.
 * All Rights Reserved.
 *
 * Derived from:
 * http://www.kernel.org/pub/linux/libs/klibc/
 */
/*
 * strtox.c
 *
 * strto...() functions, by macro definition
 */

#include <stddef.h>
#include <stdlib.h>
#include <inttypes.h>
#include <wmlibc.h>
#include <wm_utils.h>

WEAK TYPE NAME(const char *nptr, char **endptr, int base)
{
	return (TYPE) strntoumax(nptr, endptr, base, ~(size_t) 0);
}
