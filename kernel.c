#include "uart.h"
#include "trap.h"

void kernel_main(void) {
    uart_puts("Hello World");

    // Install trap handler
    trap_init();
    uart_puts("Trap handler installed. Triggering illegal instruction...\n");

    asm volatile("unimp");  // should cause an exception trap

    while (1) {}
}