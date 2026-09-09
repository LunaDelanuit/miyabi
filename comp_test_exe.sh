#!/bin/bash

nasm -f elf64 ./initramfs/test.asm -o ./initramfs/test.o
ld  -Ttext 0x400000 -nostdlib ./initramfs/test.o -o ./initramfs/test
