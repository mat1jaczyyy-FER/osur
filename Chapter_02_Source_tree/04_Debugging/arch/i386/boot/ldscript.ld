/*! linker script for memory layout */

/* Its parsed as C before used in linking! */

ENTRY(arch_startup)

SECTIONS {
	.code LOAD_ADDR:
	{
		kernel_code_addr = .;

		/* instructions */
		* ( .text* )
	}

	.rodata :
	{
		kernel_rodata_addr = .;

		/* read only data (constants) */
		* ( .rodata* )
	}

	.data DATA_ADDR:
	{
		kernel_data_addr = .;

		/* initialized global variables */
		* ( .data* )
	}
	size_data = SIZEOF(.data);

	.bss :
	{
		*( .bss* COMMON* )
		. = ALIGN (4096);
	}
	size_bss = SIZEOF(.bss);

	.stog_u_slici STACK_ADDR:
	{
		* ( .stog* )
		. = ALIGN (4096);
	}

	/DISCARD/ : { *(.eh_frame) }
	
	/*
	 * what with other sections generated with gcc (various versions)?
	 * if optimizing for size discard them with: /DISCARD/ : { *(*) }
	 * if debugging with qemu/gdb they must be included
	 * - they can be included implicitly, without declaring them here
	 * - but they will be put behind "kernel_end_addr" so its best to use
	 *   multiboot loader (QEMU is that)
	 */

#ifndef DEBUG
	/DISCARD/ : { *(*) }
#endif
	kernel_end_addr = .;
}
