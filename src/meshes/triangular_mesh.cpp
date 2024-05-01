#include "meshes/triangular_mesh.hpp"
#include <iterator>
#include <algorithm>
#include <limits>
#include <math.h>

AABBox TriangularMesh::get_bounding_box(
    std::vector<size_t>::const_iterator begin,
    std::vector<size_t>::const_iterator end
) const {
    using std::min, std::max;

    double min_x, min_y, min_z, max_x, max_y, max_z;
    min_x = min_y = min_z = std::numeric_limits<double>::max();
    max_x = max_y = max_z = std::numeric_limits<double>::min();

    for (auto cur = begin; cur != end; std::advance(cur, 1)) {
        const Triangle& triangle = triangles[*cur];
        const Vector& v1 = triangle.v1();
        const Vector& v2 = triangle.v2();
        const Vector& v3 = triangle.v3();

        min_x = min(min_x, min(v1.x(), min(v2.x(), v3.x())));
        max_x = max(max_x, max(v1.x(), max(v2.x(), v3.x())));

        max_y = max(max_y, max(v1.y(), max(v2.y(), v3.y())));
        min_y = min(min_y, min(v1.y(), min(v2.y(), v3.y())));

        max_z = max(max_z, max(v1.z(), max(v2.z(), v3.z())));
        min_z = min(min_z, min(v1.z(), min(v2.z(), v3.z())));
    }

    return {
        Vector(min_x, min_y, min_z),
        Vector(max_x, max_y, max_z)
    };
}
