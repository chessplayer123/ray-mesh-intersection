#pragma once

#include <memory>
#include <cstddef>

#include "triangular_mesh.hpp"
#include "aabbox.hpp"


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
    friend KDTree;
public:
    Iterator() noexcept = default;

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
        return Iterator(ptr->left.get(), mesh);
    }

    inline Iterator right() const {
        return Iterator(ptr->right.get(), mesh);
    }
private:
    Iterator(
        const Node* ptr,
        const TriangularMesh* mesh
    ): ptr(ptr), mesh(mesh) {}

    // Only observing
    const Node* ptr;
    const TriangularMesh* mesh;
};
