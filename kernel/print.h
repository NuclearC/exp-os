
#ifndef NC_KE_PRINT_H_
#define NC_KE_PRINT_H_

#include "ke_main.h"
#include "typedefs.h"

#include <stdarg.h>

#define DEFAULT_TEXT_COLOR 0x7

int KAPI KePrint(const char* string, ...);
int KAPI KePrintColor(const char* string, int color, ...);

#endif

