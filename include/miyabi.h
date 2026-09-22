/* SPDX-License-Identifier: LGPL-3.0-or-later */

#ifndef MIYABI_H
#define MIYABI_H

#include <stdint.h>
#include <stddef.h>

/* Expected module functions */
void module_init(void);
void module_cleanup(void); /* Not implemented as I don't expect to unload a module in the near future */

#endif
