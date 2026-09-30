> [!IMPORTANT]
> Miyabi is being archived in favour of a new operating system called [Mellow](https://www.github.com/lunadelanuit/mellow).

### Why?

As I worked on Miyabi, I felt like it wasn't really what I was looking for.  
I started Miyabi when I new little about how operating systems work, and I learned as I developed,  
resulting in a weird state where old files (such as `fb.c`, `vfs.c`) use preferences and styles I don't prefer  
to use anymore.

Miyabi used to be called *Vale* (named after an identity I don't use anymore, and I discourage  
using the term when refering to Miyabi), before I got my idea of how I wanted the OS to look.  
When I first developed Miyabi, I was primarily going for a Monolithic approach. When I wanted Miyabi to be modular, it was
already quite late in my terms, and the implementation was rushed and buggy, which made dealing with
kernel modules (excuse my language) a pain in the fucking ass.

Whilst Miyabi was the furthest I've ever gone to making an operating system, I believe a fresh start is needed.  
With **Mellow**, I'm able to make the design and what I want clear before starting work.  
I'll be able to design the architecture, philosphy, and design straight from the beginning.

That said, Miyabi will still be public, and if you ever believe Miyabi has potential, feel free  
to fork it and continue development!

 - Best regards, Luna Delanuit.

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
