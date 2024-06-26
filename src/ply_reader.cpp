#include "reader.hpp"

#include <fstream>
#include <unordered_map>
#include <vector>

namespace ply {

namespace keyword {
    enum Type {
        Format,
        Comment,
        Element,
        Property,
        Face,
        Vertex,
        List,
        EndHeader,
    };
}


struct Header {
    int vertexes_count;
    std::vector<std::pair<std::string, std::string>> vertex_properties;

    int faces_count;
    std::vector<std::pair<std::string, std::string>> face_properties;
};

} // namespace ply


template<>
ply::keyword::Type MeshReader::read<ply::keyword::Type>() {
    static std::unordered_map<std::string, ply::keyword::Type> keywords = {
        {"format",     ply::keyword::Format},
        {"comment",    ply::keyword::Comment},
        {"element",    ply::keyword::Element},
        {"property",   ply::keyword::Property},
        {"list",       ply::keyword::List},
        {"end_header", ply::keyword::EndHeader}
    };
    std::string str = read<std::string>();

    if (keywords.find(str) == keywords.end()) {
        throw "Unknown token '" + str + "'";
    }

    return keywords[str];
}

enum Section {
    VertexList,
    FaceList
};

ply::Header parse_header(MeshReader& reader) {
    using Type = ply::keyword::Type;
    reader.expect_line("ply");

    Section section = Section::VertexList;
    ply::Header header;
    for (Type kw = reader.read<Type>(); kw != Type::EndHeader; kw = reader.read<Type>()) {
        switch (kw) {
        case ply::keyword::Format:
            reader.expect_line("ascii 1.0");
            break;

        case ply::keyword::Comment:
            reader.skip_line();
            break;

        case ply::keyword::Element: {
            const std::string type = reader.read<std::string>();
            const int count = reader.read<double>();

            if (section == Section::VertexList && type == "vertex") {
                header.vertexes_count = count;
            } else if (section == Section::VertexList && type == "face") {
                header.faces_count = count;
                section = Section::FaceList;
            } else {
                throw "Unexpected type '" + type + "'";
            }
        } break;

        case ply::keyword::Property: {
            const std::string type = reader.read<std::string>();
            if (type == "list") {
                reader.skip_word();
                reader.skip_word();
            }
            const std::string name = reader.read<std::string>();
            switch (section) {
                case VertexList:
                    header.vertex_properties.emplace_back(type, name);
                    break;
                case FaceList:
                    header.face_properties.emplace_back(type, name);
                    break;
            }
        } break;

        default:
            throw "Unexpected token " + std::to_string(kw);
        }
    }

    return header;
}


template<typename float_t, typename index_t>
RawMesh<float_t, index_t> parse_elements(
    MeshReader& reader,
    const ply::Header& header
) {
    std::vector<float_t> points;
    std::vector<index_t> indices;

    for (int i = 0; i < header.vertexes_count; ++i) {
        float_t x = 0, y = 0, z = 0;

        for (auto &[type, name] : header.vertex_properties) {
            float_t value = reader.read<float_t>();
            if      (name == "x") x = value;
            else if (name == "y") y = value;
            else if (name == "z") z = value;
        }
        points.push_back(x);
        points.push_back(y);
        points.push_back(z);
    }

    for (int i = 0; i < header.faces_count; ++i) {
        for (auto &[type, name] : header.face_properties) {
            if (type != "list" || (name != "vertex_index" && name != "vertex_indices")) {
                reader.skip_line();
            }

            int count = reader.read<int>();
            if (count != 3) {
                throw "Supports only triangular meshes.";
            }
            indices.push_back(reader.read<index_t>());
            indices.push_back(reader.read<index_t>());
            indices.push_back(reader.read<index_t>());
        }
    }

    return RawMesh(std::move(points), std::move(indices));
}


template<typename float_t, typename index_t>
RawMesh<float_t, index_t> read_raw_triangular_mesh_ply(std::istream& stream) {
    MeshReader reader(stream);

    const ply::Header header = parse_header(reader);
    return parse_elements<float_t, index_t>(reader, header);
}

template TriangularMesh read_raw_triangular_mesh_ply<double, size_t>(std::istream& stream);
template WebGLMesh read_raw_triangular_mesh_ply<float, unsigned int>(std::istream& stream);
