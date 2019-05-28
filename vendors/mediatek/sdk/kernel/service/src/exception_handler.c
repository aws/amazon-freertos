/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

 /* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stdarg.h"
#include "exception_handler.h"

#include "FreeRTOSConfig.h"

#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
#endif /* __GNUC__ */

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
#include "string.h"
#include "fota_76x7.h"
#include "fota_config.h" 
#include "hal_flash.h"

#if defined(SERIAL_NOR_WITH_SPI_SUPPORT)
#include "serial_nor_flash.h"
#endif
#include "hal_gpio.h"

extern void *xTaskGetCurrentTaskHandle( void );
extern uint32_t get_crash_context_base(void);
extern uint32_t get_crash_context_length(void);
extern uint32_t get_ext_crash_context_base(void);
extern uint32_t get_ext_crash_context_length(void);



#endif /* MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE */

#define MAX_EXCEPTION_CONFIGURATIONS 6

#if defined(MTK_NO_PSRAM_ENABLE)

unsigned int *pxExceptionStack = 0;

#else /* ! MTK_NO_PSRAM_ENABLE */

#define CHECK_EXCEPTION_STACK_USAGE 0
#if (CHECK_EXCEPTION_STACK_USAGE == 1)
#include <string.h>
#endif

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
#define EXCEPTION_STACK_WORDS 384
#else
#define EXCEPTION_STACK_WORDS 192
#endif /* MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE */

static unsigned int xExceptionStack[EXCEPTION_STACK_WORDS] = {0}; /* reserved as exception handler's stack */
unsigned int *pxExceptionStack = &xExceptionStack[EXCEPTION_STACK_WORDS-1];

#endif /* MTK_NO_PSRAM_ENABLE */

typedef struct
{
  int items;
  exception_config_type configs[MAX_EXCEPTION_CONFIGURATIONS];
} exception_config_t;

#if 0
typedef struct
{
  bool is_valid;
  const char *expr;
  const char *file;
  int line;
} assert_expr_t;
#endif

static exception_config_t exception_config = {0};

assert_expr_t assert_expr = {0};

static int reboot_flag = 0;

extern memory_region_type memory_regions[];

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
void abort(void)
{
    __asm("cpsid i");
    SCB->CCR |=  SCB_CCR_UNALIGN_TRP_Msk;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;
    for (;;);
}

#if 0
void platform_assert(const char *expr, const char *file, int line)
{
    __asm("cpsid i");
    SCB->CCR |=  SCB_CCR_UNALIGN_TRP_Msk;
    assert_expr.is_valid = true;
    assert_expr.expr = expr;
    assert_expr.file = file;
    assert_expr.line = line;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;
    for (;;);
}
#endif

void exception_dump_config(int flag)
{
    reboot_flag = flag;
}

#if defined (__CC_ARM) || defined (__ICCARM__)

void __aeabi_assert(const char *expr, const char *file, int line)
{
    platform_assert(expr, file, line);
}

#endif /* __CC_ARM */

bool exception_register_callbacks(exception_config_type *cb)
{
    int i;

    if (exception_config.items >= MAX_EXCEPTION_CONFIGURATIONS) {
       return false;
    } else {
       /* check if it is already registered */
       for (i = 0; i < exception_config.items; i++) {
           if ( exception_config.configs[i].init_cb == cb->init_cb
             && exception_config.configs[i].dump_cb == cb->dump_cb) {
                return false;
           }
       }
       exception_config.configs[exception_config.items].init_cb = cb->init_cb;
       exception_config.configs[exception_config.items].dump_cb = cb->dump_cb;
       exception_config.items++;
       return true;
    }
}

#if defined (__ICCARM__)

#define __EXHDLR_INIT__
#pragma location=".ram_code"
void exception_init(void);

#else

#if (PRODUCT_VERSION == 2523) || (PRODUCT_VERSION == 2533)
#define __EXHDLR_INIT__   __attribute__((section(".ram_code")))
#endif

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)
#define __EXHDLR_INIT__
#endif

#endif

__EXHDLR_INIT__ void exception_init(void)
{
    int i;

    SCB->CCR &= ~SCB_CCR_UNALIGN_TRP_Msk;

#if (configUSE_FLASH_SUSPEND == 1)
    Flash_ReturnReady();
#endif

#if (CHECK_EXCEPTION_STACK_USAGE == 1)
    memset(xExceptionStack, (int)0xa5, (EXCEPTION_STACK_WORDS - 16)*4);
#endif

    for (i = 0; i < exception_config.items; i++) {
        if (exception_config.configs[i].init_cb) {
            exception_config.configs[i].init_cb();
        }
    }

    if (assert_expr.is_valid) {
        printf("assert failed: %s, file: %s, line: %d\n\r",
               assert_expr.expr,
               assert_expr.file,
               assert_expr.line);
    }
}

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

void printUsageErrorMsg(uint32_t CFSRValue)
{
    printf("Usage fault: ");
    CFSRValue >>= 16; /* right shift to lsb */
    if ((CFSRValue & (1 << 9)) != 0) {
        printf("Divide by zero\n\r");
    }
    if ((CFSRValue & (1 << 8)) != 0) {
        printf("Unaligned access\n\r");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        printf("Coprocessor error\n\r");
    }
    if ((CFSRValue & (1 << 2)) != 0) {
        printf("Invalid EXC_RETURN\n\r");
    }
    if ((CFSRValue & (1 << 1)) != 0) {
        printf("Invalid state\n\r");
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        printf("Undefined instruction\n\r");
    }
}

