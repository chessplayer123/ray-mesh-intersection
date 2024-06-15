#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <time.h>

#include "rmilib/raw_mesh.hpp"
#include "rmilib/reader.hpp"
#include "rmilib/rmi.hpp"
#include "rmilib/rmi_parallel.hpp" 


const std::string filename = "../../data/urn.stl";


class SampleGenerator {
public:
    SampleGenerator(int seed): seed(seed), engine(seed), dist(-1, 1) {
    }

    rmi::Ray<double> next_ray() {
        return rmi::Ray<double>(
            rmi::Vector3d(dist(engine), dist(engine), dist(engine)),
            rmi::Vector3d(dist(engine), dist(engine), dist(engine))
        );
    }

    void reset() {
        engine.seed(seed);
    }
private:
    int seed;
    std::default_random_engine engine;
    std::uniform_real_distribution<> dist;
} generator(354238);


template<typename... Args>
std::string concat(const Args&... args) {
    std::stringstream stream;
    (stream << ... << args);
    return stream.str();
}


template<int N>
void seq_tree_benchmark(rmi::Tree<TriangularMesh, N>& tree) {
    generator.reset();
    BENCHMARK_ADVANCED(concat("Sync Tree<", N, "> search "))(auto meter) {
        auto ray = generator.next_ray();
        meter.measure([&ray, &tree] { return ray.intersects(tree); });
    };
}


template<int N>
void omp_tree_benchmark(rmi::Tree<TriangularMesh, N>& tree, int threads_count) {
#ifdef RMI_INCLUDE_OMP
    generator.reset();
    BENCHMARK_ADVANCED(concat(
        "OMP (", threads_count, " threads) Tree<", N, "> search "
    ))(auto meter) {
        auto ray = generator.next_ray();
        meter.measure([&ray, &tree, threads_count] {
            return rmi::parallel::omp_intersects(ray, tree, threads_count);
        });
    };
#endif
}


template<int N>
void pool_tree_benchmark(
    rmi::Tree<TriangularMesh, N>& tree,
    int threads_count
) {
#ifdef RMI_INCLUDE_POOL 
    generator.reset();
    BENCHMARK_ADVANCED(concat(
        "Thread pool (", threads_count, " threads) Tree<", N, "> search "
    ))(auto meter) {
        auto ray = generator.next_ray();
        meter.measure([&ray, &tree, threads_count] {
            return rmi::parallel::pool_intersects(ray, tree, threads_count);
        });
    };
#endif
}


TEST_CASE("Mesh intersection", "[benchmark][ray][mesh]") {
    TriangularMesh mesh = read_raw_triangular_mesh<double, size_t>(filename);

    generator.reset();
    BENCHMARK_ADVANCED(concat("Linear search ", mesh.size()))(auto meter) {
        auto ray = generator.next_ray();
        meter.measure([&ray, &mesh] { return ray.intersects(mesh); });
    };
}


TEST_CASE("KD-Tree intersection", "[benchmark][ray][kdtree]") {
    TriangularMesh mesh = read_raw_triangular_mesh<double, size_t>(filename);
    auto tree = rmi::KDTree<TriangularMesh>::for_mesh(mesh.begin(), mesh.end());

    seq_tree_benchmark(tree);

    for (int threads_count = 2; threads_count <= 8; threads_count *= 2)
        omp_tree_benchmark(tree, threads_count);

    for (int threads_count = 2; threads_count <= 8; threads_count *= 2)
        pool_tree_benchmark(tree, threads_count);
}


TEST_CASE("Quadree intersection", "[benchmark][ray][quadtree]") {
    TriangularMesh mesh = read_raw_triangular_mesh<double, size_t>(filename);
    auto tree = rmi::Quadtree<TriangularMesh>::for_mesh(mesh.begin(), mesh.end());

    seq_tree_benchmark(tree);

    for (int threads_count = 2; threads_count <= 8; threads_count *= 2)
        omp_tree_benchmark(tree, threads_count);

    for (int threads_count = 2; threads_count <= 8; threads_count *= 2)
        pool_tree_benchmark(tree, threads_count);
}


TEST_CASE("Octree intersection", "[benchmark][ray][octree]") {
    TriangularMesh mesh = read_raw_triangular_mesh<double, size_t>(filename);
    auto tree = rmi::Octree<TriangularMesh>::for_mesh(mesh.begin(), mesh.end());

    seq_tree_benchmark(tree);

    for (int threads_count = 2; threads_count <= 8; threads_count *= 2)
        omp_tree_benchmark(tree, threads_count);

    for (int threads_count = 2; threads_count <= 8; threads_count *= 2)
        pool_tree_benchmark(tree, threads_count);
}
