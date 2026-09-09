// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define PAGE_SIZE 4096ULL

#define PTE_PRESENT  (1ULL << 0)
#define PTE_WRITE    (1ULL << 1)
#define PTE_USER     (1ULL << 2)
#define PTE_PWT      (1ULL << 3)
#define PTE_PCD      (1ULL << 4)
#define PTE_ACCESSED (1ULL << 5)
#define PTE_DIRTY    (1ULL << 6)
#define PTE_HUGE     (1ULL << 7)
#define PTE_GLOBAL   (1ULL << 8)
#define PTE_NX       (1ULL << 63)

extern uint64_t hhdm_offset;

static inline uint64_t *phys_to_virt(uint64_t phys) {
    return (uint64_t *)(phys + hhdm_offset);
}

static inline void set_cr3(uint64_t val) {
    __asm__ volatile ("mov %0, %%cr3" :: "r"(val));
}

void init_vmm(void);

uint64_t vmm_create_user_pml4(void);
void vmm_switch_pml4(uint64_t pml4_phys);

void vmm_map_page(uint64_t virt, uint64_t phys, uint64_t flags);
void vmm_unmap_page(uint64_t virt);

uint64_t vmm_virt_to_phys(uint64_t virt);

void vmm_map_range(uint64_t virt, uint64_t phys, size_t size, uint64_t flags);
void vmm_unmap_range(uint64_t virt, size_t size);

#endif
