#include "proc.h"
#include "kalloc.h"
#include "lib.h"
#include <stdint.h>

struct proc proc_table[NPROC];

void proc_init(void) {
    memset(proc_table, 0, sizeof(proc_table));
    for (size_t i = 0; i < NPROC; i++) {
        proc_table[i].state = UNUSED;
    }
}

struct proc* allocproc(void) {
    struct proc* p;

    // Search (for empty slot)
    for (size_t i = 0; i < NPROC; i++) {
        if (proc_table[i].state == UNUSED) {
            // Found!
            // Immediately request a memory page
            void* mem_page = kalloc();
            if (mem_page == NULL) {
                return NULL;
            }
            memset(mem_page, 0, PAGE_SIZE);

            p = &proc_table[i]; 
            memset(p, 0, sizeof(*p));
            p->pid = 1; // @todo
            p->state = USED;

            uintptr_t page_top = (uintptr_t) mem_page + PAGE_SIZE;
            p->tf = (trapframe_t*) (page_top - sizeof(trapframe_t));
            // kstack can start right where the trapframe starts, because it grows downwards
            p->kstack = (uintptr_t) p->tf;

            return p;
        }
    }

    return NULL;
}