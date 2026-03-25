

#include "pci_setup.h"
#include "memory/memory.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_MAX_DEVICES 128

static int device_count;
static PciDevice devices[PCI_MAX_DEVICES];

static inline void outl(uint16_t port, uint32_t val) {
    __asm__ volatile("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t val;
    __asm__ volatile("inl %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static uint32_t PciReadDword(uint8_t bus, uint8_t slot, uint8_t func,
                             uint8_t offset) {
    uint32_t address = 0x80000000 | ((uint32_t)bus << 16) |
                       ((uint32_t)slot << 11) | ((uint32_t)func << 8) |
                       (offset & 0xFC);
    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

void KAPI EnumeratePciDevices(void) {
    device_count = 0;
    KeZeroMemory(devices, sizeof(devices));

    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            uint32_t data = PciReadDword(bus, slot, 0, 0);

            uint16_t vendor = data & 0xffff;
            uint16_t device = (data >> 16) & 0xffff;
            if (device_count < PCI_MAX_DEVICES && vendor != 0xffff &&
                device != 0xffff) {
                devices[device_count].bus = bus;
                devices[device_count].slot = slot;
                devices[device_count].device_id = device;
                devices[device_count].vendor_id = vendor;
                device_count++;
            }
        }
    }
}
PciDevice *KAPI FindDevice(uint16_t vendor, uint16_t device) {
    for (int i = 0; i < device_count; i++) {
        if (devices[i].vendor_id == vendor && devices[i].device_id == device) {
            return &devices[i];
        }
    }
    return 0;
}

uint64_t KAPI PciReadBar(PciDevice *device) {
    uint32_t bar0 = PciReadDword(device->bus, device->slot, 0, 0x10);

    if ((bar0 >> 1) & 0x3) {
        uint32_t bar1 = PciReadDword(device->bus, device->slot, 0, 0x14);

        return ((uint64_t)bar0 & 0xfffffff0) | ((uint64_t)bar1) << 32;
    } else {
        return (bar0 & 0xfffffff0);
    }
}
