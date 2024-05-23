#pragma once

#include <memory>
#include <algorithm>
#include <cstddef>

#include "mesh.hpp"


template<typename It>
AABBox get_bounding_box(It begin, It end) {
    using std::min, std::max;

    double min_x, min_y, min_z, max_x, max_y, max_z;
    min_x = min_y = min_z = std::numeric_limits<double>::max();
    max_x = max_y = max_z = std::numeric_limits<double>::min();

    for (auto cur = begin; cur != end; ++cur) {
        const Vector& v1 = cur->template v<0>();
        const Vector& v2 = cur->template v<1>();
        const Vector& v3 = cur->template v<2>();

        min_x = min(min_x, min(v1.x(), min(v2.x(), v3.x())));
        max_x = max(max_x, max(v1.x(), max(v2.x(), v3.x())));

        max_y = max(max_y, max(v1.y(), max(v2.y(), v3.y())));
        min_y = min(min_y, min(v1.y(), min(v2.y(), v3.y())));

        max_z = max(max_z, max(v1.z(), max(v2.z(), v3.z())));
        min_z = min(min_z, min(v1.z(), min(v2.z(), v3.z())));
    }

    return {
        Vector(min_x, min_y, min_z),
        Vector(max_x, max_y, max_z)
    };
}


template<typename T>
class KDTree {
public:
    typedef typename T::iterator mesh_iterator;

    class iterator;

    static KDTree<T> for_mesh(
        mesh_iterator begin,
        mesh_iterator end,
        int max_depth = 16
    ) {
        return KDTree<T>(Node::build(begin, end, max_depth, 0));
    }

    iterator top() const {
        return iterator(root.get());
    }
private:
    struct Node;

    KDTree(std::unique_ptr<Node> root): root(std::move(root)) {
    }

    std::unique_ptr<Node> root;
};


template<typename T>
class KDTree<T>::iterator {
    friend KDTree;
public:
    iterator() noexcept = default;

    inline bool is_leaf() const {
        return !ptr->left && !ptr->right;
    }

    inline const AABBox& box() const {
        return ptr->bounding_box;
    }

    std::pair<mesh_iterator, mesh_iterator> triangles() const {
        return std::make_pair(ptr->begin, ptr->end);
    }

    inline iterator left() const {
        return iterator(ptr->left.get());
    }

    inline iterator right() const {
        return iterator(ptr->right.get());
    }
private:
    constexpr iterator(const Node* ptr): ptr(ptr) {}

    // Only observing
    const Node* ptr;
};

template<typename T>
struct KDTree<T>::Node {
    static std::unique_ptr<Node> build(
        typename T::iterator begin,
        typename T::iterator end,
        int max_depth,
        int depth
    ) {
        size_t length = std::distance(begin, end);

        if (length <= 0) {
            return nullptr;
        } else if (length == 1 || depth >= max_depth) {
            return std::make_unique<Node>(
                get_bounding_box(begin, end),
                begin, end,
                nullptr, nullptr
            );
        }

        switch (depth % 3) {
        case 0:
            std::sort(begin, end, [](const auto& it1, const auto& it2) {
                return it1.center.x() < it2.center.x();
            }); break;
        case 1:
            std::sort(begin, end, [](const auto& it1, const auto& it2) {
                return it1.center.y() < it2.center.y();
            }); break;
        case 2:
            std::sort(begin, end, [](const auto& it1, const auto& it2) {
                return it1.center.z() < it2.center.z();
            }); break;
        }

        auto middle = std::next(begin, length / 2);
        auto node = std::make_unique<Node>(
            get_bounding_box(begin, end),
            begin, begin,
            build(begin, middle, max_depth, depth + 1),
            build(middle, end, max_depth, depth + 1)
        );
        return node;
    }

    Node(
        AABBox box,
        mesh_iterator begin, mesh_iterator end,
        std::unique_ptr<Node>&& left,
        std::unique_ptr<Node>&& right
    ): bounding_box(box), begin(begin), end(end), left(std::move(left)), right(std::move(right)) {
    }

    AABBox bounding_box;
    mesh_iterator begin;
    mesh_iterator end;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};
