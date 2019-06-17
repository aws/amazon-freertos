/** @file mdev_startup.c
 *
 *  @brief This file provides  mdev driver for startup
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

#include <lowlevel_drivers.h>
#include <core_cm4.h>
#include <wm_os.h>
#include <wmstdio.h>
#include <mdev_pm.h>
#include <boot_flags.h>
#include <critical_error.h>
#include <compat_attribute.h>

#ifdef CONFIG_ENABLE_ROM_LIBS
#endif

#define FREQ_32M 32000000
static CLK_Src_Type sfll_ref_clk = CLK_XTAL_REF;

#ifdef CONFIG_ENABLE_CPP_SUPPORT
typedef void (*funcp_t)(void);
typedef funcp_t *funcpp_t;

/**
 * @brief   Constructors table start.
 * @pre     The symbol must be aligned to a 32 bits boundary.
 */
extern funcp_t __init_array_start;

/**
 * @brief   Constructors table end.
 * @pre     The symbol must be aligned to a 32 bits boundary.
 */
extern funcp_t __init_array_end;

/**
 * @brief   Destructors table start.
 * @pre     The symbol must be aligned to a 32 bits boundary.
 */
extern funcp_t __fini_array_start;

/**
 * @brief   Destructors table end.
 * @pre     The symbol must be aligned to a 32 bits boundary.
 */
extern funcp_t __fini_array_end;
#endif

__SECTION__(.nvram_uninit)
extern uint32_t pm4_sig;

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

/* Default Fault Handlers. */
__NAKED__ void Default_IRQHandler(void);
__NAKED__ void Reset_IRQHandler(void);
__NAKED__ void HardFault_IRQHandler(void);

void WEAK NMI_IRQHandler(void);
void WEAK MemManage_IRQHandler(void);
void WEAK BusFault_IRQHandler(void);
void WEAK UsageFault_IRQHandler(void);
void WEAK SVC_IRQHandler(void);
void WEAK DebugMonitor_IRQHandler(void);
void WEAK PendSV_IRQHandler(void);
void WEAK SysTick_IRQHandler(void);
void WEAK ExtPin0_IRQHandler(void);
void WEAK ExtPin1_IRQHandler(void);
void WEAK RTC_IRQHandler(void);
void WEAK CRC_IRQHandler(void);
void WEAK AES_IRQHandler(void);
void WEAK I2C0_IRQHandler(void);
void WEAK I2C1_IRQHandler(void);
void WEAK DMA_IRQHandler(void);
void WEAK GPIO_IRQHandler(void);
void WEAK SSP0_IRQHandler(void);
void WEAK SSP1_IRQHandler(void);
void WEAK SSP2_IRQHandler(void);
void WEAK QSPI_IRQHandler(void);
void WEAK GPT0_IRQHandler(void);
void WEAK GPT1_IRQHandler(void);
void WEAK GPT2_IRQHandler(void);
void WEAK GPT3_IRQHandler(void);
void WEAK UART0_IRQHandler(void);
void WEAK UART1_IRQHandler(void);
void WEAK UART2_IRQHandler(void);
void WEAK WDT_IRQHandler(void);
void WEAK ADC0_IRQHandler(void);
void WEAK DAC_IRQHandler(void);
void WEAK ACOMPWKUP_IRQHandler(void);
void WEAK ACOMP_IRQHandler(void);
void WEAK SDIO_IRQHandler(void);
void WEAK USB_IRQHandler(void);
void WEAK PLL_IRQHandler(void);
void WEAK RC32M_IRQHandler(void);
void WEAK ExtPin3_IRQHandler(void);
void WEAK ExtPin4_IRQHandler(void);
void WEAK ExtPin5_IRQHandler(void);
void WEAK ExtPin6_IRQHandler(void);
void WEAK ExtPin7_IRQHandler(void);
void WEAK ExtPin8_IRQHandler(void);
void WEAK ExtPin9_IRQHandler(void);
void WEAK ExtPin10_IRQHandler(void);
void WEAK ExtPin11_IRQHandler(void);
void WEAK ExtPin12_IRQHandler(void);
void WEAK ExtPin13_IRQHandler(void);
void WEAK ExtPin14_IRQHandler(void);
void WEAK ExtPin15_IRQHandler(void);
void WEAK ExtPin16_IRQHandler(void);
void WEAK ExtPin17_IRQHandler(void);
void WEAK ExtPin18_IRQHandler(void);
void WEAK ExtPin19_IRQHandler(void);
void WEAK ExtPin20_IRQHandler(void);
void WEAK ExtPin21_IRQHandler(void);
void WEAK ExtPin22_IRQHandler(void);
void WEAK ExtPin23_IRQHandler(void);
void WEAK ExtPin24_IRQHandler(void);
void WEAK ExtPin25_IRQHandler(void);
void WEAK ExtPin26_IRQHandler(void);
void WEAK ExtPin27_IRQHandler(void);
void WEAK ULPCOMP_IRQHandler(void);
void WEAK BRNDET_IRQHandler(void);
void WEAK WIFIWKUP_IRQHandler(void);

