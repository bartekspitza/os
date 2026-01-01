#pragma once
#include <stdint.h>

typedef struct trapframe {
    uintptr_t ra, sp, gp, tp;
    uintptr_t t0, t1, t2;
    uintptr_t s0, s1;
    uintptr_t a0, a1, a2, a3, a4, a5, a6, a7;
    uintptr_t s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
    uintptr_t t3, t4, t5, t6;

    uintptr_t scause;
    uintptr_t sepc;
    uintptr_t stval;
} trapframe_t;

void trap_init(void);
void trap_handler(trapframe_t* trapframe);