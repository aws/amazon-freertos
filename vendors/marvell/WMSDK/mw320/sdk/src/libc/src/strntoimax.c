/*
 * Copyright 2008-2019, Marvell International Ltd.
 * All Rights Reserved.
 *
 * Derived from:
 * http://www.kernel.org/pub/linux/libs/klibc/
 */
/*
 * strntoimax.c
 *
 * strntoimax()
 */

#include <stddef.h>
#include <inttypes.h>
#include <wmlibc.h>

intmax_t strntoimax(const char *nptr, char **endptr, int base, size_t n)
{
	return (intmax_t) strntoumax(nptr, endptr, base, n);
}
