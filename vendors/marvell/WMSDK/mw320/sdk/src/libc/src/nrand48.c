/*
 * Copyright 2008-2017, Marvell International Ltd.
 * All Rights Reserved.
 *
 * Derived from:
 * http://www.kernel.org/pub/linux/libs/klibc/
 */
/*
 * nrand48.c
 */

#include <stdlib.h>
#include <stdint.h>

extern long jrand48(unsigned short *);

long nrand48(unsigned short xsubi[3])
{
	return (long)((uint32_t) jrand48(xsubi) >> 1);
}
