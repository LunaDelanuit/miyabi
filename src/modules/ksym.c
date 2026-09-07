// SPDX-License-Identifier: GPL-3.0-or-later

#include "ksym.h"

extern kernel_symbol_t __start_ksymtab[];
extern kernel_symbol_t __stop_ksymtab[];

static int ksym_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

uint64_t ksym_lookup(const char *name) {
    kernel_symbol_t *sym = __start_ksymtab;

    while (sym < __stop_ksymtab) {
        if (ksym_strcmp(sym->name, name) == 0) {
            return sym->addr;
        }
        sym++;
    }

    return 0;
}