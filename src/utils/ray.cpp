#include "utils/ray.hpp"
#include "utils/triangle.hpp"
#include "utils/misc.hpp"


/*
Read for details:
https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
*/
std::optional<Vector> Ray::intersects(const Triangle& triangle, double epsilon) const {
    Vector edge1 = triangle.v2() - triangle.v1();
    Vector edge2 = triangle.v3() - triangle.v1();
    Vector ray_cross_e2 = vector.cross(edge2);

    double det = edge1.dot(ray_cross_e2);
    if (inside_exclusive_range(-epsilon, det, epsilon)) {
        return std::nullopt;
    }
    double inv_det = 1.0 / det;

    Vector s = origin - triangle.v1();
    double u = -inv_det * s.dot(ray_cross_e2);
    if (!inside_inclusive_range(0.0, u, 1.0)) {
        return std::nullopt;
    }

    Vector s_cross_e1 = edge1.cross(s);
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
