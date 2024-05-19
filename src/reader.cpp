#include "reader.hpp"
#include <fstream>

TriangularMesh read_triangular_mesh(const std::string& path) {
    DataFormat format = define_format(path);
    std::ifstream stream(path);
    switch (format) {
        case Ply: return read_triangular_mesh<Ply>(stream);
        case Stl: return read_triangular_mesh<Stl>(stream);
        case Obj: return read_triangular_mesh<Obj>(stream);
    }
    // unreachable
}
