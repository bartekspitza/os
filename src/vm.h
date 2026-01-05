#pragma once
#include <stdint.h>
#include <stddef.h>

// Sv39: 3 levels, 512 entries per page table page
#define PT_ENTRIES 512

// --- VA layout (Sv39) ---
#define VA_SV39_UPPER_START (-(1LL << 38))                      // canonical upper-half of sv39 (-2^38)
#define VA_KERNEL_START     ((uintptr_t) VA_SV39_UPPER_START)    // kernel starts at the upper half of sv39
#define VA_DMAP_START       VA_KERNEL_START                     // direct map base

// PTE flags (RISC-V)
#define PTE_V (1UL << 0)
#define PTE_R (1UL << 1)
#define PTE_W (1UL << 2)
#define PTE_X (1UL << 3)
#define PTE_U (1UL << 4)
#define PTE_G (1UL << 5)
#define PTE_A (1UL << 6)
#define PTE_D (1UL << 7)

typedef uint64_t pte_t;     // A page table entry
typedef pte_t* pagetable_t; // A pointer to a page table, i.e. the first pte

// Build initial kernel page table (identity + direct map + UART)
void vm_init_kernel_pagetable(pagetable_t root);

// Enable paging with the given root page table (root is a physical page address)
void vm_enable_paging(pagetable_t root);
