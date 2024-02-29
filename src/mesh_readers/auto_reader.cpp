#include "mesh_readers/reader.hpp"

template<>
TriangularMesh MeshReader::read_triangular_mesh<MeshReader::Auto>(const std::string& path) {
    size_t index = path.find_last_of('.');
    if (index == std::string::npos) {
        throw "Can't define file format";
    }

    std::string extension = path.substr(index);
    if (extension == "ply") {
        return read_triangular_mesh<MeshReader::Ply>(path);
    } else if (extension == "stl") {
        return read_triangular_mesh<MeshReader::Stl>(path);
    }
    throw "Unsupported file format";
}
