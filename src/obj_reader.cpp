#include "reader.hpp"
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
TriangularMesh read_triangular_mesh<DataFormat::Obj>(std::istream& stream) {
    MeshReader reader(stream);

    std::vector<double> vertexes;
    std::vector<size_t> indices;

    bool has_data = true;
    while (has_data) {
        auto kw = reader.read<obj::Keyword>();
        switch (kw) {
            case obj::Vertex: {
                double x = reader.read<double>();
                double y = reader.read<double>();
                double z = reader.read<double>();
                vertexes.push_back(x);
                vertexes.push_back(y);
                vertexes.push_back(z);
            } break;
            case obj::Face:
                indices.push_back(reader.read<size_t>() - 1);
                indices.push_back(reader.read<size_t>() - 1);
                indices.push_back(reader.read<size_t>() - 1);
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

    return TriangularMesh(std::move(vertexes), std::move(indices));
}

