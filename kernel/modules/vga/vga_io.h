

#ifndef NC_VGA_IO_H_
#define NC_VGA_IO_H_

#include "ke_main.h"
#include "typedefs.h"

/* returns 1 if the VGA module is active */
int KAPI VgaSupported(void);

/* write a string at the specified location on the screen */
int KAPI VgaWriteString(const char* str, int row, int column, int color);

int KAPI VgaClearString(void);

int KAPI VgaCursor(int offset);
/* print the string sequentially */
int KAPI VgaPrintString(const char* str, int color);

#endif 

