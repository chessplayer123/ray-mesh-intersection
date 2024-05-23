#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <fstream>
#include "rmilib/kd_tree.hpp"
#include "rmilib/reader.hpp"


TEST_CASE("K-D Tree Building", "[benchmark][kdtree]") {
    const std::string filename = "../../data/bunny.ply";
    TriangularMesh mesh = read_triangular_mesh(filename);

    BENCHMARK("K-D Tree Building Benchmark") {
        return KDTree<TriangularMesh>::for_mesh(mesh.begin(), mesh.end());
    };
}
