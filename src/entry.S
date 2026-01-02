.section .rodata
test_msg:
    .asciz "hello from entry.S\n"

.section .text.init
.global _start, kstack_trampoline

_start:
    # Setup initial stack pointer 'boot_stack' from kernel.ld
    la sp, boot_stack

    # Clear .bss: [bss_start, bss_end)
    la t0, bss_start
    la t1, bss_end

clear_bss:
    bgeu t0, t1, run_kernel
    sd zero, 0(t0)
    addi t0, t0, 8
    j clear_bss

run_kernel:
    call kernel_main

hang:
    wfi
    j hang

kstack_trampoline:
    mv sp, a0
    jr a1