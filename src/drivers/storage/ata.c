/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "drivers/storage/ata.h"
#include "drivers/fb.h"

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

#define ATA_POLL_LIMIT 1000000U

/* 400ns (nano-second) delay */
static void ata_io_wait(void) {
    inb(ATA_PRIMARY_ALT_STAT);
    inb(ATA_PRIMARY_ALT_STAT);
    inb(ATA_PRIMARY_ALT_STAT);
    inb(ATA_PRIMARY_ALT_STAT);
}

static int ata_wait_ready(void) {
    for (uint32_t i = 0; i < ATA_POLL_LIMIT; i++) {
        uint8_t status = inb(ATA_PRIMARY_COMMAND_STAT);

        if (status == 0 || (status & (ATA_SR_ERR | ATA_SR_DF))) return 0;
        if (!(status & ATA_SR_BSY)) return 1;
    }

    return 0;
}

static int ata_wait_drq(void) {
    for (uint32_t i = 0; i < ATA_POLL_LIMIT; i++) {
        uint8_t status = inb(ATA_PRIMARY_COMMAND_STAT);
        if (status == 0 || (status & (ATA_SR_ERR | ATA_SR_DF))) return 0;
        if (status & ATA_SR_DRQ) return 1; /* Data ready */
    }

    return 0;
}

int ata_read_sector28(uint32_t lba, uint8_t *buffer)  {
    if (!buffer || !ata_wait_ready()) return 0;

    /* Select Drive 0 (Master) and set top to 4 bits of LBA */
    outb(ATA_PRIMARY_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    ata_io_wait();

    outb(ATA_PRIMARY_ERR_FEATURES, 0x00);
    outb(ATA_PRIMARY_SECT_COUNT, 1); /* Read 1 sector */
    outb(ATA_PRIMARY_LBA_LOW,  (uint8_t)(lba & 0xFF));
    outb(ATA_PRIMARY_LBA_MID,  (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_PRIMARY_LBA_HIGH, (uint8_t)((lba >> 16) & 0xFF));

    /* Send read command */
    outb(ATA_PRIMARY_COMMAND_STAT, ATA_CMD_READ_SECTORS);
    ata_io_wait();

    if (!ata_wait_drq()) {
        printf("ATA: Error reading sector %d\n", 0xFF0000, lba);
        return 0;
    }

    uint16_t *ptr = (uint16_t *)buffer;
    for (int i = 0; i < 256; i++) {
        ptr[i] = inw(ATA_PRIMARY_DATA);
    }

    return 1;
}

int ata_block_read(block_dev_t *dev, uint64_t lba, uint32_t count, void *buf) {
    (void)dev;
    uint8_t *ptr = (uint8_t *)buf;
    for (uint32_t i = 0; i < count; i++) {
        if (!ata_read_sector28((uint32_t)(lba + i), ptr + (i * 512))) {
            return 0;
        }
    }
    return 1;
}

void ata_init(void) {
    printf("ATA: Initializing Primary ATA Controller...\n", 0x00FF00);

    /* Register raw drive as "sda" */
    block_dev_t sda = {
        .name = "sda",
        .sector_size = 512,
        .total_sectors = 131072, /* 64MB / 512 bytes */
        .start_lba = 0,
        .read = ata_block_read,
        .write = NULL,
        .priv_data = NULL
    };
    register_block_device(&sda);
}
