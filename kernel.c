#include "uart.h"
#include "trap.h"
#include <stdint.h>

void user_print(void) {
    register uint64_t a7 asm("a7") = 0;  // syscall 0
    register const char* a0 asm("a0") = "Hello from U-mode!\n";

    asm volatile("ecall" : : "r"(a7), "r"(a0));

    // Loop forever after syscall
    while (1) {}
}

void user_read(void) {
    size_t len = 64; 
    char buf[len];

    register uint64_t a7 asm("a7") = 2;  // syscall 0
    register const char* a0 asm("a0") = buf;
    register const size_t a1 asm("a1") = len;
    register size_t ret asm("a0");

    asm volatile("ecall" : "=r"(ret) : "r"(a7), "r"(a0), "r"(a1));

    // Loop forever after syscall
    while (1) {}
}

void drop_to_u_and_call(uintptr_t progamm_addr) {
    // Clear S mode bit in sstatus so sret drops to U-mode
    uintptr_t mask = 1UL << 8;
    asm volatile("csrrc zero, sstatus, %0" : : "r"(mask));

    // Set sepc
    asm volatile("csrrw zero, sepc, %0" : : "r"(progamm_addr));
    asm volatile("sret");
}

void kernel_main(void) {
    uart_puts("\n");
    uart_puts("==================================\n");

    // Install trap handler
    trap_init();
    uart_puts("kernel: trap handler installed\n");

    // End kernel init
    uart_puts("==================================\n\n");

    drop_to_u_and_call((uintptr_t) user_read);

    // while (1) {}

}