#pragma once

#include <optional>
#include <vector>
#include <limits>

#include "vector.hpp"
#include "aabbox.hpp"
#include "tree.hpp"
#include "mesh.hpp"


template<typename float_t>
class Ray {
public:
    Ray(Vector3<float_t> origin, Vector3<float_t> direction):
        origin(origin),
        vector(direction.ort())
    {
        inv_vector = Vector3<float_t>(
            1.0 / vector.x(),
            1.0 / vector.y(),
            1.0 / vector.z()
        );
    }

    bool intersects(const AABBox<float_t>& box) const;

    template<typename mesh_t>
    std::optional<Vector3<float_t>> intersects(
        const typename mesh_t::Element& triangle,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;

    template<typename mesh_t, typename index_t>
    std::vector<Vector3<float_t>> intersects(
        Mesh<mesh_t, float_t, index_t>& mesh,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;

    template<typename T, int N>
    std::vector<Vector3<float_t>> intersects(
        const Tree<T, N>& tree,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;
private:
    template<typename T, int N>
    void recursive_intersects(
        const typename Tree<T, N>::Node& iter,
        std::vector<Vector3<float_t>>& output,
        float_t epsilon = std::numeric_limits<float_t>::epsilon()
    ) const;

    Vector3<float_t> origin;
    Vector3<float_t> vector;
    Vector3<float_t> inv_vector;
};


/*
 * Read for details:
 * https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
 */
template<typename float_t>
template<typename mesh_t>
std::optional<Vector3<float_t>> Ray<float_t>::intersects(
    const typename mesh_t::Element& triangle,
    float_t epsilon
) const {
    const Vector3 v1 = triangle.template v<0>();
    const Vector3 v2 = triangle.template v<1>();
    const Vector3 v3 = triangle.template v<2>();

    Vector3 edge1 = v2 - v1;
    Vector3 edge2 = v3 - v1;
    Vector3 ray_cross_e2 = vector.cross(edge2);

    float_t det = edge1.dot(ray_cross_e2);
    if (-epsilon <= det && det <= epsilon) {
        return std::nullopt;
    }
    float_t inv_det = 1.0 / det;

    Vector3 s = origin - v1;
    float_t u = inv_det * s.dot(ray_cross_e2);
    if (u < 0.0 || u > 1.0) {
        return std::nullopt;
    }

    Vector3 s_cross_e1 = s.cross(edge1);
    float_t v = inv_det * vector.dot(s_cross_e1);
    if (v < 0.0 || u + v > 1.0) {
        return std::nullopt;
    }

    float_t t = inv_det * edge2.dot(s_cross_e1);
    if (t <= epsilon) {
        return std::nullopt;
    }
    return origin + vector * t;
}


template<typename float_t>
template<typename T, int N>
void Ray<float_t>::recursive_intersects(
    const typename Tree<T, N>::Node& node,
    std::vector<Vector3<float_t>>& output,
    float_t epsilon
) const {
    if (node.is_leaf()) {
        for (const auto& cur : node.triangles()) {
            auto intersection = intersects<T>(cur, epsilon);
            if (intersection.has_value()) {
                output.push_back(intersection.value());
            }
        }
    } else {
        for (const auto& child : node.child_nodes()) {
            if (intersects(child.box())) {
                recursive_intersects<T, N>(child, output, epsilon);
            }
        }
    }
}


template<typename float_t>
template<typename T, int N>
std::vector<Vector3<float_t>> Ray<float_t>::intersects(
    const Tree<T, N>& tree,
    float_t epsilon
) const {
    const typename Tree<T, N>::Node& node = tree.top();
    if (!intersects(node.box())) {
        return {};
    }

    std::vector<Vector3<float_t>> output;
    recursive_intersects<T, N>(node, output, epsilon);
    return output;
}


/*
 * Simple iterative intersection search
 */
template<typename float_t>
template<typename mesh_t, typename index_t>
std::vector<Vector3<float_t>> Ray<float_t>::intersects(
    Mesh<mesh_t, float_t, index_t>& mesh,
    float_t epsilon
) const {
    std::vector<Vector3<float_t>> intersections;

    for (const auto& cur : mesh) {
        auto intersection = intersects<mesh_t>(cur, epsilon);

        if (intersection.has_value()) {
            intersections.push_back(intersection.value());
        }
    }

    return intersections;
}

template<typename float_t>
inline bool Ray<float_t>::intersects(const AABBox<float_t>& box) const {
    Vector3 t1 = (box.min - origin) * inv_vector;
    Vector3 t2 = (box.max - origin) * inv_vector;

    float_t tmin = std::min(t1.x(), t2.x());
    float_t tmax = std::max(t1.x(), t2.x());

    tmin = std::max(tmin, std::min(t1.y(), t2.y()));
    tmax = std::min(tmax, std::max(t1.y(), t2.y()));

    tmin = std::max(tmin, std::min(t1.z(), t2.z()));
    tmax = std::min(tmax, std::max(t1.z(), t2.z()));

    return tmax >= 0 && tmin <= tmax;
}
