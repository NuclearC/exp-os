
#include "print.h"
#include "modules/vga/vga_io.h"

int KAPI KePrint(const char* string, ...) {
    va_list args;
    va_start(args, string);
    int ret = KePrintColor(string, DEFAULT_TEXT_COLOR, args);
    va_end(args);
    return ret;
}


int KAPI KePrintColor(const char* string, int color, ...) {

    va_list args;
    va_start(args, color);

     

    va_end(args);

}


