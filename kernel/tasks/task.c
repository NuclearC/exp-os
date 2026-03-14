
#include "task.h"

#include "memory/memory.h"
#include "memory/pool.h"

static uint32_t task_counter;
static KPool task_pool;

static KTaskHandle kernel_task;

void KAPI InitializeTasks(void) {
    KeCreatePool(&task_pool, sizeof(KTask) * MAX_TASKS);
    task_counter = 0;

    KeCreateTask(&kernel_task, 0, 0, 0, 0);
}

size_t KAPI KeEnumerateTasks(KTaskHandle *tasks, size_t max_length) {
    size_t ret = 0;
    for (int i = 0; i < MAX_TASKS; i++) {
        KTask *cur_task = (KTask *)task_pool.data + i;
        if (cur_task->id != 0) {
            if (tasks != 0 && ret < max_length) {
                tasks[ret] = cur_task;
            }
            ret++;
        }
    }

    return ret;
}

int KAPI KeCreateTask(KTaskHandle *task, const KTaskImage *image,
                      size_t image_length, uintptr_t entry,
                      uintptr_t stack_top) {

    KTask *task_ptr = (KTask *)KeTryAcquire(&task_pool, sizeof(KTask));

    if (0 == task_ptr)
        return 1;

    *task = task_ptr;
    task_ptr->id = ++task_counter;

    if (image_length > MAX_TASK_IMAGES)
        image_length = MAX_TASK_IMAGES;

    task_ptr->image_count = image_length;
    KeMemoryCopy(task_ptr->images, image, image_length * sizeof(KTaskImage));
    task_ptr->entry = entry;
    task_ptr->stack_top = stack_top;

    return 0;
}

void KAPI KeDestroyTask(KTaskHandle task) {
    if (KeHasPool(&task_pool, task)) {
        KeMemoryZero(task, sizeof(KTask));
    }
}

void KAPI DestroyTasks(void) {
    KeDestroyPool(&task_pool);
    task_counter = 0;
}
