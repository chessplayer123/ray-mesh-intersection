#pragma once

#include <emscripten.h>
#include <emscripten/bind.h>
#include <vector>
#include <sstream>


#include "rmilib/rmi.hpp"
#include "rmilib/reader.hpp"


WebGLMesh read_mesh_from_string(const std::string& filename, const std::string& data) {
    std::istringstream stream(data);
    switch (define_format(filename)) {
        case DataFormat::Ply: return read_raw_triangular_mesh_ply<float, unsigned int>(stream);
        case DataFormat::Stl: return read_raw_triangular_mesh_stl<float, unsigned int>(stream);
        case DataFormat::Obj: return read_raw_triangular_mesh_obj<float, unsigned int>(stream);
    }
}


enum class SplitterType {
    SAH,
    Median,
};


auto register_shared() {
    using namespace emscripten;

    value_array<rmi::Vector3f>("Vector")
        .element(&rmi::Vector3f::x, &rmi::Vector3f::set_x)
        .element(&rmi::Vector3f::y, &rmi::Vector3f::set_y)
        .element(&rmi::Vector3f::z, &rmi::Vector3f::set_z)
        ;

    class_<std::pair<float, float>>("FloatPair")
        .property("first",  &std::pair<float, float>::first)
        .property("second", &std::pair<float, float>::second);

    register_vector<rmi::Vector3f>("PointsList");

    enum_<SplitterType>("Splitter")
        .value("SAH",    SplitterType::SAH)
        .value("Median", SplitterType::Median)
        ;

    class_<WebGLMesh>("Mesh")
        .property("size", &WebGLMesh::size)
        .function("vertices", +[](const WebGLMesh& mesh) {
            return val(typed_memory_view(mesh.vertices().size(), mesh.vertices().data()));
        })
        .function("indices", +[](const WebGLMesh& mesh) {
            return val(typed_memory_view(mesh.indices().size(), mesh.indices().data()));
        })
        ;

    class_<rmi::AABBox<float>>("AABB")
        .property("min", &rmi::AABBox<float>::min)
        .property("max", &rmi::AABBox<float>::max)
        ;

    class_<typename rmi::KDTree<WebGLMesh>::Node>("KDTreeNode")
        .property("box",      &rmi::KDTree<WebGLMesh>::Node::box)
        .function("left",     +[](const typename rmi::KDTree<WebGLMesh>::Node* node) { return &node->left(); }, allow_raw_pointers())
        .function("right",    +[](const typename rmi::KDTree<WebGLMesh>::Node* node) { return &node->right(); }, allow_raw_pointers())
        .function("isLeaf",   &rmi::KDTree<WebGLMesh>::Node::is_leaf)
        ;

    class_<rmi::KDTree<WebGLMesh>>("KDTree")
        .class_function("forMesh", +[](WebGLMesh& mesh, SplitterType splitter) {
            switch (splitter) {
            case SplitterType::SAH:
                return rmi::KDTree<WebGLMesh>::for_mesh(mesh, rmi::SAHSplitter<WebGLMesh>());
            case SplitterType::Median:
                return rmi::KDTree<WebGLMesh>::for_mesh(mesh, rmi::MedianSplitter<WebGLMesh>());
            }
        })
        .function("intersects", +[](const rmi::KDTree<WebGLMesh>& tree, const rmi::Ray<float>& ray) {
            return ray.intersects(tree);
        })
        .function("root", +[](const rmi::KDTree<WebGLMesh>& tree) { return &tree.top(); }, allow_raw_pointers())
        ;

    function("readMesh", &read_mesh_from_string);

    return class_<rmi::Ray<float>>("Ray")
        .constructor<rmi::Vector3f, rmi::Vector3f>()
        .function("at", &rmi::Ray<float>::at)
        .function("isIntersectsAABB", &rmi::Ray<float>::is_intersects)
        .function("intersectsNode", +[](const rmi::Ray<float>& ray, const typename rmi::KDTree<WebGLMesh>::Node* node) {
            std::vector<rmi::Vector3f> result;
            for (const auto& triangle : *node) {
                if (auto intersection = ray.intersects<WebGLMesh>(triangle); intersection) {
                    result.push_back(std::move(*intersection));
                }
            }
            return result;
        }, allow_raw_pointers())
        .function("intersectsAABB", +[](const rmi::Ray<float>& ray, const rmi::AABBox<float>& box) {
            return ray.intersects(box);
        })
        .function("intersectsKDTree", +[](const rmi::Ray<float>& ray, const rmi::KDTree<WebGLMesh>& tree) {
            return ray.intersects(tree);
        })
        ;
}
