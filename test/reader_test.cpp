#include <catch2/catch.hpp>
#include "mesh_readers/reader.hpp"

TEST_CASE("Ply reader function test") {
    TriangularMesh mesh = MeshReader::read_triangular_mesh<MeshReader::Ply>("../../data/bunny.ply");
}
