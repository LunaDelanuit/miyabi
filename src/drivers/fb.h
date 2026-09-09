// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FB_H
#define FB_H

#include <stdint.h>
#include <stddef.h>
#include "limine.h"
#include "fs/vfs.h"

extern volatile struct limine_framebuffer_request framebuffer_request;

int64_t sys_write(int fd, const void *buf, uint64_t count);

void init_fb(void);
void clear(uint32_t color);
void set_cursor(size_t x, size_t y);

void put_char(char c, uint32_t color);
void puts(const char *str, uint32_t color);
void print_hex64(uint64_t value, uint32_t color);

void printf(const char *fmt, uint32_t color, ...);

vfs_node_t *fb_create_vfs_node(void);
uint64_t fb_vfs_write(vfs_node_t *node, uint8_t *buffer, uint64_t size, uint64_t offset);

#endif
