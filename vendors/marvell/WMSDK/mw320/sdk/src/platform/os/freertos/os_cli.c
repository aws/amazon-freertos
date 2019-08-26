/** @file os_cli.c
*
*  @brief OS CLI Interface
*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
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

#include <inttypes.h>
#include <wmstdio.h>
#include <wm_os.h>
#include <boot_flags.h>

/* Freertos does no size check on this buffer and hence kept higher than
 * minimal size that would be required */
#define MAX_TASK_INFO_BUF 2048

void os_dump_threadinfo(char *name)
{
	uint8_t status;
	uint32_t stack_remaining, tcbnumber, priority, stack_start,
		stack_top;
	char tname[configMAX_TASK_NAME_LEN], *substr;
	char *task_info_buf = NULL;
	task_info_buf = (char *) os_mem_alloc(MAX_TASK_INFO_BUF);
	if (task_info_buf == NULL)
		return;
	vTaskList(task_info_buf);
	substr = strtok(task_info_buf, "\r\n");
	while (substr) {
		char *tabtab = strstr(substr, "\t\t");
		if (!tabtab) {
			wmprintf("Task list info corrupted."
					"Cannot parse\n\r");
			break;
		}
		/* NULL terminate the task name */
		*tabtab = 0;
		strncpy(tname, substr, sizeof(tname));
		/* advance to thread status */
		substr = tabtab + 2;
		sscanf((const char *)substr,
		       "%c\t%u\t%u\t%u\t%x\t%x\r\n", &status,
		       (unsigned int *)&priority,
		       (unsigned int *)&stack_remaining,
		       (unsigned int *)&tcbnumber,
		       (unsigned int *)&stack_start,
		       (unsigned int *)&stack_top);
		if (strncmp(tname, name, strlen(name)) == 0 || name == NULL) {
			wmprintf("%s:\r\n", tname);
			const char *status_str;
			switch (status) {
			case 'R':
				status_str = "ready";
				break;
			case 'B':
				status_str = "blocked";
				break;
			case 'S':
				status_str = "suspended";
				break;
			case 'D':
				status_str = "deleted";
				break;
			default:
				status_str = "unknown";
				break;
			}
			wmprintf("\tstate = %s\r\n", status_str);
			wmprintf("\tpriority = %u\r\n", priority);
			wmprintf("\tstack remaining = %d bytes\r\n",
				 stack_remaining * sizeof(portSTACK_TYPE));
			wmprintf("\ttcbnumber = %d\r\n", tcbnumber);
			wmprintf("\tstack_start: 0x%x stack_top: 0x%x\r\n",
				 stack_start, stack_top);
			if (name != NULL && task_info_buf) {
				os_mem_free(task_info_buf);
				return;
			}
		}

		substr = strtok(NULL, "\r\n");
	}

	if (name)
                /* If we got this far and didn't find the thread, say so. */
		wmprintf("ERROR: No thread named '%s'\r\n", name);
	if (task_info_buf)
		os_mem_free(task_info_buf);
}

void os_dump_system_conf()
{
	wmprintf("SDK version: %s\r\n", SDK_VERSION);
	wmprintf("Compiler version: %s\r\n", __VERSION__);
	wmprintf("CPU clock frequency: %uHz\r\n", CLK_GetSystemClk());

	uint32_t program_cnt = 0;
	__asm volatile ("mov %0, pc" : "=r" (program_cnt));
	wmprintf("Code execution mode: %s\r\n",
		(program_cnt & 0x1f000000) == 0x1f000000
					? "XIP" : "Non-XIP" );

	if (g_boot_flags & BOOT_SECURE_FW_ENABLE) {
		char *type_str = "Non-secure";
		if ((g_boot_flags & BOOT_SECURE_SIGN_FW) &&
				(g_boot_flags & BOOT_SECURE_ENCRYPT_FW))
			type_str = "Signed + Encrypted";
		else if (g_boot_flags & BOOT_SECURE_SIGN_FW)
			type_str = "Signed Only";
		else if (g_boot_flags & BOOT_SECURE_ENCRYPT_FW)
			type_str = "Encrypt Only";
		wmprintf("Firmware is: %s\r\n", type_str);
	}
}
