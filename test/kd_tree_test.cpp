#include <catch2/catch.hpp>
#include <fstream>
#include "trees/kd_tree.hpp"
#include "mesh_readers/reader.hpp"


TEST_CASE("KD-Tree") {
    const std::string file_path = "../../data/bunny.ply";
    std::ifstream stream;
    stream.open(file_path);

    auto mesh = MeshReader::read_triangular_mesh<MeshReader::Ply>(stream);

    KDTree tree = KDTree(mesh);
}
