#include "reader.hpp"
#include <fstream>
#include <cstdint>

/*
solid name
facet normal ni nj nk
    outer loop
        vertex v1x v1y v1z
        vertex v2x v2y v2z
        vertex v3x v3y v3z
    endloop
endfacet
...
endsolid name
 */
template<typename float_t, typename index_t>
RawMesh<float_t, index_t> parse_ascii_mesh(MeshReader& reader) {
    std::vector<float_t> coords;
    std::vector<index_t> indices;

    for(index_t counter = 0;; ++counter) {
        std::string token = reader.read<std::string>();
        if (token == "endsolid") {
            break;
        } else if (token != "facet") {
            throw "Unexpected token '" + token + "'.";
        }

        reader.expect_word("normal");
        reader.read<float_t>();
        reader.read<float_t>();
        reader.read<float_t>();

        reader.expect_line("outer loop");
        for (index_t i = 0; i < 3; ++i) {
            reader.expect_word("vertex");
            coords.push_back(reader.read<float_t>());
            coords.push_back(reader.read<float_t>());
            coords.push_back(reader.read<float_t>());
            indices.push_back(counter + i);
        }
        reader.expect_word("endloop");
        reader.expect_word("endfacet");
    }

    return RawMesh<float_t, index_t>(std::move(coords), std::move(indices));
}


/*
UINT8[80]    – Header                 -     80 bytes
UINT32       – Number of triangles    -      4 bytes
foreach triangle                      - 50 bytes:
    REAL32[3] – Normal vector             - 12 bytes
    REAL32[3] – Vertex 1                  - 12 bytes
    REAL32[3] – Vertex 2                  - 12 bytes
    REAL32[3] – Vertex 3                  - 12 bytes
    UINT16    – Attribute byte count      -  2 bytes
end
 */
template<typename float_t, typename index_t>
RawMesh<float_t, index_t> parse_binary_mesh(MeshReader& reader) {
    auto triangles_count = reader.read_as_bytes<uint32_t>();
    std::vector<float_t> coords;
    std::vector<index_t> indices;

    for (uint32_t i = 0; i < triangles_count; ++i) {
        // skip normal
        reader.read_as_bytes<float>();
        reader.read_as_bytes<float>();
        reader.read_as_bytes<float>();
        for (int j = 0; j < 3; ++j) {
            coords.push_back(static_cast<float_t>(reader.read_as_bytes<float>()));
            coords.push_back(static_cast<float_t>(reader.read_as_bytes<float>()));
            coords.push_back(static_cast<float_t>(reader.read_as_bytes<float>()));
            indices.push_back(3 * i + j);
        }
        reader.skip_bytes(2);
    }

    return RawMesh(std::move(coords), std::move(indices));
}


template<typename float_t, typename index_t>
RawMesh<float_t, index_t> read_raw_triangular_mesh_stl(std::istream& stream) {
    MeshReader reader(stream);

    std::string magic_bytes = reader.read_as_bytes(5);
    if (magic_bytes == "solid") { // ascii
        reader.skip_word();
        return parse_ascii_mesh<float_t, index_t>(reader);
    } else { // binary
        reader.skip_bytes(75);
        return parse_binary_mesh<float_t, index_t>(reader);
    }
}

template TriangularMesh read_raw_triangular_mesh_stl<double, size_t>(std::istream& stream);
template WebGLMesh read_raw_triangular_mesh_stl<float, unsigned short>(std::istream& stream);
