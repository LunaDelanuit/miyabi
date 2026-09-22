// SPDX-License-Identifier: GPL-3.0-or-later

#include "fs/fd.h"
#include "fs/vfs.h"

#define MAX_OPEN_FILES 16

static file_descriptor_t fd_table[MAX_OPEN_FILES];

void init_fds(void) {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        fd_table[i].node = 0;
        fd_table[i].offset = 0;
        fd_table[i].flags = 0;
    }

    vfs_node_t *fb_node = vfs_get_node_by_path("/dev/fb0");
    if (fb_node) {
        fd_table[1].node = fb_node;
        fd_table[1].flags = VFS_FLAG_WRITE;

        fd_table[2].node =  fb_node;
        fd_table[2].flags = VFS_FLAG_WRITE;
    }
}

int fd_alloc(vfs_node_t *node) {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (!fd_table[i].node) {
            fd_table[i].node = node;
            fd_table[i].offset = 0;
            fd_table[i].flags = 0;
            return i;
        }
    }
    return -1;
}

vfs_node_t *fd_get_node(int fd) {
    if (fd < 0 || fd >= MAX_OPEN_FILES) return 0;
    return fd_table[fd].node;
}

void fd_close(int fd) {
    if (fd < 0 || fd >= MAX_OPEN_FILES) return;
    fd_table[fd].node = 0;
    fd_table[fd].offset = 0;
    fd_table[fd].flags = 0;
}
