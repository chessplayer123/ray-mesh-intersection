#include "readers/reader.hpp"
#include <cstddef>

using std::literals::operator""s;

template<>
Result<TriangularMesh, std::string> read_triangular_mesh<Ply>(
    const std::string& path
) {
    return Error("TODO"s);
}

template<>
Result<TriangularMesh, std::string> read_triangular_mesh<Step>(
    const std::string& path
) {
    return Error("TODO"s);
}

template<>
Result<TriangularMesh, std::string> read_triangular_mesh<Auto>(
    const std::string& path
) {
    size_t index = path.find_last_of('.');
    if (index == std::string::npos) {
        return Error("Can't define file format"s);
    }

    std::string extension = path.substr(index);
    if (extension == "step") {
        return read_triangular_mesh<Step>(path);
    } else if (extension == "ply") {
        return read_triangular_mesh<Ply>(path);
    }
    return Error("Unsupported file format"s);
}
