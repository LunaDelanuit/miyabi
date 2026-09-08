global _start

section .text
_start:
    mov rax, 2      ; miyabi syscall write
    mov rdi, 1      ; stdout
    mov rsi, msg
    mov rdx, msg_len
    int 0x80

    mov rax, 0      ; miyabi syscall exit
    mov rax, 0      ; exit code 0
    int 0x80

section .data
msg db "Hello, Miyabi!", 0Ah, 0
msg_len equ $ - msg
