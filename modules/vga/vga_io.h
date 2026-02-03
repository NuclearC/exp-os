

#ifndef NC_VGA_IO_H_
#define NC_VGA_IO_H_


/* returns 1 if the VGA module is active */
int VgaSupported(void);

/* write a string at the specified location on the screen */
int VgaWriteString(const char* str, int row, int column);


#endif 

