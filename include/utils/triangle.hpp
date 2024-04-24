#pragma once

#include <array>
#include "utils/vector.hpp"

class Triangle {
public:
    Triangle(Vector p1, Vector p2, Vector p3):
        vertex1(p1), vertex2(p2), vertex3(p3), center((p1 + p2 + p3) / 3.0)
    {
        Vector vec1 = v2() - v1();
        Vector vec2 = v3() - v1();
        normal = vec1.cross(vec2);
    }

    Triangle(Vector p1, Vector p2, Vector p3, Vector normal):
        vertex1(p1), vertex2(p2), vertex3(p3),
        normal(normal), center((p1+ p2 +p3) / 3.0) {}

    bool contains(const Vector& point) const;
    Vector get_barycentric_coords(const Vector& point) const;

    inline const Vector& get_normal() const {
        return normal;
    }

    inline const Vector& v1() const {
        return vertex1;
    }

    inline const Vector& v2() const {
        return vertex2;
    }

    inline const Vector& v3() const {
        return vertex3;
    }

    inline double x_center() const {
        return center.get_x();
    }

    inline double y_center() const {
        return center.get_y();
    }

    inline double z_center() const {
        return center.get_z();
    }
private:
    Vector vertex1, vertex2, vertex3;
    Vector normal;
    Vector center;
};
