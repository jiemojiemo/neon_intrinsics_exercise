cmake_minimum_required(VERSION 3.15)

project(android_neon_test)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Release)

set(benchmark_header_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../../")
set(x_header_dir "${benchmark_header_dir}/xsimd/include")
add_library(benchmark SHARED cpp/jni_run_benchmark.cpp)
target_include_directories(benchmark PRIVATE ${benchmark_header_dir} ${x_header_dir})

find_library(
        log-lib
        log )
target_link_libraries(benchmark PUBLIC ${log-lib})