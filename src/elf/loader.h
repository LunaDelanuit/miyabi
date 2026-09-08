#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>

uint64_t elf_load(void *elf_binary, uint64_t user_pml4);

#endif
