#include <catch2/catch.hpp>
#include "mesh_readers/reader.hpp"

TEST_CASE("Ply reader", "[reader]") {
    auto mesh = MeshReader::read_triangular_mesh<MeshReader::Ply>("../../data/bunny.ply");
}

TEST_CASE("Binary stl reader", "[reader]") {
    auto mesh = MeshReader::read_triangular_mesh<MeshReader::Stl>("../../data/Utah_teapot_(solid).stl");
}

TEST_CASE("Ascii stl reader", "[reader]") {
    auto mesh = MeshReader::read_triangular_mesh<MeshReader::Stl>("../../data/Sphericon.stl");
}
