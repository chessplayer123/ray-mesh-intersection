#ifndef TRIANGLE_HPP_
#define TRIANGLE_HPP_

#include <array>
#include "utils/vector.hpp"
#include "utils/ray.hpp"
#include "utils/vector.hpp"

class Triangle {
public:
    Triangle(Vector p1, Vector p2, Vector p3): vertexes({p1, p2, p3}) {}

    Vector intersects(const Ray& ray) const;
    Vector get_normal() const;
    bool contains(const Vector& point) const;
private:
    std::array<Vector, 3> vertexes;
};

#endif // TRIANGLE_HPP_
