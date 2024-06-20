#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <sstream>

#include "rmilib/reader.hpp"
#include "rmilib/raw_mesh.hpp"
#include "rmilib/rmi.hpp"
#include "rmilib/rmi_parallel.hpp"


template<typename... Args>
std::string concat(const Args&... args) {
    std::stringstream stream;
    (stream << ... << args);
    return stream.str();
}


template<int N>
using Splitter = rmi::SAHSplitter<TriangularMesh>;


TEST_CASE("KD-Tree Building", "[benchmark][kdtree]") {
    constexpr const char* MESH_FILEPATH = "../../data/Fantasy_Castle.stl";
    TriangularMesh mesh = read_raw_triangular_mesh<double, size_t>(MESH_FILEPATH);

    BENCHMARK(concat("KD-Tree Build Benchmark (", mesh.size(), " polygons)")) {
        return rmi::KDTree<TriangularMesh>::for_mesh(mesh.begin(), mesh.end(), Splitter<1>());
    };

#ifdef RMI_INCLUDE_OMP
    for (int threads_count = 2; threads_count <= 8; threads_count *= 2) {
        BENCHMARK(concat("KD-Tree Parallel <", threads_count, "> Build Benchmark (", mesh.size(), " polygons)")) {
            return rmi::parallel::omp_build<TriangularMesh, 1>(mesh.begin(), mesh.end(), threads_count, Splitter<1>());
        };
    }
#endif
}
