#include "trap.h"
#include "uart.h"
#include <stdint.h>

static inline void write_stvec(uint64_t x) {
    asm volatile("csrw stvec, %0" : : "r"(x));
}

static inline void write_sepc(uint64_t x) {
    asm volatile("csrw sepc, %0" : : "r"(x));
}

static void uart_puthex(uint64_t x) {
    static const char *h = "0123456789abcdef";
    uart_puts("0x");
    for (int i = 15; i >= 0; i--) {
        uart_putc(h[(x >> (i * 4)) & 0xF]);
    }
}

static int scause_is_interrupt(uint64_t scause) {
    return (scause >> 63) & 1;
}

void trap_init(void) {
    // trap_entry is our assembly trap func
    // use extern to be able to reference it

    extern void trap_entry(void);
    write_stvec((uint64_t)trap_entry);
}

void trap_handler(trapframe_t* tf) {
    uart_puts("\n=== TRAP ===\n");
    uart_puts(scause_is_interrupt(tf->scause) ? "type=interrupt\n" : "type=exception\n");

    uart_puts("scause="); uart_puthex(tf->scause); uart_puts("\n");
    uart_puts("sepc  ="); uart_puthex(tf->sepc);   uart_puts("\n");
    uart_puts("stval ="); uart_puthex(tf->stval);  uart_puts("\n");

    // For now, just hang
    while (1) {}
}