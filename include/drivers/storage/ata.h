/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef ATA_H
#define ATA_H

#include <stdint.h>
#include <stddef.h>
#include "block.h"

/* Primary bus ports */
#define ATA_PRIMARY_DATA         0x1F0
#define ATA_PRIMARY_ERR_FEATURES 0x1F1
#define ATA_PRIMARY_SECT_COUNT   0x1F2
#define ATA_PRIMARY_LBA_LOW      0x1F3
#define ATA_PRIMARY_LBA_MID      0x1F4
#define ATA_PRIMARY_LBA_HIGH     0x1F5
#define ATA_PRIMARY_DRIVE_SEL    0x1F6
#define ATA_PRIMARY_COMMAND_STAT 0x1F7
#define ATA_PRIMARY_ALT_STAT     0x3F6

/* Status register bits */
#define ATA_SR_ERR  (1 << 0) /* Error */
#define ATA_SR_DRQ  (1 << 3) /* Data Request Ready */
#define ATA_SR_SRV  (1 << 4) /* Service Request */
#define ATA_SR_DF   (1 << 5) /* Drive Fault */
#define ATA_SR_RDY  (1 << 6) /* Ready */
#define ATA_SR_BSY  (1 << 7) /* Busy */

/* Commands */
#define ATA_CMD_READ_SECTORS  0x20
#define ATA_CMD_WRITE_SECTORS 0x30
#define ATA_CMD_IDENTIFY      0xEC

void ata_init(void);
int ata_read_sector28(uint32_t lba, uint8_t *buffer);
int ata_write_sector28(uint32_t lba, const uint8_t *buffer);
int ata_block_read(block_dev_t *dev, uint64_t lba, uint32_t count, void *buf);

#endif
