@
@ Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 @ Cypress Semiconductor Corporation. All Rights Reserved.
 @ 
 @ This software, associated documentation and materials ("Software")
 @ is owned by Cypress Semiconductor Corporation,
 @ or one of its subsidiaries ("Cypress") and is protected by and subject to
 @ worldwide patent protection (United States and foreign),
 @ United States copyright laws and international treaty provisions.
 @ Therefore, you may use this Software only as provided in the license
 @ agreement accompanying the software package from which you
 @ obtained this Software ("EULA").
 @ If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 @ non-transferable license to copy, modify, and compile the Software
 @ source code solely for use in connection with Cypress's
 @ integrated circuit products. Any reproduction, modification, translation,
 @ compilation, or representation of this Software except as specified
 @ above is prohibited without the express written permission of Cypress.
 @
 @ Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 @ EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 @ WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 @ reserves the right to make changes to the Software without notice. Cypress
 @ does not assume any liability arising out of the application or use of the
 @ Software or any product or circuit described in the Software. Cypress does
 @ not authorize its products for use in any products where a malfunction or
 @ failure of the Cypress product may reasonably be expected to result in
 @ significant property damage, injury or death ("High Risk Product"). By
 @ including Cypress's product in a High Risk Product, the manufacturer
 @ of such system or application assumes all risk of such use and in doing
 @ so agrees to indemnify Cypress against all liability.
@

#include "platform_map.h"
#include "platform_checkpoint.h"
#include "platform_config.h"

#define BOOT_CHECKPOINT(val) WICED_BOOT_CHECKPOINT_WRITE_ASM((val), r11, r12)

#define SYS_CTRL_INIT_VAL \
    (0 << 31)  /* little endian */                            | \
    (0 << 30)  /* exceptions in arm mode */                   | \
    (0 << 27)  /* software can disable FIQ */                 | \
    (0 << 25)  /* CPSR E bit set to 0 on an exeption */       | \
    (0 << 24)  /* no VIC */                                   | \
    (1 << 19)  /* generate exception for divide by zero */    | \
    (0 << 17)  /* MPU background region disable */            | \
    (0 << 13)  /* normal excepion vectors (low addresses)*/   | \
    (0 << 12)  /* disable icache */                           | \
    (0 << 2)   /* disable dcache */                           | \
    (0 << 1)   /* strict alignment fault checking disabled */ | \
    (0 << 0)   /* MPU disabled */

#define CPSR_BASE_INIT_VAL  0xC0 /* Disable IRQ, FIQ and imprecise aborts */
#define CPSR_FIQ_INIT_VAL   (CPSR_BASE_INIT_VAL | 0x11)
#define CPSR_IRQ_INIT_VAL   (CPSR_BASE_INIT_VAL | 0x12)
#define CPSR_SVC_INIT_VAL   (CPSR_BASE_INIT_VAL | 0x13)
#define CPSR_ABORT_INIT_VAL (CPSR_BASE_INIT_VAL | 0x17)
#define CPSR_UNDEF_INIT_VAL (CPSR_BASE_INIT_VAL | 0x1B)

     /* Reset handler */

    .text
    .section .text.startup, "ax"
    .align 2 /* power of */
    .arm

    .global _low_start
_low_start:
    /*
     * Initial configuration.
     * Expect dcache is flushed on entry, or maybe even disabled after flushing.
     * But we should be fine even if not disabled because do disabling it very early.
     */

    BOOT_CHECKPOINT(0)

    // Initialize CPSR to SVC mode
    MSR        cpsr, #CPSR_SVC_INIT_VAL

    // Initialize system control register
    LDR        r10, =SYS_CTRL_INIT_VAL /* Assign value to r10 */
    MCR        p15, 0, r10, c1, c0, 0  /* Initialize system control register. Keep r10 value */

    // Invalidate caches
    EOR        r0, r0                  /* Init zero value */
    MCR        p15, 0, r0, c15, c5, 0  /* Invalidate dcache */
    MCR        p15, 0, r0, c7, c5, 0   /* Invalidate icache */

    // Make sure above changes applied at this point
    ISB                                /* Barrier */

#ifndef WICED_NO_VECTORS
    /* Copy vectors to flops */
    BOOT_CHECKPOINT(1)
    LDR        r8, =link_interrupt_vectors_copyfrom /* Source address */
    MOV        r9, #0                  /* Destination address */
    LDM        r8!, {r0 - r7}          /* Read 32 bytes */
    STM        r9!, {r0 - r7}          /* Write 32 bytes */
    DSB                                /* Barrier */
