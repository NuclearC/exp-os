
#ifndef NC_MODULES_VGA_H_
#define NC_MODULES_VGA_H_

#include "kernel/ke_main.h"
#include "typedefs.h"

int KAPI VgaInitialize(void);

void KAPI VgaTextClear(uint64_t color);
void KAPI VgaTextWriteChar(char ch, int color);
void KAPI VgaTextWriteString(const char *string, int length, int color);
void KAPI VgaTextCursorPosition(int offset);
void KAPI VgaTextCursor(int enabled, int cursor_start, int cursor_end);

#endif
