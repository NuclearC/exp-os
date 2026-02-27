
#ifndef NC_KE_USER_EXEC_H_
#define NC_KE_USER_EXEC_H_

#include "ke_main.h"
#include "typedefs.h"

#define USER_EXEC_FILE_INVALID 1
#define USER_EXEC_HDR_INVALID 2

int KAPI KeUserExecuteFile(const char *filename);
void KNORETURN KAPI KeUserJump(uintptr_t addr, uintptr_t stack_top);

#endif
