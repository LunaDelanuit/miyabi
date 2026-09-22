/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "drivers/storage/block.h"
#include "drivers/fb.h"
#include "lib/string.h"

static block_dev_t block_devices[MAX_BLOCK_DEVICES];
static uint32_t num_block_devices = 0;

void init_block_subsystem(void) {
    num_block_devices = 0;
    for (int i = 0; i < MAX_BLOCK_DEVICES; i++) {
        block_devices[i].name[0] = '\0';
    }
    printf("BLOCK: Subsytem initialized.\n", 0x00FF00);
}

int register_block_device(block_dev_t *dev) {
    if (num_block_devices >= MAX_BLOCK_DEVICES) {
        printf("BLOCKL: Max block device limit reached\n", 0xFF0000);
        return -1;
    }

    block_devices[num_block_devices] = *dev;
    printf("BLOCK: Registered block device: %s (Start LBA: %d, Sectors: %d)\n", 0x00FFFF,
        dev->name, (uint32_t)dev->start_lba, (uint32_t)dev->total_sectors
    );

    return num_block_devices++;
}

block_dev_t *get_block_device(const char  *name) {
    for (uint32_t i = 0; i < num_block_devices; i++) {
        if (strcmp(block_devices[i].name, name) == 0) {
            return &block_devices[i];
        }
    }
    return NULL;
}

int block_read(block_dev_t *dev, uint64_t lba, uint32_t count, void *buf) {
    if (!dev || !dev->read) return 0;

    uint64_t absolute_lba = dev->start_lba + lba;
    return dev->read(dev, absolute_lba, count, buf);
}

int block_write(block_dev_t *dev, uint64_t lba, uint32_t count, const void *buf) {
    if (!dev || !dev->write) return 0;

    uint64_t absolute_lba = dev->start_lba + lba;
    return dev->write(dev, absolute_lba, count, buf);
}
