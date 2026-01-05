#include "vm.h"
#include "kalloc.h"
#include "lib.h"
#include "param.h"
#include <stdint.h>


/**
 * Align x downwards to the nearest multiple of a
 */
static inline uint64_t align_down(uintptr_t x, uint64_t a) {
    return x & ~(a - 1);
}

/**
 * Extract physical address (PA) from a PTE
 */
static inline uint64_t pte2pa(pte_t pte) {
    // PPN is in bits [53:10] in sv39
    uint64_t ppn_mask = ((1ULL << 44) - 1);
    uint64_t ppn = (pte >> 10) & ppn_mask;

    // Since PPN is a page number, multiply by page size to get actual PA
    return ppn << PAGE_SHIFT;
}

/**
 * Get PTE with PPN from a PA
 */
static inline pte_t pa2ppn(uintptr_t pa) {
    return ((pa >> PAGE_SHIFT) << 10);
}

/**
 * Get VPN[i] from virtual address VA
 * 
 * @param indx The index of the VPN
 * @return The extracted VPN
 */
static inline int extract_vpn(uint64_t va, int indx) {
    // Sv39: level 2 -> bits 38..30, level 1 -> 29..21, level 0 -> 20..12
    return (int)((va >> (PAGE_SHIFT + 9 * indx)) & 0x1FF);
}

/**
 * Walks the page tables, and creates PTEs as necessary. This tries and walk
 * to L0, if a leaf is encountered at L2 or L1, null is returned
 */
static pte_t* walk_create(pagetable_t root, uint64_t va) {
    pagetable_t pt = root;

    // walk levels 2 -> 1, return pointer to leaf PTE at level 0
    for (int level = 2; level > 0; level--) {
        // At each level, we must extracts the VPN from the VA, i.e. VPN[level]
        // Since a VPN is 9 bits long, it matches perfectly the length of a page table
        // on sv39, i.e. 512 (2**9)
        // Below `pt + pt_index` relies on pointer arithmetic
        int pt_index = extract_vpn(va, level);
        pte_t *pte = pt + pt_index;

        // Following check is needed to know whether we need
        // to allocate a page
        // Root pt should always be zeroed,
        // and in the case of pt being freshly allocated it is zeroed below, 
        // thus, we can trust the PTE_V bit
        if (*pte & PTE_V) {
            // If any of R,W,X is set, this is a leaf node
            // Since we don't handle bigger pages, return null
            if (*pte & (PTE_R | PTE_W | PTE_X)) {
                return NULL;
            }

            // Non-leaf, continue
            pt = (pagetable_t)(uintptr_t)pte2pa(*pte); // still physical here
        } else {
            // Allocate new physical page frame
            void *new_page = kalloc();
            if (new_page == NULL) return NULL;
            memset(new_page, 0, PAGE_SIZE);

            // Encode the new_page pa into the pte
            uintptr_t new_page_pa = (uintptr_t) new_page;
            // Set V-bit to 1 to indicate its now a valid PTE
            *pte = pa2ppn(new_page_pa) | PTE_V;
            // Advance curr page table pointer to new table
            // This is the essence of how the entire page table
            // structure (tree) is built lazily
            pt = (pagetable_t)new_page_pa;
        }
    }

    // L0 reached, return VPN[0]
    int pt_index = extract_vpn(va, 0);
    return pt + pt_index;
}

/**
 * Maps a page of VA to PA, both of which are assumed to be aligned
 * to PAGE_SIZE
 * 
 * @param root Pointer to the root page table
 * @return -1 in case of error, otherwise 0
 */
static int map_page(pagetable_t root, uintptr_t va, uintptr_t pa, uint64_t flags) {
    pte_t *pte = walk_create(root, va);
    if (!pte) return -1;

    if (*pte & PTE_V) {
        // already mapped
        return -1;
    }

    // At this point, the PTE at L0 has been returned, which is zeroed,
    // so we must put PPN and flags in
    // For simplicity, set A/D so we don't need A/D fault handling yet.
    *pte = pa2ppn(pa) | flags | PTE_V | PTE_A | PTE_D;
    return 0;
}

/**
 * Maps a range, [va0, va0 + n_bytes) to [pa0, pa0 + n_bytes)
 * 
 * @param root Pointer to the root page table
 * @param flags The flags of the PTE (bits 0..7)
 * @return -1 in case of error, otherwise 0
 */
static int map_range(pagetable_t root, uintptr_t va0, uintptr_t pa0, uint64_t n_bytes, uint64_t flags) {
    // Guard against below `n_bytes - 1`
    if (n_bytes == 0) return 0;

    uintptr_t va = align_down(va0, PAGE_SIZE);
    uintptr_t pa = align_down(pa0, PAGE_SIZE);
    uintptr_t end = align_down(va0 + n_bytes - 1, PAGE_SIZE);

    for (;;) {
        if (map_page(root, va, pa, flags) != 0) return -1;
        if (va == end) break;
        va += PAGE_SIZE;
        pa += PAGE_SIZE;
    }
    return 0;
}

void vm_init_kernel_pagetable(pagetable_t root) {
    // root is a physical pointer at this stage obtained from kalloc
    memset(root, 0, PAGE_SIZE);

    // Identity map all of ram 1:1, so that we have PA = VA for all of RAM
    // This mapping itself requires 32768 PTEs = 258KiB (interesting note)
    // By doing this, when `satp` is set and paging is enabled, the program counter
    // will work as normal because MMU will translate the 'VA' into the same PA (VA = PA)
    map_range(root, PHYS_RAM_START, PHYS_RAM_START, PHYS_RAM_SIZE, PTE_R | PTE_W | PTE_X);

    // Create the actual map of RAM into the kernel space of VAs
    // This also maps all of RAM as above (and take as much storage), 
    // but at different VAs. Concretely: VA = VA_DMAP_START + PA
    map_range(root,
              VA_DMAP_START + PHYS_RAM_START,
              PHYS_RAM_START,
              PHYS_RAM_SIZE,
              PTE_R | PTE_W | PTE_X);

    // 3) Identity-map UART MMIO page so uart keeps working
    map_range(root, UART0_BASE, UART0_BASE, PAGE_SIZE, PTE_R | PTE_W);
}

static inline void write_satp(uint64_t x) {
    asm volatile("csrw satp, %0" : : "r"(x));
}

static inline void sfence_vma_all(void) {
    asm volatile("sfence.vma zero, zero");
}

static inline void fence_i(void) {
    asm volatile("fence.i");
}

void vm_enable_paging(pagetable_t root) {
    uintptr_t root_pa = (uintptr_t)root;
    uint64_t ppn = root_pa >> PAGE_SHIFT;
    uint64_t mode = (8ULL << 60); // sv39
    uint64_t asid = 0;
    uint64_t satp = mode | asid | ppn; // ASID = 0
    write_satp(satp);

    // Flush stale translations + instruction cache
    sfence_vma_all();
    fence_i();
}
