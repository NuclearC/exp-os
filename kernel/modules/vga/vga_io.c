

#include "vga_io.h"
#include "util/v_io.h"

int KAPI VgaSupported(void) { return 1; /* TODO */ }

void KAPI VgaCursor(int offset) { _v_cursor(offset); }

int KAPI VgaWriteString(const char *str, int row, int column, int color) {
    return _v_print(str, row, column, color);
}

int KAPI VgaWriteChar(const char ch, int row, int column, int color) {
    return _v_print_char(ch, row, column, color);
}

void KAPI VgaClearString(void) { _v_clear(); }
