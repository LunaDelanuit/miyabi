// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "fs/vfs.h"
#include "fs/fd.h"

#define MAX_PROCESSES 32
#define MAX_PROCESS_FDS 16

typedef enum {
    PROCESS_STATE_UNUSED = 0,
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED,
} process_state_t;

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed)) cpu_context_t;

typedef struct {
    uint64_t pid;
    process_state_t state;

    uint64_t pml4_physical;

    uint64_t kernel_stack;
    uint64_t user_stack;

    file_descriptor_t fd_table[MAX_PROCESS_FDS];

    char name[32];
} process_t;

void init_process_subsystem(void);
process_t *create_process(const char *name, void *entry_point);
process_t *get_current_processes(void);

#endif
