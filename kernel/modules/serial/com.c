
#include "com.h"

extern int _com_init(int);
extern void _com_write_byte(int, char);
extern int _com_read_byte(int);

int KAPI InitializeCom(int port) { return _com_init(port); }

void KAPI KeComWrite(int port, const char ch) { _com_write_byte(port, ch); }

int KAPI KeComRead(int port) { return _com_read_byte(port); }
