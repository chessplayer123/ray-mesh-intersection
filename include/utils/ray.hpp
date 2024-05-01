#pragma once

#include <optional>
#include <vector>
#include <limits>
#include "utils/vector.hpp"
#include "utils/triangle.hpp"
#include "utils/aabbox.hpp"
#include "trees/kd_tree.hpp"
#include "meshes/triangular_mesh.hpp"


class Ray {
public:
    Ray(Vector origin, Vector direction): origin(origin) {
        double length = direction.length();
        vector = direction / length;
        inv_origin = Vector(
            1.0 / vector.x(),
            1.0 / vector.y(),
            1.0 / vector.z()
        );
    }

    bool intersects(const AABBox& box) const;

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
private:
    Vector origin;
    Vector vector;
    Vector inv_origin;
};
