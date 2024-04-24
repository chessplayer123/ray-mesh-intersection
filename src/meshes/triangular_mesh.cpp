#include "meshes/triangular_mesh.hpp"
#include <iterator>
#include <algorithm>
#include <limits>
#include <math.h>

Box TriangularMesh::get_bounding_box(
    std::vector<int>::const_iterator begin,
    std::vector<int>::const_iterator end
) const {
    using std::min, std::max;

    double min_x, min_y, min_z, max_x, max_y, max_z;
    min_x = min_y = min_z = std::numeric_limits<double>::min();
    max_x = max_y = max_z = std::numeric_limits<double>::max();

    for (auto cur = begin; cur != end; std::advance(cur, 1)) {
        const Triangle& triangle = triangles[*cur];
        const Vector& v1 = triangle.v1();
        const Vector& v2 = triangle.v2();
        const Vector& v3 = triangle.v3();

        min_x = min(min_x, min(v1.get_x(), min(v2.get_x(), v3.get_x())));
        max_x = max(max_x, max(v1.get_x(), max(v2.get_x(), v3.get_x())));

        max_y = max(max_y, max(v1.get_y(), max(v2.get_y(), v3.get_y())));
        min_y = min(min_y, min(v1.get_y(), min(v2.get_y(), v3.get_y())));

        max_z = max(max_z, max(v1.get_z(), max(v2.get_z(), v3.get_z())));
        min_z = min(min_z, min(v1.get_z(), min(v2.get_z(), v3.get_z())));
    }

    return {
        Vector(min_x, min_y, min_z),
        Vector(max_x - min_x, max_y - min_y, max_z - min_z)
    };
}
