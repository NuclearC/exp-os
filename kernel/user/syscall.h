

#ifndef NC_USER_SYSCALL_H_
#define NC_USER_SYSCALL_H_

#include "ke_main.h"
#include "typedefs.h"

typedef struct KPACK {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
} SyscallRegisters;

int KAPI KeSyscall(SyscallRegisters *regs);
void KAPI InitializeSyscalls(void);

#endif
