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
    std::istringstream stream(data);
    switch (define_format(filename)) {
        case DataFormat::Ply: return read_raw_triangular_mesh_ply<float, unsigned int>(stream);
        case DataFormat::Stl: return read_raw_triangular_mesh_stl<float, unsigned int>(stream);
        case DataFormat::Obj: return read_raw_triangular_mesh_obj<float, unsigned int>(stream);
    }
}


template<int N>
struct NodeWrapper {
    inline bool is_leaf() const {
        return node->is_leaf();
    }

    inline const AABBox<float>& box() const {
        return node->box();
    }

    inline std::vector<NodeWrapper> children() const {
        const auto& child_nodes = node->child_nodes();
        std::vector<NodeWrapper> res;
        res.reserve(child_nodes.size());
        for (const auto& child : child_nodes) {
            res.emplace_back(&child);
        }
        return res;
    }

    NodeWrapper(const typename Tree<WebGLMesh, N>::Node* node): node(node) {
    }

    const typename Tree<WebGLMesh, N>::Node* node;
};


template<int N>
void register_tree(const std::string& name) {
    emscripten::class_<NodeWrapper<N>>((name + "Node").c_str())
        .property("box",      &NodeWrapper<N>::box)
        .function("children", &NodeWrapper<N>::children)
        .function("isLeaf",   &NodeWrapper<N>::is_leaf)
        .function("intersects",
            +[](const NodeWrapper<N>& wrapper, const Ray<float>& ray) {
                std::vector<Vector3f> output;
                for (const auto& cur : wrapper.node->triangles()) {
                    auto intersection = ray.intersects<WebGLMesh>(cur);
                    if (intersection.has_value()) {
                        output.push_back(intersection.value());
                    }
                }
                return output;
            }
        );

    emscripten::register_vector<NodeWrapper<N>>((name + "Nodes").c_str());

    emscripten::class_<Tree<WebGLMesh, N>>(name.c_str())
        .class_function("forMesh", +[](WebGLMesh& mesh) {
            return Tree<WebGLMesh, N>::for_mesh(mesh.begin(), mesh.end());
        })
        .function("intersects",
            +[](const Tree<WebGLMesh, N>& tree, const Ray<float>& ray) { return ray.intersects(tree); }
        )
        .function("root", +[](const Tree<WebGLMesh, N>& tree) {
            return NodeWrapper<N>(&tree.top());
        });
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
        .function("vertices", +[](const WebGLMesh& mesh) {
            return val(typed_memory_view(mesh.vertices().size(), mesh.vertices().data()));
        })
        .function("indices", +[](const WebGLMesh& mesh) {
            return val(typed_memory_view(mesh.indices().size(), mesh.indices().data()));
        });

    class_<AABBox<float>>("AABB")
        .property("min", &AABBox<float>::min)
        .property("max", &AABBox<float>::max)
        .function("intersects",
            +[](const AABBox<float>& aabb, const Ray<float>& ray) { return ray.intersects(aabb); }
        );

    class_<Ray<float>>("Ray")
        .constructor<Vector3f, Vector3f>();

    function("readMesh", &read_mesh_from_string);

    register_tree<1>("KDTree");
    register_tree<2>("Quadtree");
    register_tree<3>("Octree");
}
