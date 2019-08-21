/*
 * Copyright 2008-2017, Marvell International Ltd.
 * All Rights Reserved.
 *
 * Derived from:
 * http://www.kernel.org/pub/linux/libs/klibc/
 */
#include <wm_os.h>
#include <wmstdio.h>

void __assert_func(const char *file, int line,
		   const char *func, const char *failedexpr)
{
	wmprintf("assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
		 failedexpr, file, line, func ? ", function: " : "",
		 func ? func : "");

	/* Ensure that nothing runs after this */
	os_enter_critical_section();
	while (1)
		;
}

float strtof(const char *nptr, char **endptr)
{
	wmprintf("Float print not supported yet!\r\n");
	return 0;
}
