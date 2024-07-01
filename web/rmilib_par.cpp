#include "rmilib_shared.hpp"


EMSCRIPTEN_BINDINGS(module) {
    register_shared()
        .function("poolIntersectsTree", &rmi::Ray<float>::pool_intersects<WebGLMesh>);
}
