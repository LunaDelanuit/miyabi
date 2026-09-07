// SPDX-License-Identifier: GPL-3.0-or-later

#include "initramfs.h"
#include "../drivers/memory/heap.h"
#include "../lib/string.h"

#define MAX_RAMFS_NODES 128
static vfs_node_t *ramfs_nodes[MAX_RAMFS_NODES];
static uint32_t ramfs_node_count = 0;
static vfs_node_t *ramfs_root = NULL;

struct tar_header {
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char padding[355];
};

static uint64_t oct2bin(unsigned char *str, int size) {
    uint64_t n = 0;
    unsigned char *c = str;

    while (size > 0 && (*c == ' ' || *c == '\0')) {
        c++;
        size--;
    }

    while (size > 0 && *c >= '0' && *c <= '7') {
        n = (n << 3) + (*c - '0');
        c++;
        size--;
    }
    return n;
}

static const char *sanitize_path(const char *path) {
    while (*path == '.' || *path == '/') {
        path++;
    }
    return path;
}

static int tar_match_name(const char *tar_name, const char *search_name) {
    if (!tar_name || !search_name) return 0;

    const char *clean_tar = sanitize_path(tar_name);
    const char *clean_search = sanitize_path(search_name);

    int i = 0;
    while (clean_tar[i] != '\0' && clean_search[i] != '\0') {
        if (clean_tar[i] != clean_search[i]) return 0;
        i++;
    }

    if (clean_tar[i] == '\0' && clean_search[i] == '\0') return 1;
    if (clean_tar[i] == '/' && clean_tar[i + 1] == '\0' && clean_search[i] == '\0') return 1;
    if (clean_search[i] == '/' && clean_search[i + 1] == '\0' && clean_tar[i] == '\0') return 1;

    return 0;
}

static vfs_node_t *ramfs_finddir(vfs_node_t *node, char *name) {
    char expected_path[256];
    int i = 0;

    if (node != ramfs_root) {
        while (node->name[i] && i < 255) {
            expected_path[i] = node->name[i];
            i++;
        }
        if (i > 0 && expected_path[i-1] != '/') {
            expected_path[i++] = '/';
        }
    }

    int j = 0;
    while (name[j] && i < 255) {
        expected_path[i++] = name[j++];
    }
    expected_path[i] = '\0';

    for (uint32_t k = 0; k < ramfs_node_count; k++) {
        if (!ramfs_nodes[k]) continue;
        if (tar_match_name(ramfs_nodes[k]->name, expected_path)) {
            return ramfs_nodes[k];
        }
    }
    return NULL;
}

static uint64_t ramfs_read(vfs_node_t *node, uint8_t *buffer, uint64_t size, uint64_t offset) {
    if (!node || !buffer) return 0;
    if (offset >= node->length) return 0;
    if (offset + size > node->length) size = node->length - offset;

    uint8_t *file_data = (uint8_t *)node->ptr;
    for (uint64_t i = 0; i < size; i++) {
        buffer[i] = file_data[offset + i];
    }
    return size;
}

static void ensure_parent_dirs_exist(const char *raw_path) {
    const char *path = sanitize_path(raw_path);
    char dir_path[256];
    int len = 0;

    for (int i = 0; path[i]; i++) {
        if (path[i] == '/') {
            dir_path[len] = '\0';

            int found = 0;
            for (uint32_t k = 0; k < ramfs_node_count; k++) {
                if (!ramfs_nodes[k]) continue;
                if (tar_match_name(ramfs_nodes[k]->name, dir_path)) {
                    found = 1;
                    break;
                }
            }

            if (!found && len > 0) {
                if (ramfs_node_count >= MAX_RAMFS_NODES) return;

                vfs_node_t *dir_node = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
                if (!dir_node) return;

                for (uint32_t z = 0; z < sizeof(vfs_node_t); z++) ((uint8_t*)dir_node)[z] = 0;

                for (int m = 0; m < len; m++) dir_node->name[m] = dir_path[m];
                dir_node->name[len] = '\0';
                dir_node->flags = FS_DIRECTORY;
                dir_node->finddir = ramfs_finddir;

                ramfs_nodes[ramfs_node_count++] = dir_node;
            }
        }
        dir_path[len++] = path[i];
    }
}

vfs_node_t *init_initramfs(uint64_t ramfs_addr) {
    ramfs_root = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
    if (!ramfs_root) return NULL;

    for (uint32_t i = 0; i < sizeof(vfs_node_t); i++) ((uint8_t*)ramfs_root)[i] = 0;

    ramfs_root->flags = FS_DIRECTORY;
    ramfs_root->name[0] = '/';
    ramfs_root->finddir = ramfs_finddir;

    struct tar_header *header = (struct tar_header *)ramfs_addr;

    while (header->filename[0] != '\0') {
        const char *clean_filename = sanitize_path(header->filename);
        uint64_t size = oct2bin((unsigned char *)header->size, 11);

        if (header->typeflag == 'x' || header->typeflag == 'g' || header->typeflag == 'K' || header->typeflag == 'L') {
            uint64_t blocks = (size / 512) + ((size % 512) ? 1 : 0);
            header = (struct tar_header *)((uint64_t)header + 512 + (blocks * 512));
            continue;
        }

        ensure_parent_dirs_exist(clean_filename);

        if (ramfs_node_count >= MAX_RAMFS_NODES) break;

        vfs_node_t *node = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
        if (!node) break;

        for (uint32_t i = 0; i < sizeof(vfs_node_t); i++) ((uint8_t*)node)[i] = 0;

        int i = 0;
        while (clean_filename[i] && i < 99) {
            node->name[i] = clean_filename[i];
            i++;
        }
        node->name[i] = '\0';

        if (header->typeflag == '5') {
            node->flags = FS_DIRECTORY;
            node->finddir = ramfs_finddir;
        } else {
            node->flags = FS_FILE;
            node->length = size;
            node->read = ramfs_read;
            node->ptr = (struct vfs_node *)((uint64_t)header + 512);
        }

        ramfs_nodes[ramfs_node_count++] = node;

        uint64_t blocks = (size / 512) + ((size % 512) ? 1 : 0);
        header = (struct tar_header *)((uint64_t)header + 512 + (blocks * 512));
    }

    return ramfs_root;
}
