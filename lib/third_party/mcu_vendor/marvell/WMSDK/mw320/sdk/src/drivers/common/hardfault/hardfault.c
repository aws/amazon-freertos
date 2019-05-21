/** @file hardfault.c
 *
 *  @brief This file provides  IRQ handlers for hardfault
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

#include <wm_os.h>
#include <wmstdio.h>
#include <critical_error.h>
#include <compat_attribute.h>
#include <dbg_log.h>

/* A function pointer to HardFault_IRQHandler_C */
typedef void (*HardFault_IRQHandler_C_ptr_t)(unsigned long *, unsigned long *);
static HardFault_IRQHandler_C_ptr_t HardFault_function_ptr;

typedef struct stframe {
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
} stframe_t;

/* Trap MSP or PSP register value and then call real fault handler */
void HardFault_IRQHandler(void)
{
	__asm volatile ("mrs r0, msp");
	__asm volatile ("mrs r1, psp");
	__asm volatile ("b HardFault_IRQHandler_C");
}

__USED__
void HardFault_IRQHandler_C(unsigned long *msp, unsigned long *psp)
{
	if (HardFault_function_ptr)
		HardFault_function_ptr(msp, psp);

	critical_error(-CRIT_ERR_HARD_FAULT, NULL);

	while (1)
		;
}

__USED__
void HardFault_IRQHandler_C_local(unsigned long *msp, unsigned long *psp)
{
	#ifndef LL_LOG
		#error "Please define LL_LOG"
	#endif

	int regval;
	stframe_t *stf;

	stf = (stframe_t *) msp;
	LL_LOG("HardFault IRQHandler: using msp @ 0x%08x",
		  (unsigned int)msp);

	LL_LOG("\r\nr0 = 0x%08x\r\nr1 = 0x%08x\r\nr2 = 0x%08x\r\n"
		  "r3 = 0x%08x\r\nr12 = 0x%08x\r\nlr = 0x%08x\r\npc = 0x%08x"
		  "\r\npsr = 0x%08x\r\n", stf->r0, stf->r1, stf->r2, stf->r3,
		  stf->r12, stf->lr, stf->pc, stf->psr);

	stf = (stframe_t *) psp;
	LL_LOG("HardFault IRQHandler: using psp @ 0x%08x",
		  (unsigned int)psp);

	LL_LOG("\r\nr0 = 0x%08x\r\nr1 = 0x%08x\r\nr2 = 0x%08x\r\n"
		  "r3 = 0x%08x\r\nr12 = 0x%08x\r\nlr = 0x%08x\r\npc = 0x%08x"
		  "\r\npsr = 0x%08x\r\n", stf->r0, stf->r1, stf->r2, stf->r3,
		  stf->r12, stf->lr, stf->pc, stf->psr);

	LL_LOG("Task name %s\r\n", get_current_taskname());

	static int stptr;
	/* read HFSR  Hard Fault Status Register */
	stptr = SCB->HFSR;
	LL_LOG("HFSR : 0x%08x\r\n", stptr);

	if (stptr & SCB_HFSR_FORCED_Msk) {

		/* read CFSR Configurable Fault Status Register */
		stptr = SCB->CFSR;

		LL_LOG("Configurable Fault Status Register"
			  "  (CFSR): 0x%08x\r\n", stptr);
		LL_LOG("  MemManage Fault Status Register"
			  " (MMSR)  : 0x%02x\r\n", (stptr & 0xff));

		/*
		 * If MMARVALID bit is set, read the address
		 * that caused the fault
		 */
		if (stptr & SCB_CFSR_MEMFAULTSR_Msk) {
			/* MemManage Fault Address Reg (MMAR) */
			regval = SCB->MMFAR;

			LL_LOG("    MMAR : 0x%08x\r\n", regval);
		}

		LL_LOG("  Bus Fault Status Register"
			  " (BFSR)        : 0x%02x\r\n", ((stptr >> 8) & 0xff));

		/*
		 * If BFARVALID bit is set, read the address
		 * that caused the fault
		 */
		if (stptr & SCB_CFSR_BUSFAULTSR_Msk) {
			/* Bus Fault Address Reg (BFAR) */
			regval = SCB->BFAR;

			LL_LOG("      BFAR      : 0x%08x\r\n", regval);
		}

		LL_LOG("  Usage Fault Status Register (UFSR)"
			  "      : 0x%04x\r\n", (stptr >> 16));
	}
}

void HardFault_IRQHandler_C_ptr_register()
{
	HardFault_function_ptr = HardFault_IRQHandler_C_local;
}