/* System main stack pointer address */
extern unsigned long _estack;

/*
 * The minimal vector table for a Cortex-M3.  Note that the proper constructs
 * must be placed on this to ensure that it ends up at physical address
 * 0x0000.0000.
 */
__USED__ __SECTION__(.isr_vector)
void (*const __vector_table[]) (void) = {
	/* Cortex-M3 common exception */
	(void (*)(void))(&_estack),
	Reset_IRQHandler,
	NMI_IRQHandler,
	HardFault_IRQHandler,
	MemManage_IRQHandler,
	BusFault_IRQHandler,
	UsageFault_IRQHandler,
	0,
	0,
	0,
	0,
	SVC_IRQHandler,
	DebugMonitor_IRQHandler,
	0,
	PendSV_IRQHandler,
	SysTick_IRQHandler,
	/* Marvell specific interrupts */
	ExtPin0_IRQHandler,
	ExtPin1_IRQHandler,
	RTC_IRQHandler,
	CRC_IRQHandler,
	AES_IRQHandler,
	I2C0_IRQHandler,
	I2C1_IRQHandler,
	Default_IRQHandler,
	DMA_IRQHandler,
	GPIO_IRQHandler,
	SSP0_IRQHandler,
	SSP1_IRQHandler,
	SSP2_IRQHandler,
	QSPI_IRQHandler,
	GPT0_IRQHandler,
	GPT1_IRQHandler,
	GPT2_IRQHandler,
	GPT3_IRQHandler,
	UART0_IRQHandler,
	UART1_IRQHandler,
	UART2_IRQHandler,
	Default_IRQHandler,
	WDT_IRQHandler,
	Default_IRQHandler,
	ADC0_IRQHandler,
	DAC_IRQHandler,
	ACOMPWKUP_IRQHandler,
	ACOMP_IRQHandler,
	SDIO_IRQHandler,
	USB_IRQHandler,
	Default_IRQHandler,
	PLL_IRQHandler,
	Default_IRQHandler,
	RC32M_IRQHandler,
	ExtPin3_IRQHandler,
	ExtPin4_IRQHandler,
	ExtPin5_IRQHandler,
	ExtPin6_IRQHandler,
	ExtPin7_IRQHandler,
	ExtPin8_IRQHandler,
	ExtPin9_IRQHandler,
	ExtPin10_IRQHandler,
	ExtPin11_IRQHandler,
	ExtPin12_IRQHandler,
	ExtPin13_IRQHandler,
	ExtPin14_IRQHandler,
	ExtPin15_IRQHandler,
	ExtPin16_IRQHandler,
	ExtPin17_IRQHandler,
	ExtPin18_IRQHandler,
	ExtPin19_IRQHandler,
	ExtPin20_IRQHandler,
	ExtPin21_IRQHandler,
	ExtPin22_IRQHandler,
	ExtPin23_IRQHandler,
	ExtPin24_IRQHandler,
	ExtPin25_IRQHandler,
	ExtPin26_IRQHandler,
	ExtPin27_IRQHandler,
	Default_IRQHandler,
	ULPCOMP_IRQHandler,
	BRNDET_IRQHandler,
	WIFIWKUP_IRQHandler,};


