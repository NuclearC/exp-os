

#ifndef NC_VGA_IO_H_
#define NC_VGA_IO_H_

#include "ke_main.h" 
#include "typedefs.h"

#define VGA_TEXT_BASE 0xb8000
#define VGA_TEXT_WIDTH 80 
#define VGA_TEXT_HEIGHT 25

/* returns 1 if the VGA module is active */
int KAPI VgaSupported(void);

/* write a string at the specified location on the screen */
int KAPI VgaWriteString(const char* str, int row, int column, int color);
int KAPI VgaWriteChar(const char ch, int row, int column, int color);

/* Clear the text mode screen */
void KAPI VgaClearString(void);

/* Change the cursor position */
void KAPI VgaCursor(int offset);

#endif 

