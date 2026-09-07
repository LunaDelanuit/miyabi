// SPDX-License-Identifier: GPL-3.0-or-later

#if defined(__x86_64__)
    #include "arch/x86_64/gdt.h"
    #include "arch/x86_64/idt.h"
    #include "arch/x86_64/pic.h"
    #include "arch/x86_64/pit.h"
    #include "arch/x86_64/thread.h"
#else
    #error "Unsupported architecture. Compile for x86_64."
#endif

#include "limine/include/limine.h"

#include "cmdline.h"

#include "drivers/memory/pmm.h"
#include "drivers/memory/vmm.h"
#include "drivers/memory/heap.h"
#include "drivers/fb.h"

#include "fs/vfs.h"
#include "fs/devfs.h"
#include "fs/initramfs.h"

__attribute__((used, section(".requests")))
static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST_ID,
    .revision = 0
};

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}

void _start(void) {
    __asm__ volatile("cli");

    remap_pic(32, 40);

    init_gdt();
    init_idt();

    init_fb();
    clear(0x000000);

    read_boot_cmdline();

    pit_init(100);

    inb(0x60);
    outb(0x21, 0xFC);

    init_pmm();
    init_vmm();
    init_heap();

    init_scheduler();

    __asm__ volatile("sti");

    printf("\n", 0x000000);

    vfs_node_t *devfs_root_node = init_devfs();

    if (module_request.response != NULL && module_request.response->module_count > 0) {
        struct limine_file *ramfs_file = module_request.response->modules[0];
        if (DEBUG) printf("VFS: Booting from Initramfs...\n", 0x00FFCC);

        fs_root = init_initramfs((uint64_t)ramfs_file->address);
    } else {
        printf("VFS: No Initramfs module found! Please check your limine config...\n", 0xFF0000);
        for (;;) __asm__ volatile("hlt");
    }

    devfs_register(fb_create_vfs_node());

    vfs_node_t *dev_mountpoint = vfs_get_node_by_path("/dev");

    if (dev_mountpoint) {
        vfs_mount(dev_mountpoint, devfs_root_node);
        printf("VFS: DevFS mounted to /dev successfully.\n", 0x00FF00);
    } else {
        printf("VFS: Could not find /dev in Initramfs!\n", 0xFF0000);
    }

    printf("\nWelcome to Miyabi ", 0xFFFFFF);
    printf("0.1\n", 0xFFFFFF);
    printf("Copyright (c) 2026 Luna Delanuit and contributers, ", 0xCC00DD);
    printf("GNU General Public License v3.0-or-later.\n\n", 0xFF2200);

    for (;;) __asm__ volatile("hlt");
}
