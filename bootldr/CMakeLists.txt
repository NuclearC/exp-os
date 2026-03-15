

set(CMAKE_ASM_NASM_OBJECT_FORMAT bin)

enable_language(ASM_NASM)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE ASM_SOURCE_FILES ${SOURCE_DIR}/main.asm)

add_library(bootldr OBJECT ${ASM_SOURCE_FILES})

target_include_directories(bootldr PRIVATE 
    ${PROJECT_SOURCE_DIR}/ ${SOURCE_DIR}/
)

set_property(GLOBAL APPEND PROPERTY IMAGE_FILES -s $<TARGET_OBJECTS:bootldr>)

