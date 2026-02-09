
#ifndef NC_KE_ERROR_H_
#define NC_KE_ERROR_H_

#include "ke_main.h"

int KAPI KeShowHardError(int, const char* errormsg, const char* whatfailed,
    void* regptr);


#endif