/*
 * The following are constructs created by the linker, indicating where the
 * ".bss" and ".nvram" segments reside in memory.
 */

#ifdef CONFIG_ENABLE_MCU_PM3
extern unsigned long _pm3_bss;
extern unsigned long _epm3_bss;
#endif /*  CONFIG_ENABLE_MCU_PM3 */

#ifndef __ICCARM__
extern unsigned long _bss_sram0;
extern unsigned long _ebss_sram0;

extern unsigned long _bss_sram1;
extern unsigned long _ebss_sram1;

extern unsigned long _nvram_begin;
extern unsigned long _nvram_end;
#endif /* !__ICCARM__ */

#ifdef CONFIG_ENABLE_ROM_LIBS
extern unsigned long _rom_data_start;
extern unsigned long _rom_data;
static void rom_init()
{
	memset(&_rom_data_start, 0, (unsigned) &_rom_data);
	CyaSSL_SetAllocators((CyaSSL_Malloc_cb)((uint32_t)pvPortMalloc | 0x01),
		(CyaSSL_Free_cb)((uint32_t)vPortFree | 0x01),
		(CyaSSL_Realloc_cb)((uint32_t)pvPortReAlloc | 0x01));
}

#endif

/*
 * This is the code that gets called when the processor first starts execution
 * following a reset event. Only the absolutely necessary set is performed,
 * after which the os_init() routine is called, to start kernel. os_init()
 * after starting scheduler, hands over control to application specific entry
 * point, main() or user_app_init() as defined.
 */
void Reset_IRQHandler(void)
{
	__asm volatile ("mov sp, %0" : : "r" (&_estack));
	__asm volatile ("b Reset_IRQHandler_C");
}

/*
 * PLL Configuration Routine
 *
 * Fout=Fvco/P=Refclk/M*2*N /P
 * where Fout is the output frequency of CLKOUT, Fvco is the frequency of the
 * VCO, M is reference divider ratio, N is feedback divider ratio, P is post
 * divider ratio.
 * Given the CLKOUT should be programmed to Fout, it should follow these
 * steps in sequence:
 * A) Select proper M to get Refclk/M = 400K (+/-20%)
 * B) Find proper P to make P*Fout in the range of 150MHz ~ 300MHz
 * C) Find out the N by Round(P*Fout/(Refclk/M*2))
 */
static void CLK_Config_Pll(int ref_clk, CLK_Src_Type type)
{
	int i, refDiv;
	int fout = CHIP_SFLL_FREQ();
	CLK_SfllConfig_Type sfllConfigSet;

	while (CLK_GetClkStatus(CLK_SFLL) == SET)
		;
	sfllConfigSet.refClockSrc = type;

	refDiv = (int) (ref_clk / 400000);

	/* Check for (P*fout) within 150MHz to 300MHz range */
	for (i = 1; i <= 8; i <<= 1)
		if (((fout * i) >= 150000000) &&
				((fout * i) <= 300000000))
			break;

	/* Configure the SFLL */
	sfllConfigSet.refDiv = refDiv;
	sfllConfigSet.fbDiv = (int)((double) (i * fout) /
				 (((double) ref_clk / (double) refDiv) * 2));

	sfllConfigSet.kvco = 1;

	/* Post divider ratio, 2-bit
	 * 2'b00, Fout = Fvco/1
	 * 2'b01, Fout = Fvco/2
	 * 2'b10, Fout = Fvco/4
	 * 2'b11, Fout = Fvco/8
	 */
	sfllConfigSet.postDiv = ffs(i) - 1;
	CLK_SfllEnable(&sfllConfigSet);
	while (CLK_GetClkStatus(CLK_SFLL) == RESET)
		;
}

