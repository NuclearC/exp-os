

set(CMAKE_ASM_NASM_OBJECT_FORMAT bin)

enable_language(ASM_NASM)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB SOURCE_FILES ${SOURCE_DIR}/*.asm)

add_library(loader OBJECT ${SOURCE_FILES})
target_include_directories(loader PRIVATE 
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/util/
)
set_property(GLOBAL APPEND PROPERTY IMAGE_FILES -s $<TARGET_OBJECTS:loader>)

