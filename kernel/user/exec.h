
#ifndef NC_USER_EXEC_H_
#define NC_USER_EXEC_H_

#include "ke_main.h"
#include "typedefs.h"

#define USX_ERR_NOFILE 1
#define USX_ERR_INVLD 2
int KAPI UserExecuteFile(const char *filename);

#endif
