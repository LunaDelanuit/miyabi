// SPDX-License-Identifier: GPL-3.0-or-later

#include "vfs.h"

vfs_node_t *fs_root = NULL;

static int get_next_token(const char *path, int offset, char *token) {
    if (!path[offset]) return -1;
    
    while (path[offset] == '/') {
        offset++;
    }
    
    if (!path[offset]) return -1;
    
    int i = 0;
    while (path[offset] && path[offset] != '/') {
        token[i++] = path[offset++];
    }
    token[i] = '\0';
    
    return offset;
}

void vfs_mount(vfs_node_t *mountpoint, vfs_node_t *target) {
    if (!mountpoint) return;
    mountpoint->flags |= FS_MOUNTPOINT;
    mountpoint->ptr = target;
}

vfs_node_t *vfs_get_node_by_path(const char *path) {
    if (!path || path[0] != '/') return NULL;
    
    vfs_node_t *current_node = fs_root;
    char token[128];
    int offset = 0;

    while ((offset = get_next_token(path, offset, token)) != -1) {
        if (!current_node) return NULL;
        
        if (current_node->flags & FS_MOUNTPOINT) {
            current_node = current_node->ptr;
            if (!current_node) return NULL;
        }
        
        if ((current_node->flags & FS_DIRECTORY) == 0) {
            return NULL;
        }
        
        if (current_node->finddir) {
            current_node = current_node->finddir(current_node, token);
            if (!current_node) return NULL;
        } else {
            return NULL;
        }
    }
    
    if (current_node && (current_node->flags & FS_MOUNTPOINT)) {
        current_node = current_node->ptr;
    }
    
    return current_node;
}

vfs_node_t *vfs_open(const char *path, uint32_t flags) {
    vfs_node_t *node = vfs_get_node_by_path(path);
    
    if (node) {
        if (node->open) {
            node->open(node, flags);
        }
        return node;
    }
    
    return NULL;
}

uint64_t vfs_read(vfs_node_t *node, uint8_t *buffer, uint64_t size, uint64_t offset) {
    if (node && node->read) {
        return node->read(node, buffer, size, offset);
    }

    return 0;
}

void vfs_close(vfs_node_t *node) {
    if (node && node->close) {
        node->close(node);
    }
}