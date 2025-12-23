.section .text.init
.global _start

_start:
    # Setup stack pointer 'stack_top' from kernel.ld
    la sp, stack_top

    # Clear .bss: [bss_start, bss_end)
    la t0, bss_start
    la t1, bss_end
1:
    bgeu t0, t1, 2f
    sd zero, 0(t0)
    addi t0, t0, 8
    j 1b
2:
    call kernel_main
    tail .