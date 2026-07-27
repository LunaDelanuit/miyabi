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
    for (uint32_t i = 0; i < ramfs_node_count; i++) {
        if (tar_match_name(ramfs_nodes[i]->name, name)) {
            return ramfs_nodes[i];
        }
    }
    return NULL;
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
        }
        
        ramfs_nodes[ramfs_node_count++] = node;
        
        uint64_t blocks = (size / 512) + ((size % 512) ? 1 : 0);
        header = (struct tar_header *)((uint64_t)header + 512 + (blocks * 512));
    }
    
    return ramfs_root;
}