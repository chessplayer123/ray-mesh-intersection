#include "mesh_readers/reader.hpp"
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
    std::vector<Triangle> triangles;

    for(;;) {
        std::string token = reader.read<std::string>();
        if (token == "endsolid") {
            break;
        } else if (token != "facet") {
            throw "Unexpected token '" + token + "'.";
        }

        reader.expect_word("normal");
        Vector normal(
            reader.read<double>(),
            reader.read<double>(),
            reader.read<double>()
        );

        reader.expect_line("outer loop");
        std::array<Vector, 3> vertexes;
        for (int i = 0; i < 3; ++i) {
            reader.expect_word("vertex");
            vertexes[i] = Vector(
                reader.read<double>(),
                reader.read<double>(),
                reader.read<double>()
            );
        }
        reader.expect_word("endloop");
        reader.expect_word("endfacet");

        triangles.emplace_back(vertexes[0], vertexes[1], vertexes[2], normal);
    }

    return TriangularMesh(std::move(triangles));
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
    std::vector<Triangle> triangles;
    triangles.reserve(triangles_count);

    for (uint32_t i = 0; i < triangles_count; ++i) {
        std::array<Vector, 4> vertexes;
        for (int j = 0; j < 4; ++j) {
            vertexes[j] = Vector(
                static_cast<double>(reader.read_as_bytes<float>()),
                static_cast<double>(reader.read_as_bytes<float>()),
                static_cast<double>(reader.read_as_bytes<float>())
            );
        }
        triangles.emplace_back(vertexes[1], vertexes[2], vertexes[3], vertexes[0]);
        reader.skip_bytes(2);
    }

    return TriangularMesh(std::move(triangles));
}


template<>
TriangularMesh MeshReader::read_triangular_mesh<MeshReader::Stl>(std::istream& stream) {
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
