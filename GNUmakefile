CC := gcc
LD := ld
OBJCOPY := objcopy
AS := nasm

SRC_DIR := src
INITRAMFS_DIR := initramfs
SCRIPTS_DIR := scripts
BIN_DIR := bin
BUILD := build
ISO_DIR := $(BUILD)/iso

DISK_IMG := disk.img

FONT ?= $(BIN_DIR)/ter-u16n.bdf
FONT_BIN := $(BUILD)/font.bin
FONT_OBJ := $(BUILD)/font.o

INITRAMFS_TAR := $(BUILD)/initramfs.tar

CFLAGS := -ffreestanding -fno-stack-protector -fno-pic -m64 -mcmodel=kernel -mno-red-zone -mno-sse -mno-sse2 -mno-mmx -mno-80387 -O2 -Wall -Wextra -I$(SRC_DIR) -I$(SRC_DIR)/limine/include -Iinclude
ASFLAGS := -f elf64
LDFLAGS := -nostdlib -z max-page-size=0x1000 -T linker.ld

KERNEL := $(BUILD)/kernel.elf
ISO := $(BUILD)/miyabi.iso

LIMINE_BIN := limine

SRC := $(shell find $(SRC_DIR) -name "*.c")
ASM := $(shell find $(SRC_DIR) -name "*.asm")

OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD)/%.o,$(SRC)) \
       $(patsubst $(SRC_DIR)/%.asm,$(BUILD)/%.o,$(ASM))

all: $(ISO)

$(KERNEL): $(OBJ) $(FONT_OBJ)
	mkdir -p $(BUILD)
	$(LD) $(OBJ) $(FONT_OBJ) -o $@ $(LDFLAGS)

$(FONT_BIN): $(FONT) $(SCRIPTS_DIR)/font2bin.py
	mkdir -p $(BUILD)
	python3 $(SCRIPTS_DIR)/font2bin.py $(FONT) $(FONT_BIN)

$(FONT_OBJ): $(FONT_BIN)
	cd $(BUILD) && $(OBJCOPY) -I binary -O elf64-x86-64 -B i386:x86-64 font.bin font.o

$(INITRAMFS_TAR):
	mkdir -p $(INITRAMFS_DIR)/dev
	touch $(INITRAMFS_DIR)/dev/.keep
	mkdir -p $(BUILD)
	tar --transform='s|^\./||' -cf $@ -C $(INITRAMFS_DIR) .

$(BUILD)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SRC_DIR)/%.asm
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(ISO_DIR): $(KERNEL) $(INITRAMFS_TAR)
	rm -rf $(ISO_DIR)
	mkdir -p $(ISO_DIR)/boot
	mkdir -p $(ISO_DIR)/EFI/BOOT
	cp $(KERNEL) $(ISO_DIR)/boot/
	cp $(INITRAMFS_TAR) $(ISO_DIR)/boot/
	cp /usr/share/limine/BOOTX64.EFI $(ISO_DIR)/EFI/BOOT/
	cp /usr/share/limine/limine-bios.sys $(ISO_DIR)/
	cp /usr/share/limine/limine-bios-cd.bin $(ISO_DIR)/
	cp /usr/share/limine/limine-uefi-cd.bin $(ISO_DIR)/
	cp $(SRC_DIR)/limine.conf $(ISO_DIR)/limine.conf

$(ISO): $(ISO_DIR)
	xorriso -as mkisofs \
		-b limine-bios-cd.bin \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		--efi-boot limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image \
		--protective-msdos-label \
		$(ISO_DIR) -o $(ISO)
	$(LIMINE_BIN) bios-install $(ISO)

$(DISK_IMG):
	mkdir -p $(BUILD)
	dd if=/dev/zero of=$(DISK_IMG) bs=1M count=64
	(echo o; echo n; echo p; echo 1; echo 2048; echo +32M; echo t; echo 83; echo w) | fdisk $(DISK_IMG)
	printf "MIYABI_PART1_TEST_SIGNATURE" | dd of=$(DISK_IMG) bs=512 seek=2048 conv=notrunc

run: $(ISO) $(DISK_IMG)
	qemu-system-x86_64 -d int,cpu_reset -D qemu.log \
	-drive if=pflash,format=raw,unit=0,file=/usr/share/ovmf/x64/OVMF.4m.fd,readonly=on \
	-drive file=$(DISK_IMG),format=raw,if=ide,bus=0,unit=0,media=disk \
	-drive file=$(ISO),format=raw,if=ide,bus=1,unit=0,media=cdrom

clean:
	rm -rf $(BUILD)

.PHONY: all run clean
