// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdint.h>
#include "../cmdline.h"
#include "../drivers/fb.h"

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

void syscall_handler(struct interrupt_frame *frame) {
    switch (frame->rax) {
        case 0: // exit
            if (DEBUG) printf("Exit syscall\n", 0x0000FF);
            for (;;) __asm__ volatile("hlt");
            return;

        case 1: // read
            return;

        case 2: // write
            if (DEBUG) printf("Write syscall\n", 0x0000FF);
            return;
    }
}
