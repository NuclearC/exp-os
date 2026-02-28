

#include "io_object.h"
#include "memory/memory.h"
#include "pool.h"

static KPool io_object_pool;

void KAPI InitializeIoObjects(void) {
    KeCreatePool(&io_object_pool, MAX_OBJECTS * sizeof(KIoObject));
}
int KAPI KeCreateIoObject(KIoObjectHandle *io_object) {
    KObjectHandle obj_handle = KOBJECT_HANDLE_NULL;

    if (0 != KeCreateObject(&obj_handle, "io_object"))
        return 1;

    obj_handle->type = IO_OBJECT_TYPE;

    *io_object = KeTryAcquire(&io_object_pool, sizeof(KIoObjectHandle));

    if (*io_object == 0)
        return 1;

    for (int i = 0; i < IO_NUM_BUFFERS; i++) {
        KeCreateIoBuffer(&(*io_object)->io_buffers[i], IO_DEFAULT_BUFLEN);
    }

    return 0;
}

void KAPI KeDestroyIoObject(KIoObjectHandle io_object) {
    KeDestroyObject(io_object->object_handle);

    for (int i = 0; i < IO_NUM_BUFFERS; i++) {
        KeDestroyIoBuffer(&io_object->io_buffers[i]);
    }
}
int KAPI KeCreateIoBuffer(KIoBuffer *buffer, size_t length) {
    buffer->buf = KeAllocatePhysicalMemory(length, MEM_ALIGN);
    buffer->length = length;
    buffer->head = buffer->tail = 0;
    return 0;
}
void KAPI KeDestroyIoBuffer(KIoBuffer const *buffer) {
    KeDeallocatePhysicalMemory(buffer->buf);
}

size_t KAPI KeWriteIoObject(KIoObjectHandle io_object, int buffer_index,
                            const char *data, size_t length) {

    KIoBuffer *iobuf = &io_object->io_buffers[buffer_index];
    return KeWriteIoBuffer(iobuf, data, length);
}

size_t KAPI KeReadIoObject(KIoObjectHandle io_object, int buffer_index,
                           char *data, size_t max_length) {
    KIoBuffer *iobuf = &io_object->io_buffers[buffer_index];
    return KeReadIoBuffer(iobuf, data, max_length);
}

size_t KAPI KeWriteIoBuffer(KIoBuffer *buffer, const char *data,
                            size_t length) {

    size_t ret = 0;

    size_t avail = buffer->length - buffer->tail;
    size_t copy_length = length;
    if (copy_length > avail)
        copy_length = avail;

    KeMemoryCopy(buffer->buf + buffer->tail, data, copy_length);

    ret += copy_length;

    if (buffer->tail > 0 && copy_length < length) {
        copy_length = length - copy_length;
        if (copy_length > buffer->tail) {
            copy_length = buffer->tail;
        }
        KeMemoryCopy(buffer->buf, data + ret, copy_length);
    }

    buffer->tail = (buffer->tail + ret) % buffer->length;

    return ret;
}

size_t KAPI KeReadIoBuffer(KIoBuffer *buffer, char *data, size_t max_length) {
    size_t ret = 0;

    size_t copy_length = buffer->length - buffer->head;
    if (buffer->tail > buffer->head) {
        copy_length = buffer->tail - buffer->head;
        if (copy_length > max_length)
            copy_length = max_length;

        KeMemoryCopy(data, buffer->buf + buffer->head, copy_length);
        ret += copy_length;
    } else {
        copy_length = buffer->length - buffer->head;
        if (copy_length > max_length)
            copy_length = max_length;

        KeMemoryCopy(data, buffer->buf + buffer->head, copy_length);

        ret += copy_length;
        if (copy_length < max_length && buffer->tail > 0) {
            copy_length = buffer->tail;
            if (copy_length > max_length)
                copy_length = max_length;
            KeMemoryCopy(data + ret, buffer->buf, copy_length);
            ret += copy_length;
        }
    }

    buffer->head = (buffer->head + ret) % buffer->length;

    return ret;
}
