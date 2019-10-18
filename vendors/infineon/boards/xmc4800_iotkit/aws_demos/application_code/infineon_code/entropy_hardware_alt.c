/*
 * Copyright (C) 2018 Infineon Technologies AG. All rights reserved.
 *
 * Infineon Technologies AG (Infineon) is supplying this software for use with
 * Infineon's microcontrollers.
 * This file can be freely distributed within development tools that are
 * supporting such microcontrollers.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */

#include <stdlib.h>

#include "xmc_common.h"
#include "mbedtls/entropy_poll.h"

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
  (void)data;

  for (int32_t i = 0; i < len / 4; i++, output+=4)
  {
	*(uint32_t *)output = lrand48();
  }

  if ((len % 4) != 0)
  {
  	uint32_t last = lrand48();
  	for (int32_t i = 0; i < len % 4; i++, output += 1) {
  		*output = (unsigned char)(last & 0xff);
  		last = last >> 8;
  	}
  }

  *olen = len;

  return 0;
}


