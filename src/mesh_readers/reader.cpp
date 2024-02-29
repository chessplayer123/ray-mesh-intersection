#include "mesh_readers/reader.hpp"

MeshReader::DataFormat MeshReader::define_format(const std::string& path) {
    size_t index = path.find_last_of('.');
    if (index == std::string::npos) {
        throw "Can't define file format";
    }

    std::string extension = path.substr(index);
    if (extension == "ply") {
        return MeshReader::Ply;
    } else if (extension == "stl") {
        return MeshReader::Stl;
    } else if (extension == "obj") {
        return MeshReader::Obj;
    }
    throw "Unsupported file format";
}
