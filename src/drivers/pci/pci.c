/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "drivers/pci/pci.h"
#include "drivers/fb.h"

static inline void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

uint32_t pci_read32(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset) {
    uint32_t address = (uint32_t)((bus << 16) | (device << 11) |
        (func << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

void pci_write32(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint32_t value) {
    uint32_t address = (uint32_t)((bus << 16) | (device << 11) |
        (func << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    outl(PCI_CONFIG_ADDRESS, address);
    outl(PCI_CONFIG_DATA, value);
}

int pci_find_ahci(pci_device_t *out_dev) {
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t dev = 0; dev < 32; dev++) {
            for (uint8_t func = 0; func < 8; func++) {
                uint32_t vendor_device = pci_read32((uint8_t)bus, dev, func, PCI_REG_VENDOR_ID);
                uint16_t vendor = vendor_device & 0xFFFF;

                /* No device present */
                if (vendor == 0xFFFF) continue;

                uint32_t class_info = pci_read32((uint8_t)bus, dev, func, 0x08);
                uint8_t class_code = (class_info >> 24) & 0xFF;
                uint8_t subclass   = (class_info >> 16) & 0xFF;
                uint8_t prog_if    = (class_info >> 8) & 0xFF;

                if (class_code == 0x01 && subclass == 0x06 && prog_if == 0x01) {
                    out_dev->bus = (uint8_t)bus;
                    out_dev->device = dev;
                    out_dev->function = func;
                    out_dev->vendor_id = vendor;
                    out_dev->device_id = (vendor_device >> 16) & 0xFFFF;
                    out_dev->class_code = class_code;
                    out_dev->subclass = subclass;
                    out_dev->prog_if = prog_if;

                    /* Read BAR5 MMIO Address */
                    uint32_t bar5_low = pci_read32((uint8_t)bus, dev, func, PCI_REG_BAR5);
                    out_dev->bar5 = bar5_low & 0xFFFFFFF0; /* Mask out PCI flags */

                    /* Enable Bus Mastering & Memory Space in PCI Command Register */
                    uint32_t cmd = pci_read32((uint8_t)bus, dev, func, PCI_REG_BAR5);
                    cmd |= (1 << 1) | (1 << 2); /* Bit 1: Memory Space, Bit 2: Bus Master */
                    pci_write32((uint8_t)bus, dev, func, PCI_REG_COMMAND, cmd);

                    return 1;
                }
            }
        }
    }
    return 0;
}
