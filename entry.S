.section .text.init
.global _start

_start:
    # 1. Set up the Stack Pointer
    # We load the address 'stack_top' (from kernel.ld) into register sp
    la sp, stack_top

    # 2. Jump to C
    # We call the C function 'kernel_main'
    call kernel_main

    # 3. Safety Loop
    # If C ever returns (it shouldn't), we spin here forever.
    tail .