
#ifndef NC_SERIAL_COM_H_
#define NC_SERIAL_COM_H_

#include "ke_main.h"
#include "typedefs.h"

int KAPI InitializeCom(int port);
void KAPI KeComWrite(int port, const char ch);
int KAPI KeComRead(int port);

#endif
