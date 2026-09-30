/* SPDX-License-Identifier: LGPL-3.0-or-later */

#ifndef EXT2_H
#define EXT2_H

#include "drivers/storage/block.h"
#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint32_t inodes_count;
    uint32_t blocks_count;
    uint32_t reserved_blocks_count;
    uint32_t free_blocks_count;
    uint32_t free_inodes_count;
    uint32_t first_data_block;
    uint32_t log_block_size;
    int32_t  log_fragment_size;
    uint32_t blocks_per_group;
    uint32_t fragments_per_group;
    uint32_t inodes_per_group;
    uint32_t mtime;
    uint32_t wtime;
    uint16_t mount_count;
    int16_t  max_mount_count;
    uint16_t magic;
    uint16_t state;
    uint16_t errors;
    uint16_t minor_rev_level;
    uint32_t lastcheck;
    uint32_t checkinterval;
    uint32_t creator_os;
    uint32_t rev_level;
    uint16_t def_resuid;
    uint16_t def_resgid;
    uint32_t first_ino;
    uint16_t inode_size;
    uint16_t block_group_nr;
    uint32_t feature_compat;
    uint32_t feature_incompat;
    uint32_t feature_ro_compat;
} ext2_superblock_t;

typedef struct ext2_fs {
    block_dev_t *dev;

    uint32_t block_size;
    uint32_t inode_size;

    uint32_t blocks_count;
    uint32_t inodes_count;

    uint32_t blocks_per_group;
    uint32_t inodes_per_group;

    uint32_t group_count;
} ext2_fs_t;

void ext2_init(void);

#endif
