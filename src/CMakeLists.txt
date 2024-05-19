set(header_path "${RAY_MESH_INTERSECTION_SOURCE_DIR}/include/rmilib")

file(GLOB header ${header_path}/*.hpp)
file(GLOB src *.cpp)

add_library(rmilib ${header} ${src})
target_include_directories(
    rmilib
    PUBLIC $<BUILD_INTERFACE:${RAY_MESH_INTERSECTION_SOURCE_DIR}/include/>
    PRIVATE ${header_path}
)

set(BACKEND "none" CACHE STRING "Backend for parallel evaluation")

message(STATUS "Using ${BACKEND} for parallel execution")
if (${BACKEND} STREQUAL "POOL")
    target_link_libraries(rmilib PUBLIC atomic)
    set(BACKEND 1)
elseif(${BACKEND} STREQUAL "OPENMP")
    find_package(OpenMP REQUIRED)
    target_link_libraries(rmilib PUBLIC OpenMP::OpenMP_CXX)
    set(BACKEND 2)
else()
    set(BACKEND 0)
endif()

target_compile_definitions(rmilib PRIVATE
    RMI_BACKEND_NONE=0
    RMI_BACKEND_POOL=1
    RMI_BACKEND_OPENMP=2
    RMI_BACKEND=${BACKEND}
)
