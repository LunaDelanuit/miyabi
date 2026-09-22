/* SPDX-License-Identifier: LGPL-3.0-or-later */

#ifndef INITRAMFS_H
#define INITRAMFS_H

#include "vfs.h"

vfs_node_t *init_initramfs(uint64_t ramfs_addr);

#endif