static void Setup_RC32M()
{
	PMU_PowerOnWLAN();
	CLK_RefClkEnable(CLK_XTAL_REF);
	while (CLK_GetClkStatus(CLK_XTAL_REF) == RESET)
		;
	/* Configure the SFLL */
	CLK_SfllConfig_Type sfllCfgSet;

	/* 38.4M Main Crystal -> 192M Fvco */
	sfllCfgSet.refClockSrc = CLK_XTAL_REF;
	sfllCfgSet.refDiv = 0x60;
	sfllCfgSet.fbDiv = 0xF0;
	sfllCfgSet.kvco = 1;
	sfllCfgSet.postDiv = 0x00;

	CLK_SfllEnable(&sfllCfgSet);

	while (CLK_GetClkStatus(CLK_SFLL) == RESET)
		;

	 /* Set clock divider for IPs */
	CLK_ModuleClkDivider(CLK_APB0, 2);
	CLK_ModuleClkDivider(CLK_APB1, 2);
	CLK_ModuleClkDivider(CLK_PMU, 4);

	/* Switch system clock source to SFLL
	* before RC32M calibration */
	CLK_SystemClkSrc(CLK_SFLL);

	/* Enable RC32M_GATE functional clock
	* for calibration use
	*/
	PMU->PERI3_CTRL.BF.RC32M_GATE = 0;

	CLK_RC32MCalibration(CLK_AUTO_CAL, 0);

	while ((RC32M->STATUS.BF.CAL_DONE == 0) ||
		(RC32M->STATUS.BF.CLK_RDY == 0))
		;

	/* Disable RC32M_GATE functional clock
	* on calibrating RC32M
	*/
	PMU->PERI3_CTRL.BF.RC32M_GATE = 1;

	/* Reset the PMU clock divider to 1 */
	CLK_ModuleClkDivider(CLK_PMU, 1);
}

CLK_Src_Type CLK_Get_Current_Sfll_RefCLK()
{
	return sfll_ref_clk;
}

void CLK_Init(CLK_Src_Type input_ref_clk_src)
{
	int freq;
	CLK_Src_Type type;

	sfll_ref_clk = input_ref_clk_src;

	if (board_cpu_freq() == FREQ_32M) {
		Setup_RC32M();
		CLK_SystemClkSrc(CLK_RC32M);
		/* SFLL will not be used as system clock
		 * when board_cpu_frequency = 32M
		 * Hence, disable it
		 */
		CLK_SfllDisable();
		type = CLK_RC32M;
	} else {
		if (CLK_RC32M == sfll_ref_clk) {
			/* SFLL(driven by RC32M) will be used as
			 * the system clock source */
			CLK_RefClkEnable(CLK_RC32M);
			while (CLK_GetClkStatus(CLK_RC32M) == RESET)
				;
			type = CLK_RC32M;
			freq = FREQ_32M;
		} else {
			/* XTAL/SFLL(driven by XTAL) will be used as
			 * the system clock source */
			/* 38.4 MHx XTAL is routed through WLAN */
			PMU_PowerOnWLAN();
			CLK_RefClkEnable(CLK_XTAL_REF);
			while (CLK_GetClkStatus(CLK_XTAL_REF) == RESET)
				;
			type = CLK_XTAL_REF;
			freq = CLK_MAINXTAL_FREQUENCY;
		}
		Setup_RC32M();

		/* On RC32M setup, SystemClkSrc = SFLL
		 * Change the clock to reference clock before configuring PLL */
		CLK_SystemClkSrc(type);

		 /* If board_cpu_frequency > board_main_xtal, SFLL would be
		  * used as system clock source.
		  * SFLL should be disabled otherwise.
		  * Also, SFLL should be disabled before reconfiguring
		  * SFLL to a new frequency value */
		CLK_SfllDisable();

		/*
		* Check if expected cpu frequency is greater than the
		* source clock frequency. In that case we need to enable
		* the PLL.
		*/
		if (board_cpu_freq() > freq) {
			CLK_Config_Pll(freq, type);
			freq = board_cpu_freq();
			type = CLK_SFLL;
		}

		if (freq > 50000000) {
			/* Max APB0 freq 50MHz */
			CLK_ModuleClkDivider(CLK_APB0, 2);
			/* Max APB1 freq 50MHz */
			CLK_ModuleClkDivider(CLK_APB1, 2);
		}
	}
	/* Select clock source */
	CLK_SystemClkSrc(type);

	/* Power down WLAN in order to save power */
	if (CLK_RC32M == sfll_ref_clk)
		PMU_PowerDownWLAN();
}

