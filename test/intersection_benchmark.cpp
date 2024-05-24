#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <time.h>

#include "rmilib/ray.hpp"
#include "rmilib/parallel_algos.hpp"
#include "rmilib/kd_tree.hpp"
#include "rmilib/reader.hpp"


class SampleGenerator {
public:
    SampleGenerator(int seed): seed(seed), engine(seed), dist(-1, 1) {
    }

    Ray<double> next_ray() {
        return Ray(
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


TEST_CASE("Mesh intersection", "[benchmark][ray][mesh]") {
    constexpr int threads_count = 2;
    constexpr int depth = 16;
    std::string filename = "../../data/urn.stl";

    SampleGenerator generator(354238);
    TriangularMesh mesh = read_raw_triangular_mesh<double, size_t>(filename);
    auto kdtree = KDTree<TriangularMesh>::for_mesh(mesh.begin(), mesh.end(), depth);


    generator.reset();
    BENCHMARK_ADVANCED(concat("Linear search ", mesh.size()))(auto meter) {
        Ray ray = generator.next_ray();
        meter.measure([&ray, &mesh] { return ray.intersects(mesh); });
    };


    generator.reset();
    BENCHMARK_ADVANCED(concat("Sync K-D Tree search ", mesh.size()))(auto meter) {
        Ray ray = generator.next_ray();
        meter.measure([&ray, &kdtree] { return ray.intersects(kdtree); });
    };


#ifdef RMI_INCLUDE_OMP
    generator.reset();
    BENCHMARK_ADVANCED(concat(
        "OMP (", threads_count, " threads) K-D Tree search ", mesh.size()
    ))(auto meter) {
        Ray ray = generator.next_ray();
        meter.measure([&ray, &kdtree, threads_count] {
            return parallel_intersects_omp(ray, kdtree, threads_count);
        });
    };
#endif

#ifdef RMI_INCLUDE_POOL
    generator.reset();
    BENCHMARK_ADVANCED(concat(
        "Thread pool (", threads_count, " threads) K-D Tree search ", mesh.size()
    ))(auto meter) {
        Ray ray = generator.next_ray();
        meter.measure([&ray, &kdtree, threads_count] {
            return parallel_intersects_pool(ray, kdtree, threads_count);
        });
    };
#endif
}
