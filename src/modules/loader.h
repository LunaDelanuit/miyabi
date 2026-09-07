// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MODULE_LOADER_H
#define MODULE_LOADER_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    char name[64];
    uint8_t *memory_base;
    size_t memory_size;
    
    void (*init)(void);
    void (*cleanup)(void);
} kernel_module_t;

kernel_module_t *load_module(const char *name, uint8_t *file_buffer);
kernel_module_t *load_module_from_file(const char *path);

#endif