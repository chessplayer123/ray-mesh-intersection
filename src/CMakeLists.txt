file(GLOB header ${header_path}/*.hpp)
file(GLOB src ${src_path}/*.cpp)

add_library(rmilib ${header} ${src})
target_include_directories(
    rmilib
    PUBLIC $<BUILD_INTERFACE:${RAY_MESH_INTERSECTION_SOURCE_DIR}/include/>
    PRIVATE ${header_path}
)
