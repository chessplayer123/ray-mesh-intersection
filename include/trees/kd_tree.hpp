#pragma once

#include <memory>
#include "meshes/triangular_mesh.hpp"
#include "utils/box.hpp"

class KDTree {
public:
    struct Node {
        Node(Box box, std::unique_ptr<Node>&& left, std::unique_ptr<Node>&& right) :
            bounding_box(box), left(std::move(left)), right(std::move(right)) {}

        // TODO: store triangle data
        Box bounding_box;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };

    static const int DEFAULT_BUCKET_CAPACITY = 10;

    static KDTree for_mesh(
        const TriangularMesh& mesh,
        int bucket_capacity = DEFAULT_BUCKET_CAPACITY
    );
private:
    KDTree(std::unique_ptr<Node>& root) : root(std::move(root)) {
    }

    std::unique_ptr<Node> root;
};
