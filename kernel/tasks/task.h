

#ifndef NC_TASKS_TASK_H_
#define NC_TASKS_TASK_H_

#include "ke_main.h"
#include "typedefs.h"

#define MAX_TASK_IMAGES 16
#define MAX_TASKS 1024
#define TASK_NAME_LEN 16

typedef struct {
    uintptr_t begin;
    uintptr_t end;
} KTaskImage;

typedef struct {
    uint32_t id;
    size_t image_count;
    KTaskImage images[MAX_TASK_IMAGES];
    uintptr_t stack_top;
    uintptr_t entry;
} KTask;

typedef KTask *KTaskHandle;

void KAPI InitializeTasks(void);

int KAPI KeCreateTask(KTaskHandle *task, const KTaskImage *image,
                      size_t image_length, uintptr_t entry,
                      uintptr_t stack_top);
void KAPI KeDestroyTask(KTaskHandle task);

void KAPI DestroyTasks(void);

#endif
