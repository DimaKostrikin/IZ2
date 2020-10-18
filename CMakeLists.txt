cmake_minimum_required(VERSION 3.3)

set(${TARGET_NAME} IZ2.out)

project(IZ2.out)

set(CMAKE_C_STANDARD 99)

include_directories("${PROJECT_SOURCE_DIR}/include")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

file(GLOB sources
    "${PROJECT_SOURCE_DIR}/include/IZ2/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.c"
    "${PROJECT_SOURCE_DIR}/test/*.test")

add_executable(IZ2.out ${sources})
