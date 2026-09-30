/* Ignore this file, I was testing weird gcc behaviour. */

typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef long int int64_t;
typedef unsigned char uint8_t;
typedef short unsigned int uint16_t;
typedef unsigned int uint32_t;
typedef long unsigned int uint64_t;
typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;
typedef long int int_least64_t;
typedef unsigned char uint_least8_t;
typedef short unsigned int uint_least16_t;
typedef unsigned int uint_least32_t;
typedef long unsigned int uint_least64_t;
typedef signed char int_fast8_t;
typedef long int int_fast16_t;
typedef long int int_fast32_t;
typedef long int int_fast64_t;
typedef unsigned char uint_fast8_t;
typedef long unsigned int uint_fast16_t;
typedef long unsigned int uint_fast32_t;
typedef long unsigned int uint_fast64_t;
typedef long int intptr_t;
typedef long unsigned int uintptr_t;
typedef long int intmax_t;
typedef long unsigned int uintmax_t;
void init_gdt(void);
typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed)) idt_entry_t;
typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idtr_t;
void init_idt(void);
void remap_pic(uint8_t offset1, uint8_t offset2);
void pic_send_eoi(uint8_t irq);
void pit_init(uint32_t frequency);
typedef long int ptrdiff_t;
typedef long unsigned int size_t;
typedef int wchar_t;
typedef struct {
  long long __max_align_ll __attribute__((__aligned__(__alignof__(long long))));
  long double __max_align_ld __attribute__((__aligned__(__alignof__(long double))));
} max_align_t;
  typedef __typeof__(nullptr) nullptr_t;
