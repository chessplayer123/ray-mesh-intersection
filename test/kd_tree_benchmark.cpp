#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <fstream>
#include "rmilib/kd_tree.hpp"
#include "rmilib/reader.hpp"


TEST_CASE("K-D Tree Building", "[benchmark][kdtree]") {
    const std::string filename = "../../data/bunny.ply";
    auto mesh = std::make_shared<const TriangularMesh>(read_triangular_mesh(filename));

    BENCHMARK("K-D Tree Building Benchmark") {
        return KDTree::for_mesh(mesh);
    };
}
