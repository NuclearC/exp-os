
#ifndef NC_OBJECTS_IO_OBJECT_H_
#define NC_OBJECTS_IO_OBJECT_H_

#include "object.h"

#define IO_NUM_BUFFERS 2

#define IO_TXBUF 0
#define IO_RXBUF 1

#define IO_OBJECT_TYPE 1

#define IO_DEFAULT_BUFLEN 1024

/* ring buffer */
typedef struct {
    size_t length;
    char *buf;
    size_t head;
    size_t tail;
} KIoBuffer;

typedef struct {
    KObjectHandle object_handle;
    KIoBuffer io_buffers[IO_NUM_BUFFERS];
} KIoObject;

typedef KIoObject *KIoObjectHandle;

void KAPI InitializeIoObjects(void);

int KAPI KeCreateIoBuffer(KIoBuffer *buffer, size_t length);
void KAPI KeDestroyIoBuffer(KIoBuffer const *buffer);

int KAPI KeCreateIoObject(KIoObjectHandle *io_object);
void KAPI KeDestroyIoObject(KIoObjectHandle io_object);

size_t KAPI KeWriteIoObject(KIoObjectHandle io_object, int buffer_index,
                            const char *data, size_t length);
size_t KAPI KeWriteIoBuffer(KIoBuffer *buffer, const char *data, size_t length);
size_t KAPI KeReadIoObject(KIoObjectHandle io_object, int buffer_index,
                           char *data, size_t max_length);
size_t KAPI KeReadIoBuffer(KIoBuffer *buffer, char *data, size_t max_length);

#endif
