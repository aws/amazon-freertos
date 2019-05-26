/*
 * Copyright (C) 2008-2018, Marvell International Ltd.
 * All Rights Reserved.
 *
 * The same linkers script is used for both C and C++. C++ specific sections
 * are harmless for C linking.
 */

/* Entry Point */
ENTRY(Reset_IRQHandler)

/* Start address of main stack pointer
 * Note: Stack grows towards lower addresses.
 */
_estack = 0x20020000;    /* end of 128K SRAM1 */

/* Minimum required heap size in bytes */
_heap_size = (72 * 1024);

/* Flash controller memory mapped start address */
_flashc_mem_start = 0x1f000000;

/* Size of .data section for libs in ROM, this must start at ORIGIN(SRAM1) */
EXTERN(_rom_data)

MEMORY
{
	SRAM0 (rwx)  : ORIGIN = 0x00100000, LENGTH = 384K
	SRAM1 (rwx)  : ORIGIN = 0x20000000, LENGTH = 128K
	NVRAM (rw)   : ORIGIN = 0x480C0000, LENGTH = 4K
	FLASHC (rw)  : ORIGIN = 0x20020000, LENGTH = 32K
}

/* The size of the main stack that is used for interrupt processing. */
_main_stack_size = 0x800;
/* Start of main stack*/
_main_stack_start = ORIGIN(SRAM1) + LENGTH(SRAM1) - _main_stack_size;

/* Keystore size in RAM, SB_KEYSTORE_SIZE from secure_boot.h  */
_keystore_size = 0x1000;
/* Start of keystore */
_keystore_start = ORIGIN(SRAM0) + LENGTH(SRAM0) - _keystore_size;

SECTIONS
{
	.text :
	{
		. = ALIGN(512);
		KEEP(*(.isr_vector))
		. = ALIGN(4);

		*(.text.Reset_IRQHandler)
		*(.text .text.* .gnu.linkonce.t.*)
		*(.ram .ram.*)
		*(.rodata .rodata.* .gnu.linkonce.r.*)
		. = ALIGN(4);
		/* C++: DWARF Exception Header. */
		*(.eh_frame_hdr)
		/* C++: Exception Frames */
		*(.eh_frame)

		. = ALIGN(4);
		KEEP(*(.init))

		. = ALIGN(4);
		/* Constructor and Destructor startup and tear down code. */
		__preinit_array_start = .;
		KEEP (*(.preinit_array))
		__preinit_array_end = .;

		. = ALIGN(4);
		__init_array_start = .;
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array))
		__init_array_end = .;

		. = ALIGN(0x4);
		KEEP (*crtbegin.o(.ctors))
		KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*crtend.o(.ctors))

		. = ALIGN(4);
		KEEP(*(.fini))

		. = ALIGN(4);
		__fini_array_start = .;
		KEEP (*(.fini_array))
		KEEP (*(SORT(.fini_array.*)))
		__fini_array_end = .;

		KEEP (*crtbegin.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*crtend.o(.dtors))

		. = ALIGN(8);
		_etext = .;
	} > SRAM0

	/* Contains information for unwinding the stack */
	.ARM.exidx (_etext)   :
	 {
		. = ALIGN(4);
		__exidx_start = .;
		 *(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	 }

	/* Contains exception unwinding information */
	 .ARM.extab (__exidx_end) :
         {
             . = ALIGN(4);
	     *(.ARM.extab* .gnu.linkonce.armextab.*)
	 }

	/* move  bss of specific libraries
	 * to SRAM0 to mnake space in SRAM1
	 */
	.bss_sram0 (NOLOAD):
	{
		_bss_sram0 = .;
		*libwifidriver.a:(.bss .bss.*)
		. = ALIGN(4);
		_ebss_sram0 = .;
	} > SRAM0
	/* Heap Section. */
	. = ALIGN(4);
	_heap_start = .;
	. = _keystore_start;
	_heap_end = .;
	PROVIDE (end = .);

	.keystore _keystore_start :
	{
		. = . + _keystore_size;
	}

	.rom_data (NOLOAD) :
	{
		_rom_data_start = .;
		. = . + _rom_data;
	} > SRAM1

	ASSERT( _rom_data_start == ORIGIN(SRAM1), "Invalid start address of .rom_data")

	.data :
	{
		_data = .;
		*(.data)
		*(.data.*)
		_edata = .;
	} > SRAM1

	.bss_sram1 (NOLOAD):
	{
		_bss_sram1 = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		_ebss_sram1 = .;
	} > SRAM1

	/* Assure min. required size for main stack in SRAM1 */
	ASSERT(_ebss_sram1 < _main_stack_start, "Insufficient space for main stack.")

	/* Add the free space in SRAM1 to our heap */
	_heap_2_start = .;
	. = _main_stack_start;
	_heap_2_end = .;

	/* The main stack */
        ._main_stack _main_stack_start :
        {
                . = ALIGN(4);
                . = . + _main_stack_size;
                . = ALIGN(4);
        }

	_actual_heap_size = _heap_end - _heap_start + _heap_2_end - _heap_2_start;
	 ASSERT(_actual_heap_size >= _heap_size, "Insufficient space left for heap.")

	.nvram (NOLOAD):
	{
		/* BootROM uses first few bytes of retention ram */
		_nvram_start = .;
		. = . + 64;
		. = ALIGN(4);
		/* Zero initialized on bootup */
		_nvram_begin = .;
		*(.nvram)
		*(.nvram.*)
		_nvram_end = .;
		/* Un-initialized nvram section */
		. = ALIGN(4);
		*(.nvram_uninit)
		*(.nvram_uninit.*)
	} > NVRAM

	/DISCARD/ :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	}
}
