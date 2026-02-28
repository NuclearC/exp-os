
#include "print.h"
#include "modules/serial/com.h"
#include "modules/vga/vga_text.h"

#include <stdarg.h>

static void KPRIV PrintChar(const char ch) {
    VgaTextWriteChar(ch, 0x07);
    KeComWrite(0, ch);
}

static void KPRIV PrintInt(uint32_t i) {
    uint32_t mx = 1000000000;
    int first = 0;
    while (mx > 0) {
        uint32_t dg = i / mx;
        if (dg > 0 || first || (i < 10 && mx == 1)) {
            char ch = (dg + '0');
            PrintChar(ch);
            first = 1;
        }

        i %= mx;
        mx /= 10;
    }
}

int KAPI KePrint(const char *string, ...) {
    va_list args;
    va_start(args, string);
    for (; *string != 0; string++) {
        const char ch = *string;
        if (ch == '%') {
            const char dec = *(++string);
            switch (dec) {
            case 'd':
                PrintInt(va_arg(args, int));
                break;
            case 'u':
                PrintInt(va_arg(args, int));
                break;
            case 'x': {
                uint32_t v = va_arg(args, uint32_t);
                for (int i = 7; i >= 0; i--) {
                    char ch = (char)((v >> (i * 4)) & 0xf);
                    if (ch < 10)
                        ch += '0';
                    else
                        ch = (ch - 10) + 'A';
                    PrintChar(ch);
                }
            } break;
            case 's': {
                const char *s = va_arg(args, const char *);
                while (*s != 0)
                    PrintChar(*(s++));
            } break;
            }

        } else {
            PrintChar(ch);
        }
    }
    va_end(args);
    return 0;
}
