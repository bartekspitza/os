#include "uart.h"
#include "trap.h"
#include "lib.h"
#include "syscall.h"
#include "kalloc.h"
#include "proc.h"
#include "vm.h"
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

struct proc *current;
/**
 * Declare the trampoline defined in entry.S that will switch
 * the sp to the current process' kstack addr, then jump to
 * the provided function
 */
__attribute__((noreturn))
extern void kstack_trampoline(uintptr_t kstack_addr, void (*fn_to_jump_to)(void));

void after_kstack_switch(void) {
    uart_puts("Preparing to run user process");
    while(1) {}
}

static void kernel_after_paging(void) {
    uart_puts("kernel: paging enabled, jumped to higher-half alias\n");

    // Init process table
    proc_init();
    uart_puts("kernel: process table initialized\n");

    // End kernel init
    uart_puts("==================================\n\n");

    // Switch to init user process
    current = allocproc();
    kstack_trampoline(current->kstack, after_kstack_switch);

    __builtin_unreachable();
}

void enable_paging() {
    // Allocate root page table (physical page)
    void *root_page = kalloc();
    if (!root_page) {
        uart_puts("kernel: kalloc for root page table failed\n");
        while (1) {}
    }

    vm_init_kernel_pagetable((pagetable_t)root_page);

    // Enable paging (identity map keeps us alive immediately after satp write)
    vm_enable_paging((pagetable_t)root_page);

    // Perform jump (move PC) to the direct map
    void (*hi)(void) = (void (*)(void))((uintptr_t)kernel_after_paging + (uintptr_t)VA_DMAP_START);
    hi();

    __builtin_unreachable();
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

    // Enable paging: virtual memory
    enable_paging();
    __builtin_unreachable();
}