typedef enum {
    THREAD_STATE_READY,
    THREAD_STATE_RUNNING,
    THREAD_STATE_DEAD,
} thread_state_t;
typedef struct thread {
    void *rsp;
    uint64_t id;
    thread_state_t state;
    void *stack_bottom;
    struct thread *next;
} thread_t;
extern void context_switch(void **old_rsp, void *new_rsp);
void init_scheduler(void);
thread_t *kthread_create(void (*entry_point)(void));
void schedule(void);
void kthread_exit(void);
struct limine_uuid {
    uint32_t a;
    uint16_t b;
    uint16_t c;
    uint8_t d[8];
};
struct limine_file {
    uint64_t revision;
    void * address;
    uint64_t size;
    char * path;
    char * string;
    uint32_t media_type;
    uint32_t unused;
    uint32_t tftp_ip;
    uint32_t tftp_port;
    uint32_t partition_index;
    uint32_t mbr_disk_id;
    struct limine_uuid gpt_disk_uuid;
    struct limine_uuid gpt_part_uuid;
    struct limine_uuid part_uuid;
};
struct limine_bootloader_info_response {
    uint64_t revision;
    char * name;
    char * version;
};
struct limine_bootloader_info_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_bootloader_info_response * response;
};
struct limine_executable_cmdline_response {
    uint64_t revision;
    char * cmdline;
};
struct limine_executable_cmdline_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_executable_cmdline_response * response;
};
struct limine_firmware_type_response {
    uint64_t revision;
    uint64_t firmware_type;
};
struct limine_firmware_type_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_firmware_type_response * response;
};
struct limine_stack_size_response {
    uint64_t revision;
};
struct limine_stack_size_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_stack_size_response * response;
    uint64_t stack_size;
};
struct limine_hhdm_response {
    uint64_t revision;
    uint64_t offset;
};
struct limine_hhdm_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_hhdm_response * response;
};
struct limine_video_mode {
    uint64_t pitch;
    uint64_t width;
    uint64_t height;
    uint16_t bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
};
struct limine_framebuffer {
    void * address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
    uint8_t unused[7];
    uint64_t edid_size;
    void * edid;
    uint64_t mode_count;
    struct limine_video_mode ** modes;
};
struct limine_framebuffer_response {
    uint64_t revision;
    uint64_t framebuffer_count;
    struct limine_framebuffer ** framebuffers;
};
struct limine_framebuffer_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_framebuffer_response * response;
};
struct limine_flanterm_fb_init_params {
    uint32_t * canvas;
    uint64_t canvas_size;
    uint32_t ansi_colours[8];
    uint32_t ansi_bright_colours[8];
    uint32_t default_bg;
    uint32_t default_fg;
    uint32_t default_bg_bright;
    uint32_t default_fg_bright;
    void * font;
    uint64_t font_width;
    uint64_t font_height;
    uint64_t font_spacing;
    uint64_t font_scale_x;
    uint64_t font_scale_y;
    uint64_t margin;
    uint64_t rotation;
};
struct limine_flanterm_fb_init_params_response {
    uint64_t revision;
    uint64_t entry_count;
    struct limine_flanterm_fb_init_params ** entries;
};
struct limine_flanterm_fb_init_params_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_flanterm_fb_init_params_response * response;
};
struct limine_paging_mode_response {
    uint64_t revision;
    uint64_t mode;
};
struct limine_paging_mode_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_paging_mode_response * response;
    uint64_t mode;
    uint64_t max_mode;
    uint64_t min_mode;
};
struct limine_mp_info;
typedef void (*limine_goto_address)(struct limine_mp_info *);
struct limine_mp_info {
    uint32_t processor_id;
    uint32_t lapic_id;
    uint64_t reserved;
    limine_goto_address goto_address;
    uint64_t extra_argument;
};
struct limine_mp_response {
    uint64_t revision;
    uint32_t flags;
    uint32_t bsp_lapic_id;
    uint64_t cpu_count;
    struct limine_mp_info ** cpus;
};
struct limine_mp_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_mp_response * response;
    uint64_t flags;
};
struct limine_memmap_entry {
    uint64_t base;
    uint64_t length;
    uint64_t type;
};
struct limine_memmap_response {
    uint64_t revision;
    uint64_t entry_count;
    struct limine_memmap_entry ** entries;
};
struct limine_memmap_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_memmap_response * response;
};
typedef void (*limine_entry_point)(void);
struct limine_entry_point_response {
    uint64_t revision;
};
struct limine_entry_point_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_entry_point_response * response;
    limine_entry_point entry;
};
struct limine_executable_file_response {
    uint64_t revision;
    struct limine_file * executable_file;
};
struct limine_executable_file_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_executable_file_response * response;
};
struct limine_internal_module {
    const char * path;
    const char * string;
    uint64_t flags;
};
struct limine_module_response {
    uint64_t revision;
    uint64_t module_count;
    struct limine_file ** modules;
};
struct limine_module_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_module_response * response;
    uint64_t internal_module_count;
    struct limine_internal_module ** internal_modules;
};
struct limine_rsdp_response {
    uint64_t revision;
    void * address;
};
struct limine_rsdp_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_rsdp_response * response;
};
struct limine_smbios_response {
    uint64_t revision;
    void * entry_32;
    void * entry_64;
};
struct limine_smbios_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_smbios_response * response;
};
struct limine_efi_system_table_response {
    uint64_t revision;
    void * address;
};
struct limine_efi_system_table_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_efi_system_table_response * response;
};
struct limine_efi_memmap_response {
    uint64_t revision;
    void * memmap;
    uint64_t memmap_size;
    uint64_t desc_size;
    uint64_t desc_version;
};
struct limine_efi_memmap_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_efi_memmap_response * response;
};
struct limine_date_at_boot_response {
    uint64_t revision;
    int64_t timestamp;
};
struct limine_date_at_boot_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_date_at_boot_response * response;
};
struct limine_executable_address_response {
    uint64_t revision;
    uint64_t physical_base;
    uint64_t virtual_base;
};
struct limine_executable_address_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_executable_address_response * response;
};
struct limine_dtb_response {
    uint64_t revision;
    void * dtb_ptr;
};
struct limine_dtb_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_dtb_response * response;
};
struct limine_riscv_bsp_hartid_response {
    uint64_t revision;
    uint64_t bsp_hartid;
};
struct limine_riscv_bsp_hartid_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_riscv_bsp_hartid_response * response;
};
struct limine_bootloader_performance_response {
    uint64_t revision;
    uint64_t reset_usec;
    uint64_t init_usec;
    uint64_t exec_usec;
};
struct limine_bootloader_performance_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_bootloader_performance_response * response;
};
struct limine_keep_iommu_response {
    uint64_t revision;
};
struct limine_keep_iommu_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_keep_iommu_response * response;
};
extern bool DEBUG;
extern volatile struct limine_executable_cmdline_request cmdline_request;
void read_boot_cmdline(void);
size_t boot_module_count(void);
const char *boot_module_path(size_t index);
typedef struct {
    char name[64];
    uint8_t *memory_base;
    size_t memory_size;
    void (*init)(void);
    void (*cleanup)(void);
} kernel_module_t;
kernel_module_t *load_module(const char *name, uint8_t *file_buffer, size_t file_size);
kernel_module_t *load_module_from_file(const char *path);
void init_pmm(void);
void *pmm_alloc(void);
extern uint64_t hhdm_offset;
static inline uint64_t *phys_to_virt(uint64_t phys) {
    return (uint64_t *)(phys + hhdm_offset);
}
static inline void set_cr3(uint64_t val) {
    __asm__ volatile ("mov %0, %%cr3" :: "r"(val));
}
void init_vmm(void);
uint64_t vmm_create_user_pml4(void);
void vmm_switch_pml4(uint64_t pml4_phys);
void vmm_map_page(uint64_t virt, uint64_t phys, uint64_t flags);
void vmm_unmap_page(uint64_t virt);
uint64_t vmm_virt_to_phys(uint64_t virt);
void vmm_map_range(uint64_t virt, uint64_t phys, size_t size, uint64_t flags);
void vmm_unmap_range(uint64_t virt, size_t size);
typedef struct heap_block {
    size_t size;
    bool is_free;
    struct heap_block *next;
} heap_block_t;
void init_heap(void);
void *kmalloc(size_t size);
void kfree(void *ptr);
typedef struct {
    const char *name;
    uint64_t addr;
} kernel_symbol_t;
uint64_t ksym_lookup(const char *name);
typedef struct block_dev {
    char name[32];
    uint32_t sector_size;
    uint64_t total_sectors;
    uint64_t start_lba;
    int (*read)(struct block_dev *dev, uint64_t lba, uint32_t count, void *buf);
    int (*write)(struct block_dev *dev, uint64_t lba, uint32_t count, const void *buf);
    void *priv_data;
} block_dev_t;
void init_block_subsystem(void);
int register_block_device(block_dev_t *dev);
block_dev_t *get_block_device(const char *name);
int block_read(block_dev_t *dev, uint64_t lba, uint32_t count, void *buf);
int block_write(block_dev_t *dev, uint64_t lba, uint32_t count, const void *buf);
struct vfs_node;
typedef uint64_t (*read_type_t)(struct vfs_node*, uint8_t*, uint64_t, uint64_t);
typedef uint64_t (*write_type_t)(struct vfs_node*, uint8_t*, uint64_t, uint64_t);
typedef void (*open_type_t)(struct vfs_node*, uint32_t);
typedef void (*close_type_t)(struct vfs_node*);
typedef struct vfs_node* (*readdir_type_t)(struct vfs_node*, uint32_t);
typedef struct vfs_node* (*finddir_type_t)(struct vfs_node*, char *name);
typedef struct vfs_node {
    char name[128];
    uint32_t mask;
    uint32_t uid;
    uint32_t gid;
    uint32_t flags;
    uint32_t inode;
    uint32_t length;
    uint32_t impl;
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    struct vfs_node *ptr;
} vfs_node_t;
typedef struct vfs_fs_type {
    char name[32];
    vfs_node_t *(*mount)(block_dev_t *dev);
    struct vfs_fs_type *next;
} vfs_fs_type_t;
extern vfs_node_t *fs_root;
void vfs_mount(vfs_node_t *mountpoint, vfs_node_t *target);
vfs_node_t *vfs_get_node_by_path(const char *path);
vfs_node_t *vfs_open(const char *path, uint32_t flags);
uint64_t vfs_read(vfs_node_t *node, uint8_t *buffer, uint64_t size, uint64_t offset);
void vfs_close(vfs_node_t *node);
void vfs_register_filesystem(vfs_fs_type_t *fs);
vfs_fs_type_t *vfs_find_filesystem(const char *name);
int vfs_mount_by_name(const char *device_name, const char *mount_path, const char *fs_type_name);
extern volatile struct limine_framebuffer_request framebuffer_request;
int64_t sys_write(int fd, const void *buf, uint64_t count);
void init_fb(void);
void clear(uint32_t color);
void set_cursor(size_t x, size_t y);
void put_char(char c, uint32_t color);
void puts(const char *str, uint32_t color);
void print_hex64(uint64_t value, uint32_t color);
void printf(const char *fmt, uint32_t color, ...);
vfs_node_t *fb_create_vfs_node(void);
uint64_t fb_vfs_write(vfs_node_t *node, uint8_t *buffer, uint64_t size, uint64_t offset);
void ata_init(void);
int ata_read_sector28(uint32_t lba, uint8_t *buffer);
int ata_write_sector28(uint32_t lba, const uint8_t *buffer);
int ata_block_read(block_dev_t *dev, uint64_t lba, uint32_t count, void *buf);
typedef struct {
    vfs_node_t *node;
    uint64_t offset;
    int flags;
} file_descriptor_t;
void init_fds(void);
int fd_alloc(vfs_node_t *node);
vfs_node_t *fd_get_node(int fd);
void fd_close(int fd);
vfs_node_t *init_devfs(void);
void devfs_register(vfs_node_t *device);
int devfs_strcmp(const char *s1, const char *s2);
typedef struct __attribute__((packed)) {
    uint32_t inodes_count;
    uint32_t blocks_count;
    uint32_t reserved_blocks_count;
    uint32_t free_blocks_count;
    uint32_t free_inodes_count;
    uint32_t first_data_block;
    uint32_t log_block_size;
    int32_t log_fragment_size;
    uint32_t blocks_per_group;
    uint32_t fragments_per_group;
    uint32_t inodes_per_group;
    uint32_t mtime;
    uint32_t wtime;
    uint16_t mount_count;
    int16_t max_mount_count;
    uint16_t magic;
    uint16_t state;
    uint16_t errors;
    uint16_t minor_rev_level;
    uint32_t lastcheck;
    uint32_t checkinterval;
    uint32_t creator_os;
    uint32_t rev_level;
    uint16_t def_resuid;
    uint16_t def_resgid;
    uint32_t first_ino;
    uint16_t inode_size;
    uint16_t block_group_nr;
    uint32_t feature_compat;
    uint32_t feature_incompat;
    uint32_t feature_ro_compat;
} ext2_superblock_t;
typedef struct ext2_fs {
    block_dev_t *dev;
    uint32_t block_size;
    uint32_t inode_size;
    uint32_t blocks_count;
    uint32_t inodes_count;
    uint32_t blocks_per_group;
    uint32_t inodes_per_group;
    uint32_t group_count;
} ext2_fs_t;
void ext2_init(void);
vfs_node_t *init_initramfs(uint64_t ramfs_addr);
typedef struct __attribute__((packed)) {
    uint8_t boot_indicator;
    uint8_t start_head;
    uint8_t start_sector_cylinder;
    uint8_t start_cylinder_low;
    uint8_t partition_type;
    uint8_t end_head;
    uint8_t end_sector_cylinder;
    uint8_t end_cylinder_low;
    uint32_t lba_start;
    uint32_t sector_count;
} mbr_entry_t;
typedef struct __attribute__((packed)) {
    uint8_t bootstrap[446];
    mbr_entry_t partitions[4];
    uint16_t signature;
} mbr_sector_t;
void mbr_parse(const uint8_t *sector_buffer);
uint64_t elf_load(void *elf_binary, uint64_t user_pml4);
extern void switch_to_user_space(uint64_t user_stack, uint64_t user_func);
__attribute__((used, section(".requests")))
static volatile struct limine_module_request module_request = {
    .id = { 0xc7b1dd30df4c8b88, 0x0a82e883a194f07b, 0x3e7e279702be32af, 0xca1c4f3bd1280cee },
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
void test_block_abstraction(void) {
    block_dev_t *sda = get_block_device("sda");
    if (!sda) {
        printf("TEST: Failed to locate 'sda' device!\n", 0xFF0000);
        return;
    }
    uint8_t sector_buf[512];
    if (block_read(sda, 0, 1, sector_buf)) {
        printf("TEST: Successfully read sector 0 from 'sda'\n", 0x00FF00);
        mbr_parse(sector_buf);
    } else {
        printf("TEST: Failed to read sector 0 from 'sda'\n", 0xFF0000);
        return;
    }
    block_dev_t *sda1 = get_block_device("sda1");
    if (!sda1) {
        printf("TEST: Failed to locate 'sda1' partition device!\n", 0xFF0000);
        return;
    }
    printf("TEST: Successfully retrieved 'sda1'! Testing relative read...\n", 0x00FF00);
    uint8_t part_buf[512];
    if (block_read(sda1, 0, 1, part_buf)) {
        printf("TEST: Successfully read relative sector 0 from 'sda1'!\n", 0x00FF00);
    } else {
        printf("TEST: Relative sector read on 'sda1' failed!\n", 0xFF0000);
    }
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
    if (module_request.response != ((void *)0) && module_request.response->module_count > 0) {
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
        init_fds();
        printf("VFS: DevFS mounted to /dev successfully.\n", 0x00FF00);
    } else {
        printf("VFS: Could not find /dev in Initramfs!\n", 0xFF0000);
    }
    init_block_subsystem();
    ata_init();
    ext2_init();
    printf("\nWelcome to Miyabi ", 0xFFFFFF);
    printf("0.1\n", 0xFFFFFF);
    printf("Copyright (c) 2026 Luna Delanuit and contributers, ", 0xCC00DD);
    printf("GNU General Public License v3.0-or-later.\n\n", 0xFF2200);
    for (size_t i = 0; i < boot_module_count(); i++) {
        load_module_from_file(boot_module_path(i));
    }
    uint8_t sector_buf[512];
    if (ata_read_sector28(0, sector_buf)) {
        uint16_t magic = *(uint16_t *)&sector_buf[510];
        printf("Sector 0 Boot Signature: 0x%x\n", 0x00FFFF, magic);
        mbr_parse(sector_buf);
    }
    vfs_fs_type_t *ext2 = vfs_find_filesystem("ext2");
    if (ext2) {
        block_dev_t *sda1 = get_block_device("sda1");
        if (sda1) {
            ext2->mount(sda1);
        }
    }
    goto kend;
    vfs_node_t *test_node = vfs_open("/test", 0x01);
    if (!test_node) {
        printf("/test does not exist.\n", 0xFF0000);
    } else {
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
             vmm_map_page(user_stack_virtual, stack_phys, (1ULL << 0) | (1ULL << 2) | (1ULL << 1));
             uint8_t *stack_virt = (uint8_t *)phys_to_virt(stack_phys);
             for (int i = 0; i < 4096; i++) stack_virt[i] = 0;
             vmm_switch_pml4(kernel_pml4_phys);
             if (DEBUG) printf("Entering user space...\n", 0xAAAAFF);
             vmm_switch_pml4(user_pml4_phys);
             switch_to_user_space(user_stack_virtual + 3072, entry_point);
         }
    }
    kend:
    if (DEBUG) printf("Reached end of kernel.\n", 0xFF00FF);
    for (;;) __asm__ volatile("hlt");
}
