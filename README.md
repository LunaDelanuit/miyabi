As of July 31st, 2026, I have decided to rename Vale to Miyabi. This change is partly due to my new identity, and I would prefer not to have any ties to my old identity which I dislike. Miyabi is personally a better name for the project as it means "elegance" in Japanese, which is the project's core philosophy.

# Miyabi

Miyabi is a 64-bit operating system available for the x86_64 architecture.  
At its current state, Miyabi is extremely limited, but has been tested on real hardware.

I plan to make Miyabi Unix-like and potentially POSIX-complaint.

I *do not* plan to ever make Miyabi UNIX certified, nor do I plan to make Miyabi a part of the GNU Project for now.  
Miyabi is an independant operating system with no current intentions of replacing existing systems such as GNU/Linux.

Now I am following my own custom model, a hybrid of a Microkernel and a Monolithic-kernel. It reduces the overhead and reduency of microkernel designs with the IPC,
but also less complex and tied together than a Monolithic-kernel, allowing theoretically better stability.

## License

### Source Code

Copyright (C) 2026 Luna Delanuit and contributers.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

### Header Files

See `COPYING.LESSER`.

## Build

Required build tools:

* Any GNU/Linux system.
* Python 3.x `python3`
* Pillow for Python `python-pillow`
* GNU C Compiler `gcc`
* Netwide Assembler `nasm`
* GNU Linker `ld`
* Xorriso `xorriso`
* Limine 10.x-12.x `limine`

Optionally:

* QEMU `qemu-system-x86_64` if you want to emulate Miyabi.

In the project root, simply run `make`; an ISO file will be generated in ./build.
Alternatively, type `make run` to emulate Miyabi using QEMU.
You can also add `FONT=` followed by the path to either a ttf, hex, or bdf font, and it will be used in the build!
The `bin` folder has some fonts already, the makefile by default will use Terminus.

> [!WARNING]
> The Framebuffer expects an 8x16 font, please use a font desgined for the 8x16 restriction.

## Features

Currently, Miyabi has:

* BIOS & UEFI Support
* GDT & IDT
* PIC Interrupts
* Stable Heap allocator
* Device Framebuffer using VFS
* Initramfs

Limited to:

* x86_64 Only
* No APIC
* Very limited kernel modules
* 3 interrupt-based system calls.
* Barely usuable (for now)

> [!CAUTION]
> Kernel modules are very experimental, and prone to crashing.

Does not have:

* Multi-architectural support
* Filesystems (tho we have an initramfs)
* Process Switching
* Everything else
