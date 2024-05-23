#include "reader.hpp"
#include <fstream>

TriangularMesh read_triangular_mesh(const std::string& path) {
    DataFormat format = define_format(path);
    std::ifstream stream(path);
    switch (format) {
        case DataFormat::Ply: return read_triangular_mesh<DataFormat::Ply>(stream);
        case DataFormat::Stl: return read_triangular_mesh<DataFormat::Stl>(stream);
        case DataFormat::Obj: return read_triangular_mesh<DataFormat::Obj>(stream);
    }
    // unreachable
}
