/*
 * Copyright 2008-2017, Marvell International Ltd.
 * All Rights Reserved.
 *
 * Derived from:
 * http://www.kernel.org/pub/linux/libs/klibc/
 */
/*
 * strcspn
 */

#include <string.h>

#include "strxspn.h"

size_t strcspn(const char *s, const char *reject)
{
	return __strxspn(s, reject, 1);
}