void HardFault_IRQHandler_C_ptr_register();

__USED__
void Reset_IRQHandler_C(void)
{
	/* Zero fill the bss segment. */
#ifdef __ICCARM__
#pragma section = "BSS_1"
#pragma section = "BSS_2"
	memset((char *) __section_begin("BSS_1"), 0x00,
		(unsigned) __section_size("BSS_1"));

	memset((char *) __section_begin("BSS_2"), 0x00,
		(unsigned) __section_size("BSS_2"));
#else
	memset(&_bss_sram0, 0x00,
		 ((unsigned) &_ebss_sram0 - (unsigned) &_bss_sram0));

	memset(&_bss_sram1, 0x00,
		 ((unsigned) &_ebss_sram1 - (unsigned) &_bss_sram1));
#endif /* __ICCARM__ */

#ifdef CONFIG_ENABLE_MCU_PM3
	memset(&_pm3_bss, 0x00,
		 ((unsigned) &_epm3_bss - (unsigned) &_pm3_bss));
#endif /* CONFIG_ENABLE_MCU_PM3 */
	/*
	 * Vector table relocation:
	 *
	 * 1. Bit 29 decides whether CODE = 0 or SRAM = 1 area.
	 * 2. Bit 0-6 are reserved, hence address should be aligned to
	 *    32 word boundary -- minimal.
	 *
	 * See programming VTOR in cortex-m3 trm for more details.
	 */
	if ((unsigned) __vector_table >= 0x20000000)
		SCB->VTOR = ((uint32_t) __vector_table) & 0x3FFFFF80;
	else
		SCB->VTOR = ((uint32_t) __vector_table) & 0x1FFFFF80;

	/* 4 pre-emption, 4 subpriority bits */
	NVIC_SetPriorityGrouping(4);

	/* Turn ON different power domains. */
	PMU_PowerOnVDDIO(PMU_VDDIO_AON);
	PMU_PowerOnVDDIO(PMU_VDDIO_0);
	PMU_PowerOnVDDIO(PMU_VDDIO_1);
	PMU_PowerOnVDDIO(PMU_VDDIO_2);
	PMU_PowerOnVDDIO(PMU_VDDIO_3);

	/* Enable Brown-out on VBAT */
	PMU_VbatBrndetCmd(ENABLE);
	PMU_VbatBrndetRstCmd(ENABLE);

	/* Zero fill part of nvram if this is a reboot */
	if (pm4_sig != PM4_SIGN) {
#ifdef __ICCARM__
#pragma section = "NVRAM"
		memset((char *) __section_begin("NVRAM"), 0x00,
				(unsigned) __section_size("NVRAM"));
#else
		memset(&_nvram_begin, 0, ((unsigned) &_nvram_end -
				(unsigned) &_nvram_begin));
#endif /* __ICCARM__ */
	}
	pm4_sig = 0;

	/* Do board specific Power On GPIO settings */
	board_gpio_power_on();

	/* Set PMU clock to 32Mhz */
	RC32M->CLK.BF.REF_SEL = 1;
	CLK_ModuleClkDivider(CLK_PMU, 1);

	/* Initialize System Clock */
	CLK_Init(sfll_ref_clk);

	/* Initialize boot flags as set up by Boot2 */
	boot_init();

#ifdef CONFIG_ENABLE_ROM_LIBS
	/* Initialize ROM */
	rom_init();
#endif

#ifdef CONFIG_ENABLE_CPP_SUPPORT
	/*
	 * These constructors and destructors are needed for C++. Can be
	 * removed if using custom linker script without these linker
	 * variables. Default SDK linker script has these variables even
	 * for pure C apps.
	 */
	/* Constructors invocation.*/
	funcpp_t fpp = &__init_array_start;
	while (fpp < &__init_array_end) {
		(*fpp)();
		fpp++;
	}
#endif

	/* This function registers HardFault_IRQHandler_C. */
	/* Intentionally called before os_init() function. */
#ifdef CONFIG_LL_DEBUG
	HardFault_IRQHandler_C_ptr_register();
#endif /* CONFIG_LL_DEBUG */

	/* Initialize Operating System */
	os_init();

#ifdef CONFIG_ENABLE_CPP_SUPPORT
	/* Destructors invocation.*/
	fpp = &__fini_array_start;
	while (fpp < &__fini_array_end) {
		(*fpp)();
		fpp++;
	}
#endif
}

