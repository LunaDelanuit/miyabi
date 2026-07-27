// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEVFS_H
#define DEVFS_H

#include "vfs.h"

vfs_node_t *init_devfs(void);
void devfs_register(vfs_node_t *device);
int devfs_strcmp(const char *s1, const char *s2);

#endif