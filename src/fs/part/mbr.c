/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "fs/part/mbr.h"
#include "fs/part/gpt.h"
#include "drivers/fb.h"
#include "drivers/storage/ata.h"
#include "drivers/storage/block.h"

void mbr_parse(const uint8_t *sector_buffer) {
    uint16_t magic = *(uint16_t *)&sector_buffer[510];
    if (magic != 0xAA55) {
        printf("MBR: Invalid boot signature: 0x%x\n", 0xFF0000, magic);
        return;
    }

    const mbr_sector_t *mbr = (const mbr_sector_t *)sector_buffer;
    printf("MBR: Valid MBR signature 0x%x detected!\n", 0x00FF00, mbr->signature);

    for (int i = 0; i < 4; i++) {
        const mbr_entry_t *entry = &mbr->partitions[i];

        /* Skip unused partitions */
        if (entry->partition_type == 0x00) continue;

        printf("  Partition %d:\n", 0x00FFFF, i + 1);
        printf("    Bootable: %s\n", 0xFFFFFF, (entry->boot_indicator & 0x80) ? "Yes" : "No");
        printf("    Type: 0x%x ", 0xFFFFFF, entry->partition_type);
        if (entry->partition_type == 0x83) {
            printf("(Linux)\n", 0xFFFFFF);
        } else if (entry->partition_type == 0x8C) {
            printf("(FAT32 LBA)\n", 0xFFFFFF);
        } else if (entry->partition_type == 0xEE) {
            printf("(GPT Protective)\n", 0xFFFFFF);
        } else {
            printf("(Unknown)\n", 0xFF0000);
        }
        printf("    Start LBA: %d\n", 0xFFFFFF, entry->lba_start);
        printf("    Sectors: %d\n", 0xFFFFFF, entry->sector_count);

        /* Check if it's a Protective MBR indicating a GPT disk */
        if (entry->partition_type == 0xEE) {
            printf("MBR: Partition type 0xEE (GPT Protective MBR) detected.\n", 0xFFFF00);
            gpt_parse();
            return;
        }

        printf("  Partition %d: Type 0x%x, Start LBA: %d, Sectors: %d\n",
                       0x00FFFF, i + 1, entry->partition_type, entry->lba_start, entry->sector_count);

        block_dev_t part_dev = {
            .sector_size = 512,
            .total_sectors = entry->sector_count,
            .start_lba = entry->lba_start,
            .read = ata_block_read,
            .write = NULL,
            .priv_data = NULL
        };

        part_dev.name[0] = 's';
        part_dev.name[1] = 'd';
        part_dev.name[2] = 'a';
        part_dev.name[3] = '1' + i;
        part_dev.name[4] = '\0';

        register_block_device(&part_dev);
    }
}
