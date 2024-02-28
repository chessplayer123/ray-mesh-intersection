#pragma once

#include <array>
#include "utils/vector.hpp"

class Triangle {
public:
    Triangle(Vector p1, Vector p2, Vector p3):
        vertex1(p1), vertex2(p2), vertex3(p3)
    {
        Vector vec1 = v2() - v1();
        Vector vec2 = v3() - v1();
        normal = vec1.cross(vec2);
    }

    Triangle(Vector p1, Vector p2, Vector p3, Vector normal):
        vertex1(p1), vertex2(p2), vertex3(p3), normal(normal) {}

    bool contains(const Vector& point) const;
    Vector get_barycentric_coords(const Vector& point) const;

    constexpr const Vector& get_normal() const {
        return normal;
    }

    constexpr const Vector& v1() const {
        return vertex1;
    }

    constexpr const Vector& v2() const {
        return vertex2;
    }

    constexpr const Vector& v3() const {
        return vertex3;
    }
private:
    Vector vertex1, vertex2, vertex3;
    Vector normal;
};
