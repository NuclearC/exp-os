
#include "syscall.h"

#include "memory/segment.h"

extern void _sys_setup(const void *entry, uint64_t segments);

extern void _sys_entry(void);
extern KNORET void _sys_exit(uintptr_t, const void *);

void KAPI InitializeSyscalls(void) {
    uint64_t segments =
        ((uint64_t)SEG_KERNEL_CS << 32) | ((uint64_t)SEG_USER_CS << 48);
    _sys_setup(_sys_entry, segments);
}

void KNORET KAPI PrivilegeJumpExit(uintptr_t return_address,
                                   PrivilegeJumpContext const *context) {
    _sys_exit(return_address, context);
}

void KNORET KAPI PrivilegeJumpEntry(uintptr_t return_address,
                                    PrivilegeJumpContext const *context) {
    /* todo: some shit */
    PrivilegeJumpExit(return_address, context);
}
