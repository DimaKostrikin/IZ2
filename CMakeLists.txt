cmake_minimum_required(VERSION 3.3)

set(TARGET_NAME IZ2.out)


project(IZ2.out)

set(CMAKE_C_STANDARD 11)

include_directories("${PROJECT_SOURCE_DIR}/include")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")


file(GLOB program_sources
    "${PROJECT_SOURCE_DIR}/include/IZ2/*.h"
    "${PROJECT_SOURCE_DIR}/src/IZ2/*.c"
    "${PROJECT_SOURCE_DIR}/test/*.test")

file(GLOB lib_sources
        "${PROJECT_SOURCE_DIR}/include/vectlib/*.h"
        "${PROJECT_SOURCE_DIR}/src/vectlib/*.c"
        "${PROJECT_SOURCE_DIR}/test/*.test")


add_library(vect STATIC ${lib_sources})

add_executable(IZ2.out ${program_sources})

target_link_libraries(${TARGET_NAME} PUBLIC vect)
target_link_libraries(${TARGET_NAME} PUBLIC m)
target_link_libraries(${TARGET_NAME} PUBLIC pthread)

set(COMPILE_FLAGS -Wall -Werror -Wpedantic)
#target_compile_options(${TARGET_NAME} PUBLIC ${COMPILE_FLAGS})

