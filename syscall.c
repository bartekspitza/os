#include "syscall.h"
#include <stdint.h>


/**
 * Read user input
 * 
 * @param buf The buffer to store the input
 * @param len Max chars if newline is not seen
 * @return The number of bytes read
 */
size_t sys_read(trapframe_t* tf) {
    char* buf = (char*) tf->a0;
    size_t len = tf->a1;

    size_t i = 0;
    while (i < len) {
        char c = uart_getc();
        uart_putc(c);
        buf[i] = c;
        i++;

        if (c == '\r' || c == '\n') {
            break;
        }
    }
    return i;
}

/**
 * Prints via uartc the buffer
 */
void sys_write(trapframe_t* tf) {
    uart_puts((char*) tf->a0);
}

void sys_exit(trapframe_t* tf) {
    // Halt for now, until we handle processes better
    uart_puts("kernel: exit syscall - halting\n");
    while (1) { asm volatile("wfi"); }
}

void syscall_handler(trapframe_t* tf) {
    size_t sys = tf->a7;

    switch (sys) {
        case SYS_exit:
            sys_exit(tf);
            break;
        case SYS_write:
            sys_write(tf);
            break;
        case SYS_read:
            sys_read(tf);
            break;
        default:
            uart_puts("Unknown syscall");
            tf->a0 = -1;
            break;
    }

    tf->sepc += 4;
}