#pragma once

#include <optional>
#include <vector>
#include <limits>
#include "utils/vector.hpp"
#include "utils/triangle.hpp"
#include "trees/kd_tree.hpp"
#include "meshes/triangular_mesh.hpp"


class Ray {
public:
    Ray(Vector origin, Vector vector): origin(origin), vector(vector) {
    }

    std::optional<Vector> intersects(
        const Triangle& triangle,
        double epsilon = std::numeric_limits<double>::epsilon()
    ) const;

    std::vector<Vector> intersects(
        const TriangularMesh& mesh,
        double epsilon = std::numeric_limits<double>::epsilon()
    ) const;

    std::vector<Vector> intersects(
        const KDTree& tree,
        double epsilon = std::numeric_limits<double>::epsilon()
    ) const;

    inline const Vector& get_origin() const {
        return origin;
    }

    inline const Vector& get_vector() const {
        return vector;
    }
private:
    Vector origin;
    Vector vector;
};
