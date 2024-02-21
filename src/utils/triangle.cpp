#include "utils/triangle.hpp"
#include "utils/misc.hpp"


Vector Triangle::get_normal() const {
    Vector vec1 = v2() - v1();
    Vector vec2 = v3() - v1();

    return vec1.cross(vec2);
}

Vector Triangle::get_barycentric_coords(const Vector& point) const {
    Vector vec0 = v2() - v1(), vec1 = v3() - v1(), vec2 = point - v1();
    double d00 = vec0.dot(vec0), d01 = vec0.dot(vec1),
           d11 = vec1.dot(vec1),
           d20 = vec2.dot(vec0), d21 = vec2.dot(vec1),
           inv_det = 1.0 / (d00 * d11 - d01 * d01);

    double x = (d11 * d20 - d01 * d21) * inv_det,
           y = (d00 * d21 - d01 * d20) * inv_det,
           z = 1.0 - x - y;

    return Vector(x, y, z);
}


bool Triangle::contains(const Vector& point) const {
    Vector barycentric = get_barycentric_coords(point);

    return inside_inclusive_range(0.0, barycentric.get_x(), 1.0)
        && inside_inclusive_range(0.0, barycentric.get_y(), 1.0)
        && inside_inclusive_range(0.0, barycentric.get_z(), 1.0);
}
