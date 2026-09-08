// SPDX-License-Identifier: GPL-3.0-or-later

#include "fd.h"
#include "vfs.h"
#include "../drivers/fb.h"

#define MAX_OPEN_FILES 16

typedef struct {
    vfs_node_t *node;
    uint64_t offset;
    int flags;
} file_descriptor_t;

static file_descriptor_t fd_table[MAX_OPEN_FILES];

void init_fds(void) {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        fd_table[i].node = 0;
        fd_table[i].offset = 0;
    }
}

int fd_alloc(vfs_node_t *node) {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (!fd_table[i].node) {
            fd_table[i].node = node;
            fd_table[i].offset = 0;
            return i;
        }
    }
    return -1;
}

vfs_node_t *fd_get_node(int fd) {
    if (fd < 0 || fd >= MAX_OPEN_FILES) return 0;
    return fd_table[fd].node;
}
