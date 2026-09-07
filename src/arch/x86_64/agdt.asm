; SPDX-License-Identifier: GPL-3.0-or-later

[BITS 64]
global gdt_flush

gdt_flush:
    lgdt [rdi]

    mov ax, 0x10          ; kernel data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rax, .return
    push 0x08             ; kernel code
    push rax
    retfq

.return:
    ret