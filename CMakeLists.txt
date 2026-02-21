
cmake_minimum_required(VERSION 3.10)

project(OS)

define_property(GLOBAL PROPERTY IMAGE_FILES
    BRIEF_DOCS "List of the files embedded in the final image")

add_subdirectory(${PROJECT_SOURCE_DIR}/boot)
add_subdirectory(${PROJECT_SOURCE_DIR}/loader)
add_subdirectory(${PROJECT_SOURCE_DIR}/kernel)
add_subdirectory(${PROJECT_SOURCE_DIR}/shell)

get_property(DRV_ARGS GLOBAL PROPERTY IMAGE_FILES)


add_custom_target(bootimage ALL COMMAND python ${PROJECT_SOURCE_DIR}/drv/main.py ${DRV_ARGS} -o ${PROJECT_BUILD_DIR}bootimage)
add_dependencies(bootimage boot loader kernel)

