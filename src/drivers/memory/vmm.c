// SPDX-License-Identifier: GPL-3.0-or-later

#include "vmm.h"
#include "pmm.h"

extern uint64_t hhdm_offset;

static uint64_t *pml4;

static inline uint64_t *phys_to_virt(uint64_t phys) {
    return (uint64_t *)(phys + hhdm_offset);
}

static inline uint64_t alloc_page_table(void) {
    uint64_t page = (uint64_t)pmm_alloc();
    if (!page) {
        printf("VMM: fatal: page null", 0xFF0000);
        for (;;) __asm__ volatile ("hlt");
    }

    uint64_t *pt = phys_to_virt(page);
    for (int i = 0; i < 512; i++) {
        pt[i] = 0;
    }

    return page;
}

static inline uint64_t get_cr3(void) {
    uint64_t val;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(val));
    return val;
}

static inline void set_cr3(uint64_t val) {
    __asm__ volatile ("mov %0, %%cr3" :: "r"(val));
}

static uint64_t *ensure_table(uint64_t *table, int index) {
    if (!(table[index] & PTE_PRESENT)) {
        uint64_t new_table = alloc_page_table();
        table[index] = new_table | PTE_PRESENT | PTE_WRITE;
    }
    return phys_to_virt(table[index] & ~0xFFFULL);
}

void vmm_map_page(uint64_t virt, uint64_t phys, uint64_t flags) {
    uint64_t pml4i = (virt >> 39) & 0x1FF;
    uint64_t pdpti = (virt >> 30) & 0x1FF;
    uint64_t pdi   = (virt >> 21) & 0x1FF;
    uint64_t pti   = (virt >> 12) & 0x1FF;

    uint64_t *pdpt = ensure_table(pml4, pml4i);
    uint64_t *pd   = ensure_table(pdpt, pdpti);
    uint64_t *pt   = ensure_table(pd, pdi);

    pt[pti] = (phys & ~0xFFFULL) | flags | PTE_PRESENT;
}

void vmm_unmap_page(uint64_t virt) {
    uint64_t pml4i = (virt >> 39) & 0x1FF;
    uint64_t pdpti = (virt >> 30) & 0x1FF;
    uint64_t pdi   = (virt >> 21) & 0x1FF;
    uint64_t pti   = (virt >> 12) & 0x1FF;

    uint64_t *pdpt = phys_to_virt(pml4[pml4i] & ~0xFFFULL);
    if (!pdpt) return;

    uint64_t *pd = phys_to_virt(pdpt[pdpti] & ~0xFFFULL);
    if (!pd) return;

    uint64_t *pt = phys_to_virt(pd[pdi] & ~0xFFFULL);
    if (!pt) return;

    pt[pti] = 0;
}

uint64_t vmm_virt_to_phys(uint64_t virt) {
    uint64_t pml4i = (virt >> 39) & 0x1FF;
    uint64_t pdpti = (virt >> 30) & 0x1FF;
    uint64_t pdi   = (virt >> 21) & 0x1FF;
    uint64_t pti   = (virt >> 12) & 0x1FF;

    uint64_t *pdpt = phys_to_virt(pml4[pml4i] & ~0xFFFULL);
    if (!pdpt) return 0;

    uint64_t *pd = phys_to_virt(pdpt[pdpti] & ~0xFFFULL);
    if (!pd) return 0;

    uint64_t *pt = phys_to_virt(pd[pdi] & ~0xFFFULL);
    if (!pt) return 0;

    return pt[pti] & ~0xFFFULL;
}

void vmm_map_range(uint64_t virt, uint64_t phys, size_t size, uint64_t flags) {
    size_t pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (size_t i = 0; i < pages; i++) {
        vmm_map_page(virt + i * PAGE_SIZE,
                     phys + i * PAGE_SIZE,
                     flags);
    }
}

void vmm_unmap_range(uint64_t virt, size_t size) {
    size_t pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (size_t i = 0; i < pages; i++) {
        vmm_unmap_page(virt + i * PAGE_SIZE);
    }
}

void init_vmm(void) {
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));

    pml4 = phys_to_virt(cr3 & ~0xFFFULL);

    if (!pml4) {
        printf("VMM: fatal: pml4 null", 0xFF0000);
        for (;;) __asm__ volatile ("hlt");
    }

    printf("VMM: Succesfully initialized.\n", 0x00FFFF);
}