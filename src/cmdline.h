#ifndef CMDLINE_H
#define CMDLINE_H

#include "limine.h"
#include <stdbool.h>
#include <stddef.h>

#define MAX_BOOT_MODULES 16
#define MAX_BOOT_MODULE_PATH 128

extern bool DEBUG;

extern volatile struct limine_executable_cmdline_request cmdline_request;

void read_boot_cmdline(void);
size_t boot_module_count(void);
const char *boot_module_path(size_t index);

#endif