#endif

    /* Program MPU, memory and enable caches */

    BOOT_CHECKPOINT(2)

    // Read MPU configuration
    MRC        p15, 0, r5, c0, c0, 4   /* Get MPU type */
    UBFX       r5, r5, #8, #8          /* Number of unified MPU regions */

    // Check that region number is non-zero. If zero do not enable MPU
    CMP        r5, #0                  /* Check that region number is zero */
    BEQ        5f                      /* Skip MPU programming */
    ORR        r10, r10, #0x1 << 0     /* Cfg: enable MPU */

    // Loop through MPU regions. Disable each one.
    EOR        r0, r0                  /* Region index assign 0 */
    EOR        r1, r1                  /* Value used to disable MPU region */
1:
    CMP        r0, r5                  /* Compare index with number of regions */
    BEQ        2f                      /* Break loop when index moved past last region */
    MCR        p15, 0, r0, c6, c2, 0   /* MPU Memory Region Number */
    MCR        p15, 0, r1, c6, c1, 2   /* MPU Region Size and Enable */
    ADD        r0, #1                  /* Increment index */
    B          1b                      /* Continue loop */

    // Loop through table entries. Assign next region using next table entry.
2:
    LDR        r4, =_mpudcfg           /* MPU cfg table */
    EOR        r0, r0                  /* Region index assign 0 */
3:
    LDMIA      r4!, {r1, r2, r3}       /* Load table entry */
    CMP        r2, #0                  /* Last entry has size=0 */
    BEQ        5f                      /* Quit confguration for last entry */
    CMP        r0, r5                  /* Compare index with regions number */
4:
    BEQ        4b                      /* Misconfiguration (more regions asked to set then exists), loop forever */
    MCR        p15, 0, r0, c6, c2, 0   /* MPU Memory Region Number */
    MCR        p15, 0, r1, c6, c1, 0   /* MPU Region Base Address */
    MCR        p15, 0, r2, c6, c1, 2   /* MPU Region Size and Enable */
    MCR        p15, 0, r3, c6, c1, 4   /* MPU Region Access Control */
    ADD        r0, #1                  /* Increment index */
    B          3b                      /* Continue loop */
5:

    BOOT_CHECKPOINT(3)

    // Out-of-reset second channel to SOCSRAM
    MOV        r0, #0x0
    LDR        r1, =PLATFORM_SOCSRAM_CH1_SLAVE_WRAPPER_RESET_CTRL
    STR        r0, [r1]

    // Finally enable caches and MPU
    ORR        r10, r10, #0x1 << 12    /* Cfg: enable icache */
#ifdef PLATFORM_L1_CACHE_SHIFT
    ORR        r10, r10, #0x1 << 2     /* Cfg: enable dcache if it is enabled in makefiles */
#endif
    MCR        p15, 0, r10, c1, c0, 0  /* Apply final changes and write register back */

    // Make sure above changes applied at this point
    ISB                                /* Barrier */

    /* Setup stacks */

    BOOT_CHECKPOINT(4)

#ifndef WICED_NO_VECTORS
    // Prepare isr stack aligned on 8 byte
    LDR        r0, =link_stack_isr_end
    LSR        r0, r0, #3
    LSL        r0, r0, #3

    // Setup FIQ stack
    MSR        cpsr, #CPSR_FIQ_INIT_VAL
    MOV        sp, r0

    // Setup IRQ stack
    MSR        cpsr, #CPSR_IRQ_INIT_VAL
    MOV        sp, r0
#endif

    // Prepare stack aligned on 8 byte
    LDR        r0, =link_stack_end
    LSR        r0, r0, #3
    LSL        r0, r0, #3

#ifndef WICED_NO_VECTORS
    // Setup UNDEF mode stack
    MSR        cpsr, #CPSR_UNDEF_INIT_VAL
    MOV        sp, r0

    // Setup ABORT mode stack
    MSR        cpsr, #CPSR_ABORT_INIT_VAL
    MOV        sp, r0
#endif

    // Setup SVC (initial) stack
    MSR        cpsr, #CPSR_SVC_INIT_VAL
    MOV        sp, r0

    /* Continue to init */

    BOOT_CHECKPOINT(5)

#ifndef WICED_NO_VECTORS
    // Enable imprecise aborts
    CPSIE      A
#endif

    // Jump into C-code
    LDR        r0, =_start
    BOOT_CHECKPOINT(6)
    BLX        r0



    /* MPU config table */
    .data
    .section .rodata.startup
    .align 2
