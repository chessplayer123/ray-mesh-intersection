cmake_minimum_required(VERSION  3.5)
project(ray-mesh-intersection)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -O2 -std=c++17")
endif()

# Library for 
include_directories(include/)
file(GLOB SOURCES
    src/utils/*.cpp
    src/meshes/*.cpp
    src/k_d_tree/*.cpp
    src/*.cpp
)
add_library(Library OBJECT ${SOURCES})

# Main executable
add_executable(App apps/main.cpp)
target_link_libraries(App PRIVATE Library)

# Testing
include_directories(external/)
enable_testing()
add_subdirectory(test)
