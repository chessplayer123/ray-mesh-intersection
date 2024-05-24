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

template<typename float_t, typename index_t>
RawMesh<float_t, index_t> read_raw_triangular_mesh_obj(std::istream& stream) {
    MeshReader reader(stream);

    std::vector<float_t> vertices;
    std::vector<index_t> indices;

    bool has_data = true;
    while (has_data) {
        auto kw = reader.read<obj::Keyword>();
        switch (kw) {
            case obj::Vertex: {
                float_t x = reader.read<float_t>();
                float_t y = reader.read<float_t>();
                float_t z = reader.read<float_t>();
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            } break;
            case obj::Face:
                indices.push_back(reader.read<index_t>() - 1);
                indices.push_back(reader.read<index_t>() - 1);
                indices.push_back(reader.read<index_t>() - 1);
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

    return RawMesh<float_t, index_t>(std::move(vertices), std::move(indices));
}

template TriangularMesh read_raw_triangular_mesh_obj<double, size_t>(std::istream& stream);
template WebGLMesh read_raw_triangular_mesh_obj<float, unsigned short>(std::istream& stream);
