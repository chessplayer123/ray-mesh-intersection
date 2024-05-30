set(wasm_interface )

set(wasm_src rmilib_interface.cpp)

add_executable(rmilib_wasm ${wasm_src})
target_link_libraries(rmilib_wasm PRIVATE rmilib)
set(LFLAGS "--bind -O2 -Wl,--no-entry -s ASSERTIONS -std=c++17 -s NO_DISABLE_EXCEPTION_CATCHING -s INITIAL_MEMORY=512MB")

if (INCLUDE_POOL)
    set(LFLAGS "${LFLAGS} -s USE_PTHREADS -s PTHREAD_POOL_SIZE=8")
    target_compile_definitions(rmilib_wasm PRIVATE RMI_INCLUDE_POOL)
endif()

set_target_properties(
    rmilib_wasm
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${RAY_MESH_INTERSECTION_SOURCE_DIR}/web/js"
    LINK_FLAGS "${LFLAGS}"
)
