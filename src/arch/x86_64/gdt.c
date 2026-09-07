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

static struct gdt_entry gdt[] __attribute__((aligned(0x10))) = {
    {0, 0, 0, 0x00, 0x00, 0},  // 0x00: Null
    {0, 0, 0, 0x9A, 0x20, 0},  // 0x08: Kernel Code
    {0, 0, 0, 0x92, 0x00, 0},  // 0x10: Kernel Data
};

static struct gdt_ptr gp;

extern void gdt_flush(struct gdt_ptr *gp);

void init_gdt(void) {
    if (sizeof(struct gdt_ptr) != 10) {
        for(;;);
    }

    gp.limit = sizeof(gdt) - 1;
    gp.base  = (uintptr_t)&gdt;

    gdt_flush(&gp);
}