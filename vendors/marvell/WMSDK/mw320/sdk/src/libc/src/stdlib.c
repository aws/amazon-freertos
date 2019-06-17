/*
 * Copyright 2008-2017, Marvell International Ltd.
 * All Rights Reserved.
 *
 *  Derived from:
 *  http://www.kernel.org/pub/linux/libs/klibc/
 *
 * This functions implement WMSDK specific memory allocator routines.
 */

#include <wm_os.h>
#include <sys/types.h>
#include <stdlib.h>

void *malloc(size_t size)
{
	return os_mem_alloc(size);
}

void free(void *ptr)
{
	os_mem_free(ptr);
}

void *calloc(size_t nmemb, size_t size)
{
	return os_mem_calloc(nmemb * size);
}

void *realloc(void *ptr, size_t size)
{
	return os_mem_realloc(ptr, size);
}
