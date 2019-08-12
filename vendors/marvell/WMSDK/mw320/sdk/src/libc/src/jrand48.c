/*
 * Copyright 2008-2019, Marvell International Ltd.
 * All Rights Reserved.
 *
 * Derived from:
 * http://www.kernel.org/pub/linux/libs/klibc/
 */
/*
 * jrand48.c
 */

#include <stdlib.h>
#include <stdint.h>

long jrand48(unsigned short xsubi[3])
{
	uint64_t x;

	/* The xsubi[] array is littleendian by spec */
	x = (uint64_t) (uint16_t) xsubi[0] +
	    ((uint64_t) (uint16_t) xsubi[1] << 16) +
	    ((uint64_t) (uint16_t) xsubi[2] << 32);

	x = (0x5deece66dULL * x) + 0xb;

	xsubi[0] = (unsigned short)(uint16_t) x;
	xsubi[1] = (unsigned short)(uint16_t) (x >> 16);
	xsubi[2] = (unsigned short)(uint16_t) (x >> 32);

	return (long)(int32_t) (x >> 16);
}
