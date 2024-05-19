#include "ray.hpp"
#include "thread_pool.hpp"

#include <queue>
#include <array>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <optional>


/*
 * Read for details:
 * https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
 */
std::optional<Vector> Ray::intersects(const Triangle& triangle, double epsilon) const {
    Vector edge1 = triangle.v2() - triangle.v1();
    Vector edge2 = triangle.v3() - triangle.v1();
    Vector ray_cross_e2 = vector.cross(edge2);

    double det = edge1.dot(ray_cross_e2);
    if (-epsilon <= det && det <= epsilon) {
        return std::nullopt;
    }
    double inv_det = 1.0 / det;

    Vector s = origin - triangle.v1();
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


/*
 * Simple iterative intersection search
 */
std::vector<Vector> Ray::intersects(const TriangularMesh& mesh, double epsilon) const {
    std::vector<Vector> intersections;

    for (const auto& triangle : mesh) {
        auto intersection = intersects(triangle, epsilon);

        if (intersection.has_value()) {
            intersections.push_back(intersection.value());
        }
    }

    return intersections;
}


bool Ray::intersects(const AABBox& box) const {
    Vector t1 = (box.min - origin) * inv_vector;
    Vector t2 = (box.max - origin) * inv_vector;

    double tmin = std::min(t1.x(), t2.x());
    double tmax = std::max(t1.x(), t2.x());

    tmin = std::max(tmin, std::min(t1.y(), t2.y()));
    tmax = std::min(tmax, std::max(t1.y(), t2.y()));

    tmin = std::max(tmin, std::min(t1.z(), t2.z()));
    tmax = std::min(tmax, std::max(t1.z(), t2.z()));

    return tmax >= 0 && tmin <= tmax;
}


void recursive_intersects(
    const Ray& ray,
    KDTree::Iterator iter,
    std::vector<Vector>& output,
    double epsilon
) {
    if (iter.is_leaf()) {
        for (const Triangle& triangle : iter.triangles()) {
            auto intersection = ray.intersects(triangle, epsilon);
            if (intersection.has_value()) {
                output.push_back(intersection.value());
            }
        }
    } else {
        if (auto left = iter.left(); ray.intersects(left.box())) {
            recursive_intersects(ray, left, output, epsilon);
        }

        if (auto right = iter.right(); ray.intersects(right.box())) {
            recursive_intersects(ray, right, output, epsilon);
        }
    }
}


std::vector<Vector> Ray::intersects(const KDTree& tree, double epsilon) const {
    KDTree::Iterator iterator = tree.iterator();
    if (!intersects(iterator.box())) {
        return {};
    }

    std::vector<Vector> output;

    recursive_intersects(*this, iterator, output, epsilon);

    return output;
}


std::vector<Vector> Ray::intersects_parallel(
    const KDTree& tree,
    int threads_count,
    double epsilon
) const {
    auto root = tree.iterator();
    if (!intersects(root.box())) {
        return {};
    }

    ThreadPool pool(*this, tree.iterator(), threads_count);
    return pool.wait_result();
}
