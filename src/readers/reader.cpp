#include "readers/reader.hpp"
#include <fstream>

TriangularMesh read_triangular_mesh(const std::string& path) {
    size_t index = path.find_last_of('.');
    if (index == std::string::npos) {
        throw "Can't define file format";
    }
    std::string extension = path.substr(index + 1);

    std::ifstream stream(path);

    TriangularMesh mesh;
    if (extension == "ply") {
        mesh = read_triangular_mesh<DataFormat::Ply>(stream);
    } else if (extension == "stl") {
        mesh = read_triangular_mesh<DataFormat::Stl>(stream);
    } else if (extension == "obj") {
        mesh = read_triangular_mesh<DataFormat::Obj>(stream);
    } else {
        throw "Unsupported file format";
    }
    return mesh;
}
