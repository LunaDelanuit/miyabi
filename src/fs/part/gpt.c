/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "fs/part/gpt.h"
#include "drivers/storage/ata.h"
#include "drivers/fb.h"

/* Standard Linux Filesystem Data Partition GUID:
    0FC63DAF-8483-4772-8E79-3D69D8477DE4 (no im not a robot) */
static const guid_t LINUX_DATA_GUID = {
    0x0FC63DAF, 0x8483, 0x4772, 0x8E, 0x79, {0x3D, 0x69, 0xD8, 0x47, 0x7D, 0xE4}
};

static int guid_equals(const guid_t *a, const guid_t *b) {
    if (a->time_low != b->time_low || a->time_mid != b->time_mid ||
        a->time_hi_and_version != b->time_hi_and_version ||
        a->clock_seq_hi_and_reserved != b->clock_seq_hi_and_reserved ||
        a->clock_seq_low != b->clock_seq_low) {
        return 0;
    }
    for (int i = 0; i < 6; i++) {
        if (a->node[i] != b->node[i]) return 0;
    }
    return 1;
}

int gpt_parse(void) {
    uint8_t sector_buf[512];

    /* Read LBA 1 (GPT Header) */
    if (!ata_read_sector28(1, sector_buf)) {
        printf("GPT: Failed to read LBA 1 header\n", 0xFF0000);
        return 0;
    }

    const gpt_header_t *header = (const gpt_header_t *)sector_buf;

    if (header->signature != GPT_SIGNATURE) {
        printf("GPT: Invalid signature: 0x%lx\n", 0xFF0000, header->signature);
        return 0;
    }

    if (header->header_size < 92 || header->header_size > sizeof(sector_buf) ||
        header->my_lba != 1 || header->num_partition_entries == 0 ||
        header->size_of_partition_entry != sizeof(gpt_entry_t) ||
        512 % header->size_of_partition_entry != 0) {
        printf("GPT: Invalid header layout\n", 0xFF0000);
        return 0;
    }

    printf("GPT: Valid GPT header detected\n", 0x00FF00);
    printf("  Entries: %d, Entry Size: %d bytes\n", 0x00FFFF,
           header->num_partition_entries, header->size_of_partition_entry);

    /* Read partition array starting at entry LBA (usually LBA 2) */
    uint64_t entry_lba = header->partition_entry_lba;
    uint32_t entries_per_sector = 512 / header->size_of_partition_entry;
    uint32_t parsed_entries = 0;

    while (parsed_entries < header->num_partition_entries) {
        if (!ata_read_sector28((uint32_t)entry_lba, sector_buf)) {
            printf("GPT: Failed reading partition array LBA %d\n", 0xFF0000, (uint32_t)entry_lba);
            return 0;
        }

        for (uint32_t i = 0; i < entries_per_sector && parsed_entries < header->num_partition_entries; i++) {
            const gpt_entry_t *entry = (const gpt_entry_t *)(sector_buf + (i * header->size_of_partition_entry));

            /* Unused entries have zeroed type GUIDs */
            if (entry->partition_type_guid.time_low == 0 && entry->partition_type_guid.time_mid == 0) {
                parsed_entries++;
                continue;
            }

            printf("  GPT Partition %d:\n", 0x00FFFF, parsed_entries + 1);
            printf("    Start LBA: %d\n", 0xFFFFFF, (uint32_t)entry->starting_lba);
            printf("    End LBA:   %d\n", 0xFFFFFF, (uint32_t)entry->ending_lba);

            if (guid_equals(&entry->partition_type_guid, &LINUX_DATA_GUID)) {
                printf("    Type: Linux Data Partition\n", 0x00FF00);
            }

            parsed_entries++;
        }

        entry_lba++;
    }

    return 1;
}
