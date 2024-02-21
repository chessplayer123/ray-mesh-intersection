#pragma once

class Triangle;

#include "utils/vector.hpp"
#include <optional>
#include <limits>

class Ray {
public:
    Ray(Vector origin, Vector vector): origin(origin), vector(vector) {
    }

    std::optional<Vector> intersects(
        const Triangle& triangle,
        double epsilon = std::numeric_limits<double>::epsilon()
    ) const;
private:
    Vector origin;
    Vector vector;
};
