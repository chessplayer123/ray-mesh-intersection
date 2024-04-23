cmake_minimum_required(VERSION  3.5)
project(ray-mesh-intersection)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -O2 -std=c++17")
else()
    set(CMAKE_CXX_FLAGS "/std:c++17")
endif()

# Library for main application
include_directories(include/)
file(GLOB SOURCES
    src/utils/*.cpp
    src/meshes/*.cpp
    src/trees/*.cpp
    src/mesh_readers/*.cpp
    src/*.cpp
)
add_library(Library OBJECT ${SOURCES})

# Testing
include_directories(external/)
enable_testing()
add_subdirectory(test)
add_subdirectory(app)
