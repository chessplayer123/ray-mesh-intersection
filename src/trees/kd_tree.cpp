#include "trees/kd_tree.hpp"
#include <algorithm>
#include <numeric>

#define DIM 3

static std::unique_ptr<KDTree::Node> build_tree(
    std::vector<int>::iterator begin,
    std::vector<int>::iterator end,
    const TriangularMesh& mesh,
    int capacity,
    int depth
) {
    int length = std::distance(begin, end);
    if (length <= 0) {
        return nullptr;
    } else if (length <= capacity) {
        return std::make_unique<KDTree::Node>(
            mesh.get_bounding_box(begin, end),
            nullptr, nullptr
        );
    }

    switch (depth % DIM) {
    case 0:
        std::sort(begin, end, [&mesh](int i, int j) {
            return mesh.get_ith(i).x_center() < mesh.get_ith(j).x_center();
        }); break;
    case 1:
        std::sort(begin, end, [&mesh](int i, int j) {
            return mesh.get_ith(i).y_center() < mesh.get_ith(j).y_center();
        }); break;
    case 2:
        std::sort(begin, end, [&mesh](int i, int j) {
            return mesh.get_ith(i).z_center() < mesh.get_ith(j).z_center();
        }); break;
    }

    auto middle = std::next(begin, length / 2);
    auto node = std::make_unique<KDTree::Node>(
        mesh.get_bounding_box(middle, std::next(middle)),
        build_tree(begin, middle, mesh, capacity, depth + 1),
        build_tree(std::next(middle), end, mesh, capacity, depth + 1)
    );
    return node;
}

KDTree KDTree::for_mesh(const TriangularMesh& mesh, int bucket_capacity) {
    std::vector<int> idxs(mesh.size());
    std::iota(idxs.begin(), idxs.end(), 0);

    auto root = build_tree(idxs.begin(), idxs.end(), mesh, bucket_capacity, 0);
    return KDTree(root);
}
