cmake_minimum_required(VERSION 3.21)
project(neon_intrinsics_exercise)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Rpass-missed=loop-vectorize")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/xsimd/include)

if (BUILD_ASAN)
    message(STATUS "build with ASAN")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif ()

add_executable(neon_intrinsics_exercise main.cpp)
