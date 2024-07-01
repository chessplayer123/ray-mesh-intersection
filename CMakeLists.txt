cmake_minimum_required(VERSION 3.5)
project(RAY_MESH_INTERSECTION)


set(header_path "${RAY_MESH_INTERSECTION_SOURCE_DIR}/include/rmilib")
set(src_path "${RAY_MESH_INTERSECTION_SOURCE_DIR}/src")


set(CXX_MSVC_FLAGS /std:c++17 /O2 /EHsc)
set(CXX_GNU_FLAGS -Wall -Wextra -pedantic -O2 -std=c++17)


if (CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(${CXX_GNU_FLAGS})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        add_compile_options(${CXX_MSVC_FLAGS})
    elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
        add_compile_options(${CXX_GNU_FLAGS})
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(${CXX_MSVC_FLAGS})
endif()


if (INCLUDE_OMP)
    find_package(OpenMP REQUIRED)
endif()


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