#define MPU_R_ACCESS_XN              (1 << 12)          /* Execute Never */
#define MPU_R_ACCESS_AP(a)           (((a) & 0x7) << 8) /* Access Permission */
#define MPU_R_ACCESS_TEX(x)          (((x) & 0x7) << 3) /* C, B and TEX has to be treated together to define attribute (used to be Type EXtension) */
#define MPU_R_ACCESS_S(s)            (((s) & 0x1) << 2) /* Share */
#define MPU_R_ACCESS_C(c)            (((c) & 0x1) << 1) /* C, B and TEX has to be treated together to define attribute (used to be Cacheable) */
#define MPU_R_ACCESS_B(b)            (((b) & 0x1) << 0) /* C, B and TEX has to be treated together to define attribute (usef to be Bufferable) */
//---
#define MPU_R_ACCESS_AP_FULL         MPU_R_ACCESS_AP(0x3)
#define MPU_R_ACCESS_AP_READ_ONLY    MPU_R_ACCESS_AP(0x5)
#define MPU_R_ACCESS_STR_ORDERED     (MPU_R_ACCESS_TEX(0x0) | MPU_R_ACCESS_C(0x0) | MPU_R_ACCESS_B(0x0))
#define MPU_R_ACCESS_SHAREABLE_DEV   (MPU_R_ACCESS_TEX(0x0) | MPU_R_ACCESS_C(0x0) | MPU_R_ACCESS_B(0x1))
#define MPU_R_ACCESS_CACHEABLE_WBACK (MPU_R_ACCESS_TEX(0x5) | MPU_R_ACCESS_C(0x0) | MPU_R_ACCESS_B(0x1))
#define MPU_R_ACCESS_CACHEABLE_WTHR  (MPU_R_ACCESS_TEX(0x6) | MPU_R_ACCESS_C(0x1) | MPU_R_ACCESS_B(0x0))
#ifdef WICED_DCACHE_WTHROUGH
#define MPU_R_ACCESS_CACHEABLE       MPU_R_ACCESS_CACHEABLE_WTHR
#else
#define MPU_R_ACCESS_CACHEABLE       MPU_R_ACCESS_CACHEABLE_WBACK
#endif
#define MPU_R_ACCESS_UNCACHEABLE     (MPU_R_ACCESS_TEX(0x4) | MPU_R_ACCESS_C(0x0) | MPU_R_ACCESS_B(0x0))
//---
#define MPU_R_ACCESS_DEV             (MPU_R_ACCESS_SHAREABLE_DEV | MPU_R_ACCESS_AP_FULL | MPU_R_ACCESS_XN)
#define MPU_R_ACCESS_STR_ORDERED_MEM (MPU_R_ACCESS_STR_ORDERED | MPU_R_ACCESS_AP_FULL | MPU_R_ACCESS_XN)
#define MPU_R_ACCESS_CACHED_MEM      (MPU_R_ACCESS_CACHEABLE | MPU_R_ACCESS_AP_FULL)
#define MPU_R_ACCESS_CACHED_WTHR_MEM (MPU_R_ACCESS_CACHEABLE_WTHR | MPU_R_ACCESS_AP_FULL)
#define MPU_R_ACCESS_UNCACHED_MEM    (MPU_R_ACCESS_UNCACHEABLE | MPU_R_ACCESS_AP_FULL | MPU_R_ACCESS_S(0x1) | MPU_R_ACCESS_XN)
#define MPU_R_ACCESS_EXEC_ONLY_MEM   (MPU_R_ACCESS_CACHEABLE | MPU_R_ACCESS_AP_READ_ONLY) /* to execute instructions region must have read access */
//---
#define MPU_R_SIZE_ENABLE            (1 << 0)
#define MPU_R_SIZE_SZ(sz)            (((sz) & 0x1F) << 1)
#define MPU_R_SIZE_32                (MPU_R_SIZE_SZ(4) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_64                (MPU_R_SIZE_SZ(5) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_128               (MPU_R_SIZE_SZ(6) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_256               (MPU_R_SIZE_SZ(7) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_512               (MPU_R_SIZE_SZ(8) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_1K                (MPU_R_SIZE_SZ(9) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_2K                (MPU_R_SIZE_SZ(10) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_4K                (MPU_R_SIZE_SZ(11) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_8K                (MPU_R_SIZE_SZ(12) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_16K               (MPU_R_SIZE_SZ(13) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_32K               (MPU_R_SIZE_SZ(14) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_64K               (MPU_R_SIZE_SZ(15) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_128K              (MPU_R_SIZE_SZ(16) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_256K              (MPU_R_SIZE_SZ(17) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_512K              (MPU_R_SIZE_SZ(18) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_1M                (MPU_R_SIZE_SZ(19) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_2M                (MPU_R_SIZE_SZ(20) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_4M                (MPU_R_SIZE_SZ(21) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_8M                (MPU_R_SIZE_SZ(22) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_16M               (MPU_R_SIZE_SZ(23) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_32M               (MPU_R_SIZE_SZ(24) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_64M               (MPU_R_SIZE_SZ(25) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_128M              (MPU_R_SIZE_SZ(26) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_256M              (MPU_R_SIZE_SZ(27) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_512M              (MPU_R_SIZE_SZ(28) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_1G                (MPU_R_SIZE_SZ(29) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_2G                (MPU_R_SIZE_SZ(30) | MPU_R_SIZE_ENABLE)
#define MPU_R_SIZE_4G                (MPU_R_SIZE_SZ(31) | MPU_R_SIZE_ENABLE)
//---
#define MPU_REGION(base, size, type) .word base, size, type
//---
_mpudcfg:
    //
    // Regions are listed in the order how they will be programmed.
    // Regions can overlap. In this case higher index region has higher priority.
    //
    //
    // SOCSRAM memory (cached window).
    // Cache policy depends on compile-time option.
    //
    MPU_REGION(PLATFORM_SOCSRAM_CH0_BASE(0x0), MPU_R_SIZE_4M, MPU_R_ACCESS_CACHED_MEM)
    //
    // SOCSRAM memory (uncached window to same memory).
    // Uncached, shareable. For DMA descriptor region.
    //
    MPU_REGION(PLATFORM_SOCSRAM_CH1_BASE(0x0), MPU_R_SIZE_4M, MPU_R_ACCESS_UNCACHED_MEM)
    //
    // Always-On SOCSRAM memory.
    // Use write-through cache policy to ensure that write to this memory survives deep-sleep without explicit cache cleaning.
    //
