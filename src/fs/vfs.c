#include "fs/vfs.h"
#include "lib/string.h"
#include "modules/ksym.h"

vfs_node_t *fs_root = NULL;
static vfs_fs_type_t *fs_types_head = NULL;

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

void vfs_register_filesystem(vfs_fs_type_t *fs) {
    if (!fs) return;
    fs->next = fs_types_head;
    fs_types_head = fs;
}

vfs_fs_type_t *vfs_find_filesystem(const char *name) {
    if (!name) return NULL;
    vfs_fs_type_t *current = fs_types_head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int vfs_mount_by_name(const char *device_name, const char *mount_path, const char *fs_type_name) {
    block_dev_t *dev = get_block_device(device_name);
    if (!dev) return -1;

    vfs_fs_type_t *fs = vfs_find_filesystem(fs_type_name);
    if (!fs) return -2;

    vfs_node_t *root_node = fs->mount(dev);
    if (!root_node) return -3;

    vfs_node_t *mountpoint = vfs_get_node_by_path(mount_path);
    if (!mountpoint) return -4;

    vfs_mount(mountpoint, root_node);
    return 0;
}
