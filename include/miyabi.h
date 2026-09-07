// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MIYABI_H
#define MIYABI_H

#include <stdint.h>
#include <stddef.h>

// Exported kernel functions
void printf(const char *fmt, uint32_t color, ...);
void *kmalloc(size_t size);
void kfree(void *ptr);

//Expected module functions
void module_init(void);
void module_cleanup(void); // Not implemented as I don't expect to unload a module in the near futur

#endif
