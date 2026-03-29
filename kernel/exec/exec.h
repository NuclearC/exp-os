
#ifndef NC_EXEC_H_
#define NC_EXEC_H_

#include "ke_main.h"
#include "typedefs.h"

typedef struct _MappedExecSubImage {
    uintptr_t page_addr;
    uintptr_t real_addr;
    size_t length;
    int page_flags;
} MappedExecSubImage;

typedef struct {
    MappedExecSubImage *sub_images;
    int sub_image_count;

    uintptr_t entry_point;
} MappedExecImage;

#define EXC_ERR_INVLD 1
int KAPI LoadElfExecutable(int file_index, uintptr_t base_address,
                           int supervisor_mode, MappedExecImage *mapped_image);

void KAPI UnloadElfExecutable(MappedExecImage *mapped_image);

#endif
