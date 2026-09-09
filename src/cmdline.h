#ifndef CMDLINE_H
#define CMDLINE_H

#include "limine/include/limine.h"
#include <stdbool.h>

extern bool DEBUG;
extern bool ENABLE_KERMO;

extern volatile struct limine_executable_cmdline_request cmdline_request;

void read_boot_cmdline(void);

#endif
