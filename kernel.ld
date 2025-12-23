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
    *(.bss)
  }

  /* 
     Allocate 4KB (4096 bytes) for the stack.
     The stack grows DOWN, so we need the address of the TOP.
  */
  . = ALIGN(16);
  . += 4096;
  stack_top = .;
}