#pragma once
#include <stdint.h>
#include "trap.h"

#define NPROC 64

// The state of a process(slot) in the process table
enum procstate { 
    UNUSED, // Slot is empty. allocproc() can grab this
    USED, // Slot is reserved, but not yet ready to run
    SLEEPING, // Waiting for IO etc. Don't schedule this
    RUNNABLE, // Has work to do and is waiting for a CPU core
    RUNNING, // Currently executing on a CPU core
    ZOMBIE // Called exit(). Parent needs to collect return code
};

// The main data structure to hold information about a process
struct proc {
    enum procstate state;
    int pid;
    uint64_t kstack;
    struct trapframe *tf;
};

/**
 * Initializes the process table
 */
void proc_init();

/**
 * Finds and initialies a slot from the process table
 */
struct proc* allocproc();