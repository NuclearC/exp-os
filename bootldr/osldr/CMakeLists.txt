

enable_language(ASM_NASM)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LINKER_SCRIPT ${SOURCE_DIR}/linker.ld)

file(GLOB_RECURSE ASM_SOURCE_FILES ${SOURCE_DIR}/**.asm)
file(GLOB_RECURSE C_SOURCE_FILES ${SOURCE_DIR}/**.c)

add_executable(osldr ${C_SOURCE_FILES} ${ASM_SOURCE_FILES})

set(C_COMPILE_FLAGS -Wall -Wextra -Werror -ffreestanding -nostdlib -fno-builtin -fno-stack-protector)

set(LINKER_SCRIPT ${SOURCE_DIR}/linker.ld)

set_property(SOURCE ${C_SOURCE_FILES} PROPERTY COMPILE_OPTIONS ${C_COMPILE_FLAGS})
set_property(SOURCE ${ASM_SOURCE_FILES} PROPERTY COMPILE_OPTIONS "-f elf64")

target_link_options(osldr PRIVATE ${C_COMPILE_FLAGS} -T${LINKER_SCRIPT})
target_include_directories(osldr PRIVATE ${SOURCE_DIR} ${PROJECT_SOURCE_DIR}/bootldr ${PROJECT_SOURCE_DIR})

set_target_properties(osldr PROPERTIES
  LINK_DEPENDS "${LINKER_SCRIPT}"
)

set_property(GLOBAL APPEND PROPERTY IMAGE_FILES $<TARGET_FILE:osldr>)


