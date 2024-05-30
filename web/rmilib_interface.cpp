#include <emscripten.h>
#include <emscripten/bind.h>
#include <vector>
#include <sstream>

#include "rmilib/ray.hpp"
#include "rmilib/vector.hpp"
#include "rmilib/mesh.hpp"
#include "rmilib/reader.hpp"
#include "rmilib/parallel_algos.hpp"

WebGLMesh read_mesh_from_string(const std::string& filename, const std::string& data) {
    DataFormat format = define_format(filename);
    std::istringstream stream(data);
    switch (format) {
        case DataFormat::Ply: return read_raw_triangular_mesh_ply<float, unsigned int>(stream);
        case DataFormat::Stl: return read_raw_triangular_mesh_stl<float, unsigned int>(stream);
        case DataFormat::Obj: return read_raw_triangular_mesh_obj<float, unsigned int>(stream);
    }
}

template<int N>
Tree<WebGLMesh, N> build_tree(WebGLMesh& mesh) {
    return Tree<WebGLMesh, N>::for_mesh(mesh.begin(), mesh.end());
}

template<int N>
std::vector<Vector3f> ray_intersects_tree(const Ray<float>& ray, const Tree<WebGLMesh, N>& tree) {
    return ray.intersects(tree);
}

template<int N>
std::vector<Vector3f> ray_par_intersects_tree(const Ray<float>& ray, const Tree<WebGLMesh, N>& tree, int threads_count) {
    return parallel_intersects_pool(ray, tree, threads_count);
}

emscripten::val get_indices(const WebGLMesh& mesh) {
    return emscripten::val(
        emscripten::typed_memory_view(mesh.indices().size(), mesh.indices().data())
    );
}

emscripten::val get_vertices(const WebGLMesh& mesh) {
    return emscripten::val(
        emscripten::typed_memory_view(mesh.vertices().size(), mesh.vertices().data())
    );
}


EMSCRIPTEN_BINDINGS(module) {
    using namespace emscripten;

    class_<Vector3f>("Vector")
        .constructor<float, float, float>()
        .property("x", &Vector3f::x)
        .property("y", &Vector3f::y)
        .property("z", &Vector3f::z);

    register_vector<Vector3f>("PointsList");

    class_<WebGLMesh>("Mesh")
        .property("size", &WebGLMesh::size)
        .function("vertices", &get_vertices)
        .function("indices", &get_indices);

    class_<Ray<float>>("Ray")
        .constructor<Vector3f, Vector3f>()
        .function("intersects_kdtree", &ray_intersects_tree<1>)
        .function("par_intersects_kdtree", &ray_par_intersects_tree<1>)

        .function("intersects_quadtree", &ray_intersects_tree<2>)
        .function("par_intersects_quadtree", &ray_par_intersects_tree<2>)

        .function("intersects_octree", &ray_intersects_tree<3>)
        .function("par_intersects_octree", &ray_par_intersects_tree<3>);

    function("readMesh", &read_mesh_from_string);

    class_<KDTree<WebGLMesh>>("KDTree")
        .class_function("forMesh", &build_tree<1>);

    class_<Quadtree<WebGLMesh>>("Quadtree")
        .class_function("forMesh", &build_tree<2>);

    class_<Octree<WebGLMesh>>("Octree")
        .class_function("forMesh", &build_tree<3>);
}
