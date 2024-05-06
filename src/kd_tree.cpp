#include "kd_tree.hpp"
#include <algorithm>
#include <numeric>

#define DIM 3

KDTree::Node::Node(
    AABBox box,
    std::vector<size_t> data,
    std::unique_ptr<Node>&& left,
    std::unique_ptr<Node>&& right
): bounding_box(box), data(data), left(std::move(left)), right(std::move(right)) {
}

KDTree::Iterator KDTree::iterator() const {
    return KDTree::Iterator(
        root.get(),
        mesh.get()
    );
}

std::unique_ptr<KDTree::Node> KDTree::Node::build(
    std::vector<size_t>::iterator begin,
    std::vector<size_t>::iterator end,
    std::shared_ptr<const TriangularMesh> mesh,
    int max_depth,
    int depth
) {
    int length = std::distance(begin, end);

    if (length <= 0) {
        return nullptr;
    } else if (length == 1 || depth >= max_depth) {
        std::vector<size_t> indexes(length);
        std::copy(begin, end, indexes.begin());
        return std::make_unique<Node>(
            mesh->get_bounding_box(begin, end),
            indexes,
            nullptr, nullptr
        );
    }

    switch (depth % DIM) {
    case 0:
        std::sort(begin, end, [&mesh](size_t i, size_t j) {
            return mesh->get_ith(i).x_center() < mesh->get_ith(j).x_center();
        }); break;
    case 1:
        std::sort(begin, end, [&mesh](size_t i, size_t j) {
            return mesh->get_ith(i).y_center() < mesh->get_ith(j).y_center();
        }); break;
    case 2:
        std::sort(begin, end, [&mesh](size_t i, size_t j) {
            return mesh->get_ith(i).z_center() < mesh->get_ith(j).z_center();
        }); break;
    }

    auto middle = std::next(begin, length / 2);
    auto node = std::make_unique<Node>(
        mesh->get_bounding_box(begin, end),
        std::vector<size_t>(),
        build(begin, middle, mesh, max_depth, depth + 1),
        build(middle, end, mesh, max_depth, depth + 1)
    );
    return node;
}

KDTree KDTree::for_mesh(std::shared_ptr<const TriangularMesh> mesh, int max_depth) {
    std::vector<size_t> idxs(mesh->size());
    std::iota(idxs.begin(), idxs.end(), 0);

    auto root = Node::build(idxs.begin(), idxs.end(), mesh, max_depth, 0);
    return KDTree(root, mesh);
}
