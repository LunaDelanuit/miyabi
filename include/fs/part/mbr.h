/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef MBR_H
#define MBR_H

#include <stdint.h>

/* MBR Partition Entry */
typedef struct __attribute__((packed)) {
    uint8_t boot_indicator; /* 0x80 = Active / Bootable, 0x00 = Inactive */
    uint8_t start_head;
    uint8_t start_sector_cylinder; /* Bits 0-5 sector, Bit 6-9 cylinder high */
    uint8_t start_cylinder_low;
    uint8_t partition_type; /* 0x83 = Linux (Ext2/3/4), 0x0C = FAT32 LBA, 0xEE = GPT Protective */
    uint8_t end_head;
    uint8_t end_sector_cylinder;
    uint8_t end_cylinder_low;
    uint32_t lba_start; /* First LBA sector of partition */
    uint32_t sector_count; /* Total sector count */
} mbr_entry_t;

typedef struct __attribute__((packed)) {
    uint8_t bootstrap[446];
    mbr_entry_t partitions[4];
    uint16_t signature; /* 0xAA55 */
} mbr_sector_t;

void mbr_parse(const uint8_t *sector_buffer);

#endif
