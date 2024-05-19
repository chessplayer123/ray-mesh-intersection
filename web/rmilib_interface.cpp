#include <emscripten.h>
#include <emscripten/bind.h>
#include <vector>
#include <sstream>

#include "rmilib/ray.hpp"
#include "rmilib/vector.hpp"
#include "rmilib/triangular_mesh.hpp"
#include "rmilib/reader.hpp"


std::shared_ptr<const TriangularMesh> new_triangular_mesh(std::vector<Triangle> triangles) {
    return std::make_shared<const TriangularMesh>(std::move(triangles));
}

std::shared_ptr<const TriangularMesh> read_mesh_from_string(const std::string& filename, const std::string& data) {
    DataFormat format = define_format(filename);
    std::istringstream stream(data);
    switch (format) {
        case Ply: return std::make_shared<const TriangularMesh>(read_triangular_mesh<Ply>(stream));
        case Stl: return std::make_shared<const TriangularMesh>(read_triangular_mesh<Stl>(stream));
        case Obj: return std::make_shared<const TriangularMesh>(read_triangular_mesh<Obj>(stream));
    }
}

EMSCRIPTEN_BINDINGS(module) {
    using namespace emscripten;

    class_<Vector>("Vector")
        .constructor<double, double, double>()
        .property("x", &Vector::x)
        .property("y", &Vector::y)
        .property("z", &Vector::z);

    class_<Triangle>("Triangle")
        .constructor<Vector, Vector, Vector>()
        .property("v1", &Triangle::v1)
        .property("v2", &Triangle::v2)
        .property("v3", &Triangle::v3);

    register_vector<Triangle>("TrianglesList");
    register_vector<Vector>("PointsList");

    class_<TriangularMesh>("Mesh")
        .smart_ptr<std::shared_ptr<const TriangularMesh>>("Mesh")
        .constructor(&new_triangular_mesh)
        .property("size", &TriangularMesh::size)
        .function("get", &TriangularMesh::get_ith);

    class_<KDTree>("KDTree")
        .class_function("forMesh", &KDTree::for_mesh);

    class_<Ray>("Ray")
        .constructor<Vector, Vector>()
        .function("intersects_tree", select_overload<std::vector<Vector>(const KDTree&, double) const>(&Ray::intersects))
        .function("intersects_mesh", select_overload<std::vector<Vector>(const TriangularMesh&, double) const>(&Ray::intersects));

    function("readMesh", &read_mesh_from_string);
}
