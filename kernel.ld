OUTPUT_ARCH( "riscv" )
ENTRY( _start )

SECTIONS
{
  /* QEMU loads us at 0x80200000 */
  . = 0x80200000;

  .text : {
    *(.text.init) /* The entry point goes first */
    *(.text)      /* The rest of the code */
  }

  .data : {
    *(.data)
  }

  .bss : {
    . = ALIGN(16);
    bss_start = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(16);
    bss_end = .;
  }

  /* 
     Allocate 4KB (4096 bytes) for the boot stack.
     The stack grows DOWN, so we need the address of the TOP.
  */
  . = ALIGN(16);
  . += 4096;
  boot_stack = .;

  /*
    Align to 4096 pages for rest of memory
  */
  . = ALIGN(4096);
  end = .;
}