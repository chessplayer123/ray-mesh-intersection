set(COMMON_FLAGS "--bind -O2 -Wl,--no-entry -sASSERTIONS -std=c++17 -sNO_DISABLE_EXCEPTION_CATCHING")

file(GLOB header ${header_path}/*.hpp)
file(GLOB src ${src_path}/*.cpp)
add_library(rmilib_par_impl ${header} ${src})
target_compile_options(rmilib_par_impl PUBLIC "-pthread")
target_include_directories(
    rmilib_par_impl
    PUBLIC $<BUILD_INTERFACE:${RAY_MESH_INTERSECTION_SOURCE_DIR}/include/>
    PRIVATE ${header_path}
)


add_executable(rmilib_par rmilib_par.cpp)
target_compile_definitions(rmilib_par PRIVATE RMI_INCLUDE_POOL)
target_link_libraries(rmilib_par PRIVATE rmilib_par_impl)
set_target_properties(
    rmilib_par
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${RAY_MESH_INTERSECTION_SOURCE_DIR}/web/js"
    LINK_FLAGS "${COMMON_FLAGS} -sINITIAL_MEMORY=512MB -sUSE_PTHREADS -sPTHREAD_POOL_SIZE=8"
)

add_executable(rmilib_seq rmilib_seq.cpp)
target_link_libraries(rmilib_seq PRIVATE rmilib)
set_target_properties(
    rmilib_seq
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${RAY_MESH_INTERSECTION_SOURCE_DIR}/web/js"
    LINK_FLAGS "${COMMON_FLAGS} -sALLOW_MEMORY_GROWTH"
)

