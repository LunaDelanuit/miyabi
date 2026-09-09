// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PMM_H
#define PMM_H

#include "limine/include/limine.h"
#include "drivers/fb.h"
#include "stddef.h"
#include "stdint.h"
#include "stdbool.h"

void init_pmm(void);
void *pmm_alloc(void);

#endif
