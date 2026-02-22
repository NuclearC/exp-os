
#include "vga_text.h"
#include "memory/memory.h"

static int current_print_row;
static int current_print_column;

int KAPI VgaTextScroll(int amount) {
    if (amount <= 0)
        return 0;
    const int row = VGA_TEXT_WIDTH * 2;
    for (int i = 1; i < VGA_TEXT_HEIGHT; i++) {
        KeMemoryCopy((void *)(VGA_TEXT_BASE + ((i - 1) * row)),
                     (const void *)(VGA_TEXT_BASE + (i * row)), row);
    }
    return 0;
}

int KAPI VgaTextWriteChar(const char ch, int color) {
    if (ch == '\n') {
        current_print_row += 1;
        if (current_print_row >= VGA_TEXT_HEIGHT - 1) {
            VgaTextScroll(1);
            current_print_row -= 1;
        }

        current_print_column = 0;
    } else {
        VgaWriteChar(ch, current_print_row, current_print_column, color);
        current_print_column++;
    }
    VgaCursor(current_print_row * VGA_TEXT_WIDTH + current_print_column);
    return 0;
}

int KAPI VgaTextWrite(const char *str, int color) {
    while (*str != 0) {
        VgaTextWriteChar(*(str++), color);
    }
    return 0;
}

int KAPI VgaTextClear(void) {
    current_print_row = 0;
    current_print_column = 0;
    VgaClearString();
    return 0;
}
