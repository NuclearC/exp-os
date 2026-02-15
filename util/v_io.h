
#ifndef NC_UTIL_V_IO_H_
#define NC_UTIL_V_IO_H_

extern void _v_cursor(int);
extern int _v_print(const char* string, int row, int column, int col);
extern int _v_print_char(const char ch, int row, int column, int col);
extern void _v_clear(void);

#endif 

