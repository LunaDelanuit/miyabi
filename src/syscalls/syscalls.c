// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdint.h>
#include "../cmdline.h"
#include "../drivers/fb.h"
#include "../fs/vfs.h"

struct interrupt_frame {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp, r8, r9, r10, r11, r12, r13, r14, r15;

    uint64_t vector;
    uint64_t error_code;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

static void sys_exit(uint64_t status) {
    if (DEBUG) printf("Process exited with status code %d\n", 0xFF0000, status);
    for (;;) __asm__ volatile("hlt");
}

static int64_t sys_read(int fd, char *buf, uint64_t count) {
    return 0;
}

static int64_t sys_write(int fd, const char *buf, uint64_t count) {
    if (fd == 1 || fd == 2) { // stdout || stderr
        for (uint64_t i = 0; i < count; i++) {
            char str[2] = {buf[i], 0};
            printf(str, 0xFFFFFF);
        }
    }
    return -1;
}

void syscall_handler(struct interrupt_frame *frame) {
    switch (frame->rax) {
        case 0:
            sys_exit(frame->rdi);
            break;

        case 1:
            frame->rax = sys_read((int)frame->rdi, (char *)frame->rsi, frame->rdx);
            break;

        case 2:
            frame->rax = sys_write((int)frame->rdi, (const char *)frame->rsi, frame->rdx);
            return;
    }
}
