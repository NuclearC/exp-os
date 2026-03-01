
#include "task.h"

#include "memory/memory.h"
#include "memory/pool.h"

static uint32_t task_counter;
static KPool task_pool;

void KAPI InitializeTasks(void) {
    KeCreatePool(&task_pool, sizeof(KTask) * MAX_TASKS);
    task_counter = 0;
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
