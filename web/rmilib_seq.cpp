#include <emscripten.h>
#include <emscripten/bind.h>
#include <vector>
#include <sstream>

#include "rmilib_shared.hpp"


EMSCRIPTEN_BINDINGS(module) {
    register_shared();

    register_tree<1>("KDTree");
    register_tree<2>("Quadtree");
    register_tree<3>("Octree");
}
