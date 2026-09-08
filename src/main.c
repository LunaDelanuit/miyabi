// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdint.h>
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

#include "elf/loader.h"

extern void switch_to_user_space(uint64_t user_stack, uint64_t user_func);

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

   vfs_node_t *test_node = vfs_open("/test", VFS_FLAG_READ);
   if (!test_node) {
       printf("/test does not exist.", 0xFF0000);
       for (;;) __asm__ volatile("hlt");
   }

   uint64_t file_size = test_node->length;
   uint8_t *elf_buffer = (uint8_t *)kmalloc(file_size);

   vfs_read(test_node, elf_buffer, file_size, 0);
   vfs_close(test_node);

    uint64_t user_pml4_phys = vmm_create_user_pml4();
    if (!user_pml4_phys) {
        printf("Failed to create user address space.\n", 0xFF0000);
        for (;;) __asm__ volatile("hlt");
    }

    uint64_t kernel_pml4_phys;
    __asm__ volatile("mov %%cr3, %0" : "=r"(kernel_pml4_phys));

    vmm_switch_pml4(user_pml4_phys);

    uint64_t entry_point = elf_load(elf_buffer, user_pml4_phys);

    if (entry_point) {
        uint64_t stack_phys = (uint64_t)pmm_alloc();
        uint64_t user_stack_virtual = 0x7FFFFFFF0000;

        vmm_map_page(user_stack_virtual, stack_phys, PTE_PRESENT | PTE_USER | PTE_WRITE);

        uint8_t *stack_virt = (uint8_t *)phys_to_virt(stack_phys);
        for (int i = 0; i < 4096; i++) stack_virt[i] = 0;

        vmm_switch_pml4(kernel_pml4_phys);

        if (DEBUG) printf("Entering user space...\n", 0xAAAAFF);

        /* The iretq frame below executes at a user virtual address. */
        vmm_switch_pml4(user_pml4_phys);
        switch_to_user_space(user_stack_virtual + 3072, entry_point);
    }

    for (;;) __asm__ volatile("hlt");
}
