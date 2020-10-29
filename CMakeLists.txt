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

file(GLOB lib_sources_p
        "${PROJECT_SOURCE_DIR}/include/vectlib/*.h"
        "${PROJECT_SOURCE_DIR}/src/vectlib_p/*.c"
        "${PROJECT_SOURCE_DIR}/test/*.test")

file(GLOB lib_sources_s
        "${PROJECT_SOURCE_DIR}/include/vectlib/*.h"
        "${PROJECT_SOURCE_DIR}/src/vectlib_s/*.c"
        "${PROJECT_SOURCE_DIR}/test/*.test")

add_executable(IZ2.out ${program_sources})

if (LIB_STATIC AND PARALLEL)
    add_library(vect_parallel_static STATIC ${lib_sources_p})
    target_link_libraries(${TARGET_NAME} PUBLIC vect_parallel_static)
endif()

if (LIB_SHARED AND PARALLEL)
    add_library(vect_parallel_shared SHARED ${lib_sources_p})
    target_link_libraries(${TARGET_NAME} PUBLIC vect_parallel_shared)
endif()

if (LIB_STATIC AND SEQUENCE)
    add_library(vect_sequence_static STATIC ${lib_sources_s})
    target_link_libraries(${TARGET_NAME} PUBLIC vect_sequence_static)
endif()

if (LIB_SHARED AND SEQUENCE)
    add_library(vect_sequence_shared SHARED ${lib_sources_s})
    target_link_libraries(${TARGET_NAME} PUBLIC vect_sequence_shared)
endif()

target_link_libraries(${TARGET_NAME} PUBLIC m)
target_link_libraries(${TARGET_NAME} PUBLIC pthread)

set(COMPILE_FLAGS -Wall -Werror -Wpedantic)
#target_compile_options(${TARGET_NAME} PUBLIC ${COMPILE_FLAGS})

