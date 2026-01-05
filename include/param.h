#pragma once

// --- Arch, rv64 uses 4KiB pages

#define PAGE_SHIFT      12
#define PAGE_SIZE       (1UL << PAGE_SHIFT)

// --- Physical memory layout (QEMU virt defaults)
#define PHYS_RAM_START  0x80000000ULL
#define PHYS_RAM_SIZE   (128ULL * 1024 * 1024) // 128MiB
#define PHYS_RAM_END    (PHYS_RAM_START + PHYS_RAM_SIZE)

// --- MMIO
#define UART0_BASE     0x10000000UL
