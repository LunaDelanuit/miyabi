// SPDX-License-Identifier: GPL-3.0-or-later

#include "pmm.h"

uint64_t hhdm_offset = 0;
static uint8_t *bitmap = NULL;
static uint64_t bitmap_phys = 0;
static uint64_t page_count = 0;
static uint64_t bitmap_bytes = 0;
static uint64_t bitmap_pages = 0;

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0
};

static inline uint64_t align_down_4k(uint64_t x) {
    return x & ~0xFFFULL;
}

static inline uint64_t align_up_4k(uint64_t x) {
    return (x + 0xFFFULL) & ~0xFFFULL;
}

static inline void bitmap_set(uint64_t page) {
    bitmap[page >> 3] |= (uint8_t)(1u << (page & 7));
}

static inline void bitmap_clear(uint64_t page) {
    bitmap[page >> 3] &= (uint8_t)~(1u << (page & 7));
}

static inline bool bitmap_test(uint64_t page) {
    return (bitmap[page >> 3] & (uint8_t)(1u << (page & 7))) != 0;
}

static uint64_t kernel_start = 0;
static uint64_t     kernel_end = 0;

extern char _kernel_start;
extern char _kernel_end;

void *pmm_alloc(void) {
    for (uint64_t i = 1; i < page_count; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            return (void *)(i * 4096ULL);
        }
    }
    return NULL;
}

static int region_overlaps_kernel(uint64_t start, uint64_t end) {
    return !(end <= kernel_start || start >= kernel_end);
}

void init_pmm(void) {
    if (memmap_request.response == NULL || hhdm_request.response == NULL) {
        for (;;) __asm__ volatile("hlt");
    }

    hhdm_offset = hhdm_request.response->offset;

    kernel_start = (uint64_t)&_kernel_start;
    kernel_end = (uint64_t)&_kernel_end;

    struct limine_memmap_response *map = memmap_request.response;
    uint64_t highest = 0;

    for (uint64_t i = 0; i < map->entry_count; i++) {
        struct limine_memmap_entry *entry = map->entries[i];
        uint64_t end = entry->base + entry->length;
        if (end > highest) {
            highest = end;
        }
    }

    page_count = align_up_4k(highest) / 4096ULL;
    bitmap_bytes = (page_count + 7ULL) / 8ULL;
    bitmap_pages = align_up_4k(bitmap_bytes) / 4096ULL;

    uint64_t chosen_base = 0;
    int found = 0;

    for (uint64_t i = 0; i < map->entry_count; i++) {
        struct limine_memmap_entry *entry = map->entries[i];

        if (entry->type != LIMINE_MEMMAP_USABLE) {
            continue;
        }

        uint64_t start = align_up_4k(entry->base);
        uint64_t end = align_down_4k(entry->base + entry->length);

        if (end <= start) {
            continue;
        }

        if (region_overlaps_kernel(start, end)) {
            continue;
        }

        if ((end - start) >= bitmap_pages * 4096ULL) {
            chosen_base = start;
            found = 1;
            break;
        }
    }

    if (!found) {
        for (;;) __asm__ volatile("hlt");
    }

    bitmap_phys = chosen_base;
    bitmap = (uint8_t *)(bitmap_phys + hhdm_offset);

    for (uint64_t i = 0; i < bitmap_bytes; i++) {
        bitmap[i] = 0xFF;
    }

    for (uint64_t i = 0; i < map->entry_count; i++) {
        struct limine_memmap_entry *entry = map->entries[i];

        uint64_t type = entry->type;
        if (type != LIMINE_MEMMAP_USABLE) {
            continue;
        }

        uint64_t start = align_up_4k(entry->base);
        uint64_t end = align_down_4k(entry->base + entry->length);

        if (end <= start) {
            continue;
        }

        if (region_overlaps_kernel(start, end)) {
            continue;
        }

        for (uint64_t addr = start; addr < end; addr += 4096ULL) {
            bitmap_clear(addr / 4096ULL);
        }
    }

    for (uint64_t i = 0; i < bitmap_pages; i++) {
        bitmap_set((bitmap_phys / 4096ULL) + i);
    }

    bitmap_set(0);

    for (uint64_t i = 0; i < map->entry_count; i++) {
        struct limine_memmap_entry *entry = map->entries[i];

        if (entry->type == LIMINE_MEMMAP_USABLE ||
            entry->type == LIMINE_MEMMAP_ACPI_RECLAIMABLE ||
            entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {

            uint64_t start = align_up_4k(entry->base);
            uint64_t end = align_down_4k(entry->base + entry->length);

            if (end <= start) {
                continue;
            }

            if (region_overlaps_kernel(start, end)) {
                continue;
            }

            for (uint64_t addr = start; addr < end; addr += 4096ULL) {
                bitmap_clear(addr / 4096ULL);
            }
        }
    }

    printf("PMM: Succesfully initialized.\n", 0x00FFFF);
}