#include "utils/triangle.hpp"


Vector Triangle::intersects(const Ray& ray) const {
    Vector normal = get_normal();
    return Vector();
}

Vector Triangle::get_normal() const {
    Vector vec1 = vertexes[1] - vertexes[0];
    Vector vec2 = vertexes[2] - vertexes[0];

    return vec1.cross(vec2);
}

bool Triangle::contains(const Vector& point) const {
    return false;
}
