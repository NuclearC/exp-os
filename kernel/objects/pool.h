#ifndef NC_OBJECTS_POOL_H_
#define NC_OBJECTS_POOL_H_

#include "ke_main.h"
#include "typedefs.h"

typedef struct {
    size_t length;
    void *data;
} KPool;

int KAPI KeCreatePool(KPool *pool, size_t size);
void *KAPI KeTryAcquire(KPool *pool, size_t element_size);
int KAPI KeHasPool(KPool *pool, const void *object);
void KAPI KeDestroyPool(KPool const *pool);

#endif
