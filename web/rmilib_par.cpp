#include <emscripten.h>
#include <emscripten/bind.h>
#include <vector>
#include <sstream>

#include "rmilib_shared.hpp"
#include "rmilib/parallel_algos.hpp"


template<int N>
std::vector<rmi::Vector3f> tree_par_intersects_ray(
    const rmi::Tree<WebGLMesh, N>& tree,
    const rmi::Ray<float>& ray,
    int threads_count
) {
    return rmi::parallel_intersects_pool(ray, tree, threads_count);
}


EMSCRIPTEN_BINDINGS(module) {
    define_shared();

    register_tree<1>("KDTree")
        .function("par_intersects", &tree_par_intersects_ray<1>);
    register_tree<2>("Quadtree")
        .function("par_intersects", &tree_par_intersects_ray<2>);
    register_tree<3>("Octree")
        .function("par_intersects", &tree_par_intersects_ray<3>);
}
