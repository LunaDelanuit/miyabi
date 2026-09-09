// SPDX-License-Identifier: GPL-3.0-or-later

#include "fb.h"
#include <stdarg.h>
#include "../arch/x86_64/spinlock.h"
#include "../drivers/memory/heap.h"
#include "../fs/fd.h"
#include "../modules/ksym.h"

#define FONT_WIDTH  8
#define FONT_HEIGHT 16

extern uint8_t _binary_font_bin_start[];

spinlock_t fb_lock = SPINLOCK_INIT;

__attribute__((used, section(".limine_requests")))
volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

static struct limine_framebuffer *fb;

static size_t cursor_x = 0;
static size_t cursor_y = 0;

static size_t cols;
static size_t rows;

static uint32_t back;
static uint32_t default_fg = 0xFFFFFF;

static vfs_node_t *fb_vfs_node = NULL;

static int validate_user_pointer(const void *ptr, size_t size) {
    uint64_t addr = (uint64_t)ptr;
    if (addr + size < addr || addr >= 0x0000800000000000) {
        return 0;
    }
    return 1;
}

int64_t sys_write(int fd, const void *buf, uint64_t count) {
    vfs_node_t *node = fd_get_node(fd);
    if (!node) {
        return -1;
    }

    if (!node->write) {
        return -1;
    }

    if (!validate_user_pointer(buf, count)) {
        return -1;
    }

    default_fg = 0xFFFFFFF;

    uint64_t bytes_written = node->write(node, (uint8_t *)buf, count, 0);

    return (int64_t)bytes_written;
}

void init_fb(void) {
    if (!framebuffer_request.response ||
        framebuffer_request.response->framebuffer_count < 1) {
        for (;;) __asm__("hlt");
    }

    fb = framebuffer_request.response->framebuffers[0];

    cols = fb->width / FONT_WIDTH;
    rows = fb->height / FONT_HEIGHT;
}

void clear(uint32_t color) {
    back = color;

    uint32_t *dst = (uint32_t *)fb->address;
    size_t total_pixels = (fb->pitch / 4) * fb->height;

    for (size_t i = 0; i < total_pixels; i++) {
        dst[i] = color;
    }

    cursor_x = 0;
    cursor_y = 0;
}

void set_cursor(size_t x, size_t y) {
    if (x < cols) cursor_x = x;
    if (y < rows) cursor_y = y;
}

static void scroll(uint32_t bg) {
    uint8_t *base = (uint8_t *)fb->address;
    size_t line_size = fb->pitch * FONT_HEIGHT;

    for (size_t y = 1; y < rows; y++) {
        uint8_t *src = base + y * line_size;
        uint8_t *dst = base + (y - 1) * line_size;

        for (size_t i = 0; i < line_size; i++) {
            dst[i] = src[i];
        }
    }

    uint32_t *last = (uint32_t *)(base + (rows - 1) * line_size);
    size_t last_line_pixels = (fb->pitch / 4) * FONT_HEIGHT;
    for (size_t i = 0; i < last_line_pixels; i++) {
        last[i] = bg;
    }
}

static void draw_char(char c, size_t cx, size_t cy, uint32_t fg, uint32_t bg) {
    uint8_t *glyph = &_binary_font_bin_start[(unsigned char)c * 16];

    for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 8; col++) {
            int bit = (glyph[row] >> (7 - col)) & 1;
            uint32_t color = bit ? fg : bg;

            uint32_t *pixel = (uint32_t *)((uint8_t *)fb->address +
                (cy * FONT_HEIGHT + row) * fb->pitch +
                (cx * FONT_WIDTH + col) * (fb->bpp / 8));

            *pixel = color;
        }
    }
}

