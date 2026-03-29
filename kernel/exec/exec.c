
#include "exec.h"

#include "elf/elf_loader.h"
#include "memory/memory.h"
#include "memory/paging.h"
#include "modules/filesystem/filesystem.h"
#include "tasks/task.h"

int KAPI LoadElfExecutable(int file_index, uintptr_t base_address,
                           int supervisor_mode, MappedExecImage *mapped_image) {

    LoadedFile loaded_file;
    if (KSUCCESS != LoadElf(file_index, &loaded_file)) {
        return EXC_ERR_INVLD;
    }

    MappedExecSubImage *subimages = 0;

    if (KSUCCESS != LoadProgramHeaders(&loaded_file, base_address,
                                       supervisor_mode, &subimages,
                                       &mapped_image->sub_image_count)) {
        return EXC_ERR_INVLD;
    }

    mapped_image->sub_images = subimages;
    mapped_image->entry_point = loaded_file.hdr.e_entry;

    return KSUCCESS;
}

void KAPI UnloadElfExecutable(MappedExecImage *mapped_image) {
    if (mapped_image != 0 && mapped_image->sub_images != 0 &&
        mapped_image->sub_image_count > 0) {

        for (int i = 0; i < mapped_image->sub_image_count; i++) {
            KeDeallocateMemory(mapped_image->sub_images[i].page_addr,
                               mapped_image->sub_images[i].length);
        }

        DeallocatePhysicalMemory(mapped_image->sub_images);
    }
}
