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
    while (size-- > 0 && *c != '\0' && *c != ' ') {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}

static int tar_match_name(const char *tar_name, const char *search_name) {
    int i = 0;
    while (search_name[i] != '\0') {
        if (tar_name[i] != search_name[i]) return 0;
        i++;
    }
    if (tar_name[i] == '\0' || (tar_name[i] == '/' && tar_name[i+1] == '\0')) return 1;
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
        if (tar_match_name(ramfs_nodes[k]->name, expected_path)) {
            return ramfs_nodes[k];
        }
    }
    return NULL;
}

static uint64_t ramfs_read(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer) {
    if (offset >= node->length) return 0;
    if (offset + size > node->length) size = node->length - offset;
    
    uint8_t *file_data = (uint8_t *)node->ptr;
    for (uint64_t i = 0; i < size; i++) {
        buffer[i] = file_data[offset + i];
    }
    return size;
}

vfs_node_t *init_initramfs(uint64_t ramfs_addr) {
    ramfs_root = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
    for(uint32_t i=0; i<sizeof(vfs_node_t); i++) ((uint8_t*)ramfs_root)[i] = 0;
    
    ramfs_root->flags = FS_DIRECTORY;
    ramfs_root->name[0] = '/';
    ramfs_root->finddir = ramfs_finddir;
    
    struct tar_header *header = (struct tar_header *)ramfs_addr;
    
    while (header->filename[0] != '\0') {
        uint64_t size = oct2bin((unsigned char *)header->size, 11);
        
        vfs_node_t *node = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
        for(uint32_t i=0; i<sizeof(vfs_node_t); i++) ((uint8_t*)node)[i] = 0;
        
        int i = 0;
        while(header->filename[i] && i < 99) {
            node->name[i] = header->filename[i];
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