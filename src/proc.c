#include "proc.h"
#include "kalloc.h"
#include <stdint.h>

struct proc proc_table[NPROC];

void proc_init(void) {
    for (size_t i = 0; i < NPROC; i++) {
        proc_table[i].state = UNUSED;
        proc_table[i].kstack = 0;
        proc_table[i].pid = 0;
        proc_table[i].tf = 0;
    }
}

struct proc* allocproc(void) {
    struct proc* p;

    // Search (for empty slot)
    for (size_t i = 0; i < NPROC; i++) {
        if (proc_table[i].state == UNUSED) {
            // Found!
            p = &proc_table[i];
            p->pid = 1; // This will be handled down the line
            p->state = USED;

            // Request a memory page
            void* mem_page = kalloc();
            if (mem_page == NULL) {
                // If OOM, rollback the process state
                p->state = UNUSED;
            }

            uintptr_t page_top = (uintptr_t) mem_page + PAGE_SIZE;
            p->tf = (trapframe_t*) page_top - sizeof(trapframe_t);
            // kstack can start right where the trapframe starts, because it grows downwards
            p->kstack = (uintptr_t) p->tf;

            return p;
        }
    }

    return NULL;
}