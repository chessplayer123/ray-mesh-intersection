cmake_minimum_required(VERSION 3.29)
project(RAY_MESH_INTERSECTION)


set(header_path "${RAY_MESH_INTERSECTION_SOURCE_DIR}/include/rmilib")
set(src_path "${RAY_MESH_INTERSECTION_SOURCE_DIR}/src")


if (INCLUDE_OMP)
    find_package(OpenMP REQUIRED)
endif()


add_compile_options(
    "$<$<CXX_COMPILER_ID:GNU>:-Wall;-Wextra;-pedantic;-O2;-std=c++17>"
    "$<$<CXX_COMPILER_ID:Clang>:-Wall;-Wextra;-pedantic;-O2;-std=c++17>"
    "$<$<CXX_COMPILER_ID:MSVC>:/std:c++17;/O2;/EHsc>"
)


include_directories(external/)
add_subdirectory(src/)
if (BUILD_TESTS)
    message(STATUS "Include tests")
    enable_testing()
    add_subdirectory(test)
endif()
if (BUILD_WASM)
    message(STATUS "Include WASM")
    add_subdirectory(web)
endif()
