
#ifndef NC_OBJECTS_OBJECT_H_
#define NC_OBJECTS_OBJECT_H_

#include "ke_main.h"
#include "typedefs.h"

#include "string/string.h"
#define MAX_OBJECTS 1024
#define KOBJECT_HANDLE_NULL 0
typedef struct {
    uintptr_t handle;
    uint32_t type;
    KString name;
} KObject;

typedef KObject *KObjectHandle;

void KAPI InitializeObjects(void);

int KAPI KeCreateObject(KObjectHandle *object, const char *name);
void KAPI KeDestroyObject(const KObjectHandle object);

#endif
