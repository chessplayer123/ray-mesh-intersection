#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <sstream>

#include "rmilib/reader.hpp"
#include "rmilib/raw_mesh.hpp"
#include "rmilib/rmi.hpp"


template<typename... Args>
std::string concat(const Args&... args) {
    std::stringstream stream;
    (stream << ... << args);
    return stream.str();
}


using Splitter = rmi::SAHSplitter<TriangularMesh>;
constexpr const char* MESH_FILEPATH = "../../data/Fantasy_Castle.stl";

TEST_CASE("Mesh Setup", "[benchmark][mesh]") {
    TriangularMesh mesh = read_raw_triangular_mesh<double, size_t>(MESH_FILEPATH);
    BENCHMARK(concat("Mesh Setup Benchmark (", mesh.size(), " polygons)")) {
        mesh.setup(mesh.size());
    };
}

TEST_CASE("KD-Tree Building", "[benchmark][kdtree]") {
    TriangularMesh mesh = read_raw_triangular_mesh<double, size_t>(MESH_FILEPATH);

    BENCHMARK(concat("KD-Tree Build Benchmark (", mesh.size(), " polygons)")) {
        return rmi::KDTree<TriangularMesh>::for_mesh(mesh, Splitter());
    };

#ifdef RMI_INCLUDE_OMP
    for (int threads_count = 2; threads_count <= 8; threads_count *= 2) {
        BENCHMARK(concat("KD-Tree Parallel <", threads_count, "> Build Benchmark (", mesh.size(), " polygons)")) {
            return rmi::KDTree<TriangularMesh>::for_mesh(mesh, threads_count, Splitter());
        };
    }
#endif
}
