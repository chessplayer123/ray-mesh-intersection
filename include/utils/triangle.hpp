#ifndef TRIANGLE_HPP_
#define TRIANGLE_HPP_

#include <array>
#include "utils/vector.hpp"

class Triangle {
public:
    Triangle(Vector p1, Vector p2, Vector p3): vertexes({p1, p2, p3}) {}

    Vector get_normal() const;
    bool contains(const Vector& point) const;
    Vector get_barycentric_coords(const Vector& point) const;

    constexpr const Vector& v1() const {
        return vertexes[0];
    }

    constexpr const Vector& v2() const {
        return vertexes[1];
    }

    constexpr const Vector& v3() const {
        return vertexes[2];
    }
private:
    std::array<Vector, 3> vertexes;
};

#endif // TRIANGLE_HPP_
