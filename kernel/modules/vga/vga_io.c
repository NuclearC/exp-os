

#include "vga_io.h"
#include "util/v_io.h"


int current_print_row = 0;
int current_print_column = 0;

int KAPI VgaSupported(void) {
    return 1; /* TODO */
}

int KAPI VgaCursor(int offset) {
    _v_cursor(offset);
}

int KAPI VgaWriteString(const char* str, int row, int column, int color) {
    _v_print(str, row, column, color);
}

int KAPI VgaClearString(void) {
    current_print_row = 0;
    current_print_column = 0;
    _v_clear();
}

int KAPI VgaPrintString(const char* str, int color) {
    while (*str != 0) {
        if (*str == '\n') {
            current_print_row += 1;
            current_print_column = 0;
            str++;
        } else {
            char ch[2];
            ch[0] = *(str++);
            ch[1] = 0;
            VgaWriteString(ch, current_print_row, current_print_column, color);
            current_print_column++;
        }
    }
    VgaCursor(current_print_row * 80 + current_print_column);
    return 0;
}







