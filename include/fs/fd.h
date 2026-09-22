/* SPDX-License-Identifier: LGPL-3.0-or-later */

#ifndef FD_H
#define FD_H

#include "vfs.h"

typedef struct {
    vfs_node_t *node;
    uint64_t offset;
    int flags;
} file_descriptor_t;

void init_fds(void);
int fd_alloc(vfs_node_t *node);
vfs_node_t *fd_get_node(int fd);
void fd_close(int fd);

#endif
