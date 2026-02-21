
#include "print.h"
#include "modules/vga/vga_text.h"

int KAPI KePrint(const char* string, ...) {
    va_list args;
    va_start(args, string);
    int ret = KePrintColor(string, DEFAULT_TEXT_COLOR, args);
    va_end(args);
    return ret;
}


int KAPI KePrintColor(const char* string, int color, va_list args) {
    for (; *string != 0; string++) {
        const char ch = *string;
        if (ch == '%') {
            const char dec = *(++string);
            switch (dec) {
            case 'd':
            case 'u': {
                int v = va_arg(args, int);
                int t = 1;
                while ((10 * t) <= v) {
                    t *= 10;
                }
                while (t > 0) {
                    char nch = '0' + (v / t);
                    VgaTextWriteChar(nch, color);
                    v %= t;
                    t /= 10;
                }
                } break;
            case 'x': {
                uint32_t v = va_arg(args, uint32_t);
                for (int i = 7; i >= 0; i--) {
                    char ch = (char)((v >> (i * 4)) & 0xf);
                    /* if (ch == 0 && (i != 0 && !first)) continue;
                    first = 1;
                    */
                    if (ch < 10) ch += '0';
                    else ch = (ch - 10) + 'A';
                    VgaTextWriteChar(ch, color);
                }
                } break;
            }
            
        } else {
            VgaTextWriteChar(ch, color);
        }   
    }

    return 0;
}


