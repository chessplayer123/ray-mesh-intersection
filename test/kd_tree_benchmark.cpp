#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <fstream>
#include "rmilib/tree.hpp"
#include "rmilib/reader.hpp"


TEST_CASE("K-D Tree Building", "[benchmark][kdtree]") {
    const std::string filename = "../../data/bunny.ply";
    TriangularMesh mesh = read_raw_triangular_mesh<double, size_t>(filename);

    BENCHMARK("K-D Tree Building Benchmark") {
        return KDTree<TriangularMesh>::for_mesh(mesh.begin(), mesh.end());
    };
}
