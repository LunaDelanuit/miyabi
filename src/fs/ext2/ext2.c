/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "fs/ext2/ext2.h"
#include "drivers/storage/block.h"
#include "fs/vfs.h"
#include "drivers/fb.h"
#include "mm/heap.h"

static vfs_node_t *ext2_mount(block_dev_t *dev) {
    if (!dev) {
        return NULL;
    }

    uint8_t buf[1024];

    if (!block_read(dev, 2, 2, buf)){
        printf("EXT2: Failed to read superblock.\n", 0xFF0000);
        return NULL;
    }

    ext2_superblock_t *sb = (ext2_superblock_t *)buf;

    if (sb->magic != 0xEF53) {
        printf("EXT2: Invalid filesystem magic: 0x%x\n", 0xFF0000, sb->magic);
        return NULL;
    }

    ext2_fs_t *fs = kmalloc(sizeof(ext2_fs_t));
    if (!fs) {
        printf("EXT2: Failed to allocate filesystem state.\n", 0xFF0000);
        return NULL;
    }

    fs->dev = dev;
    fs->block_size = 1024U << sb->log_block_size;
    fs->inode_size = sb->inode_size;
    fs->blocks_count = sb->blocks_count;
    fs->inodes_count = sb->inodes_count;
    fs->blocks_per_group = sb->blocks_per_group;
    fs->inodes_per_group = sb->inodes_per_group;

    fs->group_count =
        (fs->blocks_count + fs->blocks_per_group - 1) /
        fs->blocks_per_group;

    printf("EXT2: Valid filesystem detected.\n", 0x00FF00);
    printf("EXT2: Block size: %d bytes\n", 0x00FFFF, fs->block_size);
    printf("EXT2: Inodes: %d\n", 0x00FFFF, fs->inodes_count);
    printf("EXT2: Blocks: %d\n", 0x00FFFF, fs->blocks_count);
    printf("EXT2: Block groups: %d\n", 0x00FFFF, fs->group_count);

    return NULL;
}

static vfs_fs_type_t ext2_fs_type = {
    .name = "ext2",
    .mount = ext2_mount,
    .next = NULL
};

void ext2_init(void) {
    vfs_register_filesystem(&ext2_fs_type);
    printf("EXT2: Filesystem initialized.\n", 0x00FF00);
}
