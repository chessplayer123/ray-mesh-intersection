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
TriangularMesh parse_ascii_mesh(MeshReader& reader) {
    std::vector<double> coords;
    std::vector<size_t> indices;
    std::vector<double> normals;

    for(size_t counter = 0;; ++counter) {
        std::string token = reader.read<std::string>();
        if (token == "endsolid") {
            break;
        } else if (token != "facet") {
            throw "Unexpected token '" + token + "'.";
        }

        reader.expect_word("normal");
        normals.push_back(reader.read<double>());
        normals.push_back(reader.read<double>());
        normals.push_back(reader.read<double>());

        reader.expect_line("outer loop");
        for (int i = 0; i < 3; ++i) {
            reader.expect_word("vertex");
            coords.push_back(reader.read<double>());
            coords.push_back(reader.read<double>());
            coords.push_back(reader.read<double>());
            indices.push_back(counter + i);
        }
        reader.expect_word("endloop");
        reader.expect_word("endfacet");
    }

    return TriangularMesh(std::move(coords), std::move(indices));
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
TriangularMesh parse_binary_mesh(MeshReader& reader) {
    auto triangles_count = reader.read_as_bytes<uint32_t>();
    std::vector<double> coords;
    std::vector<size_t> indices;
    std::vector<double> normals;

    for (uint32_t i = 0; i < triangles_count; ++i) {
        normals.push_back(static_cast<double>(reader.read_as_bytes<float>()));
        normals.push_back(static_cast<double>(reader.read_as_bytes<float>()));
        normals.push_back(static_cast<double>(reader.read_as_bytes<float>()));
        for (int j = 0; j < 3; ++j) {
            coords.push_back(static_cast<double>(reader.read_as_bytes<float>()));
            coords.push_back(static_cast<double>(reader.read_as_bytes<float>()));
            coords.push_back(static_cast<double>(reader.read_as_bytes<float>()));
            indices.push_back(3 * i + j);
        }
        reader.skip_bytes(2);
    }

    return TriangularMesh(std::move(coords), std::move(indices));
}


template<>
TriangularMesh read_triangular_mesh<DataFormat::Stl>(std::istream& stream) {
    MeshReader reader(stream);

    std::string magic_bytes = reader.read_as_bytes(5);
    if (magic_bytes == "solid") { // ascii
        reader.skip_word();
        return parse_ascii_mesh(reader);
    } else { // binary
        reader.skip_bytes(75);
        return parse_binary_mesh(reader);
    }
}
