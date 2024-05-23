#pragma once

#include <optional>
#include <vector>
#include <limits>

#include "vector.hpp"
#include "aabbox.hpp"
#include "kd_tree.hpp"
#include "mesh.hpp"


class Ray {
public:
    Ray(Vector origin, Vector direction): origin(origin) {
        double length = direction.length();
        vector = direction / length;
        inv_vector = Vector(
            1.0 / vector.x(),
            1.0 / vector.y(),
            1.0 / vector.z()
        );
    }

    bool intersects(const AABBox& box) const;

    template<typename T>
    std::optional<Vector> intersects(
        const typename Mesh<T>::Element& triangle,
        double epsilon = std::numeric_limits<double>::epsilon()
    ) const;

    template<typename T>
    std::vector<Vector> intersects(
        Mesh<T>& mesh,
        double epsilon = std::numeric_limits<double>::epsilon()
    ) const;

    template<typename T>
    std::vector<Vector> intersects(
        const KDTree<T>& tree,
        double epsilon = std::numeric_limits<double>::epsilon()
    ) const;
private:
    template<typename T>
    void recursive_intersects(
        const typename KDTree<T>::iterator iter,
        std::vector<Vector>& output,
        double epsilon = std::numeric_limits<double>::epsilon()
    ) const;

    Vector origin;
    Vector vector;
    Vector inv_vector;
};


/*
 * Read for details:
 * https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
 */
template<typename T>
std::optional<Vector> Ray::intersects(
    const typename Mesh<T>::Element& triangle,
    double epsilon
) const {
    const Vector v1 = triangle.template v<0>();
    const Vector v2 = triangle.template v<1>();
    const Vector v3 = triangle.template v<2>();

    Vector edge1 = v2 - v1;
    Vector edge2 = v3 - v1;
    Vector ray_cross_e2 = vector.cross(edge2);

    double det = edge1.dot(ray_cross_e2);
    if (-epsilon <= det && det <= epsilon) {
        return std::nullopt;
    }
    double inv_det = 1.0 / det;

    Vector s = origin - v1;
    double u = inv_det * s.dot(ray_cross_e2);
    if (u < 0.0 || u > 1.0) {
        return std::nullopt;
    }

    Vector s_cross_e1 = s.cross(edge1);
    double v = inv_det * vector.dot(s_cross_e1);
    if (v < 0.0 || u + v > 1.0) {
        return std::nullopt;
    }

    double t = inv_det * edge2.dot(s_cross_e1);
    if (t <= epsilon) {
        return std::nullopt;
    }
    return origin + vector * t;
}


template<typename T>
void Ray::recursive_intersects(
    const typename KDTree<T>::iterator iter,
    std::vector<Vector>& output,
    double epsilon
) const {
    if (iter.is_leaf()) {
        auto [begin, end] = iter.triangles();
        for (auto cur = begin; cur != end; ++cur) {
            auto intersection = intersects<T>(*cur, epsilon);
            if (intersection.has_value()) {
                output.push_back(intersection.value());
            }
        }
    } else {
        if (auto left = iter.left(); intersects(left.box())) {
            recursive_intersects<T>(left, output, epsilon);
        }

        if (auto right = iter.right(); intersects(right.box())) {
            recursive_intersects<T>(right, output, epsilon);
        }
    }
}


template<typename T>
std::vector<Vector> Ray::intersects(
    const KDTree<T>& tree,
    double epsilon
) const {
    auto iter = tree.top();
    if (!intersects(iter.box())) {
        return {};
    }

    std::vector<Vector> output;
    recursive_intersects<T>(iter, output, epsilon);
    return output;
}


/*
 * Simple iterative intersection search
 */
template<typename T>
std::vector<Vector> Ray::intersects(Mesh<T>& mesh, double epsilon) const {
    std::vector<Vector> intersections;

    for (const auto& cur : mesh) {
        auto intersection = intersects<T>(cur, epsilon);

        if (intersection.has_value()) {
            intersections.push_back(intersection.value());
        }
    }

    return intersections;
}
