#include <catch2/catch.hpp>
#include <fstream>
#include "rmilib/reader.hpp"

TEST_CASE("Ply reader", "[reader]") {
    const std::string file_path = "../../data/bunny.ply";
    std::ifstream stream;
    stream.open(file_path);

    auto mesh = read_triangular_mesh<DataFormat::Ply>(stream);
}

TEST_CASE("Binary stl reader", "[reader]") {
    const std::string file_path = "../../data/Utah_teapot_(solid).stl";
    std::ifstream stream;
    stream.open(file_path);

    auto mesh = read_triangular_mesh<DataFormat::Stl>(stream);
}

TEST_CASE("Ascii stl reader", "[reader]") {
    const std::string file_path = "../../data/Sphericon.stl";
    std::ifstream stream;
    stream.open(file_path);

    auto mesh = read_triangular_mesh<DataFormat::Stl>(stream);
}

TEST_CASE("Obj reader", "[reader]") {
    const std::string file_path = "../../data/gourd.obj";
    std::ifstream stream;
    stream.open(file_path);

    auto mesh = read_triangular_mesh<DataFormat::Obj>(stream);
}
