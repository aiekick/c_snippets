cmake_minimum_required(VERSION 3.17)
project(julia C)

set(CMAKE_C_STANDARD 99)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    include_directories(${OpenMP_INCLUDE_PATH})
    message("CMAKE_C_FLAGS : ${CMAKE_C_FLAGS}")
    message("CMAKE_EXE_LINKER_FLAGS : ${CMAKE_EXE_LINKER_FLAGS}")
    message("OpenMP_INCLUDE_PATH : ${OpenMP_INCLUDE_PATH}")
endif()

add_executable(julia main.c)

target_link_libraries(julia ${OpenMP_C_LIBRARIES})