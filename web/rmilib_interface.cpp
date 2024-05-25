#include <emscripten.h>
#include <emscripten/bind.h>
#include <vector>
#include <sstream>

#include "rmilib/ray.hpp"
#include "rmilib/vector.hpp"
#include "rmilib/mesh.hpp"
#include "rmilib/reader.hpp"

WebGLMesh read_mesh_from_string(const std::string& filename, const std::string& data) {
    DataFormat format = define_format(filename);
    std::istringstream stream(data);
    switch (format) {
        case DataFormat::Ply: return read_raw_triangular_mesh_ply<float, unsigned int>(stream);
        case DataFormat::Stl: return read_raw_triangular_mesh_stl<float, unsigned int>(stream);
        case DataFormat::Obj: return read_raw_triangular_mesh_obj<float, unsigned int>(stream);
    }
}

KDTree<WebGLMesh> build_tree(WebGLMesh& mesh) {
    return KDTree<WebGLMesh>::for_mesh(mesh.begin(), mesh.end());
}

std::vector<Vector3f> ray_intersects_tree(const Ray<float>& ray, const KDTree<WebGLMesh>& tree) {
    return ray.intersects(tree);
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

    class_<KDTree<WebGLMesh>>("KDTree")
        .class_function("forMesh", &build_tree);

    class_<Ray<float>>("Ray")
        .constructor<Vector3f, Vector3f>()
        .function("intersects_tree", &ray_intersects_tree);

    function("readMesh", &read_mesh_from_string);
}
