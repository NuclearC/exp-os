
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)

enable_language(ASM_NASM)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE ASM_SOURCE_FILES ${PROJECT_SOURCE_DIR}/util/**.asm ${SOURCE_DIR}/**.asm)
file(GLOB_RECURSE C_SOURCE_FILES ${SOURCE_DIR}/**.c)

set(C_COMPILE_FLAGS -Wall -Wextra -Werror -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -fno-pie)

set(LINKER_SCRIPT ${SOURCE_DIR}/linker.ld)

set_property(SOURCE ${C_SOURCE_FILES} PROPERTY COMPILE_OPTIONS ${C_COMPILE_FLAGS} -m32)
set_property(SOURCE ${ASM_SOURCE_FILES} PROPERTY COMPILE_OPTIONS "-f elf32")

set(SOURCE_FILES ${ASM_SOURCE_FILES} ${C_SOURCE_FILES})

add_executable(kernel ${SOURCE_FILES})

target_link_options(kernel PRIVATE ${C_COMPILE_FLAGS} -T${LINKER_SCRIPT})
target_include_directories(kernel PRIVATE ${SOURCE_DIR} ${PROJECT_SOURCE_DIR}/ ${PROJECT_SOURCE_DIR}/util/)

set_target_properties(kernel PROPERTIES
  LINK_DEPENDS "${LINKER_SCRIPT}"
)

set_property(GLOBAL APPEND PROPERTY IMAGE_FILES $<TARGET_FILE:kernel>)

