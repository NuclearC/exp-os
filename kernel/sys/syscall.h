
#ifndef NC_KE_SYSCALL_H_
#define NC_KE_SYSCALL_H_

#include "ke_main.h"
#include "typedefs.h"

typedef struct KPACK {
    uint64_t rflags;
    uint64_t rbx, rsp, rbp, r12, r13, r14, r15;
} PrivilegeJumpContext;

void KAPI InitializeSyscalls(void);

void KNORET KAPI PrivilegeJumpExit(uintptr_t return_address,
                                   PrivilegeJumpContext const *context);
void KNORET KAPI PrivilegeJumpEntry(uintptr_t return_address,
                                    PrivilegeJumpContext const *context);

#endif
