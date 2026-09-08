// SPDX-License-Identifier: GPL-3.0-or-later

#include "gdt.h"

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct tss_entry {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist[7];
    uint64_t reserved2;
    uint64_t reserved3;
    uint16_t iomap_base;
} __attribute__((packed));

static struct tss_entry kernel_tss;

static struct gdt_entry gdt[] __attribute__((aligned(0x10))) = {
    {0, 0, 0, 0x00, 0x00, 0},  // 0x00: Null
    {0, 0, 0, 0x9A, 0x20, 0},  // 0x08: Kernel Code
    {0, 0, 0, 0x92, 0x00, 0},  // 0x10: Kernel Data
    {0, 0, 0, 0xF2, 0x00, 0},  // 0x18: User Data
    {0, 0, 0, 0xFA, 0x20, 0},  // 0x20: User Code
    {0, 0, 0, 0x00, 0x00, 0},  // 0x28: TSS Descriptor Low
    {0, 0, 0, 0x00, 0x00, 0}   // 0x30: TSS Descriptor High
};

static struct gdt_ptr gp;

extern void gdt_flush(struct gdt_ptr *gp);

static uint8_t initial_kernel_stack[4096] __attribute__((aligned(16)));

static void set_tss_descriptor(int index, uint64_t base, uint32_t limit) {
    gdt[index].limit_low = limit & 0xFFFF;
    gdt[index].base_low = base & 0xFFFF;
    gdt[index].base_mid = (base >> 16) & 0xFF;
    gdt[index].access = 0x89;
    gdt[index].granularity = (limit >> 16) & 0x0F;
    gdt[index].base_high = (base >> 24) & 0xFF;

    struct gdt_entry *high_entry = &gdt[index + 1];
    *(uint32_t *)high_entry = (base >> 32);
    *(uint32_t *)((uintptr_t)high_entry + 4) = 0;
}

static inline void load_tss(uint16_t sel) {
    __asm__ volatile ("ltr %0" : : "r" (sel));
}

void init_gdt(void) {
    if (sizeof(struct gdt_ptr) != 10) {
        for(;;);
    }

    for (volatile uint8_t *p = (uint8_t*)&kernel_tss; p < (uint8_t*)&kernel_tss + sizeof(kernel_tss); p++) {
        *p = 0;
    }

    kernel_tss.rsp0 = (uint64_t)(initial_kernel_stack + sizeof(initial_kernel_stack));

    set_tss_descriptor(5, (uintptr_t)&kernel_tss, sizeof(kernel_tss) - 1);

    gp.limit = sizeof(gdt) - 1;
    gp.base  = (uintptr_t)&gdt;

    gdt_flush(&gp);

    load_tss(0x28);
}
