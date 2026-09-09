// SPDX-License-Identifier: GPL-3.0-or-later

#include "loader.h"
#include "elf.h"
#include "cmdline.h"
#include "drivers/fb.h"
#include "mm/pmm.h"
#include "mm/vmm.h"

uint64_t elf_load(void *elf_binary, uint64_t user_pml4) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf_binary;

    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr->e_ident[EI_MAG3] != ELFMAG3) {
        if (DEBUG) printf("ELF: Invalid magic number\n", 0xFF0000);
        return 0;
    }

    if (ehdr->e_machine != EM_X86_64 || ehdr->e_type != ET_EXEC) {
        if (DEBUG) printf("ELF: Not a valid x86_64 executable\n", 0xFF0000);
        return 0;
    }

    Elf64_Phdr *phdr = (Elf64_Phdr *)((uint8_t *)elf_binary + ehdr->e_phoff);

    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            uint64_t vaddr = phdr[i].p_vaddr;
            uint64_t memsz = phdr[i].p_memsz;
            uint64_t filesz = phdr[i].p_filesz;
            uint64_t offset = phdr[i].p_offset;

            uint64_t page_vaddr = vaddr & ~0xFFF;
            uint64_t page_offset = vaddr & 0xFFF;
            uint64_t total_size = memsz + page_offset;
            uint64_t num_pages = (total_size + 0xFFF) / 0x1000;

            for (uint64_t p = 0; p < num_pages; p++) {
                uint64_t phys_frame = (uint64_t)pmm_alloc();
                uint64_t flags = PTE_PRESENT | PTE_USER;

                if (phdr[i].p_flags & PF_W) {
                    flags |= PTE_WRITE;
                }

                vmm_map_page(page_vaddr + (p * 0x1000), phys_frame, flags);

                uint8_t *virt_page = (uint8_t *)phys_to_virt(phys_frame);
                for (int b = 0; b < 4096; b++) {
                    virt_page[b] = 0;
                }

                uint64_t curr_page_vaddr = page_vaddr + (p * 0x1000);
                uint64_t seg_start = vaddr;
                uint64_t seg_end = vaddr + filesz;

                uint64_t copy_start = curr_page_vaddr;
                if (copy_start < seg_start) copy_start = seg_start;

                uint64_t copy_end = curr_page_vaddr + 0x1000;
                if (copy_end > seg_end) copy_end = seg_end;

                if (copy_start < copy_end) {
                    uint64_t amount = copy_end - copy_start;
                    uint64_t file_src_offset = offset + (copy_start - seg_start);
                    uint64_t page_dest_offset = copy_start - curr_page_vaddr;

                    uint8_t *src = (uint8_t *)elf_binary + file_src_offset;
                    uint8_t *dest = virt_page + page_dest_offset;

                    for (uint64_t j = 0; j < amount; j++) {
                        dest[j] = src[j];
                    }
                }
            }
        }
    }

    if (DEBUG) printf("ELF: Successfully loaded executable. Entry: 0x%x\n", ehdr->e_entry, 0x00FF00);
    return ehdr->e_entry;
}
