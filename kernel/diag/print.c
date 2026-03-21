
#include "print.h"

#include <stdarg.h>

extern int _com_init(uint64_t);
extern void _com_write_byte(uint64_t, char);
extern int _com_read_byte(int);

static void PrintChar(const char ch) { _com_write_byte(0, ch); }

static void PrintInt(uint32_t i) {
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

void KAPI InitializeDiagnostics(void) { _com_init(0); }

int KAPI Print(const char *string, ...) {
    va_list args;
    va_start(args, string);
    for (; *string != 0; string++) {
        const char ch = *string;
        if (ch == '%') {
            const char dec = *(++string);
            switch (dec) {
            case 'd':
                PrintInt(va_arg(args, uint64_t));
                break;
            case 'u':
                PrintInt(va_arg(args, uint64_t));
                break;
            case 'x': {
                uint64_t v = va_arg(args, uint64_t);
                for (int i = 15; i >= 0; i--) {
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
