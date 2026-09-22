/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef GPT_H
#define GPT_H

#include <stdint.h>

#define GPT_SIGNATURE 0x5452415020494645ULL /* EFI PART" in little-endian */

typedef struct __attribute__((packed)) {
    uint32_t time_low;
    uint16_t time_mid;
    uint16_t time_hi_and_version;
    uint8_t  clock_seq_hi_and_reserved;
    uint8_t  clock_seq_low;
    uint8_t  node[6];
} guid_t;

typedef struct __attribute__((packed)) {
    uint64_t signature;          /* Must match GPT_SIGNATURE */
    uint32_t revision;
    uint32_t header_size;
    uint32_t header_crc32;
    uint32_t reserved;
    uint64_t my_lba;
    uint64_t alternate_lba;
    uint64_t first_usable_lba;
    uint64_t last_usable_lba;
    guid_t   disk_guid;
    uint64_t partition_entry_lba;
    uint32_t num_partition_entries;
    uint32_t size_of_partition_entry; /* Usually 128 bytes... usually... */
    uint32_t partition_entry_array_crc32;
} gpt_header_t;

typedef struct __attribute__((packed)) {
    guid_t   partition_type_guid;
    guid_t   unique_partition_guid;
    uint64_t starting_lba;
    uint64_t ending_lba;
    uint64_t attributes;
    uint16_t name[36];           /* UTF-16LE partition name */
} gpt_entry_t;

int gpt_parse(void);

#endif