#pragma weak NMI_IRQHandler = Default_IRQHandler
#pragma weak MemManage_IRQHandler = Default_IRQHandler
#pragma weak BusFault_IRQHandler = Default_IRQHandler
#pragma weak UsageFault_IRQHandler = Default_IRQHandler
#pragma weak SVC_IRQHandler = Default_IRQHandler
#pragma weak DebugMonitor_IRQHandler = Default_IRQHandler
#pragma weak PendSV_IRQHandler = Default_IRQHandler
#pragma weak SysTick_IRQHandler = Default_IRQHandler
#pragma weak ExtPin0_IRQHandler = Default_IRQHandler
#pragma weak ExtPin1_IRQHandler = Default_IRQHandler
#pragma weak RTC_IRQHandler = Default_IRQHandler
#pragma weak CRC_IRQHandler = Default_IRQHandler
#pragma weak AES_IRQHandler = Default_IRQHandler
#pragma weak I2C0_IRQHandler = Default_IRQHandler
#pragma weak I2C1_IRQHandler = Default_IRQHandler
#pragma weak DMA_IRQHandler = Default_IRQHandler
#pragma weak GPIO_IRQHandler = Default_IRQHandler
#pragma weak SSP0_IRQHandler = Default_IRQHandler
#pragma weak SSP1_IRQHandler = Default_IRQHandler
#pragma weak SSP2_IRQHandler = Default_IRQHandler
#pragma weak QSPI_IRQHandler = Default_IRQHandler
#pragma weak GPT0_IRQHandler = Default_IRQHandler
#pragma weak GPT1_IRQHandler = Default_IRQHandler
#pragma weak GPT2_IRQHandler = Default_IRQHandler
#pragma weak GPT3_IRQHandler = Default_IRQHandler
#pragma weak UART0_IRQHandler = Default_IRQHandler
#pragma weak UART1_IRQHandler = Default_IRQHandler
#pragma weak UART2_IRQHandler = Default_IRQHandler
#pragma weak WDT_IRQHandler = Default_IRQHandler
#pragma weak ADC0_IRQHandler = Default_IRQHandler
#pragma weak DAC_IRQHandler = Default_IRQHandler
#pragma weak ACOMPWKUP_IRQHandler = Default_IRQHandler
#pragma weak ACOMP_IRQHandler = Default_IRQHandler
#pragma weak SDIO_IRQHandler = Default_IRQHandler
#pragma weak USB_IRQHandler = Default_IRQHandler
#pragma weak PLL_IRQHandler = Default_IRQHandler
#pragma weak RC32M_IRQHandler = Default_IRQHandler
#pragma weak ExtPin3_IRQHandler = Default_IRQHandler
#pragma weak ExtPin4_IRQHandler = Default_IRQHandler
#pragma weak ExtPin5_IRQHandler = Default_IRQHandler
#pragma weak ExtPin6_IRQHandler = Default_IRQHandler
#pragma weak ExtPin7_IRQHandler = Default_IRQHandler
#pragma weak ExtPin8_IRQHandler = Default_IRQHandler
#pragma weak ExtPin9_IRQHandler = Default_IRQHandler
#pragma weak ExtPin10_IRQHandler = Default_IRQHandler
#pragma weak ExtPin11_IRQHandler = Default_IRQHandler
#pragma weak ExtPin12_IRQHandler = Default_IRQHandler
#pragma weak ExtPin13_IRQHandler = Default_IRQHandler
#pragma weak ExtPin14_IRQHandler = Default_IRQHandler
#pragma weak ExtPin15_IRQHandler = Default_IRQHandler
#pragma weak ExtPin16_IRQHandler = Default_IRQHandler
#pragma weak ExtPin17_IRQHandler = Default_IRQHandler
#pragma weak ExtPin18_IRQHandler = Default_IRQHandler
#pragma weak ExtPin19_IRQHandler = Default_IRQHandler
#pragma weak ExtPin20_IRQHandler = Default_IRQHandler
#pragma weak ExtPin21_IRQHandler = Default_IRQHandler
#pragma weak ExtPin22_IRQHandler = Default_IRQHandler
#pragma weak ExtPin23_IRQHandler = Default_IRQHandler
#pragma weak ExtPin24_IRQHandler = Default_IRQHandler
#pragma weak ExtPin25_IRQHandler = Default_IRQHandler
#pragma weak ExtPin26_IRQHandler = Default_IRQHandler
#pragma weak ExtPin27_IRQHandler = Default_IRQHandler
#pragma weak ULPCOMP_IRQHandler = Default_IRQHandler
#pragma weak BRNDET_IRQHandler = Default_IRQHandler
#pragma weak WIFIWKUP_IRQHandler = Default_IRQHandler