void printMemoryManagementErrorMsg(uint32_t CFSRValue)
{
    printf("Memory Management fault: ");
    CFSRValue &= 0x000000FF; /* mask mem faults */
    if ((CFSRValue & (1 << 5)) != 0) {
        printf("A MemManage fault occurred during FP lazy state preservation\n\r");
    }
    if ((CFSRValue & (1 << 4)) != 0) {
        printf("A derived MemManage fault occurred on exception entry\n\r");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        printf("A derived MemManage fault occurred on exception return\n\r");
    }
    if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
        printf("Data access violation @0x%08x\n\r", (unsigned int)SCB->MMFAR);
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        printf("MPU or Execute Never (XN) default memory map access violation\n\r");
    }
    if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
        printf("SCB->MMFAR = 0x%08x\n\r", (unsigned int)SCB->MMFAR );
    }
}

void printBusFaultErrorMsg(uint32_t CFSRValue)
{
    printf("Bus fault: ");
    CFSRValue &= 0x0000FF00; /* mask bus faults */
    CFSRValue >>= 8;
    if ((CFSRValue & (1 << 5)) != 0) {
        printf("A bus fault occurred during FP lazy state preservation\n\r");
    }
    if ((CFSRValue & (1 << 4)) != 0) {
        printf("A derived bus fault has occurred on exception entry\n\r");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        printf("A derived bus fault has occurred on exception return\n\r");
    }
    if ((CFSRValue & (1 << 2)) != 0) {
        printf("Imprecise data access error has occurred\n\r");
    }
    if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
        printf("A precise data access error has occurred @x%08x\n\r", (unsigned int)SCB->BFAR);
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        printf("A bus fault on an instruction prefetch has occurred\n\r");
    }
    if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
        printf("SCB->BFAR = 0x%08x\n\r", (unsigned int)SCB->BFAR );
    }
}

enum { r0, r1, r2, r3, r12, lr, pc, psr,
       s0, s1, s2, s3, s4, s5, s6, s7,
       s8, s9, s10, s11, s12, s13, s14, s15,
       fpscr
     };

typedef struct TaskContextType {
    unsigned int r0;
    unsigned int r1;
    unsigned int r2;
    unsigned int r3;
    unsigned int r4;
    unsigned int r5;
    unsigned int r6;
    unsigned int r7;
    unsigned int r8;
    unsigned int r9;
    unsigned int r10;
    unsigned int r11;
    unsigned int r12;
    unsigned int sp;              /* after pop r0-r3, lr, pc, xpsr                   */
    unsigned int lr;              /* lr before exception                             */
    unsigned int pc;              /* pc before exception                             */
    unsigned int psr;             /* xpsr before exeption                            */
    unsigned int control;         /* nPRIV bit & FPCA bit meaningful, SPSEL bit = 0  */
    unsigned int exc_return;      /* current lr                                      */
    unsigned int msp;             /* msp                                             */
    unsigned int psp;             /* psp                                             */
    unsigned int fpscr;
    unsigned int s0;
    unsigned int s1;
    unsigned int s2;
    unsigned int s3;
    unsigned int s4;
    unsigned int s5;
    unsigned int s6;
    unsigned int s7;
    unsigned int s8;
    unsigned int s9;
    unsigned int s10;
    unsigned int s11;
    unsigned int s12;
    unsigned int s13;
    unsigned int s14;
    unsigned int s15;
    unsigned int s16;
    unsigned int s17;
    unsigned int s18;
    unsigned int s19;
    unsigned int s20;
    unsigned int s21;
    unsigned int s22;
    unsigned int s23;
    unsigned int s24;
    unsigned int s25;
    unsigned int s26;
    unsigned int s27;
    unsigned int s28;
    unsigned int s29;
    unsigned int s30;
    unsigned int s31;
} TaskContext;

static TaskContext taskContext = {0};
TaskContext *pTaskContext = &taskContext;

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)

void exception_get_assert_expr(const char **expr, const char **file, int *line)
{
    if (assert_expr.is_valid) {
        *expr = assert_expr.expr;
        *file = assert_expr.file;
        *line = assert_expr.line;
    } else {
        *expr = NULL;
        *file = NULL;
        *line = 0;
    }
}

#define MAX_MEMORY_REGIONS 8 /* sync with memory_regions.c */

static memory_region_type load_regions[MAX_MEMORY_REGIONS];

