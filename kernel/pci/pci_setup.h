
#ifndef NC_PCI_SETUP_H_
#define NC_PCI_SETUP_H_

#include "ke_main.h"
#include "typedefs.h"

typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t bus, slot;
} PciDevice;

void KAPI EnumeratePciDevices(void);
PciDevice *KAPI FindDevice(uint16_t vendor, uint16_t device);

uint64_t KAPI PciReadBar(PciDevice *device);

#endif
