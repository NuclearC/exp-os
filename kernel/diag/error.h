
#ifndef NC_KE_ERROR_H_
#define NC_KE_ERROR_H_

#include "ke_main.h"
#include "typedefs.h"
typedef struct KPACK {
    uint32_t gs, fs, es, ds;

    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp; // The value of ESP before the PUSHAD instruction
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
} Registers;

int KAPI KeShowHardError(int, const char *errormsg, const char *whatfailed,
                         Registers *regs);
#endif
