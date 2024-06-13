#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <fstream>

#include "rmilib/reader.hpp"
#include "rmilib/raw_mesh.hpp"


TEST_CASE("K-D Tree Building", "[benchmark][kdtree]") {
    const std::string filename = "../../data/bunny.ply";
    TriangularMesh mesh = read_raw_triangular_mesh<double, size_t>(filename);

    BENCHMARK("K-D Tree Building Benchmark") {
        return rmi::KDTree<TriangularMesh>::for_mesh(mesh.begin(), mesh.end());
    };
}
