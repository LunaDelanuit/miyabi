// SPDX-License-Identifier: GPL-3.0-or-later

#include "devfs.h"
#include "drivers/memory/heap.h"

#include <stddef.h>

#define MAX_DEVFS_DEVICES 32

static vfs_node_t *devfs_root = NULL;
static vfs_node_t *devices[MAX_DEVFS_DEVICES];
static uint32_t device_count = 0;

int devfs_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

static vfs_node_t *devfs_finddir(vfs_node_t *node, char *name) {
    for (uint32_t i = 0; i < device_count; i++) {
        if (devfs_strcmp(devices[i]->name, name) == 0) {
            return devices[i];
        }
    }
    return NULL;
}

vfs_node_t *init_devfs(void) {
    devfs_root = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));

    for (uint32_t i = 0; i < sizeof(vfs_node_t); i++) {
        ((uint8_t*)devfs_root)[i] = 0;
    }

    devfs_root->flags = FS_DIRECTORY;
    devfs_root->name[0] = 'd';
    devfs_root->name[1] = 'e';
    devfs_root->name[2] = 'v';
    devfs_root->name[3] = '\0';
    devfs_root->finddir = devfs_finddir;

    for (uint32_t i = 0; i < MAX_DEVFS_DEVICES; i++) {
        devices[i] = NULL;
    }

    return devfs_root;
}

void devfs_register(vfs_node_t *device) {
    if (device_count >= MAX_DEVFS_DEVICES) return;
    devices[device_count++] = device;
}
