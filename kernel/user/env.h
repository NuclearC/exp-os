
#ifndef NC_USER_ENV_H_
#define NC_USER_ENV_H_

#include "ke_main.h"

typedef struct {

} KTaskEnvironment;

typedef struct {

} KGlobalEnvironment;

int KAPI KeCreateTaskEnvironment(KTaskEnvironment *env);
void KAPI KeDestroyTaskEnvironment(const KTaskEnvironment *env);

void KAPI InitializeGlobalEnvironment(void);

#endif
