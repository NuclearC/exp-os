
#include "error.h"

#include "modules/vga/vga_io.h"
#include "typedefs.h"

#define BACK_COL 0x1
#define ERROR_COL (BACK_COL << 4) | 0xc
#define INFO_COL (BACK_COL << 4) | 0xf

typedef struct {
    uint16_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
} Registers;

char NibbleToChar(uint8_t n) {
    if (n < 10)
        return '0' + n;
    else
        return (n - 10) + 'A';
}

void WriteByte(uint8_t b, char *ptr) {
    *(ptr) = NibbleToChar(b >> 4);
    *(ptr + 1) = NibbleToChar(b & 0xf);
}

void WriteReg16(uint16_t r, int row, int col) {
    char buf[5];
    buf[4] = 0;
    WriteByte((r >> 16) & 0xff, buf);
    WriteByte(r & 0xff, buf + 2);
    VgaWriteString(buf, row, col, INFO_COL);
}

void WriteReg32(uint32_t r, int row, int col) {
    char buf[9];
    buf[8] = 0;
    WriteByte((r >> 24) & 0xff, buf);
    WriteByte((r >> 16) & 0xff, buf + 2);
    WriteByte((r >> 8) & 0xff, buf + 4);
    WriteByte(r & 0xff, buf + 6);
    VgaWriteString(buf, row, col, INFO_COL);
}

int KAPI KeShowHardError(int, const char *errormsg, const char *whatfailed,
                         void *regptr) {
    /*   for (int i = 0; i < 80 * 25; i++) {
           VgaWriteString(" ", (i / 80), (i % 80), BACK_COL << 4);
       }*/
    Registers *regs = (Registers *)regptr;

    VgaWriteString("Encountered an unrecoverable error :(", 4, 5, INFO_COL);
    VgaWriteString(errormsg, 5, 5, ERROR_COL);
    VgaWriteString("Failed: ", 7, 6, INFO_COL);
    VgaWriteString(whatfailed, 7, 16, INFO_COL);

    VgaWriteString("Reg dump: ", 9, 6, INFO_COL);

    VgaWriteString("DS: ", 9, 16, INFO_COL);
    WriteReg16(regs->ds, 9, 20);

    VgaWriteString("GS: ", 10, 16, INFO_COL);
    WriteReg16(regs->gs, 10, 20);

    VgaWriteString("ES: ", 11, 16, INFO_COL);
    WriteReg16(regs->es, 11, 20);

    VgaWriteString("FS: ", 12, 16, INFO_COL);
    WriteReg16(regs->fs, 12, 20);

    VgaWriteString("EAX: ", 9, 28, INFO_COL);
    WriteReg32(regs->eax, 9, 33);

    VgaWriteString("EBX: ", 10, 28, INFO_COL);
    WriteReg32(regs->ebx, 10, 33);

    VgaWriteString("ECX: ", 11, 28, INFO_COL);
    WriteReg32(regs->ecx, 11, 33);

    VgaWriteString("EDX: ", 12, 28, INFO_COL);
    WriteReg32(regs->edx, 12, 33);

    VgaWriteString("ESI: ", 13, 28, INFO_COL);
    WriteReg32(regs->esi, 13, 33);

    VgaWriteString("EDI: ", 14, 28, INFO_COL);
    WriteReg32(regs->edi, 14, 33);

    VgaWriteString("EBP: ", 15, 28, INFO_COL);
    WriteReg32(regs->ebp, 15, 33);

    VgaWriteString("ESP: ", 16, 28, INFO_COL);
    WriteReg32(regs->esp, 16, 33);

    return 0;
}
