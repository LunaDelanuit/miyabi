/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef PCI_H
#define PCI_H

#include <stdint.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

/* Header offsets */
#define PCI_REG_VENDOR_ID 0x00
#define PCI_REG_DEVICE_ID 0x02
#define PCI_REG_COMMAND   0x04
#define PRI_REG_STATUS    0x06
#define PCI_REG_CLASS     0x0B
#define PCI_REG_SUBCLASS  0x0A
#define PCI_REG_PROGIF    0x09
#define PCI_REG_BAR5      0x24

typedef struct {
    uint8_t bus;
    uint8_t device;
    uint8_t function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t prog_if;
    uint64_t bar5;
} pci_device_t;

uint32_t pci_read32(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
void pci_write32(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint32_t value);
int pci_find_ahci(pci_device_t *out_dev);

#endif
