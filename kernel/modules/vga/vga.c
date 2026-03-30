
#include "vga.h"
#include "memory/memory.h"

extern void _vga_cursor_disable(void);
extern void _vga_cursor_enable(uint64_t cursor_start, uint64_t cursor_end);
extern void _vga_cursor_set(uint64_t offset);

#define VGA_TBASE (void *)0xb8000
#define VGA_TWIDTH 80
#define VGA_THEIGHT 25

static int cur_row;
static int cur_col;

static void TextScroll(int amount) {
    if (amount <= 0)
        return;
    const int row = VGA_TWIDTH * 2;
    for (int i = 1; i < VGA_THEIGHT; i++) {
        KeMemoryCopy((void *)(VGA_TBASE + ((i - 1) * row)),
                     (const void *)(VGA_TBASE + (i * row)), row);
    }
}

int KAPI VgaInitialize(void) {
    cur_row = 0;
    cur_col = 0;

    VgaTextClear(0x0f200f200f200f20);
    VgaTextCursorPosition(cur_row * VGA_TWIDTH + cur_col);
    VgaTextCursor(1, 13, 15);

    return KSUCCESS;
}

void KAPI VgaTextClear(uint64_t color) {
    uint8_t *vga_base = (uint8_t *)VGA_TBASE;
    KeSetMemory(vga_base, 2 * VGA_TWIDTH * VGA_THEIGHT, color);
}

void KAPI VgaTextWriteChar(char ch, int color) {
    if (ch == '\n') {
        cur_row += 1;
        cur_col = 0;
        return;
    } else if (ch == '\t') {
        cur_col = ALIGN(cur_col, 4);
        if (cur_col >= VGA_TWIDTH) {
            cur_col = 0;
            cur_row += 1;
        }
    }
    if (cur_col >= VGA_TWIDTH) {
        cur_col = 0;
        cur_row += 1;
    }

    if (cur_row >= VGA_THEIGHT - 1) {
        TextScroll(1);
        cur_row -= 1;
    }

    uint8_t *vga_base =
        (uint8_t *)VGA_TBASE + (cur_row * VGA_TWIDTH + cur_col) * 2;
    *(vga_base) = (uint8_t)ch;
    *(vga_base + 1) = (uint8_t)color;
    cur_col += 1;
}

void KAPI VgaTextWriteString(const char *string, int length, int color) {
    for (int i = 0; i < length; i++) {
        VgaTextWriteChar(string[i], color);
    }

    VgaTextCursorPosition(cur_row * VGA_TWIDTH + cur_col);
}

void KAPI VgaTextCursorPosition(int offset) { _vga_cursor_set(offset); }

void KAPI VgaTextCursor(int enabled, int cursor_start, int cursor_end) {
    if (!enabled) {
        _vga_cursor_disable();
    } else {
        _vga_cursor_enable(cursor_start, cursor_end);
    }
}
