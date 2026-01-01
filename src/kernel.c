#include "uart.h"
#include "trap.h"
#include "lib.h"
#include "syscall.h"
#include "kalloc.h"
#include "proc.h"
#include <stdint.h>

void user_write(void) {
    register uint64_t a7 asm("a7") = SYS_write;
    register const char* a0 asm("a0") = "Hello from U-mode!\n";

    asm volatile("ecall" : : "r"(a7), "r"(a0));


    // Loop forever after syscall
    while (1) {}
}

void user_read(void) {
    size_t len = 64; 
    char buf[len];

    register uint64_t a7 asm("a7") = SYS_read;
    register uint64_t a0 asm("a0") = (uint64_t) buf;
    register const size_t a1 asm("a1") = len;

    asm volatile("ecall" : "+r"(a0) : "r"(a7), "r"(a1));

    char br_buf[32];
    char* bytes_read = itoa((int) a0, 10, br_buf, sizeof(br_buf));
    uart_puts("\nBytes read: ");
    uart_puts(bytes_read);

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

    // Init kernel memory
    kinit();
    uart_puts("kernel: kernel memory initialized\n");

    // Init process table
    proc_init();
    uart_puts("kernel: process table initialized\n");


    // End kernel init
    uart_puts("==================================\n\n");

    uart_puts("Hello. The kernel switched to U mode, issues a read syscall and is now back in S mode,\n");
    uart_puts("waiting for input. Upon newline, execution is back in U-mode, which will, with another syscall,\n");
    uart_puts("output the length:\n");
    drop_to_u_and_call((uintptr_t) user_read);

    // while (1) {}

}