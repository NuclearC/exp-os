set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)

enable_language(ASM_NASM)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE ASM_SOURCE_FILES ${SOURCE_DIR}/*.asm)
file(GLOB_RECURSE C_SOURCE_FILES ${SOURCE_DIR}/*.c)

set(C_COMPILE_FLAGS -Wall -Wextra -Werror -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -m32)

set(LINKER_SCRIPT ${SOURCE_DIR}/linker.ld)

set_property(SOURCE ${C_SOURCE_FILES} PROPERTY COMPILE_OPTIONS ${C_COMPILE_FLAGS})
set_property(SOURCE ${ASM_SOURCE_FILES} PROPERTY COMPILE_OPTIONS "-f elf32")

set(SOURCE_FILES ${ASM_SOURCE_FILES} ${C_SOURCE_FILES})

add_executable(shell ${SOURCE_FILES})

target_link_options(shell PRIVATE ${C_COMPILE_FLAGS})
target_include_directories(shell PRIVATE ${SOURCE_DIR} ${PROJECT_SOURCE_DIR}/ ${PROJECT_SOURCE_DIR}/util/)

set_target_properties(shell PROPERTIES
  LINK_DEPENDS "${LINKER_SCRIPT}"
)

set_property(GLOBAL APPEND PROPERTY IMAGE_FILES $<TARGET_FILE:shell>)

