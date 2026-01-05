#include "param.h"
#include <stdint.h>

// 16550 registers (offsets)
#define UART_RHR 0x00 // Receive holding (read)
#define UART_THR 0x00 // Transmit holding (write)
#define UART_LSR 0x05 // Line status

#define LSR_TX_IDLE (1 << 5) // THR empty

static inline void mmio_write8(uintptr_t addr, uint8_t value) {
    *(volatile uint8_t*)addr = value;
}

static inline uint8_t mmio_read(uintptr_t addr) {
    return *(volatile uint8_t*)addr;
}

char uart_getc(void) {
    // Wait until data is ready
    while ((mmio_read(UART0_BASE+UART_LSR) & 1) == 0) {}

    // Return next char
    return (char)mmio_read(UART0_BASE+UART_RHR);
}

void uart_putc(char c) {
    while ((mmio_read(UART0_BASE+UART_LSR) & LSR_TX_IDLE) == 0) {}

    if (c == '\n') {
        uart_putc('\r');
    }

    mmio_write8(UART0_BASE + UART_THR, (uint8_t) c);
}

void uart_puts(const char* str) {
    while (*str) {
        uart_putc(*str);
        str++;
    }
}
