set(wasm_interface )

set(wasm_src rmilib_interface.cpp)

add_executable(rmilib_wasm ${wasm_src})
target_link_libraries(rmilib_wasm PRIVATE rmilib)

set_target_properties(
    rmilib_wasm
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${RAY_MESH_INTERSECTION_SOURCE_DIR}/web/js"
    LINK_FLAGS "--bind -O2 -Wl,--no-entry -sASSERTIONS -std=c++17 -sNO_DISABLE_EXCEPTION_CATCHING -sALLOW_MEMORY_GROWTH"
)
