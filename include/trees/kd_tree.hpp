#pragma once

#include <experimental/memory>
#include <memory>
#include <cstddef>

#include "meshes/triangular_mesh.hpp"
#include "utils/aabbox.hpp"

namespace std {
    using std::experimental::observer_ptr;
}

class KDTree {
public:
    class Iterator;

    static KDTree for_mesh(
        std::shared_ptr<const TriangularMesh> mesh,
        int max_depth = 16
    );

    Iterator iterator() const;
private:
    struct Node {
        static std::unique_ptr<Node> build(
            std::vector<size_t>::iterator begin,
            std::vector<size_t>::iterator end,
            std::shared_ptr<const TriangularMesh> mesh,
            int max_depth,
            int depth
        );

        Node(AABBox box, std::vector<size_t> data, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right);

        AABBox bounding_box;
        std::vector<size_t> data;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };

    KDTree(std::unique_ptr<Node>& root, std::shared_ptr<const TriangularMesh> mesh)
        : root(std::move(root)), mesh(mesh) {}

    std::unique_ptr<Node> root;
    std::shared_ptr<const TriangularMesh> mesh;
};



class KDTree::Iterator {
public:
    Iterator(
        std::observer_ptr<Node> ptr,
        std::observer_ptr<const TriangularMesh> mesh
    ): ptr(ptr), mesh(mesh) {}

    inline bool is_leaf() const {
        return !ptr->data.empty();
    }

    inline AABBox box() const {
        return ptr->bounding_box;
    }

    std::vector<Triangle> triangles() const {
        std::vector<Triangle> triangles;
        triangles.reserve(ptr->data.size());

        for (size_t index : ptr->data) {
            triangles.push_back(mesh->get_ith(index));
        }

        return triangles;
    }

    inline Iterator left() const {
        return Iterator(std::observer_ptr<Node>(ptr->left.get()), mesh);
    }

    inline Iterator right() const {
        return Iterator(std::observer_ptr<Node>(ptr->right.get()), mesh);
    }
private:
    std::observer_ptr<Node> ptr;
    std::observer_ptr<const TriangularMesh> mesh;
};
