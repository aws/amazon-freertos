/*
 * Copyright 2008-2017, Marvell International Ltd.
 * All Rights Reserved.
 *
 * Derived from:
 * http://www.kernel.org/pub/linux/libs/klibc/
 */
/*
 * strspn
 */

#include <string.h>

#include "strxspn.h"

size_t strspn(const char *s, const char *accept)
{
	return __strxspn(s, accept, 0);
}