static unsigned int sort_memory_regions(memory_region_type *load_regions)
{
    memory_region_type swap;
    int i, j, entries;
    bool swapped = false;
    for (i=0, entries=0; i < MAX_MEMORY_REGIONS; i++) {

        if (!memory_regions[i].region_name) {
            break;
        }

        if (!memory_regions[i].is_dumped) {
            continue;
        }

        load_regions[entries++] = memory_regions[i];
    }

    for (i=0; i < (entries-1); i++) {
        for (j = 0; j < (entries-1)-i; j++) {
            if (load_regions[j].start_address > load_regions[j+1].start_address) {
                swap = load_regions[j];
                load_regions[j] = load_regions[j+1];
                load_regions[j+1] = swap;
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }

    return entries;
}

static bool crash_dump_status=true;

void crash_dump_save_to_flash_enable (bool enable) 
{
  crash_dump_status=enable;
}

bool is_crash_dump_save_to_flash(void)
{
  return  crash_dump_status;
}


bool do_write_to_ext_flash(void)
{
   
	 if ( fota_is_saved_on_ext_flash() && (fota_is_triggered() != true)){
           	return true;
	 } else {
           return false;
	 }
	 	
}
#if defined(SERIAL_NOR_WITH_SPI_SUPPORT)

static uint32_t gpio7_mode;
static uint32_t gpio24_mode;
static uint32_t gpio25_mode;
static uint32_t gpio26_mode;
void crash_ext_flash_init()
{
    printf("serial_nor_init\r\n");
    gpio7_mode =  (*(volatile uint32_t*)0x81023020 >> 28) & 0x0f;
    gpio24_mode = (*(volatile uint32_t*)0x81023020 >> 0) & 0x0f;
    gpio25_mode = (*(volatile uint32_t*)0x81023020 >> 4) & 0x0f;
    gpio26_mode = (*(volatile uint32_t*)0x81023020 >> 8) & 0x0f;

    hal_gpio_init(HAL_GPIO_7);
    hal_gpio_init(HAL_GPIO_24);
    hal_gpio_init(HAL_GPIO_25);
    hal_gpio_init(HAL_GPIO_26);

    /* Call hal_pinmux_set_function() to set GPIO pinmux*/
    hal_pinmux_set_function(HAL_GPIO_24, HAL_GPIO_24_SPI_MOSI_M_CM4);
    hal_pinmux_set_function(HAL_GPIO_25, HAL_GPIO_25_SPI_MISO_M_CM4);
    hal_pinmux_set_function(HAL_GPIO_26, HAL_GPIO_26_SPI_SCK_M_CM4);
    hal_pinmux_set_function(HAL_GPIO_7, HAL_GPIO_7_GPIO7);

    serial_nor_init(HAL_SPI_MASTER_0, 1000000);
}


void crash_ext_flash_deinit()
{   
    printf("serial_nor_deinit\r\n");
    serial_nor_deinit();

    /* restore pinmux setting */
    hal_pinmux_set_function(HAL_GPIO_24, gpio24_mode);
    hal_pinmux_set_function(HAL_GPIO_25, gpio25_mode);
    hal_pinmux_set_function(HAL_GPIO_26, gpio26_mode);
    hal_pinmux_set_function(HAL_GPIO_7, gpio7_mode);
}

#endif

hal_flash_status_t hal_flash_erase_wrap(uint32_t start_address, hal_flash_block_t block)
{

  hal_flash_status_t status;
	if (do_write_to_ext_flash()==true) {
#if defined(SERIAL_NOR_WITH_SPI_SUPPORT)
	status = (hal_flash_status_t)serial_nor_erase(start_address, (block_size_type_t)block);
#else
    status = HAL_FLASH_STATUS_ERROR;
#endif
	}
	else {
	status = hal_flash_erase(start_address, block);

	}

	return status;
}

hal_flash_status_t hal_flash_write_wrap(uint32_t address, const uint8_t *data, uint32_t length)
{
	hal_flash_status_t status;
	  if (do_write_to_ext_flash()==true) {
	#if defined(SERIAL_NOR_WITH_SPI_SUPPORT)
	  status = (hal_flash_status_t)serial_nor_write(address, (uint8_t*)data, length);
	#else
    status = HAL_FLASH_STATUS_ERROR;
    #endif
	  printf("\r\n serial_nor_write address %x, length is %x",address,length);
	  }
	  else {
	  status = hal_flash_write(address, data, length);
	
	  }
	
	  return status;	  
}

hal_flash_status_t hal_flash_read_wrap(uint32_t start_address, uint8_t *buffer, uint32_t length)

{
	hal_flash_status_t status;
	  if (do_write_to_ext_flash()==true) {
	#if defined(SERIAL_NOR_WITH_SPI_SUPPORT)
	  status = (hal_flash_status_t)serial_nor_read(start_address, buffer, length);
   #else
      status = HAL_FLASH_STATUS_ERROR;
    #endif
	 // printf("\r\n serial_nor_read address %x, length is %x",start_address,length);
	  }
	  else {
	  status = hal_flash_read(start_address, buffer, length);
	
	  }
	
	  return status;	  
}

static uint32_t core_get_crash_size()
{

static uint32_t crash_context_length = 0;
 if (do_write_to_ext_flash()==true) {

     crash_context_length =  get_ext_crash_context_length();
 	} else
 		{
	crash_context_length = get_crash_context_length();

	}

printf("\n\r core_get_crash_size  crash_context_length = 0x%08x\n\r", crash_context_length);

return  crash_context_length;
}

static uint32_t core_get_crash_address()
{

static uint32_t crash_context_base = 0;
if (do_write_to_ext_flash()==true) {

    crash_context_base = get_ext_crash_context_base();
} else {

	crash_context_base = get_crash_context_base();
}



printf("\n\r core_get_crash_address  crash_context_base = 0x%08x\n\r", crash_context_base);

return  crash_context_base;

}

static unsigned int coredump_elf_hdr(unsigned int offset, int e_phnum)
{
    uint8_t buf[52] = {0};
    hal_flash_status_t status;
	uint32_t crash_context_base = 0;
    crash_context_base =  core_get_crash_address();

    buf[0] = 0x7f;                       /* e_ident[EI_MAG0] = 0x7f */
    buf[1] = 0x45;                       /* e_ident[EI_MAG1] = 'E'  */
    buf[2] = 0x4c;                       /* e_ident[EI_MAG2] = 'L'  */
    buf[3] = 0x46;                       /* e_ident[EI_MAG3] = 'F'  */
    buf[4] = 0x1;                        /* 32-bit ELF              */
    buf[5] = 0x1;                        /* little endian           */
    buf[6] = 0x1;                        /* e_ident[EI_VERSION]     */
    *((uint16_t*)(buf+16)) = 0x0004;     /* e_type = core file      */
    *((uint16_t*)(buf+18)) = 0x0028;     /* e_machine = ARM         */
    *((uint32_t*)(buf+20)) = 0x00000001; /* e_version = 1           */
    *((uint32_t*)(buf+28)) = 0x00000034; /* e_phoff                 */
    *((uint16_t*)(buf+40)) = 0x0034;     /* e_ehsize                */
    *((uint16_t*)(buf+42)) = 0x0020;     /* e_phentsize             */
    *((uint16_t*)(buf+44)) = e_phnum;    /* e_phnum                 */

    status = hal_flash_write_wrap(crash_context_base + offset, buf, 52);
    (void)status;
    /* printf("write coredump header: offset=%d, length=52, status=%d\n\r", offset, status); */

    return 52;
}

static unsigned int coredump_pt_note(unsigned int offset, int e_phnum)
{
    uint8_t buf[32] = {0};
    hal_flash_status_t status;
    uint32_t crash_context_base = 0;
    crash_context_base =  core_get_crash_address();
	
    *((uint32_t*)(buf+0))  = 0x00000004; /* p_type = PT_NOTE  */
    *((uint32_t*)(buf+4))  = 52 + e_phnum*32;  /* p_offset */

    /* NT_PRPSINFO: (20 + 124)
     * NT_PRSTATUS: (20 + 148)
     */
    *((uint32_t*)(buf+16)) = 0x00000138;       /* p_filesz                */

    status = hal_flash_write_wrap(crash_context_base + offset, buf, 32);
    (void)status;
    /* printf("write coredump pt_note: offset=%d, length=32, status=%d\n\r", offset, status); */

    return 32;
}

static unsigned int coredump_pt_load(unsigned int offset, memory_region_type *load_regions, int entries)
{
    uint8_t buf[MAX_MEMORY_REGIONS * 32] = {0};
    unsigned int p_offset, p_addr, p_size, i;
    uint8_t *p;
    hal_flash_status_t status;
    uint32_t crash_context_base = 0;
    crash_context_base =  core_get_crash_address();
	
    p_offset =  52                     /* ELF Header */
                + (1 + entries) * 32   /* Program header table */
                + (20 + 124)           /* NT_PRPSINFO */
                + (20 + 148);          /* NT_PRSTATUS */

    p = buf;

    for(i = 0; i < entries; i++) {
        *((uint32_t*)(p+0)) = 0x00000001; /* p_type = PT_LOAD */
        *((uint32_t*)(p+4)) = p_offset;   /* p_offset */

        p_addr = (uint32_t)load_regions[i].start_address;
        *((uint32_t*)(p+8))  = p_addr; /* p_vaddr */
        *((uint32_t*)(p+12)) = p_addr; /* p_paddr */

        p_size = (uint32_t)load_regions[i].end_address - (uint32_t)load_regions[i].start_address;
        *((uint32_t*)(p+16)) = p_size; /* p_filesz */
        *((uint32_t*)(p+20)) = p_size; /* p_memsz  */

        *((uint32_t*)(p+24)) = 0x00000007; /* p_flags = RWX  */

        p_offset += p_size;
        p = p + 32;
    }

    status = hal_flash_write_wrap(crash_context_base + offset, buf, entries * 32);
    (void)status;
    /* printf("write coredump pt_load: offset=%d, length=%d, status=%d\n\r", offset, entries*32, status); */

    return (entries * 32);
}

__weak void *xTaskGetCurrentTaskHandle( void )
{
    return 0;
}

static unsigned int coredump_note(unsigned int offset, TaskContext *pTaskContext)
{
    uint8_t buf[320] = {0}; /* (20 + 124) + (20 + 148) + 8 (reserved) */
    uint8_t *p;
    hal_flash_status_t status;
     uint32_t crash_context_base = 0;
    crash_context_base =  core_get_crash_address();
	
    /* Fill NT_PRPSINFO */
    p = buf;
    *((uint32_t*)(p+0)) = 0x00000005;  /* namesz                 */
    *((uint32_t*)(p+4)) = 0x0000007c;  /* descsz                 */
    *((uint32_t*)(p+8)) = 0x00000003;  /* type = 3 (NT_PRPSINFO) */
    p[12]  = 'C';                      /* name                   */
    p[13]  = 'O';
    p[14]  = 'R';
    p[15]  = 'E';
    *((uint32_t*)(p+16)) = 0x00000000; /* 0 terminator + padding */

    p = p + 20;

    /* descriptor (struct elf_prpsifo) */
    p[1]  = 'R';                       /* pr_sname = 'R'         */
    *((uint16_t*)(p+8))  = 0x0001;     /* pr_uid                 */
    *((uint16_t*)(p+10)) = 0x0001;     /* pr_uid                 */
    p[28]  = 'f';                      /* pr_fname               */
    p[29]  = 'r';
    p[30]  = 'e';
    p[31]  = 'e';
    p[32]  = 'r';
    p[33]  = 't';
    p[34]  = 'o';
    p[35]  = 's';
    p[36]  = '8';
    p[37]  =  0 ;
    p[44]  = 'f';                      /* pr_psargs              */
    p[45]  = 'r';
    p[46]  = 'e';
    p[47]  = 'e';
    p[48]  = 'r';
    p[49]  = 't';
    p[50]  = 'o';
    p[51]  = 's';
    p[52]  = '8';
    p[53]  =  0 ;

    p = p + 124;                  /* sizeof(struct elf_prpsinfo) */

    /* Fill NT_PRSTATUS */
    *((uint32_t*)(p+0)) = 0x00000005;  /* namesz                 */
    *((uint32_t*)(p+4)) = 0x00000094;  /* descsz                 */
    *((uint32_t*)(p+8)) = 0x00000001;  /* type = 1 (NT_PRSTATUS) */
    p[12]  = 'C';                      /* name                   */
    p[13]  = 'P';
    p[14]  = 'U';
    p[15]  = '0';

    p = p + 20;

    /* descriptor (struct elf_prstatus) */
    *((uint32_t*)(p+24)) = (uint32_t)xTaskGetCurrentTaskHandle(); /* pr_pid */

    p = p + 72; /* 72 = offset of 'pr_reg' in 'struct elf_prstatus' */

    /* elf_prstatus.pr_reg */
    *((unsigned int*)(p+0))  = pTaskContext->r0;
    *((unsigned int*)(p+4))  = pTaskContext->r1;
    *((unsigned int*)(p+8))  = pTaskContext->r2;
    *((unsigned int*)(p+12)) = pTaskContext->r3;
    *((unsigned int*)(p+16)) = pTaskContext->r4;
    *((unsigned int*)(p+20)) = pTaskContext->r5;
    *((unsigned int*)(p+24)) = pTaskContext->r6;
    *((unsigned int*)(p+28)) = pTaskContext->r7;
    *((unsigned int*)(p+32)) = pTaskContext->r8;
    *((unsigned int*)(p+36)) = pTaskContext->r9;
    *((unsigned int*)(p+40)) = pTaskContext->r10;
    *((unsigned int*)(p+44)) = pTaskContext->r11;
    *((unsigned int*)(p+48)) = pTaskContext->r12;
    *((unsigned int*)(p+52)) = pTaskContext->sp;
    *((unsigned int*)(p+56)) = pTaskContext->lr;
    *((unsigned int*)(p+60)) = pTaskContext->pc;
    *((unsigned int*)(p+64)) = pTaskContext->psr;
    *((unsigned int*)(p+68)) = 0; /* ORIG_r0 */

    status = hal_flash_write_wrap(crash_context_base + offset, buf, 312);
    (void)status;
    /* printf("write coredump note: offset=%d, length=%d, status=%d\n\r", offset, 312, status); */

    return 312;
}

static void coredump_load(uint32_t offset, uint8_t *address, uint32_t len)
{
    hal_flash_status_t status;
	uint32_t crash_context_base = 0;
    crash_context_base =  core_get_crash_address();

    /* Write 4KB at one time. Data may change while dumping self. Reduce the data loss. */
    while (len) {

        if (len < 0x1000) {
            /* write the last 4KB byte data */
            status = hal_flash_write_wrap(crash_context_base + offset, address, len);
            (void)status;
            /* printf("write coredump_load: address=0x%08x, status=%d\n\r", (unsigned int)address, status); */
            break;
        }

        status = hal_flash_write_wrap(crash_context_base + offset, address, 0x1000);
        (void)status;
        /* printf("write coredump_load: address=0x%08x, status=%d\n\r", (unsigned int)address, status); */
        offset  += 0x1000;
        address += 0x1000;
        len     -= 0x1000;
    }
}


static void coredump(TaskContext *pTaskContext, memory_region_type *memory_regions)
{
    unsigned int entries, offset, i; 
    uint32_t len;
    hal_flash_status_t status;
    uint32_t crash_context_base = 0;
	uint32_t crash_context_length = 0;

 crash_context_length = core_get_crash_size();
  crash_context_base =  core_get_crash_address();
    /* erase the flash blocks reserved for coredump */
    for (offset = 0; offset < crash_context_length; offset += 0x1000) {
        status  = hal_flash_erase_wrap(crash_context_base + offset, HAL_FLASH_BLOCK_4K);
        (void)status;
        /* printf("coredump erase flash: offset=0x%08x, status=%d\n\r", offset, status); */
    }

    entries = sort_memory_regions(load_regions);

    offset = 0;
    offset += coredump_elf_hdr(offset, entries+1); /* 1 note + 'entries' load */
    offset += coredump_pt_note(offset, entries+1);
    offset += coredump_pt_load(offset, load_regions, entries);
    offset += coredump_note(offset, pTaskContext);

    /* memory data starts from (coredump_header + offset) */
    for (i = 0; i < entries; i++) {
        len = (uint32_t)load_regions[i].end_address - (uint32_t)load_regions[i].start_address;
        /* printf("coredump write load region %d\n\r", i); */
        coredump_load(offset, (uint8_t*)load_regions[i].start_address, len);
        offset += len;
    }
}

#endif /* MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE */

__weak void exception_reboot(void)
{
    /* It is defined as a weak function.
     * It needs to be implemented in project.
     * The default behvior is NOP, and the memory dump continues.
     */
    return;
}

static bool reboot_check(void)
{
    return (reboot_flag == DISABLE_MEMDUMP_MAGIC);
}

#define EXT_FLASH_READ_SIZE 4

uint32_t context_base = 0x0;
uint8_t context_buffer[EXT_FLASH_READ_SIZE] ={0};
uint32_t context_count = 0;

uint32_t flash_index =0;

void stackDump(uint32_t stack[])
{
    taskContext.r0   = stack[r0];
    taskContext.r1   = stack[r1];
    taskContext.r2   = stack[r2];
    taskContext.r3   = stack[r3];
    taskContext.r12  = stack[r12];
    taskContext.sp   = ((uint32_t)stack) + 0x20;
    taskContext.lr   = stack[lr];
    taskContext.pc   = stack[pc];
    taskContext.psr  = stack[psr];

    /* FPU context? */
    if ( (taskContext.exc_return & 0x10) == 0 ) {
        taskContext.s0 = stack[s0];
        taskContext.s1 = stack[s1];
        taskContext.s2 = stack[s2];
        taskContext.s3 = stack[s3];
        taskContext.s4 = stack[s4];
        taskContext.s5 = stack[s5];
        taskContext.s6 = stack[s6];
        taskContext.s7 = stack[s7];
        taskContext.s8 = stack[s8];
        taskContext.s9 = stack[s9];
        taskContext.s10 = stack[s10];
        taskContext.s11 = stack[s11];
        taskContext.s12 = stack[s12];
        taskContext.s13 = stack[s13];
        taskContext.s14 = stack[s14];
        taskContext.s15 = stack[s15];
        taskContext.fpscr = stack[fpscr];
        taskContext.sp += 72; /* s0-s15, fpsr, reserved */
    }

    /* if CCR.STKALIGN=1, check PSR[9] to know if there is forced stack alignment */
    if ( (SCB->CCR & SCB_CCR_STKALIGN_Msk) && (taskContext.psr & 0x200)) {
        taskContext.sp += 4;
    }

    printf("r0  = 0x%08x\n\r", taskContext.r0);
    printf("r1  = 0x%08x\n\r", taskContext.r1);
    printf("r2  = 0x%08x\n\r", taskContext.r2);
    printf("r3  = 0x%08x\n\r", taskContext.r3);
    printf("r4  = 0x%08x\n\r", taskContext.r4);
    printf("r5  = 0x%08x\n\r", taskContext.r5);
    printf("r6  = 0x%08x\n\r", taskContext.r6);
    printf("r7  = 0x%08x\n\r", taskContext.r7);
    printf("r8  = 0x%08x\n\r", taskContext.r8);
    printf("r9  = 0x%08x\n\r", taskContext.r9);
    printf("r10 = 0x%08x\n\r", taskContext.r10);
    printf("r11 = 0x%08x\n\r", taskContext.r11);
    printf("r12 = 0x%08x\n\r", taskContext.r12);
    printf("lr  = 0x%08x\n\r", taskContext.lr);
    printf("pc  = 0x%08x\n\r", taskContext.pc);
    printf("psr = 0x%08x\n\r", taskContext.psr);
    printf("EXC_RET = 0x%08x\n\r", taskContext.exc_return);

    /* update CONTROL.SPSEL and psp if returning to thread mode */
    if (taskContext.exc_return & 0x4) {
        taskContext.control |= 0x2; /* CONTROL.SPSel */
        taskContext.psp = taskContext.sp;
    } else { /* update msp if returning to handler mode */
        taskContext.msp = taskContext.sp;
    }

    /* FPU context? */
    if ( (taskContext.exc_return & 0x10) == 0 ) {
        taskContext.control |= 0x4; /* CONTROL.FPCA */
        printf("s0  = 0x%08x\n\r", taskContext.s0);
        printf("s1  = 0x%08x\n\r", taskContext.s1);
        printf("s2  = 0x%08x\n\r", taskContext.s2);
        printf("s3  = 0x%08x\n\r", taskContext.s3);
        printf("s4  = 0x%08x\n\r", taskContext.s4);
        printf("s5  = 0x%08x\n\r", taskContext.s5);
        printf("s6  = 0x%08x\n\r", taskContext.s6);
        printf("s7  = 0x%08x\n\r", taskContext.s7);
        printf("s8  = 0x%08x\n\r", taskContext.s8);
        printf("s9  = 0x%08x\n\r", taskContext.s9);
        printf("s10 = 0x%08x\n\r", taskContext.s10);
        printf("s11 = 0x%08x\n\r", taskContext.s11);
        printf("s12 = 0x%08x\n\r", taskContext.s12);
        printf("s13 = 0x%08x\n\r", taskContext.s13);
        printf("s14 = 0x%08x\n\r", taskContext.s14);
        printf("s15 = 0x%08x\n\r", taskContext.s15);
        printf("s16 = 0x%08x\n\r", taskContext.s16);
        printf("s17 = 0x%08x\n\r", taskContext.s17);
        printf("s18 = 0x%08x\n\r", taskContext.s18);
        printf("s19 = 0x%08x\n\r", taskContext.s19);
        printf("s20 = 0x%08x\n\r", taskContext.s20);
        printf("s21 = 0x%08x\n\r", taskContext.s21);
        printf("s22 = 0x%08x\n\r", taskContext.s22);
        printf("s23 = 0x%08x\n\r", taskContext.s23);
        printf("s24 = 0x%08x\n\r", taskContext.s24);
        printf("s25 = 0x%08x\n\r", taskContext.s25);
        printf("s26 = 0x%08x\n\r", taskContext.s26);
        printf("s27 = 0x%08x\n\r", taskContext.s27);
        printf("s28 = 0x%08x\n\r", taskContext.s28);
        printf("s29 = 0x%08x\n\r", taskContext.s29);
        printf("s30 = 0x%08x\n\r", taskContext.s30);
        printf("s31 = 0x%08x\n\r", taskContext.s31);
        printf("fpscr = 0x%08x\n\r", taskContext.fpscr);
    }

    printf("CONTROL = 0x%08x\n\r", taskContext.control);
    printf("MSP     = 0x%08x\n\r", taskContext.msp);
    printf("PSP     = 0x%08x\n\r", taskContext.psp);
    printf("sp      = 0x%08x\n\r", taskContext.sp);

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)

 context_base =  core_get_crash_address();

 if (do_write_to_ext_flash()==true) {
 #if defined(SERIAL_NOR_WITH_SPI_SUPPORT)
	 crash_ext_flash_init(); 
 #endif
	 }

 if(is_crash_dump_save_to_flash()==true) {
    coredump(pTaskContext, memory_regions);

	for (flash_index =0; flash_index <512; flash_index++) {
	for(context_count =0; context_count <EXT_FLASH_READ_SIZE;context_count++) {
		hal_flash_read_wrap(flash_index*4+context_count,context_buffer,1);
        printf(" \r\n %x \r\n", context_buffer[0]);
 	}
		}
 	}

 
 if (do_write_to_ext_flash()==true) {
	 #if defined(SERIAL_NOR_WITH_SPI_SUPPORT)
	 crash_ext_flash_deinit();	 
	 #endif
	 }

 
#endif
reboot_check();

}

void memoryDumpAll(void)
{
    unsigned int *current, *end;
    unsigned int i;

    for (i = 0; ; i++) {

        if ( !memory_regions[i].region_name ) {
           break;
        }

        if ( !memory_regions[i].is_dumped ) {
           continue;
        }

        current = memory_regions[i].start_address;
        end     = memory_regions[i].end_address;

        for (; current < end; current += 4) {

            if (*(current + 0) == 0 &&
                *(current + 1) == 0 &&
                *(current + 2) == 0 &&
                *(current + 3) == 0 ) {
                    continue;
            }

            printf("0x%08x: %08x %08x %08x %08x\n\r",
                   (unsigned int)current,
                   *(current + 0),
                   *(current + 1),
                   *(current + 2),
                   *(current + 3));
         }
    }

    for (i = 0; i < exception_config.items; i++) {
        if (exception_config.configs[i].dump_cb) {
            exception_config.configs[i].dump_cb();
        }
    }

    printf("\n\rmemory dump completed.\n\r");

	#if defined(MTK_AUDIO_I2S_TEST)	
	printf("\n\ DMA_CM4_GLBSTA 0  is %x.\n\r",*((volatile uint32_t*)(0x83010000)));
	printf("\n\ DMA_CM4_GLBSTA 1  is %x.\n\r",*((volatile uint32_t*)(0x83010004)));

    for (i=0; i<40; i++) 
    {
    printf ("\n\r the address %x value is %x \n\r",(0x83010C10+i*4), *((volatile uint32_t*)(0x83010C10+i*4)));
	}
	#endif

#if (CHECK_EXCEPTION_STACK_USAGE == 1)
    for (i = 0; i < EXCEPTION_STACK_WORDS; i++) {
        if (xExceptionStack[i] != 0xa5a5a5a5) {
            break;
        }
    }
    printf("\n\rException Stack: used = %d, left = %d\n\r", (EXCEPTION_STACK_WORDS - i)*4, i*4);
#endif

}

/*
 * Debug scenarios:
 *
 * (1) debug with debugger, stop in first exception.
 *     Print the exception context, and halt cpu.
 *
 *     DEBUGGER_ON: 1
 *
 * (2) debug with uart, stop in first exception.
 *     Print the exception context, and enter an infinite loop.
 *
 *     DEBUGGER_ON: 0
 */

#define DEBUGGER_ON    0

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void Hard_Fault_Handler(uint32_t stack[])
{
    printf("\n\rIn Hard Fault Handler\n\r");
    printf("SCB->HFSR = 0x%08x\n\r", (unsigned int)SCB->HFSR);
    if ((SCB->HFSR & (1 << 30)) != 0) {
        printf("Forced Hard Fault\n\r");
        printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
        if ((SCB->CFSR & 0xFFFF0000) != 0) {
            printUsageErrorMsg(SCB->CFSR);
        }
        if ((SCB->CFSR & 0x0000FF00) != 0 ) {
            printBusFaultErrorMsg(SCB->CFSR);
        }
        if ((SCB->CFSR & 0x000000FF) != 0 ) {
            printMemoryManagementErrorMsg(SCB->CFSR);
        }
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    while (1);
#endif
}

void MemManage_Fault_Handler(uint32_t stack[])
{
    printf("\n\rIn MemManage Fault Handler\n\r");
    printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
    if ((SCB->CFSR & 0xFF) != 0) {
        printMemoryManagementErrorMsg(SCB->CFSR);
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    while (1);
#endif
}

void Bus_Fault_Handler(uint32_t stack[])
{
    printf("\n\rIn Bus Fault Handler\n\r");
    printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR );
    if ((SCB->CFSR & 0xFF00) != 0) {
        printBusFaultErrorMsg(SCB->CFSR);
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    while (1);
#endif
}

void Usage_Fault_Handler(uint32_t stack[])
{
    printf("\n\rIn Usage Fault Handler\n\r");
    printf("SCB->CFSR = 0x%08x\n\r", (unsigned int)SCB->CFSR);
    if ((SCB->CFSR & 0xFFFF0000) != 0) {
        printUsageErrorMsg(SCB->CFSR);
    }

    stackDump(stack);

    memoryDumpAll();

#if DEBUGGER_ON
    __ASM volatile("BKPT #01");
#else
    while (1);
#endif
}

/******************************************************************************/
/*                   Toolchain Dependent Part                                 */
/******************************************************************************/
#if defined(__GNUC__)

#if (PRODUCT_VERSION == 2523) || (PRODUCT_VERSION == 2533)
#define __EXHDLR_ATTR__   __attribute__((naked, section(".ram_code")))
#endif

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)
#define __EXHDLR_ATTR__   __attribute__((naked))
#endif

/**
  * @brief  This function is the common part of exception handlers.
  * @param  r3 holds EXC_RETURN value
  * @retval None
  */
__EXHDLR_ATTR__ void CommonFault_Handler(void)
{
    __asm volatile
    (
        "cpsid i                       \n"     /* disable irq                 */
        "ldr r3, =pxExceptionStack     \n"
        "ldr r3, [r3]                  \n"     /* r3 := pxExceptionStack      */
        "ldr r0, =pTaskContext         \n"
        "ldr r0, [r0]                  \n"     /* r0 := pTaskContext          */
        "add r0, r0, #16               \n"     /* point to context.r4         */
        "stmia r0!, {r4-r11}           \n"     /* store r4-r11                */
        "mov r5, r12                   \n"     /* r5 := EXC_RETURN            */
        "add r0, r0, #20               \n"     /* point to context.control    */
        "mrs r1, control               \n"     /* move CONTROL to r1          */
        "str r1, [r0], #4              \n"     /* store CONTROL               */
        "str r5, [r0], #4              \n"     /* store EXC_RETURN            */
        "mrs r4, msp                   \n"     /* r4 := MSP                   */
        "str r4, [r0], #4              \n"     /* store MSP                   */
        "mrs r1, psp                   \n"     /* move PSP to r1              */
        "str r1, [r0]                  \n"     /* store PSP                   */
        "tst r5, #0x10                 \n"     /* FPU context?                */
        "itt eq                        \n"
        "addeq r0, r0, #68             \n"     /* point to contex.s16         */
        "vstmeq r0, {s16-s31}          \n"     /* store r16-r31               */
        "cmp r3, #0                    \n"     /* if (!pxExceptionStack)      */
        "it ne                         \n"
        "movne sp, r3                  \n"     /* update msp                  */
        "push {lr}                     \n"
        "bl exception_init             \n"
        "pop {lr}                      \n"
        "tst r5, #4                    \n"     /* thread or handler mode?     */
        "ite eq                        \n"
        "moveq r0, r4                  \n"
        "mrsne r0, psp                 \n"
        "bx lr                         \n"
    );
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void HardFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Hard_Fault_Handler         \n"
    );
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void MemManage_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl MemManage_Fault_Handler    \n"
    );
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void BusFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Bus_Fault_Handler          \n"
    );
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void UsageFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Usage_Fault_Handler        \n"
    );
}

