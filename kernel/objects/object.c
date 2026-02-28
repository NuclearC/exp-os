

#include "object.h"
#include "diag/print.h"
#include "memory/memory.h"
#include "pool.h"

static uintptr_t object_counter;
static KPool object_pool;

void KAPI InitializeObjects(void) {
    object_counter = 0;
    KeCreatePool(&object_pool, MAX_OBJECTS * sizeof(KObject));
}

int KAPI KeCreateObject(KObjectHandle *object, const char *name) {
    KObject *obj = KeTryAcquire(&object_pool, sizeof(KObject));
    if (obj == 0)
        return 1;

    obj->handle = (++object_counter);
    obj->type = 0;

    KePrint("created object %s \n", name);
    KeCreateString(&obj->name, name);

    *object = obj;

    return 0;
}

void KAPI KeDestroyObject(const KObjectHandle object) {
    if (KeHasPool(&object_pool, object)) {
        object->handle = 0;
        KeDestroyString(&object->name);
    }
}
