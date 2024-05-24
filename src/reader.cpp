#include "reader.hpp"
#include <fstream>


template<typename float_t, typename index_t>
RawMesh<float_t, index_t> read_raw_triangular_mesh(const std::string& path) {
    DataFormat format = define_format(path);
    std::ifstream stream(path);
    switch (format) {
        case DataFormat::Ply: return read_raw_triangular_mesh_ply<float_t, index_t>(stream);
        case DataFormat::Stl: return read_raw_triangular_mesh_stl<float_t, index_t>(stream);
        case DataFormat::Obj: return read_raw_triangular_mesh_obj<float_t, index_t>(stream);
    }
    // unreachable
}

template TriangularMesh read_raw_triangular_mesh<double, size_t>(const std::string& path);

template WebGLMesh read_raw_triangular_mesh<float, unsigned short>(const std::string& path);
