/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include <stddef.h>
#include "modules/ksym.h"

#define BLOCK_DEV_NAME_LEN 32
#define MAX_BLOCK_DEVICES  16

typedef struct block_dev {
    char name[BLOCK_DEV_NAME_LEN];
    uint32_t sector_size;
    uint64_t total_sectors;
    uint64_t start_lba;

    /* Hardware drive operations */
    int (*read)(struct block_dev *dev, uint64_t lba, uint32_t count, void *buf);
    int (*write)(struct block_dev *dev, uint64_t lba, uint32_t count, const void *buf);

    void *priv_data; /* Context for hardware driver or parent device */
} block_dev_t;

void init_block_subsystem(void);
int register_block_device(block_dev_t *dev);
block_dev_t *get_block_device(const char *name);

int block_read(block_dev_t *dev, uint64_t lba, uint32_t count, void *buf);
int block_write(block_dev_t *dev, uint64_t lba, uint32_t count, const void *buf);

#endif
