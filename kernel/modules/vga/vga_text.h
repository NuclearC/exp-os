
#ifndef NC_VGA_VGA_TEXT_H_
#define NC_VGA_VGA_TEXT_H_

#include "vga_io.h"

#define VGA_WIDTH
#define VGA_HEIGHT


/* Write text in VGA text mode */
int KAPI VgaTextWrite(const char* str, int color);
int KAPI VgaTextWriteChar(const char ch, int color);

/* Scroll the screen the specified amount vetically (amount > 0 means amount lines up) */
int KAPI VgaTextScroll(int amount);

int KAPI VgaTextClear(void);

#endif