#endif /* __GNUC__ */

#if defined (__CC_ARM)

#if (PRODUCT_VERSION == 2523) || (PRODUCT_VERSION == 2533)
#define __EXHDLR_ATTR__   __asm __attribute__((section(".ram_code")))
#endif

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)
#define __EXHDLR_ATTR__ __asm
#endif

/**
  * @brief  This function is the common part of exception handlers.
  * @param  r3 holds EXC_RETURN value
  * @retval None
  */
__EXHDLR_ATTR__ void CommonFault_Handler(void)
{
    extern pTaskContext
    extern pxExceptionStack

    PRESERVE8

    cpsid i                       /* disable irq                  */
    ldr r3, =pxExceptionStack
    ldr r3, [r3]                  /* r3 := pxExceptionStack       */
    ldr r0, =pTaskContext
    ldr r0, [r0]                  /* r0 := pTaskContext           */
    add r0, r0, #16               /* point to context.r4          */
    stmia r0!, {r4-r11}           /* store r4-r11                 */
    mov r5, r12                   /* r5 := EXC_RETURN             */
    add r0, r0, #20               /* point to context.control     */
    mrs r1, control               /* move CONTROL to r1           */
    str r1, [r0], #4              /* store CONTROL                */
    str r5, [r0], #4              /* store EXC_RETURN             */
    mrs r4, msp                   /* r4 := MSP                    */
    str r4, [r0], #4              /* store MSP                    */
    mrs r1, psp                   /* move PSP to r1               */
    str r1, [r0]                  /* store PSP                    */
    tst r5, #0x10                 /* FPU context?                 */
    itt eq
    addeq r0, r0, #68             /* point to contex.s16          */
    vstmeq r0, {s16-s31}          /* store r16-r31                */
    cmp r3, #0                    /* if (!pxExceptionStack)       */
    it ne
    movne sp, r3                  /* update msp                   */
    push {lr}
    bl __cpp(exception_init)
    pop  {lr}
    tst r5, #4                    /* thread or handler mode?      */
    ite eq
    moveq r0, r4
    mrsne r0, psp
    bx lr
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void HardFault_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    bl __cpp(Hard_Fault_Handler)
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void MemManage_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    bl __cpp(MemManage_Fault_Handler)
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void BusFault_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    bl __cpp(Bus_Fault_Handler)
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void UsageFault_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    bl __cpp(Usage_Fault_Handler)
}

