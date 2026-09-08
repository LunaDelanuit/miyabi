[BITS 64]
global switch_to_user_space

section .text
switch_to_user_space:
    cli

    mov ax, 0x1B
    mov dx, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push 0x1B
    push rdi

    pushfq
    pop rax
    or rax, 0x200
    push rax

    push 0x23
    push rsi

    iretq