void put_char(char c, uint32_t color) {
    uint32_t bg = back;

    switch (c) {
        case '\n':
            cursor_x = 0;
            cursor_y++;
            break;

        case '\r':
            cursor_x = 0;
            break;

        case '\t':
            cursor_x = (cursor_x + 4) & ~(4 - 1);
            break;

        case '\b':
            if (cursor_x > 0) cursor_x--;
            break;

        default:
            draw_char(c, cursor_x, cursor_y, color, bg);
            cursor_x++;
            break;
    }

    if (cursor_x >= cols) {
        cursor_x = 0;
        cursor_y++;
    }

    if (cursor_y >= rows) {
        scroll(bg);
        cursor_y = rows - 1;
    }
}

void puts(const char *str, uint32_t color) {
    while (*str) {
        put_char(*str++, color);
    }
}

void print_hex64(uint64_t value, uint32_t color) {
    char buf[17];

    for (int i = 15; i >= 0; i--) {
        uint8_t digit = value & 0xF;

        buf[i] = digit < 10
            ? '0' + digit
            : 'A' + (digit - 10);

        value >>= 4;
    }

    buf[16] = '\0';

    puts("0x", color);
    puts(buf, color);
}

static void print_num_to_buf(char *buf, int *idx, int val, int base) {
    char tmp[32];
    int i = 0;
    int neg = 0;

    if (val == 0) {
        buf[(*idx)++] = '0';
        return;
    }

    if (base == 10 && val < 0) {
        neg = 1;
        val = -val;
    }

    while (val) {
        int d = val % base;
        tmp[i++] = d < 10 ? '0' + d : 'A' + d - 10;
        val /= base;
    }

    if (neg) tmp[i++] = '-';

    while (i--) {
        buf[(*idx)++] = tmp[i];
    }
}

uint64_t fb_vfs_write(vfs_node_t *node, uint8_t *buffer, uint64_t size, uint64_t offset) {
    (void)node;
    (void)offset;

    spin_lock(&fb_lock);

    for (uint64_t i = 0; i < size; i++) {
        put_char((char)buffer[i], default_fg);
    }

    spin_unlock(&fb_lock);

    return size;
}

vfs_node_t *fb_create_vfs_node(void) {
    vfs_node_t *node = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
    for (uint32_t i = 0; i < sizeof(vfs_node_t); i++) ((uint8_t*)node)[i] = 0;

    node->flags = FS_CHARDEVICE;
    node->name[0] = 'f'; node->name[1] = 'b'; node->name[2] = '0'; node->name[3] = '\0';
    node->write = fb_vfs_write;

    fb_vfs_node = node;
    return node;
}

void printf(const char *fmt, uint32_t color, ...) {
    char out_buf[1024];
    int buf_idx = 0;

    default_fg = color;

    va_list args;
    va_start(args, color);

    for (; *fmt; fmt++) {
        if (buf_idx >= 1023) break;

        if (*fmt != '%') {
            out_buf[buf_idx++] = *fmt;
            continue;
        }

        fmt++;

        switch (*fmt) {
            case 's': {
                char *s = va_arg(args, char *);
                while (*s && buf_idx < 1023) out_buf[buf_idx++] = *s++;
                break;
            }

            case 'c':
                if (buf_idx < 1023) out_buf[buf_idx++] = (char)va_arg(args, int);
                break;

            case 'd':
                print_num_to_buf(out_buf, &buf_idx, va_arg(args, int), 10);
                break;

            case 'x':
                print_num_to_buf(out_buf, &buf_idx, va_arg(args, int), 16);
                break;

            case '%':
                if (buf_idx < 1023) out_buf[buf_idx++] = '%';
                break;
        }
    }

    va_end(args);

    out_buf[buf_idx] = '\0';

    if (fb_vfs_node && fb_vfs_node->write) {
        fb_vfs_node->write(fb_vfs_node, (uint8_t*)out_buf, buf_idx, 0);
    } else {
        spin_lock(&fb_lock);
        for (int i = 0; i < buf_idx; i++) {
            put_char(out_buf[i], color);
        }
        spin_unlock(&fb_lock);
    }
}

EXPORT_SYMBOL(printf);
