#include "uart.h"

void kernel_main(void) {
    uart_puts("Hello World");
    while(1) {}
}