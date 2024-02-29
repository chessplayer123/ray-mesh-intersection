#include "mesh_readers/reader.hpp"
#include <unordered_map>
#include <fstream>

namespace obj {
    enum Keyword {
        Vertex,
        Face,
        VertexTexture,
        Group,
        VertexNormal,
        Line,
        Comment,
        Eof
    };
} // namespace obj

template<>
obj::Keyword MeshReader::read<obj::Keyword>() {
    static const std::unordered_map<std::string, obj::Keyword> mapping = {
        {"v",  obj::Vertex},
        {"f",  obj::Face},
        {"vt", obj::VertexTexture},
        {"g",  obj::Group},
        {"l",  obj::Line},
        {"vn", obj::VertexNormal},
    };
    std::string token = read<std::string>();

    if (token.size() == 0) {
        return obj::Eof;
    } else if (token[0] == '#') {
        return obj::Comment;
    } else if (auto found = mapping.find(token); found != mapping.end()) {
        return found->second;
    }
    throw "Unsupported keyword '" + token + "'.";
}

template<>
TriangularMesh MeshReader::read_triangular_mesh<MeshReader::Obj>(std::istream& stream) {
    MeshReader reader(stream);

    std::vector<Vector> vertexes;
    std::vector<Triangle> triangles;

    bool has_data = true;
    while (has_data) {
        auto kw = reader.read<obj::Keyword>();
        switch (kw) {
            case obj::Vertex:
                vertexes.emplace_back(
                    reader.read<double>(),
                    reader.read<double>(),
                    reader.read<double>()
                );
                break;
            case obj::Face:
                triangles.emplace_back(
                    vertexes[reader.read<int>()],
                    vertexes[reader.read<int>()],
                    vertexes[reader.read<int>()]
                );
                break;
            case obj::Comment:
            case obj::Group:
            case obj::Line:
            case obj::VertexTexture:
            case obj::VertexNormal:
                reader.skip_line();
                break;
            case obj::Eof:
                has_data = false;
                break;
        }
    }

    return TriangularMesh(std::move(triangles));
}