void Default_IRQHandler(void)
{
	/*
	 * Check bit 2 in LR to find which stack pointer we should refer to,
	 * if bit = 0 then main stack pointer (msp) else process stack
	 * pointer (psp).
	 */
	__asm volatile (
		"tst lr, #4\n"
		"ite eq\n"
		"mrseq r0, msp\n"
		"mrsne r0, psp\n"
		"b Default_IRQHandler_C\n"
	);
}

__USED__
void Default_IRQHandler_C(unsigned long *addr)
{
#ifdef CONFIG_LL_DEBUG
	stframe_t *stf;
	ll_printf("NVIC->ISPR[0] : %x\r\n", NVIC->ISPR[0]);
	ll_printf("NVIC->ISPR[1] : %x\r\n", NVIC->ISPR[1]);

	ll_printf("NVIC->ISER[0] : %x\r\n", NVIC->ISER[0]);
	ll_printf("NVIC->ISER[1] : %x\r\n", NVIC->ISER[1]);

	ll_printf("NVIC->ISPR[0] &  NVIC->ISER[0] : %x\r\n",
		  NVIC->ISER[0] & NVIC->ISPR[0]);
	ll_printf("NVIC->ISPR[1] &  NVIC->ISER[1] : %x\r\n",
		  NVIC->ISER[1] & NVIC->ISPR[1]);


	stf = (stframe_t *) addr;
	ll_printf("Default IRQHandler: using stack pointer @ 0x%08x",
		  (unsigned int)addr);

	ll_printf("\r\nr0 = 0x%08x\r\nr1 = 0x%08x\r\nr2 = 0x%08x\r\n"
		  "r3 = 0x%08x\r\nr12 = 0x%08x\r\nlr = 0x%08x\r\npc = 0x%08x"
		  "\r\npsr = 0x%08x\r\n", stf->r0, stf->r1, stf->r2, stf->r3,
		  stf->r12, stf->lr, stf->pc, stf->psr);

	ll_printf("Task name %s\r\n", get_current_taskname());

#endif
	while (1)
		;
}
