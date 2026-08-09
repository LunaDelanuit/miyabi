// SPDX-License-Identifier: GPL-3.0-or-later

#include "ksym.h"
#include "loader.h"
#include "../elf/elf.h"
#include "../drivers/fb.h"
#include "../drivers/memory/heap.h"
#include "../fs/vfs.h"

static int elf_validate(Elf64_Ehdr *ehdr) {
    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr->e_ident[EI_MAG3] != ELFMAG3) {
        printf("LOADER: Invalid ELF magic bytes.\n", 0xFF0000);
        return 0;
    }

    if (ehdr->e_ident[EI_CLASS] != ELFCLASS64) {
        printf("LOADER: Module is not 64-bit.\n", 0xFF0000);
        return 0;
    }

    if (ehdr->e_machine != EM_X86_64) {
        printf("LOADER: Module is not compiled for x86_64.\n", 0xFF0000);
        return 0;
    }

    if (ehdr->e_type != ET_REL) {
        printf("LOADER: Module is not a relocatable file (ET_REL).\n", 0xFF0000);
        return 0;
    }

    return 1;
}

kernel_module_t *load_module(const char *name, uint8_t *file_buffer) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file_buffer;

    printf("LOADER: Loading module: %s\n", 0x00FFCC, name);

    if (!elf_validate(ehdr)) {
        printf("LOADER: Failed to validate %s\n", 0xFF0000, name);
        return NULL;
    }

    printf("LOADER: %s is a valid x86_64 relocatable ELF.\n", 0x00FF00, name);

    Elf64_Shdr *shdrs = (Elf64_Shdr *)(file_buffer + ehdr->e_shoff);
    size_t total_memory_size = 0;

    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (shdrs[i].sh_flags & SHF_ALLOC) {
            if (shdrs[i].sh_addralign > 1) {
                total_memory_size = (total_memory_size + shdrs[i].sh_addralign - 1) & ~(shdrs[i].sh_addralign - 1);
            }
            total_memory_size += shdrs[i].sh_size;
        }
    }

    uint8_t *module_memory = (uint8_t *)kmalloc(total_memory_size);
    if (!module_memory) {
        printf("LOADER: Failed to allocate memory for module.\n", 0xFF0000);
        return NULL;
    }

    printf("LOADER: Allocated %d bytes at 0x%x for module.\n", 0x00FF00, total_memory_size, (uint64_t)module_memory);

    size_t current_offset = 0;
    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (shdrs[i].sh_flags & SHF_ALLOC) {
            if (shdrs[i].sh_addralign > 1) {
                current_offset = (current_offset + shdrs[i].sh_addralign - 1) & ~(shdrs[i].sh_addralign - 1);
            }

            shdrs[i].sh_addr = (uint64_t)(module_memory + current_offset);

            if (shdrs[i].sh_type == SHT_NOBITS) {
                uint8_t *dst = (uint8_t *)shdrs[i].sh_addr;
                for (size_t j = 0; j < shdrs[i].sh_size; j++) {
                    dst[j] = 0;
                }
            } else {
                uint8_t *src = file_buffer + shdrs[i].sh_offset;
                uint8_t *dst = (uint8_t *)shdrs[i].sh_addr;
                for (size_t j = 0; j < shdrs[i].sh_size; j++) {
                    dst[j] = src[j];
                }
            }

            current_offset += shdrs[i].sh_size;
        }
    }

    Elf64_Shdr *symtab = NULL;
    Elf64_Shdr *strtab = NULL;

    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (shdrs[i].sh_type == SHT_SYMTAB) {
            symtab = &shdrs[i];
            strtab = &shdrs[symtab->sh_link]; 
            break;
        }
    }

    if (!symtab || !strtab) {
        printf("LOADER: No symbol table found in module.\n", 0xFF0000);
        return NULL;
    }

    Elf64_Sym *syms = (Elf64_Sym *)(file_buffer + symtab->sh_offset);
    char *strings = (char *)(file_buffer + strtab->sh_offset);

    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (shdrs[i].sh_type != SHT_RELA) continue;

        Elf64_Shdr *target_section = &shdrs[shdrs[i].sh_info]; 
        
        if (!(target_section->sh_flags & SHF_ALLOC)) continue;

        Elf64_Rela *relas = (Elf64_Rela *)(file_buffer + shdrs[i].sh_offset);
        int num_relas = shdrs[i].sh_size / sizeof(Elf64_Rela);

        for (int j = 0; j < num_relas; j++) {
            Elf64_Rela *rela = &relas[j];
            
            int sym_idx = ELF64_R_SYM(rela->r_info);
            int rel_type = ELF64_R_TYPE(rela->r_info);
            
            Elf64_Sym *sym = &syms[sym_idx];
            char *sym_name = strings + sym->st_name;
            
            uint64_t sym_val = 0;

            if (sym->st_shndx == SHN_UNDEF) {
                sym_val = ksym_lookup(sym_name);
                if (!sym_val) {
                    printf("LOADER: Unresolved external symbol: %s\n", 0xFF0000, sym_name);
                    return NULL;
                }
            } else if (sym->st_shndx == SHN_ABS) {
                sym_val = sym->st_value;
            } else {
                Elf64_Shdr *sym_sec = &shdrs[sym->st_shndx];
                sym_val = sym_sec->sh_addr + sym->st_value;
            }

            uint64_t patch_addr = target_section->sh_addr + rela->r_offset;
            
            uint64_t *patch_ptr64 = (uint64_t *)patch_addr;
            uint32_t *patch_ptr32 = (uint32_t *)patch_addr;

            switch (rel_type) {
                case R_X86_64_NONE:
                    break;
                
                case R_X86_64_64: 
                    *patch_ptr64 = sym_val + rela->r_addend;
                    break;
                
                case R_X86_64_PC32:
                case R_X86_64_PLT32: 
                    *patch_ptr32 = (uint32_t)(sym_val + rela->r_addend - patch_addr);
                    break;
                
                case R_X86_64_32:
                case R_X86_64_32S:
                    *patch_ptr32 = (uint32_t)(sym_val + rela->r_addend);
                    break;
                
                default:
                    printf("LOADER: Unsupported relocation type: %d\n", 0xFF0000, rel_type);
                    return NULL;
            }
        }
    }
    
    printf("LOADER: Relocations processed successfully.\n", 0x00FF00);

    kernel_module_t *mod = (kernel_module_t *)kmalloc(sizeof(kernel_module_t));
    if (!mod) {
        printf("LOADER: Failed to allocate kernel_module_t.\n", 0xFF0000);
        return NULL;
    }

    for (int i = 0; i < 63 && name[i]; i++) {
        mod->name[i] = name[i];
        mod->name[i + 1] = '\0';
    }
    mod->memory_base = module_memory;
    mod->memory_size = total_memory_size;
    mod->init = NULL;
    mod->cleanup = NULL;

    int num_total_syms = symtab->sh_size / sizeof(Elf64_Sym);
    
    for (int i = 0; i < num_total_syms; i++) {
        Elf64_Sym *sym = &syms[i];
        
        if (sym->st_shndx == SHN_UNDEF || sym->st_shndx == SHN_ABS) continue;
        
        char *sym_name = strings + sym->st_name;
        
        int is_init = 1, is_cleanup = 1;
        const char *init_str = "module_init";
        const char *clean_str = "module_cleanup";
        
        for (int k = 0; init_str[k] || sym_name[k]; k++) {
            if (init_str[k] != sym_name[k]) { is_init = 0; break; }
        }
        for (int k = 0; clean_str[k] || sym_name[k]; k++) {
            if (clean_str[k] != sym_name[k]) { is_cleanup = 0; break; }
        }

        if (is_init) {
            Elf64_Shdr *sec = &shdrs[sym->st_shndx];
            mod->init = (void (*)(void))(sec->sh_addr + sym->st_value);
        } else if (is_cleanup) {
            Elf64_Shdr *sec = &shdrs[sym->st_shndx];
            mod->cleanup = (void (*)(void))(sec->sh_addr + sym->st_value);
        }
    }

    if (!mod->init) {
        printf("LOADER: No module_init() found in %s\n", 0xFFFF00, name);
    } else {
        printf("LOADER: Executing module_init() for %s...\n", 0x00FFCC, name);
        
        mod->init(); 
        
        printf("LOADER: Module %s successfully initialized!\n", 0x00FF00, name);
    }

    return mod;
}

kernel_module_t *load_module_from_file(const char *filepath) {
    printf("Fetching module from %s...\n", 0x00FFFF, filepath);

    vfs_node_t *mod_file = vfs_open(filepath, VFS_FLAG_READ);
    if (!mod_file) {
        printf("Failed to open %s\n", 0xFF0000, filepath);
        return NULL;
    }

    size_t file_size = mod_file->length;
    uint8_t *file_buffer = (uint8_t *)kmalloc(file_size);
    kernel_module_t *loaded_mod = NULL;

    if (file_buffer) {
        vfs_read(mod_file, file_buffer, file_size, 0);

        loaded_mod = load_module(filepath, file_buffer);

        kfree(file_buffer);
    } else {
        printf("Failed to allocate memory for reading %s\n", 0xFF0000, filepath);
    }

    vfs_close(mod_file);
    return loaded_mod;
}