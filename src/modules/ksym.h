// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KSYM_H
#define KSYM_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    const char *name;
    uint64_t addr;
} kernel_symbol_t;

/* 
 * Macro to automatically register a function in the .ksymtab section.
 * Using 'used' prevents the compiler from optimizing it away if it 
 * thinks it isn't referenced locally.
 */
#define EXPORT_SYMBOL(sym) \
    __attribute__((section(".ksymtab"), used)) \
    static kernel_symbol_t __ksym_##sym = { #sym, (uint64_t)&sym }

uint64_t ksym_lookup(const char *name);

#endif