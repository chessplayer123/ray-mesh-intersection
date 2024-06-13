#pragma once

#include "rmilib/rmi.h"
#include "rmilib/reader.hpp"


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

    inline const rmi::AABBox<float>& box() const {
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

    NodeWrapper(const typename rmi::Tree<WebGLMesh, N>::Node* node): node(node) {
    }

    const typename rmi::Tree<WebGLMesh, N>::Node* node;
};


template<int N>
auto register_tree(const std::string& name) {
    emscripten::class_<NodeWrapper<N>>((name + "Node").c_str())
        .property("box",      &NodeWrapper<N>::box)
        .function("children", &NodeWrapper<N>::children)
        .function("isLeaf",   &NodeWrapper<N>::is_leaf)
        .function("intersects",
            +[](const NodeWrapper<N>& wrapper, const rmi::Ray<float>& ray) {
                std::vector<rmi::Vector3f> output;
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

    return emscripten::class_<rmi::Tree<WebGLMesh, N>>(name.c_str())
        .class_function("forMesh", +[](WebGLMesh& mesh) {
            return rmi::Tree<WebGLMesh, N>::for_mesh(mesh.begin(), mesh.end());
        })
        .function("intersects",
            +[](const rmi::Tree<WebGLMesh, N>& tree, const rmi::Ray<float>& ray) { return ray.intersects(tree); }
        )
        .function("root", +[](const rmi::Tree<WebGLMesh, N>& tree) {
            return NodeWrapper<N>(&tree.top());
        });
}


void define_shared() {
    using namespace emscripten;

    class_<rmi::Vector3f>("Vector")
        .constructor<float, float, float>()
        .property("x", &rmi::Vector3f::x)
        .property("y", &rmi::Vector3f::y)
        .property("z", &rmi::Vector3f::z);

    register_vector<rmi::Vector3f>("PointsList");

    class_<WebGLMesh>("Mesh")
        .property("size", &WebGLMesh::size)
        .function("vertices", +[](const WebGLMesh& mesh) {
            return val(typed_memory_view(mesh.vertices().size(), mesh.vertices().data()));
        })
        .function("indices", +[](const WebGLMesh& mesh) {
            return val(typed_memory_view(mesh.indices().size(), mesh.indices().data()));
        });

    class_<rmi::AABBox<float>>("AABB")
        .property("min", &rmi::AABBox<float>::min)
        .property("max", &rmi::AABBox<float>::max)
        .function("intersects",
            +[](const rmi::AABBox<float>& aabb, const rmi::Ray<float>& ray) { return ray.intersects(aabb); }
        );

    class_<rmi::Ray<float>>("Ray")
        .constructor<rmi::Vector3f, rmi::Vector3f>();

    function("readMesh", &read_mesh_from_string);
}
