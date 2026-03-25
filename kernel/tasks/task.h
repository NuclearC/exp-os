
#ifndef NC_KE_TASK_H_
#define NC_KE_TASK_H_

#include "ke_main.h"
#include "typedefs.h"

#include "memory/paging.h"

typedef struct {

} TaskContext;

typedef struct {
    int id;
    uintptr_t entry;
} Task;

void KAPI InitializeTasks(void);

int KAPI CreateTask(uintptr_t entry, Task *task);

#endif
