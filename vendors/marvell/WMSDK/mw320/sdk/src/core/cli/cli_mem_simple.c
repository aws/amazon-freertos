/** @file cli_mem_simple.c
 *
 *  @brief This file provides  Simple memory allocation for cli
 *
 *  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved.
 *
 *  MARVELL CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Material") are owned by Marvell International Ltd or its
 *  suppliers or licensors. Title to the Material remains with Marvell
 *  International Ltd or its suppliers and licensors. The Material contains
 *  trade secrets and proprietary and confidential information of Marvell or its
 *  suppliers and licensors. The Material is protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Material may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without Marvell's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by Marvell in writing.
 *
 */


/* cli_mem_simple.c: Simple memory allocation for cli
 *
 */
#include <stdio.h>
#include <string.h>

#include <cli.h>
#include <wmerrno.h>
#include <cli_utils.h>

#include "cli_mem.h"
/* Simple memory allocator for Operating Systems that do not support dynamic
 * allocation. The size of the allocation is hard-coded to the need of the cli
 * module.
 *
 * If required this can be blown into a better slab-kind of allocator.
 */

char buff1[INBUF_SIZE];
char buff2[INBUF_SIZE];
char alloc[2] = { BUF_AVAILABLE, BUF_AVAILABLE};

int cli_mem_cleanup(void)
{

	return WM_SUCCESS;
}

int cli_mem_init(void)
{

	return WM_SUCCESS;
}

void *cli_mem_malloc(int size)
{
	/* Only supports fixed size = INBUF_SIZE */
	if (size != INBUF_SIZE)
		return NULL;

	if (alloc[0] == BUF_AVAILABLE) {
		alloc[0] = BUF_ALLOCATED;
		return buff1;
	} else if (alloc[1] == BUF_AVAILABLE) {
		alloc[1] = BUF_ALLOCATED;
		return buff2;
	} else {
		return NULL;
	}
}

int cli_mem_free(char **buffer)
{
	if (*buffer == buff1)
		alloc[0] = BUF_AVAILABLE;
	else if (*buffer == buff2)
		alloc[1] = BUF_AVAILABLE;

	*buffer = NULL;
	return WM_SUCCESS;
}
