#pragma once

#include <memory>
#include <algorithm>
#include <cstddef>

#include "aabbox.hpp"


template<typename T>
AABBox<typename T::float_t> get_bounding_box(
    typename T::iterator begin,
    typename T::iterator end
) {
    using std::min, std::max;

    typename T::float_t min_x, min_y, min_z, max_x, max_y, max_z;
    min_x = min_y = min_z = std::numeric_limits<float_t>::max();
    max_x = max_y = max_z = std::numeric_limits<float_t>::min();

    for (auto cur = begin; cur != end; ++cur) {
        const Vector3 v1 = cur->template v<0>();
        const Vector3 v2 = cur->template v<1>();
        const Vector3 v3 = cur->template v<2>();

        min_x = min(min_x, min(v1.x(), min(v2.x(), v3.x())));
        max_x = max(max_x, max(v1.x(), max(v2.x(), v3.x())));

        max_y = max(max_y, max(v1.y(), max(v2.y(), v3.y())));
        min_y = min(min_y, min(v1.y(), min(v2.y(), v3.y())));

        max_z = max(max_z, max(v1.z(), max(v2.z(), v3.z())));
        min_z = min(min_z, min(v1.z(), min(v2.z(), v3.z())));
    }

    return {
        Vector3(min_x, min_y, min_z),
        Vector3(max_x, max_y, max_z)
    };
}


template<typename T, int N>
class Tree {
public:
    typedef typename T::iterator mesh_iterator;

    class Node;

    struct Range {
        mesh_iterator begin() const {
            return m_begin;
        }

        mesh_iterator end() const {
            return m_end;
        }

        mesh_iterator m_begin;
        mesh_iterator m_end;
    };

    static Tree<T, N> for_mesh(
        mesh_iterator begin,
        mesh_iterator end,
        int depth_limit = 16
    ) {
        return Tree<T, N>(std::move(Node::build(begin, end, depth_limit)));
    }

    const Node& top() const {
        return root;
    }
private:
    Tree(Node&& root): root(std::move(root)) {
    }

    Node root;
};


template<typename T, int N>
class Tree<T, N>::Node {
public:
    friend Tree;

    Node() = delete;
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&&) = default;
    Node& operator=(Node&&) = default;
    ~Node() = default;

    constexpr Node(AABBox<typename T::float_t> box, Range range, std::vector<Node>&& children):
        bounding_box(box), range(range), children(std::move(children)) {
    }

    constexpr bool is_leaf() const {
        return children.empty();
    }

    constexpr const std::vector<Node>& child_nodes() const {
        return children;
    }

    constexpr const AABBox<typename T::float_t>& box() const {
        return bounding_box;
    }

    constexpr const Range& triangles() const {
        return range;
    }
private:
    static Node build(
        typename T::iterator begin,
        typename T::iterator end,
        int depth_limit
    ) {
        if (depth_limit <= 0) {
            return Node(
                get_bounding_box<T>(begin, end),
                Range{begin, end},
                std::move(std::vector<Node>())
            );
        }

        std::vector<Node> children;
        split(begin, end, children, depth_limit, N);

        return Node(get_bounding_box<T>(begin, end), Range{begin, end}, std::move(children));
    }

    static void split(
        typename T::iterator begin,
        typename T::iterator end,
        std::vector<Node>& children,
        int depth_limit,
        size_t counter
    ) {
        size_t length = std::distance(begin, end);
        if (length <= 0) {
            return;
        } else if (length <= static_cast<size_t>(1 << counter)) {
            children.emplace_back(get_bounding_box<T>(begin, end), Range{begin, end}, std::move(std::vector<Node>()));
            return;
        } else if (counter == 0) {
            children.push_back(std::move(build(begin, end, depth_limit-1)));
            return;
        }

        switch ((depth_limit + counter) % 3) {
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

        auto mid = std::next(begin, length / 2);
        split(begin, mid, children, depth_limit, counter - 1);
        split(mid, end, children, depth_limit, counter - 1);
    }

    AABBox<typename T::float_t> bounding_box;
    Range range;
    std::vector<Node> children;
};

template<typename T>
using KDTree = Tree<T, 1>;

template<typename T>
using Quadtree = Tree<T, 2>;

template<typename T>
using Octree = Tree<T, 3>;
