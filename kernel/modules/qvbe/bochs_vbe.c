

#include "bochs_vbe.h"
#include "memory/paging.h"
#include "pci/pci_setup.h"

#include "diag/print.h"

#define VBE_DISPI_IOPORT_INDEX 0x01ce
#define VBE_DISPI_IOPORT_DATA 0x01cf

#define VBE_DISPI_INDEX_ID (0)
#define VBE_DISPI_INDEX_XRES (1)
#define VBE_DISPI_INDEX_YRES (2)
#define VBE_DISPI_INDEX_BPP (3)
#define VBE_DISPI_INDEX_ENABLE (4)
#define VBE_DISPI_INDEX_BANK (5)
#define VBE_DISPI_INDEX_VIRT_WIDTH (6)
#define VBE_DISPI_INDEX_VIRT_HEIGHT (7)
#define VBE_DISPI_INDEX_X_OFFSET (8)
#define VBE_DISPI_INDEX_Y_OFFSET (9)

#define VBE_ALLOWED 0xB0C5

#define VBE_DISPI_LFB_ENABLED 0x40
#define VBE_DISPI_NOCLEARMEM 0x80
#define VBE_DISPI_ENABLED 1
#define VBE_DISPI_DISABLED 0

#define BGA_VENDOR_ID 0x1234
#define BGA_DEVICE_ID 0x1111

static uint64_t linear_fb_addr;

extern void _vbe_out(int16_t index, int16_t data);
extern int64_t _vbe_in(int16_t index);

static void DisableDispi(void) {
    _vbe_out(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
}

static void EnableDispi(int use_linear_fb, int clear_video_mem) {
    _vbe_out(VBE_DISPI_INDEX_ENABLE,
             VBE_DISPI_ENABLED | (use_linear_fb ? VBE_DISPI_LFB_ENABLED : 0) |
                 (clear_video_mem ? 0 : VBE_DISPI_NOCLEARMEM));
}

static void SetVideoMode(uint32_t width, uint32_t height, uint32_t depth) {

    _vbe_out(VBE_DISPI_INDEX_XRES, width);
    _vbe_out(VBE_DISPI_INDEX_YRES, height);
    _vbe_out(VBE_DISPI_INDEX_BPP, depth);
}

int KAPI InitializeVbe(void) {
    int64_t vbe_id = _vbe_in(VBE_DISPI_INDEX_ID);
    Print("VBE: %x \n", vbe_id);
    if (vbe_id != VBE_ALLOWED) {
        Print("invalid VBE value \n");
        return 1;
    }

    PciDevice *device = FindDevice(BGA_VENDOR_ID, BGA_DEVICE_ID);

    if (device == 0) {
        Print("cannot find BGA device \n");
        return 1;
    }

    linear_fb_addr = PciReadBar(device);
    Print("linear FB at %x\n", linear_fb_addr);

    uintptr_t pg_addr = 0xffffff0000000000 | linear_fb_addr;

    size_t len = 640 * 480 * 4;
    AllocatePages(0, linear_fb_addr, len, pg_addr,
                  PAGE_READWRITE | PAGE_NOCACHE | PAGE_WRITETHROUGH |
                      PAGE_ACCESSALL);

    for (uintptr_t i = 0; i < len; i += 0x1000) {
        InvalidatePage(pg_addr + i);
    }

    DisableDispi();

    SetVideoMode(640, 480, 32);

    EnableDispi(1, 0);
    for (int x = 0; x < 640; x++) {
        for (int y = 0; y < 480; y++) {
            uint32_t *addr =
                (uint32_t *)((uint8_t *)pg_addr + 4 * y * 640 + 4 * x);
            (*addr) = 0x00000022;
        }
    }

    return KSUCCESS;
}
