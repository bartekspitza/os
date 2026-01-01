#pragma once
#include "trap.h"
#include "uart.h"

#define SYS_write 0
#define SYS_exit 1
#define SYS_read 2

void syscall_handler(trapframe_t* tf);
