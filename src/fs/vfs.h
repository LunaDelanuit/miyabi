// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <stddef.h>

#define FS_FILE         0x01
#define FS_DIRECTORY    0x02
#define FS_CHARDEVICE   0x04
#define FS_BLOCKDEVICE  0x08
#define FS_PIPE         0x10
#define FS_SYMLINK      0x20
#define FS_MOUNTPOINT   0x40

#define VFS_FLAG_READ   0x01
#define VFS_FLAG_WRITE  0x02

struct vfs_node;

typedef uint64_t (*read_type_t)(struct vfs_node*, uint8_t*, uint64_t, uint64_t);
typedef uint64_t (*write_type_t)(struct vfs_node*, uint8_t*, uint64_t, uint64_t);
typedef void (*open_type_t)(struct vfs_node*, uint32_t);
typedef void (*close_type_t)(struct vfs_node*);
typedef struct vfs_node* (*readdir_type_t)(struct vfs_node*, uint32_t);
typedef struct vfs_node* (*finddir_type_t)(struct vfs_node*, char *name);

typedef struct vfs_node {
    char name[128];
    uint32_t mask;
    uint32_t uid;
    uint32_t gid;
    uint32_t flags;
    uint32_t inode;
    uint32_t length;
    uint32_t impl;
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    struct vfs_node *ptr;
} vfs_node_t;

extern vfs_node_t *fs_root;

void vfs_mount(vfs_node_t *mountpoint, vfs_node_t *target);
vfs_node_t *vfs_get_node_by_path(const char *path);
vfs_node_t *vfs_open(const char *path, uint32_t flags);
uint64_t vfs_read(vfs_node_t *node, uint8_t *buffer, uint64_t size, uint64_t offset);
void vfs_close(vfs_node_t *node);

#endif