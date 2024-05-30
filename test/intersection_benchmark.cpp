#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <time.h>

#include "rmilib/ray.hpp"
#include "rmilib/parallel_algos.hpp" 
#include "rmilib/tree.hpp"
#include "rmilib/reader.hpp"


class SampleGenerator {
public:
    SampleGenerator(int seed): seed(seed), engine(seed), dist(-1, 1) {
    }

    Ray<double> next_ray() {
        return Ray<double>(
            Vector3d(dist(engine), dist(engine), dist(engine)),
            Vector3d(dist(engine), dist(engine), dist(engine))
        );
    }

    void reset() {
        engine.seed(seed);
    }
private:
    int seed;
    std::default_random_engine engine;
    std::uniform_real_distribution<> dist;
};


template<typename... Args>
std::string concat(const Args&... args) {
    std::stringstream stream;
    (stream << ... << args);
    return stream.str();
}


template<int N>
void sync_tree_benchmark(
    Tree<TriangularMesh, N>& tree,
    SampleGenerator& generator
) {
    generator.reset();
    BENCHMARK_ADVANCED(concat("Sync Tree<", N, "> search "))(auto meter) {
        auto ray = generator.next_ray();
        meter.measure([&ray, &tree] { return ray.intersects(tree); });
    };
}


template<int N>
void omp_tree_benchmark(
    Tree<TriangularMesh, N>& tree,
    SampleGenerator& generator,
    int threads_count
) {
    #ifdef RMI_INCLUDE_OMP 
    generator.reset(); 
    BENCHMARK_ADVANCED(concat( 
        "OMP (", threads_count, " threads) Tree<", N, "> search "
    ))(auto meter) { 
        auto ray = generator.next_ray(); 
        meter.measure([&ray, &tree, threads_count] { 
            return parallel_intersects_omp(ray, tree, threads_count); 
        }); 
    }; 
    #endif 
}


template<int N>
void pool_tree_benchmark(
    Tree<TriangularMesh, N>& tree,
    SampleGenerator& generator,
    int threads_count
) {
    #ifdef RMI_INCLUDE_POOL 
    generator.reset(); 
    BENCHMARK_ADVANCED(concat( 
        "Thread pool (", threads_count, " threads) Tree<", N, "> search "
    ))(auto meter) { 
        auto ray = generator.next_ray(); 
        meter.measure([&ray, &tree, threads_count] { 
            return parallel_intersects_pool(ray, tree, threads_count); 
        }); 
    }; 
    #endif 
}


TEST_CASE("Mesh intersection", "[benchmark][ray][mesh]") {
    std::string filename = "../../data/urn.stl";

    SampleGenerator generator(354238);
    TriangularMesh mesh = read_raw_triangular_mesh<double, size_t>(filename);

    generator.reset();
    BENCHMARK_ADVANCED(concat("Linear search ", mesh.size()))(auto meter) {
        auto ray = generator.next_ray();
        meter.measure([&ray, &mesh] { return ray.intersects(mesh); });
    };


    constexpr int threads_count = 2;
    constexpr int depth = 16;
    auto kdtree = KDTree<TriangularMesh>::for_mesh(mesh.begin(), mesh.end(), depth);
    auto quadtree = Quadtree<TriangularMesh>::for_mesh(mesh.begin(), mesh.end(), depth);
    auto octree = Octree<TriangularMesh>::for_mesh(mesh.begin(), mesh.end(), depth);

    sync_tree_benchmark(kdtree, generator);
    sync_tree_benchmark(quadtree, generator);
    sync_tree_benchmark(octree, generator);

    omp_tree_benchmark(kdtree, generator, threads_count);
    omp_tree_benchmark(quadtree, generator, threads_count);
    omp_tree_benchmark(octree, generator, threads_count);

    pool_tree_benchmark(kdtree, generator, threads_count);
    pool_tree_benchmark(quadtree, generator, threads_count);
    pool_tree_benchmark(octree, generator, threads_count);
}