#endif /* __CC_ARM */

#if defined (__ICCARM__)

#define __EXHDLR_ATTR__  __stackless

#pragma location=".ram_code"
TaskContext* get_pTaskContext(void);

#pragma location=".ram_code"
void CommonFault_Handler(void);

#pragma location=".ram_code"
void HardFault_Handler(void);

#pragma location=".ram_code"
void MemManage_Handler(void);

#pragma location=".ram_code"
void BusFault_Handler(void);

#pragma location=".ram_code"
void UsageFault_Handler(void);

/**
  * @brief  This function is the common part of exception handlers.
  * @param  r3 holds EXC_RETURN value
  * @retval None
  */
__EXHDLR_ATTR__ void CommonFault_Handler(void)
{
    __asm volatile
    (
        "cpsid i                       \n"     /* disable irq                 */
        "mov r3, %0                    \n"     /* r3 := pxExceptionStack      */
        "mov r0, %1                    \n"     /* r0 := pTaskContext          */
        "add r0, r0, #16               \n"     /* point to context.r4         */
        "stmia r0!, {r4-r11}           \n"     /* store r4-r11                */
        "mov r5, r12                   \n"     /* r5 := EXC_RETURN            */
        "add r0, r0, #20               \n"     /* point to context.control    */
        "mrs r1, control               \n"     /* move CONTROL to r1          */
        "str r1, [r0], #4              \n"     /* store CONTROL               */
        "str r5, [r0], #4              \n"     /* store EXC_RETURN            */
        "mrs r4, msp                   \n"     /* r4 := MSP                   */
        "str r4, [r0], #4              \n"     /* store MSP                   */
        "mrs r1, psp                   \n"     /* move PSP to r1              */
        "str r1, [r0]                  \n"     /* store PSP                   */
        "tst r5, #0x10                 \n"     /* FPU context?                */
        "itt eq                        \n"
        "addeq r0, r0, #68             \n"     /* point to contex.s16         */
        "vstmeq r0, {s16-s31}          \n"     /* store r16-r31               */
        "cmp r3, #0                    \n"     /* if (!pxExceptionStack)      */
        "it ne                         \n"
        "movne sp, r3                  \n"     /* update msp                  */
        "push {lr}                     \n"
        "bl exception_init             \n"
        "pop {lr}                      \n"
        "tst r5, #4                    \n"     /* thread or handler mode?     */
        "ite eq                        \n"
        "moveq r0, r4                  \n"
        "mrsne r0, psp                 \n"
        "bx lr                         \n"
        ::"r"(pxExceptionStack), "r"(pTaskContext)
    );
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void HardFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Hard_Fault_Handler         \n"
    );
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void MemManage_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl MemManage_Fault_Handler    \n"
    );
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void BusFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Bus_Fault_Handler          \n"
    );
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void UsageFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "bl Usage_Fault_Handler        \n"
    );
}

#endif /* __ICCARM__ */

