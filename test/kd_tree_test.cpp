#include <catch2/catch.hpp>
#include <fstream>
#include "trees/kd_tree.hpp"
#include "readers/reader.hpp"


TEST_CASE("KD-Tree", "[kdtree]") {
    const std::string file_path = "../../data/bunny.ply";
    std::ifstream stream;
    stream.open(file_path);

    auto mesh = read_triangular_mesh<DataFormat::Ply>(stream);

    KDTree tree = KDTree(mesh);
}