#ifdef PLATFORM_AON_MEMORY_ACCESS
    MPU_REGION(PLATFORM_SOCSRAM_CH0_AON_RAM_BASE(0x0), MPU_R_SIZE_8K, PLATFORM_AON_MEMORY_ACCESS)
#elif !defined(WICED_DCACHE_WTHROUGH)
    MPU_REGION(PLATFORM_SOCSRAM_CH0_AON_RAM_BASE(0x0), MPU_R_SIZE_8K, MPU_R_ACCESS_CACHED_WTHR_MEM)
#endif
    //
    // WIFI ATCM.
    //
#ifndef WICED_NO_WIFI
    MPU_REGION(PLATFORM_ATCM_BASE(0x0), MPU_R_SIZE_4M, MPU_R_ACCESS_UNCACHED_MEM)
#endif
    //
    // Vectors for execution only. Let's catch NULL pointers.
    //
#ifndef WICED_NO_VECTORS
    MPU_REGION(0x0, MPU_R_SIZE_32, MPU_R_ACCESS_EXEC_ONLY_MEM)
#endif
    //
    // Device registers.
    //
    MPU_REGION(PLATFORM_REGBASE(0x0), MPU_R_SIZE_2M, MPU_R_ACCESS_DEV)
    //
    // Direct Serial Flash access.
    //
#ifdef APPLICATION_XIP_ENABLE
    MPU_REGION(PLATFORM_FLASH_BASE(0x0), MPU_R_SIZE_64M, MPU_R_ACCESS_EXEC_ONLY_MEM)
#else
    MPU_REGION(PLATFORM_FLASH_BASE(0x0), MPU_R_SIZE_64M, MPU_R_ACCESS_DEV)
#endif
    //
    // DDR memory.
    // Cache policy depends on compile-time option.
    //
#if !PLATFORM_NO_DDR
    MPU_REGION(PLATFORM_DDR_BASE(0x0), MPU_R_SIZE_128M, MPU_R_ACCESS_CACHED_MEM)
#endif
    //
    // Indicator that MPU table configuration is done.
    //
    MPU_REGION(0, 0, 0)



/*
 * void platform_start_app( uint32_t entry_point )
 */
     .text
    .section .text.platform_start_app, "ax"
    .align 2
    .arm

    .global platform_start_app
    .type platform_start_app, %function
platform_start_app:
    /* Simulate a reset in bootloader when it loads the main app: */
    /*   Switch to Service Mode */
    /*   Jump to the entry point */
    /* Function does not return */

    /* Make sure that argument is preserved accross further function calls. Function does not return, so no need to save r4 before. */
    MOV        r4, r0

    /* Re-Initialize CPSR to SVC mode */
    MSR        cpsr, #CPSR_SVC_INIT_VAL

    /* Re-Initialize system control register. During reinitialization caches will be switched off. */
    ISB
    DSB
    LDR        r0, =SYS_CTRL_INIT_VAL
    MCR        p15, 0, r0, c1, c0, 0

    /* Invalidate instruction cache */
    BLX        platform_icache_inv_all

#ifdef PLATFORM_L1_CACHE_SHIFT
    /* Clean and invalidate data cache */
    BLX        platform_dcache_clean_and_inv_all
#endif

    /* Clear LR to avoid the debugger showing any stack frame */
    MOV        lr, #0

    /* Jump to entry address */
    ISB
    DSB
    BX         r4
